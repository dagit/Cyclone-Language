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
s1,struct Cyc_Set_Set*s2);extern char Cyc_Set_Absent[11];typedef struct{int __count;
union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{
int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);int Cyc_strptrcmp(struct _tagged_arr*
s1,struct _tagged_arr*s2);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct
Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);struct _tagged_arr*Cyc_Absyn_fieldname(
int);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
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
struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Set_Set**all_changed,
struct _tuple0 pr1,struct _tuple0 pr2);void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;void*flow;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2.flow=(void*)((void*)0);_tmp2;});_tmp1;});_tmp0;})));}
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
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
static void*Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*env,
void*flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){void*_tmp4B=flow;struct
Cyc_Dict_Dict*_tmp4C;struct Cyc_List_List*_tmp4D;_LL29: if((int)_tmp4B != 0)goto
_LL2B;_LL2A: return flow;_LL2B: if(_tmp4B <= (void*)1?1:*((int*)_tmp4B)!= 0)goto
_LL28;_tmp4C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f1;_tmp4D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f2;_LL2C: {void*_tmp4E=r2;
struct Cyc_List_List*_tmp4F;_LL2E: if(_tmp4E <= (void*)3?1:*((int*)_tmp4E)!= 3)goto
_LL30;_tmp4F=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp4E)->f1;_LL2F: {void*
_tmp50=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp51;_LL33: if((int)_tmp50 != 0)goto _LL35;_LL34: return flow;_LL35: if(_tmp50 <= (
void*)1?1:*((int*)_tmp50)!= 0)goto _LL32;_tmp51=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp50)->f1;_LL36: {struct Cyc_List_List*new_cl;{void*_tmp52=r1;struct Cyc_List_List*
_tmp53;void*_tmp54;_LL38: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 3)goto _LL3A;
_tmp53=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp52)->f1;_LL39: new_cl=_tmp53;
goto _LL37;_LL3A: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 0)goto _LL3C;_tmp54=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp52)->f1;if((int)_tmp54 != 2)
goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((int)_tmp52 != 0)goto _LL3E;_LL3D: goto _LL3F;
_LL3E: if((int)_tmp52 != 1)goto _LL40;_LL3F: new_cl=0;goto _LL37;_LL40:;_LL41: return
flow;_LL37:;}for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){void*new_cmp;{struct _tuple0
_tmp56=({struct _tuple0 _tmp55;_tmp55.f1=p;_tmp55.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4F->hd)->cmp;_tmp55;});void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;_LL43:
_tmp57=_tmp56.f1;if((int)_tmp57 != 8)goto _LL45;_LL44: goto _LL46;_LL45: _tmp58=
_tmp56.f2;if((int)_tmp58 != 8)goto _LL47;_LL46: new_cmp=(void*)8;goto _LL42;_LL47:
_tmp59=_tmp56.f1;if((int)_tmp59 != 5)goto _LL49;_tmp5A=_tmp56.f2;if((int)_tmp5A != 
5)goto _LL49;_LL48: new_cmp=(void*)5;goto _LL42;_LL49:;_LL4A: new_cmp=(void*)10;goto
_LL42;_LL42:;}new_cl=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));
_tmp5B->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp5C=_cycalloc(sizeof(*_tmp5C));
_tmp5C->cmp=(void*)new_cmp;_tmp5C->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4F->hd)->bd);_tmp5C;});_tmp5B->tl=new_cl;_tmp5B;});}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp5E;_tmp5E.tag=0;_tmp5E.f1=Cyc_CfFlowInfo_assign_place(e1->loc,_tmp4C,env->all_changed,
_tmp51,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5F=_cycalloc(sizeof(*
_tmp5F));_tmp5F[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmp60;_tmp60.tag=3;
_tmp60.f1=new_cl;_tmp60;});_tmp5F;}));_tmp5E.f2=_tmp4D;_tmp5E;});_tmp5D;});}
_LL32:;}_LL30:;_LL31: return flow;_LL2D:;}_LL28:;}static struct _tuple0 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,
void*r){void*_tmp61=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp62;void*_tmp63;struct Cyc_Absyn_PtrAtts
_tmp64;struct Cyc_Absyn_Conref*_tmp65;struct Cyc_Absyn_Conref*_tmp66;_LL4C: if(
_tmp61 <= (void*)3?1:*((int*)_tmp61)!= 4)goto _LL4E;_tmp62=((struct Cyc_Absyn_PointerType_struct*)
_tmp61)->f1;_tmp63=(void*)_tmp62.elt_typ;_tmp64=_tmp62.ptr_atts;_tmp65=_tmp64.bounds;
_tmp66=_tmp64.zero_term;_LL4D: {void*_tmp67=f;struct Cyc_Dict_Dict*_tmp68;struct
Cyc_List_List*_tmp69;_LL51: if((int)_tmp67 != 0)goto _LL53;_LL52: return({struct
_tuple0 _tmp6A;_tmp6A.f1=f;_tmp6A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp63,Cyc_CfFlowInfo_unknown_all);
_tmp6A;});_LL53: if(_tmp67 <= (void*)1?1:*((int*)_tmp67)!= 0)goto _LL50;_tmp68=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp67)->f1;_tmp69=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp67)->f2;_LL54: if(Cyc_Tcutil_is_bound_one(_tmp65)){void*_tmp6B=r;struct Cyc_CfFlowInfo_Place*
_tmp6C;void*_tmp6D;_LL56: if((int)_tmp6B != 1)goto _LL58;_LL57: goto _LL59;_LL58: if((
int)_tmp6B != 2)goto _LL5A;_LL59:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp6F;_tmp6F.tag=Cyc_CfFlowInfo_NotZero;_tmp6F.f1=_tmp69;_tmp6F;});_tmp6E;})));
goto _LL55;_LL5A: if(_tmp6B <= (void*)3?1:*((int*)_tmp6B)!= 2)goto _LL5C;_tmp6C=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp6B)->f1;_LL5B:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp71;_tmp71.tag=Cyc_CfFlowInfo_NotZero;
_tmp71.f1=_tmp69;_tmp71;});_tmp70;})));return({struct _tuple0 _tmp72;_tmp72.f1=f;
_tmp72.f2=Cyc_CfFlowInfo_lookup_place(_tmp68,_tmp6C);_tmp72;});_LL5C: if((int)
_tmp6B != 0)goto _LL5E;_LL5D:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple0 _tmp73;_tmp73.f1=(void*)0;_tmp73.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp63,Cyc_CfFlowInfo_unknown_all);_tmp73;});_LL5E: if(_tmp6B <= (void*)3?1:*((int*)
_tmp6B)!= 0)goto _LL60;_tmp6D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp6B)->f1;_LL5F: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp6D);goto _LL61;
_LL60:;_LL61:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp75;_tmp75.tag=Cyc_CfFlowInfo_UnknownZ;_tmp75.f1=_tmp69;_tmp75;});_tmp74;})));
_LL55:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp77;_tmp77.tag=Cyc_CfFlowInfo_UnknownZ;_tmp77.f1=_tmp69;_tmp77;});_tmp76;})));}{
void*_tmp78=Cyc_CfFlowInfo_initlevel(_tmp68,r);_LL63: if((int)_tmp78 != 0)goto
_LL65;_LL64:({void*_tmp79[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp7A="dereference of possibly uninitialized pointer";
_tag_arr(_tmp7A,sizeof(char),_get_zero_arr_size(_tmp7A,46));}),_tag_arr(_tmp79,
sizeof(void*),0));});goto _LL66;_LL65: if((int)_tmp78 != 2)goto _LL67;_LL66: return({
struct _tuple0 _tmp7B;_tmp7B.f1=f;_tmp7B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp63,
Cyc_CfFlowInfo_unknown_all);_tmp7B;});_LL67: if((int)_tmp78 != 1)goto _LL62;_LL68:
return({struct _tuple0 _tmp7C;_tmp7C.f1=f;_tmp7C.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp63,Cyc_CfFlowInfo_unknown_none);_tmp7C;});_LL62:;}_LL50:;}_LL4E:;_LL4F:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));
_tmp7D[0]=({struct Cyc_Core_Impossible_struct _tmp7E;_tmp7E.tag=Cyc_Core_Impossible;
_tmp7E.f1=({const char*_tmp7F="right deref of non-pointer-type";_tag_arr(_tmp7F,
sizeof(char),_get_zero_arr_size(_tmp7F,32));});_tmp7E;});_tmp7D;}));_LL4B:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp80=(void*)e1->r;
void*_tmp81;struct Cyc_Absyn_Vardecl*_tmp82;void*_tmp83;struct Cyc_Absyn_Vardecl*
_tmp84;void*_tmp85;struct Cyc_Absyn_Vardecl*_tmp86;void*_tmp87;struct Cyc_Absyn_Vardecl*
_tmp88;_LL6A: if(*((int*)_tmp80)!= 1)goto _LL6C;_tmp81=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp80)->f2;if(_tmp81 <= (void*)1?1:*((int*)_tmp81)!= 4)goto _LL6C;_tmp82=((struct
Cyc_Absyn_Pat_b_struct*)_tmp81)->f1;_LL6B: _tmp84=_tmp82;goto _LL6D;_LL6C: if(*((
int*)_tmp80)!= 1)goto _LL6E;_tmp83=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp80)->f2;
if(_tmp83 <= (void*)1?1:*((int*)_tmp83)!= 3)goto _LL6E;_tmp84=((struct Cyc_Absyn_Local_b_struct*)
_tmp83)->f1;_LL6D: _tmp86=_tmp84;goto _LL6F;_LL6E: if(*((int*)_tmp80)!= 1)goto _LL70;
_tmp85=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp80)->f2;if(_tmp85 <= (void*)1?1:*((
int*)_tmp85)!= 2)goto _LL70;_tmp86=((struct Cyc_Absyn_Param_b_struct*)_tmp85)->f1;
_LL6F: _tmp88=_tmp86;goto _LL71;_LL70: if(*((int*)_tmp80)!= 1)goto _LL72;_tmp87=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp80)->f2;if(_tmp87 <= (void*)1?1:*((int*)
_tmp87)!= 0)goto _LL72;_tmp88=((struct Cyc_Absyn_Global_b_struct*)_tmp87)->f1;
_LL71: if(!_tmp88->escapes){void*_tmp89=(void*)e2->r;void*_tmp8A;struct Cyc_Absyn_Vardecl*
_tmp8B;void*_tmp8C;struct Cyc_Absyn_Vardecl*_tmp8D;void*_tmp8E;struct Cyc_Absyn_Vardecl*
_tmp8F;void*_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;_LL75: if(*((int*)_tmp89)!= 1)
goto _LL77;_tmp8A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8A <= (
void*)1?1:*((int*)_tmp8A)!= 4)goto _LL77;_tmp8B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp8A)->f1;_LL76: _tmp8D=_tmp8B;goto _LL78;_LL77: if(*((int*)_tmp89)!= 1)goto _LL79;
_tmp8C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8C <= (void*)1?1:*((
int*)_tmp8C)!= 3)goto _LL79;_tmp8D=((struct Cyc_Absyn_Local_b_struct*)_tmp8C)->f1;
_LL78: _tmp8F=_tmp8D;goto _LL7A;_LL79: if(*((int*)_tmp89)!= 1)goto _LL7B;_tmp8E=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp8E <= (void*)1?1:*((int*)
_tmp8E)!= 2)goto _LL7B;_tmp8F=((struct Cyc_Absyn_Param_b_struct*)_tmp8E)->f1;_LL7A:
_tmp91=_tmp8F;goto _LL7C;_LL7B: if(*((int*)_tmp89)!= 1)goto _LL7D;_tmp90=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp90 <= (void*)1?1:*((int*)_tmp90)
!= 0)goto _LL7D;_tmp91=((struct Cyc_Absyn_Global_b_struct*)_tmp90)->f1;_LL7C: if(!
_tmp91->escapes){int found=0;{struct Cyc_List_List*_tmp92=relns;for(0;_tmp92 != 0;
_tmp92=_tmp92->tl){struct Cyc_CfFlowInfo_Reln*_tmp93=(struct Cyc_CfFlowInfo_Reln*)
_tmp92->hd;if(_tmp93->vd == _tmp91){void*_tmp94=(void*)_tmp93->rop;struct Cyc_Absyn_Vardecl*
_tmp95;_LL80: if(*((int*)_tmp94)!= 2)goto _LL82;_tmp95=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp94)->f1;if(!(_tmp95 == _tmp88))goto _LL82;_LL81: return relns;_LL82:;_LL83:
continue;_LL7F:;}}}if(!found)return({struct Cyc_List_List*_tmp96=_cycalloc(
sizeof(*_tmp96));_tmp96->hd=({struct Cyc_CfFlowInfo_Reln*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->vd=_tmp91;_tmp97->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp99;_tmp99.tag=2;_tmp99.f1=_tmp88;_tmp99;});_tmp98;}));_tmp97;});_tmp96->tl=
relns;_tmp96;});}return relns;_LL7D:;_LL7E: return relns;_LL74:;}else{return relns;}
_LL72:;_LL73: return relns;_LL69:;}struct _tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;{void*_tmp9A=inflow;struct Cyc_Dict_Dict*_tmp9B;struct Cyc_List_List*_tmp9C;
_LL85: if((int)_tmp9A != 0)goto _LL87;_LL86: return({struct _tuple0 _tmp9D;_tmp9D.f1=(
void*)0;_tmp9D.f2=Cyc_CfFlowInfo_unknown_all;_tmp9D;});_LL87: if(_tmp9A <= (void*)
1?1:*((int*)_tmp9A)!= 0)goto _LL84;_tmp9B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp9A)->f1;_tmp9C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp9A)->f2;_LL88:
d=_tmp9B;relns=_tmp9C;_LL84:;}{void*_tmp9E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;void*_tmpA2;void*
_tmpA3;void*_tmpA4;int _tmpA5;void*_tmpA6;void*_tmpA7;void*_tmpA8;struct Cyc_List_List*
_tmpA9;void*_tmpAA;void*_tmpAB;struct Cyc_Absyn_Vardecl*_tmpAC;void*_tmpAD;struct
Cyc_Absyn_Vardecl*_tmpAE;void*_tmpAF;struct Cyc_Absyn_Vardecl*_tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Core_Opt*
_tmpB4;struct Cyc_Core_Opt _tmpB5;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*
_tmpB7;struct Cyc_Core_Opt*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*
_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_MallocInfo _tmpBF;int _tmpC0;
struct Cyc_Absyn_Exp*_tmpC1;void**_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;int _tmpC4;
struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;
struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct _tagged_arr*_tmpCA;
struct Cyc_Absyn_Exp*_tmpCB;struct _tagged_arr*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;
struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD3;
struct Cyc_List_List*_tmpD4;struct Cyc_List_List*_tmpD5;struct Cyc_List_List*_tmpD6;
struct Cyc_Absyn_Vardecl*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;int _tmpDA;struct Cyc_Absyn_Stmt*_tmpDB;void*_tmpDC;_LL8A: if(*((int*)_tmp9E)
!= 13)goto _LL8C;_tmp9F=((struct Cyc_Absyn_Cast_e_struct*)_tmp9E)->f2;_LL8B: _tmpA0=
_tmp9F;goto _LL8D;_LL8C: if(*((int*)_tmp9E)!= 11)goto _LL8E;_tmpA0=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp9E)->f1;_LL8D: _tmpA1=_tmpA0;goto _LL8F;_LL8E: if(*((int*)_tmp9E)!= 12)goto
_LL90;_tmpA1=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp9E)->f1;_LL8F: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA1);_LL90: if(*((int*)_tmp9E)!= 0)goto _LL92;_tmpA2=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp9E)->f1;if((int)_tmpA2 != 0)goto _LL92;_LL91: goto
_LL93;_LL92: if(*((int*)_tmp9E)!= 0)goto _LL94;_tmpA3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9E)->f1;if(_tmpA3 <= (void*)1?1:*((int*)_tmpA3)!= 2)goto _LL94;_tmpA4=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpA3)->f1;if((int)_tmpA4 != 0)goto _LL94;_tmpA5=((
struct Cyc_Absyn_Int_c_struct*)_tmpA3)->f2;if(_tmpA5 != 0)goto _LL94;_LL93: return({
struct _tuple0 _tmpDD;_tmpDD.f1=inflow;_tmpDD.f2=(void*)0;_tmpDD;});_LL94: if(*((
int*)_tmp9E)!= 0)goto _LL96;_tmpA6=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9E)->f1;if(_tmpA6 <= (void*)1?1:*((int*)_tmpA6)!= 2)goto _LL96;_tmpA7=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpA6)->f1;if((int)_tmpA7 != 0)goto _LL96;_LL95:
goto _LL97;_LL96: if(*((int*)_tmp9E)!= 1)goto _LL98;_tmpA8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if(_tmpA8 <= (void*)1?1:*((int*)_tmpA8)!= 1)goto _LL98;_LL97: return({
struct _tuple0 _tmpDE;_tmpDE.f1=inflow;_tmpDE.f2=(void*)1;_tmpDE;});_LL98: if(*((
int*)_tmp9E)!= 30)goto _LL9A;_tmpA9=((struct Cyc_Absyn_Tunion_e_struct*)_tmp9E)->f1;
if(_tmpA9 != 0)goto _LL9A;_LL99: goto _LL9B;_LL9A: if(*((int*)_tmp9E)!= 0)goto _LL9C;
_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp9E)!= 17)goto _LL9E;_LL9D: goto _LL9F;_LL9E:
if(*((int*)_tmp9E)!= 16)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmp9E)!= 18)
goto _LLA2;_LLA1: goto _LLA3;_LLA2: if(*((int*)_tmp9E)!= 19)goto _LLA4;_LLA3: goto
_LLA5;_LLA4: if(*((int*)_tmp9E)!= 32)goto _LLA6;_LLA5: goto _LLA7;_LLA6: if(*((int*)
_tmp9E)!= 31)goto _LLA8;_LLA7: return({struct _tuple0 _tmpDF;_tmpDF.f1=inflow;_tmpDF.f2=
Cyc_CfFlowInfo_unknown_all;_tmpDF;});_LLA8: if(*((int*)_tmp9E)!= 1)goto _LLAA;
_tmpAA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAA <= (void*)1?1:*((
int*)_tmpAA)!= 0)goto _LLAA;_LLA9: return({struct _tuple0 _tmpE0;_tmpE0.f1=inflow;
_tmpE0.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmpE0;});_LLAA: if(*((int*)_tmp9E)!= 1)
goto _LLAC;_tmpAB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAB <= (
void*)1?1:*((int*)_tmpAB)!= 2)goto _LLAC;_tmpAC=((struct Cyc_Absyn_Param_b_struct*)
_tmpAB)->f1;_LLAB: _tmpAE=_tmpAC;goto _LLAD;_LLAC: if(*((int*)_tmp9E)!= 1)goto _LLAE;
_tmpAD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAD <= (void*)1?1:*((
int*)_tmpAD)!= 3)goto _LLAE;_tmpAE=((struct Cyc_Absyn_Local_b_struct*)_tmpAD)->f1;
_LLAD: _tmpB0=_tmpAE;goto _LLAF;_LLAE: if(*((int*)_tmp9E)!= 1)goto _LLB0;_tmpAF=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E)->f2;if(_tmpAF <= (void*)1?1:*((int*)
_tmpAF)!= 4)goto _LLB0;_tmpB0=((struct Cyc_Absyn_Pat_b_struct*)_tmpAF)->f1;_LLAF:
return({struct _tuple0 _tmpE1;_tmpE1.f1=inflow;_tmpE1.f2=Cyc_Dict_lookup(d,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=_tmpB0;_tmpE3;});
_tmpE2;}));_tmpE1;});_LLB0: if(*((int*)_tmp9E)!= 3)goto _LLB2;_tmpB1=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9E)->f2;_LLB1: return Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmpB1);
_LLB2: if(*((int*)_tmp9E)!= 5)goto _LLB4;_tmpB2=((struct Cyc_Absyn_Increment_e_struct*)
_tmp9E)->f1;_LLB3: {void*_tmpE6;struct _tuple0 _tmpE5=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpE4[1];_tmpE4[0]=_tmpB2;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpE4,sizeof(struct Cyc_Absyn_Exp*),1));}));
_tmpE6=_tmpE5.f1;{void*_tmpE8;struct _tuple0 _tmpE7=Cyc_NewControlFlow_anal_Lexp(
env,_tmpE6,_tmpB2);_tmpE8=_tmpE7.f2;{struct _tuple0 _tmpEA=({struct _tuple0 _tmpE9;
_tmpE9.f1=_tmpE8;_tmpE9.f2=_tmpE6;_tmpE9;});void*_tmpEB;struct Cyc_CfFlowInfo_Place*
_tmpEC;void*_tmpED;struct Cyc_Dict_Dict*_tmpEE;struct Cyc_List_List*_tmpEF;_LLEB:
_tmpEB=_tmpEA.f1;if(_tmpEB <= (void*)1?1:*((int*)_tmpEB)!= 0)goto _LLED;_tmpEC=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpEB)->f1;_tmpED=_tmpEA.f2;if(_tmpED <= (
void*)1?1:*((int*)_tmpED)!= 0)goto _LLED;_tmpEE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpED)->f1;_tmpEF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpED)->f2;_LLEC:
_tmpEF=Cyc_CfFlowInfo_reln_kill_exp(_tmpEF,_tmpB2);_tmpE6=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpF1;_tmpF1.tag=0;_tmpF1.f1=Cyc_CfFlowInfo_assign_place(_tmpB2->loc,_tmpEE,env->all_changed,
_tmpEC,Cyc_CfFlowInfo_unknown_all);_tmpF1.f2=_tmpEF;_tmpF1;});_tmpF0;});goto
_LLEA;_LLED:;_LLEE: goto _LLEA;_LLEA:;}return({struct _tuple0 _tmpF2;_tmpF2.f1=
_tmpE6;_tmpF2.f2=Cyc_CfFlowInfo_unknown_all;_tmpF2;});}}_LLB4: if(*((int*)_tmp9E)
!= 4)goto _LLB6;_tmpB3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;_tmpB4=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f2;if(_tmpB4 == 0)goto _LLB6;_tmpB5=*
_tmpB4;_tmpB6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f3;_LLB5: {void*
_tmpF5;struct _tuple0 _tmpF4=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct
Cyc_Absyn_Exp*_tmpF3[2];_tmpF3[1]=_tmpB6;_tmpF3[0]=_tmpB3;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF3,sizeof(struct Cyc_Absyn_Exp*),2));}));
_tmpF5=_tmpF4.f1;{void*_tmpF7;struct _tuple0 _tmpF6=Cyc_NewControlFlow_anal_Lexp(
env,_tmpF5,e);_tmpF7=_tmpF6.f2;{struct _tuple0 _tmpF9=({struct _tuple0 _tmpF8;_tmpF8.f1=
_tmpF7;_tmpF8.f2=_tmpF5;_tmpF8;});void*_tmpFA;struct Cyc_CfFlowInfo_Place*_tmpFB;
void*_tmpFC;struct Cyc_Dict_Dict*_tmpFD;struct Cyc_List_List*_tmpFE;_LLF0: _tmpFA=
_tmpF9.f1;if(_tmpFA <= (void*)1?1:*((int*)_tmpFA)!= 0)goto _LLF2;_tmpFB=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmpFA)->f1;_tmpFC=_tmpF9.f2;if(_tmpFC <= (void*)1?
1:*((int*)_tmpFC)!= 0)goto _LLF2;_tmpFD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFC)->f1;_tmpFE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFC)->f2;_LLF1:
_tmpFE=Cyc_CfFlowInfo_reln_kill_exp(_tmpFE,_tmpB3);_tmpF5=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp100;_tmp100.tag=0;_tmp100.f1=Cyc_CfFlowInfo_assign_place(_tmpB3->loc,_tmpFD,
env->all_changed,_tmpFB,Cyc_CfFlowInfo_unknown_all);_tmp100.f2=_tmpFE;_tmp100;});
_tmpFF;});goto _LLEF;_LLF2:;_LLF3: goto _LLEF;_LLEF:;}return({struct _tuple0 _tmp101;
_tmp101.f1=_tmpF5;_tmp101.f2=Cyc_CfFlowInfo_unknown_all;_tmp101;});}}_LLB6: if(*((
int*)_tmp9E)!= 4)goto _LLB8;_tmpB7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f1;
_tmpB8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f2;if(_tmpB8 != 0)goto _LLB8;
_tmpB9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp9E)->f3;_LLB7: {struct Cyc_Set_Set**
_tmp102=env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmp103=
_cycalloc(sizeof(*_tmp103));_tmp103[0]=Cyc_CfFlowInfo_mt_place_set();_tmp103;});{
void*_tmp105;void*_tmp106;struct _tuple0 _tmp104=Cyc_NewControlFlow_anal_Lexp(env,
inflow,_tmpB7);_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;{struct Cyc_Set_Set*_tmp107=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));env->all_changed=({struct Cyc_Set_Set**
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp108;});{void*_tmp10A;void*_tmp10B;struct _tuple0 _tmp109=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB9);_tmp10A=_tmp109.f1;_tmp10B=_tmp109.f2;{struct Cyc_Set_Set*
_tmp10C=*((struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmp10D=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmp107,_tmp105,_tmp10A,_tmp107,_tmp10C);void*_tmp10E=Cyc_CfFlowInfo_join_flow(
_tmp102,inflow,_tmp10D);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp10E,inflow)){
if(_tmp102 == 0)env->all_changed=0;else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmp102,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp107,_tmp10C));}{void*_tmp10F=_tmp10D;struct Cyc_Dict_Dict*_tmp110;struct Cyc_List_List*
_tmp111;_LLF5: if((int)_tmp10F != 0)goto _LLF7;_LLF6: return({struct _tuple0 _tmp112;
_tmp112.f1=(void*)0;_tmp112.f2=_tmp10B;_tmp112;});_LLF7: if(_tmp10F <= (void*)1?1:*((
int*)_tmp10F)!= 0)goto _LLF4;_tmp110=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp10F)->f1;_tmp111=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp10F)->f2;
_LLF8: {void*_tmp113=_tmp106;struct Cyc_CfFlowInfo_Place*_tmp114;_LLFA: if(_tmp113
<= (void*)1?1:*((int*)_tmp113)!= 0)goto _LLFC;_tmp114=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp113)->f1;_LLFB: _tmp110=Cyc_CfFlowInfo_assign_place(e->loc,_tmp110,env->all_changed,
_tmp114,_tmp10B);_tmp111=Cyc_CfFlowInfo_reln_assign_exp(_tmp111,_tmpB7,_tmpB9);
_tmp10D=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp115=_cycalloc(
sizeof(*_tmp115));_tmp115[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp116;
_tmp116.tag=0;_tmp116.f1=_tmp110;_tmp116.f2=_tmp111;_tmp116;});_tmp115;});Cyc_NewControlFlow_update_tryflow(
env,_tmp10D);return({struct _tuple0 _tmp117;_tmp117.f1=_tmp10D;_tmp117.f2=_tmp10B;
_tmp117;});_LLFC: if((int)_tmp113 != 0)goto _LLF9;_LLFD: return({struct _tuple0
_tmp118;_tmp118.f1=Cyc_NewControlFlow_use_Rval(env,_tmpB9->loc,_tmp10D,_tmp10B);
_tmp118.f2=_tmp10B;_tmp118;});_LLF9:;}_LLF4:;}}inflow=_tmp10E;}}}}}}_LLB8: if(*((
int*)_tmp9E)!= 7)goto _LLBA;_tmpBA=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f1;
_tmpBB=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E)->f2;_LLB9: return Cyc_NewControlFlow_anal_Rexp(
env,(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpBA)).f1,_tmpBB);_LLBA: if(*((int*)
_tmp9E)!= 10)goto _LLBC;_tmpBC=((struct Cyc_Absyn_Throw_e_struct*)_tmp9E)->f1;
_LLBB: {void*_tmp11A;void*_tmp11B;struct _tuple0 _tmp119=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBC);_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;Cyc_NewControlFlow_use_Rval(
env,_tmpBC->loc,_tmp11A,_tmp11B);return({struct _tuple0 _tmp11C;_tmp11C.f1=(void*)
0;_tmp11C.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp11C;});}_LLBC: if(*((int*)
_tmp9E)!= 9)goto _LLBE;_tmpBD=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f1;
_tmpBE=((struct Cyc_Absyn_FnCall_e_struct*)_tmp9E)->f2;_LLBD: {void*_tmp11F;
struct Cyc_List_List*_tmp120;struct _tuple5 _tmp11E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=
_tmpBD;_tmp11D->tl=_tmpBE;_tmp11D;}));_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;{
void*_tmp121=Cyc_NewControlFlow_use_Rval(env,_tmpBD->loc,_tmp11F,(void*)((struct
Cyc_List_List*)_check_null(_tmp120))->hd);_tmp120=_tmp120->tl;{struct Cyc_List_List*
init_params=0;{void*_tmp122=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpBD->topt))->v);struct Cyc_Absyn_PtrInfo _tmp123;void*_tmp124;_LLFF:
if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 4)goto _LL101;_tmp123=((struct Cyc_Absyn_PointerType_struct*)
_tmp122)->f1;_tmp124=(void*)_tmp123.elt_typ;_LL100:{void*_tmp125=Cyc_Tcutil_compress(
_tmp124);struct Cyc_Absyn_FnInfo _tmp126;struct Cyc_List_List*_tmp127;_LL104: if(
_tmp125 <= (void*)3?1:*((int*)_tmp125)!= 8)goto _LL106;_tmp126=((struct Cyc_Absyn_FnType_struct*)
_tmp125)->f1;_tmp127=_tmp126.attributes;_LL105: for(0;_tmp127 != 0;_tmp127=_tmp127->tl){
void*_tmp128=(void*)_tmp127->hd;int _tmp129;_LL109: if(_tmp128 <= (void*)17?1:*((
int*)_tmp128)!= 4)goto _LL10B;_tmp129=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp128)->f1;_LL10A: init_params=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*
_tmp12A));_tmp12A->hd=(void*)_tmp129;_tmp12A->tl=init_params;_tmp12A;});goto
_LL108;_LL10B:;_LL10C: goto _LL108;_LL108:;}goto _LL103;_LL106:;_LL107:({void*
_tmp12B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp12C="anal_Rexp: bad function type";_tag_arr(_tmp12C,sizeof(char),
_get_zero_arr_size(_tmp12C,29));}),_tag_arr(_tmp12B,sizeof(void*),0));});_LL103:;}
goto _LLFE;_LL101:;_LL102:({void*_tmp12D[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp12E="anal_Rexp: bad function type";
_tag_arr(_tmp12E,sizeof(char),_get_zero_arr_size(_tmp12E,29));}),_tag_arr(
_tmp12D,sizeof(void*),0));});_LLFE:;}{int i=1;for(0;_tmp120 != 0;(((_tmp120=
_tmp120->tl,_tmpBE=((struct Cyc_List_List*)_check_null(_tmpBE))->tl)),++ i)){if(!((
int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp121=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpBE))->hd)->loc,
_tmp121,(void*)_tmp120->hd);continue;}{void*_tmp12F=_tmp11F;struct Cyc_Dict_Dict*
_tmp130;_LL10E: if((int)_tmp12F != 0)goto _LL110;_LL10F: goto _LL10D;_LL110: if(
_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 0)goto _LL10D;_tmp130=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp12F)->f1;_LL111: if(Cyc_CfFlowInfo_initlevel(_tmp130,(void*)_tmp120->hd)== (
void*)0)({void*_tmp131[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpBE))->hd)->loc,({const char*_tmp132="expression may not be initialized";
_tag_arr(_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,34));}),_tag_arr(
_tmp131,sizeof(void*),0));});{void*_tmp133=_tmp121;struct Cyc_Dict_Dict*_tmp134;
struct Cyc_List_List*_tmp135;_LL113: if((int)_tmp133 != 0)goto _LL115;_LL114: goto
_LL112;_LL115: if(_tmp133 <= (void*)1?1:*((int*)_tmp133)!= 0)goto _LL112;_tmp134=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f1;_tmp135=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp133)->f2;_LL116: {struct Cyc_Dict_Dict*_tmp136=Cyc_CfFlowInfo_escape_deref(
_tmp134,env->all_changed,(void*)_tmp120->hd);{void*_tmp137=(void*)_tmp120->hd;
struct Cyc_CfFlowInfo_Place*_tmp138;_LL118: if(_tmp137 <= (void*)3?1:*((int*)
_tmp137)!= 2)goto _LL11A;_tmp138=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp137)->f1;
_LL119:{void*_tmp139=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpBE))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp13A;void*_tmp13B;_LL11D: if(_tmp139 <= (void*)3?1:*((
int*)_tmp139)!= 4)goto _LL11F;_tmp13A=((struct Cyc_Absyn_PointerType_struct*)
_tmp139)->f1;_tmp13B=(void*)_tmp13A.elt_typ;_LL11E: _tmp136=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpBE->hd)->loc,_tmp136,env->all_changed,_tmp138,Cyc_CfFlowInfo_typ_to_absrval(
_tmp13B,Cyc_CfFlowInfo_esc_all));goto _LL11C;_LL11F:;_LL120:({void*_tmp13C[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp13D="anal_Rexp: bad type for initialized arg";_tag_arr(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,40));}),_tag_arr(_tmp13C,sizeof(void*),0));});_LL11C:;}
goto _LL117;_LL11A:;_LL11B: goto _LL117;_LL117:;}_tmp121=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp13F;_tmp13F.tag=0;_tmp13F.f1=_tmp136;_tmp13F.f2=_tmp135;_tmp13F;});_tmp13E;});
goto _LL112;}_LL112:;}goto _LL10D;_LL10D:;}}}if(Cyc_Tcutil_is_noreturn((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpBD->topt))->v))return({struct _tuple0 _tmp140;
_tmp140.f1=(void*)0;_tmp140.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp140;});else{return({
struct _tuple0 _tmp141;_tmp141.f1=_tmp121;_tmp141.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp141;});}}}}_LLBE: if(*((int*)_tmp9E)!= 33)goto _LLC0;_tmpBF=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp9E)->f1;_tmpC0=_tmpBF.is_calloc;_tmpC1=_tmpBF.rgn;_tmpC2=_tmpBF.elt_type;
_tmpC3=_tmpBF.num_elts;_tmpC4=_tmpBF.fat_result;_LLBF: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp153;_tmp153.tag=1;_tmp153.f1=_tmpC3;
_tmp153.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp153;});_tmp152;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->root=(void*)root;_tmp151->fields=0;
_tmp151;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp14F=
_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp150;_tmp150.tag=2;_tmp150.f1=place;_tmp150;});_tmp14F;});void*place_val=
_tmpC4?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpC2)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpC1 != 0){void*
_tmp144;struct Cyc_List_List*_tmp145;struct _tuple5 _tmp143=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp142[2];_tmp142[1]=_tmpC3;_tmp142[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpC1);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp142,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp144=
_tmp143.f1;_tmp145=_tmp143.f2;outflow=_tmp144;}else{void*_tmp147;struct _tuple0
_tmp146=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpC3);_tmp147=_tmp146.f1;
outflow=_tmp147;}{void*_tmp148=outflow;struct Cyc_Dict_Dict*_tmp149;struct Cyc_List_List*
_tmp14A;_LL122: if((int)_tmp148 != 0)goto _LL124;_LL123: return({struct _tuple0
_tmp14B;_tmp14B.f1=outflow;_tmp14B.f2=rval;_tmp14B;});_LL124: if(_tmp148 <= (void*)
1?1:*((int*)_tmp148)!= 0)goto _LL121;_tmp149=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp148)->f1;_tmp14A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp148)->f2;
_LL125: return({struct _tuple0 _tmp14C;_tmp14C.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14E;_tmp14E.tag=0;_tmp14E.f1=Cyc_Dict_insert(_tmp149,root,place_val);_tmp14E.f2=
_tmp14A;_tmp14E;});_tmp14D;});_tmp14C.f2=rval;_tmp14C;});_LL121:;}}_LLC0: if(*((
int*)_tmp9E)!= 15)goto _LLC2;_tmpC5=((struct Cyc_Absyn_New_e_struct*)_tmp9E)->f1;
_tmpC6=((struct Cyc_Absyn_New_e_struct*)_tmp9E)->f2;_LLC1: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp166;_tmp166.tag=1;_tmp166.f1=_tmpC6;
_tmp166.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp166;});_tmp165;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->root=(void*)root;_tmp164->fields=0;
_tmp164;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp162=
_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp163;_tmp163.tag=2;_tmp163.f1=place;_tmp163;});_tmp162;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpC5 != 0){
void*_tmp156;struct Cyc_List_List*_tmp157;struct _tuple5 _tmp155=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp154[2];_tmp154[1]=_tmpC6;_tmp154[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpC5);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp154,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp156=
_tmp155.f1;_tmp157=_tmp155.f2;outflow=_tmp156;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp157))->tl))->hd;}else{void*
_tmp159;void*_tmp15A;struct _tuple0 _tmp158=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpC6);_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;outflow=_tmp159;place_val=
_tmp15A;}{void*_tmp15B=outflow;struct Cyc_Dict_Dict*_tmp15C;struct Cyc_List_List*
_tmp15D;_LL127: if((int)_tmp15B != 0)goto _LL129;_LL128: return({struct _tuple0
_tmp15E;_tmp15E.f1=outflow;_tmp15E.f2=rval;_tmp15E;});_LL129: if(_tmp15B <= (void*)
1?1:*((int*)_tmp15B)!= 0)goto _LL126;_tmp15C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp15B)->f1;_tmp15D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp15B)->f2;
_LL12A: return({struct _tuple0 _tmp15F;_tmp15F.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp161;_tmp161.tag=0;_tmp161.f1=Cyc_Dict_insert(_tmp15C,root,place_val);_tmp161.f2=
_tmp15D;_tmp161;});_tmp160;});_tmp15F.f2=rval;_tmp15F;});_LL126:;}}}_LLC2: if(*((
int*)_tmp9E)!= 14)goto _LLC4;_tmpC7=((struct Cyc_Absyn_Address_e_struct*)_tmp9E)->f1;
_LLC3: {void*_tmp168;void*_tmp169;struct _tuple0 _tmp167=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpC7);_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;{void*_tmp16A=_tmp169;
struct Cyc_CfFlowInfo_Place*_tmp16B;_LL12C: if((int)_tmp16A != 0)goto _LL12E;_LL12D:
return({struct _tuple0 _tmp16C;_tmp16C.f1=_tmp168;_tmp16C.f2=(void*)1;_tmp16C;});
_LL12E: if(_tmp16A <= (void*)1?1:*((int*)_tmp16A)!= 0)goto _LL12B;_tmp16B=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp16A)->f1;_LL12F: return({struct _tuple0 _tmp16D;
_tmp16D.f1=_tmp168;_tmp16D.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp16F;_tmp16F.tag=2;_tmp16F.f1=_tmp16B;_tmp16F;});_tmp16E;});_tmp16D;});_LL12B:;}}
_LLC4: if(*((int*)_tmp9E)!= 20)goto _LLC6;_tmpC8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp9E)->f1;_LLC5: {void*_tmp171;void*_tmp172;struct _tuple0 _tmp170=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC8);_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp171,_tmpC8,_tmp172);}_LLC6: if(*((int*)_tmp9E)!= 21)goto _LLC8;
_tmpC9=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp9E)->f1;_tmpCA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9E)->f2;_LLC7: {void*_tmp174;void*_tmp175;struct _tuple0 _tmp173=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC9);_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpC9->topt))->v))return({struct _tuple0
_tmp176;_tmp176.f1=_tmp174;_tmp176.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp176;});{
void*_tmp177=_tmp175;struct Cyc_Dict_Dict*_tmp178;_LL131: if(_tmp177 <= (void*)3?1:*((
int*)_tmp177)!= 4)goto _LL133;_tmp178=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp177)->f1;_LL132: return({struct _tuple0 _tmp179;_tmp179.f1=_tmp174;_tmp179.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp178,
_tmpCA);_tmp179;});_LL133:;_LL134:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Core_Impossible_struct
_tmp17B;_tmp17B.tag=Cyc_Core_Impossible;_tmp17B.f1=({const char*_tmp17C="anal_Rexp: AggrMember";
_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,22));});_tmp17B;});
_tmp17A;}));_LL130:;}}_LLC8: if(*((int*)_tmp9E)!= 22)goto _LLCA;_tmpCB=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp9E)->f1;_tmpCC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9E)->f2;_LLC9: {
void*_tmp17E;void*_tmp17F;struct _tuple0 _tmp17D=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpCB);_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;{void*_tmp181;void*_tmp182;
struct _tuple0 _tmp180=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp17E,_tmpCB,
_tmp17F);_tmp181=_tmp180.f1;_tmp182=_tmp180.f2;{void*_tmp183=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpCB->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp184;void*_tmp185;_LL136: if(_tmp183 <= (void*)3?1:*((int*)_tmp183)!= 4)goto
_LL138;_tmp184=((struct Cyc_Absyn_PointerType_struct*)_tmp183)->f1;_tmp185=(void*)
_tmp184.elt_typ;_LL137: if(Cyc_Absyn_is_union_type(_tmp185))return({struct _tuple0
_tmp186;_tmp186.f1=_tmp181;_tmp186.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp186;});
goto _LL135;_LL138:;_LL139:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Core_Impossible_struct
_tmp188;_tmp188.tag=Cyc_Core_Impossible;_tmp188.f1=({const char*_tmp189="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,25));});_tmp188;});
_tmp187;}));_LL135:;}{void*_tmp18A=_tmp182;struct Cyc_Dict_Dict*_tmp18B;_LL13B:
if(_tmp18A <= (void*)3?1:*((int*)_tmp18A)!= 4)goto _LL13D;_tmp18B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp18A)->f1;_LL13C: return({struct _tuple0 _tmp18C;_tmp18C.f1=_tmp181;_tmp18C.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp18B,
_tmpCC);_tmp18C;});_LL13D:;_LL13E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Core_Impossible_struct
_tmp18E;_tmp18E.tag=Cyc_Core_Impossible;_tmp18E.f1=({const char*_tmp18F="anal_Rexp: AggrArrow";
_tag_arr(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,21));});_tmp18E;});
_tmp18D;}));_LL13A:;}}}_LLCA: if(*((int*)_tmp9E)!= 6)goto _LLCC;_tmpCD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9E)->f1;_tmpCE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f2;_tmpCF=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp9E)->f3;_LLCB: {void*_tmp191;void*
_tmp192;struct _tuple0 _tmp190=Cyc_NewControlFlow_anal_test(env,inflow,_tmpCD);
_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;{struct _tuple0 _tmp193=Cyc_NewControlFlow_anal_Rexp(
env,_tmp191,_tmpCE);struct _tuple0 _tmp194=Cyc_NewControlFlow_anal_Rexp(env,
_tmp192,_tmpCF);return Cyc_CfFlowInfo_join_flow_and_rval(env->all_changed,_tmp193,
_tmp194);}}_LLCC: if(*((int*)_tmp9E)!= 23)goto _LLCE;_tmpD0=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp9E)->f1;_tmpD1=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9E)->f2;_LLCD: {
void*_tmp197;struct Cyc_List_List*_tmp198;struct _tuple5 _tmp196=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp195[2];_tmp195[1]=_tmpD1;_tmp195[0]=_tmpD0;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp195,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;{void*
_tmp199=_tmp197;{void*_tmp19A=_tmp197;struct Cyc_Dict_Dict*_tmp19B;struct Cyc_List_List*
_tmp19C;_LL140: if(_tmp19A <= (void*)1?1:*((int*)_tmp19A)!= 0)goto _LL142;_tmp19B=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp19A)->f1;_tmp19C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp19A)->f2;_LL141: if(Cyc_CfFlowInfo_initlevel(_tmp19B,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp198))->tl))->hd)== (void*)0)({
void*_tmp19D[0]={};Cyc_Tcutil_terr(_tmpD1->loc,({const char*_tmp19E="expression may not be initialized";
_tag_arr(_tmp19E,sizeof(char),_get_zero_arr_size(_tmp19E,34));}),_tag_arr(
_tmp19D,sizeof(void*),0));});{struct Cyc_List_List*_tmp19F=Cyc_NewControlFlow_add_subscript_reln(
_tmp19C,_tmpD0,_tmpD1);if(_tmp19C != _tmp19F)_tmp199=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=_tmp19B;_tmp1A1.f2=_tmp19F;_tmp1A1;});_tmp1A0;});
goto _LL13F;}_LL142:;_LL143: goto _LL13F;_LL13F:;}{void*_tmp1A2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1A3;struct Cyc_Absyn_PtrAtts _tmp1A4;struct Cyc_Absyn_Conref*_tmp1A5;_LL145: if(
_tmp1A2 <= (void*)3?1:*((int*)_tmp1A2)!= 9)goto _LL147;_LL146: {void*_tmp1A6=(void*)((
struct Cyc_List_List*)_check_null(_tmp198))->hd;struct Cyc_Dict_Dict*_tmp1A7;
_LL14C: if(_tmp1A6 <= (void*)3?1:*((int*)_tmp1A6)!= 4)goto _LL14E;_tmp1A7=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A6)->f1;_LL14D: return({struct _tuple0
_tmp1A8;_tmp1A8.f1=_tmp199;_tmp1A8.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp1A7,Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(
_tmpD1)).f1));_tmp1A8;});_LL14E:;_LL14F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Core_Impossible_struct
_tmp1AA;_tmp1AA.tag=Cyc_Core_Impossible;_tmp1AA.f1=({const char*_tmp1AB="anal_Rexp: Subscript";
_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,21));});_tmp1AA;});
_tmp1A9;}));_LL14B:;}_LL147: if(_tmp1A2 <= (void*)3?1:*((int*)_tmp1A2)!= 4)goto
_LL149;_tmp1A3=((struct Cyc_Absyn_PointerType_struct*)_tmp1A2)->f1;_tmp1A4=
_tmp1A3.ptr_atts;_tmp1A5=_tmp1A4.bounds;_LL148:{void*_tmp1AC=(void*)(Cyc_Absyn_compress_conref(
_tmp1A5))->v;void*_tmp1AD;_LL151: if(_tmp1AC <= (void*)1?1:*((int*)_tmp1AC)!= 0)
goto _LL153;_tmp1AD=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp1AC)->f1;if(
_tmp1AD <= (void*)1?1:*((int*)_tmp1AD)!= 1)goto _LL153;_LL152:{void*_tmp1AE=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp198))->tl))->hd;
struct Cyc_List_List*_tmp1AF;_LL156: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 3)
goto _LL158;_tmp1AF=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1AE)->f1;_LL157:(
void*)(_tmpD1->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp1B1;_tmp1B1.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp1B1.f1=_tmp1AF;_tmp1B1;});
_tmp1B0;})));goto _LL155;_LL158:;_LL159: goto _LL155;_LL155:;}goto _LL150;_LL153:;
_LL154: goto _LL150;_LL150:;}{void*_tmp1B3;void*_tmp1B4;struct _tuple0 _tmp1B2=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp197,_tmpD0,(void*)((struct Cyc_List_List*)_check_null(_tmp198))->hd);
_tmp1B3=_tmp1B2.f1;_tmp1B4=_tmp1B2.f2;return _tmp1B3 == (void*)0?({struct _tuple0
_tmp1B5;_tmp1B5.f1=_tmp1B3;_tmp1B5.f2=_tmp1B4;_tmp1B5;}):({struct _tuple0 _tmp1B6;
_tmp1B6.f1=_tmp199;_tmp1B6.f2=_tmp1B4;_tmp1B6;});}_LL149:;_LL14A:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));
_tmp1B7[0]=({struct Cyc_Core_Impossible_struct _tmp1B8;_tmp1B8.tag=Cyc_Core_Impossible;
_tmp1B8.f1=({const char*_tmp1B9="anal_Rexp: Subscript -- bad type";_tag_arr(
_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,33));});_tmp1B8;});_tmp1B7;}));
_LL144:;}}}_LLCE: if(*((int*)_tmp9E)!= 30)goto _LLD0;_tmpD2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp9E)->f1;_LLCF: _tmpD3=_tmpD2;goto _LLD1;_LLD0: if(*((int*)_tmp9E)!= 24)goto
_LLD2;_tmpD3=((struct Cyc_Absyn_Tuple_e_struct*)_tmp9E)->f1;_LLD1: {void*_tmp1BB;
struct Cyc_List_List*_tmp1BC;struct _tuple5 _tmp1BA=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpD3);_tmp1BB=_tmp1BA.f1;_tmp1BC=_tmp1BA.f2;{struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp1BC != 0;(_tmp1BC=
_tmp1BC->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(i),(
void*)_tmp1BC->hd);}}return({struct _tuple0 _tmp1BD;_tmp1BD.f1=_tmp1BB;_tmp1BD.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));
_tmp1BE[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1BF;_tmp1BF.tag=4;_tmp1BF.f1=
aggrdict;_tmp1BF;});_tmp1BE;});_tmp1BD;});}}_LLD2: if(*((int*)_tmp9E)!= 29)goto
_LLD4;_tmpD4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9E)->f2;_LLD3: _tmpD5=
_tmpD4;goto _LLD5;_LLD4: if(*((int*)_tmp9E)!= 28)goto _LLD6;_tmpD5=((struct Cyc_Absyn_Struct_e_struct*)
_tmp9E)->f3;_LLD5: {void*_tmp1C1;struct Cyc_List_List*_tmp1C2;struct _tuple5
_tmp1C0=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpD5));_tmp1C1=_tmp1C0.f1;
_tmp1C2=_tmp1C0.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int
i=0;for(0;_tmp1C2 != 0;(((_tmp1C2=_tmp1C2->tl,_tmpD5=_tmpD5->tl)),++ i)){struct Cyc_List_List*
ds=(*((struct _tuple6*)((struct Cyc_List_List*)_check_null(_tmpD5))->hd)).f1;for(0;
ds != 0;ds=ds->tl){void*_tmp1C3=(void*)ds->hd;struct _tagged_arr*_tmp1C4;_LL15B:
if(*((int*)_tmp1C3)!= 0)goto _LL15D;_LL15C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=({struct Cyc_Core_Impossible_struct
_tmp1C6;_tmp1C6.tag=Cyc_Core_Impossible;_tmp1C6.f1=({const char*_tmp1C7="anal_Rexp:Struct_e";
_tag_arr(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,19));});_tmp1C6;});
_tmp1C5;}));_LL15D: if(*((int*)_tmp1C3)!= 1)goto _LL15A;_tmp1C4=((struct Cyc_Absyn_FieldName_struct*)
_tmp1C3)->f1;_LL15E: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp1C4,(void*)_tmp1C2->hd);
_LL15A:;}}}return({struct _tuple0 _tmp1C8;_tmp1C8.f1=_tmp1C1;_tmp1C8.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));
_tmp1C9[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1CA;_tmp1CA.tag=4;_tmp1CA.f1=
aggrdict;_tmp1CA;});_tmp1C9;});_tmp1C8;});}}_LLD6: if(*((int*)_tmp9E)!= 26)goto
_LLD8;_tmpD6=((struct Cyc_Absyn_Array_e_struct*)_tmp9E)->f1;_LLD7: {struct Cyc_List_List*
_tmp1CB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple6*))
Cyc_Core_snd,_tmpD6);void*_tmp1CD;struct Cyc_List_List*_tmp1CE;struct _tuple5
_tmp1CC=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp1CB);_tmp1CD=
_tmp1CC.f1;_tmp1CE=_tmp1CC.f2;for(0;_tmp1CE != 0;(_tmp1CE=_tmp1CE->tl,_tmp1CB=
_tmp1CB->tl)){_tmp1CD=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1CB))->hd)->loc,_tmp1CD,(void*)_tmp1CE->hd);}
return({struct _tuple0 _tmp1CF;_tmp1CF.f1=_tmp1CD;_tmp1CF.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1CF;});}_LLD8: if(*((int*)_tmp9E)!= 27)goto _LLDA;_tmpD7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E)->f1;_tmpD8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E)->f2;_tmpD9=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E)->f3;_tmpDA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E)->f4;_LLD9: {void*_tmp1D1;void*_tmp1D2;struct _tuple0 _tmp1D0=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD8);_tmp1D1=_tmp1D0.f1;_tmp1D2=_tmp1D0.f2;{void*_tmp1D3=_tmp1D1;
struct Cyc_Dict_Dict*_tmp1D4;struct Cyc_List_List*_tmp1D5;_LL160: if((int)_tmp1D3 != 
0)goto _LL162;_LL161: return({struct _tuple0 _tmp1D6;_tmp1D6.f1=_tmp1D1;_tmp1D6.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1D6;});_LL162: if(_tmp1D3 <= (void*)1?1:*((int*)
_tmp1D3)!= 0)goto _LL15F;_tmp1D4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1D3)->f1;_tmp1D5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1D3)->f2;
_LL163: if(Cyc_CfFlowInfo_initlevel(_tmp1D4,_tmp1D2)== (void*)0)({void*_tmp1D7[0]={};
Cyc_Tcutil_terr(_tmpD8->loc,({const char*_tmp1D8="expression may not be initialized";
_tag_arr(_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,34));}),_tag_arr(
_tmp1D7,sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp1D5;comp_loop: {
void*_tmp1D9=(void*)_tmpD8->r;struct Cyc_Absyn_Exp*_tmp1DA;void*_tmp1DB;struct Cyc_Absyn_Vardecl*
_tmp1DC;void*_tmp1DD;struct Cyc_Absyn_Vardecl*_tmp1DE;void*_tmp1DF;struct Cyc_Absyn_Vardecl*
_tmp1E0;void*_tmp1E1;struct Cyc_Absyn_Vardecl*_tmp1E2;void*_tmp1E3;int _tmp1E4;
void*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List _tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;_LL165: if(*((int*)_tmp1D9)!= 13)goto _LL167;_tmp1DA=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1D9)->f2;_LL166: _tmpD8=_tmp1DA;goto comp_loop;_LL167: if(*((int*)_tmp1D9)!= 1)
goto _LL169;_tmp1DB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D9)->f2;if(
_tmp1DB <= (void*)1?1:*((int*)_tmp1DB)!= 0)goto _LL169;_tmp1DC=((struct Cyc_Absyn_Global_b_struct*)
_tmp1DB)->f1;if(!(!_tmp1DC->escapes))goto _LL169;_LL168: _tmp1DE=_tmp1DC;goto
_LL16A;_LL169: if(*((int*)_tmp1D9)!= 1)goto _LL16B;_tmp1DD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D9)->f2;if(_tmp1DD <= (void*)1?1:*((int*)_tmp1DD)!= 3)goto _LL16B;_tmp1DE=((
struct Cyc_Absyn_Local_b_struct*)_tmp1DD)->f1;if(!(!_tmp1DE->escapes))goto _LL16B;
_LL16A: _tmp1E0=_tmp1DE;goto _LL16C;_LL16B: if(*((int*)_tmp1D9)!= 1)goto _LL16D;
_tmp1DF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D9)->f2;if(_tmp1DF <= (void*)
1?1:*((int*)_tmp1DF)!= 4)goto _LL16D;_tmp1E0=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1DF)->f1;if(!(!_tmp1E0->escapes))goto _LL16D;_LL16C: _tmp1E2=_tmp1E0;goto
_LL16E;_LL16D: if(*((int*)_tmp1D9)!= 1)goto _LL16F;_tmp1E1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D9)->f2;if(_tmp1E1 <= (void*)1?1:*((int*)_tmp1E1)!= 2)goto _LL16F;_tmp1E2=((
struct Cyc_Absyn_Param_b_struct*)_tmp1E1)->f1;if(!(!_tmp1E2->escapes))goto _LL16F;
_LL16E: new_relns=({struct Cyc_List_List*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));
_tmp1E9->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));
_tmp1EA->vd=_tmpD7;_tmp1EA->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp1EC;_tmp1EC.tag=1;_tmp1EC.f1=_tmp1E2;_tmp1EC;});_tmp1EB;}));_tmp1EA;});
_tmp1E9->tl=_tmp1D5;_tmp1E9;});goto _LL164;_LL16F: if(*((int*)_tmp1D9)!= 0)goto
_LL171;_tmp1E3=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1D9)->f1;if(_tmp1E3
<= (void*)1?1:*((int*)_tmp1E3)!= 2)goto _LL171;_tmp1E4=((struct Cyc_Absyn_Int_c_struct*)
_tmp1E3)->f2;_LL170: new_relns=({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*
_tmp1ED));_tmp1ED->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1EE=_cycalloc(sizeof(*
_tmp1EE));_tmp1EE->vd=_tmpD7;_tmp1EE->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1EF=_cycalloc_atomic(sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1F0;_tmp1F0.tag=3;_tmp1F0.f1=(unsigned int)_tmp1E4;_tmp1F0;});_tmp1EF;}));
_tmp1EE;});_tmp1ED->tl=_tmp1D5;_tmp1ED;});goto _LL164;_LL171: if(*((int*)_tmp1D9)
!= 3)goto _LL173;_tmp1E5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1D9)->f1;
if((int)_tmp1E5 != 19)goto _LL173;_tmp1E6=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D9)->f2;if(_tmp1E6 == 0)goto _LL173;_tmp1E7=*_tmp1E6;_tmp1E8=(struct Cyc_Absyn_Exp*)
_tmp1E7.hd;_LL172:{void*_tmp1F1=(void*)_tmp1E8->r;void*_tmp1F2;struct Cyc_Absyn_Vardecl*
_tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Vardecl*_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Vardecl*
_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1F9;_LL176: if(*((int*)_tmp1F1)
!= 1)goto _LL178;_tmp1F2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F1)->f2;if(
_tmp1F2 <= (void*)1?1:*((int*)_tmp1F2)!= 0)goto _LL178;_tmp1F3=((struct Cyc_Absyn_Global_b_struct*)
_tmp1F2)->f1;if(!(!_tmp1F3->escapes))goto _LL178;_LL177: _tmp1F5=_tmp1F3;goto
_LL179;_LL178: if(*((int*)_tmp1F1)!= 1)goto _LL17A;_tmp1F4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F1)->f2;if(_tmp1F4 <= (void*)1?1:*((int*)_tmp1F4)!= 3)goto _LL17A;_tmp1F5=((
struct Cyc_Absyn_Local_b_struct*)_tmp1F4)->f1;if(!(!_tmp1F5->escapes))goto _LL17A;
_LL179: _tmp1F7=_tmp1F5;goto _LL17B;_LL17A: if(*((int*)_tmp1F1)!= 1)goto _LL17C;
_tmp1F6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F1)->f2;if(_tmp1F6 <= (void*)
1?1:*((int*)_tmp1F6)!= 4)goto _LL17C;_tmp1F7=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1F6)->f1;if(!(!_tmp1F7->escapes))goto _LL17C;_LL17B: _tmp1F9=_tmp1F7;goto
_LL17D;_LL17C: if(*((int*)_tmp1F1)!= 1)goto _LL17E;_tmp1F8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F1)->f2;if(_tmp1F8 <= (void*)1?1:*((int*)_tmp1F8)!= 2)goto _LL17E;_tmp1F9=((
struct Cyc_Absyn_Param_b_struct*)_tmp1F8)->f1;if(!(!_tmp1F9->escapes))goto _LL17E;
_LL17D: new_relns=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));
_tmp1FA->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));
_tmp1FB->vd=_tmpD7;_tmp1FB->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp1FD;_tmp1FD.tag=2;_tmp1FD.f1=_tmp1F9;_tmp1FD;});_tmp1FC;}));_tmp1FB;});
_tmp1FA->tl=_tmp1D5;_tmp1FA;});goto _LL175;_LL17E:;_LL17F: goto _LL175;_LL175:;}
goto _LL164;_LL173:;_LL174: goto _LL164;_LL164:;}if(_tmp1D5 != new_relns)_tmp1D1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1FE=_cycalloc(sizeof(*
_tmp1FE));_tmp1FE[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1FF;_tmp1FF.tag=
0;_tmp1FF.f1=_tmp1D4;_tmp1FF.f2=new_relns;_tmp1FF;});_tmp1FE;});{void*_tmp200=
_tmp1D2;_LL181: if((int)_tmp200 != 0)goto _LL183;_LL182: return({struct _tuple0
_tmp201;_tmp201.f1=_tmp1D1;_tmp201.f2=Cyc_CfFlowInfo_unknown_all;_tmp201;});
_LL183: if((int)_tmp200 != 2)goto _LL185;_LL184: goto _LL186;_LL185: if((int)_tmp200 != 
1)goto _LL187;_LL186: goto _LL188;_LL187: if(_tmp200 <= (void*)3?1:*((int*)_tmp200)!= 
2)goto _LL189;_LL188: {struct Cyc_List_List _tmp202=({struct Cyc_List_List _tmp20C;
_tmp20C.hd=_tmpD7;_tmp20C.tl=0;_tmp20C;});_tmp1D1=Cyc_NewControlFlow_add_vars(
_tmp1D1,(struct Cyc_List_List*)& _tmp202,Cyc_CfFlowInfo_unknown_all);{void*_tmp204;
void*_tmp205;struct _tuple0 _tmp203=Cyc_NewControlFlow_anal_Rexp(env,_tmp1D1,
_tmpD9);_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;{void*_tmp206=_tmp204;struct Cyc_Dict_Dict*
_tmp207;_LL18C: if((int)_tmp206 != 0)goto _LL18E;_LL18D: return({struct _tuple0
_tmp208;_tmp208.f1=_tmp204;_tmp208.f2=Cyc_CfFlowInfo_unknown_all;_tmp208;});
_LL18E: if(_tmp206 <= (void*)1?1:*((int*)_tmp206)!= 0)goto _LL18B;_tmp207=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp206)->f1;_LL18F: if(Cyc_CfFlowInfo_initlevel(
_tmp207,_tmp205)!= (void*)2){({void*_tmp209[0]={};Cyc_Tcutil_terr(_tmpD8->loc,({
const char*_tmp20A="expression may not be initialized";_tag_arr(_tmp20A,sizeof(
char),_get_zero_arr_size(_tmp20A,34));}),_tag_arr(_tmp209,sizeof(void*),0));});
return({struct _tuple0 _tmp20B;_tmp20B.f1=(void*)0;_tmp20B.f2=Cyc_CfFlowInfo_unknown_all;
_tmp20B;});}_LL18B:;}_tmp1D1=_tmp204;goto _LL18A;}}_LL189:;_LL18A: while(1){struct
Cyc_List_List _tmp20D=({struct Cyc_List_List _tmp217;_tmp217.hd=_tmpD7;_tmp217.tl=0;
_tmp217;});_tmp1D1=Cyc_NewControlFlow_add_vars(_tmp1D1,(struct Cyc_List_List*)&
_tmp20D,Cyc_CfFlowInfo_unknown_all);{void*_tmp20F;void*_tmp210;struct _tuple0
_tmp20E=Cyc_NewControlFlow_anal_Rexp(env,_tmp1D1,_tmpD9);_tmp20F=_tmp20E.f1;
_tmp210=_tmp20E.f2;{void*_tmp211=_tmp20F;struct Cyc_Dict_Dict*_tmp212;_LL191: if((
int)_tmp211 != 0)goto _LL193;_LL192: goto _LL190;_LL193: if(_tmp211 <= (void*)1?1:*((
int*)_tmp211)!= 0)goto _LL190;_tmp212=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp211)->f1;_LL194: if(Cyc_CfFlowInfo_initlevel(_tmp212,_tmp210)!= (void*)2){({
void*_tmp213[0]={};Cyc_Tcutil_terr(_tmpD8->loc,({const char*_tmp214="expression may not be initialized";
_tag_arr(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,34));}),_tag_arr(
_tmp213,sizeof(void*),0));});return({struct _tuple0 _tmp215;_tmp215.f1=(void*)0;
_tmp215.f2=Cyc_CfFlowInfo_unknown_all;_tmp215;});}_LL190:;}{void*_tmp216=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp1D1,_tmp20F);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp216,
_tmp1D1))break;_tmp1D1=_tmp216;}}}return({struct _tuple0 _tmp218;_tmp218.f1=
_tmp1D1;_tmp218.f2=Cyc_CfFlowInfo_unknown_all;_tmp218;});_LL180:;}}_LL15F:;}}
_LLDA: if(*((int*)_tmp9E)!= 35)goto _LLDC;_tmpDB=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp9E)->f1;_LLDB: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpDB))->flow;{void*_tmp219=(void*)_tmpDB->r;struct Cyc_Absyn_Stmt*
_tmp21A;struct Cyc_Absyn_Stmt*_tmp21B;struct Cyc_Absyn_Decl*_tmp21C;struct Cyc_Absyn_Stmt*
_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;_LL196: if(_tmp219 <= (void*)1?1:*((int*)
_tmp219)!= 1)goto _LL198;_tmp21A=((struct Cyc_Absyn_Seq_s_struct*)_tmp219)->f1;
_tmp21B=((struct Cyc_Absyn_Seq_s_struct*)_tmp219)->f2;_LL197: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp21A);_tmpDB=_tmp21B;goto _LL195;_LL198: if(_tmp219 <= (void*)1?1:*((
int*)_tmp219)!= 12)goto _LL19A;_tmp21C=((struct Cyc_Absyn_Decl_s_struct*)_tmp219)->f1;
_tmp21D=((struct Cyc_Absyn_Decl_s_struct*)_tmp219)->f2;_LL199: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp21C);_tmpDB=_tmp21D;goto _LL195;_LL19A: if(_tmp219 <= (void*)1?1:*((
int*)_tmp219)!= 0)goto _LL19C;_tmp21E=((struct Cyc_Absyn_Exp_s_struct*)_tmp219)->f1;
_LL19B: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp21E);_LL19C:;_LL19D:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21F=_cycalloc(sizeof(*
_tmp21F));_tmp21F[0]=({struct Cyc_Core_Impossible_struct _tmp220;_tmp220.tag=Cyc_Core_Impossible;
_tmp220.f1=({const char*_tmp221="analyze_Rexp: ill-formed StmtExp";_tag_arr(
_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,33));});_tmp220;});_tmp21F;}));
_LL195:;}}_LLDC: if(*((int*)_tmp9E)!= 1)goto _LLDE;_tmpDC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9E)->f2;if((int)_tmpDC != 0)goto _LLDE;_LLDD: goto _LLDF;_LLDE: if(*((int*)_tmp9E)
!= 2)goto _LLE0;_LLDF: goto _LLE1;_LLE0: if(*((int*)_tmp9E)!= 8)goto _LLE2;_LLE1: goto
_LLE3;_LLE2: if(*((int*)_tmp9E)!= 34)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(*((int*)
_tmp9E)!= 25)goto _LLE6;_LLE5: goto _LLE7;_LLE6: if(*((int*)_tmp9E)!= 36)goto _LLE8;
_LLE7: goto _LLE9;_LLE8: if(*((int*)_tmp9E)!= 37)goto _LL89;_LLE9:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222[0]=({
struct Cyc_Core_Impossible_struct _tmp223;_tmp223.tag=Cyc_Core_Impossible;_tmp223.f1=({
const char*_tmp224="anal_Rexp, unexpected exp form";_tag_arr(_tmp224,sizeof(char),
_get_zero_arr_size(_tmp224,31));});_tmp223;});_tmp222;}));_LL89:;}}static struct
_tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*
_tmp225=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp226;void*_tmp227;struct Cyc_Absyn_PtrAtts _tmp228;
struct Cyc_Absyn_Conref*_tmp229;struct Cyc_Absyn_Conref*_tmp22A;_LL19F: if(_tmp225
<= (void*)3?1:*((int*)_tmp225)!= 4)goto _LL1A1;_tmp226=((struct Cyc_Absyn_PointerType_struct*)
_tmp225)->f1;_tmp227=(void*)_tmp226.elt_typ;_tmp228=_tmp226.ptr_atts;_tmp229=
_tmp228.bounds;_tmp22A=_tmp228.zero_term;_LL1A0: {void*_tmp22B=f;struct Cyc_Dict_Dict*
_tmp22C;struct Cyc_List_List*_tmp22D;_LL1A4: if((int)_tmp22B != 0)goto _LL1A6;_LL1A5:
return({struct _tuple0 _tmp22E;_tmp22E.f1=f;_tmp22E.f2=(void*)0;_tmp22E;});_LL1A6:
if(_tmp22B <= (void*)1?1:*((int*)_tmp22B)!= 0)goto _LL1A3;_tmp22C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp22B)->f1;_tmp22D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22B)->f2;
_LL1A7: if(Cyc_Tcutil_is_bound_one(_tmp229)){void*_tmp22F=r;struct Cyc_CfFlowInfo_Place*
_tmp230;struct Cyc_CfFlowInfo_Place _tmp231;void*_tmp232;struct Cyc_List_List*
_tmp233;void*_tmp234;_LL1A9: if((int)_tmp22F != 1)goto _LL1AB;_LL1AA: goto _LL1AC;
_LL1AB: if((int)_tmp22F != 2)goto _LL1AD;_LL1AC:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp236;_tmp236.tag=Cyc_CfFlowInfo_NotZero;
_tmp236.f1=_tmp22D;_tmp236;});_tmp235;})));goto _LL1A8;_LL1AD: if(_tmp22F <= (void*)
3?1:*((int*)_tmp22F)!= 2)goto _LL1AF;_tmp230=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp22F)->f1;_tmp231=*_tmp230;_tmp232=(void*)_tmp231.root;_tmp233=_tmp231.fields;
_LL1AE:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp238;_tmp238.tag=Cyc_CfFlowInfo_NotZero;_tmp238.f1=_tmp22D;_tmp238;});_tmp237;})));
return({struct _tuple0 _tmp239;_tmp239.f1=f;_tmp239.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp23B;_tmp23B.tag=0;_tmp23B.f1=({struct Cyc_CfFlowInfo_Place*_tmp23C=_cycalloc(
sizeof(*_tmp23C));_tmp23C->root=(void*)_tmp232;_tmp23C->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp233,flds);
_tmp23C;});_tmp23B;});_tmp23A;});_tmp239;});_LL1AF: if((int)_tmp22F != 0)goto
_LL1B1;_LL1B0:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple0 _tmp23D;_tmp23D.f1=(void*)0;_tmp23D.f2=(void*)0;_tmp23D;});_LL1B1:
if(_tmp22F <= (void*)3?1:*((int*)_tmp22F)!= 0)goto _LL1B3;_tmp234=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp22F)->f1;_LL1B2: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp234);goto _LL1B4;_LL1B3:;_LL1B4:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp23F;_tmp23F.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp23F.f1=_tmp22D;_tmp23F;});_tmp23E;})));_LL1A8:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp240=_cycalloc(sizeof(*_tmp240));
_tmp240[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp241;_tmp241.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp241.f1=_tmp22D;_tmp241;});_tmp240;})));}if(Cyc_CfFlowInfo_initlevel(_tmp22C,
r)== (void*)0)({void*_tmp242[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp243="dereference of possibly uninitialized pointer";
_tag_arr(_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,46));}),_tag_arr(
_tmp242,sizeof(void*),0));});return({struct _tuple0 _tmp244;_tmp244.f1=f;_tmp244.f2=(
void*)0;_tmp244;});_LL1A3:;}_LL1A1:;_LL1A2:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Core_Impossible_struct
_tmp246;_tmp246.tag=Cyc_Core_Impossible;_tmp246.f1=({const char*_tmp247="left deref of non-pointer-type";
_tag_arr(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,31));});_tmp246;});
_tmp245;}));_LL19E:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp248=inflow;struct Cyc_Dict_Dict*_tmp249;
struct Cyc_List_List*_tmp24A;_LL1B6: if((int)_tmp248 != 0)goto _LL1B8;_LL1B7: return({
struct _tuple0 _tmp24B;_tmp24B.f1=(void*)0;_tmp24B.f2=(void*)0;_tmp24B;});_LL1B8:
if(_tmp248 <= (void*)1?1:*((int*)_tmp248)!= 0)goto _LL1B5;_tmp249=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp248)->f1;_tmp24A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp248)->f2;
_LL1B9: d=_tmp249;_LL1B5:;}{void*_tmp24C=(void*)e->r;struct Cyc_Absyn_Exp*_tmp24D;
struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp24F;void*_tmp250;struct Cyc_Absyn_Vardecl*
_tmp251;void*_tmp252;struct Cyc_Absyn_Vardecl*_tmp253;void*_tmp254;struct Cyc_Absyn_Vardecl*
_tmp255;void*_tmp256;struct Cyc_Absyn_Vardecl*_tmp257;struct Cyc_Absyn_Exp*_tmp258;
struct _tagged_arr*_tmp259;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*
_tmp25B;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Exp*_tmp25D;struct
_tagged_arr*_tmp25E;_LL1BB: if(*((int*)_tmp24C)!= 13)goto _LL1BD;_tmp24D=((struct
Cyc_Absyn_Cast_e_struct*)_tmp24C)->f2;_LL1BC: _tmp24E=_tmp24D;goto _LL1BE;_LL1BD:
if(*((int*)_tmp24C)!= 11)goto _LL1BF;_tmp24E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp24C)->f1;_LL1BE: _tmp24F=_tmp24E;goto _LL1C0;_LL1BF: if(*((int*)_tmp24C)!= 12)
goto _LL1C1;_tmp24F=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp24C)->f1;_LL1C0:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp24F,flds);_LL1C1: if(*((int*)
_tmp24C)!= 1)goto _LL1C3;_tmp250=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24C)->f2;
if(_tmp250 <= (void*)1?1:*((int*)_tmp250)!= 2)goto _LL1C3;_tmp251=((struct Cyc_Absyn_Param_b_struct*)
_tmp250)->f1;_LL1C2: _tmp253=_tmp251;goto _LL1C4;_LL1C3: if(*((int*)_tmp24C)!= 1)
goto _LL1C5;_tmp252=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24C)->f2;if(
_tmp252 <= (void*)1?1:*((int*)_tmp252)!= 3)goto _LL1C5;_tmp253=((struct Cyc_Absyn_Local_b_struct*)
_tmp252)->f1;_LL1C4: _tmp255=_tmp253;goto _LL1C6;_LL1C5: if(*((int*)_tmp24C)!= 1)
goto _LL1C7;_tmp254=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24C)->f2;if(
_tmp254 <= (void*)1?1:*((int*)_tmp254)!= 4)goto _LL1C7;_tmp255=((struct Cyc_Absyn_Pat_b_struct*)
_tmp254)->f1;_LL1C6: return({struct _tuple0 _tmp25F;_tmp25F.f1=inflow;_tmp25F.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp260=_cycalloc(sizeof(*_tmp260));
_tmp260[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp261;_tmp261.tag=0;_tmp261.f1=({
struct Cyc_CfFlowInfo_Place*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp263=_cycalloc(sizeof(*
_tmp263));_tmp263[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp264;_tmp264.tag=0;
_tmp264.f1=_tmp255;_tmp264;});_tmp263;}));_tmp262->fields=flds;_tmp262;});
_tmp261;});_tmp260;});_tmp25F;});_LL1C7: if(*((int*)_tmp24C)!= 1)goto _LL1C9;
_tmp256=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24C)->f2;if(_tmp256 <= (void*)
1?1:*((int*)_tmp256)!= 0)goto _LL1C9;_tmp257=((struct Cyc_Absyn_Global_b_struct*)
_tmp256)->f1;_LL1C8: return({struct _tuple0 _tmp265;_tmp265.f1=inflow;_tmp265.f2=(
void*)0;_tmp265;});_LL1C9: if(*((int*)_tmp24C)!= 22)goto _LL1CB;_tmp258=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp24C)->f1;_tmp259=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp24C)->f2;_LL1CA: if(!Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp258->topt))->v))flds=({struct Cyc_List_List*_tmp266=_cycalloc(
sizeof(*_tmp266));_tmp266->hd=_tmp259;_tmp266->tl=flds;_tmp266;});_tmp25A=
_tmp258;goto _LL1CC;_LL1CB: if(*((int*)_tmp24C)!= 20)goto _LL1CD;_tmp25A=((struct
Cyc_Absyn_Deref_e_struct*)_tmp24C)->f1;_LL1CC: {void*_tmp268;void*_tmp269;struct
_tuple0 _tmp267=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp25A);_tmp268=_tmp267.f1;
_tmp269=_tmp267.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp268,
_tmp25A,_tmp269,flds);}_LL1CD: if(*((int*)_tmp24C)!= 23)goto _LL1CF;_tmp25B=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp24C)->f1;_tmp25C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp24C)->f2;_LL1CE: {void*_tmp26A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp25B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp26B;struct Cyc_Absyn_PtrAtts
_tmp26C;struct Cyc_Absyn_Conref*_tmp26D;_LL1D4: if(_tmp26A <= (void*)3?1:*((int*)
_tmp26A)!= 9)goto _LL1D6;_LL1D5: {struct _tagged_arr*_tmp26E=Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmp25C)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp25B,({struct Cyc_List_List*_tmp26F=_cycalloc(sizeof(*_tmp26F));
_tmp26F->hd=_tmp26E;_tmp26F->tl=flds;_tmp26F;}));}_LL1D6: if(_tmp26A <= (void*)3?1:*((
int*)_tmp26A)!= 4)goto _LL1D8;_tmp26B=((struct Cyc_Absyn_PointerType_struct*)
_tmp26A)->f1;_tmp26C=_tmp26B.ptr_atts;_tmp26D=_tmp26C.bounds;_LL1D7: {void*
_tmp272;struct Cyc_List_List*_tmp273;struct _tuple5 _tmp271=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp270[2];_tmp270[1]=_tmp25C;_tmp270[0]=
_tmp25B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp270,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp272=_tmp271.f1;_tmp273=_tmp271.f2;{
void*_tmp274=_tmp272;{void*_tmp275=_tmp272;struct Cyc_Dict_Dict*_tmp276;struct Cyc_List_List*
_tmp277;_LL1DB: if(_tmp275 <= (void*)1?1:*((int*)_tmp275)!= 0)goto _LL1DD;_tmp276=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp275)->f1;_tmp277=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp275)->f2;_LL1DC: if(Cyc_CfFlowInfo_initlevel(_tmp276,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp273))->tl))->hd)== (void*)0)({
void*_tmp278[0]={};Cyc_Tcutil_terr(_tmp25C->loc,({const char*_tmp279="expression may not be initialized";
_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,34));}),_tag_arr(
_tmp278,sizeof(void*),0));});{struct Cyc_List_List*_tmp27A=Cyc_NewControlFlow_add_subscript_reln(
_tmp277,_tmp25B,_tmp25C);if(_tmp277 != _tmp27A)_tmp274=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp27C;_tmp27C.tag=0;_tmp27C.f1=_tmp276;_tmp27C.f2=_tmp27A;_tmp27C;});_tmp27B;});
goto _LL1DA;}_LL1DD:;_LL1DE: goto _LL1DA;_LL1DA:;}{void*_tmp27D=(void*)(Cyc_Absyn_compress_conref(
_tmp26D))->v;void*_tmp27E;_LL1E0: if(_tmp27D <= (void*)1?1:*((int*)_tmp27D)!= 0)
goto _LL1E2;_tmp27E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp27D)->f1;if(
_tmp27E <= (void*)1?1:*((int*)_tmp27E)!= 1)goto _LL1E2;_LL1E1:{void*_tmp27F=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp273))->tl))->hd;
struct Cyc_List_List*_tmp280;_LL1E5: if(_tmp27F <= (void*)3?1:*((int*)_tmp27F)!= 3)
goto _LL1E7;_tmp280=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp27F)->f1;_LL1E6:(
void*)(_tmp25C->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp282;_tmp282.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp282.f1=_tmp280;_tmp282;});
_tmp281;})));goto _LL1E4;_LL1E7:;_LL1E8: goto _LL1E4;_LL1E4:;}goto _LL1DF;_LL1E2:;
_LL1E3: goto _LL1DF;_LL1DF:;}{void*_tmp284;void*_tmp285;struct _tuple0 _tmp283=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp272,_tmp25B,(void*)((struct Cyc_List_List*)_check_null(_tmp273))->hd,
flds);_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;return _tmp284 == (void*)0?({struct
_tuple0 _tmp286;_tmp286.f1=_tmp284;_tmp286.f2=_tmp285;_tmp286;}):({struct _tuple0
_tmp287;_tmp287.f1=_tmp274;_tmp287.f2=_tmp285;_tmp287;});}}}_LL1D8:;_LL1D9:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp288=_cycalloc(sizeof(*
_tmp288));_tmp288[0]=({struct Cyc_Core_Impossible_struct _tmp289;_tmp289.tag=Cyc_Core_Impossible;
_tmp289.f1=({const char*_tmp28A="anal_Lexp: Subscript -- bad type";_tag_arr(
_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,33));});_tmp289;});_tmp288;}));
_LL1D3:;}_LL1CF: if(*((int*)_tmp24C)!= 21)goto _LL1D1;_tmp25D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp24C)->f1;_tmp25E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp24C)->f2;_LL1D0:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp25D->topt))->v))
return({struct _tuple0 _tmp28B;_tmp28B.f1=inflow;_tmp28B.f2=(void*)0;_tmp28B;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp25D,({struct Cyc_List_List*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=_tmp25E;_tmp28C->tl=flds;_tmp28C;}));
_LL1D1:;_LL1D2: return({struct _tuple0 _tmp28D;_tmp28D.f1=(void*)0;_tmp28D.f2=(void*)
0;_tmp28D;});_LL1BA:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp28E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp28F;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*
_tmp292;struct Cyc_Absyn_Exp*_tmp293;void*_tmp294;struct Cyc_List_List*_tmp295;
struct Cyc_List_List _tmp296;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_List_List*
_tmp298;void*_tmp299;struct Cyc_List_List*_tmp29A;_LL1EA: if(*((int*)_tmp28E)!= 6)
goto _LL1EC;_tmp28F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp28E)->f1;_tmp290=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp28E)->f2;_tmp291=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp28E)->f3;_LL1EB: {void*_tmp29C;void*_tmp29D;struct _tuple0 _tmp29B=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp28F);_tmp29C=_tmp29B.f1;_tmp29D=_tmp29B.f2;{void*_tmp29F;void*
_tmp2A0;struct _tuple0 _tmp29E=Cyc_NewControlFlow_anal_test(env,_tmp29C,_tmp290);
_tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;{void*_tmp2A2;void*_tmp2A3;struct _tuple0
_tmp2A1=Cyc_NewControlFlow_anal_test(env,_tmp29D,_tmp291);_tmp2A2=_tmp2A1.f1;
_tmp2A3=_tmp2A1.f2;return({struct _tuple0 _tmp2A4;_tmp2A4.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp29F,_tmp2A2);_tmp2A4.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp2A0,_tmp2A3);_tmp2A4;});}}}_LL1EC: if(*((int*)_tmp28E)!= 7)goto _LL1EE;_tmp292=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp28E)->f1;_tmp293=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp28E)->f2;_LL1ED: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp292)).f1,_tmp293);_LL1EE: if(*((int*)_tmp28E)!= 3)goto _LL1F0;
_tmp294=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp28E)->f1;if((int)_tmp294 != 
11)goto _LL1F0;_tmp295=((struct Cyc_Absyn_Primop_e_struct*)_tmp28E)->f2;if(_tmp295
== 0)goto _LL1F0;_tmp296=*_tmp295;_tmp297=(struct Cyc_Absyn_Exp*)_tmp296.hd;
_tmp298=_tmp296.tl;if(_tmp298 != 0)goto _LL1F0;_LL1EF: {void*_tmp2A6;void*_tmp2A7;
struct _tuple0 _tmp2A5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp297);_tmp2A6=
_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;return({struct _tuple0 _tmp2A8;_tmp2A8.f1=_tmp2A7;
_tmp2A8.f2=_tmp2A6;_tmp2A8;});}_LL1F0: if(*((int*)_tmp28E)!= 3)goto _LL1F2;_tmp299=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp28E)->f1;_tmp29A=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28E)->f2;_LL1F1: {void*_tmp2AA;struct Cyc_List_List*_tmp2AB;struct _tuple5
_tmp2A9=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp29A);_tmp2AA=
_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;{void*_tmp2AC=_tmp2AA;struct Cyc_Dict_Dict*_tmp2AD;
struct Cyc_List_List*_tmp2AE;_LL1F5: if((int)_tmp2AC != 0)goto _LL1F7;_LL1F6: return({
struct _tuple0 _tmp2AF;_tmp2AF.f1=_tmp2AA;_tmp2AF.f2=_tmp2AA;_tmp2AF;});_LL1F7: if(
_tmp2AC <= (void*)1?1:*((int*)_tmp2AC)!= 0)goto _LL1F4;_tmp2AD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2AC)->f1;_tmp2AE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2AC)->f2;
_LL1F8: {struct Cyc_Absyn_Exp*_tmp2B0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp29A))->hd;struct Cyc_Absyn_Exp*_tmp2B1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp29A->tl))->hd;void*_tmp2B2=(void*)((struct
Cyc_List_List*)_check_null(_tmp2AB))->hd;void*_tmp2B3=(void*)((struct Cyc_List_List*)
_check_null(_tmp2AB->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp2AD,_tmp2B2)== (
void*)0)({void*_tmp2B4[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp29A->hd)->loc,({
const char*_tmp2B5="expression may not be initialized";_tag_arr(_tmp2B5,sizeof(
char),_get_zero_arr_size(_tmp2B5,34));}),_tag_arr(_tmp2B4,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(_tmp2AD,_tmp2B3)== (void*)0)({void*_tmp2B6[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp29A->tl))->hd)->loc,({
const char*_tmp2B7="expression may not be initialized";_tag_arr(_tmp2B7,sizeof(
char),_get_zero_arr_size(_tmp2B7,34));}),_tag_arr(_tmp2B6,sizeof(void*),0));});
if(_tmp299 == (void*)5?1: _tmp299 == (void*)6){struct _tuple0 _tmp2B9=({struct _tuple0
_tmp2B8;_tmp2B8.f1=_tmp2B2;_tmp2B8.f2=_tmp2B3;_tmp2B8;});void*_tmp2BA;void*
_tmp2BB;void*_tmp2BC;void*_tmp2BD;void*_tmp2BE;void*_tmp2BF;_LL1FA: _tmp2BA=
_tmp2B9.f1;if(_tmp2BA <= (void*)3?1:*((int*)_tmp2BA)!= 0)goto _LL1FC;_tmp2BB=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2BA)->f1;_tmp2BC=_tmp2B9.f2;if((int)
_tmp2BC != 0)goto _LL1FC;_LL1FB: {void*_tmp2C1;void*_tmp2C2;struct _tuple0 _tmp2C0=
Cyc_NewControlFlow_splitzero(env,_tmp2AA,_tmp2AA,_tmp2B0,_tmp2BB);_tmp2C1=
_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;{void*_tmp2C3=_tmp299;_LL201: if((int)_tmp2C3 != 5)
goto _LL203;_LL202: return({struct _tuple0 _tmp2C4;_tmp2C4.f1=_tmp2C2;_tmp2C4.f2=
_tmp2C1;_tmp2C4;});_LL203: if((int)_tmp2C3 != 6)goto _LL205;_LL204: return({struct
_tuple0 _tmp2C5;_tmp2C5.f1=_tmp2C1;_tmp2C5.f2=_tmp2C2;_tmp2C5;});_LL205:;_LL206:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2C6=_cycalloc(sizeof(*
_tmp2C6));_tmp2C6[0]=({struct Cyc_Core_Impossible_struct _tmp2C7;_tmp2C7.tag=Cyc_Core_Impossible;
_tmp2C7.f1=({const char*_tmp2C8="anal_test, zero-split";_tag_arr(_tmp2C8,sizeof(
char),_get_zero_arr_size(_tmp2C8,22));});_tmp2C7;});_tmp2C6;}));_LL200:;}}_LL1FC:
_tmp2BD=_tmp2B9.f1;if((int)_tmp2BD != 0)goto _LL1FE;_tmp2BE=_tmp2B9.f2;if(_tmp2BE
<= (void*)3?1:*((int*)_tmp2BE)!= 0)goto _LL1FE;_tmp2BF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2BE)->f1;_LL1FD: {void*_tmp2CA;void*_tmp2CB;struct _tuple0 _tmp2C9=Cyc_NewControlFlow_splitzero(
env,_tmp2AA,_tmp2AA,_tmp2B1,_tmp2BF);_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;{void*
_tmp2CC=_tmp299;_LL208: if((int)_tmp2CC != 5)goto _LL20A;_LL209: return({struct
_tuple0 _tmp2CD;_tmp2CD.f1=_tmp2CB;_tmp2CD.f2=_tmp2CA;_tmp2CD;});_LL20A: if((int)
_tmp2CC != 6)goto _LL20C;_LL20B: return({struct _tuple0 _tmp2CE;_tmp2CE.f1=_tmp2CA;
_tmp2CE.f2=_tmp2CB;_tmp2CE;});_LL20C:;_LL20D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF[0]=({struct Cyc_Core_Impossible_struct
_tmp2D0;_tmp2D0.tag=Cyc_Core_Impossible;_tmp2D0.f1=({const char*_tmp2D1="anal_test, zero-split";
_tag_arr(_tmp2D1,sizeof(char),_get_zero_arr_size(_tmp2D1,22));});_tmp2D0;});
_tmp2CF;}));_LL207:;}}_LL1FE:;_LL1FF: goto _LL1F9;_LL1F9:;}{struct _tuple0 _tmp2D3=({
struct _tuple0 _tmp2D2;_tmp2D2.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B0->topt))->v);_tmp2D2.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp2B1->topt))->v);_tmp2D2;});void*_tmp2D4;void*
_tmp2D5;void*_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;_LL20F: _tmp2D4=
_tmp2D3.f1;if(_tmp2D4 <= (void*)3?1:*((int*)_tmp2D4)!= 5)goto _LL211;_tmp2D5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2D4)->f1;if((int)_tmp2D5 != 1)goto _LL211;
_LL210: goto _LL212;_LL211: _tmp2D6=_tmp2D3.f2;if(_tmp2D6 <= (void*)3?1:*((int*)
_tmp2D6)!= 5)goto _LL213;_tmp2D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D6)->f1;
if((int)_tmp2D7 != 1)goto _LL213;_LL212: goto _LL214;_LL213: _tmp2D8=_tmp2D3.f1;if(
_tmp2D8 <= (void*)3?1:*((int*)_tmp2D8)!= 17)goto _LL215;_LL214: goto _LL216;_LL215:
_tmp2D9=_tmp2D3.f2;if(_tmp2D9 <= (void*)3?1:*((int*)_tmp2D9)!= 17)goto _LL217;
_LL216: goto _LL20E;_LL217:;_LL218: return({struct _tuple0 _tmp2DA;_tmp2DA.f1=_tmp2AA;
_tmp2DA.f2=_tmp2AA;_tmp2DA;});_LL20E:;}{void*_tmp2DB=_tmp299;_LL21A: if((int)
_tmp2DB != 5)goto _LL21C;_LL21B: {void*_tmp2DC=Cyc_NewControlFlow_if_tagcmp(env,
_tmp2AA,_tmp2B0,_tmp2B2,(void*)5,_tmp2B3);_tmp2DC=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2DC,_tmp2B1,_tmp2B3,(void*)5,_tmp2B2);return({struct _tuple0 _tmp2DD;
_tmp2DD.f1=_tmp2DC;_tmp2DD.f2=_tmp2AA;_tmp2DD;});}_LL21C: if((int)_tmp2DB != 6)
goto _LL21E;_LL21D: {void*_tmp2DE=Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,
_tmp2B0,_tmp2B2,(void*)5,_tmp2B3);_tmp2DE=Cyc_NewControlFlow_if_tagcmp(env,
_tmp2DE,_tmp2B1,_tmp2B3,(void*)5,_tmp2B2);return({struct _tuple0 _tmp2DF;_tmp2DF.f1=
_tmp2AA;_tmp2DF.f2=_tmp2DE;_tmp2DF;});}_LL21E: if((int)_tmp2DB != 7)goto _LL220;
_LL21F: {void*_tmp2E0=Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,_tmp2B1,_tmp2B3,(
void*)8,_tmp2B2);void*_tmp2E1=Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,_tmp2B0,
_tmp2B2,(void*)10,_tmp2B3);return({struct _tuple0 _tmp2E2;_tmp2E2.f1=_tmp2E0;
_tmp2E2.f2=_tmp2E1;_tmp2E2;});}_LL220: if((int)_tmp2DB != 9)goto _LL222;_LL221: {
void*_tmp2E3=Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,_tmp2B1,_tmp2B3,(void*)10,
_tmp2B2);void*_tmp2E4=Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,_tmp2B0,_tmp2B2,(
void*)8,_tmp2B3);return({struct _tuple0 _tmp2E5;_tmp2E5.f1=_tmp2E3;_tmp2E5.f2=
_tmp2E4;_tmp2E5;});}_LL222: if((int)_tmp2DB != 8)goto _LL224;_LL223: {void*_tmp2E6=
Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,_tmp2B0,_tmp2B2,(void*)8,_tmp2B3);void*
_tmp2E7=Cyc_NewControlFlow_if_tagcmp(env,_tmp2AA,_tmp2B1,_tmp2B3,(void*)10,
_tmp2B2);{void*_tmp2E8=_tmp2E6;struct Cyc_Dict_Dict*_tmp2E9;_LL229: if((int)
_tmp2E8 != 0)goto _LL22B;_LL22A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA[0]=({struct Cyc_Core_Impossible_struct
_tmp2EB;_tmp2EB.tag=Cyc_Core_Impossible;_tmp2EB.f1=({const char*_tmp2EC="anal_test, Lt";
_tag_arr(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,14));});_tmp2EB;});
_tmp2EA;}));_LL22B: if(_tmp2E8 <= (void*)1?1:*((int*)_tmp2E8)!= 0)goto _LL228;
_tmp2E9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2E8)->f1;_LL22C: _tmp2AD=
_tmp2E9;_LL228:;}{void*_tmp2ED=(void*)_tmp2B0->r;void*_tmp2EE;struct Cyc_Absyn_Vardecl*
_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Vardecl*
_tmp2F3;void*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;_LL22E: if(*((int*)_tmp2ED)
!= 1)goto _LL230;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if(
_tmp2EE <= (void*)1?1:*((int*)_tmp2EE)!= 0)goto _LL230;_tmp2EF=((struct Cyc_Absyn_Global_b_struct*)
_tmp2EE)->f1;if(!(!_tmp2EF->escapes))goto _LL230;_LL22F: _tmp2F1=_tmp2EF;goto
_LL231;_LL230: if(*((int*)_tmp2ED)!= 1)goto _LL232;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2ED)->f2;if(_tmp2F0 <= (void*)1?1:*((int*)_tmp2F0)!= 3)goto _LL232;_tmp2F1=((
struct Cyc_Absyn_Local_b_struct*)_tmp2F0)->f1;if(!(!_tmp2F1->escapes))goto _LL232;
_LL231: _tmp2F3=_tmp2F1;goto _LL233;_LL232: if(*((int*)_tmp2ED)!= 1)goto _LL234;
_tmp2F2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if(_tmp2F2 <= (void*)
1?1:*((int*)_tmp2F2)!= 4)goto _LL234;_tmp2F3=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2F2)->f1;if(!(!_tmp2F3->escapes))goto _LL234;_LL233: _tmp2F5=_tmp2F3;goto
_LL235;_LL234: if(*((int*)_tmp2ED)!= 1)goto _LL236;_tmp2F4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2ED)->f2;if(_tmp2F4 <= (void*)1?1:*((int*)_tmp2F4)!= 2)goto _LL236;_tmp2F5=((
struct Cyc_Absyn_Param_b_struct*)_tmp2F4)->f1;if(!(!_tmp2F5->escapes))goto _LL236;
_LL235: {void*_tmp2F6=(void*)_tmp2B1->r;void*_tmp2F7;struct Cyc_Absyn_Vardecl*
_tmp2F8;void*_tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FA;void*_tmp2FB;struct Cyc_Absyn_Vardecl*
_tmp2FC;void*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FE;void*_tmp2FF;int _tmp300;
void*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List _tmp303;struct Cyc_Absyn_Exp*
_tmp304;_LL239: if(*((int*)_tmp2F6)!= 1)goto _LL23B;_tmp2F7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2F6)->f2;if(_tmp2F7 <= (void*)1?1:*((int*)_tmp2F7)!= 0)goto _LL23B;_tmp2F8=((
struct Cyc_Absyn_Global_b_struct*)_tmp2F7)->f1;if(!(!_tmp2F8->escapes))goto _LL23B;
_LL23A: _tmp2FA=_tmp2F8;goto _LL23C;_LL23B: if(*((int*)_tmp2F6)!= 1)goto _LL23D;
_tmp2F9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F6)->f2;if(_tmp2F9 <= (void*)
1?1:*((int*)_tmp2F9)!= 3)goto _LL23D;_tmp2FA=((struct Cyc_Absyn_Local_b_struct*)
_tmp2F9)->f1;if(!(!_tmp2FA->escapes))goto _LL23D;_LL23C: _tmp2FC=_tmp2FA;goto
_LL23E;_LL23D: if(*((int*)_tmp2F6)!= 1)goto _LL23F;_tmp2FB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2F6)->f2;if(_tmp2FB <= (void*)1?1:*((int*)_tmp2FB)!= 4)goto _LL23F;_tmp2FC=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2FB)->f1;if(!(!_tmp2FC->escapes))goto _LL23F;
_LL23E: _tmp2FE=_tmp2FC;goto _LL240;_LL23F: if(*((int*)_tmp2F6)!= 1)goto _LL241;
_tmp2FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F6)->f2;if(_tmp2FD <= (void*)
1?1:*((int*)_tmp2FD)!= 2)goto _LL241;_tmp2FE=((struct Cyc_Absyn_Param_b_struct*)
_tmp2FD)->f1;if(!(!_tmp2FE->escapes))goto _LL241;_LL240: {struct Cyc_List_List*
_tmp305=({struct Cyc_List_List*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->vd=
_tmp2F5;_tmp30A->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp30C;_tmp30C.tag=1;_tmp30C.f1=_tmp2FE;_tmp30C;});_tmp30B;}));_tmp30A;});
_tmp309->tl=_tmp2AE;_tmp309;});return({struct _tuple0 _tmp306;_tmp306.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp307=_cycalloc(sizeof(*_tmp307));
_tmp307[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp308;_tmp308.tag=0;
_tmp308.f1=_tmp2AD;_tmp308.f2=_tmp305;_tmp308;});_tmp307;});_tmp306.f2=_tmp2E7;
_tmp306;});}_LL241: if(*((int*)_tmp2F6)!= 0)goto _LL243;_tmp2FF=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2F6)->f1;if(_tmp2FF <= (void*)1?1:*((int*)_tmp2FF)!= 2)goto _LL243;_tmp300=((
struct Cyc_Absyn_Int_c_struct*)_tmp2FF)->f2;_LL242: {struct Cyc_List_List*_tmp30D=({
struct Cyc_List_List*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->vd=_tmp2F5;_tmp312->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessConst_struct*_tmp313=_cycalloc_atomic(sizeof(*
_tmp313));_tmp313[0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp314;_tmp314.tag=
3;_tmp314.f1=(unsigned int)_tmp300;_tmp314;});_tmp313;}));_tmp312;});_tmp311->tl=
_tmp2AE;_tmp311;});return({struct _tuple0 _tmp30E;_tmp30E.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp310;_tmp310.tag=0;_tmp310.f1=_tmp2AD;_tmp310.f2=_tmp30D;_tmp310;});_tmp30F;});
_tmp30E.f2=_tmp2E7;_tmp30E;});}_LL243: if(*((int*)_tmp2F6)!= 3)goto _LL245;_tmp301=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2F6)->f1;if((int)_tmp301 != 19)goto
_LL245;_tmp302=((struct Cyc_Absyn_Primop_e_struct*)_tmp2F6)->f2;if(_tmp302 == 0)
goto _LL245;_tmp303=*_tmp302;_tmp304=(struct Cyc_Absyn_Exp*)_tmp303.hd;_LL244: {
void*_tmp315=(void*)_tmp304->r;void*_tmp316;struct Cyc_Absyn_Vardecl*_tmp317;void*
_tmp318;struct Cyc_Absyn_Vardecl*_tmp319;void*_tmp31A;struct Cyc_Absyn_Vardecl*
_tmp31B;void*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31D;_LL248: if(*((int*)_tmp315)
!= 1)goto _LL24A;_tmp316=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp315)->f2;if(
_tmp316 <= (void*)1?1:*((int*)_tmp316)!= 0)goto _LL24A;_tmp317=((struct Cyc_Absyn_Global_b_struct*)
_tmp316)->f1;if(!(!_tmp317->escapes))goto _LL24A;_LL249: _tmp319=_tmp317;goto
_LL24B;_LL24A: if(*((int*)_tmp315)!= 1)goto _LL24C;_tmp318=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp315)->f2;if(_tmp318 <= (void*)1?1:*((int*)_tmp318)!= 3)goto _LL24C;_tmp319=((
struct Cyc_Absyn_Local_b_struct*)_tmp318)->f1;if(!(!_tmp319->escapes))goto _LL24C;
_LL24B: _tmp31B=_tmp319;goto _LL24D;_LL24C: if(*((int*)_tmp315)!= 1)goto _LL24E;
_tmp31A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp315)->f2;if(_tmp31A <= (void*)
1?1:*((int*)_tmp31A)!= 4)goto _LL24E;_tmp31B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp31A)->f1;if(!(!_tmp31B->escapes))goto _LL24E;_LL24D: _tmp31D=_tmp31B;goto
_LL24F;_LL24E: if(*((int*)_tmp315)!= 1)goto _LL250;_tmp31C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp315)->f2;if(_tmp31C <= (void*)1?1:*((int*)_tmp31C)!= 2)goto _LL250;_tmp31D=((
struct Cyc_Absyn_Param_b_struct*)_tmp31C)->f1;if(!(!_tmp31D->escapes))goto _LL250;
_LL24F: {struct Cyc_List_List*_tmp31E=({struct Cyc_List_List*_tmp322=_cycalloc(
sizeof(*_tmp322));_tmp322->hd=({struct Cyc_CfFlowInfo_Reln*_tmp323=_cycalloc(
sizeof(*_tmp323));_tmp323->vd=_tmp2F5;_tmp323->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp325;_tmp325.tag=2;_tmp325.f1=_tmp31D;_tmp325;});_tmp324;}));_tmp323;});
_tmp322->tl=_tmp2AE;_tmp322;});return({struct _tuple0 _tmp31F;_tmp31F.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp320=_cycalloc(sizeof(*_tmp320));
_tmp320[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp321;_tmp321.tag=0;
_tmp321.f1=_tmp2AD;_tmp321.f2=_tmp31E;_tmp321;});_tmp320;});_tmp31F.f2=_tmp2E7;
_tmp31F;});}_LL250:;_LL251: return({struct _tuple0 _tmp326;_tmp326.f1=_tmp2E6;
_tmp326.f2=_tmp2E7;_tmp326;});_LL247:;}_LL245:;_LL246: return({struct _tuple0
_tmp327;_tmp327.f1=_tmp2E6;_tmp327.f2=_tmp2E7;_tmp327;});_LL238:;}_LL236:;_LL237:
return({struct _tuple0 _tmp328;_tmp328.f1=_tmp2E6;_tmp328.f2=_tmp2E7;_tmp328;});
_LL22D:;}}_LL224: if((int)_tmp2DB != 10)goto _LL226;_LL225: {void*_tmp329=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2AA,_tmp2B0,_tmp2B2,(void*)10,_tmp2B3);void*_tmp32A=Cyc_NewControlFlow_if_tagcmp(
env,_tmp2AA,_tmp2B1,_tmp2B2,(void*)8,_tmp2B2);{void*_tmp32B=_tmp329;struct Cyc_Dict_Dict*
_tmp32C;_LL253: if((int)_tmp32B != 0)goto _LL255;_LL254:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({
struct Cyc_Core_Impossible_struct _tmp32E;_tmp32E.tag=Cyc_Core_Impossible;_tmp32E.f1=({
const char*_tmp32F="anal_test, Lte";_tag_arr(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,15));});_tmp32E;});_tmp32D;}));_LL255: if(_tmp32B <= (
void*)1?1:*((int*)_tmp32B)!= 0)goto _LL252;_tmp32C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp32B)->f1;_LL256: _tmp2AD=_tmp32C;_LL252:;}{void*_tmp330=(void*)_tmp2B0->r;
void*_tmp331;struct Cyc_Absyn_Vardecl*_tmp332;void*_tmp333;struct Cyc_Absyn_Vardecl*
_tmp334;void*_tmp335;struct Cyc_Absyn_Vardecl*_tmp336;void*_tmp337;struct Cyc_Absyn_Vardecl*
_tmp338;_LL258: if(*((int*)_tmp330)!= 1)goto _LL25A;_tmp331=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp330)->f2;if(_tmp331 <= (void*)1?1:*((int*)_tmp331)!= 0)goto _LL25A;_tmp332=((
struct Cyc_Absyn_Global_b_struct*)_tmp331)->f1;if(!(!_tmp332->escapes))goto _LL25A;
_LL259: _tmp334=_tmp332;goto _LL25B;_LL25A: if(*((int*)_tmp330)!= 1)goto _LL25C;
_tmp333=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp330)->f2;if(_tmp333 <= (void*)
1?1:*((int*)_tmp333)!= 3)goto _LL25C;_tmp334=((struct Cyc_Absyn_Local_b_struct*)
_tmp333)->f1;if(!(!_tmp334->escapes))goto _LL25C;_LL25B: _tmp336=_tmp334;goto
_LL25D;_LL25C: if(*((int*)_tmp330)!= 1)goto _LL25E;_tmp335=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp330)->f2;if(_tmp335 <= (void*)1?1:*((int*)_tmp335)!= 4)goto _LL25E;_tmp336=((
struct Cyc_Absyn_Pat_b_struct*)_tmp335)->f1;if(!(!_tmp336->escapes))goto _LL25E;
_LL25D: _tmp338=_tmp336;goto _LL25F;_LL25E: if(*((int*)_tmp330)!= 1)goto _LL260;
_tmp337=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp330)->f2;if(_tmp337 <= (void*)
1?1:*((int*)_tmp337)!= 2)goto _LL260;_tmp338=((struct Cyc_Absyn_Param_b_struct*)
_tmp337)->f1;if(!(!_tmp338->escapes))goto _LL260;_LL25F: {void*_tmp339=(void*)
_tmp2B1->r;void*_tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_List_List _tmp33C;
struct Cyc_Absyn_Exp*_tmp33D;_LL263: if(*((int*)_tmp339)!= 3)goto _LL265;_tmp33A=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp339)->f1;if((int)_tmp33A != 19)goto
_LL265;_tmp33B=((struct Cyc_Absyn_Primop_e_struct*)_tmp339)->f2;if(_tmp33B == 0)
goto _LL265;_tmp33C=*_tmp33B;_tmp33D=(struct Cyc_Absyn_Exp*)_tmp33C.hd;_LL264: {
void*_tmp33E=(void*)_tmp33D->r;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp340;void*
_tmp341;struct Cyc_Absyn_Vardecl*_tmp342;void*_tmp343;struct Cyc_Absyn_Vardecl*
_tmp344;void*_tmp345;struct Cyc_Absyn_Vardecl*_tmp346;_LL268: if(*((int*)_tmp33E)
!= 1)goto _LL26A;_tmp33F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp33E)->f2;if(
_tmp33F <= (void*)1?1:*((int*)_tmp33F)!= 0)goto _LL26A;_tmp340=((struct Cyc_Absyn_Global_b_struct*)
_tmp33F)->f1;if(!(!_tmp340->escapes))goto _LL26A;_LL269: _tmp342=_tmp340;goto
_LL26B;_LL26A: if(*((int*)_tmp33E)!= 1)goto _LL26C;_tmp341=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp33E)->f2;if(_tmp341 <= (void*)1?1:*((int*)_tmp341)!= 3)goto _LL26C;_tmp342=((
struct Cyc_Absyn_Local_b_struct*)_tmp341)->f1;if(!(!_tmp342->escapes))goto _LL26C;
_LL26B: _tmp344=_tmp342;goto _LL26D;_LL26C: if(*((int*)_tmp33E)!= 1)goto _LL26E;
_tmp343=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp33E)->f2;if(_tmp343 <= (void*)
1?1:*((int*)_tmp343)!= 4)goto _LL26E;_tmp344=((struct Cyc_Absyn_Pat_b_struct*)
_tmp343)->f1;if(!(!_tmp344->escapes))goto _LL26E;_LL26D: _tmp346=_tmp344;goto
_LL26F;_LL26E: if(*((int*)_tmp33E)!= 1)goto _LL270;_tmp345=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp33E)->f2;if(_tmp345 <= (void*)1?1:*((int*)_tmp345)!= 2)goto _LL270;_tmp346=((
struct Cyc_Absyn_Param_b_struct*)_tmp345)->f1;if(!(!_tmp346->escapes))goto _LL270;
_LL26F: {struct Cyc_List_List*_tmp347=({struct Cyc_List_List*_tmp34B=_cycalloc(
sizeof(*_tmp34B));_tmp34B->hd=({struct Cyc_CfFlowInfo_Reln*_tmp34C=_cycalloc(
sizeof(*_tmp34C));_tmp34C->vd=_tmp338;_tmp34C->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp34E;_tmp34E.tag=4;_tmp34E.f1=_tmp346;_tmp34E;});_tmp34D;}));_tmp34C;});
_tmp34B->tl=_tmp2AE;_tmp34B;});return({struct _tuple0 _tmp348;_tmp348.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp349=_cycalloc(sizeof(*_tmp349));
_tmp349[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp34A;_tmp34A.tag=0;
_tmp34A.f1=_tmp2AD;_tmp34A.f2=_tmp347;_tmp34A;});_tmp349;});_tmp348.f2=_tmp32A;
_tmp348;});}_LL270:;_LL271: return({struct _tuple0 _tmp34F;_tmp34F.f1=_tmp329;
_tmp34F.f2=_tmp32A;_tmp34F;});_LL267:;}_LL265:;_LL266: return({struct _tuple0
_tmp350;_tmp350.f1=_tmp329;_tmp350.f2=_tmp32A;_tmp350;});_LL262:;}_LL260:;_LL261:
return({struct _tuple0 _tmp351;_tmp351.f1=_tmp329;_tmp351.f2=_tmp32A;_tmp351;});
_LL257:;}}_LL226:;_LL227: return({struct _tuple0 _tmp352;_tmp352.f1=_tmp2AA;_tmp352.f2=
_tmp2AA;_tmp352;});_LL219:;}}_LL1F4:;}}_LL1F2:;_LL1F3: goto _LL1E9;_LL1E9:;}{void*
_tmp354;void*_tmp355;struct _tuple0 _tmp353=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp354=_tmp353.f1;_tmp355=_tmp353.f2;{void*_tmp356=_tmp354;struct Cyc_Dict_Dict*
_tmp357;struct Cyc_List_List*_tmp358;_LL273: if((int)_tmp356 != 0)goto _LL275;_LL274:
return({struct _tuple0 _tmp359;_tmp359.f1=_tmp354;_tmp359.f2=_tmp354;_tmp359;});
_LL275: if(_tmp356 <= (void*)1?1:*((int*)_tmp356)!= 0)goto _LL272;_tmp357=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp356)->f1;_tmp358=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp356)->f2;_LL276: {void*_tmp35A=_tmp355;void*_tmp35B;void*_tmp35C;void*
_tmp35D;_LL278: if((int)_tmp35A != 0)goto _LL27A;_LL279: return({struct _tuple0
_tmp35E;_tmp35E.f1=(void*)0;_tmp35E.f2=_tmp354;_tmp35E;});_LL27A: if((int)_tmp35A
!= 2)goto _LL27C;_LL27B: goto _LL27D;_LL27C: if((int)_tmp35A != 1)goto _LL27E;_LL27D:
goto _LL27F;_LL27E: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 2)goto _LL280;_LL27F:
return({struct _tuple0 _tmp35F;_tmp35F.f1=_tmp354;_tmp35F.f2=(void*)0;_tmp35F;});
_LL280: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 0)goto _LL282;_tmp35B=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp35A)->f1;if((int)_tmp35B != 0)goto
_LL282;_LL281: goto _LL283;_LL282: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 1)
goto _LL284;_tmp35C=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp35A)->f1;if((
int)_tmp35C != 0)goto _LL284;_LL283:({void*_tmp360[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmp361="expression may not be initialized";_tag_arr(_tmp361,sizeof(
char),_get_zero_arr_size(_tmp361,34));}),_tag_arr(_tmp360,sizeof(void*),0));});
return({struct _tuple0 _tmp362;_tmp362.f1=(void*)0;_tmp362.f2=(void*)0;_tmp362;});
_LL284: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 0)goto _LL286;_tmp35D=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp35A)->f1;_LL285: return Cyc_NewControlFlow_splitzero(
env,inflow,_tmp354,e,_tmp35D);_LL286: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 
1)goto _LL288;_LL287: return({struct _tuple0 _tmp363;_tmp363.f1=_tmp354;_tmp363.f2=
_tmp354;_tmp363;});_LL288: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 3)goto
_LL28A;_LL289: return({struct _tuple0 _tmp364;_tmp364.f1=_tmp354;_tmp364.f2=_tmp354;
_tmp364;});_LL28A: if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 4)goto _LL277;_LL28B:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp365=_cycalloc(sizeof(*
_tmp365));_tmp365[0]=({struct Cyc_Core_Impossible_struct _tmp366;_tmp366.tag=Cyc_Core_Impossible;
_tmp366.f1=({const char*_tmp367="anal_test";_tag_arr(_tmp367,sizeof(char),
_get_zero_arr_size(_tmp367,10));});_tmp366;});_tmp365;}));_LL277:;}_LL272:;}}}
static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,
struct Cyc_NewControlFlow_AnalEnv*env,void*flow){void*_tmp368=flow;struct Cyc_Dict_Dict*
_tmp369;_LL28D: if((int)_tmp368 != 0)goto _LL28F;_LL28E: return;_LL28F: if(_tmp368 <= (
void*)1?1:*((int*)_tmp368)!= 0)goto _LL28C;_tmp369=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp368)->f1;_LL290:{struct Cyc_List_List*_tmp36A=env->param_roots;for(0;_tmp36A
!= 0;_tmp36A=_tmp36A->tl){if(Cyc_CfFlowInfo_initlevel(_tmp369,Cyc_CfFlowInfo_lookup_place(
_tmp369,(struct Cyc_CfFlowInfo_Place*)_tmp36A->hd))!= (void*)2)({void*_tmp36B[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp36C="function may not initialize all the parameters with attribute 'initializes'";
_tag_arr(_tmp36C,sizeof(char),_get_zero_arr_size(_tmp36C,76));}),_tag_arr(
_tmp36B,sizeof(void*),0));});}}return;_LL28C:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp36E;struct Cyc_Core_Opt*
_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Stmt*_tmp371;struct Cyc_Absyn_Switch_clause*
_tmp36D=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp36E=*_tmp36D;_tmp36F=_tmp36E.pat_vars;
_tmp370=_tmp36E.where_clause;_tmp371=_tmp36E.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp36F))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp370 != 0){void*_tmp373;void*_tmp374;struct _tuple0
_tmp372=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp370));_tmp373=_tmp372.f1;_tmp374=_tmp372.f2;inflow=_tmp374;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp373,_tmp371);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp371);}{void*
_tmp375=clause_outflow;_LL292: if((int)_tmp375 != 0)goto _LL294;_LL293: goto _LL291;
_LL294:;_LL295: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp376[0]={};Cyc_Tcutil_terr(
_tmp371->loc,({const char*_tmp377="switch clause may implicitly fallthru";
_tag_arr(_tmp377,sizeof(char),_get_zero_arr_size(_tmp377,38));}),_tag_arr(
_tmp376,sizeof(void*),0));});else{({void*_tmp378[0]={};Cyc_Tcutil_warn(_tmp371->loc,({
const char*_tmp379="switch clause may implicitly fallthru";_tag_arr(_tmp379,
sizeof(char),_get_zero_arr_size(_tmp379,38));}),_tag_arr(_tmp378,sizeof(void*),0));});}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL291;_LL291:;}}}return(
void*)0;}static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp37A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);inflow=(void*)_tmp37A->flow;{
void*_tmp37B=(void*)s->r;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;
struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Stmt*_tmp37F;struct Cyc_Absyn_Stmt*
_tmp380;struct Cyc_Absyn_Exp*_tmp381;struct Cyc_Absyn_Stmt*_tmp382;struct Cyc_Absyn_Stmt*
_tmp383;struct _tuple3 _tmp384;struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Stmt*
_tmp386;struct Cyc_Absyn_Stmt*_tmp387;struct Cyc_Absyn_Stmt*_tmp388;struct _tuple3
_tmp389;struct Cyc_Absyn_Exp*_tmp38A;struct Cyc_Absyn_Stmt*_tmp38B;struct Cyc_Absyn_Exp*
_tmp38C;struct _tuple3 _tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct Cyc_Absyn_Stmt*
_tmp38F;struct _tuple3 _tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Stmt*
_tmp392;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Stmt*_tmp394;struct Cyc_List_List*
_tmp395;struct Cyc_Absyn_Switch_clause**_tmp396;struct Cyc_Absyn_Switch_clause*
_tmp397;struct Cyc_Absyn_Stmt*_tmp398;struct Cyc_Absyn_Stmt*_tmp399;struct Cyc_Absyn_Stmt*
_tmp39A;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_Stmt*
_tmp39D;struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_Decl*_tmp39F;struct Cyc_Absyn_Stmt*
_tmp3A0;struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Tvar*_tmp3A2;struct Cyc_Absyn_Vardecl*
_tmp3A3;int _tmp3A4;struct Cyc_Absyn_Stmt*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;
_LL297: if((int)_tmp37B != 0)goto _LL299;_LL298: return inflow;_LL299: if(_tmp37B <= (
void*)1?1:*((int*)_tmp37B)!= 2)goto _LL29B;_tmp37C=((struct Cyc_Absyn_Return_s_struct*)
_tmp37B)->f1;if(_tmp37C != 0)goto _LL29B;_LL29A: if(env->noreturn)({void*_tmp3A7[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp3A8="`noreturn' function might return";
_tag_arr(_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,33));}),_tag_arr(
_tmp3A7,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,
inflow);return(void*)0;_LL29B: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 2)goto
_LL29D;_tmp37D=((struct Cyc_Absyn_Return_s_struct*)_tmp37B)->f1;_LL29C: if(env->noreturn)({
void*_tmp3A9[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3AA="`noreturn' function might return";
_tag_arr(_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,33));}),_tag_arr(
_tmp3A9,sizeof(void*),0));});{void*_tmp3AC;void*_tmp3AD;struct _tuple0 _tmp3AB=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp37D));_tmp3AC=_tmp3AB.f1;
_tmp3AD=_tmp3AB.f2;_tmp3AC=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)
_check_null(_tmp37D))->loc,_tmp3AC,_tmp3AD);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp3AC);return(void*)0;}_LL29D: if(_tmp37B <= (void*)1?1:*((int*)
_tmp37B)!= 0)goto _LL29F;_tmp37E=((struct Cyc_Absyn_Exp_s_struct*)_tmp37B)->f1;
_LL29E: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp37E)).f1;_LL29F: if(
_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 1)goto _LL2A1;_tmp37F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp37B)->f1;_tmp380=((struct Cyc_Absyn_Seq_s_struct*)_tmp37B)->f2;_LL2A0: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp37F),
_tmp380);_LL2A1: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 3)goto _LL2A3;_tmp381=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp37B)->f1;_tmp382=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp37B)->f2;_tmp383=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp37B)->f3;_LL2A2: {
void*_tmp3AF;void*_tmp3B0;struct _tuple0 _tmp3AE=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp381);_tmp3AF=_tmp3AE.f1;_tmp3B0=_tmp3AE.f2;return Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_anal_stmt(env,_tmp3AF,_tmp382),Cyc_NewControlFlow_anal_stmt(
env,_tmp3B0,_tmp383));}_LL2A3: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 4)goto
_LL2A5;_tmp384=((struct Cyc_Absyn_While_s_struct*)_tmp37B)->f1;_tmp385=_tmp384.f1;
_tmp386=_tmp384.f2;_tmp387=((struct Cyc_Absyn_While_s_struct*)_tmp37B)->f2;_LL2A4: {
void*_tmp3B1=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp386))->flow;
void*_tmp3B3;void*_tmp3B4;struct _tuple0 _tmp3B2=Cyc_NewControlFlow_anal_test(env,
_tmp3B1,_tmp385);_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;{void*_tmp3B5=Cyc_NewControlFlow_anal_stmt(
env,_tmp3B3,_tmp387);Cyc_NewControlFlow_update_flow(env,_tmp386,_tmp3B5);return
_tmp3B4;}}_LL2A5: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 16)goto _LL2A7;
_tmp388=((struct Cyc_Absyn_Do_s_struct*)_tmp37B)->f1;_tmp389=((struct Cyc_Absyn_Do_s_struct*)
_tmp37B)->f2;_tmp38A=_tmp389.f1;_tmp38B=_tmp389.f2;_LL2A6: {void*_tmp3B6=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp388);void*_tmp3B7=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3B6,_tmp38B))->flow;void*_tmp3B9;void*_tmp3BA;struct _tuple0 _tmp3B8=Cyc_NewControlFlow_anal_test(
env,_tmp3B7,_tmp38A);_tmp3B9=_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;Cyc_NewControlFlow_update_flow(
env,_tmp388,_tmp3B9);return _tmp3BA;}_LL2A7: if(_tmp37B <= (void*)1?1:*((int*)
_tmp37B)!= 8)goto _LL2A9;_tmp38C=((struct Cyc_Absyn_For_s_struct*)_tmp37B)->f1;
_tmp38D=((struct Cyc_Absyn_For_s_struct*)_tmp37B)->f2;_tmp38E=_tmp38D.f1;_tmp38F=
_tmp38D.f2;_tmp390=((struct Cyc_Absyn_For_s_struct*)_tmp37B)->f3;_tmp391=_tmp390.f1;
_tmp392=_tmp390.f2;_tmp393=((struct Cyc_Absyn_For_s_struct*)_tmp37B)->f4;_LL2A8: {
void*_tmp3BB=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp38C)).f1;void*_tmp3BC=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp3BB,_tmp38F))->flow;void*
_tmp3BE;void*_tmp3BF;struct _tuple0 _tmp3BD=Cyc_NewControlFlow_anal_test(env,
_tmp3BC,_tmp38E);_tmp3BE=_tmp3BD.f1;_tmp3BF=_tmp3BD.f2;{void*_tmp3C0=Cyc_NewControlFlow_anal_stmt(
env,_tmp3BE,_tmp393);void*_tmp3C1=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3C0,_tmp392))->flow;void*_tmp3C2=(Cyc_NewControlFlow_anal_Rexp(env,_tmp3C1,
_tmp391)).f1;Cyc_NewControlFlow_update_flow(env,_tmp38F,_tmp3C2);return _tmp3BF;}}
_LL2A9: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 5)goto _LL2AB;_tmp394=((struct
Cyc_Absyn_Break_s_struct*)_tmp37B)->f1;if(_tmp394 != 0)goto _LL2AB;_LL2AA: return(
void*)0;_LL2AB: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 11)goto _LL2AD;_tmp395=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp37B)->f1;_tmp396=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp37B)->f2;if(_tmp396 == 0)goto _LL2AD;_tmp397=*_tmp396;_LL2AC: {void*_tmp3C4;
struct Cyc_List_List*_tmp3C5;struct _tuple5 _tmp3C3=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp395);_tmp3C4=_tmp3C3.f1;_tmp3C5=_tmp3C3.f2;for(0;_tmp3C5 != 0;(
_tmp3C5=_tmp3C5->tl,_tmp395=_tmp395->tl)){_tmp3C4=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp395))->hd)->loc,
_tmp3C4,(void*)_tmp3C5->hd);}_tmp3C4=Cyc_NewControlFlow_add_vars(_tmp3C4,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp397->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp397->body,_tmp3C4);
return(void*)0;}_LL2AD: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 5)goto _LL2AF;
_tmp398=((struct Cyc_Absyn_Break_s_struct*)_tmp37B)->f1;_LL2AE: _tmp399=_tmp398;
goto _LL2B0;_LL2AF: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 6)goto _LL2B1;
_tmp399=((struct Cyc_Absyn_Continue_s_struct*)_tmp37B)->f1;_LL2B0: _tmp39A=_tmp399;
goto _LL2B2;_LL2B1: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 7)goto _LL2B3;
_tmp39A=((struct Cyc_Absyn_Goto_s_struct*)_tmp37B)->f2;_LL2B2: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp3C6=_tmp37A->encloser;struct Cyc_Absyn_Stmt*
_tmp3C7=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp39A)))->encloser;while(_tmp3C7 != _tmp3C6){struct Cyc_Absyn_Stmt*_tmp3C8=(Cyc_NewControlFlow_get_stmt_annot(
_tmp3C6))->encloser;if(_tmp3C8 == _tmp3C6){({void*_tmp3C9[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp3CA="goto enters local scope or exception handler";_tag_arr(
_tmp3CA,sizeof(char),_get_zero_arr_size(_tmp3CA,45));}),_tag_arr(_tmp3C9,sizeof(
void*),0));});break;}_tmp3C6=_tmp3C8;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp39A),inflow);return(void*)0;_LL2B3: if(
_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 9)goto _LL2B5;_tmp39B=((struct Cyc_Absyn_Switch_s_struct*)
_tmp37B)->f1;_tmp39C=((struct Cyc_Absyn_Switch_s_struct*)_tmp37B)->f2;_LL2B4: {
void*_tmp3CC;void*_tmp3CD;struct _tuple0 _tmp3CB=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp39B);_tmp3CC=_tmp3CB.f1;_tmp3CD=_tmp3CB.f2;_tmp3CC=Cyc_NewControlFlow_use_Rval(
env,_tmp39B->loc,_tmp3CC,_tmp3CD);return Cyc_NewControlFlow_anal_scs(env,_tmp3CC,
_tmp39C);}_LL2B5: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 17)goto _LL2B7;
_tmp39D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp37B)->f1;_tmp39E=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp37B)->f2;_LL2B6: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp39D);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp39E);{
void*_tmp3CE=scs_outflow;_LL2C2: if((int)_tmp3CE != 0)goto _LL2C4;_LL2C3: goto _LL2C1;
_LL2C4:;_LL2C5:({void*_tmp3CF[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3D0="last catch clause may implicitly fallthru";
_tag_arr(_tmp3D0,sizeof(char),_get_zero_arr_size(_tmp3D0,42));}),_tag_arr(
_tmp3CF,sizeof(void*),0));});_LL2C1:;}return s1_outflow;}}}_LL2B7: if(_tmp37B <= (
void*)1?1:*((int*)_tmp37B)!= 12)goto _LL2B9;_tmp39F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp37B)->f1;_tmp3A0=((struct Cyc_Absyn_Decl_s_struct*)_tmp37B)->f2;_LL2B8: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp39F),
_tmp3A0);_LL2B9: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 15)goto _LL2BB;_tmp3A1=((
struct Cyc_Absyn_Label_s_struct*)_tmp37B)->f2;_LL2BA: return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3A1);_LL2BB: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 18)goto
_LL2BD;_tmp3A2=((struct Cyc_Absyn_Region_s_struct*)_tmp37B)->f1;_tmp3A3=((struct
Cyc_Absyn_Region_s_struct*)_tmp37B)->f2;_tmp3A4=((struct Cyc_Absyn_Region_s_struct*)
_tmp37B)->f3;_tmp3A5=((struct Cyc_Absyn_Region_s_struct*)_tmp37B)->f4;_LL2BC: {
struct Cyc_List_List _tmp3D1=({struct Cyc_List_List _tmp3D2;_tmp3D2.hd=_tmp3A3;
_tmp3D2.tl=0;_tmp3D2;});inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)&
_tmp3D1,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp3A5);}_LL2BD: if(_tmp37B <= (void*)1?1:*((int*)_tmp37B)!= 20)goto _LL2BF;
_tmp3A6=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp37B)->f1;_LL2BE: {void*
_tmp3D4;void*_tmp3D5;struct _tuple0 _tmp3D3=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp3A6);_tmp3D4=_tmp3D3.f1;_tmp3D5=_tmp3D3.f2;{void*_tmp3D6=Cyc_NewControlFlow_use_Rval(
env,_tmp3A6->loc,_tmp3D4,_tmp3D5);void*_tmp3D7=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3A6->topt))->v);void*_tmp3D8;_LL2C7: if(
_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 15)goto _LL2C9;_tmp3D8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3D7)->f1;_LL2C8: return Cyc_CfFlowInfo_kill_flow_region(_tmp3D4,_tmp3D8);
_LL2C9:;_LL2CA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3D9=
_cycalloc(sizeof(*_tmp3D9));_tmp3D9[0]=({struct Cyc_Core_Impossible_struct _tmp3DA;
_tmp3DA.tag=Cyc_Core_Impossible;_tmp3DA.f1=({const char*_tmp3DB="anal_stmt -- reset_region";
_tag_arr(_tmp3DB,sizeof(char),_get_zero_arr_size(_tmp3DB,26));});_tmp3DA;});
_tmp3D9;}));_LL2C6:;}}_LL2BF:;_LL2C0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({struct Cyc_Core_Impossible_struct
_tmp3DD;_tmp3DD.tag=Cyc_Core_Impossible;_tmp3DD.f1=({const char*_tmp3DE="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_arr(_tmp3DE,sizeof(char),_get_zero_arr_size(_tmp3DE,56));});_tmp3DD;});
_tmp3DC;}));_LL296:;}}static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp3DF=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp3E0;struct Cyc_Core_Opt*_tmp3E1;struct Cyc_Core_Opt _tmp3E2;struct Cyc_List_List*
_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_List_List*_tmp3E5;_LL2CC: if(*((int*)
_tmp3DF)!= 0)goto _LL2CE;_tmp3E0=((struct Cyc_Absyn_Var_d_struct*)_tmp3DF)->f1;
_LL2CD: {struct Cyc_List_List _tmp3E6=({struct Cyc_List_List _tmp3F4;_tmp3F4.hd=
_tmp3E0;_tmp3F4.tl=0;_tmp3F4;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp3E6,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp3E7=_tmp3E0->initializer;if(_tmp3E7 == 0)return inflow;{void*_tmp3E9;void*
_tmp3EA;struct _tuple0 _tmp3E8=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3E7));_tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;{void*_tmp3EB=
_tmp3E9;struct Cyc_Dict_Dict*_tmp3EC;struct Cyc_List_List*_tmp3ED;_LL2D5: if((int)
_tmp3EB != 0)goto _LL2D7;_LL2D6: return(void*)0;_LL2D7: if(_tmp3EB <= (void*)1?1:*((
int*)_tmp3EB)!= 0)goto _LL2D4;_tmp3EC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3EB)->f1;_tmp3ED=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3EB)->f2;
_LL2D8: _tmp3EC=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp3EC,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp3EF=_cycalloc(sizeof(*
_tmp3EF));_tmp3EF[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp3F0;_tmp3F0.tag=0;
_tmp3F0.f1=_tmp3E0;_tmp3F0;});_tmp3EF;}));_tmp3EE->fields=0;_tmp3EE;}),_tmp3EA);
_tmp3ED=Cyc_CfFlowInfo_reln_assign_var(_tmp3ED,_tmp3E0,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3E0->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3F1=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3F2=_cycalloc(sizeof(*
_tmp3F2));_tmp3F2[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3F3;_tmp3F3.tag=
0;_tmp3F3.f1=_tmp3EC;_tmp3F3.f2=_tmp3ED;_tmp3F3;});_tmp3F2;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp3F1);return(void*)_tmp3F1;}_LL2D4:;}}}}_LL2CE: if(*((int*)_tmp3DF)
!= 2)goto _LL2D0;_tmp3E1=((struct Cyc_Absyn_Let_d_struct*)_tmp3DF)->f2;if(_tmp3E1
== 0)goto _LL2D0;_tmp3E2=*_tmp3E1;_tmp3E3=(struct Cyc_List_List*)_tmp3E2.v;_tmp3E4=((
struct Cyc_Absyn_Let_d_struct*)_tmp3DF)->f3;_LL2CF: {void*_tmp3F6;void*_tmp3F7;
struct _tuple0 _tmp3F5=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp3E4);_tmp3F6=
_tmp3F5.f1;_tmp3F7=_tmp3F5.f2;_tmp3F6=Cyc_NewControlFlow_use_Rval(env,_tmp3E4->loc,
_tmp3F6,_tmp3F7);return Cyc_NewControlFlow_add_vars(_tmp3F6,_tmp3E3,Cyc_CfFlowInfo_unknown_all);}
_LL2D0: if(*((int*)_tmp3DF)!= 3)goto _LL2D2;_tmp3E5=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3DF)->f1;_LL2D1: return Cyc_NewControlFlow_add_vars(inflow,_tmp3E5,Cyc_CfFlowInfo_unknown_none);
_LL2D2:;_LL2D3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3F8=
_cycalloc(sizeof(*_tmp3F8));_tmp3F8[0]=({struct Cyc_Core_Impossible_struct _tmp3F9;
_tmp3F9.tag=Cyc_Core_Impossible;_tmp3F9.f1=({const char*_tmp3FA="anal_decl: unexpected decl variant";
_tag_arr(_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,35));});_tmp3F9;});
_tmp3F8;}));_LL2CB:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp3FB=(void*)s->r;
struct _tuple3 _tmp3FC;struct Cyc_Absyn_Stmt*_tmp3FD;struct _tuple3 _tmp3FE;struct Cyc_Absyn_Stmt*
_tmp3FF;struct Cyc_Absyn_Stmt*_tmp400;struct Cyc_Absyn_Stmt*_tmp401;struct Cyc_Absyn_Stmt*
_tmp402;struct _tuple3 _tmp403;struct Cyc_Absyn_Stmt*_tmp404;struct Cyc_Absyn_Stmt*
_tmp405;struct Cyc_Absyn_Stmt*_tmp406;struct _tuple3 _tmp407;struct Cyc_Absyn_Stmt*
_tmp408;struct Cyc_Absyn_Stmt*_tmp409;struct Cyc_Absyn_Stmt*_tmp40A;struct Cyc_Absyn_Stmt*
_tmp40B;struct Cyc_Absyn_Stmt*_tmp40C;struct Cyc_Absyn_Stmt*_tmp40D;struct Cyc_Absyn_Stmt*
_tmp40E;struct Cyc_List_List*_tmp40F;struct Cyc_List_List*_tmp410;_LL2DA: if(
_tmp3FB <= (void*)1?1:*((int*)_tmp3FB)!= 8)goto _LL2DC;_tmp3FC=((struct Cyc_Absyn_For_s_struct*)
_tmp3FB)->f2;_tmp3FD=_tmp3FC.f2;_tmp3FE=((struct Cyc_Absyn_For_s_struct*)_tmp3FB)->f3;
_tmp3FF=_tmp3FE.f2;_tmp400=((struct Cyc_Absyn_For_s_struct*)_tmp3FB)->f4;_LL2DB:
Cyc_NewControlFlow_postpass(_tmp3FF);_tmp401=_tmp400;_tmp402=_tmp3FD;goto _LL2DD;
_LL2DC: if(_tmp3FB <= (void*)1?1:*((int*)_tmp3FB)!= 1)goto _LL2DE;_tmp401=((struct
Cyc_Absyn_Seq_s_struct*)_tmp3FB)->f1;_tmp402=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3FB)->f2;_LL2DD: _tmp404=_tmp401;_tmp405=_tmp402;goto _LL2DF;_LL2DE: if(_tmp3FB
<= (void*)1?1:*((int*)_tmp3FB)!= 4)goto _LL2E0;_tmp403=((struct Cyc_Absyn_While_s_struct*)
_tmp3FB)->f1;_tmp404=_tmp403.f2;_tmp405=((struct Cyc_Absyn_While_s_struct*)
_tmp3FB)->f2;_LL2DF: _tmp406=_tmp405;_tmp408=_tmp404;goto _LL2E1;_LL2E0: if(_tmp3FB
<= (void*)1?1:*((int*)_tmp3FB)!= 16)goto _LL2E2;_tmp406=((struct Cyc_Absyn_Do_s_struct*)
_tmp3FB)->f1;_tmp407=((struct Cyc_Absyn_Do_s_struct*)_tmp3FB)->f2;_tmp408=_tmp407.f2;
_LL2E1: _tmp409=_tmp406;_tmp40A=_tmp408;goto _LL2E3;_LL2E2: if(_tmp3FB <= (void*)1?1:*((
int*)_tmp3FB)!= 3)goto _LL2E4;_tmp409=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3FB)->f2;_tmp40A=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3FB)->f3;_LL2E3:
Cyc_NewControlFlow_postpass(_tmp409);_tmp40B=_tmp40A;goto _LL2E5;_LL2E4: if(
_tmp3FB <= (void*)1?1:*((int*)_tmp3FB)!= 18)goto _LL2E6;_tmp40B=((struct Cyc_Absyn_Region_s_struct*)
_tmp3FB)->f4;_LL2E5: _tmp40C=_tmp40B;goto _LL2E7;_LL2E6: if(_tmp3FB <= (void*)1?1:*((
int*)_tmp3FB)!= 15)goto _LL2E8;_tmp40C=((struct Cyc_Absyn_Label_s_struct*)_tmp3FB)->f2;
_LL2E7: _tmp40D=_tmp40C;goto _LL2E9;_LL2E8: if(_tmp3FB <= (void*)1?1:*((int*)_tmp3FB)
!= 12)goto _LL2EA;_tmp40D=((struct Cyc_Absyn_Decl_s_struct*)_tmp3FB)->f2;_LL2E9:
Cyc_NewControlFlow_postpass(_tmp40D);return;_LL2EA: if(_tmp3FB <= (void*)1?1:*((
int*)_tmp3FB)!= 17)goto _LL2EC;_tmp40E=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3FB)->f1;_tmp40F=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3FB)->f2;_LL2EB:
Cyc_NewControlFlow_postpass(_tmp40E);_tmp410=_tmp40F;goto _LL2ED;_LL2EC: if(
_tmp3FB <= (void*)1?1:*((int*)_tmp3FB)!= 9)goto _LL2EE;_tmp410=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3FB)->f2;_LL2ED: for(0;_tmp410 != 0;_tmp410=_tmp410->tl){Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*)_tmp410->hd)->body);}return;_LL2EE:;_LL2EF:
return;_LL2D9:;}}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Position_Segment*_tmp411=(fd->body)->loc;void*inflow=Cyc_NewControlFlow_add_vars((
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp446=_cycalloc(sizeof(*
_tmp446));_tmp446[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp447;_tmp447.tag=
0;_tmp447.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp447.f2=0;_tmp447;});
_tmp446;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);struct Cyc_List_List*_tmp412=0;{void*_tmp413=inflow;
struct Cyc_Dict_Dict*_tmp414;struct Cyc_List_List*_tmp415;_LL2F1: if((int)_tmp413 != 
0)goto _LL2F3;_LL2F2:({void*_tmp416[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp417="check_fun";_tag_arr(
_tmp417,sizeof(char),_get_zero_arr_size(_tmp417,10));}),_tag_arr(_tmp416,sizeof(
void*),0));});_LL2F3: if(_tmp413 <= (void*)1?1:*((int*)_tmp413)!= 0)goto _LL2F0;
_tmp414=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp413)->f1;_tmp415=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp413)->f2;_LL2F4: {struct Cyc_List_List*
atts;{void*_tmp418=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp419;struct Cyc_List_List*_tmp41A;
_LL2F6: if(_tmp418 <= (void*)3?1:*((int*)_tmp418)!= 8)goto _LL2F8;_tmp419=((struct
Cyc_Absyn_FnType_struct*)_tmp418)->f1;_tmp41A=_tmp419.attributes;_LL2F7: atts=
_tmp41A;goto _LL2F5;_LL2F8:;_LL2F9:({void*_tmp41B[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp41C="check_fun: non-function type cached with fndecl_t";
_tag_arr(_tmp41C,sizeof(char),_get_zero_arr_size(_tmp41C,50));}),_tag_arr(
_tmp41B,sizeof(void*),0));});_LL2F5:;}for(0;atts != 0;atts=atts->tl){void*_tmp41D=(
void*)atts->hd;int _tmp41E;_LL2FB: if(_tmp41D <= (void*)17?1:*((int*)_tmp41D)!= 4)
goto _LL2FD;_tmp41E=((struct Cyc_Absyn_Initializes_att_struct*)_tmp41D)->f1;_LL2FC: {
unsigned int j=(unsigned int)_tmp41E;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp41F[0]={};Cyc_Tcutil_terr(_tmp411,({const char*_tmp420="initializes attribute exceeds number of parameters";
_tag_arr(_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,51));}),_tag_arr(
_tmp41F,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp421=((struct
Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*
_tmp422=Cyc_Tcutil_compress((void*)_tmp421->type);struct Cyc_Absyn_PtrInfo _tmp423;
void*_tmp424;struct Cyc_Absyn_PtrAtts _tmp425;struct Cyc_Absyn_Conref*_tmp426;
struct Cyc_Absyn_Conref*_tmp427;struct Cyc_Absyn_Conref*_tmp428;_LL300: if(_tmp422
<= (void*)3?1:*((int*)_tmp422)!= 4)goto _LL302;_tmp423=((struct Cyc_Absyn_PointerType_struct*)
_tmp422)->f1;_tmp424=(void*)_tmp423.elt_typ;_tmp425=_tmp423.ptr_atts;_tmp426=
_tmp425.nullable;_tmp427=_tmp425.bounds;_tmp428=_tmp425.zero_term;_LL301: if(((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp426))({void*_tmp429[0]={};
Cyc_Tcutil_terr(_tmp411,({const char*_tmp42A="initializes attribute not allowed on nullable pointers";
_tag_arr(_tmp42A,sizeof(char),_get_zero_arr_size(_tmp42A,55));}),_tag_arr(
_tmp429,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp427))({void*_tmp42B[
0]={};Cyc_Tcutil_terr(_tmp411,({const char*_tmp42C="initializes attribute allowed only on pointers of size 1";
_tag_arr(_tmp42C,sizeof(char),_get_zero_arr_size(_tmp42C,57));}),_tag_arr(
_tmp42B,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp428))({void*_tmp42D[0]={};Cyc_Tcutil_terr(_tmp411,({const char*_tmp42E="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_arr(_tmp42E,sizeof(char),_get_zero_arr_size(_tmp42E,77));}),_tag_arr(
_tmp42D,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp42F=({
struct Cyc_CfFlowInfo_InitParam_struct*_tmp437=_cycalloc(sizeof(*_tmp437));
_tmp437[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp438;_tmp438.tag=2;_tmp438.f1=(
int)j;_tmp438.f2=(void*)_tmp424;_tmp438;});_tmp437;});struct Cyc_CfFlowInfo_Place*
_tmp430=({struct Cyc_CfFlowInfo_Place*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->root=(
void*)((void*)_tmp42F);_tmp436->fields=0;_tmp436;});_tmp414=Cyc_Dict_insert(
_tmp414,(void*)_tmp42F,Cyc_CfFlowInfo_typ_to_absrval(_tmp424,Cyc_CfFlowInfo_esc_none));
_tmp414=Cyc_Dict_insert(_tmp414,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp432;_tmp432.tag=0;_tmp432.f1=_tmp421;_tmp432;});_tmp431;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp434;_tmp434.tag=2;_tmp434.f1=_tmp430;
_tmp434;});_tmp433;}));_tmp412=({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*
_tmp435));_tmp435->hd=_tmp430;_tmp435->tl=_tmp412;_tmp435;});goto _LL2FF;}_LL302:;
_LL303:({void*_tmp439[0]={};Cyc_Tcutil_terr(_tmp411,({const char*_tmp43A="initializes attribute on non-pointer";
_tag_arr(_tmp43A,sizeof(char),_get_zero_arr_size(_tmp43A,37));}),_tag_arr(
_tmp439,sizeof(void*),0));});_LL2FF:;}goto _LL2FA;}}_LL2FD:;_LL2FE: goto _LL2FA;
_LL2FA:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp43B=
_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp43C;_tmp43C.tag=0;_tmp43C.f1=_tmp414;_tmp43C.f2=_tmp415;_tmp43C;});_tmp43B;});}
_LL2F0:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct
Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp445=
_cycalloc(sizeof(*_tmp445));_tmp445->iterate_again=1;_tmp445->iteration_num=0;
_tmp445->in_try=0;_tmp445->tryflow=(void*)inflow;_tmp445->all_changed=0;_tmp445->noreturn=
noreturn;_tmp445->param_roots=_tmp412;_tmp445;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp43D=outflow;_LL305: if((int)_tmp43D != 0)goto
_LL307;_LL306: goto _LL304;_LL307:;_LL308: Cyc_NewControlFlow_check_init_params(
_tmp411,env,outflow);if(noreturn)({void*_tmp43E[0]={};Cyc_Tcutil_terr(_tmp411,({
const char*_tmp43F="`noreturn' function might (implicitly) return";_tag_arr(
_tmp43F,sizeof(char),_get_zero_arr_size(_tmp43F,46));}),_tag_arr(_tmp43E,sizeof(
void*),0));});else{void*_tmp440=Cyc_Tcutil_compress((void*)fd->ret_type);_LL30A:
if((int)_tmp440 != 0)goto _LL30C;_LL30B: goto _LL309;_LL30C: if(_tmp440 <= (void*)3?1:*((
int*)_tmp440)!= 6)goto _LL30E;_LL30D: goto _LL30F;_LL30E: if((int)_tmp440 != 1)goto
_LL310;_LL30F: goto _LL311;_LL310: if(_tmp440 <= (void*)3?1:*((int*)_tmp440)!= 5)
goto _LL312;_LL311:({void*_tmp441[0]={};Cyc_Tcutil_warn(_tmp411,({const char*
_tmp442="function may not return a value";_tag_arr(_tmp442,sizeof(char),
_get_zero_arr_size(_tmp442,32));}),_tag_arr(_tmp441,sizeof(void*),0));});goto
_LL309;_LL312:;_LL313:({void*_tmp443[0]={};Cyc_Tcutil_terr(_tmp411,({const char*
_tmp444="function may not return a value";_tag_arr(_tmp444,sizeof(char),
_get_zero_arr_size(_tmp444,32));}),_tag_arr(_tmp443,sizeof(void*),0));});goto
_LL309;_LL309:;}goto _LL304;_LL304:;}Cyc_NewControlFlow_postpass(fd->body);}}void
Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*
_tmp448=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp449;
struct Cyc_List_List*_tmp44A;struct Cyc_List_List*_tmp44B;struct Cyc_List_List*
_tmp44C;_LL315: if(*((int*)_tmp448)!= 1)goto _LL317;_tmp449=((struct Cyc_Absyn_Fn_d_struct*)
_tmp448)->f1;_LL316: Cyc_NewControlFlow_check_fun(_tmp449);goto _LL314;_LL317: if(*((
int*)_tmp448)!= 10)goto _LL319;_tmp44A=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp448)->f1;_LL318: _tmp44B=_tmp44A;goto _LL31A;_LL319: if(*((int*)_tmp448)!= 9)
goto _LL31B;_tmp44B=((struct Cyc_Absyn_Using_d_struct*)_tmp448)->f2;_LL31A: _tmp44C=
_tmp44B;goto _LL31C;_LL31B: if(*((int*)_tmp448)!= 8)goto _LL31D;_tmp44C=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp448)->f2;_LL31C: Cyc_NewControlFlow_cf_check(_tmp44C);goto _LL314;_LL31D:;
_LL31E: goto _LL314;_LL314:;}}
