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
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int isdigit(int);
unsigned int Cyc_strlen(struct _tagged_arr s);struct _tagged_arr Cyc_implode(struct
Cyc_List_List*c);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Absyn_Rel_n_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct
Cyc_Tcenv_Tenv*);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _tagged_arr
s,int i);static struct Cyc_Core_Opt Cyc_Formatstr_rk={(void*)((void*)3)};struct
_tuple4{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;char f5;int f6;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s,int i){unsigned int _tmp0=Cyc_strlen((struct _tagged_arr)s);if(i
< 0?1: i >= _tmp0)return 0;{struct Cyc_List_List*_tmp1=0;char c=' ';for(0;i < _tmp0;i
++){c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));switch(c){case '+':
_LL0: goto _LL1;case '-': _LL1: goto _LL2;case ' ': _LL2: goto _LL3;case '#': _LL3: goto _LL4;
case '0': _LL4: _tmp1=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=(
void*)((int)c);_tmp2->tl=_tmp1;_tmp2;});continue;default: _LL5: break;}break;}if(i
>= _tmp0)return 0;_tmp1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp1);{struct Cyc_List_List*_tmp3=0;c=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(c == '*'){_tmp3=({struct Cyc_List_List*_tmp4=_cycalloc(sizeof(*
_tmp4));_tmp4->hd=(void*)((int)c);_tmp4->tl=_tmp3;_tmp4;});i ++;}else{for(0;i < 
_tmp0;i ++){c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(
isdigit((int)c))_tmp3=({struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));
_tmp5->hd=(void*)((int)c);_tmp5->tl=_tmp3;_tmp5;});else{break;}}}if(i >= _tmp0)
return 0;_tmp3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp3);{struct Cyc_List_List*_tmp6=0;c=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(c == '.'){_tmp6=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*
_tmp7));_tmp7->hd=(void*)((int)c);_tmp7->tl=_tmp6;_tmp7;});i ++;if(i >= _tmp0)
return 0;c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(c == '*'){
_tmp6=({struct Cyc_List_List*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=(void*)((
int)c);_tmp8->tl=_tmp6;_tmp8;});i ++;}else{for(0;i < _tmp0;i ++){c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(isdigit((int)c))_tmp6=({struct Cyc_List_List*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->hd=(void*)((int)c);_tmp9->tl=_tmp6;_tmp9;});
else{break;}}}}if(i >= _tmp0)return 0;_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp6);{struct Cyc_List_List*_tmpA=0;c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));switch(c){case 'h': _LL7: _tmpA=({
struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->hd=(void*)((int)c);
_tmpB->tl=_tmpA;_tmpB;});i ++;if(i >= _tmp0)return 0;c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(c == 'h'){_tmpA=({struct Cyc_List_List*
_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->hd=(void*)((int)c);_tmpC->tl=_tmpA;_tmpC;});
i ++;}break;case 'l': _LL8: _tmpA=({struct Cyc_List_List*_tmpD=_cycalloc(sizeof(*
_tmpD));_tmpD->hd=(void*)((int)c);_tmpD->tl=_tmpA;_tmpD;});i ++;if(i >= _tmp0)
return 0;c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(c == 'l'){
_tmpA=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=(void*)((
int)c);_tmpE->tl=_tmpA;_tmpE;});i ++;}break;case 'j': _LL9: goto _LLA;case 'z': _LLA:
goto _LLB;case 't': _LLB: goto _LLC;case 'L': _LLC: _tmpA=({struct Cyc_List_List*_tmpF=
_cycalloc(sizeof(*_tmpF));_tmpF->hd=(void*)((int)c);_tmpF->tl=_tmpA;_tmpF;});i ++;
break;default: _LLD: break;}if(i >= _tmp0)return 0;_tmpA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA);c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));switch(c){case 'd': _LLF: goto _LL10;
case 'i': _LL10: goto _LL11;case 'o': _LL11: goto _LL12;case 'u': _LL12: goto _LL13;case 'x':
_LL13: goto _LL14;case 'X': _LL14: goto _LL15;case 'f': _LL15: goto _LL16;case 'F': _LL16:
goto _LL17;case 'e': _LL17: goto _LL18;case 'E': _LL18: goto _LL19;case 'g': _LL19: goto
_LL1A;case 'G': _LL1A: goto _LL1B;case 'a': _LL1B: goto _LL1C;case 'A': _LL1C: goto _LL1D;
case 'c': _LL1D: goto _LL1E;case 's': _LL1E: goto _LL1F;case 'p': _LL1F: goto _LL20;case 'n':
_LL20: goto _LL21;case '%': _LL21: break;default: _LL22: return 0;}return({struct Cyc_Core_Opt*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->v=({struct _tuple4*_tmp11=_cycalloc(
sizeof(*_tmp11));_tmp11->f1=_tmp1;_tmp11->f2=_tmp3;_tmp11->f3=_tmp6;_tmp11->f4=
_tmpA;_tmp11->f5=c;_tmp11->f6=i + 1;_tmp11;});_tmp10;});}}}}}struct Cyc_List_List*
Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*te,struct _tagged_arr s,struct
Cyc_Position_Segment*loc){unsigned int _tmp12=Cyc_strlen((struct _tagged_arr)s);
struct Cyc_List_List*_tmp13=0;int i;for(i=0;i < _tmp12;i ++){if(*((const char*)
_check_unknown_subscript(s,sizeof(char),i))!= '%')continue;{struct Cyc_Core_Opt*
_tmp14=Cyc_Formatstr_parse_conversionspecification(s,i + 1);if(_tmp14 == 0){({void*
_tmp15[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp16="bad format string";_tag_arr(
_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,18));}),_tag_arr(_tmp15,sizeof(
void*),0));});return 0;}{struct _tuple4 _tmp18;struct Cyc_List_List*_tmp19;struct Cyc_List_List*
_tmp1A;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1C;char _tmp1D;int
_tmp1E;struct _tuple4*_tmp17=(struct _tuple4*)_tmp14->v;_tmp18=*_tmp17;_tmp19=
_tmp18.f1;_tmp1A=_tmp18.f2;_tmp1B=_tmp18.f3;_tmp1C=_tmp18.f4;_tmp1D=_tmp18.f5;
_tmp1E=_tmp18.f6;i=_tmp1E - 1;{struct Cyc_List_List*_tmp1F=_tmp1C;struct Cyc_List_List
_tmp20;int _tmp21;struct Cyc_List_List*_tmp22;_LL25: if(_tmp1F == 0)goto _LL27;_tmp20=*
_tmp1F;_tmp21=(int)_tmp20.hd;_tmp22=_tmp20.tl;if(_tmp22 != 0)goto _LL27;if(!((
_tmp21 == 'j'?1: _tmp21 == 'z')?1: _tmp21 == 't'))goto _LL27;_LL26:({struct Cyc_Int_pa_struct
_tmp25;_tmp25.tag=1;_tmp25.f1=(unsigned int)_tmp21;{void*_tmp23[1]={& _tmp25};Cyc_Tcutil_terr(
loc,({const char*_tmp24="length modifier '%c' is not supported";_tag_arr(_tmp24,
sizeof(char),_get_zero_arr_size(_tmp24,38));}),_tag_arr(_tmp23,sizeof(void*),1));}});
return 0;_LL27:;_LL28: goto _LL24;_LL24:;}{struct Cyc_List_List*_tmp26=_tmp1A;struct
Cyc_List_List _tmp27;int _tmp28;struct Cyc_List_List*_tmp29;_LL2A: if(_tmp26 == 0)
goto _LL2C;_tmp27=*_tmp26;_tmp28=(int)_tmp27.hd;_tmp29=_tmp27.tl;if(_tmp29 != 0)
goto _LL2C;if(!(_tmp28 == '*'))goto _LL2C;_LL2B: _tmp13=({struct Cyc_List_List*_tmp2A=
_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=(void*)Cyc_Absyn_sint_typ;_tmp2A->tl=
_tmp13;_tmp2A;});goto _LL29;_LL2C:;_LL2D: goto _LL29;_LL29:;}{struct Cyc_List_List*
_tmp2B=_tmp1B;struct Cyc_List_List _tmp2C;int _tmp2D;struct Cyc_List_List*_tmp2E;
struct Cyc_List_List _tmp2F;int _tmp30;struct Cyc_List_List*_tmp31;_LL2F: if(_tmp2B == 
0)goto _LL31;_tmp2C=*_tmp2B;_tmp2D=(int)_tmp2C.hd;_tmp2E=_tmp2C.tl;if(_tmp2E == 0)
goto _LL31;_tmp2F=*_tmp2E;_tmp30=(int)_tmp2F.hd;_tmp31=_tmp2F.tl;if(_tmp31 != 0)
goto _LL31;if(!(_tmp2D == '.'?_tmp30 == '*': 0))goto _LL31;_LL30: _tmp13=({struct Cyc_List_List*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->hd=(void*)Cyc_Absyn_sint_typ;_tmp32->tl=
_tmp13;_tmp32;});goto _LL2E;_LL31:;_LL32: goto _LL2E;_LL2E:;}{void*t;switch(_tmp1D){
case 'd': _LL33: goto _LL34;case 'i': _LL34:{struct Cyc_List_List*f=_tmp19;for(0;f != 0;
f=f->tl){if((int)f->hd == '#'){({struct Cyc_Int_pa_struct _tmp35;_tmp35.tag=1;
_tmp35.f1=(unsigned int)((int)_tmp1D);{void*_tmp33[1]={& _tmp35};Cyc_Tcutil_terr(
loc,({const char*_tmp34="flag '#' is not valid with %%%c";_tag_arr(_tmp34,sizeof(
char),_get_zero_arr_size(_tmp34,32));}),_tag_arr(_tmp33,sizeof(void*),1));}});
return 0;}}}{struct Cyc_List_List*_tmp36=_tmp1C;struct Cyc_List_List _tmp37;int
_tmp38;struct Cyc_List_List*_tmp39;struct Cyc_List_List _tmp3A;int _tmp3B;struct Cyc_List_List*
_tmp3C;struct Cyc_List_List _tmp3D;int _tmp3E;struct Cyc_List_List*_tmp3F;struct Cyc_List_List
_tmp40;int _tmp41;struct Cyc_List_List*_tmp42;_LL37: if(_tmp36 != 0)goto _LL39;_LL38:
t=Cyc_Absyn_sint_typ;goto _LL36;_LL39: if(_tmp36 == 0)goto _LL3B;_tmp37=*_tmp36;
_tmp38=(int)_tmp37.hd;_tmp39=_tmp37.tl;if(_tmp39 != 0)goto _LL3B;if(!(_tmp38 == 'l'))
goto _LL3B;_LL3A: t=Cyc_Absyn_slong_typ;goto _LL36;_LL3B: if(_tmp36 == 0)goto _LL3D;
_tmp3A=*_tmp36;_tmp3B=(int)_tmp3A.hd;_tmp3C=_tmp3A.tl;if(_tmp3C != 0)goto _LL3D;
if(!(_tmp3B == 'h'))goto _LL3D;_LL3C: t=Cyc_Absyn_sshort_typ;goto _LL36;_LL3D: if(
_tmp36 == 0)goto _LL3F;_tmp3D=*_tmp36;_tmp3E=(int)_tmp3D.hd;_tmp3F=_tmp3D.tl;if(
_tmp3F == 0)goto _LL3F;_tmp40=*_tmp3F;_tmp41=(int)_tmp40.hd;_tmp42=_tmp40.tl;if(
_tmp42 != 0)goto _LL3F;if(!(_tmp3E == 'h'?_tmp41 == 'h': 0))goto _LL3F;_LL3E: t=Cyc_Absyn_schar_typ;
goto _LL36;_LL3F:;_LL40:({struct Cyc_Int_pa_struct _tmp46;_tmp46.tag=1;_tmp46.f1=(
unsigned int)((int)_tmp1D);{struct Cyc_String_pa_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmp1C));{void*_tmp43[2]={&
_tmp45,& _tmp46};Cyc_Tcutil_terr(loc,({const char*_tmp44="length modifier '%s' is not allowed with %%%c";
_tag_arr(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,46));}),_tag_arr(_tmp43,
sizeof(void*),2));}}});return 0;_LL36:;}_tmp13=({struct Cyc_List_List*_tmp47=
_cycalloc(sizeof(*_tmp47));_tmp47->hd=(void*)t;_tmp47->tl=_tmp13;_tmp47;});
break;case 'u': _LL35:{struct Cyc_List_List*f=_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd
== '#'){({void*_tmp48[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp49="Flag '#' not valid with %%u";
_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,28));}),_tag_arr(_tmp48,
sizeof(void*),0));});return 0;}}}goto _LL41;case 'o': _LL41: goto _LL42;case 'x': _LL42:
goto _LL43;case 'X': _LL43:{struct Cyc_List_List*_tmp4A=_tmp1C;struct Cyc_List_List
_tmp4B;int _tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List _tmp4E;int _tmp4F;
struct Cyc_List_List*_tmp50;struct Cyc_List_List _tmp51;int _tmp52;struct Cyc_List_List*
_tmp53;struct Cyc_List_List _tmp54;int _tmp55;struct Cyc_List_List*_tmp56;_LL46: if(
_tmp4A != 0)goto _LL48;_LL47: t=Cyc_Absyn_uint_typ;goto _LL45;_LL48: if(_tmp4A == 0)
goto _LL4A;_tmp4B=*_tmp4A;_tmp4C=(int)_tmp4B.hd;_tmp4D=_tmp4B.tl;if(_tmp4D != 0)
goto _LL4A;if(!(_tmp4C == 'l'))goto _LL4A;_LL49: t=Cyc_Absyn_ulong_typ;goto _LL45;
_LL4A: if(_tmp4A == 0)goto _LL4C;_tmp4E=*_tmp4A;_tmp4F=(int)_tmp4E.hd;_tmp50=_tmp4E.tl;
if(_tmp50 != 0)goto _LL4C;if(!(_tmp4F == 'h'))goto _LL4C;_LL4B: t=Cyc_Absyn_ushort_typ;
goto _LL45;_LL4C: if(_tmp4A == 0)goto _LL4E;_tmp51=*_tmp4A;_tmp52=(int)_tmp51.hd;
_tmp53=_tmp51.tl;if(_tmp53 == 0)goto _LL4E;_tmp54=*_tmp53;_tmp55=(int)_tmp54.hd;
_tmp56=_tmp54.tl;if(_tmp56 != 0)goto _LL4E;if(!(_tmp52 == 'h'?_tmp55 == 'h': 0))goto
_LL4E;_LL4D: t=Cyc_Absyn_uchar_typ;goto _LL45;_LL4E:;_LL4F:({struct Cyc_Int_pa_struct
_tmp5A;_tmp5A.tag=1;_tmp5A.f1=(unsigned int)((int)_tmp1D);{struct Cyc_String_pa_struct
_tmp59;_tmp59.tag=0;_tmp59.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmp1C));{void*_tmp57[2]={& _tmp59,& _tmp5A};Cyc_Tcutil_terr(loc,({const char*
_tmp58="length modifier '%s' is not allowed with %%%c";_tag_arr(_tmp58,sizeof(
char),_get_zero_arr_size(_tmp58,46));}),_tag_arr(_tmp57,sizeof(void*),2));}}});
return 0;_LL45:;}_tmp13=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));
_tmp5B->hd=(void*)t;_tmp5B->tl=_tmp13;_tmp5B;});break;case 'f': _LL44: goto _LL50;
case 'F': _LL50: goto _LL51;case 'e': _LL51: goto _LL52;case 'E': _LL52: goto _LL53;case 'g':
_LL53: goto _LL54;case 'G': _LL54: goto _LL55;case 'a': _LL55: goto _LL56;case 'A': _LL56:
if(_tmp1C != 0){({struct Cyc_Int_pa_struct _tmp5E;_tmp5E.tag=1;_tmp5E.f1=(
unsigned int)((int)_tmp1D);{void*_tmp5C[1]={& _tmp5E};Cyc_Tcutil_terr(loc,({const
char*_tmp5D="length modifiers are not allowed with %%%c";_tag_arr(_tmp5D,sizeof(
char),_get_zero_arr_size(_tmp5D,43));}),_tag_arr(_tmp5C,sizeof(void*),1));}});
return 0;}_tmp13=({struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->hd=(
void*)Cyc_Absyn_double_typ(0);_tmp5F->tl=_tmp13;_tmp5F;});break;case 'c': _LL57:{
struct Cyc_List_List*f=_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd == '#'?1:(int)f->hd
== '0'){({struct Cyc_Int_pa_struct _tmp62;_tmp62.tag=1;_tmp62.f1=(unsigned int)((
int)f->hd);{void*_tmp60[1]={& _tmp62};Cyc_Tcutil_terr(loc,({const char*_tmp61="flag '%c' not allowed with %%c";
_tag_arr(_tmp61,sizeof(char),_get_zero_arr_size(_tmp61,31));}),_tag_arr(_tmp60,
sizeof(void*),1));}});return 0;}}}if(_tmp1C != 0){({struct Cyc_String_pa_struct
_tmp65;_tmp65.tag=0;_tmp65.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmp1C));{void*_tmp63[1]={& _tmp65};Cyc_Tcutil_terr(loc,({const char*_tmp64="length modifier '%s' not allowed with %%c";
_tag_arr(_tmp64,sizeof(char),_get_zero_arr_size(_tmp64,42));}),_tag_arr(_tmp63,
sizeof(void*),1));}});return 0;}if(_tmp1B != 0){({struct Cyc_String_pa_struct _tmp68;
_tmp68.tag=0;_tmp68.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmp1B));{
void*_tmp66[1]={& _tmp68};Cyc_Tcutil_terr(loc,({const char*_tmp67="precision '%s' not allowed with %%c";
_tag_arr(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,36));}),_tag_arr(_tmp66,
sizeof(void*),1));}});return 0;}_tmp13=({struct Cyc_List_List*_tmp69=_cycalloc(
sizeof(*_tmp69));_tmp69->hd=(void*)Cyc_Absyn_sint_typ;_tmp69->tl=_tmp13;_tmp69;});
break;case 's': _LL58:{struct Cyc_List_List*f=_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd
!= '-'){({void*_tmp6A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6B="a flag not allowed with %%s";
_tag_arr(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,28));}),_tag_arr(_tmp6A,
sizeof(void*),0));});return 0;}}}if(_tmp1C != 0){({void*_tmp6C[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp6D="length modifiers not allowed with %%s";_tag_arr(_tmp6D,
sizeof(char),_get_zero_arr_size(_tmp6D,38));}),_tag_arr(_tmp6C,sizeof(void*),0));});
return 0;}{void*ptr=Cyc_Absyn_tagged_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({struct Cyc_Core_Opt*_tmp6F=_cycalloc(
sizeof(*_tmp6F));_tmp6F->v=Cyc_Tcenv_lookup_type_vars(te);_tmp6F;})),Cyc_Absyn_const_tqual(),
Cyc_Absyn_false_conref);_tmp13=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E->hd=(void*)ptr;_tmp6E->tl=_tmp13;_tmp6E;});break;}case 'p': _LL59:
_tmp13=({struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->hd=(void*)
Cyc_Absyn_uint_typ;_tmp70->tl=_tmp13;_tmp70;});break;case 'n': _LL5A:{struct Cyc_List_List*
f=_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd == '#'?1:(int)f->hd == '0'){({struct
Cyc_Int_pa_struct _tmp73;_tmp73.tag=1;_tmp73.f1=(unsigned int)((int)f->hd);{void*
_tmp71[1]={& _tmp73};Cyc_Tcutil_terr(loc,({const char*_tmp72="flag '%c' not allowed with %%n";
_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,31));}),_tag_arr(_tmp71,
sizeof(void*),1));}});return 0;}}}if(_tmp1B != 0){({struct Cyc_String_pa_struct
_tmp76;_tmp76.tag=0;_tmp76.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmp1B));{void*_tmp74[1]={& _tmp76};Cyc_Tcutil_terr(loc,({const char*_tmp75="precision '%s' not allowed with %%n";
_tag_arr(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,36));}),_tag_arr(_tmp74,
sizeof(void*),1));}});return 0;}{struct Cyc_Core_Opt*_tmp77=({struct Cyc_Core_Opt*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp8A;});{struct Cyc_List_List*_tmp78=_tmp1C;struct Cyc_List_List _tmp79;int _tmp7A;
struct Cyc_List_List*_tmp7B;struct Cyc_List_List _tmp7C;int _tmp7D;struct Cyc_List_List*
_tmp7E;struct Cyc_List_List _tmp7F;int _tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List
_tmp82;int _tmp83;struct Cyc_List_List*_tmp84;_LL5D: if(_tmp78 != 0)goto _LL5F;_LL5E:
t=Cyc_Absyn_sint_typ;goto _LL5C;_LL5F: if(_tmp78 == 0)goto _LL61;_tmp79=*_tmp78;
_tmp7A=(int)_tmp79.hd;_tmp7B=_tmp79.tl;if(_tmp7B != 0)goto _LL61;if(!(_tmp7A == 'l'))
goto _LL61;_LL60: t=Cyc_Absyn_ulong_typ;goto _LL5C;_LL61: if(_tmp78 == 0)goto _LL63;
_tmp7C=*_tmp78;_tmp7D=(int)_tmp7C.hd;_tmp7E=_tmp7C.tl;if(_tmp7E != 0)goto _LL63;
if(!(_tmp7D == 'h'))goto _LL63;_LL62: t=Cyc_Absyn_sshort_typ;goto _LL5C;_LL63: if(
_tmp78 == 0)goto _LL65;_tmp7F=*_tmp78;_tmp80=(int)_tmp7F.hd;_tmp81=_tmp7F.tl;if(
_tmp81 == 0)goto _LL65;_tmp82=*_tmp81;_tmp83=(int)_tmp82.hd;_tmp84=_tmp82.tl;if(
_tmp84 != 0)goto _LL65;if(!(_tmp80 == 'h'?_tmp83 == 'h': 0))goto _LL65;_LL64: t=Cyc_Absyn_schar_typ;
goto _LL5C;_LL65:;_LL66:({struct Cyc_Int_pa_struct _tmp88;_tmp88.tag=1;_tmp88.f1=(
unsigned int)((int)_tmp1D);{struct Cyc_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmp1C));{void*_tmp85[2]={&
_tmp87,& _tmp88};Cyc_Tcutil_terr(loc,({const char*_tmp86="length modifier '%s' is not allowed with %%%c";
_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,46));}),_tag_arr(_tmp85,
sizeof(void*),2));}}});return 0;_LL5C:;}t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp77),Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);
_tmp13=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=(void*)
t;_tmp89->tl=_tmp13;_tmp89;});break;}case '%': _LL5B: if(_tmp19 != 0){({struct Cyc_String_pa_struct
_tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmp19));{void*_tmp8B[1]={& _tmp8D};Cyc_Tcutil_terr(loc,({const char*_tmp8C="flags '%s' not allowed with %%%%";
_tag_arr(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,33));}),_tag_arr(_tmp8B,
sizeof(void*),1));}});return 0;}if(_tmp1A != 0){({struct Cyc_String_pa_struct _tmp90;
_tmp90.tag=0;_tmp90.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmp1A));{
void*_tmp8E[1]={& _tmp90};Cyc_Tcutil_terr(loc,({const char*_tmp8F="width '%s' not allowed with %%%%";
_tag_arr(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,33));}),_tag_arr(_tmp8E,
sizeof(void*),1));}});return 0;}if(_tmp1B != 0){({struct Cyc_String_pa_struct _tmp93;
_tmp93.tag=0;_tmp93.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmp1B));{
void*_tmp91[1]={& _tmp93};Cyc_Tcutil_terr(loc,({const char*_tmp92="precision '%s' not allowed with %%%%";
_tag_arr(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,37));}),_tag_arr(_tmp91,
sizeof(void*),1));}});return 0;}if(_tmp1C != 0){({struct Cyc_String_pa_struct _tmp96;
_tmp96.tag=0;_tmp96.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmp1C));{
void*_tmp94[1]={& _tmp96};Cyc_Tcutil_terr(loc,({const char*_tmp95="length modifier '%s' not allowed with %%%%";
_tag_arr(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,43));}),_tag_arr(_tmp94,
sizeof(void*),1));}});return 0;}break;default: _LL67: return 0;}}}}}return Cyc_List_imp_rev(
_tmp13);}struct _tuple5{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char
f4;int f5;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _tagged_arr s,
int i){unsigned int _tmp97=Cyc_strlen((struct _tagged_arr)s);if(i < 0?1: i >= _tmp97)
return 0;{int _tmp98=0;char _tmp99=*((const char*)_check_unknown_subscript(s,sizeof(
char),i));if(_tmp99 == '*'){_tmp98=1;i ++;if(i >= _tmp97)return 0;}{struct Cyc_List_List*
_tmp9A=0;for(0;i < _tmp97;i ++){_tmp99=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));if(isdigit((int)_tmp99))_tmp9A=({struct Cyc_List_List*_tmp9B=
_cycalloc(sizeof(*_tmp9B));_tmp9B->hd=(void*)((int)_tmp99);_tmp9B->tl=_tmp9A;
_tmp9B;});else{break;}}if(i >= _tmp97)return 0;_tmp9A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9A);{struct Cyc_List_List*_tmp9C=0;
_tmp99=*((const char*)_check_unknown_subscript(s,sizeof(char),i));switch(_tmp99){
case 'h': _LL69: _tmp9C=({struct Cyc_List_List*_tmp9D=_cycalloc(sizeof(*_tmp9D));
_tmp9D->hd=(void*)((int)_tmp99);_tmp9D->tl=_tmp9C;_tmp9D;});i ++;if(i >= _tmp97)
return 0;_tmp99=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(
_tmp99 == 'h'){_tmp9C=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));
_tmp9E->hd=(void*)((int)_tmp99);_tmp9E->tl=_tmp9C;_tmp9E;});i ++;}break;case 'l':
_LL6A: _tmp9C=({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->hd=(
void*)((int)_tmp99);_tmp9F->tl=_tmp9C;_tmp9F;});i ++;if(i >= _tmp97)return 0;_tmp99=*((
const char*)_check_unknown_subscript(s,sizeof(char),i));if(_tmp99 == 'l'){_tmp9C=({
struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->hd=(void*)((int)
_tmp99);_tmpA0->tl=_tmp9C;_tmpA0;});i ++;}break;case 'j': _LL6B: goto _LL6C;case 'z':
_LL6C: goto _LL6D;case 't': _LL6D: goto _LL6E;case 'L': _LL6E: _tmp9C=({struct Cyc_List_List*
_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->hd=(void*)((int)_tmp99);_tmpA1->tl=
_tmp9C;_tmpA1;});i ++;break;default: _LL6F: break;}if(i >= _tmp97)return 0;_tmp9C=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9C);_tmp99=*((
const char*)_check_unknown_subscript(s,sizeof(char),i));switch(_tmp99){case 'd':
_LL71: goto _LL72;case 'i': _LL72: goto _LL73;case 'o': _LL73: goto _LL74;case 'u': _LL74:
goto _LL75;case 'x': _LL75: goto _LL76;case 'X': _LL76: goto _LL77;case 'f': _LL77: goto
_LL78;case 'F': _LL78: goto _LL79;case 'e': _LL79: goto _LL7A;case 'E': _LL7A: goto _LL7B;
case 'g': _LL7B: goto _LL7C;case 'G': _LL7C: goto _LL7D;case 'a': _LL7D: goto _LL7E;case 'A':
_LL7E: goto _LL7F;case 'c': _LL7F: goto _LL80;case 's': _LL80: goto _LL81;case 'p': _LL81:
goto _LL82;case 'n': _LL82: goto _LL83;case '%': _LL83: break;default: _LL84: return 0;}
return({struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->v=({struct
_tuple5*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->f1=_tmp98;_tmpA3->f2=_tmp9A;
_tmpA3->f3=_tmp9C;_tmpA3->f4=_tmp99;_tmpA3->f5=i + 1;_tmpA3;});_tmpA2;});}}}}
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*te,struct
_tagged_arr s,struct Cyc_Position_Segment*loc){unsigned int _tmpA4=Cyc_strlen((
struct _tagged_arr)s);struct Cyc_List_List*_tmpA5=0;int i;for(i=0;i < _tmpA4;i ++){
if(*((const char*)_check_unknown_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmpA6=Cyc_Formatstr_parse_inputformat(s,i + 1);if(_tmpA6 == 0){({
void*_tmpA7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpA8="bad format string";
_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,18));}),_tag_arr(_tmpA7,
sizeof(void*),0));});return 0;}{struct _tuple5 _tmpAA;int _tmpAB;struct Cyc_List_List*
_tmpAC;struct Cyc_List_List*_tmpAD;char _tmpAE;int _tmpAF;struct _tuple5*_tmpA9=(
struct _tuple5*)_tmpA6->v;_tmpAA=*_tmpA9;_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;_tmpAD=
_tmpAA.f3;_tmpAE=_tmpAA.f4;_tmpAF=_tmpAA.f5;i=_tmpAF - 1;{struct Cyc_List_List*
_tmpB0=_tmpAD;struct Cyc_List_List _tmpB1;int _tmpB2;struct Cyc_List_List*_tmpB3;
_LL87: if(_tmpB0 == 0)goto _LL89;_tmpB1=*_tmpB0;_tmpB2=(int)_tmpB1.hd;_tmpB3=_tmpB1.tl;
if(_tmpB3 != 0)goto _LL89;if(!((_tmpB2 == 'j'?1: _tmpB2 == 'z')?1: _tmpB2 == 't'))goto
_LL89;_LL88:({struct Cyc_Int_pa_struct _tmpB6;_tmpB6.tag=1;_tmpB6.f1=(unsigned int)
_tmpB2;{void*_tmpB4[1]={& _tmpB6};Cyc_Tcutil_terr(loc,({const char*_tmpB5="length modifier '%c' is not supported";
_tag_arr(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,38));}),_tag_arr(_tmpB4,
sizeof(void*),1));}});return 0;_LL89:;_LL8A: goto _LL86;_LL86:;}if(_tmpAB)continue;{
void*t;switch(_tmpAE){case 'd': _LL8B: goto _LL8C;case 'i': _LL8C: {struct Cyc_Core_Opt*
_tmpB7=({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpCA;});{struct Cyc_List_List*_tmpB8=_tmpAD;struct Cyc_List_List _tmpB9;int
_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_List_List _tmpBC;int _tmpBD;struct Cyc_List_List*
_tmpBE;struct Cyc_List_List _tmpBF;int _tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_List_List
_tmpC2;int _tmpC3;struct Cyc_List_List*_tmpC4;_LL8F: if(_tmpB8 != 0)goto _LL91;_LL90:
t=Cyc_Absyn_sint_typ;goto _LL8E;_LL91: if(_tmpB8 == 0)goto _LL93;_tmpB9=*_tmpB8;
_tmpBA=(int)_tmpB9.hd;_tmpBB=_tmpB9.tl;if(_tmpBB != 0)goto _LL93;if(!(_tmpBA == 'l'))
goto _LL93;_LL92: t=Cyc_Absyn_slong_typ;goto _LL8E;_LL93: if(_tmpB8 == 0)goto _LL95;
_tmpBC=*_tmpB8;_tmpBD=(int)_tmpBC.hd;_tmpBE=_tmpBC.tl;if(_tmpBE != 0)goto _LL95;
if(!(_tmpBD == 'h'))goto _LL95;_LL94: t=Cyc_Absyn_sshort_typ;goto _LL8E;_LL95: if(
_tmpB8 == 0)goto _LL97;_tmpBF=*_tmpB8;_tmpC0=(int)_tmpBF.hd;_tmpC1=_tmpBF.tl;if(
_tmpC1 == 0)goto _LL97;_tmpC2=*_tmpC1;_tmpC3=(int)_tmpC2.hd;_tmpC4=_tmpC2.tl;if(
_tmpC4 != 0)goto _LL97;if(!(_tmpC0 == 'h'?_tmpC3 == 'h': 0))goto _LL97;_LL96: t=Cyc_Absyn_schar_typ;
goto _LL8E;_LL97:;_LL98:({struct Cyc_Int_pa_struct _tmpC8;_tmpC8.tag=1;_tmpC8.f1=(
unsigned int)((int)_tmpAE);{struct Cyc_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmpAD));{void*_tmpC5[2]={&
_tmpC7,& _tmpC8};Cyc_Tcutil_terr(loc,({const char*_tmpC6="length modifier '%s' is not allowed with %%%c";
_tag_arr(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,46));}),_tag_arr(_tmpC5,
sizeof(void*),2));}}});return 0;_LL8E:;}t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpB7),Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->hd=(void*)
t;_tmpC9->tl=_tmpA5;_tmpC9;});break;}case 'u': _LL8D: goto _LL99;case 'o': _LL99: goto
_LL9A;case 'x': _LL9A: goto _LL9B;case 'X': _LL9B: {struct Cyc_Core_Opt*_tmpCB=({struct
Cyc_Core_Opt*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpDE;});{struct Cyc_List_List*_tmpCC=_tmpAD;struct Cyc_List_List _tmpCD;int
_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List _tmpD0;int _tmpD1;struct Cyc_List_List*
_tmpD2;struct Cyc_List_List _tmpD3;int _tmpD4;struct Cyc_List_List*_tmpD5;struct Cyc_List_List
_tmpD6;int _tmpD7;struct Cyc_List_List*_tmpD8;_LL9E: if(_tmpCC != 0)goto _LLA0;_LL9F:
t=Cyc_Absyn_uint_typ;goto _LL9D;_LLA0: if(_tmpCC == 0)goto _LLA2;_tmpCD=*_tmpCC;
_tmpCE=(int)_tmpCD.hd;_tmpCF=_tmpCD.tl;if(_tmpCF != 0)goto _LLA2;if(!(_tmpCE == 'l'))
goto _LLA2;_LLA1: t=Cyc_Absyn_ulong_typ;goto _LL9D;_LLA2: if(_tmpCC == 0)goto _LLA4;
_tmpD0=*_tmpCC;_tmpD1=(int)_tmpD0.hd;_tmpD2=_tmpD0.tl;if(_tmpD2 != 0)goto _LLA4;
if(!(_tmpD1 == 'h'))goto _LLA4;_LLA3: t=Cyc_Absyn_ushort_typ;goto _LL9D;_LLA4: if(
_tmpCC == 0)goto _LLA6;_tmpD3=*_tmpCC;_tmpD4=(int)_tmpD3.hd;_tmpD5=_tmpD3.tl;if(
_tmpD5 == 0)goto _LLA6;_tmpD6=*_tmpD5;_tmpD7=(int)_tmpD6.hd;_tmpD8=_tmpD6.tl;if(
_tmpD8 != 0)goto _LLA6;if(!(_tmpD4 == 'h'?_tmpD7 == 'h': 0))goto _LLA6;_LLA5: t=Cyc_Absyn_uchar_typ;
goto _LL9D;_LLA6:;_LLA7:({struct Cyc_Int_pa_struct _tmpDC;_tmpDC.tag=1;_tmpDC.f1=(
unsigned int)((int)_tmpAE);{struct Cyc_String_pa_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmpAD));{void*_tmpD9[2]={&
_tmpDB,& _tmpDC};Cyc_Tcutil_terr(loc,({const char*_tmpDA="length modifier '%s' is not allowed with %%%c";
_tag_arr(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,46));}),_tag_arr(_tmpD9,
sizeof(void*),2));}}});return 0;_LL9D:;}t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpCB),Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=(void*)
t;_tmpDD->tl=_tmpA5;_tmpDD;});break;}case 'f': _LL9C: goto _LLA8;case 'F': _LLA8: goto
_LLA9;case 'e': _LLA9: goto _LLAA;case 'E': _LLAA: goto _LLAB;case 'g': _LLAB: goto _LLAC;
case 'G': _LLAC: goto _LLAD;case 'a': _LLAD: goto _LLAE;case 'A': _LLAE: {struct Cyc_Core_Opt*
_tmpDF=({struct Cyc_Core_Opt*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpE9;});{struct Cyc_List_List*_tmpE0=_tmpAD;struct Cyc_List_List _tmpE1;int
_tmpE2;struct Cyc_List_List*_tmpE3;_LLB1: if(_tmpE0 != 0)goto _LLB3;_LLB2: t=Cyc_Absyn_float_typ;
goto _LLB0;_LLB3: if(_tmpE0 == 0)goto _LLB5;_tmpE1=*_tmpE0;_tmpE2=(int)_tmpE1.hd;
_tmpE3=_tmpE1.tl;if(_tmpE3 != 0)goto _LLB5;if(!(_tmpE2 == 'l'))goto _LLB5;_LLB4: t=
Cyc_Absyn_double_typ(0);goto _LLB0;_LLB5:;_LLB6:({struct Cyc_Int_pa_struct _tmpE7;
_tmpE7.tag=1;_tmpE7.f1=(unsigned int)((int)_tmpAE);{struct Cyc_String_pa_struct
_tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmpAD));{void*_tmpE4[2]={& _tmpE6,& _tmpE7};Cyc_Tcutil_terr(loc,({const char*
_tmpE5="length modifier '%s' is not allowed with %%%c";_tag_arr(_tmpE5,sizeof(
char),_get_zero_arr_size(_tmpE5,46));}),_tag_arr(_tmpE4,sizeof(void*),2));}}});
return 0;_LLB0:;}t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmpDF),Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmpA5=({
struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(void*)t;_tmpE8->tl=
_tmpA5;_tmpE8;});break;}case 'c': _LLAF: {struct Cyc_Core_Opt*_tmpEA=({struct Cyc_Core_Opt*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpEC;});_tmpA5=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(
void*)Cyc_Absyn_tagged_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmpEA),Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);_tmpEB->tl=
_tmpA5;_tmpEB;});break;}case 's': _LLB7: {struct Cyc_Core_Opt*_tmpED=({struct Cyc_Core_Opt*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpEF;});_tmpA5=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=(
void*)Cyc_Absyn_string_typ(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmpED));_tmpEE->tl=_tmpA5;_tmpEE;});break;}case '[': _LLB8: goto _LLB9;case 'p':
_LLB9:({struct Cyc_Int_pa_struct _tmpF2;_tmpF2.tag=1;_tmpF2.f1=(unsigned int)((int)
_tmpAE);{void*_tmpF0[1]={& _tmpF2};Cyc_Tcutil_terr(loc,({const char*_tmpF1="%%%c is not supported";
_tag_arr(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,22));}),_tag_arr(_tmpF0,
sizeof(void*),1));}});return 0;case 'n': _LLBA: {struct Cyc_Core_Opt*_tmpF3=({struct
Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp106;});{struct Cyc_List_List*_tmpF4=_tmpAD;struct Cyc_List_List _tmpF5;int
_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List _tmpF8;int _tmpF9;struct Cyc_List_List*
_tmpFA;struct Cyc_List_List _tmpFB;int _tmpFC;struct Cyc_List_List*_tmpFD;struct Cyc_List_List
_tmpFE;int _tmpFF;struct Cyc_List_List*_tmp100;_LLBD: if(_tmpF4 != 0)goto _LLBF;_LLBE:
t=Cyc_Absyn_sint_typ;goto _LLBC;_LLBF: if(_tmpF4 == 0)goto _LLC1;_tmpF5=*_tmpF4;
_tmpF6=(int)_tmpF5.hd;_tmpF7=_tmpF5.tl;if(_tmpF7 != 0)goto _LLC1;if(!(_tmpF6 == 'l'))
goto _LLC1;_LLC0: t=Cyc_Absyn_ulong_typ;goto _LLBC;_LLC1: if(_tmpF4 == 0)goto _LLC3;
_tmpF8=*_tmpF4;_tmpF9=(int)_tmpF8.hd;_tmpFA=_tmpF8.tl;if(_tmpFA != 0)goto _LLC3;
if(!(_tmpF9 == 'h'))goto _LLC3;_LLC2: t=Cyc_Absyn_sshort_typ;goto _LLBC;_LLC3: if(
_tmpF4 == 0)goto _LLC5;_tmpFB=*_tmpF4;_tmpFC=(int)_tmpFB.hd;_tmpFD=_tmpFB.tl;if(
_tmpFD == 0)goto _LLC5;_tmpFE=*_tmpFD;_tmpFF=(int)_tmpFE.hd;_tmp100=_tmpFE.tl;if(
_tmp100 != 0)goto _LLC5;if(!(_tmpFC == 'h'?_tmpFF == 'h': 0))goto _LLC5;_LLC4: t=Cyc_Absyn_schar_typ;
goto _LLBC;_LLC5:;_LLC6:({struct Cyc_Int_pa_struct _tmp104;_tmp104.tag=1;_tmp104.f1=(
unsigned int)((int)_tmpAE);{struct Cyc_String_pa_struct _tmp103;_tmp103.tag=0;
_tmp103.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(_tmpAD));{void*
_tmp101[2]={& _tmp103,& _tmp104};Cyc_Tcutil_terr(loc,({const char*_tmp102="length modifier '%s' is not allowed with %%%c";
_tag_arr(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,46));}),_tag_arr(
_tmp101,sizeof(void*),2));}}});return 0;_LLBC:;}t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpF3),Cyc_Absyn_empty_tqual(),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=(
void*)t;_tmp105->tl=_tmpA5;_tmp105;});break;}case '%': _LLBB: if(_tmpAB){({void*
_tmp107[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp108="Assignment suppression (*) is not allowed with %%%%";
_tag_arr(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,52));}),_tag_arr(
_tmp107,sizeof(void*),0));});return 0;}if(_tmpAC != 0){({struct Cyc_String_pa_struct
_tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmpAC));{void*_tmp109[1]={& _tmp10B};Cyc_Tcutil_terr(loc,({const char*_tmp10A="width '%s' not allowed with %%%%";
_tag_arr(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,33));}),_tag_arr(
_tmp109,sizeof(void*),1));}});return 0;}if(_tmpAD != 0){({struct Cyc_String_pa_struct
_tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_implode(
_tmpAD));{void*_tmp10C[1]={& _tmp10E};Cyc_Tcutil_terr(loc,({const char*_tmp10D="length modifier '%s' not allowed with %%%%";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,43));}),_tag_arr(
_tmp10C,sizeof(void*),1));}});return 0;}break;default: _LLC7: return 0;}}}}}return
Cyc_List_imp_rev(_tmpA5);}
