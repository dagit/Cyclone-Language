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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
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
15];struct _tagged_arr*Cyc_Absyn_fieldname(int);int Cyc_Absyn_is_union_type(void*);
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(struct
Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noreturn(void*);unsigned int Cyc_Evexp_eval_const_uint_exp(
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;int
Cyc_CfFlowInfo_root_cmp(void*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(void*t,
void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r);void*
Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
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
all_changed;int noreturn;};static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Decl*);static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp8=Cyc_NewControlFlow_get_stmt_annot(s);(void*)(
_tmp8->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(void*)_tmp8->flow));
_tmp8->visited=env->iteration_num;return _tmp8;}static void Cyc_NewControlFlow_update_tryflow(
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
env,(void*)_tmp18);return(void*)_tmp18;}}_LLA:;}struct _tuple4{void*f1;struct Cyc_List_List*
f2;};static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0)return({struct _tuple4 _tmp1B;
_tmp1B.f1=inflow;_tmp1B.f2=0;_tmp1B;});if(es->tl == 0){void*_tmp1D;void*_tmp1E;
struct _tuple0 _tmp1C=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;return({struct _tuple4 _tmp1F;_tmp1F.f1=
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
return({struct _tuple4 _tmp2C;_tmp2C.f1=outflow;_tmp2C.f2=Cyc_List_imp_rev(rvals);
_tmp2C;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){void*_tmp2E;struct Cyc_List_List*_tmp2F;
struct _tuple4 _tmp2D=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);
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
_LL4F:;_LL50: return relns;_LL46:;}struct _tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
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
_tmp83)->f1;_tmpA3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp83)->f2;_LL9A: _tmpA3=({
struct Cyc_List_List*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->hd=_tmpA2;
_tmp101->tl=_tmpA3;_tmp101;});{void*_tmp103;struct Cyc_List_List*_tmp104;struct
_tuple4 _tmp102=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmpA3);
_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;for(0;_tmp104 != 0;(_tmp104=_tmp104->tl,
_tmpA3=((struct Cyc_List_List*)_check_null(_tmpA3))->tl)){_tmp103=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpA3))->hd)->loc,
_tmp103,(void*)_tmp104->hd);}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA2->topt))->v))return({struct _tuple0 _tmp105;_tmp105.f1=(void*)0;
_tmp105.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp105;});else{return({
struct _tuple0 _tmp106;_tmp106.f1=_tmp103;_tmp106.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp106;});}}_LL9B: if(*((int*)_tmp83)!= 33)goto _LL9D;_tmpA4=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp83)->f1;_tmpA5=_tmpA4.is_calloc;_tmpA6=_tmpA4.rgn;_tmpA7=_tmpA4.elt_type;
_tmpA8=_tmpA4.num_elts;_tmpA9=_tmpA4.fat_result;_LL9C: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp118;_tmp118.tag=1;_tmp118.f1=_tmpA8;
_tmp118.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp118;});_tmp117;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->root=(void*)root;_tmp116->fields=0;
_tmp116;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp114=
_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp115;_tmp115.tag=2;_tmp115.f1=place;_tmp115;});_tmp114;});void*place_val=
_tmpA9?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpA7)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpA6 != 0){void*
_tmp109;struct Cyc_List_List*_tmp10A;struct _tuple4 _tmp108=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp107[2];_tmp107[1]=_tmpA8;_tmp107[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpA6);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp107,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp109=
_tmp108.f1;_tmp10A=_tmp108.f2;outflow=_tmp109;}else{void*_tmp10C;struct _tuple0
_tmp10B=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA8);_tmp10C=_tmp10B.f1;
outflow=_tmp10C;}{void*_tmp10D=outflow;struct Cyc_Dict_Dict*_tmp10E;struct Cyc_List_List*
_tmp10F;_LLDC: if((int)_tmp10D != 0)goto _LLDE;_LLDD: return({struct _tuple0 _tmp110;
_tmp110.f1=outflow;_tmp110.f2=rval;_tmp110;});_LLDE: if(_tmp10D <= (void*)1?1:*((
int*)_tmp10D)!= 0)goto _LLDB;_tmp10E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp10D)->f1;_tmp10F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp10D)->f2;
_LLDF: return({struct _tuple0 _tmp111;_tmp111.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp113;_tmp113.tag=0;_tmp113.f1=Cyc_Dict_insert(_tmp10E,root,place_val);_tmp113.f2=
_tmp10F;_tmp113;});_tmp112;});_tmp111.f2=rval;_tmp111;});_LLDB:;}}_LL9D: if(*((
int*)_tmp83)!= 15)goto _LL9F;_tmpAA=((struct Cyc_Absyn_New_e_struct*)_tmp83)->f1;
_tmpAB=((struct Cyc_Absyn_New_e_struct*)_tmp83)->f2;_LL9E: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp12B;_tmp12B.tag=1;_tmp12B.f1=_tmpAB;
_tmp12B.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp12B;});_tmp12A;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->root=(void*)root;_tmp129->fields=0;
_tmp129;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp127=
_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp128;_tmp128.tag=2;_tmp128.f1=place;_tmp128;});_tmp127;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpAA != 0){
void*_tmp11B;struct Cyc_List_List*_tmp11C;struct _tuple4 _tmp11A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp119[2];_tmp119[1]=_tmpAB;_tmp119[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpAA);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp119,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp11B=
_tmp11A.f1;_tmp11C=_tmp11A.f2;outflow=_tmp11B;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp11C))->tl))->hd;}else{void*
_tmp11E;void*_tmp11F;struct _tuple0 _tmp11D=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpAB);_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;outflow=_tmp11E;place_val=
_tmp11F;}{void*_tmp120=outflow;struct Cyc_Dict_Dict*_tmp121;struct Cyc_List_List*
_tmp122;_LLE1: if((int)_tmp120 != 0)goto _LLE3;_LLE2: return({struct _tuple0 _tmp123;
_tmp123.f1=outflow;_tmp123.f2=rval;_tmp123;});_LLE3: if(_tmp120 <= (void*)1?1:*((
int*)_tmp120)!= 0)goto _LLE0;_tmp121=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp120)->f1;_tmp122=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp120)->f2;
_LLE4: return({struct _tuple0 _tmp124;_tmp124.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=Cyc_Dict_insert(_tmp121,root,place_val);_tmp126.f2=
_tmp122;_tmp126;});_tmp125;});_tmp124.f2=rval;_tmp124;});_LLE0:;}}}_LL9F: if(*((
int*)_tmp83)!= 14)goto _LLA1;_tmpAC=((struct Cyc_Absyn_Address_e_struct*)_tmp83)->f1;
_LLA0: {void*_tmp12D;void*_tmp12E;struct _tuple0 _tmp12C=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpAC);_tmp12D=_tmp12C.f1;_tmp12E=_tmp12C.f2;{void*_tmp12F=_tmp12E;
struct Cyc_CfFlowInfo_Place*_tmp130;_LLE6: if((int)_tmp12F != 0)goto _LLE8;_LLE7:
return({struct _tuple0 _tmp131;_tmp131.f1=_tmp12D;_tmp131.f2=(void*)1;_tmp131;});
_LLE8: if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 0)goto _LLE5;_tmp130=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp12F)->f1;_LLE9: return({struct _tuple0 _tmp132;_tmp132.f1=_tmp12D;_tmp132.f2=(
void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp133=_cycalloc(sizeof(*_tmp133));
_tmp133[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp134;_tmp134.tag=2;_tmp134.f1=
_tmp130;_tmp134;});_tmp133;});_tmp132;});_LLE5:;}}_LLA1: if(*((int*)_tmp83)!= 20)
goto _LLA3;_tmpAD=((struct Cyc_Absyn_Deref_e_struct*)_tmp83)->f1;_LLA2: {void*
_tmp136;void*_tmp137;struct _tuple0 _tmp135=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpAD);_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp136,_tmpAD,_tmp137);}_LLA3: if(*((int*)_tmp83)!= 21)goto _LLA5;
_tmpAE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp83)->f1;_tmpAF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp83)->f2;_LLA4: {void*_tmp139;void*_tmp13A;struct _tuple0 _tmp138=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpAE);_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v))return({struct _tuple0
_tmp13B;_tmp13B.f1=_tmp139;_tmp13B.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp13B;});{
void*_tmp13C=_tmp13A;struct Cyc_Dict_Dict*_tmp13D;_LLEB: if(_tmp13C <= (void*)3?1:*((
int*)_tmp13C)!= 3)goto _LLED;_tmp13D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp13C)->f1;_LLEC: return({struct _tuple0 _tmp13E;_tmp13E.f1=_tmp139;_tmp13E.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp13D,
_tmpAF);_tmp13E;});_LLED:;_LLEE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=({struct Cyc_Core_Impossible_struct
_tmp140;_tmp140.tag=Cyc_Core_Impossible;_tmp140.f1=_tag_arr("anal_Rexp: AggrMember",
sizeof(char),22);_tmp140;});_tmp13F;}));_LLEA:;}}_LLA5: if(*((int*)_tmp83)!= 22)
goto _LLA7;_tmpB0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp83)->f1;_tmpB1=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp83)->f2;_LLA6: {void*_tmp142;void*
_tmp143;struct _tuple0 _tmp141=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpB0);
_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;{void*_tmp145;void*_tmp146;struct _tuple0
_tmp144=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp142,_tmpB0,_tmp143);
_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{void*_tmp147=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp148;void*_tmp149;_LLF0: if(_tmp147 <= (void*)3?1:*((int*)_tmp147)!= 4)goto
_LLF2;_tmp148=((struct Cyc_Absyn_PointerType_struct*)_tmp147)->f1;_tmp149=(void*)
_tmp148.elt_typ;_LLF1: if(Cyc_Absyn_is_union_type(_tmp149))return({struct _tuple0
_tmp14A;_tmp14A.f1=_tmp145;_tmp14A.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp14A;});
goto _LLEF;_LLF2:;_LLF3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Core_Impossible_struct
_tmp14C;_tmp14C.tag=Cyc_Core_Impossible;_tmp14C.f1=_tag_arr("anal_Rexp: AggrArrow ptr",
sizeof(char),25);_tmp14C;});_tmp14B;}));_LLEF:;}{void*_tmp14D=_tmp146;struct Cyc_Dict_Dict*
_tmp14E;_LLF5: if(_tmp14D <= (void*)3?1:*((int*)_tmp14D)!= 3)goto _LLF7;_tmp14E=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp14D)->f1;_LLF6: return({struct _tuple0
_tmp14F;_tmp14F.f1=_tmp145;_tmp14F.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp14E,_tmpB1);_tmp14F;});_LLF7:;_LLF8:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp150=_cycalloc(sizeof(*
_tmp150));_tmp150[0]=({struct Cyc_Core_Impossible_struct _tmp151;_tmp151.tag=Cyc_Core_Impossible;
_tmp151.f1=_tag_arr("anal_Rexp: AggrArrow",sizeof(char),21);_tmp151;});_tmp150;}));
_LLF4:;}}}_LLA7: if(*((int*)_tmp83)!= 6)goto _LLA9;_tmpB2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp83)->f1;_tmpB3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f2;_tmpB4=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f3;_LLA8: {void*_tmp153;void*
_tmp154;struct _tuple0 _tmp152=Cyc_NewControlFlow_anal_test(env,inflow,_tmpB2);
_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{void*_tmp156;void*_tmp157;struct _tuple0
_tmp155=Cyc_NewControlFlow_anal_Rexp(env,_tmp153,_tmpB3);_tmp156=_tmp155.f1;
_tmp157=_tmp155.f2;{void*_tmp159;void*_tmp15A;struct _tuple0 _tmp158=Cyc_NewControlFlow_anal_Rexp(
env,_tmp154,_tmpB4);_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;return({struct _tuple0
_tmp15B;_tmp15B.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_use_Rval(
env,_tmpB3->loc,_tmp156,_tmp157),Cyc_NewControlFlow_use_Rval(env,_tmpB4->loc,
_tmp159,_tmp15A));_tmp15B.f2=Cyc_CfFlowInfo_unknown_all;_tmp15B;});}}}_LLA9: if(*((
int*)_tmp83)!= 23)goto _LLAB;_tmpB5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f1;
_tmpB6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp83)->f2;_LLAA: {void*_tmp15E;
struct Cyc_List_List*_tmp15F;struct _tuple4 _tmp15D=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp15C[2];_tmp15C[1]=_tmpB6;_tmp15C[0]=_tmpB5;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp15C,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;{void*
_tmp160=_tmp15E;struct Cyc_Dict_Dict*_tmp161;struct Cyc_List_List*_tmp162;_LLFA:
if(_tmp160 <= (void*)1?1:*((int*)_tmp160)!= 0)goto _LLFC;_tmp161=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp160)->f1;_tmp162=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp160)->f2;
_LLFB: if(Cyc_CfFlowInfo_initlevel(_tmp161,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp15F))->tl))->hd)== (void*)0)({
void*_tmp163[0]={};Cyc_Tcutil_terr(_tmpB6->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp163,sizeof(void*),0));});{struct Cyc_List_List*
_tmp164=Cyc_NewControlFlow_add_subscript_reln(_tmp162,_tmpB5,_tmpB6);if(_tmp162
!= _tmp164)_tmp15E=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp165=
_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=_tmp161;_tmp166.f2=_tmp164;_tmp166;});_tmp165;});
goto _LLF9;}_LLFC:;_LLFD: goto _LLF9;_LLF9:;}{void*_tmp167=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB5->topt))->v);_LLFF: if(_tmp167 <= (
void*)3?1:*((int*)_tmp167)!= 9)goto _LL101;_LL100: {void*_tmp168=(void*)((struct
Cyc_List_List*)_check_null(_tmp15F))->hd;struct Cyc_Dict_Dict*_tmp169;_LL104: if(
_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 3)goto _LL106;_tmp169=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp168)->f1;_LL105: return({struct _tuple0 _tmp16A;_tmp16A.f1=_tmp15E;_tmp16A.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp169,Cyc_Absyn_fieldname((
int)Cyc_Evexp_eval_const_uint_exp(_tmpB6)));_tmp16A;});_LL106:;_LL107:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp16B=_cycalloc(sizeof(*
_tmp16B));_tmp16B[0]=({struct Cyc_Core_Impossible_struct _tmp16C;_tmp16C.tag=Cyc_Core_Impossible;
_tmp16C.f1=_tag_arr("anal_Rexp: Subscript",sizeof(char),21);_tmp16C;});_tmp16B;}));
_LL103:;}_LL101:;_LL102: goto _LLFE;_LLFE:;}return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp15E,_tmpB5,(void*)((struct Cyc_List_List*)_check_null(_tmp15F))->hd);}
_LLAB: if(*((int*)_tmp83)!= 30)goto _LLAD;_tmpB7=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp83)->f1;_LLAC: _tmpB8=_tmpB7;goto _LLAE;_LLAD: if(*((int*)_tmp83)!= 24)goto
_LLAF;_tmpB8=((struct Cyc_Absyn_Tuple_e_struct*)_tmp83)->f1;_LLAE: {void*_tmp16E;
struct Cyc_List_List*_tmp16F;struct _tuple4 _tmp16D=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpB8);_tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;{struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;for(0;_tmp16F != 0;(
_tmp16F=_tmp16F->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(i),(
void*)_tmp16F->hd);}}return({struct _tuple0 _tmp170;_tmp170.f1=_tmp16E;_tmp170.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp171=_cycalloc(sizeof(*_tmp171));
_tmp171[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp172;_tmp172.tag=3;_tmp172.f1=
aggrdict;_tmp172;});_tmp171;});_tmp170;});}}_LLAF: if(*((int*)_tmp83)!= 29)goto
_LLB1;_tmpB9=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp83)->f2;_LLB0: _tmpBA=
_tmpB9;goto _LLB2;_LLB1: if(*((int*)_tmp83)!= 28)goto _LLB3;_tmpBA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp83)->f3;_LLB2: {void*_tmp174;struct Cyc_List_List*_tmp175;struct _tuple4
_tmp173=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple5*))Cyc_Core_snd,_tmpBA));_tmp174=_tmp173.f1;
_tmp175=_tmp173.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{
int i=0;for(0;_tmp175 != 0;(((_tmp175=_tmp175->tl,_tmpBA=_tmpBA->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple5*)((struct Cyc_List_List*)_check_null(_tmpBA))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp176=(void*)ds->hd;struct _tagged_arr*_tmp177;
_LL109: if(*((int*)_tmp176)!= 0)goto _LL10B;_LL10A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Core_Impossible_struct
_tmp179;_tmp179.tag=Cyc_Core_Impossible;_tmp179.f1=_tag_arr("anal_Rexp:Struct_e",
sizeof(char),19);_tmp179;});_tmp178;}));_LL10B: if(*((int*)_tmp176)!= 1)goto
_LL108;_tmp177=((struct Cyc_Absyn_FieldName_struct*)_tmp176)->f1;_LL10C: aggrdict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(
aggrdict,_tmp177,(void*)_tmp175->hd);_LL108:;}}}return({struct _tuple0 _tmp17A;
_tmp17A.f1=_tmp174;_tmp17A.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp17C;_tmp17C.tag=3;_tmp17C.f1=aggrdict;_tmp17C;});_tmp17B;});_tmp17A;});}}
_LLB3: if(*((int*)_tmp83)!= 26)goto _LLB5;_tmpBB=((struct Cyc_Absyn_Array_e_struct*)
_tmp83)->f1;_LLB4: {struct Cyc_List_List*_tmp17D=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Exp*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((struct
Cyc_Absyn_Exp*(*)(struct _tuple5*))Cyc_Core_snd,_tmpBB);void*_tmp17F;struct Cyc_List_List*
_tmp180;struct _tuple4 _tmp17E=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,
_tmp17D);_tmp17F=_tmp17E.f1;_tmp180=_tmp17E.f2;for(0;_tmp180 != 0;(_tmp180=
_tmp180->tl,_tmp17D=_tmp17D->tl)){_tmp17F=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd)->loc,
_tmp17F,(void*)_tmp180->hd);}return({struct _tuple0 _tmp181;_tmp181.f1=_tmp17F;
_tmp181.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp181;});}_LLB5: if(*((int*)
_tmp83)!= 27)goto _LLB7;_tmpBC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f1;
_tmpBD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp83)->f2;_tmpBE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp83)->f3;_LLB6: {void*_tmp183;void*_tmp184;struct _tuple0 _tmp182=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBD);_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;{void*_tmp185=_tmp183;
struct Cyc_Dict_Dict*_tmp186;struct Cyc_List_List*_tmp187;_LL10E: if((int)_tmp185 != 
0)goto _LL110;_LL10F: return({struct _tuple0 _tmp188;_tmp188.f1=_tmp183;_tmp188.f2=
Cyc_CfFlowInfo_unknown_all;_tmp188;});_LL110: if(_tmp185 <= (void*)1?1:*((int*)
_tmp185)!= 0)goto _LL10D;_tmp186=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp185)->f1;_tmp187=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp185)->f2;
_LL111: if(Cyc_CfFlowInfo_initlevel(_tmp186,_tmp184)== (void*)0)({void*_tmp189[0]={};
Cyc_Tcutil_terr(_tmpBD->loc,_tag_arr("expression may not be initialized",sizeof(
char),34),_tag_arr(_tmp189,sizeof(void*),0));});{struct Cyc_List_List*new_relns=
_tmp187;comp_loop: {void*_tmp18A=(void*)_tmpBD->r;struct Cyc_Absyn_Exp*_tmp18B;
void*_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18D;void*_tmp18E;struct Cyc_Absyn_Vardecl*
_tmp18F;void*_tmp190;struct Cyc_Absyn_Vardecl*_tmp191;void*_tmp192;struct Cyc_Absyn_Vardecl*
_tmp193;void*_tmp194;int _tmp195;void*_tmp196;struct Cyc_List_List*_tmp197;struct
Cyc_List_List _tmp198;struct Cyc_Absyn_Exp*_tmp199;_LL113: if(*((int*)_tmp18A)!= 13)
goto _LL115;_tmp18B=((struct Cyc_Absyn_Cast_e_struct*)_tmp18A)->f2;_LL114: _tmpBD=
_tmp18B;goto comp_loop;_LL115: if(*((int*)_tmp18A)!= 1)goto _LL117;_tmp18C=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp18A)->f2;if(_tmp18C <= (void*)1?1:*((int*)
_tmp18C)!= 0)goto _LL117;_tmp18D=((struct Cyc_Absyn_Global_b_struct*)_tmp18C)->f1;
if(!(!_tmp18D->escapes))goto _LL117;_LL116: _tmp18F=_tmp18D;goto _LL118;_LL117: if(*((
int*)_tmp18A)!= 1)goto _LL119;_tmp18E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18A)->f2;if(_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 3)goto _LL119;_tmp18F=((
struct Cyc_Absyn_Local_b_struct*)_tmp18E)->f1;if(!(!_tmp18F->escapes))goto _LL119;
_LL118: _tmp191=_tmp18F;goto _LL11A;_LL119: if(*((int*)_tmp18A)!= 1)goto _LL11B;
_tmp190=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp18A)->f2;if(_tmp190 <= (void*)
1?1:*((int*)_tmp190)!= 4)goto _LL11B;_tmp191=((struct Cyc_Absyn_Pat_b_struct*)
_tmp190)->f1;if(!(!_tmp191->escapes))goto _LL11B;_LL11A: _tmp193=_tmp191;goto
_LL11C;_LL11B: if(*((int*)_tmp18A)!= 1)goto _LL11D;_tmp192=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18A)->f2;if(_tmp192 <= (void*)1?1:*((int*)_tmp192)!= 2)goto _LL11D;_tmp193=((
struct Cyc_Absyn_Param_b_struct*)_tmp192)->f1;if(!(!_tmp193->escapes))goto _LL11D;
_LL11C: new_relns=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));
_tmp19A->hd=({struct Cyc_CfFlowInfo_Reln*_tmp19B=_cycalloc(sizeof(*_tmp19B));
_tmp19B->vd=_tmpBC;_tmp19B->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp19D;_tmp19D.tag=1;_tmp19D.f1=_tmp193;_tmp19D;});_tmp19C;}));_tmp19B;});
_tmp19A->tl=_tmp187;_tmp19A;});goto _LL112;_LL11D: if(*((int*)_tmp18A)!= 0)goto
_LL11F;_tmp194=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp18A)->f1;if(_tmp194
<= (void*)1?1:*((int*)_tmp194)!= 2)goto _LL11F;_tmp195=((struct Cyc_Absyn_Int_c_struct*)
_tmp194)->f2;_LL11E: new_relns=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*
_tmp19E));_tmp19E->hd=({struct Cyc_CfFlowInfo_Reln*_tmp19F=_cycalloc(sizeof(*
_tmp19F));_tmp19F->vd=_tmpBC;_tmp19F->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1A0=_cycalloc_atomic(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1A1;_tmp1A1.tag=3;_tmp1A1.f1=(unsigned int)_tmp195;_tmp1A1;});_tmp1A0;}));
_tmp19F;});_tmp19E->tl=_tmp187;_tmp19E;});goto _LL112;_LL11F: if(*((int*)_tmp18A)
!= 3)goto _LL121;_tmp196=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp18A)->f1;
if((int)_tmp196 != 19)goto _LL121;_tmp197=((struct Cyc_Absyn_Primop_e_struct*)
_tmp18A)->f2;if(_tmp197 == 0)goto _LL121;_tmp198=*_tmp197;_tmp199=(struct Cyc_Absyn_Exp*)
_tmp198.hd;_LL120:{void*_tmp1A2=(void*)_tmp199->r;void*_tmp1A3;struct Cyc_Absyn_Vardecl*
_tmp1A4;void*_tmp1A5;struct Cyc_Absyn_Vardecl*_tmp1A6;void*_tmp1A7;struct Cyc_Absyn_Vardecl*
_tmp1A8;void*_tmp1A9;struct Cyc_Absyn_Vardecl*_tmp1AA;_LL124: if(*((int*)_tmp1A2)
!= 1)goto _LL126;_tmp1A3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A2)->f2;if(
_tmp1A3 <= (void*)1?1:*((int*)_tmp1A3)!= 0)goto _LL126;_tmp1A4=((struct Cyc_Absyn_Global_b_struct*)
_tmp1A3)->f1;if(!(!_tmp1A4->escapes))goto _LL126;_LL125: _tmp1A6=_tmp1A4;goto
_LL127;_LL126: if(*((int*)_tmp1A2)!= 1)goto _LL128;_tmp1A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A2)->f2;if(_tmp1A5 <= (void*)1?1:*((int*)_tmp1A5)!= 3)goto _LL128;_tmp1A6=((
struct Cyc_Absyn_Local_b_struct*)_tmp1A5)->f1;if(!(!_tmp1A6->escapes))goto _LL128;
_LL127: _tmp1A8=_tmp1A6;goto _LL129;_LL128: if(*((int*)_tmp1A2)!= 1)goto _LL12A;
_tmp1A7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A2)->f2;if(_tmp1A7 <= (void*)
1?1:*((int*)_tmp1A7)!= 4)goto _LL12A;_tmp1A8=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1A7)->f1;if(!(!_tmp1A8->escapes))goto _LL12A;_LL129: _tmp1AA=_tmp1A8;goto
_LL12B;_LL12A: if(*((int*)_tmp1A2)!= 1)goto _LL12C;_tmp1A9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A2)->f2;if(_tmp1A9 <= (void*)1?1:*((int*)_tmp1A9)!= 2)goto _LL12C;_tmp1AA=((
struct Cyc_Absyn_Param_b_struct*)_tmp1A9)->f1;if(!(!_tmp1AA->escapes))goto _LL12C;
_LL12B: new_relns=({struct Cyc_List_List*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));
_tmp1AB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));
_tmp1AC->vd=_tmpBC;_tmp1AC->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp1AE;_tmp1AE.tag=2;_tmp1AE.f1=_tmp1AA;_tmp1AE;});_tmp1AD;}));_tmp1AC;});
_tmp1AB->tl=_tmp187;_tmp1AB;});goto _LL123;_LL12C:;_LL12D: goto _LL123;_LL123:;}
goto _LL112;_LL121:;_LL122: goto _LL112;_LL112:;}if(_tmp187 != new_relns)_tmp183=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1AF=_cycalloc(sizeof(*
_tmp1AF));_tmp1AF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1B0;_tmp1B0.tag=
0;_tmp1B0.f1=_tmp186;_tmp1B0.f2=new_relns;_tmp1B0;});_tmp1AF;});{void*_tmp1B1=
_tmp184;_LL12F: if((int)_tmp1B1 != 0)goto _LL131;_LL130: return({struct _tuple0
_tmp1B2;_tmp1B2.f1=_tmp183;_tmp1B2.f2=Cyc_CfFlowInfo_unknown_all;_tmp1B2;});
_LL131: if((int)_tmp1B1 != 2)goto _LL133;_LL132: goto _LL134;_LL133: if((int)_tmp1B1 != 
1)goto _LL135;_LL134: goto _LL136;_LL135: if(_tmp1B1 <= (void*)3?1:*((int*)_tmp1B1)!= 
2)goto _LL137;_LL136: {struct Cyc_List_List _tmp1B3=({struct Cyc_List_List _tmp1BC;
_tmp1BC.hd=_tmpBC;_tmp1BC.tl=0;_tmp1BC;});_tmp183=Cyc_NewControlFlow_add_vars(
_tmp183,(struct Cyc_List_List*)& _tmp1B3,Cyc_CfFlowInfo_unknown_all);{void*_tmp1B5;
void*_tmp1B6;struct _tuple0 _tmp1B4=Cyc_NewControlFlow_anal_Rexp(env,_tmp183,
_tmpBE);_tmp1B5=_tmp1B4.f1;_tmp1B6=_tmp1B4.f2;{void*_tmp1B7=_tmp1B5;struct Cyc_Dict_Dict*
_tmp1B8;_LL13A: if((int)_tmp1B7 != 0)goto _LL13C;_LL13B: return({struct _tuple0
_tmp1B9;_tmp1B9.f1=_tmp1B5;_tmp1B9.f2=Cyc_CfFlowInfo_unknown_all;_tmp1B9;});
_LL13C: if(_tmp1B7 <= (void*)1?1:*((int*)_tmp1B7)!= 0)goto _LL139;_tmp1B8=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1B7)->f1;_LL13D: if(Cyc_CfFlowInfo_initlevel(
_tmp1B8,_tmp1B6)!= (void*)2){({void*_tmp1BA[0]={};Cyc_Tcutil_terr(_tmpBD->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp1BA,
sizeof(void*),0));});return({struct _tuple0 _tmp1BB;_tmp1BB.f1=(void*)0;_tmp1BB.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1BB;});}_LL139:;}_tmp183=_tmp1B5;goto _LL138;}}
_LL137:;_LL138: while(1){struct Cyc_List_List _tmp1BD=({struct Cyc_List_List _tmp1C6;
_tmp1C6.hd=_tmpBC;_tmp1C6.tl=0;_tmp1C6;});_tmp183=Cyc_NewControlFlow_add_vars(
_tmp183,(struct Cyc_List_List*)& _tmp1BD,Cyc_CfFlowInfo_unknown_all);{void*_tmp1BF;
void*_tmp1C0;struct _tuple0 _tmp1BE=Cyc_NewControlFlow_anal_Rexp(env,_tmp183,
_tmpBE);_tmp1BF=_tmp1BE.f1;_tmp1C0=_tmp1BE.f2;{void*_tmp1C1=_tmp1BF;struct Cyc_Dict_Dict*
_tmp1C2;_LL13F: if((int)_tmp1C1 != 0)goto _LL141;_LL140: goto _LL13E;_LL141: if(
_tmp1C1 <= (void*)1?1:*((int*)_tmp1C1)!= 0)goto _LL13E;_tmp1C2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1C1)->f1;_LL142: if(Cyc_CfFlowInfo_initlevel(_tmp1C2,_tmp1C0)!= (void*)2){({
void*_tmp1C3[0]={};Cyc_Tcutil_terr(_tmpBD->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp1C3,sizeof(void*),0));});return({struct _tuple0
_tmp1C4;_tmp1C4.f1=(void*)0;_tmp1C4.f2=Cyc_CfFlowInfo_unknown_all;_tmp1C4;});}
_LL13E:;}{void*_tmp1C5=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp183,_tmp1BF);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C5,_tmp183))break;_tmp183=_tmp1C5;}}}
return({struct _tuple0 _tmp1C7;_tmp1C7.f1=_tmp183;_tmp1C7.f2=Cyc_CfFlowInfo_unknown_all;
_tmp1C7;});_LL12E:;}}_LL10D:;}}_LLB7: if(*((int*)_tmp83)!= 35)goto _LLB9;_tmpBF=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp83)->f1;_LLB8: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmpBF))->flow;{void*_tmp1C8=(void*)_tmpBF->r;struct Cyc_Absyn_Stmt*
_tmp1C9;struct Cyc_Absyn_Stmt*_tmp1CA;struct Cyc_Absyn_Decl*_tmp1CB;struct Cyc_Absyn_Stmt*
_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;_LL144: if(_tmp1C8 <= (void*)1?1:*((int*)
_tmp1C8)!= 1)goto _LL146;_tmp1C9=((struct Cyc_Absyn_Seq_s_struct*)_tmp1C8)->f1;
_tmp1CA=((struct Cyc_Absyn_Seq_s_struct*)_tmp1C8)->f2;_LL145: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp1C9);_tmpBF=_tmp1CA;goto _LL143;_LL146: if(_tmp1C8 <= (void*)1?1:*((
int*)_tmp1C8)!= 12)goto _LL148;_tmp1CB=((struct Cyc_Absyn_Decl_s_struct*)_tmp1C8)->f1;
_tmp1CC=((struct Cyc_Absyn_Decl_s_struct*)_tmp1C8)->f2;_LL147: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp1CB);_tmpBF=_tmp1CC;goto _LL143;_LL148: if(_tmp1C8 <= (void*)1?1:*((
int*)_tmp1C8)!= 0)goto _LL14A;_tmp1CD=((struct Cyc_Absyn_Exp_s_struct*)_tmp1C8)->f1;
_LL149: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1CD);_LL14A:;_LL14B:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1CE=_cycalloc(sizeof(*
_tmp1CE));_tmp1CE[0]=({struct Cyc_Core_Impossible_struct _tmp1CF;_tmp1CF.tag=Cyc_Core_Impossible;
_tmp1CF.f1=_tag_arr("analyze_Rexp: ill-formed StmtExp",sizeof(char),33);_tmp1CF;});
_tmp1CE;}));_LL143:;}}_LLB9: if(*((int*)_tmp83)!= 1)goto _LLBB;_tmpC0=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((int)_tmpC0 != 0)goto _LLBB;_LLBA:
goto _LLBC;_LLBB: if(*((int*)_tmp83)!= 2)goto _LLBD;_LLBC: goto _LLBE;_LLBD: if(*((int*)
_tmp83)!= 8)goto _LLBF;_LLBE: goto _LLC0;_LLBF: if(*((int*)_tmp83)!= 34)goto _LLC1;
_LLC0: goto _LLC2;_LLC1: if(*((int*)_tmp83)!= 25)goto _LLC3;_LLC2: goto _LLC4;_LLC3:
if(*((int*)_tmp83)!= 36)goto _LLC5;_LLC4: goto _LLC6;_LLC5: if(*((int*)_tmp83)!= 37)
goto _LL66;_LLC6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1D0=
_cycalloc(sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Core_Impossible_struct _tmp1D1;
_tmp1D1.tag=Cyc_Core_Impossible;_tmp1D1.f1=_tag_arr("anal_Rexp, unexpected exp form",
sizeof(char),31);_tmp1D1;});_tmp1D0;}));_LL66:;}}static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,
void*r,struct Cyc_List_List*flds){void*_tmp1D2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1D3;
void*_tmp1D4;struct Cyc_Absyn_Conref*_tmp1D5;_LL14D: if(_tmp1D2 <= (void*)3?1:*((
int*)_tmp1D2)!= 4)goto _LL14F;_tmp1D3=((struct Cyc_Absyn_PointerType_struct*)
_tmp1D2)->f1;_tmp1D4=(void*)_tmp1D3.elt_typ;_tmp1D5=_tmp1D3.bounds;_LL14E: {void*
_tmp1D6=f;struct Cyc_Dict_Dict*_tmp1D7;struct Cyc_List_List*_tmp1D8;_LL152: if((int)
_tmp1D6 != 0)goto _LL154;_LL153: return({struct _tuple0 _tmp1D9;_tmp1D9.f1=f;_tmp1D9.f2=(
void*)0;_tmp1D9;});_LL154: if(_tmp1D6 <= (void*)1?1:*((int*)_tmp1D6)!= 0)goto
_LL151;_tmp1D7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1D6)->f1;_tmp1D8=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1D6)->f2;_LL155: if(Cyc_Tcutil_is_bound_one(
_tmp1D5)){void*_tmp1DA=r;struct Cyc_CfFlowInfo_Place*_tmp1DB;struct Cyc_CfFlowInfo_Place
_tmp1DC;void*_tmp1DD;struct Cyc_List_List*_tmp1DE;void*_tmp1DF;_LL157: if((int)
_tmp1DA != 1)goto _LL159;_LL158: goto _LL15A;_LL159: if((int)_tmp1DA != 2)goto _LL15B;
_LL15A:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp1E1;_tmp1E1.tag=Cyc_CfFlowInfo_NotZero;_tmp1E1.f1=_tmp1D8;_tmp1E1;});_tmp1E0;})));
goto _LL156;_LL15B: if(_tmp1DA <= (void*)3?1:*((int*)_tmp1DA)!= 2)goto _LL15D;
_tmp1DB=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DA)->f1;_tmp1DC=*_tmp1DB;
_tmp1DD=(void*)_tmp1DC.root;_tmp1DE=_tmp1DC.fields;_LL15C:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));
_tmp1E2[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp1E3;_tmp1E3.tag=Cyc_CfFlowInfo_NotZero;
_tmp1E3.f1=_tmp1D8;_tmp1E3;});_tmp1E2;})));return({struct _tuple0 _tmp1E4;_tmp1E4.f1=
f;_tmp1E4.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp1E6;_tmp1E6.tag=
0;_tmp1E6.f1=({struct Cyc_CfFlowInfo_Place*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));
_tmp1E7->root=(void*)_tmp1DD;_tmp1E7->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1DE,flds);_tmp1E7;});_tmp1E6;});
_tmp1E5;});_tmp1E4;});_LL15D: if((int)_tmp1DA != 0)goto _LL15F;_LL15E:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmp1E8=_cycalloc(sizeof(*
_tmp1E8));_tmp1E8[0]=({struct Cyc_CfFlowInfo_IsZero_struct _tmp1E9;_tmp1E9.tag=Cyc_CfFlowInfo_IsZero;
_tmp1E9.f1=_tmp1D8;_tmp1E9;});_tmp1E8;})));return({struct _tuple0 _tmp1EA;_tmp1EA.f1=(
void*)0;_tmp1EA.f2=(void*)0;_tmp1EA;});_LL15F: if(_tmp1DA <= (void*)3?1:*((int*)
_tmp1DA)!= 0)goto _LL161;_tmp1DF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp1DA)->f1;_LL160: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp1DF);goto
_LL162;_LL161:;_LL162:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp1EC;_tmp1EC.tag=Cyc_CfFlowInfo_UnknownZ;_tmp1EC.f1=_tmp1D8;_tmp1EC;});
_tmp1EB;})));_LL156:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp1EE;_tmp1EE.tag=Cyc_CfFlowInfo_UnknownZ;_tmp1EE.f1=_tmp1D8;_tmp1EE;});
_tmp1ED;})));}if(Cyc_CfFlowInfo_initlevel(_tmp1D7,r)== (void*)0)({void*_tmp1EF[0]={};
Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(char),46),_tag_arr(_tmp1EF,sizeof(void*),0));});return({struct _tuple0
_tmp1F0;_tmp1F0.f1=f;_tmp1F0.f2=(void*)0;_tmp1F0;});_LL151:;}_LL14F:;_LL150:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1F1=_cycalloc(sizeof(*
_tmp1F1));_tmp1F1[0]=({struct Cyc_Core_Impossible_struct _tmp1F2;_tmp1F2.tag=Cyc_Core_Impossible;
_tmp1F2.f1=_tag_arr("left deref of non-pointer-type",sizeof(char),31);_tmp1F2;});
_tmp1F1;}));_LL14C:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp1F3=inflow;struct Cyc_Dict_Dict*_tmp1F4;
struct Cyc_List_List*_tmp1F5;_LL164: if((int)_tmp1F3 != 0)goto _LL166;_LL165: return({
struct _tuple0 _tmp1F6;_tmp1F6.f1=(void*)0;_tmp1F6.f2=(void*)0;_tmp1F6;});_LL166:
if(_tmp1F3 <= (void*)1?1:*((int*)_tmp1F3)!= 0)goto _LL163;_tmp1F4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F3)->f1;_tmp1F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F3)->f2;
_LL167: d=_tmp1F4;_LL163:;}{void*_tmp1F7=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1F8;
struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Vardecl*
_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_Vardecl*_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Vardecl*
_tmp200;void*_tmp201;struct Cyc_Absyn_Vardecl*_tmp202;struct Cyc_Absyn_Exp*_tmp203;
struct _tagged_arr*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct
_tagged_arr*_tmp209;_LL169: if(*((int*)_tmp1F7)!= 13)goto _LL16B;_tmp1F8=((struct
Cyc_Absyn_Cast_e_struct*)_tmp1F7)->f2;_LL16A: _tmp1F9=_tmp1F8;goto _LL16C;_LL16B:
if(*((int*)_tmp1F7)!= 11)goto _LL16D;_tmp1F9=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1F7)->f1;_LL16C: _tmp1FA=_tmp1F9;goto _LL16E;_LL16D: if(*((int*)_tmp1F7)!= 12)
goto _LL16F;_tmp1FA=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F7)->f1;_LL16E:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp1FA,flds);_LL16F: if(*((int*)
_tmp1F7)!= 1)goto _LL171;_tmp1FB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;
if(_tmp1FB <= (void*)1?1:*((int*)_tmp1FB)!= 2)goto _LL171;_tmp1FC=((struct Cyc_Absyn_Param_b_struct*)
_tmp1FB)->f1;_LL170: _tmp1FE=_tmp1FC;goto _LL172;_LL171: if(*((int*)_tmp1F7)!= 1)
goto _LL173;_tmp1FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;if(
_tmp1FD <= (void*)1?1:*((int*)_tmp1FD)!= 3)goto _LL173;_tmp1FE=((struct Cyc_Absyn_Local_b_struct*)
_tmp1FD)->f1;_LL172: _tmp200=_tmp1FE;goto _LL174;_LL173: if(*((int*)_tmp1F7)!= 1)
goto _LL175;_tmp1FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;if(
_tmp1FF <= (void*)1?1:*((int*)_tmp1FF)!= 4)goto _LL175;_tmp200=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1FF)->f1;_LL174: return({struct _tuple0 _tmp20A;_tmp20A.f1=inflow;_tmp20A.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp20B=_cycalloc(sizeof(*_tmp20B));
_tmp20B[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp20C;_tmp20C.tag=0;_tmp20C.f1=({
struct Cyc_CfFlowInfo_Place*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp20E=_cycalloc(sizeof(*
_tmp20E));_tmp20E[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp20F;_tmp20F.tag=0;
_tmp20F.f1=_tmp200;_tmp20F;});_tmp20E;}));_tmp20D->fields=flds;_tmp20D;});
_tmp20C;});_tmp20B;});_tmp20A;});_LL175: if(*((int*)_tmp1F7)!= 1)goto _LL177;
_tmp201=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;if(_tmp201 <= (void*)
1?1:*((int*)_tmp201)!= 0)goto _LL177;_tmp202=((struct Cyc_Absyn_Global_b_struct*)
_tmp201)->f1;_LL176: return({struct _tuple0 _tmp210;_tmp210.f1=inflow;_tmp210.f2=(
void*)0;_tmp210;});_LL177: if(*((int*)_tmp1F7)!= 22)goto _LL179;_tmp203=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp1F7)->f1;_tmp204=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1F7)->f2;_LL178: if(!Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp203->topt))->v))flds=({struct Cyc_List_List*_tmp211=_cycalloc(
sizeof(*_tmp211));_tmp211->hd=_tmp204;_tmp211->tl=flds;_tmp211;});_tmp205=
_tmp203;goto _LL17A;_LL179: if(*((int*)_tmp1F7)!= 20)goto _LL17B;_tmp205=((struct
Cyc_Absyn_Deref_e_struct*)_tmp1F7)->f1;_LL17A: {void*_tmp213;void*_tmp214;struct
_tuple0 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp205);_tmp213=_tmp212.f1;
_tmp214=_tmp212.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp213,
_tmp205,_tmp214,flds);}_LL17B: if(*((int*)_tmp1F7)!= 23)goto _LL17D;_tmp206=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1F7)->f1;_tmp207=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F7)->f2;_LL17C:{void*_tmp215=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp206->topt))->v);_LL182: if(_tmp215 <= (void*)3?1:*((int*)_tmp215)
!= 9)goto _LL184;_LL183: {struct _tagged_arr*_tmp216=Cyc_Absyn_fieldname((int)Cyc_Evexp_eval_const_uint_exp(
_tmp207));return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp206,({struct Cyc_List_List*
_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->hd=_tmp216;_tmp217->tl=flds;_tmp217;}));}
_LL184:;_LL185: goto _LL181;_LL181:;}{void*_tmp21A;struct Cyc_List_List*_tmp21B;
struct _tuple4 _tmp219=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct
Cyc_Absyn_Exp*_tmp218[2];_tmp218[1]=_tmp207;_tmp218[0]=_tmp206;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp218,sizeof(struct Cyc_Absyn_Exp*),
2));}));_tmp21A=_tmp219.f1;_tmp21B=_tmp219.f2;{void*_tmp21C=_tmp21A;struct Cyc_Dict_Dict*
_tmp21D;struct Cyc_List_List*_tmp21E;_LL187: if(_tmp21C <= (void*)1?1:*((int*)
_tmp21C)!= 0)goto _LL189;_tmp21D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp21C)->f1;_tmp21E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp21C)->f2;
_LL188: if(Cyc_CfFlowInfo_initlevel(_tmp21D,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp21B))->tl))->hd)== (void*)0)({
void*_tmp21F[0]={};Cyc_Tcutil_terr(_tmp207->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp21F,sizeof(void*),0));});{struct Cyc_List_List*
_tmp220=Cyc_NewControlFlow_add_subscript_reln(_tmp21E,_tmp206,_tmp207);if(
_tmp21E != _tmp220)_tmp21A=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp222;_tmp222.tag=0;_tmp222.f1=_tmp21D;_tmp222.f2=_tmp220;_tmp222;});_tmp221;});
goto _LL186;}_LL189:;_LL18A: goto _LL186;_LL186:;}return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp21A,_tmp206,(void*)((struct Cyc_List_List*)_check_null(_tmp21B))->hd,
flds);}_LL17D: if(*((int*)_tmp1F7)!= 21)goto _LL17F;_tmp208=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1F7)->f1;_tmp209=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1F7)->f2;_LL17E:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp208->topt))->v))
return({struct _tuple0 _tmp223;_tmp223.f1=inflow;_tmp223.f2=(void*)0;_tmp223;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp208,({struct Cyc_List_List*
_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->hd=_tmp209;_tmp224->tl=flds;_tmp224;}));
_LL17F:;_LL180: return({struct _tuple0 _tmp225;_tmp225.f1=(void*)0;_tmp225.f2=(void*)
0;_tmp225;});_LL168:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp226=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp227;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*
_tmp22A;struct Cyc_Absyn_Exp*_tmp22B;void*_tmp22C;struct Cyc_List_List*_tmp22D;
struct Cyc_List_List _tmp22E;struct Cyc_Absyn_Exp*_tmp22F;struct Cyc_List_List*
_tmp230;void*_tmp231;struct Cyc_List_List*_tmp232;void*_tmp233;struct Cyc_List_List*
_tmp234;void*_tmp235;struct Cyc_List_List*_tmp236;void*_tmp237;struct Cyc_List_List*
_tmp238;_LL18C: if(*((int*)_tmp226)!= 6)goto _LL18E;_tmp227=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp226)->f1;_tmp228=((struct Cyc_Absyn_Conditional_e_struct*)_tmp226)->f2;
_tmp229=((struct Cyc_Absyn_Conditional_e_struct*)_tmp226)->f3;_LL18D: {void*
_tmp23A;void*_tmp23B;struct _tuple0 _tmp239=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp227);_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;{void*_tmp23D;void*_tmp23E;
struct _tuple0 _tmp23C=Cyc_NewControlFlow_anal_test(env,_tmp23A,_tmp228);_tmp23D=
_tmp23C.f1;_tmp23E=_tmp23C.f2;{void*_tmp240;void*_tmp241;struct _tuple0 _tmp23F=
Cyc_NewControlFlow_anal_test(env,_tmp23B,_tmp229);_tmp240=_tmp23F.f1;_tmp241=
_tmp23F.f2;return({struct _tuple0 _tmp242;_tmp242.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp23D,_tmp240);_tmp242.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp23E,
_tmp241);_tmp242;});}}}_LL18E: if(*((int*)_tmp226)!= 7)goto _LL190;_tmp22A=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp226)->f1;_tmp22B=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp226)->f2;_LL18F: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp22A)).f1,_tmp22B);_LL190: if(*((int*)_tmp226)!= 3)goto _LL192;
_tmp22C=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f1;if((int)_tmp22C != 
11)goto _LL192;_tmp22D=((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;if(_tmp22D
== 0)goto _LL192;_tmp22E=*_tmp22D;_tmp22F=(struct Cyc_Absyn_Exp*)_tmp22E.hd;
_tmp230=_tmp22E.tl;if(_tmp230 != 0)goto _LL192;_LL191: {void*_tmp244;void*_tmp245;
struct _tuple0 _tmp243=Cyc_NewControlFlow_anal_test(env,inflow,_tmp22F);_tmp244=
_tmp243.f1;_tmp245=_tmp243.f2;return({struct _tuple0 _tmp246;_tmp246.f1=_tmp245;
_tmp246.f2=_tmp244;_tmp246;});}_LL192: if(*((int*)_tmp226)!= 3)goto _LL194;_tmp231=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f1;if((int)_tmp231 != 5)goto
_LL194;_tmp232=((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;_LL193: {void*
_tmp248;struct Cyc_List_List*_tmp249;struct _tuple4 _tmp247=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp232);_tmp248=_tmp247.f1;_tmp249=_tmp247.f2;{void*_tmp24A=_tmp248;
struct Cyc_Dict_Dict*_tmp24B;struct Cyc_List_List*_tmp24C;_LL19D: if((int)_tmp24A != 
0)goto _LL19F;_LL19E: return({struct _tuple0 _tmp24D;_tmp24D.f1=_tmp248;_tmp24D.f2=
_tmp248;_tmp24D;});_LL19F: if(_tmp24A <= (void*)1?1:*((int*)_tmp24A)!= 0)goto
_LL19C;_tmp24B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24A)->f1;_tmp24C=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24A)->f2;_LL1A0: {void*_tmp24E=(
void*)((struct Cyc_List_List*)_check_null(_tmp249))->hd;void*_tmp24F=(void*)((
struct Cyc_List_List*)_check_null(_tmp249->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp24B,_tmp24E)== (void*)0)({void*_tmp250[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp232))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp250,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp24B,_tmp24F)== (void*)0)({void*_tmp251[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp232))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp251,
sizeof(void*),0));});{struct _tuple0 _tmp253=({struct _tuple0 _tmp252;_tmp252.f1=
_tmp24E;_tmp252.f2=_tmp24F;_tmp252;});void*_tmp254;void*_tmp255;void*_tmp256;
void*_tmp257;void*_tmp258;void*_tmp259;_LL1A2: _tmp254=_tmp253.f1;if(_tmp254 <= (
void*)3?1:*((int*)_tmp254)!= 0)goto _LL1A4;_tmp255=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp254)->f1;_tmp256=_tmp253.f2;if((int)_tmp256 != 0)goto _LL1A4;_LL1A3: {void*
_tmp25B;void*_tmp25C;struct _tuple0 _tmp25A=Cyc_NewControlFlow_splitzero(env,
_tmp248,_tmp248,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp232))->hd,
_tmp255);_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;return({struct _tuple0 _tmp25D;
_tmp25D.f1=_tmp25C;_tmp25D.f2=_tmp25B;_tmp25D;});}_LL1A4: _tmp257=_tmp253.f1;if((
int)_tmp257 != 0)goto _LL1A6;_tmp258=_tmp253.f2;if(_tmp258 <= (void*)3?1:*((int*)
_tmp258)!= 0)goto _LL1A6;_tmp259=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp258)->f1;_LL1A5: {void*_tmp25F;void*_tmp260;struct _tuple0 _tmp25E=Cyc_NewControlFlow_splitzero(
env,_tmp248,_tmp248,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp232))->tl))->hd,_tmp259);_tmp25F=_tmp25E.f1;
_tmp260=_tmp25E.f2;return({struct _tuple0 _tmp261;_tmp261.f1=_tmp260;_tmp261.f2=
_tmp25F;_tmp261;});}_LL1A6:;_LL1A7: return({struct _tuple0 _tmp262;_tmp262.f1=
_tmp248;_tmp262.f2=_tmp248;_tmp262;});_LL1A1:;}}_LL19C:;}}_LL194: if(*((int*)
_tmp226)!= 3)goto _LL196;_tmp233=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f1;if((int)_tmp233 != 6)goto _LL196;_tmp234=((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f2;_LL195: {void*_tmp264;struct Cyc_List_List*_tmp265;struct _tuple4
_tmp263=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp234);_tmp264=
_tmp263.f1;_tmp265=_tmp263.f2;{void*_tmp266=_tmp264;struct Cyc_Dict_Dict*_tmp267;
struct Cyc_List_List*_tmp268;_LL1A9: if((int)_tmp266 != 0)goto _LL1AB;_LL1AA: return({
struct _tuple0 _tmp269;_tmp269.f1=_tmp264;_tmp269.f2=_tmp264;_tmp269;});_LL1AB: if(
_tmp266 <= (void*)1?1:*((int*)_tmp266)!= 0)goto _LL1A8;_tmp267=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp266)->f1;_tmp268=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp266)->f2;
_LL1AC: {void*_tmp26A=(void*)((struct Cyc_List_List*)_check_null(_tmp265))->hd;
void*_tmp26B=(void*)((struct Cyc_List_List*)_check_null(_tmp265->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp267,_tmp26A)== (void*)0)({void*_tmp26C[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp234))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp26C,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp267,_tmp26B)== (void*)0)({void*_tmp26D[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp234))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp26D,
sizeof(void*),0));});{struct _tuple0 _tmp26F=({struct _tuple0 _tmp26E;_tmp26E.f1=
_tmp26A;_tmp26E.f2=_tmp26B;_tmp26E;});void*_tmp270;void*_tmp271;void*_tmp272;
void*_tmp273;void*_tmp274;void*_tmp275;_LL1AE: _tmp270=_tmp26F.f1;if(_tmp270 <= (
void*)3?1:*((int*)_tmp270)!= 0)goto _LL1B0;_tmp271=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp270)->f1;_tmp272=_tmp26F.f2;if((int)_tmp272 != 0)goto _LL1B0;_LL1AF: return Cyc_NewControlFlow_splitzero(
env,_tmp264,_tmp264,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp234))->hd,_tmp271);_LL1B0: _tmp273=_tmp26F.f1;if((int)_tmp273 != 0)goto _LL1B2;
_tmp274=_tmp26F.f2;if(_tmp274 <= (void*)3?1:*((int*)_tmp274)!= 0)goto _LL1B2;
_tmp275=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp274)->f1;_LL1B1:
return Cyc_NewControlFlow_splitzero(env,_tmp264,_tmp264,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp234))->tl))->hd,
_tmp275);_LL1B2:;_LL1B3: return({struct _tuple0 _tmp276;_tmp276.f1=_tmp264;_tmp276.f2=
_tmp264;_tmp276;});_LL1AD:;}}_LL1A8:;}}_LL196: if(*((int*)_tmp226)!= 3)goto _LL198;
_tmp235=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f1;if((int)_tmp235 != 
8)goto _LL198;_tmp236=((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;_LL197: {
void*_tmp278;struct Cyc_List_List*_tmp279;struct _tuple4 _tmp277=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp236);_tmp278=_tmp277.f1;_tmp279=_tmp277.f2;{void*_tmp27A=_tmp278;
struct Cyc_Dict_Dict*_tmp27B;struct Cyc_List_List*_tmp27C;_LL1B5: if((int)_tmp27A != 
0)goto _LL1B7;_LL1B6: return({struct _tuple0 _tmp27D;_tmp27D.f1=_tmp278;_tmp27D.f2=
_tmp278;_tmp27D;});_LL1B7: if(_tmp27A <= (void*)1?1:*((int*)_tmp27A)!= 0)goto
_LL1B4;_tmp27B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp27A)->f1;_tmp27C=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp27A)->f2;_LL1B8: {void*_tmp27E=(
void*)((struct Cyc_List_List*)_check_null(_tmp279))->hd;void*_tmp27F=(void*)((
struct Cyc_List_List*)_check_null(_tmp279->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp27B,_tmp27E)== (void*)0)({void*_tmp280[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp236))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp280,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp27B,_tmp27F)== (void*)0)({void*_tmp281[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp236))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp281,
sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp282=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp236))->hd;struct Cyc_Absyn_Exp*_tmp283=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp236->tl))->hd;void*_tmp284=(void*)_tmp282->r;
void*_tmp285;struct Cyc_Absyn_Vardecl*_tmp286;void*_tmp287;struct Cyc_Absyn_Vardecl*
_tmp288;void*_tmp289;struct Cyc_Absyn_Vardecl*_tmp28A;void*_tmp28B;struct Cyc_Absyn_Vardecl*
_tmp28C;_LL1BA: if(*((int*)_tmp284)!= 1)goto _LL1BC;_tmp285=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp284)->f2;if(_tmp285 <= (void*)1?1:*((int*)_tmp285)!= 0)goto _LL1BC;_tmp286=((
struct Cyc_Absyn_Global_b_struct*)_tmp285)->f1;if(!(!_tmp286->escapes))goto _LL1BC;
_LL1BB: _tmp288=_tmp286;goto _LL1BD;_LL1BC: if(*((int*)_tmp284)!= 1)goto _LL1BE;
_tmp287=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp284)->f2;if(_tmp287 <= (void*)
1?1:*((int*)_tmp287)!= 3)goto _LL1BE;_tmp288=((struct Cyc_Absyn_Local_b_struct*)
_tmp287)->f1;if(!(!_tmp288->escapes))goto _LL1BE;_LL1BD: _tmp28A=_tmp288;goto
_LL1BF;_LL1BE: if(*((int*)_tmp284)!= 1)goto _LL1C0;_tmp289=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp284)->f2;if(_tmp289 <= (void*)1?1:*((int*)_tmp289)!= 4)goto _LL1C0;_tmp28A=((
struct Cyc_Absyn_Pat_b_struct*)_tmp289)->f1;if(!(!_tmp28A->escapes))goto _LL1C0;
_LL1BF: _tmp28C=_tmp28A;goto _LL1C1;_LL1C0: if(*((int*)_tmp284)!= 1)goto _LL1C2;
_tmp28B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp284)->f2;if(_tmp28B <= (void*)
1?1:*((int*)_tmp28B)!= 2)goto _LL1C2;_tmp28C=((struct Cyc_Absyn_Param_b_struct*)
_tmp28B)->f1;if(!(!_tmp28C->escapes))goto _LL1C2;_LL1C1: {void*_tmp28D=(void*)
_tmp283->r;void*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;void*_tmp290;struct Cyc_Absyn_Vardecl*
_tmp291;void*_tmp292;struct Cyc_Absyn_Vardecl*_tmp293;void*_tmp294;struct Cyc_Absyn_Vardecl*
_tmp295;void*_tmp296;int _tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;struct
Cyc_List_List _tmp29A;struct Cyc_Absyn_Exp*_tmp29B;_LL1C5: if(*((int*)_tmp28D)!= 1)
goto _LL1C7;_tmp28E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if(
_tmp28E <= (void*)1?1:*((int*)_tmp28E)!= 0)goto _LL1C7;_tmp28F=((struct Cyc_Absyn_Global_b_struct*)
_tmp28E)->f1;if(!(!_tmp28F->escapes))goto _LL1C7;_LL1C6: _tmp291=_tmp28F;goto
_LL1C8;_LL1C7: if(*((int*)_tmp28D)!= 1)goto _LL1C9;_tmp290=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp290 <= (void*)1?1:*((int*)_tmp290)!= 3)goto _LL1C9;_tmp291=((
struct Cyc_Absyn_Local_b_struct*)_tmp290)->f1;if(!(!_tmp291->escapes))goto _LL1C9;
_LL1C8: _tmp293=_tmp291;goto _LL1CA;_LL1C9: if(*((int*)_tmp28D)!= 1)goto _LL1CB;
_tmp292=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if(_tmp292 <= (void*)
1?1:*((int*)_tmp292)!= 4)goto _LL1CB;_tmp293=((struct Cyc_Absyn_Pat_b_struct*)
_tmp292)->f1;if(!(!_tmp293->escapes))goto _LL1CB;_LL1CA: _tmp295=_tmp293;goto
_LL1CC;_LL1CB: if(*((int*)_tmp28D)!= 1)goto _LL1CD;_tmp294=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp294 <= (void*)1?1:*((int*)_tmp294)!= 2)goto _LL1CD;_tmp295=((
struct Cyc_Absyn_Param_b_struct*)_tmp294)->f1;if(!(!_tmp295->escapes))goto _LL1CD;
_LL1CC:{struct _tuple0 _tmp29D=({struct _tuple0 _tmp29C;_tmp29C.f1=Cyc_Tcutil_compress((
void*)_tmp28C->type);_tmp29C.f2=Cyc_Tcutil_compress((void*)_tmp295->type);
_tmp29C;});void*_tmp29E;void*_tmp29F;void*_tmp2A0;void*_tmp2A1;void*_tmp2A2;void*
_tmp2A3;_LL1D4: _tmp29E=_tmp29D.f1;if(_tmp29E <= (void*)3?1:*((int*)_tmp29E)!= 5)
goto _LL1D6;_tmp29F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp29E)->f1;if((int)
_tmp29F != 1)goto _LL1D6;_tmp2A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp29E)->f2;
if((int)_tmp2A0 != 2)goto _LL1D6;_LL1D5: goto _LL1D3;_LL1D6: _tmp2A1=_tmp29D.f2;if(
_tmp2A1 <= (void*)3?1:*((int*)_tmp2A1)!= 5)goto _LL1D8;_tmp2A2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2A1)->f1;if((int)_tmp2A2 != 1)goto _LL1D8;_tmp2A3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2A1)->f2;if((int)_tmp2A3 != 2)goto _LL1D8;_LL1D7: goto _LL1D3;_LL1D8:;_LL1D9:
return({struct _tuple0 _tmp2A4;_tmp2A4.f1=_tmp278;_tmp2A4.f2=_tmp278;_tmp2A4;});
_LL1D3:;}{struct Cyc_List_List*_tmp2A5=({struct Cyc_List_List*_tmp2A9=_cycalloc(
sizeof(*_tmp2A9));_tmp2A9->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2AA=_cycalloc(
sizeof(*_tmp2AA));_tmp2AA->vd=_tmp28C;_tmp2AA->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp2AC;_tmp2AC.tag=1;_tmp2AC.f1=_tmp295;_tmp2AC;});_tmp2AB;}));_tmp2AA;});
_tmp2A9->tl=_tmp27C;_tmp2A9;});return({struct _tuple0 _tmp2A6;_tmp2A6.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));
_tmp2A7[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2A8;_tmp2A8.tag=0;
_tmp2A8.f1=_tmp27B;_tmp2A8.f2=_tmp2A5;_tmp2A8;});_tmp2A7;});_tmp2A6.f2=_tmp278;
_tmp2A6;});}_LL1CD: if(*((int*)_tmp28D)!= 0)goto _LL1CF;_tmp296=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp28D)->f1;if(_tmp296 <= (void*)1?1:*((int*)_tmp296)!= 2)goto _LL1CF;_tmp297=((
struct Cyc_Absyn_Int_c_struct*)_tmp296)->f2;_LL1CE:{struct _tuple0 _tmp2AE=({struct
_tuple0 _tmp2AD;_tmp2AD.f1=Cyc_Tcutil_compress((void*)_tmp28C->type);_tmp2AD.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp283->topt))->v);
_tmp2AD;});void*_tmp2AF;void*_tmp2B0;void*_tmp2B1;void*_tmp2B2;void*_tmp2B3;void*
_tmp2B4;_LL1DB: _tmp2AF=_tmp2AE.f1;if(_tmp2AF <= (void*)3?1:*((int*)_tmp2AF)!= 5)
goto _LL1DD;_tmp2B0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2AF)->f1;if((int)
_tmp2B0 != 1)goto _LL1DD;_tmp2B1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2AF)->f2;
if((int)_tmp2B1 != 2)goto _LL1DD;_LL1DC: goto _LL1DA;_LL1DD: _tmp2B2=_tmp2AE.f2;if(
_tmp2B2 <= (void*)3?1:*((int*)_tmp2B2)!= 5)goto _LL1DF;_tmp2B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f1;if((int)_tmp2B3 != 1)goto _LL1DF;_tmp2B4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f2;if((int)_tmp2B4 != 2)goto _LL1DF;_LL1DE: goto _LL1DA;_LL1DF:;_LL1E0:
return({struct _tuple0 _tmp2B5;_tmp2B5.f1=_tmp278;_tmp2B5.f2=_tmp278;_tmp2B5;});
_LL1DA:;}{struct Cyc_List_List*_tmp2B6=({struct Cyc_List_List*_tmp2BA=_cycalloc(
sizeof(*_tmp2BA));_tmp2BA->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2BB=_cycalloc(
sizeof(*_tmp2BB));_tmp2BB->vd=_tmp28C;_tmp2BB->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp2BC=_cycalloc_atomic(sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp2BD;_tmp2BD.tag=3;_tmp2BD.f1=(unsigned int)_tmp297;_tmp2BD;});_tmp2BC;}));
_tmp2BB;});_tmp2BA->tl=_tmp27C;_tmp2BA;});return({struct _tuple0 _tmp2B7;_tmp2B7.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2B8=_cycalloc(sizeof(*
_tmp2B8));_tmp2B8[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2B9;_tmp2B9.tag=
0;_tmp2B9.f1=_tmp27B;_tmp2B9.f2=_tmp2B6;_tmp2B9;});_tmp2B8;});_tmp2B7.f2=_tmp278;
_tmp2B7;});}_LL1CF: if(*((int*)_tmp28D)!= 3)goto _LL1D1;_tmp298=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28D)->f1;if((int)_tmp298 != 19)goto _LL1D1;_tmp299=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28D)->f2;if(_tmp299 == 0)goto _LL1D1;_tmp29A=*_tmp299;_tmp29B=(struct Cyc_Absyn_Exp*)
_tmp29A.hd;_LL1D0: {void*_tmp2BE=(void*)_tmp29B->r;void*_tmp2BF;struct Cyc_Absyn_Vardecl*
_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C6;_LL1E2: if(*((int*)_tmp2BE)
!= 1)goto _LL1E4;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BE)->f2;if(
_tmp2BF <= (void*)1?1:*((int*)_tmp2BF)!= 0)goto _LL1E4;_tmp2C0=((struct Cyc_Absyn_Global_b_struct*)
_tmp2BF)->f1;if(!(!_tmp2C0->escapes))goto _LL1E4;_LL1E3: _tmp2C2=_tmp2C0;goto
_LL1E5;_LL1E4: if(*((int*)_tmp2BE)!= 1)goto _LL1E6;_tmp2C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BE)->f2;if(_tmp2C1 <= (void*)1?1:*((int*)_tmp2C1)!= 3)goto _LL1E6;_tmp2C2=((
struct Cyc_Absyn_Local_b_struct*)_tmp2C1)->f1;if(!(!_tmp2C2->escapes))goto _LL1E6;
_LL1E5: _tmp2C4=_tmp2C2;goto _LL1E7;_LL1E6: if(*((int*)_tmp2BE)!= 1)goto _LL1E8;
_tmp2C3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BE)->f2;if(_tmp2C3 <= (void*)
1?1:*((int*)_tmp2C3)!= 4)goto _LL1E8;_tmp2C4=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2C3)->f1;if(!(!_tmp2C4->escapes))goto _LL1E8;_LL1E7: _tmp2C6=_tmp2C4;goto
_LL1E9;_LL1E8: if(*((int*)_tmp2BE)!= 1)goto _LL1EA;_tmp2C5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BE)->f2;if(_tmp2C5 <= (void*)1?1:*((int*)_tmp2C5)!= 2)goto _LL1EA;_tmp2C6=((
struct Cyc_Absyn_Param_b_struct*)_tmp2C5)->f1;if(!(!_tmp2C6->escapes))goto _LL1EA;
_LL1E9: {struct Cyc_List_List*_tmp2C7=({struct Cyc_List_List*_tmp2CB=_cycalloc(
sizeof(*_tmp2CB));_tmp2CB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2CC=_cycalloc(
sizeof(*_tmp2CC));_tmp2CC->vd=_tmp28C;_tmp2CC->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp2CE;_tmp2CE.tag=2;_tmp2CE.f1=_tmp2C6;_tmp2CE;});_tmp2CD;}));_tmp2CC;});
_tmp2CB->tl=_tmp27C;_tmp2CB;});return({struct _tuple0 _tmp2C8;_tmp2C8.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));
_tmp2C9[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2CA;_tmp2CA.tag=0;
_tmp2CA.f1=_tmp27B;_tmp2CA.f2=_tmp2C7;_tmp2CA;});_tmp2C9;});_tmp2C8.f2=_tmp278;
_tmp2C8;});}_LL1EA:;_LL1EB: return({struct _tuple0 _tmp2CF;_tmp2CF.f1=_tmp278;
_tmp2CF.f2=_tmp278;_tmp2CF;});_LL1E1:;}_LL1D1:;_LL1D2: return({struct _tuple0
_tmp2D0;_tmp2D0.f1=_tmp278;_tmp2D0.f2=_tmp278;_tmp2D0;});_LL1C4:;}_LL1C2:;_LL1C3:
return({struct _tuple0 _tmp2D1;_tmp2D1.f1=_tmp278;_tmp2D1.f2=_tmp278;_tmp2D1;});
_LL1B9:;}}_LL1B4:;}}_LL198: if(*((int*)_tmp226)!= 3)goto _LL19A;_tmp237=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f1;if((int)_tmp237 != 10)goto _LL19A;
_tmp238=((struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;_LL199: {void*_tmp2D3;
struct Cyc_List_List*_tmp2D4;struct _tuple4 _tmp2D2=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp238);_tmp2D3=_tmp2D2.f1;_tmp2D4=_tmp2D2.f2;{void*_tmp2D5=_tmp2D3;
struct Cyc_Dict_Dict*_tmp2D6;struct Cyc_List_List*_tmp2D7;_LL1ED: if((int)_tmp2D5 != 
0)goto _LL1EF;_LL1EE: return({struct _tuple0 _tmp2D8;_tmp2D8.f1=_tmp2D3;_tmp2D8.f2=
_tmp2D3;_tmp2D8;});_LL1EF: if(_tmp2D5 <= (void*)1?1:*((int*)_tmp2D5)!= 0)goto
_LL1EC;_tmp2D6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2D5)->f1;_tmp2D7=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2D5)->f2;_LL1F0: {void*_tmp2D9=(
void*)((struct Cyc_List_List*)_check_null(_tmp2D4))->hd;void*_tmp2DA=(void*)((
struct Cyc_List_List*)_check_null(_tmp2D4->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp2D6,_tmp2D9)== (void*)0)({void*_tmp2DB[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp238))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(char),34),_tag_arr(_tmp2DB,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
_tmp2D6,_tmp2DA)== (void*)0)({void*_tmp2DC[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp238))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(_tmp2DC,
sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp2DD=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp238))->hd;struct Cyc_Absyn_Exp*_tmp2DE=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp238->tl))->hd;void*_tmp2DF=(void*)_tmp2DD->r;
void*_tmp2E0;struct Cyc_Absyn_Vardecl*_tmp2E1;void*_tmp2E2;struct Cyc_Absyn_Vardecl*
_tmp2E3;void*_tmp2E4;struct Cyc_Absyn_Vardecl*_tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Vardecl*
_tmp2E7;_LL1F2: if(*((int*)_tmp2DF)!= 1)goto _LL1F4;_tmp2E0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2DF)->f2;if(_tmp2E0 <= (void*)1?1:*((int*)_tmp2E0)!= 0)goto _LL1F4;_tmp2E1=((
struct Cyc_Absyn_Global_b_struct*)_tmp2E0)->f1;if(!(!_tmp2E1->escapes))goto _LL1F4;
_LL1F3: _tmp2E3=_tmp2E1;goto _LL1F5;_LL1F4: if(*((int*)_tmp2DF)!= 1)goto _LL1F6;
_tmp2E2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2DF)->f2;if(_tmp2E2 <= (void*)
1?1:*((int*)_tmp2E2)!= 3)goto _LL1F6;_tmp2E3=((struct Cyc_Absyn_Local_b_struct*)
_tmp2E2)->f1;if(!(!_tmp2E3->escapes))goto _LL1F6;_LL1F5: _tmp2E5=_tmp2E3;goto
_LL1F7;_LL1F6: if(*((int*)_tmp2DF)!= 1)goto _LL1F8;_tmp2E4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2DF)->f2;if(_tmp2E4 <= (void*)1?1:*((int*)_tmp2E4)!= 4)goto _LL1F8;_tmp2E5=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2E4)->f1;if(!(!_tmp2E5->escapes))goto _LL1F8;
_LL1F7: _tmp2E7=_tmp2E5;goto _LL1F9;_LL1F8: if(*((int*)_tmp2DF)!= 1)goto _LL1FA;
_tmp2E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2DF)->f2;if(_tmp2E6 <= (void*)
1?1:*((int*)_tmp2E6)!= 2)goto _LL1FA;_tmp2E7=((struct Cyc_Absyn_Param_b_struct*)
_tmp2E6)->f1;if(!(!_tmp2E7->escapes))goto _LL1FA;_LL1F9: {void*_tmp2E8=(void*)
_tmp2DE->r;void*_tmp2E9;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List _tmp2EB;
struct Cyc_Absyn_Exp*_tmp2EC;_LL1FD: if(*((int*)_tmp2E8)!= 3)goto _LL1FF;_tmp2E9=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E8)->f1;if((int)_tmp2E9 != 19)goto
_LL1FF;_tmp2EA=((struct Cyc_Absyn_Primop_e_struct*)_tmp2E8)->f2;if(_tmp2EA == 0)
goto _LL1FF;_tmp2EB=*_tmp2EA;_tmp2EC=(struct Cyc_Absyn_Exp*)_tmp2EB.hd;_LL1FE: {
void*_tmp2ED=(void*)_tmp2EC->r;void*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;void*
_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Vardecl*
_tmp2F3;void*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;_LL202: if(*((int*)_tmp2ED)
!= 1)goto _LL204;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if(
_tmp2EE <= (void*)1?1:*((int*)_tmp2EE)!= 0)goto _LL204;_tmp2EF=((struct Cyc_Absyn_Global_b_struct*)
_tmp2EE)->f1;if(!(!_tmp2EF->escapes))goto _LL204;_LL203: _tmp2F1=_tmp2EF;goto
_LL205;_LL204: if(*((int*)_tmp2ED)!= 1)goto _LL206;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2ED)->f2;if(_tmp2F0 <= (void*)1?1:*((int*)_tmp2F0)!= 3)goto _LL206;_tmp2F1=((
struct Cyc_Absyn_Local_b_struct*)_tmp2F0)->f1;if(!(!_tmp2F1->escapes))goto _LL206;
_LL205: _tmp2F3=_tmp2F1;goto _LL207;_LL206: if(*((int*)_tmp2ED)!= 1)goto _LL208;
_tmp2F2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if(_tmp2F2 <= (void*)
1?1:*((int*)_tmp2F2)!= 4)goto _LL208;_tmp2F3=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2F2)->f1;if(!(!_tmp2F3->escapes))goto _LL208;_LL207: _tmp2F5=_tmp2F3;goto
_LL209;_LL208: if(*((int*)_tmp2ED)!= 1)goto _LL20A;_tmp2F4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2ED)->f2;if(_tmp2F4 <= (void*)1?1:*((int*)_tmp2F4)!= 2)goto _LL20A;_tmp2F5=((
struct Cyc_Absyn_Param_b_struct*)_tmp2F4)->f1;if(!(!_tmp2F5->escapes))goto _LL20A;
_LL209: {struct Cyc_List_List*_tmp2F6=({struct Cyc_List_List*_tmp2FA=_cycalloc(
sizeof(*_tmp2FA));_tmp2FA->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2FB=_cycalloc(
sizeof(*_tmp2FB));_tmp2FB->vd=_tmp2E7;_tmp2FB->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp2FD;_tmp2FD.tag=4;_tmp2FD.f1=_tmp2F5;_tmp2FD;});_tmp2FC;}));_tmp2FB;});
_tmp2FA->tl=_tmp2D7;_tmp2FA;});return({struct _tuple0 _tmp2F7;_tmp2F7.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));
_tmp2F8[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2F9;_tmp2F9.tag=0;
_tmp2F9.f1=_tmp2D6;_tmp2F9.f2=_tmp2F6;_tmp2F9;});_tmp2F8;});_tmp2F7.f2=_tmp2D3;
_tmp2F7;});}_LL20A:;_LL20B: return({struct _tuple0 _tmp2FE;_tmp2FE.f1=_tmp2D3;
_tmp2FE.f2=_tmp2D3;_tmp2FE;});_LL201:;}_LL1FF:;_LL200: return({struct _tuple0
_tmp2FF;_tmp2FF.f1=_tmp2D3;_tmp2FF.f2=_tmp2D3;_tmp2FF;});_LL1FC:;}_LL1FA:;_LL1FB:
return({struct _tuple0 _tmp300;_tmp300.f1=_tmp2D3;_tmp300.f2=_tmp2D3;_tmp300;});
_LL1F1:;}}_LL1EC:;}}_LL19A:;_LL19B: goto _LL18B;_LL18B:;}{void*_tmp302;void*
_tmp303;struct _tuple0 _tmp301=Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_tmp302=
_tmp301.f1;_tmp303=_tmp301.f2;{void*_tmp304=_tmp302;struct Cyc_Dict_Dict*_tmp305;
struct Cyc_List_List*_tmp306;_LL20D: if((int)_tmp304 != 0)goto _LL20F;_LL20E: return({
struct _tuple0 _tmp307;_tmp307.f1=_tmp302;_tmp307.f2=_tmp302;_tmp307;});_LL20F: if(
_tmp304 <= (void*)1?1:*((int*)_tmp304)!= 0)goto _LL20C;_tmp305=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp304)->f1;_tmp306=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp304)->f2;
_LL210: {void*_tmp308=_tmp303;void*_tmp309;void*_tmp30A;void*_tmp30B;_LL212: if((
int)_tmp308 != 0)goto _LL214;_LL213: return({struct _tuple0 _tmp30C;_tmp30C.f1=(void*)
0;_tmp30C.f2=_tmp302;_tmp30C;});_LL214: if((int)_tmp308 != 2)goto _LL216;_LL215:
goto _LL217;_LL216: if((int)_tmp308 != 1)goto _LL218;_LL217: goto _LL219;_LL218: if(
_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 2)goto _LL21A;_LL219: return({struct
_tuple0 _tmp30D;_tmp30D.f1=_tmp302;_tmp30D.f2=(void*)0;_tmp30D;});_LL21A: if(
_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 0)goto _LL21C;_tmp309=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp308)->f1;if((int)_tmp309 != 0)goto _LL21C;_LL21B: goto _LL21D;_LL21C: if(_tmp308
<= (void*)3?1:*((int*)_tmp308)!= 1)goto _LL21E;_tmp30A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp308)->f1;if((int)_tmp30A != 0)goto _LL21E;_LL21D:({void*_tmp30E[0]={};Cyc_Tcutil_terr(
e->loc,_tag_arr("expression may not be initialized",sizeof(char),34),_tag_arr(
_tmp30E,sizeof(void*),0));});return({struct _tuple0 _tmp30F;_tmp30F.f1=(void*)0;
_tmp30F.f2=(void*)0;_tmp30F;});_LL21E: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 
0)goto _LL220;_tmp30B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp308)->f1;
_LL21F: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp302,e,_tmp30B);_LL220:
if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 1)goto _LL222;_LL221: return({struct
_tuple0 _tmp310;_tmp310.f1=_tmp302;_tmp310.f2=_tmp302;_tmp310;});_LL222: if(
_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 3)goto _LL211;_LL223:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311[0]=({
struct Cyc_Core_Impossible_struct _tmp312;_tmp312.tag=Cyc_Core_Impossible;_tmp312.f1=
_tag_arr("anal_test",sizeof(char),10);_tmp312;});_tmp311;}));_LL211:;}_LL20C:;}}}
static void*Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp314;struct Cyc_Core_Opt*_tmp315;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Absyn_Stmt*
_tmp317;struct Cyc_Absyn_Switch_clause*_tmp313=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp314=*_tmp313;_tmp315=_tmp314.pat_vars;_tmp316=_tmp314.where_clause;
_tmp317=_tmp314.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp315))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp316 != 0){void*_tmp319;void*_tmp31A;struct _tuple0
_tmp318=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp316));_tmp319=_tmp318.f1;_tmp31A=_tmp318.f2;inflow=_tmp31A;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp319,_tmp317);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp317);}{void*
_tmp31B=clause_outflow;_LL225: if((int)_tmp31B != 0)goto _LL227;_LL226: goto _LL224;
_LL227:;_LL228: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp31C[0]={};Cyc_Tcutil_terr(
_tmp317->loc,_tag_arr("switch clause may implicitly fallthru",sizeof(char),38),
_tag_arr(_tmp31C,sizeof(void*),0));});else{({void*_tmp31D[0]={};Cyc_Tcutil_warn(
_tmp317->loc,_tag_arr("switch clause may implicitly fallthru",sizeof(char),38),
_tag_arr(_tmp31D,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL224;_LL224:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp31E=Cyc_NewControlFlow_pre_stmt_check(env,
inflow,s);inflow=(void*)_tmp31E->flow;{void*_tmp31F=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp320;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Stmt*
_tmp323;struct Cyc_Absyn_Stmt*_tmp324;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Stmt*
_tmp326;struct Cyc_Absyn_Stmt*_tmp327;struct _tuple3 _tmp328;struct Cyc_Absyn_Exp*
_tmp329;struct Cyc_Absyn_Stmt*_tmp32A;struct Cyc_Absyn_Stmt*_tmp32B;struct Cyc_Absyn_Stmt*
_tmp32C;struct _tuple3 _tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Stmt*
_tmp32F;struct Cyc_Absyn_Exp*_tmp330;struct _tuple3 _tmp331;struct Cyc_Absyn_Exp*
_tmp332;struct Cyc_Absyn_Stmt*_tmp333;struct _tuple3 _tmp334;struct Cyc_Absyn_Exp*
_tmp335;struct Cyc_Absyn_Stmt*_tmp336;struct Cyc_Absyn_Stmt*_tmp337;struct Cyc_Absyn_Stmt*
_tmp338;struct Cyc_List_List*_tmp339;struct Cyc_Absyn_Switch_clause**_tmp33A;
struct Cyc_Absyn_Switch_clause*_tmp33B;struct Cyc_Absyn_Stmt*_tmp33C;struct Cyc_Absyn_Stmt*
_tmp33D;struct Cyc_Absyn_Stmt*_tmp33E;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_List_List*
_tmp340;struct Cyc_Absyn_Stmt*_tmp341;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Decl*
_tmp343;struct Cyc_Absyn_Stmt*_tmp344;struct Cyc_Absyn_Stmt*_tmp345;struct Cyc_Absyn_Tvar*
_tmp346;struct Cyc_Absyn_Vardecl*_tmp347;int _tmp348;struct Cyc_Absyn_Stmt*_tmp349;
struct Cyc_Absyn_Exp*_tmp34A;_LL22A: if((int)_tmp31F != 0)goto _LL22C;_LL22B: return
inflow;_LL22C: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 2)goto _LL22E;_tmp320=((
struct Cyc_Absyn_Return_s_struct*)_tmp31F)->f1;if(_tmp320 != 0)goto _LL22E;_LL22D:
if(env->noreturn)({void*_tmp34B[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(char),33),_tag_arr(_tmp34B,sizeof(void*),0));});return(void*)0;_LL22E: if(
_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 2)goto _LL230;_tmp321=((struct Cyc_Absyn_Return_s_struct*)
_tmp31F)->f1;_LL22F: if(env->noreturn)({void*_tmp34C[0]={};Cyc_Tcutil_terr(s->loc,
_tag_arr("`noreturn' function might return",sizeof(char),33),_tag_arr(_tmp34C,
sizeof(void*),0));});{void*_tmp34E;void*_tmp34F;struct _tuple0 _tmp34D=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp321));_tmp34E=_tmp34D.f1;
_tmp34F=_tmp34D.f2;Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)
_check_null(_tmp321))->loc,_tmp34E,_tmp34F);return(void*)0;}_LL230: if(_tmp31F <= (
void*)1?1:*((int*)_tmp31F)!= 0)goto _LL232;_tmp322=((struct Cyc_Absyn_Exp_s_struct*)
_tmp31F)->f1;_LL231: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp322)).f1;
_LL232: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 1)goto _LL234;_tmp323=((struct
Cyc_Absyn_Seq_s_struct*)_tmp31F)->f1;_tmp324=((struct Cyc_Absyn_Seq_s_struct*)
_tmp31F)->f2;_LL233: return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp323),_tmp324);_LL234: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 3)
goto _LL236;_tmp325=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp31F)->f1;_tmp326=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp31F)->f2;_tmp327=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp31F)->f3;_LL235: {void*_tmp351;void*_tmp352;struct _tuple0 _tmp350=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp325);_tmp351=_tmp350.f1;_tmp352=_tmp350.f2;return Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_anal_stmt(env,_tmp351,_tmp326),Cyc_NewControlFlow_anal_stmt(
env,_tmp352,_tmp327));}_LL236: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 4)goto
_LL238;_tmp328=((struct Cyc_Absyn_While_s_struct*)_tmp31F)->f1;_tmp329=_tmp328.f1;
_tmp32A=_tmp328.f2;_tmp32B=((struct Cyc_Absyn_While_s_struct*)_tmp31F)->f2;_LL237: {
void*_tmp353=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp32A))->flow;
void*_tmp355;void*_tmp356;struct _tuple0 _tmp354=Cyc_NewControlFlow_anal_test(env,
_tmp353,_tmp329);_tmp355=_tmp354.f1;_tmp356=_tmp354.f2;{void*_tmp357=Cyc_NewControlFlow_anal_stmt(
env,_tmp355,_tmp32B);Cyc_NewControlFlow_update_flow(env,_tmp32A,_tmp357);return
_tmp356;}}_LL238: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 16)goto _LL23A;
_tmp32C=((struct Cyc_Absyn_Do_s_struct*)_tmp31F)->f1;_tmp32D=((struct Cyc_Absyn_Do_s_struct*)
_tmp31F)->f2;_tmp32E=_tmp32D.f1;_tmp32F=_tmp32D.f2;_LL239: {void*_tmp358=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp32C);void*_tmp359=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp358,_tmp32F))->flow;void*_tmp35B;void*_tmp35C;struct _tuple0 _tmp35A=Cyc_NewControlFlow_anal_test(
env,_tmp359,_tmp32E);_tmp35B=_tmp35A.f1;_tmp35C=_tmp35A.f2;Cyc_NewControlFlow_update_flow(
env,_tmp32C,_tmp35B);return _tmp35C;}_LL23A: if(_tmp31F <= (void*)1?1:*((int*)
_tmp31F)!= 8)goto _LL23C;_tmp330=((struct Cyc_Absyn_For_s_struct*)_tmp31F)->f1;
_tmp331=((struct Cyc_Absyn_For_s_struct*)_tmp31F)->f2;_tmp332=_tmp331.f1;_tmp333=
_tmp331.f2;_tmp334=((struct Cyc_Absyn_For_s_struct*)_tmp31F)->f3;_tmp335=_tmp334.f1;
_tmp336=_tmp334.f2;_tmp337=((struct Cyc_Absyn_For_s_struct*)_tmp31F)->f4;_LL23B: {
void*_tmp35D=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp330)).f1;void*_tmp35E=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp35D,_tmp333))->flow;void*
_tmp360;void*_tmp361;struct _tuple0 _tmp35F=Cyc_NewControlFlow_anal_test(env,
_tmp35E,_tmp332);_tmp360=_tmp35F.f1;_tmp361=_tmp35F.f2;{void*_tmp362=Cyc_NewControlFlow_anal_stmt(
env,_tmp360,_tmp337);void*_tmp363=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp362,_tmp336))->flow;void*_tmp364=(Cyc_NewControlFlow_anal_Rexp(env,_tmp363,
_tmp335)).f1;Cyc_NewControlFlow_update_flow(env,_tmp333,_tmp364);return _tmp361;}}
_LL23C: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 5)goto _LL23E;_tmp338=((struct
Cyc_Absyn_Break_s_struct*)_tmp31F)->f1;if(_tmp338 != 0)goto _LL23E;_LL23D: return(
void*)0;_LL23E: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 11)goto _LL240;_tmp339=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp31F)->f1;_tmp33A=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp31F)->f2;if(_tmp33A == 0)goto _LL240;_tmp33B=*_tmp33A;_LL23F: {void*_tmp366;
struct Cyc_List_List*_tmp367;struct _tuple4 _tmp365=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp339);_tmp366=_tmp365.f1;_tmp367=_tmp365.f2;for(0;_tmp367 != 0;(
_tmp367=_tmp367->tl,_tmp339=_tmp339->tl)){_tmp366=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp339))->hd)->loc,
_tmp366,(void*)_tmp367->hd);}_tmp366=Cyc_NewControlFlow_add_vars(_tmp366,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33B->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp33B->body,_tmp366);
return(void*)0;}_LL240: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 5)goto _LL242;
_tmp33C=((struct Cyc_Absyn_Break_s_struct*)_tmp31F)->f1;_LL241: _tmp33D=_tmp33C;
goto _LL243;_LL242: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 6)goto _LL244;
_tmp33D=((struct Cyc_Absyn_Continue_s_struct*)_tmp31F)->f1;_LL243: _tmp33E=_tmp33D;
goto _LL245;_LL244: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 7)goto _LL246;
_tmp33E=((struct Cyc_Absyn_Goto_s_struct*)_tmp31F)->f2;_LL245: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp368=_tmp31E->encloser;struct Cyc_Absyn_Stmt*
_tmp369=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp33E)))->encloser;while(_tmp369 != _tmp368){struct Cyc_Absyn_Stmt*_tmp36A=(Cyc_NewControlFlow_get_stmt_annot(
_tmp368))->encloser;if(_tmp36A == _tmp368){({void*_tmp36B[0]={};Cyc_Tcutil_terr(s->loc,
_tag_arr("goto enters local scope or exception handler",sizeof(char),45),
_tag_arr(_tmp36B,sizeof(void*),0));});break;}_tmp368=_tmp36A;}}Cyc_NewControlFlow_update_flow(
env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp33E),inflow);return(void*)0;_LL246:
if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 9)goto _LL248;_tmp33F=((struct Cyc_Absyn_Switch_s_struct*)
_tmp31F)->f1;_tmp340=((struct Cyc_Absyn_Switch_s_struct*)_tmp31F)->f2;_LL247: {
void*_tmp36D;void*_tmp36E;struct _tuple0 _tmp36C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp33F);_tmp36D=_tmp36C.f1;_tmp36E=_tmp36C.f2;_tmp36D=Cyc_NewControlFlow_use_Rval(
env,_tmp33F->loc,_tmp36D,_tmp36E);return Cyc_NewControlFlow_anal_scs(env,_tmp36D,
_tmp340);}_LL248: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 17)goto _LL24A;
_tmp341=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp31F)->f1;_tmp342=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp31F)->f2;_LL249: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp341);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp342);{
void*_tmp36F=scs_outflow;_LL255: if((int)_tmp36F != 0)goto _LL257;_LL256: goto _LL254;
_LL257:;_LL258:({void*_tmp370[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("last catch clause may implicitly fallthru",
sizeof(char),42),_tag_arr(_tmp370,sizeof(void*),0));});_LL254:;}return s1_outflow;}}}
_LL24A: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 12)goto _LL24C;_tmp343=((struct
Cyc_Absyn_Decl_s_struct*)_tmp31F)->f1;_tmp344=((struct Cyc_Absyn_Decl_s_struct*)
_tmp31F)->f2;_LL24B: return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp343),_tmp344);_LL24C: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 
15)goto _LL24E;_tmp345=((struct Cyc_Absyn_Label_s_struct*)_tmp31F)->f2;_LL24D:
return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp345);_LL24E: if(_tmp31F <= (void*)
1?1:*((int*)_tmp31F)!= 18)goto _LL250;_tmp346=((struct Cyc_Absyn_Region_s_struct*)
_tmp31F)->f1;_tmp347=((struct Cyc_Absyn_Region_s_struct*)_tmp31F)->f2;_tmp348=((
struct Cyc_Absyn_Region_s_struct*)_tmp31F)->f3;_tmp349=((struct Cyc_Absyn_Region_s_struct*)
_tmp31F)->f4;_LL24F: {struct Cyc_List_List _tmp371=({struct Cyc_List_List _tmp372;
_tmp372.hd=_tmp347;_tmp372.tl=0;_tmp372;});inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)& _tmp371,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp349);}_LL250: if(_tmp31F <= (void*)1?1:*((int*)_tmp31F)!= 20)goto
_LL252;_tmp34A=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp31F)->f1;_LL251: {
void*_tmp374;void*_tmp375;struct _tuple0 _tmp373=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp34A);_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;{void*_tmp376=Cyc_NewControlFlow_use_Rval(
env,_tmp34A->loc,_tmp374,_tmp375);void*_tmp377=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp34A->topt))->v);void*_tmp378;_LL25A: if(
_tmp377 <= (void*)3?1:*((int*)_tmp377)!= 15)goto _LL25C;_tmp378=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp377)->f1;_LL25B: return Cyc_CfFlowInfo_kill_flow_region(_tmp374,_tmp378);
_LL25C:;_LL25D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp379=
_cycalloc(sizeof(*_tmp379));_tmp379[0]=({struct Cyc_Core_Impossible_struct _tmp37A;
_tmp37A.tag=Cyc_Core_Impossible;_tmp37A.f1=_tag_arr("anal_stmt -- reset_region",
sizeof(char),26);_tmp37A;});_tmp379;}));_LL259:;}}_LL252:;_LL253:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp37B=_cycalloc(sizeof(*_tmp37B));
_tmp37B[0]=({struct Cyc_Core_Impossible_struct _tmp37C;_tmp37C.tag=Cyc_Core_Impossible;
_tmp37C.f1=_tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof(char),56);_tmp37C;});_tmp37B;}));_LL229:;}}static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Decl*decl){void*
_tmp37D=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp37E;struct Cyc_Core_Opt*
_tmp37F;struct Cyc_Core_Opt _tmp380;struct Cyc_List_List*_tmp381;struct Cyc_Absyn_Exp*
_tmp382;struct Cyc_List_List*_tmp383;_LL25F: if(*((int*)_tmp37D)!= 0)goto _LL261;
_tmp37E=((struct Cyc_Absyn_Var_d_struct*)_tmp37D)->f1;_LL260: {struct Cyc_List_List
_tmp384=({struct Cyc_List_List _tmp392;_tmp392.hd=_tmp37E;_tmp392.tl=0;_tmp392;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp384,Cyc_CfFlowInfo_unknown_none);{
struct Cyc_Absyn_Exp*_tmp385=_tmp37E->initializer;if(_tmp385 == 0)return inflow;{
void*_tmp387;void*_tmp388;struct _tuple0 _tmp386=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp385));_tmp387=_tmp386.f1;_tmp388=
_tmp386.f2;{void*_tmp389=_tmp387;struct Cyc_Dict_Dict*_tmp38A;struct Cyc_List_List*
_tmp38B;_LL268: if((int)_tmp389 != 0)goto _LL26A;_LL269: return(void*)0;_LL26A: if(
_tmp389 <= (void*)1?1:*((int*)_tmp389)!= 0)goto _LL267;_tmp38A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp389)->f1;_tmp38B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp389)->f2;
_LL26B: _tmp38A=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp38A,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp38D=_cycalloc(sizeof(*
_tmp38D));_tmp38D[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp38E;_tmp38E.tag=0;
_tmp38E.f1=_tmp37E;_tmp38E;});_tmp38D;}));_tmp38C->fields=0;_tmp38C;}),_tmp388);
_tmp38B=Cyc_CfFlowInfo_reln_assign_var(_tmp38B,_tmp37E,(struct Cyc_Absyn_Exp*)
_check_null(_tmp37E->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp38F=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp390=_cycalloc(sizeof(*
_tmp390));_tmp390[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp391;_tmp391.tag=
0;_tmp391.f1=_tmp38A;_tmp391.f2=_tmp38B;_tmp391;});_tmp390;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp38F);return(void*)_tmp38F;}_LL267:;}}}}_LL261: if(*((int*)_tmp37D)
!= 2)goto _LL263;_tmp37F=((struct Cyc_Absyn_Let_d_struct*)_tmp37D)->f2;if(_tmp37F
== 0)goto _LL263;_tmp380=*_tmp37F;_tmp381=(struct Cyc_List_List*)_tmp380.v;_tmp382=((
struct Cyc_Absyn_Let_d_struct*)_tmp37D)->f4;_LL262: {void*_tmp394;void*_tmp395;
struct _tuple0 _tmp393=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp382);_tmp394=
_tmp393.f1;_tmp395=_tmp393.f2;_tmp394=Cyc_NewControlFlow_use_Rval(env,_tmp382->loc,
_tmp394,_tmp395);return Cyc_NewControlFlow_add_vars(_tmp394,_tmp381,Cyc_CfFlowInfo_unknown_all);}
_LL263: if(*((int*)_tmp37D)!= 3)goto _LL265;_tmp383=((struct Cyc_Absyn_Letv_d_struct*)
_tmp37D)->f1;_LL264: return Cyc_NewControlFlow_add_vars(inflow,_tmp383,Cyc_CfFlowInfo_unknown_none);
_LL265:;_LL266:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp396=
_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_Core_Impossible_struct _tmp397;
_tmp397.tag=Cyc_Core_Impossible;_tmp397.f1=_tag_arr("anal_decl: unexpected decl variant",
sizeof(char),35);_tmp397;});_tmp396;}));_LL25E:;}static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt*s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{
void*_tmp398=(void*)s->r;struct _tuple3 _tmp399;struct Cyc_Absyn_Stmt*_tmp39A;
struct _tuple3 _tmp39B;struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Stmt*_tmp39D;
struct Cyc_Absyn_Stmt*_tmp39E;struct Cyc_Absyn_Stmt*_tmp39F;struct _tuple3 _tmp3A0;
struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_Absyn_Stmt*
_tmp3A3;struct _tuple3 _tmp3A4;struct Cyc_Absyn_Stmt*_tmp3A5;struct Cyc_Absyn_Stmt*
_tmp3A6;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Stmt*
_tmp3A9;struct Cyc_Absyn_Stmt*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3AB;struct Cyc_List_List*
_tmp3AC;struct Cyc_List_List*_tmp3AD;_LL26D: if(_tmp398 <= (void*)1?1:*((int*)
_tmp398)!= 8)goto _LL26F;_tmp399=((struct Cyc_Absyn_For_s_struct*)_tmp398)->f2;
_tmp39A=_tmp399.f2;_tmp39B=((struct Cyc_Absyn_For_s_struct*)_tmp398)->f3;_tmp39C=
_tmp39B.f2;_tmp39D=((struct Cyc_Absyn_For_s_struct*)_tmp398)->f4;_LL26E: Cyc_NewControlFlow_postpass(
_tmp39C);_tmp39E=_tmp39D;_tmp39F=_tmp39A;goto _LL270;_LL26F: if(_tmp398 <= (void*)1?
1:*((int*)_tmp398)!= 1)goto _LL271;_tmp39E=((struct Cyc_Absyn_Seq_s_struct*)
_tmp398)->f1;_tmp39F=((struct Cyc_Absyn_Seq_s_struct*)_tmp398)->f2;_LL270: _tmp3A1=
_tmp39E;_tmp3A2=_tmp39F;goto _LL272;_LL271: if(_tmp398 <= (void*)1?1:*((int*)
_tmp398)!= 4)goto _LL273;_tmp3A0=((struct Cyc_Absyn_While_s_struct*)_tmp398)->f1;
_tmp3A1=_tmp3A0.f2;_tmp3A2=((struct Cyc_Absyn_While_s_struct*)_tmp398)->f2;_LL272:
_tmp3A3=_tmp3A2;_tmp3A5=_tmp3A1;goto _LL274;_LL273: if(_tmp398 <= (void*)1?1:*((int*)
_tmp398)!= 16)goto _LL275;_tmp3A3=((struct Cyc_Absyn_Do_s_struct*)_tmp398)->f1;
_tmp3A4=((struct Cyc_Absyn_Do_s_struct*)_tmp398)->f2;_tmp3A5=_tmp3A4.f2;_LL274:
_tmp3A6=_tmp3A3;_tmp3A7=_tmp3A5;goto _LL276;_LL275: if(_tmp398 <= (void*)1?1:*((int*)
_tmp398)!= 3)goto _LL277;_tmp3A6=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp398)->f2;
_tmp3A7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp398)->f3;_LL276: Cyc_NewControlFlow_postpass(
_tmp3A6);_tmp3A8=_tmp3A7;goto _LL278;_LL277: if(_tmp398 <= (void*)1?1:*((int*)
_tmp398)!= 18)goto _LL279;_tmp3A8=((struct Cyc_Absyn_Region_s_struct*)_tmp398)->f4;
_LL278: _tmp3A9=_tmp3A8;goto _LL27A;_LL279: if(_tmp398 <= (void*)1?1:*((int*)_tmp398)
!= 15)goto _LL27B;_tmp3A9=((struct Cyc_Absyn_Label_s_struct*)_tmp398)->f2;_LL27A:
_tmp3AA=_tmp3A9;goto _LL27C;_LL27B: if(_tmp398 <= (void*)1?1:*((int*)_tmp398)!= 12)
goto _LL27D;_tmp3AA=((struct Cyc_Absyn_Decl_s_struct*)_tmp398)->f2;_LL27C: Cyc_NewControlFlow_postpass(
_tmp3AA);return;_LL27D: if(_tmp398 <= (void*)1?1:*((int*)_tmp398)!= 17)goto _LL27F;
_tmp3AB=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp398)->f1;_tmp3AC=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp398)->f2;_LL27E: Cyc_NewControlFlow_postpass(_tmp3AB);_tmp3AD=_tmp3AC;goto
_LL280;_LL27F: if(_tmp398 <= (void*)1?1:*((int*)_tmp398)!= 9)goto _LL281;_tmp3AD=((
struct Cyc_Absyn_Switch_s_struct*)_tmp398)->f2;_LL280: for(0;_tmp3AD != 0;_tmp3AD=
_tmp3AD->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp3AD->hd)->body);}return;_LL281:;_LL282: return;_LL26C:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){void*inflow=Cyc_NewControlFlow_add_vars((void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));
_tmp3B4[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3B5;_tmp3B5.tag=0;
_tmp3B5.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp3B5.f2=0;_tmp3B5;});
_tmp3B4;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->iterate_again=1;_tmp3B3->iteration_num=
0;_tmp3B3->in_try=0;_tmp3B3->tryflow=(void*)inflow;_tmp3B3->all_changed=0;
_tmp3B3->noreturn=noreturn;_tmp3B3;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp3AE=outflow;_LL284: if((int)_tmp3AE != 0)goto
_LL286;_LL285: goto _LL283;_LL286:;_LL287: if(noreturn)({void*_tmp3AF[0]={};Cyc_Tcutil_terr((
fd->body)->loc,_tag_arr("`noreturn' function might (implicitly) return",sizeof(
char),46),_tag_arr(_tmp3AF,sizeof(void*),0));});else{{void*_tmp3B0=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL289: if((int)_tmp3B0 != 0)goto _LL28B;_LL28A: goto _LL288;
_LL28B: if(_tmp3B0 <= (void*)3?1:*((int*)_tmp3B0)!= 6)goto _LL28D;_LL28C: goto _LL28E;
_LL28D: if((int)_tmp3B0 != 1)goto _LL28F;_LL28E: goto _LL290;_LL28F: if(_tmp3B0 <= (
void*)3?1:*((int*)_tmp3B0)!= 5)goto _LL291;_LL290:({void*_tmp3B1[0]={};Cyc_Tcutil_warn((
fd->body)->loc,_tag_arr("function may not return a value",sizeof(char),32),
_tag_arr(_tmp3B1,sizeof(void*),0));});goto _LL288;_LL291:;_LL292:({void*_tmp3B2[0]={};
Cyc_Tcutil_terr((fd->body)->loc,_tag_arr("function may not return a value",
sizeof(char),32),_tag_arr(_tmp3B2,sizeof(void*),0));});goto _LL288;_LL288:;}goto
_LL283;}_LL283:;}Cyc_NewControlFlow_postpass(fd->body);}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp3B6=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp3B7;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*
_tmp3B9;struct Cyc_List_List*_tmp3BA;_LL294: if(*((int*)_tmp3B6)!= 1)goto _LL296;
_tmp3B7=((struct Cyc_Absyn_Fn_d_struct*)_tmp3B6)->f1;_LL295: Cyc_NewControlFlow_check_fun(
_tmp3B7);goto _LL293;_LL296: if(*((int*)_tmp3B6)!= 10)goto _LL298;_tmp3B8=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp3B6)->f1;_LL297: _tmp3B9=_tmp3B8;goto _LL299;
_LL298: if(*((int*)_tmp3B6)!= 9)goto _LL29A;_tmp3B9=((struct Cyc_Absyn_Using_d_struct*)
_tmp3B6)->f2;_LL299: _tmp3BA=_tmp3B9;goto _LL29B;_LL29A: if(*((int*)_tmp3B6)!= 8)
goto _LL29C;_tmp3BA=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3B6)->f2;_LL29B: Cyc_NewControlFlow_cf_check(
_tmp3BA);goto _LL293;_LL29C:;_LL29D: goto _LL293;_LL293:;}}
