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
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};int Cyc_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{void*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_PP_Ppstate;struct
Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(void*,void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
struct _tuple5{void*f1;int f2;};static struct _tuple5 Cyc_Evexp_eval_const_exp(struct
Cyc_Absyn_Exp*);static void*Cyc_Evexp_promote_const(void*cn){void*_tmp0=cn;void*
_tmp1;char _tmp2;void*_tmp3;short _tmp4;_LL1: if(_tmp0 <= (void*)1?1:*((int*)_tmp0)
!= 0)goto _LL3;_tmp1=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f1;_tmp2=((
struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;_LL2: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Absyn_Int_c_struct _tmp6;
_tmp6.tag=2;_tmp6.f1=(void*)_tmp1;_tmp6.f2=(int)_tmp2;_tmp6;});_tmp5;});_LL3: if(
_tmp0 <= (void*)1?1:*((int*)_tmp0)!= 1)goto _LL5;_tmp3=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp0)->f1;_tmp4=((struct Cyc_Absyn_Short_c_struct*)_tmp0)->f2;_LL4: return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct
Cyc_Absyn_Int_c_struct _tmp8;_tmp8.tag=2;_tmp8.f1=(void*)_tmp3;_tmp8.f2=(int)
_tmp4;_tmp8;});_tmp7;});_LL5:;_LL6: return cn;_LL0:;}struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e){void*_tmpA;int _tmpB;struct _tuple5 _tmp9=Cyc_Evexp_eval_const_exp(
e);_tmpA=_tmp9.f1;_tmpB=_tmp9.f2;if(!_tmpB)return({struct _tuple4 _tmpC;_tmpC.f1=0;
_tmpC.f2=0;_tmpC;});{void*_tmpD=Cyc_Evexp_promote_const(_tmpA);int _tmpE;_LL8: if(
_tmpD <= (void*)1?1:*((int*)_tmpD)!= 2)goto _LLA;_tmpE=((struct Cyc_Absyn_Int_c_struct*)
_tmpD)->f2;_LL9: return({struct _tuple4 _tmpF;_tmpF.f1=(unsigned int)_tmpE;_tmpF.f2=
1;_tmpF;});_LLA:;_LLB:({void*_tmp10[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp11="expecting unsigned int";_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(
_tmp11,23));}),_tag_arr(_tmp10,sizeof(void*),0));});return({struct _tuple4 _tmp12;
_tmp12.f1=0;_tmp12.f2=1;_tmp12;});_LL7:;}}struct _tuple6{int f1;int f2;};static
struct _tuple6 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){void*_tmp14;
int _tmp15;struct _tuple5 _tmp13=Cyc_Evexp_eval_const_exp(e);_tmp14=_tmp13.f1;
_tmp15=_tmp13.f2;if(!_tmp15)return({struct _tuple6 _tmp16;_tmp16.f1=0;_tmp16.f2=0;
_tmp16;});{void*_tmp17=Cyc_Evexp_promote_const(_tmp14);int _tmp18;long long _tmp19;
_LLD: if(_tmp17 <= (void*)1?1:*((int*)_tmp17)!= 2)goto _LLF;_tmp18=((struct Cyc_Absyn_Int_c_struct*)
_tmp17)->f2;_LLE: return({struct _tuple6 _tmp1A;_tmp1A.f1=_tmp18 != 0;_tmp1A.f2=1;
_tmp1A;});_LLF: if(_tmp17 <= (void*)1?1:*((int*)_tmp17)!= 3)goto _LL11;_tmp19=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp17)->f2;_LL10: return({struct _tuple6 _tmp1B;
_tmp1B.f1=_tmp19 != 0;_tmp1B.f2=1;_tmp1B;});_LL11: if((int)_tmp17 != 0)goto _LL13;
_LL12: return({struct _tuple6 _tmp1C;_tmp1C.f1=0;_tmp1C.f2=0;_tmp1C;});_LL13:;_LL14:({
void*_tmp1D[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp1E="expecting bool";
_tag_arr(_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,15));}),_tag_arr(_tmp1D,
sizeof(void*),0));});return({struct _tuple6 _tmp1F;_tmp1F.f1=0;_tmp1F.f2=0;_tmp1F;});
_LLC:;}}static struct _tuple5 Cyc_Evexp_eval_const_unprimop(void*p,struct Cyc_Absyn_Exp*
e){void*_tmp21;int _tmp22;struct _tuple5 _tmp20=Cyc_Evexp_eval_const_exp(e);_tmp21=
_tmp20.f1;_tmp22=_tmp20.f2;if(!_tmp22)return({struct _tuple5 _tmp23;_tmp23.f1=
_tmp21;_tmp23.f2=0;_tmp23;});{struct _tuple3 _tmp25=({struct _tuple3 _tmp24;_tmp24.f1=
p;_tmp24.f2=_tmp21;_tmp24;});void*_tmp26;void*_tmp27;void*_tmp28;void*_tmp29;int
_tmp2A;void*_tmp2B;void*_tmp2C;void*_tmp2D;int _tmp2E;void*_tmp2F;void*_tmp30;int
_tmp31;void*_tmp32;void*_tmp33;_LL16: _tmp26=_tmp25.f1;if((int)_tmp26 != 0)goto
_LL18;_LL17: goto _LL15;_LL18: _tmp27=_tmp25.f1;if((int)_tmp27 != 2)goto _LL1A;_tmp28=
_tmp25.f2;if(_tmp28 <= (void*)1?1:*((int*)_tmp28)!= 2)goto _LL1A;_tmp29=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp28)->f1;_tmp2A=((struct Cyc_Absyn_Int_c_struct*)
_tmp28)->f2;_LL19: _tmp21=(void*)({struct Cyc_Absyn_Int_c_struct*_tmp34=_cycalloc(
sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Int_c_struct _tmp35;_tmp35.tag=2;
_tmp35.f1=(void*)((void*)0);_tmp35.f2=- _tmp2A;_tmp35;});_tmp34;});goto _LL15;
_LL1A: _tmp2B=_tmp25.f1;if((int)_tmp2B != 12)goto _LL1C;_tmp2C=_tmp25.f2;if(_tmp2C
<= (void*)1?1:*((int*)_tmp2C)!= 2)goto _LL1C;_tmp2D=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp2C)->f1;_tmp2E=((struct Cyc_Absyn_Int_c_struct*)_tmp2C)->f2;_LL1B: _tmp21=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({
struct Cyc_Absyn_Int_c_struct _tmp37;_tmp37.tag=2;_tmp37.f1=(void*)((void*)1);
_tmp37.f2=~ _tmp2E;_tmp37;});_tmp36;});goto _LL15;_LL1C: _tmp2F=_tmp25.f1;if((int)
_tmp2F != 11)goto _LL1E;_tmp30=_tmp25.f2;if(_tmp30 <= (void*)1?1:*((int*)_tmp30)!= 
2)goto _LL1E;_tmp31=((struct Cyc_Absyn_Int_c_struct*)_tmp30)->f2;_LL1D: _tmp21=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({
struct Cyc_Absyn_Int_c_struct _tmp39;_tmp39.tag=2;_tmp39.f1=(void*)((void*)0);
_tmp39.f2=_tmp31 == 0?1: 0;_tmp39;});_tmp38;});goto _LL15;_LL1E: _tmp32=_tmp25.f1;
if((int)_tmp32 != 11)goto _LL20;_tmp33=_tmp25.f2;if((int)_tmp33 != 0)goto _LL20;
_LL1F: _tmp21=(void*)({struct Cyc_Absyn_Int_c_struct*_tmp3A=_cycalloc(sizeof(*
_tmp3A));_tmp3A[0]=({struct Cyc_Absyn_Int_c_struct _tmp3B;_tmp3B.tag=2;_tmp3B.f1=(
void*)((void*)0);_tmp3B.f2=1;_tmp3B;});_tmp3A;});goto _LL15;_LL20:;_LL21:({void*
_tmp3C[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp3D="bad constant expression";
_tag_arr(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,24));}),_tag_arr(_tmp3C,
sizeof(void*),0));});goto _LL15;_LL15:;}return({struct _tuple5 _tmp3E;_tmp3E.f1=
_tmp21;_tmp3E.f2=1;_tmp3E;});}static struct _tuple5 Cyc_Evexp_eval_const_binprimop(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp40;int _tmp41;
struct _tuple5 _tmp3F=Cyc_Evexp_eval_const_exp(e1);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;{
void*_tmp43;int _tmp44;struct _tuple5 _tmp42=Cyc_Evexp_eval_const_exp(e2);_tmp43=
_tmp42.f1;_tmp44=_tmp42.f2;if(!_tmp41?1: !_tmp44)return({struct _tuple5 _tmp45;
_tmp45.f1=_tmp40;_tmp45.f2=0;_tmp45;});_tmp40=Cyc_Evexp_promote_const(_tmp40);
_tmp43=Cyc_Evexp_promote_const(_tmp43);{void*s1;void*s2;int i1;int i2;{void*_tmp46=
_tmp40;void*_tmp47;int _tmp48;_LL23: if(_tmp46 <= (void*)1?1:*((int*)_tmp46)!= 2)
goto _LL25;_tmp47=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp46)->f1;_tmp48=((
struct Cyc_Absyn_Int_c_struct*)_tmp46)->f2;_LL24: s1=_tmp47;i1=_tmp48;goto _LL22;
_LL25:;_LL26:({void*_tmp49[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp4A="bad constant expression";
_tag_arr(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,24));}),_tag_arr(_tmp49,
sizeof(void*),0));});return({struct _tuple5 _tmp4B;_tmp4B.f1=_tmp40;_tmp4B.f2=1;
_tmp4B;});_LL22:;}{void*_tmp4C=_tmp43;void*_tmp4D;int _tmp4E;_LL28: if(_tmp4C <= (
void*)1?1:*((int*)_tmp4C)!= 2)goto _LL2A;_tmp4D=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp4C)->f1;_tmp4E=((struct Cyc_Absyn_Int_c_struct*)_tmp4C)->f2;_LL29: s2=_tmp4D;
i2=_tmp4E;goto _LL27;_LL2A:;_LL2B:({void*_tmp4F[0]={};Cyc_Tcutil_terr(e2->loc,({
const char*_tmp50="bad constant expression";_tag_arr(_tmp50,sizeof(char),
_get_zero_arr_size(_tmp50,24));}),_tag_arr(_tmp4F,sizeof(void*),0));});return({
struct _tuple5 _tmp51;_tmp51.f1=_tmp40;_tmp51.f2=1;_tmp51;});_LL27:;}{void*_tmp52=
p;_LL2D: if((int)_tmp52 != 3)goto _LL2F;_LL2E: goto _LL30;_LL2F: if((int)_tmp52 != 4)
goto _LL31;_LL30: if(i2 == 0){({void*_tmp53[0]={};Cyc_Tcutil_terr(e2->loc,({const
char*_tmp54="division by zero in constant expression";_tag_arr(_tmp54,sizeof(
char),_get_zero_arr_size(_tmp54,40));}),_tag_arr(_tmp53,sizeof(void*),0));});
return({struct _tuple5 _tmp55;_tmp55.f1=_tmp40;_tmp55.f2=1;_tmp55;});}goto _LL2C;
_LL31:;_LL32: goto _LL2C;_LL2C:;}{int has_u_arg=s1 == (void*)1?1: s2 == (void*)1;
unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple5
_tmp57=({struct _tuple5 _tmp56;_tmp56.f1=p;_tmp56.f2=has_u_arg;_tmp56;});void*
_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int _tmp5D;void*_tmp5E;int
_tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*_tmp64;int _tmp65;void*
_tmp66;int _tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int _tmp6B;void*_tmp6C;void*
_tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*_tmp72;int _tmp73;void*
_tmp74;int _tmp75;void*_tmp76;int _tmp77;void*_tmp78;int _tmp79;void*_tmp7A;int
_tmp7B;void*_tmp7C;int _tmp7D;void*_tmp7E;void*_tmp7F;void*_tmp80;void*_tmp81;
void*_tmp82;void*_tmp83;_LL34: _tmp58=_tmp57.f1;if((int)_tmp58 != 0)goto _LL36;
_tmp59=_tmp57.f2;if(_tmp59 != 0)goto _LL36;_LL35: i3=i1 + i2;use_i3=1;goto _LL33;
_LL36: _tmp5A=_tmp57.f1;if((int)_tmp5A != 1)goto _LL38;_tmp5B=_tmp57.f2;if(_tmp5B != 
0)goto _LL38;_LL37: i3=i1 * i2;use_i3=1;goto _LL33;_LL38: _tmp5C=_tmp57.f1;if((int)
_tmp5C != 2)goto _LL3A;_tmp5D=_tmp57.f2;if(_tmp5D != 0)goto _LL3A;_LL39: i3=i1 - i2;
use_i3=1;goto _LL33;_LL3A: _tmp5E=_tmp57.f1;if((int)_tmp5E != 3)goto _LL3C;_tmp5F=
_tmp57.f2;if(_tmp5F != 0)goto _LL3C;_LL3B: i3=i1 / i2;use_i3=1;goto _LL33;_LL3C:
_tmp60=_tmp57.f1;if((int)_tmp60 != 4)goto _LL3E;_tmp61=_tmp57.f2;if(_tmp61 != 0)
goto _LL3E;_LL3D: i3=i1 % i2;use_i3=1;goto _LL33;_LL3E: _tmp62=_tmp57.f1;if((int)
_tmp62 != 0)goto _LL40;_tmp63=_tmp57.f2;if(_tmp63 != 1)goto _LL40;_LL3F: u3=u1 + u2;
use_u3=1;goto _LL33;_LL40: _tmp64=_tmp57.f1;if((int)_tmp64 != 1)goto _LL42;_tmp65=
_tmp57.f2;if(_tmp65 != 1)goto _LL42;_LL41: u3=u1 * u2;use_u3=1;goto _LL33;_LL42:
_tmp66=_tmp57.f1;if((int)_tmp66 != 2)goto _LL44;_tmp67=_tmp57.f2;if(_tmp67 != 1)
goto _LL44;_LL43: u3=u1 - u2;use_u3=1;goto _LL33;_LL44: _tmp68=_tmp57.f1;if((int)
_tmp68 != 3)goto _LL46;_tmp69=_tmp57.f2;if(_tmp69 != 1)goto _LL46;_LL45: u3=u1 / u2;
use_u3=1;goto _LL33;_LL46: _tmp6A=_tmp57.f1;if((int)_tmp6A != 4)goto _LL48;_tmp6B=
_tmp57.f2;if(_tmp6B != 1)goto _LL48;_LL47: u3=u1 % u2;use_u3=1;goto _LL33;_LL48:
_tmp6C=_tmp57.f1;if((int)_tmp6C != 5)goto _LL4A;_LL49: b3=i1 == i2;use_b3=1;goto
_LL33;_LL4A: _tmp6D=_tmp57.f1;if((int)_tmp6D != 6)goto _LL4C;_LL4B: b3=i1 != i2;
use_b3=1;goto _LL33;_LL4C: _tmp6E=_tmp57.f1;if((int)_tmp6E != 7)goto _LL4E;_tmp6F=
_tmp57.f2;if(_tmp6F != 0)goto _LL4E;_LL4D: b3=i1 > i2;use_b3=1;goto _LL33;_LL4E:
_tmp70=_tmp57.f1;if((int)_tmp70 != 8)goto _LL50;_tmp71=_tmp57.f2;if(_tmp71 != 0)
goto _LL50;_LL4F: b3=i1 < i2;use_b3=1;goto _LL33;_LL50: _tmp72=_tmp57.f1;if((int)
_tmp72 != 9)goto _LL52;_tmp73=_tmp57.f2;if(_tmp73 != 0)goto _LL52;_LL51: b3=i1 >= i2;
use_b3=1;goto _LL33;_LL52: _tmp74=_tmp57.f1;if((int)_tmp74 != 10)goto _LL54;_tmp75=
_tmp57.f2;if(_tmp75 != 0)goto _LL54;_LL53: b3=i1 <= i2;use_b3=1;goto _LL33;_LL54:
_tmp76=_tmp57.f1;if((int)_tmp76 != 7)goto _LL56;_tmp77=_tmp57.f2;if(_tmp77 != 1)
goto _LL56;_LL55: b3=u1 > u2;use_b3=1;goto _LL33;_LL56: _tmp78=_tmp57.f1;if((int)
_tmp78 != 8)goto _LL58;_tmp79=_tmp57.f2;if(_tmp79 != 1)goto _LL58;_LL57: b3=u1 < u2;
use_b3=1;goto _LL33;_LL58: _tmp7A=_tmp57.f1;if((int)_tmp7A != 9)goto _LL5A;_tmp7B=
_tmp57.f2;if(_tmp7B != 1)goto _LL5A;_LL59: b3=u1 >= u2;use_b3=1;goto _LL33;_LL5A:
_tmp7C=_tmp57.f1;if((int)_tmp7C != 10)goto _LL5C;_tmp7D=_tmp57.f2;if(_tmp7D != 1)
goto _LL5C;_LL5B: b3=u1 <= u2;use_b3=1;goto _LL33;_LL5C: _tmp7E=_tmp57.f1;if((int)
_tmp7E != 13)goto _LL5E;_LL5D: u3=u1 & u2;use_u3=1;goto _LL33;_LL5E: _tmp7F=_tmp57.f1;
if((int)_tmp7F != 14)goto _LL60;_LL5F: u3=u1 | u2;use_u3=1;goto _LL33;_LL60: _tmp80=
_tmp57.f1;if((int)_tmp80 != 15)goto _LL62;_LL61: u3=u1 ^ u2;use_u3=1;goto _LL33;_LL62:
_tmp81=_tmp57.f1;if((int)_tmp81 != 16)goto _LL64;_LL63: u3=u1 << u2;use_u3=1;goto
_LL33;_LL64: _tmp82=_tmp57.f1;if((int)_tmp82 != 17)goto _LL66;_LL65: u3=u1 >> u2;
use_u3=1;goto _LL33;_LL66: _tmp83=_tmp57.f1;if((int)_tmp83 != 18)goto _LL68;_LL67:({
void*_tmp84[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp85=">>> NOT IMPLEMENTED";
_tag_arr(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,20));}),_tag_arr(_tmp84,
sizeof(void*),0));});goto _LL69;_LL68:;_LL69:({void*_tmp86[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmp87="bad constant expression";_tag_arr(_tmp87,sizeof(char),
_get_zero_arr_size(_tmp87,24));}),_tag_arr(_tmp86,sizeof(void*),0));});return({
struct _tuple5 _tmp88;_tmp88.f1=_tmp40;_tmp88.f2=1;_tmp88;});_LL33:;}if(use_i3)
return({struct _tuple5 _tmp89;_tmp89.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_Int_c_struct _tmp8B;
_tmp8B.tag=2;_tmp8B.f1=(void*)((void*)0);_tmp8B.f2=i3;_tmp8B;});_tmp8A;});_tmp89.f2=
1;_tmp89;});if(use_u3)return({struct _tuple5 _tmp8C;_tmp8C.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_Int_c_struct _tmp8E;
_tmp8E.tag=2;_tmp8E.f1=(void*)((void*)1);_tmp8E.f2=(int)u3;_tmp8E;});_tmp8D;});
_tmp8C.f2=1;_tmp8C;});if(use_b3)return({struct _tuple5 _tmp8F;_tmp8F.f1=(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({
struct Cyc_Absyn_Int_c_struct _tmp91;_tmp91.tag=2;_tmp91.f1=(void*)((void*)0);
_tmp91.f2=b3?1: 0;_tmp91;});_tmp90;});_tmp8F.f2=1;_tmp8F;});({void*_tmp92[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp93="Evexp::eval_const_binop";_tag_arr(_tmp93,sizeof(char),_get_zero_arr_size(
_tmp93,24));}),_tag_arr(_tmp92,sizeof(void*),0));});}}}}static struct _tuple5 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){struct _tuple5 ans;{void*_tmp94=(void*)e->r;void*_tmp95;
struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp98;
void*_tmp99;struct Cyc_List_List*_tmp9A;void*_tmp9B;struct Cyc_Absyn_Exp*_tmp9C;
struct Cyc_Absyn_Enumfield*_tmp9D;struct Cyc_Absyn_Enumfield*_tmp9E;_LL6B: if(*((
int*)_tmp94)!= 0)goto _LL6D;_tmp95=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp94)->f1;_LL6C: return({struct _tuple5 _tmp9F;_tmp9F.f1=_tmp95;_tmp9F.f2=1;
_tmp9F;});_LL6D: if(*((int*)_tmp94)!= 6)goto _LL6F;_tmp96=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp94)->f1;_tmp97=((struct Cyc_Absyn_Conditional_e_struct*)_tmp94)->f2;_tmp98=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp94)->f3;_LL6E: {int _tmpA1;int _tmpA2;
struct _tuple6 _tmpA0=Cyc_Evexp_eval_const_bool_exp(_tmp96);_tmpA1=_tmpA0.f1;
_tmpA2=_tmpA0.f2;if(!_tmpA2){Cyc_Evexp_eval_const_exp(_tmp97);Cyc_Evexp_eval_const_exp(
_tmp98);return({struct _tuple5 _tmpA3;_tmpA3.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_Int_c_struct _tmpA5;
_tmpA5.tag=2;_tmpA5.f1=(void*)((void*)0);_tmpA5.f2=0;_tmpA5;});_tmpA4;});_tmpA3.f2=
0;_tmpA3;});}ans=_tmpA1?Cyc_Evexp_eval_const_exp(_tmp97): Cyc_Evexp_eval_const_exp(
_tmp98);goto _LL6A;}_LL6F: if(*((int*)_tmp94)!= 3)goto _LL71;_tmp99=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp94)->f1;_tmp9A=((struct Cyc_Absyn_Primop_e_struct*)
_tmp94)->f2;_LL70: if(_tmp9A == 0){({void*_tmpA6[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmpA7="bad static expression (no args to primop)";_tag_arr(_tmpA7,
sizeof(char),_get_zero_arr_size(_tmpA7,42));}),_tag_arr(_tmpA6,sizeof(void*),0));});
return({struct _tuple5 _tmpA8;_tmpA8.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_Int_c_struct _tmpAA;
_tmpAA.tag=2;_tmpAA.f1=(void*)((void*)0);_tmpAA.f2=0;_tmpAA;});_tmpA9;});_tmpA8.f2=
1;_tmpA8;});}if(_tmp9A->tl == 0){ans=Cyc_Evexp_eval_const_unprimop(_tmp99,(struct
Cyc_Absyn_Exp*)_tmp9A->hd);goto _LL6A;}if(((struct Cyc_List_List*)_check_null(
_tmp9A->tl))->tl == 0){ans=Cyc_Evexp_eval_const_binprimop(_tmp99,(struct Cyc_Absyn_Exp*)
_tmp9A->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A->tl))->hd);
goto _LL6A;}({void*_tmpAB[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmpAC="bad static expression (too many args to primop)";
_tag_arr(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,48));}),_tag_arr(_tmpAB,
sizeof(void*),0));});return({struct _tuple5 _tmpAD;_tmpAD.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_Int_c_struct _tmpAF;
_tmpAF.tag=2;_tmpAF.f1=(void*)((void*)0);_tmpAF.f2=0;_tmpAF;});_tmpAE;});_tmpAD.f2=
1;_tmpAD;});_LL71: if(*((int*)_tmp94)!= 16)goto _LL73;_LL72: goto _LL74;_LL73: if(*((
int*)_tmp94)!= 17)goto _LL75;_LL74: goto _LL76;_LL75: if(*((int*)_tmp94)!= 18)goto
_LL77;_LL76: ans=({struct _tuple5 _tmpB0;_tmpB0.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Int_c_struct _tmpB2;
_tmpB2.tag=2;_tmpB2.f1=(void*)((void*)0);_tmpB2.f2=0;_tmpB2;});_tmpB1;});_tmpB0.f2=
0;_tmpB0;});goto _LL6A;_LL77: if(*((int*)_tmp94)!= 13)goto _LL79;_tmp9B=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp94)->f1;_tmp9C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp94)->f2;_LL78: ans=Cyc_Evexp_eval_const_exp(_tmp9C);if(ans.f2){struct _tuple3
_tmpB4=({struct _tuple3 _tmpB3;_tmpB3.f1=Cyc_Tcutil_compress(_tmp9B);_tmpB3.f2=ans.f1;
_tmpB3;});void*_tmpB5;void*_tmpB6;void*_tmpB7;void*_tmpB8;void*_tmpB9;char _tmpBA;
void*_tmpBB;void*_tmpBC;void*_tmpBD;void*_tmpBE;void*_tmpBF;short _tmpC0;void*
_tmpC1;void*_tmpC2;void*_tmpC3;void*_tmpC4;void*_tmpC5;int _tmpC6;_LL80: _tmpB5=
_tmpB4.f1;if(_tmpB5 <= (void*)3?1:*((int*)_tmpB5)!= 5)goto _LL82;_tmpB6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpB5)->f1;_tmpB7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB5)->f2;_tmpB8=_tmpB4.f2;if(_tmpB8 <= (void*)1?1:*((int*)_tmpB8)!= 0)goto
_LL82;_tmpB9=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmpB8)->f1;_tmpBA=((struct
Cyc_Absyn_Char_c_struct*)_tmpB8)->f2;_LL81: _tmpBC=_tmpB6;_tmpBD=_tmpB7;_tmpBF=
_tmpB9;_tmpC0=(short)_tmpBA;goto _LL83;_LL82: _tmpBB=_tmpB4.f1;if(_tmpBB <= (void*)
3?1:*((int*)_tmpBB)!= 5)goto _LL84;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_tmpBE=
_tmpB4.f2;if(_tmpBE <= (void*)1?1:*((int*)_tmpBE)!= 1)goto _LL84;_tmpBF=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmpBE)->f1;_tmpC0=((struct Cyc_Absyn_Short_c_struct*)
_tmpBE)->f2;_LL83: _tmpC2=_tmpBC;_tmpC3=_tmpBD;_tmpC5=_tmpBF;_tmpC6=(int)_tmpC0;
goto _LL85;_LL84: _tmpC1=_tmpB4.f1;if(_tmpC1 <= (void*)3?1:*((int*)_tmpC1)!= 5)goto
_LL86;_tmpC2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC1)->f1;_tmpC3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpC1)->f2;_tmpC4=_tmpB4.f2;if(_tmpC4 <= (void*)
1?1:*((int*)_tmpC4)!= 2)goto _LL86;_tmpC5=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpC4)->f1;_tmpC6=((struct Cyc_Absyn_Int_c_struct*)_tmpC4)->f2;_LL85: if(_tmpC2 != 
_tmpC5)ans.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpC7=_cycalloc(sizeof(*
_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Int_c_struct _tmpC8;_tmpC8.tag=2;_tmpC8.f1=(
void*)_tmpC2;_tmpC8.f2=_tmpC6;_tmpC8;});_tmpC7;});goto _LL7F;_LL86:;_LL87:({
struct Cyc_String_pa_struct _tmpCB;_tmpCB.tag=0;_tmpCB.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp9B));{void*_tmpC9[1]={& _tmpCB};Cyc_Tcutil_terr(
e->loc,({const char*_tmpCA="cannot cast to %s";_tag_arr(_tmpCA,sizeof(char),
_get_zero_arr_size(_tmpCA,18));}),_tag_arr(_tmpC9,sizeof(void*),1));}});goto
_LL7F;_LL7F:;}goto _LL6A;_LL79: if(*((int*)_tmp94)!= 32)goto _LL7B;_tmp9D=((struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp94)->f3;_LL7A: _tmp9E=_tmp9D;goto _LL7C;_LL7B: if(*((
int*)_tmp94)!= 31)goto _LL7D;_tmp9E=((struct Cyc_Absyn_Enum_e_struct*)_tmp94)->f3;
_LL7C: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp9E))->tag));goto _LL6A;_LL7D:;_LL7E:({struct Cyc_String_pa_struct
_tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmpCC[1]={& _tmpCE};Cyc_Tcutil_terr(e->loc,({const char*_tmpCD="bad static expression %s";
_tag_arr(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,25));}),_tag_arr(_tmpCC,
sizeof(void*),1));}});return({struct _tuple5 _tmpCF;_tmpCF.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_Int_c_struct _tmpD1;
_tmpD1.tag=2;_tmpD1.f1=(void*)((void*)0);_tmpD1.f2=0;_tmpD1;});_tmpD0;});_tmpCF.f2=
1;_tmpCF;});_LL6A:;}if(ans.f2)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_Const_e_struct
_tmpD3;_tmpD3.tag=0;_tmpD3.f1=(void*)ans.f1;_tmpD3;});_tmpD2;})));return ans;}
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){void*_tmpD4=(
void*)e->r;_LL89: if(*((int*)_tmpD4)!= 0)goto _LL8B;_LL8A: return 1;_LL8B: if(*((int*)
_tmpD4)!= 6)goto _LL8D;_LL8C: return 2;_LL8D: if(*((int*)_tmpD4)!= 3)goto _LL8F;_LL8E:
return 3;_LL8F: if(*((int*)_tmpD4)!= 16)goto _LL91;_LL90: goto _LL92;_LL91: if(*((int*)
_tmpD4)!= 17)goto _LL93;_LL92: return 4;_LL93: if(*((int*)_tmpD4)!= 18)goto _LL95;
_LL94: return 5;_LL95: if(*((int*)_tmpD4)!= 13)goto _LL97;_LL96: return 6;_LL97:;_LL98:({
struct Cyc_String_pa_struct _tmpD7;_tmpD7.tag=0;_tmpD7.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmpD5[1]={& _tmpD7};Cyc_Tcutil_terr(
e->loc,({const char*_tmpD6="bad static expression %s";_tag_arr(_tmpD6,sizeof(char),
_get_zero_arr_size(_tmpD6,25));}),_tag_arr(_tmpD5,sizeof(void*),1));}});return 0;
_LL88:;}int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
unsigned int _tmpD9;int _tmpDA;struct _tuple4 _tmpD8=Cyc_Evexp_eval_const_uint_exp(
e1);_tmpD9=_tmpD8.f1;_tmpDA=_tmpD8.f2;{unsigned int _tmpDC;int _tmpDD;struct
_tuple4 _tmpDB=Cyc_Evexp_eval_const_uint_exp(e2);_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;
if(_tmpDA?_tmpDD: 0)return(int)(_tmpD9 - _tmpDC);{int _tmpDE=Cyc_Evexp_const_exp_case_number(
e1);int _tmpDF=Cyc_Evexp_const_exp_case_number(e2);if(_tmpDE != _tmpDF)return
_tmpDE - _tmpDF;{struct _tuple3 _tmpE1=({struct _tuple3 _tmpE0;_tmpE0.f1=(void*)e1->r;
_tmpE0.f2=(void*)e2->r;_tmpE0;});void*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct
Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;void*_tmpE6;struct Cyc_Absyn_Exp*
_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpEA;void*
_tmpEB;struct Cyc_List_List*_tmpEC;void*_tmpED;void*_tmpEE;struct Cyc_List_List*
_tmpEF;void*_tmpF0;void*_tmpF1;void*_tmpF2;void*_tmpF3;void*_tmpF4;void*_tmpF5;
void*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;void*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;
void*_tmpFA;void*_tmpFB;void*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;void*_tmpFE;
struct Cyc_Absyn_Exp*_tmpFF;void*_tmp100;void*_tmp101;void*_tmp102;void*_tmp103;
void*_tmp104;void*_tmp105;void*_tmp106;void*_tmp107;struct Cyc_Absyn_Exp*_tmp108;
void*_tmp109;void*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;_LL9A: _tmpE2=_tmpE1.f1;if(*((
int*)_tmpE2)!= 6)goto _LL9C;_tmpE3=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE2)->f1;
_tmpE4=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE2)->f2;_tmpE5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE2)->f3;_tmpE6=_tmpE1.f2;if(*((int*)_tmpE6)!= 6)goto _LL9C;_tmpE7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE6)->f1;_tmpE8=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE6)->f2;_tmpE9=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpE6)->f3;_LL9B: {int _tmp10C=Cyc_Evexp_const_exp_cmp(
_tmpE3,_tmpE7);if(_tmp10C != 0)return _tmp10C;{int _tmp10D=Cyc_Evexp_const_exp_cmp(
_tmpE4,_tmpE8);if(_tmp10D != 0)return _tmp10D;return Cyc_Evexp_const_exp_cmp(_tmpE5,
_tmpE9);}}_LL9C: _tmpEA=_tmpE1.f1;if(*((int*)_tmpEA)!= 3)goto _LL9E;_tmpEB=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpEA)->f1;_tmpEC=((struct Cyc_Absyn_Primop_e_struct*)
_tmpEA)->f2;_tmpED=_tmpE1.f2;if(*((int*)_tmpED)!= 3)goto _LL9E;_tmpEE=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpED)->f1;_tmpEF=((struct Cyc_Absyn_Primop_e_struct*)
_tmpED)->f2;_LL9D: {int _tmp10E=(int)_tmpEB - (int)_tmpEE;if(_tmp10E != 0)return
_tmp10E;for(0;_tmpEC != 0?_tmpEF != 0: 0;(_tmpEC=_tmpEC->tl,_tmpEF=_tmpEF->tl)){int
_tmp10F=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmpEC->hd,(struct Cyc_Absyn_Exp*)
_tmpEF->hd);if(_tmp10F != 0)return _tmp10F;}return 0;}_LL9E: _tmpF0=_tmpE1.f1;if(*((
int*)_tmpF0)!= 16)goto _LLA0;_tmpF1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpF0)->f1;_tmpF2=_tmpE1.f2;if(*((int*)_tmpF2)!= 16)goto _LLA0;_tmpF3=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpF2)->f1;_LL9F: return Cyc_Tcutil_typecmp(
_tmpF1,_tmpF3);_LLA0: _tmpF4=_tmpE1.f1;if(*((int*)_tmpF4)!= 16)goto _LLA2;_tmpF5=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpF4)->f1;_tmpF6=_tmpE1.f2;if(*((
int*)_tmpF6)!= 17)goto _LLA2;_tmpF7=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpF6)->f1;
_LLA1: if(_tmpF7->topt == 0)({void*_tmp110[0]={};Cyc_Tcutil_terr(e2->loc,({const
char*_tmp111="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,51));}),_tag_arr(_tmp110,sizeof(
void*),0));});return Cyc_Tcutil_typecmp(_tmpF5,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF7->topt))->v);_LLA2: _tmpF8=_tmpE1.f1;if(*((int*)_tmpF8)!= 17)
goto _LLA4;_tmpF9=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpF8)->f1;_tmpFA=_tmpE1.f2;
if(*((int*)_tmpFA)!= 16)goto _LLA4;_tmpFB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpFA)->f1;_LLA3: if(_tmpF9->topt == 0)({void*_tmp112[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp113="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,51));}),_tag_arr(_tmp112,sizeof(
void*),0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF9->topt))->v,_tmpFB);_LLA4: _tmpFC=_tmpE1.f1;if(*((int*)_tmpFC)!= 17)goto
_LLA6;_tmpFD=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFC)->f1;_tmpFE=_tmpE1.f2;
if(*((int*)_tmpFE)!= 17)goto _LLA6;_tmpFF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpFE)->f1;_LLA5: if(_tmpFD->topt == 0)({void*_tmp114[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp115="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,51));}),_tag_arr(_tmp114,sizeof(
void*),0));});if(_tmpFF->topt == 0)({void*_tmp116[0]={};Cyc_Tcutil_terr(e2->loc,({
const char*_tmp117="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_arr(
_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,51));}),_tag_arr(_tmp116,sizeof(
void*),0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpFD->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpFF->topt))->v);
_LLA6: _tmp100=_tmpE1.f1;if(*((int*)_tmp100)!= 18)goto _LLA8;_tmp101=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp100)->f1;_tmp102=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp100)->f2;_tmp103=_tmpE1.f2;if(*((int*)_tmp103)!= 18)goto _LLA8;_tmp104=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp103)->f1;_tmp105=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp103)->f2;_LLA7: {int _tmp118=Cyc_Tcutil_typecmp(_tmp101,_tmp104);if(_tmp118 != 
0)return _tmp118;{struct _tuple3 _tmp11A=({struct _tuple3 _tmp119;_tmp119.f1=_tmp102;
_tmp119.f2=_tmp105;_tmp119;});void*_tmp11B;void*_tmp11C;void*_tmp11D;void*
_tmp11E;void*_tmp11F;struct _tagged_arr*_tmp120;void*_tmp121;struct _tagged_arr*
_tmp122;void*_tmp123;unsigned int _tmp124;void*_tmp125;unsigned int _tmp126;_LLAD:
_tmp11B=_tmp11A.f1;if(*((int*)_tmp11B)!= 0)goto _LLAF;_tmp11C=_tmp11A.f2;if(*((
int*)_tmp11C)!= 1)goto _LLAF;_LLAE: return - 1;_LLAF: _tmp11D=_tmp11A.f1;if(*((int*)
_tmp11D)!= 1)goto _LLB1;_tmp11E=_tmp11A.f2;if(*((int*)_tmp11E)!= 0)goto _LLB1;
_LLB0: return 1;_LLB1: _tmp11F=_tmp11A.f1;if(*((int*)_tmp11F)!= 0)goto _LLB3;_tmp120=((
struct Cyc_Absyn_StructField_struct*)_tmp11F)->f1;_tmp121=_tmp11A.f2;if(*((int*)
_tmp121)!= 0)goto _LLB3;_tmp122=((struct Cyc_Absyn_StructField_struct*)_tmp121)->f1;
_LLB2: return Cyc_strptrcmp(_tmp120,_tmp122);_LLB3: _tmp123=_tmp11A.f1;if(*((int*)
_tmp123)!= 1)goto _LLAC;_tmp124=((struct Cyc_Absyn_TupleIndex_struct*)_tmp123)->f1;
_tmp125=_tmp11A.f2;if(*((int*)_tmp125)!= 1)goto _LLAC;_tmp126=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp125)->f1;_LLB4: return(int)(_tmp124 - _tmp126);_LLAC:;}}_LLA8: _tmp106=_tmpE1.f1;
if(*((int*)_tmp106)!= 13)goto _LLAA;_tmp107=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp106)->f1;_tmp108=((struct Cyc_Absyn_Cast_e_struct*)_tmp106)->f2;_tmp109=
_tmpE1.f2;if(*((int*)_tmp109)!= 13)goto _LLAA;_tmp10A=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp109)->f1;_tmp10B=((struct Cyc_Absyn_Cast_e_struct*)_tmp109)->f2;_LLA9: {int
_tmp127=Cyc_Tcutil_typecmp(_tmp107,_tmp10A);if(_tmp127 != 0)return _tmp127;return
Cyc_Evexp_const_exp_cmp(_tmp108,_tmp10B);}_LLAA:;_LLAB:({void*_tmp128[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp129="Evexp::const_exp_cmp, unexpected case";_tag_arr(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,38));}),_tag_arr(_tmp128,sizeof(void*),0));});_LL99:;}}}}
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmp12B;int _tmp12C;struct _tuple4 _tmp12A=
Cyc_Evexp_eval_const_uint_exp(e1);_tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;{
unsigned int _tmp12E;int _tmp12F;struct _tuple4 _tmp12D=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;if(_tmp12C?_tmp12F: 0)return _tmp12B <= 
_tmp12E;return Cyc_Evexp_same_const_exp(e1,e2);}}struct _tuple7{struct Cyc_Absyn_Tqual
f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){void*_tmp130=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp131;struct Cyc_Core_Opt*_tmp132;struct Cyc_Core_Opt
_tmp133;void*_tmp134;struct Cyc_Absyn_TunionFieldInfo _tmp135;void*_tmp136;struct
Cyc_Absyn_Tunionfield*_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_ArrayInfo
_tmp139;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_AggrInfo _tmp13C;
void*_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_Typedefdecl*_tmp13F;
_LLB6: if((int)_tmp130 != 0)goto _LLB8;_LLB7: return 0;_LLB8: if(_tmp130 <= (void*)3?1:*((
int*)_tmp130)!= 1)goto _LLBA;_tmp131=((struct Cyc_Absyn_VarType_struct*)_tmp130)->f1;
_LLB9: {void*_tmp140=Cyc_Tcutil_tvar_kind(_tmp131);_LLED: if((int)_tmp140 != 2)
goto _LLEF;_LLEE: return 1;_LLEF:;_LLF0: return 0;_LLEC:;}_LLBA: if(_tmp130 <= (void*)3?
1:*((int*)_tmp130)!= 0)goto _LLBC;_tmp132=((struct Cyc_Absyn_Evar_struct*)_tmp130)->f1;
if(_tmp132 == 0)goto _LLBC;_tmp133=*_tmp132;_tmp134=(void*)_tmp133.v;if((int)
_tmp134 != 2)goto _LLBC;_LLBB: return 1;_LLBC: if(_tmp130 <= (void*)3?1:*((int*)
_tmp130)!= 0)goto _LLBE;_LLBD: return 0;_LLBE: if(_tmp130 <= (void*)3?1:*((int*)
_tmp130)!= 2)goto _LLC0;_LLBF: return 1;_LLC0: if(_tmp130 <= (void*)3?1:*((int*)
_tmp130)!= 3)goto _LLC2;_tmp135=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp130)->f1;
_tmp136=(void*)_tmp135.field_info;if(*((int*)_tmp136)!= 1)goto _LLC2;_tmp137=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp136)->f2;_LLC1: _tmp138=_tmp137->typs;
goto _LLC3;_LLC2: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 9)goto _LLC4;_tmp138=((
struct Cyc_Absyn_TupleType_struct*)_tmp130)->f1;_LLC3: for(0;_tmp138 != 0;_tmp138=
_tmp138->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple7*)_tmp138->hd)).f2))
return 0;}return 1;_LLC4: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 3)goto _LLC6;
_LLC5: return 0;_LLC6: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 4)goto _LLC8;_LLC7:
return 1;_LLC8: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 14)goto _LLCA;_LLC9: goto
_LLCB;_LLCA: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 17)goto _LLCC;_LLCB: goto
_LLCD;_LLCC: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 5)goto _LLCE;_LLCD: goto
_LLCF;_LLCE: if((int)_tmp130 != 1)goto _LLD0;_LLCF: goto _LLD1;_LLD0: if(_tmp130 <= (
void*)3?1:*((int*)_tmp130)!= 6)goto _LLD2;_LLD1: return 1;_LLD2: if(_tmp130 <= (void*)
3?1:*((int*)_tmp130)!= 7)goto _LLD4;_tmp139=((struct Cyc_Absyn_ArrayType_struct*)
_tmp130)->f1;_tmp13A=(void*)_tmp139.elt_type;_tmp13B=_tmp139.num_elts;_LLD3:
return _tmp13B != 0;_LLD4: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 8)goto _LLD6;
_LLD5: return 0;_LLD6: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 15)goto _LLD8;
_LLD7: return 1;_LLD8: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 10)goto _LLDA;
_tmp13C=((struct Cyc_Absyn_AggrType_struct*)_tmp130)->f1;_tmp13D=(void*)_tmp13C.aggr_info;
_LLD9: {struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp141=_tmp13D;struct Cyc_Absyn_Aggrdecl**
_tmp142;struct Cyc_Absyn_Aggrdecl*_tmp143;_LLF2: if(*((int*)_tmp141)!= 0)goto _LLF4;
_LLF3:({void*_tmp144[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp145="szof on unchecked StructType";_tag_arr(
_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,29));}),_tag_arr(_tmp144,sizeof(
void*),0));});_LLF4: if(*((int*)_tmp141)!= 1)goto _LLF1;_tmp142=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp141)->f1;_tmp143=*_tmp142;_LLF5: ad=_tmp143;_LLF1:;}if(ad->impl == 0)return 0;
_tmp13E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDB;}
_LLDA: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 11)goto _LLDC;_tmp13E=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp130)->f2;_LLDB: for(0;_tmp13E != 0;_tmp13E=
_tmp13E->tl){if(!Cyc_Evexp_okay_szofarg((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp13E->hd)->type))return 0;}return 1;_LLDC: if(_tmp130 <= (void*)3?1:*((int*)
_tmp130)!= 12)goto _LLDE;_LLDD: goto _LLDF;_LLDE: if(_tmp130 <= (void*)3?1:*((int*)
_tmp130)!= 13)goto _LLE0;_LLDF: return 1;_LLE0: if(_tmp130 <= (void*)3?1:*((int*)
_tmp130)!= 18)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if((int)_tmp130 != 2)goto _LLE4;
_LLE3: goto _LLE5;_LLE4: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 20)goto _LLE6;
_LLE5: goto _LLE7;_LLE6: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 19)goto _LLE8;
_LLE7: goto _LLE9;_LLE8: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 21)goto _LLEA;
_LLE9: return 0;_LLEA: if(_tmp130 <= (void*)3?1:*((int*)_tmp130)!= 16)goto _LLB5;
_tmp13F=((struct Cyc_Absyn_TypedefType_struct*)_tmp130)->f3;_LLEB: if(_tmp13F == 0?
1: _tmp13F->kind == 0)({void*_tmp146[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp149;_tmp149.tag=0;_tmp149.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp147[1]={& _tmp149};Cyc_aprintf(({const char*_tmp148="szof typedeftype %s";
_tag_arr(_tmp148,sizeof(char),_get_zero_arr_size(_tmp148,20));}),_tag_arr(
_tmp147,sizeof(void*),1));}}),_tag_arr(_tmp146,sizeof(void*),0));});return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp13F->kind))->v == (void*)2;_LLB5:;}
