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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};void*Cyc_List_hd(struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern char
Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct
Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*
env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);int Cyc_Dict_forall_c(int(*
f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct
_tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _tagged_arr*Cyc_Absyn_fieldname(int);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(
void*);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero";struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
13]="\000\000\000\000UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*
f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();extern void*Cyc_CfFlowInfo_unknown_none;
extern void*Cyc_CfFlowInfo_unknown_this;extern void*Cyc_CfFlowInfo_unknown_all;int
Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*
leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
void*f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={
0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;static struct Cyc_Set_Set**Cyc_CfFlowInfo_mt_place_set_opt=
0;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){if(Cyc_CfFlowInfo_mt_place_set_opt
== 0)Cyc_CfFlowInfo_mt_place_set_opt=({struct Cyc_Set_Set**_tmp6=_cycalloc(
sizeof(*_tmp6));_tmp6[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*)))Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});
return*((struct Cyc_Set_Set**)_check_null(Cyc_CfFlowInfo_mt_place_set_opt));}int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{int p1;int
p2;{void*_tmp7=r1;struct Cyc_Absyn_Vardecl*_tmp8;struct Cyc_Absyn_Exp*_tmp9;_LL1:
if(*((int*)_tmp7)!= 0)goto _LL3;_tmp8=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp7)->f1;_LL2: p1=(int)_tmp8;goto _LL0;_LL3: if(*((int*)_tmp7)!= 1)goto _LL0;_tmp9=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp7)->f1;_LL4: p1=(int)_tmp9;_LL0:;}{void*
_tmpA=r2;struct Cyc_Absyn_Vardecl*_tmpB;struct Cyc_Absyn_Exp*_tmpC;_LL6: if(*((int*)
_tmpA)!= 0)goto _LL8;_tmpB=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpA)->f1;_LL7:
p2=(int)_tmpB;goto _LL5;_LL8: if(*((int*)_tmpA)!= 1)goto _LL5;_tmpC=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmpA)->f1;_LL9: p2=(int)_tmpC;_LL5:;}return p1 - p2;}}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)
p2)return 0;{int i=Cyc_CfFlowInfo_root_cmp((void*)p1->root,(void*)p2->root);if(i != 
0)return i;return((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_zstrptrcmp,
p1->fields,p2->fields);}}static struct _tagged_arr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;{void*_tmpD=(void*)p->root;
struct Cyc_Absyn_Vardecl*_tmpE;struct Cyc_Absyn_Exp*_tmpF;_LLB: if(*((int*)_tmpD)!= 
0)goto _LLD;_tmpE=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpD)->f1;_LLC: sl=({
struct Cyc_List_List*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->hd=({struct
_tagged_arr*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=({struct Cyc_Std_String_pa_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=(struct _tagged_arr)*(*_tmpE->name).f2;{void*_tmp12[
1]={& _tmp13};Cyc_Std_aprintf(_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp12,
sizeof(void*),1));}});_tmp11;});_tmp10->tl=sl;_tmp10;});goto _LLA;_LLD: if(*((int*)
_tmpD)!= 1)goto _LLA;_tmpF=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpD)->f1;
_LLE: sl=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=({
struct _tagged_arr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Std_Int_pa_struct
_tmp17;_tmp17.tag=1;_tmp17.f1=(int)((unsigned int)((int)_tmpF));{void*_tmp16[1]={&
_tmp17};Cyc_Std_aprintf(_tag_arr("mpt%d",sizeof(char),6),_tag_arr(_tmp16,sizeof(
void*),1));}});_tmp15;});_tmp14->tl=sl;_tmp14;});_LLA:;}{struct Cyc_List_List*
fields=p->fields;for(0;fields != 0;fields=fields->tl){sl=({struct Cyc_List_List*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->hd=({struct _tagged_arr*_tmp19=
_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Std_String_pa_struct _tmp1B;
_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)*((struct _tagged_arr*)fields->hd);{
void*_tmp1A[1]={& _tmp1B};Cyc_Std_aprintf(_tag_arr("%s",sizeof(char),3),_tag_arr(
_tmp1A,sizeof(void*),1));}});_tmp19;});_tmp18->tl=sl;_tmp18;});}}{struct
_tagged_arr*_tmp1C=({struct _tagged_arr*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[
0]=({struct Cyc_Std_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=_tag_arr("",
sizeof(char),1);{void*_tmp21[1]={& _tmp22};Cyc_Std_aprintf(_tag_arr("%s",sizeof(
char),3),_tag_arr(_tmp21,sizeof(void*),1));}});_tmp20;});for(0;sl != 0;sl=sl->tl){*
_tmp1C=({struct Cyc_Std_String_pa_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct
_tagged_arr)*_tmp1C;{struct Cyc_Std_String_pa_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(
struct _tagged_arr)*((struct _tagged_arr*)sl->hd);{void*_tmp1D[2]={& _tmp1E,& _tmp1F};
Cyc_Std_aprintf(_tag_arr("%s.%s",sizeof(char),6),_tag_arr(_tmp1D,sizeof(void*),2));}}});}
return _tmp1C;}}struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval){if(!Cyc_Absyn_is_union_type(t)){void*_tmp23=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_TunionFieldInfo _tmp24;void*_tmp25;struct Cyc_Absyn_Tunionfield*
_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_Absyn_AggrInfo _tmp28;void*_tmp29;
void*_tmp2A;struct Cyc_List_List*_tmp2B;_LL10: if(_tmp23 <= (void*)3?1:*((int*)
_tmp23)!= 3)goto _LL12;_tmp24=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp23)->f1;
_tmp25=(void*)_tmp24.field_info;if(*((int*)_tmp25)!= 1)goto _LL12;_tmp26=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp25)->f2;_LL11: if(_tmp26->typs == 0)return
leafval;_tmp27=_tmp26->typs;goto _LL13;_LL12: if(_tmp23 <= (void*)3?1:*((int*)
_tmp23)!= 9)goto _LL14;_tmp27=((struct Cyc_Absyn_TupleType_struct*)_tmp23)->f1;
_LL13: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;
for(0;_tmp27 != 0;(_tmp27=_tmp27->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_typ_to_absrval((*((
struct _tuple4*)_tmp27->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2D;_tmp2D.tag=3;_tmp2D.f1=d;_tmp2D;});_tmp2C;});}_LL14: if(_tmp23 <= (void*)3?
1:*((int*)_tmp23)!= 10)goto _LL16;_tmp28=((struct Cyc_Absyn_AggrType_struct*)
_tmp23)->f1;_tmp29=(void*)_tmp28.aggr_info;_LL15: {struct Cyc_Absyn_Aggrdecl*
_tmp2E=Cyc_Absyn_get_known_aggrdecl(_tmp29);if(_tmp2E->impl == 0)goto _LLF;_tmp2B=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2E->impl))->fields;goto _LL17;}
_LL16: if(_tmp23 <= (void*)3?1:*((int*)_tmp23)!= 11)goto _LL18;_tmp2A=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp23)->f1;if((int)_tmp2A != 0)goto _LL18;
_tmp2B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp23)->f2;_LL17: {struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))
Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);for(0;_tmp2B != 0;_tmp2B=_tmp2B->tl){struct
Cyc_Absyn_Aggrfield _tmp30;struct _tagged_arr*_tmp31;void*_tmp32;struct Cyc_Absyn_Aggrfield*
_tmp2F=(struct Cyc_Absyn_Aggrfield*)_tmp2B->hd;_tmp30=*_tmp2F;_tmp31=_tmp30.name;
_tmp32=(void*)_tmp30.type;if(_get_arr_size(*_tmp31,sizeof(char))!= 1)d=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(
d,_tmp31,Cyc_CfFlowInfo_typ_to_absrval(_tmp32,leafval));}return(void*)({struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp34;_tmp34.tag=3;_tmp34.f1=d;_tmp34;});
_tmp33;});}_LL18:;_LL19: goto _LLF;_LLF:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all:
leafval;}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Set_Set*set){struct Cyc_CfFlowInfo_Place*
_tmp35=place;struct Cyc_Iter_Iter _tmp36=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(r,set);while(((int(*)(
struct Cyc_Iter_Iter,struct Cyc_CfFlowInfo_Place**))Cyc_Iter_next)(_tmp36,& _tmp35)){
if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp35->root)!= 0)continue;{
struct Cyc_List_List*_tmp37=place->fields;struct Cyc_List_List*_tmp38=_tmp35->fields;
for(0;_tmp37 != 0?_tmp38 != 0: 0;(_tmp37=_tmp37->tl,_tmp38=_tmp38->tl)){if(Cyc_Std_zstrptrcmp((
struct _tagged_arr*)_tmp37->hd,(struct _tagged_arr*)_tmp38->hd)!= 0)break;}if(
_tmp37 == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmp39=_region_malloc(pile->rgn,sizeof(*_tmp39));_tmp39->hd=place;
_tmp39->tl=pile->places;_tmp39;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*a,void*r){void*_tmp3A=r;struct Cyc_CfFlowInfo_Place*_tmp3B;struct Cyc_Dict_Dict*
_tmp3C;_LL1B: if(_tmp3A <= (void*)3?1:*((int*)_tmp3A)!= 2)goto _LL1D;_tmp3B=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3A)->f1;_LL1C: Cyc_CfFlowInfo_add_place(
pile,_tmp3B);return;_LL1D: if(_tmp3A <= (void*)3?1:*((int*)_tmp3A)!= 3)goto _LL1F;
_tmp3C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3A)->f1;_LL1E:((void(*)(void(*
f)(struct Cyc_CfFlowInfo_EscPile*,struct _tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((void(*)(struct Cyc_CfFlowInfo_EscPile*
pile,struct _tagged_arr*a,void*r))Cyc_CfFlowInfo_add_places,pile,_tmp3C);return;
_LL1F:;_LL20: return;_LL1A:;}static void*Cyc_CfFlowInfo_insert_place_inner(void*
new_val,void*old_val){void*_tmp3D=old_val;struct Cyc_Dict_Dict*_tmp3E;_LL22: if(
_tmp3D <= (void*)3?1:*((int*)_tmp3D)!= 3)goto _LL24;_tmp3E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp3D)->f1;_LL23: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp3F=
_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp40;_tmp40.tag=3;_tmp40.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp3E);_tmp40;});_tmp3F;});_LL24:;_LL25: return new_val;_LL21:;}struct
_tuple5{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple5 _tmp42=({struct _tuple5 _tmp41;_tmp41.f1=fs;_tmp41.f2=
old_val;_tmp41;});struct Cyc_List_List*_tmp43;struct Cyc_List_List _tmp44;struct
_tagged_arr*_tmp45;struct Cyc_List_List*_tmp46;void*_tmp47;struct Cyc_Dict_Dict*
_tmp48;_LL27: _tmp43=_tmp42.f1;if(_tmp43 == 0)goto _LL29;_tmp44=*_tmp43;_tmp45=(
struct _tagged_arr*)_tmp44.hd;_tmp46=_tmp44.tl;_tmp47=_tmp42.f2;if(_tmp47 <= (void*)
3?1:*((int*)_tmp47)!= 3)goto _LL29;_tmp48=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp47)->f1;_LL28: {void*_tmp49=Cyc_CfFlowInfo_insert_place_outer(_tmp46,((void*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp48,_tmp45),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4A=_cycalloc(
sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp4B;_tmp4B.tag=
3;_tmp4B.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp48,_tmp45,_tmp49);_tmp4B;});_tmp4A;});}_LL29:;_LL2A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4C=_cycalloc(sizeof(*
_tmp4C));_tmp4C[0]=({struct Cyc_Core_Impossible_struct _tmp4D;_tmp4D.tag=Cyc_Core_Impossible;
_tmp4D.f1=_tag_arr("bad insert place",sizeof(char),17);_tmp4D;});_tmp4C;}));
_LL26:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_Set_Set**all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 
0){struct Cyc_CfFlowInfo_Place*_tmp4E=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)
_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)_check_null(
pile->places))->tl;if(all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,
_tmp4E);{void*oldval;void*newval;{struct _handler_cons _tmp4F;_push_handler(&
_tmp4F);{int _tmp51=0;if(setjmp(_tmp4F.handler))_tmp51=1;if(!_tmp51){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp4E);;_pop_handler();}else{void*_tmp50=(void*)_exn_thrown;void*_tmp53=
_tmp50;_LL2C: if(_tmp53 != Cyc_Dict_Absent)goto _LL2E;_LL2D: continue;_LL2E:;_LL2F:(
void)_throw(_tmp53);_LL2B:;}}}{void*_tmp54=Cyc_CfFlowInfo_initlevel(d,oldval);
_LL31: if((int)_tmp54 != 2)goto _LL33;_LL32: newval=Cyc_CfFlowInfo_esc_all;goto _LL30;
_LL33: if((int)_tmp54 != 1)goto _LL35;_LL34: newval=Cyc_CfFlowInfo_esc_this;goto
_LL30;_LL35: if((int)_tmp54 != 0)goto _LL30;_LL36: newval=Cyc_CfFlowInfo_esc_none;
goto _LL30;_LL30:;}((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp4E->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp4E->fields,Cyc_Dict_lookup(d,(void*)_tmp4E->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp55=r;void*_tmp56;
void*_tmp57;_LL38: if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 0)goto _LL3A;_tmp56=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp55)->f1;_LL39: return _tmp56;
_LL3A: if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 1)goto _LL3C;_tmp57=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp55)->f1;_LL3B: return _tmp57;_LL3C: if((int)
_tmp55 != 0)goto _LL3E;_LL3D: goto _LL3F;_LL3E: if((int)_tmp55 != 1)goto _LL40;_LL3F:
return(void*)2;_LL40: if((int)_tmp55 != 2)goto _LL42;_LL41: return(void*)1;_LL42:;
_LL43:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp58=_cycalloc(
sizeof(*_tmp58));_tmp58[0]=({struct Cyc_Core_Impossible_struct _tmp59;_tmp59.tag=
Cyc_Core_Impossible;_tmp59.f1=_tag_arr("initlevel_approx",sizeof(char),17);
_tmp59;});_tmp58;}));_LL37:;}static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*a,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*
_tmp5A=r;struct Cyc_Dict_Dict*_tmp5B;struct Cyc_CfFlowInfo_Place*_tmp5C;_LL45: if(
_tmp5A <= (void*)3?1:*((int*)_tmp5A)!= 3)goto _LL47;_tmp5B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5A)->f1;_LL46: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp5B,(
void*)2);goto _LL44;_LL47: if(_tmp5A <= (void*)3?1:*((int*)_tmp5A)!= 2)goto _LL49;
_tmp5C=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5A)->f1;_LL48: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp5C))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmp5D=_cycalloc(
sizeof(*_tmp5D));_tmp5D->hd=_tmp5C;_tmp5D->tl=env->seen;_tmp5D;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmp5C),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL44;
_LL49:;_LL4A: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL44:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1?1: acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp5E=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmp5F;_tmp5F.d=d;_tmp5F.seen=0;_tmp5F;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmp5E,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmp61;void*_tmp62;struct Cyc_List_List*_tmp63;
struct Cyc_CfFlowInfo_Place*_tmp60=place;_tmp61=*_tmp60;_tmp62=(void*)_tmp61.root;
_tmp63=_tmp61.fields;{void*_tmp64=Cyc_Dict_lookup(d,_tmp62);for(0;_tmp63 != 0;
_tmp63=_tmp63->tl){struct _tuple1 _tmp66=({struct _tuple1 _tmp65;_tmp65.f1=_tmp64;
_tmp65.f2=(struct _tagged_arr*)_tmp63->hd;_tmp65;});void*_tmp67;struct Cyc_Dict_Dict*
_tmp68;struct _tagged_arr*_tmp69;_LL4C: _tmp67=_tmp66.f1;if(_tmp67 <= (void*)3?1:*((
int*)_tmp67)!= 3)goto _LL4E;_tmp68=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp67)->f1;_tmp69=_tmp66.f2;_LL4D: _tmp64=((void*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp68,_tmp69);goto _LL4B;_LL4E:;_LL4F:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));
_tmp6A[0]=({struct Cyc_Core_Impossible_struct _tmp6B;_tmp6B.tag=Cyc_Core_Impossible;
_tmp6B.f1=_tag_arr("bad lookup_place",sizeof(char),17);_tmp6B;});_tmp6A;}));
_LL4B:;}return _tmp64;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,
void*rval){void*_tmp6C=rval;struct Cyc_Dict_Dict*_tmp6D;_LL51: if(_tmp6C <= (void*)
3?1:*((int*)_tmp6C)!= 1)goto _LL53;_LL52: return 0;_LL53: if(_tmp6C <= (void*)3?1:*((
int*)_tmp6C)!= 3)goto _LL55;_tmp6D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp6C)->f1;_LL54: return((int(*)(int(*f)(int,struct _tagged_arr*,void*),int env,
struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((int(*)(int a,struct _tagged_arr*b,void*
rval))Cyc_CfFlowInfo_is_rval_unescaped,0,_tmp6D);_LL55:;_LL56: return 1;_LL50:;}
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(0,0,
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp6E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp70=
_region_malloc(rgn,sizeof(*_tmp70));_tmp70->rgn=rgn;_tmp70->places=0;_tmp70;});((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp6F=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp6F;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmp72=({struct _tuple0 _tmp71;
_tmp71.f1=oldval;_tmp71.f2=newval;_tmp71;});void*_tmp73;void*_tmp74;struct Cyc_CfFlowInfo_Place*
_tmp75;void*_tmp76;void*_tmp77;struct Cyc_Dict_Dict*_tmp78;void*_tmp79;struct Cyc_Dict_Dict*
_tmp7A;void*_tmp7B;void*_tmp7C;_LL58: _tmp73=_tmp72.f1;if(_tmp73 <= (void*)3?1:*((
int*)_tmp73)!= 1)goto _LL5A;_tmp74=_tmp72.f2;if(_tmp74 <= (void*)3?1:*((int*)
_tmp74)!= 2)goto _LL5A;_tmp75=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp74)->f1;
_LL59: Cyc_CfFlowInfo_add_place(env->pile,_tmp75);goto _LL5B;_LL5A: _tmp76=_tmp72.f1;
if(_tmp76 <= (void*)3?1:*((int*)_tmp76)!= 1)goto _LL5C;_LL5B: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmp7D[0]={};Cyc_Tcutil_terr(env->loc,_tag_arr("assignment puts possibly-uninitialized data in an escaped location",
sizeof(char),67),_tag_arr(_tmp7D,sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;
_LL5C: _tmp77=_tmp72.f1;if(_tmp77 <= (void*)3?1:*((int*)_tmp77)!= 3)goto _LL5E;
_tmp78=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp77)->f1;_tmp79=_tmp72.f2;if(
_tmp79 <= (void*)3?1:*((int*)_tmp79)!= 3)goto _LL5E;_tmp7A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp79)->f1;_LL5D: {struct Cyc_Dict_Dict*_tmp7E=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_AssignEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,struct _tagged_arr*a,void*oldval,void*newval))
Cyc_CfFlowInfo_assign_place_inner,env,_tmp78,_tmp7A);if(_tmp7E == _tmp78)return
oldval;if(_tmp7E == _tmp7A)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp80;_tmp80.tag=3;_tmp80.f1=_tmp7E;_tmp80;});_tmp7F;});}_LL5E: _tmp7B=_tmp72.f2;
if(_tmp7B <= (void*)3?1:*((int*)_tmp7B)!= 1)goto _LL60;_tmp7C=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp7B)->f1;_LL5F: {void*_tmp81=_tmp7C;_LL63: if((int)_tmp81 != 0)goto _LL65;_LL64:
return Cyc_CfFlowInfo_unknown_none;_LL65: if((int)_tmp81 != 1)goto _LL67;_LL66:
return Cyc_CfFlowInfo_unknown_this;_LL67: if((int)_tmp81 != 2)goto _LL62;_LL68:
return Cyc_CfFlowInfo_unknown_all;_LL62:;}_LL60:;_LL61: return newval;_LL57:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple5 _tmp83=({struct _tuple5 _tmp82;_tmp82.f1=fs;
_tmp82.f2=oldval;_tmp82;});struct Cyc_List_List*_tmp84;struct Cyc_List_List _tmp85;
struct _tagged_arr*_tmp86;struct Cyc_List_List*_tmp87;void*_tmp88;struct Cyc_Dict_Dict*
_tmp89;_LL6A: _tmp84=_tmp83.f1;if(_tmp84 == 0)goto _LL6C;_tmp85=*_tmp84;_tmp86=(
struct _tagged_arr*)_tmp85.hd;_tmp87=_tmp85.tl;_tmp88=_tmp83.f2;if(_tmp88 <= (void*)
3?1:*((int*)_tmp88)!= 3)goto _LL6C;_tmp89=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp88)->f1;_LL6B: {void*_tmp8A=Cyc_CfFlowInfo_assign_place_outer(env,_tmp87,((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp89,
_tmp86),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp8B=
_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp8C;_tmp8C.tag=3;_tmp8C.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmp89,_tmp86,_tmp8A);_tmp8C;});
_tmp8B;});}_LL6C:;_LL6D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Core_Impossible_struct
_tmp8E;_tmp8E.tag=Cyc_Core_Impossible;_tmp8E.f1=_tag_arr("bad insert place",
sizeof(char),17);_tmp8E;});_tmp8D;}));_LL69:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){if(all_changed != 0)*
all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*
elt))Cyc_Set_insert)(*all_changed,place);{struct _RegionHandle _tmp8F=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp8F;_push_region(rgn);{struct Cyc_CfFlowInfo_Place
_tmp91;void*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_CfFlowInfo_Place*_tmp90=
place;_tmp91=*_tmp90;_tmp92=(void*)_tmp91.root;_tmp93=_tmp91.fields;{struct Cyc_CfFlowInfo_AssignEnv
env=({struct Cyc_CfFlowInfo_AssignEnv _tmp95;_tmp95.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp96=_region_malloc(rgn,sizeof(*_tmp96));_tmp96->rgn=rgn;_tmp96->places=0;
_tmp96;});_tmp95.d=d;_tmp95.loc=loc;_tmp95;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp93,Cyc_Dict_lookup(d,_tmp92),r);struct Cyc_Dict_Dict*_tmp94=Cyc_CfFlowInfo_escape_these(
env.pile,all_changed,Cyc_Dict_insert(d,_tmp92,newval));_npop_handler(0);return
_tmp94;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmp98=({struct _tuple0 _tmp97;_tmp97.f1=r1;_tmp97.f2=r2;_tmp97;});
void*_tmp99;struct Cyc_CfFlowInfo_Place*_tmp9A;void*_tmp9B;struct Cyc_CfFlowInfo_Place*
_tmp9C;void*_tmp9D;struct Cyc_CfFlowInfo_Place*_tmp9E;void*_tmp9F;struct Cyc_CfFlowInfo_Place*
_tmpA0;void*_tmpA1;void*_tmpA2;void*_tmpA3;void*_tmpA4;void*_tmpA5;struct Cyc_Dict_Dict*
_tmpA6;void*_tmpA7;struct Cyc_Dict_Dict*_tmpA8;_LL6F: _tmp99=_tmp98.f1;if(_tmp99 <= (
void*)3?1:*((int*)_tmp99)!= 2)goto _LL71;_tmp9A=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp99)->f1;_tmp9B=_tmp98.f2;if(_tmp9B <= (void*)3?1:*((int*)_tmp9B)!= 2)goto
_LL71;_tmp9C=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp9B)->f1;_LL70: if(Cyc_CfFlowInfo_place_cmp(
_tmp9A,_tmp9C)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp9A);Cyc_CfFlowInfo_add_place(
env->pile,_tmp9C);goto _LL6E;_LL71: _tmp9D=_tmp98.f1;if(_tmp9D <= (void*)3?1:*((int*)
_tmp9D)!= 2)goto _LL73;_tmp9E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp9D)->f1;
_LL72: Cyc_CfFlowInfo_add_place(env->pile,_tmp9E);goto _LL6E;_LL73: _tmp9F=_tmp98.f2;
if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 2)goto _LL75;_tmpA0=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9F)->f1;_LL74: Cyc_CfFlowInfo_add_place(env->pile,_tmpA0);goto _LL6E;_LL75:
_tmpA1=_tmp98.f1;if((int)_tmpA1 != 1)goto _LL77;_tmpA2=_tmp98.f2;if((int)_tmpA2 != 
2)goto _LL77;_LL76: goto _LL78;_LL77: _tmpA3=_tmp98.f1;if((int)_tmpA3 != 2)goto _LL79;
_tmpA4=_tmp98.f2;if((int)_tmpA4 != 1)goto _LL79;_LL78: return(void*)2;_LL79: _tmpA5=
_tmp98.f1;if(_tmpA5 <= (void*)3?1:*((int*)_tmpA5)!= 3)goto _LL7B;_tmpA6=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpA5)->f1;_tmpA7=_tmp98.f2;if(_tmpA7 <= (void*)
3?1:*((int*)_tmpA7)!= 3)goto _LL7B;_tmpA8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA7)->f1;_LL7A: {struct Cyc_Dict_Dict*_tmpA9=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,
env,_tmpA6,_tmpA8);if(_tmpA9 == _tmpA6)return r1;if(_tmpA9 == _tmpA8)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpAA=_cycalloc(sizeof(*
_tmpAA));_tmpAA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpAB;_tmpAB.tag=3;
_tmpAB.f1=_tmpA9;_tmpAB;});_tmpAA;});}_LL7B:;_LL7C: goto _LL6E;_LL6E:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmpAD=({struct _tuple0 _tmpAC;_tmpAC.f1=r1;_tmpAC.f2=r2;_tmpAC;});
void*_tmpAE;void*_tmpAF;_LL7E: _tmpAE=_tmpAD.f1;if(_tmpAE <= (void*)3?1:*((int*)
_tmpAE)!= 1)goto _LL80;_LL7F: goto _LL81;_LL80: _tmpAF=_tmpAD.f2;if(_tmpAF <= (void*)
3?1:*((int*)_tmpAF)!= 1)goto _LL82;_LL81: {struct _tuple0 _tmpB1=({struct _tuple0
_tmpB0;_tmpB0.f1=il1;_tmpB0.f2=il2;_tmpB0;});void*_tmpB2;void*_tmpB3;void*_tmpB4;
void*_tmpB5;_LL85: _tmpB2=_tmpB1.f2;if((int)_tmpB2 != 0)goto _LL87;_LL86: goto _LL88;
_LL87: _tmpB3=_tmpB1.f1;if((int)_tmpB3 != 0)goto _LL89;_LL88: return Cyc_CfFlowInfo_esc_none;
_LL89: _tmpB4=_tmpB1.f2;if((int)_tmpB4 != 1)goto _LL8B;_LL8A: goto _LL8C;_LL8B: _tmpB5=
_tmpB1.f1;if((int)_tmpB5 != 1)goto _LL8D;_LL8C: return Cyc_CfFlowInfo_esc_this;_LL8D:;
_LL8E: return Cyc_CfFlowInfo_esc_all;_LL84:;}_LL82:;_LL83: {struct _tuple0 _tmpB7=({
struct _tuple0 _tmpB6;_tmpB6.f1=il1;_tmpB6.f2=il2;_tmpB6;});void*_tmpB8;void*
_tmpB9;void*_tmpBA;void*_tmpBB;_LL90: _tmpB8=_tmpB7.f2;if((int)_tmpB8 != 0)goto
_LL92;_LL91: goto _LL93;_LL92: _tmpB9=_tmpB7.f1;if((int)_tmpB9 != 0)goto _LL94;_LL93:
return Cyc_CfFlowInfo_unknown_none;_LL94: _tmpBA=_tmpB7.f2;if((int)_tmpBA != 1)goto
_LL96;_LL95: goto _LL97;_LL96: _tmpBB=_tmpB7.f1;if((int)_tmpBB != 1)goto _LL98;_LL97:
return Cyc_CfFlowInfo_unknown_this;_LL98:;_LL99: return Cyc_CfFlowInfo_unknown_all;
_LL8F:;}_LL7D:;}}int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)return
1;{struct _tuple0 _tmpBD=({struct _tuple0 _tmpBC;_tmpBC.f1=r1;_tmpBC.f2=r2;_tmpBC;});
void*_tmpBE;unsigned int _tmpBF;void*_tmpC0;unsigned int _tmpC1;void*_tmpC2;struct
Cyc_Absyn_Vardecl*_tmpC3;void*_tmpC4;struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;
struct Cyc_Absyn_Vardecl*_tmpC7;void*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*
_tmpCA;unsigned int _tmpCB;void*_tmpCC;unsigned int _tmpCD;void*_tmpCE;struct Cyc_Absyn_Vardecl*
_tmpCF;void*_tmpD0;struct Cyc_Absyn_Vardecl*_tmpD1;_LL9B: _tmpBE=_tmpBD.f1;if(*((
int*)_tmpBE)!= 0)goto _LL9D;_tmpBF=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpBE)->f1;_tmpC0=_tmpBD.f2;if(*((int*)_tmpC0)!= 0)goto _LL9D;_tmpC1=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpC0)->f1;_LL9C: return _tmpBF == _tmpC1;_LL9D: _tmpC2=_tmpBD.f1;if(*((int*)_tmpC2)
!= 1)goto _LL9F;_tmpC3=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmpC2)->f1;_tmpC4=
_tmpBD.f2;if(*((int*)_tmpC4)!= 1)goto _LL9F;_tmpC5=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmpC4)->f1;_LL9E: return _tmpC3 == _tmpC5;_LL9F: _tmpC6=_tmpBD.f1;if(*((int*)_tmpC6)
!= 2)goto _LLA1;_tmpC7=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpC6)->f1;_tmpC8=
_tmpBD.f2;if(*((int*)_tmpC8)!= 2)goto _LLA1;_tmpC9=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmpC8)->f1;_LLA0: return _tmpC7 == _tmpC9;_LLA1: _tmpCA=_tmpBD.f1;if(*((int*)_tmpCA)
!= 3)goto _LLA3;_tmpCB=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmpCA)->f1;
_tmpCC=_tmpBD.f2;if(*((int*)_tmpCC)!= 3)goto _LLA3;_tmpCD=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmpCC)->f1;_LLA2: return _tmpCB == _tmpCD;_LLA3: _tmpCE=_tmpBD.f1;if(*((int*)_tmpCE)
!= 4)goto _LLA5;_tmpCF=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmpCE)->f1;
_tmpD0=_tmpBD.f2;if(*((int*)_tmpD0)!= 4)goto _LLA5;_tmpD1=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmpD0)->f1;_LLA4: return _tmpCF == _tmpD1;_LLA5:;_LLA6: return 0;_LL9A:;}}struct Cyc_List_List*
Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(
r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmpD2=r1s;for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){struct Cyc_CfFlowInfo_Reln*_tmpD3=(
struct Cyc_CfFlowInfo_Reln*)_tmpD2->hd;int found=0;{struct Cyc_List_List*_tmpD4=r2s;
for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){struct Cyc_CfFlowInfo_Reln*_tmpD5=(struct Cyc_CfFlowInfo_Reln*)
_tmpD4->hd;if(_tmpD3 == _tmpD5?1:(_tmpD3->vd == _tmpD5->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmpD3->rop,(void*)_tmpD5->rop): 0)){res=({struct Cyc_List_List*_tmpD6=
_cycalloc(sizeof(*_tmpD6));_tmpD6->hd=_tmpD3;_tmpD6->tl=res;_tmpD6;});found=1;
break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;}}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2){if(f1 == f2)return f1;{struct
_tuple0 _tmpD8=({struct _tuple0 _tmpD7;_tmpD7.f1=f1;_tmpD7.f2=f2;_tmpD7;});void*
_tmpD9;void*_tmpDA;void*_tmpDB;struct Cyc_Dict_Dict*_tmpDC;struct Cyc_List_List*
_tmpDD;void*_tmpDE;struct Cyc_Dict_Dict*_tmpDF;struct Cyc_List_List*_tmpE0;_LLA8:
_tmpD9=_tmpD8.f1;if((int)_tmpD9 != 0)goto _LLAA;_LLA9: return f2;_LLAA: _tmpDA=_tmpD8.f2;
if((int)_tmpDA != 0)goto _LLAC;_LLAB: return f1;_LLAC: _tmpDB=_tmpD8.f1;if(_tmpDB <= (
void*)1?1:*((int*)_tmpDB)!= 0)goto _LLA7;_tmpDC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpDB)->f1;_tmpDD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpDB)->f2;_tmpDE=
_tmpD8.f2;if(_tmpDE <= (void*)1?1:*((int*)_tmpDE)!= 0)goto _LLA7;_tmpDF=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmpDE)->f1;_tmpE0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpDE)->f2;_LLAD: if(_tmpDC == _tmpDF?_tmpDD == _tmpE0: 0)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmpE1=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE1;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmpE2=({struct Cyc_CfFlowInfo_JoinEnv
_tmpE8;_tmpE8.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpE9=_region_malloc(rgn,
sizeof(*_tmpE9));_tmpE9->rgn=rgn;_tmpE9->places=0;_tmpE9;});_tmpE8.d1=_tmpDC;
_tmpE8.d2=_tmpDF;_tmpE8;});struct Cyc_Dict_Dict*_tmpE3=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmpE2,_tmpDC,_tmpDF);struct Cyc_List_List*_tmpE4=Cyc_CfFlowInfo_join_relns(
_tmpDD,_tmpE0);void*_tmpE7=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpE6;_tmpE6.tag=0;_tmpE6.f1=Cyc_CfFlowInfo_escape_these(_tmpE2.pile,
all_changed,_tmpE3);_tmpE6.f2=_tmpE4;_tmpE6;});_tmpE5;});_npop_handler(0);return
_tmpE7;};_pop_region(rgn);}_LLA7:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == 
Cyc_CfFlowInfo_One?1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))
Cyc_Set_member)(env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg2,env->curr_place);if(changed1?changed2: 0)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmpEB=({struct _tuple0 _tmpEA;_tmpEA.f1=r1;_tmpEA.f2=r2;_tmpEA;});
void*_tmpEC;struct Cyc_Dict_Dict*_tmpED;void*_tmpEE;struct Cyc_Dict_Dict*_tmpEF;
_LLAF: _tmpEC=_tmpEB.f1;if(_tmpEC <= (void*)3?1:*((int*)_tmpEC)!= 3)goto _LLB1;
_tmpED=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpEC)->f1;_tmpEE=_tmpEB.f2;if(
_tmpEE <= (void*)3?1:*((int*)_tmpEE)!= 3)goto _LLB1;_tmpEF=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpEE)->f1;_LLB0: {struct Cyc_Dict_Dict*_tmpF0=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmpED,_tmpEF);if(_tmpF0 == _tmpED)return r1;if(_tmpF0 == _tmpEF)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF1=_cycalloc(sizeof(*
_tmpF1));_tmpF1[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpF2;_tmpF2.tag=3;
_tmpF2.f1=_tmpF0;_tmpF2;});_tmpF1;});}_LLB1:;_LLB2:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct
Cyc_Core_Impossible_struct _tmpF4;_tmpF4.tag=Cyc_Core_Impossible;_tmpF4.f1=
_tag_arr("after_pathinfo -- non-aggregates!",sizeof(char),34);_tmpF4;});_tmpF3;}));
_LLAE:;}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*
env,struct _tagged_arr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmpF5=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmpF6=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmpF7=_cycalloc(sizeof(*_tmpF7));
_tmpF7->hd=key;_tmpF7->tl=0;_tmpF7;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmpF8=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmpF5;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmpF6;return _tmpF8;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmpF9;_tmpF9.root=(void*)root;
_tmpF9.fields=0;_tmpF9;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2,struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((void*)0)};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmpFB=({struct _tuple0 _tmpFA;_tmpFA.f1=f1;_tmpFA.f2=f2;_tmpFA;});void*_tmpFC;
void*_tmpFD;void*_tmpFE;struct Cyc_Dict_Dict*_tmpFF;struct Cyc_List_List*_tmp100;
void*_tmp101;struct Cyc_Dict_Dict*_tmp102;struct Cyc_List_List*_tmp103;_LLB4:
_tmpFC=_tmpFB.f1;if((int)_tmpFC != 0)goto _LLB6;_LLB5: goto _LLB7;_LLB6: _tmpFD=
_tmpFB.f2;if((int)_tmpFD != 0)goto _LLB8;_LLB7: return(void*)0;_LLB8: _tmpFE=_tmpFB.f1;
if(_tmpFE <= (void*)1?1:*((int*)_tmpFE)!= 0)goto _LLB3;_tmpFF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFE)->f1;_tmp100=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFE)->f2;
_tmp101=_tmpFB.f2;if(_tmp101 <= (void*)1?1:*((int*)_tmp101)!= 0)goto _LLB3;_tmp102=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp101)->f1;_tmp103=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp101)->f2;_LLB9: if(_tmpFF == _tmp102?_tmp100 == _tmp103: 0)return f1;{struct
_RegionHandle _tmp104=_new_region("rgn");struct _RegionHandle*rgn=& _tmp104;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place*_tmp105=({struct Cyc_CfFlowInfo_Place*
_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->root=(void*)((void*)& dummy_root);
_tmp10F->fields=0;_tmp10F;});struct Cyc_CfFlowInfo_AfterEnv _tmp106=({struct Cyc_CfFlowInfo_AfterEnv
_tmp10C;_tmp10C.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp10D;_tmp10D.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp10E=_region_malloc(rgn,sizeof(*_tmp10E));
_tmp10E->rgn=rgn;_tmp10E->places=0;_tmp10E;});_tmp10D.d1=_tmpFF;_tmp10D.d2=
_tmp102;_tmp10D;});_tmp10C.chg1=chg1;_tmp10C.chg2=chg2;_tmp10C.curr_place=
_tmp105;_tmp10C.last_field_cell=& _tmp105->fields;_tmp10C.changed=Cyc_CfFlowInfo_Neither;
_tmp10C;});struct Cyc_Dict_Dict*_tmp107=((struct Cyc_Dict_Dict*(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp106,_tmpFF,_tmp102);struct Cyc_List_List*_tmp108=Cyc_CfFlowInfo_join_relns(
_tmp100,_tmp103);void*_tmp10B=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp10A;_tmp10A.tag=0;_tmp10A.f1=Cyc_CfFlowInfo_escape_these((_tmp106.joinenv).pile,
all_changed,_tmp107);_tmp10A.f2=_tmp108;_tmp10A;});_tmp109;});_npop_handler(0);
return _tmp10B;};_pop_region(rgn);}_LLB3:;}}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp113=({struct
_tuple0 _tmp112;_tmp112.f1=r1;_tmp112.f2=r2;_tmp112;});void*_tmp114;struct Cyc_CfFlowInfo_Place*
_tmp115;void*_tmp116;struct Cyc_CfFlowInfo_Place*_tmp117;void*_tmp118;void*
_tmp119;void*_tmp11A;struct Cyc_Dict_Dict*_tmp11B;void*_tmp11C;struct Cyc_Dict_Dict*
_tmp11D;void*_tmp11E;void*_tmp11F;void*_tmp120;void*_tmp121;void*_tmp122;void*
_tmp123;_LLBB: _tmp114=_tmp113.f1;if(_tmp114 <= (void*)3?1:*((int*)_tmp114)!= 2)
goto _LLBD;_tmp115=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp114)->f1;_tmp116=
_tmp113.f2;if(_tmp116 <= (void*)3?1:*((int*)_tmp116)!= 2)goto _LLBD;_tmp117=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp116)->f1;_LLBC: return Cyc_CfFlowInfo_place_cmp(
_tmp115,_tmp117)== 0;_LLBD: _tmp118=_tmp113.f1;if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 2)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp119=_tmp113.f2;if(_tmp119 <= (
void*)3?1:*((int*)_tmp119)!= 2)goto _LLC1;_LLC0: return 0;_LLC1: _tmp11A=_tmp113.f1;
if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 3)goto _LLC3;_tmp11B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp11A)->f1;_tmp11C=_tmp113.f2;if(_tmp11C <= (void*)3?1:*((int*)_tmp11C)!= 3)
goto _LLC3;_tmp11D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp11C)->f1;_LLC2:
return _tmp11B == _tmp11D?1:((int(*)(int(*f)(struct _tagged_arr*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((int(*)(
struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp11B,_tmp11D);_LLC3: _tmp11E=_tmp113.f2;if((int)_tmp11E != 2)goto _LLC5;_LLC4:
return r1 == (void*)1;_LLC5: _tmp11F=_tmp113.f2;if((int)_tmp11F != 0)goto _LLC7;_LLC6:
goto _LLC8;_LLC7: _tmp120=_tmp113.f2;if((int)_tmp120 != 1)goto _LLC9;_LLC8: return 0;
_LLC9: _tmp121=_tmp113.f1;if(_tmp121 <= (void*)3?1:*((int*)_tmp121)!= 1)goto _LLCB;
_tmp122=_tmp113.f2;if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 1)goto _LLCB;_LLCA:
goto _LLBA;_LLCB: _tmp123=_tmp113.f1;if(_tmp123 <= (void*)3?1:*((int*)_tmp123)!= 1)
goto _LLCD;_LLCC: return 0;_LLCD:;_LLCE: goto _LLBA;_LLBA:;}{struct _tuple0 _tmp125=({
struct _tuple0 _tmp124;_tmp124.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp124.f2=
Cyc_CfFlowInfo_initlevel_approx(r2);_tmp124;});void*_tmp126;void*_tmp127;void*
_tmp128;void*_tmp129;void*_tmp12A;void*_tmp12B;_LLD0: _tmp126=_tmp125.f1;if((int)
_tmp126 != 2)goto _LLD2;_tmp127=_tmp125.f2;if((int)_tmp127 != 2)goto _LLD2;_LLD1:
return 1;_LLD2: _tmp128=_tmp125.f2;if((int)_tmp128 != 0)goto _LLD4;_LLD3: return 1;
_LLD4: _tmp129=_tmp125.f1;if((int)_tmp129 != 0)goto _LLD6;_LLD5: return 0;_LLD6:
_tmp12A=_tmp125.f2;if((int)_tmp12A != 1)goto _LLD8;_LLD7: return 1;_LLD8: _tmp12B=
_tmp125.f1;if((int)_tmp12B != 1)goto _LLCF;_LLD9: return 0;_LLCF:;}}int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp12C=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp12D=r2s;for(0;_tmp12D != 0;_tmp12D=
_tmp12D->tl){struct Cyc_CfFlowInfo_Reln*_tmp12E=(struct Cyc_CfFlowInfo_Reln*)
_tmp12D->hd;if(_tmp12C == _tmp12E?1:(_tmp12C->vd == _tmp12E->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmp12C->rop,(void*)_tmp12E->rop): 0)){found=1;break;}}}if(!found)return 0;}
return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2)
return 1;{struct _tuple0 _tmp130=({struct _tuple0 _tmp12F;_tmp12F.f1=f1;_tmp12F.f2=f2;
_tmp12F;});void*_tmp131;void*_tmp132;void*_tmp133;struct Cyc_Dict_Dict*_tmp134;
struct Cyc_List_List*_tmp135;void*_tmp136;struct Cyc_Dict_Dict*_tmp137;struct Cyc_List_List*
_tmp138;_LLDB: _tmp131=_tmp130.f1;if((int)_tmp131 != 0)goto _LLDD;_LLDC: return 1;
_LLDD: _tmp132=_tmp130.f2;if((int)_tmp132 != 0)goto _LLDF;_LLDE: return 0;_LLDF:
_tmp133=_tmp130.f1;if(_tmp133 <= (void*)1?1:*((int*)_tmp133)!= 0)goto _LLDA;
_tmp134=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f1;_tmp135=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f2;_tmp136=_tmp130.f2;if(_tmp136 <= (
void*)1?1:*((int*)_tmp136)!= 0)goto _LLDA;_tmp137=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp136)->f1;_tmp138=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp136)->f2;
_LLE0: if(_tmp134 == _tmp137?_tmp135 == _tmp138: 0)return 1;return Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp134,_tmp137)?Cyc_CfFlowInfo_relns_approx(
_tmp135,_tmp138): 0;_LLDA:;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;!found?p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp139=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp139->vd == v){found=1;break;}{void*_tmp13A=(void*)_tmp139->rop;
struct Cyc_Absyn_Vardecl*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13C;struct Cyc_Absyn_Vardecl*
_tmp13D;_LLE2: if(*((int*)_tmp13A)!= 1)goto _LLE4;_tmp13B=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp13A)->f1;_LLE3: _tmp13C=_tmp13B;goto _LLE5;_LLE4: if(*((int*)_tmp13A)!= 2)goto
_LLE6;_tmp13C=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp13A)->f1;_LLE5: _tmp13D=
_tmp13C;goto _LLE7;_LLE6: if(*((int*)_tmp13A)!= 4)goto _LLE8;_tmp13D=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp13A)->f1;_LLE7: found=1;continue;_LLE8:;_LLE9: goto _LLE1;_LLE1:;}}if(!found)
return rs;{struct Cyc_List_List*_tmp13E=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp13F=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp13F->vd != v){{void*_tmp140=(
void*)_tmp13F->rop;struct Cyc_Absyn_Vardecl*_tmp141;struct Cyc_Absyn_Vardecl*
_tmp142;struct Cyc_Absyn_Vardecl*_tmp143;_LLEB: if(*((int*)_tmp140)!= 1)goto _LLED;
_tmp141=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp140)->f1;_LLEC: _tmp142=
_tmp141;goto _LLEE;_LLED: if(*((int*)_tmp140)!= 2)goto _LLEF;_tmp142=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp140)->f1;_LLEE: _tmp143=_tmp142;goto _LLF0;_LLEF: if(*((int*)_tmp140)!= 4)goto
_LLF1;_tmp143=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp140)->f1;_LLF0: if(v
== _tmp143)continue;else{goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;_LLEA:;}_tmp13E=({
struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=_tmp13F;
_tmp144->tl=_tmp13E;_tmp144;});}}return _tmp13E;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp145=(void*)e->r;void*
_tmp146;struct Cyc_Absyn_Vardecl*_tmp147;void*_tmp148;struct Cyc_Absyn_Vardecl*
_tmp149;void*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp14B;void*_tmp14C;struct Cyc_Absyn_Vardecl*
_tmp14D;_LLF4: if(*((int*)_tmp145)!= 1)goto _LLF6;_tmp146=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp145)->f2;if(_tmp146 <= (void*)1?1:*((int*)_tmp146)!= 0)goto _LLF6;_tmp147=((
struct Cyc_Absyn_Global_b_struct*)_tmp146)->f1;_LLF5: _tmp149=_tmp147;goto _LLF7;
_LLF6: if(*((int*)_tmp145)!= 1)goto _LLF8;_tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp145)->f2;if(_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 2)goto _LLF8;_tmp149=((
struct Cyc_Absyn_Param_b_struct*)_tmp148)->f1;_LLF7: _tmp14B=_tmp149;goto _LLF9;
_LLF8: if(*((int*)_tmp145)!= 1)goto _LLFA;_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp145)->f2;if(_tmp14A <= (void*)1?1:*((int*)_tmp14A)!= 3)goto _LLFA;_tmp14B=((
struct Cyc_Absyn_Local_b_struct*)_tmp14A)->f1;_LLF9: _tmp14D=_tmp14B;goto _LLFB;
_LLFA: if(*((int*)_tmp145)!= 1)goto _LLFC;_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp145)->f2;if(_tmp14C <= (void*)1?1:*((int*)_tmp14C)!= 4)goto _LLFC;_tmp14D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp14C)->f1;_LLFB: if(!_tmp14D->escapes)return Cyc_CfFlowInfo_reln_kill_var(
r,_tmp14D);goto _LLF3;_LLFC:;_LLFD: goto _LLF3;_LLF3:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{
void*_tmp14E=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp14F;struct Cyc_Absyn_Exp*
_tmp150;int _tmp151;_LLFF: if(*((int*)_tmp14E)!= 33)goto _LL101;_tmp14F=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp14E)->f1;_tmp150=_tmp14F.num_elts;_tmp151=_tmp14F.fat_result;if(_tmp151 != 1)
goto _LL101;_LL100: malloc_loop: {void*_tmp152=(void*)_tmp150->r;struct Cyc_Absyn_Exp*
_tmp153;void*_tmp154;struct Cyc_Absyn_Vardecl*_tmp155;void*_tmp156;struct Cyc_Absyn_Vardecl*
_tmp157;void*_tmp158;struct Cyc_Absyn_Vardecl*_tmp159;void*_tmp15A;struct Cyc_Absyn_Vardecl*
_tmp15B;_LL104: if(*((int*)_tmp152)!= 13)goto _LL106;_tmp153=((struct Cyc_Absyn_Cast_e_struct*)
_tmp152)->f2;_LL105: _tmp150=_tmp153;goto malloc_loop;_LL106: if(*((int*)_tmp152)!= 
1)goto _LL108;_tmp154=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp152)->f2;if(
_tmp154 <= (void*)1?1:*((int*)_tmp154)!= 4)goto _LL108;_tmp155=((struct Cyc_Absyn_Pat_b_struct*)
_tmp154)->f1;_LL107: _tmp157=_tmp155;goto _LL109;_LL108: if(*((int*)_tmp152)!= 1)
goto _LL10A;_tmp156=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp152)->f2;if(
_tmp156 <= (void*)1?1:*((int*)_tmp156)!= 3)goto _LL10A;_tmp157=((struct Cyc_Absyn_Local_b_struct*)
_tmp156)->f1;_LL109: _tmp159=_tmp157;goto _LL10B;_LL10A: if(*((int*)_tmp152)!= 1)
goto _LL10C;_tmp158=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp152)->f2;if(
_tmp158 <= (void*)1?1:*((int*)_tmp158)!= 2)goto _LL10C;_tmp159=((struct Cyc_Absyn_Param_b_struct*)
_tmp158)->f1;_LL10B: _tmp15B=_tmp159;goto _LL10D;_LL10C: if(*((int*)_tmp152)!= 1)
goto _LL10E;_tmp15A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp152)->f2;if(
_tmp15A <= (void*)1?1:*((int*)_tmp15A)!= 0)goto _LL10E;_tmp15B=((struct Cyc_Absyn_Global_b_struct*)
_tmp15A)->f1;_LL10D: if(_tmp15B->escapes)return r;return({struct Cyc_List_List*
_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->vd=_tmp15B;_tmp15D->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));
_tmp15E[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp15F;_tmp15F.tag=4;_tmp15F.f1=
v;_tmp15F;});_tmp15E;}));_tmp15D;});_tmp15C->tl=r;_tmp15C;});_LL10E:;_LL10F:
return r;_LL103:;}_LL101:;_LL102: goto _LLFE;_LLFE:;}{void*_tmp160=Cyc_Tcutil_compress((
void*)v->type);_LL111: if(_tmp160 <= (void*)3?1:*((int*)_tmp160)!= 5)goto _LL113;
_LL112: goto _LL110;_LL113:;_LL114: return r;_LL110:;}loop: {void*_tmp161=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp162;void*_tmp163;int _tmp164;void*_tmp165;struct Cyc_List_List*
_tmp166;struct Cyc_List_List _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_List_List
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;void*_tmp16B;struct Cyc_List_List*_tmp16C;
struct Cyc_List_List _tmp16D;struct Cyc_Absyn_Exp*_tmp16E;_LL116: if(*((int*)_tmp161)
!= 13)goto _LL118;_tmp162=((struct Cyc_Absyn_Cast_e_struct*)_tmp161)->f2;_LL117: e=
_tmp162;goto loop;_LL118: if(*((int*)_tmp161)!= 0)goto _LL11A;_tmp163=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp161)->f1;if(_tmp163 <= (void*)1?1:*((int*)
_tmp163)!= 2)goto _LL11A;_tmp164=((struct Cyc_Absyn_Int_c_struct*)_tmp163)->f2;
_LL119: return({struct Cyc_List_List*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->vd=v;
_tmp170->rop=(void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp171=
_cycalloc_atomic(sizeof(*_tmp171));_tmp171[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp172;_tmp172.tag=0;_tmp172.f1=(unsigned int)_tmp164;_tmp172;});_tmp171;}));
_tmp170;});_tmp16F->tl=r;_tmp16F;});_LL11A: if(*((int*)_tmp161)!= 3)goto _LL11C;
_tmp165=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp161)->f1;if((int)_tmp165 != 
4)goto _LL11C;_tmp166=((struct Cyc_Absyn_Primop_e_struct*)_tmp161)->f2;if(_tmp166
== 0)goto _LL11C;_tmp167=*_tmp166;_tmp168=_tmp167.tl;if(_tmp168 == 0)goto _LL11C;
_tmp169=*_tmp168;_tmp16A=(struct Cyc_Absyn_Exp*)_tmp169.hd;_LL11B:{void*_tmp173=(
void*)_tmp16A->r;void*_tmp174;struct Cyc_List_List*_tmp175;struct Cyc_List_List
_tmp176;struct Cyc_Absyn_Exp*_tmp177;_LL121: if(*((int*)_tmp173)!= 3)goto _LL123;
_tmp174=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp173)->f1;if((int)_tmp174 != 
19)goto _LL123;_tmp175=((struct Cyc_Absyn_Primop_e_struct*)_tmp173)->f2;if(_tmp175
== 0)goto _LL123;_tmp176=*_tmp175;_tmp177=(struct Cyc_Absyn_Exp*)_tmp176.hd;_LL122:{
void*_tmp178=(void*)_tmp177->r;void*_tmp179;struct Cyc_Absyn_Vardecl*_tmp17A;void*
_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17C;void*_tmp17D;struct Cyc_Absyn_Vardecl*
_tmp17E;void*_tmp17F;struct Cyc_Absyn_Vardecl*_tmp180;_LL126: if(*((int*)_tmp178)
!= 1)goto _LL128;_tmp179=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp178)->f2;if(
_tmp179 <= (void*)1?1:*((int*)_tmp179)!= 0)goto _LL128;_tmp17A=((struct Cyc_Absyn_Global_b_struct*)
_tmp179)->f1;_LL127: _tmp17C=_tmp17A;goto _LL129;_LL128: if(*((int*)_tmp178)!= 1)
goto _LL12A;_tmp17B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp178)->f2;if(
_tmp17B <= (void*)1?1:*((int*)_tmp17B)!= 3)goto _LL12A;_tmp17C=((struct Cyc_Absyn_Local_b_struct*)
_tmp17B)->f1;_LL129: _tmp17E=_tmp17C;goto _LL12B;_LL12A: if(*((int*)_tmp178)!= 1)
goto _LL12C;_tmp17D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp178)->f2;if(
_tmp17D <= (void*)1?1:*((int*)_tmp17D)!= 2)goto _LL12C;_tmp17E=((struct Cyc_Absyn_Param_b_struct*)
_tmp17D)->f1;_LL12B: _tmp180=_tmp17E;goto _LL12D;_LL12C: if(*((int*)_tmp178)!= 1)
goto _LL12E;_tmp17F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp178)->f2;if(
_tmp17F <= (void*)1?1:*((int*)_tmp17F)!= 4)goto _LL12E;_tmp180=((struct Cyc_Absyn_Pat_b_struct*)
_tmp17F)->f1;_LL12D: if(_tmp180->escapes)return r;return({struct Cyc_List_List*
_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->vd=v;_tmp182->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessSize_struct*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[
0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp184;_tmp184.tag=2;_tmp184.f1=
_tmp180;_tmp184;});_tmp183;}));_tmp182;});_tmp181->tl=r;_tmp181;});_LL12E:;
_LL12F: goto _LL125;_LL125:;}goto _LL120;_LL123:;_LL124: goto _LL120;_LL120:;}goto
_LL115;_LL11C: if(*((int*)_tmp161)!= 3)goto _LL11E;_tmp16B=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp161)->f1;if((int)_tmp16B != 19)goto _LL11E;_tmp16C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp161)->f2;if(_tmp16C == 0)goto _LL11E;_tmp16D=*_tmp16C;_tmp16E=(struct Cyc_Absyn_Exp*)
_tmp16D.hd;_LL11D:{void*_tmp185=(void*)_tmp16E->r;void*_tmp186;struct Cyc_Absyn_Vardecl*
_tmp187;void*_tmp188;struct Cyc_Absyn_Vardecl*_tmp189;void*_tmp18A;struct Cyc_Absyn_Vardecl*
_tmp18B;void*_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18D;_LL131: if(*((int*)_tmp185)
!= 1)goto _LL133;_tmp186=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp185)->f2;if(
_tmp186 <= (void*)1?1:*((int*)_tmp186)!= 0)goto _LL133;_tmp187=((struct Cyc_Absyn_Global_b_struct*)
_tmp186)->f1;_LL132: _tmp189=_tmp187;goto _LL134;_LL133: if(*((int*)_tmp185)!= 1)
goto _LL135;_tmp188=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp185)->f2;if(
_tmp188 <= (void*)1?1:*((int*)_tmp188)!= 3)goto _LL135;_tmp189=((struct Cyc_Absyn_Local_b_struct*)
_tmp188)->f1;_LL134: _tmp18B=_tmp189;goto _LL136;_LL135: if(*((int*)_tmp185)!= 1)
goto _LL137;_tmp18A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp185)->f2;if(
_tmp18A <= (void*)1?1:*((int*)_tmp18A)!= 2)goto _LL137;_tmp18B=((struct Cyc_Absyn_Param_b_struct*)
_tmp18A)->f1;_LL136: _tmp18D=_tmp18B;goto _LL138;_LL137: if(*((int*)_tmp185)!= 1)
goto _LL139;_tmp18C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp185)->f2;if(
_tmp18C <= (void*)1?1:*((int*)_tmp18C)!= 4)goto _LL139;_tmp18D=((struct Cyc_Absyn_Pat_b_struct*)
_tmp18C)->f1;_LL138: if(_tmp18D->escapes)return r;return({struct Cyc_List_List*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->vd=v;_tmp18F->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp190=_cycalloc(sizeof(*_tmp190));
_tmp190[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp191;_tmp191.tag=4;_tmp191.f1=
_tmp18D;_tmp191;});_tmp190;}));_tmp18F;});_tmp18E->tl=r;_tmp18E;});_LL139:;
_LL13A: goto _LL130;_LL130:;}goto _LL115;_LL11E:;_LL11F: goto _LL115;_LL115:;}return r;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*r,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp192=(void*)e1->r;void*_tmp193;
struct Cyc_Absyn_Vardecl*_tmp194;void*_tmp195;struct Cyc_Absyn_Vardecl*_tmp196;
void*_tmp197;struct Cyc_Absyn_Vardecl*_tmp198;void*_tmp199;struct Cyc_Absyn_Vardecl*
_tmp19A;_LL13C: if(*((int*)_tmp192)!= 1)goto _LL13E;_tmp193=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp192)->f2;if(_tmp193 <= (void*)1?1:*((int*)_tmp193)!= 0)goto _LL13E;_tmp194=((
struct Cyc_Absyn_Global_b_struct*)_tmp193)->f1;_LL13D: _tmp196=_tmp194;goto _LL13F;
_LL13E: if(*((int*)_tmp192)!= 1)goto _LL140;_tmp195=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp192)->f2;if(_tmp195 <= (void*)1?1:*((int*)_tmp195)!= 2)goto _LL140;_tmp196=((
struct Cyc_Absyn_Param_b_struct*)_tmp195)->f1;_LL13F: _tmp198=_tmp196;goto _LL141;
_LL140: if(*((int*)_tmp192)!= 1)goto _LL142;_tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp192)->f2;if(_tmp197 <= (void*)1?1:*((int*)_tmp197)!= 3)goto _LL142;_tmp198=((
struct Cyc_Absyn_Local_b_struct*)_tmp197)->f1;_LL141: _tmp19A=_tmp198;goto _LL143;
_LL142: if(*((int*)_tmp192)!= 1)goto _LL144;_tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp192)->f2;if(_tmp199 <= (void*)1?1:*((int*)_tmp199)!= 4)goto _LL144;_tmp19A=((
struct Cyc_Absyn_Pat_b_struct*)_tmp199)->f1;_LL143: if(!_tmp19A->escapes)return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp19A,e2);goto _LL13B;_LL144:;_LL145: goto _LL13B;_LL13B:;}return r;}void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_Std_String_pa_struct _tmp19C;_tmp19C.tag=
0;_tmp19C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((r->vd)->name);{void*
_tmp19B[1]={& _tmp19C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(char),
3),_tag_arr(_tmp19B,sizeof(void*),1));}});{void*_tmp19D=(void*)r->rop;
unsigned int _tmp19E;struct Cyc_Absyn_Vardecl*_tmp19F;struct Cyc_Absyn_Vardecl*
_tmp1A0;unsigned int _tmp1A1;struct Cyc_Absyn_Vardecl*_tmp1A2;_LL147: if(*((int*)
_tmp19D)!= 0)goto _LL149;_tmp19E=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp19D)->f1;_LL148:({struct Cyc_Std_Int_pa_struct _tmp1A4;_tmp1A4.tag=1;_tmp1A4.f1=(
int)_tmp19E;{void*_tmp1A3[1]={& _tmp1A4};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("==%d",
sizeof(char),5),_tag_arr(_tmp1A3,sizeof(void*),1));}});goto _LL146;_LL149: if(*((
int*)_tmp19D)!= 1)goto _LL14B;_tmp19F=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp19D)->f1;_LL14A:({struct Cyc_Std_String_pa_struct _tmp1A6;_tmp1A6.tag=0;
_tmp1A6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp19F->name);{void*
_tmp1A5[1]={& _tmp1A6};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s",sizeof(char),
4),_tag_arr(_tmp1A5,sizeof(void*),1));}});goto _LL146;_LL14B: if(*((int*)_tmp19D)
!= 2)goto _LL14D;_tmp1A0=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp19D)->f1;
_LL14C:({struct Cyc_Std_String_pa_struct _tmp1A8;_tmp1A8.tag=0;_tmp1A8.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1A0->name);{void*_tmp1A7[1]={& _tmp1A8};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s.size",sizeof(char),9),_tag_arr(
_tmp1A7,sizeof(void*),1));}});goto _LL146;_LL14D: if(*((int*)_tmp19D)!= 3)goto
_LL14F;_tmp1A1=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp19D)->f1;_LL14E:({
struct Cyc_Std_Int_pa_struct _tmp1AA;_tmp1AA.tag=1;_tmp1AA.f1=(int)_tmp1A1;{void*
_tmp1A9[1]={& _tmp1AA};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%d",sizeof(char),
4),_tag_arr(_tmp1A9,sizeof(void*),1));}});goto _LL146;_LL14F: if(*((int*)_tmp19D)
!= 4)goto _LL146;_tmp1A2=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp19D)->f1;
_LL150:({struct Cyc_Std_String_pa_struct _tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1A2->name);{void*_tmp1AB[1]={& _tmp1AC};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<=%s.size",sizeof(char),10),_tag_arr(
_tmp1AB,sizeof(void*),1));}});goto _LL146;_LL146:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp1AD[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr(",",
sizeof(char),2),_tag_arr(_tmp1AD,sizeof(void*),0));});}}static int Cyc_CfFlowInfo_contains_region(
struct Cyc_Absyn_Tvar*rgn,void*t){void*_tmp1AE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp1AF;struct Cyc_Absyn_TunionInfo _tmp1B0;struct Cyc_List_List*_tmp1B1;void*
_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_AggrInfo _tmp1B4;struct Cyc_List_List*
_tmp1B5;struct Cyc_Absyn_TunionFieldInfo _tmp1B6;struct Cyc_List_List*_tmp1B7;
struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_PtrInfo _tmp1B9;void*_tmp1BA;void*
_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BD;void*_tmp1BE;
void*_tmp1BF;void*_tmp1C0;void*_tmp1C1;_LL152: if((int)_tmp1AE != 0)goto _LL154;
_LL153: goto _LL155;_LL154: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 5)goto _LL156;
_LL155: goto _LL157;_LL156: if((int)_tmp1AE != 1)goto _LL158;_LL157: goto _LL159;_LL158:
if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 6)goto _LL15A;_LL159: goto _LL15B;_LL15A:
if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 12)goto _LL15C;_LL15B: goto _LL15D;
_LL15C: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 13)goto _LL15E;_LL15D: goto
_LL15F;_LL15E: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 14)goto _LL160;_LL15F:
goto _LL161;_LL160: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 17)goto _LL162;
_LL161: goto _LL163;_LL162: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 18)goto
_LL164;_LL163: goto _LL165;_LL164: if((int)_tmp1AE != 2)goto _LL166;_LL165: goto _LL167;
_LL166: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 0)goto _LL168;_LL167: return 0;
_LL168: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 1)goto _LL16A;_tmp1AF=((struct
Cyc_Absyn_VarType_struct*)_tmp1AE)->f1;_LL169: return Cyc_Absyn_tvar_cmp(_tmp1AF,
rgn)== 0;_LL16A: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 2)goto _LL16C;_tmp1B0=((
struct Cyc_Absyn_TunionType_struct*)_tmp1AE)->f1;_tmp1B1=_tmp1B0.targs;_tmp1B2=(
void*)_tmp1B0.rgn;_LL16B: if(Cyc_CfFlowInfo_contains_region(rgn,_tmp1B2))return 1;
_tmp1B3=_tmp1B1;goto _LL16D;_LL16C: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 16)
goto _LL16E;_tmp1B3=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AE)->f2;_LL16D:
_tmp1B5=_tmp1B3;goto _LL16F;_LL16E: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 10)
goto _LL170;_tmp1B4=((struct Cyc_Absyn_AggrType_struct*)_tmp1AE)->f1;_tmp1B5=
_tmp1B4.targs;_LL16F: _tmp1B7=_tmp1B5;goto _LL171;_LL170: if(_tmp1AE <= (void*)3?1:*((
int*)_tmp1AE)!= 3)goto _LL172;_tmp1B6=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1AE)->f1;_tmp1B7=_tmp1B6.targs;_LL171: _tmp1B8=_tmp1B7;goto _LL173;_LL172: if(
_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 20)goto _LL174;_tmp1B8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1AE)->f1;_LL173: return((int(*)(int(*pred)(struct Cyc_Absyn_Tvar*,void*),
struct Cyc_Absyn_Tvar*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp1B8);_LL174: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 4)goto _LL176;
_tmp1B9=((struct Cyc_Absyn_PointerType_struct*)_tmp1AE)->f1;_tmp1BA=(void*)
_tmp1B9.elt_typ;_tmp1BB=(void*)_tmp1B9.rgn_typ;_LL175: return Cyc_CfFlowInfo_contains_region(
rgn,_tmp1BB)?1: Cyc_CfFlowInfo_contains_region(rgn,_tmp1BA);_LL176: if(_tmp1AE <= (
void*)3?1:*((int*)_tmp1AE)!= 8)goto _LL178;_LL177: return 0;_LL178: if(_tmp1AE <= (
void*)3?1:*((int*)_tmp1AE)!= 9)goto _LL17A;_tmp1BC=((struct Cyc_Absyn_TupleType_struct*)
_tmp1AE)->f1;_LL179: for(0;_tmp1BC != 0;_tmp1BC=_tmp1BC->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple4*)_tmp1BC->hd)).f2))return 1;}return 0;_LL17A: if(_tmp1AE <= (
void*)3?1:*((int*)_tmp1AE)!= 11)goto _LL17C;_tmp1BD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AE)->f2;_LL17B: for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp1BD->hd)->type))return 1;}return 0;
_LL17C: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 7)goto _LL17E;_tmp1BE=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp1AE)->f1;_LL17D: _tmp1BF=_tmp1BE;goto _LL17F;
_LL17E: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 19)goto _LL180;_tmp1BF=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1AE)->f1;_LL17F: _tmp1C0=_tmp1BF;goto _LL181;
_LL180: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 21)goto _LL182;_tmp1C0=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1AE)->f1;_LL181: return Cyc_CfFlowInfo_contains_region(
rgn,_tmp1C0);_LL182: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 15)goto _LL151;
_tmp1C1=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp1AE)->f1;_LL183:
return 0;_LL151:;}struct _tuple6{struct Cyc_Dict_Dict*f1;struct Cyc_Absyn_Tvar*f2;};
static void Cyc_CfFlowInfo_kill_root(struct _tuple6*env,void*root,void*rval){struct
_tuple6 _tmp1C3;struct Cyc_Dict_Dict*_tmp1C4;struct Cyc_Dict_Dict**_tmp1C5;struct
Cyc_Absyn_Tvar*_tmp1C6;struct _tuple6*_tmp1C2=env;_tmp1C3=*_tmp1C2;_tmp1C4=
_tmp1C3.f1;_tmp1C5=(struct Cyc_Dict_Dict**)&(*_tmp1C2).f1;_tmp1C6=_tmp1C3.f2;{
void*_tmp1C7=root;struct Cyc_Absyn_Vardecl*_tmp1C8;void*_tmp1C9;_LL185: if(*((int*)
_tmp1C7)!= 0)goto _LL187;_tmp1C8=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp1C7)->f1;
_LL186: if(Cyc_CfFlowInfo_contains_region(_tmp1C6,(void*)_tmp1C8->type))rval=Cyc_CfFlowInfo_typ_to_absrval((
void*)_tmp1C8->type,Cyc_CfFlowInfo_unknown_none);*_tmp1C5=Cyc_Dict_insert(*
_tmp1C5,root,rval);goto _LL184;_LL187: if(*((int*)_tmp1C7)!= 1)goto _LL184;_tmp1C9=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp1C7)->f2;_LL188: if(!Cyc_CfFlowInfo_contains_region(
_tmp1C6,_tmp1C9))*_tmp1C5=Cyc_Dict_insert(*_tmp1C5,root,rval);goto _LL184;_LL184:;}}
static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_Dict_Dict*
fd,void*rgn){struct Cyc_Absyn_Tvar*rgn_tvar;{void*_tmp1CA=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp1CB;_LL18A: if(_tmp1CA <= (void*)3?1:*((int*)_tmp1CA)!= 1)
goto _LL18C;_tmp1CB=((struct Cyc_Absyn_VarType_struct*)_tmp1CA)->f1;_LL18B:
rgn_tvar=_tmp1CB;goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Core_Impossible_struct
_tmp1CD;_tmp1CD.tag=Cyc_Core_Impossible;_tmp1CD.f1=_tag_arr("kill_flowdict_region",
sizeof(char),21);_tmp1CD;});_tmp1CC;}));goto _LL189;_LL189:;}{struct _tuple6 env=({
struct _tuple6 _tmp1CE;_tmp1CE.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp1CE.f2=
rgn_tvar;_tmp1CE;});((void(*)(void(*f)(struct _tuple6*,void*,void*),struct _tuple6*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f1;}}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct
Cyc_List_List*relns,void*rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*
f,void*rgn){void*_tmp1CF=f;struct Cyc_Dict_Dict*_tmp1D0;struct Cyc_List_List*
_tmp1D1;_LL18F: if((int)_tmp1CF != 0)goto _LL191;_LL190: return f;_LL191: if(_tmp1CF <= (
void*)1?1:*((int*)_tmp1CF)!= 0)goto _LL18E;_tmp1D0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1CF)->f1;_tmp1D1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1CF)->f2;
_LL192: {struct Cyc_Dict_Dict*_tmp1D2=Cyc_CfFlowInfo_kill_flowdict_region(_tmp1D0,
rgn);struct Cyc_List_List*_tmp1D3=Cyc_CfFlowInfo_kill_relns_region(_tmp1D1,rgn);
return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1D4=_cycalloc(sizeof(*
_tmp1D4));_tmp1D4[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1D5;_tmp1D5.tag=
0;_tmp1D5.f1=_tmp1D2;_tmp1D5.f2=_tmp1D3;_tmp1D5;});_tmp1D4;});}_LL18E:;}
