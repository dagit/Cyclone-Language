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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
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
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_ResetRegion_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
_tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
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
Cyc_Tcutil_snd_tqt(struct _tuple4*);unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
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
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
unsigned char _tmp3F[14]="_reset_region";static struct _tagged_arr Cyc_Toc__reset_region_str={
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
struct Cyc_Absyn_Exp*e){void*_tmp44=(void*)e->r;void*_tmp45;unsigned char _tmp46;
void*_tmp47;short _tmp48;void*_tmp49;int _tmp4A;void*_tmp4B;long long _tmp4C;void*
_tmp4D;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*
_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_List_List*
_tmp53;_LL1: if(*((int*)_tmp44)!= 0)goto _LL3;_tmp45=(void*)((struct Cyc_Absyn_Const_e_struct*)
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
ap))Cyc_Toc_toc_impos)(_tag_arr("is_nullable",sizeof(unsigned char),12),_tag_arr(
_tmp57,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
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
_tag_arr("_tuple%d",sizeof(unsigned char),9),_tag_arr(_tmp6D,sizeof(void*),1));}});
_tmp6C;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp60=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp60=({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=({
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
_tmp67));_tmp67->hd=({struct _tuple6*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=
x;_tmp68->f2=ts;_tmp68;});_tmp67->tl=Cyc_Toc_tuple_types;_tmp67;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp6F=_cycalloc(sizeof(*
_tmp6F));_tmp6F->f1=(void*)0;_tmp6F->f2=({struct _tagged_arr*_tmp70=_cycalloc(
sizeof(*_tmp70));_tmp70[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp72;_tmp72.tag=1;_tmp72.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp71[1]={& _tmp72};Cyc_Std_aprintf(_tag_arr("_tmp%X",sizeof(unsigned char),7),
_tag_arr(_tmp71,sizeof(void*),1));}});_tmp70;});_tmp6F;});}static struct
_tagged_arr*Cyc_Toc_fresh_label(){return({struct _tagged_arr*_tmp73=_cycalloc(
sizeof(*_tmp73));_tmp73[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp75;_tmp75.tag=1;_tmp75.f1=(unsigned int)Cyc_Toc_fresh_label_counter ++;{void*
_tmp74[1]={& _tmp75};Cyc_Std_aprintf(_tag_arr("_LL%X",sizeof(unsigned char),6),
_tag_arr(_tmp74,sizeof(void*),1));}});_tmp73;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(
struct Cyc_Absyn_Tuniondecl*td,struct _tuple1*name,int carries_value){int ans=0;
struct Cyc_List_List*_tmp76=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*)_check_null(_tmp76))->hd)->name)!= 0){if((carries_value?((
struct Cyc_Absyn_Tunionfield*)_tmp76->hd)->typs != 0: 0)?1:(!carries_value?((struct
Cyc_Absyn_Tunionfield*)_tmp76->hd)->typs == 0: 0))++ ans;_tmp76=_tmp76->tl;}return
Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(struct
Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp77=(struct Cyc_List_List*)((
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
_tmp8C)->f2;if(_tmp8D != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 0)goto _LL2C;_tmp8E=((struct Cyc_Absyn_Evar_struct*)
_tmp8C)->f2;if(_tmp8E == 0)goto _LL2C;_tmp8F=*_tmp8E;_tmp90=(void*)_tmp8F.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmp90);_LL2C: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 1)
goto _LL2E;_tmp91=((struct Cyc_Absyn_VarType_struct*)_tmp8C)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmp91)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 2)goto _LL30;_tmp92=((struct Cyc_Absyn_TunionType_struct*)
_tmp8C)->f1;_tmp93=(void*)_tmp92.tunion_info;if(*((int*)_tmp93)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp8C <= (void*)3?1:*((int*)
_tmp8C)!= 2)goto _LL32;_LL31:({void*_tmpB2[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionType",sizeof(
unsigned char),22),_tag_arr(_tmpB2,sizeof(void*),0));});_LL32: if(_tmp8C <= (void*)
3?1:*((int*)_tmp8C)!= 3)goto _LL34;_tmp94=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp8C)->f1;_tmp95=(void*)_tmp94.field_info;if(*((int*)_tmp95)!= 1)goto _LL34;
_tmp96=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp95)->f1;_tmp97=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp95)->f2;_LL33: if(_tmp97->typs == 0){if(_tmp96->is_xtunion)return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_typ,(void*)2,Cyc_Toc_mt_tq);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp97->name,_tag_arr("_struct",
sizeof(unsigned char),8)));}_LL34: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 3)
goto _LL36;_LL35:({void*_tmpB3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionFieldType",sizeof(
unsigned char),27),_tag_arr(_tmpB3,sizeof(void*),0));});_LL36: if(_tmp8C <= (void*)
3?1:*((int*)_tmp8C)!= 4)goto _LL38;_tmp98=((struct Cyc_Absyn_PointerType_struct*)
_tmp8C)->f1;_tmp99=(void*)_tmp98.elt_typ;_tmp9A=_tmp98.tq;_tmp9B=_tmp98.bounds;
_LL37: _tmp99=Cyc_Toc_typ_to_c_array(_tmp99);{void*_tmpB4=(void*)(Cyc_Absyn_compress_conref(
_tmp9B))->v;void*_tmpB5;_LL5F: if(_tmpB4 <= (void*)1?1:*((int*)_tmpB4)!= 0)goto
_LL61;_tmpB5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpB4)->f1;if((int)
_tmpB5 != 0)goto _LL61;_LL60: return Cyc_Toc_tagged_arr_typ;_LL61:;_LL62: return Cyc_Absyn_star_typ(
_tmp99,(void*)2,_tmp9A);_LL5E:;}_LL38: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 5)
goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp8C != 1)goto _LL3C;_LL3B: goto _LL3D;
_LL3C: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 6)goto _LL3E;_LL3D: return t;_LL3E:
if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 7)goto _LL40;_tmp9C=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp8C)->f1;_tmp9D=((struct Cyc_Absyn_ArrayType_struct*)_tmp8C)->f2;_tmp9E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp8C)->f3;_LL3F: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB7;_tmpB7.tag=7;_tmpB7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp9C);_tmpB7.f2=
_tmp9D;_tmpB7.f3=_tmp9E;_tmpB7;});_tmpB6;});_LL40: if(_tmp8C <= (void*)3?1:*((int*)
_tmp8C)!= 8)goto _LL42;_tmp9F=((struct Cyc_Absyn_FnType_struct*)_tmp8C)->f1;_tmpA0=(
void*)_tmp9F.ret_typ;_tmpA1=_tmp9F.args;_tmpA2=_tmp9F.c_varargs;_tmpA3=_tmp9F.cyc_varargs;
_tmpA4=_tmp9F.attributes;_LL41: {struct Cyc_List_List*_tmpB8=0;for(0;_tmpA4 != 0;
_tmpA4=_tmpA4->tl){void*_tmpB9=(void*)_tmpA4->hd;_LL64: if((int)_tmpB9 != 3)goto
_LL66;_LL65: goto _LL67;_LL66: if((int)_tmpB9 != 4)goto _LL68;_LL67: goto _LL69;_LL68:
if(_tmpB9 <= (void*)16?1:*((int*)_tmpB9)!= 3)goto _LL6A;_LL69: continue;_LL6A:;
_LL6B: _tmpB8=({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->hd=(
void*)((void*)_tmpA4->hd);_tmpBA->tl=_tmpB8;_tmpBA;});goto _LL63;_LL63:;}{struct
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
int*)_tmp8C)!= 14)goto _LL50;_LL4F: return Cyc_Absyn_uint_typ;_LL50: if(_tmp8C <= (
void*)3?1:*((int*)_tmp8C)!= 17)goto _LL52;_LL51: return Cyc_Absyn_sint_typ;_LL52:
if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 15)goto _LL54;_tmpB1=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8C)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 18)goto _LL56;_LL55:({
void*_tmpCC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed a type integer",sizeof(
unsigned char),54),_tag_arr(_tmpCC,sizeof(void*),0));});_LL56: if((int)_tmp8C != 2)
goto _LL58;_LL57:({void*_tmpCD[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof(unsigned char),55),_tag_arr(_tmpCD,sizeof(void*),0));});_LL58: if(_tmp8C <= (
void*)3?1:*((int*)_tmp8C)!= 19)goto _LL5A;_LL59:({void*_tmpCE[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof(unsigned char),54),_tag_arr(_tmpCE,sizeof(void*),0));});_LL5A: if(_tmp8C <= (
void*)3?1:*((int*)_tmp8C)!= 20)goto _LL5C;_LL5B:({void*_tmpCF[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof(unsigned char),52),_tag_arr(_tmpCF,sizeof(void*),0));});_LL5C: if(_tmp8C <= (
void*)3?1:*((int*)_tmp8C)!= 21)goto _LL25;_LL5D:({void*_tmpD0[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof(unsigned char),55),_tag_arr(_tmpD0,sizeof(void*),0));});_LL25:;}static
struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l){void*_tmpD1=t;void*_tmpD2;struct Cyc_Absyn_Tqual
_tmpD3;_LL6D: if(_tmpD1 <= (void*)3?1:*((int*)_tmpD1)!= 7)goto _LL6F;_tmpD2=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmpD1)->f1;_tmpD3=((struct Cyc_Absyn_ArrayType_struct*)
_tmpD1)->f2;_LL6E: return Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(_tmpD2,(void*)2,
_tmpD3),e,l);_LL6F:;_LL70: return Cyc_Absyn_cast_exp(t,e,l);_LL6C:;}static int Cyc_Toc_atomic_typ(
void*t){void*_tmpD4=Cyc_Tcutil_compress(t);void*_tmpD5;struct Cyc_Absyn_AggrInfo
_tmpD6;void*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_Absyn_TunionFieldInfo
_tmpD9;void*_tmpDA;struct Cyc_Absyn_Tuniondecl*_tmpDB;struct Cyc_Absyn_Tunionfield*
_tmpDC;struct Cyc_List_List*_tmpDD;_LL72: if((int)_tmpD4 != 0)goto _LL74;_LL73:
return 1;_LL74: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 1)goto _LL76;_LL75: return 0;
_LL76: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 5)goto _LL78;_LL77: goto _LL79;_LL78:
if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 12)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(
_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 13)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if((int)
_tmpD4 != 1)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)
!= 6)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 8)
goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 17)goto
_LL84;_LL83: goto _LL85;_LL84: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 14)goto
_LL86;_LL85: return 1;_LL86: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 7)goto _LL88;
_tmpD5=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD4)->f1;_LL87: return Cyc_Toc_atomic_typ(
_tmpD5);_LL88: if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 10)goto _LL8A;_tmpD6=((
struct Cyc_Absyn_AggrType_struct*)_tmpD4)->f1;_tmpD7=(void*)_tmpD6.aggr_info;
_LL89:{void*_tmpDE=_tmpD7;_LL99: if(*((int*)_tmpDE)!= 0)goto _LL9B;_LL9A: return 0;
_LL9B:;_LL9C: goto _LL98;_LL98:;}{struct Cyc_Absyn_Aggrdecl*_tmpDF=Cyc_Absyn_get_known_aggrdecl(
_tmpD7);if(_tmpDF->impl == 0)return 0;{struct Cyc_List_List*_tmpE0=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpDF->impl))->fields;for(0;_tmpE0 != 0;_tmpE0=_tmpE0->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpE0->hd)->type))return 0;}}return 1;}_LL8A:
if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 11)goto _LL8C;_tmpD8=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpD4)->f2;_LL8B: for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpD8->hd)->type))return 0;}return 1;_LL8C: if(
_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 3)goto _LL8E;_tmpD9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpD4)->f1;_tmpDA=(void*)_tmpD9.field_info;if(*((int*)_tmpDA)!= 1)goto _LL8E;
_tmpDB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpDA)->f2;_LL8D: _tmpDD=_tmpDC->typs;goto _LL8F;_LL8E: if(_tmpD4 <= (void*)3?1:*((
int*)_tmpD4)!= 9)goto _LL90;_tmpDD=((struct Cyc_Absyn_TupleType_struct*)_tmpD4)->f1;
_LL8F: for(0;_tmpDD != 0;_tmpDD=_tmpDD->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpDD->hd)).f2))return 0;}return 1;_LL90: if(_tmpD4 <= (void*)3?1:*((int*)
_tmpD4)!= 2)goto _LL92;_LL91: goto _LL93;_LL92: if(_tmpD4 <= (void*)3?1:*((int*)
_tmpD4)!= 4)goto _LL94;_LL93: goto _LL95;_LL94: if(_tmpD4 <= (void*)3?1:*((int*)
_tmpD4)!= 15)goto _LL96;_LL95: return 0;_LL96:;_LL97:({struct Cyc_Std_String_pa_struct
_tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpE1[1]={& _tmpE2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("atomic_typ:  bad type %s",sizeof(unsigned char),25),_tag_arr(_tmpE1,
sizeof(void*),1));}});_LL71:;}static int Cyc_Toc_is_void_star(void*t){void*_tmpE3=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE4;void*_tmpE5;_LL9E: if(_tmpE3
<= (void*)3?1:*((int*)_tmpE3)!= 4)goto _LLA0;_tmpE4=((struct Cyc_Absyn_PointerType_struct*)
_tmpE3)->f1;_tmpE5=(void*)_tmpE4.elt_typ;_LL9F: {void*_tmpE6=Cyc_Tcutil_compress(
_tmpE5);_LLA3: if((int)_tmpE6 != 0)goto _LLA5;_LLA4: return 1;_LLA5:;_LLA6: return 0;
_LLA2:;}_LLA0:;_LLA1: return 0;_LL9D:;}static int Cyc_Toc_is_poly_field(void*t,
struct _tagged_arr*f){void*_tmpE7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpE8;void*_tmpE9;struct Cyc_List_List*_tmpEA;_LLA8: if(_tmpE7 <= (void*)3?1:*((
int*)_tmpE7)!= 10)goto _LLAA;_tmpE8=((struct Cyc_Absyn_AggrType_struct*)_tmpE7)->f1;
_tmpE9=(void*)_tmpE8.aggr_info;_LLA9: {struct Cyc_Absyn_Aggrdecl*_tmpEB=Cyc_Absyn_get_known_aggrdecl(
_tmpE9);if(_tmpEB->impl == 0)({void*_tmpEC[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: type missing fields",
sizeof(unsigned char),35),_tag_arr(_tmpEC,sizeof(void*),0));});_tmpEA=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEB->impl))->fields;goto _LLAB;}_LLAA: if(
_tmpE7 <= (void*)3?1:*((int*)_tmpE7)!= 11)goto _LLAC;_tmpEA=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE7)->f2;_LLAB: {struct Cyc_Absyn_Aggrfield*_tmpED=Cyc_Absyn_lookup_field(
_tmpEA,f);if(_tmpED == 0)({struct Cyc_Std_String_pa_struct _tmpEF;_tmpEF.tag=0;
_tmpEF.f1=(struct _tagged_arr)*f;{void*_tmpEE[1]={& _tmpEF};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad field %s",
sizeof(unsigned char),28),_tag_arr(_tmpEE,sizeof(void*),1));}});return Cyc_Toc_is_void_star((
void*)_tmpED->type);}_LLAC:;_LLAD:({struct Cyc_Std_String_pa_struct _tmpF1;_tmpF1.tag=
0;_tmpF1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpF0[1]={&
_tmpF1};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad type %s",sizeof(unsigned char),27),_tag_arr(_tmpF0,
sizeof(void*),1));}});_LLA7:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*
e){void*_tmpF2=(void*)e->r;struct Cyc_Absyn_Exp*_tmpF3;struct _tagged_arr*_tmpF4;
struct Cyc_Absyn_Exp*_tmpF5;struct _tagged_arr*_tmpF6;_LLAF: if(*((int*)_tmpF2)!= 
21)goto _LLB1;_tmpF3=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpF2)->f1;_tmpF4=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpF2)->f2;_LLB0: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF3->topt))->v,_tmpF4);_LLB1: if(*((
int*)_tmpF2)!= 22)goto _LLB3;_tmpF5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpF2)->f1;
_tmpF6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpF2)->f2;_LLB2: {void*_tmpF7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF5->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF8;void*_tmpF9;_LLB6: if(_tmpF7 <= (void*)3?1:*((int*)_tmpF7)!= 4)goto _LLB8;
_tmpF8=((struct Cyc_Absyn_PointerType_struct*)_tmpF7)->f1;_tmpF9=(void*)_tmpF8.elt_typ;
_LLB7: return Cyc_Toc_is_poly_field(_tmpF9,_tmpF6);_LLB8:;_LLB9:({struct Cyc_Std_String_pa_struct
_tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF5->topt))->v);{void*_tmpFA[1]={& _tmpFB};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(unsigned char),30),_tag_arr(_tmpFA,sizeof(void*),1));}});_LLB5:;}_LLB3:;
_LLB4: return 0;_LLAE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmpFC=
_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=s;_tmpFC->tl=0;_tmpFC;}),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmpFD=_cycalloc(sizeof(*_tmpFD));
_tmpFD->hd=s;_tmpFD->tl=0;_tmpFD;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({
struct Cyc_Absyn_Exp*_tmpFE[2];_tmpFE[1]=s;_tmpFE[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFE,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmpFF[2];_tmpFF[1]=n;_tmpFF[0]=
s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFF,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({
struct Cyc_Absyn_Exp*_tmp100[2];_tmp100[1]=n;_tmp100[0]=s;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp100,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*
rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmp101[3];_tmp101[2]=n;_tmp101[1]=s;_tmp101[0]=rgn;((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp101,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*
_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->hd=e;_tmp102->tl=0;_tmp102;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp103=(void*)e->r;void*_tmp104;
_LLBB: if(*((int*)_tmp103)!= 0)goto _LLBD;_tmp104=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp103)->f1;if(_tmp104 <= (void*)1?1:*((int*)_tmp104)!= 5)goto _LLBD;_LLBC:
is_string=1;goto _LLBA;_LLBD:;_LLBE: goto _LLBA;_LLBA:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp108=_cycalloc(sizeof(*
_tmp108));_tmp108[0]=({struct Cyc_Absyn_ArrayType_struct _tmp109;_tmp109.tag=7;
_tmp109.f1=(void*)Cyc_Absyn_uchar_typ;_tmp109.f2=Cyc_Toc_mt_tq;_tmp109.f3=(
struct Cyc_Absyn_Exp*)sz;_tmp109;});_tmp108;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106[0]=({
struct Cyc_Absyn_Var_d_struct _tmp107;_tmp107.tag=0;_tmp107.f1=vd;_tmp107;});
_tmp106;}),0);_tmp105->tl=Cyc_Toc_result_decls;_tmp105;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple5*_tmp10A[3];_tmp10A[2]=({struct _tuple5*_tmp10D=_cycalloc(sizeof(*_tmp10D));
_tmp10D->f1=0;_tmp10D->f2=xplussz;_tmp10D;});_tmp10A[1]=({struct _tuple5*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=0;_tmp10C->f2=xexp;_tmp10C;});_tmp10A[0]=({
struct _tuple5*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->f1=0;_tmp10B->f2=xexp;
_tmp10B;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp10A,sizeof(struct _tuple5*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E->break_lab=0;_tmp10E->continue_lab=0;_tmp10E->fallthru_info=0;_tmp10E->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp10E->toplevel=1;_tmp10E;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp10F=_cycalloc(
sizeof(*_tmp10F));_tmp10F[0]=*e;_tmp10F;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp110=(*x).f1;_LLC0: if(_tmp110 <= (void*)1?1:*((int*)
_tmp110)!= 0)goto _LLC2;_LLC1:({struct Cyc_Std_String_pa_struct _tmp112;_tmp112.tag=
0;_tmp112.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{void*_tmp111[1]={&
_tmp112};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::add_varmap on Rel_n: %s\n",sizeof(unsigned char),30),_tag_arr(
_tmp111,sizeof(void*),1));}});_LLC2:;_LLC3: goto _LLBF;_LLBF:;}{struct Cyc_Toc_Env*
ans=({struct Cyc_Toc_Env*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=*e;
_tmp113;});ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=({struct Cyc_Toc_Env*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=*e;
_tmp114;});ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple7{struct
_tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,struct _tagged_arr*break_l,
struct _tagged_arr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*_tmp115=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){_tmp115=({struct Cyc_List_List*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp116->tl=_tmp115;_tmp116;});}_tmp115=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp115);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=*e;_tmp11A;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->v=
break_l;_tmp117;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp118=_cycalloc(
sizeof(*_tmp118));_tmp118->v=({struct _tuple7*_tmp119=_cycalloc(sizeof(*_tmp119));
_tmp119->f1=fallthru_l;_tmp119->f2=_tmp115;_tmp119->f3=next_case_env->varmap;
_tmp119;});_tmp118;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=*e;_tmp11C;});ans->break_lab=({
struct Cyc_Core_Opt*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->v=break_l;
_tmp11B;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=*e;_tmp11F;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp11D=_cycalloc(sizeof(*_tmp11D));
_tmp11D->v=({struct _tuple7*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->f1=
next_l;_tmp11E->f2=0;_tmp11E->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp11E;});_tmp11D;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp120=(void*)e->annot;
_LLC5: if(*((void**)_tmp120)!= Cyc_CfFlowInfo_UnknownZ)goto _LLC7;_LLC6: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC7: if(*((void**)_tmp120)
!= Cyc_CfFlowInfo_NotZero)goto _LLC9;_LLC8: return 0;_LLC9: if(*((void**)_tmp120)!= 
Cyc_CfFlowInfo_IsZero)goto _LLCB;_LLCA:({void*_tmp121[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("dereference of NULL pointer",sizeof(unsigned char),28),_tag_arr(
_tmp121,sizeof(void*),0));});return 0;_LLCB: if(_tmp120 != Cyc_Absyn_EmptyAnnot)
goto _LLCD;_LLCC: return 0;_LLCD:;_LLCE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Core_Impossible_struct
_tmp123;_tmp123.tag=Cyc_Core_Impossible;_tmp123.f1=_tag_arr("need_null_check",
sizeof(unsigned char),16);_tmp123;});_tmp122;}));_LLC4:;}static struct Cyc_List_List*
Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp124=(void*)e->annot;struct Cyc_List_List*
_tmp125;struct Cyc_List_List*_tmp126;_LLD0: if(*((void**)_tmp124)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLD2;_tmp125=((struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp124)->f1;_LLD1:
return _tmp125;_LLD2: if(*((void**)_tmp124)!= Cyc_CfFlowInfo_NotZero)goto _LLD4;
_tmp126=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp124)->f1;_LLD3: return _tmp126;
_LLD4: if(*((void**)_tmp124)!= Cyc_CfFlowInfo_IsZero)goto _LLD6;_LLD5:({void*
_tmp127[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp127,sizeof(void*),0));});return 0;_LLD6:
if(_tmp124 != Cyc_Absyn_EmptyAnnot)goto _LLD8;_LLD7: return 0;_LLD8:;_LLD9:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp128=_cycalloc(sizeof(*
_tmp128));_tmp128[0]=({struct Cyc_Core_Impossible_struct _tmp129;_tmp129.tag=Cyc_Core_Impossible;
_tmp129.f1=_tag_arr("get_relns",sizeof(unsigned char),10);_tmp129;});_tmp128;}));
_LLCF:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp12A=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp12B;struct Cyc_Absyn_Conref*
_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;_LLDB: if(_tmp12A <= (void*)3?1:*((int*)
_tmp12A)!= 4)goto _LLDD;_tmp12B=((struct Cyc_Absyn_PointerType_struct*)_tmp12A)->f1;
_tmp12C=_tmp12B.bounds;_LLDC: {void*_tmp12E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp12C);struct Cyc_Absyn_Exp*_tmp12F;_LLE2: if((int)_tmp12E != 0)goto _LLE4;_LLE3:
return 0;_LLE4: if(_tmp12E <= (void*)1?1:*((int*)_tmp12E)!= 0)goto _LLE1;_tmp12F=((
struct Cyc_Absyn_Upper_b_struct*)_tmp12E)->f1;_LLE5: {unsigned int j=Cyc_Evexp_eval_const_uint_exp(
_tmp12F);return i <= j;}_LLE1:;}_LLDD: if(_tmp12A <= (void*)3?1:*((int*)_tmp12A)!= 7)
goto _LLDF;_tmp12D=((struct Cyc_Absyn_ArrayType_struct*)_tmp12A)->f3;_LLDE: if(
_tmp12D == 0)return 0;{unsigned int j=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp12D));return i <= j;}_LLDF:;_LLE0: return 0;_LLDA:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp130=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp130->vd != y)continue;{void*_tmp131=(void*)_tmp130->rop;struct
Cyc_Absyn_Vardecl*_tmp132;struct Cyc_Absyn_Vardecl*_tmp133;_LLE7: if(*((int*)
_tmp131)!= 2)goto _LLE9;_tmp132=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp131)->f1;
_LLE8: _tmp133=_tmp132;goto _LLEA;_LLE9: if(*((int*)_tmp131)!= 4)goto _LLEB;_tmp133=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp131)->f1;_LLEA: if(_tmp133 == v)return
1;else{goto _LLE6;}_LLEB:;_LLEC: continue;_LLE6:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp134=(void*)e->r;void*_tmp135;struct Cyc_Absyn_Vardecl*_tmp136;void*_tmp137;
struct Cyc_Absyn_Vardecl*_tmp138;void*_tmp139;struct Cyc_Absyn_Vardecl*_tmp13A;
void*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13C;void*_tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_List_List _tmp13F;struct Cyc_Absyn_Exp*_tmp140;_LLEE: if(*((int*)
_tmp134)!= 1)goto _LLF0;_tmp135=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp134)->f2;
if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 4)goto _LLF0;_tmp136=((struct Cyc_Absyn_Pat_b_struct*)
_tmp135)->f1;_LLEF: _tmp138=_tmp136;goto _LLF1;_LLF0: if(*((int*)_tmp134)!= 1)goto
_LLF2;_tmp137=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp134)->f2;if(_tmp137 <= (
void*)1?1:*((int*)_tmp137)!= 3)goto _LLF2;_tmp138=((struct Cyc_Absyn_Local_b_struct*)
_tmp137)->f1;_LLF1: _tmp13A=_tmp138;goto _LLF3;_LLF2: if(*((int*)_tmp134)!= 1)goto
_LLF4;_tmp139=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp134)->f2;if(_tmp139 <= (
void*)1?1:*((int*)_tmp139)!= 0)goto _LLF4;_tmp13A=((struct Cyc_Absyn_Global_b_struct*)
_tmp139)->f1;_LLF3: _tmp13C=_tmp13A;goto _LLF5;_LLF4: if(*((int*)_tmp134)!= 1)goto
_LLF6;_tmp13B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp134)->f2;if(_tmp13B <= (
void*)1?1:*((int*)_tmp13B)!= 2)goto _LLF6;_tmp13C=((struct Cyc_Absyn_Param_b_struct*)
_tmp13B)->f1;_LLF5: if(_tmp13C->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp13C))return 1;goto _LLED;_LLF6: if(*((int*)_tmp134)!= 3)goto _LLF8;
_tmp13D=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp134)->f1;if((int)_tmp13D != 
19)goto _LLF8;_tmp13E=((struct Cyc_Absyn_Primop_e_struct*)_tmp134)->f2;if(_tmp13E
== 0)goto _LLF8;_tmp13F=*_tmp13E;_tmp140=(struct Cyc_Absyn_Exp*)_tmp13F.hd;_LLF7:{
void*_tmp141=(void*)_tmp140->r;void*_tmp142;struct Cyc_Absyn_Vardecl*_tmp143;void*
_tmp144;struct Cyc_Absyn_Vardecl*_tmp145;void*_tmp146;struct Cyc_Absyn_Vardecl*
_tmp147;void*_tmp148;struct Cyc_Absyn_Vardecl*_tmp149;_LLFB: if(*((int*)_tmp141)!= 
1)goto _LLFD;_tmp142=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp141)->f2;if(
_tmp142 <= (void*)1?1:*((int*)_tmp142)!= 4)goto _LLFD;_tmp143=((struct Cyc_Absyn_Pat_b_struct*)
_tmp142)->f1;_LLFC: _tmp145=_tmp143;goto _LLFE;_LLFD: if(*((int*)_tmp141)!= 1)goto
_LLFF;_tmp144=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp141)->f2;if(_tmp144 <= (
void*)1?1:*((int*)_tmp144)!= 3)goto _LLFF;_tmp145=((struct Cyc_Absyn_Local_b_struct*)
_tmp144)->f1;_LLFE: _tmp147=_tmp145;goto _LL100;_LLFF: if(*((int*)_tmp141)!= 1)goto
_LL101;_tmp146=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp141)->f2;if(_tmp146 <= (
void*)1?1:*((int*)_tmp146)!= 0)goto _LL101;_tmp147=((struct Cyc_Absyn_Global_b_struct*)
_tmp146)->f1;_LL100: _tmp149=_tmp147;goto _LL102;_LL101: if(*((int*)_tmp141)!= 1)
goto _LL103;_tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp141)->f2;if(
_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 2)goto _LL103;_tmp149=((struct Cyc_Absyn_Param_b_struct*)
_tmp148)->f1;_LL102: return _tmp149 == v;_LL103:;_LL104: goto _LLFA;_LLFA:;}goto _LLED;
_LLF8:;_LLF9: goto _LLED;_LLED:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp14A=(void*)a->r;
void*_tmp14B;struct Cyc_Absyn_Vardecl*_tmp14C;void*_tmp14D;struct Cyc_Absyn_Vardecl*
_tmp14E;void*_tmp14F;struct Cyc_Absyn_Vardecl*_tmp150;void*_tmp151;struct Cyc_Absyn_Vardecl*
_tmp152;_LL106: if(*((int*)_tmp14A)!= 1)goto _LL108;_tmp14B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14A)->f2;if(_tmp14B <= (void*)1?1:*((int*)_tmp14B)!= 4)goto _LL108;_tmp14C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp14B)->f1;_LL107: _tmp14E=_tmp14C;goto _LL109;
_LL108: if(*((int*)_tmp14A)!= 1)goto _LL10A;_tmp14D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14A)->f2;if(_tmp14D <= (void*)1?1:*((int*)_tmp14D)!= 3)goto _LL10A;_tmp14E=((
struct Cyc_Absyn_Local_b_struct*)_tmp14D)->f1;_LL109: _tmp150=_tmp14E;goto _LL10B;
_LL10A: if(*((int*)_tmp14A)!= 1)goto _LL10C;_tmp14F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14A)->f2;if(_tmp14F <= (void*)1?1:*((int*)_tmp14F)!= 0)goto _LL10C;_tmp150=((
struct Cyc_Absyn_Global_b_struct*)_tmp14F)->f1;_LL10B: _tmp152=_tmp150;goto _LL10D;
_LL10C: if(*((int*)_tmp14A)!= 1)goto _LL10E;_tmp151=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14A)->f2;if(_tmp151 <= (void*)1?1:*((int*)_tmp151)!= 2)goto _LL10E;_tmp152=((
struct Cyc_Absyn_Param_b_struct*)_tmp151)->f1;_LL10D: if(_tmp152->escapes)return 0;
inner_loop: {void*_tmp153=(void*)i->r;void*_tmp154;struct Cyc_Absyn_Exp*_tmp155;
void*_tmp156;void*_tmp157;int _tmp158;void*_tmp159;void*_tmp15A;int _tmp15B;void*
_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List _tmp15E;struct Cyc_Absyn_Exp*
_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List _tmp161;struct Cyc_Absyn_Exp*
_tmp162;void*_tmp163;struct Cyc_Absyn_Vardecl*_tmp164;void*_tmp165;struct Cyc_Absyn_Vardecl*
_tmp166;void*_tmp167;struct Cyc_Absyn_Vardecl*_tmp168;void*_tmp169;struct Cyc_Absyn_Vardecl*
_tmp16A;_LL111: if(*((int*)_tmp153)!= 13)goto _LL113;_tmp154=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp153)->f1;_tmp155=((struct Cyc_Absyn_Cast_e_struct*)_tmp153)->f2;_LL112: i=
_tmp155;goto inner_loop;_LL113: if(*((int*)_tmp153)!= 0)goto _LL115;_tmp156=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp153)->f1;if(_tmp156 <= (void*)1?1:*((int*)
_tmp156)!= 2)goto _LL115;_tmp157=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp156)->f1;
if((int)_tmp157 != 0)goto _LL115;_tmp158=((struct Cyc_Absyn_Int_c_struct*)_tmp156)->f2;
_LL114: return _tmp158 >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp158 + 1),(
void*)_tmp152->type): 0;_LL115: if(*((int*)_tmp153)!= 0)goto _LL117;_tmp159=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp153)->f1;if(_tmp159 <= (void*)1?1:*((int*)
_tmp159)!= 2)goto _LL117;_tmp15A=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp159)->f1;
if((int)_tmp15A != 1)goto _LL117;_tmp15B=((struct Cyc_Absyn_Int_c_struct*)_tmp159)->f2;
_LL116: return Cyc_Toc_check_const_array((unsigned int)(_tmp15B + 1),(void*)_tmp152->type);
_LL117: if(*((int*)_tmp153)!= 3)goto _LL119;_tmp15C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp153)->f1;if((int)_tmp15C != 4)goto _LL119;_tmp15D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp153)->f2;if(_tmp15D == 0)goto _LL119;_tmp15E=*_tmp15D;_tmp15F=(struct Cyc_Absyn_Exp*)
_tmp15E.hd;_tmp160=_tmp15E.tl;if(_tmp160 == 0)goto _LL119;_tmp161=*_tmp160;_tmp162=(
struct Cyc_Absyn_Exp*)_tmp161.hd;_LL118: return Cyc_Toc_check_leq_size(relns,
_tmp152,_tmp162);_LL119: if(*((int*)_tmp153)!= 1)goto _LL11B;_tmp163=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp153)->f2;if(_tmp163 <= (void*)1?1:*((int*)
_tmp163)!= 4)goto _LL11B;_tmp164=((struct Cyc_Absyn_Pat_b_struct*)_tmp163)->f1;
_LL11A: _tmp166=_tmp164;goto _LL11C;_LL11B: if(*((int*)_tmp153)!= 1)goto _LL11D;
_tmp165=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp153)->f2;if(_tmp165 <= (void*)
1?1:*((int*)_tmp165)!= 3)goto _LL11D;_tmp166=((struct Cyc_Absyn_Local_b_struct*)
_tmp165)->f1;_LL11C: _tmp168=_tmp166;goto _LL11E;_LL11D: if(*((int*)_tmp153)!= 1)
goto _LL11F;_tmp167=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp153)->f2;if(
_tmp167 <= (void*)1?1:*((int*)_tmp167)!= 0)goto _LL11F;_tmp168=((struct Cyc_Absyn_Global_b_struct*)
_tmp167)->f1;_LL11E: _tmp16A=_tmp168;goto _LL120;_LL11F: if(*((int*)_tmp153)!= 1)
goto _LL121;_tmp169=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp153)->f2;if(
_tmp169 <= (void*)1?1:*((int*)_tmp169)!= 2)goto _LL121;_tmp16A=((struct Cyc_Absyn_Param_b_struct*)
_tmp169)->f1;_LL120: if(_tmp16A->escapes)return 0;{struct Cyc_List_List*_tmp16B=
relns;for(0;_tmp16B != 0;_tmp16B=_tmp16B->tl){struct Cyc_CfFlowInfo_Reln*_tmp16C=(
struct Cyc_CfFlowInfo_Reln*)_tmp16B->hd;if(_tmp16C->vd == _tmp16A){void*_tmp16D=(
void*)_tmp16C->rop;struct Cyc_Absyn_Vardecl*_tmp16E;struct Cyc_Absyn_Vardecl*
_tmp16F;unsigned int _tmp170;_LL124: if(*((int*)_tmp16D)!= 2)goto _LL126;_tmp16E=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp16D)->f1;_LL125: if(_tmp152 == _tmp16E)
return 1;else{goto _LL123;}_LL126: if(*((int*)_tmp16D)!= 1)goto _LL128;_tmp16F=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp16D)->f1;_LL127:{struct Cyc_List_List*
_tmp171=relns;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){struct Cyc_CfFlowInfo_Reln*
_tmp172=(struct Cyc_CfFlowInfo_Reln*)_tmp171->hd;if(_tmp172->vd == _tmp16F){void*
_tmp173=(void*)_tmp172->rop;struct Cyc_Absyn_Vardecl*_tmp174;struct Cyc_Absyn_Vardecl*
_tmp175;unsigned int _tmp176;struct Cyc_Absyn_Vardecl*_tmp177;_LL12D: if(*((int*)
_tmp173)!= 4)goto _LL12F;_tmp174=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp173)->f1;_LL12E: _tmp175=_tmp174;goto _LL130;_LL12F: if(*((int*)_tmp173)!= 2)
goto _LL131;_tmp175=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp173)->f1;_LL130:
if(_tmp152 == _tmp175)return 1;else{goto _LL12C;}_LL131: if(*((int*)_tmp173)!= 0)
goto _LL133;_tmp176=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp173)->f1;_LL132:
return Cyc_Toc_check_const_array(_tmp176,(void*)_tmp152->type);_LL133: if(*((int*)
_tmp173)!= 1)goto _LL135;_tmp177=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp173)->f1;
_LL134: if(Cyc_Toc_check_leq_size_var(relns,_tmp152,_tmp177))return 1;else{goto
_LL12C;}_LL135:;_LL136: goto _LL12C;_LL12C:;}}}goto _LL123;_LL128: if(*((int*)
_tmp16D)!= 3)goto _LL12A;_tmp170=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp16D)->f1;
_LL129: return Cyc_Toc_check_const_array(_tmp170,(void*)_tmp152->type);_LL12A:;
_LL12B: goto _LL123;_LL123:;}}}goto _LL110;_LL121:;_LL122: goto _LL110;_LL110:;}goto
_LL105;_LL10E:;_LL10F: goto _LL105;_LL105:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp178[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp178,sizeof(void*),0));});return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp179[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp179,sizeof(void*),0));});return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp17A=_cycalloc(sizeof(*_tmp17A));
_tmp17A->f1=Cyc_Toc_mt_tq;_tmp17A->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp17A;});}static struct _tuple5*Cyc_Toc_add_designator(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({
struct _tuple5*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=0;_tmp17B->f2=e;
_tmp17B;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp17C=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(
is_atomic?Cyc_Toc_malloc_atomic(_tmp17C): Cyc_Toc_malloc_ptr(_tmp17C));else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(rgnopt);Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp17C);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct
Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp17D=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp17D != 0;_tmp17D=_tmp17D->tl){struct _tuple5 _tmp17F;struct Cyc_List_List*
_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct _tuple5*_tmp17E=(struct _tuple5*)
_tmp17D->hd;_tmp17F=*_tmp17E;_tmp180=_tmp17F.f1;_tmp181=_tmp17F.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp180 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp180->tl != 0)({void*_tmp182[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("multiple designators in array",sizeof(
unsigned char),30),_tag_arr(_tmp182,sizeof(void*),0));});{void*_tmp183=(void*)
_tmp180->hd;void*_tmp184=_tmp183;struct Cyc_Absyn_Exp*_tmp185;_LL138: if(*((int*)
_tmp184)!= 0)goto _LL13A;_tmp185=((struct Cyc_Absyn_ArrayElement_struct*)_tmp184)->f1;
_LL139: Cyc_Toc_exp_to_c(nv,_tmp185);e_index=_tmp185;goto _LL137;_LL13A: if(*((int*)
_tmp184)!= 1)goto _LL137;_LL13B:({void*_tmp186[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(unsigned char),32),_tag_arr(_tmp186,sizeof(void*),0));});_LL137:;}}{
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp187=(
void*)_tmp181->r;struct Cyc_List_List*_tmp188;struct Cyc_Absyn_Vardecl*_tmp189;
struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;void*_tmp18C;struct Cyc_List_List*
_tmp18D;_LL13D: if(*((int*)_tmp187)!= 26)goto _LL13F;_tmp188=((struct Cyc_Absyn_Array_e_struct*)
_tmp187)->f1;_LL13E: s=Cyc_Toc_init_array(nv,lval,_tmp188,s);goto _LL13C;_LL13F:
if(*((int*)_tmp187)!= 27)goto _LL141;_tmp189=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp187)->f1;_tmp18A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp187)->f2;
_tmp18B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp187)->f3;_LL140: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp189,_tmp18A,_tmp18B,s,0);goto _LL13C;_LL141: if(*((int*)_tmp187)!= 29)
goto _LL143;_tmp18C=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp187)->f1;
_tmp18D=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp187)->f2;_LL142: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp18C,_tmp18D,s);goto _LL13C;_LL143:;_LL144: Cyc_Toc_exp_to_c(nv,_tmp181);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp181,0),s,0);goto _LL13C;_LL13C:;}}}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp18E=vd->name;if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(nv,_tmp18E,Cyc_Absyn_varb_exp(
_tmp18E,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp196=_cycalloc(sizeof(*
_tmp196));_tmp196[0]=({struct Cyc_Absyn_Local_b_struct _tmp197;_tmp197.tag=3;
_tmp197.f1=vd;_tmp197;});_tmp196;}),0));struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp18E,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp18E,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp18E,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp18E,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp18F=(void*)e2->r;struct Cyc_List_List*
_tmp190;struct Cyc_Absyn_Vardecl*_tmp191;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*
_tmp193;void*_tmp194;struct Cyc_List_List*_tmp195;_LL146: if(*((int*)_tmp18F)!= 26)
goto _LL148;_tmp190=((struct Cyc_Absyn_Array_e_struct*)_tmp18F)->f1;_LL147: body=
Cyc_Toc_init_array(nv2,lval,_tmp190,Cyc_Toc_skip_stmt_dl());goto _LL145;_LL148:
if(*((int*)_tmp18F)!= 27)goto _LL14A;_tmp191=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18F)->f1;_tmp192=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp18F)->f2;
_tmp193=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp18F)->f3;_LL149: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp191,_tmp192,_tmp193,Cyc_Toc_skip_stmt_dl(),0);goto _LL145;_LL14A: if(*((
int*)_tmp18F)!= 29)goto _LL14C;_tmp194=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp18F)->f1;_tmp195=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18F)->f2;_LL14B:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp194,_tmp195,Cyc_Toc_skip_stmt_dl());
goto _LL145;_LL14C:;_LL14D: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL145;_LL145:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp18E,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp198=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp198 != 0;_tmp198=_tmp198->tl){struct _tuple5 _tmp19A;
struct Cyc_List_List*_tmp19B;struct Cyc_Absyn_Exp*_tmp19C;struct _tuple5*_tmp199=(
struct _tuple5*)_tmp198->hd;_tmp19A=*_tmp199;_tmp19B=_tmp19A.f1;_tmp19C=_tmp19A.f2;
if(_tmp19B == 0)({void*_tmp19D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(
unsigned char),22),_tag_arr(_tmp19D,sizeof(void*),0));});if(_tmp19B->tl != 0)({
void*_tmp19E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct",sizeof(unsigned char),41),
_tag_arr(_tmp19E,sizeof(void*),0));});{void*_tmp19F=(void*)_tmp19B->hd;struct
_tagged_arr*_tmp1A0;_LL14F: if(*((int*)_tmp19F)!= 1)goto _LL151;_tmp1A0=((struct
Cyc_Absyn_FieldName_struct*)_tmp19F)->f1;_LL150: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp1A0,0);{void*_tmp1A1=(void*)_tmp19C->r;struct Cyc_List_List*_tmp1A2;
struct Cyc_Absyn_Vardecl*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*
_tmp1A5;void*_tmp1A6;struct Cyc_List_List*_tmp1A7;_LL154: if(*((int*)_tmp1A1)!= 26)
goto _LL156;_tmp1A2=((struct Cyc_Absyn_Array_e_struct*)_tmp1A1)->f1;_LL155: s=Cyc_Toc_init_array(
nv,lval,_tmp1A2,s);goto _LL153;_LL156: if(*((int*)_tmp1A1)!= 27)goto _LL158;_tmp1A3=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A1)->f1;_tmp1A4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1A1)->f2;_tmp1A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A1)->f3;
_LL157: s=Cyc_Toc_init_comprehension(nv,lval,_tmp1A3,_tmp1A4,_tmp1A5,s,0);goto
_LL153;_LL158: if(*((int*)_tmp1A1)!= 29)goto _LL15A;_tmp1A6=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1A1)->f1;_tmp1A7=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1A1)->f2;_LL159:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1A6,_tmp1A7,s);goto _LL153;_LL15A:;_LL15B:
Cyc_Toc_exp_to_c(nv,_tmp19C);if(Cyc_Toc_is_poly_field(struct_type,_tmp1A0))
_tmp19C=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp19C,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp19C,0),0),s,0);goto _LL153;
_LL153:;}goto _LL14E;}_LL151:;_LL152:({void*_tmp1A8[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1A8,sizeof(void*),0));});_LL14E:;}}}
return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int
pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct _RegionHandle
_tmp1A9=_new_region("r");struct _RegionHandle*r=& _tmp1A9;_push_region(r);{struct
Cyc_List_List*_tmp1AA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,
es);struct _tagged_arr*_tmp1AB=Cyc_Toc_add_tuple_type(_tmp1AA);struct _tuple1*
_tmp1AC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1AD=Cyc_Absyn_var_exp(_tmp1AC,
0);struct Cyc_Absyn_Stmt*_tmp1AE=Cyc_Absyn_exp_stmt(_tmp1AD,0);struct Cyc_Absyn_Exp*(*
_tmp1AF)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct
Cyc_List_List*_tmp1B0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1B0);for(0;_tmp1B0 != 0;(_tmp1B0=_tmp1B0->tl,-- i)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)_tmp1B0->hd;struct Cyc_Absyn_Exp*lval=_tmp1AF(_tmp1AD,Cyc_Absyn_fieldname(
i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v): 0;{void*_tmp1B1=(void*)e->r;struct Cyc_List_List*
_tmp1B2;struct Cyc_Absyn_Vardecl*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*
_tmp1B5;_LL15D: if(*((int*)_tmp1B1)!= 26)goto _LL15F;_tmp1B2=((struct Cyc_Absyn_Array_e_struct*)
_tmp1B1)->f1;_LL15E: _tmp1AE=Cyc_Toc_init_array(nv,lval,_tmp1B2,_tmp1AE);goto
_LL15C;_LL15F: if(*((int*)_tmp1B1)!= 27)goto _LL161;_tmp1B3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1B1)->f1;_tmp1B4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B1)->f2;
_tmp1B5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B1)->f3;_LL160: _tmp1AE=
Cyc_Toc_init_comprehension(nv,lval,_tmp1B3,_tmp1B4,_tmp1B5,_tmp1AE,0);goto _LL15C;
_LL161:;_LL162: Cyc_Toc_exp_to_c(nv,e);_tmp1AE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp1AF(_tmp1AD,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1AE,0);
goto _LL15C;_LL15C:;}}}{struct Cyc_Absyn_Exp*_tmp1B6=Cyc_Toc_make_struct(nv,
_tmp1AC,Cyc_Absyn_strct(_tmp1AB),_tmp1AE,pointer,rgnopt,is_atomic);_npop_handler(
0);return _tmp1B6;}};_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1B7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1B8=Cyc_Absyn_var_exp(_tmp1B7,0);struct Cyc_Absyn_Stmt*
_tmp1B9=Cyc_Absyn_exp_stmt(_tmp1B8,0);struct Cyc_Absyn_Exp*(*_tmp1BA)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1BB=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1BC;void*
_tmp1BD;_LL164: if(_tmp1BB <= (void*)3?1:*((int*)_tmp1BB)!= 10)goto _LL166;_tmp1BC=((
struct Cyc_Absyn_AggrType_struct*)_tmp1BB)->f1;_tmp1BD=(void*)_tmp1BC.aggr_info;
_LL165: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1BD);goto _LL163;_LL166:;_LL167:({void*
_tmp1BE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("init_struct: bad struct type",sizeof(unsigned char),29),_tag_arr(
_tmp1BE,sizeof(void*),0));});_LL163:;}{struct _RegionHandle _tmp1BF=_new_region("r");
struct _RegionHandle*r=& _tmp1BF;_push_region(r);{struct Cyc_List_List*_tmp1C0=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp1C0 != 0;_tmp1C0=_tmp1C0->tl){struct _tuple5 _tmp1C2;struct Cyc_List_List*
_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct _tuple5*_tmp1C1=(struct _tuple5*)
_tmp1C0->hd;_tmp1C2=*_tmp1C1;_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;is_atomic=
is_atomic?Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C4->topt))->v):
0;if(_tmp1C3 == 0)({void*_tmp1C5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(
unsigned char),22),_tag_arr(_tmp1C5,sizeof(void*),0));});if(_tmp1C3->tl != 0){
struct _tuple1*_tmp1C6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1C7=Cyc_Absyn_var_exp(
_tmp1C6,0);for(0;_tmp1C3 != 0;_tmp1C3=_tmp1C3->tl){void*_tmp1C8=(void*)_tmp1C3->hd;
struct _tagged_arr*_tmp1C9;_LL169: if(*((int*)_tmp1C8)!= 1)goto _LL16B;_tmp1C9=((
struct Cyc_Absyn_FieldName_struct*)_tmp1C8)->f1;_LL16A: if(Cyc_Toc_is_poly_field(
struct_type,_tmp1C9))_tmp1C7=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1C7,0);_tmp1B9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1BA(_tmp1B8,_tmp1C9,0),_tmp1C7,0),0),_tmp1B9,0);goto _LL168;_LL16B:;_LL16C:({
void*_tmp1CA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct",sizeof(unsigned char),27),_tag_arr(_tmp1CA,
sizeof(void*),0));});_LL168:;}Cyc_Toc_exp_to_c(nv,_tmp1C4);_tmp1B9=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1C7,_tmp1C4,0),0),_tmp1B9,0);}else{
void*_tmp1CB=(void*)_tmp1C3->hd;struct _tagged_arr*_tmp1CC;_LL16E: if(*((int*)
_tmp1CB)!= 1)goto _LL170;_tmp1CC=((struct Cyc_Absyn_FieldName_struct*)_tmp1CB)->f1;
_LL16F: {struct Cyc_Absyn_Exp*lval=_tmp1BA(_tmp1B8,_tmp1CC,0);{void*_tmp1CD=(void*)
_tmp1C4->r;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Vardecl*_tmp1CF;struct Cyc_Absyn_Exp*
_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;void*_tmp1D2;struct Cyc_List_List*_tmp1D3;
_LL173: if(*((int*)_tmp1CD)!= 26)goto _LL175;_tmp1CE=((struct Cyc_Absyn_Array_e_struct*)
_tmp1CD)->f1;_LL174: _tmp1B9=Cyc_Toc_init_array(nv,lval,_tmp1CE,_tmp1B9);goto
_LL172;_LL175: if(*((int*)_tmp1CD)!= 27)goto _LL177;_tmp1CF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1CD)->f1;_tmp1D0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1CD)->f2;
_tmp1D1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1CD)->f3;_LL176: _tmp1B9=
Cyc_Toc_init_comprehension(nv,lval,_tmp1CF,_tmp1D0,_tmp1D1,_tmp1B9,0);goto _LL172;
_LL177: if(*((int*)_tmp1CD)!= 29)goto _LL179;_tmp1D2=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1CD)->f1;_tmp1D3=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1CD)->f2;_LL178:
_tmp1B9=Cyc_Toc_init_anon_struct(nv,lval,_tmp1D2,_tmp1D3,_tmp1B9);goto _LL172;
_LL179:;_LL17A: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C4->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp1C4);if(Cyc_Toc_is_poly_field(
struct_type,_tmp1CC)?!was_ptr_type: 0)_tmp1C4=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1C4,0);if(has_exists)_tmp1C4=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp1CC)))->type,_tmp1C4,0);_tmp1B9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1C4,0),0),
_tmp1B9,0);goto _LL172;}_LL172:;}goto _LL16D;}_LL170:;_LL171:({void*_tmp1D4[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1D4,sizeof(void*),0));});_LL16D:;}}}{
struct Cyc_Absyn_Exp*_tmp1D5=Cyc_Toc_make_struct(nv,_tmp1B7,Cyc_Absyn_strctq(tdn),
_tmp1B9,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1D5;};_pop_region(r);}}
struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));
_tmp1D6[0]=({struct Cyc_Absyn_Increment_e_struct _tmp1D7;_tmp1D7.tag=5;_tmp1D7.f1=
e1;_tmp1D7.f2=(void*)incr;_tmp1D7;});_tmp1D6;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp1D8=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp1D9;void*_tmp1DA;struct Cyc_Absyn_Exp*
_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;struct _tagged_arr*_tmp1DD;_LL17C: if(*((int*)
_tmp1D8)!= 35)goto _LL17E;_tmp1D9=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D8)->f1;
_LL17D: Cyc_Toc_lvalue_assign_stmt(_tmp1D9,fs,f,f_env);goto _LL17B;_LL17E: if(*((
int*)_tmp1D8)!= 13)goto _LL180;_tmp1DA=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1D8)->f1;_tmp1DB=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D8)->f2;_LL17F: Cyc_Toc_lvalue_assign(
_tmp1DB,fs,f,f_env);goto _LL17B;_LL180: if(*((int*)_tmp1D8)!= 21)goto _LL182;
_tmp1DC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D8)->f1;_tmp1DD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1D8)->f2;_LL181:(void*)(e1->r=(void*)((void*)_tmp1DC->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));
_tmp1DE->hd=_tmp1DD;_tmp1DE->tl=fs;_tmp1DE;}),f,f_env);goto _LL17B;_LL182:;_LL183: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL17B;}_LL17B:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp1DF=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Decl*_tmp1E1;struct Cyc_Absyn_Stmt*
_tmp1E2;struct Cyc_Absyn_Stmt*_tmp1E3;_LL185: if(_tmp1DF <= (void*)1?1:*((int*)
_tmp1DF)!= 0)goto _LL187;_tmp1E0=((struct Cyc_Absyn_Exp_s_struct*)_tmp1DF)->f1;
_LL186: Cyc_Toc_lvalue_assign(_tmp1E0,fs,f,f_env);goto _LL184;_LL187: if(_tmp1DF <= (
void*)1?1:*((int*)_tmp1DF)!= 12)goto _LL189;_tmp1E1=((struct Cyc_Absyn_Decl_s_struct*)
_tmp1DF)->f1;_tmp1E2=((struct Cyc_Absyn_Decl_s_struct*)_tmp1DF)->f2;_LL188: Cyc_Toc_lvalue_assign_stmt(
_tmp1E2,fs,f,f_env);goto _LL184;_LL189: if(_tmp1DF <= (void*)1?1:*((int*)_tmp1DF)!= 
1)goto _LL18B;_tmp1E3=((struct Cyc_Absyn_Seq_s_struct*)_tmp1DF)->f2;_LL18A: Cyc_Toc_lvalue_assign_stmt(
_tmp1E3,fs,f,f_env);goto _LL184;_LL18B:;_LL18C:({struct Cyc_Std_String_pa_struct
_tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{
void*_tmp1E4[1]={& _tmp1E5};Cyc_Toc_toc_impos(_tag_arr("lvalue_assign_stmt: %s",
sizeof(unsigned char),23),_tag_arr(_tmp1E4,sizeof(void*),1));}});_LL184:;}static
struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct Cyc_List_List*
prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp1E6=_region_malloc(r2,
sizeof(*_tmp1E6));_tmp1E6->hd=(void*)f((void*)x->hd,env);_tmp1E6->tl=0;_tmp1E6;});
prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({
struct Cyc_List_List*_tmp1E7=_region_malloc(r2,sizeof(*_tmp1E7));_tmp1E7->hd=(
void*)f((void*)x->hd,env);_tmp1E7->tl=0;_tmp1E7;});prev=((struct Cyc_List_List*)
_check_null(prev))->tl;}return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple5*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({
struct _tuple5*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->f1=0;_tmp1E8->f2=e;
_tmp1E8;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*
_tmp1E9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EA;_LL18E: if(_tmp1E9
<= (void*)3?1:*((int*)_tmp1E9)!= 4)goto _LL190;_tmp1EA=((struct Cyc_Absyn_PointerType_struct*)
_tmp1E9)->f1;_LL18F: return _tmp1EA;_LL190:;_LL191:({void*_tmp1EB[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("get_ptr_typ: not a pointer!",
sizeof(unsigned char),28),_tag_arr(_tmp1EB,sizeof(void*),0));});_LL18D:;}struct
_tuple9{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp1EC=(void*)e->r;if(e->topt
== 0)({struct Cyc_Std_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct
_tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp1ED[1]={& _tmp1EE};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",
sizeof(unsigned char),25),_tag_arr(_tmp1ED,sizeof(void*),1));}});{void*old_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp1EF=_tmp1EC;void*
_tmp1F0;struct _tuple1*_tmp1F1;void*_tmp1F2;struct _tuple1*_tmp1F3;void*_tmp1F4;
struct Cyc_List_List*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Exp*
_tmp1F8;struct Cyc_Core_Opt*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*
_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*
_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_List_List*
_tmp201;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_List_List*_tmp203;struct Cyc_Absyn_VarargCallInfo*
_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_List_List*_tmp206;struct Cyc_Absyn_VarargCallInfo*
_tmp207;struct Cyc_Absyn_VarargCallInfo _tmp208;int _tmp209;struct Cyc_List_List*
_tmp20A;struct Cyc_Absyn_VarargInfo*_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;struct Cyc_Absyn_Exp*
_tmp20D;struct Cyc_Absyn_Exp*_tmp20E;struct Cyc_List_List*_tmp20F;void*_tmp210;
void**_tmp211;struct Cyc_Absyn_Exp*_tmp212;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*
_tmp214;struct Cyc_Absyn_Exp*_tmp215;struct Cyc_Absyn_Exp*_tmp216;void*_tmp217;
void*_tmp218;void*_tmp219;struct _tagged_arr*_tmp21A;void*_tmp21B;void*_tmp21C;
unsigned int _tmp21D;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;
struct _tagged_arr*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct _tagged_arr*_tmp222;
struct Cyc_Absyn_Exp*_tmp223;struct Cyc_Absyn_Exp*_tmp224;struct Cyc_List_List*
_tmp225;struct Cyc_List_List*_tmp226;struct Cyc_Absyn_Vardecl*_tmp227;struct Cyc_Absyn_Exp*
_tmp228;struct Cyc_Absyn_Exp*_tmp229;struct _tuple1*_tmp22A;struct Cyc_List_List*
_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_Absyn_Aggrdecl*_tmp22D;void*
_tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_Absyn_Tuniondecl*
_tmp231;struct Cyc_Absyn_Tunionfield*_tmp232;struct Cyc_List_List*_tmp233;struct
Cyc_Absyn_Tuniondecl*_tmp234;struct Cyc_Absyn_Tunionfield*_tmp235;struct Cyc_Absyn_MallocInfo
_tmp236;int _tmp237;struct Cyc_Absyn_Exp*_tmp238;void**_tmp239;struct Cyc_Absyn_Exp*
_tmp23A;int _tmp23B;struct Cyc_Absyn_Stmt*_tmp23C;struct Cyc_Absyn_Fndecl*_tmp23D;
_LL193: if(*((int*)_tmp1EF)!= 0)goto _LL195;_tmp1F0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1EF)->f1;if((int)_tmp1F0 != 0)goto _LL195;_LL194: {struct Cyc_Absyn_Exp*_tmp23E=
Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp23E,
_tmp23E))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp23F[3];_tmp23F[2]=_tmp23E;_tmp23F[1]=_tmp23E;_tmp23F[0]=
_tmp23E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp23F,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL192;}_LL195: if(*((int*)
_tmp1EF)!= 0)goto _LL197;_LL196: goto _LL192;_LL197: if(*((int*)_tmp1EF)!= 1)goto
_LL199;_tmp1F1=((struct Cyc_Absyn_Var_e_struct*)_tmp1EF)->f1;_tmp1F2=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp1EF)->f2;_LL198:{struct _handler_cons _tmp240;
_push_handler(& _tmp240);{int _tmp242=0;if(setjmp(_tmp240.handler))_tmp242=1;if(!
_tmp242){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp1F1))->r));;_pop_handler();}
else{void*_tmp241=(void*)_exn_thrown;void*_tmp244=_tmp241;_LL1E8: if(_tmp244 != 
Cyc_Dict_Absent)goto _LL1EA;_LL1E9:({struct Cyc_Std_String_pa_struct _tmp246;
_tmp246.tag=0;_tmp246.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F1);{
void*_tmp245[1]={& _tmp246};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("Can't find %s in exp_to_c, Var\n",sizeof(
unsigned char),32),_tag_arr(_tmp245,sizeof(void*),1));}});_LL1EA:;_LL1EB:(void)
_throw(_tmp244);_LL1E7:;}}}goto _LL192;_LL199: if(*((int*)_tmp1EF)!= 2)goto _LL19B;
_tmp1F3=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1EF)->f1;_LL19A:({void*_tmp247[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(_tmp247,sizeof(void*),0));});
_LL19B: if(*((int*)_tmp1EF)!= 3)goto _LL19D;_tmp1F4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1EF)->f1;_tmp1F5=((struct Cyc_Absyn_Primop_e_struct*)_tmp1EF)->f2;_LL19C: {
struct Cyc_List_List*_tmp248=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp1F5);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp1F5);{void*_tmp249=_tmp1F4;_LL1ED:
if((int)_tmp249 != 19)goto _LL1EF;_LL1EE: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F5))->hd;{void*_tmp24A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*
_tmp24B;struct Cyc_Absyn_PtrInfo _tmp24C;void*_tmp24D;struct Cyc_Absyn_Conref*
_tmp24E;struct Cyc_Absyn_Conref*_tmp24F;_LL202: if(_tmp24A <= (void*)3?1:*((int*)
_tmp24A)!= 7)goto _LL204;_tmp24B=((struct Cyc_Absyn_ArrayType_struct*)_tmp24A)->f3;
_LL203:(void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp24B))->r));
goto _LL201;_LL204: if(_tmp24A <= (void*)3?1:*((int*)_tmp24A)!= 4)goto _LL206;
_tmp24C=((struct Cyc_Absyn_PointerType_struct*)_tmp24A)->f1;_tmp24D=(void*)
_tmp24C.elt_typ;_tmp24E=_tmp24C.nullable;_tmp24F=_tmp24C.bounds;_LL205:{void*
_tmp250=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp24F);struct Cyc_Absyn_Exp*
_tmp251;_LL209: if((int)_tmp250 != 0)goto _LL20B;_LL20A:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({struct Cyc_Absyn_Exp*_tmp252[2];
_tmp252[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp24D),0);_tmp252[0]=(
struct Cyc_Absyn_Exp*)_tmp1F5->hd;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp252,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));
goto _LL208;_LL20B: if(_tmp250 <= (void*)1?1:*((int*)_tmp250)!= 0)goto _LL208;
_tmp251=((struct Cyc_Absyn_Upper_b_struct*)_tmp250)->f1;_LL20C: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp24E))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_conditional_exp(arg,_tmp251,Cyc_Absyn_uint_exp(0,0),0))->r));
else{(void*)(e->r=(void*)((void*)_tmp251->r));goto _LL208;}_LL208:;}goto _LL201;
_LL206:;_LL207:({struct Cyc_Std_String_pa_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct _tmp254;_tmp254.tag=0;
_tmp254.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{void*_tmp253[2]={& _tmp254,& _tmp255};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("size primop applied to non-array %s (%s)",
sizeof(unsigned char),41),_tag_arr(_tmp253,sizeof(void*),2));}}});_LL201:;}goto
_LL1EC;}_LL1EF: if((int)_tmp249 != 0)goto _LL1F1;_LL1F0: {void*elt_typ=(void*)0;if(
Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(
_tmp248))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp1F5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F5->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp256[3];
_tmp256[2]=e2;_tmp256[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp256[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp256,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL1EC;}
_LL1F1: if((int)_tmp249 != 2)goto _LL1F3;_LL1F2: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp248))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1F5))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1F5->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp248->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp257[3];_tmp257[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp257[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp257[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp257,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL1EC;}_LL1F3: if((int)_tmp249 != 5)goto _LL1F5;_LL1F4: goto _LL1F6;
_LL1F5: if((int)_tmp249 != 6)goto _LL1F7;_LL1F6: goto _LL1F8;_LL1F7: if((int)_tmp249 != 
7)goto _LL1F9;_LL1F8: goto _LL1FA;_LL1F9: if((int)_tmp249 != 9)goto _LL1FB;_LL1FA: goto
_LL1FC;_LL1FB: if((int)_tmp249 != 8)goto _LL1FD;_LL1FC: goto _LL1FE;_LL1FD: if((int)
_tmp249 != 10)goto _LL1FF;_LL1FE: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F5->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp248))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp248->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL1EC;}_LL1FF:;_LL200: goto _LL1EC;_LL1EC:;}goto _LL192;}_LL19D: if(*((int*)
_tmp1EF)!= 5)goto _LL19F;_tmp1F6=((struct Cyc_Absyn_Increment_e_struct*)_tmp1EF)->f1;
_tmp1F7=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1EF)->f2;_LL19E: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F6->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1F6);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp258=(_tmp1F7 == (void*)1?1: _tmp1F7 == (
void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp259=(_tmp1F7 == (void*)0?1: _tmp1F7 == (void*)1)?1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp258,({struct Cyc_Absyn_Exp*_tmp25A[3];_tmp25A[2]=
Cyc_Absyn_signed_int_exp(_tmp259,0);_tmp25A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp25A[0]=Cyc_Absyn_address_exp(_tmp1F6,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}if(elt_typ == (void*)0?!Cyc_Absyn_is_lvalue(_tmp1F6): 0){Cyc_Toc_lvalue_assign(
_tmp1F6,0,Cyc_Toc_incr_lvalue,_tmp1F7);(void*)(e->r=(void*)((void*)_tmp1F6->r));}
goto _LL192;}}_LL19F: if(*((int*)_tmp1EF)!= 4)goto _LL1A1;_tmp1F8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1EF)->f1;_tmp1F9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1EF)->f2;_tmp1FA=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1EF)->f3;_LL1A0: {int e1_poly=Cyc_Toc_is_poly_project(
_tmp1F8);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F8->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FA->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1F8);Cyc_Toc_exp_to_c(nv,_tmp1FA);{int done=0;if(_tmp1F9 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp25B=(void*)_tmp1F9->v;_LL20E: if((int)_tmp25B != 0)goto _LL210;
_LL20F: change=_tmp1FA;goto _LL20D;_LL210: if((int)_tmp25B != 2)goto _LL212;_LL211:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp1FA,0);goto _LL20D;_LL212:;_LL213:({void*
_tmp25C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("bad t ? pointer arithmetic",sizeof(unsigned char),27),_tag_arr(_tmp25C,
sizeof(void*),0));});_LL20D:;}done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({struct Cyc_Absyn_Exp*_tmp25D[3];_tmp25D[2]=
change;_tmp25D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp25D[0]=
Cyc_Absyn_address_exp(_tmp1F8,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp25D,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}
if(!done){if(e1_poly)(void*)(_tmp1FA->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp1FA->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp1F8)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp1F8,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp25E=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp25E[0]=({struct _tuple8 _tmp25F;_tmp25F.f1=_tmp1F9;_tmp25F.f2=
_tmp1FA;_tmp25F;});_tmp25E;}));(void*)(e->r=(void*)((void*)_tmp1F8->r));}}goto
_LL192;}}_LL1A1: if(*((int*)_tmp1EF)!= 6)goto _LL1A3;_tmp1FB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1EF)->f1;_tmp1FC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1EF)->f2;
_tmp1FD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1EF)->f3;_LL1A2: Cyc_Toc_exp_to_c(
nv,_tmp1FB);Cyc_Toc_exp_to_c(nv,_tmp1FC);Cyc_Toc_exp_to_c(nv,_tmp1FD);goto _LL192;
_LL1A3: if(*((int*)_tmp1EF)!= 7)goto _LL1A5;_tmp1FE=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1EF)->f1;_tmp1FF=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1EF)->f2;_LL1A4: Cyc_Toc_exp_to_c(
nv,_tmp1FE);Cyc_Toc_exp_to_c(nv,_tmp1FF);goto _LL192;_LL1A5: if(*((int*)_tmp1EF)!= 
8)goto _LL1A7;_tmp200=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1EF)->f1;
_tmp201=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1EF)->f2;_LL1A6: _tmp202=
_tmp200;_tmp203=_tmp201;goto _LL1A8;_LL1A7: if(*((int*)_tmp1EF)!= 9)goto _LL1A9;
_tmp202=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1EF)->f1;_tmp203=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1EF)->f2;_tmp204=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1EF)->f3;if(_tmp204
!= 0)goto _LL1A9;_LL1A8: Cyc_Toc_exp_to_c(nv,_tmp202);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp203);goto _LL192;_LL1A9: if(*((int*)_tmp1EF)!= 9)goto
_LL1AB;_tmp205=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1EF)->f1;_tmp206=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1EF)->f2;_tmp207=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1EF)->f3;if(_tmp207 == 0)goto _LL1AB;_tmp208=*_tmp207;_tmp209=_tmp208.num_varargs;
_tmp20A=_tmp208.injectors;_tmp20B=_tmp208.vai;_LL1AA:{struct _RegionHandle _tmp260=
_new_region("r");struct _RegionHandle*r=& _tmp260;_push_region(r);{struct _tuple1*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp209,0);
void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp20B->type);void*arr_type=(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp275;_tmp275.tag=7;_tmp275.f1=(void*)cva_type;
_tmp275.f2=Cyc_Toc_mt_tq;_tmp275.f3=(struct Cyc_Absyn_Exp*)num_varargs_exp;
_tmp275;});_tmp274;});int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp206);int num_normargs=num_args - _tmp209;struct Cyc_List_List*new_args=0;{int i=
0;for(0;i < num_normargs;(++ i,_tmp206=_tmp206->tl)){Cyc_Toc_exp_to_c(nv,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp206))->hd);new_args=({
struct Cyc_List_List*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261->hd=(struct Cyc_Absyn_Exp*)
_tmp206->hd;_tmp261->tl=new_args;_tmp261;});}}new_args=({struct Cyc_List_List*
_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp263[3];_tmp263[2]=num_varargs_exp;_tmp263[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp263[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp263,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp262->tl=new_args;
_tmp262;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp205);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp205,new_args,0),0);if(_tmp20B->inject){struct Cyc_Absyn_Tuniondecl*
tud;{void*_tmp264=Cyc_Tcutil_compress((void*)_tmp20B->type);struct Cyc_Absyn_TunionInfo
_tmp265;void*_tmp266;struct Cyc_Absyn_Tuniondecl**_tmp267;struct Cyc_Absyn_Tuniondecl*
_tmp268;_LL215: if(_tmp264 <= (void*)3?1:*((int*)_tmp264)!= 2)goto _LL217;_tmp265=((
struct Cyc_Absyn_TunionType_struct*)_tmp264)->f1;_tmp266=(void*)_tmp265.tunion_info;
if(*((int*)_tmp266)!= 1)goto _LL217;_tmp267=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp266)->f1;_tmp268=*_tmp267;_LL216: tud=_tmp268;goto _LL214;_LL217:;_LL218:({
void*_tmp269[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("toc: unknown tunion in vararg with inject",sizeof(unsigned char),42),
_tag_arr(_tmp269,sizeof(void*),0));});_LL214:;}{struct _tagged_arr vs=({
unsigned int _tmp270=(unsigned int)_tmp209;struct _tuple1**_tmp271=(struct _tuple1**)
_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp270));struct _tagged_arr
_tmp273=_tag_arr(_tmp271,sizeof(struct _tuple1*),(unsigned int)_tmp209);{
unsigned int _tmp272=_tmp270;unsigned int i;for(i=0;i < _tmp272;i ++){_tmp271[i]=Cyc_Toc_temp_var();}}
_tmp273;});struct Cyc_List_List*_tmp26A=0;{int i=_tmp209 - 1;for(0;i >= 0;-- i){
_tmp26A=({struct Cyc_List_List*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp26B->tl=
_tmp26A;_tmp26B;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp26A,0),s,0);{int i=0;for(0;_tmp206 != 0;(((
_tmp206=_tmp206->tl,_tmp20A=_tmp20A->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp206->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp26D;struct _tuple1*
_tmp26E;struct Cyc_List_List*_tmp26F;struct Cyc_Absyn_Tunionfield*_tmp26C=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp20A))->hd;_tmp26D=*
_tmp26C;_tmp26E=_tmp26D.name;_tmp26F=_tmp26D.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp26F))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(field_typ,arg,
0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,
Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp26E,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp26E,_tag_arr("_struct",sizeof(unsigned char),8))),0,s,0);}}}}}else{{int i=0;
for(0;_tmp206 != 0;(_tmp206=_tmp206->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp206->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp206->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL192;_LL1AB: if(*((
int*)_tmp1EF)!= 10)goto _LL1AD;_tmp20C=((struct Cyc_Absyn_Throw_e_struct*)_tmp1EF)->f1;
_LL1AC: Cyc_Toc_exp_to_c(nv,_tmp20C);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp20C),0))->r));goto _LL192;
_LL1AD: if(*((int*)_tmp1EF)!= 11)goto _LL1AF;_tmp20D=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1EF)->f1;_LL1AE: Cyc_Toc_exp_to_c(nv,_tmp20D);goto _LL192;_LL1AF: if(*((int*)
_tmp1EF)!= 12)goto _LL1B1;_tmp20E=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1EF)->f1;
_tmp20F=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1EF)->f2;_LL1B0: Cyc_Toc_exp_to_c(
nv,_tmp20E);for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp20F->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp276=Cyc_Tcutil_compress((
void*)_tmp20F->hd);_LL21A: if(_tmp276 <= (void*)3?1:*((int*)_tmp276)!= 1)goto
_LL21C;_LL21B: goto _LL21D;_LL21C: if(_tmp276 <= (void*)3?1:*((int*)_tmp276)!= 2)
goto _LL21E;_LL21D: continue;_LL21E:;_LL21F:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp20E,
0))->r));goto _LL219;_LL219:;}break;}}goto _LL192;_LL1B1: if(*((int*)_tmp1EF)!= 13)
goto _LL1B3;_tmp210=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1EF)->f1;_tmp211=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1EF)->f1);_tmp212=((struct
Cyc_Absyn_Cast_e_struct*)_tmp1EF)->f2;_LL1B2: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp212->topt))->v;void*new_typ=*_tmp211;*_tmp211=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp212);{struct _tuple0 _tmp278=({struct _tuple0
_tmp277;_tmp277.f1=Cyc_Tcutil_compress(old_t2);_tmp277.f2=Cyc_Tcutil_compress(
new_typ);_tmp277;});void*_tmp279;struct Cyc_Absyn_PtrInfo _tmp27A;void*_tmp27B;
struct Cyc_Absyn_PtrInfo _tmp27C;void*_tmp27D;struct Cyc_Absyn_PtrInfo _tmp27E;void*
_tmp27F;_LL221: _tmp279=_tmp278.f1;if(_tmp279 <= (void*)3?1:*((int*)_tmp279)!= 4)
goto _LL223;_tmp27A=((struct Cyc_Absyn_PointerType_struct*)_tmp279)->f1;_tmp27B=
_tmp278.f2;if(_tmp27B <= (void*)3?1:*((int*)_tmp27B)!= 4)goto _LL223;_tmp27C=((
struct Cyc_Absyn_PointerType_struct*)_tmp27B)->f1;_LL222: {int _tmp280=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp27A.nullable);int
_tmp281=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp27C.nullable);
void*_tmp282=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27A.bounds);void*
_tmp283=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27C.bounds);{struct _tuple0
_tmp285=({struct _tuple0 _tmp284;_tmp284.f1=_tmp282;_tmp284.f2=_tmp283;_tmp284;});
void*_tmp286;void*_tmp287;void*_tmp288;struct Cyc_Absyn_Exp*_tmp289;void*_tmp28A;
void*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Exp*_tmp28D;_LL228: _tmp286=_tmp285.f1;
if(_tmp286 <= (void*)1?1:*((int*)_tmp286)!= 0)goto _LL22A;_tmp287=_tmp285.f2;if(
_tmp287 <= (void*)1?1:*((int*)_tmp287)!= 0)goto _LL22A;_LL229: if(_tmp280?!_tmp281:
0){if(nv->toplevel)({void*_tmp28E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(unsigned char),44),_tag_arr(_tmp28E,sizeof(void*),0));});(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(*_tmp211,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->hd=_tmp212;
_tmp28F->tl=0;_tmp28F;}),0),0))->r));}goto _LL227;_LL22A: _tmp288=_tmp285.f1;if(
_tmp288 <= (void*)1?1:*((int*)_tmp288)!= 0)goto _LL22C;_tmp289=((struct Cyc_Absyn_Upper_b_struct*)
_tmp288)->f1;_tmp28A=_tmp285.f2;if((int)_tmp28A != 0)goto _LL22C;_LL22B: if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp289,
_tmp212))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp290[3];_tmp290[2]=_tmp289;_tmp290[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp27C.elt_typ),0);_tmp290[0]=_tmp212;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp290,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL227;_LL22C: _tmp28B=_tmp285.f1;if((int)_tmp28B != 0)goto
_LL22E;_tmp28C=_tmp285.f2;if(_tmp28C <= (void*)1?1:*((int*)_tmp28C)!= 0)goto
_LL22E;_tmp28D=((struct Cyc_Absyn_Upper_b_struct*)_tmp28C)->f1;_LL22D: if(nv->toplevel)({
void*_tmp291[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level",sizeof(unsigned char),45),
_tag_arr(_tmp291,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp292=Cyc_Absyn_fncall_exp(
Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp294[3];_tmp294[2]=_tmp28D;_tmp294[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp27A.elt_typ),0);_tmp294[0]=
_tmp212;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp294,sizeof(struct Cyc_Absyn_Exp*),3));}),0);if(_tmp281)(void*)(_tmp292->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->hd=Cyc_Absyn_copy_exp(_tmp292);
_tmp293->tl=0;_tmp293;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp211,_tmp292,0))->r));goto _LL227;}_LL22E:;_LL22F: goto _LL227;_LL227:;}goto
_LL220;}_LL223: _tmp27D=_tmp278.f1;if(_tmp27D <= (void*)3?1:*((int*)_tmp27D)!= 4)
goto _LL225;_tmp27E=((struct Cyc_Absyn_PointerType_struct*)_tmp27D)->f1;_tmp27F=
_tmp278.f2;if(_tmp27F <= (void*)3?1:*((int*)_tmp27F)!= 5)goto _LL225;_LL224:{void*
_tmp295=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27E.bounds);_LL231: if((int)
_tmp295 != 0)goto _LL233;_LL232:(void*)(_tmp212->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)_tmp212->r,_tmp212->loc),Cyc_Toc_curr_sp,0))->r));goto
_LL230;_LL233:;_LL234: goto _LL230;_LL230:;}goto _LL220;_LL225:;_LL226: goto _LL220;
_LL220:;}goto _LL192;}_LL1B3: if(*((int*)_tmp1EF)!= 14)goto _LL1B5;_tmp213=((struct
Cyc_Absyn_Address_e_struct*)_tmp1EF)->f1;_LL1B4:{void*_tmp296=(void*)_tmp213->r;
struct _tuple1*_tmp297;struct Cyc_List_List*_tmp298;struct Cyc_List_List*_tmp299;
struct Cyc_List_List*_tmp29A;_LL236: if(*((int*)_tmp296)!= 28)goto _LL238;_tmp297=((
struct Cyc_Absyn_Struct_e_struct*)_tmp296)->f1;_tmp298=((struct Cyc_Absyn_Struct_e_struct*)
_tmp296)->f2;_tmp299=((struct Cyc_Absyn_Struct_e_struct*)_tmp296)->f3;_LL237: if(
nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp29C;_tmp29C.tag=0;_tmp29C.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp213->loc);{void*_tmp29B[1]={&
_tmp29C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp29B,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp213->topt))->v,_tmp298 != 0,1,0,
_tmp299,_tmp297))->r));goto _LL235;_LL238: if(*((int*)_tmp296)!= 24)goto _LL23A;
_tmp29A=((struct Cyc_Absyn_Tuple_e_struct*)_tmp296)->f1;_LL239: if(nv->toplevel)({
struct Cyc_Std_String_pa_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp213->loc);{void*_tmp29D[1]={&
_tmp29E};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp29D,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp29A))->r));goto _LL235;_LL23A:;_LL23B: Cyc_Toc_exp_to_c(nv,_tmp213);if(
!Cyc_Absyn_is_lvalue(_tmp213)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp213,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp213,
0))->r));}goto _LL235;_LL235:;}goto _LL192;_LL1B5: if(*((int*)_tmp1EF)!= 15)goto
_LL1B7;_tmp214=((struct Cyc_Absyn_New_e_struct*)_tmp1EF)->f1;_tmp215=((struct Cyc_Absyn_New_e_struct*)
_tmp1EF)->f2;_LL1B6: if(nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp2A0;
_tmp2A0.tag=0;_tmp2A0.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp215->loc);{void*_tmp29F[1]={& _tmp2A0};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: new at top-level",sizeof(
unsigned char),21),_tag_arr(_tmp29F,sizeof(void*),1));}});{void*_tmp2A1=(void*)
_tmp215->r;struct Cyc_List_List*_tmp2A2;struct Cyc_Absyn_Vardecl*_tmp2A3;struct Cyc_Absyn_Exp*
_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;struct _tuple1*_tmp2A6;struct Cyc_List_List*
_tmp2A7;struct Cyc_List_List*_tmp2A8;struct Cyc_List_List*_tmp2A9;_LL23D: if(*((int*)
_tmp2A1)!= 26)goto _LL23F;_tmp2A2=((struct Cyc_Absyn_Array_e_struct*)_tmp2A1)->f1;
_LL23E: {struct _tuple1*_tmp2AA=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2AB=
Cyc_Absyn_var_exp(_tmp2AA,0);struct Cyc_Absyn_Stmt*_tmp2AC=Cyc_Toc_init_array(nv,
_tmp2AB,_tmp2A2,Cyc_Absyn_exp_stmt(_tmp2AB,0));void*old_elt_typ;{void*_tmp2AD=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp2AE;void*_tmp2AF;struct
Cyc_Absyn_Tqual _tmp2B0;_LL248: if(_tmp2AD <= (void*)3?1:*((int*)_tmp2AD)!= 4)goto
_LL24A;_tmp2AE=((struct Cyc_Absyn_PointerType_struct*)_tmp2AD)->f1;_tmp2AF=(void*)
_tmp2AE.elt_typ;_tmp2B0=_tmp2AE.tq;_LL249: old_elt_typ=_tmp2AF;goto _LL247;_LL24A:;
_LL24B: old_elt_typ=({void*_tmp2B1[0]={};Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(unsigned char),52),_tag_arr(_tmp2B1,sizeof(void*),0));});_LL247:;}{void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp2B2=Cyc_Absyn_cstar_typ(elt_typ,
Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2A2),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp214 == 0)e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp2B3);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp214);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp2B3);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2AA,
_tmp2B2,(struct Cyc_Absyn_Exp*)e1,_tmp2AC,0),0))->r));goto _LL23C;}}_LL23F: if(*((
int*)_tmp2A1)!= 27)goto _LL241;_tmp2A3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2A1)->f1;_tmp2A4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A1)->f2;
_tmp2A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A1)->f3;_LL240: {int
is_tagged_ptr=0;{void*_tmp2B4=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp2B5;void*_tmp2B6;struct Cyc_Absyn_Tqual _tmp2B7;struct Cyc_Absyn_Conref*_tmp2B8;
_LL24D: if(_tmp2B4 <= (void*)3?1:*((int*)_tmp2B4)!= 4)goto _LL24F;_tmp2B5=((struct
Cyc_Absyn_PointerType_struct*)_tmp2B4)->f1;_tmp2B6=(void*)_tmp2B5.elt_typ;
_tmp2B7=_tmp2B5.tq;_tmp2B8=_tmp2B5.bounds;_LL24E: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2B8)== (void*)0;goto _LL24C;_LL24F:;_LL250:({void*
_tmp2B9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type",sizeof(unsigned char),42),
_tag_arr(_tmp2B9,sizeof(void*),0));});_LL24C:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A5->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp2A4);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),
_tmp2A3,Cyc_Absyn_var_exp(max,0),_tmp2A5,Cyc_Toc_skip_stmt_dl(),1);{struct
_RegionHandle _tmp2BA=_new_region("r");struct _RegionHandle*r=& _tmp2BA;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp2CA=
_region_malloc(r,sizeof(*_tmp2CA));_tmp2CA->hd=({struct _tuple9*_tmp2CB=
_region_malloc(r,sizeof(*_tmp2CB));_tmp2CB->f1=max;_tmp2CB->f2=Cyc_Absyn_uint_typ;
_tmp2CB->f3=(struct Cyc_Absyn_Exp*)_tmp2A4;_tmp2CB;});_tmp2CA->tl=0;_tmp2CA;});
struct Cyc_Absyn_Exp*ai;if(_tmp214 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp2BB[2];_tmp2BB[1]=Cyc_Absyn_var_exp(
max,0);_tmp2BB[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2BB,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp214);
Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp2BC[2];_tmp2BC[1]=Cyc_Absyn_var_exp(max,0);_tmp2BC[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp2BC,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct
Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(ptr_typ,ai,0);decls=({struct Cyc_List_List*
_tmp2BD=_region_malloc(r,sizeof(*_tmp2BD));_tmp2BD->hd=({struct _tuple9*_tmp2BE=
_region_malloc(r,sizeof(*_tmp2BE));_tmp2BE->f1=a;_tmp2BE->f2=ptr_typ;_tmp2BE->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp2BE;});_tmp2BD->tl=decls;_tmp2BD;});if(
is_tagged_ptr){struct _tuple1*_tmp2BF=Cyc_Toc_temp_var();void*_tmp2C0=Cyc_Toc_typ_to_c(
old_typ);struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2C4[3];_tmp2C4[2]=_tmp2A4;_tmp2C4[1]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);_tmp2C4[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2C4,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);decls=({struct Cyc_List_List*_tmp2C2=_region_malloc(r,sizeof(*_tmp2C2));
_tmp2C2->hd=({struct _tuple9*_tmp2C3=_region_malloc(r,sizeof(*_tmp2C3));_tmp2C3->f1=
_tmp2BF;_tmp2C3->f2=_tmp2C0;_tmp2C3->f3=(struct Cyc_Absyn_Exp*)_tmp2C1;_tmp2C3;});
_tmp2C2->tl=decls;_tmp2C2;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp2BF,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp2C5=decls;for(0;_tmp2C5 != 0;_tmp2C5=_tmp2C5->tl){
struct _tuple1*_tmp2C7;void*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C9;struct _tuple9
_tmp2C6=*((struct _tuple9*)_tmp2C5->hd);_tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;
_tmp2C9=_tmp2C6.f3;s=Cyc_Absyn_declare_stmt(_tmp2C7,_tmp2C8,_tmp2C9,s,0);}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL23C;}}}
_LL241: if(*((int*)_tmp2A1)!= 28)goto _LL243;_tmp2A6=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2A1)->f1;_tmp2A7=((struct Cyc_Absyn_Struct_e_struct*)_tmp2A1)->f2;_tmp2A8=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2A1)->f3;_LL242:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp215->topt))->v,
_tmp2A7 != 0,1,_tmp214,_tmp2A8,_tmp2A6))->r));goto _LL23C;_LL243: if(*((int*)
_tmp2A1)!= 24)goto _LL245;_tmp2A9=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2A1)->f1;
_LL244:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp214,_tmp2A9))->r));
goto _LL23C;_LL245:;_LL246: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp215->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp2CC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2CD=Cyc_Absyn_var_exp(
_tmp2CC,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp2CD,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp214 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp214);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp2CE=(void*)_tmp215->r;void*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;_LL252:
if(*((int*)_tmp2CE)!= 13)goto _LL254;_tmp2CF=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2CE)->f1;_tmp2D0=((struct Cyc_Absyn_Cast_e_struct*)_tmp2CE)->f2;_LL253:{
struct _tuple0 _tmp2D2=({struct _tuple0 _tmp2D1;_tmp2D1.f1=Cyc_Tcutil_compress(
_tmp2CF);_tmp2D1.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D0->topt))->v);_tmp2D1;});void*_tmp2D3;struct Cyc_Absyn_PtrInfo _tmp2D4;void*
_tmp2D5;struct Cyc_Absyn_Conref*_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_PtrInfo
_tmp2D8;struct Cyc_Absyn_Conref*_tmp2D9;_LL257: _tmp2D3=_tmp2D2.f1;if(_tmp2D3 <= (
void*)3?1:*((int*)_tmp2D3)!= 4)goto _LL259;_tmp2D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D3)->f1;_tmp2D5=(void*)_tmp2D4.elt_typ;_tmp2D6=_tmp2D4.bounds;_tmp2D7=
_tmp2D2.f2;if(_tmp2D7 <= (void*)3?1:*((int*)_tmp2D7)!= 4)goto _LL259;_tmp2D8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D7)->f1;_tmp2D9=_tmp2D8.bounds;_LL258:{
struct _tuple0 _tmp2DB=({struct _tuple0 _tmp2DA;_tmp2DA.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2D6);_tmp2DA.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2D9);_tmp2DA;});
void*_tmp2DC;void*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;_LL25C: _tmp2DC=_tmp2DB.f1;
if((int)_tmp2DC != 0)goto _LL25E;_tmp2DD=_tmp2DB.f2;if(_tmp2DD <= (void*)1?1:*((int*)
_tmp2DD)!= 0)goto _LL25E;_tmp2DE=((struct Cyc_Absyn_Upper_b_struct*)_tmp2DD)->f1;
_LL25D: Cyc_Toc_exp_to_c(nv,_tmp2D0);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2DF[4];_tmp2DF[3]=_tmp2DE;_tmp2DF[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp2D5),0);_tmp2DF[1]=_tmp2D0;_tmp2DF[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2DF,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL25B;_LL25E:;_LL25F: goto _LL25B;_LL25B:;}goto _LL256;_LL259:;
_LL25A: goto _LL256;_LL256:;}goto _LL251;_LL254:;_LL255: goto _LL251;_LL251:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp2E0=Cyc_Absyn_exp_stmt(_tmp2CD,0);struct Cyc_Absyn_Exp*
_tmp2E1=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp215);_tmp2E0=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp2CD,_tmp2E1,0),_tmp215,0),
_tmp2E0,0);{void*_tmp2E2=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2CC,_tmp2E2,(struct
Cyc_Absyn_Exp*)mexp,_tmp2E0,0),0))->r));}}goto _LL23C;}}_LL23C:;}goto _LL192;
_LL1B7: if(*((int*)_tmp1EF)!= 17)goto _LL1B9;_tmp216=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1EF)->f1;_LL1B8: Cyc_Toc_exp_to_c(nv,_tmp216);goto _LL192;_LL1B9: if(*((int*)
_tmp1EF)!= 16)goto _LL1BB;_tmp217=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1EF)->f1;_LL1BA:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp2E4;_tmp2E4.tag=16;_tmp2E4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp217);_tmp2E4;});
_tmp2E3;})));goto _LL192;_LL1BB: if(*((int*)_tmp1EF)!= 19)goto _LL1BD;_LL1BC:({void*
_tmp2E5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("__gen() in code generator",sizeof(unsigned char),26),_tag_arr(_tmp2E5,
sizeof(void*),0));});_LL1BD: if(*((int*)_tmp1EF)!= 18)goto _LL1BF;_tmp218=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1EF)->f1;_tmp219=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1EF)->f2;if(*((int*)_tmp219)!= 0)goto _LL1BF;_tmp21A=((struct Cyc_Absyn_StructField_struct*)
_tmp219)->f1;_LL1BE:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2E6=_cycalloc(sizeof(*_tmp2E6));_tmp2E6[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2E7;_tmp2E7.tag=18;_tmp2E7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp218);_tmp2E7.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2E8=_cycalloc(sizeof(*
_tmp2E8));_tmp2E8[0]=({struct Cyc_Absyn_StructField_struct _tmp2E9;_tmp2E9.tag=0;
_tmp2E9.f1=_tmp21A;_tmp2E9;});_tmp2E8;}));_tmp2E7;});_tmp2E6;})));goto _LL192;
_LL1BF: if(*((int*)_tmp1EF)!= 18)goto _LL1C1;_tmp21B=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1EF)->f1;_tmp21C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1EF)->f2;
if(*((int*)_tmp21C)!= 1)goto _LL1C1;_tmp21D=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp21C)->f1;_LL1C0:{void*_tmp2EA=Cyc_Tcutil_compress(_tmp21B);struct Cyc_Absyn_AggrInfo
_tmp2EB;void*_tmp2EC;struct Cyc_List_List*_tmp2ED;_LL261: if(_tmp2EA <= (void*)3?1:*((
int*)_tmp2EA)!= 10)goto _LL263;_tmp2EB=((struct Cyc_Absyn_AggrType_struct*)_tmp2EA)->f1;
_tmp2EC=(void*)_tmp2EB.aggr_info;_LL262: {struct Cyc_Absyn_Aggrdecl*_tmp2EE=Cyc_Absyn_get_known_aggrdecl(
_tmp2EC);if(_tmp2EE->impl == 0)({void*_tmp2EF[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("struct fields must be known",
sizeof(unsigned char),28),_tag_arr(_tmp2EF,sizeof(void*),0));});_tmp2ED=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2EE->impl))->fields;goto _LL264;}_LL263:
if(_tmp2EA <= (void*)3?1:*((int*)_tmp2EA)!= 11)goto _LL265;_tmp2ED=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2EA)->f2;_LL264: {struct Cyc_Absyn_Aggrfield*_tmp2F0=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2ED,(int)_tmp21D);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2F1=_cycalloc(sizeof(*
_tmp2F1));_tmp2F1[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2F2;_tmp2F2.tag=18;
_tmp2F2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp21B);_tmp2F2.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3[0]=({
struct Cyc_Absyn_StructField_struct _tmp2F4;_tmp2F4.tag=0;_tmp2F4.f1=_tmp2F0->name;
_tmp2F4;});_tmp2F3;}));_tmp2F2;});_tmp2F1;})));goto _LL260;}_LL265: if(_tmp2EA <= (
void*)3?1:*((int*)_tmp2EA)!= 9)goto _LL267;_LL266:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp2F6;_tmp2F6.tag=18;_tmp2F6.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp21B);_tmp2F6.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2F7=
_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct Cyc_Absyn_StructField_struct
_tmp2F8;_tmp2F8.tag=0;_tmp2F8.f1=Cyc_Absyn_fieldname((int)(_tmp21D + 1));_tmp2F8;});
_tmp2F7;}));_tmp2F6;});_tmp2F5;})));goto _LL260;_LL267: if(_tmp2EA <= (void*)3?1:*((
int*)_tmp2EA)!= 3)goto _LL269;_LL268: if(_tmp21D == 0)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp2FA;_tmp2FA.tag=18;_tmp2FA.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp21B);_tmp2FA.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2FB=
_cycalloc(sizeof(*_tmp2FB));_tmp2FB[0]=({struct Cyc_Absyn_StructField_struct
_tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=Cyc_Toc_tag_sp;_tmp2FC;});_tmp2FB;}));_tmp2FA;});
_tmp2F9;})));else{(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2FE;_tmp2FE.tag=18;_tmp2FE.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp21B);_tmp2FE.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2FF=_cycalloc(sizeof(*
_tmp2FF));_tmp2FF[0]=({struct Cyc_Absyn_StructField_struct _tmp300;_tmp300.tag=0;
_tmp300.f1=Cyc_Absyn_fieldname((int)_tmp21D);_tmp300;});_tmp2FF;}));_tmp2FE;});
_tmp2FD;})));}goto _LL260;_LL269:;_LL26A:({void*_tmp301[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("impossible type for offsetof tuple index",
sizeof(unsigned char),41),_tag_arr(_tmp301,sizeof(void*),0));});_LL260:;}goto
_LL192;_LL1C1: if(*((int*)_tmp1EF)!= 20)goto _LL1C3;_tmp21E=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1EF)->f1;_LL1C2: {void*_tmp302=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp21E->topt))->v);{void*_tmp303=_tmp302;struct Cyc_Absyn_PtrInfo
_tmp304;void*_tmp305;void*_tmp306;struct Cyc_Absyn_Conref*_tmp307;struct Cyc_Absyn_Tqual
_tmp308;struct Cyc_Absyn_Conref*_tmp309;_LL26C: if(_tmp303 <= (void*)3?1:*((int*)
_tmp303)!= 4)goto _LL26E;_tmp304=((struct Cyc_Absyn_PointerType_struct*)_tmp303)->f1;
_tmp305=(void*)_tmp304.elt_typ;_tmp306=(void*)_tmp304.rgn_typ;_tmp307=_tmp304.nullable;
_tmp308=_tmp304.tq;_tmp309=_tmp304.bounds;_LL26D:{void*_tmp30A=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp309);_LL271: if(_tmp30A <= (void*)1?1:*((int*)_tmp30A)!= 
0)goto _LL273;_LL272: {int do_null_check=Cyc_Toc_need_null_check(_tmp21E);Cyc_Toc_exp_to_c(
nv,_tmp21E);if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21E->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->hd=_tmp21E;_tmp30B->tl=0;_tmp30B;}),
0),0),0))->r));goto _LL270;}_LL273: if((int)_tmp30A != 0)goto _LL270;_LL274: {struct
Cyc_Absyn_Exp*_tmp30C=Cyc_Absyn_uint_exp(0,0);_tmp30C->topt=({struct Cyc_Core_Opt*
_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->v=(void*)Cyc_Absyn_uint_typ;_tmp30D;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp21E,_tmp30C,0))->r));Cyc_Toc_exp_to_c(
nv,e);goto _LL270;}_LL270:;}goto _LL26B;_LL26E:;_LL26F:({void*_tmp30E[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Deref: non-pointer",
sizeof(unsigned char),29),_tag_arr(_tmp30E,sizeof(void*),0));});_LL26B:;}goto
_LL192;}_LL1C3: if(*((int*)_tmp1EF)!= 21)goto _LL1C5;_tmp21F=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1EF)->f1;_tmp220=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1EF)->f2;_LL1C4:
Cyc_Toc_exp_to_c(nv,_tmp21F);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL192;
_LL1C5: if(*((int*)_tmp1EF)!= 22)goto _LL1C7;_tmp221=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1EF)->f1;_tmp222=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1EF)->f2;_LL1C6: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp221->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp221);Cyc_Toc_exp_to_c(nv,_tmp221);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp310;void*_tmp311;struct Cyc_Absyn_Conref*
_tmp312;struct Cyc_Absyn_Tqual _tmp313;struct Cyc_Absyn_Conref*_tmp314;struct Cyc_Absyn_PtrInfo
_tmp30F=Cyc_Toc_get_ptr_type(e1typ);_tmp310=(void*)_tmp30F.elt_typ;_tmp311=(void*)
_tmp30F.rgn_typ;_tmp312=_tmp30F.nullable;_tmp313=_tmp30F.tq;_tmp314=_tmp30F.bounds;{
void*_tmp315=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp314);struct Cyc_Absyn_Exp*
_tmp316;_LL276: if(_tmp315 <= (void*)1?1:*((int*)_tmp315)!= 0)goto _LL278;_tmp316=((
struct Cyc_Absyn_Upper_b_struct*)_tmp315)->f1;_LL277: if(Cyc_Evexp_eval_const_uint_exp(
_tmp316)< 1)({void*_tmp317[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("exp_to_c:  AggrArrow_e on pointer of size 0",
sizeof(unsigned char),44),_tag_arr(_tmp317,sizeof(void*),0));});if(do_null_check)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp221->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp318=_cycalloc(sizeof(*_tmp318));
_tmp318->hd=_tmp221;_tmp318->tl=0;_tmp318;}),0),0),_tmp222,0))->r));goto _LL275;
_LL278: if((int)_tmp315 != 0)goto _LL275;_LL279: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp310);(void*)(_tmp221->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp313),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp319[3];_tmp319[2]=Cyc_Absyn_uint_exp(0,0);_tmp319[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp319[0]=Cyc_Absyn_copy_exp(_tmp221);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp319,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL275;}_LL275:;}if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));goto _LL192;}}_LL1C7: if(*((int*)_tmp1EF)!= 23)goto _LL1C9;
_tmp223=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1EF)->f1;_tmp224=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1EF)->f2;_LL1C8: {void*_tmp31A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp223->topt))->v);{void*_tmp31B=_tmp31A;struct Cyc_List_List*
_tmp31C;struct Cyc_Absyn_PtrInfo _tmp31D;void*_tmp31E;void*_tmp31F;struct Cyc_Absyn_Conref*
_tmp320;struct Cyc_Absyn_Tqual _tmp321;struct Cyc_Absyn_Conref*_tmp322;_LL27B: if(
_tmp31B <= (void*)3?1:*((int*)_tmp31B)!= 9)goto _LL27D;_tmp31C=((struct Cyc_Absyn_TupleType_struct*)
_tmp31B)->f1;_LL27C: Cyc_Toc_exp_to_c(nv,_tmp223);Cyc_Toc_exp_to_c(nv,_tmp224);{
unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp224)+ 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(_tmp223,Cyc_Absyn_fieldname((int)i),0))->r));
goto _LL27A;}_LL27D: if(_tmp31B <= (void*)3?1:*((int*)_tmp31B)!= 4)goto _LL27F;
_tmp31D=((struct Cyc_Absyn_PointerType_struct*)_tmp31B)->f1;_tmp31E=(void*)
_tmp31D.elt_typ;_tmp31F=(void*)_tmp31D.rgn_typ;_tmp320=_tmp31D.nullable;_tmp321=
_tmp31D.tq;_tmp322=_tmp31D.bounds;_LL27E: {struct Cyc_List_List*_tmp323=Cyc_Toc_get_relns(
_tmp223);int _tmp324=Cyc_Toc_check_bounds(_tmp323,_tmp223,_tmp224);if(Cyc_Toc_warn_bounds_checks?
!_tmp324: 0)({struct Cyc_Std_String_pa_struct _tmp326;_tmp326.tag=0;_tmp326.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp325[1]={& _tmp326};Cyc_Tcutil_warn(
e->loc,_tag_arr("bounds check necessary for %s",sizeof(unsigned char),30),
_tag_arr(_tmp325,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp223);Cyc_Toc_exp_to_c(
nv,_tmp224);Cyc_Toc_total_bounds_checks ++;{void*_tmp327=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp322);struct Cyc_Absyn_Exp*_tmp328;_LL282: if(_tmp327 <= (void*)1?1:*((int*)
_tmp327)!= 0)goto _LL284;_tmp328=((struct Cyc_Absyn_Upper_b_struct*)_tmp327)->f1;
_LL283: _tmp328=Cyc_Absyn_uint_exp(Cyc_Evexp_eval_const_uint_exp(_tmp328),0);{int
possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp320);void*ta1=Cyc_Toc_typ_to_c(_tmp31E);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp321);if(_tmp324)Cyc_Toc_bounds_checks_eliminated ++;else{if(possibly_null)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp329[4];_tmp329[3]=
_tmp224;_tmp329[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp329[1]=_tmp328;_tmp329[0]=
_tmp223;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp329,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));else{(void*)(_tmp224->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp32A[2];_tmp32A[1]=Cyc_Absyn_copy_exp(_tmp224);_tmp32A[0]=
_tmp328;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp32A,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL281;}_LL284: if((
int)_tmp327 != 0)goto _LL281;_LL285: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp31E);if(
_tmp324){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp321),Cyc_Absyn_aggrmember_exp(
_tmp223,Cyc_Toc_curr_sp,0),0),_tmp224,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp321),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp32B[3];_tmp32B[2]=
_tmp224;_tmp32B[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp32B[0]=_tmp223;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL281;}_LL281:;}goto _LL27A;}_LL27F:;_LL280:({void*
_tmp32C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",sizeof(unsigned char),
49),_tag_arr(_tmp32C,sizeof(void*),0));});_LL27A:;}goto _LL192;}_LL1C9: if(*((int*)
_tmp1EF)!= 24)goto _LL1CB;_tmp225=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1EF)->f1;
_LL1CA: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp225))->r));else{struct Cyc_List_List*_tmp32D=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp225);struct _tagged_arr*_tmp32E=Cyc_Toc_add_tuple_type(_tmp32D);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp225 != 0;(_tmp225=_tmp225->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp225->hd);dles=({struct Cyc_List_List*_tmp32F=_cycalloc(
sizeof(*_tmp32F));_tmp32F->hd=({struct _tuple5*_tmp330=_cycalloc(sizeof(*_tmp330));
_tmp330->f1=0;_tmp330->f2=(struct Cyc_Absyn_Exp*)_tmp225->hd;_tmp330;});_tmp32F->tl=
dles;_tmp32F;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL192;_LL1CB: if(*((int*)_tmp1EF)!= 26)goto _LL1CD;_tmp226=((struct Cyc_Absyn_Array_e_struct*)
_tmp1EF)->f1;_LL1CC:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp226,0))->r));{struct Cyc_List_List*_tmp331=_tmp226;for(0;_tmp331 != 0;_tmp331=
_tmp331->tl){struct _tuple5 _tmp333;struct Cyc_Absyn_Exp*_tmp334;struct _tuple5*
_tmp332=(struct _tuple5*)_tmp331->hd;_tmp333=*_tmp332;_tmp334=_tmp333.f2;Cyc_Toc_exp_to_c(
nv,_tmp334);}}goto _LL192;_LL1CD: if(*((int*)_tmp1EF)!= 27)goto _LL1CF;_tmp227=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EF)->f1;_tmp228=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EF)->f2;_tmp229=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EF)->f3;
_LL1CE: {unsigned int _tmp335=Cyc_Evexp_eval_const_uint_exp(_tmp228);Cyc_Toc_exp_to_c(
nv,_tmp229);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp229)){unsigned int
i=0;for(0;i < _tmp335;i ++){es=({struct Cyc_List_List*_tmp336=_cycalloc(sizeof(*
_tmp336));_tmp336->hd=({struct _tuple5*_tmp337=_cycalloc(sizeof(*_tmp337));
_tmp337->f1=0;_tmp337->f2=_tmp229;_tmp337;});_tmp336->tl=es;_tmp336;});}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));goto _LL192;}}
_LL1CF: if(*((int*)_tmp1EF)!= 28)goto _LL1D1;_tmp22A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1EF)->f1;_tmp22B=((struct Cyc_Absyn_Struct_e_struct*)_tmp1EF)->f2;_tmp22C=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1EF)->f3;_tmp22D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1EF)->f4;_LL1D0: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp22B != 0,0,0,_tmp22C,_tmp22A))->r));else{if(_tmp22D == 0)({void*
_tmp338[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing aggrdecl pointer",sizeof(unsigned char),35),_tag_arr(
_tmp338,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)
_check_null(_tmp22D);struct _RegionHandle _tmp339=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp339;_push_region(rgn);{struct Cyc_List_List*_tmp33A=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp22C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
struct Cyc_List_List*_tmp33B=0;struct Cyc_List_List*_tmp33C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp33C != 0;_tmp33C=_tmp33C->tl){struct Cyc_List_List*
_tmp33D=_tmp33A;for(0;_tmp33D != 0;_tmp33D=_tmp33D->tl){if((*((struct _tuple10*)
_tmp33D->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp33C->hd){struct _tuple10
_tmp33F;struct Cyc_Absyn_Aggrfield*_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct
_tuple10*_tmp33E=(struct _tuple10*)_tmp33D->hd;_tmp33F=*_tmp33E;_tmp340=_tmp33F.f1;
_tmp341=_tmp33F.f2;{void*_tmp342=(void*)_tmp340->type;Cyc_Toc_exp_to_c(nv,
_tmp341);if(Cyc_Toc_is_void_star(_tmp342))(void*)(_tmp341->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp341->r,0),0))->r));
_tmp33B=({struct Cyc_List_List*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->hd=({
struct _tuple5*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344->f1=0;_tmp344->f2=
_tmp341;_tmp344;});_tmp343->tl=_tmp33B;_tmp343;});break;}}}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp33B),0))->r));};_pop_region(rgn);}}goto _LL192;_LL1D1:
if(*((int*)_tmp1EF)!= 29)goto _LL1D3;_tmp22E=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1EF)->f1;_tmp22F=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1EF)->f2;_LL1D2: {
struct Cyc_List_List*fs;{void*_tmp345=Cyc_Tcutil_compress(_tmp22E);struct Cyc_List_List*
_tmp346;_LL287: if(_tmp345 <= (void*)3?1:*((int*)_tmp345)!= 11)goto _LL289;_tmp346=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp345)->f2;_LL288: fs=_tmp346;goto _LL286;
_LL289:;_LL28A:({struct Cyc_Std_String_pa_struct _tmp348;_tmp348.tag=0;_tmp348.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp22E);{void*_tmp347[1]={& _tmp348};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("anon struct has type %s",
sizeof(unsigned char),24),_tag_arr(_tmp347,sizeof(void*),1));}});_LL286:;}{
struct _RegionHandle _tmp349=_new_region("rgn");struct _RegionHandle*rgn=& _tmp349;
_push_region(rgn);{struct Cyc_List_List*_tmp34A=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp22F,
fs);for(0;_tmp34A != 0;_tmp34A=_tmp34A->tl){struct _tuple10 _tmp34C;struct Cyc_Absyn_Aggrfield*
_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;struct _tuple10*_tmp34B=(struct _tuple10*)
_tmp34A->hd;_tmp34C=*_tmp34B;_tmp34D=_tmp34C.f1;_tmp34E=_tmp34C.f2;{void*_tmp34F=(
void*)_tmp34D->type;Cyc_Toc_exp_to_c(nv,_tmp34E);if(Cyc_Toc_is_void_star(_tmp34F))(
void*)(_tmp34E->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp34E->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp22F,0))->r));};_pop_region(rgn);}goto _LL192;}
_LL1D3: if(*((int*)_tmp1EF)!= 30)goto _LL1D5;_tmp230=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1EF)->f1;if(_tmp230 != 0)goto _LL1D5;_tmp231=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1EF)->f2;_tmp232=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1EF)->f3;_LL1D4: {
struct _tuple1*qv=_tmp232->name;struct Cyc_Absyn_Exp*tag_exp=_tmp231->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp231,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL192;}_LL1D5: if(*((int*)_tmp1EF)!= 30)goto _LL1D7;
_tmp233=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1EF)->f1;_tmp234=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1EF)->f2;_tmp235=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1EF)->f3;_LL1D6: {
struct _tuple1*_tmp350=_tmp235->name;struct Cyc_List_List*_tmp351=_tmp235->typs;
struct _tuple1*_tmp352=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp353=Cyc_Absyn_var_exp(
_tmp352,0);void*_tmp354=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp350,
_tag_arr("_struct",sizeof(unsigned char),8)));struct Cyc_Absyn_Exp*tag_exp=
_tmp234->is_xtunion?Cyc_Absyn_var_exp(_tmp350,0): Cyc_Toc_tunion_tag(_tmp234,
_tmp350,1);if(nv->toplevel){struct Cyc_List_List*dles=0;for(0;_tmp233 != 0;(
_tmp233=_tmp233->tl,_tmp351=_tmp351->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp233->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp351))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);dles=({struct Cyc_List_List*
_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->hd=({struct _tuple5*_tmp356=
_cycalloc(sizeof(*_tmp356));_tmp356->f1=0;_tmp356->f2=cur_e;_tmp356;});_tmp355->tl=
dles;_tmp355;});}dles=({struct Cyc_List_List*_tmp357=_cycalloc(sizeof(*_tmp357));
_tmp357->hd=({struct _tuple5*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->f1=0;
_tmp358->f2=tag_exp;_tmp358;});_tmp357->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp357;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359->v=Cyc_Toc_collapse_qvar_tag(
_tmp350,_tag_arr("_struct",sizeof(unsigned char),8));_tmp359;}),dles,0))->r));}
else{struct Cyc_List_List*_tmp35A=({struct Cyc_List_List*_tmp360=_cycalloc(sizeof(*
_tmp360));_tmp360->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp353,Cyc_Toc_tag_sp,
0),tag_exp,0);_tmp360->tl=0;_tmp360;});{int i=1;for(0;_tmp233 != 0;(((_tmp233=
_tmp233->tl,i ++)),_tmp351=_tmp351->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp233->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp351))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*
_tmp35B=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp353,Cyc_Absyn_fieldname(
i),0),cur_e,0);_tmp35A=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));
_tmp35C->hd=_tmp35B;_tmp35C->tl=_tmp35A;_tmp35C;});}}}{struct Cyc_Absyn_Stmt*
_tmp35D=Cyc_Absyn_exp_stmt(_tmp353,0);struct Cyc_Absyn_Stmt*_tmp35E=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->hd=_tmp35D;_tmp35F->tl=_tmp35A;
_tmp35F;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp352,_tmp354,0,_tmp35E,0),0))->r));}}goto _LL192;}_LL1D7: if(*((int*)_tmp1EF)!= 
31)goto _LL1D9;_LL1D8: goto _LL1DA;_LL1D9: if(*((int*)_tmp1EF)!= 32)goto _LL1DB;
_LL1DA: goto _LL192;_LL1DB: if(*((int*)_tmp1EF)!= 33)goto _LL1DD;_tmp236=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1EF)->f1;_tmp237=_tmp236.is_calloc;_tmp238=_tmp236.rgn;_tmp239=_tmp236.elt_type;
_tmp23A=_tmp236.num_elts;_tmp23B=_tmp236.fat_result;_LL1DC: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp239)));Cyc_Toc_exp_to_c(nv,_tmp23A);if(_tmp23B){struct
_tuple1*_tmp361=Cyc_Toc_temp_var();struct _tuple1*_tmp362=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp237){xexp=_tmp23A;if(_tmp238 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp238);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp361,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp239,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp361,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp363[3];_tmp363[2]=Cyc_Absyn_var_exp(_tmp361,0);_tmp363[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp363[0]=Cyc_Absyn_var_exp(_tmp362,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp363,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp23A,0);if(_tmp238 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp238);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp361,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp239,Cyc_Absyn_var_exp(_tmp361,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp364[3];
_tmp364[2]=Cyc_Absyn_var_exp(_tmp361,0);_tmp364[1]=Cyc_Absyn_uint_exp(1,0);
_tmp364[0]=Cyc_Absyn_var_exp(_tmp362,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp364,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp365=Cyc_Absyn_declare_stmt(_tmp361,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp362,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp365,0))->r));}}else{if(_tmp238 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp238);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp239,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL192;}_LL1DD: if(*((int*)_tmp1EF)!= 35)goto _LL1DF;_tmp23C=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1EF)->f1;_LL1DE: Cyc_Toc_stmt_to_c(nv,
_tmp23C);goto _LL192;_LL1DF: if(*((int*)_tmp1EF)!= 34)goto _LL1E1;_LL1E0:({void*
_tmp366[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem",sizeof(unsigned char),14),_tag_arr(_tmp366,sizeof(void*),
0));});_LL1E1: if(*((int*)_tmp1EF)!= 25)goto _LL1E3;_LL1E2:({void*_tmp367[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(unsigned char),12),_tag_arr(_tmp367,sizeof(void*),0));});_LL1E3: if(*((int*)
_tmp1EF)!= 36)goto _LL1E5;_tmp23D=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1EF)->f1;
_LL1E4:({void*_tmp368[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("codegen",sizeof(unsigned char),8),_tag_arr(_tmp368,
sizeof(void*),0));});_LL1E5: if(*((int*)_tmp1EF)!= 37)goto _LL192;_LL1E6:({void*
_tmp369[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("fill",sizeof(unsigned char),5),_tag_arr(_tmp369,sizeof(void*),0));});
_LL192:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{
struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple12{struct _tuple1*f1;void*f2;};struct _tuple13{struct Cyc_List_List*f1;struct
Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp36A=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp36B;
struct Cyc_Absyn_Vardecl _tmp36C;struct _tuple1*_tmp36D;struct Cyc_Absyn_Vardecl*
_tmp36E;void*_tmp36F;int _tmp370;unsigned char _tmp371;struct _tagged_arr _tmp372;
struct Cyc_Absyn_Enumdecl*_tmp373;struct Cyc_Absyn_Enumfield*_tmp374;void*_tmp375;
struct Cyc_Absyn_Enumfield*_tmp376;struct Cyc_Absyn_Tuniondecl*_tmp377;struct Cyc_Absyn_Tunionfield*
_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Pat*_tmp37A;struct Cyc_Absyn_Pat
_tmp37B;void*_tmp37C;struct Cyc_Absyn_Tuniondecl*_tmp37D;struct Cyc_Absyn_Tunionfield*
_tmp37E;struct Cyc_List_List*_tmp37F;struct Cyc_List_List*_tmp380;struct Cyc_List_List*
_tmp381;struct Cyc_Absyn_AggrInfo _tmp382;void*_tmp383;struct Cyc_List_List*_tmp384;
struct Cyc_Absyn_Pat*_tmp385;_LL28C: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 0)
goto _LL28E;_tmp36B=((struct Cyc_Absyn_Var_p_struct*)_tmp36A)->f1;_tmp36C=*_tmp36B;
_tmp36D=_tmp36C.name;_LL28D: nv=Cyc_Toc_add_varmap(nv,_tmp36D,r);goto _LL28F;
_LL28E: if((int)_tmp36A != 0)goto _LL290;_LL28F: s=Cyc_Toc_skip_stmt_dl();goto _LL28B;
_LL290: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 6)goto _LL292;_tmp36E=((struct
Cyc_Absyn_Reference_p_struct*)_tmp36A)->f1;_LL291: {struct _tuple1*_tmp386=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp387=_region_malloc(rgn,sizeof(*_tmp387));_tmp387->hd=({
struct _tuple12*_tmp388=_region_malloc(rgn,sizeof(*_tmp388));_tmp388->f1=_tmp386;
_tmp388->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp388;});
_tmp387->tl=decls;_tmp387;});nv=Cyc_Toc_add_varmap(nv,_tmp36E->name,Cyc_Absyn_var_exp(
_tmp386,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp386,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL28B;}_LL292: if((int)_tmp36A != 1)goto _LL294;_LL293: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL28B;_LL294: if(_tmp36A <= (void*)
2?1:*((int*)_tmp36A)!= 1)goto _LL296;_tmp36F=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp36A)->f1;_tmp370=((struct Cyc_Absyn_Int_p_struct*)_tmp36A)->f2;_LL295: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp36F,_tmp370,0),fail_stmt);goto _LL28B;_LL296: if(_tmp36A <= (
void*)2?1:*((int*)_tmp36A)!= 2)goto _LL298;_tmp371=((struct Cyc_Absyn_Char_p_struct*)
_tmp36A)->f1;_LL297: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp371,0),
fail_stmt);goto _LL28B;_LL298: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 3)goto
_LL29A;_tmp372=((struct Cyc_Absyn_Float_p_struct*)_tmp36A)->f1;_LL299: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp372,0),fail_stmt);goto _LL28B;_LL29A: if(_tmp36A <= (void*)
2?1:*((int*)_tmp36A)!= 9)goto _LL29C;_tmp373=((struct Cyc_Absyn_Enum_p_struct*)
_tmp36A)->f1;_tmp374=((struct Cyc_Absyn_Enum_p_struct*)_tmp36A)->f2;_LL29B: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp389=_cycalloc(
sizeof(*_tmp389));_tmp389[0]=({struct Cyc_Absyn_Enum_e_struct _tmp38A;_tmp38A.tag=
31;_tmp38A.f1=_tmp374->name;_tmp38A.f2=(struct Cyc_Absyn_Enumdecl*)_tmp373;
_tmp38A.f3=(struct Cyc_Absyn_Enumfield*)_tmp374;_tmp38A;});_tmp389;}),0),
fail_stmt);goto _LL28B;_LL29C: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 10)goto
_LL29E;_tmp375=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp36A)->f1;_tmp376=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp36A)->f2;_LL29D: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp38B=_cycalloc(
sizeof(*_tmp38B));_tmp38B[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp38C;_tmp38C.tag=
32;_tmp38C.f1=_tmp376->name;_tmp38C.f2=(void*)_tmp375;_tmp38C.f3=(struct Cyc_Absyn_Enumfield*)
_tmp376;_tmp38C;});_tmp38B;}),0),fail_stmt);goto _LL28B;_LL29E: if(_tmp36A <= (void*)
2?1:*((int*)_tmp36A)!= 8)goto _LL2A0;_tmp377=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36A)->f1;_tmp378=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f2;_tmp379=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f3;if(_tmp379 != 0)goto _LL2A0;_LL29F: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp377->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp378->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp377,_tmp378->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_typ,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL28B;}
_LL2A0: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 5)goto _LL2A2;_tmp37A=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp36A)->f1;_tmp37B=*_tmp37A;_tmp37C=(void*)_tmp37B.r;
if(_tmp37C <= (void*)2?1:*((int*)_tmp37C)!= 8)goto _LL2A2;_tmp37D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp37C)->f1;_tmp37E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp37C)->f2;_tmp37F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp37C)->f3;if(!(_tmp37F != 0))goto _LL2A2;
_LL2A1: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp37E->name,_tag_arr("_struct",sizeof(unsigned char),8));struct Cyc_Absyn_Exp*
rcast=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),
r,0);struct Cyc_List_List*_tmp38D=_tmp37E->typs;for(0;_tmp37F != 0;(((_tmp37F=
_tmp37F->tl,_tmp38D=((struct Cyc_List_List*)_check_null(_tmp38D))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp38E=(struct Cyc_Absyn_Pat*)_tmp37F->hd;if((void*)_tmp38E->r
== (void*)0)continue;{void*_tmp38F=(*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp38D))->hd)).f2;struct _tuple1*_tmp390=Cyc_Toc_temp_var();void*
_tmp391=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp38E->topt))->v;void*_tmp392=
Cyc_Toc_typ_to_c(_tmp391);struct Cyc_Absyn_Exp*_tmp393=Cyc_Absyn_aggrarrow_exp(
rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(
_tmp38F)))_tmp393=Cyc_Absyn_cast_exp(_tmp392,_tmp393,0);decls=({struct Cyc_List_List*
_tmp394=_region_malloc(rgn,sizeof(*_tmp394));_tmp394->hd=({struct _tuple12*
_tmp395=_region_malloc(rgn,sizeof(*_tmp395));_tmp395->f1=_tmp390;_tmp395->f2=
_tmp392;_tmp395;});_tmp394->tl=decls;_tmp394;});{struct _tuple11 _tmp396=Cyc_Toc_xlate_pat(
nv,rgn,_tmp391,Cyc_Absyn_var_exp(_tmp390,0),_tmp393,_tmp38E,fail_stmt,decls);nv=
_tmp396.f1;decls=_tmp396.f2;{struct Cyc_Absyn_Stmt*_tmp397=_tmp396.f3;struct Cyc_Absyn_Stmt*
_tmp398=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp390,0),_tmp393,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp398,_tmp397,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp37D->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp37E->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(_tmp37D);struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e3=
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r,0);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(
r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),max_tag_exp,0),0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp37D,_tmp37E->name,1);test_exp=max_tag > 0?Cyc_Absyn_or_exp(
e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,e,0);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);goto
_LL28B;}}_LL2A2: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 8)goto _LL2A4;_tmp380=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f3;_LL2A3: _tmp381=_tmp380;goto _LL2A5;
_LL2A4: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 4)goto _LL2A6;_tmp381=((struct
Cyc_Absyn_Tuple_p_struct*)_tmp36A)->f1;_LL2A5: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;
for(0;_tmp381 != 0;(_tmp381=_tmp381->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp399=(
struct Cyc_Absyn_Pat*)_tmp381->hd;if((void*)_tmp399->r == (void*)0)continue;{
struct _tuple1*_tmp39A=Cyc_Toc_temp_var();void*_tmp39B=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp399->topt))->v;decls=({struct Cyc_List_List*_tmp39C=
_region_malloc(rgn,sizeof(*_tmp39C));_tmp39C->hd=({struct _tuple12*_tmp39D=
_region_malloc(rgn,sizeof(*_tmp39D));_tmp39D->f1=_tmp39A;_tmp39D->f2=Cyc_Toc_typ_to_c(
_tmp39B);_tmp39D;});_tmp39C->tl=decls;_tmp39C;});{struct _tuple11 _tmp39E=Cyc_Toc_xlate_pat(
nv,rgn,_tmp39B,Cyc_Absyn_var_exp(_tmp39A,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp399,fail_stmt,decls);nv=_tmp39E.f1;decls=_tmp39E.f2;{struct Cyc_Absyn_Stmt*
_tmp39F=_tmp39E.f3;struct Cyc_Absyn_Stmt*_tmp3A0=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp39A,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3A0,_tmp39F,0),0);}}}}goto _LL28B;}_LL2A6: if(_tmp36A <= (
void*)2?1:*((int*)_tmp36A)!= 7)goto _LL2A8;_tmp382=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36A)->f1;_tmp383=(void*)_tmp382.aggr_info;_tmp384=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36A)->f3;_LL2A7: {struct Cyc_Absyn_Aggrdecl*_tmp3A1=Cyc_Absyn_get_known_aggrdecl(
_tmp383);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp384 != 0;_tmp384=_tmp384->tl){struct
_tuple13*_tmp3A2=(struct _tuple13*)_tmp384->hd;struct Cyc_Absyn_Pat*_tmp3A3=(*
_tmp3A2).f2;if((void*)_tmp3A3->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp3A4=(void*)((struct Cyc_List_List*)_check_null((*_tmp3A2).f1))->hd;struct
_tagged_arr*_tmp3A5;_LL2AF: if(*((int*)_tmp3A4)!= 1)goto _LL2B1;_tmp3A5=((struct
Cyc_Absyn_FieldName_struct*)_tmp3A4)->f1;_LL2B0: f=_tmp3A5;goto _LL2AE;_LL2B1:;
_LL2B2:(int)_throw((void*)Cyc_Toc_Match_error);_LL2AE:;}{struct _tuple1*_tmp3A6=
Cyc_Toc_temp_var();void*_tmp3A7=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A3->topt))->v;
void*_tmp3A8=Cyc_Toc_typ_to_c(_tmp3A7);decls=({struct Cyc_List_List*_tmp3A9=
_region_malloc(rgn,sizeof(*_tmp3A9));_tmp3A9->hd=({struct _tuple12*_tmp3AA=
_region_malloc(rgn,sizeof(*_tmp3AA));_tmp3AA->f1=_tmp3A6;_tmp3AA->f2=_tmp3A8;
_tmp3AA;});_tmp3A9->tl=decls;_tmp3A9;});{struct _tuple11 _tmp3AB=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3A7,Cyc_Absyn_var_exp(_tmp3A6,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp3A3,fail_stmt,decls);nv=_tmp3AB.f1;decls=_tmp3AB.f2;{struct Cyc_Absyn_Exp*
_tmp3AC=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3A1->impl))->fields,f)))->type))_tmp3AC=Cyc_Absyn_cast_exp(
_tmp3A8,_tmp3AC,0);{struct Cyc_Absyn_Stmt*_tmp3AD=_tmp3AB.f3;struct Cyc_Absyn_Stmt*
_tmp3AE=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp3A6,0),_tmp3AC,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3AE,_tmp3AD,0),0);}}}}}}goto _LL28B;}_LL2A8: if(_tmp36A <= (
void*)2?1:*((int*)_tmp36A)!= 5)goto _LL2AA;_tmp385=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp36A)->f1;_LL2A9: {struct _tuple1*_tmp3AF=Cyc_Toc_temp_var();void*_tmp3B0=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp385->topt))->v;decls=({struct Cyc_List_List*
_tmp3B1=_region_malloc(rgn,sizeof(*_tmp3B1));_tmp3B1->hd=({struct _tuple12*
_tmp3B2=_region_malloc(rgn,sizeof(*_tmp3B2));_tmp3B2->f1=_tmp3AF;_tmp3B2->f2=Cyc_Toc_typ_to_c(
_tmp3B0);_tmp3B2;});_tmp3B1->tl=decls;_tmp3B1;});{struct _tuple11 _tmp3B3=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3B0,Cyc_Absyn_var_exp(_tmp3AF,0),Cyc_Absyn_deref_exp(path,0),_tmp385,
fail_stmt,decls);nv=_tmp3B3.f1;decls=_tmp3B3.f2;{struct Cyc_Absyn_Stmt*_tmp3B4=
_tmp3B3.f3;struct Cyc_Absyn_Stmt*_tmp3B5=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp3AF,0),Cyc_Absyn_deref_exp(r,0),0),_tmp3B4,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp3B5,0);else{s=_tmp3B5;}goto
_LL28B;}}}_LL2AA: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 11)goto _LL2AC;_LL2AB:({
void*_tmp3B6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(_tmp3B6,sizeof(void*),0));});
_LL2AC: if(_tmp36A <= (void*)2?1:*((int*)_tmp36A)!= 12)goto _LL28B;_LL2AD:({void*
_tmp3B7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknowncall",sizeof(unsigned char),12),_tag_arr(_tmp3B7,sizeof(void*),
0));});_LL28B:;}return({struct _tuple11 _tmp3B8;_tmp3B8.f1=nv;_tmp3B8.f2=decls;
_tmp3B8.f3=s;_tmp3B8;});}struct _tuple14{struct _tagged_arr*f1;struct _tagged_arr*
f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple14*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple14*
_tmp3B9=_region_malloc(r,sizeof(*_tmp3B9));_tmp3B9->f1=Cyc_Toc_fresh_label();
_tmp3B9->f2=Cyc_Toc_fresh_label();_tmp3B9->f3=sc;_tmp3B9;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp3BA=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp3BB=Cyc_Tcutil_compress(
_tmp3BA);_LL2B4: if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 5)goto _LL2B6;_LL2B5:
goto _LL2B7;_LL2B6: if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 12)goto _LL2B8;
_LL2B7: leave_as_switch=1;goto _LL2B3;_LL2B8:;_LL2B9: leave_as_switch=0;goto _LL2B3;
_LL2B3:;}{struct Cyc_List_List*_tmp3BC=scs;for(0;_tmp3BC != 0;_tmp3BC=_tmp3BC->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp3BC->hd)->pat_vars))->v != 0?1:((struct Cyc_Absyn_Switch_clause*)_tmp3BC->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp3BD=scs;for(0;_tmp3BD != 0;_tmp3BD=
_tmp3BD->tl){struct Cyc_Absyn_Stmt*_tmp3BE=((struct Cyc_Absyn_Switch_clause*)
_tmp3BD->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp3BD->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp3BE,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp3BE);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp3BF=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp3BF;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp3C0=lscs;for(0;_tmp3C0 != 0;_tmp3C0=_tmp3C0->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp3C0->hd)).f3;struct _tagged_arr*fail_lab=_tmp3C0->tl == 
0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp3C0->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp3C2;struct Cyc_List_List*_tmp3C3;struct Cyc_Absyn_Stmt*
_tmp3C4;struct _tuple11 _tmp3C1=Cyc_Toc_xlate_pat(nv,rgn,_tmp3BA,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp3C2=_tmp3C1.f1;_tmp3C3=_tmp3C1.f2;
_tmp3C4=_tmp3C1.f3;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp3C5=(struct
Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp3C2,_tmp3C5);
_tmp3C4=Cyc_Absyn_seq_stmt(_tmp3C4,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((
void*)11,_tmp3C5,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}
decls=_tmp3C3;nvs=({struct Cyc_List_List*_tmp3C6=_region_malloc(rgn,sizeof(*
_tmp3C6));_tmp3C6->hd=_tmp3C2;_tmp3C6->tl=nvs;_tmp3C6;});test_stmts=({struct Cyc_List_List*
_tmp3C7=_region_malloc(rgn,sizeof(*_tmp3C7));_tmp3C7->hd=_tmp3C4;_tmp3C7->tl=
test_stmts;_tmp3C7;});}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple14 _tmp3C9;struct _tagged_arr*_tmp3CA;
struct _tagged_arr*_tmp3CB;struct Cyc_Absyn_Switch_clause*_tmp3CC;struct _tuple14*
_tmp3C8=(struct _tuple14*)lscs->hd;_tmp3C9=*_tmp3C8;_tmp3CA=_tmp3C9.f1;_tmp3CB=
_tmp3C9.f2;_tmp3CC=_tmp3C9.f3;{struct Cyc_Toc_Env*_tmp3CD=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp3CC->body;
if(lscs->tl != 0){struct _tuple14 _tmp3CF;struct _tagged_arr*_tmp3D0;struct Cyc_Absyn_Switch_clause*
_tmp3D1;struct _tuple14*_tmp3CE=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp3CF=*_tmp3CE;_tmp3D0=_tmp3CF.f2;_tmp3D1=_tmp3CF.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp3CD,end_l,_tmp3D0,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D1->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp3CD,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp3CA,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp3CB,s,0),0);stmts=({
struct Cyc_List_List*_tmp3D2=_region_malloc(rgn,sizeof(*_tmp3D2));_tmp3D2->hd=s;
_tmp3D2->tl=stmts;_tmp3D2;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple12 _tmp3D4;struct _tuple1*_tmp3D5;void*_tmp3D6;struct _tuple12*_tmp3D3=(
struct _tuple12*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp3D4=*_tmp3D3;
_tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;res=Cyc_Absyn_declare_stmt(_tmp3D5,_tmp3D6,
0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,
res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp3D7=
_cycalloc(sizeof(*_tmp3D7));_tmp3D7->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp3D7->tl=0;_tmp3D7;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp3D8=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DA;struct Cyc_Absyn_Stmt*
_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Stmt*
_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3DF;struct _tuple3 _tmp3E0;struct Cyc_Absyn_Exp*
_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Stmt*_tmp3E3;struct Cyc_Absyn_Stmt*
_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E6;struct _tuple3
_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E8;struct _tuple3 _tmp3E9;struct Cyc_Absyn_Exp*
_tmp3EA;struct Cyc_Absyn_Stmt*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3EC;struct Cyc_List_List*
_tmp3ED;struct Cyc_List_List*_tmp3EE;struct Cyc_Absyn_Switch_clause**_tmp3EF;
struct Cyc_Absyn_Decl*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F1;struct _tagged_arr*
_tmp3F2;struct Cyc_Absyn_Stmt*_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F4;struct _tuple3
_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Stmt*_tmp3F7;struct Cyc_List_List*
_tmp3F8;struct Cyc_Absyn_Tvar*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3FA;int _tmp3FB;
struct Cyc_Absyn_Stmt*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;_LL2BB: if((int)_tmp3D8
!= 0)goto _LL2BD;_LL2BC: return;_LL2BD: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 
0)goto _LL2BF;_tmp3D9=((struct Cyc_Absyn_Exp_s_struct*)_tmp3D8)->f1;_LL2BE: Cyc_Toc_exp_to_c(
nv,_tmp3D9);return;_LL2BF: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 1)goto
_LL2C1;_tmp3DA=((struct Cyc_Absyn_Seq_s_struct*)_tmp3D8)->f1;_tmp3DB=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3D8)->f2;_LL2C0: Cyc_Toc_stmt_to_c(nv,_tmp3DA);s=_tmp3DB;continue;_LL2C1: if(
_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 2)goto _LL2C3;_tmp3DC=((struct Cyc_Absyn_Return_s_struct*)
_tmp3D8)->f1;_LL2C2: {struct Cyc_Core_Opt*topt=0;if(_tmp3DC != 0){topt=({struct Cyc_Core_Opt*
_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3DC->topt))->v);_tmp3FE;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp3DC));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp3FF=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp400=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp3FF,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp3FF,(void*)topt->v,_tmp3DC,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp400,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL2C3: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 3)goto _LL2C5;_tmp3DD=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D8)->f1;_tmp3DE=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3D8)->f2;_tmp3DF=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D8)->f3;_LL2C4:
Cyc_Toc_exp_to_c(nv,_tmp3DD);Cyc_Toc_stmt_to_c(nv,_tmp3DE);s=_tmp3DF;continue;
_LL2C5: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 4)goto _LL2C7;_tmp3E0=((struct
Cyc_Absyn_While_s_struct*)_tmp3D8)->f1;_tmp3E1=_tmp3E0.f1;_tmp3E2=((struct Cyc_Absyn_While_s_struct*)
_tmp3D8)->f2;_LL2C6: Cyc_Toc_exp_to_c(nv,_tmp3E1);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3E2);return;_LL2C7: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 5)goto
_LL2C9;_tmp3E3=((struct Cyc_Absyn_Break_s_struct*)_tmp3D8)->f1;_LL2C8: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp3E3 == 0?0: _tmp3E3->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL2C9: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 6)goto _LL2CB;
_tmp3E4=((struct Cyc_Absyn_Continue_s_struct*)_tmp3D8)->f1;_LL2CA: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp3E5=_tmp3E4;
goto _LL2CC;_LL2CB: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 7)goto _LL2CD;
_tmp3E5=((struct Cyc_Absyn_Goto_s_struct*)_tmp3D8)->f2;_LL2CC: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp3E5))->try_depth,s);
return;_LL2CD: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 8)goto _LL2CF;_tmp3E6=((
struct Cyc_Absyn_For_s_struct*)_tmp3D8)->f1;_tmp3E7=((struct Cyc_Absyn_For_s_struct*)
_tmp3D8)->f2;_tmp3E8=_tmp3E7.f1;_tmp3E9=((struct Cyc_Absyn_For_s_struct*)_tmp3D8)->f3;
_tmp3EA=_tmp3E9.f1;_tmp3EB=((struct Cyc_Absyn_For_s_struct*)_tmp3D8)->f4;_LL2CE:
Cyc_Toc_exp_to_c(nv,_tmp3E6);Cyc_Toc_exp_to_c(nv,_tmp3E8);Cyc_Toc_exp_to_c(nv,
_tmp3EA);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3EB);return;_LL2CF: if(
_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 9)goto _LL2D1;_tmp3EC=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3D8)->f1;_tmp3ED=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D8)->f2;_LL2D0: Cyc_Toc_xlate_switch(
nv,s,_tmp3EC,_tmp3ED);return;_LL2D1: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 
11)goto _LL2D3;_tmp3EE=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D8)->f1;_tmp3EF=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D8)->f2;_LL2D2: if(nv->fallthru_info == 0)({
void*_tmp401[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("fallthru in unexpected place",sizeof(unsigned char),29),_tag_arr(
_tmp401,sizeof(void*),0));});{struct _tuple7 _tmp403;struct _tagged_arr*_tmp404;
struct Cyc_List_List*_tmp405;struct Cyc_Dict_Dict*_tmp406;struct _tuple7*_tmp402=(
struct _tuple7*)((struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;_tmp403=*
_tmp402;_tmp404=_tmp403.f1;_tmp405=_tmp403.f2;_tmp406=_tmp403.f3;{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_goto_stmt(_tmp404,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((
struct Cyc_Absyn_Switch_clause**)_check_null(_tmp3EF)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp407=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp405);struct Cyc_List_List*_tmp408=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp3EE);for(0;_tmp407 != 0;(_tmp407=_tmp407->tl,_tmp408=_tmp408->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp408))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp406,(struct _tuple1*)
_tmp407->hd),(struct Cyc_Absyn_Exp*)_tmp408->hd,0),s2,0);}(void*)(s->r=(void*)((
void*)s2->r));return;}}}_LL2D3: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 12)
goto _LL2D5;_tmp3F0=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D8)->f1;_tmp3F1=((
struct Cyc_Absyn_Decl_s_struct*)_tmp3D8)->f2;_LL2D4:{void*_tmp409=(void*)_tmp3F0->r;
struct Cyc_Absyn_Vardecl*_tmp40A;struct Cyc_Absyn_Pat*_tmp40B;struct Cyc_Absyn_Exp*
_tmp40C;struct Cyc_List_List*_tmp40D;_LL2E8: if(*((int*)_tmp409)!= 0)goto _LL2EA;
_tmp40A=((struct Cyc_Absyn_Var_d_struct*)_tmp409)->f1;_LL2E9: {struct Cyc_Toc_Env*
_tmp40E=Cyc_Toc_add_varmap(nv,_tmp40A->name,Cyc_Absyn_varb_exp(_tmp40A->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));
_tmp40F[0]=({struct Cyc_Absyn_Local_b_struct _tmp410;_tmp410.tag=3;_tmp410.f1=
_tmp40A;_tmp410;});_tmp40F;}),0));Cyc_Toc_local_decl_to_c(_tmp40E,_tmp40E,
_tmp40A,_tmp3F1);goto _LL2E7;}_LL2EA: if(*((int*)_tmp409)!= 2)goto _LL2EC;_tmp40B=((
struct Cyc_Absyn_Let_d_struct*)_tmp409)->f1;_tmp40C=((struct Cyc_Absyn_Let_d_struct*)
_tmp409)->f3;_LL2EB:{void*_tmp411=(void*)_tmp40B->r;struct Cyc_Absyn_Vardecl*
_tmp412;_LL2F1: if(_tmp411 <= (void*)2?1:*((int*)_tmp411)!= 0)goto _LL2F3;_tmp412=((
struct Cyc_Absyn_Var_p_struct*)_tmp411)->f1;_LL2F2: {struct _tuple1*old_name=
_tmp412->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp412->name=new_name;
_tmp412->initializer=(struct Cyc_Absyn_Exp*)_tmp40C;(void*)(_tmp3F0->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413[
0]=({struct Cyc_Absyn_Var_d_struct _tmp414;_tmp414.tag=0;_tmp414.f1=_tmp412;
_tmp414;});_tmp413;})));{struct Cyc_Toc_Env*_tmp415=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=({struct Cyc_Absyn_Local_b_struct
_tmp417;_tmp417.tag=3;_tmp417.f1=_tmp412;_tmp417;});_tmp416;}),0));Cyc_Toc_local_decl_to_c(
_tmp415,nv,_tmp412,_tmp3F1);goto _LL2F0;}}_LL2F3:;_LL2F4:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp40B,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp40C->topt))->v,_tmp40C,_tmp3F1))->r));goto _LL2F0;_LL2F0:;}goto _LL2E7;_LL2EC:
if(*((int*)_tmp409)!= 3)goto _LL2EE;_tmp40D=((struct Cyc_Absyn_Letv_d_struct*)
_tmp409)->f1;_LL2ED: {struct Cyc_List_List*_tmp418=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp40D);if(_tmp418 == 0)({void*_tmp419[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(unsigned char),13),_tag_arr(_tmp419,sizeof(void*),0));});(void*)(_tmp3F0->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));
_tmp41A[0]=({struct Cyc_Absyn_Var_d_struct _tmp41B;_tmp41B.tag=0;_tmp41B.f1=(
struct Cyc_Absyn_Vardecl*)_tmp418->hd;_tmp41B;});_tmp41A;})));_tmp418=_tmp418->tl;
for(0;_tmp418 != 0;_tmp418=_tmp418->tl){struct Cyc_Absyn_Decl*_tmp41C=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp41E;_tmp41E.tag=0;_tmp41E.f1=(struct Cyc_Absyn_Vardecl*)
_tmp418->hd;_tmp41E;});_tmp41D;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp41C,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL2E7;}_LL2EE:;_LL2EF:({void*_tmp41F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad nested declaration within function",
sizeof(unsigned char),39),_tag_arr(_tmp41F,sizeof(void*),0));});_LL2E7:;}return;
_LL2D5: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 15)goto _LL2D7;_tmp3F2=((struct
Cyc_Absyn_Label_s_struct*)_tmp3D8)->f1;_tmp3F3=((struct Cyc_Absyn_Label_s_struct*)
_tmp3D8)->f2;_LL2D6: s=_tmp3F3;continue;_LL2D7: if(_tmp3D8 <= (void*)1?1:*((int*)
_tmp3D8)!= 16)goto _LL2D9;_tmp3F4=((struct Cyc_Absyn_Do_s_struct*)_tmp3D8)->f1;
_tmp3F5=((struct Cyc_Absyn_Do_s_struct*)_tmp3D8)->f2;_tmp3F6=_tmp3F5.f1;_LL2D8:
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3F4);Cyc_Toc_exp_to_c(nv,_tmp3F6);
return;_LL2D9: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 17)goto _LL2DB;_tmp3F7=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp3D8)->f1;_tmp3F8=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D8)->f2;_LL2DA: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->v=(
void*)e_typ;_tmp420;});nv=Cyc_Toc_add_varmap(nv,e_var,e_exp);Cyc_Toc_stmt_to_c(
nv,_tmp3F7);{struct Cyc_Absyn_Stmt*_tmp421=Cyc_Absyn_seq_stmt(_tmp3F7,Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);struct _tuple1*_tmp422=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_var_exp(_tmp422,0);struct Cyc_Absyn_Vardecl*
_tmp424=Cyc_Absyn_new_vardecl(_tmp422,Cyc_Absyn_exn_typ,0);_tmp423->topt=({
struct Cyc_Core_Opt*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->v=(void*)Cyc_Absyn_exn_typ;
_tmp425;});{struct Cyc_Absyn_Pat*_tmp426=({struct Cyc_Absyn_Pat*_tmp436=_cycalloc(
sizeof(*_tmp436));_tmp436->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_Absyn_Var_p_struct
_tmp439;_tmp439.tag=0;_tmp439.f1=_tmp424;_tmp439;});_tmp438;}));_tmp436->topt=({
struct Cyc_Core_Opt*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->v=(void*)Cyc_Absyn_exn_typ;
_tmp437;});_tmp436->loc=0;_tmp436;});struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_throw_exp(
_tmp423,0);_tmp427->topt=({struct Cyc_Core_Opt*_tmp428=_cycalloc(sizeof(*_tmp428));
_tmp428->v=(void*)((void*)0);_tmp428;});{struct Cyc_Absyn_Stmt*_tmp429=Cyc_Absyn_exp_stmt(
_tmp427,0);struct Cyc_Absyn_Switch_clause*_tmp42A=({struct Cyc_Absyn_Switch_clause*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->pattern=_tmp426;_tmp433->pat_vars=({
struct Cyc_Core_Opt*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->v=({struct Cyc_List_List*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->hd=_tmp424;_tmp435->tl=0;_tmp435;});
_tmp434;});_tmp433->where_clause=0;_tmp433->body=_tmp429;_tmp433->loc=0;_tmp433;});
struct Cyc_Absyn_Stmt*_tmp42B=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3F8,({struct
Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->hd=_tmp42A;_tmp432->tl=
0;_tmp432;})),0);Cyc_Toc_stmt_to_c(nv,_tmp42B);{struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp431=_cycalloc(sizeof(*_tmp431));
_tmp431->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp431->tl=0;
_tmp431;}),0);struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp430=_cycalloc(sizeof(*_tmp430));
_tmp430->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp430->tl=0;_tmp430;}),0),0);struct
Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp42F=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp42D,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp42E,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp42C,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp42F,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp421,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp42B,0),0),0),0),0),0))->r));return;}}}}}
_LL2DB: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 18)goto _LL2DD;_tmp3F9=((struct
Cyc_Absyn_Region_s_struct*)_tmp3D8)->f1;_tmp3FA=((struct Cyc_Absyn_Region_s_struct*)
_tmp3D8)->f2;_tmp3FB=((struct Cyc_Absyn_Region_s_struct*)_tmp3D8)->f3;_tmp3FC=((
struct Cyc_Absyn_Region_s_struct*)_tmp3D8)->f4;_LL2DC: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp3FA->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp3FC);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp43A->tl=0;_tmp43A;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp43B[1];_tmp43B[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp43B,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),Cyc_Absyn_seq_stmt(_tmp3FC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp*_tmp43C[1];_tmp43C[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp43C,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL2DD: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)
!= 20)goto _LL2DF;_tmp3FD=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3D8)->f1;
_LL2DE: Cyc_Toc_exp_to_c(nv,_tmp3FD);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp43D=
_cycalloc(sizeof(*_tmp43D));_tmp43D->hd=_tmp3FD;_tmp43D->tl=0;_tmp43D;}),0),0))->r));
return;_LL2DF: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 10)goto _LL2E1;_LL2E0:({
void*_tmp43E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("switchC_s",sizeof(unsigned char),10),_tag_arr(_tmp43E,sizeof(void*),0));});
_LL2E1: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 13)goto _LL2E3;_LL2E2:({void*
_tmp43F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("cut",sizeof(unsigned char),4),_tag_arr(_tmp43F,sizeof(void*),0));});
_LL2E3: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 14)goto _LL2E5;_LL2E4:({void*
_tmp440[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("splice",sizeof(unsigned char),7),_tag_arr(_tmp440,sizeof(void*),0));});
_LL2E5: if(_tmp3D8 <= (void*)1?1:*((int*)_tmp3D8)!= 19)goto _LL2BA;_LL2E6:({void*
_tmp441[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("forarray",sizeof(unsigned char),9),_tag_arr(_tmp441,sizeof(void*),0));});
_LL2BA:;}}struct _tuple15{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=
0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct Cyc_List_List*_tmp442=f->args;for(0;_tmp442 != 0;_tmp442=_tmp442->tl){(*((
struct _tuple15*)_tmp442->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp442->hd)).f3);{
struct _tuple1*_tmp443=({struct _tuple1*_tmp444=_cycalloc(sizeof(*_tmp444));
_tmp444->f1=(void*)0;_tmp444->f2=(*((struct _tuple15*)_tmp442->hd)).f1;_tmp444;});
nv=Cyc_Toc_add_varmap(nv,_tmp443,Cyc_Absyn_var_exp(_tmp443,0));}}}if((
unsigned int)f->cyc_varargs?((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name
!= 0: 0){struct Cyc_Core_Opt*_tmp446;struct Cyc_Absyn_Tqual _tmp447;void*_tmp448;int
_tmp449;struct Cyc_Absyn_VarargInfo _tmp445=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp446=_tmp445.name;_tmp447=_tmp445.tq;_tmp448=(
void*)_tmp445.type;_tmp449=_tmp445.inject;{void*_tmp44A=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(
_tmp448,(void*)2,_tmp447));struct _tuple1*_tmp44B=({struct _tuple1*_tmp44E=
_cycalloc(sizeof(*_tmp44E));_tmp44E->f1=(void*)0;_tmp44E->f2=(struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(_tmp446))->v;_tmp44E;});f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct
Cyc_List_List*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->hd=({struct _tuple15*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->f1=(struct _tagged_arr*)_tmp446->v;
_tmp44D->f2=_tmp447;_tmp44D->f3=_tmp44A;_tmp44D;});_tmp44C->tl=0;_tmp44C;}));nv=
Cyc_Toc_add_varmap(nv,_tmp44B,Cyc_Absyn_var_exp(_tmp44B,0));f->cyc_varargs=0;}}{
struct Cyc_List_List*_tmp44F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(f->param_vardecls))->v;for(0;_tmp44F != 0;_tmp44F=_tmp44F->tl){(void*)(((
struct Cyc_Absyn_Vardecl*)_tmp44F->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Absyn_Vardecl*)_tmp44F->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp450=s;_LL2F6: if((
int)_tmp450 != 1)goto _LL2F8;_LL2F7: return(void*)2;_LL2F8: if((int)_tmp450 != 4)goto
_LL2FA;_LL2F9: return(void*)3;_LL2FA:;_LL2FB: return s;_LL2F5:;}struct Cyc_Core_Opt*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp451=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Core_Opt*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp452;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp453=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp451).f2);if(
_tmp453 == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp451).f2,ad));}
else{if(((struct Cyc_Absyn_Aggrdecl*)_tmp453->v)->impl == 0){(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*
_tmp451).f2,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp454=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp454 != 0;_tmp454=_tmp454->tl){(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp454->hd)->type=(
void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Absyn_Aggrfield*)_tmp454->hd)->type));}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->v=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);
_tmp455;});{struct _tuple1*_tmp456=tud->name;if(tud->fields == 0?1:((int(*)(struct
Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp456))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp456));{struct Cyc_List_List*
_tmp457=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp457 != 0;_tmp457=_tmp457->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp457->hd;if(f->typs != 0){struct Cyc_List_List*_tmp458=0;int i=1;{struct Cyc_List_List*
_tmp459=f->typs;for(0;_tmp459 != 0;(_tmp459=_tmp459->tl,i ++)){struct _tagged_arr*
_tmp45A=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp45B=({struct Cyc_Absyn_Aggrfield*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->name=_tmp45A;_tmp45D->tq=(*((struct
_tuple4*)_tmp459->hd)).f1;_tmp45D->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp459->hd)).f2);_tmp45D->width=0;_tmp45D->attributes=0;_tmp45D;});
_tmp458=({struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->hd=
_tmp45B;_tmp45C->tl=_tmp458;_tmp45C;});}}_tmp458=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp458);_tmp458=({struct Cyc_List_List*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->hd=({struct Cyc_Absyn_Aggrfield*
_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->name=Cyc_Toc_tag_sp;_tmp45F->tq=Cyc_Toc_mt_tq;
_tmp45F->type=(void*)Cyc_Absyn_sint_typ;_tmp45F->width=0;_tmp45F->attributes=0;
_tmp45F;});_tmp45E->tl=_tmp458;_tmp45E;});{struct Cyc_Absyn_Aggrdecl*_tmp460=({
struct Cyc_Absyn_Aggrdecl*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->kind=(void*)((
void*)0);_tmp464->sc=(void*)((void*)2);_tmp464->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(unsigned char),8));_tmp464->tvs=0;_tmp464->impl=({
struct Cyc_Absyn_AggrdeclImpl*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->exist_vars=
0;_tmp465->rgn_po=0;_tmp465->fields=_tmp458;_tmp465;});_tmp464->attributes=0;
_tmp464;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp461=_cycalloc(sizeof(*
_tmp461));_tmp461->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp463;_tmp463.tag=4;_tmp463.f1=_tmp460;_tmp463;});_tmp462;}),0);_tmp461->tl=
Cyc_Toc_result_decls;_tmp461;});}}}}}}static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=
0;static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far
== 0)Cyc_Toc_xtunion_fields_so_far=({struct Cyc_Core_Opt*_tmp466=_cycalloc(
sizeof(*_tmp466));_tmp466->v=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,
struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp466;});if(xd->fields == 
0)return;{struct _tuple1*_tmp467=xd->name;struct Cyc_List_List*_tmp468=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp468 != 0;_tmp468=
_tmp468->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)_tmp468->hd;
struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(unsigned char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*
_tmp46A=({struct Cyc_Absyn_ArrayType_struct*_tmp493=_cycalloc(sizeof(*_tmp493));
_tmp493[0]=({struct Cyc_Absyn_ArrayType_struct _tmp494;_tmp494.tag=7;_tmp494.f1=(
void*)Cyc_Absyn_uchar_typ;_tmp494.f2=Cyc_Toc_mt_tq;_tmp494.f3=(struct Cyc_Absyn_Exp*)
_tmp469;_tmp494;});_tmp493;});struct Cyc_Core_Opt*_tmp46B=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);
struct Cyc_Core_Opt _tmp46C;int _tmp46D;_LL2FD: if(_tmp46B != 0)goto _LL2FF;_LL2FE: {
struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3)initopt=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp473;
_tmp473.tag=0;_tmp473.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct
_tmp472;_tmp472.tag=1;_tmp472.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp471;_tmp471.tag=1;_tmp471.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp470;_tmp470.tag=1;_tmp470.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp46F;_tmp46F.tag=1;_tmp46F.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp46E[5]={& _tmp46F,& _tmp470,& _tmp471,& _tmp472,& _tmp473};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp46E,sizeof(void*),5));}}}}}}),
0);{struct Cyc_Absyn_Vardecl*_tmp474=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp46A,
initopt);(void*)(_tmp474->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476[
0]=({struct Cyc_Absyn_Var_d_struct _tmp477;_tmp477.tag=0;_tmp477.f1=_tmp474;
_tmp477;});_tmp476;}),0);_tmp475->tl=Cyc_Toc_result_decls;_tmp475;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp478=f->typs;for(0;_tmp478 != 0;(_tmp478=_tmp478->tl,
i ++)){struct _tagged_arr*_tmp479=({struct _tagged_arr*_tmp47D=_cycalloc(sizeof(*
_tmp47D));_tmp47D[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp47F;
_tmp47F.tag=1;_tmp47F.f1=(int)((unsigned int)i);{void*_tmp47E[1]={& _tmp47F};Cyc_Std_aprintf(
_tag_arr("f%d",sizeof(unsigned char),4),_tag_arr(_tmp47E,sizeof(void*),1));}});
_tmp47D;});struct Cyc_Absyn_Aggrfield*_tmp47A=({struct Cyc_Absyn_Aggrfield*_tmp47C=
_cycalloc(sizeof(*_tmp47C));_tmp47C->name=_tmp479;_tmp47C->tq=(*((struct _tuple4*)
_tmp478->hd)).f1;_tmp47C->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp478->hd)).f2);_tmp47C->width=0;_tmp47C->attributes=0;_tmp47C;});fields=({
struct Cyc_List_List*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->hd=_tmp47A;
_tmp47B->tl=fields;_tmp47B;});}}fields=({struct Cyc_List_List*_tmp480=_cycalloc(
sizeof(*_tmp480));_tmp480->hd=({struct Cyc_Absyn_Aggrfield*_tmp481=_cycalloc(
sizeof(*_tmp481));_tmp481->name=Cyc_Toc_tag_sp;_tmp481->tq=Cyc_Toc_mt_tq;_tmp481->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);_tmp481->width=0;
_tmp481->attributes=0;_tmp481;});_tmp480->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp480;});{struct Cyc_Absyn_Aggrdecl*_tmp482=({
struct Cyc_Absyn_Aggrdecl*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->kind=(void*)((
void*)0);_tmp486->sc=(void*)((void*)2);_tmp486->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(unsigned char),8));_tmp486->tvs=0;_tmp486->impl=({
struct Cyc_Absyn_AggrdeclImpl*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->exist_vars=
0;_tmp487->rgn_po=0;_tmp487->fields=fields;_tmp487;});_tmp486->attributes=0;
_tmp486;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp483=_cycalloc(sizeof(*
_tmp483));_tmp483->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp485;_tmp485.tag=4;_tmp485.f1=_tmp482;_tmp485;});_tmp484;}),0);_tmp483->tl=
Cyc_Toc_result_decls;_tmp483;});}}goto _LL2FC;}}_LL2FF: if(_tmp46B == 0)goto _LL301;
_tmp46C=*_tmp46B;_tmp46D=(int)_tmp46C.v;if(_tmp46D != 0)goto _LL301;_LL300: if((
void*)f->sc != (void*)3){struct Cyc_Absyn_Exp*_tmp488=Cyc_Absyn_string_exp((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp492;_tmp492.tag=0;_tmp492.f1=(
struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct _tmp491;_tmp491.tag=1;_tmp491.f1=(
int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp490;_tmp490.tag=
1;_tmp490.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp48F;_tmp48F.tag=1;_tmp48F.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp48E;_tmp48E.tag=1;_tmp48E.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp48D[5]={& _tmp48E,& _tmp48F,& _tmp490,& _tmp491,& _tmp492};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp48D,sizeof(void*),5));}}}}}}),
0);struct Cyc_Absyn_Vardecl*_tmp489=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp46A,(
struct Cyc_Absyn_Exp*)_tmp488);(void*)(_tmp489->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B[
0]=({struct Cyc_Absyn_Var_d_struct _tmp48C;_tmp48C.tag=0;_tmp48C.f1=_tmp489;
_tmp48C;});_tmp48B;}),0);_tmp48A->tl=Cyc_Toc_result_decls;_tmp48A;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL2FC;_LL301:;_LL302: goto _LL2FC;_LL2FC:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((void*)vd->type));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp495=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp498;_LL304: if(*((int*)
_tmp495)!= 27)goto _LL306;_tmp496=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp495)->f1;_tmp497=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp495)->f2;
_tmp498=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp495)->f3;_LL305: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp496,_tmp497,_tmp498,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL303;_LL306:;_LL307: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp499=({
struct Cyc_Toc_Env*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=*init_nv;
_tmp49A;});_tmp499->toplevel=1;Cyc_Toc_exp_to_c(_tmp499,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL303;_LL303:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp49B=_cycalloc(
sizeof(*_tmp49B));_tmp49B[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp49B;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp49C=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp49C;_push_region(prgn);{struct Cyc_Toc_Env*_tmp49E;
struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_Stmt*_tmp4A0;struct _tuple11 _tmp49D=
Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;_tmp4A0=_tmp49D.f3;Cyc_Toc_stmt_to_c(
_tmp49E,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp4A0,s,0),0);for(0;_tmp49F != 0;_tmp49F=_tmp49F->tl){
struct _tuple12 _tmp4A2;struct _tuple1*_tmp4A3;void*_tmp4A4;struct _tuple12*_tmp4A1=(
struct _tuple12*)_tmp49F->hd;_tmp4A2=*_tmp4A1;_tmp4A3=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;
s=Cyc_Absyn_declare_stmt(_tmp4A3,_tmp4A4,0,s,0);}};_pop_region(prgn);}return s;}}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp4A5[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("decls_to_c: not at toplevel!",
sizeof(unsigned char),29),_tag_arr(_tmp4A5,sizeof(void*),0));});{struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp4A6=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp4A7;struct Cyc_Absyn_Fndecl*_tmp4A8;struct Cyc_Absyn_Aggrdecl*_tmp4A9;struct
Cyc_Absyn_Tuniondecl*_tmp4AA;struct Cyc_Absyn_Enumdecl*_tmp4AB;struct Cyc_Absyn_Typedefdecl*
_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_List_List*_tmp4AE;struct Cyc_List_List*
_tmp4AF;_LL309: if(*((int*)_tmp4A6)!= 0)goto _LL30B;_tmp4A7=((struct Cyc_Absyn_Var_d_struct*)
_tmp4A6)->f1;_LL30A: {struct _tuple1*_tmp4B0=_tmp4A7->name;if((void*)_tmp4A7->sc
== (void*)4)_tmp4B0=({struct _tuple1*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->f1=
Cyc_Absyn_rel_ns_null;_tmp4B1->f2=(*_tmp4B0).f2;_tmp4B1;});if(_tmp4A7->initializer
!= 0)Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp4A7->initializer));
nv=Cyc_Toc_add_varmap(nv,_tmp4A7->name,Cyc_Absyn_varb_exp(_tmp4B0,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({
struct Cyc_Absyn_Global_b_struct _tmp4B3;_tmp4B3.tag=0;_tmp4B3.f1=_tmp4A7;_tmp4B3;});
_tmp4B2;}),0));_tmp4A7->name=_tmp4B0;(void*)(_tmp4A7->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp4A7->sc));(void*)(_tmp4A7->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4A7->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B4=_cycalloc(
sizeof(*_tmp4B4));_tmp4B4->hd=d;_tmp4B4->tl=Cyc_Toc_result_decls;_tmp4B4;});goto
_LL308;}_LL30B: if(*((int*)_tmp4A6)!= 1)goto _LL30D;_tmp4A8=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4A6)->f1;_LL30C: {struct _tuple1*_tmp4B5=_tmp4A8->name;if((void*)_tmp4A8->sc
== (void*)4)_tmp4B5=({struct _tuple1*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->f1=
Cyc_Absyn_rel_ns_null;_tmp4B6->f2=(*_tmp4B5).f2;_tmp4B6;});nv=Cyc_Toc_add_varmap(
nv,_tmp4A8->name,Cyc_Absyn_var_exp(_tmp4B5,0));_tmp4A8->name=_tmp4B5;Cyc_Toc_fndecl_to_c(
nv,_tmp4A8);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B7=_cycalloc(sizeof(*
_tmp4B7));_tmp4B7->hd=d;_tmp4B7->tl=Cyc_Toc_result_decls;_tmp4B7;});goto _LL308;}
_LL30D: if(*((int*)_tmp4A6)!= 2)goto _LL30F;_LL30E: goto _LL310;_LL30F: if(*((int*)
_tmp4A6)!= 3)goto _LL311;_LL310:({void*_tmp4B8[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("letdecl at toplevel",
sizeof(unsigned char),20),_tag_arr(_tmp4B8,sizeof(void*),0));});_LL311: if(*((int*)
_tmp4A6)!= 4)goto _LL313;_tmp4A9=((struct Cyc_Absyn_Aggr_d_struct*)_tmp4A6)->f1;
_LL312: Cyc_Toc_aggrdecl_to_c(_tmp4A9);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->hd=d;_tmp4B9->tl=Cyc_Toc_result_decls;
_tmp4B9;});goto _LL308;_LL313: if(*((int*)_tmp4A6)!= 5)goto _LL315;_tmp4AA=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp4A6)->f1;_LL314: if(_tmp4AA->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp4AA);else{Cyc_Toc_tuniondecl_to_c(_tmp4AA);}goto _LL308;_LL315: if(*((int*)
_tmp4A6)!= 6)goto _LL317;_tmp4AB=((struct Cyc_Absyn_Enum_d_struct*)_tmp4A6)->f1;
_LL316: Cyc_Toc_enumdecl_to_c(nv,_tmp4AB);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->hd=d;_tmp4BA->tl=Cyc_Toc_result_decls;
_tmp4BA;});goto _LL308;_LL317: if(*((int*)_tmp4A6)!= 7)goto _LL319;_tmp4AC=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp4A6)->f1;_LL318: _tmp4AC->name=_tmp4AC->name;
_tmp4AC->tvs=0;if(_tmp4AC->defn != 0)_tmp4AC->defn=({struct Cyc_Core_Opt*_tmp4BB=
_cycalloc(sizeof(*_tmp4BB));_tmp4BB->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4AC->defn))->v);_tmp4BB;});else{void*_tmp4BC=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4AC->kind))->v;_LL320: if((int)
_tmp4BC != 2)goto _LL322;_LL321: _tmp4AC->defn=({struct Cyc_Core_Opt*_tmp4BD=
_cycalloc(sizeof(*_tmp4BD));_tmp4BD->v=(void*)Cyc_Absyn_void_star_typ();_tmp4BD;});
goto _LL31F;_LL322:;_LL323: _tmp4AC->defn=({struct Cyc_Core_Opt*_tmp4BE=_cycalloc(
sizeof(*_tmp4BE));_tmp4BE->v=(void*)((void*)0);_tmp4BE;});goto _LL31F;_LL31F:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));
_tmp4BF->hd=d;_tmp4BF->tl=Cyc_Toc_result_decls;_tmp4BF;});goto _LL308;_LL319: if(*((
int*)_tmp4A6)!= 8)goto _LL31B;_tmp4AD=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp4A6)->f2;_LL31A: _tmp4AE=_tmp4AD;goto _LL31C;_LL31B: if(*((int*)_tmp4A6)!= 9)
goto _LL31D;_tmp4AE=((struct Cyc_Absyn_Using_d_struct*)_tmp4A6)->f2;_LL31C: _tmp4AF=
_tmp4AE;goto _LL31E;_LL31D: if(*((int*)_tmp4A6)!= 10)goto _LL308;_tmp4AF=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp4A6)->f1;_LL31E: nv=Cyc_Toc_decls_to_c(nv,_tmp4AF,
top);goto _LL308;_LL308:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
