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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_memq(struct Cyc_List_List*
l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2);extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;
};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int
tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct _tagged_arr*Cyc_Absyn_fieldname(int);int Cyc_Absyn_is_union_type(void*);
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);
int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{unsigned int f1;int f2;};struct
_tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
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
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*
encloser;int visited;void*flow;};static char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot\000";
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
_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,37));});_tmp7;});_tmp6;}));
_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int iterate_again;int iteration_num;int
in_try;void*tryflow;struct Cyc_Set_Set**all_changed;int noreturn;struct Cyc_List_List*
param_roots;};static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Decl*);static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(s);(void*)(
_tmp9->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(void*)_tmp9->flow));
_tmp9->visited=env->iteration_num;return _tmp9;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,void*new_flow){if(env->in_try)(void*)(env->tryflow=(
void*)Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,(void*)env->tryflow));}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(
s);void*_tmpB=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,(void*)_tmpA->flow);
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmpB,(void*)_tmpA->flow)){(void*)(_tmpA->flow=(
void*)_tmpB);if(_tmpA->visited == env->iteration_num)env->iterate_again=1;}}
static void*Cyc_NewControlFlow_add_vars(void*inflow,struct Cyc_List_List*vds,void*
leafval){void*_tmpC=inflow;struct Cyc_Dict_Dict*_tmpD;struct Cyc_List_List*_tmpE;
_LL6: if((int)_tmpC != 0)goto _LL8;_LL7: return(void*)0;_LL8: if(_tmpC <= (void*)1?1:*((
int*)_tmpC)!= 0)goto _LL5;_tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f1;
_tmpE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f2;_LL9: for(0;vds != 0;
vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmpF=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp11;_tmp11.tag=0;_tmp11.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp11;});_tmp10;});
_tmpD=Cyc_Dict_insert(_tmpD,(void*)_tmpF,Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=_tmpD;_tmp13.f2=_tmpE;_tmp13;});_tmp12;});_LL5:;}
static void*Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Position_Segment*loc,void*inflow,void*r){void*_tmp14=inflow;struct Cyc_Dict_Dict*
_tmp15;struct Cyc_List_List*_tmp16;_LLB: if((int)_tmp14 != 0)goto _LLD;_LLC: return(
void*)0;_LLD: if(_tmp14 <= (void*)1?1:*((int*)_tmp14)!= 0)goto _LLA;_tmp15=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14)->f1;_tmp16=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp14)->f2;_LLE: if(Cyc_CfFlowInfo_initlevel(_tmp15,r)!= (void*)2)({void*_tmp17[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp18="expression may not be fully initialized";
_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,40));}),_tag_arr(_tmp17,
sizeof(void*),0));});{struct Cyc_Dict_Dict*_tmp19=Cyc_CfFlowInfo_escape_deref(
_tmp15,env->all_changed,r);if(_tmp15 == _tmp19)return inflow;{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=
_tmp19;_tmp1C.f2=_tmp16;_tmp1C;});_tmp1B;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp1A);return(void*)_tmp1A;}}_LLA:;}struct _tuple5{void*f1;struct Cyc_List_List*
f2;};static struct _tuple5 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0)return({struct _tuple5 _tmp1D;
_tmp1D.f1=inflow;_tmp1D.f2=0;_tmp1D;});if(es->tl == 0){void*_tmp1F;void*_tmp20;
struct _tuple0 _tmp1E=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp1F=_tmp1E.f1;_tmp20=_tmp1E.f2;return({struct _tuple5 _tmp21;_tmp21.f1=
_tmp1F;_tmp21.f2=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=(
void*)_tmp20;_tmp22->tl=0;_tmp22;});_tmp21;});}{struct Cyc_Set_Set**
outer_all_changed=env->all_changed;struct Cyc_Set_Set*this_all_changed;void*
old_inflow;void*outflow;struct Cyc_List_List*rvals;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();
env->all_changed=({struct Cyc_Set_Set**_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=
Cyc_CfFlowInfo_mt_place_set();_tmp23;});{void*_tmp25;void*_tmp26;struct _tuple0
_tmp24=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)es->hd);
_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;outflow=_tmp25;rvals=({struct Cyc_List_List*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=(void*)_tmp26;_tmp27->tl=0;_tmp27;});
this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Set_Set**_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp28;});{void*_tmp2A;void*_tmp2B;struct _tuple0 _tmp29=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp2A=_tmp29.f1;_tmp2B=_tmp29.f2;
rvals=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=(void*)
_tmp2B;_tmp2C->tl=rvals;_tmp2C;});outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)&
this_all_changed,outflow,_tmp2A,this_all_changed,*((struct Cyc_Set_Set**)
_check_null(env->all_changed)));this_all_changed=((struct Cyc_Set_Set*(*)(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(this_all_changed,*((
struct Cyc_Set_Set**)_check_null(env->all_changed)));}}}old_inflow=inflow;inflow=
Cyc_CfFlowInfo_join_flow(outer_all_changed,inflow,outflow);}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
inflow,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Set_Set**_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*outer_all_changed,
this_all_changed);_tmp2D;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple5 _tmp2E;_tmp2E.f1=outflow;_tmp2E.f2=Cyc_List_imp_rev(rvals);
_tmp2E;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){void*_tmp30;struct Cyc_List_List*_tmp31;
struct _tuple5 _tmp2F=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);
_tmp30=_tmp2F.f1;_tmp31=_tmp2F.f2;{void*_tmp32=_tmp30;struct Cyc_Dict_Dict*_tmp33;
struct Cyc_List_List*_tmp34;_LL10: if(_tmp32 <= (void*)1?1:*((int*)_tmp32)!= 0)goto
_LL12;_tmp33=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp32)->f1;_tmp34=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp32)->f2;_LL11: for(0;_tmp31 != 0;(
_tmp31=_tmp31->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp33,(void*)_tmp31->hd)== (void*)0)({void*_tmp35[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp36="expression may not be initialized";_tag_arr(_tmp36,sizeof(char),
_get_zero_arr_size(_tmp36,34));}),_tag_arr(_tmp35,sizeof(void*),0));});}goto _LLF;
_LL12: if((int)_tmp32 != 0)goto _LLF;_LL13: goto _LLF;_LLF:;}return({struct _tuple0
_tmp37;_tmp37.f1=_tmp30;_tmp37.f2=Cyc_CfFlowInfo_unknown_all;_tmp37;});}static
void*Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp38=outflow;struct Cyc_Dict_Dict*
_tmp39;struct Cyc_List_List*_tmp3A;_LL15: if((int)_tmp38 != 0)goto _LL17;_LL16:
return outflow;_LL17: if(_tmp38 <= (void*)1?1:*((int*)_tmp38)!= 0)goto _LL14;_tmp39=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp38)->f1;_tmp3A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp38)->f2;_LL18: {void*_tmp3B=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;
struct Cyc_CfFlowInfo_Place*_tmp3C;_LL1A: if((int)_tmp3B != 0)goto _LL1C;_LL1B:
return outflow;_LL1C: if(_tmp3B <= (void*)1?1:*((int*)_tmp3B)!= 0)goto _LL19;_tmp3C=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp3B)->f1;_LL1D: {void*nzval=il == (void*)2?(
void*)1:(void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp39,env->all_changed,
_tmp3C,nzval);_tmp3E.f2=_tmp3A;_tmp3E;});_tmp3D;});}_LL19:;}_LL14:;}static struct
_tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp3F=outflow;struct Cyc_Dict_Dict*
_tmp40;struct Cyc_List_List*_tmp41;_LL1F: if((int)_tmp3F != 0)goto _LL21;_LL20:
return({struct _tuple0 _tmp42;_tmp42.f1=outflow;_tmp42.f2=outflow;_tmp42;});_LL21:
if(_tmp3F <= (void*)1?1:*((int*)_tmp3F)!= 0)goto _LL1E;_tmp40=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3F)->f1;_tmp41=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3F)->f2;_LL22: {
void*_tmp43=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp44;_LL24: if((int)_tmp43 != 0)goto _LL26;_LL25: return({struct _tuple0 _tmp45;
_tmp45.f1=outflow;_tmp45.f2=outflow;_tmp45;});_LL26: if(_tmp43 <= (void*)1?1:*((
int*)_tmp43)!= 0)goto _LL23;_tmp44=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp43)->f1;
_LL27: {void*nzval=il == (void*)2?(void*)1:(void*)2;return({struct _tuple0 _tmp46;
_tmp46.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp49=_cycalloc(
sizeof(*_tmp49));_tmp49[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4A;
_tmp4A.tag=0;_tmp4A.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp40,env->all_changed,
_tmp44,nzval);_tmp4A.f2=_tmp41;_tmp4A;});_tmp49;});_tmp46.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp48;_tmp48.tag=0;_tmp48.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp40,env->all_changed,
_tmp44,(void*)0);_tmp48.f2=_tmp41;_tmp48;});_tmp47;});_tmp46;});}_LL23:;}_LL1E:;}
static struct _tuple0 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp4B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp4C;void*_tmp4D;struct Cyc_Absyn_PtrAtts _tmp4E;struct Cyc_Absyn_Conref*_tmp4F;
struct Cyc_Absyn_Conref*_tmp50;_LL29: if(_tmp4B <= (void*)3?1:*((int*)_tmp4B)!= 4)
goto _LL2B;_tmp4C=((struct Cyc_Absyn_PointerType_struct*)_tmp4B)->f1;_tmp4D=(void*)
_tmp4C.elt_typ;_tmp4E=_tmp4C.ptr_atts;_tmp4F=_tmp4E.bounds;_tmp50=_tmp4E.zero_term;
_LL2A: {void*_tmp51=f;struct Cyc_Dict_Dict*_tmp52;struct Cyc_List_List*_tmp53;
_LL2E: if((int)_tmp51 != 0)goto _LL30;_LL2F: return({struct _tuple0 _tmp54;_tmp54.f1=f;
_tmp54.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp4D,Cyc_CfFlowInfo_unknown_all);
_tmp54;});_LL30: if(_tmp51 <= (void*)1?1:*((int*)_tmp51)!= 0)goto _LL2D;_tmp52=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp51)->f1;_tmp53=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp51)->f2;_LL31: if(Cyc_Tcutil_is_bound_one(_tmp4F)){void*_tmp55=r;struct Cyc_CfFlowInfo_Place*
_tmp56;void*_tmp57;_LL33: if((int)_tmp55 != 1)goto _LL35;_LL34: goto _LL36;_LL35: if((
int)_tmp55 != 2)goto _LL37;_LL36:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp59;_tmp59.tag=Cyc_CfFlowInfo_NotZero;_tmp59.f1=_tmp53;_tmp59;});_tmp58;})));
goto _LL32;_LL37: if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 2)goto _LL39;_tmp56=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp55)->f1;_LL38:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));
_tmp5A[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp5B;_tmp5B.tag=Cyc_CfFlowInfo_NotZero;
_tmp5B.f1=_tmp53;_tmp5B;});_tmp5A;})));return({struct _tuple0 _tmp5C;_tmp5C.f1=f;
_tmp5C.f2=Cyc_CfFlowInfo_lookup_place(_tmp52,_tmp56);_tmp5C;});_LL39: if((int)
_tmp55 != 0)goto _LL3B;_LL3A:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp5E;_tmp5E.tag=Cyc_CfFlowInfo_IsZero;_tmp5E.f1=_tmp53;_tmp5E;});_tmp5D;})));
return({struct _tuple0 _tmp5F;_tmp5F.f1=(void*)0;_tmp5F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4D,Cyc_CfFlowInfo_unknown_all);_tmp5F;});_LL3B: if(_tmp55 <= (void*)3?1:*((int*)
_tmp55)!= 0)goto _LL3D;_tmp57=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp55)->f1;_LL3C: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp57);goto _LL3E;
_LL3D:;_LL3E:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp61;_tmp61.tag=Cyc_CfFlowInfo_UnknownZ;_tmp61.f1=_tmp53;_tmp61;});_tmp60;})));
_LL32:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp63;_tmp63.tag=Cyc_CfFlowInfo_UnknownZ;_tmp63.f1=_tmp53;_tmp63;});_tmp62;})));}{
void*_tmp64=Cyc_CfFlowInfo_initlevel(_tmp52,r);_LL40: if((int)_tmp64 != 0)goto
_LL42;_LL41:({void*_tmp65[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp66="dereference of possibly uninitialized pointer";
_tag_arr(_tmp66,sizeof(char),_get_zero_arr_size(_tmp66,46));}),_tag_arr(_tmp65,
sizeof(void*),0));});goto _LL43;_LL42: if((int)_tmp64 != 2)goto _LL44;_LL43: return({
struct _tuple0 _tmp67;_tmp67.f1=f;_tmp67.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp4D,
Cyc_CfFlowInfo_unknown_all);_tmp67;});_LL44: if((int)_tmp64 != 1)goto _LL3F;_LL45:
return({struct _tuple0 _tmp68;_tmp68.f1=f;_tmp68.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4D,Cyc_CfFlowInfo_unknown_none);_tmp68;});_LL3F:;}_LL2D:;}_LL2B:;_LL2C:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp69=_cycalloc(sizeof(*_tmp69));
_tmp69[0]=({struct Cyc_Core_Impossible_struct _tmp6A;_tmp6A.tag=Cyc_Core_Impossible;
_tmp6A.f1=({const char*_tmp6B="right deref of non-pointer-type";_tag_arr(_tmp6B,
sizeof(char),_get_zero_arr_size(_tmp6B,32));});_tmp6A;});_tmp69;}));_LL28:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp6C=(void*)e1->r;
void*_tmp6D;struct Cyc_Absyn_Vardecl*_tmp6E;void*_tmp6F;struct Cyc_Absyn_Vardecl*
_tmp70;void*_tmp71;struct Cyc_Absyn_Vardecl*_tmp72;void*_tmp73;struct Cyc_Absyn_Vardecl*
_tmp74;_LL47: if(*((int*)_tmp6C)!= 1)goto _LL49;_tmp6D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp6C)->f2;if(_tmp6D <= (void*)1?1:*((int*)_tmp6D)!= 4)goto _LL49;_tmp6E=((struct
Cyc_Absyn_Pat_b_struct*)_tmp6D)->f1;_LL48: _tmp70=_tmp6E;goto _LL4A;_LL49: if(*((
int*)_tmp6C)!= 1)goto _LL4B;_tmp6F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6C)->f2;
if(_tmp6F <= (void*)1?1:*((int*)_tmp6F)!= 3)goto _LL4B;_tmp70=((struct Cyc_Absyn_Local_b_struct*)
_tmp6F)->f1;_LL4A: _tmp72=_tmp70;goto _LL4C;_LL4B: if(*((int*)_tmp6C)!= 1)goto _LL4D;
_tmp71=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6C)->f2;if(_tmp71 <= (void*)1?1:*((
int*)_tmp71)!= 2)goto _LL4D;_tmp72=((struct Cyc_Absyn_Param_b_struct*)_tmp71)->f1;
_LL4C: _tmp74=_tmp72;goto _LL4E;_LL4D: if(*((int*)_tmp6C)!= 1)goto _LL4F;_tmp73=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6C)->f2;if(_tmp73 <= (void*)1?1:*((int*)
_tmp73)!= 0)goto _LL4F;_tmp74=((struct Cyc_Absyn_Global_b_struct*)_tmp73)->f1;
_LL4E: if(!_tmp74->escapes){void*_tmp75=(void*)e2->r;void*_tmp76;struct Cyc_Absyn_Vardecl*
_tmp77;void*_tmp78;struct Cyc_Absyn_Vardecl*_tmp79;void*_tmp7A;struct Cyc_Absyn_Vardecl*
_tmp7B;void*_tmp7C;struct Cyc_Absyn_Vardecl*_tmp7D;_LL52: if(*((int*)_tmp75)!= 1)
goto _LL54;_tmp76=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp75)->f2;if(_tmp76 <= (
void*)1?1:*((int*)_tmp76)!= 4)goto _LL54;_tmp77=((struct Cyc_Absyn_Pat_b_struct*)
_tmp76)->f1;_LL53: _tmp79=_tmp77;goto _LL55;_LL54: if(*((int*)_tmp75)!= 1)goto _LL56;
_tmp78=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp75)->f2;if(_tmp78 <= (void*)1?1:*((
int*)_tmp78)!= 3)goto _LL56;_tmp79=((struct Cyc_Absyn_Local_b_struct*)_tmp78)->f1;
_LL55: _tmp7B=_tmp79;goto _LL57;_LL56: if(*((int*)_tmp75)!= 1)goto _LL58;_tmp7A=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp75)->f2;if(_tmp7A <= (void*)1?1:*((int*)
_tmp7A)!= 2)goto _LL58;_tmp7B=((struct Cyc_Absyn_Param_b_struct*)_tmp7A)->f1;_LL57:
_tmp7D=_tmp7B;goto _LL59;_LL58: if(*((int*)_tmp75)!= 1)goto _LL5A;_tmp7C=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp75)->f2;if(_tmp7C <= (void*)1?1:*((int*)_tmp7C)
!= 0)goto _LL5A;_tmp7D=((struct Cyc_Absyn_Global_b_struct*)_tmp7C)->f1;_LL59: if(!
_tmp7D->escapes){int found=0;{struct Cyc_List_List*_tmp7E=relns;for(0;_tmp7E != 0;
_tmp7E=_tmp7E->tl){struct Cyc_CfFlowInfo_Reln*_tmp7F=(struct Cyc_CfFlowInfo_Reln*)
_tmp7E->hd;if(_tmp7F->vd == _tmp7D){void*_tmp80=(void*)_tmp7F->rop;struct Cyc_Absyn_Vardecl*
_tmp81;_LL5D: if(*((int*)_tmp80)!= 2)goto _LL5F;_tmp81=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp80)->f1;if(!(_tmp81 == _tmp74))goto _LL5F;_LL5E: return relns;_LL5F:;_LL60:
continue;_LL5C:;}}}if(!found)return({struct Cyc_List_List*_tmp82=_cycalloc(
sizeof(*_tmp82));_tmp82->hd=({struct Cyc_CfFlowInfo_Reln*_tmp83=_cycalloc(sizeof(*
_tmp83));_tmp83->vd=_tmp7D;_tmp83->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp85;_tmp85.tag=2;_tmp85.f1=_tmp74;_tmp85;});_tmp84;}));_tmp83;});_tmp82->tl=
relns;_tmp82;});}return relns;_LL5A:;_LL5B: return relns;_LL51:;}else{return relns;}
_LL4F:;_LL50: return relns;_LL46:;}struct _tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;{void*_tmp86=inflow;struct Cyc_Dict_Dict*_tmp87;struct Cyc_List_List*_tmp88;
_LL62: if((int)_tmp86 != 0)goto _LL64;_LL63: return({struct _tuple0 _tmp89;_tmp89.f1=(
void*)0;_tmp89.f2=Cyc_CfFlowInfo_unknown_all;_tmp89;});_LL64: if(_tmp86 <= (void*)
1?1:*((int*)_tmp86)!= 0)goto _LL61;_tmp87=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp86)->f1;_tmp88=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp86)->f2;_LL65:
d=_tmp87;relns=_tmp88;_LL61:;}{void*_tmp8A=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;void*_tmp8E;void*
_tmp8F;void*_tmp90;int _tmp91;void*_tmp92;void*_tmp93;void*_tmp94;struct Cyc_List_List*
_tmp95;void*_tmp96;void*_tmp97;struct Cyc_Absyn_Vardecl*_tmp98;void*_tmp99;struct
Cyc_Absyn_Vardecl*_tmp9A;void*_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9C;struct Cyc_List_List*
_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Core_Opt*
_tmpA0;struct Cyc_Core_Opt _tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*
_tmpA3;struct Cyc_Core_Opt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*
_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*
_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_MallocInfo _tmpAB;int _tmpAC;
struct Cyc_Absyn_Exp*_tmpAD;void**_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;int _tmpB0;
struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;
struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct _tagged_arr*_tmpB6;
struct Cyc_Absyn_Exp*_tmpB7;struct _tagged_arr*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;
struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpBF;
struct Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC2;
struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*
_tmpC5;int _tmpC6;struct Cyc_Absyn_Stmt*_tmpC7;void*_tmpC8;_LL67: if(*((int*)_tmp8A)
!= 13)goto _LL69;_tmp8B=((struct Cyc_Absyn_Cast_e_struct*)_tmp8A)->f2;_LL68: _tmp8C=
_tmp8B;goto _LL6A;_LL69: if(*((int*)_tmp8A)!= 11)goto _LL6B;_tmp8C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp8A)->f1;_LL6A: _tmp8D=_tmp8C;goto _LL6C;_LL6B: if(*((int*)_tmp8A)!= 12)goto
_LL6D;_tmp8D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp8A)->f1;_LL6C: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp8D);_LL6D: if(*((int*)_tmp8A)!= 0)goto _LL6F;_tmp8E=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp8A)->f1;if((int)_tmp8E != 0)goto _LL6F;_LL6E: goto
_LL70;_LL6F: if(*((int*)_tmp8A)!= 0)goto _LL71;_tmp8F=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8A)->f1;if(_tmp8F <= (void*)1?1:*((int*)_tmp8F)!= 2)goto _LL71;_tmp90=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp8F)->f1;if((int)_tmp90 != 0)goto _LL71;_tmp91=((
struct Cyc_Absyn_Int_c_struct*)_tmp8F)->f2;if(_tmp91 != 0)goto _LL71;_LL70: return({
struct _tuple0 _tmpC9;_tmpC9.f1=inflow;_tmpC9.f2=(void*)0;_tmpC9;});_LL71: if(*((
int*)_tmp8A)!= 0)goto _LL73;_tmp92=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8A)->f1;if(_tmp92 <= (void*)1?1:*((int*)_tmp92)!= 2)goto _LL73;_tmp93=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp92)->f1;if((int)_tmp93 != 0)goto _LL73;_LL72:
goto _LL74;_LL73: if(*((int*)_tmp8A)!= 1)goto _LL75;_tmp94=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp8A)->f2;if(_tmp94 <= (void*)1?1:*((int*)_tmp94)!= 1)goto _LL75;_LL74: return({
struct _tuple0 _tmpCA;_tmpCA.f1=inflow;_tmpCA.f2=(void*)1;_tmpCA;});_LL75: if(*((
int*)_tmp8A)!= 30)goto _LL77;_tmp95=((struct Cyc_Absyn_Tunion_e_struct*)_tmp8A)->f1;
if(_tmp95 != 0)goto _LL77;_LL76: goto _LL78;_LL77: if(*((int*)_tmp8A)!= 0)goto _LL79;
_LL78: goto _LL7A;_LL79: if(*((int*)_tmp8A)!= 17)goto _LL7B;_LL7A: goto _LL7C;_LL7B:
if(*((int*)_tmp8A)!= 16)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(*((int*)_tmp8A)!= 18)
goto _LL7F;_LL7E: goto _LL80;_LL7F: if(*((int*)_tmp8A)!= 19)goto _LL81;_LL80: goto
_LL82;_LL81: if(*((int*)_tmp8A)!= 32)goto _LL83;_LL82: goto _LL84;_LL83: if(*((int*)
_tmp8A)!= 31)goto _LL85;_LL84: return({struct _tuple0 _tmpCB;_tmpCB.f1=inflow;_tmpCB.f2=
Cyc_CfFlowInfo_unknown_all;_tmpCB;});_LL85: if(*((int*)_tmp8A)!= 1)goto _LL87;
_tmp96=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp8A)->f2;if(_tmp96 <= (void*)1?1:*((
int*)_tmp96)!= 0)goto _LL87;_LL86: return({struct _tuple0 _tmpCC;_tmpCC.f1=inflow;
_tmpCC.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmpCC;});_LL87: if(*((int*)_tmp8A)!= 1)
goto _LL89;_tmp97=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp8A)->f2;if(_tmp97 <= (
void*)1?1:*((int*)_tmp97)!= 2)goto _LL89;_tmp98=((struct Cyc_Absyn_Param_b_struct*)
_tmp97)->f1;_LL88: _tmp9A=_tmp98;goto _LL8A;_LL89: if(*((int*)_tmp8A)!= 1)goto _LL8B;
_tmp99=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp8A)->f2;if(_tmp99 <= (void*)1?1:*((
int*)_tmp99)!= 3)goto _LL8B;_tmp9A=((struct Cyc_Absyn_Local_b_struct*)_tmp99)->f1;
_LL8A: _tmp9C=_tmp9A;goto _LL8C;_LL8B: if(*((int*)_tmp8A)!= 1)goto _LL8D;_tmp9B=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp8A)->f2;if(_tmp9B <= (void*)1?1:*((int*)
_tmp9B)!= 4)goto _LL8D;_tmp9C=((struct Cyc_Absyn_Pat_b_struct*)_tmp9B)->f1;_LL8C:
return({struct _tuple0 _tmpCD;_tmpCD.f1=inflow;_tmpCD.f2=Cyc_Dict_lookup(d,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _tmpCF;_tmpCF.tag=0;_tmpCF.f1=_tmp9C;_tmpCF;});
_tmpCE;}));_tmpCD;});_LL8D: if(*((int*)_tmp8A)!= 3)goto _LL8F;_tmp9D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp8A)->f2;_LL8E: return Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp9D);
_LL8F: if(*((int*)_tmp8A)!= 5)goto _LL91;_tmp9E=((struct Cyc_Absyn_Increment_e_struct*)
_tmp8A)->f1;_LL90: {void*_tmpD2;struct _tuple0 _tmpD1=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpD0[1];_tmpD0[0]=_tmp9E;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpD0,sizeof(struct Cyc_Absyn_Exp*),1));}));
_tmpD2=_tmpD1.f1;{void*_tmpD4;struct _tuple0 _tmpD3=Cyc_NewControlFlow_anal_Lexp(
env,_tmpD2,_tmp9E);_tmpD4=_tmpD3.f2;{struct _tuple0 _tmpD6=({struct _tuple0 _tmpD5;
_tmpD5.f1=_tmpD4;_tmpD5.f2=_tmpD2;_tmpD5;});void*_tmpD7;struct Cyc_CfFlowInfo_Place*
_tmpD8;void*_tmpD9;struct Cyc_Dict_Dict*_tmpDA;struct Cyc_List_List*_tmpDB;_LLC8:
_tmpD7=_tmpD6.f1;if(_tmpD7 <= (void*)1?1:*((int*)_tmpD7)!= 0)goto _LLCA;_tmpD8=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpD7)->f1;_tmpD9=_tmpD6.f2;if(_tmpD9 <= (
void*)1?1:*((int*)_tmpD9)!= 0)goto _LLCA;_tmpDA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpD9)->f1;_tmpDB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpD9)->f2;_LLC9:
_tmpDB=Cyc_CfFlowInfo_reln_kill_exp(_tmpDB,_tmp9E);_tmpD2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpDD;_tmpDD.tag=0;_tmpDD.f1=Cyc_CfFlowInfo_assign_place(_tmp9E->loc,_tmpDA,env->all_changed,
_tmpD8,Cyc_CfFlowInfo_unknown_all);_tmpDD.f2=_tmpDB;_tmpDD;});_tmpDC;});goto
_LLC7;_LLCA:;_LLCB: goto _LLC7;_LLC7:;}return({struct _tuple0 _tmpDE;_tmpDE.f1=
_tmpD2;_tmpDE.f2=Cyc_CfFlowInfo_unknown_all;_tmpDE;});}}_LL91: if(*((int*)_tmp8A)
!= 4)goto _LL93;_tmp9F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp8A)->f1;_tmpA0=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp8A)->f2;if(_tmpA0 == 0)goto _LL93;_tmpA1=*
_tmpA0;_tmpA2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp8A)->f3;_LL92: {void*
_tmpE1;struct _tuple0 _tmpE0=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct
Cyc_Absyn_Exp*_tmpDF[2];_tmpDF[1]=_tmpA2;_tmpDF[0]=_tmp9F;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpDF,sizeof(struct Cyc_Absyn_Exp*),2));}));
_tmpE1=_tmpE0.f1;{void*_tmpE3;struct _tuple0 _tmpE2=Cyc_NewControlFlow_anal_Lexp(
env,_tmpE1,e);_tmpE3=_tmpE2.f2;{struct _tuple0 _tmpE5=({struct _tuple0 _tmpE4;_tmpE4.f1=
_tmpE3;_tmpE4.f2=_tmpE1;_tmpE4;});void*_tmpE6;struct Cyc_CfFlowInfo_Place*_tmpE7;
void*_tmpE8;struct Cyc_Dict_Dict*_tmpE9;struct Cyc_List_List*_tmpEA;_LLCD: _tmpE6=
_tmpE5.f1;if(_tmpE6 <= (void*)1?1:*((int*)_tmpE6)!= 0)goto _LLCF;_tmpE7=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmpE6)->f1;_tmpE8=_tmpE5.f2;if(_tmpE8 <= (void*)1?
1:*((int*)_tmpE8)!= 0)goto _LLCF;_tmpE9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpE8)->f1;_tmpEA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpE8)->f2;_LLCE:
_tmpEA=Cyc_CfFlowInfo_reln_kill_exp(_tmpEA,_tmp9F);_tmpE1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpEC;_tmpEC.tag=0;_tmpEC.f1=Cyc_CfFlowInfo_assign_place(_tmp9F->loc,_tmpE9,env->all_changed,
_tmpE7,Cyc_CfFlowInfo_unknown_all);_tmpEC.f2=_tmpEA;_tmpEC;});_tmpEB;});goto
_LLCC;_LLCF:;_LLD0: goto _LLCC;_LLCC:;}return({struct _tuple0 _tmpED;_tmpED.f1=
_tmpE1;_tmpED.f2=Cyc_CfFlowInfo_unknown_all;_tmpED;});}}_LL93: if(*((int*)_tmp8A)
!= 4)goto _LL95;_tmpA3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp8A)->f1;_tmpA4=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp8A)->f2;if(_tmpA4 != 0)goto _LL95;_tmpA5=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp8A)->f3;_LL94: {struct Cyc_Set_Set**_tmpEE=
env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmpEF=_cycalloc(
sizeof(*_tmpEF));_tmpEF[0]=Cyc_CfFlowInfo_mt_place_set();_tmpEF;});{void*_tmpF1;
void*_tmpF2;struct _tuple0 _tmpF0=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmpA3);
_tmpF1=_tmpF0.f1;_tmpF2=_tmpF0.f2;{struct Cyc_Set_Set*_tmpF3=*((struct Cyc_Set_Set**)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Set_Set**_tmpF4=
_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=Cyc_CfFlowInfo_mt_place_set();_tmpF4;});{
void*_tmpF6;void*_tmpF7;struct _tuple0 _tmpF5=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpA5);_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;{struct Cyc_Set_Set*_tmpF8=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmpF9=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmpF3,_tmpF1,_tmpF6,_tmpF3,_tmpF8);void*_tmpFA=Cyc_CfFlowInfo_join_flow(
_tmpEE,inflow,_tmpF9);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmpFA,inflow)){if(
_tmpEE == 0)env->all_changed=0;else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmpEE,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmpF3,_tmpF8));}{void*_tmpFB=_tmpF9;struct Cyc_Dict_Dict*_tmpFC;struct Cyc_List_List*
_tmpFD;_LLD2: if((int)_tmpFB != 0)goto _LLD4;_LLD3: return({struct _tuple0 _tmpFE;
_tmpFE.f1=(void*)0;_tmpFE.f2=_tmpF7;_tmpFE;});_LLD4: if(_tmpFB <= (void*)1?1:*((
int*)_tmpFB)!= 0)goto _LLD1;_tmpFC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFB)->f1;_tmpFD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFB)->f2;_LLD5: {
void*_tmpFF=_tmpF2;struct Cyc_CfFlowInfo_Place*_tmp100;_LLD7: if(_tmpFF <= (void*)1?
1:*((int*)_tmpFF)!= 0)goto _LLD9;_tmp100=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmpFF)->f1;_LLD8: _tmpFC=Cyc_CfFlowInfo_assign_place(e->loc,_tmpFC,env->all_changed,
_tmp100,_tmpF7);_tmpFD=Cyc_CfFlowInfo_reln_assign_exp(_tmpFD,_tmpA3,_tmpA5);
_tmpF9=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp101=_cycalloc(
sizeof(*_tmp101));_tmp101[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp102;
_tmp102.tag=0;_tmp102.f1=_tmpFC;_tmp102.f2=_tmpFD;_tmp102;});_tmp101;});Cyc_NewControlFlow_update_tryflow(
env,_tmpF9);return({struct _tuple0 _tmp103;_tmp103.f1=_tmpF9;_tmp103.f2=_tmpF7;
_tmp103;});_LLD9: if((int)_tmpFF != 0)goto _LLD6;_LLDA: return({struct _tuple0 _tmp104;
_tmp104.f1=Cyc_NewControlFlow_use_Rval(env,_tmpA5->loc,_tmpF9,_tmpF7);_tmp104.f2=
_tmpF7;_tmp104;});_LLD6:;}_LLD1:;}}inflow=_tmpFA;}}}}}}_LL95: if(*((int*)_tmp8A)
!= 7)goto _LL97;_tmpA6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp8A)->f1;_tmpA7=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp8A)->f2;_LL96: return Cyc_NewControlFlow_anal_Rexp(
env,(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA6)).f1,_tmpA7);_LL97: if(*((int*)
_tmp8A)!= 10)goto _LL99;_tmpA8=((struct Cyc_Absyn_Throw_e_struct*)_tmp8A)->f1;
_LL98: {void*_tmp106;void*_tmp107;struct _tuple0 _tmp105=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA8);_tmp106=_tmp105.f1;_tmp107=_tmp105.f2;Cyc_NewControlFlow_use_Rval(
env,_tmpA8->loc,_tmp106,_tmp107);return({struct _tuple0 _tmp108;_tmp108.f1=(void*)
0;_tmp108.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp108;});}_LL99: if(*((int*)
_tmp8A)!= 9)goto _LL9B;_tmpA9=((struct Cyc_Absyn_FnCall_e_struct*)_tmp8A)->f1;
_tmpAA=((struct Cyc_Absyn_FnCall_e_struct*)_tmp8A)->f2;_LL9A: {void*_tmp10B;
struct Cyc_List_List*_tmp10C;struct _tuple5 _tmp10A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->hd=
_tmpA9;_tmp109->tl=_tmpAA;_tmp109;}));_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{
void*_tmp10D=Cyc_NewControlFlow_use_Rval(env,_tmpA9->loc,_tmp10B,(void*)((struct
Cyc_List_List*)_check_null(_tmp10C))->hd);_tmp10C=_tmp10C->tl;{struct Cyc_List_List*
init_params=0;{void*_tmp10E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA9->topt))->v);struct Cyc_Absyn_PtrInfo _tmp10F;void*_tmp110;_LLDC:
if(_tmp10E <= (void*)3?1:*((int*)_tmp10E)!= 4)goto _LLDE;_tmp10F=((struct Cyc_Absyn_PointerType_struct*)
_tmp10E)->f1;_tmp110=(void*)_tmp10F.elt_typ;_LLDD:{void*_tmp111=Cyc_Tcutil_compress(
_tmp110);struct Cyc_Absyn_FnInfo _tmp112;struct Cyc_List_List*_tmp113;_LLE1: if(
_tmp111 <= (void*)3?1:*((int*)_tmp111)!= 8)goto _LLE3;_tmp112=((struct Cyc_Absyn_FnType_struct*)
_tmp111)->f1;_tmp113=_tmp112.attributes;_LLE2: for(0;_tmp113 != 0;_tmp113=_tmp113->tl){
void*_tmp114=(void*)_tmp113->hd;int _tmp115;_LLE6: if(_tmp114 <= (void*)17?1:*((int*)
_tmp114)!= 4)goto _LLE8;_tmp115=((struct Cyc_Absyn_Initializes_att_struct*)_tmp114)->f1;
_LLE7: init_params=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));
_tmp116->hd=(void*)_tmp115;_tmp116->tl=init_params;_tmp116;});goto _LLE5;_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}goto _LLE0;_LLE3:;_LLE4:({void*_tmp117[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp118="anal_Rexp: bad function type";
_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,29));}),_tag_arr(
_tmp117,sizeof(void*),0));});_LLE0:;}goto _LLDB;_LLDE:;_LLDF:({void*_tmp119[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp11A="anal_Rexp: bad function type";_tag_arr(_tmp11A,sizeof(char),
_get_zero_arr_size(_tmp11A,29));}),_tag_arr(_tmp119,sizeof(void*),0));});_LLDB:;}{
int i=1;for(0;_tmp10C != 0;(((_tmp10C=_tmp10C->tl,_tmpAA=((struct Cyc_List_List*)
_check_null(_tmpAA))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp10D=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpAA))->hd)->loc,_tmp10D,(void*)_tmp10C->hd);
continue;}{void*_tmp11B=_tmp10B;struct Cyc_Dict_Dict*_tmp11C;_LLEB: if((int)
_tmp11B != 0)goto _LLED;_LLEC: goto _LLEA;_LLED: if(_tmp11B <= (void*)1?1:*((int*)
_tmp11B)!= 0)goto _LLEA;_tmp11C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11B)->f1;_LLEE: if(Cyc_CfFlowInfo_initlevel(_tmp11C,(void*)_tmp10C->hd)== (
void*)0)({void*_tmp11D[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpAA))->hd)->loc,({const char*_tmp11E="expression may not be initialized";
_tag_arr(_tmp11E,sizeof(char),_get_zero_arr_size(_tmp11E,34));}),_tag_arr(
_tmp11D,sizeof(void*),0));});{void*_tmp11F=_tmp10D;struct Cyc_Dict_Dict*_tmp120;
struct Cyc_List_List*_tmp121;_LLF0: if((int)_tmp11F != 0)goto _LLF2;_LLF1: goto _LLEF;
_LLF2: if(_tmp11F <= (void*)1?1:*((int*)_tmp11F)!= 0)goto _LLEF;_tmp120=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11F)->f1;_tmp121=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11F)->f2;
_LLF3: {struct Cyc_Dict_Dict*_tmp122=Cyc_CfFlowInfo_escape_deref(_tmp120,env->all_changed,(
void*)_tmp10C->hd);{void*_tmp123=(void*)_tmp10C->hd;struct Cyc_CfFlowInfo_Place*
_tmp124;_LLF5: if(_tmp123 <= (void*)3?1:*((int*)_tmp123)!= 2)goto _LLF7;_tmp124=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp123)->f1;_LLF6:{void*_tmp125=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpAA))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp126;void*
_tmp127;_LLFA: if(_tmp125 <= (void*)3?1:*((int*)_tmp125)!= 4)goto _LLFC;_tmp126=((
struct Cyc_Absyn_PointerType_struct*)_tmp125)->f1;_tmp127=(void*)_tmp126.elt_typ;
_LLFB: _tmp122=Cyc_CfFlowInfo_assign_place(((struct Cyc_Absyn_Exp*)_tmpAA->hd)->loc,
_tmp122,env->all_changed,_tmp124,Cyc_CfFlowInfo_typ_to_absrval(_tmp127,Cyc_CfFlowInfo_esc_all));
goto _LLF9;_LLFC:;_LLFD:({void*_tmp128[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp129="anal_Rexp: bad type for initialized arg";
_tag_arr(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,40));}),_tag_arr(
_tmp128,sizeof(void*),0));});_LLF9:;}goto _LLF4;_LLF7:;_LLF8: goto _LLF4;_LLF4:;}
_tmp10D=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp12A=_cycalloc(
sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp12B;
_tmp12B.tag=0;_tmp12B.f1=_tmp122;_tmp12B.f2=_tmp121;_tmp12B;});_tmp12A;});goto
_LLEF;}_LLEF:;}goto _LLEA;_LLEA:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA9->topt))->v))return({struct _tuple0 _tmp12C;_tmp12C.f1=(void*)0;
_tmp12C.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp12C;});else{return({
struct _tuple0 _tmp12D;_tmp12D.f1=_tmp10D;_tmp12D.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp12D;});}}}}_LL9B: if(*((int*)_tmp8A)!= 33)goto _LL9D;_tmpAB=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp8A)->f1;_tmpAC=_tmpAB.is_calloc;_tmpAD=_tmpAB.rgn;_tmpAE=_tmpAB.elt_type;
_tmpAF=_tmpAB.num_elts;_tmpB0=_tmpAB.fat_result;_LL9C: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp13F;_tmp13F.tag=1;_tmp13F.f1=_tmpAF;
_tmp13F.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp13F;});_tmp13E;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->root=(void*)root;_tmp13D->fields=0;
_tmp13D;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp13B=
_cycalloc(sizeof(*_tmp13B));_tmp13B[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp13C;_tmp13C.tag=2;_tmp13C.f1=place;_tmp13C;});_tmp13B;});void*place_val=
_tmpB0?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpAE)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpAD != 0){void*
_tmp130;struct Cyc_List_List*_tmp131;struct _tuple5 _tmp12F=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp12E[2];_tmp12E[1]=_tmpAF;_tmp12E[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpAD);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp12E,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp130=
_tmp12F.f1;_tmp131=_tmp12F.f2;outflow=_tmp130;}else{void*_tmp133;struct _tuple0
_tmp132=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpAF);_tmp133=_tmp132.f1;
outflow=_tmp133;}{void*_tmp134=outflow;struct Cyc_Dict_Dict*_tmp135;struct Cyc_List_List*
_tmp136;_LLFF: if((int)_tmp134 != 0)goto _LL101;_LL100: return({struct _tuple0 _tmp137;
_tmp137.f1=outflow;_tmp137.f2=rval;_tmp137;});_LL101: if(_tmp134 <= (void*)1?1:*((
int*)_tmp134)!= 0)goto _LLFE;_tmp135=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp134)->f1;_tmp136=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp134)->f2;
_LL102: return({struct _tuple0 _tmp138;_tmp138.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp13A;_tmp13A.tag=0;_tmp13A.f1=Cyc_Dict_insert(_tmp135,root,place_val);_tmp13A.f2=
_tmp136;_tmp13A;});_tmp139;});_tmp138.f2=rval;_tmp138;});_LLFE:;}}_LL9D: if(*((
int*)_tmp8A)!= 15)goto _LL9F;_tmpB1=((struct Cyc_Absyn_New_e_struct*)_tmp8A)->f1;
_tmpB2=((struct Cyc_Absyn_New_e_struct*)_tmp8A)->f2;_LL9E: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp152;_tmp152.tag=1;_tmp152.f1=_tmpB2;
_tmp152.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp152;});_tmp151;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->root=(void*)root;_tmp150->fields=0;
_tmp150;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp14E=
_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp14F;_tmp14F.tag=2;_tmp14F.f1=place;_tmp14F;});_tmp14E;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpB1 != 0){
void*_tmp142;struct Cyc_List_List*_tmp143;struct _tuple5 _tmp141=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp140[2];_tmp140[1]=_tmpB2;_tmp140[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpB1);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp140,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp142=
_tmp141.f1;_tmp143=_tmp141.f2;outflow=_tmp142;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp143))->tl))->hd;}else{void*
_tmp145;void*_tmp146;struct _tuple0 _tmp144=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpB2);_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;outflow=_tmp145;place_val=
_tmp146;}{void*_tmp147=outflow;struct Cyc_Dict_Dict*_tmp148;struct Cyc_List_List*
_tmp149;_LL104: if((int)_tmp147 != 0)goto _LL106;_LL105: return({struct _tuple0
_tmp14A;_tmp14A.f1=outflow;_tmp14A.f2=rval;_tmp14A;});_LL106: if(_tmp147 <= (void*)
1?1:*((int*)_tmp147)!= 0)goto _LL103;_tmp148=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp147)->f1;_tmp149=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp147)->f2;
_LL107: return({struct _tuple0 _tmp14B;_tmp14B.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=Cyc_Dict_insert(_tmp148,root,place_val);_tmp14D.f2=
_tmp149;_tmp14D;});_tmp14C;});_tmp14B.f2=rval;_tmp14B;});_LL103:;}}}_LL9F: if(*((
int*)_tmp8A)!= 14)goto _LLA1;_tmpB3=((struct Cyc_Absyn_Address_e_struct*)_tmp8A)->f1;
_LLA0: {void*_tmp154;void*_tmp155;struct _tuple0 _tmp153=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpB3);_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;{void*_tmp156=_tmp155;
struct Cyc_CfFlowInfo_Place*_tmp157;_LL109: if((int)_tmp156 != 0)goto _LL10B;_LL10A:
return({struct _tuple0 _tmp158;_tmp158.f1=_tmp154;_tmp158.f2=(void*)1;_tmp158;});
_LL10B: if(_tmp156 <= (void*)1?1:*((int*)_tmp156)!= 0)goto _LL108;_tmp157=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp156)->f1;_LL10C: return({struct _tuple0 _tmp159;
_tmp159.f1=_tmp154;_tmp159.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp15B;_tmp15B.tag=2;_tmp15B.f1=_tmp157;_tmp15B;});_tmp15A;});_tmp159;});_LL108:;}}
_LLA1: if(*((int*)_tmp8A)!= 20)goto _LLA3;_tmpB4=((struct Cyc_Absyn_Deref_e_struct*)
_tmp8A)->f1;_LLA2: {void*_tmp15D;void*_tmp15E;struct _tuple0 _tmp15C=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB4);_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp15D,_tmpB4,_tmp15E);}_LLA3: if(*((int*)_tmp8A)!= 21)goto _LLA5;
_tmpB5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8A)->f1;_tmpB6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp8A)->f2;_LLA4: {void*_tmp160;void*_tmp161;struct _tuple0 _tmp15F=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB5);_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB5->topt))->v))return({struct _tuple0
_tmp162;_tmp162.f1=_tmp160;_tmp162.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp162;});{
void*_tmp163=_tmp161;struct Cyc_Dict_Dict*_tmp164;_LL10E: if(_tmp163 <= (void*)3?1:*((
int*)_tmp163)!= 3)goto _LL110;_tmp164=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp163)->f1;_LL10F: return({struct _tuple0 _tmp165;_tmp165.f1=_tmp160;_tmp165.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp164,
_tmpB6);_tmp165;});_LL110:;_LL111:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Core_Impossible_struct
_tmp167;_tmp167.tag=Cyc_Core_Impossible;_tmp167.f1=({const char*_tmp168="anal_Rexp: AggrMember";
_tag_arr(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,22));});_tmp167;});
_tmp166;}));_LL10D:;}}_LLA5: if(*((int*)_tmp8A)!= 22)goto _LLA7;_tmpB7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp8A)->f1;_tmpB8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp8A)->f2;_LLA6: {
void*_tmp16A;void*_tmp16B;struct _tuple0 _tmp169=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpB7);_tmp16A=_tmp169.f1;_tmp16B=_tmp169.f2;{void*_tmp16D;void*_tmp16E;
struct _tuple0 _tmp16C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp16A,_tmpB7,
_tmp16B);_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{void*_tmp16F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB7->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp170;void*_tmp171;_LL113: if(_tmp16F <= (void*)3?1:*((int*)_tmp16F)!= 4)goto
_LL115;_tmp170=((struct Cyc_Absyn_PointerType_struct*)_tmp16F)->f1;_tmp171=(void*)
_tmp170.elt_typ;_LL114: if(Cyc_Absyn_is_union_type(_tmp171))return({struct _tuple0
_tmp172;_tmp172.f1=_tmp16D;_tmp172.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp172;});
goto _LL112;_LL115:;_LL116:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173[0]=({struct Cyc_Core_Impossible_struct
_tmp174;_tmp174.tag=Cyc_Core_Impossible;_tmp174.f1=({const char*_tmp175="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,25));});_tmp174;});
_tmp173;}));_LL112:;}{void*_tmp176=_tmp16E;struct Cyc_Dict_Dict*_tmp177;_LL118:
if(_tmp176 <= (void*)3?1:*((int*)_tmp176)!= 3)goto _LL11A;_tmp177=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp176)->f1;_LL119: return({struct _tuple0 _tmp178;_tmp178.f1=_tmp16D;_tmp178.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp177,
_tmpB8);_tmp178;});_LL11A:;_LL11B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Core_Impossible_struct
_tmp17A;_tmp17A.tag=Cyc_Core_Impossible;_tmp17A.f1=({const char*_tmp17B="anal_Rexp: AggrArrow";
_tag_arr(_tmp17B,sizeof(char),_get_zero_arr_size(_tmp17B,21));});_tmp17A;});
_tmp179;}));_LL117:;}}}_LLA7: if(*((int*)_tmp8A)!= 6)goto _LLA9;_tmpB9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8A)->f1;_tmpBA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8A)->f2;_tmpBB=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp8A)->f3;_LLA8: {void*_tmp17D;void*
_tmp17E;struct _tuple0 _tmp17C=Cyc_NewControlFlow_anal_test(env,inflow,_tmpB9);
_tmp17D=_tmp17C.f1;_tmp17E=_tmp17C.f2;{void*_tmp180;void*_tmp181;struct _tuple0
_tmp17F=Cyc_NewControlFlow_anal_Rexp(env,_tmp17D,_tmpBA);_tmp180=_tmp17F.f1;
_tmp181=_tmp17F.f2;{void*_tmp183;void*_tmp184;struct _tuple0 _tmp182=Cyc_NewControlFlow_anal_Rexp(
env,_tmp17E,_tmpBB);_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;return({struct _tuple0
_tmp185;_tmp185.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_use_Rval(
env,_tmpBA->loc,_tmp180,_tmp181),Cyc_NewControlFlow_use_Rval(env,_tmpBB->loc,
_tmp183,_tmp184));_tmp185.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp185;});}}}_LLA9: if(*((
int*)_tmp8A)!= 23)goto _LLAB;_tmpBC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8A)->f1;
_tmpBD=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8A)->f2;_LLAA: {void*_tmp188;
struct Cyc_List_List*_tmp189;struct _tuple5 _tmp187=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp186[2];_tmp186[1]=_tmpBD;_tmp186[0]=_tmpBC;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp186,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;{void*
_tmp18A=_tmp188;{void*_tmp18B=_tmp188;struct Cyc_Dict_Dict*_tmp18C;struct Cyc_List_List*
_tmp18D;_LL11D: if(_tmp18B <= (void*)1?1:*((int*)_tmp18B)!= 0)goto _LL11F;_tmp18C=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp18B)->f1;_tmp18D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp18B)->f2;_LL11E: if(Cyc_CfFlowInfo_initlevel(_tmp18C,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp189))->tl))->hd)== (void*)0)({
void*_tmp18E[0]={};Cyc_Tcutil_terr(_tmpBD->loc,({const char*_tmp18F="expression may not be initialized";
_tag_arr(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,34));}),_tag_arr(
_tmp18E,sizeof(void*),0));});{struct Cyc_List_List*_tmp190=Cyc_NewControlFlow_add_subscript_reln(
_tmp18D,_tmpBC,_tmpBD);if(_tmp18D != _tmp190)_tmp18A=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp192;_tmp192.tag=0;_tmp192.f1=_tmp18C;_tmp192.f2=_tmp190;_tmp192;});_tmp191;});
goto _LL11C;}_LL11F:;_LL120: goto _LL11C;_LL11C:;}{void*_tmp193=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpBC->topt))->v);_LL122: if(_tmp193 <= (
void*)3?1:*((int*)_tmp193)!= 9)goto _LL124;_LL123: {void*_tmp194=(void*)((struct
Cyc_List_List*)_check_null(_tmp189))->hd;struct Cyc_Dict_Dict*_tmp195;_LL127: if(
_tmp194 <= (void*)3?1:*((int*)_tmp194)!= 3)goto _LL129;_tmp195=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp194)->f1;_LL128: return({struct _tuple0 _tmp196;_tmp196.f1=_tmp18A;_tmp196.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp195,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpBD)).f1));_tmp196;});_LL129:;_LL12A:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp197=_cycalloc(sizeof(*
_tmp197));_tmp197[0]=({struct Cyc_Core_Impossible_struct _tmp198;_tmp198.tag=Cyc_Core_Impossible;
_tmp198.f1=({const char*_tmp199="anal_Rexp: Subscript";_tag_arr(_tmp199,sizeof(
char),_get_zero_arr_size(_tmp199,21));});_tmp198;});_tmp197;}));_LL126:;}_LL124:;
_LL125: goto _LL121;_LL121:;}{void*_tmp19B;void*_tmp19C;struct _tuple0 _tmp19A=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp188,_tmpBC,(void*)((struct Cyc_List_List*)_check_null(_tmp189))->hd);
_tmp19B=_tmp19A.f1;_tmp19C=_tmp19A.f2;if(_tmp19B == (void*)0)return({struct
_tuple0 _tmp19D;_tmp19D.f1=_tmp19B;_tmp19D.f2=_tmp19C;_tmp19D;});else{return({
struct _tuple0 _tmp19E;_tmp19E.f1=_tmp18A;_tmp19E.f2=_tmp19C;_tmp19E;});}}}}_LLAB:
if(*((int*)_tmp8A)!= 30)goto _LLAD;_tmpBE=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp8A)->f1;_LLAC: _tmpBF=_tmpBE;goto _LLAE;_LLAD: if(*((int*)_tmp8A)!= 24)goto
_LLAF;_tmpBF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp8A)->f1;_LLAE: {void*_tmp1A0;
struct Cyc_List_List*_tmp1A1;struct _tuple5 _tmp19F=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpBF);_tmp1A0=_tmp19F.f1;_tmp1A1=_tmp19F.f2;{struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp1A1 != 0;(_tmp1A1=
_tmp1A1->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(i),(
void*)_tmp1A1->hd);}}return({struct _tuple0 _tmp1A2;_tmp1A2.f1=_tmp1A0;_tmp1A2.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));
_tmp1A3[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1A4;_tmp1A4.tag=3;_tmp1A4.f1=
aggrdict;_tmp1A4;});_tmp1A3;});_tmp1A2;});}}_LLAF: if(*((int*)_tmp8A)!= 29)goto
_LLB1;_tmpC0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp8A)->f2;_LLB0: _tmpC1=
_tmpC0;goto _LLB2;_LLB1: if(*((int*)_tmp8A)!= 28)goto _LLB3;_tmpC1=((struct Cyc_Absyn_Struct_e_struct*)
_tmp8A)->f3;_LLB2: {void*_tmp1A6;struct Cyc_List_List*_tmp1A7;struct _tuple5
_tmp1A5=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpC1));_tmp1A6=_tmp1A5.f1;
_tmp1A7=_tmp1A5.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int
i=0;for(0;_tmp1A7 != 0;(((_tmp1A7=_tmp1A7->tl,_tmpC1=_tmpC1->tl)),++ i)){struct Cyc_List_List*
ds=(*((struct _tuple6*)((struct Cyc_List_List*)_check_null(_tmpC1))->hd)).f1;for(0;
ds != 0;ds=ds->tl){void*_tmp1A8=(void*)ds->hd;struct _tagged_arr*_tmp1A9;_LL12C:
if(*((int*)_tmp1A8)!= 0)goto _LL12E;_LL12D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=({struct Cyc_Core_Impossible_struct
_tmp1AB;_tmp1AB.tag=Cyc_Core_Impossible;_tmp1AB.f1=({const char*_tmp1AC="anal_Rexp:Struct_e";
_tag_arr(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,19));});_tmp1AB;});
_tmp1AA;}));_LL12E: if(*((int*)_tmp1A8)!= 1)goto _LL12B;_tmp1A9=((struct Cyc_Absyn_FieldName_struct*)
_tmp1A8)->f1;_LL12F: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp1A9,(void*)_tmp1A7->hd);
_LL12B:;}}}return({struct _tuple0 _tmp1AD;_tmp1AD.f1=_tmp1A6;_tmp1AD.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));
_tmp1AE[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1AF;_tmp1AF.tag=3;_tmp1AF.f1=
aggrdict;_tmp1AF;});_tmp1AE;});_tmp1AD;});}}_LLB3: if(*((int*)_tmp8A)!= 26)goto
_LLB5;_tmpC2=((struct Cyc_Absyn_Array_e_struct*)_tmp8A)->f1;_LLB4: {struct Cyc_List_List*
_tmp1B0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple6*))
Cyc_Core_snd,_tmpC2);void*_tmp1B2;struct Cyc_List_List*_tmp1B3;struct _tuple5
_tmp1B1=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp1B0);_tmp1B2=
_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;for(0;_tmp1B3 != 0;(_tmp1B3=_tmp1B3->tl,_tmp1B0=
_tmp1B0->tl)){_tmp1B2=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1B0))->hd)->loc,_tmp1B2,(void*)_tmp1B3->hd);}
return({struct _tuple0 _tmp1B4;_tmp1B4.f1=_tmp1B2;_tmp1B4.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1B4;});}_LLB5: if(*((int*)_tmp8A)!= 27)goto _LLB7;_tmpC3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8A)->f1;_tmpC4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp8A)->f2;_tmpC5=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp8A)->f3;_tmpC6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8A)->f4;_LLB6: {void*_tmp1B6;void*_tmp1B7;struct _tuple0 _tmp1B5=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC4);_tmp1B6=_tmp1B5.f1;_tmp1B7=_tmp1B5.f2;{void*_tmp1B8=_tmp1B6;
struct Cyc_Dict_Dict*_tmp1B9;struct Cyc_List_List*_tmp1BA;_LL131: if((int)_tmp1B8 != 
0)goto _LL133;_LL132: return({struct _tuple0 _tmp1BB;_tmp1BB.f1=_tmp1B6;_tmp1BB.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1BB;});_LL133: if(_tmp1B8 <= (void*)1?1:*((int*)
_tmp1B8)!= 0)goto _LL130;_tmp1B9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1B8)->f1;_tmp1BA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1B8)->f2;
_LL134: if(Cyc_CfFlowInfo_initlevel(_tmp1B9,_tmp1B7)== (void*)0)({void*_tmp1BC[0]={};
Cyc_Tcutil_terr(_tmpC4->loc,({const char*_tmp1BD="expression may not be initialized";
_tag_arr(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,34));}),_tag_arr(
_tmp1BC,sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp1BA;comp_loop: {
void*_tmp1BE=(void*)_tmpC4->r;struct Cyc_Absyn_Exp*_tmp1BF;void*_tmp1C0;struct Cyc_Absyn_Vardecl*
_tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Vardecl*_tmp1C3;void*_tmp1C4;struct Cyc_Absyn_Vardecl*
_tmp1C5;void*_tmp1C6;struct Cyc_Absyn_Vardecl*_tmp1C7;void*_tmp1C8;int _tmp1C9;
void*_tmp1CA;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List _tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CD;_LL136: if(*((int*)_tmp1BE)!= 13)goto _LL138;_tmp1BF=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1BE)->f2;_LL137: _tmpC4=_tmp1BF;goto comp_loop;_LL138: if(*((int*)_tmp1BE)!= 1)
goto _LL13A;_tmp1C0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BE)->f2;if(
_tmp1C0 <= (void*)1?1:*((int*)_tmp1C0)!= 0)goto _LL13A;_tmp1C1=((struct Cyc_Absyn_Global_b_struct*)
_tmp1C0)->f1;if(!(!_tmp1C1->escapes))goto _LL13A;_LL139: _tmp1C3=_tmp1C1;goto
_LL13B;_LL13A: if(*((int*)_tmp1BE)!= 1)goto _LL13C;_tmp1C2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1BE)->f2;if(_tmp1C2 <= (void*)1?1:*((int*)_tmp1C2)!= 3)goto _LL13C;_tmp1C3=((
struct Cyc_Absyn_Local_b_struct*)_tmp1C2)->f1;if(!(!_tmp1C3->escapes))goto _LL13C;
_LL13B: _tmp1C5=_tmp1C3;goto _LL13D;_LL13C: if(*((int*)_tmp1BE)!= 1)goto _LL13E;
_tmp1C4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BE)->f2;if(_tmp1C4 <= (void*)
1?1:*((int*)_tmp1C4)!= 4)goto _LL13E;_tmp1C5=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1C4)->f1;if(!(!_tmp1C5->escapes))goto _LL13E;_LL13D: _tmp1C7=_tmp1C5;goto
_LL13F;_LL13E: if(*((int*)_tmp1BE)!= 1)goto _LL140;_tmp1C6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1BE)->f2;if(_tmp1C6 <= (void*)1?1:*((int*)_tmp1C6)!= 2)goto _LL140;_tmp1C7=((
struct Cyc_Absyn_Param_b_struct*)_tmp1C6)->f1;if(!(!_tmp1C7->escapes))goto _LL140;
_LL13F: new_relns=({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));
_tmp1CE->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));
_tmp1CF->vd=_tmpC3;_tmp1CF->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=_tmp1C7;_tmp1D1;});_tmp1D0;}));_tmp1CF;});
_tmp1CE->tl=_tmp1BA;_tmp1CE;});goto _LL135;_LL140: if(*((int*)_tmp1BE)!= 0)goto
_LL142;_tmp1C8=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1BE)->f1;if(_tmp1C8
<= (void*)1?1:*((int*)_tmp1C8)!= 2)goto _LL142;_tmp1C9=((struct Cyc_Absyn_Int_c_struct*)
_tmp1C8)->f2;_LL141: new_relns=({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*
_tmp1D2));_tmp1D2->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1D3=_cycalloc(sizeof(*
_tmp1D3));_tmp1D3->vd=_tmpC3;_tmp1D3->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1D4=_cycalloc_atomic(sizeof(*_tmp1D4));_tmp1D4[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1D5;_tmp1D5.tag=3;_tmp1D5.f1=(unsigned int)_tmp1C9;_tmp1D5;});_tmp1D4;}));
_tmp1D3;});_tmp1D2->tl=_tmp1BA;_tmp1D2;});goto _LL135;_LL142: if(*((int*)_tmp1BE)
!= 3)goto _LL144;_tmp1CA=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1BE)->f1;
if((int)_tmp1CA != 19)goto _LL144;_tmp1CB=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1BE)->f2;if(_tmp1CB == 0)goto _LL144;_tmp1CC=*_tmp1CB;_tmp1CD=(struct Cyc_Absyn_Exp*)
_tmp1CC.hd;_LL143:{void*_tmp1D6=(void*)_tmp1CD->r;void*_tmp1D7;struct Cyc_Absyn_Vardecl*
_tmp1D8;void*_tmp1D9;struct Cyc_Absyn_Vardecl*_tmp1DA;void*_tmp1DB;struct Cyc_Absyn_Vardecl*
_tmp1DC;void*_tmp1DD;struct Cyc_Absyn_Vardecl*_tmp1DE;_LL147: if(*((int*)_tmp1D6)
!= 1)goto _LL149;_tmp1D7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D6)->f2;if(
_tmp1D7 <= (void*)1?1:*((int*)_tmp1D7)!= 0)goto _LL149;_tmp1D8=((struct Cyc_Absyn_Global_b_struct*)
_tmp1D7)->f1;if(!(!_tmp1D8->escapes))goto _LL149;_LL148: _tmp1DA=_tmp1D8;goto
_LL14A;_LL149: if(*((int*)_tmp1D6)!= 1)goto _LL14B;_tmp1D9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D6)->f2;if(_tmp1D9 <= (void*)1?1:*((int*)_tmp1D9)!= 3)goto _LL14B;_tmp1DA=((
struct Cyc_Absyn_Local_b_struct*)_tmp1D9)->f1;if(!(!_tmp1DA->escapes))goto _LL14B;
_LL14A: _tmp1DC=_tmp1DA;goto _LL14C;_LL14B: if(*((int*)_tmp1D6)!= 1)goto _LL14D;
_tmp1DB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D6)->f2;if(_tmp1DB <= (void*)
1?1:*((int*)_tmp1DB)!= 4)goto _LL14D;_tmp1DC=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1DB)->f1;if(!(!_tmp1DC->escapes))goto _LL14D;_LL14C: _tmp1DE=_tmp1DC;goto
_LL14E;_LL14D: if(*((int*)_tmp1D6)!= 1)goto _LL14F;_tmp1DD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D6)->f2;if(_tmp1DD <= (void*)1?1:*((int*)_tmp1DD)!= 2)goto _LL14F;_tmp1DE=((
struct Cyc_Absyn_Param_b_struct*)_tmp1DD)->f1;if(!(!_tmp1DE->escapes))goto _LL14F;
_LL14E: new_relns=({struct Cyc_List_List*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));
_tmp1DF->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));
_tmp1E0->vd=_tmpC3;_tmp1E0->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp1E2;_tmp1E2.tag=2;_tmp1E2.f1=_tmp1DE;_tmp1E2;});_tmp1E1;}));_tmp1E0;});
_tmp1DF->tl=_tmp1BA;_tmp1DF;});goto _LL146;_LL14F:;_LL150: goto _LL146;_LL146:;}
goto _LL135;_LL144:;_LL145: goto _LL135;_LL135:;}if(_tmp1BA != new_relns)_tmp1B6=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1E3=_cycalloc(sizeof(*
_tmp1E3));_tmp1E3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1E4;_tmp1E4.tag=
0;_tmp1E4.f1=_tmp1B9;_tmp1E4.f2=new_relns;_tmp1E4;});_tmp1E3;});{void*_tmp1E5=
_tmp1B7;_LL152: if((int)_tmp1E5 != 0)goto _LL154;_LL153: return({struct _tuple0
_tmp1E6;_tmp1E6.f1=_tmp1B6;_tmp1E6.f2=Cyc_CfFlowInfo_unknown_all;_tmp1E6;});
_LL154: if((int)_tmp1E5 != 2)goto _LL156;_LL155: goto _LL157;_LL156: if((int)_tmp1E5 != 
1)goto _LL158;_LL157: goto _LL159;_LL158: if(_tmp1E5 <= (void*)3?1:*((int*)_tmp1E5)!= 
2)goto _LL15A;_LL159: {struct Cyc_List_List _tmp1E7=({struct Cyc_List_List _tmp1F1;
_tmp1F1.hd=_tmpC3;_tmp1F1.tl=0;_tmp1F1;});_tmp1B6=Cyc_NewControlFlow_add_vars(
_tmp1B6,(struct Cyc_List_List*)& _tmp1E7,Cyc_CfFlowInfo_unknown_all);{void*_tmp1E9;
void*_tmp1EA;struct _tuple0 _tmp1E8=Cyc_NewControlFlow_anal_Rexp(env,_tmp1B6,
_tmpC5);_tmp1E9=_tmp1E8.f1;_tmp1EA=_tmp1E8.f2;{void*_tmp1EB=_tmp1E9;struct Cyc_Dict_Dict*
_tmp1EC;_LL15D: if((int)_tmp1EB != 0)goto _LL15F;_LL15E: return({struct _tuple0
_tmp1ED;_tmp1ED.f1=_tmp1E9;_tmp1ED.f2=Cyc_CfFlowInfo_unknown_all;_tmp1ED;});
_LL15F: if(_tmp1EB <= (void*)1?1:*((int*)_tmp1EB)!= 0)goto _LL15C;_tmp1EC=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1EB)->f1;_LL160: if(Cyc_CfFlowInfo_initlevel(
_tmp1EC,_tmp1EA)!= (void*)2){({void*_tmp1EE[0]={};Cyc_Tcutil_terr(_tmpC4->loc,({
const char*_tmp1EF="expression may not be initialized";_tag_arr(_tmp1EF,sizeof(
char),_get_zero_arr_size(_tmp1EF,34));}),_tag_arr(_tmp1EE,sizeof(void*),0));});
return({struct _tuple0 _tmp1F0;_tmp1F0.f1=(void*)0;_tmp1F0.f2=Cyc_CfFlowInfo_unknown_all;
_tmp1F0;});}_LL15C:;}_tmp1B6=_tmp1E9;goto _LL15B;}}_LL15A:;_LL15B: while(1){struct
Cyc_List_List _tmp1F2=({struct Cyc_List_List _tmp1FC;_tmp1FC.hd=_tmpC3;_tmp1FC.tl=0;
_tmp1FC;});_tmp1B6=Cyc_NewControlFlow_add_vars(_tmp1B6,(struct Cyc_List_List*)&
_tmp1F2,Cyc_CfFlowInfo_unknown_all);{void*_tmp1F4;void*_tmp1F5;struct _tuple0
_tmp1F3=Cyc_NewControlFlow_anal_Rexp(env,_tmp1B6,_tmpC5);_tmp1F4=_tmp1F3.f1;
_tmp1F5=_tmp1F3.f2;{void*_tmp1F6=_tmp1F4;struct Cyc_Dict_Dict*_tmp1F7;_LL162: if((
int)_tmp1F6 != 0)goto _LL164;_LL163: goto _LL161;_LL164: if(_tmp1F6 <= (void*)1?1:*((
int*)_tmp1F6)!= 0)goto _LL161;_tmp1F7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F6)->f1;_LL165: if(Cyc_CfFlowInfo_initlevel(_tmp1F7,_tmp1F5)!= (void*)2){({
void*_tmp1F8[0]={};Cyc_Tcutil_terr(_tmpC4->loc,({const char*_tmp1F9="expression may not be initialized";
_tag_arr(_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,34));}),_tag_arr(
_tmp1F8,sizeof(void*),0));});return({struct _tuple0 _tmp1FA;_tmp1FA.f1=(void*)0;
_tmp1FA.f2=Cyc_CfFlowInfo_unknown_all;_tmp1FA;});}_LL161:;}{void*_tmp1FB=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp1B6,_tmp1F4);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1FB,
_tmp1B6))break;_tmp1B6=_tmp1FB;}}}return({struct _tuple0 _tmp1FD;_tmp1FD.f1=
_tmp1B6;_tmp1FD.f2=Cyc_CfFlowInfo_unknown_all;_tmp1FD;});_LL151:;}}_LL130:;}}
_LLB7: if(*((int*)_tmp8A)!= 35)goto _LLB9;_tmpC7=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp8A)->f1;_LLB8: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpC7))->flow;{void*_tmp1FE=(void*)_tmpC7->r;struct Cyc_Absyn_Stmt*
_tmp1FF;struct Cyc_Absyn_Stmt*_tmp200;struct Cyc_Absyn_Decl*_tmp201;struct Cyc_Absyn_Stmt*
_tmp202;struct Cyc_Absyn_Exp*_tmp203;_LL167: if(_tmp1FE <= (void*)1?1:*((int*)
_tmp1FE)!= 1)goto _LL169;_tmp1FF=((struct Cyc_Absyn_Seq_s_struct*)_tmp1FE)->f1;
_tmp200=((struct Cyc_Absyn_Seq_s_struct*)_tmp1FE)->f2;_LL168: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp1FF);_tmpC7=_tmp200;goto _LL166;_LL169: if(_tmp1FE <= (void*)1?1:*((
int*)_tmp1FE)!= 12)goto _LL16B;_tmp201=((struct Cyc_Absyn_Decl_s_struct*)_tmp1FE)->f1;
_tmp202=((struct Cyc_Absyn_Decl_s_struct*)_tmp1FE)->f2;_LL16A: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp201);_tmpC7=_tmp202;goto _LL166;_LL16B: if(_tmp1FE <= (void*)1?1:*((
int*)_tmp1FE)!= 0)goto _LL16D;_tmp203=((struct Cyc_Absyn_Exp_s_struct*)_tmp1FE)->f1;
_LL16C: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp203);_LL16D:;_LL16E:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp204=_cycalloc(sizeof(*
_tmp204));_tmp204[0]=({struct Cyc_Core_Impossible_struct _tmp205;_tmp205.tag=Cyc_Core_Impossible;
_tmp205.f1=({const char*_tmp206="analyze_Rexp: ill-formed StmtExp";_tag_arr(
_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,33));});_tmp205;});_tmp204;}));
_LL166:;}}_LLB9: if(*((int*)_tmp8A)!= 1)goto _LLBB;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp8A)->f2;if((int)_tmpC8 != 0)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(*((int*)_tmp8A)
!= 2)goto _LLBD;_LLBC: goto _LLBE;_LLBD: if(*((int*)_tmp8A)!= 8)goto _LLBF;_LLBE: goto
_LLC0;_LLBF: if(*((int*)_tmp8A)!= 34)goto _LLC1;_LLC0: goto _LLC2;_LLC1: if(*((int*)
_tmp8A)!= 25)goto _LLC3;_LLC2: goto _LLC4;_LLC3: if(*((int*)_tmp8A)!= 36)goto _LLC5;
_LLC4: goto _LLC6;_LLC5: if(*((int*)_tmp8A)!= 37)goto _LL66;_LLC6:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({
struct Cyc_Core_Impossible_struct _tmp208;_tmp208.tag=Cyc_Core_Impossible;_tmp208.f1=({
const char*_tmp209="anal_Rexp, unexpected exp form";_tag_arr(_tmp209,sizeof(char),
_get_zero_arr_size(_tmp209,31));});_tmp208;});_tmp207;}));_LL66:;}}static struct
_tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*
_tmp20A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp20B;void*_tmp20C;struct Cyc_Absyn_PtrAtts _tmp20D;
struct Cyc_Absyn_Conref*_tmp20E;struct Cyc_Absyn_Conref*_tmp20F;_LL170: if(_tmp20A
<= (void*)3?1:*((int*)_tmp20A)!= 4)goto _LL172;_tmp20B=((struct Cyc_Absyn_PointerType_struct*)
_tmp20A)->f1;_tmp20C=(void*)_tmp20B.elt_typ;_tmp20D=_tmp20B.ptr_atts;_tmp20E=
_tmp20D.bounds;_tmp20F=_tmp20D.zero_term;_LL171: {void*_tmp210=f;struct Cyc_Dict_Dict*
_tmp211;struct Cyc_List_List*_tmp212;_LL175: if((int)_tmp210 != 0)goto _LL177;_LL176:
return({struct _tuple0 _tmp213;_tmp213.f1=f;_tmp213.f2=(void*)0;_tmp213;});_LL177:
if(_tmp210 <= (void*)1?1:*((int*)_tmp210)!= 0)goto _LL174;_tmp211=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp210)->f1;_tmp212=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp210)->f2;
_LL178: if(Cyc_Tcutil_is_bound_one(_tmp20E)){void*_tmp214=r;struct Cyc_CfFlowInfo_Place*
_tmp215;struct Cyc_CfFlowInfo_Place _tmp216;void*_tmp217;struct Cyc_List_List*
_tmp218;void*_tmp219;_LL17A: if((int)_tmp214 != 1)goto _LL17C;_LL17B: goto _LL17D;
_LL17C: if((int)_tmp214 != 2)goto _LL17E;_LL17D:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp21B;_tmp21B.tag=Cyc_CfFlowInfo_NotZero;
_tmp21B.f1=_tmp212;_tmp21B;});_tmp21A;})));goto _LL179;_LL17E: if(_tmp214 <= (void*)
3?1:*((int*)_tmp214)!= 2)goto _LL180;_tmp215=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp214)->f1;_tmp216=*_tmp215;_tmp217=(void*)_tmp216.root;_tmp218=_tmp216.fields;
_LL17F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp21D;_tmp21D.tag=Cyc_CfFlowInfo_NotZero;_tmp21D.f1=_tmp212;_tmp21D;});_tmp21C;})));
return({struct _tuple0 _tmp21E;_tmp21E.f1=f;_tmp21E.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp220;_tmp220.tag=0;_tmp220.f1=({struct Cyc_CfFlowInfo_Place*_tmp221=_cycalloc(
sizeof(*_tmp221));_tmp221->root=(void*)_tmp217;_tmp221->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp218,flds);
_tmp221;});_tmp220;});_tmp21F;});_tmp21E;});_LL180: if((int)_tmp214 != 0)goto
_LL182;_LL181:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*
_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp223;_tmp223.tag=Cyc_CfFlowInfo_IsZero;_tmp223.f1=_tmp212;_tmp223;});_tmp222;})));
return({struct _tuple0 _tmp224;_tmp224.f1=(void*)0;_tmp224.f2=(void*)0;_tmp224;});
_LL182: if(_tmp214 <= (void*)3?1:*((int*)_tmp214)!= 0)goto _LL184;_tmp219=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp214)->f1;_LL183: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp219);goto _LL185;_LL184:;_LL185:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp226;_tmp226.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp226.f1=_tmp212;_tmp226;});_tmp225;})));_LL179:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp227=_cycalloc(sizeof(*_tmp227));
_tmp227[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp228;_tmp228.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp228.f1=_tmp212;_tmp228;});_tmp227;})));}if(Cyc_CfFlowInfo_initlevel(_tmp211,
r)== (void*)0)({void*_tmp229[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp22A="dereference of possibly uninitialized pointer";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,46));}),_tag_arr(
_tmp229,sizeof(void*),0));});return({struct _tuple0 _tmp22B;_tmp22B.f1=f;_tmp22B.f2=(
void*)0;_tmp22B;});_LL174:;}_LL172:;_LL173:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C[0]=({struct Cyc_Core_Impossible_struct
_tmp22D;_tmp22D.tag=Cyc_Core_Impossible;_tmp22D.f1=({const char*_tmp22E="left deref of non-pointer-type";
_tag_arr(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,31));});_tmp22D;});
_tmp22C;}));_LL16F:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp22F=inflow;struct Cyc_Dict_Dict*_tmp230;
struct Cyc_List_List*_tmp231;_LL187: if((int)_tmp22F != 0)goto _LL189;_LL188: return({
struct _tuple0 _tmp232;_tmp232.f1=(void*)0;_tmp232.f2=(void*)0;_tmp232;});_LL189:
if(_tmp22F <= (void*)1?1:*((int*)_tmp22F)!= 0)goto _LL186;_tmp230=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp22F)->f1;_tmp231=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22F)->f2;
_LL18A: d=_tmp230;_LL186:;}{void*_tmp233=(void*)e->r;struct Cyc_Absyn_Exp*_tmp234;
struct Cyc_Absyn_Exp*_tmp235;struct Cyc_Absyn_Exp*_tmp236;void*_tmp237;struct Cyc_Absyn_Vardecl*
_tmp238;void*_tmp239;struct Cyc_Absyn_Vardecl*_tmp23A;void*_tmp23B;struct Cyc_Absyn_Vardecl*
_tmp23C;void*_tmp23D;struct Cyc_Absyn_Vardecl*_tmp23E;struct Cyc_Absyn_Exp*_tmp23F;
struct _tagged_arr*_tmp240;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_Absyn_Exp*_tmp244;struct
_tagged_arr*_tmp245;_LL18C: if(*((int*)_tmp233)!= 13)goto _LL18E;_tmp234=((struct
Cyc_Absyn_Cast_e_struct*)_tmp233)->f2;_LL18D: _tmp235=_tmp234;goto _LL18F;_LL18E:
if(*((int*)_tmp233)!= 11)goto _LL190;_tmp235=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp233)->f1;_LL18F: _tmp236=_tmp235;goto _LL191;_LL190: if(*((int*)_tmp233)!= 12)
goto _LL192;_tmp236=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp233)->f1;_LL191:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp236,flds);_LL192: if(*((int*)
_tmp233)!= 1)goto _LL194;_tmp237=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp233)->f2;
if(_tmp237 <= (void*)1?1:*((int*)_tmp237)!= 2)goto _LL194;_tmp238=((struct Cyc_Absyn_Param_b_struct*)
_tmp237)->f1;_LL193: _tmp23A=_tmp238;goto _LL195;_LL194: if(*((int*)_tmp233)!= 1)
goto _LL196;_tmp239=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp233)->f2;if(
_tmp239 <= (void*)1?1:*((int*)_tmp239)!= 3)goto _LL196;_tmp23A=((struct Cyc_Absyn_Local_b_struct*)
_tmp239)->f1;_LL195: _tmp23C=_tmp23A;goto _LL197;_LL196: if(*((int*)_tmp233)!= 1)
goto _LL198;_tmp23B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp233)->f2;if(
_tmp23B <= (void*)1?1:*((int*)_tmp23B)!= 4)goto _LL198;_tmp23C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp23B)->f1;_LL197: return({struct _tuple0 _tmp246;_tmp246.f1=inflow;_tmp246.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp247=_cycalloc(sizeof(*_tmp247));
_tmp247[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp248;_tmp248.tag=0;_tmp248.f1=({
struct Cyc_CfFlowInfo_Place*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp24A=_cycalloc(sizeof(*
_tmp24A));_tmp24A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp24B;_tmp24B.tag=0;
_tmp24B.f1=_tmp23C;_tmp24B;});_tmp24A;}));_tmp249->fields=flds;_tmp249;});
_tmp248;});_tmp247;});_tmp246;});_LL198: if(*((int*)_tmp233)!= 1)goto _LL19A;
_tmp23D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp233)->f2;if(_tmp23D <= (void*)
1?1:*((int*)_tmp23D)!= 0)goto _LL19A;_tmp23E=((struct Cyc_Absyn_Global_b_struct*)
_tmp23D)->f1;_LL199: return({struct _tuple0 _tmp24C;_tmp24C.f1=inflow;_tmp24C.f2=(
void*)0;_tmp24C;});_LL19A: if(*((int*)_tmp233)!= 22)goto _LL19C;_tmp23F=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp233)->f1;_tmp240=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp233)->f2;_LL19B: if(!Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp23F->topt))->v))flds=({struct Cyc_List_List*_tmp24D=_cycalloc(
sizeof(*_tmp24D));_tmp24D->hd=_tmp240;_tmp24D->tl=flds;_tmp24D;});_tmp241=
_tmp23F;goto _LL19D;_LL19C: if(*((int*)_tmp233)!= 20)goto _LL19E;_tmp241=((struct
Cyc_Absyn_Deref_e_struct*)_tmp233)->f1;_LL19D: {void*_tmp24F;void*_tmp250;struct
_tuple0 _tmp24E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp241);_tmp24F=_tmp24E.f1;
_tmp250=_tmp24E.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp24F,
_tmp241,_tmp250,flds);}_LL19E: if(*((int*)_tmp233)!= 23)goto _LL1A0;_tmp242=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp233)->f1;_tmp243=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp233)->f2;_LL19F:{void*_tmp251=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp242->topt))->v);_LL1A5: if(_tmp251 <= (void*)3?1:*((int*)_tmp251)
!= 9)goto _LL1A7;_LL1A6: {struct _tagged_arr*_tmp252=Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(
_tmp243)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp242,({struct
Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->hd=_tmp252;_tmp253->tl=
flds;_tmp253;}));}_LL1A7:;_LL1A8: goto _LL1A4;_LL1A4:;}{void*_tmp256;struct Cyc_List_List*
_tmp257;struct _tuple5 _tmp255=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp254[2];_tmp254[1]=_tmp243;_tmp254[0]=_tmp242;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp254,sizeof(struct Cyc_Absyn_Exp*),
2));}));_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{void*_tmp258=_tmp256;{void*
_tmp259=_tmp256;struct Cyc_Dict_Dict*_tmp25A;struct Cyc_List_List*_tmp25B;_LL1AA:
if(_tmp259 <= (void*)1?1:*((int*)_tmp259)!= 0)goto _LL1AC;_tmp25A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp259)->f1;_tmp25B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp259)->f2;
_LL1AB: if(Cyc_CfFlowInfo_initlevel(_tmp25A,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp257))->tl))->hd)== (void*)0)({
void*_tmp25C[0]={};Cyc_Tcutil_terr(_tmp243->loc,({const char*_tmp25D="expression may not be initialized";
_tag_arr(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,34));}),_tag_arr(
_tmp25C,sizeof(void*),0));});{struct Cyc_List_List*_tmp25E=Cyc_NewControlFlow_add_subscript_reln(
_tmp25B,_tmp242,_tmp243);if(_tmp25B != _tmp25E)_tmp258=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp260;_tmp260.tag=0;_tmp260.f1=_tmp25A;_tmp260.f2=_tmp25E;_tmp260;});_tmp25F;});
goto _LL1A9;}_LL1AC:;_LL1AD: goto _LL1A9;_LL1A9:;}{void*_tmp262;void*_tmp263;struct
_tuple0 _tmp261=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp256,_tmp242,(void*)((
struct Cyc_List_List*)_check_null(_tmp257))->hd,flds);_tmp262=_tmp261.f1;_tmp263=
_tmp261.f2;if(_tmp262 == (void*)0)return({struct _tuple0 _tmp264;_tmp264.f1=_tmp262;
_tmp264.f2=_tmp263;_tmp264;});else{return({struct _tuple0 _tmp265;_tmp265.f1=
_tmp258;_tmp265.f2=_tmp263;_tmp265;});}}}}_LL1A0: if(*((int*)_tmp233)!= 21)goto
_LL1A2;_tmp244=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp233)->f1;_tmp245=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp233)->f2;_LL1A1: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp244->topt))->v))return({struct
_tuple0 _tmp266;_tmp266.f1=inflow;_tmp266.f2=(void*)0;_tmp266;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp244,({struct Cyc_List_List*_tmp267=_cycalloc(sizeof(*_tmp267));
_tmp267->hd=_tmp245;_tmp267->tl=flds;_tmp267;}));_LL1A2:;_LL1A3: return({struct
_tuple0 _tmp268;_tmp268.f1=(void*)0;_tmp268.f2=(void*)0;_tmp268;});_LL18B:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp269=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp26A;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*
_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;void*_tmp26F;struct Cyc_List_List*_tmp270;
struct Cyc_List_List _tmp271;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_List_List*
_tmp273;void*_tmp274;struct Cyc_List_List*_tmp275;_LL1AF: if(*((int*)_tmp269)!= 6)
goto _LL1B1;_tmp26A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp269)->f1;_tmp26B=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp269)->f2;_tmp26C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp269)->f3;_LL1B0: {void*_tmp277;void*_tmp278;struct _tuple0 _tmp276=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp26A);_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;{void*_tmp27A;void*
_tmp27B;struct _tuple0 _tmp279=Cyc_NewControlFlow_anal_test(env,_tmp277,_tmp26B);
_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;{void*_tmp27D;void*_tmp27E;struct _tuple0
_tmp27C=Cyc_NewControlFlow_anal_test(env,_tmp278,_tmp26C);_tmp27D=_tmp27C.f1;
_tmp27E=_tmp27C.f2;return({struct _tuple0 _tmp27F;_tmp27F.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp27A,_tmp27D);_tmp27F.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp27B,_tmp27E);_tmp27F;});}}}_LL1B1: if(*((int*)_tmp269)!= 7)goto _LL1B3;_tmp26D=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp269)->f1;_tmp26E=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp269)->f2;_LL1B2: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp26D)).f1,_tmp26E);_LL1B3: if(*((int*)_tmp269)!= 3)goto _LL1B5;
_tmp26F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp269)->f1;if((int)_tmp26F != 
11)goto _LL1B5;_tmp270=((struct Cyc_Absyn_Primop_e_struct*)_tmp269)->f2;if(_tmp270
== 0)goto _LL1B5;_tmp271=*_tmp270;_tmp272=(struct Cyc_Absyn_Exp*)_tmp271.hd;
_tmp273=_tmp271.tl;if(_tmp273 != 0)goto _LL1B5;_LL1B4: {void*_tmp281;void*_tmp282;
struct _tuple0 _tmp280=Cyc_NewControlFlow_anal_test(env,inflow,_tmp272);_tmp281=
_tmp280.f1;_tmp282=_tmp280.f2;return({struct _tuple0 _tmp283;_tmp283.f1=_tmp282;
_tmp283.f2=_tmp281;_tmp283;});}_LL1B5: if(*((int*)_tmp269)!= 3)goto _LL1B7;_tmp274=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp269)->f1;_tmp275=((struct Cyc_Absyn_Primop_e_struct*)
_tmp269)->f2;if(!(((_tmp274 == (void*)5?1: _tmp274 == (void*)6)?1: _tmp274 == (void*)
8)?1: _tmp274 == (void*)10))goto _LL1B7;_LL1B6: {void*_tmp285;struct Cyc_List_List*
_tmp286;struct _tuple5 _tmp284=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,
_tmp275);_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;{void*_tmp287=_tmp285;struct Cyc_Dict_Dict*
_tmp288;struct Cyc_List_List*_tmp289;_LL1BA: if((int)_tmp287 != 0)goto _LL1BC;_LL1BB:
return({struct _tuple0 _tmp28A;_tmp28A.f1=_tmp285;_tmp28A.f2=_tmp285;_tmp28A;});
_LL1BC: if(_tmp287 <= (void*)1?1:*((int*)_tmp287)!= 0)goto _LL1B9;_tmp288=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp287)->f1;_tmp289=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp287)->f2;_LL1BD: {void*_tmp28B=(void*)((struct Cyc_List_List*)_check_null(
_tmp286))->hd;void*_tmp28C=(void*)((struct Cyc_List_List*)_check_null(_tmp286->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp288,_tmp28B)== (void*)0)({void*_tmp28D[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp275))->hd)->loc,({
const char*_tmp28E="expression may not be initialized";_tag_arr(_tmp28E,sizeof(
char),_get_zero_arr_size(_tmp28E,34));}),_tag_arr(_tmp28D,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(_tmp288,_tmp28C)== (void*)0)({void*_tmp28F[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp275))->tl))->hd)->loc,({const char*_tmp290="expression may not be initialized";
_tag_arr(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,34));}),_tag_arr(
_tmp28F,sizeof(void*),0));});{void*_tmp291=_tmp274;_LL1BF: if((int)_tmp291 != 5)
goto _LL1C1;_LL1C0: {struct _tuple0 _tmp293=({struct _tuple0 _tmp292;_tmp292.f1=
_tmp28B;_tmp292.f2=_tmp28C;_tmp292;});void*_tmp294;void*_tmp295;void*_tmp296;
void*_tmp297;void*_tmp298;void*_tmp299;_LL1CA: _tmp294=_tmp293.f1;if(_tmp294 <= (
void*)3?1:*((int*)_tmp294)!= 0)goto _LL1CC;_tmp295=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp294)->f1;_tmp296=_tmp293.f2;if((int)_tmp296 != 0)goto _LL1CC;_LL1CB: {void*
_tmp29B;void*_tmp29C;struct _tuple0 _tmp29A=Cyc_NewControlFlow_splitzero(env,
_tmp285,_tmp285,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp275))->hd,
_tmp295);_tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;return({struct _tuple0 _tmp29D;
_tmp29D.f1=_tmp29C;_tmp29D.f2=_tmp29B;_tmp29D;});}_LL1CC: _tmp297=_tmp293.f1;if((
int)_tmp297 != 0)goto _LL1CE;_tmp298=_tmp293.f2;if(_tmp298 <= (void*)3?1:*((int*)
_tmp298)!= 0)goto _LL1CE;_tmp299=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp298)->f1;_LL1CD: {void*_tmp29F;void*_tmp2A0;struct _tuple0 _tmp29E=Cyc_NewControlFlow_splitzero(
env,_tmp285,_tmp285,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp275))->tl))->hd,_tmp299);_tmp29F=_tmp29E.f1;
_tmp2A0=_tmp29E.f2;return({struct _tuple0 _tmp2A1;_tmp2A1.f1=_tmp2A0;_tmp2A1.f2=
_tmp29F;_tmp2A1;});}_LL1CE:;_LL1CF: return({struct _tuple0 _tmp2A2;_tmp2A2.f1=
_tmp285;_tmp2A2.f2=_tmp285;_tmp2A2;});_LL1C9:;}_LL1C1: if((int)_tmp291 != 6)goto
_LL1C3;_LL1C2: {struct _tuple0 _tmp2A4=({struct _tuple0 _tmp2A3;_tmp2A3.f1=_tmp28B;
_tmp2A3.f2=_tmp28C;_tmp2A3;});void*_tmp2A5;void*_tmp2A6;void*_tmp2A7;void*
_tmp2A8;void*_tmp2A9;void*_tmp2AA;_LL1D1: _tmp2A5=_tmp2A4.f1;if(_tmp2A5 <= (void*)
3?1:*((int*)_tmp2A5)!= 0)goto _LL1D3;_tmp2A6=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2A5)->f1;_tmp2A7=_tmp2A4.f2;if((int)_tmp2A7 != 0)goto _LL1D3;_LL1D2: return Cyc_NewControlFlow_splitzero(
env,_tmp285,_tmp285,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp275))->hd,_tmp2A6);_LL1D3: _tmp2A8=_tmp2A4.f1;if((int)_tmp2A8 != 0)goto _LL1D5;
_tmp2A9=_tmp2A4.f2;if(_tmp2A9 <= (void*)3?1:*((int*)_tmp2A9)!= 0)goto _LL1D5;
_tmp2AA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2A9)->f1;_LL1D4:
return Cyc_NewControlFlow_splitzero(env,_tmp285,_tmp285,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp275))->tl))->hd,
_tmp2AA);_LL1D5:;_LL1D6: return({struct _tuple0 _tmp2AB;_tmp2AB.f1=_tmp285;_tmp2AB.f2=
_tmp285;_tmp2AB;});_LL1D0:;}_LL1C3: if((int)_tmp291 != 8)goto _LL1C5;_LL1C4: {
struct Cyc_Absyn_Exp*_tmp2AC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp275))->hd;struct Cyc_Absyn_Exp*_tmp2AD=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp275->tl))->hd;void*_tmp2AE=(void*)_tmp2AC->r;
void*_tmp2AF;struct Cyc_Absyn_Vardecl*_tmp2B0;void*_tmp2B1;struct Cyc_Absyn_Vardecl*
_tmp2B2;void*_tmp2B3;struct Cyc_Absyn_Vardecl*_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Vardecl*
_tmp2B6;_LL1D8: if(*((int*)_tmp2AE)!= 1)goto _LL1DA;_tmp2AF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AE)->f2;if(_tmp2AF <= (void*)1?1:*((int*)_tmp2AF)!= 0)goto _LL1DA;_tmp2B0=((
struct Cyc_Absyn_Global_b_struct*)_tmp2AF)->f1;if(!(!_tmp2B0->escapes))goto _LL1DA;
_LL1D9: _tmp2B2=_tmp2B0;goto _LL1DB;_LL1DA: if(*((int*)_tmp2AE)!= 1)goto _LL1DC;
_tmp2B1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AE)->f2;if(_tmp2B1 <= (void*)
1?1:*((int*)_tmp2B1)!= 3)goto _LL1DC;_tmp2B2=((struct Cyc_Absyn_Local_b_struct*)
_tmp2B1)->f1;if(!(!_tmp2B2->escapes))goto _LL1DC;_LL1DB: _tmp2B4=_tmp2B2;goto
_LL1DD;_LL1DC: if(*((int*)_tmp2AE)!= 1)goto _LL1DE;_tmp2B3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AE)->f2;if(_tmp2B3 <= (void*)1?1:*((int*)_tmp2B3)!= 4)goto _LL1DE;_tmp2B4=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2B3)->f1;if(!(!_tmp2B4->escapes))goto _LL1DE;
_LL1DD: _tmp2B6=_tmp2B4;goto _LL1DF;_LL1DE: if(*((int*)_tmp2AE)!= 1)goto _LL1E0;
_tmp2B5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AE)->f2;if(_tmp2B5 <= (void*)
1?1:*((int*)_tmp2B5)!= 2)goto _LL1E0;_tmp2B6=((struct Cyc_Absyn_Param_b_struct*)
_tmp2B5)->f1;if(!(!_tmp2B6->escapes))goto _LL1E0;_LL1DF: {void*_tmp2B7=(void*)
_tmp2AD->r;void*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_Vardecl*
_tmp2BB;void*_tmp2BC;struct Cyc_Absyn_Vardecl*_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_Vardecl*
_tmp2BF;void*_tmp2C0;int _tmp2C1;void*_tmp2C2;struct Cyc_List_List*_tmp2C3;struct
Cyc_List_List _tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;_LL1E3: if(*((int*)_tmp2B7)!= 1)
goto _LL1E5;_tmp2B8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B7)->f2;if(
_tmp2B8 <= (void*)1?1:*((int*)_tmp2B8)!= 0)goto _LL1E5;_tmp2B9=((struct Cyc_Absyn_Global_b_struct*)
_tmp2B8)->f1;if(!(!_tmp2B9->escapes))goto _LL1E5;_LL1E4: _tmp2BB=_tmp2B9;goto
_LL1E6;_LL1E5: if(*((int*)_tmp2B7)!= 1)goto _LL1E7;_tmp2BA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B7)->f2;if(_tmp2BA <= (void*)1?1:*((int*)_tmp2BA)!= 3)goto _LL1E7;_tmp2BB=((
struct Cyc_Absyn_Local_b_struct*)_tmp2BA)->f1;if(!(!_tmp2BB->escapes))goto _LL1E7;
_LL1E6: _tmp2BD=_tmp2BB;goto _LL1E8;_LL1E7: if(*((int*)_tmp2B7)!= 1)goto _LL1E9;
_tmp2BC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B7)->f2;if(_tmp2BC <= (void*)
1?1:*((int*)_tmp2BC)!= 4)goto _LL1E9;_tmp2BD=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2BC)->f1;if(!(!_tmp2BD->escapes))goto _LL1E9;_LL1E8: _tmp2BF=_tmp2BD;goto
_LL1EA;_LL1E9: if(*((int*)_tmp2B7)!= 1)goto _LL1EB;_tmp2BE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B7)->f2;if(_tmp2BE <= (void*)1?1:*((int*)_tmp2BE)!= 2)goto _LL1EB;_tmp2BF=((
struct Cyc_Absyn_Param_b_struct*)_tmp2BE)->f1;if(!(!_tmp2BF->escapes))goto _LL1EB;
_LL1EA:{struct _tuple0 _tmp2C7=({struct _tuple0 _tmp2C6;_tmp2C6.f1=Cyc_Tcutil_compress((
void*)_tmp2B6->type);_tmp2C6.f2=Cyc_Tcutil_compress((void*)_tmp2BF->type);
_tmp2C6;});void*_tmp2C8;void*_tmp2C9;void*_tmp2CA;void*_tmp2CB;void*_tmp2CC;void*
_tmp2CD;_LL1F2: _tmp2C8=_tmp2C7.f1;if(_tmp2C8 <= (void*)3?1:*((int*)_tmp2C8)!= 5)
goto _LL1F4;_tmp2C9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C8)->f1;if((int)
_tmp2C9 != 1)goto _LL1F4;_tmp2CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C8)->f2;
if((int)_tmp2CA != 2)goto _LL1F4;_LL1F3: goto _LL1F1;_LL1F4: _tmp2CB=_tmp2C7.f2;if(
_tmp2CB <= (void*)3?1:*((int*)_tmp2CB)!= 5)goto _LL1F6;_tmp2CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2CB)->f1;if((int)_tmp2CC != 1)goto _LL1F6;_tmp2CD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2CB)->f2;if((int)_tmp2CD != 2)goto _LL1F6;_LL1F5: goto _LL1F1;_LL1F6:;_LL1F7:
return({struct _tuple0 _tmp2CE;_tmp2CE.f1=_tmp285;_tmp2CE.f2=_tmp285;_tmp2CE;});
_LL1F1:;}{struct Cyc_List_List*_tmp2CF=({struct Cyc_List_List*_tmp2D3=_cycalloc(
sizeof(*_tmp2D3));_tmp2D3->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2D4=_cycalloc(
sizeof(*_tmp2D4));_tmp2D4->vd=_tmp2B6;_tmp2D4->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp2D6;_tmp2D6.tag=1;_tmp2D6.f1=_tmp2BF;_tmp2D6;});_tmp2D5;}));_tmp2D4;});
_tmp2D3->tl=_tmp289;_tmp2D3;});return({struct _tuple0 _tmp2D0;_tmp2D0.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));
_tmp2D1[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2D2;_tmp2D2.tag=0;
_tmp2D2.f1=_tmp288;_tmp2D2.f2=_tmp2CF;_tmp2D2;});_tmp2D1;});_tmp2D0.f2=_tmp285;
_tmp2D0;});}_LL1EB: if(*((int*)_tmp2B7)!= 0)goto _LL1ED;_tmp2C0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2B7)->f1;if(_tmp2C0 <= (void*)1?1:*((int*)_tmp2C0)!= 2)goto _LL1ED;_tmp2C1=((
struct Cyc_Absyn_Int_c_struct*)_tmp2C0)->f2;_LL1EC:{struct _tuple0 _tmp2D8=({struct
_tuple0 _tmp2D7;_tmp2D7.f1=Cyc_Tcutil_compress((void*)_tmp2B6->type);_tmp2D7.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2AD->topt))->v);
_tmp2D7;});void*_tmp2D9;void*_tmp2DA;void*_tmp2DB;void*_tmp2DC;void*_tmp2DD;void*
_tmp2DE;_LL1F9: _tmp2D9=_tmp2D8.f1;if(_tmp2D9 <= (void*)3?1:*((int*)_tmp2D9)!= 5)
goto _LL1FB;_tmp2DA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D9)->f1;if((int)
_tmp2DA != 1)goto _LL1FB;_tmp2DB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D9)->f2;
if((int)_tmp2DB != 2)goto _LL1FB;_LL1FA: goto _LL1F8;_LL1FB: _tmp2DC=_tmp2D8.f2;if(
_tmp2DC <= (void*)3?1:*((int*)_tmp2DC)!= 5)goto _LL1FD;_tmp2DD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2DC)->f1;if((int)_tmp2DD != 1)goto _LL1FD;_tmp2DE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2DC)->f2;if((int)_tmp2DE != 2)goto _LL1FD;_LL1FC: goto _LL1F8;_LL1FD:;_LL1FE:
return({struct _tuple0 _tmp2DF;_tmp2DF.f1=_tmp285;_tmp2DF.f2=_tmp285;_tmp2DF;});
_LL1F8:;}{struct Cyc_List_List*_tmp2E0=({struct Cyc_List_List*_tmp2E4=_cycalloc(
sizeof(*_tmp2E4));_tmp2E4->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2E5=_cycalloc(
sizeof(*_tmp2E5));_tmp2E5->vd=_tmp2B6;_tmp2E5->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp2E6=_cycalloc_atomic(sizeof(*_tmp2E6));_tmp2E6[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp2E7;_tmp2E7.tag=3;_tmp2E7.f1=(unsigned int)_tmp2C1;_tmp2E7;});_tmp2E6;}));
_tmp2E5;});_tmp2E4->tl=_tmp289;_tmp2E4;});return({struct _tuple0 _tmp2E1;_tmp2E1.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2E2=_cycalloc(sizeof(*
_tmp2E2));_tmp2E2[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2E3;_tmp2E3.tag=
0;_tmp2E3.f1=_tmp288;_tmp2E3.f2=_tmp2E0;_tmp2E3;});_tmp2E2;});_tmp2E1.f2=_tmp285;
_tmp2E1;});}_LL1ED: if(*((int*)_tmp2B7)!= 3)goto _LL1EF;_tmp2C2=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B7)->f1;if((int)_tmp2C2 != 19)goto _LL1EF;_tmp2C3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B7)->f2;if(_tmp2C3 == 0)goto _LL1EF;_tmp2C4=*_tmp2C3;_tmp2C5=(struct Cyc_Absyn_Exp*)
_tmp2C4.hd;_LL1EE: {void*_tmp2E8=(void*)_tmp2C5->r;void*_tmp2E9;struct Cyc_Absyn_Vardecl*
_tmp2EA;void*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EC;void*_tmp2ED;struct Cyc_Absyn_Vardecl*
_tmp2EE;void*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F0;_LL200: if(*((int*)_tmp2E8)
!= 1)goto _LL202;_tmp2E9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E8)->f2;if(
_tmp2E9 <= (void*)1?1:*((int*)_tmp2E9)!= 0)goto _LL202;_tmp2EA=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E9)->f1;if(!(!_tmp2EA->escapes))goto _LL202;_LL201: _tmp2EC=_tmp2EA;goto
_LL203;_LL202: if(*((int*)_tmp2E8)!= 1)goto _LL204;_tmp2EB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E8)->f2;if(_tmp2EB <= (void*)1?1:*((int*)_tmp2EB)!= 3)goto _LL204;_tmp2EC=((
struct Cyc_Absyn_Local_b_struct*)_tmp2EB)->f1;if(!(!_tmp2EC->escapes))goto _LL204;
_LL203: _tmp2EE=_tmp2EC;goto _LL205;_LL204: if(*((int*)_tmp2E8)!= 1)goto _LL206;
_tmp2ED=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E8)->f2;if(_tmp2ED <= (void*)
1?1:*((int*)_tmp2ED)!= 4)goto _LL206;_tmp2EE=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2ED)->f1;if(!(!_tmp2EE->escapes))goto _LL206;_LL205: _tmp2F0=_tmp2EE;goto
_LL207;_LL206: if(*((int*)_tmp2E8)!= 1)goto _LL208;_tmp2EF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E8)->f2;if(_tmp2EF <= (void*)1?1:*((int*)_tmp2EF)!= 2)goto _LL208;_tmp2F0=((
struct Cyc_Absyn_Param_b_struct*)_tmp2EF)->f1;if(!(!_tmp2F0->escapes))goto _LL208;
_LL207: {struct Cyc_List_List*_tmp2F1=({struct Cyc_List_List*_tmp2F5=_cycalloc(
sizeof(*_tmp2F5));_tmp2F5->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F6=_cycalloc(
sizeof(*_tmp2F6));_tmp2F6->vd=_tmp2B6;_tmp2F6->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp2F8;_tmp2F8.tag=2;_tmp2F8.f1=_tmp2F0;_tmp2F8;});_tmp2F7;}));_tmp2F6;});
_tmp2F5->tl=_tmp289;_tmp2F5;});return({struct _tuple0 _tmp2F2;_tmp2F2.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));
_tmp2F3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2F4;_tmp2F4.tag=0;
_tmp2F4.f1=_tmp288;_tmp2F4.f2=_tmp2F1;_tmp2F4;});_tmp2F3;});_tmp2F2.f2=_tmp285;
_tmp2F2;});}_LL208:;_LL209: return({struct _tuple0 _tmp2F9;_tmp2F9.f1=_tmp285;
_tmp2F9.f2=_tmp285;_tmp2F9;});_LL1FF:;}_LL1EF:;_LL1F0: return({struct _tuple0
_tmp2FA;_tmp2FA.f1=_tmp285;_tmp2FA.f2=_tmp285;_tmp2FA;});_LL1E2:;}_LL1E0:;_LL1E1:
return({struct _tuple0 _tmp2FB;_tmp2FB.f1=_tmp285;_tmp2FB.f2=_tmp285;_tmp2FB;});
_LL1D7:;}_LL1C5: if((int)_tmp291 != 10)goto _LL1C7;_LL1C6: {struct Cyc_Absyn_Exp*
_tmp2FC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp275))->hd;
struct Cyc_Absyn_Exp*_tmp2FD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp275->tl))->hd;void*_tmp2FE=(void*)_tmp2FC->r;void*_tmp2FF;struct
Cyc_Absyn_Vardecl*_tmp300;void*_tmp301;struct Cyc_Absyn_Vardecl*_tmp302;void*
_tmp303;struct Cyc_Absyn_Vardecl*_tmp304;void*_tmp305;struct Cyc_Absyn_Vardecl*
_tmp306;_LL20B: if(*((int*)_tmp2FE)!= 1)goto _LL20D;_tmp2FF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FE)->f2;if(_tmp2FF <= (void*)1?1:*((int*)_tmp2FF)!= 0)goto _LL20D;_tmp300=((
struct Cyc_Absyn_Global_b_struct*)_tmp2FF)->f1;if(!(!_tmp300->escapes))goto _LL20D;
_LL20C: _tmp302=_tmp300;goto _LL20E;_LL20D: if(*((int*)_tmp2FE)!= 1)goto _LL20F;
_tmp301=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(_tmp301 <= (void*)
1?1:*((int*)_tmp301)!= 3)goto _LL20F;_tmp302=((struct Cyc_Absyn_Local_b_struct*)
_tmp301)->f1;if(!(!_tmp302->escapes))goto _LL20F;_LL20E: _tmp304=_tmp302;goto
_LL210;_LL20F: if(*((int*)_tmp2FE)!= 1)goto _LL211;_tmp303=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FE)->f2;if(_tmp303 <= (void*)1?1:*((int*)_tmp303)!= 4)goto _LL211;_tmp304=((
struct Cyc_Absyn_Pat_b_struct*)_tmp303)->f1;if(!(!_tmp304->escapes))goto _LL211;
_LL210: _tmp306=_tmp304;goto _LL212;_LL211: if(*((int*)_tmp2FE)!= 1)goto _LL213;
_tmp305=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(_tmp305 <= (void*)
1?1:*((int*)_tmp305)!= 2)goto _LL213;_tmp306=((struct Cyc_Absyn_Param_b_struct*)
_tmp305)->f1;if(!(!_tmp306->escapes))goto _LL213;_LL212: {void*_tmp307=(void*)
_tmp2FD->r;void*_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_List_List _tmp30A;
struct Cyc_Absyn_Exp*_tmp30B;_LL216: if(*((int*)_tmp307)!= 3)goto _LL218;_tmp308=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp307)->f1;if((int)_tmp308 != 19)goto
_LL218;_tmp309=((struct Cyc_Absyn_Primop_e_struct*)_tmp307)->f2;if(_tmp309 == 0)
goto _LL218;_tmp30A=*_tmp309;_tmp30B=(struct Cyc_Absyn_Exp*)_tmp30A.hd;_LL217: {
void*_tmp30C=(void*)_tmp30B->r;void*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30E;void*
_tmp30F;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp311;struct Cyc_Absyn_Vardecl*
_tmp312;void*_tmp313;struct Cyc_Absyn_Vardecl*_tmp314;_LL21B: if(*((int*)_tmp30C)
!= 1)goto _LL21D;_tmp30D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp30C)->f2;if(
_tmp30D <= (void*)1?1:*((int*)_tmp30D)!= 0)goto _LL21D;_tmp30E=((struct Cyc_Absyn_Global_b_struct*)
_tmp30D)->f1;if(!(!_tmp30E->escapes))goto _LL21D;_LL21C: _tmp310=_tmp30E;goto
_LL21E;_LL21D: if(*((int*)_tmp30C)!= 1)goto _LL21F;_tmp30F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp30C)->f2;if(_tmp30F <= (void*)1?1:*((int*)_tmp30F)!= 3)goto _LL21F;_tmp310=((
struct Cyc_Absyn_Local_b_struct*)_tmp30F)->f1;if(!(!_tmp310->escapes))goto _LL21F;
_LL21E: _tmp312=_tmp310;goto _LL220;_LL21F: if(*((int*)_tmp30C)!= 1)goto _LL221;
_tmp311=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp30C)->f2;if(_tmp311 <= (void*)
1?1:*((int*)_tmp311)!= 4)goto _LL221;_tmp312=((struct Cyc_Absyn_Pat_b_struct*)
_tmp311)->f1;if(!(!_tmp312->escapes))goto _LL221;_LL220: _tmp314=_tmp312;goto
_LL222;_LL221: if(*((int*)_tmp30C)!= 1)goto _LL223;_tmp313=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp30C)->f2;if(_tmp313 <= (void*)1?1:*((int*)_tmp313)!= 2)goto _LL223;_tmp314=((
struct Cyc_Absyn_Param_b_struct*)_tmp313)->f1;if(!(!_tmp314->escapes))goto _LL223;
_LL222: {struct Cyc_List_List*_tmp315=({struct Cyc_List_List*_tmp319=_cycalloc(
sizeof(*_tmp319));_tmp319->hd=({struct Cyc_CfFlowInfo_Reln*_tmp31A=_cycalloc(
sizeof(*_tmp31A));_tmp31A->vd=_tmp306;_tmp31A->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp31C;_tmp31C.tag=4;_tmp31C.f1=_tmp314;_tmp31C;});_tmp31B;}));_tmp31A;});
_tmp319->tl=_tmp289;_tmp319;});return({struct _tuple0 _tmp316;_tmp316.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp317=_cycalloc(sizeof(*_tmp317));
_tmp317[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp318;_tmp318.tag=0;
_tmp318.f1=_tmp288;_tmp318.f2=_tmp315;_tmp318;});_tmp317;});_tmp316.f2=_tmp285;
_tmp316;});}_LL223:;_LL224: return({struct _tuple0 _tmp31D;_tmp31D.f1=_tmp285;
_tmp31D.f2=_tmp285;_tmp31D;});_LL21A:;}_LL218:;_LL219: return({struct _tuple0
_tmp31E;_tmp31E.f1=_tmp285;_tmp31E.f2=_tmp285;_tmp31E;});_LL215:;}_LL213:;_LL214:
return({struct _tuple0 _tmp31F;_tmp31F.f1=_tmp285;_tmp31F.f2=_tmp285;_tmp31F;});
_LL20A:;}_LL1C7:;_LL1C8:({void*_tmp320[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp321="bad primop guard in new_control_flow:anal_test";
_tag_arr(_tmp321,sizeof(char),_get_zero_arr_size(_tmp321,47));}),_tag_arr(
_tmp320,sizeof(void*),0));});_LL1BE:;}}_LL1B9:;}}_LL1B7:;_LL1B8: goto _LL1AE;
_LL1AE:;}{void*_tmp323;void*_tmp324;struct _tuple0 _tmp322=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp323=_tmp322.f1;_tmp324=_tmp322.f2;{void*_tmp325=_tmp323;struct
Cyc_Dict_Dict*_tmp326;struct Cyc_List_List*_tmp327;_LL226: if((int)_tmp325 != 0)
goto _LL228;_LL227: return({struct _tuple0 _tmp328;_tmp328.f1=_tmp323;_tmp328.f2=
_tmp323;_tmp328;});_LL228: if(_tmp325 <= (void*)1?1:*((int*)_tmp325)!= 0)goto
_LL225;_tmp326=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp325)->f1;_tmp327=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp325)->f2;_LL229: {void*_tmp329=
_tmp324;void*_tmp32A;void*_tmp32B;void*_tmp32C;_LL22B: if((int)_tmp329 != 0)goto
_LL22D;_LL22C: return({struct _tuple0 _tmp32D;_tmp32D.f1=(void*)0;_tmp32D.f2=
_tmp323;_tmp32D;});_LL22D: if((int)_tmp329 != 2)goto _LL22F;_LL22E: goto _LL230;
_LL22F: if((int)_tmp329 != 1)goto _LL231;_LL230: goto _LL232;_LL231: if(_tmp329 <= (
void*)3?1:*((int*)_tmp329)!= 2)goto _LL233;_LL232: return({struct _tuple0 _tmp32E;
_tmp32E.f1=_tmp323;_tmp32E.f2=(void*)0;_tmp32E;});_LL233: if(_tmp329 <= (void*)3?1:*((
int*)_tmp329)!= 0)goto _LL235;_tmp32A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp329)->f1;if((int)_tmp32A != 0)goto _LL235;_LL234: goto _LL236;_LL235: if(_tmp329
<= (void*)3?1:*((int*)_tmp329)!= 1)goto _LL237;_tmp32B=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp329)->f1;if((int)_tmp32B != 0)goto _LL237;_LL236:({void*_tmp32F[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp330="expression may not be initialized";_tag_arr(_tmp330,
sizeof(char),_get_zero_arr_size(_tmp330,34));}),_tag_arr(_tmp32F,sizeof(void*),0));});
return({struct _tuple0 _tmp331;_tmp331.f1=(void*)0;_tmp331.f2=(void*)0;_tmp331;});
_LL237: if(_tmp329 <= (void*)3?1:*((int*)_tmp329)!= 0)goto _LL239;_tmp32C=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp329)->f1;_LL238: return Cyc_NewControlFlow_splitzero(
env,inflow,_tmp323,e,_tmp32C);_LL239: if(_tmp329 <= (void*)3?1:*((int*)_tmp329)!= 
1)goto _LL23B;_LL23A: return({struct _tuple0 _tmp332;_tmp332.f1=_tmp323;_tmp332.f2=
_tmp323;_tmp332;});_LL23B: if(_tmp329 <= (void*)3?1:*((int*)_tmp329)!= 3)goto
_LL22A;_LL23C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp333=
_cycalloc(sizeof(*_tmp333));_tmp333[0]=({struct Cyc_Core_Impossible_struct _tmp334;
_tmp334.tag=Cyc_Core_Impossible;_tmp334.f1=({const char*_tmp335="anal_test";
_tag_arr(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,10));});_tmp334;});
_tmp333;}));_LL22A:;}_LL225:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,void*flow){
void*_tmp336=flow;struct Cyc_Dict_Dict*_tmp337;_LL23E: if((int)_tmp336 != 0)goto
_LL240;_LL23F: return;_LL240: if(_tmp336 <= (void*)1?1:*((int*)_tmp336)!= 0)goto
_LL23D;_tmp337=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp336)->f1;_LL241:{
struct Cyc_List_List*_tmp338=env->param_roots;for(0;_tmp338 != 0;_tmp338=_tmp338->tl){
if(Cyc_CfFlowInfo_initlevel(_tmp337,Cyc_CfFlowInfo_lookup_place(_tmp337,(struct
Cyc_CfFlowInfo_Place*)_tmp338->hd))!= (void*)2)({void*_tmp339[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp33A="function may not initialize all the parameters with attribute 'initializes'";
_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,76));}),_tag_arr(
_tmp339,sizeof(void*),0));});}}return;_LL23D:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp33C;struct Cyc_Core_Opt*
_tmp33D;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Stmt*_tmp33F;struct Cyc_Absyn_Switch_clause*
_tmp33B=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp33C=*_tmp33B;_tmp33D=_tmp33C.pat_vars;
_tmp33E=_tmp33C.where_clause;_tmp33F=_tmp33C.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33D))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp33E != 0){void*_tmp341;void*_tmp342;struct _tuple0
_tmp340=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp33E));_tmp341=_tmp340.f1;_tmp342=_tmp340.f2;inflow=_tmp342;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp341,_tmp33F);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp33F);}{void*
_tmp343=clause_outflow;_LL243: if((int)_tmp343 != 0)goto _LL245;_LL244: goto _LL242;
_LL245:;_LL246: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp344[0]={};Cyc_Tcutil_terr(
_tmp33F->loc,({const char*_tmp345="switch clause may implicitly fallthru";
_tag_arr(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,38));}),_tag_arr(
_tmp344,sizeof(void*),0));});else{({void*_tmp346[0]={};Cyc_Tcutil_warn(_tmp33F->loc,({
const char*_tmp347="switch clause may implicitly fallthru";_tag_arr(_tmp347,
sizeof(char),_get_zero_arr_size(_tmp347,38));}),_tag_arr(_tmp346,sizeof(void*),0));});}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL242;_LL242:;}}}return(
void*)0;}static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp348=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);inflow=(void*)_tmp348->flow;{
void*_tmp349=(void*)s->r;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;
struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Stmt*_tmp34D;struct Cyc_Absyn_Stmt*
_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_Absyn_Stmt*_tmp350;struct Cyc_Absyn_Stmt*
_tmp351;struct _tuple3 _tmp352;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Stmt*
_tmp354;struct Cyc_Absyn_Stmt*_tmp355;struct Cyc_Absyn_Stmt*_tmp356;struct _tuple3
_tmp357;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Stmt*_tmp359;struct Cyc_Absyn_Exp*
_tmp35A;struct _tuple3 _tmp35B;struct Cyc_Absyn_Exp*_tmp35C;struct Cyc_Absyn_Stmt*
_tmp35D;struct _tuple3 _tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Stmt*
_tmp360;struct Cyc_Absyn_Stmt*_tmp361;struct Cyc_Absyn_Stmt*_tmp362;struct Cyc_List_List*
_tmp363;struct Cyc_Absyn_Switch_clause**_tmp364;struct Cyc_Absyn_Switch_clause*
_tmp365;struct Cyc_Absyn_Stmt*_tmp366;struct Cyc_Absyn_Stmt*_tmp367;struct Cyc_Absyn_Stmt*
_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_List_List*_tmp36A;struct Cyc_Absyn_Stmt*
_tmp36B;struct Cyc_List_List*_tmp36C;struct Cyc_Absyn_Decl*_tmp36D;struct Cyc_Absyn_Stmt*
_tmp36E;struct Cyc_Absyn_Stmt*_tmp36F;struct Cyc_Absyn_Tvar*_tmp370;struct Cyc_Absyn_Vardecl*
_tmp371;int _tmp372;struct Cyc_Absyn_Stmt*_tmp373;struct Cyc_Absyn_Exp*_tmp374;
_LL248: if((int)_tmp349 != 0)goto _LL24A;_LL249: return inflow;_LL24A: if(_tmp349 <= (
void*)1?1:*((int*)_tmp349)!= 2)goto _LL24C;_tmp34A=((struct Cyc_Absyn_Return_s_struct*)
_tmp349)->f1;if(_tmp34A != 0)goto _LL24C;_LL24B: if(env->noreturn)({void*_tmp375[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp376="`noreturn' function might return";
_tag_arr(_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,33));}),_tag_arr(
_tmp375,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,
inflow);return(void*)0;_LL24C: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 2)goto
_LL24E;_tmp34B=((struct Cyc_Absyn_Return_s_struct*)_tmp349)->f1;_LL24D: if(env->noreturn)({
void*_tmp377[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp378="`noreturn' function might return";
_tag_arr(_tmp378,sizeof(char),_get_zero_arr_size(_tmp378,33));}),_tag_arr(
_tmp377,sizeof(void*),0));});{void*_tmp37A;void*_tmp37B;struct _tuple0 _tmp379=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp34B));_tmp37A=_tmp379.f1;
_tmp37B=_tmp379.f2;_tmp37A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)
_check_null(_tmp34B))->loc,_tmp37A,_tmp37B);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp37A);return(void*)0;}_LL24E: if(_tmp349 <= (void*)1?1:*((int*)
_tmp349)!= 0)goto _LL250;_tmp34C=((struct Cyc_Absyn_Exp_s_struct*)_tmp349)->f1;
_LL24F: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp34C)).f1;_LL250: if(
_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 1)goto _LL252;_tmp34D=((struct Cyc_Absyn_Seq_s_struct*)
_tmp349)->f1;_tmp34E=((struct Cyc_Absyn_Seq_s_struct*)_tmp349)->f2;_LL251: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp34D),
_tmp34E);_LL252: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 3)goto _LL254;_tmp34F=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp349)->f1;_tmp350=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp349)->f2;_tmp351=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp349)->f3;_LL253: {
void*_tmp37D;void*_tmp37E;struct _tuple0 _tmp37C=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp34F);_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;return Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_anal_stmt(env,_tmp37D,_tmp350),Cyc_NewControlFlow_anal_stmt(
env,_tmp37E,_tmp351));}_LL254: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 4)goto
_LL256;_tmp352=((struct Cyc_Absyn_While_s_struct*)_tmp349)->f1;_tmp353=_tmp352.f1;
_tmp354=_tmp352.f2;_tmp355=((struct Cyc_Absyn_While_s_struct*)_tmp349)->f2;_LL255: {
void*_tmp37F=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp354))->flow;
void*_tmp381;void*_tmp382;struct _tuple0 _tmp380=Cyc_NewControlFlow_anal_test(env,
_tmp37F,_tmp353);_tmp381=_tmp380.f1;_tmp382=_tmp380.f2;{void*_tmp383=Cyc_NewControlFlow_anal_stmt(
env,_tmp381,_tmp355);Cyc_NewControlFlow_update_flow(env,_tmp354,_tmp383);return
_tmp382;}}_LL256: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 16)goto _LL258;
_tmp356=((struct Cyc_Absyn_Do_s_struct*)_tmp349)->f1;_tmp357=((struct Cyc_Absyn_Do_s_struct*)
_tmp349)->f2;_tmp358=_tmp357.f1;_tmp359=_tmp357.f2;_LL257: {void*_tmp384=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp356);void*_tmp385=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp384,_tmp359))->flow;void*_tmp387;void*_tmp388;struct _tuple0 _tmp386=Cyc_NewControlFlow_anal_test(
env,_tmp385,_tmp358);_tmp387=_tmp386.f1;_tmp388=_tmp386.f2;Cyc_NewControlFlow_update_flow(
env,_tmp356,_tmp387);return _tmp388;}_LL258: if(_tmp349 <= (void*)1?1:*((int*)
_tmp349)!= 8)goto _LL25A;_tmp35A=((struct Cyc_Absyn_For_s_struct*)_tmp349)->f1;
_tmp35B=((struct Cyc_Absyn_For_s_struct*)_tmp349)->f2;_tmp35C=_tmp35B.f1;_tmp35D=
_tmp35B.f2;_tmp35E=((struct Cyc_Absyn_For_s_struct*)_tmp349)->f3;_tmp35F=_tmp35E.f1;
_tmp360=_tmp35E.f2;_tmp361=((struct Cyc_Absyn_For_s_struct*)_tmp349)->f4;_LL259: {
void*_tmp389=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp35A)).f1;void*_tmp38A=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp389,_tmp35D))->flow;void*
_tmp38C;void*_tmp38D;struct _tuple0 _tmp38B=Cyc_NewControlFlow_anal_test(env,
_tmp38A,_tmp35C);_tmp38C=_tmp38B.f1;_tmp38D=_tmp38B.f2;{void*_tmp38E=Cyc_NewControlFlow_anal_stmt(
env,_tmp38C,_tmp361);void*_tmp38F=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp38E,_tmp360))->flow;void*_tmp390=(Cyc_NewControlFlow_anal_Rexp(env,_tmp38F,
_tmp35F)).f1;Cyc_NewControlFlow_update_flow(env,_tmp35D,_tmp390);return _tmp38D;}}
_LL25A: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 5)goto _LL25C;_tmp362=((struct
Cyc_Absyn_Break_s_struct*)_tmp349)->f1;if(_tmp362 != 0)goto _LL25C;_LL25B: return(
void*)0;_LL25C: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 11)goto _LL25E;_tmp363=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp349)->f1;_tmp364=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp349)->f2;if(_tmp364 == 0)goto _LL25E;_tmp365=*_tmp364;_LL25D: {void*_tmp392;
struct Cyc_List_List*_tmp393;struct _tuple5 _tmp391=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp363);_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;for(0;_tmp393 != 0;(
_tmp393=_tmp393->tl,_tmp363=_tmp363->tl)){_tmp392=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp363))->hd)->loc,
_tmp392,(void*)_tmp393->hd);}_tmp392=Cyc_NewControlFlow_add_vars(_tmp392,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp365->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp365->body,_tmp392);
return(void*)0;}_LL25E: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 5)goto _LL260;
_tmp366=((struct Cyc_Absyn_Break_s_struct*)_tmp349)->f1;_LL25F: _tmp367=_tmp366;
goto _LL261;_LL260: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 6)goto _LL262;
_tmp367=((struct Cyc_Absyn_Continue_s_struct*)_tmp349)->f1;_LL261: _tmp368=_tmp367;
goto _LL263;_LL262: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 7)goto _LL264;
_tmp368=((struct Cyc_Absyn_Goto_s_struct*)_tmp349)->f2;_LL263: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp394=_tmp348->encloser;struct Cyc_Absyn_Stmt*
_tmp395=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp368)))->encloser;while(_tmp395 != _tmp394){struct Cyc_Absyn_Stmt*_tmp396=(Cyc_NewControlFlow_get_stmt_annot(
_tmp394))->encloser;if(_tmp396 == _tmp394){({void*_tmp397[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp398="goto enters local scope or exception handler";_tag_arr(
_tmp398,sizeof(char),_get_zero_arr_size(_tmp398,45));}),_tag_arr(_tmp397,sizeof(
void*),0));});break;}_tmp394=_tmp396;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp368),inflow);return(void*)0;_LL264: if(
_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 9)goto _LL266;_tmp369=((struct Cyc_Absyn_Switch_s_struct*)
_tmp349)->f1;_tmp36A=((struct Cyc_Absyn_Switch_s_struct*)_tmp349)->f2;_LL265: {
void*_tmp39A;void*_tmp39B;struct _tuple0 _tmp399=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp369);_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;_tmp39A=Cyc_NewControlFlow_use_Rval(
env,_tmp369->loc,_tmp39A,_tmp39B);return Cyc_NewControlFlow_anal_scs(env,_tmp39A,
_tmp36A);}_LL266: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 17)goto _LL268;
_tmp36B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp349)->f1;_tmp36C=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp349)->f2;_LL267: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp36B);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp36C);{
void*_tmp39C=scs_outflow;_LL273: if((int)_tmp39C != 0)goto _LL275;_LL274: goto _LL272;
_LL275:;_LL276:({void*_tmp39D[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp39E="last catch clause may implicitly fallthru";
_tag_arr(_tmp39E,sizeof(char),_get_zero_arr_size(_tmp39E,42));}),_tag_arr(
_tmp39D,sizeof(void*),0));});_LL272:;}return s1_outflow;}}}_LL268: if(_tmp349 <= (
void*)1?1:*((int*)_tmp349)!= 12)goto _LL26A;_tmp36D=((struct Cyc_Absyn_Decl_s_struct*)
_tmp349)->f1;_tmp36E=((struct Cyc_Absyn_Decl_s_struct*)_tmp349)->f2;_LL269: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp36D),
_tmp36E);_LL26A: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 15)goto _LL26C;_tmp36F=((
struct Cyc_Absyn_Label_s_struct*)_tmp349)->f2;_LL26B: return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp36F);_LL26C: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 18)goto
_LL26E;_tmp370=((struct Cyc_Absyn_Region_s_struct*)_tmp349)->f1;_tmp371=((struct
Cyc_Absyn_Region_s_struct*)_tmp349)->f2;_tmp372=((struct Cyc_Absyn_Region_s_struct*)
_tmp349)->f3;_tmp373=((struct Cyc_Absyn_Region_s_struct*)_tmp349)->f4;_LL26D: {
struct Cyc_List_List _tmp39F=({struct Cyc_List_List _tmp3A0;_tmp3A0.hd=_tmp371;
_tmp3A0.tl=0;_tmp3A0;});inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)&
_tmp39F,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp373);}_LL26E: if(_tmp349 <= (void*)1?1:*((int*)_tmp349)!= 20)goto _LL270;
_tmp374=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp349)->f1;_LL26F: {void*
_tmp3A2;void*_tmp3A3;struct _tuple0 _tmp3A1=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp374);_tmp3A2=_tmp3A1.f1;_tmp3A3=_tmp3A1.f2;{void*_tmp3A4=Cyc_NewControlFlow_use_Rval(
env,_tmp374->loc,_tmp3A2,_tmp3A3);void*_tmp3A5=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp374->topt))->v);void*_tmp3A6;_LL278: if(
_tmp3A5 <= (void*)3?1:*((int*)_tmp3A5)!= 15)goto _LL27A;_tmp3A6=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3A5)->f1;_LL279: return Cyc_CfFlowInfo_kill_flow_region(_tmp3A2,_tmp3A6);
_LL27A:;_LL27B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A7=
_cycalloc(sizeof(*_tmp3A7));_tmp3A7[0]=({struct Cyc_Core_Impossible_struct _tmp3A8;
_tmp3A8.tag=Cyc_Core_Impossible;_tmp3A8.f1=({const char*_tmp3A9="anal_stmt -- reset_region";
_tag_arr(_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,26));});_tmp3A8;});
_tmp3A7;}));_LL277:;}}_LL270:;_LL271:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA[0]=({struct Cyc_Core_Impossible_struct
_tmp3AB;_tmp3AB.tag=Cyc_Core_Impossible;_tmp3AB.f1=({const char*_tmp3AC="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_arr(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,56));});_tmp3AB;});
_tmp3AA;}));_LL247:;}}static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp3AD=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp3AE;struct Cyc_Core_Opt*_tmp3AF;struct Cyc_Core_Opt _tmp3B0;struct Cyc_List_List*
_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_List_List*_tmp3B3;_LL27D: if(*((int*)
_tmp3AD)!= 0)goto _LL27F;_tmp3AE=((struct Cyc_Absyn_Var_d_struct*)_tmp3AD)->f1;
_LL27E: {struct Cyc_List_List _tmp3B4=({struct Cyc_List_List _tmp3C2;_tmp3C2.hd=
_tmp3AE;_tmp3C2.tl=0;_tmp3C2;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp3B4,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp3B5=_tmp3AE->initializer;if(_tmp3B5 == 0)return inflow;{void*_tmp3B7;void*
_tmp3B8;struct _tuple0 _tmp3B6=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3B5));_tmp3B7=_tmp3B6.f1;_tmp3B8=_tmp3B6.f2;{void*_tmp3B9=
_tmp3B7;struct Cyc_Dict_Dict*_tmp3BA;struct Cyc_List_List*_tmp3BB;_LL286: if((int)
_tmp3B9 != 0)goto _LL288;_LL287: return(void*)0;_LL288: if(_tmp3B9 <= (void*)1?1:*((
int*)_tmp3B9)!= 0)goto _LL285;_tmp3BA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3B9)->f1;_tmp3BB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3B9)->f2;
_LL289: _tmp3BA=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp3BA,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp3BD=_cycalloc(sizeof(*
_tmp3BD));_tmp3BD[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp3BE;_tmp3BE.tag=0;
_tmp3BE.f1=_tmp3AE;_tmp3BE;});_tmp3BD;}));_tmp3BC->fields=0;_tmp3BC;}),_tmp3B8);
_tmp3BB=Cyc_CfFlowInfo_reln_assign_var(_tmp3BB,_tmp3AE,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3AE->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3BF=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3C0=_cycalloc(sizeof(*
_tmp3C0));_tmp3C0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3C1;_tmp3C1.tag=
0;_tmp3C1.f1=_tmp3BA;_tmp3C1.f2=_tmp3BB;_tmp3C1;});_tmp3C0;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp3BF);return(void*)_tmp3BF;}_LL285:;}}}}_LL27F: if(*((int*)_tmp3AD)
!= 2)goto _LL281;_tmp3AF=((struct Cyc_Absyn_Let_d_struct*)_tmp3AD)->f2;if(_tmp3AF
== 0)goto _LL281;_tmp3B0=*_tmp3AF;_tmp3B1=(struct Cyc_List_List*)_tmp3B0.v;_tmp3B2=((
struct Cyc_Absyn_Let_d_struct*)_tmp3AD)->f3;_LL280: {void*_tmp3C4;void*_tmp3C5;
struct _tuple0 _tmp3C3=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp3B2);_tmp3C4=
_tmp3C3.f1;_tmp3C5=_tmp3C3.f2;_tmp3C4=Cyc_NewControlFlow_use_Rval(env,_tmp3B2->loc,
_tmp3C4,_tmp3C5);return Cyc_NewControlFlow_add_vars(_tmp3C4,_tmp3B1,Cyc_CfFlowInfo_unknown_all);}
_LL281: if(*((int*)_tmp3AD)!= 3)goto _LL283;_tmp3B3=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3AD)->f1;_LL282: return Cyc_NewControlFlow_add_vars(inflow,_tmp3B3,Cyc_CfFlowInfo_unknown_none);
_LL283:;_LL284:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3C6=
_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Core_Impossible_struct _tmp3C7;
_tmp3C7.tag=Cyc_Core_Impossible;_tmp3C7.f1=({const char*_tmp3C8="anal_decl: unexpected decl variant";
_tag_arr(_tmp3C8,sizeof(char),_get_zero_arr_size(_tmp3C8,35));});_tmp3C7;});
_tmp3C6;}));_LL27C:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp3C9=(void*)s->r;
struct _tuple3 _tmp3CA;struct Cyc_Absyn_Stmt*_tmp3CB;struct _tuple3 _tmp3CC;struct Cyc_Absyn_Stmt*
_tmp3CD;struct Cyc_Absyn_Stmt*_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3CF;struct Cyc_Absyn_Stmt*
_tmp3D0;struct _tuple3 _tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D2;struct Cyc_Absyn_Stmt*
_tmp3D3;struct Cyc_Absyn_Stmt*_tmp3D4;struct _tuple3 _tmp3D5;struct Cyc_Absyn_Stmt*
_tmp3D6;struct Cyc_Absyn_Stmt*_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D8;struct Cyc_Absyn_Stmt*
_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Stmt*
_tmp3DC;struct Cyc_List_List*_tmp3DD;struct Cyc_List_List*_tmp3DE;_LL28B: if(
_tmp3C9 <= (void*)1?1:*((int*)_tmp3C9)!= 8)goto _LL28D;_tmp3CA=((struct Cyc_Absyn_For_s_struct*)
_tmp3C9)->f2;_tmp3CB=_tmp3CA.f2;_tmp3CC=((struct Cyc_Absyn_For_s_struct*)_tmp3C9)->f3;
_tmp3CD=_tmp3CC.f2;_tmp3CE=((struct Cyc_Absyn_For_s_struct*)_tmp3C9)->f4;_LL28C:
Cyc_NewControlFlow_postpass(_tmp3CD);_tmp3CF=_tmp3CE;_tmp3D0=_tmp3CB;goto _LL28E;
_LL28D: if(_tmp3C9 <= (void*)1?1:*((int*)_tmp3C9)!= 1)goto _LL28F;_tmp3CF=((struct
Cyc_Absyn_Seq_s_struct*)_tmp3C9)->f1;_tmp3D0=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3C9)->f2;_LL28E: _tmp3D2=_tmp3CF;_tmp3D3=_tmp3D0;goto _LL290;_LL28F: if(_tmp3C9
<= (void*)1?1:*((int*)_tmp3C9)!= 4)goto _LL291;_tmp3D1=((struct Cyc_Absyn_While_s_struct*)
_tmp3C9)->f1;_tmp3D2=_tmp3D1.f2;_tmp3D3=((struct Cyc_Absyn_While_s_struct*)
_tmp3C9)->f2;_LL290: _tmp3D4=_tmp3D3;_tmp3D6=_tmp3D2;goto _LL292;_LL291: if(_tmp3C9
<= (void*)1?1:*((int*)_tmp3C9)!= 16)goto _LL293;_tmp3D4=((struct Cyc_Absyn_Do_s_struct*)
_tmp3C9)->f1;_tmp3D5=((struct Cyc_Absyn_Do_s_struct*)_tmp3C9)->f2;_tmp3D6=_tmp3D5.f2;
_LL292: _tmp3D7=_tmp3D4;_tmp3D8=_tmp3D6;goto _LL294;_LL293: if(_tmp3C9 <= (void*)1?1:*((
int*)_tmp3C9)!= 3)goto _LL295;_tmp3D7=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3C9)->f2;_tmp3D8=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3C9)->f3;_LL294:
Cyc_NewControlFlow_postpass(_tmp3D7);_tmp3D9=_tmp3D8;goto _LL296;_LL295: if(
_tmp3C9 <= (void*)1?1:*((int*)_tmp3C9)!= 18)goto _LL297;_tmp3D9=((struct Cyc_Absyn_Region_s_struct*)
_tmp3C9)->f4;_LL296: _tmp3DA=_tmp3D9;goto _LL298;_LL297: if(_tmp3C9 <= (void*)1?1:*((
int*)_tmp3C9)!= 15)goto _LL299;_tmp3DA=((struct Cyc_Absyn_Label_s_struct*)_tmp3C9)->f2;
_LL298: _tmp3DB=_tmp3DA;goto _LL29A;_LL299: if(_tmp3C9 <= (void*)1?1:*((int*)_tmp3C9)
!= 12)goto _LL29B;_tmp3DB=((struct Cyc_Absyn_Decl_s_struct*)_tmp3C9)->f2;_LL29A:
Cyc_NewControlFlow_postpass(_tmp3DB);return;_LL29B: if(_tmp3C9 <= (void*)1?1:*((
int*)_tmp3C9)!= 17)goto _LL29D;_tmp3DC=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3C9)->f1;_tmp3DD=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3C9)->f2;_LL29C:
Cyc_NewControlFlow_postpass(_tmp3DC);_tmp3DE=_tmp3DD;goto _LL29E;_LL29D: if(
_tmp3C9 <= (void*)1?1:*((int*)_tmp3C9)!= 9)goto _LL29F;_tmp3DE=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3C9)->f2;_LL29E: for(0;_tmp3DE != 0;_tmp3DE=_tmp3DE->tl){Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*)_tmp3DE->hd)->body);}return;_LL29F:;_LL2A0:
return;_LL28A:;}}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Position_Segment*_tmp3DF=(fd->body)->loc;void*inflow=Cyc_NewControlFlow_add_vars((
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp414=_cycalloc(sizeof(*
_tmp414));_tmp414[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp415;_tmp415.tag=
0;_tmp415.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp415.f2=0;_tmp415;});
_tmp414;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);struct Cyc_List_List*_tmp3E0=0;{void*_tmp3E1=inflow;
struct Cyc_Dict_Dict*_tmp3E2;struct Cyc_List_List*_tmp3E3;_LL2A2: if((int)_tmp3E1 != 
0)goto _LL2A4;_LL2A3:({void*_tmp3E4[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3E5="check_fun";_tag_arr(
_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,10));}),_tag_arr(_tmp3E4,sizeof(
void*),0));});_LL2A4: if(_tmp3E1 <= (void*)1?1:*((int*)_tmp3E1)!= 0)goto _LL2A1;
_tmp3E2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3E1)->f1;_tmp3E3=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3E1)->f2;_LL2A5: {struct Cyc_List_List*
atts;{void*_tmp3E6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp3E7;struct Cyc_List_List*_tmp3E8;
_LL2A7: if(_tmp3E6 <= (void*)3?1:*((int*)_tmp3E6)!= 8)goto _LL2A9;_tmp3E7=((struct
Cyc_Absyn_FnType_struct*)_tmp3E6)->f1;_tmp3E8=_tmp3E7.attributes;_LL2A8: atts=
_tmp3E8;goto _LL2A6;_LL2A9:;_LL2AA:({void*_tmp3E9[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3EA="check_fun: non-function type cached with fndecl_t";
_tag_arr(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,50));}),_tag_arr(
_tmp3E9,sizeof(void*),0));});_LL2A6:;}for(0;atts != 0;atts=atts->tl){void*_tmp3EB=(
void*)atts->hd;int _tmp3EC;_LL2AC: if(_tmp3EB <= (void*)17?1:*((int*)_tmp3EB)!= 4)
goto _LL2AE;_tmp3EC=((struct Cyc_Absyn_Initializes_att_struct*)_tmp3EB)->f1;_LL2AD: {
unsigned int j=(unsigned int)_tmp3EC;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp3ED[0]={};Cyc_Tcutil_terr(_tmp3DF,({const char*_tmp3EE="initializes attribute exceeds number of parameters";
_tag_arr(_tmp3EE,sizeof(char),_get_zero_arr_size(_tmp3EE,51));}),_tag_arr(
_tmp3ED,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp3EF=((struct
Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*
_tmp3F0=Cyc_Tcutil_compress((void*)_tmp3EF->type);struct Cyc_Absyn_PtrInfo _tmp3F1;
void*_tmp3F2;struct Cyc_Absyn_PtrAtts _tmp3F3;struct Cyc_Absyn_Conref*_tmp3F4;
struct Cyc_Absyn_Conref*_tmp3F5;struct Cyc_Absyn_Conref*_tmp3F6;_LL2B1: if(_tmp3F0
<= (void*)3?1:*((int*)_tmp3F0)!= 4)goto _LL2B3;_tmp3F1=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F0)->f1;_tmp3F2=(void*)_tmp3F1.elt_typ;_tmp3F3=_tmp3F1.ptr_atts;_tmp3F4=
_tmp3F3.nullable;_tmp3F5=_tmp3F3.bounds;_tmp3F6=_tmp3F3.zero_term;_LL2B2: if(((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp3F4))({void*_tmp3F7[0]={};
Cyc_Tcutil_terr(_tmp3DF,({const char*_tmp3F8="initializes attribute not allowed on nullable pointers";
_tag_arr(_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,55));}),_tag_arr(
_tmp3F7,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp3F5))({void*_tmp3F9[
0]={};Cyc_Tcutil_terr(_tmp3DF,({const char*_tmp3FA="initializes attribute allowed only on pointers of size 1";
_tag_arr(_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,57));}),_tag_arr(
_tmp3F9,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3F6))({void*_tmp3FB[0]={};Cyc_Tcutil_terr(_tmp3DF,({const char*_tmp3FC="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_arr(_tmp3FC,sizeof(char),_get_zero_arr_size(_tmp3FC,77));}),_tag_arr(
_tmp3FB,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp3FD=({
struct Cyc_CfFlowInfo_InitParam_struct*_tmp405=_cycalloc(sizeof(*_tmp405));
_tmp405[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp406;_tmp406.tag=2;_tmp406.f1=(
int)j;_tmp406.f2=(void*)_tmp3F2;_tmp406;});_tmp405;});struct Cyc_CfFlowInfo_Place*
_tmp3FE=({struct Cyc_CfFlowInfo_Place*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404->root=(
void*)((void*)_tmp3FD);_tmp404->fields=0;_tmp404;});_tmp3E2=Cyc_Dict_insert(
_tmp3E2,(void*)_tmp3FD,Cyc_CfFlowInfo_typ_to_absrval(_tmp3F2,Cyc_CfFlowInfo_esc_none));
_tmp3E2=Cyc_Dict_insert(_tmp3E2,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp400;_tmp400.tag=0;_tmp400.f1=_tmp3EF;_tmp400;});_tmp3FF;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp402;_tmp402.tag=2;_tmp402.f1=_tmp3FE;
_tmp402;});_tmp401;}));_tmp3E0=({struct Cyc_List_List*_tmp403=_cycalloc(sizeof(*
_tmp403));_tmp403->hd=_tmp3FE;_tmp403->tl=_tmp3E0;_tmp403;});goto _LL2B0;}_LL2B3:;
_LL2B4:({void*_tmp407[0]={};Cyc_Tcutil_terr(_tmp3DF,({const char*_tmp408="initializes attribute on non-pointer";
_tag_arr(_tmp408,sizeof(char),_get_zero_arr_size(_tmp408,37));}),_tag_arr(
_tmp407,sizeof(void*),0));});_LL2B0:;}goto _LL2AB;}}_LL2AE:;_LL2AF: goto _LL2AB;
_LL2AB:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp409=
_cycalloc(sizeof(*_tmp409));_tmp409[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp40A;_tmp40A.tag=0;_tmp40A.f1=_tmp3E2;_tmp40A.f2=_tmp3E3;_tmp40A;});_tmp409;});}
_LL2A1:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct
Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp413=
_cycalloc(sizeof(*_tmp413));_tmp413->iterate_again=1;_tmp413->iteration_num=0;
_tmp413->in_try=0;_tmp413->tryflow=(void*)inflow;_tmp413->all_changed=0;_tmp413->noreturn=
noreturn;_tmp413->param_roots=_tmp3E0;_tmp413;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp40B=outflow;_LL2B6: if((int)_tmp40B != 0)goto
_LL2B8;_LL2B7: goto _LL2B5;_LL2B8:;_LL2B9: Cyc_NewControlFlow_check_init_params(
_tmp3DF,env,outflow);if(noreturn)({void*_tmp40C[0]={};Cyc_Tcutil_terr(_tmp3DF,({
const char*_tmp40D="`noreturn' function might (implicitly) return";_tag_arr(
_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,46));}),_tag_arr(_tmp40C,sizeof(
void*),0));});else{void*_tmp40E=Cyc_Tcutil_compress((void*)fd->ret_type);_LL2BB:
if((int)_tmp40E != 0)goto _LL2BD;_LL2BC: goto _LL2BA;_LL2BD: if(_tmp40E <= (void*)3?1:*((
int*)_tmp40E)!= 6)goto _LL2BF;_LL2BE: goto _LL2C0;_LL2BF: if((int)_tmp40E != 1)goto
_LL2C1;_LL2C0: goto _LL2C2;_LL2C1: if(_tmp40E <= (void*)3?1:*((int*)_tmp40E)!= 5)
goto _LL2C3;_LL2C2:({void*_tmp40F[0]={};Cyc_Tcutil_warn(_tmp3DF,({const char*
_tmp410="function may not return a value";_tag_arr(_tmp410,sizeof(char),
_get_zero_arr_size(_tmp410,32));}),_tag_arr(_tmp40F,sizeof(void*),0));});goto
_LL2BA;_LL2C3:;_LL2C4:({void*_tmp411[0]={};Cyc_Tcutil_terr(_tmp3DF,({const char*
_tmp412="function may not return a value";_tag_arr(_tmp412,sizeof(char),
_get_zero_arr_size(_tmp412,32));}),_tag_arr(_tmp411,sizeof(void*),0));});goto
_LL2BA;_LL2BA:;}goto _LL2B5;_LL2B5:;}Cyc_NewControlFlow_postpass(fd->body);}}void
Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*
_tmp416=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp417;
struct Cyc_List_List*_tmp418;struct Cyc_List_List*_tmp419;struct Cyc_List_List*
_tmp41A;_LL2C6: if(*((int*)_tmp416)!= 1)goto _LL2C8;_tmp417=((struct Cyc_Absyn_Fn_d_struct*)
_tmp416)->f1;_LL2C7: Cyc_NewControlFlow_check_fun(_tmp417);goto _LL2C5;_LL2C8: if(*((
int*)_tmp416)!= 10)goto _LL2CA;_tmp418=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp416)->f1;_LL2C9: _tmp419=_tmp418;goto _LL2CB;_LL2CA: if(*((int*)_tmp416)!= 9)
goto _LL2CC;_tmp419=((struct Cyc_Absyn_Using_d_struct*)_tmp416)->f2;_LL2CB: _tmp41A=
_tmp419;goto _LL2CD;_LL2CC: if(*((int*)_tmp416)!= 8)goto _LL2CE;_tmp41A=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp416)->f2;_LL2CD: Cyc_NewControlFlow_cf_check(_tmp41A);goto _LL2C5;_LL2CE:;
_LL2CF: goto _LL2C5;_LL2C5:;}}
