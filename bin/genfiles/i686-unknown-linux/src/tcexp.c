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
struct _tagged_arr,struct _tagged_arr);typedef struct{int __count;union{unsigned int
__wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;typedef struct{int __pos;Cyc_Std___mbstate_t
__state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
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
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(
void*t);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
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
_LLF:(void)_throw(_tmp1A);_LLB:;}}}struct _tuple7{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple7*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple7*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->f1=0;_tmp1D->f2=
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
_tmp2E=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct
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
sizeof(char),37),_tag_arr(_tmpDE,sizeof(void*),0));});}return t;}}struct _tuple8{
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
_tmpF1)->f1;_LLE1: {unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmpE9);{struct
_handler_cons _tmpF6;_push_handler(& _tmpF6);{int _tmpF8=0;if(setjmp(_tmpF6.handler))
_tmpF8=1;if(!_tmpF8){{struct _tuple8 _tmpFA;struct Cyc_Absyn_Tqual _tmpFB;struct
_tuple8*_tmpF9=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmpF5,(int)i);_tmpFA=*_tmpF9;_tmpFB=_tmpFA.f1;if(!_tmpFB.q_const){_npop_handler(
0);return;}};_pop_handler();}else{void*_tmpF7=(void*)_exn_thrown;void*_tmpFD=
_tmpF7;_LLE5: if(_tmpFD != Cyc_List_Nth)goto _LLE7;_LLE6: return;_LLE7:;_LLE8:(void)
_throw(_tmpFD);_LLE4:;}}}goto _LLDB;}_LLE2:;_LLE3: goto _LLDB;_LLDB:;}goto _LLC4;
_LLCF: if(*((int*)_tmpDF)!= 21)goto _LLD1;_tmpEA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpDF)->f1;_tmpEB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDF)->f2;_LLD0:{
void*_tmpFE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEA->topt))->v);
struct Cyc_Absyn_AggrInfo _tmpFF;void*_tmp100;struct Cyc_Absyn_Aggrdecl**_tmp101;
struct Cyc_List_List*_tmp102;_LLEA: if(_tmpFE <= (void*)3?1:*((int*)_tmpFE)!= 10)
goto _LLEC;_tmpFF=((struct Cyc_Absyn_AggrType_struct*)_tmpFE)->f1;_tmp100=(void*)
_tmpFF.aggr_info;if(*((int*)_tmp100)!= 1)goto _LLEC;_tmp101=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp100)->f1;_LLEB: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp101 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp101,_tmpEB);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLE9;}_LLEC: if(_tmpFE <= (void*)3?1:*((int*)_tmpFE)!= 11)goto _LLEE;
_tmp102=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpFE)->f2;_LLED: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp102,_tmpEB);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: if(*((int*)_tmpDF)!= 
22)goto _LLD3;_tmpEC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDF)->f1;_tmpED=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDF)->f2;_LLD2:{void*_tmp103=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEC->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp104;void*_tmp105;_LLF1: if(_tmp103 <= (void*)3?1:*((int*)_tmp103)!= 4)goto
_LLF3;_tmp104=((struct Cyc_Absyn_PointerType_struct*)_tmp103)->f1;_tmp105=(void*)
_tmp104.elt_typ;_LLF2:{void*_tmp106=Cyc_Tcutil_compress(_tmp105);struct Cyc_Absyn_AggrInfo
_tmp107;void*_tmp108;struct Cyc_Absyn_Aggrdecl**_tmp109;struct Cyc_List_List*
_tmp10A;_LLF6: if(_tmp106 <= (void*)3?1:*((int*)_tmp106)!= 10)goto _LLF8;_tmp107=((
struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1;_tmp108=(void*)_tmp107.aggr_info;
if(*((int*)_tmp108)!= 1)goto _LLF8;_tmp109=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp108)->f1;_LLF7: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp109 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp109,_tmpED);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLF5;}_LLF8: if(_tmp106 <= (void*)3?1:*((int*)_tmp106)!= 11)goto _LLFA;
_tmp10A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp106)->f2;_LLF9: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp10A,_tmpED);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto _LLF0;_LLF0:;}
goto _LLC4;_LLD3: if(*((int*)_tmpDF)!= 20)goto _LLD5;_tmpEE=((struct Cyc_Absyn_Deref_e_struct*)
_tmpDF)->f1;_LLD4:{void*_tmp10B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE->topt))->v);struct Cyc_Absyn_PtrInfo _tmp10C;struct Cyc_Absyn_Tqual
_tmp10D;struct Cyc_Absyn_Tqual _tmp10E;_LLFD: if(_tmp10B <= (void*)3?1:*((int*)
_tmp10B)!= 4)goto _LLFF;_tmp10C=((struct Cyc_Absyn_PointerType_struct*)_tmp10B)->f1;
_tmp10D=_tmp10C.tq;_LLFE: _tmp10E=_tmp10D;goto _LL100;_LLFF: if(_tmp10B <= (void*)3?
1:*((int*)_tmp10B)!= 7)goto _LL101;_tmp10E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10B)->f2;_LL100: if(!_tmp10E.q_const)return;goto _LLFC;_LL101:;_LL102: goto
_LLFC;_LLFC:;}goto _LLC4;_LLD5: if(*((int*)_tmpDF)!= 11)goto _LLD7;_tmpEF=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmpDF)->f1;_LLD6: _tmpF0=_tmpEF;goto _LLD8;
_LLD7: if(*((int*)_tmpDF)!= 12)goto _LLD9;_tmpF0=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpDF)->f1;_LLD8: Cyc_Tcexp_check_writable(te,_tmpF0);return;_LLD9:;_LLDA: goto
_LLC4;_LLC4:;}({struct Cyc_Std_String_pa_struct _tmp110;_tmp110.tag=0;_tmp110.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp10F[1]={& _tmp110};Cyc_Tcutil_terr(
e->loc,_tag_arr("attempt to write a const location: %s",sizeof(char),38),
_tag_arr(_tmp10F,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp111[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(char),34),_tag_arr(_tmp111,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt)){if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp112[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("can't perform arithmetic on abstract pointer type",sizeof(char),50),
_tag_arr(_tmp112,sizeof(void*),0));});}else{({struct Cyc_Std_String_pa_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp113[1]={& _tmp114};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting arithmetic or ? type but found %s",
sizeof(char),44),_tag_arr(_tmp113,sizeof(void*),1));}});}}return t;}}static void*
Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(char),23));Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp11C;}));struct Cyc_List_List _tmp115=({struct
Cyc_List_List _tmp11B;_tmp11B.hd=e3;_tmp11B.tl=0;_tmp11B;});struct Cyc_List_List
_tmp116=({struct Cyc_List_List _tmp11A;_tmp11A.hd=e2;_tmp11A.tl=(struct Cyc_List_List*)&
_tmp115;_tmp11A;});if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp116)){({struct Cyc_Std_String_pa_struct _tmp119;_tmp119.tag=0;_tmp119.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v);{struct Cyc_Std_String_pa_struct _tmp118;_tmp118.tag=0;
_tmp118.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmp117[2]={& _tmp118,& _tmp119};Cyc_Tcutil_terr(
loc,_tag_arr("conditional clause types do not match: %s != %s",sizeof(char),48),
_tag_arr(_tmp117,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp11D=Cyc_Tcutil_compress(t1);_LL104: if(_tmp11D <= (void*)3?1:*((int*)
_tmp11D)!= 7)goto _LL106;_LL105:({void*_tmp11E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",
sizeof(char),26),_tag_arr(_tmp11E,sizeof(void*),0));});goto _LL103;_LL106:;_LL107:
goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1))({
void*_tmp11F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(char),41),_tag_arr(_tmp11F,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e1))return({void*_tmp120[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(char),25),_tag_arr(_tmp120,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp121=({struct
Cyc_Std_String_pa_struct _tmp124;_tmp124.tag=0;_tmp124.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp122[2]={& _tmp123,& _tmp124};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type mismatch: %s != %s",sizeof(char),24),_tag_arr(_tmp122,
sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp121;}}else{void*_tmp125=(void*)po->v;void*_tmp126=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp125,e1,e2);if(!(Cyc_Tcutil_unify(_tmp126,t1)?1:(Cyc_Tcutil_coerceable(
_tmp126)?Cyc_Tcutil_coerceable(t1): 0))){void*_tmp127=({struct Cyc_Std_String_pa_struct
_tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp129;_tmp129.tag=0;_tmp129.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp128[2]={& _tmp129,& _tmp12A};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(char),82),_tag_arr(_tmp128,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp126,t1);Cyc_Tcutil_explain_failure();return _tmp127;}return _tmp126;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct dbl={6,0};static
void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;{void*_tmp12B=Cyc_Tcutil_compress(t1);void*_tmp12C;void*
_tmp12D;_LL109: if((int)_tmp12B != 1)goto _LL10B;_LL10A: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ);t1=dbl_typ;goto _LL108;_LL10B: if(_tmp12B <= (void*)3?1:*((int*)
_tmp12B)!= 5)goto _LL10D;_tmp12C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp12B)->f2;
if((int)_tmp12C != 0)goto _LL10D;_LL10C: goto _LL10E;_LL10D: if(_tmp12B <= (void*)3?1:*((
int*)_tmp12B)!= 5)goto _LL10F;_tmp12D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp12B)->f2;if((int)_tmp12D != 1)goto _LL10F;_LL10E: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ);t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;
_LL108:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp12F;
struct Cyc_List_List*_tmp130;struct Cyc_Position_Segment*_tmp131;void*_tmp132;
struct Cyc_Absyn_Tunionfield _tmp12E=*((struct Cyc_Absyn_Tunionfield*)fields->hd);
_tmp12F=_tmp12E.name;_tmp130=_tmp12E.typs;_tmp131=_tmp12E.loc;_tmp132=(void*)
_tmp12E.sc;if(_tmp130 == 0?1: _tmp130->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple8*)_tmp130->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(
fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp134;struct Cyc_List_List*
_tmp135;struct Cyc_Position_Segment*_tmp136;void*_tmp137;struct Cyc_Absyn_Tunionfield
_tmp133=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp134=_tmp133.name;_tmp135=
_tmp133.typs;_tmp136=_tmp133.loc;_tmp137=(void*)_tmp133.sc;if(_tmp135 == 0?1:
_tmp135->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple8*)_tmp135->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}({struct Cyc_Std_String_pa_struct
_tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
struct Cyc_Std_String_pa_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(tu);{void*_tmp138[2]={& _tmp139,& _tmp13A};Cyc_Tcutil_terr(
e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",sizeof(
char),54),_tag_arr(_tmp138,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp13C=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp13D=t;struct Cyc_Absyn_PtrInfo _tmp13E;void*_tmp13F;void*_tmp140;struct
Cyc_Absyn_Conref*_tmp141;struct Cyc_Absyn_Tqual _tmp142;struct Cyc_Absyn_Conref*
_tmp143;_LL112: if(_tmp13D <= (void*)3?1:*((int*)_tmp13D)!= 4)goto _LL114;_tmp13E=((
struct Cyc_Absyn_PointerType_struct*)_tmp13D)->f1;_tmp13F=(void*)_tmp13E.elt_typ;
_tmp140=(void*)_tmp13E.rgn_typ;_tmp141=_tmp13E.nullable;_tmp142=_tmp13E.tq;
_tmp143=_tmp13E.bounds;_LL113: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp140);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp143);{void*_tmp144=Cyc_Tcutil_compress(_tmp13F);struct Cyc_Absyn_FnInfo
_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_Core_Opt*_tmp147;void*_tmp148;
struct Cyc_List_List*_tmp149;int _tmp14A;struct Cyc_Absyn_VarargInfo*_tmp14B;struct
Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp14D;_LL117: if(_tmp144 <= (void*)3?1:*((
int*)_tmp144)!= 8)goto _LL119;_tmp145=((struct Cyc_Absyn_FnType_struct*)_tmp144)->f1;
_tmp146=_tmp145.tvars;_tmp147=_tmp145.effect;_tmp148=(void*)_tmp145.ret_typ;
_tmp149=_tmp145.args;_tmp14A=_tmp145.c_varargs;_tmp14B=_tmp145.cyc_varargs;
_tmp14C=_tmp145.rgn_po;_tmp14D=_tmp145.attributes;_LL118: if(topt != 0)Cyc_Tcutil_unify(
_tmp148,*topt);while(_tmp13C != 0?_tmp149 != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp13C->hd;void*t2=(*((struct _tuple2*)_tmp149->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp150;_tmp150.tag=0;_tmp150.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp14E[2]={& _tmp14F,& _tmp150};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(char),54),_tag_arr(_tmp14E,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp13C=_tmp13C->tl;_tmp149=_tmp149->tl;}if(_tmp149 != 0)({void*_tmp151[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too few arguments for function",sizeof(char),31),_tag_arr(_tmp151,
sizeof(void*),0));});else{if((_tmp13C != 0?1: _tmp14A)?1: _tmp14B != 0){if(_tmp14A)
for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp13C->hd);}else{if(_tmp14B == 0)({void*_tmp152[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("too many arguments for function",sizeof(char),32),_tag_arr(_tmp152,
sizeof(void*),0));});else{void*_tmp154;int _tmp155;struct Cyc_Absyn_VarargInfo
_tmp153=*_tmp14B;_tmp154=(void*)_tmp153.type;_tmp155=_tmp153.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp156=({struct Cyc_Absyn_VarargCallInfo*_tmp16A=_cycalloc(sizeof(*_tmp16A));
_tmp16A->num_varargs=0;_tmp16A->injectors=0;_tmp16A->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp14B);_tmp16A;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp156;if(!_tmp155)for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp13C->hd;_tmp156->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp154,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp154)){({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp154);{void*_tmp157[2]={& _tmp158,& _tmp159};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(char),49),_tag_arr(_tmp157,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp15A=Cyc_Tcutil_compress(_tmp154);struct Cyc_Absyn_TunionInfo _tmp15B;
void*_tmp15C;struct Cyc_Absyn_Tuniondecl**_tmp15D;struct Cyc_Absyn_Tuniondecl*
_tmp15E;struct Cyc_List_List*_tmp15F;void*_tmp160;_LL11C: if(_tmp15A <= (void*)3?1:*((
int*)_tmp15A)!= 2)goto _LL11E;_tmp15B=((struct Cyc_Absyn_TunionType_struct*)
_tmp15A)->f1;_tmp15C=(void*)_tmp15B.tunion_info;if(*((int*)_tmp15C)!= 1)goto
_LL11E;_tmp15D=((struct Cyc_Absyn_KnownTunion_struct*)_tmp15C)->f1;_tmp15E=*
_tmp15D;_tmp15F=_tmp15B.targs;_tmp160=(void*)_tmp15B.rgn;_LL11D: {struct Cyc_Absyn_Tuniondecl*
_tmp161=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp15E->name);struct Cyc_List_List*
fields=0;if(_tmp161->fields == 0)({struct Cyc_Std_String_pa_struct _tmp163;_tmp163.tag=
0;_tmp163.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp154);{void*_tmp162[1]={&
_tmp163};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",sizeof(char),21),
_tag_arr(_tmp162,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp161->fields))->v;}if(!Cyc_Tcutil_unify(
_tmp160,Cyc_Tcenv_curr_rgn(te)))({void*_tmp164[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bad region for injected varargs",sizeof(char),32),_tag_arr(_tmp164,
sizeof(void*),0));});{struct _RegionHandle _tmp165=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp165;_push_region(rgn);{struct Cyc_List_List*_tmp166=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp161->tvs,_tmp15F);for(0;
_tmp13C != 0;_tmp13C=_tmp13C->tl){_tmp156->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp13C->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp167=Cyc_Tcexp_tcInjection(te,e1,_tmp154,rgn,_tmp166,fields);if(_tmp167 != 0)
_tmp156->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp156->injectors,({struct Cyc_List_List*_tmp168=_cycalloc(
sizeof(*_tmp168));_tmp168->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp167);
_tmp168->tl=0;_tmp168;}));}}}};_pop_region(rgn);}goto _LL11B;}_LL11E:;_LL11F:({
void*_tmp169[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
char),23),_tag_arr(_tmp169,sizeof(void*),0));});goto _LL11B;_LL11B:;}}}}}}{struct
Cyc_List_List*a=_tmp14D;for(0;a != 0;a=a->tl){void*_tmp16B=(void*)a->hd;void*
_tmp16C;int _tmp16D;int _tmp16E;_LL121: if(_tmp16B <= (void*)16?1:*((int*)_tmp16B)!= 
3)goto _LL123;_tmp16C=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp16B)->f1;
_tmp16D=((struct Cyc_Absyn_Format_att_struct*)_tmp16B)->f2;_tmp16E=((struct Cyc_Absyn_Format_att_struct*)
_tmp16B)->f3;_LL122:{struct _handler_cons _tmp16F;_push_handler(& _tmp16F);{int
_tmp171=0;if(setjmp(_tmp16F.handler))_tmp171=1;if(!_tmp171){{struct Cyc_Absyn_Exp*
_tmp172=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp16D - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp16E == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp16E - 1);_tmp173;});}{
void*_tmp174=_tmp16C;_LL126: if((int)_tmp174 != 0)goto _LL128;_LL127: Cyc_Tcexp_check_format_args(
te,_tmp172,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL125;_LL128: if((int)
_tmp174 != 1)goto _LL125;_LL129: Cyc_Tcexp_check_format_args(te,_tmp172,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL125;_LL125:;}};_pop_handler();}else{void*
_tmp170=(void*)_exn_thrown;void*_tmp176=_tmp170;_LL12B: if(_tmp176 != Cyc_List_Nth)
goto _LL12D;_LL12C:({void*_tmp177[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",
sizeof(char),21),_tag_arr(_tmp177,sizeof(void*),0));});goto _LL12A;_LL12D:;_LL12E:(
void)_throw(_tmp176);_LL12A:;}}}goto _LL120;_LL123:;_LL124: goto _LL120;_LL120:;}}
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp147))->v);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp14C);
return _tmp148;_LL119:;_LL11A: return({void*_tmp178[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("expected pointer to function",sizeof(char),29),_tag_arr(_tmp178,
sizeof(void*),0));});_LL116:;}_LL114:;_LL115: return({void*_tmp179[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(char),29),_tag_arr(_tmp179,
sizeof(void*),0));});_LL111:;}}static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({
struct Cyc_Std_String_pa_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp17A[1]={& _tmp17B};Cyc_Tcutil_terr(loc,_tag_arr("expected xtunion exn but found %s",
sizeof(char),34),_tag_arr(_tmp17A,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp17C;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp17D=t1;struct Cyc_Absyn_PtrInfo _tmp17E;void*_tmp17F;void*_tmp180;struct
Cyc_Absyn_Conref*_tmp181;struct Cyc_Absyn_Tqual _tmp182;struct Cyc_Absyn_Conref*
_tmp183;_LL130: if(_tmp17D <= (void*)3?1:*((int*)_tmp17D)!= 4)goto _LL132;_tmp17E=((
struct Cyc_Absyn_PointerType_struct*)_tmp17D)->f1;_tmp17F=(void*)_tmp17E.elt_typ;
_tmp180=(void*)_tmp17E.rgn_typ;_tmp181=_tmp17E.nullable;_tmp182=_tmp17E.tq;
_tmp183=_tmp17E.bounds;_LL131:{void*_tmp184=Cyc_Tcutil_compress(_tmp17F);struct
Cyc_Absyn_FnInfo _tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Core_Opt*_tmp187;
void*_tmp188;struct Cyc_List_List*_tmp189;int _tmp18A;struct Cyc_Absyn_VarargInfo*
_tmp18B;struct Cyc_List_List*_tmp18C;struct Cyc_List_List*_tmp18D;_LL135: if(
_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 8)goto _LL137;_tmp185=((struct Cyc_Absyn_FnType_struct*)
_tmp184)->f1;_tmp186=_tmp185.tvars;_tmp187=_tmp185.effect;_tmp188=(void*)_tmp185.ret_typ;
_tmp189=_tmp185.args;_tmp18A=_tmp185.c_varargs;_tmp18B=_tmp185.cyc_varargs;
_tmp18C=_tmp185.rgn_po;_tmp18D=_tmp185.attributes;_LL136: {struct Cyc_List_List*
instantiation=0;for(0;ts != 0?_tmp186 != 0: 0;(ts=ts->tl,_tmp186=_tmp186->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp186->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->hd=({struct _tuple5*
_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->f1=(struct Cyc_Absyn_Tvar*)_tmp186->hd;
_tmp18F->f2=(void*)ts->hd;_tmp18F;});_tmp18E->tl=instantiation;_tmp18E;});}if(ts
!= 0)return({void*_tmp190[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too many type variables in instantiation",
sizeof(char),41),_tag_arr(_tmp190,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp194=_cycalloc(sizeof(*
_tmp194));_tmp194[0]=({struct Cyc_Absyn_FnType_struct _tmp195;_tmp195.tag=8;
_tmp195.f1=({struct Cyc_Absyn_FnInfo _tmp196;_tmp196.tvars=_tmp186;_tmp196.effect=
_tmp187;_tmp196.ret_typ=(void*)_tmp188;_tmp196.args=_tmp189;_tmp196.c_varargs=
_tmp18A;_tmp196.cyc_varargs=_tmp18B;_tmp196.rgn_po=_tmp18C;_tmp196.attributes=
_tmp18D;_tmp196;});_tmp195;});_tmp194;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Absyn_PointerType_struct
_tmp192;_tmp192.tag=4;_tmp192.f1=({struct Cyc_Absyn_PtrInfo _tmp193;_tmp193.elt_typ=(
void*)new_fn_typ;_tmp193.rgn_typ=(void*)_tmp180;_tmp193.nullable=_tmp181;_tmp193.tq=
_tmp182;_tmp193.bounds=_tmp183;_tmp193;});_tmp192;});_tmp191;});}}_LL137:;_LL138:
goto _LL134;_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}return({struct
Cyc_Std_String_pa_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp197[1]={& _tmp198};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(char),40),_tag_arr(_tmp197,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(te,loc,t2,t)?!Cyc_Tcutil_castable(
te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp199=({struct Cyc_Std_String_pa_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp19B;
_tmp19B.tag=0;_tmp19B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*
_tmp19A[2]={& _tmp19B,& _tmp19C};Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",
sizeof(char),21),_tag_arr(_tmp19A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp199;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp19D=0;struct Cyc_Absyn_Tqual _tmp19E=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp19F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1A0;
void*_tmp1A1;struct Cyc_Absyn_Tqual _tmp1A2;_LL13A: if(_tmp19F <= (void*)3?1:*((int*)
_tmp19F)!= 4)goto _LL13C;_tmp1A0=((struct Cyc_Absyn_PointerType_struct*)_tmp19F)->f1;
_tmp1A1=(void*)_tmp1A0.elt_typ;_tmp1A2=_tmp1A0.tq;_LL13B: _tmp19D=({void**_tmp1A3=
_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=_tmp1A1;_tmp1A3;});_tmp19E=_tmp1A2;goto
_LL139;_LL13C:;_LL13D: goto _LL139;_LL139:;}Cyc_Tcexp_tcExpNoInst(te,_tmp19D,e);{
void*_tmp1A4=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;
_LL13F: if(*((int*)_tmp1A4)!= 28)goto _LL141;_LL140: goto _LL142;_LL141: if(*((int*)
_tmp1A4)!= 24)goto _LL143;_LL142:({void*_tmp1A7[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("& used to allocate",sizeof(char),19),_tag_arr(_tmp1A7,sizeof(void*),0));});{
void*(*_tmp1A8)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;if(
topt != 0){void*_tmp1A9=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1AA;
struct Cyc_Absyn_Conref*_tmp1AB;_LL148: if(_tmp1A9 <= (void*)3?1:*((int*)_tmp1A9)!= 
4)goto _LL14A;_tmp1AA=((struct Cyc_Absyn_PointerType_struct*)_tmp1A9)->f1;_tmp1AB=
_tmp1AA.nullable;_LL149: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp1AB))_tmp1A8=Cyc_Absyn_star_typ;goto _LL147;_LL14A:;_LL14B: goto _LL147;_LL147:;}
return _tmp1A8((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp19E);}_LL143: if(*((int*)_tmp1A4)!= 23)goto _LL145;_tmp1A5=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1A4)->f1;_tmp1A6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A4)->f2;_LL144:{
void*_tmp1AC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1A5->topt))->v);_LL14D: if(_tmp1AC <= (void*)3?1:*((int*)_tmp1AC)!= 9)goto
_LL14F;_LL14E: goto _LL14C;_LL14F:;_LL150:(void*)(e0->r=(void*)((void*)(Cyc_Absyn_add_exp(
_tmp1A5,_tmp1A6,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp1A5,_tmp1A6);_LL14C:;}
goto _LL13E;_LL145:;_LL146: goto _LL13E;_LL13E:;}{int _tmp1AE;void*_tmp1AF;struct
_tuple6 _tmp1AD=Cyc_Tcutil_addressof_props(te,e);_tmp1AE=_tmp1AD.f1;_tmp1AF=
_tmp1AD.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp1AE)tq.q_const=
1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp1AF,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_Std_String_pa_struct _tmp1B1;_tmp1B1.tag=0;_tmp1B1.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1B0[1]={& _tmp1B1};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
char),55),_tag_arr(_tmp1B0,sizeof(void*),1));}});if(topt != 0){void*_tmp1B2=Cyc_Tcutil_compress(*
topt);_LL152: if(_tmp1B2 <= (void*)3?1:*((int*)_tmp1B2)!= 14)goto _LL154;_LL153:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1B3=_cycalloc(sizeof(*
_tmp1B3));_tmp1B3[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1B4;_tmp1B4.tag=14;
_tmp1B4.f1=(void*)t;_tmp1B4;});_tmp1B3;});_LL154:;_LL155: goto _LL151;_LL151:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_Std_strcmp(*n,*sf->name)== 0;}static void*
Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);{void*_tmp1B5=n;struct _tagged_arr*_tmp1B6;unsigned int _tmp1B7;
_LL157: if(*((int*)_tmp1B5)!= 0)goto _LL159;_tmp1B6=((struct Cyc_Absyn_StructField_struct*)
_tmp1B5)->f1;_LL158: {int bad_type=1;{void*_tmp1B8=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1B9;void*_tmp1BA;struct Cyc_Absyn_Aggrdecl**_tmp1BB;struct
Cyc_List_List*_tmp1BC;_LL15C: if(_tmp1B8 <= (void*)3?1:*((int*)_tmp1B8)!= 10)goto
_LL15E;_tmp1B9=((struct Cyc_Absyn_AggrType_struct*)_tmp1B8)->f1;_tmp1BA=(void*)
_tmp1B9.aggr_info;if(*((int*)_tmp1BA)!= 1)goto _LL15E;_tmp1BB=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1BA)->f1;_LL15D: if((*_tmp1BB)->impl == 0)goto _LL15B;if(!((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp1B6,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp1BB)->impl))->fields))({struct Cyc_Std_String_pa_struct _tmp1BE;
_tmp1BE.tag=0;_tmp1BE.f1=(struct _tagged_arr)*_tmp1B6;{void*_tmp1BD[1]={& _tmp1BE};
Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",sizeof(char),
37),_tag_arr(_tmp1BD,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL15E: if(
_tmp1B8 <= (void*)3?1:*((int*)_tmp1B8)!= 11)goto _LL160;_tmp1BC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B8)->f2;_LL15F: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1B6,_tmp1BC))({struct Cyc_Std_String_pa_struct _tmp1C0;_tmp1C0.tag=0;_tmp1C0.f1=(
struct _tagged_arr)*_tmp1B6;{void*_tmp1BF[1]={& _tmp1C0};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct/union has name %s",sizeof(char),37),_tag_arr(
_tmp1BF,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL160:;_LL161: goto _LL15B;
_LL15B:;}if(bad_type)({struct Cyc_Std_String_pa_struct _tmp1C2;_tmp1C2.tag=0;
_tmp1C2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1C1[1]={&
_tmp1C2};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(char),36),_tag_arr(_tmp1C1,sizeof(void*),1));}});goto _LL156;}_LL159: if(*((
int*)_tmp1B5)!= 1)goto _LL156;_tmp1B7=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp1B5)->f1;_LL15A: {int bad_type=1;{void*_tmp1C3=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1C4;void*_tmp1C5;struct Cyc_Absyn_Aggrdecl**_tmp1C6;struct
Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_TunionFieldInfo
_tmp1C9;void*_tmp1CA;struct Cyc_Absyn_Tunionfield*_tmp1CB;_LL163: if(_tmp1C3 <= (
void*)3?1:*((int*)_tmp1C3)!= 10)goto _LL165;_tmp1C4=((struct Cyc_Absyn_AggrType_struct*)
_tmp1C3)->f1;_tmp1C5=(void*)_tmp1C4.aggr_info;if(*((int*)_tmp1C5)!= 1)goto _LL165;
_tmp1C6=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1C5)->f1;_LL164: if((*_tmp1C6)->impl
== 0)goto _LL162;_tmp1C7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp1C6)->impl))->fields;
goto _LL166;_LL165: if(_tmp1C3 <= (void*)3?1:*((int*)_tmp1C3)!= 11)goto _LL167;
_tmp1C7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C3)->f2;_LL166: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1C7)<= _tmp1B7)({struct Cyc_Std_Int_pa_struct
_tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=(int)_tmp1B7;{struct Cyc_Std_Int_pa_struct
_tmp1CD;_tmp1CD.tag=1;_tmp1CD.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1C7));{void*_tmp1CC[2]={& _tmp1CD,& _tmp1CE};Cyc_Tcutil_terr(
loc,_tag_arr("struct/union has too few components: %d <= %d",sizeof(char),46),
_tag_arr(_tmp1CC,sizeof(void*),2));}}});bad_type=0;goto _LL162;_LL167: if(_tmp1C3
<= (void*)3?1:*((int*)_tmp1C3)!= 9)goto _LL169;_tmp1C8=((struct Cyc_Absyn_TupleType_struct*)
_tmp1C3)->f1;_LL168: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C8)
<= _tmp1B7)({struct Cyc_Std_Int_pa_struct _tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=(int)
_tmp1B7;{struct Cyc_Std_Int_pa_struct _tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C8));{void*
_tmp1CF[2]={& _tmp1D0,& _tmp1D1};Cyc_Tcutil_terr(loc,_tag_arr("tuple has too few components: %d <= %d",
sizeof(char),39),_tag_arr(_tmp1CF,sizeof(void*),2));}}});bad_type=0;goto _LL162;
_LL169: if(_tmp1C3 <= (void*)3?1:*((int*)_tmp1C3)!= 3)goto _LL16B;_tmp1C9=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp1C3)->f1;_tmp1CA=(void*)_tmp1C9.field_info;
if(*((int*)_tmp1CA)!= 1)goto _LL16B;_tmp1CB=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1CA)->f2;_LL16A: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CB->typs)
< _tmp1B7)({struct Cyc_Std_Int_pa_struct _tmp1D4;_tmp1D4.tag=1;_tmp1D4.f1=(int)
_tmp1B7;{struct Cyc_Std_Int_pa_struct _tmp1D3;_tmp1D3.tag=1;_tmp1D3.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CB->typs));{
void*_tmp1D2[2]={& _tmp1D3,& _tmp1D4};Cyc_Tcutil_terr(loc,_tag_arr("tunionfield has too few components: %d < %d",
sizeof(char),44),_tag_arr(_tmp1D2,sizeof(void*),2));}}});bad_type=0;goto _LL162;
_LL16B:;_LL16C: goto _LL162;_LL162:;}if(bad_type)({struct Cyc_Std_String_pa_struct
_tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1D5[1]={& _tmp1D6};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known type",
sizeof(char),23),_tag_arr(_tmp1D5,sizeof(void*),1));}});goto _LL156;}_LL156:;}
return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1D7=t;struct Cyc_Absyn_PtrInfo _tmp1D8;void*_tmp1D9;void*_tmp1DA;struct
Cyc_Absyn_Conref*_tmp1DB;_LL16E: if(_tmp1D7 <= (void*)3?1:*((int*)_tmp1D7)!= 4)
goto _LL170;_tmp1D8=((struct Cyc_Absyn_PointerType_struct*)_tmp1D7)->f1;_tmp1D9=(
void*)_tmp1D8.elt_typ;_tmp1DA=(void*)_tmp1D8.rgn_typ;_tmp1DB=_tmp1D8.bounds;
_LL16F: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1DA);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1DB);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1D9),(void*)1))({
void*_tmp1DC[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(char),40),_tag_arr(_tmp1DC,sizeof(void*),0));});return _tmp1D9;_LL170:;
_LL171: return({struct Cyc_Std_String_pa_struct _tmp1DE;_tmp1DE.tag=0;_tmp1DE.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1DD[1]={& _tmp1DE};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting * or @ type but found %s",sizeof(char),35),_tag_arr(
_tmp1DD,sizeof(void*),1));}});_LL16D:;}}static void*Cyc_Tcexp_tcAggrMember(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp1DF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp1E0;void*_tmp1E1;struct Cyc_Absyn_Aggrdecl**_tmp1E2;
struct Cyc_Absyn_Aggrdecl*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*
_tmp1E5;_LL173: if(_tmp1DF <= (void*)3?1:*((int*)_tmp1DF)!= 10)goto _LL175;_tmp1E0=((
struct Cyc_Absyn_AggrType_struct*)_tmp1DF)->f1;_tmp1E1=(void*)_tmp1E0.aggr_info;
if(*((int*)_tmp1E1)!= 1)goto _LL175;_tmp1E2=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1E1)->f1;_tmp1E3=*_tmp1E2;_tmp1E4=_tmp1E0.targs;_LL174: {struct Cyc_Absyn_Aggrfield*
_tmp1E6=Cyc_Absyn_lookup_decl_field(_tmp1E3,f);if(_tmp1E6 == 0)return({struct Cyc_Std_String_pa_struct
_tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1E3->name);{void*_tmp1E7[2]={& _tmp1E8,& _tmp1E9};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("type %s has no %s field",sizeof(char),24),_tag_arr(_tmp1E7,sizeof(void*),
2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E3->impl))->exist_vars
!= 0)return({void*_tmp1EA[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(char),64),_tag_arr(_tmp1EA,sizeof(void*),0));});{void*t2;{struct
_RegionHandle _tmp1EB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1EB;
_push_region(rgn);{struct Cyc_List_List*_tmp1EC=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1E3->tvs,_tmp1E4);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp1EC,(void*)_tmp1E6->type);};_pop_region(rgn);}return t2;}}_LL175: if(_tmp1DF <= (
void*)3?1:*((int*)_tmp1DF)!= 11)goto _LL177;_tmp1E5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1DF)->f2;_LL176: {struct Cyc_Absyn_Aggrfield*_tmp1ED=Cyc_Absyn_lookup_field(
_tmp1E5,f);if(_tmp1ED == 0)return({struct Cyc_Std_String_pa_struct _tmp1EF;_tmp1EF.tag=
0;_tmp1EF.f1=(struct _tagged_arr)*f;{void*_tmp1EE[1]={& _tmp1EF};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(char),21),_tag_arr(_tmp1EE,sizeof(
void*),1));}});return(void*)_tmp1ED->type;}_LL177: if(_tmp1DF <= (void*)3?1:*((int*)
_tmp1DF)!= 7)goto _LL179;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(char),5))
== 0))goto _LL179;_LL178: goto _LL17A;_LL179: if(_tmp1DF <= (void*)3?1:*((int*)
_tmp1DF)!= 4)goto _LL17B;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(char),5))
== 0))goto _LL17B;_LL17A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp1F1;_tmp1F1.tag=3;_tmp1F1.f1=(void*)((void*)19);_tmp1F1.f2=({struct Cyc_List_List*
_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=e;_tmp1F2->tl=0;_tmp1F2;});
_tmp1F1;});_tmp1F0;})));return Cyc_Absyn_uint_typ;_LL17B:;_LL17C: if(Cyc_Std_zstrcmp(*
f,_tag_arr("size",sizeof(char),5))== 0)return({struct Cyc_Std_String_pa_struct
_tmp1F4;_tmp1F4.tag=0;_tmp1F4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F3[1]={& _tmp1F4};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct, union, or array, found %s",sizeof(char),44),
_tag_arr(_tmp1F3,sizeof(void*),1));}});else{return({struct Cyc_Std_String_pa_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F5[1]={& _tmp1F6};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct or union, found %s",sizeof(char),36),_tag_arr(
_tmp1F5,sizeof(void*),1));}});}_LL172:;}}static void*Cyc_Tcexp_tcAggrArrow(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp1F7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1F8;void*_tmp1F9;void*_tmp1FA;struct Cyc_Absyn_Conref*_tmp1FB;_LL17E: if(
_tmp1F7 <= (void*)3?1:*((int*)_tmp1F7)!= 4)goto _LL180;_tmp1F8=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F7)->f1;_tmp1F9=(void*)_tmp1F8.elt_typ;_tmp1FA=(void*)_tmp1F8.rgn_typ;
_tmp1FB=_tmp1F8.bounds;_LL17F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FB);{void*
_tmp1FC=Cyc_Tcutil_compress(_tmp1F9);struct Cyc_Absyn_AggrInfo _tmp1FD;void*
_tmp1FE;struct Cyc_Absyn_Aggrdecl**_tmp1FF;struct Cyc_Absyn_Aggrdecl*_tmp200;
struct Cyc_List_List*_tmp201;struct Cyc_List_List*_tmp202;_LL183: if(_tmp1FC <= (
void*)3?1:*((int*)_tmp1FC)!= 10)goto _LL185;_tmp1FD=((struct Cyc_Absyn_AggrType_struct*)
_tmp1FC)->f1;_tmp1FE=(void*)_tmp1FD.aggr_info;if(*((int*)_tmp1FE)!= 1)goto _LL185;
_tmp1FF=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1FE)->f1;_tmp200=*_tmp1FF;
_tmp201=_tmp1FD.targs;_LL184: {struct Cyc_Absyn_Aggrfield*_tmp203=Cyc_Absyn_lookup_decl_field(
_tmp200,f);if(_tmp203 == 0)return({struct Cyc_Std_String_pa_struct _tmp206;_tmp206.tag=
0;_tmp206.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp205;
_tmp205.tag=0;_tmp205.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp200->name);{
void*_tmp204[2]={& _tmp205,& _tmp206};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(char),24),_tag_arr(_tmp204,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp200->impl))->exist_vars != 0)return({void*_tmp207[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(char),64),_tag_arr(_tmp207,sizeof(void*),0));});{void*t3;{struct
_RegionHandle _tmp208=_new_region("rgn");struct _RegionHandle*rgn=& _tmp208;
_push_region(rgn);{struct Cyc_List_List*_tmp209=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp200->tvs,_tmp201);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp209,(void*)_tmp203->type);};_pop_region(rgn);}return t3;}}_LL185: if(_tmp1FC <= (
void*)3?1:*((int*)_tmp1FC)!= 11)goto _LL187;_tmp202=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1FC)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp20A=Cyc_Absyn_lookup_field(
_tmp202,f);if(_tmp20A == 0)return({struct Cyc_Std_String_pa_struct _tmp20C;_tmp20C.tag=
0;_tmp20C.f1=(struct _tagged_arr)*f;{void*_tmp20B[1]={& _tmp20C};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(char),21),_tag_arr(_tmp20B,sizeof(
void*),1));}});return(void*)_tmp20A->type;}_LL187:;_LL188: goto _LL182;_LL182:;}
goto _LL17D;_LL180:;_LL181: goto _LL17D;_LL17D:;}return({struct Cyc_Std_String_pa_struct
_tmp20E;_tmp20E.tag=0;_tmp20E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp20D[1]={& _tmp20E};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(char),35),_tag_arr(
_tmp20D,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(index);struct _handler_cons
_tmp20F;_push_handler(& _tmp20F);{int _tmp211=0;if(setjmp(_tmp20F.handler))_tmp211=
1;if(!_tmp211){{void*_tmp212=(*((struct _tuple8*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return _tmp212;};_pop_handler();}
else{void*_tmp210=(void*)_exn_thrown;void*_tmp214=_tmp210;_LL18A: if(_tmp214 != 
Cyc_List_Nth)goto _LL18C;_LL18B: return({struct Cyc_Std_Int_pa_struct _tmp217;
_tmp217.tag=1;_tmp217.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp216;_tmp216.tag=1;_tmp216.f1=(int)i;{void*
_tmp215[2]={& _tmp216,& _tmp217};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(char),41),_tag_arr(_tmp215,sizeof(void*),2));}}});_LL18C:;_LL18D:(void)
_throw(_tmp214);_LL189:;}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))return({struct Cyc_Std_String_pa_struct _tmp219;_tmp219.tag=0;_tmp219.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp218[1]={& _tmp219};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(char),34),
_tag_arr(_tmp218,sizeof(void*),1));}});{void*_tmp21A=t1;struct Cyc_Absyn_PtrInfo
_tmp21B;void*_tmp21C;void*_tmp21D;struct Cyc_Absyn_Tqual _tmp21E;struct Cyc_Absyn_Conref*
_tmp21F;struct Cyc_List_List*_tmp220;_LL18F: if(_tmp21A <= (void*)3?1:*((int*)
_tmp21A)!= 4)goto _LL191;_tmp21B=((struct Cyc_Absyn_PointerType_struct*)_tmp21A)->f1;
_tmp21C=(void*)_tmp21B.elt_typ;_tmp21D=(void*)_tmp21B.rgn_typ;_tmp21E=_tmp21B.tq;
_tmp21F=_tmp21B.bounds;_LL190: if(Cyc_Tcutil_is_const_exp(te,e2))Cyc_Tcutil_check_bound(
loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp21F);else{if(Cyc_Tcutil_is_bound_one(
_tmp21F))({void*_tmp221[0]={};Cyc_Tcutil_warn(e1->loc,_tag_arr("subscript applied to pointer to one object",
sizeof(char),43),_tag_arr(_tmp221,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp21F);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp21D);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp21C),(void*)1))({void*_tmp222[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(char),36),_tag_arr(_tmp222,
sizeof(void*),0));});return _tmp21C;_LL191: if(_tmp21A <= (void*)3?1:*((int*)
_tmp21A)!= 9)goto _LL193;_tmp220=((struct Cyc_Absyn_TupleType_struct*)_tmp21A)->f1;
_LL192: return Cyc_Tcexp_ithTupleType(te,loc,_tmp220,e2);_LL193:;_LL194: return({
struct Cyc_Std_String_pa_struct _tmp224;_tmp224.tag=0;_tmp224.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp223[1]={& _tmp224};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("subscript applied to %s",sizeof(char),24),_tag_arr(_tmp223,
sizeof(void*),1));}});_LL18E:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;
struct Cyc_List_List*fields=0;if(topt != 0){void*_tmp225=Cyc_Tcutil_compress(*topt);
struct Cyc_List_List*_tmp226;_LL196: if(_tmp225 <= (void*)3?1:*((int*)_tmp225)!= 9)
goto _LL198;_tmp226=((struct Cyc_Absyn_TupleType_struct*)_tmp225)->f1;_LL197: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp226)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL195;for(0;es != 0;(es=es->tl,_tmp226=_tmp226->tl)){
void*_tmp227=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp226))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp227,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228->hd=({
struct _tuple8*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->f1=(*((struct _tuple8*)
_tmp226->hd)).f1;_tmp229->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp229;});_tmp228->tl=fields;_tmp228;});}done=
1;goto _LL195;_LL198:;_LL199: goto _LL195;_LL195:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A->hd=({struct _tuple8*_tmp22B=
_cycalloc(sizeof(*_tmp22B));_tmp22B->f1=Cyc_Absyn_empty_tqual();_tmp22B->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp22B;});_tmp22A->tl=fields;_tmp22A;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C[0]=({struct Cyc_Absyn_TupleType_struct
_tmp22D;_tmp22D.tag=9;_tmp22D.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp22D;});_tmp22C;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp22E[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(char),14),_tag_arr(_tmp22E,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp23F=_cycalloc(
sizeof(*_tmp23F));_tmp23F->v=Cyc_Tcenv_lookup_type_vars(te);_tmp23F;}));struct
Cyc_Absyn_Const_e_struct*_tmp22F=({struct Cyc_Absyn_Const_e_struct*_tmp23B=
_cycalloc(sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_Const_e_struct _tmp23C;
_tmp23C.tag=0;_tmp23C.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp23D=
_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Absyn_Int_c_struct _tmp23E;
_tmp23E.tag=2;_tmp23E.f1=(void*)((void*)1);_tmp23E.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp23E;});_tmp23D;}));_tmp23C;});_tmp23B;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp22F,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->v=(void*)Cyc_Absyn_uint_typ;_tmp230;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp239=_cycalloc(sizeof(*
_tmp239));_tmp239[0]=({struct Cyc_Absyn_ArrayType_struct _tmp23A;_tmp23A.tag=7;
_tmp23A.f1=(void*)res;_tmp23A.f2=Cyc_Absyn_empty_tqual();_tmp23A.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp23A;});_tmp239;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_Std_String_pa_struct _tmp232;_tmp232.tag=0;_tmp232.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp231[1]={& _tmp232};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(char),53),_tag_arr(_tmp231,sizeof(void*),1));}});{int offset=0;for(0;des != 
0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)des->hd)).f1;
if(ds != 0){void*_tmp233=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp234;_LL19B: if(*((
int*)_tmp233)!= 1)goto _LL19D;_LL19C:({void*_tmp235[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("only array index designators are supported",sizeof(char),43),_tag_arr(
_tmp235,sizeof(void*),0));});goto _LL19A;_LL19D: if(*((int*)_tmp233)!= 0)goto
_LL19A;_tmp234=((struct Cyc_Absyn_ArrayElement_struct*)_tmp233)->f1;_LL19E: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp234);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp234);if(i != 
offset)({struct Cyc_Std_Int_pa_struct _tmp238;_tmp238.tag=1;_tmp238.f1=(int)i;{
struct Cyc_Std_Int_pa_struct _tmp237;_tmp237.tag=1;_tmp237.f1=(int)((unsigned int)
offset);{void*_tmp236[2]={& _tmp237,& _tmp238};Cyc_Tcutil_terr(_tmp234->loc,
_tag_arr("expecting index designator %d but found %d",sizeof(char),43),_tag_arr(
_tmp236,sizeof(void*),2));}}});goto _LL19A;}_LL19A:;}}}return res_t2;}}static void*
Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(te,bound))({
struct Cyc_Std_String_pa_struct _tmp241;_tmp241.tag=0;_tmp241.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
bound->topt))->v);{void*_tmp240[1]={& _tmp241};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(char),33),_tag_arr(_tmp240,
sizeof(void*),1));}});if(!(vd->tq).q_const)({void*_tmp242[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(char),52),_tag_arr(_tmp242,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(
loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;
void**_tmp243=0;struct Cyc_Absyn_Tqual*_tmp244=0;if(topt != 0){void*_tmp245=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp246;void*_tmp247;void**_tmp248;struct Cyc_Absyn_Tqual
_tmp249;struct Cyc_Absyn_Tqual*_tmp24A;struct Cyc_Absyn_Exp*_tmp24B;_LL1A0: if(
_tmp245 <= (void*)3?1:*((int*)_tmp245)!= 4)goto _LL1A2;_tmp246=((struct Cyc_Absyn_PointerType_struct*)
_tmp245)->f1;_LL1A1: pinfo=_tmp246;_tmp243=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp244=(struct Cyc_Absyn_Tqual*)& pinfo.tq;goto _LL19F;_LL1A2: if(_tmp245 <= (void*)
3?1:*((int*)_tmp245)!= 7)goto _LL1A4;_tmp247=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp245)->f1;_tmp248=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp245)->f1);_tmp249=((struct Cyc_Absyn_ArrayType_struct*)_tmp245)->f2;_tmp24A=(
struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_struct*)_tmp245)->f2;_tmp24B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp245)->f3;_LL1A3: _tmp243=(void**)_tmp248;
_tmp244=(struct Cyc_Absyn_Tqual*)_tmp24A;goto _LL19F;_LL1A4:;_LL1A5: goto _LL19F;
_LL19F:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp243,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp24C[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(char),22),_tag_arr(_tmp24C,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(
te,body))({void*_tmp24D[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(char),21),_tag_arr(_tmp24D,sizeof(void*),0));});}return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp24F;_tmp24F.tag=7;_tmp24F.f1=(void*)t;_tmp24F.f2=_tmp244 == 0?Cyc_Absyn_empty_tqual():*
_tmp244;_tmp24F.f3=(struct Cyc_Absyn_Exp*)bound;_tmp24F;});_tmp24E;});}}}struct
_tuple9{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp250;_push_handler(& _tmp250);{
int _tmp252=0;if(setjmp(_tmp250.handler))_tmp252=1;if(!_tmp252){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp251=(void*)_exn_thrown;void*_tmp254=
_tmp251;_LL1A7: if(_tmp254 != Cyc_Dict_Absent)goto _LL1A9;_LL1A8:({struct Cyc_Std_String_pa_struct
_tmp256;_tmp256.tag=0;_tmp256.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp255[1]={& _tmp256};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(char),23),_tag_arr(_tmp255,sizeof(void*),1));}});return topt != 0?*topt:(
void*)0;_LL1A9:;_LL1AA:(void)_throw(_tmp254);_LL1A6:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp257[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("expecting struct but found union",sizeof(char),33),_tag_arr(
_tmp257,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp258[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't build abstract struct",sizeof(char),28),_tag_arr(_tmp258,
sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp259=
_cycalloc(sizeof(*_tmp259));_tmp259->v=Cyc_Tcenv_lookup_type_vars(te);_tmp259;}));}{
struct _RegionHandle _tmp25A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp25A;
_push_region(rgn);{struct _tuple4 _tmp25B=({struct _tuple4 _tmp27A;_tmp27A.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp27A.f2=rgn;_tmp27A;});struct Cyc_List_List*_tmp25C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp25B,ad->tvs);struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp25B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp25E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp25C);struct Cyc_List_List*_tmp25F=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp25D);struct Cyc_List_List*_tmp260=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp25C,_tmp25D);struct Cyc_Absyn_AggrType_struct*_tmp261=({struct Cyc_Absyn_AggrType_struct*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_Absyn_AggrType_struct
_tmp275;_tmp275.tag=10;_tmp275.f1=({struct Cyc_Absyn_AggrInfo _tmp276;_tmp276.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp277=_cycalloc(sizeof(*
_tmp277));_tmp277[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp278;_tmp278.tag=1;
_tmp278.f1=({struct Cyc_Absyn_Aggrdecl**_tmp279=_cycalloc(sizeof(*_tmp279));
_tmp279[0]=ad;_tmp279;});_tmp278;});_tmp277;}));_tmp276.targs=_tmp25E;_tmp276;});
_tmp275;});_tmp274;});struct Cyc_List_List*_tmp262=*ts;struct Cyc_List_List*
_tmp263=_tmp25F;while(_tmp262 != 0?_tmp263 != 0: 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp262->hd);Cyc_Tcutil_unify((
void*)_tmp262->hd,(void*)_tmp263->hd);_tmp262=_tmp262->tl;_tmp263=_tmp263->tl;}
if(_tmp262 != 0)({void*_tmp264[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many explicit witness types",
sizeof(char),32),_tag_arr(_tmp264,sizeof(void*),0));});*ts=_tmp25F;if(topt != 0)
Cyc_Tcutil_unify((void*)_tmp261,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple9 _tmp266;struct Cyc_Absyn_Aggrfield*_tmp267;struct
Cyc_Absyn_Exp*_tmp268;struct _tuple9*_tmp265=(struct _tuple9*)fields->hd;_tmp266=*
_tmp265;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{void*_tmp269=Cyc_Tcutil_rsubstitute(
rgn,_tmp260,(void*)_tmp267->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp269,
_tmp268);if(!Cyc_Tcutil_coerce_arg(te,_tmp268,_tmp269)){({struct Cyc_Std_String_pa_struct
_tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp268->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp26D;_tmp26D.tag=0;_tmp26D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp269);{struct Cyc_Std_String_pa_struct _tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(*tn);{struct Cyc_Std_String_pa_struct _tmp26B;
_tmp26B.tag=0;_tmp26B.f1=(struct _tagged_arr)*_tmp267->name;{void*_tmp26A[4]={&
_tmp26B,& _tmp26C,& _tmp26D,& _tmp26E};Cyc_Tcutil_terr(_tmp268->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(char),44),_tag_arr(_tmp26A,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp26F=0;{struct Cyc_List_List*_tmp270=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp270 != 0;_tmp270=_tmp270->tl){_tmp26F=({
struct Cyc_List_List*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->hd=({struct
_tuple0*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp260,(*((struct _tuple0*)_tmp270->hd)).f1);_tmp272->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp260,(*((struct _tuple0*)_tmp270->hd)).f2);_tmp272;});_tmp271->tl=_tmp26F;
_tmp271;});}}_tmp26F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp26F);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp26F);{void*_tmp273=(void*)
_tmp261;_npop_handler(0);return _tmp273;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp27B=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp27B;_push_region(rgn);{void*_tmp27C=Cyc_Tcutil_compress(ts);void*_tmp27D;
struct Cyc_List_List*_tmp27E;_LL1AC: if(_tmp27C <= (void*)3?1:*((int*)_tmp27C)!= 11)
goto _LL1AE;_tmp27D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp27C)->f1;
_tmp27E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp27C)->f2;_LL1AD: if(_tmp27D == (
void*)1)({void*_tmp27F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expecting struct but found union",
sizeof(char),33),_tag_arr(_tmp27F,sizeof(void*),0));});{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp27E);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp281;
struct Cyc_Absyn_Aggrfield*_tmp282;struct Cyc_Absyn_Exp*_tmp283;struct _tuple9*
_tmp280=(struct _tuple9*)fields->hd;_tmp281=*_tmp280;_tmp282=_tmp281.f1;_tmp283=
_tmp281.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp282->type)),
_tmp283);if(!Cyc_Tcutil_coerce_arg(te,_tmp283,(void*)_tmp282->type)){({struct Cyc_Std_String_pa_struct
_tmp287;_tmp287.tag=0;_tmp287.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp283->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp286;_tmp286.tag=0;_tmp286.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp282->type);{struct Cyc_Std_String_pa_struct _tmp285;_tmp285.tag=0;_tmp285.f1=(
struct _tagged_arr)*_tmp282->name;{void*_tmp284[3]={& _tmp285,& _tmp286,& _tmp287};
Cyc_Tcutil_terr(_tmp283->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(char),41),_tag_arr(_tmp284,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL1AB;}_LL1AE:;_LL1AF:({void*_tmp288[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct: wrong type",
sizeof(char),25),_tag_arr(_tmp288,sizeof(void*),0));});_LL1AB:;};_pop_region(rgn);}
return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp289=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp289;_push_region(rgn);{struct _tuple4 _tmp28A=({
struct _tuple4 _tmp2A9;_tmp2A9.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp2A9.f2=rgn;
_tmp2A9;});struct Cyc_List_List*_tmp28B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp28A,tud->tvs);struct Cyc_List_List*_tmp28C=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp28B);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp2A5;_tmp2A5.tag=3;_tmp2A5.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp2A6;
_tmp2A6.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp2A8;_tmp2A8.tag=1;_tmp2A8.f1=tud;_tmp2A8.f2=tuf;_tmp2A8;});_tmp2A7;}));
_tmp2A6.targs=_tmp28C;_tmp2A6;});_tmp2A5;});_tmp2A4;});if(topt != 0){void*_tmp28D=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp28E;struct Cyc_List_List*
_tmp28F;void*_tmp290;_LL1B1: if(_tmp28D <= (void*)3?1:*((int*)_tmp28D)!= 3)goto
_LL1B3;_LL1B2: Cyc_Tcutil_unify(*topt,res);goto _LL1B0;_LL1B3: if(_tmp28D <= (void*)
3?1:*((int*)_tmp28D)!= 2)goto _LL1B5;_tmp28E=((struct Cyc_Absyn_TunionType_struct*)
_tmp28D)->f1;_tmp28F=_tmp28E.targs;_tmp290=(void*)_tmp28E.rgn;_LL1B4:{struct Cyc_List_List*
a=_tmp28C;for(0;a != 0?_tmp28F != 0: 0;(a=a->tl,_tmp28F=_tmp28F->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp28F->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->v=(void*)res;_tmp291;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292[0]=({
struct Cyc_Absyn_TunionType_struct _tmp293;_tmp293.tag=2;_tmp293.f1=({struct Cyc_Absyn_TunionInfo
_tmp294;_tmp294.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp296;_tmp296.tag=1;_tmp296.f1=({struct Cyc_Absyn_Tuniondecl**_tmp297=_cycalloc(
sizeof(*_tmp297));_tmp297[0]=tud;_tmp297;});_tmp296;});_tmp295;}));_tmp294.targs=
_tmp28C;_tmp294.rgn=(void*)_tmp290;_tmp294;});_tmp293;});_tmp292;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp298=res;_npop_handler(0);return _tmp298;}}goto _LL1B0;_LL1B5:;
_LL1B6: goto _LL1B0;_LL1B0:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp28B,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmp29C;_tmp29C.tag=0;_tmp29C.f1=(struct _tagged_arr)(e->topt == 0?_tag_arr("?",
sizeof(char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp29B;_tmp29B.tag=0;_tmp29B.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp29A;_tmp29A.tag=0;_tmp29A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp299[3]={& _tmp29A,& _tmp29B,& _tmp29C};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(char),80),_tag_arr(_tmp299,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp29F=({struct Cyc_Std_String_pa_struct _tmp29E;_tmp29E.tag=0;
_tmp29E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp29D[1]={&
_tmp29E};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(char),45),_tag_arr(_tmp29D,sizeof(void*),1));}});_npop_handler(0);return
_tmp29F;}if(ts != 0){void*_tmp2A2=({struct Cyc_Std_String_pa_struct _tmp2A1;_tmp2A1.tag=
0;_tmp2A1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp2A0[
1]={& _tmp2A1};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(char),44),_tag_arr(_tmp2A0,sizeof(void*),1));}});_npop_handler(0);return
_tmp2A2;}{void*_tmp2A3=res;_npop_handler(0);return _tmp2A3;}}};_pop_region(rgn);}
static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp2AA=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp2AB;struct Cyc_Absyn_Conref*_tmp2AC;void*_tmp2AD;
struct Cyc_List_List*_tmp2AE;struct Cyc_Absyn_AggrInfo _tmp2AF;void*_tmp2B0;struct
Cyc_List_List*_tmp2B1;struct Cyc_List_List*_tmp2B2;_LL1B8: if((int)_tmp2AA != 0)
goto _LL1BA;_LL1B9: return 1;_LL1BA: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 0)
goto _LL1BC;_LL1BB: goto _LL1BD;_LL1BC: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 1)
goto _LL1BE;_LL1BD: goto _LL1BF;_LL1BE: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 2)
goto _LL1C0;_LL1BF: goto _LL1C1;_LL1C0: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 3)
goto _LL1C2;_LL1C1: return 0;_LL1C2: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 4)
goto _LL1C4;_tmp2AB=((struct Cyc_Absyn_PointerType_struct*)_tmp2AA)->f1;_tmp2AC=
_tmp2AB.nullable;_LL1C3: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp2AC);_LL1C4: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 5)goto _LL1C6;_LL1C5:
goto _LL1C7;_LL1C6: if((int)_tmp2AA != 1)goto _LL1C8;_LL1C7: goto _LL1C9;_LL1C8: if(
_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 6)goto _LL1CA;_LL1C9: return 1;_LL1CA: if(
_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 7)goto _LL1CC;_tmp2AD=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp2AA)->f1;_LL1CB: return Cyc_Tcexp_zeroable_type(_tmp2AD);_LL1CC: if(_tmp2AA <= (
void*)3?1:*((int*)_tmp2AA)!= 8)goto _LL1CE;_LL1CD: return 0;_LL1CE: if(_tmp2AA <= (
void*)3?1:*((int*)_tmp2AA)!= 9)goto _LL1D0;_tmp2AE=((struct Cyc_Absyn_TupleType_struct*)
_tmp2AA)->f1;_LL1CF: for(0;(unsigned int)_tmp2AE;_tmp2AE=_tmp2AE->tl){if(!Cyc_Tcexp_zeroable_type((*((
struct _tuple8*)_tmp2AE->hd)).f2))return 0;}return 1;_LL1D0: if(_tmp2AA <= (void*)3?1:*((
int*)_tmp2AA)!= 10)goto _LL1D2;_tmp2AF=((struct Cyc_Absyn_AggrType_struct*)_tmp2AA)->f1;
_tmp2B0=(void*)_tmp2AF.aggr_info;_tmp2B1=_tmp2AF.targs;_LL1D1: {struct Cyc_Absyn_Aggrdecl*
_tmp2B3=Cyc_Absyn_get_known_aggrdecl(_tmp2B0);if(_tmp2B3->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2B3->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp2B4=
_new_region("r");struct _RegionHandle*r=& _tmp2B4;_push_region(r);{struct Cyc_List_List*
_tmp2B5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp2B3->tvs,
_tmp2B1);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp2B3->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(
r,_tmp2B5,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp2B6=0;
_npop_handler(0);return _tmp2B6;}}}{int _tmp2B7=1;_npop_handler(0);return _tmp2B7;}};
_pop_region(r);}}_LL1D2: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 12)goto _LL1D4;
_LL1D3: return 1;_LL1D4: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 14)goto _LL1D6;
_LL1D5: return 1;_LL1D6: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 17)goto _LL1D8;
_LL1D7: return 1;_LL1D8: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 11)goto _LL1DA;
_tmp2B2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2AA)->f2;_LL1D9: for(0;_tmp2B2
!= 0;_tmp2B2=_tmp2B2->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp2B2->hd)->type))return 0;}return 1;_LL1DA: if(_tmp2AA <= (void*)3?1:*((int*)
_tmp2AA)!= 13)goto _LL1DC;_LL1DB: return 1;_LL1DC: if(_tmp2AA <= (void*)3?1:*((int*)
_tmp2AA)!= 16)goto _LL1DE;_LL1DD: return 0;_LL1DE: if(_tmp2AA <= (void*)3?1:*((int*)
_tmp2AA)!= 15)goto _LL1E0;_LL1DF: return 0;_LL1E0: if(_tmp2AA <= (void*)3?1:*((int*)
_tmp2AA)!= 18)goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2: if((int)_tmp2AA != 2)goto _LL1E4;
_LL1E3: goto _LL1E5;_LL1E4: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 19)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 20)
goto _LL1E8;_LL1E7: goto _LL1E9;_LL1E8: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 
21)goto _LL1B7;_LL1E9:({struct Cyc_Std_String_pa_struct _tmp2B9;_tmp2B9.tag=0;
_tmp2B9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp2B8[1]={&
_tmp2B9};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad type `%s' in zeroable type",sizeof(char),31),_tag_arr(_tmp2B8,
sizeof(void*),1));}});_LL1B7:;}static void Cyc_Tcexp_check_malloc_type(int
allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;if(topt != 0){void*_tmp2BA=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2BB;void*_tmp2BC;_LL1EB:
if(_tmp2BA <= (void*)3?1:*((int*)_tmp2BA)!= 4)goto _LL1ED;_tmp2BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BA)->f1;_tmp2BC=(void*)_tmp2BB.elt_typ;_LL1EC: Cyc_Tcutil_unify(_tmp2BC,t);
if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;
goto _LL1EA;_LL1ED:;_LL1EE: goto _LL1EA;_LL1EA:;}({struct Cyc_Std_String_pa_struct
_tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2BE;_tmp2BE.tag=0;_tmp2BE.f1=(struct
_tagged_arr)(allow_zero?_tag_arr("calloc",sizeof(char),7): _tag_arr("malloc",
sizeof(char),7));{void*_tmp2BD[2]={& _tmp2BE,& _tmp2BF};Cyc_Tcutil_terr(loc,
_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",sizeof(
char),60),_tag_arr(_tmp2BD,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2C5;_tmp2C5.tag=15;_tmp2C5.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2C6;}));_tmp2C5;});_tmp2C4;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2C0=Cyc_Tcutil_compress(handle_type);void*_tmp2C1;
_LL1F0: if(_tmp2C0 <= (void*)3?1:*((int*)_tmp2C0)!= 15)goto _LL1F2;_tmp2C1=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2C0)->f1;_LL1F1: rgn=_tmp2C1;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1EF;_LL1F2:;_LL1F3:({struct Cyc_Std_String_pa_struct _tmp2C3;
_tmp2C3.tag=0;_tmp2C3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{
void*_tmp2C2[1]={& _tmp2C3};Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(char),37),_tag_arr(_tmp2C2,sizeof(void*),1));}});goto _LL1EF;_LL1EF:;}Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;
int one_elt;if(is_calloc){if(*t == 0)({void*_tmp2C7[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(char),23),_tag_arr(_tmp2C7,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp2C8=(void*)(*e)->r;void*_tmp2C9;void*_tmp2CA;struct Cyc_List_List*
_tmp2CB;struct Cyc_List_List _tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_List_List*
_tmp2CE;struct Cyc_List_List _tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_List_List*
_tmp2D1;_LL1F5: if(*((int*)_tmp2C8)!= 16)goto _LL1F7;_tmp2C9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2C8)->f1;_LL1F6: elt_type=_tmp2C9;*t=(void**)({void**_tmp2D2=_cycalloc(
sizeof(*_tmp2D2));_tmp2D2[0]=elt_type;_tmp2D2;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL1F4;
_LL1F7: if(*((int*)_tmp2C8)!= 3)goto _LL1F9;_tmp2CA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2C8)->f1;if((int)_tmp2CA != 1)goto _LL1F9;_tmp2CB=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2C8)->f2;if(_tmp2CB == 0)goto _LL1F9;_tmp2CC=*_tmp2CB;_tmp2CD=(struct Cyc_Absyn_Exp*)
_tmp2CC.hd;_tmp2CE=_tmp2CC.tl;if(_tmp2CE == 0)goto _LL1F9;_tmp2CF=*_tmp2CE;_tmp2D0=(
struct Cyc_Absyn_Exp*)_tmp2CF.hd;_tmp2D1=_tmp2CF.tl;if(_tmp2D1 != 0)goto _LL1F9;
_LL1F8:{struct _tuple0 _tmp2D4=({struct _tuple0 _tmp2D3;_tmp2D3.f1=(void*)_tmp2CD->r;
_tmp2D3.f2=(void*)_tmp2D0->r;_tmp2D3;});void*_tmp2D5;void*_tmp2D6;void*_tmp2D7;
void*_tmp2D8;_LL1FC: _tmp2D5=_tmp2D4.f1;if(*((int*)_tmp2D5)!= 16)goto _LL1FE;
_tmp2D6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2D5)->f1;_LL1FD: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2D6);elt_type=_tmp2D6;*t=(void**)({void**_tmp2D9=_cycalloc(
sizeof(*_tmp2D9));_tmp2D9[0]=elt_type;_tmp2D9;});num_elts=_tmp2D0;one_elt=0;goto
_LL1FB;_LL1FE: _tmp2D7=_tmp2D4.f2;if(*((int*)_tmp2D7)!= 16)goto _LL200;_tmp2D8=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2D7)->f1;_LL1FF: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2D8);elt_type=_tmp2D8;*t=(void**)({void**_tmp2DA=_cycalloc(
sizeof(*_tmp2DA));_tmp2DA[0]=elt_type;_tmp2DA;});num_elts=_tmp2CD;one_elt=0;goto
_LL1FB;_LL200:;_LL201: goto No_sizeof;_LL1FB:;}goto _LL1F4;_LL1F9:;_LL1FA: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp2DB=_cycalloc(sizeof(*_tmp2DB));
_tmp2DB[0]=elt_type;_tmp2DB;});num_elts=*e;one_elt=0;goto _LL1F4;_LL1F4:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp2DC=elt_type;struct Cyc_Absyn_AggrInfo _tmp2DD;
void*_tmp2DE;struct Cyc_Absyn_Aggrdecl**_tmp2DF;struct Cyc_Absyn_Aggrdecl*_tmp2E0;
_LL203: if(_tmp2DC <= (void*)3?1:*((int*)_tmp2DC)!= 10)goto _LL205;_tmp2DD=((struct
Cyc_Absyn_AggrType_struct*)_tmp2DC)->f1;_tmp2DE=(void*)_tmp2DD.aggr_info;if(*((
int*)_tmp2DE)!= 1)goto _LL205;_tmp2DF=((struct Cyc_Absyn_KnownAggr_struct*)_tmp2DE)->f1;
_tmp2E0=*_tmp2DF;_LL204: if(_tmp2E0->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2E0->impl))->exist_vars != 0: 0)({void*_tmp2E1[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("malloc with existential types not yet implemented",sizeof(char),50),
_tag_arr(_tmp2E1,sizeof(void*),0));});goto _LL202;_LL205:;_LL206: goto _LL202;
_LL202:;}{void*(*_tmp2E2)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;
if(topt != 0){void*_tmp2E3=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2E4;struct Cyc_Absyn_Conref*_tmp2E5;_LL208: if(_tmp2E3 <= (void*)3?1:*((int*)
_tmp2E3)!= 4)goto _LL20A;_tmp2E4=((struct Cyc_Absyn_PointerType_struct*)_tmp2E3)->f1;
_tmp2E5=_tmp2E4.nullable;_LL209: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp2E5))_tmp2E2=Cyc_Absyn_star_typ;goto _LL207;_LL20A:;_LL20B: goto _LL207;_LL207:;}
if(!one_elt)_tmp2E2=Cyc_Absyn_tagged_typ;return _tmp2E2(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp2E6=(void*)s->r;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_Absyn_Stmt*
_tmp2E8;struct Cyc_Absyn_Stmt*_tmp2E9;struct Cyc_Absyn_Decl*_tmp2EA;struct Cyc_Absyn_Stmt*
_tmp2EB;_LL20D: if(_tmp2E6 <= (void*)1?1:*((int*)_tmp2E6)!= 0)goto _LL20F;_tmp2E7=((
struct Cyc_Absyn_Exp_s_struct*)_tmp2E6)->f1;_LL20E: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E7->topt))->v;_LL20F: if(_tmp2E6 <= (void*)1?1:*((int*)_tmp2E6)!= 
1)goto _LL211;_tmp2E8=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E6)->f1;_tmp2E9=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2E6)->f2;_LL210: s=_tmp2E9;continue;_LL211: if(
_tmp2E6 <= (void*)1?1:*((int*)_tmp2E6)!= 12)goto _LL213;_tmp2EA=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2E6)->f1;_tmp2EB=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E6)->f2;_LL212: s=
_tmp2EB;continue;_LL213:;_LL214: return({void*_tmp2EC[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("statement expression must end with expression",sizeof(char),46),
_tag_arr(_tmp2EC,sizeof(void*),0));});_LL20C:;}}static void*Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*
fd){return({void*_tmp2ED[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcCodegen",
sizeof(char),10),_tag_arr(_tmp2ED,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){return({void*_tmp2EE[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcFill",
sizeof(char),7),_tag_arr(_tmp2EE,sizeof(void*),0));});}static void*Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(
rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2F4;_tmp2F4.tag=15;_tmp2F4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2F5;}));_tmp2F4;});_tmp2F3;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp2EF=Cyc_Tcutil_compress(handle_type);void*
_tmp2F0;_LL216: if(_tmp2EF <= (void*)3?1:*((int*)_tmp2EF)!= 15)goto _LL218;_tmp2F0=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2EF)->f1;_LL217: rgn=_tmp2F0;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL215;_LL218:;_LL219:({struct Cyc_Std_String_pa_struct
_tmp2F2;_tmp2F2.tag=0;_tmp2F2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type);{void*_tmp2F1[1]={& _tmp2F2};Cyc_Tcutil_terr(rgn_handle->loc,
_tag_arr("expecting region_t type but found %s",sizeof(char),37),_tag_arr(
_tmp2F1,sizeof(void*),1));}});goto _LL215;_LL215:;}{void*_tmp2F6=(void*)e1->r;
struct Cyc_Absyn_Vardecl*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_Absyn_Exp*
_tmp2F9;struct Cyc_Core_Opt*_tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_List_List*
_tmp2FC;void*_tmp2FD;struct _tagged_arr _tmp2FE;_LL21B: if(*((int*)_tmp2F6)!= 27)
goto _LL21D;_tmp2F7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F6)->f1;
_tmp2F8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F6)->f2;_tmp2F9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2F6)->f3;_LL21C: {void*_tmp2FF=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp300=Cyc_Tcutil_compress(_tmp2FF);void*_tmp301;struct Cyc_Absyn_Tqual
_tmp302;_LL226: if(_tmp300 <= (void*)3?1:*((int*)_tmp300)!= 7)goto _LL228;_tmp301=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp300)->f1;_tmp302=((struct Cyc_Absyn_ArrayType_struct*)
_tmp300)->f2;_LL227: {void*b=Cyc_Tcutil_is_const_exp(te,_tmp2F8)?(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp308;_tmp308.tag=0;_tmp308.f1=_tmp2F8;_tmp308;});
_tmp307;}):(void*)0;void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304[0]=({struct Cyc_Absyn_PointerType_struct
_tmp305;_tmp305.tag=4;_tmp305.f1=({struct Cyc_Absyn_PtrInfo _tmp306;_tmp306.elt_typ=(
void*)_tmp301;_tmp306.rgn_typ=(void*)rgn;_tmp306.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp306.tq=_tmp302;_tmp306.bounds=Cyc_Absyn_new_conref(
b);_tmp306;});_tmp305;});_tmp304;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,
res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->v=(void*)res_typ;_tmp303;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}_LL228:;_LL229:({void*_tmp309[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcNew: comprehension returned non-array type",
sizeof(char),45),_tag_arr(_tmp309,sizeof(void*),0));});_LL225:;}_LL21D: if(*((int*)
_tmp2F6)!= 34)goto _LL21F;_tmp2FA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp2F6)->f1;_tmp2FB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2F6)->f2;
_LL21E:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp30A=
_cycalloc(sizeof(*_tmp30A));_tmp30A[0]=({struct Cyc_Absyn_Array_e_struct _tmp30B;
_tmp30B.tag=26;_tmp30B.f1=_tmp2FB;_tmp30B;});_tmp30A;})));_tmp2FC=_tmp2FB;goto
_LL220;_LL21F: if(*((int*)_tmp2F6)!= 26)goto _LL221;_tmp2FC=((struct Cyc_Absyn_Array_e_struct*)
_tmp2F6)->f1;_LL220: {void**elt_typ_opt=0;if(topt != 0){void*_tmp30C=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp30D;void*_tmp30E;void**_tmp30F;struct Cyc_Absyn_Tqual
_tmp310;_LL22B: if(_tmp30C <= (void*)3?1:*((int*)_tmp30C)!= 4)goto _LL22D;_tmp30D=((
struct Cyc_Absyn_PointerType_struct*)_tmp30C)->f1;_tmp30E=(void*)_tmp30D.elt_typ;
_tmp30F=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp30C)->f1).elt_typ;
_tmp310=_tmp30D.tq;_LL22C: elt_typ_opt=(void**)_tmp30F;goto _LL22A;_LL22D:;_LL22E:
goto _LL22A;_LL22A:;}{void*_tmp311=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);
void*res_typ;{void*_tmp312=Cyc_Tcutil_compress(_tmp311);void*_tmp313;struct Cyc_Absyn_Tqual
_tmp314;struct Cyc_Absyn_Exp*_tmp315;_LL230: if(_tmp312 <= (void*)3?1:*((int*)
_tmp312)!= 7)goto _LL232;_tmp313=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp312)->f1;_tmp314=((struct Cyc_Absyn_ArrayType_struct*)_tmp312)->f2;_tmp315=((
struct Cyc_Absyn_ArrayType_struct*)_tmp312)->f3;_LL231: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=({struct Cyc_Absyn_PointerType_struct
_tmp317;_tmp317.tag=4;_tmp317.f1=({struct Cyc_Absyn_PtrInfo _tmp318;_tmp318.elt_typ=(
void*)_tmp313;_tmp318.rgn_typ=(void*)rgn;_tmp318.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp318.tq=_tmp314;_tmp318.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp319=_cycalloc(sizeof(*_tmp319));
_tmp319[0]=({struct Cyc_Absyn_Upper_b_struct _tmp31A;_tmp31A.tag=0;_tmp31A.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp315);_tmp31A;});_tmp319;}));_tmp318;});
_tmp317;});_tmp316;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp31B=_cycalloc(sizeof(*
_tmp31B));_tmp31B->v=(void*)res_typ;_tmp31B;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL22F;_LL232:;_LL233:({void*_tmp31C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(char),50),_tag_arr(_tmp31C,sizeof(void*),0));});_LL22F:;}return res_typ;}}
_LL221: if(*((int*)_tmp2F6)!= 0)goto _LL223;_tmp2FD=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2F6)->f1;if(_tmp2FD <= (void*)1?1:*((int*)_tmp2FD)!= 5)goto _LL223;_tmp2FE=((
struct Cyc_Absyn_String_c_struct*)_tmp2FD)->f1;_LL222: {void*_tmp31D=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(void*)0);void*_tmp31E=Cyc_Tcexp_tcExp(
te,(void**)& _tmp31D,e1);return Cyc_Absyn_atb_typ(_tmp31E,rgn,Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));
_tmp31F[0]=({struct Cyc_Absyn_Upper_b_struct _tmp320;_tmp320.tag=0;_tmp320.f1=Cyc_Absyn_uint_exp(
1,0);_tmp320;});_tmp31F;}));}_LL223:;_LL224: {void**topt2=0;if(topt != 0){void*
_tmp321=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp322;void*_tmp323;
void**_tmp324;struct Cyc_Absyn_Conref*_tmp325;_LL235: if(_tmp321 <= (void*)3?1:*((
int*)_tmp321)!= 4)goto _LL237;_tmp322=((struct Cyc_Absyn_PointerType_struct*)
_tmp321)->f1;_tmp323=(void*)_tmp322.elt_typ;_tmp324=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp321)->f1).elt_typ;_tmp325=_tmp322.nullable;_LL236: topt2=(void**)_tmp324;goto
_LL234;_LL237: if(_tmp321 <= (void*)3?1:*((int*)_tmp321)!= 2)goto _LL239;_LL238:
topt2=({void**_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326[0]=*topt;_tmp326;});
goto _LL234;_LL239:;_LL23A: goto _LL234;_LL234:;}{void*telt=Cyc_Tcexp_tcExp(te,
topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp328=
_cycalloc(sizeof(*_tmp328));_tmp328[0]=({struct Cyc_Absyn_PointerType_struct
_tmp329;_tmp329.tag=4;_tmp329.f1=({struct Cyc_Absyn_PtrInfo _tmp32A;_tmp32A.elt_typ=(
void*)telt;_tmp32A.rgn_typ=(void*)rgn;_tmp32A.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp32A.tq=Cyc_Absyn_empty_tqual();_tmp32A.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));
_tmp32B[0]=({struct Cyc_Absyn_Upper_b_struct _tmp32C;_tmp32C.tag=0;_tmp32C.f1=Cyc_Absyn_uint_exp(
1,0);_tmp32C;});_tmp32B;}));_tmp32A;});_tmp329;});_tmp328;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->v=(
void*)res_typ;_tmp327;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL21A:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp32D=t;void*_tmp32E;struct Cyc_Absyn_Tqual _tmp32F;struct Cyc_Absyn_Exp*
_tmp330;_LL23C: if(_tmp32D <= (void*)3?1:*((int*)_tmp32D)!= 7)goto _LL23E;_tmp32E=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp32D)->f1;_tmp32F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp32D)->f2;_tmp330=((struct Cyc_Absyn_ArrayType_struct*)_tmp32D)->f3;_LL23D: {
void*_tmp332;struct _tuple6 _tmp331=Cyc_Tcutil_addressof_props(te,e);_tmp332=
_tmp331.f2;{void*_tmp333=_tmp330 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp335;_tmp335.tag=0;_tmp335.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp330);
_tmp335;});_tmp334;});t=Cyc_Absyn_atb_typ(_tmp32E,_tmp332,_tmp32F,_tmp333);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL23E:;_LL23F:
return t;_LL23B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*
_tmp336=(void*)e->r;void*_tmp337;_LL241: if(*((int*)_tmp336)!= 26)goto _LL243;
_LL242: goto _LL244;_LL243: if(*((int*)_tmp336)!= 27)goto _LL245;_LL244: goto _LL246;
_LL245: if(*((int*)_tmp336)!= 0)goto _LL247;_tmp337=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp336)->f1;if(_tmp337 <= (void*)1?1:*((int*)_tmp337)!= 5)goto _LL247;_LL246:
return t;_LL247:;_LL248: t=Cyc_Tcutil_compress(t);{void*_tmp338=t;void*_tmp339;
struct Cyc_Absyn_Tqual _tmp33A;struct Cyc_Absyn_Exp*_tmp33B;_LL24A: if(_tmp338 <= (
void*)3?1:*((int*)_tmp338)!= 7)goto _LL24C;_tmp339=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp338)->f1;_tmp33A=((struct Cyc_Absyn_ArrayType_struct*)_tmp338)->f2;_tmp33B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp338)->f3;_LL24B: {void*_tmp33D;struct
_tuple6 _tmp33C=Cyc_Tcutil_addressof_props(te,e);_tmp33D=_tmp33C.f2;{void*b=
_tmp33B == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp33E=_cycalloc(
sizeof(*_tmp33E));_tmp33E[0]=({struct Cyc_Absyn_Upper_b_struct _tmp33F;_tmp33F.tag=
0;_tmp33F.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp33B);_tmp33F;});_tmp33E;});t=
Cyc_Absyn_atb_typ(_tmp339,_tmp33D,_tmp33A,b);Cyc_Tcutil_unchecked_cast(te,e,t);
return t;}}_LL24C:;_LL24D: return t;_LL249:;}_LL240:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp340=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp341;_LL24F: if(*((int*)_tmp340)!= 11)goto _LL251;_tmp341=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp340)->f1;_LL250: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp341);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp341->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp341->topt))->v));
e->topt=_tmp341->topt;goto _LL24E;_LL251:;_LL252: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp342=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp343;void*_tmp344;void*_tmp345;struct Cyc_Absyn_Conref*_tmp346;struct Cyc_Absyn_Tqual
_tmp347;struct Cyc_Absyn_Conref*_tmp348;_LL254: if(_tmp342 <= (void*)3?1:*((int*)
_tmp342)!= 4)goto _LL256;_tmp343=((struct Cyc_Absyn_PointerType_struct*)_tmp342)->f1;
_tmp344=(void*)_tmp343.elt_typ;_tmp345=(void*)_tmp343.rgn_typ;_tmp346=_tmp343.nullable;
_tmp347=_tmp343.tq;_tmp348=_tmp343.bounds;_LL255:{void*_tmp349=Cyc_Tcutil_compress(
_tmp344);struct Cyc_Absyn_FnInfo _tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_Core_Opt*
_tmp34C;void*_tmp34D;struct Cyc_List_List*_tmp34E;int _tmp34F;struct Cyc_Absyn_VarargInfo*
_tmp350;struct Cyc_List_List*_tmp351;struct Cyc_List_List*_tmp352;_LL259: if(
_tmp349 <= (void*)3?1:*((int*)_tmp349)!= 8)goto _LL25B;_tmp34A=((struct Cyc_Absyn_FnType_struct*)
_tmp349)->f1;_tmp34B=_tmp34A.tvars;_tmp34C=_tmp34A.effect;_tmp34D=(void*)_tmp34A.ret_typ;
_tmp34E=_tmp34A.args;_tmp34F=_tmp34A.c_varargs;_tmp350=_tmp34A.cyc_varargs;
_tmp351=_tmp34A.rgn_po;_tmp352=_tmp34A.attributes;_LL25A: if(_tmp34B != 0){struct
_RegionHandle _tmp353=_new_region("rgn");struct _RegionHandle*rgn=& _tmp353;
_push_region(rgn);{struct _tuple4 _tmp354=({struct _tuple4 _tmp35E;_tmp35E.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp35E.f2=rgn;_tmp35E;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp354,_tmp34B);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_Absyn_FnType_struct
_tmp35C;_tmp35C.tag=8;_tmp35C.f1=({struct Cyc_Absyn_FnInfo _tmp35D;_tmp35D.tvars=0;
_tmp35D.effect=_tmp34C;_tmp35D.ret_typ=(void*)_tmp34D;_tmp35D.args=_tmp34E;
_tmp35D.c_varargs=_tmp34F;_tmp35D.cyc_varargs=_tmp350;_tmp35D.rgn_po=_tmp351;
_tmp35D.attributes=_tmp352;_tmp35D;});_tmp35C;});_tmp35B;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({
struct Cyc_Absyn_PointerType_struct _tmp359;_tmp359.tag=4;_tmp359.f1=({struct Cyc_Absyn_PtrInfo
_tmp35A;_tmp35A.elt_typ=(void*)ftyp;_tmp35A.rgn_typ=(void*)_tmp345;_tmp35A.nullable=
_tmp346;_tmp35A.tq=_tmp347;_tmp35A.bounds=_tmp348;_tmp35A;});_tmp359;});_tmp358;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp356;_tmp356.tag=12;_tmp356.f1=inner;
_tmp356.f2=ts;_tmp356;});_tmp355;})));e->topt=({struct Cyc_Core_Opt*_tmp357=
_cycalloc(sizeof(*_tmp357));_tmp357->v=(void*)new_typ;_tmp357;});};_pop_region(
rgn);}goto _LL258;_LL25B:;_LL25C: goto _LL258;_LL258:;}goto _LL253;_LL256:;_LL257:
goto _LL253;_LL253:;}goto _LL24E;_LL24E:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp35F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp360;struct _tuple1*_tmp361;
struct Cyc_Absyn_Exp*_tmp362;struct Cyc_List_List*_tmp363;struct Cyc_Core_Opt*
_tmp364;struct Cyc_List_List*_tmp365;void*_tmp366;struct _tuple1*_tmp367;void*
_tmp368;void*_tmp369;struct Cyc_List_List*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;
void*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Core_Opt*_tmp36E;struct Cyc_Absyn_Exp*
_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*
_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*
_tmp375;struct Cyc_List_List*_tmp376;struct Cyc_Absyn_VarargCallInfo*_tmp377;
struct Cyc_Absyn_VarargCallInfo**_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*
_tmp37A;struct Cyc_List_List*_tmp37B;void*_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;
struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct Cyc_Absyn_Exp*
_tmp380;struct Cyc_Absyn_Exp*_tmp381;void*_tmp382;void*_tmp383;void*_tmp384;
struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Exp*_tmp386;struct _tagged_arr*
_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct _tagged_arr*_tmp389;struct Cyc_Absyn_Exp*
_tmp38A;struct Cyc_Absyn_Exp*_tmp38B;struct Cyc_List_List*_tmp38C;struct _tuple2*
_tmp38D;struct Cyc_List_List*_tmp38E;struct Cyc_List_List*_tmp38F;struct Cyc_Absyn_Stmt*
_tmp390;struct Cyc_Absyn_Fndecl*_tmp391;struct Cyc_Absyn_Exp*_tmp392;struct Cyc_Absyn_Vardecl*
_tmp393;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct _tuple1*
_tmp396;struct _tuple1**_tmp397;struct Cyc_List_List*_tmp398;struct Cyc_List_List**
_tmp399;struct Cyc_List_List*_tmp39A;struct Cyc_Absyn_Aggrdecl*_tmp39B;struct Cyc_Absyn_Aggrdecl**
_tmp39C;void*_tmp39D;struct Cyc_List_List*_tmp39E;struct Cyc_List_List*_tmp39F;
struct Cyc_Absyn_Tuniondecl*_tmp3A0;struct Cyc_Absyn_Tunionfield*_tmp3A1;struct
_tuple1*_tmp3A2;struct _tuple1**_tmp3A3;struct Cyc_Absyn_Enumdecl*_tmp3A4;struct
Cyc_Absyn_Enumfield*_tmp3A5;struct _tuple1*_tmp3A6;struct _tuple1**_tmp3A7;void*
_tmp3A8;struct Cyc_Absyn_Enumfield*_tmp3A9;struct Cyc_Absyn_MallocInfo _tmp3AA;int
_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AC;void**_tmp3AD;void***_tmp3AE;struct Cyc_Absyn_Exp*
_tmp3AF;struct Cyc_Absyn_Exp**_tmp3B0;int _tmp3B1;int*_tmp3B2;_LL25E: if(*((int*)
_tmp35F)!= 11)goto _LL260;_tmp360=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp35F)->f1;_LL25F: Cyc_Tcexp_tcExpNoInst(te,0,_tmp360);return;_LL260: if(*((int*)
_tmp35F)!= 2)goto _LL262;_tmp361=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp35F)->f1;
_LL261: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp361);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL262: if(*((int*)_tmp35F)!= 8)goto _LL264;_tmp362=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp35F)->f1;_tmp363=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp35F)->f2;_LL263:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp362,_tmp363);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL264: if(*((int*)_tmp35F)!= 34)goto _LL266;_tmp364=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp35F)->f1;_tmp365=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp35F)->f2;
_LL265: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp365);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL266: if(*((int*)_tmp35F)!= 0)goto _LL268;_tmp366=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp35F)->f1;_LL267: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp366,e);goto _LL25D;_LL268: if(*((int*)_tmp35F)!= 1)goto _LL26A;_tmp367=((
struct Cyc_Absyn_Var_e_struct*)_tmp35F)->f1;_tmp368=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35F)->f2;_LL269: t=Cyc_Tcexp_tcVar(te,loc,_tmp367,_tmp368);goto _LL25D;_LL26A:
if(*((int*)_tmp35F)!= 3)goto _LL26C;_tmp369=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp35F)->f1;_tmp36A=((struct Cyc_Absyn_Primop_e_struct*)_tmp35F)->f2;_LL26B: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp369,_tmp36A);goto _LL25D;_LL26C: if(*((int*)
_tmp35F)!= 5)goto _LL26E;_tmp36B=((struct Cyc_Absyn_Increment_e_struct*)_tmp35F)->f1;
_tmp36C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp35F)->f2;_LL26D: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp36B,_tmp36C);goto _LL25D;_LL26E: if(*((int*)_tmp35F)!= 4)goto
_LL270;_tmp36D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp35F)->f1;_tmp36E=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp35F)->f2;_tmp36F=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp35F)->f3;_LL26F: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp36D,_tmp36E,_tmp36F);
goto _LL25D;_LL270: if(*((int*)_tmp35F)!= 6)goto _LL272;_tmp370=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp35F)->f1;_tmp371=((struct Cyc_Absyn_Conditional_e_struct*)_tmp35F)->f2;
_tmp372=((struct Cyc_Absyn_Conditional_e_struct*)_tmp35F)->f3;_LL271: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp370,_tmp371,_tmp372);goto _LL25D;_LL272: if(*((int*)_tmp35F)!= 7)
goto _LL274;_tmp373=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp35F)->f1;_tmp374=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp35F)->f2;_LL273: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp373,_tmp374);goto _LL25D;_LL274: if(*((int*)_tmp35F)!= 9)goto _LL276;
_tmp375=((struct Cyc_Absyn_FnCall_e_struct*)_tmp35F)->f1;_tmp376=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp35F)->f2;_tmp377=((struct Cyc_Absyn_FnCall_e_struct*)_tmp35F)->f3;_tmp378=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp35F)->f3;
_LL275: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp375,_tmp376,_tmp378);goto _LL25D;
_LL276: if(*((int*)_tmp35F)!= 10)goto _LL278;_tmp379=((struct Cyc_Absyn_Throw_e_struct*)
_tmp35F)->f1;_LL277: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp379);goto _LL25D;_LL278:
if(*((int*)_tmp35F)!= 12)goto _LL27A;_tmp37A=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp35F)->f1;_tmp37B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp35F)->f2;_LL279:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp37A,_tmp37B);goto _LL25D;_LL27A: if(*((
int*)_tmp35F)!= 13)goto _LL27C;_tmp37C=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp35F)->f1;_tmp37D=((struct Cyc_Absyn_Cast_e_struct*)_tmp35F)->f2;_LL27B: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp37C,_tmp37D);goto _LL25D;_LL27C: if(*((int*)_tmp35F)!= 14)goto
_LL27E;_tmp37E=((struct Cyc_Absyn_Address_e_struct*)_tmp35F)->f1;_LL27D: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp37E);goto _LL25D;_LL27E: if(*((int*)_tmp35F)!= 15)goto _LL280;
_tmp37F=((struct Cyc_Absyn_New_e_struct*)_tmp35F)->f1;_tmp380=((struct Cyc_Absyn_New_e_struct*)
_tmp35F)->f2;_LL27F: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp37F,e,_tmp380);goto _LL25D;
_LL280: if(*((int*)_tmp35F)!= 17)goto _LL282;_tmp381=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp35F)->f1;_LL281: {void*_tmp3B3=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp381);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp3B3);goto _LL25D;}_LL282: if(*((int*)_tmp35F)!= 16)goto _LL284;
_tmp382=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp35F)->f1;_LL283: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp382);goto _LL25D;_LL284: if(*((int*)_tmp35F)!= 18)goto _LL286;
_tmp383=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp35F)->f1;_tmp384=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp35F)->f2;_LL285: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp383,_tmp384);goto _LL25D;_LL286: if(*((int*)_tmp35F)!= 19)goto _LL288;
_LL287:({void*_tmp3B4[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(char),35),_tag_arr(_tmp3B4,sizeof(void*),0));});return;_LL288: if(*((int*)
_tmp35F)!= 20)goto _LL28A;_tmp385=((struct Cyc_Absyn_Deref_e_struct*)_tmp35F)->f1;
_LL289: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp385);goto _LL25D;_LL28A: if(*((int*)
_tmp35F)!= 21)goto _LL28C;_tmp386=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35F)->f1;
_tmp387=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35F)->f2;_LL28B: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp386,_tmp387);goto _LL25D;_LL28C: if(*((int*)_tmp35F)!= 22)goto
_LL28E;_tmp388=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35F)->f1;_tmp389=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35F)->f2;_LL28D: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp388,_tmp389);goto _LL25D;_LL28E: if(*((int*)_tmp35F)!= 23)goto
_LL290;_tmp38A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp35F)->f1;_tmp38B=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp35F)->f2;_LL28F: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp38A,_tmp38B);goto _LL25D;_LL290: if(*((int*)_tmp35F)!= 24)goto
_LL292;_tmp38C=((struct Cyc_Absyn_Tuple_e_struct*)_tmp35F)->f1;_LL291: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp38C);goto _LL25D;_LL292: if(*((int*)_tmp35F)!= 25)goto _LL294;
_tmp38D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp35F)->f1;_tmp38E=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp35F)->f2;_LL293: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp38D,_tmp38E);goto
_LL25D;_LL294: if(*((int*)_tmp35F)!= 26)goto _LL296;_tmp38F=((struct Cyc_Absyn_Array_e_struct*)
_tmp35F)->f1;_LL295: {void**elt_topt=0;if(topt != 0){void*_tmp3B5=Cyc_Tcutil_compress(*
topt);void*_tmp3B6;void**_tmp3B7;_LL2AB: if(_tmp3B5 <= (void*)3?1:*((int*)_tmp3B5)
!= 7)goto _LL2AD;_tmp3B6=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3B5)->f1;
_tmp3B7=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3B5)->f1);
_LL2AC: elt_topt=(void**)_tmp3B7;goto _LL2AA;_LL2AD:;_LL2AE: goto _LL2AA;_LL2AA:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp38F);goto _LL25D;}_LL296: if(*((int*)_tmp35F)
!= 35)goto _LL298;_tmp390=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp35F)->f1;_LL297:
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp390);goto _LL25D;_LL298: if(*((int*)_tmp35F)
!= 36)goto _LL29A;_tmp391=((struct Cyc_Absyn_Codegen_e_struct*)_tmp35F)->f1;_LL299:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp391);goto _LL25D;_LL29A: if(*((int*)_tmp35F)
!= 37)goto _LL29C;_tmp392=((struct Cyc_Absyn_Fill_e_struct*)_tmp35F)->f1;_LL29B: t=
Cyc_Tcexp_tcFill(te,loc,topt,_tmp392);goto _LL25D;_LL29C: if(*((int*)_tmp35F)!= 27)
goto _LL29E;_tmp393=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp35F)->f1;
_tmp394=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp35F)->f2;_tmp395=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp35F)->f3;_LL29D: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp393,_tmp394,_tmp395);goto _LL25D;_LL29E: if(*((int*)_tmp35F)!= 28)
goto _LL2A0;_tmp396=((struct Cyc_Absyn_Struct_e_struct*)_tmp35F)->f1;_tmp397=(
struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp35F)->f1;_tmp398=((
struct Cyc_Absyn_Struct_e_struct*)_tmp35F)->f2;_tmp399=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp35F)->f2;_tmp39A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp35F)->f3;_tmp39B=((struct Cyc_Absyn_Struct_e_struct*)_tmp35F)->f4;_tmp39C=(
struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp35F)->f4;
_LL29F: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp397,_tmp399,_tmp39A,_tmp39C);goto
_LL25D;_LL2A0: if(*((int*)_tmp35F)!= 29)goto _LL2A2;_tmp39D=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp35F)->f1;_tmp39E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp35F)->f2;_LL2A1:
t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp39D,_tmp39E);goto _LL25D;_LL2A2: if(*((int*)
_tmp35F)!= 30)goto _LL2A4;_tmp39F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp35F)->f1;
_tmp3A0=((struct Cyc_Absyn_Tunion_e_struct*)_tmp35F)->f2;_tmp3A1=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp35F)->f3;_LL2A3: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp39F,_tmp3A0,_tmp3A1);
goto _LL25D;_LL2A4: if(*((int*)_tmp35F)!= 31)goto _LL2A6;_tmp3A2=((struct Cyc_Absyn_Enum_e_struct*)
_tmp35F)->f1;_tmp3A3=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp35F)->f1;
_tmp3A4=((struct Cyc_Absyn_Enum_e_struct*)_tmp35F)->f2;_tmp3A5=((struct Cyc_Absyn_Enum_e_struct*)
_tmp35F)->f3;_LL2A5:*_tmp3A3=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp3A5))->name;
t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));
_tmp3B8[0]=({struct Cyc_Absyn_EnumType_struct _tmp3B9;_tmp3B9.tag=12;_tmp3B9.f1=((
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3A4))->name;_tmp3B9.f2=_tmp3A4;_tmp3B9;});
_tmp3B8;});goto _LL25D;_LL2A6: if(*((int*)_tmp35F)!= 32)goto _LL2A8;_tmp3A6=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp35F)->f1;_tmp3A7=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp35F)->f1;_tmp3A8=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp35F)->f2;_tmp3A9=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp35F)->f3;_LL2A7:*
_tmp3A7=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp3A9))->name;t=_tmp3A8;goto
_LL25D;_LL2A8: if(*((int*)_tmp35F)!= 33)goto _LL25D;_tmp3AA=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp35F)->f1;_tmp3AB=_tmp3AA.is_calloc;_tmp3AC=_tmp3AA.rgn;_tmp3AD=_tmp3AA.elt_type;
_tmp3AE=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp35F)->f1).elt_type;
_tmp3AF=_tmp3AA.num_elts;_tmp3B0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp35F)->f1).num_elts;_tmp3B1=_tmp3AA.fat_result;_tmp3B2=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp35F)->f1).fat_result;_LL2A9: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp3AC,_tmp3AE,
_tmp3B0,_tmp3AB,_tmp3B2);goto _LL25D;_LL25D:;}e->topt=({struct Cyc_Core_Opt*
_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->v=(void*)t;_tmp3BA;});}
