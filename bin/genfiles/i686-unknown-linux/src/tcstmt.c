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

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct
Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_VarargInfo{
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
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Nullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_TaggedArray_ps_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int
f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_Buffer_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*
clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _tagged_arr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*cs);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r,int resetable);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
int*Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _tagged_arr msg_part);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple7{
unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));s->try_depth=
Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct _tuple6*_tmp1;struct Cyc_List_List*
_tmp2;struct Cyc_Tcpat_TcPatResult _tmp0=pat_res;_tmp1=_tmp0.tvars_and_bounds_opt;
_tmp2=_tmp0.patvars;{struct Cyc_List_List*_tmp3=_tmp1 == 0?0:(*((struct _tuple6*)
_tmp1)).f1;struct Cyc_List_List*_tmp4=_tmp1 == 0?0:(*((struct _tuple6*)_tmp1)).f2;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp3);te2=Cyc_Tcenv_new_outlives_constraints(
te2,_tmp4);if(new_block)te2=Cyc_Tcenv_new_block(loc,te2);{struct Cyc_Core_Opt*
_tmp5=({struct Cyc_Core_Opt*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->v=(void*)Cyc_Tcenv_curr_rgn(
te2);_tmp8;});{struct Cyc_List_List*_tmp6=_tmp2;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
te2=Cyc_Tcenv_add_pat_var(loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp6->hd);((struct
Cyc_Absyn_Vardecl*)_tmp6->hd)->rgn=_tmp5;}}if(where_opt != 0)Cyc_Tcexp_tcTest(te2,(
struct Cyc_Absyn_Exp*)_check_null(where_opt),({const char*_tmp7="switch clause guard";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,20));}));if(_tmp2 != 0)te2=
Cyc_Tcenv_set_encloser(te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);if(_tmp2 != 0)Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));}}}struct _tuple8{struct Cyc_Tcenv_Tenv*f1;struct Cyc_List_List*
f2;};struct _tuple8 Cyc_Tcstmt_check_forarray_decls(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct Cyc_List_List*decls){({void*_tmp9[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpA="check_forarray_decls";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,21));}),_tag_arr(_tmp9,
sizeof(void*),0));});return({struct _tuple8 _tmpB;_tmpB.f1=te;_tmpB.f2=0;_tmpB;});}
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int
new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmpC=(void*)s0->r;struct Cyc_Absyn_Exp*
_tmpD;struct Cyc_Absyn_Stmt*_tmpE;struct Cyc_Absyn_Stmt*_tmpF;struct Cyc_Absyn_Exp*
_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Stmt*_tmp12;struct Cyc_Absyn_Stmt*
_tmp13;struct _tuple2 _tmp14;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Stmt*
_tmp16;struct Cyc_Absyn_Stmt*_tmp17;struct Cyc_Absyn_Exp*_tmp18;struct _tuple2
_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Stmt*_tmp1B;struct _tuple2
_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Stmt*_tmp1E;struct Cyc_Absyn_Stmt*
_tmp1F;struct Cyc_Absyn_ForArrayInfo _tmp20;struct Cyc_List_List*_tmp21;struct
_tuple2 _tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Stmt*_tmp24;struct
_tuple2 _tmp25;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Stmt*_tmp27;struct Cyc_Absyn_Stmt*
_tmp28;struct Cyc_Absyn_Stmt*_tmp29;struct _tuple2 _tmp2A;struct Cyc_Absyn_Exp*
_tmp2B;struct Cyc_Absyn_Stmt*_tmp2C;struct Cyc_Absyn_Stmt*_tmp2D;struct Cyc_Absyn_Stmt**
_tmp2E;struct Cyc_Absyn_Stmt*_tmp2F;struct Cyc_Absyn_Stmt**_tmp30;struct
_tagged_arr*_tmp31;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_Absyn_Stmt**_tmp33;
struct Cyc_List_List*_tmp34;struct Cyc_Absyn_Switch_clause**_tmp35;struct Cyc_Absyn_Switch_clause***
_tmp36;struct _tagged_arr*_tmp37;struct Cyc_Absyn_Stmt*_tmp38;struct Cyc_Absyn_Exp*
_tmp39;struct Cyc_List_List*_tmp3A;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_List_List*
_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_List_List*_tmp3E;struct Cyc_Absyn_Decl*
_tmp3F;struct Cyc_Absyn_Stmt*_tmp40;struct Cyc_Absyn_Tvar*_tmp41;struct Cyc_Absyn_Vardecl*
_tmp42;int _tmp43;struct Cyc_Absyn_Stmt*_tmp44;struct Cyc_Absyn_Exp*_tmp45;struct
Cyc_Absyn_Stmt*_tmp46;struct Cyc_Absyn_Stmt*_tmp47;_LL1: if((int)_tmpC != 0)goto
_LL3;_LL2: return;_LL3: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 0)goto _LL5;_tmpD=((
struct Cyc_Absyn_Exp_s_struct*)_tmpC)->f1;_LL4: Cyc_Tcexp_tcExp(te,0,_tmpD);
return;_LL5: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 1)goto _LL7;_tmpE=((struct Cyc_Absyn_Seq_s_struct*)
_tmpC)->f1;_tmpF=((struct Cyc_Absyn_Seq_s_struct*)_tmpC)->f2;_LL6: {struct Cyc_Tcenv_Tenv*
_tmp48=Cyc_Tcenv_set_next(te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp4E=
_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp4F;
_tmp4F.tag=0;_tmp4F.f1=_tmpF;_tmp4F;});_tmp4E;}));Cyc_Tcstmt_tcStmt(_tmp48,_tmpE,
1);while(1){void*_tmp49=(void*)_tmpF->r;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Stmt*
_tmp4B;_LL2E: if(_tmp49 <= (void*)1?1:*((int*)_tmp49)!= 1)goto _LL30;_tmp4A=((
struct Cyc_Absyn_Seq_s_struct*)_tmp49)->f1;_tmp4B=((struct Cyc_Absyn_Seq_s_struct*)
_tmp49)->f2;_LL2F: Cyc_Tcstmt_decorate_stmt(te,_tmpF);_tmp48=Cyc_Tcenv_set_next(
te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=_tmp4B;
_tmp4D;});_tmp4C;}));Cyc_Tcstmt_tcStmt(_tmp48,_tmp4A,1);_tmpF=_tmp4B;continue;
_LL30:;_LL31: goto seq_end;_LL2D:;}seq_end: Cyc_Tcstmt_tcStmt(te,_tmpF,1);return;}
_LL7: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 2)goto _LL9;_tmp10=((struct Cyc_Absyn_Return_s_struct*)
_tmpC)->f1;_LL8: {void*t=Cyc_Tcenv_return_typ(te);if(_tmp10 == 0){void*_tmp50=Cyc_Tcutil_compress(
t);_LL33: if((int)_tmp50 != 0)goto _LL35;_LL34: goto _LL32;_LL35:;_LL36:({struct Cyc_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp51[1]={& _tmp53};Cyc_Tcutil_terr(s0->loc,({const char*_tmp52="must return a value of type %s";
_tag_arr(_tmp52,sizeof(char),_get_zero_arr_size(_tmp52,31));}),_tag_arr(_tmp51,
sizeof(void*),1));}});goto _LL32;_LL32:;}else{struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_check_null(_tmp10);Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmp57;_tmp57.tag=0;_tmp57.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_String_pa_struct _tmp56;_tmp56.tag=
0;_tmp56.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp54[2]={& _tmp56,& _tmp57};Cyc_Tcutil_terr(s0->loc,({
const char*_tmp55="returns value of type %s but requires %s";_tag_arr(_tmp55,
sizeof(char),_get_zero_arr_size(_tmp55,41));}),_tag_arr(_tmp54,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}}return;}_LL9: if(_tmpC <= (void*)1?1:*((int*)_tmpC)
!= 3)goto _LLB;_tmp11=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpC)->f1;_tmp12=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpC)->f2;_tmp13=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpC)->f3;_LLA: Cyc_Tcexp_tcTest(te,_tmp11,({const char*_tmp58="if statement";
_tag_arr(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,13));}));Cyc_Tcstmt_tcStmt(
te,_tmp12,1);Cyc_Tcstmt_tcStmt(te,_tmp13,1);return;_LLB: if(_tmpC <= (void*)1?1:*((
int*)_tmpC)!= 4)goto _LLD;_tmp14=((struct Cyc_Absyn_While_s_struct*)_tmpC)->f1;
_tmp15=_tmp14.f1;_tmp16=_tmp14.f2;_tmp17=((struct Cyc_Absyn_While_s_struct*)_tmpC)->f2;
_LLC: Cyc_Tcstmt_decorate_stmt(te,_tmp16);Cyc_Tcexp_tcTest(te,_tmp15,({const char*
_tmp59="while loop";_tag_arr(_tmp59,sizeof(char),_get_zero_arr_size(_tmp59,11));}));
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp16),_tmp17,1);return;_LLD: if(
_tmpC <= (void*)1?1:*((int*)_tmpC)!= 8)goto _LLF;_tmp18=((struct Cyc_Absyn_For_s_struct*)
_tmpC)->f1;_tmp19=((struct Cyc_Absyn_For_s_struct*)_tmpC)->f2;_tmp1A=_tmp19.f1;
_tmp1B=_tmp19.f2;_tmp1C=((struct Cyc_Absyn_For_s_struct*)_tmpC)->f3;_tmp1D=_tmp1C.f1;
_tmp1E=_tmp1C.f2;_tmp1F=((struct Cyc_Absyn_For_s_struct*)_tmpC)->f4;_LLE: Cyc_Tcstmt_decorate_stmt(
te,_tmp1B);Cyc_Tcstmt_decorate_stmt(te,_tmp1E);Cyc_Tcexp_tcExp(te,0,_tmp18);Cyc_Tcexp_tcTest(
te,_tmp1A,({const char*_tmp5A="for loop";_tag_arr(_tmp5A,sizeof(char),
_get_zero_arr_size(_tmp5A,9));}));te=Cyc_Tcenv_set_in_loop(te,_tmp1E);Cyc_Tcstmt_tcStmt(
te,_tmp1F,1);Cyc_Tcexp_tcExp(te,0,_tmp1D);return;_LLF: if(_tmpC <= (void*)1?1:*((
int*)_tmpC)!= 19)goto _LL11;_tmp20=((struct Cyc_Absyn_ForArray_s_struct*)_tmpC)->f1;
_tmp21=_tmp20.defns;_tmp22=_tmp20.condition;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;
_tmp25=_tmp20.delta;_tmp26=_tmp25.f1;_tmp27=_tmp25.f2;_tmp28=_tmp20.body;_LL10:({
void*_tmp5B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp5C="ForArray_s";_tag_arr(_tmp5C,sizeof(char),_get_zero_arr_size(
_tmp5C,11));}),_tag_arr(_tmp5B,sizeof(void*),0));});return;_LL11: if(_tmpC <= (
void*)1?1:*((int*)_tmpC)!= 16)goto _LL13;_tmp29=((struct Cyc_Absyn_Do_s_struct*)
_tmpC)->f1;_tmp2A=((struct Cyc_Absyn_Do_s_struct*)_tmpC)->f2;_tmp2B=_tmp2A.f1;
_tmp2C=_tmp2A.f2;_LL12: Cyc_Tcstmt_decorate_stmt(te,_tmp2C);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
te,_tmp2C),_tmp29,1);Cyc_Tcexp_tcTest(te,_tmp2B,({const char*_tmp5D="do loop";
_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,8));}));return;_LL13: if(
_tmpC <= (void*)1?1:*((int*)_tmpC)!= 5)goto _LL15;_tmp2D=((struct Cyc_Absyn_Break_s_struct*)
_tmpC)->f1;_tmp2E=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Break_s_struct*)
_tmpC)->f1;_LL14: Cyc_Tcenv_process_break(te,s0,_tmp2E);return;_LL15: if(_tmpC <= (
void*)1?1:*((int*)_tmpC)!= 6)goto _LL17;_tmp2F=((struct Cyc_Absyn_Continue_s_struct*)
_tmpC)->f1;_tmp30=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Continue_s_struct*)
_tmpC)->f1;_LL16: Cyc_Tcenv_process_continue(te,s0,_tmp30);return;_LL17: if(_tmpC
<= (void*)1?1:*((int*)_tmpC)!= 7)goto _LL19;_tmp31=((struct Cyc_Absyn_Goto_s_struct*)
_tmpC)->f1;_tmp32=((struct Cyc_Absyn_Goto_s_struct*)_tmpC)->f2;_tmp33=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Goto_s_struct*)_tmpC)->f2;_LL18: Cyc_Tcenv_process_goto(te,s0,
_tmp31,_tmp33);return;_LL19: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 11)goto _LL1B;
_tmp34=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpC)->f1;_tmp35=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpC)->f2;_tmp36=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpC)->f2;_LL1A: {struct _tuple4*_tmp5E=Cyc_Tcenv_process_fallthru(te,s0,_tmp36);
if(_tmp5E == 0){({void*_tmp5F[0]={};Cyc_Tcutil_terr(s0->loc,({const char*_tmp60="fallthru not in a non-last case";
_tag_arr(_tmp60,sizeof(char),_get_zero_arr_size(_tmp60,32));}),_tag_arr(_tmp5F,
sizeof(void*),0));});return;}{struct _tuple4 _tmp62;struct Cyc_List_List*_tmp63;
struct Cyc_List_List*_tmp64;struct _tuple4*_tmp61=(struct _tuple4*)_check_null(
_tmp5E);_tmp62=*_tmp61;_tmp63=_tmp62.f2;_tmp64=_tmp62.f3;{struct Cyc_List_List*
instantiation=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp63);_tmp64=((struct
Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,
_tmp64);for(0;_tmp64 != 0?_tmp34 != 0: 0;(_tmp64=_tmp64->tl,_tmp34=_tmp34->tl)){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp34->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)
_tmp34->hd,(void*)_tmp64->hd)){({struct Cyc_String_pa_struct _tmp68;_tmp68.tag=0;
_tmp68.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)_tmp64->hd);{struct
Cyc_String_pa_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmp34->hd)->topt))->v);{
void*_tmp65[2]={& _tmp67,& _tmp68};Cyc_Tcutil_terr(s0->loc,({const char*_tmp66="fallthru argument has type %s but pattern variable has type %s";
_tag_arr(_tmp66,sizeof(char),_get_zero_arr_size(_tmp66,63));}),_tag_arr(_tmp65,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(_tmp34 != 0)({void*
_tmp69[0]={};Cyc_Tcutil_terr(s0->loc,({const char*_tmp6A="too many arguments to explicit fallthru";
_tag_arr(_tmp6A,sizeof(char),_get_zero_arr_size(_tmp6A,40));}),_tag_arr(_tmp69,
sizeof(void*),0));});if(_tmp64 != 0)({void*_tmp6B[0]={};Cyc_Tcutil_terr(s0->loc,({
const char*_tmp6C="too few arguments to explicit fallthru";_tag_arr(_tmp6C,
sizeof(char),_get_zero_arr_size(_tmp6C,39));}),_tag_arr(_tmp6B,sizeof(void*),0));});
return;}}}_LL1B: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 15)goto _LL1D;_tmp37=((
struct Cyc_Absyn_Label_s_struct*)_tmpC)->f1;_tmp38=((struct Cyc_Absyn_Label_s_struct*)
_tmpC)->f2;_LL1C: Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(s0->loc,Cyc_Tcenv_add_label(
te,_tmp37,_tmp38),({struct Cyc_Absyn_Tvar*_tmp6D=_cycalloc(sizeof(*_tmp6D));
_tmp6D->name=({struct _tagged_arr*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_tmp37);{void*_tmp71[1]={& _tmp73};Cyc_aprintf(({
const char*_tmp72="`%s";_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,4));}),
_tag_arr(_tmp71,sizeof(void*),1));}});_tmp70;});_tmp6D->identity=Cyc_Tcutil_new_tvar_id();
_tmp6D->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6E=_cycalloc(
sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6F;_tmp6F.tag=0;
_tmp6F.f1=(void*)((void*)3);_tmp6F;});_tmp6E;}));_tmp6D;})),_tmp38,0);return;
_LL1D: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 9)goto _LL1F;_tmp39=((struct Cyc_Absyn_Switch_s_struct*)
_tmpC)->f1;_tmp3A=((struct Cyc_Absyn_Switch_s_struct*)_tmpC)->f2;_LL1E: Cyc_Tcexp_tcExp(
te,0,_tmp39);{void*_tmp74=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp39->topt))->v;
te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(te);{struct Cyc_List_List*
scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp3A);for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp75=((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp76=Cyc_Tcpat_tcPat(te,_tmp75,(
void**)& _tmp74);struct Cyc_List_List*_tmp77=_tmp76.tvars_and_bounds_opt == 0?0:(*((
struct _tuple6*)_tmp76.tvars_and_bounds_opt)).f1;if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp75->topt))->v,_tmp74)){({struct Cyc_String_pa_struct
_tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp75->topt))->v);{struct Cyc_String_pa_struct
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp74);{
void*_tmp78[2]={& _tmp7A,& _tmp7B};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,({const char*_tmp79="switch on type %s, but case expects type %s";
_tag_arr(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,44));}),_tag_arr(_tmp78,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(
te,_tmp75);}((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=({struct Cyc_Core_Opt*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->v=_tmp76.patvars;_tmp7C;});Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,_tmp76,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp76.tvars_and_bounds_opt != 0?(*((struct _tuple6*)_tmp76.tvars_and_bounds_opt)).f2
!= 0: 0)te=Cyc_Tcenv_clear_fallthru(te);else{te=Cyc_Tcenv_set_fallthru(te,_tmp77,
_tmp76.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}}Cyc_Tcpat_check_switch_exhaustive(
s0->loc,_tmp3A);return;}_LL1F: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 10)goto
_LL21;_tmp3B=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpC)->f1;_tmp3C=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmpC)->f2;_LL20: {struct Cyc_List_List*_tmp7D=0;for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){
struct Cyc_Absyn_SwitchC_clause _tmp7F;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_Absyn_Stmt*
_tmp81;struct Cyc_Position_Segment*_tmp82;struct Cyc_Absyn_SwitchC_clause*_tmp7E=(
struct Cyc_Absyn_SwitchC_clause*)_tmp3C->hd;_tmp7F=*_tmp7E;_tmp80=_tmp7F.cnst_exp;
_tmp81=_tmp7F.body;_tmp82=_tmp7F.loc;{struct Cyc_Absyn_Pat*pat;if(_tmp80 != 0){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp80));{unsigned int _tmp84;int _tmp85;
struct _tuple7 _tmp83=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp80));_tmp84=_tmp83.f1;_tmp85=_tmp83.f2;if(!_tmp85)({void*_tmp86[
0]={};Cyc_Tcutil_terr(_tmp80->loc,({const char*_tmp87="Cyclone cases do not support sizeof or offsetof";
_tag_arr(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,48));}),_tag_arr(_tmp86,
sizeof(void*),0));});pat=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({struct Cyc_Absyn_Int_p_struct _tmp89;
_tmp89.tag=1;_tmp89.f1=(void*)((void*)0);_tmp89.f2=(int)_tmp84;_tmp89;});_tmp88;}),
_tmp80->loc);}}else{pat=Cyc_Absyn_new_pat((void*)0,_tmp82);}_tmp7D=({struct Cyc_List_List*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->hd=({struct Cyc_Absyn_Switch_clause*
_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->pattern=pat;_tmp8B->pat_vars=0;_tmp8B->where_clause=
0;_tmp8B->body=_tmp81;_tmp8B->loc=_tmp82;_tmp8B;});_tmp8A->tl=_tmp7D;_tmp8A;});}}(
void*)(s0->r=(void*)((void*)(Cyc_Absyn_switch_stmt(_tmp3B,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7D),0))->r));Cyc_Tcstmt_tcStmt(te,
s0,new_block);return;}_LL21: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 17)goto _LL23;
_tmp3D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpC)->f1;_tmp3E=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpC)->f2;_LL22:(void*)(_tmp3D->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({
struct Cyc_Absyn_Seq_s_struct _tmp8D;_tmp8D.tag=1;_tmp8D.f1=Cyc_Absyn_new_stmt((
void*)_tmp3D->r,_tmp3D->loc);_tmp8D.f2=Cyc_Absyn_skip_stmt(_tmp3D->loc);_tmp8D;});
_tmp8C;}),_tmp3D->loc))->r));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(Cyc_Tcenv_enter_try(
te),s0),_tmp3D,1);te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(te);{
struct Cyc_List_List*_tmp8E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp3E);for(0;_tmp8E != 0;_tmp8E=_tmp8E->tl){struct Cyc_Absyn_Pat*_tmp8F=((struct
Cyc_Absyn_Switch_clause*)_tmp8E->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp90=
Cyc_Tcpat_tcPat(te,_tmp8F,(void**)& Cyc_Absyn_exn_typ);struct Cyc_List_List*_tmp91=
_tmp90.tvars_and_bounds_opt == 0?0:(*((struct _tuple6*)_tmp90.tvars_and_bounds_opt)).f1;
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp8F->topt))->v,
Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp94;_tmp94.tag=0;_tmp94.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8F->topt))->v);{void*_tmp92[1]={& _tmp94};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->loc,({const char*_tmp93="expected xtunion exn but found %s";
_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,34));}),_tag_arr(_tmp92,
sizeof(void*),1));}});else{Cyc_Tcpat_check_pat_regions(te,_tmp8F);}((struct Cyc_Absyn_Switch_clause*)
_tmp8E->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*_tmp95));
_tmp95->v=_tmp90.patvars;_tmp95;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)
_tmp8E->hd)->loc,te,_tmp90,((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->body,((
struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->where_clause,1);if(_tmp90.tvars_and_bounds_opt
!= 0?(*((struct _tuple6*)_tmp90.tvars_and_bounds_opt)).f2 != 0: 0)te=Cyc_Tcenv_clear_fallthru(
te);else{te=Cyc_Tcenv_set_fallthru(te,_tmp91,_tmp90.patvars,(struct Cyc_Absyn_Switch_clause*)
_tmp8E->hd);}}}Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp3E);return;_LL23: if(
_tmpC <= (void*)1?1:*((int*)_tmpC)!= 12)goto _LL25;_tmp3F=((struct Cyc_Absyn_Decl_s_struct*)
_tmpC)->f1;_tmp40=((struct Cyc_Absyn_Decl_s_struct*)_tmpC)->f2;_LL24: {struct
_tagged_arr unimp_msg_part;if(new_block)te=Cyc_Tcenv_new_block(s0->loc,te);{void*
_tmp96=(void*)_tmp3F->r;struct Cyc_Absyn_Vardecl*_tmp97;struct Cyc_Absyn_Pat*
_tmp98;struct Cyc_Core_Opt*_tmp99;struct Cyc_Core_Opt**_tmp9A;struct Cyc_Absyn_Exp*
_tmp9B;struct Cyc_List_List*_tmp9C;struct _tagged_arr*_tmp9D;struct Cyc_List_List*
_tmp9E;struct _tuple0*_tmp9F;struct Cyc_List_List*_tmpA0;_LL38: if(*((int*)_tmp96)
!= 0)goto _LL3A;_tmp97=((struct Cyc_Absyn_Var_d_struct*)_tmp96)->f1;_LL39: {struct
Cyc_Absyn_Vardecl _tmpA2;void*_tmpA3;struct _tuple0*_tmpA4;struct _tuple0 _tmpA5;
void*_tmpA6;struct _tagged_arr*_tmpA7;struct Cyc_Absyn_Tqual _tmpA8;void*_tmpA9;
struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Core_Opt*_tmpAB;struct Cyc_Core_Opt**_tmpAC;
struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Vardecl*_tmpA1=_tmp97;_tmpA2=*_tmpA1;
_tmpA3=(void*)_tmpA2.sc;_tmpA4=_tmpA2.name;_tmpA5=*_tmpA4;_tmpA6=_tmpA5.f1;
_tmpA7=_tmpA5.f2;_tmpA8=_tmpA2.tq;_tmpA9=(void*)_tmpA2.type;_tmpAA=_tmpA2.initializer;
_tmpAB=_tmpA2.rgn;_tmpAC=(struct Cyc_Core_Opt**)&(*_tmpA1).rgn;_tmpAD=_tmpA2.attributes;{
void*_tmpAE=Cyc_Tcenv_curr_rgn(te);int is_local;{void*_tmpAF=_tmpA3;_LL4F: if((int)
_tmpAF != 0)goto _LL51;_LL50: goto _LL52;_LL51: if((int)_tmpAF != 3)goto _LL53;_LL52:
goto _LL54;_LL53: if((int)_tmpAF != 4)goto _LL55;_LL54: is_local=0;goto _LL4E;_LL55:
if((int)_tmpAF != 1)goto _LL57;_LL56:({void*_tmpB0[0]={};Cyc_Tcutil_terr(_tmp3F->loc,({
const char*_tmpB1="bad abstract scope for local variable";_tag_arr(_tmpB1,sizeof(
char),_get_zero_arr_size(_tmpB1,38));}),_tag_arr(_tmpB0,sizeof(void*),0));});
goto _LL58;_LL57: if((int)_tmpAF != 5)goto _LL59;_LL58: goto _LL5A;_LL59: if((int)
_tmpAF != 2)goto _LL4E;_LL5A: is_local=1;goto _LL4E;_LL4E:;}*((struct Cyc_Core_Opt**)
_tmpAC)=is_local?({struct Cyc_Core_Opt*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=(
void*)_tmpAE;_tmpB2;}):({struct Cyc_Core_Opt*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3->v=(void*)((void*)2);_tmpB3;});{void*_tmpB4=_tmpA6;struct Cyc_List_List*
_tmpB5;struct Cyc_List_List*_tmpB6;_LL5C: if((int)_tmpB4 != 0)goto _LL5E;_LL5D: goto
_LL5B;_LL5E: if(_tmpB4 <= (void*)1?1:*((int*)_tmpB4)!= 0)goto _LL60;_tmpB5=((struct
Cyc_Absyn_Rel_n_struct*)_tmpB4)->f1;if(_tmpB5 != 0)goto _LL60;_LL5F:(*((struct
_tuple0*)_tmp97->name)).f1=(void*)0;goto _LL5B;_LL60: if(_tmpB4 <= (void*)1?1:*((
int*)_tmpB4)!= 1)goto _LL62;_tmpB6=((struct Cyc_Absyn_Abs_n_struct*)_tmpB4)->f1;
_LL61:(int)_throw(({void*_tmpB7[0]={};Cyc_Tcutil_impos(({const char*_tmpB8="tcstmt: Abs_n declaration";
_tag_arr(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,26));}),_tag_arr(_tmpB7,
sizeof(void*),0));}));_LL62:;_LL63:({void*_tmpB9[0]={};Cyc_Tcutil_terr(_tmp3F->loc,({
const char*_tmpBA="cannot declare a qualified local variable";_tag_arr(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,42));}),_tag_arr(_tmpB9,sizeof(void*),0));});
goto _LL5B;_LL5B:;}{void*_tmpBB=Cyc_Tcutil_compress(_tmpA9);struct Cyc_Absyn_ArrayInfo
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Tqual _tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct
Cyc_Absyn_Conref*_tmpC0;_LL65: if(_tmpBB <= (void*)3?1:*((int*)_tmpBB)!= 7)goto
_LL67;_tmpBC=((struct Cyc_Absyn_ArrayType_struct*)_tmpBB)->f1;_tmpBD=(void*)
_tmpBC.elt_type;_tmpBE=_tmpBC.tq;_tmpBF=_tmpBC.num_elts;if(_tmpBF != 0)goto _LL67;
_tmpC0=_tmpBC.zero_term;if(!(_tmp97->initializer != 0))goto _LL67;_LL66:{void*
_tmpC1=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp97->initializer))->r;void*
_tmpC2;struct _tagged_arr _tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;_LL6A: if(*((int*)_tmpC1)!= 0)goto _LL6C;_tmpC2=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpC1)->f1;if(_tmpC2 <= (void*)1?1:*((
int*)_tmpC2)!= 5)goto _LL6C;_tmpC3=((struct Cyc_Absyn_String_c_struct*)_tmpC2)->f1;
_LL6B: _tmpA9=(void*)(_tmp97->type=(void*)Cyc_Absyn_array_typ(_tmpBD,_tmpBE,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmpC3,sizeof(char)),0),
_tmpC0));goto _LL69;_LL6C: if(*((int*)_tmpC1)!= 27)goto _LL6E;_tmpC4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpC1)->f2;_LL6D: _tmpA9=(void*)(_tmp97->type=(void*)Cyc_Absyn_array_typ(_tmpBD,
_tmpBE,(struct Cyc_Absyn_Exp*)_tmpC4,_tmpC0));goto _LL69;_LL6E: if(*((int*)_tmpC1)
!= 34)goto _LL70;_tmpC5=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpC1)->f2;
_LL6F: _tmpC6=_tmpC5;goto _LL71;_LL70: if(*((int*)_tmpC1)!= 26)goto _LL72;_tmpC6=((
struct Cyc_Absyn_Array_e_struct*)_tmpC1)->f1;_LL71: _tmpA9=(void*)(_tmp97->type=(
void*)Cyc_Absyn_array_typ(_tmpBD,_tmpBE,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC6),0),_tmpC0));
goto _LL69;_LL72:;_LL73: goto _LL69;_LL69:;}goto _LL64;_LL67:;_LL68: goto _LL64;_LL64:;}{
struct Cyc_List_List*_tmpC7=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);int _tmpC8=
!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,te,_tmpC7,(void*)1,_tmpC8,_tmpA9);{
struct Cyc_Tcenv_Tenv*_tmpC9=Cyc_Tcenv_add_local_var(_tmp40->loc,te,_tmp97);if(
_tmpA3 == (void*)3?1: _tmpA3 == (void*)4)({void*_tmpCA[0]={};Cyc_Tcutil_terr(_tmp3F->loc,({
const char*_tmpCB="local extern declarations not yet supported";_tag_arr(_tmpCB,
sizeof(char),_get_zero_arr_size(_tmpCB,44));}),_tag_arr(_tmpCA,sizeof(void*),0));});
if(_tmpAA != 0){Cyc_Tcexp_tcExpInitializer(_tmpC9,(void**)& _tmpA9,(struct Cyc_Absyn_Exp*)
_check_null(_tmpAA));if(!is_local?!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmpAA)): 0)({void*_tmpCC[0]={};Cyc_Tcutil_terr(_tmp3F->loc,({const
char*_tmpCD="initializer needs to be a constant expression";_tag_arr(_tmpCD,
sizeof(char),_get_zero_arr_size(_tmpCD,46));}),_tag_arr(_tmpCC,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_assign(_tmpC9,(struct Cyc_Absyn_Exp*)_check_null(_tmpAA),
_tmpA9)){({struct Cyc_String_pa_struct _tmpD2;_tmpD2.tag=0;_tmpD2.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA->topt))->v);{struct Cyc_String_pa_struct _tmpD1;_tmpD1.tag=0;_tmpD1.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpA9);{struct Cyc_String_pa_struct
_tmpD0;_tmpD0.tag=0;_tmpD0.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmpA7);{
void*_tmpCE[3]={& _tmpD0,& _tmpD1,& _tmpD2};Cyc_Tcutil_terr(_tmp3F->loc,({const char*
_tmpCF="%s declared with type \n%s\n but initialized with type \n%s";_tag_arr(
_tmpCF,sizeof(char),_get_zero_arr_size(_tmpCF,57));}),_tag_arr(_tmpCE,sizeof(
void*),3));}}}});Cyc_Tcutil_explain_failure();}}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmpC9,s0),_tmp40,0);return;}}}}_LL3A: if(*((int*)_tmp96)!= 2)goto _LL3C;_tmp98=((
struct Cyc_Absyn_Let_d_struct*)_tmp96)->f1;_tmp99=((struct Cyc_Absyn_Let_d_struct*)
_tmp96)->f2;_tmp9A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)_tmp96)->f2;
_tmp9B=((struct Cyc_Absyn_Let_d_struct*)_tmp96)->f3;_LL3B: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp9B);{struct Cyc_Tcpat_TcPatResult _tmpD3=Cyc_Tcpat_tcPat(te,_tmp98,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(_tmp9B->topt))->v)));*((struct
Cyc_Core_Opt**)_tmp9A)=({struct Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));
_tmpD4->v=_tmpD3.patvars;_tmpD4;});if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp98->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9B->topt))->v)){({
struct Cyc_String_pa_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp9B->topt))->v);{struct Cyc_String_pa_struct
_tmpD7;_tmpD7.tag=0;_tmpD7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp98->topt))->v);{void*_tmpD5[2]={& _tmpD7,&
_tmpD8};Cyc_Tcutil_terr(_tmp3F->loc,({const char*_tmpD6="pattern type %s does not match definition type %s";
_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,50));}),_tag_arr(_tmpD5,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(
te,_tmp98);}Cyc_Tcpat_check_let_pat_exhaustive(_tmp98->loc,_tmp98);Cyc_Tcstmt_pattern_synth(
s0->loc,te,_tmpD3,_tmp40,0,0);return;}_LL3C: if(*((int*)_tmp96)!= 3)goto _LL3E;
_tmp9C=((struct Cyc_Absyn_Letv_d_struct*)_tmp96)->f1;_LL3D: {void*_tmpD9=Cyc_Tcenv_curr_rgn(
te);struct Cyc_Tcenv_Tenv*_tmpDA=te;for(0;_tmp9C != 0;_tmp9C=_tmp9C->tl){struct Cyc_Absyn_Vardecl*
_tmpDB=(struct Cyc_Absyn_Vardecl*)_tmp9C->hd;struct Cyc_Absyn_Vardecl _tmpDD;struct
_tuple0*_tmpDE;struct _tuple0 _tmpDF;void*_tmpE0;void*_tmpE1;struct Cyc_Core_Opt*
_tmpE2;struct Cyc_Core_Opt**_tmpE3;struct Cyc_Absyn_Vardecl*_tmpDC=_tmpDB;_tmpDD=*
_tmpDC;_tmpDE=_tmpDD.name;_tmpDF=*_tmpDE;_tmpE0=_tmpDF.f1;_tmpE1=(void*)_tmpDD.type;
_tmpE2=_tmpDD.rgn;_tmpE3=(struct Cyc_Core_Opt**)&(*_tmpDC).rgn;*((struct Cyc_Core_Opt**)
_tmpE3)=({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=(void*)
_tmpD9;_tmpE4;});{void*_tmpE5=_tmpE0;struct Cyc_List_List*_tmpE6;struct Cyc_List_List*
_tmpE7;_LL75: if((int)_tmpE5 != 0)goto _LL77;_LL76: goto _LL74;_LL77: if(_tmpE5 <= (
void*)1?1:*((int*)_tmpE5)!= 0)goto _LL79;_tmpE6=((struct Cyc_Absyn_Rel_n_struct*)
_tmpE5)->f1;if(_tmpE6 != 0)goto _LL79;_LL78:(*((struct _tuple0*)_tmpDB->name)).f1=(
void*)0;goto _LL74;_LL79: if(_tmpE5 <= (void*)1?1:*((int*)_tmpE5)!= 1)goto _LL7B;
_tmpE7=((struct Cyc_Absyn_Abs_n_struct*)_tmpE5)->f1;_LL7A:(int)_throw(({void*
_tmpE8[0]={};Cyc_Tcutil_impos(({const char*_tmpE9="tcstmt: Abs_n declaration";
_tag_arr(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,26));}),_tag_arr(_tmpE8,
sizeof(void*),0));}));_LL7B:;_LL7C:({void*_tmpEA[0]={};Cyc_Tcutil_terr(_tmp3F->loc,({
const char*_tmpEB="cannot declare a qualified local variable";_tag_arr(_tmpEB,
sizeof(char),_get_zero_arr_size(_tmpEB,42));}),_tag_arr(_tmpEA,sizeof(void*),0));});
goto _LL74;_LL74:;}Cyc_Tcutil_check_type(s0->loc,_tmpDA,Cyc_Tcenv_lookup_type_vars(
_tmpDA),(void*)1,1,_tmpE1);_tmpDA=Cyc_Tcenv_add_local_var(_tmp40->loc,_tmpDA,
_tmpDB);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpDA,s0),_tmp40,0);return;}
_LL3E: if(*((int*)_tmp96)!= 8)goto _LL40;_tmp9D=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp96)->f1;_tmp9E=((struct Cyc_Absyn_Namespace_d_struct*)_tmp96)->f2;_LL3F:
unimp_msg_part=({const char*_tmpEC="namespace";_tag_arr(_tmpEC,sizeof(char),
_get_zero_arr_size(_tmpEC,10));});goto _LL37;_LL40: if(*((int*)_tmp96)!= 9)goto
_LL42;_tmp9F=((struct Cyc_Absyn_Using_d_struct*)_tmp96)->f1;_tmpA0=((struct Cyc_Absyn_Using_d_struct*)
_tmp96)->f2;_LL41: unimp_msg_part=({const char*_tmpED="using";_tag_arr(_tmpED,
sizeof(char),_get_zero_arr_size(_tmpED,6));});goto _LL37;_LL42: if(*((int*)_tmp96)
!= 1)goto _LL44;_LL43: unimp_msg_part=({const char*_tmpEE="function";_tag_arr(
_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,9));});goto _LL37;_LL44: if(*((int*)
_tmp96)!= 4)goto _LL46;_LL45: unimp_msg_part=({const char*_tmpEF="type";_tag_arr(
_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,5));});goto _LL37;_LL46: if(*((int*)
_tmp96)!= 5)goto _LL48;_LL47: unimp_msg_part=({const char*_tmpF0="[x]tunion";
_tag_arr(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,10));});goto _LL37;_LL48:
if(*((int*)_tmp96)!= 6)goto _LL4A;_LL49: unimp_msg_part=({const char*_tmpF1="enum";
_tag_arr(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,5));});goto _LL37;_LL4A:
if(*((int*)_tmp96)!= 7)goto _LL4C;_LL4B: unimp_msg_part=({const char*_tmpF2="typedef";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,8));});goto _LL37;_LL4C:
if(*((int*)_tmp96)!= 10)goto _LL37;_LL4D: unimp_msg_part=({const char*_tmpF3="extern \"C\"";
_tag_arr(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,11));});goto _LL37;_LL37:;}(
int)_throw(({struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct
_tagged_arr)unimp_msg_part;{void*_tmpF4[1]={& _tmpF6};Cyc_Tcutil_impos(({const
char*_tmpF5="tcStmt: nested %s declarations unimplemented";_tag_arr(_tmpF5,
sizeof(char),_get_zero_arr_size(_tmpF5,45));}),_tag_arr(_tmpF4,sizeof(void*),1));}}));}
_LL25: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 18)goto _LL27;_tmp41=((struct Cyc_Absyn_Region_s_struct*)
_tmpC)->f1;_tmp42=((struct Cyc_Absyn_Region_s_struct*)_tmpC)->f2;_tmp43=((struct
Cyc_Absyn_Region_s_struct*)_tmpC)->f3;_tmp44=((struct Cyc_Absyn_Region_s_struct*)
_tmpC)->f4;_LL26:(void*)(_tmp44->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpF8;_tmpF8.tag=1;_tmpF8.f1=Cyc_Absyn_new_stmt((
void*)_tmp44->r,_tmp44->loc);_tmpF8.f2=Cyc_Absyn_skip_stmt(_tmp44->loc);_tmpF8;});
_tmpF7;}),_tmp44->loc))->r));if(new_block)te=Cyc_Tcenv_new_block(s0->loc,te);{
struct Cyc_Absyn_Vardecl _tmpFA;void*_tmpFB;struct Cyc_Core_Opt*_tmpFC;struct Cyc_Core_Opt**
_tmpFD;struct Cyc_Absyn_Vardecl*_tmpF9=_tmp42;_tmpFA=*_tmpF9;_tmpFB=(void*)_tmpFA.type;
_tmpFC=_tmpFA.rgn;_tmpFD=(struct Cyc_Core_Opt**)&(*_tmpF9).rgn;{void*_tmpFE=Cyc_Tcenv_curr_rgn(
te);void*rgn_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmp105=_cycalloc(
sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_VarType_struct _tmp106;_tmp106.tag=
1;_tmp106.f1=_tmp41;_tmp106;});_tmp105;});*((struct Cyc_Core_Opt**)_tmpFD)=({
struct Cyc_Core_Opt*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->v=(void*)_tmpFE;
_tmpFF;});te=Cyc_Tcenv_add_type_vars(s0->loc,te,({struct Cyc_List_List*_tmp100=
_cycalloc(sizeof(*_tmp100));_tmp100->hd=_tmp41;_tmp100->tl=0;_tmp100;}));Cyc_Tcutil_check_type(
s0->loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)2,1,_tmpFB);te=Cyc_Tcenv_add_region(
te,rgn_typ,_tmp43);if(!Cyc_Tcutil_unify(_tmpFB,(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp102;_tmp102.tag=15;_tmp102.f1=(void*)rgn_typ;_tmp102;});_tmp101;})))({void*
_tmp103[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp104="region stmt: type of region handle is wrong!";_tag_arr(
_tmp104,sizeof(char),_get_zero_arr_size(_tmp104,45));}),_tag_arr(_tmp103,sizeof(
void*),0));});Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(Cyc_Tcenv_add_local_var(
_tmp44->loc,Cyc_Tcenv_enter_try(te),_tmp42),s0),_tmp44,0);return;}}_LL27: if(
_tmpC <= (void*)1?1:*((int*)_tmpC)!= 20)goto _LL29;_tmp45=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmpC)->f1;_LL28: {void*rgn_type=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->v=(void*)((void*)3);_tmp10C;}),({struct Cyc_Core_Opt*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp10D;}));void*etype=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp10A=
_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp10B;_tmp10B.tag=15;_tmp10B.f1=(void*)rgn_type;_tmp10B;});_tmp10A;});if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& etype,_tmp45),etype))({struct Cyc_String_pa_struct
_tmp109;_tmp109.tag=0;_tmp109.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp45->topt))->v);{void*_tmp107[1]={& _tmp109};
Cyc_Tcutil_terr(_tmp45->loc,({const char*_tmp108="expecting region_t but found %s";
_tag_arr(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,32));}),_tag_arr(
_tmp107,sizeof(void*),1));}});Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LL29: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 13)goto _LL2B;_tmp46=((
struct Cyc_Absyn_Cut_s_struct*)_tmpC)->f1;_LL2A:(int)_throw(({void*_tmp10E[0]={};
Cyc_Tcutil_impos(({const char*_tmp10F="tcStmt: cut is not implemented";_tag_arr(
_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,31));}),_tag_arr(_tmp10E,sizeof(
void*),0));}));_LL2B: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 14)goto _LL0;_tmp47=((
struct Cyc_Absyn_Splice_s_struct*)_tmpC)->f1;_LL2C:(int)_throw(({void*_tmp110[0]={};
Cyc_Tcutil_impos(({const char*_tmp111="tcStmt: splice is not implemented";
_tag_arr(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,34));}),_tag_arr(
_tmp110,sizeof(void*),0));}));_LL0:;}}
