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
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
Cyc_Absyn_false_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*
tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_exn_typ;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(void*);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{
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
Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});
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
_tmp13;})));goto _LL0;_LL9: if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct
_tmp17;_tmp17.tag=0;_tmp17.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
q));{void*_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
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
_tmp3D;_tmp3D.tag=26;_tmp3D.f1=des;_tmp3D;});_tmp3C;})));return;}{void*t=*((void**)
topt);void*_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3F;void*
_tmp40;struct Cyc_Absyn_ArrayInfo _tmp41;void*_tmp42;struct Cyc_Absyn_Tqual _tmp43;
_LL26: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 10)goto _LL28;_tmp3F=((struct Cyc_Absyn_AggrType_struct*)
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
else{void*_tmp5E=Cyc_Tcutil_compress(*((void**)topt));void*_tmp5F;void*_tmp60;
void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;_LL47: if(_tmp5E <= (void*)3?1:*((
int*)_tmp5E)!= 5)goto _LL49;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E)->f1;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)
_tmp60 != 0)goto _LL49;_LL48:{void*_tmp65=_tmp5F;_LL54: if((int)_tmp65 != 1)goto
_LL56;_LL55: t=Cyc_Absyn_uchar_typ;goto _LL53;_LL56: if((int)_tmp65 != 0)goto _LL58;
_LL57: t=Cyc_Absyn_schar_typ;goto _LL53;_LL58: if((int)_tmp65 != 2)goto _LL53;_LL59: t=
Cyc_Absyn_char_typ;goto _LL53;_LL53:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
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
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp75=_cycalloc(sizeof(*
_tmp75));_tmp75->v=_tmp70;_tmp75;}));_tmp73.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp74=_cycalloc(sizeof(*
_tmp74));_tmp74->v=_tmp70;_tmp74;}));_tmp73.nullable=Cyc_Absyn_true_conref;
_tmp73.tq=Cyc_Absyn_empty_tqual();_tmp73.bounds=Cyc_Absyn_empty_conref();_tmp73.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp73;});_tmp72;});
_tmp71;});goto _LL46;}_LL4F: if(_tmp5E <= (void*)3?1:*((int*)_tmp5E)!= 17)goto _LL51;
_tmp64=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5E)->f1;_LL50: {struct Cyc_Absyn_TypeInt_struct*
_tmp76=({struct Cyc_Absyn_TypeInt_struct*_tmp7D=_cycalloc_atomic(sizeof(*_tmp7D));
_tmp7D[0]=({struct Cyc_Absyn_TypeInt_struct _tmp7E;_tmp7E.tag=18;_tmp7E.f1=_tmp5C;
_tmp7E;});_tmp7D;});if(!Cyc_Tcutil_unify(_tmp64,(void*)_tmp76)){({struct Cyc_String_pa_struct
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp76));{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp64));{void*_tmp77[2]={&
_tmp79,& _tmp7A};Cyc_Tcutil_terr(loc,({const char*_tmp78="expecting %s but found %s";
_tag_arr(_tmp78,sizeof(char),_get_zero_arr_size(_tmp78,26));}),_tag_arr(_tmp77,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_TagType_struct
_tmp7C;_tmp7C.tag=17;_tmp7C.f1=(void*)((void*)_tmp76);_tmp7C;});_tmp7B;});goto
_LL46;}_LL51:;_LL52: t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}goto _LL33;_LL42: if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 5)
goto _LL44;_tmp5D=((struct Cyc_Absyn_String_c_struct*)_tmp55)->f1;_LL43: {int len=(
int)_get_arr_size(_tmp5D,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({
struct Cyc_Absyn_Int_c_struct _tmp8B;_tmp8B.tag=2;_tmp8B.f1=(void*)((void*)1);
_tmp8B.f2=len;_tmp8B;});_tmp8A;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp7F=
_cycalloc(sizeof(*_tmp7F));_tmp7F->v=(void*)Cyc_Absyn_uint_typ;_tmp7F;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp81;_tmp81.tag=0;_tmp81.f1=elen;_tmp81;});_tmp80;}),Cyc_Absyn_true_conref);
if(topt != 0){void*_tmp82=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_ArrayInfo
_tmp83;struct Cyc_Absyn_Tqual _tmp84;_LL5B: if(_tmp82 <= (void*)3?1:*((int*)_tmp82)
!= 7)goto _LL5D;_tmp83=((struct Cyc_Absyn_ArrayType_struct*)_tmp82)->f1;_tmp84=
_tmp83.tq;_LL5C: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp84,(struct Cyc_Absyn_Exp*)
elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)());_LL5D: if(_tmp82 <= (
void*)3?1:*((int*)_tmp82)!= 4)goto _LL5F;_LL5E: if(!Cyc_Tcutil_unify(*((void**)
topt),t)?Cyc_Tcutil_silent_castable(te,loc,t,*((void**)topt)): 0){e->topt=({
struct Cyc_Core_Opt*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->v=(void*)t;_tmp85;});
Cyc_Tcutil_unchecked_cast(te,e,*((void**)topt));t=*((void**)topt);}else{t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp86;})),Cyc_Absyn_const_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp88;_tmp88.tag=0;_tmp88.f1=elen;_tmp88;});_tmp87;}),Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*((void**)topt),t)?Cyc_Tcutil_silent_castable(te,loc,t,*((
void**)topt)): 0){e->topt=({struct Cyc_Core_Opt*_tmp89=_cycalloc(sizeof(*_tmp89));
_tmp89->v=(void*)t;_tmp89;});Cyc_Tcutil_unchecked_cast(te,e,*((void**)topt));t=*((
void**)topt);}}goto _LL5A;_LL5F:;_LL60: goto _LL5A;_LL5A:;}return t;}_LL44: if((int)
_tmp55 != 0)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8C=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp8D=_cycalloc(sizeof(*
_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_PointerType_struct _tmp8E;_tmp8E.tag=4;
_tmp8E.f1=({struct Cyc_Absyn_PtrInfo _tmp8F;_tmp8F.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp91=_cycalloc(sizeof(*
_tmp91));_tmp91->v=_tmp8C;_tmp91;}));_tmp8F.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90->v=_tmp8C;_tmp90;}));_tmp8F.nullable=Cyc_Absyn_true_conref;
_tmp8F.tq=Cyc_Absyn_empty_tqual();_tmp8F.bounds=Cyc_Absyn_empty_conref();_tmp8F.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp8F;});_tmp8E;});
_tmp8D;});goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp92=b;struct
Cyc_Absyn_Vardecl*_tmp93;struct Cyc_Absyn_Fndecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;struct Cyc_Absyn_Vardecl*_tmp97;_LL62: if((
int)_tmp92 != 0)goto _LL64;_LL63: return({struct Cyc_String_pa_struct _tmp9A;_tmp9A.tag=
0;_tmp9A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(q));{
void*_tmp98[1]={& _tmp9A};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp99="undeclared identifier: %s";
_tag_arr(_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,26));}),_tag_arr(_tmp98,
sizeof(void*),1));}});_LL64: if(_tmp92 <= (void*)1?1:*((int*)_tmp92)!= 0)goto _LL66;
_tmp93=((struct Cyc_Absyn_Global_b_struct*)_tmp92)->f1;_LL65:*((struct _tuple1*)q)=*((
struct _tuple1*)_tmp93->name);return(void*)_tmp93->type;_LL66: if(_tmp92 <= (void*)
1?1:*((int*)_tmp92)!= 1)goto _LL68;_tmp94=((struct Cyc_Absyn_Funname_b_struct*)
_tmp92)->f1;_LL67:*((struct _tuple1*)q)=*((struct _tuple1*)_tmp94->name);return Cyc_Tcutil_fndecl2typ(
_tmp94);_LL68: if(_tmp92 <= (void*)1?1:*((int*)_tmp92)!= 4)goto _LL6A;_tmp95=((
struct Cyc_Absyn_Pat_b_struct*)_tmp92)->f1;_LL69: _tmp96=_tmp95;goto _LL6B;_LL6A:
if(_tmp92 <= (void*)1?1:*((int*)_tmp92)!= 3)goto _LL6C;_tmp96=((struct Cyc_Absyn_Local_b_struct*)
_tmp92)->f1;_LL6B: _tmp97=_tmp96;goto _LL6D;_LL6C: if(_tmp92 <= (void*)1?1:*((int*)
_tmp92)!= 2)goto _LL61;_tmp97=((struct Cyc_Absyn_Param_b_struct*)_tmp92)->f1;_LL6D:(*((
struct _tuple1*)q)).f1=(void*)0;return(void*)_tmp97->type;_LL61:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmp9B=(void*)
fmt->r;void*_tmp9C;struct _tagged_arr _tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp
_tmp9F;void*_tmpA0;void*_tmpA1;struct _tagged_arr _tmpA2;_LL6F: if(*((int*)_tmp9B)
!= 0)goto _LL71;_tmp9C=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp9B)->f1;if(
_tmp9C <= (void*)1?1:*((int*)_tmp9C)!= 5)goto _LL71;_tmp9D=((struct Cyc_Absyn_String_c_struct*)
_tmp9C)->f1;_LL70: _tmpA2=_tmp9D;goto _LL72;_LL71: if(*((int*)_tmp9B)!= 13)goto
_LL73;_tmp9E=((struct Cyc_Absyn_Cast_e_struct*)_tmp9B)->f2;_tmp9F=*_tmp9E;_tmpA0=(
void*)_tmp9F.r;if(*((int*)_tmpA0)!= 0)goto _LL73;_tmpA1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpA0)->f1;if(_tmpA1 <= (void*)1?1:*((int*)_tmpA1)!= 5)goto _LL73;_tmpA2=((struct
Cyc_Absyn_String_c_struct*)_tmpA1)->f1;_LL72: desc_types=type_getter(te,(struct
_tagged_arr)_tmpA2,fmt->loc);goto _LL6E;_LL73:;_LL74: if(opt_args != 0){struct Cyc_List_List*
_tmpA3=(struct Cyc_List_List*)opt_args->v;for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpA3->hd);}}return;_LL6E:;}if(opt_args != 0){struct
Cyc_List_List*_tmpA4=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 0?
_tmpA4 != 0: 0;(desc_types=desc_types->tl,_tmpA4=_tmpA4->tl)){void*t=(void*)
desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpA4->hd;Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpA7;_tmpA7.tag=0;_tmpA7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpA5[2]={& _tmpA7,& _tmpA8};Cyc_Tcutil_terr(e->loc,({const char*_tmpA6="descriptor has type \n%s\n but argument has type \n%s";
_tag_arr(_tmpA6,sizeof(char),_get_zero_arr_size(_tmpA6,51));}),_tag_arr(_tmpA5,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(desc_types != 0)({void*
_tmpA9[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpAA="too few arguments";
_tag_arr(_tmpAA,sizeof(char),_get_zero_arr_size(_tmpAA,18));}),_tag_arr(_tmpA9,
sizeof(void*),0));});if(_tmpA4 != 0)({void*_tmpAB[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA4->hd)->loc,({const char*_tmpAC="too many arguments";_tag_arr(
_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,19));}),_tag_arr(_tmpAB,sizeof(
void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpAD=p;_LL76: if((
int)_tmpAD != 0)goto _LL78;_LL77: goto _LL79;_LL78: if((int)_tmpAD != 2)goto _LL7A;
_LL79: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_String_pa_struct _tmpB0;_tmpB0.tag=
0;_tmpB0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmpAE[1]={& _tmpB0};Cyc_Tcutil_terr(loc,({const char*_tmpAF="expecting arithmetic type but found %s";
_tag_arr(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,39));}),_tag_arr(_tmpAE,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7A: if((int)_tmpAD != 11)goto _LL7C;_LL7B: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB3;_tmpB3.tag=
0;_tmpB3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmpB1[1]={& _tmpB3};Cyc_Tcutil_terr(loc,({const char*_tmpB2="expecting integral or * type but found %s";
_tag_arr(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,42));}),_tag_arr(_tmpB1,
sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7C: if((int)_tmpAD != 12)goto
_LL7E;_LL7D: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct _tmpB6;
_tmpB6.tag=0;_tmpB6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpB4[1]={& _tmpB6};Cyc_Tcutil_terr(loc,({const char*_tmpB5="expecting integral type but found %s";
_tag_arr(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,37));}),_tag_arr(_tmpB4,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7E: if((int)_tmpAD != 19)goto _LL80;_LL7F:{void*_tmpB7=t;_LL83: if(_tmpB7 <= (void*)
3?1:*((int*)_tmpB7)!= 7)goto _LL85;_LL84: goto _LL86;_LL85: if(_tmpB7 <= (void*)3?1:*((
int*)_tmpB7)!= 4)goto _LL87;_LL86: goto _LL82;_LL87:;_LL88:({struct Cyc_String_pa_struct
_tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpB8[1]={& _tmpBA};Cyc_Tcutil_terr(loc,({const char*_tmpB9="size requires pointer or array type, not %s";
_tag_arr(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,44));}),_tag_arr(_tmpB8,
sizeof(void*),1));}});_LL82:;}return Cyc_Absyn_uint_typ;_LL80:;_LL81:({void*
_tmpBB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpBC="Non-unary primop";_tag_arr(_tmpBC,sizeof(char),
_get_zero_arr_size(_tmpBC,17));}),_tag_arr(_tmpBB,sizeof(void*),0));});_LL75:;}
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmpBD[1]={& _tmpBF};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpBE="type %s cannot be used here";_tag_arr(_tmpBE,sizeof(char),
_get_zero_arr_size(_tmpBE,28));}),_tag_arr(_tmpBD,sizeof(void*),1));}});return
Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->v=
Cyc_Tcenv_lookup_type_vars(te);_tmpC0;}));}if(!checker(e2)){({struct Cyc_String_pa_struct
_tmpC3;_tmpC3.tag=0;_tmpC3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpC1[1]={& _tmpC3};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpC2="type %s cannot be used here";
_tag_arr(_tmpC2,sizeof(char),_get_zero_arr_size(_tmpC2,28));}),_tag_arr(_tmpC1,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpC4=
_cycalloc(sizeof(*_tmpC4));_tmpC4->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC4;}));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpC5=t1;struct Cyc_Absyn_PtrInfo
_tmpC6;void*_tmpC7;void*_tmpC8;struct Cyc_Absyn_Conref*_tmpC9;struct Cyc_Absyn_Tqual
_tmpCA;struct Cyc_Absyn_Conref*_tmpCB;struct Cyc_Absyn_Conref*_tmpCC;_LL8A: if(
_tmpC5 <= (void*)3?1:*((int*)_tmpC5)!= 4)goto _LL8C;_tmpC6=((struct Cyc_Absyn_PointerType_struct*)
_tmpC5)->f1;_tmpC7=(void*)_tmpC6.elt_typ;_tmpC8=(void*)_tmpC6.rgn_typ;_tmpC9=
_tmpC6.nullable;_tmpCA=_tmpC6.tq;_tmpCB=_tmpC6.bounds;_tmpCC=_tmpC6.zero_term;
_LL8B: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpC7),(void*)1))({void*
_tmpCD[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpCE="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,50));}),_tag_arr(_tmpCD,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct
_tmpD1;_tmpD1.tag=0;_tmpD1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{void*_tmpCF[1]={& _tmpD1};Cyc_Tcutil_terr(e2->loc,({const char*_tmpD0="expecting int but found %s";
_tag_arr(_tmpD0,sizeof(char),_get_zero_arr_size(_tmpD0,27));}),_tag_arr(_tmpCF,
sizeof(void*),1));}});_tmpCB=Cyc_Absyn_compress_conref(_tmpCB);{void*_tmpD2=(
void*)_tmpCB->v;void*_tmpD3;void*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;_LL8F: if(
_tmpD2 <= (void*)1?1:*((int*)_tmpD2)!= 0)goto _LL91;_tmpD3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpD2)->f1;if((int)_tmpD3 != 0)goto _LL91;_LL90: return t1;_LL91: if(_tmpD2 <= (void*)
1?1:*((int*)_tmpD2)!= 0)goto _LL93;_tmpD4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpD2)->f1;if(_tmpD4 <= (void*)1?1:*((int*)_tmpD4)!= 0)goto _LL93;_tmpD5=((struct
Cyc_Absyn_Upper_b_struct*)_tmpD4)->f1;_LL92: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpCC))({void*_tmpD6[0]={};Cyc_Tcutil_warn(e1->loc,({
const char*_tmpD7="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_arr(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,70));}),_tag_arr(_tmpD6,
sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*_tmpD8=({struct Cyc_Absyn_PointerType_struct*
_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_PointerType_struct
_tmpDA;_tmpDA.tag=4;_tmpDA.f1=({struct Cyc_Absyn_PtrInfo _tmpDB;_tmpDB.elt_typ=(
void*)_tmpC7;_tmpDB.rgn_typ=(void*)_tmpC8;_tmpDB.nullable=Cyc_Absyn_true_conref;
_tmpDB.tq=_tmpCA;_tmpDB.bounds=Cyc_Absyn_new_conref((void*)0);_tmpDB.zero_term=
_tmpCC;_tmpDB;});_tmpDA;});_tmpD9;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)
_tmpD8);return(void*)_tmpD8;}_LL93:;_LL94:(void*)(_tmpCB->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(void*)((void*)0);
_tmpDD;});_tmpDC;})));return t1;_LL8E:;}_LL8C:;_LL8D: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL89:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpE1;_tmpE1.tag=
0;_tmpE1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpDE[2]={& _tmpE0,&
_tmpE1};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDF="pointer arithmetic on values of different types (%s != %s)";
_tag_arr(_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,59));}),_tag_arr(_tmpDE,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}
else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpE2[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpE3="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,50));}),_tag_arr(_tmpE2,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmpE4[2]={& _tmpE6,& _tmpE7};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpE5="expecting either %s or int but found %s";
_tag_arr(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,40));}),_tag_arr(_tmpE4,
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
struct Cyc_Core_Opt*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpE8;}))): 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,
loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1);return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2);
return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e1)?1: Cyc_Tcutil_zero_to_null(
te,t1,e2))return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmpEA=({struct _tuple0
_tmpE9;_tmpE9.f1=Cyc_Tcutil_compress(t1);_tmpE9.f2=Cyc_Tcutil_compress(t2);
_tmpE9;});void*_tmpEB;struct Cyc_Absyn_PtrInfo _tmpEC;void*_tmpED;void*_tmpEE;
struct Cyc_Absyn_PtrInfo _tmpEF;void*_tmpF0;_LL96: _tmpEB=_tmpEA.f1;if(_tmpEB <= (
void*)3?1:*((int*)_tmpEB)!= 4)goto _LL98;_tmpEC=((struct Cyc_Absyn_PointerType_struct*)
_tmpEB)->f1;_tmpED=(void*)_tmpEC.elt_typ;_tmpEE=_tmpEA.f2;if(_tmpEE <= (void*)3?1:*((
int*)_tmpEE)!= 4)goto _LL98;_tmpEF=((struct Cyc_Absyn_PointerType_struct*)_tmpEE)->f1;
_tmpF0=(void*)_tmpEF.elt_typ;_LL97: if(Cyc_Tcutil_unify(_tmpED,_tmpF0))return Cyc_Absyn_sint_typ;
goto _LL95;_LL98:;_LL99: goto _LL95;_LL95:;}({struct Cyc_String_pa_struct _tmpF4;
_tmpF4.tag=0;_tmpF4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmpF1[2]={& _tmpF3,& _tmpF4};
Cyc_Tcutil_terr(loc,({const char*_tmpF2="comparison not allowed between %s and %s";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,41));}),_tag_arr(_tmpF1,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpF5;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpF6=p;_LL9B: if((int)_tmpF6 != 0)goto _LL9D;_LL9C: return
Cyc_Tcexp_tcPlus(te,e1,e2);_LL9D: if((int)_tmpF6 != 2)goto _LL9F;_LL9E: return Cyc_Tcexp_tcMinus(
te,e1,e2);_LL9F: if((int)_tmpF6 != 1)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if((int)
_tmpF6 != 3)goto _LLA3;_LLA2: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLA3: if((int)_tmpF6 != 4)goto _LLA5;_LLA4: goto _LLA6;_LLA5: if((int)_tmpF6 != 13)
goto _LLA7;_LLA6: goto _LLA8;_LLA7: if((int)_tmpF6 != 14)goto _LLA9;_LLA8: goto _LLAA;
_LLA9: if((int)_tmpF6 != 15)goto _LLAB;_LLAA: goto _LLAC;_LLAB: if((int)_tmpF6 != 16)
goto _LLAD;_LLAC: goto _LLAE;_LLAD: if((int)_tmpF6 != 17)goto _LLAF;_LLAE: goto _LLB0;
_LLAF: if((int)_tmpF6 != 18)goto _LLB1;_LLB0: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_integral);_LLB1: if((int)_tmpF6 != 5)goto _LLB3;_LLB2: goto _LLB4;_LLB3:
if((int)_tmpF6 != 6)goto _LLB5;_LLB4: goto _LLB6;_LLB5: if((int)_tmpF6 != 7)goto _LLB7;
_LLB6: goto _LLB8;_LLB7: if((int)_tmpF6 != 8)goto _LLB9;_LLB8: goto _LLBA;_LLB9: if((int)
_tmpF6 != 9)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if((int)_tmpF6 != 10)goto _LLBD;_LLBC:
return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLBD:;_LLBE:({void*_tmpF7[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF8="bad binary primop";
_tag_arr(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,18));}),_tag_arr(_tmpF7,
sizeof(void*),0));});_LL9A:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_List_List*es){if(p
== (void*)2?((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: return({void*_tmpF9[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmpFA="primitive operator has 0 arguments";
_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,35));}),_tag_arr(_tmpF9,
sizeof(void*),0));});case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLC2: return({void*_tmpFB[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmpFC="primitive operator has > 2 arguments";_tag_arr(_tmpFC,
sizeof(char),_get_zero_arr_size(_tmpFC,37));}),_tag_arr(_tmpFB,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmpFD=(void*)e->r;void*
_tmpFE;struct Cyc_Absyn_Vardecl*_tmpFF;void*_tmp100;struct Cyc_Absyn_Vardecl*
_tmp101;void*_tmp102;struct Cyc_Absyn_Vardecl*_tmp103;void*_tmp104;struct Cyc_Absyn_Vardecl*
_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*
_tmp108;struct _tagged_arr*_tmp109;struct Cyc_Absyn_Exp*_tmp10A;struct _tagged_arr*
_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*
_tmp10E;_LLC5: if(*((int*)_tmpFD)!= 1)goto _LLC7;_tmpFE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpFD)->f2;if(_tmpFE <= (void*)1?1:*((int*)_tmpFE)!= 2)goto _LLC7;_tmpFF=((struct
Cyc_Absyn_Param_b_struct*)_tmpFE)->f1;_LLC6: _tmp101=_tmpFF;goto _LLC8;_LLC7: if(*((
int*)_tmpFD)!= 1)goto _LLC9;_tmp100=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpFD)->f2;
if(_tmp100 <= (void*)1?1:*((int*)_tmp100)!= 3)goto _LLC9;_tmp101=((struct Cyc_Absyn_Local_b_struct*)
_tmp100)->f1;_LLC8: _tmp103=_tmp101;goto _LLCA;_LLC9: if(*((int*)_tmpFD)!= 1)goto
_LLCB;_tmp102=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpFD)->f2;if(_tmp102 <= (
void*)1?1:*((int*)_tmp102)!= 4)goto _LLCB;_tmp103=((struct Cyc_Absyn_Pat_b_struct*)
_tmp102)->f1;_LLCA: _tmp105=_tmp103;goto _LLCC;_LLCB: if(*((int*)_tmpFD)!= 1)goto
_LLCD;_tmp104=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpFD)->f2;if(_tmp104 <= (
void*)1?1:*((int*)_tmp104)!= 0)goto _LLCD;_tmp105=((struct Cyc_Absyn_Global_b_struct*)
_tmp104)->f1;_LLCC: if(!(_tmp105->tq).q_const)return;goto _LLC4;_LLCD: if(*((int*)
_tmpFD)!= 23)goto _LLCF;_tmp106=((struct Cyc_Absyn_Subscript_e_struct*)_tmpFD)->f1;
_tmp107=((struct Cyc_Absyn_Subscript_e_struct*)_tmpFD)->f2;_LLCE:{void*_tmp10F=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp106->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp110;struct Cyc_Absyn_Tqual _tmp111;struct Cyc_Absyn_ArrayInfo
_tmp112;struct Cyc_Absyn_Tqual _tmp113;struct Cyc_List_List*_tmp114;_LLDC: if(
_tmp10F <= (void*)3?1:*((int*)_tmp10F)!= 4)goto _LLDE;_tmp110=((struct Cyc_Absyn_PointerType_struct*)
_tmp10F)->f1;_tmp111=_tmp110.tq;_LLDD: _tmp113=_tmp111;goto _LLDF;_LLDE: if(_tmp10F
<= (void*)3?1:*((int*)_tmp10F)!= 7)goto _LLE0;_tmp112=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10F)->f1;_tmp113=_tmp112.tq;_LLDF: if(!_tmp113.q_const)return;goto _LLDB;_LLE0:
if(_tmp10F <= (void*)3?1:*((int*)_tmp10F)!= 9)goto _LLE2;_tmp114=((struct Cyc_Absyn_TupleType_struct*)
_tmp10F)->f1;_LLE1: {unsigned int _tmp116;int _tmp117;struct _tuple7 _tmp115=Cyc_Evexp_eval_const_uint_exp(
_tmp107);_tmp116=_tmp115.f1;_tmp117=_tmp115.f2;if(!_tmp117){({void*_tmp118[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp119="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp119,sizeof(char),_get_zero_arr_size(_tmp119,47));}),_tag_arr(
_tmp118,sizeof(void*),0));});return;}{struct _handler_cons _tmp11A;_push_handler(&
_tmp11A);{int _tmp11C=0;if(setjmp(_tmp11A.handler))_tmp11C=1;if(!_tmp11C){{struct
_tuple9 _tmp11E;struct Cyc_Absyn_Tqual _tmp11F;struct _tuple9*_tmp11D=((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp114,(int)_tmp116);
_tmp11E=*_tmp11D;_tmp11F=_tmp11E.f1;if(!_tmp11F.q_const){_npop_handler(0);
return;}};_pop_handler();}else{void*_tmp11B=(void*)_exn_thrown;void*_tmp121=
_tmp11B;_LLE5: if(_tmp121 != Cyc_List_Nth)goto _LLE7;_LLE6: return;_LLE7:;_LLE8:(
void)_throw(_tmp121);_LLE4:;}}}goto _LLDB;}_LLE2:;_LLE3: goto _LLDB;_LLDB:;}goto
_LLC4;_LLCF: if(*((int*)_tmpFD)!= 21)goto _LLD1;_tmp108=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpFD)->f1;_tmp109=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpFD)->f2;_LLD0:{
void*_tmp122=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp108->topt))->v);struct Cyc_Absyn_AggrInfo _tmp123;void*_tmp124;struct Cyc_Absyn_Aggrdecl**
_tmp125;struct Cyc_List_List*_tmp126;_LLEA: if(_tmp122 <= (void*)3?1:*((int*)
_tmp122)!= 10)goto _LLEC;_tmp123=((struct Cyc_Absyn_AggrType_struct*)_tmp122)->f1;
_tmp124=(void*)_tmp123.aggr_info;if(*((int*)_tmp124)!= 1)goto _LLEC;_tmp125=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp124)->f1;_LLEB: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp125 == 0?0: Cyc_Absyn_lookup_decl_field(*((
struct Cyc_Absyn_Aggrdecl**)_tmp125),_tmp109);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLE9;}_LLEC: if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 11)goto _LLEE;
_tmp126=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp122)->f2;_LLED: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp126,_tmp109);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: if(*((int*)_tmpFD)!= 
22)goto _LLD3;_tmp10A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpFD)->f1;_tmp10B=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpFD)->f2;_LLD2:{void*_tmp127=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp128;void*_tmp129;_LLF1: if(_tmp127 <= (void*)3?1:*((int*)_tmp127)!= 4)goto
_LLF3;_tmp128=((struct Cyc_Absyn_PointerType_struct*)_tmp127)->f1;_tmp129=(void*)
_tmp128.elt_typ;_LLF2:{void*_tmp12A=Cyc_Tcutil_compress(_tmp129);struct Cyc_Absyn_AggrInfo
_tmp12B;void*_tmp12C;struct Cyc_Absyn_Aggrdecl**_tmp12D;struct Cyc_List_List*
_tmp12E;_LLF6: if(_tmp12A <= (void*)3?1:*((int*)_tmp12A)!= 10)goto _LLF8;_tmp12B=((
struct Cyc_Absyn_AggrType_struct*)_tmp12A)->f1;_tmp12C=(void*)_tmp12B.aggr_info;
if(*((int*)_tmp12C)!= 1)goto _LLF8;_tmp12D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp12C)->f1;_LLF7: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp12D == 0?0: Cyc_Absyn_lookup_decl_field(*((struct Cyc_Absyn_Aggrdecl**)_tmp12D),
_tmp10B);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLF5;}_LLF8: if(_tmp12A <= (
void*)3?1:*((int*)_tmp12A)!= 11)goto _LLFA;_tmp12E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12A)->f2;_LLF9: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12E,
_tmp10B);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;
_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto _LLF0;_LLF0:;}goto _LLC4;_LLD3: if(*((int*)
_tmpFD)!= 20)goto _LLD5;_tmp10C=((struct Cyc_Absyn_Deref_e_struct*)_tmpFD)->f1;
_LLD4:{void*_tmp12F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp10C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp130;struct Cyc_Absyn_Tqual _tmp131;
struct Cyc_Absyn_ArrayInfo _tmp132;struct Cyc_Absyn_Tqual _tmp133;_LLFD: if(_tmp12F <= (
void*)3?1:*((int*)_tmp12F)!= 4)goto _LLFF;_tmp130=((struct Cyc_Absyn_PointerType_struct*)
_tmp12F)->f1;_tmp131=_tmp130.tq;_LLFE: _tmp133=_tmp131;goto _LL100;_LLFF: if(
_tmp12F <= (void*)3?1:*((int*)_tmp12F)!= 7)goto _LL101;_tmp132=((struct Cyc_Absyn_ArrayType_struct*)
_tmp12F)->f1;_tmp133=_tmp132.tq;_LL100: if(!_tmp133.q_const)return;goto _LLFC;
_LL101:;_LL102: goto _LLFC;_LLFC:;}goto _LLC4;_LLD5: if(*((int*)_tmpFD)!= 11)goto
_LLD7;_tmp10D=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpFD)->f1;_LLD6:
_tmp10E=_tmp10D;goto _LLD8;_LLD7: if(*((int*)_tmpFD)!= 12)goto _LLD9;_tmp10E=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmpFD)->f1;_LLD8: Cyc_Tcexp_check_writable(
te,_tmp10E);return;_LLD9:;_LLDA: goto _LLC4;_LLC4:;}({struct Cyc_String_pa_struct
_tmp136;_tmp136.tag=0;_tmp136.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e));{void*_tmp134[1]={& _tmp136};Cyc_Tcutil_terr(e->loc,({const char*_tmp135="attempt to write a const location: %s";
_tag_arr(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,38));}),_tag_arr(
_tmp134,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp137[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp138="increment/decrement of non-lvalue";_tag_arr(_tmp138,
sizeof(char),_get_zero_arr_size(_tmp138,34));}),_tag_arr(_tmp137,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt)?1:(Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt)?i == (void*)0?1: i == (void*)
1: 0)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp139[
0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp13A="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,50));}),_tag_arr(
_tmp139,sizeof(void*),0));});}else{({struct Cyc_String_pa_struct _tmp13D;_tmp13D.tag=
0;_tmp13D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp13B[1]={& _tmp13D};Cyc_Tcutil_terr(e->loc,({const char*_tmp13C="expecting arithmetic or ? type but found %s";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,44));}),_tag_arr(
_tmp13B,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp13E="conditional expression";_tag_arr(_tmp13E,sizeof(char),
_get_zero_arr_size(_tmp13E,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp147;}));struct Cyc_List_List _tmp13F=({struct Cyc_List_List _tmp146;_tmp146.hd=
e3;_tmp146.tl=0;_tmp146;});struct Cyc_List_List _tmp140=({struct Cyc_List_List
_tmp145;_tmp145.hd=e2;_tmp145.tl=(struct Cyc_List_List*)& _tmp13F;_tmp145;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp140)){({struct Cyc_String_pa_struct
_tmp144;_tmp144.tag=0;_tmp144.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp143;_tmp143.tag=0;_tmp143.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp141[2]={&
_tmp143,& _tmp144};Cyc_Tcutil_terr(loc,({const char*_tmp142="conditional clause types do not match: %s != %s";
_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,48));}),_tag_arr(
_tmp141,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}static
void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp148=Cyc_Tcutil_compress(t1);_LL104: if(_tmp148 <= (void*)3?1:*((int*)
_tmp148)!= 7)goto _LL106;_LL105:({void*_tmp149[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp14A="cannot assign to an array";_tag_arr(_tmp14A,sizeof(char),
_get_zero_arr_size(_tmp14A,26));}),_tag_arr(_tmp149,sizeof(void*),0));});goto
_LL103;_LL106:;_LL107: goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp14B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp14C="type is abstract (can't determine size).";
_tag_arr(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,41));}),_tag_arr(
_tmp14B,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp14D[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp14E="assignment to non-lvalue";
_tag_arr(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,25));}),_tag_arr(
_tmp14D,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(
te,e2,t1)){void*_tmp14F=({struct Cyc_String_pa_struct _tmp153;_tmp153.tag=0;
_tmp153.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp150[2]={& _tmp152,&
_tmp153};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp151="type mismatch: %s != %s";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,24));}),_tag_arr(
_tmp150,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp14F;}}else{void*_tmp154=(void*)po->v;void*_tmp155=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp154,e1,e2);if(!(Cyc_Tcutil_unify(_tmp155,t1)?1:(Cyc_Tcutil_coerceable(
_tmp155)?Cyc_Tcutil_coerceable(t1): 0))){void*_tmp156=({struct Cyc_String_pa_struct
_tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp157[2]={&
_tmp159,& _tmp15A};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp158="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_arr(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,82));}),_tag_arr(
_tmp157,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp155,t1);Cyc_Tcutil_explain_failure();
return _tmp156;}return _tmp155;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp15B=Cyc_Tcutil_compress(
t1);void*_tmp15C;void*_tmp15D;_LL109: if((int)_tmp15B != 1)goto _LL10B;_LL10A: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ);t1=dbl_typ;goto _LL108;_LL10B: if(_tmp15B <= (void*)3?1:*((int*)
_tmp15B)!= 5)goto _LL10D;_tmp15C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp15B)->f2;
if((int)_tmp15C != 0)goto _LL10D;_LL10C: goto _LL10E;_LL10D: if(_tmp15B <= (void*)3?1:*((
int*)_tmp15B)!= 5)goto _LL10F;_tmp15D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp15B)->f2;if((int)_tmp15D != 1)goto _LL10F;_LL10E: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ);t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;
_LL108:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp15F;
struct Cyc_List_List*_tmp160;struct Cyc_Position_Segment*_tmp161;void*_tmp162;
struct Cyc_Absyn_Tunionfield _tmp15E=*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd));_tmp15F=_tmp15E.name;_tmp160=_tmp15E.typs;_tmp161=_tmp15E.loc;
_tmp162=(void*)_tmp15E.sc;if(_tmp160 == 0?1: _tmp160->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)((struct _tuple9*)_tmp160->hd))).f2);if(Cyc_Tcutil_unify(
t1,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp164;struct Cyc_List_List*
_tmp165;struct Cyc_Position_Segment*_tmp166;void*_tmp167;struct Cyc_Absyn_Tunionfield
_tmp163=*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd));
_tmp164=_tmp163.name;_tmp165=_tmp163.typs;_tmp166=_tmp163.loc;_tmp167=(void*)
_tmp163.sc;if(_tmp165 == 0?1: _tmp165->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)((struct _tuple9*)_tmp165->hd))).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{struct Cyc_String_pa_struct
_tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
tu));{void*_tmp168[2]={& _tmp16A,& _tmp16B};Cyc_Tcutil_terr(e->loc,({const char*
_tmp169="can't find a field in %s to inject a value of type %s";_tag_arr(_tmp169,
sizeof(char),_get_zero_arr_size(_tmp169,54));}),_tag_arr(_tmp168,sizeof(void*),2));}}});
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){struct Cyc_List_List*_tmp16D=args;te=Cyc_Tcenv_new_block(loc,te);
Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp16E=t;struct Cyc_Absyn_PtrInfo _tmp16F;void*
_tmp170;void*_tmp171;struct Cyc_Absyn_Conref*_tmp172;struct Cyc_Absyn_Tqual _tmp173;
struct Cyc_Absyn_Conref*_tmp174;struct Cyc_Absyn_Conref*_tmp175;_LL112: if(_tmp16E
<= (void*)3?1:*((int*)_tmp16E)!= 4)goto _LL114;_tmp16F=((struct Cyc_Absyn_PointerType_struct*)
_tmp16E)->f1;_tmp170=(void*)_tmp16F.elt_typ;_tmp171=(void*)_tmp16F.rgn_typ;
_tmp172=_tmp16F.nullable;_tmp173=_tmp16F.tq;_tmp174=_tmp16F.bounds;_tmp175=
_tmp16F.zero_term;_LL113: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp171);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp174);{void*_tmp176=Cyc_Tcutil_compress(_tmp170);struct Cyc_Absyn_FnInfo
_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_Core_Opt*_tmp179;void*_tmp17A;
struct Cyc_List_List*_tmp17B;int _tmp17C;struct Cyc_Absyn_VarargInfo*_tmp17D;struct
Cyc_List_List*_tmp17E;struct Cyc_List_List*_tmp17F;_LL117: if(_tmp176 <= (void*)3?1:*((
int*)_tmp176)!= 8)goto _LL119;_tmp177=((struct Cyc_Absyn_FnType_struct*)_tmp176)->f1;
_tmp178=_tmp177.tvars;_tmp179=_tmp177.effect;_tmp17A=(void*)_tmp177.ret_typ;
_tmp17B=_tmp177.args;_tmp17C=_tmp177.c_varargs;_tmp17D=_tmp177.cyc_varargs;
_tmp17E=_tmp177.rgn_po;_tmp17F=_tmp177.attributes;_LL118: if(topt != 0)Cyc_Tcutil_unify(
_tmp17A,*((void**)topt));while(_tmp16D != 0?_tmp17B != 0: 0){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp16D->hd;void*t2=(*((struct _tuple2*)((struct _tuple2*)
_tmp17B->hd))).f3;Cyc_Tcexp_tcExp(te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(
te,e1,t2)){({struct Cyc_String_pa_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp182;_tmp182.tag=0;_tmp182.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp180[2]={&
_tmp182,& _tmp183};Cyc_Tcutil_terr(e1->loc,({const char*_tmp181="actual argument has type \n%s\n but formal has type \n%s";
_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,54));}),_tag_arr(
_tmp180,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}_tmp16D=_tmp16D->tl;
_tmp17B=_tmp17B->tl;}{int args_already_checked=0;{struct Cyc_List_List*a=_tmp17F;
for(0;a != 0;a=a->tl){void*_tmp184=(void*)a->hd;void*_tmp185;int _tmp186;int
_tmp187;_LL11C: if(_tmp184 <= (void*)17?1:*((int*)_tmp184)!= 3)goto _LL11E;_tmp185=(
void*)((struct Cyc_Absyn_Format_att_struct*)_tmp184)->f1;_tmp186=((struct Cyc_Absyn_Format_att_struct*)
_tmp184)->f2;_tmp187=((struct Cyc_Absyn_Format_att_struct*)_tmp184)->f3;_LL11D:{
struct _handler_cons _tmp188;_push_handler(& _tmp188);{int _tmp18A=0;if(setjmp(
_tmp188.handler))_tmp18A=1;if(!_tmp18A){{struct Cyc_Absyn_Exp*_tmp18B=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp186 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp187 == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp18C=
_cycalloc(sizeof(*_tmp18C));_tmp18C->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp187 - 1);_tmp18C;});}args_already_checked=1;{
void*_tmp18D=_tmp185;_LL121: if((int)_tmp18D != 0)goto _LL123;_LL122: Cyc_Tcexp_check_format_args(
te,_tmp18B,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL120;_LL123: if((int)
_tmp18D != 1)goto _LL120;_LL124: Cyc_Tcexp_check_format_args(te,_tmp18B,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL120;_LL120:;}};_pop_handler();}else{void*
_tmp189=(void*)_exn_thrown;void*_tmp18F=_tmp189;_LL126: if(_tmp18F != Cyc_List_Nth)
goto _LL128;_LL127:({void*_tmp190[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp191="bad format arguments";
_tag_arr(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,21));}),_tag_arr(
_tmp190,sizeof(void*),0));});goto _LL125;_LL128:;_LL129:(void)_throw(_tmp18F);
_LL125:;}}}goto _LL11B;_LL11E:;_LL11F: goto _LL11B;_LL11B:;}}if(_tmp17B != 0)({void*
_tmp192[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp193="too few arguments for function";
_tag_arr(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,31));}),_tag_arr(
_tmp192,sizeof(void*),0));});else{if((_tmp16D != 0?1: _tmp17C)?1: _tmp17D != 0){if(
_tmp17C)for(0;_tmp16D != 0;_tmp16D=_tmp16D->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp16D->hd);}else{if(_tmp17D == 0)({void*_tmp194[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp195="too many arguments for function";_tag_arr(_tmp195,sizeof(char),
_get_zero_arr_size(_tmp195,32));}),_tag_arr(_tmp194,sizeof(void*),0));});else{
void*_tmp197;int _tmp198;struct Cyc_Absyn_VarargInfo _tmp196=*((struct Cyc_Absyn_VarargInfo*)
_tmp17D);_tmp197=(void*)_tmp196.type;_tmp198=_tmp196.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp199=({struct Cyc_Absyn_VarargCallInfo*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));
_tmp1B1->num_varargs=0;_tmp1B1->injectors=0;_tmp1B1->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp17D);_tmp1B1;});*((struct Cyc_Absyn_VarargCallInfo**)
vararg_call_info)=(struct Cyc_Absyn_VarargCallInfo*)_tmp199;if(!_tmp198)for(0;
_tmp16D != 0;_tmp16D=_tmp16D->tl){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp16D->hd;_tmp199->num_varargs ++;Cyc_Tcexp_tcExp(te,(void**)& _tmp197,e1);if(!
Cyc_Tcutil_coerce_arg(te,e1,_tmp197)){({struct Cyc_String_pa_struct _tmp19D;
_tmp19D.tag=0;_tmp19D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{struct Cyc_String_pa_struct
_tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp197));{void*_tmp19A[2]={& _tmp19C,& _tmp19D};Cyc_Tcutil_terr(loc,({const char*
_tmp19B="vararg requires type %s but argument has type %s";_tag_arr(_tmp19B,
sizeof(char),_get_zero_arr_size(_tmp19B,49));}),_tag_arr(_tmp19A,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}}else{void*_tmp19E=Cyc_Tcutil_compress(_tmp197);
struct Cyc_Absyn_TunionInfo _tmp19F;void*_tmp1A0;struct Cyc_Absyn_Tuniondecl**
_tmp1A1;struct Cyc_Absyn_Tuniondecl*_tmp1A2;struct Cyc_List_List*_tmp1A3;void*
_tmp1A4;_LL12B: if(_tmp19E <= (void*)3?1:*((int*)_tmp19E)!= 2)goto _LL12D;_tmp19F=((
struct Cyc_Absyn_TunionType_struct*)_tmp19E)->f1;_tmp1A0=(void*)_tmp19F.tunion_info;
if(*((int*)_tmp1A0)!= 1)goto _LL12D;_tmp1A1=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp1A0)->f1;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp19F.targs;_tmp1A4=(void*)_tmp19F.rgn;
_LL12C: {struct Cyc_Absyn_Tuniondecl*_tmp1A5=*((struct Cyc_Absyn_Tuniondecl**)Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp1A2->name));struct Cyc_List_List*fields=0;if(_tmp1A5->fields == 0)({
struct Cyc_String_pa_struct _tmp1A8;_tmp1A8.tag=0;_tmp1A8.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp197));{void*_tmp1A6[1]={& _tmp1A8};
Cyc_Tcutil_terr(loc,({const char*_tmp1A7="can't inject into %s";_tag_arr(_tmp1A7,
sizeof(char),_get_zero_arr_size(_tmp1A7,21));}),_tag_arr(_tmp1A6,sizeof(void*),1));}});
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1A5->fields))->v;}
if(!Cyc_Tcutil_unify(_tmp1A4,Cyc_Tcenv_curr_rgn(te)))({void*_tmp1A9[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1AA="bad region for injected varargs";_tag_arr(_tmp1AA,
sizeof(char),_get_zero_arr_size(_tmp1AA,32));}),_tag_arr(_tmp1A9,sizeof(void*),0));});{
struct _RegionHandle _tmp1AB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1AB;
_push_region(rgn);{struct Cyc_List_List*_tmp1AC=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1A5->tvs,_tmp1A3);for(0;_tmp16D != 0;_tmp16D=
_tmp16D->tl){_tmp199->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp16D->hd;if(!args_already_checked)Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp1AD=Cyc_Tcexp_tcInjection(te,e1,_tmp197,rgn,_tmp1AC,fields);if(_tmp1AD != 0)
_tmp199->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp199->injectors,({struct Cyc_List_List*_tmp1AE=_cycalloc(
sizeof(*_tmp1AE));_tmp1AE->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp1AD);
_tmp1AE->tl=0;_tmp1AE;}));}}}};_pop_region(rgn);}goto _LL12A;}_LL12D:;_LL12E:({
void*_tmp1AF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1B0="bad inject vararg type";
_tag_arr(_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,23));}),_tag_arr(
_tmp1AF,sizeof(void*),0));});goto _LL12A;_LL12A:;}}}}}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp179))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp17E);return _tmp17A;}_LL119:;_LL11A: return({void*_tmp1B2[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp1B3="expected pointer to function";_tag_arr(_tmp1B3,
sizeof(char),_get_zero_arr_size(_tmp1B3,29));}),_tag_arr(_tmp1B2,sizeof(void*),0));});
_LL116:;}_LL114:;_LL115: return({void*_tmp1B4[0]={};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp1B5="expected pointer to function";_tag_arr(_tmp1B5,sizeof(char),
_get_zero_arr_size(_tmp1B5,29));}),_tag_arr(_tmp1B4,sizeof(void*),0));});_LL111:;}}
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1B8;_tmp1B8.tag=0;_tmp1B8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1B6[1]={&
_tmp1B8};Cyc_Tcutil_terr(loc,({const char*_tmp1B7="expected xtunion exn but found %s";
_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,34));}),_tag_arr(
_tmp1B6,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp1B9;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp1BA=t1;struct Cyc_Absyn_PtrInfo _tmp1BB;void*_tmp1BC;void*_tmp1BD;struct
Cyc_Absyn_Conref*_tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;struct Cyc_Absyn_Conref*
_tmp1C0;struct Cyc_Absyn_Conref*_tmp1C1;_LL130: if(_tmp1BA <= (void*)3?1:*((int*)
_tmp1BA)!= 4)goto _LL132;_tmp1BB=((struct Cyc_Absyn_PointerType_struct*)_tmp1BA)->f1;
_tmp1BC=(void*)_tmp1BB.elt_typ;_tmp1BD=(void*)_tmp1BB.rgn_typ;_tmp1BE=_tmp1BB.nullable;
_tmp1BF=_tmp1BB.tq;_tmp1C0=_tmp1BB.bounds;_tmp1C1=_tmp1BB.zero_term;_LL131:{void*
_tmp1C2=Cyc_Tcutil_compress(_tmp1BC);struct Cyc_Absyn_FnInfo _tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_Core_Opt*_tmp1C5;void*_tmp1C6;struct Cyc_List_List*_tmp1C7;int
_tmp1C8;struct Cyc_Absyn_VarargInfo*_tmp1C9;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*
_tmp1CB;_LL135: if(_tmp1C2 <= (void*)3?1:*((int*)_tmp1C2)!= 8)goto _LL137;_tmp1C3=((
struct Cyc_Absyn_FnType_struct*)_tmp1C2)->f1;_tmp1C4=_tmp1C3.tvars;_tmp1C5=
_tmp1C3.effect;_tmp1C6=(void*)_tmp1C3.ret_typ;_tmp1C7=_tmp1C3.args;_tmp1C8=
_tmp1C3.c_varargs;_tmp1C9=_tmp1C3.cyc_varargs;_tmp1CA=_tmp1C3.rgn_po;_tmp1CB=
_tmp1C3.attributes;_LL136: {struct Cyc_List_List*instantiation=0;for(0;ts != 0?
_tmp1C4 != 0: 0;(ts=ts->tl,_tmp1C4=_tmp1C4->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp1C4->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp1CC=_cycalloc(
sizeof(*_tmp1CC));_tmp1CC->hd=({struct _tuple5*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));
_tmp1CD->f1=(struct Cyc_Absyn_Tvar*)_tmp1C4->hd;_tmp1CD->f2=(void*)ts->hd;_tmp1CD;});
_tmp1CC->tl=instantiation;_tmp1CC;});}if(ts != 0)return({void*_tmp1CE[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp1CF="too many type variables in instantiation";_tag_arr(
_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,41));}),_tag_arr(_tmp1CE,sizeof(
void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(instantiation,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({
struct Cyc_Absyn_FnType_struct _tmp1D4;_tmp1D4.tag=8;_tmp1D4.f1=({struct Cyc_Absyn_FnInfo
_tmp1D5;_tmp1D5.tvars=_tmp1C4;_tmp1D5.effect=_tmp1C5;_tmp1D5.ret_typ=(void*)
_tmp1C6;_tmp1D5.args=_tmp1C7;_tmp1D5.c_varargs=_tmp1C8;_tmp1D5.cyc_varargs=
_tmp1C9;_tmp1D5.rgn_po=_tmp1CA;_tmp1D5.attributes=_tmp1CB;_tmp1D5;});_tmp1D4;});
_tmp1D3;}));return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp1D0=_cycalloc(
sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Absyn_PointerType_struct _tmp1D1;_tmp1D1.tag=
4;_tmp1D1.f1=({struct Cyc_Absyn_PtrInfo _tmp1D2;_tmp1D2.elt_typ=(void*)new_fn_typ;
_tmp1D2.rgn_typ=(void*)_tmp1BD;_tmp1D2.nullable=_tmp1BE;_tmp1D2.tq=_tmp1BF;
_tmp1D2.bounds=_tmp1C0;_tmp1D2.zero_term=_tmp1C1;_tmp1D2;});_tmp1D1;});_tmp1D0;});}}
_LL137:;_LL138: goto _LL134;_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}
return({struct Cyc_String_pa_struct _tmp1D8;_tmp1D8.tag=0;_tmp1D8.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp1D6[1]={&
_tmp1D8};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1D7="expecting polymorphic type but found %s";
_tag_arr(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,40));}),_tag_arr(
_tmp1D6,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(
te,loc,t2,t)?!Cyc_Tcutil_castable(te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,
e): 0){Cyc_Tcutil_unify(t2,t);{void*_tmp1D9=({struct Cyc_String_pa_struct _tmp1DD;
_tmp1DD.tag=0;_tmp1DD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp1DC;_tmp1DC.tag=0;_tmp1DC.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{void*_tmp1DA[2]={&
_tmp1DC,& _tmp1DD};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1DB="cannot cast %s to %s";
_tag_arr(_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,21));}),_tag_arr(
_tmp1DA,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp1D9;}}
return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp1DE=0;
struct Cyc_Absyn_Tqual _tmp1DF=Cyc_Absyn_empty_tqual();if(topt != 0){void*_tmp1E0=
Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo _tmp1E1;void*_tmp1E2;
struct Cyc_Absyn_Tqual _tmp1E3;struct Cyc_Absyn_Conref*_tmp1E4;_LL13A: if(_tmp1E0 <= (
void*)3?1:*((int*)_tmp1E0)!= 4)goto _LL13C;_tmp1E1=((struct Cyc_Absyn_PointerType_struct*)
_tmp1E0)->f1;_tmp1E2=(void*)_tmp1E1.elt_typ;_tmp1E3=_tmp1E1.tq;_tmp1E4=_tmp1E1.zero_term;
_LL13B: _tmp1DE=({void**_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[0]=_tmp1E2;
_tmp1E5;});_tmp1DF=_tmp1E3;goto _LL139;_LL13C:;_LL13D: goto _LL139;_LL139:;}Cyc_Tcexp_tcExpNoInst(
te,_tmp1DE,e);{void*_tmp1E6=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;_LL13F: if(*((int*)_tmp1E6)!= 28)goto _LL141;_LL140: goto _LL142;_LL141: if(*((
int*)_tmp1E6)!= 24)goto _LL143;_LL142:({void*_tmp1E9[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp1EA="& used to allocate";_tag_arr(_tmp1EA,sizeof(char),
_get_zero_arr_size(_tmp1EA,19));}),_tag_arr(_tmp1E9,sizeof(void*),0));});{void*(*
_tmp1EB)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp1EC=Cyc_Tcutil_compress(*((
void**)topt));struct Cyc_Absyn_PtrInfo _tmp1ED;struct Cyc_Absyn_Conref*_tmp1EE;
struct Cyc_Absyn_Conref*_tmp1EF;_LL148: if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 
4)goto _LL14A;_tmp1ED=((struct Cyc_Absyn_PointerType_struct*)_tmp1EC)->f1;_tmp1EE=
_tmp1ED.nullable;_tmp1EF=_tmp1ED.zero_term;_LL149: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp1EE))_tmp1EB=Cyc_Absyn_star_typ;goto _LL147;_LL14A:;
_LL14B: goto _LL147;_LL147:;}return _tmp1EB((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)2,_tmp1DF,Cyc_Absyn_false_conref);}_LL143: if(*((
int*)_tmp1E6)!= 23)goto _LL145;_tmp1E7=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1E6)->f1;_tmp1E8=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1E6)->f2;_LL144:{
void*_tmp1F0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1E7->topt))->v);_LL14D: if(_tmp1F0 <= (void*)3?1:*((int*)_tmp1F0)!= 9)goto
_LL14F;_LL14E: goto _LL14C;_LL14F:;_LL150:(void*)(e0->r=(void*)((void*)(Cyc_Absyn_add_exp(
_tmp1E7,_tmp1E8,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp1E7,_tmp1E8);_LL14C:;}
goto _LL13E;_LL145:;_LL146: goto _LL13E;_LL13E:;}{int _tmp1F2;void*_tmp1F3;struct
_tuple6 _tmp1F1=Cyc_Tcutil_addressof_props(te,e);_tmp1F2=_tmp1F1.f1;_tmp1F3=
_tmp1F1.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp1F2)tq.q_const=
1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp1F3,tq,Cyc_Absyn_false_conref);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_String_pa_struct _tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp1F4[1]={& _tmp1F6};Cyc_Tcutil_terr(
loc,({const char*_tmp1F5="sizeof applied to type %s, which has unknown size here";
_tag_arr(_tmp1F5,sizeof(char),_get_zero_arr_size(_tmp1F5,55));}),_tag_arr(
_tmp1F4,sizeof(void*),1));}});if(topt != 0){void*_tmp1F7=Cyc_Tcutil_compress(*((
void**)topt));_LL152: if(_tmp1F7 <= (void*)3?1:*((int*)_tmp1F7)!= 14)goto _LL154;
_LL153: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1F8=_cycalloc(
sizeof(*_tmp1F8));_tmp1F8[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1F9;_tmp1F9.tag=
14;_tmp1F9.f1=(void*)t;_tmp1F9;});_tmp1F8;});_LL154:;_LL155: goto _LL151;_LL151:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct _tagged_arr)*((struct
_tagged_arr*)n),(struct _tagged_arr)*((struct _tagged_arr*)sf->name))== 0;}static
void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);{void*_tmp1FA=n;struct _tagged_arr*_tmp1FB;unsigned int _tmp1FC;
_LL157: if(*((int*)_tmp1FA)!= 0)goto _LL159;_tmp1FB=((struct Cyc_Absyn_StructField_struct*)
_tmp1FA)->f1;_LL158: {int bad_type=1;{void*_tmp1FD=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Aggrdecl**_tmp200;struct
Cyc_List_List*_tmp201;_LL15C: if(_tmp1FD <= (void*)3?1:*((int*)_tmp1FD)!= 10)goto
_LL15E;_tmp1FE=((struct Cyc_Absyn_AggrType_struct*)_tmp1FD)->f1;_tmp1FF=(void*)
_tmp1FE.aggr_info;if(*((int*)_tmp1FF)!= 1)goto _LL15E;_tmp200=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1FF)->f1;_LL15D: if((*((struct Cyc_Absyn_Aggrdecl**)_tmp200))->impl == 0)goto
_LL15B;if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1FB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*((struct Cyc_Absyn_Aggrdecl**)
_tmp200))->impl))->fields))({struct Cyc_String_pa_struct _tmp204;_tmp204.tag=0;
_tmp204.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_tmp1FB));{
void*_tmp202[1]={& _tmp204};Cyc_Tcutil_terr(loc,({const char*_tmp203="no field of struct/union has name %s";
_tag_arr(_tmp203,sizeof(char),_get_zero_arr_size(_tmp203,37));}),_tag_arr(
_tmp202,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL15E: if(_tmp1FD <= (void*)3?
1:*((int*)_tmp1FD)!= 11)goto _LL160;_tmp201=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1FD)->f2;_LL15F: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1FB,_tmp201))({struct Cyc_String_pa_struct _tmp207;_tmp207.tag=0;_tmp207.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_tmp1FB));{void*
_tmp205[1]={& _tmp207};Cyc_Tcutil_terr(loc,({const char*_tmp206="no field of struct/union has name %s";
_tag_arr(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,37));}),_tag_arr(
_tmp205,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL160:;_LL161: goto _LL15B;
_LL15B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp20A;_tmp20A.tag=0;_tmp20A.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp208[1]={&
_tmp20A};Cyc_Tcutil_terr(loc,({const char*_tmp209="%s is not a known struct/union type";
_tag_arr(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,36));}),_tag_arr(
_tmp208,sizeof(void*),1));}});goto _LL156;}_LL159: if(*((int*)_tmp1FA)!= 1)goto
_LL156;_tmp1FC=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FA)->f1;_LL15A: {int
bad_type=1;{void*_tmp20B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp20C;
void*_tmp20D;struct Cyc_Absyn_Aggrdecl**_tmp20E;struct Cyc_List_List*_tmp20F;
struct Cyc_List_List*_tmp210;struct Cyc_Absyn_TunionFieldInfo _tmp211;void*_tmp212;
struct Cyc_Absyn_Tunionfield*_tmp213;_LL163: if(_tmp20B <= (void*)3?1:*((int*)
_tmp20B)!= 10)goto _LL165;_tmp20C=((struct Cyc_Absyn_AggrType_struct*)_tmp20B)->f1;
_tmp20D=(void*)_tmp20C.aggr_info;if(*((int*)_tmp20D)!= 1)goto _LL165;_tmp20E=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp20D)->f1;_LL164: if((*((struct Cyc_Absyn_Aggrdecl**)
_tmp20E))->impl == 0)goto _LL162;_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*((struct Cyc_Absyn_Aggrdecl**)_tmp20E))->impl))->fields;goto _LL166;
_LL165: if(_tmp20B <= (void*)3?1:*((int*)_tmp20B)!= 11)goto _LL167;_tmp20F=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp20B)->f2;_LL166: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp20F)<= _tmp1FC)({struct Cyc_Int_pa_struct _tmp217;_tmp217.tag=
1;_tmp217.f1=(unsigned int)((int)_tmp1FC);{struct Cyc_Int_pa_struct _tmp216;
_tmp216.tag=1;_tmp216.f1=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp20F);{void*_tmp214[2]={& _tmp216,& _tmp217};Cyc_Tcutil_terr(loc,({const char*
_tmp215="struct/union has too few components: %d <= %d";_tag_arr(_tmp215,sizeof(
char),_get_zero_arr_size(_tmp215,46));}),_tag_arr(_tmp214,sizeof(void*),2));}}});
bad_type=0;goto _LL162;_LL167: if(_tmp20B <= (void*)3?1:*((int*)_tmp20B)!= 9)goto
_LL169;_tmp210=((struct Cyc_Absyn_TupleType_struct*)_tmp20B)->f1;_LL168: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp210)<= _tmp1FC)({struct Cyc_Int_pa_struct
_tmp21B;_tmp21B.tag=1;_tmp21B.f1=(unsigned int)((int)_tmp1FC);{struct Cyc_Int_pa_struct
_tmp21A;_tmp21A.tag=1;_tmp21A.f1=(unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp210);{void*_tmp218[2]={& _tmp21A,& _tmp21B};Cyc_Tcutil_terr(
loc,({const char*_tmp219="tuple has too few components: %d <= %d";_tag_arr(
_tmp219,sizeof(char),_get_zero_arr_size(_tmp219,39));}),_tag_arr(_tmp218,sizeof(
void*),2));}}});bad_type=0;goto _LL162;_LL169: if(_tmp20B <= (void*)3?1:*((int*)
_tmp20B)!= 3)goto _LL16B;_tmp211=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp20B)->f1;_tmp212=(void*)_tmp211.field_info;if(*((int*)_tmp212)!= 1)goto
_LL16B;_tmp213=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp212)->f2;_LL16A:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp213->typs)< _tmp1FC)({
struct Cyc_Int_pa_struct _tmp21F;_tmp21F.tag=1;_tmp21F.f1=(unsigned int)((int)
_tmp1FC);{struct Cyc_Int_pa_struct _tmp21E;_tmp21E.tag=1;_tmp21E.f1=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp213->typs);{void*_tmp21C[2]={&
_tmp21E,& _tmp21F};Cyc_Tcutil_terr(loc,({const char*_tmp21D="tunionfield has too few components: %d < %d";
_tag_arr(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,44));}),_tag_arr(
_tmp21C,sizeof(void*),2));}}});bad_type=0;goto _LL162;_LL16B:;_LL16C: goto _LL162;
_LL162:;}if(bad_type)({struct Cyc_String_pa_struct _tmp222;_tmp222.tag=0;_tmp222.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp220[1]={&
_tmp222};Cyc_Tcutil_terr(loc,({const char*_tmp221="%s is not a known type";
_tag_arr(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,23));}),_tag_arr(
_tmp220,sizeof(void*),1));}});goto _LL156;}_LL156:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp223=t;struct Cyc_Absyn_PtrInfo
_tmp224;void*_tmp225;void*_tmp226;struct Cyc_Absyn_Conref*_tmp227;struct Cyc_Absyn_Conref*
_tmp228;_LL16E: if(_tmp223 <= (void*)3?1:*((int*)_tmp223)!= 4)goto _LL170;_tmp224=((
struct Cyc_Absyn_PointerType_struct*)_tmp223)->f1;_tmp225=(void*)_tmp224.elt_typ;
_tmp226=(void*)_tmp224.rgn_typ;_tmp227=_tmp224.bounds;_tmp228=_tmp224.zero_term;
_LL16F: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp226);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp227);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp225),(void*)1))({
void*_tmp229[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp22A="can't dereference abstract pointer type";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,40));}),_tag_arr(
_tmp229,sizeof(void*),0));});return _tmp225;_LL170:;_LL171: return({struct Cyc_String_pa_struct
_tmp22D;_tmp22D.tag=0;_tmp22D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp22B[1]={& _tmp22D};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp22C="expecting * or @ type but found %s";
_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,35));}),_tag_arr(
_tmp22B,sizeof(void*),1));}});_LL16D:;}}static void*Cyc_Tcexp_tcAggrMember(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp22E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp22F;void*_tmp230;struct Cyc_Absyn_Aggrdecl**_tmp231;
struct Cyc_Absyn_Aggrdecl*_tmp232;struct Cyc_List_List*_tmp233;struct Cyc_List_List*
_tmp234;_LL173: if(_tmp22E <= (void*)3?1:*((int*)_tmp22E)!= 10)goto _LL175;_tmp22F=((
struct Cyc_Absyn_AggrType_struct*)_tmp22E)->f1;_tmp230=(void*)_tmp22F.aggr_info;
if(*((int*)_tmp230)!= 1)goto _LL175;_tmp231=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp230)->f1;_tmp232=*_tmp231;_tmp233=_tmp22F.targs;_LL174: {struct Cyc_Absyn_Aggrfield*
_tmp237=Cyc_Absyn_lookup_decl_field(_tmp232,f);if(_tmp237 == 0)return({struct Cyc_String_pa_struct
_tmp23B;_tmp23B.tag=0;_tmp23B.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)f));{struct Cyc_String_pa_struct _tmp23A;_tmp23A.tag=0;_tmp23A.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp232->name));{
void*_tmp238[2]={& _tmp23A,& _tmp23B};Cyc_Tcexp_expr_err(te,loc,({const char*
_tmp239="type %s has no %s field";_tag_arr(_tmp239,sizeof(char),
_get_zero_arr_size(_tmp239,24));}),_tag_arr(_tmp238,sizeof(void*),2));}}});if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp232->impl))->exist_vars != 0)return({
void*_tmp23C[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp23D="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp23D,sizeof(char),_get_zero_arr_size(_tmp23D,64));}),_tag_arr(
_tmp23C,sizeof(void*),0));});{void*t2;{struct _RegionHandle _tmp23E=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp23E;_push_region(rgn);{struct Cyc_List_List*_tmp23F=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp232->tvs,_tmp233);t2=Cyc_Tcutil_rsubstitute(
rgn,_tmp23F,(void*)_tmp237->type);};_pop_region(rgn);}return t2;}}_LL175: if(
_tmp22E <= (void*)3?1:*((int*)_tmp22E)!= 11)goto _LL177;_tmp234=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp22E)->f2;_LL176: {struct Cyc_Absyn_Aggrfield*_tmp240=Cyc_Absyn_lookup_field(
_tmp234,f);if(_tmp240 == 0)return({struct Cyc_String_pa_struct _tmp243;_tmp243.tag=
0;_tmp243.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)f));{
void*_tmp241[1]={& _tmp243};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp242="type has no %s field";
_tag_arr(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,21));}),_tag_arr(
_tmp241,sizeof(void*),1));}});return(void*)_tmp240->type;}_LL177: if(_tmp22E <= (
void*)3?1:*((int*)_tmp22E)!= 7)goto _LL179;if(!(Cyc_strcmp((struct _tagged_arr)*((
struct _tagged_arr*)f),({const char*_tmp235="size";_tag_arr(_tmp235,sizeof(char),
_get_zero_arr_size(_tmp235,5));}))== 0))goto _LL179;_LL178: goto _LL17A;_LL179: if(
_tmp22E <= (void*)3?1:*((int*)_tmp22E)!= 4)goto _LL17B;if(!(Cyc_strcmp((struct
_tagged_arr)*((struct _tagged_arr*)f),({const char*_tmp236="size";_tag_arr(_tmp236,
sizeof(char),_get_zero_arr_size(_tmp236,5));}))== 0))goto _LL17B;_LL17A:(void*)(
outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp244=_cycalloc(
sizeof(*_tmp244));_tmp244[0]=({struct Cyc_Absyn_Primop_e_struct _tmp245;_tmp245.tag=
3;_tmp245.f1=(void*)((void*)19);_tmp245.f2=({struct Cyc_List_List*_tmp246=
_cycalloc(sizeof(*_tmp246));_tmp246->hd=e;_tmp246->tl=0;_tmp246;});_tmp245;});
_tmp244;})));return Cyc_Absyn_uint_typ;_LL17B:;_LL17C: if(Cyc_strcmp((struct
_tagged_arr)*((struct _tagged_arr*)f),({const char*_tmp247="size";_tag_arr(_tmp247,
sizeof(char),_get_zero_arr_size(_tmp247,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp248[1]={&
_tmp24A};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp249="expecting struct, union, or array, found %s";
_tag_arr(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,44));}),_tag_arr(
_tmp248,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct _tmp24D;
_tmp24D.tag=0;_tmp24D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp24B[1]={&
_tmp24D};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp24C="expecting struct or union, found %s";
_tag_arr(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,36));}),_tag_arr(
_tmp24B,sizeof(void*),1));}});}_LL172:;}}static void*Cyc_Tcexp_tcAggrArrow(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp24E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp24F;void*_tmp250;void*_tmp251;struct Cyc_Absyn_Conref*_tmp252;struct Cyc_Absyn_Conref*
_tmp253;_LL17E: if(_tmp24E <= (void*)3?1:*((int*)_tmp24E)!= 4)goto _LL180;_tmp24F=((
struct Cyc_Absyn_PointerType_struct*)_tmp24E)->f1;_tmp250=(void*)_tmp24F.elt_typ;
_tmp251=(void*)_tmp24F.rgn_typ;_tmp252=_tmp24F.bounds;_tmp253=_tmp24F.zero_term;
_LL17F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp252);{void*_tmp254=Cyc_Tcutil_compress(
_tmp250);struct Cyc_Absyn_AggrInfo _tmp255;void*_tmp256;struct Cyc_Absyn_Aggrdecl**
_tmp257;struct Cyc_Absyn_Aggrdecl*_tmp258;struct Cyc_List_List*_tmp259;struct Cyc_List_List*
_tmp25A;_LL183: if(_tmp254 <= (void*)3?1:*((int*)_tmp254)!= 10)goto _LL185;_tmp255=((
struct Cyc_Absyn_AggrType_struct*)_tmp254)->f1;_tmp256=(void*)_tmp255.aggr_info;
if(*((int*)_tmp256)!= 1)goto _LL185;_tmp257=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp256)->f1;_tmp258=*_tmp257;_tmp259=_tmp255.targs;_LL184: {struct Cyc_Absyn_Aggrfield*
_tmp25B=Cyc_Absyn_lookup_decl_field(_tmp258,f);if(_tmp25B == 0)return({struct Cyc_String_pa_struct
_tmp25F;_tmp25F.tag=0;_tmp25F.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct _tagged_arr*)f));{struct Cyc_String_pa_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp258->name));{
void*_tmp25C[2]={& _tmp25E,& _tmp25F};Cyc_Tcexp_expr_err(te,loc,({const char*
_tmp25D="type %s has no %s field";_tag_arr(_tmp25D,sizeof(char),
_get_zero_arr_size(_tmp25D,24));}),_tag_arr(_tmp25C,sizeof(void*),2));}}});if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp258->impl))->exist_vars != 0)return({
void*_tmp260[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp261="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,64));}),_tag_arr(
_tmp260,sizeof(void*),0));});{void*t3;{struct _RegionHandle _tmp262=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp262;_push_region(rgn);{struct Cyc_List_List*_tmp263=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp258->tvs,_tmp259);t3=Cyc_Tcutil_rsubstitute(
rgn,_tmp263,(void*)_tmp25B->type);};_pop_region(rgn);}return t3;}}_LL185: if(
_tmp254 <= (void*)3?1:*((int*)_tmp254)!= 11)goto _LL187;_tmp25A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp254)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp264=Cyc_Absyn_lookup_field(
_tmp25A,f);if(_tmp264 == 0)return({struct Cyc_String_pa_struct _tmp267;_tmp267.tag=
0;_tmp267.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)f));{
void*_tmp265[1]={& _tmp267};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp266="type has no %s field";
_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,21));}),_tag_arr(
_tmp265,sizeof(void*),1));}});return(void*)_tmp264->type;}_LL187:;_LL188: goto
_LL182;_LL182:;}goto _LL17D;_LL180:;_LL181: goto _LL17D;_LL17D:;}return({struct Cyc_String_pa_struct
_tmp26A;_tmp26A.tag=0;_tmp26A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp268[1]={&
_tmp26A};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp269="expecting struct pointer, found %s";
_tag_arr(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,35));}),_tag_arr(
_tmp268,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int _tmp26C;int _tmp26D;struct _tuple7 _tmp26B=Cyc_Evexp_eval_const_uint_exp(
index);_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;if(!_tmp26D)return({void*_tmp26E[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp26F="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,47));}),_tag_arr(
_tmp26E,sizeof(void*),0));});{struct _handler_cons _tmp270;_push_handler(& _tmp270);{
int _tmp272=0;if(setjmp(_tmp270.handler))_tmp272=1;if(!_tmp272){{void*_tmp273=(*((
struct _tuple9*)((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(
int)_tmp26C))).f2;_npop_handler(0);return _tmp273;};_pop_handler();}else{void*
_tmp271=(void*)_exn_thrown;void*_tmp275=_tmp271;_LL18A: if(_tmp275 != Cyc_List_Nth)
goto _LL18C;_LL18B: return({struct Cyc_Int_pa_struct _tmp279;_tmp279.tag=1;_tmp279.f1=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp278;_tmp278.tag=1;_tmp278.f1=(unsigned int)((int)_tmp26C);{void*_tmp276[2]={&
_tmp278,& _tmp279};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp277="index is %d but tuple has only %d fields";
_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,41));}),_tag_arr(
_tmp276,sizeof(void*),2));}}});_LL18C:;_LL18D:(void)_throw(_tmp275);_LL189:;}}}}
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(te,e2))return({struct
Cyc_String_pa_struct _tmp27C;_tmp27C.tag=0;_tmp27C.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_typ2string(t2));{void*_tmp27A[1]={& _tmp27C};Cyc_Tcexp_expr_err(
te,e2->loc,({const char*_tmp27B="expecting int subscript, found %s";_tag_arr(
_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,34));}),_tag_arr(_tmp27A,sizeof(
void*),1));}});{void*_tmp27D=t1;struct Cyc_Absyn_PtrInfo _tmp27E;void*_tmp27F;void*
_tmp280;struct Cyc_Absyn_Tqual _tmp281;struct Cyc_Absyn_Conref*_tmp282;struct Cyc_Absyn_Conref*
_tmp283;struct Cyc_List_List*_tmp284;_LL18F: if(_tmp27D <= (void*)3?1:*((int*)
_tmp27D)!= 4)goto _LL191;_tmp27E=((struct Cyc_Absyn_PointerType_struct*)_tmp27D)->f1;
_tmp27F=(void*)_tmp27E.elt_typ;_tmp280=(void*)_tmp27E.rgn_typ;_tmp281=_tmp27E.tq;
_tmp282=_tmp27E.bounds;_tmp283=_tmp27E.zero_term;_LL190: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp283)){int emit_warning=1;{void*
_tmp285=(void*)(Cyc_Absyn_compress_conref(_tmp282))->v;void*_tmp286;struct Cyc_Absyn_Exp*
_tmp287;void*_tmp288;_LL196: if(_tmp285 <= (void*)1?1:*((int*)_tmp285)!= 0)goto
_LL198;_tmp286=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp285)->f1;if(
_tmp286 <= (void*)1?1:*((int*)_tmp286)!= 0)goto _LL198;_tmp287=((struct Cyc_Absyn_Upper_b_struct*)
_tmp286)->f1;_LL197: if(Cyc_Tcutil_is_const_exp(te,e2)){unsigned int _tmp28A;int
_tmp28B;struct _tuple7 _tmp289=Cyc_Evexp_eval_const_uint_exp(e2);_tmp28A=_tmp289.f1;
_tmp28B=_tmp289.f2;if(_tmp28B){unsigned int _tmp28D;int _tmp28E;struct _tuple7
_tmp28C=Cyc_Evexp_eval_const_uint_exp(_tmp287);_tmp28D=_tmp28C.f1;_tmp28E=
_tmp28C.f2;if(_tmp28E?_tmp28D > _tmp28A: 0)emit_warning=0;}}goto _LL195;_LL198: if(
_tmp285 <= (void*)1?1:*((int*)_tmp285)!= 0)goto _LL19A;_tmp288=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp285)->f1;if((int)_tmp288 != 0)goto _LL19A;_LL199: emit_warning=0;goto _LL195;
_LL19A:;_LL19B: goto _LL195;_LL195:;}if(emit_warning)({void*_tmp28F[0]={};Cyc_Tcutil_warn(
e2->loc,({const char*_tmp290="subscript on thin, zero-terminated pointer could be expensive.";
_tag_arr(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,63));}),_tag_arr(
_tmp28F,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(te,e2)){
unsigned int _tmp292;int _tmp293;struct _tuple7 _tmp291=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp292=_tmp291.f1;_tmp293=_tmp291.f2;if(_tmp293)Cyc_Tcutil_check_bound(loc,
_tmp292,_tmp282);}else{if(Cyc_Tcutil_is_bound_one(_tmp282)?!((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp283): 0)({void*_tmp294[0]={};Cyc_Tcutil_warn(
e1->loc,({const char*_tmp295="subscript applied to pointer to one object";
_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,43));}),_tag_arr(
_tmp294,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,_tmp282);}}Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp280);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp27F),(void*)1))({
void*_tmp296[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp297="can't subscript an abstract pointer";
_tag_arr(_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,36));}),_tag_arr(
_tmp296,sizeof(void*),0));});return _tmp27F;_LL191: if(_tmp27D <= (void*)3?1:*((int*)
_tmp27D)!= 9)goto _LL193;_tmp284=((struct Cyc_Absyn_TupleType_struct*)_tmp27D)->f1;
_LL192: return Cyc_Tcexp_ithTupleType(te,loc,_tmp284,e2);_LL193:;_LL194: return({
struct Cyc_String_pa_struct _tmp29A;_tmp29A.tag=0;_tmp29A.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp298[1]={& _tmp29A};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp299="subscript applied to %s";_tag_arr(_tmp299,sizeof(
char),_get_zero_arr_size(_tmp299,24));}),_tag_arr(_tmp298,sizeof(void*),1));}});
_LL18E:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp29B=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_List_List*
_tmp29C;_LL19D: if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 9)goto _LL19F;_tmp29C=((
struct Cyc_Absyn_TupleType_struct*)_tmp29B)->f1;_LL19E: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp29C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es))goto _LL19C;for(0;es != 0;(es=es->tl,_tmp29C=_tmp29C->tl)){void*_tmp29D=(*((
struct _tuple9*)((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp29C))->hd))).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp29D,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E->hd=({
struct _tuple9*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->f1=(*((struct _tuple9*)((
struct _tuple9*)_tmp29C->hd))).f1;_tmp29F->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp29F;});_tmp29E->tl=
fields;_tmp29E;});}done=1;goto _LL19C;_LL19F:;_LL1A0: goto _LL19C;_LL19C:;}if(!done)
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->hd=({
struct _tuple9*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->f1=Cyc_Absyn_empty_tqual();
_tmp2A1->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2A1;});_tmp2A0->tl=fields;_tmp2A0;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2A3;_tmp2A3.tag=9;_tmp2A3.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2A3;});_tmp2A2;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2A4[0]={};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp2A5="tcCompoundLit";_tag_arr(_tmp2A5,sizeof(char),
_get_zero_arr_size(_tmp2A5,14));}),_tag_arr(_tmp2A4,sizeof(void*),0));});}static
void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**elt_topt,int zero_term,struct Cyc_List_List*des){struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*
res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp2C0;}));int _tmp2A6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
struct Cyc_Absyn_Const_e_struct*_tmp2A7=({struct Cyc_Absyn_Const_e_struct*_tmp2BC=
_cycalloc(sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_Absyn_Const_e_struct _tmp2BD;
_tmp2BD.tag=0;_tmp2BD.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp2BE=
_cycalloc(sizeof(*_tmp2BE));_tmp2BE[0]=({struct Cyc_Absyn_Int_c_struct _tmp2BF;
_tmp2BF.tag=2;_tmp2BF.f1=(void*)((void*)1);_tmp2BF.f2=_tmp2A6;_tmp2BF;});_tmp2BE;}));
_tmp2BD;});_tmp2BC;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2A7,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2A8=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2A6 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2A8))({void*_tmp2A9[0]={};Cyc_Tcutil_terr(_tmp2A8->loc,({const char*_tmp2AA="zero-terminated array doesn't end with zero.";
_tag_arr(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,45));}),_tag_arr(
_tmp2A9,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp2AB=
_cycalloc(sizeof(*_tmp2AB));_tmp2AB->v=(void*)Cyc_Absyn_uint_typ;_tmp2AB;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
res));{void*_tmp2AC[1]={& _tmp2AE};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp2AD="elements of array do not all have the same type (%s)";
_tag_arr(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,53));}),_tag_arr(
_tmp2AC,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple8*)((struct _tuple8*)des->hd))).f1;if(ds != 
0){void*_tmp2AF=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2B0;_LL1A2: if(*((int*)
_tmp2AF)!= 1)goto _LL1A4;_LL1A3:({void*_tmp2B1[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp2B2="only array index designators are supported";_tag_arr(_tmp2B2,
sizeof(char),_get_zero_arr_size(_tmp2B2,43));}),_tag_arr(_tmp2B1,sizeof(void*),0));});
goto _LL1A1;_LL1A4: if(*((int*)_tmp2AF)!= 0)goto _LL1A1;_tmp2B0=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2AF)->f1;_LL1A5: Cyc_Tcexp_tcExpInitializer(te,0,_tmp2B0);{unsigned int
_tmp2B4;int _tmp2B5;struct _tuple7 _tmp2B3=Cyc_Evexp_eval_const_uint_exp(_tmp2B0);
_tmp2B4=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;if(!_tmp2B5)({void*_tmp2B6[0]={};Cyc_Tcutil_terr(
_tmp2B0->loc,({const char*_tmp2B7="index designator cannot use sizeof or offsetof";
_tag_arr(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,47));}),_tag_arr(
_tmp2B6,sizeof(void*),0));});else{if(_tmp2B4 != offset)({struct Cyc_Int_pa_struct
_tmp2BB;_tmp2BB.tag=1;_tmp2BB.f1=(unsigned int)((int)_tmp2B4);{struct Cyc_Int_pa_struct
_tmp2BA;_tmp2BA.tag=1;_tmp2BA.f1=(unsigned int)offset;{void*_tmp2B8[2]={& _tmp2BA,&
_tmp2BB};Cyc_Tcutil_terr(_tmp2B0->loc,({const char*_tmp2B9="expecting index designator %d but found %d";
_tag_arr(_tmp2B9,sizeof(char),_get_zero_arr_size(_tmp2B9,43));}),_tag_arr(
_tmp2B8,sizeof(void*),2));}}});}goto _LL1A1;}_LL1A1:;}}}return res_t2;}}static void*
Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v));{void*_tmp2C1[1]={& _tmp2C3};Cyc_Tcutil_terr(bound->loc,({
const char*_tmp2C2="expecting unsigned int, found %s";_tag_arr(_tmp2C2,sizeof(
char),_get_zero_arr_size(_tmp2C2,33));}),_tag_arr(_tmp2C1,sizeof(void*),1));}});
if(!(vd->tq).q_const)({void*_tmp2C4[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2C5="comprehension index variable is not declared const!";
_tag_arr(_tmp2C5,sizeof(char),_get_zero_arr_size(_tmp2C5,52));}),_tag_arr(
_tmp2C4,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(loc,te);te=
Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;void**_tmp2C6=
0;struct Cyc_Absyn_Tqual*_tmp2C7=0;struct Cyc_Absyn_Conref**_tmp2C8=0;if(topt != 0){
void*_tmp2C9=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo _tmp2CA;
struct Cyc_Absyn_ArrayInfo _tmp2CB;void*_tmp2CC;void**_tmp2CD;struct Cyc_Absyn_Tqual
_tmp2CE;struct Cyc_Absyn_Tqual*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Conref*
_tmp2D1;struct Cyc_Absyn_Conref**_tmp2D2;_LL1A7: if(_tmp2C9 <= (void*)3?1:*((int*)
_tmp2C9)!= 4)goto _LL1A9;_tmp2CA=((struct Cyc_Absyn_PointerType_struct*)_tmp2C9)->f1;
_LL1A8: pinfo=_tmp2CA;_tmp2C6=(void**)((void**)((void*)& pinfo.elt_typ));_tmp2C7=(
struct Cyc_Absyn_Tqual*)& pinfo.tq;_tmp2C8=(struct Cyc_Absyn_Conref**)& pinfo.zero_term;
goto _LL1A6;_LL1A9: if(_tmp2C9 <= (void*)3?1:*((int*)_tmp2C9)!= 7)goto _LL1AB;
_tmp2CB=((struct Cyc_Absyn_ArrayType_struct*)_tmp2C9)->f1;_tmp2CC=(void*)_tmp2CB.elt_type;
_tmp2CD=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp2C9)->f1).elt_type;
_tmp2CE=_tmp2CB.tq;_tmp2CF=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp2C9)->f1).tq;_tmp2D0=_tmp2CB.num_elts;_tmp2D1=_tmp2CB.zero_term;_tmp2D2=(
struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp2C9)->f1).zero_term;
_LL1AA: _tmp2C6=(void**)_tmp2CD;_tmp2C7=(struct Cyc_Absyn_Tqual*)_tmp2CF;_tmp2C8=(
struct Cyc_Absyn_Conref**)_tmp2D2;goto _LL1A6;_LL1AB:;_LL1AC: goto _LL1A6;_LL1A6:;}{
void*t=Cyc_Tcexp_tcExp(te,_tmp2C6,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp2D3[0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp2D4="bound is not constant";
_tag_arr(_tmp2D4,sizeof(char),_get_zero_arr_size(_tmp2D4,22));}),_tag_arr(
_tmp2D3,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(te,body))({void*_tmp2D5[
0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp2D6="body is not constant";
_tag_arr(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,21));}),_tag_arr(
_tmp2D5,sizeof(void*),0));});}if(_tmp2C8 != 0?((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,*((struct Cyc_Absyn_Conref**)_tmp2C8)): 0){struct Cyc_Absyn_Exp*
_tmp2D7=Cyc_Absyn_uint_exp(1,0);_tmp2D7->topt=({struct Cyc_Core_Opt*_tmp2D8=
_cycalloc(sizeof(*_tmp2D8));_tmp2D8->v=(void*)Cyc_Absyn_uint_typ;_tmp2D8;});
bound=Cyc_Absyn_add_exp(bound,_tmp2D7,0);bound->topt=({struct Cyc_Core_Opt*
_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9->v=(void*)Cyc_Absyn_uint_typ;_tmp2D9;});*((
int*)is_zero_term)=1;}return Cyc_Absyn_array_typ(t,_tmp2C7 == 0?Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*)_tmp2C7),(struct Cyc_Absyn_Exp*)bound,_tmp2C8 == 0?Cyc_Absyn_false_conref:*((
struct Cyc_Absyn_Conref**)_tmp2C8));}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*((struct Cyc_Absyn_Aggrdecl**)ad_opt)!= 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*((struct Cyc_Absyn_Aggrdecl**)ad_opt));else{{struct _handler_cons
_tmp2DA;_push_handler(& _tmp2DA);{int _tmp2DC=0;if(setjmp(_tmp2DA.handler))_tmp2DC=
1;if(!_tmp2DC){ad=*((struct Cyc_Absyn_Aggrdecl**)Cyc_Tcenv_lookup_aggrdecl(te,loc,*((
struct _tuple1**)tn)));;_pop_handler();}else{void*_tmp2DB=(void*)_exn_thrown;void*
_tmp2DE=_tmp2DB;_LL1AE: if(_tmp2DE != Cyc_Dict_Absent)goto _LL1B0;_LL1AF:({struct
Cyc_String_pa_struct _tmp2E1;_tmp2E1.tag=0;_tmp2E1.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(*((struct _tuple1**)tn)));{void*_tmp2DF[1]={&
_tmp2E1};Cyc_Tcutil_terr(loc,({const char*_tmp2E0="unbound struct name %s";
_tag_arr(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,23));}),_tag_arr(
_tmp2DF,sizeof(void*),1));}});return topt != 0?*((void**)topt):(void*)0;_LL1B0:;
_LL1B1:(void)_throw(_tmp2DE);_LL1AD:;}}}*((struct Cyc_Absyn_Aggrdecl**)ad_opt)=(
struct Cyc_Absyn_Aggrdecl*)ad;*((struct _tuple1**)tn)=ad->name;}if((void*)ad->kind
== (void*)1)({void*_tmp2E2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2E3="expecting struct but found union";
_tag_arr(_tmp2E3,sizeof(char),_get_zero_arr_size(_tmp2E3,33));}),_tag_arr(
_tmp2E2,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp2E4[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp2E5="can't build abstract struct";_tag_arr(_tmp2E5,sizeof(
char),_get_zero_arr_size(_tmp2E5,28));}),_tag_arr(_tmp2E4,sizeof(void*),0));});
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp2E6=_cycalloc(sizeof(*_tmp2E6));
_tmp2E6->v=Cyc_Tcenv_lookup_type_vars(te);_tmp2E6;}));}{struct _RegionHandle
_tmp2E7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E7;_push_region(rgn);{
struct _tuple4 _tmp2E8=({struct _tuple4 _tmp309;_tmp309.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp309.f2=rgn;_tmp309;});struct Cyc_List_List*_tmp2E9=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp2E8,ad->tvs);struct Cyc_List_List*_tmp2EA=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp2E8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp2EB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp2E9);struct Cyc_List_List*_tmp2EC=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp2EA);struct Cyc_List_List*_tmp2ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp2E9,_tmp2EA);struct Cyc_Absyn_AggrType_struct*_tmp2EE=({struct Cyc_Absyn_AggrType_struct*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_Absyn_AggrType_struct
_tmp304;_tmp304.tag=10;_tmp304.f1=({struct Cyc_Absyn_AggrInfo _tmp305;_tmp305.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp306=_cycalloc(sizeof(*
_tmp306));_tmp306[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp307;_tmp307.tag=1;
_tmp307.f1=({struct Cyc_Absyn_Aggrdecl**_tmp308=_cycalloc(sizeof(*_tmp308));
_tmp308[0]=ad;_tmp308;});_tmp307;});_tmp306;}));_tmp305.targs=_tmp2EB;_tmp305;});
_tmp304;});_tmp303;});struct Cyc_List_List*_tmp2EF=*((struct Cyc_List_List**)ts);
struct Cyc_List_List*_tmp2F0=_tmp2EC;while(_tmp2EF != 0?_tmp2F0 != 0: 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp2EF->hd);Cyc_Tcutil_unify((
void*)_tmp2EF->hd,(void*)_tmp2F0->hd);_tmp2EF=_tmp2EF->tl;_tmp2F0=_tmp2F0->tl;}
if(_tmp2EF != 0)({void*_tmp2F1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2F2="too many explicit witness types";
_tag_arr(_tmp2F2,sizeof(char),_get_zero_arr_size(_tmp2F2,32));}),_tag_arr(
_tmp2F1,sizeof(void*),0));});*((struct Cyc_List_List**)ts)=_tmp2EC;if(topt != 0)
Cyc_Tcutil_unify((void*)_tmp2EE,*((void**)topt));{struct Cyc_List_List*fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp2F4;struct Cyc_Absyn_Aggrfield*
_tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;struct _tuple10*_tmp2F3=(struct _tuple10*)
fields->hd;_tmp2F4=*_tmp2F3;_tmp2F5=_tmp2F4.f1;_tmp2F6=_tmp2F4.f2;{void*_tmp2F7=
Cyc_Tcutil_rsubstitute(rgn,_tmp2ED,(void*)_tmp2F5->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp2F7,_tmp2F6);if(!Cyc_Tcutil_coerce_arg(te,_tmp2F6,_tmp2F7)){({
struct Cyc_String_pa_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F6->topt))->v));{struct Cyc_String_pa_struct _tmp2FC;_tmp2FC.tag=
0;_tmp2FC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp2F7));{struct Cyc_String_pa_struct _tmp2FB;_tmp2FB.tag=0;_tmp2FB.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(*((struct _tuple1**)tn)));{
struct Cyc_String_pa_struct _tmp2FA;_tmp2FA.tag=0;_tmp2FA.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct _tagged_arr*)_tmp2F5->name));{void*_tmp2F8[4]={&
_tmp2FA,& _tmp2FB,& _tmp2FC,& _tmp2FD};Cyc_Tcutil_terr(_tmp2F6->loc,({const char*
_tmp2F9="field %s of struct %s expects type %s != %s";_tag_arr(_tmp2F9,sizeof(
char),_get_zero_arr_size(_tmp2F9,44));}),_tag_arr(_tmp2F8,sizeof(void*),4));}}}}});
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp2FE=0;{struct Cyc_List_List*
_tmp2FF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp2FF != 0;_tmp2FF=_tmp2FF->tl){_tmp2FE=({struct Cyc_List_List*_tmp300=_cycalloc(
sizeof(*_tmp300));_tmp300->hd=({struct _tuple0*_tmp301=_cycalloc(sizeof(*_tmp301));
_tmp301->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp2ED,(*((struct _tuple0*)((struct
_tuple0*)_tmp2FF->hd))).f1);_tmp301->f2=Cyc_Tcutil_rsubstitute(rgn,_tmp2ED,(*((
struct _tuple0*)((struct _tuple0*)_tmp2FF->hd))).f2);_tmp301;});_tmp300->tl=
_tmp2FE;_tmp300;});}}_tmp2FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp2FE);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp2FE);{
void*_tmp302=(void*)_tmp2EE;_npop_handler(0);return _tmp302;}}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp30A=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp30A;_push_region(rgn);{void*_tmp30B=Cyc_Tcutil_compress(
ts);void*_tmp30C;struct Cyc_List_List*_tmp30D;_LL1B3: if(_tmp30B <= (void*)3?1:*((
int*)_tmp30B)!= 11)goto _LL1B5;_tmp30C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp30B)->f1;_tmp30D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp30B)->f2;_LL1B4:
if(_tmp30C == (void*)1)({void*_tmp30E[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp30F="expecting struct but found union";_tag_arr(_tmp30F,sizeof(char),
_get_zero_arr_size(_tmp30F,33));}),_tag_arr(_tmp30E,sizeof(void*),0));});{struct
Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp30D);for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp311;
struct Cyc_Absyn_Aggrfield*_tmp312;struct Cyc_Absyn_Exp*_tmp313;struct _tuple10*
_tmp310=(struct _tuple10*)fields->hd;_tmp311=*_tmp310;_tmp312=_tmp311.f1;_tmp313=
_tmp311.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp312->type)),
_tmp313);if(!Cyc_Tcutil_coerce_arg(te,_tmp313,(void*)_tmp312->type)){({struct Cyc_String_pa_struct
_tmp318;_tmp318.tag=0;_tmp318.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp313->topt))->v));{struct Cyc_String_pa_struct
_tmp317;_tmp317.tag=0;_tmp317.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp312->type));{struct Cyc_String_pa_struct _tmp316;_tmp316.tag=0;_tmp316.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_tmp312->name));{
void*_tmp314[3]={& _tmp316,& _tmp317,& _tmp318};Cyc_Tcutil_terr(_tmp313->loc,({
const char*_tmp315="field %s of struct expects type %s != %s";_tag_arr(_tmp315,
sizeof(char),_get_zero_arr_size(_tmp315,41));}),_tag_arr(_tmp314,sizeof(void*),3));}}}});
Cyc_Tcutil_explain_failure();}}goto _LL1B2;}_LL1B5:;_LL1B6:({void*_tmp319[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp31A="tcAnonStruct: wrong type";_tag_arr(_tmp31A,sizeof(char),
_get_zero_arr_size(_tmp31A,25));}),_tag_arr(_tmp319,sizeof(void*),0));});_LL1B2:;};
_pop_region(rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf){struct
_RegionHandle _tmp31B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp31B;
_push_region(rgn);{struct _tuple4 _tmp31C=({struct _tuple4 _tmp33F;_tmp33F.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp33F.f2=rgn;_tmp33F;});struct Cyc_List_List*_tmp31D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp31C,tud->tvs);struct Cyc_List_List*_tmp31E=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp31D);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp33B;_tmp33B.tag=3;_tmp33B.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp33C;
_tmp33C.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp33E;_tmp33E.tag=1;_tmp33E.f1=tud;_tmp33E.f2=tuf;_tmp33E;});_tmp33D;}));
_tmp33C.targs=_tmp31E;_tmp33C;});_tmp33B;});_tmp33A;});if(topt != 0){void*_tmp31F=
Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_TunionInfo _tmp320;struct Cyc_List_List*
_tmp321;void*_tmp322;_LL1B8: if(_tmp31F <= (void*)3?1:*((int*)_tmp31F)!= 3)goto
_LL1BA;_LL1B9: Cyc_Tcutil_unify(*((void**)topt),res);goto _LL1B7;_LL1BA: if(_tmp31F
<= (void*)3?1:*((int*)_tmp31F)!= 2)goto _LL1BC;_tmp320=((struct Cyc_Absyn_TunionType_struct*)
_tmp31F)->f1;_tmp321=_tmp320.targs;_tmp322=(void*)_tmp320.rgn;_LL1BB:{struct Cyc_List_List*
a=_tmp31E;for(0;a != 0?_tmp321 != 0: 0;(a=a->tl,_tmp321=_tmp321->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp321->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323->v=(void*)res;_tmp323;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({
struct Cyc_Absyn_TunionType_struct _tmp325;_tmp325.tag=2;_tmp325.f1=({struct Cyc_Absyn_TunionInfo
_tmp326;_tmp326.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp328;_tmp328.tag=1;_tmp328.f1=({struct Cyc_Absyn_Tuniondecl**_tmp329=_cycalloc(
sizeof(*_tmp329));_tmp329[0]=tud;_tmp329;});_tmp328;});_tmp327;}));_tmp326.targs=
_tmp31E;_tmp326.rgn=(void*)_tmp322;_tmp326;});_tmp325;});_tmp324;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp32A=res;_npop_handler(0);return _tmp32A;}}goto _LL1B7;_LL1BC:;
_LL1BD: goto _LL1B7;_LL1B7:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp31D,(*((struct _tuple9*)((struct _tuple9*)ts->hd))).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp32F;_tmp32F.tag=0;_tmp32F.f1=(struct _tagged_arr)((
struct _tagged_arr)(e->topt == 0?({const char*_tmp330="?";_tag_arr(_tmp330,sizeof(
char),_get_zero_arr_size(_tmp330,2));}): Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v)));{struct Cyc_String_pa_struct _tmp32E;
_tmp32E.tag=0;_tmp32E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp32D;_tmp32D.tag=0;_tmp32D.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp32B[3]={& _tmp32D,& _tmp32E,& _tmp32F};Cyc_Tcutil_terr(e->loc,({const char*
_tmp32C="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_arr(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,80));}),_tag_arr(
_tmp32B,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0){void*
_tmp334=({struct Cyc_String_pa_struct _tmp333;_tmp333.tag=0;_tmp333.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp331[1]={& _tmp333};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,({
const char*_tmp332="too many arguments for tunion constructor %s";_tag_arr(
_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,45));}),_tag_arr(_tmp331,sizeof(
void*),1));}});_npop_handler(0);return _tmp334;}if(ts != 0){void*_tmp338=({struct
Cyc_String_pa_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp335[1]={& _tmp337};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp336="too few arguments for tunion constructor %s";
_tag_arr(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,44));}),_tag_arr(
_tmp335,sizeof(void*),1));}});_npop_handler(0);return _tmp338;}{void*_tmp339=res;
_npop_handler(0);return _tmp339;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp340=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp341;
struct Cyc_Absyn_Conref*_tmp342;struct Cyc_Absyn_ArrayInfo _tmp343;void*_tmp344;
struct Cyc_List_List*_tmp345;struct Cyc_Absyn_AggrInfo _tmp346;void*_tmp347;struct
Cyc_List_List*_tmp348;struct Cyc_List_List*_tmp349;_LL1BF: if((int)_tmp340 != 0)
goto _LL1C1;_LL1C0: return 1;_LL1C1: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 0)
goto _LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 1)
goto _LL1C5;_LL1C4: goto _LL1C6;_LL1C5: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 2)
goto _LL1C7;_LL1C6: goto _LL1C8;_LL1C7: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 3)
goto _LL1C9;_LL1C8: return 0;_LL1C9: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 4)
goto _LL1CB;_tmp341=((struct Cyc_Absyn_PointerType_struct*)_tmp340)->f1;_tmp342=
_tmp341.nullable;_LL1CA: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp342);_LL1CB: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 5)goto _LL1CD;_LL1CC:
goto _LL1CE;_LL1CD: if((int)_tmp340 != 1)goto _LL1CF;_LL1CE: goto _LL1D0;_LL1CF: if(
_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 6)goto _LL1D1;_LL1D0: return 1;_LL1D1: if(
_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 7)goto _LL1D3;_tmp343=((struct Cyc_Absyn_ArrayType_struct*)
_tmp340)->f1;_tmp344=(void*)_tmp343.elt_type;_LL1D2: return Cyc_Tcexp_zeroable_type(
_tmp344);_LL1D3: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 8)goto _LL1D5;_LL1D4:
return 0;_LL1D5: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 9)goto _LL1D7;_tmp345=((
struct Cyc_Absyn_TupleType_struct*)_tmp340)->f1;_LL1D6: for(0;(unsigned int)
_tmp345;_tmp345=_tmp345->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)((
struct _tuple9*)_tmp345->hd))).f2))return 0;}return 1;_LL1D7: if(_tmp340 <= (void*)3?
1:*((int*)_tmp340)!= 10)goto _LL1D9;_tmp346=((struct Cyc_Absyn_AggrType_struct*)
_tmp340)->f1;_tmp347=(void*)_tmp346.aggr_info;_tmp348=_tmp346.targs;_LL1D8: {
struct Cyc_Absyn_Aggrdecl*_tmp34A=Cyc_Absyn_get_known_aggrdecl(_tmp347);if(
_tmp34A->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34A->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp34B=_new_region("r");struct _RegionHandle*r=&
_tmp34B;_push_region(r);{struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp34A->tvs,_tmp348);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp34A->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp34C,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp34D=0;_npop_handler(0);return _tmp34D;}}}{int _tmp34E=1;_npop_handler(0);
return _tmp34E;}};_pop_region(r);}}_LL1D9: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)
!= 12)goto _LL1DB;_LL1DA: return 1;_LL1DB: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)
!= 14)goto _LL1DD;_LL1DC: return 1;_LL1DD: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)
!= 17)goto _LL1DF;_LL1DE: return 1;_LL1DF: if(_tmp340 <= (void*)3?1:*((int*)_tmp340)
!= 11)goto _LL1E1;_tmp349=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp340)->f2;
_LL1E0: for(0;_tmp349 != 0;_tmp349=_tmp349->tl){if(!Cyc_Tcexp_zeroable_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp349->hd)->type))return 0;}return 1;_LL1E1: if(
_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 13)goto _LL1E3;_LL1E2: return 1;_LL1E3: if(
_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 16)goto _LL1E5;_LL1E4: return 0;_LL1E5: if(
_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 15)goto _LL1E7;_LL1E6: return 0;_LL1E7: if(
_tmp340 <= (void*)3?1:*((int*)_tmp340)!= 18)goto _LL1E9;_LL1E8: goto _LL1EA;_LL1E9:
if((int)_tmp340 != 2)goto _LL1EB;_LL1EA: goto _LL1EC;_LL1EB: if(_tmp340 <= (void*)3?1:*((
int*)_tmp340)!= 19)goto _LL1ED;_LL1EC: goto _LL1EE;_LL1ED: if(_tmp340 <= (void*)3?1:*((
int*)_tmp340)!= 20)goto _LL1EF;_LL1EE: goto _LL1F0;_LL1EF: if(_tmp340 <= (void*)3?1:*((
int*)_tmp340)!= 21)goto _LL1BE;_LL1F0:({struct Cyc_String_pa_struct _tmp351;_tmp351.tag=
0;_tmp351.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp34F[1]={& _tmp351};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp350="bad type `%s' in zeroable type";_tag_arr(
_tmp350,sizeof(char),_get_zero_arr_size(_tmp350,31));}),_tag_arr(_tmp34F,sizeof(
void*),1));}});_LL1BE:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t)?1:(
allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;if(topt != 0){void*_tmp352=Cyc_Tcutil_compress(*((
void**)topt));struct Cyc_Absyn_PtrInfo _tmp353;void*_tmp354;_LL1F2: if(_tmp352 <= (
void*)3?1:*((int*)_tmp352)!= 4)goto _LL1F4;_tmp353=((struct Cyc_Absyn_PointerType_struct*)
_tmp352)->f1;_tmp354=(void*)_tmp353.elt_typ;_LL1F3: Cyc_Tcutil_unify(_tmp354,t);
if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;
goto _LL1F1;_LL1F4:;_LL1F5: goto _LL1F1;_LL1F1:;}({struct Cyc_String_pa_struct
_tmp358;_tmp358.tag=0;_tmp358.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp357;_tmp357.tag=0;_tmp357.f1=(struct
_tagged_arr)((struct _tagged_arr)(allow_zero?({const char*_tmp359="calloc";
_tag_arr(_tmp359,sizeof(char),_get_zero_arr_size(_tmp359,7));}):({const char*
_tmp35A="malloc";_tag_arr(_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,7));})));{
void*_tmp355[2]={& _tmp357,& _tmp358};Cyc_Tcutil_terr(loc,({const char*_tmp356="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_arr(_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,60));}),_tag_arr(
_tmp355,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp360=
_cycalloc(sizeof(*_tmp360));_tmp360[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp361;_tmp361.tag=15;_tmp361.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp362;}));_tmp361;});_tmp360;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp35B=Cyc_Tcutil_compress(handle_type);void*_tmp35C;
_LL1F7: if(_tmp35B <= (void*)3?1:*((int*)_tmp35B)!= 15)goto _LL1F9;_tmp35C=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp35B)->f1;_LL1F8: rgn=_tmp35C;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1F6;_LL1F9:;_LL1FA:({struct Cyc_String_pa_struct _tmp35F;
_tmp35F.tag=0;_tmp35F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp35D[1]={& _tmp35F};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp35E="expecting region_t type but found %s";_tag_arr(_tmp35E,sizeof(char),
_get_zero_arr_size(_tmp35E,37));}),_tag_arr(_tmp35D,sizeof(void*),1));}});goto
_LL1F6;_LL1F6:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*((struct Cyc_Absyn_Exp**)
e));{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*((int*)is_calloc)){
if(*((void***)t)== 0)({void*_tmp363[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp364="calloc with empty type";
_tag_arr(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,23));}),_tag_arr(
_tmp363,sizeof(void*),0));});elt_type=*((void**)_check_null(*((void***)t)));Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(
1,loc,topt,elt_type);num_elts=*((struct Cyc_Absyn_Exp**)e);one_elt=0;}else{void*
_tmp365=(void*)(*((struct Cyc_Absyn_Exp**)e))->r;void*_tmp366;void*_tmp367;struct
Cyc_List_List*_tmp368;struct Cyc_List_List _tmp369;struct Cyc_Absyn_Exp*_tmp36A;
struct Cyc_List_List*_tmp36B;struct Cyc_List_List _tmp36C;struct Cyc_Absyn_Exp*
_tmp36D;struct Cyc_List_List*_tmp36E;_LL1FC: if(*((int*)_tmp365)!= 16)goto _LL1FE;
_tmp366=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp365)->f1;_LL1FD:
elt_type=_tmp366;*((void***)t)=(void**)({void**_tmp36F=_cycalloc(sizeof(*_tmp36F));
_tmp36F[0]=elt_type;_tmp36F;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL1FB;_LL1FE: if(*((int*)
_tmp365)!= 3)goto _LL200;_tmp367=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp365)->f1;if((int)_tmp367 != 1)goto _LL200;_tmp368=((struct Cyc_Absyn_Primop_e_struct*)
_tmp365)->f2;if(_tmp368 == 0)goto _LL200;_tmp369=*_tmp368;_tmp36A=(struct Cyc_Absyn_Exp*)
_tmp369.hd;_tmp36B=_tmp369.tl;if(_tmp36B == 0)goto _LL200;_tmp36C=*_tmp36B;_tmp36D=(
struct Cyc_Absyn_Exp*)_tmp36C.hd;_tmp36E=_tmp36C.tl;if(_tmp36E != 0)goto _LL200;
_LL1FF:{struct _tuple0 _tmp371=({struct _tuple0 _tmp370;_tmp370.f1=(void*)_tmp36A->r;
_tmp370.f2=(void*)_tmp36D->r;_tmp370;});void*_tmp372;void*_tmp373;void*_tmp374;
void*_tmp375;_LL203: _tmp372=_tmp371.f1;if(*((int*)_tmp372)!= 16)goto _LL205;
_tmp373=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp372)->f1;_LL204: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp373);elt_type=_tmp373;*((void***)t)=(void**)({void**_tmp376=
_cycalloc(sizeof(*_tmp376));_tmp376[0]=elt_type;_tmp376;});num_elts=_tmp36D;
one_elt=0;goto _LL202;_LL205: _tmp374=_tmp371.f2;if(*((int*)_tmp374)!= 16)goto
_LL207;_tmp375=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp374)->f1;_LL206:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp375);elt_type=_tmp375;*((void***)t)=(
void**)({void**_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377[0]=elt_type;_tmp377;});
num_elts=_tmp36A;one_elt=0;goto _LL202;_LL207:;_LL208: goto No_sizeof;_LL202:;}goto
_LL1FB;_LL200:;_LL201: No_sizeof: elt_type=Cyc_Absyn_char_typ;*((void***)t)=(void**)({
void**_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=elt_type;_tmp378;});
num_elts=*((struct Cyc_Absyn_Exp**)e);one_elt=0;goto _LL1FB;_LL1FB:;}*((struct Cyc_Absyn_Exp**)
e)=num_elts;*((int*)is_fat)=!one_elt;{void*_tmp379=elt_type;struct Cyc_Absyn_AggrInfo
_tmp37A;void*_tmp37B;struct Cyc_Absyn_Aggrdecl**_tmp37C;struct Cyc_Absyn_Aggrdecl*
_tmp37D;_LL20A: if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 10)goto _LL20C;_tmp37A=((
struct Cyc_Absyn_AggrType_struct*)_tmp379)->f1;_tmp37B=(void*)_tmp37A.aggr_info;
if(*((int*)_tmp37B)!= 1)goto _LL20C;_tmp37C=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp37B)->f1;_tmp37D=*_tmp37C;_LL20B: if(_tmp37D->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp37D->impl))->exist_vars != 0: 0)({void*_tmp37E[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp37F="malloc with existential types not yet implemented";
_tag_arr(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,50));}),_tag_arr(
_tmp37E,sizeof(void*),0));});goto _LL209;_LL20C:;_LL20D: goto _LL209;_LL209:;}{void*(*
_tmp380)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp381=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp382=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo
_tmp383;struct Cyc_Absyn_Conref*_tmp384;struct Cyc_Absyn_Conref*_tmp385;_LL20F: if(
_tmp382 <= (void*)3?1:*((int*)_tmp382)!= 4)goto _LL211;_tmp383=((struct Cyc_Absyn_PointerType_struct*)
_tmp382)->f1;_tmp384=_tmp383.nullable;_tmp385=_tmp383.zero_term;_LL210: _tmp381=
_tmp385;if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp384))
_tmp380=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp385)?!(*((int*)is_calloc)): 0){({void*_tmp386[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp387="converting malloc to calloc to ensure zero-termination";
_tag_arr(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,55));}),_tag_arr(
_tmp386,sizeof(void*),0));});*((int*)is_calloc)=1;}goto _LL20E;_LL211:;_LL212:
goto _LL20E;_LL20E:;}if(!one_elt)_tmp380=Cyc_Absyn_tagged_typ;return _tmp380(
elt_type,rgn,Cyc_Absyn_empty_tqual(),_tmp381);}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(te,s),s,1);Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp388=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp389;struct Cyc_Absyn_Stmt*_tmp38A;struct Cyc_Absyn_Stmt*_tmp38B;struct Cyc_Absyn_Decl*
_tmp38C;struct Cyc_Absyn_Stmt*_tmp38D;_LL214: if(_tmp388 <= (void*)1?1:*((int*)
_tmp388)!= 0)goto _LL216;_tmp389=((struct Cyc_Absyn_Exp_s_struct*)_tmp388)->f1;
_LL215: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp389->topt))->v;_LL216:
if(_tmp388 <= (void*)1?1:*((int*)_tmp388)!= 1)goto _LL218;_tmp38A=((struct Cyc_Absyn_Seq_s_struct*)
_tmp388)->f1;_tmp38B=((struct Cyc_Absyn_Seq_s_struct*)_tmp388)->f2;_LL217: s=
_tmp38B;continue;_LL218: if(_tmp388 <= (void*)1?1:*((int*)_tmp388)!= 12)goto _LL21A;
_tmp38C=((struct Cyc_Absyn_Decl_s_struct*)_tmp388)->f1;_tmp38D=((struct Cyc_Absyn_Decl_s_struct*)
_tmp388)->f2;_LL219: s=_tmp38D;continue;_LL21A:;_LL21B: return({void*_tmp38E[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp38F="statement expression must end with expression";
_tag_arr(_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,46));}),_tag_arr(
_tmp38E,sizeof(void*),0));});_LL213:;}}static void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*fd){return({
void*_tmp390[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp391="tcCodegen";
_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,10));}),_tag_arr(
_tmp390,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*
_tmp392[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp393="tcFill";_tag_arr(
_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,7));}),_tag_arr(_tmp392,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp39A;_tmp39A.tag=15;_tmp39A.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp39B;}));_tmp39A;});_tmp399;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp394=Cyc_Tcutil_compress(handle_type);void*
_tmp395;_LL21D: if(_tmp394 <= (void*)3?1:*((int*)_tmp394)!= 15)goto _LL21F;_tmp395=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp394)->f1;_LL21E: rgn=_tmp395;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL21C;_LL21F:;_LL220:({struct Cyc_String_pa_struct
_tmp398;_tmp398.tag=0;_tmp398.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp396[1]={& _tmp398};Cyc_Tcutil_terr(rgn_handle->loc,({
const char*_tmp397="expecting region_t type but found %s";_tag_arr(_tmp397,
sizeof(char),_get_zero_arr_size(_tmp397,37));}),_tag_arr(_tmp396,sizeof(void*),1));}});
goto _LL21C;_LL21C:;}{void*_tmp39C=(void*)e1->r;struct Cyc_Core_Opt*_tmp39D;struct
Cyc_List_List*_tmp39E;struct Cyc_List_List*_tmp39F;void*_tmp3A0;struct _tagged_arr
_tmp3A1;_LL222: if(*((int*)_tmp39C)!= 27)goto _LL224;_LL223: {void*_tmp3A2=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp3A3=Cyc_Tcutil_compress(_tmp3A2);struct Cyc_Absyn_ArrayInfo
_tmp3A4;void*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A6;struct Cyc_Absyn_Exp*_tmp3A7;
struct Cyc_Absyn_Conref*_tmp3A8;_LL22D: if(_tmp3A3 <= (void*)3?1:*((int*)_tmp3A3)!= 
7)goto _LL22F;_tmp3A4=((struct Cyc_Absyn_ArrayType_struct*)_tmp3A3)->f1;_tmp3A5=(
void*)_tmp3A4.elt_type;_tmp3A6=_tmp3A4.tq;_tmp3A7=_tmp3A4.num_elts;_tmp3A8=
_tmp3A4.zero_term;_LL22E: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp3A7);void*b=Cyc_Tcutil_is_const_exp(te,bnd)?(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3AE;_tmp3AE.tag=0;_tmp3AE.f1=bnd;_tmp3AE;});_tmp3AD;}):(void*)0;void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));
_tmp3AA[0]=({struct Cyc_Absyn_PointerType_struct _tmp3AB;_tmp3AB.tag=4;_tmp3AB.f1=({
struct Cyc_Absyn_PtrInfo _tmp3AC;_tmp3AC.elt_typ=(void*)_tmp3A5;_tmp3AC.rgn_typ=(
void*)rgn;_tmp3AC.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3AC.tq=_tmp3A6;_tmp3AC.bounds=Cyc_Absyn_new_conref(b);_tmp3AC.zero_term=
_tmp3A8;_tmp3AC;});_tmp3AB;});_tmp3AA;});if(topt != 0){if(!Cyc_Tcutil_unify(*((
void**)topt),res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*((void**)topt)):
0){e->topt=({struct Cyc_Core_Opt*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->v=(
void*)res_typ;_tmp3A9;});Cyc_Tcutil_unchecked_cast(te,e,*((void**)topt));res_typ=*((
void**)topt);}}return res_typ;}_LL22F:;_LL230:({void*_tmp3AF[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3B0="tcNew: comprehension returned non-array type";
_tag_arr(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,45));}),_tag_arr(
_tmp3AF,sizeof(void*),0));});_LL22C:;}_LL224: if(*((int*)_tmp39C)!= 34)goto _LL226;
_tmp39D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp39C)->f1;_tmp39E=((struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp39C)->f2;_LL225:(void*)(e1->r=(void*)((
void*)({struct Cyc_Absyn_Array_e_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));
_tmp3B1[0]=({struct Cyc_Absyn_Array_e_struct _tmp3B2;_tmp3B2.tag=26;_tmp3B2.f1=
_tmp39E;_tmp3B2;});_tmp3B1;})));_tmp39F=_tmp39E;goto _LL227;_LL226: if(*((int*)
_tmp39C)!= 26)goto _LL228;_tmp39F=((struct Cyc_Absyn_Array_e_struct*)_tmp39C)->f1;
_LL227: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp3B3=Cyc_Tcutil_compress(*((
void**)topt));struct Cyc_Absyn_PtrInfo _tmp3B4;void*_tmp3B5;void**_tmp3B6;struct
Cyc_Absyn_Tqual _tmp3B7;struct Cyc_Absyn_Conref*_tmp3B8;_LL232: if(_tmp3B3 <= (void*)
3?1:*((int*)_tmp3B3)!= 4)goto _LL234;_tmp3B4=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B3)->f1;_tmp3B5=(void*)_tmp3B4.elt_typ;_tmp3B6=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3B3)->f1).elt_typ;_tmp3B7=_tmp3B4.tq;_tmp3B8=_tmp3B4.zero_term;_LL233:
elt_typ_opt=(void**)_tmp3B6;zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3B8);goto _LL231;_LL234:;_LL235: goto _LL231;_LL231:;}{void*_tmp3B9=Cyc_Tcexp_tcArray(
te,e1->loc,elt_typ_opt,zero_term,_tmp39F);e1->topt=({struct Cyc_Core_Opt*_tmp3BA=
_cycalloc(sizeof(*_tmp3BA));_tmp3BA->v=(void*)_tmp3B9;_tmp3BA;});{void*res_typ;{
void*_tmp3BB=Cyc_Tcutil_compress(_tmp3B9);struct Cyc_Absyn_ArrayInfo _tmp3BC;void*
_tmp3BD;struct Cyc_Absyn_Tqual _tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Conref*
_tmp3C0;_LL237: if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 7)goto _LL239;_tmp3BC=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3BB)->f1;_tmp3BD=(void*)_tmp3BC.elt_type;
_tmp3BE=_tmp3BC.tq;_tmp3BF=_tmp3BC.num_elts;_tmp3C0=_tmp3BC.zero_term;_LL238:
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3C1=_cycalloc(sizeof(*
_tmp3C1));_tmp3C1[0]=({struct Cyc_Absyn_PointerType_struct _tmp3C2;_tmp3C2.tag=4;
_tmp3C2.f1=({struct Cyc_Absyn_PtrInfo _tmp3C3;_tmp3C3.elt_typ=(void*)_tmp3BD;
_tmp3C3.rgn_typ=(void*)rgn;_tmp3C3.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3C3.tq=_tmp3BE;_tmp3C3.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3C5;_tmp3C5.tag=0;_tmp3C5.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3BF);
_tmp3C5;});_tmp3C4;}));_tmp3C3.zero_term=_tmp3C0;_tmp3C3;});_tmp3C2;});_tmp3C1;});
if(topt != 0){if(!Cyc_Tcutil_unify(*((void**)topt),res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*((void**)topt)): 0){e->topt=({struct Cyc_Core_Opt*_tmp3C6=
_cycalloc(sizeof(*_tmp3C6));_tmp3C6->v=(void*)res_typ;_tmp3C6;});Cyc_Tcutil_unchecked_cast(
te,e,*((void**)topt));res_typ=*((void**)topt);}}goto _LL236;_LL239:;_LL23A:({void*
_tmp3C7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp3C8="tcExpNoPromote on Array_e returned non-array type";_tag_arr(
_tmp3C8,sizeof(char),_get_zero_arr_size(_tmp3C8,50));}),_tag_arr(_tmp3C7,sizeof(
void*),0));});_LL236:;}return res_typ;}}}_LL228: if(*((int*)_tmp39C)!= 0)goto
_LL22A;_tmp3A0=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp39C)->f1;if(_tmp3A0
<= (void*)1?1:*((int*)_tmp3A0)!= 5)goto _LL22A;_tmp3A1=((struct Cyc_Absyn_String_c_struct*)
_tmp3A0)->f1;_LL229: {void*_tmp3C9=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(
void*)0,Cyc_Absyn_true_conref);void*_tmp3CA=Cyc_Tcexp_tcExp(te,(void**)& _tmp3C9,
e1);return Cyc_Absyn_atb_typ(_tmp3CA,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp3CC;_tmp3CC.tag=0;_tmp3CC.f1=Cyc_Absyn_uint_exp(1,0);
_tmp3CC;});_tmp3CB;}),Cyc_Absyn_false_conref);}_LL22A:;_LL22B: {void**topt2=0;
if(topt != 0){void*_tmp3CD=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo
_tmp3CE;void*_tmp3CF;void**_tmp3D0;struct Cyc_Absyn_Conref*_tmp3D1;_LL23C: if(
_tmp3CD <= (void*)3?1:*((int*)_tmp3CD)!= 4)goto _LL23E;_tmp3CE=((struct Cyc_Absyn_PointerType_struct*)
_tmp3CD)->f1;_tmp3CF=(void*)_tmp3CE.elt_typ;_tmp3D0=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3CD)->f1).elt_typ;_tmp3D1=_tmp3CE.nullable;_LL23D: topt2=(void**)_tmp3D0;goto
_LL23B;_LL23E: if(_tmp3CD <= (void*)3?1:*((int*)_tmp3CD)!= 2)goto _LL240;_LL23F:
topt2=({void**_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=*((void**)topt);
_tmp3D2;});goto _LL23B;_LL240:;_LL241: goto _LL23B;_LL23B:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3D4=
_cycalloc(sizeof(*_tmp3D4));_tmp3D4[0]=({struct Cyc_Absyn_PointerType_struct
_tmp3D5;_tmp3D5.tag=4;_tmp3D5.f1=({struct Cyc_Absyn_PtrInfo _tmp3D6;_tmp3D6.elt_typ=(
void*)telt;_tmp3D6.rgn_typ=(void*)rgn;_tmp3D6.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp3D6.tq=Cyc_Absyn_empty_tqual();_tmp3D6.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));
_tmp3D7[0]=({struct Cyc_Absyn_Upper_b_struct _tmp3D8;_tmp3D8.tag=0;_tmp3D8.f1=Cyc_Absyn_uint_exp(
1,0);_tmp3D8;});_tmp3D7;}));_tmp3D6.zero_term=Cyc_Absyn_false_conref;_tmp3D6;});
_tmp3D5;});_tmp3D4;});if(topt != 0){if(!Cyc_Tcutil_unify(*((void**)topt),res_typ)?
Cyc_Tcutil_silent_castable(te,loc,res_typ,*((void**)topt)): 0){e->topt=({struct
Cyc_Core_Opt*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->v=(void*)res_typ;
_tmp3D3;});Cyc_Tcutil_unchecked_cast(te,e,*((void**)topt));res_typ=*((void**)
topt);}}return res_typ;}}_LL221:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp3D9=t;struct Cyc_Absyn_ArrayInfo _tmp3DA;void*_tmp3DB;struct
Cyc_Absyn_Tqual _tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Conref*
_tmp3DE;_LL243: if(_tmp3D9 <= (void*)3?1:*((int*)_tmp3D9)!= 7)goto _LL245;_tmp3DA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3D9)->f1;_tmp3DB=(void*)_tmp3DA.elt_type;
_tmp3DC=_tmp3DA.tq;_tmp3DD=_tmp3DA.num_elts;_tmp3DE=_tmp3DA.zero_term;_LL244: {
void*_tmp3E0;struct _tuple6 _tmp3DF=Cyc_Tcutil_addressof_props(te,e);_tmp3E0=
_tmp3DF.f2;{void*_tmp3E1=_tmp3DD == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3E3;_tmp3E3.tag=0;_tmp3E3.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3DD);
_tmp3E3;});_tmp3E2;});t=Cyc_Absyn_atb_typ(_tmp3DB,_tmp3E0,_tmp3DC,_tmp3E1,
_tmp3DE);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return
t;}}_LL245:;_LL246: return t;_LL242:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
void*_tmp3E4=(void*)e->r;void*_tmp3E5;_LL248: if(*((int*)_tmp3E4)!= 26)goto _LL24A;
_LL249: goto _LL24B;_LL24A: if(*((int*)_tmp3E4)!= 27)goto _LL24C;_LL24B: goto _LL24D;
_LL24C: if(*((int*)_tmp3E4)!= 0)goto _LL24E;_tmp3E5=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp3E4)->f1;if(_tmp3E5 <= (void*)1?1:*((int*)_tmp3E5)!= 5)goto _LL24E;_LL24D:
return t;_LL24E:;_LL24F: t=Cyc_Tcutil_compress(t);{void*_tmp3E6=t;struct Cyc_Absyn_ArrayInfo
_tmp3E7;void*_tmp3E8;struct Cyc_Absyn_Tqual _tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;
struct Cyc_Absyn_Conref*_tmp3EB;_LL251: if(_tmp3E6 <= (void*)3?1:*((int*)_tmp3E6)!= 
7)goto _LL253;_tmp3E7=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E6)->f1;_tmp3E8=(
void*)_tmp3E7.elt_type;_tmp3E9=_tmp3E7.tq;_tmp3EA=_tmp3E7.num_elts;_tmp3EB=
_tmp3E7.zero_term;_LL252: {void*_tmp3ED;struct _tuple6 _tmp3EC=Cyc_Tcutil_addressof_props(
te,e);_tmp3ED=_tmp3EC.f2;{void*b=_tmp3EA == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3EF;_tmp3EF.tag=0;_tmp3EF.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3EA);
_tmp3EF;});_tmp3EE;});t=Cyc_Absyn_atb_typ(_tmp3E8,_tmp3ED,_tmp3E9,b,_tmp3EB);Cyc_Tcutil_unchecked_cast(
te,e,t);return t;}}_LL253:;_LL254: return t;_LL250:;}_LL247:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp3F0=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp3F1;_LL256: if(*((int*)_tmp3F0)!= 11)goto _LL258;_tmp3F1=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp3F0)->f1;_LL257: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp3F1);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp3F1->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F1->topt))->v));
e->topt=_tmp3F1->topt;goto _LL255;_LL258:;_LL259: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp3F2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3F3;void*_tmp3F4;void*_tmp3F5;struct Cyc_Absyn_Conref*_tmp3F6;struct Cyc_Absyn_Tqual
_tmp3F7;struct Cyc_Absyn_Conref*_tmp3F8;struct Cyc_Absyn_Conref*_tmp3F9;_LL25B: if(
_tmp3F2 <= (void*)3?1:*((int*)_tmp3F2)!= 4)goto _LL25D;_tmp3F3=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F2)->f1;_tmp3F4=(void*)_tmp3F3.elt_typ;_tmp3F5=(void*)_tmp3F3.rgn_typ;
_tmp3F6=_tmp3F3.nullable;_tmp3F7=_tmp3F3.tq;_tmp3F8=_tmp3F3.bounds;_tmp3F9=
_tmp3F3.zero_term;_LL25C:{void*_tmp3FA=Cyc_Tcutil_compress(_tmp3F4);struct Cyc_Absyn_FnInfo
_tmp3FB;struct Cyc_List_List*_tmp3FC;struct Cyc_Core_Opt*_tmp3FD;void*_tmp3FE;
struct Cyc_List_List*_tmp3FF;int _tmp400;struct Cyc_Absyn_VarargInfo*_tmp401;struct
Cyc_List_List*_tmp402;struct Cyc_List_List*_tmp403;_LL260: if(_tmp3FA <= (void*)3?1:*((
int*)_tmp3FA)!= 8)goto _LL262;_tmp3FB=((struct Cyc_Absyn_FnType_struct*)_tmp3FA)->f1;
_tmp3FC=_tmp3FB.tvars;_tmp3FD=_tmp3FB.effect;_tmp3FE=(void*)_tmp3FB.ret_typ;
_tmp3FF=_tmp3FB.args;_tmp400=_tmp3FB.c_varargs;_tmp401=_tmp3FB.cyc_varargs;
_tmp402=_tmp3FB.rgn_po;_tmp403=_tmp3FB.attributes;_LL261: if(_tmp3FC != 0){struct
_RegionHandle _tmp404=_new_region("rgn");struct _RegionHandle*rgn=& _tmp404;
_push_region(rgn);{struct _tuple4 _tmp405=({struct _tuple4 _tmp40F;_tmp40F.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp40F.f2=rgn;_tmp40F;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp405,_tmp3FC);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Absyn_FnType_struct
_tmp40D;_tmp40D.tag=8;_tmp40D.f1=({struct Cyc_Absyn_FnInfo _tmp40E;_tmp40E.tvars=0;
_tmp40E.effect=_tmp3FD;_tmp40E.ret_typ=(void*)_tmp3FE;_tmp40E.args=_tmp3FF;
_tmp40E.c_varargs=_tmp400;_tmp40E.cyc_varargs=_tmp401;_tmp40E.rgn_po=_tmp402;
_tmp40E.attributes=_tmp403;_tmp40E;});_tmp40D;});_tmp40C;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409[0]=({
struct Cyc_Absyn_PointerType_struct _tmp40A;_tmp40A.tag=4;_tmp40A.f1=({struct Cyc_Absyn_PtrInfo
_tmp40B;_tmp40B.elt_typ=(void*)ftyp;_tmp40B.rgn_typ=(void*)_tmp3F5;_tmp40B.nullable=
_tmp3F6;_tmp40B.tq=_tmp3F7;_tmp40B.bounds=_tmp3F8;_tmp40B.zero_term=_tmp3F9;
_tmp40B;});_tmp40A;});_tmp409;});struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp406=
_cycalloc(sizeof(*_tmp406));_tmp406[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp407;_tmp407.tag=12;_tmp407.f1=inner;_tmp407.f2=ts;_tmp407;});_tmp406;})));e->topt=({
struct Cyc_Core_Opt*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->v=(void*)new_typ;
_tmp408;});};_pop_region(rgn);}goto _LL25F;_LL262:;_LL263: goto _LL25F;_LL25F:;}
goto _LL25A;_LL25D:;_LL25E: goto _LL25A;_LL25A:;}goto _LL255;_LL255:;}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*
loc=e->loc;void*t;{void*_tmp410=(void*)e->r;struct Cyc_Absyn_Exp*_tmp411;struct
_tuple1*_tmp412;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_List_List*_tmp414;struct
Cyc_Core_Opt*_tmp415;struct Cyc_List_List*_tmp416;void*_tmp417;struct _tuple1*
_tmp418;void*_tmp419;void*_tmp41A;struct Cyc_List_List*_tmp41B;struct Cyc_Absyn_Exp*
_tmp41C;void*_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Core_Opt*_tmp41F;
struct Cyc_Absyn_Exp*_tmp420;struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Exp*
_tmp422;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_Absyn_Exp*
_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_List_List*_tmp427;struct Cyc_Absyn_VarargCallInfo*
_tmp428;struct Cyc_Absyn_VarargCallInfo**_tmp429;struct Cyc_Absyn_Exp*_tmp42A;
struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_List_List*_tmp42C;void*_tmp42D;struct Cyc_Absyn_Exp*
_tmp42E;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*
_tmp431;struct Cyc_Absyn_Exp*_tmp432;void*_tmp433;void*_tmp434;void*_tmp435;
struct Cyc_Absyn_Exp*_tmp436;struct Cyc_Absyn_Exp*_tmp437;struct _tagged_arr*
_tmp438;struct Cyc_Absyn_Exp*_tmp439;struct _tagged_arr*_tmp43A;struct Cyc_Absyn_Exp*
_tmp43B;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_List_List*_tmp43D;struct _tuple2*
_tmp43E;struct Cyc_List_List*_tmp43F;struct Cyc_List_List*_tmp440;struct Cyc_Absyn_Stmt*
_tmp441;struct Cyc_Absyn_Fndecl*_tmp442;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Vardecl*
_tmp444;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Exp*_tmp446;int _tmp447;int*
_tmp448;struct _tuple1*_tmp449;struct _tuple1**_tmp44A;struct Cyc_List_List*_tmp44B;
struct Cyc_List_List**_tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_Absyn_Aggrdecl*
_tmp44E;struct Cyc_Absyn_Aggrdecl**_tmp44F;void*_tmp450;struct Cyc_List_List*
_tmp451;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Tuniondecl*_tmp453;struct Cyc_Absyn_Tunionfield*
_tmp454;struct _tuple1*_tmp455;struct _tuple1**_tmp456;struct Cyc_Absyn_Enumdecl*
_tmp457;struct Cyc_Absyn_Enumfield*_tmp458;struct _tuple1*_tmp459;struct _tuple1**
_tmp45A;void*_tmp45B;struct Cyc_Absyn_Enumfield*_tmp45C;struct Cyc_Absyn_MallocInfo
_tmp45D;int _tmp45E;int*_tmp45F;struct Cyc_Absyn_Exp*_tmp460;void**_tmp461;void***
_tmp462;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Exp**_tmp464;int _tmp465;int*
_tmp466;_LL265: if(*((int*)_tmp410)!= 11)goto _LL267;_tmp411=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp410)->f1;_LL266: Cyc_Tcexp_tcExpNoInst(te,0,_tmp411);return;_LL267: if(*((int*)
_tmp410)!= 2)goto _LL269;_tmp412=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp410)->f1;
_LL268: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp412);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL269: if(*((int*)_tmp410)!= 8)goto _LL26B;_tmp413=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp410)->f1;_tmp414=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp410)->f2;_LL26A:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp413,_tmp414);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL26B: if(*((int*)_tmp410)!= 34)goto _LL26D;_tmp415=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp410)->f1;_tmp416=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp410)->f2;
_LL26C: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp416);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL26D: if(*((int*)_tmp410)!= 0)goto _LL26F;_tmp417=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp410)->f1;_LL26E: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp417,e);goto _LL264;_LL26F: if(*((int*)_tmp410)!= 1)goto _LL271;_tmp418=((
struct Cyc_Absyn_Var_e_struct*)_tmp410)->f1;_tmp419=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp410)->f2;_LL270: t=Cyc_Tcexp_tcVar(te,loc,_tmp418,_tmp419);goto _LL264;_LL271:
if(*((int*)_tmp410)!= 3)goto _LL273;_tmp41A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp410)->f1;_tmp41B=((struct Cyc_Absyn_Primop_e_struct*)_tmp410)->f2;_LL272: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp41A,_tmp41B);goto _LL264;_LL273: if(*((int*)
_tmp410)!= 5)goto _LL275;_tmp41C=((struct Cyc_Absyn_Increment_e_struct*)_tmp410)->f1;
_tmp41D=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp410)->f2;_LL274: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp41C,_tmp41D);goto _LL264;_LL275: if(*((int*)_tmp410)!= 4)goto
_LL277;_tmp41E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp410)->f1;_tmp41F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp410)->f2;_tmp420=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp410)->f3;_LL276: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp41E,_tmp41F,_tmp420);
goto _LL264;_LL277: if(*((int*)_tmp410)!= 6)goto _LL279;_tmp421=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp410)->f1;_tmp422=((struct Cyc_Absyn_Conditional_e_struct*)_tmp410)->f2;
_tmp423=((struct Cyc_Absyn_Conditional_e_struct*)_tmp410)->f3;_LL278: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp421,_tmp422,_tmp423);goto _LL264;_LL279: if(*((int*)_tmp410)!= 7)
goto _LL27B;_tmp424=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp410)->f1;_tmp425=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp410)->f2;_LL27A: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp424,_tmp425);goto _LL264;_LL27B: if(*((int*)_tmp410)!= 9)goto _LL27D;
_tmp426=((struct Cyc_Absyn_FnCall_e_struct*)_tmp410)->f1;_tmp427=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp410)->f2;_tmp428=((struct Cyc_Absyn_FnCall_e_struct*)_tmp410)->f3;_tmp429=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp410)->f3;
_LL27C: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp426,_tmp427,_tmp429);goto _LL264;
_LL27D: if(*((int*)_tmp410)!= 10)goto _LL27F;_tmp42A=((struct Cyc_Absyn_Throw_e_struct*)
_tmp410)->f1;_LL27E: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp42A);goto _LL264;_LL27F:
if(*((int*)_tmp410)!= 12)goto _LL281;_tmp42B=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp410)->f1;_tmp42C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp410)->f2;_LL280:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp42B,_tmp42C);goto _LL264;_LL281: if(*((
int*)_tmp410)!= 13)goto _LL283;_tmp42D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp410)->f1;_tmp42E=((struct Cyc_Absyn_Cast_e_struct*)_tmp410)->f2;_LL282: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp42D,_tmp42E);goto _LL264;_LL283: if(*((int*)_tmp410)!= 14)goto
_LL285;_tmp42F=((struct Cyc_Absyn_Address_e_struct*)_tmp410)->f1;_LL284: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp42F);goto _LL264;_LL285: if(*((int*)_tmp410)!= 15)goto _LL287;
_tmp430=((struct Cyc_Absyn_New_e_struct*)_tmp410)->f1;_tmp431=((struct Cyc_Absyn_New_e_struct*)
_tmp410)->f2;_LL286: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp430,e,_tmp431);goto _LL264;
_LL287: if(*((int*)_tmp410)!= 17)goto _LL289;_tmp432=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp410)->f1;_LL288: {void*_tmp467=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp432);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp467);goto _LL264;}_LL289: if(*((int*)_tmp410)!= 16)goto _LL28B;
_tmp433=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp410)->f1;_LL28A: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp433);goto _LL264;_LL28B: if(*((int*)_tmp410)!= 18)goto _LL28D;
_tmp434=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp410)->f1;_tmp435=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp410)->f2;_LL28C: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp434,_tmp435);goto _LL264;_LL28D: if(*((int*)_tmp410)!= 19)goto _LL28F;
_LL28E:({void*_tmp468[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp469="gen() not in top-level initializer";
_tag_arr(_tmp469,sizeof(char),_get_zero_arr_size(_tmp469,35));}),_tag_arr(
_tmp468,sizeof(void*),0));});return;_LL28F: if(*((int*)_tmp410)!= 20)goto _LL291;
_tmp436=((struct Cyc_Absyn_Deref_e_struct*)_tmp410)->f1;_LL290: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp436);goto _LL264;_LL291: if(*((int*)_tmp410)!= 21)goto _LL293;
_tmp437=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp410)->f1;_tmp438=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp410)->f2;_LL292: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp437,_tmp438);goto
_LL264;_LL293: if(*((int*)_tmp410)!= 22)goto _LL295;_tmp439=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp410)->f1;_tmp43A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp410)->f2;_LL294: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp439,_tmp43A);goto _LL264;_LL295: if(*((int*)
_tmp410)!= 23)goto _LL297;_tmp43B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp410)->f1;
_tmp43C=((struct Cyc_Absyn_Subscript_e_struct*)_tmp410)->f2;_LL296: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp43B,_tmp43C);goto _LL264;_LL297: if(*((int*)_tmp410)!= 24)goto
_LL299;_tmp43D=((struct Cyc_Absyn_Tuple_e_struct*)_tmp410)->f1;_LL298: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp43D);goto _LL264;_LL299: if(*((int*)_tmp410)!= 25)goto _LL29B;
_tmp43E=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp410)->f1;_tmp43F=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp410)->f2;_LL29A: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp43E,_tmp43F);goto
_LL264;_LL29B: if(*((int*)_tmp410)!= 26)goto _LL29D;_tmp440=((struct Cyc_Absyn_Array_e_struct*)
_tmp410)->f1;_LL29C: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp46A=
Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_ArrayInfo _tmp46B;void*
_tmp46C;void**_tmp46D;struct Cyc_Absyn_Conref*_tmp46E;_LL2B2: if(_tmp46A <= (void*)
3?1:*((int*)_tmp46A)!= 7)goto _LL2B4;_tmp46B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp46A)->f1;_tmp46C=(void*)_tmp46B.elt_type;_tmp46D=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp46A)->f1).elt_type;_tmp46E=_tmp46B.zero_term;_LL2B3: elt_topt=(void**)_tmp46D;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp46E);
goto _LL2B1;_LL2B4:;_LL2B5: goto _LL2B1;_LL2B1:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp440);goto _LL264;}_LL29D: if(*((int*)_tmp410)!= 35)goto
_LL29F;_tmp441=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp410)->f1;_LL29E: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp441);goto _LL264;_LL29F: if(*((int*)_tmp410)!= 36)goto _LL2A1;
_tmp442=((struct Cyc_Absyn_Codegen_e_struct*)_tmp410)->f1;_LL2A0: t=Cyc_Tcexp_tcCodegen(
te,loc,topt,_tmp442);goto _LL264;_LL2A1: if(*((int*)_tmp410)!= 37)goto _LL2A3;
_tmp443=((struct Cyc_Absyn_Fill_e_struct*)_tmp410)->f1;_LL2A2: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp443);goto _LL264;_LL2A3: if(*((int*)_tmp410)!= 27)goto _LL2A5;
_tmp444=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp410)->f1;_tmp445=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp410)->f2;_tmp446=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp410)->f3;_tmp447=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp410)->f4;
_tmp448=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp410)->f4;_LL2A4: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp444,_tmp445,_tmp446,_tmp448);goto
_LL264;_LL2A5: if(*((int*)_tmp410)!= 28)goto _LL2A7;_tmp449=((struct Cyc_Absyn_Struct_e_struct*)
_tmp410)->f1;_tmp44A=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp410)->f1;_tmp44B=((struct Cyc_Absyn_Struct_e_struct*)_tmp410)->f2;_tmp44C=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp410)->f2;_tmp44D=((
struct Cyc_Absyn_Struct_e_struct*)_tmp410)->f3;_tmp44E=((struct Cyc_Absyn_Struct_e_struct*)
_tmp410)->f4;_tmp44F=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp410)->f4;_LL2A6: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp44A,_tmp44C,_tmp44D,
_tmp44F);goto _LL264;_LL2A7: if(*((int*)_tmp410)!= 29)goto _LL2A9;_tmp450=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp410)->f1;_tmp451=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp410)->f2;_LL2A8: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp450,_tmp451);goto _LL264;
_LL2A9: if(*((int*)_tmp410)!= 30)goto _LL2AB;_tmp452=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp410)->f1;_tmp453=((struct Cyc_Absyn_Tunion_e_struct*)_tmp410)->f2;_tmp454=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp410)->f3;_LL2AA: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp452,_tmp453,_tmp454);goto _LL264;_LL2AB: if(*((int*)_tmp410)!= 31)goto
_LL2AD;_tmp455=((struct Cyc_Absyn_Enum_e_struct*)_tmp410)->f1;_tmp456=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp410)->f1;_tmp457=((struct Cyc_Absyn_Enum_e_struct*)
_tmp410)->f2;_tmp458=((struct Cyc_Absyn_Enum_e_struct*)_tmp410)->f3;_LL2AC:*((
struct _tuple1**)_tmp456)=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp458))->name;
t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));
_tmp46F[0]=({struct Cyc_Absyn_EnumType_struct _tmp470;_tmp470.tag=12;_tmp470.f1=((
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp457))->name;_tmp470.f2=_tmp457;_tmp470;});
_tmp46F;});goto _LL264;_LL2AD: if(*((int*)_tmp410)!= 32)goto _LL2AF;_tmp459=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp410)->f1;_tmp45A=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp410)->f1;_tmp45B=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp410)->f2;_tmp45C=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp410)->f3;_LL2AE:*((
struct _tuple1**)_tmp45A)=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp45C))->name;
t=_tmp45B;goto _LL264;_LL2AF: if(*((int*)_tmp410)!= 33)goto _LL264;_tmp45D=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp410)->f1;_tmp45E=_tmp45D.is_calloc;_tmp45F=(int*)&(((
struct Cyc_Absyn_Malloc_e_struct*)_tmp410)->f1).is_calloc;_tmp460=_tmp45D.rgn;
_tmp461=_tmp45D.elt_type;_tmp462=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp410)->f1).elt_type;_tmp463=_tmp45D.num_elts;_tmp464=(struct Cyc_Absyn_Exp**)&(((
struct Cyc_Absyn_Malloc_e_struct*)_tmp410)->f1).num_elts;_tmp465=_tmp45D.fat_result;
_tmp466=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp410)->f1).fat_result;
_LL2B0: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp460,_tmp462,_tmp464,_tmp45F,_tmp466);
goto _LL264;_LL264:;}e->topt=({struct Cyc_Core_Opt*_tmp471=_cycalloc(sizeof(*
_tmp471));_tmp471->v=(void*)t;_tmp471;});}
