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
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)
outer_all_changed),this_all_changed);_tmp2D;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple5 _tmp2E;_tmp2E.f1=outflow;_tmp2E.f2=Cyc_List_imp_rev(
rvals);_tmp2E;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
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
_tmp4C;void*_tmp4D;struct Cyc_Absyn_Conref*_tmp4E;struct Cyc_Absyn_Conref*_tmp4F;
_LL29: if(_tmp4B <= (void*)3?1:*((int*)_tmp4B)!= 4)goto _LL2B;_tmp4C=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B)->f1;_tmp4D=(void*)_tmp4C.elt_typ;_tmp4E=_tmp4C.bounds;_tmp4F=_tmp4C.zero_term;
_LL2A: {void*_tmp50=f;struct Cyc_Dict_Dict*_tmp51;struct Cyc_List_List*_tmp52;
_LL2E: if((int)_tmp50 != 0)goto _LL30;_LL2F: return({struct _tuple0 _tmp53;_tmp53.f1=f;
_tmp53.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp4D,Cyc_CfFlowInfo_unknown_all);
_tmp53;});_LL30: if(_tmp50 <= (void*)1?1:*((int*)_tmp50)!= 0)goto _LL2D;_tmp51=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f1;_tmp52=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp50)->f2;_LL31: if(Cyc_Tcutil_is_bound_one(_tmp4E)){void*_tmp54=r;struct Cyc_CfFlowInfo_Place*
_tmp55;void*_tmp56;_LL33: if((int)_tmp54 != 1)goto _LL35;_LL34: goto _LL36;_LL35: if((
int)_tmp54 != 2)goto _LL37;_LL36:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp58;_tmp58.tag=Cyc_CfFlowInfo_NotZero;_tmp58.f1=_tmp52;_tmp58;});_tmp57;})));
goto _LL32;_LL37: if(_tmp54 <= (void*)3?1:*((int*)_tmp54)!= 2)goto _LL39;_tmp55=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp54)->f1;_LL38:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp59=_cycalloc(sizeof(*_tmp59));
_tmp59[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp5A;_tmp5A.tag=Cyc_CfFlowInfo_NotZero;
_tmp5A.f1=_tmp52;_tmp5A;});_tmp59;})));return({struct _tuple0 _tmp5B;_tmp5B.f1=f;
_tmp5B.f2=Cyc_CfFlowInfo_lookup_place(_tmp51,_tmp55);_tmp5B;});_LL39: if((int)
_tmp54 != 0)goto _LL3B;_LL3A:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*
_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp5D;_tmp5D.tag=Cyc_CfFlowInfo_IsZero;_tmp5D.f1=_tmp52;_tmp5D;});_tmp5C;})));
return({struct _tuple0 _tmp5E;_tmp5E.f1=(void*)0;_tmp5E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4D,Cyc_CfFlowInfo_unknown_all);_tmp5E;});_LL3B: if(_tmp54 <= (void*)3?1:*((int*)
_tmp54)!= 0)goto _LL3D;_tmp56=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp54)->f1;_LL3C: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp56);goto _LL3E;
_LL3D:;_LL3E:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp60;_tmp60.tag=Cyc_CfFlowInfo_UnknownZ;_tmp60.f1=_tmp52;_tmp60;});_tmp5F;})));
_LL32:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp62;_tmp62.tag=Cyc_CfFlowInfo_UnknownZ;_tmp62.f1=_tmp52;_tmp62;});_tmp61;})));}{
void*_tmp63=Cyc_CfFlowInfo_initlevel(_tmp51,r);_LL40: if((int)_tmp63 != 0)goto
_LL42;_LL41:({void*_tmp64[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp65="dereference of possibly uninitialized pointer";
_tag_arr(_tmp65,sizeof(char),_get_zero_arr_size(_tmp65,46));}),_tag_arr(_tmp64,
sizeof(void*),0));});goto _LL43;_LL42: if((int)_tmp63 != 2)goto _LL44;_LL43: return({
struct _tuple0 _tmp66;_tmp66.f1=f;_tmp66.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp4D,
Cyc_CfFlowInfo_unknown_all);_tmp66;});_LL44: if((int)_tmp63 != 1)goto _LL3F;_LL45:
return({struct _tuple0 _tmp67;_tmp67.f1=f;_tmp67.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4D,Cyc_CfFlowInfo_unknown_none);_tmp67;});_LL3F:;}_LL2D:;}_LL2B:;_LL2C:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68[0]=({struct Cyc_Core_Impossible_struct _tmp69;_tmp69.tag=Cyc_Core_Impossible;
_tmp69.f1=({const char*_tmp6A="right deref of non-pointer-type";_tag_arr(_tmp6A,
sizeof(char),_get_zero_arr_size(_tmp6A,32));});_tmp69;});_tmp68;}));_LL28:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp6B=(void*)e1->r;
void*_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6D;void*_tmp6E;struct Cyc_Absyn_Vardecl*
_tmp6F;void*_tmp70;struct Cyc_Absyn_Vardecl*_tmp71;void*_tmp72;struct Cyc_Absyn_Vardecl*
_tmp73;_LL47: if(*((int*)_tmp6B)!= 1)goto _LL49;_tmp6C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp6B)->f2;if(_tmp6C <= (void*)1?1:*((int*)_tmp6C)!= 4)goto _LL49;_tmp6D=((struct
Cyc_Absyn_Pat_b_struct*)_tmp6C)->f1;_LL48: _tmp6F=_tmp6D;goto _LL4A;_LL49: if(*((
int*)_tmp6B)!= 1)goto _LL4B;_tmp6E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6B)->f2;
if(_tmp6E <= (void*)1?1:*((int*)_tmp6E)!= 3)goto _LL4B;_tmp6F=((struct Cyc_Absyn_Local_b_struct*)
_tmp6E)->f1;_LL4A: _tmp71=_tmp6F;goto _LL4C;_LL4B: if(*((int*)_tmp6B)!= 1)goto _LL4D;
_tmp70=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6B)->f2;if(_tmp70 <= (void*)1?1:*((
int*)_tmp70)!= 2)goto _LL4D;_tmp71=((struct Cyc_Absyn_Param_b_struct*)_tmp70)->f1;
_LL4C: _tmp73=_tmp71;goto _LL4E;_LL4D: if(*((int*)_tmp6B)!= 1)goto _LL4F;_tmp72=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6B)->f2;if(_tmp72 <= (void*)1?1:*((int*)
_tmp72)!= 0)goto _LL4F;_tmp73=((struct Cyc_Absyn_Global_b_struct*)_tmp72)->f1;
_LL4E: if(!_tmp73->escapes){void*_tmp74=(void*)e2->r;void*_tmp75;struct Cyc_Absyn_Vardecl*
_tmp76;void*_tmp77;struct Cyc_Absyn_Vardecl*_tmp78;void*_tmp79;struct Cyc_Absyn_Vardecl*
_tmp7A;void*_tmp7B;struct Cyc_Absyn_Vardecl*_tmp7C;_LL52: if(*((int*)_tmp74)!= 1)
goto _LL54;_tmp75=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp74)->f2;if(_tmp75 <= (
void*)1?1:*((int*)_tmp75)!= 4)goto _LL54;_tmp76=((struct Cyc_Absyn_Pat_b_struct*)
_tmp75)->f1;_LL53: _tmp78=_tmp76;goto _LL55;_LL54: if(*((int*)_tmp74)!= 1)goto _LL56;
_tmp77=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp74)->f2;if(_tmp77 <= (void*)1?1:*((
int*)_tmp77)!= 3)goto _LL56;_tmp78=((struct Cyc_Absyn_Local_b_struct*)_tmp77)->f1;
_LL55: _tmp7A=_tmp78;goto _LL57;_LL56: if(*((int*)_tmp74)!= 1)goto _LL58;_tmp79=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp74)->f2;if(_tmp79 <= (void*)1?1:*((int*)
_tmp79)!= 2)goto _LL58;_tmp7A=((struct Cyc_Absyn_Param_b_struct*)_tmp79)->f1;_LL57:
_tmp7C=_tmp7A;goto _LL59;_LL58: if(*((int*)_tmp74)!= 1)goto _LL5A;_tmp7B=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp74)->f2;if(_tmp7B <= (void*)1?1:*((int*)_tmp7B)
!= 0)goto _LL5A;_tmp7C=((struct Cyc_Absyn_Global_b_struct*)_tmp7B)->f1;_LL59: if(!
_tmp7C->escapes){int found=0;{struct Cyc_List_List*_tmp7D=relns;for(0;_tmp7D != 0;
_tmp7D=_tmp7D->tl){struct Cyc_CfFlowInfo_Reln*_tmp7E=(struct Cyc_CfFlowInfo_Reln*)
_tmp7D->hd;if(_tmp7E->vd == _tmp7C){void*_tmp7F=(void*)_tmp7E->rop;struct Cyc_Absyn_Vardecl*
_tmp80;_LL5D: if(*((int*)_tmp7F)!= 2)goto _LL5F;_tmp80=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp7F)->f1;if(!(_tmp80 == _tmp73))goto _LL5F;_LL5E: return relns;_LL5F:;_LL60:
continue;_LL5C:;}}}if(!found)return({struct Cyc_List_List*_tmp81=_cycalloc(
sizeof(*_tmp81));_tmp81->hd=({struct Cyc_CfFlowInfo_Reln*_tmp82=_cycalloc(sizeof(*
_tmp82));_tmp82->vd=_tmp7C;_tmp82->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp84;_tmp84.tag=2;_tmp84.f1=_tmp73;_tmp84;});_tmp83;}));_tmp82;});_tmp81->tl=
relns;_tmp81;});}return relns;_LL5A:;_LL5B: return relns;_LL51:;}else{return relns;}
_LL4F:;_LL50: return relns;_LL46:;}struct _tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;{void*_tmp85=inflow;struct Cyc_Dict_Dict*_tmp86;struct Cyc_List_List*_tmp87;
_LL62: if((int)_tmp85 != 0)goto _LL64;_LL63: return({struct _tuple0 _tmp88;_tmp88.f1=(
void*)0;_tmp88.f2=Cyc_CfFlowInfo_unknown_all;_tmp88;});_LL64: if(_tmp85 <= (void*)
1?1:*((int*)_tmp85)!= 0)goto _LL61;_tmp86=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp85)->f1;_tmp87=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp85)->f2;_LL65:
d=_tmp86;relns=_tmp87;_LL61:;}{void*_tmp89=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp8A;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;void*_tmp8D;void*
_tmp8E;void*_tmp8F;int _tmp90;void*_tmp91;void*_tmp92;void*_tmp93;struct Cyc_List_List*
_tmp94;void*_tmp95;void*_tmp96;struct Cyc_Absyn_Vardecl*_tmp97;void*_tmp98;struct
Cyc_Absyn_Vardecl*_tmp99;void*_tmp9A;struct Cyc_Absyn_Vardecl*_tmp9B;struct Cyc_List_List*
_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Core_Opt*
_tmp9F;struct Cyc_Core_Opt _tmpA0;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*
_tmpA2;struct Cyc_Core_Opt*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Exp*
_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*
_tmpA8;struct Cyc_List_List*_tmpA9;struct Cyc_Absyn_MallocInfo _tmpAA;int _tmpAB;
struct Cyc_Absyn_Exp*_tmpAC;void**_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;int _tmpAF;
struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct _tagged_arr*_tmpB5;
struct Cyc_Absyn_Exp*_tmpB6;struct _tagged_arr*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;
struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;
struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_List_List*_tmpBE;
struct Cyc_List_List*_tmpBF;struct Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpC1;
struct Cyc_Absyn_Vardecl*_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*
_tmpC4;int _tmpC5;struct Cyc_Absyn_Stmt*_tmpC6;void*_tmpC7;_LL67: if(*((int*)_tmp89)
!= 13)goto _LL69;_tmp8A=((struct Cyc_Absyn_Cast_e_struct*)_tmp89)->f2;_LL68: _tmp8B=
_tmp8A;goto _LL6A;_LL69: if(*((int*)_tmp89)!= 11)goto _LL6B;_tmp8B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp89)->f1;_LL6A: _tmp8C=_tmp8B;goto _LL6C;_LL6B: if(*((int*)_tmp89)!= 12)goto
_LL6D;_tmp8C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp89)->f1;_LL6C: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp8C);_LL6D: if(*((int*)_tmp89)!= 0)goto _LL6F;_tmp8D=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp89)->f1;if((int)_tmp8D != 0)goto _LL6F;_LL6E: goto
_LL70;_LL6F: if(*((int*)_tmp89)!= 0)goto _LL71;_tmp8E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp89)->f1;if(_tmp8E <= (void*)1?1:*((int*)_tmp8E)!= 2)goto _LL71;_tmp8F=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp8E)->f1;if((int)_tmp8F != 0)goto _LL71;_tmp90=((
struct Cyc_Absyn_Int_c_struct*)_tmp8E)->f2;if(_tmp90 != 0)goto _LL71;_LL70: return({
struct _tuple0 _tmpC8;_tmpC8.f1=inflow;_tmpC8.f2=(void*)0;_tmpC8;});_LL71: if(*((
int*)_tmp89)!= 0)goto _LL73;_tmp91=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp89)->f1;if(_tmp91 <= (void*)1?1:*((int*)_tmp91)!= 2)goto _LL73;_tmp92=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp91)->f1;if((int)_tmp92 != 0)goto _LL73;_LL72:
goto _LL74;_LL73: if(*((int*)_tmp89)!= 1)goto _LL75;_tmp93=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp89)->f2;if(_tmp93 <= (void*)1?1:*((int*)_tmp93)!= 1)goto _LL75;_LL74: return({
struct _tuple0 _tmpC9;_tmpC9.f1=inflow;_tmpC9.f2=(void*)1;_tmpC9;});_LL75: if(*((
int*)_tmp89)!= 30)goto _LL77;_tmp94=((struct Cyc_Absyn_Tunion_e_struct*)_tmp89)->f1;
if(_tmp94 != 0)goto _LL77;_LL76: goto _LL78;_LL77: if(*((int*)_tmp89)!= 0)goto _LL79;
_LL78: goto _LL7A;_LL79: if(*((int*)_tmp89)!= 17)goto _LL7B;_LL7A: goto _LL7C;_LL7B:
if(*((int*)_tmp89)!= 16)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(*((int*)_tmp89)!= 18)
goto _LL7F;_LL7E: goto _LL80;_LL7F: if(*((int*)_tmp89)!= 19)goto _LL81;_LL80: goto
_LL82;_LL81: if(*((int*)_tmp89)!= 32)goto _LL83;_LL82: goto _LL84;_LL83: if(*((int*)
_tmp89)!= 31)goto _LL85;_LL84: return({struct _tuple0 _tmpCA;_tmpCA.f1=inflow;_tmpCA.f2=
Cyc_CfFlowInfo_unknown_all;_tmpCA;});_LL85: if(*((int*)_tmp89)!= 1)goto _LL87;
_tmp95=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp95 <= (void*)1?1:*((
int*)_tmp95)!= 0)goto _LL87;_LL86: return({struct _tuple0 _tmpCB;_tmpCB.f1=inflow;
_tmpCB.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmpCB;});_LL87: if(*((int*)_tmp89)!= 1)
goto _LL89;_tmp96=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp96 <= (
void*)1?1:*((int*)_tmp96)!= 2)goto _LL89;_tmp97=((struct Cyc_Absyn_Param_b_struct*)
_tmp96)->f1;_LL88: _tmp99=_tmp97;goto _LL8A;_LL89: if(*((int*)_tmp89)!= 1)goto _LL8B;
_tmp98=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp98 <= (void*)1?1:*((
int*)_tmp98)!= 3)goto _LL8B;_tmp99=((struct Cyc_Absyn_Local_b_struct*)_tmp98)->f1;
_LL8A: _tmp9B=_tmp99;goto _LL8C;_LL8B: if(*((int*)_tmp89)!= 1)goto _LL8D;_tmp9A=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp89)->f2;if(_tmp9A <= (void*)1?1:*((int*)
_tmp9A)!= 4)goto _LL8D;_tmp9B=((struct Cyc_Absyn_Pat_b_struct*)_tmp9A)->f1;_LL8C:
return({struct _tuple0 _tmpCC;_tmpCC.f1=inflow;_tmpCC.f2=Cyc_Dict_lookup(d,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=_tmp9B;_tmpCE;});
_tmpCD;}));_tmpCC;});_LL8D: if(*((int*)_tmp89)!= 3)goto _LL8F;_tmp9C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp89)->f2;_LL8E: return Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp9C);
_LL8F: if(*((int*)_tmp89)!= 5)goto _LL91;_tmp9D=((struct Cyc_Absyn_Increment_e_struct*)
_tmp89)->f1;_LL90: {void*_tmpD1;struct _tuple0 _tmpD0=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpCF[1];_tmpCF[0]=_tmp9D;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpCF,sizeof(struct Cyc_Absyn_Exp*),1));}));
_tmpD1=_tmpD0.f1;{void*_tmpD3;struct _tuple0 _tmpD2=Cyc_NewControlFlow_anal_Lexp(
env,_tmpD1,_tmp9D);_tmpD3=_tmpD2.f2;{struct _tuple0 _tmpD5=({struct _tuple0 _tmpD4;
_tmpD4.f1=_tmpD3;_tmpD4.f2=_tmpD1;_tmpD4;});void*_tmpD6;struct Cyc_CfFlowInfo_Place*
_tmpD7;void*_tmpD8;struct Cyc_Dict_Dict*_tmpD9;struct Cyc_List_List*_tmpDA;_LLC8:
_tmpD6=_tmpD5.f1;if(_tmpD6 <= (void*)1?1:*((int*)_tmpD6)!= 0)goto _LLCA;_tmpD7=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpD6)->f1;_tmpD8=_tmpD5.f2;if(_tmpD8 <= (
void*)1?1:*((int*)_tmpD8)!= 0)goto _LLCA;_tmpD9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpD8)->f1;_tmpDA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpD8)->f2;_LLC9:
_tmpDA=Cyc_CfFlowInfo_reln_kill_exp(_tmpDA,_tmp9D);_tmpD1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpDC;_tmpDC.tag=0;_tmpDC.f1=Cyc_CfFlowInfo_assign_place(_tmp9D->loc,_tmpD9,env->all_changed,
_tmpD7,Cyc_CfFlowInfo_unknown_all);_tmpDC.f2=_tmpDA;_tmpDC;});_tmpDB;});goto
_LLC7;_LLCA:;_LLCB: goto _LLC7;_LLC7:;}return({struct _tuple0 _tmpDD;_tmpDD.f1=
_tmpD1;_tmpDD.f2=Cyc_CfFlowInfo_unknown_all;_tmpDD;});}}_LL91: if(*((int*)_tmp89)
!= 4)goto _LL93;_tmp9E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp89)->f1;_tmp9F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp89)->f2;if(_tmp9F == 0)goto _LL93;_tmpA0=*
_tmp9F;_tmpA1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp89)->f3;_LL92: {void*
_tmpE0;struct _tuple0 _tmpDF=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct
Cyc_Absyn_Exp*_tmpDE[2];_tmpDE[1]=_tmpA1;_tmpDE[0]=_tmp9E;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpDE,sizeof(struct Cyc_Absyn_Exp*),2));}));
_tmpE0=_tmpDF.f1;{void*_tmpE2;struct _tuple0 _tmpE1=Cyc_NewControlFlow_anal_Lexp(
env,_tmpE0,e);_tmpE2=_tmpE1.f2;{struct _tuple0 _tmpE4=({struct _tuple0 _tmpE3;_tmpE3.f1=
_tmpE2;_tmpE3.f2=_tmpE0;_tmpE3;});void*_tmpE5;struct Cyc_CfFlowInfo_Place*_tmpE6;
void*_tmpE7;struct Cyc_Dict_Dict*_tmpE8;struct Cyc_List_List*_tmpE9;_LLCD: _tmpE5=
_tmpE4.f1;if(_tmpE5 <= (void*)1?1:*((int*)_tmpE5)!= 0)goto _LLCF;_tmpE6=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmpE5)->f1;_tmpE7=_tmpE4.f2;if(_tmpE7 <= (void*)1?
1:*((int*)_tmpE7)!= 0)goto _LLCF;_tmpE8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpE7)->f1;_tmpE9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpE7)->f2;_LLCE:
_tmpE9=Cyc_CfFlowInfo_reln_kill_exp(_tmpE9,_tmp9E);_tmpE0=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpEB;_tmpEB.tag=0;_tmpEB.f1=Cyc_CfFlowInfo_assign_place(_tmp9E->loc,_tmpE8,env->all_changed,
_tmpE6,Cyc_CfFlowInfo_unknown_all);_tmpEB.f2=_tmpE9;_tmpEB;});_tmpEA;});goto
_LLCC;_LLCF:;_LLD0: goto _LLCC;_LLCC:;}return({struct _tuple0 _tmpEC;_tmpEC.f1=
_tmpE0;_tmpEC.f2=Cyc_CfFlowInfo_unknown_all;_tmpEC;});}}_LL93: if(*((int*)_tmp89)
!= 4)goto _LL95;_tmpA2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp89)->f1;_tmpA3=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp89)->f2;if(_tmpA3 != 0)goto _LL95;_tmpA4=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp89)->f3;_LL94: {struct Cyc_Set_Set**_tmpED=
env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmpEE=_cycalloc(
sizeof(*_tmpEE));_tmpEE[0]=Cyc_CfFlowInfo_mt_place_set();_tmpEE;});{void*_tmpF0;
void*_tmpF1;struct _tuple0 _tmpEF=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmpA2);
_tmpF0=_tmpEF.f1;_tmpF1=_tmpEF.f2;{struct Cyc_Set_Set*_tmpF2=*((struct Cyc_Set_Set**)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Set_Set**_tmpF3=
_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=Cyc_CfFlowInfo_mt_place_set();_tmpF3;});{
void*_tmpF5;void*_tmpF6;struct _tuple0 _tmpF4=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpA4);_tmpF5=_tmpF4.f1;_tmpF6=_tmpF4.f2;{struct Cyc_Set_Set*_tmpF7=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmpF8=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmpF2,_tmpF0,_tmpF5,_tmpF2,_tmpF7);void*_tmpF9=Cyc_CfFlowInfo_join_flow(
_tmpED,inflow,_tmpF8);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmpF9,inflow)){if(
_tmpED == 0)env->all_changed=0;else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((
struct Cyc_Set_Set**)_tmpED),((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmpF2,_tmpF7));}{void*_tmpFA=_tmpF8;struct
Cyc_Dict_Dict*_tmpFB;struct Cyc_List_List*_tmpFC;_LLD2: if((int)_tmpFA != 0)goto
_LLD4;_LLD3: return({struct _tuple0 _tmpFD;_tmpFD.f1=(void*)0;_tmpFD.f2=_tmpF6;
_tmpFD;});_LLD4: if(_tmpFA <= (void*)1?1:*((int*)_tmpFA)!= 0)goto _LLD1;_tmpFB=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFA)->f1;_tmpFC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFA)->f2;_LLD5: {void*_tmpFE=_tmpF1;struct Cyc_CfFlowInfo_Place*_tmpFF;_LLD7:
if(_tmpFE <= (void*)1?1:*((int*)_tmpFE)!= 0)goto _LLD9;_tmpFF=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmpFE)->f1;_LLD8: _tmpFB=Cyc_CfFlowInfo_assign_place(e->loc,_tmpFB,env->all_changed,
_tmpFF,_tmpF6);_tmpFC=Cyc_CfFlowInfo_reln_assign_exp(_tmpFC,_tmpA2,_tmpA4);
_tmpF8=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp100=_cycalloc(
sizeof(*_tmp100));_tmp100[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp101;
_tmp101.tag=0;_tmp101.f1=_tmpFB;_tmp101.f2=_tmpFC;_tmp101;});_tmp100;});Cyc_NewControlFlow_update_tryflow(
env,_tmpF8);return({struct _tuple0 _tmp102;_tmp102.f1=_tmpF8;_tmp102.f2=_tmpF6;
_tmp102;});_LLD9: if((int)_tmpFE != 0)goto _LLD6;_LLDA: return({struct _tuple0 _tmp103;
_tmp103.f1=Cyc_NewControlFlow_use_Rval(env,_tmpA4->loc,_tmpF8,_tmpF6);_tmp103.f2=
_tmpF6;_tmp103;});_LLD6:;}_LLD1:;}}inflow=_tmpF9;}}}}}}_LL95: if(*((int*)_tmp89)
!= 7)goto _LL97;_tmpA5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp89)->f1;_tmpA6=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp89)->f2;_LL96: return Cyc_NewControlFlow_anal_Rexp(
env,(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA5)).f1,_tmpA6);_LL97: if(*((int*)
_tmp89)!= 10)goto _LL99;_tmpA7=((struct Cyc_Absyn_Throw_e_struct*)_tmp89)->f1;
_LL98: {void*_tmp105;void*_tmp106;struct _tuple0 _tmp104=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA7);_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;Cyc_NewControlFlow_use_Rval(
env,_tmpA7->loc,_tmp105,_tmp106);return({struct _tuple0 _tmp107;_tmp107.f1=(void*)
0;_tmp107.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp107;});}_LL99: if(*((int*)
_tmp89)!= 9)goto _LL9B;_tmpA8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp89)->f1;
_tmpA9=((struct Cyc_Absyn_FnCall_e_struct*)_tmp89)->f2;_LL9A: {void*_tmp10A;
struct Cyc_List_List*_tmp10B;struct _tuple5 _tmp109=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->hd=
_tmpA8;_tmp108->tl=_tmpA9;_tmp108;}));_tmp10A=_tmp109.f1;_tmp10B=_tmp109.f2;{
void*_tmp10C=Cyc_NewControlFlow_use_Rval(env,_tmpA8->loc,_tmp10A,(void*)((struct
Cyc_List_List*)_check_null(_tmp10B))->hd);_tmp10B=_tmp10B->tl;{struct Cyc_List_List*
init_params=0;{void*_tmp10D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA8->topt))->v);struct Cyc_Absyn_PtrInfo _tmp10E;void*_tmp10F;_LLDC:
if(_tmp10D <= (void*)3?1:*((int*)_tmp10D)!= 4)goto _LLDE;_tmp10E=((struct Cyc_Absyn_PointerType_struct*)
_tmp10D)->f1;_tmp10F=(void*)_tmp10E.elt_typ;_LLDD:{void*_tmp110=Cyc_Tcutil_compress(
_tmp10F);struct Cyc_Absyn_FnInfo _tmp111;struct Cyc_List_List*_tmp112;_LLE1: if(
_tmp110 <= (void*)3?1:*((int*)_tmp110)!= 8)goto _LLE3;_tmp111=((struct Cyc_Absyn_FnType_struct*)
_tmp110)->f1;_tmp112=_tmp111.attributes;_LLE2: for(0;_tmp112 != 0;_tmp112=_tmp112->tl){
void*_tmp113=(void*)_tmp112->hd;int _tmp114;_LLE6: if(_tmp113 <= (void*)17?1:*((int*)
_tmp113)!= 4)goto _LLE8;_tmp114=((struct Cyc_Absyn_Initializes_att_struct*)_tmp113)->f1;
_LLE7: init_params=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));
_tmp115->hd=(void*)_tmp114;_tmp115->tl=init_params;_tmp115;});goto _LLE5;_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}goto _LLE0;_LLE3:;_LLE4:({void*_tmp116[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp117="anal_Rexp: bad function type";
_tag_arr(_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,29));}),_tag_arr(
_tmp116,sizeof(void*),0));});_LLE0:;}goto _LLDB;_LLDE:;_LLDF:({void*_tmp118[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp119="anal_Rexp: bad function type";_tag_arr(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,29));}),_tag_arr(_tmp118,sizeof(void*),0));});_LLDB:;}{
int i=1;for(0;_tmp10B != 0;(((_tmp10B=_tmp10B->tl,_tmpA9=((struct Cyc_List_List*)
_check_null(_tmpA9))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp10C=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpA9))->hd)->loc,_tmp10C,(void*)_tmp10B->hd);
continue;}{void*_tmp11A=_tmp10A;struct Cyc_Dict_Dict*_tmp11B;_LLEB: if((int)
_tmp11A != 0)goto _LLED;_LLEC: goto _LLEA;_LLED: if(_tmp11A <= (void*)1?1:*((int*)
_tmp11A)!= 0)goto _LLEA;_tmp11B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11A)->f1;_LLEE: if(Cyc_CfFlowInfo_initlevel(_tmp11B,(void*)_tmp10B->hd)== (
void*)0)({void*_tmp11C[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA9))->hd)->loc,({const char*_tmp11D="expression may not be initialized";
_tag_arr(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,34));}),_tag_arr(
_tmp11C,sizeof(void*),0));});{void*_tmp11E=_tmp10C;struct Cyc_Dict_Dict*_tmp11F;
struct Cyc_List_List*_tmp120;_LLF0: if((int)_tmp11E != 0)goto _LLF2;_LLF1: goto _LLEF;
_LLF2: if(_tmp11E <= (void*)1?1:*((int*)_tmp11E)!= 0)goto _LLEF;_tmp11F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11E)->f1;_tmp120=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11E)->f2;
_LLF3: {struct Cyc_Dict_Dict*_tmp121=Cyc_CfFlowInfo_escape_deref(_tmp11F,env->all_changed,(
void*)_tmp10B->hd);{void*_tmp122=(void*)_tmp10B->hd;struct Cyc_CfFlowInfo_Place*
_tmp123;_LLF5: if(_tmp122 <= (void*)3?1:*((int*)_tmp122)!= 2)goto _LLF7;_tmp123=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp122)->f1;_LLF6:{void*_tmp124=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpA9))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp125;void*
_tmp126;_LLFA: if(_tmp124 <= (void*)3?1:*((int*)_tmp124)!= 4)goto _LLFC;_tmp125=((
struct Cyc_Absyn_PointerType_struct*)_tmp124)->f1;_tmp126=(void*)_tmp125.elt_typ;
_LLFB: _tmp121=Cyc_CfFlowInfo_assign_place(((struct Cyc_Absyn_Exp*)_tmpA9->hd)->loc,
_tmp121,env->all_changed,_tmp123,Cyc_CfFlowInfo_typ_to_absrval(_tmp126,Cyc_CfFlowInfo_esc_all));
goto _LLF9;_LLFC:;_LLFD:({void*_tmp127[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp128="anal_Rexp: bad type for initialized arg";
_tag_arr(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,40));}),_tag_arr(
_tmp127,sizeof(void*),0));});_LLF9:;}goto _LLF4;_LLF7:;_LLF8: goto _LLF4;_LLF4:;}
_tmp10C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp129=_cycalloc(
sizeof(*_tmp129));_tmp129[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp12A;
_tmp12A.tag=0;_tmp12A.f1=_tmp121;_tmp12A.f2=_tmp120;_tmp12A;});_tmp129;});goto
_LLEF;}_LLEF:;}goto _LLEA;_LLEA:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA8->topt))->v))return({struct _tuple0 _tmp12B;_tmp12B.f1=(void*)0;
_tmp12B.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp12B;});else{return({
struct _tuple0 _tmp12C;_tmp12C.f1=_tmp10C;_tmp12C.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp12C;});}}}}_LL9B: if(*((int*)_tmp89)!= 33)goto _LL9D;_tmpAA=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp89)->f1;_tmpAB=_tmpAA.is_calloc;_tmpAC=_tmpAA.rgn;_tmpAD=_tmpAA.elt_type;
_tmpAE=_tmpAA.num_elts;_tmpAF=_tmpAA.fat_result;_LL9C: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp13E;_tmp13E.tag=1;_tmp13E.f1=_tmpAE;
_tmp13E.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp13E;});_tmp13D;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->root=(void*)root;_tmp13C->fields=0;
_tmp13C;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp13A=
_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp13B;_tmp13B.tag=2;_tmp13B.f1=place;_tmp13B;});_tmp13A;});void*place_val=
_tmpAF?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpAD)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);if(_tmpAC != 0){void*
_tmp12F;struct Cyc_List_List*_tmp130;struct _tuple5 _tmp12E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp12D[2];_tmp12D[1]=_tmpAE;_tmp12D[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpAC);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp12D,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp12F=
_tmp12E.f1;_tmp130=_tmp12E.f2;outflow=_tmp12F;}else{void*_tmp132;struct _tuple0
_tmp131=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpAE);_tmp132=_tmp131.f1;
outflow=_tmp132;}{void*_tmp133=outflow;struct Cyc_Dict_Dict*_tmp134;struct Cyc_List_List*
_tmp135;_LLFF: if((int)_tmp133 != 0)goto _LL101;_LL100: return({struct _tuple0 _tmp136;
_tmp136.f1=outflow;_tmp136.f2=rval;_tmp136;});_LL101: if(_tmp133 <= (void*)1?1:*((
int*)_tmp133)!= 0)goto _LLFE;_tmp134=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp133)->f1;_tmp135=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f2;
_LL102: return({struct _tuple0 _tmp137;_tmp137.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp139;_tmp139.tag=0;_tmp139.f1=Cyc_Dict_insert(_tmp134,root,place_val);_tmp139.f2=
_tmp135;_tmp139;});_tmp138;});_tmp137.f2=rval;_tmp137;});_LLFE:;}}_LL9D: if(*((
int*)_tmp89)!= 15)goto _LL9F;_tmpB0=((struct Cyc_Absyn_New_e_struct*)_tmp89)->f1;
_tmpB1=((struct Cyc_Absyn_New_e_struct*)_tmp89)->f2;_LL9E: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp151;_tmp151.tag=1;_tmp151.f1=_tmpB1;
_tmp151.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp151;});_tmp150;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->root=(void*)root;_tmp14F->fields=0;
_tmp14F;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp14D=
_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp14E;_tmp14E.tag=2;_tmp14E.f1=place;_tmp14E;});_tmp14D;});if(env->all_changed
!= 0)*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);{void*outflow;void*place_val;if(_tmpB0 != 0){
void*_tmp141;struct Cyc_List_List*_tmp142;struct _tuple5 _tmp140=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp13F[2];_tmp13F[1]=_tmpB1;_tmp13F[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpB0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp13F,sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp141=
_tmp140.f1;_tmp142=_tmp140.f2;outflow=_tmp141;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp142))->tl))->hd;}else{void*
_tmp144;void*_tmp145;struct _tuple0 _tmp143=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpB1);_tmp144=_tmp143.f1;_tmp145=_tmp143.f2;outflow=_tmp144;place_val=
_tmp145;}{void*_tmp146=outflow;struct Cyc_Dict_Dict*_tmp147;struct Cyc_List_List*
_tmp148;_LL104: if((int)_tmp146 != 0)goto _LL106;_LL105: return({struct _tuple0
_tmp149;_tmp149.f1=outflow;_tmp149.f2=rval;_tmp149;});_LL106: if(_tmp146 <= (void*)
1?1:*((int*)_tmp146)!= 0)goto _LL103;_tmp147=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp146)->f1;_tmp148=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp146)->f2;
_LL107: return({struct _tuple0 _tmp14A;_tmp14A.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14C;_tmp14C.tag=0;_tmp14C.f1=Cyc_Dict_insert(_tmp147,root,place_val);_tmp14C.f2=
_tmp148;_tmp14C;});_tmp14B;});_tmp14A.f2=rval;_tmp14A;});_LL103:;}}}_LL9F: if(*((
int*)_tmp89)!= 14)goto _LLA1;_tmpB2=((struct Cyc_Absyn_Address_e_struct*)_tmp89)->f1;
_LLA0: {void*_tmp153;void*_tmp154;struct _tuple0 _tmp152=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpB2);_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{void*_tmp155=_tmp154;
struct Cyc_CfFlowInfo_Place*_tmp156;_LL109: if((int)_tmp155 != 0)goto _LL10B;_LL10A:
return({struct _tuple0 _tmp157;_tmp157.f1=_tmp153;_tmp157.f2=(void*)1;_tmp157;});
_LL10B: if(_tmp155 <= (void*)1?1:*((int*)_tmp155)!= 0)goto _LL108;_tmp156=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp155)->f1;_LL10C: return({struct _tuple0 _tmp158;
_tmp158.f1=_tmp153;_tmp158.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp15A;_tmp15A.tag=2;_tmp15A.f1=_tmp156;_tmp15A;});_tmp159;});_tmp158;});_LL108:;}}
_LLA1: if(*((int*)_tmp89)!= 20)goto _LLA3;_tmpB3=((struct Cyc_Absyn_Deref_e_struct*)
_tmp89)->f1;_LLA2: {void*_tmp15C;void*_tmp15D;struct _tuple0 _tmp15B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB3);_tmp15C=_tmp15B.f1;_tmp15D=_tmp15B.f2;return Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp15C,_tmpB3,_tmp15D);}_LLA3: if(*((int*)_tmp89)!= 21)goto _LLA5;
_tmpB4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp89)->f1;_tmpB5=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp89)->f2;_LLA4: {void*_tmp15F;void*_tmp160;struct _tuple0 _tmp15E=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB4);_tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB4->topt))->v))return({struct _tuple0
_tmp161;_tmp161.f1=_tmp15F;_tmp161.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp161;});{
void*_tmp162=_tmp160;struct Cyc_Dict_Dict*_tmp163;_LL10E: if(_tmp162 <= (void*)3?1:*((
int*)_tmp162)!= 3)goto _LL110;_tmp163=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp162)->f1;_LL10F: return({struct _tuple0 _tmp164;_tmp164.f1=_tmp15F;_tmp164.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp163,
_tmpB5);_tmp164;});_LL110:;_LL111:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_Core_Impossible_struct
_tmp166;_tmp166.tag=Cyc_Core_Impossible;_tmp166.f1=({const char*_tmp167="anal_Rexp: AggrMember";
_tag_arr(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,22));});_tmp166;});
_tmp165;}));_LL10D:;}}_LLA5: if(*((int*)_tmp89)!= 22)goto _LLA7;_tmpB6=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp89)->f1;_tmpB7=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp89)->f2;_LLA6: {
void*_tmp169;void*_tmp16A;struct _tuple0 _tmp168=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpB6);_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;{void*_tmp16C;void*_tmp16D;
struct _tuple0 _tmp16B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp169,_tmpB6,
_tmp16A);_tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;{void*_tmp16E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB6->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp16F;void*_tmp170;_LL113: if(_tmp16E <= (void*)3?1:*((int*)_tmp16E)!= 4)goto
_LL115;_tmp16F=((struct Cyc_Absyn_PointerType_struct*)_tmp16E)->f1;_tmp170=(void*)
_tmp16F.elt_typ;_LL114: if(Cyc_Absyn_is_union_type(_tmp170))return({struct _tuple0
_tmp171;_tmp171.f1=_tmp16C;_tmp171.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp171;});
goto _LL112;_LL115:;_LL116:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172[0]=({struct Cyc_Core_Impossible_struct
_tmp173;_tmp173.tag=Cyc_Core_Impossible;_tmp173.f1=({const char*_tmp174="anal_Rexp: AggrArrow ptr";
_tag_arr(_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,25));});_tmp173;});
_tmp172;}));_LL112:;}{void*_tmp175=_tmp16D;struct Cyc_Dict_Dict*_tmp176;_LL118:
if(_tmp175 <= (void*)3?1:*((int*)_tmp175)!= 3)goto _LL11A;_tmp176=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp175)->f1;_LL119: return({struct _tuple0 _tmp177;_tmp177.f1=_tmp16C;_tmp177.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp176,
_tmpB7);_tmp177;});_LL11A:;_LL11B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Core_Impossible_struct
_tmp179;_tmp179.tag=Cyc_Core_Impossible;_tmp179.f1=({const char*_tmp17A="anal_Rexp: AggrArrow";
_tag_arr(_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,21));});_tmp179;});
_tmp178;}));_LL117:;}}}_LLA7: if(*((int*)_tmp89)!= 6)goto _LLA9;_tmpB8=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp89)->f1;_tmpB9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp89)->f2;_tmpBA=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp89)->f3;_LLA8: {void*_tmp17C;void*
_tmp17D;struct _tuple0 _tmp17B=Cyc_NewControlFlow_anal_test(env,inflow,_tmpB8);
_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;{void*_tmp17F;void*_tmp180;struct _tuple0
_tmp17E=Cyc_NewControlFlow_anal_Rexp(env,_tmp17C,_tmpB9);_tmp17F=_tmp17E.f1;
_tmp180=_tmp17E.f2;{void*_tmp182;void*_tmp183;struct _tuple0 _tmp181=Cyc_NewControlFlow_anal_Rexp(
env,_tmp17D,_tmpBA);_tmp182=_tmp181.f1;_tmp183=_tmp181.f2;return({struct _tuple0
_tmp184;_tmp184.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_use_Rval(
env,_tmpB9->loc,_tmp17F,_tmp180),Cyc_NewControlFlow_use_Rval(env,_tmpBA->loc,
_tmp182,_tmp183));_tmp184.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp184;});}}}_LLA9: if(*((
int*)_tmp89)!= 23)goto _LLAB;_tmpBB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp89)->f1;
_tmpBC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp89)->f2;_LLAA: {void*_tmp187;
struct Cyc_List_List*_tmp188;struct _tuple5 _tmp186=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp185[2];_tmp185[1]=_tmpBC;_tmp185[0]=_tmpBB;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp185,
sizeof(struct Cyc_Absyn_Exp*),2));}));_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;{void*
_tmp189=_tmp187;{void*_tmp18A=_tmp187;struct Cyc_Dict_Dict*_tmp18B;struct Cyc_List_List*
_tmp18C;_LL11D: if(_tmp18A <= (void*)1?1:*((int*)_tmp18A)!= 0)goto _LL11F;_tmp18B=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp18A)->f1;_tmp18C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp18A)->f2;_LL11E: if(Cyc_CfFlowInfo_initlevel(_tmp18B,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp188))->tl))->hd)== (void*)0)({
void*_tmp18D[0]={};Cyc_Tcutil_terr(_tmpBC->loc,({const char*_tmp18E="expression may not be initialized";
_tag_arr(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,34));}),_tag_arr(
_tmp18D,sizeof(void*),0));});{struct Cyc_List_List*_tmp18F=Cyc_NewControlFlow_add_subscript_reln(
_tmp18C,_tmpBB,_tmpBC);if(_tmp18C != _tmp18F)_tmp189=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp191;_tmp191.tag=0;_tmp191.f1=_tmp18B;_tmp191.f2=_tmp18F;_tmp191;});_tmp190;});
goto _LL11C;}_LL11F:;_LL120: goto _LL11C;_LL11C:;}{void*_tmp192=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpBB->topt))->v);_LL122: if(_tmp192 <= (
void*)3?1:*((int*)_tmp192)!= 9)goto _LL124;_LL123: {void*_tmp193=(void*)((struct
Cyc_List_List*)_check_null(_tmp188))->hd;struct Cyc_Dict_Dict*_tmp194;_LL127: if(
_tmp193 <= (void*)3?1:*((int*)_tmp193)!= 3)goto _LL129;_tmp194=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp193)->f1;_LL128: return({struct _tuple0 _tmp195;_tmp195.f1=_tmp189;_tmp195.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp194,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpBC)).f1));_tmp195;});_LL129:;_LL12A:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp196=_cycalloc(sizeof(*
_tmp196));_tmp196[0]=({struct Cyc_Core_Impossible_struct _tmp197;_tmp197.tag=Cyc_Core_Impossible;
_tmp197.f1=({const char*_tmp198="anal_Rexp: Subscript";_tag_arr(_tmp198,sizeof(
char),_get_zero_arr_size(_tmp198,21));});_tmp197;});_tmp196;}));_LL126:;}_LL124:;
_LL125: goto _LL121;_LL121:;}{void*_tmp19A;void*_tmp19B;struct _tuple0 _tmp199=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp187,_tmpBB,(void*)((struct Cyc_List_List*)_check_null(_tmp188))->hd);
_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;if(_tmp19A == (void*)0)return({struct
_tuple0 _tmp19C;_tmp19C.f1=_tmp19A;_tmp19C.f2=_tmp19B;_tmp19C;});else{return({
struct _tuple0 _tmp19D;_tmp19D.f1=_tmp189;_tmp19D.f2=_tmp19B;_tmp19D;});}}}}_LLAB:
if(*((int*)_tmp89)!= 30)goto _LLAD;_tmpBD=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp89)->f1;_LLAC: _tmpBE=_tmpBD;goto _LLAE;_LLAD: if(*((int*)_tmp89)!= 24)goto
_LLAF;_tmpBE=((struct Cyc_Absyn_Tuple_e_struct*)_tmp89)->f1;_LLAE: {void*_tmp19F;
struct Cyc_List_List*_tmp1A0;struct _tuple5 _tmp19E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpBE);_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;{struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp1A0 != 0;(_tmp1A0=
_tmp1A0->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(i),(
void*)_tmp1A0->hd);}}return({struct _tuple0 _tmp1A1;_tmp1A1.f1=_tmp19F;_tmp1A1.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));
_tmp1A2[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1A3;_tmp1A3.tag=3;_tmp1A3.f1=
aggrdict;_tmp1A3;});_tmp1A2;});_tmp1A1;});}}_LLAF: if(*((int*)_tmp89)!= 29)goto
_LLB1;_tmpBF=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp89)->f2;_LLB0: _tmpC0=
_tmpBF;goto _LLB2;_LLB1: if(*((int*)_tmp89)!= 28)goto _LLB3;_tmpC0=((struct Cyc_Absyn_Struct_e_struct*)
_tmp89)->f3;_LLB2: {void*_tmp1A5;struct Cyc_List_List*_tmp1A6;struct _tuple5
_tmp1A4=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple6*))Cyc_Core_snd,_tmpC0));_tmp1A5=_tmp1A4.f1;
_tmp1A6=_tmp1A4.f2;{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int
i=0;for(0;_tmp1A6 != 0;(((_tmp1A6=_tmp1A6->tl,_tmpC0=_tmpC0->tl)),++ i)){struct Cyc_List_List*
ds=(*((struct _tuple6*)((struct _tuple6*)((struct Cyc_List_List*)_check_null(_tmpC0))->hd))).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp1A7=(void*)ds->hd;struct _tagged_arr*_tmp1A8;
_LL12C: if(*((int*)_tmp1A7)!= 0)goto _LL12E;_LL12D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Core_Impossible_struct
_tmp1AA;_tmp1AA.tag=Cyc_Core_Impossible;_tmp1AA.f1=({const char*_tmp1AB="anal_Rexp:Struct_e";
_tag_arr(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,19));});_tmp1AA;});
_tmp1A9;}));_LL12E: if(*((int*)_tmp1A7)!= 1)goto _LL12B;_tmp1A8=((struct Cyc_Absyn_FieldName_struct*)
_tmp1A7)->f1;_LL12F: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp1A8,(void*)_tmp1A6->hd);
_LL12B:;}}}return({struct _tuple0 _tmp1AC;_tmp1AC.f1=_tmp1A5;_tmp1AC.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));
_tmp1AD[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp1AE;_tmp1AE.tag=3;_tmp1AE.f1=
aggrdict;_tmp1AE;});_tmp1AD;});_tmp1AC;});}}_LLB3: if(*((int*)_tmp89)!= 26)goto
_LLB5;_tmpC1=((struct Cyc_Absyn_Array_e_struct*)_tmp89)->f1;_LLB4: {struct Cyc_List_List*
_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple6*))
Cyc_Core_snd,_tmpC1);void*_tmp1B1;struct Cyc_List_List*_tmp1B2;struct _tuple5
_tmp1B0=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp1AF);_tmp1B1=
_tmp1B0.f1;_tmp1B2=_tmp1B0.f2;for(0;_tmp1B2 != 0;(_tmp1B2=_tmp1B2->tl,_tmp1AF=
_tmp1AF->tl)){_tmp1B1=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1AF))->hd)->loc,_tmp1B1,(void*)_tmp1B2->hd);}
return({struct _tuple0 _tmp1B3;_tmp1B3.f1=_tmp1B1;_tmp1B3.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1B3;});}_LLB5: if(*((int*)_tmp89)!= 27)goto _LLB7;_tmpC2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp89)->f1;_tmpC3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp89)->f2;_tmpC4=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp89)->f3;_tmpC5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp89)->f4;_LLB6: {void*_tmp1B5;void*_tmp1B6;struct _tuple0 _tmp1B4=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpC3);_tmp1B5=_tmp1B4.f1;_tmp1B6=_tmp1B4.f2;{void*_tmp1B7=_tmp1B5;
struct Cyc_Dict_Dict*_tmp1B8;struct Cyc_List_List*_tmp1B9;_LL131: if((int)_tmp1B7 != 
0)goto _LL133;_LL132: return({struct _tuple0 _tmp1BA;_tmp1BA.f1=_tmp1B5;_tmp1BA.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1BA;});_LL133: if(_tmp1B7 <= (void*)1?1:*((int*)
_tmp1B7)!= 0)goto _LL130;_tmp1B8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1B7)->f1;_tmp1B9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1B7)->f2;
_LL134: if(Cyc_CfFlowInfo_initlevel(_tmp1B8,_tmp1B6)== (void*)0)({void*_tmp1BB[0]={};
Cyc_Tcutil_terr(_tmpC3->loc,({const char*_tmp1BC="expression may not be initialized";
_tag_arr(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,34));}),_tag_arr(
_tmp1BB,sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp1B9;comp_loop: {
void*_tmp1BD=(void*)_tmpC3->r;struct Cyc_Absyn_Exp*_tmp1BE;void*_tmp1BF;struct Cyc_Absyn_Vardecl*
_tmp1C0;void*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C2;void*_tmp1C3;struct Cyc_Absyn_Vardecl*
_tmp1C4;void*_tmp1C5;struct Cyc_Absyn_Vardecl*_tmp1C6;void*_tmp1C7;int _tmp1C8;
void*_tmp1C9;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List _tmp1CB;struct Cyc_Absyn_Exp*
_tmp1CC;_LL136: if(*((int*)_tmp1BD)!= 13)goto _LL138;_tmp1BE=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1BD)->f2;_LL137: _tmpC3=_tmp1BE;goto comp_loop;_LL138: if(*((int*)_tmp1BD)!= 1)
goto _LL13A;_tmp1BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BD)->f2;if(
_tmp1BF <= (void*)1?1:*((int*)_tmp1BF)!= 0)goto _LL13A;_tmp1C0=((struct Cyc_Absyn_Global_b_struct*)
_tmp1BF)->f1;if(!(!_tmp1C0->escapes))goto _LL13A;_LL139: _tmp1C2=_tmp1C0;goto
_LL13B;_LL13A: if(*((int*)_tmp1BD)!= 1)goto _LL13C;_tmp1C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1BD)->f2;if(_tmp1C1 <= (void*)1?1:*((int*)_tmp1C1)!= 3)goto _LL13C;_tmp1C2=((
struct Cyc_Absyn_Local_b_struct*)_tmp1C1)->f1;if(!(!_tmp1C2->escapes))goto _LL13C;
_LL13B: _tmp1C4=_tmp1C2;goto _LL13D;_LL13C: if(*((int*)_tmp1BD)!= 1)goto _LL13E;
_tmp1C3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BD)->f2;if(_tmp1C3 <= (void*)
1?1:*((int*)_tmp1C3)!= 4)goto _LL13E;_tmp1C4=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1C3)->f1;if(!(!_tmp1C4->escapes))goto _LL13E;_LL13D: _tmp1C6=_tmp1C4;goto
_LL13F;_LL13E: if(*((int*)_tmp1BD)!= 1)goto _LL140;_tmp1C5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1BD)->f2;if(_tmp1C5 <= (void*)1?1:*((int*)_tmp1C5)!= 2)goto _LL140;_tmp1C6=((
struct Cyc_Absyn_Param_b_struct*)_tmp1C5)->f1;if(!(!_tmp1C6->escapes))goto _LL140;
_LL13F: new_relns=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));
_tmp1CD->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));
_tmp1CE->vd=_tmpC2;_tmp1CE->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=_tmp1C6;_tmp1D0;});_tmp1CF;}));_tmp1CE;});
_tmp1CD->tl=_tmp1B9;_tmp1CD;});goto _LL135;_LL140: if(*((int*)_tmp1BD)!= 0)goto
_LL142;_tmp1C7=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1BD)->f1;if(_tmp1C7
<= (void*)1?1:*((int*)_tmp1C7)!= 2)goto _LL142;_tmp1C8=((struct Cyc_Absyn_Int_c_struct*)
_tmp1C7)->f2;_LL141: new_relns=({struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*
_tmp1D1));_tmp1D1->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1D2=_cycalloc(sizeof(*
_tmp1D2));_tmp1D2->vd=_tmpC2;_tmp1D2->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1D3=_cycalloc_atomic(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1D4;_tmp1D4.tag=3;_tmp1D4.f1=(unsigned int)_tmp1C8;_tmp1D4;});_tmp1D3;}));
_tmp1D2;});_tmp1D1->tl=_tmp1B9;_tmp1D1;});goto _LL135;_LL142: if(*((int*)_tmp1BD)
!= 3)goto _LL144;_tmp1C9=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1BD)->f1;
if((int)_tmp1C9 != 19)goto _LL144;_tmp1CA=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1BD)->f2;if(_tmp1CA == 0)goto _LL144;_tmp1CB=*_tmp1CA;_tmp1CC=(struct Cyc_Absyn_Exp*)
_tmp1CB.hd;_LL143:{void*_tmp1D5=(void*)_tmp1CC->r;void*_tmp1D6;struct Cyc_Absyn_Vardecl*
_tmp1D7;void*_tmp1D8;struct Cyc_Absyn_Vardecl*_tmp1D9;void*_tmp1DA;struct Cyc_Absyn_Vardecl*
_tmp1DB;void*_tmp1DC;struct Cyc_Absyn_Vardecl*_tmp1DD;_LL147: if(*((int*)_tmp1D5)
!= 1)goto _LL149;_tmp1D6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D5)->f2;if(
_tmp1D6 <= (void*)1?1:*((int*)_tmp1D6)!= 0)goto _LL149;_tmp1D7=((struct Cyc_Absyn_Global_b_struct*)
_tmp1D6)->f1;if(!(!_tmp1D7->escapes))goto _LL149;_LL148: _tmp1D9=_tmp1D7;goto
_LL14A;_LL149: if(*((int*)_tmp1D5)!= 1)goto _LL14B;_tmp1D8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D5)->f2;if(_tmp1D8 <= (void*)1?1:*((int*)_tmp1D8)!= 3)goto _LL14B;_tmp1D9=((
struct Cyc_Absyn_Local_b_struct*)_tmp1D8)->f1;if(!(!_tmp1D9->escapes))goto _LL14B;
_LL14A: _tmp1DB=_tmp1D9;goto _LL14C;_LL14B: if(*((int*)_tmp1D5)!= 1)goto _LL14D;
_tmp1DA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D5)->f2;if(_tmp1DA <= (void*)
1?1:*((int*)_tmp1DA)!= 4)goto _LL14D;_tmp1DB=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1DA)->f1;if(!(!_tmp1DB->escapes))goto _LL14D;_LL14C: _tmp1DD=_tmp1DB;goto
_LL14E;_LL14D: if(*((int*)_tmp1D5)!= 1)goto _LL14F;_tmp1DC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D5)->f2;if(_tmp1DC <= (void*)1?1:*((int*)_tmp1DC)!= 2)goto _LL14F;_tmp1DD=((
struct Cyc_Absyn_Param_b_struct*)_tmp1DC)->f1;if(!(!_tmp1DD->escapes))goto _LL14F;
_LL14E: new_relns=({struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));
_tmp1DE->hd=({struct Cyc_CfFlowInfo_Reln*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));
_tmp1DF->vd=_tmpC2;_tmp1DF->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp1E1;_tmp1E1.tag=2;_tmp1E1.f1=_tmp1DD;_tmp1E1;});_tmp1E0;}));_tmp1DF;});
_tmp1DE->tl=_tmp1B9;_tmp1DE;});goto _LL146;_LL14F:;_LL150: goto _LL146;_LL146:;}
goto _LL135;_LL144:;_LL145: goto _LL135;_LL135:;}if(_tmp1B9 != new_relns)_tmp1B5=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1E2=_cycalloc(sizeof(*
_tmp1E2));_tmp1E2[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1E3;_tmp1E3.tag=
0;_tmp1E3.f1=_tmp1B8;_tmp1E3.f2=new_relns;_tmp1E3;});_tmp1E2;});{void*_tmp1E4=
_tmp1B6;_LL152: if((int)_tmp1E4 != 0)goto _LL154;_LL153: return({struct _tuple0
_tmp1E5;_tmp1E5.f1=_tmp1B5;_tmp1E5.f2=Cyc_CfFlowInfo_unknown_all;_tmp1E5;});
_LL154: if((int)_tmp1E4 != 2)goto _LL156;_LL155: goto _LL157;_LL156: if((int)_tmp1E4 != 
1)goto _LL158;_LL157: goto _LL159;_LL158: if(_tmp1E4 <= (void*)3?1:*((int*)_tmp1E4)!= 
2)goto _LL15A;_LL159: {struct Cyc_List_List _tmp1E6=({struct Cyc_List_List _tmp1F0;
_tmp1F0.hd=_tmpC2;_tmp1F0.tl=0;_tmp1F0;});_tmp1B5=Cyc_NewControlFlow_add_vars(
_tmp1B5,(struct Cyc_List_List*)& _tmp1E6,Cyc_CfFlowInfo_unknown_all);{void*_tmp1E8;
void*_tmp1E9;struct _tuple0 _tmp1E7=Cyc_NewControlFlow_anal_Rexp(env,_tmp1B5,
_tmpC4);_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;{void*_tmp1EA=_tmp1E8;struct Cyc_Dict_Dict*
_tmp1EB;_LL15D: if((int)_tmp1EA != 0)goto _LL15F;_LL15E: return({struct _tuple0
_tmp1EC;_tmp1EC.f1=_tmp1E8;_tmp1EC.f2=Cyc_CfFlowInfo_unknown_all;_tmp1EC;});
_LL15F: if(_tmp1EA <= (void*)1?1:*((int*)_tmp1EA)!= 0)goto _LL15C;_tmp1EB=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1EA)->f1;_LL160: if(Cyc_CfFlowInfo_initlevel(
_tmp1EB,_tmp1E9)!= (void*)2){({void*_tmp1ED[0]={};Cyc_Tcutil_terr(_tmpC3->loc,({
const char*_tmp1EE="expression may not be initialized";_tag_arr(_tmp1EE,sizeof(
char),_get_zero_arr_size(_tmp1EE,34));}),_tag_arr(_tmp1ED,sizeof(void*),0));});
return({struct _tuple0 _tmp1EF;_tmp1EF.f1=(void*)0;_tmp1EF.f2=Cyc_CfFlowInfo_unknown_all;
_tmp1EF;});}_LL15C:;}_tmp1B5=_tmp1E8;goto _LL15B;}}_LL15A:;_LL15B: while(1){struct
Cyc_List_List _tmp1F1=({struct Cyc_List_List _tmp1FB;_tmp1FB.hd=_tmpC2;_tmp1FB.tl=0;
_tmp1FB;});_tmp1B5=Cyc_NewControlFlow_add_vars(_tmp1B5,(struct Cyc_List_List*)&
_tmp1F1,Cyc_CfFlowInfo_unknown_all);{void*_tmp1F3;void*_tmp1F4;struct _tuple0
_tmp1F2=Cyc_NewControlFlow_anal_Rexp(env,_tmp1B5,_tmpC4);_tmp1F3=_tmp1F2.f1;
_tmp1F4=_tmp1F2.f2;{void*_tmp1F5=_tmp1F3;struct Cyc_Dict_Dict*_tmp1F6;_LL162: if((
int)_tmp1F5 != 0)goto _LL164;_LL163: goto _LL161;_LL164: if(_tmp1F5 <= (void*)1?1:*((
int*)_tmp1F5)!= 0)goto _LL161;_tmp1F6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F5)->f1;_LL165: if(Cyc_CfFlowInfo_initlevel(_tmp1F6,_tmp1F4)!= (void*)2){({
void*_tmp1F7[0]={};Cyc_Tcutil_terr(_tmpC3->loc,({const char*_tmp1F8="expression may not be initialized";
_tag_arr(_tmp1F8,sizeof(char),_get_zero_arr_size(_tmp1F8,34));}),_tag_arr(
_tmp1F7,sizeof(void*),0));});return({struct _tuple0 _tmp1F9;_tmp1F9.f1=(void*)0;
_tmp1F9.f2=Cyc_CfFlowInfo_unknown_all;_tmp1F9;});}_LL161:;}{void*_tmp1FA=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp1B5,_tmp1F3);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1FA,
_tmp1B5))break;_tmp1B5=_tmp1FA;}}}return({struct _tuple0 _tmp1FC;_tmp1FC.f1=
_tmp1B5;_tmp1FC.f2=Cyc_CfFlowInfo_unknown_all;_tmp1FC;});_LL151:;}}_LL130:;}}
_LLB7: if(*((int*)_tmp89)!= 35)goto _LLB9;_tmpC6=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp89)->f1;_LLB8: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpC6))->flow;{void*_tmp1FD=(void*)_tmpC6->r;struct Cyc_Absyn_Stmt*
_tmp1FE;struct Cyc_Absyn_Stmt*_tmp1FF;struct Cyc_Absyn_Decl*_tmp200;struct Cyc_Absyn_Stmt*
_tmp201;struct Cyc_Absyn_Exp*_tmp202;_LL167: if(_tmp1FD <= (void*)1?1:*((int*)
_tmp1FD)!= 1)goto _LL169;_tmp1FE=((struct Cyc_Absyn_Seq_s_struct*)_tmp1FD)->f1;
_tmp1FF=((struct Cyc_Absyn_Seq_s_struct*)_tmp1FD)->f2;_LL168: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp1FE);_tmpC6=_tmp1FF;goto _LL166;_LL169: if(_tmp1FD <= (void*)1?1:*((
int*)_tmp1FD)!= 12)goto _LL16B;_tmp200=((struct Cyc_Absyn_Decl_s_struct*)_tmp1FD)->f1;
_tmp201=((struct Cyc_Absyn_Decl_s_struct*)_tmp1FD)->f2;_LL16A: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp200);_tmpC6=_tmp201;goto _LL166;_LL16B: if(_tmp1FD <= (void*)1?1:*((
int*)_tmp1FD)!= 0)goto _LL16D;_tmp202=((struct Cyc_Absyn_Exp_s_struct*)_tmp1FD)->f1;
_LL16C: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp202);_LL16D:;_LL16E:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp203=_cycalloc(sizeof(*
_tmp203));_tmp203[0]=({struct Cyc_Core_Impossible_struct _tmp204;_tmp204.tag=Cyc_Core_Impossible;
_tmp204.f1=({const char*_tmp205="analyze_Rexp: ill-formed StmtExp";_tag_arr(
_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,33));});_tmp204;});_tmp203;}));
_LL166:;}}_LLB9: if(*((int*)_tmp89)!= 1)goto _LLBB;_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp89)->f2;if((int)_tmpC7 != 0)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(*((int*)_tmp89)
!= 2)goto _LLBD;_LLBC: goto _LLBE;_LLBD: if(*((int*)_tmp89)!= 8)goto _LLBF;_LLBE: goto
_LLC0;_LLBF: if(*((int*)_tmp89)!= 34)goto _LLC1;_LLC0: goto _LLC2;_LLC1: if(*((int*)
_tmp89)!= 25)goto _LLC3;_LLC2: goto _LLC4;_LLC3: if(*((int*)_tmp89)!= 36)goto _LLC5;
_LLC4: goto _LLC6;_LLC5: if(*((int*)_tmp89)!= 37)goto _LL66;_LLC6:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206[0]=({
struct Cyc_Core_Impossible_struct _tmp207;_tmp207.tag=Cyc_Core_Impossible;_tmp207.f1=({
const char*_tmp208="anal_Rexp, unexpected exp form";_tag_arr(_tmp208,sizeof(char),
_get_zero_arr_size(_tmp208,31));});_tmp207;});_tmp206;}));_LL66:;}}static struct
_tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*
_tmp209=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp20A;void*_tmp20B;struct Cyc_Absyn_Conref*_tmp20C;
struct Cyc_Absyn_Conref*_tmp20D;_LL170: if(_tmp209 <= (void*)3?1:*((int*)_tmp209)!= 
4)goto _LL172;_tmp20A=((struct Cyc_Absyn_PointerType_struct*)_tmp209)->f1;_tmp20B=(
void*)_tmp20A.elt_typ;_tmp20C=_tmp20A.bounds;_tmp20D=_tmp20A.zero_term;_LL171: {
void*_tmp20E=f;struct Cyc_Dict_Dict*_tmp20F;struct Cyc_List_List*_tmp210;_LL175:
if((int)_tmp20E != 0)goto _LL177;_LL176: return({struct _tuple0 _tmp211;_tmp211.f1=f;
_tmp211.f2=(void*)0;_tmp211;});_LL177: if(_tmp20E <= (void*)1?1:*((int*)_tmp20E)!= 
0)goto _LL174;_tmp20F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20E)->f1;
_tmp210=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20E)->f2;_LL178: if(Cyc_Tcutil_is_bound_one(
_tmp20C)){void*_tmp212=r;struct Cyc_CfFlowInfo_Place*_tmp213;struct Cyc_CfFlowInfo_Place
_tmp214;void*_tmp215;struct Cyc_List_List*_tmp216;void*_tmp217;_LL17A: if((int)
_tmp212 != 1)goto _LL17C;_LL17B: goto _LL17D;_LL17C: if((int)_tmp212 != 2)goto _LL17E;
_LL17D:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp219;_tmp219.tag=Cyc_CfFlowInfo_NotZero;_tmp219.f1=_tmp210;_tmp219;});_tmp218;})));
goto _LL179;_LL17E: if(_tmp212 <= (void*)3?1:*((int*)_tmp212)!= 2)goto _LL180;
_tmp213=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp212)->f1;_tmp214=*_tmp213;
_tmp215=(void*)_tmp214.root;_tmp216=_tmp214.fields;_LL17F:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp21A=_cycalloc(sizeof(*_tmp21A));
_tmp21A[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp21B;_tmp21B.tag=Cyc_CfFlowInfo_NotZero;
_tmp21B.f1=_tmp210;_tmp21B;});_tmp21A;})));return({struct _tuple0 _tmp21C;_tmp21C.f1=
f;_tmp21C.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp21D=_cycalloc(
sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp21E;_tmp21E.tag=
0;_tmp21E.f1=({struct Cyc_CfFlowInfo_Place*_tmp21F=_cycalloc(sizeof(*_tmp21F));
_tmp21F->root=(void*)_tmp215;_tmp21F->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp216,flds);_tmp21F;});_tmp21E;});
_tmp21D;});_tmp21C;});_LL180: if((int)_tmp212 != 0)goto _LL182;_LL181:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmp220=_cycalloc(sizeof(*
_tmp220));_tmp220[0]=({struct Cyc_CfFlowInfo_IsZero_struct _tmp221;_tmp221.tag=Cyc_CfFlowInfo_IsZero;
_tmp221.f1=_tmp210;_tmp221;});_tmp220;})));return({struct _tuple0 _tmp222;_tmp222.f1=(
void*)0;_tmp222.f2=(void*)0;_tmp222;});_LL182: if(_tmp212 <= (void*)3?1:*((int*)
_tmp212)!= 0)goto _LL184;_tmp217=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp212)->f1;_LL183: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp217);goto
_LL185;_LL184:;_LL185:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp224;_tmp224.tag=Cyc_CfFlowInfo_UnknownZ;_tmp224.f1=_tmp210;_tmp224;});
_tmp223;})));_LL179:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp226;_tmp226.tag=Cyc_CfFlowInfo_UnknownZ;_tmp226.f1=_tmp210;_tmp226;});
_tmp225;})));}if(Cyc_CfFlowInfo_initlevel(_tmp20F,r)== (void*)0)({void*_tmp227[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp228="dereference of possibly uninitialized pointer";
_tag_arr(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,46));}),_tag_arr(
_tmp227,sizeof(void*),0));});return({struct _tuple0 _tmp229;_tmp229.f1=f;_tmp229.f2=(
void*)0;_tmp229;});_LL174:;}_LL172:;_LL173:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=({struct Cyc_Core_Impossible_struct
_tmp22B;_tmp22B.tag=Cyc_Core_Impossible;_tmp22B.f1=({const char*_tmp22C="left deref of non-pointer-type";
_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,31));});_tmp22B;});
_tmp22A;}));_LL16F:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp22D=inflow;struct Cyc_Dict_Dict*_tmp22E;
struct Cyc_List_List*_tmp22F;_LL187: if((int)_tmp22D != 0)goto _LL189;_LL188: return({
struct _tuple0 _tmp230;_tmp230.f1=(void*)0;_tmp230.f2=(void*)0;_tmp230;});_LL189:
if(_tmp22D <= (void*)1?1:*((int*)_tmp22D)!= 0)goto _LL186;_tmp22E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp22D)->f1;_tmp22F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22D)->f2;
_LL18A: d=_tmp22E;_LL186:;}{void*_tmp231=(void*)e->r;struct Cyc_Absyn_Exp*_tmp232;
struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_Exp*_tmp234;void*_tmp235;struct Cyc_Absyn_Vardecl*
_tmp236;void*_tmp237;struct Cyc_Absyn_Vardecl*_tmp238;void*_tmp239;struct Cyc_Absyn_Vardecl*
_tmp23A;void*_tmp23B;struct Cyc_Absyn_Vardecl*_tmp23C;struct Cyc_Absyn_Exp*_tmp23D;
struct _tagged_arr*_tmp23E;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*
_tmp240;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*_tmp242;struct
_tagged_arr*_tmp243;_LL18C: if(*((int*)_tmp231)!= 13)goto _LL18E;_tmp232=((struct
Cyc_Absyn_Cast_e_struct*)_tmp231)->f2;_LL18D: _tmp233=_tmp232;goto _LL18F;_LL18E:
if(*((int*)_tmp231)!= 11)goto _LL190;_tmp233=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp231)->f1;_LL18F: _tmp234=_tmp233;goto _LL191;_LL190: if(*((int*)_tmp231)!= 12)
goto _LL192;_tmp234=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp231)->f1;_LL191:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp234,flds);_LL192: if(*((int*)
_tmp231)!= 1)goto _LL194;_tmp235=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp231)->f2;
if(_tmp235 <= (void*)1?1:*((int*)_tmp235)!= 2)goto _LL194;_tmp236=((struct Cyc_Absyn_Param_b_struct*)
_tmp235)->f1;_LL193: _tmp238=_tmp236;goto _LL195;_LL194: if(*((int*)_tmp231)!= 1)
goto _LL196;_tmp237=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp231)->f2;if(
_tmp237 <= (void*)1?1:*((int*)_tmp237)!= 3)goto _LL196;_tmp238=((struct Cyc_Absyn_Local_b_struct*)
_tmp237)->f1;_LL195: _tmp23A=_tmp238;goto _LL197;_LL196: if(*((int*)_tmp231)!= 1)
goto _LL198;_tmp239=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp231)->f2;if(
_tmp239 <= (void*)1?1:*((int*)_tmp239)!= 4)goto _LL198;_tmp23A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp239)->f1;_LL197: return({struct _tuple0 _tmp244;_tmp244.f1=inflow;_tmp244.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp245=_cycalloc(sizeof(*_tmp245));
_tmp245[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp246;_tmp246.tag=0;_tmp246.f1=({
struct Cyc_CfFlowInfo_Place*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp248=_cycalloc(sizeof(*
_tmp248));_tmp248[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp249;_tmp249.tag=0;
_tmp249.f1=_tmp23A;_tmp249;});_tmp248;}));_tmp247->fields=flds;_tmp247;});
_tmp246;});_tmp245;});_tmp244;});_LL198: if(*((int*)_tmp231)!= 1)goto _LL19A;
_tmp23B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp231)->f2;if(_tmp23B <= (void*)
1?1:*((int*)_tmp23B)!= 0)goto _LL19A;_tmp23C=((struct Cyc_Absyn_Global_b_struct*)
_tmp23B)->f1;_LL199: return({struct _tuple0 _tmp24A;_tmp24A.f1=inflow;_tmp24A.f2=(
void*)0;_tmp24A;});_LL19A: if(*((int*)_tmp231)!= 22)goto _LL19C;_tmp23D=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp231)->f1;_tmp23E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp231)->f2;_LL19B: if(!Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp23D->topt))->v))flds=({struct Cyc_List_List*_tmp24B=_cycalloc(
sizeof(*_tmp24B));_tmp24B->hd=_tmp23E;_tmp24B->tl=flds;_tmp24B;});_tmp23F=
_tmp23D;goto _LL19D;_LL19C: if(*((int*)_tmp231)!= 20)goto _LL19E;_tmp23F=((struct
Cyc_Absyn_Deref_e_struct*)_tmp231)->f1;_LL19D: {void*_tmp24D;void*_tmp24E;struct
_tuple0 _tmp24C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp23F);_tmp24D=_tmp24C.f1;
_tmp24E=_tmp24C.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp24D,
_tmp23F,_tmp24E,flds);}_LL19E: if(*((int*)_tmp231)!= 23)goto _LL1A0;_tmp240=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp231)->f1;_tmp241=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp231)->f2;_LL19F:{void*_tmp24F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp240->topt))->v);_LL1A5: if(_tmp24F <= (void*)3?1:*((int*)_tmp24F)
!= 9)goto _LL1A7;_LL1A6: {struct _tagged_arr*_tmp250=Cyc_Absyn_fieldname((int)(Cyc_Evexp_eval_const_uint_exp(
_tmp241)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp240,({struct
Cyc_List_List*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251->hd=_tmp250;_tmp251->tl=
flds;_tmp251;}));}_LL1A7:;_LL1A8: goto _LL1A4;_LL1A4:;}{void*_tmp254;struct Cyc_List_List*
_tmp255;struct _tuple5 _tmp253=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp252[2];_tmp252[1]=_tmp241;_tmp252[0]=_tmp240;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp252,sizeof(struct Cyc_Absyn_Exp*),
2));}));_tmp254=_tmp253.f1;_tmp255=_tmp253.f2;{void*_tmp256=_tmp254;{void*
_tmp257=_tmp254;struct Cyc_Dict_Dict*_tmp258;struct Cyc_List_List*_tmp259;_LL1AA:
if(_tmp257 <= (void*)1?1:*((int*)_tmp257)!= 0)goto _LL1AC;_tmp258=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp257)->f1;_tmp259=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp257)->f2;
_LL1AB: if(Cyc_CfFlowInfo_initlevel(_tmp258,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp255))->tl))->hd)== (void*)0)({
void*_tmp25A[0]={};Cyc_Tcutil_terr(_tmp241->loc,({const char*_tmp25B="expression may not be initialized";
_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,34));}),_tag_arr(
_tmp25A,sizeof(void*),0));});{struct Cyc_List_List*_tmp25C=Cyc_NewControlFlow_add_subscript_reln(
_tmp259,_tmp240,_tmp241);if(_tmp259 != _tmp25C)_tmp256=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp25E;_tmp25E.tag=0;_tmp25E.f1=_tmp258;_tmp25E.f2=_tmp25C;_tmp25E;});_tmp25D;});
goto _LL1A9;}_LL1AC:;_LL1AD: goto _LL1A9;_LL1A9:;}{void*_tmp260;void*_tmp261;struct
_tuple0 _tmp25F=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp254,_tmp240,(void*)((
struct Cyc_List_List*)_check_null(_tmp255))->hd,flds);_tmp260=_tmp25F.f1;_tmp261=
_tmp25F.f2;if(_tmp260 == (void*)0)return({struct _tuple0 _tmp262;_tmp262.f1=_tmp260;
_tmp262.f2=_tmp261;_tmp262;});else{return({struct _tuple0 _tmp263;_tmp263.f1=
_tmp256;_tmp263.f2=_tmp261;_tmp263;});}}}}_LL1A0: if(*((int*)_tmp231)!= 21)goto
_LL1A2;_tmp242=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp231)->f1;_tmp243=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp231)->f2;_LL1A1: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp242->topt))->v))return({struct
_tuple0 _tmp264;_tmp264.f1=inflow;_tmp264.f2=(void*)0;_tmp264;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp242,({struct Cyc_List_List*_tmp265=_cycalloc(sizeof(*_tmp265));
_tmp265->hd=_tmp243;_tmp265->tl=flds;_tmp265;}));_LL1A2:;_LL1A3: return({struct
_tuple0 _tmp266;_tmp266.f1=(void*)0;_tmp266.f2=(void*)0;_tmp266;});_LL18B:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp267=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp268;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*
_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;void*_tmp26D;struct Cyc_List_List*_tmp26E;
struct Cyc_List_List _tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_List_List*
_tmp271;void*_tmp272;struct Cyc_List_List*_tmp273;void*_tmp274;struct Cyc_List_List*
_tmp275;void*_tmp276;struct Cyc_List_List*_tmp277;void*_tmp278;struct Cyc_List_List*
_tmp279;_LL1AF: if(*((int*)_tmp267)!= 6)goto _LL1B1;_tmp268=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp267)->f1;_tmp269=((struct Cyc_Absyn_Conditional_e_struct*)_tmp267)->f2;
_tmp26A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp267)->f3;_LL1B0: {void*
_tmp27B;void*_tmp27C;struct _tuple0 _tmp27A=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp268);_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;{void*_tmp27E;void*_tmp27F;
struct _tuple0 _tmp27D=Cyc_NewControlFlow_anal_test(env,_tmp27B,_tmp269);_tmp27E=
_tmp27D.f1;_tmp27F=_tmp27D.f2;{void*_tmp281;void*_tmp282;struct _tuple0 _tmp280=
Cyc_NewControlFlow_anal_test(env,_tmp27C,_tmp26A);_tmp281=_tmp280.f1;_tmp282=
_tmp280.f2;return({struct _tuple0 _tmp283;_tmp283.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp27E,_tmp281);_tmp283.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp27F,
_tmp282);_tmp283;});}}}_LL1B1: if(*((int*)_tmp267)!= 7)goto _LL1B3;_tmp26B=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp267)->f1;_tmp26C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp267)->f2;_LL1B2: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp26B)).f1,_tmp26C);_LL1B3: if(*((int*)_tmp267)!= 3)goto _LL1B5;
_tmp26D=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f1;if((int)_tmp26D != 
11)goto _LL1B5;_tmp26E=((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f2;if(_tmp26E
== 0)goto _LL1B5;_tmp26F=*_tmp26E;_tmp270=(struct Cyc_Absyn_Exp*)_tmp26F.hd;
_tmp271=_tmp26F.tl;if(_tmp271 != 0)goto _LL1B5;_LL1B4: {void*_tmp285;void*_tmp286;
struct _tuple0 _tmp284=Cyc_NewControlFlow_anal_test(env,inflow,_tmp270);_tmp285=
_tmp284.f1;_tmp286=_tmp284.f2;return({struct _tuple0 _tmp287;_tmp287.f1=_tmp286;
_tmp287.f2=_tmp285;_tmp287;});}_LL1B5: if(*((int*)_tmp267)!= 3)goto _LL1B7;_tmp272=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f1;if((int)_tmp272 != 5)goto
_LL1B7;_tmp273=((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f2;_LL1B6: {void*
_tmp289;struct Cyc_List_List*_tmp28A;struct _tuple5 _tmp288=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp273);_tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;{void*_tmp28B=_tmp289;
struct Cyc_Dict_Dict*_tmp28C;struct Cyc_List_List*_tmp28D;_LL1C0: if((int)_tmp28B != 
0)goto _LL1C2;_LL1C1: return({struct _tuple0 _tmp28E;_tmp28E.f1=_tmp289;_tmp28E.f2=
_tmp289;_tmp28E;});_LL1C2: if(_tmp28B <= (void*)1?1:*((int*)_tmp28B)!= 0)goto
_LL1BF;_tmp28C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp28B)->f1;_tmp28D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp28B)->f2;_LL1C3: {void*_tmp28F=(
void*)((struct Cyc_List_List*)_check_null(_tmp28A))->hd;void*_tmp290=(void*)((
struct Cyc_List_List*)_check_null(_tmp28A->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp28C,_tmp28F)== (void*)0)({void*_tmp291[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp273))->hd)->loc,({const char*_tmp292="expression may not be initialized";
_tag_arr(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,34));}),_tag_arr(
_tmp291,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp28C,_tmp290)== (
void*)0)({void*_tmp293[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp273))->tl))->hd)->loc,({const
char*_tmp294="expression may not be initialized";_tag_arr(_tmp294,sizeof(char),
_get_zero_arr_size(_tmp294,34));}),_tag_arr(_tmp293,sizeof(void*),0));});{struct
_tuple0 _tmp296=({struct _tuple0 _tmp295;_tmp295.f1=_tmp28F;_tmp295.f2=_tmp290;
_tmp295;});void*_tmp297;void*_tmp298;void*_tmp299;void*_tmp29A;void*_tmp29B;void*
_tmp29C;_LL1C5: _tmp297=_tmp296.f1;if(_tmp297 <= (void*)3?1:*((int*)_tmp297)!= 0)
goto _LL1C7;_tmp298=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp297)->f1;
_tmp299=_tmp296.f2;if((int)_tmp299 != 0)goto _LL1C7;_LL1C6: {void*_tmp29E;void*
_tmp29F;struct _tuple0 _tmp29D=Cyc_NewControlFlow_splitzero(env,_tmp289,_tmp289,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp273))->hd,_tmp298);
_tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;return({struct _tuple0 _tmp2A0;_tmp2A0.f1=
_tmp29F;_tmp2A0.f2=_tmp29E;_tmp2A0;});}_LL1C7: _tmp29A=_tmp296.f1;if((int)_tmp29A
!= 0)goto _LL1C9;_tmp29B=_tmp296.f2;if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 0)
goto _LL1C9;_tmp29C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp29B)->f1;
_LL1C8: {void*_tmp2A2;void*_tmp2A3;struct _tuple0 _tmp2A1=Cyc_NewControlFlow_splitzero(
env,_tmp289,_tmp289,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp273))->tl))->hd,_tmp29C);_tmp2A2=_tmp2A1.f1;
_tmp2A3=_tmp2A1.f2;return({struct _tuple0 _tmp2A4;_tmp2A4.f1=_tmp2A3;_tmp2A4.f2=
_tmp2A2;_tmp2A4;});}_LL1C9:;_LL1CA: return({struct _tuple0 _tmp2A5;_tmp2A5.f1=
_tmp289;_tmp2A5.f2=_tmp289;_tmp2A5;});_LL1C4:;}}_LL1BF:;}}_LL1B7: if(*((int*)
_tmp267)!= 3)goto _LL1B9;_tmp274=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp267)->f1;if((int)_tmp274 != 6)goto _LL1B9;_tmp275=((struct Cyc_Absyn_Primop_e_struct*)
_tmp267)->f2;_LL1B8: {void*_tmp2A7;struct Cyc_List_List*_tmp2A8;struct _tuple5
_tmp2A6=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp275);_tmp2A7=
_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;{void*_tmp2A9=_tmp2A7;struct Cyc_Dict_Dict*_tmp2AA;
struct Cyc_List_List*_tmp2AB;_LL1CC: if((int)_tmp2A9 != 0)goto _LL1CE;_LL1CD: return({
struct _tuple0 _tmp2AC;_tmp2AC.f1=_tmp2A7;_tmp2AC.f2=_tmp2A7;_tmp2AC;});_LL1CE: if(
_tmp2A9 <= (void*)1?1:*((int*)_tmp2A9)!= 0)goto _LL1CB;_tmp2AA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A9)->f1;_tmp2AB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A9)->f2;
_LL1CF: {void*_tmp2AD=(void*)((struct Cyc_List_List*)_check_null(_tmp2A8))->hd;
void*_tmp2AE=(void*)((struct Cyc_List_List*)_check_null(_tmp2A8->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp2AA,_tmp2AD)== (void*)0)({void*_tmp2AF[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp275))->hd)->loc,({const char*_tmp2B0="expression may not be initialized";
_tag_arr(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,34));}),_tag_arr(
_tmp2AF,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp2AA,_tmp2AE)== (
void*)0)({void*_tmp2B1[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp275))->tl))->hd)->loc,({const
char*_tmp2B2="expression may not be initialized";_tag_arr(_tmp2B2,sizeof(char),
_get_zero_arr_size(_tmp2B2,34));}),_tag_arr(_tmp2B1,sizeof(void*),0));});{struct
_tuple0 _tmp2B4=({struct _tuple0 _tmp2B3;_tmp2B3.f1=_tmp2AD;_tmp2B3.f2=_tmp2AE;
_tmp2B3;});void*_tmp2B5;void*_tmp2B6;void*_tmp2B7;void*_tmp2B8;void*_tmp2B9;void*
_tmp2BA;_LL1D1: _tmp2B5=_tmp2B4.f1;if(_tmp2B5 <= (void*)3?1:*((int*)_tmp2B5)!= 0)
goto _LL1D3;_tmp2B6=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2B5)->f1;
_tmp2B7=_tmp2B4.f2;if((int)_tmp2B7 != 0)goto _LL1D3;_LL1D2: return Cyc_NewControlFlow_splitzero(
env,_tmp2A7,_tmp2A7,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp275))->hd,_tmp2B6);_LL1D3: _tmp2B8=_tmp2B4.f1;if((int)_tmp2B8 != 0)goto _LL1D5;
_tmp2B9=_tmp2B4.f2;if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 0)goto _LL1D5;
_tmp2BA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2B9)->f1;_LL1D4:
return Cyc_NewControlFlow_splitzero(env,_tmp2A7,_tmp2A7,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp275))->tl))->hd,
_tmp2BA);_LL1D5:;_LL1D6: return({struct _tuple0 _tmp2BB;_tmp2BB.f1=_tmp2A7;_tmp2BB.f2=
_tmp2A7;_tmp2BB;});_LL1D0:;}}_LL1CB:;}}_LL1B9: if(*((int*)_tmp267)!= 3)goto _LL1BB;
_tmp276=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f1;if((int)_tmp276 != 
8)goto _LL1BB;_tmp277=((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f2;_LL1BA: {
void*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct _tuple5 _tmp2BC=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp277);_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;{void*_tmp2BF=_tmp2BD;
struct Cyc_Dict_Dict*_tmp2C0;struct Cyc_List_List*_tmp2C1;_LL1D8: if((int)_tmp2BF != 
0)goto _LL1DA;_LL1D9: return({struct _tuple0 _tmp2C2;_tmp2C2.f1=_tmp2BD;_tmp2C2.f2=
_tmp2BD;_tmp2C2;});_LL1DA: if(_tmp2BF <= (void*)1?1:*((int*)_tmp2BF)!= 0)goto
_LL1D7;_tmp2C0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2BF)->f1;_tmp2C1=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2BF)->f2;_LL1DB: {void*_tmp2C3=(
void*)((struct Cyc_List_List*)_check_null(_tmp2BE))->hd;void*_tmp2C4=(void*)((
struct Cyc_List_List*)_check_null(_tmp2BE->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp2C0,_tmp2C3)== (void*)0)({void*_tmp2C5[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp277))->hd)->loc,({const char*_tmp2C6="expression may not be initialized";
_tag_arr(_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,34));}),_tag_arr(
_tmp2C5,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp2C0,_tmp2C4)== (
void*)0)({void*_tmp2C7[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp277))->tl))->hd)->loc,({const
char*_tmp2C8="expression may not be initialized";_tag_arr(_tmp2C8,sizeof(char),
_get_zero_arr_size(_tmp2C8,34));}),_tag_arr(_tmp2C7,sizeof(void*),0));});{struct
Cyc_Absyn_Exp*_tmp2C9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp277))->hd;struct Cyc_Absyn_Exp*_tmp2CA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp277->tl))->hd;void*_tmp2CB=(void*)_tmp2C9->r;void*_tmp2CC;struct
Cyc_Absyn_Vardecl*_tmp2CD;void*_tmp2CE;struct Cyc_Absyn_Vardecl*_tmp2CF;void*
_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D1;void*_tmp2D2;struct Cyc_Absyn_Vardecl*
_tmp2D3;_LL1DD: if(*((int*)_tmp2CB)!= 1)goto _LL1DF;_tmp2CC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2CB)->f2;if(_tmp2CC <= (void*)1?1:*((int*)_tmp2CC)!= 0)goto _LL1DF;_tmp2CD=((
struct Cyc_Absyn_Global_b_struct*)_tmp2CC)->f1;if(!(!_tmp2CD->escapes))goto _LL1DF;
_LL1DE: _tmp2CF=_tmp2CD;goto _LL1E0;_LL1DF: if(*((int*)_tmp2CB)!= 1)goto _LL1E1;
_tmp2CE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2CB)->f2;if(_tmp2CE <= (void*)
1?1:*((int*)_tmp2CE)!= 3)goto _LL1E1;_tmp2CF=((struct Cyc_Absyn_Local_b_struct*)
_tmp2CE)->f1;if(!(!_tmp2CF->escapes))goto _LL1E1;_LL1E0: _tmp2D1=_tmp2CF;goto
_LL1E2;_LL1E1: if(*((int*)_tmp2CB)!= 1)goto _LL1E3;_tmp2D0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2CB)->f2;if(_tmp2D0 <= (void*)1?1:*((int*)_tmp2D0)!= 4)goto _LL1E3;_tmp2D1=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2D0)->f1;if(!(!_tmp2D1->escapes))goto _LL1E3;
_LL1E2: _tmp2D3=_tmp2D1;goto _LL1E4;_LL1E3: if(*((int*)_tmp2CB)!= 1)goto _LL1E5;
_tmp2D2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2CB)->f2;if(_tmp2D2 <= (void*)
1?1:*((int*)_tmp2D2)!= 2)goto _LL1E5;_tmp2D3=((struct Cyc_Absyn_Param_b_struct*)
_tmp2D2)->f1;if(!(!_tmp2D3->escapes))goto _LL1E5;_LL1E4: {void*_tmp2D4=(void*)
_tmp2CA->r;void*_tmp2D5;struct Cyc_Absyn_Vardecl*_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Vardecl*
_tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Vardecl*_tmp2DA;void*_tmp2DB;struct Cyc_Absyn_Vardecl*
_tmp2DC;void*_tmp2DD;int _tmp2DE;void*_tmp2DF;struct Cyc_List_List*_tmp2E0;struct
Cyc_List_List _tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;_LL1E8: if(*((int*)_tmp2D4)!= 1)
goto _LL1EA;_tmp2D5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D4)->f2;if(
_tmp2D5 <= (void*)1?1:*((int*)_tmp2D5)!= 0)goto _LL1EA;_tmp2D6=((struct Cyc_Absyn_Global_b_struct*)
_tmp2D5)->f1;if(!(!_tmp2D6->escapes))goto _LL1EA;_LL1E9: _tmp2D8=_tmp2D6;goto
_LL1EB;_LL1EA: if(*((int*)_tmp2D4)!= 1)goto _LL1EC;_tmp2D7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D4)->f2;if(_tmp2D7 <= (void*)1?1:*((int*)_tmp2D7)!= 3)goto _LL1EC;_tmp2D8=((
struct Cyc_Absyn_Local_b_struct*)_tmp2D7)->f1;if(!(!_tmp2D8->escapes))goto _LL1EC;
_LL1EB: _tmp2DA=_tmp2D8;goto _LL1ED;_LL1EC: if(*((int*)_tmp2D4)!= 1)goto _LL1EE;
_tmp2D9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D4)->f2;if(_tmp2D9 <= (void*)
1?1:*((int*)_tmp2D9)!= 4)goto _LL1EE;_tmp2DA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2D9)->f1;if(!(!_tmp2DA->escapes))goto _LL1EE;_LL1ED: _tmp2DC=_tmp2DA;goto
_LL1EF;_LL1EE: if(*((int*)_tmp2D4)!= 1)goto _LL1F0;_tmp2DB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D4)->f2;if(_tmp2DB <= (void*)1?1:*((int*)_tmp2DB)!= 2)goto _LL1F0;_tmp2DC=((
struct Cyc_Absyn_Param_b_struct*)_tmp2DB)->f1;if(!(!_tmp2DC->escapes))goto _LL1F0;
_LL1EF:{struct _tuple0 _tmp2E4=({struct _tuple0 _tmp2E3;_tmp2E3.f1=Cyc_Tcutil_compress((
void*)_tmp2D3->type);_tmp2E3.f2=Cyc_Tcutil_compress((void*)_tmp2DC->type);
_tmp2E3;});void*_tmp2E5;void*_tmp2E6;void*_tmp2E7;void*_tmp2E8;void*_tmp2E9;void*
_tmp2EA;_LL1F7: _tmp2E5=_tmp2E4.f1;if(_tmp2E5 <= (void*)3?1:*((int*)_tmp2E5)!= 5)
goto _LL1F9;_tmp2E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E5)->f1;if((int)
_tmp2E6 != 1)goto _LL1F9;_tmp2E7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E5)->f2;
if((int)_tmp2E7 != 2)goto _LL1F9;_LL1F8: goto _LL1F6;_LL1F9: _tmp2E8=_tmp2E4.f2;if(
_tmp2E8 <= (void*)3?1:*((int*)_tmp2E8)!= 5)goto _LL1FB;_tmp2E9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2E8)->f1;if((int)_tmp2E9 != 1)goto _LL1FB;_tmp2EA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2E8)->f2;if((int)_tmp2EA != 2)goto _LL1FB;_LL1FA: goto _LL1F6;_LL1FB:;_LL1FC:
return({struct _tuple0 _tmp2EB;_tmp2EB.f1=_tmp2BD;_tmp2EB.f2=_tmp2BD;_tmp2EB;});
_LL1F6:;}{struct Cyc_List_List*_tmp2EC=({struct Cyc_List_List*_tmp2F0=_cycalloc(
sizeof(*_tmp2F0));_tmp2F0->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F1=_cycalloc(
sizeof(*_tmp2F1));_tmp2F1->vd=_tmp2D3;_tmp2F1->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp2F3;_tmp2F3.tag=1;_tmp2F3.f1=_tmp2DC;_tmp2F3;});_tmp2F2;}));_tmp2F1;});
_tmp2F0->tl=_tmp2C1;_tmp2F0;});return({struct _tuple0 _tmp2ED;_tmp2ED.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));
_tmp2EE[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2EF;_tmp2EF.tag=0;
_tmp2EF.f1=_tmp2C0;_tmp2EF.f2=_tmp2EC;_tmp2EF;});_tmp2EE;});_tmp2ED.f2=_tmp2BD;
_tmp2ED;});}_LL1F0: if(*((int*)_tmp2D4)!= 0)goto _LL1F2;_tmp2DD=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2D4)->f1;if(_tmp2DD <= (void*)1?1:*((int*)_tmp2DD)!= 2)goto _LL1F2;_tmp2DE=((
struct Cyc_Absyn_Int_c_struct*)_tmp2DD)->f2;_LL1F1:{struct _tuple0 _tmp2F5=({struct
_tuple0 _tmp2F4;_tmp2F4.f1=Cyc_Tcutil_compress((void*)_tmp2D3->type);_tmp2F4.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2CA->topt))->v);
_tmp2F4;});void*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*_tmp2F9;void*_tmp2FA;void*
_tmp2FB;_LL1FE: _tmp2F6=_tmp2F5.f1;if(_tmp2F6 <= (void*)3?1:*((int*)_tmp2F6)!= 5)
goto _LL200;_tmp2F7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F6)->f1;if((int)
_tmp2F7 != 1)goto _LL200;_tmp2F8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F6)->f2;
if((int)_tmp2F8 != 2)goto _LL200;_LL1FF: goto _LL1FD;_LL200: _tmp2F9=_tmp2F5.f2;if(
_tmp2F9 <= (void*)3?1:*((int*)_tmp2F9)!= 5)goto _LL202;_tmp2FA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2F9)->f1;if((int)_tmp2FA != 1)goto _LL202;_tmp2FB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2F9)->f2;if((int)_tmp2FB != 2)goto _LL202;_LL201: goto _LL1FD;_LL202:;_LL203:
return({struct _tuple0 _tmp2FC;_tmp2FC.f1=_tmp2BD;_tmp2FC.f2=_tmp2BD;_tmp2FC;});
_LL1FD:;}{struct Cyc_List_List*_tmp2FD=({struct Cyc_List_List*_tmp301=_cycalloc(
sizeof(*_tmp301));_tmp301->hd=({struct Cyc_CfFlowInfo_Reln*_tmp302=_cycalloc(
sizeof(*_tmp302));_tmp302->vd=_tmp2D3;_tmp302->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp303=_cycalloc_atomic(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp304;_tmp304.tag=3;_tmp304.f1=(unsigned int)_tmp2DE;_tmp304;});_tmp303;}));
_tmp302;});_tmp301->tl=_tmp2C1;_tmp301;});return({struct _tuple0 _tmp2FE;_tmp2FE.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2FF=_cycalloc(sizeof(*
_tmp2FF));_tmp2FF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp300;_tmp300.tag=
0;_tmp300.f1=_tmp2C0;_tmp300.f2=_tmp2FD;_tmp300;});_tmp2FF;});_tmp2FE.f2=_tmp2BD;
_tmp2FE;});}_LL1F2: if(*((int*)_tmp2D4)!= 3)goto _LL1F4;_tmp2DF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2D4)->f1;if((int)_tmp2DF != 19)goto _LL1F4;_tmp2E0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2D4)->f2;if(_tmp2E0 == 0)goto _LL1F4;_tmp2E1=*_tmp2E0;_tmp2E2=(struct Cyc_Absyn_Exp*)
_tmp2E1.hd;_LL1F3: {void*_tmp305=(void*)_tmp2E2->r;void*_tmp306;struct Cyc_Absyn_Vardecl*
_tmp307;void*_tmp308;struct Cyc_Absyn_Vardecl*_tmp309;void*_tmp30A;struct Cyc_Absyn_Vardecl*
_tmp30B;void*_tmp30C;struct Cyc_Absyn_Vardecl*_tmp30D;_LL205: if(*((int*)_tmp305)
!= 1)goto _LL207;_tmp306=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp305)->f2;if(
_tmp306 <= (void*)1?1:*((int*)_tmp306)!= 0)goto _LL207;_tmp307=((struct Cyc_Absyn_Global_b_struct*)
_tmp306)->f1;if(!(!_tmp307->escapes))goto _LL207;_LL206: _tmp309=_tmp307;goto
_LL208;_LL207: if(*((int*)_tmp305)!= 1)goto _LL209;_tmp308=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp305)->f2;if(_tmp308 <= (void*)1?1:*((int*)_tmp308)!= 3)goto _LL209;_tmp309=((
struct Cyc_Absyn_Local_b_struct*)_tmp308)->f1;if(!(!_tmp309->escapes))goto _LL209;
_LL208: _tmp30B=_tmp309;goto _LL20A;_LL209: if(*((int*)_tmp305)!= 1)goto _LL20B;
_tmp30A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp305)->f2;if(_tmp30A <= (void*)
1?1:*((int*)_tmp30A)!= 4)goto _LL20B;_tmp30B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp30A)->f1;if(!(!_tmp30B->escapes))goto _LL20B;_LL20A: _tmp30D=_tmp30B;goto
_LL20C;_LL20B: if(*((int*)_tmp305)!= 1)goto _LL20D;_tmp30C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp305)->f2;if(_tmp30C <= (void*)1?1:*((int*)_tmp30C)!= 2)goto _LL20D;_tmp30D=((
struct Cyc_Absyn_Param_b_struct*)_tmp30C)->f1;if(!(!_tmp30D->escapes))goto _LL20D;
_LL20C: {struct Cyc_List_List*_tmp30E=({struct Cyc_List_List*_tmp312=_cycalloc(
sizeof(*_tmp312));_tmp312->hd=({struct Cyc_CfFlowInfo_Reln*_tmp313=_cycalloc(
sizeof(*_tmp313));_tmp313->vd=_tmp2D3;_tmp313->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp315;_tmp315.tag=2;_tmp315.f1=_tmp30D;_tmp315;});_tmp314;}));_tmp313;});
_tmp312->tl=_tmp2C1;_tmp312;});return({struct _tuple0 _tmp30F;_tmp30F.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp310=_cycalloc(sizeof(*_tmp310));
_tmp310[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp311;_tmp311.tag=0;
_tmp311.f1=_tmp2C0;_tmp311.f2=_tmp30E;_tmp311;});_tmp310;});_tmp30F.f2=_tmp2BD;
_tmp30F;});}_LL20D:;_LL20E: return({struct _tuple0 _tmp316;_tmp316.f1=_tmp2BD;
_tmp316.f2=_tmp2BD;_tmp316;});_LL204:;}_LL1F4:;_LL1F5: return({struct _tuple0
_tmp317;_tmp317.f1=_tmp2BD;_tmp317.f2=_tmp2BD;_tmp317;});_LL1E7:;}_LL1E5:;_LL1E6:
return({struct _tuple0 _tmp318;_tmp318.f1=_tmp2BD;_tmp318.f2=_tmp2BD;_tmp318;});
_LL1DC:;}}_LL1D7:;}}_LL1BB: if(*((int*)_tmp267)!= 3)goto _LL1BD;_tmp278=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f1;if((int)_tmp278 != 10)goto _LL1BD;
_tmp279=((struct Cyc_Absyn_Primop_e_struct*)_tmp267)->f2;_LL1BC: {void*_tmp31A;
struct Cyc_List_List*_tmp31B;struct _tuple5 _tmp319=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp279);_tmp31A=_tmp319.f1;_tmp31B=_tmp319.f2;{void*_tmp31C=_tmp31A;
struct Cyc_Dict_Dict*_tmp31D;struct Cyc_List_List*_tmp31E;_LL210: if((int)_tmp31C != 
0)goto _LL212;_LL211: return({struct _tuple0 _tmp31F;_tmp31F.f1=_tmp31A;_tmp31F.f2=
_tmp31A;_tmp31F;});_LL212: if(_tmp31C <= (void*)1?1:*((int*)_tmp31C)!= 0)goto
_LL20F;_tmp31D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp31C)->f1;_tmp31E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp31C)->f2;_LL213: {void*_tmp320=(
void*)((struct Cyc_List_List*)_check_null(_tmp31B))->hd;void*_tmp321=(void*)((
struct Cyc_List_List*)_check_null(_tmp31B->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp31D,_tmp320)== (void*)0)({void*_tmp322[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp279))->hd)->loc,({const char*_tmp323="expression may not be initialized";
_tag_arr(_tmp323,sizeof(char),_get_zero_arr_size(_tmp323,34));}),_tag_arr(
_tmp322,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp31D,_tmp321)== (
void*)0)({void*_tmp324[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp279))->tl))->hd)->loc,({const
char*_tmp325="expression may not be initialized";_tag_arr(_tmp325,sizeof(char),
_get_zero_arr_size(_tmp325,34));}),_tag_arr(_tmp324,sizeof(void*),0));});{struct
Cyc_Absyn_Exp*_tmp326=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp279))->hd;struct Cyc_Absyn_Exp*_tmp327=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp279->tl))->hd;void*_tmp328=(void*)_tmp326->r;void*_tmp329;struct
Cyc_Absyn_Vardecl*_tmp32A;void*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32C;void*
_tmp32D;struct Cyc_Absyn_Vardecl*_tmp32E;void*_tmp32F;struct Cyc_Absyn_Vardecl*
_tmp330;_LL215: if(*((int*)_tmp328)!= 1)goto _LL217;_tmp329=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp328)->f2;if(_tmp329 <= (void*)1?1:*((int*)_tmp329)!= 0)goto _LL217;_tmp32A=((
struct Cyc_Absyn_Global_b_struct*)_tmp329)->f1;if(!(!_tmp32A->escapes))goto _LL217;
_LL216: _tmp32C=_tmp32A;goto _LL218;_LL217: if(*((int*)_tmp328)!= 1)goto _LL219;
_tmp32B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp328)->f2;if(_tmp32B <= (void*)
1?1:*((int*)_tmp32B)!= 3)goto _LL219;_tmp32C=((struct Cyc_Absyn_Local_b_struct*)
_tmp32B)->f1;if(!(!_tmp32C->escapes))goto _LL219;_LL218: _tmp32E=_tmp32C;goto
_LL21A;_LL219: if(*((int*)_tmp328)!= 1)goto _LL21B;_tmp32D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp328)->f2;if(_tmp32D <= (void*)1?1:*((int*)_tmp32D)!= 4)goto _LL21B;_tmp32E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp32D)->f1;if(!(!_tmp32E->escapes))goto _LL21B;
_LL21A: _tmp330=_tmp32E;goto _LL21C;_LL21B: if(*((int*)_tmp328)!= 1)goto _LL21D;
_tmp32F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp328)->f2;if(_tmp32F <= (void*)
1?1:*((int*)_tmp32F)!= 2)goto _LL21D;_tmp330=((struct Cyc_Absyn_Param_b_struct*)
_tmp32F)->f1;if(!(!_tmp330->escapes))goto _LL21D;_LL21C: {void*_tmp331=(void*)
_tmp327->r;void*_tmp332;struct Cyc_List_List*_tmp333;struct Cyc_List_List _tmp334;
struct Cyc_Absyn_Exp*_tmp335;_LL220: if(*((int*)_tmp331)!= 3)goto _LL222;_tmp332=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp331)->f1;if((int)_tmp332 != 19)goto
_LL222;_tmp333=((struct Cyc_Absyn_Primop_e_struct*)_tmp331)->f2;if(_tmp333 == 0)
goto _LL222;_tmp334=*_tmp333;_tmp335=(struct Cyc_Absyn_Exp*)_tmp334.hd;_LL221: {
void*_tmp336=(void*)_tmp335->r;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp338;void*
_tmp339;struct Cyc_Absyn_Vardecl*_tmp33A;void*_tmp33B;struct Cyc_Absyn_Vardecl*
_tmp33C;void*_tmp33D;struct Cyc_Absyn_Vardecl*_tmp33E;_LL225: if(*((int*)_tmp336)
!= 1)goto _LL227;_tmp337=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp336)->f2;if(
_tmp337 <= (void*)1?1:*((int*)_tmp337)!= 0)goto _LL227;_tmp338=((struct Cyc_Absyn_Global_b_struct*)
_tmp337)->f1;if(!(!_tmp338->escapes))goto _LL227;_LL226: _tmp33A=_tmp338;goto
_LL228;_LL227: if(*((int*)_tmp336)!= 1)goto _LL229;_tmp339=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp336)->f2;if(_tmp339 <= (void*)1?1:*((int*)_tmp339)!= 3)goto _LL229;_tmp33A=((
struct Cyc_Absyn_Local_b_struct*)_tmp339)->f1;if(!(!_tmp33A->escapes))goto _LL229;
_LL228: _tmp33C=_tmp33A;goto _LL22A;_LL229: if(*((int*)_tmp336)!= 1)goto _LL22B;
_tmp33B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp336)->f2;if(_tmp33B <= (void*)
1?1:*((int*)_tmp33B)!= 4)goto _LL22B;_tmp33C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp33B)->f1;if(!(!_tmp33C->escapes))goto _LL22B;_LL22A: _tmp33E=_tmp33C;goto
_LL22C;_LL22B: if(*((int*)_tmp336)!= 1)goto _LL22D;_tmp33D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp336)->f2;if(_tmp33D <= (void*)1?1:*((int*)_tmp33D)!= 2)goto _LL22D;_tmp33E=((
struct Cyc_Absyn_Param_b_struct*)_tmp33D)->f1;if(!(!_tmp33E->escapes))goto _LL22D;
_LL22C: {struct Cyc_List_List*_tmp33F=({struct Cyc_List_List*_tmp343=_cycalloc(
sizeof(*_tmp343));_tmp343->hd=({struct Cyc_CfFlowInfo_Reln*_tmp344=_cycalloc(
sizeof(*_tmp344));_tmp344->vd=_tmp330;_tmp344->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp346;_tmp346.tag=4;_tmp346.f1=_tmp33E;_tmp346;});_tmp345;}));_tmp344;});
_tmp343->tl=_tmp31E;_tmp343;});return({struct _tuple0 _tmp340;_tmp340.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp341=_cycalloc(sizeof(*_tmp341));
_tmp341[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp342;_tmp342.tag=0;
_tmp342.f1=_tmp31D;_tmp342.f2=_tmp33F;_tmp342;});_tmp341;});_tmp340.f2=_tmp31A;
_tmp340;});}_LL22D:;_LL22E: return({struct _tuple0 _tmp347;_tmp347.f1=_tmp31A;
_tmp347.f2=_tmp31A;_tmp347;});_LL224:;}_LL222:;_LL223: return({struct _tuple0
_tmp348;_tmp348.f1=_tmp31A;_tmp348.f2=_tmp31A;_tmp348;});_LL21F:;}_LL21D:;_LL21E:
return({struct _tuple0 _tmp349;_tmp349.f1=_tmp31A;_tmp349.f2=_tmp31A;_tmp349;});
_LL214:;}}_LL20F:;}}_LL1BD:;_LL1BE: goto _LL1AE;_LL1AE:;}{void*_tmp34B;void*
_tmp34C;struct _tuple0 _tmp34A=Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_tmp34B=
_tmp34A.f1;_tmp34C=_tmp34A.f2;{void*_tmp34D=_tmp34B;struct Cyc_Dict_Dict*_tmp34E;
struct Cyc_List_List*_tmp34F;_LL230: if((int)_tmp34D != 0)goto _LL232;_LL231: return({
struct _tuple0 _tmp350;_tmp350.f1=_tmp34B;_tmp350.f2=_tmp34B;_tmp350;});_LL232: if(
_tmp34D <= (void*)1?1:*((int*)_tmp34D)!= 0)goto _LL22F;_tmp34E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp34D)->f1;_tmp34F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp34D)->f2;
_LL233: {void*_tmp351=_tmp34C;void*_tmp352;void*_tmp353;void*_tmp354;_LL235: if((
int)_tmp351 != 0)goto _LL237;_LL236: return({struct _tuple0 _tmp355;_tmp355.f1=(void*)
0;_tmp355.f2=_tmp34B;_tmp355;});_LL237: if((int)_tmp351 != 2)goto _LL239;_LL238:
goto _LL23A;_LL239: if((int)_tmp351 != 1)goto _LL23B;_LL23A: goto _LL23C;_LL23B: if(
_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 2)goto _LL23D;_LL23C: return({struct
_tuple0 _tmp356;_tmp356.f1=_tmp34B;_tmp356.f2=(void*)0;_tmp356;});_LL23D: if(
_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 0)goto _LL23F;_tmp352=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp351)->f1;if((int)_tmp352 != 0)goto _LL23F;_LL23E: goto _LL240;_LL23F: if(_tmp351
<= (void*)3?1:*((int*)_tmp351)!= 1)goto _LL241;_tmp353=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp351)->f1;if((int)_tmp353 != 0)goto _LL241;_LL240:({void*_tmp357[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp358="expression may not be initialized";_tag_arr(_tmp358,
sizeof(char),_get_zero_arr_size(_tmp358,34));}),_tag_arr(_tmp357,sizeof(void*),0));});
return({struct _tuple0 _tmp359;_tmp359.f1=(void*)0;_tmp359.f2=(void*)0;_tmp359;});
_LL241: if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 0)goto _LL243;_tmp354=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp351)->f1;_LL242: return Cyc_NewControlFlow_splitzero(
env,inflow,_tmp34B,e,_tmp354);_LL243: if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 
1)goto _LL245;_LL244: return({struct _tuple0 _tmp35A;_tmp35A.f1=_tmp34B;_tmp35A.f2=
_tmp34B;_tmp35A;});_LL245: if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 3)goto
_LL234;_LL246:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp35B=
_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_Core_Impossible_struct _tmp35C;
_tmp35C.tag=Cyc_Core_Impossible;_tmp35C.f1=({const char*_tmp35D="anal_test";
_tag_arr(_tmp35D,sizeof(char),_get_zero_arr_size(_tmp35D,10));});_tmp35C;});
_tmp35B;}));_LL234:;}_LL22F:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,void*flow){
void*_tmp35E=flow;struct Cyc_Dict_Dict*_tmp35F;_LL248: if((int)_tmp35E != 0)goto
_LL24A;_LL249: return;_LL24A: if(_tmp35E <= (void*)1?1:*((int*)_tmp35E)!= 0)goto
_LL247;_tmp35F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp35E)->f1;_LL24B:{
struct Cyc_List_List*_tmp360=env->param_roots;for(0;_tmp360 != 0;_tmp360=_tmp360->tl){
if(Cyc_CfFlowInfo_initlevel(_tmp35F,Cyc_CfFlowInfo_lookup_place(_tmp35F,(struct
Cyc_CfFlowInfo_Place*)_tmp360->hd))!= (void*)2)({void*_tmp361[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp362="function may not initialize all the parameters with attribute 'initializes'";
_tag_arr(_tmp362,sizeof(char),_get_zero_arr_size(_tmp362,76));}),_tag_arr(
_tmp361,sizeof(void*),0));});}}return;_LL247:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp364;struct Cyc_Core_Opt*
_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Stmt*_tmp367;struct Cyc_Absyn_Switch_clause*
_tmp363=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp364=*_tmp363;_tmp365=_tmp364.pat_vars;
_tmp366=_tmp364.where_clause;_tmp367=_tmp364.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp365))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp366 != 0){void*_tmp369;void*_tmp36A;struct _tuple0
_tmp368=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp366));_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;inflow=_tmp36A;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp369,_tmp367);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp367);}{void*
_tmp36B=clause_outflow;_LL24D: if((int)_tmp36B != 0)goto _LL24F;_LL24E: goto _LL24C;
_LL24F:;_LL250: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp36C[0]={};Cyc_Tcutil_terr(
_tmp367->loc,({const char*_tmp36D="switch clause may implicitly fallthru";
_tag_arr(_tmp36D,sizeof(char),_get_zero_arr_size(_tmp36D,38));}),_tag_arr(
_tmp36C,sizeof(void*),0));});else{({void*_tmp36E[0]={};Cyc_Tcutil_warn(_tmp367->loc,({
const char*_tmp36F="switch clause may implicitly fallthru";_tag_arr(_tmp36F,
sizeof(char),_get_zero_arr_size(_tmp36F,38));}),_tag_arr(_tmp36E,sizeof(void*),0));});}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL24C;_LL24C:;}}}return(
void*)0;}static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp370=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);inflow=(void*)_tmp370->flow;{
void*_tmp371=(void*)s->r;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_Exp*_tmp373;
struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Stmt*_tmp375;struct Cyc_Absyn_Stmt*
_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Stmt*_tmp378;struct Cyc_Absyn_Stmt*
_tmp379;struct _tuple3 _tmp37A;struct Cyc_Absyn_Exp*_tmp37B;struct Cyc_Absyn_Stmt*
_tmp37C;struct Cyc_Absyn_Stmt*_tmp37D;struct Cyc_Absyn_Stmt*_tmp37E;struct _tuple3
_tmp37F;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_Absyn_Stmt*_tmp381;struct Cyc_Absyn_Exp*
_tmp382;struct _tuple3 _tmp383;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Stmt*
_tmp385;struct _tuple3 _tmp386;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Stmt*
_tmp388;struct Cyc_Absyn_Stmt*_tmp389;struct Cyc_Absyn_Stmt*_tmp38A;struct Cyc_List_List*
_tmp38B;struct Cyc_Absyn_Switch_clause**_tmp38C;struct Cyc_Absyn_Switch_clause*
_tmp38D;struct Cyc_Absyn_Stmt*_tmp38E;struct Cyc_Absyn_Stmt*_tmp38F;struct Cyc_Absyn_Stmt*
_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_List_List*_tmp392;struct Cyc_Absyn_Stmt*
_tmp393;struct Cyc_List_List*_tmp394;struct Cyc_Absyn_Decl*_tmp395;struct Cyc_Absyn_Stmt*
_tmp396;struct Cyc_Absyn_Stmt*_tmp397;struct Cyc_Absyn_Tvar*_tmp398;struct Cyc_Absyn_Vardecl*
_tmp399;int _tmp39A;struct Cyc_Absyn_Stmt*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;
_LL252: if((int)_tmp371 != 0)goto _LL254;_LL253: return inflow;_LL254: if(_tmp371 <= (
void*)1?1:*((int*)_tmp371)!= 2)goto _LL256;_tmp372=((struct Cyc_Absyn_Return_s_struct*)
_tmp371)->f1;if(_tmp372 != 0)goto _LL256;_LL255: if(env->noreturn)({void*_tmp39D[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp39E="`noreturn' function might return";
_tag_arr(_tmp39E,sizeof(char),_get_zero_arr_size(_tmp39E,33));}),_tag_arr(
_tmp39D,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,
inflow);return(void*)0;_LL256: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 2)goto
_LL258;_tmp373=((struct Cyc_Absyn_Return_s_struct*)_tmp371)->f1;_LL257: if(env->noreturn)({
void*_tmp39F[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3A0="`noreturn' function might return";
_tag_arr(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,33));}),_tag_arr(
_tmp39F,sizeof(void*),0));});{void*_tmp3A2;void*_tmp3A3;struct _tuple0 _tmp3A1=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp373));_tmp3A2=_tmp3A1.f1;
_tmp3A3=_tmp3A1.f2;_tmp3A2=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)
_check_null(_tmp373))->loc,_tmp3A2,_tmp3A3);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp3A2);return(void*)0;}_LL258: if(_tmp371 <= (void*)1?1:*((int*)
_tmp371)!= 0)goto _LL25A;_tmp374=((struct Cyc_Absyn_Exp_s_struct*)_tmp371)->f1;
_LL259: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp374)).f1;_LL25A: if(
_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 1)goto _LL25C;_tmp375=((struct Cyc_Absyn_Seq_s_struct*)
_tmp371)->f1;_tmp376=((struct Cyc_Absyn_Seq_s_struct*)_tmp371)->f2;_LL25B: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp375),
_tmp376);_LL25C: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 3)goto _LL25E;_tmp377=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp371)->f1;_tmp378=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp371)->f2;_tmp379=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp371)->f3;_LL25D: {
void*_tmp3A5;void*_tmp3A6;struct _tuple0 _tmp3A4=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp377);_tmp3A5=_tmp3A4.f1;_tmp3A6=_tmp3A4.f2;return Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_anal_stmt(env,_tmp3A5,_tmp378),Cyc_NewControlFlow_anal_stmt(
env,_tmp3A6,_tmp379));}_LL25E: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 4)goto
_LL260;_tmp37A=((struct Cyc_Absyn_While_s_struct*)_tmp371)->f1;_tmp37B=_tmp37A.f1;
_tmp37C=_tmp37A.f2;_tmp37D=((struct Cyc_Absyn_While_s_struct*)_tmp371)->f2;_LL25F: {
void*_tmp3A7=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp37C))->flow;
void*_tmp3A9;void*_tmp3AA;struct _tuple0 _tmp3A8=Cyc_NewControlFlow_anal_test(env,
_tmp3A7,_tmp37B);_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;{void*_tmp3AB=Cyc_NewControlFlow_anal_stmt(
env,_tmp3A9,_tmp37D);Cyc_NewControlFlow_update_flow(env,_tmp37C,_tmp3AB);return
_tmp3AA;}}_LL260: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 16)goto _LL262;
_tmp37E=((struct Cyc_Absyn_Do_s_struct*)_tmp371)->f1;_tmp37F=((struct Cyc_Absyn_Do_s_struct*)
_tmp371)->f2;_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;_LL261: {void*_tmp3AC=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp37E);void*_tmp3AD=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3AC,_tmp381))->flow;void*_tmp3AF;void*_tmp3B0;struct _tuple0 _tmp3AE=Cyc_NewControlFlow_anal_test(
env,_tmp3AD,_tmp380);_tmp3AF=_tmp3AE.f1;_tmp3B0=_tmp3AE.f2;Cyc_NewControlFlow_update_flow(
env,_tmp37E,_tmp3AF);return _tmp3B0;}_LL262: if(_tmp371 <= (void*)1?1:*((int*)
_tmp371)!= 8)goto _LL264;_tmp382=((struct Cyc_Absyn_For_s_struct*)_tmp371)->f1;
_tmp383=((struct Cyc_Absyn_For_s_struct*)_tmp371)->f2;_tmp384=_tmp383.f1;_tmp385=
_tmp383.f2;_tmp386=((struct Cyc_Absyn_For_s_struct*)_tmp371)->f3;_tmp387=_tmp386.f1;
_tmp388=_tmp386.f2;_tmp389=((struct Cyc_Absyn_For_s_struct*)_tmp371)->f4;_LL263: {
void*_tmp3B1=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp382)).f1;void*_tmp3B2=(
void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp3B1,_tmp385))->flow;void*
_tmp3B4;void*_tmp3B5;struct _tuple0 _tmp3B3=Cyc_NewControlFlow_anal_test(env,
_tmp3B2,_tmp384);_tmp3B4=_tmp3B3.f1;_tmp3B5=_tmp3B3.f2;{void*_tmp3B6=Cyc_NewControlFlow_anal_stmt(
env,_tmp3B4,_tmp389);void*_tmp3B7=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp3B6,_tmp388))->flow;void*_tmp3B8=(Cyc_NewControlFlow_anal_Rexp(env,_tmp3B7,
_tmp387)).f1;Cyc_NewControlFlow_update_flow(env,_tmp385,_tmp3B8);return _tmp3B5;}}
_LL264: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 5)goto _LL266;_tmp38A=((struct
Cyc_Absyn_Break_s_struct*)_tmp371)->f1;if(_tmp38A != 0)goto _LL266;_LL265: return(
void*)0;_LL266: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 11)goto _LL268;_tmp38B=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp371)->f1;_tmp38C=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp371)->f2;if(_tmp38C == 0)goto _LL268;_tmp38D=*_tmp38C;_LL267: {void*_tmp3BA;
struct Cyc_List_List*_tmp3BB;struct _tuple5 _tmp3B9=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp38B);_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B9.f2;for(0;_tmp3BB != 0;(
_tmp3BB=_tmp3BB->tl,_tmp38B=_tmp38B->tl)){_tmp3BA=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp38B))->hd)->loc,
_tmp3BA,(void*)_tmp3BB->hd);}_tmp3BA=Cyc_NewControlFlow_add_vars(_tmp3BA,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp38D->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp38D->body,_tmp3BA);
return(void*)0;}_LL268: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 5)goto _LL26A;
_tmp38E=((struct Cyc_Absyn_Break_s_struct*)_tmp371)->f1;_LL269: _tmp38F=_tmp38E;
goto _LL26B;_LL26A: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 6)goto _LL26C;
_tmp38F=((struct Cyc_Absyn_Continue_s_struct*)_tmp371)->f1;_LL26B: _tmp390=_tmp38F;
goto _LL26D;_LL26C: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 7)goto _LL26E;
_tmp390=((struct Cyc_Absyn_Goto_s_struct*)_tmp371)->f2;_LL26D: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp3BC=_tmp370->encloser;struct Cyc_Absyn_Stmt*
_tmp3BD=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp390)))->encloser;while(_tmp3BD != _tmp3BC){struct Cyc_Absyn_Stmt*_tmp3BE=(Cyc_NewControlFlow_get_stmt_annot(
_tmp3BC))->encloser;if(_tmp3BE == _tmp3BC){({void*_tmp3BF[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp3C0="goto enters local scope or exception handler";_tag_arr(
_tmp3C0,sizeof(char),_get_zero_arr_size(_tmp3C0,45));}),_tag_arr(_tmp3BF,sizeof(
void*),0));});break;}_tmp3BC=_tmp3BE;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp390),inflow);return(void*)0;_LL26E: if(
_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 9)goto _LL270;_tmp391=((struct Cyc_Absyn_Switch_s_struct*)
_tmp371)->f1;_tmp392=((struct Cyc_Absyn_Switch_s_struct*)_tmp371)->f2;_LL26F: {
void*_tmp3C2;void*_tmp3C3;struct _tuple0 _tmp3C1=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp391);_tmp3C2=_tmp3C1.f1;_tmp3C3=_tmp3C1.f2;_tmp3C2=Cyc_NewControlFlow_use_Rval(
env,_tmp391->loc,_tmp3C2,_tmp3C3);return Cyc_NewControlFlow_anal_scs(env,_tmp3C2,
_tmp392);}_LL270: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 17)goto _LL272;
_tmp393=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp371)->f1;_tmp394=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp371)->f2;_LL271: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp393);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp394);{
void*_tmp3C4=scs_outflow;_LL27D: if((int)_tmp3C4 != 0)goto _LL27F;_LL27E: goto _LL27C;
_LL27F:;_LL280:({void*_tmp3C5[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp3C6="last catch clause may implicitly fallthru";
_tag_arr(_tmp3C6,sizeof(char),_get_zero_arr_size(_tmp3C6,42));}),_tag_arr(
_tmp3C5,sizeof(void*),0));});_LL27C:;}return s1_outflow;}}}_LL272: if(_tmp371 <= (
void*)1?1:*((int*)_tmp371)!= 12)goto _LL274;_tmp395=((struct Cyc_Absyn_Decl_s_struct*)
_tmp371)->f1;_tmp396=((struct Cyc_Absyn_Decl_s_struct*)_tmp371)->f2;_LL273: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp395),
_tmp396);_LL274: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 15)goto _LL276;_tmp397=((
struct Cyc_Absyn_Label_s_struct*)_tmp371)->f2;_LL275: return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp397);_LL276: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 18)goto
_LL278;_tmp398=((struct Cyc_Absyn_Region_s_struct*)_tmp371)->f1;_tmp399=((struct
Cyc_Absyn_Region_s_struct*)_tmp371)->f2;_tmp39A=((struct Cyc_Absyn_Region_s_struct*)
_tmp371)->f3;_tmp39B=((struct Cyc_Absyn_Region_s_struct*)_tmp371)->f4;_LL277: {
struct Cyc_List_List _tmp3C7=({struct Cyc_List_List _tmp3C8;_tmp3C8.hd=_tmp399;
_tmp3C8.tl=0;_tmp3C8;});inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)&
_tmp3C7,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp39B);}_LL278: if(_tmp371 <= (void*)1?1:*((int*)_tmp371)!= 20)goto _LL27A;
_tmp39C=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp371)->f1;_LL279: {void*
_tmp3CA;void*_tmp3CB;struct _tuple0 _tmp3C9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp39C);_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;{void*_tmp3CC=Cyc_NewControlFlow_use_Rval(
env,_tmp39C->loc,_tmp3CA,_tmp3CB);void*_tmp3CD=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp39C->topt))->v);void*_tmp3CE;_LL282: if(
_tmp3CD <= (void*)3?1:*((int*)_tmp3CD)!= 15)goto _LL284;_tmp3CE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3CD)->f1;_LL283: return Cyc_CfFlowInfo_kill_flow_region(_tmp3CA,_tmp3CE);
_LL284:;_LL285:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3CF=
_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Core_Impossible_struct _tmp3D0;
_tmp3D0.tag=Cyc_Core_Impossible;_tmp3D0.f1=({const char*_tmp3D1="anal_stmt -- reset_region";
_tag_arr(_tmp3D1,sizeof(char),_get_zero_arr_size(_tmp3D1,26));});_tmp3D0;});
_tmp3CF;}));_LL281:;}}_LL27A:;_LL27B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_Core_Impossible_struct
_tmp3D3;_tmp3D3.tag=Cyc_Core_Impossible;_tmp3D3.f1=({const char*_tmp3D4="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_arr(_tmp3D4,sizeof(char),_get_zero_arr_size(_tmp3D4,56));});_tmp3D3;});
_tmp3D2;}));_LL251:;}}static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp3D5=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp3D6;struct Cyc_Core_Opt*_tmp3D7;struct Cyc_Core_Opt _tmp3D8;struct Cyc_List_List*
_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_List_List*_tmp3DB;_LL287: if(*((int*)
_tmp3D5)!= 0)goto _LL289;_tmp3D6=((struct Cyc_Absyn_Var_d_struct*)_tmp3D5)->f1;
_LL288: {struct Cyc_List_List _tmp3DC=({struct Cyc_List_List _tmp3EA;_tmp3EA.hd=
_tmp3D6;_tmp3EA.tl=0;_tmp3EA;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp3DC,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp3DD=_tmp3D6->initializer;if(_tmp3DD == 0)return inflow;{void*_tmp3DF;void*
_tmp3E0;struct _tuple0 _tmp3DE=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3DD));_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;{void*_tmp3E1=
_tmp3DF;struct Cyc_Dict_Dict*_tmp3E2;struct Cyc_List_List*_tmp3E3;_LL290: if((int)
_tmp3E1 != 0)goto _LL292;_LL291: return(void*)0;_LL292: if(_tmp3E1 <= (void*)1?1:*((
int*)_tmp3E1)!= 0)goto _LL28F;_tmp3E2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3E1)->f1;_tmp3E3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3E1)->f2;
_LL293: _tmp3E2=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp3E2,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp3E5=_cycalloc(sizeof(*
_tmp3E5));_tmp3E5[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp3E6;_tmp3E6.tag=0;
_tmp3E6.f1=_tmp3D6;_tmp3E6;});_tmp3E5;}));_tmp3E4->fields=0;_tmp3E4;}),_tmp3E0);
_tmp3E3=Cyc_CfFlowInfo_reln_assign_var(_tmp3E3,_tmp3D6,(struct Cyc_Absyn_Exp*)
_check_null(_tmp3D6->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3E7=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3E8=_cycalloc(sizeof(*
_tmp3E8));_tmp3E8[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3E9;_tmp3E9.tag=
0;_tmp3E9.f1=_tmp3E2;_tmp3E9.f2=_tmp3E3;_tmp3E9;});_tmp3E8;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp3E7);return(void*)_tmp3E7;}_LL28F:;}}}}_LL289: if(*((int*)_tmp3D5)
!= 2)goto _LL28B;_tmp3D7=((struct Cyc_Absyn_Let_d_struct*)_tmp3D5)->f2;if(_tmp3D7
== 0)goto _LL28B;_tmp3D8=*_tmp3D7;_tmp3D9=(struct Cyc_List_List*)_tmp3D8.v;_tmp3DA=((
struct Cyc_Absyn_Let_d_struct*)_tmp3D5)->f3;_LL28A: {void*_tmp3EC;void*_tmp3ED;
struct _tuple0 _tmp3EB=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp3DA);_tmp3EC=
_tmp3EB.f1;_tmp3ED=_tmp3EB.f2;_tmp3EC=Cyc_NewControlFlow_use_Rval(env,_tmp3DA->loc,
_tmp3EC,_tmp3ED);return Cyc_NewControlFlow_add_vars(_tmp3EC,_tmp3D9,Cyc_CfFlowInfo_unknown_all);}
_LL28B: if(*((int*)_tmp3D5)!= 3)goto _LL28D;_tmp3DB=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3D5)->f1;_LL28C: return Cyc_NewControlFlow_add_vars(inflow,_tmp3DB,Cyc_CfFlowInfo_unknown_none);
_LL28D:;_LL28E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3EE=
_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_Core_Impossible_struct _tmp3EF;
_tmp3EF.tag=Cyc_Core_Impossible;_tmp3EF.f1=({const char*_tmp3F0="anal_decl: unexpected decl variant";
_tag_arr(_tmp3F0,sizeof(char),_get_zero_arr_size(_tmp3F0,35));});_tmp3EF;});
_tmp3EE;}));_LL286:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp3F1=(void*)s->r;
struct _tuple3 _tmp3F2;struct Cyc_Absyn_Stmt*_tmp3F3;struct _tuple3 _tmp3F4;struct Cyc_Absyn_Stmt*
_tmp3F5;struct Cyc_Absyn_Stmt*_tmp3F6;struct Cyc_Absyn_Stmt*_tmp3F7;struct Cyc_Absyn_Stmt*
_tmp3F8;struct _tuple3 _tmp3F9;struct Cyc_Absyn_Stmt*_tmp3FA;struct Cyc_Absyn_Stmt*
_tmp3FB;struct Cyc_Absyn_Stmt*_tmp3FC;struct _tuple3 _tmp3FD;struct Cyc_Absyn_Stmt*
_tmp3FE;struct Cyc_Absyn_Stmt*_tmp3FF;struct Cyc_Absyn_Stmt*_tmp400;struct Cyc_Absyn_Stmt*
_tmp401;struct Cyc_Absyn_Stmt*_tmp402;struct Cyc_Absyn_Stmt*_tmp403;struct Cyc_Absyn_Stmt*
_tmp404;struct Cyc_List_List*_tmp405;struct Cyc_List_List*_tmp406;_LL295: if(
_tmp3F1 <= (void*)1?1:*((int*)_tmp3F1)!= 8)goto _LL297;_tmp3F2=((struct Cyc_Absyn_For_s_struct*)
_tmp3F1)->f2;_tmp3F3=_tmp3F2.f2;_tmp3F4=((struct Cyc_Absyn_For_s_struct*)_tmp3F1)->f3;
_tmp3F5=_tmp3F4.f2;_tmp3F6=((struct Cyc_Absyn_For_s_struct*)_tmp3F1)->f4;_LL296:
Cyc_NewControlFlow_postpass(_tmp3F5);_tmp3F7=_tmp3F6;_tmp3F8=_tmp3F3;goto _LL298;
_LL297: if(_tmp3F1 <= (void*)1?1:*((int*)_tmp3F1)!= 1)goto _LL299;_tmp3F7=((struct
Cyc_Absyn_Seq_s_struct*)_tmp3F1)->f1;_tmp3F8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3F1)->f2;_LL298: _tmp3FA=_tmp3F7;_tmp3FB=_tmp3F8;goto _LL29A;_LL299: if(_tmp3F1
<= (void*)1?1:*((int*)_tmp3F1)!= 4)goto _LL29B;_tmp3F9=((struct Cyc_Absyn_While_s_struct*)
_tmp3F1)->f1;_tmp3FA=_tmp3F9.f2;_tmp3FB=((struct Cyc_Absyn_While_s_struct*)
_tmp3F1)->f2;_LL29A: _tmp3FC=_tmp3FB;_tmp3FE=_tmp3FA;goto _LL29C;_LL29B: if(_tmp3F1
<= (void*)1?1:*((int*)_tmp3F1)!= 16)goto _LL29D;_tmp3FC=((struct Cyc_Absyn_Do_s_struct*)
_tmp3F1)->f1;_tmp3FD=((struct Cyc_Absyn_Do_s_struct*)_tmp3F1)->f2;_tmp3FE=_tmp3FD.f2;
_LL29C: _tmp3FF=_tmp3FC;_tmp400=_tmp3FE;goto _LL29E;_LL29D: if(_tmp3F1 <= (void*)1?1:*((
int*)_tmp3F1)!= 3)goto _LL29F;_tmp3FF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3F1)->f2;_tmp400=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3F1)->f3;_LL29E:
Cyc_NewControlFlow_postpass(_tmp3FF);_tmp401=_tmp400;goto _LL2A0;_LL29F: if(
_tmp3F1 <= (void*)1?1:*((int*)_tmp3F1)!= 18)goto _LL2A1;_tmp401=((struct Cyc_Absyn_Region_s_struct*)
_tmp3F1)->f4;_LL2A0: _tmp402=_tmp401;goto _LL2A2;_LL2A1: if(_tmp3F1 <= (void*)1?1:*((
int*)_tmp3F1)!= 15)goto _LL2A3;_tmp402=((struct Cyc_Absyn_Label_s_struct*)_tmp3F1)->f2;
_LL2A2: _tmp403=_tmp402;goto _LL2A4;_LL2A3: if(_tmp3F1 <= (void*)1?1:*((int*)_tmp3F1)
!= 12)goto _LL2A5;_tmp403=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F1)->f2;_LL2A4:
Cyc_NewControlFlow_postpass(_tmp403);return;_LL2A5: if(_tmp3F1 <= (void*)1?1:*((
int*)_tmp3F1)!= 17)goto _LL2A7;_tmp404=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3F1)->f1;_tmp405=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3F1)->f2;_LL2A6:
Cyc_NewControlFlow_postpass(_tmp404);_tmp406=_tmp405;goto _LL2A8;_LL2A7: if(
_tmp3F1 <= (void*)1?1:*((int*)_tmp3F1)!= 9)goto _LL2A9;_tmp406=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3F1)->f2;_LL2A8: for(0;_tmp406 != 0;_tmp406=_tmp406->tl){Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*)_tmp406->hd)->body);}return;_LL2A9:;_LL2AA:
return;_LL294:;}}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Position_Segment*_tmp407=(fd->body)->loc;void*inflow=Cyc_NewControlFlow_add_vars((
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp43B=_cycalloc(sizeof(*
_tmp43B));_tmp43B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp43C;_tmp43C.tag=
0;_tmp43C.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp43C.f2=0;_tmp43C;});
_tmp43B;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);struct Cyc_List_List*_tmp408=0;{void*_tmp409=inflow;
struct Cyc_Dict_Dict*_tmp40A;struct Cyc_List_List*_tmp40B;_LL2AC: if((int)_tmp409 != 
0)goto _LL2AE;_LL2AD:({void*_tmp40C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp40D="check_fun";_tag_arr(
_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,10));}),_tag_arr(_tmp40C,sizeof(
void*),0));});_LL2AE: if(_tmp409 <= (void*)1?1:*((int*)_tmp409)!= 0)goto _LL2AB;
_tmp40A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp409)->f1;_tmp40B=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp409)->f2;_LL2AF: {struct Cyc_List_List*
atts;{void*_tmp40E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp40F;struct Cyc_List_List*_tmp410;
_LL2B1: if(_tmp40E <= (void*)3?1:*((int*)_tmp40E)!= 8)goto _LL2B3;_tmp40F=((struct
Cyc_Absyn_FnType_struct*)_tmp40E)->f1;_tmp410=_tmp40F.attributes;_LL2B2: atts=
_tmp410;goto _LL2B0;_LL2B3:;_LL2B4:({void*_tmp411[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp412="check_fun: non-function type cached with fndecl_t";
_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,50));}),_tag_arr(
_tmp411,sizeof(void*),0));});_LL2B0:;}for(0;atts != 0;atts=atts->tl){void*_tmp413=(
void*)atts->hd;int _tmp414;_LL2B6: if(_tmp413 <= (void*)17?1:*((int*)_tmp413)!= 4)
goto _LL2B8;_tmp414=((struct Cyc_Absyn_Initializes_att_struct*)_tmp413)->f1;_LL2B7: {
unsigned int j=(unsigned int)_tmp414;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp415[0]={};Cyc_Tcutil_terr(_tmp407,({const char*_tmp416="initializes attribute exceeds number of parameters";
_tag_arr(_tmp416,sizeof(char),_get_zero_arr_size(_tmp416,51));}),_tag_arr(
_tmp415,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp417=((struct
Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*
_tmp418=Cyc_Tcutil_compress((void*)_tmp417->type);struct Cyc_Absyn_PtrInfo _tmp419;
void*_tmp41A;struct Cyc_Absyn_Conref*_tmp41B;struct Cyc_Absyn_Conref*_tmp41C;
struct Cyc_Absyn_Conref*_tmp41D;_LL2BB: if(_tmp418 <= (void*)3?1:*((int*)_tmp418)!= 
4)goto _LL2BD;_tmp419=((struct Cyc_Absyn_PointerType_struct*)_tmp418)->f1;_tmp41A=(
void*)_tmp419.elt_typ;_tmp41B=_tmp419.nullable;_tmp41C=_tmp419.bounds;_tmp41D=
_tmp419.zero_term;_LL2BC: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp41B))({void*_tmp41E[0]={};Cyc_Tcutil_terr(_tmp407,({const char*_tmp41F="initializes attribute not allowed on nullable pointers";
_tag_arr(_tmp41F,sizeof(char),_get_zero_arr_size(_tmp41F,55));}),_tag_arr(
_tmp41E,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp41C))({void*_tmp420[
0]={};Cyc_Tcutil_terr(_tmp407,({const char*_tmp421="initializes attribute allowed only on pointers of size 1";
_tag_arr(_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,57));}),_tag_arr(
_tmp420,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp41D))({void*_tmp422[0]={};Cyc_Tcutil_terr(_tmp407,({const char*_tmp423="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_arr(_tmp423,sizeof(char),_get_zero_arr_size(_tmp423,77));}),_tag_arr(
_tmp422,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp424=({
struct Cyc_CfFlowInfo_InitParam_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));
_tmp42C[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp42D;_tmp42D.tag=2;_tmp42D.f1=(
int)j;_tmp42D.f2=(void*)_tmp41A;_tmp42D;});_tmp42C;});struct Cyc_CfFlowInfo_Place*
_tmp425=({struct Cyc_CfFlowInfo_Place*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->root=(
void*)((void*)_tmp424);_tmp42B->fields=0;_tmp42B;});_tmp40A=Cyc_Dict_insert(
_tmp40A,(void*)_tmp424,Cyc_CfFlowInfo_typ_to_absrval(_tmp41A,Cyc_CfFlowInfo_esc_none));
_tmp40A=Cyc_Dict_insert(_tmp40A,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp427;_tmp427.tag=0;_tmp427.f1=_tmp417;_tmp427;});_tmp426;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp429;_tmp429.tag=2;_tmp429.f1=_tmp425;
_tmp429;});_tmp428;}));_tmp408=({struct Cyc_List_List*_tmp42A=_cycalloc(sizeof(*
_tmp42A));_tmp42A->hd=_tmp425;_tmp42A->tl=_tmp408;_tmp42A;});goto _LL2BA;}_LL2BD:;
_LL2BE:({void*_tmp42E[0]={};Cyc_Tcutil_terr(_tmp407,({const char*_tmp42F="initializes attribute on non-pointer";
_tag_arr(_tmp42F,sizeof(char),_get_zero_arr_size(_tmp42F,37));}),_tag_arr(
_tmp42E,sizeof(void*),0));});_LL2BA:;}goto _LL2B5;}}_LL2B8:;_LL2B9: goto _LL2B5;
_LL2B5:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp430=
_cycalloc(sizeof(*_tmp430));_tmp430[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp431;_tmp431.tag=0;_tmp431.f1=_tmp40A;_tmp431.f2=_tmp40B;_tmp431;});_tmp430;});}
_LL2AB:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct
Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp43A=
_cycalloc(sizeof(*_tmp43A));_tmp43A->iterate_again=1;_tmp43A->iteration_num=0;
_tmp43A->in_try=0;_tmp43A->tryflow=(void*)inflow;_tmp43A->all_changed=0;_tmp43A->noreturn=
noreturn;_tmp43A->param_roots=_tmp408;_tmp43A;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp432=outflow;_LL2C0: if((int)_tmp432 != 0)goto
_LL2C2;_LL2C1: goto _LL2BF;_LL2C2:;_LL2C3: Cyc_NewControlFlow_check_init_params(
_tmp407,env,outflow);if(noreturn)({void*_tmp433[0]={};Cyc_Tcutil_terr(_tmp407,({
const char*_tmp434="`noreturn' function might (implicitly) return";_tag_arr(
_tmp434,sizeof(char),_get_zero_arr_size(_tmp434,46));}),_tag_arr(_tmp433,sizeof(
void*),0));});else{void*_tmp435=Cyc_Tcutil_compress((void*)fd->ret_type);_LL2C5:
if((int)_tmp435 != 0)goto _LL2C7;_LL2C6: goto _LL2C4;_LL2C7: if(_tmp435 <= (void*)3?1:*((
int*)_tmp435)!= 6)goto _LL2C9;_LL2C8: goto _LL2CA;_LL2C9: if((int)_tmp435 != 1)goto
_LL2CB;_LL2CA: goto _LL2CC;_LL2CB: if(_tmp435 <= (void*)3?1:*((int*)_tmp435)!= 5)
goto _LL2CD;_LL2CC:({void*_tmp436[0]={};Cyc_Tcutil_warn(_tmp407,({const char*
_tmp437="function may not return a value";_tag_arr(_tmp437,sizeof(char),
_get_zero_arr_size(_tmp437,32));}),_tag_arr(_tmp436,sizeof(void*),0));});goto
_LL2C4;_LL2CD:;_LL2CE:({void*_tmp438[0]={};Cyc_Tcutil_terr(_tmp407,({const char*
_tmp439="function may not return a value";_tag_arr(_tmp439,sizeof(char),
_get_zero_arr_size(_tmp439,32));}),_tag_arr(_tmp438,sizeof(void*),0));});goto
_LL2C4;_LL2C4:;}goto _LL2BF;_LL2BF:;}Cyc_NewControlFlow_postpass(fd->body);}}void
Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*
_tmp43D=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp43E;
struct Cyc_List_List*_tmp43F;struct Cyc_List_List*_tmp440;struct Cyc_List_List*
_tmp441;_LL2D0: if(*((int*)_tmp43D)!= 1)goto _LL2D2;_tmp43E=((struct Cyc_Absyn_Fn_d_struct*)
_tmp43D)->f1;_LL2D1: Cyc_NewControlFlow_check_fun(_tmp43E);goto _LL2CF;_LL2D2: if(*((
int*)_tmp43D)!= 10)goto _LL2D4;_tmp43F=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp43D)->f1;_LL2D3: _tmp440=_tmp43F;goto _LL2D5;_LL2D4: if(*((int*)_tmp43D)!= 9)
goto _LL2D6;_tmp440=((struct Cyc_Absyn_Using_d_struct*)_tmp43D)->f2;_LL2D5: _tmp441=
_tmp440;goto _LL2D7;_LL2D6: if(*((int*)_tmp43D)!= 8)goto _LL2D8;_tmp441=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp43D)->f2;_LL2D7: Cyc_NewControlFlow_cf_check(_tmp441);goto _LL2CF;_LL2D8:;
_LL2D9: goto _LL2CF;_LL2CF:;}}
