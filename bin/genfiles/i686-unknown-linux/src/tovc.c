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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_rempty(
struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*
v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;void*
f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Dict_Dict*Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict*,void*);int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct
Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
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
15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct _tuple1*Cyc_Absyn_exn_name;
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*
elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter Cyc_Set_make_iter(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_Tcenv_VarRes_struct{int tag;void*
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
le;};void*Cyc_Tcutil_compress(void*t);struct _tuple1*Cyc_Toc_temp_var();struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_Tovc_BoxingEnv{struct
_RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict*varmap;
struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};struct Cyc_Tovc_ToExpEnv{
struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict*
varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};struct _tuple4{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Tovc_box_free_vars_exp(
struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){while(1){void*_tmp0=(void*)e->r;
struct _tuple1*_tmp1;struct _tuple1 _tmp2;void*_tmp3;struct _tagged_arr*_tmp4;struct
Cyc_Absyn_Exp*_tmp5;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp7;struct
Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct
Cyc_Absyn_Exp*_tmpB;struct Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Exp*_tmpD;struct
Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmp10;struct
Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp13;
struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp16;
struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp19;
struct Cyc_List_List*_tmp1A;_LL1: if(*((int*)_tmp0)!= 1)goto _LL3;_tmp1=((struct Cyc_Absyn_Var_e_struct*)
_tmp0)->f1;_tmp2=*_tmp1;_tmp3=_tmp2.f1;if((int)_tmp3 != 0)goto _LL3;_tmp4=_tmp2.f2;
_LL2: if(((int(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(env.varmap,
_tmp4)){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(env.varmap,_tmp4))->r));continue;}for(0;
env.boundvars != 0;env.boundvars=(env.boundvars)->tl){if(Cyc_Std_zstrptrcmp(_tmp4,(*((
struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)return;}for(0;env.all_locals
!= 0;env.all_locals=(env.all_locals)->tl){struct Cyc_Absyn_Vardecl*_tmp1B=(struct
Cyc_Absyn_Vardecl*)(env.all_locals)->hd;if(Cyc_Std_zstrptrcmp(_tmp4,(*_tmp1B->name).f2)
== 0){{void*_tmp1C=(void*)_tmp1B->type;_LL32: if(_tmp1C <= (void*)3?1:*((int*)
_tmp1C)!= 7)goto _LL34;_LL33: goto _LL31;_LL34:;_LL35:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp1B->name,0),0))->r));goto _LL31;_LL31:;}*
env.freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,
struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp1B);
return;}}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1D=_cycalloc(
sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Core_Impossible_struct _tmp1E;_tmp1E.tag=
Cyc_Core_Impossible;_tmp1E.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp20;_tmp20.tag=0;_tmp20.f1=(struct _tagged_arr)*_tmp4;{void*_tmp1F[1]={& _tmp20};
Cyc_Std_aprintf(_tag_arr("unbound variable %s found in box_free_vars_exp",
sizeof(char),47),_tag_arr(_tmp1F,sizeof(void*),1));}});_tmp1E;});_tmp1D;}));_LL3:
if(*((int*)_tmp0)!= 1)goto _LL5;_LL4: return;_LL5: if(*((int*)_tmp0)!= 0)goto _LL7;
_LL6: return;_LL7: if(*((int*)_tmp0)!= 9)goto _LL9;_tmp5=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp0)->f1;_tmp6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp0)->f2;_LL8: Cyc_Tovc_box_free_vars_exp(
env,_tmp5);_tmp7=_tmp6;goto _LLA;_LL9: if(*((int*)_tmp0)!= 3)goto _LLB;_tmp7=((
struct Cyc_Absyn_Primop_e_struct*)_tmp0)->f2;_LLA: for(0;_tmp7 != 0;_tmp7=_tmp7->tl){
Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp7->hd);}return;_LLB: if(*((
int*)_tmp0)!= 6)goto _LLD;_tmp8=((struct Cyc_Absyn_Conditional_e_struct*)_tmp0)->f1;
_tmp9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp0)->f2;_tmpA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp0)->f3;_LLC: Cyc_Tovc_box_free_vars_exp(env,_tmpA);_tmpB=_tmp8;_tmpC=_tmp9;
goto _LLE;_LLD: if(*((int*)_tmp0)!= 23)goto _LLF;_tmpB=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp0)->f1;_tmpC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp0)->f2;_LLE: _tmpD=
_tmpB;_tmpE=_tmpC;goto _LL10;_LLF: if(*((int*)_tmp0)!= 7)goto _LL11;_tmpD=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp0)->f1;_tmpE=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp0)->f2;_LL10: _tmpF=_tmpD;_tmp10=_tmpE;goto _LL12;_LL11: if(*((int*)_tmp0)!= 4)
goto _LL13;_tmpF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp0)->f1;_tmp10=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmp0)->f3;_LL12: Cyc_Tovc_box_free_vars_exp(env,
_tmp10);_tmp11=_tmpF;goto _LL14;_LL13: if(*((int*)_tmp0)!= 11)goto _LL15;_tmp11=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp0)->f1;_LL14: _tmp12=_tmp11;goto _LL16;
_LL15: if(*((int*)_tmp0)!= 12)goto _LL17;_tmp12=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp0)->f1;_LL16: _tmp13=_tmp12;goto _LL18;_LL17: if(*((int*)_tmp0)!= 13)goto _LL19;
_tmp13=((struct Cyc_Absyn_Cast_e_struct*)_tmp0)->f2;_LL18: _tmp14=_tmp13;goto _LL1A;
_LL19: if(*((int*)_tmp0)!= 14)goto _LL1B;_tmp14=((struct Cyc_Absyn_Address_e_struct*)
_tmp0)->f1;_LL1A: _tmp15=_tmp14;goto _LL1C;_LL1B: if(*((int*)_tmp0)!= 17)goto _LL1D;
_tmp15=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp0)->f1;_LL1C: _tmp16=_tmp15;goto
_LL1E;_LL1D: if(*((int*)_tmp0)!= 20)goto _LL1F;_tmp16=((struct Cyc_Absyn_Deref_e_struct*)
_tmp0)->f1;_LL1E: _tmp17=_tmp16;goto _LL20;_LL1F: if(*((int*)_tmp0)!= 21)goto _LL21;
_tmp17=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp0)->f1;_LL20: _tmp18=_tmp17;
goto _LL22;_LL21: if(*((int*)_tmp0)!= 22)goto _LL23;_tmp18=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp0)->f1;_LL22: _tmp19=_tmp18;goto _LL24;_LL23: if(*((int*)_tmp0)!= 5)goto _LL25;
_tmp19=((struct Cyc_Absyn_Increment_e_struct*)_tmp0)->f1;_LL24: Cyc_Tovc_box_free_vars_exp(
env,_tmp19);return;_LL25: if(*((int*)_tmp0)!= 16)goto _LL27;_LL26: goto _LL28;_LL27:
if(*((int*)_tmp0)!= 18)goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)_tmp0)!= 19)
goto _LL2B;_LL2A: goto _LL2C;_LL2B: if(*((int*)_tmp0)!= 31)goto _LL2D;_LL2C: return;
_LL2D: if(*((int*)_tmp0)!= 34)goto _LL2F;_tmp1A=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp0)->f2;_LL2E: for(0;_tmp1A != 0;_tmp1A=_tmp1A->tl){Cyc_Tovc_box_free_vars_exp(
env,(*((struct _tuple4*)_tmp1A->hd)).f2);}return;_LL2F:;_LL30:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({
struct Cyc_Core_Impossible_struct _tmp22;_tmp22.tag=Cyc_Core_Impossible;_tmp22.f1=
_tag_arr("bad exp form in Tocv::box_free_vars_exp",sizeof(char),40);_tmp22;});
_tmp21;}));_LL0:;}}static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv
env,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp23=(void*)s->r;struct Cyc_Absyn_Stmt*
_tmp24;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Stmt*
_tmp27;struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Stmt*
_tmp2A;struct Cyc_Absyn_Stmt*_tmp2B;struct _tuple3 _tmp2C;struct Cyc_Absyn_Exp*
_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;struct Cyc_Absyn_Exp*_tmp2F;struct _tuple3
_tmp30;struct Cyc_Absyn_Exp*_tmp31;struct _tuple3 _tmp32;struct Cyc_Absyn_Exp*_tmp33;
struct Cyc_Absyn_Stmt*_tmp34;struct Cyc_Absyn_Stmt*_tmp35;struct _tuple3 _tmp36;
struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_List_List*_tmp39;
struct Cyc_Absyn_Decl*_tmp3A;struct Cyc_Absyn_Stmt*_tmp3B;_LL37: if((int)_tmp23 != 0)
goto _LL39;_LL38: goto _LL3A;_LL39: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 5)goto
_LL3B;_LL3A: goto _LL3C;_LL3B: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 6)goto _LL3D;
_LL3C: return;_LL3D: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 7)goto _LL3F;_LL3E:
return;_LL3F: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 15)goto _LL41;_tmp24=((
struct Cyc_Absyn_Label_s_struct*)_tmp23)->f2;_LL40: s=_tmp24;continue;_LL41: if(
_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 2)goto _LL43;_tmp25=((struct Cyc_Absyn_Return_s_struct*)
_tmp23)->f1;_LL42: if(_tmp25 == 0)return;_tmp26=(struct Cyc_Absyn_Exp*)_check_null(
_tmp25);goto _LL44;_LL43: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 0)goto _LL45;
_tmp26=((struct Cyc_Absyn_Exp_s_struct*)_tmp23)->f1;_LL44: Cyc_Tovc_box_free_vars_exp(
env,_tmp26);return;_LL45: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 1)goto _LL47;
_tmp27=((struct Cyc_Absyn_Seq_s_struct*)_tmp23)->f1;_tmp28=((struct Cyc_Absyn_Seq_s_struct*)
_tmp23)->f2;_LL46: Cyc_Tovc_box_free_vars_stmt(env,_tmp27);s=_tmp28;continue;
_LL47: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 3)goto _LL49;_tmp29=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp23)->f1;_tmp2A=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp23)->f2;_tmp2B=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp23)->f3;_LL48: Cyc_Tovc_box_free_vars_exp(
env,_tmp29);Cyc_Tovc_box_free_vars_stmt(env,_tmp2A);s=_tmp2B;continue;_LL49: if(
_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 4)goto _LL4B;_tmp2C=((struct Cyc_Absyn_While_s_struct*)
_tmp23)->f1;_tmp2D=_tmp2C.f1;_tmp2E=((struct Cyc_Absyn_While_s_struct*)_tmp23)->f2;
_LL4A: Cyc_Tovc_box_free_vars_exp(env,_tmp2D);s=_tmp2E;continue;_LL4B: if(_tmp23 <= (
void*)1?1:*((int*)_tmp23)!= 8)goto _LL4D;_tmp2F=((struct Cyc_Absyn_For_s_struct*)
_tmp23)->f1;_tmp30=((struct Cyc_Absyn_For_s_struct*)_tmp23)->f2;_tmp31=_tmp30.f1;
_tmp32=((struct Cyc_Absyn_For_s_struct*)_tmp23)->f3;_tmp33=_tmp32.f1;_tmp34=((
struct Cyc_Absyn_For_s_struct*)_tmp23)->f4;_LL4C: Cyc_Tovc_box_free_vars_exp(env,
_tmp2F);Cyc_Tovc_box_free_vars_exp(env,_tmp31);Cyc_Tovc_box_free_vars_exp(env,
_tmp33);s=_tmp34;continue;_LL4D: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 16)goto
_LL4F;_tmp35=((struct Cyc_Absyn_Do_s_struct*)_tmp23)->f1;_tmp36=((struct Cyc_Absyn_Do_s_struct*)
_tmp23)->f2;_tmp37=_tmp36.f1;_LL4E: Cyc_Tovc_box_free_vars_exp(env,_tmp37);s=
_tmp35;continue;_LL4F: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 9)goto _LL51;
_tmp38=((struct Cyc_Absyn_Switch_s_struct*)_tmp23)->f1;_tmp39=((struct Cyc_Absyn_Switch_s_struct*)
_tmp23)->f2;_LL50: Cyc_Tovc_box_free_vars_exp(env,_tmp38);for(0;_tmp39 != 0;_tmp39=
_tmp39->tl){Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)
_tmp39->hd)->body);}return;_LL51: if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 12)
goto _LL53;_tmp3A=((struct Cyc_Absyn_Decl_s_struct*)_tmp23)->f1;_tmp3B=((struct Cyc_Absyn_Decl_s_struct*)
_tmp23)->f2;_LL52:{void*_tmp3C=(void*)_tmp3A->r;struct Cyc_Absyn_Vardecl*_tmp3D;
_LL56: if(*((int*)_tmp3C)!= 0)goto _LL58;_tmp3D=((struct Cyc_Absyn_Var_d_struct*)
_tmp3C)->f1;_LL57: env.boundvars=({struct Cyc_List_List*_tmp3E=_region_malloc(env.rgn,
sizeof(*_tmp3E));_tmp3E->hd=_tmp3D;_tmp3E->tl=env.boundvars;_tmp3E;});env.varmap=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct _tagged_arr*))Cyc_Dict_rdelete_same)(
env.varmap,(*_tmp3D->name).f2);if(_tmp3D->initializer != 0)Cyc_Tovc_box_free_vars_exp(
env,(struct Cyc_Absyn_Exp*)_check_null(_tmp3D->initializer));s=_tmp3B;continue;
_LL58:;_LL59: goto _LL55;_LL55:;}goto _LL54;_LL53:;_LL54:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({
struct Cyc_Core_Impossible_struct _tmp40;_tmp40.tag=Cyc_Core_Impossible;_tmp40.f1=
_tag_arr("bad stmt after xlation to C",sizeof(char),28);_tmp40;});_tmp3F;}));
_LL36:;}}struct _tuple5{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct
Cyc_Absyn_Stmt*s,void*rettype){struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))
Cyc_Set_rempty)(env.rgn,(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp);struct Cyc_Tovc_BoxingEnv _tmp41=({struct Cyc_Tovc_BoxingEnv _tmp51;
_tmp51.rgn=env.rgn;_tmp51.all_locals=env.all_locals;_tmp51.varmap=env.varmap;
_tmp51.boundvars=0;_tmp51.freevars=& freevars;_tmp51;});Cyc_Tovc_box_free_vars_stmt(
_tmp41,s);{struct Cyc_List_List*params=0;struct Cyc_List_List*args=0;struct Cyc_Iter_Iter
_tmp42=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
Cyc_Core_heap_region,freevars);struct Cyc_Absyn_Vardecl*_tmp43=({struct Cyc_Absyn_Vardecl*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->sc=(void*)((void*)2);_tmp50->name=Cyc_Absyn_exn_name;
_tmp50->tq=Cyc_Absyn_empty_tqual();_tmp50->type=(void*)((void*)0);_tmp50->initializer=
0;_tmp50->rgn=0;_tmp50->attributes=0;_tmp50->escapes=0;_tmp50;});while(((int(*)(
struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(_tmp42,& _tmp43)){
void*_tmp44=(void*)_tmp43->type;void*_tmp45;_LL5B: if(_tmp44 <= (void*)3?1:*((int*)
_tmp44)!= 7)goto _LL5D;_tmp45=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp44)->f1;
_LL5C: params=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=({
struct _tuple5*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=(*_tmp43->name).f2;
_tmp47->f2=_tmp43->tq;_tmp47->f3=Cyc_Absyn_cstar_typ(_tmp45,Cyc_Absyn_empty_tqual());
_tmp47;});_tmp46->tl=params;_tmp46;});args=({struct Cyc_List_List*_tmp48=
_cycalloc(sizeof(*_tmp48));_tmp48->hd=Cyc_Absyn_var_exp(_tmp43->name,0);_tmp48->tl=
args;_tmp48;});goto _LL5A;_LL5D:;_LL5E: params=({struct Cyc_List_List*_tmp49=
_cycalloc(sizeof(*_tmp49));_tmp49->hd=({struct _tuple5*_tmp4A=_cycalloc(sizeof(*
_tmp4A));_tmp4A->f1=(*_tmp43->name).f2;_tmp4A->f2=_tmp43->tq;_tmp4A->f3=Cyc_Absyn_cstar_typ((
void*)_tmp43->type,Cyc_Absyn_empty_tqual());_tmp4A;});_tmp49->tl=params;_tmp49;});
args=({struct Cyc_List_List*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->hd=Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(_tmp43->name,0),0);_tmp4B->tl=args;_tmp4B;});goto _LL5A;_LL5A:;}{
struct _tuple1*funname=Cyc_Toc_temp_var();*env.gen_ds=({struct Cyc_List_List*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp4E;_tmp4E.tag=1;_tmp4E.f1=({struct Cyc_Absyn_Fndecl*_tmp4F=_cycalloc(sizeof(*
_tmp4F));_tmp4F->sc=(void*)((void*)0);_tmp4F->is_inline=1;_tmp4F->name=funname;
_tmp4F->tvs=0;_tmp4F->effect=0;_tmp4F->ret_type=(void*)rettype;_tmp4F->args=
params;_tmp4F->c_varargs=0;_tmp4F->cyc_varargs=0;_tmp4F->rgn_po=0;_tmp4F->body=s;
_tmp4F->cached_typ=0;_tmp4F->param_vardecls=0;_tmp4F->attributes=0;_tmp4F;});
_tmp4E;});_tmp4D;}),0);_tmp4C->tl=*env.gen_ds;_tmp4C;});return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(funname,0),args,0);}}}static void Cyc_Tovc_apply_varmap(struct
Cyc_Dict_Dict*varmap,struct Cyc_Absyn_Exp*e){void*_tmp52=(void*)e->r;struct
_tuple1*_tmp53;struct _tuple1 _tmp54;void*_tmp55;struct _tagged_arr*_tmp56;struct
Cyc_Absyn_Exp*_tmp57;struct Cyc_List_List*_tmp58;struct Cyc_List_List*_tmp59;
struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Exp*_tmp5C;
struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_Absyn_Exp*_tmp5F;
struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_Absyn_Exp*_tmp62;
struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp65;
struct Cyc_Absyn_Exp*_tmp66;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp68;
struct Cyc_Absyn_Exp*_tmp69;struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Exp*_tmp6B;
struct Cyc_List_List*_tmp6C;_LL60: if(*((int*)_tmp52)!= 1)goto _LL62;_tmp53=((
struct Cyc_Absyn_Var_e_struct*)_tmp52)->f1;_tmp54=*_tmp53;_tmp55=_tmp54.f1;if((
int)_tmp55 != 0)goto _LL62;_tmp56=_tmp54.f2;_LL61: if(((int(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_member)(varmap,_tmp56))(void*)(e->r=(void*)((
void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
varmap,_tmp56))->r));return;_LL62: if(*((int*)_tmp52)!= 1)goto _LL64;_LL63: return;
_LL64: if(*((int*)_tmp52)!= 0)goto _LL66;_LL65: return;_LL66: if(*((int*)_tmp52)!= 9)
goto _LL68;_tmp57=((struct Cyc_Absyn_FnCall_e_struct*)_tmp52)->f1;_tmp58=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp52)->f2;_LL67: Cyc_Tovc_apply_varmap(varmap,_tmp57);
_tmp59=_tmp58;goto _LL69;_LL68: if(*((int*)_tmp52)!= 3)goto _LL6A;_tmp59=((struct
Cyc_Absyn_Primop_e_struct*)_tmp52)->f2;_LL69: for(0;_tmp59 != 0;_tmp59=_tmp59->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp59->hd);}return;_LL6A: if(*((
int*)_tmp52)!= 6)goto _LL6C;_tmp5A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp52)->f1;
_tmp5B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp52)->f2;_tmp5C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp52)->f3;_LL6B: Cyc_Tovc_apply_varmap(varmap,_tmp5C);_tmp5D=_tmp5A;_tmp5E=
_tmp5B;goto _LL6D;_LL6C: if(*((int*)_tmp52)!= 23)goto _LL6E;_tmp5D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp52)->f1;_tmp5E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp52)->f2;_LL6D:
_tmp5F=_tmp5D;_tmp60=_tmp5E;goto _LL6F;_LL6E: if(*((int*)_tmp52)!= 7)goto _LL70;
_tmp5F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp52)->f1;_tmp60=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp52)->f2;_LL6F: _tmp61=_tmp5F;_tmp62=_tmp60;goto _LL71;_LL70: if(*((int*)_tmp52)
!= 4)goto _LL72;_tmp61=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp52)->f1;_tmp62=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp52)->f3;_LL71: Cyc_Tovc_apply_varmap(
varmap,_tmp62);_tmp63=_tmp61;goto _LL73;_LL72: if(*((int*)_tmp52)!= 11)goto _LL74;
_tmp63=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp52)->f1;_LL73: _tmp64=_tmp63;
goto _LL75;_LL74: if(*((int*)_tmp52)!= 12)goto _LL76;_tmp64=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp52)->f1;_LL75: _tmp65=_tmp64;goto _LL77;_LL76: if(*((int*)_tmp52)!= 13)goto
_LL78;_tmp65=((struct Cyc_Absyn_Cast_e_struct*)_tmp52)->f2;_LL77: _tmp66=_tmp65;
goto _LL79;_LL78: if(*((int*)_tmp52)!= 14)goto _LL7A;_tmp66=((struct Cyc_Absyn_Address_e_struct*)
_tmp52)->f1;_LL79: _tmp67=_tmp66;goto _LL7B;_LL7A: if(*((int*)_tmp52)!= 17)goto
_LL7C;_tmp67=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp52)->f1;_LL7B: _tmp68=
_tmp67;goto _LL7D;_LL7C: if(*((int*)_tmp52)!= 20)goto _LL7E;_tmp68=((struct Cyc_Absyn_Deref_e_struct*)
_tmp52)->f1;_LL7D: _tmp69=_tmp68;goto _LL7F;_LL7E: if(*((int*)_tmp52)!= 21)goto
_LL80;_tmp69=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp52)->f1;_LL7F: _tmp6A=
_tmp69;goto _LL81;_LL80: if(*((int*)_tmp52)!= 22)goto _LL82;_tmp6A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp52)->f1;_LL81: _tmp6B=_tmp6A;goto _LL83;_LL82: if(*((int*)_tmp52)!= 5)goto _LL84;
_tmp6B=((struct Cyc_Absyn_Increment_e_struct*)_tmp52)->f1;_LL83: Cyc_Tovc_apply_varmap(
varmap,_tmp6B);return;_LL84: if(*((int*)_tmp52)!= 16)goto _LL86;_LL85: goto _LL87;
_LL86: if(*((int*)_tmp52)!= 18)goto _LL88;_LL87: goto _LL89;_LL88: if(*((int*)_tmp52)
!= 19)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)_tmp52)!= 31)goto _LL8C;_LL8B:
return;_LL8C: if(*((int*)_tmp52)!= 34)goto _LL8E;_tmp6C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp52)->f2;_LL8D: for(0;_tmp6C != 0;_tmp6C=_tmp6C->tl){Cyc_Tovc_apply_varmap(
varmap,(*((struct _tuple4*)_tmp6C->hd)).f2);}return;_LL8E:;_LL8F:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));
_tmp6D[0]=({struct Cyc_Core_Impossible_struct _tmp6E;_tmp6E.tag=Cyc_Core_Impossible;
_tmp6E.f1=_tag_arr("bad exp form in Tocv::apply_varmap",sizeof(char),35);_tmp6E;});
_tmp6D;}));_LL5F:;}static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv
env,struct Cyc_Absyn_Stmt*s){void*_tmp6F=(void*)s->r;struct Cyc_Absyn_Exp*_tmp70;
struct Cyc_Absyn_Stmt*_tmp71;struct Cyc_Absyn_Stmt*_tmp72;struct Cyc_Absyn_Exp*
_tmp73;struct Cyc_Absyn_Stmt*_tmp74;struct Cyc_Absyn_Stmt*_tmp75;struct Cyc_Absyn_Decl*
_tmp76;struct Cyc_Absyn_Stmt*_tmp77;_LL91: if((int)_tmp6F != 0)goto _LL93;_LL92:
return Cyc_Absyn_true_exp(0);_LL93: if(_tmp6F <= (void*)1?1:*((int*)_tmp6F)!= 0)
goto _LL95;_tmp70=((struct Cyc_Absyn_Exp_s_struct*)_tmp6F)->f1;_LL94: Cyc_Tovc_apply_varmap(
env.varmap,_tmp70);return _tmp70;_LL95: if(_tmp6F <= (void*)1?1:*((int*)_tmp6F)!= 1)
goto _LL97;_tmp71=((struct Cyc_Absyn_Seq_s_struct*)_tmp6F)->f1;_tmp72=((struct Cyc_Absyn_Seq_s_struct*)
_tmp6F)->f2;_LL96: return Cyc_Absyn_seq_exp(Cyc_Tovc_stmt_to_exp(env,_tmp71),Cyc_Tovc_stmt_to_exp(
env,_tmp72),0);_LL97: if(_tmp6F <= (void*)1?1:*((int*)_tmp6F)!= 3)goto _LL99;_tmp73=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6F)->f1;_tmp74=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6F)->f2;_tmp75=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6F)->f3;_LL98: Cyc_Tovc_apply_varmap(
env.varmap,_tmp73);return Cyc_Absyn_conditional_exp(_tmp73,Cyc_Tovc_stmt_to_exp(
env,_tmp74),Cyc_Tovc_stmt_to_exp(env,_tmp75),0);_LL99: if(_tmp6F <= (void*)1?1:*((
int*)_tmp6F)!= 12)goto _LL9B;_tmp76=((struct Cyc_Absyn_Decl_s_struct*)_tmp6F)->f1;
_tmp77=((struct Cyc_Absyn_Decl_s_struct*)_tmp6F)->f2;_LL9A: {void*_tmp78=(void*)
_tmp76->r;struct Cyc_Absyn_Vardecl*_tmp79;_LL9E: if(*((int*)_tmp78)!= 0)goto _LLA0;
_tmp79=((struct Cyc_Absyn_Var_d_struct*)_tmp78)->f1;_LL9F: {struct _tagged_arr*
_tmp7A=(*_tmp79->name).f2;struct _tuple1*_tmp7B=Cyc_Toc_temp_var();_tmp79->name=
_tmp7B;env.varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmp7A,Cyc_Absyn_var_exp(
_tmp7B,0));if(_tmp79->initializer != 0){Cyc_Tovc_apply_varmap(env.varmap,(struct
Cyc_Absyn_Exp*)_check_null(_tmp79->initializer));{void*_tmp7C=(void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmp79->initializer))->r;_LLA3: if(*((int*)_tmp7C)!= 34)goto _LLA5;
_LLA4: {struct _tuple1*tmp=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*body=Cyc_Absyn_declare_stmt(
tmp,(void*)_tmp79->type,_tmp79->initializer,Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_Absyn_var_exp(tmp,0),0),0);_tmp79->initializer=(struct Cyc_Absyn_Exp*)Cyc_Tovc_stmt_to_fun(
env,body,(void*)_tmp79->type);goto _LLA2;}_LLA5:;_LLA6: goto _LLA2;_LLA2:;}}env.all_locals=({
struct Cyc_List_List*_tmp7D=_region_malloc(env.rgn,sizeof(*_tmp7D));_tmp7D->hd=
_tmp79;_tmp7D->tl=env.all_locals;_tmp7D;});{struct Cyc_Absyn_Exp*_tmp7E=_tmp79->initializer;
_tmp79->initializer=0;(void*)((env.encloser)->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp76,Cyc_Absyn_new_stmt((void*)(env.encloser)->r,0),0))->r));{struct Cyc_Absyn_Exp*
_tmp7F=Cyc_Tovc_stmt_to_exp(env,_tmp77);if(_tmp7E != 0)_tmp7F=Cyc_Absyn_seq_exp(
Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp79->name,0),(struct Cyc_Absyn_Exp*)
_check_null(_tmp7E),0),_tmp7F,0);return _tmp7F;}}}_LLA0:;_LLA1:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Core_Impossible_struct _tmp81;_tmp81.tag=Cyc_Core_Impossible;_tmp81.f1=
_tag_arr("bad local decl in Tovc::stmt_to_exp",sizeof(char),36);_tmp81;});_tmp80;}));
_LL9D:;}_LL9B:;_LL9C: return Cyc_Tovc_stmt_to_fun(env,s,(void*)0);_LL90:;}static
void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
void*_tmp82=(void*)e->r;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_List_List*_tmp84;
struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp87;
struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp8A;
struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;
struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;
struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp93;
struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp96;
struct Cyc_Absyn_Exp*_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Stmt*
_tmp99;_LLA8: if(*((int*)_tmp82)!= 0)goto _LLAA;_LLA9: return;_LLAA: if(*((int*)
_tmp82)!= 1)goto _LLAC;_LLAB: return;_LLAC: if(*((int*)_tmp82)!= 9)goto _LLAE;_tmp83=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp82)->f1;_tmp84=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp82)->f2;_LLAD: Cyc_Tovc_exp_to_vc(env,_tmp83);_tmp85=_tmp84;goto _LLAF;_LLAE:
if(*((int*)_tmp82)!= 3)goto _LLB0;_tmp85=((struct Cyc_Absyn_Primop_e_struct*)
_tmp82)->f2;_LLAF: for(0;_tmp85 != 0;_tmp85=_tmp85->tl){Cyc_Tovc_exp_to_vc(env,(
struct Cyc_Absyn_Exp*)_tmp85->hd);}return;_LLB0: if(*((int*)_tmp82)!= 6)goto _LLB2;
_tmp86=((struct Cyc_Absyn_Conditional_e_struct*)_tmp82)->f1;_tmp87=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp82)->f2;_tmp88=((struct Cyc_Absyn_Conditional_e_struct*)_tmp82)->f3;_LLB1: Cyc_Tovc_exp_to_vc(
env,_tmp88);_tmp89=_tmp86;_tmp8A=_tmp87;goto _LLB3;_LLB2: if(*((int*)_tmp82)!= 23)
goto _LLB4;_tmp89=((struct Cyc_Absyn_Subscript_e_struct*)_tmp82)->f1;_tmp8A=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp82)->f2;_LLB3: _tmp8B=_tmp89;_tmp8C=
_tmp8A;goto _LLB5;_LLB4: if(*((int*)_tmp82)!= 7)goto _LLB6;_tmp8B=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp82)->f1;_tmp8C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp82)->f2;_LLB5: _tmp8D=
_tmp8B;_tmp8E=_tmp8C;goto _LLB7;_LLB6: if(*((int*)_tmp82)!= 4)goto _LLB8;_tmp8D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp82)->f1;_tmp8E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp82)->f3;_LLB7: Cyc_Tovc_exp_to_vc(env,_tmp8E);_tmp8F=_tmp8D;goto _LLB9;_LLB8:
if(*((int*)_tmp82)!= 11)goto _LLBA;_tmp8F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp82)->f1;_LLB9: _tmp90=_tmp8F;goto _LLBB;_LLBA: if(*((int*)_tmp82)!= 12)goto
_LLBC;_tmp90=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp82)->f1;_LLBB: _tmp91=
_tmp90;goto _LLBD;_LLBC: if(*((int*)_tmp82)!= 13)goto _LLBE;_tmp91=((struct Cyc_Absyn_Cast_e_struct*)
_tmp82)->f2;_LLBD: _tmp92=_tmp91;goto _LLBF;_LLBE: if(*((int*)_tmp82)!= 14)goto
_LLC0;_tmp92=((struct Cyc_Absyn_Address_e_struct*)_tmp82)->f1;_LLBF: _tmp93=_tmp92;
goto _LLC1;_LLC0: if(*((int*)_tmp82)!= 17)goto _LLC2;_tmp93=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp82)->f1;_LLC1: _tmp94=_tmp93;goto _LLC3;_LLC2: if(*((int*)_tmp82)!= 20)goto
_LLC4;_tmp94=((struct Cyc_Absyn_Deref_e_struct*)_tmp82)->f1;_LLC3: _tmp95=_tmp94;
goto _LLC5;_LLC4: if(*((int*)_tmp82)!= 21)goto _LLC6;_tmp95=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp82)->f1;_LLC5: _tmp96=_tmp95;goto _LLC7;_LLC6: if(*((int*)_tmp82)!= 22)goto
_LLC8;_tmp96=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp82)->f1;_LLC7: _tmp97=
_tmp96;goto _LLC9;_LLC8: if(*((int*)_tmp82)!= 5)goto _LLCA;_tmp97=((struct Cyc_Absyn_Increment_e_struct*)
_tmp82)->f1;_LLC9: Cyc_Tovc_exp_to_vc(env,_tmp97);return;_LLCA: if(*((int*)_tmp82)
!= 16)goto _LLCC;_LLCB: goto _LLCD;_LLCC: if(*((int*)_tmp82)!= 18)goto _LLCE;_LLCD:
goto _LLCF;_LLCE: if(*((int*)_tmp82)!= 19)goto _LLD0;_LLCF: goto _LLD1;_LLD0: if(*((
int*)_tmp82)!= 31)goto _LLD2;_LLD1: return;_LLD2: if(*((int*)_tmp82)!= 34)goto _LLD4;
_tmp98=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp82)->f2;_LLD3: for(0;_tmp98
!= 0;_tmp98=_tmp98->tl){Cyc_Tovc_exp_to_vc(env,(*((struct _tuple4*)_tmp98->hd)).f2);}
return;_LLD4: if(*((int*)_tmp82)!= 35)goto _LLD6;_tmp99=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp82)->f1;_LLD5: Cyc_Tovc_stmt_to_vc(env,_tmp99);(void*)(e->r=(void*)((void*)(
Cyc_Tovc_stmt_to_exp(env,_tmp99))->r));return;_LLD6:;_LLD7:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({
struct Cyc_Core_Impossible_struct _tmp9B;_tmp9B.tag=Cyc_Core_Impossible;_tmp9B.f1=
_tag_arr("bad exp form after xlation to C",sizeof(char),32);_tmp9B;});_tmp9A;}));
_LLA7:;}static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*
s){while(1){env.encloser=s;{void*_tmp9C=(void*)s->r;struct Cyc_Absyn_Stmt*_tmp9D;
struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Stmt*
_tmpA0;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Stmt*
_tmpA3;struct Cyc_Absyn_Stmt*_tmpA4;struct _tuple3 _tmpA5;struct Cyc_Absyn_Exp*
_tmpA6;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct _tuple3
_tmpA9;struct Cyc_Absyn_Exp*_tmpAA;struct _tuple3 _tmpAB;struct Cyc_Absyn_Exp*_tmpAC;
struct Cyc_Absyn_Stmt*_tmpAD;struct Cyc_Absyn_Stmt*_tmpAE;struct _tuple3 _tmpAF;
struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_List_List*_tmpB2;
struct Cyc_Absyn_Decl*_tmpB3;struct Cyc_Absyn_Stmt*_tmpB4;_LLD9: if((int)_tmp9C != 0)
goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 5)goto
_LLDD;_LLDC: goto _LLDE;_LLDD: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 6)goto _LLDF;
_LLDE: goto _LLE0;_LLDF: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 7)goto _LLE1;_LLE0:
return;_LLE1: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 15)goto _LLE3;_tmp9D=((
struct Cyc_Absyn_Label_s_struct*)_tmp9C)->f2;_LLE2: s=_tmp9D;continue;_LLE3: if(
_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 2)goto _LLE5;_tmp9E=((struct Cyc_Absyn_Return_s_struct*)
_tmp9C)->f1;_LLE4: if(_tmp9E == 0)return;_tmp9F=(struct Cyc_Absyn_Exp*)_check_null(
_tmp9E);goto _LLE6;_LLE5: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 0)goto _LLE7;
_tmp9F=((struct Cyc_Absyn_Exp_s_struct*)_tmp9C)->f1;_LLE6: Cyc_Tovc_exp_to_vc(env,
_tmp9F);return;_LLE7: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 1)goto _LLE9;_tmpA0=((
struct Cyc_Absyn_Seq_s_struct*)_tmp9C)->f1;_tmpA1=((struct Cyc_Absyn_Seq_s_struct*)
_tmp9C)->f2;_LLE8: Cyc_Tovc_stmt_to_vc(env,_tmpA0);s=_tmpA1;continue;_LLE9: if(
_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 3)goto _LLEB;_tmpA2=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9C)->f1;_tmpA3=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp9C)->f2;_tmpA4=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp9C)->f3;_LLEA: Cyc_Tovc_exp_to_vc(env,
_tmpA2);Cyc_Tovc_stmt_to_vc(env,_tmpA3);s=_tmpA4;continue;_LLEB: if(_tmp9C <= (
void*)1?1:*((int*)_tmp9C)!= 4)goto _LLED;_tmpA5=((struct Cyc_Absyn_While_s_struct*)
_tmp9C)->f1;_tmpA6=_tmpA5.f1;_tmpA7=((struct Cyc_Absyn_While_s_struct*)_tmp9C)->f2;
_LLEC: Cyc_Tovc_exp_to_vc(env,_tmpA6);s=_tmpA7;continue;_LLED: if(_tmp9C <= (void*)
1?1:*((int*)_tmp9C)!= 8)goto _LLEF;_tmpA8=((struct Cyc_Absyn_For_s_struct*)_tmp9C)->f1;
_tmpA9=((struct Cyc_Absyn_For_s_struct*)_tmp9C)->f2;_tmpAA=_tmpA9.f1;_tmpAB=((
struct Cyc_Absyn_For_s_struct*)_tmp9C)->f3;_tmpAC=_tmpAB.f1;_tmpAD=((struct Cyc_Absyn_For_s_struct*)
_tmp9C)->f4;_LLEE: Cyc_Tovc_exp_to_vc(env,_tmpA8);Cyc_Tovc_exp_to_vc(env,_tmpAA);
Cyc_Tovc_exp_to_vc(env,_tmpAC);s=_tmpAD;continue;_LLEF: if(_tmp9C <= (void*)1?1:*((
int*)_tmp9C)!= 16)goto _LLF1;_tmpAE=((struct Cyc_Absyn_Do_s_struct*)_tmp9C)->f1;
_tmpAF=((struct Cyc_Absyn_Do_s_struct*)_tmp9C)->f2;_tmpB0=_tmpAF.f1;_LLF0: Cyc_Tovc_exp_to_vc(
env,_tmpB0);s=_tmpAE;continue;_LLF1: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 9)
goto _LLF3;_tmpB1=((struct Cyc_Absyn_Switch_s_struct*)_tmp9C)->f1;_tmpB2=((struct
Cyc_Absyn_Switch_s_struct*)_tmp9C)->f2;_LLF2: Cyc_Tovc_exp_to_vc(env,_tmpB1);for(
0;_tmpB2 != 0;_tmpB2=_tmpB2->tl){Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)
_tmpB2->hd)->body);}return;_LLF3: if(_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 12)
goto _LLF5;_tmpB3=((struct Cyc_Absyn_Decl_s_struct*)_tmp9C)->f1;_tmpB4=((struct Cyc_Absyn_Decl_s_struct*)
_tmp9C)->f2;_LLF4:{void*_tmpB5=(void*)_tmpB3->r;struct Cyc_Absyn_Vardecl*_tmpB6;
_LLF8: if(*((int*)_tmpB5)!= 0)goto _LLFA;_tmpB6=((struct Cyc_Absyn_Var_d_struct*)
_tmpB5)->f1;_LLF9:{void*_tmpB7=Cyc_Tcutil_compress((void*)_tmpB6->type);_LLFD:
if(_tmpB7 <= (void*)3?1:*((int*)_tmpB7)!= 7)goto _LLFF;_LLFE: if(_tmpB6->initializer
!= 0){void*_tmpB8=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmpB6->initializer))->r;
struct Cyc_List_List*_tmpB9;_LL102: if(*((int*)_tmpB8)!= 34)goto _LL104;_tmpB9=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpB8)->f2;_LL103: _tmpB6->initializer=0;{
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmpB6->name,0);struct Cyc_Absyn_Stmt*
s3=Cyc_Absyn_new_stmt((void*)_tmpB4->r,0);{unsigned int i=0;for(0;_tmpB9 != 0;(
_tmpB9=_tmpB9->tl,++ i)){s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(arr,Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple4*)_tmpB9->hd)).f2,
0),0),s3,0);}}(void*)(_tmpB4->r=(void*)((void*)s3->r));goto _LL101;}_LL104:;
_LL105:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Core_Impossible_struct _tmpBB;_tmpBB.tag=
Cyc_Core_Impossible;_tmpBB.f1=_tag_arr("array type bad initializer",sizeof(char),
27);_tmpBB;});_tmpBA;}));_LL101:;}goto _LLFC;_LLFF:;_LL100: goto _LLFC;_LLFC:;}env.all_locals=({
struct Cyc_List_List*_tmpBC=_region_malloc(env.rgn,sizeof(*_tmpBC));_tmpBC->hd=
_tmpB6;_tmpBC->tl=env.all_locals;_tmpBC;});if(_tmpB6->initializer != 0)Cyc_Tovc_exp_to_vc(
env,(struct Cyc_Absyn_Exp*)_check_null(_tmpB6->initializer));s=_tmpB4;continue;
_LLFA:;_LLFB: goto _LLF7;_LLF7:;}goto _LLF6;_LLF5:;_LLF6:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({
struct Cyc_Core_Impossible_struct _tmpBE;_tmpBE.tag=Cyc_Core_Impossible;_tmpBE.f1=
_tag_arr("bad stmt after xlation to C",sizeof(char),28);_tmpBE;});_tmpBD;}));
_LLD8:;}}}struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){struct
Cyc_List_List*_tmpBF=0;for(0;old_ds != 0;old_ds=old_ds->tl){struct Cyc_Absyn_Decl*
_tmpC0=(struct Cyc_Absyn_Decl*)old_ds->hd;void*_tmpC1=(void*)_tmpC0->r;struct Cyc_Absyn_Fndecl*
_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_List_List*
_tmpC5;_LL107: if(*((int*)_tmpC1)!= 1)goto _LL109;_tmpC2=((struct Cyc_Absyn_Fn_d_struct*)
_tmpC1)->f1;_LL108:{struct _RegionHandle _tmpC6=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpC6;_push_region(rgn);{struct Cyc_List_List**_tmpC7=({
struct Cyc_List_List**_tmpC9=_region_malloc(rgn,sizeof(*_tmpC9));_tmpC9[0]=0;
_tmpC9;});Cyc_Tovc_stmt_to_vc(({struct Cyc_Tovc_ToExpEnv _tmpC8;_tmpC8.rgn=rgn;
_tmpC8.all_locals=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmpC2->param_vardecls))->v);_tmpC8.varmap=((struct Cyc_Dict_Dict*(*)(struct
_RegionHandle*,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_rempty)(
rgn,Cyc_Std_zstrptrcmp);_tmpC8.encloser=_tmpC2->body;_tmpC8.gen_ds=_tmpC7;_tmpC8;}),
_tmpC2->body);_tmpBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(*_tmpC7,_tmpBF);};_pop_region(rgn);}goto _LL10A;_LL109: if(*((
int*)_tmpC1)!= 0)goto _LL10B;_LL10A: goto _LL10C;_LL10B: if(*((int*)_tmpC1)!= 2)goto
_LL10D;_LL10C: goto _LL10E;_LL10D: if(*((int*)_tmpC1)!= 3)goto _LL10F;_LL10E: goto
_LL110;_LL10F: if(*((int*)_tmpC1)!= 4)goto _LL111;_LL110: goto _LL112;_LL111: if(*((
int*)_tmpC1)!= 5)goto _LL113;_LL112: goto _LL114;_LL113: if(*((int*)_tmpC1)!= 6)goto
_LL115;_LL114: goto _LL116;_LL115: if(*((int*)_tmpC1)!= 7)goto _LL117;_LL116: _tmpBF=({
struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->hd=_tmpC0;_tmpCA->tl=
_tmpBF;_tmpCA;});goto _LL106;_LL117: if(*((int*)_tmpC1)!= 8)goto _LL119;_tmpC3=((
struct Cyc_Absyn_Namespace_d_struct*)_tmpC1)->f2;_LL118: _tmpC4=_tmpC3;goto _LL11A;
_LL119: if(*((int*)_tmpC1)!= 9)goto _LL11B;_tmpC4=((struct Cyc_Absyn_Using_d_struct*)
_tmpC1)->f2;_LL11A: _tmpC5=_tmpC4;goto _LL11C;_LL11B: if(*((int*)_tmpC1)!= 10)goto
_LL106;_tmpC5=((struct Cyc_Absyn_ExternC_d_struct*)_tmpC1)->f1;_LL11C:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));
_tmpCB[0]=({struct Cyc_Core_Impossible_struct _tmpCC;_tmpCC.tag=Cyc_Core_Impossible;
_tmpCC.f1=_tag_arr("nested translation unit after translation to C",sizeof(char),
47);_tmpCC;});_tmpCB;}));_LL106:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpBF);}
