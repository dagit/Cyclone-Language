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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*
x);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_Std_strptrcmp(
struct _tagged_arr*s1,struct _tagged_arr*s2);int Cyc_Std_zstrcmp(struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple2 condition;
struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();void*Cyc_Absyn_compress_kb(void*);
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);int Cyc_Absyn_fntype_att(void*a);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;typedef struct{int
__pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t Cyc_Std_fpos_t;
struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d);
int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);
struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d);struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_filter_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Iter_Iter
Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*
te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcgenrep_RepInfo;
struct Cyc_Dict_Dict*Cyc_Tcgenrep_empty_typerep_dict();struct _tuple5{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple5 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict);static char _tmp0[1]="";static struct
_tagged_arr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};static struct _tagged_arr*Cyc_Tc_tc_msg=(
struct _tagged_arr*)& Cyc_Tc_tc_msg_c;static struct Cyc_List_List*Cyc_Tc_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp2;struct Cyc_List_List*_tmp3;struct Cyc_List_List**_tmp4;_LL1: if(_tmp1 <= (void*)
3?1:*((int*)_tmp1)!= 8)goto _LL3;_tmp2=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;
_tmp3=_tmp2.attributes;_tmp4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp1)->f1).attributes;_LL2: {struct Cyc_List_List*_tmp5=0;for(0;atts != 0;atts=
atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmp4=({struct Cyc_List_List*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)((void*)atts->hd);_tmp6->tl=*
_tmp4;_tmp6;});else{_tmp5=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));
_tmp7->hd=(void*)((void*)atts->hd);_tmp7->tl=_tmp5;_tmp7;});}}return _tmp5;}_LL3:;
_LL4:({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("transfer_fn_type_atts",sizeof(char),22),_tag_arr(_tmp8,sizeof(void*),0));});
_LL0:;}struct _tuple6{void*f1;int f2;};static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*
vd,int check_var_init){struct Cyc_Absyn_Vardecl _tmpA;void*_tmpB;struct _tuple0*
_tmpC;struct _tuple0 _tmpD;void*_tmpE;struct _tagged_arr*_tmpF;void*_tmp10;struct
Cyc_Absyn_Exp*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Vardecl*_tmp9=vd;
_tmpA=*_tmp9;_tmpB=(void*)_tmpA.sc;_tmpC=_tmpA.name;_tmpD=*_tmpC;_tmpE=_tmpD.f1;
_tmpF=_tmpD.f2;_tmp10=(void*)_tmpA.type;_tmp11=_tmpA.initializer;_tmp12=_tmpA.attributes;{
void*_tmp13=_tmpE;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;_LL6: if(
_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LL8;_tmp14=((struct Cyc_Absyn_Rel_n_struct*)
_tmp13)->f1;if(_tmp14 != 0)goto _LL8;_LL7: goto _LL9;_LL8: if(_tmp13 <= (void*)1?1:*((
int*)_tmp13)!= 1)goto _LLA;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)_tmp13)->f1;if(
_tmp15 != 0)goto _LLA;_LL9: goto _LL5;_LLA:;_LLB:({struct Cyc_Std_String_pa_struct
_tmp17;_tmp17.tag=0;_tmp17.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
void*_tmp16[1]={& _tmp17};Cyc_Tcutil_terr(loc,_tag_arr("qualified variable declarations are not implemented (%s)",
sizeof(char),57),_tag_arr(_tmp16,sizeof(void*),1));}});return;_LL5:;}(*vd->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp19;_tmp19.tag=1;_tmp19.f1=te->ns;_tmp19;});
_tmp18;});{void*_tmp1A=Cyc_Tcutil_compress(_tmp10);void*_tmp1B;struct Cyc_Absyn_Tqual
_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;_LLD: if(_tmp1A <= (void*)3?1:*((int*)_tmp1A)!= 
7)goto _LLF;_tmp1B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp1A)->f1;_tmp1C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp1A)->f2;_tmp1D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1A)->f3;if(_tmp1D != 0)goto _LLF;if(!(_tmp11 != 0))goto _LLF;_LLE:{void*_tmp1E=(
void*)_tmp11->r;void*_tmp1F;struct _tagged_arr _tmp20;struct Cyc_Absyn_Exp*_tmp21;
struct Cyc_List_List*_tmp22;struct Cyc_List_List*_tmp23;_LL12: if(*((int*)_tmp1E)!= 
0)goto _LL14;_tmp1F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1E)->f1;if(
_tmp1F <= (void*)1?1:*((int*)_tmp1F)!= 5)goto _LL14;_tmp20=((struct Cyc_Absyn_String_c_struct*)
_tmp1F)->f1;_LL13: _tmp10=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp25;_tmp25.tag=7;_tmp25.f1=(void*)_tmp1B;_tmp25.f2=_tmp1C;_tmp25.f3=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmp20,sizeof(char)),0);_tmp25;});
_tmp24;})));goto _LL11;_LL14: if(*((int*)_tmp1E)!= 27)goto _LL16;_tmp21=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E)->f2;_LL15: _tmp10=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp27;_tmp27.tag=7;_tmp27.f1=(void*)_tmp1B;_tmp27.f2=_tmp1C;_tmp27.f3=(struct
Cyc_Absyn_Exp*)_tmp21;_tmp27;});_tmp26;})));goto _LL11;_LL16: if(*((int*)_tmp1E)!= 
34)goto _LL18;_tmp22=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1E)->f2;_LL17:
_tmp23=_tmp22;goto _LL19;_LL18: if(*((int*)_tmp1E)!= 26)goto _LL1A;_tmp23=((struct
Cyc_Absyn_Array_e_struct*)_tmp1E)->f1;_LL19: _tmp10=(void*)(vd->type=(void*)((
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp28=_cycalloc(sizeof(*_tmp28));
_tmp28[0]=({struct Cyc_Absyn_ArrayType_struct _tmp29;_tmp29.tag=7;_tmp29.f1=(void*)
_tmp1B;_tmp29.f2=_tmp1C;_tmp29.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp23),0);_tmp29;});
_tmp28;})));goto _LL11;_LL1A:;_LL1B: goto _LL11;_LL11:;}goto _LLC;_LLF:;_LL10: goto
_LLC;_LLC:;}Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp10);{void*_tmp2A=Cyc_Tcutil_compress(
_tmp10);_LL1D: if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 7)goto _LL1F;_LL1E: vd->escapes=
0;goto _LL1C;_LL1F:;_LL20: vd->escapes=1;goto _LL1C;_LL1C:;}if(Cyc_Tcutil_is_function_type(
_tmp10))_tmp12=Cyc_Tc_transfer_fn_type_atts(_tmp10,_tmp12);if(_tmpB == (void*)3?1:
_tmpB == (void*)4){if(_tmp11 != 0)({void*_tmp2B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("extern declaration should not have initializer",
sizeof(char),47),_tag_arr(_tmp2B,sizeof(void*),0));});}else{if(!Cyc_Tcutil_is_function_type(
_tmp10)){for(0;_tmp12 != 0;_tmp12=_tmp12->tl){void*_tmp2C=(void*)_tmp12->hd;_LL22:
if(_tmp2C <= (void*)16?1:*((int*)_tmp2C)!= 1)goto _LL24;_LL23: goto _LL25;_LL24: if(
_tmp2C <= (void*)16?1:*((int*)_tmp2C)!= 2)goto _LL26;_LL25: goto _LL27;_LL26: if((int)
_tmp2C != 6)goto _LL28;_LL27: goto _LL29;_LL28: if((int)_tmp2C != 7)goto _LL2A;_LL29:
goto _LL2B;_LL2A: if((int)_tmp2C != 8)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if((int)
_tmp2C != 9)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if((int)_tmp2C != 10)goto _LL30;_LL2F:
goto _LL31;_LL30: if((int)_tmp2C != 11)goto _LL32;_LL31: continue;_LL32:;_LL33:({
struct Cyc_Std_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(vd->name);{struct Cyc_Std_String_pa_struct _tmp2E;_tmp2E.tag=
0;_tmp2E.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp12->hd);{
void*_tmp2D[2]={& _tmp2E,& _tmp2F};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for variable %s",
sizeof(char),33),_tag_arr(_tmp2D,sizeof(void*),2));}}});goto _LL21;_LL21:;}if(
_tmp11 == 0){if(check_var_init?!Cyc_Tcutil_supports_default(_tmp10): 0)({struct Cyc_Std_String_pa_struct
_tmp32;_tmp32.tag=0;_tmp32.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp10);{
struct Cyc_Std_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(vd->name);{void*_tmp30[2]={& _tmp31,& _tmp32};Cyc_Tcutil_terr(
loc,_tag_arr("initializer required for variable %s of type %s",sizeof(char),48),
_tag_arr(_tmp30,sizeof(void*),2));}}});}else{struct Cyc_Absyn_Exp*_tmp33=(struct
Cyc_Absyn_Exp*)_check_null(_tmp11);void*_tmp34=Cyc_Tcexp_tcExpInitializer(te,(
void**)& _tmp10,_tmp33);if(!Cyc_Tcutil_coerce_assign(te,_tmp33,_tmp10)){({struct
Cyc_Std_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp34);{struct Cyc_Std_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(_tmp10);{struct Cyc_Std_String_pa_struct _tmp36;
_tmp36.tag=0;_tmp36.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
void*_tmp35[3]={& _tmp36,& _tmp37,& _tmp38};Cyc_Tcutil_terr(loc,_tag_arr("%s declared with type \n%s\n but initialized with type \n%s",
sizeof(char),57),_tag_arr(_tmp35,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}
if(!Cyc_Tcutil_is_const_exp(te,_tmp33))({void*_tmp39[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("initializer is not a constant expression",sizeof(char),41),_tag_arr(
_tmp39,sizeof(void*),0));});}}else{for(0;_tmp12 != 0;_tmp12=_tmp12->tl){void*
_tmp3A=(void*)_tmp12->hd;_LL35: if(_tmp3A <= (void*)16?1:*((int*)_tmp3A)!= 0)goto
_LL37;_LL36: goto _LL38;_LL37: if((int)_tmp3A != 0)goto _LL39;_LL38: goto _LL3A;_LL39:
if((int)_tmp3A != 1)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if((int)_tmp3A != 2)goto _LL3D;
_LL3C: goto _LL3E;_LL3D: if((int)_tmp3A != 3)goto _LL3F;_LL3E: goto _LL40;_LL3F: if(
_tmp3A <= (void*)16?1:*((int*)_tmp3A)!= 3)goto _LL41;_LL40: goto _LL42;_LL41: if((int)
_tmp3A != 4)goto _LL43;_LL42:({void*_tmp3B[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcVardecl: fn type atts in function var decl",
sizeof(char),45),_tag_arr(_tmp3B,sizeof(void*),0));});_LL43: if(_tmp3A <= (void*)
16?1:*((int*)_tmp3A)!= 1)goto _LL45;_LL44: goto _LL46;_LL45: if((int)_tmp3A != 5)goto
_LL47;_LL46:({struct Cyc_Std_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp12->hd);{void*_tmp3C[1]={&
_tmp3D};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in function declaration",
sizeof(char),41),_tag_arr(_tmp3C,sizeof(void*),1));}});goto _LL34;_LL47:;_LL48:
continue;_LL34:;}}}{struct _handler_cons _tmp3E;_push_handler(& _tmp3E);{int _tmp40=
0;if(setjmp(_tmp3E.handler))_tmp40=1;if(!_tmp40){{struct _tuple6*_tmp41=((struct
_tuple6*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
_tmpF);void*_tmp42=(*_tmp41).f1;void*_tmp43;_LL4A: if(*((int*)_tmp42)!= 0)goto
_LL4C;_tmp43=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp42)->f1;_LL4B: {struct
Cyc_Absyn_Global_b_struct*_tmp44=({struct Cyc_Absyn_Global_b_struct*_tmp49=
_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Global_b_struct _tmp4A;
_tmp4A.tag=0;_tmp4A.f1=vd;_tmp4A;});_tmp49;});void*_tmp45=Cyc_Tcdecl_merge_binding(
_tmp43,(void*)_tmp44,loc,Cyc_Tc_tc_msg);if(_tmp45 == (void*)0)goto _LL49;if(_tmp45
== _tmp43?(*_tmp41).f2: 0)goto _LL49;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,
_tmpF,({struct _tuple6*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(void*)_tmp45;_tmp48;});
_tmp47;});_tmp46->f2=1;_tmp46;}));goto _LL49;}_LL4C: if(*((int*)_tmp42)!= 1)goto
_LL4E;_LL4D:({void*_tmp4B[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous struct declaration",
sizeof(char),57),_tag_arr(_tmp4B,sizeof(void*),0));});goto _LL49;_LL4E: if(*((int*)
_tmp42)!= 2)goto _LL50;_LL4F:({void*_tmp4C[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof(char),60),_tag_arr(_tmp4C,sizeof(void*),0));});goto _LL49;_LL50: if(*((int*)
_tmp42)!= 4)goto _LL52;_LL51: goto _LL53;_LL52: if(*((int*)_tmp42)!= 3)goto _LL49;
_LL53:({void*_tmp4D[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous enum tag",
sizeof(char),47),_tag_arr(_tmp4D,sizeof(void*),0));});goto _LL49;_LL49:;};
_pop_handler();}else{void*_tmp3F=(void*)_exn_thrown;void*_tmp4F=_tmp3F;_LL55: if(
_tmp4F != Cyc_Dict_Absent)goto _LL57;_LL56: ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,
_tmpF,({struct _tuple6*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(void*)((void*)({
struct Cyc_Absyn_Global_b_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({
struct Cyc_Absyn_Global_b_struct _tmp54;_tmp54.tag=0;_tmp54.f1=vd;_tmp54;});_tmp53;}));
_tmp52;});_tmp51;});_tmp50->f2=0;_tmp50;}));goto _LL54;_LL57:;_LL58:(void)_throw(
_tmp4F);_LL54:;}}}}static int Cyc_Tc_is_main(struct _tuple0*n){struct _tuple0 _tmp56;
void*_tmp57;struct _tagged_arr*_tmp58;struct _tuple0*_tmp55=n;_tmp56=*_tmp55;
_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{void*_tmp59=_tmp57;struct Cyc_List_List*_tmp5A;
_LL5A: if(_tmp59 <= (void*)1?1:*((int*)_tmp59)!= 1)goto _LL5C;_tmp5A=((struct Cyc_Absyn_Abs_n_struct*)
_tmp59)->f1;if(_tmp5A != 0)goto _LL5C;_LL5B: return Cyc_Std_zstrcmp(*_tmp58,_tag_arr("main",
sizeof(char),5))== 0;_LL5C:;_LL5D: return 0;_LL59:;}}struct _tuple7{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Tc_tcFndecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Fndecl*fd){struct _tagged_arr*v=(*fd->name).f2;if((void*)fd->sc == (
void*)4)({void*_tmp5B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("extern \"C\" functions cannot be implemented in Cyclone",
sizeof(char),54),_tag_arr(_tmp5B,sizeof(void*),0));});{void*_tmp5C=(*fd->name).f1;
struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;_LL5F: if(_tmp5C <= (void*)1?
1:*((int*)_tmp5C)!= 0)goto _LL61;_tmp5D=((struct Cyc_Absyn_Rel_n_struct*)_tmp5C)->f1;
if(_tmp5D != 0)goto _LL61;_LL60: goto _LL5E;_LL61: if(_tmp5C <= (void*)1?1:*((int*)
_tmp5C)!= 1)goto _LL63;_tmp5E=((struct Cyc_Absyn_Abs_n_struct*)_tmp5C)->f1;_LL62:({
void*_tmp5F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("tc: Abs_n in tcFndecl",sizeof(char),22),_tag_arr(_tmp5F,sizeof(void*),
0));});_LL63:;_LL64:({struct Cyc_Std_String_pa_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(fd->name);{void*_tmp60[1]={& _tmp61};
Cyc_Tcutil_terr(loc,_tag_arr("qualified function declarations are not implemented (%s)",
sizeof(char),57),_tag_arr(_tmp60,sizeof(void*),1));}});return;_LL5E:;}(*fd->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp63;_tmp63.tag=1;_tmp63.f1=te->ns;_tmp63;});
_tmp62;});Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(
fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(t,fd->attributes);{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){void*_tmp64=(void*)atts->hd;
_LL66: if((int)_tmp64 != 5)goto _LL68;_LL67: goto _LL69;_LL68: if(_tmp64 <= (void*)16?1:*((
int*)_tmp64)!= 1)goto _LL6A;_LL69:({struct Cyc_Std_String_pa_struct _tmp66;_tmp66.tag=
0;_tmp66.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*
_tmp65[1]={& _tmp66};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for function",
sizeof(char),30),_tag_arr(_tmp65,sizeof(void*),1));}});goto _LL65;_LL6A:;_LL6B:
goto _LL65;_LL65:;}}{struct _handler_cons _tmp67;_push_handler(& _tmp67);{int _tmp69=
0;if(setjmp(_tmp67.handler))_tmp69=1;if(!_tmp69){{struct _tuple6*_tmp6A=((struct
_tuple6*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
v);void*_tmp6B=(*_tmp6A).f1;void*_tmp6C;_LL6D: if(*((int*)_tmp6B)!= 0)goto _LL6F;
_tmp6C=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp6B)->f1;_LL6E: {struct Cyc_Absyn_Funname_b_struct*
_tmp6D=({struct Cyc_Absyn_Funname_b_struct*_tmp72=_cycalloc(sizeof(*_tmp72));
_tmp72[0]=({struct Cyc_Absyn_Funname_b_struct _tmp73;_tmp73.tag=1;_tmp73.f1=fd;
_tmp73;});_tmp72;});void*_tmp6E=Cyc_Tcdecl_merge_binding(_tmp6C,(void*)_tmp6D,
loc,Cyc_Tc_tc_msg);if(_tmp6E == (void*)0)goto _LL6C;if(_tmp6E == _tmp6C?(*_tmp6A).f2:
0)goto _LL6C;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct
_tuple6*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp71;_tmp71.tag=0;_tmp71.f1=(void*)_tmp6E;_tmp71;});_tmp70;});_tmp6F->f2=1;
_tmp6F;}));goto _LL6C;}_LL6F: if(*((int*)_tmp6B)!= 1)goto _LL71;_LL70:({void*_tmp74[
0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous type declaration",
sizeof(char),55),_tag_arr(_tmp74,sizeof(void*),0));});goto _LL6C;_LL71: if(*((int*)
_tmp6B)!= 2)goto _LL73;_LL72:({void*_tmp75[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof(char),60),_tag_arr(_tmp75,sizeof(void*),0));});goto _LL6C;_LL73: if(*((int*)
_tmp6B)!= 4)goto _LL75;_LL74: goto _LL76;_LL75: if(*((int*)_tmp6B)!= 3)goto _LL6C;
_LL76:({void*_tmp76[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous enum tag",
sizeof(char),47),_tag_arr(_tmp76,sizeof(void*),0));});goto _LL6C;_LL6C:;};
_pop_handler();}else{void*_tmp68=(void*)_exn_thrown;void*_tmp78=_tmp68;_LL78: if(
_tmp78 != Cyc_Dict_Absent)goto _LL7A;_LL79: ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,
v,({struct _tuple6*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=(void*)({struct
Cyc_Tcenv_VarRes_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp7B;_tmp7B.tag=0;_tmp7B.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp7D;_tmp7D.tag=1;_tmp7D.f1=fd;_tmp7D;});_tmp7C;}));_tmp7B;});_tmp7A;});_tmp79->f2=
0;_tmp79;}));goto _LL77;_LL7A:;_LL7B:(void)_throw(_tmp78);_LL77:;}}}te->le=({
struct Cyc_Core_Opt*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->v=(void*)((void*)({
struct Cyc_Tcenv_Outermost_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({
struct Cyc_Tcenv_Outermost_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(void*)Cyc_Tcenv_new_fenv(
loc,fd);_tmp80;});_tmp7F;}));_tmp7E;});Cyc_Tcstmt_tcStmt(te,fd->body,0);Cyc_Tcenv_check_delayed_effects(
te);Cyc_Tcenv_check_delayed_constraints(te);if(!Cyc_Tcenv_all_labels_resolved(te))({
void*_tmp81[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function has goto statements to undefined labels",
sizeof(char),49),_tag_arr(_tmp81,sizeof(void*),0));});if(Cyc_Tc_is_main(fd->name)){{
void*_tmp82=Cyc_Tcutil_compress((void*)fd->ret_type);void*_tmp83;_LL7D: if((int)
_tmp82 != 0)goto _LL7F;_LL7E:({void*_tmp84[0]={};Cyc_Tcutil_warn(loc,_tag_arr("main declared with return type void",
sizeof(char),36),_tag_arr(_tmp84,sizeof(void*),0));});goto _LL7C;_LL7F: if(_tmp82
<= (void*)3?1:*((int*)_tmp82)!= 5)goto _LL81;_tmp83=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp82)->f2;if((int)_tmp83 != 2)goto _LL81;_LL80: goto _LL7C;_LL81:;_LL82:({struct
Cyc_Std_String_pa_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)fd->ret_type);{void*_tmp85[1]={& _tmp86};Cyc_Tcutil_terr(loc,_tag_arr("main declared with return type %s instead of int or void",
sizeof(char),57),_tag_arr(_tmp85,sizeof(void*),1));}});goto _LL7C;_LL7C:;}if(fd->c_varargs?
1: fd->cyc_varargs != 0)({void*_tmp87[0]={};Cyc_Tcutil_terr(loc,_tag_arr("main declared with varargs",
sizeof(char),27),_tag_arr(_tmp87,sizeof(void*),0));});{struct Cyc_List_List*
_tmp88=fd->args;if(_tmp88 != 0){struct _tuple7 _tmp8A;void*_tmp8B;struct _tuple7*
_tmp89=(struct _tuple7*)_tmp88->hd;_tmp8A=*_tmp89;_tmp8B=_tmp8A.f3;{void*_tmp8C=
Cyc_Tcutil_compress(_tmp8B);void*_tmp8D;_LL84: if(_tmp8C <= (void*)3?1:*((int*)
_tmp8C)!= 5)goto _LL86;_tmp8D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp8C)->f2;
if((int)_tmp8D != 2)goto _LL86;_LL85: goto _LL83;_LL86:;_LL87:({struct Cyc_Std_String_pa_struct
_tmp8F;_tmp8F.tag=0;_tmp8F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp8B);{
void*_tmp8E[1]={& _tmp8F};Cyc_Tcutil_terr(loc,_tag_arr("main declared with first argument of type %s instead of int",
sizeof(char),60),_tag_arr(_tmp8E,sizeof(void*),1));}});goto _LL83;_LL83:;}_tmp88=
_tmp88->tl;if(_tmp88 != 0){struct _tuple7 _tmp91;void*_tmp92;struct _tuple7*_tmp90=(
struct _tuple7*)_tmp88->hd;_tmp91=*_tmp90;_tmp92=_tmp91.f3;_tmp88=_tmp88->tl;if(
_tmp88 != 0)({void*_tmp93[0]={};Cyc_Tcutil_terr(loc,_tag_arr("main declared with too many arguments",
sizeof(char),38),_tag_arr(_tmp93,sizeof(void*),0));});{struct Cyc_Core_Opt*tvs=({
struct Cyc_Core_Opt*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->v=fd->tvs;_tmp9E;});
if(((!Cyc_Tcutil_unify(_tmp92,Cyc_Absyn_tagged_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->v=(void*)((void*)3);
_tmp94;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*
_tmp95));_tmp95->v=(void*)((void*)3);_tmp95;}),tvs),Cyc_Absyn_empty_tqual()))?!
Cyc_Tcutil_unify(_tmp92,Cyc_Absyn_tagged_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->v=(void*)((void*)3);
_tmp96;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->v=(void*)((void*)3);_tmp97;}),tvs),Cyc_Absyn_empty_tqual())): 0)?
!Cyc_Tcutil_unify(_tmp92,Cyc_Absyn_tagged_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->v=(void*)((void*)3);
_tmp98;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99->v=(void*)((void*)3);_tmp99;}),tvs),Cyc_Absyn_const_tqual())): 0)?
!Cyc_Tcutil_unify(_tmp92,Cyc_Absyn_tagged_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->v=(void*)((void*)3);
_tmp9A;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp9B=_cycalloc(sizeof(*
_tmp9B));_tmp9B->v=(void*)((void*)3);_tmp9B;}),tvs),Cyc_Absyn_const_tqual())): 0)({
struct Cyc_Std_String_pa_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(_tmp92);{void*_tmp9C[1]={& _tmp9D};Cyc_Tcutil_terr(loc,
_tag_arr("second argument of main has type %s instead of char??",sizeof(char),54),
_tag_arr(_tmp9C,sizeof(void*),1));}});}}}}}te->le=0;}}static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Typedefdecl*td){struct _tagged_arr*v=(*td->name).f2;{void*_tmp9F=(*
td->name).f1;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmpA1;_LL89: if(
_tmp9F <= (void*)1?1:*((int*)_tmp9F)!= 0)goto _LL8B;_tmpA0=((struct Cyc_Absyn_Rel_n_struct*)
_tmp9F)->f1;if(_tmpA0 != 0)goto _LL8B;_LL8A: goto _LL8C;_LL8B: if(_tmp9F <= (void*)1?1:*((
int*)_tmp9F)!= 1)goto _LL8D;_tmpA1=((struct Cyc_Absyn_Abs_n_struct*)_tmp9F)->f1;
if(_tmpA1 != 0)goto _LL8D;_LL8C: goto _LL88;_LL8D:;_LL8E:({struct Cyc_Std_String_pa_struct
_tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(td->name);{
void*_tmpA2[1]={& _tmpA3};Cyc_Tcutil_terr(loc,_tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof(char),56),_tag_arr(_tmpA2,sizeof(void*),1));}});return;_LL88:;}if(((int(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(ge->typedefs,v)){({
struct Cyc_Std_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _tagged_arr)*
v;{void*_tmpA4[1]={& _tmpA5};Cyc_Tcutil_terr(loc,_tag_arr("redeclaration of typedef %s",
sizeof(char),28),_tag_arr(_tmpA4,sizeof(void*),1));}});return;}(*td->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({
struct Cyc_Absyn_Abs_n_struct _tmpA7;_tmpA7.tag=1;_tmpA7.f1=te->ns;_tmpA7;});
_tmpA6;});Cyc_Tcutil_check_unique_tvars(loc,td->tvs);Cyc_Tcutil_add_tvar_identities(
td->tvs);if(td->defn != 0)Cyc_Tcutil_check_type(loc,te,td->tvs,(void*)0,0,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);else{struct Cyc_List_List*tvs=td->tvs;
for(0;tvs != 0;tvs=tvs->tl){void*_tmpA8=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmpA9;struct Cyc_Core_Opt**_tmpAA;struct Cyc_Core_Opt*
_tmpAB;struct Cyc_Core_Opt**_tmpAC;void*_tmpAD;_LL90: if(*((int*)_tmpA8)!= 1)goto
_LL92;_tmpA9=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpA8)->f1;_tmpAA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpA8)->f1;_LL91:*_tmpAA=({struct Cyc_Core_Opt*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpB0;
_tmpB0.tag=0;_tmpB0.f1=(void*)((void*)2);_tmpB0;});_tmpAF;}));_tmpAE;});goto
_LL8F;_LL92: if(*((int*)_tmpA8)!= 2)goto _LL94;_tmpAB=((struct Cyc_Absyn_Less_kb_struct*)
_tmpA8)->f1;_tmpAC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpA8)->f1;_tmpAD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpA8)->f2;_LL93:*
_tmpAC=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=(void*)((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmpB3;_tmpB3.tag=0;_tmpB3.f1=(void*)_tmpAD;_tmpB3;});
_tmpB2;}));_tmpB1;});goto _LL8F;_LL94:;_LL95: continue;_LL8F:;}}ge->typedefs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle
_tmpB4=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpB4;
_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple3 _tmpB6;void*
_tmpB7;void*_tmpB8;struct _tuple3*_tmpB5=(struct _tuple3*)rpo->hd;_tmpB6=*_tmpB5;
_tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;Cyc_Tcutil_check_type(loc,te,tvs,(void*)4,0,
_tmpB7);Cyc_Tcutil_check_type(loc,te,tvs,(void*)3,0,_tmpB8);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpB9=fields;for(0;_tmpB9 != 0;_tmpB9=_tmpB9->tl){
struct Cyc_Absyn_Aggrfield _tmpBB;struct _tagged_arr*_tmpBC;struct Cyc_Absyn_Tqual
_tmpBD;void*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_List_List*_tmpC0;struct
Cyc_Absyn_Aggrfield*_tmpBA=(struct Cyc_Absyn_Aggrfield*)_tmpB9->hd;_tmpBB=*_tmpBA;
_tmpBC=_tmpBB.name;_tmpBD=_tmpBB.tq;_tmpBE=(void*)_tmpBB.type;_tmpBF=_tmpBB.width;
_tmpC0=_tmpBB.attributes;if(((int(*)(int(*compare)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmpBC))({struct Cyc_Std_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(
struct _tagged_arr)*_tmpBC;{void*_tmpC1[1]={& _tmpC2};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmpC1,sizeof(void*),1));}});if(Cyc_Std_strcmp(*_tmpBC,
_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*_tmpC3=
_region_malloc(uprev_rgn,sizeof(*_tmpC3));_tmpC3->hd=_tmpBC;_tmpC3->tl=
prev_fields;_tmpC3;});Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmpBE);Cyc_Tcutil_check_bitfield(
loc,te,_tmpBE,_tmpBF,_tmpBC);}};_pop_region(uprev_rgn);}struct _tuple8{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tagged_arr*_tmpC4=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmpC5=(void*)atts->hd;_LL97: if((int)_tmpC5 != 
5)goto _LL99;_LL98: goto _LL9A;_LL99: if(_tmpC5 <= (void*)16?1:*((int*)_tmpC5)!= 1)
goto _LL9B;_LL9A: continue;_LL9B:;_LL9C:({struct Cyc_Std_String_pa_struct _tmpC8;
_tmpC8.tag=0;_tmpC8.f1=(struct _tagged_arr)*_tmpC4;{struct Cyc_Std_String_pa_struct
_tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmpC6[2]={& _tmpC7,& _tmpC8};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in  %s definition",
sizeof(char),35),_tag_arr(_tmpC6,sizeof(void*),2));}}});goto _LL96;_LL96:;}}{
struct Cyc_List_List*_tmpC9=ad->tvs;{struct Cyc_List_List*tvs2=_tmpC9;for(0;tvs2 != 
0;tvs2=tvs2->tl){void*_tmpCA=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpCB;struct Cyc_Core_Opt**_tmpCC;struct Cyc_Core_Opt*
_tmpCD;struct Cyc_Core_Opt**_tmpCE;void*_tmpCF;struct Cyc_Core_Opt*_tmpD0;struct
Cyc_Core_Opt**_tmpD1;void*_tmpD2;void*_tmpD3;_LL9E: if(*((int*)_tmpCA)!= 1)goto
_LLA0;_tmpCB=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpCA)->f1;_tmpCC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpCA)->f1;_LL9F: _tmpCE=_tmpCC;goto _LLA1;
_LLA0: if(*((int*)_tmpCA)!= 2)goto _LLA2;_tmpCD=((struct Cyc_Absyn_Less_kb_struct*)
_tmpCA)->f1;_tmpCE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpCA)->f1;_tmpCF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpCA)->f2;if((int)
_tmpCF != 1)goto _LLA2;_LLA1: _tmpD1=_tmpCE;goto _LLA3;_LLA2: if(*((int*)_tmpCA)!= 2)
goto _LLA4;_tmpD0=((struct Cyc_Absyn_Less_kb_struct*)_tmpCA)->f1;_tmpD1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpCA)->f1;_tmpD2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpCA)->f2;if((int)_tmpD2 != 0)goto _LLA4;_LLA3:*_tmpD1=({struct Cyc_Core_Opt*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpD6;
_tmpD6.tag=0;_tmpD6.f1=(void*)((void*)2);_tmpD6;});_tmpD5;}));_tmpD4;});
continue;_LLA4: if(*((int*)_tmpCA)!= 0)goto _LLA6;_tmpD3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpCA)->f1;if((int)_tmpD3 != 1)goto _LLA6;_LLA5:({struct Cyc_Std_String_pa_struct
_tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name;{
struct Cyc_Std_String_pa_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)*
_tmpC4;{void*_tmpD7[2]={& _tmpD8,& _tmpD9};Cyc_Tcutil_terr(loc,_tag_arr("type %s attempts to abstract type variable %s of kind M",
sizeof(char),56),_tag_arr(_tmpD7,sizeof(void*),2));}}});continue;_LLA6:;_LLA7:
continue;_LL9D:;}}{void*_tmpDA=(*ad->name).f1;struct Cyc_List_List*_tmpDB;struct
Cyc_List_List*_tmpDC;_LLA9: if(_tmpDA <= (void*)1?1:*((int*)_tmpDA)!= 0)goto _LLAB;
_tmpDB=((struct Cyc_Absyn_Rel_n_struct*)_tmpDA)->f1;if(_tmpDB != 0)goto _LLAB;_LLAA:
goto _LLAC;_LLAB: if(_tmpDA <= (void*)1?1:*((int*)_tmpDA)!= 1)goto _LLAD;_tmpDC=((
struct Cyc_Absyn_Abs_n_struct*)_tmpDA)->f1;if(_tmpDC != 0)goto _LLAD;_LLAC: goto
_LLA8;_LLAD:;_LLAE:({struct Cyc_Std_String_pa_struct _tmpDE;_tmpDE.tag=0;_tmpDE.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(ad->name);{void*_tmpDD[1]={& _tmpDE};
Cyc_Tcutil_terr(loc,_tag_arr("qualified struct declarations are not implemented (%s)",
sizeof(char),55),_tag_arr(_tmpDD,sizeof(void*),1));}});return;_LLA8:;}(*ad->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({
struct Cyc_Absyn_Abs_n_struct _tmpE0;_tmpE0.tag=1;_tmpE0.f1=te->ns;_tmpE0;});
_tmpDF;});Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(
ad->tvs);{struct _tuple8 _tmpE2=({struct _tuple8 _tmpE1;_tmpE1.f1=ad->impl;_tmpE1.f2=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->aggrdecls,_tmpC4);_tmpE1;});struct Cyc_Absyn_AggrdeclImpl*_tmpE3;struct Cyc_Core_Opt*
_tmpE4;struct Cyc_Absyn_AggrdeclImpl*_tmpE5;struct Cyc_Absyn_AggrdeclImpl _tmpE6;
struct Cyc_List_List*_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*_tmpE9;
struct Cyc_Core_Opt*_tmpEA;struct Cyc_Absyn_AggrdeclImpl*_tmpEB;struct Cyc_Absyn_AggrdeclImpl
_tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_Core_Opt*_tmpF0;struct Cyc_Core_Opt _tmpF1;struct Cyc_Absyn_Aggrdecl**
_tmpF2;struct Cyc_Absyn_AggrdeclImpl*_tmpF3;struct Cyc_Core_Opt*_tmpF4;struct Cyc_Core_Opt
_tmpF5;struct Cyc_Absyn_Aggrdecl**_tmpF6;_LLB0: _tmpE3=_tmpE2.f1;if(_tmpE3 != 0)
goto _LLB2;_tmpE4=_tmpE2.f2;if(_tmpE4 != 0)goto _LLB2;_LLB1: ge->aggrdecls=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**
v))Cyc_Dict_insert)(ge->aggrdecls,_tmpC4,({struct Cyc_Absyn_Aggrdecl**_tmpF7=
_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=ad;_tmpF7;}));goto _LLAF;_LLB2: _tmpE5=_tmpE2.f1;
if(_tmpE5 == 0)goto _LLB4;_tmpE6=*_tmpE5;_tmpE7=_tmpE6.exist_vars;_tmpE8=_tmpE6.rgn_po;
_tmpE9=_tmpE6.fields;_tmpEA=_tmpE2.f2;if(_tmpEA != 0)goto _LLB4;_LLB3: {struct Cyc_Absyn_Aggrdecl**
_tmpF8=({struct Cyc_Absyn_Aggrdecl**_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({
struct Cyc_Absyn_Aggrdecl*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->kind=(void*)((
void*)ad->kind);_tmpFE->sc=(void*)((void*)3);_tmpFE->name=ad->name;_tmpFE->tvs=
_tmpC9;_tmpFE->impl=0;_tmpFE->attributes=ad->attributes;_tmpFE;});_tmpFD;});ge->aggrdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**
v))Cyc_Dict_insert)(ge->aggrdecls,_tmpC4,_tmpF8);Cyc_Tcutil_check_unique_tvars(
loc,_tmpE7);Cyc_Tcutil_add_tvar_identities(_tmpE7);Cyc_Tc_tcAggrImpl(te,ge,loc,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpC9,_tmpE7),_tmpE8,_tmpE9);if((void*)ad->kind == (void*)1){struct Cyc_List_List*
f=_tmpE9;for(0;f != 0;f=f->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
f->hd)->type))({struct Cyc_Std_String_pa_struct _tmpFC;_tmpFC.tag=0;_tmpFC.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type);{
struct Cyc_Std_String_pa_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _tagged_arr)*
_tmpC4;{struct Cyc_Std_String_pa_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct
_tagged_arr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;{void*_tmpF9[3]={& _tmpFA,&
_tmpFB,& _tmpFC};Cyc_Tcutil_terr(loc,_tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof(char),58),_tag_arr(_tmpF9,sizeof(void*),3));}}}});}}*_tmpF8=ad;goto _LLAF;}
_LLB4: _tmpEB=_tmpE2.f1;if(_tmpEB == 0)goto _LLB6;_tmpEC=*_tmpEB;_tmpED=_tmpEC.exist_vars;
_tmpEE=_tmpEC.rgn_po;_tmpEF=_tmpEC.fields;_tmpF0=_tmpE2.f2;if(_tmpF0 == 0)goto
_LLB6;_tmpF1=*_tmpF0;_tmpF2=(struct Cyc_Absyn_Aggrdecl**)_tmpF1.v;_LLB5: if((void*)
ad->kind != (void*)(*_tmpF2)->kind)({void*_tmpFF[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("cannot reuse struct names for unions and vice-versa",sizeof(char),52),
_tag_arr(_tmpFF,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*_tmp100=*_tmpF2;*
_tmpF2=({struct Cyc_Absyn_Aggrdecl*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->kind=(
void*)((void*)ad->kind);_tmp101->sc=(void*)((void*)3);_tmp101->name=ad->name;
_tmp101->tvs=_tmpC9;_tmp101->impl=0;_tmp101->attributes=ad->attributes;_tmp101;});
Cyc_Tcutil_check_unique_tvars(loc,_tmpED);Cyc_Tcutil_add_tvar_identities(_tmpED);
Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmpC9,_tmpED),_tmpEE,_tmpEF);*_tmpF2=
_tmp100;_tmpF6=_tmpF2;goto _LLB7;}_LLB6: _tmpF3=_tmpE2.f1;if(_tmpF3 != 0)goto _LLAF;
_tmpF4=_tmpE2.f2;if(_tmpF4 == 0)goto _LLAF;_tmpF5=*_tmpF4;_tmpF6=(struct Cyc_Absyn_Aggrdecl**)
_tmpF5.v;_LLB7: {struct Cyc_Absyn_Aggrdecl*_tmp102=Cyc_Tcdecl_merge_aggrdecl(*
_tmpF6,ad,loc,Cyc_Tc_tc_msg);if(_tmp102 == 0)return;else{*_tmpF6=(struct Cyc_Absyn_Aggrdecl*)
_check_null(_tmp102);ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp102);goto
_LLAF;}}_LLAF:;}ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,_tmpC4,({
struct _tuple6*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->f1=(void*)({struct Cyc_Tcenv_AggrRes_struct*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Tcenv_AggrRes_struct
_tmp105;_tmp105.tag=1;_tmp105.f1=ad;_tmp105;});_tmp104;});_tmp103->f2=1;_tmp103;}));}}
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _tagged_arr obj,int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*
_tmp106=fields;for(0;_tmp106 != 0;_tmp106=_tmp106->tl){struct Cyc_Absyn_Tunionfield*
_tmp107=(struct Cyc_Absyn_Tunionfield*)_tmp106->hd;{struct Cyc_List_List*typs=
_tmp107->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp107->loc,
te,tvs,(void*)1,0,(*((struct _tuple9*)typs->hd)).f2);}}{void*_tmp108=(*_tmp107->name).f1;
struct Cyc_List_List*_tmp109;_LLB9: if(_tmp108 <= (void*)1?1:*((int*)_tmp108)!= 0)
goto _LLBB;_tmp109=((struct Cyc_Absyn_Rel_n_struct*)_tmp108)->f1;if(_tmp109 != 0)
goto _LLBB;_LLBA: if(is_xtunion)(*_tmp107->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp10B;_tmp10B.tag=1;_tmp10B.f1=te->ns;_tmp10B;});_tmp10A;});else{(*_tmp107->name).f1=(*
name).f1;}goto _LLB8;_LLBB: if(_tmp108 <= (void*)1?1:*((int*)_tmp108)!= 0)goto _LLBD;
_LLBC:({struct Cyc_Std_String_pa_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp107->name);{void*_tmp10C[1]={& _tmp10D};
Cyc_Tcutil_terr(_tmp107->loc,_tag_arr("qualified tunionfield declarations are not allowed (%s)",
sizeof(char),56),_tag_arr(_tmp10C,sizeof(void*),1));}});goto _LLB8;_LLBD: if(
_tmp108 <= (void*)1?1:*((int*)_tmp108)!= 1)goto _LLBF;_LLBE: goto _LLB8;_LLBF: if((
int)_tmp108 != 0)goto _LLB8;_LLC0:({void*_tmp10E[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcTunionFields: Loc_n",
sizeof(char),22),_tag_arr(_tmp10E,sizeof(void*),0));});_LLB8:;}}}{struct Cyc_List_List*
fields2;if(is_xtunion){int _tmp10F=1;struct Cyc_List_List*_tmp110=Cyc_Tcdecl_sort_xtunion_fields(
fields,& _tmp10F,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmp10F)fields2=_tmp110;else{
fields2=0;}}else{struct _RegionHandle _tmp111=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp111;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*
_tmp112=(struct Cyc_Absyn_Tunionfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp112->name).f2))({struct Cyc_Std_String_pa_struct
_tmp115;_tmp115.tag=0;_tmp115.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=(struct _tagged_arr)*(*_tmp112->name).f2;{void*
_tmp113[2]={& _tmp114,& _tmp115};Cyc_Tcutil_terr(_tmp112->loc,_tag_arr("duplicate field name %s in %s",
sizeof(char),30),_tag_arr(_tmp113,sizeof(void*),2));}}});else{prev_fields=({
struct Cyc_List_List*_tmp116=_region_malloc(uprev_rgn,sizeof(*_tmp116));_tmp116->hd=(*
_tmp112->name).f2;_tmp116->tl=prev_fields;_tmp116;});}if((void*)_tmp112->sc != (
void*)2){({struct Cyc_Std_String_pa_struct _tmp118;_tmp118.tag=0;_tmp118.f1=(
struct _tagged_arr)*(*_tmp112->name).f2;{void*_tmp117[1]={& _tmp118};Cyc_Tcutil_warn(
loc,_tag_arr("ignoring scope of field %s",sizeof(char),27),_tag_arr(_tmp117,
sizeof(void*),1));}});(void*)(_tmp112->sc=(void*)((void*)2));}}}fields2=fields;};
_pop_region(uprev_rgn);}{struct Cyc_List_List*_tmp119=fields;for(0;_tmp119 != 0;
_tmp119=_tmp119->tl){struct Cyc_Absyn_Tunionfield*_tmp11A=(struct Cyc_Absyn_Tunionfield*)
_tmp119->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp11A->name).f2,({
struct _tuple6*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmp11D;_tmp11D.tag=2;_tmp11D.f1=tudres;_tmp11D.f2=_tmp11A;_tmp11D;});_tmp11C;});
_tmp11B->f2=1;_tmp11B;}));}}return fields2;}}struct _tuple10{struct Cyc_Core_Opt*f1;
struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*tud){
struct _tagged_arr*v=(*tud->name).f2;struct _tagged_arr obj=tud->is_xtunion?
_tag_arr("xtunion",sizeof(char),8): _tag_arr("tunion",sizeof(char),7);struct Cyc_List_List*
tvs=tud->tvs;{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*
_tmp11E=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);
struct Cyc_Core_Opt*_tmp11F;struct Cyc_Core_Opt**_tmp120;struct Cyc_Core_Opt*
_tmp121;struct Cyc_Core_Opt**_tmp122;void*_tmp123;struct Cyc_Core_Opt*_tmp124;
struct Cyc_Core_Opt**_tmp125;void*_tmp126;void*_tmp127;_LLC2: if(*((int*)_tmp11E)
!= 1)goto _LLC4;_tmp11F=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp11E)->f1;_tmp120=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp11E)->f1;_LLC3:
_tmp122=_tmp120;goto _LLC5;_LLC4: if(*((int*)_tmp11E)!= 2)goto _LLC6;_tmp121=((
struct Cyc_Absyn_Less_kb_struct*)_tmp11E)->f1;_tmp122=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp11E)->f1;_tmp123=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp11E)->f2;if((int)_tmp123 != 1)goto _LLC6;_LLC5: _tmp125=_tmp122;goto _LLC7;_LLC6:
if(*((int*)_tmp11E)!= 2)goto _LLC8;_tmp124=((struct Cyc_Absyn_Less_kb_struct*)
_tmp11E)->f1;_tmp125=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp11E)->f1;_tmp126=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp11E)->f2;if((
int)_tmp126 != 0)goto _LLC8;_LLC7:*_tmp125=({struct Cyc_Core_Opt*_tmp128=_cycalloc(
sizeof(*_tmp128));_tmp128->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp12A;_tmp12A.tag=0;_tmp12A.f1=(void*)((void*)2);_tmp12A;});_tmp129;}));
_tmp128;});goto _LLC1;_LLC8: if(*((int*)_tmp11E)!= 0)goto _LLCA;_tmp127=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp11E)->f1;if((int)_tmp127 != 1)goto _LLCA;_LLC9:({
struct Cyc_Std_String_pa_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct
_tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name;{struct Cyc_Std_String_pa_struct
_tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _tagged_arr)*v;{struct Cyc_Std_String_pa_struct
_tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _tagged_arr)obj;{void*_tmp12B[3]={&
_tmp12C,& _tmp12D,& _tmp12E};Cyc_Tcutil_terr(loc,_tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof(char),54),_tag_arr(_tmp12B,sizeof(void*),3));}}}});goto _LLC1;_LLCA:;_LLCB:
goto _LLC1;_LLC1:;}}Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{struct _handler_cons _tmp12F;
_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!
_tmp131){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,loc,tud->name);;_pop_handler();}
else{void*_tmp130=(void*)_exn_thrown;void*_tmp133=_tmp130;_LLCD: if(_tmp133 != Cyc_Dict_Absent)
goto _LLCF;_LLCE:({struct Cyc_Std_String_pa_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp134[1]={& _tmp135};
Cyc_Tcutil_terr(loc,_tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof(char),60),_tag_arr(_tmp134,sizeof(void*),1));}});return;_LLCF:;_LLD0:(
void)_throw(_tmp133);_LLCC:;}}}if(tud_opt != 0)tud->name=(*((struct Cyc_Absyn_Tuniondecl**)
tud_opt->v))->name;else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp137;_tmp137.tag=1;_tmp137.f1=te->ns;_tmp137;});_tmp136;});}}else{{void*
_tmp138=(*tud->name).f1;struct Cyc_List_List*_tmp139;_LLD2: if(_tmp138 <= (void*)1?
1:*((int*)_tmp138)!= 0)goto _LLD4;_tmp139=((struct Cyc_Absyn_Rel_n_struct*)_tmp138)->f1;
if(_tmp139 != 0)goto _LLD4;_LLD3:(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp13B;_tmp13B.tag=1;_tmp13B.f1=te->ns;_tmp13B;});_tmp13A;});goto _LLD1;_LLD4:
if(_tmp138 <= (void*)1?1:*((int*)_tmp138)!= 1)goto _LLD6;_LLD5: goto _LLD7;_LLD6:;
_LLD7:({struct Cyc_Std_String_pa_struct _tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp13C[1]={& _tmp13D};Cyc_Tcutil_terr(
loc,_tag_arr("qualified tunion declarations are not implemented (%s)",sizeof(
char),55),_tag_arr(_tmp13C,sizeof(void*),1));}});return;_LLD1:;}tud_opt=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->tuniondecls,v);}{struct _tuple10 _tmp13F=({struct _tuple10 _tmp13E;_tmp13E.f1=
tud->fields;_tmp13E.f2=tud_opt;_tmp13E;});struct Cyc_Core_Opt*_tmp140;struct Cyc_Core_Opt*
_tmp141;struct Cyc_Core_Opt*_tmp142;struct Cyc_Core_Opt _tmp143;struct Cyc_List_List*
_tmp144;struct Cyc_List_List**_tmp145;struct Cyc_Core_Opt*_tmp146;struct Cyc_Core_Opt*
_tmp147;struct Cyc_Core_Opt _tmp148;struct Cyc_List_List*_tmp149;struct Cyc_List_List**
_tmp14A;struct Cyc_Core_Opt*_tmp14B;struct Cyc_Core_Opt _tmp14C;struct Cyc_Absyn_Tuniondecl**
_tmp14D;struct Cyc_Core_Opt*_tmp14E;struct Cyc_Core_Opt*_tmp14F;struct Cyc_Core_Opt
_tmp150;struct Cyc_Absyn_Tuniondecl**_tmp151;_LLD9: _tmp140=_tmp13F.f1;if(_tmp140
!= 0)goto _LLDB;_tmp141=_tmp13F.f2;if(_tmp141 != 0)goto _LLDB;_LLDA: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,({struct Cyc_Absyn_Tuniondecl**_tmp152=
_cycalloc(sizeof(*_tmp152));_tmp152[0]=tud;_tmp152;}));goto _LLD8;_LLDB: _tmp142=
_tmp13F.f1;if(_tmp142 == 0)goto _LLDD;_tmp143=*_tmp142;_tmp144=(struct Cyc_List_List*)
_tmp143.v;_tmp145=(struct Cyc_List_List**)&(*_tmp13F.f1).v;_tmp146=_tmp13F.f2;if(
_tmp146 != 0)goto _LLDD;_LLDC: {struct Cyc_Absyn_Tuniondecl**_tmp153=({struct Cyc_Absyn_Tuniondecl**
_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->sc=(void*)((void*)3);_tmp155->name=
tud->name;_tmp155->tvs=tvs;_tmp155->fields=0;_tmp155->is_xtunion=tud->is_xtunion;
_tmp155;});_tmp154;});ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,
v,_tmp153);*_tmp145=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp145,tvs,tud);*_tmp153=tud;goto _LLD8;}_LLDD: _tmp147=_tmp13F.f1;if(_tmp147 == 0)
goto _LLDF;_tmp148=*_tmp147;_tmp149=(struct Cyc_List_List*)_tmp148.v;_tmp14A=(
struct Cyc_List_List**)&(*_tmp13F.f1).v;_tmp14B=_tmp13F.f2;if(_tmp14B == 0)goto
_LLDF;_tmp14C=*_tmp14B;_tmp14D=(struct Cyc_Absyn_Tuniondecl**)_tmp14C.v;_LLDE: {
struct Cyc_Absyn_Tuniondecl*_tmp156=*_tmp14D;*_tmp14D=({struct Cyc_Absyn_Tuniondecl*
_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->sc=(void*)((void*)3);_tmp157->name=
tud->name;_tmp157->tvs=tvs;_tmp157->fields=0;_tmp157->is_xtunion=tud->is_xtunion;
_tmp157;});*_tmp14A=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp14A,tvs,tud);*_tmp14D=_tmp156;_tmp151=_tmp14D;goto _LLE0;}_LLDF: _tmp14E=
_tmp13F.f1;if(_tmp14E != 0)goto _LLD8;_tmp14F=_tmp13F.f2;if(_tmp14F == 0)goto _LLD8;
_tmp150=*_tmp14F;_tmp151=(struct Cyc_Absyn_Tuniondecl**)_tmp150.v;_LLE0: {struct
Cyc_Absyn_Tuniondecl*_tmp158=Cyc_Tcdecl_merge_tuniondecl(*_tmp151,tud,loc,Cyc_Tc_tc_msg);
if(_tmp158 == 0)return;else{*_tmp151=(struct Cyc_Absyn_Tuniondecl*)_check_null(
_tmp158);goto _LLD8;}}_LLD8:;}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*
ed){struct _tagged_arr*v=(*ed->name).f2;{void*_tmp159=(*ed->name).f1;struct Cyc_List_List*
_tmp15A;struct Cyc_List_List*_tmp15B;_LLE2: if(_tmp159 <= (void*)1?1:*((int*)
_tmp159)!= 0)goto _LLE4;_tmp15A=((struct Cyc_Absyn_Rel_n_struct*)_tmp159)->f1;if(
_tmp15A != 0)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp159 <= (void*)1?1:*((int*)
_tmp159)!= 1)goto _LLE6;_tmp15B=((struct Cyc_Absyn_Abs_n_struct*)_tmp159)->f1;if(
_tmp15B != 0)goto _LLE6;_LLE5: goto _LLE1;_LLE6:;_LLE7:({struct Cyc_Std_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name);{
void*_tmp15C[1]={& _tmp15D};Cyc_Tcutil_terr(loc,_tag_arr("qualified enum declarations are not implemented (%s)",
sizeof(char),53),_tag_arr(_tmp15C,sizeof(void*),1));}});return;_LLE1:;}(*ed->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp15F;_tmp15F.tag=1;_tmp15F.f1=te->ns;_tmp15F;});
_tmp15E;});if(ed->fields != 0){struct _RegionHandle _tmp160=_new_region("uprev_rgn");
struct _RegionHandle*uprev_rgn=& _tmp160;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct
Cyc_Absyn_Enumfield*_tmp161=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,(*_tmp161->name).f2))({
struct Cyc_Std_String_pa_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(struct
_tagged_arr)*(*_tmp161->name).f2;{void*_tmp162[1]={& _tmp163};Cyc_Tcutil_terr(
_tmp161->loc,_tag_arr("duplicate field name %s",sizeof(char),24),_tag_arr(
_tmp162,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*_tmp164=
_region_malloc(uprev_rgn,sizeof(*_tmp164));_tmp164->hd=(*_tmp161->name).f2;
_tmp164->tl=prev_fields;_tmp164;});}if(_tmp161->tag == 0)_tmp161->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp161->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp161->tag)))({struct Cyc_Std_String_pa_struct
_tmp167;_tmp167.tag=0;_tmp167.f1=(struct _tagged_arr)*(*_tmp161->name).f2;{struct
Cyc_Std_String_pa_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(struct _tagged_arr)*v;{
void*_tmp165[2]={& _tmp166,& _tmp167};Cyc_Tcutil_terr(loc,_tag_arr("enum %s, field %s: expression is not constant",
sizeof(char),46),_tag_arr(_tmp165,sizeof(void*),2));}}});}{unsigned int _tmp169;
int _tmp16A;struct _tuple4 _tmp168=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp161->tag));_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;if(!_tmp16A)({
void*_tmp16B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("Cyclone enum tags cannot use sizeof or offsetof",
sizeof(char),48),_tag_arr(_tmp16B,sizeof(void*),0));});tag_count=_tmp169 + 1;(*
_tmp161->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp16C=_cycalloc(
sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_Absyn_Abs_n_struct _tmp16D;_tmp16D.tag=1;
_tmp16D.f1=te->ns;_tmp16D;});_tmp16C;});}}};_pop_region(uprev_rgn);}{struct
_handler_cons _tmp16E;_push_handler(& _tmp16E);{int _tmp170=0;if(setjmp(_tmp16E.handler))
_tmp170=1;if(!_tmp170){{struct Cyc_Absyn_Enumdecl**_tmp171=((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp172=Cyc_Tcdecl_merge_enumdecl(*_tmp171,ed,loc,Cyc_Tc_tc_msg);
if(_tmp172 == 0){_npop_handler(0);return;}*_tmp171=(struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp172);};_pop_handler();}else{void*_tmp16F=(void*)_exn_thrown;void*
_tmp174=_tmp16F;_LLE9: if(_tmp174 != Cyc_Dict_Absent)goto _LLEB;_LLEA: {struct Cyc_Absyn_Enumdecl**
_tmp175=({struct Cyc_Absyn_Enumdecl**_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[
0]=ed;_tmp176;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp175);goto _LLE8;}_LLEB:;_LLEC:(void)_throw(_tmp174);_LLE8:;}}}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp177=(struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple6*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp177->name).f2,({struct _tuple6*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->f1=(
void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp179=_cycalloc(sizeof(*_tmp179));
_tmp179[0]=({struct Cyc_Tcenv_EnumRes_struct _tmp17A;_tmp17A.tag=3;_tmp17A.f1=ed;
_tmp17A.f2=_tmp177;_tmp17A;});_tmp179;});_tmp178->f2=1;_tmp178;}));}}}static int
Cyc_Tc_okay_externC(struct Cyc_Position_Segment*loc,void*sc){void*_tmp17B=sc;
_LLEE: if((int)_tmp17B != 0)goto _LLF0;_LLEF:({void*_tmp17C[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("static declaration nested within extern \"C\"",sizeof(char),44),
_tag_arr(_tmp17C,sizeof(void*),0));});return 0;_LLF0: if((int)_tmp17B != 1)goto
_LLF2;_LLF1:({void*_tmp17D[0]={};Cyc_Tcutil_warn(loc,_tag_arr("abstract declaration nested within extern \"C\"",
sizeof(char),46),_tag_arr(_tmp17D,sizeof(void*),0));});return 0;_LLF2: if((int)
_tmp17B != 2)goto _LLF4;_LLF3: goto _LLF5;_LLF4: if((int)_tmp17B != 5)goto _LLF6;_LLF5:
goto _LLF7;_LLF6: if((int)_tmp17B != 3)goto _LLF8;_LLF7: return 1;_LLF8: if((int)
_tmp17B != 4)goto _LLED;_LLF9:({void*_tmp17E[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested extern \"C\" declaration",
sizeof(char),30),_tag_arr(_tmp17E,sizeof(void*),0));});return 1;_LLED:;}static
void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int
in_externC,int check_var_init){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp17F=ds0;for(0;_tmp17F != 0;(last=_tmp17F,_tmp17F=_tmp17F->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp17F->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp180=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp181;struct Cyc_Absyn_Fndecl*
_tmp182;struct Cyc_Absyn_Typedefdecl*_tmp183;struct Cyc_Absyn_Aggrdecl*_tmp184;
struct Cyc_Absyn_Tuniondecl*_tmp185;struct Cyc_Absyn_Enumdecl*_tmp186;struct
_tagged_arr*_tmp187;struct Cyc_List_List*_tmp188;struct _tuple0*_tmp189;struct
_tuple0 _tmp18A;void*_tmp18B;struct _tagged_arr*_tmp18C;struct Cyc_List_List*
_tmp18D;struct Cyc_List_List*_tmp18E;_LLFB: if(*((int*)_tmp180)!= 2)goto _LLFD;
_LLFC: goto _LLFE;_LLFD: if(*((int*)_tmp180)!= 3)goto _LLFF;_LLFE:({void*_tmp18F[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("top level let-declarations are not implemented",
sizeof(char),47),_tag_arr(_tmp18F,sizeof(void*),0));});goto _LLFA;_LLFF: if(*((int*)
_tmp180)!= 0)goto _LL101;_tmp181=((struct Cyc_Absyn_Var_d_struct*)_tmp180)->f1;
_LL100: if(in_externC?Cyc_Tc_okay_externC(d->loc,(void*)_tmp181->sc): 0)(void*)(
_tmp181->sc=(void*)((void*)4));if(_tmp181->initializer != 0){void*_tmp190=(void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp181->initializer))->r;void*_tmp191;_LL112:
if(*((int*)_tmp190)!= 19)goto _LL114;_tmp191=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp190)->f2;_LL113: {struct Cyc_Dict_Dict*_tmp193;struct Cyc_List_List*_tmp194;
struct Cyc_Absyn_Exp*_tmp195;struct _tuple5 _tmp192=Cyc_Tcgenrep_tcGenrep(te,ge,loc,
_tmp191,dict);_tmp193=_tmp192.f1;_tmp194=_tmp192.f2;_tmp195=_tmp192.f3;dict=
_tmp193;Cyc_Tc_tc_decls(te,_tmp194,in_externC,check_var_init);_tmp181->initializer=(
struct Cyc_Absyn_Exp*)_tmp195;Cyc_Tc_tcVardecl(te,ge,loc,_tmp181,check_var_init);
if(_tmp194 != 0){if(last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp194,_tmp17F);((struct Cyc_List_List*)
_check_null(last))->tl=_tmp194;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp196;_tmp196.hd=(struct Cyc_Absyn_Decl*)_tmp17F->hd;_tmp196.tl=_tmp17F->tl;
_tmp196;});(struct Cyc_Absyn_Decl*)(_tmp17F->hd=(void*)((struct Cyc_Absyn_Decl*)
_tmp194->hd));_tmp17F->tl=_tmp194->tl;(struct Cyc_Absyn_Decl*)(_tmp194->hd=(void*)((
struct Cyc_Absyn_Decl*)tmp.hd));_tmp194->tl=tmp.tl;((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp17F,
_tmp194);}}continue;}_LL114:;_LL115: goto _LL111;_LL111:;}Cyc_Tc_tcVardecl(te,ge,
loc,_tmp181,check_var_init);goto _LLFA;_LL101: if(*((int*)_tmp180)!= 1)goto _LL103;
_tmp182=((struct Cyc_Absyn_Fn_d_struct*)_tmp180)->f1;_LL102: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp182->sc): 0)(void*)(_tmp182->sc=(void*)((void*)4));Cyc_Tc_tcFndecl(
te,ge,loc,_tmp182);goto _LLFA;_LL103: if(*((int*)_tmp180)!= 7)goto _LL105;_tmp183=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp180)->f1;_LL104: Cyc_Tc_tcTypedefdecl(te,ge,
loc,_tmp183);goto _LLFA;_LL105: if(*((int*)_tmp180)!= 4)goto _LL107;_tmp184=((
struct Cyc_Absyn_Aggr_d_struct*)_tmp180)->f1;_LL106: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp184->sc): 0)(void*)(_tmp184->sc=(void*)((void*)4));Cyc_Tc_tcAggrdecl(
te,ge,loc,_tmp184);goto _LLFA;_LL107: if(*((int*)_tmp180)!= 5)goto _LL109;_tmp185=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp180)->f1;_LL108: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp185->sc): 0)(void*)(_tmp185->sc=(void*)((void*)4));Cyc_Tc_tcTuniondecl(
te,ge,loc,_tmp185);goto _LLFA;_LL109: if(*((int*)_tmp180)!= 6)goto _LL10B;_tmp186=((
struct Cyc_Absyn_Enum_d_struct*)_tmp180)->f1;_LL10A: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp186->sc): 0)(void*)(_tmp186->sc=(void*)((void*)4));Cyc_Tc_tcEnumdecl(
te,ge,loc,_tmp186);goto _LLFA;_LL10B: if(*((int*)_tmp180)!= 8)goto _LL10D;_tmp187=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp180)->f1;_tmp188=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp180)->f2;_LL10C: {struct Cyc_List_List*_tmp197=te->ns;struct Cyc_List_List*
_tmp198=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp197,({struct Cyc_List_List*_tmp199=_cycalloc(sizeof(*_tmp199));
_tmp199->hd=_tmp187;_tmp199->tl=0;_tmp199;}));if(!((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(ge->namespaces,_tmp187)){ge->namespaces=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
ge->namespaces,_tmp187);te->ae=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp198,
Cyc_Tcenv_empty_genv());}te->ns=_tmp198;Cyc_Tc_tc_decls(te,_tmp188,in_externC,
check_var_init);te->ns=_tmp197;goto _LLFA;}_LL10D: if(*((int*)_tmp180)!= 9)goto
_LL10F;_tmp189=((struct Cyc_Absyn_Using_d_struct*)_tmp180)->f1;_tmp18A=*_tmp189;
_tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;_tmp18D=((struct Cyc_Absyn_Using_d_struct*)
_tmp180)->f2;_LL10E: {struct _tagged_arr*first;struct Cyc_List_List*rest;{void*
_tmp19A=_tmp18B;struct Cyc_List_List*_tmp19B;struct Cyc_List_List*_tmp19C;struct
Cyc_List_List*_tmp19D;struct Cyc_List_List _tmp19E;struct _tagged_arr*_tmp19F;
struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_List_List
_tmp1A2;struct _tagged_arr*_tmp1A3;struct Cyc_List_List*_tmp1A4;_LL117: if((int)
_tmp19A != 0)goto _LL119;_LL118: goto _LL11A;_LL119: if(_tmp19A <= (void*)1?1:*((int*)
_tmp19A)!= 0)goto _LL11B;_tmp19B=((struct Cyc_Absyn_Rel_n_struct*)_tmp19A)->f1;if(
_tmp19B != 0)goto _LL11B;_LL11A: goto _LL11C;_LL11B: if(_tmp19A <= (void*)1?1:*((int*)
_tmp19A)!= 1)goto _LL11D;_tmp19C=((struct Cyc_Absyn_Abs_n_struct*)_tmp19A)->f1;if(
_tmp19C != 0)goto _LL11D;_LL11C: first=_tmp18C;rest=0;goto _LL116;_LL11D: if(_tmp19A
<= (void*)1?1:*((int*)_tmp19A)!= 0)goto _LL11F;_tmp19D=((struct Cyc_Absyn_Rel_n_struct*)
_tmp19A)->f1;if(_tmp19D == 0)goto _LL11F;_tmp19E=*_tmp19D;_tmp19F=(struct
_tagged_arr*)_tmp19E.hd;_tmp1A0=_tmp19E.tl;_LL11E: _tmp1A3=_tmp19F;_tmp1A4=
_tmp1A0;goto _LL120;_LL11F: if(_tmp19A <= (void*)1?1:*((int*)_tmp19A)!= 1)goto
_LL116;_tmp1A1=((struct Cyc_Absyn_Abs_n_struct*)_tmp19A)->f1;if(_tmp1A1 == 0)goto
_LL116;_tmp1A2=*_tmp1A1;_tmp1A3=(struct _tagged_arr*)_tmp1A2.hd;_tmp1A4=_tmp1A2.tl;
_LL120: first=_tmp1A3;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp1A4,({struct Cyc_List_List*_tmp1A5=
_cycalloc(sizeof(*_tmp1A5));_tmp1A5->hd=_tmp18C;_tmp1A5->tl=0;_tmp1A5;}));goto
_LL116;_LL116:;}{struct Cyc_List_List*_tmp1A6=Cyc_Tcenv_resolve_namespace(te,loc,
first,rest);ge->availables=({struct Cyc_List_List*_tmp1A7=_cycalloc(sizeof(*
_tmp1A7));_tmp1A7->hd=_tmp1A6;_tmp1A7->tl=ge->availables;_tmp1A7;});Cyc_Tc_tc_decls(
te,_tmp18D,in_externC,check_var_init);ge->availables=((struct Cyc_List_List*)
_check_null(ge->availables))->tl;goto _LLFA;}}_LL10F: if(*((int*)_tmp180)!= 10)
goto _LLFA;_tmp18E=((struct Cyc_Absyn_ExternC_d_struct*)_tmp180)->f1;_LL110: Cyc_Tc_tc_decls(
te,_tmp18E,1,check_var_init);goto _LLFA;_LLFA:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init);}struct Cyc_Tc_TreeshakeEnv{struct
_RegionHandle*rgn;struct Cyc_Dict_Dict*nsdict;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*
Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,
env,ds);}struct _tuple11{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int
Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*
_tmp1A8=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1A9;struct Cyc_List_List*_tmp1AA;
struct Cyc_List_List**_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List**
_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List**_tmp1AF;_LL122: if(*((
int*)_tmp1A8)!= 0)goto _LL124;_tmp1A9=((struct Cyc_Absyn_Var_d_struct*)_tmp1A8)->f1;
_LL123: if((void*)_tmp1A9->sc != (void*)3?(void*)_tmp1A9->sc != (void*)4: 0)return 1;{
struct _tuple0 _tmp1B1;void*_tmp1B2;struct _tagged_arr*_tmp1B3;struct _tuple0*
_tmp1B0=_tmp1A9->name;_tmp1B1=*_tmp1B0;_tmp1B2=_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{
struct Cyc_List_List*ns;{void*_tmp1B4=_tmp1B2;struct Cyc_List_List*_tmp1B5;struct
Cyc_List_List*_tmp1B6;_LL12D: if((int)_tmp1B4 != 0)goto _LL12F;_LL12E: ns=0;goto
_LL12C;_LL12F: if(_tmp1B4 <= (void*)1?1:*((int*)_tmp1B4)!= 0)goto _LL131;_tmp1B5=((
struct Cyc_Absyn_Rel_n_struct*)_tmp1B4)->f1;_LL130: ns=_tmp1B5;goto _LL12C;_LL131:
if(_tmp1B4 <= (void*)1?1:*((int*)_tmp1B4)!= 1)goto _LL12C;_tmp1B6=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1B4)->f1;_LL132: ns=_tmp1B6;goto _LL12C;_LL12C:;}{struct _tuple11*_tmp1B7=((
struct _tuple11*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
env->nsdict,ns);struct Cyc_Tcenv_Genv*_tmp1B8=(*_tmp1B7).f1;int _tmp1B9=(*((struct
_tuple6*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp1B8->ordinaries,
_tmp1B3)).f2;if(!_tmp1B9)(*_tmp1B7).f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_rinsert)(env->rgn,(*
_tmp1B7).f2,_tmp1B3);return _tmp1B9;}}}_LL124: if(*((int*)_tmp1A8)!= 10)goto _LL126;
_tmp1AA=((struct Cyc_Absyn_ExternC_d_struct*)_tmp1A8)->f1;_tmp1AB=(struct Cyc_List_List**)&((
struct Cyc_Absyn_ExternC_d_struct*)_tmp1A8)->f1;_LL125: _tmp1AD=_tmp1AB;goto _LL127;
_LL126: if(*((int*)_tmp1A8)!= 9)goto _LL128;_tmp1AC=((struct Cyc_Absyn_Using_d_struct*)
_tmp1A8)->f2;_tmp1AD=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)
_tmp1A8)->f2;_LL127: _tmp1AF=_tmp1AD;goto _LL129;_LL128: if(*((int*)_tmp1A8)!= 8)
goto _LL12A;_tmp1AE=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1A8)->f2;_tmp1AF=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_struct*)_tmp1A8)->f2;_LL129:*
_tmp1AF=Cyc_Tc_treeshake_f(env,*_tmp1AF);return 1;_LL12A:;_LL12B: return 1;_LL121:;}
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _tagged_arr*x,
void*y){return !((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
set,x);}static struct _tuple11*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,
struct Cyc_Tcenv_Genv*ge){return({struct _tuple11*_tmp1BA=_region_malloc(rgn,
sizeof(*_tmp1BA));_tmp1BA->f1=ge;_tmp1BA->f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_rempty)(
rgn,Cyc_Std_strptrcmp);_tmp1BA;});}struct _tuple12{struct Cyc_List_List*f1;struct
_tuple11*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*ds){struct _RegionHandle _tmp1BB=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp1BB;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv _tmp1BC=({
struct Cyc_Tc_TreeshakeEnv _tmp1C7;_tmp1C7.rgn=rgn;_tmp1C7.nsdict=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),
struct _RegionHandle*env,struct Cyc_Dict_Dict*d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,
rgn,te->ae);_tmp1C7;});struct Cyc_List_List*_tmp1BD=Cyc_Tc_treeshake_f(& _tmp1BC,
ds);if(((int(*)(struct Cyc_Dict_Dict*d))Cyc_Dict_is_empty)(_tmp1BC.nsdict)){
struct Cyc_List_List*_tmp1BE=_tmp1BD;_npop_handler(0);return _tmp1BE;}{struct Cyc_Iter_Iter
_tmp1BF=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp1BC.nsdict);struct _tuple12 _tmp1C0=*((
struct _tuple12*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(
rgn,_tmp1BC.nsdict);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next)(
_tmp1BF,& _tmp1C0)){struct _tuple11*_tmp1C2;struct _tuple11 _tmp1C3;struct Cyc_Tcenv_Genv*
_tmp1C4;struct Cyc_Set_Set*_tmp1C5;struct _tuple12 _tmp1C1=_tmp1C0;_tmp1C2=_tmp1C1.f2;
_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;_tmp1C4->ordinaries=((
struct Cyc_Dict_Dict*(*)(int(*f)(struct Cyc_Set_Set*,struct _tagged_arr*,struct
_tuple6*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict*d))Cyc_Dict_filter_c)((int(*)(
struct Cyc_Set_Set*set,struct _tagged_arr*x,struct _tuple6*y))Cyc_Tc_treeshake_remove_f,
_tmp1C5,_tmp1C4->ordinaries);}{struct Cyc_List_List*_tmp1C6=_tmp1BD;_npop_handler(
0);return _tmp1C6;}}};_pop_region(rgn);}
