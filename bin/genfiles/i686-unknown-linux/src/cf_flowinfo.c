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
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(
void(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void*Cyc_Dict_fold_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,
struct Cyc_Dict_Dict*d2);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct _tagged_arr*Cyc_Absyn_fieldname(int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct
Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[15]="\000\000\000\000HasTagCmps\000";
struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[
11]="\000\000\000\000IsZero\000";char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
13]="\000\000\000\000UnknownZ\000";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_this;
extern void*Cyc_CfFlowInfo_unknown_all;extern void*Cyc_CfFlowInfo_esc_none;extern
void*Cyc_CfFlowInfo_esc_this;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_Set_Set**all_changed,struct _tuple0 pr1,struct _tuple0 pr2);void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**,void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={
0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){
static struct Cyc_Set_Set**mt_place_set_opt=0;if(mt_place_set_opt == 0)
mt_place_set_opt=({struct Cyc_Set_Set**_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});return*((struct Cyc_Set_Set**)
_check_null(mt_place_set_opt));}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp8=({struct _tuple0 _tmp7;_tmp7.f1=r1;
_tmp7.f2=r2;_tmp7;});void*_tmp9;struct Cyc_Absyn_Vardecl*_tmpA;void*_tmpB;struct
Cyc_Absyn_Vardecl*_tmpC;void*_tmpD;void*_tmpE;void*_tmpF;struct Cyc_Absyn_Exp*
_tmp10;void*_tmp11;struct Cyc_Absyn_Exp*_tmp12;void*_tmp13;void*_tmp14;void*
_tmp15;int _tmp16;void*_tmp17;int _tmp18;_LL1: _tmp9=_tmp8.f1;if(*((int*)_tmp9)!= 0)
goto _LL3;_tmpA=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp9)->f1;_tmpB=_tmp8.f2;
if(*((int*)_tmpB)!= 0)goto _LL3;_tmpC=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpB)->f1;_LL2: return(int)_tmpA - (int)_tmpC;_LL3: _tmpD=_tmp8.f1;if(*((int*)
_tmpD)!= 0)goto _LL5;_LL4: return - 1;_LL5: _tmpE=_tmp8.f2;if(*((int*)_tmpE)!= 0)goto
_LL7;_LL6: return 1;_LL7: _tmpF=_tmp8.f1;if(*((int*)_tmpF)!= 1)goto _LL9;_tmp10=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpF)->f1;_tmp11=_tmp8.f2;if(*((int*)
_tmp11)!= 1)goto _LL9;_tmp12=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp11)->f1;
_LL8: return(int)_tmp10 - (int)_tmp12;_LL9: _tmp13=_tmp8.f1;if(*((int*)_tmp13)!= 1)
goto _LLB;_LLA: return - 1;_LLB: _tmp14=_tmp8.f2;if(*((int*)_tmp14)!= 1)goto _LLD;_LLC:
return 1;_LLD: _tmp15=_tmp8.f1;if(*((int*)_tmp15)!= 2)goto _LL0;_tmp16=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp15)->f1;_tmp17=_tmp8.f2;if(*((int*)_tmp17)!= 2)goto _LL0;_tmp18=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp17)->f1;_LLE: return _tmp16 - _tmp18;_LL0:;}}int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)
return 0;{int i=Cyc_CfFlowInfo_root_cmp((void*)p1->root,(void*)p2->root);if(i != 0)
return i;return((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,p1->fields,
p2->fields);}}static struct _tagged_arr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*
p){struct Cyc_List_List*sl=0;{void*_tmp19=(void*)p->root;struct Cyc_Absyn_Vardecl*
_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;int _tmp1C;_LL10: if(*((int*)_tmp19)!= 0)goto
_LL12;_tmp1A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp19)->f1;_LL11: sl=({
struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=({struct
_tagged_arr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp1A->name).f2);{
void*_tmp1F[1]={& _tmp21};Cyc_aprintf(({const char*_tmp20="%s";_tag_arr(_tmp20,
sizeof(char),_get_zero_arr_size(_tmp20,3));}),_tag_arr(_tmp1F,sizeof(void*),1));}});
_tmp1E;});_tmp1D->tl=sl;_tmp1D;});goto _LLF;_LL12: if(*((int*)_tmp19)!= 1)goto
_LL14;_tmp1B=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp19)->f1;_LL13: sl=({
struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=({struct
_tagged_arr*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Int_pa_struct
_tmp26;_tmp26.tag=1;_tmp26.f1=(unsigned int)((int)_tmp1B);{void*_tmp24[1]={&
_tmp26};Cyc_aprintf(({const char*_tmp25="mpt%d";_tag_arr(_tmp25,sizeof(char),
_get_zero_arr_size(_tmp25,6));}),_tag_arr(_tmp24,sizeof(void*),1));}});_tmp23;});
_tmp22->tl=sl;_tmp22;});goto _LLF;_LL14: if(*((int*)_tmp19)!= 2)goto _LLF;_tmp1C=((
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp19)->f1;_LL15: sl=({struct Cyc_List_List*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=({struct _tagged_arr*_tmp28=
_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Int_pa_struct _tmp2B;_tmp2B.tag=
1;_tmp2B.f1=(unsigned int)_tmp1C;{void*_tmp29[1]={& _tmp2B};Cyc_aprintf(({const
char*_tmp2A="param%d";_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,8));}),
_tag_arr(_tmp29,sizeof(void*),1));}});_tmp28;});_tmp27->tl=sl;_tmp27;});goto _LLF;
_LLF:;}{struct Cyc_List_List*fields=p->fields;for(0;fields != 0;fields=fields->tl){
sl=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=({struct
_tagged_arr*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)fields->hd));{void*_tmp2E[1]={& _tmp30};Cyc_aprintf(({const char*
_tmp2F="%s";_tag_arr(_tmp2F,sizeof(char),_get_zero_arr_size(_tmp2F,3));}),
_tag_arr(_tmp2E,sizeof(void*),1));}});_tmp2D;});_tmp2C->tl=sl;_tmp2C;});}}{
struct _tagged_arr*_tmp31=({struct _tagged_arr*_tmp36=_cycalloc(sizeof(*_tmp36));
_tmp36[0]=({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct
_tagged_arr)({const char*_tmp3A="";_tag_arr(_tmp3A,sizeof(char),
_get_zero_arr_size(_tmp3A,1));});{void*_tmp37[1]={& _tmp39};Cyc_aprintf(({const
char*_tmp38="%s";_tag_arr(_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,3));}),
_tag_arr(_tmp37,sizeof(void*),1));}});_tmp36;});for(0;sl != 0;sl=sl->tl){*_tmp31=({
struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp31);{struct Cyc_String_pa_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)sl->hd));{void*
_tmp32[2]={& _tmp34,& _tmp35};Cyc_aprintf(({const char*_tmp33="%s.%s";_tag_arr(
_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,6));}),_tag_arr(_tmp32,sizeof(void*),
2));}}});}return _tmp31;}}struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};static
void*Cyc_CfFlowInfo_i_typ_to_absrval(int allow_zeroterm,void*t,void*leafval){if(!
Cyc_Absyn_is_union_type(t)){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo
_tmp3C;void*_tmp3D;struct Cyc_Absyn_Tunionfield*_tmp3E;struct Cyc_List_List*_tmp3F;
struct Cyc_Absyn_AggrInfo _tmp40;void*_tmp41;void*_tmp42;struct Cyc_List_List*
_tmp43;struct Cyc_Absyn_ArrayInfo _tmp44;void*_tmp45;struct Cyc_Absyn_Conref*_tmp46;
void*_tmp47;_LL17: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 3)goto _LL19;_tmp3C=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3B)->f1;_tmp3D=(void*)_tmp3C.field_info;
if(*((int*)_tmp3D)!= 1)goto _LL19;_tmp3E=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3D)->f2;_LL18: if(_tmp3E->typs == 0)return leafval;_tmp3F=_tmp3E->typs;goto
_LL1A;_LL19: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 9)goto _LL1B;_tmp3F=((struct
Cyc_Absyn_TupleType_struct*)_tmp3B)->f1;_LL1A: {struct Cyc_Dict_Dict*d=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_strptrcmp);{int i=0;for(0;_tmp3F != 0;(_tmp3F=_tmp3F->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(
i),Cyc_CfFlowInfo_i_typ_to_absrval(0,(*((struct _tuple4*)_tmp3F->hd)).f2,leafval));}}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp48=_cycalloc(sizeof(*
_tmp48));_tmp48[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp49;_tmp49.tag=4;
_tmp49.f1=d;_tmp49;});_tmp48;});}_LL1B: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 
10)goto _LL1D;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3B)->f1;_tmp41=(void*)
_tmp40.aggr_info;_LL1C: {struct Cyc_Absyn_Aggrdecl*_tmp4A=Cyc_Absyn_get_known_aggrdecl(
_tmp41);if(_tmp4A->impl == 0)goto _LL16;_tmp43=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4A->impl))->fields;goto _LL1E;}_LL1D: if(_tmp3B <= (void*)3?1:*((
int*)_tmp3B)!= 11)goto _LL1F;_tmp42=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3B)->f1;if((int)_tmp42 != 0)goto _LL1F;_tmp43=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3B)->f2;_LL1E: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);for(0;
_tmp43 != 0;_tmp43=_tmp43->tl){struct Cyc_Absyn_Aggrfield _tmp4C;struct _tagged_arr*
_tmp4D;void*_tmp4E;struct Cyc_Absyn_Aggrfield*_tmp4B=(struct Cyc_Absyn_Aggrfield*)
_tmp43->hd;_tmp4C=*_tmp4B;_tmp4D=_tmp4C.name;_tmp4E=(void*)_tmp4C.type;if(
_get_arr_size(*_tmp4D,sizeof(char))!= 1)d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,_tmp4D,Cyc_CfFlowInfo_i_typ_to_absrval(
0,_tmp4E,leafval));}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4F=
_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp50;_tmp50.tag=4;_tmp50.f1=d;_tmp50;});_tmp4F;});}_LL1F: if(_tmp3B <= (void*)3?
1:*((int*)_tmp3B)!= 7)goto _LL21;_tmp44=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3B)->f1;_tmp45=(void*)_tmp44.elt_type;_tmp46=_tmp44.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp46))goto _LL21;_LL20:
return(allow_zeroterm?Cyc_Tcutil_bits_only(_tmp45): 0)?Cyc_CfFlowInfo_unknown_all:
leafval;_LL21: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 17)goto _LL23;_tmp47=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp3B)->f1;_LL22: {void*_tmp51=leafval;void*
_tmp52;void*_tmp53;_LL26: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 0)goto _LL28;
_tmp52=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp51)->f1;if((int)_tmp52
!= 2)goto _LL28;_LL27: goto _LL29;_LL28: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 1)
goto _LL2A;_tmp53=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp51)->f1;if((int)
_tmp53 != 2)goto _LL2A;_LL29: goto _LL2B;_LL2A: if((int)_tmp51 != 0)goto _LL2C;_LL2B:
goto _LL2D;_LL2C: if((int)_tmp51 != 1)goto _LL2E;_LL2D: return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp55;_tmp55.tag=3;_tmp55.f1=({struct Cyc_CfFlowInfo_TagCmp*_tmp56[1];_tmp56[0]=({
struct Cyc_CfFlowInfo_TagCmp*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->cmp=(void*)((
void*)5);_tmp57->bd=(void*)_tmp47;_tmp57;});((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp56,sizeof(struct Cyc_CfFlowInfo_TagCmp*),
1));});_tmp55;});_tmp54;});_LL2E:;_LL2F: return leafval;_LL25:;}_LL23:;_LL24: goto
_LL16;_LL16:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all: leafval;}
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Set_Set*set){struct Cyc_CfFlowInfo_Place*
_tmp58=place;struct Cyc_Iter_Iter _tmp59=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(r,set);while(((int(*)(
struct Cyc_Iter_Iter,struct Cyc_CfFlowInfo_Place**))Cyc_Iter_next)(_tmp59,& _tmp58)){
if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp58->root)!= 0)continue;{
struct Cyc_List_List*_tmp5A=place->fields;struct Cyc_List_List*_tmp5B=_tmp58->fields;
for(0;_tmp5A != 0?_tmp5B != 0: 0;(_tmp5A=_tmp5A->tl,_tmp5B=_tmp5B->tl)){if(Cyc_strptrcmp((
struct _tagged_arr*)_tmp5A->hd,(struct _tagged_arr*)_tmp5B->hd)!= 0)break;}if(
_tmp5A == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmp5C=_region_malloc(pile->rgn,sizeof(*_tmp5C));_tmp5C->hd=place;
_tmp5C->tl=pile->places;_tmp5C;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*a,void*r){void*_tmp5D=r;struct Cyc_CfFlowInfo_Place*_tmp5E;struct Cyc_Dict_Dict*
_tmp5F;_LL31: if(_tmp5D <= (void*)3?1:*((int*)_tmp5D)!= 2)goto _LL33;_tmp5E=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5D)->f1;_LL32: Cyc_CfFlowInfo_add_place(
pile,_tmp5E);return;_LL33: if(_tmp5D <= (void*)3?1:*((int*)_tmp5D)!= 4)goto _LL35;
_tmp5F=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp5D)->f1;_LL34:((void(*)(void(*
f)(struct Cyc_CfFlowInfo_EscPile*,struct _tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((void(*)(struct Cyc_CfFlowInfo_EscPile*
pile,struct _tagged_arr*a,void*r))Cyc_CfFlowInfo_add_places,pile,_tmp5F);return;
_LL35:;_LL36: return;_LL30:;}static void*Cyc_CfFlowInfo_insert_place_inner(void*
new_val,void*old_val){void*_tmp60=old_val;struct Cyc_Dict_Dict*_tmp61;_LL38: if(
_tmp60 <= (void*)3?1:*((int*)_tmp60)!= 4)goto _LL3A;_tmp61=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp60)->f1;_LL39: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp62=
_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp63;_tmp63.tag=4;_tmp63.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp61);_tmp63;});_tmp62;});_LL3A:;_LL3B: return new_val;_LL37:;}struct
_tuple5{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple5 _tmp65=({struct _tuple5 _tmp64;_tmp64.f1=fs;_tmp64.f2=
old_val;_tmp64;});struct Cyc_List_List*_tmp66;struct Cyc_List_List _tmp67;struct
_tagged_arr*_tmp68;struct Cyc_List_List*_tmp69;void*_tmp6A;struct Cyc_Dict_Dict*
_tmp6B;_LL3D: _tmp66=_tmp65.f1;if(_tmp66 == 0)goto _LL3F;_tmp67=*_tmp66;_tmp68=(
struct _tagged_arr*)_tmp67.hd;_tmp69=_tmp67.tl;_tmp6A=_tmp65.f2;if(_tmp6A <= (void*)
3?1:*((int*)_tmp6A)!= 4)goto _LL3F;_tmp6B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp6A)->f1;_LL3E: {void*_tmp6C=Cyc_CfFlowInfo_insert_place_outer(_tmp69,((void*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp6B,_tmp68),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp6D=_cycalloc(
sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp6E;_tmp6E.tag=
4;_tmp6E.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp6B,_tmp68,_tmp6C);_tmp6E;});_tmp6D;});}_LL3F:;_LL40:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6F=_cycalloc(sizeof(*
_tmp6F));_tmp6F[0]=({struct Cyc_Core_Impossible_struct _tmp70;_tmp70.tag=Cyc_Core_Impossible;
_tmp70.f1=({const char*_tmp71="bad insert place";_tag_arr(_tmp71,sizeof(char),
_get_zero_arr_size(_tmp71,17));});_tmp70;});_tmp6F;}));_LL3C:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Set_Set**
all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp72=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;if(
all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,_tmp72);{void*oldval;void*
newval;{struct _handler_cons _tmp73;_push_handler(& _tmp73);{int _tmp75=0;if(setjmp(
_tmp73.handler))_tmp75=1;if(!_tmp75){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp72);;
_pop_handler();}else{void*_tmp74=(void*)_exn_thrown;void*_tmp77=_tmp74;_LL42: if(
_tmp77 != Cyc_Dict_Absent)goto _LL44;_LL43: continue;_LL44:;_LL45:(void)_throw(
_tmp77);_LL41:;}}}{void*_tmp78=Cyc_CfFlowInfo_initlevel(d,oldval);_LL47: if((int)
_tmp78 != 2)goto _LL49;_LL48: newval=Cyc_CfFlowInfo_esc_all;goto _LL46;_LL49: if((int)
_tmp78 != 1)goto _LL4B;_LL4A: newval=Cyc_CfFlowInfo_esc_this;goto _LL46;_LL4B: if((
int)_tmp78 != 0)goto _LL46;_LL4C: newval=Cyc_CfFlowInfo_esc_none;goto _LL46;_LL46:;}((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp72->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp72->fields,Cyc_Dict_lookup(d,(void*)_tmp72->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp79=r;void*_tmp7A;
void*_tmp7B;_LL4E: if(_tmp79 <= (void*)3?1:*((int*)_tmp79)!= 0)goto _LL50;_tmp7A=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp79)->f1;_LL4F: return _tmp7A;
_LL50: if(_tmp79 <= (void*)3?1:*((int*)_tmp79)!= 1)goto _LL52;_tmp7B=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp79)->f1;_LL51: return _tmp7B;_LL52: if((int)
_tmp79 != 0)goto _LL54;_LL53: goto _LL55;_LL54: if((int)_tmp79 != 1)goto _LL56;_LL55:
return(void*)2;_LL56: if((int)_tmp79 != 2)goto _LL58;_LL57: return(void*)1;_LL58: if(
_tmp79 <= (void*)3?1:*((int*)_tmp79)!= 3)goto _LL5A;_LL59: return(void*)2;_LL5A:;
_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp7C=_cycalloc(
sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Core_Impossible_struct _tmp7D;_tmp7D.tag=
Cyc_Core_Impossible;_tmp7D.f1=({const char*_tmp7E="initlevel_approx";_tag_arr(
_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,17));});_tmp7D;});_tmp7C;}));_LL4D:;}
static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,
void*a,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*
_tmp7F=r;struct Cyc_Dict_Dict*_tmp80;struct Cyc_CfFlowInfo_Place*_tmp81;_LL5D: if(
_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 4)goto _LL5F;_tmp80=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp7F)->f1;_LL5E: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp80,(
void*)2);goto _LL5C;_LL5F: if(_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 2)goto _LL61;
_tmp81=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp7F)->f1;_LL60: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp81))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmp82=_cycalloc(
sizeof(*_tmp82));_tmp82->hd=_tmp81;_tmp82->tl=env->seen;_tmp82;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmp81),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL5C;
_LL61:;_LL62: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL5C:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1?1: acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp83=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmp84;_tmp84.d=d;_tmp84.seen=0;_tmp84;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmp83,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;
struct Cyc_CfFlowInfo_Place*_tmp85=place;_tmp86=*_tmp85;_tmp87=(void*)_tmp86.root;
_tmp88=_tmp86.fields;{void*_tmp89=Cyc_Dict_lookup(d,_tmp87);for(0;_tmp88 != 0;
_tmp88=_tmp88->tl){struct _tuple1 _tmp8B=({struct _tuple1 _tmp8A;_tmp8A.f1=_tmp89;
_tmp8A.f2=(struct _tagged_arr*)_tmp88->hd;_tmp8A;});void*_tmp8C;struct Cyc_Dict_Dict*
_tmp8D;struct _tagged_arr*_tmp8E;_LL64: _tmp8C=_tmp8B.f1;if(_tmp8C <= (void*)3?1:*((
int*)_tmp8C)!= 4)goto _LL66;_tmp8D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp8C)->f1;_tmp8E=_tmp8B.f2;_LL65: _tmp89=((void*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp8D,_tmp8E);goto _LL63;_LL66:;_LL67:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));
_tmp8F[0]=({struct Cyc_Core_Impossible_struct _tmp90;_tmp90.tag=Cyc_Core_Impossible;
_tmp90.f1=({const char*_tmp91="bad lookup_place";_tag_arr(_tmp91,sizeof(char),
_get_zero_arr_size(_tmp91,17));});_tmp90;});_tmp8F;}));_LL63:;}return _tmp89;}}
static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*_tmp92=
rval;struct Cyc_Dict_Dict*_tmp93;_LL69: if(_tmp92 <= (void*)3?1:*((int*)_tmp92)!= 1)
goto _LL6B;_LL6A: return 0;_LL6B: if(_tmp92 <= (void*)3?1:*((int*)_tmp92)!= 4)goto
_LL6D;_tmp93=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp92)->f1;_LL6C: return((
int(*)(int(*f)(int,struct _tagged_arr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((
int(*)(int a,struct _tagged_arr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,0,
_tmp93);_LL6D:;_LL6E: return 1;_LL68:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp94=_new_region("rgn");struct _RegionHandle*rgn=& _tmp94;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp96=
_region_malloc(rgn,sizeof(*_tmp96));_tmp96->rgn=rgn;_tmp96->places=0;_tmp96;});((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp95=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp95;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmp98=({struct _tuple0 _tmp97;
_tmp97.f1=oldval;_tmp97.f2=newval;_tmp97;});void*_tmp99;void*_tmp9A;struct Cyc_CfFlowInfo_Place*
_tmp9B;void*_tmp9C;void*_tmp9D;struct Cyc_Dict_Dict*_tmp9E;void*_tmp9F;struct Cyc_Dict_Dict*
_tmpA0;void*_tmpA1;void*_tmpA2;_LL70: _tmp99=_tmp98.f1;if(_tmp99 <= (void*)3?1:*((
int*)_tmp99)!= 1)goto _LL72;_tmp9A=_tmp98.f2;if(_tmp9A <= (void*)3?1:*((int*)
_tmp9A)!= 2)goto _LL72;_tmp9B=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp9A)->f1;
_LL71: Cyc_CfFlowInfo_add_place(env->pile,_tmp9B);goto _LL73;_LL72: _tmp9C=_tmp98.f1;
if(_tmp9C <= (void*)3?1:*((int*)_tmp9C)!= 1)goto _LL74;_LL73: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmpA3[0]={};Cyc_Tcutil_terr(env->loc,({const
char*_tmpA4="assignment puts possibly-uninitialized data in an escaped location";
_tag_arr(_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,67));}),_tag_arr(_tmpA3,
sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL74: _tmp9D=_tmp98.f1;if(
_tmp9D <= (void*)3?1:*((int*)_tmp9D)!= 4)goto _LL76;_tmp9E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp9D)->f1;_tmp9F=_tmp98.f2;if(_tmp9F <= (void*)3?1:*((int*)_tmp9F)!= 4)goto
_LL76;_tmpA0=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp9F)->f1;_LL75: {struct
Cyc_Dict_Dict*_tmpA5=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AssignEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_AssignEnv*
env,struct _tagged_arr*a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner,
env,_tmp9E,_tmpA0);if(_tmpA5 == _tmp9E)return oldval;if(_tmpA5 == _tmpA0)return
newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpA6=_cycalloc(
sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpA7;_tmpA7.tag=
4;_tmpA7.f1=_tmpA5;_tmpA7;});_tmpA6;});}_LL76: _tmpA1=_tmp98.f2;if(_tmpA1 <= (void*)
3?1:*((int*)_tmpA1)!= 1)goto _LL78;_tmpA2=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmpA1)->f1;_LL77: {void*_tmpA8=_tmpA2;_LL7B: if((int)_tmpA8 != 0)goto _LL7D;_LL7C:
return Cyc_CfFlowInfo_unknown_none;_LL7D: if((int)_tmpA8 != 1)goto _LL7F;_LL7E:
return Cyc_CfFlowInfo_unknown_this;_LL7F: if((int)_tmpA8 != 2)goto _LL7A;_LL80:
return Cyc_CfFlowInfo_unknown_all;_LL7A:;}_LL78:;_LL79: return newval;_LL6F:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple5 _tmpAA=({struct _tuple5 _tmpA9;_tmpA9.f1=fs;
_tmpA9.f2=oldval;_tmpA9;});struct Cyc_List_List*_tmpAB;struct Cyc_List_List _tmpAC;
struct _tagged_arr*_tmpAD;struct Cyc_List_List*_tmpAE;void*_tmpAF;struct Cyc_Dict_Dict*
_tmpB0;_LL82: _tmpAB=_tmpAA.f1;if(_tmpAB == 0)goto _LL84;_tmpAC=*_tmpAB;_tmpAD=(
struct _tagged_arr*)_tmpAC.hd;_tmpAE=_tmpAC.tl;_tmpAF=_tmpAA.f2;if(_tmpAF <= (void*)
3?1:*((int*)_tmpAF)!= 4)goto _LL84;_tmpB0=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpAF)->f1;_LL83: {void*_tmpB1=Cyc_CfFlowInfo_assign_place_outer(env,_tmpAE,((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpB0,
_tmpAD),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpB3;_tmpB3.tag=4;_tmpB3.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpB0,_tmpAD,_tmpB1);_tmpB3;});
_tmpB2;});}_LL84:;_LL85:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Core_Impossible_struct
_tmpB5;_tmpB5.tag=Cyc_Core_Impossible;_tmpB5.f1=({const char*_tmpB6="bad insert place";
_tag_arr(_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,17));});_tmpB5;});_tmpB4;}));
_LL81:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){if(all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,place);{struct
_RegionHandle _tmpB7=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB7;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpB9;void*_tmpBA;struct Cyc_List_List*
_tmpBB;struct Cyc_CfFlowInfo_Place*_tmpB8=place;_tmpB9=*_tmpB8;_tmpBA=(void*)
_tmpB9.root;_tmpBB=_tmpB9.fields;{struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmpBD;_tmpBD.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpBE=_region_malloc(rgn,
sizeof(*_tmpBE));_tmpBE->rgn=rgn;_tmpBE->places=0;_tmpBE;});_tmpBD.d=d;_tmpBD.loc=
loc;_tmpBD;});void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmpBB,Cyc_Dict_lookup(
d,_tmpBA),r);struct Cyc_Dict_Dict*_tmpBC=Cyc_CfFlowInfo_escape_these(env.pile,
all_changed,Cyc_Dict_insert(d,_tmpBA,newval));_npop_handler(0);return _tmpBC;}};
_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*
_tmpBF=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmpC1;void*_tmpC2;
void*_tmpC3;struct Cyc_CfFlowInfo_TagCmp*_tmpC0=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;
_tmpC1=*_tmpC0;_tmpC2=(void*)_tmpC1.cmp;_tmpC3=(void*)_tmpC1.bd;{int found=0;void*
joined_cmp=(void*)10;{struct Cyc_List_List*_tmpC4=l1;for(0;_tmpC4 != 0;_tmpC4=
_tmpC4->tl){struct Cyc_CfFlowInfo_TagCmp _tmpC6;void*_tmpC7;void*_tmpC8;struct Cyc_CfFlowInfo_TagCmp*
_tmpC5=(struct Cyc_CfFlowInfo_TagCmp*)_tmpC4->hd;_tmpC6=*_tmpC5;_tmpC7=(void*)
_tmpC6.cmp;_tmpC8=(void*)_tmpC6.bd;if(Cyc_Tcutil_typecmp(_tmpC3,_tmpC8)== 0){
found=1;if(_tmpC7 == _tmpC2){joined_cmp=_tmpC7;break;}}}}if(found)_tmpBF=({struct
Cyc_List_List*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->cmp=(void*)joined_cmp;_tmpCA->bd=(void*)
_tmpC3;_tmpCA;});_tmpC9->tl=_tmpBF;_tmpC9;});}}return _tmpBF;}}static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpCC=({struct _tuple0 _tmpCB;_tmpCB.f1=r1;_tmpCB.f2=r2;_tmpCB;});
void*_tmpCD;struct Cyc_CfFlowInfo_Place*_tmpCE;void*_tmpCF;struct Cyc_CfFlowInfo_Place*
_tmpD0;void*_tmpD1;struct Cyc_CfFlowInfo_Place*_tmpD2;void*_tmpD3;struct Cyc_CfFlowInfo_Place*
_tmpD4;void*_tmpD5;void*_tmpD6;void*_tmpD7;void*_tmpD8;void*_tmpD9;struct Cyc_Dict_Dict*
_tmpDA;void*_tmpDB;struct Cyc_Dict_Dict*_tmpDC;void*_tmpDD;struct Cyc_List_List*
_tmpDE;void*_tmpDF;struct Cyc_List_List*_tmpE0;void*_tmpE1;void*_tmpE2;_LL87:
_tmpCD=_tmpCC.f1;if(_tmpCD <= (void*)3?1:*((int*)_tmpCD)!= 2)goto _LL89;_tmpCE=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpCD)->f1;_tmpCF=_tmpCC.f2;if(_tmpCF <= (
void*)3?1:*((int*)_tmpCF)!= 2)goto _LL89;_tmpD0=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpCF)->f1;_LL88: if(Cyc_CfFlowInfo_place_cmp(_tmpCE,_tmpD0)== 0)return r1;Cyc_CfFlowInfo_add_place(
env->pile,_tmpCE);Cyc_CfFlowInfo_add_place(env->pile,_tmpD0);goto _LL86;_LL89:
_tmpD1=_tmpCC.f1;if(_tmpD1 <= (void*)3?1:*((int*)_tmpD1)!= 2)goto _LL8B;_tmpD2=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpD1)->f1;_LL8A: Cyc_CfFlowInfo_add_place(
env->pile,_tmpD2);goto _LL86;_LL8B: _tmpD3=_tmpCC.f2;if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 2)goto _LL8D;_tmpD4=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpD3)->f1;
_LL8C: Cyc_CfFlowInfo_add_place(env->pile,_tmpD4);goto _LL86;_LL8D: _tmpD5=_tmpCC.f1;
if((int)_tmpD5 != 1)goto _LL8F;_tmpD6=_tmpCC.f2;if((int)_tmpD6 != 2)goto _LL8F;_LL8E:
goto _LL90;_LL8F: _tmpD7=_tmpCC.f1;if((int)_tmpD7 != 2)goto _LL91;_tmpD8=_tmpCC.f2;
if((int)_tmpD8 != 1)goto _LL91;_LL90: return(void*)2;_LL91: _tmpD9=_tmpCC.f1;if(
_tmpD9 <= (void*)3?1:*((int*)_tmpD9)!= 4)goto _LL93;_tmpDA=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD9)->f1;_tmpDB=_tmpCC.f2;if(_tmpDB <= (void*)3?1:*((int*)_tmpDB)!= 4)goto
_LL93;_tmpDC=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDB)->f1;_LL92: {struct
Cyc_Dict_Dict*_tmpE3=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,env,_tmpDA,
_tmpDC);if(_tmpE3 == _tmpDA)return r1;if(_tmpE3 == _tmpDC)return r2;return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmpE5;_tmpE5.tag=4;_tmpE5.f1=_tmpE3;_tmpE5;});
_tmpE4;});}_LL93: _tmpDD=_tmpCC.f1;if(_tmpDD <= (void*)3?1:*((int*)_tmpDD)!= 3)
goto _LL95;_tmpDE=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmpDD)->f1;_tmpDF=
_tmpCC.f2;if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 3)goto _LL95;_tmpE0=((struct
Cyc_CfFlowInfo_TagCmps_struct*)_tmpDF)->f1;_LL94: {struct Cyc_List_List*_tmpE6=
Cyc_CfFlowInfo_join_tag_cmps(_tmpDE,_tmpE0);if(_tmpE6 == _tmpDE)return r1;return(
void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));
_tmpE7[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmpE8;_tmpE8.tag=3;_tmpE8.f1=
_tmpE6;_tmpE8;});_tmpE7;});}_LL95: _tmpE1=_tmpCC.f1;if(_tmpE1 <= (void*)3?1:*((int*)
_tmpE1)!= 3)goto _LL97;_LL96: return r2;_LL97: _tmpE2=_tmpCC.f2;if(_tmpE2 <= (void*)3?
1:*((int*)_tmpE2)!= 3)goto _LL99;_LL98: return r1;_LL99:;_LL9A: goto _LL86;_LL86:;}{
void*il1=Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(
env->d2,r2);struct _tuple0 _tmpEA=({struct _tuple0 _tmpE9;_tmpE9.f1=r1;_tmpE9.f2=r2;
_tmpE9;});void*_tmpEB;void*_tmpEC;_LL9C: _tmpEB=_tmpEA.f1;if(_tmpEB <= (void*)3?1:*((
int*)_tmpEB)!= 1)goto _LL9E;_LL9D: goto _LL9F;_LL9E: _tmpEC=_tmpEA.f2;if(_tmpEC <= (
void*)3?1:*((int*)_tmpEC)!= 1)goto _LLA0;_LL9F: {struct _tuple0 _tmpEE=({struct
_tuple0 _tmpED;_tmpED.f1=il1;_tmpED.f2=il2;_tmpED;});void*_tmpEF;void*_tmpF0;void*
_tmpF1;void*_tmpF2;_LLA3: _tmpEF=_tmpEE.f2;if((int)_tmpEF != 0)goto _LLA5;_LLA4:
goto _LLA6;_LLA5: _tmpF0=_tmpEE.f1;if((int)_tmpF0 != 0)goto _LLA7;_LLA6: return Cyc_CfFlowInfo_esc_none;
_LLA7: _tmpF1=_tmpEE.f2;if((int)_tmpF1 != 1)goto _LLA9;_LLA8: goto _LLAA;_LLA9: _tmpF2=
_tmpEE.f1;if((int)_tmpF2 != 1)goto _LLAB;_LLAA: return Cyc_CfFlowInfo_esc_this;_LLAB:;
_LLAC: return Cyc_CfFlowInfo_esc_all;_LLA2:;}_LLA0:;_LLA1: {struct _tuple0 _tmpF4=({
struct _tuple0 _tmpF3;_tmpF3.f1=il1;_tmpF3.f2=il2;_tmpF3;});void*_tmpF5;void*
_tmpF6;void*_tmpF7;void*_tmpF8;_LLAE: _tmpF5=_tmpF4.f2;if((int)_tmpF5 != 0)goto
_LLB0;_LLAF: goto _LLB1;_LLB0: _tmpF6=_tmpF4.f1;if((int)_tmpF6 != 0)goto _LLB2;_LLB1:
return Cyc_CfFlowInfo_unknown_none;_LLB2: _tmpF7=_tmpF4.f2;if((int)_tmpF7 != 1)goto
_LLB4;_LLB3: goto _LLB5;_LLB4: _tmpF8=_tmpF4.f1;if((int)_tmpF8 != 1)goto _LLB6;_LLB5:
return Cyc_CfFlowInfo_unknown_this;_LLB6:;_LLB7: return Cyc_CfFlowInfo_unknown_all;
_LLAD:;}_LL9B:;}}static int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmpFA=({struct _tuple0 _tmpF9;_tmpF9.f1=r1;_tmpF9.f2=r2;
_tmpF9;});void*_tmpFB;unsigned int _tmpFC;void*_tmpFD;unsigned int _tmpFE;void*
_tmpFF;struct Cyc_Absyn_Vardecl*_tmp100;void*_tmp101;struct Cyc_Absyn_Vardecl*
_tmp102;void*_tmp103;struct Cyc_Absyn_Vardecl*_tmp104;void*_tmp105;struct Cyc_Absyn_Vardecl*
_tmp106;void*_tmp107;unsigned int _tmp108;void*_tmp109;unsigned int _tmp10A;void*
_tmp10B;struct Cyc_Absyn_Vardecl*_tmp10C;void*_tmp10D;struct Cyc_Absyn_Vardecl*
_tmp10E;_LLB9: _tmpFB=_tmpFA.f1;if(*((int*)_tmpFB)!= 0)goto _LLBB;_tmpFC=((struct
Cyc_CfFlowInfo_EqualConst_struct*)_tmpFB)->f1;_tmpFD=_tmpFA.f2;if(*((int*)_tmpFD)
!= 0)goto _LLBB;_tmpFE=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmpFD)->f1;
_LLBA: return _tmpFC == _tmpFE;_LLBB: _tmpFF=_tmpFA.f1;if(*((int*)_tmpFF)!= 1)goto
_LLBD;_tmp100=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmpFF)->f1;_tmp101=_tmpFA.f2;
if(*((int*)_tmp101)!= 1)goto _LLBD;_tmp102=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp101)->f1;_LLBC: return _tmp100 == _tmp102;_LLBD: _tmp103=_tmpFA.f1;if(*((int*)
_tmp103)!= 2)goto _LLBF;_tmp104=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp103)->f1;
_tmp105=_tmpFA.f2;if(*((int*)_tmp105)!= 2)goto _LLBF;_tmp106=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp105)->f1;_LLBE: return _tmp104 == _tmp106;_LLBF: _tmp107=_tmpFA.f1;if(*((int*)
_tmp107)!= 3)goto _LLC1;_tmp108=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp107)->f1;
_tmp109=_tmpFA.f2;if(*((int*)_tmp109)!= 3)goto _LLC1;_tmp10A=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmp109)->f1;_LLC0: return _tmp108 == _tmp10A;_LLC1: _tmp10B=_tmpFA.f1;if(*((int*)
_tmp10B)!= 4)goto _LLC3;_tmp10C=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp10B)->f1;
_tmp10D=_tmpFA.f2;if(*((int*)_tmp10D)!= 4)goto _LLC3;_tmp10E=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp10D)->f1;_LLC2: return _tmp10C == _tmp10E;_LLC3:;_LLC4: return 0;_LLB8:;}}static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*
r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmp10F=r1s;for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){struct Cyc_CfFlowInfo_Reln*
_tmp110=(struct Cyc_CfFlowInfo_Reln*)_tmp10F->hd;int found=0;{struct Cyc_List_List*
_tmp111=r2s;for(0;_tmp111 != 0;_tmp111=_tmp111->tl){struct Cyc_CfFlowInfo_Reln*
_tmp112=(struct Cyc_CfFlowInfo_Reln*)_tmp111->hd;if(_tmp110 == _tmp112?1:(_tmp110->vd
== _tmp112->vd?Cyc_CfFlowInfo_same_relop((void*)_tmp110->rop,(void*)_tmp112->rop):
0)){res=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=
_tmp110;_tmp113->tl=res;_tmp113;});found=1;break;}}}if(!found)diff=1;}}if(!diff)
res=r1s;return res;}}void*Cyc_CfFlowInfo_join_flow(struct Cyc_Set_Set**all_changed,
void*f1,void*f2){if(f1 == f2)return f1;{struct _tuple0 _tmp115=({struct _tuple0
_tmp114;_tmp114.f1=f1;_tmp114.f2=f2;_tmp114;});void*_tmp116;void*_tmp117;void*
_tmp118;struct Cyc_Dict_Dict*_tmp119;struct Cyc_List_List*_tmp11A;void*_tmp11B;
struct Cyc_Dict_Dict*_tmp11C;struct Cyc_List_List*_tmp11D;_LLC6: _tmp116=_tmp115.f1;
if((int)_tmp116 != 0)goto _LLC8;_LLC7: return f2;_LLC8: _tmp117=_tmp115.f2;if((int)
_tmp117 != 0)goto _LLCA;_LLC9: return f1;_LLCA: _tmp118=_tmp115.f1;if(_tmp118 <= (void*)
1?1:*((int*)_tmp118)!= 0)goto _LLC5;_tmp119=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp118)->f1;_tmp11A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp118)->f2;
_tmp11B=_tmp115.f2;if(_tmp11B <= (void*)1?1:*((int*)_tmp11B)!= 0)goto _LLC5;
_tmp11C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11B)->f1;_tmp11D=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11B)->f2;_LLCB: if(_tmp119 == _tmp11C?
_tmp11A == _tmp11D: 0)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp11E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11E;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp11F=({struct Cyc_CfFlowInfo_JoinEnv
_tmp125;_tmp125.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp126=_region_malloc(rgn,
sizeof(*_tmp126));_tmp126->rgn=rgn;_tmp126->places=0;_tmp126;});_tmp125.d1=
_tmp119;_tmp125.d2=_tmp11C;_tmp125;});struct Cyc_Dict_Dict*_tmp120=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp11F,_tmp119,_tmp11C);struct Cyc_List_List*_tmp121=Cyc_CfFlowInfo_join_relns(
_tmp11A,_tmp11D);void*_tmp124=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp123;_tmp123.tag=0;_tmp123.f1=Cyc_CfFlowInfo_escape_these(_tmp11F.pile,
all_changed,_tmp120);_tmp123.f2=_tmp121;_tmp123;});_tmp122;});_npop_handler(0);
return _tmp124;};_pop_region(rgn);}_LLC5:;}}struct _tuple6{void*f1;void*f2;void*f3;
};struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Set_Set**all_changed,
struct _tuple0 pr1,struct _tuple0 pr2){void*_tmp128;void*_tmp129;struct _tuple0
_tmp127=pr1;_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;{void*_tmp12B;void*_tmp12C;
struct _tuple0 _tmp12A=pr2;_tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;{void*outflow=Cyc_CfFlowInfo_join_flow(
all_changed,_tmp128,_tmp12B);struct _tuple6 _tmp12E=({struct _tuple6 _tmp12D;_tmp12D.f1=
_tmp128;_tmp12D.f2=_tmp12B;_tmp12D.f3=outflow;_tmp12D;});void*_tmp12F;void*
_tmp130;void*_tmp131;struct Cyc_Dict_Dict*_tmp132;void*_tmp133;struct Cyc_Dict_Dict*
_tmp134;void*_tmp135;struct Cyc_Dict_Dict*_tmp136;struct Cyc_List_List*_tmp137;
_LLCD: _tmp12F=_tmp12E.f1;if((int)_tmp12F != 0)goto _LLCF;_LLCE: return({struct
_tuple0 _tmp138;_tmp138.f1=outflow;_tmp138.f2=_tmp12C;_tmp138;});_LLCF: _tmp130=
_tmp12E.f2;if((int)_tmp130 != 0)goto _LLD1;_LLD0: return({struct _tuple0 _tmp139;
_tmp139.f1=outflow;_tmp139.f2=_tmp129;_tmp139;});_LLD1: _tmp131=_tmp12E.f1;if(
_tmp131 <= (void*)1?1:*((int*)_tmp131)!= 0)goto _LLD3;_tmp132=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp131)->f1;_tmp133=_tmp12E.f2;if(_tmp133 <= (void*)1?1:*((int*)_tmp133)!= 0)
goto _LLD3;_tmp134=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f1;
_tmp135=_tmp12E.f3;if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 0)goto _LLD3;
_tmp136=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp135)->f1;_tmp137=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp135)->f2;_LLD2: if(((int(*)(int ignore,void*
r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp129,_tmp12C))return({
struct _tuple0 _tmp13A;_tmp13A.f1=outflow;_tmp13A.f2=_tmp12C;_tmp13A;});if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp12C,
_tmp129))return({struct _tuple0 _tmp13B;_tmp13B.f1=outflow;_tmp13B.f2=_tmp129;
_tmp13B;});{struct _RegionHandle _tmp13C=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp13C;_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp13D=({struct Cyc_CfFlowInfo_JoinEnv
_tmp143;_tmp143.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp144=_region_malloc(rgn,
sizeof(*_tmp144));_tmp144->rgn=rgn;_tmp144->places=0;_tmp144;});_tmp143.d1=
_tmp132;_tmp143.d2=_tmp134;_tmp143;});void*_tmp13E=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp13D,0,_tmp129,
_tmp12C);struct _tuple0 _tmp142=({struct _tuple0 _tmp13F;_tmp13F.f1=(void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp141;_tmp141.tag=0;_tmp141.f1=Cyc_CfFlowInfo_escape_these(
_tmp13D.pile,all_changed,_tmp136);_tmp141.f2=_tmp137;_tmp141;});_tmp140;});
_tmp13F.f2=_tmp13E;_tmp13F;});_npop_handler(0);return _tmp142;};_pop_region(rgn);}
_LLD3:;_LLD4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp145=
_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_Core_Impossible_struct _tmp146;
_tmp146.tag=Cyc_Core_Impossible;_tmp146.f1=({const char*_tmp147="join_flow_and_rval: BottomFL outflow";
_tag_arr(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,37));});_tmp146;});
_tmp145;}));_LLCC:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct
Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?1:((int(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(env->chg2,
env->curr_place);if(changed1?changed2: 0)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmp149=({struct _tuple0 _tmp148;_tmp148.f1=r1;_tmp148.f2=r2;_tmp148;});
void*_tmp14A;struct Cyc_Dict_Dict*_tmp14B;void*_tmp14C;struct Cyc_Dict_Dict*
_tmp14D;_LLD6: _tmp14A=_tmp149.f1;if(_tmp14A <= (void*)3?1:*((int*)_tmp14A)!= 4)
goto _LLD8;_tmp14B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp14A)->f1;_tmp14C=
_tmp149.f2;if(_tmp14C <= (void*)3?1:*((int*)_tmp14C)!= 4)goto _LLD8;_tmp14D=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp14C)->f1;_LLD7: {struct Cyc_Dict_Dict*
_tmp14E=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmp14B,_tmp14D);if(_tmp14E == _tmp14B)return r1;if(_tmp14E == _tmp14D)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp14F=_cycalloc(sizeof(*
_tmp14F));_tmp14F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp150;_tmp150.tag=
4;_tmp150.f1=_tmp14E;_tmp150;});_tmp14F;});}_LLD8:;_LLD9:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151[0]=({
struct Cyc_Core_Impossible_struct _tmp152;_tmp152.tag=Cyc_Core_Impossible;_tmp152.f1=({
const char*_tmp153="after_absRval -- non-aggregates!";_tag_arr(_tmp153,sizeof(
char),_get_zero_arr_size(_tmp153,33));});_tmp152;});_tmp151;}));_LLD5:;}}static
void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,struct
_tagged_arr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp154=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp155=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));
_tmp156->hd=key;_tmp156->tl=0;_tmp156;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp157=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp154;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp155;return _tmp157;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp158;_tmp158.root=(void*)root;
_tmp158.fields=0;_tmp158;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2,struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((void*)0)};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp15A=({struct _tuple0 _tmp159;_tmp159.f1=f1;_tmp159.f2=f2;_tmp159;});void*
_tmp15B;void*_tmp15C;void*_tmp15D;struct Cyc_Dict_Dict*_tmp15E;struct Cyc_List_List*
_tmp15F;void*_tmp160;struct Cyc_Dict_Dict*_tmp161;struct Cyc_List_List*_tmp162;
_LLDB: _tmp15B=_tmp15A.f1;if((int)_tmp15B != 0)goto _LLDD;_LLDC: goto _LLDE;_LLDD:
_tmp15C=_tmp15A.f2;if((int)_tmp15C != 0)goto _LLDF;_LLDE: return(void*)0;_LLDF:
_tmp15D=_tmp15A.f1;if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 0)goto _LLDA;
_tmp15E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp15D)->f1;_tmp15F=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp15D)->f2;_tmp160=_tmp15A.f2;if(_tmp160 <= (
void*)1?1:*((int*)_tmp160)!= 0)goto _LLDA;_tmp161=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp160)->f1;_tmp162=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp160)->f2;
_LLE0: if(_tmp15E == _tmp161?_tmp15F == _tmp162: 0)return f1;{struct _RegionHandle
_tmp163=_new_region("rgn");struct _RegionHandle*rgn=& _tmp163;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp164=({struct Cyc_CfFlowInfo_Place*_tmp16E=
_cycalloc(sizeof(*_tmp16E));_tmp16E->root=(void*)((void*)& dummy_root);_tmp16E->fields=
0;_tmp16E;});struct Cyc_CfFlowInfo_AfterEnv _tmp165=({struct Cyc_CfFlowInfo_AfterEnv
_tmp16B;_tmp16B.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp16C;_tmp16C.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp16D=_region_malloc(rgn,sizeof(*_tmp16D));
_tmp16D->rgn=rgn;_tmp16D->places=0;_tmp16D;});_tmp16C.d1=_tmp15E;_tmp16C.d2=
_tmp161;_tmp16C;});_tmp16B.chg1=chg1;_tmp16B.chg2=chg2;_tmp16B.curr_place=
_tmp164;_tmp16B.last_field_cell=& _tmp164->fields;_tmp16B.changed=Cyc_CfFlowInfo_Neither;
_tmp16B;});struct Cyc_Dict_Dict*_tmp166=((struct Cyc_Dict_Dict*(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp165,_tmp15E,_tmp161);struct Cyc_List_List*_tmp167=Cyc_CfFlowInfo_join_relns(
_tmp15F,_tmp162);void*_tmp16A=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp169;_tmp169.tag=0;_tmp169.f1=Cyc_CfFlowInfo_escape_these((_tmp165.joinenv).pile,
all_changed,_tmp166);_tmp169.f2=_tmp167;_tmp169;});_tmp168;});_npop_handler(0);
return _tmp16A;};_pop_region(rgn);}_LLDA:;}}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp172;void*_tmp173;void*_tmp174;struct Cyc_CfFlowInfo_TagCmp*_tmp171=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp172=*_tmp171;_tmp173=(void*)_tmp172.cmp;_tmp174=(void*)_tmp172.bd;{
struct Cyc_List_List*_tmp175=l1;for(0;_tmp175 != 0;_tmp175=_tmp175->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp177;void*_tmp178;void*_tmp179;struct Cyc_CfFlowInfo_TagCmp*_tmp176=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp175->hd;_tmp177=*_tmp176;_tmp178=(void*)_tmp177.cmp;_tmp179=(void*)_tmp177.bd;
if(_tmp178 == _tmp173?Cyc_Tcutil_typecmp(_tmp179,_tmp174)== 0: 0)break;}if(_tmp175
== 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*
ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp17B=({struct _tuple0
_tmp17A;_tmp17A.f1=r1;_tmp17A.f2=r2;_tmp17A;});void*_tmp17C;struct Cyc_CfFlowInfo_Place*
_tmp17D;void*_tmp17E;struct Cyc_CfFlowInfo_Place*_tmp17F;void*_tmp180;void*
_tmp181;void*_tmp182;struct Cyc_Dict_Dict*_tmp183;void*_tmp184;struct Cyc_Dict_Dict*
_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*_tmp189;void*_tmp18A;void*
_tmp18B;void*_tmp18C;struct Cyc_List_List*_tmp18D;void*_tmp18E;struct Cyc_List_List*
_tmp18F;void*_tmp190;_LLE2: _tmp17C=_tmp17B.f1;if(_tmp17C <= (void*)3?1:*((int*)
_tmp17C)!= 2)goto _LLE4;_tmp17D=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp17C)->f1;
_tmp17E=_tmp17B.f2;if(_tmp17E <= (void*)3?1:*((int*)_tmp17E)!= 2)goto _LLE4;
_tmp17F=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp17E)->f1;_LLE3: return Cyc_CfFlowInfo_place_cmp(
_tmp17D,_tmp17F)== 0;_LLE4: _tmp180=_tmp17B.f1;if(_tmp180 <= (void*)3?1:*((int*)
_tmp180)!= 2)goto _LLE6;_LLE5: goto _LLE7;_LLE6: _tmp181=_tmp17B.f2;if(_tmp181 <= (
void*)3?1:*((int*)_tmp181)!= 2)goto _LLE8;_LLE7: return 0;_LLE8: _tmp182=_tmp17B.f1;
if(_tmp182 <= (void*)3?1:*((int*)_tmp182)!= 4)goto _LLEA;_tmp183=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp182)->f1;_tmp184=_tmp17B.f2;if(_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 4)
goto _LLEA;_tmp185=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp184)->f1;_LLE9:
return _tmp183 == _tmp185?1:((int(*)(int(*f)(struct _tagged_arr*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((int(*)(
struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp183,_tmp185);_LLEA: _tmp186=_tmp17B.f2;if((int)_tmp186 != 2)goto _LLEC;_LLEB:
return r1 == (void*)1;_LLEC: _tmp187=_tmp17B.f2;if((int)_tmp187 != 0)goto _LLEE;_LLED:
goto _LLEF;_LLEE: _tmp188=_tmp17B.f2;if((int)_tmp188 != 1)goto _LLF0;_LLEF: return 0;
_LLF0: _tmp189=_tmp17B.f1;if(_tmp189 <= (void*)3?1:*((int*)_tmp189)!= 1)goto _LLF2;
_tmp18A=_tmp17B.f2;if(_tmp18A <= (void*)3?1:*((int*)_tmp18A)!= 1)goto _LLF2;_LLF1:
goto _LLE1;_LLF2: _tmp18B=_tmp17B.f1;if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 1)
goto _LLF4;_LLF3: return 0;_LLF4: _tmp18C=_tmp17B.f1;if(_tmp18C <= (void*)3?1:*((int*)
_tmp18C)!= 3)goto _LLF6;_tmp18D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp18C)->f1;
_tmp18E=_tmp17B.f2;if(_tmp18E <= (void*)3?1:*((int*)_tmp18E)!= 3)goto _LLF6;
_tmp18F=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp18E)->f1;_LLF5: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp18D,_tmp18F);_LLF6: _tmp190=_tmp17B.f2;if(_tmp190 <= (void*)3?1:*((int*)
_tmp190)!= 3)goto _LLF8;_LLF7: return 0;_LLF8:;_LLF9: goto _LLE1;_LLE1:;}{struct
_tuple0 _tmp192=({struct _tuple0 _tmp191;_tmp191.f1=Cyc_CfFlowInfo_initlevel_approx(
r1);_tmp191.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp191;});void*_tmp193;void*
_tmp194;void*_tmp195;void*_tmp196;void*_tmp197;void*_tmp198;_LLFB: _tmp193=
_tmp192.f1;if((int)_tmp193 != 2)goto _LLFD;_tmp194=_tmp192.f2;if((int)_tmp194 != 2)
goto _LLFD;_LLFC: return 1;_LLFD: _tmp195=_tmp192.f2;if((int)_tmp195 != 0)goto _LLFF;
_LLFE: return 1;_LLFF: _tmp196=_tmp192.f1;if((int)_tmp196 != 0)goto _LL101;_LL100:
return 0;_LL101: _tmp197=_tmp192.f2;if((int)_tmp197 != 1)goto _LL103;_LL102: return 1;
_LL103: _tmp198=_tmp192.f1;if((int)_tmp198 != 1)goto _LLFA;_LL104: return 0;_LLFA:;}}
static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp199=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp19A=r2s;for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp19B=(struct Cyc_CfFlowInfo_Reln*)_tmp19A->hd;if(_tmp199 == _tmp19B?1:(_tmp199->vd
== _tmp19B->vd?Cyc_CfFlowInfo_same_relop((void*)_tmp199->rop,(void*)_tmp19B->rop):
0)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
void*f1,void*f2){if(f1 == f2)return 1;{struct _tuple0 _tmp19D=({struct _tuple0 _tmp19C;
_tmp19C.f1=f1;_tmp19C.f2=f2;_tmp19C;});void*_tmp19E;void*_tmp19F;void*_tmp1A0;
struct Cyc_Dict_Dict*_tmp1A1;struct Cyc_List_List*_tmp1A2;void*_tmp1A3;struct Cyc_Dict_Dict*
_tmp1A4;struct Cyc_List_List*_tmp1A5;_LL106: _tmp19E=_tmp19D.f1;if((int)_tmp19E != 
0)goto _LL108;_LL107: return 1;_LL108: _tmp19F=_tmp19D.f2;if((int)_tmp19F != 0)goto
_LL10A;_LL109: return 0;_LL10A: _tmp1A0=_tmp19D.f1;if(_tmp1A0 <= (void*)1?1:*((int*)
_tmp1A0)!= 0)goto _LL105;_tmp1A1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A0)->f1;_tmp1A2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A0)->f2;
_tmp1A3=_tmp19D.f2;if(_tmp1A3 <= (void*)1?1:*((int*)_tmp1A3)!= 0)goto _LL105;
_tmp1A4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A3)->f1;_tmp1A5=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A3)->f2;_LL10B: if(_tmp1A1 == _tmp1A4?
_tmp1A2 == _tmp1A5: 0)return 1;return Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp1A1,_tmp1A4)?Cyc_CfFlowInfo_relns_approx(_tmp1A2,_tmp1A5): 0;_LL105:;}}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*
v){struct Cyc_List_List*p;int found=0;for(p=rs;!found?p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A6=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp1A6->vd == v){found=1;break;}{
void*_tmp1A7=(void*)_tmp1A6->rop;struct Cyc_Absyn_Vardecl*_tmp1A8;struct Cyc_Absyn_Vardecl*
_tmp1A9;struct Cyc_Absyn_Vardecl*_tmp1AA;_LL10D: if(*((int*)_tmp1A7)!= 1)goto
_LL10F;_tmp1A8=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A7)->f1;_LL10E:
_tmp1A9=_tmp1A8;goto _LL110;_LL10F: if(*((int*)_tmp1A7)!= 2)goto _LL111;_tmp1A9=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A7)->f1;_LL110: _tmp1AA=_tmp1A9;goto
_LL112;_LL111: if(*((int*)_tmp1A7)!= 4)goto _LL113;_tmp1AA=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1A7)->f1;_LL112: if(v == _tmp1AA)found=1;goto _LL10C;_LL113:;_LL114: goto _LL10C;
_LL10C:;}}if(!found)return rs;{struct Cyc_List_List*_tmp1AB=0;for(p=rs;p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1AC=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp1AC->vd
!= v){{void*_tmp1AD=(void*)_tmp1AC->rop;struct Cyc_Absyn_Vardecl*_tmp1AE;struct
Cyc_Absyn_Vardecl*_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B0;_LL116: if(*((int*)
_tmp1AD)!= 1)goto _LL118;_tmp1AE=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1AD)->f1;
_LL117: _tmp1AF=_tmp1AE;goto _LL119;_LL118: if(*((int*)_tmp1AD)!= 2)goto _LL11A;
_tmp1AF=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1AD)->f1;_LL119: _tmp1B0=
_tmp1AF;goto _LL11B;_LL11A: if(*((int*)_tmp1AD)!= 4)goto _LL11C;_tmp1B0=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1AD)->f1;_LL11B: if(v == _tmp1B0)continue;goto _LL115;_LL11C:;_LL11D: goto _LL115;
_LL115:;}_tmp1AB=({struct Cyc_List_List*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));
_tmp1B1->hd=_tmp1AC;_tmp1B1->tl=_tmp1AB;_tmp1B1;});}}return _tmp1AB;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp1B2=(void*)e->r;void*_tmp1B3;struct Cyc_Absyn_Vardecl*_tmp1B4;void*_tmp1B5;
struct Cyc_Absyn_Vardecl*_tmp1B6;void*_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B8;
void*_tmp1B9;struct Cyc_Absyn_Vardecl*_tmp1BA;_LL11F: if(*((int*)_tmp1B2)!= 1)goto
_LL121;_tmp1B3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B2)->f2;if(_tmp1B3 <= (
void*)1?1:*((int*)_tmp1B3)!= 0)goto _LL121;_tmp1B4=((struct Cyc_Absyn_Global_b_struct*)
_tmp1B3)->f1;_LL120: _tmp1B6=_tmp1B4;goto _LL122;_LL121: if(*((int*)_tmp1B2)!= 1)
goto _LL123;_tmp1B5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B2)->f2;if(
_tmp1B5 <= (void*)1?1:*((int*)_tmp1B5)!= 2)goto _LL123;_tmp1B6=((struct Cyc_Absyn_Param_b_struct*)
_tmp1B5)->f1;_LL122: _tmp1B8=_tmp1B6;goto _LL124;_LL123: if(*((int*)_tmp1B2)!= 1)
goto _LL125;_tmp1B7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B2)->f2;if(
_tmp1B7 <= (void*)1?1:*((int*)_tmp1B7)!= 3)goto _LL125;_tmp1B8=((struct Cyc_Absyn_Local_b_struct*)
_tmp1B7)->f1;_LL124: _tmp1BA=_tmp1B8;goto _LL126;_LL125: if(*((int*)_tmp1B2)!= 1)
goto _LL127;_tmp1B9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1B2)->f2;if(
_tmp1B9 <= (void*)1?1:*((int*)_tmp1B9)!= 4)goto _LL127;_tmp1BA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1B9)->f1;_LL126: if(!_tmp1BA->escapes)return Cyc_CfFlowInfo_reln_kill_var(r,
_tmp1BA);goto _LL11E;_LL127:;_LL128: goto _LL11E;_LL11E:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{
void*_tmp1BB=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp1BC;struct Cyc_Absyn_Exp*
_tmp1BD;int _tmp1BE;_LL12A: if(*((int*)_tmp1BB)!= 33)goto _LL12C;_tmp1BC=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp1BB)->f1;_tmp1BD=_tmp1BC.num_elts;_tmp1BE=_tmp1BC.fat_result;
if(_tmp1BE != 1)goto _LL12C;_LL12B: malloc_loop: {void*_tmp1BF=(void*)_tmp1BD->r;
struct Cyc_Absyn_Exp*_tmp1C0;void*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C2;void*
_tmp1C3;struct Cyc_Absyn_Vardecl*_tmp1C4;void*_tmp1C5;struct Cyc_Absyn_Vardecl*
_tmp1C6;void*_tmp1C7;struct Cyc_Absyn_Vardecl*_tmp1C8;_LL12F: if(*((int*)_tmp1BF)
!= 13)goto _LL131;_tmp1C0=((struct Cyc_Absyn_Cast_e_struct*)_tmp1BF)->f2;_LL130:
_tmp1BD=_tmp1C0;goto malloc_loop;_LL131: if(*((int*)_tmp1BF)!= 1)goto _LL133;
_tmp1C1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BF)->f2;if(_tmp1C1 <= (void*)
1?1:*((int*)_tmp1C1)!= 4)goto _LL133;_tmp1C2=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1C1)->f1;_LL132: _tmp1C4=_tmp1C2;goto _LL134;_LL133: if(*((int*)_tmp1BF)!= 1)
goto _LL135;_tmp1C3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BF)->f2;if(
_tmp1C3 <= (void*)1?1:*((int*)_tmp1C3)!= 3)goto _LL135;_tmp1C4=((struct Cyc_Absyn_Local_b_struct*)
_tmp1C3)->f1;_LL134: _tmp1C6=_tmp1C4;goto _LL136;_LL135: if(*((int*)_tmp1BF)!= 1)
goto _LL137;_tmp1C5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BF)->f2;if(
_tmp1C5 <= (void*)1?1:*((int*)_tmp1C5)!= 2)goto _LL137;_tmp1C6=((struct Cyc_Absyn_Param_b_struct*)
_tmp1C5)->f1;_LL136: _tmp1C8=_tmp1C6;goto _LL138;_LL137: if(*((int*)_tmp1BF)!= 1)
goto _LL139;_tmp1C7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1BF)->f2;if(
_tmp1C7 <= (void*)1?1:*((int*)_tmp1C7)!= 0)goto _LL139;_tmp1C8=((struct Cyc_Absyn_Global_b_struct*)
_tmp1C7)->f1;_LL138: if(_tmp1C8->escapes)return r;return({struct Cyc_List_List*
_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->vd=_tmp1C8;_tmp1CA->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));
_tmp1CB[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp1CC;_tmp1CC.tag=4;_tmp1CC.f1=
v;_tmp1CC;});_tmp1CB;}));_tmp1CA;});_tmp1C9->tl=r;_tmp1C9;});_LL139:;_LL13A:
return r;_LL12E:;}_LL12C:;_LL12D: goto _LL129;_LL129:;}{void*_tmp1CD=Cyc_Tcutil_compress((
void*)v->type);_LL13C: if(_tmp1CD <= (void*)3?1:*((int*)_tmp1CD)!= 5)goto _LL13E;
_LL13D: goto _LL13B;_LL13E:;_LL13F: return r;_LL13B:;}loop: {void*_tmp1CE=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp1CF;void*_tmp1D0;int _tmp1D1;void*_tmp1D2;struct Cyc_List_List*
_tmp1D3;struct Cyc_List_List _tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List
_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;void*_tmp1D8;struct Cyc_List_List*_tmp1D9;
struct Cyc_List_List _tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;_LL141: if(*((int*)_tmp1CE)
!= 13)goto _LL143;_tmp1CF=((struct Cyc_Absyn_Cast_e_struct*)_tmp1CE)->f2;_LL142: e=
_tmp1CF;goto loop;_LL143: if(*((int*)_tmp1CE)!= 0)goto _LL145;_tmp1D0=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp1CE)->f1;if(_tmp1D0 <= (void*)1?1:*((int*)
_tmp1D0)!= 2)goto _LL145;_tmp1D1=((struct Cyc_Absyn_Int_c_struct*)_tmp1D0)->f2;
_LL144: return({struct Cyc_List_List*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->vd=v;
_tmp1DD->rop=(void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp1DE=
_cycalloc_atomic(sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp1DF;_tmp1DF.tag=0;_tmp1DF.f1=(unsigned int)_tmp1D1;_tmp1DF;});_tmp1DE;}));
_tmp1DD;});_tmp1DC->tl=r;_tmp1DC;});_LL145: if(*((int*)_tmp1CE)!= 3)goto _LL147;
_tmp1D2=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1CE)->f1;if((int)_tmp1D2 != 
4)goto _LL147;_tmp1D3=((struct Cyc_Absyn_Primop_e_struct*)_tmp1CE)->f2;if(_tmp1D3
== 0)goto _LL147;_tmp1D4=*_tmp1D3;_tmp1D5=_tmp1D4.tl;if(_tmp1D5 == 0)goto _LL147;
_tmp1D6=*_tmp1D5;_tmp1D7=(struct Cyc_Absyn_Exp*)_tmp1D6.hd;_LL146:{void*_tmp1E0=(
void*)_tmp1D7->r;void*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List
_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;_LL14C: if(*((int*)_tmp1E0)!= 3)goto _LL14E;
_tmp1E1=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1E0)->f1;if((int)_tmp1E1 != 
19)goto _LL14E;_tmp1E2=((struct Cyc_Absyn_Primop_e_struct*)_tmp1E0)->f2;if(_tmp1E2
== 0)goto _LL14E;_tmp1E3=*_tmp1E2;_tmp1E4=(struct Cyc_Absyn_Exp*)_tmp1E3.hd;_LL14D:{
void*_tmp1E5=(void*)_tmp1E4->r;void*_tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E7;void*
_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1E9;void*_tmp1EA;struct Cyc_Absyn_Vardecl*
_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1ED;_LL151: if(*((int*)_tmp1E5)
!= 1)goto _LL153;_tmp1E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E5)->f2;if(
_tmp1E6 <= (void*)1?1:*((int*)_tmp1E6)!= 0)goto _LL153;_tmp1E7=((struct Cyc_Absyn_Global_b_struct*)
_tmp1E6)->f1;_LL152: _tmp1E9=_tmp1E7;goto _LL154;_LL153: if(*((int*)_tmp1E5)!= 1)
goto _LL155;_tmp1E8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E5)->f2;if(
_tmp1E8 <= (void*)1?1:*((int*)_tmp1E8)!= 3)goto _LL155;_tmp1E9=((struct Cyc_Absyn_Local_b_struct*)
_tmp1E8)->f1;_LL154: _tmp1EB=_tmp1E9;goto _LL156;_LL155: if(*((int*)_tmp1E5)!= 1)
goto _LL157;_tmp1EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E5)->f2;if(
_tmp1EA <= (void*)1?1:*((int*)_tmp1EA)!= 2)goto _LL157;_tmp1EB=((struct Cyc_Absyn_Param_b_struct*)
_tmp1EA)->f1;_LL156: _tmp1ED=_tmp1EB;goto _LL158;_LL157: if(*((int*)_tmp1E5)!= 1)
goto _LL159;_tmp1EC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E5)->f2;if(
_tmp1EC <= (void*)1?1:*((int*)_tmp1EC)!= 4)goto _LL159;_tmp1ED=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1EC)->f1;_LL158: if(_tmp1ED->escapes)return r;return({struct Cyc_List_List*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->vd=v;_tmp1EF->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessSize_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[
0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp1F1;_tmp1F1.tag=2;_tmp1F1.f1=
_tmp1ED;_tmp1F1;});_tmp1F0;}));_tmp1EF;});_tmp1EE->tl=r;_tmp1EE;});_LL159:;
_LL15A: goto _LL150;_LL150:;}goto _LL14B;_LL14E:;_LL14F: goto _LL14B;_LL14B:;}goto
_LL140;_LL147: if(*((int*)_tmp1CE)!= 3)goto _LL149;_tmp1D8=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1CE)->f1;if((int)_tmp1D8 != 19)goto _LL149;_tmp1D9=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1CE)->f2;if(_tmp1D9 == 0)goto _LL149;_tmp1DA=*_tmp1D9;_tmp1DB=(struct Cyc_Absyn_Exp*)
_tmp1DA.hd;_LL148:{void*_tmp1F2=(void*)_tmp1DB->r;void*_tmp1F3;struct Cyc_Absyn_Vardecl*
_tmp1F4;void*_tmp1F5;struct Cyc_Absyn_Vardecl*_tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Vardecl*
_tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Vardecl*_tmp1FA;_LL15C: if(*((int*)_tmp1F2)
!= 1)goto _LL15E;_tmp1F3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F2)->f2;if(
_tmp1F3 <= (void*)1?1:*((int*)_tmp1F3)!= 0)goto _LL15E;_tmp1F4=((struct Cyc_Absyn_Global_b_struct*)
_tmp1F3)->f1;_LL15D: _tmp1F6=_tmp1F4;goto _LL15F;_LL15E: if(*((int*)_tmp1F2)!= 1)
goto _LL160;_tmp1F5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F2)->f2;if(
_tmp1F5 <= (void*)1?1:*((int*)_tmp1F5)!= 3)goto _LL160;_tmp1F6=((struct Cyc_Absyn_Local_b_struct*)
_tmp1F5)->f1;_LL15F: _tmp1F8=_tmp1F6;goto _LL161;_LL160: if(*((int*)_tmp1F2)!= 1)
goto _LL162;_tmp1F7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F2)->f2;if(
_tmp1F7 <= (void*)1?1:*((int*)_tmp1F7)!= 2)goto _LL162;_tmp1F8=((struct Cyc_Absyn_Param_b_struct*)
_tmp1F7)->f1;_LL161: _tmp1FA=_tmp1F8;goto _LL163;_LL162: if(*((int*)_tmp1F2)!= 1)
goto _LL164;_tmp1F9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F2)->f2;if(
_tmp1F9 <= (void*)1?1:*((int*)_tmp1F9)!= 4)goto _LL164;_tmp1FA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1F9)->f1;_LL163: if(_tmp1FA->escapes)return r;return({struct Cyc_List_List*
_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->vd=v;_tmp1FC->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));
_tmp1FD[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp1FE;_tmp1FE.tag=4;_tmp1FE.f1=
_tmp1FA;_tmp1FE;});_tmp1FD;}));_tmp1FC;});_tmp1FB->tl=r;_tmp1FB;});_LL164:;
_LL165: goto _LL15B;_LL15B:;}goto _LL140;_LL149:;_LL14A: goto _LL140;_LL140:;}return r;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*r,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp1FF=(void*)e1->r;void*_tmp200;
struct Cyc_Absyn_Vardecl*_tmp201;void*_tmp202;struct Cyc_Absyn_Vardecl*_tmp203;
void*_tmp204;struct Cyc_Absyn_Vardecl*_tmp205;void*_tmp206;struct Cyc_Absyn_Vardecl*
_tmp207;_LL167: if(*((int*)_tmp1FF)!= 1)goto _LL169;_tmp200=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp200 <= (void*)1?1:*((int*)_tmp200)!= 0)goto _LL169;_tmp201=((
struct Cyc_Absyn_Global_b_struct*)_tmp200)->f1;_LL168: _tmp203=_tmp201;goto _LL16A;
_LL169: if(*((int*)_tmp1FF)!= 1)goto _LL16B;_tmp202=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp202 <= (void*)1?1:*((int*)_tmp202)!= 2)goto _LL16B;_tmp203=((
struct Cyc_Absyn_Param_b_struct*)_tmp202)->f1;_LL16A: _tmp205=_tmp203;goto _LL16C;
_LL16B: if(*((int*)_tmp1FF)!= 1)goto _LL16D;_tmp204=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp204 <= (void*)1?1:*((int*)_tmp204)!= 3)goto _LL16D;_tmp205=((
struct Cyc_Absyn_Local_b_struct*)_tmp204)->f1;_LL16C: _tmp207=_tmp205;goto _LL16E;
_LL16D: if(*((int*)_tmp1FF)!= 1)goto _LL16F;_tmp206=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp206 <= (void*)1?1:*((int*)_tmp206)!= 4)goto _LL16F;_tmp207=((
struct Cyc_Absyn_Pat_b_struct*)_tmp206)->f1;_LL16E: if(!_tmp207->escapes)return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp207,e2);goto _LL166;_LL16F:;_LL170: goto _LL166;_LL166:;}return r;}static void
Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_String_pa_struct
_tmp20A;_tmp20A.tag=0;_tmp20A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string((
r->vd)->name));{void*_tmp208[1]={& _tmp20A};Cyc_fprintf(Cyc_stderr,({const char*
_tmp209="%s";_tag_arr(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,3));}),
_tag_arr(_tmp208,sizeof(void*),1));}});{void*_tmp20B=(void*)r->rop;unsigned int
_tmp20C;struct Cyc_Absyn_Vardecl*_tmp20D;struct Cyc_Absyn_Vardecl*_tmp20E;
unsigned int _tmp20F;struct Cyc_Absyn_Vardecl*_tmp210;_LL172: if(*((int*)_tmp20B)!= 
0)goto _LL174;_tmp20C=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp20B)->f1;
_LL173:({struct Cyc_Int_pa_struct _tmp213;_tmp213.tag=1;_tmp213.f1=(unsigned int)((
int)_tmp20C);{void*_tmp211[1]={& _tmp213};Cyc_fprintf(Cyc_stderr,({const char*
_tmp212="==%d";_tag_arr(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,5));}),
_tag_arr(_tmp211,sizeof(void*),1));}});goto _LL171;_LL174: if(*((int*)_tmp20B)!= 1)
goto _LL176;_tmp20D=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp20B)->f1;_LL175:({
struct Cyc_String_pa_struct _tmp216;_tmp216.tag=0;_tmp216.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp20D->name));{void*_tmp214[1]={&
_tmp216};Cyc_fprintf(Cyc_stderr,({const char*_tmp215="<%s";_tag_arr(_tmp215,
sizeof(char),_get_zero_arr_size(_tmp215,4));}),_tag_arr(_tmp214,sizeof(void*),1));}});
goto _LL171;_LL176: if(*((int*)_tmp20B)!= 2)goto _LL178;_tmp20E=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp20B)->f1;_LL177:({struct Cyc_String_pa_struct _tmp219;_tmp219.tag=0;_tmp219.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp20E->name));{
void*_tmp217[1]={& _tmp219};Cyc_fprintf(Cyc_stderr,({const char*_tmp218="<%s.size";
_tag_arr(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,9));}),_tag_arr(_tmp217,
sizeof(void*),1));}});goto _LL171;_LL178: if(*((int*)_tmp20B)!= 3)goto _LL17A;
_tmp20F=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp20B)->f1;_LL179:({struct Cyc_Int_pa_struct
_tmp21C;_tmp21C.tag=1;_tmp21C.f1=(unsigned int)((int)_tmp20F);{void*_tmp21A[1]={&
_tmp21C};Cyc_fprintf(Cyc_stderr,({const char*_tmp21B="<%d";_tag_arr(_tmp21B,
sizeof(char),_get_zero_arr_size(_tmp21B,4));}),_tag_arr(_tmp21A,sizeof(void*),1));}});
goto _LL171;_LL17A: if(*((int*)_tmp20B)!= 4)goto _LL171;_tmp210=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp20B)->f1;_LL17B:({struct Cyc_String_pa_struct _tmp21F;_tmp21F.tag=0;_tmp21F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp210->name));{
void*_tmp21D[1]={& _tmp21F};Cyc_fprintf(Cyc_stderr,({const char*_tmp21E="<=%s.size";
_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,10));}),_tag_arr(
_tmp21D,sizeof(void*),1));}});goto _LL171;_LL171:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp220[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp221=",";_tag_arr(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,2));}),
_tag_arr(_tmp220,sizeof(void*),0));});}}static int Cyc_CfFlowInfo_contains_region(
struct Cyc_Absyn_Tvar*rgn,void*t){void*_tmp222=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp223;struct Cyc_Absyn_TunionInfo _tmp224;struct Cyc_List_List*_tmp225;void*
_tmp226;struct Cyc_List_List*_tmp227;struct Cyc_Absyn_AggrInfo _tmp228;struct Cyc_List_List*
_tmp229;struct Cyc_Absyn_TunionFieldInfo _tmp22A;struct Cyc_List_List*_tmp22B;
struct Cyc_List_List*_tmp22C;struct Cyc_Absyn_PtrInfo _tmp22D;void*_tmp22E;struct
Cyc_Absyn_PtrAtts _tmp22F;void*_tmp230;struct Cyc_List_List*_tmp231;struct Cyc_List_List*
_tmp232;struct Cyc_Absyn_ArrayInfo _tmp233;void*_tmp234;void*_tmp235;void*_tmp236;
void*_tmp237;_LL17D: if((int)_tmp222 != 0)goto _LL17F;_LL17E: goto _LL180;_LL17F: if(
_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 5)goto _LL181;_LL180: goto _LL182;_LL181:
if((int)_tmp222 != 1)goto _LL183;_LL182: goto _LL184;_LL183: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 6)goto _LL185;_LL184: goto _LL186;_LL185: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 12)goto _LL187;_LL186: goto _LL188;_LL187: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 13)goto _LL189;_LL188: goto _LL18A;_LL189: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 14)goto _LL18B;_LL18A: goto _LL18C;_LL18B: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 17)goto _LL18D;_LL18C: goto _LL18E;_LL18D: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 18)goto _LL18F;_LL18E: goto _LL190;_LL18F: if((int)_tmp222 != 2)goto
_LL191;_LL190: goto _LL192;_LL191: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 0)
goto _LL193;_LL192: return 0;_LL193: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 1)
goto _LL195;_tmp223=((struct Cyc_Absyn_VarType_struct*)_tmp222)->f1;_LL194: return
Cyc_Absyn_tvar_cmp(_tmp223,rgn)== 0;_LL195: if(_tmp222 <= (void*)3?1:*((int*)
_tmp222)!= 2)goto _LL197;_tmp224=((struct Cyc_Absyn_TunionType_struct*)_tmp222)->f1;
_tmp225=_tmp224.targs;_tmp226=(void*)_tmp224.rgn;_LL196: if(Cyc_CfFlowInfo_contains_region(
rgn,_tmp226))return 1;_tmp227=_tmp225;goto _LL198;_LL197: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 16)goto _LL199;_tmp227=((struct Cyc_Absyn_TypedefType_struct*)
_tmp222)->f2;_LL198: _tmp229=_tmp227;goto _LL19A;_LL199: if(_tmp222 <= (void*)3?1:*((
int*)_tmp222)!= 10)goto _LL19B;_tmp228=((struct Cyc_Absyn_AggrType_struct*)_tmp222)->f1;
_tmp229=_tmp228.targs;_LL19A: _tmp22B=_tmp229;goto _LL19C;_LL19B: if(_tmp222 <= (
void*)3?1:*((int*)_tmp222)!= 3)goto _LL19D;_tmp22A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp222)->f1;_tmp22B=_tmp22A.targs;_LL19C: _tmp22C=_tmp22B;goto _LL19E;_LL19D: if(
_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 20)goto _LL19F;_tmp22C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp222)->f1;_LL19E: return((int(*)(int(*pred)(struct Cyc_Absyn_Tvar*,void*),
struct Cyc_Absyn_Tvar*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp22C);_LL19F: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 4)goto _LL1A1;
_tmp22D=((struct Cyc_Absyn_PointerType_struct*)_tmp222)->f1;_tmp22E=(void*)
_tmp22D.elt_typ;_tmp22F=_tmp22D.ptr_atts;_tmp230=(void*)_tmp22F.rgn;_LL1A0:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp230)?1: Cyc_CfFlowInfo_contains_region(
rgn,_tmp22E);_LL1A1: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 8)goto _LL1A3;
_LL1A2: return 0;_LL1A3: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 9)goto _LL1A5;
_tmp231=((struct Cyc_Absyn_TupleType_struct*)_tmp222)->f1;_LL1A4: for(0;_tmp231 != 
0;_tmp231=_tmp231->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple4*)
_tmp231->hd)).f2))return 1;}return 0;_LL1A5: if(_tmp222 <= (void*)3?1:*((int*)
_tmp222)!= 11)goto _LL1A7;_tmp232=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp222)->f2;
_LL1A6: for(0;_tmp232 != 0;_tmp232=_tmp232->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp232->hd)->type))return 1;}return 0;
_LL1A7: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 7)goto _LL1A9;_tmp233=((struct
Cyc_Absyn_ArrayType_struct*)_tmp222)->f1;_tmp234=(void*)_tmp233.elt_type;_LL1A8:
_tmp235=_tmp234;goto _LL1AA;_LL1A9: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 19)
goto _LL1AB;_tmp235=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp222)->f1;
_LL1AA: _tmp236=_tmp235;goto _LL1AC;_LL1AB: if(_tmp222 <= (void*)3?1:*((int*)_tmp222)
!= 21)goto _LL1AD;_tmp236=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp222)->f1;
_LL1AC: return Cyc_CfFlowInfo_contains_region(rgn,_tmp236);_LL1AD: if(_tmp222 <= (
void*)3?1:*((int*)_tmp222)!= 15)goto _LL17C;_tmp237=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp222)->f1;_LL1AE: return 0;_LL17C:;}struct _tuple7{struct Cyc_Dict_Dict*f1;struct
Cyc_Absyn_Tvar*f2;};static void Cyc_CfFlowInfo_kill_root(struct _tuple7*env,void*
root,void*rval){struct _tuple7 _tmp239;struct Cyc_Dict_Dict*_tmp23A;struct Cyc_Dict_Dict**
_tmp23B;struct Cyc_Absyn_Tvar*_tmp23C;struct _tuple7*_tmp238=env;_tmp239=*_tmp238;
_tmp23A=_tmp239.f1;_tmp23B=(struct Cyc_Dict_Dict**)&(*_tmp238).f1;_tmp23C=_tmp239.f2;{
void*_tmp23D=root;struct Cyc_Absyn_Vardecl*_tmp23E;void*_tmp23F;_LL1B0: if(*((int*)
_tmp23D)!= 0)goto _LL1B2;_tmp23E=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp23D)->f1;
_LL1B1: if(Cyc_CfFlowInfo_contains_region(_tmp23C,(void*)_tmp23E->type))rval=Cyc_CfFlowInfo_typ_to_absrval((
void*)_tmp23E->type,Cyc_CfFlowInfo_unknown_none);*_tmp23B=Cyc_Dict_insert(*
_tmp23B,root,rval);goto _LL1AF;_LL1B2: if(*((int*)_tmp23D)!= 1)goto _LL1B4;_tmp23F=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp23D)->f2;_LL1B3: if(!Cyc_CfFlowInfo_contains_region(
_tmp23C,_tmp23F))*_tmp23B=Cyc_Dict_insert(*_tmp23B,root,rval);goto _LL1AF;_LL1B4:
if(*((int*)_tmp23D)!= 2)goto _LL1AF;_LL1B5: goto _LL1AF;_LL1AF:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_Dict_Dict*fd,void*rgn){struct Cyc_Absyn_Tvar*
rgn_tvar;{void*_tmp240=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp241;
_LL1B7: if(_tmp240 <= (void*)3?1:*((int*)_tmp240)!= 1)goto _LL1B9;_tmp241=((struct
Cyc_Absyn_VarType_struct*)_tmp240)->f1;_LL1B8: rgn_tvar=_tmp241;goto _LL1B6;_LL1B9:;
_LL1BA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp242=_cycalloc(
sizeof(*_tmp242));_tmp242[0]=({struct Cyc_Core_Impossible_struct _tmp243;_tmp243.tag=
Cyc_Core_Impossible;_tmp243.f1=({const char*_tmp244="kill_flowdict_region";
_tag_arr(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,21));});_tmp243;});
_tmp242;}));goto _LL1B6;_LL1B6:;}{struct _tuple7 env=({struct _tuple7 _tmp245;_tmp245.f1=
Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp245.f2=rgn_tvar;_tmp245;});((void(*)(
void(*f)(struct _tuple7*,void*,void*),struct _tuple7*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}}static struct
Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*
rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){void*_tmp246=
f;struct Cyc_Dict_Dict*_tmp247;struct Cyc_List_List*_tmp248;_LL1BC: if((int)_tmp246
!= 0)goto _LL1BE;_LL1BD: return f;_LL1BE: if(_tmp246 <= (void*)1?1:*((int*)_tmp246)!= 
0)goto _LL1BB;_tmp247=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp246)->f1;
_tmp248=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp246)->f2;_LL1BF: {struct
Cyc_Dict_Dict*_tmp249=Cyc_CfFlowInfo_kill_flowdict_region(_tmp247,rgn);struct Cyc_List_List*
_tmp24A=Cyc_CfFlowInfo_kill_relns_region(_tmp248,rgn);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp24C;_tmp24C.tag=0;_tmp24C.f1=_tmp249;_tmp24C.f2=_tmp24A;_tmp24C;});_tmp24B;});}
_LL1BB:;}
