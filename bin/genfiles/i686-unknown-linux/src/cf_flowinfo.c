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
void*hd;struct Cyc_List_List*tl;};void*Cyc_List_hd(struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
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
Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct
Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};char Cyc_CfFlowInfo_IsZero[
11]="\000\000\000\000IsZero\000";struct Cyc_CfFlowInfo_IsZero_struct{char*tag;
struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
13]="\000\000\000\000UnknownZ\000";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*
f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();extern void*Cyc_CfFlowInfo_unknown_none;
extern void*Cyc_CfFlowInfo_unknown_this;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_this;extern void*
Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
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
void*)& Cyc_CfFlowInfo_esc_all_v;static struct Cyc_Set_Set**Cyc_CfFlowInfo_mt_place_set_opt=
0;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){if(Cyc_CfFlowInfo_mt_place_set_opt
== 0)Cyc_CfFlowInfo_mt_place_set_opt=({struct Cyc_Set_Set**_tmp6=_cycalloc(
sizeof(*_tmp6));_tmp6[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*)))Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});
return*((struct Cyc_Set_Set**)_check_null(Cyc_CfFlowInfo_mt_place_set_opt));}int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp8=({struct _tuple0 _tmp7;_tmp7.f1=r1;_tmp7.f2=r2;_tmp7;});void*_tmp9;
struct Cyc_Absyn_Vardecl*_tmpA;void*_tmpB;struct Cyc_Absyn_Vardecl*_tmpC;void*
_tmpD;void*_tmpE;void*_tmpF;struct Cyc_Absyn_Exp*_tmp10;void*_tmp11;struct Cyc_Absyn_Exp*
_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;int _tmp16;void*_tmp17;int _tmp18;_LL1:
_tmp9=_tmp8.f1;if(*((int*)_tmp9)!= 0)goto _LL3;_tmpA=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp9)->f1;_tmpB=_tmp8.f2;if(*((int*)_tmpB)!= 0)goto _LL3;_tmpC=((struct Cyc_CfFlowInfo_VarRoot_struct*)
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
2));}}});}return _tmp31;}}struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*
Cyc_CfFlowInfo_i_typ_to_absrval(int allow_zeroterm,void*t,void*leafval){if(!Cyc_Absyn_is_union_type(
t)){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo _tmp3C;
void*_tmp3D;struct Cyc_Absyn_Tunionfield*_tmp3E;struct Cyc_List_List*_tmp3F;struct
Cyc_Absyn_AggrInfo _tmp40;void*_tmp41;void*_tmp42;struct Cyc_List_List*_tmp43;
struct Cyc_Absyn_ArrayInfo _tmp44;void*_tmp45;struct Cyc_Absyn_Conref*_tmp46;_LL17:
if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 3)goto _LL19;_tmp3C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B)->f1;_tmp3D=(void*)_tmp3C.field_info;if(*((int*)_tmp3D)!= 1)goto _LL19;
_tmp3E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D)->f2;_LL18: if(_tmp3E->typs
== 0)return leafval;_tmp3F=_tmp3E->typs;goto _LL1A;_LL19: if(_tmp3B <= (void*)3?1:*((
int*)_tmp3B)!= 9)goto _LL1B;_tmp3F=((struct Cyc_Absyn_TupleType_struct*)_tmp3B)->f1;
_LL1A: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;
_tmp3F != 0;(_tmp3F=_tmp3F->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_i_typ_to_absrval(
0,(*((struct _tuple4*)_tmp3F->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp48;_tmp48.tag=3;_tmp48.f1=d;_tmp48;});_tmp47;});}_LL1B: if(_tmp3B <= (void*)3?
1:*((int*)_tmp3B)!= 10)goto _LL1D;_tmp40=((struct Cyc_Absyn_AggrType_struct*)
_tmp3B)->f1;_tmp41=(void*)_tmp40.aggr_info;_LL1C: {struct Cyc_Absyn_Aggrdecl*
_tmp49=Cyc_Absyn_get_known_aggrdecl(_tmp41);if(_tmp49->impl == 0)goto _LL16;_tmp43=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49->impl))->fields;goto _LL1E;}
_LL1D: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 11)goto _LL1F;_tmp42=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B)->f1;if((int)_tmp42 != 0)goto _LL1F;
_tmp43=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B)->f2;_LL1E: {struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))
Cyc_Dict_empty)(Cyc_strptrcmp);for(0;_tmp43 != 0;_tmp43=_tmp43->tl){struct Cyc_Absyn_Aggrfield
_tmp4B;struct _tagged_arr*_tmp4C;void*_tmp4D;struct Cyc_Absyn_Aggrfield*_tmp4A=(
struct Cyc_Absyn_Aggrfield*)_tmp43->hd;_tmp4B=*_tmp4A;_tmp4C=_tmp4B.name;_tmp4D=(
void*)_tmp4B.type;if(_get_arr_size(*_tmp4C,sizeof(char))!= 1)d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,_tmp4C,Cyc_CfFlowInfo_i_typ_to_absrval(
0,_tmp4D,leafval));}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4E=
_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp4F;_tmp4F.tag=3;_tmp4F.f1=d;_tmp4F;});_tmp4E;});}_LL1F: if(_tmp3B <= (void*)3?
1:*((int*)_tmp3B)!= 7)goto _LL21;_tmp44=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3B)->f1;_tmp45=(void*)_tmp44.elt_type;_tmp46=_tmp44.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp46))goto _LL21;_LL20:
return(allow_zeroterm?Cyc_Tcutil_bits_only(_tmp45): 0)?Cyc_CfFlowInfo_unknown_all:
leafval;_LL21:;_LL22: goto _LL16;_LL16:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all:
leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Set_Set*set){struct Cyc_CfFlowInfo_Place*
_tmp50=place;struct Cyc_Iter_Iter _tmp51=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(r,set);while(((int(*)(
struct Cyc_Iter_Iter,struct Cyc_CfFlowInfo_Place**))Cyc_Iter_next)(_tmp51,& _tmp50)){
if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp50->root)!= 0)continue;{
struct Cyc_List_List*_tmp52=place->fields;struct Cyc_List_List*_tmp53=_tmp50->fields;
for(0;_tmp52 != 0?_tmp53 != 0: 0;(_tmp52=_tmp52->tl,_tmp53=_tmp53->tl)){if(Cyc_strptrcmp((
struct _tagged_arr*)_tmp52->hd,(struct _tagged_arr*)_tmp53->hd)!= 0)break;}if(
_tmp52 == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmp54=_region_malloc(pile->rgn,sizeof(*_tmp54));_tmp54->hd=place;
_tmp54->tl=pile->places;_tmp54;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*a,void*r){void*_tmp55=r;struct Cyc_CfFlowInfo_Place*_tmp56;struct Cyc_Dict_Dict*
_tmp57;_LL24: if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 2)goto _LL26;_tmp56=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp55)->f1;_LL25: Cyc_CfFlowInfo_add_place(
pile,_tmp56);return;_LL26: if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 3)goto _LL28;
_tmp57=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp55)->f1;_LL27:((void(*)(void(*
f)(struct Cyc_CfFlowInfo_EscPile*,struct _tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((void(*)(struct Cyc_CfFlowInfo_EscPile*
pile,struct _tagged_arr*a,void*r))Cyc_CfFlowInfo_add_places,pile,_tmp57);return;
_LL28:;_LL29: return;_LL23:;}static void*Cyc_CfFlowInfo_insert_place_inner(void*
new_val,void*old_val){void*_tmp58=old_val;struct Cyc_Dict_Dict*_tmp59;_LL2B: if(
_tmp58 <= (void*)3?1:*((int*)_tmp58)!= 3)goto _LL2D;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp58)->f1;_LL2C: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5A=
_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5B;_tmp5B.tag=3;_tmp5B.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp59);_tmp5B;});_tmp5A;});_LL2D:;_LL2E: return new_val;_LL2A:;}struct
_tuple5{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple5 _tmp5D=({struct _tuple5 _tmp5C;_tmp5C.f1=fs;_tmp5C.f2=
old_val;_tmp5C;});struct Cyc_List_List*_tmp5E;struct Cyc_List_List _tmp5F;struct
_tagged_arr*_tmp60;struct Cyc_List_List*_tmp61;void*_tmp62;struct Cyc_Dict_Dict*
_tmp63;_LL30: _tmp5E=_tmp5D.f1;if(_tmp5E == 0)goto _LL32;_tmp5F=*_tmp5E;_tmp60=(
struct _tagged_arr*)_tmp5F.hd;_tmp61=_tmp5F.tl;_tmp62=_tmp5D.f2;if(_tmp62 <= (void*)
3?1:*((int*)_tmp62)!= 3)goto _LL32;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp62)->f1;_LL31: {void*_tmp64=Cyc_CfFlowInfo_insert_place_outer(_tmp61,((void*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp63,_tmp60),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp65=_cycalloc(
sizeof(*_tmp65));_tmp65[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp66;_tmp66.tag=
3;_tmp66.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp63,_tmp60,_tmp64);_tmp66;});_tmp65;});}_LL32:;_LL33:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp67=_cycalloc(sizeof(*
_tmp67));_tmp67[0]=({struct Cyc_Core_Impossible_struct _tmp68;_tmp68.tag=Cyc_Core_Impossible;
_tmp68.f1=({const char*_tmp69="bad insert place";_tag_arr(_tmp69,sizeof(char),
_get_zero_arr_size(_tmp69,17));});_tmp68;});_tmp67;}));_LL2F:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Set_Set**
all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp6A=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;if(
all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,_tmp6A);{void*oldval;void*
newval;{struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(
_tmp6B.handler))_tmp6D=1;if(!_tmp6D){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp6A);;
_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp6F=_tmp6C;_LL35: if(
_tmp6F != Cyc_Dict_Absent)goto _LL37;_LL36: continue;_LL37:;_LL38:(void)_throw(
_tmp6F);_LL34:;}}}{void*_tmp70=Cyc_CfFlowInfo_initlevel(d,oldval);_LL3A: if((int)
_tmp70 != 2)goto _LL3C;_LL3B: newval=Cyc_CfFlowInfo_esc_all;goto _LL39;_LL3C: if((int)
_tmp70 != 1)goto _LL3E;_LL3D: newval=Cyc_CfFlowInfo_esc_this;goto _LL39;_LL3E: if((
int)_tmp70 != 0)goto _LL39;_LL3F: newval=Cyc_CfFlowInfo_esc_none;goto _LL39;_LL39:;}((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp6A->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp6A->fields,Cyc_Dict_lookup(d,(void*)_tmp6A->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static char Cyc_CfFlowInfo_dummy_str_v[1]="";static struct _tagged_arr Cyc_CfFlowInfo_dummy_str={(
void*)((char*)Cyc_CfFlowInfo_dummy_str_v),(void*)((char*)Cyc_CfFlowInfo_dummy_str_v),(
void*)((char*)Cyc_CfFlowInfo_dummy_str_v + 1)};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmp71=r;void*_tmp72;void*_tmp73;_LL41: if(_tmp71 <= (void*)3?1:*((
int*)_tmp71)!= 0)goto _LL43;_tmp72=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp71)->f1;_LL42: return _tmp72;_LL43: if(_tmp71 <= (void*)3?1:*((int*)_tmp71)!= 1)
goto _LL45;_tmp73=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp71)->f1;_LL44:
return _tmp73;_LL45: if((int)_tmp71 != 0)goto _LL47;_LL46: goto _LL48;_LL47: if((int)
_tmp71 != 1)goto _LL49;_LL48: return(void*)2;_LL49: if((int)_tmp71 != 2)goto _LL4B;
_LL4A: return(void*)1;_LL4B:;_LL4C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Core_Impossible_struct
_tmp75;_tmp75.tag=Cyc_Core_Impossible;_tmp75.f1=({const char*_tmp76="initlevel_approx";
_tag_arr(_tmp76,sizeof(char),_get_zero_arr_size(_tmp76,17));});_tmp75;});_tmp74;}));
_LL40:;}static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*a,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*
_tmp77=r;struct Cyc_Dict_Dict*_tmp78;struct Cyc_CfFlowInfo_Place*_tmp79;_LL4E: if(
_tmp77 <= (void*)3?1:*((int*)_tmp77)!= 3)goto _LL50;_tmp78=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp77)->f1;_LL4F: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp78,(
void*)2);goto _LL4D;_LL50: if(_tmp77 <= (void*)3?1:*((int*)_tmp77)!= 2)goto _LL52;
_tmp79=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp77)->f1;_LL51: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp79))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmp7A=_cycalloc(
sizeof(*_tmp7A));_tmp7A->hd=_tmp79;_tmp7A->tl=env->seen;_tmp7A;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmp79),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL4D;
_LL52:;_LL53: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL4D:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1?1: acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp7B=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmp7C;_tmp7C.d=d;_tmp7C.seen=0;_tmp7C;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmp7B,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmp7E;void*_tmp7F;struct Cyc_List_List*_tmp80;
struct Cyc_CfFlowInfo_Place*_tmp7D=place;_tmp7E=*_tmp7D;_tmp7F=(void*)_tmp7E.root;
_tmp80=_tmp7E.fields;{void*_tmp81=Cyc_Dict_lookup(d,_tmp7F);for(0;_tmp80 != 0;
_tmp80=_tmp80->tl){struct _tuple1 _tmp83=({struct _tuple1 _tmp82;_tmp82.f1=_tmp81;
_tmp82.f2=(struct _tagged_arr*)_tmp80->hd;_tmp82;});void*_tmp84;struct Cyc_Dict_Dict*
_tmp85;struct _tagged_arr*_tmp86;_LL55: _tmp84=_tmp83.f1;if(_tmp84 <= (void*)3?1:*((
int*)_tmp84)!= 3)goto _LL57;_tmp85=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp84)->f1;_tmp86=_tmp83.f2;_LL56: _tmp81=((void*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp85,_tmp86);goto _LL54;_LL57:;_LL58:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp87=_cycalloc(sizeof(*_tmp87));
_tmp87[0]=({struct Cyc_Core_Impossible_struct _tmp88;_tmp88.tag=Cyc_Core_Impossible;
_tmp88.f1=({const char*_tmp89="bad lookup_place";_tag_arr(_tmp89,sizeof(char),
_get_zero_arr_size(_tmp89,17));});_tmp88;});_tmp87;}));_LL54:;}return _tmp81;}}
static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*_tmp8A=
rval;struct Cyc_Dict_Dict*_tmp8B;_LL5A: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 1)
goto _LL5C;_LL5B: return 0;_LL5C: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 3)goto
_LL5E;_tmp8B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp8A)->f1;_LL5D: return((
int(*)(int(*f)(int,struct _tagged_arr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((
int(*)(int a,struct _tagged_arr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,0,
_tmp8B);_LL5E:;_LL5F: return 1;_LL59:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp8C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp8E=
_region_malloc(rgn,sizeof(*_tmp8E));_tmp8E->rgn=rgn;_tmp8E->places=0;_tmp8E;});((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp8D=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp8D;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmp90=({struct _tuple0 _tmp8F;
_tmp8F.f1=oldval;_tmp8F.f2=newval;_tmp8F;});void*_tmp91;void*_tmp92;struct Cyc_CfFlowInfo_Place*
_tmp93;void*_tmp94;void*_tmp95;struct Cyc_Dict_Dict*_tmp96;void*_tmp97;struct Cyc_Dict_Dict*
_tmp98;void*_tmp99;void*_tmp9A;_LL61: _tmp91=_tmp90.f1;if(_tmp91 <= (void*)3?1:*((
int*)_tmp91)!= 1)goto _LL63;_tmp92=_tmp90.f2;if(_tmp92 <= (void*)3?1:*((int*)
_tmp92)!= 2)goto _LL63;_tmp93=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp92)->f1;
_LL62: Cyc_CfFlowInfo_add_place(env->pile,_tmp93);goto _LL64;_LL63: _tmp94=_tmp90.f1;
if(_tmp94 <= (void*)3?1:*((int*)_tmp94)!= 1)goto _LL65;_LL64: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmp9B[0]={};Cyc_Tcutil_terr(env->loc,({const
char*_tmp9C="assignment puts possibly-uninitialized data in an escaped location";
_tag_arr(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,67));}),_tag_arr(_tmp9B,
sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL65: _tmp95=_tmp90.f1;if(
_tmp95 <= (void*)3?1:*((int*)_tmp95)!= 3)goto _LL67;_tmp96=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp95)->f1;_tmp97=_tmp90.f2;if(_tmp97 <= (void*)3?1:*((int*)_tmp97)!= 3)goto
_LL67;_tmp98=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp97)->f1;_LL66: {struct
Cyc_Dict_Dict*_tmp9D=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AssignEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_AssignEnv*
env,struct _tagged_arr*a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner,
env,_tmp96,_tmp98);if(_tmp9D == _tmp96)return oldval;if(_tmp9D == _tmp98)return
newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp9E=_cycalloc(
sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp9F;_tmp9F.tag=
3;_tmp9F.f1=_tmp9D;_tmp9F;});_tmp9E;});}_LL67: _tmp99=_tmp90.f2;if(_tmp99 <= (void*)
3?1:*((int*)_tmp99)!= 1)goto _LL69;_tmp9A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp99)->f1;_LL68: {void*_tmpA0=_tmp9A;_LL6C: if((int)_tmpA0 != 0)goto _LL6E;_LL6D:
return Cyc_CfFlowInfo_unknown_none;_LL6E: if((int)_tmpA0 != 1)goto _LL70;_LL6F:
return Cyc_CfFlowInfo_unknown_this;_LL70: if((int)_tmpA0 != 2)goto _LL6B;_LL71:
return Cyc_CfFlowInfo_unknown_all;_LL6B:;}_LL69:;_LL6A: return newval;_LL60:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple5 _tmpA2=({struct _tuple5 _tmpA1;_tmpA1.f1=fs;
_tmpA1.f2=oldval;_tmpA1;});struct Cyc_List_List*_tmpA3;struct Cyc_List_List _tmpA4;
struct _tagged_arr*_tmpA5;struct Cyc_List_List*_tmpA6;void*_tmpA7;struct Cyc_Dict_Dict*
_tmpA8;_LL73: _tmpA3=_tmpA2.f1;if(_tmpA3 == 0)goto _LL75;_tmpA4=*_tmpA3;_tmpA5=(
struct _tagged_arr*)_tmpA4.hd;_tmpA6=_tmpA4.tl;_tmpA7=_tmpA2.f2;if(_tmpA7 <= (void*)
3?1:*((int*)_tmpA7)!= 3)goto _LL75;_tmpA8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA7)->f1;_LL74: {void*_tmpA9=Cyc_CfFlowInfo_assign_place_outer(env,_tmpA6,((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpA8,
_tmpA5),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpAB;_tmpAB.tag=3;_tmpAB.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpA8,_tmpA5,_tmpA9);_tmpAB;});
_tmpAA;});}_LL75:;_LL76:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Core_Impossible_struct
_tmpAD;_tmpAD.tag=Cyc_Core_Impossible;_tmpAD.f1=({const char*_tmpAE="bad insert place";
_tag_arr(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,17));});_tmpAD;});_tmpAC;}));
_LL72:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){if(all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,place);{struct
_RegionHandle _tmpAF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAF;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpB1;void*_tmpB2;struct Cyc_List_List*
_tmpB3;struct Cyc_CfFlowInfo_Place*_tmpB0=place;_tmpB1=*_tmpB0;_tmpB2=(void*)
_tmpB1.root;_tmpB3=_tmpB1.fields;{struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmpB5;_tmpB5.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpB6=_region_malloc(rgn,
sizeof(*_tmpB6));_tmpB6->rgn=rgn;_tmpB6->places=0;_tmpB6;});_tmpB5.d=d;_tmpB5.loc=
loc;_tmpB5;});void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmpB3,Cyc_Dict_lookup(
d,_tmpB2),r);struct Cyc_Dict_Dict*_tmpB4=Cyc_CfFlowInfo_escape_these(env.pile,
all_changed,Cyc_Dict_insert(d,_tmpB2,newval));_npop_handler(0);return _tmpB4;}};
_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpB8=({struct _tuple0 _tmpB7;_tmpB7.f1=r1;_tmpB7.f2=r2;_tmpB7;});
void*_tmpB9;struct Cyc_CfFlowInfo_Place*_tmpBA;void*_tmpBB;struct Cyc_CfFlowInfo_Place*
_tmpBC;void*_tmpBD;struct Cyc_CfFlowInfo_Place*_tmpBE;void*_tmpBF;struct Cyc_CfFlowInfo_Place*
_tmpC0;void*_tmpC1;void*_tmpC2;void*_tmpC3;void*_tmpC4;void*_tmpC5;struct Cyc_Dict_Dict*
_tmpC6;void*_tmpC7;struct Cyc_Dict_Dict*_tmpC8;_LL78: _tmpB9=_tmpB8.f1;if(_tmpB9 <= (
void*)3?1:*((int*)_tmpB9)!= 2)goto _LL7A;_tmpBA=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpB9)->f1;_tmpBB=_tmpB8.f2;if(_tmpBB <= (void*)3?1:*((int*)_tmpBB)!= 2)goto
_LL7A;_tmpBC=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBB)->f1;_LL79: if(Cyc_CfFlowInfo_place_cmp(
_tmpBA,_tmpBC)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmpBA);Cyc_CfFlowInfo_add_place(
env->pile,_tmpBC);goto _LL77;_LL7A: _tmpBD=_tmpB8.f1;if(_tmpBD <= (void*)3?1:*((int*)
_tmpBD)!= 2)goto _LL7C;_tmpBE=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBD)->f1;
_LL7B: Cyc_CfFlowInfo_add_place(env->pile,_tmpBE);goto _LL77;_LL7C: _tmpBF=_tmpB8.f2;
if(_tmpBF <= (void*)3?1:*((int*)_tmpBF)!= 2)goto _LL7E;_tmpC0=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpBF)->f1;_LL7D: Cyc_CfFlowInfo_add_place(env->pile,_tmpC0);goto _LL77;_LL7E:
_tmpC1=_tmpB8.f1;if((int)_tmpC1 != 1)goto _LL80;_tmpC2=_tmpB8.f2;if((int)_tmpC2 != 
2)goto _LL80;_LL7F: goto _LL81;_LL80: _tmpC3=_tmpB8.f1;if((int)_tmpC3 != 2)goto _LL82;
_tmpC4=_tmpB8.f2;if((int)_tmpC4 != 1)goto _LL82;_LL81: return(void*)2;_LL82: _tmpC5=
_tmpB8.f1;if(_tmpC5 <= (void*)3?1:*((int*)_tmpC5)!= 3)goto _LL84;_tmpC6=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpC5)->f1;_tmpC7=_tmpB8.f2;if(_tmpC7 <= (void*)
3?1:*((int*)_tmpC7)!= 3)goto _LL84;_tmpC8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC7)->f1;_LL83: {struct Cyc_Dict_Dict*_tmpC9=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,
env,_tmpC6,_tmpC8);if(_tmpC9 == _tmpC6)return r1;if(_tmpC9 == _tmpC8)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpCA=_cycalloc(sizeof(*
_tmpCA));_tmpCA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpCB;_tmpCB.tag=3;
_tmpCB.f1=_tmpC9;_tmpCB;});_tmpCA;});}_LL84:;_LL85: goto _LL77;_LL77:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmpCD=({struct _tuple0 _tmpCC;_tmpCC.f1=r1;_tmpCC.f2=r2;_tmpCC;});
void*_tmpCE;void*_tmpCF;_LL87: _tmpCE=_tmpCD.f1;if(_tmpCE <= (void*)3?1:*((int*)
_tmpCE)!= 1)goto _LL89;_LL88: goto _LL8A;_LL89: _tmpCF=_tmpCD.f2;if(_tmpCF <= (void*)
3?1:*((int*)_tmpCF)!= 1)goto _LL8B;_LL8A: {struct _tuple0 _tmpD1=({struct _tuple0
_tmpD0;_tmpD0.f1=il1;_tmpD0.f2=il2;_tmpD0;});void*_tmpD2;void*_tmpD3;void*_tmpD4;
void*_tmpD5;_LL8E: _tmpD2=_tmpD1.f2;if((int)_tmpD2 != 0)goto _LL90;_LL8F: goto _LL91;
_LL90: _tmpD3=_tmpD1.f1;if((int)_tmpD3 != 0)goto _LL92;_LL91: return Cyc_CfFlowInfo_esc_none;
_LL92: _tmpD4=_tmpD1.f2;if((int)_tmpD4 != 1)goto _LL94;_LL93: goto _LL95;_LL94: _tmpD5=
_tmpD1.f1;if((int)_tmpD5 != 1)goto _LL96;_LL95: return Cyc_CfFlowInfo_esc_this;_LL96:;
_LL97: return Cyc_CfFlowInfo_esc_all;_LL8D:;}_LL8B:;_LL8C: {struct _tuple0 _tmpD7=({
struct _tuple0 _tmpD6;_tmpD6.f1=il1;_tmpD6.f2=il2;_tmpD6;});void*_tmpD8;void*
_tmpD9;void*_tmpDA;void*_tmpDB;_LL99: _tmpD8=_tmpD7.f2;if((int)_tmpD8 != 0)goto
_LL9B;_LL9A: goto _LL9C;_LL9B: _tmpD9=_tmpD7.f1;if((int)_tmpD9 != 0)goto _LL9D;_LL9C:
return Cyc_CfFlowInfo_unknown_none;_LL9D: _tmpDA=_tmpD7.f2;if((int)_tmpDA != 1)goto
_LL9F;_LL9E: goto _LLA0;_LL9F: _tmpDB=_tmpD7.f1;if((int)_tmpDB != 1)goto _LLA1;_LLA0:
return Cyc_CfFlowInfo_unknown_this;_LLA1:;_LLA2: return Cyc_CfFlowInfo_unknown_all;
_LL98:;}_LL86:;}}int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)return
1;{struct _tuple0 _tmpDD=({struct _tuple0 _tmpDC;_tmpDC.f1=r1;_tmpDC.f2=r2;_tmpDC;});
void*_tmpDE;unsigned int _tmpDF;void*_tmpE0;unsigned int _tmpE1;void*_tmpE2;struct
Cyc_Absyn_Vardecl*_tmpE3;void*_tmpE4;struct Cyc_Absyn_Vardecl*_tmpE5;void*_tmpE6;
struct Cyc_Absyn_Vardecl*_tmpE7;void*_tmpE8;struct Cyc_Absyn_Vardecl*_tmpE9;void*
_tmpEA;unsigned int _tmpEB;void*_tmpEC;unsigned int _tmpED;void*_tmpEE;struct Cyc_Absyn_Vardecl*
_tmpEF;void*_tmpF0;struct Cyc_Absyn_Vardecl*_tmpF1;_LLA4: _tmpDE=_tmpDD.f1;if(*((
int*)_tmpDE)!= 0)goto _LLA6;_tmpDF=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpDE)->f1;_tmpE0=_tmpDD.f2;if(*((int*)_tmpE0)!= 0)goto _LLA6;_tmpE1=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpE0)->f1;_LLA5: return _tmpDF == _tmpE1;_LLA6: _tmpE2=_tmpDD.f1;if(*((int*)_tmpE2)
!= 1)goto _LLA8;_tmpE3=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmpE2)->f1;_tmpE4=
_tmpDD.f2;if(*((int*)_tmpE4)!= 1)goto _LLA8;_tmpE5=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmpE4)->f1;_LLA7: return _tmpE3 == _tmpE5;_LLA8: _tmpE6=_tmpDD.f1;if(*((int*)_tmpE6)
!= 2)goto _LLAA;_tmpE7=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpE6)->f1;_tmpE8=
_tmpDD.f2;if(*((int*)_tmpE8)!= 2)goto _LLAA;_tmpE9=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmpE8)->f1;_LLA9: return _tmpE7 == _tmpE9;_LLAA: _tmpEA=_tmpDD.f1;if(*((int*)_tmpEA)
!= 3)goto _LLAC;_tmpEB=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmpEA)->f1;
_tmpEC=_tmpDD.f2;if(*((int*)_tmpEC)!= 3)goto _LLAC;_tmpED=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmpEC)->f1;_LLAB: return _tmpEB == _tmpED;_LLAC: _tmpEE=_tmpDD.f1;if(*((int*)_tmpEE)
!= 4)goto _LLAE;_tmpEF=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmpEE)->f1;
_tmpF0=_tmpDD.f2;if(*((int*)_tmpF0)!= 4)goto _LLAE;_tmpF1=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmpF0)->f1;_LLAD: return _tmpEF == _tmpF1;_LLAE:;_LLAF: return 0;_LLA3:;}}struct Cyc_List_List*
Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(
r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmpF2=r1s;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){struct Cyc_CfFlowInfo_Reln*_tmpF3=(
struct Cyc_CfFlowInfo_Reln*)_tmpF2->hd;int found=0;{struct Cyc_List_List*_tmpF4=r2s;
for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){struct Cyc_CfFlowInfo_Reln*_tmpF5=(struct Cyc_CfFlowInfo_Reln*)
_tmpF4->hd;if(_tmpF3 == _tmpF5?1:(_tmpF3->vd == _tmpF5->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmpF3->rop,(void*)_tmpF5->rop): 0)){res=({struct Cyc_List_List*_tmpF6=
_cycalloc(sizeof(*_tmpF6));_tmpF6->hd=_tmpF3;_tmpF6->tl=res;_tmpF6;});found=1;
break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;}}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2){if(f1 == f2)return f1;{struct
_tuple0 _tmpF8=({struct _tuple0 _tmpF7;_tmpF7.f1=f1;_tmpF7.f2=f2;_tmpF7;});void*
_tmpF9;void*_tmpFA;void*_tmpFB;struct Cyc_Dict_Dict*_tmpFC;struct Cyc_List_List*
_tmpFD;void*_tmpFE;struct Cyc_Dict_Dict*_tmpFF;struct Cyc_List_List*_tmp100;_LLB1:
_tmpF9=_tmpF8.f1;if((int)_tmpF9 != 0)goto _LLB3;_LLB2: return f2;_LLB3: _tmpFA=_tmpF8.f2;
if((int)_tmpFA != 0)goto _LLB5;_LLB4: return f1;_LLB5: _tmpFB=_tmpF8.f1;if(_tmpFB <= (
void*)1?1:*((int*)_tmpFB)!= 0)goto _LLB0;_tmpFC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFB)->f1;_tmpFD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFB)->f2;_tmpFE=
_tmpF8.f2;if(_tmpFE <= (void*)1?1:*((int*)_tmpFE)!= 0)goto _LLB0;_tmpFF=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmpFE)->f1;_tmp100=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpFE)->f2;_LLB6: if(_tmpFC == _tmpFF?_tmpFD == _tmp100: 0)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp101=_new_region("rgn");struct _RegionHandle*rgn=& _tmp101;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp102=({struct Cyc_CfFlowInfo_JoinEnv
_tmp108;_tmp108.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp109=_region_malloc(rgn,
sizeof(*_tmp109));_tmp109->rgn=rgn;_tmp109->places=0;_tmp109;});_tmp108.d1=
_tmpFC;_tmp108.d2=_tmpFF;_tmp108;});struct Cyc_Dict_Dict*_tmp103=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp102,_tmpFC,_tmpFF);struct Cyc_List_List*_tmp104=Cyc_CfFlowInfo_join_relns(
_tmpFD,_tmp100);void*_tmp107=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp106;_tmp106.tag=0;_tmp106.f1=Cyc_CfFlowInfo_escape_these(_tmp102.pile,
all_changed,_tmp103);_tmp106.f2=_tmp104;_tmp106;});_tmp105;});_npop_handler(0);
return _tmp107;};_pop_region(rgn);}_LLB0:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == 
Cyc_CfFlowInfo_One?1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))
Cyc_Set_member)(env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg2,env->curr_place);if(changed1?changed2: 0)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmp10B=({struct _tuple0 _tmp10A;_tmp10A.f1=r1;_tmp10A.f2=r2;_tmp10A;});
void*_tmp10C;struct Cyc_Dict_Dict*_tmp10D;void*_tmp10E;struct Cyc_Dict_Dict*
_tmp10F;_LLB8: _tmp10C=_tmp10B.f1;if(_tmp10C <= (void*)3?1:*((int*)_tmp10C)!= 3)
goto _LLBA;_tmp10D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10C)->f1;_tmp10E=
_tmp10B.f2;if(_tmp10E <= (void*)3?1:*((int*)_tmp10E)!= 3)goto _LLBA;_tmp10F=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10E)->f1;_LLB9: {struct Cyc_Dict_Dict*
_tmp110=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmp10D,_tmp10F);if(_tmp110 == _tmp10D)return r1;if(_tmp110 == _tmp10F)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp111=_cycalloc(sizeof(*
_tmp111));_tmp111[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp112;_tmp112.tag=
3;_tmp112.f1=_tmp110;_tmp112;});_tmp111;});}_LLBA:;_LLBB:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({
struct Cyc_Core_Impossible_struct _tmp114;_tmp114.tag=Cyc_Core_Impossible;_tmp114.f1=({
const char*_tmp115="after_pathinfo -- non-aggregates!";_tag_arr(_tmp115,sizeof(
char),_get_zero_arr_size(_tmp115,34));});_tmp114;});_tmp113;}));_LLB7:;}}static
void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,struct
_tagged_arr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp116=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp117=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));
_tmp118->hd=key;_tmp118->tl=0;_tmp118;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp119=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp116;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp117;return _tmp119;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp11A;_tmp11A.root=(void*)root;
_tmp11A.fields=0;_tmp11A;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2,struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((void*)0)};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp11C=({struct _tuple0 _tmp11B;_tmp11B.f1=f1;_tmp11B.f2=f2;_tmp11B;});void*
_tmp11D;void*_tmp11E;void*_tmp11F;struct Cyc_Dict_Dict*_tmp120;struct Cyc_List_List*
_tmp121;void*_tmp122;struct Cyc_Dict_Dict*_tmp123;struct Cyc_List_List*_tmp124;
_LLBD: _tmp11D=_tmp11C.f1;if((int)_tmp11D != 0)goto _LLBF;_LLBE: goto _LLC0;_LLBF:
_tmp11E=_tmp11C.f2;if((int)_tmp11E != 0)goto _LLC1;_LLC0: return(void*)0;_LLC1:
_tmp11F=_tmp11C.f1;if(_tmp11F <= (void*)1?1:*((int*)_tmp11F)!= 0)goto _LLBC;
_tmp120=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11F)->f1;_tmp121=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11F)->f2;_tmp122=_tmp11C.f2;if(_tmp122 <= (
void*)1?1:*((int*)_tmp122)!= 0)goto _LLBC;_tmp123=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp122)->f1;_tmp124=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp122)->f2;
_LLC2: if(_tmp120 == _tmp123?_tmp121 == _tmp124: 0)return f1;{struct _RegionHandle
_tmp125=_new_region("rgn");struct _RegionHandle*rgn=& _tmp125;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp126=({struct Cyc_CfFlowInfo_Place*_tmp130=
_cycalloc(sizeof(*_tmp130));_tmp130->root=(void*)((void*)& dummy_root);_tmp130->fields=
0;_tmp130;});struct Cyc_CfFlowInfo_AfterEnv _tmp127=({struct Cyc_CfFlowInfo_AfterEnv
_tmp12D;_tmp12D.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp12E;_tmp12E.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp12F=_region_malloc(rgn,sizeof(*_tmp12F));
_tmp12F->rgn=rgn;_tmp12F->places=0;_tmp12F;});_tmp12E.d1=_tmp120;_tmp12E.d2=
_tmp123;_tmp12E;});_tmp12D.chg1=chg1;_tmp12D.chg2=chg2;_tmp12D.curr_place=
_tmp126;_tmp12D.last_field_cell=& _tmp126->fields;_tmp12D.changed=Cyc_CfFlowInfo_Neither;
_tmp12D;});struct Cyc_Dict_Dict*_tmp128=((struct Cyc_Dict_Dict*(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp127,_tmp120,_tmp123);struct Cyc_List_List*_tmp129=Cyc_CfFlowInfo_join_relns(
_tmp121,_tmp124);void*_tmp12C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp12B;_tmp12B.tag=0;_tmp12B.f1=Cyc_CfFlowInfo_escape_these((_tmp127.joinenv).pile,
all_changed,_tmp128);_tmp12B.f2=_tmp129;_tmp12B;});_tmp12A;});_npop_handler(0);
return _tmp12C;};_pop_region(rgn);}_LLBC:;}}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp134=({struct
_tuple0 _tmp133;_tmp133.f1=r1;_tmp133.f2=r2;_tmp133;});void*_tmp135;struct Cyc_CfFlowInfo_Place*
_tmp136;void*_tmp137;struct Cyc_CfFlowInfo_Place*_tmp138;void*_tmp139;void*
_tmp13A;void*_tmp13B;struct Cyc_Dict_Dict*_tmp13C;void*_tmp13D;struct Cyc_Dict_Dict*
_tmp13E;void*_tmp13F;void*_tmp140;void*_tmp141;void*_tmp142;void*_tmp143;void*
_tmp144;_LLC4: _tmp135=_tmp134.f1;if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 2)
goto _LLC6;_tmp136=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp135)->f1;_tmp137=
_tmp134.f2;if(_tmp137 <= (void*)3?1:*((int*)_tmp137)!= 2)goto _LLC6;_tmp138=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp137)->f1;_LLC5: return Cyc_CfFlowInfo_place_cmp(
_tmp136,_tmp138)== 0;_LLC6: _tmp139=_tmp134.f1;if(_tmp139 <= (void*)3?1:*((int*)
_tmp139)!= 2)goto _LLC8;_LLC7: goto _LLC9;_LLC8: _tmp13A=_tmp134.f2;if(_tmp13A <= (
void*)3?1:*((int*)_tmp13A)!= 2)goto _LLCA;_LLC9: return 0;_LLCA: _tmp13B=_tmp134.f1;
if(_tmp13B <= (void*)3?1:*((int*)_tmp13B)!= 3)goto _LLCC;_tmp13C=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp13B)->f1;_tmp13D=_tmp134.f2;if(_tmp13D <= (void*)3?1:*((int*)_tmp13D)!= 3)
goto _LLCC;_tmp13E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp13D)->f1;_LLCB:
return _tmp13C == _tmp13E?1:((int(*)(int(*f)(struct _tagged_arr*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((int(*)(
struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp13C,_tmp13E);_LLCC: _tmp13F=_tmp134.f2;if((int)_tmp13F != 2)goto _LLCE;_LLCD:
return r1 == (void*)1;_LLCE: _tmp140=_tmp134.f2;if((int)_tmp140 != 0)goto _LLD0;_LLCF:
goto _LLD1;_LLD0: _tmp141=_tmp134.f2;if((int)_tmp141 != 1)goto _LLD2;_LLD1: return 0;
_LLD2: _tmp142=_tmp134.f1;if(_tmp142 <= (void*)3?1:*((int*)_tmp142)!= 1)goto _LLD4;
_tmp143=_tmp134.f2;if(_tmp143 <= (void*)3?1:*((int*)_tmp143)!= 1)goto _LLD4;_LLD3:
goto _LLC3;_LLD4: _tmp144=_tmp134.f1;if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 1)
goto _LLD6;_LLD5: return 0;_LLD6:;_LLD7: goto _LLC3;_LLC3:;}{struct _tuple0 _tmp146=({
struct _tuple0 _tmp145;_tmp145.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp145.f2=
Cyc_CfFlowInfo_initlevel_approx(r2);_tmp145;});void*_tmp147;void*_tmp148;void*
_tmp149;void*_tmp14A;void*_tmp14B;void*_tmp14C;_LLD9: _tmp147=_tmp146.f1;if((int)
_tmp147 != 2)goto _LLDB;_tmp148=_tmp146.f2;if((int)_tmp148 != 2)goto _LLDB;_LLDA:
return 1;_LLDB: _tmp149=_tmp146.f2;if((int)_tmp149 != 0)goto _LLDD;_LLDC: return 1;
_LLDD: _tmp14A=_tmp146.f1;if((int)_tmp14A != 0)goto _LLDF;_LLDE: return 0;_LLDF:
_tmp14B=_tmp146.f2;if((int)_tmp14B != 1)goto _LLE1;_LLE0: return 1;_LLE1: _tmp14C=
_tmp146.f1;if((int)_tmp14C != 1)goto _LLD8;_LLE2: return 0;_LLD8:;}}int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp14D=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp14E=r2s;for(0;_tmp14E != 0;_tmp14E=
_tmp14E->tl){struct Cyc_CfFlowInfo_Reln*_tmp14F=(struct Cyc_CfFlowInfo_Reln*)
_tmp14E->hd;if(_tmp14D == _tmp14F?1:(_tmp14D->vd == _tmp14F->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmp14D->rop,(void*)_tmp14F->rop): 0)){found=1;break;}}}if(!found)return 0;}
return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2)
return 1;{struct _tuple0 _tmp151=({struct _tuple0 _tmp150;_tmp150.f1=f1;_tmp150.f2=f2;
_tmp150;});void*_tmp152;void*_tmp153;void*_tmp154;struct Cyc_Dict_Dict*_tmp155;
struct Cyc_List_List*_tmp156;void*_tmp157;struct Cyc_Dict_Dict*_tmp158;struct Cyc_List_List*
_tmp159;_LLE4: _tmp152=_tmp151.f1;if((int)_tmp152 != 0)goto _LLE6;_LLE5: return 1;
_LLE6: _tmp153=_tmp151.f2;if((int)_tmp153 != 0)goto _LLE8;_LLE7: return 0;_LLE8:
_tmp154=_tmp151.f1;if(_tmp154 <= (void*)1?1:*((int*)_tmp154)!= 0)goto _LLE3;
_tmp155=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp154)->f1;_tmp156=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp154)->f2;_tmp157=_tmp151.f2;if(_tmp157 <= (
void*)1?1:*((int*)_tmp157)!= 0)goto _LLE3;_tmp158=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp157)->f1;_tmp159=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp157)->f2;
_LLE9: if(_tmp155 == _tmp158?_tmp156 == _tmp159: 0)return 1;return Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp155,_tmp158)?Cyc_CfFlowInfo_relns_approx(
_tmp156,_tmp159): 0;_LLE3:;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;!found?p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp15A=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp15A->vd == v){found=1;break;}{void*_tmp15B=(void*)_tmp15A->rop;
struct Cyc_Absyn_Vardecl*_tmp15C;struct Cyc_Absyn_Vardecl*_tmp15D;struct Cyc_Absyn_Vardecl*
_tmp15E;_LLEB: if(*((int*)_tmp15B)!= 1)goto _LLED;_tmp15C=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp15B)->f1;_LLEC: _tmp15D=_tmp15C;goto _LLEE;_LLED: if(*((int*)_tmp15B)!= 2)goto
_LLEF;_tmp15D=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp15B)->f1;_LLEE: _tmp15E=
_tmp15D;goto _LLF0;_LLEF: if(*((int*)_tmp15B)!= 4)goto _LLF1;_tmp15E=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp15B)->f1;_LLF0: if(v == _tmp15E)found=1;goto _LLEA;_LLF1:;_LLF2: goto _LLEA;_LLEA:;}}
if(!found)return rs;{struct Cyc_List_List*_tmp15F=0;for(p=rs;p != 0;p=p->tl){struct
Cyc_CfFlowInfo_Reln*_tmp160=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp160->vd != v){{
void*_tmp161=(void*)_tmp160->rop;struct Cyc_Absyn_Vardecl*_tmp162;struct Cyc_Absyn_Vardecl*
_tmp163;struct Cyc_Absyn_Vardecl*_tmp164;_LLF4: if(*((int*)_tmp161)!= 1)goto _LLF6;
_tmp162=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp161)->f1;_LLF5: _tmp163=
_tmp162;goto _LLF7;_LLF6: if(*((int*)_tmp161)!= 2)goto _LLF8;_tmp163=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp161)->f1;_LLF7: _tmp164=_tmp163;goto _LLF9;_LLF8: if(*((int*)_tmp161)!= 4)goto
_LLFA;_tmp164=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp161)->f1;_LLF9: if(v
== _tmp164)continue;goto _LLF3;_LLFA:;_LLFB: goto _LLF3;_LLF3:;}_tmp15F=({struct Cyc_List_List*
_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->hd=_tmp160;_tmp165->tl=_tmp15F;
_tmp165;});}}return _tmp15F;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp166=(void*)e->r;void*
_tmp167;struct Cyc_Absyn_Vardecl*_tmp168;void*_tmp169;struct Cyc_Absyn_Vardecl*
_tmp16A;void*_tmp16B;struct Cyc_Absyn_Vardecl*_tmp16C;void*_tmp16D;struct Cyc_Absyn_Vardecl*
_tmp16E;_LLFD: if(*((int*)_tmp166)!= 1)goto _LLFF;_tmp167=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp166)->f2;if(_tmp167 <= (void*)1?1:*((int*)_tmp167)!= 0)goto _LLFF;_tmp168=((
struct Cyc_Absyn_Global_b_struct*)_tmp167)->f1;_LLFE: _tmp16A=_tmp168;goto _LL100;
_LLFF: if(*((int*)_tmp166)!= 1)goto _LL101;_tmp169=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp166)->f2;if(_tmp169 <= (void*)1?1:*((int*)_tmp169)!= 2)goto _LL101;_tmp16A=((
struct Cyc_Absyn_Param_b_struct*)_tmp169)->f1;_LL100: _tmp16C=_tmp16A;goto _LL102;
_LL101: if(*((int*)_tmp166)!= 1)goto _LL103;_tmp16B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp166)->f2;if(_tmp16B <= (void*)1?1:*((int*)_tmp16B)!= 3)goto _LL103;_tmp16C=((
struct Cyc_Absyn_Local_b_struct*)_tmp16B)->f1;_LL102: _tmp16E=_tmp16C;goto _LL104;
_LL103: if(*((int*)_tmp166)!= 1)goto _LL105;_tmp16D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp166)->f2;if(_tmp16D <= (void*)1?1:*((int*)_tmp16D)!= 4)goto _LL105;_tmp16E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp16D)->f1;_LL104: if(!_tmp16E->escapes)return Cyc_CfFlowInfo_reln_kill_var(
r,_tmp16E);goto _LLFC;_LL105:;_LL106: goto _LLFC;_LLFC:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{
void*_tmp16F=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp170;struct Cyc_Absyn_Exp*
_tmp171;int _tmp172;_LL108: if(*((int*)_tmp16F)!= 33)goto _LL10A;_tmp170=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp16F)->f1;_tmp171=_tmp170.num_elts;_tmp172=_tmp170.fat_result;
if(_tmp172 != 1)goto _LL10A;_LL109: malloc_loop: {void*_tmp173=(void*)_tmp171->r;
struct Cyc_Absyn_Exp*_tmp174;void*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;void*
_tmp177;struct Cyc_Absyn_Vardecl*_tmp178;void*_tmp179;struct Cyc_Absyn_Vardecl*
_tmp17A;void*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17C;_LL10D: if(*((int*)_tmp173)
!= 13)goto _LL10F;_tmp174=((struct Cyc_Absyn_Cast_e_struct*)_tmp173)->f2;_LL10E:
_tmp171=_tmp174;goto malloc_loop;_LL10F: if(*((int*)_tmp173)!= 1)goto _LL111;
_tmp175=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp173)->f2;if(_tmp175 <= (void*)
1?1:*((int*)_tmp175)!= 4)goto _LL111;_tmp176=((struct Cyc_Absyn_Pat_b_struct*)
_tmp175)->f1;_LL110: _tmp178=_tmp176;goto _LL112;_LL111: if(*((int*)_tmp173)!= 1)
goto _LL113;_tmp177=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp173)->f2;if(
_tmp177 <= (void*)1?1:*((int*)_tmp177)!= 3)goto _LL113;_tmp178=((struct Cyc_Absyn_Local_b_struct*)
_tmp177)->f1;_LL112: _tmp17A=_tmp178;goto _LL114;_LL113: if(*((int*)_tmp173)!= 1)
goto _LL115;_tmp179=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp173)->f2;if(
_tmp179 <= (void*)1?1:*((int*)_tmp179)!= 2)goto _LL115;_tmp17A=((struct Cyc_Absyn_Param_b_struct*)
_tmp179)->f1;_LL114: _tmp17C=_tmp17A;goto _LL116;_LL115: if(*((int*)_tmp173)!= 1)
goto _LL117;_tmp17B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp173)->f2;if(
_tmp17B <= (void*)1?1:*((int*)_tmp17B)!= 0)goto _LL117;_tmp17C=((struct Cyc_Absyn_Global_b_struct*)
_tmp17B)->f1;_LL116: if(_tmp17C->escapes)return r;return({struct Cyc_List_List*
_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->vd=_tmp17C;_tmp17E->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));
_tmp17F[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp180;_tmp180.tag=4;_tmp180.f1=
v;_tmp180;});_tmp17F;}));_tmp17E;});_tmp17D->tl=r;_tmp17D;});_LL117:;_LL118:
return r;_LL10C:;}_LL10A:;_LL10B: goto _LL107;_LL107:;}{void*_tmp181=Cyc_Tcutil_compress((
void*)v->type);_LL11A: if(_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 5)goto _LL11C;
_LL11B: goto _LL119;_LL11C:;_LL11D: return r;_LL119:;}loop: {void*_tmp182=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp183;void*_tmp184;int _tmp185;void*_tmp186;struct Cyc_List_List*
_tmp187;struct Cyc_List_List _tmp188;struct Cyc_List_List*_tmp189;struct Cyc_List_List
_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;void*_tmp18C;struct Cyc_List_List*_tmp18D;
struct Cyc_List_List _tmp18E;struct Cyc_Absyn_Exp*_tmp18F;_LL11F: if(*((int*)_tmp182)
!= 13)goto _LL121;_tmp183=((struct Cyc_Absyn_Cast_e_struct*)_tmp182)->f2;_LL120: e=
_tmp183;goto loop;_LL121: if(*((int*)_tmp182)!= 0)goto _LL123;_tmp184=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp182)->f1;if(_tmp184 <= (void*)1?1:*((int*)
_tmp184)!= 2)goto _LL123;_tmp185=((struct Cyc_Absyn_Int_c_struct*)_tmp184)->f2;
_LL122: return({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->vd=v;
_tmp191->rop=(void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp192=
_cycalloc_atomic(sizeof(*_tmp192));_tmp192[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp193;_tmp193.tag=0;_tmp193.f1=(unsigned int)_tmp185;_tmp193;});_tmp192;}));
_tmp191;});_tmp190->tl=r;_tmp190;});_LL123: if(*((int*)_tmp182)!= 3)goto _LL125;
_tmp186=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp182)->f1;if((int)_tmp186 != 
4)goto _LL125;_tmp187=((struct Cyc_Absyn_Primop_e_struct*)_tmp182)->f2;if(_tmp187
== 0)goto _LL125;_tmp188=*_tmp187;_tmp189=_tmp188.tl;if(_tmp189 == 0)goto _LL125;
_tmp18A=*_tmp189;_tmp18B=(struct Cyc_Absyn_Exp*)_tmp18A.hd;_LL124:{void*_tmp194=(
void*)_tmp18B->r;void*_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_List_List
_tmp197;struct Cyc_Absyn_Exp*_tmp198;_LL12A: if(*((int*)_tmp194)!= 3)goto _LL12C;
_tmp195=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp194)->f1;if((int)_tmp195 != 
19)goto _LL12C;_tmp196=((struct Cyc_Absyn_Primop_e_struct*)_tmp194)->f2;if(_tmp196
== 0)goto _LL12C;_tmp197=*_tmp196;_tmp198=(struct Cyc_Absyn_Exp*)_tmp197.hd;_LL12B:{
void*_tmp199=(void*)_tmp198->r;void*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19B;void*
_tmp19C;struct Cyc_Absyn_Vardecl*_tmp19D;void*_tmp19E;struct Cyc_Absyn_Vardecl*
_tmp19F;void*_tmp1A0;struct Cyc_Absyn_Vardecl*_tmp1A1;_LL12F: if(*((int*)_tmp199)
!= 1)goto _LL131;_tmp19A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp199)->f2;if(
_tmp19A <= (void*)1?1:*((int*)_tmp19A)!= 0)goto _LL131;_tmp19B=((struct Cyc_Absyn_Global_b_struct*)
_tmp19A)->f1;_LL130: _tmp19D=_tmp19B;goto _LL132;_LL131: if(*((int*)_tmp199)!= 1)
goto _LL133;_tmp19C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp199)->f2;if(
_tmp19C <= (void*)1?1:*((int*)_tmp19C)!= 3)goto _LL133;_tmp19D=((struct Cyc_Absyn_Local_b_struct*)
_tmp19C)->f1;_LL132: _tmp19F=_tmp19D;goto _LL134;_LL133: if(*((int*)_tmp199)!= 1)
goto _LL135;_tmp19E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp199)->f2;if(
_tmp19E <= (void*)1?1:*((int*)_tmp19E)!= 2)goto _LL135;_tmp19F=((struct Cyc_Absyn_Param_b_struct*)
_tmp19E)->f1;_LL134: _tmp1A1=_tmp19F;goto _LL136;_LL135: if(*((int*)_tmp199)!= 1)
goto _LL137;_tmp1A0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp199)->f2;if(
_tmp1A0 <= (void*)1?1:*((int*)_tmp1A0)!= 4)goto _LL137;_tmp1A1=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1A0)->f1;_LL136: if(_tmp1A1->escapes)return r;return({struct Cyc_List_List*
_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->vd=v;_tmp1A3->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessSize_struct*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4[
0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp1A5;_tmp1A5.tag=2;_tmp1A5.f1=
_tmp1A1;_tmp1A5;});_tmp1A4;}));_tmp1A3;});_tmp1A2->tl=r;_tmp1A2;});_LL137:;
_LL138: goto _LL12E;_LL12E:;}goto _LL129;_LL12C:;_LL12D: goto _LL129;_LL129:;}goto
_LL11E;_LL125: if(*((int*)_tmp182)!= 3)goto _LL127;_tmp18C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp182)->f1;if((int)_tmp18C != 19)goto _LL127;_tmp18D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp182)->f2;if(_tmp18D == 0)goto _LL127;_tmp18E=*_tmp18D;_tmp18F=(struct Cyc_Absyn_Exp*)
_tmp18E.hd;_LL126:{void*_tmp1A6=(void*)_tmp18F->r;void*_tmp1A7;struct Cyc_Absyn_Vardecl*
_tmp1A8;void*_tmp1A9;struct Cyc_Absyn_Vardecl*_tmp1AA;void*_tmp1AB;struct Cyc_Absyn_Vardecl*
_tmp1AC;void*_tmp1AD;struct Cyc_Absyn_Vardecl*_tmp1AE;_LL13A: if(*((int*)_tmp1A6)
!= 1)goto _LL13C;_tmp1A7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A6)->f2;if(
_tmp1A7 <= (void*)1?1:*((int*)_tmp1A7)!= 0)goto _LL13C;_tmp1A8=((struct Cyc_Absyn_Global_b_struct*)
_tmp1A7)->f1;_LL13B: _tmp1AA=_tmp1A8;goto _LL13D;_LL13C: if(*((int*)_tmp1A6)!= 1)
goto _LL13E;_tmp1A9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A6)->f2;if(
_tmp1A9 <= (void*)1?1:*((int*)_tmp1A9)!= 3)goto _LL13E;_tmp1AA=((struct Cyc_Absyn_Local_b_struct*)
_tmp1A9)->f1;_LL13D: _tmp1AC=_tmp1AA;goto _LL13F;_LL13E: if(*((int*)_tmp1A6)!= 1)
goto _LL140;_tmp1AB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A6)->f2;if(
_tmp1AB <= (void*)1?1:*((int*)_tmp1AB)!= 2)goto _LL140;_tmp1AC=((struct Cyc_Absyn_Param_b_struct*)
_tmp1AB)->f1;_LL13F: _tmp1AE=_tmp1AC;goto _LL141;_LL140: if(*((int*)_tmp1A6)!= 1)
goto _LL142;_tmp1AD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A6)->f2;if(
_tmp1AD <= (void*)1?1:*((int*)_tmp1AD)!= 4)goto _LL142;_tmp1AE=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1AD)->f1;_LL141: if(_tmp1AE->escapes)return r;return({struct Cyc_List_List*
_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->vd=v;_tmp1B0->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));
_tmp1B1[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp1B2;_tmp1B2.tag=4;_tmp1B2.f1=
_tmp1AE;_tmp1B2;});_tmp1B1;}));_tmp1B0;});_tmp1AF->tl=r;_tmp1AF;});_LL142:;
_LL143: goto _LL139;_LL139:;}goto _LL11E;_LL127:;_LL128: goto _LL11E;_LL11E:;}return r;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*r,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp1B3=(void*)e1->r;void*_tmp1B4;
struct Cyc_Absyn_Vardecl*_tmp1B5;void*_tmp1B6;struct Cyc_Absyn_Vardecl*_tmp1B7;
void*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1B9;void*_tmp1BA;struct Cyc_Absyn_Vardecl*
_tmp1BB;_LL145: if(*((int*)_tmp1B3)!= 1)goto _LL147;_tmp1B4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B3)->f2;if(_tmp1B4 <= (void*)1?1:*((int*)_tmp1B4)!= 0)goto _LL147;_tmp1B5=((
struct Cyc_Absyn_Global_b_struct*)_tmp1B4)->f1;_LL146: _tmp1B7=_tmp1B5;goto _LL148;
_LL147: if(*((int*)_tmp1B3)!= 1)goto _LL149;_tmp1B6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B3)->f2;if(_tmp1B6 <= (void*)1?1:*((int*)_tmp1B6)!= 2)goto _LL149;_tmp1B7=((
struct Cyc_Absyn_Param_b_struct*)_tmp1B6)->f1;_LL148: _tmp1B9=_tmp1B7;goto _LL14A;
_LL149: if(*((int*)_tmp1B3)!= 1)goto _LL14B;_tmp1B8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B3)->f2;if(_tmp1B8 <= (void*)1?1:*((int*)_tmp1B8)!= 3)goto _LL14B;_tmp1B9=((
struct Cyc_Absyn_Local_b_struct*)_tmp1B8)->f1;_LL14A: _tmp1BB=_tmp1B9;goto _LL14C;
_LL14B: if(*((int*)_tmp1B3)!= 1)goto _LL14D;_tmp1BA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1B3)->f2;if(_tmp1BA <= (void*)1?1:*((int*)_tmp1BA)!= 4)goto _LL14D;_tmp1BB=((
struct Cyc_Absyn_Pat_b_struct*)_tmp1BA)->f1;_LL14C: if(!_tmp1BB->escapes)return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp1BB,e2);goto _LL144;_LL14D:;_LL14E: goto _LL144;_LL144:;}return r;}void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_String_pa_struct _tmp1BE;_tmp1BE.tag=0;
_tmp1BE.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp1BC[1]={& _tmp1BE};Cyc_fprintf(Cyc_stderr,({const char*_tmp1BD="%s";
_tag_arr(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,3));}),_tag_arr(_tmp1BC,
sizeof(void*),1));}});{void*_tmp1BF=(void*)r->rop;unsigned int _tmp1C0;struct Cyc_Absyn_Vardecl*
_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C2;unsigned int _tmp1C3;struct Cyc_Absyn_Vardecl*
_tmp1C4;_LL150: if(*((int*)_tmp1BF)!= 0)goto _LL152;_tmp1C0=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp1BF)->f1;_LL151:({struct Cyc_Int_pa_struct _tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=(
unsigned int)((int)_tmp1C0);{void*_tmp1C5[1]={& _tmp1C7};Cyc_fprintf(Cyc_stderr,({
const char*_tmp1C6="==%d";_tag_arr(_tmp1C6,sizeof(char),_get_zero_arr_size(
_tmp1C6,5));}),_tag_arr(_tmp1C5,sizeof(void*),1));}});goto _LL14F;_LL152: if(*((
int*)_tmp1BF)!= 1)goto _LL154;_tmp1C1=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp1BF)->f1;_LL153:({struct Cyc_String_pa_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1C1->name));{
void*_tmp1C8[1]={& _tmp1CA};Cyc_fprintf(Cyc_stderr,({const char*_tmp1C9="<%s";
_tag_arr(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,4));}),_tag_arr(_tmp1C8,
sizeof(void*),1));}});goto _LL14F;_LL154: if(*((int*)_tmp1BF)!= 2)goto _LL156;
_tmp1C2=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1BF)->f1;_LL155:({struct Cyc_String_pa_struct
_tmp1CD;_tmp1CD.tag=0;_tmp1CD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1C2->name));{void*_tmp1CB[1]={& _tmp1CD};Cyc_fprintf(Cyc_stderr,({const char*
_tmp1CC="<%s.size";_tag_arr(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,9));}),
_tag_arr(_tmp1CB,sizeof(void*),1));}});goto _LL14F;_LL156: if(*((int*)_tmp1BF)!= 3)
goto _LL158;_tmp1C3=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1BF)->f1;_LL157:({
struct Cyc_Int_pa_struct _tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=(unsigned int)((int)
_tmp1C3);{void*_tmp1CE[1]={& _tmp1D0};Cyc_fprintf(Cyc_stderr,({const char*_tmp1CF="<%d";
_tag_arr(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,4));}),_tag_arr(_tmp1CE,
sizeof(void*),1));}});goto _LL14F;_LL158: if(*((int*)_tmp1BF)!= 4)goto _LL14F;
_tmp1C4=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1BF)->f1;_LL159:({struct
Cyc_String_pa_struct _tmp1D3;_tmp1D3.tag=0;_tmp1D3.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1C4->name));{void*_tmp1D1[1]={& _tmp1D3};
Cyc_fprintf(Cyc_stderr,({const char*_tmp1D2="<=%s.size";_tag_arr(_tmp1D2,sizeof(
char),_get_zero_arr_size(_tmp1D2,10));}),_tag_arr(_tmp1D1,sizeof(void*),1));}});
goto _LL14F;_LL14F:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;
r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl
!= 0)({void*_tmp1D4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1D5=",";
_tag_arr(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,2));}),_tag_arr(_tmp1D4,
sizeof(void*),0));});}}static int Cyc_CfFlowInfo_contains_region(struct Cyc_Absyn_Tvar*
rgn,void*t){void*_tmp1D6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1D7;
struct Cyc_Absyn_TunionInfo _tmp1D8;struct Cyc_List_List*_tmp1D9;void*_tmp1DA;
struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_AggrInfo _tmp1DC;struct Cyc_List_List*
_tmp1DD;struct Cyc_Absyn_TunionFieldInfo _tmp1DE;struct Cyc_List_List*_tmp1DF;
struct Cyc_List_List*_tmp1E0;struct Cyc_Absyn_PtrInfo _tmp1E1;void*_tmp1E2;struct
Cyc_Absyn_PtrAtts _tmp1E3;void*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*
_tmp1E6;struct Cyc_Absyn_ArrayInfo _tmp1E7;void*_tmp1E8;void*_tmp1E9;void*_tmp1EA;
void*_tmp1EB;_LL15B: if((int)_tmp1D6 != 0)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if(
_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 5)goto _LL15F;_LL15E: goto _LL160;_LL15F:
if((int)_tmp1D6 != 1)goto _LL161;_LL160: goto _LL162;_LL161: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 6)goto _LL163;_LL162: goto _LL164;_LL163: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 12)goto _LL165;_LL164: goto _LL166;_LL165: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 13)goto _LL167;_LL166: goto _LL168;_LL167: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 14)goto _LL169;_LL168: goto _LL16A;_LL169: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 17)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 18)goto _LL16D;_LL16C: goto _LL16E;_LL16D: if((int)_tmp1D6 != 2)goto
_LL16F;_LL16E: goto _LL170;_LL16F: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 0)
goto _LL171;_LL170: return 0;_LL171: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 1)
goto _LL173;_tmp1D7=((struct Cyc_Absyn_VarType_struct*)_tmp1D6)->f1;_LL172: return
Cyc_Absyn_tvar_cmp(_tmp1D7,rgn)== 0;_LL173: if(_tmp1D6 <= (void*)3?1:*((int*)
_tmp1D6)!= 2)goto _LL175;_tmp1D8=((struct Cyc_Absyn_TunionType_struct*)_tmp1D6)->f1;
_tmp1D9=_tmp1D8.targs;_tmp1DA=(void*)_tmp1D8.rgn;_LL174: if(Cyc_CfFlowInfo_contains_region(
rgn,_tmp1DA))return 1;_tmp1DB=_tmp1D9;goto _LL176;_LL175: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 16)goto _LL177;_tmp1DB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1D6)->f2;_LL176: _tmp1DD=_tmp1DB;goto _LL178;_LL177: if(_tmp1D6 <= (void*)3?1:*((
int*)_tmp1D6)!= 10)goto _LL179;_tmp1DC=((struct Cyc_Absyn_AggrType_struct*)_tmp1D6)->f1;
_tmp1DD=_tmp1DC.targs;_LL178: _tmp1DF=_tmp1DD;goto _LL17A;_LL179: if(_tmp1D6 <= (
void*)3?1:*((int*)_tmp1D6)!= 3)goto _LL17B;_tmp1DE=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1D6)->f1;_tmp1DF=_tmp1DE.targs;_LL17A: _tmp1E0=_tmp1DF;goto _LL17C;_LL17B: if(
_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 20)goto _LL17D;_tmp1E0=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D6)->f1;_LL17C: return((int(*)(int(*pred)(struct Cyc_Absyn_Tvar*,void*),
struct Cyc_Absyn_Tvar*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp1E0);_LL17D: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 4)goto _LL17F;
_tmp1E1=((struct Cyc_Absyn_PointerType_struct*)_tmp1D6)->f1;_tmp1E2=(void*)
_tmp1E1.elt_typ;_tmp1E3=_tmp1E1.ptr_atts;_tmp1E4=(void*)_tmp1E3.rgn;_LL17E:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp1E4)?1: Cyc_CfFlowInfo_contains_region(
rgn,_tmp1E2);_LL17F: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 8)goto _LL181;
_LL180: return 0;_LL181: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 9)goto _LL183;
_tmp1E5=((struct Cyc_Absyn_TupleType_struct*)_tmp1D6)->f1;_LL182: for(0;_tmp1E5 != 
0;_tmp1E5=_tmp1E5->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple4*)
_tmp1E5->hd)).f2))return 1;}return 0;_LL183: if(_tmp1D6 <= (void*)3?1:*((int*)
_tmp1D6)!= 11)goto _LL185;_tmp1E6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D6)->f2;
_LL184: for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp1E6->hd)->type))return 1;}return 0;
_LL185: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 7)goto _LL187;_tmp1E7=((struct
Cyc_Absyn_ArrayType_struct*)_tmp1D6)->f1;_tmp1E8=(void*)_tmp1E7.elt_type;_LL186:
_tmp1E9=_tmp1E8;goto _LL188;_LL187: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)!= 19)
goto _LL189;_tmp1E9=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1D6)->f1;
_LL188: _tmp1EA=_tmp1E9;goto _LL18A;_LL189: if(_tmp1D6 <= (void*)3?1:*((int*)_tmp1D6)
!= 21)goto _LL18B;_tmp1EA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1D6)->f1;
_LL18A: return Cyc_CfFlowInfo_contains_region(rgn,_tmp1EA);_LL18B: if(_tmp1D6 <= (
void*)3?1:*((int*)_tmp1D6)!= 15)goto _LL15A;_tmp1EB=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1D6)->f1;_LL18C: return 0;_LL15A:;}struct _tuple6{struct Cyc_Dict_Dict*f1;struct
Cyc_Absyn_Tvar*f2;};static void Cyc_CfFlowInfo_kill_root(struct _tuple6*env,void*
root,void*rval){struct _tuple6 _tmp1ED;struct Cyc_Dict_Dict*_tmp1EE;struct Cyc_Dict_Dict**
_tmp1EF;struct Cyc_Absyn_Tvar*_tmp1F0;struct _tuple6*_tmp1EC=env;_tmp1ED=*_tmp1EC;
_tmp1EE=_tmp1ED.f1;_tmp1EF=(struct Cyc_Dict_Dict**)&(*_tmp1EC).f1;_tmp1F0=_tmp1ED.f2;{
void*_tmp1F1=root;struct Cyc_Absyn_Vardecl*_tmp1F2;void*_tmp1F3;_LL18E: if(*((int*)
_tmp1F1)!= 0)goto _LL190;_tmp1F2=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp1F1)->f1;
_LL18F: if(Cyc_CfFlowInfo_contains_region(_tmp1F0,(void*)_tmp1F2->type))rval=Cyc_CfFlowInfo_typ_to_absrval((
void*)_tmp1F2->type,Cyc_CfFlowInfo_unknown_none);*_tmp1EF=Cyc_Dict_insert(*
_tmp1EF,root,rval);goto _LL18D;_LL190: if(*((int*)_tmp1F1)!= 1)goto _LL192;_tmp1F3=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp1F1)->f2;_LL191: if(!Cyc_CfFlowInfo_contains_region(
_tmp1F0,_tmp1F3))*_tmp1EF=Cyc_Dict_insert(*_tmp1EF,root,rval);goto _LL18D;_LL192:
if(*((int*)_tmp1F1)!= 2)goto _LL18D;_LL193: goto _LL18D;_LL18D:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_Dict_Dict*fd,void*rgn){struct Cyc_Absyn_Tvar*
rgn_tvar;{void*_tmp1F4=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp1F5;
_LL195: if(_tmp1F4 <= (void*)3?1:*((int*)_tmp1F4)!= 1)goto _LL197;_tmp1F5=((struct
Cyc_Absyn_VarType_struct*)_tmp1F4)->f1;_LL196: rgn_tvar=_tmp1F5;goto _LL194;_LL197:;
_LL198:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1F6=_cycalloc(
sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Core_Impossible_struct _tmp1F7;_tmp1F7.tag=
Cyc_Core_Impossible;_tmp1F7.f1=({const char*_tmp1F8="kill_flowdict_region";
_tag_arr(_tmp1F8,sizeof(char),_get_zero_arr_size(_tmp1F8,21));});_tmp1F7;});
_tmp1F6;}));goto _LL194;_LL194:;}{struct _tuple6 env=({struct _tuple6 _tmp1F9;_tmp1F9.f1=
Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp1F9.f2=rgn_tvar;_tmp1F9;});((void(*)(
void(*f)(struct _tuple6*,void*,void*),struct _tuple6*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}}static struct
Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*
rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){void*_tmp1FA=
f;struct Cyc_Dict_Dict*_tmp1FB;struct Cyc_List_List*_tmp1FC;_LL19A: if((int)_tmp1FA
!= 0)goto _LL19C;_LL19B: return f;_LL19C: if(_tmp1FA <= (void*)1?1:*((int*)_tmp1FA)!= 
0)goto _LL199;_tmp1FB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1FA)->f1;
_tmp1FC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1FA)->f2;_LL19D: {struct
Cyc_Dict_Dict*_tmp1FD=Cyc_CfFlowInfo_kill_flowdict_region(_tmp1FB,rgn);struct Cyc_List_List*
_tmp1FE=Cyc_CfFlowInfo_kill_relns_region(_tmp1FC,rgn);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp200;_tmp200.tag=0;_tmp200.f1=_tmp1FD;_tmp200.f2=_tmp1FE;_tmp200;});_tmp1FF;});}
_LL199:;}
