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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fflush(struct Cyc___cycFILE*);
int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct
Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;
unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,void*
elt);extern char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[
12];extern char Cyc_SlowDict_Absent[11];struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(
int(*cmp)(void*,void*));int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,
void*v);struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,
void*k);struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,
void*k);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;
int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_strptrcmp(
struct _tagged_arr*s1,struct _tagged_arr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{void*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple1*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_env_err(struct _tagged_arr msg);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
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
struct _tagged_arr*,struct Cyc_Absyn_Stmt*);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs);void*Cyc_Tcenv_curr_rgn(struct
Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*
te,void*r,int resetable);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void
Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _tagged_arr msg){({struct Cyc_String_pa_struct _tmp2;
_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp0[1]={&
_tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Error: %s\n";_tag_arr(_tmp1,
sizeof(char),_get_zero_arr_size(_tmp1,11));}),_tag_arr(_tmp0,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_CtrlEnv{void*
continue_stmt;void*break_stmt;struct _tuple4*fallthru_clause;void*next_stmt;int
try_depth;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_Dict_Dict*
seen_labels;struct Cyc_SlowDict_Dict*needed_labels;struct Cyc_List_List*
delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Fenv{
struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*
region_order;struct Cyc_Dict_Dict*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;};struct _tuple5{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(){Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp3;});{struct
Cyc_Tcenv_Genv*_tmp4=Cyc_Tcenv_empty_genv();struct Cyc_List_List*_tmp5=0;_tmp4->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(_tmp4->tuniondecls,(*((struct _tuple1*)Cyc_Absyn_exn_name)).f2,({
struct Cyc_Absyn_Tuniondecl**_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=Cyc_Absyn_exn_tud;
_tmp6;}));{struct Cyc_List_List*_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp7 != 0;_tmp7=_tmp7->tl){
_tmp4->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(_tmp4->ordinaries,(*((struct
_tuple1*)((struct Cyc_Absyn_Tunionfield*)_tmp7->hd)->name)).f2,({struct _tuple5*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Tcenv_TunionRes_struct _tmpA;
_tmpA.tag=2;_tmpA.f1=Cyc_Absyn_exn_tud;_tmpA.f2=(struct Cyc_Absyn_Tunionfield*)
_tmp7->hd;_tmpA;});_tmp9;});_tmp8->f2=1;_tmp8;}));}}{struct Cyc_Dict_Dict*ae=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,
struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),_tmp5,_tmp4);
return({struct Cyc_Tcenv_Tenv*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->ns=_tmp5;
_tmpB->ae=ae;_tmpB->le=0;_tmpB;});}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tagged_arr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0)return((struct Cyc_List_List*(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmpC="Tcenv::outer_namespace";_tag_arr(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,23));}));return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _tagged_arr*)n1->hd,(struct
_tagged_arr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}static
void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v,
struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;nss=nss->tl){if(!
Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)nss->hd);struct _handler_cons _tmpD;_push_handler(&
_tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))_tmpF=1;if(!_tmpF){lookup(ge2,v);({
struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct _tagged_arr*)v));{void*_tmp10[1]={& _tmp12};Cyc_Tcutil_terr(
loc,({const char*_tmp11="%s is ambiguous";_tag_arr(_tmp11,sizeof(char),
_get_zero_arr_size(_tmp11,16));}),_tag_arr(_tmp10,sizeof(void*),1));}});;
_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*_tmp14=_tmpE;_LL1: if(
_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp14);
_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_tagged_arr*),struct _tagged_arr*v){struct Cyc_List_List*cns=te->ns;while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _tagged_arr*v){struct _tuple5*ans=((struct _tuple5*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,v);(*((struct _tuple5*)ans)).f2=
1;return(*((struct _tuple5*)ans)).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp23;void*_tmp24;struct _tagged_arr*_tmp25;struct _tuple1*_tmp22=q;_tmp23=*
_tmp22;_tmp24=_tmp23.f1;_tmp25=_tmp23.f2;{void*_tmp26=_tmp24;struct Cyc_List_List*
_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_List_List _tmp29;struct _tagged_arr*
_tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2C;_LL10: if((int)
_tmp26 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp26 <= (void*)1?1:*((int*)_tmp26)
!= 0)goto _LL14;_tmp27=((struct Cyc_Absyn_Rel_n_struct*)_tmp26)->f1;if(_tmp27 != 0)
goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp25);_LL14: if(_tmp26 <= (void*)1?1:*((int*)_tmp26)!= 0)goto _LL16;_tmp28=((
struct Cyc_Absyn_Rel_n_struct*)_tmp26)->f1;if(_tmp28 == 0)goto _LL16;_tmp29=*_tmp28;
_tmp2A=(struct _tagged_arr*)_tmp29.hd;_tmp2B=_tmp29.tl;_LL15: {struct Cyc_Tcenv_Genv*
_tmp2D=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2A,_tmp2B);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp2D,_tmp25);}_LL16: if(_tmp26 <= (void*)1?1:*((int*)_tmp26)!= 1)goto _LLF;_tmp2C=((
struct Cyc_Absyn_Abs_n_struct*)_tmp26)->f1;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp2C),_tmp25);_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tagged_arr*n,
struct Cyc_List_List*ns){struct Cyc_List_List*_tmp2E=te->ns;struct Cyc_List_List*
_tmp2F=0;while(1){struct Cyc_Tcenv_Genv*_tmp30=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2E);{struct
Cyc_List_List*_tmp31=_tmp30->availables;for(0;_tmp31 != 0;_tmp31=_tmp31->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp31->hd);if(((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ge2->namespaces,
n))_tmp2F=({struct Cyc_List_List*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->hd=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp2E,({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=n;
_tmp33->tl=ns;_tmp33;}));_tmp32->tl=_tmp2F;_tmp32;});}}if(((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp30->namespaces,n))_tmp2F=({struct
Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2E,({struct Cyc_List_List*
_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=n;_tmp35->tl=ns;_tmp35;}));_tmp34->tl=
_tmp2F;_tmp34;});if(_tmp2F != 0){if(_tmp2F->tl != 0)({struct Cyc_String_pa_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)n));{void*_tmp36[1]={& _tmp38};Cyc_Tcutil_terr(loc,({const char*
_tmp37="%s is ambiguous";_tag_arr(_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,
16));}),_tag_arr(_tmp36,sizeof(void*),1));}});return(struct Cyc_List_List*)_tmp2F->hd;}
if(_tmp2E == 0)(int)_throw((void*)Cyc_Dict_Absent);_tmp2E=Cyc_Tcenv_outer_namespace(
_tmp2E);}}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->aggrdecls,v);}struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct _tuple1 _tmp3A;void*_tmp3B;struct _tagged_arr*_tmp3C;struct
_tuple1*_tmp39=q;_tmp3A=*_tmp39;_tmp3B=_tmp3A.f1;_tmp3C=_tmp3A.f2;{void*_tmp3D=
_tmp3B;struct Cyc_List_List*_tmp3E;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*
_tmp40;struct Cyc_List_List _tmp41;struct _tagged_arr*_tmp42;struct Cyc_List_List*
_tmp43;_LL19: if((int)_tmp3D != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp3D <= (
void*)1?1:*((int*)_tmp3D)!= 0)goto _LL1D;_tmp3E=((struct Cyc_Absyn_Rel_n_struct*)
_tmp3D)->f1;if(_tmp3E != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp3C);_LL1D: if(_tmp3D <= (void*)1?1:*((int*)
_tmp3D)!= 1)goto _LL1F;_tmp3F=((struct Cyc_Absyn_Abs_n_struct*)_tmp3D)->f1;_LL1E: {
struct Cyc_Tcenv_Genv*_tmp44=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp3F);return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp44->aggrdecls,
_tmp3C);}_LL1F: if(_tmp3D <= (void*)1?1:*((int*)_tmp3D)!= 0)goto _LL18;_tmp40=((
struct Cyc_Absyn_Rel_n_struct*)_tmp3D)->f1;if(_tmp40 == 0)goto _LL18;_tmp41=*_tmp40;
_tmp42=(struct _tagged_arr*)_tmp41.hd;_tmp43=_tmp41.tl;_LL20: {struct Cyc_Tcenv_Genv*
_tmp45=Cyc_Tcenv_lookup_namespace(te,loc,_tmp42,_tmp43);return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp45->aggrdecls,
_tmp3C);}_LL18:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}
struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp47;void*_tmp48;
struct _tagged_arr*_tmp49;struct _tuple1*_tmp46=q;_tmp47=*_tmp46;_tmp48=_tmp47.f1;
_tmp49=_tmp47.f2;{void*_tmp4A=_tmp48;struct Cyc_List_List*_tmp4B;struct Cyc_List_List*
_tmp4C;struct Cyc_List_List _tmp4D;struct _tagged_arr*_tmp4E;struct Cyc_List_List*
_tmp4F;struct Cyc_List_List*_tmp50;_LL22: if((int)_tmp4A != 0)goto _LL24;_LL23: goto
_LL25;_LL24: if(_tmp4A <= (void*)1?1:*((int*)_tmp4A)!= 0)goto _LL26;_tmp4B=((struct
Cyc_Absyn_Rel_n_struct*)_tmp4A)->f1;if(_tmp4B != 0)goto _LL26;_LL25: return((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),
struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_tuniondecl_f,
_tmp49);_LL26: if(_tmp4A <= (void*)1?1:*((int*)_tmp4A)!= 0)goto _LL28;_tmp4C=((
struct Cyc_Absyn_Rel_n_struct*)_tmp4A)->f1;if(_tmp4C == 0)goto _LL28;_tmp4D=*_tmp4C;
_tmp4E=(struct _tagged_arr*)_tmp4D.hd;_tmp4F=_tmp4D.tl;_LL27: {struct Cyc_Tcenv_Genv*
_tmp51=Cyc_Tcenv_lookup_namespace(te,loc,_tmp4E,_tmp4F);return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp51->tuniondecls,
_tmp49);}_LL28: if(_tmp4A <= (void*)1?1:*((int*)_tmp4A)!= 1)goto _LL21;_tmp50=((
struct Cyc_Absyn_Abs_n_struct*)_tmp4A)->f1;_LL29: {struct Cyc_Tcenv_Genv*_tmp52=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp50);return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp52->tuniondecls,_tmp49);}_LL21:;}}
static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_xtuniondecl_f(struct Cyc_Tcenv_Genv*
ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp54;void*_tmp55;struct _tagged_arr*_tmp56;
struct _tuple1*_tmp53=q;_tmp54=*_tmp53;_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;{void*
_tmp57=_tmp55;struct Cyc_List_List*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List
_tmp5A;struct _tagged_arr*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*
_tmp5D;_LL2B: if(_tmp57 <= (void*)1?1:*((int*)_tmp57)!= 0)goto _LL2D;_tmp58=((
struct Cyc_Absyn_Rel_n_struct*)_tmp57)->f1;if(_tmp58 != 0)goto _LL2D;_LL2C: {struct
_handler_cons _tmp5E;_push_handler(& _tmp5E);{int _tmp60=0;if(setjmp(_tmp5E.handler))
_tmp60=1;if(!_tmp60){{struct Cyc_Core_Opt*_tmp62=({struct Cyc_Core_Opt*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61->v=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmp56);_tmp61;});_npop_handler(0);return
_tmp62;};_pop_handler();}else{void*_tmp5F=(void*)_exn_thrown;void*_tmp64=_tmp5F;
_LL34: if(_tmp64 != Cyc_Dict_Absent)goto _LL36;_LL35: return 0;_LL36:;_LL37:(void)
_throw(_tmp64);_LL33:;}}}_LL2D: if((int)_tmp57 != 0)goto _LL2F;_LL2E:({void*_tmp65[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp66="lookup_xtuniondecl: impossible";
_tag_arr(_tmp66,sizeof(char),_get_zero_arr_size(_tmp66,31));}),_tag_arr(_tmp65,
sizeof(void*),0));});return 0;_LL2F: if(_tmp57 <= (void*)1?1:*((int*)_tmp57)!= 0)
goto _LL31;_tmp59=((struct Cyc_Absyn_Rel_n_struct*)_tmp57)->f1;if(_tmp59 == 0)goto
_LL31;_tmp5A=*_tmp59;_tmp5B=(struct _tagged_arr*)_tmp5A.hd;_tmp5C=_tmp5A.tl;_LL30: {
struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp67;_push_handler(& _tmp67);{int
_tmp69=0;if(setjmp(_tmp67.handler))_tmp69=1;if(!_tmp69){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp5B,_tmp5C);;_pop_handler();}else{void*_tmp68=(void*)_exn_thrown;void*
_tmp6B=_tmp68;_LL39: if(_tmp6B != Cyc_Dict_Absent)goto _LL3B;_LL3A:({void*_tmp6C[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp6D="bad qualified name for xtunion";_tag_arr(
_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,31));}),_tag_arr(_tmp6C,sizeof(
void*),0));});(int)_throw((void*)Cyc_Dict_Absent);_LL3B:;_LL3C:(void)_throw(
_tmp6B);_LL38:;}}}return({struct Cyc_Core_Opt*_tmp6E=_cycalloc(sizeof(*_tmp6E));
_tmp6E->v=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,_tmp56);_tmp6E;});}_LL31: if(
_tmp57 <= (void*)1?1:*((int*)_tmp57)!= 1)goto _LL2A;_tmp5D=((struct Cyc_Absyn_Abs_n_struct*)
_tmp57)->f1;_LL32: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5D);return({struct Cyc_Core_Opt*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->v=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,
_tmp56);_tmp6F;});}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->enumdecls,v);}
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp71;void*_tmp72;
struct _tagged_arr*_tmp73;struct _tuple1*_tmp70=q;_tmp71=*_tmp70;_tmp72=_tmp71.f1;
_tmp73=_tmp71.f2;{void*_tmp74=_tmp72;struct Cyc_List_List*_tmp75;struct Cyc_List_List*
_tmp76;struct Cyc_List_List _tmp77;struct _tagged_arr*_tmp78;struct Cyc_List_List*
_tmp79;struct Cyc_List_List*_tmp7A;_LL3E: if((int)_tmp74 != 0)goto _LL40;_LL3F: goto
_LL41;_LL40: if(_tmp74 <= (void*)1?1:*((int*)_tmp74)!= 0)goto _LL42;_tmp75=((struct
Cyc_Absyn_Rel_n_struct*)_tmp74)->f1;if(_tmp75 != 0)goto _LL42;_LL41: return((struct
Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),
struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp73);_LL42: if(_tmp74 <= (void*)1?1:*((int*)_tmp74)!= 0)goto _LL44;_tmp76=((
struct Cyc_Absyn_Rel_n_struct*)_tmp74)->f1;if(_tmp76 == 0)goto _LL44;_tmp77=*_tmp76;
_tmp78=(struct _tagged_arr*)_tmp77.hd;_tmp79=_tmp77.tl;_LL43: {struct Cyc_Tcenv_Genv*
_tmp7B=Cyc_Tcenv_lookup_namespace(te,loc,_tmp78,_tmp79);return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp7B->enumdecls,
_tmp73);}_LL44: if(_tmp74 <= (void*)1?1:*((int*)_tmp74)!= 1)goto _LL3D;_tmp7A=((
struct Cyc_Absyn_Abs_n_struct*)_tmp74)->f1;_LL45: {struct Cyc_Tcenv_Genv*_tmp7C=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp7A);return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp7C->enumdecls,_tmp73);}_LL3D:;}}static
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->typedefs,v);}struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp7E;void*_tmp7F;struct _tagged_arr*_tmp80;
struct _tuple1*_tmp7D=q;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;{void*
_tmp81=_tmp7F;struct Cyc_List_List*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_List_List
_tmp84;struct _tagged_arr*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*
_tmp87;_LL47: if((int)_tmp81 != 0)goto _LL49;_LL48: goto _LL4A;_LL49: if(_tmp81 <= (
void*)1?1:*((int*)_tmp81)!= 0)goto _LL4B;_tmp82=((struct Cyc_Absyn_Rel_n_struct*)
_tmp81)->f1;if(_tmp82 != 0)goto _LL4B;_LL4A: return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*(*
lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp80);_LL4B: if(_tmp81 <= (void*)1?1:*((
int*)_tmp81)!= 0)goto _LL4D;_tmp83=((struct Cyc_Absyn_Rel_n_struct*)_tmp81)->f1;
if(_tmp83 == 0)goto _LL4D;_tmp84=*_tmp83;_tmp85=(struct _tagged_arr*)_tmp84.hd;
_tmp86=_tmp84.tl;_LL4C: {struct Cyc_Tcenv_Genv*_tmp88=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp85,_tmp86);return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp88->typedefs,_tmp80);}_LL4D: if(
_tmp81 <= (void*)1?1:*((int*)_tmp81)!= 1)goto _LL46;_tmp87=((struct Cyc_Absyn_Abs_n_struct*)
_tmp81)->f1;_LL4E: {struct Cyc_Tcenv_Genv*_tmp89=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp87);
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(_tmp89->typedefs,_tmp80);}_LL46:;}}struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(){
return({struct Cyc_Tcenv_Genv*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->namespaces=(
struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Tcutil_empty_var_set))->v;
_tmp8A->aggrdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp8A->tuniondecls=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
_tmp8A->enumdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp8A->typedefs=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
_tmp8A->ordinaries=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp8A->availables=0;_tmp8A;});}
void*Cyc_Tcenv_get_visible(void*f){while(1){void*_tmp8B=f;void*_tmp8C;void*
_tmp8D;void*_tmp8E;void*_tmp8F;_LL50: if(*((int*)_tmp8B)!= 0)goto _LL52;_tmp8C=(
void*)((struct Cyc_Tcenv_Outermost_struct*)_tmp8B)->f1;_LL51: return _tmp8C;_LL52:
if(*((int*)_tmp8B)!= 1)goto _LL54;_tmp8D=(void*)((struct Cyc_Tcenv_Frame_struct*)
_tmp8B)->f1;_LL53: return _tmp8D;_LL54: if(*((int*)_tmp8B)!= 2)goto _LL4F;_tmp8E=(
void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp8B)->f1;_tmp8F=(void*)((struct Cyc_Tcenv_Hidden_struct*)
_tmp8B)->f2;_LL55: f=_tmp8F;goto _LL4F;_LL4F:;}}void*Cyc_Tcenv_put_visible(void*f,
void*x){void*_tmp90=f;void*_tmp91;void*_tmp92;void*_tmp93;void*_tmp94;void*
_tmp95;_LL57: if(*((int*)_tmp90)!= 0)goto _LL59;_tmp91=(void*)((struct Cyc_Tcenv_Outermost_struct*)
_tmp90)->f1;_LL58: return(void*)({struct Cyc_Tcenv_Outermost_struct*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Tcenv_Outermost_struct _tmp97;
_tmp97.tag=0;_tmp97.f1=(void*)x;_tmp97;});_tmp96;});_LL59: if(*((int*)_tmp90)!= 1)
goto _LL5B;_tmp92=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp90)->f1;_tmp93=(void*)((
struct Cyc_Tcenv_Frame_struct*)_tmp90)->f2;_LL5A: return(void*)({struct Cyc_Tcenv_Frame_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Tcenv_Frame_struct _tmp99;
_tmp99.tag=1;_tmp99.f1=(void*)x;_tmp99.f2=(void*)_tmp93;_tmp99;});_tmp98;});
_LL5B: if(*((int*)_tmp90)!= 2)goto _LL56;_tmp94=(void*)((struct Cyc_Tcenv_Hidden_struct*)
_tmp90)->f1;_tmp95=(void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp90)->f2;_LL5C:
return(void*)({struct Cyc_Tcenv_Hidden_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));
_tmp9A[0]=({struct Cyc_Tcenv_Hidden_struct _tmp9B;_tmp9B.tag=2;_tmp9B.f1=(void*)
_tmp94;_tmp9B.f2=(void*)Cyc_Tcenv_put_visible(_tmp95,x);_tmp9B;});_tmp9A;});
_LL56:;}void*Cyc_Tcenv_apply_to_visible(void*(*g)(void*),void*f){return Cyc_Tcenv_put_visible(
f,g(Cyc_Tcenv_get_visible(f)));}void*Cyc_Tcenv_map_frames(void*(*g)(void*),void*
f){void*_tmp9C=f;void*_tmp9D;void*_tmp9E;void*_tmp9F;void*_tmpA0;void*_tmpA1;
_LL5E: if(*((int*)_tmp9C)!= 0)goto _LL60;_tmp9D=(void*)((struct Cyc_Tcenv_Outermost_struct*)
_tmp9C)->f1;_LL5F: return(void*)({struct Cyc_Tcenv_Outermost_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Tcenv_Outermost_struct _tmpA3;
_tmpA3.tag=0;_tmpA3.f1=(void*)g(_tmp9D);_tmpA3;});_tmpA2;});_LL60: if(*((int*)
_tmp9C)!= 1)goto _LL62;_tmp9E=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp9C)->f1;
_tmp9F=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp9C)->f2;_LL61: return(void*)({
struct Cyc_Tcenv_Frame_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_Tcenv_Frame_struct _tmpA5;_tmpA5.tag=1;_tmpA5.f1=(void*)g(_tmp9E);
_tmpA5.f2=(void*)Cyc_Tcenv_map_frames(g,_tmp9F);_tmpA5;});_tmpA4;});_LL62: if(*((
int*)_tmp9C)!= 2)goto _LL5D;_tmpA0=(void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp9C)->f1;
_tmpA1=(void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp9C)->f2;_LL63: return(void*)({
struct Cyc_Tcenv_Hidden_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({
struct Cyc_Tcenv_Hidden_struct _tmpA7;_tmpA7.tag=2;_tmpA7.f1=(void*)g(_tmpA0);
_tmpA7.f2=(void*)Cyc_Tcenv_map_frames(g,_tmpA1);_tmpA7;});_tmpA6;});_LL5D:;}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpA8="get_fenv";
_tag_arr(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,9));}));return((struct Cyc_Tcenv_Fenv*(*)(
void*f))Cyc_Tcenv_get_visible)((void*)((struct Cyc_Core_Opt*)_check_null(te->le))->v);}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*
fe){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*
_tmpA9="put_fenv";_tag_arr(_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,9));}));
return({struct Cyc_Tcenv_Tenv*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->ns=te->ns;
_tmpAA->ae=te->ae;_tmpAA->le=({struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*
_tmpAB));_tmpAB->v=(void*)((void*(*)(void*f,struct Cyc_Tcenv_Fenv*x))Cyc_Tcenv_put_visible)((
void*)((struct Cyc_Core_Opt*)_check_null(te->le))->v,fe);_tmpAB;});_tmpAA;});}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct Cyc_Tcenv_Fenv*f){
return({struct Cyc_Tcenv_Fenv*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=*((
struct Cyc_Tcenv_Fenv*)f);_tmpAC;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_Fenv*_tmpAD=({struct Cyc_Tcenv_Fenv*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=*((struct Cyc_Tcenv_Fenv*)f);_tmpAF;});
_tmpAD->ctrl_env=({struct Cyc_Tcenv_CtrlEnv*_tmpAE=_cycalloc(sizeof(*_tmpAE));
_tmpAE[0]=*((struct Cyc_Tcenv_CtrlEnv*)f->ctrl_env);_tmpAE;});return _tmpAD;}void*
Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpB0="Tcenv: unexpected return_typ";
_tag_arr(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,29));}));return(void*)((
Cyc_Tcenv_get_fenv(te))->shared)->return_typ;}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0)return 0;return(Cyc_Tcenv_get_fenv(te))->type_vars;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){if(te->le == 0)((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpB1="Tcenv: unexpected add_type_vars";
_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,32));}));{struct Cyc_Tcenv_Fenv*
_tmpB2=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpB2->type_vars);Cyc_Tcutil_check_unique_tvars(
loc,_tmpB3);_tmpB2->type_vars=_tmpB3;return Cyc_Tcenv_put_fenv(te,_tmpB2);}}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpB4="Tcenv: unexpected add_local_var";
_tag_arr(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,32));}));{struct Cyc_Tcenv_Fenv*
_tmpB5=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));struct _tagged_arr*
_tmpB6=(*((struct _tuple1*)vd->name)).f2;{void*_tmpB7=(*((struct _tuple1*)vd->name)).f1;
_LL65: if((int)_tmpB7 != 0)goto _LL67;_LL66: goto _LL64;_LL67:;_LL68:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));
_tmpB8[0]=({struct Cyc_Core_Impossible_struct _tmpB9;_tmpB9.tag=Cyc_Core_Impossible;
_tmpB9.f1=({const char*_tmpBA="add_local_var: called with Rel_n";_tag_arr(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,33));});_tmpB9;});_tmpB8;}));_LL64:;}
_tmpB5->locals=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k,void*v))Cyc_Dict_insert)(_tmpB5->locals,_tmpB6,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_Local_b_struct
_tmpBC;_tmpBC.tag=3;_tmpBC.f1=vd;_tmpBC;});_tmpBB;}));return Cyc_Tcenv_put_fenv(
te,_tmpB5);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpBD="Tcenv: unexpected add_pat_var";
_tag_arr(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,30));}));{struct Cyc_Tcenv_Fenv*
_tmpBE=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));struct _tagged_arr*
_tmpBF=(*((struct _tuple1*)vd->name)).f2;_tmpBE->locals=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpBE->locals,
_tmpBF,(void*)({struct Cyc_Absyn_Pat_b_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_Pat_b_struct _tmpC1;_tmpC1.tag=4;_tmpC1.f1=vd;_tmpC1;});
_tmpC0;}));return Cyc_Tcenv_put_fenv(te,_tmpBE);}}void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmpC3;void*_tmpC4;struct _tagged_arr*_tmpC5;struct _tuple1*_tmpC2=q;_tmpC3=*
_tmpC2;_tmpC4=_tmpC3.f1;_tmpC5=_tmpC3.f2;{void*_tmpC6=_tmpC4;struct Cyc_List_List*
_tmpC7;_LL6A: if((int)_tmpC6 != 0)goto _LL6C;_LL6B: if(te->le == 0)(int)_throw((void*)
Cyc_Dict_Absent);goto _LL6D;_LL6C: if(_tmpC6 <= (void*)1?1:*((int*)_tmpC6)!= 0)goto
_LL6E;_tmpC7=((struct Cyc_Absyn_Rel_n_struct*)_tmpC6)->f1;if(_tmpC7 != 0)goto _LL6E;
if(!(te->le != 0))goto _LL6E;_LL6D: {struct Cyc_Tcenv_Fenv*_tmpC8=Cyc_Tcenv_get_fenv(
te);struct _handler_cons _tmpC9;_push_handler(& _tmpC9);{int _tmpCB=0;if(setjmp(
_tmpC9.handler))_tmpCB=1;if(!_tmpCB){{void*_tmpCE=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({struct Cyc_Tcenv_VarRes_struct
_tmpCD;_tmpCD.tag=0;_tmpCD.f1=(void*)((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmpC8->locals,_tmpC5);_tmpCD;});_tmpCC;});
_npop_handler(0);return _tmpCE;};_pop_handler();}else{void*_tmpCA=(void*)
_exn_thrown;void*_tmpD0=_tmpCA;_LL71: if(_tmpD0 != Cyc_Dict_Absent)goto _LL73;_LL72:
return Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL73:;_LL74:(void)_throw(
_tmpD0);_LL70:;}}}_LL6E:;_LL6F: {struct _handler_cons _tmpD1;_push_handler(& _tmpD1);{
int _tmpD3=0;if(setjmp(_tmpD1.handler))_tmpD3=1;if(!_tmpD3){{void*_tmpD4=Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_npop_handler(0);return _tmpD4;};_pop_handler();}else{void*_tmpD2=(void*)
_exn_thrown;void*_tmpD6=_tmpD2;_LL76: if(_tmpD6 != Cyc_Dict_Absent)goto _LL78;_LL77:
return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));
_tmpD7[0]=({struct Cyc_Tcenv_VarRes_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=(void*)((
void*)0);_tmpD8;});_tmpD7;});_LL78:;_LL79:(void)_throw(_tmpD6);_LL75:;}}}_LL69:;}}
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
struct Cyc_Absyn_Stmt**sopt){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmpD9="Tcenv: unexpected process_continue";_tag_arr(_tmpD9,sizeof(
char),_get_zero_arr_size(_tmpD9,35));}));{void*_tmpDA=(void*)((Cyc_Tcenv_get_fenv(
te))->ctrl_env)->continue_stmt;struct Cyc_Absyn_Stmt*_tmpDB;_LL7B: if(_tmpDA <= (
void*)3?1:*((int*)_tmpDA)!= 0)goto _LL7D;_tmpDB=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmpDA)->f1;_LL7C: _tmpDB->non_local_preds=({struct Cyc_List_List*_tmpDC=_cycalloc(
sizeof(*_tmpDC));_tmpDC->hd=s;_tmpDC->tl=_tmpDB->non_local_preds;_tmpDC;});*((
struct Cyc_Absyn_Stmt**)sopt)=(struct Cyc_Absyn_Stmt*)_tmpDB;return;_LL7D: if((int)
_tmpDA != 0)goto _LL7F;_LL7E:({void*_tmpDD[0]={};Cyc_Tcutil_terr(s->loc,({const
char*_tmpDE="continue not in a loop";_tag_arr(_tmpDE,sizeof(char),
_get_zero_arr_size(_tmpDE,23));}),_tag_arr(_tmpDD,sizeof(void*),0));});return;
_LL7F: if((int)_tmpDA != 1)goto _LL81;_LL80: goto _LL82;_LL81: if((int)_tmpDA != 2)goto
_LL7A;_LL82:((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpDF="Tcenv: bad continue destination";
_tag_arr(_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,32));}));_LL7A:;}}void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpE0="Tcenv: unexpected process_break";
_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,32));}));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_get_fenv(te);void*_tmpE1=(void*)(fe->ctrl_env)->break_stmt;struct
Cyc_Absyn_Stmt*_tmpE2;_LL84: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 0)goto _LL86;
_tmpE2=((struct Cyc_Tcenv_Stmt_j_struct*)_tmpE1)->f1;_LL85: _tmpE2->non_local_preds=({
struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->hd=s;_tmpE3->tl=
_tmpE2->non_local_preds;_tmpE3;});*((struct Cyc_Absyn_Stmt**)sopt)=(struct Cyc_Absyn_Stmt*)
_tmpE2;return;_LL86: if((int)_tmpE1 != 0)goto _LL88;_LL87:({void*_tmpE4[0]={};Cyc_Tcutil_terr(
s->loc,({const char*_tmpE5="break not in a loop or switch";_tag_arr(_tmpE5,
sizeof(char),_get_zero_arr_size(_tmpE5,30));}),_tag_arr(_tmpE4,sizeof(void*),0));});
return;_LL88: if((int)_tmpE1 != 2)goto _LL8A;_LL89: if((void*)(fe->shared)->return_typ
!= (void*)0)({void*_tmpE6[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmpE7="break causes function not to return a value";
_tag_arr(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,44));}),_tag_arr(_tmpE6,
sizeof(void*),0));});return;_LL8A: if((int)_tmpE1 != 1)goto _LL83;_LL8B:({void*
_tmpE8[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmpE9="break causes outer switch clause to implicitly fallthru";
_tag_arr(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,56));}),_tag_arr(_tmpE8,
sizeof(void*),0));});return;_LL83:;}}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct _tagged_arr*l,struct Cyc_Absyn_Stmt**sopt){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpEA="Tcenv: unexpected process_goto";
_tag_arr(_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,31));}));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_get_fenv(te);struct Cyc_Core_Opt*_tmpEB=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((fe->shared)->seen_labels,
l);if(_tmpEB == 0){struct Cyc_SlowDict_Dict*_tmpEC=(fe->shared)->needed_labels;
struct Cyc_Core_Opt*_tmpED=((struct Cyc_Core_Opt*(*)(struct Cyc_SlowDict_Dict*d,
struct _tagged_arr*k))Cyc_SlowDict_lookup_opt)(_tmpEC,l);if(_tmpED == 0)_tmpED=({
struct Cyc_Core_Opt*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->v=0;_tmpEE;});(fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)(struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k,struct
Cyc_List_List*v))Cyc_SlowDict_insert)(_tmpEC,l,({struct Cyc_List_List*_tmpEF=
_cycalloc(sizeof(*_tmpEF));_tmpEF->hd=s;_tmpEF->tl=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpED))->v;_tmpEF;}));}else{((struct Cyc_Absyn_Stmt*)
_tmpEB->v)->non_local_preds=({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*
_tmpF0));_tmpF0->hd=s;_tmpF0->tl=((struct Cyc_Absyn_Stmt*)_tmpEB->v)->non_local_preds;
_tmpF0;});*((struct Cyc_Absyn_Stmt**)sopt)=(struct Cyc_Absyn_Stmt*)((struct Cyc_Absyn_Stmt*)
_tmpEB->v);}}}struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){if(te->le == 0)((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmpF1="Tcenv: unexpected process_fallthru";
_tag_arr(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,35));}));{struct Cyc_Tcenv_Fenv*
_tmpF2=Cyc_Tcenv_get_fenv(te);struct _tuple4*_tmpF3=(_tmpF2->ctrl_env)->fallthru_clause;
if(_tmpF3 != 0){(((*((struct _tuple4*)_tmpF3)).f1)->body)->non_local_preds=({
struct Cyc_List_List*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->hd=s;_tmpF4->tl=(((*((
struct _tuple4*)_tmpF3)).f1)->body)->non_local_preds;_tmpF4;});*((struct Cyc_Absyn_Switch_clause***)
clauseopt)=(struct Cyc_Absyn_Switch_clause**)({struct Cyc_Absyn_Switch_clause**
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=(*((struct _tuple4*)_tmpF3)).f1;
_tmpF5;});}return _tmpF3;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*
clause){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const
char*_tmpF6="Tcenv: unexpected set_fallthru";_tag_arr(_tmpF6,sizeof(char),
_get_zero_arr_size(_tmpF6,31));}));{struct Cyc_List_List*_tmpF7=0;for(0;vds != 0;
vds=vds->tl){_tmpF7=({struct Cyc_List_List*_tmpF8=_cycalloc(sizeof(*_tmpF8));
_tmpF8->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_tmpF8->tl=
_tmpF7;_tmpF8;});}_tmpF7=Cyc_List_imp_rev(_tmpF7);{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(fe->ctrl_env)->fallthru_clause=({struct _tuple4*_tmpF9=
_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=clause;_tmpF9->f2=new_tvs;_tmpF9->f3=
_tmpF7;_tmpF9;});return Cyc_Tcenv_put_fenv(te,fe);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmpFA="Tcenv: unexpected clear_fallthru";_tag_arr(_tmpFA,sizeof(char),
_get_zero_arr_size(_tmpFA,33));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(fe->ctrl_env)->fallthru_clause=0;return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*continue_dest){if(te->le == 0)((int(*)(struct _tagged_arr msg))
Cyc_Tcenv_env_err)(({const char*_tmpFB="Tcenv: unexpected set_in_loop";_tag_arr(
_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,30));}));{struct Cyc_Tcenv_Fenv*fe=
Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(te));(void*)((fe->ctrl_env)->continue_stmt=(
void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));
_tmpFC[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=
continue_dest;_tmpFD;});_tmpFC;})));(void*)((fe->ctrl_env)->break_stmt=(void*)((
void*)(fe->ctrl_env)->next_stmt));(void*)((fe->ctrl_env)->next_stmt=(void*)((
void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({
struct Cyc_Tcenv_Stmt_j_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=continue_dest;_tmpFF;});
_tmpFE;})));return Cyc_Tcenv_put_fenv(te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmp100="Tcenv: unexpected enter_try";_tag_arr(_tmp100,sizeof(char),
_get_zero_arr_size(_tmp100,28));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));++(fe->ctrl_env)->try_depth;return Cyc_Tcenv_put_fenv(te,
fe);}}int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmp101="Tcenv: unexpected get_try_depth";
_tag_arr(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,32));}));return((Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct
Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmp102="Tcenv: unexpected set_in_switch";_tag_arr(_tmp102,sizeof(char),
_get_zero_arr_size(_tmp102,32));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(void*)((fe->ctrl_env)->break_stmt=(void*)((void*)(fe->ctrl_env)->next_stmt));(
void*)((fe->ctrl_env)->next_stmt=(void*)((void*)1));return Cyc_Tcenv_put_fenv(te,
fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*te,void*j){
if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*
_tmp103="Tcenv: unexpected set_next";_tag_arr(_tmp103,sizeof(char),
_get_zero_arr_size(_tmp103,27));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(void*)((fe->ctrl_env)->next_stmt=(void*)j);return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct
_tagged_arr*v,struct Cyc_Absyn_Stmt*s){if(te->le == 0)((int(*)(struct _tagged_arr
msg))Cyc_Tcenv_env_err)(({const char*_tmp104="Tcenv: unexpected add_label";
_tag_arr(_tmp104,sizeof(char),_get_zero_arr_size(_tmp104,28));}));{struct Cyc_Tcenv_Fenv*
_tmp105=Cyc_Tcenv_get_fenv(te);struct Cyc_SlowDict_Dict*_tmp106=(_tmp105->shared)->needed_labels;
struct Cyc_Core_Opt*_tmp107=((struct Cyc_Core_Opt*(*)(struct Cyc_SlowDict_Dict*d,
struct _tagged_arr*k))Cyc_SlowDict_lookup_opt)(_tmp106,v);if(_tmp107 != 0){(
_tmp105->shared)->needed_labels=((struct Cyc_SlowDict_Dict*(*)(struct Cyc_SlowDict_Dict*
d,struct _tagged_arr*k))Cyc_SlowDict_delete)(_tmp106,v);{struct Cyc_List_List*
_tmp108=(struct Cyc_List_List*)_tmp107->v;s->non_local_preds=_tmp108;for(0;
_tmp108 != 0;_tmp108=_tmp108->tl){void*_tmp109=(void*)((struct Cyc_Absyn_Stmt*)
_tmp108->hd)->r;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_Absyn_Stmt**_tmp10B;_LL8D:
if(_tmp109 <= (void*)1?1:*((int*)_tmp109)!= 7)goto _LL8F;_tmp10A=((struct Cyc_Absyn_Goto_s_struct*)
_tmp109)->f2;_tmp10B=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)
_tmp109)->f2;_LL8E:*((struct Cyc_Absyn_Stmt**)_tmp10B)=(struct Cyc_Absyn_Stmt*)s;
goto _LL8C;_LL8F:;_LL90:((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const
char*_tmp10C="Tcenv: add_label backpatching of non-goto";_tag_arr(_tmp10C,
sizeof(char),_get_zero_arr_size(_tmp10C,42));}));goto _LL8C;_LL8C:;}}}if(((int(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)((_tmp105->shared)->seen_labels,
v))({struct Cyc_String_pa_struct _tmp10F;_tmp10F.tag=0;_tmp10F.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)v));{void*_tmp10D[1]={&
_tmp10F};Cyc_Tcutil_terr(s->loc,({const char*_tmp10E="Repeated label: %s";
_tag_arr(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,19));}),_tag_arr(
_tmp10D,sizeof(void*),1));}});(_tmp105->shared)->seen_labels=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)((
_tmp105->shared)->seen_labels,v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){return((int(*)(struct Cyc_SlowDict_Dict*d))Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv(te))->shared)->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmp110="Tcenv: unexpected get_encloser";_tag_arr(_tmp110,sizeof(char),
_get_zero_arr_size(_tmp110,31));}));return(Cyc_Tcenv_get_fenv(te))->encloser;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*
_tmp111="Tcenv: unexpected set_encloser";_tag_arr(_tmp111,sizeof(char),
_get_zero_arr_size(_tmp111,31));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv(te));fe->encloser=s;return Cyc_Tcenv_put_fenv(te,fe);}}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r,int resetable){
if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*
_tmp112="Tcenv::add_region: unexpected add_region";_tag_arr(_tmp112,sizeof(char),
_get_zero_arr_size(_tmp112,41));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv(te));struct Cyc_Absyn_Tvar*tv;{void*_tmp113=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmp114;_LL92: if(_tmp113 <= (void*)3?1:*((int*)_tmp113)!= 
1)goto _LL94;_tmp114=((struct Cyc_Absyn_VarType_struct*)_tmp113)->f1;_LL93: tv=
_tmp114;goto _LL91;_LL94:;_LL95: tv=((struct Cyc_Absyn_Tvar*(*)(struct _tagged_arr
msg))Cyc_Tcenv_env_err)(({const char*_tmp115="bad add region";_tag_arr(_tmp115,
sizeof(char),_get_zero_arr_size(_tmp115,15));}));goto _LL91;_LL91:;}fe->region_order=
Cyc_RgnOrder_add_youngest(fe->region_order,tv,resetable);(void*)(fe->capability=(
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp116=_cycalloc(sizeof(*_tmp116));
_tmp116[0]=({struct Cyc_Absyn_JoinEff_struct _tmp117;_tmp117.tag=20;_tmp117.f1=({
struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));
_tmp11A[0]=({struct Cyc_Absyn_AccessEff_struct _tmp11B;_tmp11B.tag=19;_tmp11B.f1=(
void*)r;_tmp11B;});_tmp11A;}));_tmp118->tl=({struct Cyc_List_List*_tmp119=
_cycalloc(sizeof(*_tmp119));_tmp119->hd=(void*)((void*)fe->capability);_tmp119->tl=
0;_tmp119;});_tmp118;});_tmp117;});_tmp116;})));return Cyc_Tcenv_put_fenv(te,fe);}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmp11C="Tcenv: unexpected new_block";
_tag_arr(_tmp11C,sizeof(char),_get_zero_arr_size(_tmp11C,28));}));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));void*block_typ=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({
struct Cyc_Absyn_VarType_struct _tmp125;_tmp125.tag=1;_tmp125.f1=block_rgn;_tmp125;});
_tmp124;});fe->type_vars=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));
_tmp11D->hd=block_rgn;_tmp11D->tl=fe->type_vars;_tmp11D;});Cyc_Tcutil_check_unique_tvars(
loc,fe->type_vars);fe->region_order=Cyc_RgnOrder_add_youngest(fe->region_order,
block_rgn,0);(void*)(fe->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp11F;_tmp11F.tag=20;_tmp11F.f1=({struct Cyc_List_List*_tmp120=_cycalloc(
sizeof(*_tmp120));_tmp120->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp123;_tmp123.tag=19;_tmp123.f1=(void*)block_typ;_tmp123;});_tmp122;}));
_tmp120->tl=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=(
void*)((void*)fe->capability);_tmp121->tl=0;_tmp121;});_tmp120;});_tmp11F;});
_tmp11E;})));(void*)(fe->curr_rgn=(void*)block_typ);return Cyc_Tcenv_put_fenv(te,
fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(void*)((void*)3);
_tmp127;});_tmp126;}));Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(
loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*cs){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmp128="Tcenv: unexpected new_outlives_constraints";_tag_arr(_tmp128,
sizeof(char),_get_zero_arr_size(_tmp128,43));}));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv(te));struct Cyc_RgnOrder_RgnPO*_tmp129=fe->region_order;for(0;
cs != 0;cs=cs->tl){_tmp129=Cyc_RgnOrder_add_outlives_constraint(_tmp129,(*((
struct _tuple0*)((struct _tuple0*)cs->hd))).f1,(*((struct _tuple0*)((struct _tuple0*)
cs->hd))).f2);}fe->region_order=_tmp129;return Cyc_Tcenv_put_fenv(te,fe);}}void*
Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)return(void*)2;return(
void*)(Cyc_Tcenv_get_fenv(te))->curr_rgn;}void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){if(te->le == 0)((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmp12A="Tcenv: unexpected check_rgn_accessible";
_tag_arr(_tmp12A,sizeof(char),_get_zero_arr_size(_tmp12A,39));}));{struct Cyc_Tcenv_Fenv*
_tmp12B=Cyc_Tcenv_get_fenv(te);void*_tmp12C=(void*)_tmp12B->capability;if(Cyc_Tcutil_region_in_effect(
0,rgn,_tmp12C)?1: Cyc_Tcutil_region_in_effect(1,rgn,_tmp12C))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp12B->region_order,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp12D=
_cycalloc(sizeof(*_tmp12D));_tmp12D[0]=({struct Cyc_Absyn_AccessEff_struct _tmp12E;
_tmp12E.tag=19;_tmp12E.f1=(void*)rgn;_tmp12E;});_tmp12D;}),_tmp12C))return;({
struct Cyc_String_pa_struct _tmp131;_tmp131.tag=0;_tmp131.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(rgn));{void*_tmp12F[1]={& _tmp131};Cyc_Tcutil_terr(
loc,({const char*_tmp130="Expression accesses unavailable region %s";_tag_arr(
_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,42));}),_tag_arr(_tmp12F,sizeof(
void*),1));}});}}void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*rgn){if(te->le == 0)((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmp132="Tcenv: unexpected check_rgn_resetable";
_tag_arr(_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,38));}));Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);{void*_tmp133=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp134;
_LL97: if(_tmp133 <= (void*)3?1:*((int*)_tmp133)!= 1)goto _LL99;_tmp134=((struct Cyc_Absyn_VarType_struct*)
_tmp133)->f1;_LL98: if(!Cyc_RgnOrder_is_region_resetable((Cyc_Tcenv_get_fenv(te))->region_order,
_tmp134))({struct Cyc_String_pa_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(rgn));{void*_tmp135[1]={&
_tmp137};Cyc_Tcutil_terr(loc,({const char*_tmp136="Region %s is not resetable";
_tag_arr(_tmp136,sizeof(char),_get_zero_arr_size(_tmp136,27));}),_tag_arr(
_tmp135,sizeof(void*),1));}});return;_LL99:;_LL9A:({void*_tmp138[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp139="check_rgn_resetable";
_tag_arr(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,20));}),_tag_arr(
_tmp138,sizeof(void*),0));});_LL96:;}}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*
te,void*rt_a,void*rt_b){if(te->le == 0)return rt_a == (void*)2;return Cyc_RgnOrder_effect_outlives((
Cyc_Tcenv_get_fenv(te))->region_order,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp13B;_tmp13B.tag=19;_tmp13B.f1=(void*)rt_a;_tmp13B;});_tmp13A;}),rt_b);}
struct _tuple6{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({
const char*_tmp13C="Tcenv: unexpected check_effect_accessible";_tag_arr(_tmp13C,
sizeof(char),_get_zero_arr_size(_tmp13C,42));}));{struct Cyc_Tcenv_Fenv*_tmp13D=
Cyc_Tcenv_get_fenv(te);void*_tmp13E=(void*)_tmp13D->capability;if(Cyc_Tcutil_subset_effect(
0,eff,_tmp13E))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp13D->region_order,eff,
_tmp13E))return;(_tmp13D->shared)->delayed_effect_checks=({struct Cyc_List_List*
_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->hd=({struct _tuple6*_tmp140=
_cycalloc(sizeof(*_tmp140));_tmp140->f1=_tmp13E;_tmp140->f2=eff;_tmp140->f3=
_tmp13D->region_order;_tmp140->f4=loc;_tmp140;});_tmp13F->tl=(_tmp13D->shared)->delayed_effect_checks;
_tmp13F;});}}void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmp141="Tcenv: unexpected check_delayed_effects";
_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,40));}));{struct Cyc_List_List*
_tmp142=((Cyc_Tcenv_get_fenv(te))->shared)->delayed_effect_checks;for(0;_tmp142
!= 0;_tmp142=_tmp142->tl){struct _tuple6 _tmp144;void*_tmp145;void*_tmp146;struct
Cyc_RgnOrder_RgnPO*_tmp147;struct Cyc_Position_Segment*_tmp148;struct _tuple6*
_tmp143=(struct _tuple6*)_tmp142->hd;_tmp144=*_tmp143;_tmp145=_tmp144.f1;_tmp146=
_tmp144.f2;_tmp147=_tmp144.f3;_tmp148=_tmp144.f4;if(Cyc_Tcutil_subset_effect(1,
_tmp146,_tmp145))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp147,_tmp146,
_tmp145))continue;({struct Cyc_String_pa_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp146));{struct
Cyc_String_pa_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_typ2string(_tmp145));{void*_tmp149[2]={& _tmp14B,& _tmp14C};
Cyc_Tcutil_terr(_tmp148,({const char*_tmp14A="Capability \n%s\ndoes not cover function's effect\n%s";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,51));}),_tag_arr(
_tmp149,sizeof(void*),2));}}});}}}struct _tuple7{struct Cyc_RgnOrder_RgnPO*f1;
struct Cyc_List_List*f2;struct Cyc_Position_Segment*f3;};void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){
if(te->le == 0){for(0;po != 0;po=po->tl){if(!Cyc_Tcutil_subset_effect(1,(*((struct
_tuple0*)((struct _tuple0*)po->hd))).f1,Cyc_Absyn_empty_effect)?1: !Cyc_Tcutil_subset_effect(
1,(*((struct _tuple0*)((struct _tuple0*)po->hd))).f2,Cyc_Absyn_empty_effect))({
void*_tmp14D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp14E="the required region ordering is not satisfied here";
_tag_arr(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,51));}),_tag_arr(
_tmp14D,sizeof(void*),0));});}return;}{struct Cyc_Tcenv_Fenv*_tmp14F=Cyc_Tcenv_get_fenv(
te);if(!Cyc_RgnOrder_satisfies_constraints(_tmp14F->region_order,po,(void*)2,0))(
_tmp14F->shared)->delayed_constraint_checks=({struct Cyc_List_List*_tmp150=
_cycalloc(sizeof(*_tmp150));_tmp150->hd=({struct _tuple7*_tmp151=_cycalloc(
sizeof(*_tmp151));_tmp151->f1=_tmp14F->region_order;_tmp151->f2=po;_tmp151->f3=
loc;_tmp151;});_tmp150->tl=(_tmp14F->shared)->delayed_constraint_checks;_tmp150;});}}
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(({const char*_tmp152="Tcenv: unexpected check_delayed_constraints";
_tag_arr(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,44));}));{struct Cyc_List_List*
_tmp153=((Cyc_Tcenv_get_fenv(te))->shared)->delayed_constraint_checks;for(0;
_tmp153 != 0;_tmp153=_tmp153->tl){struct _tuple7 _tmp155;struct Cyc_RgnOrder_RgnPO*
_tmp156;struct Cyc_List_List*_tmp157;struct Cyc_Position_Segment*_tmp158;struct
_tuple7*_tmp154=(struct _tuple7*)_tmp153->hd;_tmp155=*_tmp154;_tmp156=_tmp155.f1;
_tmp157=_tmp155.f2;_tmp158=_tmp155.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp156,_tmp157,(void*)2,1))({void*_tmp159[0]={};Cyc_Tcutil_terr(_tmp158,({const
char*_tmp15A="the required region ordering is not satisfied here";_tag_arr(
_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,51));}),_tag_arr(_tmp159,sizeof(
void*),0));});}}}struct _tuple8{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Fndecl*fd){struct Cyc_Dict_Dict*locals=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp186=_cycalloc(sizeof(*
_tmp186));_tmp186->name=({struct _tagged_arr*_tmp189=_cycalloc(sizeof(*_tmp189));
_tmp189[0]=(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp18C;_tmp18C.tag=0;
_tmp18C.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)(*((
struct _tuple1*)fd->name)).f2));{void*_tmp18A[1]={& _tmp18C};Cyc_aprintf(({const
char*_tmp18B="`%s";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,4));}),
_tag_arr(_tmp18A,sizeof(void*),1));}});_tmp189;});_tmp186->identity=Cyc_Tcutil_new_tvar_id();
_tmp186->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp187=_cycalloc(
sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp188;_tmp188.tag=0;
_tmp188.f1=(void*)((void*)3);_tmp188;});_tmp187;}));_tmp186;});struct Cyc_List_List*
_tmp15B=({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->hd=
rgn0;_tmp185->tl=fd->tvs;_tmp185;});Cyc_Tcutil_check_unique_tvars(loc,_tmp15B);{
struct Cyc_RgnOrder_RgnPO*_tmp15C=Cyc_RgnOrder_initial_fn_po(fd->tvs,fd->rgn_po,(
void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0);void*param_rgn=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp183=_cycalloc(sizeof(*_tmp183));
_tmp183[0]=({struct Cyc_Absyn_VarType_struct _tmp184;_tmp184.tag=1;_tmp184.f1=rgn0;
_tmp184;});_tmp183;});struct Cyc_List_List*_tmp15D=0;{struct Cyc_List_List*_tmp15E=
fd->args;for(0;_tmp15E != 0;_tmp15E=_tmp15E->tl){struct Cyc_Absyn_Vardecl*_tmp15F=({
struct Cyc_Absyn_Vardecl*_tmp164=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);
_tmp164[0]=({struct Cyc_Absyn_Vardecl _tmp165;_tmp165.sc=(void*)((void*)2);_tmp165.name=({
struct _tuple1*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->f1=(void*)0;_tmp167->f2=(*((
struct _tuple8*)((struct _tuple8*)_tmp15E->hd))).f1;_tmp167;});_tmp165.tq=(*((
struct _tuple8*)((struct _tuple8*)_tmp15E->hd))).f2;_tmp165.type=(void*)(*((struct
_tuple8*)((struct _tuple8*)_tmp15E->hd))).f3;_tmp165.initializer=0;_tmp165.rgn=({
struct Cyc_Core_Opt*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->v=(void*)
param_rgn;_tmp166;});_tmp165.attributes=0;_tmp165.escapes=0;_tmp165;});_tmp164;});
_tmp15D=({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(struct Cyc_List_List)* 1);
_tmp160[0]=({struct Cyc_List_List _tmp161;_tmp161.hd=_tmp15F;_tmp161.tl=_tmp15D;
_tmp161;});_tmp160;});locals=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(locals,(*((struct _tuple8*)((struct
_tuple8*)_tmp15E->hd))).f1,(void*)({struct Cyc_Absyn_Param_b_struct*_tmp162=
_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_Absyn_Param_b_struct _tmp163;
_tmp163.tag=2;_tmp163.f1=_tmp15F;_tmp163;});_tmp162;}));}}if(fd->cyc_varargs != 0){
struct Cyc_Core_Opt*_tmp169;struct Cyc_Absyn_Tqual _tmp16A;void*_tmp16B;int _tmp16C;
struct Cyc_Absyn_VarargInfo _tmp168=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));
_tmp169=_tmp168.name;_tmp16A=_tmp168.tq;_tmp16B=(void*)_tmp168.type;_tmp16C=
_tmp168.inject;if(_tmp169 != 0){void*_tmp16D=Cyc_Absyn_tagged_typ(_tmp16B,
param_rgn,_tmp16A,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*_tmp16E=({
struct Cyc_Absyn_Vardecl*_tmp172=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);
_tmp172[0]=({struct Cyc_Absyn_Vardecl _tmp173;_tmp173.sc=(void*)((void*)2);_tmp173.name=({
struct _tuple1*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->f1=(void*)0;_tmp175->f2=(
struct _tagged_arr*)_tmp169->v;_tmp175;});_tmp173.tq=Cyc_Absyn_empty_tqual();
_tmp173.type=(void*)_tmp16D;_tmp173.initializer=0;_tmp173.rgn=({struct Cyc_Core_Opt*
_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->v=(void*)param_rgn;_tmp174;});
_tmp173.attributes=0;_tmp173.escapes=0;_tmp173;});_tmp172;});_tmp15D=({struct Cyc_List_List*
_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=_tmp16E;_tmp16F->tl=_tmp15D;
_tmp16F;});locals=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(locals,(struct _tagged_arr*)_tmp169->v,(
void*)({struct Cyc_Absyn_Param_b_struct*_tmp170=_cycalloc(sizeof(*_tmp170));
_tmp170[0]=({struct Cyc_Absyn_Param_b_struct _tmp171;_tmp171.tag=2;_tmp171.f1=
_tmp16E;_tmp171;});_tmp170;}));}else{({void*_tmp176[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp177="missing name for varargs";_tag_arr(_tmp177,sizeof(char),
_get_zero_arr_size(_tmp177,25));}),_tag_arr(_tmp176,sizeof(void*),0));});}}fd->param_vardecls=({
struct Cyc_Core_Opt*_tmp178=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp178[0]=({
struct Cyc_Core_Opt _tmp179;_tmp179.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp15D);_tmp179;});_tmp178;});return({struct Cyc_Tcenv_Fenv*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->shared=({struct Cyc_Tcenv_SharedFenv*
_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->return_typ=(void*)((void*)fd->ret_type);
_tmp182->seen_labels=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp182->needed_labels=((
struct Cyc_SlowDict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))
Cyc_SlowDict_empty)(Cyc_strptrcmp);_tmp182->delayed_effect_checks=0;_tmp182->delayed_constraint_checks=
0;_tmp182;});_tmp17A->type_vars=_tmp15B;_tmp17A->region_order=_tmp15C;_tmp17A->locals=
locals;_tmp17A->encloser=fd->body;_tmp17A->ctrl_env=({struct Cyc_Tcenv_CtrlEnv*
_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->continue_stmt=(void*)((void*)0);
_tmp181->break_stmt=(void*)((void*)0);_tmp181->fallthru_clause=0;_tmp181->next_stmt=(
void*)((void*)2);_tmp181->try_depth=0;_tmp181;});_tmp17A->capability=(void*)((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));
_tmp17B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp17C;_tmp17C.tag=20;_tmp17C.f1=({
struct Cyc_List_List*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));
_tmp17F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp180;_tmp180.tag=19;_tmp180.f1=(
void*)param_rgn;_tmp180;});_tmp17F;}));_tmp17D->tl=({struct Cyc_List_List*_tmp17E=
_cycalloc(sizeof(*_tmp17E));_tmp17E->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp17E->tl=0;_tmp17E;});_tmp17D;});_tmp17C;});
_tmp17B;}));_tmp17A->curr_rgn=(void*)param_rgn;_tmp17A;});}}
