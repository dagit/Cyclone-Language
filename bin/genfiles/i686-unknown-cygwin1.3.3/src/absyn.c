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
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownTunionInfo{struct _tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0*tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
_tagged_arr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();struct Cyc_Core_Opt*Cyc_Absyn_void_star_typ_opt();void*
Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);void*Cyc_Absyn_union_typ(struct
_tagged_arr*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple0*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct
Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct
_tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct
Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tuple3{struct Cyc_Absyn_Tqual f1;
void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*
a);struct _tagged_arr*Cyc_Absyn_fieldname(int);struct _tuple4{void*f1;struct
_tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(void*);void
Cyc_Absyn_print_decls(struct Cyc_List_List*);struct Cyc_Typerep_Int_struct{int tag;
int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;
void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple5{
unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int
tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple6{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct _tuple7{unsigned int f1;struct _tagged_arr f2;};struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct
_tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;unsigned int f3;struct _tagged_arr f4;};struct _tuple8{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{int tag;
struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;}
;struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_printf(struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];
extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple9*Cyc_Dict_rchoose(struct
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void Cyc_Marshal_print_type(void*
rep,void*val);struct _tuple10{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple11{
struct _tagged_arr f1;int f2;};static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*
ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct
Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(
vs1,vs2);}int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){void*_tmp0=(*((
struct _tuple0*)q1)).f1;void*_tmp1=(*((struct _tuple0*)q2)).f1;{struct _tuple9 _tmp3=({
struct _tuple9 _tmp2;_tmp2.f1=_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;
void*_tmp6;struct Cyc_List_List*_tmp7;void*_tmp8;struct Cyc_List_List*_tmp9;void*
_tmpA;struct Cyc_List_List*_tmpB;void*_tmpC;struct Cyc_List_List*_tmpD;void*_tmpE;
void*_tmpF;void*_tmp10;void*_tmp11;_LL1: _tmp4=_tmp3.f1;if((int)_tmp4 != 0)goto
_LL3;_tmp5=_tmp3.f2;if((int)_tmp5 != 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;
if(_tmp6 <= (void*)1?1:*((int*)_tmp6)!= 0)goto _LL5;_tmp7=((struct Cyc_Absyn_Rel_n_struct*)
_tmp6)->f1;_tmp8=_tmp3.f2;if(_tmp8 <= (void*)1?1:*((int*)_tmp8)!= 0)goto _LL5;
_tmp9=((struct Cyc_Absyn_Rel_n_struct*)_tmp8)->f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;
goto _LL6;_LL5: _tmpA=_tmp3.f1;if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 1)goto _LL7;
_tmpB=((struct Cyc_Absyn_Abs_n_struct*)_tmpA)->f1;_tmpC=_tmp3.f2;if(_tmpC <= (void*)
1?1:*((int*)_tmpC)!= 1)goto _LL7;_tmpD=((struct Cyc_Absyn_Abs_n_struct*)_tmpC)->f1;
_LL6: {int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7:
_tmpE=_tmp3.f1;if((int)_tmpE != 0)goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if((
int)_tmpF != 0)goto _LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(_tmp10 <= (void*)1?1:*((
int*)_tmp10)!= 0)goto _LLD;_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(_tmp11 <= (void*)
1?1:*((int*)_tmp11)!= 0)goto _LL0;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*((
struct _tuple0*)q1)).f2,(*((struct _tuple0*)q2)).f2);}int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity != 0?
tv2->identity != 0: 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*((struct _tuple0*)qv)).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*
_tmp15;_LL10: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LL12;_tmp14=((
struct Cyc_Absyn_Rel_n_struct*)_tmp13)->f1;if(_tmp14 != 0)goto _LL12;_LL11: goto
_LL13;_LL12: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 1)goto _LL14;_tmp15=((struct
Cyc_Absyn_Abs_n_struct*)_tmp13)->f1;if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;
_LL14: if((int)_tmp13 != 0)goto _LL16;_LL15: return 0;_LL16:;_LL17: return 1;_LLF:;}
static int Cyc_Absyn_new_type_counter=0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*env){return(void*)({struct Cyc_Absyn_Evar_struct*_tmp16=
_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=
0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;
_tmp17;});_tmp16;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(void*)((void*)1)};
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp18;_tmp18.q_const=x.q_const?1: y.q_const;_tmp18.q_volatile=x.q_volatile?1: y.q_volatile;
_tmp18.q_restrict=x.q_restrict?1: y.q_restrict;_tmp18;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.q_const=0;
_tmp19.q_volatile=0;_tmp19.q_restrict=0;_tmp19;});}struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){
return({struct Cyc_Absyn_Tqual _tmp1A;_tmp1A.q_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=
0;_tmp1A;});}struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){return({struct
Cyc_Absyn_Conref*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(void*)x;_tmp1D;});
_tmp1C;}));_tmp1B;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){return({
struct Cyc_Absyn_Conref*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(void*)((void*)
0);_tmp1E;});}static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={0,(
void*)1};static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={0,(
void*)0};struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(void*)((void*)& Cyc_Absyn_true_constraint)};
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(void*)((void*)& Cyc_Absyn_false_constraint)};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x){void*_tmp21=(void*)x->v;struct Cyc_Absyn_Conref*_tmp22;
struct Cyc_Absyn_Conref**_tmp23;_LL19: if((int)_tmp21 != 0)goto _LL1B;_LL1A: goto
_LL1C;_LL1B: if(_tmp21 <= (void*)1?1:*((int*)_tmp21)!= 0)goto _LL1D;_LL1C: return x;
_LL1D: if(_tmp21 <= (void*)1?1:*((int*)_tmp21)!= 1)goto _LL18;_tmp22=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp21)->f1;_tmp23=(struct Cyc_Absyn_Conref**)&((struct Cyc_Absyn_Forward_constr_struct*)
_tmp21)->f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp24=Cyc_Absyn_compress_conref(*((
struct Cyc_Absyn_Conref**)_tmp23));*((struct Cyc_Absyn_Conref**)_tmp23)=_tmp24;
return _tmp24;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x){void*
_tmp25=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp26;_LL20: if(_tmp25 <= (
void*)1?1:*((int*)_tmp25)!= 0)goto _LL22;_tmp26=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp25)->f1;_LL21: return _tmp26;_LL22:;_LL23:({void*_tmp27[0]={};Cyc_Tcutil_impos(({
const char*_tmp28="conref_val";_tag_arr(_tmp28,sizeof(char),_get_zero_arr_size(
_tmp28,11));}),_tag_arr(_tmp27,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){void*_tmp29=(void*)(Cyc_Absyn_compress_conref(x))->v;
void*_tmp2A;_LL25: if(_tmp29 <= (void*)1?1:*((int*)_tmp29)!= 0)goto _LL27;_tmp2A=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp29)->f1;_LL26: return _tmp2A;_LL27:;
_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*k){void*_tmp2B=k;struct Cyc_Core_Opt*
_tmp2C;struct Cyc_Core_Opt*_tmp2D;struct Cyc_Core_Opt*_tmp2E;struct Cyc_Core_Opt
_tmp2F;void*_tmp30;void**_tmp31;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt
_tmp33;void*_tmp34;void**_tmp35;_LL2A: if(*((int*)_tmp2B)!= 0)goto _LL2C;_LL2B:
goto _LL2D;_LL2C: if(*((int*)_tmp2B)!= 1)goto _LL2E;_tmp2C=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp2B)->f1;if(_tmp2C != 0)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(*((int*)_tmp2B)!= 2)
goto _LL30;_tmp2D=((struct Cyc_Absyn_Less_kb_struct*)_tmp2B)->f1;if(_tmp2D != 0)
goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp2B)!= 1)goto _LL32;_tmp2E=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp2B)->f1;if(_tmp2E == 0)goto _LL32;_tmp2F=*_tmp2E;
_tmp30=(void*)_tmp2F.v;_tmp31=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp2B)->f1).v;_LL31: _tmp35=_tmp31;goto _LL33;_LL32: if(*((int*)_tmp2B)!= 2)goto
_LL29;_tmp32=((struct Cyc_Absyn_Less_kb_struct*)_tmp2B)->f1;if(_tmp32 == 0)goto
_LL29;_tmp33=*_tmp32;_tmp34=(void*)_tmp33.v;_tmp35=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp2B)->f1).v;_LL33:*((void**)_tmp35)=Cyc_Absyn_compress_kb(*((void**)_tmp35));
return*((void**)_tmp35);_LL29:;}void*Cyc_Absyn_force_kb(void*kb){void*_tmp36=Cyc_Absyn_compress_kb(
kb);void*_tmp37;struct Cyc_Core_Opt*_tmp38;struct Cyc_Core_Opt**_tmp39;struct Cyc_Core_Opt*
_tmp3A;struct Cyc_Core_Opt**_tmp3B;void*_tmp3C;_LL35: if(*((int*)_tmp36)!= 0)goto
_LL37;_tmp37=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp36)->f1;_LL36: return
_tmp37;_LL37: if(*((int*)_tmp36)!= 1)goto _LL39;_tmp38=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp36)->f1;_tmp39=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp36)->f1;_LL38: _tmp3B=_tmp39;_tmp3C=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp36)!= 2)goto _LL34;_tmp3A=((struct Cyc_Absyn_Less_kb_struct*)_tmp36)->f1;
_tmp3B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp36)->f1;
_tmp3C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp36)->f2;_LL3A:*((struct Cyc_Core_Opt**)
_tmp3B)=({struct Cyc_Core_Opt*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->v=(void*)((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(void*)_tmp3C;_tmp3F;});
_tmp3E;}));_tmp3D;});return _tmp3C;_LL34:;}static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_char_tt={5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={5,(void*)((void*)1),(void*)((void*)3)};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_schar_tt={5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sint_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slong_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={5,(void*)((void*)0),(void*)((void*)3)};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp4B=_cycalloc_atomic(sizeof(*_tmp4B));
_tmp4B[0]=({struct Cyc_Absyn_DoubleType_struct _tmp4C;_tmp4C.tag=6;_tmp4C.f1=b;
_tmp4C;});_tmp4B;});}static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={1,0};
static char _tmp4E[4]="exn";static struct _tagged_arr Cyc_Absyn_exn_str={_tmp4E,
_tmp4E,_tmp4E + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static
char _tmp4F[15]="Null_Exception";static struct _tagged_arr Cyc_Absyn_Null_Exception_str={
_tmp4F,_tmp4F,_tmp4F + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp50[13]="Array_bounds";
static struct _tagged_arr Cyc_Absyn_Array_bounds_str={_tmp50,_tmp50,_tmp50 + 13};
static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp51[16]="Match_Exception";static struct
_tagged_arr Cyc_Absyn_Match_Exception_str={_tmp51,_tmp51,_tmp51 + 16};static struct
_tuple0 Cyc_Absyn_Match_Exception_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp52[10]="Bad_alloc";static struct
_tagged_arr Cyc_Absyn_Bad_alloc_str={_tmp52,_tmp52,_tmp52 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str};
struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)((void*)3)};struct
Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static
struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={1,& Cyc_Absyn_exn_tud};
static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={2,{(void*)((void*)&
Cyc_Absyn_exn_tinfou),0,(void*)((void*)2)}};void*Cyc_Absyn_exn_typ=(void*)& Cyc_Absyn_exn_typ_tt;
static char _tmp55[9]="PrintArg";static struct _tagged_arr Cyc_Absyn_printarg_str={
_tmp55,_tmp55,_tmp55 + 9};static char _tmp56[9]="ScanfArg";static struct _tagged_arr
Cyc_Absyn_scanfarg_str={_tmp56,_tmp56,_tmp56 + 9};static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_std_namespace={1,0};static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_tunion_scanf_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static struct Cyc_Core_Opt*Cyc_Absyn_string_t_opt=
0;void*Cyc_Absyn_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(),(void*)0,Cyc_Absyn_true_conref);
else{if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_empty_tqual(),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_string_t_opt=({
struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->v=(void*)t;_tmp58;});}
return(void*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_string_t_opt))->v;}}
static struct Cyc_Core_Opt*Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(
void*)0,Cyc_Absyn_true_conref);else{if(Cyc_Absyn_const_string_t_opt == 0){void*t=
Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(void*)0,
Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({struct Cyc_Core_Opt*_tmp59=
_cycalloc(sizeof(*_tmp59));_tmp59->v=(void*)t;_tmp59;});}return(void*)((struct
Cyc_Core_Opt*)_check_null(Cyc_Absyn_const_string_t_opt))->v;}}static struct Cyc_Absyn_Int_c_struct
Cyc_Absyn_one_intc={2,(void*)((void*)1),1};static struct Cyc_Absyn_Const_e_struct
Cyc_Absyn_one_b_raw={0,(void*)((void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp
Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;void*Cyc_Absyn_starb_typ(void*
t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));
_tmp5D[0]=({struct Cyc_Absyn_PointerType_struct _tmp5E;_tmp5E.tag=4;_tmp5E.f1=({
struct Cyc_Absyn_PtrInfo _tmp5F;_tmp5F.elt_typ=(void*)t;_tmp5F.rgn_typ=(void*)r;
_tmp5F.nullable=Cyc_Absyn_true_conref;_tmp5F.tq=tq;_tmp5F.bounds=Cyc_Absyn_new_conref(
b);_tmp5F.zero_term=zeroterm;_tmp5F;});_tmp5E;});_tmp5D;});}void*Cyc_Absyn_atb_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp60=_cycalloc(sizeof(*
_tmp60));_tmp60[0]=({struct Cyc_Absyn_PointerType_struct _tmp61;_tmp61.tag=4;
_tmp61.f1=({struct Cyc_Absyn_PtrInfo _tmp62;_tmp62.elt_typ=(void*)t;_tmp62.rgn_typ=(
void*)r;_tmp62.nullable=Cyc_Absyn_false_conref;_tmp62.tq=tq;_tmp62.bounds=Cyc_Absyn_new_conref(
b);_tmp62.zero_term=zeroterm;_tmp62;});_tmp61;});_tmp60;});}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return Cyc_Absyn_starb_typ(
t,r,tq,Cyc_Absyn_bounds_one,zeroterm);}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual
tq){return Cyc_Absyn_starb_typ(t,(void*)2,tq,Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm){return Cyc_Absyn_atb_typ(t,r,tq,Cyc_Absyn_bounds_one,zeroterm);}void*
Cyc_Absyn_tagged_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp63=_cycalloc(
sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_PointerType_struct _tmp64;_tmp64.tag=
4;_tmp64.f1=({struct Cyc_Absyn_PtrInfo _tmp65;_tmp65.elt_typ=(void*)t;_tmp65.rgn_typ=(
void*)r;_tmp65.nullable=Cyc_Absyn_true_conref;_tmp65.tq=tq;_tmp65.bounds=Cyc_Absyn_new_conref((
void*)0);_tmp65.zero_term=zeroterm;_tmp65;});_tmp64;});_tmp63;});}void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term){return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp66=_cycalloc(
sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Absyn_ArrayType_struct _tmp67;_tmp67.tag=7;
_tmp67.f1=({struct Cyc_Absyn_ArrayInfo _tmp68;_tmp68.elt_type=(void*)elt_type;
_tmp68.tq=tq;_tmp68.num_elts=num_elts;_tmp68.zero_term=zero_term;_tmp68;});
_tmp67;});_tmp66;});}static struct Cyc_Core_Opt*Cyc_Absyn_file_t_opt=0;static char
_tmp69[8]="__sFILE";static struct _tagged_arr Cyc_Absyn_sf_str={_tmp69,_tmp69,
_tmp69 + 8};static struct _tagged_arr*Cyc_Absyn_sf=& Cyc_Absyn_sf_str;void*Cyc_Absyn_file_typ(){
if(Cyc_Absyn_file_t_opt == 0){struct _tuple0*file_t_name=({struct _tuple0*_tmp72=
_cycalloc(sizeof(*_tmp72));_tmp72->f1=(void*)& Cyc_Absyn_std_namespace;_tmp72->f2=
Cyc_Absyn_sf;_tmp72;});struct Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->kind=(void*)((void*)0);_tmp71->sc=(
void*)((void*)1);_tmp71->name=file_t_name;_tmp71->tvs=0;_tmp71->impl=0;_tmp71->attributes=
0;_tmp71;});void*file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp6B=
_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_AggrType_struct _tmp6C;
_tmp6C.tag=10;_tmp6C.f1=({struct Cyc_Absyn_AggrInfo _tmp6D;_tmp6D.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));
_tmp6E[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6F;_tmp6F.tag=1;_tmp6F.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=sd;_tmp70;});
_tmp6F;});_tmp6E;}));_tmp6D.targs=0;_tmp6D;});_tmp6C;});_tmp6B;});Cyc_Absyn_file_t_opt=({
struct Cyc_Core_Opt*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->v=(void*)Cyc_Absyn_at_typ(
file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmp6A;});}
return(void*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_file_t_opt))->v;}static
struct Cyc_Core_Opt*Cyc_Absyn_void_star_t_opt=0;void*Cyc_Absyn_void_star_typ(){
if(Cyc_Absyn_void_star_t_opt == 0)Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->v=(void*)Cyc_Absyn_star_typ((void*)0,(
void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmp73;});return(void*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_void_star_t_opt))->v;}struct Cyc_Core_Opt*
Cyc_Absyn_void_star_typ_opt(){if(Cyc_Absyn_void_star_t_opt == 0)Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->v=(void*)Cyc_Absyn_star_typ((
void*)0,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmp74;});
return Cyc_Absyn_void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _tagged_arr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_AggrType_struct
_tmp77;_tmp77.tag=10;_tmp77.f1=({struct Cyc_Absyn_AggrInfo _tmp78;_tmp78.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp79=_cycalloc(sizeof(*
_tmp79));_tmp79[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp7A;_tmp7A.tag=0;
_tmp7A.f1=(void*)k;_tmp7A.f2=({struct _tuple0*_tmp7B=_cycalloc(sizeof(*_tmp7B));
_tmp7B->f1=Cyc_Absyn_rel_ns_null;_tmp7B->f2=name;_tmp7B;});_tmp7A;});_tmp79;}));
_tmp78.targs=0;_tmp78;});_tmp77;});_tmp76;});}void*Cyc_Absyn_strct(struct
_tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp7C=
_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_AggrType_struct _tmp7D;
_tmp7D.tag=10;_tmp7D.f1=({struct Cyc_Absyn_AggrInfo _tmp7E;_tmp7E.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));
_tmp7F[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(
void*)((void*)0);_tmp80.f2=name;_tmp80;});_tmp7F;}));_tmp7E.targs=0;_tmp7E;});
_tmp7D;});_tmp7C;});}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({
struct Cyc_Absyn_AggrType_struct _tmp82;_tmp82.tag=10;_tmp82.f1=({struct Cyc_Absyn_AggrInfo
_tmp83;_tmp83.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp85;_tmp85.tag=0;_tmp85.f1=(void*)((void*)1);_tmp85.f2=name;_tmp85;});_tmp84;}));
_tmp83.targs=0;_tmp83;});_tmp82;});_tmp81;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Exp*_tmp86=
_cycalloc(sizeof(*_tmp86));_tmp86->topt=0;_tmp86->r=(void*)r;_tmp86->loc=loc;
_tmp86->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp86;});}struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_New_e_struct*_tmp87=
_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_New_e_struct _tmp88;_tmp88.tag=
15;_tmp88.f1=rgn_handle;_tmp88.f2=e;_tmp88;});_tmp87;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){return({struct Cyc_Absyn_Exp*_tmp89=
_cycalloc(sizeof(*_tmp89));_tmp89[0]=*((struct Cyc_Absyn_Exp*)e);_tmp89;});}
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*c,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*_tmp8A=_cycalloc(
sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_Const_e_struct _tmp8B;_tmp8B.tag=0;
_tmp8B.f1=(void*)c;_tmp8B;});_tmp8A;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_Const_e_struct
_tmp8D;_tmp8D.tag=0;_tmp8D.f1=(void*)((void*)0);_tmp8D;});_tmp8C;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp8E=_cycalloc(
sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Absyn_Int_c_struct _tmp8F;_tmp8F.tag=2;
_tmp8F.f1=(void*)s;_tmp8F.f2=i;_tmp8F;});_tmp8E;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_Char_c_struct
_tmp91;_tmp91.tag=0;_tmp91.f1=(void*)((void*)2);_tmp91.f2=c;_tmp91;});_tmp90;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Float_c_struct*_tmp92=
_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_Float_c_struct _tmp93;
_tmp93.tag=4;_tmp93.f1=f;_tmp93;});_tmp92;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_String_c_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[
0]=({struct Cyc_Absyn_String_c_struct _tmp95;_tmp95.tag=5;_tmp95.f1=s;_tmp95;});
_tmp94;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_Var_e_struct _tmp97;_tmp97.tag=
1;_tmp97.f1=q;_tmp97.f2=(void*)((void*)0);_tmp97;});_tmp96;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmp98=_cycalloc(sizeof(*
_tmp98));_tmp98[0]=({struct Cyc_Absyn_Var_e_struct _tmp99;_tmp99.tag=1;_tmp99.f1=q;
_tmp99.f2=(void*)b;_tmp99;});_tmp98;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp9B;_tmp9B.tag=2;_tmp9B.f1=q;_tmp9B;});
_tmp9A;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp9D;_tmp9D.tag=3;_tmp9D.f1=(void*)p;_tmp9D.f2=es;_tmp9D;});_tmp9C;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*
_tmp9E));_tmp9E->hd=e;_tmp9E->tl=0;_tmp9E;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->hd=e1;_tmp9F->tl=({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*
_tmpA0));_tmpA0->hd=e2;_tmpA0->tl=0;_tmpA0;});_tmp9F;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpA1=
_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpA2;
_tmpA2.tag=4;_tmpA2.f1=e1;_tmpA2.f2=popt;_tmpA2.f3=e2;_tmpA2;});_tmpA1;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpA4;_tmpA4.tag=5;_tmpA4.f1=e;_tmpA4.f2=(void*)i;_tmpA4;});_tmpA3;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpA6;_tmpA6.tag=6;_tmpA6.f1=e1;_tmpA6.f2=e2;_tmpA6.f3=e3;_tmpA6;});_tmpA5;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,e2,Cyc_Absyn_false_exp(
loc),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,Cyc_Absyn_true_exp(
loc),e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[
0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpA8;_tmpA8.tag=7;_tmpA8.f1=e1;_tmpA8.f2=
e2;_tmpA8;});_tmpA7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*_tmpA9=
_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpAA;_tmpAA.tag=8;_tmpAA.f1=e;_tmpAA.f2=es;_tmpAA;});_tmpA9;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_FnCall_e_struct _tmpAC;
_tmpAC.tag=9;_tmpAC.f1=e;_tmpAC.f2=es;_tmpAC.f3=0;_tmpAC;});_tmpAB;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpAE;_tmpAE.tag=11;_tmpAE.f1=e;_tmpAE;});_tmpAD;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpB0;_tmpB0.tag=12;_tmpB0.f1=e;_tmpB0.f2=ts;_tmpB0;});_tmpAF;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Cast_e_struct _tmpB2;
_tmpB2.tag=13;_tmpB2.f1=(void*)t;_tmpB2.f2=e;_tmpB2;});_tmpB1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_struct*_tmpB3=_cycalloc(
sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_Throw_e_struct _tmpB4;_tmpB4.tag=10;
_tmpB4.f1=e;_tmpB4;});_tmpB3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Address_e_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5[0]=({struct Cyc_Absyn_Address_e_struct _tmpB6;_tmpB6.tag=14;_tmpB6.f1=e;
_tmpB6;});_tmpB5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpB8;_tmpB8.tag=16;_tmpB8.f1=(void*)t;_tmpB8;});_tmpB7;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmpBA;
_tmpBA.tag=17;_tmpBA.f1=e;_tmpBA;});_tmpB9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmpBC;_tmpBC.tag=18;_tmpBC.f1=(void*)t;_tmpBC.f2=(
void*)of;_tmpBC;});_tmpBB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpBE;_tmpBE.tag=19;_tmpBE.f1=tvs;_tmpBE.f2=(
void*)t;_tmpBE;});_tmpBD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpC0;_tmpC0.tag=20;_tmpC0.f1=e;_tmpC0;});_tmpBF;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpC2;_tmpC2.tag=21;_tmpC2.f1=e;_tmpC2.f2=n;
_tmpC2;});_tmpC1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));
_tmpC3[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpC4;_tmpC4.tag=22;_tmpC4.f1=e;
_tmpC4.f2=n;_tmpC4;});_tmpC3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpC6;
_tmpC6.tag=23;_tmpC6.f1=e1;_tmpC6.f2=e2;_tmpC6;});_tmpC5;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpC7=_cycalloc(
sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpC8;_tmpC8.tag=24;
_tmpC8.f1=es;_tmpC8;});_tmpC7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));
_tmpC9[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpCA;_tmpCA.tag=35;_tmpCA.f1=s;
_tmpCA;});_tmpC9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*
_tmpCB));_tmpCB->hd=({struct _tuple12*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->f1=
0;_tmpCC->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpCC;});_tmpCB->tl=dles;_tmpCB;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpCD=_cycalloc(
sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Array_e_struct _tmpCE;_tmpCE.tag=26;
_tmpCE.f1=dles;_tmpCE;});_tmpCD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD0;_tmpD0.tag=34;_tmpD0.f1=n;_tmpD0.f2=dles;_tmpD0;});_tmpCF;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->r=(void*)s;_tmpD1->loc=
loc;_tmpD1->non_local_preds=0;_tmpD1->try_depth=0;_tmpD1->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpD1;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpD2=_cycalloc(
sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_Exp_s_struct _tmpD3;_tmpD3.tag=0;
_tmpD3.f1=e;_tmpD3;});_tmpD2;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpD4=
_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_Return_s_struct _tmpD5;
_tmpD5.tag=2;_tmpD5.f1=e;_tmpD5;});_tmpD4;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpD7;_tmpD7.tag=3;_tmpD7.f1=e;_tmpD7.f2=s1;_tmpD7.f3=s2;_tmpD7;});_tmpD6;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_While_s_struct
_tmpD9;_tmpD9.tag=4;_tmpD9.f1=({struct _tuple2 _tmpDA;_tmpDA.f1=e;_tmpDA.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpDA;});_tmpD9.f2=s;_tmpD9;});_tmpD8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_Break_s_struct
_tmpDC;_tmpDC.tag=5;_tmpDC.f1=0;_tmpDC;});_tmpDB;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));
_tmpDD[0]=({struct Cyc_Absyn_Continue_s_struct _tmpDE;_tmpDE.tag=6;_tmpDE.f1=0;
_tmpDE;});_tmpDD;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_For_s_struct _tmpE0;
_tmpE0.tag=8;_tmpE0.f1=e1;_tmpE0.f2=({struct _tuple2 _tmpE1;_tmpE1.f1=e2;_tmpE1.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpE1;});_tmpE0.f3=({struct _tuple2 _tmpE2;_tmpE2.f1=
e3;_tmpE2.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpE2;});_tmpE0.f4=s;_tmpE0;});_tmpDF;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpE4;_tmpE4.tag=9;_tmpE4.f1=e;_tmpE4.f2=scs;_tmpE4;});_tmpE3;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpE6=({struct _tuple9 _tmpE5;
_tmpE5.f1=(void*)s1->r;_tmpE5.f2=(void*)s2->r;_tmpE5;});void*_tmpE7;void*_tmpE8;
_LL3C: _tmpE7=_tmpE6.f1;if((int)_tmpE7 != 0)goto _LL3E;_LL3D: return s2;_LL3E: _tmpE8=
_tmpE6.f2;if((int)_tmpE8 != 0)goto _LL40;_LL3F: return s1;_LL40:;_LL41: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpEA;_tmpEA.tag=1;_tmpEA.f1=s1;_tmpEA.f2=s2;_tmpEA;});
_tmpE9;}),loc);_LL3B:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmpEC;_tmpEC.tag=11;_tmpEC.f1=el;_tmpEC.f2=0;_tmpEC;});_tmpEB;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEE;_tmpEE.tag=12;_tmpEE.f1=d;_tmpEE.f2=s;_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({
struct Cyc_Absyn_Var_d_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmpF2;});_tmpF1;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpF0;_tmpF0.tag=12;_tmpF0.f1=d;_tmpF0.f2=s;_tmpF0;});_tmpEF;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmpF3=_cycalloc(
sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_Cut_s_struct _tmpF4;_tmpF4.tag=13;
_tmpF4.f1=s;_tmpF4;});_tmpF3;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[
0]=({struct Cyc_Absyn_Splice_s_struct _tmpF6;_tmpF6.tag=14;_tmpF6.f1=s;_tmpF6;});
_tmpF5;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[
0]=({struct Cyc_Absyn_Label_s_struct _tmpF8;_tmpF8.tag=15;_tmpF8.f1=v;_tmpF8.f2=s;
_tmpF8;});_tmpF7;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Do_s_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({
struct Cyc_Absyn_Do_s_struct _tmpFA;_tmpFA.tag=16;_tmpFA.f1=s;_tmpFA.f2=({struct
_tuple2 _tmpFB;_tmpFB.f1=e;_tmpFB.f2=Cyc_Absyn_skip_stmt(e->loc);_tmpFB;});_tmpFA;});
_tmpF9;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_TryCatch_s_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));
_tmpFC[0]=({struct Cyc_Absyn_TryCatch_s_struct _tmpFD;_tmpFD.tag=17;_tmpFD.f1=s;
_tmpFD.f2=scs;_tmpFD;});_tmpFC;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpFF;_tmpFF.tag=7;_tmpFF.f1=lab;_tmpFF.f2=0;
_tmpFF;});_tmpFE;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*d,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ForArray_s_struct*
_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_ForArray_s_struct
_tmp101;_tmp101.tag=19;_tmp101.f1=({struct Cyc_Absyn_ForArrayInfo _tmp102;_tmp102.defns=
d;_tmp102.condition=({struct _tuple2 _tmp104;_tmp104.f1=e1;_tmp104.f2=Cyc_Absyn_skip_stmt(
e1->loc);_tmp104;});_tmp102.delta=({struct _tuple2 _tmp103;_tmp103.f1=e2;_tmp103.f2=
Cyc_Absyn_skip_stmt(e2->loc);_tmp103;});_tmp102.body=s;_tmp102;});_tmp101;});
_tmp100;}),loc);}struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*
s){return({struct Cyc_Absyn_Pat*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->r=(
void*)p;_tmp105->topt=0;_tmp105->loc=s;_tmp105;});}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106->r=(void*)r;_tmp106->loc=loc;_tmp106;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Let_d_struct
_tmp108;_tmp108.tag=2;_tmp108.f1=p;_tmp108.f2=0;_tmp108.f3=e;_tmp108;});_tmp107;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp109=
_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Letv_d_struct _tmp10A;
_tmp10A.tag=3;_tmp10A.f1=vds;_tmp10A;});_tmp109;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->sc=(void*)((
void*)2);_tmp10B->name=x;_tmp10B->tq=Cyc_Absyn_empty_tqual();_tmp10B->type=(void*)
t;_tmp10B->initializer=init;_tmp10B->rgn=0;_tmp10B->attributes=0;_tmp10B->escapes=
0;_tmp10B;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->sc=(void*)((void*)0);_tmp10C->name=x;
_tmp10C->tq=Cyc_Absyn_empty_tqual();_tmp10C->type=(void*)t;_tmp10C->initializer=
init;_tmp10C->rgn=0;_tmp10C->attributes=0;_tmp10C->escapes=0;_tmp10C;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->exist_vars=exists;_tmp10D->rgn_po=
po;_tmp10D->fields=fs;_tmp10D;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp10F;_tmp10F.tag=4;_tmp10F.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->kind=(void*)
k;_tmp110->sc=(void*)s;_tmp110->name=n;_tmp110->tvs=ts;_tmp110->impl=i;_tmp110->attributes=
atts;_tmp110;});_tmp10F;});_tmp10E;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp111=
_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp112;
_tmp112.tag=5;_tmp112.f1=({struct Cyc_Absyn_Tuniondecl*_tmp113=_cycalloc(sizeof(*
_tmp113));_tmp113->sc=(void*)s;_tmp113->name=n;_tmp113->tvs=ts;_tmp113->fields=
fs;_tmp113->is_xtunion=is_xtunion;_tmp113;});_tmp112;});_tmp111;}),loc);}static
struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){return({struct _tuple1*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->f1=(*((struct _tuple1*)a)).f1;
_tmp114->f2=(*((struct _tuple1*)a)).f2;_tmp114->f3=Cyc_Absyn_pointer_expand((*((
struct _tuple1*)a)).f3);_tmp114;});}void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*atts){return(void*)({struct Cyc_Absyn_FnType_struct*_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_FnType_struct _tmp116;
_tmp116.tag=8;_tmp116.f1=({struct Cyc_Absyn_FnInfo _tmp117;_tmp117.tvars=tvs;
_tmp117.ret_typ=(void*)Cyc_Absyn_pointer_expand(ret_typ);_tmp117.effect=eff_typ;
_tmp117.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args);_tmp117.c_varargs=
c_varargs;_tmp117.cyc_varargs=cyc_varargs;_tmp117.rgn_po=rgn_po;_tmp117.attributes=
atts;_tmp117;});_tmp116;});_tmp115;});}void*Cyc_Absyn_pointer_expand(void*t){
void*_tmp118=Cyc_Tcutil_compress(t);_LL43: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(t,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_false_conref);_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp119=(void*)e->r;void*_tmp11A;void*_tmp11B;struct
Cyc_Absyn_Vardecl*_tmp11C;void*_tmp11D;struct Cyc_Absyn_Vardecl*_tmp11E;struct Cyc_Absyn_Exp*
_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;_LL48: if(*((int*)
_tmp119)!= 1)goto _LL4A;_tmp11A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp119)->f2;
if(_tmp11A <= (void*)1?1:*((int*)_tmp11A)!= 1)goto _LL4A;_LL49: return 0;_LL4A: if(*((
int*)_tmp119)!= 1)goto _LL4C;_tmp11B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp119)->f2;if(_tmp11B <= (void*)1?1:*((int*)_tmp11B)!= 0)goto _LL4C;_tmp11C=((
struct Cyc_Absyn_Global_b_struct*)_tmp11B)->f1;_LL4B: _tmp11E=_tmp11C;goto _LL4D;
_LL4C: if(*((int*)_tmp119)!= 1)goto _LL4E;_tmp11D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp119)->f2;if(_tmp11D <= (void*)1?1:*((int*)_tmp11D)!= 3)goto _LL4E;_tmp11E=((
struct Cyc_Absyn_Local_b_struct*)_tmp11D)->f1;_LL4D: {void*_tmp122=Cyc_Tcutil_compress((
void*)_tmp11E->type);_LL5F: if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 7)goto
_LL61;_LL60: return 0;_LL61:;_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp119)!= 1)
goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp119)!= 22)goto _LL52;_LL51: goto
_LL53;_LL52: if(*((int*)_tmp119)!= 20)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)
_tmp119)!= 23)goto _LL56;_LL55: return 1;_LL56: if(*((int*)_tmp119)!= 21)goto _LL58;
_tmp11F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp119)->f1;_LL57: return Cyc_Absyn_is_lvalue(
_tmp11F);_LL58: if(*((int*)_tmp119)!= 12)goto _LL5A;_tmp120=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp119)->f1;_LL59: return Cyc_Absyn_is_lvalue(_tmp120);_LL5A: if(*((int*)_tmp119)
!= 11)goto _LL5C;_tmp121=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp119)->f1;
_LL5B: return Cyc_Absyn_is_lvalue(_tmp121);_LL5C:;_LL5D: return 0;_LL47:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _tagged_arr*v){{struct
Cyc_List_List*_tmp123=fields;for(0;_tmp123 != 0;_tmp123=_tmp123->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp123->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp123->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _tagged_arr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple3*)((struct _tuple3*)
ts->hd);}struct _tagged_arr Cyc_Absyn_attribute2string(void*a){void*_tmp124=a;int
_tmp125;int _tmp126;struct _tagged_arr _tmp127;void*_tmp128;int _tmp129;int _tmp12A;
void*_tmp12B;int _tmp12C;int _tmp12D;int _tmp12E;_LL64: if(_tmp124 <= (void*)17?1:*((
int*)_tmp124)!= 0)goto _LL66;_tmp125=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp124)->f1;_LL65: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp131;
_tmp131.tag=1;_tmp131.f1=(int)((unsigned int)_tmp125);{void*_tmp12F[1]={& _tmp131};
Cyc_aprintf(({const char*_tmp130="regparm(%d)";_tag_arr(_tmp130,sizeof(char),
_get_zero_arr_size(_tmp130,12));}),_tag_arr(_tmp12F,sizeof(void*),1));}});_LL66:
if((int)_tmp124 != 0)goto _LL68;_LL67: return({const char*_tmp132="stdcall";_tag_arr(
_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,8));});_LL68: if((int)_tmp124 != 1)
goto _LL6A;_LL69: return({const char*_tmp133="cdecl";_tag_arr(_tmp133,sizeof(char),
_get_zero_arr_size(_tmp133,6));});_LL6A: if((int)_tmp124 != 2)goto _LL6C;_LL6B:
return({const char*_tmp134="fastcall";_tag_arr(_tmp134,sizeof(char),
_get_zero_arr_size(_tmp134,9));});_LL6C: if((int)_tmp124 != 3)goto _LL6E;_LL6D:
return({const char*_tmp135="noreturn";_tag_arr(_tmp135,sizeof(char),
_get_zero_arr_size(_tmp135,9));});_LL6E: if((int)_tmp124 != 4)goto _LL70;_LL6F:
return({const char*_tmp136="const";_tag_arr(_tmp136,sizeof(char),
_get_zero_arr_size(_tmp136,6));});_LL70: if(_tmp124 <= (void*)17?1:*((int*)_tmp124)
!= 1)goto _LL72;_tmp126=((struct Cyc_Absyn_Aligned_att_struct*)_tmp124)->f1;_LL71:
if(_tmp126 == - 1)return({const char*_tmp137="aligned";_tag_arr(_tmp137,sizeof(char),
_get_zero_arr_size(_tmp137,8));});else{return(struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp13A;_tmp13A.tag=1;_tmp13A.f1=(int)((unsigned int)_tmp126);{void*_tmp138[1]={&
_tmp13A};Cyc_aprintf(({const char*_tmp139="aligned(%d)";_tag_arr(_tmp139,sizeof(
char),_get_zero_arr_size(_tmp139,12));}),_tag_arr(_tmp138,sizeof(void*),1));}});}
_LL72: if((int)_tmp124 != 5)goto _LL74;_LL73: return({const char*_tmp13B="packed";
_tag_arr(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,7));});_LL74: if(_tmp124
<= (void*)17?1:*((int*)_tmp124)!= 2)goto _LL76;_tmp127=((struct Cyc_Absyn_Section_att_struct*)
_tmp124)->f1;_LL75: return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp13E;
_tmp13E.tag=0;_tmp13E.f1=(struct _tagged_arr)_tmp127;{void*_tmp13C[1]={& _tmp13E};
Cyc_aprintf(({const char*_tmp13D="section(\"%s\")";_tag_arr(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,14));}),_tag_arr(_tmp13C,sizeof(void*),1));}});_LL76:
if((int)_tmp124 != 6)goto _LL78;_LL77: return({const char*_tmp13F="nocommon";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,9));});_LL78: if((int)
_tmp124 != 7)goto _LL7A;_LL79: return({const char*_tmp140="shared";_tag_arr(_tmp140,
sizeof(char),_get_zero_arr_size(_tmp140,7));});_LL7A: if((int)_tmp124 != 8)goto
_LL7C;_LL7B: return({const char*_tmp141="unused";_tag_arr(_tmp141,sizeof(char),
_get_zero_arr_size(_tmp141,7));});_LL7C: if((int)_tmp124 != 9)goto _LL7E;_LL7D:
return({const char*_tmp142="weak";_tag_arr(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,5));});_LL7E: if((int)_tmp124 != 10)goto _LL80;_LL7F:
return({const char*_tmp143="dllimport";_tag_arr(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,10));});_LL80: if((int)_tmp124 != 11)goto _LL82;_LL81:
return({const char*_tmp144="dllexport";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,10));});_LL82: if((int)_tmp124 != 12)goto _LL84;_LL83:
return({const char*_tmp145="no_instrument_function";_tag_arr(_tmp145,sizeof(char),
_get_zero_arr_size(_tmp145,23));});_LL84: if((int)_tmp124 != 13)goto _LL86;_LL85:
return({const char*_tmp146="constructor";_tag_arr(_tmp146,sizeof(char),
_get_zero_arr_size(_tmp146,12));});_LL86: if((int)_tmp124 != 14)goto _LL88;_LL87:
return({const char*_tmp147="destructor";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,11));});_LL88: if((int)_tmp124 != 15)goto _LL8A;_LL89:
return({const char*_tmp148="no_check_memory_usage";_tag_arr(_tmp148,sizeof(char),
_get_zero_arr_size(_tmp148,22));});_LL8A: if(_tmp124 <= (void*)17?1:*((int*)
_tmp124)!= 3)goto _LL8C;_tmp128=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp124)->f1;if((int)_tmp128 != 0)goto _LL8C;_tmp129=((struct Cyc_Absyn_Format_att_struct*)
_tmp124)->f2;_tmp12A=((struct Cyc_Absyn_Format_att_struct*)_tmp124)->f3;_LL8B:
return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp14C;_tmp14C.tag=1;_tmp14C.f1=(
unsigned int)_tmp12A;{struct Cyc_Int_pa_struct _tmp14B;_tmp14B.tag=1;_tmp14B.f1=(
unsigned int)_tmp129;{void*_tmp149[2]={& _tmp14B,& _tmp14C};Cyc_aprintf(({const
char*_tmp14A="format(printf,%u,%u)";_tag_arr(_tmp14A,sizeof(char),
_get_zero_arr_size(_tmp14A,21));}),_tag_arr(_tmp149,sizeof(void*),2));}}});_LL8C:
if(_tmp124 <= (void*)17?1:*((int*)_tmp124)!= 3)goto _LL8E;_tmp12B=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp124)->f1;if((int)_tmp12B != 1)goto _LL8E;_tmp12C=((
struct Cyc_Absyn_Format_att_struct*)_tmp124)->f2;_tmp12D=((struct Cyc_Absyn_Format_att_struct*)
_tmp124)->f3;_LL8D: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp150;
_tmp150.tag=1;_tmp150.f1=(unsigned int)_tmp12D;{struct Cyc_Int_pa_struct _tmp14F;
_tmp14F.tag=1;_tmp14F.f1=(unsigned int)_tmp12C;{void*_tmp14D[2]={& _tmp14F,&
_tmp150};Cyc_aprintf(({const char*_tmp14E="format(scanf,%u,%u)";_tag_arr(_tmp14E,
sizeof(char),_get_zero_arr_size(_tmp14E,20));}),_tag_arr(_tmp14D,sizeof(void*),2));}}});
_LL8E: if(_tmp124 <= (void*)17?1:*((int*)_tmp124)!= 4)goto _LL90;_tmp12E=((struct
Cyc_Absyn_Initializes_att_struct*)_tmp124)->f1;_LL8F: return(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp153;_tmp153.tag=1;_tmp153.f1=(int)((unsigned int)
_tmp12E);{void*_tmp151[1]={& _tmp153};Cyc_aprintf(({const char*_tmp152="initializes(%d)";
_tag_arr(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,16));}),_tag_arr(
_tmp151,sizeof(void*),1));}});_LL90: if((int)_tmp124 != 16)goto _LL63;_LL91: return({
const char*_tmp154="pure";_tag_arr(_tmp154,sizeof(char),_get_zero_arr_size(
_tmp154,5));});_LL63:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp155=a;_LL93: if(
_tmp155 <= (void*)17?1:*((int*)_tmp155)!= 0)goto _LL95;_LL94: goto _LL96;_LL95: if((
int)_tmp155 != 2)goto _LL97;_LL96: goto _LL98;_LL97: if((int)_tmp155 != 0)goto _LL99;
_LL98: goto _LL9A;_LL99: if((int)_tmp155 != 1)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if((
int)_tmp155 != 3)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if((int)_tmp155 != 16)goto _LL9F;
_LL9E: goto _LLA0;_LL9F: if(_tmp155 <= (void*)17?1:*((int*)_tmp155)!= 3)goto _LLA1;
_LLA0: goto _LLA2;_LLA1: if((int)_tmp155 != 4)goto _LLA3;_LLA2: return 1;_LLA3: if(
_tmp155 <= (void*)17?1:*((int*)_tmp155)!= 4)goto _LLA5;_LLA4: return 1;_LLA5:;_LLA6:
return 0;_LL92:;}static char _tmp156[3]="f0";static struct _tagged_arr Cyc_Absyn_f0={
_tmp156,_tmp156,_tmp156 + 3};static struct _tagged_arr*Cyc_Absyn_field_names_v[1]={&
Cyc_Absyn_f0};static struct _tagged_arr Cyc_Absyn_field_names={(void*)((struct
_tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(
int i){unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp157=(
unsigned int)(i + 1);struct _tagged_arr**_tmp158=(struct _tagged_arr**)_cycalloc(
_check_times(sizeof(struct _tagged_arr*),_tmp157));struct _tagged_arr _tmp15E=
_tag_arr(_tmp158,sizeof(struct _tagged_arr*),_tmp157);{unsigned int _tmp159=
_tmp157;unsigned int j;for(j=0;j < _tmp159;j ++){_tmp158[j]=j < fsz?*((struct
_tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),(int)j)):({struct _tagged_arr*_tmp15A=_cycalloc(sizeof(*_tmp15A));
_tmp15A[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp15D;_tmp15D.tag=1;
_tmp15D.f1=(int)j;{void*_tmp15B[1]={& _tmp15D};Cyc_aprintf(({const char*_tmp15C="f%d";
_tag_arr(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,4));}),_tag_arr(_tmp15B,
sizeof(void*),1));}});_tmp15A;});}}_tmp15E;});return*((struct _tagged_arr**)
_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*),i));}
struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*info){void*_tmp15F=info;void*
_tmp160;struct _tuple0*_tmp161;struct Cyc_Absyn_Aggrdecl**_tmp162;struct Cyc_Absyn_Aggrdecl*
_tmp163;struct Cyc_Absyn_Aggrdecl _tmp164;void*_tmp165;struct _tuple0*_tmp166;_LLA8:
if(*((int*)_tmp15F)!= 0)goto _LLAA;_tmp160=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp15F)->f1;_tmp161=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp15F)->f2;_LLA9:
return({struct _tuple4 _tmp167;_tmp167.f1=_tmp160;_tmp167.f2=_tmp161;_tmp167;});
_LLAA: if(*((int*)_tmp15F)!= 1)goto _LLA7;_tmp162=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp15F)->f1;_tmp163=*_tmp162;_tmp164=*_tmp163;_tmp165=(void*)_tmp164.kind;
_tmp166=_tmp164.name;_LLAB: return({struct _tuple4 _tmp168;_tmp168.f1=_tmp165;
_tmp168.f2=_tmp166;_tmp168;});_LLA7:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info){void*_tmp169=info;void*_tmp16A;struct _tuple0*_tmp16B;struct Cyc_Absyn_Aggrdecl**
_tmp16C;struct Cyc_Absyn_Aggrdecl*_tmp16D;_LLAD: if(*((int*)_tmp169)!= 0)goto _LLAF;
_tmp16A=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp169)->f1;_tmp16B=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp169)->f2;_LLAE:({void*_tmp16E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp16F="unchecked aggrdecl";_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(
_tmp16F,19));}),_tag_arr(_tmp16E,sizeof(void*),0));});_LLAF: if(*((int*)_tmp169)
!= 1)goto _LLAC;_tmp16C=((struct Cyc_Absyn_KnownAggr_struct*)_tmp169)->f1;_tmp16D=*
_tmp16C;_LLB0: return _tmp16D;_LLAC:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp170=Cyc_Tcutil_compress(t);void*_tmp171;struct Cyc_Absyn_AggrInfo _tmp172;void*
_tmp173;_LLB2: if(_tmp170 <= (void*)3?1:*((int*)_tmp170)!= 11)goto _LLB4;_tmp171=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp170)->f1;if((int)_tmp171 != 1)
goto _LLB4;_LLB3: return 1;_LLB4: if(_tmp170 <= (void*)3?1:*((int*)_tmp170)!= 10)goto
_LLB6;_tmp172=((struct Cyc_Absyn_AggrType_struct*)_tmp170)->f1;_tmp173=(void*)
_tmp172.aggr_info;_LLB5: return(Cyc_Absyn_aggr_kinded_name(_tmp173)).f1 == (void*)
1;_LLB6:;_LLB7: return 0;_LLB1:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp174[0]={};Cyc_printf(({const char*_tmp175="\n";_tag_arr(_tmp175,
sizeof(char),_get_zero_arr_size(_tmp175,2));}),_tag_arr(_tmp174,sizeof(void*),0));});}
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_402;static struct Cyc_Typerep_Int_struct Cyc__genrep_5={0,0,32};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_132;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Vardecl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;
static char _tmp177[7]="Static";static struct _tuple7 Cyc__gentuple_136={0,{_tmp177,
_tmp177,_tmp177 + 7}};static char _tmp178[9]="Abstract";static struct _tuple7 Cyc__gentuple_137={
1,{_tmp178,_tmp178,_tmp178 + 9}};static char _tmp179[7]="Public";static struct
_tuple7 Cyc__gentuple_138={2,{_tmp179,_tmp179,_tmp179 + 7}};static char _tmp17A[7]="Extern";
static struct _tuple7 Cyc__gentuple_139={3,{_tmp17A,_tmp17A,_tmp17A + 7}};static char
_tmp17B[8]="ExternC";static struct _tuple7 Cyc__gentuple_140={4,{_tmp17B,_tmp17B,
_tmp17B + 8}};static char _tmp17C[9]="Register";static struct _tuple7 Cyc__gentuple_141={
5,{_tmp17C,_tmp17C,_tmp17C + 9}};static struct _tuple7*Cyc__genarr_142[6]={& Cyc__gentuple_136,&
Cyc__gentuple_137,& Cyc__gentuple_138,& Cyc__gentuple_139,& Cyc__gentuple_140,& Cyc__gentuple_141};
static struct _tuple5*Cyc__genarr_143[0]={};static char _tmp17E[6]="Scope";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_scope_t_rep={5,{_tmp17E,_tmp17E,_tmp17E + 6},{(void*)((struct _tuple7**)
Cyc__genarr_142),(void*)((struct _tuple7**)Cyc__genarr_142),(void*)((struct
_tuple7**)Cyc__genarr_142 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_143),(void*)((
struct _tuple5**)Cyc__genarr_143),(void*)((struct _tuple5**)Cyc__genarr_143 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_11;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_17;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_18;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_13;static struct Cyc_Typerep_Int_struct Cyc__genrep_14={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_13={2,(void*)((void*)& Cyc__genrep_14)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12={1,1,(void*)((void*)& Cyc__genrep_13)};
static char _tmp182[5]="List";static struct _tagged_arr Cyc__genname_22={_tmp182,
_tmp182,_tmp182 + 5};static char _tmp183[3]="hd";static struct _tuple5 Cyc__gentuple_19={
offsetof(struct Cyc_List_List,hd),{_tmp183,_tmp183,_tmp183 + 3},(void*)& Cyc__genrep_12};
static char _tmp184[3]="tl";static struct _tuple5 Cyc__gentuple_20={offsetof(struct
Cyc_List_List,tl),{_tmp184,_tmp184,_tmp184 + 3},(void*)& Cyc__genrep_18};static
struct _tuple5*Cyc__genarr_21[2]={& Cyc__gentuple_19,& Cyc__gentuple_20};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_22,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_21),(void*)((
struct _tuple5**)Cyc__genarr_21),(void*)((struct _tuple5**)Cyc__genarr_21 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_18={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple13{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_23={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_24={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_18};static struct _tuple6*Cyc__genarr_25[
2]={& Cyc__gentuple_23,& Cyc__gentuple_24};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_17={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_25),(void*)((
struct _tuple6**)Cyc__genarr_25),(void*)((struct _tuple6**)Cyc__genarr_25 + 2)}};
static char _tmp188[6]="Loc_n";static struct _tuple7 Cyc__gentuple_15={0,{_tmp188,
_tmp188,_tmp188 + 6}};static struct _tuple7*Cyc__genarr_16[1]={& Cyc__gentuple_15};
static char _tmp189[6]="Rel_n";static struct _tuple5 Cyc__gentuple_26={0,{_tmp189,
_tmp189,_tmp189 + 6},(void*)& Cyc__genrep_17};static char _tmp18A[6]="Abs_n";static
struct _tuple5 Cyc__gentuple_27={1,{_tmp18A,_tmp18A,_tmp18A + 6},(void*)& Cyc__genrep_17};
static struct _tuple5*Cyc__genarr_28[2]={& Cyc__gentuple_26,& Cyc__gentuple_27};
static char _tmp18C[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={
5,{_tmp18C,_tmp18C,_tmp18C + 8},{(void*)((struct _tuple7**)Cyc__genarr_16),(void*)((
struct _tuple7**)Cyc__genarr_16),(void*)((struct _tuple7**)Cyc__genarr_16 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_28),(void*)((struct _tuple5**)Cyc__genarr_28),(
void*)((struct _tuple5**)Cyc__genarr_28 + 2)}};static struct _tuple6 Cyc__gentuple_29={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_30={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_12};static
struct _tuple6*Cyc__genarr_31[2]={& Cyc__gentuple_29,& Cyc__gentuple_30};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_11={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_31),(void*)((struct _tuple6**)Cyc__genarr_31),(void*)((
struct _tuple6**)Cyc__genarr_31 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10={
1,1,(void*)((void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_133;
struct _tuple14{char f1;};static struct _tuple6 Cyc__gentuple_134={offsetof(struct
_tuple14,f1),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_135[
1]={& Cyc__gentuple_134};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_133={4,
sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_135),(void*)((
struct _tuple6**)Cyc__genarr_135),(void*)((struct _tuple6**)Cyc__genarr_135 + 1)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1068;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1073;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp190[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_187={0,{_tmp190,_tmp190,_tmp190 + 8}};static char
_tmp191[8]="MemKind";static struct _tuple7 Cyc__gentuple_188={1,{_tmp191,_tmp191,
_tmp191 + 8}};static char _tmp192[8]="BoxKind";static struct _tuple7 Cyc__gentuple_189={
2,{_tmp192,_tmp192,_tmp192 + 8}};static char _tmp193[8]="RgnKind";static struct
_tuple7 Cyc__gentuple_190={3,{_tmp193,_tmp193,_tmp193 + 8}};static char _tmp194[8]="EffKind";
static struct _tuple7 Cyc__gentuple_191={4,{_tmp194,_tmp194,_tmp194 + 8}};static char
_tmp195[8]="IntKind";static struct _tuple7 Cyc__gentuple_192={5,{_tmp195,_tmp195,
_tmp195 + 8}};static struct _tuple7*Cyc__genarr_193[6]={& Cyc__gentuple_187,& Cyc__gentuple_188,&
Cyc__gentuple_189,& Cyc__gentuple_190,& Cyc__gentuple_191,& Cyc__gentuple_192};
static struct _tuple5*Cyc__genarr_194[0]={};static char _tmp197[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,{_tmp197,_tmp197,_tmp197 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_193),(void*)((struct _tuple7**)Cyc__genarr_193),(void*)((struct
_tuple7**)Cyc__genarr_193 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_194),(void*)((
struct _tuple5**)Cyc__genarr_194),(void*)((struct _tuple5**)Cyc__genarr_194 + 0)}};
static char _tmp198[4]="Opt";static struct _tagged_arr Cyc__genname_1076={_tmp198,
_tmp198,_tmp198 + 4};static char _tmp199[2]="v";static struct _tuple5 Cyc__gentuple_1074={
offsetof(struct Cyc_Core_Opt,v),{_tmp199,_tmp199,_tmp199 + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_1075[1]={& Cyc__gentuple_1074};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1076,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1075),(void*)((
struct _tuple5**)Cyc__genarr_1075),(void*)((struct _tuple5**)Cyc__genarr_1075 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1073={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_43;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp19C[4]="Opt";static struct
_tagged_arr Cyc__genname_1108={_tmp19C,_tmp19C,_tmp19C + 4};static char _tmp19D[2]="v";
static struct _tuple5 Cyc__gentuple_1106={offsetof(struct Cyc_Core_Opt,v),{_tmp19D,
_tmp19D,_tmp19D + 2},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1107[
1]={& Cyc__gentuple_1106};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1108,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1107),(void*)((struct _tuple5**)Cyc__genarr_1107),(
void*)((struct _tuple5**)Cyc__genarr_1107 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_43={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
static struct Cyc_Typerep_Int_struct Cyc__genrep_62={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1069;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_296;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_184;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_215;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_215={1,1,(void*)((void*)& Cyc__genrep_62)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_207;
static struct _tuple6 Cyc__gentuple_208={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_209={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_210[2]={& Cyc__gentuple_208,& Cyc__gentuple_209};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_207={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_210),(void*)((struct _tuple6**)Cyc__genarr_210),(
void*)((struct _tuple6**)Cyc__genarr_210 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_203;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_195;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static
char _tmp1A3[4]="Opt";static struct _tagged_arr Cyc__genname_198={_tmp1A3,_tmp1A3,
_tmp1A3 + 4};static char _tmp1A4[2]="v";static struct _tuple5 Cyc__gentuple_196={
offsetof(struct Cyc_Core_Opt,v),{_tmp1A4,_tmp1A4,_tmp1A4 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_197[1]={& Cyc__gentuple_196};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_198,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_197),(void*)((
struct _tuple5**)Cyc__genarr_197),(void*)((struct _tuple5**)Cyc__genarr_197 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_195={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_204={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_205={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_195};static struct _tuple6*Cyc__genarr_206[
2]={& Cyc__gentuple_204,& Cyc__gentuple_205};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_203={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_206),(
void*)((struct _tuple6**)Cyc__genarr_206),(void*)((struct _tuple6**)Cyc__genarr_206
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_186;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_199={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_200={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_195};static struct _tuple6 Cyc__gentuple_201={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_202[3]={& Cyc__gentuple_199,& Cyc__gentuple_200,& Cyc__gentuple_201};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_186={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_202),(void*)((struct _tuple6**)Cyc__genarr_202),(
void*)((struct _tuple6**)Cyc__genarr_202 + 3)}};static struct _tuple7*Cyc__genarr_185[
0]={};static char _tmp1A9[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_211={0,{
_tmp1A9,_tmp1A9,_tmp1A9 + 6},(void*)& Cyc__genrep_207};static char _tmp1AA[11]="Unknown_kb";
static struct _tuple5 Cyc__gentuple_212={1,{_tmp1AA,_tmp1AA,_tmp1AA + 11},(void*)&
Cyc__genrep_203};static char _tmp1AB[8]="Less_kb";static struct _tuple5 Cyc__gentuple_213={
2,{_tmp1AB,_tmp1AB,_tmp1AB + 8},(void*)& Cyc__genrep_186};static struct _tuple5*Cyc__genarr_214[
3]={& Cyc__gentuple_211,& Cyc__gentuple_212,& Cyc__gentuple_213};static char _tmp1AD[
10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,{
_tmp1AD,_tmp1AD,_tmp1AD + 10},{(void*)((struct _tuple7**)Cyc__genarr_185),(void*)((
struct _tuple7**)Cyc__genarr_185),(void*)((struct _tuple7**)Cyc__genarr_185 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_214),(void*)((struct _tuple5**)Cyc__genarr_214),(
void*)((struct _tuple5**)Cyc__genarr_214 + 3)}};static char _tmp1AE[5]="Tvar";static
struct _tagged_arr Cyc__genname_220={_tmp1AE,_tmp1AE,_tmp1AE + 5};static char _tmp1AF[
5]="name";static struct _tuple5 Cyc__gentuple_216={offsetof(struct Cyc_Absyn_Tvar,name),{
_tmp1AF,_tmp1AF,_tmp1AF + 5},(void*)& Cyc__genrep_12};static char _tmp1B0[9]="identity";
static struct _tuple5 Cyc__gentuple_217={offsetof(struct Cyc_Absyn_Tvar,identity),{
_tmp1B0,_tmp1B0,_tmp1B0 + 9},(void*)& Cyc__genrep_215};static char _tmp1B1[5]="kind";
static struct _tuple5 Cyc__gentuple_218={offsetof(struct Cyc_Absyn_Tvar,kind),{
_tmp1B1,_tmp1B1,_tmp1B1 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_219[3]={& Cyc__gentuple_216,& Cyc__gentuple_217,& Cyc__gentuple_218};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_220,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple5**)Cyc__genarr_219),(
void*)((struct _tuple5**)Cyc__genarr_219),(void*)((struct _tuple5**)Cyc__genarr_219
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_184={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static char _tmp1B4[5]="List";static struct _tagged_arr
Cyc__genname_300={_tmp1B4,_tmp1B4,_tmp1B4 + 5};static char _tmp1B5[3]="hd";static
struct _tuple5 Cyc__gentuple_297={offsetof(struct Cyc_List_List,hd),{_tmp1B5,
_tmp1B5,_tmp1B5 + 3},(void*)& Cyc__genrep_184};static char _tmp1B6[3]="tl";static
struct _tuple5 Cyc__gentuple_298={offsetof(struct Cyc_List_List,tl),{_tmp1B6,
_tmp1B6,_tmp1B6 + 3},(void*)& Cyc__genrep_296};static struct _tuple5*Cyc__genarr_299[
2]={& Cyc__gentuple_297,& Cyc__gentuple_298};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_300,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_299),(void*)((struct _tuple5**)Cyc__genarr_299),(void*)((
struct _tuple5**)Cyc__genarr_299 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_296={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1B9[4]="Opt";static struct _tagged_arr Cyc__genname_1072={_tmp1B9,_tmp1B9,
_tmp1B9 + 4};static char _tmp1BA[2]="v";static struct _tuple5 Cyc__gentuple_1070={
offsetof(struct Cyc_Core_Opt,v),{_tmp1BA,_tmp1BA,_tmp1BA + 2},(void*)& Cyc__genrep_296};
static struct _tuple5*Cyc__genarr_1071[1]={& Cyc__gentuple_1070};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_1072,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1071),(
void*)((struct _tuple5**)Cyc__genarr_1071),(void*)((struct _tuple5**)Cyc__genarr_1071
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1069={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1077={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1078={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1073};static struct _tuple6 Cyc__gentuple_1079={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_43};static struct _tuple6 Cyc__gentuple_1080={
offsetof(struct _tuple17,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_1081={offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1069};static
struct _tuple6*Cyc__genarr_1082[5]={& Cyc__gentuple_1077,& Cyc__gentuple_1078,& Cyc__gentuple_1079,&
Cyc__gentuple_1080,& Cyc__gentuple_1081};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1068={
4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1082),(void*)((
struct _tuple6**)Cyc__genarr_1082),(void*)((struct _tuple6**)Cyc__genarr_1082 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1064;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1065={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1066={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_184};static struct _tuple6*Cyc__genarr_1067[
2]={& Cyc__gentuple_1065,& Cyc__gentuple_1066};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1064={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1067),(
void*)((struct _tuple6**)Cyc__genarr_1067),(void*)((struct _tuple6**)Cyc__genarr_1067
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1038;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1BF[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1049={_tmp1BF,_tmp1BF,_tmp1BF + 18};static
char _tmp1C0[5]="name";static struct _tuple5 Cyc__gentuple_1046={offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),{
_tmp1C0,_tmp1C0,_tmp1C0 + 5},(void*)& Cyc__genrep_10};static char _tmp1C1[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1047={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),{
_tmp1C1,_tmp1C1,_tmp1C1 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1048[2]={& Cyc__gentuple_1046,& Cyc__gentuple_1047};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _tagged_arr*)& Cyc__genname_1049,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1048),(
void*)((struct _tuple5**)Cyc__genarr_1048),(void*)((struct _tuple5**)Cyc__genarr_1048
+ 2)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1050={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1051={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1052[2]={& Cyc__gentuple_1050,& Cyc__gentuple_1051};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1052),(void*)((struct _tuple6**)Cyc__genarr_1052),(
void*)((struct _tuple6**)Cyc__genarr_1052 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1040;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1041;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_287;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_288;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_269;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_270;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_271;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_272;static struct
_tuple6 Cyc__gentuple_273={offsetof(struct _tuple3,f1),(void*)& Cyc__genrep_133};
static struct _tuple6 Cyc__gentuple_274={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_275[2]={& Cyc__gentuple_273,&
Cyc__gentuple_274};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_272={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_275),(void*)((struct
_tuple6**)Cyc__genarr_275),(void*)((struct _tuple6**)Cyc__genarr_275 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_271={1,1,(void*)((void*)& Cyc__genrep_272)};
static char _tmp1C6[5]="List";static struct _tagged_arr Cyc__genname_279={_tmp1C6,
_tmp1C6,_tmp1C6 + 5};static char _tmp1C7[3]="hd";static struct _tuple5 Cyc__gentuple_276={
offsetof(struct Cyc_List_List,hd),{_tmp1C7,_tmp1C7,_tmp1C7 + 3},(void*)& Cyc__genrep_271};
static char _tmp1C8[3]="tl";static struct _tuple5 Cyc__gentuple_277={offsetof(struct
Cyc_List_List,tl),{_tmp1C8,_tmp1C8,_tmp1C8 + 3},(void*)& Cyc__genrep_270};static
struct _tuple5*Cyc__genarr_278[2]={& Cyc__gentuple_276,& Cyc__gentuple_277};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_279,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_278),(void*)((struct _tuple5**)Cyc__genarr_278),(void*)((
struct _tuple5**)Cyc__genarr_278 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_270={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Position_Segment_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={
1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};static char _tmp1CC[12]="Tunionfield";
static struct _tagged_arr Cyc__genname_285={_tmp1CC,_tmp1CC,_tmp1CC + 12};static char
_tmp1CD[5]="name";static struct _tuple5 Cyc__gentuple_280={offsetof(struct Cyc_Absyn_Tunionfield,name),{
_tmp1CD,_tmp1CD,_tmp1CD + 5},(void*)& Cyc__genrep_10};static char _tmp1CE[5]="typs";
static struct _tuple5 Cyc__gentuple_281={offsetof(struct Cyc_Absyn_Tunionfield,typs),{
_tmp1CE,_tmp1CE,_tmp1CE + 5},(void*)& Cyc__genrep_270};static char _tmp1CF[4]="loc";
static struct _tuple5 Cyc__gentuple_282={offsetof(struct Cyc_Absyn_Tunionfield,loc),{
_tmp1CF,_tmp1CF,_tmp1CF + 4},(void*)& Cyc__genrep_2};static char _tmp1D0[3]="sc";
static struct _tuple5 Cyc__gentuple_283={offsetof(struct Cyc_Absyn_Tunionfield,sc),{
_tmp1D0,_tmp1D0,_tmp1D0 + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*
Cyc__genarr_284[4]={& Cyc__gentuple_280,& Cyc__gentuple_281,& Cyc__gentuple_282,&
Cyc__gentuple_283};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_285,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_284),(void*)((struct _tuple5**)Cyc__genarr_284),(
void*)((struct _tuple5**)Cyc__genarr_284 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_269={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
char _tmp1D3[5]="List";static struct _tagged_arr Cyc__genname_292={_tmp1D3,_tmp1D3,
_tmp1D3 + 5};static char _tmp1D4[3]="hd";static struct _tuple5 Cyc__gentuple_289={
offsetof(struct Cyc_List_List,hd),{_tmp1D4,_tmp1D4,_tmp1D4 + 3},(void*)& Cyc__genrep_269};
static char _tmp1D5[3]="tl";static struct _tuple5 Cyc__gentuple_290={offsetof(struct
Cyc_List_List,tl),{_tmp1D5,_tmp1D5,_tmp1D5 + 3},(void*)& Cyc__genrep_288};static
struct _tuple5*Cyc__genarr_291[2]={& Cyc__gentuple_289,& Cyc__gentuple_290};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_292,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_291),(void*)((struct _tuple5**)Cyc__genarr_291),(void*)((
struct _tuple5**)Cyc__genarr_291 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_288={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp1D8[4]="Opt";static struct _tagged_arr Cyc__genname_295={_tmp1D8,_tmp1D8,
_tmp1D8 + 4};static char _tmp1D9[2]="v";static struct _tuple5 Cyc__gentuple_293={
offsetof(struct Cyc_Core_Opt,v),{_tmp1D9,_tmp1D9,_tmp1D9 + 2},(void*)& Cyc__genrep_288};
static struct _tuple5*Cyc__genarr_294[1]={& Cyc__gentuple_293};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_295,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_294),(void*)((struct _tuple5**)Cyc__genarr_294),(void*)((
struct _tuple5**)Cyc__genarr_294 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_287={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1DC[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_307={
_tmp1DC,_tmp1DC,_tmp1DC + 11};static char _tmp1DD[3]="sc";static struct _tuple5 Cyc__gentuple_301={
offsetof(struct Cyc_Absyn_Tuniondecl,sc),{_tmp1DD,_tmp1DD,_tmp1DD + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp1DE[5]="name";static struct _tuple5 Cyc__gentuple_302={offsetof(
struct Cyc_Absyn_Tuniondecl,name),{_tmp1DE,_tmp1DE,_tmp1DE + 5},(void*)& Cyc__genrep_10};
static char _tmp1DF[4]="tvs";static struct _tuple5 Cyc__gentuple_303={offsetof(struct
Cyc_Absyn_Tuniondecl,tvs),{_tmp1DF,_tmp1DF,_tmp1DF + 4},(void*)& Cyc__genrep_296};
static char _tmp1E0[7]="fields";static struct _tuple5 Cyc__gentuple_304={offsetof(
struct Cyc_Absyn_Tuniondecl,fields),{_tmp1E0,_tmp1E0,_tmp1E0 + 7},(void*)& Cyc__genrep_287};
static char _tmp1E1[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_305={
offsetof(struct Cyc_Absyn_Tuniondecl,is_xtunion),{_tmp1E1,_tmp1E1,_tmp1E1 + 11},(
void*)((void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_306[5]={& Cyc__gentuple_301,&
Cyc__gentuple_302,& Cyc__gentuple_303,& Cyc__gentuple_304,& Cyc__gentuple_305};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_307,sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((
struct _tuple5**)Cyc__genarr_306),(void*)((struct _tuple5**)Cyc__genarr_306),(void*)((
struct _tuple5**)Cyc__genarr_306 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1041={1,1,(void*)((void*)& Cyc__genrep_286)};struct _tuple20{
unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct _tuple6 Cyc__gentuple_1042={
offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1043={
offsetof(struct _tuple20,f2),(void*)& Cyc__genrep_1041};static struct _tuple6*Cyc__genarr_1044[
2]={& Cyc__gentuple_1042,& Cyc__gentuple_1043};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1040={4,sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1044),(
void*)((struct _tuple6**)Cyc__genarr_1044),(void*)((struct _tuple6**)Cyc__genarr_1044
+ 2)}};static struct _tuple7*Cyc__genarr_1039[0]={};static char _tmp1E6[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1053={0,{_tmp1E6,_tmp1E6,_tmp1E6 + 14},(void*)&
Cyc__genrep_1045};static char _tmp1E7[12]="KnownTunion";static struct _tuple5 Cyc__gentuple_1054={
1,{_tmp1E7,_tmp1E7,_tmp1E7 + 12},(void*)& Cyc__genrep_1040};static struct _tuple5*
Cyc__genarr_1055[2]={& Cyc__gentuple_1053,& Cyc__gentuple_1054};static char _tmp1E9[
12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={
5,{_tmp1E9,_tmp1E9,_tmp1E9 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1039),(
void*)((struct _tuple7**)Cyc__genarr_1039),(void*)((struct _tuple7**)Cyc__genarr_1039
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1055),(void*)((struct _tuple5**)Cyc__genarr_1055),(
void*)((struct _tuple5**)Cyc__genarr_1055 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_53;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp1EA[5]="List";static struct _tagged_arr Cyc__genname_57={_tmp1EA,
_tmp1EA,_tmp1EA + 5};static char _tmp1EB[3]="hd";static struct _tuple5 Cyc__gentuple_54={
offsetof(struct Cyc_List_List,hd),{_tmp1EB,_tmp1EB,_tmp1EB + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp1EC[3]="tl";static struct _tuple5 Cyc__gentuple_55={
offsetof(struct Cyc_List_List,tl),{_tmp1EC,_tmp1EC,_tmp1EC + 3},(void*)& Cyc__genrep_53};
static struct _tuple5*Cyc__genarr_56[2]={& Cyc__gentuple_54,& Cyc__gentuple_55};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_57,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_56),(void*)((struct _tuple5**)Cyc__genarr_56),(void*)((
struct _tuple5**)Cyc__genarr_56 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_53={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp1EF[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1060={_tmp1EF,
_tmp1EF,_tmp1EF + 11};static char _tmp1F0[12]="tunion_info";static struct _tuple5 Cyc__gentuple_1056={
offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),{_tmp1F0,_tmp1F0,_tmp1F0 + 12},(
void*)& Cyc_tunion_Absyn_TunionInfoU_rep};static char _tmp1F1[6]="targs";static
struct _tuple5 Cyc__gentuple_1057={offsetof(struct Cyc_Absyn_TunionInfo,targs),{
_tmp1F1,_tmp1F1,_tmp1F1 + 6},(void*)& Cyc__genrep_53};static char _tmp1F2[4]="rgn";
static struct _tuple5 Cyc__gentuple_1058={offsetof(struct Cyc_Absyn_TunionInfo,rgn),{
_tmp1F2,_tmp1F2,_tmp1F2 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_1059[3]={& Cyc__gentuple_1056,& Cyc__gentuple_1057,& Cyc__gentuple_1058};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1060,sizeof(struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)
Cyc__genarr_1059),(void*)((struct _tuple5**)Cyc__genarr_1059),(void*)((struct
_tuple5**)Cyc__genarr_1059 + 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo
f2;};static struct _tuple6 Cyc__gentuple_1061={offsetof(struct _tuple21,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1062={offsetof(struct _tuple21,f2),(
void*)& Cyc_Absyn_tunion_info_t_rep};static struct _tuple6*Cyc__genarr_1063[2]={&
Cyc__gentuple_1061,& Cyc__gentuple_1062};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1038={
4,sizeof(struct _tuple21),{(void*)((struct _tuple6**)Cyc__genarr_1063),(void*)((
struct _tuple6**)Cyc__genarr_1063),(void*)((struct _tuple6**)Cyc__genarr_1063 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1012;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp1F5[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1024={_tmp1F5,_tmp1F5,_tmp1F5 + 23};static
char _tmp1F6[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1020={offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),{_tmp1F6,_tmp1F6,_tmp1F6 + 12},(
void*)& Cyc__genrep_10};static char _tmp1F7[11]="field_name";static struct _tuple5 Cyc__gentuple_1021={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),{_tmp1F7,_tmp1F7,
_tmp1F7 + 11},(void*)& Cyc__genrep_10};static char _tmp1F8[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1022={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),{
_tmp1F8,_tmp1F8,_tmp1F8 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1023[3]={& Cyc__gentuple_1020,& Cyc__gentuple_1021,& Cyc__gentuple_1022};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(
struct _tagged_arr*)& Cyc__genname_1024,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1023),(void*)((struct _tuple5**)Cyc__genarr_1023),(
void*)((struct _tuple5**)Cyc__genarr_1023 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1025={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1026={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1027[2]={& Cyc__gentuple_1025,& Cyc__gentuple_1026};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1027),(void*)((struct _tuple6**)Cyc__genarr_1027),(
void*)((struct _tuple6**)Cyc__genarr_1027 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1014;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1015={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1016={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_286)};static struct
_tuple6 Cyc__gentuple_1017={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_269};
static struct _tuple6*Cyc__genarr_1018[3]={& Cyc__gentuple_1015,& Cyc__gentuple_1016,&
Cyc__gentuple_1017};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1014={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1018),(void*)((
struct _tuple6**)Cyc__genarr_1018),(void*)((struct _tuple6**)Cyc__genarr_1018 + 3)}};
static struct _tuple7*Cyc__genarr_1013[0]={};static char _tmp1FC[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1028={0,{_tmp1FC,_tmp1FC,_tmp1FC + 19},(void*)&
Cyc__genrep_1019};static char _tmp1FD[17]="KnownTunionfield";static struct _tuple5
Cyc__gentuple_1029={1,{_tmp1FD,_tmp1FD,_tmp1FD + 17},(void*)& Cyc__genrep_1014};
static struct _tuple5*Cyc__genarr_1030[2]={& Cyc__gentuple_1028,& Cyc__gentuple_1029};
static char _tmp1FF[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,{_tmp1FF,_tmp1FF,_tmp1FF + 17},{(void*)((struct _tuple7**)Cyc__genarr_1013),(
void*)((struct _tuple7**)Cyc__genarr_1013),(void*)((struct _tuple7**)Cyc__genarr_1013
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1030),(void*)((struct _tuple5**)Cyc__genarr_1030),(
void*)((struct _tuple5**)Cyc__genarr_1030 + 2)}};static char _tmp200[16]="TunionFieldInfo";
static struct _tagged_arr Cyc__genname_1034={_tmp200,_tmp200,_tmp200 + 16};static
char _tmp201[11]="field_info";static struct _tuple5 Cyc__gentuple_1031={offsetof(
struct Cyc_Absyn_TunionFieldInfo,field_info),{_tmp201,_tmp201,_tmp201 + 11},(void*)&
Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char _tmp202[6]="targs";static struct
_tuple5 Cyc__gentuple_1032={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),{
_tmp202,_tmp202,_tmp202 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_1033[
2]={& Cyc__gentuple_1031,& Cyc__gentuple_1032};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1034,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1033),(void*)((struct _tuple5**)Cyc__genarr_1033),(
void*)((struct _tuple5**)Cyc__genarr_1033 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1035={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1036={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1037[2]={& Cyc__gentuple_1035,& Cyc__gentuple_1036};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1012={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1037),(void*)((struct _tuple6**)Cyc__genarr_1037),(
void*)((struct _tuple6**)Cyc__genarr_1037 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_996;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_963;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_61;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_63={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_64={offsetof(struct _tuple25,f2),(
void*)((void*)& Cyc__genrep_62)};static struct _tuple6*Cyc__genarr_65[2]={& Cyc__gentuple_63,&
Cyc__gentuple_64};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_61={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_65),(void*)((struct _tuple6**)
Cyc__genarr_65),(void*)((struct _tuple6**)Cyc__genarr_65 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_966;struct _tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static
struct _tuple6 Cyc__gentuple_967={offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_968={offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_963};
static struct _tuple6*Cyc__genarr_969[2]={& Cyc__gentuple_967,& Cyc__gentuple_968};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_966={4,sizeof(struct _tuple26),{(
void*)((struct _tuple6**)Cyc__genarr_969),(void*)((struct _tuple6**)Cyc__genarr_969),(
void*)((struct _tuple6**)Cyc__genarr_969 + 2)}};static char _tmp207[10]="No_constr";
static struct _tuple7 Cyc__gentuple_964={0,{_tmp207,_tmp207,_tmp207 + 10}};static
struct _tuple7*Cyc__genarr_965[1]={& Cyc__gentuple_964};static char _tmp208[10]="Eq_constr";
static struct _tuple5 Cyc__gentuple_970={0,{_tmp208,_tmp208,_tmp208 + 10},(void*)&
Cyc__genrep_61};static char _tmp209[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_971={
1,{_tmp209,_tmp209,_tmp209 + 15},(void*)& Cyc__genrep_966};static struct _tuple5*Cyc__genarr_972[
2]={& Cyc__gentuple_970,& Cyc__gentuple_971};static char _tmp20B[11]="Constraint";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={5,{_tmp20B,
_tmp20B,_tmp20B + 11},{(void*)((struct _tuple7**)Cyc__genarr_965),(void*)((struct
_tuple7**)Cyc__genarr_965),(void*)((struct _tuple7**)Cyc__genarr_965 + 1)},{(void*)((
struct _tuple5**)Cyc__genarr_972),(void*)((struct _tuple5**)Cyc__genarr_972),(void*)((
struct _tuple5**)Cyc__genarr_972 + 2)}};static char _tmp20C[7]="Conref";static struct
_tagged_arr Cyc__genname_975={_tmp20C,_tmp20C,_tmp20C + 7};static char _tmp20D[2]="v";
static struct _tuple5 Cyc__gentuple_973={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp20D,_tmp20D,_tmp20D + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_974[1]={& Cyc__gentuple_973};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _tagged_arr*)& Cyc__genname_975,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_974),(void*)((
struct _tuple5**)Cyc__genarr_974),(void*)((struct _tuple5**)Cyc__genarr_974 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_963={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_997;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;static char _tmp210[7]="Conref";static
struct _tagged_arr Cyc__genname_1000={_tmp210,_tmp210,_tmp210 + 7};static char
_tmp211[2]="v";static struct _tuple5 Cyc__gentuple_998={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp211,_tmp211,_tmp211 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_999[1]={& Cyc__gentuple_998};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1000,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_999),(void*)((
struct _tuple5**)Cyc__genarr_999),(void*)((struct _tuple5**)Cyc__genarr_999 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_997={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
static char _tmp214[8]="PtrInfo";static struct _tagged_arr Cyc__genname_1008={_tmp214,
_tmp214,_tmp214 + 8};static char _tmp215[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1001={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),{_tmp215,_tmp215,_tmp215 + 8},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp216[8]="rgn_typ";static struct _tuple5
Cyc__gentuple_1002={offsetof(struct Cyc_Absyn_PtrInfo,rgn_typ),{_tmp216,_tmp216,
_tmp216 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp217[9]="nullable";
static struct _tuple5 Cyc__gentuple_1003={offsetof(struct Cyc_Absyn_PtrInfo,nullable),{
_tmp217,_tmp217,_tmp217 + 9},(void*)& Cyc__genrep_963};static char _tmp218[3]="tq";
static struct _tuple5 Cyc__gentuple_1004={offsetof(struct Cyc_Absyn_PtrInfo,tq),{
_tmp218,_tmp218,_tmp218 + 3},(void*)& Cyc__genrep_133};static char _tmp219[7]="bounds";
static struct _tuple5 Cyc__gentuple_1005={offsetof(struct Cyc_Absyn_PtrInfo,bounds),{
_tmp219,_tmp219,_tmp219 + 7},(void*)& Cyc__genrep_997};static char _tmp21A[10]="zero_term";
static struct _tuple5 Cyc__gentuple_1006={offsetof(struct Cyc_Absyn_PtrInfo,zero_term),{
_tmp21A,_tmp21A,_tmp21A + 10},(void*)& Cyc__genrep_963};static struct _tuple5*Cyc__genarr_1007[
6]={& Cyc__gentuple_1001,& Cyc__gentuple_1002,& Cyc__gentuple_1003,& Cyc__gentuple_1004,&
Cyc__gentuple_1005,& Cyc__gentuple_1006};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1008,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1007),(void*)((struct _tuple5**)Cyc__genarr_1007),(
void*)((struct _tuple5**)Cyc__genarr_1007 + 6)}};struct _tuple27{unsigned int f1;
struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1009={offsetof(
struct _tuple27,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1010={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1011[2]={& Cyc__gentuple_1009,& Cyc__gentuple_1010};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_996={4,sizeof(struct _tuple27),{(void*)((
struct _tuple6**)Cyc__genarr_1011),(void*)((struct _tuple6**)Cyc__genarr_1011),(
void*)((struct _tuple6**)Cyc__genarr_1011 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_985;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static
char _tmp21D[7]="Signed";static struct _tuple7 Cyc__gentuple_419={0,{_tmp21D,_tmp21D,
_tmp21D + 7}};static char _tmp21E[9]="Unsigned";static struct _tuple7 Cyc__gentuple_420={
1,{_tmp21E,_tmp21E,_tmp21E + 9}};static char _tmp21F[5]="None";static struct _tuple7
Cyc__gentuple_421={2,{_tmp21F,_tmp21F,_tmp21F + 5}};static struct _tuple7*Cyc__genarr_422[
3]={& Cyc__gentuple_419,& Cyc__gentuple_420,& Cyc__gentuple_421};static struct
_tuple5*Cyc__genarr_423[0]={};static char _tmp221[5]="Sign";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep={5,{_tmp221,_tmp221,_tmp221 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_422),(void*)((struct _tuple7**)Cyc__genarr_422),(void*)((struct
_tuple7**)Cyc__genarr_422 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_423),(void*)((
struct _tuple5**)Cyc__genarr_423),(void*)((struct _tuple5**)Cyc__genarr_423 + 0)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;static char _tmp222[3]="B1";
static struct _tuple7 Cyc__gentuple_986={0,{_tmp222,_tmp222,_tmp222 + 3}};static char
_tmp223[3]="B2";static struct _tuple7 Cyc__gentuple_987={1,{_tmp223,_tmp223,_tmp223
+ 3}};static char _tmp224[3]="B4";static struct _tuple7 Cyc__gentuple_988={2,{_tmp224,
_tmp224,_tmp224 + 3}};static char _tmp225[3]="B8";static struct _tuple7 Cyc__gentuple_989={
3,{_tmp225,_tmp225,_tmp225 + 3}};static struct _tuple7*Cyc__genarr_990[4]={& Cyc__gentuple_986,&
Cyc__gentuple_987,& Cyc__gentuple_988,& Cyc__gentuple_989};static struct _tuple5*Cyc__genarr_991[
0]={};static char _tmp227[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,{_tmp227,_tmp227,_tmp227 + 8},{(void*)((struct _tuple7**)Cyc__genarr_990),(void*)((
struct _tuple7**)Cyc__genarr_990),(void*)((struct _tuple7**)Cyc__genarr_990 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_991),(void*)((struct _tuple5**)Cyc__genarr_991),(
void*)((struct _tuple5**)Cyc__genarr_991 + 0)}};struct _tuple28{unsigned int f1;void*
f2;void*f3;};static struct _tuple6 Cyc__gentuple_992={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_993={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_994={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_995[3]={& Cyc__gentuple_992,& Cyc__gentuple_993,& Cyc__gentuple_994};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_985={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_995),(void*)((struct _tuple6**)Cyc__genarr_995),(
void*)((struct _tuple6**)Cyc__genarr_995 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_962;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_838;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_853;struct
_tuple29{unsigned int f1;void*f2;char f3;};static struct _tuple6 Cyc__gentuple_854={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_855={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_856={
offsetof(struct _tuple29,f3),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_857[3]={& Cyc__gentuple_854,& Cyc__gentuple_855,& Cyc__gentuple_856};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_853={4,sizeof(struct _tuple29),{(
void*)((struct _tuple6**)Cyc__genarr_857),(void*)((struct _tuple6**)Cyc__genarr_857),(
void*)((struct _tuple6**)Cyc__genarr_857 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_847;static struct Cyc_Typerep_Int_struct Cyc__genrep_848={0,1,16};
struct _tuple30{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_849={
offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_850={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_851={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_848};static struct _tuple6*Cyc__genarr_852[
3]={& Cyc__gentuple_849,& Cyc__gentuple_850,& Cyc__gentuple_851};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_847={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_852),(
void*)((struct _tuple6**)Cyc__genarr_852),(void*)((struct _tuple6**)Cyc__genarr_852
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_418;struct _tuple31{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_424={offsetof(
struct _tuple31,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_425={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_426={
offsetof(struct _tuple31,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_427[3]={& Cyc__gentuple_424,& Cyc__gentuple_425,& Cyc__gentuple_426};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_418={4,sizeof(struct _tuple31),{(
void*)((struct _tuple6**)Cyc__genarr_427),(void*)((struct _tuple6**)Cyc__genarr_427),(
void*)((struct _tuple6**)Cyc__genarr_427 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_841;static struct Cyc_Typerep_Int_struct Cyc__genrep_842={0,1,64};
struct _tuple32{unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_843={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_844={
offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_845={
offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_842};static struct _tuple6*Cyc__genarr_846[
3]={& Cyc__gentuple_843,& Cyc__gentuple_844,& Cyc__gentuple_845};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_841={4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_846),(
void*)((struct _tuple6**)Cyc__genarr_846),(void*)((struct _tuple6**)Cyc__genarr_846
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;static struct _tuple6
Cyc__gentuple_117={offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_118={offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_13)};
static struct _tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple7),{(
void*)((struct _tuple6**)Cyc__genarr_119),(void*)((struct _tuple6**)Cyc__genarr_119),(
void*)((struct _tuple6**)Cyc__genarr_119 + 2)}};static char _tmp230[7]="Null_c";
static struct _tuple7 Cyc__gentuple_839={0,{_tmp230,_tmp230,_tmp230 + 7}};static
struct _tuple7*Cyc__genarr_840[1]={& Cyc__gentuple_839};static char _tmp231[7]="Char_c";
static struct _tuple5 Cyc__gentuple_858={0,{_tmp231,_tmp231,_tmp231 + 7},(void*)& Cyc__genrep_853};
static char _tmp232[8]="Short_c";static struct _tuple5 Cyc__gentuple_859={1,{_tmp232,
_tmp232,_tmp232 + 8},(void*)& Cyc__genrep_847};static char _tmp233[6]="Int_c";static
struct _tuple5 Cyc__gentuple_860={2,{_tmp233,_tmp233,_tmp233 + 6},(void*)& Cyc__genrep_418};
static char _tmp234[11]="LongLong_c";static struct _tuple5 Cyc__gentuple_861={3,{
_tmp234,_tmp234,_tmp234 + 11},(void*)& Cyc__genrep_841};static char _tmp235[8]="Float_c";
static struct _tuple5 Cyc__gentuple_862={4,{_tmp235,_tmp235,_tmp235 + 8},(void*)& Cyc__genrep_116};
static char _tmp236[9]="String_c";static struct _tuple5 Cyc__gentuple_863={5,{_tmp236,
_tmp236,_tmp236 + 9},(void*)& Cyc__genrep_116};static struct _tuple5*Cyc__genarr_864[
6]={& Cyc__gentuple_858,& Cyc__gentuple_859,& Cyc__gentuple_860,& Cyc__gentuple_861,&
Cyc__gentuple_862,& Cyc__gentuple_863};static char _tmp238[5]="Cnst";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep={5,{_tmp238,_tmp238,_tmp238 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_840),(void*)((struct _tuple7**)Cyc__genarr_840),(void*)((struct
_tuple7**)Cyc__genarr_840 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_864),(void*)((
struct _tuple5**)Cyc__genarr_864),(void*)((struct _tuple5**)Cyc__genarr_864 + 6)}};
static struct _tuple6 Cyc__gentuple_865={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_866={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_cnst_t_rep};
static struct _tuple6*Cyc__genarr_867[2]={& Cyc__gentuple_865,& Cyc__gentuple_866};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_838={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_867),(void*)((struct _tuple6**)Cyc__genarr_867),(
void*)((struct _tuple6**)Cyc__genarr_867 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_825;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_86;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_590;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_591;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_592;struct _tuple33{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple6 Cyc__gentuple_593={offsetof(struct _tuple33,f1),(void*)&
Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_594={offsetof(struct _tuple33,f2),(
void*)& Cyc__genrep_133};static struct _tuple6 Cyc__gentuple_595={offsetof(struct
_tuple33,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_596[
3]={& Cyc__gentuple_593,& Cyc__gentuple_594,& Cyc__gentuple_595};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_592={4,sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_596),(
void*)((struct _tuple6**)Cyc__genarr_596),(void*)((struct _tuple6**)Cyc__genarr_596
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_591={1,1,(void*)((void*)&
Cyc__genrep_592)};static char _tmp23C[5]="List";static struct _tagged_arr Cyc__genname_600={
_tmp23C,_tmp23C,_tmp23C + 5};static char _tmp23D[3]="hd";static struct _tuple5 Cyc__gentuple_597={
offsetof(struct Cyc_List_List,hd),{_tmp23D,_tmp23D,_tmp23D + 3},(void*)& Cyc__genrep_591};
static char _tmp23E[3]="tl";static struct _tuple5 Cyc__gentuple_598={offsetof(struct
Cyc_List_List,tl),{_tmp23E,_tmp23E,_tmp23E + 3},(void*)& Cyc__genrep_590};static
struct _tuple5*Cyc__genarr_599[2]={& Cyc__gentuple_597,& Cyc__gentuple_598};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_600,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_599),(void*)((struct _tuple5**)Cyc__genarr_599),(void*)((
struct _tuple5**)Cyc__genarr_599 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_590={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_579;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_580;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp241[4]="Opt";static struct _tagged_arr Cyc__genname_583={_tmp241,_tmp241,
_tmp241 + 4};static char _tmp242[2]="v";static struct _tuple5 Cyc__gentuple_581={
offsetof(struct Cyc_Core_Opt,v),{_tmp242,_tmp242,_tmp242 + 2},(void*)& Cyc__genrep_12};
static struct _tuple5*Cyc__genarr_582[1]={& Cyc__gentuple_581};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_583,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_582),(void*)((
struct _tuple5**)Cyc__genarr_582),(void*)((struct _tuple5**)Cyc__genarr_582 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_580={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp245[11]="VarargInfo";static struct _tagged_arr Cyc__genname_589={
_tmp245,_tmp245,_tmp245 + 11};static char _tmp246[5]="name";static struct _tuple5 Cyc__gentuple_584={
offsetof(struct Cyc_Absyn_VarargInfo,name),{_tmp246,_tmp246,_tmp246 + 5},(void*)&
Cyc__genrep_580};static char _tmp247[3]="tq";static struct _tuple5 Cyc__gentuple_585={
offsetof(struct Cyc_Absyn_VarargInfo,tq),{_tmp247,_tmp247,_tmp247 + 3},(void*)& Cyc__genrep_133};
static char _tmp248[5]="type";static struct _tuple5 Cyc__gentuple_586={offsetof(
struct Cyc_Absyn_VarargInfo,type),{_tmp248,_tmp248,_tmp248 + 5},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp249[7]="inject";static struct _tuple5 Cyc__gentuple_587={
offsetof(struct Cyc_Absyn_VarargInfo,inject),{_tmp249,_tmp249,_tmp249 + 7},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_588[4]={& Cyc__gentuple_584,&
Cyc__gentuple_585,& Cyc__gentuple_586,& Cyc__gentuple_587};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_589,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_588),(void*)((
struct _tuple5**)Cyc__genarr_588),(void*)((struct _tuple5**)Cyc__genarr_588 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_579={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_355;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_356;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_357;static struct
_tuple6 Cyc__gentuple_358={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_359={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_360[2]={& Cyc__gentuple_358,&
Cyc__gentuple_359};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_357={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_360),(void*)((struct
_tuple6**)Cyc__genarr_360),(void*)((struct _tuple6**)Cyc__genarr_360 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356={1,1,(void*)((void*)& Cyc__genrep_357)};
static char _tmp24E[5]="List";static struct _tagged_arr Cyc__genname_364={_tmp24E,
_tmp24E,_tmp24E + 5};static char _tmp24F[3]="hd";static struct _tuple5 Cyc__gentuple_361={
offsetof(struct Cyc_List_List,hd),{_tmp24F,_tmp24F,_tmp24F + 3},(void*)& Cyc__genrep_356};
static char _tmp250[3]="tl";static struct _tuple5 Cyc__gentuple_362={offsetof(struct
Cyc_List_List,tl),{_tmp250,_tmp250,_tmp250 + 3},(void*)& Cyc__genrep_355};static
struct _tuple5*Cyc__genarr_363[2]={& Cyc__gentuple_361,& Cyc__gentuple_362};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_364,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_363),(void*)((struct _tuple5**)Cyc__genarr_363),(void*)((
struct _tuple5**)Cyc__genarr_363 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_355={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_161;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_80;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_81;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_81={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple34{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_82={offsetof(struct _tuple34,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_83={offsetof(struct _tuple34,f2),(
void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_84[2]={& Cyc__gentuple_82,&
Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_80={4,sizeof(
struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_84),(void*)((struct _tuple6**)
Cyc__genarr_84),(void*)((struct _tuple6**)Cyc__genarr_84 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_545;struct _tuple35{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*
f3;};static struct _tuple6 Cyc__gentuple_546={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_547={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_161};static struct _tuple6 Cyc__gentuple_548={offsetof(struct
_tuple35,f3),(void*)& Cyc__genrep_161};static struct _tuple6*Cyc__genarr_549[3]={&
Cyc__gentuple_546,& Cyc__gentuple_547,& Cyc__gentuple_548};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_545={4,sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_549),(
void*)((struct _tuple6**)Cyc__genarr_549),(void*)((struct _tuple6**)Cyc__genarr_549
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_541;static struct _tuple6
Cyc__gentuple_542={offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_543={offsetof(struct _tuple34,f2),(void*)& Cyc__genrep_77};
static struct _tuple6*Cyc__genarr_544[2]={& Cyc__gentuple_542,& Cyc__gentuple_543};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_541={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_544),(void*)((struct _tuple6**)Cyc__genarr_544),(
void*)((struct _tuple6**)Cyc__genarr_544 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_535;struct _tuple36{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_536={offsetof(
struct _tuple36,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_537={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_161};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple36,f4),(void*)& Cyc__genrep_161};static struct _tuple6*Cyc__genarr_540[
4]={& Cyc__gentuple_536,& Cyc__gentuple_537,& Cyc__gentuple_538,& Cyc__gentuple_539};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_535={4,sizeof(struct _tuple36),{(
void*)((struct _tuple6**)Cyc__genarr_540),(void*)((struct _tuple6**)Cyc__genarr_540),(
void*)((struct _tuple6**)Cyc__genarr_540 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_530;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_170;static struct
_tuple6 Cyc__gentuple_171={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_172={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_161};
static struct _tuple6*Cyc__genarr_173[2]={& Cyc__gentuple_171,& Cyc__gentuple_172};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_170={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_173),(void*)((struct _tuple6**)Cyc__genarr_173),(
void*)((struct _tuple6**)Cyc__genarr_173 + 2)}};struct _tuple37{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_531={
offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_532={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_170};static struct _tuple6 Cyc__gentuple_533={
offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_161};static struct _tuple6*Cyc__genarr_534[
3]={& Cyc__gentuple_531,& Cyc__gentuple_532,& Cyc__gentuple_533};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_530={4,sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_534),(
void*)((struct _tuple6**)Cyc__genarr_534),(void*)((struct _tuple6**)Cyc__genarr_534
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_526;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_521;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_521={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple38{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_527={offsetof(struct _tuple38,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_528={offsetof(struct _tuple38,f2),(
void*)& Cyc__genrep_521};static struct _tuple6*Cyc__genarr_529[2]={& Cyc__gentuple_527,&
Cyc__gentuple_528};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_526={4,
sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_529),(void*)((
struct _tuple6**)Cyc__genarr_529),(void*)((struct _tuple6**)Cyc__genarr_529 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_520;struct _tuple39{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_522={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_523={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_524={
offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_521};static struct _tuple6*Cyc__genarr_525[
3]={& Cyc__gentuple_522,& Cyc__gentuple_523,& Cyc__gentuple_524};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_520={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_525),(
void*)((struct _tuple6**)Cyc__genarr_525),(void*)((struct _tuple6**)Cyc__genarr_525
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_513;struct _tuple40{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_514={offsetof(struct _tuple40,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_515={offsetof(struct _tuple40,f2),(
void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_516={offsetof(struct
_tuple40,f3),(void*)& Cyc__genrep_170};static struct _tuple6 Cyc__gentuple_517={
offsetof(struct _tuple40,f4),(void*)& Cyc__genrep_170};static struct _tuple6 Cyc__gentuple_518={
offsetof(struct _tuple40,f5),(void*)& Cyc__genrep_161};static struct _tuple6*Cyc__genarr_519[
5]={& Cyc__gentuple_514,& Cyc__gentuple_515,& Cyc__gentuple_516,& Cyc__gentuple_517,&
Cyc__gentuple_518};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_513={4,
sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_519),(void*)((
struct _tuple6**)Cyc__genarr_519),(void*)((struct _tuple6**)Cyc__genarr_519 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_508;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_228;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_229;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_230;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_414;
struct _tuple41{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_415={
offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_416={
offsetof(struct _tuple41,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_417[2]={& Cyc__gentuple_415,& Cyc__gentuple_416};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_414={4,sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_417),(
void*)((struct _tuple6**)Cyc__genarr_417),(void*)((struct _tuple6**)Cyc__genarr_417
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_410;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_235;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp25F[5]="List";static struct _tagged_arr Cyc__genname_239={_tmp25F,
_tmp25F,_tmp25F + 5};static char _tmp260[3]="hd";static struct _tuple5 Cyc__gentuple_236={
offsetof(struct Cyc_List_List,hd),{_tmp260,_tmp260,_tmp260 + 3},(void*)& Cyc__genrep_230};
static char _tmp261[3]="tl";static struct _tuple5 Cyc__gentuple_237={offsetof(struct
Cyc_List_List,tl),{_tmp261,_tmp261,_tmp261 + 3},(void*)& Cyc__genrep_235};static
struct _tuple5*Cyc__genarr_238[2]={& Cyc__gentuple_236,& Cyc__gentuple_237};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_239,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_238),(void*)((struct _tuple5**)Cyc__genarr_238),(void*)((
struct _tuple5**)Cyc__genarr_238 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_235={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};static struct
_tuple6 Cyc__gentuple_411={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_412={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_235};
static struct _tuple6*Cyc__genarr_413[2]={& Cyc__gentuple_411,& Cyc__gentuple_412};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_410={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_413),(void*)((struct _tuple6**)Cyc__genarr_413),(
void*)((struct _tuple6**)Cyc__genarr_413 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_406;struct _tuple42{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple6 Cyc__gentuple_407={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_408={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_230};
static struct _tuple6*Cyc__genarr_409[2]={& Cyc__gentuple_407,& Cyc__gentuple_408};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_406={4,sizeof(struct _tuple42),{(
void*)((struct _tuple6**)Cyc__genarr_409),(void*)((struct _tuple6**)Cyc__genarr_409),(
void*)((struct _tuple6**)Cyc__genarr_409 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_313;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_385;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static char _tmp266[8]="StructA";static struct _tuple7 Cyc__gentuple_370={0,{_tmp266,
_tmp266,_tmp266 + 8}};static char _tmp267[7]="UnionA";static struct _tuple7 Cyc__gentuple_371={
1,{_tmp267,_tmp267,_tmp267 + 7}};static struct _tuple7*Cyc__genarr_372[2]={& Cyc__gentuple_370,&
Cyc__gentuple_371};static struct _tuple5*Cyc__genarr_373[0]={};static char _tmp269[9]="AggrKind";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,{_tmp269,_tmp269,
_tmp269 + 9},{(void*)((struct _tuple7**)Cyc__genarr_372),(void*)((struct _tuple7**)
Cyc__genarr_372),(void*)((struct _tuple7**)Cyc__genarr_372 + 2)},{(void*)((struct
_tuple5**)Cyc__genarr_373),(void*)((struct _tuple5**)Cyc__genarr_373),(void*)((
struct _tuple5**)Cyc__genarr_373 + 0)}};struct _tuple43{unsigned int f1;void*f2;
struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_386={offsetof(struct _tuple43,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_387={offsetof(struct
_tuple43,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct _tuple6 Cyc__gentuple_388={
offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_389[
3]={& Cyc__gentuple_386,& Cyc__gentuple_387,& Cyc__gentuple_388};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_385={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_389),(
void*)((struct _tuple6**)Cyc__genarr_389),(void*)((struct _tuple6**)Cyc__genarr_389
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_338;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_339;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_341;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_343;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_87;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_106;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp26B[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_107={0,{_tmp26B,_tmp26B,
_tmp26B + 10}};static char _tmp26C[9]="Scanf_ft";static struct _tuple7 Cyc__gentuple_108={
1,{_tmp26C,_tmp26C,_tmp26C + 9}};static struct _tuple7*Cyc__genarr_109[2]={& Cyc__gentuple_107,&
Cyc__gentuple_108};static struct _tuple5*Cyc__genarr_110[0]={};static char _tmp26E[
12]="Format_Type";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={
5,{_tmp26E,_tmp26E,_tmp26E + 12},{(void*)((struct _tuple7**)Cyc__genarr_109),(void*)((
struct _tuple7**)Cyc__genarr_109),(void*)((struct _tuple7**)Cyc__genarr_109 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_110),(void*)((struct _tuple5**)Cyc__genarr_110),(
void*)((struct _tuple5**)Cyc__genarr_110 + 0)}};struct _tuple44{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_111={offsetof(struct _tuple44,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_112={offsetof(struct
_tuple44,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_113={
offsetof(struct _tuple44,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_114={offsetof(struct _tuple44,f4),(void*)((void*)& Cyc__genrep_62)};
static struct _tuple6*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_106={
4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_115),(void*)((
struct _tuple6**)Cyc__genarr_115),(void*)((struct _tuple6**)Cyc__genarr_115 + 4)}};
static char _tmp270[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_88={0,{
_tmp270,_tmp270,_tmp270 + 12}};static char _tmp271[10]="Cdecl_att";static struct
_tuple7 Cyc__gentuple_89={1,{_tmp271,_tmp271,_tmp271 + 10}};static char _tmp272[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_90={2,{_tmp272,_tmp272,_tmp272 + 13}};static char
_tmp273[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_91={3,{_tmp273,
_tmp273,_tmp273 + 13}};static char _tmp274[10]="Const_att";static struct _tuple7 Cyc__gentuple_92={
4,{_tmp274,_tmp274,_tmp274 + 10}};static char _tmp275[11]="Packed_att";static struct
_tuple7 Cyc__gentuple_93={5,{_tmp275,_tmp275,_tmp275 + 11}};static char _tmp276[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_94={6,{_tmp276,_tmp276,_tmp276 + 13}};static char
_tmp277[11]="Shared_att";static struct _tuple7 Cyc__gentuple_95={7,{_tmp277,_tmp277,
_tmp277 + 11}};static char _tmp278[11]="Unused_att";static struct _tuple7 Cyc__gentuple_96={
8,{_tmp278,_tmp278,_tmp278 + 11}};static char _tmp279[9]="Weak_att";static struct
_tuple7 Cyc__gentuple_97={9,{_tmp279,_tmp279,_tmp279 + 9}};static char _tmp27A[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_98={10,{_tmp27A,_tmp27A,_tmp27A + 14}};static
char _tmp27B[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_99={11,{_tmp27B,
_tmp27B,_tmp27B + 14}};static char _tmp27C[27]="No_instrument_function_att";static
struct _tuple7 Cyc__gentuple_100={12,{_tmp27C,_tmp27C,_tmp27C + 27}};static char
_tmp27D[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_101={13,{_tmp27D,
_tmp27D,_tmp27D + 16}};static char _tmp27E[15]="Destructor_att";static struct _tuple7
Cyc__gentuple_102={14,{_tmp27E,_tmp27E,_tmp27E + 15}};static char _tmp27F[26]="No_check_memory_usage_att";
static struct _tuple7 Cyc__gentuple_103={15,{_tmp27F,_tmp27F,_tmp27F + 26}};static
char _tmp280[9]="Pure_att";static struct _tuple7 Cyc__gentuple_104={16,{_tmp280,
_tmp280,_tmp280 + 9}};static struct _tuple7*Cyc__genarr_105[17]={& Cyc__gentuple_88,&
Cyc__gentuple_89,& Cyc__gentuple_90,& Cyc__gentuple_91,& Cyc__gentuple_92,& Cyc__gentuple_93,&
Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97,& Cyc__gentuple_98,&
Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,& Cyc__gentuple_102,& Cyc__gentuple_103,&
Cyc__gentuple_104};static char _tmp281[12]="Regparm_att";static struct _tuple5 Cyc__gentuple_120={
0,{_tmp281,_tmp281,_tmp281 + 12},(void*)& Cyc__genrep_61};static char _tmp282[12]="Aligned_att";
static struct _tuple5 Cyc__gentuple_121={1,{_tmp282,_tmp282,_tmp282 + 12},(void*)&
Cyc__genrep_61};static char _tmp283[12]="Section_att";static struct _tuple5 Cyc__gentuple_122={
2,{_tmp283,_tmp283,_tmp283 + 12},(void*)& Cyc__genrep_116};static char _tmp284[11]="Format_att";
static struct _tuple5 Cyc__gentuple_123={3,{_tmp284,_tmp284,_tmp284 + 11},(void*)&
Cyc__genrep_106};static char _tmp285[16]="Initializes_att";static struct _tuple5 Cyc__gentuple_124={
4,{_tmp285,_tmp285,_tmp285 + 16},(void*)& Cyc__genrep_61};static struct _tuple5*Cyc__genarr_125[
5]={& Cyc__gentuple_120,& Cyc__gentuple_121,& Cyc__gentuple_122,& Cyc__gentuple_123,&
Cyc__gentuple_124};static char _tmp287[10]="Attribute";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep={5,{_tmp287,_tmp287,_tmp287 + 10},{(void*)((struct
_tuple7**)Cyc__genarr_105),(void*)((struct _tuple7**)Cyc__genarr_105),(void*)((
struct _tuple7**)Cyc__genarr_105 + 17)},{(void*)((struct _tuple5**)Cyc__genarr_125),(
void*)((struct _tuple5**)Cyc__genarr_125),(void*)((struct _tuple5**)Cyc__genarr_125
+ 5)}};static char _tmp288[5]="List";static struct _tagged_arr Cyc__genname_129={
_tmp288,_tmp288,_tmp288 + 5};static char _tmp289[3]="hd";static struct _tuple5 Cyc__gentuple_126={
offsetof(struct Cyc_List_List,hd),{_tmp289,_tmp289,_tmp289 + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static char _tmp28A[3]="tl";static struct _tuple5 Cyc__gentuple_127={offsetof(struct
Cyc_List_List,tl),{_tmp28A,_tmp28A,_tmp28A + 3},(void*)& Cyc__genrep_87};static
struct _tuple5*Cyc__genarr_128[2]={& Cyc__gentuple_126,& Cyc__gentuple_127};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_129,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_128),(void*)((struct _tuple5**)Cyc__genarr_128),(void*)((
struct _tuple5**)Cyc__genarr_128 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_87={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp28D[10]="Aggrfield";static struct _tagged_arr Cyc__genname_350={_tmp28D,_tmp28D,
_tmp28D + 10};static char _tmp28E[5]="name";static struct _tuple5 Cyc__gentuple_344={
offsetof(struct Cyc_Absyn_Aggrfield,name),{_tmp28E,_tmp28E,_tmp28E + 5},(void*)&
Cyc__genrep_12};static char _tmp28F[3]="tq";static struct _tuple5 Cyc__gentuple_345={
offsetof(struct Cyc_Absyn_Aggrfield,tq),{_tmp28F,_tmp28F,_tmp28F + 3},(void*)& Cyc__genrep_133};
static char _tmp290[5]="type";static struct _tuple5 Cyc__gentuple_346={offsetof(
struct Cyc_Absyn_Aggrfield,type),{_tmp290,_tmp290,_tmp290 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp291[6]="width";static struct _tuple5 Cyc__gentuple_347={offsetof(
struct Cyc_Absyn_Aggrfield,width),{_tmp291,_tmp291,_tmp291 + 6},(void*)& Cyc__genrep_77};
static char _tmp292[11]="attributes";static struct _tuple5 Cyc__gentuple_348={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),{_tmp292,_tmp292,_tmp292 + 11},(
void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_349[5]={& Cyc__gentuple_344,&
Cyc__gentuple_345,& Cyc__gentuple_346,& Cyc__gentuple_347,& Cyc__gentuple_348};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_350,sizeof(struct Cyc_Absyn_Aggrfield),{(void*)((
struct _tuple5**)Cyc__genarr_349),(void*)((struct _tuple5**)Cyc__genarr_349),(void*)((
struct _tuple5**)Cyc__genarr_349 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_343={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp295[5]="List";
static struct _tagged_arr Cyc__genname_354={_tmp295,_tmp295,_tmp295 + 5};static char
_tmp296[3]="hd";static struct _tuple5 Cyc__gentuple_351={offsetof(struct Cyc_List_List,hd),{
_tmp296,_tmp296,_tmp296 + 3},(void*)& Cyc__genrep_343};static char _tmp297[3]="tl";
static struct _tuple5 Cyc__gentuple_352={offsetof(struct Cyc_List_List,tl),{_tmp297,
_tmp297,_tmp297 + 3},(void*)& Cyc__genrep_342};static struct _tuple5*Cyc__genarr_353[
2]={& Cyc__gentuple_351,& Cyc__gentuple_352};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_354,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_353),(void*)((struct _tuple5**)Cyc__genarr_353),(void*)((
struct _tuple5**)Cyc__genarr_353 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp29A[13]="AggrdeclImpl";static struct _tagged_arr Cyc__genname_369={_tmp29A,
_tmp29A,_tmp29A + 13};static char _tmp29B[11]="exist_vars";static struct _tuple5 Cyc__gentuple_365={
offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),{_tmp29B,_tmp29B,_tmp29B + 11},(
void*)& Cyc__genrep_296};static char _tmp29C[7]="rgn_po";static struct _tuple5 Cyc__gentuple_366={
offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),{_tmp29C,_tmp29C,_tmp29C + 7},(void*)&
Cyc__genrep_355};static char _tmp29D[7]="fields";static struct _tuple5 Cyc__gentuple_367={
offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),{_tmp29D,_tmp29D,_tmp29D + 7},(void*)&
Cyc__genrep_342};static struct _tuple5*Cyc__genarr_368[3]={& Cyc__gentuple_365,& Cyc__gentuple_366,&
Cyc__gentuple_367};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={
3,(struct _tagged_arr*)& Cyc__genname_369,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(
void*)((struct _tuple5**)Cyc__genarr_368),(void*)((struct _tuple5**)Cyc__genarr_368),(
void*)((struct _tuple5**)Cyc__genarr_368 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_341={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static
char _tmp2A0[9]="Aggrdecl";static struct _tagged_arr Cyc__genname_381={_tmp2A0,
_tmp2A0,_tmp2A0 + 9};static char _tmp2A1[5]="kind";static struct _tuple5 Cyc__gentuple_374={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),{_tmp2A1,_tmp2A1,_tmp2A1 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};
static char _tmp2A2[3]="sc";static struct _tuple5 Cyc__gentuple_375={offsetof(struct
Cyc_Absyn_Aggrdecl,sc),{_tmp2A2,_tmp2A2,_tmp2A2 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp2A3[5]="name";static struct _tuple5 Cyc__gentuple_376={offsetof(
struct Cyc_Absyn_Aggrdecl,name),{_tmp2A3,_tmp2A3,_tmp2A3 + 5},(void*)& Cyc__genrep_10};
static char _tmp2A4[4]="tvs";static struct _tuple5 Cyc__gentuple_377={offsetof(struct
Cyc_Absyn_Aggrdecl,tvs),{_tmp2A4,_tmp2A4,_tmp2A4 + 4},(void*)& Cyc__genrep_296};
static char _tmp2A5[5]="impl";static struct _tuple5 Cyc__gentuple_378={offsetof(
struct Cyc_Absyn_Aggrdecl,impl),{_tmp2A5,_tmp2A5,_tmp2A5 + 5},(void*)& Cyc__genrep_341};
static char _tmp2A6[11]="attributes";static struct _tuple5 Cyc__gentuple_379={
offsetof(struct Cyc_Absyn_Aggrdecl,attributes),{_tmp2A6,_tmp2A6,_tmp2A6 + 11},(
void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_380[6]={& Cyc__gentuple_374,&
Cyc__gentuple_375,& Cyc__gentuple_376,& Cyc__gentuple_377,& Cyc__gentuple_378,& Cyc__gentuple_379};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_381,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((struct
_tuple5**)Cyc__genarr_380),(void*)((struct _tuple5**)Cyc__genarr_380),(void*)((
struct _tuple5**)Cyc__genarr_380 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_339={1,1,(void*)((void*)& Cyc__genrep_340)};struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_382={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_383={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_339};static struct _tuple6*Cyc__genarr_384[
2]={& Cyc__gentuple_382,& Cyc__gentuple_383};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_338={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_384),(
void*)((struct _tuple6**)Cyc__genarr_384),(void*)((struct _tuple6**)Cyc__genarr_384
+ 2)}};static struct _tuple7*Cyc__genarr_337[0]={};static char _tmp2AB[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_390={0,{_tmp2AB,_tmp2AB,_tmp2AB + 12},(void*)&
Cyc__genrep_385};static char _tmp2AC[10]="KnownAggr";static struct _tuple5 Cyc__gentuple_391={
1,{_tmp2AC,_tmp2AC,_tmp2AC + 10},(void*)& Cyc__genrep_338};static struct _tuple5*Cyc__genarr_392[
2]={& Cyc__gentuple_390,& Cyc__gentuple_391};static char _tmp2AE[10]="AggrInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={5,{_tmp2AE,
_tmp2AE,_tmp2AE + 10},{(void*)((struct _tuple7**)Cyc__genarr_337),(void*)((struct
_tuple7**)Cyc__genarr_337),(void*)((struct _tuple7**)Cyc__genarr_337 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_392),(void*)((struct _tuple5**)Cyc__genarr_392),(void*)((
struct _tuple5**)Cyc__genarr_392 + 2)}};static char _tmp2AF[9]="AggrInfo";static
struct _tagged_arr Cyc__genname_396={_tmp2AF,_tmp2AF,_tmp2AF + 9};static char _tmp2B0[
10]="aggr_info";static struct _tuple5 Cyc__gentuple_393={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),{
_tmp2B0,_tmp2B0,_tmp2B0 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char
_tmp2B1[6]="targs";static struct _tuple5 Cyc__gentuple_394={offsetof(struct Cyc_Absyn_AggrInfo,targs),{
_tmp2B1,_tmp2B1,_tmp2B1 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_395[
2]={& Cyc__gentuple_393,& Cyc__gentuple_394};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_396,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_395),(void*)((struct _tuple5**)Cyc__genarr_395),(void*)((
struct _tuple5**)Cyc__genarr_395 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_314;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_315;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_316;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_317;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_319;struct _tuple46{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple6 Cyc__gentuple_320={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_321={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_12};
static struct _tuple6*Cyc__genarr_322[2]={& Cyc__gentuple_320,& Cyc__gentuple_321};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_319={4,sizeof(struct _tuple46),{(
void*)((struct _tuple6**)Cyc__genarr_322),(void*)((struct _tuple6**)Cyc__genarr_322),(
void*)((struct _tuple6**)Cyc__genarr_322 + 2)}};static struct _tuple7*Cyc__genarr_318[
0]={};static char _tmp2B4[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_323={
0,{_tmp2B4,_tmp2B4,_tmp2B4 + 13},(void*)& Cyc__genrep_80};static char _tmp2B5[10]="FieldName";
static struct _tuple5 Cyc__gentuple_324={1,{_tmp2B5,_tmp2B5,_tmp2B5 + 10},(void*)&
Cyc__genrep_319};static struct _tuple5*Cyc__genarr_325[2]={& Cyc__gentuple_323,& Cyc__gentuple_324};
static char _tmp2B7[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,{_tmp2B7,_tmp2B7,_tmp2B7 + 11},{(void*)((struct _tuple7**)Cyc__genarr_318),(void*)((
struct _tuple7**)Cyc__genarr_318),(void*)((struct _tuple7**)Cyc__genarr_318 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_325),(void*)((struct _tuple5**)Cyc__genarr_325),(
void*)((struct _tuple5**)Cyc__genarr_325 + 2)}};static char _tmp2B8[5]="List";static
struct _tagged_arr Cyc__genname_329={_tmp2B8,_tmp2B8,_tmp2B8 + 5};static char _tmp2B9[
3]="hd";static struct _tuple5 Cyc__gentuple_326={offsetof(struct Cyc_List_List,hd),{
_tmp2B9,_tmp2B9,_tmp2B9 + 3},(void*)& Cyc_Absyn_designator_t_rep};static char
_tmp2BA[3]="tl";static struct _tuple5 Cyc__gentuple_327={offsetof(struct Cyc_List_List,tl),{
_tmp2BA,_tmp2BA,_tmp2BA + 3},(void*)& Cyc__genrep_317};static struct _tuple5*Cyc__genarr_328[
2]={& Cyc__gentuple_326,& Cyc__gentuple_327};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_329,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_328),(void*)((struct _tuple5**)Cyc__genarr_328),(void*)((
struct _tuple5**)Cyc__genarr_328 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_317={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple47{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_330={
offsetof(struct _tuple47,f1),(void*)& Cyc__genrep_317};static struct _tuple6 Cyc__gentuple_331={
offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_230};static struct _tuple6*Cyc__genarr_332[
2]={& Cyc__gentuple_330,& Cyc__gentuple_331};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_316={4,sizeof(struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_332),(
void*)((struct _tuple6**)Cyc__genarr_332),(void*)((struct _tuple6**)Cyc__genarr_332
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_315={1,1,(void*)((void*)&
Cyc__genrep_316)};static char _tmp2BF[5]="List";static struct _tagged_arr Cyc__genname_336={
_tmp2BF,_tmp2BF,_tmp2BF + 5};static char _tmp2C0[3]="hd";static struct _tuple5 Cyc__gentuple_333={
offsetof(struct Cyc_List_List,hd),{_tmp2C0,_tmp2C0,_tmp2C0 + 3},(void*)& Cyc__genrep_315};
static char _tmp2C1[3]="tl";static struct _tuple5 Cyc__gentuple_334={offsetof(struct
Cyc_List_List,tl),{_tmp2C1,_tmp2C1,_tmp2C1 + 3},(void*)& Cyc__genrep_314};static
struct _tuple5*Cyc__genarr_335[2]={& Cyc__gentuple_333,& Cyc__gentuple_334};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_336,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_335),(void*)((struct _tuple5**)Cyc__genarr_335),(void*)((
struct _tuple5**)Cyc__genarr_335 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_314={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};static struct _tuple6 Cyc__gentuple_397={offsetof(struct
_tuple48,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_398={
offsetof(struct _tuple48,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_399={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_296};
static struct _tuple6 Cyc__gentuple_400={offsetof(struct _tuple48,f4),(void*)& Cyc__genrep_314};
static struct _tuple6*Cyc__genarr_401[4]={& Cyc__gentuple_397,& Cyc__gentuple_398,&
Cyc__gentuple_399,& Cyc__gentuple_400};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_313={
4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_401),(void*)((
struct _tuple6**)Cyc__genarr_401),(void*)((struct _tuple6**)Cyc__genarr_401 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_268;struct _tuple49{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*
f4;};static struct _tuple6 Cyc__gentuple_308={offsetof(struct _tuple49,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_309={offsetof(struct _tuple49,f2),(
void*)((void*)& Cyc__genrep_286)};static struct _tuple6 Cyc__gentuple_310={offsetof(
struct _tuple49,f3),(void*)& Cyc__genrep_269};static struct _tuple6 Cyc__gentuple_311={
offsetof(struct _tuple49,f4),(void*)& Cyc__genrep_235};static struct _tuple6*Cyc__genarr_312[
4]={& Cyc__gentuple_308,& Cyc__gentuple_309,& Cyc__gentuple_310,& Cyc__gentuple_311};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_268={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_312),(void*)((struct _tuple6**)Cyc__genarr_312),(
void*)((struct _tuple6**)Cyc__genarr_312 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_253;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_255;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_75;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_76;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep;
static char _tmp2C6[10]="Enumfield";static struct _tagged_arr Cyc__genname_917={
_tmp2C6,_tmp2C6,_tmp2C6 + 10};static char _tmp2C7[5]="name";static struct _tuple5 Cyc__gentuple_913={
offsetof(struct Cyc_Absyn_Enumfield,name),{_tmp2C7,_tmp2C7,_tmp2C7 + 5},(void*)&
Cyc__genrep_10};static char _tmp2C8[4]="tag";static struct _tuple5 Cyc__gentuple_914={
offsetof(struct Cyc_Absyn_Enumfield,tag),{_tmp2C8,_tmp2C8,_tmp2C8 + 4},(void*)& Cyc__genrep_77};
static char _tmp2C9[4]="loc";static struct _tuple5 Cyc__gentuple_915={offsetof(struct
Cyc_Absyn_Enumfield,loc),{_tmp2C9,_tmp2C9,_tmp2C9 + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_916[3]={& Cyc__gentuple_913,& Cyc__gentuple_914,&
Cyc__gentuple_915};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_917,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_916),(void*)((struct _tuple5**)Cyc__genarr_916),(void*)((
struct _tuple5**)Cyc__genarr_916 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_76={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp2CC[5]="List";
static struct _tagged_arr Cyc__genname_921={_tmp2CC,_tmp2CC,_tmp2CC + 5};static char
_tmp2CD[3]="hd";static struct _tuple5 Cyc__gentuple_918={offsetof(struct Cyc_List_List,hd),{
_tmp2CD,_tmp2CD,_tmp2CD + 3},(void*)& Cyc__genrep_76};static char _tmp2CE[3]="tl";
static struct _tuple5 Cyc__gentuple_919={offsetof(struct Cyc_List_List,tl),{_tmp2CE,
_tmp2CE,_tmp2CE + 3},(void*)& Cyc__genrep_75};static struct _tuple5*Cyc__genarr_920[
2]={& Cyc__gentuple_918,& Cyc__gentuple_919};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_921,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_920),(void*)((struct _tuple5**)Cyc__genarr_920),(void*)((
struct _tuple5**)Cyc__genarr_920 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_75={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp2D1[4]="Opt";static struct _tagged_arr Cyc__genname_258={_tmp2D1,_tmp2D1,
_tmp2D1 + 4};static char _tmp2D2[2]="v";static struct _tuple5 Cyc__gentuple_256={
offsetof(struct Cyc_Core_Opt,v),{_tmp2D2,_tmp2D2,_tmp2D2 + 2},(void*)& Cyc__genrep_75};
static struct _tuple5*Cyc__genarr_257[1]={& Cyc__gentuple_256};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_258,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_257),(
void*)((struct _tuple5**)Cyc__genarr_257),(void*)((struct _tuple5**)Cyc__genarr_257
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_255={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};static char _tmp2D5[9]="Enumdecl";
static struct _tagged_arr Cyc__genname_263={_tmp2D5,_tmp2D5,_tmp2D5 + 9};static char
_tmp2D6[3]="sc";static struct _tuple5 Cyc__gentuple_259={offsetof(struct Cyc_Absyn_Enumdecl,sc),{
_tmp2D6,_tmp2D6,_tmp2D6 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp2D7[5]="name";
static struct _tuple5 Cyc__gentuple_260={offsetof(struct Cyc_Absyn_Enumdecl,name),{
_tmp2D7,_tmp2D7,_tmp2D7 + 5},(void*)& Cyc__genrep_10};static char _tmp2D8[7]="fields";
static struct _tuple5 Cyc__gentuple_261={offsetof(struct Cyc_Absyn_Enumdecl,fields),{
_tmp2D8,_tmp2D8,_tmp2D8 + 7},(void*)& Cyc__genrep_255};static struct _tuple5*Cyc__genarr_262[
3]={& Cyc__gentuple_259,& Cyc__gentuple_260,& Cyc__gentuple_261};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_263,sizeof(
struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_262),(void*)((
struct _tuple5**)Cyc__genarr_262),(void*)((struct _tuple5**)Cyc__genarr_262 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple50{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_264={offsetof(struct _tuple50,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_265={offsetof(struct _tuple50,f2),(
void*)& Cyc__genrep_254};static struct _tuple6 Cyc__gentuple_266={offsetof(struct
_tuple50,f3),(void*)& Cyc__genrep_76};static struct _tuple6*Cyc__genarr_267[3]={&
Cyc__gentuple_264,& Cyc__gentuple_265,& Cyc__gentuple_266};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_253={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_267),(
void*)((struct _tuple6**)Cyc__genarr_267),(void*)((struct _tuple6**)Cyc__genarr_267
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_248;struct _tuple51{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_249={
offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_250={
offsetof(struct _tuple51,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_251={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_76};
static struct _tuple6*Cyc__genarr_252[3]={& Cyc__gentuple_249,& Cyc__gentuple_250,&
Cyc__gentuple_251};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_248={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_252),(void*)((
struct _tuple6**)Cyc__genarr_252),(void*)((struct _tuple6**)Cyc__genarr_252 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_244;struct _tuple52{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_245={offsetof(struct
_tuple52,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_246={
offsetof(struct _tuple52,f2),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_247[
2]={& Cyc__gentuple_245,& Cyc__gentuple_246};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_244={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_247),(
void*)((struct _tuple6**)Cyc__genarr_247),(void*)((struct _tuple6**)Cyc__genarr_247
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_234;struct _tuple53{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_240={
offsetof(struct _tuple53,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_241={
offsetof(struct _tuple53,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_242={
offsetof(struct _tuple53,f3),(void*)& Cyc__genrep_235};static struct _tuple6*Cyc__genarr_243[
3]={& Cyc__gentuple_240,& Cyc__gentuple_241,& Cyc__gentuple_242};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_234={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_243),(
void*)((struct _tuple6**)Cyc__genarr_243),(void*)((struct _tuple6**)Cyc__genarr_243
+ 3)}};static char _tmp2DF[7]="Wild_p";static struct _tuple7 Cyc__gentuple_231={0,{
_tmp2DF,_tmp2DF,_tmp2DF + 7}};static char _tmp2E0[7]="Null_p";static struct _tuple7
Cyc__gentuple_232={1,{_tmp2E0,_tmp2E0,_tmp2E0 + 7}};static struct _tuple7*Cyc__genarr_233[
2]={& Cyc__gentuple_231,& Cyc__gentuple_232};static char _tmp2E1[6]="Var_p";static
struct _tuple5 Cyc__gentuple_428={0,{_tmp2E1,_tmp2E1,_tmp2E1 + 6},(void*)& Cyc__genrep_402};
static char _tmp2E2[6]="Int_p";static struct _tuple5 Cyc__gentuple_429={1,{_tmp2E2,
_tmp2E2,_tmp2E2 + 6},(void*)& Cyc__genrep_418};static char _tmp2E3[7]="Char_p";
static struct _tuple5 Cyc__gentuple_430={2,{_tmp2E3,_tmp2E3,_tmp2E3 + 7},(void*)& Cyc__genrep_414};
static char _tmp2E4[8]="Float_p";static struct _tuple5 Cyc__gentuple_431={3,{_tmp2E4,
_tmp2E4,_tmp2E4 + 8},(void*)& Cyc__genrep_116};static char _tmp2E5[8]="Tuple_p";
static struct _tuple5 Cyc__gentuple_432={4,{_tmp2E5,_tmp2E5,_tmp2E5 + 8},(void*)& Cyc__genrep_410};
static char _tmp2E6[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_433={5,{
_tmp2E6,_tmp2E6,_tmp2E6 + 10},(void*)& Cyc__genrep_406};static char _tmp2E7[12]="Reference_p";
static struct _tuple5 Cyc__gentuple_434={6,{_tmp2E7,_tmp2E7,_tmp2E7 + 12},(void*)&
Cyc__genrep_402};static char _tmp2E8[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_435={
7,{_tmp2E8,_tmp2E8,_tmp2E8 + 7},(void*)& Cyc__genrep_313};static char _tmp2E9[9]="Tunion_p";
static struct _tuple5 Cyc__gentuple_436={8,{_tmp2E9,_tmp2E9,_tmp2E9 + 9},(void*)& Cyc__genrep_268};
static char _tmp2EA[7]="Enum_p";static struct _tuple5 Cyc__gentuple_437={9,{_tmp2EA,
_tmp2EA,_tmp2EA + 7},(void*)& Cyc__genrep_253};static char _tmp2EB[11]="AnonEnum_p";
static struct _tuple5 Cyc__gentuple_438={10,{_tmp2EB,_tmp2EB,_tmp2EB + 11},(void*)&
Cyc__genrep_248};static char _tmp2EC[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_439={
11,{_tmp2EC,_tmp2EC,_tmp2EC + 12},(void*)& Cyc__genrep_244};static char _tmp2ED[14]="UnknownCall_p";
static struct _tuple5 Cyc__gentuple_440={12,{_tmp2ED,_tmp2ED,_tmp2ED + 14},(void*)&
Cyc__genrep_234};static struct _tuple5*Cyc__genarr_441[13]={& Cyc__gentuple_428,&
Cyc__gentuple_429,& Cyc__gentuple_430,& Cyc__gentuple_431,& Cyc__gentuple_432,& Cyc__gentuple_433,&
Cyc__gentuple_434,& Cyc__gentuple_435,& Cyc__gentuple_436,& Cyc__gentuple_437,& Cyc__gentuple_438,&
Cyc__gentuple_439,& Cyc__gentuple_440};static char _tmp2EF[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,{_tmp2EF,_tmp2EF,_tmp2EF + 8},{(void*)((struct _tuple7**)
Cyc__genarr_233),(void*)((struct _tuple7**)Cyc__genarr_233),(void*)((struct
_tuple7**)Cyc__genarr_233 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_441),(void*)((
struct _tuple5**)Cyc__genarr_441),(void*)((struct _tuple5**)Cyc__genarr_441 + 13)}};
static char _tmp2F0[4]="Pat";static struct _tagged_arr Cyc__genname_446={_tmp2F0,
_tmp2F0,_tmp2F0 + 4};static char _tmp2F1[2]="r";static struct _tuple5 Cyc__gentuple_442={
offsetof(struct Cyc_Absyn_Pat,r),{_tmp2F1,_tmp2F1,_tmp2F1 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp2F2[5]="topt";static struct _tuple5 Cyc__gentuple_443={offsetof(
struct Cyc_Absyn_Pat,topt),{_tmp2F2,_tmp2F2,_tmp2F2 + 5},(void*)& Cyc__genrep_43};
static char _tmp2F3[4]="loc";static struct _tuple5 Cyc__gentuple_444={offsetof(struct
Cyc_Absyn_Pat,loc),{_tmp2F3,_tmp2F3,_tmp2F3 + 4},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_445[3]={& Cyc__gentuple_442,& Cyc__gentuple_443,& Cyc__gentuple_444};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct _tagged_arr*)&
Cyc__genname_446,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct _tuple5**)Cyc__genarr_445),(
void*)((struct _tuple5**)Cyc__genarr_445),(void*)((struct _tuple5**)Cyc__genarr_445
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_230={1,1,(void*)((void*)&
Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp2F6[5]="List";static
struct _tagged_arr Cyc__genname_157={_tmp2F6,_tmp2F6,_tmp2F6 + 5};static char _tmp2F7[
3]="hd";static struct _tuple5 Cyc__gentuple_154={offsetof(struct Cyc_List_List,hd),{
_tmp2F7,_tmp2F7,_tmp2F7 + 3},(void*)& Cyc__genrep_132};static char _tmp2F8[3]="tl";
static struct _tuple5 Cyc__gentuple_155={offsetof(struct Cyc_List_List,tl),{_tmp2F8,
_tmp2F8,_tmp2F8 + 3},(void*)& Cyc__genrep_131};static struct _tuple5*Cyc__genarr_156[
2]={& Cyc__gentuple_154,& Cyc__gentuple_155};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_157,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_156),(void*)((struct _tuple5**)Cyc__genarr_156),(void*)((
struct _tuple5**)Cyc__genarr_156 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp2FB[4]="Opt";static struct _tagged_arr Cyc__genname_160={_tmp2FB,_tmp2FB,
_tmp2FB + 4};static char _tmp2FC[2]="v";static struct _tuple5 Cyc__gentuple_158={
offsetof(struct Cyc_Core_Opt,v),{_tmp2FC,_tmp2FC,_tmp2FC + 2},(void*)& Cyc__genrep_131};
static struct _tuple5*Cyc__genarr_159[1]={& Cyc__gentuple_158};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_160,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_159),(
void*)((struct _tuple5**)Cyc__genarr_159),(void*)((struct _tuple5**)Cyc__genarr_159
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};static char _tmp2FF[14]="Switch_clause";
static struct _tagged_arr Cyc__genname_453={_tmp2FF,_tmp2FF,_tmp2FF + 14};static char
_tmp300[8]="pattern";static struct _tuple5 Cyc__gentuple_447={offsetof(struct Cyc_Absyn_Switch_clause,pattern),{
_tmp300,_tmp300,_tmp300 + 8},(void*)& Cyc__genrep_230};static char _tmp301[9]="pat_vars";
static struct _tuple5 Cyc__gentuple_448={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),{
_tmp301,_tmp301,_tmp301 + 9},(void*)& Cyc__genrep_130};static char _tmp302[13]="where_clause";
static struct _tuple5 Cyc__gentuple_449={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),{
_tmp302,_tmp302,_tmp302 + 13},(void*)& Cyc__genrep_77};static char _tmp303[5]="body";
static struct _tuple5 Cyc__gentuple_450={offsetof(struct Cyc_Absyn_Switch_clause,body),{
_tmp303,_tmp303,_tmp303 + 5},(void*)& Cyc__genrep_161};static char _tmp304[4]="loc";
static struct _tuple5 Cyc__gentuple_451={offsetof(struct Cyc_Absyn_Switch_clause,loc),{
_tmp304,_tmp304,_tmp304 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_452[
5]={& Cyc__gentuple_447,& Cyc__gentuple_448,& Cyc__gentuple_449,& Cyc__gentuple_450,&
Cyc__gentuple_451};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_453,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_452),(void*)((struct _tuple5**)Cyc__genarr_452),(
void*)((struct _tuple5**)Cyc__genarr_452 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_229={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp307[5]="List";static struct _tagged_arr Cyc__genname_457={_tmp307,_tmp307,
_tmp307 + 5};static char _tmp308[3]="hd";static struct _tuple5 Cyc__gentuple_454={
offsetof(struct Cyc_List_List,hd),{_tmp308,_tmp308,_tmp308 + 3},(void*)((void*)&
Cyc__genrep_229)};static char _tmp309[3]="tl";static struct _tuple5 Cyc__gentuple_455={
offsetof(struct Cyc_List_List,tl),{_tmp309,_tmp309,_tmp309 + 3},(void*)& Cyc__genrep_228};
static struct _tuple5*Cyc__genarr_456[2]={& Cyc__gentuple_454,& Cyc__gentuple_455};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_457,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_456),(void*)((struct _tuple5**)Cyc__genarr_456),(void*)((
struct _tuple5**)Cyc__genarr_456 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_228={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple54{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_509={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_510={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_228};
static struct _tuple6*Cyc__genarr_512[3]={& Cyc__gentuple_509,& Cyc__gentuple_510,&
Cyc__gentuple_511};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_508={4,
sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_512),(void*)((
struct _tuple6**)Cyc__genarr_512),(void*)((struct _tuple6**)Cyc__genarr_512 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_492;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_493;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_494;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep;static char _tmp30D[15]="SwitchC_clause";
static struct _tagged_arr Cyc__genname_499={_tmp30D,_tmp30D,_tmp30D + 15};static char
_tmp30E[9]="cnst_exp";static struct _tuple5 Cyc__gentuple_495={offsetof(struct Cyc_Absyn_SwitchC_clause,cnst_exp),{
_tmp30E,_tmp30E,_tmp30E + 9},(void*)& Cyc__genrep_77};static char _tmp30F[5]="body";
static struct _tuple5 Cyc__gentuple_496={offsetof(struct Cyc_Absyn_SwitchC_clause,body),{
_tmp30F,_tmp30F,_tmp30F + 5},(void*)& Cyc__genrep_161};static char _tmp310[4]="loc";
static struct _tuple5 Cyc__gentuple_497={offsetof(struct Cyc_Absyn_SwitchC_clause,loc),{
_tmp310,_tmp310,_tmp310 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_498[
3]={& Cyc__gentuple_495,& Cyc__gentuple_496,& Cyc__gentuple_497};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep={3,(struct _tagged_arr*)& Cyc__genname_499,
sizeof(struct Cyc_Absyn_SwitchC_clause),{(void*)((struct _tuple5**)Cyc__genarr_498),(
void*)((struct _tuple5**)Cyc__genarr_498),(void*)((struct _tuple5**)Cyc__genarr_498
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_494={1,1,(void*)((void*)&
Cyc_struct_Absyn_SwitchC_clause_rep)};static char _tmp313[5]="List";static struct
_tagged_arr Cyc__genname_503={_tmp313,_tmp313,_tmp313 + 5};static char _tmp314[3]="hd";
static struct _tuple5 Cyc__gentuple_500={offsetof(struct Cyc_List_List,hd),{_tmp314,
_tmp314,_tmp314 + 3},(void*)& Cyc__genrep_494};static char _tmp315[3]="tl";static
struct _tuple5 Cyc__gentuple_501={offsetof(struct Cyc_List_List,tl),{_tmp315,
_tmp315,_tmp315 + 3},(void*)& Cyc__genrep_493};static struct _tuple5*Cyc__genarr_502[
2]={& Cyc__gentuple_500,& Cyc__gentuple_501};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_503,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_502),(void*)((struct _tuple5**)Cyc__genarr_502),(void*)((
struct _tuple5**)Cyc__genarr_502 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_493={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)};static
struct _tuple6 Cyc__gentuple_504={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_505={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_493};
static struct _tuple6*Cyc__genarr_507[3]={& Cyc__gentuple_504,& Cyc__gentuple_505,&
Cyc__gentuple_506};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_492={4,
sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_507),(void*)((
struct _tuple6**)Cyc__genarr_507),(void*)((struct _tuple6**)Cyc__genarr_507 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_481;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_483;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp319[5]="List";static struct _tagged_arr Cyc__genname_487={_tmp319,
_tmp319,_tmp319 + 5};static char _tmp31A[3]="hd";static struct _tuple5 Cyc__gentuple_484={
offsetof(struct Cyc_List_List,hd),{_tmp31A,_tmp31A,_tmp31A + 3},(void*)& Cyc__genrep_81};
static char _tmp31B[3]="tl";static struct _tuple5 Cyc__gentuple_485={offsetof(struct
Cyc_List_List,tl),{_tmp31B,_tmp31B,_tmp31B + 3},(void*)& Cyc__genrep_483};static
struct _tuple5*Cyc__genarr_486[2]={& Cyc__gentuple_484,& Cyc__gentuple_485};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_487,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_486),(void*)((struct _tuple5**)Cyc__genarr_486),(void*)((
struct _tuple5**)Cyc__genarr_486 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_483={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_482;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_482={1,1,(void*)((
void*)& Cyc__genrep_229)};struct _tuple55{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_488={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_489={
offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_483};static struct _tuple6 Cyc__gentuple_490={
offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_482};static struct _tuple6*Cyc__genarr_491[
3]={& Cyc__gentuple_488,& Cyc__gentuple_489,& Cyc__gentuple_490};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_481={4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_491),(
void*)((struct _tuple6**)Cyc__genarr_491),(void*)((struct _tuple6**)Cyc__genarr_491
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_476;struct _tuple56{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_477={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_478={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_479={offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_161};
static struct _tuple6*Cyc__genarr_480[3]={& Cyc__gentuple_477,& Cyc__gentuple_478,&
Cyc__gentuple_479};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_476={4,
sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_480),(void*)((
struct _tuple6**)Cyc__genarr_480),(void*)((struct _tuple6**)Cyc__genarr_480 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_472;static struct _tuple6 Cyc__gentuple_473={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_474={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_161};static struct _tuple6*Cyc__genarr_475[
2]={& Cyc__gentuple_473,& Cyc__gentuple_474};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_472={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_475),(
void*)((struct _tuple6**)Cyc__genarr_475),(void*)((struct _tuple6**)Cyc__genarr_475
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_467;static struct _tuple6
Cyc__gentuple_468={offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_469={offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_470={offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_161};
static struct _tuple6*Cyc__genarr_471[3]={& Cyc__gentuple_468,& Cyc__gentuple_469,&
Cyc__gentuple_470};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_467={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_471),(void*)((
struct _tuple6**)Cyc__genarr_471),(void*)((struct _tuple6**)Cyc__genarr_471 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_462;struct _tuple57{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_463={
offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_464={
offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_161};static struct _tuple6 Cyc__gentuple_465={
offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_170};static struct _tuple6*Cyc__genarr_466[
3]={& Cyc__gentuple_463,& Cyc__gentuple_464,& Cyc__gentuple_465};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_462={4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_466),(
void*)((struct _tuple6**)Cyc__genarr_466),(void*)((struct _tuple6**)Cyc__genarr_466
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_227;struct _tuple58{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_458={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_459={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_161};
static struct _tuple6 Cyc__gentuple_460={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_228};
static struct _tuple6*Cyc__genarr_461[3]={& Cyc__gentuple_458,& Cyc__gentuple_459,&
Cyc__gentuple_460};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_227={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_461),(void*)((
struct _tuple6**)Cyc__genarr_461),(void*)((struct _tuple6**)Cyc__genarr_461 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_183;struct _tuple59{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_221={offsetof(struct _tuple59,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_222={offsetof(struct _tuple59,f2),(
void*)& Cyc__genrep_184};static struct _tuple6 Cyc__gentuple_223={offsetof(struct
_tuple59,f3),(void*)& Cyc__genrep_132};static struct _tuple6 Cyc__gentuple_224={
offsetof(struct _tuple59,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_225={offsetof(struct _tuple59,f5),(void*)& Cyc__genrep_161};static
struct _tuple6*Cyc__genarr_226[5]={& Cyc__gentuple_221,& Cyc__gentuple_222,& Cyc__gentuple_223,&
Cyc__gentuple_224,& Cyc__gentuple_225};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_183={
4,sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_226),(void*)((
struct _tuple6**)Cyc__genarr_226),(void*)((struct _tuple6**)Cyc__genarr_226 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_169;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_forarray_info_t_rep;static char _tmp326[13]="ForArrayInfo";static struct
_tagged_arr Cyc__genname_179={_tmp326,_tmp326,_tmp326 + 13};static char _tmp327[6]="defns";
static struct _tuple5 Cyc__gentuple_174={offsetof(struct Cyc_Absyn_ForArrayInfo,defns),{
_tmp327,_tmp327,_tmp327 + 6},(void*)& Cyc__genrep_131};static char _tmp328[10]="condition";
static struct _tuple5 Cyc__gentuple_175={offsetof(struct Cyc_Absyn_ForArrayInfo,condition),{
_tmp328,_tmp328,_tmp328 + 10},(void*)& Cyc__genrep_170};static char _tmp329[6]="delta";
static struct _tuple5 Cyc__gentuple_176={offsetof(struct Cyc_Absyn_ForArrayInfo,delta),{
_tmp329,_tmp329,_tmp329 + 6},(void*)& Cyc__genrep_170};static char _tmp32A[5]="body";
static struct _tuple5 Cyc__gentuple_177={offsetof(struct Cyc_Absyn_ForArrayInfo,body),{
_tmp32A,_tmp32A,_tmp32A + 5},(void*)& Cyc__genrep_161};static struct _tuple5*Cyc__genarr_178[
4]={& Cyc__gentuple_174,& Cyc__gentuple_175,& Cyc__gentuple_176,& Cyc__gentuple_177};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_forarray_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_179,sizeof(struct Cyc_Absyn_ForArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_178),(void*)((struct _tuple5**)Cyc__genarr_178),(void*)((
struct _tuple5**)Cyc__genarr_178 + 4)}};struct _tuple60{unsigned int f1;struct Cyc_Absyn_ForArrayInfo
f2;};static struct _tuple6 Cyc__gentuple_180={offsetof(struct _tuple60,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_181={offsetof(struct _tuple60,f2),(
void*)& Cyc_Absyn_forarray_info_t_rep};static struct _tuple6*Cyc__genarr_182[2]={&
Cyc__gentuple_180,& Cyc__gentuple_181};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_169={
4,sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_182),(void*)((
struct _tuple6**)Cyc__genarr_182),(void*)((struct _tuple6**)Cyc__genarr_182 + 2)}};
static char _tmp32D[7]="Skip_s";static struct _tuple7 Cyc__gentuple_167={0,{_tmp32D,
_tmp32D,_tmp32D + 7}};static struct _tuple7*Cyc__genarr_168[1]={& Cyc__gentuple_167};
static char _tmp32E[6]="Exp_s";static struct _tuple5 Cyc__gentuple_550={0,{_tmp32E,
_tmp32E,_tmp32E + 6},(void*)& Cyc__genrep_80};static char _tmp32F[6]="Seq_s";static
struct _tuple5 Cyc__gentuple_551={1,{_tmp32F,_tmp32F,_tmp32F + 6},(void*)& Cyc__genrep_545};
static char _tmp330[9]="Return_s";static struct _tuple5 Cyc__gentuple_552={2,{_tmp330,
_tmp330,_tmp330 + 9},(void*)& Cyc__genrep_541};static char _tmp331[13]="IfThenElse_s";
static struct _tuple5 Cyc__gentuple_553={3,{_tmp331,_tmp331,_tmp331 + 13},(void*)&
Cyc__genrep_535};static char _tmp332[8]="While_s";static struct _tuple5 Cyc__gentuple_554={
4,{_tmp332,_tmp332,_tmp332 + 8},(void*)& Cyc__genrep_530};static char _tmp333[8]="Break_s";
static struct _tuple5 Cyc__gentuple_555={5,{_tmp333,_tmp333,_tmp333 + 8},(void*)& Cyc__genrep_526};
static char _tmp334[11]="Continue_s";static struct _tuple5 Cyc__gentuple_556={6,{
_tmp334,_tmp334,_tmp334 + 11},(void*)& Cyc__genrep_526};static char _tmp335[7]="Goto_s";
static struct _tuple5 Cyc__gentuple_557={7,{_tmp335,_tmp335,_tmp335 + 7},(void*)& Cyc__genrep_520};
static char _tmp336[6]="For_s";static struct _tuple5 Cyc__gentuple_558={8,{_tmp336,
_tmp336,_tmp336 + 6},(void*)& Cyc__genrep_513};static char _tmp337[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_559={9,{_tmp337,_tmp337,_tmp337 + 9},(void*)& Cyc__genrep_508};
static char _tmp338[10]="SwitchC_s";static struct _tuple5 Cyc__gentuple_560={10,{
_tmp338,_tmp338,_tmp338 + 10},(void*)& Cyc__genrep_492};static char _tmp339[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_561={11,{_tmp339,_tmp339,_tmp339 + 11},(void*)&
Cyc__genrep_481};static char _tmp33A[7]="Decl_s";static struct _tuple5 Cyc__gentuple_562={
12,{_tmp33A,_tmp33A,_tmp33A + 7},(void*)& Cyc__genrep_476};static char _tmp33B[6]="Cut_s";
static struct _tuple5 Cyc__gentuple_563={13,{_tmp33B,_tmp33B,_tmp33B + 6},(void*)&
Cyc__genrep_472};static char _tmp33C[9]="Splice_s";static struct _tuple5 Cyc__gentuple_564={
14,{_tmp33C,_tmp33C,_tmp33C + 9},(void*)& Cyc__genrep_472};static char _tmp33D[8]="Label_s";
static struct _tuple5 Cyc__gentuple_565={15,{_tmp33D,_tmp33D,_tmp33D + 8},(void*)&
Cyc__genrep_467};static char _tmp33E[5]="Do_s";static struct _tuple5 Cyc__gentuple_566={
16,{_tmp33E,_tmp33E,_tmp33E + 5},(void*)& Cyc__genrep_462};static char _tmp33F[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_567={17,{_tmp33F,_tmp33F,_tmp33F + 11},(void*)&
Cyc__genrep_227};static char _tmp340[9]="Region_s";static struct _tuple5 Cyc__gentuple_568={
18,{_tmp340,_tmp340,_tmp340 + 9},(void*)& Cyc__genrep_183};static char _tmp341[11]="ForArray_s";
static struct _tuple5 Cyc__gentuple_569={19,{_tmp341,_tmp341,_tmp341 + 11},(void*)&
Cyc__genrep_169};static char _tmp342[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_570={
20,{_tmp342,_tmp342,_tmp342 + 14},(void*)& Cyc__genrep_80};static struct _tuple5*Cyc__genarr_571[
21]={& Cyc__gentuple_550,& Cyc__gentuple_551,& Cyc__gentuple_552,& Cyc__gentuple_553,&
Cyc__gentuple_554,& Cyc__gentuple_555,& Cyc__gentuple_556,& Cyc__gentuple_557,& Cyc__gentuple_558,&
Cyc__gentuple_559,& Cyc__gentuple_560,& Cyc__gentuple_561,& Cyc__gentuple_562,& Cyc__gentuple_563,&
Cyc__gentuple_564,& Cyc__gentuple_565,& Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568,&
Cyc__gentuple_569,& Cyc__gentuple_570};static char _tmp344[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_stmt_t_rep={5,{_tmp344,_tmp344,_tmp344 + 9},{(void*)((struct _tuple7**)
Cyc__genarr_168),(void*)((struct _tuple7**)Cyc__genarr_168),(void*)((struct
_tuple7**)Cyc__genarr_168 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_571),(void*)((
struct _tuple5**)Cyc__genarr_571),(void*)((struct _tuple5**)Cyc__genarr_571 + 21)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_162;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep;static char _tmp345[5]="List";static
struct _tagged_arr Cyc__genname_166={_tmp345,_tmp345,_tmp345 + 5};static char _tmp346[
3]="hd";static struct _tuple5 Cyc__gentuple_163={offsetof(struct Cyc_List_List,hd),{
_tmp346,_tmp346,_tmp346 + 3},(void*)& Cyc__genrep_161};static char _tmp347[3]="tl";
static struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_List_List,tl),{_tmp347,
_tmp347,_tmp347 + 3},(void*)& Cyc__genrep_162};static struct _tuple5*Cyc__genarr_165[
2]={& Cyc__gentuple_163,& Cyc__gentuple_164};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_166,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_165),(void*)((struct _tuple5**)Cyc__genarr_165),(void*)((
struct _tuple5**)Cyc__genarr_165 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_162={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_78[0]={};static char
_tmp34B[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,{_tmp34B,_tmp34B,_tmp34B + 11},{(void*)((struct _tuple8**)Cyc__genarr_78),(void*)((
struct _tuple8**)Cyc__genarr_78),(void*)((struct _tuple8**)Cyc__genarr_78 + 0)}};
static char _tmp34C[5]="Stmt";static struct _tagged_arr Cyc__genname_578={_tmp34C,
_tmp34C,_tmp34C + 5};static char _tmp34D[2]="r";static struct _tuple5 Cyc__gentuple_572={
offsetof(struct Cyc_Absyn_Stmt,r),{_tmp34D,_tmp34D,_tmp34D + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp34E[4]="loc";static struct _tuple5 Cyc__gentuple_573={offsetof(struct
Cyc_Absyn_Stmt,loc),{_tmp34E,_tmp34E,_tmp34E + 4},(void*)& Cyc__genrep_2};static
char _tmp34F[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_574={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),{_tmp34F,_tmp34F,_tmp34F + 16},(
void*)& Cyc__genrep_162};static char _tmp350[10]="try_depth";static struct _tuple5 Cyc__gentuple_575={
offsetof(struct Cyc_Absyn_Stmt,try_depth),{_tmp350,_tmp350,_tmp350 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp351[6]="annot";static struct _tuple5 Cyc__gentuple_576={
offsetof(struct Cyc_Absyn_Stmt,annot),{_tmp351,_tmp351,_tmp351 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_577[5]={& Cyc__gentuple_572,& Cyc__gentuple_573,&
Cyc__gentuple_574,& Cyc__gentuple_575,& Cyc__gentuple_576};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)& Cyc__genname_578,sizeof(struct
Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_577),(void*)((struct
_tuple5**)Cyc__genarr_577),(void*)((struct _tuple5**)Cyc__genarr_577 + 5)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_161={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
static char _tmp354[7]="Fndecl";static struct _tagged_arr Cyc__genname_616={_tmp354,
_tmp354,_tmp354 + 7};static char _tmp355[3]="sc";static struct _tuple5 Cyc__gentuple_601={
offsetof(struct Cyc_Absyn_Fndecl,sc),{_tmp355,_tmp355,_tmp355 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp356[10]="is_inline";static struct _tuple5 Cyc__gentuple_602={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),{_tmp356,_tmp356,_tmp356 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp357[5]="name";static struct _tuple5 Cyc__gentuple_603={
offsetof(struct Cyc_Absyn_Fndecl,name),{_tmp357,_tmp357,_tmp357 + 5},(void*)& Cyc__genrep_10};
static char _tmp358[4]="tvs";static struct _tuple5 Cyc__gentuple_604={offsetof(struct
Cyc_Absyn_Fndecl,tvs),{_tmp358,_tmp358,_tmp358 + 4},(void*)& Cyc__genrep_296};
static char _tmp359[7]="effect";static struct _tuple5 Cyc__gentuple_605={offsetof(
struct Cyc_Absyn_Fndecl,effect),{_tmp359,_tmp359,_tmp359 + 7},(void*)& Cyc__genrep_43};
static char _tmp35A[9]="ret_type";static struct _tuple5 Cyc__gentuple_606={offsetof(
struct Cyc_Absyn_Fndecl,ret_type),{_tmp35A,_tmp35A,_tmp35A + 9},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp35B[5]="args";static struct _tuple5 Cyc__gentuple_607={
offsetof(struct Cyc_Absyn_Fndecl,args),{_tmp35B,_tmp35B,_tmp35B + 5},(void*)& Cyc__genrep_590};
static char _tmp35C[10]="c_varargs";static struct _tuple5 Cyc__gentuple_608={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),{_tmp35C,_tmp35C,_tmp35C + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp35D[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_609={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),{_tmp35D,_tmp35D,
_tmp35D + 12},(void*)& Cyc__genrep_579};static char _tmp35E[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_610={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),{_tmp35E,
_tmp35E,_tmp35E + 7},(void*)& Cyc__genrep_355};static char _tmp35F[5]="body";static
struct _tuple5 Cyc__gentuple_611={offsetof(struct Cyc_Absyn_Fndecl,body),{_tmp35F,
_tmp35F,_tmp35F + 5},(void*)& Cyc__genrep_161};static char _tmp360[11]="cached_typ";
static struct _tuple5 Cyc__gentuple_612={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),{
_tmp360,_tmp360,_tmp360 + 11},(void*)& Cyc__genrep_43};static char _tmp361[15]="param_vardecls";
static struct _tuple5 Cyc__gentuple_613={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),{
_tmp361,_tmp361,_tmp361 + 15},(void*)& Cyc__genrep_130};static char _tmp362[11]="attributes";
static struct _tuple5 Cyc__gentuple_614={offsetof(struct Cyc_Absyn_Fndecl,attributes),{
_tmp362,_tmp362,_tmp362 + 11},(void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_615[
14]={& Cyc__gentuple_601,& Cyc__gentuple_602,& Cyc__gentuple_603,& Cyc__gentuple_604,&
Cyc__gentuple_605,& Cyc__gentuple_606,& Cyc__gentuple_607,& Cyc__gentuple_608,& Cyc__gentuple_609,&
Cyc__gentuple_610,& Cyc__gentuple_611,& Cyc__gentuple_612,& Cyc__gentuple_613,& Cyc__gentuple_614};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)&
Cyc__genname_616,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_615),(
void*)((struct _tuple5**)Cyc__genarr_615),(void*)((struct _tuple5**)Cyc__genarr_615
+ 14)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_86={1,1,(void*)((void*)&
Cyc_struct_Absyn_Fndecl_rep)};struct _tuple61{unsigned int f1;struct Cyc_Absyn_Fndecl*
f2;};static struct _tuple6 Cyc__gentuple_617={offsetof(struct _tuple61,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_618={offsetof(struct _tuple61,f2),(
void*)& Cyc__genrep_86};static struct _tuple6*Cyc__genarr_619[2]={& Cyc__gentuple_617,&
Cyc__gentuple_618};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_619),(void*)((struct
_tuple6**)Cyc__genarr_619),(void*)((struct _tuple6**)Cyc__genarr_619 + 2)}};static
char _tmp366[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_826={0,{_tmp366,
_tmp366,_tmp366 + 13}};static struct _tuple7*Cyc__genarr_827[1]={& Cyc__gentuple_826};
static char _tmp367[9]="Global_b";static struct _tuple5 Cyc__gentuple_828={0,{_tmp367,
_tmp367,_tmp367 + 9},(void*)& Cyc__genrep_402};static char _tmp368[10]="Funname_b";
static struct _tuple5 Cyc__gentuple_829={1,{_tmp368,_tmp368,_tmp368 + 10},(void*)&
Cyc__genrep_85};static char _tmp369[8]="Param_b";static struct _tuple5 Cyc__gentuple_830={
2,{_tmp369,_tmp369,_tmp369 + 8},(void*)& Cyc__genrep_402};static char _tmp36A[8]="Local_b";
static struct _tuple5 Cyc__gentuple_831={3,{_tmp36A,_tmp36A,_tmp36A + 8},(void*)& Cyc__genrep_402};
static char _tmp36B[6]="Pat_b";static struct _tuple5 Cyc__gentuple_832={4,{_tmp36B,
_tmp36B,_tmp36B + 6},(void*)& Cyc__genrep_402};static struct _tuple5*Cyc__genarr_833[
5]={& Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830,& Cyc__gentuple_831,&
Cyc__gentuple_832};static char _tmp36D[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,{_tmp36D,_tmp36D,_tmp36D + 8},{(void*)((struct _tuple7**)
Cyc__genarr_827),(void*)((struct _tuple7**)Cyc__genarr_827),(void*)((struct
_tuple7**)Cyc__genarr_827 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_833),(void*)((
struct _tuple5**)Cyc__genarr_833),(void*)((struct _tuple5**)Cyc__genarr_833 + 5)}};
struct _tuple62{unsigned int f1;struct _tuple0*f2;void*f3;};static struct _tuple6 Cyc__gentuple_834={
offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_835={
offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_836={
offsetof(struct _tuple62,f3),(void*)& Cyc_Absyn_binding_t_rep};static struct _tuple6*
Cyc__genarr_837[3]={& Cyc__gentuple_834,& Cyc__gentuple_835,& Cyc__gentuple_836};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_825={4,sizeof(struct _tuple62),{(
void*)((struct _tuple6**)Cyc__genarr_837),(void*)((struct _tuple6**)Cyc__genarr_837),(
void*)((struct _tuple6**)Cyc__genarr_837 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_820;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;
static char _tmp36F[5]="Plus";static struct _tuple7 Cyc__gentuple_790={0,{_tmp36F,
_tmp36F,_tmp36F + 5}};static char _tmp370[6]="Times";static struct _tuple7 Cyc__gentuple_791={
1,{_tmp370,_tmp370,_tmp370 + 6}};static char _tmp371[6]="Minus";static struct _tuple7
Cyc__gentuple_792={2,{_tmp371,_tmp371,_tmp371 + 6}};static char _tmp372[4]="Div";
static struct _tuple7 Cyc__gentuple_793={3,{_tmp372,_tmp372,_tmp372 + 4}};static char
_tmp373[4]="Mod";static struct _tuple7 Cyc__gentuple_794={4,{_tmp373,_tmp373,
_tmp373 + 4}};static char _tmp374[3]="Eq";static struct _tuple7 Cyc__gentuple_795={5,{
_tmp374,_tmp374,_tmp374 + 3}};static char _tmp375[4]="Neq";static struct _tuple7 Cyc__gentuple_796={
6,{_tmp375,_tmp375,_tmp375 + 4}};static char _tmp376[3]="Gt";static struct _tuple7 Cyc__gentuple_797={
7,{_tmp376,_tmp376,_tmp376 + 3}};static char _tmp377[3]="Lt";static struct _tuple7 Cyc__gentuple_798={
8,{_tmp377,_tmp377,_tmp377 + 3}};static char _tmp378[4]="Gte";static struct _tuple7
Cyc__gentuple_799={9,{_tmp378,_tmp378,_tmp378 + 4}};static char _tmp379[4]="Lte";
static struct _tuple7 Cyc__gentuple_800={10,{_tmp379,_tmp379,_tmp379 + 4}};static
char _tmp37A[4]="Not";static struct _tuple7 Cyc__gentuple_801={11,{_tmp37A,_tmp37A,
_tmp37A + 4}};static char _tmp37B[7]="Bitnot";static struct _tuple7 Cyc__gentuple_802={
12,{_tmp37B,_tmp37B,_tmp37B + 7}};static char _tmp37C[7]="Bitand";static struct
_tuple7 Cyc__gentuple_803={13,{_tmp37C,_tmp37C,_tmp37C + 7}};static char _tmp37D[6]="Bitor";
static struct _tuple7 Cyc__gentuple_804={14,{_tmp37D,_tmp37D,_tmp37D + 6}};static
char _tmp37E[7]="Bitxor";static struct _tuple7 Cyc__gentuple_805={15,{_tmp37E,
_tmp37E,_tmp37E + 7}};static char _tmp37F[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_806={
16,{_tmp37F,_tmp37F,_tmp37F + 10}};static char _tmp380[11]="Bitlrshift";static
struct _tuple7 Cyc__gentuple_807={17,{_tmp380,_tmp380,_tmp380 + 11}};static char
_tmp381[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_808={18,{_tmp381,
_tmp381,_tmp381 + 11}};static char _tmp382[5]="Size";static struct _tuple7 Cyc__gentuple_809={
19,{_tmp382,_tmp382,_tmp382 + 5}};static struct _tuple7*Cyc__genarr_810[20]={& Cyc__gentuple_790,&
Cyc__gentuple_791,& Cyc__gentuple_792,& Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,&
Cyc__gentuple_796,& Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,&
Cyc__gentuple_801,& Cyc__gentuple_802,& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,&
Cyc__gentuple_806,& Cyc__gentuple_807,& Cyc__gentuple_808,& Cyc__gentuple_809};
static struct _tuple5*Cyc__genarr_811[0]={};static char _tmp384[7]="Primop";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,{_tmp384,_tmp384,_tmp384 + 7},{(
void*)((struct _tuple7**)Cyc__genarr_810),(void*)((struct _tuple7**)Cyc__genarr_810),(
void*)((struct _tuple7**)Cyc__genarr_810 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_811),(
void*)((struct _tuple5**)Cyc__genarr_811),(void*)((struct _tuple5**)Cyc__genarr_811
+ 0)}};struct _tuple63{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_821={offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_822={offsetof(struct _tuple63,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_823={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_483};
static struct _tuple6*Cyc__genarr_824[3]={& Cyc__gentuple_821,& Cyc__gentuple_822,&
Cyc__gentuple_823};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_820={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_824),(void*)((
struct _tuple6**)Cyc__genarr_824),(void*)((struct _tuple6**)Cyc__genarr_824 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_788;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_789;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp386[4]="Opt";static struct _tagged_arr Cyc__genname_814={_tmp386,
_tmp386,_tmp386 + 4};static char _tmp387[2]="v";static struct _tuple5 Cyc__gentuple_812={
offsetof(struct Cyc_Core_Opt,v),{_tmp387,_tmp387,_tmp387 + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_813[1]={& Cyc__gentuple_812};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_814,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_813),(void*)((
struct _tuple5**)Cyc__genarr_813),(void*)((struct _tuple5**)Cyc__genarr_813 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_789={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple64{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_815={offsetof(struct
_tuple64,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_816={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_817={
offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_789};static struct _tuple6 Cyc__gentuple_818={
offsetof(struct _tuple64,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_819[
4]={& Cyc__gentuple_815,& Cyc__gentuple_816,& Cyc__gentuple_817,& Cyc__gentuple_818};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_788={4,sizeof(struct _tuple64),{(
void*)((struct _tuple6**)Cyc__genarr_819),(void*)((struct _tuple6**)Cyc__genarr_819),(
void*)((struct _tuple6**)Cyc__genarr_819 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_777;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp38B[7]="PreInc";static struct _tuple7 Cyc__gentuple_778={0,{_tmp38B,
_tmp38B,_tmp38B + 7}};static char _tmp38C[8]="PostInc";static struct _tuple7 Cyc__gentuple_779={
1,{_tmp38C,_tmp38C,_tmp38C + 8}};static char _tmp38D[7]="PreDec";static struct
_tuple7 Cyc__gentuple_780={2,{_tmp38D,_tmp38D,_tmp38D + 7}};static char _tmp38E[8]="PostDec";
static struct _tuple7 Cyc__gentuple_781={3,{_tmp38E,_tmp38E,_tmp38E + 8}};static
struct _tuple7*Cyc__genarr_782[4]={& Cyc__gentuple_778,& Cyc__gentuple_779,& Cyc__gentuple_780,&
Cyc__gentuple_781};static struct _tuple5*Cyc__genarr_783[0]={};static char _tmp390[
12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={5,{
_tmp390,_tmp390,_tmp390 + 12},{(void*)((struct _tuple7**)Cyc__genarr_782),(void*)((
struct _tuple7**)Cyc__genarr_782),(void*)((struct _tuple7**)Cyc__genarr_782 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_783),(void*)((struct _tuple5**)Cyc__genarr_783),(
void*)((struct _tuple5**)Cyc__genarr_783 + 0)}};struct _tuple65{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_784={offsetof(
struct _tuple65,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_785={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_786={
offsetof(struct _tuple65,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_787[3]={& Cyc__gentuple_784,& Cyc__gentuple_785,& Cyc__gentuple_786};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_777={4,sizeof(struct _tuple65),{(
void*)((struct _tuple6**)Cyc__genarr_787),(void*)((struct _tuple6**)Cyc__genarr_787),(
void*)((struct _tuple6**)Cyc__genarr_787 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_771;struct _tuple66{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_772={offsetof(
struct _tuple66,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_773={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_774={
offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_775={
offsetof(struct _tuple66,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_776[
4]={& Cyc__gentuple_772,& Cyc__gentuple_773,& Cyc__gentuple_774,& Cyc__gentuple_775};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_771={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_776),(void*)((struct _tuple6**)Cyc__genarr_776),(
void*)((struct _tuple6**)Cyc__genarr_776 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_710;struct _tuple67{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple6 Cyc__gentuple_711={offsetof(struct _tuple67,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_712={offsetof(struct _tuple67,f2),(
void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_713={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_714[3]={&
Cyc__gentuple_711,& Cyc__gentuple_712,& Cyc__gentuple_713};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_710={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_714),(
void*)((struct _tuple6**)Cyc__genarr_714),(void*)((struct _tuple6**)Cyc__genarr_714
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_766;static struct _tuple6
Cyc__gentuple_767={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_768={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_769={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_483};
static struct _tuple6*Cyc__genarr_770[3]={& Cyc__gentuple_767,& Cyc__gentuple_768,&
Cyc__gentuple_769};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_766={4,
sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_770),(void*)((
struct _tuple6**)Cyc__genarr_770),(void*)((struct _tuple6**)Cyc__genarr_770 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_753;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_754;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_755;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_755={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp396[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_760={_tmp396,
_tmp396,_tmp396 + 15};static char _tmp397[12]="num_varargs";static struct _tuple5 Cyc__gentuple_756={
offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),{_tmp397,_tmp397,_tmp397 + 
12},(void*)((void*)& Cyc__genrep_62)};static char _tmp398[10]="injectors";static
struct _tuple5 Cyc__gentuple_757={offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),{
_tmp398,_tmp398,_tmp398 + 10},(void*)& Cyc__genrep_288};static char _tmp399[4]="vai";
static struct _tuple5 Cyc__gentuple_758={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),{
_tmp399,_tmp399,_tmp399 + 4},(void*)& Cyc__genrep_755};static struct _tuple5*Cyc__genarr_759[
3]={& Cyc__gentuple_756,& Cyc__gentuple_757,& Cyc__gentuple_758};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_760,
sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((struct _tuple5**)Cyc__genarr_759),(
void*)((struct _tuple5**)Cyc__genarr_759),(void*)((struct _tuple5**)Cyc__genarr_759
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_754={1,1,(void*)((void*)&
Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*f4;};static struct
_tuple6 Cyc__gentuple_761={offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_762={offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_763={offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_483};
static struct _tuple6 Cyc__gentuple_764={offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_754};
static struct _tuple6*Cyc__genarr_765[4]={& Cyc__gentuple_761,& Cyc__gentuple_762,&
Cyc__gentuple_763,& Cyc__gentuple_764};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_753={
4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_765),(void*)((
struct _tuple6**)Cyc__genarr_765),(void*)((struct _tuple6**)Cyc__genarr_765 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_748;static struct _tuple6 Cyc__gentuple_749={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_750={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_751={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_752[
3]={& Cyc__gentuple_749,& Cyc__gentuple_750,& Cyc__gentuple_751};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_748={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_752),(
void*)((struct _tuple6**)Cyc__genarr_752),(void*)((struct _tuple6**)Cyc__genarr_752
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_743;struct _tuple69{
unsigned int f1;void*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_744={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_745={
offsetof(struct _tuple69,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_746={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_81};
static struct _tuple6*Cyc__genarr_747[3]={& Cyc__gentuple_744,& Cyc__gentuple_745,&
Cyc__gentuple_746};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_743={4,
sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_747),(void*)((
struct _tuple6**)Cyc__genarr_747),(void*)((struct _tuple6**)Cyc__genarr_747 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_738;static struct _tuple6 Cyc__gentuple_739={
offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_740={
offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_741={
offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_742[
3]={& Cyc__gentuple_739,& Cyc__gentuple_740,& Cyc__gentuple_741};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_738={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_742),(
void*)((struct _tuple6**)Cyc__genarr_742),(void*)((struct _tuple6**)Cyc__genarr_742
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_48;static struct _tuple6 Cyc__gentuple_49={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_50={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_51[2]={& Cyc__gentuple_49,& Cyc__gentuple_50};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_48={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_51),(
void*)((struct _tuple6**)Cyc__genarr_51),(void*)((struct _tuple6**)Cyc__genarr_51 + 
2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_725;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_727;
struct _tuple70{unsigned int f1;unsigned int f2;};static struct _tuple6 Cyc__gentuple_728={
offsetof(struct _tuple70,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_729={
offsetof(struct _tuple70,f2),(void*)& Cyc__genrep_5};static struct _tuple6*Cyc__genarr_730[
2]={& Cyc__gentuple_728,& Cyc__gentuple_729};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_727={4,sizeof(struct _tuple70),{(void*)((struct _tuple6**)Cyc__genarr_730),(
void*)((struct _tuple6**)Cyc__genarr_730),(void*)((struct _tuple6**)Cyc__genarr_730
+ 2)}};static struct _tuple7*Cyc__genarr_726[0]={};static char _tmp3A2[12]="StructField";
static struct _tuple5 Cyc__gentuple_731={0,{_tmp3A2,_tmp3A2,_tmp3A2 + 12},(void*)&
Cyc__genrep_319};static char _tmp3A3[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_732={
1,{_tmp3A3,_tmp3A3,_tmp3A3 + 11},(void*)& Cyc__genrep_727};static struct _tuple5*Cyc__genarr_733[
2]={& Cyc__gentuple_731,& Cyc__gentuple_732};static char _tmp3A5[14]="OffsetofField";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={5,{_tmp3A5,
_tmp3A5,_tmp3A5 + 14},{(void*)((struct _tuple7**)Cyc__genarr_726),(void*)((struct
_tuple7**)Cyc__genarr_726),(void*)((struct _tuple7**)Cyc__genarr_726 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_733),(void*)((struct _tuple5**)Cyc__genarr_733),(void*)((
struct _tuple5**)Cyc__genarr_733 + 2)}};static struct _tuple6 Cyc__gentuple_734={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_735={
offsetof(struct _tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_736={offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple6*Cyc__genarr_737[3]={& Cyc__gentuple_734,& Cyc__gentuple_735,&
Cyc__gentuple_736};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_725={4,
sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_737),(void*)((
struct _tuple6**)Cyc__genarr_737),(void*)((struct _tuple6**)Cyc__genarr_737 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_720;struct _tuple71{unsigned int
f1;struct Cyc_List_List*f2;void*f3;};static struct _tuple6 Cyc__gentuple_721={
offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_722={
offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_296};static struct _tuple6 Cyc__gentuple_723={
offsetof(struct _tuple71,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_724[3]={& Cyc__gentuple_721,& Cyc__gentuple_722,& Cyc__gentuple_723};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_720={4,sizeof(struct _tuple71),{(
void*)((struct _tuple6**)Cyc__genarr_724),(void*)((struct _tuple6**)Cyc__genarr_724),(
void*)((struct _tuple6**)Cyc__genarr_724 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_715;struct _tuple72{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tagged_arr*f3;};static struct _tuple6 Cyc__gentuple_716={offsetof(struct _tuple72,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_717={offsetof(struct
_tuple72,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_718={
offsetof(struct _tuple72,f3),(void*)& Cyc__genrep_12};static struct _tuple6*Cyc__genarr_719[
3]={& Cyc__gentuple_716,& Cyc__gentuple_717,& Cyc__gentuple_718};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_715={4,sizeof(struct _tuple72),{(void*)((struct _tuple6**)Cyc__genarr_719),(
void*)((struct _tuple6**)Cyc__genarr_719),(void*)((struct _tuple6**)Cyc__genarr_719
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_706;static struct _tuple6
Cyc__gentuple_707={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_708={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_483};
static struct _tuple6*Cyc__genarr_709[2]={& Cyc__gentuple_707,& Cyc__gentuple_708};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_706={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_709),(void*)((struct _tuple6**)Cyc__genarr_709),(
void*)((struct _tuple6**)Cyc__genarr_709 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_695;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_696;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_697;static struct _tuple6 Cyc__gentuple_698={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_580};static struct _tuple6 Cyc__gentuple_699={
offsetof(struct _tuple1,f2),(void*)& Cyc__genrep_133};static struct _tuple6 Cyc__gentuple_700={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_701[3]={& Cyc__gentuple_698,& Cyc__gentuple_699,& Cyc__gentuple_700};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_697={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_701),(void*)((struct _tuple6**)Cyc__genarr_701),(
void*)((struct _tuple6**)Cyc__genarr_701 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_696={1,1,(void*)((void*)& Cyc__genrep_697)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_621;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_622;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_623;static struct _tuple6 Cyc__gentuple_624={offsetof(struct _tuple12,f1),(
void*)& Cyc__genrep_317};static struct _tuple6 Cyc__gentuple_625={offsetof(struct
_tuple12,f2),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_626[2]={&
Cyc__gentuple_624,& Cyc__gentuple_625};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_623={
4,sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_626),(void*)((
struct _tuple6**)Cyc__genarr_626),(void*)((struct _tuple6**)Cyc__genarr_626 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_622={1,1,(void*)((void*)& Cyc__genrep_623)};
static char _tmp3AE[5]="List";static struct _tagged_arr Cyc__genname_630={_tmp3AE,
_tmp3AE,_tmp3AE + 5};static char _tmp3AF[3]="hd";static struct _tuple5 Cyc__gentuple_627={
offsetof(struct Cyc_List_List,hd),{_tmp3AF,_tmp3AF,_tmp3AF + 3},(void*)& Cyc__genrep_622};
static char _tmp3B0[3]="tl";static struct _tuple5 Cyc__gentuple_628={offsetof(struct
Cyc_List_List,tl),{_tmp3B0,_tmp3B0,_tmp3B0 + 3},(void*)& Cyc__genrep_621};static
struct _tuple5*Cyc__genarr_629[2]={& Cyc__gentuple_627,& Cyc__gentuple_628};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_630,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_629),(void*)((struct _tuple5**)Cyc__genarr_629),(void*)((
struct _tuple5**)Cyc__genarr_629 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_621={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple73{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_702={offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_703={offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_696};
static struct _tuple6 Cyc__gentuple_704={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_621};
static struct _tuple6*Cyc__genarr_705[3]={& Cyc__gentuple_702,& Cyc__gentuple_703,&
Cyc__gentuple_704};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_695={4,
sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_705),(void*)((
struct _tuple6**)Cyc__genarr_705),(void*)((struct _tuple6**)Cyc__genarr_705 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_691;static struct _tuple6 Cyc__gentuple_692={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_693={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_621};static struct _tuple6*Cyc__genarr_694[
2]={& Cyc__gentuple_692,& Cyc__gentuple_693};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_691={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_694),(
void*)((struct _tuple6**)Cyc__genarr_694),(void*)((struct _tuple6**)Cyc__genarr_694
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_684;struct _tuple74{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_685={offsetof(struct _tuple74,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_686={offsetof(struct
_tuple74,f2),(void*)& Cyc__genrep_132};static struct _tuple6 Cyc__gentuple_687={
offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_688={
offsetof(struct _tuple74,f4),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_689={
offsetof(struct _tuple74,f5),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_690[5]={& Cyc__gentuple_685,& Cyc__gentuple_686,& Cyc__gentuple_687,&
Cyc__gentuple_688,& Cyc__gentuple_689};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_684={
4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_690),(void*)((
struct _tuple6**)Cyc__genarr_690),(void*)((struct _tuple6**)Cyc__genarr_690 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_676;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_677;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_677={1,1,(void*)((
void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct _tuple75{unsigned int f1;struct
_tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Aggrdecl*
f5;};static struct _tuple6 Cyc__gentuple_678={offsetof(struct _tuple75,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_679={offsetof(struct _tuple75,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_680={offsetof(struct
_tuple75,f3),(void*)& Cyc__genrep_53};static struct _tuple6 Cyc__gentuple_681={
offsetof(struct _tuple75,f4),(void*)& Cyc__genrep_621};static struct _tuple6 Cyc__gentuple_682={
offsetof(struct _tuple75,f5),(void*)& Cyc__genrep_677};static struct _tuple6*Cyc__genarr_683[
5]={& Cyc__gentuple_678,& Cyc__gentuple_679,& Cyc__gentuple_680,& Cyc__gentuple_681,&
Cyc__gentuple_682};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_676={4,
sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_683),(void*)((
struct _tuple6**)Cyc__genarr_683),(void*)((struct _tuple6**)Cyc__genarr_683 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_671;static struct _tuple6 Cyc__gentuple_672={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_673={
offsetof(struct _tuple63,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_674={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_621};
static struct _tuple6*Cyc__genarr_675[3]={& Cyc__gentuple_672,& Cyc__gentuple_673,&
Cyc__gentuple_674};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_671={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_675),(void*)((
struct _tuple6**)Cyc__genarr_675),(void*)((struct _tuple6**)Cyc__genarr_675 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_665;struct _tuple76{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_666={offsetof(struct _tuple76,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple76,f2),(
void*)& Cyc__genrep_483};static struct _tuple6 Cyc__gentuple_668={offsetof(struct
_tuple76,f3),(void*)((void*)& Cyc__genrep_286)};static struct _tuple6 Cyc__gentuple_669={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_269};static struct _tuple6*Cyc__genarr_670[
4]={& Cyc__gentuple_666,& Cyc__gentuple_667,& Cyc__gentuple_668,& Cyc__gentuple_669};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_665={4,sizeof(struct _tuple76),{(
void*)((struct _tuple6**)Cyc__genarr_670),(void*)((struct _tuple6**)Cyc__genarr_670),(
void*)((struct _tuple6**)Cyc__genarr_670 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_658;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_659;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_659={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_652;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_652={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple77{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_660={offsetof(struct _tuple77,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_661={offsetof(struct _tuple77,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_662={offsetof(struct
_tuple77,f3),(void*)& Cyc__genrep_659};static struct _tuple6 Cyc__gentuple_663={
offsetof(struct _tuple77,f4),(void*)& Cyc__genrep_652};static struct _tuple6*Cyc__genarr_664[
4]={& Cyc__gentuple_660,& Cyc__gentuple_661,& Cyc__gentuple_662,& Cyc__gentuple_663};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_658={4,sizeof(struct _tuple77),{(
void*)((struct _tuple6**)Cyc__genarr_664),(void*)((struct _tuple6**)Cyc__genarr_664),(
void*)((struct _tuple6**)Cyc__genarr_664 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_651;struct _tuple78{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_653={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_654={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_655={offsetof(struct
_tuple78,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_656={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_652};static struct _tuple6*Cyc__genarr_657[
4]={& Cyc__gentuple_653,& Cyc__gentuple_654,& Cyc__gentuple_655,& Cyc__gentuple_656};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_651={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_657),(void*)((struct _tuple6**)Cyc__genarr_657),(
void*)((struct _tuple6**)Cyc__genarr_657 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_639;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_640;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_640={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3BF[11]="MallocInfo";
static struct _tagged_arr Cyc__genname_647={_tmp3BF,_tmp3BF,_tmp3BF + 11};static char
_tmp3C0[10]="is_calloc";static struct _tuple5 Cyc__gentuple_641={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),{
_tmp3C0,_tmp3C0,_tmp3C0 + 10},(void*)((void*)& Cyc__genrep_62)};static char _tmp3C1[
4]="rgn";static struct _tuple5 Cyc__gentuple_642={offsetof(struct Cyc_Absyn_MallocInfo,rgn),{
_tmp3C1,_tmp3C1,_tmp3C1 + 4},(void*)& Cyc__genrep_77};static char _tmp3C2[9]="elt_type";
static struct _tuple5 Cyc__gentuple_643={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),{
_tmp3C2,_tmp3C2,_tmp3C2 + 9},(void*)& Cyc__genrep_640};static char _tmp3C3[9]="num_elts";
static struct _tuple5 Cyc__gentuple_644={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),{
_tmp3C3,_tmp3C3,_tmp3C3 + 9},(void*)& Cyc__genrep_81};static char _tmp3C4[11]="fat_result";
static struct _tuple5 Cyc__gentuple_645={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),{
_tmp3C4,_tmp3C4,_tmp3C4 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_646[5]={& Cyc__gentuple_641,& Cyc__gentuple_642,& Cyc__gentuple_643,&
Cyc__gentuple_644,& Cyc__gentuple_645};struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_647,sizeof(struct Cyc_Absyn_MallocInfo),{(
void*)((struct _tuple5**)Cyc__genarr_646),(void*)((struct _tuple5**)Cyc__genarr_646),(
void*)((struct _tuple5**)Cyc__genarr_646 + 5)}};struct _tuple79{unsigned int f1;
struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6 Cyc__gentuple_648={offsetof(
struct _tuple79,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_649={
offsetof(struct _tuple79,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};static struct
_tuple6*Cyc__genarr_650[2]={& Cyc__gentuple_648,& Cyc__gentuple_649};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_639={4,sizeof(struct _tuple79),{(void*)((
struct _tuple6**)Cyc__genarr_650),(void*)((struct _tuple6**)Cyc__genarr_650),(void*)((
struct _tuple6**)Cyc__genarr_650 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_620;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_631;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;static char _tmp3C7[4]="Opt";static
struct _tagged_arr Cyc__genname_634={_tmp3C7,_tmp3C7,_tmp3C7 + 4};static char _tmp3C8[
2]="v";static struct _tuple5 Cyc__gentuple_632={offsetof(struct Cyc_Core_Opt,v),{
_tmp3C8,_tmp3C8,_tmp3C8 + 2},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_633[
1]={& Cyc__gentuple_632};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_634,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_633),(void*)((struct _tuple5**)Cyc__genarr_633),(void*)((
struct _tuple5**)Cyc__genarr_633 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_631={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};struct
_tuple80{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_635={offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_636={offsetof(struct _tuple80,f2),(void*)& Cyc__genrep_631};
static struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple80,f3),(void*)& Cyc__genrep_621};
static struct _tuple6*Cyc__genarr_638[3]={& Cyc__gentuple_635,& Cyc__gentuple_636,&
Cyc__gentuple_637};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_620={4,
sizeof(struct _tuple80),{(void*)((struct _tuple6**)Cyc__genarr_638),(void*)((
struct _tuple6**)Cyc__genarr_638),(void*)((struct _tuple6**)Cyc__genarr_638 + 3)}};
static struct _tuple7*Cyc__genarr_79[0]={};static char _tmp3CC[8]="Const_e";static
struct _tuple5 Cyc__gentuple_868={0,{_tmp3CC,_tmp3CC,_tmp3CC + 8},(void*)& Cyc__genrep_838};
static char _tmp3CD[6]="Var_e";static struct _tuple5 Cyc__gentuple_869={1,{_tmp3CD,
_tmp3CD,_tmp3CD + 6},(void*)& Cyc__genrep_825};static char _tmp3CE[12]="UnknownId_e";
static struct _tuple5 Cyc__gentuple_870={2,{_tmp3CE,_tmp3CE,_tmp3CE + 12},(void*)&
Cyc__genrep_244};static char _tmp3CF[9]="Primop_e";static struct _tuple5 Cyc__gentuple_871={
3,{_tmp3CF,_tmp3CF,_tmp3CF + 9},(void*)& Cyc__genrep_820};static char _tmp3D0[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_872={4,{_tmp3D0,_tmp3D0,_tmp3D0 + 11},(void*)&
Cyc__genrep_788};static char _tmp3D1[12]="Increment_e";static struct _tuple5 Cyc__gentuple_873={
5,{_tmp3D1,_tmp3D1,_tmp3D1 + 12},(void*)& Cyc__genrep_777};static char _tmp3D2[14]="Conditional_e";
static struct _tuple5 Cyc__gentuple_874={6,{_tmp3D2,_tmp3D2,_tmp3D2 + 14},(void*)&
Cyc__genrep_771};static char _tmp3D3[9]="SeqExp_e";static struct _tuple5 Cyc__gentuple_875={
7,{_tmp3D3,_tmp3D3,_tmp3D3 + 9},(void*)& Cyc__genrep_710};static char _tmp3D4[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_876={8,{_tmp3D4,_tmp3D4,_tmp3D4 + 14},(void*)&
Cyc__genrep_766};static char _tmp3D5[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_877={
9,{_tmp3D5,_tmp3D5,_tmp3D5 + 9},(void*)& Cyc__genrep_753};static char _tmp3D6[8]="Throw_e";
static struct _tuple5 Cyc__gentuple_878={10,{_tmp3D6,_tmp3D6,_tmp3D6 + 8},(void*)&
Cyc__genrep_80};static char _tmp3D7[16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_879={
11,{_tmp3D7,_tmp3D7,_tmp3D7 + 16},(void*)& Cyc__genrep_80};static char _tmp3D8[14]="Instantiate_e";
static struct _tuple5 Cyc__gentuple_880={12,{_tmp3D8,_tmp3D8,_tmp3D8 + 14},(void*)&
Cyc__genrep_748};static char _tmp3D9[7]="Cast_e";static struct _tuple5 Cyc__gentuple_881={
13,{_tmp3D9,_tmp3D9,_tmp3D9 + 7},(void*)& Cyc__genrep_743};static char _tmp3DA[10]="Address_e";
static struct _tuple5 Cyc__gentuple_882={14,{_tmp3DA,_tmp3DA,_tmp3DA + 10},(void*)&
Cyc__genrep_80};static char _tmp3DB[6]="New_e";static struct _tuple5 Cyc__gentuple_883={
15,{_tmp3DB,_tmp3DB,_tmp3DB + 6},(void*)& Cyc__genrep_738};static char _tmp3DC[12]="Sizeoftyp_e";
static struct _tuple5 Cyc__gentuple_884={16,{_tmp3DC,_tmp3DC,_tmp3DC + 12},(void*)&
Cyc__genrep_48};static char _tmp3DD[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_885={
17,{_tmp3DD,_tmp3DD,_tmp3DD + 12},(void*)& Cyc__genrep_80};static char _tmp3DE[11]="Offsetof_e";
static struct _tuple5 Cyc__gentuple_886={18,{_tmp3DE,_tmp3DE,_tmp3DE + 11},(void*)&
Cyc__genrep_725};static char _tmp3DF[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_887={
19,{_tmp3DF,_tmp3DF,_tmp3DF + 9},(void*)& Cyc__genrep_720};static char _tmp3E0[8]="Deref_e";
static struct _tuple5 Cyc__gentuple_888={20,{_tmp3E0,_tmp3E0,_tmp3E0 + 8},(void*)&
Cyc__genrep_80};static char _tmp3E1[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_889={
21,{_tmp3E1,_tmp3E1,_tmp3E1 + 13},(void*)& Cyc__genrep_715};static char _tmp3E2[12]="AggrArrow_e";
static struct _tuple5 Cyc__gentuple_890={22,{_tmp3E2,_tmp3E2,_tmp3E2 + 12},(void*)&
Cyc__genrep_715};static char _tmp3E3[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_891={
23,{_tmp3E3,_tmp3E3,_tmp3E3 + 12},(void*)& Cyc__genrep_710};static char _tmp3E4[8]="Tuple_e";
static struct _tuple5 Cyc__gentuple_892={24,{_tmp3E4,_tmp3E4,_tmp3E4 + 8},(void*)&
Cyc__genrep_706};static char _tmp3E5[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_893={
25,{_tmp3E5,_tmp3E5,_tmp3E5 + 14},(void*)& Cyc__genrep_695};static char _tmp3E6[8]="Array_e";
static struct _tuple5 Cyc__gentuple_894={26,{_tmp3E6,_tmp3E6,_tmp3E6 + 8},(void*)&
Cyc__genrep_691};static char _tmp3E7[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_895={
27,{_tmp3E7,_tmp3E7,_tmp3E7 + 16},(void*)& Cyc__genrep_684};static char _tmp3E8[9]="Struct_e";
static struct _tuple5 Cyc__gentuple_896={28,{_tmp3E8,_tmp3E8,_tmp3E8 + 9},(void*)&
Cyc__genrep_676};static char _tmp3E9[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_897={
29,{_tmp3E9,_tmp3E9,_tmp3E9 + 13},(void*)& Cyc__genrep_671};static char _tmp3EA[9]="Tunion_e";
static struct _tuple5 Cyc__gentuple_898={30,{_tmp3EA,_tmp3EA,_tmp3EA + 9},(void*)&
Cyc__genrep_665};static char _tmp3EB[7]="Enum_e";static struct _tuple5 Cyc__gentuple_899={
31,{_tmp3EB,_tmp3EB,_tmp3EB + 7},(void*)& Cyc__genrep_658};static char _tmp3EC[11]="AnonEnum_e";
static struct _tuple5 Cyc__gentuple_900={32,{_tmp3EC,_tmp3EC,_tmp3EC + 11},(void*)&
Cyc__genrep_651};static char _tmp3ED[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_901={
33,{_tmp3ED,_tmp3ED,_tmp3ED + 9},(void*)& Cyc__genrep_639};static char _tmp3EE[16]="UnresolvedMem_e";
static struct _tuple5 Cyc__gentuple_902={34,{_tmp3EE,_tmp3EE,_tmp3EE + 16},(void*)&
Cyc__genrep_620};static char _tmp3EF[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_903={
35,{_tmp3EF,_tmp3EF,_tmp3EF + 10},(void*)& Cyc__genrep_472};static char _tmp3F0[10]="Codegen_e";
static struct _tuple5 Cyc__gentuple_904={36,{_tmp3F0,_tmp3F0,_tmp3F0 + 10},(void*)&
Cyc__genrep_85};static char _tmp3F1[7]="Fill_e";static struct _tuple5 Cyc__gentuple_905={
37,{_tmp3F1,_tmp3F1,_tmp3F1 + 7},(void*)& Cyc__genrep_80};static struct _tuple5*Cyc__genarr_906[
38]={& Cyc__gentuple_868,& Cyc__gentuple_869,& Cyc__gentuple_870,& Cyc__gentuple_871,&
Cyc__gentuple_872,& Cyc__gentuple_873,& Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876,&
Cyc__gentuple_877,& Cyc__gentuple_878,& Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881,&
Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,&
Cyc__gentuple_887,& Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,&
Cyc__gentuple_892,& Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,&
Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,&
Cyc__gentuple_902,& Cyc__gentuple_903,& Cyc__gentuple_904,& Cyc__gentuple_905};
static char _tmp3F3[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,{_tmp3F3,_tmp3F3,_tmp3F3 + 8},{(void*)((struct _tuple7**)Cyc__genarr_79),(void*)((
struct _tuple7**)Cyc__genarr_79),(void*)((struct _tuple7**)Cyc__genarr_79 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_906),(void*)((struct _tuple5**)Cyc__genarr_906),(
void*)((struct _tuple5**)Cyc__genarr_906 + 38)}};static char _tmp3F4[4]="Exp";static
struct _tagged_arr Cyc__genname_912={_tmp3F4,_tmp3F4,_tmp3F4 + 4};static char _tmp3F5[
5]="topt";static struct _tuple5 Cyc__gentuple_907={offsetof(struct Cyc_Absyn_Exp,topt),{
_tmp3F5,_tmp3F5,_tmp3F5 + 5},(void*)& Cyc__genrep_43};static char _tmp3F6[2]="r";
static struct _tuple5 Cyc__gentuple_908={offsetof(struct Cyc_Absyn_Exp,r),{_tmp3F6,
_tmp3F6,_tmp3F6 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp3F7[4]="loc";
static struct _tuple5 Cyc__gentuple_909={offsetof(struct Cyc_Absyn_Exp,loc),{_tmp3F7,
_tmp3F7,_tmp3F7 + 4},(void*)& Cyc__genrep_2};static char _tmp3F8[6]="annot";static
struct _tuple5 Cyc__gentuple_910={offsetof(struct Cyc_Absyn_Exp,annot),{_tmp3F8,
_tmp3F8,_tmp3F8 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_911[
4]={& Cyc__gentuple_907,& Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _tagged_arr*)&
Cyc__genname_912,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_911),(
void*)((struct _tuple5**)Cyc__genarr_911),(void*)((struct _tuple5**)Cyc__genarr_911
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static char _tmp3FB[10]="ArrayInfo";static struct
_tagged_arr Cyc__genname_981={_tmp3FB,_tmp3FB,_tmp3FB + 10};static char _tmp3FC[9]="elt_type";
static struct _tuple5 Cyc__gentuple_976={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),{
_tmp3FC,_tmp3FC,_tmp3FC + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char
_tmp3FD[3]="tq";static struct _tuple5 Cyc__gentuple_977={offsetof(struct Cyc_Absyn_ArrayInfo,tq),{
_tmp3FD,_tmp3FD,_tmp3FD + 3},(void*)& Cyc__genrep_133};static char _tmp3FE[9]="num_elts";
static struct _tuple5 Cyc__gentuple_978={offsetof(struct Cyc_Absyn_ArrayInfo,num_elts),{
_tmp3FE,_tmp3FE,_tmp3FE + 9},(void*)& Cyc__genrep_77};static char _tmp3FF[10]="zero_term";
static struct _tuple5 Cyc__gentuple_979={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),{
_tmp3FF,_tmp3FF,_tmp3FF + 10},(void*)& Cyc__genrep_963};static struct _tuple5*Cyc__genarr_980[
4]={& Cyc__gentuple_976,& Cyc__gentuple_977,& Cyc__gentuple_978,& Cyc__gentuple_979};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_981,sizeof(struct Cyc_Absyn_ArrayInfo),{(void*)((struct _tuple5**)Cyc__genarr_980),(
void*)((struct _tuple5**)Cyc__genarr_980),(void*)((struct _tuple5**)Cyc__genarr_980
+ 4)}};struct _tuple81{unsigned int f1;struct Cyc_Absyn_ArrayInfo f2;};static struct
_tuple6 Cyc__gentuple_982={offsetof(struct _tuple81,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_983={offsetof(struct _tuple81,f2),(void*)& Cyc_Absyn_array_info_t_rep};
static struct _tuple6*Cyc__genarr_984[2]={& Cyc__gentuple_982,& Cyc__gentuple_983};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_962={4,sizeof(struct _tuple81),{(
void*)((struct _tuple6**)Cyc__genarr_984),(void*)((struct _tuple6**)Cyc__genarr_984),(
void*)((struct _tuple6**)Cyc__genarr_984 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_943;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_944;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp402[5]="List";static struct _tagged_arr Cyc__genname_948={_tmp402,
_tmp402,_tmp402 + 5};static char _tmp403[3]="hd";static struct _tuple5 Cyc__gentuple_945={
offsetof(struct Cyc_List_List,hd),{_tmp403,_tmp403,_tmp403 + 3},(void*)& Cyc__genrep_696};
static char _tmp404[3]="tl";static struct _tuple5 Cyc__gentuple_946={offsetof(struct
Cyc_List_List,tl),{_tmp404,_tmp404,_tmp404 + 3},(void*)& Cyc__genrep_944};static
struct _tuple5*Cyc__genarr_947[2]={& Cyc__gentuple_945,& Cyc__gentuple_946};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_948,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_947),(void*)((struct _tuple5**)Cyc__genarr_947),(void*)((
struct _tuple5**)Cyc__genarr_947 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_944={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp407[7]="FnInfo";static struct _tagged_arr Cyc__genname_958={_tmp407,
_tmp407,_tmp407 + 7};static char _tmp408[6]="tvars";static struct _tuple5 Cyc__gentuple_949={
offsetof(struct Cyc_Absyn_FnInfo,tvars),{_tmp408,_tmp408,_tmp408 + 6},(void*)& Cyc__genrep_296};
static char _tmp409[7]="effect";static struct _tuple5 Cyc__gentuple_950={offsetof(
struct Cyc_Absyn_FnInfo,effect),{_tmp409,_tmp409,_tmp409 + 7},(void*)& Cyc__genrep_43};
static char _tmp40A[8]="ret_typ";static struct _tuple5 Cyc__gentuple_951={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),{_tmp40A,_tmp40A,_tmp40A + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp40B[5]="args";static struct _tuple5 Cyc__gentuple_952={offsetof(
struct Cyc_Absyn_FnInfo,args),{_tmp40B,_tmp40B,_tmp40B + 5},(void*)& Cyc__genrep_944};
static char _tmp40C[10]="c_varargs";static struct _tuple5 Cyc__gentuple_953={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),{_tmp40C,_tmp40C,_tmp40C + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp40D[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_954={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),{_tmp40D,_tmp40D,
_tmp40D + 12},(void*)& Cyc__genrep_579};static char _tmp40E[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_955={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),{_tmp40E,
_tmp40E,_tmp40E + 7},(void*)& Cyc__genrep_355};static char _tmp40F[11]="attributes";
static struct _tuple5 Cyc__gentuple_956={offsetof(struct Cyc_Absyn_FnInfo,attributes),{
_tmp40F,_tmp40F,_tmp40F + 11},(void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_957[
8]={& Cyc__gentuple_949,& Cyc__gentuple_950,& Cyc__gentuple_951,& Cyc__gentuple_952,&
Cyc__gentuple_953,& Cyc__gentuple_954,& Cyc__gentuple_955,& Cyc__gentuple_956};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_958,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_957),(
void*)((struct _tuple5**)Cyc__genarr_957),(void*)((struct _tuple5**)Cyc__genarr_957
+ 8)}};struct _tuple82{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_959={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_960={offsetof(struct _tuple82,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_961[2]={& Cyc__gentuple_959,& Cyc__gentuple_960};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_943={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_961),(void*)((struct _tuple6**)Cyc__genarr_961),(
void*)((struct _tuple6**)Cyc__genarr_961 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_939;static struct _tuple6 Cyc__gentuple_940={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_941={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_270};static struct _tuple6*Cyc__genarr_942[2]={&
Cyc__gentuple_940,& Cyc__gentuple_941};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_939={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_942),(void*)((
struct _tuple6**)Cyc__genarr_942),(void*)((struct _tuple6**)Cyc__genarr_942 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_935;struct _tuple83{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_936={offsetof(
struct _tuple83,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_937={
offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_938[2]={& Cyc__gentuple_936,& Cyc__gentuple_937};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_935={4,sizeof(struct _tuple83),{(void*)((
struct _tuple6**)Cyc__genarr_938),(void*)((struct _tuple6**)Cyc__genarr_938),(void*)((
struct _tuple6**)Cyc__genarr_938 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_930;
static struct _tuple6 Cyc__gentuple_931={offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_932={offsetof(struct _tuple63,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_933={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_342};
static struct _tuple6*Cyc__genarr_934[3]={& Cyc__gentuple_931,& Cyc__gentuple_932,&
Cyc__gentuple_933};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_930={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_934),(void*)((
struct _tuple6**)Cyc__genarr_934),(void*)((struct _tuple6**)Cyc__genarr_934 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_925;struct _tuple84{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_926={
offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_927={
offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_928={
offsetof(struct _tuple84,f3),(void*)& Cyc__genrep_659};static struct _tuple6*Cyc__genarr_929[
3]={& Cyc__gentuple_926,& Cyc__gentuple_927,& Cyc__gentuple_928};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_925={4,sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_929),(
void*)((struct _tuple6**)Cyc__genarr_929),(void*)((struct _tuple6**)Cyc__genarr_929
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_74;static struct _tuple6 Cyc__gentuple_922={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_923={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_75};static struct _tuple6*Cyc__genarr_924[
2]={& Cyc__gentuple_922,& Cyc__gentuple_923};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_74={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_924),(
void*)((struct _tuple6**)Cyc__genarr_924),(void*)((struct _tuple6**)Cyc__genarr_924
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_66;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_67;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static char _tmp417[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1114={
_tmp417,_tmp417,_tmp417 + 12};static char _tmp418[5]="name";static struct _tuple5 Cyc__gentuple_1109={
offsetof(struct Cyc_Absyn_Typedefdecl,name),{_tmp418,_tmp418,_tmp418 + 5},(void*)&
Cyc__genrep_10};static char _tmp419[4]="tvs";static struct _tuple5 Cyc__gentuple_1110={
offsetof(struct Cyc_Absyn_Typedefdecl,tvs),{_tmp419,_tmp419,_tmp419 + 4},(void*)&
Cyc__genrep_296};static char _tmp41A[5]="kind";static struct _tuple5 Cyc__gentuple_1111={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),{_tmp41A,_tmp41A,_tmp41A + 5},(void*)&
Cyc__genrep_1073};static char _tmp41B[5]="defn";static struct _tuple5 Cyc__gentuple_1112={
offsetof(struct Cyc_Absyn_Typedefdecl,defn),{_tmp41B,_tmp41B,_tmp41B + 5},(void*)&
Cyc__genrep_43};static struct _tuple5*Cyc__genarr_1113[4]={& Cyc__gentuple_1109,&
Cyc__gentuple_1110,& Cyc__gentuple_1111,& Cyc__gentuple_1112};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_1114,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1113),(
void*)((struct _tuple5**)Cyc__genarr_1113),(void*)((struct _tuple5**)Cyc__genarr_1113
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_67={1,1,(void*)((void*)&
Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple85{unsigned int f1;struct _tuple0*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*f4;struct Cyc_Core_Opt*f5;}
;static struct _tuple6 Cyc__gentuple_68={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_69={offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_70={offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_53};
static struct _tuple6 Cyc__gentuple_71={offsetof(struct _tuple85,f4),(void*)& Cyc__genrep_67};
static struct _tuple6 Cyc__gentuple_72={offsetof(struct _tuple85,f5),(void*)& Cyc__genrep_43};
static struct _tuple6*Cyc__genarr_73[5]={& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,&
Cyc__gentuple_71,& Cyc__gentuple_72};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_66={
4,sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_73),(void*)((
struct _tuple6**)Cyc__genarr_73),(void*)((struct _tuple6**)Cyc__genarr_73 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_52;static struct _tuple6 Cyc__gentuple_58={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_59={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_60[
2]={& Cyc__gentuple_58,& Cyc__gentuple_59};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_52={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_60),(void*)((
struct _tuple6**)Cyc__genarr_60),(void*)((struct _tuple6**)Cyc__genarr_60 + 2)}};
static char _tmp420[9]="VoidType";static struct _tuple7 Cyc__gentuple_44={0,{_tmp420,
_tmp420,_tmp420 + 9}};static char _tmp421[10]="FloatType";static struct _tuple7 Cyc__gentuple_45={
1,{_tmp421,_tmp421,_tmp421 + 10}};static char _tmp422[8]="HeapRgn";static struct
_tuple7 Cyc__gentuple_46={2,{_tmp422,_tmp422,_tmp422 + 8}};static struct _tuple7*Cyc__genarr_47[
3]={& Cyc__gentuple_44,& Cyc__gentuple_45,& Cyc__gentuple_46};static char _tmp423[5]="Evar";
static struct _tuple5 Cyc__gentuple_1083={0,{_tmp423,_tmp423,_tmp423 + 5},(void*)&
Cyc__genrep_1068};static char _tmp424[8]="VarType";static struct _tuple5 Cyc__gentuple_1084={
1,{_tmp424,_tmp424,_tmp424 + 8},(void*)& Cyc__genrep_1064};static char _tmp425[11]="TunionType";
static struct _tuple5 Cyc__gentuple_1085={2,{_tmp425,_tmp425,_tmp425 + 11},(void*)&
Cyc__genrep_1038};static char _tmp426[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1086={
3,{_tmp426,_tmp426,_tmp426 + 16},(void*)& Cyc__genrep_1012};static char _tmp427[12]="PointerType";
static struct _tuple5 Cyc__gentuple_1087={4,{_tmp427,_tmp427,_tmp427 + 12},(void*)&
Cyc__genrep_996};static char _tmp428[8]="IntType";static struct _tuple5 Cyc__gentuple_1088={
5,{_tmp428,_tmp428,_tmp428 + 8},(void*)& Cyc__genrep_985};static char _tmp429[11]="DoubleType";
static struct _tuple5 Cyc__gentuple_1089={6,{_tmp429,_tmp429,_tmp429 + 11},(void*)&
Cyc__genrep_61};static char _tmp42A[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1090={
7,{_tmp42A,_tmp42A,_tmp42A + 10},(void*)& Cyc__genrep_962};static char _tmp42B[7]="FnType";
static struct _tuple5 Cyc__gentuple_1091={8,{_tmp42B,_tmp42B,_tmp42B + 7},(void*)&
Cyc__genrep_943};static char _tmp42C[10]="TupleType";static struct _tuple5 Cyc__gentuple_1092={
9,{_tmp42C,_tmp42C,_tmp42C + 10},(void*)& Cyc__genrep_939};static char _tmp42D[9]="AggrType";
static struct _tuple5 Cyc__gentuple_1093={10,{_tmp42D,_tmp42D,_tmp42D + 9},(void*)&
Cyc__genrep_935};static char _tmp42E[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1094={
11,{_tmp42E,_tmp42E,_tmp42E + 13},(void*)& Cyc__genrep_930};static char _tmp42F[9]="EnumType";
static struct _tuple5 Cyc__gentuple_1095={12,{_tmp42F,_tmp42F,_tmp42F + 9},(void*)&
Cyc__genrep_925};static char _tmp430[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1096={
13,{_tmp430,_tmp430,_tmp430 + 13},(void*)& Cyc__genrep_74};static char _tmp431[11]="SizeofType";
static struct _tuple5 Cyc__gentuple_1097={14,{_tmp431,_tmp431,_tmp431 + 11},(void*)&
Cyc__genrep_48};static char _tmp432[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1098={
15,{_tmp432,_tmp432,_tmp432 + 14},(void*)& Cyc__genrep_48};static char _tmp433[12]="TypedefType";
static struct _tuple5 Cyc__gentuple_1099={16,{_tmp433,_tmp433,_tmp433 + 12},(void*)&
Cyc__genrep_66};static char _tmp434[8]="TagType";static struct _tuple5 Cyc__gentuple_1100={
17,{_tmp434,_tmp434,_tmp434 + 8},(void*)& Cyc__genrep_48};static char _tmp435[8]="TypeInt";
static struct _tuple5 Cyc__gentuple_1101={18,{_tmp435,_tmp435,_tmp435 + 8},(void*)&
Cyc__genrep_61};static char _tmp436[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1102={
19,{_tmp436,_tmp436,_tmp436 + 10},(void*)& Cyc__genrep_48};static char _tmp437[8]="JoinEff";
static struct _tuple5 Cyc__gentuple_1103={20,{_tmp437,_tmp437,_tmp437 + 8},(void*)&
Cyc__genrep_52};static char _tmp438[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1104={
21,{_tmp438,_tmp438,_tmp438 + 8},(void*)& Cyc__genrep_48};static struct _tuple5*Cyc__genarr_1105[
22]={& Cyc__gentuple_1083,& Cyc__gentuple_1084,& Cyc__gentuple_1085,& Cyc__gentuple_1086,&
Cyc__gentuple_1087,& Cyc__gentuple_1088,& Cyc__gentuple_1089,& Cyc__gentuple_1090,&
Cyc__gentuple_1091,& Cyc__gentuple_1092,& Cyc__gentuple_1093,& Cyc__gentuple_1094,&
Cyc__gentuple_1095,& Cyc__gentuple_1096,& Cyc__gentuple_1097,& Cyc__gentuple_1098,&
Cyc__gentuple_1099,& Cyc__gentuple_1100,& Cyc__gentuple_1101,& Cyc__gentuple_1102,&
Cyc__gentuple_1103,& Cyc__gentuple_1104};static char _tmp43A[5]="Type";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_type_t_rep={5,{_tmp43A,_tmp43A,_tmp43A + 5},{(void*)((struct _tuple7**)
Cyc__genarr_47),(void*)((struct _tuple7**)Cyc__genarr_47),(void*)((struct _tuple7**)
Cyc__genarr_47 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_1105),(void*)((struct
_tuple5**)Cyc__genarr_1105),(void*)((struct _tuple5**)Cyc__genarr_1105 + 22)}};
static char _tmp43B[8]="Vardecl";static struct _tagged_arr Cyc__genname_153={_tmp43B,
_tmp43B,_tmp43B + 8};static char _tmp43C[3]="sc";static struct _tuple5 Cyc__gentuple_144={
offsetof(struct Cyc_Absyn_Vardecl,sc),{_tmp43C,_tmp43C,_tmp43C + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp43D[5]="name";static struct _tuple5 Cyc__gentuple_145={offsetof(
struct Cyc_Absyn_Vardecl,name),{_tmp43D,_tmp43D,_tmp43D + 5},(void*)& Cyc__genrep_10};
static char _tmp43E[3]="tq";static struct _tuple5 Cyc__gentuple_146={offsetof(struct
Cyc_Absyn_Vardecl,tq),{_tmp43E,_tmp43E,_tmp43E + 3},(void*)& Cyc__genrep_133};
static char _tmp43F[5]="type";static struct _tuple5 Cyc__gentuple_147={offsetof(
struct Cyc_Absyn_Vardecl,type),{_tmp43F,_tmp43F,_tmp43F + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp440[12]="initializer";static struct _tuple5 Cyc__gentuple_148={
offsetof(struct Cyc_Absyn_Vardecl,initializer),{_tmp440,_tmp440,_tmp440 + 12},(
void*)& Cyc__genrep_77};static char _tmp441[4]="rgn";static struct _tuple5 Cyc__gentuple_149={
offsetof(struct Cyc_Absyn_Vardecl,rgn),{_tmp441,_tmp441,_tmp441 + 4},(void*)& Cyc__genrep_43};
static char _tmp442[11]="attributes";static struct _tuple5 Cyc__gentuple_150={
offsetof(struct Cyc_Absyn_Vardecl,attributes),{_tmp442,_tmp442,_tmp442 + 11},(void*)&
Cyc__genrep_87};static char _tmp443[8]="escapes";static struct _tuple5 Cyc__gentuple_151={
offsetof(struct Cyc_Absyn_Vardecl,escapes),{_tmp443,_tmp443,_tmp443 + 8},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_152[8]={& Cyc__gentuple_144,&
Cyc__gentuple_145,& Cyc__gentuple_146,& Cyc__gentuple_147,& Cyc__gentuple_148,& Cyc__gentuple_149,&
Cyc__gentuple_150,& Cyc__gentuple_151};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_153,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_152),(void*)((struct _tuple5**)Cyc__genarr_152),(void*)((
struct _tuple5**)Cyc__genarr_152 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_132={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple86{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_403={offsetof(
struct _tuple86,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_404={
offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_132};static struct _tuple6*Cyc__genarr_405[
2]={& Cyc__gentuple_403,& Cyc__gentuple_404};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_402={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_405),(
void*)((struct _tuple6**)Cyc__genarr_405),(void*)((struct _tuple6**)Cyc__genarr_405
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1134;struct _tuple87{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1135={offsetof(struct _tuple87,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1136={offsetof(struct _tuple87,f2),(
void*)& Cyc__genrep_230};static struct _tuple6 Cyc__gentuple_1137={offsetof(struct
_tuple87,f3),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_1138={
offsetof(struct _tuple87,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_1139[
4]={& Cyc__gentuple_1135,& Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1134={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_1139),(void*)((struct _tuple6**)Cyc__genarr_1139),(
void*)((struct _tuple6**)Cyc__genarr_1139 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1130;static struct _tuple6 Cyc__gentuple_1131={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1132={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_131};static struct _tuple6*Cyc__genarr_1133[2]={&
Cyc__gentuple_1131,& Cyc__gentuple_1132};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1130={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_1133),(void*)((
struct _tuple6**)Cyc__genarr_1133),(void*)((struct _tuple6**)Cyc__genarr_1133 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1126;struct _tuple88{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1127={
offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1128={
offsetof(struct _tuple88,f2),(void*)((void*)& Cyc__genrep_340)};static struct
_tuple6*Cyc__genarr_1129[2]={& Cyc__gentuple_1127,& Cyc__gentuple_1128};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1126={4,sizeof(struct _tuple88),{(void*)((
struct _tuple6**)Cyc__genarr_1129),(void*)((struct _tuple6**)Cyc__genarr_1129),(
void*)((struct _tuple6**)Cyc__genarr_1129 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1122;struct _tuple89{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1123={offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1124={offsetof(struct _tuple89,f2),(void*)((void*)&
Cyc__genrep_286)};static struct _tuple6*Cyc__genarr_1125[2]={& Cyc__gentuple_1123,&
Cyc__gentuple_1124};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1122={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_1125),(void*)((
struct _tuple6**)Cyc__genarr_1125),(void*)((struct _tuple6**)Cyc__genarr_1125 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1118;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1119={
offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1120={
offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_254};static struct _tuple6*Cyc__genarr_1121[
2]={& Cyc__gentuple_1119,& Cyc__gentuple_1120};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1118={4,sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1121),(
void*)((struct _tuple6**)Cyc__genarr_1121),(void*)((struct _tuple6**)Cyc__genarr_1121
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_41;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_42={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple91{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1115={offsetof(
struct _tuple91,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1116={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_42};static struct _tuple6*Cyc__genarr_1117[
2]={& Cyc__gentuple_1115,& Cyc__gentuple_1116};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41={4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1117),(
void*)((struct _tuple6**)Cyc__genarr_1117),(void*)((struct _tuple6**)Cyc__genarr_1117
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_36;struct _tuple92{
unsigned int f1;struct _tagged_arr*f2;struct Cyc_List_List*f3;};static struct _tuple6
Cyc__gentuple_37={offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_38={offsetof(struct _tuple92,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_39={offsetof(struct _tuple92,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_40[3]={& Cyc__gentuple_37,& Cyc__gentuple_38,& Cyc__gentuple_39};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_36={4,sizeof(struct _tuple92),{(
void*)((struct _tuple6**)Cyc__genarr_40),(void*)((struct _tuple6**)Cyc__genarr_40),(
void*)((struct _tuple6**)Cyc__genarr_40 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;static struct _tuple6 Cyc__gentuple_32={offsetof(struct _tuple53,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_33={offsetof(struct
_tuple53,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_34={
offsetof(struct _tuple53,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_35[
3]={& Cyc__gentuple_32,& Cyc__gentuple_33,& Cyc__gentuple_34};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_35),(
void*)((struct _tuple6**)Cyc__genarr_35),(void*)((struct _tuple6**)Cyc__genarr_35 + 
3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_4;static struct _tuple6 Cyc__gentuple_6={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_7={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_8[
2]={& Cyc__gentuple_6,& Cyc__gentuple_7};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_4={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_8),(void*)((
struct _tuple6**)Cyc__genarr_8),(void*)((struct _tuple6**)Cyc__genarr_8 + 2)}};
static struct _tuple7*Cyc__genarr_3[0]={};static char _tmp451[6]="Var_d";static
struct _tuple5 Cyc__gentuple_1140={0,{_tmp451,_tmp451,_tmp451 + 6},(void*)& Cyc__genrep_402};
static char _tmp452[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1141={1,{_tmp452,
_tmp452,_tmp452 + 5},(void*)& Cyc__genrep_85};static char _tmp453[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1142={2,{_tmp453,_tmp453,_tmp453 + 6},(void*)& Cyc__genrep_1134};
static char _tmp454[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1143={3,{_tmp454,
_tmp454,_tmp454 + 7},(void*)& Cyc__genrep_1130};static char _tmp455[7]="Aggr_d";
static struct _tuple5 Cyc__gentuple_1144={4,{_tmp455,_tmp455,_tmp455 + 7},(void*)&
Cyc__genrep_1126};static char _tmp456[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1145={
5,{_tmp456,_tmp456,_tmp456 + 9},(void*)& Cyc__genrep_1122};static char _tmp457[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1146={6,{_tmp457,_tmp457,_tmp457 + 7},(void*)&
Cyc__genrep_1118};static char _tmp458[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1147={
7,{_tmp458,_tmp458,_tmp458 + 10},(void*)& Cyc__genrep_41};static char _tmp459[12]="Namespace_d";
static struct _tuple5 Cyc__gentuple_1148={8,{_tmp459,_tmp459,_tmp459 + 12},(void*)&
Cyc__genrep_36};static char _tmp45A[8]="Using_d";static struct _tuple5 Cyc__gentuple_1149={
9,{_tmp45A,_tmp45A,_tmp45A + 8},(void*)& Cyc__genrep_9};static char _tmp45B[10]="ExternC_d";
static struct _tuple5 Cyc__gentuple_1150={10,{_tmp45B,_tmp45B,_tmp45B + 10},(void*)&
Cyc__genrep_4};static struct _tuple5*Cyc__genarr_1151[11]={& Cyc__gentuple_1140,&
Cyc__gentuple_1141,& Cyc__gentuple_1142,& Cyc__gentuple_1143,& Cyc__gentuple_1144,&
Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,& Cyc__gentuple_1148,&
Cyc__gentuple_1149,& Cyc__gentuple_1150};static char _tmp45D[9]="Raw_decl";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,{_tmp45D,_tmp45D,_tmp45D + 9},{(
void*)((struct _tuple7**)Cyc__genarr_3),(void*)((struct _tuple7**)Cyc__genarr_3),(
void*)((struct _tuple7**)Cyc__genarr_3 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1151),(
void*)((struct _tuple5**)Cyc__genarr_1151),(void*)((struct _tuple5**)Cyc__genarr_1151
+ 11)}};static char _tmp45E[5]="Decl";static struct _tagged_arr Cyc__genname_1155={
_tmp45E,_tmp45E,_tmp45E + 5};static char _tmp45F[2]="r";static struct _tuple5 Cyc__gentuple_1152={
offsetof(struct Cyc_Absyn_Decl,r),{_tmp45F,_tmp45F,_tmp45F + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static char _tmp460[4]="loc";static struct _tuple5 Cyc__gentuple_1153={offsetof(
struct Cyc_Absyn_Decl,loc),{_tmp460,_tmp460,_tmp460 + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_1154[2]={& Cyc__gentuple_1152,& Cyc__gentuple_1153};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1155,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1154),(
void*)((struct _tuple5**)Cyc__genarr_1154),(void*)((struct _tuple5**)Cyc__genarr_1154
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static char _tmp463[5]="List";static struct _tagged_arr
Cyc__genname_1159={_tmp463,_tmp463,_tmp463 + 5};static char _tmp464[3]="hd";static
struct _tuple5 Cyc__gentuple_1156={offsetof(struct Cyc_List_List,hd),{_tmp464,
_tmp464,_tmp464 + 3},(void*)& Cyc__genrep_1};static char _tmp465[3]="tl";static
struct _tuple5 Cyc__gentuple_1157={offsetof(struct Cyc_List_List,tl),{_tmp465,
_tmp465,_tmp465 + 3},(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_1158[
2]={& Cyc__gentuple_1156,& Cyc__gentuple_1157};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1159,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1158),(void*)((struct _tuple5**)Cyc__genarr_1158),(
void*)((struct _tuple5**)Cyc__genarr_1158 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
