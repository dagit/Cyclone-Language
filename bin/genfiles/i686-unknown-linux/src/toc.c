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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
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
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_vfprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);extern
char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter_c(void(*f)(void*,void*),void*env,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(
void*),struct Cyc_List_List*x);struct _tagged_arr Cyc_strconcat(struct _tagged_arr,
struct _tagged_arr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);}
;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(
struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct _tuple1*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr
f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct
_tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);
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
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(
struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct
Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;static struct Cyc_List_List*Cyc_Toc_tuple_types=
0;static int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";
char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*
Cyc_Toc_unimp(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_vfprintf(Cyc_stderr,
fmt,ap);({void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_arr(_tmp0,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct _tagged_arr
fmt,struct _tagged_arr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmp2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp3="\n";_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(
_tmp3,2));}),_tag_arr(_tmp2,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[
16]="\000\000\000\000Match_error\000";static char _tmp4[5]="curr";static struct
_tagged_arr Cyc_Toc_curr_string={_tmp4,_tmp4,_tmp4 + 5};static struct _tagged_arr*
Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp5[4]="tag";static struct
_tagged_arr Cyc_Toc_tag_string={_tmp5,_tmp5,_tmp5 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=&
Cyc_Toc_tag_string;static char _tmp6[14]="_handler_cons";static struct _tagged_arr
Cyc_Toc__handler_cons_string={_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp7[8]="handler";static struct
_tagged_arr Cyc_Toc_handler_string={_tmp7,_tmp7,_tmp7 + 8};static struct _tagged_arr*
Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp8[14]="_RegionHandle";
static struct _tagged_arr Cyc_Toc__RegionHandle_string={_tmp8,_tmp8,_tmp8 + 14};
static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp9[7]="_throw";static struct _tagged_arr Cyc_Toc__throw_str={_tmp9,
_tmp9,_tmp9 + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpB[7]="setjmp";static struct
_tagged_arr Cyc_Toc_setjmp_str={_tmpB,_tmpB,_tmpB + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmpD[14]="_push_handler";
static struct _tagged_arr Cyc_Toc__push_handler_str={_tmpD,_tmpD,_tmpD + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmpF[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={_tmpF,
_tmpF,_tmpF + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp11[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmp11,_tmp11,_tmp11 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
char _tmp13[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp13,_tmp13,_tmp13 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static char _tmp15[12]="_check_null";static struct
_tagged_arr Cyc_Toc__check_null_str={_tmp15,_tmp15,_tmp15 + 12};static struct
_tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char
_tmp17[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp17,_tmp17,_tmp17 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp19[31]="_check_known_subscript_notnull";static struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={
_tmp19,_tmp19,_tmp19 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp1B[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp1B,_tmp1B,
_tmp1B + 25};static struct _tuple1 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static char _tmp1D[12]="_tagged_arr";static
struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1D,_tmp1D,_tmp1D + 12};static struct
_tuple1 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static char
_tmp1F[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={_tmp1F,_tmp1F,
_tmp1F + 9};static struct _tuple1 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static char
_tmp21[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp21,_tmp21,_tmp21 + 14};static struct _tuple1 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static char _tmp23[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp23,_tmp23,_tmp23 + 11};static struct _tuple1
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static char _tmp25[
14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={_tmp25,
_tmp25,_tmp25 + 14};static struct _tuple1 Cyc_Toc__get_arr_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={
0,(void*)((void*)& Cyc_Toc__get_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static char _tmp27[19]="_get_zero_arr_size";static struct _tagged_arr Cyc_Toc__get_zero_arr_size_str={
_tmp27,_tmp27,_tmp27 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp29[17]="_tagged_arr_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={
_tmp29,_tmp29,_tmp29 + 17};static struct _tuple1 Cyc_Toc__tagged_arr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev;static char _tmp2B[15]="_zero_arr_plus";static struct
_tagged_arr Cyc_Toc__zero_arr_plus_str={_tmp2B,_tmp2B,_tmp2B + 15};static struct
_tuple1 Cyc_Toc__zero_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp2D[25]="_tagged_arr_inplace_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={
_tmp2D,_tmp2D,_tmp2D + 25};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static char _tmp2F[23]="_zero_arr_inplace_plus";static struct _tagged_arr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp2F,_tmp2F,_tmp2F + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp31[30]="_tagged_arr_inplace_plus_post";static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={
_tmp31,_tmp31,_tmp31 + 30};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,&
Cyc_Toc__tagged_arr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=&
Cyc_Toc__tagged_arr_inplace_plus_post_ev;static char _tmp33[28]="_zero_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp33,_tmp33,
_tmp33 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp35[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp35,_tmp35,_tmp35 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp37[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={_tmp37,
_tmp37,_tmp37 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char
_tmp39[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp39,_tmp39,_tmp39 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmp3B[18]="_cyccalloc_atomic";static
struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp3B,_tmp3B,_tmp3B + 18};static
struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp3D[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp3D,_tmp3D,_tmp3D + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp3F[15]="_region_calloc";static struct
_tagged_arr Cyc_Toc__region_calloc_str={_tmp3F,_tmp3F,_tmp3F + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmp41[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp41,_tmp41,_tmp41 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp43[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp43,_tmp43,_tmp43 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char
_tmp45[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp45,_tmp45,_tmp45 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp47[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp47,_tmp47,_tmp47 + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char
_tmp49[14]="_reset_region";static struct _tagged_arr Cyc_Toc__reset_region_str={
_tmp49,_tmp49,_tmp49 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp4B[19]="_throw_arraybounds";static struct
_tagged_arr Cyc_Toc__throw_arraybounds_str={_tmp4B,_tmp4B,_tmp4B + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp4D[26]="_tagged_ptr_decrease_size";static struct _tagged_arr Cyc_Toc__tagged_ptr_decrease_size_str={
_tmp4D,_tmp4D,_tmp4D + 26};static struct _tuple1 Cyc_Toc__tagged_ptr_decrease_size_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_ptr_decrease_size_re={1,& Cyc_Toc__tagged_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_ptr_decrease_size_ev={
0,(void*)((void*)& Cyc_Toc__tagged_ptr_decrease_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_ptr_decrease_size_e=& Cyc_Toc__tagged_ptr_decrease_size_ev;
static char _tmp4F[7]="_bogus";static struct _tagged_arr Cyc_Toc__bogus_str={_tmp4F,
_tmp4F,_tmp4F + 7};static struct _tuple1 Cyc_Toc__bogus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__bogus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__bogus_re={1,& Cyc_Toc__bogus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__bogus_ev={0,(void*)((void*)&
Cyc_Toc__bogus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__bogus_e=& Cyc_Toc__bogus_ev;static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_tagged_arr_typ_u={
0,(void*)((void*)0),& Cyc_Toc__tagged_arr_pr};static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_tagged_arr_typ_v={10,{(void*)((void*)& Cyc_Toc_tagged_arr_typ_u),0}};
static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;static struct
Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=Cyc_Absyn_skip_stmt(
0);_tmp53;});return*((struct Cyc_Absyn_Stmt**)_check_null(skip_stmt_opt));}struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp54=(void*)e->r;void*_tmp55;char _tmp56;void*
_tmp57;short _tmp58;void*_tmp59;int _tmp5A;void*_tmp5B;long long _tmp5C;void*_tmp5D;
struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp60;
struct Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_List_List*_tmp63;
_LL1: if(*((int*)_tmp54)!= 0)goto _LL3;_tmp55=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp54)->f1;if(_tmp55 <= (void*)1?1:*((int*)_tmp55)!= 0)goto _LL3;_tmp56=((struct
Cyc_Absyn_Char_c_struct*)_tmp55)->f2;_LL2: return _tmp56 == '\000';_LL3: if(*((int*)
_tmp54)!= 0)goto _LL5;_tmp57=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;
if(_tmp57 <= (void*)1?1:*((int*)_tmp57)!= 1)goto _LL5;_tmp58=((struct Cyc_Absyn_Short_c_struct*)
_tmp57)->f2;_LL4: return _tmp58 == 0;_LL5: if(*((int*)_tmp54)!= 0)goto _LL7;_tmp59=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;if(_tmp59 <= (void*)1?1:*((
int*)_tmp59)!= 2)goto _LL7;_tmp5A=((struct Cyc_Absyn_Int_c_struct*)_tmp59)->f2;
_LL6: return _tmp5A == 0;_LL7: if(*((int*)_tmp54)!= 0)goto _LL9;_tmp5B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;if(_tmp5B <= (void*)1?1:*((int*)
_tmp5B)!= 3)goto _LL9;_tmp5C=((struct Cyc_Absyn_LongLong_c_struct*)_tmp5B)->f2;
_LL8: return _tmp5C == 0;_LL9: if(*((int*)_tmp54)!= 0)goto _LLB;_tmp5D=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp54)->f1;if((int)_tmp5D != 0)goto _LLB;_LLA:
return 1;_LLB: if(*((int*)_tmp54)!= 13)goto _LLD;_tmp5E=((struct Cyc_Absyn_Cast_e_struct*)
_tmp54)->f2;_LLC: return Cyc_Toc_is_zero(_tmp5E);_LLD: if(*((int*)_tmp54)!= 24)goto
_LLF;_tmp5F=((struct Cyc_Absyn_Tuple_e_struct*)_tmp54)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp5F);_LLF: if(*((int*)_tmp54)!= 26)goto _LL11;_tmp60=((struct Cyc_Absyn_Array_e_struct*)
_tmp54)->f1;_LL10: _tmp61=_tmp60;goto _LL12;_LL11: if(*((int*)_tmp54)!= 28)goto
_LL13;_tmp61=((struct Cyc_Absyn_Struct_e_struct*)_tmp54)->f3;_LL12: _tmp62=_tmp61;
goto _LL14;_LL13: if(*((int*)_tmp54)!= 25)goto _LL15;_tmp62=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp54)->f2;_LL14: _tmp63=_tmp62;goto _LL16;_LL15: if(*((int*)_tmp54)!= 34)goto
_LL17;_tmp63=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp54)->f2;_LL16: for(0;
_tmp63 != 0;_tmp63=_tmp63->tl){if(!Cyc_Toc_is_zero((*((struct _tuple6*)_tmp63->hd)).f2))
return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp64=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp65;struct Cyc_Absyn_PtrAtts
_tmp66;struct Cyc_Absyn_Conref*_tmp67;_LL1A: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)
!= 4)goto _LL1C;_tmp65=((struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp66=
_tmp65.ptr_atts;_tmp67=_tmp66.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp67);_LL1C:;_LL1D:({void*_tmp68[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp69="is_nullable";
_tag_arr(_tmp69,sizeof(char),_get_zero_arr_size(_tmp69,12));}),_tag_arr(_tmp68,
sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _tagged_arr tag){return({struct _tuple1*_tmp6A=_cycalloc(
sizeof(*_tmp6A));_tmp6A->f1=(*x).f1;_tmp6A->f2=({struct _tagged_arr*_tmp6B=
_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=(struct _tagged_arr)Cyc_strconcat((struct
_tagged_arr)*(*x).f2,(struct _tagged_arr)tag);_tmp6B;});_tmp6A;});}struct _tuple7{
struct _tagged_arr*f1;struct Cyc_List_List*f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp6C=Cyc_Toc_tuple_types;for(0;
_tmp6C != 0;_tmp6C=_tmp6C->tl){struct _tuple7 _tmp6E;struct _tagged_arr*_tmp6F;
struct Cyc_List_List*_tmp70;struct _tuple7*_tmp6D=(struct _tuple7*)_tmp6C->hd;
_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;_tmp70=_tmp6E.f2;{struct Cyc_List_List*_tmp71=
tqs0;for(0;_tmp71 != 0?_tmp70 != 0: 0;(_tmp71=_tmp71->tl,_tmp70=_tmp70->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)_tmp71->hd)).f2,(void*)_tmp70->hd))break;}
if(_tmp71 == 0?_tmp70 == 0: 0)return _tmp6F;}}}{struct _tagged_arr*x=({struct
_tagged_arr*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp81;_tmp81.tag=1;_tmp81.f1=(unsigned int)Cyc_Toc_tuple_type_counter
++;{void*_tmp7F[1]={& _tmp81};Cyc_aprintf(({const char*_tmp80="_tuple%d";_tag_arr(
_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,9));}),_tag_arr(_tmp7F,sizeof(void*),
1));}});_tmp7E;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp72=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(
ts2=ts2->tl,i ++)){_tmp72=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));
_tmp73->hd=({struct Cyc_Absyn_Aggrfield*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->name=
Cyc_Absyn_fieldname(i);_tmp74->tq=Cyc_Toc_mt_tq;_tmp74->type=(void*)((void*)ts2->hd);
_tmp74->width=0;_tmp74->attributes=0;_tmp74;});_tmp73->tl=_tmp72;_tmp73;});}}
_tmp72=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72);{
struct Cyc_Absyn_Aggrdecl*_tmp75=({struct Cyc_Absyn_Aggrdecl*_tmp7B=_cycalloc(
sizeof(*_tmp7B));_tmp7B->kind=(void*)((void*)0);_tmp7B->sc=(void*)((void*)2);
_tmp7B->name=({struct _tuple1*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=Cyc_Absyn_rel_ns_null;
_tmp7D->f2=x;_tmp7D;});_tmp7B->tvs=0;_tmp7B->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->exist_vars=0;_tmp7C->rgn_po=0;_tmp7C->fields=
_tmp72;_tmp7C;});_tmp7B->attributes=0;_tmp7B;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp78;_tmp78.tag=4;_tmp78.f1=_tmp75;_tmp78;});_tmp77;}),0);_tmp76->tl=Cyc_Toc_result_decls;
_tmp76;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp79=_cycalloc(sizeof(*
_tmp79));_tmp79->hd=({struct _tuple7*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=
x;_tmp7A->f2=ts;_tmp7A;});_tmp79->tl=Cyc_Toc_tuple_types;_tmp79;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp82=_cycalloc(sizeof(*
_tmp82));_tmp82->f1=(void*)0;_tmp82->f2=({struct _tagged_arr*_tmp83=_cycalloc(
sizeof(*_tmp83));_tmp83[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp86;
_tmp86.tag=1;_tmp86.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*_tmp84[1]={&
_tmp86};Cyc_aprintf(({const char*_tmp85="_tmp%X";_tag_arr(_tmp85,sizeof(char),
_get_zero_arr_size(_tmp85,7));}),_tag_arr(_tmp84,sizeof(void*),1));}});_tmp83;});
_tmp82;});}static struct _tagged_arr*Cyc_Toc_fresh_label(){return({struct
_tagged_arr*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp8A;_tmp8A.tag=1;_tmp8A.f1=(unsigned int)Cyc_Toc_fresh_label_counter
++;{void*_tmp88[1]={& _tmp8A};Cyc_aprintf(({const char*_tmp89="_LL%X";_tag_arr(
_tmp89,sizeof(char),_get_zero_arr_size(_tmp89,6));}),_tag_arr(_tmp88,sizeof(void*),
1));}});_tmp87;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp8B=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp8B))->hd)->name)
!= 0){if((carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp8B->hd)->typs != 0: 0)?
1:(!carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp8B->hd)->typs == 0: 0))++ ans;
_tmp8B=_tmp8B->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp8C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp8C != 0;_tmp8C=_tmp8C->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp8C->hd)->typs == 0)++ ans;}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct
_tuple2*a){struct Cyc_Core_Opt*_tmp8E;struct Cyc_Absyn_Tqual _tmp8F;void*_tmp90;
struct _tuple2 _tmp8D=*a;_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;_tmp90=_tmp8D.f3;return({
struct _tuple2*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->f1=_tmp8E;_tmp91->f2=
_tmp8F;_tmp91->f3=Cyc_Toc_typ_to_c(_tmp90);_tmp91;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp93;void*_tmp94;struct _tuple4 _tmp92=*x;
_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;return({struct _tuple4*_tmp95=_cycalloc(sizeof(*
_tmp95));_tmp95->f1=_tmp93;_tmp95->f2=Cyc_Toc_typ_to_c(_tmp94);_tmp95;});}static
void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp96=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp97;void*_tmp98;struct Cyc_Absyn_Tqual _tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct
Cyc_Absyn_Conref*_tmp9B;struct Cyc_Core_Opt*_tmp9C;struct Cyc_Core_Opt _tmp9D;void*
_tmp9E;_LL1F: if(_tmp96 <= (void*)3?1:*((int*)_tmp96)!= 7)goto _LL21;_tmp97=((
struct Cyc_Absyn_ArrayType_struct*)_tmp96)->f1;_tmp98=(void*)_tmp97.elt_type;
_tmp99=_tmp97.tq;_tmp9A=_tmp97.num_elts;_tmp9B=_tmp97.zero_term;_LL20: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmp98),_tmp99,_tmp9A,Cyc_Absyn_false_conref);_LL21: if(
_tmp96 <= (void*)3?1:*((int*)_tmp96)!= 0)goto _LL23;_tmp9C=((struct Cyc_Absyn_Evar_struct*)
_tmp96)->f2;if(_tmp9C == 0)goto _LL23;_tmp9D=*_tmp9C;_tmp9E=(void*)_tmp9D.v;_LL22:
return Cyc_Toc_typ_to_c_array(_tmp9E);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);
_LL1E:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->name=
f->name;_tmp9F->tq=f->tq;_tmp9F->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);
_tmp9F->width=f->width;_tmp9F->attributes=f->attributes;_tmp9F;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmpA0=
t;struct Cyc_Core_Opt*_tmpA1;struct Cyc_Core_Opt*_tmpA2;struct Cyc_Core_Opt _tmpA3;
void*_tmpA4;struct Cyc_Absyn_Tvar*_tmpA5;struct Cyc_Absyn_TunionInfo _tmpA6;void*
_tmpA7;struct Cyc_Absyn_TunionFieldInfo _tmpA8;void*_tmpA9;struct Cyc_Absyn_Tuniondecl*
_tmpAA;struct Cyc_Absyn_Tunionfield*_tmpAB;struct Cyc_Absyn_PtrInfo _tmpAC;void*
_tmpAD;struct Cyc_Absyn_Tqual _tmpAE;struct Cyc_Absyn_PtrAtts _tmpAF;struct Cyc_Absyn_Conref*
_tmpB0;struct Cyc_Absyn_ArrayInfo _tmpB1;void*_tmpB2;struct Cyc_Absyn_Tqual _tmpB3;
struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_FnInfo _tmpB5;void*_tmpB6;struct Cyc_List_List*
_tmpB7;int _tmpB8;struct Cyc_Absyn_VarargInfo*_tmpB9;struct Cyc_List_List*_tmpBA;
struct Cyc_List_List*_tmpBB;void*_tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_Absyn_AggrInfo
_tmpBE;void*_tmpBF;struct Cyc_List_List*_tmpC0;struct _tuple1*_tmpC1;struct Cyc_List_List*
_tmpC2;struct _tuple1*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Typedefdecl*
_tmpC5;void**_tmpC6;void*_tmpC7;_LL26: if((int)_tmpA0 != 0)goto _LL28;_LL27: return t;
_LL28: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 0)goto _LL2A;_tmpA1=((struct Cyc_Absyn_Evar_struct*)
_tmpA0)->f2;if(_tmpA1 != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 0)goto _LL2C;_tmpA2=((struct Cyc_Absyn_Evar_struct*)
_tmpA0)->f2;if(_tmpA2 == 0)goto _LL2C;_tmpA3=*_tmpA2;_tmpA4=(void*)_tmpA3.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmpA4);_LL2C: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 1)
goto _LL2E;_tmpA5=((struct Cyc_Absyn_VarType_struct*)_tmpA0)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmpA5)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 2)goto _LL30;_tmpA6=((struct Cyc_Absyn_TunionType_struct*)
_tmpA0)->f1;_tmpA7=(void*)_tmpA6.tunion_info;if(*((int*)_tmpA7)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmpA0 <= (void*)3?1:*((int*)
_tmpA0)!= 2)goto _LL32;_LL31:({void*_tmpC8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmpC9="unresolved TunionType";
_tag_arr(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,22));}),_tag_arr(_tmpC8,
sizeof(void*),0));});_LL32: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 3)goto _LL34;
_tmpA8=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpA0)->f1;_tmpA9=(void*)
_tmpA8.field_info;if(*((int*)_tmpA9)!= 1)goto _LL34;_tmpAA=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA9)->f1;_tmpAB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA9)->f2;_LL33:
if(_tmpAB->typs == 0){if(_tmpAA->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmpAB->name,({const char*
_tmpCA="_struct";_tag_arr(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,8));})));}
_LL34: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 3)goto _LL36;_LL35:({void*_tmpCB[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpCC="unresolved TunionFieldType";_tag_arr(_tmpCC,sizeof(char),
_get_zero_arr_size(_tmpCC,27));}),_tag_arr(_tmpCB,sizeof(void*),0));});_LL36: if(
_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 4)goto _LL38;_tmpAC=((struct Cyc_Absyn_PointerType_struct*)
_tmpA0)->f1;_tmpAD=(void*)_tmpAC.elt_typ;_tmpAE=_tmpAC.elt_tq;_tmpAF=_tmpAC.ptr_atts;
_tmpB0=_tmpAF.bounds;_LL37: _tmpAD=Cyc_Toc_typ_to_c_array(_tmpAD);{void*_tmpCD=(
void*)(Cyc_Absyn_compress_conref(_tmpB0))->v;void*_tmpCE;_LL5F: if(_tmpCD <= (void*)
1?1:*((int*)_tmpCD)!= 0)goto _LL61;_tmpCE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpCD)->f1;if((int)_tmpCE != 0)goto _LL61;_LL60: return Cyc_Toc_tagged_arr_typ;
_LL61:;_LL62: return Cyc_Absyn_star_typ(_tmpAD,(void*)2,_tmpAE,Cyc_Absyn_false_conref);
_LL5E:;}_LL38: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 5)goto _LL3A;_LL39: goto
_LL3B;_LL3A: if((int)_tmpA0 != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmpA0 <= (void*)
3?1:*((int*)_tmpA0)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(_tmpA0 <= (void*)3?1:*((
int*)_tmpA0)!= 7)goto _LL40;_tmpB1=((struct Cyc_Absyn_ArrayType_struct*)_tmpA0)->f1;
_tmpB2=(void*)_tmpB1.elt_type;_tmpB3=_tmpB1.tq;_tmpB4=_tmpB1.num_elts;_LL3F:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmpB2),_tmpB3,_tmpB4,Cyc_Absyn_false_conref);
_LL40: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 8)goto _LL42;_tmpB5=((struct Cyc_Absyn_FnType_struct*)
_tmpA0)->f1;_tmpB6=(void*)_tmpB5.ret_typ;_tmpB7=_tmpB5.args;_tmpB8=_tmpB5.c_varargs;
_tmpB9=_tmpB5.cyc_varargs;_tmpBA=_tmpB5.attributes;_LL41: {struct Cyc_List_List*
_tmpCF=0;for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){void*_tmpD0=(void*)_tmpBA->hd;_LL64:
if((int)_tmpD0 != 3)goto _LL66;_LL65: goto _LL67;_LL66: if((int)_tmpD0 != 4)goto _LL68;
_LL67: goto _LL69;_LL68: if(_tmpD0 <= (void*)17?1:*((int*)_tmpD0)!= 3)goto _LL6A;
_LL69: continue;_LL6A: if(_tmpD0 <= (void*)17?1:*((int*)_tmpD0)!= 4)goto _LL6C;_LL6B:
continue;_LL6C:;_LL6D: _tmpCF=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*
_tmpD1));_tmpD1->hd=(void*)((void*)_tmpBA->hd);_tmpD1->tl=_tmpCF;_tmpD1;});goto
_LL63;_LL63:;}{struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct
_tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmpB7);if(_tmpB9 != 0){void*_tmpD3=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)
_tmpB9->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple2*
_tmpD4=({struct _tuple2*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->f1=_tmpB9->name;
_tmpD6->f2=_tmpB9->tq;_tmpD6->f3=_tmpD3;_tmpD6;});_tmpD2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD2,({
struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->hd=_tmpD4;_tmpD5->tl=
0;_tmpD5;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpD7=_cycalloc(
sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_FnType_struct _tmpD8;_tmpD8.tag=8;
_tmpD8.f1=({struct Cyc_Absyn_FnInfo _tmpD9;_tmpD9.tvars=0;_tmpD9.effect=0;_tmpD9.ret_typ=(
void*)Cyc_Toc_typ_to_c(_tmpB6);_tmpD9.args=_tmpD2;_tmpD9.c_varargs=_tmpB8;_tmpD9.cyc_varargs=
0;_tmpD9.rgn_po=0;_tmpD9.attributes=_tmpCF;_tmpD9;});_tmpD8;});_tmpD7;});}}_LL42:
if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 9)goto _LL44;_tmpBB=((struct Cyc_Absyn_TupleType_struct*)
_tmpA0)->f1;_LL43: _tmpBB=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpBB);{
struct _tagged_arr*_tmpDA=Cyc_Toc_add_tuple_type(_tmpBB);return Cyc_Absyn_strct(
_tmpDA);}_LL44: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 11)goto _LL46;_tmpBC=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA0)->f1;_tmpBD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA0)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpDB=
_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpDC;
_tmpDC.tag=11;_tmpDC.f1=(void*)_tmpBC;_tmpDC.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_aggrfield_to_c,_tmpBD);_tmpDC;});_tmpDB;});_LL46: if(_tmpA0 <= (void*)3?1:*((
int*)_tmpA0)!= 10)goto _LL48;_tmpBE=((struct Cyc_Absyn_AggrType_struct*)_tmpA0)->f1;
_tmpBF=(void*)_tmpBE.aggr_info;_tmpC0=_tmpBE.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmpDD=Cyc_Absyn_get_known_aggrdecl(_tmpBF);if((void*)_tmpDD->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmpDD->name);return Cyc_Absyn_strctq(_tmpDD->name);}
_LL48: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 12)goto _LL4A;_tmpC1=((struct Cyc_Absyn_EnumType_struct*)
_tmpA0)->f1;_LL49: return t;_LL4A: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 13)goto
_LL4C;_tmpC2=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpA0)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpC2);return t;_LL4C: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 16)goto _LL4E;
_tmpC3=((struct Cyc_Absyn_TypedefType_struct*)_tmpA0)->f1;_tmpC4=((struct Cyc_Absyn_TypedefType_struct*)
_tmpA0)->f2;_tmpC5=((struct Cyc_Absyn_TypedefType_struct*)_tmpA0)->f3;_tmpC6=((
struct Cyc_Absyn_TypedefType_struct*)_tmpA0)->f4;_LL4D: if(_tmpC6 == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpDF;_tmpDF.tag=16;_tmpDF.f1=_tmpC3;_tmpDF.f2=
0;_tmpDF.f3=_tmpC5;_tmpDF.f4=0;_tmpDF;});_tmpDE;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpE1;_tmpE1.tag=16;_tmpE1.f1=_tmpC3;_tmpE1.f2=0;_tmpE1.f3=_tmpC5;_tmpE1.f4=({
void**_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=Cyc_Toc_typ_to_c_array(*_tmpC6);
_tmpE2;});_tmpE1;});_tmpE0;});}_LL4E: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 14)
goto _LL50;_LL4F: return Cyc_Absyn_uint_typ;_LL50: if(_tmpA0 <= (void*)3?1:*((int*)
_tmpA0)!= 17)goto _LL52;_LL51: return Cyc_Absyn_sint_typ;_LL52: if(_tmpA0 <= (void*)3?
1:*((int*)_tmpA0)!= 15)goto _LL54;_tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpA0)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 18)goto _LL56;_LL55:({
void*_tmpE3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE4="Toc::typ_to_c: type translation passed a type integer";
_tag_arr(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,54));}),_tag_arr(_tmpE3,
sizeof(void*),0));});_LL56: if((int)_tmpA0 != 2)goto _LL58;_LL57:({void*_tmpE5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmpE6="Toc::typ_to_c: type translation passed the heap region";_tag_arr(_tmpE6,
sizeof(char),_get_zero_arr_size(_tmpE6,55));}),_tag_arr(_tmpE5,sizeof(void*),0));});
_LL58: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 19)goto _LL5A;_LL59:({void*_tmpE7[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE8="Toc::typ_to_c: type translation passed the access eff";
_tag_arr(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,54));}),_tag_arr(_tmpE7,
sizeof(void*),0));});_LL5A: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 20)goto _LL5C;
_LL5B:({void*_tmpE9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpEA="Toc::typ_to_c: type translation passed the join eff";_tag_arr(
_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,52));}),_tag_arr(_tmpE9,sizeof(
void*),0));});_LL5C: if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 21)goto _LL25;_LL5D:({
void*_tmpEB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmpEC="Toc::typ_to_c: type translation passed the regions eff";
_tag_arr(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,55));}),_tag_arr(_tmpEB,
sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmpED=t;struct
Cyc_Absyn_ArrayInfo _tmpEE;void*_tmpEF;struct Cyc_Absyn_Tqual _tmpF0;_LL6F: if(
_tmpED <= (void*)3?1:*((int*)_tmpED)!= 7)goto _LL71;_tmpEE=((struct Cyc_Absyn_ArrayType_struct*)
_tmpED)->f1;_tmpEF=(void*)_tmpEE.elt_type;_tmpF0=_tmpEE.tq;_LL70: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ(_tmpEF,(void*)2,_tmpF0,Cyc_Absyn_false_conref),e,l);_LL71:;
_LL72: return Cyc_Absyn_cast_exp(t,e,l);_LL6E:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmpF1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpF2;void*_tmpF3;
struct Cyc_Absyn_AggrInfo _tmpF4;void*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_Absyn_TunionFieldInfo
_tmpF7;void*_tmpF8;struct Cyc_Absyn_Tuniondecl*_tmpF9;struct Cyc_Absyn_Tunionfield*
_tmpFA;struct Cyc_List_List*_tmpFB;_LL74: if((int)_tmpF1 != 0)goto _LL76;_LL75:
return 1;_LL76: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 1)goto _LL78;_LL77: return 0;
_LL78: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 5)goto _LL7A;_LL79: goto _LL7B;_LL7A:
if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 12)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(
_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 13)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if((int)
_tmpF1 != 1)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)
!= 6)goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 8)
goto _LL84;_LL83: goto _LL85;_LL84: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 17)goto
_LL86;_LL85: goto _LL87;_LL86: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 14)goto
_LL88;_LL87: return 1;_LL88: if(_tmpF1 <= (void*)3?1:*((int*)_tmpF1)!= 7)goto _LL8A;
_tmpF2=((struct Cyc_Absyn_ArrayType_struct*)_tmpF1)->f1;_tmpF3=(void*)_tmpF2.elt_type;
_LL89: return Cyc_Toc_atomic_typ(_tmpF3);_LL8A: if(_tmpF1 <= (void*)3?1:*((int*)
_tmpF1)!= 10)goto _LL8C;_tmpF4=((struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1;
_tmpF5=(void*)_tmpF4.aggr_info;_LL8B:{void*_tmpFC=_tmpF5;_LL9B: if(*((int*)_tmpFC)
!= 0)goto _LL9D;_LL9C: return 0;_LL9D:;_LL9E: goto _LL9A;_LL9A:;}{struct Cyc_Absyn_Aggrdecl*
_tmpFD=Cyc_Absyn_get_known_aggrdecl(_tmpF5);if(_tmpFD->impl == 0)return 0;{struct
Cyc_List_List*_tmpFE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFD->impl))->fields;
for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpFE->hd)->type))return 0;}}return 1;}_LL8C: if(_tmpF1 <= (void*)3?1:*((int*)
_tmpF1)!= 11)goto _LL8E;_tmpF6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpF1)->f2;
_LL8D: for(0;_tmpF6 != 0;_tmpF6=_tmpF6->tl){if(!Cyc_Toc_atomic_typ((void*)((struct
Cyc_Absyn_Aggrfield*)_tmpF6->hd)->type))return 0;}return 1;_LL8E: if(_tmpF1 <= (void*)
3?1:*((int*)_tmpF1)!= 3)goto _LL90;_tmpF7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1;_tmpF8=(void*)_tmpF7.field_info;if(*((int*)_tmpF8)!= 1)goto _LL90;
_tmpF9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF8)->f1;_tmpFA=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpF8)->f2;_LL8F: _tmpFB=_tmpFA->typs;goto _LL91;_LL90: if(_tmpF1 <= (void*)3?1:*((
int*)_tmpF1)!= 9)goto _LL92;_tmpFB=((struct Cyc_Absyn_TupleType_struct*)_tmpF1)->f1;
_LL91: for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpFB->hd)).f2))return 0;}return 1;_LL92: if(_tmpF1 <= (void*)3?1:*((int*)
_tmpF1)!= 2)goto _LL94;_LL93: goto _LL95;_LL94: if(_tmpF1 <= (void*)3?1:*((int*)
_tmpF1)!= 4)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmpF1 <= (void*)3?1:*((int*)
_tmpF1)!= 15)goto _LL98;_LL97: return 0;_LL98:;_LL99:({struct Cyc_String_pa_struct
_tmp101;_tmp101.tag=0;_tmp101.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpFF[1]={& _tmp101};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp100="atomic_typ:  bad type %s";_tag_arr(
_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,25));}),_tag_arr(_tmpFF,sizeof(
void*),1));}});_LL73:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp102=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp103;void*_tmp104;_LLA0: if(_tmp102 <= (void*)3?1:*((
int*)_tmp102)!= 4)goto _LLA2;_tmp103=((struct Cyc_Absyn_PointerType_struct*)
_tmp102)->f1;_tmp104=(void*)_tmp103.elt_typ;_LLA1: {void*_tmp105=Cyc_Tcutil_compress(
_tmp104);_LLA5: if((int)_tmp105 != 0)goto _LLA7;_LLA6: return 1;_LLA7:;_LLA8: return 0;
_LLA4:;}_LLA2:;_LLA3: return 0;_LL9F:;}static int Cyc_Toc_is_poly_field(void*t,
struct _tagged_arr*f){void*_tmp106=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp107;void*_tmp108;struct Cyc_List_List*_tmp109;_LLAA: if(_tmp106 <= (void*)3?1:*((
int*)_tmp106)!= 10)goto _LLAC;_tmp107=((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1;
_tmp108=(void*)_tmp107.aggr_info;_LLAB: {struct Cyc_Absyn_Aggrdecl*_tmp10A=Cyc_Absyn_get_known_aggrdecl(
_tmp108);if(_tmp10A->impl == 0)({void*_tmp10B[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10C="is_poly_field: type missing fields";
_tag_arr(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,35));}),_tag_arr(
_tmp10B,sizeof(void*),0));});_tmp109=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp10A->impl))->fields;goto _LLAD;}_LLAC: if(_tmp106 <= (void*)3?1:*((int*)_tmp106)
!= 11)goto _LLAE;_tmp109=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp106)->f2;
_LLAD: {struct Cyc_Absyn_Aggrfield*_tmp10D=Cyc_Absyn_lookup_field(_tmp109,f);if(
_tmp10D == 0)({struct Cyc_String_pa_struct _tmp110;_tmp110.tag=0;_tmp110.f1=(struct
_tagged_arr)((struct _tagged_arr)*f);{void*_tmp10E[1]={& _tmp110};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp10F="is_poly_field: bad field %s";
_tag_arr(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,28));}),_tag_arr(
_tmp10E,sizeof(void*),1));}});return Cyc_Toc_is_void_star((void*)_tmp10D->type);}
_LLAE:;_LLAF:({struct Cyc_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp111[1]={&
_tmp113};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp112="is_poly_field: bad type %s";_tag_arr(_tmp112,sizeof(char),
_get_zero_arr_size(_tmp112,27));}),_tag_arr(_tmp111,sizeof(void*),1));}});_LLA9:;}
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp114=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp115;struct _tagged_arr*_tmp116;struct Cyc_Absyn_Exp*
_tmp117;struct _tagged_arr*_tmp118;_LLB1: if(*((int*)_tmp114)!= 21)goto _LLB3;
_tmp115=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp114)->f1;_tmp116=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp114)->f2;_LLB2: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp115->topt))->v,_tmp116);_LLB3: if(*((int*)_tmp114)!= 22)goto _LLB5;
_tmp117=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp114)->f1;_tmp118=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp114)->f2;_LLB4: {void*_tmp119=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp117->topt))->v);struct Cyc_Absyn_PtrInfo _tmp11A;void*_tmp11B;
_LLB8: if(_tmp119 <= (void*)3?1:*((int*)_tmp119)!= 4)goto _LLBA;_tmp11A=((struct Cyc_Absyn_PointerType_struct*)
_tmp119)->f1;_tmp11B=(void*)_tmp11A.elt_typ;_LLB9: return Cyc_Toc_is_poly_field(
_tmp11B,_tmp118);_LLBA:;_LLBB:({struct Cyc_String_pa_struct _tmp11E;_tmp11E.tag=0;
_tmp11E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp117->topt))->v));{void*_tmp11C[1]={& _tmp11E};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp11D="is_poly_project:  bad type %s";_tag_arr(_tmp11D,sizeof(char),
_get_zero_arr_size(_tmp11D,30));}),_tag_arr(_tmp11C,sizeof(void*),1));}});_LLB7:;}
_LLB5:;_LLB6: return 0;_LLB0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->hd=s;_tmp11F->tl=0;_tmp11F;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp120=_cycalloc(sizeof(*
_tmp120));_tmp120->hd=s;_tmp120->tl=0;_tmp120;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp121[2];_tmp121[1]=s;_tmp121[0]=
rgn;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp121,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(
void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(
elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*
_tmp122[2];_tmp122[1]=n;_tmp122[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp122,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp123[2];
_tmp123[1]=n;_tmp123[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp123,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*
_tmp124[3];_tmp124[2]=n;_tmp124[1]=s;_tmp124[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp124,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp125=
_cycalloc(sizeof(*_tmp125));_tmp125->hd=e;_tmp125->tl=0;_tmp125;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp126=(void*)e->r;void*_tmp127;
_LLBD: if(*((int*)_tmp126)!= 0)goto _LLBF;_tmp127=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp126)->f1;if(_tmp127 <= (void*)1?1:*((int*)_tmp127)!= 5)goto _LLBF;_LLBE:
is_string=1;goto _LLBC;_LLBF:;_LLC0: goto _LLBC;_LLBC:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,
vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({
struct Cyc_Absyn_Var_d_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=vd;_tmp12A;});
_tmp129;}),0);_tmp128->tl=Cyc_Toc_result_decls;_tmp128;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple6*_tmp12B[3];_tmp12B[2]=({struct _tuple6*_tmp12E=_cycalloc(sizeof(*_tmp12E));
_tmp12E->f1=0;_tmp12E->f2=xplussz;_tmp12E;});_tmp12B[1]=({struct _tuple6*_tmp12D=
_cycalloc(sizeof(*_tmp12D));_tmp12D->f1=0;_tmp12D->f2=xexp;_tmp12D;});_tmp12B[0]=({
struct _tuple6*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->f1=0;_tmp12C->f2=xexp;
_tmp12C;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp12B,sizeof(struct _tuple6*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp12F=_cycalloc(sizeof(*_tmp12F));
_tmp12F->break_lab=0;_tmp12F->continue_lab=0;_tmp12F->fallthru_info=0;_tmp12F->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp12F->toplevel=1;_tmp12F;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp130=_cycalloc(
sizeof(*_tmp130));_tmp130[0]=*e;_tmp130;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp131=(*x).f1;_LLC2: if(_tmp131 <= (void*)1?1:*((int*)
_tmp131)!= 0)goto _LLC4;_LLC3:({struct Cyc_String_pa_struct _tmp134;_tmp134.tag=0;
_tmp134.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(x));{
void*_tmp132[1]={& _tmp134};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp133="Toc::add_varmap on Rel_n: %s\n";_tag_arr(
_tmp133,sizeof(char),_get_zero_arr_size(_tmp133,30));}),_tag_arr(_tmp132,sizeof(
void*),1));}});_LLC4:;_LLC5: goto _LLC1;_LLC1:;}{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135[0]=*e;_tmp135;});ans->varmap=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp136=_cycalloc(
sizeof(*_tmp136));_tmp136[0]=*e;_tmp136;});ans->break_lab=0;ans->continue_lab=0;
return ans;}struct _tuple8{struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*
f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp137=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp137=({
struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp138->tl=_tmp137;_tmp138;});}_tmp137=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp137);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=*e;_tmp13C;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->v=
break_l;_tmp139;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp13A=_cycalloc(
sizeof(*_tmp13A));_tmp13A->v=({struct _tuple8*_tmp13B=_cycalloc(sizeof(*_tmp13B));
_tmp13B->f1=fallthru_l;_tmp13B->f2=_tmp137;_tmp13B->f3=next_case_env->varmap;
_tmp13B;});_tmp13A;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=*e;_tmp13E;});ans->break_lab=({
struct Cyc_Core_Opt*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->v=break_l;
_tmp13D;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=*e;_tmp141;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp13F=_cycalloc(sizeof(*_tmp13F));
_tmp13F->v=({struct _tuple8*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->f1=
next_l;_tmp140->f2=0;_tmp140->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp140;});_tmp13F;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp142=(void*)e->annot;
_LLC7: if(*((void**)_tmp142)!= Cyc_CfFlowInfo_UnknownZ)goto _LLC9;_LLC8: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC9: if(*((void**)_tmp142)
!= Cyc_CfFlowInfo_NotZero)goto _LLCB;_LLCA: return 0;_LLCB: if(*((void**)_tmp142)!= 
Cyc_CfFlowInfo_IsZero)goto _LLCD;_LLCC:({void*_tmp143[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmp144="dereference of NULL pointer";_tag_arr(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,28));}),_tag_arr(_tmp143,sizeof(void*),0));});return 0;
_LLCD: if(_tmp142 != Cyc_Absyn_EmptyAnnot)goto _LLCF;_LLCE: return 0;_LLCF:;_LLD0:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp145=_cycalloc(sizeof(*
_tmp145));_tmp145[0]=({struct Cyc_Core_Impossible_struct _tmp146;_tmp146.tag=Cyc_Core_Impossible;
_tmp146.f1=({const char*_tmp147="need_null_check";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,16));});_tmp146;});_tmp145;}));_LLC6:;}static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp148=(void*)e->annot;
struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp14A;_LLD2: if(*((void**)
_tmp148)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD4;_tmp149=((struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp148)->f1;_LLD3: return _tmp149;_LLD4: if(*((void**)_tmp148)!= Cyc_CfFlowInfo_NotZero)
goto _LLD6;_tmp14A=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp148)->f1;_LLD5:
return _tmp14A;_LLD6: if(*((void**)_tmp148)!= Cyc_CfFlowInfo_IsZero)goto _LLD8;
_LLD7:({void*_tmp14B[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp14C="dereference of NULL pointer";
_tag_arr(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,28));}),_tag_arr(
_tmp14B,sizeof(void*),0));});return 0;_LLD8: if(_tmp148 != Cyc_Absyn_EmptyAnnot)
goto _LLDA;_LLD9: return 0;_LLDA:;_LLDB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Core_Impossible_struct
_tmp14E;_tmp14E.tag=Cyc_Core_Impossible;_tmp14E.f1=({const char*_tmp14F="get_relns";
_tag_arr(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,10));});_tmp14E;});
_tmp14D;}));_LLD1:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp150=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp151;struct Cyc_Absyn_PtrAtts
_tmp152;struct Cyc_Absyn_Conref*_tmp153;struct Cyc_Absyn_Conref*_tmp154;struct Cyc_Absyn_ArrayInfo
_tmp155;struct Cyc_Absyn_Exp*_tmp156;_LLDD: if(_tmp150 <= (void*)3?1:*((int*)
_tmp150)!= 4)goto _LLDF;_tmp151=((struct Cyc_Absyn_PointerType_struct*)_tmp150)->f1;
_tmp152=_tmp151.ptr_atts;_tmp153=_tmp152.bounds;_tmp154=_tmp152.zero_term;_LLDE: {
void*_tmp157=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp153);struct Cyc_Absyn_Exp*
_tmp158;_LLE4: if((int)_tmp157 != 0)goto _LLE6;_LLE5: return 0;_LLE6: if(_tmp157 <= (
void*)1?1:*((int*)_tmp157)!= 0)goto _LLE8;_tmp158=((struct Cyc_Absyn_Upper_b_struct*)
_tmp157)->f1;_LLE7: {unsigned int _tmp15A;int _tmp15B;struct _tuple5 _tmp159=Cyc_Evexp_eval_const_uint_exp(
_tmp158);_tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;return _tmp15B?i <= _tmp15A: 0;}
_LLE8: if(_tmp157 <= (void*)1?1:*((int*)_tmp157)!= 1)goto _LLE3;_LLE9:({void*
_tmp15C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp15D="check_const_array: AbsUpper_b";_tag_arr(_tmp15D,sizeof(char),
_get_zero_arr_size(_tmp15D,30));}),_tag_arr(_tmp15C,sizeof(void*),0));});_LLE3:;}
_LLDF: if(_tmp150 <= (void*)3?1:*((int*)_tmp150)!= 7)goto _LLE1;_tmp155=((struct Cyc_Absyn_ArrayType_struct*)
_tmp150)->f1;_tmp156=_tmp155.num_elts;_LLE0: if(_tmp156 == 0)return 0;{unsigned int
_tmp15F;int _tmp160;struct _tuple5 _tmp15E=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp156));_tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;return _tmp160?i <= 
_tmp15F: 0;}_LLE1:;_LLE2: return 0;_LLDC:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp161=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp161->vd != y)continue;{void*_tmp162=(void*)_tmp161->rop;struct
Cyc_Absyn_Vardecl*_tmp163;struct Cyc_Absyn_Vardecl*_tmp164;_LLEB: if(*((int*)
_tmp162)!= 2)goto _LLED;_tmp163=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp162)->f1;
_LLEC: _tmp164=_tmp163;goto _LLEE;_LLED: if(*((int*)_tmp162)!= 4)goto _LLEF;_tmp164=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp162)->f1;_LLEE: if(_tmp164 == v)return
1;else{goto _LLEA;}_LLEF:;_LLF0: continue;_LLEA:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp165=(void*)e->r;void*_tmp166;struct Cyc_Absyn_Vardecl*_tmp167;void*_tmp168;
struct Cyc_Absyn_Vardecl*_tmp169;void*_tmp16A;struct Cyc_Absyn_Vardecl*_tmp16B;
void*_tmp16C;struct Cyc_Absyn_Vardecl*_tmp16D;void*_tmp16E;struct Cyc_List_List*
_tmp16F;struct Cyc_List_List _tmp170;struct Cyc_Absyn_Exp*_tmp171;_LLF2: if(*((int*)
_tmp165)!= 1)goto _LLF4;_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp165)->f2;
if(_tmp166 <= (void*)1?1:*((int*)_tmp166)!= 4)goto _LLF4;_tmp167=((struct Cyc_Absyn_Pat_b_struct*)
_tmp166)->f1;_LLF3: _tmp169=_tmp167;goto _LLF5;_LLF4: if(*((int*)_tmp165)!= 1)goto
_LLF6;_tmp168=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp165)->f2;if(_tmp168 <= (
void*)1?1:*((int*)_tmp168)!= 3)goto _LLF6;_tmp169=((struct Cyc_Absyn_Local_b_struct*)
_tmp168)->f1;_LLF5: _tmp16B=_tmp169;goto _LLF7;_LLF6: if(*((int*)_tmp165)!= 1)goto
_LLF8;_tmp16A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp165)->f2;if(_tmp16A <= (
void*)1?1:*((int*)_tmp16A)!= 0)goto _LLF8;_tmp16B=((struct Cyc_Absyn_Global_b_struct*)
_tmp16A)->f1;_LLF7: _tmp16D=_tmp16B;goto _LLF9;_LLF8: if(*((int*)_tmp165)!= 1)goto
_LLFA;_tmp16C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp165)->f2;if(_tmp16C <= (
void*)1?1:*((int*)_tmp16C)!= 2)goto _LLFA;_tmp16D=((struct Cyc_Absyn_Param_b_struct*)
_tmp16C)->f1;_LLF9: if(_tmp16D->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp16D))return 1;goto _LLF1;_LLFA: if(*((int*)_tmp165)!= 3)goto _LLFC;
_tmp16E=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp165)->f1;if((int)_tmp16E != 
19)goto _LLFC;_tmp16F=((struct Cyc_Absyn_Primop_e_struct*)_tmp165)->f2;if(_tmp16F
== 0)goto _LLFC;_tmp170=*_tmp16F;_tmp171=(struct Cyc_Absyn_Exp*)_tmp170.hd;_LLFB:{
void*_tmp172=(void*)_tmp171->r;void*_tmp173;struct Cyc_Absyn_Vardecl*_tmp174;void*
_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;void*_tmp177;struct Cyc_Absyn_Vardecl*
_tmp178;void*_tmp179;struct Cyc_Absyn_Vardecl*_tmp17A;_LLFF: if(*((int*)_tmp172)!= 
1)goto _LL101;_tmp173=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp172)->f2;if(
_tmp173 <= (void*)1?1:*((int*)_tmp173)!= 4)goto _LL101;_tmp174=((struct Cyc_Absyn_Pat_b_struct*)
_tmp173)->f1;_LL100: _tmp176=_tmp174;goto _LL102;_LL101: if(*((int*)_tmp172)!= 1)
goto _LL103;_tmp175=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp172)->f2;if(
_tmp175 <= (void*)1?1:*((int*)_tmp175)!= 3)goto _LL103;_tmp176=((struct Cyc_Absyn_Local_b_struct*)
_tmp175)->f1;_LL102: _tmp178=_tmp176;goto _LL104;_LL103: if(*((int*)_tmp172)!= 1)
goto _LL105;_tmp177=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp172)->f2;if(
_tmp177 <= (void*)1?1:*((int*)_tmp177)!= 0)goto _LL105;_tmp178=((struct Cyc_Absyn_Global_b_struct*)
_tmp177)->f1;_LL104: _tmp17A=_tmp178;goto _LL106;_LL105: if(*((int*)_tmp172)!= 1)
goto _LL107;_tmp179=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp172)->f2;if(
_tmp179 <= (void*)1?1:*((int*)_tmp179)!= 2)goto _LL107;_tmp17A=((struct Cyc_Absyn_Param_b_struct*)
_tmp179)->f1;_LL106: return _tmp17A == v;_LL107:;_LL108: goto _LLFE;_LLFE:;}goto _LLF1;
_LLFC:;_LLFD: goto _LLF1;_LLF1:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp17B=(void*)a->r;
void*_tmp17C;struct Cyc_Absyn_Vardecl*_tmp17D;void*_tmp17E;struct Cyc_Absyn_Vardecl*
_tmp17F;void*_tmp180;struct Cyc_Absyn_Vardecl*_tmp181;void*_tmp182;struct Cyc_Absyn_Vardecl*
_tmp183;_LL10A: if(*((int*)_tmp17B)!= 1)goto _LL10C;_tmp17C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp17B)->f2;if(_tmp17C <= (void*)1?1:*((int*)_tmp17C)!= 4)goto _LL10C;_tmp17D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp17C)->f1;_LL10B: _tmp17F=_tmp17D;goto _LL10D;
_LL10C: if(*((int*)_tmp17B)!= 1)goto _LL10E;_tmp17E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp17B)->f2;if(_tmp17E <= (void*)1?1:*((int*)_tmp17E)!= 3)goto _LL10E;_tmp17F=((
struct Cyc_Absyn_Local_b_struct*)_tmp17E)->f1;_LL10D: _tmp181=_tmp17F;goto _LL10F;
_LL10E: if(*((int*)_tmp17B)!= 1)goto _LL110;_tmp180=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp17B)->f2;if(_tmp180 <= (void*)1?1:*((int*)_tmp180)!= 0)goto _LL110;_tmp181=((
struct Cyc_Absyn_Global_b_struct*)_tmp180)->f1;_LL10F: _tmp183=_tmp181;goto _LL111;
_LL110: if(*((int*)_tmp17B)!= 1)goto _LL112;_tmp182=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp17B)->f2;if(_tmp182 <= (void*)1?1:*((int*)_tmp182)!= 2)goto _LL112;_tmp183=((
struct Cyc_Absyn_Param_b_struct*)_tmp182)->f1;_LL111: if(_tmp183->escapes)return 0;
inner_loop: {void*_tmp184=(void*)i->r;void*_tmp185;struct Cyc_Absyn_Exp*_tmp186;
void*_tmp187;void*_tmp188;int _tmp189;void*_tmp18A;void*_tmp18B;int _tmp18C;void*
_tmp18D;void*_tmp18E;int _tmp18F;void*_tmp190;struct Cyc_List_List*_tmp191;struct
Cyc_List_List _tmp192;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_List_List*_tmp194;
struct Cyc_List_List _tmp195;struct Cyc_Absyn_Exp*_tmp196;void*_tmp197;struct Cyc_Absyn_Vardecl*
_tmp198;void*_tmp199;struct Cyc_Absyn_Vardecl*_tmp19A;void*_tmp19B;struct Cyc_Absyn_Vardecl*
_tmp19C;void*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp19E;_LL115: if(*((int*)_tmp184)
!= 13)goto _LL117;_tmp185=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp184)->f1;
_tmp186=((struct Cyc_Absyn_Cast_e_struct*)_tmp184)->f2;_LL116: i=_tmp186;goto
inner_loop;_LL117: if(*((int*)_tmp184)!= 0)goto _LL119;_tmp187=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp184)->f1;if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 2)goto _LL119;_tmp188=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp187)->f1;if((int)_tmp188 != 2)goto
_LL119;_tmp189=((struct Cyc_Absyn_Int_c_struct*)_tmp187)->f2;_LL118: _tmp18C=
_tmp189;goto _LL11A;_LL119: if(*((int*)_tmp184)!= 0)goto _LL11B;_tmp18A=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp184)->f1;if(_tmp18A <= (void*)1?1:*((int*)
_tmp18A)!= 2)goto _LL11B;_tmp18B=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp18A)->f1;
if((int)_tmp18B != 0)goto _LL11B;_tmp18C=((struct Cyc_Absyn_Int_c_struct*)_tmp18A)->f2;
_LL11A: return _tmp18C >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp18C + 1),(
void*)_tmp183->type): 0;_LL11B: if(*((int*)_tmp184)!= 0)goto _LL11D;_tmp18D=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp184)->f1;if(_tmp18D <= (void*)1?1:*((int*)
_tmp18D)!= 2)goto _LL11D;_tmp18E=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp18D)->f1;
if((int)_tmp18E != 1)goto _LL11D;_tmp18F=((struct Cyc_Absyn_Int_c_struct*)_tmp18D)->f2;
_LL11C: return Cyc_Toc_check_const_array((unsigned int)(_tmp18F + 1),(void*)_tmp183->type);
_LL11D: if(*((int*)_tmp184)!= 3)goto _LL11F;_tmp190=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp184)->f1;if((int)_tmp190 != 4)goto _LL11F;_tmp191=((struct Cyc_Absyn_Primop_e_struct*)
_tmp184)->f2;if(_tmp191 == 0)goto _LL11F;_tmp192=*_tmp191;_tmp193=(struct Cyc_Absyn_Exp*)
_tmp192.hd;_tmp194=_tmp192.tl;if(_tmp194 == 0)goto _LL11F;_tmp195=*_tmp194;_tmp196=(
struct Cyc_Absyn_Exp*)_tmp195.hd;_LL11E: return Cyc_Toc_check_leq_size(relns,
_tmp183,_tmp196);_LL11F: if(*((int*)_tmp184)!= 1)goto _LL121;_tmp197=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp184)->f2;if(_tmp197 <= (void*)1?1:*((int*)
_tmp197)!= 4)goto _LL121;_tmp198=((struct Cyc_Absyn_Pat_b_struct*)_tmp197)->f1;
_LL120: _tmp19A=_tmp198;goto _LL122;_LL121: if(*((int*)_tmp184)!= 1)goto _LL123;
_tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp184)->f2;if(_tmp199 <= (void*)
1?1:*((int*)_tmp199)!= 3)goto _LL123;_tmp19A=((struct Cyc_Absyn_Local_b_struct*)
_tmp199)->f1;_LL122: _tmp19C=_tmp19A;goto _LL124;_LL123: if(*((int*)_tmp184)!= 1)
goto _LL125;_tmp19B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp184)->f2;if(
_tmp19B <= (void*)1?1:*((int*)_tmp19B)!= 0)goto _LL125;_tmp19C=((struct Cyc_Absyn_Global_b_struct*)
_tmp19B)->f1;_LL124: _tmp19E=_tmp19C;goto _LL126;_LL125: if(*((int*)_tmp184)!= 1)
goto _LL127;_tmp19D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp184)->f2;if(
_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 2)goto _LL127;_tmp19E=((struct Cyc_Absyn_Param_b_struct*)
_tmp19D)->f1;_LL126: if(_tmp19E->escapes)return 0;{struct Cyc_List_List*_tmp19F=
relns;for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){struct Cyc_CfFlowInfo_Reln*_tmp1A0=(
struct Cyc_CfFlowInfo_Reln*)_tmp19F->hd;if(_tmp1A0->vd == _tmp19E){void*_tmp1A1=(
void*)_tmp1A0->rop;struct Cyc_Absyn_Vardecl*_tmp1A2;struct Cyc_Absyn_Vardecl*
_tmp1A3;unsigned int _tmp1A4;_LL12A: if(*((int*)_tmp1A1)!= 2)goto _LL12C;_tmp1A2=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A1)->f1;_LL12B: if(_tmp183 == _tmp1A2)
return 1;else{goto _LL129;}_LL12C: if(*((int*)_tmp1A1)!= 1)goto _LL12E;_tmp1A3=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A1)->f1;_LL12D:{struct Cyc_List_List*
_tmp1A5=relns;for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1A6=(struct Cyc_CfFlowInfo_Reln*)_tmp1A5->hd;if(_tmp1A6->vd == _tmp1A3){void*
_tmp1A7=(void*)_tmp1A6->rop;struct Cyc_Absyn_Vardecl*_tmp1A8;struct Cyc_Absyn_Vardecl*
_tmp1A9;unsigned int _tmp1AA;struct Cyc_Absyn_Vardecl*_tmp1AB;_LL133: if(*((int*)
_tmp1A7)!= 4)goto _LL135;_tmp1A8=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1A7)->f1;_LL134: _tmp1A9=_tmp1A8;goto _LL136;_LL135: if(*((int*)_tmp1A7)!= 2)
goto _LL137;_tmp1A9=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1A7)->f1;_LL136:
if(_tmp183 == _tmp1A9)return 1;goto _LL132;_LL137: if(*((int*)_tmp1A7)!= 0)goto
_LL139;_tmp1AA=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1A7)->f1;_LL138:
return Cyc_Toc_check_const_array(_tmp1AA,(void*)_tmp183->type);_LL139: if(*((int*)
_tmp1A7)!= 1)goto _LL13B;_tmp1AB=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1A7)->f1;
_LL13A: if(Cyc_Toc_check_leq_size_var(relns,_tmp183,_tmp1AB))return 1;goto _LL132;
_LL13B:;_LL13C: goto _LL132;_LL132:;}}}goto _LL129;_LL12E: if(*((int*)_tmp1A1)!= 3)
goto _LL130;_tmp1A4=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1A1)->f1;_LL12F:
return Cyc_Toc_check_const_array(_tmp1A4,(void*)_tmp183->type);_LL130:;_LL131:
goto _LL129;_LL129:;}}}goto _LL114;_LL127:;_LL128: goto _LL114;_LL114:;}goto _LL109;
_LL112:;_LL113: goto _LL109;_LL109:;}return 0;}static void*Cyc_Toc_get_c_typ(struct
Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1AC[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1AD="Missing type in primop ";
_tag_arr(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,24));}),_tag_arr(
_tmp1AC,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp1AE[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1AF="Missing type in primop ";
_tag_arr(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,24));}),_tag_arr(
_tmp1AE,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->f1=Cyc_Toc_mt_tq;_tmp1B0->f2=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1B0;});}
static struct _tuple6*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple6*_tmp1B1=_cycalloc(sizeof(*
_tmp1B1));_tmp1B1->f1=0;_tmp1B1->f2=e;_tmp1B1;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1B2=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1B2):
Cyc_Toc_malloc_ptr(_tmp1B2));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(rgnopt);Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(
r,_tmp1B2);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){struct _tuple6 _tmp1B5;struct Cyc_List_List*
_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;struct _tuple6*_tmp1B4=(struct _tuple6*)
_tmp1B3->hd;_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.f1;_tmp1B7=_tmp1B5.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1B6 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1B6->tl != 0)({void*_tmp1B8[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp1B9="multiple designators in array";
_tag_arr(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,30));}),_tag_arr(
_tmp1B8,sizeof(void*),0));});{void*_tmp1BA=(void*)_tmp1B6->hd;void*_tmp1BB=
_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;_LL13E: if(*((int*)_tmp1BB)!= 0)goto _LL140;
_tmp1BC=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1BB)->f1;_LL13F: Cyc_Toc_exp_to_c(
nv,_tmp1BC);e_index=_tmp1BC;goto _LL13D;_LL140: if(*((int*)_tmp1BB)!= 1)goto _LL13D;
_LL141:({void*_tmp1BD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(({const char*_tmp1BE="field name designators in array";_tag_arr(
_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,32));}),_tag_arr(_tmp1BD,sizeof(
void*),0));});_LL13D:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,
e_index,0);void*_tmp1BF=(void*)_tmp1B7->r;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Vardecl*
_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C3;int _tmp1C4;void*
_tmp1C5;struct Cyc_List_List*_tmp1C6;_LL143: if(*((int*)_tmp1BF)!= 26)goto _LL145;
_tmp1C0=((struct Cyc_Absyn_Array_e_struct*)_tmp1BF)->f1;_LL144: s=Cyc_Toc_init_array(
nv,lval,_tmp1C0,s);goto _LL142;_LL145: if(*((int*)_tmp1BF)!= 27)goto _LL147;_tmp1C1=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BF)->f1;_tmp1C2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BF)->f2;_tmp1C3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BF)->f3;
_tmp1C4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1BF)->f4;_LL146: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1C1,_tmp1C2,_tmp1C3,_tmp1C4,s,0);goto _LL142;_LL147: if(*((int*)
_tmp1BF)!= 29)goto _LL149;_tmp1C5=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1BF)->f1;_tmp1C6=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1BF)->f2;_LL148:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1C5,_tmp1C6,s);goto _LL142;_LL149:;_LL14A:
Cyc_Toc_exp_to_c(nv,_tmp1B7);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1B7,0),s,0);goto _LL142;_LL142:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1C7=vd->name;
void*_tmp1C8=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1C7,Cyc_Absyn_varb_exp(_tmp1C7,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1D2;_tmp1D2.tag=3;_tmp1D2.f1=vd;_tmp1D2;});_tmp1D1;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1C7,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1C7,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1C7,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1C7,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1C9=(void*)e2->r;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_Vardecl*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;int _tmp1CE;void*
_tmp1CF;struct Cyc_List_List*_tmp1D0;_LL14C: if(*((int*)_tmp1C9)!= 26)goto _LL14E;
_tmp1CA=((struct Cyc_Absyn_Array_e_struct*)_tmp1C9)->f1;_LL14D: body=Cyc_Toc_init_array(
nv2,lval,_tmp1CA,Cyc_Toc_skip_stmt_dl());goto _LL14B;_LL14E: if(*((int*)_tmp1C9)!= 
27)goto _LL150;_tmp1CB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C9)->f1;
_tmp1CC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1C9)->f2;_tmp1CD=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1C9)->f3;_tmp1CE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C9)->f4;_LL14F: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1CB,_tmp1CC,
_tmp1CD,_tmp1CE,Cyc_Toc_skip_stmt_dl(),0);goto _LL14B;_LL150: if(*((int*)_tmp1C9)
!= 29)goto _LL152;_tmp1CF=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C9)->f1;
_tmp1D0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C9)->f2;_LL151: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1CF,_tmp1D0,Cyc_Toc_skip_stmt_dl());goto _LL14B;_LL152:;_LL153: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL14B;_LL14B:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Absyn_cast_exp(
_tmp1C8,Cyc_Absyn_uint_exp(0,0),0),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1C7,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1D3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1D3 != 0;_tmp1D3=_tmp1D3->tl){struct _tuple6 _tmp1D5;
struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;struct _tuple6*_tmp1D4=(
struct _tuple6*)_tmp1D3->hd;_tmp1D5=*_tmp1D4;_tmp1D6=_tmp1D5.f1;_tmp1D7=_tmp1D5.f2;
if(_tmp1D6 == 0)({void*_tmp1D8[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D9="empty designator list";
_tag_arr(_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,22));}),_tag_arr(
_tmp1D8,sizeof(void*),0));});if(_tmp1D6->tl != 0)({void*_tmp1DA[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1DB="too many designators in anonymous struct";
_tag_arr(_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,41));}),_tag_arr(
_tmp1DA,sizeof(void*),0));});{void*_tmp1DC=(void*)_tmp1D6->hd;struct _tagged_arr*
_tmp1DD;_LL155: if(*((int*)_tmp1DC)!= 1)goto _LL157;_tmp1DD=((struct Cyc_Absyn_FieldName_struct*)
_tmp1DC)->f1;_LL156: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1DD,0);{void*_tmp1DE=(void*)_tmp1D7->r;struct Cyc_List_List*_tmp1DF;struct Cyc_Absyn_Vardecl*
_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;int _tmp1E3;void*
_tmp1E4;struct Cyc_List_List*_tmp1E5;_LL15A: if(*((int*)_tmp1DE)!= 26)goto _LL15C;
_tmp1DF=((struct Cyc_Absyn_Array_e_struct*)_tmp1DE)->f1;_LL15B: s=Cyc_Toc_init_array(
nv,lval,_tmp1DF,s);goto _LL159;_LL15C: if(*((int*)_tmp1DE)!= 27)goto _LL15E;_tmp1E0=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f1;_tmp1E1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1DE)->f2;_tmp1E2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f3;
_tmp1E3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f4;_LL15D: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1E0,_tmp1E1,_tmp1E2,_tmp1E3,s,0);goto _LL159;_LL15E: if(*((int*)
_tmp1DE)!= 29)goto _LL160;_tmp1E4=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1DE)->f1;_tmp1E5=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1DE)->f2;_LL15F:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1E4,_tmp1E5,s);goto _LL159;_LL160:;_LL161:
Cyc_Toc_exp_to_c(nv,_tmp1D7);if(Cyc_Toc_is_poly_field(struct_type,_tmp1DD))
_tmp1D7=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1D7,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1D7,0),0),s,0);goto _LL159;
_LL159:;}goto _LL154;}_LL157:;_LL158:({void*_tmp1E6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E7="array designator in struct";
_tag_arr(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,27));}),_tag_arr(
_tmp1E6,sizeof(void*),0));});_LL154:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;
_push_region(r);{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1EA=Cyc_Toc_add_tuple_type(
_tmp1E9);struct _tuple1*_tmp1EB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1EC=
Cyc_Absyn_var_exp(_tmp1EB,0);struct Cyc_Absyn_Stmt*_tmp1ED=Cyc_Absyn_exp_stmt(
_tmp1EC,0);struct Cyc_Absyn_Exp*(*_tmp1EE)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1EF=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1EF);for(0;_tmp1EF != 0;(_tmp1EF=_tmp1EF->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1EF->hd;struct Cyc_Absyn_Exp*
lval=_tmp1EE(_tmp1EC,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v): 0;{void*_tmp1F0=(void*)e->r;
struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Vardecl*_tmp1F2;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;int _tmp1F5;_LL163: if(*((int*)_tmp1F0)!= 26)
goto _LL165;_tmp1F1=((struct Cyc_Absyn_Array_e_struct*)_tmp1F0)->f1;_LL164: _tmp1ED=
Cyc_Toc_init_array(nv,lval,_tmp1F1,_tmp1ED);goto _LL162;_LL165: if(*((int*)_tmp1F0)
!= 27)goto _LL167;_tmp1F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1F0)->f1;
_tmp1F3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1F0)->f2;_tmp1F4=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1F0)->f3;_tmp1F5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1F0)->f4;_LL166: _tmp1ED=Cyc_Toc_init_comprehension(nv,lval,_tmp1F2,_tmp1F3,
_tmp1F4,_tmp1F5,_tmp1ED,0);goto _LL162;_LL167:;_LL168: Cyc_Toc_exp_to_c(nv,e);
_tmp1ED=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1EE(
_tmp1EC,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1ED,0);goto _LL162;_LL162:;}}}{
struct Cyc_Absyn_Exp*_tmp1F6=Cyc_Toc_make_struct(nv,_tmp1EB,Cyc_Absyn_strct(
_tmp1EA),_tmp1ED,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1F6;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1F7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1F8=Cyc_Absyn_var_exp(_tmp1F7,0);struct Cyc_Absyn_Stmt*
_tmp1F9=Cyc_Absyn_exp_stmt(_tmp1F8,0);struct Cyc_Absyn_Exp*(*_tmp1FA)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1FB=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1FC;void*
_tmp1FD;_LL16A: if(_tmp1FB <= (void*)3?1:*((int*)_tmp1FB)!= 10)goto _LL16C;_tmp1FC=((
struct Cyc_Absyn_AggrType_struct*)_tmp1FB)->f1;_tmp1FD=(void*)_tmp1FC.aggr_info;
_LL16B: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1FD);goto _LL169;_LL16C:;_LL16D:({void*
_tmp1FE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FF="init_struct: bad struct type";_tag_arr(_tmp1FF,sizeof(char),
_get_zero_arr_size(_tmp1FF,29));}),_tag_arr(_tmp1FE,sizeof(void*),0));});_LL169:;}{
struct _RegionHandle _tmp200=_new_region("r");struct _RegionHandle*r=& _tmp200;
_push_region(r);{struct Cyc_List_List*_tmp201=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp201 != 0;
_tmp201=_tmp201->tl){struct _tuple6 _tmp203;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Exp*
_tmp205;struct _tuple6*_tmp202=(struct _tuple6*)_tmp201->hd;_tmp203=*_tmp202;
_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp205->topt))->v): 0;if(_tmp204 == 0)({
void*_tmp206[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp207="empty designator list";_tag_arr(_tmp207,sizeof(char),
_get_zero_arr_size(_tmp207,22));}),_tag_arr(_tmp206,sizeof(void*),0));});if(
_tmp204->tl != 0){struct _tuple1*_tmp208=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp209=Cyc_Absyn_var_exp(_tmp208,0);for(0;_tmp204 != 0;_tmp204=_tmp204->tl){void*
_tmp20A=(void*)_tmp204->hd;struct _tagged_arr*_tmp20B;_LL16F: if(*((int*)_tmp20A)
!= 1)goto _LL171;_tmp20B=((struct Cyc_Absyn_FieldName_struct*)_tmp20A)->f1;_LL170:
if(Cyc_Toc_is_poly_field(struct_type,_tmp20B))_tmp209=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp209,0);_tmp1F9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1FA(_tmp1F8,_tmp20B,0),_tmp209,0),0),_tmp1F9,0);goto _LL16E;_LL171:;_LL172:({
void*_tmp20C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp20D="array designator in struct";_tag_arr(_tmp20D,sizeof(char),
_get_zero_arr_size(_tmp20D,27));}),_tag_arr(_tmp20C,sizeof(void*),0));});_LL16E:;}
Cyc_Toc_exp_to_c(nv,_tmp205);_tmp1F9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp209,_tmp205,0),0),_tmp1F9,0);}else{void*_tmp20E=(void*)_tmp204->hd;struct
_tagged_arr*_tmp20F;_LL174: if(*((int*)_tmp20E)!= 1)goto _LL176;_tmp20F=((struct
Cyc_Absyn_FieldName_struct*)_tmp20E)->f1;_LL175: {struct Cyc_Absyn_Exp*lval=
_tmp1FA(_tmp1F8,_tmp20F,0);{void*_tmp210=(void*)_tmp205->r;struct Cyc_List_List*
_tmp211;struct Cyc_Absyn_Vardecl*_tmp212;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*
_tmp214;int _tmp215;void*_tmp216;struct Cyc_List_List*_tmp217;_LL179: if(*((int*)
_tmp210)!= 26)goto _LL17B;_tmp211=((struct Cyc_Absyn_Array_e_struct*)_tmp210)->f1;
_LL17A: _tmp1F9=Cyc_Toc_init_array(nv,lval,_tmp211,_tmp1F9);goto _LL178;_LL17B: if(*((
int*)_tmp210)!= 27)goto _LL17D;_tmp212=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp210)->f1;_tmp213=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp210)->f2;
_tmp214=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp210)->f3;_tmp215=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp210)->f4;_LL17C: _tmp1F9=Cyc_Toc_init_comprehension(
nv,lval,_tmp212,_tmp213,_tmp214,_tmp215,_tmp1F9,0);goto _LL178;_LL17D: if(*((int*)
_tmp210)!= 29)goto _LL17F;_tmp216=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp210)->f1;_tmp217=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp210)->f2;_LL17E:
_tmp1F9=Cyc_Toc_init_anon_struct(nv,lval,_tmp216,_tmp217,_tmp1F9);goto _LL178;
_LL17F:;_LL180: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp205->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp205);if(Cyc_Toc_is_poly_field(
struct_type,_tmp20F)?!was_ptr_type: 0)_tmp205=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp205,0);if(has_exists)_tmp205=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp20F)))->type,_tmp205,0);_tmp1F9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp205,0),0),
_tmp1F9,0);goto _LL178;}_LL178:;}goto _LL173;}_LL176:;_LL177:({void*_tmp218[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp219="array designator in struct";_tag_arr(_tmp219,sizeof(char),
_get_zero_arr_size(_tmp219,27));}),_tag_arr(_tmp218,sizeof(void*),0));});_LL173:;}}}{
struct Cyc_Absyn_Exp*_tmp21A=Cyc_Toc_make_struct(nv,_tmp1F7,Cyc_Absyn_strctq(tdn),
_tmp1F9,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp21A;};_pop_region(r);}}
struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));
_tmp21B[0]=({struct Cyc_Absyn_Increment_e_struct _tmp21C;_tmp21C.tag=5;_tmp21C.f1=
e1;_tmp21C.f2=(void*)incr;_tmp21C;});_tmp21B;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp21D=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp21E;void*_tmp21F;struct Cyc_Absyn_Exp*
_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct _tagged_arr*_tmp222;_LL182: if(*((int*)
_tmp21D)!= 35)goto _LL184;_tmp21E=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp21D)->f1;
_LL183: Cyc_Toc_lvalue_assign_stmt(_tmp21E,fs,f,f_env);goto _LL181;_LL184: if(*((
int*)_tmp21D)!= 13)goto _LL186;_tmp21F=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp21D)->f1;_tmp220=((struct Cyc_Absyn_Cast_e_struct*)_tmp21D)->f2;_LL185: Cyc_Toc_lvalue_assign(
_tmp220,fs,f,f_env);goto _LL181;_LL186: if(*((int*)_tmp21D)!= 21)goto _LL188;
_tmp221=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp21D)->f1;_tmp222=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp21D)->f2;_LL187:(void*)(e1->r=(void*)((void*)_tmp221->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp223=_cycalloc(sizeof(*_tmp223));
_tmp223->hd=_tmp222;_tmp223->tl=fs;_tmp223;}),f,f_env);goto _LL181;_LL188:;_LL189: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL181;}_LL181:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp224=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Decl*_tmp226;struct Cyc_Absyn_Stmt*
_tmp227;struct Cyc_Absyn_Stmt*_tmp228;_LL18B: if(_tmp224 <= (void*)1?1:*((int*)
_tmp224)!= 0)goto _LL18D;_tmp225=((struct Cyc_Absyn_Exp_s_struct*)_tmp224)->f1;
_LL18C: Cyc_Toc_lvalue_assign(_tmp225,fs,f,f_env);goto _LL18A;_LL18D: if(_tmp224 <= (
void*)1?1:*((int*)_tmp224)!= 12)goto _LL18F;_tmp226=((struct Cyc_Absyn_Decl_s_struct*)
_tmp224)->f1;_tmp227=((struct Cyc_Absyn_Decl_s_struct*)_tmp224)->f2;_LL18E: Cyc_Toc_lvalue_assign_stmt(
_tmp227,fs,f,f_env);goto _LL18A;_LL18F: if(_tmp224 <= (void*)1?1:*((int*)_tmp224)!= 
1)goto _LL191;_tmp228=((struct Cyc_Absyn_Seq_s_struct*)_tmp224)->f2;_LL190: Cyc_Toc_lvalue_assign_stmt(
_tmp228,fs,f,f_env);goto _LL18A;_LL191:;_LL192:({struct Cyc_String_pa_struct
_tmp22B;_tmp22B.tag=0;_tmp22B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_stmt2string(
s));{void*_tmp229[1]={& _tmp22B};Cyc_Toc_toc_impos(({const char*_tmp22A="lvalue_assign_stmt: %s";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,23));}),_tag_arr(
_tmp229,sizeof(void*),1));}});_LL18A:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp22C=_region_malloc(r2,sizeof(*_tmp22C));_tmp22C->hd=(
void*)f((void*)x->hd,env);_tmp22C->tl=0;_tmp22C;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp22D=_region_malloc(r2,sizeof(*_tmp22D));_tmp22D->hd=(void*)f((void*)x->hd,
env);_tmp22D->tl=0;_tmp22D;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->f1=0;_tmp22E->f2=e;_tmp22E;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp22F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp230;_LL194: if(_tmp22F <= (void*)3?1:*((int*)_tmp22F)
!= 4)goto _LL196;_tmp230=((struct Cyc_Absyn_PointerType_struct*)_tmp22F)->f1;
_LL195: return _tmp230;_LL196:;_LL197:({void*_tmp231[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp232="get_ptr_typ: not a pointer!";
_tag_arr(_tmp232,sizeof(char),_get_zero_arr_size(_tmp232,28));}),_tag_arr(
_tmp231,sizeof(void*),0));});_LL193:;}static int Cyc_Toc_is_zero_ptr_type(void*t,
void**ptr_type,int*is_fat,void**elt_type){void*_tmp233=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp234;void*_tmp235;struct Cyc_Absyn_PtrAtts _tmp236;
struct Cyc_Absyn_Conref*_tmp237;struct Cyc_Absyn_Conref*_tmp238;struct Cyc_Absyn_ArrayInfo
_tmp239;void*_tmp23A;struct Cyc_Absyn_Tqual _tmp23B;struct Cyc_Absyn_Exp*_tmp23C;
struct Cyc_Absyn_Conref*_tmp23D;_LL199: if(_tmp233 <= (void*)3?1:*((int*)_tmp233)!= 
4)goto _LL19B;_tmp234=((struct Cyc_Absyn_PointerType_struct*)_tmp233)->f1;_tmp235=(
void*)_tmp234.elt_typ;_tmp236=_tmp234.ptr_atts;_tmp237=_tmp236.bounds;_tmp238=
_tmp236.zero_term;_LL19A: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp238)){*ptr_type=t;*elt_type=_tmp235;{void*_tmp23E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp237);_LL1A0: if((int)_tmp23E != 0)goto _LL1A2;_LL1A1:*is_fat=1;goto _LL19F;
_LL1A2:;_LL1A3:*is_fat=0;goto _LL19F;_LL19F:;}return 1;}else{return 0;}_LL19B: if(
_tmp233 <= (void*)3?1:*((int*)_tmp233)!= 7)goto _LL19D;_tmp239=((struct Cyc_Absyn_ArrayType_struct*)
_tmp233)->f1;_tmp23A=(void*)_tmp239.elt_type;_tmp23B=_tmp239.tq;_tmp23C=_tmp239.num_elts;
_tmp23D=_tmp239.zero_term;_LL19C: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp23D)){*elt_type=_tmp23A;*is_fat=0;*ptr_type=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp240;_tmp240.tag=4;_tmp240.f1=({struct Cyc_Absyn_PtrInfo _tmp241;_tmp241.elt_typ=(
void*)_tmp23A;_tmp241.elt_tq=_tmp23B;_tmp241.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp242;_tmp242.rgn=(void*)((void*)2);_tmp242.nullable=Cyc_Absyn_false_conref;
_tmp242.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp244;_tmp244.tag=0;_tmp244.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp23C);
_tmp244;});_tmp243;}));_tmp242.zero_term=_tmp23D;_tmp242;});_tmp241;});_tmp240;});
_tmp23F;});return 1;}else{return 0;}_LL19D:;_LL19E: return 0;_LL198:;}static int Cyc_Toc_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){void*_tmp245=(
void*)e1->r;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Exp*
_tmp248;struct Cyc_Absyn_Exp*_tmp249;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_Absyn_Exp*
_tmp24B;_LL1A5: if(*((int*)_tmp245)!= 13)goto _LL1A7;_LL1A6:({struct Cyc_String_pa_struct
_tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e1));{void*_tmp24C[1]={& _tmp24E};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp24D="we have a cast in a lhs:  %s";
_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,29));}),_tag_arr(
_tmp24C,sizeof(void*),1));}});_LL1A7: if(*((int*)_tmp245)!= 20)goto _LL1A9;_tmp246=((
struct Cyc_Absyn_Deref_e_struct*)_tmp245)->f1;_LL1A8: _tmp247=_tmp246;goto _LL1AA;
_LL1A9: if(*((int*)_tmp245)!= 23)goto _LL1AB;_tmp247=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp245)->f1;_LL1AA: return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp247->topt))->v,ptr_type,is_fat,elt_type);_LL1AB: if(*((int*)
_tmp245)!= 22)goto _LL1AD;_tmp248=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp245)->f1;
_LL1AC: _tmp249=_tmp248;goto _LL1AE;_LL1AD: if(*((int*)_tmp245)!= 21)goto _LL1AF;
_tmp249=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp245)->f1;_LL1AE: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp249->topt))->v,ptr_type,is_fat,
elt_type))({struct Cyc_String_pa_struct _tmp251;_tmp251.tag=0;_tmp251.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{void*_tmp24F[1]={&
_tmp251};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp250="found zero pointer aggregate member assignment: %s";_tag_arr(
_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,51));}),_tag_arr(_tmp24F,sizeof(
void*),1));}});return 0;_LL1AF: if(*((int*)_tmp245)!= 12)goto _LL1B1;_tmp24A=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp245)->f1;_LL1B0: _tmp24B=_tmp24A;goto
_LL1B2;_LL1B1: if(*((int*)_tmp245)!= 11)goto _LL1B3;_tmp24B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp245)->f1;_LL1B2: if(Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp24B->topt))->v,ptr_type,is_fat,elt_type))({struct Cyc_String_pa_struct
_tmp254;_tmp254.tag=0;_tmp254.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(
e1));{void*_tmp252[1]={& _tmp254};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp253="found zero pointer instantiate/noinstantiate: %s";
_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,49));}),_tag_arr(
_tmp252,sizeof(void*),1));}});return 0;_LL1B3: if(*((int*)_tmp245)!= 1)goto _LL1B5;
_LL1B4: return 0;_LL1B5:;_LL1B6:({struct Cyc_String_pa_struct _tmp257;_tmp257.tag=0;
_tmp257.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e1));{
void*_tmp255[1]={& _tmp257};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(({const char*_tmp256="found bad lhs in is_zero_ptr_deref: %s";
_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,39));}),_tag_arr(
_tmp255,sizeof(void*),1));}});_LL1A4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(
void*t){struct Cyc_Absyn_Exp*res;{void*_tmp258=Cyc_Tcutil_compress(t);void*
_tmp259;void*_tmp25A;void*_tmp25B;void*_tmp25C;void*_tmp25D;void*_tmp25E;void*
_tmp25F;void*_tmp260;_LL1B8: if(_tmp258 <= (void*)3?1:*((int*)_tmp258)!= 4)goto
_LL1BA;_LL1B9: res=Cyc_Absyn_null_exp(0);goto _LL1B7;_LL1BA: if(_tmp258 <= (void*)3?
1:*((int*)_tmp258)!= 5)goto _LL1BC;_tmp259=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp258)->f1;_tmp25A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f2;if((
int)_tmp25A != 0)goto _LL1BC;_LL1BB: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=({struct Cyc_Absyn_Char_c_struct
_tmp262;_tmp262.tag=0;_tmp262.f1=(void*)_tmp259;_tmp262.f2='\000';_tmp262;});
_tmp261;}),0);goto _LL1B7;_LL1BC: if(_tmp258 <= (void*)3?1:*((int*)_tmp258)!= 5)
goto _LL1BE;_tmp25B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f1;_tmp25C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f2;if((int)_tmp25C != 1)goto
_LL1BE;_LL1BD: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Short_c_struct*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Absyn_Short_c_struct
_tmp264;_tmp264.tag=1;_tmp264.f1=(void*)_tmp25B;_tmp264.f2=0;_tmp264;});_tmp263;}),
0);goto _LL1B7;_LL1BE: if(_tmp258 <= (void*)3?1:*((int*)_tmp258)!= 12)goto _LL1C0;
_LL1BF: goto _LL1C1;_LL1C0: if(_tmp258 <= (void*)3?1:*((int*)_tmp258)!= 13)goto
_LL1C2;_LL1C1: _tmp25D=(void*)1;goto _LL1C3;_LL1C2: if(_tmp258 <= (void*)3?1:*((int*)
_tmp258)!= 5)goto _LL1C4;_tmp25D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f1;
_tmp25E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f2;if((int)_tmp25E != 
2)goto _LL1C4;_LL1C3: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_Int_c_struct
_tmp266;_tmp266.tag=2;_tmp266.f1=(void*)_tmp25D;_tmp266.f2=0;_tmp266;});_tmp265;}),
0);goto _LL1B7;_LL1C4: if(_tmp258 <= (void*)3?1:*((int*)_tmp258)!= 5)goto _LL1C6;
_tmp25F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f1;_tmp260=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp258)->f2;if((int)_tmp260 != 3)goto _LL1C6;
_LL1C5: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_LongLong_c_struct*
_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_LongLong_c_struct
_tmp268;_tmp268.tag=3;_tmp268.f1=(void*)_tmp25F;_tmp268.f2=(long long)0;_tmp268;});
_tmp267;}),0);goto _LL1B7;_LL1C6: if((int)_tmp258 != 1)goto _LL1C8;_LL1C7: goto _LL1C9;
_LL1C8: if(_tmp258 <= (void*)3?1:*((int*)_tmp258)!= 6)goto _LL1CA;_LL1C9: res=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp269=_cycalloc(sizeof(*_tmp269));
_tmp269[0]=({struct Cyc_Absyn_Float_c_struct _tmp26A;_tmp26A.tag=4;_tmp26A.f1=({
const char*_tmp26B="0.0";_tag_arr(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,
4));});_tmp26A;});_tmp269;}),0);goto _LL1B7;_LL1CA:;_LL1CB:({struct Cyc_String_pa_struct
_tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp26C[1]={& _tmp26E};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(({const char*_tmp26D="found non-zero type %s in generate_zero";
_tag_arr(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,40));}),_tag_arr(
_tmp26C,sizeof(void*),1));}});_LL1B7:;}res->topt=({struct Cyc_Core_Opt*_tmp26F=
_cycalloc(sizeof(*_tmp26F));_tmp26F->v=(void*)t;_tmp26F;});return res;}static void
Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,
int is_fat,void*elt_type){void*_tmp270=Cyc_Absyn_tagged_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);void*_tmp271=Cyc_Toc_typ_to_c(elt_type);void*_tmp272=Cyc_Toc_typ_to_c(
_tmp270);void*_tmp273=Cyc_Absyn_cstar_typ(_tmp271,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp274=({struct Cyc_Core_Opt*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->v=(
void*)_tmp273;_tmp2AE;});struct Cyc_Absyn_Exp*xinit;{void*_tmp275=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*
_tmp278;_LL1CD: if(*((int*)_tmp275)!= 20)goto _LL1CF;_tmp276=((struct Cyc_Absyn_Deref_e_struct*)
_tmp275)->f1;_LL1CE: if(!is_fat)_tmp276=Cyc_Absyn_cast_exp(_tmp270,_tmp276,0);
_tmp276->topt=({struct Cyc_Core_Opt*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->v=(
void*)_tmp270;_tmp279;});Cyc_Toc_exp_to_c(nv,_tmp276);xinit=_tmp276;goto _LL1CC;
_LL1CF: if(*((int*)_tmp275)!= 23)goto _LL1D1;_tmp277=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp275)->f1;_tmp278=((struct Cyc_Absyn_Subscript_e_struct*)_tmp275)->f2;_LL1D0:
if(!is_fat)_tmp277=Cyc_Absyn_cast_exp(_tmp270,_tmp277,0);_tmp277->topt=({struct
Cyc_Core_Opt*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->v=(void*)_tmp270;
_tmp27A;});Cyc_Toc_exp_to_c(nv,_tmp277);Cyc_Toc_exp_to_c(nv,_tmp278);xinit=Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp27B[3];_tmp27B[2]=_tmp278;
_tmp27B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp27B[0]=
_tmp277;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp27B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);goto _LL1CC;_LL1D1:;_LL1D2:({void*
_tmp27C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp27D="found bad lhs for zero-terminated pointer assignment";
_tag_arr(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,53));}),_tag_arr(
_tmp27C,sizeof(void*),0));});_LL1CC:;}{struct _tuple1*_tmp27E=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp27E,Cyc_Absyn_var_exp(_tmp27E,0));{struct Cyc_Absyn_Vardecl*
_tmp27F=({struct Cyc_Absyn_Vardecl*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->sc=(
void*)((void*)2);_tmp2AD->name=_tmp27E;_tmp2AD->tq=Cyc_Toc_mt_tq;_tmp2AD->type=(
void*)_tmp272;_tmp2AD->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2AD->rgn=0;
_tmp2AD->attributes=0;_tmp2AD->escapes=0;_tmp2AD;});struct Cyc_Absyn_Local_b_struct*
_tmp280=({struct Cyc_Absyn_Local_b_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));
_tmp2AB[0]=({struct Cyc_Absyn_Local_b_struct _tmp2AC;_tmp2AC.tag=3;_tmp2AC.f1=
_tmp27F;_tmp2AC;});_tmp2AB;});struct Cyc_Absyn_Exp*_tmp281=Cyc_Absyn_varb_exp(
_tmp27E,(void*)_tmp280,0);_tmp281->topt=({struct Cyc_Core_Opt*_tmp282=_cycalloc(
sizeof(*_tmp282));_tmp282->v=(void*)_tmp270;_tmp282;});{struct Cyc_Absyn_Exp*
_tmp283=Cyc_Absyn_deref_exp(_tmp281,0);_tmp283->topt=({struct Cyc_Core_Opt*
_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->v=(void*)elt_type;_tmp284;});Cyc_Toc_exp_to_c(
nv,_tmp283);{struct _tuple1*_tmp285=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp285,Cyc_Absyn_var_exp(_tmp285,0));{struct Cyc_Absyn_Vardecl*_tmp286=({struct
Cyc_Absyn_Vardecl*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->sc=(void*)((void*)
2);_tmp2AA->name=_tmp285;_tmp2AA->tq=Cyc_Toc_mt_tq;_tmp2AA->type=(void*)_tmp271;
_tmp2AA->initializer=(struct Cyc_Absyn_Exp*)_tmp283;_tmp2AA->rgn=0;_tmp2AA->attributes=
0;_tmp2AA->escapes=0;_tmp2AA;});struct Cyc_Absyn_Local_b_struct*_tmp287=({struct
Cyc_Absyn_Local_b_struct*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2A9;_tmp2A9.tag=3;_tmp2A9.f1=_tmp286;_tmp2A9;});
_tmp2A8;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp288=Cyc_Absyn_varb_exp(_tmp285,(void*)_tmp287,0);_tmp288->topt=_tmp283->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp288,e2,0);z_init->topt=_tmp288->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp289=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp28A=({struct Cyc_Absyn_Vardecl*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->sc=(
void*)((void*)2);_tmp2A7->name=_tmp289;_tmp2A7->tq=Cyc_Toc_mt_tq;_tmp2A7->type=(
void*)_tmp271;_tmp2A7->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2A7->rgn=0;
_tmp2A7->attributes=0;_tmp2A7->escapes=0;_tmp2A7;});struct Cyc_Absyn_Local_b_struct*
_tmp28B=({struct Cyc_Absyn_Local_b_struct*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));
_tmp2A5[0]=({struct Cyc_Absyn_Local_b_struct _tmp2A6;_tmp2A6.tag=3;_tmp2A6.f1=
_tmp28A;_tmp2A6;});_tmp2A5;});nv=Cyc_Toc_add_varmap(nv,_tmp289,Cyc_Absyn_var_exp(
_tmp289,0));{struct Cyc_Absyn_Exp*_tmp28C=Cyc_Absyn_varb_exp(_tmp285,(void*)
_tmp287,0);_tmp28C->topt=_tmp283->topt;{struct Cyc_Absyn_Exp*_tmp28D=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp28E=Cyc_Absyn_prim2_exp((void*)5,_tmp28C,
_tmp28D,0);_tmp28E->topt=({struct Cyc_Core_Opt*_tmp28F=_cycalloc(sizeof(*_tmp28F));
_tmp28F->v=(void*)Cyc_Absyn_sint_typ;_tmp28F;});Cyc_Toc_exp_to_c(nv,_tmp28E);{
struct Cyc_Absyn_Exp*_tmp290=Cyc_Absyn_varb_exp(_tmp289,(void*)_tmp28B,0);_tmp290->topt=
_tmp283->topt;{struct Cyc_Absyn_Exp*_tmp291=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp292=Cyc_Absyn_prim2_exp((void*)6,_tmp290,_tmp291,0);
_tmp292->topt=({struct Cyc_Core_Opt*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->v=(
void*)Cyc_Absyn_sint_typ;_tmp293;});Cyc_Toc_exp_to_c(nv,_tmp292);{struct Cyc_Absyn_Exp*
_tmp294=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp2A4[2];_tmp2A4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2A4[0]=Cyc_Absyn_varb_exp(_tmp27E,(void*)_tmp280,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2A4,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_uint_exp(1,0),0);struct Cyc_Absyn_Exp*_tmp295=Cyc_Absyn_and_exp(
_tmp294,Cyc_Absyn_and_exp(_tmp28E,_tmp292,0),0);struct Cyc_Absyn_Stmt*_tmp296=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*
_tmp297=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp27E,(void*)_tmp280,0),
Cyc_Toc_curr_sp,0);_tmp297=Cyc_Absyn_cast_exp(_tmp273,_tmp297,0);{struct Cyc_Absyn_Exp*
_tmp298=Cyc_Absyn_deref_exp(_tmp297,0);struct Cyc_Absyn_Exp*_tmp299=Cyc_Absyn_assign_exp(
_tmp298,Cyc_Absyn_var_exp(_tmp289,0),0);struct Cyc_Absyn_Stmt*_tmp29A=Cyc_Absyn_exp_stmt(
_tmp299,0);_tmp29A=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp295,_tmp296,
Cyc_Absyn_skip_stmt(0),0),_tmp29A,0);_tmp29A=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*
_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C[0]=({struct Cyc_Absyn_Var_d_struct
_tmp29D;_tmp29D.tag=0;_tmp29D.f1=_tmp28A;_tmp29D;});_tmp29C;}));_tmp29B->loc=0;
_tmp29B;}),_tmp29A,0);_tmp29A=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp29E=
_cycalloc(sizeof(*_tmp29E));_tmp29E->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=_tmp286;_tmp2A0;});_tmp29F;}));_tmp29E->loc=0;
_tmp29E;}),_tmp29A,0);_tmp29A=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2A1=
_cycalloc(sizeof(*_tmp2A1));_tmp2A1->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=_tmp27F;_tmp2A3;});_tmp2A2;}));_tmp2A1->loc=0;
_tmp2A1;}),_tmp29A,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp29A,0))->r));}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2AF=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp2B0[1]={&
_tmp2B2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2B1="exp_to_c: no type for %s";_tag_arr(_tmp2B1,sizeof(char),
_get_zero_arr_size(_tmp2B1,25));}),_tag_arr(_tmp2B0,sizeof(void*),1));}});{void*
old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp2B3=
_tmp2AF;void*_tmp2B4;struct _tuple1*_tmp2B5;void*_tmp2B6;struct _tuple1*_tmp2B7;
void*_tmp2B8;struct Cyc_List_List*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;void*
_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Core_Opt*_tmp2BD;struct Cyc_Absyn_Exp*
_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*
_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*
_tmp2C4;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_List_List*
_tmp2C7;struct Cyc_Absyn_VarargCallInfo*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C9;
struct Cyc_List_List*_tmp2CA;struct Cyc_Absyn_VarargCallInfo*_tmp2CB;struct Cyc_Absyn_VarargCallInfo
_tmp2CC;int _tmp2CD;struct Cyc_List_List*_tmp2CE;struct Cyc_Absyn_VarargInfo*
_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*
_tmp2D2;struct Cyc_List_List*_tmp2D3;void*_tmp2D4;void**_tmp2D5;struct Cyc_Absyn_Exp*
_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*
_tmp2D9;struct Cyc_Absyn_Exp*_tmp2DA;void*_tmp2DB;void*_tmp2DC;void*_tmp2DD;
struct _tagged_arr*_tmp2DE;void*_tmp2DF;void*_tmp2E0;unsigned int _tmp2E1;struct
Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;struct _tagged_arr*_tmp2E4;
struct Cyc_Absyn_Exp*_tmp2E5;struct _tagged_arr*_tmp2E6;struct Cyc_Absyn_Exp*
_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List*
_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_Absyn_Exp*
_tmp2ED;int _tmp2EE;struct _tuple1*_tmp2EF;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*
_tmp2F1;struct Cyc_Absyn_Aggrdecl*_tmp2F2;void*_tmp2F3;struct Cyc_List_List*
_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_Absyn_Tuniondecl*_tmp2F6;struct Cyc_Absyn_Tunionfield*
_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_Absyn_Tuniondecl*_tmp2F9;struct Cyc_Absyn_Tunionfield*
_tmp2FA;struct Cyc_Absyn_MallocInfo _tmp2FB;int _tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;
void**_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;int _tmp300;struct Cyc_Absyn_Stmt*
_tmp301;struct Cyc_Absyn_Fndecl*_tmp302;_LL1D4: if(*((int*)_tmp2B3)!= 0)goto _LL1D6;
_tmp2B4=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2B3)->f1;if((int)_tmp2B4 != 
0)goto _LL1D6;_LL1D5: {struct Cyc_Absyn_Exp*_tmp303=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){if(nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(
old_typ,_tmp303,_tmp303))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp304[3];_tmp304[2]=_tmp303;_tmp304[1]=
_tmp303;_tmp304[0]=_tmp303;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp304,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1D3;}_LL1D6: if(*((int*)
_tmp2B3)!= 0)goto _LL1D8;_LL1D7: goto _LL1D3;_LL1D8: if(*((int*)_tmp2B3)!= 1)goto
_LL1DA;_tmp2B5=((struct Cyc_Absyn_Var_e_struct*)_tmp2B3)->f1;_tmp2B6=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2B3)->f2;_LL1D9:{struct _handler_cons _tmp305;
_push_handler(& _tmp305);{int _tmp307=0;if(setjmp(_tmp305.handler))_tmp307=1;if(!
_tmp307){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2B5))->r));;_pop_handler();}
else{void*_tmp306=(void*)_exn_thrown;void*_tmp309=_tmp306;_LL229: if(_tmp309 != 
Cyc_Dict_Absent)goto _LL22B;_LL22A:({struct Cyc_String_pa_struct _tmp30C;_tmp30C.tag=
0;_tmp30C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp2B5));{void*_tmp30A[1]={& _tmp30C};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp30B="Can't find %s in exp_to_c, Var\n";
_tag_arr(_tmp30B,sizeof(char),_get_zero_arr_size(_tmp30B,32));}),_tag_arr(
_tmp30A,sizeof(void*),1));}});_LL22B:;_LL22C:(void)_throw(_tmp309);_LL228:;}}}
goto _LL1D3;_LL1DA: if(*((int*)_tmp2B3)!= 2)goto _LL1DC;_tmp2B7=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp2B3)->f1;_LL1DB:({void*_tmp30D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp30E="unknownid";_tag_arr(
_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,10));}),_tag_arr(_tmp30D,sizeof(
void*),0));});_LL1DC: if(*((int*)_tmp2B3)!= 3)goto _LL1DE;_tmp2B8=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp2B3)->f1;_tmp2B9=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B3)->f2;_LL1DD: {struct Cyc_List_List*_tmp30F=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp2B9);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2B9);{void*
_tmp310=_tmp2B8;_LL22E: if((int)_tmp310 != 19)goto _LL230;_LL22F: {struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B9))->hd;{void*
_tmp311=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);
struct Cyc_Absyn_ArrayInfo _tmp312;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_Absyn_PtrInfo
_tmp314;void*_tmp315;struct Cyc_Absyn_PtrAtts _tmp316;struct Cyc_Absyn_Conref*
_tmp317;struct Cyc_Absyn_Conref*_tmp318;struct Cyc_Absyn_Conref*_tmp319;_LL243: if(
_tmp311 <= (void*)3?1:*((int*)_tmp311)!= 7)goto _LL245;_tmp312=((struct Cyc_Absyn_ArrayType_struct*)
_tmp311)->f1;_tmp313=_tmp312.num_elts;_LL244:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp313))->r));goto _LL242;_LL245: if(_tmp311 <= (
void*)3?1:*((int*)_tmp311)!= 4)goto _LL247;_tmp314=((struct Cyc_Absyn_PointerType_struct*)
_tmp311)->f1;_tmp315=(void*)_tmp314.elt_typ;_tmp316=_tmp314.ptr_atts;_tmp317=
_tmp316.nullable;_tmp318=_tmp316.bounds;_tmp319=_tmp316.zero_term;_LL246:{void*
_tmp31A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp318);struct Cyc_Absyn_Exp*
_tmp31B;_LL24A: if((int)_tmp31A != 0)goto _LL24C;_LL24B:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({struct Cyc_Absyn_Exp*_tmp31C[2];
_tmp31C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp315),0);_tmp31C[0]=(
struct Cyc_Absyn_Exp*)_tmp2B9->hd;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp31C,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));
goto _LL249;_LL24C: if(_tmp31A <= (void*)1?1:*((int*)_tmp31A)!= 0)goto _LL24E;
_tmp31B=((struct Cyc_Absyn_Upper_b_struct*)_tmp31A)->f1;_LL24D: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp319))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp31D[2];_tmp31D[1]=_tmp31B;_tmp31D[0]=(struct Cyc_Absyn_Exp*)_tmp2B9->hd;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp31D,
sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp317))(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(
arg,_tmp31B,Cyc_Absyn_uint_exp(0,0),0))->r));else{(void*)(e->r=(void*)((void*)
_tmp31B->r));goto _LL249;}}goto _LL249;_LL24E: if(_tmp31A <= (void*)1?1:*((int*)
_tmp31A)!= 1)goto _LL249;_LL24F:({void*_tmp31E[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp31F="toc: size of AbsUpper_b";
_tag_arr(_tmp31F,sizeof(char),_get_zero_arr_size(_tmp31F,24));}),_tag_arr(
_tmp31E,sizeof(void*),0));});_LL249:;}goto _LL242;_LL247:;_LL248:({struct Cyc_String_pa_struct
_tmp323;_tmp323.tag=0;_tmp323.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp322;_tmp322.tag=0;_tmp322.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp320[2]={&
_tmp322,& _tmp323};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp321="size primop applied to non-array %s (%s)";_tag_arr(_tmp321,
sizeof(char),_get_zero_arr_size(_tmp321,41));}),_tag_arr(_tmp320,sizeof(void*),2));}}});
_LL242:;}goto _LL22D;}_LL230: if((int)_tmp310 != 0)goto _LL232;_LL231:{void*_tmp324=
Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd);
struct Cyc_Absyn_PtrInfo _tmp325;void*_tmp326;struct Cyc_Absyn_PtrAtts _tmp327;
struct Cyc_Absyn_Conref*_tmp328;struct Cyc_Absyn_Conref*_tmp329;_LL251: if(_tmp324
<= (void*)3?1:*((int*)_tmp324)!= 4)goto _LL253;_tmp325=((struct Cyc_Absyn_PointerType_struct*)
_tmp324)->f1;_tmp326=(void*)_tmp325.elt_typ;_tmp327=_tmp325.ptr_atts;_tmp328=
_tmp327.bounds;_tmp329=_tmp327.zero_term;_LL252:{void*_tmp32A=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp328);struct Cyc_Absyn_Exp*_tmp32B;_LL256: if((int)_tmp32A
!= 0)goto _LL258;_LL257: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B9))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B9->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp32C[3];
_tmp32C[2]=e2;_tmp32C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp326),0);
_tmp32C[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp32C,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL255;}_LL258:
if(_tmp32A <= (void*)1?1:*((int*)_tmp32A)!= 0)goto _LL25A;_tmp32B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp32A)->f1;_LL259: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp329)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2B9))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2B9->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp32D[3];_tmp32D[2]=e2;_tmp32D[
1]=_tmp32B;_tmp32D[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp32D,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL255;_LL25A:
if(_tmp32A <= (void*)1?1:*((int*)_tmp32A)!= 1)goto _LL255;_LL25B:({void*_tmp32E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp32F="toc: plus on AbsUpper_b";_tag_arr(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,24));}),_tag_arr(_tmp32E,sizeof(void*),0));});_LL255:;}
goto _LL250;_LL253:;_LL254: goto _LL250;_LL250:;}goto _LL22D;_LL232: if((int)_tmp310
!= 2)goto _LL234;_LL233: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2B9))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2B9->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp30F->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp330[3];_tmp330[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp330[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp330[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp330,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL22D;}_LL234: if((int)_tmp310 != 5)goto _LL236;_LL235: goto _LL237;
_LL236: if((int)_tmp310 != 6)goto _LL238;_LL237: goto _LL239;_LL238: if((int)_tmp310 != 
7)goto _LL23A;_LL239: goto _LL23B;_LL23A: if((int)_tmp310 != 9)goto _LL23C;_LL23B: goto
_LL23D;_LL23C: if((int)_tmp310 != 8)goto _LL23E;_LL23D: goto _LL23F;_LL23E: if((int)
_tmp310 != 10)goto _LL240;_LL23F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B9))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2B9->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp30F))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp30F->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL22D;}_LL240:;_LL241: goto _LL22D;_LL22D:;}goto _LL1D3;}_LL1DE: if(*((int*)
_tmp2B3)!= 5)goto _LL1E0;_tmp2BA=((struct Cyc_Absyn_Increment_e_struct*)_tmp2B3)->f1;
_tmp2BB=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp2B3)->f2;_LL1DF: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BA->topt))->v;void*
ptr_type=(void*)0;void*elt_type=(void*)0;int fat_ptr=0;struct _tagged_arr incr_str=({
const char*_tmp33D="increment";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,10));});if(_tmp2BB == (void*)2?1: _tmp2BB == (void*)3)incr_str=({const char*
_tmp331="decrement";_tag_arr(_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,10));});
if(Cyc_Toc_is_zero_ptr_deref(_tmp2BA,& ptr_type,& fat_ptr,& elt_type)){({struct Cyc_String_pa_struct
_tmp334;_tmp334.tag=0;_tmp334.f1=(struct _tagged_arr)((struct _tagged_arr)incr_str);{
void*_tmp332[1]={& _tmp334};Cyc_Tcutil_terr(e->loc,({const char*_tmp333="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_arr(_tmp333,sizeof(char),_get_zero_arr_size(_tmp333,74));}),_tag_arr(
_tmp332,sizeof(void*),1));}});(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335[0]=({struct Cyc_Core_Impossible_struct
_tmp336;_tmp336.tag=Cyc_Core_Impossible;_tmp336.f1=({const char*_tmp337="in-place inc on zero-term";
_tag_arr(_tmp337,sizeof(char),_get_zero_arr_size(_tmp337,26));});_tmp336;});
_tmp335;}));}Cyc_Toc_exp_to_c(nv,_tmp2BA);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp338=(_tmp2BB == (void*)1?1: _tmp2BB == (
void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp339=(_tmp2BB == (void*)0?1: _tmp2BB == (void*)1)?1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp338,({struct Cyc_Absyn_Exp*_tmp33A[3];_tmp33A[2]=
Cyc_Absyn_signed_int_exp(_tmp339,0);_tmp33A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp33A[0]=Cyc_Absyn_address_exp(_tmp2BA,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp33A,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*_tmp33B=_tmp2BB == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp33B,({struct Cyc_Absyn_Exp*_tmp33C[2];_tmp33C[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp33C[0]=_tmp2BA;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp33C,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(elt_typ == (
void*)0?!Cyc_Absyn_is_lvalue(_tmp2BA): 0){Cyc_Toc_lvalue_assign(_tmp2BA,0,Cyc_Toc_incr_lvalue,
_tmp2BB);(void*)(e->r=(void*)((void*)_tmp2BA->r));}}}goto _LL1D3;}}_LL1E0: if(*((
int*)_tmp2B3)!= 4)goto _LL1E2;_tmp2BC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2B3)->f1;_tmp2BD=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2B3)->f2;_tmp2BE=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2B3)->f3;_LL1E1: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_fat=0;if(Cyc_Toc_is_zero_ptr_deref(_tmp2BC,&
ptr_type,& is_fat,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp2BC,_tmp2BD,
_tmp2BE,ptr_type,is_fat,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp2BC);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BC->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BE->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp2BC);Cyc_Toc_exp_to_c(nv,_tmp2BE);{int done=0;if(_tmp2BD != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp33E=(void*)_tmp2BD->v;_LL25D: if((int)_tmp33E != 0)goto _LL25F;
_LL25E: change=_tmp2BE;goto _LL25C;_LL25F: if((int)_tmp33E != 2)goto _LL261;_LL260:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp2BE,0);goto _LL25C;_LL261:;_LL262:({void*
_tmp33F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp340="bad t ? pointer arithmetic";_tag_arr(_tmp340,sizeof(char),
_get_zero_arr_size(_tmp340,27));}),_tag_arr(_tmp33F,sizeof(void*),0));});_LL25C:;}
done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp341[3];_tmp341[2]=change;_tmp341[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp341[0]=Cyc_Absyn_address_exp(_tmp2BC,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp341,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp342=(void*)_tmp2BD->v;_LL264: if((int)_tmp342 != 0)
goto _LL266;_LL265: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp343[2];_tmp343[1]=_tmp2BE;_tmp343[0]=_tmp2BC;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp343,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL263;_LL266:;_LL267:({void*_tmp344[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp345="bad zero-terminated pointer arithmetic";
_tag_arr(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,39));}),_tag_arr(
_tmp344,sizeof(void*),0));});_LL263:;}}}if(!done){if(e1_poly)(void*)(_tmp2BE->r=(
void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp2BE->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp2BC)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(_tmp2BC,0,Cyc_Toc_assignop_lvalue,({
struct _tuple9*_tmp346=_cycalloc(sizeof(struct _tuple9)* 1);_tmp346[0]=({struct
_tuple9 _tmp347;_tmp347.f1=_tmp2BD;_tmp347.f2=_tmp2BE;_tmp347;});_tmp346;}));(
void*)(e->r=(void*)((void*)_tmp2BC->r));}}goto _LL1D3;}}}_LL1E2: if(*((int*)
_tmp2B3)!= 6)goto _LL1E4;_tmp2BF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2B3)->f1;
_tmp2C0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2B3)->f2;_tmp2C1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2B3)->f3;_LL1E3: Cyc_Toc_exp_to_c(nv,_tmp2BF);Cyc_Toc_exp_to_c(nv,_tmp2C0);
Cyc_Toc_exp_to_c(nv,_tmp2C1);goto _LL1D3;_LL1E4: if(*((int*)_tmp2B3)!= 7)goto
_LL1E6;_tmp2C2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2B3)->f1;_tmp2C3=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp2B3)->f2;_LL1E5: Cyc_Toc_exp_to_c(nv,_tmp2C2);Cyc_Toc_exp_to_c(
nv,_tmp2C3);goto _LL1D3;_LL1E6: if(*((int*)_tmp2B3)!= 8)goto _LL1E8;_tmp2C4=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2B3)->f1;_tmp2C5=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp2B3)->f2;_LL1E7: _tmp2C6=_tmp2C4;_tmp2C7=_tmp2C5;goto _LL1E9;_LL1E8: if(*((int*)
_tmp2B3)!= 9)goto _LL1EA;_tmp2C6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2B3)->f1;
_tmp2C7=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2B3)->f2;_tmp2C8=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2B3)->f3;if(_tmp2C8 != 0)goto _LL1EA;_LL1E9: Cyc_Toc_exp_to_c(nv,_tmp2C6);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2C7);goto _LL1D3;
_LL1EA: if(*((int*)_tmp2B3)!= 9)goto _LL1EC;_tmp2C9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2B3)->f1;_tmp2CA=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2B3)->f2;_tmp2CB=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2B3)->f3;if(_tmp2CB == 0)goto _LL1EC;_tmp2CC=*
_tmp2CB;_tmp2CD=_tmp2CC.num_varargs;_tmp2CE=_tmp2CC.injectors;_tmp2CF=_tmp2CC.vai;
_LL1EB:{struct _RegionHandle _tmp348=_new_region("r");struct _RegionHandle*r=&
_tmp348;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp2CD,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2CF->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2CA);int num_normargs=num_args - _tmp2CD;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp2CA=_tmp2CA->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2CA))->hd);
new_args=({struct Cyc_List_List*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->hd=(
struct Cyc_Absyn_Exp*)_tmp2CA->hd;_tmp349->tl=new_args;_tmp349;});}}new_args=({
struct Cyc_List_List*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp34B[3];_tmp34B[2]=num_varargs_exp;
_tmp34B[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp34B[0]=argvexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp34B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);_tmp34A->tl=new_args;_tmp34A;});new_args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp2C9);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp2C9,new_args,
0),0);if(_tmp2CF->inject){struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp34C=Cyc_Tcutil_compress((
void*)_tmp2CF->type);struct Cyc_Absyn_TunionInfo _tmp34D;void*_tmp34E;struct Cyc_Absyn_Tuniondecl**
_tmp34F;struct Cyc_Absyn_Tuniondecl*_tmp350;_LL269: if(_tmp34C <= (void*)3?1:*((int*)
_tmp34C)!= 2)goto _LL26B;_tmp34D=((struct Cyc_Absyn_TunionType_struct*)_tmp34C)->f1;
_tmp34E=(void*)_tmp34D.tunion_info;if(*((int*)_tmp34E)!= 1)goto _LL26B;_tmp34F=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp34E)->f1;_tmp350=*_tmp34F;_LL26A: tud=
_tmp350;goto _LL268;_LL26B:;_LL26C:({void*_tmp351[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp352="toc: unknown tunion in vararg with inject";
_tag_arr(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,42));}),_tag_arr(
_tmp351,sizeof(void*),0));});_LL268:;}{struct _tagged_arr vs=({unsigned int _tmp35A=(
unsigned int)_tmp2CD;struct _tuple1**_tmp35B=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp35A));struct _tagged_arr _tmp35D=_tag_arr(
_tmp35B,sizeof(struct _tuple1*),_tmp35A);{unsigned int _tmp35C=_tmp35A;
unsigned int i;for(i=0;i < _tmp35C;i ++){_tmp35B[i]=Cyc_Toc_temp_var();}}_tmp35D;});
struct Cyc_List_List*_tmp353=0;{int i=_tmp2CD - 1;for(0;i >= 0;-- i){_tmp353=({struct
Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->hd=Cyc_Toc_make_dle(
Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp354->tl=
_tmp353;_tmp354;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp353,0),s,0);{int i=0;for(0;_tmp2CA != 0;(((
_tmp2CA=_tmp2CA->tl,_tmp2CE=_tmp2CE->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp2CA->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp356;struct _tuple1*
_tmp357;struct Cyc_List_List*_tmp358;struct Cyc_Absyn_Tunionfield*_tmp355=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2CE))->hd;_tmp356=*
_tmp355;_tmp357=_tmp356.name;_tmp358=_tmp356.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp358))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(field_typ,arg,
0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,
Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp357,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp357,({const char*_tmp359="_struct";_tag_arr(_tmp359,sizeof(char),
_get_zero_arr_size(_tmp359,8));}))),0,s,0);}}}}}else{{int i=0;for(0;_tmp2CA != 0;(
_tmp2CA=_tmp2CA->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp2CA->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((
unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2CA->hd,0),s,0);}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,0,s,0);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};
_pop_region(r);}goto _LL1D3;_LL1EC: if(*((int*)_tmp2B3)!= 10)goto _LL1EE;_tmp2D0=((
struct Cyc_Absyn_Throw_e_struct*)_tmp2B3)->f1;_LL1ED: Cyc_Toc_exp_to_c(nv,_tmp2D0);(
void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),
Cyc_Toc_newthrow_exp(_tmp2D0),0))->r));goto _LL1D3;_LL1EE: if(*((int*)_tmp2B3)!= 
11)goto _LL1F0;_tmp2D1=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp2B3)->f1;
_LL1EF: Cyc_Toc_exp_to_c(nv,_tmp2D1);goto _LL1D3;_LL1F0: if(*((int*)_tmp2B3)!= 12)
goto _LL1F2;_tmp2D2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2B3)->f1;_tmp2D3=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp2B3)->f2;_LL1F1: Cyc_Toc_exp_to_c(nv,
_tmp2D2);for(0;_tmp2D3 != 0;_tmp2D3=_tmp2D3->tl){void*k=Cyc_Tcutil_typ_kind((void*)
_tmp2D3->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp35E=Cyc_Tcutil_compress((
void*)_tmp2D3->hd);_LL26E: if(_tmp35E <= (void*)3?1:*((int*)_tmp35E)!= 1)goto
_LL270;_LL26F: goto _LL271;_LL270: if(_tmp35E <= (void*)3?1:*((int*)_tmp35E)!= 2)
goto _LL272;_LL271: continue;_LL272:;_LL273:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D2,
0))->r));goto _LL26D;_LL26D:;}break;}}goto _LL1D3;_LL1F2: if(*((int*)_tmp2B3)!= 13)
goto _LL1F4;_tmp2D4=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2B3)->f1;_tmp2D5=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2B3)->f1);_tmp2D6=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2B3)->f2;_LL1F3: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D6->topt))->v;void*new_typ=*_tmp2D5;*_tmp2D5=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp2D6);{struct _tuple0 _tmp360=({struct _tuple0
_tmp35F;_tmp35F.f1=Cyc_Tcutil_compress(old_t2);_tmp35F.f2=Cyc_Tcutil_compress(
new_typ);_tmp35F;});void*_tmp361;struct Cyc_Absyn_PtrInfo _tmp362;void*_tmp363;
struct Cyc_Absyn_PtrInfo _tmp364;void*_tmp365;struct Cyc_Absyn_PtrInfo _tmp366;void*
_tmp367;_LL275: _tmp361=_tmp360.f1;if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 4)
goto _LL277;_tmp362=((struct Cyc_Absyn_PointerType_struct*)_tmp361)->f1;_tmp363=
_tmp360.f2;if(_tmp363 <= (void*)3?1:*((int*)_tmp363)!= 4)goto _LL277;_tmp364=((
struct Cyc_Absyn_PointerType_struct*)_tmp363)->f1;_LL276: {int _tmp368=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp362.ptr_atts).nullable);
int _tmp369=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp364.ptr_atts).nullable);void*_tmp36A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp362.ptr_atts).bounds);void*_tmp36B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp364.ptr_atts).bounds);int _tmp36C=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp362.ptr_atts).zero_term);int _tmp36D=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp364.ptr_atts).zero_term);{struct _tuple0 _tmp36F=({
struct _tuple0 _tmp36E;_tmp36E.f1=_tmp36A;_tmp36E.f2=_tmp36B;_tmp36E;});void*
_tmp370;struct Cyc_Absyn_Exp*_tmp371;void*_tmp372;struct Cyc_Absyn_Exp*_tmp373;
void*_tmp374;struct Cyc_Absyn_Exp*_tmp375;void*_tmp376;void*_tmp377;void*_tmp378;
struct Cyc_Absyn_Exp*_tmp379;void*_tmp37A;void*_tmp37B;void*_tmp37C;void*_tmp37D;
_LL27C: _tmp370=_tmp36F.f1;if(_tmp370 <= (void*)1?1:*((int*)_tmp370)!= 0)goto
_LL27E;_tmp371=((struct Cyc_Absyn_Upper_b_struct*)_tmp370)->f1;_tmp372=_tmp36F.f2;
if(_tmp372 <= (void*)1?1:*((int*)_tmp372)!= 0)goto _LL27E;_tmp373=((struct Cyc_Absyn_Upper_b_struct*)
_tmp372)->f1;_LL27D: if(_tmp368?!_tmp369: 0){if(nv->toplevel)({void*_tmp37E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp37F="can't do NULL-check conversion at top-level";_tag_arr(_tmp37F,sizeof(
char),_get_zero_arr_size(_tmp37F,44));}),_tag_arr(_tmp37E,sizeof(void*),0));});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*_tmp2D5,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->hd=_tmp2D6;
_tmp380->tl=0;_tmp380;}),0),0))->r));}goto _LL27B;_LL27E: _tmp374=_tmp36F.f1;if(
_tmp374 <= (void*)1?1:*((int*)_tmp374)!= 0)goto _LL280;_tmp375=((struct Cyc_Absyn_Upper_b_struct*)
_tmp374)->f1;_tmp376=_tmp36F.f2;if((int)_tmp376 != 0)goto _LL280;_LL27F: if(nv->toplevel){
if((_tmp36C?!(_tmp364.elt_tq).q_const: 0)?!_tmp36D: 0)_tmp375=Cyc_Absyn_prim2_exp((
void*)2,_tmp375,Cyc_Absyn_uint_exp(1,0),0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(
old_t2,_tmp375,_tmp2D6))->r));}else{if(_tmp36C){struct _tuple1*_tmp381=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp382=Cyc_Absyn_var_exp(_tmp381,0);struct Cyc_Absyn_Exp*
_tmp383=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp388[2];_tmp388[1]=_tmp375;_tmp388[0]=_tmp382;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp388,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp36D?!(_tmp364.elt_tq).q_const: 0)_tmp383=Cyc_Absyn_prim2_exp((
void*)2,_tmp383,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp384=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp364.elt_typ),0);struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp387[3];_tmp387[2]=_tmp383;_tmp387[1]=
_tmp384;_tmp387[0]=_tmp382;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp387,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp386=Cyc_Absyn_exp_stmt(_tmp385,0);_tmp386=Cyc_Absyn_declare_stmt(_tmp381,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2D6,_tmp386,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp386,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp389[3];_tmp389[2]=_tmp375;_tmp389[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp364.elt_typ),0);_tmp389[0]=
_tmp2D6;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp389,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL27B;_LL280: _tmp377=
_tmp36F.f1;if((int)_tmp377 != 0)goto _LL282;_tmp378=_tmp36F.f2;if(_tmp378 <= (void*)
1?1:*((int*)_tmp378)!= 0)goto _LL282;_tmp379=((struct Cyc_Absyn_Upper_b_struct*)
_tmp378)->f1;_LL281: if(nv->toplevel)({void*_tmp38A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp38B="can't coerce t? to t* or t@ at the top-level";
_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,45));}),_tag_arr(
_tmp38A,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp38C=_tmp379;if(_tmp36C?!
_tmp36D: 0)_tmp38C=Cyc_Absyn_add_exp(_tmp379,Cyc_Absyn_uint_exp(1,0),0);{struct
Cyc_Absyn_Exp*_tmp38D=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*
_tmp38F[3];_tmp38F[2]=_tmp38C;_tmp38F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp362.elt_typ),0);_tmp38F[0]=_tmp2D6;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp38F,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp369)(void*)(_tmp38D->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->hd=Cyc_Absyn_copy_exp(
_tmp38D);_tmp38E->tl=0;_tmp38E;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp2D5,_tmp38D,0))->r));goto _LL27B;}}_LL282: _tmp37A=_tmp36F.f1;if((int)_tmp37A
!= 0)goto _LL284;_tmp37B=_tmp36F.f2;if((int)_tmp37B != 0)goto _LL284;_LL283: if((
_tmp36C?!_tmp36D: 0)?!(_tmp364.elt_tq).q_const: 0){if(nv->toplevel)({void*_tmp390[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const
char*_tmp391="can't coerce a ?ZEROTERM to a non-const ?NOZEROTERM at toplevel";
_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,64));}),_tag_arr(
_tmp390,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_ptr_decrease_size_e,({struct Cyc_Absyn_Exp*_tmp392[3];_tmp392[2]=
Cyc_Absyn_uint_exp(1,0);_tmp392[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp362.elt_typ),0);_tmp392[0]=_tmp2D6;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp392,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL27B;_LL284: _tmp37C=_tmp36F.f1;if(_tmp37C <= (void*)1?1:*((int*)
_tmp37C)!= 1)goto _LL286;_LL285: goto _LL287;_LL286: _tmp37D=_tmp36F.f2;if(_tmp37D <= (
void*)1?1:*((int*)_tmp37D)!= 1)goto _LL27B;_LL287:({void*_tmp393[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp394="toc, cast, AbsUpper_b";
_tag_arr(_tmp394,sizeof(char),_get_zero_arr_size(_tmp394,22));}),_tag_arr(
_tmp393,sizeof(void*),0));});_LL27B:;}goto _LL274;}_LL277: _tmp365=_tmp360.f1;if(
_tmp365 <= (void*)3?1:*((int*)_tmp365)!= 4)goto _LL279;_tmp366=((struct Cyc_Absyn_PointerType_struct*)
_tmp365)->f1;_tmp367=_tmp360.f2;if(_tmp367 <= (void*)3?1:*((int*)_tmp367)!= 5)
goto _LL279;_LL278:{void*_tmp395=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp366.ptr_atts).bounds);_LL289: if((int)_tmp395 != 0)goto _LL28B;_LL28A:(void*)(
_tmp2D6->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)
_tmp2D6->r,_tmp2D6->loc),Cyc_Toc_curr_sp,0))->r));goto _LL288;_LL28B:;_LL28C: goto
_LL288;_LL288:;}goto _LL274;_LL279:;_LL27A: goto _LL274;_LL274:;}goto _LL1D3;}_LL1F4:
if(*((int*)_tmp2B3)!= 14)goto _LL1F6;_tmp2D7=((struct Cyc_Absyn_Address_e_struct*)
_tmp2B3)->f1;_LL1F5:{void*_tmp396=(void*)_tmp2D7->r;struct _tuple1*_tmp397;struct
Cyc_List_List*_tmp398;struct Cyc_List_List*_tmp399;struct Cyc_List_List*_tmp39A;
_LL28E: if(*((int*)_tmp396)!= 28)goto _LL290;_tmp397=((struct Cyc_Absyn_Struct_e_struct*)
_tmp396)->f1;_tmp398=((struct Cyc_Absyn_Struct_e_struct*)_tmp396)->f2;_tmp399=((
struct Cyc_Absyn_Struct_e_struct*)_tmp396)->f3;_LL28F: if(nv->toplevel)({struct Cyc_String_pa_struct
_tmp39D;_tmp39D.tag=0;_tmp39D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D7->loc));{void*_tmp39B[1]={& _tmp39D};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp39C="%s: & on non-identifiers at the top-level";
_tag_arr(_tmp39C,sizeof(char),_get_zero_arr_size(_tmp39C,42));}),_tag_arr(
_tmp39B,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D7->topt))->v,_tmp398 != 0,1,0,
_tmp399,_tmp397))->r));goto _LL28D;_LL290: if(*((int*)_tmp396)!= 24)goto _LL292;
_tmp39A=((struct Cyc_Absyn_Tuple_e_struct*)_tmp396)->f1;_LL291: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3A0;_tmp3A0.tag=0;_tmp3A0.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp2D7->loc));{void*_tmp39E[1]={&
_tmp3A0};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp39F="%s: & on non-identifiers at the top-level";_tag_arr(_tmp39F,
sizeof(char),_get_zero_arr_size(_tmp39F,42));}),_tag_arr(_tmp39E,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp39A))->r));goto _LL28D;
_LL292:;_LL293: Cyc_Toc_exp_to_c(nv,_tmp2D7);if(!Cyc_Absyn_is_lvalue(_tmp2D7)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp2D7,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2D7,0))->r));}goto _LL28D;
_LL28D:;}goto _LL1D3;_LL1F6: if(*((int*)_tmp2B3)!= 15)goto _LL1F8;_tmp2D8=((struct
Cyc_Absyn_New_e_struct*)_tmp2B3)->f1;_tmp2D9=((struct Cyc_Absyn_New_e_struct*)
_tmp2B3)->f2;_LL1F7: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp3A3;_tmp3A3.tag=
0;_tmp3A3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp2D9->loc));{void*_tmp3A1[1]={& _tmp3A3};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp3A2="%s: new at top-level";
_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,21));}),_tag_arr(
_tmp3A1,sizeof(void*),1));}});{void*_tmp3A4=(void*)_tmp2D9->r;struct Cyc_List_List*
_tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Exp*
_tmp3A8;int _tmp3A9;struct _tuple1*_tmp3AA;struct Cyc_List_List*_tmp3AB;struct Cyc_List_List*
_tmp3AC;struct Cyc_List_List*_tmp3AD;_LL295: if(*((int*)_tmp3A4)!= 26)goto _LL297;
_tmp3A5=((struct Cyc_Absyn_Array_e_struct*)_tmp3A4)->f1;_LL296: {struct _tuple1*
_tmp3AE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Absyn_var_exp(_tmp3AE,
0);struct Cyc_Absyn_Stmt*_tmp3B0=Cyc_Toc_init_array(nv,_tmp3AF,_tmp3A5,Cyc_Absyn_exp_stmt(
_tmp3AF,0));void*old_elt_typ;{void*_tmp3B1=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_PtrInfo _tmp3B2;void*_tmp3B3;struct Cyc_Absyn_Tqual _tmp3B4;struct Cyc_Absyn_PtrAtts
_tmp3B5;struct Cyc_Absyn_Conref*_tmp3B6;_LL2A0: if(_tmp3B1 <= (void*)3?1:*((int*)
_tmp3B1)!= 4)goto _LL2A2;_tmp3B2=((struct Cyc_Absyn_PointerType_struct*)_tmp3B1)->f1;
_tmp3B3=(void*)_tmp3B2.elt_typ;_tmp3B4=_tmp3B2.elt_tq;_tmp3B5=_tmp3B2.ptr_atts;
_tmp3B6=_tmp3B5.zero_term;_LL2A1: old_elt_typ=_tmp3B3;goto _LL29F;_LL2A2:;_LL2A3:
old_elt_typ=({void*_tmp3B7[0]={};Cyc_Toc_toc_impos(({const char*_tmp3B8="exp_to_c:new array expression doesn't have ptr type";
_tag_arr(_tmp3B8,sizeof(char),_get_zero_arr_size(_tmp3B8,52));}),_tag_arr(
_tmp3B7,sizeof(void*),0));});_LL29F:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp3B9=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp3BA=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3A5),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp2D8 == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp3BA);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D8);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(
r,_tmp3BA);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp3AE,_tmp3B9,(struct Cyc_Absyn_Exp*)e1,_tmp3B0,0),0))->r));goto _LL294;}}_LL297:
if(*((int*)_tmp3A4)!= 27)goto _LL299;_tmp3A6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3A4)->f1;_tmp3A7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3A4)->f2;
_tmp3A8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3A4)->f3;_tmp3A9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp3A4)->f4;_LL298: {int is_tagged_ptr=0;{void*
_tmp3BB=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp3BC;void*_tmp3BD;
struct Cyc_Absyn_Tqual _tmp3BE;struct Cyc_Absyn_PtrAtts _tmp3BF;struct Cyc_Absyn_Conref*
_tmp3C0;struct Cyc_Absyn_Conref*_tmp3C1;_LL2A5: if(_tmp3BB <= (void*)3?1:*((int*)
_tmp3BB)!= 4)goto _LL2A7;_tmp3BC=((struct Cyc_Absyn_PointerType_struct*)_tmp3BB)->f1;
_tmp3BD=(void*)_tmp3BC.elt_typ;_tmp3BE=_tmp3BC.elt_tq;_tmp3BF=_tmp3BC.ptr_atts;
_tmp3C0=_tmp3BF.bounds;_tmp3C1=_tmp3BF.zero_term;_LL2A6: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3C0)== (void*)0;goto _LL2A4;_LL2A7:;_LL2A8:({void*
_tmp3C2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3C3="exp_to_c: comprehension not an array type";_tag_arr(_tmp3C3,
sizeof(char),_get_zero_arr_size(_tmp3C3,42));}),_tag_arr(_tmp3C2,sizeof(void*),0));});
_LL2A4:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A8->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp3A7);{struct Cyc_Absyn_Exp*_tmp3C4=
Cyc_Absyn_var_exp(max,0);if(_tmp3A9)_tmp3C4=Cyc_Absyn_add_exp(_tmp3C4,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp3A6,Cyc_Absyn_var_exp(max,0),_tmp3A8,_tmp3A9,Cyc_Toc_skip_stmt_dl(),1);{
struct _RegionHandle _tmp3C5=_new_region("r");struct _RegionHandle*r=& _tmp3C5;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp3D5=
_region_malloc(r,sizeof(*_tmp3D5));_tmp3D5->hd=({struct _tuple10*_tmp3D6=
_region_malloc(r,sizeof(*_tmp3D6));_tmp3D6->f1=max;_tmp3D6->f2=Cyc_Absyn_uint_typ;
_tmp3D6->f3=(struct Cyc_Absyn_Exp*)_tmp3A7;_tmp3D6;});_tmp3D5->tl=0;_tmp3D5;});
struct Cyc_Absyn_Exp*ai;if(_tmp2D8 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3C6[2];_tmp3C6[1]=_tmp3C4;
_tmp3C6[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3C6,sizeof(struct Cyc_Absyn_Exp*),2));}),
0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D8);Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp3C7[2];_tmp3C7[1]=_tmp3C4;_tmp3C7[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3C7,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(
ptr_typ,ai,0);decls=({struct Cyc_List_List*_tmp3C8=_region_malloc(r,sizeof(*
_tmp3C8));_tmp3C8->hd=({struct _tuple10*_tmp3C9=_region_malloc(r,sizeof(*_tmp3C9));
_tmp3C9->f1=a;_tmp3C9->f2=ptr_typ;_tmp3C9->f3=(struct Cyc_Absyn_Exp*)ainit;
_tmp3C9;});_tmp3C8->tl=decls;_tmp3C8;});if(is_tagged_ptr){struct _tuple1*_tmp3CA=
Cyc_Toc_temp_var();void*_tmp3CB=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*
_tmp3CC=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp3CF[3];
_tmp3CF[2]=_tmp3C4;_tmp3CF[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp3CF[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp3CF,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp3CD=_region_malloc(r,sizeof(*_tmp3CD));_tmp3CD->hd=({struct _tuple10*_tmp3CE=
_region_malloc(r,sizeof(*_tmp3CE));_tmp3CE->f1=_tmp3CA;_tmp3CE->f2=_tmp3CB;
_tmp3CE->f3=(struct Cyc_Absyn_Exp*)_tmp3CC;_tmp3CE;});_tmp3CD->tl=decls;_tmp3CD;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp3CA,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp3D0=decls;for(0;_tmp3D0 != 0;_tmp3D0=_tmp3D0->tl){struct
_tuple1*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;struct _tuple10 _tmp3D1=*((
struct _tuple10*)_tmp3D0->hd);_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;_tmp3D4=
_tmp3D1.f3;s=Cyc_Absyn_declare_stmt(_tmp3D2,_tmp3D3,_tmp3D4,s,0);}}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL294;}}}}
_LL299: if(*((int*)_tmp3A4)!= 28)goto _LL29B;_tmp3AA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3A4)->f1;_tmp3AB=((struct Cyc_Absyn_Struct_e_struct*)_tmp3A4)->f2;_tmp3AC=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3A4)->f3;_LL29A:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D9->topt))->v,
_tmp3AB != 0,1,_tmp2D8,_tmp3AC,_tmp3AA))->r));goto _LL294;_LL29B: if(*((int*)
_tmp3A4)!= 24)goto _LL29D;_tmp3AD=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3A4)->f1;
_LL29C:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp2D8,_tmp3AD))->r));
goto _LL294;_LL29D:;_LL29E: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D9->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp3D7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3D8=Cyc_Absyn_var_exp(
_tmp3D7,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp3D8,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp2D8 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp2D8);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp3D9=(void*)_tmp2D9->r;void*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;_LL2AA:
if(*((int*)_tmp3D9)!= 13)goto _LL2AC;_tmp3DA=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3D9)->f1;_tmp3DB=((struct Cyc_Absyn_Cast_e_struct*)_tmp3D9)->f2;_LL2AB:{
struct _tuple0 _tmp3DD=({struct _tuple0 _tmp3DC;_tmp3DC.f1=Cyc_Tcutil_compress(
_tmp3DA);_tmp3DC.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3DB->topt))->v);_tmp3DC;});void*_tmp3DE;struct Cyc_Absyn_PtrInfo _tmp3DF;void*
_tmp3E0;struct Cyc_Absyn_PtrAtts _tmp3E1;struct Cyc_Absyn_Conref*_tmp3E2;void*
_tmp3E3;struct Cyc_Absyn_PtrInfo _tmp3E4;struct Cyc_Absyn_PtrAtts _tmp3E5;struct Cyc_Absyn_Conref*
_tmp3E6;_LL2AF: _tmp3DE=_tmp3DD.f1;if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 4)
goto _LL2B1;_tmp3DF=((struct Cyc_Absyn_PointerType_struct*)_tmp3DE)->f1;_tmp3E0=(
void*)_tmp3DF.elt_typ;_tmp3E1=_tmp3DF.ptr_atts;_tmp3E2=_tmp3E1.bounds;_tmp3E3=
_tmp3DD.f2;if(_tmp3E3 <= (void*)3?1:*((int*)_tmp3E3)!= 4)goto _LL2B1;_tmp3E4=((
struct Cyc_Absyn_PointerType_struct*)_tmp3E3)->f1;_tmp3E5=_tmp3E4.ptr_atts;
_tmp3E6=_tmp3E5.bounds;_LL2B0:{struct _tuple0 _tmp3E8=({struct _tuple0 _tmp3E7;
_tmp3E7.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3E2);_tmp3E7.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3E6);_tmp3E7;});void*_tmp3E9;void*_tmp3EA;struct Cyc_Absyn_Exp*
_tmp3EB;_LL2B4: _tmp3E9=_tmp3E8.f1;if((int)_tmp3E9 != 0)goto _LL2B6;_tmp3EA=_tmp3E8.f2;
if(_tmp3EA <= (void*)1?1:*((int*)_tmp3EA)!= 0)goto _LL2B6;_tmp3EB=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3EA)->f1;_LL2B5: Cyc_Toc_exp_to_c(nv,_tmp3DB);(void*)(inner_mexp->r=(void*)((
void*)(Cyc_Absyn_sizeoftyp_exp(elt_typ,0))->r));done=1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({struct Cyc_Absyn_Exp*
_tmp3EC[4];_tmp3EC[3]=_tmp3EB;_tmp3EC[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3E0),0);_tmp3EC[1]=_tmp3DB;_tmp3EC[0]=mexp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp3EC,sizeof(struct Cyc_Absyn_Exp*),4));}),
0))->r));goto _LL2B3;_LL2B6:;_LL2B7: goto _LL2B3;_LL2B3:;}goto _LL2AE;_LL2B1:;_LL2B2:
goto _LL2AE;_LL2AE:;}goto _LL2A9;_LL2AC:;_LL2AD: goto _LL2A9;_LL2A9:;}if(!done){
struct Cyc_Absyn_Stmt*_tmp3ED=Cyc_Absyn_exp_stmt(_tmp3D8,0);struct Cyc_Absyn_Exp*
_tmp3EE=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp2D9);_tmp3ED=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp3D8,_tmp3EE,0),_tmp2D9,0),
_tmp3ED,0);{void*_tmp3EF=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp3D7,_tmp3EF,(struct
Cyc_Absyn_Exp*)mexp,_tmp3ED,0),0))->r));}}goto _LL294;}}_LL294:;}goto _LL1D3;
_LL1F8: if(*((int*)_tmp2B3)!= 17)goto _LL1FA;_tmp2DA=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2B3)->f1;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp2DA);goto _LL1D3;_LL1FA: if(*((int*)
_tmp2B3)!= 16)goto _LL1FC;_tmp2DB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2B3)->f1;_LL1FB:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp3F1;_tmp3F1.tag=16;_tmp3F1.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DB);_tmp3F1;});
_tmp3F0;})));goto _LL1D3;_LL1FC: if(*((int*)_tmp2B3)!= 19)goto _LL1FE;_LL1FD:({void*
_tmp3F2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3F3="__gen() in code generator";_tag_arr(_tmp3F3,sizeof(char),
_get_zero_arr_size(_tmp3F3,26));}),_tag_arr(_tmp3F2,sizeof(void*),0));});_LL1FE:
if(*((int*)_tmp2B3)!= 18)goto _LL200;_tmp2DC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2B3)->f1;_tmp2DD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2B3)->f2;
if(*((int*)_tmp2DD)!= 0)goto _LL200;_tmp2DE=((struct Cyc_Absyn_StructField_struct*)
_tmp2DD)->f1;_LL1FF:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp3F5;_tmp3F5.tag=18;_tmp3F5.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DC);_tmp3F5.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp3F6=_cycalloc(sizeof(*
_tmp3F6));_tmp3F6[0]=({struct Cyc_Absyn_StructField_struct _tmp3F7;_tmp3F7.tag=0;
_tmp3F7.f1=_tmp2DE;_tmp3F7;});_tmp3F6;}));_tmp3F5;});_tmp3F4;})));goto _LL1D3;
_LL200: if(*((int*)_tmp2B3)!= 18)goto _LL202;_tmp2DF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2B3)->f1;_tmp2E0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2B3)->f2;
if(*((int*)_tmp2E0)!= 1)goto _LL202;_tmp2E1=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E0)->f1;_LL201:{void*_tmp3F8=Cyc_Tcutil_compress(_tmp2DF);struct Cyc_Absyn_AggrInfo
_tmp3F9;void*_tmp3FA;struct Cyc_List_List*_tmp3FB;_LL2B9: if(_tmp3F8 <= (void*)3?1:*((
int*)_tmp3F8)!= 10)goto _LL2BB;_tmp3F9=((struct Cyc_Absyn_AggrType_struct*)_tmp3F8)->f1;
_tmp3FA=(void*)_tmp3F9.aggr_info;_LL2BA: {struct Cyc_Absyn_Aggrdecl*_tmp3FC=Cyc_Absyn_get_known_aggrdecl(
_tmp3FA);if(_tmp3FC->impl == 0)({void*_tmp3FD[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp3FE="struct fields must be known";
_tag_arr(_tmp3FE,sizeof(char),_get_zero_arr_size(_tmp3FE,28));}),_tag_arr(
_tmp3FD,sizeof(void*),0));});_tmp3FB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3FC->impl))->fields;goto _LL2BC;}_LL2BB: if(_tmp3F8 <= (void*)3?1:*((int*)
_tmp3F8)!= 11)goto _LL2BD;_tmp3FB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3F8)->f2;
_LL2BC: {struct Cyc_Absyn_Aggrfield*_tmp3FF=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3FB,(int)_tmp2E1);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp400=_cycalloc(sizeof(*
_tmp400));_tmp400[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp401;_tmp401.tag=18;
_tmp401.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DF);_tmp401.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402[0]=({
struct Cyc_Absyn_StructField_struct _tmp403;_tmp403.tag=0;_tmp403.f1=_tmp3FF->name;
_tmp403;});_tmp402;}));_tmp401;});_tmp400;})));goto _LL2B8;}_LL2BD: if(_tmp3F8 <= (
void*)3?1:*((int*)_tmp3F8)!= 9)goto _LL2BF;_LL2BE:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp405;_tmp405.tag=18;_tmp405.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp2DF);_tmp405.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp406=
_cycalloc(sizeof(*_tmp406));_tmp406[0]=({struct Cyc_Absyn_StructField_struct
_tmp407;_tmp407.tag=0;_tmp407.f1=Cyc_Absyn_fieldname((int)(_tmp2E1 + 1));_tmp407;});
_tmp406;}));_tmp405;});_tmp404;})));goto _LL2B8;_LL2BF: if(_tmp3F8 <= (void*)3?1:*((
int*)_tmp3F8)!= 3)goto _LL2C1;_LL2C0: if(_tmp2E1 == 0)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp409;_tmp409.tag=18;_tmp409.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp2DF);_tmp409.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp40A=
_cycalloc(sizeof(*_tmp40A));_tmp40A[0]=({struct Cyc_Absyn_StructField_struct
_tmp40B;_tmp40B.tag=0;_tmp40B.f1=Cyc_Toc_tag_sp;_tmp40B;});_tmp40A;}));_tmp409;});
_tmp408;})));else{(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp40D;_tmp40D.tag=18;_tmp40D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp2DF);_tmp40D.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp40E=_cycalloc(sizeof(*
_tmp40E));_tmp40E[0]=({struct Cyc_Absyn_StructField_struct _tmp40F;_tmp40F.tag=0;
_tmp40F.f1=Cyc_Absyn_fieldname((int)_tmp2E1);_tmp40F;});_tmp40E;}));_tmp40D;});
_tmp40C;})));}goto _LL2B8;_LL2C1:;_LL2C2:({void*_tmp410[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp411="impossible type for offsetof tuple index";
_tag_arr(_tmp411,sizeof(char),_get_zero_arr_size(_tmp411,41));}),_tag_arr(
_tmp410,sizeof(void*),0));});_LL2B8:;}goto _LL1D3;_LL202: if(*((int*)_tmp2B3)!= 20)
goto _LL204;_tmp2E2=((struct Cyc_Absyn_Deref_e_struct*)_tmp2B3)->f1;_LL203: {void*
_tmp412=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E2->topt))->v);{
void*_tmp413=_tmp412;struct Cyc_Absyn_PtrInfo _tmp414;void*_tmp415;struct Cyc_Absyn_Tqual
_tmp416;struct Cyc_Absyn_PtrAtts _tmp417;void*_tmp418;struct Cyc_Absyn_Conref*
_tmp419;struct Cyc_Absyn_Conref*_tmp41A;struct Cyc_Absyn_Conref*_tmp41B;_LL2C4: if(
_tmp413 <= (void*)3?1:*((int*)_tmp413)!= 4)goto _LL2C6;_tmp414=((struct Cyc_Absyn_PointerType_struct*)
_tmp413)->f1;_tmp415=(void*)_tmp414.elt_typ;_tmp416=_tmp414.elt_tq;_tmp417=
_tmp414.ptr_atts;_tmp418=(void*)_tmp417.rgn;_tmp419=_tmp417.nullable;_tmp41A=
_tmp417.bounds;_tmp41B=_tmp417.zero_term;_LL2C5:{void*_tmp41C=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp41A);_LL2C9: if(_tmp41C <= (void*)1?1:*((int*)_tmp41C)!= 
0)goto _LL2CB;_LL2CA: {int do_null_check=Cyc_Toc_need_null_check(_tmp2E2);Cyc_Toc_exp_to_c(
nv,_tmp2E2);if(do_null_check)(void*)(_tmp2E2->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(_tmp412),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->hd=Cyc_Absyn_copy_exp(_tmp2E2);
_tmp41D->tl=0;_tmp41D;}),0),0))->r));goto _LL2C8;}_LL2CB: if((int)_tmp41C != 0)goto
_LL2CD;_LL2CC: {struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_uint_exp(0,0);_tmp41E->topt=({
struct Cyc_Core_Opt*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->v=(void*)Cyc_Absyn_uint_typ;
_tmp41F;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp2E2,_tmp41E,
0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL2C8;}_LL2CD: if(_tmp41C <= (void*)1?1:*((
int*)_tmp41C)!= 1)goto _LL2C8;_LL2CE:({void*_tmp420[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp421="exp_to_c: deref w/ AbsUpper_b";
_tag_arr(_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,30));}),_tag_arr(
_tmp420,sizeof(void*),0));});_LL2C8:;}goto _LL2C3;_LL2C6:;_LL2C7:({void*_tmp422[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*
_tmp423="exp_to_c: Deref: non-pointer";_tag_arr(_tmp423,sizeof(char),
_get_zero_arr_size(_tmp423,29));}),_tag_arr(_tmp422,sizeof(void*),0));});_LL2C3:;}
goto _LL1D3;}_LL204: if(*((int*)_tmp2B3)!= 21)goto _LL206;_tmp2E3=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2B3)->f1;_tmp2E4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2B3)->f2;_LL205:
Cyc_Toc_exp_to_c(nv,_tmp2E3);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1D3;
_LL206: if(*((int*)_tmp2B3)!= 22)goto _LL208;_tmp2E5=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2B3)->f1;_tmp2E6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2B3)->f2;_LL207: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E5->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp2E5);Cyc_Toc_exp_to_c(nv,_tmp2E5);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp425;struct Cyc_Absyn_Tqual _tmp426;
struct Cyc_Absyn_PtrAtts _tmp427;void*_tmp428;struct Cyc_Absyn_Conref*_tmp429;
struct Cyc_Absyn_Conref*_tmp42A;struct Cyc_Absyn_Conref*_tmp42B;struct Cyc_Absyn_PtrInfo
_tmp424=Cyc_Toc_get_ptr_type(e1typ);_tmp425=(void*)_tmp424.elt_typ;_tmp426=
_tmp424.elt_tq;_tmp427=_tmp424.ptr_atts;_tmp428=(void*)_tmp427.rgn;_tmp429=
_tmp427.nullable;_tmp42A=_tmp427.bounds;_tmp42B=_tmp427.zero_term;{void*_tmp42C=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp42A);struct Cyc_Absyn_Exp*_tmp42D;
_LL2D0: if(_tmp42C <= (void*)1?1:*((int*)_tmp42C)!= 0)goto _LL2D2;_tmp42D=((struct
Cyc_Absyn_Upper_b_struct*)_tmp42C)->f1;_LL2D1: {unsigned int _tmp42F;int _tmp430;
struct _tuple5 _tmp42E=Cyc_Evexp_eval_const_uint_exp(_tmp42D);_tmp42F=_tmp42E.f1;
_tmp430=_tmp42E.f2;if(_tmp430){if(_tmp42F < 1)({void*_tmp431[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp432="exp_to_c:  AggrArrow_e on pointer of size 0";
_tag_arr(_tmp432,sizeof(char),_get_zero_arr_size(_tmp432,44));}),_tag_arr(
_tmp431,sizeof(void*),0));});if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E5->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->hd=_tmp2E5;_tmp433->tl=0;_tmp433;}),
0),0),_tmp2E6,0))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E5->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp434[4];_tmp434[3]=Cyc_Absyn_uint_exp(0,0);_tmp434[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp425,0);_tmp434[1]=_tmp42D;_tmp434[0]=_tmp2E5;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp434,sizeof(struct Cyc_Absyn_Exp*),
4));}),0),0),_tmp2E6,0))->r));}goto _LL2CF;}_LL2D2: if((int)_tmp42C != 0)goto _LL2D4;
_LL2D3: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp425);(void*)(_tmp2E5->r=(void*)((
void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp426),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp435[3];_tmp435[2]=
Cyc_Absyn_uint_exp(0,0);_tmp435[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp435[0]=Cyc_Absyn_copy_exp(
_tmp2E5);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp435,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0))->r));goto _LL2CF;}_LL2D4: if(
_tmp42C <= (void*)1?1:*((int*)_tmp42C)!= 1)goto _LL2CF;_LL2D5:({void*_tmp436[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*
_tmp437="exp_to_c: AggrArrow w/ AbsUpper_b";_tag_arr(_tmp437,sizeof(char),
_get_zero_arr_size(_tmp437,34));}),_tag_arr(_tmp436,sizeof(void*),0));});_LL2CF:;}
if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));goto _LL1D3;}}_LL208: if(*((int*)_tmp2B3)!= 23)goto _LL20A;_tmp2E7=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp2B3)->f1;_tmp2E8=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2B3)->f2;_LL209: {void*_tmp438=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E7->topt))->v);{void*_tmp439=_tmp438;struct Cyc_List_List*
_tmp43A;struct Cyc_Absyn_PtrInfo _tmp43B;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;
struct Cyc_Absyn_PtrAtts _tmp43E;void*_tmp43F;struct Cyc_Absyn_Conref*_tmp440;
struct Cyc_Absyn_Conref*_tmp441;struct Cyc_Absyn_Conref*_tmp442;_LL2D7: if(_tmp439
<= (void*)3?1:*((int*)_tmp439)!= 9)goto _LL2D9;_tmp43A=((struct Cyc_Absyn_TupleType_struct*)
_tmp439)->f1;_LL2D8: Cyc_Toc_exp_to_c(nv,_tmp2E7);Cyc_Toc_exp_to_c(nv,_tmp2E8);{
unsigned int _tmp444;int _tmp445;struct _tuple5 _tmp443=Cyc_Evexp_eval_const_uint_exp(
_tmp2E8);_tmp444=_tmp443.f1;_tmp445=_tmp443.f2;if(!_tmp445)({void*_tmp446[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp447="unknown tuple subscript in translation to C";_tag_arr(_tmp447,sizeof(
char),_get_zero_arr_size(_tmp447,44));}),_tag_arr(_tmp446,sizeof(void*),0));});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(_tmp2E7,Cyc_Absyn_fieldname((
int)(_tmp444 + 1)),0))->r));goto _LL2D6;}_LL2D9: if(_tmp439 <= (void*)3?1:*((int*)
_tmp439)!= 4)goto _LL2DB;_tmp43B=((struct Cyc_Absyn_PointerType_struct*)_tmp439)->f1;
_tmp43C=(void*)_tmp43B.elt_typ;_tmp43D=_tmp43B.elt_tq;_tmp43E=_tmp43B.ptr_atts;
_tmp43F=(void*)_tmp43E.rgn;_tmp440=_tmp43E.nullable;_tmp441=_tmp43E.bounds;
_tmp442=_tmp43E.zero_term;_LL2DA: {struct Cyc_List_List*_tmp448=Cyc_Toc_get_relns(
_tmp2E7);int _tmp449=Cyc_Toc_check_bounds(_tmp448,_tmp2E7,_tmp2E8);if(Cyc_Toc_warn_bounds_checks?
!_tmp449: 0)({struct Cyc_String_pa_struct _tmp44C;_tmp44C.tag=0;_tmp44C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_exp2string(e));{void*_tmp44A[1]={&
_tmp44C};Cyc_Tcutil_warn(e->loc,({const char*_tmp44B="bounds check necessary for %s";
_tag_arr(_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,30));}),_tag_arr(
_tmp44A,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp2E7);Cyc_Toc_exp_to_c(nv,
_tmp2E8);Cyc_Toc_total_bounds_checks ++;{void*_tmp44D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp441);struct Cyc_Absyn_Exp*_tmp44E;_LL2DE: if(_tmp44D <= (void*)1?1:*((int*)
_tmp44D)!= 0)goto _LL2E0;_tmp44E=((struct Cyc_Absyn_Upper_b_struct*)_tmp44D)->f1;
_LL2DF: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp440);void*ta1=Cyc_Toc_typ_to_c(_tmp43C);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp43D);if(_tmp449)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp442))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp44F[3];_tmp44F[2]=_tmp2E8;_tmp44F[1]=_tmp44E;_tmp44F[0]=
_tmp2E7;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp44F,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0),0))->r));else{if(possibly_null)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp450[4];_tmp450[3]=
_tmp2E8;_tmp450[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp450[1]=_tmp44E;_tmp450[0]=
_tmp2E7;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp450,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));else{(void*)(_tmp2E8->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp451[2];_tmp451[1]=Cyc_Absyn_copy_exp(_tmp2E8);_tmp451[0]=
_tmp44E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp451,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}}goto _LL2DD;}_LL2E0: if((
int)_tmp44D != 0)goto _LL2E2;_LL2E1: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp43C);if(
_tmp449){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp43D),Cyc_Absyn_aggrmember_exp(
_tmp2E7,Cyc_Toc_curr_sp,0),0),_tmp2E8,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp43D),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp452[3];_tmp452[2]=
_tmp2E8;_tmp452[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp452[0]=_tmp2E7;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp452,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL2DD;}_LL2E2: if(_tmp44D <= (void*)1?1:*((int*)
_tmp44D)!= 1)goto _LL2DD;_LL2E3: if(!_tmp449)({void*_tmp453[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp454="can't determine that subscript is in-bounds";
_tag_arr(_tmp454,sizeof(char),_get_zero_arr_size(_tmp454,44));}),_tag_arr(
_tmp453,sizeof(void*),0));});goto _LL2DD;_LL2DD:;}goto _LL2D6;}_LL2DB:;_LL2DC:({
void*_tmp455[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp456="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_arr(
_tmp456,sizeof(char),_get_zero_arr_size(_tmp456,49));}),_tag_arr(_tmp455,sizeof(
void*),0));});_LL2D6:;}goto _LL1D3;}_LL20A: if(*((int*)_tmp2B3)!= 24)goto _LL20C;
_tmp2E9=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2B3)->f1;_LL20B: if(!nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp2E9))->r));else{struct
Cyc_List_List*_tmp457=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp2E9);struct _tagged_arr*
_tmp458=Cyc_Toc_add_tuple_type(_tmp457);struct Cyc_List_List*dles=0;{int i=1;for(0;
_tmp2E9 != 0;(_tmp2E9=_tmp2E9->tl,i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp2E9->hd);dles=({struct Cyc_List_List*_tmp459=_cycalloc(sizeof(*_tmp459));
_tmp459->hd=({struct _tuple6*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->f1=0;
_tmp45A->f2=(struct Cyc_Absyn_Exp*)_tmp2E9->hd;_tmp45A;});_tmp459->tl=dles;
_tmp459;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1D3;_LL20C: if(*((int*)_tmp2B3)!= 26)goto _LL20E;_tmp2EA=((struct Cyc_Absyn_Array_e_struct*)
_tmp2B3)->f1;_LL20D:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp2EA,0))->r));{struct Cyc_List_List*_tmp45B=_tmp2EA;for(0;_tmp45B != 0;_tmp45B=
_tmp45B->tl){struct _tuple6 _tmp45D;struct Cyc_Absyn_Exp*_tmp45E;struct _tuple6*
_tmp45C=(struct _tuple6*)_tmp45B->hd;_tmp45D=*_tmp45C;_tmp45E=_tmp45D.f2;Cyc_Toc_exp_to_c(
nv,_tmp45E);}}goto _LL1D3;_LL20E: if(*((int*)_tmp2B3)!= 27)goto _LL210;_tmp2EB=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B3)->f1;_tmp2EC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2B3)->f2;_tmp2ED=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B3)->f3;
_tmp2EE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B3)->f4;_LL20F: {
unsigned int _tmp460;int _tmp461;struct _tuple5 _tmp45F=Cyc_Evexp_eval_const_uint_exp(
_tmp2EC);_tmp460=_tmp45F.f1;_tmp461=_tmp45F.f2;{void*_tmp462=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2ED->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp2ED);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp2ED)){if(!_tmp461)({
void*_tmp463[0]={};Cyc_Tcutil_terr(_tmp2EC->loc,({const char*_tmp464="cannot determine value of constant";
_tag_arr(_tmp464,sizeof(char),_get_zero_arr_size(_tmp464,35));}),_tag_arr(
_tmp463,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp460;i ++){es=({struct
Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->hd=({struct _tuple6*
_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->f1=0;_tmp466->f2=_tmp2ED;_tmp466;});
_tmp465->tl=es;_tmp465;});}}if(_tmp2EE){struct Cyc_Absyn_Exp*_tmp467=Cyc_Absyn_cast_exp(
_tmp462,Cyc_Absyn_uint_exp(0,0),0);es=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp468=
_cycalloc(sizeof(*_tmp468));_tmp468->hd=({struct _tuple6*_tmp469=_cycalloc(
sizeof(*_tmp469));_tmp469->f1=0;_tmp469->f2=_tmp467;_tmp469;});_tmp468->tl=0;
_tmp468;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1D3;}}}_LL210: if(*((int*)_tmp2B3)!= 28)goto _LL212;_tmp2EF=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2B3)->f1;_tmp2F0=((struct Cyc_Absyn_Struct_e_struct*)_tmp2B3)->f2;_tmp2F1=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2B3)->f3;_tmp2F2=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2B3)->f4;_LL211: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp2F0 != 0,0,0,_tmp2F1,_tmp2EF))->r));else{if(_tmp2F2 == 0)({void*
_tmp46A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp46B="Struct_e: missing aggrdecl pointer";_tag_arr(_tmp46B,sizeof(
char),_get_zero_arr_size(_tmp46B,35));}),_tag_arr(_tmp46A,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp2F2);
struct _RegionHandle _tmp46C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp46C;
_push_region(rgn);{struct Cyc_List_List*_tmp46D=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2F1,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp46E=0;struct Cyc_List_List*_tmp46F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp46F != 0;_tmp46F=_tmp46F->tl){struct Cyc_List_List*
_tmp470=_tmp46D;for(0;_tmp470 != 0;_tmp470=_tmp470->tl){if((*((struct _tuple11*)
_tmp470->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp46F->hd){struct _tuple11
_tmp472;struct Cyc_Absyn_Aggrfield*_tmp473;struct Cyc_Absyn_Exp*_tmp474;struct
_tuple11*_tmp471=(struct _tuple11*)_tmp470->hd;_tmp472=*_tmp471;_tmp473=_tmp472.f1;
_tmp474=_tmp472.f2;{void*_tmp475=(void*)_tmp473->type;Cyc_Toc_exp_to_c(nv,
_tmp474);if(Cyc_Toc_is_void_star(_tmp475))(void*)(_tmp474->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp474->r,0),0))->r));
_tmp46E=({struct Cyc_List_List*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->hd=({
struct _tuple6*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->f1=0;_tmp477->f2=
_tmp474;_tmp477;});_tmp476->tl=_tmp46E;_tmp476;});break;}}}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp46E),0))->r));};_pop_region(rgn);}}goto _LL1D3;_LL212:
if(*((int*)_tmp2B3)!= 29)goto _LL214;_tmp2F3=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2B3)->f1;_tmp2F4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2B3)->f2;_LL213: {
struct Cyc_List_List*fs;{void*_tmp478=Cyc_Tcutil_compress(_tmp2F3);struct Cyc_List_List*
_tmp479;_LL2E5: if(_tmp478 <= (void*)3?1:*((int*)_tmp478)!= 11)goto _LL2E7;_tmp479=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp478)->f2;_LL2E6: fs=_tmp479;goto _LL2E4;
_LL2E7:;_LL2E8:({struct Cyc_String_pa_struct _tmp47C;_tmp47C.tag=0;_tmp47C.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2F3));{void*
_tmp47A[1]={& _tmp47C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp47B="anon struct has type %s";_tag_arr(_tmp47B,sizeof(char),
_get_zero_arr_size(_tmp47B,24));}),_tag_arr(_tmp47A,sizeof(void*),1));}});_LL2E4:;}{
struct _RegionHandle _tmp47D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp47D;
_push_region(rgn);{struct Cyc_List_List*_tmp47E=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp2F4,
fs);for(0;_tmp47E != 0;_tmp47E=_tmp47E->tl){struct _tuple11 _tmp480;struct Cyc_Absyn_Aggrfield*
_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct _tuple11*_tmp47F=(struct _tuple11*)
_tmp47E->hd;_tmp480=*_tmp47F;_tmp481=_tmp480.f1;_tmp482=_tmp480.f2;{void*_tmp483=(
void*)_tmp481->type;Cyc_Toc_exp_to_c(nv,_tmp482);if(Cyc_Toc_is_void_star(_tmp483))(
void*)(_tmp482->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp482->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp2F4,0))->r));};_pop_region(rgn);}goto _LL1D3;}
_LL214: if(*((int*)_tmp2B3)!= 30)goto _LL216;_tmp2F5=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2B3)->f1;if(_tmp2F5 != 0)goto _LL216;_tmp2F6=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2B3)->f2;_tmp2F7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B3)->f3;_LL215: {
struct _tuple1*qv=_tmp2F7->name;struct Cyc_Absyn_Exp*tag_exp=_tmp2F6->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp2F6,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1D3;}_LL216: if(*((int*)_tmp2B3)!= 30)goto _LL218;
_tmp2F8=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B3)->f1;_tmp2F9=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2B3)->f2;_tmp2FA=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2B3)->f3;_LL217: {
struct _tuple1*_tmp484=_tmp2FA->name;struct Cyc_List_List*_tmp485=_tmp2FA->typs;
struct _tuple1*_tmp486=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp487=Cyc_Absyn_var_exp(
_tmp486,0);void*_tmp488=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp484,({
const char*_tmp496="_struct";_tag_arr(_tmp496,sizeof(char),_get_zero_arr_size(
_tmp496,8));})));struct Cyc_Absyn_Exp*tag_exp=_tmp2F9->is_xtunion?Cyc_Absyn_var_exp(
_tmp484,0): Cyc_Toc_tunion_tag(_tmp2F9,_tmp484,1);if(nv->toplevel){struct Cyc_List_List*
dles=0;for(0;_tmp2F8 != 0;(_tmp2F8=_tmp2F8->tl,_tmp485=_tmp485->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp2F8->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp485))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,
cur_e,0);dles=({struct Cyc_List_List*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->hd=({
struct _tuple6*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->f1=0;_tmp48A->f2=
cur_e;_tmp48A;});_tmp489->tl=dles;_tmp489;});}dles=({struct Cyc_List_List*_tmp48B=
_cycalloc(sizeof(*_tmp48B));_tmp48B->hd=({struct _tuple6*_tmp48C=_cycalloc(
sizeof(*_tmp48C));_tmp48C->f1=0;_tmp48C->f2=tag_exp;_tmp48C;});_tmp48B->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp48B;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({struct Cyc_Core_Opt*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->v=Cyc_Toc_collapse_qvar_tag(_tmp484,({
const char*_tmp48E="_struct";_tag_arr(_tmp48E,sizeof(char),_get_zero_arr_size(
_tmp48E,8));}));_tmp48D;}),dles,0))->r));}else{struct Cyc_List_List*_tmp48F=({
struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->hd=Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(_tmp487,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp495->tl=0;
_tmp495;});{int i=1;for(0;_tmp2F8 != 0;(((_tmp2F8=_tmp2F8->tl,i ++)),_tmp485=
_tmp485->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp2F8->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp485))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*_tmp490=Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(_tmp487,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp48F=({
struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->hd=_tmp490;
_tmp491->tl=_tmp48F;_tmp491;});}}}{struct Cyc_Absyn_Stmt*_tmp492=Cyc_Absyn_exp_stmt(
_tmp487,0);struct Cyc_Absyn_Stmt*_tmp493=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp494=_cycalloc(
sizeof(*_tmp494));_tmp494->hd=_tmp492;_tmp494->tl=_tmp48F;_tmp494;})),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp486,_tmp488,0,
_tmp493,0),0))->r));}}goto _LL1D3;}_LL218: if(*((int*)_tmp2B3)!= 31)goto _LL21A;
_LL219: goto _LL21B;_LL21A: if(*((int*)_tmp2B3)!= 32)goto _LL21C;_LL21B: goto _LL1D3;
_LL21C: if(*((int*)_tmp2B3)!= 33)goto _LL21E;_tmp2FB=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2B3)->f1;_tmp2FC=_tmp2FB.is_calloc;_tmp2FD=_tmp2FB.rgn;_tmp2FE=_tmp2FB.elt_type;
_tmp2FF=_tmp2FB.num_elts;_tmp300=_tmp2FB.fat_result;_LL21D: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp2FE)));Cyc_Toc_exp_to_c(nv,_tmp2FF);if(_tmp300){struct
_tuple1*_tmp497=Cyc_Toc_temp_var();struct _tuple1*_tmp498=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp2FC){xexp=_tmp2FF;if(_tmp2FD != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp2FD);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp497,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp2FE,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp497,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp499[3];_tmp499[2]=Cyc_Absyn_var_exp(_tmp497,0);_tmp499[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp499[0]=Cyc_Absyn_var_exp(_tmp498,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp499,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp2FF,0);if(_tmp2FD != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp2FD);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp497,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp2FE,Cyc_Absyn_var_exp(_tmp497,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp49A[3];
_tmp49A[2]=Cyc_Absyn_var_exp(_tmp497,0);_tmp49A[1]=Cyc_Absyn_uint_exp(1,0);
_tmp49A[0]=Cyc_Absyn_var_exp(_tmp498,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp49A,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp49B=Cyc_Absyn_declare_stmt(_tmp497,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp498,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp49B,0))->r));}}else{if(_tmp2FD != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp2FD);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp2FE,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1D3;}_LL21E: if(*((int*)_tmp2B3)!= 35)goto _LL220;_tmp301=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp2B3)->f1;_LL21F: Cyc_Toc_stmt_to_c(nv,
_tmp301);goto _LL1D3;_LL220: if(*((int*)_tmp2B3)!= 34)goto _LL222;_LL221:({void*
_tmp49C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp49D="UnresolvedMem";_tag_arr(_tmp49D,sizeof(char),
_get_zero_arr_size(_tmp49D,14));}),_tag_arr(_tmp49C,sizeof(void*),0));});_LL222:
if(*((int*)_tmp2B3)!= 25)goto _LL224;_LL223:({void*_tmp49E[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp49F="compoundlit";
_tag_arr(_tmp49F,sizeof(char),_get_zero_arr_size(_tmp49F,12));}),_tag_arr(
_tmp49E,sizeof(void*),0));});_LL224: if(*((int*)_tmp2B3)!= 36)goto _LL226;_tmp302=((
struct Cyc_Absyn_Codegen_e_struct*)_tmp2B3)->f1;_LL225:({void*_tmp4A0[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp4A1="codegen";
_tag_arr(_tmp4A1,sizeof(char),_get_zero_arr_size(_tmp4A1,8));}),_tag_arr(_tmp4A0,
sizeof(void*),0));});_LL226: if(*((int*)_tmp2B3)!= 37)goto _LL1D3;_LL227:({void*
_tmp4A2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp4A3="fill";_tag_arr(_tmp4A3,sizeof(char),_get_zero_arr_size(
_tmp4A3,5));}),_tag_arr(_tmp4A2,sizeof(void*),0));});_LL1D3:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct _tuple1*f1;void*f2;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp4A4=(void*)p->r;struct Cyc_Absyn_Vardecl*
_tmp4A5;struct Cyc_Absyn_Vardecl _tmp4A6;struct _tuple1*_tmp4A7;struct Cyc_Absyn_Vardecl*
_tmp4A8;void*_tmp4A9;int _tmp4AA;char _tmp4AB;struct _tagged_arr _tmp4AC;struct Cyc_Absyn_Enumdecl*
_tmp4AD;struct Cyc_Absyn_Enumfield*_tmp4AE;void*_tmp4AF;struct Cyc_Absyn_Enumfield*
_tmp4B0;struct Cyc_Absyn_Tuniondecl*_tmp4B1;struct Cyc_Absyn_Tunionfield*_tmp4B2;
struct Cyc_List_List*_tmp4B3;struct Cyc_Absyn_Pat*_tmp4B4;struct Cyc_Absyn_Pat
_tmp4B5;void*_tmp4B6;struct Cyc_Absyn_Tuniondecl*_tmp4B7;struct Cyc_Absyn_Tunionfield*
_tmp4B8;struct Cyc_List_List*_tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_List_List*
_tmp4BB;struct Cyc_Absyn_AggrInfo _tmp4BC;void*_tmp4BD;struct Cyc_List_List*_tmp4BE;
struct Cyc_Absyn_Pat*_tmp4BF;_LL2EA: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 0)
goto _LL2EC;_tmp4A5=((struct Cyc_Absyn_Var_p_struct*)_tmp4A4)->f1;_tmp4A6=*_tmp4A5;
_tmp4A7=_tmp4A6.name;_LL2EB: nv=Cyc_Toc_add_varmap(nv,_tmp4A7,r);goto _LL2ED;
_LL2EC: if((int)_tmp4A4 != 0)goto _LL2EE;_LL2ED: s=Cyc_Toc_skip_stmt_dl();goto _LL2E9;
_LL2EE: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 6)goto _LL2F0;_tmp4A8=((struct
Cyc_Absyn_Reference_p_struct*)_tmp4A4)->f1;_LL2EF: {struct _tuple1*_tmp4C0=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp4C1=_region_malloc(rgn,sizeof(*_tmp4C1));_tmp4C1->hd=({
struct _tuple13*_tmp4C2=_region_malloc(rgn,sizeof(*_tmp4C2));_tmp4C2->f1=_tmp4C0;
_tmp4C2->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp4C2;});
_tmp4C1->tl=decls;_tmp4C1;});nv=Cyc_Toc_add_varmap(nv,_tmp4A8->name,Cyc_Absyn_var_exp(
_tmp4C0,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp4C0,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL2E9;}_LL2F0: if((int)_tmp4A4 != 1)goto _LL2F2;_LL2F1: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL2E9;_LL2F2: if(_tmp4A4 <= (void*)
2?1:*((int*)_tmp4A4)!= 1)goto _LL2F4;_tmp4A9=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp4A4)->f1;_tmp4AA=((struct Cyc_Absyn_Int_p_struct*)_tmp4A4)->f2;_LL2F3: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp4A9,_tmp4AA,0),fail_stmt);goto _LL2E9;_LL2F4: if(_tmp4A4 <= (
void*)2?1:*((int*)_tmp4A4)!= 2)goto _LL2F6;_tmp4AB=((struct Cyc_Absyn_Char_p_struct*)
_tmp4A4)->f1;_LL2F5: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp4AB,0),
fail_stmt);goto _LL2E9;_LL2F6: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 3)goto
_LL2F8;_tmp4AC=((struct Cyc_Absyn_Float_p_struct*)_tmp4A4)->f1;_LL2F7: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp4AC,0),fail_stmt);goto _LL2E9;_LL2F8: if(_tmp4A4 <= (void*)
2?1:*((int*)_tmp4A4)!= 9)goto _LL2FA;_tmp4AD=((struct Cyc_Absyn_Enum_p_struct*)
_tmp4A4)->f1;_tmp4AE=((struct Cyc_Absyn_Enum_p_struct*)_tmp4A4)->f2;_LL2F9: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp4C3=_cycalloc(
sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_Absyn_Enum_e_struct _tmp4C4;_tmp4C4.tag=
31;_tmp4C4.f1=_tmp4AE->name;_tmp4C4.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4AD;
_tmp4C4.f3=(struct Cyc_Absyn_Enumfield*)_tmp4AE;_tmp4C4;});_tmp4C3;}),0),
fail_stmt);goto _LL2E9;_LL2FA: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 10)goto
_LL2FC;_tmp4AF=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp4A4)->f1;_tmp4B0=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp4A4)->f2;_LL2FB: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp4C5=_cycalloc(
sizeof(*_tmp4C5));_tmp4C5[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp4C6;_tmp4C6.tag=
32;_tmp4C6.f1=_tmp4B0->name;_tmp4C6.f2=(void*)_tmp4AF;_tmp4C6.f3=(struct Cyc_Absyn_Enumfield*)
_tmp4B0;_tmp4C6;});_tmp4C5;}),0),fail_stmt);goto _LL2E9;_LL2FC: if(_tmp4A4 <= (void*)
2?1:*((int*)_tmp4A4)!= 8)goto _LL2FE;_tmp4B1=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4A4)->f1;_tmp4B2=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4A4)->f2;_tmp4B3=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4A4)->f3;if(_tmp4B3 != 0)goto _LL2FE;_LL2FD: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp4B1->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp4B2->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp4B1,_tmp4B2->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_typ,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL2E9;}
_LL2FE: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 5)goto _LL300;_tmp4B4=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp4A4)->f1;_tmp4B5=*_tmp4B4;_tmp4B6=(void*)_tmp4B5.r;
if(_tmp4B6 <= (void*)2?1:*((int*)_tmp4B6)!= 8)goto _LL300;_tmp4B7=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4B6)->f1;_tmp4B8=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4B6)->f2;_tmp4B9=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4B6)->f3;if(!(_tmp4B9 != 0))goto _LL300;
_LL2FF: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp4B8->name,({const char*_tmp4D3="_struct";_tag_arr(_tmp4D3,sizeof(char),
_get_zero_arr_size(_tmp4D3,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*
_tmp4C7=_tmp4B8->typs;for(0;_tmp4B9 != 0;(((_tmp4B9=_tmp4B9->tl,_tmp4C7=((struct
Cyc_List_List*)_check_null(_tmp4C7))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp4C8=(
struct Cyc_Absyn_Pat*)_tmp4B9->hd;if((void*)_tmp4C8->r == (void*)0)continue;{void*
_tmp4C9=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp4C7))->hd)).f2;
struct _tuple1*_tmp4CA=Cyc_Toc_temp_var();void*_tmp4CB=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C8->topt))->v;void*_tmp4CC=Cyc_Toc_typ_to_c(_tmp4CB);struct Cyc_Absyn_Exp*
_tmp4CD=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp4C9)))_tmp4CD=Cyc_Absyn_cast_exp(_tmp4CC,_tmp4CD,0);decls=({
struct Cyc_List_List*_tmp4CE=_region_malloc(rgn,sizeof(*_tmp4CE));_tmp4CE->hd=({
struct _tuple13*_tmp4CF=_region_malloc(rgn,sizeof(*_tmp4CF));_tmp4CF->f1=_tmp4CA;
_tmp4CF->f2=_tmp4CC;_tmp4CF;});_tmp4CE->tl=decls;_tmp4CE;});{struct _tuple12
_tmp4D0=Cyc_Toc_xlate_pat(nv,rgn,_tmp4CB,Cyc_Absyn_var_exp(_tmp4CA,0),_tmp4CD,
_tmp4C8,fail_stmt,decls);nv=_tmp4D0.f1;decls=_tmp4D0.f2;{struct Cyc_Absyn_Stmt*
_tmp4D1=_tmp4D0.f3;struct Cyc_Absyn_Stmt*_tmp4D2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4CA,0),_tmp4CD,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp4D2,_tmp4D1,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp4B7->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp4B8->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(
_tmp4B7);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)
max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e5=Cyc_Absyn_lte_exp(r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),max_tag_exp,
0),0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp4B7,_tmp4B8->name,1);test_exp=
max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,
e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL2E9;}}_LL300: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 8)goto
_LL302;_tmp4BA=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4A4)->f3;_LL301: _tmp4BB=
_tmp4BA;goto _LL303;_LL302: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 4)goto
_LL304;_tmp4BB=((struct Cyc_Absyn_Tuple_p_struct*)_tmp4A4)->f1;_LL303: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp4BB != 0;(_tmp4BB=_tmp4BB->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp4D4=(struct Cyc_Absyn_Pat*)_tmp4BB->hd;if((void*)_tmp4D4->r == (void*)0)
continue;{struct _tuple1*_tmp4D5=Cyc_Toc_temp_var();void*_tmp4D6=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v;decls=({struct Cyc_List_List*_tmp4D7=
_region_malloc(rgn,sizeof(*_tmp4D7));_tmp4D7->hd=({struct _tuple13*_tmp4D8=
_region_malloc(rgn,sizeof(*_tmp4D8));_tmp4D8->f1=_tmp4D5;_tmp4D8->f2=Cyc_Toc_typ_to_c(
_tmp4D6);_tmp4D8;});_tmp4D7->tl=decls;_tmp4D7;});{struct _tuple12 _tmp4D9=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4D6,Cyc_Absyn_var_exp(_tmp4D5,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp4D4,fail_stmt,decls);nv=_tmp4D9.f1;decls=_tmp4D9.f2;{struct Cyc_Absyn_Stmt*
_tmp4DA=_tmp4D9.f3;struct Cyc_Absyn_Stmt*_tmp4DB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp4D5,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4DB,_tmp4DA,0),0);}}}}goto _LL2E9;}_LL304: if(_tmp4A4 <= (
void*)2?1:*((int*)_tmp4A4)!= 7)goto _LL306;_tmp4BC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4A4)->f1;_tmp4BD=(void*)_tmp4BC.aggr_info;_tmp4BE=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp4A4)->f3;_LL305: {struct Cyc_Absyn_Aggrdecl*_tmp4DC=Cyc_Absyn_get_known_aggrdecl(
_tmp4BD);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp4BE != 0;_tmp4BE=_tmp4BE->tl){struct
_tuple14*_tmp4DD=(struct _tuple14*)_tmp4BE->hd;struct Cyc_Absyn_Pat*_tmp4DE=(*
_tmp4DD).f2;if((void*)_tmp4DE->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp4DF=(void*)((struct Cyc_List_List*)_check_null((*_tmp4DD).f1))->hd;struct
_tagged_arr*_tmp4E0;_LL30D: if(*((int*)_tmp4DF)!= 1)goto _LL30F;_tmp4E0=((struct
Cyc_Absyn_FieldName_struct*)_tmp4DF)->f1;_LL30E: f=_tmp4E0;goto _LL30C;_LL30F:;
_LL310:(int)_throw((void*)Cyc_Toc_Match_error);_LL30C:;}{struct _tuple1*_tmp4E1=
Cyc_Toc_temp_var();void*_tmp4E2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DE->topt))->v;
void*_tmp4E3=Cyc_Toc_typ_to_c(_tmp4E2);decls=({struct Cyc_List_List*_tmp4E4=
_region_malloc(rgn,sizeof(*_tmp4E4));_tmp4E4->hd=({struct _tuple13*_tmp4E5=
_region_malloc(rgn,sizeof(*_tmp4E5));_tmp4E5->f1=_tmp4E1;_tmp4E5->f2=_tmp4E3;
_tmp4E5;});_tmp4E4->tl=decls;_tmp4E4;});{struct _tuple12 _tmp4E6=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4E2,Cyc_Absyn_var_exp(_tmp4E1,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp4DE,fail_stmt,decls);nv=_tmp4E6.f1;decls=_tmp4E6.f2;{struct Cyc_Absyn_Exp*
_tmp4E7=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4DC->impl))->fields,f)))->type))_tmp4E7=Cyc_Absyn_cast_exp(
_tmp4E3,_tmp4E7,0);{struct Cyc_Absyn_Stmt*_tmp4E8=_tmp4E6.f3;struct Cyc_Absyn_Stmt*
_tmp4E9=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp4E1,0),_tmp4E7,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp4E9,_tmp4E8,0),0);}}}}}}goto _LL2E9;}_LL306: if(_tmp4A4 <= (
void*)2?1:*((int*)_tmp4A4)!= 5)goto _LL308;_tmp4BF=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp4A4)->f1;_LL307: {struct _tuple1*_tmp4EA=Cyc_Toc_temp_var();void*_tmp4EB=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4BF->topt))->v;decls=({struct Cyc_List_List*
_tmp4EC=_region_malloc(rgn,sizeof(*_tmp4EC));_tmp4EC->hd=({struct _tuple13*
_tmp4ED=_region_malloc(rgn,sizeof(*_tmp4ED));_tmp4ED->f1=_tmp4EA;_tmp4ED->f2=Cyc_Toc_typ_to_c(
_tmp4EB);_tmp4ED;});_tmp4EC->tl=decls;_tmp4EC;});{struct _tuple12 _tmp4EE=Cyc_Toc_xlate_pat(
nv,rgn,_tmp4EB,Cyc_Absyn_var_exp(_tmp4EA,0),Cyc_Absyn_deref_exp(path,0),_tmp4BF,
fail_stmt,decls);nv=_tmp4EE.f1;decls=_tmp4EE.f2;{struct Cyc_Absyn_Stmt*_tmp4EF=
_tmp4EE.f3;struct Cyc_Absyn_Stmt*_tmp4F0=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp4EA,0),Cyc_Absyn_deref_exp(r,0),0),_tmp4EF,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp4F0,0);else{s=_tmp4F0;}goto
_LL2E9;}}}_LL308: if(_tmp4A4 <= (void*)2?1:*((int*)_tmp4A4)!= 11)goto _LL30A;_LL309:({
void*_tmp4F1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4F2="unknownid";_tag_arr(_tmp4F2,sizeof(char),_get_zero_arr_size(
_tmp4F2,10));}),_tag_arr(_tmp4F1,sizeof(void*),0));});_LL30A: if(_tmp4A4 <= (void*)
2?1:*((int*)_tmp4A4)!= 12)goto _LL2E9;_LL30B:({void*_tmp4F3[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp4F4="unknowncall";
_tag_arr(_tmp4F4,sizeof(char),_get_zero_arr_size(_tmp4F4,12));}),_tag_arr(
_tmp4F3,sizeof(void*),0));});_LL2E9:;}return({struct _tuple12 _tmp4F5;_tmp4F5.f1=
nv;_tmp4F5.f2=decls;_tmp4F5.f3=s;_tmp4F5;});}struct _tuple15{struct _tagged_arr*f1;
struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple15*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple15*
_tmp4F6=_region_malloc(r,sizeof(*_tmp4F6));_tmp4F6->f1=Cyc_Toc_fresh_label();
_tmp4F6->f2=Cyc_Toc_fresh_label();_tmp4F6->f3=sc;_tmp4F6;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp4F7=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp4F8=Cyc_Tcutil_compress(
_tmp4F7);_LL312: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 5)goto _LL314;_LL313:
goto _LL315;_LL314: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 12)goto _LL316;
_LL315: leave_as_switch=1;goto _LL311;_LL316:;_LL317: leave_as_switch=0;goto _LL311;
_LL311:;}{struct Cyc_List_List*_tmp4F9=scs;for(0;_tmp4F9 != 0;_tmp4F9=_tmp4F9->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp4F9->hd)->pat_vars))->v != 0?1:((struct Cyc_Absyn_Switch_clause*)_tmp4F9->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp4FA=scs;for(0;_tmp4FA != 0;_tmp4FA=
_tmp4FA->tl){struct Cyc_Absyn_Stmt*_tmp4FB=((struct Cyc_Absyn_Switch_clause*)
_tmp4FA->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp4FA->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp4FB,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp4FB);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp4FC=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp4FC;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp4FD=lscs;for(0;_tmp4FD != 0;_tmp4FD=_tmp4FD->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple15*)_tmp4FD->hd)).f3;struct _tagged_arr*fail_lab=_tmp4FD->tl == 
0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp4FD->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp4FF;struct Cyc_List_List*_tmp500;struct Cyc_Absyn_Stmt*
_tmp501;struct _tuple12 _tmp4FE=Cyc_Toc_xlate_pat(nv,rgn,_tmp4F7,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp4FF=_tmp4FE.f1;_tmp500=_tmp4FE.f2;
_tmp501=_tmp4FE.f3;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp502=(struct
Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp4FF,_tmp502);
_tmp501=Cyc_Absyn_seq_stmt(_tmp501,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((
void*)11,_tmp502,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}
decls=_tmp500;nvs=({struct Cyc_List_List*_tmp503=_region_malloc(rgn,sizeof(*
_tmp503));_tmp503->hd=_tmp4FF;_tmp503->tl=nvs;_tmp503;});test_stmts=({struct Cyc_List_List*
_tmp504=_region_malloc(rgn,sizeof(*_tmp504));_tmp504->hd=_tmp501;_tmp504->tl=
test_stmts;_tmp504;});}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple15 _tmp506;struct _tagged_arr*_tmp507;
struct _tagged_arr*_tmp508;struct Cyc_Absyn_Switch_clause*_tmp509;struct _tuple15*
_tmp505=(struct _tuple15*)lscs->hd;_tmp506=*_tmp505;_tmp507=_tmp506.f1;_tmp508=
_tmp506.f2;_tmp509=_tmp506.f3;{struct Cyc_Toc_Env*_tmp50A=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp509->body;
if(lscs->tl != 0){struct _tuple15 _tmp50C;struct _tagged_arr*_tmp50D;struct Cyc_Absyn_Switch_clause*
_tmp50E;struct _tuple15*_tmp50B=(struct _tuple15*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp50C=*_tmp50B;_tmp50D=_tmp50C.f2;_tmp50E=_tmp50C.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp50A,end_l,_tmp50D,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp50E->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp50A,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp507,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp508,s,0),0);stmts=({
struct Cyc_List_List*_tmp50F=_region_malloc(rgn,sizeof(*_tmp50F));_tmp50F->hd=s;
_tmp50F->tl=stmts;_tmp50F;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple13 _tmp511;struct _tuple1*_tmp512;void*_tmp513;struct _tuple13*_tmp510=(
struct _tuple13*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp511=*_tmp510;
_tmp512=_tmp511.f1;_tmp513=_tmp511.f2;res=Cyc_Absyn_declare_stmt(_tmp512,_tmp513,
0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,
res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp514=
_cycalloc(sizeof(*_tmp514));_tmp514->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp514->tl=0;_tmp514;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp515=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Stmt*_tmp517;struct Cyc_Absyn_Stmt*
_tmp518;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Stmt*
_tmp51B;struct Cyc_Absyn_Stmt*_tmp51C;struct _tuple3 _tmp51D;struct Cyc_Absyn_Exp*
_tmp51E;struct Cyc_Absyn_Stmt*_tmp51F;struct Cyc_Absyn_Stmt*_tmp520;struct Cyc_Absyn_Stmt*
_tmp521;struct Cyc_Absyn_Stmt*_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct _tuple3
_tmp524;struct Cyc_Absyn_Exp*_tmp525;struct _tuple3 _tmp526;struct Cyc_Absyn_Exp*
_tmp527;struct Cyc_Absyn_Stmt*_tmp528;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_List_List*
_tmp52A;struct Cyc_List_List*_tmp52B;struct Cyc_Absyn_Switch_clause**_tmp52C;
struct Cyc_Absyn_Decl*_tmp52D;struct Cyc_Absyn_Stmt*_tmp52E;struct _tagged_arr*
_tmp52F;struct Cyc_Absyn_Stmt*_tmp530;struct Cyc_Absyn_Stmt*_tmp531;struct _tuple3
_tmp532;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Stmt*_tmp534;struct Cyc_List_List*
_tmp535;struct Cyc_Absyn_Tvar*_tmp536;struct Cyc_Absyn_Vardecl*_tmp537;int _tmp538;
struct Cyc_Absyn_Stmt*_tmp539;struct Cyc_Absyn_Exp*_tmp53A;_LL319: if((int)_tmp515
!= 0)goto _LL31B;_LL31A: return;_LL31B: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 
0)goto _LL31D;_tmp516=((struct Cyc_Absyn_Exp_s_struct*)_tmp515)->f1;_LL31C: Cyc_Toc_exp_to_c(
nv,_tmp516);return;_LL31D: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 1)goto
_LL31F;_tmp517=((struct Cyc_Absyn_Seq_s_struct*)_tmp515)->f1;_tmp518=((struct Cyc_Absyn_Seq_s_struct*)
_tmp515)->f2;_LL31E: Cyc_Toc_stmt_to_c(nv,_tmp517);s=_tmp518;continue;_LL31F: if(
_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 2)goto _LL321;_tmp519=((struct Cyc_Absyn_Return_s_struct*)
_tmp515)->f1;_LL320: {struct Cyc_Core_Opt*topt=0;if(_tmp519 != 0){topt=({struct Cyc_Core_Opt*
_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp519->topt))->v);_tmp53B;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp519));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp53C=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp53D=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp53C,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp53C,(void*)topt->v,_tmp519,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp53D,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL321: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 3)goto _LL323;_tmp51A=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp515)->f1;_tmp51B=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp515)->f2;_tmp51C=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp515)->f3;_LL322:
Cyc_Toc_exp_to_c(nv,_tmp51A);Cyc_Toc_stmt_to_c(nv,_tmp51B);s=_tmp51C;continue;
_LL323: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 4)goto _LL325;_tmp51D=((struct
Cyc_Absyn_While_s_struct*)_tmp515)->f1;_tmp51E=_tmp51D.f1;_tmp51F=((struct Cyc_Absyn_While_s_struct*)
_tmp515)->f2;_LL324: Cyc_Toc_exp_to_c(nv,_tmp51E);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp51F);return;_LL325: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 5)goto
_LL327;_tmp520=((struct Cyc_Absyn_Break_s_struct*)_tmp515)->f1;_LL326: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp520 == 0?0: _tmp520->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL327: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 6)goto _LL329;
_tmp521=((struct Cyc_Absyn_Continue_s_struct*)_tmp515)->f1;_LL328: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp522=_tmp521;
goto _LL32A;_LL329: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 7)goto _LL32B;
_tmp522=((struct Cyc_Absyn_Goto_s_struct*)_tmp515)->f2;_LL32A: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp522))->try_depth,s);
return;_LL32B: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 8)goto _LL32D;_tmp523=((
struct Cyc_Absyn_For_s_struct*)_tmp515)->f1;_tmp524=((struct Cyc_Absyn_For_s_struct*)
_tmp515)->f2;_tmp525=_tmp524.f1;_tmp526=((struct Cyc_Absyn_For_s_struct*)_tmp515)->f3;
_tmp527=_tmp526.f1;_tmp528=((struct Cyc_Absyn_For_s_struct*)_tmp515)->f4;_LL32C:
Cyc_Toc_exp_to_c(nv,_tmp523);Cyc_Toc_exp_to_c(nv,_tmp525);Cyc_Toc_exp_to_c(nv,
_tmp527);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp528);return;_LL32D: if(
_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 9)goto _LL32F;_tmp529=((struct Cyc_Absyn_Switch_s_struct*)
_tmp515)->f1;_tmp52A=((struct Cyc_Absyn_Switch_s_struct*)_tmp515)->f2;_LL32E: Cyc_Toc_xlate_switch(
nv,s,_tmp529,_tmp52A);return;_LL32F: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 
11)goto _LL331;_tmp52B=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp515)->f1;_tmp52C=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp515)->f2;_LL330: if(nv->fallthru_info == 0)({
void*_tmp53E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp53F="fallthru in unexpected place";_tag_arr(_tmp53F,sizeof(char),
_get_zero_arr_size(_tmp53F,29));}),_tag_arr(_tmp53E,sizeof(void*),0));});{struct
_tuple8 _tmp541;struct _tagged_arr*_tmp542;struct Cyc_List_List*_tmp543;struct Cyc_Dict_Dict*
_tmp544;struct _tuple8*_tmp540=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(
nv->fallthru_info))->v;_tmp541=*_tmp540;_tmp542=_tmp541.f1;_tmp543=_tmp541.f2;
_tmp544=_tmp541.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp542,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp52C)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp545=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp543);struct Cyc_List_List*_tmp546=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp52B);for(0;_tmp545 != 0;(_tmp545=_tmp545->tl,
_tmp546=_tmp546->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp546))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp544,(struct _tuple1*)_tmp545->hd),(struct Cyc_Absyn_Exp*)_tmp546->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL331: if(_tmp515 <= (void*)1?1:*((
int*)_tmp515)!= 12)goto _LL333;_tmp52D=((struct Cyc_Absyn_Decl_s_struct*)_tmp515)->f1;
_tmp52E=((struct Cyc_Absyn_Decl_s_struct*)_tmp515)->f2;_LL332:{void*_tmp547=(void*)
_tmp52D->r;struct Cyc_Absyn_Vardecl*_tmp548;struct Cyc_Absyn_Pat*_tmp549;struct Cyc_Absyn_Exp*
_tmp54A;struct Cyc_List_List*_tmp54B;_LL346: if(*((int*)_tmp547)!= 0)goto _LL348;
_tmp548=((struct Cyc_Absyn_Var_d_struct*)_tmp547)->f1;_LL347: {struct Cyc_Toc_Env*
_tmp54C=Cyc_Toc_add_varmap(nv,_tmp548->name,Cyc_Absyn_varb_exp(_tmp548->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));
_tmp54D[0]=({struct Cyc_Absyn_Local_b_struct _tmp54E;_tmp54E.tag=3;_tmp54E.f1=
_tmp548;_tmp54E;});_tmp54D;}),0));Cyc_Toc_local_decl_to_c(_tmp54C,_tmp54C,
_tmp548,_tmp52E);goto _LL345;}_LL348: if(*((int*)_tmp547)!= 2)goto _LL34A;_tmp549=((
struct Cyc_Absyn_Let_d_struct*)_tmp547)->f1;_tmp54A=((struct Cyc_Absyn_Let_d_struct*)
_tmp547)->f3;_LL349:{void*_tmp54F=(void*)_tmp549->r;struct Cyc_Absyn_Vardecl*
_tmp550;_LL34F: if(_tmp54F <= (void*)2?1:*((int*)_tmp54F)!= 0)goto _LL351;_tmp550=((
struct Cyc_Absyn_Var_p_struct*)_tmp54F)->f1;_LL350: {struct _tuple1*old_name=
_tmp550->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp550->name=new_name;
_tmp550->initializer=(struct Cyc_Absyn_Exp*)_tmp54A;(void*)(_tmp52D->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551[
0]=({struct Cyc_Absyn_Var_d_struct _tmp552;_tmp552.tag=0;_tmp552.f1=_tmp550;
_tmp552;});_tmp551;})));{struct Cyc_Toc_Env*_tmp553=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554[0]=({struct Cyc_Absyn_Local_b_struct
_tmp555;_tmp555.tag=3;_tmp555.f1=_tmp550;_tmp555;});_tmp554;}),0));Cyc_Toc_local_decl_to_c(
_tmp553,nv,_tmp550,_tmp52E);goto _LL34E;}}_LL351:;_LL352:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp549,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp54A->topt))->v,_tmp54A,_tmp52E))->r));goto _LL34E;_LL34E:;}goto _LL345;_LL34A:
if(*((int*)_tmp547)!= 3)goto _LL34C;_tmp54B=((struct Cyc_Absyn_Letv_d_struct*)
_tmp547)->f1;_LL34B: {struct Cyc_List_List*_tmp556=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp54B);if(_tmp556 == 0)({void*_tmp557[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp558="empty Letv_d";_tag_arr(_tmp558,sizeof(char),_get_zero_arr_size(_tmp558,
13));}),_tag_arr(_tmp557,sizeof(void*),0));});(void*)(_tmp52D->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({
struct Cyc_Absyn_Var_d_struct _tmp55A;_tmp55A.tag=0;_tmp55A.f1=(struct Cyc_Absyn_Vardecl*)
_tmp556->hd;_tmp55A;});_tmp559;})));_tmp556=_tmp556->tl;for(0;_tmp556 != 0;
_tmp556=_tmp556->tl){struct Cyc_Absyn_Decl*_tmp55B=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({
struct Cyc_Absyn_Var_d_struct _tmp55D;_tmp55D.tag=0;_tmp55D.f1=(struct Cyc_Absyn_Vardecl*)
_tmp556->hd;_tmp55D;});_tmp55C;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp55B,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL345;}_LL34C:;_LL34D:({void*_tmp55E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp55F="bad nested declaration within function";
_tag_arr(_tmp55F,sizeof(char),_get_zero_arr_size(_tmp55F,39));}),_tag_arr(
_tmp55E,sizeof(void*),0));});_LL345:;}return;_LL333: if(_tmp515 <= (void*)1?1:*((
int*)_tmp515)!= 15)goto _LL335;_tmp52F=((struct Cyc_Absyn_Label_s_struct*)_tmp515)->f1;
_tmp530=((struct Cyc_Absyn_Label_s_struct*)_tmp515)->f2;_LL334: s=_tmp530;
continue;_LL335: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 16)goto _LL337;_tmp531=((
struct Cyc_Absyn_Do_s_struct*)_tmp515)->f1;_tmp532=((struct Cyc_Absyn_Do_s_struct*)
_tmp515)->f2;_tmp533=_tmp532.f1;_LL336: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp531);Cyc_Toc_exp_to_c(nv,_tmp533);return;_LL337: if(_tmp515 <= (void*)1?1:*((
int*)_tmp515)!= 17)goto _LL339;_tmp534=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp515)->f1;_tmp535=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp515)->f2;_LL338: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->v=(void*)e_typ;_tmp560;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp534);{struct Cyc_Absyn_Stmt*_tmp561=Cyc_Absyn_seq_stmt(
_tmp534,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp562=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp563=Cyc_Absyn_var_exp(
_tmp562,0);struct Cyc_Absyn_Vardecl*_tmp564=Cyc_Absyn_new_vardecl(_tmp562,Cyc_Absyn_exn_typ,
0);_tmp563->topt=({struct Cyc_Core_Opt*_tmp565=_cycalloc(sizeof(*_tmp565));
_tmp565->v=(void*)Cyc_Absyn_exn_typ;_tmp565;});{struct Cyc_Absyn_Pat*_tmp566=({
struct Cyc_Absyn_Pat*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578[0]=({
struct Cyc_Absyn_Var_p_struct _tmp579;_tmp579.tag=0;_tmp579.f1=_tmp564;_tmp579;});
_tmp578;}));_tmp576->topt=({struct Cyc_Core_Opt*_tmp577=_cycalloc(sizeof(*_tmp577));
_tmp577->v=(void*)Cyc_Absyn_exn_typ;_tmp577;});_tmp576->loc=0;_tmp576;});struct
Cyc_Absyn_Exp*_tmp567=Cyc_Absyn_throw_exp(_tmp563,0);_tmp567->topt=({struct Cyc_Core_Opt*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->v=(void*)((void*)0);_tmp568;});{
struct Cyc_Absyn_Stmt*_tmp569=Cyc_Absyn_exp_stmt(_tmp567,0);struct Cyc_Absyn_Switch_clause*
_tmp56A=({struct Cyc_Absyn_Switch_clause*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573->pattern=_tmp566;_tmp573->pat_vars=({struct Cyc_Core_Opt*_tmp574=
_cycalloc(sizeof(*_tmp574));_tmp574->v=({struct Cyc_List_List*_tmp575=_cycalloc(
sizeof(*_tmp575));_tmp575->hd=_tmp564;_tmp575->tl=0;_tmp575;});_tmp574;});
_tmp573->where_clause=0;_tmp573->body=_tmp569;_tmp573->loc=0;_tmp573;});struct
Cyc_Absyn_Stmt*_tmp56B=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp535,({struct
Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->hd=_tmp56A;_tmp572->tl=
0;_tmp572;})),0);Cyc_Toc_stmt_to_c(nv,_tmp56B);{struct Cyc_Absyn_Exp*_tmp56C=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));
_tmp571->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp571->tl=0;
_tmp571;}),0);struct Cyc_Absyn_Stmt*_tmp56D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp570=_cycalloc(sizeof(*_tmp570));
_tmp570->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp570->tl=0;_tmp570;}),0),0);struct
Cyc_Absyn_Exp*_tmp56E=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp56F=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp56D,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp56E,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp56C,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp56F,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp561,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp56B,0),0),0),0),0),0))->r));return;}}}}}
_LL339: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 18)goto _LL33B;_tmp536=((struct
Cyc_Absyn_Region_s_struct*)_tmp515)->f1;_tmp537=((struct Cyc_Absyn_Region_s_struct*)
_tmp515)->f2;_tmp538=((struct Cyc_Absyn_Region_s_struct*)_tmp515)->f3;_tmp539=((
struct Cyc_Absyn_Region_s_struct*)_tmp515)->f4;_LL33A: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp537->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp539);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp57A->tl=0;_tmp57A;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp57B[1];_tmp57B[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp57B,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),Cyc_Absyn_seq_stmt(_tmp539,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp*_tmp57C[1];_tmp57C[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp57C,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL33B: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)
!= 20)goto _LL33D;_tmp53A=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp515)->f1;
_LL33C: Cyc_Toc_exp_to_c(nv,_tmp53A);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp57D=
_cycalloc(sizeof(*_tmp57D));_tmp57D->hd=_tmp53A;_tmp57D->tl=0;_tmp57D;}),0),0))->r));
return;_LL33D: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 10)goto _LL33F;_LL33E:({
void*_tmp57E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp57F="switchC_s";_tag_arr(_tmp57F,sizeof(char),_get_zero_arr_size(
_tmp57F,10));}),_tag_arr(_tmp57E,sizeof(void*),0));});_LL33F: if(_tmp515 <= (void*)
1?1:*((int*)_tmp515)!= 13)goto _LL341;_LL340:({void*_tmp580[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp581="cut";
_tag_arr(_tmp581,sizeof(char),_get_zero_arr_size(_tmp581,4));}),_tag_arr(_tmp580,
sizeof(void*),0));});_LL341: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 14)goto
_LL343;_LL342:({void*_tmp582[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(({const char*_tmp583="splice";_tag_arr(_tmp583,
sizeof(char),_get_zero_arr_size(_tmp583,7));}),_tag_arr(_tmp582,sizeof(void*),0));});
_LL343: if(_tmp515 <= (void*)1?1:*((int*)_tmp515)!= 19)goto _LL318;_LL344:({void*
_tmp584[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(({
const char*_tmp585="forarray";_tag_arr(_tmp585,sizeof(char),_get_zero_arr_size(
_tmp585,9));}),_tag_arr(_tmp584,sizeof(void*),0));});_LL318:;}}struct _tuple16{
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp586=f->args;for(0;_tmp586 != 0;_tmp586=_tmp586->tl){(*((struct _tuple16*)
_tmp586->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp586->hd)).f3);{struct
_tuple1*_tmp587=({struct _tuple1*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->f1=(
void*)0;_tmp588->f2=(*((struct _tuple16*)_tmp586->hd)).f1;_tmp588;});nv=Cyc_Toc_add_varmap(
nv,_tmp587,Cyc_Absyn_var_exp(_tmp587,0));}}}if((unsigned int)f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0: 0){struct Cyc_Core_Opt*
_tmp58A;struct Cyc_Absyn_Tqual _tmp58B;void*_tmp58C;int _tmp58D;struct Cyc_Absyn_VarargInfo
_tmp589=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp58A=
_tmp589.name;_tmp58B=_tmp589.tq;_tmp58C=(void*)_tmp589.type;_tmp58D=_tmp589.inject;{
void*_tmp58E=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp58C,(void*)2,_tmp58B,Cyc_Absyn_false_conref));
struct _tuple1*_tmp58F=({struct _tuple1*_tmp592=_cycalloc(sizeof(*_tmp592));
_tmp592->f1=(void*)0;_tmp592->f2=(struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(_tmp58A))->v;_tmp592;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp590=
_cycalloc(sizeof(*_tmp590));_tmp590->hd=({struct _tuple16*_tmp591=_cycalloc(
sizeof(*_tmp591));_tmp591->f1=(struct _tagged_arr*)_tmp58A->v;_tmp591->f2=_tmp58B;
_tmp591->f3=_tmp58E;_tmp591;});_tmp590->tl=0;_tmp590;}));nv=Cyc_Toc_add_varmap(
nv,_tmp58F,Cyc_Absyn_var_exp(_tmp58F,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp593=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp593 != 0;_tmp593=_tmp593->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp593->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp593->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp594=s;_LL354: if((int)_tmp594 != 1)
goto _LL356;_LL355: return(void*)2;_LL356: if((int)_tmp594 != 4)goto _LL358;_LL357:
return(void*)3;_LL358:;_LL359: return s;_LL353:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp595=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp596;});{
int seen_defn_before;struct Cyc_Core_Opt*_tmp597=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp595);if(_tmp597 == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp595,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp597->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp595,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp598=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp598 != 0;_tmp598=_tmp598->tl){(void*)(((
struct Cyc_Absyn_Aggrfield*)_tmp598->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp598->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp599;});{
struct _tuple1*_tmp59A=tud->name;if(tud->fields == 0?1:((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp59A))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp59A));{struct Cyc_List_List*
_tmp59B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp59B != 0;_tmp59B=_tmp59B->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp59B->hd;if(f->typs != 0){struct Cyc_List_List*_tmp59C=0;int i=1;{struct Cyc_List_List*
_tmp59D=f->typs;for(0;_tmp59D != 0;(_tmp59D=_tmp59D->tl,i ++)){struct _tagged_arr*
_tmp59E=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp59F=({struct Cyc_Absyn_Aggrfield*
_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->name=_tmp59E;_tmp5A1->tq=(*((struct
_tuple4*)_tmp59D->hd)).f1;_tmp5A1->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp59D->hd)).f2);_tmp5A1->width=0;_tmp5A1->attributes=0;_tmp5A1;});
_tmp59C=({struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->hd=
_tmp59F;_tmp5A0->tl=_tmp59C;_tmp5A0;});}}_tmp59C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp59C);_tmp59C=({struct Cyc_List_List*
_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->hd=({struct Cyc_Absyn_Aggrfield*
_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->name=Cyc_Toc_tag_sp;_tmp5A3->tq=Cyc_Toc_mt_tq;
_tmp5A3->type=(void*)Cyc_Absyn_sint_typ;_tmp5A3->width=0;_tmp5A3->attributes=0;
_tmp5A3;});_tmp5A2->tl=_tmp59C;_tmp5A2;});{struct Cyc_Absyn_Aggrdecl*_tmp5A4=({
struct Cyc_Absyn_Aggrdecl*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->kind=(void*)((
void*)0);_tmp5A8->sc=(void*)((void*)2);_tmp5A8->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp5AA="_struct";_tag_arr(_tmp5AA,sizeof(char),_get_zero_arr_size(
_tmp5AA,8));}));_tmp5A8->tvs=0;_tmp5A8->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->exist_vars=0;_tmp5A9->rgn_po=0;
_tmp5A9->fields=_tmp59C;_tmp5A9;});_tmp5A8->attributes=0;_tmp5A8;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));
_tmp5A6[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5A7;_tmp5A7.tag=4;_tmp5A7.f1=
_tmp5A4;_tmp5A7;});_tmp5A6;}),0);_tmp5A5->tl=Cyc_Toc_result_decls;_tmp5A5;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp5AB;});if(xd->fields == 0)return;{struct _tuple1*_tmp5AC=xd->name;struct Cyc_List_List*
_tmp5AD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp5AD != 0;_tmp5AD=_tmp5AD->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp5AD->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp5AE=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(char))+ 4,0);void*_tmp5AF=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp5AE,Cyc_Absyn_false_conref);struct Cyc_Core_Opt*
_tmp5B0=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name);struct Cyc_Core_Opt _tmp5B1;int _tmp5B2;_LL35B: if(_tmp5B0 != 0)goto _LL35D;
_LL35C: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_String_pa_struct _tmp5B9;_tmp5B9.tag=0;_tmp5B9.f1=(struct _tagged_arr)((struct
_tagged_arr)*fn);{struct Cyc_Int_pa_struct _tmp5B8;_tmp5B8.tag=1;_tmp5B8.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5B7;_tmp5B7.tag=1;_tmp5B7.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5B6;_tmp5B6.tag=1;_tmp5B6.f1=(
unsigned int)((int)zero);{struct Cyc_Int_pa_struct _tmp5B5;_tmp5B5.tag=1;_tmp5B5.f1=(
unsigned int)((int)zero);{void*_tmp5B3[5]={& _tmp5B5,& _tmp5B6,& _tmp5B7,& _tmp5B8,&
_tmp5B9};Cyc_aprintf(({const char*_tmp5B4="%c%c%c%c%s";_tag_arr(_tmp5B4,sizeof(
char),_get_zero_arr_size(_tmp5B4,11));}),_tag_arr(_tmp5B3,sizeof(void*),5));}}}}}}),
0);}{struct Cyc_Absyn_Vardecl*_tmp5BA=Cyc_Absyn_new_vardecl(f->name,_tmp5AF,
initopt);(void*)(_tmp5BA->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[
0]=({struct Cyc_Absyn_Var_d_struct _tmp5BD;_tmp5BD.tag=0;_tmp5BD.f1=_tmp5BA;
_tmp5BD;});_tmp5BC;}),0);_tmp5BB->tl=Cyc_Toc_result_decls;_tmp5BB;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp5BE=f->typs;for(0;_tmp5BE != 0;(_tmp5BE=_tmp5BE->tl,
i ++)){struct _tagged_arr*_tmp5BF=({struct _tagged_arr*_tmp5C3=_cycalloc(sizeof(*
_tmp5C3));_tmp5C3[0]=(struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp5C6;_tmp5C6.tag=
1;_tmp5C6.f1=(unsigned int)i;{void*_tmp5C4[1]={& _tmp5C6};Cyc_aprintf(({const char*
_tmp5C5="f%d";_tag_arr(_tmp5C5,sizeof(char),_get_zero_arr_size(_tmp5C5,4));}),
_tag_arr(_tmp5C4,sizeof(void*),1));}});_tmp5C3;});struct Cyc_Absyn_Aggrfield*
_tmp5C0=({struct Cyc_Absyn_Aggrfield*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->name=
_tmp5BF;_tmp5C2->tq=(*((struct _tuple4*)_tmp5BE->hd)).f1;_tmp5C2->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp5BE->hd)).f2);_tmp5C2->width=0;_tmp5C2->attributes=0;_tmp5C2;});
fields=({struct Cyc_List_List*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->hd=
_tmp5C0;_tmp5C1->tl=fields;_tmp5C1;});}}fields=({struct Cyc_List_List*_tmp5C7=
_cycalloc(sizeof(*_tmp5C7));_tmp5C7->hd=({struct Cyc_Absyn_Aggrfield*_tmp5C8=
_cycalloc(sizeof(*_tmp5C8));_tmp5C8->name=Cyc_Toc_tag_sp;_tmp5C8->tq=Cyc_Toc_mt_tq;
_tmp5C8->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);
_tmp5C8->width=0;_tmp5C8->attributes=0;_tmp5C8;});_tmp5C7->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp5C7;});{struct Cyc_Absyn_Aggrdecl*
_tmp5C9=({struct Cyc_Absyn_Aggrdecl*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->kind=(
void*)((void*)0);_tmp5CD->sc=(void*)((void*)2);_tmp5CD->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp5CF="_struct";_tag_arr(_tmp5CF,sizeof(char),
_get_zero_arr_size(_tmp5CF,8));}));_tmp5CD->tvs=0;_tmp5CD->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->exist_vars=0;_tmp5CE->rgn_po=0;
_tmp5CE->fields=fields;_tmp5CE;});_tmp5CD->attributes=0;_tmp5CD;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));
_tmp5CB[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp5CC;_tmp5CC.tag=4;_tmp5CC.f1=
_tmp5C9;_tmp5CC;});_tmp5CB;}),0);_tmp5CA->tl=Cyc_Toc_result_decls;_tmp5CA;});}}
goto _LL35A;}}_LL35D: if(_tmp5B0 == 0)goto _LL35F;_tmp5B1=*_tmp5B0;_tmp5B2=(int)
_tmp5B1.v;if(_tmp5B2 != 0)goto _LL35F;_LL35E: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp5D0=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp5DB;_tmp5DB.tag=0;_tmp5DB.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_Int_pa_struct _tmp5DA;_tmp5DA.tag=1;_tmp5DA.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5D9;_tmp5D9.tag=1;_tmp5D9.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5D8;_tmp5D8.tag=1;_tmp5D8.f1=(unsigned int)((int)zero);{
struct Cyc_Int_pa_struct _tmp5D7;_tmp5D7.tag=1;_tmp5D7.f1=(unsigned int)((int)zero);{
void*_tmp5D5[5]={& _tmp5D7,& _tmp5D8,& _tmp5D9,& _tmp5DA,& _tmp5DB};Cyc_aprintf(({
const char*_tmp5D6="%c%c%c%c%s";_tag_arr(_tmp5D6,sizeof(char),_get_zero_arr_size(
_tmp5D6,11));}),_tag_arr(_tmp5D5,sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*
_tmp5D1=Cyc_Absyn_new_vardecl(f->name,_tmp5AF,(struct Cyc_Absyn_Exp*)_tmp5D0);(
void*)(_tmp5D1->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({
struct Cyc_Absyn_Var_d_struct _tmp5D4;_tmp5D4.tag=0;_tmp5D4.f1=_tmp5D1;_tmp5D4;});
_tmp5D3;}),0);_tmp5D2->tl=Cyc_Toc_result_decls;_tmp5D2;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL35A;_LL35F:;_LL360: goto _LL35A;_LL35A:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5?Cyc_Tcutil_is_tagged_pointer_typ(old_typ):
0)(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp5DC=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp5DD;struct Cyc_Absyn_Exp*
_tmp5DE;struct Cyc_Absyn_Exp*_tmp5DF;int _tmp5E0;_LL362: if(*((int*)_tmp5DC)!= 27)
goto _LL364;_tmp5DD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5DC)->f1;
_tmp5DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp5DC)->f2;_tmp5DF=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp5DC)->f3;_tmp5E0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5DC)->f4;_LL363: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp5DD,_tmp5DE,_tmp5DF,_tmp5E0,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL361;_LL364:;_LL365: if((void*)vd->sc == (void*)0){
struct Cyc_Toc_Env*_tmp5E1=({struct Cyc_Toc_Env*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));
_tmp5E2[0]=*init_nv;_tmp5E2;});_tmp5E1->toplevel=1;Cyc_Toc_exp_to_c(_tmp5E1,init);}
else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL361;_LL361:;}else{void*_tmp5E3=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_ArrayInfo _tmp5E4;void*_tmp5E5;struct Cyc_Absyn_Exp*
_tmp5E6;struct Cyc_Absyn_Conref*_tmp5E7;_LL367: if(_tmp5E3 <= (void*)3?1:*((int*)
_tmp5E3)!= 7)goto _LL369;_tmp5E4=((struct Cyc_Absyn_ArrayType_struct*)_tmp5E3)->f1;
_tmp5E5=(void*)_tmp5E4.elt_type;_tmp5E6=_tmp5E4.num_elts;_tmp5E7=_tmp5E4.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5E7))goto
_LL369;_LL368: if(_tmp5E6 == 0)({void*_tmp5E8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(({const char*_tmp5E9="can't initialize zero-terminated array -- size unknown";
_tag_arr(_tmp5E9,sizeof(char),_get_zero_arr_size(_tmp5E9,55));}),_tag_arr(
_tmp5E8,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)
_check_null(_tmp5E6);struct Cyc_Absyn_Exp*_tmp5EA=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(
vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
struct Cyc_Absyn_Exp*_tmp5EB=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp5EA,
_tmp5EB,0),0),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));goto _LL366;}_LL369:;
_LL36A: goto _LL366;_LL366:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp5EC=_cycalloc(
sizeof(*_tmp5EC));_tmp5EC[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp5EC;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp5ED=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp5ED;_push_region(prgn);{struct Cyc_Toc_Env*_tmp5EF;
struct Cyc_List_List*_tmp5F0;struct Cyc_Absyn_Stmt*_tmp5F1;struct _tuple12 _tmp5EE=
Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp5EF=_tmp5EE.f1;_tmp5F0=_tmp5EE.f2;_tmp5F1=_tmp5EE.f3;Cyc_Toc_stmt_to_c(
_tmp5EF,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp5F1,s,0),0);for(0;_tmp5F0 != 0;_tmp5F0=_tmp5F0->tl){
struct _tuple13 _tmp5F3;struct _tuple1*_tmp5F4;void*_tmp5F5;struct _tuple13*_tmp5F2=(
struct _tuple13*)_tmp5F0->hd;_tmp5F3=*_tmp5F2;_tmp5F4=_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;
s=Cyc_Absyn_declare_stmt(_tmp5F4,_tmp5F5,0,s,0);}};_pop_region(prgn);}return s;}}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp5F6[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp5F7="decls_to_c: not at toplevel!";
_tag_arr(_tmp5F7,sizeof(char),_get_zero_arr_size(_tmp5F7,29));}),_tag_arr(
_tmp5F6,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp5F8=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp5F9;struct Cyc_Absyn_Fndecl*
_tmp5FA;struct Cyc_Absyn_Aggrdecl*_tmp5FB;struct Cyc_Absyn_Tuniondecl*_tmp5FC;
struct Cyc_Absyn_Enumdecl*_tmp5FD;struct Cyc_Absyn_Typedefdecl*_tmp5FE;struct Cyc_List_List*
_tmp5FF;struct Cyc_List_List*_tmp600;struct Cyc_List_List*_tmp601;_LL36C: if(*((int*)
_tmp5F8)!= 0)goto _LL36E;_tmp5F9=((struct Cyc_Absyn_Var_d_struct*)_tmp5F8)->f1;
_LL36D: {struct _tuple1*_tmp602=_tmp5F9->name;if((void*)_tmp5F9->sc == (void*)4)
_tmp602=({struct _tuple1*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->f1=Cyc_Absyn_rel_ns_null;
_tmp603->f2=(*_tmp602).f2;_tmp603;});if(_tmp5F9->initializer != 0)Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F9->initializer));nv=Cyc_Toc_add_varmap(
nv,_tmp5F9->name,Cyc_Absyn_varb_exp(_tmp602,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Absyn_Global_b_struct
_tmp605;_tmp605.tag=0;_tmp605.f1=_tmp5F9;_tmp605;});_tmp604;}),0));_tmp5F9->name=
_tmp602;(void*)(_tmp5F9->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp5F9->sc));(void*)(
_tmp5F9->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp5F9->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->hd=d;_tmp606->tl=
Cyc_Toc_result_decls;_tmp606;});goto _LL36B;}_LL36E: if(*((int*)_tmp5F8)!= 1)goto
_LL370;_tmp5FA=((struct Cyc_Absyn_Fn_d_struct*)_tmp5F8)->f1;_LL36F: {struct
_tuple1*_tmp607=_tmp5FA->name;if((void*)_tmp5FA->sc == (void*)4)_tmp607=({struct
_tuple1*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->f1=Cyc_Absyn_rel_ns_null;
_tmp608->f2=(*_tmp607).f2;_tmp608;});nv=Cyc_Toc_add_varmap(nv,_tmp5FA->name,Cyc_Absyn_var_exp(
_tmp607,0));_tmp5FA->name=_tmp607;Cyc_Toc_fndecl_to_c(nv,_tmp5FA);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->hd=d;_tmp609->tl=
Cyc_Toc_result_decls;_tmp609;});goto _LL36B;}_LL370: if(*((int*)_tmp5F8)!= 2)goto
_LL372;_LL371: goto _LL373;_LL372: if(*((int*)_tmp5F8)!= 3)goto _LL374;_LL373:({void*
_tmp60A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(({
const char*_tmp60B="letdecl at toplevel";_tag_arr(_tmp60B,sizeof(char),
_get_zero_arr_size(_tmp60B,20));}),_tag_arr(_tmp60A,sizeof(void*),0));});_LL374:
if(*((int*)_tmp5F8)!= 4)goto _LL376;_tmp5FB=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp5F8)->f1;_LL375: Cyc_Toc_aggrdecl_to_c(_tmp5FB);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=d;_tmp60C->tl=Cyc_Toc_result_decls;
_tmp60C;});goto _LL36B;_LL376: if(*((int*)_tmp5F8)!= 5)goto _LL378;_tmp5FC=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp5F8)->f1;_LL377: if(_tmp5FC->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp5FC);else{Cyc_Toc_tuniondecl_to_c(_tmp5FC);}goto _LL36B;_LL378: if(*((int*)
_tmp5F8)!= 6)goto _LL37A;_tmp5FD=((struct Cyc_Absyn_Enum_d_struct*)_tmp5F8)->f1;
_LL379: Cyc_Toc_enumdecl_to_c(nv,_tmp5FD);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->hd=d;_tmp60D->tl=Cyc_Toc_result_decls;
_tmp60D;});goto _LL36B;_LL37A: if(*((int*)_tmp5F8)!= 7)goto _LL37C;_tmp5FE=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp5F8)->f1;_LL37B: _tmp5FE->name=_tmp5FE->name;
_tmp5FE->tvs=0;if(_tmp5FE->defn != 0)_tmp5FE->defn=({struct Cyc_Core_Opt*_tmp60E=
_cycalloc(sizeof(*_tmp60E));_tmp60E->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp5FE->defn))->v);_tmp60E;});else{void*_tmp60F=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp5FE->kind))->v;_LL383: if((int)
_tmp60F != 2)goto _LL385;_LL384: _tmp5FE->defn=({struct Cyc_Core_Opt*_tmp610=
_cycalloc(sizeof(*_tmp610));_tmp610->v=(void*)Cyc_Absyn_void_star_typ();_tmp610;});
goto _LL382;_LL385:;_LL386: _tmp5FE->defn=({struct Cyc_Core_Opt*_tmp611=_cycalloc(
sizeof(*_tmp611));_tmp611->v=(void*)((void*)0);_tmp611;});goto _LL382;_LL382:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp612=_cycalloc(sizeof(*_tmp612));
_tmp612->hd=d;_tmp612->tl=Cyc_Toc_result_decls;_tmp612;});goto _LL36B;_LL37C: if(*((
int*)_tmp5F8)!= 8)goto _LL37E;_tmp5FF=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5F8)->f2;_LL37D: _tmp600=_tmp5FF;goto _LL37F;_LL37E: if(*((int*)_tmp5F8)!= 9)
goto _LL380;_tmp600=((struct Cyc_Absyn_Using_d_struct*)_tmp5F8)->f2;_LL37F: _tmp601=
_tmp600;goto _LL381;_LL380: if(*((int*)_tmp5F8)!= 10)goto _LL36B;_tmp601=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp5F8)->f1;_LL381: nv=Cyc_Toc_decls_to_c(nv,_tmp601,
top);goto _LL36B;_LL36B:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
