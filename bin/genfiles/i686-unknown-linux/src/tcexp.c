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
short*f1;};struct Cyc_Buffer_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,
struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];extern char
Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr
f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple4{struct
Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;
void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
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
_tmp17;_tmp17.tag=0;_tmp17.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(q);{
void*_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
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
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp27->name);{void*_tmp2B[1]={& _tmp2D};Cyc_Tcutil_terr(e->loc,({const char*
_tmp2C="%s is a constant, not a function";_tag_arr(_tmp2C,sizeof(char),
_get_zero_arr_size(_tmp2C,33));}),_tag_arr(_tmp2B,sizeof(void*),1));}});(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp2E=_cycalloc(sizeof(*
_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp2F;_tmp2F.tag=30;_tmp2F.f1=
es;_tmp2F.f2=_tmp26;_tmp2F.f3=_tmp27;_tmp2F;});_tmp2E;})));_npop_handler(0);
return;_LL1A: if(*((int*)_tmp24)!= 1)goto _LL1C;_tmp28=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp24)->f1;_LL1B: {struct Cyc_List_List*_tmp30=((struct Cyc_List_List*(*)(struct
_tuple8*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp31=
_cycalloc(sizeof(*_tmp31));_tmp31[0]=({struct Cyc_Absyn_Struct_e_struct _tmp32;
_tmp32.tag=28;_tmp32.f1=_tmp28->name;_tmp32.f2=0;_tmp32.f3=_tmp30;_tmp32.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp28;_tmp32;});_tmp31;})));_npop_handler(0);return;}
_LL1C: if(*((int*)_tmp24)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp24)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp20);{void*_tmp33[1]={& _tmp35};Cyc_Tcutil_terr(
e->loc,({const char*_tmp34="%s is an enum constructor, not a function";_tag_arr(
_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,42));}),_tag_arr(_tmp33,sizeof(
void*),1));}});_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(
void*)_exn_thrown;void*_tmp37=_tmp22;_LL21: if(_tmp37 != Cyc_Dict_Absent)goto _LL23;
_LL22:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp38=
_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp39;
_tmp39.tag=9;_tmp39.f1=e1;_tmp39.f2=es;_tmp39.f3=0;_tmp39;});_tmp38;})));return;
_LL23:;_LL24:(void)_throw(_tmp37);_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=9;_tmp3B.f1=e1;_tmp3B.f2=
es;_tmp3B.f3=0;_tmp3B;});_tmp3A;})));return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
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
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_String_pa_struct _tmp53;
_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)msg_part;{void*_tmp51[2]={& _tmp53,&
_tmp54};Cyc_Tcutil_terr(e->loc,({const char*_tmp52="test of %s has type %s instead of integral or * type";
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
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp76);{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(_tmp64);{void*_tmp77[2]={& _tmp79,& _tmp7A};Cyc_Tcutil_terr(
loc,({const char*_tmp78="expecting %s but found %s";_tag_arr(_tmp78,sizeof(char),
_get_zero_arr_size(_tmp78,26));}),_tag_arr(_tmp77,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
t=(void*)({struct Cyc_Absyn_TagType_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));
_tmp7B[0]=({struct Cyc_Absyn_TagType_struct _tmp7C;_tmp7C.tag=17;_tmp7C.f1=(void*)((
void*)_tmp76);_tmp7C;});_tmp7B;});goto _LL46;}_LL51:;_LL52: t=_tmp5B == (void*)1?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if(
_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 5)goto _LL44;_tmp5D=((struct Cyc_Absyn_String_c_struct*)
_tmp55)->f1;_LL43: {int len=(int)_get_arr_size(_tmp5D,sizeof(char));struct Cyc_Absyn_Exp*
elen=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp8A=_cycalloc(
sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_Int_c_struct _tmp8B;_tmp8B.tag=2;
_tmp8B.f1=(void*)((void*)1);_tmp8B.f2=len;_tmp8B;});_tmp8A;}),loc);elen->topt=({
struct Cyc_Core_Opt*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->v=(void*)Cyc_Absyn_uint_typ;
_tmp7F;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp81;_tmp81.tag=0;_tmp81.f1=elen;_tmp81;});
_tmp80;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmp82=Cyc_Tcutil_compress(*((
void**)topt));struct Cyc_Absyn_ArrayInfo _tmp83;struct Cyc_Absyn_Tqual _tmp84;_LL5B:
if(_tmp82 <= (void*)3?1:*((int*)_tmp82)!= 7)goto _LL5D;_tmp83=((struct Cyc_Absyn_ArrayType_struct*)
_tmp82)->f1;_tmp84=_tmp83.tq;_LL5C: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmp84,(struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)());
_LL5D: if(_tmp82 <= (void*)3?1:*((int*)_tmp82)!= 4)goto _LL5F;_LL5E: if(!Cyc_Tcutil_unify(*((
void**)topt),t)?Cyc_Tcutil_silent_castable(te,loc,t,*((void**)topt)): 0){e->topt=({
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
0;_tmp9A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(q);{void*_tmp98[1]={&
_tmp9A};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp99="undeclared identifier: %s";
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
_tagged_arr)_tmpA2,fmt->loc);goto _LL6E;_LL73:;_LL74: return;_LL6E:;}if(opt_args != 
0){struct Cyc_List_List*_tmpA3=(struct Cyc_List_List*)opt_args->v;for(0;desc_types
!= 0?_tmpA3 != 0: 0;(desc_types=desc_types->tl,_tmpA3=_tmpA3->tl)){void*t=(void*)
desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpA3->hd;if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmpA7;_tmpA7.tag=0;_tmpA7.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
struct Cyc_String_pa_struct _tmpA6;_tmpA6.tag=0;_tmpA6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmpA4[2]={& _tmpA6,& _tmpA7};Cyc_Tcutil_terr(e->loc,({const char*_tmpA5="descriptor has type \n%s\n but argument has type \n%s";
_tag_arr(_tmpA5,sizeof(char),_get_zero_arr_size(_tmpA5,51));}),_tag_arr(_tmpA4,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(desc_types != 0)({void*
_tmpA8[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpA9="too few arguments";
_tag_arr(_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,18));}),_tag_arr(_tmpA8,
sizeof(void*),0));});if(_tmpA3 != 0)({void*_tmpAA[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA3->hd)->loc,({const char*_tmpAB="too many arguments";_tag_arr(
_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,19));}),_tag_arr(_tmpAA,sizeof(
void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpAC=p;_LL76: if((
int)_tmpAC != 0)goto _LL78;_LL77: goto _LL79;_LL78: if((int)_tmpAC != 2)goto _LL7A;
_LL79: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=
0;_tmpAF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpAD[1]={&
_tmpAF};Cyc_Tcutil_terr(loc,({const char*_tmpAE="expecting arithmetic type but found %s";
_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,39));}),_tag_arr(_tmpAD,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7A: if((int)_tmpAC != 11)goto _LL7C;_LL7B: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB2;_tmpB2.tag=
0;_tmpB2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpB0[1]={&
_tmpB2};Cyc_Tcutil_terr(loc,({const char*_tmpB1="expecting integral or * type but found %s";
_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,42));}),_tag_arr(_tmpB0,
sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7C: if((int)_tmpAC != 12)goto
_LL7E;_LL7D: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct _tmpB5;
_tmpB5.tag=0;_tmpB5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpB3[
1]={& _tmpB5};Cyc_Tcutil_terr(loc,({const char*_tmpB4="expecting integral type but found %s";
_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,37));}),_tag_arr(_tmpB3,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7E: if((int)_tmpAC != 19)goto _LL80;_LL7F:{void*_tmpB6=t;_LL83: if(_tmpB6 <= (void*)
3?1:*((int*)_tmpB6)!= 7)goto _LL85;_LL84: goto _LL86;_LL85: if(_tmpB6 <= (void*)3?1:*((
int*)_tmpB6)!= 4)goto _LL87;_LL86: goto _LL82;_LL87:;_LL88:({struct Cyc_String_pa_struct
_tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpB7[1]={& _tmpB9};Cyc_Tcutil_terr(loc,({const char*_tmpB8="size requires pointer or array type, not %s";
_tag_arr(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,44));}),_tag_arr(_tmpB7,
sizeof(void*),1));}});_LL82:;}return Cyc_Absyn_uint_typ;_LL80:;_LL81:({void*
_tmpBA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpBB="Non-unary primop";_tag_arr(_tmpBB,sizeof(char),
_get_zero_arr_size(_tmpBB,17));}),_tag_arr(_tmpBA,sizeof(void*),0));});_LL75:;}
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{void*_tmpBC[1]={& _tmpBE};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpBD="type %s cannot be used here";
_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,28));}),_tag_arr(_tmpBC,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpBF;}));}
if(!checker(e2)){({struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmpC0[1]={& _tmpC2};Cyc_Tcutil_terr(e2->loc,({
const char*_tmpC1="type %s cannot be used here";_tag_arr(_tmpC1,sizeof(char),
_get_zero_arr_size(_tmpC1,28));}),_tag_arr(_tmpC0,sizeof(void*),1));}});return
Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->v=
Cyc_Tcenv_lookup_type_vars(te);_tmpC3;}));}{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmpC4=t1;struct Cyc_Absyn_PtrInfo _tmpC5;void*
_tmpC6;void*_tmpC7;struct Cyc_Absyn_Conref*_tmpC8;struct Cyc_Absyn_Tqual _tmpC9;
struct Cyc_Absyn_Conref*_tmpCA;struct Cyc_Absyn_Conref*_tmpCB;_LL8A: if(_tmpC4 <= (
void*)3?1:*((int*)_tmpC4)!= 4)goto _LL8C;_tmpC5=((struct Cyc_Absyn_PointerType_struct*)
_tmpC4)->f1;_tmpC6=(void*)_tmpC5.elt_typ;_tmpC7=(void*)_tmpC5.rgn_typ;_tmpC8=
_tmpC5.nullable;_tmpC9=_tmpC5.tq;_tmpCA=_tmpC5.bounds;_tmpCB=_tmpC5.zero_term;
_LL8B: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpC6),(void*)1))({void*
_tmpCC[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpCD="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,50));}),_tag_arr(_tmpCC,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct
_tmpD0;_tmpD0.tag=0;_tmpD0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
void*_tmpCE[1]={& _tmpD0};Cyc_Tcutil_terr(e2->loc,({const char*_tmpCF="expecting int but found %s";
_tag_arr(_tmpCF,sizeof(char),_get_zero_arr_size(_tmpCF,27));}),_tag_arr(_tmpCE,
sizeof(void*),1));}});_tmpCA=Cyc_Absyn_compress_conref(_tmpCA);{void*_tmpD1=(
void*)_tmpCA->v;void*_tmpD2;void*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;_LL8F: if(
_tmpD1 <= (void*)1?1:*((int*)_tmpD1)!= 0)goto _LL91;_tmpD2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpD1)->f1;if((int)_tmpD2 != 0)goto _LL91;_LL90: return t1;_LL91: if(_tmpD1 <= (void*)
1?1:*((int*)_tmpD1)!= 0)goto _LL93;_tmpD3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpD1)->f1;if(_tmpD3 <= (void*)1?1:*((int*)_tmpD3)!= 0)goto _LL93;_tmpD4=((struct
Cyc_Absyn_Upper_b_struct*)_tmpD3)->f1;_LL92: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpCB))({void*_tmpD5[0]={};Cyc_Tcutil_warn(e1->loc,({
const char*_tmpD6="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,70));}),_tag_arr(_tmpD5,
sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*_tmpD7=({struct Cyc_Absyn_PointerType_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_PointerType_struct
_tmpD9;_tmpD9.tag=4;_tmpD9.f1=({struct Cyc_Absyn_PtrInfo _tmpDA;_tmpDA.elt_typ=(
void*)_tmpC6;_tmpDA.rgn_typ=(void*)_tmpC7;_tmpDA.nullable=Cyc_Absyn_true_conref;
_tmpDA.tq=_tmpC9;_tmpDA.bounds=Cyc_Absyn_new_conref((void*)0);_tmpDA.zero_term=
_tmpCB;_tmpDA;});_tmpD9;});_tmpD8;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)
_tmpD7);return(void*)_tmpD7;}_LL93:;_LL94:(void*)(_tmpCA->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(void*)((void*)0);
_tmpDC;});_tmpDB;})));return t1;_LL8E:;}_LL8C:;_LL8D: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL89:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=
0;_tmpE0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct Cyc_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmpDD[2]={& _tmpDF,& _tmpE0};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpDE="pointer arithmetic on values of different types (%s != %s)";
_tag_arr(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,59));}),_tag_arr(_tmpDD,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}
else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpE1[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpE2="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,50));}),_tag_arr(_tmpE1,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_String_pa_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmpE3[2]={& _tmpE5,& _tmpE6};Cyc_Tcutil_terr(e2->loc,({const char*_tmpE4="expecting either %s or int but found %s";
_tag_arr(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,40));}),_tag_arr(_tmpE3,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t1;}}else{return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);}}static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num?e2_is_num: 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2)?
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({
struct Cyc_Core_Opt*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpE7;}))): 0)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,
loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1);return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2);
return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e1)?1: Cyc_Tcutil_zero_to_null(
te,t1,e2))return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmpE9=({struct _tuple0
_tmpE8;_tmpE8.f1=Cyc_Tcutil_compress(t1);_tmpE8.f2=Cyc_Tcutil_compress(t2);
_tmpE8;});void*_tmpEA;struct Cyc_Absyn_PtrInfo _tmpEB;void*_tmpEC;void*_tmpED;
struct Cyc_Absyn_PtrInfo _tmpEE;void*_tmpEF;_LL96: _tmpEA=_tmpE9.f1;if(_tmpEA <= (
void*)3?1:*((int*)_tmpEA)!= 4)goto _LL98;_tmpEB=((struct Cyc_Absyn_PointerType_struct*)
_tmpEA)->f1;_tmpEC=(void*)_tmpEB.elt_typ;_tmpED=_tmpE9.f2;if(_tmpED <= (void*)3?1:*((
int*)_tmpED)!= 4)goto _LL98;_tmpEE=((struct Cyc_Absyn_PointerType_struct*)_tmpED)->f1;
_tmpEF=(void*)_tmpEE.elt_typ;_LL97: if(Cyc_Tcutil_unify(_tmpEC,_tmpEF))return Cyc_Absyn_sint_typ;
goto _LL95;_LL98:;_LL99: goto _LL95;_LL95:;}({struct Cyc_String_pa_struct _tmpF3;
_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_String_pa_struct
_tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmpF0[2]={& _tmpF2,& _tmpF3};Cyc_Tcutil_terr(loc,({const char*_tmpF1="comparison not allowed between %s and %s";
_tag_arr(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,41));}),_tag_arr(_tmpF0,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpF4;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpF5=p;_LL9B: if((int)_tmpF5 != 0)goto _LL9D;_LL9C: return
Cyc_Tcexp_tcPlus(te,e1,e2);_LL9D: if((int)_tmpF5 != 2)goto _LL9F;_LL9E: return Cyc_Tcexp_tcMinus(
te,e1,e2);_LL9F: if((int)_tmpF5 != 1)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if((int)
_tmpF5 != 3)goto _LLA3;_LLA2: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLA3: if((int)_tmpF5 != 4)goto _LLA5;_LLA4: goto _LLA6;_LLA5: if((int)_tmpF5 != 13)
goto _LLA7;_LLA6: goto _LLA8;_LLA7: if((int)_tmpF5 != 14)goto _LLA9;_LLA8: goto _LLAA;
_LLA9: if((int)_tmpF5 != 15)goto _LLAB;_LLAA: goto _LLAC;_LLAB: if((int)_tmpF5 != 16)
goto _LLAD;_LLAC: goto _LLAE;_LLAD: if((int)_tmpF5 != 17)goto _LLAF;_LLAE: goto _LLB0;
_LLAF: if((int)_tmpF5 != 18)goto _LLB1;_LLB0: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_integral);_LLB1: if((int)_tmpF5 != 5)goto _LLB3;_LLB2: goto _LLB4;_LLB3:
if((int)_tmpF5 != 6)goto _LLB5;_LLB4: goto _LLB6;_LLB5: if((int)_tmpF5 != 7)goto _LLB7;
_LLB6: goto _LLB8;_LLB7: if((int)_tmpF5 != 8)goto _LLB9;_LLB8: goto _LLBA;_LLB9: if((int)
_tmpF5 != 9)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if((int)_tmpF5 != 10)goto _LLBD;_LLBC:
return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLBD:;_LLBE:({void*_tmpF6[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF7="bad binary primop";
_tag_arr(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,18));}),_tag_arr(_tmpF6,
sizeof(void*),0));});_LL9A:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_List_List*es){if(p
== (void*)2?((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: return({void*_tmpF8[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmpF9="primitive operator has 0 arguments";
_tag_arr(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,35));}),_tag_arr(_tmpF8,
sizeof(void*),0));});case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLC2: return({void*_tmpFA[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmpFB="primitive operator has > 2 arguments";_tag_arr(_tmpFB,
sizeof(char),_get_zero_arr_size(_tmpFB,37));}),_tag_arr(_tmpFA,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmpFC=(void*)e->r;void*
_tmpFD;struct Cyc_Absyn_Vardecl*_tmpFE;void*_tmpFF;struct Cyc_Absyn_Vardecl*
_tmp100;void*_tmp101;struct Cyc_Absyn_Vardecl*_tmp102;void*_tmp103;struct Cyc_Absyn_Vardecl*
_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*
_tmp107;struct _tagged_arr*_tmp108;struct Cyc_Absyn_Exp*_tmp109;struct _tagged_arr*
_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*
_tmp10D;_LLC5: if(*((int*)_tmpFC)!= 1)goto _LLC7;_tmpFD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpFC)->f2;if(_tmpFD <= (void*)1?1:*((int*)_tmpFD)!= 2)goto _LLC7;_tmpFE=((struct
Cyc_Absyn_Param_b_struct*)_tmpFD)->f1;_LLC6: _tmp100=_tmpFE;goto _LLC8;_LLC7: if(*((
int*)_tmpFC)!= 1)goto _LLC9;_tmpFF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpFC)->f2;
if(_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 3)goto _LLC9;_tmp100=((struct Cyc_Absyn_Local_b_struct*)
_tmpFF)->f1;_LLC8: _tmp102=_tmp100;goto _LLCA;_LLC9: if(*((int*)_tmpFC)!= 1)goto
_LLCB;_tmp101=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpFC)->f2;if(_tmp101 <= (
void*)1?1:*((int*)_tmp101)!= 4)goto _LLCB;_tmp102=((struct Cyc_Absyn_Pat_b_struct*)
_tmp101)->f1;_LLCA: _tmp104=_tmp102;goto _LLCC;_LLCB: if(*((int*)_tmpFC)!= 1)goto
_LLCD;_tmp103=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpFC)->f2;if(_tmp103 <= (
void*)1?1:*((int*)_tmp103)!= 0)goto _LLCD;_tmp104=((struct Cyc_Absyn_Global_b_struct*)
_tmp103)->f1;_LLCC: if(!(_tmp104->tq).q_const)return;goto _LLC4;_LLCD: if(*((int*)
_tmpFC)!= 23)goto _LLCF;_tmp105=((struct Cyc_Absyn_Subscript_e_struct*)_tmpFC)->f1;
_tmp106=((struct Cyc_Absyn_Subscript_e_struct*)_tmpFC)->f2;_LLCE:{void*_tmp10E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp105->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp10F;struct Cyc_Absyn_Tqual _tmp110;struct Cyc_Absyn_ArrayInfo
_tmp111;struct Cyc_Absyn_Tqual _tmp112;struct Cyc_List_List*_tmp113;_LLDC: if(
_tmp10E <= (void*)3?1:*((int*)_tmp10E)!= 4)goto _LLDE;_tmp10F=((struct Cyc_Absyn_PointerType_struct*)
_tmp10E)->f1;_tmp110=_tmp10F.tq;_LLDD: _tmp112=_tmp110;goto _LLDF;_LLDE: if(_tmp10E
<= (void*)3?1:*((int*)_tmp10E)!= 7)goto _LLE0;_tmp111=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10E)->f1;_tmp112=_tmp111.tq;_LLDF: if(!_tmp112.q_const)return;goto _LLDB;_LLE0:
if(_tmp10E <= (void*)3?1:*((int*)_tmp10E)!= 9)goto _LLE2;_tmp113=((struct Cyc_Absyn_TupleType_struct*)
_tmp10E)->f1;_LLE1: {unsigned int _tmp115;int _tmp116;struct _tuple7 _tmp114=Cyc_Evexp_eval_const_uint_exp(
_tmp106);_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;if(!_tmp116){({void*_tmp117[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp118="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,47));}),_tag_arr(
_tmp117,sizeof(void*),0));});return;}{struct _handler_cons _tmp119;_push_handler(&
_tmp119);{int _tmp11B=0;if(setjmp(_tmp119.handler))_tmp11B=1;if(!_tmp11B){{struct
_tuple9 _tmp11D;struct Cyc_Absyn_Tqual _tmp11E;struct _tuple9*_tmp11C=((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp113,(int)_tmp115);
_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;if(!_tmp11E.q_const){_npop_handler(0);
return;}};_pop_handler();}else{void*_tmp11A=(void*)_exn_thrown;void*_tmp120=
_tmp11A;_LLE5: if(_tmp120 != Cyc_List_Nth)goto _LLE7;_LLE6: return;_LLE7:;_LLE8:(
void)_throw(_tmp120);_LLE4:;}}}goto _LLDB;}_LLE2:;_LLE3: goto _LLDB;_LLDB:;}goto
_LLC4;_LLCF: if(*((int*)_tmpFC)!= 21)goto _LLD1;_tmp107=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpFC)->f1;_tmp108=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpFC)->f2;_LLD0:{
void*_tmp121=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp107->topt))->v);struct Cyc_Absyn_AggrInfo _tmp122;void*_tmp123;struct Cyc_Absyn_Aggrdecl**
_tmp124;struct Cyc_List_List*_tmp125;_LLEA: if(_tmp121 <= (void*)3?1:*((int*)
_tmp121)!= 10)goto _LLEC;_tmp122=((struct Cyc_Absyn_AggrType_struct*)_tmp121)->f1;
_tmp123=(void*)_tmp122.aggr_info;if(*((int*)_tmp123)!= 1)goto _LLEC;_tmp124=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp123)->f1;_LLEB: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp124 == 0?0: Cyc_Absyn_lookup_decl_field(*((
struct Cyc_Absyn_Aggrdecl**)_tmp124),_tmp108);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLE9;}_LLEC: if(_tmp121 <= (void*)3?1:*((int*)_tmp121)!= 11)goto _LLEE;
_tmp125=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp121)->f2;_LLED: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp125,_tmp108);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: if(*((int*)_tmpFC)!= 
22)goto _LLD3;_tmp109=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpFC)->f1;_tmp10A=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpFC)->f2;_LLD2:{void*_tmp126=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp109->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp127;void*_tmp128;_LLF1: if(_tmp126 <= (void*)3?1:*((int*)_tmp126)!= 4)goto
_LLF3;_tmp127=((struct Cyc_Absyn_PointerType_struct*)_tmp126)->f1;_tmp128=(void*)
_tmp127.elt_typ;_LLF2:{void*_tmp129=Cyc_Tcutil_compress(_tmp128);struct Cyc_Absyn_AggrInfo
_tmp12A;void*_tmp12B;struct Cyc_Absyn_Aggrdecl**_tmp12C;struct Cyc_List_List*
_tmp12D;_LLF6: if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 10)goto _LLF8;_tmp12A=((
struct Cyc_Absyn_AggrType_struct*)_tmp129)->f1;_tmp12B=(void*)_tmp12A.aggr_info;
if(*((int*)_tmp12B)!= 1)goto _LLF8;_tmp12C=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp12B)->f1;_LLF7: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp12C == 0?0: Cyc_Absyn_lookup_decl_field(*((struct Cyc_Absyn_Aggrdecl**)_tmp12C),
_tmp10A);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLF5;}_LLF8: if(_tmp129 <= (
void*)3?1:*((int*)_tmp129)!= 11)goto _LLFA;_tmp12D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp129)->f2;_LLF9: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12D,
_tmp10A);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;
_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto _LLF0;_LLF0:;}goto _LLC4;_LLD3: if(*((int*)
_tmpFC)!= 20)goto _LLD5;_tmp10B=((struct Cyc_Absyn_Deref_e_struct*)_tmpFC)->f1;
_LLD4:{void*_tmp12E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp10B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp12F;struct Cyc_Absyn_Tqual _tmp130;
struct Cyc_Absyn_ArrayInfo _tmp131;struct Cyc_Absyn_Tqual _tmp132;_LLFD: if(_tmp12E <= (
void*)3?1:*((int*)_tmp12E)!= 4)goto _LLFF;_tmp12F=((struct Cyc_Absyn_PointerType_struct*)
_tmp12E)->f1;_tmp130=_tmp12F.tq;_LLFE: _tmp132=_tmp130;goto _LL100;_LLFF: if(
_tmp12E <= (void*)3?1:*((int*)_tmp12E)!= 7)goto _LL101;_tmp131=((struct Cyc_Absyn_ArrayType_struct*)
_tmp12E)->f1;_tmp132=_tmp131.tq;_LL100: if(!_tmp132.q_const)return;goto _LLFC;
_LL101:;_LL102: goto _LLFC;_LLFC:;}goto _LLC4;_LLD5: if(*((int*)_tmpFC)!= 11)goto
_LLD7;_tmp10C=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpFC)->f1;_LLD6:
_tmp10D=_tmp10C;goto _LLD8;_LLD7: if(*((int*)_tmpFC)!= 12)goto _LLD9;_tmp10D=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmpFC)->f1;_LLD8: Cyc_Tcexp_check_writable(
te,_tmp10D);return;_LLD9:;_LLDA: goto _LLC4;_LLC4:;}({struct Cyc_String_pa_struct
_tmp135;_tmp135.tag=0;_tmp135.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp133[1]={& _tmp135};Cyc_Tcutil_terr(e->loc,({const char*_tmp134="attempt to write a const location: %s";
_tag_arr(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,38));}),_tag_arr(
_tmp133,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp136[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp137="increment/decrement of non-lvalue";_tag_arr(_tmp137,
sizeof(char),_get_zero_arr_size(_tmp137,34));}),_tag_arr(_tmp136,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt)?1:(Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt)?i == (void*)0?1: i == (void*)
1: 0)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp138[
0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp139="can't perform arithmetic on abstract pointer type";
_tag_arr(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,50));}),_tag_arr(
_tmp138,sizeof(void*),0));});}else{({struct Cyc_String_pa_struct _tmp13C;_tmp13C.tag=
0;_tmp13C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp13A[1]={&
_tmp13C};Cyc_Tcutil_terr(e->loc,({const char*_tmp13B="expecting arithmetic or ? type but found %s";
_tag_arr(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,44));}),_tag_arr(
_tmp13A,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp13D="conditional expression";_tag_arr(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp146;}));struct Cyc_List_List _tmp13E=({struct Cyc_List_List _tmp145;_tmp145.hd=
e3;_tmp145.tl=0;_tmp145;});struct Cyc_List_List _tmp13F=({struct Cyc_List_List
_tmp144;_tmp144.hd=e2;_tmp144.tl=(struct Cyc_List_List*)& _tmp13E;_tmp144;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp13F)){({struct Cyc_String_pa_struct
_tmp143;_tmp143.tag=0;_tmp143.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e3->topt))->v);{struct Cyc_String_pa_struct
_tmp142;_tmp142.tag=0;_tmp142.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{void*_tmp140[2]={& _tmp142,&
_tmp143};Cyc_Tcutil_terr(loc,({const char*_tmp141="conditional clause types do not match: %s != %s";
_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,48));}),_tag_arr(
_tmp140,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}return t;}}static
void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp147=Cyc_Tcutil_compress(t1);_LL104: if(_tmp147 <= (void*)3?1:*((int*)
_tmp147)!= 7)goto _LL106;_LL105:({void*_tmp148[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp149="cannot assign to an array";_tag_arr(_tmp149,sizeof(char),
_get_zero_arr_size(_tmp149,26));}),_tag_arr(_tmp148,sizeof(void*),0));});goto
_LL103;_LL106:;_LL107: goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp14A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp14B="type is abstract (can't determine size).";
_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,41));}),_tag_arr(
_tmp14A,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp14C[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp14D="assignment to non-lvalue";
_tag_arr(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,25));}),_tag_arr(
_tmp14C,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(
te,e2,t1)){void*_tmp14E=({struct Cyc_String_pa_struct _tmp152;_tmp152.tag=0;
_tmp152.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_String_pa_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmp14F[2]={& _tmp151,& _tmp152};Cyc_Tcexp_expr_err(te,loc,({const char*
_tmp150="type mismatch: %s != %s";_tag_arr(_tmp150,sizeof(char),
_get_zero_arr_size(_tmp150,24));}),_tag_arr(_tmp14F,sizeof(void*),2));}}});Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp14E;}}else{void*_tmp153=(void*)po->v;
void*_tmp154=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp153,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp154,t1)?1:(Cyc_Tcutil_coerceable(_tmp154)?Cyc_Tcutil_coerceable(t1): 0))){
void*_tmp155=({struct Cyc_String_pa_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_String_pa_struct _tmp158;
_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp156[2]={& _tmp158,& _tmp159};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp157="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,82));}),_tag_arr(
_tmp156,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp154,t1);Cyc_Tcutil_explain_failure();
return _tmp155;}return _tmp154;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp15A=Cyc_Tcutil_compress(
t1);void*_tmp15B;void*_tmp15C;_LL109: if((int)_tmp15A != 1)goto _LL10B;_LL10A: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ);t1=dbl_typ;goto _LL108;_LL10B: if(_tmp15A <= (void*)3?1:*((int*)
_tmp15A)!= 5)goto _LL10D;_tmp15B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp15A)->f2;
if((int)_tmp15B != 0)goto _LL10D;_LL10C: goto _LL10E;_LL10D: if(_tmp15A <= (void*)3?1:*((
int*)_tmp15A)!= 5)goto _LL10F;_tmp15C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp15A)->f2;if((int)_tmp15C != 1)goto _LL10F;_LL10E: Cyc_Tcutil_unchecked_cast(te,
e,Cyc_Absyn_sint_typ);t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;
_LL108:;}for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp15E;
struct Cyc_List_List*_tmp15F;struct Cyc_Position_Segment*_tmp160;void*_tmp161;
struct Cyc_Absyn_Tunionfield _tmp15D=*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd));_tmp15E=_tmp15D.name;_tmp15F=_tmp15D.typs;_tmp160=_tmp15D.loc;
_tmp161=(void*)_tmp15D.sc;if(_tmp15F == 0?1: _tmp15F->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)((struct _tuple9*)_tmp15F->hd))).f2);if(Cyc_Tcutil_unify(
t1,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp163;struct Cyc_List_List*
_tmp164;struct Cyc_Position_Segment*_tmp165;void*_tmp166;struct Cyc_Absyn_Tunionfield
_tmp162=*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd));
_tmp163=_tmp162.name;_tmp164=_tmp162.typs;_tmp165=_tmp162.loc;_tmp166=(void*)
_tmp162.sc;if(_tmp164 == 0?1: _tmp164->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple9*)((struct _tuple9*)_tmp164->hd))).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp16A;_tmp16A.tag=0;_tmp16A.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{struct Cyc_String_pa_struct _tmp169;
_tmp169.tag=0;_tmp169.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(tu);{void*
_tmp167[2]={& _tmp169,& _tmp16A};Cyc_Tcutil_terr(e->loc,({const char*_tmp168="can't find a field in %s to inject a value of type %s";
_tag_arr(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,54));}),_tag_arr(
_tmp167,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp16C=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp16D=t;struct Cyc_Absyn_PtrInfo
_tmp16E;void*_tmp16F;void*_tmp170;struct Cyc_Absyn_Conref*_tmp171;struct Cyc_Absyn_Tqual
_tmp172;struct Cyc_Absyn_Conref*_tmp173;struct Cyc_Absyn_Conref*_tmp174;_LL112: if(
_tmp16D <= (void*)3?1:*((int*)_tmp16D)!= 4)goto _LL114;_tmp16E=((struct Cyc_Absyn_PointerType_struct*)
_tmp16D)->f1;_tmp16F=(void*)_tmp16E.elt_typ;_tmp170=(void*)_tmp16E.rgn_typ;
_tmp171=_tmp16E.nullable;_tmp172=_tmp16E.tq;_tmp173=_tmp16E.bounds;_tmp174=
_tmp16E.zero_term;_LL113: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp170);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp173);{void*_tmp175=Cyc_Tcutil_compress(_tmp16F);struct Cyc_Absyn_FnInfo
_tmp176;struct Cyc_List_List*_tmp177;struct Cyc_Core_Opt*_tmp178;void*_tmp179;
struct Cyc_List_List*_tmp17A;int _tmp17B;struct Cyc_Absyn_VarargInfo*_tmp17C;struct
Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17E;_LL117: if(_tmp175 <= (void*)3?1:*((
int*)_tmp175)!= 8)goto _LL119;_tmp176=((struct Cyc_Absyn_FnType_struct*)_tmp175)->f1;
_tmp177=_tmp176.tvars;_tmp178=_tmp176.effect;_tmp179=(void*)_tmp176.ret_typ;
_tmp17A=_tmp176.args;_tmp17B=_tmp176.c_varargs;_tmp17C=_tmp176.cyc_varargs;
_tmp17D=_tmp176.rgn_po;_tmp17E=_tmp176.attributes;_LL118: if(topt != 0)Cyc_Tcutil_unify(
_tmp179,*((void**)topt));while(_tmp16C != 0?_tmp17A != 0: 0){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp16C->hd;void*t2=(*((struct _tuple2*)((struct _tuple2*)
_tmp17A->hd))).f3;Cyc_Tcexp_tcExp(te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(
te,e1,t2)){({struct Cyc_String_pa_struct _tmp182;_tmp182.tag=0;_tmp182.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_String_pa_struct _tmp181;
_tmp181.tag=0;_tmp181.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{void*_tmp17F[2]={& _tmp181,&
_tmp182};Cyc_Tcutil_terr(e1->loc,({const char*_tmp180="actual argument has type \n%s\n but formal has type \n%s";
_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,54));}),_tag_arr(
_tmp17F,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}_tmp16C=_tmp16C->tl;
_tmp17A=_tmp17A->tl;}if(_tmp17A != 0)({void*_tmp183[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp184="too few arguments for function";_tag_arr(_tmp184,sizeof(char),
_get_zero_arr_size(_tmp184,31));}),_tag_arr(_tmp183,sizeof(void*),0));});else{
if((_tmp16C != 0?1: _tmp17B)?1: _tmp17C != 0){if(_tmp17B)for(0;_tmp16C != 0;_tmp16C=
_tmp16C->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp16C->hd);}else{if(
_tmp17C == 0)({void*_tmp185[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp186="too many arguments for function";
_tag_arr(_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,32));}),_tag_arr(
_tmp185,sizeof(void*),0));});else{void*_tmp188;int _tmp189;struct Cyc_Absyn_VarargInfo
_tmp187=*((struct Cyc_Absyn_VarargInfo*)_tmp17C);_tmp188=(void*)_tmp187.type;
_tmp189=_tmp187.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp18A=({struct Cyc_Absyn_VarargCallInfo*
_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->num_varargs=0;_tmp1A2->injectors=0;
_tmp1A2->vai=(struct Cyc_Absyn_VarargInfo*)_check_null(_tmp17C);_tmp1A2;});*((
struct Cyc_Absyn_VarargCallInfo**)vararg_call_info)=(struct Cyc_Absyn_VarargCallInfo*)
_tmp18A;if(!_tmp189)for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp16C->hd;_tmp18A->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp188,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp188)){({struct Cyc_String_pa_struct
_tmp18E;_tmp18E.tag=0;_tmp18E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_String_pa_struct
_tmp18D;_tmp18D.tag=0;_tmp18D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp188);{void*_tmp18B[2]={& _tmp18D,& _tmp18E};Cyc_Tcutil_terr(loc,({const char*
_tmp18C="vararg requires type %s but argument has type %s";_tag_arr(_tmp18C,
sizeof(char),_get_zero_arr_size(_tmp18C,49));}),_tag_arr(_tmp18B,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}}else{void*_tmp18F=Cyc_Tcutil_compress(_tmp188);
struct Cyc_Absyn_TunionInfo _tmp190;void*_tmp191;struct Cyc_Absyn_Tuniondecl**
_tmp192;struct Cyc_Absyn_Tuniondecl*_tmp193;struct Cyc_List_List*_tmp194;void*
_tmp195;_LL11C: if(_tmp18F <= (void*)3?1:*((int*)_tmp18F)!= 2)goto _LL11E;_tmp190=((
struct Cyc_Absyn_TunionType_struct*)_tmp18F)->f1;_tmp191=(void*)_tmp190.tunion_info;
if(*((int*)_tmp191)!= 1)goto _LL11E;_tmp192=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp191)->f1;_tmp193=*_tmp192;_tmp194=_tmp190.targs;_tmp195=(void*)_tmp190.rgn;
_LL11D: {struct Cyc_Absyn_Tuniondecl*_tmp196=*((struct Cyc_Absyn_Tuniondecl**)Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp193->name));struct Cyc_List_List*fields=0;if(_tmp196->fields == 0)({
struct Cyc_String_pa_struct _tmp199;_tmp199.tag=0;_tmp199.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(_tmp188);{void*_tmp197[1]={& _tmp199};Cyc_Tcutil_terr(loc,({
const char*_tmp198="can't inject into %s";_tag_arr(_tmp198,sizeof(char),
_get_zero_arr_size(_tmp198,21));}),_tag_arr(_tmp197,sizeof(void*),1));}});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp196->fields))->v;}
if(!Cyc_Tcutil_unify(_tmp195,Cyc_Tcenv_curr_rgn(te)))({void*_tmp19A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp19B="bad region for injected varargs";_tag_arr(_tmp19B,
sizeof(char),_get_zero_arr_size(_tmp19B,32));}),_tag_arr(_tmp19A,sizeof(void*),0));});{
struct _RegionHandle _tmp19C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp19C;
_push_region(rgn);{struct Cyc_List_List*_tmp19D=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp196->tvs,_tmp194);for(0;_tmp16C != 0;_tmp16C=
_tmp16C->tl){_tmp18A->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp16C->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*_tmp19E=Cyc_Tcexp_tcInjection(
te,e1,_tmp188,rgn,_tmp19D,fields);if(_tmp19E != 0)_tmp18A->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp18A->injectors,({
struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=(struct Cyc_Absyn_Tunionfield*)
_check_null(_tmp19E);_tmp19F->tl=0;_tmp19F;}));}}}};_pop_region(rgn);}goto _LL11B;}
_LL11E:;_LL11F:({void*_tmp1A0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1A1="bad inject vararg type";
_tag_arr(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,23));}),_tag_arr(
_tmp1A0,sizeof(void*),0));});goto _LL11B;_LL11B:;}}}}}}{struct Cyc_List_List*a=
_tmp17E;for(0;a != 0;a=a->tl){void*_tmp1A3=(void*)a->hd;void*_tmp1A4;int _tmp1A5;
int _tmp1A6;_LL121: if(_tmp1A3 <= (void*)17?1:*((int*)_tmp1A3)!= 3)goto _LL123;
_tmp1A4=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A3)->f1;_tmp1A5=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A3)->f2;_tmp1A6=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A3)->f3;_LL122:{struct _handler_cons _tmp1A7;_push_handler(& _tmp1A7);{int
_tmp1A9=0;if(setjmp(_tmp1A7.handler))_tmp1A9=1;if(!_tmp1A9){{struct Cyc_Absyn_Exp*
_tmp1AA=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A5 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A6 == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A6 - 1);_tmp1AB;});}{
void*_tmp1AC=_tmp1A4;_LL126: if((int)_tmp1AC != 0)goto _LL128;_LL127: Cyc_Tcexp_check_format_args(
te,_tmp1AA,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL125;_LL128: if((int)
_tmp1AC != 1)goto _LL125;_LL129: Cyc_Tcexp_check_format_args(te,_tmp1AA,fmt_args,
Cyc_Formatstr_get_scanf_typs);goto _LL125;_LL125:;}};_pop_handler();}else{void*
_tmp1A8=(void*)_exn_thrown;void*_tmp1AE=_tmp1A8;_LL12B: if(_tmp1AE != Cyc_List_Nth)
goto _LL12D;_LL12C:({void*_tmp1AF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1B0="bad format arguments";
_tag_arr(_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,21));}),_tag_arr(
_tmp1AF,sizeof(void*),0));});goto _LL12A;_LL12D:;_LL12E:(void)_throw(_tmp1AE);
_LL12A:;}}}goto _LL120;_LL123:;_LL124: goto _LL120;_LL120:;}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp178))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp17D);return _tmp179;_LL119:;_LL11A: return({void*_tmp1B1[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp1B2="expected pointer to function";_tag_arr(_tmp1B2,
sizeof(char),_get_zero_arr_size(_tmp1B2,29));}),_tag_arr(_tmp1B1,sizeof(void*),0));});
_LL116:;}_LL114:;_LL115: return({void*_tmp1B3[0]={};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp1B4="expected pointer to function";_tag_arr(_tmp1B4,sizeof(char),
_get_zero_arr_size(_tmp1B4,29));}),_tag_arr(_tmp1B3,sizeof(void*),0));});_LL111:;}}
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1B7;_tmp1B7.tag=0;_tmp1B7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1B5[1]={& _tmp1B7};Cyc_Tcutil_terr(
loc,({const char*_tmp1B6="expected xtunion exn but found %s";_tag_arr(_tmp1B6,
sizeof(char),_get_zero_arr_size(_tmp1B6,34));}),_tag_arr(_tmp1B5,sizeof(void*),1));}});
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));
_tmp1B8->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1B8;}));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp1B9=t1;struct Cyc_Absyn_PtrInfo _tmp1BA;void*
_tmp1BB;void*_tmp1BC;struct Cyc_Absyn_Conref*_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;
struct Cyc_Absyn_Conref*_tmp1BF;struct Cyc_Absyn_Conref*_tmp1C0;_LL130: if(_tmp1B9
<= (void*)3?1:*((int*)_tmp1B9)!= 4)goto _LL132;_tmp1BA=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B9)->f1;_tmp1BB=(void*)_tmp1BA.elt_typ;_tmp1BC=(void*)_tmp1BA.rgn_typ;
_tmp1BD=_tmp1BA.nullable;_tmp1BE=_tmp1BA.tq;_tmp1BF=_tmp1BA.bounds;_tmp1C0=
_tmp1BA.zero_term;_LL131:{void*_tmp1C1=Cyc_Tcutil_compress(_tmp1BB);struct Cyc_Absyn_FnInfo
_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Core_Opt*_tmp1C4;void*_tmp1C5;
struct Cyc_List_List*_tmp1C6;int _tmp1C7;struct Cyc_Absyn_VarargInfo*_tmp1C8;struct
Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1CA;_LL135: if(_tmp1C1 <= (void*)3?1:*((
int*)_tmp1C1)!= 8)goto _LL137;_tmp1C2=((struct Cyc_Absyn_FnType_struct*)_tmp1C1)->f1;
_tmp1C3=_tmp1C2.tvars;_tmp1C4=_tmp1C2.effect;_tmp1C5=(void*)_tmp1C2.ret_typ;
_tmp1C6=_tmp1C2.args;_tmp1C7=_tmp1C2.c_varargs;_tmp1C8=_tmp1C2.cyc_varargs;
_tmp1C9=_tmp1C2.rgn_po;_tmp1CA=_tmp1C2.attributes;_LL136: {struct Cyc_List_List*
instantiation=0;for(0;ts != 0?_tmp1C3 != 0: 0;(ts=ts->tl,_tmp1C3=_tmp1C3->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1C3->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=({struct _tuple5*
_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=(struct Cyc_Absyn_Tvar*)_tmp1C3->hd;
_tmp1CC->f2=(void*)ts->hd;_tmp1CC;});_tmp1CB->tl=instantiation;_tmp1CB;});}if(ts
!= 0)return({void*_tmp1CD[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1CE="too many type variables in instantiation";
_tag_arr(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,41));}),_tag_arr(
_tmp1CD,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp1D2=_cycalloc(sizeof(*
_tmp1D2));_tmp1D2[0]=({struct Cyc_Absyn_FnType_struct _tmp1D3;_tmp1D3.tag=8;
_tmp1D3.f1=({struct Cyc_Absyn_FnInfo _tmp1D4;_tmp1D4.tvars=_tmp1C3;_tmp1D4.effect=
_tmp1C4;_tmp1D4.ret_typ=(void*)_tmp1C5;_tmp1D4.args=_tmp1C6;_tmp1D4.c_varargs=
_tmp1C7;_tmp1D4.cyc_varargs=_tmp1C8;_tmp1D4.rgn_po=_tmp1C9;_tmp1D4.attributes=
_tmp1CA;_tmp1D4;});_tmp1D3;});_tmp1D2;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_Absyn_PointerType_struct
_tmp1D0;_tmp1D0.tag=4;_tmp1D0.f1=({struct Cyc_Absyn_PtrInfo _tmp1D1;_tmp1D1.elt_typ=(
void*)new_fn_typ;_tmp1D1.rgn_typ=(void*)_tmp1BC;_tmp1D1.nullable=_tmp1BD;_tmp1D1.tq=
_tmp1BE;_tmp1D1.bounds=_tmp1BF;_tmp1D1.zero_term=_tmp1C0;_tmp1D1;});_tmp1D0;});
_tmp1CF;});}}_LL137:;_LL138: goto _LL134;_LL134:;}goto _LL12F;_LL132:;_LL133: goto
_LL12F;_LL12F:;}return({struct Cyc_String_pa_struct _tmp1D7;_tmp1D7.tag=0;_tmp1D7.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp1D5[1]={& _tmp1D7};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp1D6="expecting polymorphic type but found %s";_tag_arr(
_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,40));}),_tag_arr(_tmp1D5,sizeof(
void*),1));}});}}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(te,loc,t2,t)?!Cyc_Tcutil_castable(
te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp1D8=({struct Cyc_String_pa_struct _tmp1DC;_tmp1DC.tag=0;_tmp1DC.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_String_pa_struct _tmp1DB;_tmp1DB.tag=
0;_tmp1DB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp1D9[2]={&
_tmp1DB,& _tmp1DC};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp1DA="cannot cast %s to %s";
_tag_arr(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,21));}),_tag_arr(
_tmp1D9,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp1D8;}}
return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp1DD=0;
struct Cyc_Absyn_Tqual _tmp1DE=Cyc_Absyn_empty_tqual();if(topt != 0){void*_tmp1DF=
Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo _tmp1E0;void*_tmp1E1;
struct Cyc_Absyn_Tqual _tmp1E2;struct Cyc_Absyn_Conref*_tmp1E3;_LL13A: if(_tmp1DF <= (
void*)3?1:*((int*)_tmp1DF)!= 4)goto _LL13C;_tmp1E0=((struct Cyc_Absyn_PointerType_struct*)
_tmp1DF)->f1;_tmp1E1=(void*)_tmp1E0.elt_typ;_tmp1E2=_tmp1E0.tq;_tmp1E3=_tmp1E0.zero_term;
_LL13B: _tmp1DD=({void**_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=_tmp1E1;
_tmp1E4;});_tmp1DE=_tmp1E2;goto _LL139;_LL13C:;_LL13D: goto _LL139;_LL139:;}Cyc_Tcexp_tcExpNoInst(
te,_tmp1DD,e);{void*_tmp1E5=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*
_tmp1E7;_LL13F: if(*((int*)_tmp1E5)!= 28)goto _LL141;_LL140: goto _LL142;_LL141: if(*((
int*)_tmp1E5)!= 24)goto _LL143;_LL142:({void*_tmp1E8[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmp1E9="& used to allocate";_tag_arr(_tmp1E9,sizeof(char),
_get_zero_arr_size(_tmp1E9,19));}),_tag_arr(_tmp1E8,sizeof(void*),0));});{void*(*
_tmp1EA)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp1EB=Cyc_Tcutil_compress(*((
void**)topt));struct Cyc_Absyn_PtrInfo _tmp1EC;struct Cyc_Absyn_Conref*_tmp1ED;
struct Cyc_Absyn_Conref*_tmp1EE;_LL148: if(_tmp1EB <= (void*)3?1:*((int*)_tmp1EB)!= 
4)goto _LL14A;_tmp1EC=((struct Cyc_Absyn_PointerType_struct*)_tmp1EB)->f1;_tmp1ED=
_tmp1EC.nullable;_tmp1EE=_tmp1EC.zero_term;_LL149: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp1ED))_tmp1EA=Cyc_Absyn_star_typ;goto _LL147;_LL14A:;
_LL14B: goto _LL147;_LL147:;}return _tmp1EA((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)2,_tmp1DE,Cyc_Absyn_false_conref);}_LL143: if(*((
int*)_tmp1E5)!= 23)goto _LL145;_tmp1E6=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1E5)->f1;_tmp1E7=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1E5)->f2;_LL144:{
void*_tmp1EF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1E6->topt))->v);_LL14D: if(_tmp1EF <= (void*)3?1:*((int*)_tmp1EF)!= 9)goto
_LL14F;_LL14E: goto _LL14C;_LL14F:;_LL150:(void*)(e0->r=(void*)((void*)(Cyc_Absyn_add_exp(
_tmp1E6,_tmp1E7,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp1E6,_tmp1E7);_LL14C:;}
goto _LL13E;_LL145:;_LL146: goto _LL13E;_LL13E:;}{int _tmp1F1;void*_tmp1F2;struct
_tuple6 _tmp1F0=Cyc_Tcutil_addressof_props(te,e);_tmp1F1=_tmp1F0.f1;_tmp1F2=
_tmp1F0.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();if(_tmp1F1)tq.q_const=
1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp1F2,tq,Cyc_Absyn_false_conref);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_String_pa_struct _tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmp1F3[1]={& _tmp1F5};Cyc_Tcutil_terr(loc,({
const char*_tmp1F4="sizeof applied to type %s, which has unknown size here";
_tag_arr(_tmp1F4,sizeof(char),_get_zero_arr_size(_tmp1F4,55));}),_tag_arr(
_tmp1F3,sizeof(void*),1));}});if(topt != 0){void*_tmp1F6=Cyc_Tcutil_compress(*((
void**)topt));_LL152: if(_tmp1F6 <= (void*)3?1:*((int*)_tmp1F6)!= 14)goto _LL154;
_LL153: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1F7=_cycalloc(
sizeof(*_tmp1F7));_tmp1F7[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1F8;_tmp1F8.tag=
14;_tmp1F8.f1=(void*)t;_tmp1F8;});_tmp1F7;});_LL154:;_LL155: goto _LL151;_LL151:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,
struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct _tagged_arr)*((struct
_tagged_arr*)n),(struct _tagged_arr)*((struct _tagged_arr*)sf->name))== 0;}static
void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);{void*_tmp1F9=n;struct _tagged_arr*_tmp1FA;unsigned int _tmp1FB;
_LL157: if(*((int*)_tmp1F9)!= 0)goto _LL159;_tmp1FA=((struct Cyc_Absyn_StructField_struct*)
_tmp1F9)->f1;_LL158: {int bad_type=1;{void*_tmp1FC=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Aggrdecl**_tmp1FF;struct
Cyc_List_List*_tmp200;_LL15C: if(_tmp1FC <= (void*)3?1:*((int*)_tmp1FC)!= 10)goto
_LL15E;_tmp1FD=((struct Cyc_Absyn_AggrType_struct*)_tmp1FC)->f1;_tmp1FE=(void*)
_tmp1FD.aggr_info;if(*((int*)_tmp1FE)!= 1)goto _LL15E;_tmp1FF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1FE)->f1;_LL15D: if((*((struct Cyc_Absyn_Aggrdecl**)_tmp1FF))->impl == 0)goto
_LL15B;if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1FA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*((struct Cyc_Absyn_Aggrdecl**)
_tmp1FF))->impl))->fields))({struct Cyc_String_pa_struct _tmp203;_tmp203.tag=0;
_tmp203.f1=(struct _tagged_arr)*((struct _tagged_arr*)_tmp1FA);{void*_tmp201[1]={&
_tmp203};Cyc_Tcutil_terr(loc,({const char*_tmp202="no field of struct/union has name %s";
_tag_arr(_tmp202,sizeof(char),_get_zero_arr_size(_tmp202,37));}),_tag_arr(
_tmp201,sizeof(void*),1));}});bad_type=0;goto _LL15B;_LL15E: if(_tmp1FC <= (void*)3?
1:*((int*)_tmp1FC)!= 11)goto _LL160;_tmp200=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1FC)->f2;_LL15F: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1FA,_tmp200))({struct Cyc_String_pa_struct _tmp206;_tmp206.tag=0;_tmp206.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_tmp1FA);{void*_tmp204[1]={& _tmp206};Cyc_Tcutil_terr(
loc,({const char*_tmp205="no field of struct/union has name %s";_tag_arr(_tmp205,
sizeof(char),_get_zero_arr_size(_tmp205,37));}),_tag_arr(_tmp204,sizeof(void*),1));}});
bad_type=0;goto _LL15B;_LL160:;_LL161: goto _LL15B;_LL15B:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp209;_tmp209.tag=0;_tmp209.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp207[1]={& _tmp209};Cyc_Tcutil_terr(loc,({const char*_tmp208="%s is not a known struct/union type";
_tag_arr(_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,36));}),_tag_arr(
_tmp207,sizeof(void*),1));}});goto _LL156;}_LL159: if(*((int*)_tmp1F9)!= 1)goto
_LL156;_tmp1FB=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1F9)->f1;_LL15A: {int
bad_type=1;{void*_tmp20A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp20B;
void*_tmp20C;struct Cyc_Absyn_Aggrdecl**_tmp20D;struct Cyc_List_List*_tmp20E;
struct Cyc_List_List*_tmp20F;struct Cyc_Absyn_TunionFieldInfo _tmp210;void*_tmp211;
struct Cyc_Absyn_Tunionfield*_tmp212;_LL163: if(_tmp20A <= (void*)3?1:*((int*)
_tmp20A)!= 10)goto _LL165;_tmp20B=((struct Cyc_Absyn_AggrType_struct*)_tmp20A)->f1;
_tmp20C=(void*)_tmp20B.aggr_info;if(*((int*)_tmp20C)!= 1)goto _LL165;_tmp20D=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp20C)->f1;_LL164: if((*((struct Cyc_Absyn_Aggrdecl**)
_tmp20D))->impl == 0)goto _LL162;_tmp20E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*((struct Cyc_Absyn_Aggrdecl**)_tmp20D))->impl))->fields;goto _LL166;
_LL165: if(_tmp20A <= (void*)3?1:*((int*)_tmp20A)!= 11)goto _LL167;_tmp20E=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp20A)->f2;_LL166: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp20E)<= _tmp1FB)({struct Cyc_Int_pa_struct _tmp216;_tmp216.tag=
1;_tmp216.f1=(int)_tmp1FB;{struct Cyc_Int_pa_struct _tmp215;_tmp215.tag=1;_tmp215.f1=(
int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp20E));{
void*_tmp213[2]={& _tmp215,& _tmp216};Cyc_Tcutil_terr(loc,({const char*_tmp214="struct/union has too few components: %d <= %d";
_tag_arr(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,46));}),_tag_arr(
_tmp213,sizeof(void*),2));}}});bad_type=0;goto _LL162;_LL167: if(_tmp20A <= (void*)
3?1:*((int*)_tmp20A)!= 9)goto _LL169;_tmp20F=((struct Cyc_Absyn_TupleType_struct*)
_tmp20A)->f1;_LL168: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp20F)
<= _tmp1FB)({struct Cyc_Int_pa_struct _tmp21A;_tmp21A.tag=1;_tmp21A.f1=(int)
_tmp1FB;{struct Cyc_Int_pa_struct _tmp219;_tmp219.tag=1;_tmp219.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp20F));{void*
_tmp217[2]={& _tmp219,& _tmp21A};Cyc_Tcutil_terr(loc,({const char*_tmp218="tuple has too few components: %d <= %d";
_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,39));}),_tag_arr(
_tmp217,sizeof(void*),2));}}});bad_type=0;goto _LL162;_LL169: if(_tmp20A <= (void*)
3?1:*((int*)_tmp20A)!= 3)goto _LL16B;_tmp210=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp20A)->f1;_tmp211=(void*)_tmp210.field_info;if(*((int*)_tmp211)!= 1)goto
_LL16B;_tmp212=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp211)->f2;_LL16A:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp212->typs)< _tmp1FB)({
struct Cyc_Int_pa_struct _tmp21E;_tmp21E.tag=1;_tmp21E.f1=(int)_tmp1FB;{struct Cyc_Int_pa_struct
_tmp21D;_tmp21D.tag=1;_tmp21D.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp212->typs));{void*_tmp21B[2]={& _tmp21D,& _tmp21E};Cyc_Tcutil_terr(
loc,({const char*_tmp21C="tunionfield has too few components: %d < %d";_tag_arr(
_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,44));}),_tag_arr(_tmp21B,sizeof(
void*),2));}}});bad_type=0;goto _LL162;_LL16B:;_LL16C: goto _LL162;_LL162:;}if(
bad_type)({struct Cyc_String_pa_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp21F[1]={& _tmp221};Cyc_Tcutil_terr(
loc,({const char*_tmp220="%s is not a known type";_tag_arr(_tmp220,sizeof(char),
_get_zero_arr_size(_tmp220,23));}),_tag_arr(_tmp21F,sizeof(void*),1));}});goto
_LL156;}_LL156:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp222=t;struct Cyc_Absyn_PtrInfo _tmp223;void*
_tmp224;void*_tmp225;struct Cyc_Absyn_Conref*_tmp226;struct Cyc_Absyn_Conref*
_tmp227;_LL16E: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 4)goto _LL170;_tmp223=((
struct Cyc_Absyn_PointerType_struct*)_tmp222)->f1;_tmp224=(void*)_tmp223.elt_typ;
_tmp225=(void*)_tmp223.rgn_typ;_tmp226=_tmp223.bounds;_tmp227=_tmp223.zero_term;
_LL16F: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp225);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp226);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp224),(void*)1))({
void*_tmp228[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp229="can't dereference abstract pointer type";
_tag_arr(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,40));}),_tag_arr(
_tmp228,sizeof(void*),0));});return _tmp224;_LL170:;_LL171: return({struct Cyc_String_pa_struct
_tmp22C;_tmp22C.tag=0;_tmp22C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp22A[1]={& _tmp22C};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp22B="expecting * or @ type but found %s";
_tag_arr(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,35));}),_tag_arr(
_tmp22A,sizeof(void*),1));}});_LL16D:;}}static void*Cyc_Tcexp_tcAggrMember(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp22D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp22E;void*_tmp22F;struct Cyc_Absyn_Aggrdecl**_tmp230;
struct Cyc_Absyn_Aggrdecl*_tmp231;struct Cyc_List_List*_tmp232;struct Cyc_List_List*
_tmp233;_LL173: if(_tmp22D <= (void*)3?1:*((int*)_tmp22D)!= 10)goto _LL175;_tmp22E=((
struct Cyc_Absyn_AggrType_struct*)_tmp22D)->f1;_tmp22F=(void*)_tmp22E.aggr_info;
if(*((int*)_tmp22F)!= 1)goto _LL175;_tmp230=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp22F)->f1;_tmp231=*_tmp230;_tmp232=_tmp22E.targs;_LL174: {struct Cyc_Absyn_Aggrfield*
_tmp236=Cyc_Absyn_lookup_decl_field(_tmp231,f);if(_tmp236 == 0)return({struct Cyc_String_pa_struct
_tmp23A;_tmp23A.tag=0;_tmp23A.f1=(struct _tagged_arr)*((struct _tagged_arr*)f);{
struct Cyc_String_pa_struct _tmp239;_tmp239.tag=0;_tmp239.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp231->name);{void*_tmp237[2]={& _tmp239,& _tmp23A};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp238="type %s has no %s field";_tag_arr(_tmp238,sizeof(
char),_get_zero_arr_size(_tmp238,24));}),_tag_arr(_tmp237,sizeof(void*),2));}}});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->exist_vars != 0)
return({void*_tmp23B[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp23C="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,64));}),_tag_arr(
_tmp23B,sizeof(void*),0));});{void*t2;{struct _RegionHandle _tmp23D=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp23D;_push_region(rgn);{struct Cyc_List_List*_tmp23E=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp231->tvs,_tmp232);t2=Cyc_Tcutil_rsubstitute(
rgn,_tmp23E,(void*)_tmp236->type);};_pop_region(rgn);}return t2;}}_LL175: if(
_tmp22D <= (void*)3?1:*((int*)_tmp22D)!= 11)goto _LL177;_tmp233=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp22D)->f2;_LL176: {struct Cyc_Absyn_Aggrfield*_tmp23F=Cyc_Absyn_lookup_field(
_tmp233,f);if(_tmp23F == 0)return({struct Cyc_String_pa_struct _tmp242;_tmp242.tag=
0;_tmp242.f1=(struct _tagged_arr)*((struct _tagged_arr*)f);{void*_tmp240[1]={&
_tmp242};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp241="type has no %s field";
_tag_arr(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,21));}),_tag_arr(
_tmp240,sizeof(void*),1));}});return(void*)_tmp23F->type;}_LL177: if(_tmp22D <= (
void*)3?1:*((int*)_tmp22D)!= 7)goto _LL179;if(!(Cyc_strcmp((struct _tagged_arr)*((
struct _tagged_arr*)f),({const char*_tmp234="size";_tag_arr(_tmp234,sizeof(char),
_get_zero_arr_size(_tmp234,5));}))== 0))goto _LL179;_LL178: goto _LL17A;_LL179: if(
_tmp22D <= (void*)3?1:*((int*)_tmp22D)!= 4)goto _LL17B;if(!(Cyc_strcmp((struct
_tagged_arr)*((struct _tagged_arr*)f),({const char*_tmp235="size";_tag_arr(_tmp235,
sizeof(char),_get_zero_arr_size(_tmp235,5));}))== 0))goto _LL17B;_LL17A:(void*)(
outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp243=_cycalloc(
sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_Primop_e_struct _tmp244;_tmp244.tag=
3;_tmp244.f1=(void*)((void*)19);_tmp244.f2=({struct Cyc_List_List*_tmp245=
_cycalloc(sizeof(*_tmp245));_tmp245->hd=e;_tmp245->tl=0;_tmp245;});_tmp244;});
_tmp243;})));return Cyc_Absyn_uint_typ;_LL17B:;_LL17C: if(Cyc_strcmp((struct
_tagged_arr)*((struct _tagged_arr*)f),({const char*_tmp246="size";_tag_arr(_tmp246,
sizeof(char),_get_zero_arr_size(_tmp246,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp249;_tmp249.tag=0;_tmp249.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp247[1]={& _tmp249};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp248="expecting struct, union, or array, found %s";
_tag_arr(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,44));}),_tag_arr(
_tmp247,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct _tmp24C;
_tmp24C.tag=0;_tmp24C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp24A[1]={& _tmp24C};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp24B="expecting struct or union, found %s";_tag_arr(
_tmp24B,sizeof(char),_get_zero_arr_size(_tmp24B,36));}),_tag_arr(_tmp24A,sizeof(
void*),1));}});}_LL172:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp24D=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp24E;
void*_tmp24F;void*_tmp250;struct Cyc_Absyn_Conref*_tmp251;struct Cyc_Absyn_Conref*
_tmp252;_LL17E: if(_tmp24D <= (void*)3?1:*((int*)_tmp24D)!= 4)goto _LL180;_tmp24E=((
struct Cyc_Absyn_PointerType_struct*)_tmp24D)->f1;_tmp24F=(void*)_tmp24E.elt_typ;
_tmp250=(void*)_tmp24E.rgn_typ;_tmp251=_tmp24E.bounds;_tmp252=_tmp24E.zero_term;
_LL17F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp251);{void*_tmp253=Cyc_Tcutil_compress(
_tmp24F);struct Cyc_Absyn_AggrInfo _tmp254;void*_tmp255;struct Cyc_Absyn_Aggrdecl**
_tmp256;struct Cyc_Absyn_Aggrdecl*_tmp257;struct Cyc_List_List*_tmp258;struct Cyc_List_List*
_tmp259;_LL183: if(_tmp253 <= (void*)3?1:*((int*)_tmp253)!= 10)goto _LL185;_tmp254=((
struct Cyc_Absyn_AggrType_struct*)_tmp253)->f1;_tmp255=(void*)_tmp254.aggr_info;
if(*((int*)_tmp255)!= 1)goto _LL185;_tmp256=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp255)->f1;_tmp257=*_tmp256;_tmp258=_tmp254.targs;_LL184: {struct Cyc_Absyn_Aggrfield*
_tmp25A=Cyc_Absyn_lookup_decl_field(_tmp257,f);if(_tmp25A == 0)return({struct Cyc_String_pa_struct
_tmp25E;_tmp25E.tag=0;_tmp25E.f1=(struct _tagged_arr)*((struct _tagged_arr*)f);{
struct Cyc_String_pa_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp257->name);{void*_tmp25B[2]={& _tmp25D,& _tmp25E};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp25C="type %s has no %s field";_tag_arr(_tmp25C,sizeof(
char),_get_zero_arr_size(_tmp25C,24));}),_tag_arr(_tmp25B,sizeof(void*),2));}}});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp257->impl))->exist_vars != 0)
return({void*_tmp25F[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp260="must use pattern-matching to access fields of existential types";
_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,64));}),_tag_arr(
_tmp25F,sizeof(void*),0));});{void*t3;{struct _RegionHandle _tmp261=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp261;_push_region(rgn);{struct Cyc_List_List*_tmp262=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp257->tvs,_tmp258);t3=Cyc_Tcutil_rsubstitute(
rgn,_tmp262,(void*)_tmp25A->type);};_pop_region(rgn);}return t3;}}_LL185: if(
_tmp253 <= (void*)3?1:*((int*)_tmp253)!= 11)goto _LL187;_tmp259=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp253)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp263=Cyc_Absyn_lookup_field(
_tmp259,f);if(_tmp263 == 0)return({struct Cyc_String_pa_struct _tmp266;_tmp266.tag=
0;_tmp266.f1=(struct _tagged_arr)*((struct _tagged_arr*)f);{void*_tmp264[1]={&
_tmp266};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp265="type has no %s field";
_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,21));}),_tag_arr(
_tmp264,sizeof(void*),1));}});return(void*)_tmp263->type;}_LL187:;_LL188: goto
_LL182;_LL182:;}goto _LL17D;_LL180:;_LL181: goto _LL17D;_LL17D:;}return({struct Cyc_String_pa_struct
_tmp269;_tmp269.tag=0;_tmp269.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp267[1]={& _tmp269};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp268="expecting struct pointer, found %s";_tag_arr(_tmp268,
sizeof(char),_get_zero_arr_size(_tmp268,35));}),_tag_arr(_tmp267,sizeof(void*),1));}});}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp26B;int
_tmp26C;struct _tuple7 _tmp26A=Cyc_Evexp_eval_const_uint_exp(index);_tmp26B=
_tmp26A.f1;_tmp26C=_tmp26A.f2;if(!_tmp26C)return({void*_tmp26D[0]={};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp26E="tuple projection cannot use sizeof or offsetof";
_tag_arr(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,47));}),_tag_arr(
_tmp26D,sizeof(void*),0));});{struct _handler_cons _tmp26F;_push_handler(& _tmp26F);{
int _tmp271=0;if(setjmp(_tmp26F.handler))_tmp271=1;if(!_tmp271){{void*_tmp272=(*((
struct _tuple9*)((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(
int)_tmp26B))).f2;_npop_handler(0);return _tmp272;};_pop_handler();}else{void*
_tmp270=(void*)_exn_thrown;void*_tmp274=_tmp270;_LL18A: if(_tmp274 != Cyc_List_Nth)
goto _LL18C;_LL18B: return({struct Cyc_Int_pa_struct _tmp278;_tmp278.tag=1;_tmp278.f1=(
int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts));{struct
Cyc_Int_pa_struct _tmp277;_tmp277.tag=1;_tmp277.f1=(int)_tmp26B;{void*_tmp275[2]={&
_tmp277,& _tmp278};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp276="index is %d but tuple has only %d fields";
_tag_arr(_tmp276,sizeof(char),_get_zero_arr_size(_tmp276,41));}),_tag_arr(
_tmp275,sizeof(void*),2));}}});_LL18C:;_LL18D:(void)_throw(_tmp274);_LL189:;}}}}
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(te,e2))return({struct
Cyc_String_pa_struct _tmp27B;_tmp27B.tag=0;_tmp27B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{void*_tmp279[1]={& _tmp27B};Cyc_Tcexp_expr_err(te,e2->loc,({const char*
_tmp27A="expecting int subscript, found %s";_tag_arr(_tmp27A,sizeof(char),
_get_zero_arr_size(_tmp27A,34));}),_tag_arr(_tmp279,sizeof(void*),1));}});{void*
_tmp27C=t1;struct Cyc_Absyn_PtrInfo _tmp27D;void*_tmp27E;void*_tmp27F;struct Cyc_Absyn_Tqual
_tmp280;struct Cyc_Absyn_Conref*_tmp281;struct Cyc_Absyn_Conref*_tmp282;struct Cyc_List_List*
_tmp283;_LL18F: if(_tmp27C <= (void*)3?1:*((int*)_tmp27C)!= 4)goto _LL191;_tmp27D=((
struct Cyc_Absyn_PointerType_struct*)_tmp27C)->f1;_tmp27E=(void*)_tmp27D.elt_typ;
_tmp27F=(void*)_tmp27D.rgn_typ;_tmp280=_tmp27D.tq;_tmp281=_tmp27D.bounds;_tmp282=
_tmp27D.zero_term;_LL190: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp282)){int emit_warning=1;{void*_tmp284=(void*)(Cyc_Absyn_compress_conref(
_tmp281))->v;void*_tmp285;struct Cyc_Absyn_Exp*_tmp286;void*_tmp287;_LL196: if(
_tmp284 <= (void*)1?1:*((int*)_tmp284)!= 0)goto _LL198;_tmp285=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp284)->f1;if(_tmp285 <= (void*)1?1:*((int*)_tmp285)!= 0)goto _LL198;_tmp286=((
struct Cyc_Absyn_Upper_b_struct*)_tmp285)->f1;_LL197: if(Cyc_Tcutil_is_const_exp(
te,e2)){unsigned int _tmp289;int _tmp28A;struct _tuple7 _tmp288=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;if(_tmp28A){unsigned int _tmp28C;int
_tmp28D;struct _tuple7 _tmp28B=Cyc_Evexp_eval_const_uint_exp(_tmp286);_tmp28C=
_tmp28B.f1;_tmp28D=_tmp28B.f2;if(_tmp28D?_tmp28C > _tmp289: 0)emit_warning=0;}}
goto _LL195;_LL198: if(_tmp284 <= (void*)1?1:*((int*)_tmp284)!= 0)goto _LL19A;
_tmp287=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp284)->f1;if((int)_tmp287
!= 0)goto _LL19A;_LL199: emit_warning=0;goto _LL195;_LL19A:;_LL19B: goto _LL195;
_LL195:;}if(emit_warning)({void*_tmp28E[0]={};Cyc_Tcutil_warn(e2->loc,({const
char*_tmp28F="subscript on thin, zero-terminated pointer could be expensive.";
_tag_arr(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,63));}),_tag_arr(
_tmp28E,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(te,e2)){
unsigned int _tmp291;int _tmp292;struct _tuple7 _tmp290=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp291=_tmp290.f1;_tmp292=_tmp290.f2;if(_tmp292)Cyc_Tcutil_check_bound(loc,
_tmp291,_tmp281);}else{if(Cyc_Tcutil_is_bound_one(_tmp281)?!((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp282): 0)({void*_tmp293[0]={};Cyc_Tcutil_warn(
e1->loc,({const char*_tmp294="subscript applied to pointer to one object";
_tag_arr(_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,43));}),_tag_arr(
_tmp293,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,_tmp281);}}Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp27F);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp27E),(void*)1))({
void*_tmp295[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp296="can't subscript an abstract pointer";
_tag_arr(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,36));}),_tag_arr(
_tmp295,sizeof(void*),0));});return _tmp27E;_LL191: if(_tmp27C <= (void*)3?1:*((int*)
_tmp27C)!= 9)goto _LL193;_tmp283=((struct Cyc_Absyn_TupleType_struct*)_tmp27C)->f1;
_LL192: return Cyc_Tcexp_ithTupleType(te,loc,_tmp283,e2);_LL193:;_LL194: return({
struct Cyc_String_pa_struct _tmp299;_tmp299.tag=0;_tmp299.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t1);{void*_tmp297[1]={& _tmp299};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp298="subscript applied to %s";_tag_arr(_tmp298,sizeof(char),
_get_zero_arr_size(_tmp298,24));}),_tag_arr(_tmp297,sizeof(void*),1));}});_LL18E:;}}}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp29A=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_List_List*
_tmp29B;_LL19D: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 9)goto _LL19F;_tmp29B=((
struct Cyc_Absyn_TupleType_struct*)_tmp29A)->f1;_LL19E: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp29B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es))goto _LL19C;for(0;es != 0;(es=es->tl,_tmp29B=_tmp29B->tl)){void*_tmp29C=(*((
struct _tuple9*)((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp29B))->hd))).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp29C,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->hd=({
struct _tuple9*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E->f1=(*((struct _tuple9*)((
struct _tuple9*)_tmp29B->hd))).f1;_tmp29E->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp29E;});_tmp29D->tl=
fields;_tmp29D;});}done=1;goto _LL19C;_LL19F:;_LL1A0: goto _LL19C;_LL19C:;}if(!done)
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->hd=({
struct _tuple9*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->f1=Cyc_Absyn_empty_tqual();
_tmp2A0->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2A0;});_tmp29F->tl=fields;_tmp29F;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2A2;_tmp2A2.tag=9;_tmp2A2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2A2;});_tmp2A1;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2A3[0]={};Cyc_Tcexp_expr_err(te,loc,({
const char*_tmp2A4="tcCompoundLit";_tag_arr(_tmp2A4,sizeof(char),
_get_zero_arr_size(_tmp2A4,14));}),_tag_arr(_tmp2A3,sizeof(void*),0));});}static
void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**elt_topt,int zero_term,struct Cyc_List_List*des){struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*
res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp2BF;}));int _tmp2A5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
struct Cyc_Absyn_Const_e_struct*_tmp2A6=({struct Cyc_Absyn_Const_e_struct*_tmp2BB=
_cycalloc(sizeof(*_tmp2BB));_tmp2BB[0]=({struct Cyc_Absyn_Const_e_struct _tmp2BC;
_tmp2BC.tag=0;_tmp2BC.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp2BD=
_cycalloc(sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_Absyn_Int_c_struct _tmp2BE;
_tmp2BE.tag=2;_tmp2BE.f1=(void*)((void*)1);_tmp2BE.f2=_tmp2A5;_tmp2BE;});_tmp2BD;}));
_tmp2BC;});_tmp2BB;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2A6,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2A7=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2A5 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2A7))({void*_tmp2A8[0]={};Cyc_Tcutil_terr(_tmp2A7->loc,({const char*_tmp2A9="zero-terminated array doesn't end with zero.";
_tag_arr(_tmp2A9,sizeof(char),_get_zero_arr_size(_tmp2A9,45));}),_tag_arr(
_tmp2A8,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp2AA=
_cycalloc(sizeof(*_tmp2AA));_tmp2AA->v=(void*)Cyc_Absyn_uint_typ;_tmp2AA;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2AD;_tmp2AD.tag=0;_tmp2AD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(res);{
void*_tmp2AB[1]={& _tmp2AD};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp2AC="elements of array do not all have the same type (%s)";
_tag_arr(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,53));}),_tag_arr(
_tmp2AB,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple8*)((struct _tuple8*)des->hd))).f1;if(ds != 
0){void*_tmp2AE=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2AF;_LL1A2: if(*((int*)
_tmp2AE)!= 1)goto _LL1A4;_LL1A3:({void*_tmp2B0[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp2B1="only array index designators are supported";_tag_arr(_tmp2B1,
sizeof(char),_get_zero_arr_size(_tmp2B1,43));}),_tag_arr(_tmp2B0,sizeof(void*),0));});
goto _LL1A1;_LL1A4: if(*((int*)_tmp2AE)!= 0)goto _LL1A1;_tmp2AF=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2AE)->f1;_LL1A5: Cyc_Tcexp_tcExpInitializer(te,0,_tmp2AF);{unsigned int
_tmp2B3;int _tmp2B4;struct _tuple7 _tmp2B2=Cyc_Evexp_eval_const_uint_exp(_tmp2AF);
_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;if(!_tmp2B4)({void*_tmp2B5[0]={};Cyc_Tcutil_terr(
_tmp2AF->loc,({const char*_tmp2B6="index designator cannot use sizeof or offsetof";
_tag_arr(_tmp2B6,sizeof(char),_get_zero_arr_size(_tmp2B6,47));}),_tag_arr(
_tmp2B5,sizeof(void*),0));});else{if(_tmp2B3 != offset)({struct Cyc_Int_pa_struct
_tmp2BA;_tmp2BA.tag=1;_tmp2BA.f1=(int)_tmp2B3;{struct Cyc_Int_pa_struct _tmp2B9;
_tmp2B9.tag=1;_tmp2B9.f1=(int)((unsigned int)offset);{void*_tmp2B7[2]={& _tmp2B9,&
_tmp2BA};Cyc_Tcutil_terr(_tmp2AF->loc,({const char*_tmp2B8="expecting index designator %d but found %d";
_tag_arr(_tmp2B8,sizeof(char),_get_zero_arr_size(_tmp2B8,43));}),_tag_arr(
_tmp2B7,sizeof(void*),2));}}});}goto _LL1A1;}_LL1A1:;}}}return res_t2;}}static void*
Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
bound->topt))->v);{void*_tmp2C0[1]={& _tmp2C2};Cyc_Tcutil_terr(bound->loc,({const
char*_tmp2C1="expecting unsigned int, found %s";_tag_arr(_tmp2C1,sizeof(char),
_get_zero_arr_size(_tmp2C1,33));}),_tag_arr(_tmp2C0,sizeof(void*),1));}});if(!(
vd->tq).q_const)({void*_tmp2C3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2C4="comprehension index variable is not declared const!";
_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,52));}),_tag_arr(
_tmp2C3,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(loc,te);te=
Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;void**_tmp2C5=
0;struct Cyc_Absyn_Tqual*_tmp2C6=0;struct Cyc_Absyn_Conref**_tmp2C7=0;if(topt != 0){
void*_tmp2C8=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo _tmp2C9;
struct Cyc_Absyn_ArrayInfo _tmp2CA;void*_tmp2CB;void**_tmp2CC;struct Cyc_Absyn_Tqual
_tmp2CD;struct Cyc_Absyn_Tqual*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Conref*
_tmp2D0;struct Cyc_Absyn_Conref**_tmp2D1;_LL1A7: if(_tmp2C8 <= (void*)3?1:*((int*)
_tmp2C8)!= 4)goto _LL1A9;_tmp2C9=((struct Cyc_Absyn_PointerType_struct*)_tmp2C8)->f1;
_LL1A8: pinfo=_tmp2C9;_tmp2C5=(void**)((void**)((void*)& pinfo.elt_typ));_tmp2C6=(
struct Cyc_Absyn_Tqual*)& pinfo.tq;_tmp2C7=(struct Cyc_Absyn_Conref**)& pinfo.zero_term;
goto _LL1A6;_LL1A9: if(_tmp2C8 <= (void*)3?1:*((int*)_tmp2C8)!= 7)goto _LL1AB;
_tmp2CA=((struct Cyc_Absyn_ArrayType_struct*)_tmp2C8)->f1;_tmp2CB=(void*)_tmp2CA.elt_type;
_tmp2CC=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp2C8)->f1).elt_type;
_tmp2CD=_tmp2CA.tq;_tmp2CE=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp2C8)->f1).tq;_tmp2CF=_tmp2CA.num_elts;_tmp2D0=_tmp2CA.zero_term;_tmp2D1=(
struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp2C8)->f1).zero_term;
_LL1AA: _tmp2C5=(void**)_tmp2CC;_tmp2C6=(struct Cyc_Absyn_Tqual*)_tmp2CE;_tmp2C7=(
struct Cyc_Absyn_Conref**)_tmp2D1;goto _LL1A6;_LL1AB:;_LL1AC: goto _LL1A6;_LL1A6:;}{
void*t=Cyc_Tcexp_tcExp(te,_tmp2C5,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp2D2[0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp2D3="bound is not constant";
_tag_arr(_tmp2D3,sizeof(char),_get_zero_arr_size(_tmp2D3,22));}),_tag_arr(
_tmp2D2,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(te,body))({void*_tmp2D4[
0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp2D5="body is not constant";
_tag_arr(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,21));}),_tag_arr(
_tmp2D4,sizeof(void*),0));});}if(_tmp2C7 != 0?((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,*((struct Cyc_Absyn_Conref**)_tmp2C7)): 0){struct Cyc_Absyn_Exp*
_tmp2D6=Cyc_Absyn_uint_exp(1,0);_tmp2D6->topt=({struct Cyc_Core_Opt*_tmp2D7=
_cycalloc(sizeof(*_tmp2D7));_tmp2D7->v=(void*)Cyc_Absyn_uint_typ;_tmp2D7;});
bound=Cyc_Absyn_add_exp(bound,_tmp2D6,0);bound->topt=({struct Cyc_Core_Opt*
_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->v=(void*)Cyc_Absyn_uint_typ;_tmp2D8;});*((
int*)is_zero_term)=1;}return Cyc_Absyn_array_typ(t,_tmp2C6 == 0?Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*)_tmp2C6),(struct Cyc_Absyn_Exp*)bound,_tmp2C7 == 0?Cyc_Absyn_false_conref:*((
struct Cyc_Absyn_Conref**)_tmp2C7));}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*((struct Cyc_Absyn_Aggrdecl**)ad_opt)!= 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*((struct Cyc_Absyn_Aggrdecl**)ad_opt));else{{struct _handler_cons
_tmp2D9;_push_handler(& _tmp2D9);{int _tmp2DB=0;if(setjmp(_tmp2D9.handler))_tmp2DB=
1;if(!_tmp2DB){ad=*((struct Cyc_Absyn_Aggrdecl**)Cyc_Tcenv_lookup_aggrdecl(te,loc,*((
struct _tuple1**)tn)));;_pop_handler();}else{void*_tmp2DA=(void*)_exn_thrown;void*
_tmp2DD=_tmp2DA;_LL1AE: if(_tmp2DD != Cyc_Dict_Absent)goto _LL1B0;_LL1AF:({struct
Cyc_String_pa_struct _tmp2E0;_tmp2E0.tag=0;_tmp2E0.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*((
struct _tuple1**)tn));{void*_tmp2DE[1]={& _tmp2E0};Cyc_Tcutil_terr(loc,({const char*
_tmp2DF="unbound struct name %s";_tag_arr(_tmp2DF,sizeof(char),
_get_zero_arr_size(_tmp2DF,23));}),_tag_arr(_tmp2DE,sizeof(void*),1));}});return
topt != 0?*((void**)topt):(void*)0;_LL1B0:;_LL1B1:(void)_throw(_tmp2DD);_LL1AD:;}}}*((
struct Cyc_Absyn_Aggrdecl**)ad_opt)=(struct Cyc_Absyn_Aggrdecl*)ad;*((struct
_tuple1**)tn)=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp2E1[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp2E2="expecting struct but found union";_tag_arr(_tmp2E2,
sizeof(char),_get_zero_arr_size(_tmp2E2,33));}),_tag_arr(_tmp2E1,sizeof(void*),0));});
if(ad->impl == 0){({void*_tmp2E3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2E4="can't build abstract struct";
_tag_arr(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,28));}),_tag_arr(
_tmp2E3,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp2E5;}));}{struct _RegionHandle _tmp2E6=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp2E6;_push_region(rgn);{struct _tuple4 _tmp2E7=({struct _tuple4 _tmp308;
_tmp308.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp308.f2=rgn;_tmp308;});struct Cyc_List_List*
_tmp2E8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp2E7,ad->tvs);struct Cyc_List_List*_tmp2E9=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp2E7,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp2EA=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp2E8);struct Cyc_List_List*_tmp2EB=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp2E9);struct Cyc_List_List*_tmp2EC=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp2E8,_tmp2E9);struct Cyc_Absyn_AggrType_struct*_tmp2ED=({struct Cyc_Absyn_AggrType_struct*
_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302[0]=({struct Cyc_Absyn_AggrType_struct
_tmp303;_tmp303.tag=10;_tmp303.f1=({struct Cyc_Absyn_AggrInfo _tmp304;_tmp304.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp305=_cycalloc(sizeof(*
_tmp305));_tmp305[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp306;_tmp306.tag=1;
_tmp306.f1=({struct Cyc_Absyn_Aggrdecl**_tmp307=_cycalloc(sizeof(*_tmp307));
_tmp307[0]=ad;_tmp307;});_tmp306;});_tmp305;}));_tmp304.targs=_tmp2EA;_tmp304;});
_tmp303;});_tmp302;});struct Cyc_List_List*_tmp2EE=*((struct Cyc_List_List**)ts);
struct Cyc_List_List*_tmp2EF=_tmp2EB;while(_tmp2EE != 0?_tmp2EF != 0: 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp2EE->hd);Cyc_Tcutil_unify((
void*)_tmp2EE->hd,(void*)_tmp2EF->hd);_tmp2EE=_tmp2EE->tl;_tmp2EF=_tmp2EF->tl;}
if(_tmp2EE != 0)({void*_tmp2F0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2F1="too many explicit witness types";
_tag_arr(_tmp2F1,sizeof(char),_get_zero_arr_size(_tmp2F1,32));}),_tag_arr(
_tmp2F0,sizeof(void*),0));});*((struct Cyc_List_List**)ts)=_tmp2EB;if(topt != 0)
Cyc_Tcutil_unify((void*)_tmp2ED,*((void**)topt));{struct Cyc_List_List*fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp2F3;struct Cyc_Absyn_Aggrfield*
_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;struct _tuple10*_tmp2F2=(struct _tuple10*)
fields->hd;_tmp2F3=*_tmp2F2;_tmp2F4=_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;{void*_tmp2F6=
Cyc_Tcutil_rsubstitute(rgn,_tmp2EC,(void*)_tmp2F4->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp2F6,_tmp2F5);if(!Cyc_Tcutil_coerce_arg(te,_tmp2F5,_tmp2F6)){({
struct Cyc_String_pa_struct _tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F5->topt))->v);{
struct Cyc_String_pa_struct _tmp2FB;_tmp2FB.tag=0;_tmp2FB.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(_tmp2F6);{struct Cyc_String_pa_struct _tmp2FA;_tmp2FA.tag=0;
_tmp2FA.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*((struct _tuple1**)tn));{
struct Cyc_String_pa_struct _tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp2F4->name);{void*_tmp2F7[4]={& _tmp2F9,& _tmp2FA,& _tmp2FB,&
_tmp2FC};Cyc_Tcutil_terr(_tmp2F5->loc,({const char*_tmp2F8="field %s of struct %s expects type %s != %s";
_tag_arr(_tmp2F8,sizeof(char),_get_zero_arr_size(_tmp2F8,44));}),_tag_arr(
_tmp2F7,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*
_tmp2FD=0;{struct Cyc_List_List*_tmp2FE=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp2FE != 0;_tmp2FE=_tmp2FE->tl){_tmp2FD=({
struct Cyc_List_List*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->hd=({struct
_tuple0*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp2EC,(*((struct _tuple0*)((struct _tuple0*)_tmp2FE->hd))).f1);_tmp300->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp2EC,(*((struct _tuple0*)((struct _tuple0*)_tmp2FE->hd))).f2);
_tmp300;});_tmp2FF->tl=_tmp2FD;_tmp2FF;});}}_tmp2FD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2FD);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp2FD);{void*_tmp301=(void*)_tmp2ED;_npop_handler(0);return _tmp301;}}}};
_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*args){{struct
_RegionHandle _tmp309=_new_region("rgn");struct _RegionHandle*rgn=& _tmp309;
_push_region(rgn);{void*_tmp30A=Cyc_Tcutil_compress(ts);void*_tmp30B;struct Cyc_List_List*
_tmp30C;_LL1B3: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 11)goto _LL1B5;_tmp30B=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp30A)->f1;_tmp30C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp30A)->f2;_LL1B4: if(_tmp30B == (void*)1)({void*_tmp30D[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp30E="expecting struct but found union";_tag_arr(_tmp30E,
sizeof(char),_get_zero_arr_size(_tmp30E,33));}),_tag_arr(_tmp30D,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp30C);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp310;struct Cyc_Absyn_Aggrfield*
_tmp311;struct Cyc_Absyn_Exp*_tmp312;struct _tuple10*_tmp30F=(struct _tuple10*)
fields->hd;_tmp310=*_tmp30F;_tmp311=_tmp310.f1;_tmp312=_tmp310.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp311->type)),_tmp312);if(!Cyc_Tcutil_coerce_arg(
te,_tmp312,(void*)_tmp311->type)){({struct Cyc_String_pa_struct _tmp317;_tmp317.tag=
0;_tmp317.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp312->topt))->v);{struct Cyc_String_pa_struct _tmp316;_tmp316.tag=0;
_tmp316.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)_tmp311->type);{
struct Cyc_String_pa_struct _tmp315;_tmp315.tag=0;_tmp315.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_tmp311->name);{void*_tmp313[3]={& _tmp315,& _tmp316,& _tmp317};
Cyc_Tcutil_terr(_tmp312->loc,({const char*_tmp314="field %s of struct expects type %s != %s";
_tag_arr(_tmp314,sizeof(char),_get_zero_arr_size(_tmp314,41));}),_tag_arr(
_tmp313,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1B2;}
_LL1B5:;_LL1B6:({void*_tmp318[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp319="tcAnonStruct: wrong type";
_tag_arr(_tmp319,sizeof(char),_get_zero_arr_size(_tmp319,25));}),_tag_arr(
_tmp318,sizeof(void*),0));});_LL1B2:;};_pop_region(rgn);}return ts;}static void*
Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*tud,
struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp31A=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp31A;_push_region(rgn);{struct _tuple4 _tmp31B=({
struct _tuple4 _tmp33E;_tmp33E.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp33E.f2=rgn;
_tmp33E;});struct Cyc_List_List*_tmp31C=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp31B,tud->tvs);struct Cyc_List_List*_tmp31D=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp31C);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp33A;_tmp33A.tag=3;_tmp33A.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp33B;
_tmp33B.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp33D;_tmp33D.tag=1;_tmp33D.f1=tud;_tmp33D.f2=tuf;_tmp33D;});_tmp33C;}));
_tmp33B.targs=_tmp31D;_tmp33B;});_tmp33A;});_tmp339;});if(topt != 0){void*_tmp31E=
Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_TunionInfo _tmp31F;struct Cyc_List_List*
_tmp320;void*_tmp321;_LL1B8: if(_tmp31E <= (void*)3?1:*((int*)_tmp31E)!= 3)goto
_LL1BA;_LL1B9: Cyc_Tcutil_unify(*((void**)topt),res);goto _LL1B7;_LL1BA: if(_tmp31E
<= (void*)3?1:*((int*)_tmp31E)!= 2)goto _LL1BC;_tmp31F=((struct Cyc_Absyn_TunionType_struct*)
_tmp31E)->f1;_tmp320=_tmp31F.targs;_tmp321=(void*)_tmp31F.rgn;_LL1BB:{struct Cyc_List_List*
a=_tmp31D;for(0;a != 0?_tmp320 != 0: 0;(a=a->tl,_tmp320=_tmp320->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp320->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->v=(void*)res;_tmp322;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323[0]=({
struct Cyc_Absyn_TunionType_struct _tmp324;_tmp324.tag=2;_tmp324.f1=({struct Cyc_Absyn_TunionInfo
_tmp325;_tmp325.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp327;_tmp327.tag=1;_tmp327.f1=({struct Cyc_Absyn_Tuniondecl**_tmp328=_cycalloc(
sizeof(*_tmp328));_tmp328[0]=tud;_tmp328;});_tmp327;});_tmp326;}));_tmp325.targs=
_tmp31D;_tmp325.rgn=(void*)_tmp321;_tmp325;});_tmp324;});_tmp323;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp329=res;_npop_handler(0);return _tmp329;}}goto _LL1B7;_LL1BC:;
_LL1BD: goto _LL1B7;_LL1B7:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp31C,(*((struct _tuple9*)((struct _tuple9*)ts->hd))).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp32E;_tmp32E.tag=0;_tmp32E.f1=(struct _tagged_arr)(e->topt
== 0?({const char*_tmp32F="?";_tag_arr(_tmp32F,sizeof(char),_get_zero_arr_size(
_tmp32F,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
struct Cyc_String_pa_struct _tmp32D;_tmp32D.tag=0;_tmp32D.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{struct Cyc_String_pa_struct _tmp32C;_tmp32C.tag=0;
_tmp32C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp32A[3]={&
_tmp32C,& _tmp32D,& _tmp32E};Cyc_Tcutil_terr(e->loc,({const char*_tmp32B="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,80));}),_tag_arr(
_tmp32A,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0){void*
_tmp333=({struct Cyc_String_pa_struct _tmp332;_tmp332.tag=0;_tmp332.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp330[1]={& _tmp332};Cyc_Tcexp_expr_err(
te,((struct Cyc_Absyn_Exp*)es->hd)->loc,({const char*_tmp331="too many arguments for tunion constructor %s";
_tag_arr(_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,45));}),_tag_arr(
_tmp330,sizeof(void*),1));}});_npop_handler(0);return _tmp333;}if(ts != 0){void*
_tmp337=({struct Cyc_String_pa_struct _tmp336;_tmp336.tag=0;_tmp336.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp334[1]={& _tmp336};Cyc_Tcexp_expr_err(
te,loc,({const char*_tmp335="too few arguments for tunion constructor %s";
_tag_arr(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,44));}),_tag_arr(
_tmp334,sizeof(void*),1));}});_npop_handler(0);return _tmp337;}{void*_tmp338=res;
_npop_handler(0);return _tmp338;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp33F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp340;
struct Cyc_Absyn_Conref*_tmp341;struct Cyc_Absyn_ArrayInfo _tmp342;void*_tmp343;
struct Cyc_List_List*_tmp344;struct Cyc_Absyn_AggrInfo _tmp345;void*_tmp346;struct
Cyc_List_List*_tmp347;struct Cyc_List_List*_tmp348;_LL1BF: if((int)_tmp33F != 0)
goto _LL1C1;_LL1C0: return 1;_LL1C1: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 0)
goto _LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 1)
goto _LL1C5;_LL1C4: goto _LL1C6;_LL1C5: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 2)
goto _LL1C7;_LL1C6: goto _LL1C8;_LL1C7: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 3)
goto _LL1C9;_LL1C8: return 0;_LL1C9: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 4)
goto _LL1CB;_tmp340=((struct Cyc_Absyn_PointerType_struct*)_tmp33F)->f1;_tmp341=
_tmp340.nullable;_LL1CA: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp341);_LL1CB: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 5)goto _LL1CD;_LL1CC:
goto _LL1CE;_LL1CD: if((int)_tmp33F != 1)goto _LL1CF;_LL1CE: goto _LL1D0;_LL1CF: if(
_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 6)goto _LL1D1;_LL1D0: return 1;_LL1D1: if(
_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 7)goto _LL1D3;_tmp342=((struct Cyc_Absyn_ArrayType_struct*)
_tmp33F)->f1;_tmp343=(void*)_tmp342.elt_type;_LL1D2: return Cyc_Tcexp_zeroable_type(
_tmp343);_LL1D3: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 8)goto _LL1D5;_LL1D4:
return 0;_LL1D5: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 9)goto _LL1D7;_tmp344=((
struct Cyc_Absyn_TupleType_struct*)_tmp33F)->f1;_LL1D6: for(0;(unsigned int)
_tmp344;_tmp344=_tmp344->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)((
struct _tuple9*)_tmp344->hd))).f2))return 0;}return 1;_LL1D7: if(_tmp33F <= (void*)3?
1:*((int*)_tmp33F)!= 10)goto _LL1D9;_tmp345=((struct Cyc_Absyn_AggrType_struct*)
_tmp33F)->f1;_tmp346=(void*)_tmp345.aggr_info;_tmp347=_tmp345.targs;_LL1D8: {
struct Cyc_Absyn_Aggrdecl*_tmp349=Cyc_Absyn_get_known_aggrdecl(_tmp346);if(
_tmp349->impl == 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp349->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp34A=_new_region("r");struct _RegionHandle*r=&
_tmp34A;_push_region(r);{struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp349->tvs,_tmp347);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp349->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp34B,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp34C=0;_npop_handler(0);return _tmp34C;}}}{int _tmp34D=1;_npop_handler(0);
return _tmp34D;}};_pop_region(r);}}_LL1D9: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)
!= 12)goto _LL1DB;_LL1DA: return 1;_LL1DB: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)
!= 14)goto _LL1DD;_LL1DC: return 1;_LL1DD: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)
!= 17)goto _LL1DF;_LL1DE: return 1;_LL1DF: if(_tmp33F <= (void*)3?1:*((int*)_tmp33F)
!= 11)goto _LL1E1;_tmp348=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp33F)->f2;
_LL1E0: for(0;_tmp348 != 0;_tmp348=_tmp348->tl){if(!Cyc_Tcexp_zeroable_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp348->hd)->type))return 0;}return 1;_LL1E1: if(
_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 13)goto _LL1E3;_LL1E2: return 1;_LL1E3: if(
_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 16)goto _LL1E5;_LL1E4: return 0;_LL1E5: if(
_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 15)goto _LL1E7;_LL1E6: return 0;_LL1E7: if(
_tmp33F <= (void*)3?1:*((int*)_tmp33F)!= 18)goto _LL1E9;_LL1E8: goto _LL1EA;_LL1E9:
if((int)_tmp33F != 2)goto _LL1EB;_LL1EA: goto _LL1EC;_LL1EB: if(_tmp33F <= (void*)3?1:*((
int*)_tmp33F)!= 19)goto _LL1ED;_LL1EC: goto _LL1EE;_LL1ED: if(_tmp33F <= (void*)3?1:*((
int*)_tmp33F)!= 20)goto _LL1EF;_LL1EE: goto _LL1F0;_LL1EF: if(_tmp33F <= (void*)3?1:*((
int*)_tmp33F)!= 21)goto _LL1BE;_LL1F0:({struct Cyc_String_pa_struct _tmp350;_tmp350.tag=
0;_tmp350.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp34E[1]={&
_tmp350};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp34F="bad type `%s' in zeroable type";_tag_arr(_tmp34F,sizeof(char),
_get_zero_arr_size(_tmp34F,31));}),_tag_arr(_tmp34E,sizeof(void*),1));}});_LL1BE:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(
t): 0))return;if(topt != 0){void*_tmp351=Cyc_Tcutil_compress(*((void**)topt));
struct Cyc_Absyn_PtrInfo _tmp352;void*_tmp353;_LL1F2: if(_tmp351 <= (void*)3?1:*((
int*)_tmp351)!= 4)goto _LL1F4;_tmp352=((struct Cyc_Absyn_PointerType_struct*)
_tmp351)->f1;_tmp353=(void*)_tmp352.elt_typ;_LL1F3: Cyc_Tcutil_unify(_tmp353,t);
if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;
goto _LL1F1;_LL1F4:;_LL1F5: goto _LL1F1;_LL1F1:;}({struct Cyc_String_pa_struct
_tmp357;_tmp357.tag=0;_tmp357.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_String_pa_struct _tmp356;_tmp356.tag=0;_tmp356.f1=(struct _tagged_arr)(
allow_zero?({const char*_tmp358="calloc";_tag_arr(_tmp358,sizeof(char),
_get_zero_arr_size(_tmp358,7));}):({const char*_tmp359="malloc";_tag_arr(_tmp359,
sizeof(char),_get_zero_arr_size(_tmp359,7));}));{void*_tmp354[2]={& _tmp356,&
_tmp357};Cyc_Tcutil_terr(loc,({const char*_tmp355="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_arr(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,60));}),_tag_arr(
_tmp354,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp35F=
_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp360;_tmp360.tag=15;_tmp360.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp361;}));_tmp360;});_tmp35F;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp35A=Cyc_Tcutil_compress(handle_type);void*_tmp35B;
_LL1F7: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 15)goto _LL1F9;_tmp35B=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp35A)->f1;_LL1F8: rgn=_tmp35B;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1F6;_LL1F9:;_LL1FA:({struct Cyc_String_pa_struct _tmp35E;
_tmp35E.tag=0;_tmp35E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{
void*_tmp35C[1]={& _tmp35E};Cyc_Tcutil_terr(ropt->loc,({const char*_tmp35D="expecting region_t type but found %s";
_tag_arr(_tmp35D,sizeof(char),_get_zero_arr_size(_tmp35D,37));}),_tag_arr(
_tmp35C,sizeof(void*),1));}});goto _LL1F6;_LL1F6:;}Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,*((struct Cyc_Absyn_Exp**)e));{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(is_calloc){if(*((void***)t)== 0)({void*_tmp362[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp363="calloc with empty type";_tag_arr(_tmp363,sizeof(char),
_get_zero_arr_size(_tmp363,23));}),_tag_arr(_tmp362,sizeof(void*),0));});
elt_type=*((void**)_check_null(*((void***)t)));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*((struct Cyc_Absyn_Exp**)e);one_elt=0;}else{void*_tmp364=(void*)(*((
struct Cyc_Absyn_Exp**)e))->r;void*_tmp365;void*_tmp366;struct Cyc_List_List*
_tmp367;struct Cyc_List_List _tmp368;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_List_List*
_tmp36A;struct Cyc_List_List _tmp36B;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_List_List*
_tmp36D;_LL1FC: if(*((int*)_tmp364)!= 16)goto _LL1FE;_tmp365=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp364)->f1;_LL1FD: elt_type=_tmp365;*((void***)t)=(void**)({void**_tmp36E=
_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=elt_type;_tmp36E;});num_elts=Cyc_Absyn_uint_exp(
1,0);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto
_LL1FB;_LL1FE: if(*((int*)_tmp364)!= 3)goto _LL200;_tmp366=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp364)->f1;if((int)_tmp366 != 1)goto _LL200;_tmp367=((struct Cyc_Absyn_Primop_e_struct*)
_tmp364)->f2;if(_tmp367 == 0)goto _LL200;_tmp368=*_tmp367;_tmp369=(struct Cyc_Absyn_Exp*)
_tmp368.hd;_tmp36A=_tmp368.tl;if(_tmp36A == 0)goto _LL200;_tmp36B=*_tmp36A;_tmp36C=(
struct Cyc_Absyn_Exp*)_tmp36B.hd;_tmp36D=_tmp36B.tl;if(_tmp36D != 0)goto _LL200;
_LL1FF:{struct _tuple0 _tmp370=({struct _tuple0 _tmp36F;_tmp36F.f1=(void*)_tmp369->r;
_tmp36F.f2=(void*)_tmp36C->r;_tmp36F;});void*_tmp371;void*_tmp372;void*_tmp373;
void*_tmp374;_LL203: _tmp371=_tmp370.f1;if(*((int*)_tmp371)!= 16)goto _LL205;
_tmp372=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp371)->f1;_LL204: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp372);elt_type=_tmp372;*((void***)t)=(void**)({void**_tmp375=
_cycalloc(sizeof(*_tmp375));_tmp375[0]=elt_type;_tmp375;});num_elts=_tmp36C;
one_elt=0;goto _LL202;_LL205: _tmp373=_tmp370.f2;if(*((int*)_tmp373)!= 16)goto
_LL207;_tmp374=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp373)->f1;_LL206:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp374);elt_type=_tmp374;*((void***)t)=(
void**)({void**_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376[0]=elt_type;_tmp376;});
num_elts=_tmp369;one_elt=0;goto _LL202;_LL207:;_LL208: goto No_sizeof;_LL202:;}goto
_LL1FB;_LL200:;_LL201: No_sizeof: elt_type=Cyc_Absyn_char_typ;*((void***)t)=(void**)({
void**_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377[0]=elt_type;_tmp377;});
num_elts=*((struct Cyc_Absyn_Exp**)e);one_elt=0;goto _LL1FB;_LL1FB:;}*((struct Cyc_Absyn_Exp**)
e)=num_elts;*((int*)is_fat)=!one_elt;{void*_tmp378=elt_type;struct Cyc_Absyn_AggrInfo
_tmp379;void*_tmp37A;struct Cyc_Absyn_Aggrdecl**_tmp37B;struct Cyc_Absyn_Aggrdecl*
_tmp37C;_LL20A: if(_tmp378 <= (void*)3?1:*((int*)_tmp378)!= 10)goto _LL20C;_tmp379=((
struct Cyc_Absyn_AggrType_struct*)_tmp378)->f1;_tmp37A=(void*)_tmp379.aggr_info;
if(*((int*)_tmp37A)!= 1)goto _LL20C;_tmp37B=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp37A)->f1;_tmp37C=*_tmp37B;_LL20B: if(_tmp37C->impl != 0?((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp37C->impl))->exist_vars != 0: 0)({void*_tmp37D[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp37E="malloc with existential types not yet implemented";
_tag_arr(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,50));}),_tag_arr(
_tmp37D,sizeof(void*),0));});goto _LL209;_LL20C:;_LL20D: goto _LL209;_LL209:;}{void*(*
_tmp37F)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp380=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp381=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo
_tmp382;struct Cyc_Absyn_Conref*_tmp383;struct Cyc_Absyn_Conref*_tmp384;_LL20F: if(
_tmp381 <= (void*)3?1:*((int*)_tmp381)!= 4)goto _LL211;_tmp382=((struct Cyc_Absyn_PointerType_struct*)
_tmp381)->f1;_tmp383=_tmp382.nullable;_tmp384=_tmp382.zero_term;_LL210: _tmp380=
_tmp384;if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp383))
_tmp37F=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp384)?!is_calloc: 0)({void*_tmp385[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp386="must allocate zero-terminated array with calloc";_tag_arr(_tmp386,
sizeof(char),_get_zero_arr_size(_tmp386,48));}),_tag_arr(_tmp385,sizeof(void*),0));});
goto _LL20E;_LL211:;_LL212: goto _LL20E;_LL20E:;}if(!one_elt)_tmp37F=Cyc_Absyn_tagged_typ;
return _tmp37F(elt_type,rgn,Cyc_Absyn_empty_tqual(),_tmp380);}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(te,s),s,1);Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp387=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp388;struct Cyc_Absyn_Stmt*_tmp389;struct Cyc_Absyn_Stmt*_tmp38A;struct Cyc_Absyn_Decl*
_tmp38B;struct Cyc_Absyn_Stmt*_tmp38C;_LL214: if(_tmp387 <= (void*)1?1:*((int*)
_tmp387)!= 0)goto _LL216;_tmp388=((struct Cyc_Absyn_Exp_s_struct*)_tmp387)->f1;
_LL215: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp388->topt))->v;_LL216:
if(_tmp387 <= (void*)1?1:*((int*)_tmp387)!= 1)goto _LL218;_tmp389=((struct Cyc_Absyn_Seq_s_struct*)
_tmp387)->f1;_tmp38A=((struct Cyc_Absyn_Seq_s_struct*)_tmp387)->f2;_LL217: s=
_tmp38A;continue;_LL218: if(_tmp387 <= (void*)1?1:*((int*)_tmp387)!= 12)goto _LL21A;
_tmp38B=((struct Cyc_Absyn_Decl_s_struct*)_tmp387)->f1;_tmp38C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp387)->f2;_LL219: s=_tmp38C;continue;_LL21A:;_LL21B: return({void*_tmp38D[0]={};
Cyc_Tcexp_expr_err(te,loc,({const char*_tmp38E="statement expression must end with expression";
_tag_arr(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,46));}),_tag_arr(
_tmp38D,sizeof(void*),0));});_LL213:;}}static void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Fndecl*fd){return({
void*_tmp38F[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp390="tcCodegen";
_tag_arr(_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,10));}),_tag_arr(
_tmp38F,sizeof(void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*
_tmp391[0]={};Cyc_Tcexp_expr_err(te,loc,({const char*_tmp392="tcFill";_tag_arr(
_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,7));}),_tag_arr(_tmp391,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp399;_tmp399.tag=15;_tmp399.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp39A;}));_tmp399;});_tmp398;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp393=Cyc_Tcutil_compress(handle_type);void*
_tmp394;_LL21D: if(_tmp393 <= (void*)3?1:*((int*)_tmp393)!= 15)goto _LL21F;_tmp394=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp393)->f1;_LL21E: rgn=_tmp394;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL21C;_LL21F:;_LL220:({struct Cyc_String_pa_struct
_tmp397;_tmp397.tag=0;_tmp397.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type);{void*_tmp395[1]={& _tmp397};Cyc_Tcutil_terr(rgn_handle->loc,({const
char*_tmp396="expecting region_t type but found %s";_tag_arr(_tmp396,sizeof(char),
_get_zero_arr_size(_tmp396,37));}),_tag_arr(_tmp395,sizeof(void*),1));}});goto
_LL21C;_LL21C:;}{void*_tmp39B=(void*)e1->r;struct Cyc_Core_Opt*_tmp39C;struct Cyc_List_List*
_tmp39D;struct Cyc_List_List*_tmp39E;void*_tmp39F;struct _tagged_arr _tmp3A0;_LL222:
if(*((int*)_tmp39B)!= 27)goto _LL224;_LL223: {void*_tmp3A1=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp3A2=Cyc_Tcutil_compress(_tmp3A1);struct Cyc_Absyn_ArrayInfo
_tmp3A3;void*_tmp3A4;struct Cyc_Absyn_Tqual _tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;
struct Cyc_Absyn_Conref*_tmp3A7;_LL22D: if(_tmp3A2 <= (void*)3?1:*((int*)_tmp3A2)!= 
7)goto _LL22F;_tmp3A3=((struct Cyc_Absyn_ArrayType_struct*)_tmp3A2)->f1;_tmp3A4=(
void*)_tmp3A3.elt_type;_tmp3A5=_tmp3A3.tq;_tmp3A6=_tmp3A3.num_elts;_tmp3A7=
_tmp3A3.zero_term;_LL22E: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp3A6);void*b=Cyc_Tcutil_is_const_exp(te,bnd)?(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3AD;_tmp3AD.tag=0;_tmp3AD.f1=bnd;_tmp3AD;});_tmp3AC;}):(void*)0;void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));
_tmp3A9[0]=({struct Cyc_Absyn_PointerType_struct _tmp3AA;_tmp3AA.tag=4;_tmp3AA.f1=({
struct Cyc_Absyn_PtrInfo _tmp3AB;_tmp3AB.elt_typ=(void*)_tmp3A4;_tmp3AB.rgn_typ=(
void*)rgn;_tmp3AB.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3AB.tq=_tmp3A5;_tmp3AB.bounds=Cyc_Absyn_new_conref(b);_tmp3AB.zero_term=
_tmp3A7;_tmp3AB;});_tmp3AA;});_tmp3A9;});if(topt != 0){if(!Cyc_Tcutil_unify(*((
void**)topt),res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*((void**)topt)):
0){e->topt=({struct Cyc_Core_Opt*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->v=(
void*)res_typ;_tmp3A8;});Cyc_Tcutil_unchecked_cast(te,e,*((void**)topt));res_typ=*((
void**)topt);}}return res_typ;}_LL22F:;_LL230:({void*_tmp3AE[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3AF="tcNew: comprehension returned non-array type";
_tag_arr(_tmp3AF,sizeof(char),_get_zero_arr_size(_tmp3AF,45));}),_tag_arr(
_tmp3AE,sizeof(void*),0));});_LL22C:;}_LL224: if(*((int*)_tmp39B)!= 34)goto _LL226;
_tmp39C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp39B)->f1;_tmp39D=((struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp39B)->f2;_LL225:(void*)(e1->r=(void*)((
void*)({struct Cyc_Absyn_Array_e_struct*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));
_tmp3B0[0]=({struct Cyc_Absyn_Array_e_struct _tmp3B1;_tmp3B1.tag=26;_tmp3B1.f1=
_tmp39D;_tmp3B1;});_tmp3B0;})));_tmp39E=_tmp39D;goto _LL227;_LL226: if(*((int*)
_tmp39B)!= 26)goto _LL228;_tmp39E=((struct Cyc_Absyn_Array_e_struct*)_tmp39B)->f1;
_LL227: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp3B2=Cyc_Tcutil_compress(*((
void**)topt));struct Cyc_Absyn_PtrInfo _tmp3B3;void*_tmp3B4;void**_tmp3B5;struct
Cyc_Absyn_Tqual _tmp3B6;struct Cyc_Absyn_Conref*_tmp3B7;_LL232: if(_tmp3B2 <= (void*)
3?1:*((int*)_tmp3B2)!= 4)goto _LL234;_tmp3B3=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B2)->f1;_tmp3B4=(void*)_tmp3B3.elt_typ;_tmp3B5=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3B2)->f1).elt_typ;_tmp3B6=_tmp3B3.tq;_tmp3B7=_tmp3B3.zero_term;_LL233:
elt_typ_opt=(void**)_tmp3B5;zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3B7);goto _LL231;_LL234:;_LL235: goto _LL231;_LL231:;}{void*_tmp3B8=Cyc_Tcexp_tcArray(
te,e1->loc,elt_typ_opt,zero_term,_tmp39E);e1->topt=({struct Cyc_Core_Opt*_tmp3B9=
_cycalloc(sizeof(*_tmp3B9));_tmp3B9->v=(void*)_tmp3B8;_tmp3B9;});{void*res_typ;{
void*_tmp3BA=Cyc_Tcutil_compress(_tmp3B8);struct Cyc_Absyn_ArrayInfo _tmp3BB;void*
_tmp3BC;struct Cyc_Absyn_Tqual _tmp3BD;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Conref*
_tmp3BF;_LL237: if(_tmp3BA <= (void*)3?1:*((int*)_tmp3BA)!= 7)goto _LL239;_tmp3BB=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3BA)->f1;_tmp3BC=(void*)_tmp3BB.elt_type;
_tmp3BD=_tmp3BB.tq;_tmp3BE=_tmp3BB.num_elts;_tmp3BF=_tmp3BB.zero_term;_LL238:
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3C0=_cycalloc(sizeof(*
_tmp3C0));_tmp3C0[0]=({struct Cyc_Absyn_PointerType_struct _tmp3C1;_tmp3C1.tag=4;
_tmp3C1.f1=({struct Cyc_Absyn_PtrInfo _tmp3C2;_tmp3C2.elt_typ=(void*)_tmp3BC;
_tmp3C2.rgn_typ=(void*)rgn;_tmp3C2.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3C2.tq=_tmp3BD;_tmp3C2.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3C4;_tmp3C4.tag=0;_tmp3C4.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3BE);
_tmp3C4;});_tmp3C3;}));_tmp3C2.zero_term=_tmp3BF;_tmp3C2;});_tmp3C1;});_tmp3C0;});
if(topt != 0){if(!Cyc_Tcutil_unify(*((void**)topt),res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*((void**)topt)): 0){e->topt=({struct Cyc_Core_Opt*_tmp3C5=
_cycalloc(sizeof(*_tmp3C5));_tmp3C5->v=(void*)res_typ;_tmp3C5;});Cyc_Tcutil_unchecked_cast(
te,e,*((void**)topt));res_typ=*((void**)topt);}}goto _LL236;_LL239:;_LL23A:({void*
_tmp3C6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp3C7="tcExpNoPromote on Array_e returned non-array type";_tag_arr(
_tmp3C7,sizeof(char),_get_zero_arr_size(_tmp3C7,50));}),_tag_arr(_tmp3C6,sizeof(
void*),0));});_LL236:;}return res_typ;}}}_LL228: if(*((int*)_tmp39B)!= 0)goto
_LL22A;_tmp39F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp39B)->f1;if(_tmp39F
<= (void*)1?1:*((int*)_tmp39F)!= 5)goto _LL22A;_tmp3A0=((struct Cyc_Absyn_String_c_struct*)
_tmp39F)->f1;_LL229: {void*_tmp3C8=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(
void*)0,Cyc_Absyn_true_conref);void*_tmp3C9=Cyc_Tcexp_tcExp(te,(void**)& _tmp3C8,
e1);return Cyc_Absyn_atb_typ(_tmp3C9,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp3CB;_tmp3CB.tag=0;_tmp3CB.f1=Cyc_Absyn_uint_exp(1,0);
_tmp3CB;});_tmp3CA;}),Cyc_Absyn_false_conref);}_LL22A:;_LL22B: {void**topt2=0;
if(topt != 0){void*_tmp3CC=Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_PtrInfo
_tmp3CD;void*_tmp3CE;void**_tmp3CF;struct Cyc_Absyn_Conref*_tmp3D0;_LL23C: if(
_tmp3CC <= (void*)3?1:*((int*)_tmp3CC)!= 4)goto _LL23E;_tmp3CD=((struct Cyc_Absyn_PointerType_struct*)
_tmp3CC)->f1;_tmp3CE=(void*)_tmp3CD.elt_typ;_tmp3CF=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp3CC)->f1).elt_typ;_tmp3D0=_tmp3CD.nullable;_LL23D: topt2=(void**)_tmp3CF;goto
_LL23B;_LL23E: if(_tmp3CC <= (void*)3?1:*((int*)_tmp3CC)!= 2)goto _LL240;_LL23F:
topt2=({void**_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=*((void**)topt);
_tmp3D1;});goto _LL23B;_LL240:;_LL241: goto _LL23B;_LL23B:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp3D3=
_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_Absyn_PointerType_struct
_tmp3D4;_tmp3D4.tag=4;_tmp3D4.f1=({struct Cyc_Absyn_PtrInfo _tmp3D5;_tmp3D5.elt_typ=(
void*)telt;_tmp3D5.rgn_typ=(void*)rgn;_tmp3D5.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp3D5.tq=Cyc_Absyn_empty_tqual();_tmp3D5.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));
_tmp3D6[0]=({struct Cyc_Absyn_Upper_b_struct _tmp3D7;_tmp3D7.tag=0;_tmp3D7.f1=Cyc_Absyn_uint_exp(
1,0);_tmp3D7;});_tmp3D6;}));_tmp3D5.zero_term=Cyc_Absyn_false_conref;_tmp3D5;});
_tmp3D4;});_tmp3D3;});if(topt != 0){if(!Cyc_Tcutil_unify(*((void**)topt),res_typ)?
Cyc_Tcutil_silent_castable(te,loc,res_typ,*((void**)topt)): 0){e->topt=({struct
Cyc_Core_Opt*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->v=(void*)res_typ;
_tmp3D2;});Cyc_Tcutil_unchecked_cast(te,e,*((void**)topt));res_typ=*((void**)
topt);}}return res_typ;}}_LL221:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp3D8=t;struct Cyc_Absyn_ArrayInfo _tmp3D9;void*_tmp3DA;struct
Cyc_Absyn_Tqual _tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Conref*
_tmp3DD;_LL243: if(_tmp3D8 <= (void*)3?1:*((int*)_tmp3D8)!= 7)goto _LL245;_tmp3D9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3D8)->f1;_tmp3DA=(void*)_tmp3D9.elt_type;
_tmp3DB=_tmp3D9.tq;_tmp3DC=_tmp3D9.num_elts;_tmp3DD=_tmp3D9.zero_term;_LL244: {
void*_tmp3DF;struct _tuple6 _tmp3DE=Cyc_Tcutil_addressof_props(te,e);_tmp3DF=
_tmp3DE.f2;{void*_tmp3E0=_tmp3DC == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3E2;_tmp3E2.tag=0;_tmp3E2.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3DC);
_tmp3E2;});_tmp3E1;});t=Cyc_Absyn_atb_typ(_tmp3DA,_tmp3DF,_tmp3DB,_tmp3E0,
_tmp3DD);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return
t;}}_LL245:;_LL246: return t;_LL242:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
void*_tmp3E3=(void*)e->r;void*_tmp3E4;_LL248: if(*((int*)_tmp3E3)!= 26)goto _LL24A;
_LL249: goto _LL24B;_LL24A: if(*((int*)_tmp3E3)!= 27)goto _LL24C;_LL24B: goto _LL24D;
_LL24C: if(*((int*)_tmp3E3)!= 0)goto _LL24E;_tmp3E4=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp3E3)->f1;if(_tmp3E4 <= (void*)1?1:*((int*)_tmp3E4)!= 5)goto _LL24E;_LL24D:
return t;_LL24E:;_LL24F: t=Cyc_Tcutil_compress(t);{void*_tmp3E5=t;struct Cyc_Absyn_ArrayInfo
_tmp3E6;void*_tmp3E7;struct Cyc_Absyn_Tqual _tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;
struct Cyc_Absyn_Conref*_tmp3EA;_LL251: if(_tmp3E5 <= (void*)3?1:*((int*)_tmp3E5)!= 
7)goto _LL253;_tmp3E6=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E5)->f1;_tmp3E7=(
void*)_tmp3E6.elt_type;_tmp3E8=_tmp3E6.tq;_tmp3E9=_tmp3E6.num_elts;_tmp3EA=
_tmp3E6.zero_term;_LL252: {void*_tmp3EC;struct _tuple6 _tmp3EB=Cyc_Tcutil_addressof_props(
te,e);_tmp3EC=_tmp3EB.f2;{void*b=_tmp3E9 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp3EE;_tmp3EE.tag=0;_tmp3EE.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3E9);
_tmp3EE;});_tmp3ED;});t=Cyc_Absyn_atb_typ(_tmp3E7,_tmp3EC,_tmp3E8,b,_tmp3EA);Cyc_Tcutil_unchecked_cast(
te,e,t);return t;}}_LL253:;_LL254: return t;_LL250:;}_LL247:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp3EF=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp3F0;_LL256: if(*((int*)_tmp3EF)!= 11)goto _LL258;_tmp3F0=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp3EF)->f1;_LL257: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp3F0);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v));
e->topt=_tmp3F0->topt;goto _LL255;_LL258:;_LL259: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp3F1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3F2;void*_tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Conref*_tmp3F5;struct Cyc_Absyn_Tqual
_tmp3F6;struct Cyc_Absyn_Conref*_tmp3F7;struct Cyc_Absyn_Conref*_tmp3F8;_LL25B: if(
_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 4)goto _LL25D;_tmp3F2=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F1)->f1;_tmp3F3=(void*)_tmp3F2.elt_typ;_tmp3F4=(void*)_tmp3F2.rgn_typ;
_tmp3F5=_tmp3F2.nullable;_tmp3F6=_tmp3F2.tq;_tmp3F7=_tmp3F2.bounds;_tmp3F8=
_tmp3F2.zero_term;_LL25C:{void*_tmp3F9=Cyc_Tcutil_compress(_tmp3F3);struct Cyc_Absyn_FnInfo
_tmp3FA;struct Cyc_List_List*_tmp3FB;struct Cyc_Core_Opt*_tmp3FC;void*_tmp3FD;
struct Cyc_List_List*_tmp3FE;int _tmp3FF;struct Cyc_Absyn_VarargInfo*_tmp400;struct
Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp402;_LL260: if(_tmp3F9 <= (void*)3?1:*((
int*)_tmp3F9)!= 8)goto _LL262;_tmp3FA=((struct Cyc_Absyn_FnType_struct*)_tmp3F9)->f1;
_tmp3FB=_tmp3FA.tvars;_tmp3FC=_tmp3FA.effect;_tmp3FD=(void*)_tmp3FA.ret_typ;
_tmp3FE=_tmp3FA.args;_tmp3FF=_tmp3FA.c_varargs;_tmp400=_tmp3FA.cyc_varargs;
_tmp401=_tmp3FA.rgn_po;_tmp402=_tmp3FA.attributes;_LL261: if(_tmp3FB != 0){struct
_RegionHandle _tmp403=_new_region("rgn");struct _RegionHandle*rgn=& _tmp403;
_push_region(rgn);{struct _tuple4 _tmp404=({struct _tuple4 _tmp40E;_tmp40E.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp40E.f2=rgn;_tmp40E;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp404,_tmp3FB);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_FnType_struct
_tmp40C;_tmp40C.tag=8;_tmp40C.f1=({struct Cyc_Absyn_FnInfo _tmp40D;_tmp40D.tvars=0;
_tmp40D.effect=_tmp3FC;_tmp40D.ret_typ=(void*)_tmp3FD;_tmp40D.args=_tmp3FE;
_tmp40D.c_varargs=_tmp3FF;_tmp40D.cyc_varargs=_tmp400;_tmp40D.rgn_po=_tmp401;
_tmp40D.attributes=_tmp402;_tmp40D;});_tmp40C;});_tmp40B;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=({
struct Cyc_Absyn_PointerType_struct _tmp409;_tmp409.tag=4;_tmp409.f1=({struct Cyc_Absyn_PtrInfo
_tmp40A;_tmp40A.elt_typ=(void*)ftyp;_tmp40A.rgn_typ=(void*)_tmp3F4;_tmp40A.nullable=
_tmp3F5;_tmp40A.tq=_tmp3F6;_tmp40A.bounds=_tmp3F7;_tmp40A.zero_term=_tmp3F8;
_tmp40A;});_tmp409;});_tmp408;});struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp405=
_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp406;_tmp406.tag=12;_tmp406.f1=inner;_tmp406.f2=ts;_tmp406;});_tmp405;})));e->topt=({
struct Cyc_Core_Opt*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->v=(void*)new_typ;
_tmp407;});};_pop_region(rgn);}goto _LL25F;_LL262:;_LL263: goto _LL25F;_LL25F:;}
goto _LL25A;_LL25D:;_LL25E: goto _LL25A;_LL25A:;}goto _LL255;_LL255:;}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*
loc=e->loc;void*t;{void*_tmp40F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp410;struct
_tuple1*_tmp411;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_List_List*_tmp413;struct
Cyc_Core_Opt*_tmp414;struct Cyc_List_List*_tmp415;void*_tmp416;struct _tuple1*
_tmp417;void*_tmp418;void*_tmp419;struct Cyc_List_List*_tmp41A;struct Cyc_Absyn_Exp*
_tmp41B;void*_tmp41C;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Core_Opt*_tmp41E;
struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Exp*_tmp420;struct Cyc_Absyn_Exp*
_tmp421;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*
_tmp424;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_List_List*_tmp426;struct Cyc_Absyn_VarargCallInfo*
_tmp427;struct Cyc_Absyn_VarargCallInfo**_tmp428;struct Cyc_Absyn_Exp*_tmp429;
struct Cyc_Absyn_Exp*_tmp42A;struct Cyc_List_List*_tmp42B;void*_tmp42C;struct Cyc_Absyn_Exp*
_tmp42D;struct Cyc_Absyn_Exp*_tmp42E;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_Absyn_Exp*
_tmp430;struct Cyc_Absyn_Exp*_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;
struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Exp*_tmp436;struct _tagged_arr*
_tmp437;struct Cyc_Absyn_Exp*_tmp438;struct _tagged_arr*_tmp439;struct Cyc_Absyn_Exp*
_tmp43A;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_List_List*_tmp43C;struct _tuple2*
_tmp43D;struct Cyc_List_List*_tmp43E;struct Cyc_List_List*_tmp43F;struct Cyc_Absyn_Stmt*
_tmp440;struct Cyc_Absyn_Fndecl*_tmp441;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Vardecl*
_tmp443;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_Absyn_Exp*_tmp445;int _tmp446;int*
_tmp447;struct _tuple1*_tmp448;struct _tuple1**_tmp449;struct Cyc_List_List*_tmp44A;
struct Cyc_List_List**_tmp44B;struct Cyc_List_List*_tmp44C;struct Cyc_Absyn_Aggrdecl*
_tmp44D;struct Cyc_Absyn_Aggrdecl**_tmp44E;void*_tmp44F;struct Cyc_List_List*
_tmp450;struct Cyc_List_List*_tmp451;struct Cyc_Absyn_Tuniondecl*_tmp452;struct Cyc_Absyn_Tunionfield*
_tmp453;struct _tuple1*_tmp454;struct _tuple1**_tmp455;struct Cyc_Absyn_Enumdecl*
_tmp456;struct Cyc_Absyn_Enumfield*_tmp457;struct _tuple1*_tmp458;struct _tuple1**
_tmp459;void*_tmp45A;struct Cyc_Absyn_Enumfield*_tmp45B;struct Cyc_Absyn_MallocInfo
_tmp45C;int _tmp45D;struct Cyc_Absyn_Exp*_tmp45E;void**_tmp45F;void***_tmp460;
struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Exp**_tmp462;int _tmp463;int*_tmp464;
_LL265: if(*((int*)_tmp40F)!= 11)goto _LL267;_tmp410=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp40F)->f1;_LL266: Cyc_Tcexp_tcExpNoInst(te,0,_tmp410);return;_LL267: if(*((int*)
_tmp40F)!= 2)goto _LL269;_tmp411=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp40F)->f1;
_LL268: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp411);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL269: if(*((int*)_tmp40F)!= 8)goto _LL26B;_tmp412=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp40F)->f1;_tmp413=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp40F)->f2;_LL26A:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp412,_tmp413);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL26B: if(*((int*)_tmp40F)!= 34)goto _LL26D;_tmp414=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp40F)->f1;_tmp415=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp40F)->f2;
_LL26C: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp415);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL26D: if(*((int*)_tmp40F)!= 0)goto _LL26F;_tmp416=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp40F)->f1;_LL26E: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp416,e);goto _LL264;_LL26F: if(*((int*)_tmp40F)!= 1)goto _LL271;_tmp417=((
struct Cyc_Absyn_Var_e_struct*)_tmp40F)->f1;_tmp418=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp40F)->f2;_LL270: t=Cyc_Tcexp_tcVar(te,loc,_tmp417,_tmp418);goto _LL264;_LL271:
if(*((int*)_tmp40F)!= 3)goto _LL273;_tmp419=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp40F)->f1;_tmp41A=((struct Cyc_Absyn_Primop_e_struct*)_tmp40F)->f2;_LL272: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp419,_tmp41A);goto _LL264;_LL273: if(*((int*)
_tmp40F)!= 5)goto _LL275;_tmp41B=((struct Cyc_Absyn_Increment_e_struct*)_tmp40F)->f1;
_tmp41C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp40F)->f2;_LL274: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp41B,_tmp41C);goto _LL264;_LL275: if(*((int*)_tmp40F)!= 4)goto
_LL277;_tmp41D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp40F)->f1;_tmp41E=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp40F)->f2;_tmp41F=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp40F)->f3;_LL276: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp41D,_tmp41E,_tmp41F);
goto _LL264;_LL277: if(*((int*)_tmp40F)!= 6)goto _LL279;_tmp420=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp40F)->f1;_tmp421=((struct Cyc_Absyn_Conditional_e_struct*)_tmp40F)->f2;
_tmp422=((struct Cyc_Absyn_Conditional_e_struct*)_tmp40F)->f3;_LL278: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp420,_tmp421,_tmp422);goto _LL264;_LL279: if(*((int*)_tmp40F)!= 7)
goto _LL27B;_tmp423=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp40F)->f1;_tmp424=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp40F)->f2;_LL27A: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp423,_tmp424);goto _LL264;_LL27B: if(*((int*)_tmp40F)!= 9)goto _LL27D;
_tmp425=((struct Cyc_Absyn_FnCall_e_struct*)_tmp40F)->f1;_tmp426=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp40F)->f2;_tmp427=((struct Cyc_Absyn_FnCall_e_struct*)_tmp40F)->f3;_tmp428=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp40F)->f3;
_LL27C: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp425,_tmp426,_tmp428);goto _LL264;
_LL27D: if(*((int*)_tmp40F)!= 10)goto _LL27F;_tmp429=((struct Cyc_Absyn_Throw_e_struct*)
_tmp40F)->f1;_LL27E: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp429);goto _LL264;_LL27F:
if(*((int*)_tmp40F)!= 12)goto _LL281;_tmp42A=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp40F)->f1;_tmp42B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp40F)->f2;_LL280:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp42A,_tmp42B);goto _LL264;_LL281: if(*((
int*)_tmp40F)!= 13)goto _LL283;_tmp42C=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp40F)->f1;_tmp42D=((struct Cyc_Absyn_Cast_e_struct*)_tmp40F)->f2;_LL282: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp42C,_tmp42D);goto _LL264;_LL283: if(*((int*)_tmp40F)!= 14)goto
_LL285;_tmp42E=((struct Cyc_Absyn_Address_e_struct*)_tmp40F)->f1;_LL284: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp42E);goto _LL264;_LL285: if(*((int*)_tmp40F)!= 15)goto _LL287;
_tmp42F=((struct Cyc_Absyn_New_e_struct*)_tmp40F)->f1;_tmp430=((struct Cyc_Absyn_New_e_struct*)
_tmp40F)->f2;_LL286: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp42F,e,_tmp430);goto _LL264;
_LL287: if(*((int*)_tmp40F)!= 17)goto _LL289;_tmp431=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp40F)->f1;_LL288: {void*_tmp465=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp431);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp465);goto _LL264;}_LL289: if(*((int*)_tmp40F)!= 16)goto _LL28B;
_tmp432=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp40F)->f1;_LL28A: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp432);goto _LL264;_LL28B: if(*((int*)_tmp40F)!= 18)goto _LL28D;
_tmp433=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp40F)->f1;_tmp434=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp40F)->f2;_LL28C: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp433,_tmp434);goto _LL264;_LL28D: if(*((int*)_tmp40F)!= 19)goto _LL28F;
_LL28E:({void*_tmp466[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp467="gen() not in top-level initializer";
_tag_arr(_tmp467,sizeof(char),_get_zero_arr_size(_tmp467,35));}),_tag_arr(
_tmp466,sizeof(void*),0));});return;_LL28F: if(*((int*)_tmp40F)!= 20)goto _LL291;
_tmp435=((struct Cyc_Absyn_Deref_e_struct*)_tmp40F)->f1;_LL290: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp435);goto _LL264;_LL291: if(*((int*)_tmp40F)!= 21)goto _LL293;
_tmp436=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp40F)->f1;_tmp437=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp40F)->f2;_LL292: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp436,_tmp437);goto
_LL264;_LL293: if(*((int*)_tmp40F)!= 22)goto _LL295;_tmp438=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp40F)->f1;_tmp439=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp40F)->f2;_LL294: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp438,_tmp439);goto _LL264;_LL295: if(*((int*)
_tmp40F)!= 23)goto _LL297;_tmp43A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp40F)->f1;
_tmp43B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp40F)->f2;_LL296: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp43A,_tmp43B);goto _LL264;_LL297: if(*((int*)_tmp40F)!= 24)goto
_LL299;_tmp43C=((struct Cyc_Absyn_Tuple_e_struct*)_tmp40F)->f1;_LL298: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp43C);goto _LL264;_LL299: if(*((int*)_tmp40F)!= 25)goto _LL29B;
_tmp43D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp40F)->f1;_tmp43E=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp40F)->f2;_LL29A: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp43D,_tmp43E);goto
_LL264;_LL29B: if(*((int*)_tmp40F)!= 26)goto _LL29D;_tmp43F=((struct Cyc_Absyn_Array_e_struct*)
_tmp40F)->f1;_LL29C: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp468=
Cyc_Tcutil_compress(*((void**)topt));struct Cyc_Absyn_ArrayInfo _tmp469;void*
_tmp46A;void**_tmp46B;struct Cyc_Absyn_Conref*_tmp46C;_LL2B2: if(_tmp468 <= (void*)
3?1:*((int*)_tmp468)!= 7)goto _LL2B4;_tmp469=((struct Cyc_Absyn_ArrayType_struct*)
_tmp468)->f1;_tmp46A=(void*)_tmp469.elt_type;_tmp46B=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp468)->f1).elt_type;_tmp46C=_tmp469.zero_term;_LL2B3: elt_topt=(void**)_tmp46B;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp46C);
goto _LL2B1;_LL2B4:;_LL2B5: goto _LL2B1;_LL2B1:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp43F);goto _LL264;}_LL29D: if(*((int*)_tmp40F)!= 35)goto
_LL29F;_tmp440=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp40F)->f1;_LL29E: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp440);goto _LL264;_LL29F: if(*((int*)_tmp40F)!= 36)goto _LL2A1;
_tmp441=((struct Cyc_Absyn_Codegen_e_struct*)_tmp40F)->f1;_LL2A0: t=Cyc_Tcexp_tcCodegen(
te,loc,topt,_tmp441);goto _LL264;_LL2A1: if(*((int*)_tmp40F)!= 37)goto _LL2A3;
_tmp442=((struct Cyc_Absyn_Fill_e_struct*)_tmp40F)->f1;_LL2A2: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp442);goto _LL264;_LL2A3: if(*((int*)_tmp40F)!= 27)goto _LL2A5;
_tmp443=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp40F)->f1;_tmp444=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp40F)->f2;_tmp445=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp40F)->f3;_tmp446=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp40F)->f4;
_tmp447=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp40F)->f4;_LL2A4: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp443,_tmp444,_tmp445,_tmp447);goto
_LL264;_LL2A5: if(*((int*)_tmp40F)!= 28)goto _LL2A7;_tmp448=((struct Cyc_Absyn_Struct_e_struct*)
_tmp40F)->f1;_tmp449=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp40F)->f1;_tmp44A=((struct Cyc_Absyn_Struct_e_struct*)_tmp40F)->f2;_tmp44B=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp40F)->f2;_tmp44C=((
struct Cyc_Absyn_Struct_e_struct*)_tmp40F)->f3;_tmp44D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp40F)->f4;_tmp44E=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp40F)->f4;_LL2A6: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp449,_tmp44B,_tmp44C,
_tmp44E);goto _LL264;_LL2A7: if(*((int*)_tmp40F)!= 29)goto _LL2A9;_tmp44F=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp40F)->f1;_tmp450=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp40F)->f2;_LL2A8: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp44F,_tmp450);goto _LL264;
_LL2A9: if(*((int*)_tmp40F)!= 30)goto _LL2AB;_tmp451=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp40F)->f1;_tmp452=((struct Cyc_Absyn_Tunion_e_struct*)_tmp40F)->f2;_tmp453=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp40F)->f3;_LL2AA: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp451,_tmp452,_tmp453);goto _LL264;_LL2AB: if(*((int*)_tmp40F)!= 31)goto
_LL2AD;_tmp454=((struct Cyc_Absyn_Enum_e_struct*)_tmp40F)->f1;_tmp455=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp40F)->f1;_tmp456=((struct Cyc_Absyn_Enum_e_struct*)
_tmp40F)->f2;_tmp457=((struct Cyc_Absyn_Enum_e_struct*)_tmp40F)->f3;_LL2AC:*((
struct _tuple1**)_tmp455)=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp457))->name;
t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));
_tmp46D[0]=({struct Cyc_Absyn_EnumType_struct _tmp46E;_tmp46E.tag=12;_tmp46E.f1=((
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp456))->name;_tmp46E.f2=_tmp456;_tmp46E;});
_tmp46D;});goto _LL264;_LL2AD: if(*((int*)_tmp40F)!= 32)goto _LL2AF;_tmp458=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp40F)->f1;_tmp459=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp40F)->f1;_tmp45A=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp40F)->f2;_tmp45B=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp40F)->f3;_LL2AE:*((
struct _tuple1**)_tmp459)=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp45B))->name;
t=_tmp45A;goto _LL264;_LL2AF: if(*((int*)_tmp40F)!= 33)goto _LL264;_tmp45C=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp40F)->f1;_tmp45D=_tmp45C.is_calloc;_tmp45E=
_tmp45C.rgn;_tmp45F=_tmp45C.elt_type;_tmp460=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp40F)->f1).elt_type;_tmp461=_tmp45C.num_elts;_tmp462=(struct Cyc_Absyn_Exp**)&(((
struct Cyc_Absyn_Malloc_e_struct*)_tmp40F)->f1).num_elts;_tmp463=_tmp45C.fat_result;
_tmp464=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp40F)->f1).fat_result;
_LL2B0: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp45E,_tmp460,_tmp462,_tmp45D,_tmp464);
goto _LL264;_LL264:;}e->topt=({struct Cyc_Core_Opt*_tmp46F=_cycalloc(sizeof(*
_tmp46F));_tmp46F->v=(void*)t;_tmp46F;});}
