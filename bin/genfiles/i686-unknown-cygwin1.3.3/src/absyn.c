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
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
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
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
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
#endif

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
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;
struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Carray_mod_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
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
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
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
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(
struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct _tuple0*name);void*Cyc_Absyn_union_typ(
struct _tagged_arr*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*
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
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
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
vs1,vs2);}int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){void*_tmp0=(*
q1).f1;void*_tmp1=(*q2).f1;{struct _tuple9 _tmp3=({struct _tuple9 _tmp2;_tmp2.f1=
_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;void*_tmp6;struct Cyc_List_List*
_tmp7;void*_tmp8;struct Cyc_List_List*_tmp9;void*_tmpA;struct Cyc_List_List*_tmpB;
void*_tmpC;struct Cyc_List_List*_tmpD;void*_tmpE;void*_tmpF;void*_tmp10;void*
_tmp11;_LL1: _tmp4=_tmp3.f1;if((int)_tmp4 != 0)goto _LL3;_tmp5=_tmp3.f2;if((int)
_tmp5 != 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(_tmp6 <= (void*)1?1:*((
int*)_tmp6)!= 0)goto _LL5;_tmp7=((struct Cyc_Absyn_Rel_n_struct*)_tmp6)->f1;_tmp8=
_tmp3.f2;if(_tmp8 <= (void*)1?1:*((int*)_tmp8)!= 0)goto _LL5;_tmp9=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8)->f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(_tmpA <= (
void*)1?1:*((int*)_tmpA)!= 1)goto _LL7;_tmpB=((struct Cyc_Absyn_Abs_n_struct*)
_tmpA)->f1;_tmpC=_tmp3.f2;if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 1)goto _LL7;
_tmpD=((struct Cyc_Absyn_Abs_n_struct*)_tmpC)->f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(
_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if((int)_tmpE != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if((int)_tmpF != 0)goto _LLB;_LLA:
return 1;_LLB: _tmp10=_tmp3.f1;if(_tmp10 <= (void*)1?1:*((int*)_tmp10)!= 0)goto _LLD;
_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(_tmp11 <= (void*)1?1:*((int*)_tmp11)!= 0)
goto _LL0;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity
!= 0?tv2->identity != 0: 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;
_LL10: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LL12;_tmp14=((struct Cyc_Absyn_Rel_n_struct*)
_tmp13)->f1;if(_tmp14 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp13 <= (void*)1?1:*((
int*)_tmp13)!= 1)goto _LL14;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)_tmp13)->f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((int)_tmp13 != 0)goto _LL16;_LL15:
return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(void*)({
struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct
Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=Cyc_Absyn_new_type_counter
++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(
void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp18;_tmp18.q_const=x.q_const?1: y.q_const;_tmp18.q_volatile=x.q_volatile?1: y.q_volatile;
_tmp18.q_restrict=x.q_restrict?1: y.q_restrict;_tmp18;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.q_const=0;
_tmp19.q_volatile=0;_tmp19.q_restrict=0;_tmp19;});}struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){
return({struct Cyc_Absyn_Tqual _tmp1A;_tmp1A.q_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=
0;_tmp1A;});}static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={2,(void*)((
void*)1),1};static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,(void*)((
void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(
void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x){return({struct Cyc_Absyn_Conref*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));
_tmp1F[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(void*)
x;_tmp20;});_tmp1F;}));_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->v=(void*)((
void*)0);_tmp21;});}static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_conref_constraint={
0,(void*)1};static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(void*)((void*)&
Cyc_Absyn_true_conref_constraint)};struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v;static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_conref_constraint={
0,(void*)0};static struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(void*)((void*)&
Cyc_Absyn_false_conref_constraint)};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_bounds_one_conref_constraint={
0,(void*)((void*)& Cyc_Absyn_one_bt)};static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
void*)((void*)& Cyc_Absyn_bounds_one_conref_constraint)};struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Eq_constr_struct
Cyc_Absyn_bounds_unknown_conref_constraint={0,(void*)((void*)0)};static struct Cyc_Absyn_Conref
Cyc_Absyn_bounds_unknown_conref_v={(void*)((void*)& Cyc_Absyn_bounds_unknown_conref_constraint)};
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref=& Cyc_Absyn_bounds_unknown_conref_v;
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){void*
_tmp26=(void*)x->v;struct Cyc_Absyn_Conref*_tmp27;struct Cyc_Absyn_Conref**_tmp28;
_LL19: if((int)_tmp26 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp26 <= (void*)1?1:*((
int*)_tmp26)!= 0)goto _LL1D;_LL1C: return x;_LL1D: if(_tmp26 <= (void*)1?1:*((int*)
_tmp26)!= 1)goto _LL18;_tmp27=((struct Cyc_Absyn_Forward_constr_struct*)_tmp26)->f1;
_tmp28=(struct Cyc_Absyn_Conref**)&((struct Cyc_Absyn_Forward_constr_struct*)
_tmp26)->f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp29=Cyc_Absyn_compress_conref(*
_tmp28);*_tmp28=_tmp29;return _tmp29;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp2A=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp2B;_LL20: if(
_tmp2A <= (void*)1?1:*((int*)_tmp2A)!= 0)goto _LL22;_tmp2B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2A)->f1;_LL21: return _tmp2B;_LL22:;_LL23:({void*_tmp2C[0]={};Cyc_Tcutil_impos(({
const char*_tmp2D="conref_val";_tag_arr(_tmp2D,sizeof(char),_get_zero_arr_size(
_tmp2D,11));}),_tag_arr(_tmp2C,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){void*_tmp2E=(void*)(Cyc_Absyn_compress_conref(x))->v;
void*_tmp2F;_LL25: if(_tmp2E <= (void*)1?1:*((int*)_tmp2E)!= 0)goto _LL27;_tmp2F=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2E)->f1;_LL26: return _tmp2F;_LL27:;
_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*k){void*_tmp30=k;struct Cyc_Core_Opt*
_tmp31;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt
_tmp34;void*_tmp35;void**_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt
_tmp38;void*_tmp39;void**_tmp3A;_LL2A: if(*((int*)_tmp30)!= 0)goto _LL2C;_LL2B:
goto _LL2D;_LL2C: if(*((int*)_tmp30)!= 1)goto _LL2E;_tmp31=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1;if(_tmp31 != 0)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(*((int*)_tmp30)!= 2)
goto _LL30;_tmp32=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp32 != 0)
goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp30)!= 1)goto _LL32;_tmp33=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp30)->f1;if(_tmp33 == 0)goto _LL32;_tmp34=*_tmp33;
_tmp35=(void*)_tmp34.v;_tmp36=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1).v;_LL31: _tmp3A=_tmp36;goto _LL33;_LL32: if(*((int*)_tmp30)!= 2)goto
_LL29;_tmp37=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp37 == 0)goto
_LL29;_tmp38=*_tmp37;_tmp39=(void*)_tmp38.v;_tmp3A=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp30)->f1).v;_LL33:*_tmp3A=Cyc_Absyn_compress_kb(*_tmp3A);return*_tmp3A;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3B=Cyc_Absyn_compress_kb(kb);void*
_tmp3C;struct Cyc_Core_Opt*_tmp3D;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Core_Opt*
_tmp3F;struct Cyc_Core_Opt**_tmp40;void*_tmp41;_LL35: if(*((int*)_tmp3B)!= 0)goto
_LL37;_tmp3C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3B)->f1;_LL36: return
_tmp3C;_LL37: if(*((int*)_tmp3B)!= 1)goto _LL39;_tmp3D=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_LL38: _tmp40=_tmp3E;_tmp41=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3B)!= 2)goto _LL34;_tmp3F=((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp40=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp41=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f2;_LL3A:*_tmp40=({
struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(void*)_tmp41;_tmp44;});
_tmp43;}));_tmp42;});return _tmp41;_LL34:;}static struct Cyc_Absyn_IntType_struct
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
struct Cyc_Absyn_DoubleType_struct*_tmp50=_cycalloc_atomic(sizeof(*_tmp50));
_tmp50[0]=({struct Cyc_Absyn_DoubleType_struct _tmp51;_tmp51.tag=6;_tmp51.f1=b;
_tmp51;});_tmp50;});}static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={1,0};
static char _tmp53[4]="exn";static struct _tagged_arr Cyc_Absyn_exn_str={_tmp53,
_tmp53,_tmp53 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static
char _tmp54[15]="Null_Exception";static struct _tagged_arr Cyc_Absyn_Null_Exception_str={
_tmp54,_tmp54,_tmp54 + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp55[13]="Array_bounds";
static struct _tagged_arr Cyc_Absyn_Array_bounds_str={_tmp55,_tmp55,_tmp55 + 13};
static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp56[16]="Match_Exception";static struct
_tagged_arr Cyc_Absyn_Match_Exception_str={_tmp56,_tmp56,_tmp56 + 16};static struct
_tuple0 Cyc_Absyn_Match_Exception_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp57[10]="Bad_alloc";static struct
_tagged_arr Cyc_Absyn_Bad_alloc_str={_tmp57,_tmp57,_tmp57 + 10};static struct
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
static char _tmp5A[9]="PrintArg";static struct _tagged_arr Cyc_Absyn_printarg_str={
_tmp5A,_tmp5A,_tmp5A + 9};static char _tmp5B[9]="ScanfArg";static struct _tagged_arr
Cyc_Absyn_scanfarg_str={_tmp5B,_tmp5B,_tmp5B + 9};static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_std_namespace={1,0};static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_tunion_scanf_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(),(
void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=t;
_tmp5D;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=t;_tmp5E;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp60;_tmp60.tag=4;_tmp60.f1=({struct Cyc_Absyn_PtrInfo
_tmp61;_tmp61.elt_typ=(void*)t;_tmp61.elt_tq=tq;_tmp61.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp62;_tmp62.rgn=(void*)r;_tmp62.nullable=Cyc_Absyn_true_conref;_tmp62.bounds=
Cyc_Absyn_new_conref(b);_tmp62.zero_term=zeroterm;_tmp62;});_tmp61;});_tmp60;});
_tmp5F;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,
struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_PointerType_struct
_tmp64;_tmp64.tag=4;_tmp64.f1=({struct Cyc_Absyn_PtrInfo _tmp65;_tmp65.elt_typ=(
void*)t;_tmp65.elt_tq=tq;_tmp65.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp66;_tmp66.rgn=(
void*)r;_tmp66.nullable=Cyc_Absyn_false_conref;_tmp66.bounds=Cyc_Absyn_new_conref(
b);_tmp66.zero_term=zeroterm;_tmp66;});_tmp65;});_tmp64;});_tmp63;});}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67[0]=({struct Cyc_Absyn_PointerType_struct _tmp68;_tmp68.tag=4;_tmp68.f1=({
struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=(void*)t;_tmp69.elt_tq=tq;_tmp69.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp6A;_tmp6A.rgn=(void*)r;_tmp6A.nullable=Cyc_Absyn_true_conref;
_tmp6A.bounds=Cyc_Absyn_bounds_one_conref;_tmp6A.zero_term=zeroterm;_tmp6A;});
_tmp69;});_tmp68;});_tmp67;});}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual
tq){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6B=_cycalloc(sizeof(*
_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_PointerType_struct _tmp6C;_tmp6C.tag=4;
_tmp6C.f1=({struct Cyc_Absyn_PtrInfo _tmp6D;_tmp6D.elt_typ=(void*)t;_tmp6D.elt_tq=
tq;_tmp6D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6E;_tmp6E.rgn=(void*)((void*)2);
_tmp6E.nullable=Cyc_Absyn_true_conref;_tmp6E.bounds=Cyc_Absyn_bounds_one_conref;
_tmp6E.zero_term=Cyc_Absyn_false_conref;_tmp6E;});_tmp6D;});_tmp6C;});_tmp6B;});}
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6F=_cycalloc(
sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_PointerType_struct _tmp70;_tmp70.tag=
4;_tmp70.f1=({struct Cyc_Absyn_PtrInfo _tmp71;_tmp71.elt_typ=(void*)t;_tmp71.elt_tq=
tq;_tmp71.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp72;_tmp72.rgn=(void*)r;_tmp72.nullable=
Cyc_Absyn_false_conref;_tmp72.bounds=Cyc_Absyn_bounds_one_conref;_tmp72.zero_term=
zeroterm;_tmp72;});_tmp71;});_tmp70;});_tmp6F;});}void*Cyc_Absyn_tagged_typ(void*
t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=({
struct Cyc_Absyn_PointerType_struct _tmp74;_tmp74.tag=4;_tmp74.f1=({struct Cyc_Absyn_PtrInfo
_tmp75;_tmp75.elt_typ=(void*)t;_tmp75.elt_tq=tq;_tmp75.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp76;_tmp76.rgn=(void*)r;_tmp76.nullable=Cyc_Absyn_true_conref;_tmp76.bounds=
Cyc_Absyn_bounds_unknown_conref;_tmp76.zero_term=zeroterm;_tmp76;});_tmp75;});
_tmp74;});_tmp73;});}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term){return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp78;_tmp78.tag=7;_tmp78.f1=({struct Cyc_Absyn_ArrayInfo
_tmp79;_tmp79.elt_type=(void*)elt_type;_tmp79.tq=tq;_tmp79.num_elts=num_elts;
_tmp79.zero_term=zero_term;_tmp79;});_tmp78;});_tmp77;});}static char _tmp83[8]="__sFILE";
void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;static struct _tagged_arr sf_str={
_tmp83,_tmp83,_tmp83 + 8};static struct _tagged_arr*sf=& sf_str;if(file_t_opt == 0){
struct _tuple0*file_t_name=({struct _tuple0*_tmp82=_cycalloc(sizeof(*_tmp82));
_tmp82->f1=(void*)& Cyc_Absyn_std_namespace;_tmp82->f2=sf;_tmp82;});struct Cyc_Absyn_Aggrdecl*
sd=({struct Cyc_Absyn_Aggrdecl*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->kind=(
void*)((void*)0);_tmp81->sc=(void*)((void*)1);_tmp81->name=file_t_name;_tmp81->tvs=
0;_tmp81->impl=0;_tmp81->attributes=0;_tmp81;});void*file_struct_typ=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({
struct Cyc_Absyn_AggrType_struct _tmp7C;_tmp7C.tag=10;_tmp7C.f1=({struct Cyc_Absyn_AggrInfo
_tmp7D;_tmp7D.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp7E=
_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp7F;
_tmp7F.tag=1;_tmp7F.f1=({struct Cyc_Absyn_Aggrdecl**_tmp80=_cycalloc(sizeof(*
_tmp80));_tmp80[0]=sd;_tmp80;});_tmp7F;});_tmp7E;}));_tmp7D.targs=0;_tmp7D;});
_tmp7C;});_tmp7B;});file_t_opt=({void**_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[
0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);
_tmp7A;});}return*((void**)_check_null(file_t_opt));}void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;if(void_star_t_opt == 0)void_star_t_opt=({void**
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,
Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmp84;});return*((void**)
_check_null(void_star_t_opt));}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _tagged_arr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_AggrType_struct
_tmp87;_tmp87.tag=10;_tmp87.f1=({struct Cyc_Absyn_AggrInfo _tmp88;_tmp88.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp8A;_tmp8A.tag=0;
_tmp8A.f1=(void*)k;_tmp8A.f2=({struct _tuple0*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B->f1=Cyc_Absyn_rel_ns_null;_tmp8B->f2=name;_tmp8B;});_tmp8A;});_tmp89;}));
_tmp88.targs=0;_tmp88;});_tmp87;});_tmp86;});}void*Cyc_Absyn_strct(struct
_tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp8C=
_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_AggrType_struct _tmp8D;
_tmp8D.tag=10;_tmp8D.f1=({struct Cyc_Absyn_AggrInfo _tmp8E;_tmp8E.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));
_tmp8F[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(
void*)((void*)0);_tmp90.f2=name;_tmp90;});_tmp8F;}));_tmp8E.targs=0;_tmp8E;});
_tmp8D;});_tmp8C;});}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({
struct Cyc_Absyn_AggrType_struct _tmp92;_tmp92.tag=10;_tmp92.f1=({struct Cyc_Absyn_AggrInfo
_tmp93;_tmp93.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp95;_tmp95.tag=0;_tmp95.f1=(void*)((void*)1);_tmp95.f2=name;_tmp95;});_tmp94;}));
_tmp93.targs=0;_tmp93;});_tmp92;});_tmp91;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Exp*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96->topt=0;_tmp96->r=(void*)r;_tmp96->loc=loc;
_tmp96->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp96;});}struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_New_e_struct*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_New_e_struct _tmp98;_tmp98.tag=
15;_tmp98.f1=rgn_handle;_tmp98.f2=e;_tmp98;});_tmp97;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){return({struct Cyc_Absyn_Exp*_tmp99=
_cycalloc(sizeof(*_tmp99));_tmp99[0]=*e;_tmp99;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
void*c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Const_e_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_Const_e_struct
_tmp9B;_tmp9B.tag=0;_tmp9B.f1=(void*)c;_tmp9B;});_tmp9A;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({
struct Cyc_Absyn_Const_e_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(void*)((void*)0);
_tmp9D;});_tmp9C;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_Int_c_struct _tmp9F;
_tmp9F.tag=2;_tmp9F.f1=(void*)s;_tmp9F.f2=i;_tmp9F;});_tmp9E;}),seg);}struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((
void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_Char_c_struct
_tmpA1;_tmpA1.tag=0;_tmpA1.f1=(void*)((void*)2);_tmpA1.f2=c;_tmpA1;});_tmpA0;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Float_c_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_Float_c_struct _tmpA3;
_tmpA3.tag=4;_tmpA3.f1=f;_tmpA3;});_tmpA2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_String_c_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[
0]=({struct Cyc_Absyn_String_c_struct _tmpA5;_tmpA5.tag=5;_tmpA5.f1=s;_tmpA5;});
_tmpA4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpA6=
_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_Var_e_struct _tmpA7;_tmpA7.tag=
1;_tmpA7.f1=q;_tmpA7.f2=(void*)((void*)0);_tmpA7;});_tmpA6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpA8=_cycalloc(sizeof(*
_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Var_e_struct _tmpA9;_tmpA9.tag=1;_tmpA9.f1=q;
_tmpA9.f2=(void*)b;_tmpA9;});_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpAB;_tmpAB.tag=2;_tmpAB.f1=q;_tmpAB;});
_tmpAA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpAD;_tmpAD.tag=3;_tmpAD.f1=(void*)p;_tmpAD.f2=es;_tmpAD;});_tmpAC;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE->hd=e;_tmpAE->tl=0;_tmpAE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpAF=_cycalloc(sizeof(*
_tmpAF));_tmpAF->hd=e1;_tmpAF->tl=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*
_tmpB0));_tmpB0->hd=e2;_tmpB0->tl=0;_tmpB0;});_tmpAF;}),loc);}struct Cyc_Absyn_Exp*
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
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpB2;
_tmpB2.tag=4;_tmpB2.f1=e1;_tmpB2.f2=popt;_tmpB2.f3=e2;_tmpB2;});_tmpB1;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpB4;_tmpB4.tag=5;_tmpB4.f1=e;_tmpB4.f2=(void*)i;_tmpB4;});_tmpB3;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpB6;_tmpB6.tag=6;_tmpB6.f1=e1;_tmpB6.f2=e2;_tmpB6.f3=e3;_tmpB6;});_tmpB5;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,e2,Cyc_Absyn_false_exp(
loc),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,Cyc_Absyn_true_exp(
loc),e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[
0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpB8;_tmpB8.tag=7;_tmpB8.f1=e1;_tmpB8.f2=
e2;_tmpB8;});_tmpB7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpBA;_tmpBA.tag=8;_tmpBA.f1=e;_tmpBA.f2=es;_tmpBA;});_tmpB9;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmpBB=
_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_FnCall_e_struct _tmpBC;
_tmpBC.tag=9;_tmpBC.f1=e;_tmpBC.f2=es;_tmpBC.f3=0;_tmpBC;});_tmpBB;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpBE;_tmpBE.tag=11;_tmpBE.f1=e;_tmpBE;});_tmpBD;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC0;_tmpC0.tag=12;_tmpC0.f1=e;_tmpC0.f2=ts;_tmpC0;});_tmpBF;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpC1=
_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_Cast_e_struct _tmpC2;
_tmpC2.tag=13;_tmpC2.f1=(void*)t;_tmpC2.f2=e;_tmpC2;});_tmpC1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_struct*_tmpC3=_cycalloc(
sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Throw_e_struct _tmpC4;_tmpC4.tag=10;
_tmpC4.f1=e;_tmpC4;});_tmpC3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Address_e_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));
_tmpC5[0]=({struct Cyc_Absyn_Address_e_struct _tmpC6;_tmpC6.tag=14;_tmpC6.f1=e;
_tmpC6;});_tmpC5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpC8;_tmpC8.tag=16;_tmpC8.f1=(void*)t;_tmpC8;});_tmpC7;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmpC9=
_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmpCA;
_tmpCA.tag=17;_tmpCA.f1=e;_tmpCA;});_tmpC9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmpCC;_tmpCC.tag=18;_tmpCC.f1=(void*)t;_tmpCC.f2=(
void*)of;_tmpCC;});_tmpCB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpCE;_tmpCE.tag=19;_tmpCE.f1=tvs;_tmpCE.f2=(
void*)t;_tmpCE;});_tmpCD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD0;_tmpD0.tag=20;_tmpD0.f1=e;_tmpD0;});_tmpCF;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpD2;_tmpD2.tag=21;_tmpD2.f1=e;_tmpD2.f2=n;
_tmpD2;});_tmpD1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));
_tmpD3[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpD4;_tmpD4.tag=22;_tmpD4.f1=e;
_tmpD4.f2=n;_tmpD4;});_tmpD3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpD6;
_tmpD6.tag=23;_tmpD6.f1=e1;_tmpD6.f2=e2;_tmpD6;});_tmpD5;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpD7=_cycalloc(
sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpD8;_tmpD8.tag=24;
_tmpD8.f1=es;_tmpD8;});_tmpD7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpDA;_tmpDA.tag=35;_tmpDA.f1=s;
_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*
_tmpDB));_tmpDB->hd=({struct _tuple12*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=
0;_tmpDC->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpDC;});_tmpDB->tl=dles;_tmpDB;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpDD=_cycalloc(
sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_Array_e_struct _tmpDE;_tmpDE.tag=26;
_tmpDE.f1=dles;_tmpDE;});_tmpDD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpDF=
_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpE0;_tmpE0.tag=34;_tmpE0.f1=n;_tmpE0.f2=dles;_tmpE0;});_tmpDF;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->r=(void*)s;_tmpE1->loc=
loc;_tmpE1->non_local_preds=0;_tmpE1->try_depth=0;_tmpE1->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpE1;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_Exp_s_struct _tmpE3;_tmpE3.tag=0;
_tmpE3.f1=e;_tmpE3;});_tmpE2;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpE4=
_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_Return_s_struct _tmpE5;
_tmpE5.tag=2;_tmpE5.f1=e;_tmpE5;});_tmpE4;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpE7;_tmpE7.tag=3;_tmpE7.f1=e;_tmpE7.f2=s1;_tmpE7.f3=s2;_tmpE7;});_tmpE6;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_While_s_struct
_tmpE9;_tmpE9.tag=4;_tmpE9.f1=({struct _tuple2 _tmpEA;_tmpEA.f1=e;_tmpEA.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpEA;});_tmpE9.f2=s;_tmpE9;});_tmpE8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Break_s_struct
_tmpEC;_tmpEC.tag=5;_tmpEC.f1=0;_tmpEC;});_tmpEB;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpED=_cycalloc(sizeof(*_tmpED));
_tmpED[0]=({struct Cyc_Absyn_Continue_s_struct _tmpEE;_tmpEE.tag=6;_tmpEE.f1=0;
_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_For_s_struct _tmpF0;
_tmpF0.tag=8;_tmpF0.f1=e1;_tmpF0.f2=({struct _tuple2 _tmpF1;_tmpF1.f1=e2;_tmpF1.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpF1;});_tmpF0.f3=({struct _tuple2 _tmpF2;_tmpF2.f1=
e3;_tmpF2.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpF2;});_tmpF0.f4=s;_tmpF0;});_tmpEF;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpF4;_tmpF4.tag=9;_tmpF4.f1=e;_tmpF4.f2=scs;_tmpF4;});_tmpF3;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpF6=({struct _tuple9 _tmpF5;
_tmpF5.f1=(void*)s1->r;_tmpF5.f2=(void*)s2->r;_tmpF5;});void*_tmpF7;void*_tmpF8;
_LL3C: _tmpF7=_tmpF6.f1;if((int)_tmpF7 != 0)goto _LL3E;_LL3D: return s2;_LL3E: _tmpF8=
_tmpF6.f2;if((int)_tmpF8 != 0)goto _LL40;_LL3F: return s1;_LL40:;_LL41: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpFA;_tmpFA.tag=1;_tmpFA.f1=s1;_tmpFA.f2=s2;_tmpFA;});
_tmpF9;}),loc);_LL3B:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmpFC;_tmpFC.tag=11;_tmpFC.f1=el;_tmpFC.f2=0;_tmpFC;});_tmpFB;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpFE;_tmpFE.tag=12;_tmpFE.f1=d;_tmpFE.f2=s;_tmpFE;});_tmpFD;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[
0]=({struct Cyc_Absyn_Var_d_struct _tmp102;_tmp102.tag=0;_tmp102.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmp102;});_tmp101;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp100;_tmp100.tag=12;_tmp100.f1=d;_tmp100.f2=s;_tmp100;});_tmpFF;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmp103=
_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Cut_s_struct _tmp104;
_tmp104.tag=13;_tmp104.f1=s;_tmp104;});_tmp103;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmp105=_cycalloc(sizeof(*_tmp105));
_tmp105[0]=({struct Cyc_Absyn_Splice_s_struct _tmp106;_tmp106.tag=14;_tmp106.f1=s;
_tmp106;});_tmp105;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct
_tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp107=_cycalloc(sizeof(*_tmp107));
_tmp107[0]=({struct Cyc_Absyn_Label_s_struct _tmp108;_tmp108.tag=15;_tmp108.f1=v;
_tmp108.f2=s;_tmp108;});_tmp107;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp109=_cycalloc(
sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Do_s_struct _tmp10A;_tmp10A.tag=16;
_tmp10A.f1=s;_tmp10A.f2=({struct _tuple2 _tmp10B;_tmp10B.f1=e;_tmp10B.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmp10B;});_tmp10A;});_tmp109;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp10D;_tmp10D.tag=17;_tmp10D.f1=s;_tmp10D.f2=scs;_tmp10D;});_tmp10C;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Goto_s_struct _tmp10F;
_tmp10F.tag=7;_tmp10F.f1=lab;_tmp10F.f2=0;_tmp10F;});_tmp10E;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_forarray_stmt(struct Cyc_List_List*d,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ForArray_s_struct*_tmp110=_cycalloc(sizeof(*_tmp110));
_tmp110[0]=({struct Cyc_Absyn_ForArray_s_struct _tmp111;_tmp111.tag=19;_tmp111.f1=({
struct Cyc_Absyn_ForArrayInfo _tmp112;_tmp112.defns=d;_tmp112.condition=({struct
_tuple2 _tmp114;_tmp114.f1=e1;_tmp114.f2=Cyc_Absyn_skip_stmt(e1->loc);_tmp114;});
_tmp112.delta=({struct _tuple2 _tmp113;_tmp113.f1=e2;_tmp113.f2=Cyc_Absyn_skip_stmt(
e2->loc);_tmp113;});_tmp112.body=s;_tmp112;});_tmp111;});_tmp110;}),loc);}struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({
struct Cyc_Absyn_Pat*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->r=(void*)p;
_tmp115->topt=0;_tmp115->loc=s;_tmp115;});}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116->r=(void*)r;_tmp116->loc=loc;_tmp116;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=({struct Cyc_Absyn_Let_d_struct
_tmp118;_tmp118.tag=2;_tmp118.f1=p;_tmp118.f2=0;_tmp118.f3=e;_tmp118;});_tmp117;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp119=
_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_Letv_d_struct _tmp11A;
_tmp11A.tag=3;_tmp11A.f1=vds;_tmp11A;});_tmp119;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->sc=(void*)((
void*)2);_tmp11B->name=x;_tmp11B->tq=Cyc_Absyn_empty_tqual();_tmp11B->type=(void*)
t;_tmp11B->initializer=init;_tmp11B->rgn=0;_tmp11B->attributes=0;_tmp11B->escapes=
0;_tmp11B;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp11C=
_cycalloc(sizeof(*_tmp11C));_tmp11C->sc=(void*)((void*)0);_tmp11C->name=x;
_tmp11C->tq=Cyc_Absyn_empty_tqual();_tmp11C->type=(void*)t;_tmp11C->initializer=
init;_tmp11C->rgn=0;_tmp11C->attributes=0;_tmp11C->escapes=0;_tmp11C;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->exist_vars=exists;_tmp11D->rgn_po=
po;_tmp11D->fields=fs;_tmp11D;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));
_tmp11E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp11F;_tmp11F.tag=4;_tmp11F.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->kind=(void*)
k;_tmp120->sc=(void*)s;_tmp120->name=n;_tmp120->tvs=ts;_tmp120->impl=i;_tmp120->attributes=
atts;_tmp120;});_tmp11F;});_tmp11E;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp122;
_tmp122.tag=5;_tmp122.f1=({struct Cyc_Absyn_Tuniondecl*_tmp123=_cycalloc(sizeof(*
_tmp123));_tmp123->sc=(void*)s;_tmp123->name=n;_tmp123->tvs=ts;_tmp123->fields=
fs;_tmp123->is_xtunion=is_xtunion;_tmp123;});_tmp122;});_tmp121;}),loc);}static
struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){return({struct _tuple1*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=(*a).f1;_tmp124->f2=(*a).f2;
_tmp124->f3=Cyc_Absyn_pointer_expand((*a).f3);_tmp124;});}void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts){return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Absyn_FnType_struct
_tmp126;_tmp126.tag=8;_tmp126.f1=({struct Cyc_Absyn_FnInfo _tmp127;_tmp127.tvars=
tvs;_tmp127.ret_typ=(void*)Cyc_Absyn_pointer_expand(ret_typ);_tmp127.effect=
eff_typ;_tmp127.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple1*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args);_tmp127.c_varargs=
c_varargs;_tmp127.cyc_varargs=cyc_varargs;_tmp127.rgn_po=rgn_po;_tmp127.attributes=
atts;_tmp127;});_tmp126;});_tmp125;});}void*Cyc_Absyn_pointer_expand(void*t){
void*_tmp128=Cyc_Tcutil_compress(t);_LL43: if(_tmp128 <= (void*)3?1:*((int*)
_tmp128)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(t,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_false_conref);_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp129=(void*)e->r;void*_tmp12A;void*_tmp12B;struct
Cyc_Absyn_Vardecl*_tmp12C;void*_tmp12D;struct Cyc_Absyn_Vardecl*_tmp12E;struct Cyc_Absyn_Exp*
_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp131;_LL48: if(*((int*)
_tmp129)!= 1)goto _LL4A;_tmp12A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp129)->f2;
if(_tmp12A <= (void*)1?1:*((int*)_tmp12A)!= 1)goto _LL4A;_LL49: return 0;_LL4A: if(*((
int*)_tmp129)!= 1)goto _LL4C;_tmp12B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp129)->f2;if(_tmp12B <= (void*)1?1:*((int*)_tmp12B)!= 0)goto _LL4C;_tmp12C=((
struct Cyc_Absyn_Global_b_struct*)_tmp12B)->f1;_LL4B: _tmp12E=_tmp12C;goto _LL4D;
_LL4C: if(*((int*)_tmp129)!= 1)goto _LL4E;_tmp12D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp129)->f2;if(_tmp12D <= (void*)1?1:*((int*)_tmp12D)!= 3)goto _LL4E;_tmp12E=((
struct Cyc_Absyn_Local_b_struct*)_tmp12D)->f1;_LL4D: {void*_tmp132=Cyc_Tcutil_compress((
void*)_tmp12E->type);_LL5F: if(_tmp132 <= (void*)3?1:*((int*)_tmp132)!= 7)goto
_LL61;_LL60: return 0;_LL61:;_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp129)!= 1)
goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp129)!= 22)goto _LL52;_LL51: goto
_LL53;_LL52: if(*((int*)_tmp129)!= 20)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)
_tmp129)!= 23)goto _LL56;_LL55: return 1;_LL56: if(*((int*)_tmp129)!= 21)goto _LL58;
_tmp12F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp129)->f1;_LL57: return Cyc_Absyn_is_lvalue(
_tmp12F);_LL58: if(*((int*)_tmp129)!= 12)goto _LL5A;_tmp130=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp129)->f1;_LL59: return Cyc_Absyn_is_lvalue(_tmp130);_LL5A: if(*((int*)_tmp129)
!= 11)goto _LL5C;_tmp131=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp129)->f1;
_LL5B: return Cyc_Absyn_is_lvalue(_tmp131);_LL5C:;_LL5D: return 0;_LL47:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _tagged_arr*v){{struct
Cyc_List_List*_tmp133=fields;for(0;_tmp133 != 0;_tmp133=_tmp133->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp133->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp133->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _tagged_arr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple3*)((struct _tuple3*)
ts->hd);}struct _tagged_arr Cyc_Absyn_attribute2string(void*a){void*_tmp134=a;int
_tmp135;int _tmp136;struct _tagged_arr _tmp137;void*_tmp138;int _tmp139;int _tmp13A;
void*_tmp13B;int _tmp13C;int _tmp13D;int _tmp13E;_LL64: if(_tmp134 <= (void*)17?1:*((
int*)_tmp134)!= 0)goto _LL66;_tmp135=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp134)->f1;_LL65: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp141;
_tmp141.tag=1;_tmp141.f1=(unsigned int)_tmp135;{void*_tmp13F[1]={& _tmp141};Cyc_aprintf(({
const char*_tmp140="regparm(%d)";_tag_arr(_tmp140,sizeof(char),_get_zero_arr_size(
_tmp140,12));}),_tag_arr(_tmp13F,sizeof(void*),1));}});_LL66: if((int)_tmp134 != 0)
goto _LL68;_LL67: return({const char*_tmp142="stdcall";_tag_arr(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,8));});_LL68: if((int)_tmp134 != 1)goto _LL6A;_LL69:
return({const char*_tmp143="cdecl";_tag_arr(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,6));});_LL6A: if((int)_tmp134 != 2)goto _LL6C;_LL6B:
return({const char*_tmp144="fastcall";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,9));});_LL6C: if((int)_tmp134 != 3)goto _LL6E;_LL6D:
return({const char*_tmp145="noreturn";_tag_arr(_tmp145,sizeof(char),
_get_zero_arr_size(_tmp145,9));});_LL6E: if((int)_tmp134 != 4)goto _LL70;_LL6F:
return({const char*_tmp146="const";_tag_arr(_tmp146,sizeof(char),
_get_zero_arr_size(_tmp146,6));});_LL70: if(_tmp134 <= (void*)17?1:*((int*)_tmp134)
!= 1)goto _LL72;_tmp136=((struct Cyc_Absyn_Aligned_att_struct*)_tmp134)->f1;_LL71:
if(_tmp136 == - 1)return({const char*_tmp147="aligned";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,8));});else{return(struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp14A;_tmp14A.tag=1;_tmp14A.f1=(unsigned int)_tmp136;{void*_tmp148[1]={&
_tmp14A};Cyc_aprintf(({const char*_tmp149="aligned(%d)";_tag_arr(_tmp149,sizeof(
char),_get_zero_arr_size(_tmp149,12));}),_tag_arr(_tmp148,sizeof(void*),1));}});}
_LL72: if((int)_tmp134 != 5)goto _LL74;_LL73: return({const char*_tmp14B="packed";
_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,7));});_LL74: if(_tmp134
<= (void*)17?1:*((int*)_tmp134)!= 2)goto _LL76;_tmp137=((struct Cyc_Absyn_Section_att_struct*)
_tmp134)->f1;_LL75: return(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp14E;
_tmp14E.tag=0;_tmp14E.f1=(struct _tagged_arr)((struct _tagged_arr)_tmp137);{void*
_tmp14C[1]={& _tmp14E};Cyc_aprintf(({const char*_tmp14D="section(\"%s\")";_tag_arr(
_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,14));}),_tag_arr(_tmp14C,sizeof(
void*),1));}});_LL76: if((int)_tmp134 != 6)goto _LL78;_LL77: return({const char*
_tmp14F="nocommon";_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,9));});
_LL78: if((int)_tmp134 != 7)goto _LL7A;_LL79: return({const char*_tmp150="shared";
_tag_arr(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,7));});_LL7A: if((int)
_tmp134 != 8)goto _LL7C;_LL7B: return({const char*_tmp151="unused";_tag_arr(_tmp151,
sizeof(char),_get_zero_arr_size(_tmp151,7));});_LL7C: if((int)_tmp134 != 9)goto
_LL7E;_LL7D: return({const char*_tmp152="weak";_tag_arr(_tmp152,sizeof(char),
_get_zero_arr_size(_tmp152,5));});_LL7E: if((int)_tmp134 != 10)goto _LL80;_LL7F:
return({const char*_tmp153="dllimport";_tag_arr(_tmp153,sizeof(char),
_get_zero_arr_size(_tmp153,10));});_LL80: if((int)_tmp134 != 11)goto _LL82;_LL81:
return({const char*_tmp154="dllexport";_tag_arr(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,10));});_LL82: if((int)_tmp134 != 12)goto _LL84;_LL83:
return({const char*_tmp155="no_instrument_function";_tag_arr(_tmp155,sizeof(char),
_get_zero_arr_size(_tmp155,23));});_LL84: if((int)_tmp134 != 13)goto _LL86;_LL85:
return({const char*_tmp156="constructor";_tag_arr(_tmp156,sizeof(char),
_get_zero_arr_size(_tmp156,12));});_LL86: if((int)_tmp134 != 14)goto _LL88;_LL87:
return({const char*_tmp157="destructor";_tag_arr(_tmp157,sizeof(char),
_get_zero_arr_size(_tmp157,11));});_LL88: if((int)_tmp134 != 15)goto _LL8A;_LL89:
return({const char*_tmp158="no_check_memory_usage";_tag_arr(_tmp158,sizeof(char),
_get_zero_arr_size(_tmp158,22));});_LL8A: if(_tmp134 <= (void*)17?1:*((int*)
_tmp134)!= 3)goto _LL8C;_tmp138=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp134)->f1;if((int)_tmp138 != 0)goto _LL8C;_tmp139=((struct Cyc_Absyn_Format_att_struct*)
_tmp134)->f2;_tmp13A=((struct Cyc_Absyn_Format_att_struct*)_tmp134)->f3;_LL8B:
return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp15C;_tmp15C.tag=1;_tmp15C.f1=(
unsigned int)_tmp13A;{struct Cyc_Int_pa_struct _tmp15B;_tmp15B.tag=1;_tmp15B.f1=(
unsigned int)_tmp139;{void*_tmp159[2]={& _tmp15B,& _tmp15C};Cyc_aprintf(({const
char*_tmp15A="format(printf,%u,%u)";_tag_arr(_tmp15A,sizeof(char),
_get_zero_arr_size(_tmp15A,21));}),_tag_arr(_tmp159,sizeof(void*),2));}}});_LL8C:
if(_tmp134 <= (void*)17?1:*((int*)_tmp134)!= 3)goto _LL8E;_tmp13B=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp134)->f1;if((int)_tmp13B != 1)goto _LL8E;_tmp13C=((
struct Cyc_Absyn_Format_att_struct*)_tmp134)->f2;_tmp13D=((struct Cyc_Absyn_Format_att_struct*)
_tmp134)->f3;_LL8D: return(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp160;
_tmp160.tag=1;_tmp160.f1=(unsigned int)_tmp13D;{struct Cyc_Int_pa_struct _tmp15F;
_tmp15F.tag=1;_tmp15F.f1=(unsigned int)_tmp13C;{void*_tmp15D[2]={& _tmp15F,&
_tmp160};Cyc_aprintf(({const char*_tmp15E="format(scanf,%u,%u)";_tag_arr(_tmp15E,
sizeof(char),_get_zero_arr_size(_tmp15E,20));}),_tag_arr(_tmp15D,sizeof(void*),2));}}});
_LL8E: if(_tmp134 <= (void*)17?1:*((int*)_tmp134)!= 4)goto _LL90;_tmp13E=((struct
Cyc_Absyn_Initializes_att_struct*)_tmp134)->f1;_LL8F: return(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp163;_tmp163.tag=1;_tmp163.f1=(unsigned int)_tmp13E;{
void*_tmp161[1]={& _tmp163};Cyc_aprintf(({const char*_tmp162="initializes(%d)";
_tag_arr(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,16));}),_tag_arr(
_tmp161,sizeof(void*),1));}});_LL90: if((int)_tmp134 != 16)goto _LL63;_LL91: return({
const char*_tmp164="pure";_tag_arr(_tmp164,sizeof(char),_get_zero_arr_size(
_tmp164,5));});_LL63:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp165=a;_LL93: if(
_tmp165 <= (void*)17?1:*((int*)_tmp165)!= 0)goto _LL95;_LL94: goto _LL96;_LL95: if((
int)_tmp165 != 2)goto _LL97;_LL96: goto _LL98;_LL97: if((int)_tmp165 != 0)goto _LL99;
_LL98: goto _LL9A;_LL99: if((int)_tmp165 != 1)goto _LL9B;_LL9A: goto _LL9C;_LL9B: if((
int)_tmp165 != 3)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if((int)_tmp165 != 16)goto _LL9F;
_LL9E: goto _LLA0;_LL9F: if(_tmp165 <= (void*)17?1:*((int*)_tmp165)!= 3)goto _LLA1;
_LLA0: goto _LLA2;_LLA1: if((int)_tmp165 != 4)goto _LLA3;_LLA2: return 1;_LLA3: if(
_tmp165 <= (void*)17?1:*((int*)_tmp165)!= 4)goto _LLA5;_LLA4: return 1;_LLA5:;_LLA6:
return 0;_LL92:;}static char _tmp166[3]="f0";static struct _tagged_arr Cyc_Absyn_f0={
_tmp166,_tmp166,_tmp166 + 3};static struct _tagged_arr*Cyc_Absyn_field_names_v[1]={&
Cyc_Absyn_f0};static struct _tagged_arr Cyc_Absyn_field_names={(void*)((struct
_tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(
int i){unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp167=(
unsigned int)(i + 1);struct _tagged_arr**_tmp168=(struct _tagged_arr**)_cycalloc(
_check_times(sizeof(struct _tagged_arr*),_tmp167));struct _tagged_arr _tmp16E=
_tag_arr(_tmp168,sizeof(struct _tagged_arr*),_tmp167);{unsigned int _tmp169=
_tmp167;unsigned int j;for(j=0;j < _tmp169;j ++){_tmp168[j]=j < fsz?*((struct
_tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),(int)j)):({struct _tagged_arr*_tmp16A=_cycalloc(sizeof(*_tmp16A));
_tmp16A[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp16D;_tmp16D.tag=1;
_tmp16D.f1=(unsigned int)((int)j);{void*_tmp16B[1]={& _tmp16D};Cyc_aprintf(({
const char*_tmp16C="f%d";_tag_arr(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,
4));}),_tag_arr(_tmp16B,sizeof(void*),1));}});_tmp16A;});}}_tmp16E;});return*((
struct _tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*info){void*
_tmp16F=info;void*_tmp170;struct _tuple0*_tmp171;struct Cyc_Absyn_Aggrdecl**
_tmp172;struct Cyc_Absyn_Aggrdecl*_tmp173;struct Cyc_Absyn_Aggrdecl _tmp174;void*
_tmp175;struct _tuple0*_tmp176;_LLA8: if(*((int*)_tmp16F)!= 0)goto _LLAA;_tmp170=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp16F)->f1;_tmp171=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp16F)->f2;_LLA9: return({struct _tuple4 _tmp177;_tmp177.f1=_tmp170;_tmp177.f2=
_tmp171;_tmp177;});_LLAA: if(*((int*)_tmp16F)!= 1)goto _LLA7;_tmp172=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp16F)->f1;_tmp173=*_tmp172;_tmp174=*_tmp173;_tmp175=(void*)_tmp174.kind;
_tmp176=_tmp174.name;_LLAB: return({struct _tuple4 _tmp178;_tmp178.f1=_tmp175;
_tmp178.f2=_tmp176;_tmp178;});_LLA7:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info){void*_tmp179=info;void*_tmp17A;struct _tuple0*_tmp17B;struct Cyc_Absyn_Aggrdecl**
_tmp17C;struct Cyc_Absyn_Aggrdecl*_tmp17D;_LLAD: if(*((int*)_tmp179)!= 0)goto _LLAF;
_tmp17A=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp179)->f1;_tmp17B=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp179)->f2;_LLAE:({void*_tmp17E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp17F="unchecked aggrdecl";_tag_arr(_tmp17F,sizeof(char),_get_zero_arr_size(
_tmp17F,19));}),_tag_arr(_tmp17E,sizeof(void*),0));});_LLAF: if(*((int*)_tmp179)
!= 1)goto _LLAC;_tmp17C=((struct Cyc_Absyn_KnownAggr_struct*)_tmp179)->f1;_tmp17D=*
_tmp17C;_LLB0: return _tmp17D;_LLAC:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp180=Cyc_Tcutil_compress(t);void*_tmp181;struct Cyc_Absyn_AggrInfo _tmp182;void*
_tmp183;_LLB2: if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 11)goto _LLB4;_tmp181=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp180)->f1;if((int)_tmp181 != 1)
goto _LLB4;_LLB3: return 1;_LLB4: if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 10)goto
_LLB6;_tmp182=((struct Cyc_Absyn_AggrType_struct*)_tmp180)->f1;_tmp183=(void*)
_tmp182.aggr_info;_LLB5: return(Cyc_Absyn_aggr_kinded_name(_tmp183)).f1 == (void*)
1;_LLB6:;_LLB7: return 0;_LLB1:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp184[0]={};Cyc_printf(({const char*_tmp185="\n";_tag_arr(_tmp185,
sizeof(char),_get_zero_arr_size(_tmp185,2));}),_tag_arr(_tmp184,sizeof(void*),0));});}
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_403;static struct Cyc_Typerep_Int_struct Cyc__genrep_5={0,0,32};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_133;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Vardecl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;
static char _tmp187[7]="Static";static struct _tuple7 Cyc__gentuple_137={0,{_tmp187,
_tmp187,_tmp187 + 7}};static char _tmp188[9]="Abstract";static struct _tuple7 Cyc__gentuple_138={
1,{_tmp188,_tmp188,_tmp188 + 9}};static char _tmp189[7]="Public";static struct
_tuple7 Cyc__gentuple_139={2,{_tmp189,_tmp189,_tmp189 + 7}};static char _tmp18A[7]="Extern";
static struct _tuple7 Cyc__gentuple_140={3,{_tmp18A,_tmp18A,_tmp18A + 7}};static char
_tmp18B[8]="ExternC";static struct _tuple7 Cyc__gentuple_141={4,{_tmp18B,_tmp18B,
_tmp18B + 8}};static char _tmp18C[9]="Register";static struct _tuple7 Cyc__gentuple_142={
5,{_tmp18C,_tmp18C,_tmp18C + 9}};static struct _tuple7*Cyc__genarr_143[6]={& Cyc__gentuple_137,&
Cyc__gentuple_138,& Cyc__gentuple_139,& Cyc__gentuple_140,& Cyc__gentuple_141,& Cyc__gentuple_142};
static struct _tuple5*Cyc__genarr_144[0]={};static char _tmp18E[6]="Scope";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_scope_t_rep={5,{_tmp18E,_tmp18E,_tmp18E + 6},{(void*)((struct _tuple7**)
Cyc__genarr_143),(void*)((struct _tuple7**)Cyc__genarr_143),(void*)((struct
_tuple7**)Cyc__genarr_143 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_144),(void*)((
struct _tuple5**)Cyc__genarr_144),(void*)((struct _tuple5**)Cyc__genarr_144 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_11;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_17;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_18;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_13;static struct Cyc_Typerep_Int_struct Cyc__genrep_14={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_13={2,(void*)((void*)& Cyc__genrep_14)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12={1,1,(void*)((void*)& Cyc__genrep_13)};
static char _tmp192[5]="List";static struct _tagged_arr Cyc__genname_22={_tmp192,
_tmp192,_tmp192 + 5};static char _tmp193[3]="hd";static struct _tuple5 Cyc__gentuple_19={
offsetof(struct Cyc_List_List,hd),{_tmp193,_tmp193,_tmp193 + 3},(void*)& Cyc__genrep_12};
static char _tmp194[3]="tl";static struct _tuple5 Cyc__gentuple_20={offsetof(struct
Cyc_List_List,tl),{_tmp194,_tmp194,_tmp194 + 3},(void*)& Cyc__genrep_18};static
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
static char _tmp198[6]="Loc_n";static struct _tuple7 Cyc__gentuple_15={0,{_tmp198,
_tmp198,_tmp198 + 6}};static struct _tuple7*Cyc__genarr_16[1]={& Cyc__gentuple_15};
static char _tmp199[6]="Rel_n";static struct _tuple5 Cyc__gentuple_26={0,{_tmp199,
_tmp199,_tmp199 + 6},(void*)& Cyc__genrep_17};static char _tmp19A[6]="Abs_n";static
struct _tuple5 Cyc__gentuple_27={1,{_tmp19A,_tmp19A,_tmp19A + 6},(void*)& Cyc__genrep_17};
static struct _tuple5*Cyc__genarr_28[2]={& Cyc__gentuple_26,& Cyc__gentuple_27};
static char _tmp19C[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={
5,{_tmp19C,_tmp19C,_tmp19C + 8},{(void*)((struct _tuple7**)Cyc__genarr_16),(void*)((
struct _tuple7**)Cyc__genarr_16),(void*)((struct _tuple7**)Cyc__genarr_16 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_28),(void*)((struct _tuple5**)Cyc__genarr_28),(
void*)((struct _tuple5**)Cyc__genarr_28 + 2)}};static struct _tuple6 Cyc__gentuple_29={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_30={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_12};static
struct _tuple6*Cyc__genarr_31[2]={& Cyc__gentuple_29,& Cyc__gentuple_30};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_11={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_31),(void*)((struct _tuple6**)Cyc__genarr_31),(void*)((
struct _tuple6**)Cyc__genarr_31 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10={
1,1,(void*)((void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_134;
struct _tuple14{char f1;};static struct _tuple6 Cyc__gentuple_135={offsetof(struct
_tuple14,f1),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_136[
1]={& Cyc__gentuple_135};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_134={4,
sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_136),(void*)((
struct _tuple6**)Cyc__genarr_136),(void*)((struct _tuple6**)Cyc__genarr_136 + 1)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1071;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1076;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1A0[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_188={0,{_tmp1A0,_tmp1A0,_tmp1A0 + 8}};static char
_tmp1A1[8]="MemKind";static struct _tuple7 Cyc__gentuple_189={1,{_tmp1A1,_tmp1A1,
_tmp1A1 + 8}};static char _tmp1A2[8]="BoxKind";static struct _tuple7 Cyc__gentuple_190={
2,{_tmp1A2,_tmp1A2,_tmp1A2 + 8}};static char _tmp1A3[8]="RgnKind";static struct
_tuple7 Cyc__gentuple_191={3,{_tmp1A3,_tmp1A3,_tmp1A3 + 8}};static char _tmp1A4[8]="EffKind";
static struct _tuple7 Cyc__gentuple_192={4,{_tmp1A4,_tmp1A4,_tmp1A4 + 8}};static char
_tmp1A5[8]="IntKind";static struct _tuple7 Cyc__gentuple_193={5,{_tmp1A5,_tmp1A5,
_tmp1A5 + 8}};static struct _tuple7*Cyc__genarr_194[6]={& Cyc__gentuple_188,& Cyc__gentuple_189,&
Cyc__gentuple_190,& Cyc__gentuple_191,& Cyc__gentuple_192,& Cyc__gentuple_193};
static struct _tuple5*Cyc__genarr_195[0]={};static char _tmp1A7[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,{_tmp1A7,_tmp1A7,_tmp1A7 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_194),(void*)((struct _tuple7**)Cyc__genarr_194),(void*)((struct
_tuple7**)Cyc__genarr_194 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_195),(void*)((
struct _tuple5**)Cyc__genarr_195),(void*)((struct _tuple5**)Cyc__genarr_195 + 0)}};
static char _tmp1A8[4]="Opt";static struct _tagged_arr Cyc__genname_1079={_tmp1A8,
_tmp1A8,_tmp1A8 + 4};static char _tmp1A9[2]="v";static struct _tuple5 Cyc__gentuple_1077={
offsetof(struct Cyc_Core_Opt,v),{_tmp1A9,_tmp1A9,_tmp1A9 + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_1078[1]={& Cyc__gentuple_1077};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1079,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1078),(void*)((
struct _tuple5**)Cyc__genarr_1078),(void*)((struct _tuple5**)Cyc__genarr_1078 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1076={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_43;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1AC[4]="Opt";static struct
_tagged_arr Cyc__genname_1111={_tmp1AC,_tmp1AC,_tmp1AC + 4};static char _tmp1AD[2]="v";
static struct _tuple5 Cyc__gentuple_1109={offsetof(struct Cyc_Core_Opt,v),{_tmp1AD,
_tmp1AD,_tmp1AD + 2},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1110[
1]={& Cyc__gentuple_1109};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1111,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1110),(void*)((struct _tuple5**)Cyc__genarr_1110),(
void*)((struct _tuple5**)Cyc__genarr_1110 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_43={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
static struct Cyc_Typerep_Int_struct Cyc__genrep_62={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1072;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_297;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_185;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_216;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_216={1,1,(void*)((void*)& Cyc__genrep_62)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_208;
static struct _tuple6 Cyc__gentuple_209={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_210={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_211[2]={& Cyc__gentuple_209,& Cyc__gentuple_210};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_208={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_211),(void*)((struct _tuple6**)Cyc__genarr_211),(
void*)((struct _tuple6**)Cyc__genarr_211 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_204;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static
char _tmp1B3[4]="Opt";static struct _tagged_arr Cyc__genname_199={_tmp1B3,_tmp1B3,
_tmp1B3 + 4};static char _tmp1B4[2]="v";static struct _tuple5 Cyc__gentuple_197={
offsetof(struct Cyc_Core_Opt,v),{_tmp1B4,_tmp1B4,_tmp1B4 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_198[1]={& Cyc__gentuple_197};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_199,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_198),(void*)((
struct _tuple5**)Cyc__genarr_198),(void*)((struct _tuple5**)Cyc__genarr_198 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_205={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_206={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_196};static struct _tuple6*Cyc__genarr_207[
2]={& Cyc__gentuple_205,& Cyc__gentuple_206};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_204={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_207),(
void*)((struct _tuple6**)Cyc__genarr_207),(void*)((struct _tuple6**)Cyc__genarr_207
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_187;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_200={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_201={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_196};static struct _tuple6 Cyc__gentuple_202={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_203[3]={& Cyc__gentuple_200,& Cyc__gentuple_201,& Cyc__gentuple_202};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_187={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_203),(void*)((struct _tuple6**)Cyc__genarr_203),(
void*)((struct _tuple6**)Cyc__genarr_203 + 3)}};static struct _tuple7*Cyc__genarr_186[
0]={};static char _tmp1B9[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_212={0,{
_tmp1B9,_tmp1B9,_tmp1B9 + 6},(void*)& Cyc__genrep_208};static char _tmp1BA[11]="Unknown_kb";
static struct _tuple5 Cyc__gentuple_213={1,{_tmp1BA,_tmp1BA,_tmp1BA + 11},(void*)&
Cyc__genrep_204};static char _tmp1BB[8]="Less_kb";static struct _tuple5 Cyc__gentuple_214={
2,{_tmp1BB,_tmp1BB,_tmp1BB + 8},(void*)& Cyc__genrep_187};static struct _tuple5*Cyc__genarr_215[
3]={& Cyc__gentuple_212,& Cyc__gentuple_213,& Cyc__gentuple_214};static char _tmp1BD[
10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,{
_tmp1BD,_tmp1BD,_tmp1BD + 10},{(void*)((struct _tuple7**)Cyc__genarr_186),(void*)((
struct _tuple7**)Cyc__genarr_186),(void*)((struct _tuple7**)Cyc__genarr_186 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_215),(void*)((struct _tuple5**)Cyc__genarr_215),(
void*)((struct _tuple5**)Cyc__genarr_215 + 3)}};static char _tmp1BE[5]="Tvar";static
struct _tagged_arr Cyc__genname_221={_tmp1BE,_tmp1BE,_tmp1BE + 5};static char _tmp1BF[
5]="name";static struct _tuple5 Cyc__gentuple_217={offsetof(struct Cyc_Absyn_Tvar,name),{
_tmp1BF,_tmp1BF,_tmp1BF + 5},(void*)& Cyc__genrep_12};static char _tmp1C0[9]="identity";
static struct _tuple5 Cyc__gentuple_218={offsetof(struct Cyc_Absyn_Tvar,identity),{
_tmp1C0,_tmp1C0,_tmp1C0 + 9},(void*)& Cyc__genrep_216};static char _tmp1C1[5]="kind";
static struct _tuple5 Cyc__gentuple_219={offsetof(struct Cyc_Absyn_Tvar,kind),{
_tmp1C1,_tmp1C1,_tmp1C1 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_220[3]={& Cyc__gentuple_217,& Cyc__gentuple_218,& Cyc__gentuple_219};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_221,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple5**)Cyc__genarr_220),(
void*)((struct _tuple5**)Cyc__genarr_220),(void*)((struct _tuple5**)Cyc__genarr_220
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static char _tmp1C4[5]="List";static struct _tagged_arr
Cyc__genname_301={_tmp1C4,_tmp1C4,_tmp1C4 + 5};static char _tmp1C5[3]="hd";static
struct _tuple5 Cyc__gentuple_298={offsetof(struct Cyc_List_List,hd),{_tmp1C5,
_tmp1C5,_tmp1C5 + 3},(void*)& Cyc__genrep_185};static char _tmp1C6[3]="tl";static
struct _tuple5 Cyc__gentuple_299={offsetof(struct Cyc_List_List,tl),{_tmp1C6,
_tmp1C6,_tmp1C6 + 3},(void*)& Cyc__genrep_297};static struct _tuple5*Cyc__genarr_300[
2]={& Cyc__gentuple_298,& Cyc__gentuple_299};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_301,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_300),(void*)((struct _tuple5**)Cyc__genarr_300),(void*)((
struct _tuple5**)Cyc__genarr_300 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_297={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1C9[4]="Opt";static struct _tagged_arr Cyc__genname_1075={_tmp1C9,_tmp1C9,
_tmp1C9 + 4};static char _tmp1CA[2]="v";static struct _tuple5 Cyc__gentuple_1073={
offsetof(struct Cyc_Core_Opt,v),{_tmp1CA,_tmp1CA,_tmp1CA + 2},(void*)& Cyc__genrep_297};
static struct _tuple5*Cyc__genarr_1074[1]={& Cyc__gentuple_1073};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_1075,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1074),(
void*)((struct _tuple5**)Cyc__genarr_1074),(void*)((struct _tuple5**)Cyc__genarr_1074
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1072={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1080={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1081={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1076};static struct _tuple6 Cyc__gentuple_1082={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_43};static struct _tuple6 Cyc__gentuple_1083={
offsetof(struct _tuple17,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_1084={offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1072};static
struct _tuple6*Cyc__genarr_1085[5]={& Cyc__gentuple_1080,& Cyc__gentuple_1081,& Cyc__gentuple_1082,&
Cyc__gentuple_1083,& Cyc__gentuple_1084};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1071={
4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1085),(void*)((
struct _tuple6**)Cyc__genarr_1085),(void*)((struct _tuple6**)Cyc__genarr_1085 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1067;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1068={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1069={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_185};static struct _tuple6*Cyc__genarr_1070[
2]={& Cyc__gentuple_1068,& Cyc__gentuple_1069};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1067={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1070),(
void*)((struct _tuple6**)Cyc__genarr_1070),(void*)((struct _tuple6**)Cyc__genarr_1070
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1041;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1048;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1CF[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1052={_tmp1CF,_tmp1CF,_tmp1CF + 18};static
char _tmp1D0[5]="name";static struct _tuple5 Cyc__gentuple_1049={offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),{
_tmp1D0,_tmp1D0,_tmp1D0 + 5},(void*)& Cyc__genrep_10};static char _tmp1D1[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1050={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),{
_tmp1D1,_tmp1D1,_tmp1D1 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1051[2]={& Cyc__gentuple_1049,& Cyc__gentuple_1050};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _tagged_arr*)& Cyc__genname_1052,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1051),(
void*)((struct _tuple5**)Cyc__genarr_1051),(void*)((struct _tuple5**)Cyc__genarr_1051
+ 2)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1053={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1054={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1055[2]={& Cyc__gentuple_1053,& Cyc__gentuple_1054};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1048={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1055),(void*)((struct _tuple6**)Cyc__genarr_1055),(
void*)((struct _tuple6**)Cyc__genarr_1055 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1043;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1044;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_287;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_288;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_289;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_270;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_271;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_272;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_273;static struct
_tuple6 Cyc__gentuple_274={offsetof(struct _tuple3,f1),(void*)& Cyc__genrep_134};
static struct _tuple6 Cyc__gentuple_275={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_276[2]={& Cyc__gentuple_274,&
Cyc__gentuple_275};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_273={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_276),(void*)((struct
_tuple6**)Cyc__genarr_276),(void*)((struct _tuple6**)Cyc__genarr_276 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_272={1,1,(void*)((void*)& Cyc__genrep_273)};
static char _tmp1D6[5]="List";static struct _tagged_arr Cyc__genname_280={_tmp1D6,
_tmp1D6,_tmp1D6 + 5};static char _tmp1D7[3]="hd";static struct _tuple5 Cyc__gentuple_277={
offsetof(struct Cyc_List_List,hd),{_tmp1D7,_tmp1D7,_tmp1D7 + 3},(void*)& Cyc__genrep_272};
static char _tmp1D8[3]="tl";static struct _tuple5 Cyc__gentuple_278={offsetof(struct
Cyc_List_List,tl),{_tmp1D8,_tmp1D8,_tmp1D8 + 3},(void*)& Cyc__genrep_271};static
struct _tuple5*Cyc__genarr_279[2]={& Cyc__gentuple_277,& Cyc__gentuple_278};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_280,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_279),(void*)((struct _tuple5**)Cyc__genarr_279),(void*)((
struct _tuple5**)Cyc__genarr_279 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_271={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Position_Segment_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={
1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};static char _tmp1DC[12]="Tunionfield";
static struct _tagged_arr Cyc__genname_286={_tmp1DC,_tmp1DC,_tmp1DC + 12};static char
_tmp1DD[5]="name";static struct _tuple5 Cyc__gentuple_281={offsetof(struct Cyc_Absyn_Tunionfield,name),{
_tmp1DD,_tmp1DD,_tmp1DD + 5},(void*)& Cyc__genrep_10};static char _tmp1DE[5]="typs";
static struct _tuple5 Cyc__gentuple_282={offsetof(struct Cyc_Absyn_Tunionfield,typs),{
_tmp1DE,_tmp1DE,_tmp1DE + 5},(void*)& Cyc__genrep_271};static char _tmp1DF[4]="loc";
static struct _tuple5 Cyc__gentuple_283={offsetof(struct Cyc_Absyn_Tunionfield,loc),{
_tmp1DF,_tmp1DF,_tmp1DF + 4},(void*)& Cyc__genrep_2};static char _tmp1E0[3]="sc";
static struct _tuple5 Cyc__gentuple_284={offsetof(struct Cyc_Absyn_Tunionfield,sc),{
_tmp1E0,_tmp1E0,_tmp1E0 + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*
Cyc__genarr_285[4]={& Cyc__gentuple_281,& Cyc__gentuple_282,& Cyc__gentuple_283,&
Cyc__gentuple_284};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_286,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_285),(void*)((struct _tuple5**)Cyc__genarr_285),(
void*)((struct _tuple5**)Cyc__genarr_285 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_270={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
char _tmp1E3[5]="List";static struct _tagged_arr Cyc__genname_293={_tmp1E3,_tmp1E3,
_tmp1E3 + 5};static char _tmp1E4[3]="hd";static struct _tuple5 Cyc__gentuple_290={
offsetof(struct Cyc_List_List,hd),{_tmp1E4,_tmp1E4,_tmp1E4 + 3},(void*)& Cyc__genrep_270};
static char _tmp1E5[3]="tl";static struct _tuple5 Cyc__gentuple_291={offsetof(struct
Cyc_List_List,tl),{_tmp1E5,_tmp1E5,_tmp1E5 + 3},(void*)& Cyc__genrep_289};static
struct _tuple5*Cyc__genarr_292[2]={& Cyc__gentuple_290,& Cyc__gentuple_291};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_293,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_292),(void*)((struct _tuple5**)Cyc__genarr_292),(void*)((
struct _tuple5**)Cyc__genarr_292 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_289={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp1E8[4]="Opt";static struct _tagged_arr Cyc__genname_296={_tmp1E8,_tmp1E8,
_tmp1E8 + 4};static char _tmp1E9[2]="v";static struct _tuple5 Cyc__gentuple_294={
offsetof(struct Cyc_Core_Opt,v),{_tmp1E9,_tmp1E9,_tmp1E9 + 2},(void*)& Cyc__genrep_289};
static struct _tuple5*Cyc__genarr_295[1]={& Cyc__gentuple_294};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_296,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_295),(void*)((struct _tuple5**)Cyc__genarr_295),(void*)((
struct _tuple5**)Cyc__genarr_295 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_288={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1EC[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_308={
_tmp1EC,_tmp1EC,_tmp1EC + 11};static char _tmp1ED[3]="sc";static struct _tuple5 Cyc__gentuple_302={
offsetof(struct Cyc_Absyn_Tuniondecl,sc),{_tmp1ED,_tmp1ED,_tmp1ED + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp1EE[5]="name";static struct _tuple5 Cyc__gentuple_303={offsetof(
struct Cyc_Absyn_Tuniondecl,name),{_tmp1EE,_tmp1EE,_tmp1EE + 5},(void*)& Cyc__genrep_10};
static char _tmp1EF[4]="tvs";static struct _tuple5 Cyc__gentuple_304={offsetof(struct
Cyc_Absyn_Tuniondecl,tvs),{_tmp1EF,_tmp1EF,_tmp1EF + 4},(void*)& Cyc__genrep_297};
static char _tmp1F0[7]="fields";static struct _tuple5 Cyc__gentuple_305={offsetof(
struct Cyc_Absyn_Tuniondecl,fields),{_tmp1F0,_tmp1F0,_tmp1F0 + 7},(void*)& Cyc__genrep_288};
static char _tmp1F1[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_306={
offsetof(struct Cyc_Absyn_Tuniondecl,is_xtunion),{_tmp1F1,_tmp1F1,_tmp1F1 + 11},(
void*)((void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_307[5]={& Cyc__gentuple_302,&
Cyc__gentuple_303,& Cyc__gentuple_304,& Cyc__gentuple_305,& Cyc__gentuple_306};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_308,sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((
struct _tuple5**)Cyc__genarr_307),(void*)((struct _tuple5**)Cyc__genarr_307),(void*)((
struct _tuple5**)Cyc__genarr_307 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_287={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1044={1,1,(void*)((void*)& Cyc__genrep_287)};struct _tuple20{
unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct _tuple6 Cyc__gentuple_1045={
offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1046={
offsetof(struct _tuple20,f2),(void*)& Cyc__genrep_1044};static struct _tuple6*Cyc__genarr_1047[
2]={& Cyc__gentuple_1045,& Cyc__gentuple_1046};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1043={4,sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1047),(
void*)((struct _tuple6**)Cyc__genarr_1047),(void*)((struct _tuple6**)Cyc__genarr_1047
+ 2)}};static struct _tuple7*Cyc__genarr_1042[0]={};static char _tmp1F6[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1056={0,{_tmp1F6,_tmp1F6,_tmp1F6 + 14},(void*)&
Cyc__genrep_1048};static char _tmp1F7[12]="KnownTunion";static struct _tuple5 Cyc__gentuple_1057={
1,{_tmp1F7,_tmp1F7,_tmp1F7 + 12},(void*)& Cyc__genrep_1043};static struct _tuple5*
Cyc__genarr_1058[2]={& Cyc__gentuple_1056,& Cyc__gentuple_1057};static char _tmp1F9[
12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={
5,{_tmp1F9,_tmp1F9,_tmp1F9 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1042),(
void*)((struct _tuple7**)Cyc__genarr_1042),(void*)((struct _tuple7**)Cyc__genarr_1042
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1058),(void*)((struct _tuple5**)Cyc__genarr_1058),(
void*)((struct _tuple5**)Cyc__genarr_1058 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_53;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp1FA[5]="List";static struct _tagged_arr Cyc__genname_57={_tmp1FA,
_tmp1FA,_tmp1FA + 5};static char _tmp1FB[3]="hd";static struct _tuple5 Cyc__gentuple_54={
offsetof(struct Cyc_List_List,hd),{_tmp1FB,_tmp1FB,_tmp1FB + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp1FC[3]="tl";static struct _tuple5 Cyc__gentuple_55={
offsetof(struct Cyc_List_List,tl),{_tmp1FC,_tmp1FC,_tmp1FC + 3},(void*)& Cyc__genrep_53};
static struct _tuple5*Cyc__genarr_56[2]={& Cyc__gentuple_54,& Cyc__gentuple_55};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_57,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_56),(void*)((struct _tuple5**)Cyc__genarr_56),(void*)((
struct _tuple5**)Cyc__genarr_56 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_53={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp1FF[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1063={_tmp1FF,
_tmp1FF,_tmp1FF + 11};static char _tmp200[12]="tunion_info";static struct _tuple5 Cyc__gentuple_1059={
offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),{_tmp200,_tmp200,_tmp200 + 12},(
void*)& Cyc_tunion_Absyn_TunionInfoU_rep};static char _tmp201[6]="targs";static
struct _tuple5 Cyc__gentuple_1060={offsetof(struct Cyc_Absyn_TunionInfo,targs),{
_tmp201,_tmp201,_tmp201 + 6},(void*)& Cyc__genrep_53};static char _tmp202[4]="rgn";
static struct _tuple5 Cyc__gentuple_1061={offsetof(struct Cyc_Absyn_TunionInfo,rgn),{
_tmp202,_tmp202,_tmp202 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_1062[3]={& Cyc__gentuple_1059,& Cyc__gentuple_1060,& Cyc__gentuple_1061};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1063,sizeof(struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)
Cyc__genarr_1062),(void*)((struct _tuple5**)Cyc__genarr_1062),(void*)((struct
_tuple5**)Cyc__genarr_1062 + 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo
f2;};static struct _tuple6 Cyc__gentuple_1064={offsetof(struct _tuple21,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1065={offsetof(struct _tuple21,f2),(
void*)& Cyc_Absyn_tunion_info_t_rep};static struct _tuple6*Cyc__genarr_1066[2]={&
Cyc__gentuple_1064,& Cyc__gentuple_1065};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1041={
4,sizeof(struct _tuple21),{(void*)((struct _tuple6**)Cyc__genarr_1066),(void*)((
struct _tuple6**)Cyc__genarr_1066),(void*)((struct _tuple6**)Cyc__genarr_1066 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1015;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1022;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp205[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1027={_tmp205,_tmp205,_tmp205 + 23};static
char _tmp206[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1023={offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),{_tmp206,_tmp206,_tmp206 + 12},(
void*)& Cyc__genrep_10};static char _tmp207[11]="field_name";static struct _tuple5 Cyc__gentuple_1024={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),{_tmp207,_tmp207,
_tmp207 + 11},(void*)& Cyc__genrep_10};static char _tmp208[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1025={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),{
_tmp208,_tmp208,_tmp208 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1026[3]={& Cyc__gentuple_1023,& Cyc__gentuple_1024,& Cyc__gentuple_1025};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(
struct _tagged_arr*)& Cyc__genname_1027,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1026),(void*)((struct _tuple5**)Cyc__genarr_1026),(
void*)((struct _tuple5**)Cyc__genarr_1026 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1028={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1029={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1030[2]={& Cyc__gentuple_1028,& Cyc__gentuple_1029};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1022={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1030),(void*)((struct _tuple6**)Cyc__genarr_1030),(
void*)((struct _tuple6**)Cyc__genarr_1030 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1017;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1018={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1019={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_287)};static struct
_tuple6 Cyc__gentuple_1020={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_270};
static struct _tuple6*Cyc__genarr_1021[3]={& Cyc__gentuple_1018,& Cyc__gentuple_1019,&
Cyc__gentuple_1020};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1017={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1021),(void*)((
struct _tuple6**)Cyc__genarr_1021),(void*)((struct _tuple6**)Cyc__genarr_1021 + 3)}};
static struct _tuple7*Cyc__genarr_1016[0]={};static char _tmp20C[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1031={0,{_tmp20C,_tmp20C,_tmp20C + 19},(void*)&
Cyc__genrep_1022};static char _tmp20D[17]="KnownTunionfield";static struct _tuple5
Cyc__gentuple_1032={1,{_tmp20D,_tmp20D,_tmp20D + 17},(void*)& Cyc__genrep_1017};
static struct _tuple5*Cyc__genarr_1033[2]={& Cyc__gentuple_1031,& Cyc__gentuple_1032};
static char _tmp20F[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,{_tmp20F,_tmp20F,_tmp20F + 17},{(void*)((struct _tuple7**)Cyc__genarr_1016),(
void*)((struct _tuple7**)Cyc__genarr_1016),(void*)((struct _tuple7**)Cyc__genarr_1016
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1033),(void*)((struct _tuple5**)Cyc__genarr_1033),(
void*)((struct _tuple5**)Cyc__genarr_1033 + 2)}};static char _tmp210[16]="TunionFieldInfo";
static struct _tagged_arr Cyc__genname_1037={_tmp210,_tmp210,_tmp210 + 16};static
char _tmp211[11]="field_info";static struct _tuple5 Cyc__gentuple_1034={offsetof(
struct Cyc_Absyn_TunionFieldInfo,field_info),{_tmp211,_tmp211,_tmp211 + 11},(void*)&
Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char _tmp212[6]="targs";static struct
_tuple5 Cyc__gentuple_1035={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),{
_tmp212,_tmp212,_tmp212 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_1036[
2]={& Cyc__gentuple_1034,& Cyc__gentuple_1035};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1037,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1036),(void*)((struct _tuple5**)Cyc__genarr_1036),(
void*)((struct _tuple5**)Cyc__genarr_1036 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1038={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1039={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1040[2]={& Cyc__gentuple_1038,& Cyc__gentuple_1039};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1015={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1040),(void*)((struct _tuple6**)Cyc__genarr_1040),(
void*)((struct _tuple6**)Cyc__genarr_1040 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_996;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_963;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
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
void*)((struct _tuple6**)Cyc__genarr_969 + 2)}};static char _tmp217[10]="No_constr";
static struct _tuple7 Cyc__gentuple_964={0,{_tmp217,_tmp217,_tmp217 + 10}};static
struct _tuple7*Cyc__genarr_965[1]={& Cyc__gentuple_964};static char _tmp218[10]="Eq_constr";
static struct _tuple5 Cyc__gentuple_970={0,{_tmp218,_tmp218,_tmp218 + 10},(void*)&
Cyc__genrep_61};static char _tmp219[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_971={
1,{_tmp219,_tmp219,_tmp219 + 15},(void*)& Cyc__genrep_966};static struct _tuple5*Cyc__genarr_972[
2]={& Cyc__gentuple_970,& Cyc__gentuple_971};static char _tmp21B[11]="Constraint";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={5,{_tmp21B,
_tmp21B,_tmp21B + 11},{(void*)((struct _tuple7**)Cyc__genarr_965),(void*)((struct
_tuple7**)Cyc__genarr_965),(void*)((struct _tuple7**)Cyc__genarr_965 + 1)},{(void*)((
struct _tuple5**)Cyc__genarr_972),(void*)((struct _tuple5**)Cyc__genarr_972),(void*)((
struct _tuple5**)Cyc__genarr_972 + 2)}};static char _tmp21C[7]="Conref";static struct
_tagged_arr Cyc__genname_975={_tmp21C,_tmp21C,_tmp21C + 7};static char _tmp21D[2]="v";
static struct _tuple5 Cyc__gentuple_973={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp21D,_tmp21D,_tmp21D + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_974[1]={& Cyc__gentuple_973};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _tagged_arr*)& Cyc__genname_975,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_974),(void*)((
struct _tuple5**)Cyc__genarr_974),(void*)((struct _tuple5**)Cyc__genarr_974 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_963={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_997;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;static char _tmp220[7]="Conref";static
struct _tagged_arr Cyc__genname_1000={_tmp220,_tmp220,_tmp220 + 7};static char
_tmp221[2]="v";static struct _tuple5 Cyc__gentuple_998={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp221,_tmp221,_tmp221 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_999[1]={& Cyc__gentuple_998};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1000,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_999),(void*)((
struct _tuple5**)Cyc__genarr_999),(void*)((struct _tuple5**)Cyc__genarr_999 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_997={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
static char _tmp224[8]="PtrAtts";static struct _tagged_arr Cyc__genname_1006={_tmp224,
_tmp224,_tmp224 + 8};static char _tmp225[4]="rgn";static struct _tuple5 Cyc__gentuple_1001={
offsetof(struct Cyc_Absyn_PtrAtts,rgn),{_tmp225,_tmp225,_tmp225 + 4},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp226[9]="nullable";static struct _tuple5 Cyc__gentuple_1002={
offsetof(struct Cyc_Absyn_PtrAtts,nullable),{_tmp226,_tmp226,_tmp226 + 9},(void*)&
Cyc__genrep_963};static char _tmp227[7]="bounds";static struct _tuple5 Cyc__gentuple_1003={
offsetof(struct Cyc_Absyn_PtrAtts,bounds),{_tmp227,_tmp227,_tmp227 + 7},(void*)&
Cyc__genrep_997};static char _tmp228[10]="zero_term";static struct _tuple5 Cyc__gentuple_1004={
offsetof(struct Cyc_Absyn_PtrAtts,zero_term),{_tmp228,_tmp228,_tmp228 + 10},(void*)&
Cyc__genrep_963};static struct _tuple5*Cyc__genarr_1005[4]={& Cyc__gentuple_1001,&
Cyc__gentuple_1002,& Cyc__gentuple_1003,& Cyc__gentuple_1004};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_atts_t_rep={3,(struct _tagged_arr*)& Cyc__genname_1006,sizeof(struct
Cyc_Absyn_PtrAtts),{(void*)((struct _tuple5**)Cyc__genarr_1005),(void*)((struct
_tuple5**)Cyc__genarr_1005),(void*)((struct _tuple5**)Cyc__genarr_1005 + 4)}};
static char _tmp22A[8]="PtrInfo";static struct _tagged_arr Cyc__genname_1011={_tmp22A,
_tmp22A,_tmp22A + 8};static char _tmp22B[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1007={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),{_tmp22B,_tmp22B,_tmp22B + 8},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp22C[7]="elt_tq";static struct _tuple5
Cyc__gentuple_1008={offsetof(struct Cyc_Absyn_PtrInfo,elt_tq),{_tmp22C,_tmp22C,
_tmp22C + 7},(void*)& Cyc__genrep_134};static char _tmp22D[9]="ptr_atts";static
struct _tuple5 Cyc__gentuple_1009={offsetof(struct Cyc_Absyn_PtrInfo,ptr_atts),{
_tmp22D,_tmp22D,_tmp22D + 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static struct
_tuple5*Cyc__genarr_1010[3]={& Cyc__gentuple_1007,& Cyc__gentuple_1008,& Cyc__gentuple_1009};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1011,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple5**)Cyc__genarr_1010),(
void*)((struct _tuple5**)Cyc__genarr_1010),(void*)((struct _tuple5**)Cyc__genarr_1010
+ 3)}};struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct
_tuple6 Cyc__gentuple_1012={offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1013={offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};
static struct _tuple6*Cyc__genarr_1014[2]={& Cyc__gentuple_1012,& Cyc__gentuple_1013};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_996={4,sizeof(struct _tuple27),{(
void*)((struct _tuple6**)Cyc__genarr_1014),(void*)((struct _tuple6**)Cyc__genarr_1014),(
void*)((struct _tuple6**)Cyc__genarr_1014 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_985;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static
char _tmp230[7]="Signed";static struct _tuple7 Cyc__gentuple_420={0,{_tmp230,_tmp230,
_tmp230 + 7}};static char _tmp231[9]="Unsigned";static struct _tuple7 Cyc__gentuple_421={
1,{_tmp231,_tmp231,_tmp231 + 9}};static char _tmp232[5]="None";static struct _tuple7
Cyc__gentuple_422={2,{_tmp232,_tmp232,_tmp232 + 5}};static struct _tuple7*Cyc__genarr_423[
3]={& Cyc__gentuple_420,& Cyc__gentuple_421,& Cyc__gentuple_422};static struct
_tuple5*Cyc__genarr_424[0]={};static char _tmp234[5]="Sign";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep={5,{_tmp234,_tmp234,_tmp234 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_423),(void*)((struct _tuple7**)Cyc__genarr_423),(void*)((struct
_tuple7**)Cyc__genarr_423 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_424),(void*)((
struct _tuple5**)Cyc__genarr_424),(void*)((struct _tuple5**)Cyc__genarr_424 + 0)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;static char _tmp235[3]="B1";
static struct _tuple7 Cyc__gentuple_986={0,{_tmp235,_tmp235,_tmp235 + 3}};static char
_tmp236[3]="B2";static struct _tuple7 Cyc__gentuple_987={1,{_tmp236,_tmp236,_tmp236
+ 3}};static char _tmp237[3]="B4";static struct _tuple7 Cyc__gentuple_988={2,{_tmp237,
_tmp237,_tmp237 + 3}};static char _tmp238[3]="B8";static struct _tuple7 Cyc__gentuple_989={
3,{_tmp238,_tmp238,_tmp238 + 3}};static struct _tuple7*Cyc__genarr_990[4]={& Cyc__gentuple_986,&
Cyc__gentuple_987,& Cyc__gentuple_988,& Cyc__gentuple_989};static struct _tuple5*Cyc__genarr_991[
0]={};static char _tmp23A[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,{_tmp23A,_tmp23A,_tmp23A + 8},{(void*)((struct _tuple7**)Cyc__genarr_990),(void*)((
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
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_78;extern struct Cyc_Typerep_Struct_struct
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
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_419;struct _tuple31{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_425={offsetof(
struct _tuple31,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_426={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_427={
offsetof(struct _tuple31,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_428[3]={& Cyc__gentuple_425,& Cyc__gentuple_426,& Cyc__gentuple_427};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_419={4,sizeof(struct _tuple31),{(
void*)((struct _tuple6**)Cyc__genarr_428),(void*)((struct _tuple6**)Cyc__genarr_428),(
void*)((struct _tuple6**)Cyc__genarr_428 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_841;static struct Cyc_Typerep_Int_struct Cyc__genrep_842={0,1,64};
struct _tuple32{unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_843={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_844={
offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_845={
offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_842};static struct _tuple6*Cyc__genarr_846[
3]={& Cyc__gentuple_843,& Cyc__gentuple_844,& Cyc__gentuple_845};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_841={4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_846),(
void*)((struct _tuple6**)Cyc__genarr_846),(void*)((struct _tuple6**)Cyc__genarr_846
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_117;static struct _tuple6
Cyc__gentuple_118={offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_119={offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_13)};
static struct _tuple6*Cyc__genarr_120[2]={& Cyc__gentuple_118,& Cyc__gentuple_119};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_117={4,sizeof(struct _tuple7),{(
void*)((struct _tuple6**)Cyc__genarr_120),(void*)((struct _tuple6**)Cyc__genarr_120),(
void*)((struct _tuple6**)Cyc__genarr_120 + 2)}};static char _tmp243[7]="Null_c";
static struct _tuple7 Cyc__gentuple_839={0,{_tmp243,_tmp243,_tmp243 + 7}};static
struct _tuple7*Cyc__genarr_840[1]={& Cyc__gentuple_839};static char _tmp244[7]="Char_c";
static struct _tuple5 Cyc__gentuple_858={0,{_tmp244,_tmp244,_tmp244 + 7},(void*)& Cyc__genrep_853};
static char _tmp245[8]="Short_c";static struct _tuple5 Cyc__gentuple_859={1,{_tmp245,
_tmp245,_tmp245 + 8},(void*)& Cyc__genrep_847};static char _tmp246[6]="Int_c";static
struct _tuple5 Cyc__gentuple_860={2,{_tmp246,_tmp246,_tmp246 + 6},(void*)& Cyc__genrep_419};
static char _tmp247[11]="LongLong_c";static struct _tuple5 Cyc__gentuple_861={3,{
_tmp247,_tmp247,_tmp247 + 11},(void*)& Cyc__genrep_841};static char _tmp248[8]="Float_c";
static struct _tuple5 Cyc__gentuple_862={4,{_tmp248,_tmp248,_tmp248 + 8},(void*)& Cyc__genrep_117};
static char _tmp249[9]="String_c";static struct _tuple5 Cyc__gentuple_863={5,{_tmp249,
_tmp249,_tmp249 + 9},(void*)& Cyc__genrep_117};static struct _tuple5*Cyc__genarr_864[
6]={& Cyc__gentuple_858,& Cyc__gentuple_859,& Cyc__gentuple_860,& Cyc__gentuple_861,&
Cyc__gentuple_862,& Cyc__gentuple_863};static char _tmp24B[5]="Cnst";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep={5,{_tmp24B,_tmp24B,_tmp24B + 5},{(void*)((struct _tuple7**)
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
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_86;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_87;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_591;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_592;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_593;struct _tuple33{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple6 Cyc__gentuple_594={offsetof(struct _tuple33,f1),(void*)&
Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_595={offsetof(struct _tuple33,f2),(
void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_596={offsetof(struct
_tuple33,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_597[
3]={& Cyc__gentuple_594,& Cyc__gentuple_595,& Cyc__gentuple_596};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_593={4,sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_597),(
void*)((struct _tuple6**)Cyc__genarr_597),(void*)((struct _tuple6**)Cyc__genarr_597
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_592={1,1,(void*)((void*)&
Cyc__genrep_593)};static char _tmp24F[5]="List";static struct _tagged_arr Cyc__genname_601={
_tmp24F,_tmp24F,_tmp24F + 5};static char _tmp250[3]="hd";static struct _tuple5 Cyc__gentuple_598={
offsetof(struct Cyc_List_List,hd),{_tmp250,_tmp250,_tmp250 + 3},(void*)& Cyc__genrep_592};
static char _tmp251[3]="tl";static struct _tuple5 Cyc__gentuple_599={offsetof(struct
Cyc_List_List,tl),{_tmp251,_tmp251,_tmp251 + 3},(void*)& Cyc__genrep_591};static
struct _tuple5*Cyc__genarr_600[2]={& Cyc__gentuple_598,& Cyc__gentuple_599};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_601,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_600),(void*)((struct _tuple5**)Cyc__genarr_600),(void*)((
struct _tuple5**)Cyc__genarr_600 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_591={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_580;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_581;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp254[4]="Opt";static struct _tagged_arr Cyc__genname_584={_tmp254,_tmp254,
_tmp254 + 4};static char _tmp255[2]="v";static struct _tuple5 Cyc__gentuple_582={
offsetof(struct Cyc_Core_Opt,v),{_tmp255,_tmp255,_tmp255 + 2},(void*)& Cyc__genrep_12};
static struct _tuple5*Cyc__genarr_583[1]={& Cyc__gentuple_582};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_584,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_583),(void*)((
struct _tuple5**)Cyc__genarr_583),(void*)((struct _tuple5**)Cyc__genarr_583 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_581={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp258[11]="VarargInfo";static struct _tagged_arr Cyc__genname_590={
_tmp258,_tmp258,_tmp258 + 11};static char _tmp259[5]="name";static struct _tuple5 Cyc__gentuple_585={
offsetof(struct Cyc_Absyn_VarargInfo,name),{_tmp259,_tmp259,_tmp259 + 5},(void*)&
Cyc__genrep_581};static char _tmp25A[3]="tq";static struct _tuple5 Cyc__gentuple_586={
offsetof(struct Cyc_Absyn_VarargInfo,tq),{_tmp25A,_tmp25A,_tmp25A + 3},(void*)& Cyc__genrep_134};
static char _tmp25B[5]="type";static struct _tuple5 Cyc__gentuple_587={offsetof(
struct Cyc_Absyn_VarargInfo,type),{_tmp25B,_tmp25B,_tmp25B + 5},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp25C[7]="inject";static struct _tuple5 Cyc__gentuple_588={
offsetof(struct Cyc_Absyn_VarargInfo,inject),{_tmp25C,_tmp25C,_tmp25C + 7},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_589[4]={& Cyc__gentuple_585,&
Cyc__gentuple_586,& Cyc__gentuple_587,& Cyc__gentuple_588};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_590,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_589),(void*)((
struct _tuple5**)Cyc__genarr_589),(void*)((struct _tuple5**)Cyc__genarr_589 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_580={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_357;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_358;static struct
_tuple6 Cyc__gentuple_359={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_360={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_361[2]={& Cyc__gentuple_359,&
Cyc__gentuple_360};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_358={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_361),(void*)((struct
_tuple6**)Cyc__genarr_361),(void*)((struct _tuple6**)Cyc__genarr_361 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357={1,1,(void*)((void*)& Cyc__genrep_358)};
static char _tmp261[5]="List";static struct _tagged_arr Cyc__genname_365={_tmp261,
_tmp261,_tmp261 + 5};static char _tmp262[3]="hd";static struct _tuple5 Cyc__gentuple_362={
offsetof(struct Cyc_List_List,hd),{_tmp262,_tmp262,_tmp262 + 3},(void*)& Cyc__genrep_357};
static char _tmp263[3]="tl";static struct _tuple5 Cyc__gentuple_363={offsetof(struct
Cyc_List_List,tl),{_tmp263,_tmp263,_tmp263 + 3},(void*)& Cyc__genrep_356};static
struct _tuple5*Cyc__genarr_364[2]={& Cyc__gentuple_362,& Cyc__gentuple_363};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_365,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_364),(void*)((struct _tuple5**)Cyc__genarr_364),(void*)((
struct _tuple5**)Cyc__genarr_364 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_162;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_82;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_82={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple34{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_83={offsetof(struct _tuple34,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_84={offsetof(struct _tuple34,f2),(
void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_85[2]={& Cyc__gentuple_83,&
Cyc__gentuple_84};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={4,sizeof(
struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_85),(void*)((struct _tuple6**)
Cyc__genarr_85),(void*)((struct _tuple6**)Cyc__genarr_85 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_546;struct _tuple35{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*
f3;};static struct _tuple6 Cyc__gentuple_547={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_548={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_162};static struct _tuple6 Cyc__gentuple_549={offsetof(struct
_tuple35,f3),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_550[3]={&
Cyc__gentuple_547,& Cyc__gentuple_548,& Cyc__gentuple_549};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_546={4,sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_550),(
void*)((struct _tuple6**)Cyc__genarr_550),(void*)((struct _tuple6**)Cyc__genarr_550
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_542;static struct _tuple6
Cyc__gentuple_543={offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_544={offsetof(struct _tuple34,f2),(void*)& Cyc__genrep_78};
static struct _tuple6*Cyc__genarr_545[2]={& Cyc__gentuple_543,& Cyc__gentuple_544};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_542={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_545),(void*)((struct _tuple6**)Cyc__genarr_545),(
void*)((struct _tuple6**)Cyc__genarr_545 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_536;struct _tuple36{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_537={offsetof(
struct _tuple36,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_162};static struct _tuple6 Cyc__gentuple_540={
offsetof(struct _tuple36,f4),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_541[
4]={& Cyc__gentuple_537,& Cyc__gentuple_538,& Cyc__gentuple_539,& Cyc__gentuple_540};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_536={4,sizeof(struct _tuple36),{(
void*)((struct _tuple6**)Cyc__genarr_541),(void*)((struct _tuple6**)Cyc__genarr_541),(
void*)((struct _tuple6**)Cyc__genarr_541 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_531;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_171;static struct
_tuple6 Cyc__gentuple_172={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_173={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_162};
static struct _tuple6*Cyc__genarr_174[2]={& Cyc__gentuple_172,& Cyc__gentuple_173};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_171={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_174),(void*)((struct _tuple6**)Cyc__genarr_174),(
void*)((struct _tuple6**)Cyc__genarr_174 + 2)}};struct _tuple37{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_532={
offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_533={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_171};static struct _tuple6 Cyc__gentuple_534={
offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_535[
3]={& Cyc__gentuple_532,& Cyc__gentuple_533,& Cyc__gentuple_534};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_531={4,sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_535),(
void*)((struct _tuple6**)Cyc__genarr_535),(void*)((struct _tuple6**)Cyc__genarr_535
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_527;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_522;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_522={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple38{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_528={offsetof(struct _tuple38,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_529={offsetof(struct _tuple38,f2),(
void*)& Cyc__genrep_522};static struct _tuple6*Cyc__genarr_530[2]={& Cyc__gentuple_528,&
Cyc__gentuple_529};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_527={4,
sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_530),(void*)((
struct _tuple6**)Cyc__genarr_530),(void*)((struct _tuple6**)Cyc__genarr_530 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_521;struct _tuple39{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_523={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_524={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_525={
offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_522};static struct _tuple6*Cyc__genarr_526[
3]={& Cyc__gentuple_523,& Cyc__gentuple_524,& Cyc__gentuple_525};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_521={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_526),(
void*)((struct _tuple6**)Cyc__genarr_526),(void*)((struct _tuple6**)Cyc__genarr_526
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_514;struct _tuple40{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_515={offsetof(struct _tuple40,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_516={offsetof(struct _tuple40,f2),(
void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_517={offsetof(struct
_tuple40,f3),(void*)& Cyc__genrep_171};static struct _tuple6 Cyc__gentuple_518={
offsetof(struct _tuple40,f4),(void*)& Cyc__genrep_171};static struct _tuple6 Cyc__gentuple_519={
offsetof(struct _tuple40,f5),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_520[
5]={& Cyc__gentuple_515,& Cyc__gentuple_516,& Cyc__gentuple_517,& Cyc__gentuple_518,&
Cyc__gentuple_519};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_514={4,
sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_520),(void*)((
struct _tuple6**)Cyc__genarr_520),(void*)((struct _tuple6**)Cyc__genarr_520 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_509;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_229;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_230;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_415;
struct _tuple41{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_416={
offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_417={
offsetof(struct _tuple41,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_418[2]={& Cyc__gentuple_416,& Cyc__gentuple_417};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_415={4,sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_418),(
void*)((struct _tuple6**)Cyc__genarr_418),(void*)((struct _tuple6**)Cyc__genarr_418
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_411;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_236;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp272[5]="List";static struct _tagged_arr Cyc__genname_240={_tmp272,
_tmp272,_tmp272 + 5};static char _tmp273[3]="hd";static struct _tuple5 Cyc__gentuple_237={
offsetof(struct Cyc_List_List,hd),{_tmp273,_tmp273,_tmp273 + 3},(void*)& Cyc__genrep_231};
static char _tmp274[3]="tl";static struct _tuple5 Cyc__gentuple_238={offsetof(struct
Cyc_List_List,tl),{_tmp274,_tmp274,_tmp274 + 3},(void*)& Cyc__genrep_236};static
struct _tuple5*Cyc__genarr_239[2]={& Cyc__gentuple_237,& Cyc__gentuple_238};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_240,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_239),(void*)((struct _tuple5**)Cyc__genarr_239),(void*)((
struct _tuple5**)Cyc__genarr_239 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_236={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};static struct
_tuple6 Cyc__gentuple_412={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_413={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_236};
static struct _tuple6*Cyc__genarr_414[2]={& Cyc__gentuple_412,& Cyc__gentuple_413};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_411={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_414),(void*)((struct _tuple6**)Cyc__genarr_414),(
void*)((struct _tuple6**)Cyc__genarr_414 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_407;struct _tuple42{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple6 Cyc__gentuple_408={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_409={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_231};
static struct _tuple6*Cyc__genarr_410[2]={& Cyc__gentuple_408,& Cyc__gentuple_409};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_407={4,sizeof(struct _tuple42),{(
void*)((struct _tuple6**)Cyc__genarr_410),(void*)((struct _tuple6**)Cyc__genarr_410),(
void*)((struct _tuple6**)Cyc__genarr_410 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_314;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_386;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static char _tmp279[8]="StructA";static struct _tuple7 Cyc__gentuple_371={0,{_tmp279,
_tmp279,_tmp279 + 8}};static char _tmp27A[7]="UnionA";static struct _tuple7 Cyc__gentuple_372={
1,{_tmp27A,_tmp27A,_tmp27A + 7}};static struct _tuple7*Cyc__genarr_373[2]={& Cyc__gentuple_371,&
Cyc__gentuple_372};static struct _tuple5*Cyc__genarr_374[0]={};static char _tmp27C[9]="AggrKind";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,{_tmp27C,_tmp27C,
_tmp27C + 9},{(void*)((struct _tuple7**)Cyc__genarr_373),(void*)((struct _tuple7**)
Cyc__genarr_373),(void*)((struct _tuple7**)Cyc__genarr_373 + 2)},{(void*)((struct
_tuple5**)Cyc__genarr_374),(void*)((struct _tuple5**)Cyc__genarr_374),(void*)((
struct _tuple5**)Cyc__genarr_374 + 0)}};struct _tuple43{unsigned int f1;void*f2;
struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_387={offsetof(struct _tuple43,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_388={offsetof(struct
_tuple43,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct _tuple6 Cyc__gentuple_389={
offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_390[
3]={& Cyc__gentuple_387,& Cyc__gentuple_388,& Cyc__gentuple_389};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_386={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_390),(
void*)((struct _tuple6**)Cyc__genarr_390),(void*)((struct _tuple6**)Cyc__genarr_390
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_339;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_340;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_342;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_343;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_344;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_88;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_107;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp27E[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_108={0,{_tmp27E,_tmp27E,
_tmp27E + 10}};static char _tmp27F[9]="Scanf_ft";static struct _tuple7 Cyc__gentuple_109={
1,{_tmp27F,_tmp27F,_tmp27F + 9}};static struct _tuple7*Cyc__genarr_110[2]={& Cyc__gentuple_108,&
Cyc__gentuple_109};static struct _tuple5*Cyc__genarr_111[0]={};static char _tmp281[
12]="Format_Type";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={
5,{_tmp281,_tmp281,_tmp281 + 12},{(void*)((struct _tuple7**)Cyc__genarr_110),(void*)((
struct _tuple7**)Cyc__genarr_110),(void*)((struct _tuple7**)Cyc__genarr_110 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_111),(void*)((struct _tuple5**)Cyc__genarr_111),(
void*)((struct _tuple5**)Cyc__genarr_111 + 0)}};struct _tuple44{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_112={offsetof(struct _tuple44,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_113={offsetof(struct
_tuple44,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_114={
offsetof(struct _tuple44,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_115={offsetof(struct _tuple44,f4),(void*)((void*)& Cyc__genrep_62)};
static struct _tuple6*Cyc__genarr_116[4]={& Cyc__gentuple_112,& Cyc__gentuple_113,&
Cyc__gentuple_114,& Cyc__gentuple_115};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_107={
4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_116),(void*)((
struct _tuple6**)Cyc__genarr_116),(void*)((struct _tuple6**)Cyc__genarr_116 + 4)}};
static char _tmp283[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_89={0,{
_tmp283,_tmp283,_tmp283 + 12}};static char _tmp284[10]="Cdecl_att";static struct
_tuple7 Cyc__gentuple_90={1,{_tmp284,_tmp284,_tmp284 + 10}};static char _tmp285[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_91={2,{_tmp285,_tmp285,_tmp285 + 13}};static char
_tmp286[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_92={3,{_tmp286,
_tmp286,_tmp286 + 13}};static char _tmp287[10]="Const_att";static struct _tuple7 Cyc__gentuple_93={
4,{_tmp287,_tmp287,_tmp287 + 10}};static char _tmp288[11]="Packed_att";static struct
_tuple7 Cyc__gentuple_94={5,{_tmp288,_tmp288,_tmp288 + 11}};static char _tmp289[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_95={6,{_tmp289,_tmp289,_tmp289 + 13}};static char
_tmp28A[11]="Shared_att";static struct _tuple7 Cyc__gentuple_96={7,{_tmp28A,_tmp28A,
_tmp28A + 11}};static char _tmp28B[11]="Unused_att";static struct _tuple7 Cyc__gentuple_97={
8,{_tmp28B,_tmp28B,_tmp28B + 11}};static char _tmp28C[9]="Weak_att";static struct
_tuple7 Cyc__gentuple_98={9,{_tmp28C,_tmp28C,_tmp28C + 9}};static char _tmp28D[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_99={10,{_tmp28D,_tmp28D,_tmp28D + 14}};static
char _tmp28E[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_100={11,{
_tmp28E,_tmp28E,_tmp28E + 14}};static char _tmp28F[27]="No_instrument_function_att";
static struct _tuple7 Cyc__gentuple_101={12,{_tmp28F,_tmp28F,_tmp28F + 27}};static
char _tmp290[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_102={13,{
_tmp290,_tmp290,_tmp290 + 16}};static char _tmp291[15]="Destructor_att";static
struct _tuple7 Cyc__gentuple_103={14,{_tmp291,_tmp291,_tmp291 + 15}};static char
_tmp292[26]="No_check_memory_usage_att";static struct _tuple7 Cyc__gentuple_104={15,{
_tmp292,_tmp292,_tmp292 + 26}};static char _tmp293[9]="Pure_att";static struct
_tuple7 Cyc__gentuple_105={16,{_tmp293,_tmp293,_tmp293 + 9}};static struct _tuple7*
Cyc__genarr_106[17]={& Cyc__gentuple_89,& Cyc__gentuple_90,& Cyc__gentuple_91,& Cyc__gentuple_92,&
Cyc__gentuple_93,& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97,&
Cyc__gentuple_98,& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,& Cyc__gentuple_102,&
Cyc__gentuple_103,& Cyc__gentuple_104,& Cyc__gentuple_105};static char _tmp294[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_121={0,{_tmp294,_tmp294,_tmp294 + 12},(void*)&
Cyc__genrep_61};static char _tmp295[12]="Aligned_att";static struct _tuple5 Cyc__gentuple_122={
1,{_tmp295,_tmp295,_tmp295 + 12},(void*)& Cyc__genrep_61};static char _tmp296[12]="Section_att";
static struct _tuple5 Cyc__gentuple_123={2,{_tmp296,_tmp296,_tmp296 + 12},(void*)&
Cyc__genrep_117};static char _tmp297[11]="Format_att";static struct _tuple5 Cyc__gentuple_124={
3,{_tmp297,_tmp297,_tmp297 + 11},(void*)& Cyc__genrep_107};static char _tmp298[16]="Initializes_att";
static struct _tuple5 Cyc__gentuple_125={4,{_tmp298,_tmp298,_tmp298 + 16},(void*)&
Cyc__genrep_61};static struct _tuple5*Cyc__genarr_126[5]={& Cyc__gentuple_121,& Cyc__gentuple_122,&
Cyc__gentuple_123,& Cyc__gentuple_124,& Cyc__gentuple_125};static char _tmp29A[10]="Attribute";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,{_tmp29A,_tmp29A,
_tmp29A + 10},{(void*)((struct _tuple7**)Cyc__genarr_106),(void*)((struct _tuple7**)
Cyc__genarr_106),(void*)((struct _tuple7**)Cyc__genarr_106 + 17)},{(void*)((struct
_tuple5**)Cyc__genarr_126),(void*)((struct _tuple5**)Cyc__genarr_126),(void*)((
struct _tuple5**)Cyc__genarr_126 + 5)}};static char _tmp29B[5]="List";static struct
_tagged_arr Cyc__genname_130={_tmp29B,_tmp29B,_tmp29B + 5};static char _tmp29C[3]="hd";
static struct _tuple5 Cyc__gentuple_127={offsetof(struct Cyc_List_List,hd),{_tmp29C,
_tmp29C,_tmp29C + 3},(void*)& Cyc_Absyn_attribute_t_rep};static char _tmp29D[3]="tl";
static struct _tuple5 Cyc__gentuple_128={offsetof(struct Cyc_List_List,tl),{_tmp29D,
_tmp29D,_tmp29D + 3},(void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_129[
2]={& Cyc__gentuple_127,& Cyc__gentuple_128};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_130,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_129),(void*)((struct _tuple5**)Cyc__genarr_129),(void*)((
struct _tuple5**)Cyc__genarr_129 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_88={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp2A0[10]="Aggrfield";static struct _tagged_arr Cyc__genname_351={_tmp2A0,_tmp2A0,
_tmp2A0 + 10};static char _tmp2A1[5]="name";static struct _tuple5 Cyc__gentuple_345={
offsetof(struct Cyc_Absyn_Aggrfield,name),{_tmp2A1,_tmp2A1,_tmp2A1 + 5},(void*)&
Cyc__genrep_12};static char _tmp2A2[3]="tq";static struct _tuple5 Cyc__gentuple_346={
offsetof(struct Cyc_Absyn_Aggrfield,tq),{_tmp2A2,_tmp2A2,_tmp2A2 + 3},(void*)& Cyc__genrep_134};
static char _tmp2A3[5]="type";static struct _tuple5 Cyc__gentuple_347={offsetof(
struct Cyc_Absyn_Aggrfield,type),{_tmp2A3,_tmp2A3,_tmp2A3 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp2A4[6]="width";static struct _tuple5 Cyc__gentuple_348={offsetof(
struct Cyc_Absyn_Aggrfield,width),{_tmp2A4,_tmp2A4,_tmp2A4 + 6},(void*)& Cyc__genrep_78};
static char _tmp2A5[11]="attributes";static struct _tuple5 Cyc__gentuple_349={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),{_tmp2A5,_tmp2A5,_tmp2A5 + 11},(
void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_350[5]={& Cyc__gentuple_345,&
Cyc__gentuple_346,& Cyc__gentuple_347,& Cyc__gentuple_348,& Cyc__gentuple_349};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_351,sizeof(struct Cyc_Absyn_Aggrfield),{(void*)((
struct _tuple5**)Cyc__genarr_350),(void*)((struct _tuple5**)Cyc__genarr_350),(void*)((
struct _tuple5**)Cyc__genarr_350 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_344={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp2A8[5]="List";
static struct _tagged_arr Cyc__genname_355={_tmp2A8,_tmp2A8,_tmp2A8 + 5};static char
_tmp2A9[3]="hd";static struct _tuple5 Cyc__gentuple_352={offsetof(struct Cyc_List_List,hd),{
_tmp2A9,_tmp2A9,_tmp2A9 + 3},(void*)& Cyc__genrep_344};static char _tmp2AA[3]="tl";
static struct _tuple5 Cyc__gentuple_353={offsetof(struct Cyc_List_List,tl),{_tmp2AA,
_tmp2AA,_tmp2AA + 3},(void*)& Cyc__genrep_343};static struct _tuple5*Cyc__genarr_354[
2]={& Cyc__gentuple_352,& Cyc__gentuple_353};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_355,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_354),(void*)((struct _tuple5**)Cyc__genarr_354),(void*)((
struct _tuple5**)Cyc__genarr_354 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_343={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp2AD[13]="AggrdeclImpl";static struct _tagged_arr Cyc__genname_370={_tmp2AD,
_tmp2AD,_tmp2AD + 13};static char _tmp2AE[11]="exist_vars";static struct _tuple5 Cyc__gentuple_366={
offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),{_tmp2AE,_tmp2AE,_tmp2AE + 11},(
void*)& Cyc__genrep_297};static char _tmp2AF[7]="rgn_po";static struct _tuple5 Cyc__gentuple_367={
offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),{_tmp2AF,_tmp2AF,_tmp2AF + 7},(void*)&
Cyc__genrep_356};static char _tmp2B0[7]="fields";static struct _tuple5 Cyc__gentuple_368={
offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),{_tmp2B0,_tmp2B0,_tmp2B0 + 7},(void*)&
Cyc__genrep_343};static struct _tuple5*Cyc__genarr_369[3]={& Cyc__gentuple_366,& Cyc__gentuple_367,&
Cyc__gentuple_368};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={
3,(struct _tagged_arr*)& Cyc__genname_370,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(
void*)((struct _tuple5**)Cyc__genarr_369),(void*)((struct _tuple5**)Cyc__genarr_369),(
void*)((struct _tuple5**)Cyc__genarr_369 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_342={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static
char _tmp2B3[9]="Aggrdecl";static struct _tagged_arr Cyc__genname_382={_tmp2B3,
_tmp2B3,_tmp2B3 + 9};static char _tmp2B4[5]="kind";static struct _tuple5 Cyc__gentuple_375={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),{_tmp2B4,_tmp2B4,_tmp2B4 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};
static char _tmp2B5[3]="sc";static struct _tuple5 Cyc__gentuple_376={offsetof(struct
Cyc_Absyn_Aggrdecl,sc),{_tmp2B5,_tmp2B5,_tmp2B5 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp2B6[5]="name";static struct _tuple5 Cyc__gentuple_377={offsetof(
struct Cyc_Absyn_Aggrdecl,name),{_tmp2B6,_tmp2B6,_tmp2B6 + 5},(void*)& Cyc__genrep_10};
static char _tmp2B7[4]="tvs";static struct _tuple5 Cyc__gentuple_378={offsetof(struct
Cyc_Absyn_Aggrdecl,tvs),{_tmp2B7,_tmp2B7,_tmp2B7 + 4},(void*)& Cyc__genrep_297};
static char _tmp2B8[5]="impl";static struct _tuple5 Cyc__gentuple_379={offsetof(
struct Cyc_Absyn_Aggrdecl,impl),{_tmp2B8,_tmp2B8,_tmp2B8 + 5},(void*)& Cyc__genrep_342};
static char _tmp2B9[11]="attributes";static struct _tuple5 Cyc__gentuple_380={
offsetof(struct Cyc_Absyn_Aggrdecl,attributes),{_tmp2B9,_tmp2B9,_tmp2B9 + 11},(
void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_381[6]={& Cyc__gentuple_375,&
Cyc__gentuple_376,& Cyc__gentuple_377,& Cyc__gentuple_378,& Cyc__gentuple_379,& Cyc__gentuple_380};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_382,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((struct
_tuple5**)Cyc__genarr_381),(void*)((struct _tuple5**)Cyc__genarr_381),(void*)((
struct _tuple5**)Cyc__genarr_381 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_340={1,1,(void*)((void*)& Cyc__genrep_341)};struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_383={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_384={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_340};static struct _tuple6*Cyc__genarr_385[
2]={& Cyc__gentuple_383,& Cyc__gentuple_384};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_339={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_385),(
void*)((struct _tuple6**)Cyc__genarr_385),(void*)((struct _tuple6**)Cyc__genarr_385
+ 2)}};static struct _tuple7*Cyc__genarr_338[0]={};static char _tmp2BE[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_391={0,{_tmp2BE,_tmp2BE,_tmp2BE + 12},(void*)&
Cyc__genrep_386};static char _tmp2BF[10]="KnownAggr";static struct _tuple5 Cyc__gentuple_392={
1,{_tmp2BF,_tmp2BF,_tmp2BF + 10},(void*)& Cyc__genrep_339};static struct _tuple5*Cyc__genarr_393[
2]={& Cyc__gentuple_391,& Cyc__gentuple_392};static char _tmp2C1[10]="AggrInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={5,{_tmp2C1,
_tmp2C1,_tmp2C1 + 10},{(void*)((struct _tuple7**)Cyc__genarr_338),(void*)((struct
_tuple7**)Cyc__genarr_338),(void*)((struct _tuple7**)Cyc__genarr_338 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_393),(void*)((struct _tuple5**)Cyc__genarr_393),(void*)((
struct _tuple5**)Cyc__genarr_393 + 2)}};static char _tmp2C2[9]="AggrInfo";static
struct _tagged_arr Cyc__genname_397={_tmp2C2,_tmp2C2,_tmp2C2 + 9};static char _tmp2C3[
10]="aggr_info";static struct _tuple5 Cyc__gentuple_394={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),{
_tmp2C3,_tmp2C3,_tmp2C3 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char
_tmp2C4[6]="targs";static struct _tuple5 Cyc__gentuple_395={offsetof(struct Cyc_Absyn_AggrInfo,targs),{
_tmp2C4,_tmp2C4,_tmp2C4 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_396[
2]={& Cyc__gentuple_394,& Cyc__gentuple_395};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_397,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_396),(void*)((struct _tuple5**)Cyc__genarr_396),(void*)((
struct _tuple5**)Cyc__genarr_396 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_315;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_316;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_317;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_318;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_320;struct _tuple46{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple6 Cyc__gentuple_321={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_322={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_12};
static struct _tuple6*Cyc__genarr_323[2]={& Cyc__gentuple_321,& Cyc__gentuple_322};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_320={4,sizeof(struct _tuple46),{(
void*)((struct _tuple6**)Cyc__genarr_323),(void*)((struct _tuple6**)Cyc__genarr_323),(
void*)((struct _tuple6**)Cyc__genarr_323 + 2)}};static struct _tuple7*Cyc__genarr_319[
0]={};static char _tmp2C7[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_324={
0,{_tmp2C7,_tmp2C7,_tmp2C7 + 13},(void*)& Cyc__genrep_81};static char _tmp2C8[10]="FieldName";
static struct _tuple5 Cyc__gentuple_325={1,{_tmp2C8,_tmp2C8,_tmp2C8 + 10},(void*)&
Cyc__genrep_320};static struct _tuple5*Cyc__genarr_326[2]={& Cyc__gentuple_324,& Cyc__gentuple_325};
static char _tmp2CA[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,{_tmp2CA,_tmp2CA,_tmp2CA + 11},{(void*)((struct _tuple7**)Cyc__genarr_319),(void*)((
struct _tuple7**)Cyc__genarr_319),(void*)((struct _tuple7**)Cyc__genarr_319 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_326),(void*)((struct _tuple5**)Cyc__genarr_326),(
void*)((struct _tuple5**)Cyc__genarr_326 + 2)}};static char _tmp2CB[5]="List";static
struct _tagged_arr Cyc__genname_330={_tmp2CB,_tmp2CB,_tmp2CB + 5};static char _tmp2CC[
3]="hd";static struct _tuple5 Cyc__gentuple_327={offsetof(struct Cyc_List_List,hd),{
_tmp2CC,_tmp2CC,_tmp2CC + 3},(void*)& Cyc_Absyn_designator_t_rep};static char
_tmp2CD[3]="tl";static struct _tuple5 Cyc__gentuple_328={offsetof(struct Cyc_List_List,tl),{
_tmp2CD,_tmp2CD,_tmp2CD + 3},(void*)& Cyc__genrep_318};static struct _tuple5*Cyc__genarr_329[
2]={& Cyc__gentuple_327,& Cyc__gentuple_328};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_330,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_329),(void*)((struct _tuple5**)Cyc__genarr_329),(void*)((
struct _tuple5**)Cyc__genarr_329 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_318={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple47{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_331={
offsetof(struct _tuple47,f1),(void*)& Cyc__genrep_318};static struct _tuple6 Cyc__gentuple_332={
offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_231};static struct _tuple6*Cyc__genarr_333[
2]={& Cyc__gentuple_331,& Cyc__gentuple_332};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_317={4,sizeof(struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_333),(
void*)((struct _tuple6**)Cyc__genarr_333),(void*)((struct _tuple6**)Cyc__genarr_333
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_316={1,1,(void*)((void*)&
Cyc__genrep_317)};static char _tmp2D2[5]="List";static struct _tagged_arr Cyc__genname_337={
_tmp2D2,_tmp2D2,_tmp2D2 + 5};static char _tmp2D3[3]="hd";static struct _tuple5 Cyc__gentuple_334={
offsetof(struct Cyc_List_List,hd),{_tmp2D3,_tmp2D3,_tmp2D3 + 3},(void*)& Cyc__genrep_316};
static char _tmp2D4[3]="tl";static struct _tuple5 Cyc__gentuple_335={offsetof(struct
Cyc_List_List,tl),{_tmp2D4,_tmp2D4,_tmp2D4 + 3},(void*)& Cyc__genrep_315};static
struct _tuple5*Cyc__genarr_336[2]={& Cyc__gentuple_334,& Cyc__gentuple_335};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_337,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_336),(void*)((struct _tuple5**)Cyc__genarr_336),(void*)((
struct _tuple5**)Cyc__genarr_336 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_315={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};static struct _tuple6 Cyc__gentuple_398={offsetof(struct
_tuple48,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_399={
offsetof(struct _tuple48,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_400={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_297};
static struct _tuple6 Cyc__gentuple_401={offsetof(struct _tuple48,f4),(void*)& Cyc__genrep_315};
static struct _tuple6*Cyc__genarr_402[4]={& Cyc__gentuple_398,& Cyc__gentuple_399,&
Cyc__gentuple_400,& Cyc__gentuple_401};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_314={
4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_402),(void*)((
struct _tuple6**)Cyc__genarr_402),(void*)((struct _tuple6**)Cyc__genarr_402 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_269;struct _tuple49{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*
f4;};static struct _tuple6 Cyc__gentuple_309={offsetof(struct _tuple49,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_310={offsetof(struct _tuple49,f2),(
void*)((void*)& Cyc__genrep_287)};static struct _tuple6 Cyc__gentuple_311={offsetof(
struct _tuple49,f3),(void*)& Cyc__genrep_270};static struct _tuple6 Cyc__gentuple_312={
offsetof(struct _tuple49,f4),(void*)& Cyc__genrep_236};static struct _tuple6*Cyc__genarr_313[
4]={& Cyc__gentuple_309,& Cyc__gentuple_310,& Cyc__gentuple_311,& Cyc__gentuple_312};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_269={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_313),(void*)((struct _tuple6**)Cyc__genarr_313),(
void*)((struct _tuple6**)Cyc__genarr_313 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_254;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_255;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_256;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_76;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_77;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep;
static char _tmp2D9[10]="Enumfield";static struct _tagged_arr Cyc__genname_917={
_tmp2D9,_tmp2D9,_tmp2D9 + 10};static char _tmp2DA[5]="name";static struct _tuple5 Cyc__gentuple_913={
offsetof(struct Cyc_Absyn_Enumfield,name),{_tmp2DA,_tmp2DA,_tmp2DA + 5},(void*)&
Cyc__genrep_10};static char _tmp2DB[4]="tag";static struct _tuple5 Cyc__gentuple_914={
offsetof(struct Cyc_Absyn_Enumfield,tag),{_tmp2DB,_tmp2DB,_tmp2DB + 4},(void*)& Cyc__genrep_78};
static char _tmp2DC[4]="loc";static struct _tuple5 Cyc__gentuple_915={offsetof(struct
Cyc_Absyn_Enumfield,loc),{_tmp2DC,_tmp2DC,_tmp2DC + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_916[3]={& Cyc__gentuple_913,& Cyc__gentuple_914,&
Cyc__gentuple_915};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_917,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_916),(void*)((struct _tuple5**)Cyc__genarr_916),(void*)((
struct _tuple5**)Cyc__genarr_916 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp2DF[5]="List";
static struct _tagged_arr Cyc__genname_921={_tmp2DF,_tmp2DF,_tmp2DF + 5};static char
_tmp2E0[3]="hd";static struct _tuple5 Cyc__gentuple_918={offsetof(struct Cyc_List_List,hd),{
_tmp2E0,_tmp2E0,_tmp2E0 + 3},(void*)& Cyc__genrep_77};static char _tmp2E1[3]="tl";
static struct _tuple5 Cyc__gentuple_919={offsetof(struct Cyc_List_List,tl),{_tmp2E1,
_tmp2E1,_tmp2E1 + 3},(void*)& Cyc__genrep_76};static struct _tuple5*Cyc__genarr_920[
2]={& Cyc__gentuple_918,& Cyc__gentuple_919};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_921,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_920),(void*)((struct _tuple5**)Cyc__genarr_920),(void*)((
struct _tuple5**)Cyc__genarr_920 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_76={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp2E4[4]="Opt";static struct _tagged_arr Cyc__genname_259={_tmp2E4,_tmp2E4,
_tmp2E4 + 4};static char _tmp2E5[2]="v";static struct _tuple5 Cyc__gentuple_257={
offsetof(struct Cyc_Core_Opt,v),{_tmp2E5,_tmp2E5,_tmp2E5 + 2},(void*)& Cyc__genrep_76};
static struct _tuple5*Cyc__genarr_258[1]={& Cyc__gentuple_257};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_259,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_258),(
void*)((struct _tuple5**)Cyc__genarr_258),(void*)((struct _tuple5**)Cyc__genarr_258
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_256={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};static char _tmp2E8[9]="Enumdecl";
static struct _tagged_arr Cyc__genname_264={_tmp2E8,_tmp2E8,_tmp2E8 + 9};static char
_tmp2E9[3]="sc";static struct _tuple5 Cyc__gentuple_260={offsetof(struct Cyc_Absyn_Enumdecl,sc),{
_tmp2E9,_tmp2E9,_tmp2E9 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp2EA[5]="name";
static struct _tuple5 Cyc__gentuple_261={offsetof(struct Cyc_Absyn_Enumdecl,name),{
_tmp2EA,_tmp2EA,_tmp2EA + 5},(void*)& Cyc__genrep_10};static char _tmp2EB[7]="fields";
static struct _tuple5 Cyc__gentuple_262={offsetof(struct Cyc_Absyn_Enumdecl,fields),{
_tmp2EB,_tmp2EB,_tmp2EB + 7},(void*)& Cyc__genrep_256};static struct _tuple5*Cyc__genarr_263[
3]={& Cyc__gentuple_260,& Cyc__gentuple_261,& Cyc__gentuple_262};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_264,sizeof(
struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_263),(void*)((
struct _tuple5**)Cyc__genarr_263),(void*)((struct _tuple5**)Cyc__genarr_263 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_255={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple50{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_265={offsetof(struct _tuple50,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_266={offsetof(struct _tuple50,f2),(
void*)& Cyc__genrep_255};static struct _tuple6 Cyc__gentuple_267={offsetof(struct
_tuple50,f3),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_268[3]={&
Cyc__gentuple_265,& Cyc__gentuple_266,& Cyc__gentuple_267};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_254={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_268),(
void*)((struct _tuple6**)Cyc__genarr_268),(void*)((struct _tuple6**)Cyc__genarr_268
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_249;struct _tuple51{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_250={
offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_251={
offsetof(struct _tuple51,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_252={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_77};
static struct _tuple6*Cyc__genarr_253[3]={& Cyc__gentuple_250,& Cyc__gentuple_251,&
Cyc__gentuple_252};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_249={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_253),(void*)((
struct _tuple6**)Cyc__genarr_253),(void*)((struct _tuple6**)Cyc__genarr_253 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_245;struct _tuple52{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_246={offsetof(struct
_tuple52,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_247={
offsetof(struct _tuple52,f2),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_248[
2]={& Cyc__gentuple_246,& Cyc__gentuple_247};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_245={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_248),(
void*)((struct _tuple6**)Cyc__genarr_248),(void*)((struct _tuple6**)Cyc__genarr_248
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_235;struct _tuple53{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_241={
offsetof(struct _tuple53,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_242={
offsetof(struct _tuple53,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_243={
offsetof(struct _tuple53,f3),(void*)& Cyc__genrep_236};static struct _tuple6*Cyc__genarr_244[
3]={& Cyc__gentuple_241,& Cyc__gentuple_242,& Cyc__gentuple_243};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_235={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_244),(
void*)((struct _tuple6**)Cyc__genarr_244),(void*)((struct _tuple6**)Cyc__genarr_244
+ 3)}};static char _tmp2F2[7]="Wild_p";static struct _tuple7 Cyc__gentuple_232={0,{
_tmp2F2,_tmp2F2,_tmp2F2 + 7}};static char _tmp2F3[7]="Null_p";static struct _tuple7
Cyc__gentuple_233={1,{_tmp2F3,_tmp2F3,_tmp2F3 + 7}};static struct _tuple7*Cyc__genarr_234[
2]={& Cyc__gentuple_232,& Cyc__gentuple_233};static char _tmp2F4[6]="Var_p";static
struct _tuple5 Cyc__gentuple_429={0,{_tmp2F4,_tmp2F4,_tmp2F4 + 6},(void*)& Cyc__genrep_403};
static char _tmp2F5[6]="Int_p";static struct _tuple5 Cyc__gentuple_430={1,{_tmp2F5,
_tmp2F5,_tmp2F5 + 6},(void*)& Cyc__genrep_419};static char _tmp2F6[7]="Char_p";
static struct _tuple5 Cyc__gentuple_431={2,{_tmp2F6,_tmp2F6,_tmp2F6 + 7},(void*)& Cyc__genrep_415};
static char _tmp2F7[8]="Float_p";static struct _tuple5 Cyc__gentuple_432={3,{_tmp2F7,
_tmp2F7,_tmp2F7 + 8},(void*)& Cyc__genrep_117};static char _tmp2F8[8]="Tuple_p";
static struct _tuple5 Cyc__gentuple_433={4,{_tmp2F8,_tmp2F8,_tmp2F8 + 8},(void*)& Cyc__genrep_411};
static char _tmp2F9[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_434={5,{
_tmp2F9,_tmp2F9,_tmp2F9 + 10},(void*)& Cyc__genrep_407};static char _tmp2FA[12]="Reference_p";
static struct _tuple5 Cyc__gentuple_435={6,{_tmp2FA,_tmp2FA,_tmp2FA + 12},(void*)&
Cyc__genrep_403};static char _tmp2FB[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_436={
7,{_tmp2FB,_tmp2FB,_tmp2FB + 7},(void*)& Cyc__genrep_314};static char _tmp2FC[9]="Tunion_p";
static struct _tuple5 Cyc__gentuple_437={8,{_tmp2FC,_tmp2FC,_tmp2FC + 9},(void*)& Cyc__genrep_269};
static char _tmp2FD[7]="Enum_p";static struct _tuple5 Cyc__gentuple_438={9,{_tmp2FD,
_tmp2FD,_tmp2FD + 7},(void*)& Cyc__genrep_254};static char _tmp2FE[11]="AnonEnum_p";
static struct _tuple5 Cyc__gentuple_439={10,{_tmp2FE,_tmp2FE,_tmp2FE + 11},(void*)&
Cyc__genrep_249};static char _tmp2FF[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_440={
11,{_tmp2FF,_tmp2FF,_tmp2FF + 12},(void*)& Cyc__genrep_245};static char _tmp300[14]="UnknownCall_p";
static struct _tuple5 Cyc__gentuple_441={12,{_tmp300,_tmp300,_tmp300 + 14},(void*)&
Cyc__genrep_235};static struct _tuple5*Cyc__genarr_442[13]={& Cyc__gentuple_429,&
Cyc__gentuple_430,& Cyc__gentuple_431,& Cyc__gentuple_432,& Cyc__gentuple_433,& Cyc__gentuple_434,&
Cyc__gentuple_435,& Cyc__gentuple_436,& Cyc__gentuple_437,& Cyc__gentuple_438,& Cyc__gentuple_439,&
Cyc__gentuple_440,& Cyc__gentuple_441};static char _tmp302[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,{_tmp302,_tmp302,_tmp302 + 8},{(void*)((struct _tuple7**)
Cyc__genarr_234),(void*)((struct _tuple7**)Cyc__genarr_234),(void*)((struct
_tuple7**)Cyc__genarr_234 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_442),(void*)((
struct _tuple5**)Cyc__genarr_442),(void*)((struct _tuple5**)Cyc__genarr_442 + 13)}};
static char _tmp303[4]="Pat";static struct _tagged_arr Cyc__genname_447={_tmp303,
_tmp303,_tmp303 + 4};static char _tmp304[2]="r";static struct _tuple5 Cyc__gentuple_443={
offsetof(struct Cyc_Absyn_Pat,r),{_tmp304,_tmp304,_tmp304 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp305[5]="topt";static struct _tuple5 Cyc__gentuple_444={offsetof(
struct Cyc_Absyn_Pat,topt),{_tmp305,_tmp305,_tmp305 + 5},(void*)& Cyc__genrep_43};
static char _tmp306[4]="loc";static struct _tuple5 Cyc__gentuple_445={offsetof(struct
Cyc_Absyn_Pat,loc),{_tmp306,_tmp306,_tmp306 + 4},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_446[3]={& Cyc__gentuple_443,& Cyc__gentuple_444,& Cyc__gentuple_445};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct _tagged_arr*)&
Cyc__genname_447,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct _tuple5**)Cyc__genarr_446),(
void*)((struct _tuple5**)Cyc__genarr_446),(void*)((struct _tuple5**)Cyc__genarr_446
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231={1,1,(void*)((void*)&
Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_132;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp309[5]="List";static
struct _tagged_arr Cyc__genname_158={_tmp309,_tmp309,_tmp309 + 5};static char _tmp30A[
3]="hd";static struct _tuple5 Cyc__gentuple_155={offsetof(struct Cyc_List_List,hd),{
_tmp30A,_tmp30A,_tmp30A + 3},(void*)& Cyc__genrep_133};static char _tmp30B[3]="tl";
static struct _tuple5 Cyc__gentuple_156={offsetof(struct Cyc_List_List,tl),{_tmp30B,
_tmp30B,_tmp30B + 3},(void*)& Cyc__genrep_132};static struct _tuple5*Cyc__genarr_157[
2]={& Cyc__gentuple_155,& Cyc__gentuple_156};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_158,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_157),(void*)((struct _tuple5**)Cyc__genarr_157),(void*)((
struct _tuple5**)Cyc__genarr_157 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_132={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp30E[4]="Opt";static struct _tagged_arr Cyc__genname_161={_tmp30E,_tmp30E,
_tmp30E + 4};static char _tmp30F[2]="v";static struct _tuple5 Cyc__gentuple_159={
offsetof(struct Cyc_Core_Opt,v),{_tmp30F,_tmp30F,_tmp30F + 2},(void*)& Cyc__genrep_132};
static struct _tuple5*Cyc__genarr_160[1]={& Cyc__gentuple_159};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_161,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_160),(
void*)((struct _tuple5**)Cyc__genarr_160),(void*)((struct _tuple5**)Cyc__genarr_160
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};static char _tmp312[14]="Switch_clause";
static struct _tagged_arr Cyc__genname_454={_tmp312,_tmp312,_tmp312 + 14};static char
_tmp313[8]="pattern";static struct _tuple5 Cyc__gentuple_448={offsetof(struct Cyc_Absyn_Switch_clause,pattern),{
_tmp313,_tmp313,_tmp313 + 8},(void*)& Cyc__genrep_231};static char _tmp314[9]="pat_vars";
static struct _tuple5 Cyc__gentuple_449={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),{
_tmp314,_tmp314,_tmp314 + 9},(void*)& Cyc__genrep_131};static char _tmp315[13]="where_clause";
static struct _tuple5 Cyc__gentuple_450={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),{
_tmp315,_tmp315,_tmp315 + 13},(void*)& Cyc__genrep_78};static char _tmp316[5]="body";
static struct _tuple5 Cyc__gentuple_451={offsetof(struct Cyc_Absyn_Switch_clause,body),{
_tmp316,_tmp316,_tmp316 + 5},(void*)& Cyc__genrep_162};static char _tmp317[4]="loc";
static struct _tuple5 Cyc__gentuple_452={offsetof(struct Cyc_Absyn_Switch_clause,loc),{
_tmp317,_tmp317,_tmp317 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_453[
5]={& Cyc__gentuple_448,& Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451,&
Cyc__gentuple_452};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_454,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_453),(void*)((struct _tuple5**)Cyc__genarr_453),(
void*)((struct _tuple5**)Cyc__genarr_453 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_230={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp31A[5]="List";static struct _tagged_arr Cyc__genname_458={_tmp31A,_tmp31A,
_tmp31A + 5};static char _tmp31B[3]="hd";static struct _tuple5 Cyc__gentuple_455={
offsetof(struct Cyc_List_List,hd),{_tmp31B,_tmp31B,_tmp31B + 3},(void*)((void*)&
Cyc__genrep_230)};static char _tmp31C[3]="tl";static struct _tuple5 Cyc__gentuple_456={
offsetof(struct Cyc_List_List,tl),{_tmp31C,_tmp31C,_tmp31C + 3},(void*)& Cyc__genrep_229};
static struct _tuple5*Cyc__genarr_457[2]={& Cyc__gentuple_455,& Cyc__gentuple_456};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_458,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_457),(void*)((struct _tuple5**)Cyc__genarr_457),(void*)((
struct _tuple5**)Cyc__genarr_457 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_229={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple54{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_510={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_513[3]={& Cyc__gentuple_510,& Cyc__gentuple_511,&
Cyc__gentuple_512};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_509={4,
sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_513),(void*)((
struct _tuple6**)Cyc__genarr_513),(void*)((struct _tuple6**)Cyc__genarr_513 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_493;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_494;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_495;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep;static char _tmp320[15]="SwitchC_clause";
static struct _tagged_arr Cyc__genname_500={_tmp320,_tmp320,_tmp320 + 15};static char
_tmp321[9]="cnst_exp";static struct _tuple5 Cyc__gentuple_496={offsetof(struct Cyc_Absyn_SwitchC_clause,cnst_exp),{
_tmp321,_tmp321,_tmp321 + 9},(void*)& Cyc__genrep_78};static char _tmp322[5]="body";
static struct _tuple5 Cyc__gentuple_497={offsetof(struct Cyc_Absyn_SwitchC_clause,body),{
_tmp322,_tmp322,_tmp322 + 5},(void*)& Cyc__genrep_162};static char _tmp323[4]="loc";
static struct _tuple5 Cyc__gentuple_498={offsetof(struct Cyc_Absyn_SwitchC_clause,loc),{
_tmp323,_tmp323,_tmp323 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_499[
3]={& Cyc__gentuple_496,& Cyc__gentuple_497,& Cyc__gentuple_498};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep={3,(struct _tagged_arr*)& Cyc__genname_500,
sizeof(struct Cyc_Absyn_SwitchC_clause),{(void*)((struct _tuple5**)Cyc__genarr_499),(
void*)((struct _tuple5**)Cyc__genarr_499),(void*)((struct _tuple5**)Cyc__genarr_499
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_495={1,1,(void*)((void*)&
Cyc_struct_Absyn_SwitchC_clause_rep)};static char _tmp326[5]="List";static struct
_tagged_arr Cyc__genname_504={_tmp326,_tmp326,_tmp326 + 5};static char _tmp327[3]="hd";
static struct _tuple5 Cyc__gentuple_501={offsetof(struct Cyc_List_List,hd),{_tmp327,
_tmp327,_tmp327 + 3},(void*)& Cyc__genrep_495};static char _tmp328[3]="tl";static
struct _tuple5 Cyc__gentuple_502={offsetof(struct Cyc_List_List,tl),{_tmp328,
_tmp328,_tmp328 + 3},(void*)& Cyc__genrep_494};static struct _tuple5*Cyc__genarr_503[
2]={& Cyc__gentuple_501,& Cyc__gentuple_502};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_504,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_503),(void*)((struct _tuple5**)Cyc__genarr_503),(void*)((
struct _tuple5**)Cyc__genarr_503 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_494={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)};static
struct _tuple6 Cyc__gentuple_505={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_507={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_494};
static struct _tuple6*Cyc__genarr_508[3]={& Cyc__gentuple_505,& Cyc__gentuple_506,&
Cyc__gentuple_507};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_493={4,
sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_508),(void*)((
struct _tuple6**)Cyc__genarr_508),(void*)((struct _tuple6**)Cyc__genarr_508 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_482;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_484;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp32C[5]="List";static struct _tagged_arr Cyc__genname_488={_tmp32C,
_tmp32C,_tmp32C + 5};static char _tmp32D[3]="hd";static struct _tuple5 Cyc__gentuple_485={
offsetof(struct Cyc_List_List,hd),{_tmp32D,_tmp32D,_tmp32D + 3},(void*)& Cyc__genrep_82};
static char _tmp32E[3]="tl";static struct _tuple5 Cyc__gentuple_486={offsetof(struct
Cyc_List_List,tl),{_tmp32E,_tmp32E,_tmp32E + 3},(void*)& Cyc__genrep_484};static
struct _tuple5*Cyc__genarr_487[2]={& Cyc__gentuple_485,& Cyc__gentuple_486};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_488,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_487),(void*)((struct _tuple5**)Cyc__genarr_487),(void*)((
struct _tuple5**)Cyc__genarr_487 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_484={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_483;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_483={1,1,(void*)((
void*)& Cyc__genrep_230)};struct _tuple55{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_489={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_490={
offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_484};static struct _tuple6 Cyc__gentuple_491={
offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_483};static struct _tuple6*Cyc__genarr_492[
3]={& Cyc__gentuple_489,& Cyc__gentuple_490,& Cyc__gentuple_491};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_482={4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_492),(
void*)((struct _tuple6**)Cyc__genarr_492),(void*)((struct _tuple6**)Cyc__genarr_492
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_477;struct _tuple56{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_478={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_479={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_480={offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_162};
static struct _tuple6*Cyc__genarr_481[3]={& Cyc__gentuple_478,& Cyc__gentuple_479,&
Cyc__gentuple_480};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_477={4,
sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_481),(void*)((
struct _tuple6**)Cyc__genarr_481),(void*)((struct _tuple6**)Cyc__genarr_481 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_473;static struct _tuple6 Cyc__gentuple_474={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_475={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_162};static struct _tuple6*Cyc__genarr_476[
2]={& Cyc__gentuple_474,& Cyc__gentuple_475};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_473={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_476),(
void*)((struct _tuple6**)Cyc__genarr_476),(void*)((struct _tuple6**)Cyc__genarr_476
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_468;static struct _tuple6
Cyc__gentuple_469={offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_470={offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_471={offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_162};
static struct _tuple6*Cyc__genarr_472[3]={& Cyc__gentuple_469,& Cyc__gentuple_470,&
Cyc__gentuple_471};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_468={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_472),(void*)((
struct _tuple6**)Cyc__genarr_472),(void*)((struct _tuple6**)Cyc__genarr_472 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_463;struct _tuple57{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_464={
offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_465={
offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_162};static struct _tuple6 Cyc__gentuple_466={
offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_171};static struct _tuple6*Cyc__genarr_467[
3]={& Cyc__gentuple_464,& Cyc__gentuple_465,& Cyc__gentuple_466};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_463={4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_467),(
void*)((struct _tuple6**)Cyc__genarr_467),(void*)((struct _tuple6**)Cyc__genarr_467
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_228;struct _tuple58{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_459={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_460={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_162};
static struct _tuple6 Cyc__gentuple_461={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_462[3]={& Cyc__gentuple_459,& Cyc__gentuple_460,&
Cyc__gentuple_461};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_228={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_462),(void*)((
struct _tuple6**)Cyc__genarr_462),(void*)((struct _tuple6**)Cyc__genarr_462 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_184;struct _tuple59{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_222={offsetof(struct _tuple59,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_223={offsetof(struct _tuple59,f2),(
void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_224={offsetof(struct
_tuple59,f3),(void*)& Cyc__genrep_133};static struct _tuple6 Cyc__gentuple_225={
offsetof(struct _tuple59,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_226={offsetof(struct _tuple59,f5),(void*)& Cyc__genrep_162};static
struct _tuple6*Cyc__genarr_227[5]={& Cyc__gentuple_222,& Cyc__gentuple_223,& Cyc__gentuple_224,&
Cyc__gentuple_225,& Cyc__gentuple_226};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_184={
4,sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_227),(void*)((
struct _tuple6**)Cyc__genarr_227),(void*)((struct _tuple6**)Cyc__genarr_227 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_170;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_forarray_info_t_rep;static char _tmp339[13]="ForArrayInfo";static struct
_tagged_arr Cyc__genname_180={_tmp339,_tmp339,_tmp339 + 13};static char _tmp33A[6]="defns";
static struct _tuple5 Cyc__gentuple_175={offsetof(struct Cyc_Absyn_ForArrayInfo,defns),{
_tmp33A,_tmp33A,_tmp33A + 6},(void*)& Cyc__genrep_132};static char _tmp33B[10]="condition";
static struct _tuple5 Cyc__gentuple_176={offsetof(struct Cyc_Absyn_ForArrayInfo,condition),{
_tmp33B,_tmp33B,_tmp33B + 10},(void*)& Cyc__genrep_171};static char _tmp33C[6]="delta";
static struct _tuple5 Cyc__gentuple_177={offsetof(struct Cyc_Absyn_ForArrayInfo,delta),{
_tmp33C,_tmp33C,_tmp33C + 6},(void*)& Cyc__genrep_171};static char _tmp33D[5]="body";
static struct _tuple5 Cyc__gentuple_178={offsetof(struct Cyc_Absyn_ForArrayInfo,body),{
_tmp33D,_tmp33D,_tmp33D + 5},(void*)& Cyc__genrep_162};static struct _tuple5*Cyc__genarr_179[
4]={& Cyc__gentuple_175,& Cyc__gentuple_176,& Cyc__gentuple_177,& Cyc__gentuple_178};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_forarray_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_180,sizeof(struct Cyc_Absyn_ForArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_179),(void*)((struct _tuple5**)Cyc__genarr_179),(void*)((
struct _tuple5**)Cyc__genarr_179 + 4)}};struct _tuple60{unsigned int f1;struct Cyc_Absyn_ForArrayInfo
f2;};static struct _tuple6 Cyc__gentuple_181={offsetof(struct _tuple60,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_182={offsetof(struct _tuple60,f2),(
void*)& Cyc_Absyn_forarray_info_t_rep};static struct _tuple6*Cyc__genarr_183[2]={&
Cyc__gentuple_181,& Cyc__gentuple_182};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_170={
4,sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_183),(void*)((
struct _tuple6**)Cyc__genarr_183),(void*)((struct _tuple6**)Cyc__genarr_183 + 2)}};
static char _tmp340[7]="Skip_s";static struct _tuple7 Cyc__gentuple_168={0,{_tmp340,
_tmp340,_tmp340 + 7}};static struct _tuple7*Cyc__genarr_169[1]={& Cyc__gentuple_168};
static char _tmp341[6]="Exp_s";static struct _tuple5 Cyc__gentuple_551={0,{_tmp341,
_tmp341,_tmp341 + 6},(void*)& Cyc__genrep_81};static char _tmp342[6]="Seq_s";static
struct _tuple5 Cyc__gentuple_552={1,{_tmp342,_tmp342,_tmp342 + 6},(void*)& Cyc__genrep_546};
static char _tmp343[9]="Return_s";static struct _tuple5 Cyc__gentuple_553={2,{_tmp343,
_tmp343,_tmp343 + 9},(void*)& Cyc__genrep_542};static char _tmp344[13]="IfThenElse_s";
static struct _tuple5 Cyc__gentuple_554={3,{_tmp344,_tmp344,_tmp344 + 13},(void*)&
Cyc__genrep_536};static char _tmp345[8]="While_s";static struct _tuple5 Cyc__gentuple_555={
4,{_tmp345,_tmp345,_tmp345 + 8},(void*)& Cyc__genrep_531};static char _tmp346[8]="Break_s";
static struct _tuple5 Cyc__gentuple_556={5,{_tmp346,_tmp346,_tmp346 + 8},(void*)& Cyc__genrep_527};
static char _tmp347[11]="Continue_s";static struct _tuple5 Cyc__gentuple_557={6,{
_tmp347,_tmp347,_tmp347 + 11},(void*)& Cyc__genrep_527};static char _tmp348[7]="Goto_s";
static struct _tuple5 Cyc__gentuple_558={7,{_tmp348,_tmp348,_tmp348 + 7},(void*)& Cyc__genrep_521};
static char _tmp349[6]="For_s";static struct _tuple5 Cyc__gentuple_559={8,{_tmp349,
_tmp349,_tmp349 + 6},(void*)& Cyc__genrep_514};static char _tmp34A[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_560={9,{_tmp34A,_tmp34A,_tmp34A + 9},(void*)& Cyc__genrep_509};
static char _tmp34B[10]="SwitchC_s";static struct _tuple5 Cyc__gentuple_561={10,{
_tmp34B,_tmp34B,_tmp34B + 10},(void*)& Cyc__genrep_493};static char _tmp34C[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_562={11,{_tmp34C,_tmp34C,_tmp34C + 11},(void*)&
Cyc__genrep_482};static char _tmp34D[7]="Decl_s";static struct _tuple5 Cyc__gentuple_563={
12,{_tmp34D,_tmp34D,_tmp34D + 7},(void*)& Cyc__genrep_477};static char _tmp34E[6]="Cut_s";
static struct _tuple5 Cyc__gentuple_564={13,{_tmp34E,_tmp34E,_tmp34E + 6},(void*)&
Cyc__genrep_473};static char _tmp34F[9]="Splice_s";static struct _tuple5 Cyc__gentuple_565={
14,{_tmp34F,_tmp34F,_tmp34F + 9},(void*)& Cyc__genrep_473};static char _tmp350[8]="Label_s";
static struct _tuple5 Cyc__gentuple_566={15,{_tmp350,_tmp350,_tmp350 + 8},(void*)&
Cyc__genrep_468};static char _tmp351[5]="Do_s";static struct _tuple5 Cyc__gentuple_567={
16,{_tmp351,_tmp351,_tmp351 + 5},(void*)& Cyc__genrep_463};static char _tmp352[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_568={17,{_tmp352,_tmp352,_tmp352 + 11},(void*)&
Cyc__genrep_228};static char _tmp353[9]="Region_s";static struct _tuple5 Cyc__gentuple_569={
18,{_tmp353,_tmp353,_tmp353 + 9},(void*)& Cyc__genrep_184};static char _tmp354[11]="ForArray_s";
static struct _tuple5 Cyc__gentuple_570={19,{_tmp354,_tmp354,_tmp354 + 11},(void*)&
Cyc__genrep_170};static char _tmp355[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_571={
20,{_tmp355,_tmp355,_tmp355 + 14},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_572[
21]={& Cyc__gentuple_551,& Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554,&
Cyc__gentuple_555,& Cyc__gentuple_556,& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,&
Cyc__gentuple_560,& Cyc__gentuple_561,& Cyc__gentuple_562,& Cyc__gentuple_563,& Cyc__gentuple_564,&
Cyc__gentuple_565,& Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568,& Cyc__gentuple_569,&
Cyc__gentuple_570,& Cyc__gentuple_571};static char _tmp357[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_stmt_t_rep={5,{_tmp357,_tmp357,_tmp357 + 9},{(void*)((struct _tuple7**)
Cyc__genarr_169),(void*)((struct _tuple7**)Cyc__genarr_169),(void*)((struct
_tuple7**)Cyc__genarr_169 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_572),(void*)((
struct _tuple5**)Cyc__genarr_572),(void*)((struct _tuple5**)Cyc__genarr_572 + 21)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_163;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep;static char _tmp358[5]="List";static
struct _tagged_arr Cyc__genname_167={_tmp358,_tmp358,_tmp358 + 5};static char _tmp359[
3]="hd";static struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_List_List,hd),{
_tmp359,_tmp359,_tmp359 + 3},(void*)& Cyc__genrep_162};static char _tmp35A[3]="tl";
static struct _tuple5 Cyc__gentuple_165={offsetof(struct Cyc_List_List,tl),{_tmp35A,
_tmp35A,_tmp35A + 3},(void*)& Cyc__genrep_163};static struct _tuple5*Cyc__genarr_166[
2]={& Cyc__gentuple_164,& Cyc__gentuple_165};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_167,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_166),(void*)((struct _tuple5**)Cyc__genarr_166),(void*)((
struct _tuple5**)Cyc__genarr_166 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_163={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_79[0]={};static char
_tmp35E[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,{_tmp35E,_tmp35E,_tmp35E + 11},{(void*)((struct _tuple8**)Cyc__genarr_79),(void*)((
struct _tuple8**)Cyc__genarr_79),(void*)((struct _tuple8**)Cyc__genarr_79 + 0)}};
static char _tmp35F[5]="Stmt";static struct _tagged_arr Cyc__genname_579={_tmp35F,
_tmp35F,_tmp35F + 5};static char _tmp360[2]="r";static struct _tuple5 Cyc__gentuple_573={
offsetof(struct Cyc_Absyn_Stmt,r),{_tmp360,_tmp360,_tmp360 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp361[4]="loc";static struct _tuple5 Cyc__gentuple_574={offsetof(struct
Cyc_Absyn_Stmt,loc),{_tmp361,_tmp361,_tmp361 + 4},(void*)& Cyc__genrep_2};static
char _tmp362[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_575={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),{_tmp362,_tmp362,_tmp362 + 16},(
void*)& Cyc__genrep_163};static char _tmp363[10]="try_depth";static struct _tuple5 Cyc__gentuple_576={
offsetof(struct Cyc_Absyn_Stmt,try_depth),{_tmp363,_tmp363,_tmp363 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp364[6]="annot";static struct _tuple5 Cyc__gentuple_577={
offsetof(struct Cyc_Absyn_Stmt,annot),{_tmp364,_tmp364,_tmp364 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_578[5]={& Cyc__gentuple_573,& Cyc__gentuple_574,&
Cyc__gentuple_575,& Cyc__gentuple_576,& Cyc__gentuple_577};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)& Cyc__genname_579,sizeof(struct
Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_578),(void*)((struct
_tuple5**)Cyc__genarr_578),(void*)((struct _tuple5**)Cyc__genarr_578 + 5)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_162={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
static char _tmp367[7]="Fndecl";static struct _tagged_arr Cyc__genname_617={_tmp367,
_tmp367,_tmp367 + 7};static char _tmp368[3]="sc";static struct _tuple5 Cyc__gentuple_602={
offsetof(struct Cyc_Absyn_Fndecl,sc),{_tmp368,_tmp368,_tmp368 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp369[10]="is_inline";static struct _tuple5 Cyc__gentuple_603={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),{_tmp369,_tmp369,_tmp369 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp36A[5]="name";static struct _tuple5 Cyc__gentuple_604={
offsetof(struct Cyc_Absyn_Fndecl,name),{_tmp36A,_tmp36A,_tmp36A + 5},(void*)& Cyc__genrep_10};
static char _tmp36B[4]="tvs";static struct _tuple5 Cyc__gentuple_605={offsetof(struct
Cyc_Absyn_Fndecl,tvs),{_tmp36B,_tmp36B,_tmp36B + 4},(void*)& Cyc__genrep_297};
static char _tmp36C[7]="effect";static struct _tuple5 Cyc__gentuple_606={offsetof(
struct Cyc_Absyn_Fndecl,effect),{_tmp36C,_tmp36C,_tmp36C + 7},(void*)& Cyc__genrep_43};
static char _tmp36D[9]="ret_type";static struct _tuple5 Cyc__gentuple_607={offsetof(
struct Cyc_Absyn_Fndecl,ret_type),{_tmp36D,_tmp36D,_tmp36D + 9},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp36E[5]="args";static struct _tuple5 Cyc__gentuple_608={
offsetof(struct Cyc_Absyn_Fndecl,args),{_tmp36E,_tmp36E,_tmp36E + 5},(void*)& Cyc__genrep_591};
static char _tmp36F[10]="c_varargs";static struct _tuple5 Cyc__gentuple_609={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),{_tmp36F,_tmp36F,_tmp36F + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp370[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_610={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),{_tmp370,_tmp370,
_tmp370 + 12},(void*)& Cyc__genrep_580};static char _tmp371[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_611={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),{_tmp371,
_tmp371,_tmp371 + 7},(void*)& Cyc__genrep_356};static char _tmp372[5]="body";static
struct _tuple5 Cyc__gentuple_612={offsetof(struct Cyc_Absyn_Fndecl,body),{_tmp372,
_tmp372,_tmp372 + 5},(void*)& Cyc__genrep_162};static char _tmp373[11]="cached_typ";
static struct _tuple5 Cyc__gentuple_613={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),{
_tmp373,_tmp373,_tmp373 + 11},(void*)& Cyc__genrep_43};static char _tmp374[15]="param_vardecls";
static struct _tuple5 Cyc__gentuple_614={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),{
_tmp374,_tmp374,_tmp374 + 15},(void*)& Cyc__genrep_131};static char _tmp375[11]="attributes";
static struct _tuple5 Cyc__gentuple_615={offsetof(struct Cyc_Absyn_Fndecl,attributes),{
_tmp375,_tmp375,_tmp375 + 11},(void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_616[
14]={& Cyc__gentuple_602,& Cyc__gentuple_603,& Cyc__gentuple_604,& Cyc__gentuple_605,&
Cyc__gentuple_606,& Cyc__gentuple_607,& Cyc__gentuple_608,& Cyc__gentuple_609,& Cyc__gentuple_610,&
Cyc__gentuple_611,& Cyc__gentuple_612,& Cyc__gentuple_613,& Cyc__gentuple_614,& Cyc__gentuple_615};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)&
Cyc__genname_617,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_616),(
void*)((struct _tuple5**)Cyc__genarr_616),(void*)((struct _tuple5**)Cyc__genarr_616
+ 14)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_87={1,1,(void*)((void*)&
Cyc_struct_Absyn_Fndecl_rep)};struct _tuple61{unsigned int f1;struct Cyc_Absyn_Fndecl*
f2;};static struct _tuple6 Cyc__gentuple_618={offsetof(struct _tuple61,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_619={offsetof(struct _tuple61,f2),(
void*)& Cyc__genrep_87};static struct _tuple6*Cyc__genarr_620[2]={& Cyc__gentuple_618,&
Cyc__gentuple_619};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_86={4,sizeof(
struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_620),(void*)((struct
_tuple6**)Cyc__genarr_620),(void*)((struct _tuple6**)Cyc__genarr_620 + 2)}};static
char _tmp379[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_826={0,{_tmp379,
_tmp379,_tmp379 + 13}};static struct _tuple7*Cyc__genarr_827[1]={& Cyc__gentuple_826};
static char _tmp37A[9]="Global_b";static struct _tuple5 Cyc__gentuple_828={0,{_tmp37A,
_tmp37A,_tmp37A + 9},(void*)& Cyc__genrep_403};static char _tmp37B[10]="Funname_b";
static struct _tuple5 Cyc__gentuple_829={1,{_tmp37B,_tmp37B,_tmp37B + 10},(void*)&
Cyc__genrep_86};static char _tmp37C[8]="Param_b";static struct _tuple5 Cyc__gentuple_830={
2,{_tmp37C,_tmp37C,_tmp37C + 8},(void*)& Cyc__genrep_403};static char _tmp37D[8]="Local_b";
static struct _tuple5 Cyc__gentuple_831={3,{_tmp37D,_tmp37D,_tmp37D + 8},(void*)& Cyc__genrep_403};
static char _tmp37E[6]="Pat_b";static struct _tuple5 Cyc__gentuple_832={4,{_tmp37E,
_tmp37E,_tmp37E + 6},(void*)& Cyc__genrep_403};static struct _tuple5*Cyc__genarr_833[
5]={& Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830,& Cyc__gentuple_831,&
Cyc__gentuple_832};static char _tmp380[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,{_tmp380,_tmp380,_tmp380 + 8},{(void*)((struct _tuple7**)
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
static char _tmp382[5]="Plus";static struct _tuple7 Cyc__gentuple_790={0,{_tmp382,
_tmp382,_tmp382 + 5}};static char _tmp383[6]="Times";static struct _tuple7 Cyc__gentuple_791={
1,{_tmp383,_tmp383,_tmp383 + 6}};static char _tmp384[6]="Minus";static struct _tuple7
Cyc__gentuple_792={2,{_tmp384,_tmp384,_tmp384 + 6}};static char _tmp385[4]="Div";
static struct _tuple7 Cyc__gentuple_793={3,{_tmp385,_tmp385,_tmp385 + 4}};static char
_tmp386[4]="Mod";static struct _tuple7 Cyc__gentuple_794={4,{_tmp386,_tmp386,
_tmp386 + 4}};static char _tmp387[3]="Eq";static struct _tuple7 Cyc__gentuple_795={5,{
_tmp387,_tmp387,_tmp387 + 3}};static char _tmp388[4]="Neq";static struct _tuple7 Cyc__gentuple_796={
6,{_tmp388,_tmp388,_tmp388 + 4}};static char _tmp389[3]="Gt";static struct _tuple7 Cyc__gentuple_797={
7,{_tmp389,_tmp389,_tmp389 + 3}};static char _tmp38A[3]="Lt";static struct _tuple7 Cyc__gentuple_798={
8,{_tmp38A,_tmp38A,_tmp38A + 3}};static char _tmp38B[4]="Gte";static struct _tuple7
Cyc__gentuple_799={9,{_tmp38B,_tmp38B,_tmp38B + 4}};static char _tmp38C[4]="Lte";
static struct _tuple7 Cyc__gentuple_800={10,{_tmp38C,_tmp38C,_tmp38C + 4}};static
char _tmp38D[4]="Not";static struct _tuple7 Cyc__gentuple_801={11,{_tmp38D,_tmp38D,
_tmp38D + 4}};static char _tmp38E[7]="Bitnot";static struct _tuple7 Cyc__gentuple_802={
12,{_tmp38E,_tmp38E,_tmp38E + 7}};static char _tmp38F[7]="Bitand";static struct
_tuple7 Cyc__gentuple_803={13,{_tmp38F,_tmp38F,_tmp38F + 7}};static char _tmp390[6]="Bitor";
static struct _tuple7 Cyc__gentuple_804={14,{_tmp390,_tmp390,_tmp390 + 6}};static
char _tmp391[7]="Bitxor";static struct _tuple7 Cyc__gentuple_805={15,{_tmp391,
_tmp391,_tmp391 + 7}};static char _tmp392[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_806={
16,{_tmp392,_tmp392,_tmp392 + 10}};static char _tmp393[11]="Bitlrshift";static
struct _tuple7 Cyc__gentuple_807={17,{_tmp393,_tmp393,_tmp393 + 11}};static char
_tmp394[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_808={18,{_tmp394,
_tmp394,_tmp394 + 11}};static char _tmp395[5]="Size";static struct _tuple7 Cyc__gentuple_809={
19,{_tmp395,_tmp395,_tmp395 + 5}};static struct _tuple7*Cyc__genarr_810[20]={& Cyc__gentuple_790,&
Cyc__gentuple_791,& Cyc__gentuple_792,& Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,&
Cyc__gentuple_796,& Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,&
Cyc__gentuple_801,& Cyc__gentuple_802,& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,&
Cyc__gentuple_806,& Cyc__gentuple_807,& Cyc__gentuple_808,& Cyc__gentuple_809};
static struct _tuple5*Cyc__genarr_811[0]={};static char _tmp397[7]="Primop";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,{_tmp397,_tmp397,_tmp397 + 7},{(
void*)((struct _tuple7**)Cyc__genarr_810),(void*)((struct _tuple7**)Cyc__genarr_810),(
void*)((struct _tuple7**)Cyc__genarr_810 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_811),(
void*)((struct _tuple5**)Cyc__genarr_811),(void*)((struct _tuple5**)Cyc__genarr_811
+ 0)}};struct _tuple63{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_821={offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_822={offsetof(struct _tuple63,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_823={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_484};
static struct _tuple6*Cyc__genarr_824[3]={& Cyc__gentuple_821,& Cyc__gentuple_822,&
Cyc__gentuple_823};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_820={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_824),(void*)((
struct _tuple6**)Cyc__genarr_824),(void*)((struct _tuple6**)Cyc__genarr_824 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_788;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_789;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp399[4]="Opt";static struct _tagged_arr Cyc__genname_814={_tmp399,
_tmp399,_tmp399 + 4};static char _tmp39A[2]="v";static struct _tuple5 Cyc__gentuple_812={
offsetof(struct Cyc_Core_Opt,v),{_tmp39A,_tmp39A,_tmp39A + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_813[1]={& Cyc__gentuple_812};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_814,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_813),(void*)((
struct _tuple5**)Cyc__genarr_813),(void*)((struct _tuple5**)Cyc__genarr_813 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_789={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple64{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_815={offsetof(struct
_tuple64,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_816={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_817={
offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_789};static struct _tuple6 Cyc__gentuple_818={
offsetof(struct _tuple64,f4),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_819[
4]={& Cyc__gentuple_815,& Cyc__gentuple_816,& Cyc__gentuple_817,& Cyc__gentuple_818};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_788={4,sizeof(struct _tuple64),{(
void*)((struct _tuple6**)Cyc__genarr_819),(void*)((struct _tuple6**)Cyc__genarr_819),(
void*)((struct _tuple6**)Cyc__genarr_819 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_777;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp39E[7]="PreInc";static struct _tuple7 Cyc__gentuple_778={0,{_tmp39E,
_tmp39E,_tmp39E + 7}};static char _tmp39F[8]="PostInc";static struct _tuple7 Cyc__gentuple_779={
1,{_tmp39F,_tmp39F,_tmp39F + 8}};static char _tmp3A0[7]="PreDec";static struct
_tuple7 Cyc__gentuple_780={2,{_tmp3A0,_tmp3A0,_tmp3A0 + 7}};static char _tmp3A1[8]="PostDec";
static struct _tuple7 Cyc__gentuple_781={3,{_tmp3A1,_tmp3A1,_tmp3A1 + 8}};static
struct _tuple7*Cyc__genarr_782[4]={& Cyc__gentuple_778,& Cyc__gentuple_779,& Cyc__gentuple_780,&
Cyc__gentuple_781};static struct _tuple5*Cyc__genarr_783[0]={};static char _tmp3A3[
12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={5,{
_tmp3A3,_tmp3A3,_tmp3A3 + 12},{(void*)((struct _tuple7**)Cyc__genarr_782),(void*)((
struct _tuple7**)Cyc__genarr_782),(void*)((struct _tuple7**)Cyc__genarr_782 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_783),(void*)((struct _tuple5**)Cyc__genarr_783),(
void*)((struct _tuple5**)Cyc__genarr_783 + 0)}};struct _tuple65{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_784={offsetof(
struct _tuple65,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_785={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_786={
offsetof(struct _tuple65,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_787[3]={& Cyc__gentuple_784,& Cyc__gentuple_785,& Cyc__gentuple_786};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_777={4,sizeof(struct _tuple65),{(
void*)((struct _tuple6**)Cyc__genarr_787),(void*)((struct _tuple6**)Cyc__genarr_787),(
void*)((struct _tuple6**)Cyc__genarr_787 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_771;struct _tuple66{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_772={offsetof(
struct _tuple66,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_773={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_774={
offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_775={
offsetof(struct _tuple66,f4),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_776[
4]={& Cyc__gentuple_772,& Cyc__gentuple_773,& Cyc__gentuple_774,& Cyc__gentuple_775};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_771={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_776),(void*)((struct _tuple6**)Cyc__genarr_776),(
void*)((struct _tuple6**)Cyc__genarr_776 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_710;struct _tuple67{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple6 Cyc__gentuple_711={offsetof(struct _tuple67,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_712={offsetof(struct _tuple67,f2),(
void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_713={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_714[3]={&
Cyc__gentuple_711,& Cyc__gentuple_712,& Cyc__gentuple_713};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_710={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_714),(
void*)((struct _tuple6**)Cyc__genarr_714),(void*)((struct _tuple6**)Cyc__genarr_714
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_766;static struct _tuple6
Cyc__gentuple_767={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_768={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_769={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_484};
static struct _tuple6*Cyc__genarr_770[3]={& Cyc__gentuple_767,& Cyc__gentuple_768,&
Cyc__gentuple_769};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_766={4,
sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_770),(void*)((
struct _tuple6**)Cyc__genarr_770),(void*)((struct _tuple6**)Cyc__genarr_770 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_753;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_754;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_755;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_755={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp3A9[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_760={_tmp3A9,
_tmp3A9,_tmp3A9 + 15};static char _tmp3AA[12]="num_varargs";static struct _tuple5 Cyc__gentuple_756={
offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),{_tmp3AA,_tmp3AA,_tmp3AA + 
12},(void*)((void*)& Cyc__genrep_62)};static char _tmp3AB[10]="injectors";static
struct _tuple5 Cyc__gentuple_757={offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),{
_tmp3AB,_tmp3AB,_tmp3AB + 10},(void*)& Cyc__genrep_289};static char _tmp3AC[4]="vai";
static struct _tuple5 Cyc__gentuple_758={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),{
_tmp3AC,_tmp3AC,_tmp3AC + 4},(void*)& Cyc__genrep_755};static struct _tuple5*Cyc__genarr_759[
3]={& Cyc__gentuple_756,& Cyc__gentuple_757,& Cyc__gentuple_758};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_760,
sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((struct _tuple5**)Cyc__genarr_759),(
void*)((struct _tuple5**)Cyc__genarr_759),(void*)((struct _tuple5**)Cyc__genarr_759
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_754={1,1,(void*)((void*)&
Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*f4;};static struct
_tuple6 Cyc__gentuple_761={offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_762={offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_82};
static struct _tuple6 Cyc__gentuple_763={offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_484};
static struct _tuple6 Cyc__gentuple_764={offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_754};
static struct _tuple6*Cyc__genarr_765[4]={& Cyc__gentuple_761,& Cyc__gentuple_762,&
Cyc__gentuple_763,& Cyc__gentuple_764};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_753={
4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_765),(void*)((
struct _tuple6**)Cyc__genarr_765),(void*)((struct _tuple6**)Cyc__genarr_765 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_748;static struct _tuple6 Cyc__gentuple_749={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_750={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_751={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_752[
3]={& Cyc__gentuple_749,& Cyc__gentuple_750,& Cyc__gentuple_751};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_748={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_752),(
void*)((struct _tuple6**)Cyc__genarr_752),(void*)((struct _tuple6**)Cyc__genarr_752
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_743;struct _tuple69{
unsigned int f1;void*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_744={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_745={
offsetof(struct _tuple69,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_746={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_82};
static struct _tuple6*Cyc__genarr_747[3]={& Cyc__gentuple_744,& Cyc__gentuple_745,&
Cyc__gentuple_746};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_743={4,
sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_747),(void*)((
struct _tuple6**)Cyc__genarr_747),(void*)((struct _tuple6**)Cyc__genarr_747 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_738;static struct _tuple6 Cyc__gentuple_739={
offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_740={
offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_78};static struct _tuple6 Cyc__gentuple_741={
offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_742[
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
+ 2)}};static struct _tuple7*Cyc__genarr_726[0]={};static char _tmp3B5[12]="StructField";
static struct _tuple5 Cyc__gentuple_731={0,{_tmp3B5,_tmp3B5,_tmp3B5 + 12},(void*)&
Cyc__genrep_320};static char _tmp3B6[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_732={
1,{_tmp3B6,_tmp3B6,_tmp3B6 + 11},(void*)& Cyc__genrep_727};static struct _tuple5*Cyc__genarr_733[
2]={& Cyc__gentuple_731,& Cyc__gentuple_732};static char _tmp3B8[14]="OffsetofField";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={5,{_tmp3B8,
_tmp3B8,_tmp3B8 + 14},{(void*)((struct _tuple7**)Cyc__genarr_726),(void*)((struct
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
offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_297};static struct _tuple6 Cyc__gentuple_723={
offsetof(struct _tuple71,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_724[3]={& Cyc__gentuple_721,& Cyc__gentuple_722,& Cyc__gentuple_723};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_720={4,sizeof(struct _tuple71),{(
void*)((struct _tuple6**)Cyc__genarr_724),(void*)((struct _tuple6**)Cyc__genarr_724),(
void*)((struct _tuple6**)Cyc__genarr_724 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_715;struct _tuple72{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tagged_arr*f3;};static struct _tuple6 Cyc__gentuple_716={offsetof(struct _tuple72,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_717={offsetof(struct
_tuple72,f2),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_718={
offsetof(struct _tuple72,f3),(void*)& Cyc__genrep_12};static struct _tuple6*Cyc__genarr_719[
3]={& Cyc__gentuple_716,& Cyc__gentuple_717,& Cyc__gentuple_718};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_715={4,sizeof(struct _tuple72),{(void*)((struct _tuple6**)Cyc__genarr_719),(
void*)((struct _tuple6**)Cyc__genarr_719),(void*)((struct _tuple6**)Cyc__genarr_719
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_706;static struct _tuple6
Cyc__gentuple_707={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_708={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_484};
static struct _tuple6*Cyc__genarr_709[2]={& Cyc__gentuple_707,& Cyc__gentuple_708};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_706={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_709),(void*)((struct _tuple6**)Cyc__genarr_709),(
void*)((struct _tuple6**)Cyc__genarr_709 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_695;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_696;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_697;static struct _tuple6 Cyc__gentuple_698={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_581};static struct _tuple6 Cyc__gentuple_699={
offsetof(struct _tuple1,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_700={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_701[3]={& Cyc__gentuple_698,& Cyc__gentuple_699,& Cyc__gentuple_700};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_697={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_701),(void*)((struct _tuple6**)Cyc__genarr_701),(
void*)((struct _tuple6**)Cyc__genarr_701 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_696={1,1,(void*)((void*)& Cyc__genrep_697)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_622;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_623;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_624;static struct _tuple6 Cyc__gentuple_625={offsetof(struct _tuple12,f1),(
void*)& Cyc__genrep_318};static struct _tuple6 Cyc__gentuple_626={offsetof(struct
_tuple12,f2),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_627[2]={&
Cyc__gentuple_625,& Cyc__gentuple_626};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_624={
4,sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_627),(void*)((
struct _tuple6**)Cyc__genarr_627),(void*)((struct _tuple6**)Cyc__genarr_627 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_623={1,1,(void*)((void*)& Cyc__genrep_624)};
static char _tmp3C1[5]="List";static struct _tagged_arr Cyc__genname_631={_tmp3C1,
_tmp3C1,_tmp3C1 + 5};static char _tmp3C2[3]="hd";static struct _tuple5 Cyc__gentuple_628={
offsetof(struct Cyc_List_List,hd),{_tmp3C2,_tmp3C2,_tmp3C2 + 3},(void*)& Cyc__genrep_623};
static char _tmp3C3[3]="tl";static struct _tuple5 Cyc__gentuple_629={offsetof(struct
Cyc_List_List,tl),{_tmp3C3,_tmp3C3,_tmp3C3 + 3},(void*)& Cyc__genrep_622};static
struct _tuple5*Cyc__genarr_630[2]={& Cyc__gentuple_628,& Cyc__gentuple_629};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_631,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_630),(void*)((struct _tuple5**)Cyc__genarr_630),(void*)((
struct _tuple5**)Cyc__genarr_630 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_622={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple73{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_702={offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_703={offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_696};
static struct _tuple6 Cyc__gentuple_704={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_622};
static struct _tuple6*Cyc__genarr_705[3]={& Cyc__gentuple_702,& Cyc__gentuple_703,&
Cyc__gentuple_704};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_695={4,
sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_705),(void*)((
struct _tuple6**)Cyc__genarr_705),(void*)((struct _tuple6**)Cyc__genarr_705 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_691;static struct _tuple6 Cyc__gentuple_692={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_693={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_622};static struct _tuple6*Cyc__genarr_694[
2]={& Cyc__gentuple_692,& Cyc__gentuple_693};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_691={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_694),(
void*)((struct _tuple6**)Cyc__genarr_694),(void*)((struct _tuple6**)Cyc__genarr_694
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_684;struct _tuple74{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_685={offsetof(struct _tuple74,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_686={offsetof(struct
_tuple74,f2),(void*)& Cyc__genrep_133};static struct _tuple6 Cyc__gentuple_687={
offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_688={
offsetof(struct _tuple74,f4),(void*)& Cyc__genrep_82};static struct _tuple6 Cyc__gentuple_689={
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
offsetof(struct _tuple75,f4),(void*)& Cyc__genrep_622};static struct _tuple6 Cyc__gentuple_682={
offsetof(struct _tuple75,f5),(void*)& Cyc__genrep_677};static struct _tuple6*Cyc__genarr_683[
5]={& Cyc__gentuple_678,& Cyc__gentuple_679,& Cyc__gentuple_680,& Cyc__gentuple_681,&
Cyc__gentuple_682};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_676={4,
sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_683),(void*)((
struct _tuple6**)Cyc__genarr_683),(void*)((struct _tuple6**)Cyc__genarr_683 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_671;static struct _tuple6 Cyc__gentuple_672={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_673={
offsetof(struct _tuple63,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_674={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_622};
static struct _tuple6*Cyc__genarr_675[3]={& Cyc__gentuple_672,& Cyc__gentuple_673,&
Cyc__gentuple_674};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_671={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_675),(void*)((
struct _tuple6**)Cyc__genarr_675),(void*)((struct _tuple6**)Cyc__genarr_675 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_665;struct _tuple76{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_666={offsetof(struct _tuple76,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple76,f2),(
void*)& Cyc__genrep_484};static struct _tuple6 Cyc__gentuple_668={offsetof(struct
_tuple76,f3),(void*)((void*)& Cyc__genrep_287)};static struct _tuple6 Cyc__gentuple_669={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_270};static struct _tuple6*Cyc__genarr_670[
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
Cyc__genrep_640;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_67;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_67={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3D2[11]="MallocInfo";
static struct _tagged_arr Cyc__genname_647={_tmp3D2,_tmp3D2,_tmp3D2 + 11};static char
_tmp3D3[10]="is_calloc";static struct _tuple5 Cyc__gentuple_641={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),{
_tmp3D3,_tmp3D3,_tmp3D3 + 10},(void*)((void*)& Cyc__genrep_62)};static char _tmp3D4[
4]="rgn";static struct _tuple5 Cyc__gentuple_642={offsetof(struct Cyc_Absyn_MallocInfo,rgn),{
_tmp3D4,_tmp3D4,_tmp3D4 + 4},(void*)& Cyc__genrep_78};static char _tmp3D5[9]="elt_type";
static struct _tuple5 Cyc__gentuple_643={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),{
_tmp3D5,_tmp3D5,_tmp3D5 + 9},(void*)& Cyc__genrep_67};static char _tmp3D6[9]="num_elts";
static struct _tuple5 Cyc__gentuple_644={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),{
_tmp3D6,_tmp3D6,_tmp3D6 + 9},(void*)& Cyc__genrep_82};static char _tmp3D7[11]="fat_result";
static struct _tuple5 Cyc__gentuple_645={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),{
_tmp3D7,_tmp3D7,_tmp3D7 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_646[5]={& Cyc__gentuple_641,& Cyc__gentuple_642,& Cyc__gentuple_643,&
Cyc__gentuple_644,& Cyc__gentuple_645};struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_647,sizeof(struct Cyc_Absyn_MallocInfo),{(
void*)((struct _tuple5**)Cyc__genarr_646),(void*)((struct _tuple5**)Cyc__genarr_646),(
void*)((struct _tuple5**)Cyc__genarr_646 + 5)}};struct _tuple79{unsigned int f1;
struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6 Cyc__gentuple_648={offsetof(
struct _tuple79,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_649={
offsetof(struct _tuple79,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};static struct
_tuple6*Cyc__genarr_650[2]={& Cyc__gentuple_648,& Cyc__gentuple_649};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_640={4,sizeof(struct _tuple79),{(void*)((
struct _tuple6**)Cyc__genarr_650),(void*)((struct _tuple6**)Cyc__genarr_650),(void*)((
struct _tuple6**)Cyc__genarr_650 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_621;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_632;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;static char _tmp3DA[4]="Opt";static
struct _tagged_arr Cyc__genname_635={_tmp3DA,_tmp3DA,_tmp3DA + 4};static char _tmp3DB[
2]="v";static struct _tuple5 Cyc__gentuple_633={offsetof(struct Cyc_Core_Opt,v),{
_tmp3DB,_tmp3DB,_tmp3DB + 2},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_634[
1]={& Cyc__gentuple_633};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_635,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_634),(void*)((struct _tuple5**)Cyc__genarr_634),(void*)((
struct _tuple5**)Cyc__genarr_634 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_632={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};struct
_tuple80{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_636={offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple80,f2),(void*)& Cyc__genrep_632};
static struct _tuple6 Cyc__gentuple_638={offsetof(struct _tuple80,f3),(void*)& Cyc__genrep_622};
static struct _tuple6*Cyc__genarr_639[3]={& Cyc__gentuple_636,& Cyc__gentuple_637,&
Cyc__gentuple_638};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_621={4,
sizeof(struct _tuple80),{(void*)((struct _tuple6**)Cyc__genarr_639),(void*)((
struct _tuple6**)Cyc__genarr_639),(void*)((struct _tuple6**)Cyc__genarr_639 + 3)}};
static struct _tuple7*Cyc__genarr_80[0]={};static char _tmp3DF[8]="Const_e";static
struct _tuple5 Cyc__gentuple_868={0,{_tmp3DF,_tmp3DF,_tmp3DF + 8},(void*)& Cyc__genrep_838};
static char _tmp3E0[6]="Var_e";static struct _tuple5 Cyc__gentuple_869={1,{_tmp3E0,
_tmp3E0,_tmp3E0 + 6},(void*)& Cyc__genrep_825};static char _tmp3E1[12]="UnknownId_e";
static struct _tuple5 Cyc__gentuple_870={2,{_tmp3E1,_tmp3E1,_tmp3E1 + 12},(void*)&
Cyc__genrep_245};static char _tmp3E2[9]="Primop_e";static struct _tuple5 Cyc__gentuple_871={
3,{_tmp3E2,_tmp3E2,_tmp3E2 + 9},(void*)& Cyc__genrep_820};static char _tmp3E3[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_872={4,{_tmp3E3,_tmp3E3,_tmp3E3 + 11},(void*)&
Cyc__genrep_788};static char _tmp3E4[12]="Increment_e";static struct _tuple5 Cyc__gentuple_873={
5,{_tmp3E4,_tmp3E4,_tmp3E4 + 12},(void*)& Cyc__genrep_777};static char _tmp3E5[14]="Conditional_e";
static struct _tuple5 Cyc__gentuple_874={6,{_tmp3E5,_tmp3E5,_tmp3E5 + 14},(void*)&
Cyc__genrep_771};static char _tmp3E6[9]="SeqExp_e";static struct _tuple5 Cyc__gentuple_875={
7,{_tmp3E6,_tmp3E6,_tmp3E6 + 9},(void*)& Cyc__genrep_710};static char _tmp3E7[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_876={8,{_tmp3E7,_tmp3E7,_tmp3E7 + 14},(void*)&
Cyc__genrep_766};static char _tmp3E8[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_877={
9,{_tmp3E8,_tmp3E8,_tmp3E8 + 9},(void*)& Cyc__genrep_753};static char _tmp3E9[8]="Throw_e";
static struct _tuple5 Cyc__gentuple_878={10,{_tmp3E9,_tmp3E9,_tmp3E9 + 8},(void*)&
Cyc__genrep_81};static char _tmp3EA[16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_879={
11,{_tmp3EA,_tmp3EA,_tmp3EA + 16},(void*)& Cyc__genrep_81};static char _tmp3EB[14]="Instantiate_e";
static struct _tuple5 Cyc__gentuple_880={12,{_tmp3EB,_tmp3EB,_tmp3EB + 14},(void*)&
Cyc__genrep_748};static char _tmp3EC[7]="Cast_e";static struct _tuple5 Cyc__gentuple_881={
13,{_tmp3EC,_tmp3EC,_tmp3EC + 7},(void*)& Cyc__genrep_743};static char _tmp3ED[10]="Address_e";
static struct _tuple5 Cyc__gentuple_882={14,{_tmp3ED,_tmp3ED,_tmp3ED + 10},(void*)&
Cyc__genrep_81};static char _tmp3EE[6]="New_e";static struct _tuple5 Cyc__gentuple_883={
15,{_tmp3EE,_tmp3EE,_tmp3EE + 6},(void*)& Cyc__genrep_738};static char _tmp3EF[12]="Sizeoftyp_e";
static struct _tuple5 Cyc__gentuple_884={16,{_tmp3EF,_tmp3EF,_tmp3EF + 12},(void*)&
Cyc__genrep_48};static char _tmp3F0[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_885={
17,{_tmp3F0,_tmp3F0,_tmp3F0 + 12},(void*)& Cyc__genrep_81};static char _tmp3F1[11]="Offsetof_e";
static struct _tuple5 Cyc__gentuple_886={18,{_tmp3F1,_tmp3F1,_tmp3F1 + 11},(void*)&
Cyc__genrep_725};static char _tmp3F2[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_887={
19,{_tmp3F2,_tmp3F2,_tmp3F2 + 9},(void*)& Cyc__genrep_720};static char _tmp3F3[8]="Deref_e";
static struct _tuple5 Cyc__gentuple_888={20,{_tmp3F3,_tmp3F3,_tmp3F3 + 8},(void*)&
Cyc__genrep_81};static char _tmp3F4[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_889={
21,{_tmp3F4,_tmp3F4,_tmp3F4 + 13},(void*)& Cyc__genrep_715};static char _tmp3F5[12]="AggrArrow_e";
static struct _tuple5 Cyc__gentuple_890={22,{_tmp3F5,_tmp3F5,_tmp3F5 + 12},(void*)&
Cyc__genrep_715};static char _tmp3F6[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_891={
23,{_tmp3F6,_tmp3F6,_tmp3F6 + 12},(void*)& Cyc__genrep_710};static char _tmp3F7[8]="Tuple_e";
static struct _tuple5 Cyc__gentuple_892={24,{_tmp3F7,_tmp3F7,_tmp3F7 + 8},(void*)&
Cyc__genrep_706};static char _tmp3F8[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_893={
25,{_tmp3F8,_tmp3F8,_tmp3F8 + 14},(void*)& Cyc__genrep_695};static char _tmp3F9[8]="Array_e";
static struct _tuple5 Cyc__gentuple_894={26,{_tmp3F9,_tmp3F9,_tmp3F9 + 8},(void*)&
Cyc__genrep_691};static char _tmp3FA[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_895={
27,{_tmp3FA,_tmp3FA,_tmp3FA + 16},(void*)& Cyc__genrep_684};static char _tmp3FB[9]="Struct_e";
static struct _tuple5 Cyc__gentuple_896={28,{_tmp3FB,_tmp3FB,_tmp3FB + 9},(void*)&
Cyc__genrep_676};static char _tmp3FC[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_897={
29,{_tmp3FC,_tmp3FC,_tmp3FC + 13},(void*)& Cyc__genrep_671};static char _tmp3FD[9]="Tunion_e";
static struct _tuple5 Cyc__gentuple_898={30,{_tmp3FD,_tmp3FD,_tmp3FD + 9},(void*)&
Cyc__genrep_665};static char _tmp3FE[7]="Enum_e";static struct _tuple5 Cyc__gentuple_899={
31,{_tmp3FE,_tmp3FE,_tmp3FE + 7},(void*)& Cyc__genrep_658};static char _tmp3FF[11]="AnonEnum_e";
static struct _tuple5 Cyc__gentuple_900={32,{_tmp3FF,_tmp3FF,_tmp3FF + 11},(void*)&
Cyc__genrep_651};static char _tmp400[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_901={
33,{_tmp400,_tmp400,_tmp400 + 9},(void*)& Cyc__genrep_640};static char _tmp401[16]="UnresolvedMem_e";
static struct _tuple5 Cyc__gentuple_902={34,{_tmp401,_tmp401,_tmp401 + 16},(void*)&
Cyc__genrep_621};static char _tmp402[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_903={
35,{_tmp402,_tmp402,_tmp402 + 10},(void*)& Cyc__genrep_473};static char _tmp403[10]="Codegen_e";
static struct _tuple5 Cyc__gentuple_904={36,{_tmp403,_tmp403,_tmp403 + 10},(void*)&
Cyc__genrep_86};static char _tmp404[7]="Fill_e";static struct _tuple5 Cyc__gentuple_905={
37,{_tmp404,_tmp404,_tmp404 + 7},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_906[
38]={& Cyc__gentuple_868,& Cyc__gentuple_869,& Cyc__gentuple_870,& Cyc__gentuple_871,&
Cyc__gentuple_872,& Cyc__gentuple_873,& Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876,&
Cyc__gentuple_877,& Cyc__gentuple_878,& Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881,&
Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,&
Cyc__gentuple_887,& Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,&
Cyc__gentuple_892,& Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,&
Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,&
Cyc__gentuple_902,& Cyc__gentuple_903,& Cyc__gentuple_904,& Cyc__gentuple_905};
static char _tmp406[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,{_tmp406,_tmp406,_tmp406 + 8},{(void*)((struct _tuple7**)Cyc__genarr_80),(void*)((
struct _tuple7**)Cyc__genarr_80),(void*)((struct _tuple7**)Cyc__genarr_80 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_906),(void*)((struct _tuple5**)Cyc__genarr_906),(
void*)((struct _tuple5**)Cyc__genarr_906 + 38)}};static char _tmp407[4]="Exp";static
struct _tagged_arr Cyc__genname_912={_tmp407,_tmp407,_tmp407 + 4};static char _tmp408[
5]="topt";static struct _tuple5 Cyc__gentuple_907={offsetof(struct Cyc_Absyn_Exp,topt),{
_tmp408,_tmp408,_tmp408 + 5},(void*)& Cyc__genrep_43};static char _tmp409[2]="r";
static struct _tuple5 Cyc__gentuple_908={offsetof(struct Cyc_Absyn_Exp,r),{_tmp409,
_tmp409,_tmp409 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp40A[4]="loc";
static struct _tuple5 Cyc__gentuple_909={offsetof(struct Cyc_Absyn_Exp,loc),{_tmp40A,
_tmp40A,_tmp40A + 4},(void*)& Cyc__genrep_2};static char _tmp40B[6]="annot";static
struct _tuple5 Cyc__gentuple_910={offsetof(struct Cyc_Absyn_Exp,annot),{_tmp40B,
_tmp40B,_tmp40B + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_911[
4]={& Cyc__gentuple_907,& Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _tagged_arr*)&
Cyc__genname_912,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_911),(
void*)((struct _tuple5**)Cyc__genarr_911),(void*)((struct _tuple5**)Cyc__genarr_911
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_78={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static char _tmp40E[10]="ArrayInfo";static struct
_tagged_arr Cyc__genname_981={_tmp40E,_tmp40E,_tmp40E + 10};static char _tmp40F[9]="elt_type";
static struct _tuple5 Cyc__gentuple_976={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),{
_tmp40F,_tmp40F,_tmp40F + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char
_tmp410[3]="tq";static struct _tuple5 Cyc__gentuple_977={offsetof(struct Cyc_Absyn_ArrayInfo,tq),{
_tmp410,_tmp410,_tmp410 + 3},(void*)& Cyc__genrep_134};static char _tmp411[9]="num_elts";
static struct _tuple5 Cyc__gentuple_978={offsetof(struct Cyc_Absyn_ArrayInfo,num_elts),{
_tmp411,_tmp411,_tmp411 + 9},(void*)& Cyc__genrep_78};static char _tmp412[10]="zero_term";
static struct _tuple5 Cyc__gentuple_979={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),{
_tmp412,_tmp412,_tmp412 + 10},(void*)& Cyc__genrep_963};static struct _tuple5*Cyc__genarr_980[
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
static char _tmp415[5]="List";static struct _tagged_arr Cyc__genname_948={_tmp415,
_tmp415,_tmp415 + 5};static char _tmp416[3]="hd";static struct _tuple5 Cyc__gentuple_945={
offsetof(struct Cyc_List_List,hd),{_tmp416,_tmp416,_tmp416 + 3},(void*)& Cyc__genrep_696};
static char _tmp417[3]="tl";static struct _tuple5 Cyc__gentuple_946={offsetof(struct
Cyc_List_List,tl),{_tmp417,_tmp417,_tmp417 + 3},(void*)& Cyc__genrep_944};static
struct _tuple5*Cyc__genarr_947[2]={& Cyc__gentuple_945,& Cyc__gentuple_946};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_948,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_947),(void*)((struct _tuple5**)Cyc__genarr_947),(void*)((
struct _tuple5**)Cyc__genarr_947 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_944={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp41A[7]="FnInfo";static struct _tagged_arr Cyc__genname_958={_tmp41A,
_tmp41A,_tmp41A + 7};static char _tmp41B[6]="tvars";static struct _tuple5 Cyc__gentuple_949={
offsetof(struct Cyc_Absyn_FnInfo,tvars),{_tmp41B,_tmp41B,_tmp41B + 6},(void*)& Cyc__genrep_297};
static char _tmp41C[7]="effect";static struct _tuple5 Cyc__gentuple_950={offsetof(
struct Cyc_Absyn_FnInfo,effect),{_tmp41C,_tmp41C,_tmp41C + 7},(void*)& Cyc__genrep_43};
static char _tmp41D[8]="ret_typ";static struct _tuple5 Cyc__gentuple_951={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),{_tmp41D,_tmp41D,_tmp41D + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp41E[5]="args";static struct _tuple5 Cyc__gentuple_952={offsetof(
struct Cyc_Absyn_FnInfo,args),{_tmp41E,_tmp41E,_tmp41E + 5},(void*)& Cyc__genrep_944};
static char _tmp41F[10]="c_varargs";static struct _tuple5 Cyc__gentuple_953={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),{_tmp41F,_tmp41F,_tmp41F + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp420[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_954={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),{_tmp420,_tmp420,
_tmp420 + 12},(void*)& Cyc__genrep_580};static char _tmp421[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_955={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),{_tmp421,
_tmp421,_tmp421 + 7},(void*)& Cyc__genrep_356};static char _tmp422[11]="attributes";
static struct _tuple5 Cyc__gentuple_956={offsetof(struct Cyc_Absyn_FnInfo,attributes),{
_tmp422,_tmp422,_tmp422 + 11},(void*)& Cyc__genrep_88};static struct _tuple5*Cyc__genarr_957[
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
_tuple13,f2),(void*)& Cyc__genrep_271};static struct _tuple6*Cyc__genarr_942[2]={&
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
static struct _tuple6 Cyc__gentuple_933={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_343};
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
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_75;static struct _tuple6 Cyc__gentuple_922={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_923={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_76};static struct _tuple6*Cyc__genarr_924[
2]={& Cyc__gentuple_922,& Cyc__gentuple_923};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_75={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_924),(
void*)((struct _tuple6**)Cyc__genarr_924),(void*)((struct _tuple6**)Cyc__genarr_924
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_66;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_68;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static char _tmp42A[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1117={
_tmp42A,_tmp42A,_tmp42A + 12};static char _tmp42B[5]="name";static struct _tuple5 Cyc__gentuple_1112={
offsetof(struct Cyc_Absyn_Typedefdecl,name),{_tmp42B,_tmp42B,_tmp42B + 5},(void*)&
Cyc__genrep_10};static char _tmp42C[4]="tvs";static struct _tuple5 Cyc__gentuple_1113={
offsetof(struct Cyc_Absyn_Typedefdecl,tvs),{_tmp42C,_tmp42C,_tmp42C + 4},(void*)&
Cyc__genrep_297};static char _tmp42D[5]="kind";static struct _tuple5 Cyc__gentuple_1114={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),{_tmp42D,_tmp42D,_tmp42D + 5},(void*)&
Cyc__genrep_1076};static char _tmp42E[5]="defn";static struct _tuple5 Cyc__gentuple_1115={
offsetof(struct Cyc_Absyn_Typedefdecl,defn),{_tmp42E,_tmp42E,_tmp42E + 5},(void*)&
Cyc__genrep_43};static struct _tuple5*Cyc__genarr_1116[4]={& Cyc__gentuple_1112,&
Cyc__gentuple_1113,& Cyc__gentuple_1114,& Cyc__gentuple_1115};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_1117,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1116),(
void*)((struct _tuple5**)Cyc__genarr_1116),(void*)((struct _tuple5**)Cyc__genarr_1116
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_68={1,1,(void*)((void*)&
Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple85{unsigned int f1;struct _tuple0*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*f4;void**f5;};static struct
_tuple6 Cyc__gentuple_69={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_70={offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_71={offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_53};
static struct _tuple6 Cyc__gentuple_72={offsetof(struct _tuple85,f4),(void*)& Cyc__genrep_68};
static struct _tuple6 Cyc__gentuple_73={offsetof(struct _tuple85,f5),(void*)& Cyc__genrep_67};
static struct _tuple6*Cyc__genarr_74[5]={& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_66={
4,sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_74),(void*)((
struct _tuple6**)Cyc__genarr_74),(void*)((struct _tuple6**)Cyc__genarr_74 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_52;static struct _tuple6 Cyc__gentuple_58={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_59={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_60[
2]={& Cyc__gentuple_58,& Cyc__gentuple_59};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_52={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_60),(void*)((
struct _tuple6**)Cyc__genarr_60),(void*)((struct _tuple6**)Cyc__genarr_60 + 2)}};
static char _tmp433[9]="VoidType";static struct _tuple7 Cyc__gentuple_44={0,{_tmp433,
_tmp433,_tmp433 + 9}};static char _tmp434[10]="FloatType";static struct _tuple7 Cyc__gentuple_45={
1,{_tmp434,_tmp434,_tmp434 + 10}};static char _tmp435[8]="HeapRgn";static struct
_tuple7 Cyc__gentuple_46={2,{_tmp435,_tmp435,_tmp435 + 8}};static struct _tuple7*Cyc__genarr_47[
3]={& Cyc__gentuple_44,& Cyc__gentuple_45,& Cyc__gentuple_46};static char _tmp436[5]="Evar";
static struct _tuple5 Cyc__gentuple_1086={0,{_tmp436,_tmp436,_tmp436 + 5},(void*)&
Cyc__genrep_1071};static char _tmp437[8]="VarType";static struct _tuple5 Cyc__gentuple_1087={
1,{_tmp437,_tmp437,_tmp437 + 8},(void*)& Cyc__genrep_1067};static char _tmp438[11]="TunionType";
static struct _tuple5 Cyc__gentuple_1088={2,{_tmp438,_tmp438,_tmp438 + 11},(void*)&
Cyc__genrep_1041};static char _tmp439[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1089={
3,{_tmp439,_tmp439,_tmp439 + 16},(void*)& Cyc__genrep_1015};static char _tmp43A[12]="PointerType";
static struct _tuple5 Cyc__gentuple_1090={4,{_tmp43A,_tmp43A,_tmp43A + 12},(void*)&
Cyc__genrep_996};static char _tmp43B[8]="IntType";static struct _tuple5 Cyc__gentuple_1091={
5,{_tmp43B,_tmp43B,_tmp43B + 8},(void*)& Cyc__genrep_985};static char _tmp43C[11]="DoubleType";
static struct _tuple5 Cyc__gentuple_1092={6,{_tmp43C,_tmp43C,_tmp43C + 11},(void*)&
Cyc__genrep_61};static char _tmp43D[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1093={
7,{_tmp43D,_tmp43D,_tmp43D + 10},(void*)& Cyc__genrep_962};static char _tmp43E[7]="FnType";
static struct _tuple5 Cyc__gentuple_1094={8,{_tmp43E,_tmp43E,_tmp43E + 7},(void*)&
Cyc__genrep_943};static char _tmp43F[10]="TupleType";static struct _tuple5 Cyc__gentuple_1095={
9,{_tmp43F,_tmp43F,_tmp43F + 10},(void*)& Cyc__genrep_939};static char _tmp440[9]="AggrType";
static struct _tuple5 Cyc__gentuple_1096={10,{_tmp440,_tmp440,_tmp440 + 9},(void*)&
Cyc__genrep_935};static char _tmp441[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1097={
11,{_tmp441,_tmp441,_tmp441 + 13},(void*)& Cyc__genrep_930};static char _tmp442[9]="EnumType";
static struct _tuple5 Cyc__gentuple_1098={12,{_tmp442,_tmp442,_tmp442 + 9},(void*)&
Cyc__genrep_925};static char _tmp443[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1099={
13,{_tmp443,_tmp443,_tmp443 + 13},(void*)& Cyc__genrep_75};static char _tmp444[11]="SizeofType";
static struct _tuple5 Cyc__gentuple_1100={14,{_tmp444,_tmp444,_tmp444 + 11},(void*)&
Cyc__genrep_48};static char _tmp445[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1101={
15,{_tmp445,_tmp445,_tmp445 + 14},(void*)& Cyc__genrep_48};static char _tmp446[12]="TypedefType";
static struct _tuple5 Cyc__gentuple_1102={16,{_tmp446,_tmp446,_tmp446 + 12},(void*)&
Cyc__genrep_66};static char _tmp447[8]="TagType";static struct _tuple5 Cyc__gentuple_1103={
17,{_tmp447,_tmp447,_tmp447 + 8},(void*)& Cyc__genrep_48};static char _tmp448[8]="TypeInt";
static struct _tuple5 Cyc__gentuple_1104={18,{_tmp448,_tmp448,_tmp448 + 8},(void*)&
Cyc__genrep_61};static char _tmp449[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1105={
19,{_tmp449,_tmp449,_tmp449 + 10},(void*)& Cyc__genrep_48};static char _tmp44A[8]="JoinEff";
static struct _tuple5 Cyc__gentuple_1106={20,{_tmp44A,_tmp44A,_tmp44A + 8},(void*)&
Cyc__genrep_52};static char _tmp44B[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1107={
21,{_tmp44B,_tmp44B,_tmp44B + 8},(void*)& Cyc__genrep_48};static struct _tuple5*Cyc__genarr_1108[
22]={& Cyc__gentuple_1086,& Cyc__gentuple_1087,& Cyc__gentuple_1088,& Cyc__gentuple_1089,&
Cyc__gentuple_1090,& Cyc__gentuple_1091,& Cyc__gentuple_1092,& Cyc__gentuple_1093,&
Cyc__gentuple_1094,& Cyc__gentuple_1095,& Cyc__gentuple_1096,& Cyc__gentuple_1097,&
Cyc__gentuple_1098,& Cyc__gentuple_1099,& Cyc__gentuple_1100,& Cyc__gentuple_1101,&
Cyc__gentuple_1102,& Cyc__gentuple_1103,& Cyc__gentuple_1104,& Cyc__gentuple_1105,&
Cyc__gentuple_1106,& Cyc__gentuple_1107};static char _tmp44D[5]="Type";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_type_t_rep={5,{_tmp44D,_tmp44D,_tmp44D + 5},{(void*)((struct _tuple7**)
Cyc__genarr_47),(void*)((struct _tuple7**)Cyc__genarr_47),(void*)((struct _tuple7**)
Cyc__genarr_47 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_1108),(void*)((struct
_tuple5**)Cyc__genarr_1108),(void*)((struct _tuple5**)Cyc__genarr_1108 + 22)}};
static char _tmp44E[8]="Vardecl";static struct _tagged_arr Cyc__genname_154={_tmp44E,
_tmp44E,_tmp44E + 8};static char _tmp44F[3]="sc";static struct _tuple5 Cyc__gentuple_145={
offsetof(struct Cyc_Absyn_Vardecl,sc),{_tmp44F,_tmp44F,_tmp44F + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp450[5]="name";static struct _tuple5 Cyc__gentuple_146={offsetof(
struct Cyc_Absyn_Vardecl,name),{_tmp450,_tmp450,_tmp450 + 5},(void*)& Cyc__genrep_10};
static char _tmp451[3]="tq";static struct _tuple5 Cyc__gentuple_147={offsetof(struct
Cyc_Absyn_Vardecl,tq),{_tmp451,_tmp451,_tmp451 + 3},(void*)& Cyc__genrep_134};
static char _tmp452[5]="type";static struct _tuple5 Cyc__gentuple_148={offsetof(
struct Cyc_Absyn_Vardecl,type),{_tmp452,_tmp452,_tmp452 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp453[12]="initializer";static struct _tuple5 Cyc__gentuple_149={
offsetof(struct Cyc_Absyn_Vardecl,initializer),{_tmp453,_tmp453,_tmp453 + 12},(
void*)& Cyc__genrep_78};static char _tmp454[4]="rgn";static struct _tuple5 Cyc__gentuple_150={
offsetof(struct Cyc_Absyn_Vardecl,rgn),{_tmp454,_tmp454,_tmp454 + 4},(void*)& Cyc__genrep_43};
static char _tmp455[11]="attributes";static struct _tuple5 Cyc__gentuple_151={
offsetof(struct Cyc_Absyn_Vardecl,attributes),{_tmp455,_tmp455,_tmp455 + 11},(void*)&
Cyc__genrep_88};static char _tmp456[8]="escapes";static struct _tuple5 Cyc__gentuple_152={
offsetof(struct Cyc_Absyn_Vardecl,escapes),{_tmp456,_tmp456,_tmp456 + 8},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_153[8]={& Cyc__gentuple_145,&
Cyc__gentuple_146,& Cyc__gentuple_147,& Cyc__gentuple_148,& Cyc__gentuple_149,& Cyc__gentuple_150,&
Cyc__gentuple_151,& Cyc__gentuple_152};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_154,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_153),(void*)((struct _tuple5**)Cyc__genarr_153),(void*)((
struct _tuple5**)Cyc__genarr_153 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_133={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple86{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_404={offsetof(
struct _tuple86,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_405={
offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_133};static struct _tuple6*Cyc__genarr_406[
2]={& Cyc__gentuple_404,& Cyc__gentuple_405};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_403={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_406),(
void*)((struct _tuple6**)Cyc__genarr_406),(void*)((struct _tuple6**)Cyc__genarr_406
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1137;struct _tuple87{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1138={offsetof(struct _tuple87,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1139={offsetof(struct _tuple87,f2),(
void*)& Cyc__genrep_231};static struct _tuple6 Cyc__gentuple_1140={offsetof(struct
_tuple87,f3),(void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_1141={
offsetof(struct _tuple87,f4),(void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_1142[
4]={& Cyc__gentuple_1138,& Cyc__gentuple_1139,& Cyc__gentuple_1140,& Cyc__gentuple_1141};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1137={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_1142),(void*)((struct _tuple6**)Cyc__genarr_1142),(
void*)((struct _tuple6**)Cyc__genarr_1142 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1133;static struct _tuple6 Cyc__gentuple_1134={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1135={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_132};static struct _tuple6*Cyc__genarr_1136[2]={&
Cyc__gentuple_1134,& Cyc__gentuple_1135};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1133={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_1136),(void*)((
struct _tuple6**)Cyc__genarr_1136),(void*)((struct _tuple6**)Cyc__genarr_1136 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1129;struct _tuple88{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1130={
offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1131={
offsetof(struct _tuple88,f2),(void*)((void*)& Cyc__genrep_341)};static struct
_tuple6*Cyc__genarr_1132[2]={& Cyc__gentuple_1130,& Cyc__gentuple_1131};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1129={4,sizeof(struct _tuple88),{(void*)((
struct _tuple6**)Cyc__genarr_1132),(void*)((struct _tuple6**)Cyc__genarr_1132),(
void*)((struct _tuple6**)Cyc__genarr_1132 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1125;struct _tuple89{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1126={offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1127={offsetof(struct _tuple89,f2),(void*)((void*)&
Cyc__genrep_287)};static struct _tuple6*Cyc__genarr_1128[2]={& Cyc__gentuple_1126,&
Cyc__gentuple_1127};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1125={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_1128),(void*)((
struct _tuple6**)Cyc__genarr_1128),(void*)((struct _tuple6**)Cyc__genarr_1128 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1121;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1122={
offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1123={
offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_255};static struct _tuple6*Cyc__genarr_1124[
2]={& Cyc__gentuple_1122,& Cyc__gentuple_1123};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1121={4,sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1124),(
void*)((struct _tuple6**)Cyc__genarr_1124),(void*)((struct _tuple6**)Cyc__genarr_1124
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_41;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_42={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple91{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1118={offsetof(
struct _tuple91,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1119={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_42};static struct _tuple6*Cyc__genarr_1120[
2]={& Cyc__gentuple_1118,& Cyc__gentuple_1119};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41={4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1120),(
void*)((struct _tuple6**)Cyc__genarr_1120),(void*)((struct _tuple6**)Cyc__genarr_1120
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
static struct _tuple7*Cyc__genarr_3[0]={};static char _tmp464[6]="Var_d";static
struct _tuple5 Cyc__gentuple_1143={0,{_tmp464,_tmp464,_tmp464 + 6},(void*)& Cyc__genrep_403};
static char _tmp465[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1144={1,{_tmp465,
_tmp465,_tmp465 + 5},(void*)& Cyc__genrep_86};static char _tmp466[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1145={2,{_tmp466,_tmp466,_tmp466 + 6},(void*)& Cyc__genrep_1137};
static char _tmp467[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1146={3,{_tmp467,
_tmp467,_tmp467 + 7},(void*)& Cyc__genrep_1133};static char _tmp468[7]="Aggr_d";
static struct _tuple5 Cyc__gentuple_1147={4,{_tmp468,_tmp468,_tmp468 + 7},(void*)&
Cyc__genrep_1129};static char _tmp469[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1148={
5,{_tmp469,_tmp469,_tmp469 + 9},(void*)& Cyc__genrep_1125};static char _tmp46A[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1149={6,{_tmp46A,_tmp46A,_tmp46A + 7},(void*)&
Cyc__genrep_1121};static char _tmp46B[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1150={
7,{_tmp46B,_tmp46B,_tmp46B + 10},(void*)& Cyc__genrep_41};static char _tmp46C[12]="Namespace_d";
static struct _tuple5 Cyc__gentuple_1151={8,{_tmp46C,_tmp46C,_tmp46C + 12},(void*)&
Cyc__genrep_36};static char _tmp46D[8]="Using_d";static struct _tuple5 Cyc__gentuple_1152={
9,{_tmp46D,_tmp46D,_tmp46D + 8},(void*)& Cyc__genrep_9};static char _tmp46E[10]="ExternC_d";
static struct _tuple5 Cyc__gentuple_1153={10,{_tmp46E,_tmp46E,_tmp46E + 10},(void*)&
Cyc__genrep_4};static struct _tuple5*Cyc__genarr_1154[11]={& Cyc__gentuple_1143,&
Cyc__gentuple_1144,& Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,&
Cyc__gentuple_1148,& Cyc__gentuple_1149,& Cyc__gentuple_1150,& Cyc__gentuple_1151,&
Cyc__gentuple_1152,& Cyc__gentuple_1153};static char _tmp470[9]="Raw_decl";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,{_tmp470,_tmp470,_tmp470 + 9},{(
void*)((struct _tuple7**)Cyc__genarr_3),(void*)((struct _tuple7**)Cyc__genarr_3),(
void*)((struct _tuple7**)Cyc__genarr_3 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1154),(
void*)((struct _tuple5**)Cyc__genarr_1154),(void*)((struct _tuple5**)Cyc__genarr_1154
+ 11)}};static char _tmp471[5]="Decl";static struct _tagged_arr Cyc__genname_1158={
_tmp471,_tmp471,_tmp471 + 5};static char _tmp472[2]="r";static struct _tuple5 Cyc__gentuple_1155={
offsetof(struct Cyc_Absyn_Decl,r),{_tmp472,_tmp472,_tmp472 + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static char _tmp473[4]="loc";static struct _tuple5 Cyc__gentuple_1156={offsetof(
struct Cyc_Absyn_Decl,loc),{_tmp473,_tmp473,_tmp473 + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_1157[2]={& Cyc__gentuple_1155,& Cyc__gentuple_1156};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1158,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1157),(
void*)((struct _tuple5**)Cyc__genarr_1157),(void*)((struct _tuple5**)Cyc__genarr_1157
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static char _tmp476[5]="List";static struct _tagged_arr
Cyc__genname_1162={_tmp476,_tmp476,_tmp476 + 5};static char _tmp477[3]="hd";static
struct _tuple5 Cyc__gentuple_1159={offsetof(struct Cyc_List_List,hd),{_tmp477,
_tmp477,_tmp477 + 3},(void*)& Cyc__genrep_1};static char _tmp478[3]="tl";static
struct _tuple5 Cyc__gentuple_1160={offsetof(struct Cyc_List_List,tl),{_tmp478,
_tmp478,_tmp478 + 3},(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_1161[
2]={& Cyc__gentuple_1159,& Cyc__gentuple_1160};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1162,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1161),(void*)((struct _tuple5**)Cyc__genarr_1161),(
void*)((struct _tuple5**)Cyc__genarr_1161 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
