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
Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_typ;extern void*
Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_at_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_Std_zstrcmp(struct _tagged_arr,
struct _tagged_arr);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct
Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*r1,struct _tagged_arr fmt,struct
_tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_PP_Ppstate;struct
Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
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
_tmp53;int _tmp54;struct _tagged_arr _tmp55;_LL34: if(_tmp4F <= (void*)1?1:*((int*)
_tmp4F)!= 0)goto _LL36;_tmp50=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp4F)->f1;
_LL35: t=_tmp50 == (void*)1?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;goto _LL33;
_LL36: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 1)goto _LL38;_tmp51=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp4F)->f1;_LL37: t=_tmp51 == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL33;_LL38: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 3)
goto _LL3A;_tmp52=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp4F)->f1;_LL39: t=
_tmp52 == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL33;
_LL3A: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 4)goto _LL3C;_LL3B: t=Cyc_Absyn_float_typ;
goto _LL33;_LL3C: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 2)goto _LL3E;_tmp53=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp4F)->f1;_tmp54=((struct Cyc_Absyn_Int_c_struct*)
_tmp4F)->f2;_LL3D: if(topt == 0)t=_tmp53 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
else{void*_tmp56=Cyc_Tcutil_compress(*topt);void*_tmp57;void*_tmp58;void*_tmp59;
void*_tmp5A;void*_tmp5B;void*_tmp5C;_LL43: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)
!= 5)goto _LL45;_tmp57=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f1;
_tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f2;if((int)_tmp58 != 0)
goto _LL45;_LL44: t=_tmp57 == (void*)1?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp5D=_cycalloc(
sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Const_e_struct _tmp5E;_tmp5E.tag=0;
_tmp5E.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*_tmp5F=_cycalloc(
sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_Char_c_struct _tmp60;_tmp60.tag=0;
_tmp60.f1=(void*)_tmp57;_tmp60.f2=(char)_tmp54;_tmp60;});_tmp5F;}));_tmp5E;});
_tmp5D;})));goto _LL42;_LL45: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 5)goto _LL47;
_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f1;_tmp5A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp56)->f2;if((int)_tmp5A != 1)goto _LL47;_LL46: t=
_tmp59 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Const_e_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[
0]=({struct Cyc_Absyn_Const_e_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(void*)((void*)({
struct Cyc_Absyn_Short_c_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({
struct Cyc_Absyn_Short_c_struct _tmp64;_tmp64.tag=1;_tmp64.f1=(void*)_tmp59;_tmp64.f2=(
short)_tmp54;_tmp64;});_tmp63;}));_tmp62;});_tmp61;})));goto _LL42;_LL47: if(
_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 5)goto _LL49;_tmp5B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp56)->f1;_LL48: t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL42;_LL49: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 4)goto _LL4B;if(!(_tmp54 == 0))
goto _LL4B;_LL4A:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Absyn_Const_e_struct
_tmp66;_tmp66.tag=0;_tmp66.f1=(void*)((void*)0);_tmp66;});_tmp65;})));{struct Cyc_List_List*
_tmp67=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=({struct Cyc_Absyn_PointerType_struct
_tmp69;_tmp69.tag=4;_tmp69.f1=({struct Cyc_Absyn_PtrInfo _tmp6A;_tmp6A.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->v=_tmp67;_tmp6C;}));_tmp6A.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->v=_tmp67;_tmp6B;}));_tmp6A.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp6A.tq=Cyc_Absyn_empty_tqual();
_tmp6A.bounds=Cyc_Absyn_empty_conref();_tmp6A;});_tmp69;});_tmp68;});goto _LL42;}
_LL4B: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 17)goto _LL4D;_tmp5C=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp56)->f1;_LL4C: {struct Cyc_Absyn_TypeInt_struct*
_tmp6D=({struct Cyc_Absyn_TypeInt_struct*_tmp73=_cycalloc_atomic(sizeof(*_tmp73));
_tmp73[0]=({struct Cyc_Absyn_TypeInt_struct _tmp74;_tmp74.tag=18;_tmp74.f1=_tmp54;
_tmp74;});_tmp73;});if(!Cyc_Tcutil_unify(_tmp5C,(void*)_tmp6D)){({struct Cyc_Std_String_pa_struct
_tmp70;_tmp70.tag=0;_tmp70.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp6D);{struct Cyc_Std_String_pa_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(_tmp5C);{void*_tmp6E[2]={& _tmp6F,& _tmp70};Cyc_Tcutil_terr(
loc,_tag_arr("expecting %s but found %s",sizeof(char),26),_tag_arr(_tmp6E,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_TagType_struct
_tmp72;_tmp72.tag=17;_tmp72.f1=(void*)((void*)_tmp6D);_tmp72;});_tmp71;});goto
_LL42;}_LL4D:;_LL4E: t=_tmp53 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL42;_LL42:;}goto _LL33;_LL3E: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 5)
goto _LL40;_tmp55=((struct Cyc_Absyn_String_c_struct*)_tmp4F)->f1;_LL3F: {int len=(
int)_get_arr_size(_tmp55,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({
struct Cyc_Absyn_Int_c_struct _tmp82;_tmp82.tag=2;_tmp82.f1=(void*)((void*)1);
_tmp82.f2=len;_tmp82;});_tmp81;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp75=
_cycalloc(sizeof(*_tmp75));_tmp75->v=(void*)Cyc_Absyn_uint_typ;_tmp75;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_typ,(void*)2,Cyc_Absyn_const_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp77;_tmp77.tag=0;_tmp77.f1=elen;_tmp77;});_tmp76;}));if(topt != 0){void*_tmp78=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Tqual _tmp79;_LL50: if(_tmp78 <= (void*)
3?1:*((int*)_tmp78)!= 7)goto _LL52;_tmp79=((struct Cyc_Absyn_ArrayType_struct*)
_tmp78)->f2;_LL51: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp7A=
_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_ArrayType_struct _tmp7B;
_tmp7B.tag=7;_tmp7B.f1=(void*)Cyc_Absyn_uchar_typ;_tmp7B.f2=_tmp79;_tmp7B.f3=(
struct Cyc_Absyn_Exp*)elen;_tmp7B;});_tmp7A;});_LL52: if(_tmp78 <= (void*)3?1:*((
int*)_tmp78)!= 4)goto _LL54;_LL53: if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp7C=_cycalloc(sizeof(*_tmp7C));
_tmp7C->v=(void*)t;_tmp7C;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}
else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp7D;})),Cyc_Absyn_const_tqual(),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=elen;_tmp7F;});
_tmp7E;}));if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(te,loc,t,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->v=(
void*)t;_tmp80;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}}goto _LL4F;
_LL54:;_LL55: goto _LL4F;_LL4F:;}return t;}_LL40: if((int)_tmp4F != 0)goto _LL33;_LL41: {
struct Cyc_List_List*_tmp83=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_PointerType_struct
_tmp85;_tmp85.tag=4;_tmp85.f1=({struct Cyc_Absyn_PtrInfo _tmp86;_tmp86.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->v=_tmp83;_tmp88;}));_tmp86.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->v=_tmp83;_tmp87;}));_tmp86.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp86.tq=Cyc_Absyn_empty_tqual();
_tmp86.bounds=Cyc_Absyn_empty_conref();_tmp86;});_tmp85;});_tmp84;});goto _LL33;}
_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp89=b;struct Cyc_Absyn_Vardecl*_tmp8A;struct
Cyc_Absyn_Fndecl*_tmp8B;struct Cyc_Absyn_Vardecl*_tmp8C;struct Cyc_Absyn_Vardecl*
_tmp8D;struct Cyc_Absyn_Vardecl*_tmp8E;_LL57: if((int)_tmp89 != 0)goto _LL59;_LL58:
return({struct Cyc_Std_String_pa_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(q);{void*_tmp8F[1]={& _tmp90};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("undeclared identifier: %s",sizeof(char),26),_tag_arr(_tmp8F,
sizeof(void*),1));}});_LL59: if(_tmp89 <= (void*)1?1:*((int*)_tmp89)!= 0)goto _LL5B;
_tmp8A=((struct Cyc_Absyn_Global_b_struct*)_tmp89)->f1;_LL5A:*q=*_tmp8A->name;
return(void*)_tmp8A->type;_LL5B: if(_tmp89 <= (void*)1?1:*((int*)_tmp89)!= 1)goto
_LL5D;_tmp8B=((struct Cyc_Absyn_Funname_b_struct*)_tmp89)->f1;_LL5C:*q=*_tmp8B->name;
return Cyc_Tcutil_fndecl2typ(_tmp8B);_LL5D: if(_tmp89 <= (void*)1?1:*((int*)_tmp89)
!= 4)goto _LL5F;_tmp8C=((struct Cyc_Absyn_Pat_b_struct*)_tmp89)->f1;_LL5E: _tmp8D=
_tmp8C;goto _LL60;_LL5F: if(_tmp89 <= (void*)1?1:*((int*)_tmp89)!= 3)goto _LL61;
_tmp8D=((struct Cyc_Absyn_Local_b_struct*)_tmp89)->f1;_LL60: _tmp8E=_tmp8D;goto
_LL62;_LL61: if(_tmp89 <= (void*)1?1:*((int*)_tmp89)!= 2)goto _LL56;_tmp8E=((struct
Cyc_Absyn_Param_b_struct*)_tmp89)->f1;_LL62:(*q).f1=(void*)0;return(void*)_tmp8E->type;
_LL56:;}static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{
void*_tmp91=(void*)fmt->r;void*_tmp92;struct _tagged_arr _tmp93;struct Cyc_Absyn_Exp*
_tmp94;struct Cyc_Absyn_Exp _tmp95;void*_tmp96;void*_tmp97;struct _tagged_arr _tmp98;
_LL64: if(*((int*)_tmp91)!= 0)goto _LL66;_tmp92=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp91)->f1;if(_tmp92 <= (void*)1?1:*((int*)_tmp92)!= 5)goto _LL66;_tmp93=((struct
Cyc_Absyn_String_c_struct*)_tmp92)->f1;_LL65: _tmp98=_tmp93;goto _LL67;_LL66: if(*((
int*)_tmp91)!= 13)goto _LL68;_tmp94=((struct Cyc_Absyn_Cast_e_struct*)_tmp91)->f2;
_tmp95=*_tmp94;_tmp96=(void*)_tmp95.r;if(*((int*)_tmp96)!= 0)goto _LL68;_tmp97=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp96)->f1;if(_tmp97 <= (void*)1?1:*((
int*)_tmp97)!= 5)goto _LL68;_tmp98=((struct Cyc_Absyn_String_c_struct*)_tmp97)->f1;
_LL67: desc_types=type_getter(te,(struct _tagged_arr)_tmp98,fmt->loc);goto _LL63;
_LL68:;_LL69: return;_LL63:;}if(opt_args != 0){struct Cyc_List_List*_tmp99=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0?_tmp99 != 0: 0;(desc_types=
desc_types->tl,_tmp99=_tmp99->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmp99->hd;if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct
Cyc_Std_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp9A[2]={& _tmp9B,& _tmp9C};Cyc_Tcutil_terr(e->loc,_tag_arr("descriptor has type \n%s\n but argument has type \n%s",
sizeof(char),51),_tag_arr(_tmp9A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(desc_types != 0)({void*_tmp9D[0]={};Cyc_Tcutil_terr(fmt->loc,_tag_arr("too few arguments",
sizeof(char),18),_tag_arr(_tmp9D,sizeof(void*),0));});if(_tmp99 != 0)({void*
_tmp9E[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp99->hd)->loc,_tag_arr("too many arguments",
sizeof(char),19),_tag_arr(_tmp9E,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp9F=p;_LL6B: if((int)_tmp9F != 0)goto _LL6D;_LL6C:
goto _LL6E;_LL6D: if((int)_tmp9F != 2)goto _LL6F;_LL6E: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_Std_String_pa_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmpA0[1]={& _tmpA1};Cyc_Tcutil_terr(loc,_tag_arr("expecting arithmetic type but found %s",
sizeof(char),39),_tag_arr(_tmpA0,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL6F: if((int)_tmp9F != 11)goto _LL71;_LL70: Cyc_Tcutil_check_contains_assign(
e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_Std_String_pa_struct _tmpA3;
_tmpA3.tag=0;_tmpA3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA2[
1]={& _tmpA3};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral or * type but found %s",
sizeof(char),42),_tag_arr(_tmpA2,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;
_LL71: if((int)_tmp9F != 12)goto _LL73;_LL72: if(!Cyc_Tcutil_is_integral(e))({struct
Cyc_Std_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmpA4[1]={& _tmpA5};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral type but found %s",
sizeof(char),37),_tag_arr(_tmpA4,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL73: if((int)_tmp9F != 19)goto _LL75;_LL74:{void*_tmpA6=
t;_LL78: if(_tmpA6 <= (void*)3?1:*((int*)_tmpA6)!= 7)goto _LL7A;_LL79: goto _LL7B;
_LL7A: if(_tmpA6 <= (void*)3?1:*((int*)_tmpA6)!= 4)goto _LL7C;_LL7B: goto _LL77;_LL7C:;
_LL7D:({struct Cyc_Std_String_pa_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA7[1]={& _tmpA8};Cyc_Tcutil_terr(
loc,_tag_arr("size requires pointer or array type, not %s",sizeof(char),44),
_tag_arr(_tmpA7,sizeof(void*),1));}});_LL77:;}return Cyc_Absyn_uint_typ;_LL75:;
_LL76:({void*_tmpA9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Non-unary primop",sizeof(char),17),_tag_arr(_tmpA9,sizeof(void*),0));});
_LL6A:;}static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_Std_String_pa_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmpAA[1]={& _tmpAB};Cyc_Tcutil_terr(e1->loc,_tag_arr("type %s cannot be used here",
sizeof(char),28),_tag_arr(_tmpAA,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpAC;}));}if(!checker(e2)){({struct Cyc_Std_String_pa_struct _tmpAE;_tmpAE.tag=
0;_tmpAE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmpAD[1]={& _tmpAE};Cyc_Tcutil_terr(e2->loc,
_tag_arr("type %s cannot be used here",sizeof(char),28),_tag_arr(_tmpAD,sizeof(
void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpAF=_cycalloc(
sizeof(*_tmpAF));_tmpAF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpAF;}));}{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpB0=t1;struct Cyc_Absyn_PtrInfo
_tmpB1;void*_tmpB2;void*_tmpB3;struct Cyc_Absyn_Conref*_tmpB4;struct Cyc_Absyn_Tqual
_tmpB5;struct Cyc_Absyn_Conref*_tmpB6;_LL7F: if(_tmpB0 <= (void*)3?1:*((int*)_tmpB0)
!= 4)goto _LL81;_tmpB1=((struct Cyc_Absyn_PointerType_struct*)_tmpB0)->f1;_tmpB2=(
void*)_tmpB1.elt_typ;_tmpB3=(void*)_tmpB1.rgn_typ;_tmpB4=_tmpB1.nullable;_tmpB5=
_tmpB1.tq;_tmpB6=_tmpB1.bounds;_LL80: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmpB2),(void*)1))({void*_tmpB7[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(char),50),_tag_arr(_tmpB7,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))({struct Cyc_Std_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmpB8[1]={& _tmpB9};Cyc_Tcutil_terr(
e2->loc,_tag_arr("expecting int but found %s",sizeof(char),27),_tag_arr(_tmpB8,
sizeof(void*),1));}});_tmpB6=Cyc_Absyn_compress_conref(_tmpB6);{void*_tmpBA=(
void*)_tmpB6->v;void*_tmpBB;void*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;_LL84: if(
_tmpBA <= (void*)1?1:*((int*)_tmpBA)!= 0)goto _LL86;_tmpBB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBA)->f1;if((int)_tmpBB != 0)goto _LL86;_LL85: return t1;_LL86: if(_tmpBA <= (void*)
1?1:*((int*)_tmpBA)!= 0)goto _LL88;_tmpBC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBA)->f1;if(_tmpBC <= (void*)1?1:*((int*)_tmpBC)!= 0)goto _LL88;_tmpBD=((struct
Cyc_Absyn_Upper_b_struct*)_tmpBC)->f1;_LL87: {struct Cyc_Absyn_PointerType_struct*
_tmpBE=({struct Cyc_Absyn_PointerType_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));
_tmpBF[0]=({struct Cyc_Absyn_PointerType_struct _tmpC0;_tmpC0.tag=4;_tmpC0.f1=({
struct Cyc_Absyn_PtrInfo _tmpC1;_tmpC1.elt_typ=(void*)_tmpB2;_tmpC1.rgn_typ=(void*)
_tmpB3;_tmpC1.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
1);_tmpC1.tq=_tmpB5;_tmpC1.bounds=Cyc_Absyn_new_conref((void*)0);_tmpC1;});
_tmpC0;});_tmpBF;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpBE);return(void*)
_tmpBE;}_LL88:;_LL89:(void*)(_tmpB6->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmpC3;_tmpC3.tag=0;_tmpC3.f1=(void*)((void*)0);_tmpC3;});_tmpC2;})));return t1;
_LL83:;}_LL81:;_LL82: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LL7E:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_Std_String_pa_struct _tmpC6;_tmpC6.tag=
0;_tmpC6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct Cyc_Std_String_pa_struct _tmpC5;_tmpC5.tag=0;
_tmpC5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmpC4[2]={& _tmpC5,& _tmpC6};Cyc_Tcutil_terr(e1->loc,
_tag_arr("pointer arithmetic on values of different types (%s != %s)",sizeof(
char),59),_tag_arr(_tmpC4,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpC7[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(char),50),_tag_arr(_tmpC7,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmpC9;
_tmpC9.tag=0;_tmpC9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmpC8[2]={& _tmpC9,& _tmpCA};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting either %s or int but found %s",
sizeof(char),40),_tag_arr(_tmpC8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}else{return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num?e2_is_num: 0)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2)?Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*
_tmpCB));_tmpCB->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCB;}))): 0)return Cyc_Absyn_sint_typ;
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1)?1: Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{{
struct _tuple0 _tmpCD=({struct _tuple0 _tmpCC;_tmpCC.f1=Cyc_Tcutil_compress(t1);
_tmpCC.f2=Cyc_Tcutil_compress(t2);_tmpCC;});void*_tmpCE;struct Cyc_Absyn_PtrInfo
_tmpCF;void*_tmpD0;void*_tmpD1;struct Cyc_Absyn_PtrInfo _tmpD2;void*_tmpD3;_LL8B:
_tmpCE=_tmpCD.f1;if(_tmpCE <= (void*)3?1:*((int*)_tmpCE)!= 4)goto _LL8D;_tmpCF=((
struct Cyc_Absyn_PointerType_struct*)_tmpCE)->f1;_tmpD0=(void*)_tmpCF.elt_typ;
_tmpD1=_tmpCD.f2;if(_tmpD1 <= (void*)3?1:*((int*)_tmpD1)!= 4)goto _LL8D;_tmpD2=((
struct Cyc_Absyn_PointerType_struct*)_tmpD1)->f1;_tmpD3=(void*)_tmpD2.elt_typ;
_LL8C: if(Cyc_Tcutil_unify(_tmpD0,_tmpD3))return Cyc_Absyn_sint_typ;goto _LL8A;
_LL8D:;_LL8E: goto _LL8A;_LL8A:;}({struct Cyc_Std_String_pa_struct _tmpD6;_tmpD6.tag=
0;_tmpD6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmpD5;_tmpD5.tag=0;_tmpD5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmpD4[2]={& _tmpD5,& _tmpD6};Cyc_Tcutil_terr(loc,_tag_arr("comparison not allowed between %s and %s",
sizeof(char),41),_tag_arr(_tmpD4,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpD7=_cycalloc(sizeof(*_tmpD7));
_tmpD7->v=Cyc_Tcenv_lookup_type_vars(te);_tmpD7;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpD8=p;_LL90: if((int)_tmpD8 != 0)
goto _LL92;_LL91: return Cyc_Tcexp_tcPlus(te,e1,e2);_LL92: if((int)_tmpD8 != 2)goto
_LL94;_LL93: return Cyc_Tcexp_tcMinus(te,e1,e2);_LL94: if((int)_tmpD8 != 1)goto _LL96;
_LL95: goto _LL97;_LL96: if((int)_tmpD8 != 3)goto _LL98;_LL97: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL98: if((int)_tmpD8 != 4)goto _LL9A;_LL99: goto
_LL9B;_LL9A: if((int)_tmpD8 != 13)goto _LL9C;_LL9B: goto _LL9D;_LL9C: if((int)_tmpD8 != 
14)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if((int)_tmpD8 != 15)goto _LLA0;_LL9F: goto _LLA1;
_LLA0: if((int)_tmpD8 != 16)goto _LLA2;_LLA1: goto _LLA3;_LLA2: if((int)_tmpD8 != 17)
goto _LLA4;_LLA3: goto _LLA5;_LLA4: if((int)_tmpD8 != 18)goto _LLA6;_LLA5: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LLA6: if((int)_tmpD8 != 5)goto _LLA8;_LLA7: goto
_LLA9;_LLA8: if((int)_tmpD8 != 6)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if((int)_tmpD8 != 
7)goto _LLAC;_LLAB: goto _LLAD;_LLAC: if((int)_tmpD8 != 8)goto _LLAE;_LLAD: goto _LLAF;
_LLAE: if((int)_tmpD8 != 9)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmpD8 != 10)
goto _LLB2;_LLB1: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLB2:;_LLB3:({void*
_tmpD9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad binary primop",sizeof(char),18),_tag_arr(_tmpD9,sizeof(void*),0));});
_LL8F:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2?((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLB4: return({
void*_tmpDA[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has 0 arguments",
sizeof(char),35),_tag_arr(_tmpDA,sizeof(void*),0));});case 1: _LLB5: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLB6: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLB7: return({void*_tmpDB[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has > 2 arguments",
sizeof(char),37),_tag_arr(_tmpDB,sizeof(void*),0));});}return t;}}struct _tuple8{
struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){{void*_tmpDC=(void*)e->r;void*_tmpDD;struct Cyc_Absyn_Vardecl*
_tmpDE;void*_tmpDF;struct Cyc_Absyn_Vardecl*_tmpE0;void*_tmpE1;struct Cyc_Absyn_Vardecl*
_tmpE2;void*_tmpE3;struct Cyc_Absyn_Vardecl*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;
struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct _tagged_arr*_tmpE8;
struct Cyc_Absyn_Exp*_tmpE9;struct _tagged_arr*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;
struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;_LLBA: if(*((int*)_tmpDC)!= 
1)goto _LLBC;_tmpDD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpDC)->f2;if(_tmpDD
<= (void*)1?1:*((int*)_tmpDD)!= 2)goto _LLBC;_tmpDE=((struct Cyc_Absyn_Param_b_struct*)
_tmpDD)->f1;_LLBB: _tmpE0=_tmpDE;goto _LLBD;_LLBC: if(*((int*)_tmpDC)!= 1)goto _LLBE;
_tmpDF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpDC)->f2;if(_tmpDF <= (void*)1?1:*((
int*)_tmpDF)!= 3)goto _LLBE;_tmpE0=((struct Cyc_Absyn_Local_b_struct*)_tmpDF)->f1;
_LLBD: _tmpE2=_tmpE0;goto _LLBF;_LLBE: if(*((int*)_tmpDC)!= 1)goto _LLC0;_tmpE1=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpDC)->f2;if(_tmpE1 <= (void*)1?1:*((int*)
_tmpE1)!= 4)goto _LLC0;_tmpE2=((struct Cyc_Absyn_Pat_b_struct*)_tmpE1)->f1;_LLBF:
_tmpE4=_tmpE2;goto _LLC1;_LLC0: if(*((int*)_tmpDC)!= 1)goto _LLC2;_tmpE3=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpDC)->f2;if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)
!= 0)goto _LLC2;_tmpE4=((struct Cyc_Absyn_Global_b_struct*)_tmpE3)->f1;_LLC1: if(!(
_tmpE4->tq).q_const)return;goto _LLB9;_LLC2: if(*((int*)_tmpDC)!= 23)goto _LLC4;
_tmpE5=((struct Cyc_Absyn_Subscript_e_struct*)_tmpDC)->f1;_tmpE6=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpDC)->f2;_LLC3:{void*_tmpEE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE5->topt))->v);struct Cyc_Absyn_PtrInfo _tmpEF;struct Cyc_Absyn_Tqual
_tmpF0;struct Cyc_Absyn_Tqual _tmpF1;struct Cyc_List_List*_tmpF2;_LLD1: if(_tmpEE <= (
void*)3?1:*((int*)_tmpEE)!= 4)goto _LLD3;_tmpEF=((struct Cyc_Absyn_PointerType_struct*)
_tmpEE)->f1;_tmpF0=_tmpEF.tq;_LLD2: _tmpF1=_tmpF0;goto _LLD4;_LLD3: if(_tmpEE <= (
void*)3?1:*((int*)_tmpEE)!= 7)goto _LLD5;_tmpF1=((struct Cyc_Absyn_ArrayType_struct*)
_tmpEE)->f2;_LLD4: if(!_tmpF1.q_const)return;goto _LLD0;_LLD5: if(_tmpEE <= (void*)3?
1:*((int*)_tmpEE)!= 9)goto _LLD7;_tmpF2=((struct Cyc_Absyn_TupleType_struct*)
_tmpEE)->f1;_LLD6: {unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmpE6);{struct
_handler_cons _tmpF3;_push_handler(& _tmpF3);{int _tmpF5=0;if(setjmp(_tmpF3.handler))
_tmpF5=1;if(!_tmpF5){{struct _tuple8 _tmpF7;struct Cyc_Absyn_Tqual _tmpF8;struct
_tuple8*_tmpF6=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmpF2,(int)i);_tmpF7=*_tmpF6;_tmpF8=_tmpF7.f1;if(!_tmpF8.q_const){_npop_handler(
0);return;}};_pop_handler();}else{void*_tmpF4=(void*)_exn_thrown;void*_tmpFA=
_tmpF4;_LLDA: if(_tmpFA != Cyc_List_Nth)goto _LLDC;_LLDB: return;_LLDC:;_LLDD:(void)
_throw(_tmpFA);_LLD9:;}}}goto _LLD0;}_LLD7:;_LLD8: goto _LLD0;_LLD0:;}goto _LLB9;
_LLC4: if(*((int*)_tmpDC)!= 21)goto _LLC6;_tmpE7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpDC)->f1;_tmpE8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDC)->f2;_LLC5:{
void*_tmpFB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE7->topt))->v);
struct Cyc_Absyn_AggrInfo _tmpFC;void*_tmpFD;struct Cyc_Absyn_Aggrdecl**_tmpFE;
struct Cyc_List_List*_tmpFF;_LLDF: if(_tmpFB <= (void*)3?1:*((int*)_tmpFB)!= 10)
goto _LLE1;_tmpFC=((struct Cyc_Absyn_AggrType_struct*)_tmpFB)->f1;_tmpFD=(void*)
_tmpFC.aggr_info;if(*((int*)_tmpFD)!= 1)goto _LLE1;_tmpFE=((struct Cyc_Absyn_KnownAggr_struct*)
_tmpFD)->f1;_LLE0: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmpFE == 0?0: Cyc_Absyn_lookup_decl_field(*_tmpFE,_tmpE8);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLDE;}_LLE1: if(_tmpFB <= (void*)3?1:*((int*)_tmpFB)!= 11)goto _LLE3;
_tmpFF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpFB)->f2;_LLE2: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmpFF,_tmpE8);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLDE;}_LLE3:;_LLE4: goto _LLDE;_LLDE:;}goto _LLB9;_LLC6: if(*((int*)_tmpDC)!= 
22)goto _LLC8;_tmpE9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDC)->f1;_tmpEA=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDC)->f2;_LLC7:{void*_tmp100=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp101;void*_tmp102;_LLE6: if(_tmp100 <= (void*)3?1:*((int*)_tmp100)!= 4)goto
_LLE8;_tmp101=((struct Cyc_Absyn_PointerType_struct*)_tmp100)->f1;_tmp102=(void*)
_tmp101.elt_typ;_LLE7:{void*_tmp103=Cyc_Tcutil_compress(_tmp102);struct Cyc_Absyn_AggrInfo
_tmp104;void*_tmp105;struct Cyc_Absyn_Aggrdecl**_tmp106;struct Cyc_List_List*
_tmp107;_LLEB: if(_tmp103 <= (void*)3?1:*((int*)_tmp103)!= 10)goto _LLED;_tmp104=((
struct Cyc_Absyn_AggrType_struct*)_tmp103)->f1;_tmp105=(void*)_tmp104.aggr_info;
if(*((int*)_tmp105)!= 1)goto _LLED;_tmp106=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp105)->f1;_LLEC: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp106 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp106,_tmpEA);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLEA;}_LLED: if(_tmp103 <= (void*)3?1:*((int*)_tmp103)!= 11)goto _LLEF;
_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp103)->f2;_LLEE: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp107,_tmpEA);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLEA;}_LLEF:;_LLF0: goto _LLEA;_LLEA:;}goto _LLE5;_LLE8:;_LLE9: goto _LLE5;_LLE5:;}
goto _LLB9;_LLC8: if(*((int*)_tmpDC)!= 20)goto _LLCA;_tmpEB=((struct Cyc_Absyn_Deref_e_struct*)
_tmpDC)->f1;_LLC9:{void*_tmp108=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEB->topt))->v);struct Cyc_Absyn_PtrInfo _tmp109;struct Cyc_Absyn_Tqual
_tmp10A;struct Cyc_Absyn_Tqual _tmp10B;_LLF2: if(_tmp108 <= (void*)3?1:*((int*)
_tmp108)!= 4)goto _LLF4;_tmp109=((struct Cyc_Absyn_PointerType_struct*)_tmp108)->f1;
_tmp10A=_tmp109.tq;_LLF3: _tmp10B=_tmp10A;goto _LLF5;_LLF4: if(_tmp108 <= (void*)3?1:*((
int*)_tmp108)!= 7)goto _LLF6;_tmp10B=((struct Cyc_Absyn_ArrayType_struct*)_tmp108)->f2;
_LLF5: if(!_tmp10B.q_const)return;goto _LLF1;_LLF6:;_LLF7: goto _LLF1;_LLF1:;}goto
_LLB9;_LLCA: if(*((int*)_tmpDC)!= 11)goto _LLCC;_tmpEC=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpDC)->f1;_LLCB: _tmpED=_tmpEC;goto _LLCD;_LLCC: if(*((int*)_tmpDC)!= 12)goto
_LLCE;_tmpED=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpDC)->f1;_LLCD: Cyc_Tcexp_check_writable(
te,_tmpED);return;_LLCE:;_LLCF: goto _LLB9;_LLB9:;}({struct Cyc_Std_String_pa_struct
_tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp10C[1]={& _tmp10D};Cyc_Tcutil_terr(e->loc,_tag_arr("attempt to write a const location: %s",
sizeof(char),38),_tag_arr(_tmp10C,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp10E[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(char),34),_tag_arr(_tmp10E,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt)){if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp10F[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("can't perform arithmetic on abstract pointer type",sizeof(char),50),
_tag_arr(_tmp10F,sizeof(void*),0));});}else{({struct Cyc_Std_String_pa_struct
_tmp111;_tmp111.tag=0;_tmp111.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp110[1]={& _tmp111};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting arithmetic or ? type but found %s",
sizeof(char),44),_tag_arr(_tmp110,sizeof(void*),1));}});}}return t;}}static void*
Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(char),23));Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp119;}));struct Cyc_List_List _tmp112=({struct
Cyc_List_List _tmp118;_tmp118.hd=e3;_tmp118.tl=0;_tmp118;});struct Cyc_List_List
_tmp113=({struct Cyc_List_List _tmp117;_tmp117.hd=e2;_tmp117.tl=(struct Cyc_List_List*)&
_tmp112;_tmp117;});if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp113)){({struct Cyc_Std_String_pa_struct _tmp116;_tmp116.tag=0;_tmp116.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v);{struct Cyc_Std_String_pa_struct _tmp115;_tmp115.tag=0;
_tmp115.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmp114[2]={& _tmp115,& _tmp116};Cyc_Tcutil_terr(
loc,_tag_arr("conditional clause types do not match: %s != %s",sizeof(char),48),
_tag_arr(_tmp114,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp11A=Cyc_Tcutil_compress(t1);_LLF9: if(_tmp11A <= (void*)3?1:*((int*)
_tmp11A)!= 7)goto _LLFB;_LLFA:({void*_tmp11B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",
sizeof(char),26),_tag_arr(_tmp11B,sizeof(void*),0));});goto _LLF8;_LLFB:;_LLFC:
goto _LLF8;_LLF8:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1))({
void*_tmp11C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(char),41),_tag_arr(_tmp11C,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e1))return({void*_tmp11D[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(char),25),_tag_arr(_tmp11D,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp11E=({struct
Cyc_Std_String_pa_struct _tmp121;_tmp121.tag=0;_tmp121.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp120;_tmp120.tag=0;_tmp120.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp11F[2]={& _tmp120,& _tmp121};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type mismatch: %s != %s",sizeof(char),24),_tag_arr(_tmp11F,
sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp11E;}}else{void*_tmp122=(void*)po->v;void*_tmp123=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp122,e1,e2);if(!(Cyc_Tcutil_unify(_tmp123,t1)?1:(Cyc_Tcutil_coerceable(
_tmp123)?Cyc_Tcutil_coerceable(t1): 0))){void*_tmp124=({struct Cyc_Std_String_pa_struct
_tmp127;_tmp127.tag=0;_tmp127.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp125[2]={& _tmp126,& _tmp127};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(char),82),_tag_arr(_tmp125,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp123,t1);Cyc_Tcutil_explain_failure();return _tmp124;}return _tmp123;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct dbl={6,0};static
void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;{void*_tmp128=Cyc_Tcutil_compress(t1);void*_tmp129;void*
_tmp12A;_LLFE: if((int)_tmp128 != 1)goto _LL100;_LLFF: Cyc_Tcutil_unchecked_cast(te,
e,dbl_typ);t1=dbl_typ;goto _LLFD;_LL100: if(_tmp128 <= (void*)3?1:*((int*)_tmp128)
!= 5)goto _LL102;_tmp129=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp128)->f2;
if((int)_tmp129 != 0)goto _LL102;_LL101: goto _LL103;_LL102: if(_tmp128 <= (void*)3?1:*((
int*)_tmp128)!= 5)goto _LL104;_tmp12A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp128)->f2;if((int)_tmp12A != 1)goto _LL104;_LL103: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ);t1=Cyc_Absyn_sint_typ;goto _LLFD;_LL104:;_LL105: goto _LLFD;
_LLFD:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp12C;struct
Cyc_List_List*_tmp12D;struct Cyc_Position_Segment*_tmp12E;void*_tmp12F;struct Cyc_Absyn_Tunionfield
_tmp12B=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp12C=_tmp12B.name;_tmp12D=
_tmp12B.typs;_tmp12E=_tmp12B.loc;_tmp12F=(void*)_tmp12B.sc;if(_tmp12D == 0?1:
_tmp12D->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple8*)_tmp12D->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Position_Segment*
_tmp133;void*_tmp134;struct Cyc_Absyn_Tunionfield _tmp130=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp131=_tmp130.name;_tmp132=_tmp130.typs;_tmp133=_tmp130.loc;
_tmp134=(void*)_tmp130.sc;if(_tmp132 == 0?1: _tmp132->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple8*)_tmp132->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))
return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}({
struct Cyc_Std_String_pa_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{struct Cyc_Std_String_pa_struct _tmp136;
_tmp136.tag=0;_tmp136.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(tu);{void*
_tmp135[2]={& _tmp136,& _tmp137};Cyc_Tcutil_terr(e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",
sizeof(char),54),_tag_arr(_tmp135,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp139=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp13A=t;struct Cyc_Absyn_PtrInfo _tmp13B;void*_tmp13C;void*_tmp13D;struct
Cyc_Absyn_Conref*_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;struct Cyc_Absyn_Conref*
_tmp140;_LL107: if(_tmp13A <= (void*)3?1:*((int*)_tmp13A)!= 4)goto _LL109;_tmp13B=((
struct Cyc_Absyn_PointerType_struct*)_tmp13A)->f1;_tmp13C=(void*)_tmp13B.elt_typ;
_tmp13D=(void*)_tmp13B.rgn_typ;_tmp13E=_tmp13B.nullable;_tmp13F=_tmp13B.tq;
_tmp140=_tmp13B.bounds;_LL108: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp13D);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp140);{void*_tmp141=Cyc_Tcutil_compress(_tmp13C);struct Cyc_Absyn_FnInfo
_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_Core_Opt*_tmp144;void*_tmp145;
struct Cyc_List_List*_tmp146;int _tmp147;struct Cyc_Absyn_VarargInfo*_tmp148;struct
Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp14A;_LL10C: if(_tmp141 <= (void*)3?1:*((
int*)_tmp141)!= 8)goto _LL10E;_tmp142=((struct Cyc_Absyn_FnType_struct*)_tmp141)->f1;
_tmp143=_tmp142.tvars;_tmp144=_tmp142.effect;_tmp145=(void*)_tmp142.ret_typ;
_tmp146=_tmp142.args;_tmp147=_tmp142.c_varargs;_tmp148=_tmp142.cyc_varargs;
_tmp149=_tmp142.rgn_po;_tmp14A=_tmp142.attributes;_LL10D: if(topt != 0)Cyc_Tcutil_unify(
_tmp145,*topt);while(_tmp139 != 0?_tmp146 != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp139->hd;void*t2=(*((struct _tuple2*)_tmp146->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp14B[2]={& _tmp14C,& _tmp14D};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(char),54),_tag_arr(_tmp14B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp139=_tmp139->tl;_tmp146=_tmp146->tl;}if(_tmp146 != 0)({void*_tmp14E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too few arguments for function",sizeof(char),31),_tag_arr(_tmp14E,
sizeof(void*),0));});else{if((_tmp139 != 0?1: _tmp147)?1: _tmp148 != 0){if(_tmp147)
for(0;_tmp139 != 0;_tmp139=_tmp139->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp139->hd);}else{if(_tmp148 == 0)({void*_tmp14F[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("too many arguments for function",sizeof(char),32),_tag_arr(_tmp14F,
sizeof(void*),0));});else{void*_tmp151;int _tmp152;struct Cyc_Absyn_VarargInfo
_tmp150=*_tmp148;_tmp151=(void*)_tmp150.type;_tmp152=_tmp150.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp153=({struct Cyc_Absyn_VarargCallInfo*_tmp167=_cycalloc(sizeof(*_tmp167));
_tmp167->num_varargs=0;_tmp167->injectors=0;_tmp167->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp148);_tmp167;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp153;if(!_tmp152)for(0;_tmp139 != 0;_tmp139=_tmp139->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp139->hd;_tmp153->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp151,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp151)){({struct Cyc_Std_String_pa_struct
_tmp156;_tmp156.tag=0;_tmp156.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp155;_tmp155.tag=0;_tmp155.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp151);{void*_tmp154[2]={& _tmp155,& _tmp156};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(char),49),_tag_arr(_tmp154,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp157=Cyc_Tcutil_compress(_tmp151);struct Cyc_Absyn_TunionInfo _tmp158;
void*_tmp159;struct Cyc_Absyn_Tuniondecl**_tmp15A;struct Cyc_Absyn_Tuniondecl*
_tmp15B;struct Cyc_List_List*_tmp15C;void*_tmp15D;_LL111: if(_tmp157 <= (void*)3?1:*((
int*)_tmp157)!= 2)goto _LL113;_tmp158=((struct Cyc_Absyn_TunionType_struct*)
_tmp157)->f1;_tmp159=(void*)_tmp158.tunion_info;if(*((int*)_tmp159)!= 1)goto
_LL113;_tmp15A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp159)->f1;_tmp15B=*
_tmp15A;_tmp15C=_tmp158.targs;_tmp15D=(void*)_tmp158.rgn;_LL112: {struct Cyc_Absyn_Tuniondecl*
_tmp15E=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp15B->name);struct Cyc_List_List*
fields=0;if(_tmp15E->fields == 0)({struct Cyc_Std_String_pa_struct _tmp160;_tmp160.tag=
0;_tmp160.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp151);{void*_tmp15F[1]={&
_tmp160};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",sizeof(char),21),
_tag_arr(_tmp15F,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp15E->fields))->v;}if(!Cyc_Tcutil_unify(
_tmp15D,Cyc_Tcenv_curr_rgn(te)))({void*_tmp161[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bad region for injected varargs",sizeof(char),32),_tag_arr(_tmp161,
sizeof(void*),0));});{struct _RegionHandle _tmp162=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp162;_push_region(rgn);{struct Cyc_List_List*_tmp163=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp15E->tvs,_tmp15C);for(0;
_tmp139 != 0;_tmp139=_tmp139->tl){_tmp153->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp139->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp164=Cyc_Tcexp_tcInjection(te,e1,_tmp151,rgn,_tmp163,fields);if(_tmp164 != 0)
_tmp153->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp153->injectors,({struct Cyc_List_List*_tmp165=_cycalloc(
sizeof(*_tmp165));_tmp165->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp164);
_tmp165->tl=0;_tmp165;}));}}}};_pop_region(rgn);}goto _LL110;}_LL113:;_LL114:({
void*_tmp166[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
char),23),_tag_arr(_tmp166,sizeof(void*),0));});goto _LL110;_LL110:;}}}}}}{struct
Cyc_List_List*a=_tmp14A;for(0;a != 0;a=a->tl){void*_tmp168=(void*)a->hd;void*
_tmp169;int _tmp16A;int _tmp16B;_LL116: if(_tmp168 <= (void*)16?1:*((int*)_tmp168)!= 
3)goto _LL118;_tmp169=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp168)->f1;
_tmp16A=((struct Cyc_Absyn_Format_att_struct*)_tmp168)->f2;_tmp16B=((struct Cyc_Absyn_Format_att_struct*)
_tmp168)->f3;_LL117:{struct _handler_cons _tmp16C;_push_handler(& _tmp16C);{int
_tmp16E=0;if(setjmp(_tmp16C.handler))_tmp16E=1;if(!_tmp16E){{struct Cyc_Absyn_Exp*
_tmp16F=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp16A - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp16B == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp16B - 1);_tmp170;});}{
void*_tmp171=_tmp169;_LL11B: if((int)_tmp171 != 0)goto _LL11D;_LL11C: Cyc_Tcexp_check_format_args(
te,_tmp16F,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL11A;_LL11D: if((int)
_tmp171 != 1)goto _LL11A;_LL11E: Cyc_Tcexp_check_format_args(te,_tmp16F,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL11A;_LL11A:;}};_pop_handler();}else{void*
_tmp16D=(void*)_exn_thrown;void*_tmp173=_tmp16D;_LL120: if(_tmp173 != Cyc_List_Nth)
goto _LL122;_LL121:({void*_tmp174[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",
sizeof(char),21),_tag_arr(_tmp174,sizeof(void*),0));});goto _LL11F;_LL122:;_LL123:(
void)_throw(_tmp173);_LL11F:;}}}goto _LL115;_LL118:;_LL119: goto _LL115;_LL115:;}}
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp144))->v);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp149);
return _tmp145;_LL10E:;_LL10F: return({void*_tmp175[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("expected pointer to function",sizeof(char),29),_tag_arr(_tmp175,
sizeof(void*),0));});_LL10B:;}_LL109:;_LL10A: return({void*_tmp176[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(char),29),_tag_arr(_tmp176,
sizeof(void*),0));});_LL106:;}}static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({
struct Cyc_Std_String_pa_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp177[1]={& _tmp178};Cyc_Tcutil_terr(loc,_tag_arr("expected xtunion exn but found %s",
sizeof(char),34),_tag_arr(_tmp177,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp179;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp17A=t1;struct Cyc_Absyn_PtrInfo _tmp17B;void*_tmp17C;void*_tmp17D;struct
Cyc_Absyn_Conref*_tmp17E;struct Cyc_Absyn_Tqual _tmp17F;struct Cyc_Absyn_Conref*
_tmp180;_LL125: if(_tmp17A <= (void*)3?1:*((int*)_tmp17A)!= 4)goto _LL127;_tmp17B=((
struct Cyc_Absyn_PointerType_struct*)_tmp17A)->f1;_tmp17C=(void*)_tmp17B.elt_typ;
_tmp17D=(void*)_tmp17B.rgn_typ;_tmp17E=_tmp17B.nullable;_tmp17F=_tmp17B.tq;
_tmp180=_tmp17B.bounds;_LL126:{void*_tmp181=Cyc_Tcutil_compress(_tmp17C);struct
Cyc_Absyn_FnInfo _tmp182;struct Cyc_List_List*_tmp183;struct Cyc_Core_Opt*_tmp184;
void*_tmp185;struct Cyc_List_List*_tmp186;int _tmp187;struct Cyc_Absyn_VarargInfo*
_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18A;_LL12A: if(
_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 8)goto _LL12C;_tmp182=((struct Cyc_Absyn_FnType_struct*)
_tmp181)->f1;_tmp183=_tmp182.tvars;_tmp184=_tmp182.effect;_tmp185=(void*)_tmp182.ret_typ;
_tmp186=_tmp182.args;_tmp187=_tmp182.c_varargs;_tmp188=_tmp182.cyc_varargs;
_tmp189=_tmp182.rgn_po;_tmp18A=_tmp182.attributes;_LL12B: {struct Cyc_List_List*
instantiation=0;for(0;ts != 0?_tmp183 != 0: 0;(ts=ts->tl,_tmp183=_tmp183->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp183->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=({struct _tuple5*
_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->f1=(struct Cyc_Absyn_Tvar*)_tmp183->hd;
_tmp18C->f2=(void*)ts->hd;_tmp18C;});_tmp18B->tl=instantiation;_tmp18B;});}if(ts
!= 0)return({void*_tmp18D[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too many type variables in instantiation",
sizeof(char),41),_tag_arr(_tmp18D,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp191=_cycalloc(sizeof(*
_tmp191));_tmp191[0]=({struct Cyc_Absyn_FnType_struct _tmp192;_tmp192.tag=8;
_tmp192.f1=({struct Cyc_Absyn_FnInfo _tmp193;_tmp193.tvars=_tmp183;_tmp193.effect=
_tmp184;_tmp193.ret_typ=(void*)_tmp185;_tmp193.args=_tmp186;_tmp193.c_varargs=
_tmp187;_tmp193.cyc_varargs=_tmp188;_tmp193.rgn_po=_tmp189;_tmp193.attributes=
_tmp18A;_tmp193;});_tmp192;});_tmp191;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp18F;_tmp18F.tag=4;_tmp18F.f1=({struct Cyc_Absyn_PtrInfo _tmp190;_tmp190.elt_typ=(
void*)new_fn_typ;_tmp190.rgn_typ=(void*)_tmp17D;_tmp190.nullable=_tmp17E;_tmp190.tq=
_tmp17F;_tmp190.bounds=_tmp180;_tmp190;});_tmp18F;});_tmp18E;});}}_LL12C:;_LL12D:
goto _LL129;_LL129:;}goto _LL124;_LL127:;_LL128: goto _LL124;_LL124:;}return({struct
Cyc_Std_String_pa_struct _tmp195;_tmp195.tag=0;_tmp195.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp194[1]={& _tmp195};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(char),40),_tag_arr(_tmp194,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(te,loc,t2,t)?!Cyc_Tcutil_castable(
te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp196=({struct Cyc_Std_String_pa_struct _tmp199;_tmp199.tag=0;_tmp199.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp198;
_tmp198.tag=0;_tmp198.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*
_tmp197[2]={& _tmp198,& _tmp199};Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",
sizeof(char),21),_tag_arr(_tmp197,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp196;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp19A=0;struct Cyc_Absyn_Tqual _tmp19B=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp19C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp19D;
void*_tmp19E;struct Cyc_Absyn_Tqual _tmp19F;_LL12F: if(_tmp19C <= (void*)3?1:*((int*)
_tmp19C)!= 4)goto _LL131;_tmp19D=((struct Cyc_Absyn_PointerType_struct*)_tmp19C)->f1;
_tmp19E=(void*)_tmp19D.elt_typ;_tmp19F=_tmp19D.tq;_LL130: _tmp19A=({void**_tmp1A0=
_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=_tmp19E;_tmp1A0;});_tmp19B=_tmp19F;goto
_LL12E;_LL131:;_LL132: goto _LL12E;_LL12E:;}Cyc_Tcexp_tcExpNoInst(te,_tmp19A,e);{
void*_tmp1A1=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;
_LL134: if(*((int*)_tmp1A1)!= 28)goto _LL136;_LL135: goto _LL137;_LL136: if(*((int*)
_tmp1A1)!= 24)goto _LL138;_LL137:({void*_tmp1A4[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("& used to allocate",sizeof(char),19),_tag_arr(_tmp1A4,sizeof(void*),0));});{
void*(*_tmp1A5)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;if(
topt != 0){void*_tmp1A6=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1A7;
struct Cyc_Absyn_Conref*_tmp1A8;_LL13D: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 
4)goto _LL13F;_tmp1A7=((struct Cyc_Absyn_PointerType_struct*)_tmp1A6)->f1;_tmp1A8=
_tmp1A7.nullable;_LL13E: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp1A8))_tmp1A5=Cyc_Absyn_star_typ;goto _LL13C;_LL13F:;_LL140: goto _LL13C;_LL13C:;}
return _tmp1A5((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp19B);}_LL138: if(*((int*)_tmp1A1)!= 23)goto _LL13A;_tmp1A2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1A1)->f1;_tmp1A3=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A1)->f2;_LL139:{
void*_tmp1A9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1A2->topt))->v);_LL142: if(_tmp1A9 <= (void*)3?1:*((int*)_tmp1A9)!= 9)goto
_LL144;_LL143: goto _LL141;_LL144:;_LL145:(void*)(e0->r=(void*)((void*)(Cyc_Absyn_add_exp(
_tmp1A2,_tmp1A3,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp1A2,_tmp1A3);_LL141:;}
goto _LL133;_LL13A:;_LL13B: goto _LL133;_LL133:;}{int _tmp1AB;void*_tmp1AC;struct
_tuple6 _tmp1AA=Cyc_Tcutil_addressof_props(te,e);_tmp1AB=_tmp1AA.f1;_tmp1AC=
_tmp1AA.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp1AB)tq.q_const=
1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp1AC,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_Std_String_pa_struct _tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1AD[1]={& _tmp1AE};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
char),55),_tag_arr(_tmp1AD,sizeof(void*),1));}});if(topt != 0){void*_tmp1AF=Cyc_Tcutil_compress(*
topt);_LL147: if(_tmp1AF <= (void*)3?1:*((int*)_tmp1AF)!= 14)goto _LL149;_LL148:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1B0=_cycalloc(sizeof(*
_tmp1B0));_tmp1B0[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1B1;_tmp1B1.tag=14;
_tmp1B1.f1=(void*)t;_tmp1B1;});_tmp1B0;});_LL149:;_LL14A: goto _LL146;_LL146:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_Std_strcmp(*n,*sf->name)== 0;}static void*
Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);{void*_tmp1B2=n;struct _tagged_arr*_tmp1B3;unsigned int _tmp1B4;
_LL14C: if(*((int*)_tmp1B2)!= 0)goto _LL14E;_tmp1B3=((struct Cyc_Absyn_StructField_struct*)
_tmp1B2)->f1;_LL14D: {int bad_type=1;{void*_tmp1B5=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1B6;void*_tmp1B7;struct Cyc_Absyn_Aggrdecl**_tmp1B8;struct
Cyc_List_List*_tmp1B9;_LL151: if(_tmp1B5 <= (void*)3?1:*((int*)_tmp1B5)!= 10)goto
_LL153;_tmp1B6=((struct Cyc_Absyn_AggrType_struct*)_tmp1B5)->f1;_tmp1B7=(void*)
_tmp1B6.aggr_info;if(*((int*)_tmp1B7)!= 1)goto _LL153;_tmp1B8=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1B7)->f1;_LL152: if((*_tmp1B8)->impl == 0)goto _LL150;if(!((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp1B3,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp1B8)->impl))->fields))({struct Cyc_Std_String_pa_struct _tmp1BB;
_tmp1BB.tag=0;_tmp1BB.f1=(struct _tagged_arr)*_tmp1B3;{void*_tmp1BA[1]={& _tmp1BB};
Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",sizeof(char),
37),_tag_arr(_tmp1BA,sizeof(void*),1));}});bad_type=0;goto _LL150;_LL153: if(
_tmp1B5 <= (void*)3?1:*((int*)_tmp1B5)!= 11)goto _LL155;_tmp1B9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B5)->f2;_LL154: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1B3,_tmp1B9))({struct Cyc_Std_String_pa_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(
struct _tagged_arr)*_tmp1B3;{void*_tmp1BC[1]={& _tmp1BD};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct/union has name %s",sizeof(char),37),_tag_arr(
_tmp1BC,sizeof(void*),1));}});bad_type=0;goto _LL150;_LL155:;_LL156: goto _LL150;
_LL150:;}if(bad_type)({struct Cyc_Std_String_pa_struct _tmp1BF;_tmp1BF.tag=0;
_tmp1BF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1BE[1]={&
_tmp1BF};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(char),36),_tag_arr(_tmp1BE,sizeof(void*),1));}});goto _LL14B;}_LL14E: if(*((
int*)_tmp1B2)!= 1)goto _LL14B;_tmp1B4=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp1B2)->f1;_LL14F: {int bad_type=1;{void*_tmp1C0=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Aggrdecl**_tmp1C3;struct
Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_TunionFieldInfo
_tmp1C6;void*_tmp1C7;struct Cyc_Absyn_Tunionfield*_tmp1C8;_LL158: if(_tmp1C0 <= (
void*)3?1:*((int*)_tmp1C0)!= 10)goto _LL15A;_tmp1C1=((struct Cyc_Absyn_AggrType_struct*)
_tmp1C0)->f1;_tmp1C2=(void*)_tmp1C1.aggr_info;if(*((int*)_tmp1C2)!= 1)goto _LL15A;
_tmp1C3=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1C2)->f1;_LL159: if((*_tmp1C3)->impl
== 0)goto _LL157;_tmp1C4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp1C3)->impl))->fields;
goto _LL15B;_LL15A: if(_tmp1C0 <= (void*)3?1:*((int*)_tmp1C0)!= 11)goto _LL15C;
_tmp1C4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C0)->f2;_LL15B: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1C4)<= _tmp1B4)({struct Cyc_Std_Int_pa_struct
_tmp1CB;_tmp1CB.tag=1;_tmp1CB.f1=(int)_tmp1B4;{struct Cyc_Std_Int_pa_struct
_tmp1CA;_tmp1CA.tag=1;_tmp1CA.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1C4));{void*_tmp1C9[2]={& _tmp1CA,& _tmp1CB};Cyc_Tcutil_terr(
loc,_tag_arr("struct/union has too few components: %d <= %d",sizeof(char),46),
_tag_arr(_tmp1C9,sizeof(void*),2));}}});bad_type=0;goto _LL157;_LL15C: if(_tmp1C0
<= (void*)3?1:*((int*)_tmp1C0)!= 9)goto _LL15E;_tmp1C5=((struct Cyc_Absyn_TupleType_struct*)
_tmp1C0)->f1;_LL15D: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C5)
<= _tmp1B4)({struct Cyc_Std_Int_pa_struct _tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=(int)
_tmp1B4;{struct Cyc_Std_Int_pa_struct _tmp1CD;_tmp1CD.tag=1;_tmp1CD.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C5));{void*
_tmp1CC[2]={& _tmp1CD,& _tmp1CE};Cyc_Tcutil_terr(loc,_tag_arr("tuple has too few components: %d <= %d",
sizeof(char),39),_tag_arr(_tmp1CC,sizeof(void*),2));}}});bad_type=0;goto _LL157;
_LL15E: if(_tmp1C0 <= (void*)3?1:*((int*)_tmp1C0)!= 3)goto _LL160;_tmp1C6=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp1C0)->f1;_tmp1C7=(void*)_tmp1C6.field_info;
if(*((int*)_tmp1C7)!= 1)goto _LL160;_tmp1C8=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1C7)->f2;_LL15F: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C8->typs)
< _tmp1B4)({struct Cyc_Std_Int_pa_struct _tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=(int)
_tmp1B4;{struct Cyc_Std_Int_pa_struct _tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C8->typs));{
void*_tmp1CF[2]={& _tmp1D0,& _tmp1D1};Cyc_Tcutil_terr(loc,_tag_arr("tunionfield has too few components: %d < %d",
sizeof(char),44),_tag_arr(_tmp1CF,sizeof(void*),2));}}});bad_type=0;goto _LL157;
_LL160:;_LL161: goto _LL157;_LL157:;}if(bad_type)({struct Cyc_Std_String_pa_struct
_tmp1D3;_tmp1D3.tag=0;_tmp1D3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1D2[1]={& _tmp1D3};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known type",
sizeof(char),23),_tag_arr(_tmp1D2,sizeof(void*),1));}});goto _LL14B;}_LL14B:;}
return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1D4=t;struct Cyc_Absyn_PtrInfo _tmp1D5;void*_tmp1D6;void*_tmp1D7;struct
Cyc_Absyn_Conref*_tmp1D8;_LL163: if(_tmp1D4 <= (void*)3?1:*((int*)_tmp1D4)!= 4)
goto _LL165;_tmp1D5=((struct Cyc_Absyn_PointerType_struct*)_tmp1D4)->f1;_tmp1D6=(
void*)_tmp1D5.elt_typ;_tmp1D7=(void*)_tmp1D5.rgn_typ;_tmp1D8=_tmp1D5.bounds;
_LL164: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1D7);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1D8);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1D6),(void*)1))({
void*_tmp1D9[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(char),40),_tag_arr(_tmp1D9,sizeof(void*),0));});return _tmp1D6;_LL165:;
_LL166: return({struct Cyc_Std_String_pa_struct _tmp1DB;_tmp1DB.tag=0;_tmp1DB.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1DA[1]={& _tmp1DB};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting * or @ type but found %s",sizeof(char),35),_tag_arr(
_tmp1DA,sizeof(void*),1));}});_LL162:;}}static void*Cyc_Tcexp_tcAggrMember(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp1DC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp1DD;void*_tmp1DE;struct Cyc_Absyn_Aggrdecl**_tmp1DF;
struct Cyc_Absyn_Aggrdecl*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*
_tmp1E2;_LL168: if(_tmp1DC <= (void*)3?1:*((int*)_tmp1DC)!= 10)goto _LL16A;_tmp1DD=((
struct Cyc_Absyn_AggrType_struct*)_tmp1DC)->f1;_tmp1DE=(void*)_tmp1DD.aggr_info;
if(*((int*)_tmp1DE)!= 1)goto _LL16A;_tmp1DF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1DE)->f1;_tmp1E0=*_tmp1DF;_tmp1E1=_tmp1DD.targs;_LL169: {struct Cyc_Absyn_Aggrfield*
_tmp1E3=Cyc_Absyn_lookup_decl_field(_tmp1E0,f);if(_tmp1E3 == 0)return({struct Cyc_Std_String_pa_struct
_tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1E0->name);{void*_tmp1E4[2]={& _tmp1E5,& _tmp1E6};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("type %s has no %s field",sizeof(char),24),_tag_arr(_tmp1E4,sizeof(void*),
2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E0->impl))->exist_vars
!= 0)return({void*_tmp1E7[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(char),64),_tag_arr(_tmp1E7,sizeof(void*),0));});{void*t2;{struct
_RegionHandle _tmp1E8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1E8;
_push_region(rgn);{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1E0->tvs,_tmp1E1);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp1E9,(void*)_tmp1E3->type);};_pop_region(rgn);}return t2;}}_LL16A: if(_tmp1DC <= (
void*)3?1:*((int*)_tmp1DC)!= 11)goto _LL16C;_tmp1E2=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1DC)->f2;_LL16B: {struct Cyc_Absyn_Aggrfield*_tmp1EA=Cyc_Absyn_lookup_field(
_tmp1E2,f);if(_tmp1EA == 0)return({struct Cyc_Std_String_pa_struct _tmp1EC;_tmp1EC.tag=
0;_tmp1EC.f1=(struct _tagged_arr)*f;{void*_tmp1EB[1]={& _tmp1EC};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(char),21),_tag_arr(_tmp1EB,sizeof(
void*),1));}});return(void*)_tmp1EA->type;}_LL16C: if(_tmp1DC <= (void*)3?1:*((int*)
_tmp1DC)!= 7)goto _LL16E;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(char),5))
== 0))goto _LL16E;_LL16D: goto _LL16F;_LL16E: if(_tmp1DC <= (void*)3?1:*((int*)
_tmp1DC)!= 4)goto _LL170;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(char),5))
== 0))goto _LL170;_LL16F:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp1EE;_tmp1EE.tag=3;_tmp1EE.f1=(void*)((void*)19);_tmp1EE.f2=({struct Cyc_List_List*
_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=e;_tmp1EF->tl=0;_tmp1EF;});
_tmp1EE;});_tmp1ED;})));return Cyc_Absyn_uint_typ;_LL170:;_LL171: if(Cyc_Std_zstrcmp(*
f,_tag_arr("size",sizeof(char),5))== 0)return({struct Cyc_Std_String_pa_struct
_tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F0[1]={& _tmp1F1};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct, union, or array, found %s",sizeof(char),44),
_tag_arr(_tmp1F0,sizeof(void*),1));}});else{return({struct Cyc_Std_String_pa_struct
_tmp1F3;_tmp1F3.tag=0;_tmp1F3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F2[1]={& _tmp1F3};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct or union, found %s",sizeof(char),36),_tag_arr(
_tmp1F2,sizeof(void*),1));}});}_LL167:;}}static void*Cyc_Tcexp_tcAggrArrow(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp1F4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1F5;void*_tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Conref*_tmp1F8;_LL173: if(
_tmp1F4 <= (void*)3?1:*((int*)_tmp1F4)!= 4)goto _LL175;_tmp1F5=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F4)->f1;_tmp1F6=(void*)_tmp1F5.elt_typ;_tmp1F7=(void*)_tmp1F5.rgn_typ;
_tmp1F8=_tmp1F5.bounds;_LL174: Cyc_Tcutil_check_nonzero_bound(loc,_tmp1F8);{void*
_tmp1F9=Cyc_Tcutil_compress(_tmp1F6);struct Cyc_Absyn_AggrInfo _tmp1FA;void*
_tmp1FB;struct Cyc_Absyn_Aggrdecl**_tmp1FC;struct Cyc_Absyn_Aggrdecl*_tmp1FD;
struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FF;_LL178: if(_tmp1F9 <= (
void*)3?1:*((int*)_tmp1F9)!= 10)goto _LL17A;_tmp1FA=((struct Cyc_Absyn_AggrType_struct*)
_tmp1F9)->f1;_tmp1FB=(void*)_tmp1FA.aggr_info;if(*((int*)_tmp1FB)!= 1)goto _LL17A;
_tmp1FC=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1FB)->f1;_tmp1FD=*_tmp1FC;
_tmp1FE=_tmp1FA.targs;_LL179: {struct Cyc_Absyn_Aggrfield*_tmp200=Cyc_Absyn_lookup_decl_field(
_tmp1FD,f);if(_tmp200 == 0)return({struct Cyc_Std_String_pa_struct _tmp203;_tmp203.tag=
0;_tmp203.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp202;
_tmp202.tag=0;_tmp202.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1FD->name);{
void*_tmp201[2]={& _tmp202,& _tmp203};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(char),24),_tag_arr(_tmp201,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1FD->impl))->exist_vars != 0)return({void*_tmp204[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(char),64),_tag_arr(_tmp204,sizeof(void*),0));});{void*t3;{struct
_RegionHandle _tmp205=_new_region("rgn");struct _RegionHandle*rgn=& _tmp205;
_push_region(rgn);{struct Cyc_List_List*_tmp206=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1FD->tvs,_tmp1FE);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp206,(void*)_tmp200->type);};_pop_region(rgn);}return t3;}}_LL17A: if(_tmp1F9 <= (
void*)3?1:*((int*)_tmp1F9)!= 11)goto _LL17C;_tmp1FF=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F9)->f2;_LL17B: {struct Cyc_Absyn_Aggrfield*_tmp207=Cyc_Absyn_lookup_field(
_tmp1FF,f);if(_tmp207 == 0)return({struct Cyc_Std_String_pa_struct _tmp209;_tmp209.tag=
0;_tmp209.f1=(struct _tagged_arr)*f;{void*_tmp208[1]={& _tmp209};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(char),21),_tag_arr(_tmp208,sizeof(
void*),1));}});return(void*)_tmp207->type;}_LL17C:;_LL17D: goto _LL177;_LL177:;}
goto _LL172;_LL175:;_LL176: goto _LL172;_LL172:;}return({struct Cyc_Std_String_pa_struct
_tmp20B;_tmp20B.tag=0;_tmp20B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp20A[1]={& _tmp20B};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(char),35),_tag_arr(
_tmp20A,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(index);struct _handler_cons
_tmp20C;_push_handler(& _tmp20C);{int _tmp20E=0;if(setjmp(_tmp20C.handler))_tmp20E=
1;if(!_tmp20E){{void*_tmp20F=(*((struct _tuple8*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return _tmp20F;};_pop_handler();}
else{void*_tmp20D=(void*)_exn_thrown;void*_tmp211=_tmp20D;_LL17F: if(_tmp211 != 
Cyc_List_Nth)goto _LL181;_LL180: return({struct Cyc_Std_Int_pa_struct _tmp214;
_tmp214.tag=1;_tmp214.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp213;_tmp213.tag=1;_tmp213.f1=(int)i;{void*
_tmp212[2]={& _tmp213,& _tmp214};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(char),41),_tag_arr(_tmp212,sizeof(void*),2));}}});_LL181:;_LL182:(void)
_throw(_tmp211);_LL17E:;}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))return({struct Cyc_Std_String_pa_struct _tmp216;_tmp216.tag=0;_tmp216.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp215[1]={& _tmp216};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(char),34),
_tag_arr(_tmp215,sizeof(void*),1));}});{void*_tmp217=t1;struct Cyc_Absyn_PtrInfo
_tmp218;void*_tmp219;void*_tmp21A;struct Cyc_Absyn_Tqual _tmp21B;struct Cyc_Absyn_Conref*
_tmp21C;struct Cyc_List_List*_tmp21D;_LL184: if(_tmp217 <= (void*)3?1:*((int*)
_tmp217)!= 4)goto _LL186;_tmp218=((struct Cyc_Absyn_PointerType_struct*)_tmp217)->f1;
_tmp219=(void*)_tmp218.elt_typ;_tmp21A=(void*)_tmp218.rgn_typ;_tmp21B=_tmp218.tq;
_tmp21C=_tmp218.bounds;_LL185: if(Cyc_Tcutil_is_const_exp(te,e2))Cyc_Tcutil_check_bound(
loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp21C);else{if(Cyc_Tcutil_is_bound_one(
_tmp21C))({void*_tmp21E[0]={};Cyc_Tcutil_warn(e1->loc,_tag_arr("subscript applied to pointer to one object",
sizeof(char),43),_tag_arr(_tmp21E,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp21C);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp21A);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp219),(void*)1))({void*_tmp21F[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(char),36),_tag_arr(_tmp21F,
sizeof(void*),0));});return _tmp219;_LL186: if(_tmp217 <= (void*)3?1:*((int*)
_tmp217)!= 9)goto _LL188;_tmp21D=((struct Cyc_Absyn_TupleType_struct*)_tmp217)->f1;
_LL187: return Cyc_Tcexp_ithTupleType(te,loc,_tmp21D,e2);_LL188:;_LL189: return({
struct Cyc_Std_String_pa_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp220[1]={& _tmp221};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("subscript applied to %s",sizeof(char),24),_tag_arr(_tmp220,
sizeof(void*),1));}});_LL183:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;
struct Cyc_List_List*fields=0;if(topt != 0){void*_tmp222=Cyc_Tcutil_compress(*topt);
struct Cyc_List_List*_tmp223;_LL18B: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 9)
goto _LL18D;_tmp223=((struct Cyc_Absyn_TupleType_struct*)_tmp222)->f1;_LL18C: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp223)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL18A;for(0;es != 0;(es=es->tl,_tmp223=_tmp223->tl)){
void*_tmp224=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp223))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp224,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=({
struct _tuple8*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226->f1=(*((struct _tuple8*)
_tmp223->hd)).f1;_tmp226->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp226;});_tmp225->tl=fields;_tmp225;});}done=
1;goto _LL18A;_LL18D:;_LL18E: goto _LL18A;_LL18A:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->hd=({struct _tuple8*_tmp228=
_cycalloc(sizeof(*_tmp228));_tmp228->f1=Cyc_Absyn_empty_tqual();_tmp228->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp228;});_tmp227->tl=fields;_tmp227;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229[0]=({struct Cyc_Absyn_TupleType_struct
_tmp22A;_tmp22A.tag=9;_tmp22A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp22A;});_tmp229;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp22B[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(char),14),_tag_arr(_tmp22B,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp23C=_cycalloc(
sizeof(*_tmp23C));_tmp23C->v=Cyc_Tcenv_lookup_type_vars(te);_tmp23C;}));struct
Cyc_Absyn_Const_e_struct*_tmp22C=({struct Cyc_Absyn_Const_e_struct*_tmp238=
_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Absyn_Const_e_struct _tmp239;
_tmp239.tag=0;_tmp239.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp23A=
_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=({struct Cyc_Absyn_Int_c_struct _tmp23B;
_tmp23B.tag=2;_tmp23B.f1=(void*)((void*)1);_tmp23B.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp23B;});_tmp23A;}));_tmp239;});_tmp238;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp22C,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->v=(void*)Cyc_Absyn_uint_typ;_tmp22D;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp236=_cycalloc(sizeof(*
_tmp236));_tmp236[0]=({struct Cyc_Absyn_ArrayType_struct _tmp237;_tmp237.tag=7;
_tmp237.f1=(void*)res;_tmp237.f2=Cyc_Absyn_empty_tqual();_tmp237.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp237;});_tmp236;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_Std_String_pa_struct _tmp22F;_tmp22F.tag=0;_tmp22F.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp22E[1]={& _tmp22F};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(char),53),_tag_arr(_tmp22E,sizeof(void*),1));}});{int offset=0;for(0;des != 
0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)des->hd)).f1;
if(ds != 0){void*_tmp230=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp231;_LL190: if(*((
int*)_tmp230)!= 1)goto _LL192;_LL191:({void*_tmp232[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("only array index designators are supported",sizeof(char),43),_tag_arr(
_tmp232,sizeof(void*),0));});goto _LL18F;_LL192: if(*((int*)_tmp230)!= 0)goto
_LL18F;_tmp231=((struct Cyc_Absyn_ArrayElement_struct*)_tmp230)->f1;_LL193: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp231);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp231);if(i != 
offset)({struct Cyc_Std_Int_pa_struct _tmp235;_tmp235.tag=1;_tmp235.f1=(int)i;{
struct Cyc_Std_Int_pa_struct _tmp234;_tmp234.tag=1;_tmp234.f1=(int)((unsigned int)
offset);{void*_tmp233[2]={& _tmp234,& _tmp235};Cyc_Tcutil_terr(_tmp231->loc,
_tag_arr("expecting index designator %d but found %d",sizeof(char),43),_tag_arr(
_tmp233,sizeof(void*),2));}}});goto _LL18F;}_LL18F:;}}}return res_t2;}}static void*
Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(te,bound))({
struct Cyc_Std_String_pa_struct _tmp23E;_tmp23E.tag=0;_tmp23E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
bound->topt))->v);{void*_tmp23D[1]={& _tmp23E};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(char),33),_tag_arr(_tmp23D,
sizeof(void*),1));}});if(!(vd->tq).q_const)({void*_tmp23F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(char),52),_tag_arr(_tmp23F,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(
loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;
void**_tmp240=0;struct Cyc_Absyn_Tqual*_tmp241=0;if(topt != 0){void*_tmp242=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp243;void*_tmp244;void**_tmp245;struct Cyc_Absyn_Tqual
_tmp246;struct Cyc_Absyn_Tqual*_tmp247;struct Cyc_Absyn_Exp*_tmp248;_LL195: if(
_tmp242 <= (void*)3?1:*((int*)_tmp242)!= 4)goto _LL197;_tmp243=((struct Cyc_Absyn_PointerType_struct*)
_tmp242)->f1;_LL196: pinfo=_tmp243;_tmp240=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp241=(struct Cyc_Absyn_Tqual*)& pinfo.tq;goto _LL194;_LL197: if(_tmp242 <= (void*)
3?1:*((int*)_tmp242)!= 7)goto _LL199;_tmp244=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp242)->f1;_tmp245=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp242)->f1);_tmp246=((struct Cyc_Absyn_ArrayType_struct*)_tmp242)->f2;_tmp247=(
struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_struct*)_tmp242)->f2;_tmp248=((
struct Cyc_Absyn_ArrayType_struct*)_tmp242)->f3;_LL198: _tmp240=(void**)_tmp245;
_tmp241=(struct Cyc_Absyn_Tqual*)_tmp247;goto _LL194;_LL199:;_LL19A: goto _LL194;
_LL194:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp240,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp249[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(char),22),_tag_arr(_tmp249,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(
te,body))({void*_tmp24A[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(char),21),_tag_arr(_tmp24A,sizeof(void*),0));});}return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp24C;_tmp24C.tag=7;_tmp24C.f1=(void*)t;_tmp24C.f2=_tmp241 == 0?Cyc_Absyn_empty_tqual():*
_tmp241;_tmp24C.f3=(struct Cyc_Absyn_Exp*)bound;_tmp24C;});_tmp24B;});}}}struct
_tuple9{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp24D;_push_handler(& _tmp24D);{
int _tmp24F=0;if(setjmp(_tmp24D.handler))_tmp24F=1;if(!_tmp24F){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp24E=(void*)_exn_thrown;void*_tmp251=
_tmp24E;_LL19C: if(_tmp251 != Cyc_Dict_Absent)goto _LL19E;_LL19D:({struct Cyc_Std_String_pa_struct
_tmp253;_tmp253.tag=0;_tmp253.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp252[1]={& _tmp253};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(char),23),_tag_arr(_tmp252,sizeof(void*),1));}});return topt != 0?*topt:(
void*)0;_LL19E:;_LL19F:(void)_throw(_tmp251);_LL19B:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp254[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("expecting struct but found union",sizeof(char),33),_tag_arr(
_tmp254,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp255[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't build abstract struct",sizeof(char),28),_tag_arr(_tmp255,
sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp256=
_cycalloc(sizeof(*_tmp256));_tmp256->v=Cyc_Tcenv_lookup_type_vars(te);_tmp256;}));}{
struct _RegionHandle _tmp257=_new_region("rgn");struct _RegionHandle*rgn=& _tmp257;
_push_region(rgn);{struct _tuple4 _tmp258=({struct _tuple4 _tmp273;_tmp273.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp273.f2=rgn;_tmp273;});struct Cyc_List_List*_tmp259=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp258,ad->tvs);struct Cyc_List_List*_tmp25A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp258,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp25B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp259);struct Cyc_List_List*_tmp25C=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp25A);struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp259,_tmp25A);struct Cyc_Absyn_AggrType_struct*_tmp25E=({struct Cyc_Absyn_AggrType_struct*
_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Absyn_AggrType_struct
_tmp26E;_tmp26E.tag=10;_tmp26E.f1=({struct Cyc_Absyn_AggrInfo _tmp26F;_tmp26F.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp270=_cycalloc(sizeof(*
_tmp270));_tmp270[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp271;_tmp271.tag=1;
_tmp271.f1=({struct Cyc_Absyn_Aggrdecl**_tmp272=_cycalloc(sizeof(*_tmp272));
_tmp272[0]=ad;_tmp272;});_tmp271;});_tmp270;}));_tmp26F.targs=_tmp25B;_tmp26F;});
_tmp26E;});_tmp26D;});struct Cyc_List_List*_tmp25F=*ts;struct Cyc_List_List*
_tmp260=_tmp25C;while(_tmp25F != 0?_tmp260 != 0: 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp25F->hd);Cyc_Tcutil_unify((
void*)_tmp25F->hd,(void*)_tmp260->hd);_tmp25F=_tmp25F->tl;_tmp260=_tmp260->tl;}
if(_tmp25F != 0)({void*_tmp261[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many explicit witness types",
sizeof(char),32),_tag_arr(_tmp261,sizeof(void*),0));});*ts=_tmp25C;if(topt != 0)
Cyc_Tcutil_unify((void*)_tmp25E,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple9 _tmp263;struct Cyc_Absyn_Aggrfield*_tmp264;struct
Cyc_Absyn_Exp*_tmp265;struct _tuple9*_tmp262=(struct _tuple9*)fields->hd;_tmp263=*
_tmp262;_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;{void*_tmp266=Cyc_Tcutil_rsubstitute(
rgn,_tmp25D,(void*)_tmp264->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp266,
_tmp265);if(!Cyc_Tcutil_coerce_arg(te,_tmp265,_tmp266)){({struct Cyc_Std_String_pa_struct
_tmp26B;_tmp26B.tag=0;_tmp26B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp265->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp26A;_tmp26A.tag=0;_tmp26A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp266);{struct Cyc_Std_String_pa_struct _tmp269;_tmp269.tag=0;_tmp269.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(*tn);{struct Cyc_Std_String_pa_struct _tmp268;
_tmp268.tag=0;_tmp268.f1=(struct _tagged_arr)*_tmp264->name;{void*_tmp267[4]={&
_tmp268,& _tmp269,& _tmp26A,& _tmp26B};Cyc_Tcutil_terr(_tmp265->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(char),44),_tag_arr(_tmp267,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
void*_tmp26C=(void*)_tmp25E;_npop_handler(0);return _tmp26C;}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp274=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp274;_push_region(rgn);{void*_tmp275=Cyc_Tcutil_compress(
ts);void*_tmp276;struct Cyc_List_List*_tmp277;_LL1A1: if(_tmp275 <= (void*)3?1:*((
int*)_tmp275)!= 11)goto _LL1A3;_tmp276=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp275)->f1;_tmp277=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp275)->f2;_LL1A2:
if(_tmp276 == (void*)1)({void*_tmp278[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expecting struct but found union",
sizeof(char),33),_tag_arr(_tmp278,sizeof(void*),0));});{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp277);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp27A;
struct Cyc_Absyn_Aggrfield*_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;struct _tuple9*
_tmp279=(struct _tuple9*)fields->hd;_tmp27A=*_tmp279;_tmp27B=_tmp27A.f1;_tmp27C=
_tmp27A.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp27B->type)),
_tmp27C);if(!Cyc_Tcutil_coerce_arg(te,_tmp27C,(void*)_tmp27B->type)){({struct Cyc_Std_String_pa_struct
_tmp280;_tmp280.tag=0;_tmp280.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp27C->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp27F;_tmp27F.tag=0;_tmp27F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp27B->type);{struct Cyc_Std_String_pa_struct _tmp27E;_tmp27E.tag=0;_tmp27E.f1=(
struct _tagged_arr)*_tmp27B->name;{void*_tmp27D[3]={& _tmp27E,& _tmp27F,& _tmp280};
Cyc_Tcutil_terr(_tmp27C->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(char),41),_tag_arr(_tmp27D,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL1A0;}_LL1A3:;_LL1A4:({void*_tmp281[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct: wrong type",
sizeof(char),25),_tag_arr(_tmp281,sizeof(void*),0));});_LL1A0:;};_pop_region(rgn);}
return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp282=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp282;_push_region(rgn);{struct _tuple4 _tmp283=({
struct _tuple4 _tmp2A2;_tmp2A2.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp2A2.f2=rgn;
_tmp2A2;});struct Cyc_List_List*_tmp284=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp283,tud->tvs);struct Cyc_List_List*_tmp285=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp284);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp29E;_tmp29E.tag=3;_tmp29E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp29F;
_tmp29F.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp2A1;_tmp2A1.tag=1;_tmp2A1.f1=tud;_tmp2A1.f2=tuf;_tmp2A1;});_tmp2A0;}));
_tmp29F.targs=_tmp285;_tmp29F;});_tmp29E;});_tmp29D;});if(topt != 0){void*_tmp286=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp287;struct Cyc_List_List*
_tmp288;void*_tmp289;_LL1A6: if(_tmp286 <= (void*)3?1:*((int*)_tmp286)!= 3)goto
_LL1A8;_LL1A7: Cyc_Tcutil_unify(*topt,res);goto _LL1A5;_LL1A8: if(_tmp286 <= (void*)
3?1:*((int*)_tmp286)!= 2)goto _LL1AA;_tmp287=((struct Cyc_Absyn_TunionType_struct*)
_tmp286)->f1;_tmp288=_tmp287.targs;_tmp289=(void*)_tmp287.rgn;_LL1A9:{struct Cyc_List_List*
a=_tmp285;for(0;a != 0?_tmp288 != 0: 0;(a=a->tl,_tmp288=_tmp288->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp288->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A->v=(void*)res;_tmp28A;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B[0]=({
struct Cyc_Absyn_TunionType_struct _tmp28C;_tmp28C.tag=2;_tmp28C.f1=({struct Cyc_Absyn_TunionInfo
_tmp28D;_tmp28D.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp28F;_tmp28F.tag=1;_tmp28F.f1=({struct Cyc_Absyn_Tuniondecl**_tmp290=_cycalloc(
sizeof(*_tmp290));_tmp290[0]=tud;_tmp290;});_tmp28F;});_tmp28E;}));_tmp28D.targs=
_tmp285;_tmp28D.rgn=(void*)_tmp289;_tmp28D;});_tmp28C;});_tmp28B;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp291=res;_npop_handler(0);return _tmp291;}}goto _LL1A5;_LL1AA:;
_LL1AB: goto _LL1A5;_LL1A5:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp284,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmp295;_tmp295.tag=0;_tmp295.f1=(struct _tagged_arr)(e->topt == 0?_tag_arr("?",
sizeof(char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp294;_tmp294.tag=0;_tmp294.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp293;_tmp293.tag=0;_tmp293.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp292[3]={& _tmp293,& _tmp294,& _tmp295};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(char),80),_tag_arr(_tmp292,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp298=({struct Cyc_Std_String_pa_struct _tmp297;_tmp297.tag=0;
_tmp297.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp296[1]={&
_tmp297};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(char),45),_tag_arr(_tmp296,sizeof(void*),1));}});_npop_handler(0);return
_tmp298;}if(ts != 0){void*_tmp29B=({struct Cyc_Std_String_pa_struct _tmp29A;_tmp29A.tag=
0;_tmp29A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp299[
1]={& _tmp29A};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(char),44),_tag_arr(_tmp299,sizeof(void*),1));}});_npop_handler(0);return
_tmp29B;}{void*_tmp29C=res;_npop_handler(0);return _tmp29C;}}};_pop_region(rgn);}
static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp2A3=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp2A4;struct Cyc_Absyn_Conref*_tmp2A5;void*_tmp2A6;
struct Cyc_List_List*_tmp2A7;struct Cyc_Absyn_AggrInfo _tmp2A8;void*_tmp2A9;struct
Cyc_List_List*_tmp2AA;struct Cyc_List_List*_tmp2AB;_LL1AD: if((int)_tmp2A3 != 0)
goto _LL1AF;_LL1AE: return 1;_LL1AF: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 0)
goto _LL1B1;_LL1B0: goto _LL1B2;_LL1B1: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 1)
goto _LL1B3;_LL1B2: goto _LL1B4;_LL1B3: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 2)
goto _LL1B5;_LL1B4: goto _LL1B6;_LL1B5: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 3)
goto _LL1B7;_LL1B6: return 0;_LL1B7: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 4)
goto _LL1B9;_tmp2A4=((struct Cyc_Absyn_PointerType_struct*)_tmp2A3)->f1;_tmp2A5=
_tmp2A4.nullable;_LL1B8: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp2A5);_LL1B9: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 5)goto _LL1BB;_LL1BA:
goto _LL1BC;_LL1BB: if((int)_tmp2A3 != 1)goto _LL1BD;_LL1BC: goto _LL1BE;_LL1BD: if(
_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 6)goto _LL1BF;_LL1BE: return 1;_LL1BF: if(
_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 7)goto _LL1C1;_tmp2A6=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp2A3)->f1;_LL1C0: return Cyc_Tcexp_zeroable_type(_tmp2A6);_LL1C1: if(_tmp2A3 <= (
void*)3?1:*((int*)_tmp2A3)!= 8)goto _LL1C3;_LL1C2: return 0;_LL1C3: if(_tmp2A3 <= (
void*)3?1:*((int*)_tmp2A3)!= 9)goto _LL1C5;_tmp2A7=((struct Cyc_Absyn_TupleType_struct*)
_tmp2A3)->f1;_LL1C4: for(0;(unsigned int)_tmp2A7;_tmp2A7=_tmp2A7->tl){if(!Cyc_Tcexp_zeroable_type((*((
struct _tuple8*)_tmp2A7->hd)).f2))return 0;}return 1;_LL1C5: if(_tmp2A3 <= (void*)3?1:*((
int*)_tmp2A3)!= 10)goto _LL1C7;_tmp2A8=((struct Cyc_Absyn_AggrType_struct*)_tmp2A3)->f1;
_tmp2A9=(void*)_tmp2A8.aggr_info;_tmp2AA=_tmp2A8.targs;_LL1C6: {struct Cyc_Absyn_Aggrdecl*
_tmp2AC=Cyc_Absyn_get_known_aggrdecl(_tmp2A9);if(_tmp2AC->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2AC->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp2AD=
_new_region("r");struct _RegionHandle*r=& _tmp2AD;_push_region(r);{struct Cyc_List_List*
_tmp2AE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp2AC->tvs,
_tmp2AA);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp2AC->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(
r,_tmp2AE,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp2AF=0;
_npop_handler(0);return _tmp2AF;}}}{int _tmp2B0=1;_npop_handler(0);return _tmp2B0;}};
_pop_region(r);}}_LL1C7: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 12)goto _LL1C9;
_LL1C8: return 1;_LL1C9: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 14)goto _LL1CB;
_LL1CA: return 1;_LL1CB: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 17)goto _LL1CD;
_LL1CC: return 1;_LL1CD: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 11)goto _LL1CF;
_tmp2AB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2A3)->f2;_LL1CE: for(0;_tmp2AB
!= 0;_tmp2AB=_tmp2AB->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp2AB->hd)->type))return 0;}return 1;_LL1CF: if(_tmp2A3 <= (void*)3?1:*((int*)
_tmp2A3)!= 13)goto _LL1D1;_LL1D0: return 1;_LL1D1: if(_tmp2A3 <= (void*)3?1:*((int*)
_tmp2A3)!= 16)goto _LL1D3;_LL1D2: return 0;_LL1D3: if(_tmp2A3 <= (void*)3?1:*((int*)
_tmp2A3)!= 15)goto _LL1D5;_LL1D4: return 0;_LL1D5: if(_tmp2A3 <= (void*)3?1:*((int*)
_tmp2A3)!= 18)goto _LL1D7;_LL1D6: goto _LL1D8;_LL1D7: if((int)_tmp2A3 != 2)goto _LL1D9;
_LL1D8: goto _LL1DA;_LL1D9: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 19)goto
_LL1DB;_LL1DA: goto _LL1DC;_LL1DB: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 20)
goto _LL1DD;_LL1DC: goto _LL1DE;_LL1DD: if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 
21)goto _LL1AC;_LL1DE:({struct Cyc_Std_String_pa_struct _tmp2B2;_tmp2B2.tag=0;
_tmp2B2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp2B1[1]={&
_tmp2B2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad type `%s' in zeroable type",sizeof(char),31),_tag_arr(_tmp2B1,
sizeof(void*),1));}});_LL1AC:;}static void Cyc_Tcexp_check_malloc_type(int
allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;if(topt != 0){void*_tmp2B3=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2B4;void*_tmp2B5;_LL1E0:
if(_tmp2B3 <= (void*)3?1:*((int*)_tmp2B3)!= 4)goto _LL1E2;_tmp2B4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B3)->f1;_tmp2B5=(void*)_tmp2B4.elt_typ;_LL1E1: Cyc_Tcutil_unify(_tmp2B5,t);
if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;
goto _LL1DF;_LL1E2:;_LL1E3: goto _LL1DF;_LL1DF:;}({struct Cyc_Std_String_pa_struct
_tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2B7;_tmp2B7.tag=0;_tmp2B7.f1=(struct
_tagged_arr)(allow_zero?_tag_arr("calloc",sizeof(char),7): _tag_arr("malloc",
sizeof(char),7));{void*_tmp2B6[2]={& _tmp2B7,& _tmp2B8};Cyc_Tcutil_terr(loc,
_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",sizeof(
char),60),_tag_arr(_tmp2B6,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2BE;_tmp2BE.tag=15;_tmp2BE.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2BF;}));_tmp2BE;});_tmp2BD;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2B9=Cyc_Tcutil_compress(handle_type);void*_tmp2BA;
_LL1E5: if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 15)goto _LL1E7;_tmp2BA=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2B9)->f1;_LL1E6: rgn=_tmp2BA;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1E4;_LL1E7:;_LL1E8:({struct Cyc_Std_String_pa_struct _tmp2BC;
_tmp2BC.tag=0;_tmp2BC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{
void*_tmp2BB[1]={& _tmp2BC};Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(char),37),_tag_arr(_tmp2BB,sizeof(void*),1));}});goto _LL1E4;_LL1E4:;}Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;
int one_elt;if(is_calloc){if(*t == 0)({void*_tmp2C0[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(char),23),_tag_arr(_tmp2C0,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp2C1=(void*)(*e)->r;void*_tmp2C2;void*_tmp2C3;struct Cyc_List_List*
_tmp2C4;struct Cyc_List_List _tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_List_List*
_tmp2C7;struct Cyc_List_List _tmp2C8;struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_List_List*
_tmp2CA;_LL1EA: if(*((int*)_tmp2C1)!= 16)goto _LL1EC;_tmp2C2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2C1)->f1;_LL1EB: elt_type=_tmp2C2;*t=(void**)({void**_tmp2CB=_cycalloc(
sizeof(*_tmp2CB));_tmp2CB[0]=elt_type;_tmp2CB;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL1E9;
_LL1EC: if(*((int*)_tmp2C1)!= 3)goto _LL1EE;_tmp2C3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2C1)->f1;if((int)_tmp2C3 != 1)goto _LL1EE;_tmp2C4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2C1)->f2;if(_tmp2C4 == 0)goto _LL1EE;_tmp2C5=*_tmp2C4;_tmp2C6=(struct Cyc_Absyn_Exp*)
_tmp2C5.hd;_tmp2C7=_tmp2C5.tl;if(_tmp2C7 == 0)goto _LL1EE;_tmp2C8=*_tmp2C7;_tmp2C9=(
struct Cyc_Absyn_Exp*)_tmp2C8.hd;_tmp2CA=_tmp2C8.tl;if(_tmp2CA != 0)goto _LL1EE;
_LL1ED:{struct _tuple0 _tmp2CD=({struct _tuple0 _tmp2CC;_tmp2CC.f1=(void*)_tmp2C6->r;
_tmp2CC.f2=(void*)_tmp2C9->r;_tmp2CC;});void*_tmp2CE;void*_tmp2CF;void*_tmp2D0;
void*_tmp2D1;_LL1F1: _tmp2CE=_tmp2CD.f1;if(*((int*)_tmp2CE)!= 16)goto _LL1F3;
_tmp2CF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2CE)->f1;_LL1F2: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2CF);elt_type=_tmp2CF;*t=(void**)({void**_tmp2D2=_cycalloc(
sizeof(*_tmp2D2));_tmp2D2[0]=elt_type;_tmp2D2;});num_elts=_tmp2C9;one_elt=0;goto
_LL1F0;_LL1F3: _tmp2D0=_tmp2CD.f2;if(*((int*)_tmp2D0)!= 16)goto _LL1F5;_tmp2D1=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2D0)->f1;_LL1F4: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2D1);elt_type=_tmp2D1;*t=(void**)({void**_tmp2D3=_cycalloc(
sizeof(*_tmp2D3));_tmp2D3[0]=elt_type;_tmp2D3;});num_elts=_tmp2C6;one_elt=0;goto
_LL1F0;_LL1F5:;_LL1F6: goto No_sizeof;_LL1F0:;}goto _LL1E9;_LL1EE:;_LL1EF: No_sizeof:
elt_type=Cyc_Absyn_uchar_typ;*t=(void**)({void**_tmp2D4=_cycalloc(sizeof(*
_tmp2D4));_tmp2D4[0]=elt_type;_tmp2D4;});num_elts=*e;one_elt=0;goto _LL1E9;_LL1E9:;}*
e=num_elts;*is_fat=!one_elt;{void*_tmp2D5=elt_type;struct Cyc_Absyn_AggrInfo
_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Aggrdecl**_tmp2D8;struct Cyc_Absyn_Aggrdecl*
_tmp2D9;_LL1F8: if(_tmp2D5 <= (void*)3?1:*((int*)_tmp2D5)!= 10)goto _LL1FA;_tmp2D6=((
struct Cyc_Absyn_AggrType_struct*)_tmp2D5)->f1;_tmp2D7=(void*)_tmp2D6.aggr_info;
if(*((int*)_tmp2D7)!= 1)goto _LL1FA;_tmp2D8=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp2D7)->f1;_tmp2D9=*_tmp2D8;_LL1F9: if(_tmp2D9->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2D9->impl))->exist_vars != 0: 0)({void*_tmp2DA[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("malloc with existential types not yet implemented",sizeof(char),50),
_tag_arr(_tmp2DA,sizeof(void*),0));});goto _LL1F7;_LL1FA:;_LL1FB: goto _LL1F7;
_LL1F7:;}{void*(*_tmp2DB)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;
if(topt != 0){void*_tmp2DC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2DD;struct Cyc_Absyn_Conref*_tmp2DE;_LL1FD: if(_tmp2DC <= (void*)3?1:*((int*)
_tmp2DC)!= 4)goto _LL1FF;_tmp2DD=((struct Cyc_Absyn_PointerType_struct*)_tmp2DC)->f1;
_tmp2DE=_tmp2DD.nullable;_LL1FE: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp2DE))_tmp2DB=Cyc_Absyn_star_typ;goto _LL1FC;_LL1FF:;_LL200: goto _LL1FC;_LL1FC:;}
if(!one_elt)_tmp2DB=Cyc_Absyn_tagged_typ;return _tmp2DB(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp2DF=(void*)s->r;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Stmt*
_tmp2E1;struct Cyc_Absyn_Stmt*_tmp2E2;struct Cyc_Absyn_Decl*_tmp2E3;struct Cyc_Absyn_Stmt*
_tmp2E4;_LL202: if(_tmp2DF <= (void*)1?1:*((int*)_tmp2DF)!= 0)goto _LL204;_tmp2E0=((
struct Cyc_Absyn_Exp_s_struct*)_tmp2DF)->f1;_LL203: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E0->topt))->v;_LL204: if(_tmp2DF <= (void*)1?1:*((int*)_tmp2DF)!= 
1)goto _LL206;_tmp2E1=((struct Cyc_Absyn_Seq_s_struct*)_tmp2DF)->f1;_tmp2E2=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2DF)->f2;_LL205: s=_tmp2E2;continue;_LL206: if(
_tmp2DF <= (void*)1?1:*((int*)_tmp2DF)!= 12)goto _LL208;_tmp2E3=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2DF)->f1;_tmp2E4=((struct Cyc_Absyn_Decl_s_struct*)_tmp2DF)->f2;_LL207: s=
_tmp2E4;continue;_LL208:;_LL209: return({void*_tmp2E5[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("statement expression must end with expression",sizeof(char),46),
_tag_arr(_tmp2E5,sizeof(void*),0));});_LL201:;}}static void*Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*
fd){return({void*_tmp2E6[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcCodegen",
sizeof(char),10),_tag_arr(_tmp2E6,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){return({void*_tmp2E7[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcFill",
sizeof(char),7),_tag_arr(_tmp2E7,sizeof(void*),0));});}static void*Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(
rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2ED;_tmp2ED.tag=15;_tmp2ED.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2EE;}));_tmp2ED;});_tmp2EC;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp2E8=Cyc_Tcutil_compress(handle_type);void*
_tmp2E9;_LL20B: if(_tmp2E8 <= (void*)3?1:*((int*)_tmp2E8)!= 15)goto _LL20D;_tmp2E9=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2E8)->f1;_LL20C: rgn=_tmp2E9;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL20A;_LL20D:;_LL20E:({struct Cyc_Std_String_pa_struct
_tmp2EB;_tmp2EB.tag=0;_tmp2EB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type);{void*_tmp2EA[1]={& _tmp2EB};Cyc_Tcutil_terr(rgn_handle->loc,
_tag_arr("expecting region_t type but found %s",sizeof(char),37),_tag_arr(
_tmp2EA,sizeof(void*),1));}});goto _LL20A;_LL20A:;}{void*_tmp2EF=(void*)e1->r;
struct Cyc_Absyn_Vardecl*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*
_tmp2F2;struct Cyc_Core_Opt*_tmp2F3;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*
_tmp2F5;void*_tmp2F6;struct _tagged_arr _tmp2F7;_LL210: if(*((int*)_tmp2EF)!= 27)
goto _LL212;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2EF)->f1;
_tmp2F1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2EF)->f2;_tmp2F2=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2EF)->f3;_LL211: {void*_tmp2F8=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp2F9=Cyc_Tcutil_compress(_tmp2F8);void*_tmp2FA;struct Cyc_Absyn_Tqual
_tmp2FB;_LL21B: if(_tmp2F9 <= (void*)3?1:*((int*)_tmp2F9)!= 7)goto _LL21D;_tmp2FA=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp2F9)->f1;_tmp2FB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2F9)->f2;_LL21C: {void*b=Cyc_Tcutil_is_const_exp(te,_tmp2F1)?(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp301;_tmp301.tag=0;_tmp301.f1=_tmp2F1;_tmp301;});
_tmp300;}):(void*)0;void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD[0]=({struct Cyc_Absyn_PointerType_struct
_tmp2FE;_tmp2FE.tag=4;_tmp2FE.f1=({struct Cyc_Absyn_PtrInfo _tmp2FF;_tmp2FF.elt_typ=(
void*)_tmp2FA;_tmp2FF.rgn_typ=(void*)rgn;_tmp2FF.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp2FF.tq=_tmp2FB;_tmp2FF.bounds=Cyc_Absyn_new_conref(
b);_tmp2FF;});_tmp2FE;});_tmp2FD;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,
res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->v=(void*)res_typ;_tmp2FC;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}_LL21D:;_LL21E:({void*_tmp302[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcNew: comprehension returned non-array type",
sizeof(char),45),_tag_arr(_tmp302,sizeof(void*),0));});_LL21A:;}_LL212: if(*((int*)
_tmp2EF)!= 34)goto _LL214;_tmp2F3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp2EF)->f1;_tmp2F4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2EF)->f2;
_LL213:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp303=
_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_Absyn_Array_e_struct _tmp304;
_tmp304.tag=26;_tmp304.f1=_tmp2F4;_tmp304;});_tmp303;})));_tmp2F5=_tmp2F4;goto
_LL215;_LL214: if(*((int*)_tmp2EF)!= 26)goto _LL216;_tmp2F5=((struct Cyc_Absyn_Array_e_struct*)
_tmp2EF)->f1;_LL215: {void**elt_typ_opt=0;if(topt != 0){void*_tmp305=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp306;void*_tmp307;void**_tmp308;struct Cyc_Absyn_Tqual
_tmp309;_LL220: if(_tmp305 <= (void*)3?1:*((int*)_tmp305)!= 4)goto _LL222;_tmp306=((
struct Cyc_Absyn_PointerType_struct*)_tmp305)->f1;_tmp307=(void*)_tmp306.elt_typ;
_tmp308=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp305)->f1).elt_typ;
_tmp309=_tmp306.tq;_LL221: elt_typ_opt=(void**)_tmp308;goto _LL21F;_LL222:;_LL223:
goto _LL21F;_LL21F:;}{void*_tmp30A=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);
void*res_typ;{void*_tmp30B=Cyc_Tcutil_compress(_tmp30A);void*_tmp30C;struct Cyc_Absyn_Tqual
_tmp30D;struct Cyc_Absyn_Exp*_tmp30E;_LL225: if(_tmp30B <= (void*)3?1:*((int*)
_tmp30B)!= 7)goto _LL227;_tmp30C=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp30B)->f1;_tmp30D=((struct Cyc_Absyn_ArrayType_struct*)_tmp30B)->f2;_tmp30E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp30B)->f3;_LL226: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp310;_tmp310.tag=4;_tmp310.f1=({struct Cyc_Absyn_PtrInfo _tmp311;_tmp311.elt_typ=(
void*)_tmp30C;_tmp311.rgn_typ=(void*)rgn;_tmp311.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp311.tq=_tmp30D;_tmp311.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp312=_cycalloc(sizeof(*_tmp312));
_tmp312[0]=({struct Cyc_Absyn_Upper_b_struct _tmp313;_tmp313.tag=0;_tmp313.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp30E);_tmp313;});_tmp312;}));_tmp311;});
_tmp310;});_tmp30F;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp314=_cycalloc(sizeof(*
_tmp314));_tmp314->v=(void*)res_typ;_tmp314;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL224;_LL227:;_LL228:({void*_tmp315[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(char),50),_tag_arr(_tmp315,sizeof(void*),0));});_LL224:;}return res_typ;}}
_LL216: if(*((int*)_tmp2EF)!= 0)goto _LL218;_tmp2F6=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2EF)->f1;if(_tmp2F6 <= (void*)1?1:*((int*)_tmp2F6)!= 5)goto _LL218;_tmp2F7=((
struct Cyc_Absyn_String_c_struct*)_tmp2F6)->f1;_LL217: {void*_tmp316=Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_typ,rgn,Cyc_Absyn_const_tqual(),(void*)0);void*_tmp317=Cyc_Tcexp_tcExp(
te,(void**)& _tmp316,e1);return Cyc_Absyn_atb_typ(_tmp317,rgn,Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp318=_cycalloc(sizeof(*_tmp318));
_tmp318[0]=({struct Cyc_Absyn_Upper_b_struct _tmp319;_tmp319.tag=0;_tmp319.f1=Cyc_Absyn_uint_exp(
1,0);_tmp319;});_tmp318;}));}_LL218:;_LL219: {void**topt2=0;if(topt != 0){void*
_tmp31A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp31B;void*_tmp31C;
void**_tmp31D;struct Cyc_Absyn_Conref*_tmp31E;_LL22A: if(_tmp31A <= (void*)3?1:*((
int*)_tmp31A)!= 4)goto _LL22C;_tmp31B=((struct Cyc_Absyn_PointerType_struct*)
_tmp31A)->f1;_tmp31C=(void*)_tmp31B.elt_typ;_tmp31D=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp31A)->f1).elt_typ;_tmp31E=_tmp31B.nullable;_LL22B: topt2=(void**)_tmp31D;goto
_LL229;_LL22C: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 2)goto _LL22E;_LL22D:
topt2=({void**_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F[0]=*topt;_tmp31F;});
goto _LL229;_LL22E:;_LL22F: goto _LL229;_LL229:;}{void*telt=Cyc_Tcexp_tcExp(te,
topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp321=
_cycalloc(sizeof(*_tmp321));_tmp321[0]=({struct Cyc_Absyn_PointerType_struct
_tmp322;_tmp322.tag=4;_tmp322.f1=({struct Cyc_Absyn_PtrInfo _tmp323;_tmp323.elt_typ=(
void*)telt;_tmp323.rgn_typ=(void*)rgn;_tmp323.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp323.tq=Cyc_Absyn_empty_tqual();_tmp323.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp324=_cycalloc(sizeof(*_tmp324));
_tmp324[0]=({struct Cyc_Absyn_Upper_b_struct _tmp325;_tmp325.tag=0;_tmp325.f1=Cyc_Absyn_uint_exp(
1,0);_tmp325;});_tmp324;}));_tmp323;});_tmp322;});_tmp321;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320->v=(
void*)res_typ;_tmp320;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL20F:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp326=t;void*_tmp327;struct Cyc_Absyn_Tqual _tmp328;struct Cyc_Absyn_Exp*
_tmp329;_LL231: if(_tmp326 <= (void*)3?1:*((int*)_tmp326)!= 7)goto _LL233;_tmp327=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp326)->f1;_tmp328=((struct Cyc_Absyn_ArrayType_struct*)
_tmp326)->f2;_tmp329=((struct Cyc_Absyn_ArrayType_struct*)_tmp326)->f3;_LL232: {
void*_tmp32B;struct _tuple6 _tmp32A=Cyc_Tcutil_addressof_props(te,e);_tmp32B=
_tmp32A.f2;{void*_tmp32C=_tmp329 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp32E;_tmp32E.tag=0;_tmp32E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp329);
_tmp32E;});_tmp32D;});t=Cyc_Absyn_atb_typ(_tmp327,_tmp32B,_tmp328,_tmp32C);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL233:;_LL234:
return t;_LL230:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*
_tmp32F=(void*)e->r;void*_tmp330;_LL236: if(*((int*)_tmp32F)!= 26)goto _LL238;
_LL237: goto _LL239;_LL238: if(*((int*)_tmp32F)!= 27)goto _LL23A;_LL239: goto _LL23B;
_LL23A: if(*((int*)_tmp32F)!= 0)goto _LL23C;_tmp330=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp32F)->f1;if(_tmp330 <= (void*)1?1:*((int*)_tmp330)!= 5)goto _LL23C;_LL23B:
return t;_LL23C:;_LL23D: t=Cyc_Tcutil_compress(t);{void*_tmp331=t;void*_tmp332;
struct Cyc_Absyn_Tqual _tmp333;struct Cyc_Absyn_Exp*_tmp334;_LL23F: if(_tmp331 <= (
void*)3?1:*((int*)_tmp331)!= 7)goto _LL241;_tmp332=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp331)->f1;_tmp333=((struct Cyc_Absyn_ArrayType_struct*)_tmp331)->f2;_tmp334=((
struct Cyc_Absyn_ArrayType_struct*)_tmp331)->f3;_LL240: {void*_tmp336;struct
_tuple6 _tmp335=Cyc_Tcutil_addressof_props(te,e);_tmp336=_tmp335.f2;{void*b=
_tmp334 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp337=_cycalloc(
sizeof(*_tmp337));_tmp337[0]=({struct Cyc_Absyn_Upper_b_struct _tmp338;_tmp338.tag=
0;_tmp338.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp334);_tmp338;});_tmp337;});t=
Cyc_Absyn_atb_typ(_tmp332,_tmp336,_tmp333,b);Cyc_Tcutil_unchecked_cast(te,e,t);
return t;}}_LL241:;_LL242: return t;_LL23E:;}_LL235:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp339=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp33A;_LL244: if(*((int*)_tmp339)!= 11)goto _LL246;_tmp33A=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp339)->f1;_LL245: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp33A);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp33A->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp33A->topt))->v));
e->topt=_tmp33A->topt;goto _LL243;_LL246:;_LL247: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp33B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp33C;void*_tmp33D;void*_tmp33E;struct Cyc_Absyn_Conref*_tmp33F;struct Cyc_Absyn_Tqual
_tmp340;struct Cyc_Absyn_Conref*_tmp341;_LL249: if(_tmp33B <= (void*)3?1:*((int*)
_tmp33B)!= 4)goto _LL24B;_tmp33C=((struct Cyc_Absyn_PointerType_struct*)_tmp33B)->f1;
_tmp33D=(void*)_tmp33C.elt_typ;_tmp33E=(void*)_tmp33C.rgn_typ;_tmp33F=_tmp33C.nullable;
_tmp340=_tmp33C.tq;_tmp341=_tmp33C.bounds;_LL24A:{void*_tmp342=Cyc_Tcutil_compress(
_tmp33D);struct Cyc_Absyn_FnInfo _tmp343;struct Cyc_List_List*_tmp344;struct Cyc_Core_Opt*
_tmp345;void*_tmp346;struct Cyc_List_List*_tmp347;int _tmp348;struct Cyc_Absyn_VarargInfo*
_tmp349;struct Cyc_List_List*_tmp34A;struct Cyc_List_List*_tmp34B;_LL24E: if(
_tmp342 <= (void*)3?1:*((int*)_tmp342)!= 8)goto _LL250;_tmp343=((struct Cyc_Absyn_FnType_struct*)
_tmp342)->f1;_tmp344=_tmp343.tvars;_tmp345=_tmp343.effect;_tmp346=(void*)_tmp343.ret_typ;
_tmp347=_tmp343.args;_tmp348=_tmp343.c_varargs;_tmp349=_tmp343.cyc_varargs;
_tmp34A=_tmp343.rgn_po;_tmp34B=_tmp343.attributes;_LL24F: if(_tmp344 != 0){struct
_RegionHandle _tmp34C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp34C;
_push_region(rgn);{struct _tuple4 _tmp34D=({struct _tuple4 _tmp357;_tmp357.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp357.f2=rgn;_tmp357;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp34D,_tmp344);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_Absyn_FnType_struct
_tmp355;_tmp355.tag=8;_tmp355.f1=({struct Cyc_Absyn_FnInfo _tmp356;_tmp356.tvars=0;
_tmp356.effect=_tmp345;_tmp356.ret_typ=(void*)_tmp346;_tmp356.args=_tmp347;
_tmp356.c_varargs=_tmp348;_tmp356.cyc_varargs=_tmp349;_tmp356.rgn_po=_tmp34A;
_tmp356.attributes=_tmp34B;_tmp356;});_tmp355;});_tmp354;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351[0]=({
struct Cyc_Absyn_PointerType_struct _tmp352;_tmp352.tag=4;_tmp352.f1=({struct Cyc_Absyn_PtrInfo
_tmp353;_tmp353.elt_typ=(void*)ftyp;_tmp353.rgn_typ=(void*)_tmp33E;_tmp353.nullable=
_tmp33F;_tmp353.tq=_tmp340;_tmp353.bounds=_tmp341;_tmp353;});_tmp352;});_tmp351;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp34F;_tmp34F.tag=12;_tmp34F.f1=inner;
_tmp34F.f2=ts;_tmp34F;});_tmp34E;})));e->topt=({struct Cyc_Core_Opt*_tmp350=
_cycalloc(sizeof(*_tmp350));_tmp350->v=(void*)new_typ;_tmp350;});};_pop_region(
rgn);}goto _LL24D;_LL250:;_LL251: goto _LL24D;_LL24D:;}goto _LL248;_LL24B:;_LL24C:
goto _LL248;_LL248:;}goto _LL243;_LL243:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp358=(void*)e->r;struct Cyc_Absyn_Exp*_tmp359;struct _tuple1*_tmp35A;
struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_Core_Opt*
_tmp35D;struct Cyc_List_List*_tmp35E;void*_tmp35F;struct _tuple1*_tmp360;void*
_tmp361;void*_tmp362;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_Exp*_tmp364;
void*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Core_Opt*_tmp367;struct Cyc_Absyn_Exp*
_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*
_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*
_tmp36E;struct Cyc_List_List*_tmp36F;struct Cyc_Absyn_VarargCallInfo*_tmp370;
struct Cyc_Absyn_VarargCallInfo**_tmp371;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_Exp*
_tmp373;struct Cyc_List_List*_tmp374;void*_tmp375;struct Cyc_Absyn_Exp*_tmp376;
struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*
_tmp379;struct Cyc_Absyn_Exp*_tmp37A;void*_tmp37B;void*_tmp37C;void*_tmp37D;
struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct _tagged_arr*
_tmp380;struct Cyc_Absyn_Exp*_tmp381;struct _tagged_arr*_tmp382;struct Cyc_Absyn_Exp*
_tmp383;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_List_List*_tmp385;struct _tuple2*
_tmp386;struct Cyc_List_List*_tmp387;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_Stmt*
_tmp389;struct Cyc_Absyn_Fndecl*_tmp38A;struct Cyc_Absyn_Exp*_tmp38B;struct Cyc_Absyn_Vardecl*
_tmp38C;struct Cyc_Absyn_Exp*_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct _tuple1*
_tmp38F;struct _tuple1**_tmp390;struct Cyc_List_List*_tmp391;struct Cyc_List_List**
_tmp392;struct Cyc_List_List*_tmp393;struct Cyc_Absyn_Aggrdecl*_tmp394;struct Cyc_Absyn_Aggrdecl**
_tmp395;void*_tmp396;struct Cyc_List_List*_tmp397;struct Cyc_List_List*_tmp398;
struct Cyc_Absyn_Tuniondecl*_tmp399;struct Cyc_Absyn_Tunionfield*_tmp39A;struct
_tuple1*_tmp39B;struct _tuple1**_tmp39C;struct Cyc_Absyn_Enumdecl*_tmp39D;struct
Cyc_Absyn_Enumfield*_tmp39E;struct _tuple1*_tmp39F;struct _tuple1**_tmp3A0;void*
_tmp3A1;struct Cyc_Absyn_Enumfield*_tmp3A2;struct Cyc_Absyn_MallocInfo _tmp3A3;int
_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;void**_tmp3A6;void***_tmp3A7;struct Cyc_Absyn_Exp*
_tmp3A8;struct Cyc_Absyn_Exp**_tmp3A9;int _tmp3AA;int*_tmp3AB;_LL253: if(*((int*)
_tmp358)!= 11)goto _LL255;_tmp359=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp358)->f1;_LL254: Cyc_Tcexp_tcExpNoInst(te,0,_tmp359);return;_LL255: if(*((int*)
_tmp358)!= 2)goto _LL257;_tmp35A=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp358)->f1;
_LL256: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp35A);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL257: if(*((int*)_tmp358)!= 8)goto _LL259;_tmp35B=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp358)->f1;_tmp35C=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp358)->f2;_LL258:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp35B,_tmp35C);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL259: if(*((int*)_tmp358)!= 34)goto _LL25B;_tmp35D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp358)->f1;_tmp35E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp358)->f2;
_LL25A: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp35E);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL25B: if(*((int*)_tmp358)!= 0)goto _LL25D;_tmp35F=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp358)->f1;_LL25C: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp35F,e);goto _LL252;_LL25D: if(*((int*)_tmp358)!= 1)goto _LL25F;_tmp360=((
struct Cyc_Absyn_Var_e_struct*)_tmp358)->f1;_tmp361=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp358)->f2;_LL25E: t=Cyc_Tcexp_tcVar(te,loc,_tmp360,_tmp361);goto _LL252;_LL25F:
if(*((int*)_tmp358)!= 3)goto _LL261;_tmp362=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp358)->f1;_tmp363=((struct Cyc_Absyn_Primop_e_struct*)_tmp358)->f2;_LL260: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp362,_tmp363);goto _LL252;_LL261: if(*((int*)
_tmp358)!= 5)goto _LL263;_tmp364=((struct Cyc_Absyn_Increment_e_struct*)_tmp358)->f1;
_tmp365=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp358)->f2;_LL262: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp364,_tmp365);goto _LL252;_LL263: if(*((int*)_tmp358)!= 4)goto
_LL265;_tmp366=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp358)->f1;_tmp367=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp358)->f2;_tmp368=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp358)->f3;_LL264: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp366,_tmp367,_tmp368);
goto _LL252;_LL265: if(*((int*)_tmp358)!= 6)goto _LL267;_tmp369=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp358)->f1;_tmp36A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp358)->f2;
_tmp36B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp358)->f3;_LL266: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp369,_tmp36A,_tmp36B);goto _LL252;_LL267: if(*((int*)_tmp358)!= 7)
goto _LL269;_tmp36C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp358)->f1;_tmp36D=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp358)->f2;_LL268: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp36C,_tmp36D);goto _LL252;_LL269: if(*((int*)_tmp358)!= 9)goto _LL26B;
_tmp36E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp358)->f1;_tmp36F=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp358)->f2;_tmp370=((struct Cyc_Absyn_FnCall_e_struct*)_tmp358)->f3;_tmp371=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp358)->f3;
_LL26A: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp36E,_tmp36F,_tmp371);goto _LL252;
_LL26B: if(*((int*)_tmp358)!= 10)goto _LL26D;_tmp372=((struct Cyc_Absyn_Throw_e_struct*)
_tmp358)->f1;_LL26C: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp372);goto _LL252;_LL26D:
if(*((int*)_tmp358)!= 12)goto _LL26F;_tmp373=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp358)->f1;_tmp374=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp358)->f2;_LL26E:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp373,_tmp374);goto _LL252;_LL26F: if(*((
int*)_tmp358)!= 13)goto _LL271;_tmp375=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp358)->f1;_tmp376=((struct Cyc_Absyn_Cast_e_struct*)_tmp358)->f2;_LL270: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp375,_tmp376);goto _LL252;_LL271: if(*((int*)_tmp358)!= 14)goto
_LL273;_tmp377=((struct Cyc_Absyn_Address_e_struct*)_tmp358)->f1;_LL272: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp377);goto _LL252;_LL273: if(*((int*)_tmp358)!= 15)goto _LL275;
_tmp378=((struct Cyc_Absyn_New_e_struct*)_tmp358)->f1;_tmp379=((struct Cyc_Absyn_New_e_struct*)
_tmp358)->f2;_LL274: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp378,e,_tmp379);goto _LL252;
_LL275: if(*((int*)_tmp358)!= 17)goto _LL277;_tmp37A=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp358)->f1;_LL276: {void*_tmp3AC=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp37A);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp3AC);goto _LL252;}_LL277: if(*((int*)_tmp358)!= 16)goto _LL279;
_tmp37B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp358)->f1;_LL278: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp37B);goto _LL252;_LL279: if(*((int*)_tmp358)!= 18)goto _LL27B;
_tmp37C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp358)->f1;_tmp37D=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp358)->f2;_LL27A: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp37C,_tmp37D);goto _LL252;_LL27B: if(*((int*)_tmp358)!= 19)goto _LL27D;
_LL27C:({void*_tmp3AD[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(char),35),_tag_arr(_tmp3AD,sizeof(void*),0));});return;_LL27D: if(*((int*)
_tmp358)!= 20)goto _LL27F;_tmp37E=((struct Cyc_Absyn_Deref_e_struct*)_tmp358)->f1;
_LL27E: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp37E);goto _LL252;_LL27F: if(*((int*)
_tmp358)!= 21)goto _LL281;_tmp37F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp358)->f1;
_tmp380=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp358)->f2;_LL280: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp37F,_tmp380);goto _LL252;_LL281: if(*((int*)_tmp358)!= 22)goto
_LL283;_tmp381=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp358)->f1;_tmp382=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp358)->f2;_LL282: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp381,_tmp382);goto _LL252;_LL283: if(*((int*)_tmp358)!= 23)goto
_LL285;_tmp383=((struct Cyc_Absyn_Subscript_e_struct*)_tmp358)->f1;_tmp384=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp358)->f2;_LL284: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp383,_tmp384);goto _LL252;_LL285: if(*((int*)_tmp358)!= 24)goto
_LL287;_tmp385=((struct Cyc_Absyn_Tuple_e_struct*)_tmp358)->f1;_LL286: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp385);goto _LL252;_LL287: if(*((int*)_tmp358)!= 25)goto _LL289;
_tmp386=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp358)->f1;_tmp387=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp358)->f2;_LL288: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp386,_tmp387);goto
_LL252;_LL289: if(*((int*)_tmp358)!= 26)goto _LL28B;_tmp388=((struct Cyc_Absyn_Array_e_struct*)
_tmp358)->f1;_LL28A: {void**elt_topt=0;if(topt != 0){void*_tmp3AE=Cyc_Tcutil_compress(*
topt);void*_tmp3AF;void**_tmp3B0;_LL2A0: if(_tmp3AE <= (void*)3?1:*((int*)_tmp3AE)
!= 7)goto _LL2A2;_tmp3AF=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3AE)->f1;
_tmp3B0=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3AE)->f1);
_LL2A1: elt_topt=(void**)_tmp3B0;goto _LL29F;_LL2A2:;_LL2A3: goto _LL29F;_LL29F:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp388);goto _LL252;}_LL28B: if(*((int*)_tmp358)
!= 35)goto _LL28D;_tmp389=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp358)->f1;_LL28C:
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp389);goto _LL252;_LL28D: if(*((int*)_tmp358)
!= 36)goto _LL28F;_tmp38A=((struct Cyc_Absyn_Codegen_e_struct*)_tmp358)->f1;_LL28E:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp38A);goto _LL252;_LL28F: if(*((int*)_tmp358)
!= 37)goto _LL291;_tmp38B=((struct Cyc_Absyn_Fill_e_struct*)_tmp358)->f1;_LL290: t=
Cyc_Tcexp_tcFill(te,loc,topt,_tmp38B);goto _LL252;_LL291: if(*((int*)_tmp358)!= 27)
goto _LL293;_tmp38C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp358)->f1;
_tmp38D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp358)->f2;_tmp38E=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp358)->f3;_LL292: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp38C,_tmp38D,_tmp38E);goto _LL252;_LL293: if(*((int*)_tmp358)!= 28)
goto _LL295;_tmp38F=((struct Cyc_Absyn_Struct_e_struct*)_tmp358)->f1;_tmp390=(
struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp358)->f1;_tmp391=((
struct Cyc_Absyn_Struct_e_struct*)_tmp358)->f2;_tmp392=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp358)->f2;_tmp393=((struct Cyc_Absyn_Struct_e_struct*)
_tmp358)->f3;_tmp394=((struct Cyc_Absyn_Struct_e_struct*)_tmp358)->f4;_tmp395=(
struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp358)->f4;
_LL294: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp390,_tmp392,_tmp393,_tmp395);goto
_LL252;_LL295: if(*((int*)_tmp358)!= 29)goto _LL297;_tmp396=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp358)->f1;_tmp397=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp358)->f2;_LL296:
t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp396,_tmp397);goto _LL252;_LL297: if(*((int*)
_tmp358)!= 30)goto _LL299;_tmp398=((struct Cyc_Absyn_Tunion_e_struct*)_tmp358)->f1;
_tmp399=((struct Cyc_Absyn_Tunion_e_struct*)_tmp358)->f2;_tmp39A=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp358)->f3;_LL298: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp398,_tmp399,_tmp39A);
goto _LL252;_LL299: if(*((int*)_tmp358)!= 31)goto _LL29B;_tmp39B=((struct Cyc_Absyn_Enum_e_struct*)
_tmp358)->f1;_tmp39C=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp358)->f1;
_tmp39D=((struct Cyc_Absyn_Enum_e_struct*)_tmp358)->f2;_tmp39E=((struct Cyc_Absyn_Enum_e_struct*)
_tmp358)->f3;_LL29A:*_tmp39C=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp39E))->name;
t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));
_tmp3B1[0]=({struct Cyc_Absyn_EnumType_struct _tmp3B2;_tmp3B2.tag=12;_tmp3B2.f1=((
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp39D))->name;_tmp3B2.f2=_tmp39D;_tmp3B2;});
_tmp3B1;});goto _LL252;_LL29B: if(*((int*)_tmp358)!= 32)goto _LL29D;_tmp39F=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp358)->f1;_tmp3A0=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp358)->f1;_tmp3A1=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp358)->f2;_tmp3A2=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp358)->f3;_LL29C:*
_tmp3A0=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp3A2))->name;t=_tmp3A1;goto
_LL252;_LL29D: if(*((int*)_tmp358)!= 33)goto _LL252;_tmp3A3=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp358)->f1;_tmp3A4=_tmp3A3.is_calloc;_tmp3A5=_tmp3A3.rgn;_tmp3A6=_tmp3A3.elt_type;
_tmp3A7=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp358)->f1).elt_type;
_tmp3A8=_tmp3A3.num_elts;_tmp3A9=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp358)->f1).num_elts;_tmp3AA=_tmp3A3.fat_result;_tmp3AB=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp358)->f1).fat_result;_LL29E: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp3A5,_tmp3A7,
_tmp3A9,_tmp3A4,_tmp3AB);goto _LL252;_LL252:;}e->topt=({struct Cyc_Core_Opt*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->v=(void*)t;_tmp3B3;});}
