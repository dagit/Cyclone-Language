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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_memq(struct Cyc_List_List*
l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2);extern char Cyc_Set_Absent[11];struct Cyc_Std___cycFILE;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char
Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);struct _tagged_arr*Cyc_Absyn_fieldname(int);int Cyc_Absyn_is_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);
int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{unsigned int f1;int f2;};struct
_tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct
Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,void*r);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);void*Cyc_CfFlowInfo_join_flow(struct Cyc_Set_Set**,void*,void*);
void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,void*,void*,struct Cyc_Set_Set*,
struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn);void
Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*
encloser;int visited;void*flow;};static char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){(void*)(enclosee->annot=(void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct
_tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot
_tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2.flow=(void*)((void*)0);_tmp2;});
_tmp1;});_tmp0;})));}static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;
_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=_tag_arr("ControlFlow -- wrong stmt annotation",
sizeof(char),37);_tmp7;});_tmp6;}));_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int
iterate_again;int iteration_num;int in_try;void*tryflow;struct Cyc_Set_Set**
all_changed;int noreturn;struct Cyc_List_List*param_roots;};static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Decl*);static struct
_tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp8=Cyc_NewControlFlow_get_stmt_annot(
s);(void*)(_tmp8->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(
void*)_tmp8->flow));_tmp8->visited=env->iteration_num;return _tmp8;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,void*new_flow){if(env->in_try)(void*)(env->tryflow=(
void*)Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,(void*)env->tryflow));}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(
s);void*_tmpA=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,(void*)_tmp9->flow);
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmpA,(void*)_tmp9->flow)){(void*)(_tmp9->flow=(
void*)_tmpA);if(_tmp9->visited == env->iteration_num)env->iterate_again=1;}}
static void*Cyc_NewControlFlow_add_vars(void*inflow,struct Cyc_List_List*vds,void*
leafval){void*_tmpB=inflow;struct Cyc_Dict_Dict*_tmpC;struct Cyc_List_List*_tmpD;
_LL6: if((int)_tmpB != 0)goto _LL8;_LL7: return(void*)0;_LL8: if(_tmpB <= (void*)1?1:*((
int*)_tmpB)!= 0)goto _LL5;_tmpC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB)->f1;
_tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB)->f2;_LL9: for(0;vds != 0;
vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmpE=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp10;_tmp10.tag=0;_tmp10.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp10;});_tmpF;});
_tmpC=Cyc_Dict_insert(_tmpC,(void*)_tmpE,Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=_tmpC;_tmp12.f2=_tmpD;_tmp12;});_tmp11;});_LL5:;}
static void*Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Position_Segment*loc,void*inflow,void*r){void*_tmp13=inflow;struct Cyc_Dict_Dict*
_tmp14;struct Cyc_List_List*_tmp15;_LLB: if((int)_tmp13 != 0)goto _LLD;_LLC: return(
void*)0;_LLD: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LLA;_tmp14=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13)->f1;_tmp15=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13)->f2;_LLE: if(Cyc_CfFlowInfo_initlevel(_tmp14,r)!= (void*)2)({void*_tmp16[
0]={};Cyc_Tcutil_terr(loc,_tag_arr("expression may not be fully initialized",
sizeof(char),40),_tag_arr(_tmp16,sizeof(void*),0));});{struct Cyc_Dict_Dict*
_tmp17=Cyc_CfFlowInfo_escape_deref(_tmp14,env->all_changed,r);if(_tmp14 == _tmp17)
return inflow;{struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp18=({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=_tmp17;_tmp1A.f2=_tmp15;_tmp1A;});_tmp19;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp18);return(void*)_tmp18;}}_LLA:;}struct _tuple5{void*f1;struct Cyc_List_List*
f2;};static struct _tuple5 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0)return({struct _tuple5 _tmp1B;
_tmp1B.f1=inflow;_tmp1B.f2=0;_tmp1B;});if(es->tl == 0){void*_tmp1D;void*_tmp1E;
struct _tuple0 _tmp1C=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;return({struct _tuple5 _tmp1F;_tmp1F.f1=
_tmp1D;_tmp1F.f2=({struct Cyc_List_List*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->hd=(
void*)_tmp1E;_tmp20->tl=0;_tmp20;});_tmp1F;});}{struct Cyc_Set_Set**
outer_all_changed=env->all_changed;struct Cyc_Set_Set*this_all_changed;void*
old_inflow;void*outflow;struct Cyc_List_List*rvals;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();
env->all_changed=({struct Cyc_Set_Set**_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=
Cyc_CfFlowInfo_mt_place_set();_tmp21;});{void*_tmp23;void*_tmp24;struct _tuple0
_tmp22=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)es->hd);
_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;outflow=_tmp23;rvals=({struct Cyc_List_List*
_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=(void*)_tmp24;_tmp25->tl=0;_tmp25;});
this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Set_Set**_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp26;});{void*_tmp28;void*_tmp29;struct _tuple0 _tmp27=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp28=_tmp27.f1;_tmp29=_tmp27.f2;
rvals=({struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=(void*)
_tmp29;_tmp2A->tl=rvals;_tmp2A;});outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)&
this_all_changed,outflow,_tmp28,this_all_changed,*((struct Cyc_Set_Set**)
_check_null(env->all_changed)));this_all_changed=((struct Cyc_Set_Set*(*)(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(this_all_changed,*((
struct Cyc_Set_Set**)_check_null(env->all_changed)));}}}old_inflow=inflow;inflow=
Cyc_CfFlowInfo_join_flow(outer_all_changed,inflow,outflow);}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
inflow,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Set_Set**_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*outer_all_changed,
this_all_changed);_tmp2B;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple5 _tmp2C;_tmp2C.f1=outflow;_tmp2C.f2=Cyc_List_imp_rev(rvals);
_tmp2C;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){void*_tmp2E;struct Cyc_List_List*_tmp2F;
struct _tuple5 _tmp2D=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);
_tmp2E=_tmp2D.f1;_tmp2F=_tmp2D.f2;{void*_tmp30=_tmp2E;struct Cyc_Dict_Dict*_tmp31;
struct Cyc_List_List*_tmp32;_LL10: if(_tmp30 <= (void*)1?1:*((int*)_tmp30)!= 0)goto
_LL12;_tmp31=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp30)->f1;_tmp32=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp30)->f2;_LL11: for(0;_tmp2F != 0;(
_tmp2F=_tmp2F->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp31,(void*)_tmp2F->hd)== (void*)0)({void*_tmp33[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp33,sizeof(void*),0));});}goto _LLF;_LL12: if((int)
_tmp30 != 0)goto _LLF;_LL13: goto _LLF;_LLF:;}return({struct _tuple0 _tmp34;_tmp34.f1=
_tmp2E;_tmp34.f2=Cyc_CfFlowInfo_unknown_all;_tmp34;});}static void*Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*
e,void*il){void*_tmp35=outflow;struct Cyc_Dict_Dict*_tmp36;struct Cyc_List_List*
_tmp37;_LL15: if((int)_tmp35 != 0)goto _LL17;_LL16: return outflow;_LL17: if(_tmp35 <= (
void*)1?1:*((int*)_tmp35)!= 0)goto _LL14;_tmp36=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp35)->f1;_tmp37=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp35)->f2;_LL18: {
void*_tmp38=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp39;_LL1A: if((int)_tmp38 != 0)goto _LL1C;_LL1B: return outflow;_LL1C: if(_tmp38 <= (
void*)1?1:*((int*)_tmp38)!= 0)goto _LL19;_tmp39=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp38)->f1;_LL1D: {void*nzval=il == (void*)2?(void*)1:(void*)2;return(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[
0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp36,env->all_changed,_tmp39,nzval);_tmp3B.f2=_tmp37;_tmp3B;});_tmp3A;});}
_LL19:;}_LL14:;}static struct _tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp3C=outflow;
struct Cyc_Dict_Dict*_tmp3D;struct Cyc_List_List*_tmp3E;_LL1F: if((int)_tmp3C != 0)
goto _LL21;_LL20: return({struct _tuple0 _tmp3F;_tmp3F.f1=outflow;_tmp3F.f2=outflow;
_tmp3F;});_LL21: if(_tmp3C <= (void*)1?1:*((int*)_tmp3C)!= 0)goto _LL1E;_tmp3D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3C)->f1;_tmp3E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3C)->f2;_LL22: {void*_tmp40=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;
struct Cyc_CfFlowInfo_Place*_tmp41;_LL24: if((int)_tmp40 != 0)goto _LL26;_LL25:
return({struct _tuple0 _tmp42;_tmp42.f1=outflow;_tmp42.f2=outflow;_tmp42;});_LL26:
if(_tmp40 <= (void*)1?1:*((int*)_tmp40)!= 0)goto _LL23;_tmp41=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp40)->f1;_LL27: {void*nzval=il == (void*)2?(void*)1:(void*)2;return({struct
_tuple0 _tmp43;_tmp43.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp46=
_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp3D,env->all_changed,
_tmp41,nzval);_tmp47.f2=_tmp3E;_tmp47;});_tmp46;});_tmp43.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp45;_tmp45.tag=0;_tmp45.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp3D,env->all_changed,
_tmp41,(void*)0);_tmp45.f2=_tmp3E;_tmp45;});_tmp44;});_tmp43;});}_LL23:;}_LL1E:;}
static struct _tuple0 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp48=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp49;void*_tmp4A;struct Cyc_Absyn_Conref*_tmp4B;_LL29: if(_tmp48 <= (void*)3?1:*((
int*)_tmp48)!= 4)goto _LL2B;_tmp49=((struct Cyc_Absyn_PointerType_struct*)_tmp48)->f1;
_tmp4A=(void*)_tmp49.elt_typ;_tmp4B=_tmp49.bounds;_LL2A: {void*_tmp4C=f;struct
Cyc_Dict_Dict*_tmp4D;struct Cyc_List_List*_tmp4E;_LL2E: if((int)_tmp4C != 0)goto
_LL30;_LL2F: return({struct _tuple0 _tmp4F;_tmp4F.f1=f;_tmp4F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4A,Cyc_CfFlowInfo_unknown_all);_tmp4F;});_LL30: if(_tmp4C <= (void*)1?1:*((int*)
_tmp4C)!= 0)goto _LL2D;_tmp4D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4C)->f1;
_tmp4E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4C)->f2;_LL31: if(Cyc_Tcutil_is_bound_one(
_tmp4B)){void*_tmp50=r;struct Cyc_CfFlowInfo_Place*_tmp51;void*_tmp52;_LL33: if((
int)_tmp50 != 1)goto _LL35;_LL34: goto _LL36;_LL35: if((int)_tmp50 != 2)goto _LL37;
_LL36:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp54;_tmp54.tag=Cyc_CfFlowInfo_NotZero;_tmp54.f1=_tmp4E;_tmp54;});_tmp53;})));
goto _LL32;_LL37: if(_tmp50 <= (void*)3?1:*((int*)_tmp50)!= 2)goto _LL39;_tmp51=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp50)->f1;_LL38:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp55=_cycalloc(sizeof(*_tmp55));
_tmp55[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp56;_tmp56.tag=Cyc_CfFlowInfo_NotZero;
_tmp56.f1=_tmp4E;_tmp56;});_tmp55;})));return({struct _tuple0 _tmp57;_tmp57.f1=f;
_tmp57.f2=Cyc_CfFlowInfo_lookup_place(_tmp4D,_tmp51);_tmp57;});_LL39: if((int)
_tmp50 != 0)goto _LL3B;_LL3A:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*
_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp59;_tmp59.tag=Cyc_CfFlowInfo_IsZero;_tmp59.f1=_tmp4E;_tmp59;});_tmp58;})));
return({struct _tuple0 _tmp5A;_tmp5A.f1=(void*)0;_tmp5A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4A,Cyc_CfFlowInfo_unknown_all);_tmp5A;});_LL3B: if(_tmp50 <= (void*)3?1:*((int*)
_tmp50)!= 0)goto _LL3D;_tmp52=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp50)->f1;_LL3C: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp52);goto _LL3E;
_LL3D:;_LL3E:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp5C;_tmp5C.tag=Cyc_CfFlowInfo_UnknownZ;_tmp5C.f1=_tmp4E;_tmp5C;});_tmp5B;})));
_LL32:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp5E;_tmp5E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp5E.f1=_tmp4E;_tmp5E;});_tmp5D;})));}{
void*_tmp5F=Cyc_CfFlowInfo_initlevel(_tmp4D,r);_LL40: if((int)_tmp5F != 0)goto
_LL42;_LL41:({void*_tmp60[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(char),46),_tag_arr(_tmp60,sizeof(void*),0));});goto _LL43;_LL42: if((int)
_tmp5F != 2)goto _LL44;_LL43: return({struct _tuple0 _tmp61;_tmp61.f1=f;_tmp61.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4A,Cyc_CfFlowInfo_unknown_all);_tmp61;});_LL44: if((int)_tmp5F != 1)goto _LL3F;
_LL45: return({struct _tuple0 _tmp62;_tmp62.f1=f;_tmp62.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4A,Cyc_CfFlowInfo_unknown_none);_tmp62;});_LL3F:;}_LL2D:;}_LL2B:;_LL2C:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63[0]=({struct Cyc_Core_Impossible_struct _tmp64;_tmp64.tag=Cyc_Core_Impossible;
_tmp64.f1=_tag_arr("right deref of non-pointer-type",sizeof(char),32);_tmp64;});
_tmp63;}));_LL28:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*
_tmp65=(void*)e1->r;void*_tmp66;struct Cyc_Absyn_Vardecl*_tmp67;void*_tmp68;
struct Cyc_Absyn_Vardecl*_tmp69;void*_tmp6A;struct Cyc_Absyn_Vardecl*_tmp6B;void*
_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6D;_LL47: if(*((int*)_tmp65)!= 1)goto _LL49;
_tmp66=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if(_tmp66 <= (void*)1?1:*((
int*)_tmp66)!= 4)goto _LL49;_tmp67=((struct Cyc_Absyn_Pat_b_struct*)_tmp66)->f1;
_LL48: _tmp69=_tmp67;goto _LL4A;_LL49: if(*((int*)_tmp65)!= 1)goto _LL4B;_tmp68=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if(_tmp68 <= (void*)1?1:*((int*)
_tmp68)!= 3)goto _LL4B;_tmp69=((struct Cyc_Absyn_Local_b_struct*)_tmp68)->f1;_LL4A:
_tmp6B=_tmp69;goto _LL4C;_LL4B: if(*((int*)_tmp65)!= 1)goto _LL4D;_tmp6A=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if(_tmp6A <= (void*)1?1:*((int*)_tmp6A)
!= 2)goto _LL4D;_tmp6B=((struct Cyc_Absyn_Param_b_struct*)_tmp6A)->f1;_LL4C: _tmp6D=
_tmp6B;goto _LL4E;_LL4D: if(*((int*)_tmp65)!= 1)goto _LL4F;_tmp6C=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if(_tmp6C <= (void*)1?1:*((int*)_tmp6C)!= 0)
goto _LL4F;_tmp6D=((struct Cyc_Absyn_Global_b_struct*)_tmp6C)->f1;_LL4E: if(!_tmp6D->escapes){
void*_tmp6E=(void*)e2->r;void*_tmp6F;struct Cyc_Absyn_Vardecl*_tmp70;void*_tmp71;
struct Cyc_Absyn_Vardecl*_tmp72;void*_tmp73;struct Cyc_Absyn_Vardecl*_tmp74;void*
_tmp75;struct Cyc_Absyn_Vardecl*_tmp76;_LL52: if(*((int*)_tmp6E)!= 1)goto _LL54;
_tmp6F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if(_tmp6F <= (void*)1?1:*((
int*)_tmp6F)!= 4)goto _LL54;_tmp70=((struct Cyc_Absyn_Pat_b_struct*)_tmp6F)->f1;
_LL53: _tmp72=_tmp70;goto _LL55;_LL54: if(*((int*)_tmp6E)!= 1)goto _LL56;_tmp71=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if(_tmp71 <= (void*)1?1:*((int*)
_tmp71)!= 3)goto _LL56;_tmp72=((struct Cyc_Absyn_Local_b_struct*)_tmp71)->f1;_LL55:
_tmp74=_tmp72;goto _LL57;_LL56: if(*((int*)_tmp6E)!= 1)goto _LL58;_tmp73=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if(_tmp73 <= (void*)1?1:*((int*)_tmp73)
!= 2)goto _LL58;_tmp74=((struct Cyc_Absyn_Param_b_struct*)_tmp73)->f1;_LL57: _tmp76=
_tmp74;goto _LL59;_LL58: if(*((int*)_tmp6E)!= 1)goto _LL5A;_tmp75=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if(_tmp75 <= (void*)1?1:*((int*)_tmp75)!= 0)
goto _LL5A;_tmp76=((struct Cyc_Absyn_Global_b_struct*)_tmp75)->f1;_LL59: if(!_tmp76->escapes){
int found=0;{struct Cyc_List_List*_tmp77=relns;for(0;_tmp77 != 0;_tmp77=_tmp77->tl){
struct Cyc_CfFlowInfo_Reln*_tmp78=(struct Cyc_CfFlowInfo_Reln*)_tmp77->hd;if(
_tmp78->vd == _tmp76){void*_tmp79=(void*)_tmp78->rop;struct Cyc_Absyn_Vardecl*
_tmp7A;_LL5D: if(*((int*)_tmp79)!= 2)goto _LL5F;_tmp7A=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp79)->f1;if(!(_tmp7A == _tmp6D))goto _LL5F;_LL5E: return relns;_LL5F:;_LL60:
continue;_LL5C:;}}}if(!found)return({struct Cyc_List_List*_tmp7B=_cycalloc(
sizeof(*_tmp7B));_tmp7B->hd=({struct Cyc_CfFlowInfo_Reln*_tmp7C=_cycalloc(sizeof(*
_tmp7C));_tmp7C->vd=_tmp76;_tmp7C->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp7E;_tmp7E.tag=2;_tmp7E.f1=_tmp6D;_tmp7E;});_tmp7D;}));_tmp7C;});_tmp7B->tl=
relns;_tmp7B;});}return relns;_LL5A:;_LL5B: return relns;_LL51:;}else{return relns;}
_LL4F:;_LL50: return relns;_LL46:;}struct _tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;{void*_tmp7F=inflow;struct Cyc_Dict_Dict*_tmp80;struct Cyc_List_List*_tmp81;
_LL62: if((int)_tmp7F != 0)goto _LL64;_LL63: return({struct _tuple0 _tmp82;_tmp82.f1=(
void*)0;_tmp82.f2=Cyc_CfFlowInfo_unknown_all;_tmp82;});_LL64: if(_tmp7F <= (void*)
1?1:*((int*)_tmp7F)!= 0)goto _LL61;_tmp80=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp7F)->f1;_tmp81=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp7F)->f2;_LL65:
d=_tmp80;relns=_tmp81;_LL61:;}{void*_tmp83=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp84;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp86;void*_tmp87;void*
_tmp88;void*_tmp89;int _tmp8A;void*_tmp8B;void*_tmp8C;void*_tmp8D;struct Cyc_List_List*
_tmp8E;void*_tmp8F;void*_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;void*_tmp92;struct
Cyc_Absyn_Vardecl*_tmp93;void*_tmp94;struct Cyc_Absyn_Vardecl*_tmp95;struct Cyc_List_List*
_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Core_Opt*
_tmp99;struct Cyc_Core_Opt _tmp9A;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*
_tmp9C;struct Cyc_Core_Opt*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*
_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*
_tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_Absyn_MallocInfo _tmpA4;int _tmpA5;
struct Cyc_Absyn_Exp*_tmpA6;void**_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;int _tmpA9;
struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;
struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;struct _tagged_arr*_tmpAF;
struct Cyc_Absyn_Exp*_tmpB0;struct _tagged_arr*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;
struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_List_List*_tmpB7;struct Cyc_List_List*_tmpB8;
struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpBB;
struct Cyc_Absyn_Vardecl*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*
_tmpBE;struct Cyc_Absyn_Stmt*_tmpBF;void*_tmpC0;_LL67: if(*((int*)_tmp83)!= 13)
goto _LL69;_tmp84=((struct Cyc_Absyn_Cast_e_struct*)_tmp83)->f2;_LL68: _tmp85=
_tmp84;goto _LL6A;_LL69: if(*((int*)_tmp83)!= 11)goto _LL6B;_tmp85=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp83)->f1;_LL6A: _tmp86=_tmp85;goto _LL6C;_LL6B: if(*((int*)_tmp83)!= 12)goto
_LL6D;_tmp86=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp83)->f1;_LL6C: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp86);_LL6D: if(*((int*)_tmp83)!= 0)goto _LL6F;_tmp87=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if((int)_tmp87 != 0)goto _LL6F;_LL6E: goto
_LL70;_LL6F: if(*((int*)_tmp83)!= 0)goto _LL71;_tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if(_tmp88 <= (void*)1?1:*((int*)_tmp88)!= 2)goto _LL71;_tmp89=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp88)->f1;if((int)_tmp89 != 0)goto _LL71;_tmp8A=((
struct Cyc_Absyn_Int_c_struct*)_tmp88)->f2;if(_tmp8A != 0)goto _LL71;_LL70: return({
struct _tuple0 _tmpC1;_tmpC1.f1=inflow;_tmpC1.f2=(void*)0;_tmpC1;});_LL71: if(*((
int*)_tmp83)!= 0)goto _LL73;_tmp8B=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp83)->f1;if(_tmp8B <= (void*)1?1:*((int*)_tmp8B)!= 2)goto _LL73;_tmp8C=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp8B)->f1;if((int)_tmp8C != 0)goto _LL73;_LL72:
goto _LL74;_LL73: if(*((int*)_tmp83)!= 1)goto _LL75;_tmp8D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if(_tmp8D <= (void*)1?1:*((int*)_tmp8D)!= 1)goto _LL75;_LL74: return({
struct _tuple0 _tmpC2;_tmpC2.f1=inflow;_tmpC2.f2=(void*)1;_tmpC2;});_LL75: if(*((
int*)_tmp83)!= 30)goto _LL77;_tmp8E=((struct Cyc_Absyn_Tunion_e_struct*)_tmp83)->f1;
if(_tmp8E != 0)goto _LL77;_LL76: goto _LL78;_LL77: if(*((int*)_tmp83)!= 0)goto _LL79;
_LL78: goto _LL7A;_LL79: if(*((int*)_tmp83)!= 17)goto _LL7B;_LL7A: goto _LL7C;_LL7B:
if(*((int*)_tmp83)!= 16)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(*((int*)_tmp83)!= 18)
goto _LL7F;_LL7E: goto _LL80;_LL7F: if(*((int*)_tmp83)!= 19)goto _LL81;_LL80: goto
_LL82;_LL81: if(*((int*)_tmp83)!= 32)goto _LL83;_LL82: goto _LL84;_LL83: if(*((int*)
_tmp83)!= 31)goto _LL85;_LL84: return({struct _tuple0 _tmpC3;_tmpC3.f1=inflow;_tmpC3.f2=
Cyc_CfFlowInfo_unknown_all;_tmpC3;});_LL85: if(*((int*)_tmp83)!= 1)goto _LL87;
_tmp8F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if(_tmp8F <= (void*)1?1:*((
int*)_tmp8F)!= 0)goto _LL87;_LL86: return({struct _tuple0 _tmpC4;_tmpC4.f1=inflow;
_tmpC4.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmpC4;});_LL87: if(*((int*)_tmp83)!= 1)
goto _LL89;_tmp90=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if(_tmp90 <= (
void*)1?1:*((int*)_tmp90)!= 2)goto _LL89;_tmp91=((struct Cyc_Absyn_Param_b_struct*)
_tmp90)->f1;_LL88: _tmp93=_tmp91;goto _LL8A;_LL89: if(*((int*)_tmp83)!= 1)goto _LL8B;
_tmp92=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if(_tmp92 <= (void*)1?1:*((
int*)_tmp92)!= 3)goto _LL8B;_tmp93=((struct Cyc_Absyn_Local_b_struct*)_tmp92)->f1;
_LL8A: _tmp95=_tmp93;goto _LL8C;_LL8B: if(*((int*)_tmp83)!= 1)goto _LL8D;_tmp94=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if(_tmp94 <= (void*)1?1:*((int*)
_tmp94)!= 4)goto _LL8D;_tmp95=((struct Cyc_Absyn_Pat_b_struct*)_tmp94)->f1;_LL8C:
return({struct _tuple0 _tmpC5;_tmpC5.f1=inflow;_tmpC5.f2=Cyc_Dict_lookup(d,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=_tmp95;_tmpC7;});
_tmpC6;}));_tmpC5;});_LL8D: if(*((int*)_tmp83)!= 3)goto _LL8F;_tmp96=((struct Cyc_Absyn_Primop_e_struct*)
_tmp83)->f2;_LL8E: return Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp96);
_LL8F: if(*((int*)_tmp83)!= 5)goto _LL91;_tmp97=((struct Cyc_Absyn_Increment_e_struct*)
_tmp83)->f1;_LL90: {void*_tmpCA;struct _tuple0 _tmpC9=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpC8[1];_tmpC8[0]=_tmp97;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpC8,sizeof(struct Cyc_Absyn_Exp*),1));}));
_tmpCA=_tmpC9.f1;{void*_tmpCC;struct _tuple0 _tmpCB=Cyc_NewControlFlow_anal_Lexp(
env,_tmpCA,_tmp97);_tmpCC=_tmpCB.f2;{struct _tuple0 _tmpCE=({struct _tuple0 _tmpCD;
_tmpCD.f1=_tmpCC;_tmpCD.f2=_tmpCA;_tmpCD;});void*_tmpCF;struct Cyc_CfFlowInfo_Place*
_tmpD0;void*_tmpD1;struct Cyc_Dict_Dict*_tmpD2;struct Cyc_List_List*_tmpD3;_LLC8:
_tmpCF=_tmpCE.f1;if(_tmpCF <= (void*)1?1:*((int*)_tmpCF)!= 0)goto _LLCA;_tmpD0=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpCF)->f1;_tmpD1=_tmpCE.f2;if(_tmpD1 <= (
void*)1?1:*((int*)_tmpD1)!= 0)goto _LLCA;_tmpD2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpD1)->f1;_tmpD3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpD1)->f2;_LLC9:
_tmpD3=Cyc_CfFlowInfo_reln_kill_exp(_tmpD3,_tmp97);_tmpCA=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpD5;_tmpD5.tag=0;_tmpD5.f1=Cyc_CfFlowInfo_assign_place(_tmp97->loc,_tmpD2,env->all_changed,
_tmpD0,Cyc_CfFlowInfo_unknown_all);_tmpD5.f2=_tmpD3;_tmpD5;});_tmpD4;});goto
_LLC7;_LLCA:;_LLCB: goto _LLC7;_LLC7:;}return({struct _tuple0 _tmpD6;_tmpD6.f1=
_tmpCA;_tmpD6.f2=Cyc_CfFlowInfo_unknown_all;_tmpD6;});}}_LL91: if(*((int*)_tmp83)
!= 4)goto _LL93;_tmp98=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f1;_tmp99=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;if(_tmp99 == 0)goto _LL93;_tmp9A=*
_tmp99;_tmp9B=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f3;_LL92: {void*
_tmpD9;struct _tuple0 _tmpD8=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct
Cyc_Absyn_Exp*_tmpD7[2];_tmpD7[1]=_tmp9B;_tmpD7[0]=_tmp98;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpD7,sizeof(struct Cyc_Absyn_Exp*),2));}));
_tmpD9=_tmpD8.f1;{void*_tmpDB;struct _tuple0 _tmpDA=Cyc_NewControlFlow_anal_Lexp(
env,_tmpD9,e);_tmpDB=_tmpDA.f2;{struct _tuple0 _tmpDD=({struct _tuple0 _tmpDC;_tmpDC.f1=
_tmpDB;_tmpDC.f2=_tmpD9;_tmpDC;});void*_tmpDE;struct Cyc_CfFlowInfo_Place*_tmpDF;
void*_tmpE0;struct Cyc_Dict_Dict*_tmpE1;struct Cyc_List_List*_tmpE2;_LLCD: _tmpDE=
_tmpDD.f1;if(_tmpDE <= (void*)1?1:*((int*)_tmpDE)!= 0)goto _LLCF;_tmpDF=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmpDE)->f1;_tmpE0=_tmpDD.f2;if(_tmpE0 <= (void*)1?
1:*((int*)_tmpE0)!= 0)goto _LLCF;_tmpE1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpE0)->f1;_tmpE2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpE0)->f2;_LLCE:
_tmpE2=Cyc_CfFlowInfo_reln_kill_exp(_tmpE2,_tmp98);_tmpD9=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=Cyc_CfFlowInfo_assign_place(_tmp98->loc,_tmpE1,env->all_changed,
_tmpDF,Cyc_CfFlowInfo_unknown_all);_tmpE4.f2=_tmpE2;_tmpE4;});_tmpE3;});goto
_LLCC;_LLCF:;_LLD0: goto _LLCC;_LLCC:;}return({struct _tuple0 _tmpE5;_tmpE5.f1=
_tmpD9;_tmpE5.f2=Cyc_CfFlowInfo_unknown_all;_tmpE5;});}}_LL93: if(*((int*)_tmp83)
!= 4)goto _LL95;_tmp9C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f1;_tmp9D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;if(_tmp9D != 0)goto _LL95;_tmp9E=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f3;_LL94: {struct Cyc_Set_Set**_tmpE6=
env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmpE7=_cycalloc(
sizeof(*_tmpE7));_tmpE7[0]=Cyc_CfFlowInfo_mt_place_set();_tmpE7;});{void*_tmpE9;
void*_tmpEA;struct _tuple0 _tmpE8=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp9C);
_tmpE9=_tmpE8.f1;_tmpEA=_tmpE8.f2;{struct Cyc_Set_Set*_tmpEB=*((struct Cyc_Set_Set**)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Set_Set**_tmpEC=
_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=Cyc_CfFlowInfo_mt_place_set();_tmpEC;});{
void*_tmpEE;void*_tmpEF;struct _tuple0 _tmpED=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp9E);_tmpEE=_tmpED.f1;_tmpEF=_tmpED.f2;{struct Cyc_Set_Set*_tmpF0=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmpF1=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmpEB,_tmpE9,_tmpEE,_tmpEB,_tmpF0);void*_tmpF2=Cyc_CfFlowInfo_join_flow(
_tmpE6,inflow,_tmpF1);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmpF2,inflow)){if(
_tmpE6 == 0)env->all_changed=0;else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmpE6,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmpEB,_tmpF0));}{void*_tmpF3=_tmpF1;struct Cyc_Dict_Dict*_tmpF4;struct Cyc_List_List*
_tmpF5;_LLD2: if((int)_tmpF3 != 0)goto _LLD4;_LLD3: return({struct _tuple0 _tmpF6;
_tmpF6.f1=(void*)0;_tmpF6.f2=_tmpEF;_tmpF6;});_LLD4: if(_tmpF3 <= (void*)1?1:*((
int*)_tmpF3)!= 0)goto _LLD1;_tmpF4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF3)->f1;_tmpF5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpF3)->f2;_LLD5: {
void*_tmpF7=_tmpEA;struct Cyc_CfFlowInfo_Place*_tmpF8;_LLD7: if(_tmpF7 <= (void*)1?
1:*((int*)_tmpF7)!= 0)goto _LLD9;_tmpF8=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmpF7)->f1;_LLD8: _tmpF4=Cyc_CfFlowInfo_assign_place(e->loc,_tmpF4,env->all_changed,
_tmpF8,_tmpEF);_tmpF5=Cyc_CfFlowInfo_reln_assign_exp(_tmpF5,_tmp9C,_tmp9E);
_tmpF1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpF9=_cycalloc(sizeof(*
_tmpF9));_tmpF9[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmpFA;_tmpFA.tag=0;
_tmpFA.f1=_tmpF4;_tmpFA.f2=_tmpF5;_tmpFA;});_tmpF9;});Cyc_NewControlFlow_update_tryflow(
env,_tmpF1);return({struct _tuple0 _tmpFB;_tmpFB.f1=_tmpF1;_tmpFB.f2=_tmpEF;_tmpFB;});
_LLD9: if((int)_tmpF7 != 0)goto _LLD6;_LLDA: return({struct _tuple0 _tmpFC;_tmpFC.f1=
Cyc_NewControlFlow_use_Rval(env,_tmp9E->loc,_tmpF1,_tmpEF);_tmpFC.f2=_tmpEF;
_tmpFC;});_LLD6:;}_LLD1:;}}inflow=_tmpF2;}}}}}}_LL95: if(*((int*)_tmp83)!= 7)goto
_LL97;_tmp9F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp83)->f1;_tmpA0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp83)->f2;_LL96: return Cyc_NewControlFlow_anal_Rexp(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp9F)).f1,_tmpA0);_LL97: if(*((int*)_tmp83)!= 10)goto _LL99;_tmpA1=((
struct Cyc_Absyn_Throw_e_struct*)_tmp83)->f1;_LL98: {void*_tmpFE;void*_tmpFF;
struct _tuple0 _tmpFD=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA1);_tmpFE=
_tmpFD.f1;_tmpFF=_tmpFD.f2;Cyc_NewControlFlow_use_Rval(env,_tmpA1->loc,_tmpFE,
_tmpFF);return({struct _tuple0 _tmp100;_tmp100.f1=(void*)0;_tmp100.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp100;});}_LL99: if(*((int*)_tmp83)!= 9)goto _LL9B;_tmpA2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp83)->f1;_tmpA3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp83)->f2;_LL9A: {void*
_tmp103;struct Cyc_List_List*_tmp104;struct _tuple5 _tmp102=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->hd=
_tmpA2;_tmp101->tl=_tmpA3;_tmp101;}));_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;{
void*_tmp105=Cyc_NewControlFlow_use_Rval(env,_tmpA2->loc,_tmp103,(void*)((struct
Cyc_List_List*)_check_null(_tmp104))->hd);_tmp104=_tmp104->tl;{struct Cyc_List_List*
init_params=0;{void*_tmp106=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA2->topt))->v);struct Cyc_Absyn_PtrInfo _tmp107;void*_tmp108;_LLDC:
if(_tmp106 <= (void*)3?1:*((int*)_tmp106)!= 4)goto _LLDE;_tmp107=((struct Cyc_Absyn_PointerType_struct*)
_tmp106)->f1;_tmp108=(void*)_tmp107.elt_typ;_LLDD:{void*_tmp109=Cyc_Tcutil_compress(
_tmp108);struct Cyc_Absyn_FnInfo _tmp10A;struct Cyc_List_List*_tmp10B;_LLE1: if(
_tmp109 <= (void*)3?1:*((int*)_tmp109)!= 8)goto _LLE3;_tmp10A=((struct Cyc_Absyn_FnType_struct*)
_tmp109)->f1;_tmp10B=_tmp10A.attributes;_LLE2: for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
void*_tmp10C=(void*)_tmp10B->hd;int _tmp10D;_LLE6: if(_tmp10C <= (void*)16?1:*((int*)
_tmp10C)!= 4)goto _LLE8;_tmp10D=((struct Cyc_Absyn_Initializes_att_struct*)_tmp10C)->f1;
_LLE7: init_params=({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E->hd=(void*)_tmp10D;_tmp10E->tl=init_params;_tmp10E;});goto _LLE5;_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}goto _LLE0;_LLE3:;_LLE4:({void*_tmp10F[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("anal_Rexp: bad function type",
sizeof(char),29),_tag_arr(_tmp10F,sizeof(void*),0));});_LLE0:;}goto _LLDB;_LLDE:;
_LLDF:({void*_tmp110[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("anal_Rexp: bad function type",sizeof(char),29),
_tag_arr(_tmp110,sizeof(void*),0));});_LLDB:;}{int i=1;for(0;_tmp104 != 0;(((
_tmp104=_tmp104->tl,_tmpA3=((struct Cyc_List_List*)_check_null(_tmpA3))->tl)),++ i)){
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp105=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA3))->hd)->loc,_tmp105,(void*)_tmp104->hd);continue;}{void*
_tmp111=_tmp103;struct Cyc_Dict_Dict*_tmp112;_LLEB: if((int)_tmp111 != 0)goto _LLED;
_LLEC: goto _LLEA;_LLED: if(_tmp111 <= (void*)1?1:*((int*)_tmp111)!= 0)goto _LLEA;
_tmp112=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp111)->f1;_LLEE: if(Cyc_CfFlowInfo_initlevel(
_tmp112,(void*)_tmp104->hd)== (void*)0)({void*_tmp113[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpA3))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp113,
sizeof(void*),0));});{void*_tmp114=_tmp105;struct Cyc_Dict_Dict*_tmp115;struct Cyc_List_List*
_tmp116;_LLF0: if((int)_tmp114 != 0)goto _LLF2;_LLF1: goto _LLEF;_LLF2: if(_tmp114 <= (
void*)1?1:*((int*)_tmp114)!= 0)goto _LLEF;_tmp115=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp114)->f1;_tmp116=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp114)->f2;
_LLF3: {struct Cyc_Dict_Dict*_tmp117=Cyc_CfFlowInfo_escape_deref(_tmp115,env->all_changed,(
void*)_tmp104->hd);{void*_tmp118=(void*)_tmp104->hd;struct Cyc_CfFlowInfo_Place*
_tmp119;_LLF5: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 2)goto _LLF7;_tmp119=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp118)->f1;_LLF6:{void*_tmp11A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA3))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp11B;void*
_tmp11C;_LLFA: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 4)goto _LLFC;_tmp11B=((
struct Cyc_Absyn_PointerType_struct*)_tmp11A)->f1;_tmp11C=(void*)_tmp11B.elt_typ;
_LLFB: _tmp117=Cyc_CfFlowInfo_assign_place(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->loc,
_tmp117,env->all_changed,_tmp119,Cyc_CfFlowInfo_typ_to_absrval(_tmp11C,Cyc_CfFlowInfo_esc_all));
goto _LLF9;_LLFC:;_LLFD:({void*_tmp11D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("anal_Rexp: bad type for initialized arg",
sizeof(char),40),_tag_arr(_tmp11D,sizeof(void*),0));});_LLF9:;}goto _LLF4;_LLF7:;
_LLF8: goto _LLF4;_LLF4:;}_tmp105=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp11F;_tmp11F.tag=0;_tmp11F.f1=_tmp117;_tmp11F.f2=_tmp116;_tmp11F;});_tmp11E;});
goto _LLEF;}_LLEF:;}goto _LLEA;_LLEA:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpA2->topt))->v))return({struct _tuple0 _tmp120;
_tmp120.f1=(void*)0;_tmp120.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp120;});else{return({
struct _tuple0 _tmp121;_tmp121.f1=_tmp105;_tmp121.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp121;});}}}}_LL9B: if(*((int*)_tmp83)!= 33)goto _LL9D;_tmpA4=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp83)->f1;_tmpA5=_tmpA4.is_calloc;_tmpA6=_tmpA4.rgn;_tmpA7=_tmpA4.elt_type;
_tmpA8=_tmpA4.num_elts;_tmpA9=_tmpA4.fat_result;_LL9C: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp133;_tmp133.tag=1;_tmp133.f1=_tmpA8;
_tmp133.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp133;});_tmp132;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->root=(void*)root;_tmp131->fields=0;
_tmp131;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp12F=
_cycalloc(sizeof(*_tmp12F));_tmp12F[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp130;_tmp130.tag=2;_tmp130.f1=place;_tmp130;});_tmp12F;});void*place_val=
_tmpA9?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpA7)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpA6 != 0){void*
_tmp124;struct Cyc_List_List*_tmp125;struct _tuple5 _tmp123=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp122[2];_tmp122[1]=_tmpA8;_tmp122[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpA6);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp122,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp124=
_tmp123.f1;_tmp125=_tmp123.f2;outflow=_tmp124;}else{void*_tmp127;struct _tuple0
_tmp126=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA8);_tmp127=_tmp126.f1;
outflow=_tmp127;}{void*_tmp128=outflow;struct Cyc_Dict_Dict*_tmp129;struct Cyc_List_List*
_tmp12A;_LLFF: if((int)_tmp128 != 0)goto _LL101;_LL100: return({struct _tuple0 _tmp12B;
_tmp12B.f1=outflow;_tmp12B.f2=rval;_tmp12B;});_LL101: if(_tmp128 <= (void*)1?1:*((
int*)_tmp128)!= 0)goto _LLFE;_tmp129=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp128)->f1;_tmp12A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp128)->f2;
_LL102: return({struct _tuple0 _tmp12C;_tmp12C.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=Cyc_Dict_insert(_tmp129,root,place_val);_tmp12E.f2=
_tmp12A;_tmp12E;});_tmp12D;});_tmp12C.f2=rval;_tmp12C;});_LLFE:;}}_LL9D: if(*((
int*)_tmp83)!= 15)goto _LL9F;_tmpAA=((struct Cyc_Absyn_New_e_struct*)_tmp83)->f1;
_tmpAB=((struct Cyc_Absyn_New_e_struct*)_tmp83)->f2;_LL9E: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp146;_tmp146.tag=1;_tmp146.f1=_tmpAB;
_tmp146.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp146;});_tmp145;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->root=(void*)root;_tmp144->fields=0;
_tmp144;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp142=
_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp143;_tmp143.tag=2;_tmp143.f1=place;_tmp143;});_tmp142;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpAA != 0){
void*_tmp136;struct Cyc_List_List*_tmp137;struct _tuple5 _tmp135=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp134[2];_tmp134[1]=_tmpAB;_tmp134[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpAA);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp134,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp136=
_tmp135.f1;_tmp137=_tmp135.f2;outflow=_tmp136;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp137))->tl))->hd;}else{void*
_tmp139;void*_tmp13A;struct _tuple0 _tmp138=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpAB);_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;outflow=_tmp139;place_val=
_tmp13A;}{void*_tmp13B=outflow;struct Cyc_Dict_Dict*_tmp13C;struct Cyc_List_List*
_tmp13D;_LL104: if((int)_tmp13B != 0)goto _LL106;_LL105: return({struct _tuple0
_tmp13E;_tmp13E.f1=outflow;_tmp13E.f2=rval;_tmp13E;});_LL106: if(_tmp13B <= (void*)
1?1:*((int*)_tmp13B)!= 0)goto _LL103;_tmp13C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp13B)->f1;_tmp13D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13B)->f2;
_LL107: return({struct _tuple0 _tmp13F;_tmp13F.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp141;_tmp141.tag=0;_tmp141.f1=Cyc_Dict_insert(_tmp13C,root,place_val);_tmp141.f2=
_tmp13D;_tmp141;});_tmp140;});_tmp13F.f2=rval;_tmp13F;});_LL103:;}}}_LL9F: if(*((
int*)_tmp83)!= 14)goto _LLA1;_tmpAC=((struct Cyc_Absyn_Address_e_struct*)_tmp83)->f1;
_LLA0: {void*_tmp148;void*_tmp149;struct _tuple0 _tmp147=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpAC);_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;{void*_tmp14A=_tmp149;
struct Cyc_CfFlowInfo_Place*_tmp14B;_LL109: if((int)_tmp14A != 0)goto _LL10B;_LL10A:
return({struct _tuple0 _tmp14C;_tmp14C.f1=_tmp148;_tmp14C.f2=(void*)1;_tmp14C;});
_LL10B: if(_tmp14A <= (void*)1?1:*((int*)_tmp14A)!= 0)goto _LL108;_tmp14B=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp14A)->f1;_LL10C: return({struct _tuple0 _tmp14D;
_tmp14D.f1=_tmp148;_tmp14D.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp14F;_tmp14F.tag=2;_tmp14F.f1=_tmp14B;_tmp14F;});_tmp14E;});_tmp14D;});_LL108:;}}
_LLA1: if(*((int*)_tmp83)!= 20)goto _LLA3;_tmpAD=((struct Cyc_Absyn_Deref_e_struct*)
_tmp83)->f1;_LLA2: {void*_tmp151;void*_tmp152;struct _tuple0 _tmp150=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpAD);_tmp151=_tmp150.f1;_tmp152=_tmp150.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp151,_tmpAD,_tmp152);}_LLA3: if(*((int*)_tmp83)!= 21)goto _LLA5;
_tmpAE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp83)->f1;_tmpAF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp83)->f2;_LLA4: {void*_tmp154;void*_tmp155;struct _tuple0 _tmp153=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpAE);_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v))return({struct _tuple0
_tmp156;_tmp156.f1=_tmp154;_tmp156.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp156;});{
void*_tmp157=_tmp155;struct Cyc_Dict_Dict*_tmp158;_LL10E: if(_tmp157 <= (void*)3?1:*((
int*)_tmp157)!= 3)goto _LL110;_tmp158=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp157)->f1;_LL10F: return({struct _tuple0 _tmp159;_tmp159.f1=_tmp154;_tmp159.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp158,
_tmpAF);_tmp159;});_LL110:;_LL111:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Core_Impossible_struct
_tmp15B;_tmp15B.tag=Cyc_Core_Impossible;_tmp15B.f1=_tag_arr("anal_Rexp: AggrMember",
sizeof(char),22);_tmp15B;});_tmp15A;}));_LL10D:;}}_LLA5: if(*((int*)_tmp83)!= 22)
goto _LLA7;_tmpB0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp83)->f1;_tmpB1=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp83)->f2;_LLA6: {void*_tmp15D;void*
_tmp15E;struct _tuple0 _tmp15C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpB0);
_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;{void*_tmp160;void*_tmp161;struct _tuple0
_tmp15F=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp15D,_tmpB0,_tmp15E);
_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;{void*_tmp162=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp163;void*_tmp164;_LL113: if(_tmp162 <= (void*)3?1:*((int*)_tmp162)!= 4)goto
_LL115;_tmp163=((struct Cyc_Absyn_PointerType_struct*)_tmp162)->f1;_tmp164=(void*)
_tmp163.elt_typ;_LL114: if(Cyc_Absyn_is_union_type(_tmp164))return({struct _tuple0
_tmp165;_tmp165.f1=_tmp160;_tmp165.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp165;});
goto _LL112;_LL115:;_LL116:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Core_Impossible_struct
_tmp167;_tmp167.tag=Cyc_Core_Impossible;_tmp167.f1=_tag_arr("anal_Rexp: AggrArrow ptr",
sizeof(char),25);_tmp167;});_tmp166;}));_LL112:;}{void*_tmp168=_tmp161;struct Cyc_Dict_Dict*
_tmp169;_LL118: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 3)goto _LL11A;_tmp169=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp168)->f1;_LL119: return({struct _tuple0
_tmp16A;_tmp16A.f1=_tmp160;_tmp16A.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp169,_tmpB1);_tmp16A;});_LL11A:;_LL11B:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp16B=_cycalloc(sizeof(*
_tmp16B));_tmp16B[0]=({struct Cyc_Core_Impossible_struct _tmp16C;_tmp16C.tag=Cyc_Core_Impossible;
_tmp16C.f1=_tag_arr("anal_Rexp: AggrArrow",sizeof(char),21);_tmp16C;});_tmp16B;}));
_LL117:;}}}_LLA7: if(*((int*)_tmp83)!= 6)goto _LLA9;_tmpB2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp83)->f1;_tmpB3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f2;_tmpB4=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f3;_LLA8: {void*_tmp16E;void*
_tmp16F;struct _tuple0 _tmp16D=Cyc_NewControlFlow_anal_test(env,inflow,_tmpB2);
_tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;{void*_tmp171;void*_tmp172;struct _tuple0
_tmp170=Cyc_NewControlFlow_anal_Rexp(env,_tmp16E,_tmpB3);_tmp171=_tmp170.f1;
_tmp172=_tmp170.f2;{void*_tmp174;void*_tmp175;struct _tuple0 _tmp173=Cyc_NewControlFlow_anal_Rexp(
env,_tmp16F,_tmpB4);_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;return({struct _tuple0
_tmp176;_tmp176.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_use_Rval(
env,_tmpB3->loc,_tmp171,_tmp172),Cyc_NewControlFlow_use_Rval(env,_tmpB4->loc,
_tmp174,_tmp175));_tmp176.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp176;});}}}_LLA9: if(*((
int*)_tmp83)!= 23)goto _LLAB;_tmpB5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f1;
_tmpB6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f2;_LLAA: {void*_tmp179;
struct Cyc_List_List*_tmp17A;struct _tuple5 _tmp178=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp177[2];_tmp177[1]=_tmpB6;_tmp177[0]=_tmpB5;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp177,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp179=_tmp178.f1;_tmp17A=_tmp178.f2;{void*
_tmp17B=_tmp179;{void*_tmp17C=_tmp179;struct Cyc_Dict_Dict*_tmp17D;struct Cyc_List_List*
_tmp17E;_LL11D: if(_tmp17C <= (void*)1?1:*((int*)_tmp17C)!= 0)goto _LL11F;_tmp17D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp17C)->f1;_tmp17E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp17C)->f2;_LL11E: if(Cyc_CfFlowInfo_initlevel(_tmp17D,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp17A))->tl))->hd)== (void*)0)({
void*_tmp17F[0]={};Cyc_Tcutil_terr(_tmpB6->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp17F,sizeof(void*),0));});{struct Cyc_List_List*
_tmp180=Cyc_NewControlFlow_add_subscript_reln(_tmp17E,_tmpB5,_tmpB6);if(_tmp17E
!= _tmp180)_tmp17B=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp181=
_cycalloc(sizeof(*_tmp181));_tmp181[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp182;_tmp182.tag=0;_tmp182.f1=_tmp17D;_tmp182.f2=_tmp180;_tmp182;});_tmp181;});
goto _LL11C;}_LL11F:;_LL120: goto _LL11C;_LL11C:;}{void*_tmp183=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB5->topt))->v);_LL122: if(_tmp183 <= (
void*)3?1:*((int*)_tmp183)!= 9)goto _LL124;_LL123: {void*_tmp184=(void*)((struct
Cyc_List_List*)_check_null(_tmp17A))->hd;struct Cyc_Dict_Dict*_tmp185;_LL127: if(
_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 3)goto _LL129;_tmp185=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp184)->f1;_LL128: return({struct _tuple0 _tmp186;_tmp186.f1=_tmp17B;_tmp186.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp185,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpB6)).f1));_tmp186;});_LL129:;_LL12A:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp187=_cycalloc(sizeof(*
_tmp187));_tmp187[0]=({struct Cyc_Core_Impossible_struct _tmp188;_tmp188.tag=Cyc_Core_Impossible;
_tmp188.f1=_tag_arr("anal_Rexp: Subscript",sizeof(char),21);_tmp188;});_tmp187;}));
_LL126:;}_LL124:;_LL125: goto _LL121;_LL121:;}{void*_tmp18A;void*_tmp18B;struct
_tuple0 _tmp189=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp179,_tmpB5,(void*)((
struct Cyc_List_List*)_check_null(_tmp17A))->hd);_tmp18A=_tmp189.f1;_tmp18B=
_tmp189.f2;if(_tmp18A == (void*)0)return({struct _tuple0 _tmp18C;_tmp18C.f1=_tmp18A;
_tmp18C.f2=_tmp18B;_tmp18C;});else{return({struct _tuple0 _tmp18D;_tmp18D.f1=
_tmp17B;_tmp18D.f2=_tmp18B;_tmp18D;});}}}}_LLAB: if(*((int*)_tmp83)!= 30)goto
_LLAD;_tmpB7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp83)->f1;_LLAC: _tmpB8=_tmpB7;
goto _LLAE;_LLAD: if(*((int*)_tmp83)!= 24)goto _LLAF;_tmpB8=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp83)->f1;_LLAE: {void*_tmp18F;struct Cyc_List_List*_tmp190;struct _tuple5
_tmp18E=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmpB8);_tmp18F=
_tmp18E.f1;_tmp190=_tmp18E.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{
int i=0;for(0;_tmp190 != 0;(_tmp190=_tmp190->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(
i),(void*)_tmp190->hd);}}return({struct _tuple0 _tmp191;_tmp191.f1=_tmp18F;_tmp191.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp192=_cycalloc(sizeof(*_tmp192));
_tmp192[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp193;_tmp193.tag=3;_tmp193.f1=
aggrdict;_tmp193;});_tmp192;});_tmp191;});}}_LLAF: if(*((int*)_tmp83)!= 29)goto
_LLB1;_tmpB9=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp83)->f2;_LLB0: _tmpBA=
_tmpB9;goto _LLB2;_LLB1: if(*((int*)_tmp83)!= 28)goto _LLB3;_tmpBA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp83)->f3;_LLB2: {void*_tmp195;struct Cyc_List_List*_tmp196;struct _tuple5
_tmp194=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpBA));_tmp195=_tmp194.f1;
_tmp196=_tmp194.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{
int i=0;for(0;_tmp196 != 0;(((_tmp196=_tmp196->tl,_tmpBA=_tmpBA->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple6*)((struct Cyc_List_List*)_check_null(_tmpBA))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp197=(void*)ds->hd;struct _tagged_arr*_tmp198;
_LL12C: if(*((int*)_tmp197)!= 0)goto _LL12E;_LL12D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Core_Impossible_struct
_tmp19A;_tmp19A.tag=Cyc_Core_Impossible;_tmp19A.f1=_tag_arr("anal_Rexp:Struct_e",
sizeof(char),19);_tmp19A;});_tmp199;}));_LL12E: if(*((int*)_tmp197)!= 1)goto
_LL12B;_tmp198=((struct Cyc_Absyn_FieldName_struct*)_tmp197)->f1;_LL12F: aggrdict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(
aggrdict,_tmp198,(void*)_tmp196->hd);_LL12B:;}}}return({struct _tuple0 _tmp19B;
_tmp19B.f1=_tmp195;_tmp19B.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp19D;_tmp19D.tag=3;_tmp19D.f1=aggrdict;_tmp19D;});_tmp19C;});_tmp19B;});}}
_LLB3: if(*((int*)_tmp83)!= 26)goto _LLB5;_tmpBB=((struct Cyc_Absyn_Array_e_struct*)
_tmp83)->f1;_LLB4: {struct Cyc_List_List*_tmp19E=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((struct
Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpBB);void*_tmp1A0;struct Cyc_List_List*
_tmp1A1;struct _tuple5 _tmp19F=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,
_tmp19E);_tmp1A0=_tmp19F.f1;_tmp1A1=_tmp19F.f2;for(0;_tmp1A1 != 0;(_tmp1A1=
_tmp1A1->tl,_tmp19E=_tmp19E->tl)){_tmp1A0=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19E))->hd)->loc,
_tmp1A0,(void*)_tmp1A1->hd);}return({struct _tuple0 _tmp1A2;_tmp1A2.f1=_tmp1A0;
_tmp1A2.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1A2;});}_LLB5: if(*((int*)
_tmp83)!= 27)goto _LLB7;_tmpBC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f1;
_tmpBD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f2;_tmpBE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp83)->f3;_LLB6: {void*_tmp1A4;void*_tmp1A5;struct _tuple0 _tmp1A3=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBD);_tmp1A4=_tmp1A3.f1;_tmp1A5=_tmp1A3.f2;{void*_tmp1A6=_tmp1A4;
struct Cyc_Dict_Dict*_tmp1A7;struct Cyc_List_List*_tmp1A8;_LL131: if((int)_tmp1A6 != 
0)goto _LL133;_LL132: return({struct _tuple0 _tmp1A9;_tmp1A9.f1=_tmp1A4;_tmp1A9.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1A9;});_LL133: if(_tmp1A6 <= (void*)1?1:*((int*)
_tmp1A6)!= 0)goto _LL130;_tmp1A7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A6)->f1;_tmp1A8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A6)->f2;
_LL134: if(Cyc_CfFlowInfo_initlevel(_tmp1A7,_tmp1A5)== (void*)0)({void*_tmp1AA[0]={};
Cyc_Tcutil_terr(_tmpBD->loc,_tag_arr("expression may not be initialized",sizeof(
char),34),_tag_arr(_tmp1AA,sizeof(void*),0));});{struct Cyc_List_List*new_relns=
_tmp1A8;comp_loop: {void*_tmp1AB=(void*)_tmpBD->r;struct Cyc_Absyn_Exp*_tmp1AC;
void*_tmp1AD;struct Cyc_Absyn_Vardecl*_tmp1AE;void*_tmp1AF;struct Cyc_Absyn_Vardecl*
_tmp1B0;void*_tmp1B1;struct Cyc_Absyn_Vardecl*_tmp1B2;void*_tmp1B3;struct Cyc_Absyn_Vardecl*
_tmp1B4;void*_tmp1B5;int _tmp1B6;void*_tmp1B7;struct Cyc_List_List*_tmp1B8;struct
Cyc_List_List _tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;_LL136: if(*((int*)_tmp1AB)!= 13)
goto _LL138;_tmp1AC=((struct Cyc_Absyn_Cast_e_struct*)_tmp1AB)->f2;_LL137: _tmpBD=
_tmp1AC;goto comp_loop;_LL138: if(*((int*)_tmp1AB)!= 1)goto _LL13A;_tmp1AD=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp1AB)->f2;if(_tmp1AD <= (void*)1?1:*((int*)
_tmp1AD)!= 0)goto _LL13A;_tmp1AE=((struct Cyc_Absyn_Global_b_struct*)_tmp1AD)->f1;
if(!(!_tmp1AE->escapes))goto _LL13A;_LL139: _tmp1B0=_tmp1AE;goto _LL13B;_LL13A: if(*((
int*)_tmp1AB)!= 1)goto _LL13C;_tmp1AF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1AB)->f2;if(_tmp1AF <= (void*)1?1:*((int*)_tmp1AF)!= 3)goto _LL13C;_tmp1B0=((
struct Cyc_Absyn_Local_b_struct*)_tmp1AF)->f1;if(!(!_tmp1B0->escapes))goto _LL13C;
_LL13B: _tmp1B2=_tmp1B0;goto _LL13D;_LL13C: if(*((int*)_tmp1AB)!= 1)goto _LL13E;
_tmp1B1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1AB)->f2;if(_tmp1B1 <= (void*)
1?1:*((int*)_tmp1B1)!= 4)goto _LL13E;_tmp1B2=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1B1)->f1;if(!(!_tmp1B2->escapes))goto _LL13E;_LL13D: _tmp1B4=_tmp1B2;goto
_LL13F;_LL13E: if(*((int*)_tmp1AB)!= 1)goto _LL140;_tmp1B3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1AB)->f2;if(_tmp1B3 <= (void*)1?1:*((int*)_tmp1B3)!= 2)goto _LL140;_tmp1B4=((
struct Cyc_Absyn_Param_b_struct*)_tmp1B3)->f1;if(!(!_tmp1B4->escapes))goto _LL140;
_LL13F: new_relns=({struct Cyc_List_List*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));
_tmp1BC->vd=_tmpBC;_tmp1BC->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp1BE;_tmp1BE.tag=1;_tmp1BE.f1=_tmp1B4;_tmp1BE;});_tmp1BD;}));_tmp1BC;});
_tmp1BB->tl=_tmp1A8;_tmp1BB;});goto _LL135;_LL140: if(*((int*)_tmp1AB)!= 0)goto
_LL142;_tmp1B5=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1AB)->f1;if(_tmp1B5
<= (void*)1?1:*((int*)_tmp1B5)!= 2)goto _LL142;_tmp1B6=((struct Cyc_Absyn_Int_c_struct*)
_tmp1B5)->f2;_LL141: new_relns=({struct Cyc_List_List*_tmp1BF=_cycalloc(sizeof(*
_tmp1BF));_tmp1BF->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1C0=_cycalloc(sizeof(*
_tmp1C0));_tmp1C0->vd=_tmpBC;_tmp1C0->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1C1=_cycalloc_atomic(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1C2;_tmp1C2.tag=3;_tmp1C2.f1=(unsigned int)_tmp1B6;_tmp1C2;});_tmp1C1;}));
_tmp1C0;});_tmp1BF->tl=_tmp1A8;_tmp1BF;});goto _LL135;_LL142: if(*((int*)_tmp1AB)
!= 3)goto _LL144;_tmp1B7=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1AB)->f1;
if((int)_tmp1B7 != 19)goto _LL144;_tmp1B8=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1AB)->f2;if(_tmp1B8 == 0)goto _LL144;_tmp1B9=*_tmp1B8;_tmp1BA=(struct Cyc_Absyn_Exp*)
_tmp1B9.hd;_LL143:{void*_tmp1C3=(void*)_tmp1BA->r;void*_tmp1C4;struct Cyc_Absyn_Vardecl*
_tmp1C5;void*_tmp1C6;struct Cyc_Absyn_Vardecl*_tmp1C7;void*_tmp1C8;struct Cyc_Absyn_Vardecl*
_tmp1C9;void*_tmp1CA;struct Cyc_Absyn_Vardecl*_tmp1CB;_LL147: if(*((int*)_tmp1C3)
!= 1)goto _LL149;_tmp1C4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1C3)->f2;if(
_tmp1C4 <= (void*)1?1:*((int*)_tmp1C4)!= 0)goto _LL149;_tmp1C5=((struct Cyc_Absyn_Global_b_struct*)
_tmp1C4)->f1;if(!(!_tmp1C5->escapes))goto _LL149;_LL148: _tmp1C7=_tmp1C5;goto
_LL14A;_LL149: if(*((int*)_tmp1C3)!= 1)goto _LL14B;_tmp1C6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1C3)->f2;if(_tmp1C6 <= (void*)1?1:*((int*)_tmp1C6)!= 3)goto _LL14B;_tmp1C7=((
struct Cyc_Absyn_Local_b_struct*)_tmp1C6)->f1;if(!(!_tmp1C7->escapes))goto _LL14B;
_LL14A: _tmp1C9=_tmp1C7;goto _LL14C;_LL14B: if(*((int*)_tmp1C3)!= 1)goto _LL14D;
_tmp1C8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1C3)->f2;if(_tmp1C8 <= (void*)
1?1:*((int*)_tmp1C8)!= 4)goto _LL14D;_tmp1C9=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1C8)->f1;if(!(!_tmp1C9->escapes))goto _LL14D;_LL14C: _tmp1CB=_tmp1C9;goto
_LL14E;_LL14D: if(*((int*)_tmp1C3)!= 1)goto _LL14F;_tmp1CA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1C3)->f2;if(_tmp1CA <= (void*)1?1:*((int*)_tmp1CA)!= 2)goto _LL14F;_tmp1CB=((
struct Cyc_Absyn_Param_b_struct*)_tmp1CA)->f1;if(!(!_tmp1CB->escapes))goto _LL14F;
_LL14E: new_relns=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));
_tmp1CC->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));
_tmp1CD->vd=_tmpBC;_tmp1CD->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp1CF;_tmp1CF.tag=2;_tmp1CF.f1=_tmp1CB;_tmp1CF;});_tmp1CE;}));_tmp1CD;});
_tmp1CC->tl=_tmp1A8;_tmp1CC;});goto _LL146;_LL14F:;_LL150: goto _LL146;_LL146:;}
goto _LL135;_LL144:;_LL145: goto _LL135;_LL135:;}if(_tmp1A8 != new_relns)_tmp1A4=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1D0=_cycalloc(sizeof(*
_tmp1D0));_tmp1D0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1D1;_tmp1D1.tag=
0;_tmp1D1.f1=_tmp1A7;_tmp1D1.f2=new_relns;_tmp1D1;});_tmp1D0;});{void*_tmp1D2=
_tmp1A5;_LL152: if((int)_tmp1D2 != 0)goto _LL154;_LL153: return({struct _tuple0
_tmp1D3;_tmp1D3.f1=_tmp1A4;_tmp1D3.f2=Cyc_CfFlowInfo_unknown_all;_tmp1D3;});
_LL154: if((int)_tmp1D2 != 2)goto _LL156;_LL155: goto _LL157;_LL156: if((int)_tmp1D2 != 
1)goto _LL158;_LL157: goto _LL159;_LL158: if(_tmp1D2 <= (void*)3?1:*((int*)_tmp1D2)!= 
2)goto _LL15A;_LL159: {struct Cyc_List_List _tmp1D4=({struct Cyc_List_List _tmp1DD;
_tmp1DD.hd=_tmpBC;_tmp1DD.tl=0;_tmp1DD;});_tmp1A4=Cyc_NewControlFlow_add_vars(
_tmp1A4,(struct Cyc_List_List*)& _tmp1D4,Cyc_CfFlowInfo_unknown_all);{void*_tmp1D6;
void*_tmp1D7;struct _tuple0 _tmp1D5=Cyc_NewControlFlow_anal_Rexp(env,_tmp1A4,
_tmpBE);_tmp1D6=_tmp1D5.f1;_tmp1D7=_tmp1D5.f2;{void*_tmp1D8=_tmp1D6;struct Cyc_Dict_Dict*
_tmp1D9;_LL15D: if((int)_tmp1D8 != 0)goto _LL15F;_LL15E: return({struct _tuple0
_tmp1DA;_tmp1DA.f1=_tmp1D6;_tmp1DA.f2=Cyc_CfFlowInfo_unknown_all;_tmp1DA;});
_LL15F: if(_tmp1D8 <= (void*)1?1:*((int*)_tmp1D8)!= 0)goto _LL15C;_tmp1D9=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1D8)->f1;_LL160: if(Cyc_CfFlowInfo_initlevel(
_tmp1D9,_tmp1D7)!= (void*)2){({void*_tmp1DB[0]={};Cyc_Tcutil_terr(_tmpBD->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp1DB,
sizeof(void*),0));});return({struct _tuple0 _tmp1DC;_tmp1DC.f1=(void*)0;_tmp1DC.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1DC;});}_LL15C:;}_tmp1A4=_tmp1D6;goto _LL15B;}}
_LL15A:;_LL15B: while(1){struct Cyc_List_List _tmp1DE=({struct Cyc_List_List _tmp1E7;
_tmp1E7.hd=_tmpBC;_tmp1E7.tl=0;_tmp1E7;});_tmp1A4=Cyc_NewControlFlow_add_vars(
_tmp1A4,(struct Cyc_List_List*)& _tmp1DE,Cyc_CfFlowInfo_unknown_all);{void*_tmp1E0;
void*_tmp1E1;struct _tuple0 _tmp1DF=Cyc_NewControlFlow_anal_Rexp(env,_tmp1A4,
_tmpBE);_tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;{void*_tmp1E2=_tmp1E0;struct Cyc_Dict_Dict*
_tmp1E3;_LL162: if((int)_tmp1E2 != 0)goto _LL164;_LL163: goto _LL161;_LL164: if(
_tmp1E2 <= (void*)1?1:*((int*)_tmp1E2)!= 0)goto _LL161;_tmp1E3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1E2)->f1;_LL165: if(Cyc_CfFlowInfo_initlevel(_tmp1E3,_tmp1E1)!= (void*)2){({
void*_tmp1E4[0]={};Cyc_Tcutil_terr(_tmpBD->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp1E4,sizeof(void*),0));});return({struct _tuple0
_tmp1E5;_tmp1E5.f1=(void*)0;_tmp1E5.f2=Cyc_CfFlowInfo_unknown_all;_tmp1E5;});}
_LL161:;}{void*_tmp1E6=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp1A4,_tmp1E0);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1E6,_tmp1A4))break;_tmp1A4=_tmp1E6;}}}
return({struct _tuple0 _tmp1E8;_tmp1E8.f1=_tmp1A4;_tmp1E8.f2=Cyc_CfFlowInfo_unknown_all;
_tmp1E8;});_LL151:;}}_LL130:;}}_LLB7: if(*((int*)_tmp83)!= 35)goto _LLB9;_tmpBF=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp83)->f1;_LLB8: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmpBF))->flow;{void*_tmp1E9=(void*)_tmpBF->r;struct Cyc_Absyn_Stmt*
_tmp1EA;struct Cyc_Absyn_Stmt*_tmp1EB;struct Cyc_Absyn_Decl*_tmp1EC;struct Cyc_Absyn_Stmt*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;_LL167: if(_tmp1E9 <= (void*)1?1:*((int*)
_tmp1E9)!= 1)goto _LL169;_tmp1EA=((struct Cyc_Absyn_Seq_s_struct*)_tmp1E9)->f1;
_tmp1EB=((struct Cyc_Absyn_Seq_s_struct*)_tmp1E9)->f2;_LL168: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp1EA);_tmpBF=_tmp1EB;goto _LL166;_LL169: if(_tmp1E9 <= (void*)1?1:*((
int*)_tmp1E9)!= 12)goto _LL16B;_tmp1EC=((struct Cyc_Absyn_Decl_s_struct*)_tmp1E9)->f1;
_tmp1ED=((struct Cyc_Absyn_Decl_s_struct*)_tmp1E9)->f2;_LL16A: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp1EC);_tmpBF=_tmp1ED;goto _LL166;_LL16B: if(_tmp1E9 <= (void*)1?1:*((
int*)_tmp1E9)!= 0)goto _LL16D;_tmp1EE=((struct Cyc_Absyn_Exp_s_struct*)_tmp1E9)->f1;
_LL16C: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1EE);_LL16D:;_LL16E:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1EF=_cycalloc(sizeof(*
_tmp1EF));_tmp1EF[0]=({struct Cyc_Core_Impossible_struct _tmp1F0;_tmp1F0.tag=Cyc_Core_Impossible;
_tmp1F0.f1=_tag_arr("analyze_Rexp: ill-formed StmtExp",sizeof(char),33);_tmp1F0;});
_tmp1EF;}));_LL166:;}}_LLB9: if(*((int*)_tmp83)!= 1)goto _LLBB;_tmpC0=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((int)_tmpC0 != 0)goto _LLBB;_LLBA:
goto _LLBC;_LLBB: if(*((int*)_tmp83)!= 2)goto _LLBD;_LLBC: goto _LLBE;_LLBD: if(*((int*)
_tmp83)!= 8)goto _LLBF;_LLBE: goto _LLC0;_LLBF: if(*((int*)_tmp83)!= 34)goto _LLC1;
_LLC0: goto _LLC2;_LLC1: if(*((int*)_tmp83)!= 25)goto _LLC3;_LLC2: goto _LLC4;_LLC3:
if(*((int*)_tmp83)!= 36)goto _LLC5;_LLC4: goto _LLC6;_LLC5: if(*((int*)_tmp83)!= 37)
goto _LL66;_LLC6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1F1=
_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Core_Impossible_struct _tmp1F2;
_tmp1F2.tag=Cyc_Core_Impossible;_tmp1F2.f1=_tag_arr("anal_Rexp, unexpected exp form",
sizeof(char),31);_tmp1F2;});_tmp1F1;}));_LL66:;}}static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,
void*r,struct Cyc_List_List*flds){void*_tmp1F3=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1F4;
void*_tmp1F5;struct Cyc_Absyn_Conref*_tmp1F6;_LL170: if(_tmp1F3 <= (void*)3?1:*((
int*)_tmp1F3)!= 4)goto _LL172;_tmp1F4=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F3)->f1;_tmp1F5=(void*)_tmp1F4.elt_typ;_tmp1F6=_tmp1F4.bounds;_LL171: {void*
_tmp1F7=f;struct Cyc_Dict_Dict*_tmp1F8;struct Cyc_List_List*_tmp1F9;_LL175: if((int)
_tmp1F7 != 0)goto _LL177;_LL176: return({struct _tuple0 _tmp1FA;_tmp1FA.f1=f;_tmp1FA.f2=(
void*)0;_tmp1FA;});_LL177: if(_tmp1F7 <= (void*)1?1:*((int*)_tmp1F7)!= 0)goto
_LL174;_tmp1F8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F7)->f1;_tmp1F9=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F7)->f2;_LL178: if(Cyc_Tcutil_is_bound_one(
_tmp1F6)){void*_tmp1FB=r;struct Cyc_CfFlowInfo_Place*_tmp1FC;struct Cyc_CfFlowInfo_Place
_tmp1FD;void*_tmp1FE;struct Cyc_List_List*_tmp1FF;void*_tmp200;_LL17A: if((int)
_tmp1FB != 1)goto _LL17C;_LL17B: goto _LL17D;_LL17C: if((int)_tmp1FB != 2)goto _LL17E;
_LL17D:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp202;_tmp202.tag=Cyc_CfFlowInfo_NotZero;_tmp202.f1=_tmp1F9;_tmp202;});_tmp201;})));
goto _LL179;_LL17E: if(_tmp1FB <= (void*)3?1:*((int*)_tmp1FB)!= 2)goto _LL180;
_tmp1FC=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FB)->f1;_tmp1FD=*_tmp1FC;
_tmp1FE=(void*)_tmp1FD.root;_tmp1FF=_tmp1FD.fields;_LL17F:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp203=_cycalloc(sizeof(*_tmp203));
_tmp203[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp204;_tmp204.tag=Cyc_CfFlowInfo_NotZero;
_tmp204.f1=_tmp1F9;_tmp204;});_tmp203;})));return({struct _tuple0 _tmp205;_tmp205.f1=
f;_tmp205.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp206=_cycalloc(
sizeof(*_tmp206));_tmp206[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp207;_tmp207.tag=
0;_tmp207.f1=({struct Cyc_CfFlowInfo_Place*_tmp208=_cycalloc(sizeof(*_tmp208));
_tmp208->root=(void*)_tmp1FE;_tmp208->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1FF,flds);_tmp208;});_tmp207;});
_tmp206;});_tmp205;});_LL180: if((int)_tmp1FB != 0)goto _LL182;_LL181:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmp209=_cycalloc(sizeof(*
_tmp209));_tmp209[0]=({struct Cyc_CfFlowInfo_IsZero_struct _tmp20A;_tmp20A.tag=Cyc_CfFlowInfo_IsZero;
_tmp20A.f1=_tmp1F9;_tmp20A;});_tmp209;})));return({struct _tuple0 _tmp20B;_tmp20B.f1=(
void*)0;_tmp20B.f2=(void*)0;_tmp20B;});_LL182: if(_tmp1FB <= (void*)3?1:*((int*)
_tmp1FB)!= 0)goto _LL184;_tmp200=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp1FB)->f1;_LL183: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp200);goto
_LL185;_LL184:;_LL185:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp20D;_tmp20D.tag=Cyc_CfFlowInfo_UnknownZ;_tmp20D.f1=_tmp1F9;_tmp20D;});
_tmp20C;})));_LL179:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp20F;_tmp20F.tag=Cyc_CfFlowInfo_UnknownZ;_tmp20F.f1=_tmp1F9;_tmp20F;});
_tmp20E;})));}if(Cyc_CfFlowInfo_initlevel(_tmp1F8,r)== (void*)0)({void*_tmp210[0]={};
Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(char),46),_tag_arr(_tmp210,sizeof(void*),0));});return({struct _tuple0
_tmp211;_tmp211.f1=f;_tmp211.f2=(void*)0;_tmp211;});_LL174:;}_LL172:;_LL173:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp212=_cycalloc(sizeof(*
_tmp212));_tmp212[0]=({struct Cyc_Core_Impossible_struct _tmp213;_tmp213.tag=Cyc_Core_Impossible;
_tmp213.f1=_tag_arr("left deref of non-pointer-type",sizeof(char),31);_tmp213;});
_tmp212;}));_LL16F:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp214=inflow;struct Cyc_Dict_Dict*_tmp215;
struct Cyc_List_List*_tmp216;_LL187: if((int)_tmp214 != 0)goto _LL189;_LL188: return({
struct _tuple0 _tmp217;_tmp217.f1=(void*)0;_tmp217.f2=(void*)0;_tmp217;});_LL189:
if(_tmp214 <= (void*)1?1:*((int*)_tmp214)!= 0)goto _LL186;_tmp215=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp214)->f1;_tmp216=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp214)->f2;
_LL18A: d=_tmp215;_LL186:;}{void*_tmp218=(void*)e->r;struct Cyc_Absyn_Exp*_tmp219;
struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;void*_tmp21C;struct Cyc_Absyn_Vardecl*
_tmp21D;void*_tmp21E;struct Cyc_Absyn_Vardecl*_tmp21F;void*_tmp220;struct Cyc_Absyn_Vardecl*
_tmp221;void*_tmp222;struct Cyc_Absyn_Vardecl*_tmp223;struct Cyc_Absyn_Exp*_tmp224;
struct _tagged_arr*_tmp225;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*
_tmp227;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_Absyn_Exp*_tmp229;struct
_tagged_arr*_tmp22A;_LL18C: if(*((int*)_tmp218)!= 13)goto _LL18E;_tmp219=((struct
Cyc_Absyn_Cast_e_struct*)_tmp218)->f2;_LL18D: _tmp21A=_tmp219;goto _LL18F;_LL18E:
if(*((int*)_tmp218)!= 11)goto _LL190;_tmp21A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp218)->f1;_LL18F: _tmp21B=_tmp21A;goto _LL191;_LL190: if(*((int*)_tmp218)!= 12)
goto _LL192;_tmp21B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp218)->f1;_LL191:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp21B,flds);_LL192: if(*((int*)
_tmp218)!= 1)goto _LL194;_tmp21C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp218)->f2;
if(_tmp21C <= (void*)1?1:*((int*)_tmp21C)!= 2)goto _LL194;_tmp21D=((struct Cyc_Absyn_Param_b_struct*)
_tmp21C)->f1;_LL193: _tmp21F=_tmp21D;goto _LL195;_LL194: if(*((int*)_tmp218)!= 1)
goto _LL196;_tmp21E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp218)->f2;if(
_tmp21E <= (void*)1?1:*((int*)_tmp21E)!= 3)goto _LL196;_tmp21F=((struct Cyc_Absyn_Local_b_struct*)
_tmp21E)->f1;_LL195: _tmp221=_tmp21F;goto _LL197;_LL196: if(*((int*)_tmp218)!= 1)
goto _LL198;_tmp220=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp218)->f2;if(
_tmp220 <= (void*)1?1:*((int*)_tmp220)!= 4)goto _LL198;_tmp221=((struct Cyc_Absyn_Pat_b_struct*)
_tmp220)->f1;_LL197: return({struct _tuple0 _tmp22B;_tmp22B.f1=inflow;_tmp22B.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp22C=_cycalloc(sizeof(*_tmp22C));
_tmp22C[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp22D;_tmp22D.tag=0;_tmp22D.f1=({
struct Cyc_CfFlowInfo_Place*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp22F=_cycalloc(sizeof(*
_tmp22F));_tmp22F[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp230;_tmp230.tag=0;
_tmp230.f1=_tmp221;_tmp230;});_tmp22F;}));_tmp22E->fields=flds;_tmp22E;});
_tmp22D;});_tmp22C;});_tmp22B;});_LL198: if(*((int*)_tmp218)!= 1)goto _LL19A;
_tmp222=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp218)->f2;if(_tmp222 <= (void*)
1?1:*((int*)_tmp222)!= 0)goto _LL19A;_tmp223=((struct Cyc_Absyn_Global_b_struct*)
_tmp222)->f1;_LL199: return({struct _tuple0 _tmp231;_tmp231.f1=inflow;_tmp231.f2=(
void*)0;_tmp231;});_LL19A: if(*((int*)_tmp218)!= 22)goto _LL19C;_tmp224=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp218)->f1;_tmp225=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp218)->f2;_LL19B: if(!Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp224->topt))->v))flds=({struct Cyc_List_List*_tmp232=_cycalloc(
sizeof(*_tmp232));_tmp232->hd=_tmp225;_tmp232->tl=flds;_tmp232;});_tmp226=
_tmp224;goto _LL19D;_LL19C: if(*((int*)_tmp218)!= 20)goto _LL19E;_tmp226=((struct
Cyc_Absyn_Deref_e_struct*)_tmp218)->f1;_LL19D: {void*_tmp234;void*_tmp235;struct
_tuple0 _tmp233=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp226);_tmp234=_tmp233.f1;
_tmp235=_tmp233.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp234,
_tmp226,_tmp235,flds);}_LL19E: if(*((int*)_tmp218)!= 23)goto _LL1A0;_tmp227=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp218)->f1;_tmp228=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp218)->f2;_LL19F:{void*_tmp236=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp227->topt))->v);_LL1A5: if(_tmp236 <= (void*)3?1:*((int*)_tmp236)
!= 9)goto _LL1A7;_LL1A6: {struct _tagged_arr*_tmp237=Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(
_tmp228)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp227,({struct
Cyc_List_List*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238->hd=_tmp237;_tmp238->tl=
flds;_tmp238;}));}_LL1A7:;_LL1A8: goto _LL1A4;_LL1A4:;}{void*_tmp23B;struct Cyc_List_List*
_tmp23C;struct _tuple5 _tmp23A=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp239[2];_tmp239[1]=_tmp228;_tmp239[0]=_tmp227;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp239,sizeof(struct Cyc_Absyn_Exp*),
2));}));_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;{void*_tmp23D=_tmp23B;{void*
_tmp23E=_tmp23B;struct Cyc_Dict_Dict*_tmp23F;struct Cyc_List_List*_tmp240;_LL1AA:
if(_tmp23E <= (void*)1?1:*((int*)_tmp23E)!= 0)goto _LL1AC;_tmp23F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp23E)->f1;_tmp240=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp23E)->f2;
_LL1AB: if(Cyc_CfFlowInfo_initlevel(_tmp23F,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp23C))->tl))->hd)== (void*)0)({
void*_tmp241[0]={};Cyc_Tcutil_terr(_tmp228->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp241,sizeof(void*),0));});{struct Cyc_List_List*
_tmp242=Cyc_NewControlFlow_add_subscript_reln(_tmp240,_tmp227,_tmp228);if(
_tmp240 != _tmp242)_tmp23D=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp244;_tmp244.tag=0;_tmp244.f1=_tmp23F;_tmp244.f2=_tmp242;_tmp244;});_tmp243;});
goto _LL1A9;}_LL1AC:;_LL1AD: goto _LL1A9;_LL1A9:;}{void*_tmp246;void*_tmp247;struct
_tuple0 _tmp245=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp23B,_tmp227,(void*)((
struct Cyc_List_List*)_check_null(_tmp23C))->hd,flds);_tmp246=_tmp245.f1;_tmp247=
_tmp245.f2;if(_tmp246 == (void*)0)return({struct _tuple0 _tmp248;_tmp248.f1=_tmp246;
_tmp248.f2=_tmp247;_tmp248;});else{return({struct _tuple0 _tmp249;_tmp249.f1=
_tmp23D;_tmp249.f2=_tmp247;_tmp249;});}}}}_LL1A0: if(*((int*)_tmp218)!= 21)goto
_LL1A2;_tmp229=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp218)->f1;_tmp22A=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp218)->f2;_LL1A1: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp229->topt))->v))return({struct
_tuple0 _tmp24A;_tmp24A.f1=inflow;_tmp24A.f2=(void*)0;_tmp24A;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp229,({struct Cyc_List_List*_tmp24B=_cycalloc(sizeof(*_tmp24B));
_tmp24B->hd=_tmp22A;_tmp24B->tl=flds;_tmp24B;}));_LL1A2:;_LL1A3: return({struct
_tuple0 _tmp24C;_tmp24C.f1=(void*)0;_tmp24C.f2=(void*)0;_tmp24C;});_LL18B:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp24D=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp24E;struct Cyc_Absyn_Exp*_tmp24F;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Absyn_Exp*
_tmp251;struct Cyc_Absyn_Exp*_tmp252;void*_tmp253;struct Cyc_List_List*_tmp254;
struct Cyc_List_List _tmp255;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_List_List*
_tmp257;void*_tmp258;struct Cyc_List_List*_tmp259;void*_tmp25A;struct Cyc_List_List*
_tmp25B;void*_tmp25C;struct Cyc_List_List*_tmp25D;void*_tmp25E;struct Cyc_List_List*
_tmp25F;_LL1AF: if(*((int*)_tmp24D)!= 6)goto _LL1B1;_tmp24E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp24D)->f1;_tmp24F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp24D)->f2;
_tmp250=((struct Cyc_Absyn_Conditional_e_struct*)_tmp24D)->f3;_LL1B0: {void*
_tmp261;void*_tmp262;struct _tuple0 _tmp260=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp24E);_tmp261=_tmp260.f1;_tmp262=_tmp260.f2;{void*_tmp264;void*_tmp265;
struct _tuple0 _tmp263=Cyc_NewControlFlow_anal_test(env,_tmp261,_tmp24F);_tmp264=
_tmp263.f1;_tmp265=_tmp263.f2;{void*_tmp267;void*_tmp268;struct _tuple0 _tmp266=
Cyc_NewControlFlow_anal_test(env,_tmp262,_tmp250);_tmp267=_tmp266.f1;_tmp268=
_tmp266.f2;return({struct _tuple0 _tmp269;_tmp269.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp264,_tmp267);_tmp269.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp265,
_tmp268);_tmp269;});}}}_LL1B1: if(*((int*)_tmp24D)!= 7)goto _LL1B3;_tmp251=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp24D)->f1;_tmp252=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp24D)->f2;_LL1B2: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp251)).f1,_tmp252);_LL1B3: if(*((int*)_tmp24D)!= 3)goto _LL1B5;
_tmp253=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f1;if((int)_tmp253 != 
11)goto _LL1B5;_tmp254=((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f2;if(_tmp254
== 0)goto _LL1B5;_tmp255=*_tmp254;_tmp256=(struct Cyc_Absyn_Exp*)_tmp255.hd;
_tmp257=_tmp255.tl;if(_tmp257 != 0)goto _LL1B5;_LL1B4: {void*_tmp26B;void*_tmp26C;
struct _tuple0 _tmp26A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp256);_tmp26B=
_tmp26A.f1;_tmp26C=_tmp26A.f2;return({struct _tuple0 _tmp26D;_tmp26D.f1=_tmp26C;
_tmp26D.f2=_tmp26B;_tmp26D;});}_LL1B5: if(*((int*)_tmp24D)!= 3)goto _LL1B7;_tmp258=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f1;if((int)_tmp258 != 5)goto
_LL1B7;_tmp259=((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f2;_LL1B6: {void*
_tmp26F;struct Cyc_List_List*_tmp270;struct _tuple5 _tmp26E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp259);_tmp26F=_tmp26E.f1;_tmp270=_tmp26E.f2;{void*_tmp271=_tmp26F;
struct Cyc_Dict_Dict*_tmp272;struct Cyc_List_List*_tmp273;_LL1C0: if((int)_tmp271 != 
0)goto _LL1C2;_LL1C1: return({struct _tuple0 _tmp274;_tmp274.f1=_tmp26F;_tmp274.f2=
_tmp26F;_tmp274;});_LL1C2: if(_tmp271 <= (void*)1?1:*((int*)_tmp271)!= 0)goto
_LL1BF;_tmp272=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp271)->f1;_tmp273=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp271)->f2;_LL1C3: {void*_tmp275=(
void*)((struct Cyc_List_List*)_check_null(_tmp270))->hd;void*_tmp276=(void*)((
struct Cyc_List_List*)_check_null(_tmp270->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp272,_tmp275)== (void*)0)({void*_tmp277[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp259))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp277,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp272,_tmp276)== (void*)0)({void*_tmp278[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp259))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp278,
sizeof(void*),0));});{struct _tuple0 _tmp27A=({struct _tuple0 _tmp279;_tmp279.f1=
_tmp275;_tmp279.f2=_tmp276;_tmp279;});void*_tmp27B;void*_tmp27C;void*_tmp27D;
void*_tmp27E;void*_tmp27F;void*_tmp280;_LL1C5: _tmp27B=_tmp27A.f1;if(_tmp27B <= (
void*)3?1:*((int*)_tmp27B)!= 0)goto _LL1C7;_tmp27C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp27B)->f1;_tmp27D=_tmp27A.f2;if((int)_tmp27D != 0)goto _LL1C7;_LL1C6: {void*
_tmp282;void*_tmp283;struct _tuple0 _tmp281=Cyc_NewControlFlow_splitzero(env,
_tmp26F,_tmp26F,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp259))->hd,
_tmp27C);_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;return({struct _tuple0 _tmp284;
_tmp284.f1=_tmp283;_tmp284.f2=_tmp282;_tmp284;});}_LL1C7: _tmp27E=_tmp27A.f1;if((
int)_tmp27E != 0)goto _LL1C9;_tmp27F=_tmp27A.f2;if(_tmp27F <= (void*)3?1:*((int*)
_tmp27F)!= 0)goto _LL1C9;_tmp280=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp27F)->f1;_LL1C8: {void*_tmp286;void*_tmp287;struct _tuple0 _tmp285=Cyc_NewControlFlow_splitzero(
env,_tmp26F,_tmp26F,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp259))->tl))->hd,_tmp280);_tmp286=_tmp285.f1;
_tmp287=_tmp285.f2;return({struct _tuple0 _tmp288;_tmp288.f1=_tmp287;_tmp288.f2=
_tmp286;_tmp288;});}_LL1C9:;_LL1CA: return({struct _tuple0 _tmp289;_tmp289.f1=
_tmp26F;_tmp289.f2=_tmp26F;_tmp289;});_LL1C4:;}}_LL1BF:;}}_LL1B7: if(*((int*)
_tmp24D)!= 3)goto _LL1B9;_tmp25A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp24D)->f1;if((int)_tmp25A != 6)goto _LL1B9;_tmp25B=((struct Cyc_Absyn_Primop_e_struct*)
_tmp24D)->f2;_LL1B8: {void*_tmp28B;struct Cyc_List_List*_tmp28C;struct _tuple5
_tmp28A=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp25B);_tmp28B=
_tmp28A.f1;_tmp28C=_tmp28A.f2;{void*_tmp28D=_tmp28B;struct Cyc_Dict_Dict*_tmp28E;
struct Cyc_List_List*_tmp28F;_LL1CC: if((int)_tmp28D != 0)goto _LL1CE;_LL1CD: return({
struct _tuple0 _tmp290;_tmp290.f1=_tmp28B;_tmp290.f2=_tmp28B;_tmp290;});_LL1CE: if(
_tmp28D <= (void*)1?1:*((int*)_tmp28D)!= 0)goto _LL1CB;_tmp28E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp28D)->f1;_tmp28F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp28D)->f2;
_LL1CF: {void*_tmp291=(void*)((struct Cyc_List_List*)_check_null(_tmp28C))->hd;
void*_tmp292=(void*)((struct Cyc_List_List*)_check_null(_tmp28C->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp28E,_tmp291)== (void*)0)({void*_tmp293[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp25B))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp293,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp28E,_tmp292)== (void*)0)({void*_tmp294[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp25B))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp294,
sizeof(void*),0));});{struct _tuple0 _tmp296=({struct _tuple0 _tmp295;_tmp295.f1=
_tmp291;_tmp295.f2=_tmp292;_tmp295;});void*_tmp297;void*_tmp298;void*_tmp299;
void*_tmp29A;void*_tmp29B;void*_tmp29C;_LL1D1: _tmp297=_tmp296.f1;if(_tmp297 <= (
void*)3?1:*((int*)_tmp297)!= 0)goto _LL1D3;_tmp298=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp297)->f1;_tmp299=_tmp296.f2;if((int)_tmp299 != 0)goto _LL1D3;_LL1D2: return Cyc_NewControlFlow_splitzero(
env,_tmp28B,_tmp28B,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp25B))->hd,_tmp298);_LL1D3: _tmp29A=_tmp296.f1;if((int)_tmp29A != 0)goto _LL1D5;
_tmp29B=_tmp296.f2;if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 0)goto _LL1D5;
_tmp29C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp29B)->f1;_LL1D4:
return Cyc_NewControlFlow_splitzero(env,_tmp28B,_tmp28B,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp25B))->tl))->hd,
_tmp29C);_LL1D5:;_LL1D6: return({struct _tuple0 _tmp29D;_tmp29D.f1=_tmp28B;_tmp29D.f2=
_tmp28B;_tmp29D;});_LL1D0:;}}_LL1CB:;}}_LL1B9: if(*((int*)_tmp24D)!= 3)goto _LL1BB;
_tmp25C=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f1;if((int)_tmp25C != 
8)goto _LL1BB;_tmp25D=((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f2;_LL1BA: {
void*_tmp29F;struct Cyc_List_List*_tmp2A0;struct _tuple5 _tmp29E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp25D);_tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;{void*_tmp2A1=_tmp29F;
struct Cyc_Dict_Dict*_tmp2A2;struct Cyc_List_List*_tmp2A3;_LL1D8: if((int)_tmp2A1 != 
0)goto _LL1DA;_LL1D9: return({struct _tuple0 _tmp2A4;_tmp2A4.f1=_tmp29F;_tmp2A4.f2=
_tmp29F;_tmp2A4;});_LL1DA: if(_tmp2A1 <= (void*)1?1:*((int*)_tmp2A1)!= 0)goto
_LL1D7;_tmp2A2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A1)->f1;_tmp2A3=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A1)->f2;_LL1DB: {void*_tmp2A5=(
void*)((struct Cyc_List_List*)_check_null(_tmp2A0))->hd;void*_tmp2A6=(void*)((
struct Cyc_List_List*)_check_null(_tmp2A0->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp2A2,_tmp2A5)== (void*)0)({void*_tmp2A7[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp25D))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp2A7,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp2A2,_tmp2A6)== (void*)0)({void*_tmp2A8[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp25D))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp2A8,
sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp2A9=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp25D))->hd;struct Cyc_Absyn_Exp*_tmp2AA=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp25D->tl))->hd;void*_tmp2AB=(void*)_tmp2A9->r;
void*_tmp2AC;struct Cyc_Absyn_Vardecl*_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_Vardecl*
_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2B1;void*_tmp2B2;struct Cyc_Absyn_Vardecl*
_tmp2B3;_LL1DD: if(*((int*)_tmp2AB)!= 1)goto _LL1DF;_tmp2AC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2AC <= (void*)1?1:*((int*)_tmp2AC)!= 0)goto _LL1DF;_tmp2AD=((
struct Cyc_Absyn_Global_b_struct*)_tmp2AC)->f1;if(!(!_tmp2AD->escapes))goto _LL1DF;
_LL1DE: _tmp2AF=_tmp2AD;goto _LL1E0;_LL1DF: if(*((int*)_tmp2AB)!= 1)goto _LL1E1;
_tmp2AE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AB)->f2;if(_tmp2AE <= (void*)
1?1:*((int*)_tmp2AE)!= 3)goto _LL1E1;_tmp2AF=((struct Cyc_Absyn_Local_b_struct*)
_tmp2AE)->f1;if(!(!_tmp2AF->escapes))goto _LL1E1;_LL1E0: _tmp2B1=_tmp2AF;goto
_LL1E2;_LL1E1: if(*((int*)_tmp2AB)!= 1)goto _LL1E3;_tmp2B0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2B0 <= (void*)1?1:*((int*)_tmp2B0)!= 4)goto _LL1E3;_tmp2B1=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2B0)->f1;if(!(!_tmp2B1->escapes))goto _LL1E3;
_LL1E2: _tmp2B3=_tmp2B1;goto _LL1E4;_LL1E3: if(*((int*)_tmp2AB)!= 1)goto _LL1E5;
_tmp2B2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AB)->f2;if(_tmp2B2 <= (void*)
1?1:*((int*)_tmp2B2)!= 2)goto _LL1E5;_tmp2B3=((struct Cyc_Absyn_Param_b_struct*)
_tmp2B2)->f1;if(!(!_tmp2B3->escapes))goto _LL1E5;_LL1E4: {void*_tmp2B4=(void*)
_tmp2AA->r;void*_tmp2B5;struct Cyc_Absyn_Vardecl*_tmp2B6;void*_tmp2B7;struct Cyc_Absyn_Vardecl*
_tmp2B8;void*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2BA;void*_tmp2BB;struct Cyc_Absyn_Vardecl*
_tmp2BC;void*_tmp2BD;int _tmp2BE;void*_tmp2BF;struct Cyc_List_List*_tmp2C0;struct
Cyc_List_List _tmp2C1;struct Cyc_Absyn_Exp*_tmp2C2;_LL1E8: if(*((int*)_tmp2B4)!= 1)
goto _LL1EA;_tmp2B5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B4)->f2;if(
_tmp2B5 <= (void*)1?1:*((int*)_tmp2B5)!= 0)goto _LL1EA;_tmp2B6=((struct Cyc_Absyn_Global_b_struct*)
_tmp2B5)->f1;if(!(!_tmp2B6->escapes))goto _LL1EA;_LL1E9: _tmp2B8=_tmp2B6;goto
_LL1EB;_LL1EA: if(*((int*)_tmp2B4)!= 1)goto _LL1EC;_tmp2B7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B4)->f2;if(_tmp2B7 <= (void*)1?1:*((int*)_tmp2B7)!= 3)goto _LL1EC;_tmp2B8=((
struct Cyc_Absyn_Local_b_struct*)_tmp2B7)->f1;if(!(!_tmp2B8->escapes))goto _LL1EC;
_LL1EB: _tmp2BA=_tmp2B8;goto _LL1ED;_LL1EC: if(*((int*)_tmp2B4)!= 1)goto _LL1EE;
_tmp2B9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B4)->f2;if(_tmp2B9 <= (void*)
1?1:*((int*)_tmp2B9)!= 4)goto _LL1EE;_tmp2BA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2B9)->f1;if(!(!_tmp2BA->escapes))goto _LL1EE;_LL1ED: _tmp2BC=_tmp2BA;goto
_LL1EF;_LL1EE: if(*((int*)_tmp2B4)!= 1)goto _LL1F0;_tmp2BB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B4)->f2;if(_tmp2BB <= (void*)1?1:*((int*)_tmp2BB)!= 2)goto _LL1F0;_tmp2BC=((
struct Cyc_Absyn_Param_b_struct*)_tmp2BB)->f1;if(!(!_tmp2BC->escapes))goto _LL1F0;
_LL1EF:{struct _tuple0 _tmp2C4=({struct _tuple0 _tmp2C3;_tmp2C3.f1=Cyc_Tcutil_compress((
void*)_tmp2B3->type);_tmp2C3.f2=Cyc_Tcutil_compress((void*)_tmp2BC->type);
_tmp2C3;});void*_tmp2C5;void*_tmp2C6;void*_tmp2C7;void*_tmp2C8;void*_tmp2C9;void*
_tmp2CA;_LL1F7: _tmp2C5=_tmp2C4.f1;if(_tmp2C5 <= (void*)3?1:*((int*)_tmp2C5)!= 5)
goto _LL1F9;_tmp2C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C5)->f1;if((int)
_tmp2C6 != 1)goto _LL1F9;_tmp2C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C5)->f2;
if((int)_tmp2C7 != 2)goto _LL1F9;_LL1F8: goto _LL1F6;_LL1F9: _tmp2C8=_tmp2C4.f2;if(
_tmp2C8 <= (void*)3?1:*((int*)_tmp2C8)!= 5)goto _LL1FB;_tmp2C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C8)->f1;if((int)_tmp2C9 != 1)goto _LL1FB;_tmp2CA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C8)->f2;if((int)_tmp2CA != 2)goto _LL1FB;_LL1FA: goto _LL1F6;_LL1FB:;_LL1FC:
return({struct _tuple0 _tmp2CB;_tmp2CB.f1=_tmp29F;_tmp2CB.f2=_tmp29F;_tmp2CB;});
_LL1F6:;}{struct Cyc_List_List*_tmp2CC=({struct Cyc_List_List*_tmp2D0=_cycalloc(
sizeof(*_tmp2D0));_tmp2D0->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2D1=_cycalloc(
sizeof(*_tmp2D1));_tmp2D1->vd=_tmp2B3;_tmp2D1->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp2D3;_tmp2D3.tag=1;_tmp2D3.f1=_tmp2BC;_tmp2D3;});_tmp2D2;}));_tmp2D1;});
_tmp2D0->tl=_tmp2A3;_tmp2D0;});return({struct _tuple0 _tmp2CD;_tmp2CD.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));
_tmp2CE[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2CF;_tmp2CF.tag=0;
_tmp2CF.f1=_tmp2A2;_tmp2CF.f2=_tmp2CC;_tmp2CF;});_tmp2CE;});_tmp2CD.f2=_tmp29F;
_tmp2CD;});}_LL1F0: if(*((int*)_tmp2B4)!= 0)goto _LL1F2;_tmp2BD=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2B4)->f1;if(_tmp2BD <= (void*)1?1:*((int*)_tmp2BD)!= 2)goto _LL1F2;_tmp2BE=((
struct Cyc_Absyn_Int_c_struct*)_tmp2BD)->f2;_LL1F1:{struct _tuple0 _tmp2D5=({struct
_tuple0 _tmp2D4;_tmp2D4.f1=Cyc_Tcutil_compress((void*)_tmp2B3->type);_tmp2D4.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2AA->topt))->v);
_tmp2D4;});void*_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;void*_tmp2DA;void*
_tmp2DB;_LL1FE: _tmp2D6=_tmp2D5.f1;if(_tmp2D6 <= (void*)3?1:*((int*)_tmp2D6)!= 5)
goto _LL200;_tmp2D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D6)->f1;if((int)
_tmp2D7 != 1)goto _LL200;_tmp2D8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D6)->f2;
if((int)_tmp2D8 != 2)goto _LL200;_LL1FF: goto _LL1FD;_LL200: _tmp2D9=_tmp2D5.f2;if(
_tmp2D9 <= (void*)3?1:*((int*)_tmp2D9)!= 5)goto _LL202;_tmp2DA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2D9)->f1;if((int)_tmp2DA != 1)goto _LL202;_tmp2DB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2D9)->f2;if((int)_tmp2DB != 2)goto _LL202;_LL201: goto _LL1FD;_LL202:;_LL203:
return({struct _tuple0 _tmp2DC;_tmp2DC.f1=_tmp29F;_tmp2DC.f2=_tmp29F;_tmp2DC;});
_LL1FD:;}{struct Cyc_List_List*_tmp2DD=({struct Cyc_List_List*_tmp2E1=_cycalloc(
sizeof(*_tmp2E1));_tmp2E1->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2E2=_cycalloc(
sizeof(*_tmp2E2));_tmp2E2->vd=_tmp2B3;_tmp2E2->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp2E3=_cycalloc_atomic(sizeof(*_tmp2E3));_tmp2E3[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp2E4;_tmp2E4.tag=3;_tmp2E4.f1=(unsigned int)_tmp2BE;_tmp2E4;});_tmp2E3;}));
_tmp2E2;});_tmp2E1->tl=_tmp2A3;_tmp2E1;});return({struct _tuple0 _tmp2DE;_tmp2DE.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2DF=_cycalloc(sizeof(*
_tmp2DF));_tmp2DF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2E0;_tmp2E0.tag=
0;_tmp2E0.f1=_tmp2A2;_tmp2E0.f2=_tmp2DD;_tmp2E0;});_tmp2DF;});_tmp2DE.f2=_tmp29F;
_tmp2DE;});}_LL1F2: if(*((int*)_tmp2B4)!= 3)goto _LL1F4;_tmp2BF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B4)->f1;if((int)_tmp2BF != 19)goto _LL1F4;_tmp2C0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B4)->f2;if(_tmp2C0 == 0)goto _LL1F4;_tmp2C1=*_tmp2C0;_tmp2C2=(struct Cyc_Absyn_Exp*)
_tmp2C1.hd;_LL1F3: {void*_tmp2E5=(void*)_tmp2C2->r;void*_tmp2E6;struct Cyc_Absyn_Vardecl*
_tmp2E7;void*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;_LL205: if(*((int*)_tmp2E5)
!= 1)goto _LL207;_tmp2E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E5)->f2;if(
_tmp2E6 <= (void*)1?1:*((int*)_tmp2E6)!= 0)goto _LL207;_tmp2E7=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E6)->f1;if(!(!_tmp2E7->escapes))goto _LL207;_LL206: _tmp2E9=_tmp2E7;goto
_LL208;_LL207: if(*((int*)_tmp2E5)!= 1)goto _LL209;_tmp2E8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E5)->f2;if(_tmp2E8 <= (void*)1?1:*((int*)_tmp2E8)!= 3)goto _LL209;_tmp2E9=((
struct Cyc_Absyn_Local_b_struct*)_tmp2E8)->f1;if(!(!_tmp2E9->escapes))goto _LL209;
_LL208: _tmp2EB=_tmp2E9;goto _LL20A;_LL209: if(*((int*)_tmp2E5)!= 1)goto _LL20B;
_tmp2EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E5)->f2;if(_tmp2EA <= (void*)
1?1:*((int*)_tmp2EA)!= 4)goto _LL20B;_tmp2EB=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2EA)->f1;if(!(!_tmp2EB->escapes))goto _LL20B;_LL20A: _tmp2ED=_tmp2EB;goto
_LL20C;_LL20B: if(*((int*)_tmp2E5)!= 1)goto _LL20D;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E5)->f2;if(_tmp2EC <= (void*)1?1:*((int*)_tmp2EC)!= 2)goto _LL20D;_tmp2ED=((
struct Cyc_Absyn_Param_b_struct*)_tmp2EC)->f1;if(!(!_tmp2ED->escapes))goto _LL20D;
_LL20C: {struct Cyc_List_List*_tmp2EE=({struct Cyc_List_List*_tmp2F2=_cycalloc(
sizeof(*_tmp2F2));_tmp2F2->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F3=_cycalloc(
sizeof(*_tmp2F3));_tmp2F3->vd=_tmp2B3;_tmp2F3->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp2F5;_tmp2F5.tag=2;_tmp2F5.f1=_tmp2ED;_tmp2F5;});_tmp2F4;}));_tmp2F3;});
_tmp2F2->tl=_tmp2A3;_tmp2F2;});return({struct _tuple0 _tmp2EF;_tmp2EF.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));
_tmp2F0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2F1;_tmp2F1.tag=0;
_tmp2F1.f1=_tmp2A2;_tmp2F1.f2=_tmp2EE;_tmp2F1;});_tmp2F0;});_tmp2EF.f2=_tmp29F;
_tmp2EF;});}_LL20D:;_LL20E: return({struct _tuple0 _tmp2F6;_tmp2F6.f1=_tmp29F;
_tmp2F6.f2=_tmp29F;_tmp2F6;});_LL204:;}_LL1F4:;_LL1F5: return({struct _tuple0
_tmp2F7;_tmp2F7.f1=_tmp29F;_tmp2F7.f2=_tmp29F;_tmp2F7;});_LL1E7:;}_LL1E5:;_LL1E6:
return({struct _tuple0 _tmp2F8;_tmp2F8.f1=_tmp29F;_tmp2F8.f2=_tmp29F;_tmp2F8;});
_LL1DC:;}}_LL1D7:;}}_LL1BB: if(*((int*)_tmp24D)!= 3)goto _LL1BD;_tmp25E=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f1;if((int)_tmp25E != 10)goto _LL1BD;
_tmp25F=((struct Cyc_Absyn_Primop_e_struct*)_tmp24D)->f2;_LL1BC: {void*_tmp2FA;
struct Cyc_List_List*_tmp2FB;struct _tuple5 _tmp2F9=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp25F);_tmp2FA=_tmp2F9.f1;_tmp2FB=_tmp2F9.f2;{void*_tmp2FC=_tmp2FA;
struct Cyc_Dict_Dict*_tmp2FD;struct Cyc_List_List*_tmp2FE;_LL210: if((int)_tmp2FC != 
0)goto _LL212;_LL211: return({struct _tuple0 _tmp2FF;_tmp2FF.f1=_tmp2FA;_tmp2FF.f2=
_tmp2FA;_tmp2FF;});_LL212: if(_tmp2FC <= (void*)1?1:*((int*)_tmp2FC)!= 0)goto
_LL20F;_tmp2FD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2FC)->f1;_tmp2FE=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2FC)->f2;_LL213: {void*_tmp300=(
void*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd;void*_tmp301=(void*)((
struct Cyc_List_List*)_check_null(_tmp2FB->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp2FD,_tmp300)== (void*)0)({void*_tmp302[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp25F))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp302,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp2FD,_tmp301)== (void*)0)({void*_tmp303[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp25F))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp303,
sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp304=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp25F))->hd;struct Cyc_Absyn_Exp*_tmp305=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp25F->tl))->hd;void*_tmp306=(void*)_tmp304->r;
void*_tmp307;struct Cyc_Absyn_Vardecl*_tmp308;void*_tmp309;struct Cyc_Absyn_Vardecl*
_tmp30A;void*_tmp30B;struct Cyc_Absyn_Vardecl*_tmp30C;void*_tmp30D;struct Cyc_Absyn_Vardecl*
_tmp30E;_LL215: if(*((int*)_tmp306)!= 1)goto _LL217;_tmp307=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp306)->f2;if(_tmp307 <= (void*)1?1:*((int*)_tmp307)!= 0)goto _LL217;_tmp308=((
struct Cyc_Absyn_Global_b_struct*)_tmp307)->f1;if(!(!_tmp308->escapes))goto _LL217;
_LL216: _tmp30A=_tmp308;goto _LL218;_LL217: if(*((int*)_tmp306)!= 1)goto _LL219;
_tmp309=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp306)->f2;if(_tmp309 <= (void*)
1?1:*((int*)_tmp309)!= 3)goto _LL219;_tmp30A=((struct Cyc_Absyn_Local_b_struct*)
_tmp309)->f1;if(!(!_tmp30A->escapes))goto _LL219;_LL218: _tmp30C=_tmp30A;goto
_LL21A;_LL219: if(*((int*)_tmp306)!= 1)goto _LL21B;_tmp30B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp306)->f2;if(_tmp30B <= (void*)1?1:*((int*)_tmp30B)!= 4)goto _LL21B;_tmp30C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp30B)->f1;if(!(!_tmp30C->escapes))goto _LL21B;
_LL21A: _tmp30E=_tmp30C;goto _LL21C;_LL21B: if(*((int*)_tmp306)!= 1)goto _LL21D;
_tmp30D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp306)->f2;if(_tmp30D <= (void*)
1?1:*((int*)_tmp30D)!= 2)goto _LL21D;_tmp30E=((struct Cyc_Absyn_Param_b_struct*)
_tmp30D)->f1;if(!(!_tmp30E->escapes))goto _LL21D;_LL21C: {void*_tmp30F=(void*)
_tmp305->r;void*_tmp310;struct Cyc_List_List*_tmp311;struct Cyc_List_List _tmp312;
struct Cyc_Absyn_Exp*_tmp313;_LL220: if(*((int*)_tmp30F)!= 3)goto _LL222;_tmp310=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp30F)->f1;if((int)_tmp310 != 19)goto
_LL222;_tmp311=((struct Cyc_Absyn_Primop_e_struct*)_tmp30F)->f2;if(_tmp311 == 0)
goto _LL222;_tmp312=*_tmp311;_tmp313=(struct Cyc_Absyn_Exp*)_tmp312.hd;_LL221: {
void*_tmp314=(void*)_tmp313->r;void*_tmp315;struct Cyc_Absyn_Vardecl*_tmp316;void*
_tmp317;struct Cyc_Absyn_Vardecl*_tmp318;void*_tmp319;struct Cyc_Absyn_Vardecl*
_tmp31A;void*_tmp31B;struct Cyc_Absyn_Vardecl*_tmp31C;_LL225: if(*((int*)_tmp314)
!= 1)goto _LL227;_tmp315=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp314)->f2;if(
_tmp315 <= (void*)1?1:*((int*)_tmp315)!= 0)goto _LL227;_tmp316=((struct Cyc_Absyn_Global_b_struct*)
_tmp315)->f1;if(!(!_tmp316->escapes))goto _LL227;_LL226: _tmp318=_tmp316;goto
_LL228;_LL227: if(*((int*)_tmp314)!= 1)goto _LL229;_tmp317=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp314)->f2;if(_tmp317 <= (void*)1?1:*((int*)_tmp317)!= 3)goto _LL229;_tmp318=((
struct Cyc_Absyn_Local_b_struct*)_tmp317)->f1;if(!(!_tmp318->escapes))goto _LL229;
_LL228: _tmp31A=_tmp318;goto _LL22A;_LL229: if(*((int*)_tmp314)!= 1)goto _LL22B;
_tmp319=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp314)->f2;if(_tmp319 <= (void*)
1?1:*((int*)_tmp319)!= 4)goto _LL22B;_tmp31A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp319)->f1;if(!(!_tmp31A->escapes))goto _LL22B;_LL22A: _tmp31C=_tmp31A;goto
_LL22C;_LL22B: if(*((int*)_tmp314)!= 1)goto _LL22D;_tmp31B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp314)->f2;if(_tmp31B <= (void*)1?1:*((int*)_tmp31B)!= 2)goto _LL22D;_tmp31C=((
struct Cyc_Absyn_Param_b_struct*)_tmp31B)->f1;if(!(!_tmp31C->escapes))goto _LL22D;
_LL22C: {struct Cyc_List_List*_tmp31D=({struct Cyc_List_List*_tmp321=_cycalloc(
sizeof(*_tmp321));_tmp321->hd=({struct Cyc_CfFlowInfo_Reln*_tmp322=_cycalloc(
sizeof(*_tmp322));_tmp322->vd=_tmp30E;_tmp322->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp324;_tmp324.tag=4;_tmp324.f1=_tmp31C;_tmp324;});_tmp323;}));_tmp322;});
_tmp321->tl=_tmp2FE;_tmp321;});return({struct _tuple0 _tmp31E;_tmp31E.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));
_tmp31F[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp320;_tmp320.tag=0;
_tmp320.f1=_tmp2FD;_tmp320.f2=_tmp31D;_tmp320;});_tmp31F;});_tmp31E.f2=_tmp2FA;
_tmp31E;});}_LL22D:;_LL22E: return({struct _tuple0 _tmp325;_tmp325.f1=_tmp2FA;
_tmp325.f2=_tmp2FA;_tmp325;});_LL224:;}_LL222:;_LL223: return({struct _tuple0
_tmp326;_tmp326.f1=_tmp2FA;_tmp326.f2=_tmp2FA;_tmp326;});_LL21F:;}_LL21D:;_LL21E:
return({struct _tuple0 _tmp327;_tmp327.f1=_tmp2FA;_tmp327.f2=_tmp2FA;_tmp327;});
_LL214:;}}_LL20F:;}}_LL1BD:;_LL1BE: goto _LL1AE;_LL1AE:;}{void*_tmp329;void*
_tmp32A;struct _tuple0 _tmp328=Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_tmp329=
_tmp328.f1;_tmp32A=_tmp328.f2;{void*_tmp32B=_tmp329;struct Cyc_Dict_Dict*_tmp32C;
struct Cyc_List_List*_tmp32D;_LL230: if((int)_tmp32B != 0)goto _LL232;_LL231: return({
struct _tuple0 _tmp32E;_tmp32E.f1=_tmp329;_tmp32E.f2=_tmp329;_tmp32E;});_LL232: if(
_tmp32B <= (void*)1?1:*((int*)_tmp32B)!= 0)goto _LL22F;_tmp32C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp32B)->f1;_tmp32D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp32B)->f2;
_LL233: {void*_tmp32F=_tmp32A;void*_tmp330;void*_tmp331;void*_tmp332;_LL235: if((
int)_tmp32F != 0)goto _LL237;_LL236: return({struct _tuple0 _tmp333;_tmp333.f1=(void*)
0;_tmp333.f2=_tmp329;_tmp333;});_LL237: if((int)_tmp32F != 2)goto _LL239;_LL238:
goto _LL23A;_LL239: if((int)_tmp32F != 1)goto _LL23B;_LL23A: goto _LL23C;_LL23B: if(
_tmp32F <= (void*)3?1:*((int*)_tmp32F)!= 2)goto _LL23D;_LL23C: return({struct
_tuple0 _tmp334;_tmp334.f1=_tmp329;_tmp334.f2=(void*)0;_tmp334;});_LL23D: if(
_tmp32F <= (void*)3?1:*((int*)_tmp32F)!= 0)goto _LL23F;_tmp330=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp32F)->f1;if((int)_tmp330 != 0)goto _LL23F;_LL23E: goto _LL240;_LL23F: if(_tmp32F
<= (void*)3?1:*((int*)_tmp32F)!= 1)goto _LL241;_tmp331=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp32F)->f1;if((int)_tmp331 != 0)goto _LL241;_LL240:({void*_tmp335[0]={};Cyc_Tcutil_terr(
e->loc,_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(
_tmp335,sizeof(void*),0));});return({struct _tuple0 _tmp336;_tmp336.f1=(void*)0;
_tmp336.f2=(void*)0;_tmp336;});_LL241: if(_tmp32F <= (void*)3?1:*((int*)_tmp32F)!= 
0)goto _LL243;_tmp332=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp32F)->f1;
_LL242: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp329,e,_tmp332);_LL243:
if(_tmp32F <= (void*)3?1:*((int*)_tmp32F)!= 1)goto _LL245;_LL244: return({struct
_tuple0 _tmp337;_tmp337.f1=_tmp329;_tmp337.f2=_tmp329;_tmp337;});_LL245: if(
_tmp32F <= (void*)3?1:*((int*)_tmp32F)!= 3)goto _LL234;_LL246:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338[0]=({
struct Cyc_Core_Impossible_struct _tmp339;_tmp339.tag=Cyc_Core_Impossible;_tmp339.f1=
_tag_arr("anal_test",sizeof(char),10);_tmp339;});_tmp338;}));_LL234:;}_LL22F:;}}}
static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,
struct Cyc_NewControlFlow_AnalEnv*env,void*flow){void*_tmp33A=flow;struct Cyc_Dict_Dict*
_tmp33B;_LL248: if((int)_tmp33A != 0)goto _LL24A;_LL249: return;_LL24A: if(_tmp33A <= (
void*)1?1:*((int*)_tmp33A)!= 0)goto _LL247;_tmp33B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp33A)->f1;_LL24B:{struct Cyc_List_List*_tmp33C=env->param_roots;for(0;_tmp33C
!= 0;_tmp33C=_tmp33C->tl){if(Cyc_CfFlowInfo_initlevel(_tmp33B,Cyc_CfFlowInfo_lookup_place(
_tmp33B,(struct Cyc_CfFlowInfo_Place*)_tmp33C->hd))!= (void*)2)({void*_tmp33D[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("function may not initialize all the parameters with attribute 'initializes'",
sizeof(char),76),_tag_arr(_tmp33D,sizeof(void*),0));});}}return;_LL247:;}static
void*Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp33F;struct Cyc_Core_Opt*_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Stmt*
_tmp342;struct Cyc_Absyn_Switch_clause*_tmp33E=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp33F=*_tmp33E;_tmp340=_tmp33F.pat_vars;_tmp341=_tmp33F.where_clause;
_tmp342=_tmp33F.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp340))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp341 != 0){void*_tmp344;void*_tmp345;struct _tuple0
_tmp343=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp341));_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;inflow=_tmp345;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp344,_tmp342);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp342);}{void*
_tmp346=clause_outflow;_LL24D: if((int)_tmp346 != 0)goto _LL24F;_LL24E: goto _LL24C;
_LL24F:;_LL250: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp347[0]={};Cyc_Tcutil_terr(
_tmp342->loc,_tag_arr("switch clause may implicitly fallthru",sizeof(char),38),
_tag_arr(_tmp347,sizeof(void*),0));});else{({void*_tmp348[0]={};Cyc_Tcutil_warn(
_tmp342->loc,_tag_arr("switch clause may implicitly fallthru",sizeof(char),38),
_tag_arr(_tmp348,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL24C;_LL24C:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp349=Cyc_NewControlFlow_pre_stmt_check(env,
inflow,s);inflow=(void*)_tmp349->flow;{void*_tmp34A=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp34B;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Stmt*
_tmp34E;struct Cyc_Absyn_Stmt*_tmp34F;struct Cyc_Absyn_Exp*_tmp350;struct Cyc_Absyn_Stmt*
_tmp351;struct Cyc_Absyn_Stmt*_tmp352;struct _tuple3 _tmp353;struct Cyc_Absyn_Exp*
_tmp354;struct Cyc_Absyn_Stmt*_tmp355;struct Cyc_Absyn_Stmt*_tmp356;struct Cyc_Absyn_Stmt*
_tmp357;struct _tuple3 _tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Stmt*
_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct _tuple3 _tmp35C;struct Cyc_Absyn_Exp*
_tmp35D;struct Cyc_Absyn_Stmt*_tmp35E;struct _tuple3 _tmp35F;struct Cyc_Absyn_Exp*
_tmp360;struct Cyc_Absyn_Stmt*_tmp361;struct Cyc_Absyn_Stmt*_tmp362;struct Cyc_Absyn_Stmt*
_tmp363;struct Cyc_List_List*_tmp364;struct Cyc_Absyn_Switch_clause**_tmp365;
struct Cyc_Absyn_Switch_clause*_tmp366;struct Cyc_Absyn_Stmt*_tmp367;struct Cyc_Absyn_Stmt*
_tmp368;struct Cyc_Absyn_Stmt*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_List_List*
_tmp36B;struct Cyc_Absyn_Stmt*_tmp36C;struct Cyc_List_List*_tmp36D;struct Cyc_Absyn_Decl*
_tmp36E;struct Cyc_Absyn_Stmt*_tmp36F;struct Cyc_Absyn_Stmt*_tmp370;struct Cyc_Absyn_Tvar*
_tmp371;struct Cyc_Absyn_Vardecl*_tmp372;int _tmp373;struct Cyc_Absyn_Stmt*_tmp374;
struct Cyc_Absyn_Exp*_tmp375;_LL252: if((int)_tmp34A != 0)goto _LL254;_LL253: return
inflow;_LL254: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 2)goto _LL256;_tmp34B=((
struct Cyc_Absyn_Return_s_struct*)_tmp34A)->f1;if(_tmp34B != 0)goto _LL256;_LL255:
if(env->noreturn)({void*_tmp376[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(char),33),_tag_arr(_tmp376,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(void*)0;_LL256: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)
!= 2)goto _LL258;_tmp34C=((struct Cyc_Absyn_Return_s_struct*)_tmp34A)->f1;_LL257:
if(env->noreturn)({void*_tmp377[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(char),33),_tag_arr(_tmp377,sizeof(void*),0));});{void*_tmp379;void*
_tmp37A;struct _tuple0 _tmp378=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp34C));_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;_tmp379=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)_check_null(_tmp34C))->loc,_tmp379,_tmp37A);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp379);return(void*)0;}_LL258: if(_tmp34A <= (void*)1?1:*((int*)
_tmp34A)!= 0)goto _LL25A;_tmp34D=((struct Cyc_Absyn_Exp_s_struct*)_tmp34A)->f1;
_LL259: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp34D)).f1;_LL25A: if(
_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 1)goto _LL25C;_tmp34E=((struct Cyc_Absyn_Seq_s_struct*)
_tmp34A)->f1;_tmp34F=((struct Cyc_Absyn_Seq_s_struct*)_tmp34A)->f2;_LL25B: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp34E),
_tmp34F);_LL25C: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 3)goto _LL25E;_tmp350=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp34A)->f1;_tmp351=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp34A)->f2;_tmp352=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp34A)->f3;_LL25D: {
void*_tmp37C;void*_tmp37D;struct _tuple0 _tmp37B=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp350);_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;return Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_anal_stmt(env,_tmp37C,_tmp351),Cyc_NewControlFlow_anal_stmt(
env,_tmp37D,_tmp352));}_LL25E: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 4)goto
_LL260;_tmp353=((struct Cyc_Absyn_While_s_struct*)_tmp34A)->f1;_tmp354=_tmp353.f1;
_tmp355=_tmp353.f2;_tmp356=((struct Cyc_Absyn_While_s_struct*)_tmp34A)->f2;_LL25F: {
void*_tmp37E=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp355))->flow;
void*_tmp380;void*_tmp381;struct _tuple0 _tmp37F=Cyc_NewControlFlow_anal_test(env,
_tmp37E,_tmp354);_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;{void*_tmp382=Cyc_NewControlFlow_anal_stmt(
env,_tmp380,_tmp356);Cyc_NewControlFlow_update_flow(env,_tmp355,_tmp382);return
_tmp381;}}_LL260: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 16)goto _LL262;
_tmp357=((struct Cyc_Absyn_Do_s_struct*)_tmp34A)->f1;_tmp358=((struct Cyc_Absyn_Do_s_struct*)
_tmp34A)->f2;_tmp359=_tmp358.f1;_tmp35A=_tmp358.f2;_LL261: {void*_tmp383=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp357);void*_tmp384=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp383,_tmp35A))->flow;void*_tmp386;void*_tmp387;struct _tuple0 _tmp385=Cyc_NewControlFlow_anal_test(
env,_tmp384,_tmp359);_tmp386=_tmp385.f1;_tmp387=_tmp385.f2;Cyc_NewControlFlow_update_flow(
env,_tmp357,_tmp386);return _tmp387;}_LL262: if(_tmp34A <= (void*)1?1:*((int*)
_tmp34A)!= 8)goto _LL264;_tmp35B=((struct Cyc_Absyn_For_s_struct*)_tmp34A)->f1;
_tmp35C=((struct Cyc_Absyn_For_s_struct*)_tmp34A)->f2;_tmp35D=_tmp35C.f1;_tmp35E=
_tmp35C.f2;_tmp35F=((struct Cyc_Absyn_For_s_struct*)_tmp34A)->f3;_tmp360=_tmp35F.f1;
_tmp361=_tmp35F.f2;_tmp362=((struct Cyc_Absyn_For_s_struct*)_tmp34A)->f4;_LL263: {
void*_tmp388=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp35B)).f1;void*_tmp389=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp388,_tmp35E))->flow;void*
_tmp38B;void*_tmp38C;struct _tuple0 _tmp38A=Cyc_NewControlFlow_anal_test(env,
_tmp389,_tmp35D);_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;{void*_tmp38D=Cyc_NewControlFlow_anal_stmt(
env,_tmp38B,_tmp362);void*_tmp38E=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp38D,_tmp361))->flow;void*_tmp38F=(Cyc_NewControlFlow_anal_Rexp(env,_tmp38E,
_tmp360)).f1;Cyc_NewControlFlow_update_flow(env,_tmp35E,_tmp38F);return _tmp38C;}}
_LL264: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 5)goto _LL266;_tmp363=((struct
Cyc_Absyn_Break_s_struct*)_tmp34A)->f1;if(_tmp363 != 0)goto _LL266;_LL265: return(
void*)0;_LL266: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 11)goto _LL268;_tmp364=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp34A)->f1;_tmp365=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp34A)->f2;if(_tmp365 == 0)goto _LL268;_tmp366=*_tmp365;_LL267: {void*_tmp391;
struct Cyc_List_List*_tmp392;struct _tuple5 _tmp390=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp364);_tmp391=_tmp390.f1;_tmp392=_tmp390.f2;for(0;_tmp392 != 0;(
_tmp392=_tmp392->tl,_tmp364=_tmp364->tl)){_tmp391=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp364))->hd)->loc,
_tmp391,(void*)_tmp392->hd);}_tmp391=Cyc_NewControlFlow_add_vars(_tmp391,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp366->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp366->body,_tmp391);
return(void*)0;}_LL268: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 5)goto _LL26A;
_tmp367=((struct Cyc_Absyn_Break_s_struct*)_tmp34A)->f1;_LL269: _tmp368=_tmp367;
goto _LL26B;_LL26A: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 6)goto _LL26C;
_tmp368=((struct Cyc_Absyn_Continue_s_struct*)_tmp34A)->f1;_LL26B: _tmp369=_tmp368;
goto _LL26D;_LL26C: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 7)goto _LL26E;
_tmp369=((struct Cyc_Absyn_Goto_s_struct*)_tmp34A)->f2;_LL26D: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp393=_tmp349->encloser;struct Cyc_Absyn_Stmt*
_tmp394=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp369)))->encloser;while(_tmp394 != _tmp393){struct Cyc_Absyn_Stmt*_tmp395=(Cyc_NewControlFlow_get_stmt_annot(
_tmp393))->encloser;if(_tmp395 == _tmp393){({void*_tmp396[0]={};Cyc_Tcutil_terr(s->loc,
_tag_arr("goto enters local scope or exception handler",sizeof(char),45),
_tag_arr(_tmp396,sizeof(void*),0));});break;}_tmp393=_tmp395;}}Cyc_NewControlFlow_update_flow(
env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp369),inflow);return(void*)0;_LL26E:
if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 9)goto _LL270;_tmp36A=((struct Cyc_Absyn_Switch_s_struct*)
_tmp34A)->f1;_tmp36B=((struct Cyc_Absyn_Switch_s_struct*)_tmp34A)->f2;_LL26F: {
void*_tmp398;void*_tmp399;struct _tuple0 _tmp397=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp36A);_tmp398=_tmp397.f1;_tmp399=_tmp397.f2;_tmp398=Cyc_NewControlFlow_use_Rval(
env,_tmp36A->loc,_tmp398,_tmp399);return Cyc_NewControlFlow_anal_scs(env,_tmp398,
_tmp36B);}_LL270: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 17)goto _LL272;
_tmp36C=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp34A)->f1;_tmp36D=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp34A)->f2;_LL271: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp36C);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp36D);{
void*_tmp39A=scs_outflow;_LL27D: if((int)_tmp39A != 0)goto _LL27F;_LL27E: goto _LL27C;
_LL27F:;_LL280:({void*_tmp39B[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("last catch clause may implicitly fallthru",
sizeof(char),42),_tag_arr(_tmp39B,sizeof(void*),0));});_LL27C:;}return s1_outflow;}}}
_LL272: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 12)goto _LL274;_tmp36E=((struct
Cyc_Absyn_Decl_s_struct*)_tmp34A)->f1;_tmp36F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp34A)->f2;_LL273: return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp36E),_tmp36F);_LL274: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 
15)goto _LL276;_tmp370=((struct Cyc_Absyn_Label_s_struct*)_tmp34A)->f2;_LL275:
return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp370);_LL276: if(_tmp34A <= (void*)
1?1:*((int*)_tmp34A)!= 18)goto _LL278;_tmp371=((struct Cyc_Absyn_Region_s_struct*)
_tmp34A)->f1;_tmp372=((struct Cyc_Absyn_Region_s_struct*)_tmp34A)->f2;_tmp373=((
struct Cyc_Absyn_Region_s_struct*)_tmp34A)->f3;_tmp374=((struct Cyc_Absyn_Region_s_struct*)
_tmp34A)->f4;_LL277: {struct Cyc_List_List _tmp39C=({struct Cyc_List_List _tmp39D;
_tmp39D.hd=_tmp372;_tmp39D.tl=0;_tmp39D;});inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)& _tmp39C,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp374);}_LL278: if(_tmp34A <= (void*)1?1:*((int*)_tmp34A)!= 20)goto
_LL27A;_tmp375=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp34A)->f1;_LL279: {
void*_tmp39F;void*_tmp3A0;struct _tuple0 _tmp39E=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp375);_tmp39F=_tmp39E.f1;_tmp3A0=_tmp39E.f2;{void*_tmp3A1=Cyc_NewControlFlow_use_Rval(
env,_tmp375->loc,_tmp39F,_tmp3A0);void*_tmp3A2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp375->topt))->v);void*_tmp3A3;_LL282: if(
_tmp3A2 <= (void*)3?1:*((int*)_tmp3A2)!= 15)goto _LL284;_tmp3A3=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3A2)->f1;_LL283: return Cyc_CfFlowInfo_kill_flow_region(_tmp39F,_tmp3A3);
_LL284:;_LL285:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A4=
_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_Core_Impossible_struct _tmp3A5;
_tmp3A5.tag=Cyc_Core_Impossible;_tmp3A5.f1=_tag_arr("anal_stmt -- reset_region",
sizeof(char),26);_tmp3A5;});_tmp3A4;}));_LL281:;}}_LL27A:;_LL27B:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));
_tmp3A6[0]=({struct Cyc_Core_Impossible_struct _tmp3A7;_tmp3A7.tag=Cyc_Core_Impossible;
_tmp3A7.f1=_tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof(char),56);_tmp3A7;});_tmp3A6;}));_LL251:;}}static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Decl*decl){void*
_tmp3A8=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp3A9;struct Cyc_Core_Opt*
_tmp3AA;struct Cyc_Core_Opt _tmp3AB;struct Cyc_List_List*_tmp3AC;struct Cyc_Absyn_Exp*
_tmp3AD;struct Cyc_List_List*_tmp3AE;_LL287: if(*((int*)_tmp3A8)!= 0)goto _LL289;
_tmp3A9=((struct Cyc_Absyn_Var_d_struct*)_tmp3A8)->f1;_LL288: {struct Cyc_List_List
_tmp3AF=({struct Cyc_List_List _tmp3BD;_tmp3BD.hd=_tmp3A9;_tmp3BD.tl=0;_tmp3BD;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp3AF,Cyc_CfFlowInfo_unknown_none);{
struct Cyc_Absyn_Exp*_tmp3B0=_tmp3A9->initializer;if(_tmp3B0 == 0)return inflow;{
void*_tmp3B2;void*_tmp3B3;struct _tuple0 _tmp3B1=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp3B0));_tmp3B2=_tmp3B1.f1;_tmp3B3=
_tmp3B1.f2;{void*_tmp3B4=_tmp3B2;struct Cyc_Dict_Dict*_tmp3B5;struct Cyc_List_List*
_tmp3B6;_LL290: if((int)_tmp3B4 != 0)goto _LL292;_LL291: return(void*)0;_LL292: if(
_tmp3B4 <= (void*)1?1:*((int*)_tmp3B4)!= 0)goto _LL28F;_tmp3B5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3B4)->f1;_tmp3B6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3B4)->f2;
_LL293: _tmp3B5=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp3B5,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp3B8=_cycalloc(sizeof(*
_tmp3B8));_tmp3B8[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp3B9;_tmp3B9.tag=0;
_tmp3B9.f1=_tmp3A9;_tmp3B9;});_tmp3B8;}));_tmp3B7->fields=0;_tmp3B7;}),_tmp3B3);
_tmp3B6=Cyc_CfFlowInfo_reln_assign_var(_tmp3B6,_tmp3A9,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3A9->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3BA=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3BB=_cycalloc(sizeof(*
_tmp3BB));_tmp3BB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3BC;_tmp3BC.tag=
0;_tmp3BC.f1=_tmp3B5;_tmp3BC.f2=_tmp3B6;_tmp3BC;});_tmp3BB;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp3BA);return(void*)_tmp3BA;}_LL28F:;}}}}_LL289: if(*((int*)_tmp3A8)
!= 2)goto _LL28B;_tmp3AA=((struct Cyc_Absyn_Let_d_struct*)_tmp3A8)->f2;if(_tmp3AA
== 0)goto _LL28B;_tmp3AB=*_tmp3AA;_tmp3AC=(struct Cyc_List_List*)_tmp3AB.v;_tmp3AD=((
struct Cyc_Absyn_Let_d_struct*)_tmp3A8)->f3;_LL28A: {void*_tmp3BF;void*_tmp3C0;
struct _tuple0 _tmp3BE=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp3AD);_tmp3BF=
_tmp3BE.f1;_tmp3C0=_tmp3BE.f2;_tmp3BF=Cyc_NewControlFlow_use_Rval(env,_tmp3AD->loc,
_tmp3BF,_tmp3C0);return Cyc_NewControlFlow_add_vars(_tmp3BF,_tmp3AC,Cyc_CfFlowInfo_unknown_all);}
_LL28B: if(*((int*)_tmp3A8)!= 3)goto _LL28D;_tmp3AE=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3A8)->f1;_LL28C: return Cyc_NewControlFlow_add_vars(inflow,_tmp3AE,Cyc_CfFlowInfo_unknown_none);
_LL28D:;_LL28E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3C1=
_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({struct Cyc_Core_Impossible_struct _tmp3C2;
_tmp3C2.tag=Cyc_Core_Impossible;_tmp3C2.f1=_tag_arr("anal_decl: unexpected decl variant",
sizeof(char),35);_tmp3C2;});_tmp3C1;}));_LL286:;}static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt*s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{
void*_tmp3C3=(void*)s->r;struct _tuple3 _tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C5;
struct _tuple3 _tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;
struct Cyc_Absyn_Stmt*_tmp3C9;struct Cyc_Absyn_Stmt*_tmp3CA;struct _tuple3 _tmp3CB;
struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CD;struct Cyc_Absyn_Stmt*
_tmp3CE;struct _tuple3 _tmp3CF;struct Cyc_Absyn_Stmt*_tmp3D0;struct Cyc_Absyn_Stmt*
_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D2;struct Cyc_Absyn_Stmt*_tmp3D3;struct Cyc_Absyn_Stmt*
_tmp3D4;struct Cyc_Absyn_Stmt*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D6;struct Cyc_List_List*
_tmp3D7;struct Cyc_List_List*_tmp3D8;_LL295: if(_tmp3C3 <= (void*)1?1:*((int*)
_tmp3C3)!= 8)goto _LL297;_tmp3C4=((struct Cyc_Absyn_For_s_struct*)_tmp3C3)->f2;
_tmp3C5=_tmp3C4.f2;_tmp3C6=((struct Cyc_Absyn_For_s_struct*)_tmp3C3)->f3;_tmp3C7=
_tmp3C6.f2;_tmp3C8=((struct Cyc_Absyn_For_s_struct*)_tmp3C3)->f4;_LL296: Cyc_NewControlFlow_postpass(
_tmp3C7);_tmp3C9=_tmp3C8;_tmp3CA=_tmp3C5;goto _LL298;_LL297: if(_tmp3C3 <= (void*)1?
1:*((int*)_tmp3C3)!= 1)goto _LL299;_tmp3C9=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3C3)->f1;_tmp3CA=((struct Cyc_Absyn_Seq_s_struct*)_tmp3C3)->f2;_LL298: _tmp3CC=
_tmp3C9;_tmp3CD=_tmp3CA;goto _LL29A;_LL299: if(_tmp3C3 <= (void*)1?1:*((int*)
_tmp3C3)!= 4)goto _LL29B;_tmp3CB=((struct Cyc_Absyn_While_s_struct*)_tmp3C3)->f1;
_tmp3CC=_tmp3CB.f2;_tmp3CD=((struct Cyc_Absyn_While_s_struct*)_tmp3C3)->f2;_LL29A:
_tmp3CE=_tmp3CD;_tmp3D0=_tmp3CC;goto _LL29C;_LL29B: if(_tmp3C3 <= (void*)1?1:*((int*)
_tmp3C3)!= 16)goto _LL29D;_tmp3CE=((struct Cyc_Absyn_Do_s_struct*)_tmp3C3)->f1;
_tmp3CF=((struct Cyc_Absyn_Do_s_struct*)_tmp3C3)->f2;_tmp3D0=_tmp3CF.f2;_LL29C:
_tmp3D1=_tmp3CE;_tmp3D2=_tmp3D0;goto _LL29E;_LL29D: if(_tmp3C3 <= (void*)1?1:*((int*)
_tmp3C3)!= 3)goto _LL29F;_tmp3D1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3C3)->f2;
_tmp3D2=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3C3)->f3;_LL29E: Cyc_NewControlFlow_postpass(
_tmp3D1);_tmp3D3=_tmp3D2;goto _LL2A0;_LL29F: if(_tmp3C3 <= (void*)1?1:*((int*)
_tmp3C3)!= 18)goto _LL2A1;_tmp3D3=((struct Cyc_Absyn_Region_s_struct*)_tmp3C3)->f4;
_LL2A0: _tmp3D4=_tmp3D3;goto _LL2A2;_LL2A1: if(_tmp3C3 <= (void*)1?1:*((int*)_tmp3C3)
!= 15)goto _LL2A3;_tmp3D4=((struct Cyc_Absyn_Label_s_struct*)_tmp3C3)->f2;_LL2A2:
_tmp3D5=_tmp3D4;goto _LL2A4;_LL2A3: if(_tmp3C3 <= (void*)1?1:*((int*)_tmp3C3)!= 12)
goto _LL2A5;_tmp3D5=((struct Cyc_Absyn_Decl_s_struct*)_tmp3C3)->f2;_LL2A4: Cyc_NewControlFlow_postpass(
_tmp3D5);return;_LL2A5: if(_tmp3C3 <= (void*)1?1:*((int*)_tmp3C3)!= 17)goto _LL2A7;
_tmp3D6=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3C3)->f1;_tmp3D7=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3C3)->f2;_LL2A6: Cyc_NewControlFlow_postpass(_tmp3D6);_tmp3D8=_tmp3D7;goto
_LL2A8;_LL2A7: if(_tmp3C3 <= (void*)1?1:*((int*)_tmp3C3)!= 9)goto _LL2A9;_tmp3D8=((
struct Cyc_Absyn_Switch_s_struct*)_tmp3C3)->f2;_LL2A8: for(0;_tmp3D8 != 0;_tmp3D8=
_tmp3D8->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp3D8->hd)->body);}return;_LL2A9:;_LL2AA: return;_LL294:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct Cyc_Position_Segment*_tmp3D9=(fd->body)->loc;
void*inflow=Cyc_NewControlFlow_add_vars((void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp402;_tmp402.tag=0;_tmp402.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp402.f2=
0;_tmp402;});_tmp401;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all);struct Cyc_List_List*_tmp3DA=
0;{void*_tmp3DB=inflow;struct Cyc_Dict_Dict*_tmp3DC;struct Cyc_List_List*_tmp3DD;
_LL2AC: if((int)_tmp3DB != 0)goto _LL2AE;_LL2AD:({void*_tmp3DE[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fun",
sizeof(char),10),_tag_arr(_tmp3DE,sizeof(void*),0));});_LL2AE: if(_tmp3DB <= (void*)
1?1:*((int*)_tmp3DB)!= 0)goto _LL2AB;_tmp3DC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3DB)->f1;_tmp3DD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3DB)->f2;
_LL2AF: {struct Cyc_List_List*atts;{void*_tmp3DF=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp3E0;struct Cyc_List_List*_tmp3E1;_LL2B1: if(_tmp3DF <= (void*)3?1:*((int*)
_tmp3DF)!= 8)goto _LL2B3;_tmp3E0=((struct Cyc_Absyn_FnType_struct*)_tmp3DF)->f1;
_tmp3E1=_tmp3E0.attributes;_LL2B2: atts=_tmp3E1;goto _LL2B0;_LL2B3:;_LL2B4:({void*
_tmp3E2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("check_fun: non-function type cached with fndecl_t",sizeof(char),50),
_tag_arr(_tmp3E2,sizeof(void*),0));});_LL2B0:;}for(0;atts != 0;atts=atts->tl){
void*_tmp3E3=(void*)atts->hd;int _tmp3E4;_LL2B6: if(_tmp3E3 <= (void*)16?1:*((int*)
_tmp3E3)!= 4)goto _LL2B8;_tmp3E4=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp3E3)->f1;_LL2B7: {unsigned int j=(unsigned int)_tmp3E4;if(j > ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){({void*_tmp3E5[0]={};Cyc_Tcutil_terr(
_tmp3D9,_tag_arr("initializes attribute exceeds number of parameters",sizeof(
char),51),_tag_arr(_tmp3E5,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp3E6=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp3E7=Cyc_Tcutil_compress((void*)_tmp3E6->type);struct Cyc_Absyn_PtrInfo
_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Conref*_tmp3EA;struct Cyc_Absyn_Conref*
_tmp3EB;_LL2BB: if(_tmp3E7 <= (void*)3?1:*((int*)_tmp3E7)!= 4)goto _LL2BD;_tmp3E8=((
struct Cyc_Absyn_PointerType_struct*)_tmp3E7)->f1;_tmp3E9=(void*)_tmp3E8.elt_typ;
_tmp3EA=_tmp3E8.nullable;_tmp3EB=_tmp3E8.bounds;_LL2BC: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp3EA))({void*_tmp3EC[0]={};Cyc_Tcutil_terr(_tmp3D9,
_tag_arr("initializes attribute not allowed on nullable pointers",sizeof(char),
55),_tag_arr(_tmp3EC,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp3EB))({
void*_tmp3ED[0]={};Cyc_Tcutil_terr(_tmp3D9,_tag_arr("initializes attribute allowed only on pointers of size 1",
sizeof(char),57),_tag_arr(_tmp3ED,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp3EE=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp3F6=_cycalloc(sizeof(*
_tmp3F6));_tmp3F6[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp3F7;_tmp3F7.tag=
2;_tmp3F7.f1=(int)j;_tmp3F7.f2=(void*)_tmp3E9;_tmp3F7;});_tmp3F6;});struct Cyc_CfFlowInfo_Place*
_tmp3EF=({struct Cyc_CfFlowInfo_Place*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->root=(
void*)((void*)_tmp3EE);_tmp3F5->fields=0;_tmp3F5;});_tmp3DC=Cyc_Dict_insert(
_tmp3DC,(void*)_tmp3EE,Cyc_CfFlowInfo_typ_to_absrval(_tmp3E9,Cyc_CfFlowInfo_esc_none));
_tmp3DC=Cyc_Dict_insert(_tmp3DC,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp3F1;_tmp3F1.tag=0;_tmp3F1.f1=_tmp3E6;_tmp3F1;});_tmp3F0;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp3F3;_tmp3F3.tag=2;_tmp3F3.f1=_tmp3EF;
_tmp3F3;});_tmp3F2;}));_tmp3DA=({struct Cyc_List_List*_tmp3F4=_cycalloc(sizeof(*
_tmp3F4));_tmp3F4->hd=_tmp3EF;_tmp3F4->tl=_tmp3DA;_tmp3F4;});goto _LL2BA;}_LL2BD:;
_LL2BE:({void*_tmp3F8[0]={};Cyc_Tcutil_terr(_tmp3D9,_tag_arr("initializes attribute on non-pointer",
sizeof(char),37),_tag_arr(_tmp3F8,sizeof(void*),0));});_LL2BA:;}goto _LL2B5;}}
_LL2B8:;_LL2B9: goto _LL2B5;_LL2B5:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3FA;_tmp3FA.tag=0;_tmp3FA.f1=_tmp3DC;_tmp3FA.f2=_tmp3DD;_tmp3FA;});_tmp3F9;});}
_LL2AB:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct
Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp400=
_cycalloc(sizeof(*_tmp400));_tmp400->iterate_again=1;_tmp400->iteration_num=0;
_tmp400->in_try=0;_tmp400->tryflow=(void*)inflow;_tmp400->all_changed=0;_tmp400->noreturn=
noreturn;_tmp400->param_roots=_tmp3DA;_tmp400;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp3FB=outflow;_LL2C0: if((int)_tmp3FB != 0)goto
_LL2C2;_LL2C1: goto _LL2BF;_LL2C2:;_LL2C3: Cyc_NewControlFlow_check_init_params(
_tmp3D9,env,outflow);if(noreturn)({void*_tmp3FC[0]={};Cyc_Tcutil_terr(_tmp3D9,
_tag_arr("`noreturn' function might (implicitly) return",sizeof(char),46),
_tag_arr(_tmp3FC,sizeof(void*),0));});else{void*_tmp3FD=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL2C5: if((int)_tmp3FD != 0)goto _LL2C7;_LL2C6: goto _LL2C4;
_LL2C7: if(_tmp3FD <= (void*)3?1:*((int*)_tmp3FD)!= 6)goto _LL2C9;_LL2C8: goto _LL2CA;
_LL2C9: if((int)_tmp3FD != 1)goto _LL2CB;_LL2CA: goto _LL2CC;_LL2CB: if(_tmp3FD <= (
void*)3?1:*((int*)_tmp3FD)!= 5)goto _LL2CD;_LL2CC:({void*_tmp3FE[0]={};Cyc_Tcutil_warn(
_tmp3D9,_tag_arr("function may not return a value",sizeof(char),32),_tag_arr(
_tmp3FE,sizeof(void*),0));});goto _LL2C4;_LL2CD:;_LL2CE:({void*_tmp3FF[0]={};Cyc_Tcutil_terr(
_tmp3D9,_tag_arr("function may not return a value",sizeof(char),32),_tag_arr(
_tmp3FF,sizeof(void*),0));});goto _LL2C4;_LL2C4:;}goto _LL2BF;_LL2BF:;}Cyc_NewControlFlow_postpass(
fd->body);}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp403=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp404;struct Cyc_List_List*_tmp405;struct Cyc_List_List*_tmp406;struct Cyc_List_List*
_tmp407;_LL2D0: if(*((int*)_tmp403)!= 1)goto _LL2D2;_tmp404=((struct Cyc_Absyn_Fn_d_struct*)
_tmp403)->f1;_LL2D1: Cyc_NewControlFlow_check_fun(_tmp404);goto _LL2CF;_LL2D2: if(*((
int*)_tmp403)!= 10)goto _LL2D4;_tmp405=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp403)->f1;_LL2D3: _tmp406=_tmp405;goto _LL2D5;_LL2D4: if(*((int*)_tmp403)!= 9)
goto _LL2D6;_tmp406=((struct Cyc_Absyn_Using_d_struct*)_tmp403)->f2;_LL2D5: _tmp407=
_tmp406;goto _LL2D7;_LL2D6: if(*((int*)_tmp403)!= 8)goto _LL2D8;_tmp407=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp403)->f2;_LL2D7: Cyc_NewControlFlow_cf_check(_tmp407);goto _LL2CF;_LL2D8:;
_LL2D9: goto _LL2CF;_LL2CF:;}}
