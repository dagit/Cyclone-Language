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
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct
Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownTunionInfo{struct _tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1*tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
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
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_unknown_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;void*Cyc_Absyn_atb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
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
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
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
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tagged_arr msg,struct _tagged_arr ap){({
void*_tmp0[0]={};Cyc_Tcutil_terr(loc,(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,
msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1;}));}
static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _tuple1*q){struct _handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;
if(setjmp(_tmp2.handler))_tmp4=1;if(!_tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(
te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*
_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*
_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;
_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;_LL2:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({
struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;
_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct
Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=31;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=32;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=30;_tmp14.f1=0;
_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9: if(*((int*)
_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(q));{void*_tmp15[1]={&
_tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,31));}),_tag_arr(_tmp15,
sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Var_e_struct _tmp19;
_tmp19.tag=1;_tmp19.f1=q;_tmp19.f2=(void*)((void*)0);_tmp19;});_tmp18;})));goto
_LL0;_LL0:;};_pop_handler();}else{void*_tmp3=(void*)_exn_thrown;void*_tmp1B=
_tmp3;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({
struct Cyc_Absyn_Var_e_struct _tmp1D;_tmp1D.tag=1;_tmp1D.f1=q;_tmp1D.f2=(void*)((
void*)0);_tmp1D;});_tmp1C;})));goto _LLB;_LLE:;_LLF:(void)_throw(_tmp1B);_LLB:;}}}
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){return({struct _tuple8*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=0;_tmp1E->f2=e;_tmp1E;});}static
void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(void*)e1->r;struct
_tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{void*_tmp24=Cyc_Tcenv_lookup_ordinary(
te,e1->loc,_tmp20);void*_tmp25;struct Cyc_Absyn_Tuniondecl*_tmp26;struct Cyc_Absyn_Tunionfield*
_tmp27;struct Cyc_Absyn_Aggrdecl*_tmp28;_LL16: if(*((int*)_tmp24)!= 0)goto _LL18;
_tmp25=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp24)->f1;_LL17:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp29=_cycalloc(sizeof(*_tmp29));
_tmp29[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2A;_tmp2A.tag=9;_tmp2A.f1=e1;
_tmp2A.f2=es;_tmp2A.f3=0;_tmp2A;});_tmp29;})));_npop_handler(0);return;_LL18: if(*((
int*)_tmp24)!= 2)goto _LL1A;_tmp26=((struct Cyc_Tcenv_TunionRes_struct*)_tmp24)->f1;
_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp24)->f2;_LL19: if(_tmp27->typs == 0)({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp27->name));{void*_tmp2B[1]={&
_tmp2D};Cyc_Tcutil_terr(e->loc,({const char*_tmp2C="%s is a constant, not a function";
_tag_arr(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,33));}),_tag_arr(_tmp2B,
sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp2F;_tmp2F.tag=30;_tmp2F.f1=es;_tmp2F.f2=_tmp26;_tmp2F.f3=_tmp27;_tmp2F;});
_tmp2E;})));_npop_handler(0);return;_LL1A: if(*((int*)_tmp24)!= 1)goto _LL1C;
_tmp28=((struct Cyc_Tcenv_AggrRes_struct*)_tmp24)->f1;_LL1B: {struct Cyc_List_List*
_tmp30=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp32;_tmp32.tag=28;_tmp32.f1=_tmp28->name;
_tmp32.f2=0;_tmp32.f3=_tmp30;_tmp32.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp28;_tmp32;});
_tmp31;})));_npop_handler(0);return;}_LL1C: if(*((int*)_tmp24)!= 4)goto _LL1E;
_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp24)!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct
_tmp35;_tmp35.tag=0;_tmp35.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp20));{void*_tmp33[1]={& _tmp35};Cyc_Tcutil_terr(e->loc,({const char*_tmp34="%s is an enum constructor, not a function";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,42));}),_tag_arr(_tmp33,
sizeof(void*),1));}});_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*
_tmp22=(void*)_exn_thrown;void*_tmp37=_tmp22;_LL21: if(_tmp37 != Cyc_Dict_Absent)
goto _LL23;_LL22:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp39;_tmp39.tag=9;_tmp39.f1=e1;_tmp39.f2=es;_tmp39.f3=0;_tmp39;});_tmp38;})));
return;_LL23:;_LL24:(void)_throw(_tmp37);_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=9;_tmp3B.f1=e1;
_tmp3B.f2=es;_tmp3B.f3=0;_tmp3B;});_tmp3A;})));return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3D;_tmp3D.tag=26;_tmp3D.f1=des;_tmp3D;});_tmp3C;})));return;}{void*t=*topt;
void*_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3F;void*_tmp40;
struct Cyc_Absyn_ArrayInfo _tmp41;void*_tmp42;struct Cyc_Absyn_Tqual _tmp43;_LL26:
if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 10)goto _LL28;_tmp3F=((struct Cyc_Absyn_AggrType_struct*)
_tmp3E)->f1;_tmp40=(void*)_tmp3F.aggr_info;_LL27:{void*_tmp44=_tmp40;struct Cyc_Absyn_Aggrdecl**
_tmp45;struct Cyc_Absyn_Aggrdecl*_tmp46;_LL2F: if(*((int*)_tmp44)!= 0)goto _LL31;
_LL30:({void*_tmp47[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp48="struct type not properly set";_tag_arr(_tmp48,sizeof(char),
_get_zero_arr_size(_tmp48,29));}),_tag_arr(_tmp47,sizeof(void*),0));});_LL31: if(*((
int*)_tmp44)!= 1)goto _LL2E;_tmp45=((struct Cyc_Absyn_KnownAggr_struct*)_tmp44)->f1;
_tmp46=*_tmp45;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp4A;_tmp4A.tag=28;_tmp4A.f1=_tmp46->name;_tmp4A.f2=0;_tmp4A.f3=des;_tmp4A.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp46;_tmp4A;});_tmp49;})));_LL2E:;}goto _LL25;_LL28:
if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 7)goto _LL2A;_tmp41=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E)->f1;_tmp42=(void*)_tmp41.elt_type;_tmp43=_tmp41.tq;_LL29:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));
_tmp4B[0]=({struct Cyc_Absyn_Array_e_struct _tmp4C;_tmp4C.tag=26;_tmp4C.f1=des;
_tmp4C;});_tmp4B;})));goto _LL25;_LL2A: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 
11)goto _LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp4E;_tmp4E.tag=29;_tmp4E.f1=(void*)t;_tmp4E.f2=des;_tmp4E;});_tmp4D;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Array_e_struct
_tmp50;_tmp50.tag=26;_tmp50.f1=des;_tmp50;});_tmp4F;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)((struct _tagged_arr)msg_part);{
void*_tmp51[2]={& _tmp53,& _tmp54};Cyc_Tcutil_terr(e->loc,({const char*_tmp52="test of %s has type %s instead of integral or * type";
_tag_arr(_tmp52,sizeof(char),_get_zero_arr_size(_tmp52,53));}),_tag_arr(_tmp51,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{
void*_tmp55=c;void*_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*
_tmp5B;int _tmp5C;struct _tagged_arr _tmp5D;_LL34: if(_tmp55 <= (void*)1?1:*((int*)
_tmp55)!= 0)goto _LL36;_tmp56=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp55)->f1;
if((int)_tmp56 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;_LL36: if(
_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 0)goto _LL38;_tmp57=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp55)->f1;if((int)_tmp57 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;
_LL38: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 0)goto _LL3A;_tmp58=(void*)((
struct Cyc_Absyn_Char_c_struct*)_tmp55)->f1;if((int)_tmp58 != 2)goto _LL3A;_LL39: t=
Cyc_Absyn_char_typ;goto _LL33;_LL3A: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 1)
goto _LL3C;_tmp59=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp55)->f1;_LL3B: t=
_tmp59 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if(
_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 3)goto _LL3E;_tmp5A=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp55)->f1;_LL3D: t=_tmp5A == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL33;_LL3E: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 4)goto _LL40;_LL3F: t=Cyc_Absyn_float_typ;
goto _LL33;_LL40: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 2)goto _LL42;_tmp5B=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp55)->f1;_tmp5C=((struct Cyc_Absyn_Int_c_struct*)
_tmp55)->f2;_LL41: if(topt == 0)t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
else{void*_tmp5E=Cyc_Tcutil_compress(*topt);void*_tmp5F;void*_tmp60;void*_tmp61;
void*_tmp62;void*_tmp63;void*_tmp64;_LL47: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)
!= 5)goto _LL49;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;
_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)_tmp60 != 0)
goto _LL49;_LL48:{void*_tmp65=_tmp5F;_LL54: if((int)_tmp65 != 1)goto _LL56;_LL55: t=
Cyc_Absyn_uchar_typ;goto _LL53;_LL56: if((int)_tmp65 != 0)goto _LL58;_LL57: t=Cyc_Absyn_schar_typ;
goto _LL53;_LL58: if((int)_tmp65 != 2)goto _LL53;_LL59: t=Cyc_Absyn_char_typ;goto
_LL53;_LL53:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Absyn_Const_e_struct
_tmp67;_tmp67.tag=0;_tmp67.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=({struct Cyc_Absyn_Char_c_struct
_tmp69;_tmp69.tag=0;_tmp69.f1=(void*)_tmp5F;_tmp69.f2=(char)_tmp5C;_tmp69;});
_tmp68;}));_tmp67;});_tmp66;})));goto _LL46;_LL49: if(_tmp5E <= (void*)3?1:*((int*)
_tmp5E)!= 5)goto _LL4B;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;
_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)_tmp62 != 1)
goto _LL4B;_LL4A: t=_tmp61 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6A=_cycalloc(
sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Absyn_Const_e_struct _tmp6B;_tmp6B.tag=0;
_tmp6B.f1=(void*)((void*)({struct Cyc_Absyn_Short_c_struct*_tmp6C=_cycalloc(
sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_Short_c_struct _tmp6D;_tmp6D.tag=1;
_tmp6D.f1=(void*)_tmp61;_tmp6D.f2=(short)_tmp5C;_tmp6D;});_tmp6C;}));_tmp6B;});
_tmp6A;})));goto _LL46;_LL4B: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)!= 5)goto _LL4D;
_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;_LL4C: t=_tmp63 == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(_tmp5E <= (void*)
3?1:*((int*)_tmp5E)!= 4)goto _LL4F;if(!(_tmp5C == 0))goto _LL4F;_LL4E:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));
_tmp6E[0]=({struct Cyc_Absyn_Const_e_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(void*)((
void*)0);_tmp6F;});_tmp6E;})));{struct Cyc_List_List*_tmp70=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp71=_cycalloc(sizeof(*
_tmp71));_tmp71[0]=({struct Cyc_Absyn_PointerType_struct _tmp72;_tmp72.tag=4;
_tmp72.f1=({struct Cyc_Absyn_PtrInfo _tmp73;_tmp73.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76->v=_tmp70;_tmp76;}));_tmp73.elt_tq=Cyc_Absyn_empty_tqual();
_tmp73.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp74;_tmp74.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp75=_cycalloc(sizeof(*
_tmp75));_tmp75->v=_tmp70;_tmp75;}));_tmp74.nullable=Cyc_Absyn_true_conref;
_tmp74.bounds=Cyc_Absyn_empty_conref();_tmp74.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp74;});_tmp73;});_tmp72;});_tmp71;});goto _LL46;}
_LL4F: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)!= 17)goto _LL51;_tmp64=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp5E)->f1;_LL50: {struct Cyc_Absyn_TypeInt_struct*
_tmp77=({struct Cyc_Absyn_TypeInt_struct*_tmp7E=_cycalloc_atomic(sizeof(*_tmp7E));
_tmp7E[0]=({struct Cyc_Absyn_TypeInt_struct _tmp7F;_tmp7F.tag=18;_tmp7F.f1=_tmp5C;
_tmp7F;});_tmp7E;});if(!Cyc_Tcutil_unify(_tmp64,(void*)_tmp77)){({struct Cyc_String_pa_struct
_tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp77));{struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp64));{void*_tmp78[2]={&
_tmp7A,& _tmp7B};Cyc_Tcutil_terr(loc,({const char*_tmp79="expecting %s but found %s";
_tag_arr(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,26));}),_tag_arr(_tmp78,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_TagType_struct
_tmp7D;_tmp7D.tag=17;_tmp7D.f1=(void*)((void*)_tmp77);_tmp7D;});_tmp7C;});goto
_LL46;}_LL51:;_LL52: t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}goto _LL33;_LL42: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 5)
goto _LL44;_tmp5D=((struct Cyc_Absyn_String_c_struct*)_tmp55)->f1;_LL43: {int len=(
int)_get_arr_size(_tmp5D,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({
struct Cyc_Absyn_Int_c_struct _tmp8C;_tmp8C.tag=2;_tmp8C.f1=(void*)((void*)1);
_tmp8C.f2=len;_tmp8C;});_tmp8B;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp80=
_cycalloc(sizeof(*_tmp80));_tmp80->v=(void*)Cyc_Absyn_uint_typ;_tmp80;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp82;_tmp82.tag=0;_tmp82.f1=elen;_tmp82;});_tmp81;}),Cyc_Absyn_true_conref);
if(topt != 0){void*_tmp83=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo
_tmp84;struct Cyc_Absyn_Tqual _tmp85;_LL5B: if(_tmp83 <= (void*)3?1:*((int*)_tmp83)
!= 7)goto _LL5D;_tmp84=((struct Cyc_Absyn_ArrayType_struct*)_tmp83)->f1;_tmp85=
_tmp84.tq;_LL5C: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp85,(struct Cyc_Absyn_Exp*)
elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)());_LL5D: if(_tmp83 <= (
void*)3?1:*((int*)_tmp83)!= 4)goto _LL5F;_LL5E: if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86->v=(void*)t;_tmp86;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}
else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp87;})),Cyc_Absyn_const_tqual(),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp89;_tmp89.tag=0;_tmp89.f1=elen;_tmp89;});
_tmp88;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A->v=(void*)t;_tmp8A;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}}
goto _LL5A;_LL5F:;_LL60: goto _LL5A;_LL5A:;}return t;}_LL44: if((int)_tmp55 != 0)goto
_LL33;_LL45: {struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({
struct Cyc_Absyn_PointerType_struct _tmp8F;_tmp8F.tag=4;_tmp8F.f1=({struct Cyc_Absyn_PtrInfo
_tmp90;_tmp90.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({
struct Cyc_Core_Opt*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->v=_tmp8D;_tmp93;}));
_tmp90.elt_tq=Cyc_Absyn_empty_tqual();_tmp90.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp91;_tmp91.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->v=_tmp8D;_tmp92;}));
_tmp91.nullable=Cyc_Absyn_true_conref;_tmp91.bounds=Cyc_Absyn_empty_conref();
_tmp91.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp91;});
_tmp90;});_tmp8F;});_tmp8E;});goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){
void*_tmp94=b;struct Cyc_Absyn_Vardecl*_tmp95;struct Cyc_Absyn_Fndecl*_tmp96;
struct Cyc_Absyn_Vardecl*_tmp97;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Vardecl*
_tmp99;_LL62: if((int)_tmp94 != 0)goto _LL64;_LL63: return({struct Cyc_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
q));{void*_tmp9A[1]={& _tmp9C};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp9B="undeclared identifier: %s";
_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,26));}),_tag_arr(_tmp9A,
sizeof(void*),1));}});_LL64: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 0)goto _LL66;
_tmp95=((struct Cyc_Absyn_Global_b_struct*)_tmp94)->f1;_LL65:*q=*_tmp95->name;
return(void*)_tmp95->type;_LL66: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 1)goto
_LL68;_tmp96=((struct Cyc_Absyn_Funname_b_struct*)_tmp94)->f1;_LL67:*q=*_tmp96->name;
return Cyc_Tcutil_fndecl2typ(_tmp96);_LL68: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)
!= 4)goto _LL6A;_tmp97=((struct Cyc_Absyn_Pat_b_struct*)_tmp94)->f1;_LL69: _tmp98=
_tmp97;goto _LL6B;_LL6A: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 3)goto _LL6C;
_tmp98=((struct Cyc_Absyn_Local_b_struct*)_tmp94)->f1;_LL6B: _tmp99=_tmp98;goto
_LL6D;_LL6C: if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 2)goto _LL61;_tmp99=((struct
Cyc_Absyn_Param_b_struct*)_tmp94)->f1;_LL6D:(*q).f1=(void*)0;return(void*)_tmp99->type;
_LL61:;}static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{
void*_tmp9D=(void*)fmt->r;void*_tmp9E;struct _tagged_arr _tmp9F;struct Cyc_Absyn_Exp*
_tmpA0;struct Cyc_Absyn_Exp _tmpA1;void*_tmpA2;void*_tmpA3;struct _tagged_arr _tmpA4;
_LL6F: if(*((int*)_tmp9D)!= 0)goto _LL71;_tmp9E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9D)->f1;if(_tmp9E <= (void*)1?1:*((int*)_tmp9E)!= 5)goto _LL71;_tmp9F=((struct
Cyc_Absyn_String_c_struct*)_tmp9E)->f1;_LL70: _tmpA4=_tmp9F;goto _LL72;_LL71: if(*((
int*)_tmp9D)!= 13)goto _LL73;_tmpA0=((struct Cyc_Absyn_Cast_e_struct*)_tmp9D)->f2;
_tmpA1=*_tmpA0;_tmpA2=(void*)_tmpA1.r;if(*((int*)_tmpA2)!= 0)goto _LL73;_tmpA3=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpA2)->f1;if(_tmpA3 <= (void*)1?1:*((
int*)_tmpA3)!= 5)goto _LL73;_tmpA4=((struct Cyc_Absyn_String_c_struct*)_tmpA3)->f1;
_LL72: desc_types=type_getter(te,(struct _tagged_arr)_tmpA4,fmt->loc);goto _LL6E;
_LL73:;_LL74: if(opt_args != 0){struct Cyc_List_List*_tmpA5=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpA5->hd);}}return;_LL6E:;}if(opt_args != 0){struct Cyc_List_List*_tmpA6=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0?_tmpA6 != 0: 0;(desc_types=
desc_types->tl,_tmpA6=_tmpA6->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmpA6->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmpA7[2]={&
_tmpA9,& _tmpAA};Cyc_Tcutil_terr(e->loc,({const char*_tmpA8="descriptor has type \n%s\n but argument has type \n%s";
_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,51));}),_tag_arr(_tmpA7,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(desc_types != 0)({void*
_tmpAB[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpAC="too few arguments";
_tag_arr(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,18));}),_tag_arr(_tmpAB,
sizeof(void*),0));});if(_tmpA6 != 0)({void*_tmpAD[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA6->hd)->loc,({const char*_tmpAE="too many arguments";_tag_arr(
_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,19));}),_tag_arr(_tmpAD,sizeof(
void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpAF=p;_LL76: if((
int)_tmpAF != 0)goto _LL78;_LL77: goto _LL79;_LL78: if((int)_tmpAF != 2)goto _LL7A;
_LL79: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_String_pa_struct _tmpB2;_tmpB2.tag=
0;_tmpB2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmpB0[1]={& _tmpB2};Cyc_Tcutil_terr(loc,({const char*_tmpB1="expecting arithmetic type but found %s";
_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,39));}),_tag_arr(_tmpB0,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7A: if((int)_tmpAF != 11)goto _LL7C;_LL7B: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=
0;_tmpB5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmpB3[1]={& _tmpB5};Cyc_Tcutil_terr(loc,({const char*_tmpB4="expecting integral or * type but found %s";
_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,42));}),_tag_arr(_tmpB3,
sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7C: if((int)_tmpAF != 12)goto
_LL7E;_LL7D: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct _tmpB8;
_tmpB8.tag=0;_tmpB8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpB6[1]={& _tmpB8};Cyc_Tcutil_terr(loc,({const char*_tmpB7="expecting integral type but found %s";
_tag_arr(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,37));}),_tag_arr(_tmpB6,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7E: if((int)_tmpAF != 19)goto _LL80;_LL7F:{void*_tmpB9=t;struct Cyc_Absyn_PtrInfo
_tmpBA;struct Cyc_Absyn_PtrAtts _tmpBB;struct Cyc_Absyn_Conref*_tmpBC;_LL83: if(
_tmpB9 <= (void*)3?1:*((int*)_tmpB9)!= 7)goto _LL85;_LL84: goto _LL82;_LL85: if(
_tmpB9 <= (void*)3?1:*((int*)_tmpB9)!= 4)goto _LL87;_tmpBA=((struct Cyc_Absyn_PointerType_struct*)
_tmpB9)->f1;_tmpBB=_tmpBA.ptr_atts;_tmpBC=_tmpBB.bounds;_LL86:{void*_tmpBD=(void*)(
Cyc_Absyn_compress_conref(_tmpBC))->v;void*_tmpBE;void*_tmpBF;_LL8A: if(_tmpBD <= (
void*)1?1:*((int*)_tmpBD)!= 0)goto _LL8C;_tmpBE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBD)->f1;if((int)_tmpBE != 0)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmpBD <= (void*)
1?1:*((int*)_tmpBD)!= 0)goto _LL8E;_tmpBF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpBD)->f1;if(_tmpBF <= (void*)1?1:*((int*)_tmpBF)!= 0)goto _LL8E;_LL8D: goto _LL89;
_LL8E:;_LL8F:({void*_tmpC0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC1="can't apply size to pointer/array of abstract length";
_tag_arr(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,53));}),_tag_arr(_tmpC0,
sizeof(void*),0));});_LL89:;}goto _LL82;_LL87:;_LL88:({struct Cyc_String_pa_struct
_tmpC4;_tmpC4.tag=0;_tmpC4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpC2[1]={& _tmpC4};Cyc_Tcutil_terr(loc,({const char*_tmpC3="size requires pointer or array type, not %s";
_tag_arr(_tmpC3,sizeof(char),_get_zero_arr_size(_tmpC3,44));}),_tag_arr(_tmpC2,
sizeof(void*),1));}});_LL82:;}return Cyc_Absyn_uint_typ;_LL80:;_LL81:({void*
_tmpC5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpC6="Non-unary primop";_tag_arr(_tmpC6,sizeof(char),
_get_zero_arr_size(_tmpC6,17));}),_tag_arr(_tmpC5,sizeof(void*),0));});_LL75:;}
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmpC7[1]={& _tmpC9};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpC8="type %s cannot be used here";_tag_arr(_tmpC8,sizeof(char),
_get_zero_arr_size(_tmpC8,28));}),_tag_arr(_tmpC7,sizeof(void*),1));}});return
Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=
Cyc_Tcenv_lookup_type_vars(te);_tmpCA;}));}if(!checker(e2)){({struct Cyc_String_pa_struct
_tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpCB[1]={& _tmpCD};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpCC="type %s cannot be used here";
_tag_arr(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,28));}),_tag_arr(_tmpCB,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCE;}));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpCF=t1;struct Cyc_Absyn_PtrInfo
_tmpD0;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;
void*_tmpD4;struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct
Cyc_Absyn_Conref*_tmpD7;_LL91: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 4)goto
_LL93;_tmpD0=((struct Cyc_Absyn_PointerType_struct*)_tmpCF)->f1;_tmpD1=(void*)
_tmpD0.elt_typ;_tmpD2=_tmpD0.elt_tq;_tmpD3=_tmpD0.ptr_atts;_tmpD4=(void*)_tmpD3.rgn;
_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;_LL92: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD1),(void*)1))({void*_tmpD8[0]={};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpD9="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,50));}),_tag_arr(_tmpD8,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct
_tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{void*_tmpDA[1]={& _tmpDC};Cyc_Tcutil_terr(e2->loc,({const char*_tmpDB="expecting int but found %s";
_tag_arr(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,27));}),_tag_arr(_tmpDA,
sizeof(void*),1));}});_tmpD6=Cyc_Absyn_compress_conref(_tmpD6);{void*_tmpDD=(
void*)_tmpD6->v;void*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;void*_tmpE1;
_LL96: if(_tmpDD <= (void*)1?1:*((int*)_tmpDD)!= 0)goto _LL98;_tmpDE=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpDD)->f1;if((int)_tmpDE != 0)goto _LL98;_LL97:
return t1;_LL98: if(_tmpDD <= (void*)1?1:*((int*)_tmpDD)!= 0)goto _LL9A;_tmpDF=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpDD)->f1;if(_tmpDF <= (void*)1?1:*((int*)
_tmpDF)!= 0)goto _LL9A;_tmpE0=((struct Cyc_Absyn_Upper_b_struct*)_tmpDF)->f1;_LL99:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7))({void*
_tmpE2[0]={};Cyc_Tcutil_warn(e1->loc,({const char*_tmpE3="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,70));}),_tag_arr(_tmpE2,
sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*_tmpE4=({struct Cyc_Absyn_PointerType_struct*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_PointerType_struct
_tmpE6;_tmpE6.tag=4;_tmpE6.f1=({struct Cyc_Absyn_PtrInfo _tmpE7;_tmpE7.elt_typ=(
void*)_tmpD1;_tmpE7.elt_tq=_tmpD2;_tmpE7.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpE8;_tmpE8.rgn=(void*)_tmpD4;_tmpE8.nullable=Cyc_Absyn_true_conref;_tmpE8.bounds=
Cyc_Absyn_bounds_unknown_conref;_tmpE8.zero_term=_tmpD7;_tmpE8;});_tmpE7;});
_tmpE6;});_tmpE5;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpE4);return(void*)
_tmpE4;}_LL9A: if(_tmpDD <= (void*)1?1:*((int*)_tmpDD)!= 0)goto _LL9C;_tmpE1=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpDD)->f1;if(_tmpE1 <= (void*)1?1:*((int*)
_tmpE1)!= 1)goto _LL9C;_LL9B:({void*_tmpE9[0]={};Cyc_Tcutil_terr(e1->loc,({const
char*_tmpEA="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,64));}),_tag_arr(_tmpE9,
sizeof(void*),0));});return t1;_LL9C:;_LL9D:(void*)(_tmpD6->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(void*)((void*)0);
_tmpEC;});_tmpEB;})));return t1;_LL95:;}_LL93:;_LL94: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL90:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF0;_tmpF0.tag=
0;_tmpF0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpED[2]={& _tmpEF,&
_tmpF0};Cyc_Tcutil_terr(e1->loc,({const char*_tmpEE="pointer arithmetic on values of different types (%s != %s)";
_tag_arr(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,59));}),_tag_arr(_tmpED,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}
else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpF1[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF2="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,50));}),_tag_arr(_tmpF1,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmpF3[2]={& _tmpF5,& _tmpF6};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpF4="expecting either %s or int but found %s";
_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,40));}),_tag_arr(_tmpF3,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(
t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ);return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num?e2_is_num: 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2)?
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({
struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpF7;}))): 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,
loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1);return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2);
return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e1)?1: Cyc_Tcutil_zero_to_null(
te,t1,e2))return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmpF9=({struct _tuple0
_tmpF8;_tmpF8.f1=Cyc_Tcutil_compress(t1);_tmpF8.f2=Cyc_Tcutil_compress(t2);
_tmpF8;});void*_tmpFA;struct Cyc_Absyn_PtrInfo _tmpFB;void*_tmpFC;void*_tmpFD;
struct Cyc_Absyn_PtrInfo _tmpFE;void*_tmpFF;_LL9F: _tmpFA=_tmpF9.f1;if(_tmpFA <= (
void*)3?1:*((int*)_tmpFA)!= 4)goto _LLA1;_tmpFB=((struct Cyc_Absyn_PointerType_struct*)
_tmpFA)->f1;_tmpFC=(void*)_tmpFB.elt_typ;_tmpFD=_tmpF9.f2;if(_tmpFD <= (void*)3?1:*((
int*)_tmpFD)!= 4)goto _LLA1;_tmpFE=((struct Cyc_Absyn_PointerType_struct*)_tmpFD)->f1;
_tmpFF=(void*)_tmpFE.elt_typ;_LLA0: if(Cyc_Tcutil_unify(_tmpFC,_tmpFF))return Cyc_Absyn_sint_typ;
goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}({struct Cyc_String_pa_struct _tmp103;
_tmp103.tag=0;_tmp103.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp102;_tmp102.tag=0;_tmp102.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp100[2]={&
_tmp102,& _tmp103};Cyc_Tcutil_terr(loc,({const char*_tmp101="comparison not allowed between %s and %s";
_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,41));}),_tag_arr(
_tmp100,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp104;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmp105=p;_LLA4: if((int)_tmp105 != 0)goto _LLA6;_LLA5:
return Cyc_Tcexp_tcPlus(te,e1,e2);_LLA6: if((int)_tmp105 != 2)goto _LLA8;_LLA7:
return Cyc_Tcexp_tcMinus(te,e1,e2);_LLA8: if((int)_tmp105 != 1)goto _LLAA;_LLA9: goto
_LLAB;_LLAA: if((int)_tmp105 != 3)goto _LLAC;_LLAB: return Cyc_Tcexp_tcArithBinop(te,
e1,e2,Cyc_Tcutil_is_numeric);_LLAC: if((int)_tmp105 != 4)goto _LLAE;_LLAD: goto _LLAF;
_LLAE: if((int)_tmp105 != 13)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp105 != 14)
goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((int)_tmp105 != 15)goto _LLB4;_LLB3: goto _LLB5;
_LLB4: if((int)_tmp105 != 16)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp105 != 17)
goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp105 != 18)goto _LLBA;_LLB9: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LLBA: if((int)_tmp105 != 5)goto _LLBC;_LLBB: goto
_LLBD;_LLBC: if((int)_tmp105 != 6)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp105
!= 7)goto _LLC0;_LLBF: goto _LLC1;_LLC0: if((int)_tmp105 != 8)goto _LLC2;_LLC1: goto
_LLC3;_LLC2: if((int)_tmp105 != 9)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp105
!= 10)goto _LLC6;_LLC5: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLC6:;_LLC7:({
void*_tmp106[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp107="bad binary primop";_tag_arr(_tmp107,sizeof(char),
_get_zero_arr_size(_tmp107,18));}),_tag_arr(_tmp106,sizeof(void*),0));});_LLA3:;}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2?((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLC8: return({
void*_tmp108[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp109="primitive operator has 0 arguments";
_tag_arr(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,35));}),_tag_arr(
_tmp108,sizeof(void*),0));});case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2:
_LLCA: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCB: return({
void*_tmp10A[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp10B="primitive operator has > 2 arguments";
_tag_arr(_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,37));}),_tag_arr(
_tmp10A,sizeof(void*),0));});}return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;
void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){{void*_tmp10C=(void*)e->r;void*_tmp10D;struct Cyc_Absyn_Vardecl*_tmp10E;void*
_tmp10F;struct Cyc_Absyn_Vardecl*_tmp110;void*_tmp111;struct Cyc_Absyn_Vardecl*
_tmp112;void*_tmp113;struct Cyc_Absyn_Vardecl*_tmp114;struct Cyc_Absyn_Exp*_tmp115;
struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct _tagged_arr*
_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct _tagged_arr*_tmp11A;struct Cyc_Absyn_Exp*
_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;_LLCE: if(*((int*)
_tmp10C)!= 1)goto _LLD0;_tmp10D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;
if(_tmp10D <= (void*)1?1:*((int*)_tmp10D)!= 2)goto _LLD0;_tmp10E=((struct Cyc_Absyn_Param_b_struct*)
_tmp10D)->f1;_LLCF: _tmp110=_tmp10E;goto _LLD1;_LLD0: if(*((int*)_tmp10C)!= 1)goto
_LLD2;_tmp10F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp10F <= (
void*)1?1:*((int*)_tmp10F)!= 3)goto _LLD2;_tmp110=((struct Cyc_Absyn_Local_b_struct*)
_tmp10F)->f1;_LLD1: _tmp112=_tmp110;goto _LLD3;_LLD2: if(*((int*)_tmp10C)!= 1)goto
_LLD4;_tmp111=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp111 <= (
void*)1?1:*((int*)_tmp111)!= 4)goto _LLD4;_tmp112=((struct Cyc_Absyn_Pat_b_struct*)
_tmp111)->f1;_LLD3: _tmp114=_tmp112;goto _LLD5;_LLD4: if(*((int*)_tmp10C)!= 1)goto
_LLD6;_tmp113=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp113 <= (
void*)1?1:*((int*)_tmp113)!= 0)goto _LLD6;_tmp114=((struct Cyc_Absyn_Global_b_struct*)
_tmp113)->f1;_LLD5: if(!(_tmp114->tq).q_const)return;goto _LLCD;_LLD6: if(*((int*)
_tmp10C)!= 23)goto _LLD8;_tmp115=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f1;
_tmp116=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f2;_LLD7:{void*_tmp11E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp115->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp11F;struct Cyc_Absyn_Tqual _tmp120;struct Cyc_Absyn_ArrayInfo
_tmp121;struct Cyc_Absyn_Tqual _tmp122;struct Cyc_List_List*_tmp123;_LLE5: if(
_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 4)goto _LLE7;_tmp11F=((struct Cyc_Absyn_PointerType_struct*)
_tmp11E)->f1;_tmp120=_tmp11F.elt_tq;_LLE6: _tmp122=_tmp120;goto _LLE8;_LLE7: if(
_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 7)goto _LLE9;_tmp121=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11E)->f1;_tmp122=_tmp121.tq;_LLE8: if(!_tmp122.q_const)return;goto _LLE4;_LLE9:
if(_tmp11E <= (void*)3?1:*((int*)_tmp11E)!= 9)goto _LLEB;_tmp123=((struct Cyc_Absyn_TupleType_struct*)
_tmp11E)->f1;_LLEA: {unsigned int _tmp125;int _tmp126;struct _tuple7 _tmp124=Cyc_Evexp_eval_const_uint_exp(
_tmp116);_tmp125=_tmp124.f1;_tmp126=_tmp124.f2;if(!_tmp126){({void*_tmp127[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp128="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,47));}),_tag_arr(
_tmp127,sizeof(void*),0));});return;}{struct _handler_cons _tmp129;_push_handler(&
_tmp129);{int _tmp12B=0;if(setjmp(_tmp129.handler))_tmp12B=1;if(!_tmp12B){{struct
_tuple9 _tmp12D;struct Cyc_Absyn_Tqual _tmp12E;struct _tuple9*_tmp12C=((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp123,(int)_tmp125);
_tmp12D=*_tmp12C;_tmp12E=_tmp12D.f1;if(!_tmp12E.q_const){_npop_handler(0);
return;}};_pop_handler();}else{void*_tmp12A=(void*)_exn_thrown;void*_tmp130=
_tmp12A;_LLEE: if(_tmp130 != Cyc_List_Nth)goto _LLF0;_LLEF: return;_LLF0:;_LLF1:(
void)_throw(_tmp130);_LLED:;}}}goto _LLE4;}_LLEB:;_LLEC: goto _LLE4;_LLE4:;}goto
_LLCD;_LLD8: if(*((int*)_tmp10C)!= 21)goto _LLDA;_tmp117=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp10C)->f1;_tmp118=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp10C)->f2;_LLD9:{
void*_tmp131=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp117->topt))->v);struct Cyc_Absyn_AggrInfo _tmp132;void*_tmp133;struct Cyc_Absyn_Aggrdecl**
_tmp134;struct Cyc_List_List*_tmp135;_LLF3: if(_tmp131 <= (void*)3?1:*((int*)
_tmp131)!= 10)goto _LLF5;_tmp132=((struct Cyc_Absyn_AggrType_struct*)_tmp131)->f1;
_tmp133=(void*)_tmp132.aggr_info;if(*((int*)_tmp133)!= 1)goto _LLF5;_tmp134=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp133)->f1;_LLF4: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp134 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp134,_tmp118);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLF2;}_LLF5: if(
_tmp131 <= (void*)3?1:*((int*)_tmp131)!= 11)goto _LLF7;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp131)->f2;_LLF6: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp135,
_tmp118);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLF2;}_LLF7:;_LLF8: goto _LLF2;
_LLF2:;}goto _LLCD;_LLDA: if(*((int*)_tmp10C)!= 22)goto _LLDC;_tmp119=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp10C)->f1;_tmp11A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp10C)->f2;_LLDB:{
void*_tmp136=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp119->topt))->v);struct Cyc_Absyn_PtrInfo _tmp137;void*_tmp138;_LLFA: if(_tmp136
<= (void*)3?1:*((int*)_tmp136)!= 4)goto _LLFC;_tmp137=((struct Cyc_Absyn_PointerType_struct*)
_tmp136)->f1;_tmp138=(void*)_tmp137.elt_typ;_LLFB:{void*_tmp139=Cyc_Tcutil_compress(
_tmp138);struct Cyc_Absyn_AggrInfo _tmp13A;void*_tmp13B;struct Cyc_Absyn_Aggrdecl**
_tmp13C;struct Cyc_List_List*_tmp13D;_LLFF: if(_tmp139 <= (void*)3?1:*((int*)
_tmp139)!= 10)goto _LL101;_tmp13A=((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1;
_tmp13B=(void*)_tmp13A.aggr_info;if(*((int*)_tmp13B)!= 1)goto _LL101;_tmp13C=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp13B)->f1;_LL100: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13C,_tmp11A);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLFE;}_LL101: if(
_tmp139 <= (void*)3?1:*((int*)_tmp139)!= 11)goto _LL103;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp139)->f2;_LL102: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp13D,_tmp11A);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLFE;}_LL103:;_LL104:
goto _LLFE;_LLFE:;}goto _LLF9;_LLFC:;_LLFD: goto _LLF9;_LLF9:;}goto _LLCD;_LLDC: if(*((
int*)_tmp10C)!= 20)goto _LLDE;_tmp11B=((struct Cyc_Absyn_Deref_e_struct*)_tmp10C)->f1;
_LLDD:{void*_tmp13E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13F;struct Cyc_Absyn_Tqual _tmp140;
struct Cyc_Absyn_ArrayInfo _tmp141;struct Cyc_Absyn_Tqual _tmp142;_LL106: if(_tmp13E
<= (void*)3?1:*((int*)_tmp13E)!= 4)goto _LL108;_tmp13F=((struct Cyc_Absyn_PointerType_struct*)
_tmp13E)->f1;_tmp140=_tmp13F.elt_tq;_LL107: _tmp142=_tmp140;goto _LL109;_LL108: if(
_tmp13E <= (void*)3?1:*((int*)_tmp13E)!= 7)goto _LL10A;_tmp141=((struct Cyc_Absyn_ArrayType_struct*)
_tmp13E)->f1;_tmp142=_tmp141.tq;_LL109: if(!_tmp142.q_const)return;goto _LL105;
_LL10A:;_LL10B: goto _LL105;_LL105:;}goto _LLCD;_LLDE: if(*((int*)_tmp10C)!= 11)goto
_LLE0;_tmp11C=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp10C)->f1;_LLDF:
_tmp11D=_tmp11C;goto _LLE1;_LLE0: if(*((int*)_tmp10C)!= 12)goto _LLE2;_tmp11D=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp10C)->f1;_LLE1: Cyc_Tcexp_check_writable(
te,_tmp11D);return;_LLE2:;_LLE3: goto _LLCD;_LLCD:;}({struct Cyc_String_pa_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp143[1]={& _tmp145};Cyc_Tcutil_terr(e->loc,({const char*_tmp144="attempt to write a const location: %s";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,38));}),_tag_arr(
_tmp143,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp146[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp147="increment/decrement of non-lvalue";_tag_arr(_tmp147,
sizeof(char),_get_zero_arr_size(_tmp147,34));}),_tag_arr(_tmp146,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt)?1:(Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt)?i == (void*)0?1: i == (void*)
1: 0)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp148[
0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp149="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,50));}),_tag_arr(
_tmp148,sizeof(void*),0));});}else{({struct Cyc_String_pa_struct _tmp14C;_tmp14C.tag=
0;_tmp14C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp14A[1]={& _tmp14C};Cyc_Tcutil_terr(e->loc,({const char*_tmp14B="expecting arithmetic or ? type but found %s";
_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,44));}),_tag_arr(
_tmp14A,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp14D="conditional expression";_tag_arr(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp156;}));struct Cyc_List_List _tmp14E=({struct Cyc_List_List _tmp155;_tmp155.hd=
e3;_tmp155.tl=0;_tmp155;});struct Cyc_List_List _tmp14F=({struct Cyc_List_List
_tmp154;_tmp154.hd=e2;_tmp154.tl=(struct Cyc_List_List*)& _tmp14E;_tmp154;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp14F)){({struct Cyc_String_pa_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp150[2]={&
_tmp152,& _tmp153};Cyc_Tcutil_terr(loc,({const char*_tmp151="conditional clause types do not match: %s != %s";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,48));}),_tag_arr(
_tmp150,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}static
void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp157=Cyc_Tcutil_compress(t1);_LL10D: if(_tmp157 <= (void*)3?1:*((int*)
_tmp157)!= 7)goto _LL10F;_LL10E:({void*_tmp158[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp159="cannot assign to an array";_tag_arr(_tmp159,sizeof(char),
_get_zero_arr_size(_tmp159,26));}),_tag_arr(_tmp158,sizeof(void*),0));});goto
_LL10C;_LL10F:;_LL110: goto _LL10C;_LL10C:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp15A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp15B="type is abstract (can't determine size).";
_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,41));}),_tag_arr(
_tmp15A,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp15C[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp15D="assignment to non-lvalue";
_tag_arr(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,25));}),_tag_arr(
_tmp15C,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(
te,e2,t1)){void*_tmp15E=({struct Cyc_String_pa_struct _tmp162;_tmp162.tag=0;
_tmp162.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp161;_tmp161.tag=0;_tmp161.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp15F[2]={& _tmp161,&
_tmp162};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp160="type mismatch: %s != %s";
_tag_arr(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,24));}),_tag_arr(
_tmp15F,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp15E;}}else{void*_tmp163=(void*)po->v;void*_tmp164=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp163,e1,e2);if(!(Cyc_Tcutil_unify(_tmp164,t1)?1:(Cyc_Tcutil_coerceable(
_tmp164)?Cyc_Tcutil_coerceable(t1): 0))){void*_tmp165=({struct Cyc_String_pa_struct
_tmp169;_tmp169.tag=0;_tmp169.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp168;_tmp168.tag=0;_tmp168.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp166[2]={&
_tmp168,& _tmp169};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp167="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_arr(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,82));}),_tag_arr(
_tmp166,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp164,t1);Cyc_Tcutil_explain_failure();
return _tmp165;}return _tmp164;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp16A=Cyc_Tcutil_compress(
t1);void*_tmp16B;void*_tmp16C;_LL112: if((int)_tmp16A != 1)goto _LL114;_LL113: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ);t1=dbl_typ;goto _LL111;_LL114: if(_tmp16A <= (void*)3?1:*((int*)
_tmp16A)!= 5)goto _LL116;_tmp16B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp16A)->f2;
if((int)_tmp16B != 0)goto _LL116;_LL115: goto _LL117;_LL116: if(_tmp16A <= (void*)3?1:*((
int*)_tmp16A)!= 5)goto _LL118;_tmp16C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp16A)->f2;if((int)_tmp16C != 1)goto _LL118;_LL117: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ);t1=Cyc_Absyn_sint_typ;goto _LL111;_LL118:;_LL119: goto _LL111;
_LL111:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp16E;
struct Cyc_List_List*_tmp16F;struct Cyc_Position_Segment*_tmp170;void*_tmp171;
struct Cyc_Absyn_Tunionfield _tmp16D=*((struct Cyc_Absyn_Tunionfield*)fields->hd);
_tmp16E=_tmp16D.name;_tmp16F=_tmp16D.typs;_tmp170=_tmp16D.loc;_tmp171=(void*)
_tmp16D.sc;if(_tmp16F == 0?1: _tmp16F->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)_tmp16F->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(
fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp173;struct Cyc_List_List*
_tmp174;struct Cyc_Position_Segment*_tmp175;void*_tmp176;struct Cyc_Absyn_Tunionfield
_tmp172=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp173=_tmp172.name;_tmp174=
_tmp172.typs;_tmp175=_tmp172.loc;_tmp176=(void*)_tmp172.sc;if(_tmp174 == 0?1:
_tmp174->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp174->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}({struct Cyc_String_pa_struct _tmp17A;
_tmp17A.tag=0;_tmp17A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t1));{struct Cyc_String_pa_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(tu));{void*_tmp177[2]={&
_tmp179,& _tmp17A};Cyc_Tcutil_terr(e->loc,({const char*_tmp178="can't find a field in %s to inject a value of type %s";
_tag_arr(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,54));}),_tag_arr(
_tmp177,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp17C=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp17D=t;struct Cyc_Absyn_PtrInfo
_tmp17E;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp180;struct Cyc_Absyn_PtrAtts _tmp181;
void*_tmp182;struct Cyc_Absyn_Conref*_tmp183;struct Cyc_Absyn_Conref*_tmp184;
struct Cyc_Absyn_Conref*_tmp185;_LL11B: if(_tmp17D <= (void*)3?1:*((int*)_tmp17D)!= 
4)goto _LL11D;_tmp17E=((struct Cyc_Absyn_PointerType_struct*)_tmp17D)->f1;_tmp17F=(
void*)_tmp17E.elt_typ;_tmp180=_tmp17E.elt_tq;_tmp181=_tmp17E.ptr_atts;_tmp182=(
void*)_tmp181.rgn;_tmp183=_tmp181.nullable;_tmp184=_tmp181.bounds;_tmp185=
_tmp181.zero_term;_LL11C: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp182);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp184);{void*_tmp186=Cyc_Tcutil_compress(_tmp17F);struct Cyc_Absyn_FnInfo
_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Core_Opt*_tmp189;void*_tmp18A;
struct Cyc_List_List*_tmp18B;int _tmp18C;struct Cyc_Absyn_VarargInfo*_tmp18D;struct
Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;_LL120: if(_tmp186 <= (void*)3?1:*((
int*)_tmp186)!= 8)goto _LL122;_tmp187=((struct Cyc_Absyn_FnType_struct*)_tmp186)->f1;
_tmp188=_tmp187.tvars;_tmp189=_tmp187.effect;_tmp18A=(void*)_tmp187.ret_typ;
_tmp18B=_tmp187.args;_tmp18C=_tmp187.c_varargs;_tmp18D=_tmp187.cyc_varargs;
_tmp18E=_tmp187.rgn_po;_tmp18F=_tmp187.attributes;_LL121: if(topt != 0)Cyc_Tcutil_unify(
_tmp18A,*topt);while(_tmp17C != 0?_tmp18B != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp17C->hd;void*t2=(*((struct _tuple2*)_tmp18B->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_String_pa_struct
_tmp193;_tmp193.tag=0;_tmp193.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmp190[2]={& _tmp192,& _tmp193};Cyc_Tcutil_terr(
e1->loc,({const char*_tmp191="actual argument has type \n%s\n but formal has type \n%s";
_tag_arr(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,54));}),_tag_arr(
_tmp190,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}_tmp17C=_tmp17C->tl;
_tmp18B=_tmp18B->tl;}{int args_already_checked=0;{struct Cyc_List_List*a=_tmp18F;
for(0;a != 0;a=a->tl){void*_tmp194=(void*)a->hd;void*_tmp195;int _tmp196;int
_tmp197;_LL125: if(_tmp194 <= (void*)17?1:*((int*)_tmp194)!= 3)goto _LL127;_tmp195=(
void*)((struct Cyc_Absyn_Format_att_struct*)_tmp194)->f1;_tmp196=((struct Cyc_Absyn_Format_att_struct*)
_tmp194)->f2;_tmp197=((struct Cyc_Absyn_Format_att_struct*)_tmp194)->f3;_LL126:{
struct _handler_cons _tmp198;_push_handler(& _tmp198);{int _tmp19A=0;if(setjmp(
_tmp198.handler))_tmp19A=1;if(!_tmp19A){{struct Cyc_Absyn_Exp*_tmp19B=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp196 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp197 == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp19C=
_cycalloc(sizeof(*_tmp19C));_tmp19C->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp197 - 1);_tmp19C;});}args_already_checked=1;{
void*_tmp19D=_tmp195;_LL12A: if((int)_tmp19D != 0)goto _LL12C;_LL12B: Cyc_Tcexp_check_format_args(
te,_tmp19B,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL129;_LL12C: if((int)
_tmp19D != 1)goto _LL129;_LL12D: Cyc_Tcexp_check_format_args(te,_tmp19B,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL129;_LL129:;}};_pop_handler();}else{void*
_tmp199=(void*)_exn_thrown;void*_tmp19F=_tmp199;_LL12F: if(_tmp19F != Cyc_List_Nth)
goto _LL131;_LL130:({void*_tmp1A0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1A1="bad format arguments";
_tag_arr(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,21));}),_tag_arr(
_tmp1A0,sizeof(void*),0));});goto _LL12E;_LL131:;_LL132:(void)_throw(_tmp19F);
_LL12E:;}}}goto _LL124;_LL127:;_LL128: goto _LL124;_LL124:;}}if(_tmp18B != 0)({void*
_tmp1A2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1A3="too few arguments for function";
_tag_arr(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,31));}),_tag_arr(
_tmp1A2,sizeof(void*),0));});else{if((_tmp17C != 0?1: _tmp18C)?1: _tmp18D != 0){if(
_tmp18C)for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp17C->hd);}else{if(_tmp18D == 0)({void*_tmp1A4[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1A5="too many arguments for function";_tag_arr(_tmp1A5,sizeof(char),
_get_zero_arr_size(_tmp1A5,32));}),_tag_arr(_tmp1A4,sizeof(void*),0));});else{
void*_tmp1A7;int _tmp1A8;struct Cyc_Absyn_VarargInfo _tmp1A6=*_tmp18D;_tmp1A7=(void*)
_tmp1A6.type;_tmp1A8=_tmp1A6.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp1A9=({
struct Cyc_Absyn_VarargCallInfo*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->num_varargs=
0;_tmp1C1->injectors=0;_tmp1C1->vai=(struct Cyc_Absyn_VarargInfo*)_check_null(
_tmp18D);_tmp1C1;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1A9;
if(!_tmp1A8)for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp17C->hd;_tmp1A9->num_varargs ++;Cyc_Tcexp_tcExp(te,(void**)&
_tmp1A7,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp1A7)){({struct Cyc_String_pa_struct
_tmp1AD;_tmp1AD.tag=0;_tmp1AD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{struct Cyc_String_pa_struct
_tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp1A7));{void*_tmp1AA[2]={& _tmp1AC,& _tmp1AD};Cyc_Tcutil_terr(loc,({const char*
_tmp1AB="vararg requires type %s but argument has type %s";_tag_arr(_tmp1AB,
sizeof(char),_get_zero_arr_size(_tmp1AB,49));}),_tag_arr(_tmp1AA,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}}else{void*_tmp1AE=Cyc_Tcutil_compress(_tmp1A7);
struct Cyc_Absyn_TunionInfo _tmp1AF;void*_tmp1B0;struct Cyc_Absyn_Tuniondecl**
_tmp1B1;struct Cyc_Absyn_Tuniondecl*_tmp1B2;struct Cyc_List_List*_tmp1B3;void*
_tmp1B4;_LL134: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 2)goto _LL136;_tmp1AF=((
struct Cyc_Absyn_TunionType_struct*)_tmp1AE)->f1;_tmp1B0=(void*)_tmp1AF.tunion_info;
if(*((int*)_tmp1B0)!= 1)goto _LL136;_tmp1B1=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp1B0)->f1;_tmp1B2=*_tmp1B1;_tmp1B3=_tmp1AF.targs;_tmp1B4=(void*)_tmp1AF.rgn;
_LL135: {struct Cyc_Absyn_Tuniondecl*_tmp1B5=*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp1B2->name);struct Cyc_List_List*fields=0;if(_tmp1B5->fields == 0)({struct Cyc_String_pa_struct
_tmp1B8;_tmp1B8.tag=0;_tmp1B8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp1A7));{void*_tmp1B6[1]={& _tmp1B8};Cyc_Tcutil_terr(loc,({const char*_tmp1B7="can't inject into %s";
_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,21));}),_tag_arr(
_tmp1B6,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1B5->fields))->v;}if(!Cyc_Tcutil_unify(_tmp1B4,Cyc_Tcenv_curr_rgn(
te)))({void*_tmp1B9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1BA="bad region for injected varargs";
_tag_arr(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,32));}),_tag_arr(
_tmp1B9,sizeof(void*),0));});{struct _RegionHandle _tmp1BB=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp1BB;_push_region(rgn);{struct Cyc_List_List*_tmp1BC=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1B5->tvs,_tmp1B3);for(0;
_tmp17C != 0;_tmp17C=_tmp17C->tl){_tmp1A9->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp17C->hd;if(!args_already_checked)Cyc_Tcexp_tcExp(te,0,
e1);{struct Cyc_Absyn_Tunionfield*_tmp1BD=Cyc_Tcexp_tcInjection(te,e1,_tmp1A7,rgn,
_tmp1BC,fields);if(_tmp1BD != 0)_tmp1A9->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A9->injectors,({
struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=(struct Cyc_Absyn_Tunionfield*)
_check_null(_tmp1BD);_tmp1BE->tl=0;_tmp1BE;}));}}}};_pop_region(rgn);}goto _LL133;}
_LL136:;_LL137:({void*_tmp1BF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1C0="bad inject vararg type";
_tag_arr(_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,23));}),_tag_arr(
_tmp1BF,sizeof(void*),0));});goto _LL133;_LL133:;}}}}}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp189))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp18E);return _tmp18A;}_LL122:;_LL123: return({void*_tmp1C2[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp1C3="expected pointer to function";_tag_arr(_tmp1C3,
sizeof(char),_get_zero_arr_size(_tmp1C3,29));}),_tag_arr(_tmp1C2,sizeof(void*),0));});
_LL11F:;}_LL11D:;_LL11E: return({void*_tmp1C4[0]={};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp1C5="expected pointer to function";_tag_arr(_tmp1C5,sizeof(char),
_get_zero_arr_size(_tmp1C5,29));}),_tag_arr(_tmp1C4,sizeof(void*),0));});_LL11A:;}}
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1C8;_tmp1C8.tag=0;_tmp1C8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1C6[1]={&
_tmp1C8};Cyc_Tcutil_terr(loc,({const char*_tmp1C7="expected xtunion exn but found %s";
_tag_arr(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,34));}),_tag_arr(
_tmp1C6,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp1C9;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp1CA=t1;struct Cyc_Absyn_PtrInfo _tmp1CB;void*_tmp1CC;struct Cyc_Absyn_Tqual
_tmp1CD;struct Cyc_Absyn_PtrAtts _tmp1CE;void*_tmp1CF;struct Cyc_Absyn_Conref*
_tmp1D0;struct Cyc_Absyn_Conref*_tmp1D1;struct Cyc_Absyn_Conref*_tmp1D2;_LL139: if(
_tmp1CA <= (void*)3?1:*((int*)_tmp1CA)!= 4)goto _LL13B;_tmp1CB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1CA)->f1;_tmp1CC=(void*)_tmp1CB.elt_typ;_tmp1CD=_tmp1CB.elt_tq;_tmp1CE=
_tmp1CB.ptr_atts;_tmp1CF=(void*)_tmp1CE.rgn;_tmp1D0=_tmp1CE.nullable;_tmp1D1=
_tmp1CE.bounds;_tmp1D2=_tmp1CE.zero_term;_LL13A:{void*_tmp1D3=Cyc_Tcutil_compress(
_tmp1CC);struct Cyc_Absyn_FnInfo _tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Core_Opt*
_tmp1D6;void*_tmp1D7;struct Cyc_List_List*_tmp1D8;int _tmp1D9;struct Cyc_Absyn_VarargInfo*
_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DC;_LL13E: if(
_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 8)goto _LL140;_tmp1D4=((struct Cyc_Absyn_FnType_struct*)
_tmp1D3)->f1;_tmp1D5=_tmp1D4.tvars;_tmp1D6=_tmp1D4.effect;_tmp1D7=(void*)_tmp1D4.ret_typ;
_tmp1D8=_tmp1D4.args;_tmp1D9=_tmp1D4.c_varargs;_tmp1DA=_tmp1D4.cyc_varargs;
_tmp1DB=_tmp1D4.rgn_po;_tmp1DC=_tmp1D4.attributes;_LL13F: {struct Cyc_List_List*
instantiation=0;for(0;ts != 0?_tmp1D5 != 0: 0;(ts=ts->tl,_tmp1D5=_tmp1D5->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1D5->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->hd=({struct _tuple5*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->f1=(struct Cyc_Absyn_Tvar*)_tmp1D5->hd;
_tmp1DE->f2=(void*)ts->hd;_tmp1DE;});_tmp1DD->tl=instantiation;_tmp1DD;});}if(ts
!= 0)return({void*_tmp1DF[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1E0="too many type variables in instantiation";
_tag_arr(_tmp1E0,sizeof(char),_get_zero_arr_size(_tmp1E0,41));}),_tag_arr(
_tmp1DF,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp1E5=_cycalloc(sizeof(*
_tmp1E5));_tmp1E5[0]=({struct Cyc_Absyn_FnType_struct _tmp1E6;_tmp1E6.tag=8;
_tmp1E6.f1=({struct Cyc_Absyn_FnInfo _tmp1E7;_tmp1E7.tvars=_tmp1D5;_tmp1E7.effect=
_tmp1D6;_tmp1E7.ret_typ=(void*)_tmp1D7;_tmp1E7.args=_tmp1D8;_tmp1E7.c_varargs=
_tmp1D9;_tmp1E7.cyc_varargs=_tmp1DA;_tmp1E7.rgn_po=_tmp1DB;_tmp1E7.attributes=
_tmp1DC;_tmp1E7;});_tmp1E6;});_tmp1E5;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1[0]=({struct Cyc_Absyn_PointerType_struct
_tmp1E2;_tmp1E2.tag=4;_tmp1E2.f1=({struct Cyc_Absyn_PtrInfo _tmp1E3;_tmp1E3.elt_typ=(
void*)new_fn_typ;_tmp1E3.elt_tq=_tmp1CD;_tmp1E3.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp1E4;_tmp1E4.rgn=(void*)_tmp1CF;_tmp1E4.nullable=_tmp1D0;_tmp1E4.bounds=
_tmp1D1;_tmp1E4.zero_term=_tmp1D2;_tmp1E4;});_tmp1E3;});_tmp1E2;});_tmp1E1;});}}
_LL140:;_LL141: goto _LL13D;_LL13D:;}goto _LL138;_LL13B:;_LL13C: goto _LL138;_LL138:;}
return({struct Cyc_String_pa_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp1E8[1]={&
_tmp1EA};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1E9="expecting polymorphic type but found %s";
_tag_arr(_tmp1E9,sizeof(char),_get_zero_arr_size(_tmp1E9,40));}),_tag_arr(
_tmp1E8,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(
te,loc,t2,t)?!Cyc_Tcutil_castable(te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,
e): 0){Cyc_Tcutil_unify(t2,t);{void*_tmp1EB=({struct Cyc_String_pa_struct _tmp1EF;
_tmp1EF.tag=0;_tmp1EF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{void*_tmp1EC[2]={&
_tmp1EE,& _tmp1EF};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1ED="cannot cast %s to %s";
_tag_arr(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,21));}),_tag_arr(
_tmp1EC,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp1EB;}}
return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp1F0=0;
struct Cyc_Absyn_Tqual _tmp1F1=Cyc_Absyn_empty_tqual();if(topt != 0){void*_tmp1F2=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Tqual
_tmp1F5;struct Cyc_Absyn_PtrAtts _tmp1F6;struct Cyc_Absyn_Conref*_tmp1F7;_LL143: if(
_tmp1F2 <= (void*)3?1:*((int*)_tmp1F2)!= 4)goto _LL145;_tmp1F3=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F2)->f1;_tmp1F4=(void*)_tmp1F3.elt_typ;_tmp1F5=_tmp1F3.elt_tq;_tmp1F6=
_tmp1F3.ptr_atts;_tmp1F7=_tmp1F6.zero_term;_LL144: _tmp1F0=({void**_tmp1F8=
_cycalloc(sizeof(*_tmp1F8));_tmp1F8[0]=_tmp1F4;_tmp1F8;});_tmp1F1=_tmp1F5;goto
_LL142;_LL145:;_LL146: goto _LL142;_LL142:;}Cyc_Tcexp_tcExpNoInst(te,_tmp1F0,e);{
void*_tmp1F9=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FB;
_LL148: if(*((int*)_tmp1F9)!= 28)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(*((int*)
_tmp1F9)!= 24)goto _LL14C;_LL14B:({void*_tmp1FC[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp1FD="& used to allocate";_tag_arr(_tmp1FD,sizeof(char),
_get_zero_arr_size(_tmp1FD,19));}),_tag_arr(_tmp1FC,sizeof(void*),0));});{void*(*
_tmp1FE)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp1FF=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp200;struct Cyc_Absyn_PtrAtts _tmp201;struct Cyc_Absyn_Conref*
_tmp202;_LL151: if(_tmp1FF <= (void*)3?1:*((int*)_tmp1FF)!= 4)goto _LL153;_tmp200=((
struct Cyc_Absyn_PointerType_struct*)_tmp1FF)->f1;_tmp201=_tmp200.ptr_atts;
_tmp202=_tmp201.nullable;_LL152: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp202))_tmp1FE=Cyc_Absyn_star_typ;goto _LL150;_LL153:;_LL154: goto _LL150;_LL150:;}
return _tmp1FE((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp1F1,Cyc_Absyn_false_conref);}_LL14C: if(*((int*)_tmp1F9)!= 23)goto _LL14E;
_tmp1FA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1F9)->f1;_tmp1FB=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F9)->f2;_LL14D:{void*_tmp203=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1FA->topt))->v);_LL156: if(_tmp203 <= (void*)3?1:*((int*)_tmp203)
!= 9)goto _LL158;_LL157: goto _LL155;_LL158:;_LL159:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp1FA,_tmp1FB,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp1FA,
_tmp1FB);_LL155:;}goto _LL147;_LL14E:;_LL14F: goto _LL147;_LL147:;}{int _tmp205;void*
_tmp206;struct _tuple6 _tmp204=Cyc_Tcutil_addressof_props(te,e);_tmp205=_tmp204.f1;
_tmp206=_tmp204.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp205)
tq.q_const=1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp206,tq,Cyc_Absyn_false_conref);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_String_pa_struct _tmp209;_tmp209.tag=0;_tmp209.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp207[1]={& _tmp209};Cyc_Tcutil_terr(
loc,({const char*_tmp208="sizeof applied to type %s, which has unknown size here";
_tag_arr(_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,55));}),_tag_arr(
_tmp207,sizeof(void*),1));}});if(topt != 0){void*_tmp20A=Cyc_Tcutil_compress(*
topt);_LL15B: if(_tmp20A <= (void*)3?1:*((int*)_tmp20A)!= 14)goto _LL15D;_LL15C:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp20B=_cycalloc(sizeof(*
_tmp20B));_tmp20B[0]=({struct Cyc_Absyn_SizeofType_struct _tmp20C;_tmp20C.tag=14;
_tmp20C.f1=(void*)t;_tmp20C;});_tmp20B;});_LL15D:;_LL15E: goto _LL15A;_LL15A:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct _tagged_arr)*n,(struct
_tagged_arr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*_tmp20D=n;struct
_tagged_arr*_tmp20E;unsigned int _tmp20F;_LL160: if(*((int*)_tmp20D)!= 0)goto
_LL162;_tmp20E=((struct Cyc_Absyn_StructField_struct*)_tmp20D)->f1;_LL161: {int
bad_type=1;{void*_tmp210=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp211;
void*_tmp212;struct Cyc_Absyn_Aggrdecl**_tmp213;struct Cyc_List_List*_tmp214;
_LL165: if(_tmp210 <= (void*)3?1:*((int*)_tmp210)!= 10)goto _LL167;_tmp211=((struct
Cyc_Absyn_AggrType_struct*)_tmp210)->f1;_tmp212=(void*)_tmp211.aggr_info;if(*((
int*)_tmp212)!= 1)goto _LL167;_tmp213=((struct Cyc_Absyn_KnownAggr_struct*)_tmp212)->f1;
_LL166: if((*_tmp213)->impl == 0)goto _LL164;if(!((int(*)(int(*pred)(struct
_tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp20E,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp213)->impl))->fields))({struct Cyc_String_pa_struct _tmp217;
_tmp217.tag=0;_tmp217.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp20E);{void*
_tmp215[1]={& _tmp217};Cyc_Tcutil_terr(loc,({const char*_tmp216="no field of struct/union has name %s";
_tag_arr(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,37));}),_tag_arr(
_tmp215,sizeof(void*),1));}});bad_type=0;goto _LL164;_LL167: if(_tmp210 <= (void*)3?
1:*((int*)_tmp210)!= 11)goto _LL169;_tmp214=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp210)->f2;_LL168: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp20E,_tmp214))({struct Cyc_String_pa_struct _tmp21A;_tmp21A.tag=0;_tmp21A.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp20E);{void*_tmp218[1]={& _tmp21A};Cyc_Tcutil_terr(
loc,({const char*_tmp219="no field of struct/union has name %s";_tag_arr(_tmp219,
sizeof(char),_get_zero_arr_size(_tmp219,37));}),_tag_arr(_tmp218,sizeof(void*),1));}});
bad_type=0;goto _LL164;_LL169:;_LL16A: goto _LL164;_LL164:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp21D;_tmp21D.tag=0;_tmp21D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp21B[1]={& _tmp21D};Cyc_Tcutil_terr(loc,({const char*_tmp21C="%s is not a known struct/union type";
_tag_arr(_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,36));}),_tag_arr(
_tmp21B,sizeof(void*),1));}});goto _LL15F;}_LL162: if(*((int*)_tmp20D)!= 1)goto
_LL15F;_tmp20F=((struct Cyc_Absyn_TupleIndex_struct*)_tmp20D)->f1;_LL163: {int
bad_type=1;{void*_tmp21E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp21F;
void*_tmp220;struct Cyc_Absyn_Aggrdecl**_tmp221;struct Cyc_List_List*_tmp222;
struct Cyc_List_List*_tmp223;struct Cyc_Absyn_TunionFieldInfo _tmp224;void*_tmp225;
struct Cyc_Absyn_Tunionfield*_tmp226;_LL16C: if(_tmp21E <= (void*)3?1:*((int*)
_tmp21E)!= 10)goto _LL16E;_tmp21F=((struct Cyc_Absyn_AggrType_struct*)_tmp21E)->f1;
_tmp220=(void*)_tmp21F.aggr_info;if(*((int*)_tmp220)!= 1)goto _LL16E;_tmp221=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp220)->f1;_LL16D: if((*_tmp221)->impl == 0)
goto _LL16B;_tmp222=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp221)->impl))->fields;
goto _LL16F;_LL16E: if(_tmp21E <= (void*)3?1:*((int*)_tmp21E)!= 11)goto _LL170;
_tmp222=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp21E)->f2;_LL16F: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp222)<= _tmp20F)({struct Cyc_Int_pa_struct
_tmp22A;_tmp22A.tag=1;_tmp22A.f1=(unsigned int)((int)_tmp20F);{struct Cyc_Int_pa_struct
_tmp229;_tmp229.tag=1;_tmp229.f1=(unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp222);{void*_tmp227[2]={& _tmp229,& _tmp22A};Cyc_Tcutil_terr(
loc,({const char*_tmp228="struct/union has too few components: %d <= %d";_tag_arr(
_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,46));}),_tag_arr(_tmp227,sizeof(
void*),2));}}});bad_type=0;goto _LL16B;_LL170: if(_tmp21E <= (void*)3?1:*((int*)
_tmp21E)!= 9)goto _LL172;_tmp223=((struct Cyc_Absyn_TupleType_struct*)_tmp21E)->f1;
_LL171: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp223)<= _tmp20F)({
struct Cyc_Int_pa_struct _tmp22E;_tmp22E.tag=1;_tmp22E.f1=(unsigned int)((int)
_tmp20F);{struct Cyc_Int_pa_struct _tmp22D;_tmp22D.tag=1;_tmp22D.f1=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp223);{void*_tmp22B[2]={&
_tmp22D,& _tmp22E};Cyc_Tcutil_terr(loc,({const char*_tmp22C="tuple has too few components: %d <= %d";
_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,39));}),_tag_arr(
_tmp22B,sizeof(void*),2));}}});bad_type=0;goto _LL16B;_LL172: if(_tmp21E <= (void*)
3?1:*((int*)_tmp21E)!= 3)goto _LL174;_tmp224=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp21E)->f1;_tmp225=(void*)_tmp224.field_info;if(*((int*)_tmp225)!= 1)goto
_LL174;_tmp226=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp225)->f2;_LL173:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp226->typs)< _tmp20F)({
struct Cyc_Int_pa_struct _tmp232;_tmp232.tag=1;_tmp232.f1=(unsigned int)((int)
_tmp20F);{struct Cyc_Int_pa_struct _tmp231;_tmp231.tag=1;_tmp231.f1=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp226->typs);{void*_tmp22F[2]={&
_tmp231,& _tmp232};Cyc_Tcutil_terr(loc,({const char*_tmp230="tunionfield has too few components: %d < %d";
_tag_arr(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,44));}),_tag_arr(
_tmp22F,sizeof(void*),2));}}});bad_type=0;goto _LL16B;_LL174:;_LL175: goto _LL16B;
_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp235;_tmp235.tag=0;_tmp235.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp233[1]={&
_tmp235};Cyc_Tcutil_terr(loc,({const char*_tmp234="%s is not a known type";
_tag_arr(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,23));}),_tag_arr(
_tmp233,sizeof(void*),1));}});goto _LL15F;}_LL15F:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp236=t;struct Cyc_Absyn_PtrInfo
_tmp237;void*_tmp238;struct Cyc_Absyn_PtrAtts _tmp239;void*_tmp23A;struct Cyc_Absyn_Conref*
_tmp23B;struct Cyc_Absyn_Conref*_tmp23C;_LL177: if(_tmp236 <= (void*)3?1:*((int*)
_tmp236)!= 4)goto _LL179;_tmp237=((struct Cyc_Absyn_PointerType_struct*)_tmp236)->f1;
_tmp238=(void*)_tmp237.elt_typ;_tmp239=_tmp237.ptr_atts;_tmp23A=(void*)_tmp239.rgn;
_tmp23B=_tmp239.bounds;_tmp23C=_tmp239.zero_term;_LL178: Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp23A);Cyc_Tcutil_check_nonzero_bound(loc,_tmp23B);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp238),(void*)1))({void*_tmp23D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp23E="can't dereference abstract pointer type";_tag_arr(_tmp23E,
sizeof(char),_get_zero_arr_size(_tmp23E,40));}),_tag_arr(_tmp23D,sizeof(void*),0));});
return _tmp238;_LL179:;_LL17A: return({struct Cyc_String_pa_struct _tmp241;_tmp241.tag=
0;_tmp241.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp23F[1]={& _tmp241};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp240="expecting * or @ type but found %s";
_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,35));}),_tag_arr(
_tmp23F,sizeof(void*),1));}});_LL176:;}}static void*Cyc_Tcexp_tcAggrMember(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp242=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp243;void*_tmp244;struct Cyc_Absyn_Aggrdecl**_tmp245;
struct Cyc_Absyn_Aggrdecl*_tmp246;struct Cyc_List_List*_tmp247;struct Cyc_List_List*
_tmp248;_LL17C: if(_tmp242 <= (void*)3?1:*((int*)_tmp242)!= 10)goto _LL17E;_tmp243=((
struct Cyc_Absyn_AggrType_struct*)_tmp242)->f1;_tmp244=(void*)_tmp243.aggr_info;
if(*((int*)_tmp244)!= 1)goto _LL17E;_tmp245=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp244)->f1;_tmp246=*_tmp245;_tmp247=_tmp243.targs;_LL17D: {struct Cyc_Absyn_Aggrfield*
_tmp24B=Cyc_Absyn_lookup_decl_field(_tmp246,f);if(_tmp24B == 0)return({struct Cyc_String_pa_struct
_tmp24F;_tmp24F.tag=0;_tmp24F.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{
struct Cyc_String_pa_struct _tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp246->name));{void*_tmp24C[2]={&
_tmp24E,& _tmp24F};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp24D="type %s has no %s field";
_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,24));}),_tag_arr(
_tmp24C,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp246->impl))->exist_vars != 0)return({void*_tmp250[0]={};Cyc_Tcexp_expr_err(te,
loc,({const char*_tmp251="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,64));}),_tag_arr(
_tmp250,sizeof(void*),0));});{void*t2;{struct _RegionHandle _tmp252=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp252;_push_region(rgn);{struct Cyc_List_List*_tmp253=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp246->tvs,_tmp247);t2=Cyc_Tcutil_rsubstitute(
rgn,_tmp253,(void*)_tmp24B->type);};_pop_region(rgn);}return t2;}}_LL17E: if(
_tmp242 <= (void*)3?1:*((int*)_tmp242)!= 11)goto _LL180;_tmp248=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp242)->f2;_LL17F: {struct Cyc_Absyn_Aggrfield*_tmp254=Cyc_Absyn_lookup_field(
_tmp248,f);if(_tmp254 == 0)return({struct Cyc_String_pa_struct _tmp257;_tmp257.tag=
0;_tmp257.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{void*_tmp255[1]={&
_tmp257};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp256="type has no %s field";
_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,21));}),_tag_arr(
_tmp255,sizeof(void*),1));}});return(void*)_tmp254->type;}_LL180: if(_tmp242 <= (
void*)3?1:*((int*)_tmp242)!= 7)goto _LL182;if(!(Cyc_strcmp((struct _tagged_arr)*f,({
const char*_tmp249="size";_tag_arr(_tmp249,sizeof(char),_get_zero_arr_size(
_tmp249,5));}))== 0))goto _LL182;_LL181: goto _LL183;_LL182: if(_tmp242 <= (void*)3?1:*((
int*)_tmp242)!= 4)goto _LL184;if(!(Cyc_strcmp((struct _tagged_arr)*f,({const char*
_tmp24A="size";_tag_arr(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,5));}))
== 0))goto _LL184;_LL183:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp259;_tmp259.tag=3;_tmp259.f1=(void*)((void*)19);_tmp259.f2=({struct Cyc_List_List*
_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->hd=e;_tmp25A->tl=0;_tmp25A;});
_tmp259;});_tmp258;})));return Cyc_Absyn_uint_typ;_LL184:;_LL185: if(Cyc_strcmp((
struct _tagged_arr)*f,({const char*_tmp25B="size";_tag_arr(_tmp25B,sizeof(char),
_get_zero_arr_size(_tmp25B,5));}))== 0)return({struct Cyc_String_pa_struct _tmp25E;
_tmp25E.tag=0;_tmp25E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp25C[1]={&
_tmp25E};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp25D="expecting struct, union, or array, found %s";
_tag_arr(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,44));}),_tag_arr(
_tmp25C,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct _tmp261;
_tmp261.tag=0;_tmp261.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp25F[1]={&
_tmp261};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp260="expecting struct or union, found %s";
_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,36));}),_tag_arr(
_tmp25F,sizeof(void*),1));}});}_LL17B:;}}static void*Cyc_Tcexp_tcAggrArrow(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp262=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp263;void*_tmp264;struct Cyc_Absyn_PtrAtts _tmp265;void*_tmp266;struct Cyc_Absyn_Conref*
_tmp267;struct Cyc_Absyn_Conref*_tmp268;_LL187: if(_tmp262 <= (void*)3?1:*((int*)
_tmp262)!= 4)goto _LL189;_tmp263=((struct Cyc_Absyn_PointerType_struct*)_tmp262)->f1;
_tmp264=(void*)_tmp263.elt_typ;_tmp265=_tmp263.ptr_atts;_tmp266=(void*)_tmp265.rgn;
_tmp267=_tmp265.bounds;_tmp268=_tmp265.zero_term;_LL188: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp267);{void*_tmp269=Cyc_Tcutil_compress(_tmp264);struct Cyc_Absyn_AggrInfo
_tmp26A;void*_tmp26B;struct Cyc_Absyn_Aggrdecl**_tmp26C;struct Cyc_Absyn_Aggrdecl*
_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp26F;_LL18C: if(
_tmp269 <= (void*)3?1:*((int*)_tmp269)!= 10)goto _LL18E;_tmp26A=((struct Cyc_Absyn_AggrType_struct*)
_tmp269)->f1;_tmp26B=(void*)_tmp26A.aggr_info;if(*((int*)_tmp26B)!= 1)goto _LL18E;
_tmp26C=((struct Cyc_Absyn_KnownAggr_struct*)_tmp26B)->f1;_tmp26D=*_tmp26C;
_tmp26E=_tmp26A.targs;_LL18D: {struct Cyc_Absyn_Aggrfield*_tmp270=Cyc_Absyn_lookup_decl_field(
_tmp26D,f);if(_tmp270 == 0)return({struct Cyc_String_pa_struct _tmp274;_tmp274.tag=
0;_tmp274.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{struct Cyc_String_pa_struct
_tmp273;_tmp273.tag=0;_tmp273.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp26D->name));{void*_tmp271[2]={& _tmp273,& _tmp274};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp272="type %s has no %s field";_tag_arr(_tmp272,sizeof(char),
_get_zero_arr_size(_tmp272,24));}),_tag_arr(_tmp271,sizeof(void*),2));}}});if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp26D->impl))->exist_vars != 0)return({
void*_tmp275[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp276="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp276,sizeof(char),_get_zero_arr_size(_tmp276,64));}),_tag_arr(
_tmp275,sizeof(void*),0));});{void*t3;{struct _RegionHandle _tmp277=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp277;_push_region(rgn);{struct Cyc_List_List*_tmp278=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp26D->tvs,_tmp26E);t3=Cyc_Tcutil_rsubstitute(
rgn,_tmp278,(void*)_tmp270->type);};_pop_region(rgn);}return t3;}}_LL18E: if(
_tmp269 <= (void*)3?1:*((int*)_tmp269)!= 11)goto _LL190;_tmp26F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp269)->f2;_LL18F: {struct Cyc_Absyn_Aggrfield*_tmp279=Cyc_Absyn_lookup_field(
_tmp26F,f);if(_tmp279 == 0)return({struct Cyc_String_pa_struct _tmp27C;_tmp27C.tag=
0;_tmp27C.f1=(struct _tagged_arr)((struct _tagged_arr)*f);{void*_tmp27A[1]={&
_tmp27C};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp27B="type has no %s field";
_tag_arr(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,21));}),_tag_arr(
_tmp27A,sizeof(void*),1));}});return(void*)_tmp279->type;}_LL190:;_LL191: goto
_LL18B;_LL18B:;}goto _LL186;_LL189:;_LL18A: goto _LL186;_LL186:;}return({struct Cyc_String_pa_struct
_tmp27F;_tmp27F.tag=0;_tmp27F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp27D[1]={&
_tmp27F};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp27E="expecting struct pointer, found %s";
_tag_arr(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,35));}),_tag_arr(
_tmp27D,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int _tmp281;int _tmp282;struct _tuple7 _tmp280=Cyc_Evexp_eval_const_uint_exp(
index);_tmp281=_tmp280.f1;_tmp282=_tmp280.f2;if(!_tmp282)return({void*_tmp283[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp284="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,47));}),_tag_arr(
_tmp283,sizeof(void*),0));});{struct _handler_cons _tmp285;_push_handler(& _tmp285);{
int _tmp287=0;if(setjmp(_tmp285.handler))_tmp287=1;if(!_tmp287){{void*_tmp288=(*((
struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp281)).f2;
_npop_handler(0);return _tmp288;};_pop_handler();}else{void*_tmp286=(void*)
_exn_thrown;void*_tmp28A=_tmp286;_LL193: if(_tmp28A != Cyc_List_Nth)goto _LL195;
_LL194: return({struct Cyc_Int_pa_struct _tmp28E;_tmp28E.tag=1;_tmp28E.f1=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp28D;_tmp28D.tag=1;_tmp28D.f1=(unsigned int)((int)_tmp281);{void*_tmp28B[2]={&
_tmp28D,& _tmp28E};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp28C="index is %d but tuple has only %d fields";
_tag_arr(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,41));}),_tag_arr(
_tmp28B,sizeof(void*),2));}}});_LL195:;_LL196:(void)_throw(_tmp28A);_LL192:;}}}}
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(te,e2))return({struct
Cyc_String_pa_struct _tmp291;_tmp291.tag=0;_tmp291.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_typ2string(t2));{void*_tmp28F[1]={& _tmp291};Cyc_Tcexp_expr_err(
te,e2->loc,({const char*_tmp290="expecting int subscript, found %s";_tag_arr(
_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,34));}),_tag_arr(_tmp28F,sizeof(
void*),1));}});{void*_tmp292=t1;struct Cyc_Absyn_PtrInfo _tmp293;void*_tmp294;
struct Cyc_Absyn_Tqual _tmp295;struct Cyc_Absyn_PtrAtts _tmp296;void*_tmp297;struct
Cyc_Absyn_Conref*_tmp298;struct Cyc_Absyn_Conref*_tmp299;struct Cyc_List_List*
_tmp29A;_LL198: if(_tmp292 <= (void*)3?1:*((int*)_tmp292)!= 4)goto _LL19A;_tmp293=((
struct Cyc_Absyn_PointerType_struct*)_tmp292)->f1;_tmp294=(void*)_tmp293.elt_typ;
_tmp295=_tmp293.elt_tq;_tmp296=_tmp293.ptr_atts;_tmp297=(void*)_tmp296.rgn;
_tmp298=_tmp296.bounds;_tmp299=_tmp296.zero_term;_LL199: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp299)){int emit_warning=1;{void*
_tmp29B=(void*)(Cyc_Absyn_compress_conref(_tmp298))->v;void*_tmp29C;struct Cyc_Absyn_Exp*
_tmp29D;void*_tmp29E;_LL19F: if(_tmp29B <= (void*)1?1:*((int*)_tmp29B)!= 0)goto
_LL1A1;_tmp29C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp29B)->f1;if(
_tmp29C <= (void*)1?1:*((int*)_tmp29C)!= 0)goto _LL1A1;_tmp29D=((struct Cyc_Absyn_Upper_b_struct*)
_tmp29C)->f1;_LL1A0: if(Cyc_Tcutil_is_const_exp(te,e2)){unsigned int _tmp2A0;int
_tmp2A1;struct _tuple7 _tmp29F=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2A0=_tmp29F.f1;
_tmp2A1=_tmp29F.f2;if(_tmp2A1){unsigned int _tmp2A3;int _tmp2A4;struct _tuple7
_tmp2A2=Cyc_Evexp_eval_const_uint_exp(_tmp29D);_tmp2A3=_tmp2A2.f1;_tmp2A4=
_tmp2A2.f2;if(_tmp2A4?_tmp2A3 > _tmp2A0: 0)emit_warning=0;}}goto _LL19E;_LL1A1: if(
_tmp29B <= (void*)1?1:*((int*)_tmp29B)!= 0)goto _LL1A3;_tmp29E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp29B)->f1;if((int)_tmp29E != 0)goto _LL1A3;_LL1A2: emit_warning=0;goto _LL19E;
_LL1A3:;_LL1A4: goto _LL19E;_LL19E:;}if(emit_warning)({void*_tmp2A5[0]={};Cyc_Tcutil_warn(
e2->loc,({const char*_tmp2A6="subscript on thin, zero-terminated pointer could be expensive.";
_tag_arr(_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,63));}),_tag_arr(
_tmp2A5,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(te,e2)){
unsigned int _tmp2A8;int _tmp2A9;struct _tuple7 _tmp2A7=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2A8=_tmp2A7.f1;_tmp2A9=_tmp2A7.f2;if(_tmp2A9)Cyc_Tcutil_check_bound(loc,
_tmp2A8,_tmp298);}else{if(Cyc_Tcutil_is_bound_one(_tmp298)?!((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp299): 0)({void*_tmp2AA[0]={};Cyc_Tcutil_warn(
e1->loc,({const char*_tmp2AB="subscript applied to pointer to one object";
_tag_arr(_tmp2AB,sizeof(char),_get_zero_arr_size(_tmp2AB,43));}),_tag_arr(
_tmp2AA,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,_tmp298);}}Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp297);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp294),(void*)1))({
void*_tmp2AC[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp2AD="can't subscript an abstract pointer";
_tag_arr(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,36));}),_tag_arr(
_tmp2AC,sizeof(void*),0));});return _tmp294;_LL19A: if(_tmp292 <= (void*)3?1:*((int*)
_tmp292)!= 9)goto _LL19C;_tmp29A=((struct Cyc_Absyn_TupleType_struct*)_tmp292)->f1;
_LL19B: return Cyc_Tcexp_ithTupleType(te,loc,_tmp29A,e2);_LL19C:;_LL19D: return({
struct Cyc_String_pa_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp2AE[1]={& _tmp2B0};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp2AF="subscript applied to %s";_tag_arr(_tmp2AF,sizeof(
char),_get_zero_arr_size(_tmp2AF,24));}),_tag_arr(_tmp2AE,sizeof(void*),1));}});
_LL197:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2B1=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2B2;
_LL1A6: if(_tmp2B1 <= (void*)3?1:*((int*)_tmp2B1)!= 9)goto _LL1A8;_tmp2B2=((struct
Cyc_Absyn_TupleType_struct*)_tmp2B1)->f1;_LL1A7: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2B2)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es))goto _LL1A5;for(0;es != 0;(es=es->tl,_tmp2B2=_tmp2B2->tl)){void*_tmp2B3=(*((
struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2B2))->hd)).f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp2B3,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->hd=({struct _tuple9*_tmp2B5=
_cycalloc(sizeof(*_tmp2B5));_tmp2B5->f1=(*((struct _tuple9*)_tmp2B2->hd)).f1;
_tmp2B5->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2B5;});_tmp2B4->tl=fields;_tmp2B4;});}done=1;goto _LL1A5;_LL1A8:;_LL1A9: goto
_LL1A5;_LL1A5:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(
struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*_tmp2B6=_cycalloc(
sizeof(*_tmp2B6));_tmp2B6->hd=({struct _tuple9*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));
_tmp2B7->f1=Cyc_Absyn_empty_tqual();_tmp2B7->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2B7;});_tmp2B6->tl=
fields;_tmp2B6;});}return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp2B8=
_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Absyn_TupleType_struct _tmp2B9;
_tmp2B9.tag=9;_tmp2B9.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp2B9;});_tmp2B8;});}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des){return({void*_tmp2BA[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp2BB="tcCompoundLit";
_tag_arr(_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,14));}),_tag_arr(
_tmp2BA,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,struct Cyc_List_List*
des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2D6;}));int _tmp2BC=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);struct Cyc_Absyn_Const_e_struct*_tmp2BD=({struct Cyc_Absyn_Const_e_struct*
_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_Absyn_Const_e_struct
_tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_Absyn_Int_c_struct
_tmp2D5;_tmp2D5.tag=2;_tmp2D5.f1=(void*)((void*)1);_tmp2D5.f2=_tmp2BC;_tmp2D5;});
_tmp2D4;}));_tmp2D3;});_tmp2D2;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp2BD,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2BE=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2BC - 1);if(!Cyc_Tcutil_is_zero(
_tmp2BE))({void*_tmp2BF[0]={};Cyc_Tcutil_terr(_tmp2BE->loc,({const char*_tmp2C0="zero-terminated array doesn't end with zero.";
_tag_arr(_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,45));}),_tag_arr(
_tmp2BF,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp2C1=
_cycalloc(sizeof(*_tmp2C1));_tmp2C1->v=(void*)Cyc_Absyn_uint_typ;_tmp2C1;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
res));{void*_tmp2C2[1]={& _tmp2C4};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp2C3="elements of array do not all have the same type (%s)";
_tag_arr(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,53));}),_tag_arr(
_tmp2C2,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp2C5=(
void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2C6;_LL1AB: if(*((int*)_tmp2C5)!= 1)goto
_LL1AD;_LL1AC:({void*_tmp2C7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2C8="only array index designators are supported";
_tag_arr(_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,43));}),_tag_arr(
_tmp2C7,sizeof(void*),0));});goto _LL1AA;_LL1AD: if(*((int*)_tmp2C5)!= 0)goto
_LL1AA;_tmp2C6=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2C5)->f1;_LL1AE: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp2C6);{unsigned int _tmp2CA;int _tmp2CB;struct _tuple7 _tmp2C9=Cyc_Evexp_eval_const_uint_exp(
_tmp2C6);_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;if(!_tmp2CB)({void*_tmp2CC[0]={};
Cyc_Tcutil_terr(_tmp2C6->loc,({const char*_tmp2CD="index designator cannot use sizeof or offsetof";
_tag_arr(_tmp2CD,sizeof(char),_get_zero_arr_size(_tmp2CD,47));}),_tag_arr(
_tmp2CC,sizeof(void*),0));});else{if(_tmp2CA != offset)({struct Cyc_Int_pa_struct
_tmp2D1;_tmp2D1.tag=1;_tmp2D1.f1=(unsigned int)((int)_tmp2CA);{struct Cyc_Int_pa_struct
_tmp2D0;_tmp2D0.tag=1;_tmp2D0.f1=(unsigned int)offset;{void*_tmp2CE[2]={& _tmp2D0,&
_tmp2D1};Cyc_Tcutil_terr(_tmp2C6->loc,({const char*_tmp2CF="expecting index designator %d but found %d";
_tag_arr(_tmp2CF,sizeof(char),_get_zero_arr_size(_tmp2CF,43));}),_tag_arr(
_tmp2CE,sizeof(void*),2));}}});}goto _LL1AA;}_LL1AA:;}}}return res_t2;}}static void*
Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp2D9;_tmp2D9.tag=0;_tmp2D9.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v));{void*_tmp2D7[1]={& _tmp2D9};Cyc_Tcutil_terr(bound->loc,({
const char*_tmp2D8="expecting unsigned int, found %s";_tag_arr(_tmp2D8,sizeof(
char),_get_zero_arr_size(_tmp2D8,33));}),_tag_arr(_tmp2D7,sizeof(void*),1));}});
if(!(vd->tq).q_const)({void*_tmp2DA[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2DB="comprehension index variable is not declared const!";
_tag_arr(_tmp2DB,sizeof(char),_get_zero_arr_size(_tmp2DB,52));}),_tag_arr(
_tmp2DA,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(loc,te);te=
Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;void**_tmp2DC=
0;struct Cyc_Absyn_Tqual*_tmp2DD=0;struct Cyc_Absyn_Conref**_tmp2DE=0;if(topt != 0){
void*_tmp2DF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2E0;struct Cyc_Absyn_ArrayInfo
_tmp2E1;void*_tmp2E2;void**_tmp2E3;struct Cyc_Absyn_Tqual _tmp2E4;struct Cyc_Absyn_Tqual*
_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Conref*_tmp2E7;struct Cyc_Absyn_Conref**
_tmp2E8;_LL1B0: if(_tmp2DF <= (void*)3?1:*((int*)_tmp2DF)!= 4)goto _LL1B2;_tmp2E0=((
struct Cyc_Absyn_PointerType_struct*)_tmp2DF)->f1;_LL1B1: pinfo=_tmp2E0;_tmp2DC=(
void**)((void**)((void*)& pinfo.elt_typ));_tmp2DD=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;
_tmp2DE=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto _LL1AF;_LL1B2:
if(_tmp2DF <= (void*)3?1:*((int*)_tmp2DF)!= 7)goto _LL1B4;_tmp2E1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2DF)->f1;_tmp2E2=(void*)_tmp2E1.elt_type;_tmp2E3=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp2DF)->f1).elt_type;_tmp2E4=_tmp2E1.tq;_tmp2E5=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp2DF)->f1).tq;_tmp2E6=_tmp2E1.num_elts;
_tmp2E7=_tmp2E1.zero_term;_tmp2E8=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp2DF)->f1).zero_term;_LL1B3: _tmp2DC=(void**)_tmp2E3;_tmp2DD=(struct Cyc_Absyn_Tqual*)
_tmp2E5;_tmp2DE=(struct Cyc_Absyn_Conref**)_tmp2E8;goto _LL1AF;_LL1B4:;_LL1B5: goto
_LL1AF;_LL1AF:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp2DC,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp2E9[0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp2EA="bound is not constant";
_tag_arr(_tmp2EA,sizeof(char),_get_zero_arr_size(_tmp2EA,22));}),_tag_arr(
_tmp2E9,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(te,body))({void*_tmp2EB[
0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp2EC="body is not constant";
_tag_arr(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,21));}),_tag_arr(
_tmp2EB,sizeof(void*),0));});}if(_tmp2DE != 0?((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,*_tmp2DE): 0){struct Cyc_Absyn_Exp*_tmp2ED=Cyc_Absyn_uint_exp(
1,0);_tmp2ED->topt=({struct Cyc_Core_Opt*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));
_tmp2EE->v=(void*)Cyc_Absyn_uint_typ;_tmp2EE;});bound=Cyc_Absyn_add_exp(bound,
_tmp2ED,0);bound->topt=({struct Cyc_Core_Opt*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));
_tmp2EF->v=(void*)Cyc_Absyn_uint_typ;_tmp2EF;});*is_zero_term=1;}return Cyc_Absyn_array_typ(
t,_tmp2DD == 0?Cyc_Absyn_empty_tqual():*_tmp2DD,(struct Cyc_Absyn_Exp*)bound,
_tmp2DE == 0?Cyc_Absyn_false_conref:*_tmp2DE);}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp2F0;_push_handler(& _tmp2F0);{int _tmp2F2=0;if(setjmp(
_tmp2F0.handler))_tmp2F2=1;if(!_tmp2F2){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp2F1=(void*)_exn_thrown;void*_tmp2F4=_tmp2F1;_LL1B7:
if(_tmp2F4 != Cyc_Dict_Absent)goto _LL1B9;_LL1B8:({struct Cyc_String_pa_struct
_tmp2F7;_tmp2F7.tag=0;_tmp2F7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(*
tn));{void*_tmp2F5[1]={& _tmp2F7};Cyc_Tcutil_terr(loc,({const char*_tmp2F6="unbound struct name %s";
_tag_arr(_tmp2F6,sizeof(char),_get_zero_arr_size(_tmp2F6,23));}),_tag_arr(
_tmp2F5,sizeof(void*),1));}});return topt != 0?*topt:(void*)0;_LL1B9:;_LL1BA:(void)
_throw(_tmp2F4);_LL1B6:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}
if((void*)ad->kind == (void*)1)({void*_tmp2F8[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp2F9="expecting struct but found union";_tag_arr(_tmp2F9,sizeof(char),
_get_zero_arr_size(_tmp2F9,33));}),_tag_arr(_tmp2F8,sizeof(void*),0));});if(ad->impl
== 0){({void*_tmp2FA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2FB="can't build abstract struct";
_tag_arr(_tmp2FB,sizeof(char),_get_zero_arr_size(_tmp2FB,28));}),_tag_arr(
_tmp2FA,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp2FC;}));}{struct _RegionHandle _tmp2FD=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp2FD;_push_region(rgn);{struct _tuple4 _tmp2FE=({struct _tuple4 _tmp31F;
_tmp31F.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp31F.f2=rgn;_tmp31F;});struct Cyc_List_List*
_tmp2FF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp2FE,ad->tvs);struct Cyc_List_List*_tmp300=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp2FE,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp301=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp2FF);struct Cyc_List_List*_tmp302=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp300);struct Cyc_List_List*_tmp303=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp2FF,_tmp300);struct Cyc_Absyn_AggrType_struct*_tmp304=({struct Cyc_Absyn_AggrType_struct*
_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319[0]=({struct Cyc_Absyn_AggrType_struct
_tmp31A;_tmp31A.tag=10;_tmp31A.f1=({struct Cyc_Absyn_AggrInfo _tmp31B;_tmp31B.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp31C=_cycalloc(sizeof(*
_tmp31C));_tmp31C[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp31D;_tmp31D.tag=1;
_tmp31D.f1=({struct Cyc_Absyn_Aggrdecl**_tmp31E=_cycalloc(sizeof(*_tmp31E));
_tmp31E[0]=ad;_tmp31E;});_tmp31D;});_tmp31C;}));_tmp31B.targs=_tmp301;_tmp31B;});
_tmp31A;});_tmp319;});struct Cyc_List_List*_tmp305=*ts;struct Cyc_List_List*
_tmp306=_tmp302;while(_tmp305 != 0?_tmp306 != 0: 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp305->hd);Cyc_Tcutil_unify((
void*)_tmp305->hd,(void*)_tmp306->hd);_tmp305=_tmp305->tl;_tmp306=_tmp306->tl;}
if(_tmp305 != 0)({void*_tmp307[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp308="too many explicit witness types";
_tag_arr(_tmp308,sizeof(char),_get_zero_arr_size(_tmp308,32));}),_tag_arr(
_tmp307,sizeof(void*),0));});*ts=_tmp302;if(topt != 0)Cyc_Tcutil_unify((void*)
_tmp304,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp30A;struct Cyc_Absyn_Aggrfield*_tmp30B;
struct Cyc_Absyn_Exp*_tmp30C;struct _tuple10*_tmp309=(struct _tuple10*)fields->hd;
_tmp30A=*_tmp309;_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;{void*_tmp30D=Cyc_Tcutil_rsubstitute(
rgn,_tmp303,(void*)_tmp30B->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp30D,
_tmp30C);if(!Cyc_Tcutil_coerce_arg(te,_tmp30C,_tmp30D)){({struct Cyc_String_pa_struct
_tmp313;_tmp313.tag=0;_tmp313.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp30C->topt))->v));{struct Cyc_String_pa_struct
_tmp312;_tmp312.tag=0;_tmp312.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp30D));{struct Cyc_String_pa_struct _tmp311;_tmp311.tag=0;_tmp311.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp310;_tmp310.tag=0;_tmp310.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp30B->name);{
void*_tmp30E[4]={& _tmp310,& _tmp311,& _tmp312,& _tmp313};Cyc_Tcutil_terr(_tmp30C->loc,({
const char*_tmp30F="field %s of struct %s expects type %s != %s";_tag_arr(_tmp30F,
sizeof(char),_get_zero_arr_size(_tmp30F,44));}),_tag_arr(_tmp30E,sizeof(void*),4));}}}}});
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp314=0;{struct Cyc_List_List*
_tmp315=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp315 != 0;_tmp315=_tmp315->tl){_tmp314=({struct Cyc_List_List*_tmp316=_cycalloc(
sizeof(*_tmp316));_tmp316->hd=({struct _tuple0*_tmp317=_cycalloc(sizeof(*_tmp317));
_tmp317->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp303,(*((struct _tuple0*)_tmp315->hd)).f1);
_tmp317->f2=Cyc_Tcutil_rsubstitute(rgn,_tmp303,(*((struct _tuple0*)_tmp315->hd)).f2);
_tmp317;});_tmp316->tl=_tmp314;_tmp316;});}}_tmp314=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp314);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp314);{void*_tmp318=(void*)_tmp304;_npop_handler(0);return _tmp318;}}}};
_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*args){{struct
_RegionHandle _tmp320=_new_region("rgn");struct _RegionHandle*rgn=& _tmp320;
_push_region(rgn);{void*_tmp321=Cyc_Tcutil_compress(ts);void*_tmp322;struct Cyc_List_List*
_tmp323;_LL1BC: if(_tmp321 <= (void*)3?1:*((int*)_tmp321)!= 11)goto _LL1BE;_tmp322=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp321)->f1;_tmp323=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp321)->f2;_LL1BD: if(_tmp322 == (void*)1)({void*_tmp324[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp325="expecting struct but found union";_tag_arr(_tmp325,
sizeof(char),_get_zero_arr_size(_tmp325,33));}),_tag_arr(_tmp324,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp323);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp327;struct Cyc_Absyn_Aggrfield*
_tmp328;struct Cyc_Absyn_Exp*_tmp329;struct _tuple10*_tmp326=(struct _tuple10*)
fields->hd;_tmp327=*_tmp326;_tmp328=_tmp327.f1;_tmp329=_tmp327.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp328->type)),_tmp329);if(!Cyc_Tcutil_coerce_arg(
te,_tmp329,(void*)_tmp328->type)){({struct Cyc_String_pa_struct _tmp32E;_tmp32E.tag=
0;_tmp32E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp329->topt))->v));{struct Cyc_String_pa_struct
_tmp32D;_tmp32D.tag=0;_tmp32D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp328->type));{struct Cyc_String_pa_struct _tmp32C;_tmp32C.tag=0;_tmp32C.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp328->name);{void*_tmp32A[3]={& _tmp32C,&
_tmp32D,& _tmp32E};Cyc_Tcutil_terr(_tmp329->loc,({const char*_tmp32B="field %s of struct expects type %s != %s";
_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,41));}),_tag_arr(
_tmp32A,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1BB;}
_LL1BE:;_LL1BF:({void*_tmp32F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp330="tcAnonStruct: wrong type";
_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,25));}),_tag_arr(
_tmp32F,sizeof(void*),0));});_LL1BB:;};_pop_region(rgn);}return ts;}static void*
Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*tud,
struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp331=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp331;_push_region(rgn);{struct _tuple4 _tmp332=({
struct _tuple4 _tmp355;_tmp355.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp355.f2=rgn;
_tmp355;});struct Cyc_List_List*_tmp333=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp332,tud->tvs);struct Cyc_List_List*_tmp334=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp333);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp351;_tmp351.tag=3;_tmp351.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp352;
_tmp352.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp354;_tmp354.tag=1;_tmp354.f1=tud;_tmp354.f2=tuf;_tmp354;});_tmp353;}));
_tmp352.targs=_tmp334;_tmp352;});_tmp351;});_tmp350;});if(topt != 0){void*_tmp335=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp336;struct Cyc_List_List*
_tmp337;void*_tmp338;_LL1C1: if(_tmp335 <= (void*)3?1:*((int*)_tmp335)!= 3)goto
_LL1C3;_LL1C2: Cyc_Tcutil_unify(*topt,res);goto _LL1C0;_LL1C3: if(_tmp335 <= (void*)
3?1:*((int*)_tmp335)!= 2)goto _LL1C5;_tmp336=((struct Cyc_Absyn_TunionType_struct*)
_tmp335)->f1;_tmp337=_tmp336.targs;_tmp338=(void*)_tmp336.rgn;_LL1C4:{struct Cyc_List_List*
a=_tmp334;for(0;a != 0?_tmp337 != 0: 0;(a=a->tl,_tmp337=_tmp337->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp337->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->v=(void*)res;_tmp339;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A[0]=({
struct Cyc_Absyn_TunionType_struct _tmp33B;_tmp33B.tag=2;_tmp33B.f1=({struct Cyc_Absyn_TunionInfo
_tmp33C;_tmp33C.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp33E;_tmp33E.tag=1;_tmp33E.f1=({struct Cyc_Absyn_Tuniondecl**_tmp33F=_cycalloc(
sizeof(*_tmp33F));_tmp33F[0]=tud;_tmp33F;});_tmp33E;});_tmp33D;}));_tmp33C.targs=
_tmp334;_tmp33C.rgn=(void*)_tmp338;_tmp33C;});_tmp33B;});_tmp33A;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp340=res;_npop_handler(0);return _tmp340;}}goto _LL1C0;_LL1C5:;
_LL1C6: goto _LL1C0;_LL1C0:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp333,(*((struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmp345;_tmp345.tag=0;_tmp345.f1=(struct _tagged_arr)((struct _tagged_arr)(e->topt
== 0?({const char*_tmp346="?";_tag_arr(_tmp346,sizeof(char),_get_zero_arr_size(
_tmp346,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp344;_tmp344.tag=0;_tmp344.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp343;
_tmp343.tag=0;_tmp343.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp341[3]={& _tmp343,& _tmp344,& _tmp345};Cyc_Tcutil_terr(e->loc,({
const char*_tmp342="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_arr(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,80));}),_tag_arr(
_tmp341,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0){void*
_tmp34A=({struct Cyc_String_pa_struct _tmp349;_tmp349.tag=0;_tmp349.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp347[1]={& _tmp349};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,({
const char*_tmp348="too many arguments for tunion constructor %s";_tag_arr(
_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,45));}),_tag_arr(_tmp347,sizeof(
void*),1));}});_npop_handler(0);return _tmp34A;}if(ts != 0){void*_tmp34E=({struct
Cyc_String_pa_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp34B[1]={& _tmp34D};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp34C="too few arguments for tunion constructor %s";
_tag_arr(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,44));}),_tag_arr(
_tmp34B,sizeof(void*),1));}});_npop_handler(0);return _tmp34E;}{void*_tmp34F=res;
_npop_handler(0);return _tmp34F;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp356=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp357;
struct Cyc_Absyn_PtrAtts _tmp358;struct Cyc_Absyn_Conref*_tmp359;struct Cyc_Absyn_ArrayInfo
_tmp35A;void*_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_AggrInfo _tmp35D;
void*_tmp35E;struct Cyc_List_List*_tmp35F;struct Cyc_List_List*_tmp360;_LL1C8: if((
int)_tmp356 != 0)goto _LL1CA;_LL1C9: return 1;_LL1CA: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 0)goto _LL1CC;_LL1CB: goto _LL1CD;_LL1CC: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 1)goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 2)goto _LL1D0;_LL1CF: goto _LL1D1;_LL1D0: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 3)goto _LL1D2;_LL1D1: return 0;_LL1D2: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 4)goto _LL1D4;_tmp357=((struct Cyc_Absyn_PointerType_struct*)_tmp356)->f1;
_tmp358=_tmp357.ptr_atts;_tmp359=_tmp358.nullable;_LL1D3: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp359);_LL1D4: if(_tmp356 <= (
void*)3?1:*((int*)_tmp356)!= 5)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6: if((int)
_tmp356 != 1)goto _LL1D8;_LL1D7: goto _LL1D9;_LL1D8: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 6)goto _LL1DA;_LL1D9: return 1;_LL1DA: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 7)goto _LL1DC;_tmp35A=((struct Cyc_Absyn_ArrayType_struct*)_tmp356)->f1;
_tmp35B=(void*)_tmp35A.elt_type;_LL1DB: return Cyc_Tcexp_zeroable_type(_tmp35B);
_LL1DC: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 8)goto _LL1DE;_LL1DD: return 0;
_LL1DE: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 9)goto _LL1E0;_tmp35C=((struct
Cyc_Absyn_TupleType_struct*)_tmp356)->f1;_LL1DF: for(0;(unsigned int)_tmp35C;
_tmp35C=_tmp35C->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)_tmp35C->hd)).f2))
return 0;}return 1;_LL1E0: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 10)goto _LL1E2;
_tmp35D=((struct Cyc_Absyn_AggrType_struct*)_tmp356)->f1;_tmp35E=(void*)_tmp35D.aggr_info;
_tmp35F=_tmp35D.targs;_LL1E1: {struct Cyc_Absyn_Aggrdecl*_tmp361=Cyc_Absyn_get_known_aggrdecl(
_tmp35E);if(_tmp361->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp361->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp362=_new_region("r");
struct _RegionHandle*r=& _tmp362;_push_region(r);{struct Cyc_List_List*_tmp363=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp361->tvs,_tmp35F);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp361->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp363,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp364=0;_npop_handler(0);
return _tmp364;}}}{int _tmp365=1;_npop_handler(0);return _tmp365;}};_pop_region(r);}}
_LL1E2: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 12)goto _LL1E4;_LL1E3: return 1;
_LL1E4: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 14)goto _LL1E6;_LL1E5: return 1;
_LL1E6: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 17)goto _LL1E8;_LL1E7: return 1;
_LL1E8: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 11)goto _LL1EA;_tmp360=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp356)->f2;_LL1E9: for(0;_tmp360 != 0;_tmp360=
_tmp360->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp360->hd)->type))return 0;}return 1;_LL1EA: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 13)goto _LL1EC;_LL1EB: return 1;_LL1EC: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 16)goto _LL1EE;_LL1ED: return 0;_LL1EE: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 15)goto _LL1F0;_LL1EF: return 0;_LL1F0: if(_tmp356 <= (void*)3?1:*((int*)
_tmp356)!= 18)goto _LL1F2;_LL1F1: goto _LL1F3;_LL1F2: if((int)_tmp356 != 2)goto _LL1F4;
_LL1F3: goto _LL1F5;_LL1F4: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 19)goto
_LL1F6;_LL1F5: goto _LL1F7;_LL1F6: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 20)
goto _LL1F8;_LL1F7: goto _LL1F9;_LL1F8: if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 
21)goto _LL1C7;_LL1F9:({struct Cyc_String_pa_struct _tmp368;_tmp368.tag=0;_tmp368.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp366[1]={&
_tmp368};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp367="bad type `%s' in zeroable type";_tag_arr(_tmp367,sizeof(char),
_get_zero_arr_size(_tmp367,31));}),_tag_arr(_tmp366,sizeof(void*),1));}});_LL1C7:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(
t): 0))return;if(topt != 0){void*_tmp369=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp36A;void*_tmp36B;_LL1FB: if(_tmp369 <= (void*)3?1:*((int*)_tmp369)!= 4)goto
_LL1FD;_tmp36A=((struct Cyc_Absyn_PointerType_struct*)_tmp369)->f1;_tmp36B=(void*)
_tmp36A.elt_typ;_LL1FC: Cyc_Tcutil_unify(_tmp36B,t);if(Cyc_Tcutil_bits_only(t)?1:(
allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;goto _LL1FA;_LL1FD:;_LL1FE: goto
_LL1FA;_LL1FA:;}({struct Cyc_String_pa_struct _tmp36F;_tmp36F.tag=0;_tmp36F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp36E;_tmp36E.tag=0;_tmp36E.f1=(struct _tagged_arr)((struct _tagged_arr)(
allow_zero?({const char*_tmp370="calloc";_tag_arr(_tmp370,sizeof(char),
_get_zero_arr_size(_tmp370,7));}):({const char*_tmp371="malloc";_tag_arr(_tmp371,
sizeof(char),_get_zero_arr_size(_tmp371,7));})));{void*_tmp36C[2]={& _tmp36E,&
_tmp36F};Cyc_Tcutil_terr(loc,({const char*_tmp36D="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_arr(_tmp36D,sizeof(char),_get_zero_arr_size(_tmp36D,60));}),_tag_arr(
_tmp36C,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp377=
_cycalloc(sizeof(*_tmp377));_tmp377[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp378;_tmp378.tag=15;_tmp378.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp379;}));_tmp378;});_tmp377;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp372=Cyc_Tcutil_compress(handle_type);void*_tmp373;
_LL200: if(_tmp372 <= (void*)3?1:*((int*)_tmp372)!= 15)goto _LL202;_tmp373=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp372)->f1;_LL201: rgn=_tmp373;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1FF;_LL202:;_LL203:({struct Cyc_String_pa_struct _tmp376;
_tmp376.tag=0;_tmp376.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp374[1]={& _tmp376};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp375="expecting region_t type but found %s";_tag_arr(_tmp375,sizeof(char),
_get_zero_arr_size(_tmp375,37));}),_tag_arr(_tmp374,sizeof(void*),1));}});goto
_LL1FF;_LL1FF:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp37A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp37B="calloc with empty type";_tag_arr(_tmp37B,sizeof(char),
_get_zero_arr_size(_tmp37B,23));}),_tag_arr(_tmp37A,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp37C=(void*)(*e)->r;void*_tmp37D;void*
_tmp37E;struct Cyc_List_List*_tmp37F;struct Cyc_List_List _tmp380;struct Cyc_Absyn_Exp*
_tmp381;struct Cyc_List_List*_tmp382;struct Cyc_List_List _tmp383;struct Cyc_Absyn_Exp*
_tmp384;struct Cyc_List_List*_tmp385;_LL205: if(*((int*)_tmp37C)!= 16)goto _LL207;
_tmp37D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp37C)->f1;_LL206:
elt_type=_tmp37D;*t=(void**)({void**_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[
0]=elt_type;_tmp386;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL204;_LL207: if(*((int*)_tmp37C)!= 3)
goto _LL209;_tmp37E=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp37C)->f1;if((
int)_tmp37E != 1)goto _LL209;_tmp37F=((struct Cyc_Absyn_Primop_e_struct*)_tmp37C)->f2;
if(_tmp37F == 0)goto _LL209;_tmp380=*_tmp37F;_tmp381=(struct Cyc_Absyn_Exp*)_tmp380.hd;
_tmp382=_tmp380.tl;if(_tmp382 == 0)goto _LL209;_tmp383=*_tmp382;_tmp384=(struct Cyc_Absyn_Exp*)
_tmp383.hd;_tmp385=_tmp383.tl;if(_tmp385 != 0)goto _LL209;_LL208:{struct _tuple0
_tmp388=({struct _tuple0 _tmp387;_tmp387.f1=(void*)_tmp381->r;_tmp387.f2=(void*)
_tmp384->r;_tmp387;});void*_tmp389;void*_tmp38A;void*_tmp38B;void*_tmp38C;_LL20C:
_tmp389=_tmp388.f1;if(*((int*)_tmp389)!= 16)goto _LL20E;_tmp38A=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp389)->f1;_LL20D: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp38A);elt_type=_tmp38A;*t=(void**)({void**_tmp38D=_cycalloc(sizeof(*
_tmp38D));_tmp38D[0]=elt_type;_tmp38D;});num_elts=_tmp384;one_elt=0;goto _LL20B;
_LL20E: _tmp38B=_tmp388.f2;if(*((int*)_tmp38B)!= 16)goto _LL210;_tmp38C=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp38B)->f1;_LL20F: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp38C);elt_type=_tmp38C;*t=(void**)({void**_tmp38E=_cycalloc(
sizeof(*_tmp38E));_tmp38E[0]=elt_type;_tmp38E;});num_elts=_tmp381;one_elt=0;goto
_LL20B;_LL210:;_LL211: goto No_sizeof;_LL20B:;}goto _LL204;_LL209:;_LL20A: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp38F=_cycalloc(sizeof(*_tmp38F));
_tmp38F[0]=elt_type;_tmp38F;});num_elts=*e;one_elt=0;goto _LL204;_LL204:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp390=elt_type;struct Cyc_Absyn_AggrInfo _tmp391;
void*_tmp392;struct Cyc_Absyn_Aggrdecl**_tmp393;struct Cyc_Absyn_Aggrdecl*_tmp394;
_LL213: if(_tmp390 <= (void*)3?1:*((int*)_tmp390)!= 10)goto _LL215;_tmp391=((struct
Cyc_Absyn_AggrType_struct*)_tmp390)->f1;_tmp392=(void*)_tmp391.aggr_info;if(*((
int*)_tmp392)!= 1)goto _LL215;_tmp393=((struct Cyc_Absyn_KnownAggr_struct*)_tmp392)->f1;
_tmp394=*_tmp393;_LL214: if(_tmp394->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp394->impl))->exist_vars != 0: 0)({void*_tmp395[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp396="malloc with existential types not yet implemented";
_tag_arr(_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,50));}),_tag_arr(
_tmp395,sizeof(void*),0));});goto _LL212;_LL215:;_LL216: goto _LL212;_LL212:;}{void*(*
_tmp397)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp398=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp399=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp39A;struct Cyc_Absyn_PtrAtts _tmp39B;struct Cyc_Absyn_Conref*_tmp39C;struct Cyc_Absyn_Conref*
_tmp39D;_LL218: if(_tmp399 <= (void*)3?1:*((int*)_tmp399)!= 4)goto _LL21A;_tmp39A=((
struct Cyc_Absyn_PointerType_struct*)_tmp399)->f1;_tmp39B=_tmp39A.ptr_atts;
_tmp39C=_tmp39B.nullable;_tmp39D=_tmp39B.zero_term;_LL219: _tmp398=_tmp39D;if(((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp39C))_tmp397=Cyc_Absyn_star_typ;
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp39D)?!(*
is_calloc): 0){({void*_tmp39E[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp39F="converting malloc to calloc to ensure zero-termination";
_tag_arr(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,55));}),_tag_arr(
_tmp39E,sizeof(void*),0));});*is_calloc=1;}goto _LL217;_LL21A:;_LL21B: goto _LL217;
_LL217:;}if(!one_elt)_tmp397=Cyc_Absyn_tagged_typ;return _tmp397(elt_type,rgn,Cyc_Absyn_empty_tqual(),
_tmp398);}}}static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp3A0=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Stmt*
_tmp3A2;struct Cyc_Absyn_Stmt*_tmp3A3;struct Cyc_Absyn_Decl*_tmp3A4;struct Cyc_Absyn_Stmt*
_tmp3A5;_LL21D: if(_tmp3A0 <= (void*)1?1:*((int*)_tmp3A0)!= 0)goto _LL21F;_tmp3A1=((
struct Cyc_Absyn_Exp_s_struct*)_tmp3A0)->f1;_LL21E: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A1->topt))->v;_LL21F: if(_tmp3A0 <= (void*)1?1:*((int*)_tmp3A0)!= 
1)goto _LL221;_tmp3A2=((struct Cyc_Absyn_Seq_s_struct*)_tmp3A0)->f1;_tmp3A3=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3A0)->f2;_LL220: s=_tmp3A3;continue;_LL221: if(
_tmp3A0 <= (void*)1?1:*((int*)_tmp3A0)!= 12)goto _LL223;_tmp3A4=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3A0)->f1;_tmp3A5=((struct Cyc_Absyn_Decl_s_struct*)_tmp3A0)->f2;_LL222: s=
_tmp3A5;continue;_LL223:;_LL224: return({void*_tmp3A6[0]={};Cyc_Tcexp_expr_err(te,
loc,({const char*_tmp3A7="statement expression must end with expression";_tag_arr(
_tmp3A7,sizeof(char),_get_zero_arr_size(_tmp3A7,46));}),_tag_arr(_tmp3A6,sizeof(
void*),0));});_LL21C:;}}static void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*fd){return({
void*_tmp3A8[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp3A9="tcCodegen";
_tag_arr(_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,10));}),_tag_arr(
_tmp3A8,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*
_tmp3AA[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp3AB="tcFill";_tag_arr(
_tmp3AB,sizeof(char),_get_zero_arr_size(_tmp3AB,7));}),_tag_arr(_tmp3AA,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3B2;_tmp3B2.tag=15;_tmp3B2.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3B3;}));_tmp3B2;});_tmp3B1;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp3AC=Cyc_Tcutil_compress(handle_type);void*
_tmp3AD;_LL226: if(_tmp3AC <= (void*)3?1:*((int*)_tmp3AC)!= 15)goto _LL228;_tmp3AD=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3AC)->f1;_LL227: rgn=_tmp3AD;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL225;_LL228:;_LL229:({struct Cyc_String_pa_struct
_tmp3B0;_tmp3B0.tag=0;_tmp3B0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3AE[1]={& _tmp3B0};Cyc_Tcutil_terr(rgn_handle->loc,({
const char*_tmp3AF="expecting region_t type but found %s";_tag_arr(_tmp3AF,
sizeof(char),_get_zero_arr_size(_tmp3AF,37));}),_tag_arr(_tmp3AE,sizeof(void*),1));}});
goto _LL225;_LL225:;}{void*_tmp3B4=(void*)e1->r;struct Cyc_Core_Opt*_tmp3B5;struct
Cyc_List_List*_tmp3B6;struct Cyc_List_List*_tmp3B7;void*_tmp3B8;struct _tagged_arr
_tmp3B9;_LL22B: if(*((int*)_tmp3B4)!= 27)goto _LL22D;_LL22C: {void*_tmp3BA=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp3BB=Cyc_Tcutil_compress(_tmp3BA);struct Cyc_Absyn_ArrayInfo
_tmp3BC;void*_tmp3BD;struct Cyc_Absyn_Tqual _tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;
struct Cyc_Absyn_Conref*_tmp3C0;_LL236: if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 
7)goto _LL238;_tmp3BC=((struct Cyc_Absyn_ArrayType_struct*)_tmp3BB)->f1;_tmp3BD=(
void*)_tmp3BC.elt_type;_tmp3BE=_tmp3BC.tq;_tmp3BF=_tmp3BC.num_elts;_tmp3C0=
_tmp3BC.zero_term;_LL237: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp3BF);void*b=Cyc_Tcutil_is_const_exp(te,bnd)?(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3C7;_tmp3C7.tag=0;_tmp3C7.f1=bnd;_tmp3C7;});_tmp3C6;}):(void*)0;void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));
_tmp3C2[0]=({struct Cyc_Absyn_PointerType_struct _tmp3C3;_tmp3C3.tag=4;_tmp3C3.f1=({
struct Cyc_Absyn_PtrInfo _tmp3C4;_tmp3C4.elt_typ=(void*)_tmp3BD;_tmp3C4.elt_tq=
_tmp3BE;_tmp3C4.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp3C5;_tmp3C5.rgn=(void*)rgn;
_tmp3C5.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3C5.bounds=Cyc_Absyn_new_conref(b);_tmp3C5.zero_term=_tmp3C0;_tmp3C5;});
_tmp3C4;});_tmp3C3;});_tmp3C2;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?
Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->v=(void*)res_typ;_tmp3C1;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}_LL238:;_LL239:({void*_tmp3C8[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp3C9="tcNew: comprehension returned non-array type";_tag_arr(_tmp3C9,sizeof(
char),_get_zero_arr_size(_tmp3C9,45));}),_tag_arr(_tmp3C8,sizeof(void*),0));});
_LL235:;}_LL22D: if(*((int*)_tmp3B4)!= 34)goto _LL22F;_tmp3B5=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp3B4)->f1;_tmp3B6=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3B4)->f2;
_LL22E:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp3CA=
_cycalloc(sizeof(*_tmp3CA));_tmp3CA[0]=({struct Cyc_Absyn_Array_e_struct _tmp3CB;
_tmp3CB.tag=26;_tmp3CB.f1=_tmp3B6;_tmp3CB;});_tmp3CA;})));_tmp3B7=_tmp3B6;goto
_LL230;_LL22F: if(*((int*)_tmp3B4)!= 26)goto _LL231;_tmp3B7=((struct Cyc_Absyn_Array_e_struct*)
_tmp3B4)->f1;_LL230: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*
_tmp3CC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3CD;void*_tmp3CE;
void**_tmp3CF;struct Cyc_Absyn_Tqual _tmp3D0;struct Cyc_Absyn_PtrAtts _tmp3D1;struct
Cyc_Absyn_Conref*_tmp3D2;_LL23B: if(_tmp3CC <= (void*)3?1:*((int*)_tmp3CC)!= 4)
goto _LL23D;_tmp3CD=((struct Cyc_Absyn_PointerType_struct*)_tmp3CC)->f1;_tmp3CE=(
void*)_tmp3CD.elt_typ;_tmp3CF=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3CC)->f1).elt_typ;_tmp3D0=_tmp3CD.elt_tq;_tmp3D1=_tmp3CD.ptr_atts;_tmp3D2=
_tmp3D1.zero_term;_LL23C: elt_typ_opt=(void**)_tmp3CF;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3D2);goto _LL23A;_LL23D:;
_LL23E: goto _LL23A;_LL23A:;}{void*_tmp3D3=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp3B7);e1->topt=({struct Cyc_Core_Opt*_tmp3D4=_cycalloc(
sizeof(*_tmp3D4));_tmp3D4->v=(void*)_tmp3D3;_tmp3D4;});{void*res_typ;{void*
_tmp3D5=Cyc_Tcutil_compress(_tmp3D3);struct Cyc_Absyn_ArrayInfo _tmp3D6;void*
_tmp3D7;struct Cyc_Absyn_Tqual _tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Conref*
_tmp3DA;_LL240: if(_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 7)goto _LL242;_tmp3D6=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3D5)->f1;_tmp3D7=(void*)_tmp3D6.elt_type;
_tmp3D8=_tmp3D6.tq;_tmp3D9=_tmp3D6.num_elts;_tmp3DA=_tmp3D6.zero_term;_LL241:
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3DB=_cycalloc(sizeof(*
_tmp3DB));_tmp3DB[0]=({struct Cyc_Absyn_PointerType_struct _tmp3DC;_tmp3DC.tag=4;
_tmp3DC.f1=({struct Cyc_Absyn_PtrInfo _tmp3DD;_tmp3DD.elt_typ=(void*)_tmp3D7;
_tmp3DD.elt_tq=_tmp3D8;_tmp3DD.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp3DE;_tmp3DE.rgn=(
void*)rgn;_tmp3DE.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3DE.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3E0;_tmp3E0.tag=0;_tmp3E0.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3D9);
_tmp3E0;});_tmp3DF;}));_tmp3DE.zero_term=_tmp3DA;_tmp3DE;});_tmp3DD;});_tmp3DC;});
_tmp3DB;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp3E1=_cycalloc(sizeof(*
_tmp3E1));_tmp3E1->v=(void*)res_typ;_tmp3E1;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL23F;_LL242:;_LL243:({void*_tmp3E2[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3E3="tcExpNoPromote on Array_e returned non-array type";
_tag_arr(_tmp3E3,sizeof(char),_get_zero_arr_size(_tmp3E3,50));}),_tag_arr(
_tmp3E2,sizeof(void*),0));});_LL23F:;}return res_typ;}}}_LL231: if(*((int*)_tmp3B4)
!= 0)goto _LL233;_tmp3B8=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp3B4)->f1;
if(_tmp3B8 <= (void*)1?1:*((int*)_tmp3B8)!= 5)goto _LL233;_tmp3B9=((struct Cyc_Absyn_String_c_struct*)
_tmp3B8)->f1;_LL232: {void*_tmp3E4=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(
void*)0,Cyc_Absyn_true_conref);void*_tmp3E5=Cyc_Tcexp_tcExp(te,(void**)& _tmp3E4,
e1);return Cyc_Absyn_atb_typ(_tmp3E5,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=Cyc_Absyn_uint_exp(1,0);
_tmp3E7;});_tmp3E6;}),Cyc_Absyn_false_conref);}_LL233:;_LL234: {void**topt2=0;
if(topt != 0){void*_tmp3E8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3E9;void*_tmp3EA;void**_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EC;_LL245: if(
_tmp3E8 <= (void*)3?1:*((int*)_tmp3E8)!= 4)goto _LL247;_tmp3E9=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E8)->f1;_tmp3EA=(void*)_tmp3E9.elt_typ;_tmp3EB=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3E8)->f1).elt_typ;_tmp3EC=_tmp3E9.elt_tq;_LL246: topt2=(void**)_tmp3EB;goto
_LL244;_LL247: if(_tmp3E8 <= (void*)3?1:*((int*)_tmp3E8)!= 2)goto _LL249;_LL248:
topt2=({void**_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=*topt;_tmp3ED;});
goto _LL244;_LL249:;_LL24A: goto _LL244;_LL244:;}{void*telt=Cyc_Tcexp_tcExp(te,
topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3EF=
_cycalloc(sizeof(*_tmp3EF));_tmp3EF[0]=({struct Cyc_Absyn_PointerType_struct
_tmp3F0;_tmp3F0.tag=4;_tmp3F0.f1=({struct Cyc_Absyn_PtrInfo _tmp3F1;_tmp3F1.elt_typ=(
void*)telt;_tmp3F1.elt_tq=Cyc_Absyn_empty_tqual();_tmp3F1.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp3F2;_tmp3F2.rgn=(void*)rgn;_tmp3F2.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp3F2.bounds=Cyc_Absyn_bounds_one_conref;_tmp3F2.zero_term=
Cyc_Absyn_false_conref;_tmp3F2;});_tmp3F1;});_tmp3F0;});_tmp3EF;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));
_tmp3EE->v=(void*)res_typ;_tmp3EE;});Cyc_Tcutil_unchecked_cast(te,e,*topt);
res_typ=*topt;}}return res_typ;}}_LL22A:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp3F3=t;struct Cyc_Absyn_ArrayInfo _tmp3F4;void*_tmp3F5;struct
Cyc_Absyn_Tqual _tmp3F6;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Conref*
_tmp3F8;_LL24C: if(_tmp3F3 <= (void*)3?1:*((int*)_tmp3F3)!= 7)goto _LL24E;_tmp3F4=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F3)->f1;_tmp3F5=(void*)_tmp3F4.elt_type;
_tmp3F6=_tmp3F4.tq;_tmp3F7=_tmp3F4.num_elts;_tmp3F8=_tmp3F4.zero_term;_LL24D: {
void*_tmp3FA;struct _tuple6 _tmp3F9=Cyc_Tcutil_addressof_props(te,e);_tmp3FA=
_tmp3F9.f2;{void*_tmp3FB=_tmp3F7 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3FD;_tmp3FD.tag=0;_tmp3FD.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3F7);
_tmp3FD;});_tmp3FC;});t=Cyc_Absyn_atb_typ(_tmp3F5,_tmp3FA,_tmp3F6,_tmp3FB,
_tmp3F8);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return
t;}}_LL24E:;_LL24F: return t;_LL24B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
void*_tmp3FE=(void*)e->r;void*_tmp3FF;_LL251: if(*((int*)_tmp3FE)!= 26)goto _LL253;
_LL252: goto _LL254;_LL253: if(*((int*)_tmp3FE)!= 27)goto _LL255;_LL254: goto _LL256;
_LL255: if(*((int*)_tmp3FE)!= 0)goto _LL257;_tmp3FF=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp3FE)->f1;if(_tmp3FF <= (void*)1?1:*((int*)_tmp3FF)!= 5)goto _LL257;_LL256:
return t;_LL257:;_LL258: t=Cyc_Tcutil_compress(t);{void*_tmp400=t;struct Cyc_Absyn_ArrayInfo
_tmp401;void*_tmp402;struct Cyc_Absyn_Tqual _tmp403;struct Cyc_Absyn_Exp*_tmp404;
struct Cyc_Absyn_Conref*_tmp405;_LL25A: if(_tmp400 <= (void*)3?1:*((int*)_tmp400)!= 
7)goto _LL25C;_tmp401=((struct Cyc_Absyn_ArrayType_struct*)_tmp400)->f1;_tmp402=(
void*)_tmp401.elt_type;_tmp403=_tmp401.tq;_tmp404=_tmp401.num_elts;_tmp405=
_tmp401.zero_term;_LL25B: {void*_tmp407;struct _tuple6 _tmp406=Cyc_Tcutil_addressof_props(
te,e);_tmp407=_tmp406.f2;{void*b=_tmp404 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp409;_tmp409.tag=0;_tmp409.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp404);
_tmp409;});_tmp408;});t=Cyc_Absyn_atb_typ(_tmp402,_tmp407,_tmp403,b,_tmp405);Cyc_Tcutil_unchecked_cast(
te,e,t);return t;}}_LL25C:;_LL25D: return t;_LL259:;}_LL250:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp40A=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp40B;_LL25F: if(*((int*)_tmp40A)!= 11)goto _LL261;_tmp40B=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp40A)->f1;_LL260: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp40B);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp40B->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp40B->topt))->v));
e->topt=_tmp40B->topt;goto _LL25E;_LL261:;_LL262: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp40C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp40D;void*_tmp40E;struct Cyc_Absyn_Tqual _tmp40F;struct Cyc_Absyn_PtrAtts _tmp410;
void*_tmp411;struct Cyc_Absyn_Conref*_tmp412;struct Cyc_Absyn_Conref*_tmp413;
struct Cyc_Absyn_Conref*_tmp414;_LL264: if(_tmp40C <= (void*)3?1:*((int*)_tmp40C)!= 
4)goto _LL266;_tmp40D=((struct Cyc_Absyn_PointerType_struct*)_tmp40C)->f1;_tmp40E=(
void*)_tmp40D.elt_typ;_tmp40F=_tmp40D.elt_tq;_tmp410=_tmp40D.ptr_atts;_tmp411=(
void*)_tmp410.rgn;_tmp412=_tmp410.nullable;_tmp413=_tmp410.bounds;_tmp414=
_tmp410.zero_term;_LL265:{void*_tmp415=Cyc_Tcutil_compress(_tmp40E);struct Cyc_Absyn_FnInfo
_tmp416;struct Cyc_List_List*_tmp417;struct Cyc_Core_Opt*_tmp418;void*_tmp419;
struct Cyc_List_List*_tmp41A;int _tmp41B;struct Cyc_Absyn_VarargInfo*_tmp41C;struct
Cyc_List_List*_tmp41D;struct Cyc_List_List*_tmp41E;_LL269: if(_tmp415 <= (void*)3?1:*((
int*)_tmp415)!= 8)goto _LL26B;_tmp416=((struct Cyc_Absyn_FnType_struct*)_tmp415)->f1;
_tmp417=_tmp416.tvars;_tmp418=_tmp416.effect;_tmp419=(void*)_tmp416.ret_typ;
_tmp41A=_tmp416.args;_tmp41B=_tmp416.c_varargs;_tmp41C=_tmp416.cyc_varargs;
_tmp41D=_tmp416.rgn_po;_tmp41E=_tmp416.attributes;_LL26A: if(_tmp417 != 0){struct
_RegionHandle _tmp41F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp41F;
_push_region(rgn);{struct _tuple4 _tmp420=({struct _tuple4 _tmp42D;_tmp42D.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp42D.f2=rgn;_tmp42D;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp420,_tmp417);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A[0]=({struct Cyc_Absyn_FnType_struct
_tmp42B;_tmp42B.tag=8;_tmp42B.f1=({struct Cyc_Absyn_FnInfo _tmp42C;_tmp42C.tvars=0;
_tmp42C.effect=_tmp418;_tmp42C.ret_typ=(void*)_tmp419;_tmp42C.args=_tmp41A;
_tmp42C.c_varargs=_tmp41B;_tmp42C.cyc_varargs=_tmp41C;_tmp42C.rgn_po=_tmp41D;
_tmp42C.attributes=_tmp41E;_tmp42C;});_tmp42B;});_tmp42A;}));struct Cyc_Absyn_PointerType_struct*
_tmp421=({struct Cyc_Absyn_PointerType_struct*_tmp426=_cycalloc(sizeof(*_tmp426));
_tmp426[0]=({struct Cyc_Absyn_PointerType_struct _tmp427;_tmp427.tag=4;_tmp427.f1=({
struct Cyc_Absyn_PtrInfo _tmp428;_tmp428.elt_typ=(void*)ftyp;_tmp428.elt_tq=
_tmp40F;_tmp428.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp429;_tmp429.rgn=(void*)
_tmp411;_tmp429.nullable=_tmp412;_tmp429.bounds=_tmp413;_tmp429.zero_term=
_tmp414;_tmp429;});_tmp428;});_tmp427;});_tmp426;});struct Cyc_Absyn_Exp*_tmp422=
Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp424;_tmp424.tag=12;_tmp424.f1=_tmp422;_tmp424.f2=ts;_tmp424;});_tmp423;})));
e->topt=({struct Cyc_Core_Opt*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->v=(
void*)((void*)_tmp421);_tmp425;});};_pop_region(rgn);}goto _LL268;_LL26B:;_LL26C:
goto _LL268;_LL268:;}goto _LL263;_LL266:;_LL267: goto _LL263;_LL263:;}goto _LL25E;
_LL25E:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp42E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp42F;struct _tuple1*_tmp430;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_List_List*
_tmp432;struct Cyc_Core_Opt*_tmp433;struct Cyc_List_List*_tmp434;void*_tmp435;
struct _tuple1*_tmp436;void*_tmp437;void*_tmp438;struct Cyc_List_List*_tmp439;
struct Cyc_Absyn_Exp*_tmp43A;void*_tmp43B;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Core_Opt*
_tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Exp*
_tmp440;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Exp*
_tmp443;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_List_List*_tmp445;struct Cyc_Absyn_VarargCallInfo*
_tmp446;struct Cyc_Absyn_VarargCallInfo**_tmp447;struct Cyc_Absyn_Exp*_tmp448;
struct Cyc_Absyn_Exp*_tmp449;struct Cyc_List_List*_tmp44A;void*_tmp44B;struct Cyc_Absyn_Exp*
_tmp44C;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*
_tmp44F;struct Cyc_Absyn_Exp*_tmp450;void*_tmp451;void*_tmp452;void*_tmp453;
struct Cyc_Absyn_Exp*_tmp454;struct Cyc_Absyn_Exp*_tmp455;struct _tagged_arr*
_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct _tagged_arr*_tmp458;struct Cyc_Absyn_Exp*
_tmp459;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_List_List*_tmp45B;struct _tuple2*
_tmp45C;struct Cyc_List_List*_tmp45D;struct Cyc_List_List*_tmp45E;struct Cyc_Absyn_Stmt*
_tmp45F;struct Cyc_Absyn_Fndecl*_tmp460;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Vardecl*
_tmp462;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Exp*_tmp464;int _tmp465;int*
_tmp466;struct _tuple1*_tmp467;struct _tuple1**_tmp468;struct Cyc_List_List*_tmp469;
struct Cyc_List_List**_tmp46A;struct Cyc_List_List*_tmp46B;struct Cyc_Absyn_Aggrdecl*
_tmp46C;struct Cyc_Absyn_Aggrdecl**_tmp46D;void*_tmp46E;struct Cyc_List_List*
_tmp46F;struct Cyc_List_List*_tmp470;struct Cyc_Absyn_Tuniondecl*_tmp471;struct Cyc_Absyn_Tunionfield*
_tmp472;struct _tuple1*_tmp473;struct _tuple1**_tmp474;struct Cyc_Absyn_Enumdecl*
_tmp475;struct Cyc_Absyn_Enumfield*_tmp476;struct _tuple1*_tmp477;struct _tuple1**
_tmp478;void*_tmp479;struct Cyc_Absyn_Enumfield*_tmp47A;struct Cyc_Absyn_MallocInfo
_tmp47B;int _tmp47C;int*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;void**_tmp47F;void***
_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp**_tmp482;int _tmp483;int*
_tmp484;_LL26E: if(*((int*)_tmp42E)!= 11)goto _LL270;_tmp42F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp42E)->f1;_LL26F: Cyc_Tcexp_tcExpNoInst(te,0,_tmp42F);return;_LL270: if(*((int*)
_tmp42E)!= 2)goto _LL272;_tmp430=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp42E)->f1;
_LL271: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp430);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL272: if(*((int*)_tmp42E)!= 8)goto _LL274;_tmp431=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp42E)->f1;_tmp432=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp42E)->f2;_LL273:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp431,_tmp432);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL274: if(*((int*)_tmp42E)!= 34)goto _LL276;_tmp433=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp42E)->f1;_tmp434=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp42E)->f2;
_LL275: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp434);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL276: if(*((int*)_tmp42E)!= 0)goto _LL278;_tmp435=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp42E)->f1;_LL277: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp435,e);goto _LL26D;_LL278: if(*((int*)_tmp42E)!= 1)goto _LL27A;_tmp436=((
struct Cyc_Absyn_Var_e_struct*)_tmp42E)->f1;_tmp437=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp42E)->f2;_LL279: t=Cyc_Tcexp_tcVar(te,loc,_tmp436,_tmp437);goto _LL26D;_LL27A:
if(*((int*)_tmp42E)!= 3)goto _LL27C;_tmp438=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp42E)->f1;_tmp439=((struct Cyc_Absyn_Primop_e_struct*)_tmp42E)->f2;_LL27B: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp438,_tmp439);goto _LL26D;_LL27C: if(*((int*)
_tmp42E)!= 5)goto _LL27E;_tmp43A=((struct Cyc_Absyn_Increment_e_struct*)_tmp42E)->f1;
_tmp43B=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp42E)->f2;_LL27D: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp43A,_tmp43B);goto _LL26D;_LL27E: if(*((int*)_tmp42E)!= 4)goto
_LL280;_tmp43C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp42E)->f1;_tmp43D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp42E)->f2;_tmp43E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp42E)->f3;_LL27F: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp43C,_tmp43D,_tmp43E);
goto _LL26D;_LL280: if(*((int*)_tmp42E)!= 6)goto _LL282;_tmp43F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp42E)->f1;_tmp440=((struct Cyc_Absyn_Conditional_e_struct*)_tmp42E)->f2;
_tmp441=((struct Cyc_Absyn_Conditional_e_struct*)_tmp42E)->f3;_LL281: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp43F,_tmp440,_tmp441);goto _LL26D;_LL282: if(*((int*)_tmp42E)!= 7)
goto _LL284;_tmp442=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp42E)->f1;_tmp443=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp42E)->f2;_LL283: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp442,_tmp443);goto _LL26D;_LL284: if(*((int*)_tmp42E)!= 9)goto _LL286;
_tmp444=((struct Cyc_Absyn_FnCall_e_struct*)_tmp42E)->f1;_tmp445=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp42E)->f2;_tmp446=((struct Cyc_Absyn_FnCall_e_struct*)_tmp42E)->f3;_tmp447=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp42E)->f3;
_LL285: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp444,_tmp445,_tmp447);goto _LL26D;
_LL286: if(*((int*)_tmp42E)!= 10)goto _LL288;_tmp448=((struct Cyc_Absyn_Throw_e_struct*)
_tmp42E)->f1;_LL287: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp448);goto _LL26D;_LL288:
if(*((int*)_tmp42E)!= 12)goto _LL28A;_tmp449=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp42E)->f1;_tmp44A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp42E)->f2;_LL289:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp449,_tmp44A);goto _LL26D;_LL28A: if(*((
int*)_tmp42E)!= 13)goto _LL28C;_tmp44B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp42E)->f1;_tmp44C=((struct Cyc_Absyn_Cast_e_struct*)_tmp42E)->f2;_LL28B: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp44B,_tmp44C);goto _LL26D;_LL28C: if(*((int*)_tmp42E)!= 14)goto
_LL28E;_tmp44D=((struct Cyc_Absyn_Address_e_struct*)_tmp42E)->f1;_LL28D: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp44D);goto _LL26D;_LL28E: if(*((int*)_tmp42E)!= 15)goto _LL290;
_tmp44E=((struct Cyc_Absyn_New_e_struct*)_tmp42E)->f1;_tmp44F=((struct Cyc_Absyn_New_e_struct*)
_tmp42E)->f2;_LL28F: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp44E,e,_tmp44F);goto _LL26D;
_LL290: if(*((int*)_tmp42E)!= 17)goto _LL292;_tmp450=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp42E)->f1;_LL291: {void*_tmp485=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp450);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp485);goto _LL26D;}_LL292: if(*((int*)_tmp42E)!= 16)goto _LL294;
_tmp451=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp42E)->f1;_LL293: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp451);goto _LL26D;_LL294: if(*((int*)_tmp42E)!= 18)goto _LL296;
_tmp452=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp42E)->f1;_tmp453=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp42E)->f2;_LL295: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp452,_tmp453);goto _LL26D;_LL296: if(*((int*)_tmp42E)!= 19)goto _LL298;
_LL297:({void*_tmp486[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp487="gen() not in top-level initializer";
_tag_arr(_tmp487,sizeof(char),_get_zero_arr_size(_tmp487,35));}),_tag_arr(
_tmp486,sizeof(void*),0));});return;_LL298: if(*((int*)_tmp42E)!= 20)goto _LL29A;
_tmp454=((struct Cyc_Absyn_Deref_e_struct*)_tmp42E)->f1;_LL299: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp454);goto _LL26D;_LL29A: if(*((int*)_tmp42E)!= 21)goto _LL29C;
_tmp455=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp42E)->f1;_tmp456=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp42E)->f2;_LL29B: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp455,_tmp456);goto
_LL26D;_LL29C: if(*((int*)_tmp42E)!= 22)goto _LL29E;_tmp457=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp42E)->f1;_tmp458=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp42E)->f2;_LL29D: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp457,_tmp458);goto _LL26D;_LL29E: if(*((int*)
_tmp42E)!= 23)goto _LL2A0;_tmp459=((struct Cyc_Absyn_Subscript_e_struct*)_tmp42E)->f1;
_tmp45A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp42E)->f2;_LL29F: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp459,_tmp45A);goto _LL26D;_LL2A0: if(*((int*)_tmp42E)!= 24)goto
_LL2A2;_tmp45B=((struct Cyc_Absyn_Tuple_e_struct*)_tmp42E)->f1;_LL2A1: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp45B);goto _LL26D;_LL2A2: if(*((int*)_tmp42E)!= 25)goto _LL2A4;
_tmp45C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp42E)->f1;_tmp45D=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp42E)->f2;_LL2A3: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp45C,_tmp45D);goto
_LL26D;_LL2A4: if(*((int*)_tmp42E)!= 26)goto _LL2A6;_tmp45E=((struct Cyc_Absyn_Array_e_struct*)
_tmp42E)->f1;_LL2A5: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp488=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp489;void*_tmp48A;void**
_tmp48B;struct Cyc_Absyn_Conref*_tmp48C;_LL2BB: if(_tmp488 <= (void*)3?1:*((int*)
_tmp488)!= 7)goto _LL2BD;_tmp489=((struct Cyc_Absyn_ArrayType_struct*)_tmp488)->f1;
_tmp48A=(void*)_tmp489.elt_type;_tmp48B=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp488)->f1).elt_type;_tmp48C=_tmp489.zero_term;_LL2BC: elt_topt=(void**)_tmp48B;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp48C);
goto _LL2BA;_LL2BD:;_LL2BE: goto _LL2BA;_LL2BA:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp45E);goto _LL26D;}_LL2A6: if(*((int*)_tmp42E)!= 35)goto
_LL2A8;_tmp45F=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp42E)->f1;_LL2A7: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp45F);goto _LL26D;_LL2A8: if(*((int*)_tmp42E)!= 36)goto _LL2AA;
_tmp460=((struct Cyc_Absyn_Codegen_e_struct*)_tmp42E)->f1;_LL2A9: t=Cyc_Tcexp_tcCodegen(
te,loc,topt,_tmp460);goto _LL26D;_LL2AA: if(*((int*)_tmp42E)!= 37)goto _LL2AC;
_tmp461=((struct Cyc_Absyn_Fill_e_struct*)_tmp42E)->f1;_LL2AB: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp461);goto _LL26D;_LL2AC: if(*((int*)_tmp42E)!= 27)goto _LL2AE;
_tmp462=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp42E)->f1;_tmp463=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp42E)->f2;_tmp464=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp42E)->f3;_tmp465=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp42E)->f4;
_tmp466=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp42E)->f4;_LL2AD: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp462,_tmp463,_tmp464,_tmp466);goto
_LL26D;_LL2AE: if(*((int*)_tmp42E)!= 28)goto _LL2B0;_tmp467=((struct Cyc_Absyn_Struct_e_struct*)
_tmp42E)->f1;_tmp468=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp42E)->f1;_tmp469=((struct Cyc_Absyn_Struct_e_struct*)_tmp42E)->f2;_tmp46A=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp42E)->f2;_tmp46B=((
struct Cyc_Absyn_Struct_e_struct*)_tmp42E)->f3;_tmp46C=((struct Cyc_Absyn_Struct_e_struct*)
_tmp42E)->f4;_tmp46D=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp42E)->f4;_LL2AF: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp468,_tmp46A,_tmp46B,
_tmp46D);goto _LL26D;_LL2B0: if(*((int*)_tmp42E)!= 29)goto _LL2B2;_tmp46E=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp42E)->f1;_tmp46F=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp42E)->f2;_LL2B1: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp46E,_tmp46F);goto _LL26D;
_LL2B2: if(*((int*)_tmp42E)!= 30)goto _LL2B4;_tmp470=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp42E)->f1;_tmp471=((struct Cyc_Absyn_Tunion_e_struct*)_tmp42E)->f2;_tmp472=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp42E)->f3;_LL2B3: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp470,_tmp471,_tmp472);goto _LL26D;_LL2B4: if(*((int*)_tmp42E)!= 31)goto
_LL2B6;_tmp473=((struct Cyc_Absyn_Enum_e_struct*)_tmp42E)->f1;_tmp474=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp42E)->f1;_tmp475=((struct Cyc_Absyn_Enum_e_struct*)
_tmp42E)->f2;_tmp476=((struct Cyc_Absyn_Enum_e_struct*)_tmp42E)->f3;_LL2B5:*
_tmp474=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp476))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({
struct Cyc_Absyn_EnumType_struct _tmp48E;_tmp48E.tag=12;_tmp48E.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp475))->name;_tmp48E.f2=_tmp475;_tmp48E;});_tmp48D;});goto _LL26D;
_LL2B6: if(*((int*)_tmp42E)!= 32)goto _LL2B8;_tmp477=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp42E)->f1;_tmp478=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp42E)->f1;_tmp479=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp42E)->f2;
_tmp47A=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp42E)->f3;_LL2B7:*_tmp478=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp47A))->name;t=_tmp479;goto _LL26D;
_LL2B8: if(*((int*)_tmp42E)!= 33)goto _LL26D;_tmp47B=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp42E)->f1;_tmp47C=_tmp47B.is_calloc;_tmp47D=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp42E)->f1).is_calloc;_tmp47E=_tmp47B.rgn;_tmp47F=_tmp47B.elt_type;_tmp480=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp42E)->f1).elt_type;_tmp481=
_tmp47B.num_elts;_tmp482=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp42E)->f1).num_elts;_tmp483=_tmp47B.fat_result;_tmp484=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp42E)->f1).fat_result;_LL2B9: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp47E,_tmp480,
_tmp482,_tmp47D,_tmp484);goto _LL26D;_LL26D:;}e->topt=({struct Cyc_Core_Opt*
_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->v=(void*)t;_tmp48F;});}
