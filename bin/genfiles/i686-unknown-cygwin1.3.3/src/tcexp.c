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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*
Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern void*
Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_exn_typ;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*
Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(void*);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_Std_zstrcmp(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct
_tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);int Cyc_Tcutil_kind_leq(
void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tagged_arr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tagged_arr msg,
struct _tagged_arr ap){({void*_tmp0[0]={};Cyc_Tcutil_terr(loc,(struct _tagged_arr)
Cyc_Std_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));
_tmp1->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1;}));}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))
_tmp4=1;if(!_tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(te,e->loc,q);void*_tmp6;
struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct
Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*_tmpB;struct Cyc_Absyn_Tunionfield*
_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp5)->f1;_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=
_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=
1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)
_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Enum_e_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct
Cyc_Absyn_Enum_e_struct _tmp10;_tmp10.tag=31;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp7;_tmp10.f3=(struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;
_LL5: if(*((int*)_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5)->f1;_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*
_tmp11));_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=32;
_tmp12.f1=q;_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;
_tmp12;});_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f2;_LL8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp14;_tmp14.tag=30;_tmp14.f1=0;_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});
_tmp13;})));goto _LL0;_LL9: if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_Std_String_pa_struct
_tmp16;_tmp16.tag=0;_tmp16.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(q);{
void*_tmp15[1]={& _tmp16};Cyc_Tcutil_terr(e->loc,_tag_arr("bad occurrence of type name %s",
sizeof(char),31),_tag_arr(_tmp15,sizeof(void*),1));}});(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Var_e_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({
struct Cyc_Absyn_Var_e_struct _tmp18;_tmp18.tag=1;_tmp18.f1=q;_tmp18.f2=(void*)((
void*)0);_tmp18;});_tmp17;})));goto _LL0;_LL0:;};_pop_handler();}else{void*_tmp3=(
void*)_exn_thrown;void*_tmp1A=_tmp3;_LLC: if(_tmp1A != Cyc_Dict_Absent)goto _LLE;
_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1B=_cycalloc(
sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Var_e_struct _tmp1C;_tmp1C.tag=1;
_tmp1C.f1=q;_tmp1C.f2=(void*)((void*)0);_tmp1C;});_tmp1B;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1A);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->f1=0;_tmp1D->f2=
e;_tmp1D;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1E=(
void*)e1->r;struct _tuple1*_tmp1F;_LL11: if(*((int*)_tmp1E)!= 2)goto _LL13;_tmp1F=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp1E)->f1;_LL12: {struct _handler_cons
_tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler))_tmp22=1;
if(!_tmp22){{void*_tmp23=Cyc_Tcenv_lookup_ordinary(te,e1->loc,_tmp1F);void*
_tmp24;struct Cyc_Absyn_Tuniondecl*_tmp25;struct Cyc_Absyn_Tunionfield*_tmp26;
struct Cyc_Absyn_Aggrdecl*_tmp27;_LL16: if(*((int*)_tmp23)!= 0)goto _LL18;_tmp24=(
void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp23)->f1;_LL17:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp29;_tmp29.tag=9;_tmp29.f1=e1;_tmp29.f2=
es;_tmp29.f3=0;_tmp29;});_tmp28;})));_npop_handler(0);return;_LL18: if(*((int*)
_tmp23)!= 2)goto _LL1A;_tmp25=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f1;
_tmp26=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f2;_LL19: if(_tmp26->typs == 0)({
struct Cyc_Std_String_pa_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp26->name);{void*_tmp2A[1]={& _tmp2B};Cyc_Tcutil_terr(
e->loc,_tag_arr("%s is a constant, not a function",sizeof(char),33),_tag_arr(
_tmp2A,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp2D;_tmp2D.tag=30;_tmp2D.f1=es;_tmp2D.f2=_tmp25;_tmp2D.f3=_tmp26;_tmp2D;});
_tmp2C;})));_npop_handler(0);return;_LL1A: if(*((int*)_tmp23)!= 1)goto _LL1C;
_tmp27=((struct Cyc_Tcenv_AggrRes_struct*)_tmp23)->f1;_LL1B: {struct Cyc_List_List*
_tmp2E=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp30;_tmp30.tag=28;_tmp30.f1=_tmp27->name;
_tmp30.f2=0;_tmp30.f3=_tmp2E;_tmp30.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp27;_tmp30;});
_tmp2F;})));_npop_handler(0);return;}_LL1C: if(*((int*)_tmp23)!= 4)goto _LL1E;
_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp23)!= 3)goto _LL15;_LL1F:({struct Cyc_Std_String_pa_struct
_tmp32;_tmp32.tag=0;_tmp32.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F);{
void*_tmp31[1]={& _tmp32};Cyc_Tcutil_terr(e->loc,_tag_arr("%s is an enum constructor, not a function",
sizeof(char),42),_tag_arr(_tmp31,sizeof(void*),1));}});_npop_handler(0);return;
_LL15:;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp34=_tmp21;
_LL21: if(_tmp34 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_FnCall_e_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({
struct Cyc_Absyn_FnCall_e_struct _tmp36;_tmp36.tag=9;_tmp36.f1=e1;_tmp36.f2=es;
_tmp36.f3=0;_tmp36;});_tmp35;})));return;_LL23:;_LL24:(void)_throw(_tmp34);_LL20:;}}}
_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp38;_tmp38.tag=9;_tmp38.f1=e1;_tmp38.f2=es;_tmp38.f3=0;_tmp38;});_tmp37;})));
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3A;_tmp3A.tag=26;_tmp3A.f1=des;_tmp3A;});_tmp39;})));return;}{void*t=*topt;
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3C;void*_tmp3D;
void*_tmp3E;struct Cyc_Absyn_Tqual _tmp3F;_LL26: if(_tmp3B <= (void*)3?1:*((int*)
_tmp3B)!= 10)goto _LL28;_tmp3C=((struct Cyc_Absyn_AggrType_struct*)_tmp3B)->f1;
_tmp3D=(void*)_tmp3C.aggr_info;_LL27:{void*_tmp40=_tmp3D;struct Cyc_Absyn_Aggrdecl**
_tmp41;struct Cyc_Absyn_Aggrdecl*_tmp42;_LL2F: if(*((int*)_tmp40)!= 0)goto _LL31;
_LL30:({void*_tmp43[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("struct type not properly set",sizeof(char),29),_tag_arr(_tmp43,sizeof(
void*),0));});_LL31: if(*((int*)_tmp40)!= 1)goto _LL2E;_tmp41=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp40)->f1;_tmp42=*_tmp41;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp45;_tmp45.tag=28;_tmp45.f1=_tmp42->name;_tmp45.f2=0;_tmp45.f3=des;_tmp45.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp42;_tmp45;});_tmp44;})));_LL2E:;}goto _LL25;_LL28:
if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 7)goto _LL2A;_tmp3E=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3B)->f1;_tmp3F=((struct Cyc_Absyn_ArrayType_struct*)_tmp3B)->f2;_LL29:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp46=_cycalloc(sizeof(*
_tmp46));_tmp46[0]=({struct Cyc_Absyn_Array_e_struct _tmp47;_tmp47.tag=26;_tmp47.f1=
des;_tmp47;});_tmp46;})));goto _LL25;_LL2A: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)
!= 11)goto _LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp49;_tmp49.tag=29;_tmp49.f1=(void*)t;_tmp49.f2=des;_tmp49;});_tmp48;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4B;_tmp4B.tag=26;_tmp4B.f1=des;_tmp4B;});_tmp4A;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_Std_String_pa_struct
_tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _tagged_arr)msg_part;{void*_tmp4C[2]={&
_tmp4D,& _tmp4E};Cyc_Tcutil_terr(e->loc,_tag_arr("test of %s has type %s instead of integral or * type",
sizeof(char),53),_tag_arr(_tmp4C,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*c,struct
Cyc_Absyn_Exp*e){void*t;{void*_tmp4F=c;void*_tmp50;void*_tmp51;void*_tmp52;void*
_tmp53;void*_tmp54;void*_tmp55;int _tmp56;struct _tagged_arr _tmp57;_LL34: if(_tmp4F
<= (void*)1?1:*((int*)_tmp4F)!= 0)goto _LL36;_tmp50=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp4F)->f1;if((int)_tmp50 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;
_LL36: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 0)goto _LL38;_tmp51=(void*)((
struct Cyc_Absyn_Char_c_struct*)_tmp4F)->f1;if((int)_tmp51 != 1)goto _LL38;_LL37: t=
Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 0)
goto _LL3A;_tmp52=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp4F)->f1;if((int)
_tmp52 != 2)goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if(_tmp4F <= (
void*)1?1:*((int*)_tmp4F)!= 1)goto _LL3C;_tmp53=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp4F)->f1;_LL3B: t=_tmp53 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
goto _LL33;_LL3C: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 3)goto _LL3E;_tmp54=(
void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp4F)->f1;_LL3D: t=_tmp54 == (void*)1?
Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if(_tmp4F <= (
void*)1?1:*((int*)_tmp4F)!= 4)goto _LL40;_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;
_LL40: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 2)goto _LL42;_tmp55=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp4F)->f1;_tmp56=((struct Cyc_Absyn_Int_c_struct*)
_tmp4F)->f2;_LL41: if(topt == 0)t=_tmp55 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
else{void*_tmp58=Cyc_Tcutil_compress(*topt);void*_tmp59;void*_tmp5A;void*_tmp5B;
void*_tmp5C;void*_tmp5D;void*_tmp5E;_LL47: if(_tmp58 <= (void*)3?1:*((int*)_tmp58)
!= 5)goto _LL49;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp58)->f1;
_tmp5A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp58)->f2;if((int)_tmp5A != 0)
goto _LL49;_LL48:{void*_tmp5F=_tmp59;_LL54: if((int)_tmp5F != 1)goto _LL56;_LL55: t=
Cyc_Absyn_uchar_typ;goto _LL53;_LL56: if((int)_tmp5F != 0)goto _LL58;_LL57: t=Cyc_Absyn_schar_typ;
goto _LL53;_LL58: if((int)_tmp5F != 2)goto _LL53;_LL59: t=Cyc_Absyn_char_typ;goto
_LL53;_LL53:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_Const_e_struct
_tmp61;_tmp61.tag=0;_tmp61.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Char_c_struct
_tmp63;_tmp63.tag=0;_tmp63.f1=(void*)_tmp59;_tmp63.f2=(char)_tmp56;_tmp63;});
_tmp62;}));_tmp61;});_tmp60;})));goto _LL46;_LL49: if(_tmp58 <= (void*)3?1:*((int*)
_tmp58)!= 5)goto _LL4B;_tmp5B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp58)->f1;
_tmp5C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp58)->f2;if((int)_tmp5C != 1)
goto _LL4B;_LL4A: t=_tmp5B == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp64=_cycalloc(
sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_Const_e_struct _tmp65;_tmp65.tag=0;
_tmp65.f1=(void*)((void*)({struct Cyc_Absyn_Short_c_struct*_tmp66=_cycalloc(
sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Absyn_Short_c_struct _tmp67;_tmp67.tag=1;
_tmp67.f1=(void*)_tmp5B;_tmp67.f2=(short)_tmp56;_tmp67;});_tmp66;}));_tmp65;});
_tmp64;})));goto _LL46;_LL4B: if(_tmp58 <= (void*)3?1:*((int*)_tmp58)!= 5)goto _LL4D;
_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp58)->f1;_LL4C: t=_tmp5D == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(_tmp58 <= (void*)
3?1:*((int*)_tmp58)!= 4)goto _LL4F;if(!(_tmp56 == 0))goto _LL4F;_LL4E:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68[0]=({struct Cyc_Absyn_Const_e_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(void*)((
void*)0);_tmp69;});_tmp68;})));{struct Cyc_List_List*_tmp6A=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6B=_cycalloc(sizeof(*
_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_PointerType_struct _tmp6C;_tmp6C.tag=4;
_tmp6C.f1=({struct Cyc_Absyn_PtrInfo _tmp6D;_tmp6D.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp6F=_cycalloc(sizeof(*
_tmp6F));_tmp6F->v=_tmp6A;_tmp6F;}));_tmp6D.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E->v=_tmp6A;_tmp6E;}));_tmp6D.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp6D.tq=Cyc_Absyn_empty_tqual();_tmp6D.bounds=
Cyc_Absyn_empty_conref();_tmp6D;});_tmp6C;});_tmp6B;});goto _LL46;}_LL4F: if(
_tmp58 <= (void*)3?1:*((int*)_tmp58)!= 17)goto _LL51;_tmp5E=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp58)->f1;_LL50: {struct Cyc_Absyn_TypeInt_struct*_tmp70=({struct Cyc_Absyn_TypeInt_struct*
_tmp76=_cycalloc_atomic(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp77;_tmp77.tag=18;_tmp77.f1=_tmp56;_tmp77;});_tmp76;});if(!Cyc_Tcutil_unify(
_tmp5E,(void*)_tmp70)){({struct Cyc_Std_String_pa_struct _tmp73;_tmp73.tag=0;
_tmp73.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)_tmp70);{struct Cyc_Std_String_pa_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp5E);{
void*_tmp71[2]={& _tmp72,& _tmp73};Cyc_Tcutil_terr(loc,_tag_arr("expecting %s but found %s",
sizeof(char),26),_tag_arr(_tmp71,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
t=(void*)({struct Cyc_Absyn_TagType_struct*_tmp74=_cycalloc(sizeof(*_tmp74));
_tmp74[0]=({struct Cyc_Absyn_TagType_struct _tmp75;_tmp75.tag=17;_tmp75.f1=(void*)((
void*)_tmp70);_tmp75;});_tmp74;});goto _LL46;}_LL51:;_LL52: t=_tmp55 == (void*)1?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if(
_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 5)goto _LL44;_tmp57=((struct Cyc_Absyn_String_c_struct*)
_tmp4F)->f1;_LL43: {int len=(int)_get_arr_size(_tmp57,sizeof(char));struct Cyc_Absyn_Exp*
elen=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp84=_cycalloc(
sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_Int_c_struct _tmp85;_tmp85.tag=2;
_tmp85.f1=(void*)((void*)1);_tmp85.f2=len;_tmp85;});_tmp84;}),loc);elen->topt=({
struct Cyc_Core_Opt*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->v=(void*)Cyc_Absyn_uint_typ;
_tmp78;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=elen;_tmp7A;});
_tmp79;}));if(topt != 0){void*_tmp7B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Tqual
_tmp7C;_LL5B: if(_tmp7B <= (void*)3?1:*((int*)_tmp7B)!= 7)goto _LL5D;_tmp7C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp7B)->f2;_LL5C: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp7E;_tmp7E.tag=7;_tmp7E.f1=(void*)Cyc_Absyn_char_typ;_tmp7E.f2=_tmp7C;_tmp7E.f3=(
struct Cyc_Absyn_Exp*)elen;_tmp7E;});_tmp7D;});_LL5D: if(_tmp7B <= (void*)3?1:*((
int*)_tmp7B)!= 4)goto _LL5F;_LL5E: if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp7F=_cycalloc(sizeof(*_tmp7F));
_tmp7F->v=(void*)t;_tmp7F;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}
else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp80;})),Cyc_Absyn_const_tqual(),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp82;_tmp82.tag=0;_tmp82.f1=elen;_tmp82;});
_tmp81;}));if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(te,loc,t,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->v=(
void*)t;_tmp83;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}}goto _LL5A;
_LL5F:;_LL60: goto _LL5A;_LL5A:;}return t;}_LL44: if((int)_tmp4F != 0)goto _LL33;_LL45: {
struct Cyc_List_List*_tmp86=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_PointerType_struct
_tmp88;_tmp88.tag=4;_tmp88.f1=({struct Cyc_Absyn_PtrInfo _tmp89;_tmp89.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->v=_tmp86;_tmp8B;}));_tmp89.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->v=_tmp86;_tmp8A;}));_tmp89.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp89.tq=Cyc_Absyn_empty_tqual();
_tmp89.bounds=Cyc_Absyn_empty_conref();_tmp89;});_tmp88;});_tmp87;});goto _LL33;}
_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp8C=b;struct Cyc_Absyn_Vardecl*_tmp8D;struct
Cyc_Absyn_Fndecl*_tmp8E;struct Cyc_Absyn_Vardecl*_tmp8F;struct Cyc_Absyn_Vardecl*
_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;_LL62: if((int)_tmp8C != 0)goto _LL64;_LL63:
return({struct Cyc_Std_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(q);{void*_tmp92[1]={& _tmp93};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("undeclared identifier: %s",sizeof(char),26),_tag_arr(_tmp92,
sizeof(void*),1));}});_LL64: if(_tmp8C <= (void*)1?1:*((int*)_tmp8C)!= 0)goto _LL66;
_tmp8D=((struct Cyc_Absyn_Global_b_struct*)_tmp8C)->f1;_LL65:*q=*_tmp8D->name;
return(void*)_tmp8D->type;_LL66: if(_tmp8C <= (void*)1?1:*((int*)_tmp8C)!= 1)goto
_LL68;_tmp8E=((struct Cyc_Absyn_Funname_b_struct*)_tmp8C)->f1;_LL67:*q=*_tmp8E->name;
return Cyc_Tcutil_fndecl2typ(_tmp8E);_LL68: if(_tmp8C <= (void*)1?1:*((int*)_tmp8C)
!= 4)goto _LL6A;_tmp8F=((struct Cyc_Absyn_Pat_b_struct*)_tmp8C)->f1;_LL69: _tmp90=
_tmp8F;goto _LL6B;_LL6A: if(_tmp8C <= (void*)1?1:*((int*)_tmp8C)!= 3)goto _LL6C;
_tmp90=((struct Cyc_Absyn_Local_b_struct*)_tmp8C)->f1;_LL6B: _tmp91=_tmp90;goto
_LL6D;_LL6C: if(_tmp8C <= (void*)1?1:*((int*)_tmp8C)!= 2)goto _LL61;_tmp91=((struct
Cyc_Absyn_Param_b_struct*)_tmp8C)->f1;_LL6D:(*q).f1=(void*)0;return(void*)_tmp91->type;
_LL61:;}static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{
void*_tmp94=(void*)fmt->r;void*_tmp95;struct _tagged_arr _tmp96;struct Cyc_Absyn_Exp*
_tmp97;struct Cyc_Absyn_Exp _tmp98;void*_tmp99;void*_tmp9A;struct _tagged_arr _tmp9B;
_LL6F: if(*((int*)_tmp94)!= 0)goto _LL71;_tmp95=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp94)->f1;if(_tmp95 <= (void*)1?1:*((int*)_tmp95)!= 5)goto _LL71;_tmp96=((struct
Cyc_Absyn_String_c_struct*)_tmp95)->f1;_LL70: _tmp9B=_tmp96;goto _LL72;_LL71: if(*((
int*)_tmp94)!= 13)goto _LL73;_tmp97=((struct Cyc_Absyn_Cast_e_struct*)_tmp94)->f2;
_tmp98=*_tmp97;_tmp99=(void*)_tmp98.r;if(*((int*)_tmp99)!= 0)goto _LL73;_tmp9A=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;if(_tmp9A <= (void*)1?1:*((
int*)_tmp9A)!= 5)goto _LL73;_tmp9B=((struct Cyc_Absyn_String_c_struct*)_tmp9A)->f1;
_LL72: desc_types=type_getter(te,(struct _tagged_arr)_tmp9B,fmt->loc);goto _LL6E;
_LL73:;_LL74: return;_LL6E:;}if(opt_args != 0){struct Cyc_List_List*_tmp9C=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0?_tmp9C != 0: 0;(desc_types=
desc_types->tl,_tmp9C=_tmp9C->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmp9C->hd;if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct
Cyc_Std_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp9E;_tmp9E.tag=0;_tmp9E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp9D[2]={& _tmp9E,& _tmp9F};Cyc_Tcutil_terr(e->loc,_tag_arr("descriptor has type \n%s\n but argument has type \n%s",
sizeof(char),51),_tag_arr(_tmp9D,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(desc_types != 0)({void*_tmpA0[0]={};Cyc_Tcutil_terr(fmt->loc,_tag_arr("too few arguments",
sizeof(char),18),_tag_arr(_tmpA0,sizeof(void*),0));});if(_tmp9C != 0)({void*
_tmpA1[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp9C->hd)->loc,_tag_arr("too many arguments",
sizeof(char),19),_tag_arr(_tmpA1,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpA2=p;_LL76: if((int)_tmpA2 != 0)goto _LL78;_LL77:
goto _LL79;_LL78: if((int)_tmpA2 != 2)goto _LL7A;_LL79: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_Std_String_pa_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmpA3[1]={& _tmpA4};Cyc_Tcutil_terr(loc,_tag_arr("expecting arithmetic type but found %s",
sizeof(char),39),_tag_arr(_tmpA3,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL7A: if((int)_tmpA2 != 11)goto _LL7C;_LL7B: Cyc_Tcutil_check_contains_assign(
e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_Std_String_pa_struct _tmpA6;
_tmpA6.tag=0;_tmpA6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA5[
1]={& _tmpA6};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral or * type but found %s",
sizeof(char),42),_tag_arr(_tmpA5,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;
_LL7C: if((int)_tmpA2 != 12)goto _LL7E;_LL7D: if(!Cyc_Tcutil_is_integral(e))({struct
Cyc_Std_String_pa_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmpA7[1]={& _tmpA8};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral type but found %s",
sizeof(char),37),_tag_arr(_tmpA7,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL7E: if((int)_tmpA2 != 19)goto _LL80;_LL7F:{void*_tmpA9=
t;_LL83: if(_tmpA9 <= (void*)3?1:*((int*)_tmpA9)!= 7)goto _LL85;_LL84: goto _LL86;
_LL85: if(_tmpA9 <= (void*)3?1:*((int*)_tmpA9)!= 4)goto _LL87;_LL86: goto _LL82;_LL87:;
_LL88:({struct Cyc_Std_String_pa_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpAA[1]={& _tmpAB};Cyc_Tcutil_terr(
loc,_tag_arr("size requires pointer or array type, not %s",sizeof(char),44),
_tag_arr(_tmpAA,sizeof(void*),1));}});_LL82:;}return Cyc_Absyn_uint_typ;_LL80:;
_LL81:({void*_tmpAC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Non-unary primop",sizeof(char),17),_tag_arr(_tmpAC,sizeof(void*),0));});
_LL75:;}static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_Std_String_pa_struct _tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmpAD[1]={& _tmpAE};Cyc_Tcutil_terr(e1->loc,_tag_arr("type %s cannot be used here",
sizeof(char),28),_tag_arr(_tmpAD,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpAF;}));}if(!checker(e2)){({struct Cyc_Std_String_pa_struct _tmpB1;_tmpB1.tag=
0;_tmpB1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmpB0[1]={& _tmpB1};Cyc_Tcutil_terr(e2->loc,
_tag_arr("type %s cannot be used here",sizeof(char),28),_tag_arr(_tmpB0,sizeof(
void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpB2=_cycalloc(
sizeof(*_tmpB2));_tmpB2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB2;}));}{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpB3=t1;struct Cyc_Absyn_PtrInfo
_tmpB4;void*_tmpB5;void*_tmpB6;struct Cyc_Absyn_Conref*_tmpB7;struct Cyc_Absyn_Tqual
_tmpB8;struct Cyc_Absyn_Conref*_tmpB9;_LL8A: if(_tmpB3 <= (void*)3?1:*((int*)_tmpB3)
!= 4)goto _LL8C;_tmpB4=((struct Cyc_Absyn_PointerType_struct*)_tmpB3)->f1;_tmpB5=(
void*)_tmpB4.elt_typ;_tmpB6=(void*)_tmpB4.rgn_typ;_tmpB7=_tmpB4.nullable;_tmpB8=
_tmpB4.tq;_tmpB9=_tmpB4.bounds;_LL8B: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmpB5),(void*)1))({void*_tmpBA[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(char),50),_tag_arr(_tmpBA,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))({struct Cyc_Std_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmpBB[1]={& _tmpBC};Cyc_Tcutil_terr(
e2->loc,_tag_arr("expecting int but found %s",sizeof(char),27),_tag_arr(_tmpBB,
sizeof(void*),1));}});_tmpB9=Cyc_Absyn_compress_conref(_tmpB9);{void*_tmpBD=(
void*)_tmpB9->v;void*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;_LL8F: if(
_tmpBD <= (void*)1?1:*((int*)_tmpBD)!= 0)goto _LL91;_tmpBE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBD)->f1;if((int)_tmpBE != 0)goto _LL91;_LL90: return t1;_LL91: if(_tmpBD <= (void*)
1?1:*((int*)_tmpBD)!= 0)goto _LL93;_tmpBF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBD)->f1;if(_tmpBF <= (void*)1?1:*((int*)_tmpBF)!= 0)goto _LL93;_tmpC0=((struct
Cyc_Absyn_Upper_b_struct*)_tmpBF)->f1;_LL92: {struct Cyc_Absyn_PointerType_struct*
_tmpC1=({struct Cyc_Absyn_PointerType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));
_tmpC2[0]=({struct Cyc_Absyn_PointerType_struct _tmpC3;_tmpC3.tag=4;_tmpC3.f1=({
struct Cyc_Absyn_PtrInfo _tmpC4;_tmpC4.elt_typ=(void*)_tmpB5;_tmpC4.rgn_typ=(void*)
_tmpB6;_tmpC4.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
1);_tmpC4.tq=_tmpB8;_tmpC4.bounds=Cyc_Absyn_new_conref((void*)0);_tmpC4;});
_tmpC3;});_tmpC2;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpC1);return(void*)
_tmpC1;}_LL93:;_LL94:(void*)(_tmpB9->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmpC6;_tmpC6.tag=0;_tmpC6.f1=(void*)((void*)0);_tmpC6;});_tmpC5;})));return t1;
_LL8E:;}_LL8C:;_LL8D: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LL89:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=
0;_tmpC9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct Cyc_Std_String_pa_struct _tmpC8;_tmpC8.tag=0;
_tmpC8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmpC7[2]={& _tmpC8,& _tmpC9};Cyc_Tcutil_terr(e1->loc,
_tag_arr("pointer arithmetic on values of different types (%s != %s)",sizeof(
char),59),_tag_arr(_tmpC7,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpCA[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(char),50),_tag_arr(_tmpCA,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmpCC;
_tmpCC.tag=0;_tmpCC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmpCB[2]={& _tmpCC,& _tmpCD};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting either %s or int but found %s",
sizeof(char),40),_tag_arr(_tmpCB,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}else{return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num?e2_is_num: 0)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2)?Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*
_tmpCE));_tmpCE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCE;}))): 0)return Cyc_Absyn_sint_typ;
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1)?1: Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{{
struct _tuple0 _tmpD0=({struct _tuple0 _tmpCF;_tmpCF.f1=Cyc_Tcutil_compress(t1);
_tmpCF.f2=Cyc_Tcutil_compress(t2);_tmpCF;});void*_tmpD1;struct Cyc_Absyn_PtrInfo
_tmpD2;void*_tmpD3;void*_tmpD4;struct Cyc_Absyn_PtrInfo _tmpD5;void*_tmpD6;_LL96:
_tmpD1=_tmpD0.f1;if(_tmpD1 <= (void*)3?1:*((int*)_tmpD1)!= 4)goto _LL98;_tmpD2=((
struct Cyc_Absyn_PointerType_struct*)_tmpD1)->f1;_tmpD3=(void*)_tmpD2.elt_typ;
_tmpD4=_tmpD0.f2;if(_tmpD4 <= (void*)3?1:*((int*)_tmpD4)!= 4)goto _LL98;_tmpD5=((
struct Cyc_Absyn_PointerType_struct*)_tmpD4)->f1;_tmpD6=(void*)_tmpD5.elt_typ;
_LL97: if(Cyc_Tcutil_unify(_tmpD3,_tmpD6))return Cyc_Absyn_sint_typ;goto _LL95;
_LL98:;_LL99: goto _LL95;_LL95:;}({struct Cyc_Std_String_pa_struct _tmpD9;_tmpD9.tag=
0;_tmpD9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmpD7[2]={& _tmpD8,& _tmpD9};Cyc_Tcutil_terr(loc,_tag_arr("comparison not allowed between %s and %s",
sizeof(char),41),_tag_arr(_tmpD7,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpDA=_cycalloc(sizeof(*_tmpDA));
_tmpDA->v=Cyc_Tcenv_lookup_type_vars(te);_tmpDA;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpDB=p;_LL9B: if((int)_tmpDB != 0)
goto _LL9D;_LL9C: return Cyc_Tcexp_tcPlus(te,e1,e2);_LL9D: if((int)_tmpDB != 2)goto
_LL9F;_LL9E: return Cyc_Tcexp_tcMinus(te,e1,e2);_LL9F: if((int)_tmpDB != 1)goto _LLA1;
_LLA0: goto _LLA2;_LLA1: if((int)_tmpDB != 3)goto _LLA3;_LLA2: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3: if((int)_tmpDB != 4)goto _LLA5;_LLA4: goto
_LLA6;_LLA5: if((int)_tmpDB != 13)goto _LLA7;_LLA6: goto _LLA8;_LLA7: if((int)_tmpDB != 
14)goto _LLA9;_LLA8: goto _LLAA;_LLA9: if((int)_tmpDB != 15)goto _LLAB;_LLAA: goto _LLAC;
_LLAB: if((int)_tmpDB != 16)goto _LLAD;_LLAC: goto _LLAE;_LLAD: if((int)_tmpDB != 17)
goto _LLAF;_LLAE: goto _LLB0;_LLAF: if((int)_tmpDB != 18)goto _LLB1;_LLB0: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LLB1: if((int)_tmpDB != 5)goto _LLB3;_LLB2: goto
_LLB4;_LLB3: if((int)_tmpDB != 6)goto _LLB5;_LLB4: goto _LLB6;_LLB5: if((int)_tmpDB != 
7)goto _LLB7;_LLB6: goto _LLB8;_LLB7: if((int)_tmpDB != 8)goto _LLB9;_LLB8: goto _LLBA;
_LLB9: if((int)_tmpDB != 9)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if((int)_tmpDB != 10)
goto _LLBD;_LLBC: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLBD:;_LLBE:({void*
_tmpDC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad binary primop",sizeof(char),18),_tag_arr(_tmpDC,sizeof(void*),0));});
_LL9A:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2?((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLBF: return({
void*_tmpDD[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has 0 arguments",
sizeof(char),35),_tag_arr(_tmpDD,sizeof(void*),0));});case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: return({void*_tmpDE[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has > 2 arguments",
sizeof(char),37),_tag_arr(_tmpDE,sizeof(void*),0));});}return t;}}struct _tuple9{
struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){{void*_tmpDF=(void*)e->r;void*_tmpE0;struct Cyc_Absyn_Vardecl*
_tmpE1;void*_tmpE2;struct Cyc_Absyn_Vardecl*_tmpE3;void*_tmpE4;struct Cyc_Absyn_Vardecl*
_tmpE5;void*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;
struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct _tagged_arr*_tmpEB;
struct Cyc_Absyn_Exp*_tmpEC;struct _tagged_arr*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;
struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;_LLC5: if(*((int*)_tmpDF)!= 
1)goto _LLC7;_tmpE0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpDF)->f2;if(_tmpE0
<= (void*)1?1:*((int*)_tmpE0)!= 2)goto _LLC7;_tmpE1=((struct Cyc_Absyn_Param_b_struct*)
_tmpE0)->f1;_LLC6: _tmpE3=_tmpE1;goto _LLC8;_LLC7: if(*((int*)_tmpDF)!= 1)goto _LLC9;
_tmpE2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpDF)->f2;if(_tmpE2 <= (void*)1?1:*((
int*)_tmpE2)!= 3)goto _LLC9;_tmpE3=((struct Cyc_Absyn_Local_b_struct*)_tmpE2)->f1;
_LLC8: _tmpE5=_tmpE3;goto _LLCA;_LLC9: if(*((int*)_tmpDF)!= 1)goto _LLCB;_tmpE4=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpDF)->f2;if(_tmpE4 <= (void*)1?1:*((int*)
_tmpE4)!= 4)goto _LLCB;_tmpE5=((struct Cyc_Absyn_Pat_b_struct*)_tmpE4)->f1;_LLCA:
_tmpE7=_tmpE5;goto _LLCC;_LLCB: if(*((int*)_tmpDF)!= 1)goto _LLCD;_tmpE6=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpDF)->f2;if(_tmpE6 <= (void*)1?1:*((int*)_tmpE6)
!= 0)goto _LLCD;_tmpE7=((struct Cyc_Absyn_Global_b_struct*)_tmpE6)->f1;_LLCC: if(!(
_tmpE7->tq).q_const)return;goto _LLC4;_LLCD: if(*((int*)_tmpDF)!= 23)goto _LLCF;
_tmpE8=((struct Cyc_Absyn_Subscript_e_struct*)_tmpDF)->f1;_tmpE9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpDF)->f2;_LLCE:{void*_tmpF1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE8->topt))->v);struct Cyc_Absyn_PtrInfo _tmpF2;struct Cyc_Absyn_Tqual
_tmpF3;struct Cyc_Absyn_Tqual _tmpF4;struct Cyc_List_List*_tmpF5;_LLDC: if(_tmpF1 <= (
void*)3?1:*((int*)_tmpF1)!= 4)goto _LLDE;_tmpF2=((struct Cyc_Absyn_PointerType_struct*)
_tmpF1)->f1;_tmpF3=_tmpF2.tq;_LLDD: _tmpF4=_tmpF3;goto _LLDF;_LLDE: if(_tmpF1 <= (
void*)3?1:*((int*)_tmpF1)!= 7)goto _LLE0;_tmpF4=((struct Cyc_Absyn_ArrayType_struct*)
_tmpF1)->f2;_LLDF: if(!_tmpF4.q_const)return;goto _LLDB;_LLE0: if(_tmpF1 <= (void*)3?
1:*((int*)_tmpF1)!= 9)goto _LLE2;_tmpF5=((struct Cyc_Absyn_TupleType_struct*)
_tmpF1)->f1;_LLE1: {unsigned int _tmpF7;int _tmpF8;struct _tuple7 _tmpF6=Cyc_Evexp_eval_const_uint_exp(
_tmpE9);_tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;if(!_tmpF8){({void*_tmpF9[0]={};Cyc_Tcutil_terr(
e->loc,_tag_arr("tuple projection cannot use sizeof or offsetof",sizeof(char),47),
_tag_arr(_tmpF9,sizeof(void*),0));});return;}{struct _handler_cons _tmpFA;
_push_handler(& _tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler))_tmpFC=1;if(!
_tmpFC){{struct _tuple9 _tmpFE;struct Cyc_Absyn_Tqual _tmpFF;struct _tuple9*_tmpFD=((
struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF5,(int)_tmpF7);
_tmpFE=*_tmpFD;_tmpFF=_tmpFE.f1;if(!_tmpFF.q_const){_npop_handler(0);return;}};
_pop_handler();}else{void*_tmpFB=(void*)_exn_thrown;void*_tmp101=_tmpFB;_LLE5:
if(_tmp101 != Cyc_List_Nth)goto _LLE7;_LLE6: return;_LLE7:;_LLE8:(void)_throw(
_tmp101);_LLE4:;}}}goto _LLDB;}_LLE2:;_LLE3: goto _LLDB;_LLDB:;}goto _LLC4;_LLCF: if(*((
int*)_tmpDF)!= 21)goto _LLD1;_tmpEA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDF)->f1;
_tmpEB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDF)->f2;_LLD0:{void*_tmp102=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEA->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp103;void*_tmp104;struct Cyc_Absyn_Aggrdecl**_tmp105;
struct Cyc_List_List*_tmp106;_LLEA: if(_tmp102 <= (void*)3?1:*((int*)_tmp102)!= 10)
goto _LLEC;_tmp103=((struct Cyc_Absyn_AggrType_struct*)_tmp102)->f1;_tmp104=(void*)
_tmp103.aggr_info;if(*((int*)_tmp104)!= 1)goto _LLEC;_tmp105=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp104)->f1;_LLEB: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp105 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp105,_tmpEB);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLE9;}_LLEC: if(_tmp102 <= (void*)3?1:*((int*)_tmp102)!= 11)goto _LLEE;
_tmp106=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp102)->f2;_LLED: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp106,_tmpEB);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: if(*((int*)_tmpDF)!= 
22)goto _LLD3;_tmpEC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDF)->f1;_tmpED=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDF)->f2;_LLD2:{void*_tmp107=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEC->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp108;void*_tmp109;_LLF1: if(_tmp107 <= (void*)3?1:*((int*)_tmp107)!= 4)goto
_LLF3;_tmp108=((struct Cyc_Absyn_PointerType_struct*)_tmp107)->f1;_tmp109=(void*)
_tmp108.elt_typ;_LLF2:{void*_tmp10A=Cyc_Tcutil_compress(_tmp109);struct Cyc_Absyn_AggrInfo
_tmp10B;void*_tmp10C;struct Cyc_Absyn_Aggrdecl**_tmp10D;struct Cyc_List_List*
_tmp10E;_LLF6: if(_tmp10A <= (void*)3?1:*((int*)_tmp10A)!= 10)goto _LLF8;_tmp10B=((
struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1;_tmp10C=(void*)_tmp10B.aggr_info;
if(*((int*)_tmp10C)!= 1)goto _LLF8;_tmp10D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp10C)->f1;_LLF7: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp10D == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp10D,_tmpED);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLF5;}_LLF8: if(_tmp10A <= (void*)3?1:*((int*)_tmp10A)!= 11)goto _LLFA;
_tmp10E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp10A)->f2;_LLF9: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp10E,_tmpED);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto _LLF0;_LLF0:;}
goto _LLC4;_LLD3: if(*((int*)_tmpDF)!= 20)goto _LLD5;_tmpEE=((struct Cyc_Absyn_Deref_e_struct*)
_tmpDF)->f1;_LLD4:{void*_tmp10F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE->topt))->v);struct Cyc_Absyn_PtrInfo _tmp110;struct Cyc_Absyn_Tqual
_tmp111;struct Cyc_Absyn_Tqual _tmp112;_LLFD: if(_tmp10F <= (void*)3?1:*((int*)
_tmp10F)!= 4)goto _LLFF;_tmp110=((struct Cyc_Absyn_PointerType_struct*)_tmp10F)->f1;
_tmp111=_tmp110.tq;_LLFE: _tmp112=_tmp111;goto _LL100;_LLFF: if(_tmp10F <= (void*)3?
1:*((int*)_tmp10F)!= 7)goto _LL101;_tmp112=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10F)->f2;_LL100: if(!_tmp112.q_const)return;goto _LLFC;_LL101:;_LL102: goto
_LLFC;_LLFC:;}goto _LLC4;_LLD5: if(*((int*)_tmpDF)!= 11)goto _LLD7;_tmpEF=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmpDF)->f1;_LLD6: _tmpF0=_tmpEF;goto _LLD8;
_LLD7: if(*((int*)_tmpDF)!= 12)goto _LLD9;_tmpF0=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpDF)->f1;_LLD8: Cyc_Tcexp_check_writable(te,_tmpF0);return;_LLD9:;_LLDA: goto
_LLC4;_LLC4:;}({struct Cyc_Std_String_pa_struct _tmp114;_tmp114.tag=0;_tmp114.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp113[1]={& _tmp114};Cyc_Tcutil_terr(
e->loc,_tag_arr("attempt to write a const location: %s",sizeof(char),38),
_tag_arr(_tmp113,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp115[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(char),34),_tag_arr(_tmp115,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt)){if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp116[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("can't perform arithmetic on abstract pointer type",sizeof(char),50),
_tag_arr(_tmp116,sizeof(void*),0));});}else{({struct Cyc_Std_String_pa_struct
_tmp118;_tmp118.tag=0;_tmp118.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp117[1]={& _tmp118};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting arithmetic or ? type but found %s",
sizeof(char),44),_tag_arr(_tmp117,sizeof(void*),1));}});}}return t;}}static void*
Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(char),23));Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp120;}));struct Cyc_List_List _tmp119=({struct
Cyc_List_List _tmp11F;_tmp11F.hd=e3;_tmp11F.tl=0;_tmp11F;});struct Cyc_List_List
_tmp11A=({struct Cyc_List_List _tmp11E;_tmp11E.hd=e2;_tmp11E.tl=(struct Cyc_List_List*)&
_tmp119;_tmp11E;});if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp11A)){({struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v);{struct Cyc_Std_String_pa_struct _tmp11C;_tmp11C.tag=0;
_tmp11C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmp11B[2]={& _tmp11C,& _tmp11D};Cyc_Tcutil_terr(
loc,_tag_arr("conditional clause types do not match: %s != %s",sizeof(char),48),
_tag_arr(_tmp11B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp121=Cyc_Tcutil_compress(t1);_LL104: if(_tmp121 <= (void*)3?1:*((int*)
_tmp121)!= 7)goto _LL106;_LL105:({void*_tmp122[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",
sizeof(char),26),_tag_arr(_tmp122,sizeof(void*),0));});goto _LL103;_LL106:;_LL107:
goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1))({
void*_tmp123[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(char),41),_tag_arr(_tmp123,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e1))return({void*_tmp124[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(char),25),_tag_arr(_tmp124,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp125=({struct
Cyc_Std_String_pa_struct _tmp128;_tmp128.tag=0;_tmp128.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp126[2]={& _tmp127,& _tmp128};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type mismatch: %s != %s",sizeof(char),24),_tag_arr(_tmp126,
sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp125;}}else{void*_tmp129=(void*)po->v;void*_tmp12A=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp129,e1,e2);if(!(Cyc_Tcutil_unify(_tmp12A,t1)?1:(Cyc_Tcutil_coerceable(
_tmp12A)?Cyc_Tcutil_coerceable(t1): 0))){void*_tmp12B=({struct Cyc_Std_String_pa_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp12C[2]={& _tmp12D,& _tmp12E};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(char),82),_tag_arr(_tmp12C,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp12A,t1);Cyc_Tcutil_explain_failure();return _tmp12B;}return _tmp12A;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct dbl={6,0};static
void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;{void*_tmp12F=Cyc_Tcutil_compress(t1);void*_tmp130;void*
_tmp131;_LL109: if((int)_tmp12F != 1)goto _LL10B;_LL10A: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ);t1=dbl_typ;goto _LL108;_LL10B: if(_tmp12F <= (void*)3?1:*((int*)
_tmp12F)!= 5)goto _LL10D;_tmp130=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp12F)->f2;
if((int)_tmp130 != 0)goto _LL10D;_LL10C: goto _LL10E;_LL10D: if(_tmp12F <= (void*)3?1:*((
int*)_tmp12F)!= 5)goto _LL10F;_tmp131=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp12F)->f2;if((int)_tmp131 != 1)goto _LL10F;_LL10E: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ);t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;
_LL108:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp133;
struct Cyc_List_List*_tmp134;struct Cyc_Position_Segment*_tmp135;void*_tmp136;
struct Cyc_Absyn_Tunionfield _tmp132=*((struct Cyc_Absyn_Tunionfield*)fields->hd);
_tmp133=_tmp132.name;_tmp134=_tmp132.typs;_tmp135=_tmp132.loc;_tmp136=(void*)
_tmp132.sc;if(_tmp134 == 0?1: _tmp134->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)_tmp134->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(
fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp138;struct Cyc_List_List*
_tmp139;struct Cyc_Position_Segment*_tmp13A;void*_tmp13B;struct Cyc_Absyn_Tunionfield
_tmp137=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp138=_tmp137.name;_tmp139=
_tmp137.typs;_tmp13A=_tmp137.loc;_tmp13B=(void*)_tmp137.sc;if(_tmp139 == 0?1:
_tmp139->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp139->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}({struct Cyc_Std_String_pa_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
struct Cyc_Std_String_pa_struct _tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(tu);{void*_tmp13C[2]={& _tmp13D,& _tmp13E};Cyc_Tcutil_terr(
e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",sizeof(
char),54),_tag_arr(_tmp13C,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp140=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp141=t;struct Cyc_Absyn_PtrInfo _tmp142;void*_tmp143;void*_tmp144;struct
Cyc_Absyn_Conref*_tmp145;struct Cyc_Absyn_Tqual _tmp146;struct Cyc_Absyn_Conref*
_tmp147;_LL112: if(_tmp141 <= (void*)3?1:*((int*)_tmp141)!= 4)goto _LL114;_tmp142=((
struct Cyc_Absyn_PointerType_struct*)_tmp141)->f1;_tmp143=(void*)_tmp142.elt_typ;
_tmp144=(void*)_tmp142.rgn_typ;_tmp145=_tmp142.nullable;_tmp146=_tmp142.tq;
_tmp147=_tmp142.bounds;_LL113: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp144);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp147);{void*_tmp148=Cyc_Tcutil_compress(_tmp143);struct Cyc_Absyn_FnInfo
_tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_Core_Opt*_tmp14B;void*_tmp14C;
struct Cyc_List_List*_tmp14D;int _tmp14E;struct Cyc_Absyn_VarargInfo*_tmp14F;struct
Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;_LL117: if(_tmp148 <= (void*)3?1:*((
int*)_tmp148)!= 8)goto _LL119;_tmp149=((struct Cyc_Absyn_FnType_struct*)_tmp148)->f1;
_tmp14A=_tmp149.tvars;_tmp14B=_tmp149.effect;_tmp14C=(void*)_tmp149.ret_typ;
_tmp14D=_tmp149.args;_tmp14E=_tmp149.c_varargs;_tmp14F=_tmp149.cyc_varargs;
_tmp150=_tmp149.rgn_po;_tmp151=_tmp149.attributes;_LL118: if(topt != 0)Cyc_Tcutil_unify(
_tmp14C,*topt);while(_tmp140 != 0?_tmp14D != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp140->hd;void*t2=(*((struct _tuple2*)_tmp14D->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp152[2]={& _tmp153,& _tmp154};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(char),54),_tag_arr(_tmp152,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp140=_tmp140->tl;_tmp14D=_tmp14D->tl;}if(_tmp14D != 0)({void*_tmp155[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too few arguments for function",sizeof(char),31),_tag_arr(_tmp155,
sizeof(void*),0));});else{if((_tmp140 != 0?1: _tmp14E)?1: _tmp14F != 0){if(_tmp14E)
for(0;_tmp140 != 0;_tmp140=_tmp140->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp140->hd);}else{if(_tmp14F == 0)({void*_tmp156[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("too many arguments for function",sizeof(char),32),_tag_arr(_tmp156,
sizeof(void*),0));});else{void*_tmp158;int _tmp159;struct Cyc_Absyn_VarargInfo
_tmp157=*_tmp14F;_tmp158=(void*)_tmp157.type;_tmp159=_tmp157.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp15A=({struct Cyc_Absyn_VarargCallInfo*_tmp16E=_cycalloc(sizeof(*_tmp16E));
_tmp16E->num_varargs=0;_tmp16E->injectors=0;_tmp16E->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp14F);_tmp16E;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp15A;if(!_tmp159)for(0;_tmp140 != 0;_tmp140=_tmp140->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp140->hd;_tmp15A->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp158,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp158)){({struct Cyc_Std_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp158);{void*_tmp15B[2]={& _tmp15C,& _tmp15D};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(char),49),_tag_arr(_tmp15B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp15E=Cyc_Tcutil_compress(_tmp158);struct Cyc_Absyn_TunionInfo _tmp15F;
void*_tmp160;struct Cyc_Absyn_Tuniondecl**_tmp161;struct Cyc_Absyn_Tuniondecl*
_tmp162;struct Cyc_List_List*_tmp163;void*_tmp164;_LL11C: if(_tmp15E <= (void*)3?1:*((
int*)_tmp15E)!= 2)goto _LL11E;_tmp15F=((struct Cyc_Absyn_TunionType_struct*)
_tmp15E)->f1;_tmp160=(void*)_tmp15F.tunion_info;if(*((int*)_tmp160)!= 1)goto
_LL11E;_tmp161=((struct Cyc_Absyn_KnownTunion_struct*)_tmp160)->f1;_tmp162=*
_tmp161;_tmp163=_tmp15F.targs;_tmp164=(void*)_tmp15F.rgn;_LL11D: {struct Cyc_Absyn_Tuniondecl*
_tmp165=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp162->name);struct Cyc_List_List*
fields=0;if(_tmp165->fields == 0)({struct Cyc_Std_String_pa_struct _tmp167;_tmp167.tag=
0;_tmp167.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp158);{void*_tmp166[1]={&
_tmp167};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",sizeof(char),21),
_tag_arr(_tmp166,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp165->fields))->v;}if(!Cyc_Tcutil_unify(
_tmp164,Cyc_Tcenv_curr_rgn(te)))({void*_tmp168[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bad region for injected varargs",sizeof(char),32),_tag_arr(_tmp168,
sizeof(void*),0));});{struct _RegionHandle _tmp169=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp169;_push_region(rgn);{struct Cyc_List_List*_tmp16A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp165->tvs,_tmp163);for(0;
_tmp140 != 0;_tmp140=_tmp140->tl){_tmp15A->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp140->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp16B=Cyc_Tcexp_tcInjection(te,e1,_tmp158,rgn,_tmp16A,fields);if(_tmp16B != 0)
_tmp15A->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp15A->injectors,({struct Cyc_List_List*_tmp16C=_cycalloc(
sizeof(*_tmp16C));_tmp16C->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp16B);
_tmp16C->tl=0;_tmp16C;}));}}}};_pop_region(rgn);}goto _LL11B;}_LL11E:;_LL11F:({
void*_tmp16D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
char),23),_tag_arr(_tmp16D,sizeof(void*),0));});goto _LL11B;_LL11B:;}}}}}}{struct
Cyc_List_List*a=_tmp151;for(0;a != 0;a=a->tl){void*_tmp16F=(void*)a->hd;void*
_tmp170;int _tmp171;int _tmp172;_LL121: if(_tmp16F <= (void*)16?1:*((int*)_tmp16F)!= 
3)goto _LL123;_tmp170=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp16F)->f1;
_tmp171=((struct Cyc_Absyn_Format_att_struct*)_tmp16F)->f2;_tmp172=((struct Cyc_Absyn_Format_att_struct*)
_tmp16F)->f3;_LL122:{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int
_tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){{struct Cyc_Absyn_Exp*
_tmp176=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp171 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp172 == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp172 - 1);_tmp177;});}{
void*_tmp178=_tmp170;_LL126: if((int)_tmp178 != 0)goto _LL128;_LL127: Cyc_Tcexp_check_format_args(
te,_tmp176,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL125;_LL128: if((int)
_tmp178 != 1)goto _LL125;_LL129: Cyc_Tcexp_check_format_args(te,_tmp176,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL125;_LL125:;}};_pop_handler();}else{void*
_tmp174=(void*)_exn_thrown;void*_tmp17A=_tmp174;_LL12B: if(_tmp17A != Cyc_List_Nth)
goto _LL12D;_LL12C:({void*_tmp17B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",
sizeof(char),21),_tag_arr(_tmp17B,sizeof(void*),0));});goto _LL12A;_LL12D:;_LL12E:(
void)_throw(_tmp17A);_LL12A:;}}}goto _LL120;_LL123:;_LL124: goto _LL120;_LL120:;}}
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp14B))->v);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp150);
return _tmp14C;_LL119:;_LL11A: return({void*_tmp17C[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("expected pointer to function",sizeof(char),29),_tag_arr(_tmp17C,
sizeof(void*),0));});_LL116:;}_LL114:;_LL115: return({void*_tmp17D[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(char),29),_tag_arr(_tmp17D,
sizeof(void*),0));});_LL111:;}}static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({
struct Cyc_Std_String_pa_struct _tmp17F;_tmp17F.tag=0;_tmp17F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp17E[1]={& _tmp17F};Cyc_Tcutil_terr(loc,_tag_arr("expected xtunion exn but found %s",
sizeof(char),34),_tag_arr(_tmp17E,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp180;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp181=t1;struct Cyc_Absyn_PtrInfo _tmp182;void*_tmp183;void*_tmp184;struct
Cyc_Absyn_Conref*_tmp185;struct Cyc_Absyn_Tqual _tmp186;struct Cyc_Absyn_Conref*
_tmp187;_LL130: if(_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 4)goto _LL132;_tmp182=((
struct Cyc_Absyn_PointerType_struct*)_tmp181)->f1;_tmp183=(void*)_tmp182.elt_typ;
_tmp184=(void*)_tmp182.rgn_typ;_tmp185=_tmp182.nullable;_tmp186=_tmp182.tq;
_tmp187=_tmp182.bounds;_LL131:{void*_tmp188=Cyc_Tcutil_compress(_tmp183);struct
Cyc_Absyn_FnInfo _tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_Core_Opt*_tmp18B;
void*_tmp18C;struct Cyc_List_List*_tmp18D;int _tmp18E;struct Cyc_Absyn_VarargInfo*
_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_List_List*_tmp191;_LL135: if(
_tmp188 <= (void*)3?1:*((int*)_tmp188)!= 8)goto _LL137;_tmp189=((struct Cyc_Absyn_FnType_struct*)
_tmp188)->f1;_tmp18A=_tmp189.tvars;_tmp18B=_tmp189.effect;_tmp18C=(void*)_tmp189.ret_typ;
_tmp18D=_tmp189.args;_tmp18E=_tmp189.c_varargs;_tmp18F=_tmp189.cyc_varargs;
_tmp190=_tmp189.rgn_po;_tmp191=_tmp189.attributes;_LL136: {struct Cyc_List_List*
instantiation=0;for(0;ts != 0?_tmp18A != 0: 0;(ts=ts->tl,_tmp18A=_tmp18A->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp18A->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=({struct _tuple5*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->f1=(struct Cyc_Absyn_Tvar*)_tmp18A->hd;
_tmp193->f2=(void*)ts->hd;_tmp193;});_tmp192->tl=instantiation;_tmp192;});}if(ts
!= 0)return({void*_tmp194[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too many type variables in instantiation",
sizeof(char),41),_tag_arr(_tmp194,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp198=_cycalloc(sizeof(*
_tmp198));_tmp198[0]=({struct Cyc_Absyn_FnType_struct _tmp199;_tmp199.tag=8;
_tmp199.f1=({struct Cyc_Absyn_FnInfo _tmp19A;_tmp19A.tvars=_tmp18A;_tmp19A.effect=
_tmp18B;_tmp19A.ret_typ=(void*)_tmp18C;_tmp19A.args=_tmp18D;_tmp19A.c_varargs=
_tmp18E;_tmp19A.cyc_varargs=_tmp18F;_tmp19A.rgn_po=_tmp190;_tmp19A.attributes=
_tmp191;_tmp19A;});_tmp199;});_tmp198;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195[0]=({struct Cyc_Absyn_PointerType_struct
_tmp196;_tmp196.tag=4;_tmp196.f1=({struct Cyc_Absyn_PtrInfo _tmp197;_tmp197.elt_typ=(
void*)new_fn_typ;_tmp197.rgn_typ=(void*)_tmp184;_tmp197.nullable=_tmp185;_tmp197.tq=
_tmp186;_tmp197.bounds=_tmp187;_tmp197;});_tmp196;});_tmp195;});}}_LL137:;_LL138:
goto _LL134;_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}return({struct
Cyc_Std_String_pa_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp19B[1]={& _tmp19C};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(char),40),_tag_arr(_tmp19B,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(te,loc,t2,t)?!Cyc_Tcutil_castable(
te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp19D=({struct Cyc_Std_String_pa_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp19F;
_tmp19F.tag=0;_tmp19F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*
_tmp19E[2]={& _tmp19F,& _tmp1A0};Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",
sizeof(char),21),_tag_arr(_tmp19E,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp19D;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp1A1=0;struct Cyc_Absyn_Tqual _tmp1A2=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp1A3=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1A4;
void*_tmp1A5;struct Cyc_Absyn_Tqual _tmp1A6;_LL13A: if(_tmp1A3 <= (void*)3?1:*((int*)
_tmp1A3)!= 4)goto _LL13C;_tmp1A4=((struct Cyc_Absyn_PointerType_struct*)_tmp1A3)->f1;
_tmp1A5=(void*)_tmp1A4.elt_typ;_tmp1A6=_tmp1A4.tq;_LL13B: _tmp1A1=({void**_tmp1A7=
_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=_tmp1A5;_tmp1A7;});_tmp1A2=_tmp1A6;goto
_LL139;_LL13C:;_LL13D: goto _LL139;_LL139:;}Cyc_Tcexp_tcExpNoInst(te,_tmp1A1,e);{
void*_tmp1A8=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;
_LL13F: if(*((int*)_tmp1A8)!= 28)goto _LL141;_LL140: goto _LL142;_LL141: if(*((int*)
_tmp1A8)!= 24)goto _LL143;_LL142:({void*_tmp1AB[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("& used to allocate",sizeof(char),19),_tag_arr(_tmp1AB,sizeof(void*),0));});{
void*(*_tmp1AC)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;if(
topt != 0){void*_tmp1AD=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1AE;
struct Cyc_Absyn_Conref*_tmp1AF;_LL148: if(_tmp1AD <= (void*)3?1:*((int*)_tmp1AD)!= 
4)goto _LL14A;_tmp1AE=((struct Cyc_Absyn_PointerType_struct*)_tmp1AD)->f1;_tmp1AF=
_tmp1AE.nullable;_LL149: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp1AF))_tmp1AC=Cyc_Absyn_star_typ;goto _LL147;_LL14A:;_LL14B: goto _LL147;_LL147:;}
return _tmp1AC((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp1A2);}_LL143: if(*((int*)_tmp1A8)!= 23)goto _LL145;_tmp1A9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1A8)->f1;_tmp1AA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A8)->f2;_LL144:{
void*_tmp1B0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1A9->topt))->v);_LL14D: if(_tmp1B0 <= (void*)3?1:*((int*)_tmp1B0)!= 9)goto
_LL14F;_LL14E: goto _LL14C;_LL14F:;_LL150:(void*)(e0->r=(void*)((void*)(Cyc_Absyn_add_exp(
_tmp1A9,_tmp1AA,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp1A9,_tmp1AA);_LL14C:;}
goto _LL13E;_LL145:;_LL146: goto _LL13E;_LL13E:;}{int _tmp1B2;void*_tmp1B3;struct
_tuple6 _tmp1B1=Cyc_Tcutil_addressof_props(te,e);_tmp1B2=_tmp1B1.f1;_tmp1B3=
_tmp1B1.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp1B2)tq.q_const=
1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp1B3,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_Std_String_pa_struct _tmp1B5;_tmp1B5.tag=0;_tmp1B5.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1B4[1]={& _tmp1B5};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
char),55),_tag_arr(_tmp1B4,sizeof(void*),1));}});if(topt != 0){void*_tmp1B6=Cyc_Tcutil_compress(*
topt);_LL152: if(_tmp1B6 <= (void*)3?1:*((int*)_tmp1B6)!= 14)goto _LL154;_LL153:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1B7=_cycalloc(sizeof(*
_tmp1B7));_tmp1B7[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1B8;_tmp1B8.tag=14;
_tmp1B8.f1=(void*)t;_tmp1B8;});_tmp1B7;});_LL154:;_LL155: goto _LL151;_LL151:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_Std_strcmp(*n,*sf->name)== 0;}static void*
Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);{void*_tmp1B9=n;struct _tagged_arr*_tmp1BA;unsigned int _tmp1BB;
_LL157: if(*((int*)_tmp1B9)!= 0)goto _LL159;_tmp1BA=((struct Cyc_Absyn_StructField_struct*)
_tmp1B9)->f1;_LL158: {int bad_type=1;{void*_tmp1BC=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Aggrdecl**_tmp1BF;struct
Cyc_List_List*_tmp1C0;_LL15C: if(_tmp1BC <= (void*)3?1:*((int*)_tmp1BC)!= 10)goto
_LL15E;_tmp1BD=((struct Cyc_Absyn_AggrType_struct*)_tmp1BC)->f1;_tmp1BE=(void*)
_tmp1BD.aggr_info;if(*((int*)_tmp1BE)!= 1)goto _LL15E;_tmp1BF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1BE)->f1;_LL15D: if((*_tmp1BF)->impl == 0)goto _LL15B;if(!((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp1BA,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp1BF)->impl))->fields))({struct Cyc_Std_String_pa_struct _tmp1C2;
_tmp1C2.tag=0;_tmp1C2.f1=(struct _tagged_arr)*_tmp1BA;{void*_tmp1C1[1]={& _tmp1C2};
Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",sizeof(char),
37),_tag_arr(_tmp1C1,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL15E: if(
_tmp1BC <= (void*)3?1:*((int*)_tmp1BC)!= 11)goto _LL160;_tmp1C0=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BC)->f2;_LL15F: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BA,_tmp1C0))({struct Cyc_Std_String_pa_struct _tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=(
struct _tagged_arr)*_tmp1BA;{void*_tmp1C3[1]={& _tmp1C4};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct/union has name %s",sizeof(char),37),_tag_arr(
_tmp1C3,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL160:;_LL161: goto _LL15B;
_LL15B:;}if(bad_type)({struct Cyc_Std_String_pa_struct _tmp1C6;_tmp1C6.tag=0;
_tmp1C6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1C5[1]={&
_tmp1C6};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(char),36),_tag_arr(_tmp1C5,sizeof(void*),1));}});goto _LL156;}_LL159: if(*((
int*)_tmp1B9)!= 1)goto _LL156;_tmp1BB=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp1B9)->f1;_LL15A: {int bad_type=1;{void*_tmp1C7=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Aggrdecl**_tmp1CA;struct
Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_Absyn_TunionFieldInfo
_tmp1CD;void*_tmp1CE;struct Cyc_Absyn_Tunionfield*_tmp1CF;_LL163: if(_tmp1C7 <= (
void*)3?1:*((int*)_tmp1C7)!= 10)goto _LL165;_tmp1C8=((struct Cyc_Absyn_AggrType_struct*)
_tmp1C7)->f1;_tmp1C9=(void*)_tmp1C8.aggr_info;if(*((int*)_tmp1C9)!= 1)goto _LL165;
_tmp1CA=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1C9)->f1;_LL164: if((*_tmp1CA)->impl
== 0)goto _LL162;_tmp1CB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp1CA)->impl))->fields;
goto _LL166;_LL165: if(_tmp1C7 <= (void*)3?1:*((int*)_tmp1C7)!= 11)goto _LL167;
_tmp1CB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C7)->f2;_LL166: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1CB)<= _tmp1BB)({struct Cyc_Std_Int_pa_struct
_tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=(int)_tmp1BB;{struct Cyc_Std_Int_pa_struct
_tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1CB));{void*_tmp1D0[2]={& _tmp1D1,& _tmp1D2};Cyc_Tcutil_terr(
loc,_tag_arr("struct/union has too few components: %d <= %d",sizeof(char),46),
_tag_arr(_tmp1D0,sizeof(void*),2));}}});bad_type=0;goto _LL162;_LL167: if(_tmp1C7
<= (void*)3?1:*((int*)_tmp1C7)!= 9)goto _LL169;_tmp1CC=((struct Cyc_Absyn_TupleType_struct*)
_tmp1C7)->f1;_LL168: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CC)
<= _tmp1BB)({struct Cyc_Std_Int_pa_struct _tmp1D5;_tmp1D5.tag=1;_tmp1D5.f1=(int)
_tmp1BB;{struct Cyc_Std_Int_pa_struct _tmp1D4;_tmp1D4.tag=1;_tmp1D4.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CC));{void*
_tmp1D3[2]={& _tmp1D4,& _tmp1D5};Cyc_Tcutil_terr(loc,_tag_arr("tuple has too few components: %d <= %d",
sizeof(char),39),_tag_arr(_tmp1D3,sizeof(void*),2));}}});bad_type=0;goto _LL162;
_LL169: if(_tmp1C7 <= (void*)3?1:*((int*)_tmp1C7)!= 3)goto _LL16B;_tmp1CD=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp1C7)->f1;_tmp1CE=(void*)_tmp1CD.field_info;
if(*((int*)_tmp1CE)!= 1)goto _LL16B;_tmp1CF=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1CE)->f2;_LL16A: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CF->typs)
< _tmp1BB)({struct Cyc_Std_Int_pa_struct _tmp1D8;_tmp1D8.tag=1;_tmp1D8.f1=(int)
_tmp1BB;{struct Cyc_Std_Int_pa_struct _tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CF->typs));{
void*_tmp1D6[2]={& _tmp1D7,& _tmp1D8};Cyc_Tcutil_terr(loc,_tag_arr("tunionfield has too few components: %d < %d",
sizeof(char),44),_tag_arr(_tmp1D6,sizeof(void*),2));}}});bad_type=0;goto _LL162;
_LL16B:;_LL16C: goto _LL162;_LL162:;}if(bad_type)({struct Cyc_Std_String_pa_struct
_tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1D9[1]={& _tmp1DA};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known type",
sizeof(char),23),_tag_arr(_tmp1D9,sizeof(void*),1));}});goto _LL156;}_LL156:;}
return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1DB=t;struct Cyc_Absyn_PtrInfo _tmp1DC;void*_tmp1DD;void*_tmp1DE;struct
Cyc_Absyn_Conref*_tmp1DF;_LL16E: if(_tmp1DB <= (void*)3?1:*((int*)_tmp1DB)!= 4)
goto _LL170;_tmp1DC=((struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;_tmp1DD=(
void*)_tmp1DC.elt_typ;_tmp1DE=(void*)_tmp1DC.rgn_typ;_tmp1DF=_tmp1DC.bounds;
_LL16F: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1DE);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1DF);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1DD),(void*)1))({
void*_tmp1E0[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(char),40),_tag_arr(_tmp1E0,sizeof(void*),0));});return _tmp1DD;_LL170:;
_LL171: return({struct Cyc_Std_String_pa_struct _tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1E1[1]={& _tmp1E2};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting * or @ type but found %s",sizeof(char),35),_tag_arr(
_tmp1E1,sizeof(void*),1));}});_LL16D:;}}static void*Cyc_Tcexp_tcAggrMember(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp1E3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Aggrdecl**_tmp1E6;
struct Cyc_Absyn_Aggrdecl*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*
_tmp1E9;_LL173: if(_tmp1E3 <= (void*)3?1:*((int*)_tmp1E3)!= 10)goto _LL175;_tmp1E4=((
struct Cyc_Absyn_AggrType_struct*)_tmp1E3)->f1;_tmp1E5=(void*)_tmp1E4.aggr_info;
if(*((int*)_tmp1E5)!= 1)goto _LL175;_tmp1E6=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1E5)->f1;_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E4.targs;_LL174: {struct Cyc_Absyn_Aggrfield*
_tmp1EA=Cyc_Absyn_lookup_decl_field(_tmp1E7,f);if(_tmp1EA == 0)return({struct Cyc_Std_String_pa_struct
_tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1E7->name);{void*_tmp1EB[2]={& _tmp1EC,& _tmp1ED};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("type %s has no %s field",sizeof(char),24),_tag_arr(_tmp1EB,sizeof(void*),
2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E7->impl))->exist_vars
!= 0)return({void*_tmp1EE[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(char),64),_tag_arr(_tmp1EE,sizeof(void*),0));});{void*t2;{struct
_RegionHandle _tmp1EF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1EF;
_push_region(rgn);{struct Cyc_List_List*_tmp1F0=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1E7->tvs,_tmp1E8);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp1F0,(void*)_tmp1EA->type);};_pop_region(rgn);}return t2;}}_LL175: if(_tmp1E3 <= (
void*)3?1:*((int*)_tmp1E3)!= 11)goto _LL177;_tmp1E9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1E3)->f2;_LL176: {struct Cyc_Absyn_Aggrfield*_tmp1F1=Cyc_Absyn_lookup_field(
_tmp1E9,f);if(_tmp1F1 == 0)return({struct Cyc_Std_String_pa_struct _tmp1F3;_tmp1F3.tag=
0;_tmp1F3.f1=(struct _tagged_arr)*f;{void*_tmp1F2[1]={& _tmp1F3};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(char),21),_tag_arr(_tmp1F2,sizeof(
void*),1));}});return(void*)_tmp1F1->type;}_LL177: if(_tmp1E3 <= (void*)3?1:*((int*)
_tmp1E3)!= 7)goto _LL179;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(char),5))
== 0))goto _LL179;_LL178: goto _LL17A;_LL179: if(_tmp1E3 <= (void*)3?1:*((int*)
_tmp1E3)!= 4)goto _LL17B;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(char),5))
== 0))goto _LL17B;_LL17A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp1F5;_tmp1F5.tag=3;_tmp1F5.f1=(void*)((void*)19);_tmp1F5.f2=({struct Cyc_List_List*
_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->hd=e;_tmp1F6->tl=0;_tmp1F6;});
_tmp1F5;});_tmp1F4;})));return Cyc_Absyn_uint_typ;_LL17B:;_LL17C: if(Cyc_Std_zstrcmp(*
f,_tag_arr("size",sizeof(char),5))== 0)return({struct Cyc_Std_String_pa_struct
_tmp1F8;_tmp1F8.tag=0;_tmp1F8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F7[1]={& _tmp1F8};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct, union, or array, found %s",sizeof(char),44),
_tag_arr(_tmp1F7,sizeof(void*),1));}});else{return({struct Cyc_Std_String_pa_struct
_tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F9[1]={& _tmp1FA};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct or union, found %s",sizeof(char),36),_tag_arr(
_tmp1F9,sizeof(void*),1));}});}_LL172:;}}static void*Cyc_Tcexp_tcAggrArrow(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp1FB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1FC;void*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Conref*_tmp1FF;_LL17E: if(
_tmp1FB <= (void*)3?1:*((int*)_tmp1FB)!= 4)goto _LL180;_tmp1FC=((struct Cyc_Absyn_PointerType_struct*)
_tmp1FB)->f1;_tmp1FD=(void*)_tmp1FC.elt_typ;_tmp1FE=(void*)_tmp1FC.rgn_typ;
_tmp1FF=_tmp1FC.bounds;_LL17F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FF);{void*
_tmp200=Cyc_Tcutil_compress(_tmp1FD);struct Cyc_Absyn_AggrInfo _tmp201;void*
_tmp202;struct Cyc_Absyn_Aggrdecl**_tmp203;struct Cyc_Absyn_Aggrdecl*_tmp204;
struct Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp206;_LL183: if(_tmp200 <= (
void*)3?1:*((int*)_tmp200)!= 10)goto _LL185;_tmp201=((struct Cyc_Absyn_AggrType_struct*)
_tmp200)->f1;_tmp202=(void*)_tmp201.aggr_info;if(*((int*)_tmp202)!= 1)goto _LL185;
_tmp203=((struct Cyc_Absyn_KnownAggr_struct*)_tmp202)->f1;_tmp204=*_tmp203;
_tmp205=_tmp201.targs;_LL184: {struct Cyc_Absyn_Aggrfield*_tmp207=Cyc_Absyn_lookup_decl_field(
_tmp204,f);if(_tmp207 == 0)return({struct Cyc_Std_String_pa_struct _tmp20A;_tmp20A.tag=
0;_tmp20A.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp209;
_tmp209.tag=0;_tmp209.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp204->name);{
void*_tmp208[2]={& _tmp209,& _tmp20A};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(char),24),_tag_arr(_tmp208,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp204->impl))->exist_vars != 0)return({void*_tmp20B[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(char),64),_tag_arr(_tmp20B,sizeof(void*),0));});{void*t3;{struct
_RegionHandle _tmp20C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp20C;
_push_region(rgn);{struct Cyc_List_List*_tmp20D=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp204->tvs,_tmp205);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp20D,(void*)_tmp207->type);};_pop_region(rgn);}return t3;}}_LL185: if(_tmp200 <= (
void*)3?1:*((int*)_tmp200)!= 11)goto _LL187;_tmp206=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp200)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp20E=Cyc_Absyn_lookup_field(
_tmp206,f);if(_tmp20E == 0)return({struct Cyc_Std_String_pa_struct _tmp210;_tmp210.tag=
0;_tmp210.f1=(struct _tagged_arr)*f;{void*_tmp20F[1]={& _tmp210};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(char),21),_tag_arr(_tmp20F,sizeof(
void*),1));}});return(void*)_tmp20E->type;}_LL187:;_LL188: goto _LL182;_LL182:;}
goto _LL17D;_LL180:;_LL181: goto _LL17D;_LL17D:;}return({struct Cyc_Std_String_pa_struct
_tmp212;_tmp212.tag=0;_tmp212.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp211[1]={& _tmp212};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(char),35),_tag_arr(
_tmp211,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int _tmp214;int _tmp215;struct _tuple7 _tmp213=Cyc_Evexp_eval_const_uint_exp(
index);_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;if(!_tmp215)return({void*_tmp216[0]={};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("tuple projection cannot use sizeof or offsetof",
sizeof(char),47),_tag_arr(_tmp216,sizeof(void*),0));});{struct _handler_cons
_tmp217;_push_handler(& _tmp217);{int _tmp219=0;if(setjmp(_tmp217.handler))_tmp219=
1;if(!_tmp219){{void*_tmp21A=(*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(ts,(int)_tmp214)).f2;_npop_handler(0);return _tmp21A;};_pop_handler();}
else{void*_tmp218=(void*)_exn_thrown;void*_tmp21C=_tmp218;_LL18A: if(_tmp21C != 
Cyc_List_Nth)goto _LL18C;_LL18B: return({struct Cyc_Std_Int_pa_struct _tmp21F;
_tmp21F.tag=1;_tmp21F.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp21E;_tmp21E.tag=1;_tmp21E.f1=(int)_tmp214;{
void*_tmp21D[2]={& _tmp21E,& _tmp21F};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(char),41),_tag_arr(_tmp21D,sizeof(void*),2));}}});_LL18C:;_LL18D:(void)
_throw(_tmp21C);_LL189:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))return({struct Cyc_Std_String_pa_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp220[1]={& _tmp221};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(char),34),
_tag_arr(_tmp220,sizeof(void*),1));}});{void*_tmp222=t1;struct Cyc_Absyn_PtrInfo
_tmp223;void*_tmp224;void*_tmp225;struct Cyc_Absyn_Tqual _tmp226;struct Cyc_Absyn_Conref*
_tmp227;struct Cyc_List_List*_tmp228;_LL18F: if(_tmp222 <= (void*)3?1:*((int*)
_tmp222)!= 4)goto _LL191;_tmp223=((struct Cyc_Absyn_PointerType_struct*)_tmp222)->f1;
_tmp224=(void*)_tmp223.elt_typ;_tmp225=(void*)_tmp223.rgn_typ;_tmp226=_tmp223.tq;
_tmp227=_tmp223.bounds;_LL190: if(Cyc_Tcutil_is_const_exp(te,e2)){unsigned int
_tmp22A;int _tmp22B;struct _tuple7 _tmp229=Cyc_Evexp_eval_const_uint_exp(e2);
_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;if(_tmp22B)Cyc_Tcutil_check_bound(loc,
_tmp22A,_tmp227);}else{if(Cyc_Tcutil_is_bound_one(_tmp227))({void*_tmp22C[0]={};
Cyc_Tcutil_warn(e1->loc,_tag_arr("subscript applied to pointer to one object",
sizeof(char),43),_tag_arr(_tmp22C,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp227);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp225);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp224),(void*)1))({void*_tmp22D[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(char),36),_tag_arr(_tmp22D,
sizeof(void*),0));});return _tmp224;_LL191: if(_tmp222 <= (void*)3?1:*((int*)
_tmp222)!= 9)goto _LL193;_tmp228=((struct Cyc_Absyn_TupleType_struct*)_tmp222)->f1;
_LL192: return Cyc_Tcexp_ithTupleType(te,loc,_tmp228,e2);_LL193:;_LL194: return({
struct Cyc_Std_String_pa_struct _tmp22F;_tmp22F.tag=0;_tmp22F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp22E[1]={& _tmp22F};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("subscript applied to %s",sizeof(char),24),_tag_arr(_tmp22E,
sizeof(void*),1));}});_LL18E:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;
struct Cyc_List_List*fields=0;if(topt != 0){void*_tmp230=Cyc_Tcutil_compress(*topt);
struct Cyc_List_List*_tmp231;_LL196: if(_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 9)
goto _LL198;_tmp231=((struct Cyc_Absyn_TupleType_struct*)_tmp230)->f1;_LL197: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp231)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL195;for(0;es != 0;(es=es->tl,_tmp231=_tmp231->tl)){
void*_tmp232=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp231))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp232,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233->hd=({
struct _tuple9*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->f1=(*((struct _tuple9*)
_tmp231->hd)).f1;_tmp234->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp234;});_tmp233->tl=fields;_tmp233;});}done=
1;goto _LL195;_LL198:;_LL199: goto _LL195;_LL195:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235->hd=({struct _tuple9*_tmp236=
_cycalloc(sizeof(*_tmp236));_tmp236->f1=Cyc_Absyn_empty_tqual();_tmp236->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp236;});_tmp235->tl=fields;_tmp235;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Absyn_TupleType_struct
_tmp238;_tmp238.tag=9;_tmp238.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp238;});_tmp237;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp239[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(char),14),_tag_arr(_tmp239,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp24E=_cycalloc(
sizeof(*_tmp24E));_tmp24E->v=Cyc_Tcenv_lookup_type_vars(te);_tmp24E;}));struct
Cyc_Absyn_Const_e_struct*_tmp23A=({struct Cyc_Absyn_Const_e_struct*_tmp24A=
_cycalloc(sizeof(*_tmp24A));_tmp24A[0]=({struct Cyc_Absyn_Const_e_struct _tmp24B;
_tmp24B.tag=0;_tmp24B.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp24C=
_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_Absyn_Int_c_struct _tmp24D;
_tmp24D.tag=2;_tmp24D.f1=(void*)((void*)1);_tmp24D.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp24D;});_tmp24C;}));_tmp24B;});_tmp24A;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp23A,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->v=(void*)Cyc_Absyn_uint_typ;_tmp23B;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp248=_cycalloc(sizeof(*
_tmp248));_tmp248[0]=({struct Cyc_Absyn_ArrayType_struct _tmp249;_tmp249.tag=7;
_tmp249.f1=(void*)res;_tmp249.f2=Cyc_Absyn_empty_tqual();_tmp249.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp249;});_tmp248;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_Std_String_pa_struct _tmp23D;_tmp23D.tag=0;_tmp23D.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp23C[1]={& _tmp23D};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(char),53),_tag_arr(_tmp23C,sizeof(void*),1));}});{int offset=0;for(0;des != 
0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)des->hd)).f1;
if(ds != 0){void*_tmp23E=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp23F;_LL19B: if(*((
int*)_tmp23E)!= 1)goto _LL19D;_LL19C:({void*_tmp240[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("only array index designators are supported",sizeof(char),43),_tag_arr(
_tmp240,sizeof(void*),0));});goto _LL19A;_LL19D: if(*((int*)_tmp23E)!= 0)goto
_LL19A;_tmp23F=((struct Cyc_Absyn_ArrayElement_struct*)_tmp23E)->f1;_LL19E: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp23F);{unsigned int _tmp242;int _tmp243;struct _tuple7 _tmp241=Cyc_Evexp_eval_const_uint_exp(
_tmp23F);_tmp242=_tmp241.f1;_tmp243=_tmp241.f2;if(!_tmp243)({void*_tmp244[0]={};
Cyc_Tcutil_terr(_tmp23F->loc,_tag_arr("index designator cannot use sizeof or offsetof",
sizeof(char),47),_tag_arr(_tmp244,sizeof(void*),0));});else{if(_tmp242 != offset)({
struct Cyc_Std_Int_pa_struct _tmp247;_tmp247.tag=1;_tmp247.f1=(int)_tmp242;{struct
Cyc_Std_Int_pa_struct _tmp246;_tmp246.tag=1;_tmp246.f1=(int)((unsigned int)offset);{
void*_tmp245[2]={& _tmp246,& _tmp247};Cyc_Tcutil_terr(_tmp23F->loc,_tag_arr("expecting index designator %d but found %d",
sizeof(char),43),_tag_arr(_tmp245,sizeof(void*),2));}}});}goto _LL19A;}_LL19A:;}}}
return res_t2;}}static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*
bound,struct Cyc_Absyn_Exp*body){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_Std_String_pa_struct _tmp250;_tmp250.tag=0;_tmp250.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);{void*_tmp24F[1]={& _tmp250};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(char),33),_tag_arr(_tmp24F,
sizeof(void*),1));}});if(!(vd->tq).q_const)({void*_tmp251[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(char),52),_tag_arr(_tmp251,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(
loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;
void**_tmp252=0;struct Cyc_Absyn_Tqual*_tmp253=0;if(topt != 0){void*_tmp254=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp255;void*_tmp256;void**_tmp257;struct Cyc_Absyn_Tqual
_tmp258;struct Cyc_Absyn_Tqual*_tmp259;struct Cyc_Absyn_Exp*_tmp25A;_LL1A0: if(
_tmp254 <= (void*)3?1:*((int*)_tmp254)!= 4)goto _LL1A2;_tmp255=((struct Cyc_Absyn_PointerType_struct*)
_tmp254)->f1;_LL1A1: pinfo=_tmp255;_tmp252=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp253=(struct Cyc_Absyn_Tqual*)& pinfo.tq;goto _LL19F;_LL1A2: if(_tmp254 <= (void*)
3?1:*((int*)_tmp254)!= 7)goto _LL1A4;_tmp256=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp254)->f1;_tmp257=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp254)->f1);_tmp258=((struct Cyc_Absyn_ArrayType_struct*)_tmp254)->f2;_tmp259=(
struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_struct*)_tmp254)->f2;_tmp25A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp254)->f3;_LL1A3: _tmp252=(void**)_tmp257;
_tmp253=(struct Cyc_Absyn_Tqual*)_tmp259;goto _LL19F;_LL1A4:;_LL1A5: goto _LL19F;
_LL19F:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp252,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp25B[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(char),22),_tag_arr(_tmp25B,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(
te,body))({void*_tmp25C[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(char),21),_tag_arr(_tmp25C,sizeof(void*),0));});}return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp25E;_tmp25E.tag=7;_tmp25E.f1=(void*)t;_tmp25E.f2=_tmp253 == 0?Cyc_Absyn_empty_tqual():*
_tmp253;_tmp25E.f3=(struct Cyc_Absyn_Exp*)bound;_tmp25E;});_tmp25D;});}}}struct
_tuple10{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp25F;_push_handler(& _tmp25F);{
int _tmp261=0;if(setjmp(_tmp25F.handler))_tmp261=1;if(!_tmp261){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp260=(void*)_exn_thrown;void*_tmp263=
_tmp260;_LL1A7: if(_tmp263 != Cyc_Dict_Absent)goto _LL1A9;_LL1A8:({struct Cyc_Std_String_pa_struct
_tmp265;_tmp265.tag=0;_tmp265.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp264[1]={& _tmp265};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(char),23),_tag_arr(_tmp264,sizeof(void*),1));}});return topt != 0?*topt:(
void*)0;_LL1A9:;_LL1AA:(void)_throw(_tmp263);_LL1A6:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp266[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("expecting struct but found union",sizeof(char),33),_tag_arr(
_tmp266,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp267[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't build abstract struct",sizeof(char),28),_tag_arr(_tmp267,
sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp268=
_cycalloc(sizeof(*_tmp268));_tmp268->v=Cyc_Tcenv_lookup_type_vars(te);_tmp268;}));}{
struct _RegionHandle _tmp269=_new_region("rgn");struct _RegionHandle*rgn=& _tmp269;
_push_region(rgn);{struct _tuple4 _tmp26A=({struct _tuple4 _tmp289;_tmp289.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp289.f2=rgn;_tmp289;});struct Cyc_List_List*_tmp26B=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp26A,ad->tvs);struct Cyc_List_List*_tmp26C=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp26A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp26D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp26B);struct Cyc_List_List*_tmp26E=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp26C);struct Cyc_List_List*_tmp26F=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp26B,_tmp26C);struct Cyc_Absyn_AggrType_struct*_tmp270=({struct Cyc_Absyn_AggrType_struct*
_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Absyn_AggrType_struct
_tmp284;_tmp284.tag=10;_tmp284.f1=({struct Cyc_Absyn_AggrInfo _tmp285;_tmp285.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp286=_cycalloc(sizeof(*
_tmp286));_tmp286[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp287;_tmp287.tag=1;
_tmp287.f1=({struct Cyc_Absyn_Aggrdecl**_tmp288=_cycalloc(sizeof(*_tmp288));
_tmp288[0]=ad;_tmp288;});_tmp287;});_tmp286;}));_tmp285.targs=_tmp26D;_tmp285;});
_tmp284;});_tmp283;});struct Cyc_List_List*_tmp271=*ts;struct Cyc_List_List*
_tmp272=_tmp26E;while(_tmp271 != 0?_tmp272 != 0: 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp271->hd);Cyc_Tcutil_unify((
void*)_tmp271->hd,(void*)_tmp272->hd);_tmp271=_tmp271->tl;_tmp272=_tmp272->tl;}
if(_tmp271 != 0)({void*_tmp273[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many explicit witness types",
sizeof(char),32),_tag_arr(_tmp273,sizeof(void*),0));});*ts=_tmp26E;if(topt != 0)
Cyc_Tcutil_unify((void*)_tmp270,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp275;struct Cyc_Absyn_Aggrfield*_tmp276;
struct Cyc_Absyn_Exp*_tmp277;struct _tuple10*_tmp274=(struct _tuple10*)fields->hd;
_tmp275=*_tmp274;_tmp276=_tmp275.f1;_tmp277=_tmp275.f2;{void*_tmp278=Cyc_Tcutil_rsubstitute(
rgn,_tmp26F,(void*)_tmp276->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp278,
_tmp277);if(!Cyc_Tcutil_coerce_arg(te,_tmp277,_tmp278)){({struct Cyc_Std_String_pa_struct
_tmp27D;_tmp27D.tag=0;_tmp27D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp277->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp27C;_tmp27C.tag=0;_tmp27C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp278);{struct Cyc_Std_String_pa_struct _tmp27B;_tmp27B.tag=0;_tmp27B.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(*tn);{struct Cyc_Std_String_pa_struct _tmp27A;
_tmp27A.tag=0;_tmp27A.f1=(struct _tagged_arr)*_tmp276->name;{void*_tmp279[4]={&
_tmp27A,& _tmp27B,& _tmp27C,& _tmp27D};Cyc_Tcutil_terr(_tmp277->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(char),44),_tag_arr(_tmp279,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp27E=0;{struct Cyc_List_List*_tmp27F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp27F != 0;_tmp27F=_tmp27F->tl){_tmp27E=({
struct Cyc_List_List*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->hd=({struct
_tuple0*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp26F,(*((struct _tuple0*)_tmp27F->hd)).f1);_tmp281->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp26F,(*((struct _tuple0*)_tmp27F->hd)).f2);_tmp281;});_tmp280->tl=_tmp27E;
_tmp280;});}}_tmp27E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp27E);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp27E);{void*_tmp282=(void*)
_tmp270;_npop_handler(0);return _tmp282;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp28A=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp28A;_push_region(rgn);{void*_tmp28B=Cyc_Tcutil_compress(ts);void*_tmp28C;
struct Cyc_List_List*_tmp28D;_LL1AC: if(_tmp28B <= (void*)3?1:*((int*)_tmp28B)!= 11)
goto _LL1AE;_tmp28C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28B)->f1;
_tmp28D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28B)->f2;_LL1AD: if(_tmp28C == (
void*)1)({void*_tmp28E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expecting struct but found union",
sizeof(char),33),_tag_arr(_tmp28E,sizeof(void*),0));});{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp28D);for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp290;
struct Cyc_Absyn_Aggrfield*_tmp291;struct Cyc_Absyn_Exp*_tmp292;struct _tuple10*
_tmp28F=(struct _tuple10*)fields->hd;_tmp290=*_tmp28F;_tmp291=_tmp290.f1;_tmp292=
_tmp290.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp291->type)),
_tmp292);if(!Cyc_Tcutil_coerce_arg(te,_tmp292,(void*)_tmp291->type)){({struct Cyc_Std_String_pa_struct
_tmp296;_tmp296.tag=0;_tmp296.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp292->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp295;_tmp295.tag=0;_tmp295.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp291->type);{struct Cyc_Std_String_pa_struct _tmp294;_tmp294.tag=0;_tmp294.f1=(
struct _tagged_arr)*_tmp291->name;{void*_tmp293[3]={& _tmp294,& _tmp295,& _tmp296};
Cyc_Tcutil_terr(_tmp292->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(char),41),_tag_arr(_tmp293,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL1AB;}_LL1AE:;_LL1AF:({void*_tmp297[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct: wrong type",
sizeof(char),25),_tag_arr(_tmp297,sizeof(void*),0));});_LL1AB:;};_pop_region(rgn);}
return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp298=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp298;_push_region(rgn);{struct _tuple4 _tmp299=({
struct _tuple4 _tmp2B8;_tmp2B8.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp2B8.f2=rgn;
_tmp2B8;});struct Cyc_List_List*_tmp29A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp299,tud->tvs);struct Cyc_List_List*_tmp29B=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp29A);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp2B4;_tmp2B4.tag=3;_tmp2B4.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp2B5;
_tmp2B5.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp2B7;_tmp2B7.tag=1;_tmp2B7.f1=tud;_tmp2B7.f2=tuf;_tmp2B7;});_tmp2B6;}));
_tmp2B5.targs=_tmp29B;_tmp2B5;});_tmp2B4;});_tmp2B3;});if(topt != 0){void*_tmp29C=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp29D;struct Cyc_List_List*
_tmp29E;void*_tmp29F;_LL1B1: if(_tmp29C <= (void*)3?1:*((int*)_tmp29C)!= 3)goto
_LL1B3;_LL1B2: Cyc_Tcutil_unify(*topt,res);goto _LL1B0;_LL1B3: if(_tmp29C <= (void*)
3?1:*((int*)_tmp29C)!= 2)goto _LL1B5;_tmp29D=((struct Cyc_Absyn_TunionType_struct*)
_tmp29C)->f1;_tmp29E=_tmp29D.targs;_tmp29F=(void*)_tmp29D.rgn;_LL1B4:{struct Cyc_List_List*
a=_tmp29B;for(0;a != 0?_tmp29E != 0: 0;(a=a->tl,_tmp29E=_tmp29E->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp29E->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->v=(void*)res;_tmp2A0;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1[0]=({
struct Cyc_Absyn_TunionType_struct _tmp2A2;_tmp2A2.tag=2;_tmp2A2.f1=({struct Cyc_Absyn_TunionInfo
_tmp2A3;_tmp2A3.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp2A5;_tmp2A5.tag=1;_tmp2A5.f1=({struct Cyc_Absyn_Tuniondecl**_tmp2A6=_cycalloc(
sizeof(*_tmp2A6));_tmp2A6[0]=tud;_tmp2A6;});_tmp2A5;});_tmp2A4;}));_tmp2A3.targs=
_tmp29B;_tmp2A3.rgn=(void*)_tmp29F;_tmp2A3;});_tmp2A2;});_tmp2A1;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp2A7=res;_npop_handler(0);return _tmp2A7;}}goto _LL1B0;_LL1B5:;
_LL1B6: goto _LL1B0;_LL1B0:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp29A,(*((struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmp2AB;_tmp2AB.tag=0;_tmp2AB.f1=(struct _tagged_arr)(e->topt == 0?_tag_arr("?",
sizeof(char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp2A8[3]={& _tmp2A9,& _tmp2AA,& _tmp2AB};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(char),80),_tag_arr(_tmp2A8,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp2AE=({struct Cyc_Std_String_pa_struct _tmp2AD;_tmp2AD.tag=0;
_tmp2AD.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp2AC[1]={&
_tmp2AD};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(char),45),_tag_arr(_tmp2AC,sizeof(void*),1));}});_npop_handler(0);return
_tmp2AE;}if(ts != 0){void*_tmp2B1=({struct Cyc_Std_String_pa_struct _tmp2B0;_tmp2B0.tag=
0;_tmp2B0.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp2AF[
1]={& _tmp2B0};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(char),44),_tag_arr(_tmp2AF,sizeof(void*),1));}});_npop_handler(0);return
_tmp2B1;}{void*_tmp2B2=res;_npop_handler(0);return _tmp2B2;}}};_pop_region(rgn);}
static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp2B9=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp2BA;struct Cyc_Absyn_Conref*_tmp2BB;void*_tmp2BC;
struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_AggrInfo _tmp2BE;void*_tmp2BF;struct
Cyc_List_List*_tmp2C0;struct Cyc_List_List*_tmp2C1;_LL1B8: if((int)_tmp2B9 != 0)
goto _LL1BA;_LL1B9: return 1;_LL1BA: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 0)
goto _LL1BC;_LL1BB: goto _LL1BD;_LL1BC: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 1)
goto _LL1BE;_LL1BD: goto _LL1BF;_LL1BE: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 2)
goto _LL1C0;_LL1BF: goto _LL1C1;_LL1C0: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 3)
goto _LL1C2;_LL1C1: return 0;_LL1C2: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 4)
goto _LL1C4;_tmp2BA=((struct Cyc_Absyn_PointerType_struct*)_tmp2B9)->f1;_tmp2BB=
_tmp2BA.nullable;_LL1C3: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp2BB);_LL1C4: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 5)goto _LL1C6;_LL1C5:
goto _LL1C7;_LL1C6: if((int)_tmp2B9 != 1)goto _LL1C8;_LL1C7: goto _LL1C9;_LL1C8: if(
_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 6)goto _LL1CA;_LL1C9: return 1;_LL1CA: if(
_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 7)goto _LL1CC;_tmp2BC=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp2B9)->f1;_LL1CB: return Cyc_Tcexp_zeroable_type(_tmp2BC);_LL1CC: if(_tmp2B9 <= (
void*)3?1:*((int*)_tmp2B9)!= 8)goto _LL1CE;_LL1CD: return 0;_LL1CE: if(_tmp2B9 <= (
void*)3?1:*((int*)_tmp2B9)!= 9)goto _LL1D0;_tmp2BD=((struct Cyc_Absyn_TupleType_struct*)
_tmp2B9)->f1;_LL1CF: for(0;(unsigned int)_tmp2BD;_tmp2BD=_tmp2BD->tl){if(!Cyc_Tcexp_zeroable_type((*((
struct _tuple9*)_tmp2BD->hd)).f2))return 0;}return 1;_LL1D0: if(_tmp2B9 <= (void*)3?1:*((
int*)_tmp2B9)!= 10)goto _LL1D2;_tmp2BE=((struct Cyc_Absyn_AggrType_struct*)_tmp2B9)->f1;
_tmp2BF=(void*)_tmp2BE.aggr_info;_tmp2C0=_tmp2BE.targs;_LL1D1: {struct Cyc_Absyn_Aggrdecl*
_tmp2C2=Cyc_Absyn_get_known_aggrdecl(_tmp2BF);if(_tmp2C2->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2C2->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp2C3=
_new_region("r");struct _RegionHandle*r=& _tmp2C3;_push_region(r);{struct Cyc_List_List*
_tmp2C4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp2C2->tvs,
_tmp2C0);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp2C2->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(
r,_tmp2C4,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp2C5=0;
_npop_handler(0);return _tmp2C5;}}}{int _tmp2C6=1;_npop_handler(0);return _tmp2C6;}};
_pop_region(r);}}_LL1D2: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 12)goto _LL1D4;
_LL1D3: return 1;_LL1D4: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 14)goto _LL1D6;
_LL1D5: return 1;_LL1D6: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 17)goto _LL1D8;
_LL1D7: return 1;_LL1D8: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 11)goto _LL1DA;
_tmp2C1=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2B9)->f2;_LL1D9: for(0;_tmp2C1
!= 0;_tmp2C1=_tmp2C1->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp2C1->hd)->type))return 0;}return 1;_LL1DA: if(_tmp2B9 <= (void*)3?1:*((int*)
_tmp2B9)!= 13)goto _LL1DC;_LL1DB: return 1;_LL1DC: if(_tmp2B9 <= (void*)3?1:*((int*)
_tmp2B9)!= 16)goto _LL1DE;_LL1DD: return 0;_LL1DE: if(_tmp2B9 <= (void*)3?1:*((int*)
_tmp2B9)!= 15)goto _LL1E0;_LL1DF: return 0;_LL1E0: if(_tmp2B9 <= (void*)3?1:*((int*)
_tmp2B9)!= 18)goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2: if((int)_tmp2B9 != 2)goto _LL1E4;
_LL1E3: goto _LL1E5;_LL1E4: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 19)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 20)
goto _LL1E8;_LL1E7: goto _LL1E9;_LL1E8: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 
21)goto _LL1B7;_LL1E9:({struct Cyc_Std_String_pa_struct _tmp2C8;_tmp2C8.tag=0;
_tmp2C8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp2C7[1]={&
_tmp2C8};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad type `%s' in zeroable type",sizeof(char),31),_tag_arr(_tmp2C7,
sizeof(void*),1));}});_LL1B7:;}static void Cyc_Tcexp_check_malloc_type(int
allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;if(topt != 0){void*_tmp2C9=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2CA;void*_tmp2CB;_LL1EB:
if(_tmp2C9 <= (void*)3?1:*((int*)_tmp2C9)!= 4)goto _LL1ED;_tmp2CA=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C9)->f1;_tmp2CB=(void*)_tmp2CA.elt_typ;_LL1EC: Cyc_Tcutil_unify(_tmp2CB,t);
if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;
goto _LL1EA;_LL1ED:;_LL1EE: goto _LL1EA;_LL1EA:;}({struct Cyc_Std_String_pa_struct
_tmp2CE;_tmp2CE.tag=0;_tmp2CE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(struct
_tagged_arr)(allow_zero?_tag_arr("calloc",sizeof(char),7): _tag_arr("malloc",
sizeof(char),7));{void*_tmp2CC[2]={& _tmp2CD,& _tmp2CE};Cyc_Tcutil_terr(loc,
_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",sizeof(
char),60),_tag_arr(_tmp2CC,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2D4;_tmp2D4.tag=15;_tmp2D4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2D5;}));_tmp2D4;});_tmp2D3;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2CF=Cyc_Tcutil_compress(handle_type);void*_tmp2D0;
_LL1F0: if(_tmp2CF <= (void*)3?1:*((int*)_tmp2CF)!= 15)goto _LL1F2;_tmp2D0=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2CF)->f1;_LL1F1: rgn=_tmp2D0;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1EF;_LL1F2:;_LL1F3:({struct Cyc_Std_String_pa_struct _tmp2D2;
_tmp2D2.tag=0;_tmp2D2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{
void*_tmp2D1[1]={& _tmp2D2};Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(char),37),_tag_arr(_tmp2D1,sizeof(void*),1));}});goto _LL1EF;_LL1EF:;}Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;
int one_elt;if(is_calloc){if(*t == 0)({void*_tmp2D6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(char),23),_tag_arr(_tmp2D6,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp2D7=(void*)(*e)->r;void*_tmp2D8;void*_tmp2D9;struct Cyc_List_List*
_tmp2DA;struct Cyc_List_List _tmp2DB;struct Cyc_Absyn_Exp*_tmp2DC;struct Cyc_List_List*
_tmp2DD;struct Cyc_List_List _tmp2DE;struct Cyc_Absyn_Exp*_tmp2DF;struct Cyc_List_List*
_tmp2E0;_LL1F5: if(*((int*)_tmp2D7)!= 16)goto _LL1F7;_tmp2D8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2D7)->f1;_LL1F6: elt_type=_tmp2D8;*t=(void**)({void**_tmp2E1=_cycalloc(
sizeof(*_tmp2E1));_tmp2E1[0]=elt_type;_tmp2E1;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL1F4;
_LL1F7: if(*((int*)_tmp2D7)!= 3)goto _LL1F9;_tmp2D9=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2D7)->f1;if((int)_tmp2D9 != 1)goto _LL1F9;_tmp2DA=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2D7)->f2;if(_tmp2DA == 0)goto _LL1F9;_tmp2DB=*_tmp2DA;_tmp2DC=(struct Cyc_Absyn_Exp*)
_tmp2DB.hd;_tmp2DD=_tmp2DB.tl;if(_tmp2DD == 0)goto _LL1F9;_tmp2DE=*_tmp2DD;_tmp2DF=(
struct Cyc_Absyn_Exp*)_tmp2DE.hd;_tmp2E0=_tmp2DE.tl;if(_tmp2E0 != 0)goto _LL1F9;
_LL1F8:{struct _tuple0 _tmp2E3=({struct _tuple0 _tmp2E2;_tmp2E2.f1=(void*)_tmp2DC->r;
_tmp2E2.f2=(void*)_tmp2DF->r;_tmp2E2;});void*_tmp2E4;void*_tmp2E5;void*_tmp2E6;
void*_tmp2E7;_LL1FC: _tmp2E4=_tmp2E3.f1;if(*((int*)_tmp2E4)!= 16)goto _LL1FE;
_tmp2E5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2E4)->f1;_LL1FD: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2E5);elt_type=_tmp2E5;*t=(void**)({void**_tmp2E8=_cycalloc(
sizeof(*_tmp2E8));_tmp2E8[0]=elt_type;_tmp2E8;});num_elts=_tmp2DF;one_elt=0;goto
_LL1FB;_LL1FE: _tmp2E6=_tmp2E3.f2;if(*((int*)_tmp2E6)!= 16)goto _LL200;_tmp2E7=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2E6)->f1;_LL1FF: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2E7);elt_type=_tmp2E7;*t=(void**)({void**_tmp2E9=_cycalloc(
sizeof(*_tmp2E9));_tmp2E9[0]=elt_type;_tmp2E9;});num_elts=_tmp2DC;one_elt=0;goto
_LL1FB;_LL200:;_LL201: goto No_sizeof;_LL1FB:;}goto _LL1F4;_LL1F9:;_LL1FA: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp2EA=_cycalloc(sizeof(*_tmp2EA));
_tmp2EA[0]=elt_type;_tmp2EA;});num_elts=*e;one_elt=0;goto _LL1F4;_LL1F4:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp2EB=elt_type;struct Cyc_Absyn_AggrInfo _tmp2EC;
void*_tmp2ED;struct Cyc_Absyn_Aggrdecl**_tmp2EE;struct Cyc_Absyn_Aggrdecl*_tmp2EF;
_LL203: if(_tmp2EB <= (void*)3?1:*((int*)_tmp2EB)!= 10)goto _LL205;_tmp2EC=((struct
Cyc_Absyn_AggrType_struct*)_tmp2EB)->f1;_tmp2ED=(void*)_tmp2EC.aggr_info;if(*((
int*)_tmp2ED)!= 1)goto _LL205;_tmp2EE=((struct Cyc_Absyn_KnownAggr_struct*)_tmp2ED)->f1;
_tmp2EF=*_tmp2EE;_LL204: if(_tmp2EF->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2EF->impl))->exist_vars != 0: 0)({void*_tmp2F0[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("malloc with existential types not yet implemented",sizeof(char),50),
_tag_arr(_tmp2F0,sizeof(void*),0));});goto _LL202;_LL205:;_LL206: goto _LL202;
_LL202:;}{void*(*_tmp2F1)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;
if(topt != 0){void*_tmp2F2=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2F3;struct Cyc_Absyn_Conref*_tmp2F4;_LL208: if(_tmp2F2 <= (void*)3?1:*((int*)
_tmp2F2)!= 4)goto _LL20A;_tmp2F3=((struct Cyc_Absyn_PointerType_struct*)_tmp2F2)->f1;
_tmp2F4=_tmp2F3.nullable;_LL209: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp2F4))_tmp2F1=Cyc_Absyn_star_typ;goto _LL207;_LL20A:;_LL20B: goto _LL207;_LL207:;}
if(!one_elt)_tmp2F1=Cyc_Absyn_tagged_typ;return _tmp2F1(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp2F5=(void*)s->r;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Stmt*
_tmp2F7;struct Cyc_Absyn_Stmt*_tmp2F8;struct Cyc_Absyn_Decl*_tmp2F9;struct Cyc_Absyn_Stmt*
_tmp2FA;_LL20D: if(_tmp2F5 <= (void*)1?1:*((int*)_tmp2F5)!= 0)goto _LL20F;_tmp2F6=((
struct Cyc_Absyn_Exp_s_struct*)_tmp2F5)->f1;_LL20E: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F6->topt))->v;_LL20F: if(_tmp2F5 <= (void*)1?1:*((int*)_tmp2F5)!= 
1)goto _LL211;_tmp2F7=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F5)->f1;_tmp2F8=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2F5)->f2;_LL210: s=_tmp2F8;continue;_LL211: if(
_tmp2F5 <= (void*)1?1:*((int*)_tmp2F5)!= 12)goto _LL213;_tmp2F9=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F5)->f1;_tmp2FA=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F5)->f2;_LL212: s=
_tmp2FA;continue;_LL213:;_LL214: return({void*_tmp2FB[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("statement expression must end with expression",sizeof(char),46),
_tag_arr(_tmp2FB,sizeof(void*),0));});_LL20C:;}}static void*Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*
fd){return({void*_tmp2FC[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcCodegen",
sizeof(char),10),_tag_arr(_tmp2FC,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){return({void*_tmp2FD[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcFill",
sizeof(char),7),_tag_arr(_tmp2FD,sizeof(void*),0));});}static void*Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(
rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp303;_tmp303.tag=15;_tmp303.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp304;}));_tmp303;});_tmp302;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp2FE=Cyc_Tcutil_compress(handle_type);void*
_tmp2FF;_LL216: if(_tmp2FE <= (void*)3?1:*((int*)_tmp2FE)!= 15)goto _LL218;_tmp2FF=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2FE)->f1;_LL217: rgn=_tmp2FF;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL215;_LL218:;_LL219:({struct Cyc_Std_String_pa_struct
_tmp301;_tmp301.tag=0;_tmp301.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type);{void*_tmp300[1]={& _tmp301};Cyc_Tcutil_terr(rgn_handle->loc,
_tag_arr("expecting region_t type but found %s",sizeof(char),37),_tag_arr(
_tmp300,sizeof(void*),1));}});goto _LL215;_LL215:;}{void*_tmp305=(void*)e1->r;
struct Cyc_Absyn_Vardecl*_tmp306;struct Cyc_Absyn_Exp*_tmp307;struct Cyc_Absyn_Exp*
_tmp308;struct Cyc_Core_Opt*_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_List_List*
_tmp30B;void*_tmp30C;struct _tagged_arr _tmp30D;_LL21B: if(*((int*)_tmp305)!= 27)
goto _LL21D;_tmp306=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp305)->f1;
_tmp307=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp305)->f2;_tmp308=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp305)->f3;_LL21C: {void*_tmp30E=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp30F=Cyc_Tcutil_compress(_tmp30E);void*_tmp310;struct Cyc_Absyn_Tqual
_tmp311;_LL226: if(_tmp30F <= (void*)3?1:*((int*)_tmp30F)!= 7)goto _LL228;_tmp310=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp30F)->f1;_tmp311=((struct Cyc_Absyn_ArrayType_struct*)
_tmp30F)->f2;_LL227: {void*b=Cyc_Tcutil_is_const_exp(te,_tmp307)?(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp317;_tmp317.tag=0;_tmp317.f1=_tmp307;_tmp317;});
_tmp316;}):(void*)0;void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313[0]=({struct Cyc_Absyn_PointerType_struct
_tmp314;_tmp314.tag=4;_tmp314.f1=({struct Cyc_Absyn_PtrInfo _tmp315;_tmp315.elt_typ=(
void*)_tmp310;_tmp315.rgn_typ=(void*)rgn;_tmp315.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp315.tq=_tmp311;_tmp315.bounds=Cyc_Absyn_new_conref(
b);_tmp315;});_tmp314;});_tmp313;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,
res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->v=(void*)res_typ;_tmp312;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}_LL228:;_LL229:({void*_tmp318[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcNew: comprehension returned non-array type",
sizeof(char),45),_tag_arr(_tmp318,sizeof(void*),0));});_LL225:;}_LL21D: if(*((int*)
_tmp305)!= 34)goto _LL21F;_tmp309=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp305)->f1;_tmp30A=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp305)->f2;
_LL21E:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp319=
_cycalloc(sizeof(*_tmp319));_tmp319[0]=({struct Cyc_Absyn_Array_e_struct _tmp31A;
_tmp31A.tag=26;_tmp31A.f1=_tmp30A;_tmp31A;});_tmp319;})));_tmp30B=_tmp30A;goto
_LL220;_LL21F: if(*((int*)_tmp305)!= 26)goto _LL221;_tmp30B=((struct Cyc_Absyn_Array_e_struct*)
_tmp305)->f1;_LL220: {void**elt_typ_opt=0;if(topt != 0){void*_tmp31B=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp31C;void*_tmp31D;void**_tmp31E;struct Cyc_Absyn_Tqual
_tmp31F;_LL22B: if(_tmp31B <= (void*)3?1:*((int*)_tmp31B)!= 4)goto _LL22D;_tmp31C=((
struct Cyc_Absyn_PointerType_struct*)_tmp31B)->f1;_tmp31D=(void*)_tmp31C.elt_typ;
_tmp31E=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp31B)->f1).elt_typ;
_tmp31F=_tmp31C.tq;_LL22C: elt_typ_opt=(void**)_tmp31E;goto _LL22A;_LL22D:;_LL22E:
goto _LL22A;_LL22A:;}{void*_tmp320=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);
void*res_typ;{void*_tmp321=Cyc_Tcutil_compress(_tmp320);void*_tmp322;struct Cyc_Absyn_Tqual
_tmp323;struct Cyc_Absyn_Exp*_tmp324;_LL230: if(_tmp321 <= (void*)3?1:*((int*)
_tmp321)!= 7)goto _LL232;_tmp322=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp321)->f1;_tmp323=((struct Cyc_Absyn_ArrayType_struct*)_tmp321)->f2;_tmp324=((
struct Cyc_Absyn_ArrayType_struct*)_tmp321)->f3;_LL231: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325[0]=({struct Cyc_Absyn_PointerType_struct
_tmp326;_tmp326.tag=4;_tmp326.f1=({struct Cyc_Absyn_PtrInfo _tmp327;_tmp327.elt_typ=(
void*)_tmp322;_tmp327.rgn_typ=(void*)rgn;_tmp327.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp327.tq=_tmp323;_tmp327.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp328=_cycalloc(sizeof(*_tmp328));
_tmp328[0]=({struct Cyc_Absyn_Upper_b_struct _tmp329;_tmp329.tag=0;_tmp329.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp324);_tmp329;});_tmp328;}));_tmp327;});
_tmp326;});_tmp325;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp32A=_cycalloc(sizeof(*
_tmp32A));_tmp32A->v=(void*)res_typ;_tmp32A;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL22F;_LL232:;_LL233:({void*_tmp32B[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(char),50),_tag_arr(_tmp32B,sizeof(void*),0));});_LL22F:;}return res_typ;}}
_LL221: if(*((int*)_tmp305)!= 0)goto _LL223;_tmp30C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp305)->f1;if(_tmp30C <= (void*)1?1:*((int*)_tmp30C)!= 5)goto _LL223;_tmp30D=((
struct Cyc_Absyn_String_c_struct*)_tmp30C)->f1;_LL222: {void*_tmp32C=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(void*)0);void*_tmp32D=Cyc_Tcexp_tcExp(
te,(void**)& _tmp32C,e1);return Cyc_Absyn_atb_typ(_tmp32D,rgn,Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp32E=_cycalloc(sizeof(*_tmp32E));
_tmp32E[0]=({struct Cyc_Absyn_Upper_b_struct _tmp32F;_tmp32F.tag=0;_tmp32F.f1=Cyc_Absyn_uint_exp(
1,0);_tmp32F;});_tmp32E;}));}_LL223:;_LL224: {void**topt2=0;if(topt != 0){void*
_tmp330=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp331;void*_tmp332;
void**_tmp333;struct Cyc_Absyn_Conref*_tmp334;_LL235: if(_tmp330 <= (void*)3?1:*((
int*)_tmp330)!= 4)goto _LL237;_tmp331=((struct Cyc_Absyn_PointerType_struct*)
_tmp330)->f1;_tmp332=(void*)_tmp331.elt_typ;_tmp333=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp330)->f1).elt_typ;_tmp334=_tmp331.nullable;_LL236: topt2=(void**)_tmp333;goto
_LL234;_LL237: if(_tmp330 <= (void*)3?1:*((int*)_tmp330)!= 2)goto _LL239;_LL238:
topt2=({void**_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335[0]=*topt;_tmp335;});
goto _LL234;_LL239:;_LL23A: goto _LL234;_LL234:;}{void*telt=Cyc_Tcexp_tcExp(te,
topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp337=
_cycalloc(sizeof(*_tmp337));_tmp337[0]=({struct Cyc_Absyn_PointerType_struct
_tmp338;_tmp338.tag=4;_tmp338.f1=({struct Cyc_Absyn_PtrInfo _tmp339;_tmp339.elt_typ=(
void*)telt;_tmp339.rgn_typ=(void*)rgn;_tmp339.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp339.tq=Cyc_Absyn_empty_tqual();_tmp339.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp33A=_cycalloc(sizeof(*_tmp33A));
_tmp33A[0]=({struct Cyc_Absyn_Upper_b_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=Cyc_Absyn_uint_exp(
1,0);_tmp33B;});_tmp33A;}));_tmp339;});_tmp338;});_tmp337;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->v=(
void*)res_typ;_tmp336;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL21A:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp33C=t;void*_tmp33D;struct Cyc_Absyn_Tqual _tmp33E;struct Cyc_Absyn_Exp*
_tmp33F;_LL23C: if(_tmp33C <= (void*)3?1:*((int*)_tmp33C)!= 7)goto _LL23E;_tmp33D=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp33C)->f1;_tmp33E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp33C)->f2;_tmp33F=((struct Cyc_Absyn_ArrayType_struct*)_tmp33C)->f3;_LL23D: {
void*_tmp341;struct _tuple6 _tmp340=Cyc_Tcutil_addressof_props(te,e);_tmp341=
_tmp340.f2;{void*_tmp342=_tmp33F == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp344;_tmp344.tag=0;_tmp344.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp33F);
_tmp344;});_tmp343;});t=Cyc_Absyn_atb_typ(_tmp33D,_tmp341,_tmp33E,_tmp342);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL23E:;_LL23F:
return t;_LL23B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*
_tmp345=(void*)e->r;void*_tmp346;_LL241: if(*((int*)_tmp345)!= 26)goto _LL243;
_LL242: goto _LL244;_LL243: if(*((int*)_tmp345)!= 27)goto _LL245;_LL244: goto _LL246;
_LL245: if(*((int*)_tmp345)!= 0)goto _LL247;_tmp346=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp345)->f1;if(_tmp346 <= (void*)1?1:*((int*)_tmp346)!= 5)goto _LL247;_LL246:
return t;_LL247:;_LL248: t=Cyc_Tcutil_compress(t);{void*_tmp347=t;void*_tmp348;
struct Cyc_Absyn_Tqual _tmp349;struct Cyc_Absyn_Exp*_tmp34A;_LL24A: if(_tmp347 <= (
void*)3?1:*((int*)_tmp347)!= 7)goto _LL24C;_tmp348=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp347)->f1;_tmp349=((struct Cyc_Absyn_ArrayType_struct*)_tmp347)->f2;_tmp34A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp347)->f3;_LL24B: {void*_tmp34C;struct
_tuple6 _tmp34B=Cyc_Tcutil_addressof_props(te,e);_tmp34C=_tmp34B.f2;{void*b=
_tmp34A == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp34D=_cycalloc(
sizeof(*_tmp34D));_tmp34D[0]=({struct Cyc_Absyn_Upper_b_struct _tmp34E;_tmp34E.tag=
0;_tmp34E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp34A);_tmp34E;});_tmp34D;});t=
Cyc_Absyn_atb_typ(_tmp348,_tmp34C,_tmp349,b);Cyc_Tcutil_unchecked_cast(te,e,t);
return t;}}_LL24C:;_LL24D: return t;_LL249:;}_LL240:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp34F=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp350;_LL24F: if(*((int*)_tmp34F)!= 11)goto _LL251;_tmp350=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp34F)->f1;_LL250: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp350);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp350->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp350->topt))->v));
e->topt=_tmp350->topt;goto _LL24E;_LL251:;_LL252: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp351=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp352;void*_tmp353;void*_tmp354;struct Cyc_Absyn_Conref*_tmp355;struct Cyc_Absyn_Tqual
_tmp356;struct Cyc_Absyn_Conref*_tmp357;_LL254: if(_tmp351 <= (void*)3?1:*((int*)
_tmp351)!= 4)goto _LL256;_tmp352=((struct Cyc_Absyn_PointerType_struct*)_tmp351)->f1;
_tmp353=(void*)_tmp352.elt_typ;_tmp354=(void*)_tmp352.rgn_typ;_tmp355=_tmp352.nullable;
_tmp356=_tmp352.tq;_tmp357=_tmp352.bounds;_LL255:{void*_tmp358=Cyc_Tcutil_compress(
_tmp353);struct Cyc_Absyn_FnInfo _tmp359;struct Cyc_List_List*_tmp35A;struct Cyc_Core_Opt*
_tmp35B;void*_tmp35C;struct Cyc_List_List*_tmp35D;int _tmp35E;struct Cyc_Absyn_VarargInfo*
_tmp35F;struct Cyc_List_List*_tmp360;struct Cyc_List_List*_tmp361;_LL259: if(
_tmp358 <= (void*)3?1:*((int*)_tmp358)!= 8)goto _LL25B;_tmp359=((struct Cyc_Absyn_FnType_struct*)
_tmp358)->f1;_tmp35A=_tmp359.tvars;_tmp35B=_tmp359.effect;_tmp35C=(void*)_tmp359.ret_typ;
_tmp35D=_tmp359.args;_tmp35E=_tmp359.c_varargs;_tmp35F=_tmp359.cyc_varargs;
_tmp360=_tmp359.rgn_po;_tmp361=_tmp359.attributes;_LL25A: if(_tmp35A != 0){struct
_RegionHandle _tmp362=_new_region("rgn");struct _RegionHandle*rgn=& _tmp362;
_push_region(rgn);{struct _tuple4 _tmp363=({struct _tuple4 _tmp36D;_tmp36D.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp36D.f2=rgn;_tmp36D;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp363,_tmp35A);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A[0]=({struct Cyc_Absyn_FnType_struct
_tmp36B;_tmp36B.tag=8;_tmp36B.f1=({struct Cyc_Absyn_FnInfo _tmp36C;_tmp36C.tvars=0;
_tmp36C.effect=_tmp35B;_tmp36C.ret_typ=(void*)_tmp35C;_tmp36C.args=_tmp35D;
_tmp36C.c_varargs=_tmp35E;_tmp36C.cyc_varargs=_tmp35F;_tmp36C.rgn_po=_tmp360;
_tmp36C.attributes=_tmp361;_tmp36C;});_tmp36B;});_tmp36A;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367[0]=({
struct Cyc_Absyn_PointerType_struct _tmp368;_tmp368.tag=4;_tmp368.f1=({struct Cyc_Absyn_PtrInfo
_tmp369;_tmp369.elt_typ=(void*)ftyp;_tmp369.rgn_typ=(void*)_tmp354;_tmp369.nullable=
_tmp355;_tmp369.tq=_tmp356;_tmp369.bounds=_tmp357;_tmp369;});_tmp368;});_tmp367;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp365;_tmp365.tag=12;_tmp365.f1=inner;
_tmp365.f2=ts;_tmp365;});_tmp364;})));e->topt=({struct Cyc_Core_Opt*_tmp366=
_cycalloc(sizeof(*_tmp366));_tmp366->v=(void*)new_typ;_tmp366;});};_pop_region(
rgn);}goto _LL258;_LL25B:;_LL25C: goto _LL258;_LL258:;}goto _LL253;_LL256:;_LL257:
goto _LL253;_LL253:;}goto _LL24E;_LL24E:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp36E=(void*)e->r;struct Cyc_Absyn_Exp*_tmp36F;struct _tuple1*_tmp370;
struct Cyc_Absyn_Exp*_tmp371;struct Cyc_List_List*_tmp372;struct Cyc_Core_Opt*
_tmp373;struct Cyc_List_List*_tmp374;void*_tmp375;struct _tuple1*_tmp376;void*
_tmp377;void*_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;
void*_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Core_Opt*_tmp37D;struct Cyc_Absyn_Exp*
_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_Absyn_Exp*
_tmp381;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Exp*_tmp383;struct Cyc_Absyn_Exp*
_tmp384;struct Cyc_List_List*_tmp385;struct Cyc_Absyn_VarargCallInfo*_tmp386;
struct Cyc_Absyn_VarargCallInfo**_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_Absyn_Exp*
_tmp389;struct Cyc_List_List*_tmp38A;void*_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;
struct Cyc_Absyn_Exp*_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct Cyc_Absyn_Exp*
_tmp38F;struct Cyc_Absyn_Exp*_tmp390;void*_tmp391;void*_tmp392;void*_tmp393;
struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct _tagged_arr*
_tmp396;struct Cyc_Absyn_Exp*_tmp397;struct _tagged_arr*_tmp398;struct Cyc_Absyn_Exp*
_tmp399;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_List_List*_tmp39B;struct _tuple2*
_tmp39C;struct Cyc_List_List*_tmp39D;struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_Stmt*
_tmp39F;struct Cyc_Absyn_Fndecl*_tmp3A0;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Vardecl*
_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;struct _tuple1*
_tmp3A5;struct _tuple1**_tmp3A6;struct Cyc_List_List*_tmp3A7;struct Cyc_List_List**
_tmp3A8;struct Cyc_List_List*_tmp3A9;struct Cyc_Absyn_Aggrdecl*_tmp3AA;struct Cyc_Absyn_Aggrdecl**
_tmp3AB;void*_tmp3AC;struct Cyc_List_List*_tmp3AD;struct Cyc_List_List*_tmp3AE;
struct Cyc_Absyn_Tuniondecl*_tmp3AF;struct Cyc_Absyn_Tunionfield*_tmp3B0;struct
_tuple1*_tmp3B1;struct _tuple1**_tmp3B2;struct Cyc_Absyn_Enumdecl*_tmp3B3;struct
Cyc_Absyn_Enumfield*_tmp3B4;struct _tuple1*_tmp3B5;struct _tuple1**_tmp3B6;void*
_tmp3B7;struct Cyc_Absyn_Enumfield*_tmp3B8;struct Cyc_Absyn_MallocInfo _tmp3B9;int
_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;void**_tmp3BC;void***_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_Absyn_Exp**_tmp3BF;int _tmp3C0;int*_tmp3C1;_LL25E: if(*((int*)
_tmp36E)!= 11)goto _LL260;_tmp36F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp36E)->f1;_LL25F: Cyc_Tcexp_tcExpNoInst(te,0,_tmp36F);return;_LL260: if(*((int*)
_tmp36E)!= 2)goto _LL262;_tmp370=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp36E)->f1;
_LL261: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp370);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL262: if(*((int*)_tmp36E)!= 8)goto _LL264;_tmp371=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp36E)->f1;_tmp372=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp36E)->f2;_LL263:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp371,_tmp372);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL264: if(*((int*)_tmp36E)!= 34)goto _LL266;_tmp373=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp36E)->f1;_tmp374=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp36E)->f2;
_LL265: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp374);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL266: if(*((int*)_tmp36E)!= 0)goto _LL268;_tmp375=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp36E)->f1;_LL267: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp375,e);goto _LL25D;_LL268: if(*((int*)_tmp36E)!= 1)goto _LL26A;_tmp376=((
struct Cyc_Absyn_Var_e_struct*)_tmp36E)->f1;_tmp377=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp36E)->f2;_LL269: t=Cyc_Tcexp_tcVar(te,loc,_tmp376,_tmp377);goto _LL25D;_LL26A:
if(*((int*)_tmp36E)!= 3)goto _LL26C;_tmp378=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp36E)->f1;_tmp379=((struct Cyc_Absyn_Primop_e_struct*)_tmp36E)->f2;_LL26B: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp378,_tmp379);goto _LL25D;_LL26C: if(*((int*)
_tmp36E)!= 5)goto _LL26E;_tmp37A=((struct Cyc_Absyn_Increment_e_struct*)_tmp36E)->f1;
_tmp37B=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp36E)->f2;_LL26D: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp37A,_tmp37B);goto _LL25D;_LL26E: if(*((int*)_tmp36E)!= 4)goto
_LL270;_tmp37C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp36E)->f1;_tmp37D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp36E)->f2;_tmp37E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp36E)->f3;_LL26F: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp37C,_tmp37D,_tmp37E);
goto _LL25D;_LL270: if(*((int*)_tmp36E)!= 6)goto _LL272;_tmp37F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp36E)->f1;_tmp380=((struct Cyc_Absyn_Conditional_e_struct*)_tmp36E)->f2;
_tmp381=((struct Cyc_Absyn_Conditional_e_struct*)_tmp36E)->f3;_LL271: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp37F,_tmp380,_tmp381);goto _LL25D;_LL272: if(*((int*)_tmp36E)!= 7)
goto _LL274;_tmp382=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp36E)->f1;_tmp383=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp36E)->f2;_LL273: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp382,_tmp383);goto _LL25D;_LL274: if(*((int*)_tmp36E)!= 9)goto _LL276;
_tmp384=((struct Cyc_Absyn_FnCall_e_struct*)_tmp36E)->f1;_tmp385=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp36E)->f2;_tmp386=((struct Cyc_Absyn_FnCall_e_struct*)_tmp36E)->f3;_tmp387=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp36E)->f3;
_LL275: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp384,_tmp385,_tmp387);goto _LL25D;
_LL276: if(*((int*)_tmp36E)!= 10)goto _LL278;_tmp388=((struct Cyc_Absyn_Throw_e_struct*)
_tmp36E)->f1;_LL277: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp388);goto _LL25D;_LL278:
if(*((int*)_tmp36E)!= 12)goto _LL27A;_tmp389=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp36E)->f1;_tmp38A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp36E)->f2;_LL279:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp389,_tmp38A);goto _LL25D;_LL27A: if(*((
int*)_tmp36E)!= 13)goto _LL27C;_tmp38B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp36E)->f1;_tmp38C=((struct Cyc_Absyn_Cast_e_struct*)_tmp36E)->f2;_LL27B: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp38B,_tmp38C);goto _LL25D;_LL27C: if(*((int*)_tmp36E)!= 14)goto
_LL27E;_tmp38D=((struct Cyc_Absyn_Address_e_struct*)_tmp36E)->f1;_LL27D: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp38D);goto _LL25D;_LL27E: if(*((int*)_tmp36E)!= 15)goto _LL280;
_tmp38E=((struct Cyc_Absyn_New_e_struct*)_tmp36E)->f1;_tmp38F=((struct Cyc_Absyn_New_e_struct*)
_tmp36E)->f2;_LL27F: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp38E,e,_tmp38F);goto _LL25D;
_LL280: if(*((int*)_tmp36E)!= 17)goto _LL282;_tmp390=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp36E)->f1;_LL281: {void*_tmp3C2=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp390);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp3C2);goto _LL25D;}_LL282: if(*((int*)_tmp36E)!= 16)goto _LL284;
_tmp391=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp36E)->f1;_LL283: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp391);goto _LL25D;_LL284: if(*((int*)_tmp36E)!= 18)goto _LL286;
_tmp392=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp36E)->f1;_tmp393=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp36E)->f2;_LL285: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp392,_tmp393);goto _LL25D;_LL286: if(*((int*)_tmp36E)!= 19)goto _LL288;
_LL287:({void*_tmp3C3[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(char),35),_tag_arr(_tmp3C3,sizeof(void*),0));});return;_LL288: if(*((int*)
_tmp36E)!= 20)goto _LL28A;_tmp394=((struct Cyc_Absyn_Deref_e_struct*)_tmp36E)->f1;
_LL289: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp394);goto _LL25D;_LL28A: if(*((int*)
_tmp36E)!= 21)goto _LL28C;_tmp395=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp36E)->f1;
_tmp396=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp36E)->f2;_LL28B: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp395,_tmp396);goto _LL25D;_LL28C: if(*((int*)_tmp36E)!= 22)goto
_LL28E;_tmp397=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp36E)->f1;_tmp398=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp36E)->f2;_LL28D: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp397,_tmp398);goto _LL25D;_LL28E: if(*((int*)_tmp36E)!= 23)goto
_LL290;_tmp399=((struct Cyc_Absyn_Subscript_e_struct*)_tmp36E)->f1;_tmp39A=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp36E)->f2;_LL28F: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp399,_tmp39A);goto _LL25D;_LL290: if(*((int*)_tmp36E)!= 24)goto
_LL292;_tmp39B=((struct Cyc_Absyn_Tuple_e_struct*)_tmp36E)->f1;_LL291: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp39B);goto _LL25D;_LL292: if(*((int*)_tmp36E)!= 25)goto _LL294;
_tmp39C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp36E)->f1;_tmp39D=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp36E)->f2;_LL293: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp39C,_tmp39D);goto
_LL25D;_LL294: if(*((int*)_tmp36E)!= 26)goto _LL296;_tmp39E=((struct Cyc_Absyn_Array_e_struct*)
_tmp36E)->f1;_LL295: {void**elt_topt=0;if(topt != 0){void*_tmp3C4=Cyc_Tcutil_compress(*
topt);void*_tmp3C5;void**_tmp3C6;_LL2AB: if(_tmp3C4 <= (void*)3?1:*((int*)_tmp3C4)
!= 7)goto _LL2AD;_tmp3C5=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3C4)->f1;
_tmp3C6=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3C4)->f1);
_LL2AC: elt_topt=(void**)_tmp3C6;goto _LL2AA;_LL2AD:;_LL2AE: goto _LL2AA;_LL2AA:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp39E);goto _LL25D;}_LL296: if(*((int*)_tmp36E)
!= 35)goto _LL298;_tmp39F=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp36E)->f1;_LL297:
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp39F);goto _LL25D;_LL298: if(*((int*)_tmp36E)
!= 36)goto _LL29A;_tmp3A0=((struct Cyc_Absyn_Codegen_e_struct*)_tmp36E)->f1;_LL299:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp3A0);goto _LL25D;_LL29A: if(*((int*)_tmp36E)
!= 37)goto _LL29C;_tmp3A1=((struct Cyc_Absyn_Fill_e_struct*)_tmp36E)->f1;_LL29B: t=
Cyc_Tcexp_tcFill(te,loc,topt,_tmp3A1);goto _LL25D;_LL29C: if(*((int*)_tmp36E)!= 27)
goto _LL29E;_tmp3A2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp36E)->f1;
_tmp3A3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp36E)->f2;_tmp3A4=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp36E)->f3;_LL29D: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp3A2,_tmp3A3,_tmp3A4);goto _LL25D;_LL29E: if(*((int*)_tmp36E)!= 28)
goto _LL2A0;_tmp3A5=((struct Cyc_Absyn_Struct_e_struct*)_tmp36E)->f1;_tmp3A6=(
struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp36E)->f1;_tmp3A7=((
struct Cyc_Absyn_Struct_e_struct*)_tmp36E)->f2;_tmp3A8=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp36E)->f2;_tmp3A9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp36E)->f3;_tmp3AA=((struct Cyc_Absyn_Struct_e_struct*)_tmp36E)->f4;_tmp3AB=(
struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp36E)->f4;
_LL29F: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp3A6,_tmp3A8,_tmp3A9,_tmp3AB);goto
_LL25D;_LL2A0: if(*((int*)_tmp36E)!= 29)goto _LL2A2;_tmp3AC=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp36E)->f1;_tmp3AD=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp36E)->f2;_LL2A1:
t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp3AC,_tmp3AD);goto _LL25D;_LL2A2: if(*((int*)
_tmp36E)!= 30)goto _LL2A4;_tmp3AE=((struct Cyc_Absyn_Tunion_e_struct*)_tmp36E)->f1;
_tmp3AF=((struct Cyc_Absyn_Tunion_e_struct*)_tmp36E)->f2;_tmp3B0=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp36E)->f3;_LL2A3: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp3AE,_tmp3AF,_tmp3B0);
goto _LL25D;_LL2A4: if(*((int*)_tmp36E)!= 31)goto _LL2A6;_tmp3B1=((struct Cyc_Absyn_Enum_e_struct*)
_tmp36E)->f1;_tmp3B2=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp36E)->f1;
_tmp3B3=((struct Cyc_Absyn_Enum_e_struct*)_tmp36E)->f2;_tmp3B4=((struct Cyc_Absyn_Enum_e_struct*)
_tmp36E)->f3;_LL2A5:*_tmp3B2=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp3B4))->name;
t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));
_tmp3C7[0]=({struct Cyc_Absyn_EnumType_struct _tmp3C8;_tmp3C8.tag=12;_tmp3C8.f1=((
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3B3))->name;_tmp3C8.f2=_tmp3B3;_tmp3C8;});
_tmp3C7;});goto _LL25D;_LL2A6: if(*((int*)_tmp36E)!= 32)goto _LL2A8;_tmp3B5=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp36E)->f1;_tmp3B6=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp36E)->f1;_tmp3B7=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp36E)->f2;_tmp3B8=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp36E)->f3;_LL2A7:*
_tmp3B6=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp3B8))->name;t=_tmp3B7;goto
_LL25D;_LL2A8: if(*((int*)_tmp36E)!= 33)goto _LL25D;_tmp3B9=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp36E)->f1;_tmp3BA=_tmp3B9.is_calloc;_tmp3BB=_tmp3B9.rgn;_tmp3BC=_tmp3B9.elt_type;
_tmp3BD=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp36E)->f1).elt_type;
_tmp3BE=_tmp3B9.num_elts;_tmp3BF=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp36E)->f1).num_elts;_tmp3C0=_tmp3B9.fat_result;_tmp3C1=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp36E)->f1).fat_result;_LL2A9: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp3BB,_tmp3BD,
_tmp3BF,_tmp3BA,_tmp3C1);goto _LL25D;_LL25D:;}e->topt=({struct Cyc_Core_Opt*
_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->v=(void*)t;_tmp3C9;});}
