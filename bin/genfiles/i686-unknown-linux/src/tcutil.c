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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
Cyc_printf(struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_vrprintf(
struct _RegionHandle*,struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*
a);struct _tuple5{void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple6{void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);unsigned int Cyc_strlen(struct _tagged_arr s);
int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tuple7{unsigned int f1;int f2;};struct
_tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_flush_warnings();extern struct
Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*t);int Cyc_Tcutil_kind_leq(
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
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _tagged_arr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);void*
Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";void
Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;void*
Cyc_Tcutil_t2_failure=(void*)0;struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct _tagged_arr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _tagged_arr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);int pos=8;({
struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)((struct
_tagged_arr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\t%s and ";
_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,9));}),_tag_arr(_tmp0,
sizeof(void*),1));}});pos +=_get_arr_size(s1,sizeof(char))+ 5;if(pos >= 80){({void*
_tmp3[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp4="\n\t";_tag_arr(_tmp4,
sizeof(char),_get_zero_arr_size(_tmp4,3));}),_tag_arr(_tmp3,sizeof(void*),0));});
pos=8;}({struct Cyc_String_pa_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _tagged_arr)((
struct _tagged_arr)s2);{void*_tmp5[1]={& _tmp7};Cyc_fprintf(Cyc_stderr,({const char*
_tmp6="%s ";_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,4));}),_tag_arr(
_tmp5,sizeof(void*),1));}});pos +=_get_arr_size(s2,sizeof(char))+ 1;if(pos >= 80){({
void*_tmp8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp9="\n\t";_tag_arr(_tmp9,
sizeof(char),_get_zero_arr_size(_tmp9,3));}),_tag_arr(_tmp8,sizeof(void*),0));});
pos=8;}({void*_tmpA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB="are not compatible. ";
_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,21));}),_tag_arr(_tmpA,
sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){if(pos + Cyc_strlen((struct _tagged_arr)Cyc_Tcutil_failure_reason)
>= 80)({void*_tmpC[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpD="\n\t";
_tag_arr(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,3));}),_tag_arr(_tmpC,
sizeof(void*),0));});({struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_failure_reason);{void*_tmpE[1]={&
_tmp10};Cyc_fprintf(Cyc_stderr,({const char*_tmpF="%s";_tag_arr(_tmpF,sizeof(char),
_get_zero_arr_size(_tmpF,3));}),_tag_arr(_tmpE,sizeof(void*),1));}});}({void*
_tmp11[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";_tag_arr(_tmp12,
sizeof(char),_get_zero_arr_size(_tmp12,2));}),_tag_arr(_tmp11,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct
_tmp15;_tmp15.tag=0;_tmp15.f1=(struct _tagged_arr)((struct _tagged_arr)msg);{void*
_tmp13[1]={& _tmp15};Cyc_fprintf(Cyc_stderr,({const char*_tmp14="Compiler Error (Tcutil::impos): %s\n";
_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,36));}),_tag_arr(_tmp13,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp16=_cycalloc(sizeof(*_tmp16));
_tmp16[0]=({struct Cyc_Core_Impossible_struct _tmp17;_tmp17.tag=Cyc_Core_Impossible;
_tmp17.f1=msg;_tmp17;});_tmp16;}));}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;
_tmp1B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp1A;
_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd));{void*_tmp18[2]={& _tmp1A,& _tmp1B};Cyc_fprintf(
Cyc_stderr,({const char*_tmp19="%s::%s ";_tag_arr(_tmp19,sizeof(char),
_get_zero_arr_size(_tmp19,8));}),_tag_arr(_tmp18,sizeof(void*),2));}}});}({void*
_tmp1C[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1D="\n";_tag_arr(_tmp1D,
sizeof(char),_get_zero_arr_size(_tmp1D,2));}),_tag_arr(_tmp1C,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){struct
_tagged_arr msg=(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({
struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=sg;_tmp1E->tl=
Cyc_Tcutil_warning_segs;_tmp1E;});Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*
_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->hd=({struct _tagged_arr*_tmp20=
_cycalloc(sizeof(*_tmp20));_tmp20[0]=msg;_tmp20;});_tmp1F->tl=Cyc_Tcutil_warning_msgs;
_tmp1F;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;({void*_tmp21[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp22="***Warnings***\n";
_tag_arr(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,16));}),_tag_arr(_tmp21,
sizeof(void*),0));});{struct Cyc_List_List*_tmp23=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp27;_tmp27.tag=
0;_tmp27.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct
_tmp26;_tmp26.tag=0;_tmp26.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(_tmp23))->hd));{void*_tmp24[2]={&
_tmp26,& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp25="%s: %s\n";_tag_arr(
_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,8));}),_tag_arr(_tmp24,sizeof(void*),
2));}}});_tmp23=_tmp23->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp28[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp29="**************\n";_tag_arr(_tmp29,sizeof(char),
_get_zero_arr_size(_tmp29,16));}),_tag_arr(_tmp28,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return*((int*)_check_null(tv1->identity))- *((int*)_check_null(tv2->identity));}
void*Cyc_Tcutil_compress(void*t){void*_tmp2A=t;struct Cyc_Core_Opt*_tmp2B;void**
_tmp2C;void**_tmp2D;void***_tmp2E;struct Cyc_Core_Opt*_tmp2F;struct Cyc_Core_Opt**
_tmp30;_LL1: if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 0)goto _LL3;_tmp2B=((struct
Cyc_Absyn_Evar_struct*)_tmp2A)->f2;if(_tmp2B != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 16)goto _LL5;_tmp2C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp2A)->f4;if(_tmp2C != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp2A <= (void*)3?1:*((
int*)_tmp2A)!= 16)goto _LL7;_tmp2D=((struct Cyc_Absyn_TypedefType_struct*)_tmp2A)->f4;
_tmp2E=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp2A)->f4;_LL6: {void*
t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp2E)));if(t2 != *((void**)
_check_null(*_tmp2E)))*_tmp2E=({void**_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=
t2;_tmp31;});return t2;}_LL7: if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 0)goto _LL9;
_tmp2F=((struct Cyc_Absyn_Evar_struct*)_tmp2A)->f2;_tmp30=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp2A)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp30))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp30))->v)*_tmp30=({struct Cyc_Core_Opt*_tmp32=_cycalloc(sizeof(*
_tmp32));_tmp32->v=(void*)t2;_tmp32;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp33=(void*)c->v;void*_tmp34;struct Cyc_Absyn_Conref*_tmp35;_LLC: if((int)_tmp33
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp33 <= (void*)1?1:*((
int*)_tmp33)!= 0)goto _LL10;_tmp34=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp33)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp34);_LL10: if(_tmp33 <= (void*)1?1:*((
int*)_tmp33)!= 1)goto _LLB;_tmp35=((struct Cyc_Absyn_Forward_constr_struct*)_tmp33)->f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp35);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp36=Cyc_Absyn_compress_kb(kb);void*_tmp37;void*_tmp38;_LL13: if(*((
int*)_tmp36)!= 0)goto _LL15;_tmp37=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp36)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp39=_cycalloc(sizeof(*
_tmp39));_tmp39[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(
void*)_tmp37;_tmp3A;});_tmp39;});_LL15: if(*((int*)_tmp36)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));
_tmp3B[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp3C;_tmp3C.tag=1;_tmp3C.f1=0;
_tmp3C;});_tmp3B;});_LL17: if(*((int*)_tmp36)!= 2)goto _LL12;_tmp38=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp36)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp3E;_tmp3E.tag=2;_tmp3E.f1=0;_tmp3E.f2=(void*)_tmp38;_tmp3E;});_tmp3D;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=tv->name;
_tmp3F->identity=0;_tmp3F->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp3F;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp41;struct Cyc_Core_Opt*_tmp42;struct Cyc_Absyn_Tqual _tmp43;void*_tmp44;
struct _tuple2*_tmp40=arg;_tmp41=*_tmp40;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;_tmp44=
_tmp41.f3;return({struct _tuple2*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=
_tmp42;_tmp45->f2=_tmp43;_tmp45->f3=Cyc_Tcutil_copy_type(_tmp44);_tmp45;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp47;
struct Cyc_Absyn_Tqual _tmp48;void*_tmp49;struct _tuple4*_tmp46=arg;_tmp47=*_tmp46;
_tmp48=_tmp47.f1;_tmp49=_tmp47.f2;return({struct _tuple4*_tmp4A=_cycalloc(sizeof(*
_tmp4A));_tmp4A->f1=_tmp48;_tmp4A->f2=Cyc_Tcutil_copy_type(_tmp49);_tmp4A;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->name=
f->name;_tmp4B->tq=f->tq;_tmp4B->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp4B->width=f->width;_tmp4B->attributes=f->attributes;_tmp4B;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp4D;void*_tmp4E;
void*_tmp4F;struct _tuple6*_tmp4C=x;_tmp4D=*_tmp4C;_tmp4E=_tmp4D.f1;_tmp4F=_tmp4D.f2;
return({struct _tuple6*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=Cyc_Tcutil_copy_type(
_tmp4E);_tmp50->f2=Cyc_Tcutil_copy_type(_tmp4F);_tmp50;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=f->name;_tmp51->tag=f->tag;_tmp51->loc=
f->loc;_tmp51;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp52=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp53;struct Cyc_Absyn_TunionInfo _tmp54;void*_tmp55;
struct Cyc_List_List*_tmp56;void*_tmp57;struct Cyc_Absyn_TunionFieldInfo _tmp58;
void*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_Absyn_PtrInfo _tmp5B;void*_tmp5C;
struct Cyc_Absyn_Tqual _tmp5D;struct Cyc_Absyn_PtrAtts _tmp5E;void*_tmp5F;struct Cyc_Absyn_Conref*
_tmp60;struct Cyc_Absyn_Conref*_tmp61;struct Cyc_Absyn_Conref*_tmp62;void*_tmp63;
void*_tmp64;int _tmp65;struct Cyc_Absyn_ArrayInfo _tmp66;void*_tmp67;struct Cyc_Absyn_Tqual
_tmp68;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_Absyn_Conref*_tmp6A;struct Cyc_Absyn_FnInfo
_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Core_Opt*_tmp6D;void*_tmp6E;struct
Cyc_List_List*_tmp6F;int _tmp70;struct Cyc_Absyn_VarargInfo*_tmp71;struct Cyc_List_List*
_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_AggrInfo
_tmp75;void*_tmp76;void*_tmp77;struct _tuple1*_tmp78;struct Cyc_List_List*_tmp79;
struct Cyc_Absyn_AggrInfo _tmp7A;void*_tmp7B;struct Cyc_Absyn_Aggrdecl**_tmp7C;
struct Cyc_List_List*_tmp7D;void*_tmp7E;struct Cyc_List_List*_tmp7F;struct _tuple1*
_tmp80;struct Cyc_Absyn_Enumdecl*_tmp81;struct Cyc_List_List*_tmp82;void*_tmp83;
int _tmp84;void*_tmp85;void*_tmp86;struct _tuple1*_tmp87;struct Cyc_List_List*
_tmp88;struct Cyc_Absyn_Typedefdecl*_tmp89;void*_tmp8A;struct Cyc_List_List*_tmp8B;
void*_tmp8C;_LL1A: if((int)_tmp52 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp52 <= (
void*)3?1:*((int*)_tmp52)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(_tmp52 <= (void*)3?
1:*((int*)_tmp52)!= 1)goto _LL20;_tmp53=((struct Cyc_Absyn_VarType_struct*)_tmp52)->f1;
_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*_tmp8D=_cycalloc(sizeof(*
_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_VarType_struct _tmp8E;_tmp8E.tag=1;_tmp8E.f1=
Cyc_Tcutil_copy_tvar(_tmp53);_tmp8E;});_tmp8D;});_LL20: if(_tmp52 <= (void*)3?1:*((
int*)_tmp52)!= 2)goto _LL22;_tmp54=((struct Cyc_Absyn_TunionType_struct*)_tmp52)->f1;
_tmp55=(void*)_tmp54.tunion_info;_tmp56=_tmp54.targs;_tmp57=(void*)_tmp54.rgn;
_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_TunionType_struct _tmp90;_tmp90.tag=2;_tmp90.f1=({
struct Cyc_Absyn_TunionInfo _tmp91;_tmp91.tunion_info=(void*)_tmp55;_tmp91.targs=
Cyc_Tcutil_copy_types(_tmp56);_tmp91.rgn=(void*)Cyc_Tcutil_copy_type(_tmp57);
_tmp91;});_tmp90;});_tmp8F;});_LL22: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 3)
goto _LL24;_tmp58=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp52)->f1;_tmp59=(
void*)_tmp58.field_info;_tmp5A=_tmp58.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp93;_tmp93.tag=3;_tmp93.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp94;_tmp94.field_info=(
void*)_tmp59;_tmp94.targs=Cyc_Tcutil_copy_types(_tmp5A);_tmp94;});_tmp93;});
_tmp92;});_LL24: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 4)goto _LL26;_tmp5B=((
struct Cyc_Absyn_PointerType_struct*)_tmp52)->f1;_tmp5C=(void*)_tmp5B.elt_typ;
_tmp5D=_tmp5B.elt_tq;_tmp5E=_tmp5B.ptr_atts;_tmp5F=(void*)_tmp5E.rgn;_tmp60=
_tmp5E.nullable;_tmp61=_tmp5E.bounds;_tmp62=_tmp5E.zero_term;_LL25: {void*_tmp95=
Cyc_Tcutil_copy_type(_tmp5C);void*_tmp96=Cyc_Tcutil_copy_type(_tmp5F);struct Cyc_Absyn_Conref*
_tmp97=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp60);struct Cyc_Absyn_Tqual _tmp98=_tmp5D;struct Cyc_Absyn_Conref*_tmp99=Cyc_Tcutil_copy_conref(
_tmp61);struct Cyc_Absyn_Conref*_tmp9A=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp62);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9C;_tmp9C.tag=4;_tmp9C.f1=({struct Cyc_Absyn_PtrInfo _tmp9D;_tmp9D.elt_typ=(
void*)_tmp95;_tmp9D.elt_tq=_tmp98;_tmp9D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp9E;_tmp9E.rgn=(void*)_tmp96;_tmp9E.nullable=_tmp97;_tmp9E.bounds=_tmp99;
_tmp9E.zero_term=_tmp9A;_tmp9E;});_tmp9D;});_tmp9C;});_tmp9B;});}_LL26: if(_tmp52
<= (void*)3?1:*((int*)_tmp52)!= 5)goto _LL28;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp52)->f1;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp52)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));
_tmp9F[0]=({struct Cyc_Absyn_IntType_struct _tmpA0;_tmpA0.tag=5;_tmpA0.f1=(void*)
_tmp63;_tmpA0.f2=(void*)_tmp64;_tmpA0;});_tmp9F;});_LL28: if((int)_tmp52 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 6)goto _LL2C;
_tmp65=((struct Cyc_Absyn_DoubleType_struct*)_tmp52)->f1;_LL2B: return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmpA1=_cycalloc_atomic(sizeof(*_tmpA1));
_tmpA1[0]=({struct Cyc_Absyn_DoubleType_struct _tmpA2;_tmpA2.tag=6;_tmpA2.f1=
_tmp65;_tmpA2;});_tmpA1;});_LL2C: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 7)goto
_LL2E;_tmp66=((struct Cyc_Absyn_ArrayType_struct*)_tmp52)->f1;_tmp67=(void*)
_tmp66.elt_type;_tmp68=_tmp66.tq;_tmp69=_tmp66.num_elts;_tmp6A=_tmp66.zero_term;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpA3=_cycalloc(sizeof(*
_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_ArrayType_struct _tmpA4;_tmpA4.tag=7;_tmpA4.f1=({
struct Cyc_Absyn_ArrayInfo _tmpA5;_tmpA5.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp67);_tmpA5.tq=_tmp68;_tmpA5.num_elts=_tmp69;_tmpA5.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp6A);_tmpA5;});_tmpA4;});
_tmpA3;});_LL2E: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 8)goto _LL30;_tmp6B=((
struct Cyc_Absyn_FnType_struct*)_tmp52)->f1;_tmp6C=_tmp6B.tvars;_tmp6D=_tmp6B.effect;
_tmp6E=(void*)_tmp6B.ret_typ;_tmp6F=_tmp6B.args;_tmp70=_tmp6B.c_varargs;_tmp71=
_tmp6B.cyc_varargs;_tmp72=_tmp6B.rgn_po;_tmp73=_tmp6B.attributes;_LL2F: {struct
Cyc_List_List*_tmpA6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp6C);struct Cyc_Core_Opt*_tmpA7=_tmp6D == 0?0:({struct Cyc_Core_Opt*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp6D->v);
_tmpB1;});void*_tmpA8=Cyc_Tcutil_copy_type(_tmp6E);struct Cyc_List_List*_tmpA9=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp6F);int _tmpAA=_tmp70;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp71 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp71);cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpAB=_cycalloc(
sizeof(*_tmpAB));_tmpAB->name=cv->name;_tmpAB->tq=cv->tq;_tmpAB->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpAB->inject=cv->inject;_tmpAB;});}{struct Cyc_List_List*_tmpAC=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp72);struct Cyc_List_List*_tmpAD=_tmp73;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));
_tmpAE[0]=({struct Cyc_Absyn_FnType_struct _tmpAF;_tmpAF.tag=8;_tmpAF.f1=({struct
Cyc_Absyn_FnInfo _tmpB0;_tmpB0.tvars=_tmpA6;_tmpB0.effect=_tmpA7;_tmpB0.ret_typ=(
void*)_tmpA8;_tmpB0.args=_tmpA9;_tmpB0.c_varargs=_tmpAA;_tmpB0.cyc_varargs=
cyc_varargs2;_tmpB0.rgn_po=_tmpAC;_tmpB0.attributes=_tmpAD;_tmpB0;});_tmpAF;});
_tmpAE;});}}_LL30: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 9)goto _LL32;_tmp74=((
struct Cyc_Absyn_TupleType_struct*)_tmp52)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_TupleType_struct
_tmpB3;_tmpB3.tag=9;_tmpB3.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp74);
_tmpB3;});_tmpB2;});_LL32: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 10)goto _LL34;
_tmp75=((struct Cyc_Absyn_AggrType_struct*)_tmp52)->f1;_tmp76=(void*)_tmp75.aggr_info;
if(*((int*)_tmp76)!= 0)goto _LL34;_tmp77=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp76)->f1;_tmp78=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp76)->f2;_tmp79=
_tmp75.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpB4=
_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_AggrType_struct _tmpB5;
_tmpB5.tag=10;_tmpB5.f1=({struct Cyc_Absyn_AggrInfo _tmpB6;_tmpB6.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(
void*)_tmp77;_tmpB8.f2=_tmp78;_tmpB8;});_tmpB7;}));_tmpB6.targs=Cyc_Tcutil_copy_types(
_tmp79);_tmpB6;});_tmpB5;});_tmpB4;});_LL34: if(_tmp52 <= (void*)3?1:*((int*)
_tmp52)!= 10)goto _LL36;_tmp7A=((struct Cyc_Absyn_AggrType_struct*)_tmp52)->f1;
_tmp7B=(void*)_tmp7A.aggr_info;if(*((int*)_tmp7B)!= 1)goto _LL36;_tmp7C=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp7B)->f1;_tmp7D=_tmp7A.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({
struct Cyc_Absyn_AggrType_struct _tmpBA;_tmpBA.tag=10;_tmpBA.f1=({struct Cyc_Absyn_AggrInfo
_tmpBB;_tmpBB.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpBC=
_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpBD;
_tmpBD.tag=1;_tmpBD.f1=_tmp7C;_tmpBD;});_tmpBC;}));_tmpBB.targs=Cyc_Tcutil_copy_types(
_tmp7D);_tmpBB;});_tmpBA;});_tmpB9;});_LL36: if(_tmp52 <= (void*)3?1:*((int*)
_tmp52)!= 11)goto _LL38;_tmp7E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp52)->f1;_tmp7F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp52)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpBE=_cycalloc(sizeof(*
_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpBF;_tmpBF.tag=11;
_tmpBF.f1=(void*)_tmp7E;_tmpBF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp7F);_tmpBF;});_tmpBE;});_LL38: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 12)
goto _LL3A;_tmp80=((struct Cyc_Absyn_EnumType_struct*)_tmp52)->f1;_tmp81=((struct
Cyc_Absyn_EnumType_struct*)_tmp52)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_EnumType_struct
_tmpC1;_tmpC1.tag=12;_tmpC1.f1=_tmp80;_tmpC1.f2=_tmp81;_tmpC1;});_tmpC0;});_LL3A:
if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 13)goto _LL3C;_tmp82=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp52)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpC2=
_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpC3;
_tmpC3.tag=13;_tmpC3.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp82);_tmpC3;});_tmpC2;});_LL3C: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 14)
goto _LL3E;_tmp83=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp52)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));
_tmpC4[0]=({struct Cyc_Absyn_SizeofType_struct _tmpC5;_tmpC5.tag=14;_tmpC5.f1=(
void*)Cyc_Tcutil_copy_type(_tmp83);_tmpC5;});_tmpC4;});_LL3E: if(_tmp52 <= (void*)
3?1:*((int*)_tmp52)!= 18)goto _LL40;_tmp84=((struct Cyc_Absyn_TypeInt_struct*)
_tmp52)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpC6=
_cycalloc_atomic(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpC7;_tmpC7.tag=18;_tmpC7.f1=_tmp84;_tmpC7;});_tmpC6;});_LL40: if(_tmp52 <= (
void*)3?1:*((int*)_tmp52)!= 17)goto _LL42;_tmp85=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp52)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpC8=_cycalloc(
sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_TagType_struct _tmpC9;_tmpC9.tag=17;
_tmpC9.f1=(void*)Cyc_Tcutil_copy_type(_tmp85);_tmpC9;});_tmpC8;});_LL42: if(
_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 15)goto _LL44;_tmp86=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp52)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpCA=
_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpCB;_tmpCB.tag=15;_tmpCB.f1=(void*)Cyc_Tcutil_copy_type(_tmp86);_tmpCB;});
_tmpCA;});_LL44: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 16)goto _LL46;_tmp87=((
struct Cyc_Absyn_TypedefType_struct*)_tmp52)->f1;_tmp88=((struct Cyc_Absyn_TypedefType_struct*)
_tmp52)->f2;_tmp89=((struct Cyc_Absyn_TypedefType_struct*)_tmp52)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpCC=_cycalloc(sizeof(*
_tmpCC));_tmpCC[0]=({struct Cyc_Absyn_TypedefType_struct _tmpCD;_tmpCD.tag=16;
_tmpCD.f1=_tmp87;_tmpCD.f2=Cyc_Tcutil_copy_types(_tmp88);_tmpCD.f3=_tmp89;_tmpCD.f4=
0;_tmpCD;});_tmpCC;});_LL46: if((int)_tmp52 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 19)goto _LL4A;_tmp8A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp52)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_AccessEff_struct _tmpCF;
_tmpCF.tag=19;_tmpCF.f1=(void*)Cyc_Tcutil_copy_type(_tmp8A);_tmpCF;});_tmpCE;});
_LL4A: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 20)goto _LL4C;_tmp8B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp52)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpD0=_cycalloc(
sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_JoinEff_struct _tmpD1;_tmpD1.tag=20;
_tmpD1.f1=Cyc_Tcutil_copy_types(_tmp8B);_tmpD1;});_tmpD0;});_LL4C: if(_tmp52 <= (
void*)3?1:*((int*)_tmp52)!= 21)goto _LL19;_tmp8C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp52)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpD2=_cycalloc(
sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpD3;_tmpD3.tag=21;
_tmpD3.f1=(void*)Cyc_Tcutil_copy_type(_tmp8C);_tmpD3;});_tmpD2;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpD5=({struct _tuple6 _tmpD4;
_tmpD4.f1=k1;_tmpD4.f2=k2;_tmpD4;});void*_tmpD6;void*_tmpD7;void*_tmpD8;void*
_tmpD9;void*_tmpDA;void*_tmpDB;_LL4F: _tmpD6=_tmpD5.f1;if((int)_tmpD6 != 2)goto
_LL51;_tmpD7=_tmpD5.f2;if((int)_tmpD7 != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpD8=
_tmpD5.f1;if((int)_tmpD8 != 2)goto _LL53;_tmpD9=_tmpD5.f2;if((int)_tmpD9 != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpDA=_tmpD5.f1;if((int)_tmpDA != 1)goto _LL55;_tmpDB=
_tmpD5.f2;if((int)_tmpDB != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpDC=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpDD;void*_tmpDE;_LL58: if(*((int*)_tmpDC)!= 0)goto _LL5A;
_tmpDD=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDC)->f1;_LL59: return _tmpDD;
_LL5A: if(*((int*)_tmpDC)!= 2)goto _LL5C;_tmpDE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDC)->f2;_LL5B: return _tmpDE;_LL5C:;_LL5D:({void*_tmpDF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpE0="kind not suitably constrained!";
_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,31));}),_tag_arr(_tmpDF,
sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpE1=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpE2;struct Cyc_Core_Opt*_tmpE3;struct Cyc_Absyn_Tvar*
_tmpE4;void*_tmpE5;struct Cyc_Absyn_TunionFieldInfo _tmpE6;void*_tmpE7;struct Cyc_Absyn_Tunionfield*
_tmpE8;struct Cyc_Absyn_TunionFieldInfo _tmpE9;void*_tmpEA;struct Cyc_Absyn_Enumdecl*
_tmpEB;struct Cyc_Absyn_AggrInfo _tmpEC;void*_tmpED;struct Cyc_Absyn_AggrInfo _tmpEE;
void*_tmpEF;struct Cyc_Absyn_Aggrdecl**_tmpF0;struct Cyc_Absyn_Aggrdecl*_tmpF1;
struct Cyc_Absyn_Aggrdecl _tmpF2;struct Cyc_Absyn_AggrdeclImpl*_tmpF3;struct Cyc_Absyn_Enumdecl*
_tmpF4;struct Cyc_Absyn_PtrInfo _tmpF5;struct Cyc_Absyn_Typedefdecl*_tmpF6;_LL5F:
if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 0)goto _LL61;_tmpE2=((struct Cyc_Absyn_Evar_struct*)
_tmpE1)->f1;_tmpE3=((struct Cyc_Absyn_Evar_struct*)_tmpE1)->f2;_LL60: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpE2))->v;_LL61: if(_tmpE1 <= (void*)3?1:*((int*)
_tmpE1)!= 1)goto _LL63;_tmpE4=((struct Cyc_Absyn_VarType_struct*)_tmpE1)->f1;_LL62:
return Cyc_Tcutil_tvar_kind(_tmpE4);_LL63: if((int)_tmpE1 != 0)goto _LL65;_LL64:
return(void*)1;_LL65: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 5)goto _LL67;_tmpE5=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpE1)->f2;_LL66: return _tmpE5 == (void*)
2?(void*)2:(void*)1;_LL67: if((int)_tmpE1 != 1)goto _LL69;_LL68: goto _LL6A;_LL69: if(
_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 6)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if(
_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 8)goto _LL6D;_LL6C: return(void*)1;_LL6D: if(
_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 15)goto _LL6F;_LL6E: return(void*)2;_LL6F:
if((int)_tmpE1 != 2)goto _LL71;_LL70: return(void*)3;_LL71: if(_tmpE1 <= (void*)3?1:*((
int*)_tmpE1)!= 2)goto _LL73;_LL72: return(void*)2;_LL73: if(_tmpE1 <= (void*)3?1:*((
int*)_tmpE1)!= 3)goto _LL75;_tmpE6=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpE1)->f1;_tmpE7=(void*)_tmpE6.field_info;if(*((int*)_tmpE7)!= 1)goto _LL75;
_tmpE8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpE7)->f2;_LL74: if(_tmpE8->typs
== 0)return(void*)2;else{return(void*)1;}_LL75: if(_tmpE1 <= (void*)3?1:*((int*)
_tmpE1)!= 3)goto _LL77;_tmpE9=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpE1)->f1;
_tmpEA=(void*)_tmpE9.field_info;if(*((int*)_tmpEA)!= 0)goto _LL77;_LL76:({void*
_tmpF7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpF8="typ_kind: Unresolved TunionFieldType";_tag_arr(_tmpF8,sizeof(
char),_get_zero_arr_size(_tmpF8,37));}),_tag_arr(_tmpF7,sizeof(void*),0));});
_LL77: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 12)goto _LL79;_tmpEB=((struct Cyc_Absyn_EnumType_struct*)
_tmpE1)->f2;if(_tmpEB != 0)goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpE1 <= (void*)3?1:*((
int*)_tmpE1)!= 10)goto _LL7B;_tmpEC=((struct Cyc_Absyn_AggrType_struct*)_tmpE1)->f1;
_tmpED=(void*)_tmpEC.aggr_info;if(*((int*)_tmpED)!= 0)goto _LL7B;_LL7A: return(
void*)0;_LL7B: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 10)goto _LL7D;_tmpEE=((
struct Cyc_Absyn_AggrType_struct*)_tmpE1)->f1;_tmpEF=(void*)_tmpEE.aggr_info;if(*((
int*)_tmpEF)!= 1)goto _LL7D;_tmpF0=((struct Cyc_Absyn_KnownAggr_struct*)_tmpEF)->f1;
_tmpF1=*_tmpF0;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.impl;_LL7C: return _tmpF3 == 0?(void*)0:(
void*)1;_LL7D: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 11)goto _LL7F;_LL7E: goto
_LL80;_LL7F: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 13)goto _LL81;_LL80: return(
void*)1;_LL81: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 12)goto _LL83;_tmpF4=((
struct Cyc_Absyn_EnumType_struct*)_tmpE1)->f2;_LL82: if(_tmpF4->fields == 0)return(
void*)0;else{return(void*)1;}_LL83: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 4)
goto _LL85;_tmpF5=((struct Cyc_Absyn_PointerType_struct*)_tmpE1)->f1;_LL84: {void*
_tmpF9=(void*)(Cyc_Absyn_compress_conref((_tmpF5.ptr_atts).bounds))->v;void*
_tmpFA;void*_tmpFB;void*_tmpFC;_LL98: if((int)_tmpF9 != 0)goto _LL9A;_LL99: goto
_LL9B;_LL9A: if(_tmpF9 <= (void*)1?1:*((int*)_tmpF9)!= 0)goto _LL9C;_tmpFA=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpF9)->f1;if((int)_tmpFA != 0)goto _LL9C;_LL9B:
return(void*)1;_LL9C: if(_tmpF9 <= (void*)1?1:*((int*)_tmpF9)!= 0)goto _LL9E;_tmpFB=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpF9)->f1;if(_tmpFB <= (void*)1?1:*((
int*)_tmpFB)!= 0)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(_tmpF9 <= (void*)1?1:*((int*)
_tmpF9)!= 0)goto _LLA0;_tmpFC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpF9)->f1;
if(_tmpFC <= (void*)1?1:*((int*)_tmpFC)!= 1)goto _LLA0;_LL9F: return(void*)2;_LLA0:
if(_tmpF9 <= (void*)1?1:*((int*)_tmpF9)!= 1)goto _LL97;_LLA1:({void*_tmpFD[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmpFE="typ_kind: forward constr in ptr bounds";_tag_arr(_tmpFE,sizeof(char),
_get_zero_arr_size(_tmpFE,39));}),_tag_arr(_tmpFD,sizeof(void*),0));});_LL97:;}
_LL85: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 14)goto _LL87;_LL86: return(void*)2;
_LL87: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 18)goto _LL89;_LL88: return(void*)5;
_LL89: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 17)goto _LL8B;_LL8A: return(void*)2;
_LL8B: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 7)goto _LL8D;_LL8C: goto _LL8E;_LL8D:
if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 9)goto _LL8F;_LL8E: return(void*)1;_LL8F:
if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 16)goto _LL91;_tmpF6=((struct Cyc_Absyn_TypedefType_struct*)
_tmpE1)->f3;_LL90: if(_tmpF6 == 0?1: _tmpF6->kind == 0)({struct Cyc_String_pa_struct
_tmp101;_tmp101.tag=0;_tmp101.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmpFF[1]={& _tmp101};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp100="typ_kind: typedef found: %s";_tag_arr(
_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,28));}),_tag_arr(_tmpFF,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6->kind))->v;
_LL91: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 19)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 20)goto _LL95;_LL94: goto _LL96;
_LL95: if(_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 21)goto _LL5E;_LL96: return(void*)4;
_LL5E:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp102;
_push_handler(& _tmp102);{int _tmp104=0;if(setjmp(_tmp102.handler))_tmp104=1;if(!
_tmp104){Cyc_Tcutil_unify_it(t1,t2);{int _tmp105=1;_npop_handler(0);return _tmp105;};
_pop_handler();}else{void*_tmp103=(void*)_exn_thrown;void*_tmp107=_tmp103;_LLA3:
if(_tmp107 != Cyc_Tcutil_Unify)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6:(void)_throw(
_tmp107);_LLA2:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmp108=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp109;struct Cyc_Core_Opt*_tmp10A;struct Cyc_Core_Opt*
_tmp10B;struct Cyc_Core_Opt**_tmp10C;struct Cyc_Absyn_PtrInfo _tmp10D;struct Cyc_Absyn_ArrayInfo
_tmp10E;void*_tmp10F;struct Cyc_Absyn_FnInfo _tmp110;struct Cyc_List_List*_tmp111;
struct Cyc_Core_Opt*_tmp112;void*_tmp113;struct Cyc_List_List*_tmp114;int _tmp115;
struct Cyc_Absyn_VarargInfo*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*
_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_TunionInfo _tmp11A;struct Cyc_List_List*
_tmp11B;void*_tmp11C;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_TunionFieldInfo
_tmp11E;struct Cyc_List_List*_tmp11F;struct Cyc_Absyn_AggrInfo _tmp120;struct Cyc_List_List*
_tmp121;struct Cyc_List_List*_tmp122;void*_tmp123;void*_tmp124;void*_tmp125;void*
_tmp126;void*_tmp127;struct Cyc_List_List*_tmp128;_LLA8: if(_tmp108 <= (void*)3?1:*((
int*)_tmp108)!= 1)goto _LLAA;_tmp109=((struct Cyc_Absyn_VarType_struct*)_tmp108)->f1;
_LLA9: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp109)){Cyc_Tcutil_failure_reason=({const char*_tmp129="(type variable would escape scope)";
_tag_arr(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,35));});(int)_throw((
void*)Cyc_Tcutil_Unify);}goto _LLA7;_LLAA: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)
!= 0)goto _LLAC;_tmp10A=((struct Cyc_Absyn_Evar_struct*)_tmp108)->f2;_tmp10B=((
struct Cyc_Absyn_Evar_struct*)_tmp108)->f4;_tmp10C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp108)->f4;_LLAB: if(t == evar){Cyc_Tcutil_failure_reason=({
const char*_tmp12A="(occurs check)";_tag_arr(_tmp12A,sizeof(char),
_get_zero_arr_size(_tmp12A,15));});(int)_throw((void*)Cyc_Tcutil_Unify);}else{
if(_tmp10A != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp10A->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp10C))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp12B=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp10C))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd))_tmp12B=({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->hd=(
struct Cyc_Absyn_Tvar*)s->hd;_tmp12C->tl=_tmp12B;_tmp12C;});}}*_tmp10C=({struct
Cyc_Core_Opt*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->v=_tmp12B;_tmp12D;});}}}
goto _LLA7;_LLAC: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 4)goto _LLAE;_tmp10D=((
struct Cyc_Absyn_PointerType_struct*)_tmp108)->f1;_LLAD: Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp10D.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp10D.ptr_atts).rgn);{
void*_tmp12E=(void*)(Cyc_Absyn_compress_conref((_tmp10D.ptr_atts).bounds))->v;
void*_tmp12F;void*_tmp130;_LLCD: if(_tmp12E <= (void*)1?1:*((int*)_tmp12E)!= 0)
goto _LLCF;_tmp12F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp12E)->f1;if(
_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 1)goto _LLCF;_tmp130=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp12F)->f1;_LLCE: Cyc_Tcutil_occurs(evar,r,env,_tmp130);goto _LLCC;_LLCF:;_LLD0:
goto _LLCC;_LLCC:;}goto _LLA7;_LLAE: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 7)
goto _LLB0;_tmp10E=((struct Cyc_Absyn_ArrayType_struct*)_tmp108)->f1;_tmp10F=(void*)
_tmp10E.elt_type;_LLAF: Cyc_Tcutil_occurs(evar,r,env,_tmp10F);goto _LLA7;_LLB0: if(
_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 8)goto _LLB2;_tmp110=((struct Cyc_Absyn_FnType_struct*)
_tmp108)->f1;_tmp111=_tmp110.tvars;_tmp112=_tmp110.effect;_tmp113=(void*)_tmp110.ret_typ;
_tmp114=_tmp110.args;_tmp115=_tmp110.c_varargs;_tmp116=_tmp110.cyc_varargs;
_tmp117=_tmp110.rgn_po;_tmp118=_tmp110.attributes;_LLB1: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp111,env);if(_tmp112 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp112->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp113);for(0;_tmp114 != 0;_tmp114=_tmp114->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp114->hd)).f3);}if(_tmp116 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp116->type);for(0;_tmp117 != 0;_tmp117=_tmp117->tl){struct
_tuple6 _tmp132;void*_tmp133;void*_tmp134;struct _tuple6*_tmp131=(struct _tuple6*)
_tmp117->hd;_tmp132=*_tmp131;_tmp133=_tmp132.f1;_tmp134=_tmp132.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp133);Cyc_Tcutil_occurs(evar,r,env,_tmp134);}goto _LLA7;_LLB2: if(
_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 9)goto _LLB4;_tmp119=((struct Cyc_Absyn_TupleType_struct*)
_tmp108)->f1;_LLB3: for(0;_tmp119 != 0;_tmp119=_tmp119->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp119->hd)).f2);}goto _LLA7;_LLB4: if(_tmp108 <= (void*)
3?1:*((int*)_tmp108)!= 2)goto _LLB6;_tmp11A=((struct Cyc_Absyn_TunionType_struct*)
_tmp108)->f1;_tmp11B=_tmp11A.targs;_tmp11C=(void*)_tmp11A.rgn;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,_tmp11C);Cyc_Tcutil_occurslist(evar,r,env,_tmp11B);goto _LLA7;_LLB6:
if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 16)goto _LLB8;_tmp11D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp108)->f2;_LLB7: _tmp11F=_tmp11D;goto _LLB9;_LLB8: if(_tmp108 <= (void*)3?1:*((
int*)_tmp108)!= 3)goto _LLBA;_tmp11E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp108)->f1;_tmp11F=_tmp11E.targs;_LLB9: _tmp121=_tmp11F;goto _LLBB;_LLBA: if(
_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 10)goto _LLBC;_tmp120=((struct Cyc_Absyn_AggrType_struct*)
_tmp108)->f1;_tmp121=_tmp120.targs;_LLBB: Cyc_Tcutil_occurslist(evar,r,env,
_tmp121);goto _LLA7;_LLBC: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 11)goto _LLBE;
_tmp122=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp108)->f2;_LLBD: for(0;_tmp122
!= 0;_tmp122=_tmp122->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp122->hd)->type);}goto _LLA7;_LLBE: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 
17)goto _LLC0;_tmp123=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp108)->f1;_LLBF:
_tmp124=_tmp123;goto _LLC1;_LLC0: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 14)
goto _LLC2;_tmp124=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp108)->f1;_LLC1:
_tmp125=_tmp124;goto _LLC3;_LLC2: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 19)
goto _LLC4;_tmp125=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp108)->f1;_LLC3:
_tmp126=_tmp125;goto _LLC5;_LLC4: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 15)
goto _LLC6;_tmp126=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp108)->f1;
_LLC5: _tmp127=_tmp126;goto _LLC7;_LLC6: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 
21)goto _LLC8;_tmp127=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp108)->f1;_LLC7:
Cyc_Tcutil_occurs(evar,r,env,_tmp127);goto _LLA7;_LLC8: if(_tmp108 <= (void*)3?1:*((
int*)_tmp108)!= 20)goto _LLCA;_tmp128=((struct Cyc_Absyn_JoinEff_struct*)_tmp108)->f1;
_LLC9: Cyc_Tcutil_occurslist(evar,r,env,_tmp128);goto _LLA7;_LLCA:;_LLCB: goto _LLA7;
_LLA7:;}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){if((tq1.q_const != tq2.q_const?1: tq1.q_volatile != tq2.q_volatile)?1: tq1.q_restrict
!= tq2.q_restrict){Cyc_Tcutil_failure_reason=({const char*_tmp135="(qualifiers don't match)";
_tag_arr(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?tq1.q_volatile == tq2.q_volatile:
0)?tq1.q_restrict == tq2.q_restrict: 0;}static void Cyc_Tcutil_unify_it_conrefs(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp136=(void*)x->v;void*_tmp137;_LLD2: if((int)_tmp136 != 0)
goto _LLD4;_LLD3:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp139;_tmp139.tag=1;_tmp139.f1=y;_tmp139;});_tmp138;})));return;_LLD4: if(
_tmp136 <= (void*)1?1:*((int*)_tmp136)!= 0)goto _LLD6;_tmp137=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp136)->f1;_LLD5: {void*_tmp13A=(void*)y->v;void*_tmp13B;_LLD9: if((int)_tmp13A
!= 0)goto _LLDB;_LLDA:(void*)(y->v=(void*)((void*)x->v));return;_LLDB: if(_tmp13A
<= (void*)1?1:*((int*)_tmp13A)!= 0)goto _LLDD;_tmp13B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp13A)->f1;_LLDC: if(cmp(_tmp137,_tmp13B)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLDD: if(_tmp13A <= (void*)1?1:*((int*)
_tmp13A)!= 1)goto _LLD8;_LLDE:({void*_tmp13C[0]={};Cyc_Tcutil_impos(({const char*
_tmp13D="unify_conref: forward after compress(2)";_tag_arr(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,40));}),_tag_arr(_tmp13C,sizeof(void*),0));});_LLD8:;}
_LLD6: if(_tmp136 <= (void*)1?1:*((int*)_tmp136)!= 1)goto _LLD1;_LLD7:({void*
_tmp13E[0]={};Cyc_Tcutil_impos(({const char*_tmp13F="unify_conref: forward after compress";
_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,37));}),_tag_arr(
_tmp13E,sizeof(void*),0));});_LLD1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp140;_push_handler(& _tmp140);{int _tmp142=0;if(setjmp(_tmp140.handler))
_tmp142=1;if(!_tmp142){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp143=1;_npop_handler(0);return _tmp143;};_pop_handler();}
else{void*_tmp141=(void*)_exn_thrown;void*_tmp145=_tmp141;_LLE0: if(_tmp145 != Cyc_Tcutil_Unify)
goto _LLE2;_LLE1: return 0;_LLE2:;_LLE3:(void)_throw(_tmp145);_LLDF:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp147=({struct _tuple6
_tmp146;_tmp146.f1=b1;_tmp146.f2=b2;_tmp146;});void*_tmp148;void*_tmp149;void*
_tmp14A;void*_tmp14B;void*_tmp14C;struct Cyc_Absyn_Exp*_tmp14D;void*_tmp14E;
struct Cyc_Absyn_Exp*_tmp14F;void*_tmp150;void*_tmp151;void*_tmp152;void*_tmp153;
void*_tmp154;void*_tmp155;void*_tmp156;void*_tmp157;_LLE5: _tmp148=_tmp147.f1;if((
int)_tmp148 != 0)goto _LLE7;_tmp149=_tmp147.f2;if((int)_tmp149 != 0)goto _LLE7;_LLE6:
return 0;_LLE7: _tmp14A=_tmp147.f1;if((int)_tmp14A != 0)goto _LLE9;_LLE8: return - 1;
_LLE9: _tmp14B=_tmp147.f2;if((int)_tmp14B != 0)goto _LLEB;_LLEA: return 1;_LLEB:
_tmp14C=_tmp147.f1;if(_tmp14C <= (void*)1?1:*((int*)_tmp14C)!= 0)goto _LLED;
_tmp14D=((struct Cyc_Absyn_Upper_b_struct*)_tmp14C)->f1;_tmp14E=_tmp147.f2;if(
_tmp14E <= (void*)1?1:*((int*)_tmp14E)!= 0)goto _LLED;_tmp14F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp14E)->f1;_LLEC: return Cyc_Evexp_const_exp_cmp(_tmp14D,_tmp14F);_LLED: _tmp150=
_tmp147.f1;if(_tmp150 <= (void*)1?1:*((int*)_tmp150)!= 0)goto _LLEF;_tmp151=
_tmp147.f2;if(_tmp151 <= (void*)1?1:*((int*)_tmp151)!= 1)goto _LLEF;_LLEE: return - 1;
_LLEF: _tmp152=_tmp147.f1;if(_tmp152 <= (void*)1?1:*((int*)_tmp152)!= 1)goto _LLF1;
_tmp153=_tmp147.f2;if(_tmp153 <= (void*)1?1:*((int*)_tmp153)!= 0)goto _LLF1;_LLF0:
return 1;_LLF1: _tmp154=_tmp147.f1;if(_tmp154 <= (void*)1?1:*((int*)_tmp154)!= 1)
goto _LLE4;_tmp155=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp154)->f1;
_tmp156=_tmp147.f2;if(_tmp156 <= (void*)1?1:*((int*)_tmp156)!= 1)goto _LLE4;
_tmp157=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp156)->f1;_LLF2: if(Cyc_Tcutil_unify(
_tmp155,_tmp157))return 0;return Cyc_Tcutil_typecmp(_tmp155,_tmp157);_LLE4:;}
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp158=att;_LLF4: if(
_tmp158 <= (void*)17?1:*((int*)_tmp158)!= 0)goto _LLF6;_LLF5: return 0;_LLF6: if((int)
_tmp158 != 0)goto _LLF8;_LLF7: return 1;_LLF8: if((int)_tmp158 != 1)goto _LLFA;_LLF9:
return 2;_LLFA: if((int)_tmp158 != 2)goto _LLFC;_LLFB: return 3;_LLFC: if((int)_tmp158
!= 3)goto _LLFE;_LLFD: return 4;_LLFE: if((int)_tmp158 != 4)goto _LL100;_LLFF: return 5;
_LL100: if(_tmp158 <= (void*)17?1:*((int*)_tmp158)!= 1)goto _LL102;_LL101: return 6;
_LL102: if((int)_tmp158 != 5)goto _LL104;_LL103: return 7;_LL104: if(_tmp158 <= (void*)
17?1:*((int*)_tmp158)!= 2)goto _LL106;_LL105: return 8;_LL106: if((int)_tmp158 != 6)
goto _LL108;_LL107: return 9;_LL108: if((int)_tmp158 != 7)goto _LL10A;_LL109: return 10;
_LL10A: if((int)_tmp158 != 8)goto _LL10C;_LL10B: return 11;_LL10C: if((int)_tmp158 != 9)
goto _LL10E;_LL10D: return 12;_LL10E: if((int)_tmp158 != 10)goto _LL110;_LL10F: return
13;_LL110: if((int)_tmp158 != 11)goto _LL112;_LL111: return 14;_LL112: if((int)_tmp158
!= 12)goto _LL114;_LL113: return 15;_LL114: if((int)_tmp158 != 13)goto _LL116;_LL115:
return 16;_LL116: if((int)_tmp158 != 14)goto _LL118;_LL117: return 17;_LL118: if((int)
_tmp158 != 15)goto _LL11A;_LL119: return 18;_LL11A: if(_tmp158 <= (void*)17?1:*((int*)
_tmp158)!= 3)goto _LL11C;_LL11B: return 19;_LL11C: if(_tmp158 <= (void*)17?1:*((int*)
_tmp158)!= 4)goto _LL11E;_LL11D: return 20;_LL11E:;_LL11F: return 21;_LLF3:;}static
int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct _tuple6 _tmp15A=({struct
_tuple6 _tmp159;_tmp159.f1=att1;_tmp159.f2=att2;_tmp159;});void*_tmp15B;int
_tmp15C;void*_tmp15D;int _tmp15E;void*_tmp15F;int _tmp160;void*_tmp161;int _tmp162;
void*_tmp163;int _tmp164;void*_tmp165;int _tmp166;void*_tmp167;struct _tagged_arr
_tmp168;void*_tmp169;struct _tagged_arr _tmp16A;void*_tmp16B;void*_tmp16C;int
_tmp16D;int _tmp16E;void*_tmp16F;void*_tmp170;int _tmp171;int _tmp172;_LL121:
_tmp15B=_tmp15A.f1;if(_tmp15B <= (void*)17?1:*((int*)_tmp15B)!= 0)goto _LL123;
_tmp15C=((struct Cyc_Absyn_Regparm_att_struct*)_tmp15B)->f1;_tmp15D=_tmp15A.f2;
if(_tmp15D <= (void*)17?1:*((int*)_tmp15D)!= 0)goto _LL123;_tmp15E=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp15D)->f1;_LL122: _tmp160=_tmp15C;_tmp162=_tmp15E;goto _LL124;_LL123: _tmp15F=
_tmp15A.f1;if(_tmp15F <= (void*)17?1:*((int*)_tmp15F)!= 4)goto _LL125;_tmp160=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp15F)->f1;_tmp161=_tmp15A.f2;if(
_tmp161 <= (void*)17?1:*((int*)_tmp161)!= 4)goto _LL125;_tmp162=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp161)->f1;_LL124: _tmp164=_tmp160;_tmp166=_tmp162;goto _LL126;_LL125: _tmp163=
_tmp15A.f1;if(_tmp163 <= (void*)17?1:*((int*)_tmp163)!= 1)goto _LL127;_tmp164=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp163)->f1;_tmp165=_tmp15A.f2;if(_tmp165 <= (
void*)17?1:*((int*)_tmp165)!= 1)goto _LL127;_tmp166=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp165)->f1;_LL126: return Cyc_Core_intcmp(_tmp164,_tmp166);_LL127: _tmp167=
_tmp15A.f1;if(_tmp167 <= (void*)17?1:*((int*)_tmp167)!= 2)goto _LL129;_tmp168=((
struct Cyc_Absyn_Section_att_struct*)_tmp167)->f1;_tmp169=_tmp15A.f2;if(_tmp169 <= (
void*)17?1:*((int*)_tmp169)!= 2)goto _LL129;_tmp16A=((struct Cyc_Absyn_Section_att_struct*)
_tmp169)->f1;_LL128: return Cyc_strcmp((struct _tagged_arr)_tmp168,(struct
_tagged_arr)_tmp16A);_LL129: _tmp16B=_tmp15A.f1;if(_tmp16B <= (void*)17?1:*((int*)
_tmp16B)!= 3)goto _LL12B;_tmp16C=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp16B)->f1;_tmp16D=((struct Cyc_Absyn_Format_att_struct*)_tmp16B)->f2;_tmp16E=((
struct Cyc_Absyn_Format_att_struct*)_tmp16B)->f3;_tmp16F=_tmp15A.f2;if(_tmp16F <= (
void*)17?1:*((int*)_tmp16F)!= 3)goto _LL12B;_tmp170=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp16F)->f1;_tmp171=((struct Cyc_Absyn_Format_att_struct*)_tmp16F)->f2;_tmp172=((
struct Cyc_Absyn_Format_att_struct*)_tmp16F)->f3;_LL12A: {int _tmp173=Cyc_Core_intcmp((
int)((unsigned int)_tmp16C),(int)((unsigned int)_tmp170));if(_tmp173 != 0)return
_tmp173;{int _tmp174=Cyc_Core_intcmp(_tmp16D,_tmp171);if(_tmp174 != 0)return
_tmp174;return Cyc_Core_intcmp(_tmp16E,_tmp172);}}_LL12B:;_LL12C: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL120:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp175=Cyc_Tcutil_tvar_kind(tv);_LL12E:
if((int)_tmp175 != 3)goto _LL130;_LL12F: t=(void*)2;goto _LL12D;_LL130: if((int)
_tmp175 != 4)goto _LL132;_LL131: t=Cyc_Absyn_empty_effect;goto _LL12D;_LL132: if((int)
_tmp175 != 5)goto _LL134;_LL133: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp176=
_cycalloc_atomic(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp177;_tmp177.tag=18;_tmp177.f1=0;_tmp177;});_tmp176;});goto _LL12D;_LL134:;
_LL135: t=Cyc_Absyn_sint_typ;goto _LL12D;_LL12D:;}return({struct _tuple8*_tmp178=
_cycalloc(sizeof(*_tmp178));_tmp178->f1=tv;_tmp178->f2=t;_tmp178;});}static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp179=Cyc_Tcutil_compress(t);void*_tmp17A;
struct Cyc_Absyn_TunionInfo _tmp17B;struct Cyc_List_List*_tmp17C;void*_tmp17D;
struct Cyc_Absyn_PtrInfo _tmp17E;void*_tmp17F;struct Cyc_Absyn_PtrAtts _tmp180;void*
_tmp181;struct Cyc_Absyn_ArrayInfo _tmp182;void*_tmp183;struct Cyc_List_List*
_tmp184;struct Cyc_Absyn_TunionFieldInfo _tmp185;struct Cyc_List_List*_tmp186;
struct Cyc_Absyn_AggrInfo _tmp187;struct Cyc_List_List*_tmp188;struct Cyc_List_List*
_tmp189;void*_tmp18A;struct Cyc_Absyn_FnInfo _tmp18B;struct Cyc_List_List*_tmp18C;
struct Cyc_Core_Opt*_tmp18D;void*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_VarargInfo*
_tmp190;struct Cyc_List_List*_tmp191;void*_tmp192;struct Cyc_List_List*_tmp193;
_LL137: if((int)_tmp179 != 0)goto _LL139;_LL138: goto _LL13A;_LL139: if((int)_tmp179 != 
1)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 
6)goto _LL13D;_LL13C: goto _LL13E;_LL13D: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 
12)goto _LL13F;_LL13E: goto _LL140;_LL13F: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)
!= 13)goto _LL141;_LL140: goto _LL142;_LL141: if(_tmp179 <= (void*)3?1:*((int*)
_tmp179)!= 18)goto _LL143;_LL142: goto _LL144;_LL143: if(_tmp179 <= (void*)3?1:*((int*)
_tmp179)!= 5)goto _LL145;_LL144: return Cyc_Absyn_empty_effect;_LL145: if(_tmp179 <= (
void*)3?1:*((int*)_tmp179)!= 0)goto _LL147;_LL146: goto _LL148;_LL147: if(_tmp179 <= (
void*)3?1:*((int*)_tmp179)!= 1)goto _LL149;_LL148: {void*_tmp194=Cyc_Tcutil_typ_kind(
t);_LL16A: if((int)_tmp194 != 3)goto _LL16C;_LL16B: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp196;_tmp196.tag=19;_tmp196.f1=(void*)t;_tmp196;});_tmp195;});_LL16C: if((int)
_tmp194 != 4)goto _LL16E;_LL16D: return t;_LL16E: if((int)_tmp194 != 5)goto _LL170;
_LL16F: return Cyc_Absyn_empty_effect;_LL170:;_LL171: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp198;_tmp198.tag=21;_tmp198.f1=(void*)t;_tmp198;});_tmp197;});_LL169:;}_LL149:
if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 15)goto _LL14B;_tmp17A=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp179)->f1;_LL14A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp19A;_tmp19A.tag=19;_tmp19A.f1=(void*)_tmp17A;_tmp19A;});_tmp199;});_LL14B:
if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 2)goto _LL14D;_tmp17B=((struct Cyc_Absyn_TunionType_struct*)
_tmp179)->f1;_tmp17C=_tmp17B.targs;_tmp17D=(void*)_tmp17B.rgn;_LL14C: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));
_tmp19E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp19F;_tmp19F.tag=19;_tmp19F.f1=(
void*)_tmp17D;_tmp19F;});_tmp19E;}));_tmp19D->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp17C);_tmp19D;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp19C;_tmp19C.tag=20;_tmp19C.f1=ts;_tmp19C;});_tmp19B;}));}_LL14D: if(_tmp179 <= (
void*)3?1:*((int*)_tmp179)!= 4)goto _LL14F;_tmp17E=((struct Cyc_Absyn_PointerType_struct*)
_tmp179)->f1;_tmp17F=(void*)_tmp17E.elt_typ;_tmp180=_tmp17E.ptr_atts;_tmp181=(
void*)_tmp180.rgn;_LL14E: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1A1;_tmp1A1.tag=20;_tmp1A1.f1=({void*_tmp1A2[2];_tmp1A2[1]=Cyc_Tcutil_rgns_of(
_tmp17F);_tmp1A2[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1A3=_cycalloc(
sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1A4;_tmp1A4.tag=
19;_tmp1A4.f1=(void*)_tmp181;_tmp1A4;});_tmp1A3;});Cyc_List_list(_tag_arr(
_tmp1A2,sizeof(void*),2));});_tmp1A1;});_tmp1A0;}));_LL14F: if(_tmp179 <= (void*)3?
1:*((int*)_tmp179)!= 7)goto _LL151;_tmp182=((struct Cyc_Absyn_ArrayType_struct*)
_tmp179)->f1;_tmp183=(void*)_tmp182.elt_type;_LL150: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp183));_LL151: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 9)
goto _LL153;_tmp184=((struct Cyc_Absyn_TupleType_struct*)_tmp179)->f1;_LL152: {
struct Cyc_List_List*_tmp1A5=0;for(0;_tmp184 != 0;_tmp184=_tmp184->tl){_tmp1A5=({
struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->hd=(void*)(*((
struct _tuple4*)_tmp184->hd)).f2;_tmp1A6->tl=_tmp1A5;_tmp1A6;});}_tmp186=_tmp1A5;
goto _LL154;}_LL153: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 3)goto _LL155;
_tmp185=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp179)->f1;_tmp186=_tmp185.targs;
_LL154: _tmp188=_tmp186;goto _LL156;_LL155: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)
!= 10)goto _LL157;_tmp187=((struct Cyc_Absyn_AggrType_struct*)_tmp179)->f1;_tmp188=
_tmp187.targs;_LL156: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1A8;_tmp1A8.tag=20;_tmp1A8.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp188);
_tmp1A8;});_tmp1A7;}));_LL157: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 11)goto
_LL159;_tmp189=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp179)->f2;_LL158: return
Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1A9=
_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1AA;
_tmp1AA.tag=20;_tmp1AA.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp189);_tmp1AA;});
_tmp1A9;}));_LL159: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 14)goto _LL15B;
_tmp18A=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp179)->f1;_LL15A: return
Cyc_Tcutil_rgns_of(_tmp18A);_LL15B: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 17)
goto _LL15D;_LL15C: return Cyc_Tcutil_rgns_of(t);_LL15D: if(_tmp179 <= (void*)3?1:*((
int*)_tmp179)!= 8)goto _LL15F;_tmp18B=((struct Cyc_Absyn_FnType_struct*)_tmp179)->f1;
_tmp18C=_tmp18B.tvars;_tmp18D=_tmp18B.effect;_tmp18E=(void*)_tmp18B.ret_typ;
_tmp18F=_tmp18B.args;_tmp190=_tmp18B.cyc_varargs;_tmp191=_tmp18B.rgn_po;_LL15E: {
void*_tmp1AB=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp18C),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp18D))->v);return Cyc_Tcutil_normalize_effect(
_tmp1AB);}_LL15F: if((int)_tmp179 != 2)goto _LL161;_LL160: return Cyc_Absyn_empty_effect;
_LL161: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 19)goto _LL163;_LL162: goto
_LL164;_LL163: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 20)goto _LL165;_LL164:
return t;_LL165: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 21)goto _LL167;_tmp192=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp179)->f1;_LL166: return Cyc_Tcutil_rgns_of(
_tmp192);_LL167: if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 16)goto _LL136;_tmp193=((
struct Cyc_Absyn_TypedefType_struct*)_tmp179)->f2;_LL168: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));
_tmp1AC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1AD;_tmp1AD.tag=20;_tmp1AD.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp193);_tmp1AD;});_tmp1AC;}));_LL136:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1AE=e;struct Cyc_List_List*_tmp1AF;
struct Cyc_List_List**_tmp1B0;void*_tmp1B1;_LL173: if(_tmp1AE <= (void*)3?1:*((int*)
_tmp1AE)!= 20)goto _LL175;_tmp1AF=((struct Cyc_Absyn_JoinEff_struct*)_tmp1AE)->f1;
_tmp1B0=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp1AE)->f1;
_LL174: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp1B0;for(0;effs != 0;effs=
effs->tl){void*_tmp1B2=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp1B2)));{void*_tmp1B3=(void*)effs->hd;void*
_tmp1B4;_LL17A: if(_tmp1B3 <= (void*)3?1:*((int*)_tmp1B3)!= 20)goto _LL17C;_LL17B:
goto _LL17D;_LL17C: if(_tmp1B3 <= (void*)3?1:*((int*)_tmp1B3)!= 19)goto _LL17E;
_tmp1B4=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1B3)->f1;if((int)_tmp1B4
!= 2)goto _LL17E;_LL17D: redo_join=1;goto _LL179;_LL17E:;_LL17F: goto _LL179;_LL179:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp1B0;for(0;effs != 0;effs=effs->tl){void*_tmp1B5=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp1B6;void*_tmp1B7;_LL181: if(_tmp1B5 <= (void*)3?
1:*((int*)_tmp1B5)!= 20)goto _LL183;_tmp1B6=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1B5)->f1;_LL182: effects=Cyc_List_revappend(_tmp1B6,effects);goto _LL180;
_LL183: if(_tmp1B5 <= (void*)3?1:*((int*)_tmp1B5)!= 19)goto _LL185;_tmp1B7=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1B5)->f1;if((int)_tmp1B7 != 2)goto _LL185;
_LL184: goto _LL180;_LL185:;_LL186: effects=({struct Cyc_List_List*_tmp1B8=_cycalloc(
sizeof(*_tmp1B8));_tmp1B8->hd=(void*)_tmp1B5;_tmp1B8->tl=effects;_tmp1B8;});goto
_LL180;_LL180:;}}*_tmp1B0=Cyc_List_imp_rev(effects);return e;}}_LL175: if(_tmp1AE
<= (void*)3?1:*((int*)_tmp1AE)!= 21)goto _LL177;_tmp1B1=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1AE)->f1;_LL176: {void*_tmp1B9=Cyc_Tcutil_compress(_tmp1B1);_LL188: if(
_tmp1B9 <= (void*)3?1:*((int*)_tmp1B9)!= 0)goto _LL18A;_LL189: goto _LL18B;_LL18A:
if(_tmp1B9 <= (void*)3?1:*((int*)_tmp1B9)!= 1)goto _LL18C;_LL18B: return e;_LL18C:;
_LL18D: return Cyc_Tcutil_rgns_of(_tmp1B1);_LL187:;}_LL177:;_LL178: return e;_LL172:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp1BA=({struct Cyc_Absyn_FnType_struct*
_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=({struct Cyc_Absyn_FnType_struct
_tmp1BC;_tmp1BC.tag=8;_tmp1BC.f1=({struct Cyc_Absyn_FnInfo _tmp1BD;_tmp1BD.tvars=0;
_tmp1BD.effect=({struct Cyc_Core_Opt*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->v=(
void*)eff;_tmp1BE;});_tmp1BD.ret_typ=(void*)((void*)0);_tmp1BD.args=0;_tmp1BD.c_varargs=
0;_tmp1BD.cyc_varargs=0;_tmp1BD.rgn_po=0;_tmp1BD.attributes=0;_tmp1BD;});_tmp1BC;});
_tmp1BB;});return Cyc_Absyn_atb_typ((void*)_tmp1BA,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*
_tmp1BF=Cyc_Tcutil_compress(e);void*_tmp1C0;struct Cyc_List_List*_tmp1C1;void*
_tmp1C2;struct Cyc_Core_Opt*_tmp1C3;struct Cyc_Core_Opt*_tmp1C4;struct Cyc_Core_Opt**
_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;_LL18F: if(_tmp1BF <= (void*)3?1:*((int*)
_tmp1BF)!= 19)goto _LL191;_tmp1C0=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1BF)->f1;_LL190: if(constrain)return Cyc_Tcutil_unify(r,_tmp1C0);_tmp1C0=Cyc_Tcutil_compress(
_tmp1C0);if(r == _tmp1C0)return 1;{struct _tuple6 _tmp1C8=({struct _tuple6 _tmp1C7;
_tmp1C7.f1=r;_tmp1C7.f2=_tmp1C0;_tmp1C7;});void*_tmp1C9;struct Cyc_Absyn_Tvar*
_tmp1CA;void*_tmp1CB;struct Cyc_Absyn_Tvar*_tmp1CC;_LL19A: _tmp1C9=_tmp1C8.f1;if(
_tmp1C9 <= (void*)3?1:*((int*)_tmp1C9)!= 1)goto _LL19C;_tmp1CA=((struct Cyc_Absyn_VarType_struct*)
_tmp1C9)->f1;_tmp1CB=_tmp1C8.f2;if(_tmp1CB <= (void*)3?1:*((int*)_tmp1CB)!= 1)
goto _LL19C;_tmp1CC=((struct Cyc_Absyn_VarType_struct*)_tmp1CB)->f1;_LL19B: return
Cyc_Absyn_tvar_cmp(_tmp1CA,_tmp1CC)== 0;_LL19C:;_LL19D: return 0;_LL199:;}_LL191:
if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 20)goto _LL193;_tmp1C1=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1BF)->f1;_LL192: for(0;_tmp1C1 != 0;_tmp1C1=_tmp1C1->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1C1->hd))return 1;}return 0;_LL193: if(_tmp1BF <= (void*)3?1:*((
int*)_tmp1BF)!= 21)goto _LL195;_tmp1C2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1BF)->f1;_LL194: {void*_tmp1CD=Cyc_Tcutil_rgns_of(_tmp1C2);void*_tmp1CE;
_LL19F: if(_tmp1CD <= (void*)3?1:*((int*)_tmp1CD)!= 21)goto _LL1A1;_tmp1CE=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1CD)->f1;_LL1A0: if(!constrain)return 0;{void*
_tmp1CF=Cyc_Tcutil_compress(_tmp1CE);struct Cyc_Core_Opt*_tmp1D0;struct Cyc_Core_Opt*
_tmp1D1;struct Cyc_Core_Opt**_tmp1D2;struct Cyc_Core_Opt*_tmp1D3;_LL1A4: if(_tmp1CF
<= (void*)3?1:*((int*)_tmp1CF)!= 0)goto _LL1A6;_tmp1D0=((struct Cyc_Absyn_Evar_struct*)
_tmp1CF)->f1;_tmp1D1=((struct Cyc_Absyn_Evar_struct*)_tmp1CF)->f2;_tmp1D2=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1CF)->f2;_tmp1D3=((struct Cyc_Absyn_Evar_struct*)
_tmp1CF)->f4;_LL1A5: {void*_tmp1D4=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1D3);Cyc_Tcutil_occurs(_tmp1D4,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D3))->v,r);{void*_tmp1D5=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));
_tmp1D7[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1D8;_tmp1D8.tag=20;_tmp1D8.f1=({
void*_tmp1D9[2];_tmp1D9[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DA=
_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DB;
_tmp1DB.tag=19;_tmp1DB.f1=(void*)r;_tmp1DB;});_tmp1DA;});_tmp1D9[0]=_tmp1D4;Cyc_List_list(
_tag_arr(_tmp1D9,sizeof(void*),2));});_tmp1D8;});_tmp1D7;}));*_tmp1D2=({struct
Cyc_Core_Opt*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->v=(void*)_tmp1D5;
_tmp1D6;});return 1;}}_LL1A6:;_LL1A7: return 0;_LL1A3:;}_LL1A1:;_LL1A2: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1CD);_LL19E:;}_LL195: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 
0)goto _LL197;_tmp1C3=((struct Cyc_Absyn_Evar_struct*)_tmp1BF)->f1;_tmp1C4=((
struct Cyc_Absyn_Evar_struct*)_tmp1BF)->f2;_tmp1C5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1BF)->f2;_tmp1C6=((struct Cyc_Absyn_Evar_struct*)
_tmp1BF)->f4;_LL196: if(_tmp1C3 == 0?1:(void*)_tmp1C3->v != (void*)4)({void*_tmp1DC[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1DD="effect evar has wrong kind";_tag_arr(_tmp1DD,sizeof(char),
_get_zero_arr_size(_tmp1DD,27));}),_tag_arr(_tmp1DC,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp1DE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1C6);Cyc_Tcutil_occurs(_tmp1DE,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C6))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1DF=({struct Cyc_Absyn_JoinEff_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));
_tmp1E1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1E2;_tmp1E2.tag=20;_tmp1E2.f1=({
struct Cyc_List_List*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=(void*)
_tmp1DE;_tmp1E3->tl=({struct Cyc_List_List*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));
_tmp1E4->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E6;_tmp1E6.tag=
19;_tmp1E6.f1=(void*)r;_tmp1E6;});_tmp1E5;}));_tmp1E4->tl=0;_tmp1E4;});_tmp1E3;});
_tmp1E2;});_tmp1E1;});*_tmp1C5=({struct Cyc_Core_Opt*_tmp1E0=_cycalloc(sizeof(*
_tmp1E0));_tmp1E0->v=(void*)((void*)_tmp1DF);_tmp1E0;});return 1;}}_LL197:;_LL198:
return 0;_LL18E:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1E7=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp1E8;void*_tmp1E9;struct Cyc_Core_Opt*_tmp1EA;struct
Cyc_Core_Opt*_tmp1EB;struct Cyc_Core_Opt**_tmp1EC;struct Cyc_Core_Opt*_tmp1ED;
_LL1A9: if(_tmp1E7 <= (void*)3?1:*((int*)_tmp1E7)!= 19)goto _LL1AB;_LL1AA: return 0;
_LL1AB: if(_tmp1E7 <= (void*)3?1:*((int*)_tmp1E7)!= 20)goto _LL1AD;_tmp1E8=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1E7)->f1;_LL1AC: for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1E8->hd))return 1;}
return 0;_LL1AD: if(_tmp1E7 <= (void*)3?1:*((int*)_tmp1E7)!= 21)goto _LL1AF;_tmp1E9=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E7)->f1;_LL1AE: _tmp1E9=Cyc_Tcutil_compress(
_tmp1E9);if(t == _tmp1E9)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp1E9);{void*_tmp1EE=Cyc_Tcutil_rgns_of(t);void*_tmp1EF;_LL1B4: if(_tmp1EE <= (
void*)3?1:*((int*)_tmp1EE)!= 21)goto _LL1B6;_tmp1EF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EE)->f1;_LL1B5: {struct _tuple6 _tmp1F1=({struct _tuple6 _tmp1F0;_tmp1F0.f1=Cyc_Tcutil_compress(
_tmp1EF);_tmp1F0.f2=_tmp1E9;_tmp1F0;});void*_tmp1F2;struct Cyc_Absyn_Tvar*_tmp1F3;
void*_tmp1F4;struct Cyc_Absyn_Tvar*_tmp1F5;_LL1B9: _tmp1F2=_tmp1F1.f1;if(_tmp1F2 <= (
void*)3?1:*((int*)_tmp1F2)!= 1)goto _LL1BB;_tmp1F3=((struct Cyc_Absyn_VarType_struct*)
_tmp1F2)->f1;_tmp1F4=_tmp1F1.f2;if(_tmp1F4 <= (void*)3?1:*((int*)_tmp1F4)!= 1)
goto _LL1BB;_tmp1F5=((struct Cyc_Absyn_VarType_struct*)_tmp1F4)->f1;_LL1BA: return
Cyc_Tcutil_unify(t,_tmp1E9);_LL1BB:;_LL1BC: return _tmp1EF == _tmp1E9;_LL1B8:;}
_LL1B6:;_LL1B7: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1EE);
_LL1B3:;}_LL1AF: if(_tmp1E7 <= (void*)3?1:*((int*)_tmp1E7)!= 0)goto _LL1B1;_tmp1EA=((
struct Cyc_Absyn_Evar_struct*)_tmp1E7)->f1;_tmp1EB=((struct Cyc_Absyn_Evar_struct*)
_tmp1E7)->f2;_tmp1EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1E7)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)_tmp1E7)->f4;_LL1B0: if(
_tmp1EA == 0?1:(void*)_tmp1EA->v != (void*)4)({void*_tmp1F6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1F7="effect evar has wrong kind";
_tag_arr(_tmp1F7,sizeof(char),_get_zero_arr_size(_tmp1F7,27));}),_tag_arr(
_tmp1F6,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp1F8=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1ED);Cyc_Tcutil_occurs(_tmp1F8,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1ED))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp1F9=({struct Cyc_Absyn_JoinEff_struct*_tmp1FB=
_cycalloc(sizeof(*_tmp1FB));_tmp1FB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1FC;
_tmp1FC.tag=20;_tmp1FC.f1=({struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*
_tmp1FD));_tmp1FD->hd=(void*)_tmp1F8;_tmp1FD->tl=({struct Cyc_List_List*_tmp1FE=
_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp200;_tmp200.tag=21;_tmp200.f1=(void*)t;_tmp200;});_tmp1FF;}));_tmp1FE->tl=0;
_tmp1FE;});_tmp1FD;});_tmp1FC;});_tmp1FB;});*_tmp1EC=({struct Cyc_Core_Opt*
_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->v=(void*)((void*)_tmp1F9);_tmp1FA;});
return 1;}}_LL1B1:;_LL1B2: return 0;_LL1A8:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp201=e;struct Cyc_Absyn_Tvar*_tmp202;struct Cyc_List_List*_tmp203;void*
_tmp204;struct Cyc_Core_Opt*_tmp205;struct Cyc_Core_Opt*_tmp206;struct Cyc_Core_Opt**
_tmp207;struct Cyc_Core_Opt*_tmp208;_LL1BE: if(_tmp201 <= (void*)3?1:*((int*)
_tmp201)!= 1)goto _LL1C0;_tmp202=((struct Cyc_Absyn_VarType_struct*)_tmp201)->f1;
_LL1BF: return Cyc_Absyn_tvar_cmp(v,_tmp202)== 0;_LL1C0: if(_tmp201 <= (void*)3?1:*((
int*)_tmp201)!= 20)goto _LL1C2;_tmp203=((struct Cyc_Absyn_JoinEff_struct*)_tmp201)->f1;
_LL1C1: for(0;_tmp203 != 0;_tmp203=_tmp203->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp203->hd))return 1;}return 0;_LL1C2: if(_tmp201 <= (
void*)3?1:*((int*)_tmp201)!= 21)goto _LL1C4;_tmp204=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp201)->f1;_LL1C3: {void*_tmp209=Cyc_Tcutil_rgns_of(_tmp204);void*_tmp20A;
_LL1C9: if(_tmp209 <= (void*)3?1:*((int*)_tmp209)!= 21)goto _LL1CB;_tmp20A=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp209)->f1;_LL1CA: if(!may_constrain_evars)
return 0;{void*_tmp20B=Cyc_Tcutil_compress(_tmp20A);struct Cyc_Core_Opt*_tmp20C;
struct Cyc_Core_Opt*_tmp20D;struct Cyc_Core_Opt**_tmp20E;struct Cyc_Core_Opt*
_tmp20F;_LL1CE: if(_tmp20B <= (void*)3?1:*((int*)_tmp20B)!= 0)goto _LL1D0;_tmp20C=((
struct Cyc_Absyn_Evar_struct*)_tmp20B)->f1;_tmp20D=((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f2;_tmp20E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f2;_tmp20F=((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f4;_LL1CF: {void*
_tmp210=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp20F);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20F))->v,v))return 0;{void*_tmp211=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Absyn_JoinEff_struct _tmp214;_tmp214.tag=20;_tmp214.f1=({
void*_tmp215[2];_tmp215[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp216=
_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_VarType_struct _tmp217;
_tmp217.tag=1;_tmp217.f1=v;_tmp217;});_tmp216;});_tmp215[0]=_tmp210;Cyc_List_list(
_tag_arr(_tmp215,sizeof(void*),2));});_tmp214;});_tmp213;}));*_tmp20E=({struct
Cyc_Core_Opt*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->v=(void*)_tmp211;
_tmp212;});return 1;}}_LL1D0:;_LL1D1: return 0;_LL1CD:;}_LL1CB:;_LL1CC: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp209);_LL1C8:;}_LL1C4: if(_tmp201 <= (void*)3?1:*((int*)
_tmp201)!= 0)goto _LL1C6;_tmp205=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f1;
_tmp206=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp207=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp208=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f4;_LL1C5: if(_tmp205 == 0?1:(void*)_tmp205->v != (void*)4)({void*_tmp218[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp219="effect evar has wrong kind";_tag_arr(_tmp219,sizeof(char),
_get_zero_arr_size(_tmp219,27));}),_tag_arr(_tmp218,sizeof(void*),0));});{void*
_tmp21A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp208);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp208))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp21B=({struct Cyc_Absyn_JoinEff_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));
_tmp21D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21E;_tmp21E.tag=20;_tmp21E.f1=({
struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=(void*)
_tmp21A;_tmp21F->tl=({struct Cyc_List_List*_tmp220=_cycalloc(sizeof(*_tmp220));
_tmp220->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp221=_cycalloc(
sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Absyn_VarType_struct _tmp222;_tmp222.tag=
1;_tmp222.f1=v;_tmp222;});_tmp221;}));_tmp220->tl=0;_tmp220;});_tmp21F;});
_tmp21E;});_tmp21D;});*_tmp207=({struct Cyc_Core_Opt*_tmp21C=_cycalloc(sizeof(*
_tmp21C));_tmp21C->v=(void*)((void*)_tmp21B);_tmp21C;});return 1;}}_LL1C6:;_LL1C7:
return 0;_LL1BD:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp223=e;struct Cyc_List_List*_tmp224;void*_tmp225;_LL1D3: if(_tmp223 <= (
void*)3?1:*((int*)_tmp223)!= 20)goto _LL1D5;_tmp224=((struct Cyc_Absyn_JoinEff_struct*)
_tmp223)->f1;_LL1D4: for(0;_tmp224 != 0;_tmp224=_tmp224->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp224->hd))return 1;}return 0;_LL1D5: if(_tmp223 <= (void*)3?1:*((int*)
_tmp223)!= 21)goto _LL1D7;_tmp225=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp223)->f1;_LL1D6: {void*_tmp226=Cyc_Tcutil_rgns_of(_tmp225);void*_tmp227;
_LL1DC: if(_tmp226 <= (void*)3?1:*((int*)_tmp226)!= 21)goto _LL1DE;_tmp227=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp226)->f1;_LL1DD: return 0;_LL1DE:;_LL1DF:
return Cyc_Tcutil_evar_in_effect(evar,_tmp226);_LL1DB:;}_LL1D7: if(_tmp223 <= (void*)
3?1:*((int*)_tmp223)!= 0)goto _LL1D9;_LL1D8: return evar == e;_LL1D9:;_LL1DA: return 0;
_LL1D2:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp228=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp229;void*_tmp22A;
struct Cyc_Absyn_Tvar*_tmp22B;void*_tmp22C;struct Cyc_Core_Opt*_tmp22D;struct Cyc_Core_Opt**
_tmp22E;struct Cyc_Core_Opt*_tmp22F;_LL1E1: if(_tmp228 <= (void*)3?1:*((int*)
_tmp228)!= 20)goto _LL1E3;_tmp229=((struct Cyc_Absyn_JoinEff_struct*)_tmp228)->f1;
_LL1E2: for(0;_tmp229 != 0;_tmp229=_tmp229->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp229->hd,e2))return 0;}return 1;_LL1E3: if(_tmp228 <= (
void*)3?1:*((int*)_tmp228)!= 19)goto _LL1E5;_tmp22A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp228)->f1;_LL1E4: return Cyc_Tcutil_region_in_effect(0,_tmp22A,e2)?1:(
may_constrain_evars?Cyc_Tcutil_unify(_tmp22A,(void*)2): 0);_LL1E5: if(_tmp228 <= (
void*)3?1:*((int*)_tmp228)!= 1)goto _LL1E7;_tmp22B=((struct Cyc_Absyn_VarType_struct*)
_tmp228)->f1;_LL1E6: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp22B,e2);_LL1E7: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 21)goto _LL1E9;
_tmp22C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp228)->f1;_LL1E8: {void*
_tmp230=Cyc_Tcutil_rgns_of(_tmp22C);void*_tmp231;_LL1EE: if(_tmp230 <= (void*)3?1:*((
int*)_tmp230)!= 21)goto _LL1F0;_tmp231=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp230)->f1;_LL1EF: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp231,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp231,Cyc_Absyn_sint_typ): 0);
_LL1F0:;_LL1F1: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp230,e2);
_LL1ED:;}_LL1E9: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 0)goto _LL1EB;_tmp22D=((
struct Cyc_Absyn_Evar_struct*)_tmp228)->f2;_tmp22E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp228)->f2;_tmp22F=((struct Cyc_Absyn_Evar_struct*)
_tmp228)->f4;_LL1EA: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp22E=({struct Cyc_Core_Opt*
_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->v=(void*)Cyc_Absyn_empty_effect;
_tmp232;});return 1;_LL1EB:;_LL1EC:({struct Cyc_String_pa_struct _tmp235;_tmp235.tag=
0;_tmp235.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp233[1]={& _tmp235};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp234="subset_effect: bad effect: %s";_tag_arr(
_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,30));}),_tag_arr(_tmp233,sizeof(
void*),1));}});_LL1E0:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(
0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)?Cyc_Tcutil_subset_effect(
1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple6 _tmp237;void*_tmp238;void*_tmp239;struct _tuple6*_tmp236=(struct
_tuple6*)r1->hd;_tmp237=*_tmp236;_tmp238=_tmp237.f1;_tmp239=_tmp237.f2;{int found=
_tmp238 == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){
struct _tuple6 _tmp23B;void*_tmp23C;void*_tmp23D;struct _tuple6*_tmp23A=(struct
_tuple6*)r2->hd;_tmp23B=*_tmp23A;_tmp23C=_tmp23B.f1;_tmp23D=_tmp23B.f2;if(Cyc_Tcutil_unify(
_tmp238,_tmp23C)?Cyc_Tcutil_unify(_tmp239,_tmp23D): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp23E=t1;struct Cyc_Core_Opt*_tmp23F;struct Cyc_Core_Opt*
_tmp240;struct Cyc_Core_Opt**_tmp241;struct Cyc_Core_Opt*_tmp242;_LL1F3: if(_tmp23E
<= (void*)3?1:*((int*)_tmp23E)!= 0)goto _LL1F5;_tmp23F=((struct Cyc_Absyn_Evar_struct*)
_tmp23E)->f1;_tmp240=((struct Cyc_Absyn_Evar_struct*)_tmp23E)->f2;_tmp241=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp23E)->f2;_tmp242=((struct Cyc_Absyn_Evar_struct*)
_tmp23E)->f4;_LL1F4: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp242))->v,t2);{void*_tmp243=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp243,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp23F))->v)){*_tmp241=({struct Cyc_Core_Opt*_tmp244=_cycalloc(sizeof(*_tmp244));
_tmp244->v=(void*)t2;_tmp244;});return;}else{{void*_tmp245=t2;struct Cyc_Core_Opt*
_tmp246;struct Cyc_Core_Opt**_tmp247;struct Cyc_Core_Opt*_tmp248;struct Cyc_Absyn_PtrInfo
_tmp249;_LL1F8: if(_tmp245 <= (void*)3?1:*((int*)_tmp245)!= 0)goto _LL1FA;_tmp246=((
struct Cyc_Absyn_Evar_struct*)_tmp245)->f2;_tmp247=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp245)->f2;_tmp248=((struct Cyc_Absyn_Evar_struct*)
_tmp245)->f4;_LL1F9: {struct Cyc_List_List*_tmp24A=(struct Cyc_List_List*)_tmp242->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp248))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp24A,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp24B="(type variable would escape scope)";_tag_arr(_tmp24B,sizeof(
char),_get_zero_arr_size(_tmp24B,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp23F->v,_tmp243)){*_tmp247=({struct Cyc_Core_Opt*
_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->v=(void*)t1;_tmp24C;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp24D="(kinds are incompatible)";_tag_arr(_tmp24D,sizeof(char),
_get_zero_arr_size(_tmp24D,25));});goto _LL1F7;}_LL1FA: if(_tmp245 <= (void*)3?1:*((
int*)_tmp245)!= 4)goto _LL1FC;_tmp249=((struct Cyc_Absyn_PointerType_struct*)
_tmp245)->f1;if(!((void*)_tmp23F->v == (void*)2))goto _LL1FC;_LL1FB: {struct Cyc_Absyn_Conref*
_tmp24E=Cyc_Absyn_compress_conref((_tmp249.ptr_atts).bounds);{void*_tmp24F=(void*)
_tmp24E->v;_LL1FF: if((int)_tmp24F != 0)goto _LL201;_LL200:(void*)(_tmp24E->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp250=_cycalloc(sizeof(*_tmp250));
_tmp250[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp251;_tmp251.tag=0;_tmp251.f1=(
void*)Cyc_Absyn_bounds_one;_tmp251;});_tmp250;})));*_tmp241=({struct Cyc_Core_Opt*
_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->v=(void*)t2;_tmp252;});return;
_LL201:;_LL202: goto _LL1FE;_LL1FE:;}goto _LL1F7;}_LL1FC:;_LL1FD: goto _LL1F7;_LL1F7:;}
Cyc_Tcutil_failure_reason=({const char*_tmp253="(kinds are incompatible)";
_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL1F5:;_LL1F6: goto _LL1F2;_LL1F2:;}{struct _tuple6
_tmp255=({struct _tuple6 _tmp254;_tmp254.f1=t2;_tmp254.f2=t1;_tmp254;});void*
_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;struct Cyc_Absyn_Tvar*_tmp25A;void*
_tmp25B;struct Cyc_Absyn_Tvar*_tmp25C;void*_tmp25D;struct Cyc_Absyn_AggrInfo
_tmp25E;void*_tmp25F;struct Cyc_List_List*_tmp260;void*_tmp261;struct Cyc_Absyn_AggrInfo
_tmp262;void*_tmp263;struct Cyc_List_List*_tmp264;void*_tmp265;struct _tuple1*
_tmp266;void*_tmp267;struct _tuple1*_tmp268;void*_tmp269;struct Cyc_List_List*
_tmp26A;void*_tmp26B;struct Cyc_List_List*_tmp26C;void*_tmp26D;struct Cyc_Absyn_TunionInfo
_tmp26E;void*_tmp26F;struct Cyc_Absyn_Tuniondecl**_tmp270;struct Cyc_Absyn_Tuniondecl*
_tmp271;struct Cyc_List_List*_tmp272;void*_tmp273;void*_tmp274;struct Cyc_Absyn_TunionInfo
_tmp275;void*_tmp276;struct Cyc_Absyn_Tuniondecl**_tmp277;struct Cyc_Absyn_Tuniondecl*
_tmp278;struct Cyc_List_List*_tmp279;void*_tmp27A;void*_tmp27B;struct Cyc_Absyn_TunionFieldInfo
_tmp27C;void*_tmp27D;struct Cyc_Absyn_Tuniondecl*_tmp27E;struct Cyc_Absyn_Tunionfield*
_tmp27F;struct Cyc_List_List*_tmp280;void*_tmp281;struct Cyc_Absyn_TunionFieldInfo
_tmp282;void*_tmp283;struct Cyc_Absyn_Tuniondecl*_tmp284;struct Cyc_Absyn_Tunionfield*
_tmp285;struct Cyc_List_List*_tmp286;void*_tmp287;struct Cyc_Absyn_PtrInfo _tmp288;
void*_tmp289;struct Cyc_Absyn_Tqual _tmp28A;struct Cyc_Absyn_PtrAtts _tmp28B;void*
_tmp28C;struct Cyc_Absyn_Conref*_tmp28D;struct Cyc_Absyn_Conref*_tmp28E;struct Cyc_Absyn_Conref*
_tmp28F;void*_tmp290;struct Cyc_Absyn_PtrInfo _tmp291;void*_tmp292;struct Cyc_Absyn_Tqual
_tmp293;struct Cyc_Absyn_PtrAtts _tmp294;void*_tmp295;struct Cyc_Absyn_Conref*
_tmp296;struct Cyc_Absyn_Conref*_tmp297;struct Cyc_Absyn_Conref*_tmp298;void*
_tmp299;void*_tmp29A;void*_tmp29B;void*_tmp29C;void*_tmp29D;void*_tmp29E;void*
_tmp29F;void*_tmp2A0;void*_tmp2A1;int _tmp2A2;void*_tmp2A3;int _tmp2A4;void*
_tmp2A5;void*_tmp2A6;void*_tmp2A7;void*_tmp2A8;void*_tmp2A9;int _tmp2AA;void*
_tmp2AB;int _tmp2AC;void*_tmp2AD;void*_tmp2AE;void*_tmp2AF;void*_tmp2B0;void*
_tmp2B1;struct Cyc_Absyn_ArrayInfo _tmp2B2;void*_tmp2B3;struct Cyc_Absyn_Tqual
_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Conref*_tmp2B6;void*_tmp2B7;
struct Cyc_Absyn_ArrayInfo _tmp2B8;void*_tmp2B9;struct Cyc_Absyn_Tqual _tmp2BA;
struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Conref*_tmp2BC;void*_tmp2BD;struct
Cyc_Absyn_FnInfo _tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_Core_Opt*_tmp2C0;
void*_tmp2C1;struct Cyc_List_List*_tmp2C2;int _tmp2C3;struct Cyc_Absyn_VarargInfo*
_tmp2C4;struct Cyc_List_List*_tmp2C5;struct Cyc_List_List*_tmp2C6;void*_tmp2C7;
struct Cyc_Absyn_FnInfo _tmp2C8;struct Cyc_List_List*_tmp2C9;struct Cyc_Core_Opt*
_tmp2CA;void*_tmp2CB;struct Cyc_List_List*_tmp2CC;int _tmp2CD;struct Cyc_Absyn_VarargInfo*
_tmp2CE;struct Cyc_List_List*_tmp2CF;struct Cyc_List_List*_tmp2D0;void*_tmp2D1;
struct Cyc_List_List*_tmp2D2;void*_tmp2D3;struct Cyc_List_List*_tmp2D4;void*
_tmp2D5;void*_tmp2D6;struct Cyc_List_List*_tmp2D7;void*_tmp2D8;void*_tmp2D9;
struct Cyc_List_List*_tmp2DA;void*_tmp2DB;void*_tmp2DC;void*_tmp2DD;void*_tmp2DE;
void*_tmp2DF;void*_tmp2E0;void*_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*_tmp2E4;
void*_tmp2E5;void*_tmp2E6;_LL204: _tmp256=_tmp255.f1;if(_tmp256 <= (void*)3?1:*((
int*)_tmp256)!= 0)goto _LL206;_LL205: Cyc_Tcutil_unify_it(t2,t1);return;_LL206:
_tmp257=_tmp255.f1;if((int)_tmp257 != 0)goto _LL208;_tmp258=_tmp255.f2;if((int)
_tmp258 != 0)goto _LL208;_LL207: return;_LL208: _tmp259=_tmp255.f1;if(_tmp259 <= (
void*)3?1:*((int*)_tmp259)!= 1)goto _LL20A;_tmp25A=((struct Cyc_Absyn_VarType_struct*)
_tmp259)->f1;_tmp25B=_tmp255.f2;if(_tmp25B <= (void*)3?1:*((int*)_tmp25B)!= 1)
goto _LL20A;_tmp25C=((struct Cyc_Absyn_VarType_struct*)_tmp25B)->f1;_LL209: {
struct _tagged_arr*_tmp2E7=_tmp25A->name;struct _tagged_arr*_tmp2E8=_tmp25C->name;
int _tmp2E9=*((int*)_check_null(_tmp25A->identity));int _tmp2EA=*((int*)
_check_null(_tmp25C->identity));void*_tmp2EB=Cyc_Tcutil_tvar_kind(_tmp25A);void*
_tmp2EC=Cyc_Tcutil_tvar_kind(_tmp25C);if(_tmp2EA == _tmp2E9?Cyc_strptrcmp(_tmp2E7,
_tmp2E8)== 0: 0){if(_tmp2EB != _tmp2EC)({struct Cyc_String_pa_struct _tmp2F1;_tmp2F1.tag=
0;_tmp2F1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2EC));{struct Cyc_String_pa_struct _tmp2F0;_tmp2F0.tag=0;_tmp2F0.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp2EB));{struct Cyc_String_pa_struct
_tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp2E7);{
void*_tmp2ED[3]={& _tmp2EF,& _tmp2F0,& _tmp2F1};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp2EE="same type variable %s has kinds %s and %s";
_tag_arr(_tmp2EE,sizeof(char),_get_zero_arr_size(_tmp2EE,42));}),_tag_arr(
_tmp2ED,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp2F2="(variable types are not the same)";_tag_arr(_tmp2F2,sizeof(char),
_get_zero_arr_size(_tmp2F2,34));});goto _LL203;}_LL20A: _tmp25D=_tmp255.f1;if(
_tmp25D <= (void*)3?1:*((int*)_tmp25D)!= 10)goto _LL20C;_tmp25E=((struct Cyc_Absyn_AggrType_struct*)
_tmp25D)->f1;_tmp25F=(void*)_tmp25E.aggr_info;_tmp260=_tmp25E.targs;_tmp261=
_tmp255.f2;if(_tmp261 <= (void*)3?1:*((int*)_tmp261)!= 10)goto _LL20C;_tmp262=((
struct Cyc_Absyn_AggrType_struct*)_tmp261)->f1;_tmp263=(void*)_tmp262.aggr_info;
_tmp264=_tmp262.targs;_LL20B: {void*_tmp2F4;struct _tuple1*_tmp2F5;struct _tuple5
_tmp2F3=Cyc_Absyn_aggr_kinded_name(_tmp263);_tmp2F4=_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;{
void*_tmp2F7;struct _tuple1*_tmp2F8;struct _tuple5 _tmp2F6=Cyc_Absyn_aggr_kinded_name(
_tmp25F);_tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;if(_tmp2F4 != _tmp2F7){Cyc_Tcutil_failure_reason=({
const char*_tmp2F9="(struct and union type)";_tag_arr(_tmp2F9,sizeof(char),
_get_zero_arr_size(_tmp2F9,24));});goto _LL203;}if(Cyc_Absyn_qvar_cmp(_tmp2F5,
_tmp2F8)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp2FA="(different type name)";
_tag_arr(_tmp2FA,sizeof(char),_get_zero_arr_size(_tmp2FA,22));});goto _LL203;}Cyc_Tcutil_unify_list(
_tmp264,_tmp260);return;}}_LL20C: _tmp265=_tmp255.f1;if(_tmp265 <= (void*)3?1:*((
int*)_tmp265)!= 12)goto _LL20E;_tmp266=((struct Cyc_Absyn_EnumType_struct*)_tmp265)->f1;
_tmp267=_tmp255.f2;if(_tmp267 <= (void*)3?1:*((int*)_tmp267)!= 12)goto _LL20E;
_tmp268=((struct Cyc_Absyn_EnumType_struct*)_tmp267)->f1;_LL20D: if(Cyc_Absyn_qvar_cmp(
_tmp266,_tmp268)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp2FB="(different enum types)";
_tag_arr(_tmp2FB,sizeof(char),_get_zero_arr_size(_tmp2FB,23));});goto _LL203;
_LL20E: _tmp269=_tmp255.f1;if(_tmp269 <= (void*)3?1:*((int*)_tmp269)!= 13)goto
_LL210;_tmp26A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp269)->f1;_tmp26B=
_tmp255.f2;if(_tmp26B <= (void*)3?1:*((int*)_tmp26B)!= 13)goto _LL210;_tmp26C=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp26B)->f1;_LL20F: {int bad=0;for(0;
_tmp26A != 0?_tmp26C != 0: 0;(_tmp26A=_tmp26A->tl,_tmp26C=_tmp26C->tl)){struct Cyc_Absyn_Enumfield*
_tmp2FC=(struct Cyc_Absyn_Enumfield*)_tmp26A->hd;struct Cyc_Absyn_Enumfield*
_tmp2FD=(struct Cyc_Absyn_Enumfield*)_tmp26C->hd;if(Cyc_Absyn_qvar_cmp(_tmp2FC->name,
_tmp2FD->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp2FE="(different names for enum fields)";
_tag_arr(_tmp2FE,sizeof(char),_get_zero_arr_size(_tmp2FE,34));});bad=1;break;}
if(_tmp2FC->tag == _tmp2FD->tag)continue;if(_tmp2FC->tag == 0?1: _tmp2FD->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2FF="(different tag values for enum fields)";
_tag_arr(_tmp2FF,sizeof(char),_get_zero_arr_size(_tmp2FF,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp2FC->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp2FD->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp300="(different tag values for enum fields)";_tag_arr(_tmp300,
sizeof(char),_get_zero_arr_size(_tmp300,39));});bad=1;break;}}if(bad)goto _LL203;
if(_tmp26A == 0?_tmp26C == 0: 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp301="(different number of fields for enums)";_tag_arr(_tmp301,sizeof(char),
_get_zero_arr_size(_tmp301,39));});goto _LL203;}_LL210: _tmp26D=_tmp255.f1;if(
_tmp26D <= (void*)3?1:*((int*)_tmp26D)!= 2)goto _LL212;_tmp26E=((struct Cyc_Absyn_TunionType_struct*)
_tmp26D)->f1;_tmp26F=(void*)_tmp26E.tunion_info;if(*((int*)_tmp26F)!= 1)goto
_LL212;_tmp270=((struct Cyc_Absyn_KnownTunion_struct*)_tmp26F)->f1;_tmp271=*
_tmp270;_tmp272=_tmp26E.targs;_tmp273=(void*)_tmp26E.rgn;_tmp274=_tmp255.f2;if(
_tmp274 <= (void*)3?1:*((int*)_tmp274)!= 2)goto _LL212;_tmp275=((struct Cyc_Absyn_TunionType_struct*)
_tmp274)->f1;_tmp276=(void*)_tmp275.tunion_info;if(*((int*)_tmp276)!= 1)goto
_LL212;_tmp277=((struct Cyc_Absyn_KnownTunion_struct*)_tmp276)->f1;_tmp278=*
_tmp277;_tmp279=_tmp275.targs;_tmp27A=(void*)_tmp275.rgn;_LL211: if(_tmp271 == 
_tmp278?1: Cyc_Absyn_qvar_cmp(_tmp271->name,_tmp278->name)== 0){Cyc_Tcutil_unify_it(
_tmp27A,_tmp273);Cyc_Tcutil_unify_list(_tmp279,_tmp272);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp302="(different tunion types)";_tag_arr(_tmp302,sizeof(char),
_get_zero_arr_size(_tmp302,25));});goto _LL203;_LL212: _tmp27B=_tmp255.f1;if(
_tmp27B <= (void*)3?1:*((int*)_tmp27B)!= 3)goto _LL214;_tmp27C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp27B)->f1;_tmp27D=(void*)_tmp27C.field_info;if(*((int*)_tmp27D)!= 1)goto
_LL214;_tmp27E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp27D)->f1;_tmp27F=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp27D)->f2;_tmp280=_tmp27C.targs;
_tmp281=_tmp255.f2;if(_tmp281 <= (void*)3?1:*((int*)_tmp281)!= 3)goto _LL214;
_tmp282=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp281)->f1;_tmp283=(void*)
_tmp282.field_info;if(*((int*)_tmp283)!= 1)goto _LL214;_tmp284=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp283)->f1;_tmp285=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp283)->f2;
_tmp286=_tmp282.targs;_LL213: if((_tmp27E == _tmp284?1: Cyc_Absyn_qvar_cmp(_tmp27E->name,
_tmp284->name)== 0)?_tmp27F == _tmp285?1: Cyc_Absyn_qvar_cmp(_tmp27F->name,_tmp285->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp286,_tmp280);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp303="(different tunion field types)";_tag_arr(_tmp303,sizeof(char),
_get_zero_arr_size(_tmp303,31));});goto _LL203;_LL214: _tmp287=_tmp255.f1;if(
_tmp287 <= (void*)3?1:*((int*)_tmp287)!= 4)goto _LL216;_tmp288=((struct Cyc_Absyn_PointerType_struct*)
_tmp287)->f1;_tmp289=(void*)_tmp288.elt_typ;_tmp28A=_tmp288.elt_tq;_tmp28B=
_tmp288.ptr_atts;_tmp28C=(void*)_tmp28B.rgn;_tmp28D=_tmp28B.nullable;_tmp28E=
_tmp28B.bounds;_tmp28F=_tmp28B.zero_term;_tmp290=_tmp255.f2;if(_tmp290 <= (void*)
3?1:*((int*)_tmp290)!= 4)goto _LL216;_tmp291=((struct Cyc_Absyn_PointerType_struct*)
_tmp290)->f1;_tmp292=(void*)_tmp291.elt_typ;_tmp293=_tmp291.elt_tq;_tmp294=
_tmp291.ptr_atts;_tmp295=(void*)_tmp294.rgn;_tmp296=_tmp294.nullable;_tmp297=
_tmp294.bounds;_tmp298=_tmp294.zero_term;_LL215: Cyc_Tcutil_unify_it(_tmp292,
_tmp289);Cyc_Tcutil_unify_it(_tmp28C,_tmp295);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp298,
_tmp28F,({const char*_tmp304="(not both zero terminated)";_tag_arr(_tmp304,
sizeof(char),_get_zero_arr_size(_tmp304,27));}));Cyc_Tcutil_unify_tqual(_tmp293,
_tmp28A);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_boundscmp,_tmp297,_tmp28E,({
const char*_tmp305="(different pointer bounds)";_tag_arr(_tmp305,sizeof(char),
_get_zero_arr_size(_tmp305,27));}));{void*_tmp306=(void*)(Cyc_Absyn_compress_conref(
_tmp297))->v;void*_tmp307;_LL23D: if(_tmp306 <= (void*)1?1:*((int*)_tmp306)!= 0)
goto _LL23F;_tmp307=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp306)->f1;if((
int)_tmp307 != 0)goto _LL23F;_LL23E: return;_LL23F:;_LL240: goto _LL23C;_LL23C:;}((
void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,
struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp296,
_tmp28D,({const char*_tmp308="(incompatible pointer types)";_tag_arr(_tmp308,
sizeof(char),_get_zero_arr_size(_tmp308,29));}));return;_LL216: _tmp299=_tmp255.f1;
if(_tmp299 <= (void*)3?1:*((int*)_tmp299)!= 5)goto _LL218;_tmp29A=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp299)->f1;_tmp29B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp299)->f2;_tmp29C=_tmp255.f2;if(_tmp29C <= (void*)3?1:*((int*)_tmp29C)!= 5)
goto _LL218;_tmp29D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp29C)->f1;_tmp29E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp29C)->f2;_LL217: if(_tmp29D == _tmp29A?
_tmp29E == _tmp29B: 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp309="(different integral types)";
_tag_arr(_tmp309,sizeof(char),_get_zero_arr_size(_tmp309,27));});goto _LL203;
_LL218: _tmp29F=_tmp255.f1;if((int)_tmp29F != 1)goto _LL21A;_tmp2A0=_tmp255.f2;if((
int)_tmp2A0 != 1)goto _LL21A;_LL219: return;_LL21A: _tmp2A1=_tmp255.f1;if(_tmp2A1 <= (
void*)3?1:*((int*)_tmp2A1)!= 6)goto _LL21C;_tmp2A2=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2A1)->f1;_tmp2A3=_tmp255.f2;if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 6)
goto _LL21C;_tmp2A4=((struct Cyc_Absyn_DoubleType_struct*)_tmp2A3)->f1;_LL21B: if(
_tmp2A2 == _tmp2A4)return;goto _LL203;_LL21C: _tmp2A5=_tmp255.f1;if(_tmp2A5 <= (void*)
3?1:*((int*)_tmp2A5)!= 14)goto _LL21E;_tmp2A6=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2A5)->f1;_tmp2A7=_tmp255.f2;if(_tmp2A7 <= (void*)3?1:*((int*)_tmp2A7)!= 14)
goto _LL21E;_tmp2A8=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2A7)->f1;
_LL21D: Cyc_Tcutil_unify_it(_tmp2A6,_tmp2A8);return;_LL21E: _tmp2A9=_tmp255.f1;if(
_tmp2A9 <= (void*)3?1:*((int*)_tmp2A9)!= 18)goto _LL220;_tmp2AA=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2A9)->f1;_tmp2AB=_tmp255.f2;if(_tmp2AB <= (void*)3?1:*((int*)_tmp2AB)!= 18)
goto _LL220;_tmp2AC=((struct Cyc_Absyn_TypeInt_struct*)_tmp2AB)->f1;_LL21F: if(
_tmp2AA == _tmp2AC)return;Cyc_Tcutil_failure_reason=({const char*_tmp30A="(different type integers)";
_tag_arr(_tmp30A,sizeof(char),_get_zero_arr_size(_tmp30A,26));});goto _LL203;
_LL220: _tmp2AD=_tmp255.f1;if(_tmp2AD <= (void*)3?1:*((int*)_tmp2AD)!= 17)goto
_LL222;_tmp2AE=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2AD)->f1;_tmp2AF=
_tmp255.f2;if(_tmp2AF <= (void*)3?1:*((int*)_tmp2AF)!= 17)goto _LL222;_tmp2B0=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2AF)->f1;_LL221: Cyc_Tcutil_unify_it(
_tmp2AE,_tmp2B0);return;_LL222: _tmp2B1=_tmp255.f1;if(_tmp2B1 <= (void*)3?1:*((int*)
_tmp2B1)!= 7)goto _LL224;_tmp2B2=((struct Cyc_Absyn_ArrayType_struct*)_tmp2B1)->f1;
_tmp2B3=(void*)_tmp2B2.elt_type;_tmp2B4=_tmp2B2.tq;_tmp2B5=_tmp2B2.num_elts;
_tmp2B6=_tmp2B2.zero_term;_tmp2B7=_tmp255.f2;if(_tmp2B7 <= (void*)3?1:*((int*)
_tmp2B7)!= 7)goto _LL224;_tmp2B8=((struct Cyc_Absyn_ArrayType_struct*)_tmp2B7)->f1;
_tmp2B9=(void*)_tmp2B8.elt_type;_tmp2BA=_tmp2B8.tq;_tmp2BB=_tmp2B8.num_elts;
_tmp2BC=_tmp2B8.zero_term;_LL223: Cyc_Tcutil_unify_tqual(_tmp2BA,_tmp2B4);Cyc_Tcutil_unify_it(
_tmp2B9,_tmp2B3);((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2B6,
_tmp2BC,({const char*_tmp30B="(not both zero terminated)";_tag_arr(_tmp30B,
sizeof(char),_get_zero_arr_size(_tmp30B,27));}));if(_tmp2B5 == _tmp2BB)return;if(
_tmp2B5 == 0?1: _tmp2BB == 0)goto _LL203;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2B5),(struct Cyc_Absyn_Exp*)_check_null(_tmp2BB)))return;Cyc_Tcutil_failure_reason=({
const char*_tmp30C="(different array sizes)";_tag_arr(_tmp30C,sizeof(char),
_get_zero_arr_size(_tmp30C,24));});goto _LL203;_LL224: _tmp2BD=_tmp255.f1;if(
_tmp2BD <= (void*)3?1:*((int*)_tmp2BD)!= 8)goto _LL226;_tmp2BE=((struct Cyc_Absyn_FnType_struct*)
_tmp2BD)->f1;_tmp2BF=_tmp2BE.tvars;_tmp2C0=_tmp2BE.effect;_tmp2C1=(void*)_tmp2BE.ret_typ;
_tmp2C2=_tmp2BE.args;_tmp2C3=_tmp2BE.c_varargs;_tmp2C4=_tmp2BE.cyc_varargs;
_tmp2C5=_tmp2BE.rgn_po;_tmp2C6=_tmp2BE.attributes;_tmp2C7=_tmp255.f2;if(_tmp2C7
<= (void*)3?1:*((int*)_tmp2C7)!= 8)goto _LL226;_tmp2C8=((struct Cyc_Absyn_FnType_struct*)
_tmp2C7)->f1;_tmp2C9=_tmp2C8.tvars;_tmp2CA=_tmp2C8.effect;_tmp2CB=(void*)_tmp2C8.ret_typ;
_tmp2CC=_tmp2C8.args;_tmp2CD=_tmp2C8.c_varargs;_tmp2CE=_tmp2C8.cyc_varargs;
_tmp2CF=_tmp2C8.rgn_po;_tmp2D0=_tmp2C8.attributes;_LL225: {int done=0;{struct
_RegionHandle _tmp30D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp30D;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2C9 != 0){if(_tmp2BF == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp30E="(second function type has too few type variables)";
_tag_arr(_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp30F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2C9->hd);void*_tmp310=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2BF->hd);
if(_tmp30F != _tmp310){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp315;_tmp315.tag=0;_tmp315.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp310));{struct Cyc_String_pa_struct _tmp314;_tmp314.tag=0;_tmp314.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp30F));{struct Cyc_String_pa_struct
_tmp313;_tmp313.tag=0;_tmp313.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2C9->hd));{void*_tmp311[3]={& _tmp313,& _tmp314,& _tmp315};
Cyc_aprintf(({const char*_tmp312="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,44));}),_tag_arr(
_tmp311,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp316=_region_malloc(rgn,sizeof(*_tmp316));_tmp316->hd=({
struct _tuple8*_tmp317=_region_malloc(rgn,sizeof(*_tmp317));_tmp317->f1=(struct
Cyc_Absyn_Tvar*)_tmp2BF->hd;_tmp317->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318[0]=({struct Cyc_Absyn_VarType_struct
_tmp319;_tmp319.tag=1;_tmp319.f1=(struct Cyc_Absyn_Tvar*)_tmp2C9->hd;_tmp319;});
_tmp318;});_tmp317;});_tmp316->tl=inst;_tmp316;});_tmp2C9=_tmp2C9->tl;_tmp2BF=
_tmp2BF->tl;}}if(_tmp2BF != 0){Cyc_Tcutil_failure_reason=({const char*_tmp31A="(second function type has too many type variables)";
_tag_arr(_tmp31A,sizeof(char),_get_zero_arr_size(_tmp31A,51));});_npop_handler(0);
goto _LL203;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=({struct Cyc_Absyn_FnType_struct
_tmp31C;_tmp31C.tag=8;_tmp31C.f1=({struct Cyc_Absyn_FnInfo _tmp31D;_tmp31D.tvars=0;
_tmp31D.effect=_tmp2CA;_tmp31D.ret_typ=(void*)_tmp2CB;_tmp31D.args=_tmp2CC;
_tmp31D.c_varargs=_tmp2CD;_tmp31D.cyc_varargs=_tmp2CE;_tmp31D.rgn_po=_tmp2CF;
_tmp31D.attributes=_tmp2D0;_tmp31D;});_tmp31C;});_tmp31B;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp31E=_cycalloc(sizeof(*
_tmp31E));_tmp31E[0]=({struct Cyc_Absyn_FnType_struct _tmp31F;_tmp31F.tag=8;
_tmp31F.f1=({struct Cyc_Absyn_FnInfo _tmp320;_tmp320.tvars=0;_tmp320.effect=
_tmp2C0;_tmp320.ret_typ=(void*)_tmp2C1;_tmp320.args=_tmp2C2;_tmp320.c_varargs=
_tmp2C3;_tmp320.cyc_varargs=_tmp2C4;_tmp320.rgn_po=_tmp2C5;_tmp320.attributes=
_tmp2C6;_tmp320;});_tmp31F;});_tmp31E;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2CB,_tmp2C1);for(0;_tmp2CC != 0?_tmp2C2 != 0: 0;(
_tmp2CC=_tmp2CC->tl,_tmp2C2=_tmp2C2->tl)){Cyc_Tcutil_unify_tqual((*((struct
_tuple2*)_tmp2CC->hd)).f2,(*((struct _tuple2*)_tmp2C2->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp2CC->hd)).f3,(*((struct _tuple2*)_tmp2C2->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2CC != 0?1: _tmp2C2 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp321="(function types have different number of arguments)";_tag_arr(
_tmp321,sizeof(char),_get_zero_arr_size(_tmp321,52));});goto _LL203;}if(_tmp2CD != 
_tmp2C3){Cyc_Tcutil_failure_reason=({const char*_tmp322="(only one function type takes C varargs)";
_tag_arr(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,41));});goto _LL203;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp324=({struct _tuple11 _tmp323;_tmp323.f1=
_tmp2CE;_tmp323.f2=_tmp2C4;_tmp323;});struct Cyc_Absyn_VarargInfo*_tmp325;struct
Cyc_Absyn_VarargInfo*_tmp326;struct Cyc_Absyn_VarargInfo*_tmp327;struct Cyc_Absyn_VarargInfo*
_tmp328;struct Cyc_Absyn_VarargInfo*_tmp329;struct Cyc_Absyn_VarargInfo _tmp32A;
struct Cyc_Core_Opt*_tmp32B;struct Cyc_Absyn_Tqual _tmp32C;void*_tmp32D;int _tmp32E;
struct Cyc_Absyn_VarargInfo*_tmp32F;struct Cyc_Absyn_VarargInfo _tmp330;struct Cyc_Core_Opt*
_tmp331;struct Cyc_Absyn_Tqual _tmp332;void*_tmp333;int _tmp334;_LL242: _tmp325=
_tmp324.f1;if(_tmp325 != 0)goto _LL244;_tmp326=_tmp324.f2;if(_tmp326 != 0)goto
_LL244;_LL243: goto _LL241;_LL244: _tmp327=_tmp324.f1;if(_tmp327 != 0)goto _LL246;
_LL245: goto _LL247;_LL246: _tmp328=_tmp324.f2;if(_tmp328 != 0)goto _LL248;_LL247:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp335="(only one function type takes varargs)";
_tag_arr(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,39));});goto _LL241;
_LL248: _tmp329=_tmp324.f1;if(_tmp329 == 0)goto _LL241;_tmp32A=*_tmp329;_tmp32B=
_tmp32A.name;_tmp32C=_tmp32A.tq;_tmp32D=(void*)_tmp32A.type;_tmp32E=_tmp32A.inject;
_tmp32F=_tmp324.f2;if(_tmp32F == 0)goto _LL241;_tmp330=*_tmp32F;_tmp331=_tmp330.name;
_tmp332=_tmp330.tq;_tmp333=(void*)_tmp330.type;_tmp334=_tmp330.inject;_LL249: Cyc_Tcutil_unify_tqual(
_tmp32C,_tmp332);Cyc_Tcutil_unify_it(_tmp32D,_tmp333);if(_tmp32E != _tmp334){
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp336="(only one function type injects varargs)";
_tag_arr(_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,41));});}goto _LL241;
_LL241:;}if(bad_cyc_vararg)goto _LL203;{int bad_effect=0;{struct _tuple12 _tmp338=({
struct _tuple12 _tmp337;_tmp337.f1=_tmp2CA;_tmp337.f2=_tmp2C0;_tmp337;});struct Cyc_Core_Opt*
_tmp339;struct Cyc_Core_Opt*_tmp33A;struct Cyc_Core_Opt*_tmp33B;struct Cyc_Core_Opt*
_tmp33C;_LL24B: _tmp339=_tmp338.f1;if(_tmp339 != 0)goto _LL24D;_tmp33A=_tmp338.f2;
if(_tmp33A != 0)goto _LL24D;_LL24C: goto _LL24A;_LL24D: _tmp33B=_tmp338.f1;if(_tmp33B
!= 0)goto _LL24F;_LL24E: goto _LL250;_LL24F: _tmp33C=_tmp338.f2;if(_tmp33C != 0)goto
_LL251;_LL250: bad_effect=1;goto _LL24A;_LL251:;_LL252: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2CA))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C0))->v);goto _LL24A;_LL24A:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp33D="(function type effects do not unify)";
_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,37));});goto _LL203;}if(
!Cyc_Tcutil_same_atts(_tmp2C6,_tmp2D0)){Cyc_Tcutil_failure_reason=({const char*
_tmp33E="(function types have different attributes)";_tag_arr(_tmp33E,sizeof(
char),_get_zero_arr_size(_tmp33E,43));});goto _LL203;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2C5,_tmp2CF)){Cyc_Tcutil_failure_reason=({const char*_tmp33F="(function types have different region lifetime orderings)";
_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,58));});goto _LL203;}
return;}}}_LL226: _tmp2D1=_tmp255.f1;if(_tmp2D1 <= (void*)3?1:*((int*)_tmp2D1)!= 9)
goto _LL228;_tmp2D2=((struct Cyc_Absyn_TupleType_struct*)_tmp2D1)->f1;_tmp2D3=
_tmp255.f2;if(_tmp2D3 <= (void*)3?1:*((int*)_tmp2D3)!= 9)goto _LL228;_tmp2D4=((
struct Cyc_Absyn_TupleType_struct*)_tmp2D3)->f1;_LL227: for(0;_tmp2D4 != 0?_tmp2D2
!= 0: 0;(_tmp2D4=_tmp2D4->tl,_tmp2D2=_tmp2D2->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp2D4->hd)).f1,(*((struct _tuple4*)_tmp2D2->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp2D4->hd)).f2,(*((struct _tuple4*)_tmp2D2->hd)).f2);}if(_tmp2D4
== 0?_tmp2D2 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp340="(tuple types have different numbers of components)";_tag_arr(
_tmp340,sizeof(char),_get_zero_arr_size(_tmp340,51));});goto _LL203;_LL228:
_tmp2D5=_tmp255.f1;if(_tmp2D5 <= (void*)3?1:*((int*)_tmp2D5)!= 11)goto _LL22A;
_tmp2D6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2D5)->f1;_tmp2D7=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2D5)->f2;_tmp2D8=_tmp255.f2;if(_tmp2D8
<= (void*)3?1:*((int*)_tmp2D8)!= 11)goto _LL22A;_tmp2D9=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2D8)->f1;_tmp2DA=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2D8)->f2;_LL229:
if(_tmp2D9 != _tmp2D6){Cyc_Tcutil_failure_reason=({const char*_tmp341="(struct and union type)";
_tag_arr(_tmp341,sizeof(char),_get_zero_arr_size(_tmp341,24));});goto _LL203;}
for(0;_tmp2DA != 0?_tmp2D7 != 0: 0;(_tmp2DA=_tmp2DA->tl,_tmp2D7=_tmp2D7->tl)){
struct Cyc_Absyn_Aggrfield*_tmp342=(struct Cyc_Absyn_Aggrfield*)_tmp2DA->hd;struct
Cyc_Absyn_Aggrfield*_tmp343=(struct Cyc_Absyn_Aggrfield*)_tmp2D7->hd;if(Cyc_strptrcmp(
_tmp342->name,_tmp343->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp344="(different member names)";
_tag_arr(_tmp344,sizeof(char),_get_zero_arr_size(_tmp344,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_tqual(_tmp342->tq,_tmp343->tq);Cyc_Tcutil_unify_it((
void*)_tmp342->type,(void*)_tmp343->type);if(!Cyc_Tcutil_same_atts(_tmp342->attributes,
_tmp343->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp345="(different attributes on member)";_tag_arr(_tmp345,sizeof(
char),_get_zero_arr_size(_tmp345,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if(((_tmp342->width != 0?_tmp343->width == 0: 0)?1:(_tmp343->width != 0?_tmp342->width
== 0: 0))?1:((_tmp342->width != 0?_tmp343->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp342->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp343->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp346="(different bitfield widths on member)";
_tag_arr(_tmp346,sizeof(char),_get_zero_arr_size(_tmp346,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp2DA == 0?_tmp2D7 == 0: 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp347="(different number of members)";
_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,30));});goto _LL203;
_LL22A: _tmp2DB=_tmp255.f1;if((int)_tmp2DB != 2)goto _LL22C;_tmp2DC=_tmp255.f2;if((
int)_tmp2DC != 2)goto _LL22C;_LL22B: return;_LL22C: _tmp2DD=_tmp255.f1;if(_tmp2DD <= (
void*)3?1:*((int*)_tmp2DD)!= 15)goto _LL22E;_tmp2DE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp2DD)->f1;_tmp2DF=_tmp255.f2;if(_tmp2DF <= (void*)3?1:*((int*)_tmp2DF)!= 15)
goto _LL22E;_tmp2E0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2DF)->f1;
_LL22D: Cyc_Tcutil_unify_it(_tmp2DE,_tmp2E0);return;_LL22E: _tmp2E1=_tmp255.f1;if(
_tmp2E1 <= (void*)3?1:*((int*)_tmp2E1)!= 20)goto _LL230;_LL22F: goto _LL231;_LL230:
_tmp2E2=_tmp255.f2;if(_tmp2E2 <= (void*)3?1:*((int*)_tmp2E2)!= 20)goto _LL232;
_LL231: goto _LL233;_LL232: _tmp2E3=_tmp255.f1;if(_tmp2E3 <= (void*)3?1:*((int*)
_tmp2E3)!= 19)goto _LL234;_LL233: goto _LL235;_LL234: _tmp2E4=_tmp255.f1;if(_tmp2E4
<= (void*)3?1:*((int*)_tmp2E4)!= 21)goto _LL236;_LL235: goto _LL237;_LL236: _tmp2E5=
_tmp255.f2;if(_tmp2E5 <= (void*)3?1:*((int*)_tmp2E5)!= 21)goto _LL238;_LL237: goto
_LL239;_LL238: _tmp2E6=_tmp255.f2;if(_tmp2E6 <= (void*)3?1:*((int*)_tmp2E6)!= 19)
goto _LL23A;_LL239: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp348="(effects don't unify)";_tag_arr(_tmp348,sizeof(char),
_get_zero_arr_size(_tmp348,22));});goto _LL203;_LL23A:;_LL23B: goto _LL203;_LL203:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0:
0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp349=(tq1.q_const + (tq1.q_volatile
<< 1))+ (tq1.q_restrict << 2);int _tmp34A=(tq2.q_const + (tq2.q_volatile << 1))+ (
tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp349,_tmp34A);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp34B=(void*)x->v;
void*_tmp34C;_LL254: if((int)_tmp34B != 0)goto _LL256;_LL255: return - 1;_LL256: if(
_tmp34B <= (void*)1?1:*((int*)_tmp34B)!= 0)goto _LL258;_tmp34C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp34B)->f1;_LL257: {void*_tmp34D=(void*)y->v;void*_tmp34E;_LL25B: if((int)
_tmp34D != 0)goto _LL25D;_LL25C: return 1;_LL25D: if(_tmp34D <= (void*)1?1:*((int*)
_tmp34D)!= 0)goto _LL25F;_tmp34E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp34D)->f1;_LL25E: return cmp(_tmp34C,_tmp34E);_LL25F: if(_tmp34D <= (void*)1?1:*((
int*)_tmp34D)!= 1)goto _LL25A;_LL260:({void*_tmp34F[0]={};Cyc_Tcutil_impos(({
const char*_tmp350="unify_conref: forward after compress(2)";_tag_arr(_tmp350,
sizeof(char),_get_zero_arr_size(_tmp350,40));}),_tag_arr(_tmp34F,sizeof(void*),0));});
_LL25A:;}_LL258: if(_tmp34B <= (void*)1?1:*((int*)_tmp34B)!= 1)goto _LL253;_LL259:({
void*_tmp351[0]={};Cyc_Tcutil_impos(({const char*_tmp352="unify_conref: forward after compress";
_tag_arr(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,37));}),_tag_arr(
_tmp351,sizeof(void*),0));});_LL253:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp354;struct Cyc_Absyn_Tqual
_tmp355;void*_tmp356;struct _tuple4*_tmp353=tqt1;_tmp354=*_tmp353;_tmp355=_tmp354.f1;
_tmp356=_tmp354.f2;{struct _tuple4 _tmp358;struct Cyc_Absyn_Tqual _tmp359;void*
_tmp35A;struct _tuple4*_tmp357=tqt2;_tmp358=*_tmp357;_tmp359=_tmp358.f1;_tmp35A=
_tmp358.f2;{int _tmp35B=Cyc_Tcutil_tqual_cmp(_tmp355,_tmp359);if(_tmp35B != 0)
return _tmp35B;return Cyc_Tcutil_typecmp(_tmp356,_tmp35A);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp35C=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp35C != 0)return _tmp35C;{int _tmp35D=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp35D != 0)return _tmp35D;{int _tmp35E=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp35E != 0)return _tmp35E;{int _tmp35F=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp35F != 0)return
_tmp35F;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp360=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp360 != 0)return _tmp360;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp361=t;_LL262: if((int)_tmp361 != 0)goto _LL264;_LL263: return 0;
_LL264: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 0)goto _LL266;_LL265: return 1;
_LL266: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 1)goto _LL268;_LL267: return 2;
_LL268: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 2)goto _LL26A;_LL269: return 3;
_LL26A: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 3)goto _LL26C;_LL26B: return 4;
_LL26C: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 4)goto _LL26E;_LL26D: return 5;
_LL26E: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 5)goto _LL270;_LL26F: return 6;
_LL270: if((int)_tmp361 != 1)goto _LL272;_LL271: return 7;_LL272: if(_tmp361 <= (void*)
3?1:*((int*)_tmp361)!= 6)goto _LL274;_LL273: return 8;_LL274: if(_tmp361 <= (void*)3?
1:*((int*)_tmp361)!= 7)goto _LL276;_LL275: return 9;_LL276: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 8)goto _LL278;_LL277: return 10;_LL278: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 9)goto _LL27A;_LL279: return 11;_LL27A: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 10)goto _LL27C;_LL27B: return 12;_LL27C: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 11)goto _LL27E;_LL27D: return 14;_LL27E: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 12)goto _LL280;_LL27F: return 16;_LL280: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 13)goto _LL282;_LL281: return 17;_LL282: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 15)goto _LL284;_LL283: return 18;_LL284: if(_tmp361 <= (void*)3?1:*((
int*)_tmp361)!= 16)goto _LL286;_LL285: return 19;_LL286: if((int)_tmp361 != 2)goto
_LL288;_LL287: return 20;_LL288: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 19)goto
_LL28A;_LL289: return 21;_LL28A: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 20)goto
_LL28C;_LL28B: return 22;_LL28C: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 21)goto
_LL28E;_LL28D: return 23;_LL28E: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 14)goto
_LL290;_LL28F: return 24;_LL290: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 18)goto
_LL292;_LL291: return 25;_LL292: if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 17)goto
_LL261;_LL293: return 26;_LL261:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp362=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp362 != 0)
return _tmp362;{struct _tuple6 _tmp364=({struct _tuple6 _tmp363;_tmp363.f1=t2;_tmp363.f2=
t1;_tmp363;});void*_tmp365;void*_tmp366;void*_tmp367;struct Cyc_Absyn_Tvar*
_tmp368;void*_tmp369;struct Cyc_Absyn_Tvar*_tmp36A;void*_tmp36B;struct Cyc_Absyn_AggrInfo
_tmp36C;void*_tmp36D;struct Cyc_List_List*_tmp36E;void*_tmp36F;struct Cyc_Absyn_AggrInfo
_tmp370;void*_tmp371;struct Cyc_List_List*_tmp372;void*_tmp373;struct _tuple1*
_tmp374;void*_tmp375;struct _tuple1*_tmp376;void*_tmp377;struct Cyc_List_List*
_tmp378;void*_tmp379;struct Cyc_List_List*_tmp37A;void*_tmp37B;struct Cyc_Absyn_TunionInfo
_tmp37C;void*_tmp37D;struct Cyc_Absyn_Tuniondecl**_tmp37E;struct Cyc_Absyn_Tuniondecl*
_tmp37F;struct Cyc_List_List*_tmp380;void*_tmp381;void*_tmp382;struct Cyc_Absyn_TunionInfo
_tmp383;void*_tmp384;struct Cyc_Absyn_Tuniondecl**_tmp385;struct Cyc_Absyn_Tuniondecl*
_tmp386;struct Cyc_List_List*_tmp387;void*_tmp388;void*_tmp389;struct Cyc_Absyn_TunionFieldInfo
_tmp38A;void*_tmp38B;struct Cyc_Absyn_Tuniondecl*_tmp38C;struct Cyc_Absyn_Tunionfield*
_tmp38D;struct Cyc_List_List*_tmp38E;void*_tmp38F;struct Cyc_Absyn_TunionFieldInfo
_tmp390;void*_tmp391;struct Cyc_Absyn_Tuniondecl*_tmp392;struct Cyc_Absyn_Tunionfield*
_tmp393;struct Cyc_List_List*_tmp394;void*_tmp395;struct Cyc_Absyn_PtrInfo _tmp396;
void*_tmp397;struct Cyc_Absyn_Tqual _tmp398;struct Cyc_Absyn_PtrAtts _tmp399;void*
_tmp39A;struct Cyc_Absyn_Conref*_tmp39B;struct Cyc_Absyn_Conref*_tmp39C;struct Cyc_Absyn_Conref*
_tmp39D;void*_tmp39E;struct Cyc_Absyn_PtrInfo _tmp39F;void*_tmp3A0;struct Cyc_Absyn_Tqual
_tmp3A1;struct Cyc_Absyn_PtrAtts _tmp3A2;void*_tmp3A3;struct Cyc_Absyn_Conref*
_tmp3A4;struct Cyc_Absyn_Conref*_tmp3A5;struct Cyc_Absyn_Conref*_tmp3A6;void*
_tmp3A7;void*_tmp3A8;void*_tmp3A9;void*_tmp3AA;void*_tmp3AB;void*_tmp3AC;void*
_tmp3AD;void*_tmp3AE;void*_tmp3AF;int _tmp3B0;void*_tmp3B1;int _tmp3B2;void*
_tmp3B3;struct Cyc_Absyn_ArrayInfo _tmp3B4;void*_tmp3B5;struct Cyc_Absyn_Tqual
_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Conref*_tmp3B8;void*_tmp3B9;
struct Cyc_Absyn_ArrayInfo _tmp3BA;void*_tmp3BB;struct Cyc_Absyn_Tqual _tmp3BC;
struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Conref*_tmp3BE;void*_tmp3BF;struct
Cyc_Absyn_FnInfo _tmp3C0;struct Cyc_List_List*_tmp3C1;struct Cyc_Core_Opt*_tmp3C2;
void*_tmp3C3;struct Cyc_List_List*_tmp3C4;int _tmp3C5;struct Cyc_Absyn_VarargInfo*
_tmp3C6;struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*_tmp3C8;void*_tmp3C9;
struct Cyc_Absyn_FnInfo _tmp3CA;struct Cyc_List_List*_tmp3CB;struct Cyc_Core_Opt*
_tmp3CC;void*_tmp3CD;struct Cyc_List_List*_tmp3CE;int _tmp3CF;struct Cyc_Absyn_VarargInfo*
_tmp3D0;struct Cyc_List_List*_tmp3D1;struct Cyc_List_List*_tmp3D2;void*_tmp3D3;
struct Cyc_List_List*_tmp3D4;void*_tmp3D5;struct Cyc_List_List*_tmp3D6;void*
_tmp3D7;void*_tmp3D8;struct Cyc_List_List*_tmp3D9;void*_tmp3DA;void*_tmp3DB;
struct Cyc_List_List*_tmp3DC;void*_tmp3DD;void*_tmp3DE;void*_tmp3DF;void*_tmp3E0;
void*_tmp3E1;void*_tmp3E2;void*_tmp3E3;void*_tmp3E4;void*_tmp3E5;void*_tmp3E6;
void*_tmp3E7;void*_tmp3E8;void*_tmp3E9;int _tmp3EA;void*_tmp3EB;int _tmp3EC;void*
_tmp3ED;void*_tmp3EE;void*_tmp3EF;void*_tmp3F0;void*_tmp3F1;void*_tmp3F2;_LL295:
_tmp365=_tmp364.f1;if(_tmp365 <= (void*)3?1:*((int*)_tmp365)!= 0)goto _LL297;
_tmp366=_tmp364.f2;if(_tmp366 <= (void*)3?1:*((int*)_tmp366)!= 0)goto _LL297;
_LL296:({void*_tmp3F3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp3F4="typecmp: can only compare closed types";
_tag_arr(_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,39));}),_tag_arr(
_tmp3F3,sizeof(void*),0));});_LL297: _tmp367=_tmp364.f1;if(_tmp367 <= (void*)3?1:*((
int*)_tmp367)!= 1)goto _LL299;_tmp368=((struct Cyc_Absyn_VarType_struct*)_tmp367)->f1;
_tmp369=_tmp364.f2;if(_tmp369 <= (void*)3?1:*((int*)_tmp369)!= 1)goto _LL299;
_tmp36A=((struct Cyc_Absyn_VarType_struct*)_tmp369)->f1;_LL298: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp36A->identity)),*((int*)_check_null(_tmp368->identity)));
_LL299: _tmp36B=_tmp364.f1;if(_tmp36B <= (void*)3?1:*((int*)_tmp36B)!= 10)goto
_LL29B;_tmp36C=((struct Cyc_Absyn_AggrType_struct*)_tmp36B)->f1;_tmp36D=(void*)
_tmp36C.aggr_info;_tmp36E=_tmp36C.targs;_tmp36F=_tmp364.f2;if(_tmp36F <= (void*)3?
1:*((int*)_tmp36F)!= 10)goto _LL29B;_tmp370=((struct Cyc_Absyn_AggrType_struct*)
_tmp36F)->f1;_tmp371=(void*)_tmp370.aggr_info;_tmp372=_tmp370.targs;_LL29A: {
struct _tuple1*_tmp3F6;struct _tuple5 _tmp3F5=Cyc_Absyn_aggr_kinded_name(_tmp36D);
_tmp3F6=_tmp3F5.f2;{struct _tuple1*_tmp3F8;struct _tuple5 _tmp3F7=Cyc_Absyn_aggr_kinded_name(
_tmp371);_tmp3F8=_tmp3F7.f2;{int _tmp3F9=Cyc_Absyn_qvar_cmp(_tmp3F6,_tmp3F8);if(
_tmp3F9 != 0)return _tmp3F9;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp36E,_tmp372);}}}}_LL29B: _tmp373=_tmp364.f1;if(_tmp373 <= (void*)3?1:*((int*)
_tmp373)!= 12)goto _LL29D;_tmp374=((struct Cyc_Absyn_EnumType_struct*)_tmp373)->f1;
_tmp375=_tmp364.f2;if(_tmp375 <= (void*)3?1:*((int*)_tmp375)!= 12)goto _LL29D;
_tmp376=((struct Cyc_Absyn_EnumType_struct*)_tmp375)->f1;_LL29C: return Cyc_Absyn_qvar_cmp(
_tmp374,_tmp376);_LL29D: _tmp377=_tmp364.f1;if(_tmp377 <= (void*)3?1:*((int*)
_tmp377)!= 13)goto _LL29F;_tmp378=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp377)->f1;
_tmp379=_tmp364.f2;if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 13)goto _LL29F;
_tmp37A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp379)->f1;_LL29E: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp378,
_tmp37A);_LL29F: _tmp37B=_tmp364.f1;if(_tmp37B <= (void*)3?1:*((int*)_tmp37B)!= 2)
goto _LL2A1;_tmp37C=((struct Cyc_Absyn_TunionType_struct*)_tmp37B)->f1;_tmp37D=(
void*)_tmp37C.tunion_info;if(*((int*)_tmp37D)!= 1)goto _LL2A1;_tmp37E=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp37D)->f1;_tmp37F=*_tmp37E;_tmp380=_tmp37C.targs;_tmp381=(void*)_tmp37C.rgn;
_tmp382=_tmp364.f2;if(_tmp382 <= (void*)3?1:*((int*)_tmp382)!= 2)goto _LL2A1;
_tmp383=((struct Cyc_Absyn_TunionType_struct*)_tmp382)->f1;_tmp384=(void*)_tmp383.tunion_info;
if(*((int*)_tmp384)!= 1)goto _LL2A1;_tmp385=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp384)->f1;_tmp386=*_tmp385;_tmp387=_tmp383.targs;_tmp388=(void*)_tmp383.rgn;
_LL2A0: if(_tmp386 == _tmp37F)return 0;{int _tmp3FA=Cyc_Absyn_qvar_cmp(_tmp386->name,
_tmp37F->name);if(_tmp3FA != 0)return _tmp3FA;{int _tmp3FB=Cyc_Tcutil_typecmp(
_tmp388,_tmp381);if(_tmp3FB != 0)return _tmp3FB;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp387,_tmp380);}}_LL2A1: _tmp389=_tmp364.f1;if(_tmp389 <= (void*)3?1:*((int*)
_tmp389)!= 3)goto _LL2A3;_tmp38A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp389)->f1;_tmp38B=(void*)_tmp38A.field_info;if(*((int*)_tmp38B)!= 1)goto
_LL2A3;_tmp38C=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp38B)->f1;_tmp38D=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp38B)->f2;_tmp38E=_tmp38A.targs;
_tmp38F=_tmp364.f2;if(_tmp38F <= (void*)3?1:*((int*)_tmp38F)!= 3)goto _LL2A3;
_tmp390=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp38F)->f1;_tmp391=(void*)
_tmp390.field_info;if(*((int*)_tmp391)!= 1)goto _LL2A3;_tmp392=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp391)->f1;_tmp393=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp391)->f2;
_tmp394=_tmp390.targs;_LL2A2: if(_tmp392 == _tmp38C)return 0;{int _tmp3FC=Cyc_Absyn_qvar_cmp(
_tmp38C->name,_tmp392->name);if(_tmp3FC != 0)return _tmp3FC;{int _tmp3FD=Cyc_Absyn_qvar_cmp(
_tmp38D->name,_tmp393->name);if(_tmp3FD != 0)return _tmp3FD;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp394,_tmp38E);}}_LL2A3: _tmp395=_tmp364.f1;if(_tmp395 <= (
void*)3?1:*((int*)_tmp395)!= 4)goto _LL2A5;_tmp396=((struct Cyc_Absyn_PointerType_struct*)
_tmp395)->f1;_tmp397=(void*)_tmp396.elt_typ;_tmp398=_tmp396.elt_tq;_tmp399=
_tmp396.ptr_atts;_tmp39A=(void*)_tmp399.rgn;_tmp39B=_tmp399.nullable;_tmp39C=
_tmp399.bounds;_tmp39D=_tmp399.zero_term;_tmp39E=_tmp364.f2;if(_tmp39E <= (void*)
3?1:*((int*)_tmp39E)!= 4)goto _LL2A5;_tmp39F=((struct Cyc_Absyn_PointerType_struct*)
_tmp39E)->f1;_tmp3A0=(void*)_tmp39F.elt_typ;_tmp3A1=_tmp39F.elt_tq;_tmp3A2=
_tmp39F.ptr_atts;_tmp3A3=(void*)_tmp3A2.rgn;_tmp3A4=_tmp3A2.nullable;_tmp3A5=
_tmp3A2.bounds;_tmp3A6=_tmp3A2.zero_term;_LL2A4: {int _tmp3FE=Cyc_Tcutil_typecmp(
_tmp3A0,_tmp397);if(_tmp3FE != 0)return _tmp3FE;{int _tmp3FF=Cyc_Tcutil_typecmp(
_tmp3A3,_tmp39A);if(_tmp3FF != 0)return _tmp3FF;{int _tmp400=Cyc_Tcutil_tqual_cmp(
_tmp3A1,_tmp398);if(_tmp400 != 0)return _tmp400;{int _tmp401=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3A5,_tmp39C);if(_tmp401 != 0)return _tmp401;{int _tmp402=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3A6,_tmp39D);if(_tmp402 != 0)return _tmp402;{void*_tmp403=(
void*)(Cyc_Absyn_compress_conref(_tmp3A5))->v;void*_tmp404;_LL2CA: if(_tmp403 <= (
void*)1?1:*((int*)_tmp403)!= 0)goto _LL2CC;_tmp404=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp403)->f1;if((int)_tmp404 != 0)goto _LL2CC;_LL2CB: return 0;_LL2CC:;_LL2CD: goto
_LL2C9;_LL2C9:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3A4,
_tmp39B);}}}}}_LL2A5: _tmp3A7=_tmp364.f1;if(_tmp3A7 <= (void*)3?1:*((int*)_tmp3A7)
!= 5)goto _LL2A7;_tmp3A8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3A7)->f1;
_tmp3A9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3A7)->f2;_tmp3AA=_tmp364.f2;
if(_tmp3AA <= (void*)3?1:*((int*)_tmp3AA)!= 5)goto _LL2A7;_tmp3AB=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3AA)->f1;_tmp3AC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3AA)->f2;_LL2A6: if(_tmp3AB != _tmp3A8)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3AB),(int)((unsigned int)_tmp3A8));if(_tmp3AC != _tmp3A9)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3AC),(int)((unsigned int)_tmp3A9));
return 0;_LL2A7: _tmp3AD=_tmp364.f1;if((int)_tmp3AD != 1)goto _LL2A9;_tmp3AE=_tmp364.f2;
if((int)_tmp3AE != 1)goto _LL2A9;_LL2A8: return 0;_LL2A9: _tmp3AF=_tmp364.f1;if(
_tmp3AF <= (void*)3?1:*((int*)_tmp3AF)!= 6)goto _LL2AB;_tmp3B0=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3AF)->f1;_tmp3B1=_tmp364.f2;if(_tmp3B1 <= (void*)3?1:*((int*)_tmp3B1)!= 6)
goto _LL2AB;_tmp3B2=((struct Cyc_Absyn_DoubleType_struct*)_tmp3B1)->f1;_LL2AA: if(
_tmp3B0 == _tmp3B2)return 0;else{if(_tmp3B0)return - 1;else{return 1;}}_LL2AB: _tmp3B3=
_tmp364.f1;if(_tmp3B3 <= (void*)3?1:*((int*)_tmp3B3)!= 7)goto _LL2AD;_tmp3B4=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3B3)->f1;_tmp3B5=(void*)_tmp3B4.elt_type;
_tmp3B6=_tmp3B4.tq;_tmp3B7=_tmp3B4.num_elts;_tmp3B8=_tmp3B4.zero_term;_tmp3B9=
_tmp364.f2;if(_tmp3B9 <= (void*)3?1:*((int*)_tmp3B9)!= 7)goto _LL2AD;_tmp3BA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3B9)->f1;_tmp3BB=(void*)_tmp3BA.elt_type;
_tmp3BC=_tmp3BA.tq;_tmp3BD=_tmp3BA.num_elts;_tmp3BE=_tmp3BA.zero_term;_LL2AC: {
int _tmp405=Cyc_Tcutil_tqual_cmp(_tmp3BC,_tmp3B6);if(_tmp405 != 0)return _tmp405;{
int _tmp406=Cyc_Tcutil_typecmp(_tmp3BB,_tmp3B5);if(_tmp406 != 0)return _tmp406;{int
_tmp407=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3B8,_tmp3BE);if(_tmp407 != 0)
return _tmp407;if(_tmp3B7 == _tmp3BD)return 0;if(_tmp3B7 == 0?1: _tmp3BD == 0)({void*
_tmp408[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp409="missing expression in array index";_tag_arr(_tmp409,sizeof(
char),_get_zero_arr_size(_tmp409,34));}),_tag_arr(_tmp408,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B7,
_tmp3BD);}}}_LL2AD: _tmp3BF=_tmp364.f1;if(_tmp3BF <= (void*)3?1:*((int*)_tmp3BF)!= 
8)goto _LL2AF;_tmp3C0=((struct Cyc_Absyn_FnType_struct*)_tmp3BF)->f1;_tmp3C1=
_tmp3C0.tvars;_tmp3C2=_tmp3C0.effect;_tmp3C3=(void*)_tmp3C0.ret_typ;_tmp3C4=
_tmp3C0.args;_tmp3C5=_tmp3C0.c_varargs;_tmp3C6=_tmp3C0.cyc_varargs;_tmp3C7=
_tmp3C0.rgn_po;_tmp3C8=_tmp3C0.attributes;_tmp3C9=_tmp364.f2;if(_tmp3C9 <= (void*)
3?1:*((int*)_tmp3C9)!= 8)goto _LL2AF;_tmp3CA=((struct Cyc_Absyn_FnType_struct*)
_tmp3C9)->f1;_tmp3CB=_tmp3CA.tvars;_tmp3CC=_tmp3CA.effect;_tmp3CD=(void*)_tmp3CA.ret_typ;
_tmp3CE=_tmp3CA.args;_tmp3CF=_tmp3CA.c_varargs;_tmp3D0=_tmp3CA.cyc_varargs;
_tmp3D1=_tmp3CA.rgn_po;_tmp3D2=_tmp3CA.attributes;_LL2AE:({void*_tmp40A[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp40B="typecmp: function types not handled";_tag_arr(_tmp40B,sizeof(char),
_get_zero_arr_size(_tmp40B,36));}),_tag_arr(_tmp40A,sizeof(void*),0));});_LL2AF:
_tmp3D3=_tmp364.f1;if(_tmp3D3 <= (void*)3?1:*((int*)_tmp3D3)!= 9)goto _LL2B1;
_tmp3D4=((struct Cyc_Absyn_TupleType_struct*)_tmp3D3)->f1;_tmp3D5=_tmp364.f2;if(
_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 9)goto _LL2B1;_tmp3D6=((struct Cyc_Absyn_TupleType_struct*)
_tmp3D5)->f1;_LL2B0: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp3D6,_tmp3D4);_LL2B1: _tmp3D7=_tmp364.f1;if(_tmp3D7 <= (void*)3?1:*((int*)
_tmp3D7)!= 11)goto _LL2B3;_tmp3D8=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3D7)->f1;_tmp3D9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3D7)->f2;_tmp3DA=
_tmp364.f2;if(_tmp3DA <= (void*)3?1:*((int*)_tmp3DA)!= 11)goto _LL2B3;_tmp3DB=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3DA)->f1;_tmp3DC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3DA)->f2;_LL2B2: if(_tmp3DB != _tmp3D8){if(_tmp3DB == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp3DC,_tmp3D9);_LL2B3: _tmp3DD=_tmp364.f1;if(_tmp3DD <= (void*)3?1:*((int*)
_tmp3DD)!= 15)goto _LL2B5;_tmp3DE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3DD)->f1;_tmp3DF=_tmp364.f2;if(_tmp3DF <= (void*)3?1:*((int*)_tmp3DF)!= 15)
goto _LL2B5;_tmp3E0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3DF)->f1;
_LL2B4: return Cyc_Tcutil_typecmp(_tmp3DE,_tmp3E0);_LL2B5: _tmp3E1=_tmp364.f1;if(
_tmp3E1 <= (void*)3?1:*((int*)_tmp3E1)!= 14)goto _LL2B7;_tmp3E2=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp3E1)->f1;_tmp3E3=_tmp364.f2;if(_tmp3E3 <= (void*)3?1:*((int*)_tmp3E3)!= 14)
goto _LL2B7;_tmp3E4=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp3E3)->f1;
_LL2B6: return Cyc_Tcutil_typecmp(_tmp3E2,_tmp3E4);_LL2B7: _tmp3E5=_tmp364.f1;if(
_tmp3E5 <= (void*)3?1:*((int*)_tmp3E5)!= 17)goto _LL2B9;_tmp3E6=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp3E5)->f1;_tmp3E7=_tmp364.f2;if(_tmp3E7 <= (void*)3?1:*((int*)_tmp3E7)!= 17)
goto _LL2B9;_tmp3E8=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp3E7)->f1;_LL2B8:
return Cyc_Tcutil_typecmp(_tmp3E6,_tmp3E8);_LL2B9: _tmp3E9=_tmp364.f1;if(_tmp3E9 <= (
void*)3?1:*((int*)_tmp3E9)!= 18)goto _LL2BB;_tmp3EA=((struct Cyc_Absyn_TypeInt_struct*)
_tmp3E9)->f1;_tmp3EB=_tmp364.f2;if(_tmp3EB <= (void*)3?1:*((int*)_tmp3EB)!= 18)
goto _LL2BB;_tmp3EC=((struct Cyc_Absyn_TypeInt_struct*)_tmp3EB)->f1;_LL2BA: return
Cyc_Core_intcmp(_tmp3EA,_tmp3EC);_LL2BB: _tmp3ED=_tmp364.f1;if(_tmp3ED <= (void*)3?
1:*((int*)_tmp3ED)!= 20)goto _LL2BD;_LL2BC: goto _LL2BE;_LL2BD: _tmp3EE=_tmp364.f2;
if(_tmp3EE <= (void*)3?1:*((int*)_tmp3EE)!= 20)goto _LL2BF;_LL2BE: goto _LL2C0;
_LL2BF: _tmp3EF=_tmp364.f1;if(_tmp3EF <= (void*)3?1:*((int*)_tmp3EF)!= 19)goto
_LL2C1;_LL2C0: goto _LL2C2;_LL2C1: _tmp3F0=_tmp364.f1;if(_tmp3F0 <= (void*)3?1:*((
int*)_tmp3F0)!= 21)goto _LL2C3;_LL2C2: goto _LL2C4;_LL2C3: _tmp3F1=_tmp364.f2;if(
_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 21)goto _LL2C5;_LL2C4: goto _LL2C6;_LL2C5:
_tmp3F2=_tmp364.f2;if(_tmp3F2 <= (void*)3?1:*((int*)_tmp3F2)!= 19)goto _LL2C7;
_LL2C6:({void*_tmp40C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp40D="typecmp: effects not handled";_tag_arr(
_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,29));}),_tag_arr(_tmp40C,sizeof(
void*),0));});_LL2C7:;_LL2C8:({void*_tmp40E[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp40F="Unmatched case in typecmp";
_tag_arr(_tmp40F,sizeof(char),_get_zero_arr_size(_tmp40F,26));}),_tag_arr(
_tmp40E,sizeof(void*),0));});_LL294:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp410=Cyc_Tcutil_compress(t);_LL2CF: if(_tmp410 <= (void*)3?1:*((int*)
_tmp410)!= 5)goto _LL2D1;_LL2D0: goto _LL2D2;_LL2D1: if((int)_tmp410 != 1)goto _LL2D3;
_LL2D2: goto _LL2D4;_LL2D3: if(_tmp410 <= (void*)3?1:*((int*)_tmp410)!= 6)goto _LL2D5;
_LL2D4: goto _LL2D6;_LL2D5: if(_tmp410 <= (void*)3?1:*((int*)_tmp410)!= 12)goto
_LL2D7;_LL2D6: goto _LL2D8;_LL2D7: if(_tmp410 <= (void*)3?1:*((int*)_tmp410)!= 13)
goto _LL2D9;_LL2D8: return 1;_LL2D9:;_LL2DA: return 0;_LL2CE:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp412=({struct _tuple6 _tmp411;_tmp411.f1=Cyc_Tcutil_compress(
t1);_tmp411.f2=Cyc_Tcutil_compress(t2);_tmp411;});void*_tmp413;int _tmp414;void*
_tmp415;int _tmp416;void*_tmp417;void*_tmp418;void*_tmp419;void*_tmp41A;void*
_tmp41B;void*_tmp41C;void*_tmp41D;void*_tmp41E;void*_tmp41F;void*_tmp420;void*
_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;void*_tmp425;void*_tmp426;void*
_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;void*
_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*_tmp431;void*_tmp432;void*
_tmp433;void*_tmp434;void*_tmp435;void*_tmp436;void*_tmp437;void*_tmp438;void*
_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;void*_tmp43E;void*
_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;void*_tmp443;void*_tmp444;void*
_tmp445;_LL2DC: _tmp413=_tmp412.f1;if(_tmp413 <= (void*)3?1:*((int*)_tmp413)!= 6)
goto _LL2DE;_tmp414=((struct Cyc_Absyn_DoubleType_struct*)_tmp413)->f1;_tmp415=
_tmp412.f2;if(_tmp415 <= (void*)3?1:*((int*)_tmp415)!= 6)goto _LL2DE;_tmp416=((
struct Cyc_Absyn_DoubleType_struct*)_tmp415)->f1;_LL2DD: return !_tmp416?_tmp414: 0;
_LL2DE: _tmp417=_tmp412.f1;if(_tmp417 <= (void*)3?1:*((int*)_tmp417)!= 6)goto
_LL2E0;_tmp418=_tmp412.f2;if((int)_tmp418 != 1)goto _LL2E0;_LL2DF: goto _LL2E1;
_LL2E0: _tmp419=_tmp412.f1;if(_tmp419 <= (void*)3?1:*((int*)_tmp419)!= 6)goto
_LL2E2;_tmp41A=_tmp412.f2;if(_tmp41A <= (void*)3?1:*((int*)_tmp41A)!= 5)goto
_LL2E2;_LL2E1: goto _LL2E3;_LL2E2: _tmp41B=_tmp412.f1;if(_tmp41B <= (void*)3?1:*((
int*)_tmp41B)!= 6)goto _LL2E4;_tmp41C=_tmp412.f2;if(_tmp41C <= (void*)3?1:*((int*)
_tmp41C)!= 14)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4: _tmp41D=_tmp412.f1;if((int)
_tmp41D != 1)goto _LL2E6;_tmp41E=_tmp412.f2;if(_tmp41E <= (void*)3?1:*((int*)
_tmp41E)!= 14)goto _LL2E6;_LL2E5: goto _LL2E7;_LL2E6: _tmp41F=_tmp412.f1;if(_tmp41F
<= (void*)3?1:*((int*)_tmp41F)!= 6)goto _LL2E8;_tmp420=_tmp412.f2;if(_tmp420 <= (
void*)3?1:*((int*)_tmp420)!= 17)goto _LL2E8;_LL2E7: goto _LL2E9;_LL2E8: _tmp421=
_tmp412.f1;if((int)_tmp421 != 1)goto _LL2EA;_tmp422=_tmp412.f2;if(_tmp422 <= (void*)
3?1:*((int*)_tmp422)!= 17)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp423=_tmp412.f1;
if((int)_tmp423 != 1)goto _LL2EC;_tmp424=_tmp412.f2;if(_tmp424 <= (void*)3?1:*((int*)
_tmp424)!= 5)goto _LL2EC;_LL2EB: return 1;_LL2EC: _tmp425=_tmp412.f1;if(_tmp425 <= (
void*)3?1:*((int*)_tmp425)!= 5)goto _LL2EE;_tmp426=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp425)->f2;if((int)_tmp426 != 3)goto _LL2EE;_tmp427=_tmp412.f2;if(_tmp427 <= (
void*)3?1:*((int*)_tmp427)!= 5)goto _LL2EE;_tmp428=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp427)->f2;if((int)_tmp428 != 3)goto _LL2EE;_LL2ED: return 0;_LL2EE: _tmp429=
_tmp412.f1;if(_tmp429 <= (void*)3?1:*((int*)_tmp429)!= 5)goto _LL2F0;_tmp42A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp429)->f2;if((int)_tmp42A != 3)goto _LL2F0;
_LL2EF: goto _LL2F1;_LL2F0: _tmp42B=_tmp412.f1;if(_tmp42B <= (void*)3?1:*((int*)
_tmp42B)!= 5)goto _LL2F2;_tmp42C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp42B)->f2;
if((int)_tmp42C != 2)goto _LL2F2;_tmp42D=_tmp412.f2;if((int)_tmp42D != 1)goto _LL2F2;
_LL2F1: goto _LL2F3;_LL2F2: _tmp42E=_tmp412.f1;if(_tmp42E <= (void*)3?1:*((int*)
_tmp42E)!= 5)goto _LL2F4;_tmp42F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp42E)->f2;
if((int)_tmp42F != 2)goto _LL2F4;_tmp430=_tmp412.f2;if(_tmp430 <= (void*)3?1:*((int*)
_tmp430)!= 5)goto _LL2F4;_tmp431=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp430)->f2;
if((int)_tmp431 != 1)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: _tmp432=_tmp412.f1;if(
_tmp432 <= (void*)3?1:*((int*)_tmp432)!= 5)goto _LL2F6;_tmp433=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp432)->f2;if((int)_tmp433 != 2)goto _LL2F6;_tmp434=_tmp412.f2;if(_tmp434 <= (
void*)3?1:*((int*)_tmp434)!= 5)goto _LL2F6;_tmp435=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp434)->f2;if((int)_tmp435 != 0)goto _LL2F6;_LL2F5: goto _LL2F7;_LL2F6: _tmp436=
_tmp412.f1;if(_tmp436 <= (void*)3?1:*((int*)_tmp436)!= 5)goto _LL2F8;_tmp437=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp436)->f2;if((int)_tmp437 != 1)goto _LL2F8;
_tmp438=_tmp412.f2;if(_tmp438 <= (void*)3?1:*((int*)_tmp438)!= 5)goto _LL2F8;
_tmp439=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp438)->f2;if((int)_tmp439 != 
0)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: _tmp43A=_tmp412.f1;if(_tmp43A <= (void*)3?1:*((
int*)_tmp43A)!= 17)goto _LL2FA;_tmp43B=_tmp412.f2;if(_tmp43B <= (void*)3?1:*((int*)
_tmp43B)!= 5)goto _LL2FA;_tmp43C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp43B)->f2;
if((int)_tmp43C != 1)goto _LL2FA;_LL2F9: goto _LL2FB;_LL2FA: _tmp43D=_tmp412.f1;if(
_tmp43D <= (void*)3?1:*((int*)_tmp43D)!= 17)goto _LL2FC;_tmp43E=_tmp412.f2;if(
_tmp43E <= (void*)3?1:*((int*)_tmp43E)!= 5)goto _LL2FC;_tmp43F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp43E)->f2;if((int)_tmp43F != 0)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: _tmp440=
_tmp412.f1;if(_tmp440 <= (void*)3?1:*((int*)_tmp440)!= 14)goto _LL2FE;_tmp441=
_tmp412.f2;if(_tmp441 <= (void*)3?1:*((int*)_tmp441)!= 5)goto _LL2FE;_tmp442=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp441)->f2;if((int)_tmp442 != 1)goto _LL2FE;
_LL2FD: goto _LL2FF;_LL2FE: _tmp443=_tmp412.f1;if(_tmp443 <= (void*)3?1:*((int*)
_tmp443)!= 14)goto _LL300;_tmp444=_tmp412.f2;if(_tmp444 <= (void*)3?1:*((int*)
_tmp444)!= 5)goto _LL300;_tmp445=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp444)->f2;
if((int)_tmp445 != 0)goto _LL300;_LL2FF: return 1;_LL300:;_LL301: return 0;_LL2DB:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->v=(
void*)t1;_tmp446;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp44A;_tmp44A.tag=0;_tmp44A.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp449;_tmp449.tag=0;_tmp449.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp447[2]={& _tmp449,& _tmp44A};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,({const char*_tmp448="type mismatch: expecting %s but found %s";
_tag_arr(_tmp448,sizeof(char),_get_zero_arr_size(_tmp448,41));}),_tag_arr(
_tmp447,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp44B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL303: if(_tmp44B <= (void*)3?1:*((int*)_tmp44B)!= 4)goto _LL305;_LL304: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);goto _LL302;_LL305:;_LL306: return 0;_LL302:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp44C=Cyc_Tcutil_compress(t);_LL308:
if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 5)goto _LL30A;_LL309: goto _LL30B;_LL30A:
if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 14)goto _LL30C;_LL30B: goto _LL30D;
_LL30C: if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 17)goto _LL30E;_LL30D: goto
_LL30F;_LL30E: if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 12)goto _LL310;_LL30F:
goto _LL311;_LL310: if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 13)goto _LL312;
_LL311: return 1;_LL312:;_LL313: return 0;_LL307:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp44D[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp44E="integral size mismatch; conversion supplied";
_tag_arr(_tmp44E,sizeof(char),_get_zero_arr_size(_tmp44E,44));}),_tag_arr(
_tmp44D,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*_tmp44F[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp450="integral size mismatch; conversion supplied";
_tag_arr(_tmp450,sizeof(char),_get_zero_arr_size(_tmp450,44));}),_tag_arr(
_tmp44F,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ);
return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);{struct _tuple6 _tmp452=({struct _tuple6 _tmp451;_tmp451.f1=t1;_tmp451.f2=t2;
_tmp451;});void*_tmp453;struct Cyc_Absyn_PtrInfo _tmp454;void*_tmp455;struct Cyc_Absyn_PtrInfo
_tmp456;void*_tmp457;struct Cyc_Absyn_ArrayInfo _tmp458;void*_tmp459;struct Cyc_Absyn_Tqual
_tmp45A;struct Cyc_Absyn_Exp*_tmp45B;struct Cyc_Absyn_Conref*_tmp45C;void*_tmp45D;
struct Cyc_Absyn_ArrayInfo _tmp45E;void*_tmp45F;struct Cyc_Absyn_Tqual _tmp460;
struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Conref*_tmp462;void*_tmp463;struct
Cyc_Absyn_TunionFieldInfo _tmp464;void*_tmp465;struct Cyc_Absyn_Tuniondecl*_tmp466;
struct Cyc_Absyn_Tunionfield*_tmp467;struct Cyc_List_List*_tmp468;void*_tmp469;
struct Cyc_Absyn_TunionInfo _tmp46A;void*_tmp46B;struct Cyc_Absyn_Tuniondecl**
_tmp46C;struct Cyc_Absyn_Tuniondecl*_tmp46D;struct Cyc_List_List*_tmp46E;void*
_tmp46F;struct Cyc_Absyn_PtrInfo _tmp470;void*_tmp471;struct Cyc_Absyn_Tqual _tmp472;
struct Cyc_Absyn_PtrAtts _tmp473;void*_tmp474;struct Cyc_Absyn_Conref*_tmp475;
struct Cyc_Absyn_Conref*_tmp476;struct Cyc_Absyn_Conref*_tmp477;void*_tmp478;
struct Cyc_Absyn_TunionInfo _tmp479;void*_tmp47A;struct Cyc_Absyn_Tuniondecl**
_tmp47B;struct Cyc_Absyn_Tuniondecl*_tmp47C;struct Cyc_List_List*_tmp47D;void*
_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*_tmp483;void*
_tmp484;_LL315: _tmp453=_tmp452.f1;if(_tmp453 <= (void*)3?1:*((int*)_tmp453)!= 4)
goto _LL317;_tmp454=((struct Cyc_Absyn_PointerType_struct*)_tmp453)->f1;_tmp455=
_tmp452.f2;if(_tmp455 <= (void*)3?1:*((int*)_tmp455)!= 4)goto _LL317;_tmp456=((
struct Cyc_Absyn_PointerType_struct*)_tmp455)->f1;_LL316: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp454.ptr_atts).nullable,(_tmp456.ptr_atts).nullable)){void*
_tmp485=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((
_tmp454.ptr_atts).nullable))->v;int _tmp486;_LL324: if(_tmp485 <= (void*)1?1:*((int*)
_tmp485)!= 0)goto _LL326;_tmp486=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp485)->f1;
_LL325: okay=!_tmp486;goto _LL323;_LL326:;_LL327:({void*_tmp487[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp488="silent_castable conref not eq";
_tag_arr(_tmp488,sizeof(char),_get_zero_arr_size(_tmp488,30));}),_tag_arr(
_tmp487,sizeof(void*),0));});_LL323:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,(
_tmp454.ptr_atts).bounds,(_tmp456.ptr_atts).bounds)){struct _tuple6 _tmp48A=({
struct _tuple6 _tmp489;_tmp489.f1=(void*)(Cyc_Absyn_compress_conref((_tmp454.ptr_atts).bounds))->v;
_tmp489.f2=(void*)(Cyc_Absyn_compress_conref((_tmp456.ptr_atts).bounds))->v;
_tmp489;});void*_tmp48B;void*_tmp48C;void*_tmp48D;void*_tmp48E;void*_tmp48F;
_LL329: _tmp48B=_tmp48A.f1;if(_tmp48B <= (void*)1?1:*((int*)_tmp48B)!= 0)goto
_LL32B;_tmp48C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp48B)->f1;_tmp48D=
_tmp48A.f2;if(_tmp48D <= (void*)1?1:*((int*)_tmp48D)!= 0)goto _LL32B;_tmp48E=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp48D)->f1;_LL32A:{struct _tuple6 _tmp491=({
struct _tuple6 _tmp490;_tmp490.f1=_tmp48C;_tmp490.f2=_tmp48E;_tmp490;});void*
_tmp492;void*_tmp493;void*_tmp494;void*_tmp495;void*_tmp496;struct Cyc_Absyn_Exp*
_tmp497;void*_tmp498;struct Cyc_Absyn_Exp*_tmp499;void*_tmp49A;void*_tmp49B;
struct Cyc_Absyn_Exp*_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;
void*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;void*_tmp4A3;void*_tmp4A4;void*_tmp4A5;
void*_tmp4A6;_LL330: _tmp492=_tmp491.f1;if(_tmp492 <= (void*)1?1:*((int*)_tmp492)
!= 0)goto _LL332;_tmp493=_tmp491.f2;if((int)_tmp493 != 0)goto _LL332;_LL331: goto
_LL333;_LL332: _tmp494=_tmp491.f1;if((int)_tmp494 != 0)goto _LL334;_tmp495=_tmp491.f2;
if((int)_tmp495 != 0)goto _LL334;_LL333: okay=1;goto _LL32F;_LL334: _tmp496=_tmp491.f1;
if(_tmp496 <= (void*)1?1:*((int*)_tmp496)!= 0)goto _LL336;_tmp497=((struct Cyc_Absyn_Upper_b_struct*)
_tmp496)->f1;_tmp498=_tmp491.f2;if(_tmp498 <= (void*)1?1:*((int*)_tmp498)!= 0)
goto _LL336;_tmp499=((struct Cyc_Absyn_Upper_b_struct*)_tmp498)->f1;_LL335: okay=
okay?Cyc_Evexp_lte_const_exp(_tmp499,_tmp497): 0;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp456.ptr_atts).zero_term))({void*_tmp4A7[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4A8="implicit cast to shorter array";
_tag_arr(_tmp4A8,sizeof(char),_get_zero_arr_size(_tmp4A8,31));}),_tag_arr(
_tmp4A7,sizeof(void*),0));});goto _LL32F;_LL336: _tmp49A=_tmp491.f1;if((int)
_tmp49A != 0)goto _LL338;_tmp49B=_tmp491.f2;if(_tmp49B <= (void*)1?1:*((int*)
_tmp49B)!= 0)goto _LL338;_tmp49C=((struct Cyc_Absyn_Upper_b_struct*)_tmp49B)->f1;
_LL337: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp454.ptr_atts).zero_term)?Cyc_Tcutil_is_bound_one((_tmp456.ptr_atts).bounds):
0)goto _LL32F;okay=0;goto _LL32F;_LL338: _tmp49D=_tmp491.f1;if(_tmp49D <= (void*)1?1:*((
int*)_tmp49D)!= 1)goto _LL33A;_tmp49E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp49D)->f1;_tmp49F=_tmp491.f2;if(_tmp49F <= (void*)1?1:*((int*)_tmp49F)!= 1)
goto _LL33A;_tmp4A0=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp49F)->f1;
_LL339: if(!Cyc_Tcutil_unify(_tmp49E,_tmp4A0)){struct _tuple6 _tmp4AA=({struct
_tuple6 _tmp4A9;_tmp4A9.f1=Cyc_Tcutil_compress(_tmp49E);_tmp4A9.f2=Cyc_Tcutil_compress(
_tmp4A0);_tmp4A9;});void*_tmp4AB;int _tmp4AC;void*_tmp4AD;int _tmp4AE;_LL341:
_tmp4AB=_tmp4AA.f1;if(_tmp4AB <= (void*)3?1:*((int*)_tmp4AB)!= 18)goto _LL343;
_tmp4AC=((struct Cyc_Absyn_TypeInt_struct*)_tmp4AB)->f1;_tmp4AD=_tmp4AA.f2;if(
_tmp4AD <= (void*)3?1:*((int*)_tmp4AD)!= 18)goto _LL343;_tmp4AE=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4AD)->f1;_LL342: if(_tmp4AC > _tmp4AE)okay=0;goto _LL340;_LL343:;_LL344: okay=0;
goto _LL340;_LL340:;}goto _LL32F;_LL33A: _tmp4A1=_tmp491.f1;if(_tmp4A1 <= (void*)1?1:*((
int*)_tmp4A1)!= 0)goto _LL33C;_tmp4A2=((struct Cyc_Absyn_Upper_b_struct*)_tmp4A1)->f1;
_tmp4A3=_tmp491.f2;if(_tmp4A3 <= (void*)1?1:*((int*)_tmp4A3)!= 1)goto _LL33C;
_tmp4A4=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4A3)->f1;_LL33B: {
unsigned int _tmp4B0;int _tmp4B1;struct _tuple7 _tmp4AF=Cyc_Evexp_eval_const_uint_exp(
_tmp4A2);_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;if(!_tmp4B1){okay=0;goto _LL32F;}{
void*_tmp4B2=Cyc_Tcutil_compress(_tmp4A4);int _tmp4B3;_LL346: if(_tmp4B2 <= (void*)
3?1:*((int*)_tmp4B2)!= 18)goto _LL348;_tmp4B3=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4B2)->f1;_LL347: if(_tmp4B0 > _tmp4B3)okay=0;goto _LL345;_LL348:;_LL349: okay=0;
goto _LL345;_LL345:;}goto _LL32F;}_LL33C: _tmp4A5=_tmp491.f1;if(_tmp4A5 <= (void*)1?
1:*((int*)_tmp4A5)!= 1)goto _LL33E;_LL33D: goto _LL33F;_LL33E: _tmp4A6=_tmp491.f2;
if(_tmp4A6 <= (void*)1?1:*((int*)_tmp4A6)!= 1)goto _LL32F;_LL33F: okay=0;goto _LL32F;
_LL32F:;}goto _LL328;_LL32B: _tmp48F=_tmp48A.f1;if((int)_tmp48F != 0)goto _LL32D;
_LL32C: okay=0;goto _LL328;_LL32D:;_LL32E:({void*_tmp4B4[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4B5="silent_castable conrefs didn't unify";
_tag_arr(_tmp4B5,sizeof(char),_get_zero_arr_size(_tmp4B5,37));}),_tag_arr(
_tmp4B4,sizeof(void*),0));});_LL328:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp454.elt_typ,(
void*)_tmp456.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)(_tmp454.ptr_atts).rgn,(
void*)(_tmp456.ptr_atts).rgn)?1: Cyc_Tcenv_region_outlives(te,(void*)(_tmp454.ptr_atts).rgn,(
void*)(_tmp456.ptr_atts).rgn): 0;okay=okay?!(_tmp454.elt_tq).q_const?1:(_tmp456.elt_tq).q_const:
0;okay=okay?((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp454.ptr_atts).zero_term,(
_tmp456.ptr_atts).zero_term)?1:(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp454.ptr_atts).zero_term)?(_tmp456.elt_tq).q_const: 0): 0;return okay;}_LL317:
_tmp457=_tmp452.f1;if(_tmp457 <= (void*)3?1:*((int*)_tmp457)!= 7)goto _LL319;
_tmp458=((struct Cyc_Absyn_ArrayType_struct*)_tmp457)->f1;_tmp459=(void*)_tmp458.elt_type;
_tmp45A=_tmp458.tq;_tmp45B=_tmp458.num_elts;_tmp45C=_tmp458.zero_term;_tmp45D=
_tmp452.f2;if(_tmp45D <= (void*)3?1:*((int*)_tmp45D)!= 7)goto _LL319;_tmp45E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp45D)->f1;_tmp45F=(void*)_tmp45E.elt_type;
_tmp460=_tmp45E.tq;_tmp461=_tmp45E.num_elts;_tmp462=_tmp45E.zero_term;_LL318: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp45C,_tmp462)?(_tmp45B != 0?
_tmp461 != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp45B),(struct Cyc_Absyn_Exp*)_check_null(_tmp461)): 0: 0;return(okay?Cyc_Tcutil_unify(
_tmp459,_tmp45F): 0)?!_tmp45A.q_const?1: _tmp460.q_const: 0;}_LL319: _tmp463=_tmp452.f1;
if(_tmp463 <= (void*)3?1:*((int*)_tmp463)!= 3)goto _LL31B;_tmp464=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp463)->f1;_tmp465=(void*)_tmp464.field_info;if(*((int*)_tmp465)!= 1)goto
_LL31B;_tmp466=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp465)->f1;_tmp467=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp465)->f2;_tmp468=_tmp464.targs;
_tmp469=_tmp452.f2;if(_tmp469 <= (void*)3?1:*((int*)_tmp469)!= 2)goto _LL31B;
_tmp46A=((struct Cyc_Absyn_TunionType_struct*)_tmp469)->f1;_tmp46B=(void*)_tmp46A.tunion_info;
if(*((int*)_tmp46B)!= 1)goto _LL31B;_tmp46C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp46B)->f1;_tmp46D=*_tmp46C;_tmp46E=_tmp46A.targs;_LL31A: if((_tmp466 == _tmp46D?
1: Cyc_Absyn_qvar_cmp(_tmp466->name,_tmp46D->name)== 0)?_tmp467->typs == 0: 0){for(
0;_tmp468 != 0?_tmp46E != 0: 0;(_tmp468=_tmp468->tl,_tmp46E=_tmp46E->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp468->hd,(void*)_tmp46E->hd))break;}if(_tmp468 == 0?_tmp46E == 0: 0)return
1;}return 0;_LL31B: _tmp46F=_tmp452.f1;if(_tmp46F <= (void*)3?1:*((int*)_tmp46F)!= 
4)goto _LL31D;_tmp470=((struct Cyc_Absyn_PointerType_struct*)_tmp46F)->f1;_tmp471=(
void*)_tmp470.elt_typ;_tmp472=_tmp470.elt_tq;_tmp473=_tmp470.ptr_atts;_tmp474=(
void*)_tmp473.rgn;_tmp475=_tmp473.nullable;_tmp476=_tmp473.bounds;_tmp477=
_tmp473.zero_term;_tmp478=_tmp452.f2;if(_tmp478 <= (void*)3?1:*((int*)_tmp478)!= 
2)goto _LL31D;_tmp479=((struct Cyc_Absyn_TunionType_struct*)_tmp478)->f1;_tmp47A=(
void*)_tmp479.tunion_info;if(*((int*)_tmp47A)!= 1)goto _LL31D;_tmp47B=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp47A)->f1;_tmp47C=*_tmp47B;_tmp47D=_tmp479.targs;_tmp47E=(void*)_tmp479.rgn;
_LL31C:{void*_tmp4B6=Cyc_Tcutil_compress(_tmp471);struct Cyc_Absyn_TunionFieldInfo
_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_Tuniondecl*_tmp4B9;struct Cyc_Absyn_Tunionfield*
_tmp4BA;struct Cyc_List_List*_tmp4BB;_LL34B: if(_tmp4B6 <= (void*)3?1:*((int*)
_tmp4B6)!= 3)goto _LL34D;_tmp4B7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4B6)->f1;_tmp4B8=(void*)_tmp4B7.field_info;if(*((int*)_tmp4B8)!= 1)goto
_LL34D;_tmp4B9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4B8)->f1;_tmp4BA=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4B8)->f2;_tmp4BB=_tmp4B7.targs;
_LL34C: if(!Cyc_Tcutil_unify(_tmp474,_tmp47E)?!Cyc_Tcenv_region_outlives(te,
_tmp474,_tmp47E): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp475,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp476,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp477,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp47C->name,
_tmp4B9->name)== 0?_tmp4BA->typs != 0: 0){int okay=1;for(0;_tmp4BB != 0?_tmp47D != 0:
0;(_tmp4BB=_tmp4BB->tl,_tmp47D=_tmp47D->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4BB->hd,(
void*)_tmp47D->hd)){okay=0;break;}}if((!okay?1: _tmp4BB != 0)?1: _tmp47D != 0)return
0;return 1;}goto _LL34A;_LL34D:;_LL34E: goto _LL34A;_LL34A:;}return 0;_LL31D: _tmp47F=
_tmp452.f1;if(_tmp47F <= (void*)3?1:*((int*)_tmp47F)!= 14)goto _LL31F;_tmp480=
_tmp452.f2;if(_tmp480 <= (void*)3?1:*((int*)_tmp480)!= 5)goto _LL31F;_tmp481=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp480)->f2;if((int)_tmp481 != 2)goto _LL31F;
_LL31E: goto _LL320;_LL31F: _tmp482=_tmp452.f1;if(_tmp482 <= (void*)3?1:*((int*)
_tmp482)!= 17)goto _LL321;_tmp483=_tmp452.f2;if(_tmp483 <= (void*)3?1:*((int*)
_tmp483)!= 5)goto _LL321;_tmp484=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp483)->f2;
if((int)_tmp484 != 2)goto _LL321;_LL320: return 1;_LL321:;_LL322: return Cyc_Tcutil_unify(
t1,t2);_LL314:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4BC=Cyc_Tcutil_compress(
t);_LL350: if(_tmp4BC <= (void*)3?1:*((int*)_tmp4BC)!= 4)goto _LL352;_LL351: return 1;
_LL352:;_LL353: return 0;_LL34F:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4BD=(void*)e->r;void*_tmp4BE;int _tmp4BF;void*_tmp4C0;char _tmp4C1;void*
_tmp4C2;short _tmp4C3;void*_tmp4C4;long long _tmp4C5;void*_tmp4C6;struct Cyc_Absyn_Exp*
_tmp4C7;_LL355: if(*((int*)_tmp4BD)!= 0)goto _LL357;_tmp4BE=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4BD)->f1;if(_tmp4BE <= (void*)1?1:*((int*)_tmp4BE)!= 2)goto _LL357;_tmp4BF=((
struct Cyc_Absyn_Int_c_struct*)_tmp4BE)->f2;if(_tmp4BF != 0)goto _LL357;_LL356: goto
_LL358;_LL357: if(*((int*)_tmp4BD)!= 0)goto _LL359;_tmp4C0=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4BD)->f1;if(_tmp4C0 <= (void*)1?1:*((int*)_tmp4C0)!= 0)goto _LL359;_tmp4C1=((
struct Cyc_Absyn_Char_c_struct*)_tmp4C0)->f2;if(_tmp4C1 != 0)goto _LL359;_LL358:
goto _LL35A;_LL359: if(*((int*)_tmp4BD)!= 0)goto _LL35B;_tmp4C2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4BD)->f1;if(_tmp4C2 <= (void*)1?1:*((int*)_tmp4C2)!= 1)goto _LL35B;_tmp4C3=((
struct Cyc_Absyn_Short_c_struct*)_tmp4C2)->f2;if(_tmp4C3 != 0)goto _LL35B;_LL35A:
goto _LL35C;_LL35B: if(*((int*)_tmp4BD)!= 0)goto _LL35D;_tmp4C4=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4BD)->f1;if(_tmp4C4 <= (void*)1?1:*((int*)_tmp4C4)!= 3)goto _LL35D;_tmp4C5=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp4C4)->f2;if(_tmp4C5 != 0)goto _LL35D;_LL35C:
return 1;_LL35D: if(*((int*)_tmp4BD)!= 13)goto _LL35F;_tmp4C6=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4BD)->f1;_tmp4C7=((struct Cyc_Absyn_Cast_e_struct*)_tmp4BD)->f2;_LL35E: return
Cyc_Tcutil_is_zero(_tmp4C7)?Cyc_Tcutil_admits_zero(_tmp4C6): 0;_LL35F:;_LL360:
return 0;_LL354:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8[0]=({struct Cyc_Absyn_Const_e_struct
_tmp4C9;_tmp4C9.tag=0;_tmp4C9.f1=(void*)((void*)0);_tmp4C9;});_tmp4C8;})));{
struct Cyc_List_List*_tmp4CA=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp4CB=({struct Cyc_Absyn_PointerType_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));
_tmp4CC[0]=({struct Cyc_Absyn_PointerType_struct _tmp4CD;_tmp4CD.tag=4;_tmp4CD.f1=({
struct Cyc_Absyn_PtrInfo _tmp4CE;_tmp4CE.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp4D1=_cycalloc(sizeof(*
_tmp4D1));_tmp4D1->v=_tmp4CA;_tmp4D1;}));_tmp4CE.elt_tq=Cyc_Absyn_empty_tqual();
_tmp4CE.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp4CF;_tmp4CF.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp4D0=_cycalloc(
sizeof(*_tmp4D0));_tmp4D0->v=_tmp4CA;_tmp4D0;}));_tmp4CF.nullable=Cyc_Absyn_true_conref;
_tmp4CF.bounds=Cyc_Absyn_empty_conref();_tmp4CF.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp4CF;});_tmp4CE;});_tmp4CD;});_tmp4CC;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp4CB));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v);if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(
t2)?Cyc_Tcutil_is_arithmetic_type(t1): 0){if(Cyc_Tcutil_will_lose_precision(t1,t2))({
struct Cyc_String_pa_struct _tmp4D5;_tmp4D5.tag=0;_tmp4D5.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp4D4;
_tmp4D4.tag=0;_tmp4D4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t1));{void*_tmp4D2[2]={& _tmp4D4,& _tmp4D5};Cyc_Tcutil_warn(e->loc,({const char*
_tmp4D3="integral size mismatch; %s -> %s conversion supplied";_tag_arr(_tmp4D3,
sizeof(char),_get_zero_arr_size(_tmp4D3,53));}),_tag_arr(_tmp4D2,sizeof(void*),2));}}});
Cyc_Tcutil_unchecked_cast(te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e))return 1;else{if(Cyc_Tcutil_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);({struct Cyc_String_pa_struct _tmp4D9;_tmp4D9.tag=0;_tmp4D9.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp4D8;_tmp4D8.tag=0;_tmp4D8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t1));{void*_tmp4D6[2]={& _tmp4D8,& _tmp4D9};Cyc_Tcutil_warn(e->loc,({const char*
_tmp4D7="implicit cast from %s to %s";_tag_arr(_tmp4D7,sizeof(char),
_get_zero_arr_size(_tmp4D7,28));}),_tag_arr(_tmp4D6,sizeof(void*),2));}}});
return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp4DA=Cyc_Tcutil_compress(t);_LL362: if(_tmp4DA <= (void*)3?1:*((
int*)_tmp4DA)!= 5)goto _LL364;_LL363: goto _LL365;_LL364: if((int)_tmp4DA != 1)goto
_LL366;_LL365: goto _LL367;_LL366: if(_tmp4DA <= (void*)3?1:*((int*)_tmp4DA)!= 6)
goto _LL368;_LL367: return 1;_LL368:;_LL369: return 0;_LL361:;}static struct _tuple4*
Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*x){
return({struct _tuple4*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->f1=x->tq;
_tmp4DB->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);_tmp4DB;});}static struct
Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(
t1);{void*_tmp4DC=t1;struct Cyc_List_List*_tmp4DD;struct Cyc_Absyn_AggrInfo _tmp4DE;
void*_tmp4DF;struct Cyc_Absyn_Aggrdecl**_tmp4E0;struct Cyc_Absyn_Aggrdecl*_tmp4E1;
struct Cyc_List_List*_tmp4E2;void*_tmp4E3;struct Cyc_List_List*_tmp4E4;_LL36B: if((
int)_tmp4DC != 0)goto _LL36D;_LL36C: return 0;_LL36D: if(_tmp4DC <= (void*)3?1:*((int*)
_tmp4DC)!= 9)goto _LL36F;_tmp4DD=((struct Cyc_Absyn_TupleType_struct*)_tmp4DC)->f1;
_LL36E: return _tmp4DD;_LL36F: if(_tmp4DC <= (void*)3?1:*((int*)_tmp4DC)!= 10)goto
_LL371;_tmp4DE=((struct Cyc_Absyn_AggrType_struct*)_tmp4DC)->f1;_tmp4DF=(void*)
_tmp4DE.aggr_info;if(*((int*)_tmp4DF)!= 1)goto _LL371;_tmp4E0=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp4DF)->f1;_tmp4E1=*_tmp4E0;_tmp4E2=_tmp4DE.targs;_LL370: if((((void*)_tmp4E1->kind
== (void*)1?1: _tmp4E1->impl == 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4E1->impl))->exist_vars != 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4E1->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp4E5=_cycalloc(
sizeof(*_tmp4E5));_tmp4E5->hd=({struct _tuple4*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6->f1=Cyc_Absyn_empty_tqual();_tmp4E6->f2=t1;_tmp4E6;});_tmp4E5->tl=0;
_tmp4E5;});{struct Cyc_List_List*_tmp4E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp4E1->tvs,_tmp4E2);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp4E7,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E1->impl))->fields);}_LL371: if(
_tmp4DC <= (void*)3?1:*((int*)_tmp4DC)!= 11)goto _LL373;_tmp4E3=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4DC)->f1;if((int)_tmp4E3 != 0)goto _LL373;_tmp4E4=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4DC)->f2;_LL372: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp4E4);_LL373:;_LL374: return({struct Cyc_List_List*
_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->hd=({struct _tuple4*_tmp4E9=
_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=Cyc_Absyn_empty_tqual();_tmp4E9->f2=t1;
_tmp4E9;});_tmp4E8->tl=0;_tmp4E8;});_LL36A:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp4EB=({struct _tuple6 _tmp4EA;_tmp4EA.f1=t1;_tmp4EA.f2=t2;_tmp4EA;});void*
_tmp4EC;struct Cyc_Absyn_PtrInfo _tmp4ED;void*_tmp4EE;struct Cyc_Absyn_Tqual _tmp4EF;
struct Cyc_Absyn_PtrAtts _tmp4F0;void*_tmp4F1;struct Cyc_Absyn_Conref*_tmp4F2;
struct Cyc_Absyn_Conref*_tmp4F3;struct Cyc_Absyn_Conref*_tmp4F4;void*_tmp4F5;
struct Cyc_Absyn_PtrInfo _tmp4F6;void*_tmp4F7;struct Cyc_Absyn_Tqual _tmp4F8;struct
Cyc_Absyn_PtrAtts _tmp4F9;void*_tmp4FA;struct Cyc_Absyn_Conref*_tmp4FB;struct Cyc_Absyn_Conref*
_tmp4FC;struct Cyc_Absyn_Conref*_tmp4FD;_LL376: _tmp4EC=_tmp4EB.f1;if(_tmp4EC <= (
void*)3?1:*((int*)_tmp4EC)!= 4)goto _LL378;_tmp4ED=((struct Cyc_Absyn_PointerType_struct*)
_tmp4EC)->f1;_tmp4EE=(void*)_tmp4ED.elt_typ;_tmp4EF=_tmp4ED.elt_tq;_tmp4F0=
_tmp4ED.ptr_atts;_tmp4F1=(void*)_tmp4F0.rgn;_tmp4F2=_tmp4F0.nullable;_tmp4F3=
_tmp4F0.bounds;_tmp4F4=_tmp4F0.zero_term;_tmp4F5=_tmp4EB.f2;if(_tmp4F5 <= (void*)
3?1:*((int*)_tmp4F5)!= 4)goto _LL378;_tmp4F6=((struct Cyc_Absyn_PointerType_struct*)
_tmp4F5)->f1;_tmp4F7=(void*)_tmp4F6.elt_typ;_tmp4F8=_tmp4F6.elt_tq;_tmp4F9=
_tmp4F6.ptr_atts;_tmp4FA=(void*)_tmp4F9.rgn;_tmp4FB=_tmp4F9.nullable;_tmp4FC=
_tmp4F9.bounds;_tmp4FD=_tmp4F9.zero_term;_LL377: if(_tmp4EF.q_const?!_tmp4F8.q_const:
0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F2,_tmp4FB)?((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp4F2): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp4FB): 0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F4,
_tmp4FD)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp4F4): 0)?((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp4FD): 0)return 0;if(!
Cyc_Tcutil_unify(_tmp4F1,_tmp4FA)?!Cyc_Tcenv_region_outlives(te,_tmp4F1,_tmp4FA):
0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp4F3,_tmp4FC)){
struct _tuple6 _tmp4FF=({struct _tuple6 _tmp4FE;_tmp4FE.f1=Cyc_Absyn_conref_val(
_tmp4F3);_tmp4FE.f2=Cyc_Absyn_conref_val(_tmp4FC);_tmp4FE;});void*_tmp500;void*
_tmp501;void*_tmp502;struct Cyc_Absyn_Exp*_tmp503;void*_tmp504;struct Cyc_Absyn_Exp*
_tmp505;_LL37B: _tmp500=_tmp4FF.f1;if(_tmp500 <= (void*)1?1:*((int*)_tmp500)!= 0)
goto _LL37D;_tmp501=_tmp4FF.f2;if((int)_tmp501 != 0)goto _LL37D;_LL37C: goto _LL37A;
_LL37D: _tmp502=_tmp4FF.f1;if(_tmp502 <= (void*)1?1:*((int*)_tmp502)!= 0)goto
_LL37F;_tmp503=((struct Cyc_Absyn_Upper_b_struct*)_tmp502)->f1;_tmp504=_tmp4FF.f2;
if(_tmp504 <= (void*)1?1:*((int*)_tmp504)!= 0)goto _LL37F;_tmp505=((struct Cyc_Absyn_Upper_b_struct*)
_tmp504)->f1;_LL37E: if(!Cyc_Evexp_lte_const_exp(_tmp505,_tmp503))return 0;goto
_LL37A;_LL37F:;_LL380: return 0;_LL37A:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->hd=({struct _tuple6*
_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->f1=t1;_tmp507->f2=t2;_tmp507;});
_tmp506->tl=assume;_tmp506;}),_tmp4EE,_tmp4F7);_LL378:;_LL379: return 0;_LL375:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp509=({struct
_tuple6 _tmp508;_tmp508.f1=Cyc_Tcutil_compress(t1);_tmp508.f2=Cyc_Tcutil_compress(
t2);_tmp508;});void*_tmp50A;void*_tmp50B;void*_tmp50C;void*_tmp50D;_LL382:
_tmp50A=_tmp509.f1;if(_tmp50A <= (void*)3?1:*((int*)_tmp50A)!= 5)goto _LL384;
_tmp50B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp50A)->f2;_tmp50C=_tmp509.f2;
if(_tmp50C <= (void*)3?1:*((int*)_tmp50C)!= 5)goto _LL384;_tmp50D=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp50C)->f2;_LL383: return _tmp50B == _tmp50D;_LL384:;
_LL385: return 0;_LL381:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp50F;struct
Cyc_Absyn_Tqual _tmp510;void*_tmp511;struct _tuple4*_tmp50E=(struct _tuple4*)tqs1->hd;
_tmp50F=*_tmp50E;_tmp510=_tmp50F.f1;_tmp511=_tmp50F.f2;{struct _tuple4 _tmp513;
struct Cyc_Absyn_Tqual _tmp514;void*_tmp515;struct _tuple4*_tmp512=(struct _tuple4*)
tqs2->hd;_tmp513=*_tmp512;_tmp514=_tmp513.f1;_tmp515=_tmp513.f2;if(_tmp514.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp511,_tmp515): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp511,_tmp515))continue;else{if(Cyc_Tcutil_isomorphic(_tmp511,_tmp515))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp516=Cyc_Tcutil_compress(t);void*_tmp517;_LL387: if(_tmp516 <= (void*)3?1:*((
int*)_tmp516)!= 5)goto _LL389;_tmp517=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp516)->f2;if((int)_tmp517 != 0)goto _LL389;_LL388: return 1;_LL389:;_LL38A: return
0;_LL386:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return 1;{void*_tmp518=t2;void*
_tmp519;_LL38C: if(_tmp518 <= (void*)3?1:*((int*)_tmp518)!= 5)goto _LL38E;_tmp519=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp518)->f2;if((int)_tmp519 != 2)goto
_LL38E;_LL38D: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return 1;goto _LL38B;_LL38E:;
_LL38F: goto _LL38B;_LL38B:;}{void*_tmp51A=t1;struct Cyc_Absyn_PtrInfo _tmp51B;void*
_tmp51C;struct Cyc_Absyn_Tqual _tmp51D;struct Cyc_Absyn_PtrAtts _tmp51E;void*_tmp51F;
struct Cyc_Absyn_Conref*_tmp520;struct Cyc_Absyn_Conref*_tmp521;struct Cyc_Absyn_Conref*
_tmp522;struct Cyc_Absyn_ArrayInfo _tmp523;void*_tmp524;struct Cyc_Absyn_Tqual
_tmp525;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Conref*_tmp527;struct Cyc_Absyn_Enumdecl*
_tmp528;_LL391: if(_tmp51A <= (void*)3?1:*((int*)_tmp51A)!= 4)goto _LL393;_tmp51B=((
struct Cyc_Absyn_PointerType_struct*)_tmp51A)->f1;_tmp51C=(void*)_tmp51B.elt_typ;
_tmp51D=_tmp51B.elt_tq;_tmp51E=_tmp51B.ptr_atts;_tmp51F=(void*)_tmp51E.rgn;
_tmp520=_tmp51E.nullable;_tmp521=_tmp51E.bounds;_tmp522=_tmp51E.zero_term;_LL392:{
void*_tmp529=t2;struct Cyc_Absyn_PtrInfo _tmp52A;void*_tmp52B;struct Cyc_Absyn_Tqual
_tmp52C;struct Cyc_Absyn_PtrAtts _tmp52D;void*_tmp52E;struct Cyc_Absyn_Conref*
_tmp52F;struct Cyc_Absyn_Conref*_tmp530;struct Cyc_Absyn_Conref*_tmp531;_LL3A0: if(
_tmp529 <= (void*)3?1:*((int*)_tmp529)!= 4)goto _LL3A2;_tmp52A=((struct Cyc_Absyn_PointerType_struct*)
_tmp529)->f1;_tmp52B=(void*)_tmp52A.elt_typ;_tmp52C=_tmp52A.elt_tq;_tmp52D=
_tmp52A.ptr_atts;_tmp52E=(void*)_tmp52D.rgn;_tmp52F=_tmp52D.nullable;_tmp530=
_tmp52D.bounds;_tmp531=_tmp52D.zero_term;_LL3A1: {struct Cyc_List_List*_tmp532=({
struct Cyc_List_List*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->hd=({struct
_tuple6*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E->f1=t1;_tmp53E->f2=t2;
_tmp53E;});_tmp53D->tl=0;_tmp53D;});int _tmp533=Cyc_Tcutil_ptrsubtype(te,_tmp532,
_tmp51C,_tmp52B)?!_tmp51D.q_const?1: _tmp52C.q_const: 0;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp522,
_tmp531)?1: !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp531);
int _tmp534=_tmp533?0:(((Cyc_Tcutil_bits_only(_tmp51C)?Cyc_Tcutil_is_char_type(
_tmp52B): 0)?!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp531): 0)?_tmp52C.q_const?1: !_tmp51D.q_const: 0);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp521,_tmp530);if(!bounds_ok?!_tmp534: 0){struct _tuple6
_tmp536=({struct _tuple6 _tmp535;_tmp535.f1=Cyc_Absyn_conref_val(_tmp521);_tmp535.f2=
Cyc_Absyn_conref_val(_tmp530);_tmp535;});void*_tmp537;struct Cyc_Absyn_Exp*
_tmp538;void*_tmp539;struct Cyc_Absyn_Exp*_tmp53A;void*_tmp53B;void*_tmp53C;
_LL3A5: _tmp537=_tmp536.f1;if(_tmp537 <= (void*)1?1:*((int*)_tmp537)!= 0)goto
_LL3A7;_tmp538=((struct Cyc_Absyn_Upper_b_struct*)_tmp537)->f1;_tmp539=_tmp536.f2;
if(_tmp539 <= (void*)1?1:*((int*)_tmp539)!= 0)goto _LL3A7;_tmp53A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp539)->f1;_LL3A6: if(Cyc_Evexp_lte_const_exp(_tmp53A,_tmp538))bounds_ok=1;goto
_LL3A4;_LL3A7: _tmp53B=_tmp536.f1;if(_tmp53B <= (void*)1?1:*((int*)_tmp53B)!= 1)
goto _LL3A9;_LL3A8: goto _LL3AA;_LL3A9: _tmp53C=_tmp536.f2;if(_tmp53C <= (void*)1?1:*((
int*)_tmp53C)!= 1)goto _LL3AB;_LL3AA: bounds_ok=0;goto _LL3A4;_LL3AB:;_LL3AC:
bounds_ok=1;goto _LL3A4;_LL3A4:;}return((bounds_ok?zeroterm_ok: 0)?_tmp533?1:
_tmp534: 0)?Cyc_Tcutil_unify(_tmp51F,_tmp52E)?1: Cyc_Tcenv_region_outlives(te,
_tmp51F,_tmp52E): 0;}}_LL3A2:;_LL3A3: goto _LL39F;_LL39F:;}return 0;_LL393: if(
_tmp51A <= (void*)3?1:*((int*)_tmp51A)!= 7)goto _LL395;_tmp523=((struct Cyc_Absyn_ArrayType_struct*)
_tmp51A)->f1;_tmp524=(void*)_tmp523.elt_type;_tmp525=_tmp523.tq;_tmp526=_tmp523.num_elts;
_tmp527=_tmp523.zero_term;_LL394:{void*_tmp53F=t2;struct Cyc_Absyn_ArrayInfo
_tmp540;void*_tmp541;struct Cyc_Absyn_Tqual _tmp542;struct Cyc_Absyn_Exp*_tmp543;
struct Cyc_Absyn_Conref*_tmp544;_LL3AE: if(_tmp53F <= (void*)3?1:*((int*)_tmp53F)!= 
7)goto _LL3B0;_tmp540=((struct Cyc_Absyn_ArrayType_struct*)_tmp53F)->f1;_tmp541=(
void*)_tmp540.elt_type;_tmp542=_tmp540.tq;_tmp543=_tmp540.num_elts;_tmp544=
_tmp540.zero_term;_LL3AF: {int okay;okay=((_tmp526 != 0?_tmp543 != 0: 0)?((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp527,_tmp544): 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp543),(struct Cyc_Absyn_Exp*)_check_null(_tmp526)): 0;return(okay?
Cyc_Tcutil_unify(_tmp524,_tmp541): 0)?!_tmp525.q_const?1: _tmp542.q_const: 0;}
_LL3B0:;_LL3B1: return 0;_LL3AD:;}return 0;_LL395: if(_tmp51A <= (void*)3?1:*((int*)
_tmp51A)!= 12)goto _LL397;_tmp528=((struct Cyc_Absyn_EnumType_struct*)_tmp51A)->f2;
_LL396:{void*_tmp545=t2;struct Cyc_Absyn_Enumdecl*_tmp546;_LL3B3: if(_tmp545 <= (
void*)3?1:*((int*)_tmp545)!= 12)goto _LL3B5;_tmp546=((struct Cyc_Absyn_EnumType_struct*)
_tmp545)->f2;_LL3B4: if((_tmp528->fields != 0?_tmp546->fields != 0: 0)?((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp528->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp546->fields))->v): 0)
return 1;goto _LL3B2;_LL3B5:;_LL3B6: goto _LL3B2;_LL3B2:;}goto _LL398;_LL397: if(
_tmp51A <= (void*)3?1:*((int*)_tmp51A)!= 5)goto _LL399;_LL398: goto _LL39A;_LL399:
if((int)_tmp51A != 1)goto _LL39B;_LL39A: goto _LL39C;_LL39B: if(_tmp51A <= (void*)3?1:*((
int*)_tmp51A)!= 6)goto _LL39D;_LL39C: return Cyc_Tcutil_coerceable(t2);_LL39D:;
_LL39E: return 0;_LL390:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp547=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp548=_cycalloc(
sizeof(*_tmp548));_tmp548[0]=({struct Cyc_Absyn_Cast_e_struct _tmp549;_tmp549.tag=
13;_tmp549.f1=(void*)t;_tmp549.f2=_tmp547;_tmp549;});_tmp548;})));e->topt=({
struct Cyc_Core_Opt*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->v=(void*)t;
_tmp54A;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp54B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL3B8: if(_tmp54B <= (void*)
3?1:*((int*)_tmp54B)!= 5)goto _LL3BA;_LL3B9: goto _LL3BB;_LL3BA: if(_tmp54B <= (void*)
3?1:*((int*)_tmp54B)!= 12)goto _LL3BC;_LL3BB: goto _LL3BD;_LL3BC: if(_tmp54B <= (void*)
3?1:*((int*)_tmp54B)!= 13)goto _LL3BE;_LL3BD: goto _LL3BF;_LL3BE: if(_tmp54B <= (void*)
3?1:*((int*)_tmp54B)!= 17)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(_tmp54B <= (void*)
3?1:*((int*)_tmp54B)!= 14)goto _LL3C2;_LL3C1: return 1;_LL3C2: if(_tmp54B <= (void*)3?
1:*((int*)_tmp54B)!= 0)goto _LL3C4;_LL3C3: return Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3C4:;_LL3C5: return
0;_LL3B7:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp54C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3C7: if((int)_tmp54C != 1)goto _LL3C9;_LL3C8: goto
_LL3CA;_LL3C9: if(_tmp54C <= (void*)3?1:*((int*)_tmp54C)!= 6)goto _LL3CB;_LL3CA:
return 1;_LL3CB:;_LL3CC: return 0;_LL3C6:;}}int Cyc_Tcutil_is_function_type(void*t){
void*_tmp54D=Cyc_Tcutil_compress(t);_LL3CE: if(_tmp54D <= (void*)3?1:*((int*)
_tmp54D)!= 8)goto _LL3D0;_LL3CF: return 1;_LL3D0:;_LL3D1: return 0;_LL3CD:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple6 _tmp54F=({struct _tuple6 _tmp54E;_tmp54E.f1=t1;
_tmp54E.f2=t2;_tmp54E;});void*_tmp550;int _tmp551;void*_tmp552;int _tmp553;void*
_tmp554;void*_tmp555;void*_tmp556;void*_tmp557;void*_tmp558;void*_tmp559;void*
_tmp55A;void*_tmp55B;void*_tmp55C;void*_tmp55D;void*_tmp55E;void*_tmp55F;void*
_tmp560;void*_tmp561;void*_tmp562;void*_tmp563;void*_tmp564;void*_tmp565;void*
_tmp566;void*_tmp567;void*_tmp568;void*_tmp569;void*_tmp56A;void*_tmp56B;void*
_tmp56C;void*_tmp56D;void*_tmp56E;void*_tmp56F;_LL3D3: _tmp550=_tmp54F.f1;if(
_tmp550 <= (void*)3?1:*((int*)_tmp550)!= 6)goto _LL3D5;_tmp551=((struct Cyc_Absyn_DoubleType_struct*)
_tmp550)->f1;_tmp552=_tmp54F.f2;if(_tmp552 <= (void*)3?1:*((int*)_tmp552)!= 6)
goto _LL3D5;_tmp553=((struct Cyc_Absyn_DoubleType_struct*)_tmp552)->f1;_LL3D4: if(
_tmp551)return t1;else{return t2;}_LL3D5: _tmp554=_tmp54F.f1;if(_tmp554 <= (void*)3?
1:*((int*)_tmp554)!= 6)goto _LL3D7;_LL3D6: return t1;_LL3D7: _tmp555=_tmp54F.f2;if(
_tmp555 <= (void*)3?1:*((int*)_tmp555)!= 6)goto _LL3D9;_LL3D8: return t2;_LL3D9:
_tmp556=_tmp54F.f1;if((int)_tmp556 != 1)goto _LL3DB;_LL3DA: goto _LL3DC;_LL3DB:
_tmp557=_tmp54F.f2;if((int)_tmp557 != 1)goto _LL3DD;_LL3DC: return(void*)1;_LL3DD:
_tmp558=_tmp54F.f1;if(_tmp558 <= (void*)3?1:*((int*)_tmp558)!= 5)goto _LL3DF;
_tmp559=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp558)->f1;if((int)_tmp559 != 
1)goto _LL3DF;_tmp55A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp558)->f2;if((
int)_tmp55A != 3)goto _LL3DF;_LL3DE: goto _LL3E0;_LL3DF: _tmp55B=_tmp54F.f2;if(
_tmp55B <= (void*)3?1:*((int*)_tmp55B)!= 5)goto _LL3E1;_tmp55C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp55B)->f1;if((int)_tmp55C != 1)goto _LL3E1;_tmp55D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp55B)->f2;if((int)_tmp55D != 3)goto _LL3E1;_LL3E0: return Cyc_Absyn_ulonglong_typ;
_LL3E1: _tmp55E=_tmp54F.f1;if(_tmp55E <= (void*)3?1:*((int*)_tmp55E)!= 5)goto
_LL3E3;_tmp55F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp55E)->f1;if((int)
_tmp55F != 2)goto _LL3E3;_tmp560=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp55E)->f2;
if((int)_tmp560 != 3)goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: _tmp561=_tmp54F.f2;if(
_tmp561 <= (void*)3?1:*((int*)_tmp561)!= 5)goto _LL3E5;_tmp562=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp561)->f1;if((int)_tmp562 != 2)goto _LL3E5;_tmp563=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp561)->f2;if((int)_tmp563 != 3)goto _LL3E5;_LL3E4: goto _LL3E6;_LL3E5: _tmp564=
_tmp54F.f1;if(_tmp564 <= (void*)3?1:*((int*)_tmp564)!= 5)goto _LL3E7;_tmp565=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp564)->f1;if((int)_tmp565 != 0)goto _LL3E7;
_tmp566=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp564)->f2;if((int)_tmp566 != 
3)goto _LL3E7;_LL3E6: goto _LL3E8;_LL3E7: _tmp567=_tmp54F.f2;if(_tmp567 <= (void*)3?1:*((
int*)_tmp567)!= 5)goto _LL3E9;_tmp568=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp567)->f1;if((int)_tmp568 != 0)goto _LL3E9;_tmp569=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp567)->f2;if((int)_tmp569 != 3)goto _LL3E9;_LL3E8: return Cyc_Absyn_slonglong_typ;
_LL3E9: _tmp56A=_tmp54F.f1;if(_tmp56A <= (void*)3?1:*((int*)_tmp56A)!= 5)goto
_LL3EB;_tmp56B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56A)->f1;if((int)
_tmp56B != 1)goto _LL3EB;_tmp56C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56A)->f2;
if((int)_tmp56C != 2)goto _LL3EB;_LL3EA: goto _LL3EC;_LL3EB: _tmp56D=_tmp54F.f2;if(
_tmp56D <= (void*)3?1:*((int*)_tmp56D)!= 5)goto _LL3ED;_tmp56E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp56D)->f1;if((int)_tmp56E != 1)goto _LL3ED;_tmp56F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp56D)->f2;if((int)_tmp56F != 2)goto _LL3ED;_LL3EC: return Cyc_Absyn_uint_typ;
_LL3ED:;_LL3EE: return Cyc_Absyn_sint_typ;_LL3D2:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp570=(void*)e->r;struct Cyc_Core_Opt*_tmp571;
_LL3F0: if(*((int*)_tmp570)!= 4)goto _LL3F2;_tmp571=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp570)->f2;if(_tmp571 != 0)goto _LL3F2;_LL3F1:({void*_tmp572[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp573="assignment in test";_tag_arr(_tmp573,sizeof(char),
_get_zero_arr_size(_tmp573,19));}),_tag_arr(_tmp572,sizeof(void*),0));});goto
_LL3EF;_LL3F2:;_LL3F3: goto _LL3EF;_LL3EF:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp575=({struct _tuple6 _tmp574;_tmp574.f1=c1;_tmp574.f2=c2;_tmp574;});
void*_tmp576;void*_tmp577;void*_tmp578;void*_tmp579;void*_tmp57A;struct Cyc_Core_Opt*
_tmp57B;struct Cyc_Core_Opt**_tmp57C;void*_tmp57D;struct Cyc_Core_Opt*_tmp57E;
struct Cyc_Core_Opt**_tmp57F;void*_tmp580;struct Cyc_Core_Opt*_tmp581;struct Cyc_Core_Opt**
_tmp582;void*_tmp583;void*_tmp584;void*_tmp585;void*_tmp586;void*_tmp587;void*
_tmp588;struct Cyc_Core_Opt*_tmp589;struct Cyc_Core_Opt**_tmp58A;void*_tmp58B;void*
_tmp58C;struct Cyc_Core_Opt*_tmp58D;struct Cyc_Core_Opt**_tmp58E;void*_tmp58F;void*
_tmp590;struct Cyc_Core_Opt*_tmp591;struct Cyc_Core_Opt**_tmp592;void*_tmp593;
_LL3F5: _tmp576=_tmp575.f1;if(*((int*)_tmp576)!= 0)goto _LL3F7;_tmp577=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp576)->f1;_tmp578=_tmp575.f2;if(*((int*)_tmp578)
!= 0)goto _LL3F7;_tmp579=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp578)->f1;
_LL3F6: return _tmp577 == _tmp579;_LL3F7: _tmp57A=_tmp575.f2;if(*((int*)_tmp57A)!= 1)
goto _LL3F9;_tmp57B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp57A)->f1;_tmp57C=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp57A)->f1;_LL3F8:*
_tmp57C=({struct Cyc_Core_Opt*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->v=(
void*)c1;_tmp594;});return 1;_LL3F9: _tmp57D=_tmp575.f1;if(*((int*)_tmp57D)!= 1)
goto _LL3FB;_tmp57E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp57D)->f1;_tmp57F=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp57D)->f1;_LL3FA:*
_tmp57F=({struct Cyc_Core_Opt*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->v=(
void*)c2;_tmp595;});return 1;_LL3FB: _tmp580=_tmp575.f1;if(*((int*)_tmp580)!= 2)
goto _LL3FD;_tmp581=((struct Cyc_Absyn_Less_kb_struct*)_tmp580)->f1;_tmp582=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp580)->f1;_tmp583=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp580)->f2;_tmp584=_tmp575.f2;if(*((
int*)_tmp584)!= 0)goto _LL3FD;_tmp585=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp584)->f1;_LL3FC: if(Cyc_Tcutil_kind_leq(_tmp585,_tmp583)){*_tmp582=({struct
Cyc_Core_Opt*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->v=(void*)c2;_tmp596;});
return 1;}else{return 0;}_LL3FD: _tmp586=_tmp575.f1;if(*((int*)_tmp586)!= 0)goto
_LL3FF;_tmp587=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp586)->f1;_tmp588=
_tmp575.f2;if(*((int*)_tmp588)!= 2)goto _LL3FF;_tmp589=((struct Cyc_Absyn_Less_kb_struct*)
_tmp588)->f1;_tmp58A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp588)->f1;_tmp58B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp588)->f2;
_LL3FE: if(Cyc_Tcutil_kind_leq(_tmp587,_tmp58B)){*_tmp58A=({struct Cyc_Core_Opt*
_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->v=(void*)c1;_tmp597;});return 1;}
else{return 0;}_LL3FF: _tmp58C=_tmp575.f1;if(*((int*)_tmp58C)!= 2)goto _LL3F4;
_tmp58D=((struct Cyc_Absyn_Less_kb_struct*)_tmp58C)->f1;_tmp58E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp58C)->f1;_tmp58F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp58C)->f2;_tmp590=_tmp575.f2;if(*((int*)_tmp590)!= 2)goto _LL3F4;_tmp591=((
struct Cyc_Absyn_Less_kb_struct*)_tmp590)->f1;_tmp592=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp590)->f1;_tmp593=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp590)->f2;_LL400: if(Cyc_Tcutil_kind_leq(_tmp58F,_tmp593)){*_tmp592=({struct
Cyc_Core_Opt*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->v=(void*)c1;_tmp598;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp593,_tmp58F)){*_tmp58E=({struct Cyc_Core_Opt*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->v=(void*)c2;_tmp599;});return 1;}
else{return 0;}}_LL3F4:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp59A=_cycalloc_atomic(sizeof(*_tmp59A));_tmp59A[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp59A;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp59F;_tmp59F.tag=1;_tmp59F.f1=(unsigned int)i;{void*
_tmp59D[1]={& _tmp59F};Cyc_aprintf(({const char*_tmp59E="#%d";_tag_arr(_tmp59E,
sizeof(char),_get_zero_arr_size(_tmp59E,4));}),_tag_arr(_tmp59D,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->name=({
struct _tagged_arr*_tmp59C=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp59C[0]=s;
_tmp59C;});_tmp59B->identity=0;_tmp59B->kind=(void*)k;_tmp59B;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp5A0=*t->name;return*((const char*)
_check_unknown_subscript(_tmp5A0,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp5A3;_tmp5A3.tag=0;
_tmp5A3.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp5A1[1]={&
_tmp5A3};Cyc_printf(({const char*_tmp5A2="%s";_tag_arr(_tmp5A2,sizeof(char),
_get_zero_arr_size(_tmp5A2,3));}),_tag_arr(_tmp5A1,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp5A4=Cyc_strconcat(({const char*_tmp5A9="`";
_tag_arr(_tmp5A9,sizeof(char),_get_zero_arr_size(_tmp5A9,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp5A5=_tagged_arr_plus(_tmp5A4,
sizeof(char),1);char _tmp5A6=*((char*)_check_unknown_subscript(_tmp5A5,sizeof(
char),0));char _tmp5A7='t';if(_get_arr_size(_tmp5A5,sizeof(char))== 1?_tmp5A6 == '\000'?
_tmp5A7 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp5A5.curr)=_tmp5A7;});t->name=({
struct _tagged_arr*_tmp5A8=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp5A8[0]=(
struct _tagged_arr)_tmp5A4;_tmp5A8;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));
_tmp5AA->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp5AB=_cycalloc(sizeof(*
_tmp5AB));_tmp5AB->v=(*x).f1;_tmp5AB;});_tmp5AA->f2=(*x).f2;_tmp5AA->f3=(*x).f3;
_tmp5AA;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp5AC=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp5AC=({
struct Cyc_List_List*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->hd=(void*)((
void*)atts->hd);_tmp5AD->tl=_tmp5AC;_tmp5AD;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE[0]=({struct Cyc_Absyn_FnType_struct
_tmp5AF;_tmp5AF.tag=8;_tmp5AF.f1=({struct Cyc_Absyn_FnInfo _tmp5B0;_tmp5B0.tvars=
fd->tvs;_tmp5B0.effect=fd->effect;_tmp5B0.ret_typ=(void*)((void*)fd->ret_type);
_tmp5B0.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5B0.c_varargs=
fd->c_varargs;_tmp5B0.cyc_varargs=fd->cyc_varargs;_tmp5B0.rgn_po=fd->rgn_po;
_tmp5B0.attributes=_tmp5AC;_tmp5B0;});_tmp5AF;});_tmp5AE;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->f1=(*pr).f1;_tmp5B1->f2=t;_tmp5B1;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp5B2=_region_malloc(
rgn,sizeof(*_tmp5B2));_tmp5B2->f1=({struct _tuple15*_tmp5B3=_region_malloc(rgn,
sizeof(*_tmp5B3));_tmp5B3->f1=(*y).f1;_tmp5B3->f2=(*y).f2;_tmp5B3;});_tmp5B2->f2=(*
y).f3;_tmp5B2;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp5B5;void*_tmp5B6;struct _tuple16 _tmp5B4=*w;_tmp5B5=_tmp5B4.f1;
_tmp5B6=_tmp5B4.f2;{struct Cyc_Core_Opt*_tmp5B8;struct Cyc_Absyn_Tqual _tmp5B9;
struct _tuple15 _tmp5B7=*_tmp5B5;_tmp5B8=_tmp5B7.f1;_tmp5B9=_tmp5B7.f2;return({
struct _tuple2*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->f1=_tmp5B8;_tmp5BA->f2=
_tmp5B9;_tmp5BA->f3=_tmp5B6;_tmp5BA;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp5BB=
_cycalloc(sizeof(*_tmp5BB));_tmp5BB->name=f->name;_tmp5BB->tq=f->tq;_tmp5BB->type=(
void*)t;_tmp5BB->width=f->width;_tmp5BB->attributes=f->attributes;_tmp5BB;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp5BC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp5BD;struct Cyc_Absyn_AggrInfo _tmp5BE;void*_tmp5BF;struct Cyc_List_List*_tmp5C0;
struct Cyc_Absyn_TunionInfo _tmp5C1;void*_tmp5C2;struct Cyc_List_List*_tmp5C3;void*
_tmp5C4;struct Cyc_Absyn_TunionFieldInfo _tmp5C5;void*_tmp5C6;struct Cyc_List_List*
_tmp5C7;struct _tuple1*_tmp5C8;struct Cyc_List_List*_tmp5C9;struct Cyc_Absyn_Typedefdecl*
_tmp5CA;void**_tmp5CB;struct Cyc_Absyn_ArrayInfo _tmp5CC;void*_tmp5CD;struct Cyc_Absyn_Tqual
_tmp5CE;struct Cyc_Absyn_Exp*_tmp5CF;struct Cyc_Absyn_Conref*_tmp5D0;struct Cyc_Absyn_PtrInfo
_tmp5D1;void*_tmp5D2;struct Cyc_Absyn_Tqual _tmp5D3;struct Cyc_Absyn_PtrAtts _tmp5D4;
void*_tmp5D5;struct Cyc_Absyn_Conref*_tmp5D6;struct Cyc_Absyn_Conref*_tmp5D7;
struct Cyc_Absyn_Conref*_tmp5D8;struct Cyc_Absyn_FnInfo _tmp5D9;struct Cyc_List_List*
_tmp5DA;struct Cyc_Core_Opt*_tmp5DB;void*_tmp5DC;struct Cyc_List_List*_tmp5DD;int
_tmp5DE;struct Cyc_Absyn_VarargInfo*_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*
_tmp5E1;struct Cyc_List_List*_tmp5E2;void*_tmp5E3;struct Cyc_List_List*_tmp5E4;
struct Cyc_Core_Opt*_tmp5E5;void*_tmp5E6;void*_tmp5E7;void*_tmp5E8;void*_tmp5E9;
void*_tmp5EA;struct Cyc_List_List*_tmp5EB;_LL402: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 1)goto _LL404;_tmp5BD=((struct Cyc_Absyn_VarType_struct*)_tmp5BC)->f1;
_LL403: {struct _handler_cons _tmp5EC;_push_handler(& _tmp5EC);{int _tmp5EE=0;if(
setjmp(_tmp5EC.handler))_tmp5EE=1;if(!_tmp5EE){{void*_tmp5EF=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5BD);_npop_handler(0);return
_tmp5EF;};_pop_handler();}else{void*_tmp5ED=(void*)_exn_thrown;void*_tmp5F1=
_tmp5ED;_LL435: if(_tmp5F1 != Cyc_Core_Not_found)goto _LL437;_LL436: return t;_LL437:;
_LL438:(void)_throw(_tmp5F1);_LL434:;}}}_LL404: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 10)goto _LL406;_tmp5BE=((struct Cyc_Absyn_AggrType_struct*)_tmp5BC)->f1;
_tmp5BF=(void*)_tmp5BE.aggr_info;_tmp5C0=_tmp5BE.targs;_LL405: {struct Cyc_List_List*
_tmp5F2=Cyc_Tcutil_substs(rgn,inst,_tmp5C0);return _tmp5F2 == _tmp5C0?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3[0]=({
struct Cyc_Absyn_AggrType_struct _tmp5F4;_tmp5F4.tag=10;_tmp5F4.f1=({struct Cyc_Absyn_AggrInfo
_tmp5F5;_tmp5F5.aggr_info=(void*)_tmp5BF;_tmp5F5.targs=_tmp5F2;_tmp5F5;});
_tmp5F4;});_tmp5F3;});}_LL406: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 2)goto
_LL408;_tmp5C1=((struct Cyc_Absyn_TunionType_struct*)_tmp5BC)->f1;_tmp5C2=(void*)
_tmp5C1.tunion_info;_tmp5C3=_tmp5C1.targs;_tmp5C4=(void*)_tmp5C1.rgn;_LL407: {
struct Cyc_List_List*_tmp5F6=Cyc_Tcutil_substs(rgn,inst,_tmp5C3);void*_tmp5F7=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp5C4);return(_tmp5F6 == _tmp5C3?_tmp5F7 == _tmp5C4: 0)?t:(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8[0]=({
struct Cyc_Absyn_TunionType_struct _tmp5F9;_tmp5F9.tag=2;_tmp5F9.f1=({struct Cyc_Absyn_TunionInfo
_tmp5FA;_tmp5FA.tunion_info=(void*)_tmp5C2;_tmp5FA.targs=_tmp5F6;_tmp5FA.rgn=(
void*)_tmp5F7;_tmp5FA;});_tmp5F9;});_tmp5F8;});}_LL408: if(_tmp5BC <= (void*)3?1:*((
int*)_tmp5BC)!= 3)goto _LL40A;_tmp5C5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5BC)->f1;_tmp5C6=(void*)_tmp5C5.field_info;_tmp5C7=_tmp5C5.targs;_LL409: {
struct Cyc_List_List*_tmp5FB=Cyc_Tcutil_substs(rgn,inst,_tmp5C7);return _tmp5FB == 
_tmp5C7?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp5FC=_cycalloc(
sizeof(*_tmp5FC));_tmp5FC[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp5FD;
_tmp5FD.tag=3;_tmp5FD.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp5FE;_tmp5FE.field_info=(
void*)_tmp5C6;_tmp5FE.targs=_tmp5FB;_tmp5FE;});_tmp5FD;});_tmp5FC;});}_LL40A: if(
_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 16)goto _LL40C;_tmp5C8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5BC)->f1;_tmp5C9=((struct Cyc_Absyn_TypedefType_struct*)_tmp5BC)->f2;_tmp5CA=((
struct Cyc_Absyn_TypedefType_struct*)_tmp5BC)->f3;_tmp5CB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5BC)->f4;_LL40B: {struct Cyc_List_List*_tmp5FF=Cyc_Tcutil_substs(rgn,inst,
_tmp5C9);return _tmp5FF == _tmp5C9?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp601;_tmp601.tag=16;_tmp601.f1=_tmp5C8;_tmp601.f2=_tmp5FF;_tmp601.f3=_tmp5CA;
_tmp601.f4=_tmp5CB;_tmp601;});_tmp600;});}_LL40C: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 7)goto _LL40E;_tmp5CC=((struct Cyc_Absyn_ArrayType_struct*)_tmp5BC)->f1;
_tmp5CD=(void*)_tmp5CC.elt_type;_tmp5CE=_tmp5CC.tq;_tmp5CF=_tmp5CC.num_elts;
_tmp5D0=_tmp5CC.zero_term;_LL40D: {void*_tmp602=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp5CD);return _tmp602 == _tmp5CD?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp604;_tmp604.tag=7;_tmp604.f1=({struct Cyc_Absyn_ArrayInfo _tmp605;_tmp605.elt_type=(
void*)_tmp602;_tmp605.tq=_tmp5CE;_tmp605.num_elts=_tmp5CF;_tmp605.zero_term=
_tmp5D0;_tmp605;});_tmp604;});_tmp603;});}_LL40E: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 4)goto _LL410;_tmp5D1=((struct Cyc_Absyn_PointerType_struct*)_tmp5BC)->f1;
_tmp5D2=(void*)_tmp5D1.elt_typ;_tmp5D3=_tmp5D1.elt_tq;_tmp5D4=_tmp5D1.ptr_atts;
_tmp5D5=(void*)_tmp5D4.rgn;_tmp5D6=_tmp5D4.nullable;_tmp5D7=_tmp5D4.bounds;
_tmp5D8=_tmp5D4.zero_term;_LL40F: {void*_tmp606=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp5D2);void*_tmp607=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D5);struct Cyc_Absyn_Conref*
_tmp608=_tmp5D7;{void*_tmp609=(void*)(Cyc_Absyn_compress_conref(_tmp5D7))->v;
void*_tmp60A;void*_tmp60B;_LL43A: if(_tmp609 <= (void*)1?1:*((int*)_tmp609)!= 0)
goto _LL43C;_tmp60A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp609)->f1;if(
_tmp60A <= (void*)1?1:*((int*)_tmp60A)!= 1)goto _LL43C;_tmp60B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp60A)->f1;_LL43B: {void*_tmp60C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp60B);if(
_tmp60B != _tmp60C)_tmp608=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp60E;_tmp60E.tag=1;_tmp60E.f1=(void*)_tmp60C;_tmp60E;});_tmp60D;}));goto
_LL439;}_LL43C:;_LL43D: goto _LL439;_LL439:;}if((_tmp606 == _tmp5D2?_tmp607 == 
_tmp5D5: 0)?_tmp608 == _tmp5D7: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp610;_tmp610.tag=4;_tmp610.f1=({struct Cyc_Absyn_PtrInfo _tmp611;_tmp611.elt_typ=(
void*)_tmp606;_tmp611.elt_tq=_tmp5D3;_tmp611.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp612;_tmp612.rgn=(void*)_tmp607;_tmp612.nullable=_tmp5D6;_tmp612.bounds=
_tmp608;_tmp612.zero_term=_tmp5D8;_tmp612;});_tmp611;});_tmp610;});_tmp60F;});}
_LL410: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 8)goto _LL412;_tmp5D9=((struct
Cyc_Absyn_FnType_struct*)_tmp5BC)->f1;_tmp5DA=_tmp5D9.tvars;_tmp5DB=_tmp5D9.effect;
_tmp5DC=(void*)_tmp5D9.ret_typ;_tmp5DD=_tmp5D9.args;_tmp5DE=_tmp5D9.c_varargs;
_tmp5DF=_tmp5D9.cyc_varargs;_tmp5E0=_tmp5D9.rgn_po;_tmp5E1=_tmp5D9.attributes;
_LL411:{struct Cyc_List_List*_tmp613=_tmp5DA;for(0;_tmp613 != 0;_tmp613=_tmp613->tl){
inst=({struct Cyc_List_List*_tmp614=_region_malloc(rgn,sizeof(*_tmp614));_tmp614->hd=({
struct _tuple8*_tmp615=_region_malloc(rgn,sizeof(*_tmp615));_tmp615->f1=(struct
Cyc_Absyn_Tvar*)_tmp613->hd;_tmp615->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616[0]=({struct Cyc_Absyn_VarType_struct
_tmp617;_tmp617.tag=1;_tmp617.f1=(struct Cyc_Absyn_Tvar*)_tmp613->hd;_tmp617;});
_tmp616;});_tmp615;});_tmp614->tl=inst;_tmp614;});}}{struct Cyc_List_List*_tmp619;
struct Cyc_List_List*_tmp61A;struct _tuple0 _tmp618=((struct _tuple0(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(
struct _RegionHandle*,struct _tuple2*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5DD));_tmp619=_tmp618.f1;
_tmp61A=_tmp618.f2;{struct Cyc_List_List*_tmp61B=Cyc_Tcutil_substs(rgn,inst,
_tmp61A);struct Cyc_List_List*_tmp61C=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp619,_tmp61B));struct Cyc_Core_Opt*
eff2;if(_tmp5DB == 0)eff2=0;else{void*_tmp61D=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp5DB->v);if(_tmp61D == (void*)_tmp5DB->v)eff2=_tmp5DB;else{eff2=({struct
Cyc_Core_Opt*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->v=(void*)_tmp61D;
_tmp61E;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp5DF == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp620;struct Cyc_Absyn_Tqual _tmp621;void*
_tmp622;int _tmp623;struct Cyc_Absyn_VarargInfo _tmp61F=*_tmp5DF;_tmp620=_tmp61F.name;
_tmp621=_tmp61F.tq;_tmp622=(void*)_tmp61F.type;_tmp623=_tmp61F.inject;{void*
_tmp624=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp622);if(_tmp624 == _tmp622)
cyc_varargs2=_tmp5DF;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp625=
_cycalloc(sizeof(*_tmp625));_tmp625->name=_tmp620;_tmp625->tq=_tmp621;_tmp625->type=(
void*)_tmp624;_tmp625->inject=_tmp623;_tmp625;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp627;struct Cyc_List_List*_tmp628;struct _tuple0 _tmp626=Cyc_List_rsplit(
rgn,rgn,_tmp5E0);_tmp627=_tmp626.f1;_tmp628=_tmp626.f2;{struct Cyc_List_List*
_tmp629=Cyc_Tcutil_substs(rgn,inst,_tmp627);struct Cyc_List_List*_tmp62A=Cyc_Tcutil_substs(
rgn,inst,_tmp628);if(_tmp629 == _tmp627?_tmp62A == _tmp628: 0)rgn_po2=_tmp5E0;else{
rgn_po2=Cyc_List_zip(_tmp629,_tmp62A);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B[0]=({struct Cyc_Absyn_FnType_struct
_tmp62C;_tmp62C.tag=8;_tmp62C.f1=({struct Cyc_Absyn_FnInfo _tmp62D;_tmp62D.tvars=
_tmp5DA;_tmp62D.effect=eff2;_tmp62D.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp5DC);_tmp62D.args=_tmp61C;_tmp62D.c_varargs=_tmp5DE;_tmp62D.cyc_varargs=
cyc_varargs2;_tmp62D.rgn_po=rgn_po2;_tmp62D.attributes=_tmp5E1;_tmp62D;});
_tmp62C;});_tmp62B;});}}}}}_LL412: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 9)
goto _LL414;_tmp5E2=((struct Cyc_Absyn_TupleType_struct*)_tmp5BC)->f1;_LL413: {
struct Cyc_List_List*_tmp62E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp5E2);struct Cyc_List_List*_tmp62F=Cyc_Tcutil_substs(rgn,inst,_tmp62E);if(
_tmp62F == _tmp62E)return t;{struct Cyc_List_List*_tmp630=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp5E2,_tmp62F);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631[0]=({struct Cyc_Absyn_TupleType_struct
_tmp632;_tmp632.tag=9;_tmp632.f1=_tmp630;_tmp632;});_tmp631;});}}_LL414: if(
_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 11)goto _LL416;_tmp5E3=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5BC)->f1;_tmp5E4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5BC)->f2;_LL415: {
struct Cyc_List_List*_tmp633=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp5E4);struct Cyc_List_List*_tmp634=Cyc_Tcutil_substs(rgn,inst,_tmp633);if(
_tmp634 == _tmp633)return t;{struct Cyc_List_List*_tmp635=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp5E4,
_tmp634);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp636=_cycalloc(
sizeof(*_tmp636));_tmp636[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp637;
_tmp637.tag=11;_tmp637.f1=(void*)_tmp5E3;_tmp637.f2=_tmp635;_tmp637;});_tmp636;});}}
_LL416: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 0)goto _LL418;_tmp5E5=((struct
Cyc_Absyn_Evar_struct*)_tmp5BC)->f2;_LL417: if(_tmp5E5 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp5E5->v);else{return t;}_LL418: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 15)goto _LL41A;_tmp5E6=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5BC)->f1;_LL419: {void*_tmp638=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E6);
return _tmp638 == _tmp5E6?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp639=
_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp63A;_tmp63A.tag=15;_tmp63A.f1=(void*)_tmp638;_tmp63A;});_tmp639;});}_LL41A:
if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 14)goto _LL41C;_tmp5E7=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp5BC)->f1;_LL41B: {void*_tmp63B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp5E7);return _tmp63B == _tmp5E7?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp63D;_tmp63D.tag=14;_tmp63D.f1=(void*)_tmp63B;_tmp63D;});_tmp63C;});}_LL41C:
if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 17)goto _LL41E;_tmp5E8=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp5BC)->f1;_LL41D: {void*_tmp63E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp5E8);return _tmp63E == _tmp5E8?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_TagType_struct
_tmp640;_tmp640.tag=17;_tmp640.f1=(void*)_tmp63E;_tmp640;});_tmp63F;});}_LL41E:
if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 18)goto _LL420;_LL41F: goto _LL421;
_LL420: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 12)goto _LL422;_LL421: goto
_LL423;_LL422: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 13)goto _LL424;_LL423:
goto _LL425;_LL424: if((int)_tmp5BC != 0)goto _LL426;_LL425: goto _LL427;_LL426: if(
_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 5)goto _LL428;_LL427: goto _LL429;_LL428:
if((int)_tmp5BC != 1)goto _LL42A;_LL429: goto _LL42B;_LL42A: if(_tmp5BC <= (void*)3?1:*((
int*)_tmp5BC)!= 6)goto _LL42C;_LL42B: goto _LL42D;_LL42C: if((int)_tmp5BC != 2)goto
_LL42E;_LL42D: return t;_LL42E: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 21)goto
_LL430;_tmp5E9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5BC)->f1;_LL42F: {
void*_tmp641=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E9);return _tmp641 == _tmp5E9?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp642=_cycalloc(sizeof(*_tmp642));
_tmp642[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp643;_tmp643.tag=21;_tmp643.f1=(
void*)_tmp641;_tmp643;});_tmp642;});}_LL430: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 19)goto _LL432;_tmp5EA=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5BC)->f1;_LL431: {void*_tmp644=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5EA);
return _tmp644 == _tmp5EA?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp645=
_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_AccessEff_struct _tmp646;
_tmp646.tag=19;_tmp646.f1=(void*)_tmp644;_tmp646;});_tmp645;});}_LL432: if(
_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 20)goto _LL401;_tmp5EB=((struct Cyc_Absyn_JoinEff_struct*)
_tmp5BC)->f1;_LL433: {struct Cyc_List_List*_tmp647=Cyc_Tcutil_substs(rgn,inst,
_tmp5EB);return _tmp647 == _tmp5EB?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp649;_tmp649.tag=20;_tmp649.f1=_tmp647;_tmp649;});_tmp648;});}_LL401:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp64A=(void*)ts->hd;
struct Cyc_List_List*_tmp64B=ts->tl;void*_tmp64C=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp64A);struct Cyc_List_List*_tmp64D=Cyc_Tcutil_substs(rgn,inst,_tmp64B);if(
_tmp64A == _tmp64C?_tmp64B == _tmp64D: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->hd=(void*)
_tmp64C;_tmp64E->tl=_tmp64D;_tmp64E;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp64F=_cycalloc(
sizeof(*_tmp64F));_tmp64F->f1=tv;_tmp64F->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->v=(void*)k;_tmp650;}),({struct Cyc_Core_Opt*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->v=s;_tmp651;}));_tmp64F;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp653;struct Cyc_List_List*_tmp654;struct _RegionHandle*_tmp655;
struct _tuple9*_tmp652=env;_tmp653=*_tmp652;_tmp654=_tmp653.f1;_tmp655=_tmp653.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp656=_region_malloc(
_tmp655,sizeof(*_tmp656));_tmp656->f1=tv;_tmp656->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->v=(void*)k;_tmp657;}),({
struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->v=_tmp654;
_tmp658;}));_tmp656;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp65D;_tmp65D.tag=0;_tmp65D.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2));{struct Cyc_String_pa_struct
_tmp65C;_tmp65C.tag=0;_tmp65C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1));{struct Cyc_String_pa_struct _tmp65B;_tmp65B.tag=0;_tmp65B.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmp659[3]={& _tmp65B,& _tmp65C,&
_tmp65D};Cyc_Tcutil_terr(loc,({const char*_tmp65A="type variable %s is used with inconsistent kinds %s and %s";
_tag_arr(_tmp65A,sizeof(char),_get_zero_arr_size(_tmp65A,59));}),_tag_arr(
_tmp659,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))!= *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*_tmp65E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp65F="same type variable has different identity!";_tag_arr(_tmp65F,sizeof(
char),_get_zero_arr_size(_tmp65F,43));}),_tag_arr(_tmp65E,sizeof(void*),0));});}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*
_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->hd=tv;_tmp660->tl=tvs;_tmp660;});}
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp661[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp662="fast_add_free_tvar: bad identity in tv";
_tag_arr(_tmp662,sizeof(char),_get_zero_arr_size(_tmp662,39));}),_tag_arr(
_tmp661,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp663[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp664="fast_add_free_tvar: bad identity in tvs2";_tag_arr(_tmp664,sizeof(char),
_get_zero_arr_size(_tmp664,41));}),_tag_arr(_tmp663,sizeof(void*),0));});if(*((
int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)
_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*_tmp665=
_cycalloc(sizeof(*_tmp665));_tmp665->hd=tv;_tmp665->tl=tvs;_tmp665;});}static
struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*
tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp668;_tmp668.tag=0;
_tmp668.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string(tv));{
void*_tmp666[1]={& _tmp668};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp667="bound tvar id for %s is NULL";_tag_arr(
_tmp667,sizeof(char),_get_zero_arr_size(_tmp667,29));}),_tag_arr(_tmp666,sizeof(
void*),1));}});return({struct Cyc_List_List*_tmp669=_cycalloc(sizeof(*_tmp669));
_tmp669->hd=tv;_tmp669->tl=tvs;_tmp669;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmp66A=Cyc_Tcutil_compress(e);int _tmp66B;
_LL43F: if(_tmp66A <= (void*)3?1:*((int*)_tmp66A)!= 0)goto _LL441;_tmp66B=((struct
Cyc_Absyn_Evar_struct*)_tmp66A)->f3;_LL440:{struct Cyc_List_List*es2=es;for(0;es2
!= 0;es2=es2->tl){void*_tmp66C=Cyc_Tcutil_compress((void*)es2->hd);int _tmp66D;
_LL444: if(_tmp66C <= (void*)3?1:*((int*)_tmp66C)!= 0)goto _LL446;_tmp66D=((struct
Cyc_Absyn_Evar_struct*)_tmp66C)->f3;_LL445: if(_tmp66B == _tmp66D)return es;goto
_LL443;_LL446:;_LL447: goto _LL443;_LL443:;}}return({struct Cyc_List_List*_tmp66E=
_cycalloc(sizeof(*_tmp66E));_tmp66E->hd=(void*)e;_tmp66E->tl=es;_tmp66E;});
_LL441:;_LL442: return es;_LL43E:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp66F->tl=r;_tmp66F;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_String_pa_struct _tmp672;
_tmp672.tag=0;_tmp672.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{void*
_tmp670[1]={& _tmp672};Cyc_Tcutil_terr(loc,({const char*_tmp671="bitfield %s does not have constant width";
_tag_arr(_tmp671,sizeof(char),_get_zero_arr_size(_tmp671,41));}),_tag_arr(
_tmp670,sizeof(void*),1));}});else{unsigned int _tmp674;int _tmp675;struct _tuple7
_tmp673=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(width));
_tmp674=_tmp673.f1;_tmp675=_tmp673.f2;if(!_tmp675)({void*_tmp676[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp677="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp677,sizeof(char),_get_zero_arr_size(_tmp677,45));}),_tag_arr(_tmp676,sizeof(
void*),0));});w=_tmp674;}{void*_tmp678=Cyc_Tcutil_compress(field_typ);void*
_tmp679;_LL449: if(_tmp678 <= (void*)3?1:*((int*)_tmp678)!= 5)goto _LL44B;_tmp679=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp678)->f2;_LL44A:{void*_tmp67A=
_tmp679;_LL44E: if((int)_tmp67A != 0)goto _LL450;_LL44F: if(w > 8)({void*_tmp67B[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp67C="bitfield larger than type";_tag_arr(
_tmp67C,sizeof(char),_get_zero_arr_size(_tmp67C,26));}),_tag_arr(_tmp67B,sizeof(
void*),0));});goto _LL44D;_LL450: if((int)_tmp67A != 1)goto _LL452;_LL451: if(w > 16)({
void*_tmp67D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp67E="bitfield larger than type";
_tag_arr(_tmp67E,sizeof(char),_get_zero_arr_size(_tmp67E,26));}),_tag_arr(
_tmp67D,sizeof(void*),0));});goto _LL44D;_LL452: if((int)_tmp67A != 2)goto _LL454;
_LL453: if(w > 32)({void*_tmp67F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp680="bitfield larger than type";
_tag_arr(_tmp680,sizeof(char),_get_zero_arr_size(_tmp680,26));}),_tag_arr(
_tmp67F,sizeof(void*),0));});goto _LL44D;_LL454: if((int)_tmp67A != 3)goto _LL44D;
_LL455: if(w > 64)({void*_tmp681[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp682="bitfield larger than type";
_tag_arr(_tmp682,sizeof(char),_get_zero_arr_size(_tmp682,26));}),_tag_arr(
_tmp681,sizeof(void*),0));});goto _LL44D;_LL44D:;}goto _LL448;_LL44B:;_LL44C:({
struct Cyc_String_pa_struct _tmp686;_tmp686.tag=0;_tmp686.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ));{struct Cyc_String_pa_struct
_tmp685;_tmp685.tag=0;_tmp685.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
void*_tmp683[2]={& _tmp685,& _tmp686};Cyc_Tcutil_terr(loc,({const char*_tmp684="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp684,sizeof(char),_get_zero_arr_size(_tmp684,52));}),_tag_arr(
_tmp683,sizeof(void*),2));}}});goto _LL448;_LL448:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp687=(void*)atts->hd;_LL457: if((int)
_tmp687 != 5)goto _LL459;_LL458: continue;_LL459: if(_tmp687 <= (void*)17?1:*((int*)
_tmp687)!= 1)goto _LL45B;_LL45A: continue;_LL45B:;_LL45C:({struct Cyc_String_pa_struct
_tmp68B;_tmp68B.tag=0;_tmp68B.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_String_pa_struct _tmp68A;_tmp68A.tag=0;_tmp68A.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp688[2]={&
_tmp68A,& _tmp68B};Cyc_Tcutil_terr(loc,({const char*_tmp689="bad attribute %s on member %s";
_tag_arr(_tmp689,sizeof(char),_get_zero_arr_size(_tmp689,30));}),_tag_arr(
_tmp688,sizeof(void*),2));}}});_LL456:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp68C=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp68D;struct Cyc_Core_Opt**_tmp68E;struct Cyc_Core_Opt*
_tmp68F;struct Cyc_Core_Opt**_tmp690;struct Cyc_Absyn_Tvar*_tmp691;struct Cyc_List_List*
_tmp692;struct _tuple1*_tmp693;struct Cyc_Absyn_Enumdecl*_tmp694;struct Cyc_Absyn_Enumdecl**
_tmp695;struct Cyc_Absyn_TunionInfo _tmp696;void*_tmp697;void**_tmp698;struct Cyc_List_List*
_tmp699;struct Cyc_List_List**_tmp69A;void*_tmp69B;struct Cyc_Absyn_TunionFieldInfo
_tmp69C;void*_tmp69D;void**_tmp69E;struct Cyc_List_List*_tmp69F;struct Cyc_Absyn_PtrInfo
_tmp6A0;void*_tmp6A1;struct Cyc_Absyn_Tqual _tmp6A2;struct Cyc_Absyn_PtrAtts _tmp6A3;
void*_tmp6A4;struct Cyc_Absyn_Conref*_tmp6A5;struct Cyc_Absyn_Conref*_tmp6A6;
struct Cyc_Absyn_Conref*_tmp6A7;void*_tmp6A8;void*_tmp6A9;struct Cyc_Absyn_ArrayInfo
_tmp6AA;void*_tmp6AB;struct Cyc_Absyn_Tqual _tmp6AC;struct Cyc_Absyn_Exp*_tmp6AD;
struct Cyc_Absyn_Conref*_tmp6AE;struct Cyc_Absyn_FnInfo _tmp6AF;struct Cyc_List_List*
_tmp6B0;struct Cyc_List_List**_tmp6B1;struct Cyc_Core_Opt*_tmp6B2;struct Cyc_Core_Opt**
_tmp6B3;void*_tmp6B4;struct Cyc_List_List*_tmp6B5;int _tmp6B6;struct Cyc_Absyn_VarargInfo*
_tmp6B7;struct Cyc_List_List*_tmp6B8;struct Cyc_List_List*_tmp6B9;struct Cyc_List_List*
_tmp6BA;void*_tmp6BB;struct Cyc_List_List*_tmp6BC;struct Cyc_Absyn_AggrInfo _tmp6BD;
void*_tmp6BE;void**_tmp6BF;struct Cyc_List_List*_tmp6C0;struct Cyc_List_List**
_tmp6C1;struct _tuple1*_tmp6C2;struct Cyc_List_List*_tmp6C3;struct Cyc_List_List**
_tmp6C4;struct Cyc_Absyn_Typedefdecl*_tmp6C5;struct Cyc_Absyn_Typedefdecl**_tmp6C6;
void**_tmp6C7;void***_tmp6C8;void*_tmp6C9;void*_tmp6CA;void*_tmp6CB;struct Cyc_List_List*
_tmp6CC;_LL45E: if((int)_tmp68C != 0)goto _LL460;_LL45F: goto _LL45D;_LL460: if(
_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 0)goto _LL462;_tmp68D=((struct Cyc_Absyn_Evar_struct*)
_tmp68C)->f1;_tmp68E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp68C)->f1;_tmp68F=((struct Cyc_Absyn_Evar_struct*)_tmp68C)->f2;_tmp690=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp68C)->f2;_LL461: if(*_tmp68E == 
0)*_tmp68E=({struct Cyc_Core_Opt*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->v=(
void*)expected_kind;_tmp6CD;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp690=({struct Cyc_Core_Opt*_tmp6CE=_cycalloc(
sizeof(*_tmp6CE));_tmp6CE->v=(void*)((void*)2);_tmp6CE;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp6CF=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp6D4;_tmp6D4.tag=2;_tmp6D4.f1=0;_tmp6D4.f2=(void*)expected_kind;_tmp6D4;});
_tmp6D3;}));*_tmp690=({struct Cyc_Core_Opt*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6D1=_cycalloc(
sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_VarType_struct _tmp6D2;_tmp6D2.tag=
1;_tmp6D2.f1=_tmp6CF;_tmp6D2;});_tmp6D1;}));_tmp6D0;});_tmp691=_tmp6CF;goto
_LL463;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL45D;_LL462: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 1)goto _LL464;
_tmp691=((struct Cyc_Absyn_VarType_struct*)_tmp68C)->f1;_LL463:{void*_tmp6D5=Cyc_Absyn_compress_kb((
void*)_tmp691->kind);struct Cyc_Core_Opt*_tmp6D6;struct Cyc_Core_Opt**_tmp6D7;
_LL491: if(*((int*)_tmp6D5)!= 1)goto _LL493;_tmp6D6=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6D5)->f1;_tmp6D7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6D5)->f1;_LL492:*_tmp6D7=({struct Cyc_Core_Opt*_tmp6D8=_cycalloc(sizeof(*
_tmp6D8));_tmp6D8->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6D9=
_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6DA;
_tmp6DA.tag=2;_tmp6DA.f1=0;_tmp6DA.f2=(void*)expected_kind;_tmp6DA;});_tmp6D9;}));
_tmp6D8;});goto _LL490;_LL493:;_LL494: goto _LL490;_LL490:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp691);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp691);goto _LL45D;_LL464: if(_tmp68C <= (void*)3?1:*((int*)
_tmp68C)!= 13)goto _LL466;_tmp692=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp68C)->f1;
_LL465: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp6DB=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp6DB;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp692 != 0;_tmp692=_tmp692->tl){struct Cyc_Absyn_Enumfield*_tmp6DC=(
struct Cyc_Absyn_Enumfield*)_tmp692->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp6DC->name).f2))({struct Cyc_String_pa_struct
_tmp6DF;_tmp6DF.tag=0;_tmp6DF.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp6DC->name).f2);{void*_tmp6DD[1]={& _tmp6DF};Cyc_Tcutil_terr(_tmp6DC->loc,({
const char*_tmp6DE="duplicate enum field name %s";_tag_arr(_tmp6DE,sizeof(char),
_get_zero_arr_size(_tmp6DE,29));}),_tag_arr(_tmp6DD,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp6E0=_region_malloc(uprev_rgn,sizeof(*
_tmp6E0));_tmp6E0->hd=(*_tmp6DC->name).f2;_tmp6E0->tl=prev_fields;_tmp6E0;});}
if(_tmp6DC->tag == 0)_tmp6DC->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp6DC->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6DC->tag)))({struct Cyc_String_pa_struct _tmp6E3;_tmp6E3.tag=0;
_tmp6E3.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp6DC->name).f2);{void*
_tmp6E1[1]={& _tmp6E3};Cyc_Tcutil_terr(loc,({const char*_tmp6E2="enum field %s: expression is not constant";
_tag_arr(_tmp6E2,sizeof(char),_get_zero_arr_size(_tmp6E2,42));}),_tag_arr(
_tmp6E1,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp6DC->tag))).f1;tag_count=t1 + 1;(*_tmp6DC->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp6E5;_tmp6E5.tag=1;_tmp6E5.f1=te->ns;_tmp6E5;});
_tmp6E4;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp6DC->name).f2,({
struct _tuple17*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp6E8;_tmp6E8.tag=4;_tmp6E8.f1=(void*)t;
_tmp6E8.f2=_tmp6DC;_tmp6E8;});_tmp6E7;});_tmp6E6->f2=1;_tmp6E6;}));}}};
_pop_region(uprev_rgn);}goto _LL45D;}_LL466: if(_tmp68C <= (void*)3?1:*((int*)
_tmp68C)!= 12)goto _LL468;_tmp693=((struct Cyc_Absyn_EnumType_struct*)_tmp68C)->f1;
_tmp694=((struct Cyc_Absyn_EnumType_struct*)_tmp68C)->f2;_tmp695=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp68C)->f2;_LL467: if(*_tmp695 == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp695))->fields == 0){struct _handler_cons
_tmp6E9;_push_handler(& _tmp6E9);{int _tmp6EB=0;if(setjmp(_tmp6E9.handler))_tmp6EB=
1;if(!_tmp6EB){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp693);*_tmp695=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp6EA=(void*)_exn_thrown;void*_tmp6ED=_tmp6EA;_LL496: if(_tmp6ED != Cyc_Dict_Absent)
goto _LL498;_LL497: {struct Cyc_Tcenv_Genv*_tmp6EE=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp6EF=({struct Cyc_Absyn_Enumdecl*_tmp6F0=_cycalloc(
sizeof(*_tmp6F0));_tmp6F0->sc=(void*)((void*)3);_tmp6F0->name=_tmp693;_tmp6F0->fields=
0;_tmp6F0;});Cyc_Tc_tcEnumdecl(te,_tmp6EE,loc,_tmp6EF);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp693);*_tmp695=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL495;}}_LL498:;_LL499:(void)_throw(_tmp6ED);_LL495:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp695);*_tmp693=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL45D;}_LL468: if(_tmp68C <= (void*)3?1:*((
int*)_tmp68C)!= 2)goto _LL46A;_tmp696=((struct Cyc_Absyn_TunionType_struct*)
_tmp68C)->f1;_tmp697=(void*)_tmp696.tunion_info;_tmp698=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp68C)->f1).tunion_info;_tmp699=_tmp696.targs;_tmp69A=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp68C)->f1).targs;_tmp69B=(void*)_tmp696.rgn;
_LL469: {struct Cyc_List_List*_tmp6F1=*_tmp69A;{void*_tmp6F2=*_tmp698;struct Cyc_Absyn_UnknownTunionInfo
_tmp6F3;struct _tuple1*_tmp6F4;int _tmp6F5;struct Cyc_Absyn_Tuniondecl**_tmp6F6;
struct Cyc_Absyn_Tuniondecl*_tmp6F7;_LL49B: if(*((int*)_tmp6F2)!= 0)goto _LL49D;
_tmp6F3=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp6F2)->f1;_tmp6F4=_tmp6F3.name;
_tmp6F5=_tmp6F3.is_xtunion;_LL49C: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp6F8;_push_handler(& _tmp6F8);{int _tmp6FA=0;if(setjmp(_tmp6F8.handler))
_tmp6FA=1;if(!_tmp6FA){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp6F4);;
_pop_handler();}else{void*_tmp6F9=(void*)_exn_thrown;void*_tmp6FC=_tmp6F9;_LL4A0:
if(_tmp6FC != Cyc_Dict_Absent)goto _LL4A2;_LL4A1: {struct Cyc_Tcenv_Genv*_tmp6FD=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp6FE=({struct Cyc_Absyn_Tuniondecl*
_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705->sc=(void*)((void*)3);_tmp705->name=
_tmp6F4;_tmp705->tvs=0;_tmp705->fields=0;_tmp705->is_xtunion=_tmp6F5;_tmp705;});
Cyc_Tc_tcTuniondecl(te,_tmp6FD,loc,_tmp6FE);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp6F4);if(_tmp6F1 != 0){({struct Cyc_String_pa_struct _tmp702;_tmp702.tag=0;
_tmp702.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6F4));{struct Cyc_String_pa_struct _tmp701;_tmp701.tag=0;_tmp701.f1=(struct
_tagged_arr)(_tmp6F5?({const char*_tmp703="xtunion";_tag_arr(_tmp703,sizeof(char),
_get_zero_arr_size(_tmp703,8));}):({const char*_tmp704="tunion";_tag_arr(_tmp704,
sizeof(char),_get_zero_arr_size(_tmp704,7));}));{void*_tmp6FF[2]={& _tmp701,&
_tmp702};Cyc_Tcutil_terr(loc,({const char*_tmp700="declare parameterized %s %s before using";
_tag_arr(_tmp700,sizeof(char),_get_zero_arr_size(_tmp700,41));}),_tag_arr(
_tmp6FF,sizeof(void*),2));}}});return cvtenv;}goto _LL49F;}_LL4A2:;_LL4A3:(void)
_throw(_tmp6FC);_LL49F:;}}}if((*tudp)->is_xtunion != _tmp6F5)({struct Cyc_String_pa_struct
_tmp708;_tmp708.tag=0;_tmp708.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6F4));{void*_tmp706[1]={& _tmp708};Cyc_Tcutil_terr(loc,({const char*_tmp707="[x]tunion is different from type declaration %s";
_tag_arr(_tmp707,sizeof(char),_get_zero_arr_size(_tmp707,48));}),_tag_arr(
_tmp706,sizeof(void*),1));}});*_tmp698=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp70A;_tmp70A.tag=1;_tmp70A.f1=tudp;_tmp70A;});_tmp709;});_tmp6F7=*tudp;goto
_LL49E;}_LL49D: if(*((int*)_tmp6F2)!= 1)goto _LL49A;_tmp6F6=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6F2)->f1;_tmp6F7=*_tmp6F6;_LL49E: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp69B);{struct Cyc_List_List*tvs=_tmp6F7->tvs;for(0;_tmp6F1 != 0?
tvs != 0: 0;(_tmp6F1=_tmp6F1->tl,tvs=tvs->tl)){void*t1=(void*)_tmp6F1->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp6F1 != 0)({struct Cyc_String_pa_struct _tmp70D;_tmp70D.tag=
0;_tmp70D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6F7->name));{void*_tmp70B[1]={& _tmp70D};Cyc_Tcutil_terr(loc,({const char*
_tmp70C="too many type arguments for tunion %s";_tag_arr(_tmp70C,sizeof(char),
_get_zero_arr_size(_tmp70C,38));}),_tag_arr(_tmp70B,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->hd=(void*)e;_tmp70E->tl=
hidden_ts;_tmp70E;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp69A=Cyc_List_imp_append(*_tmp69A,Cyc_List_imp_rev(hidden_ts));}goto _LL49A;}
_LL49A:;}goto _LL45D;}_LL46A: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 3)goto
_LL46C;_tmp69C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp68C)->f1;_tmp69D=(
void*)_tmp69C.field_info;_tmp69E=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp68C)->f1).field_info;_tmp69F=_tmp69C.targs;_LL46B:{void*_tmp70F=*_tmp69E;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp710;struct _tuple1*_tmp711;struct
_tuple1*_tmp712;int _tmp713;struct Cyc_Absyn_Tuniondecl*_tmp714;struct Cyc_Absyn_Tunionfield*
_tmp715;_LL4A5: if(*((int*)_tmp70F)!= 0)goto _LL4A7;_tmp710=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp70F)->f1;_tmp711=_tmp710.tunion_name;_tmp712=_tmp710.field_name;_tmp713=
_tmp710.is_xtunion;_LL4A6: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp716;_push_handler(& _tmp716);{int _tmp718=0;if(setjmp(
_tmp716.handler))_tmp718=1;if(!_tmp718){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp711);;_pop_handler();}else{void*_tmp717=(void*)_exn_thrown;void*_tmp71A=
_tmp717;_LL4AA: if(_tmp71A != Cyc_Dict_Absent)goto _LL4AC;_LL4AB:({struct Cyc_String_pa_struct
_tmp71D;_tmp71D.tag=0;_tmp71D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp711));{void*_tmp71B[1]={& _tmp71D};Cyc_Tcutil_terr(loc,({const char*_tmp71C="unbound type tunion %s";
_tag_arr(_tmp71C,sizeof(char),_get_zero_arr_size(_tmp71C,23));}),_tag_arr(
_tmp71B,sizeof(void*),1));}});return cvtenv;_LL4AC:;_LL4AD:(void)_throw(_tmp71A);
_LL4A9:;}}}{struct _handler_cons _tmp71E;_push_handler(& _tmp71E);{int _tmp720=0;if(
setjmp(_tmp71E.handler))_tmp720=1;if(!_tmp720){{void*_tmp721=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp712);struct Cyc_Absyn_Tuniondecl*_tmp722;struct Cyc_Absyn_Tunionfield*
_tmp723;_LL4AF: if(*((int*)_tmp721)!= 2)goto _LL4B1;_tmp722=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp721)->f1;_tmp723=((struct Cyc_Tcenv_TunionRes_struct*)_tmp721)->f2;_LL4B0: tuf=
_tmp723;tud=_tmp722;if(tud->is_xtunion != _tmp713)({struct Cyc_String_pa_struct
_tmp726;_tmp726.tag=0;_tmp726.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp711));{void*_tmp724[1]={& _tmp726};Cyc_Tcutil_terr(loc,({const char*_tmp725="[x]tunion is different from type declaration %s";
_tag_arr(_tmp725,sizeof(char),_get_zero_arr_size(_tmp725,48));}),_tag_arr(
_tmp724,sizeof(void*),1));}});goto _LL4AE;_LL4B1:;_LL4B2:({struct Cyc_String_pa_struct
_tmp72A;_tmp72A.tag=0;_tmp72A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp711));{struct Cyc_String_pa_struct _tmp729;_tmp729.tag=0;_tmp729.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp712));{void*_tmp727[
2]={& _tmp729,& _tmp72A};Cyc_Tcutil_terr(loc,({const char*_tmp728="unbound field %s in type tunion %s";
_tag_arr(_tmp728,sizeof(char),_get_zero_arr_size(_tmp728,35));}),_tag_arr(
_tmp727,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp72B=cvtenv;
_npop_handler(0);return _tmp72B;}_LL4AE:;};_pop_handler();}else{void*_tmp71F=(
void*)_exn_thrown;void*_tmp72D=_tmp71F;_LL4B4: if(_tmp72D != Cyc_Dict_Absent)goto
_LL4B6;_LL4B5:({struct Cyc_String_pa_struct _tmp731;_tmp731.tag=0;_tmp731.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp711));{struct
Cyc_String_pa_struct _tmp730;_tmp730.tag=0;_tmp730.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp712));{void*_tmp72E[2]={& _tmp730,&
_tmp731};Cyc_Tcutil_terr(loc,({const char*_tmp72F="unbound field %s in type tunion %s";
_tag_arr(_tmp72F,sizeof(char),_get_zero_arr_size(_tmp72F,35));}),_tag_arr(
_tmp72E,sizeof(void*),2));}}});return cvtenv;_LL4B6:;_LL4B7:(void)_throw(_tmp72D);
_LL4B3:;}}}*_tmp69E=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp732=
_cycalloc(sizeof(*_tmp732));_tmp732[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp733;_tmp733.tag=1;_tmp733.f1=tud;_tmp733.f2=tuf;_tmp733;});_tmp732;});
_tmp714=tud;_tmp715=tuf;goto _LL4A8;}_LL4A7: if(*((int*)_tmp70F)!= 1)goto _LL4A4;
_tmp714=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp70F)->f1;_tmp715=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp70F)->f2;_LL4A8: {struct Cyc_List_List*tvs=
_tmp714->tvs;for(0;_tmp69F != 0?tvs != 0: 0;(_tmp69F=_tmp69F->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp69F->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp69F != 0)({
struct Cyc_String_pa_struct _tmp737;_tmp737.tag=0;_tmp737.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp715->name));{struct Cyc_String_pa_struct
_tmp736;_tmp736.tag=0;_tmp736.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp714->name));{void*_tmp734[2]={& _tmp736,& _tmp737};Cyc_Tcutil_terr(loc,({const
char*_tmp735="too many type arguments for tunion %s.%s";_tag_arr(_tmp735,sizeof(
char),_get_zero_arr_size(_tmp735,41));}),_tag_arr(_tmp734,sizeof(void*),2));}}});
if(tvs != 0)({struct Cyc_String_pa_struct _tmp73B;_tmp73B.tag=0;_tmp73B.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp715->name));{struct
Cyc_String_pa_struct _tmp73A;_tmp73A.tag=0;_tmp73A.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp714->name));{void*_tmp738[2]={& _tmp73A,&
_tmp73B};Cyc_Tcutil_terr(loc,({const char*_tmp739="too few type arguments for tunion %s.%s";
_tag_arr(_tmp739,sizeof(char),_get_zero_arr_size(_tmp739,40));}),_tag_arr(
_tmp738,sizeof(void*),2));}}});goto _LL4A4;}_LL4A4:;}goto _LL45D;_LL46C: if(_tmp68C
<= (void*)3?1:*((int*)_tmp68C)!= 4)goto _LL46E;_tmp6A0=((struct Cyc_Absyn_PointerType_struct*)
_tmp68C)->f1;_tmp6A1=(void*)_tmp6A0.elt_typ;_tmp6A2=_tmp6A0.elt_tq;_tmp6A3=
_tmp6A0.ptr_atts;_tmp6A4=(void*)_tmp6A3.rgn;_tmp6A5=_tmp6A3.nullable;_tmp6A6=
_tmp6A3.bounds;_tmp6A7=_tmp6A3.zero_term;_LL46D: {int is_zero_terminated;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp6A1);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp6A4);{void*_tmp73C=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp6A7))->v;int _tmp73D;
_LL4B9: if((int)_tmp73C != 0)goto _LL4BB;_LL4BA:{void*_tmp73E=Cyc_Tcutil_compress(
_tmp6A1);void*_tmp73F;_LL4C0: if(_tmp73E <= (void*)3?1:*((int*)_tmp73E)!= 5)goto
_LL4C2;_tmp73F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp73E)->f2;if((int)
_tmp73F != 0)goto _LL4C2;_LL4C1:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6A7,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4BF;_LL4C2:;_LL4C3:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp6A7,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4BF;
_LL4BF:;}goto _LL4B8;_LL4BB: if(_tmp73C <= (void*)1?1:*((int*)_tmp73C)!= 0)goto
_LL4BD;_tmp73D=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp73C)->f1;if(_tmp73D
!= 1)goto _LL4BD;_LL4BC: if(!Cyc_Tcutil_admits_zero(_tmp6A1))({struct Cyc_String_pa_struct
_tmp742;_tmp742.tag=0;_tmp742.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp6A1));{void*_tmp740[1]={& _tmp742};Cyc_Tcutil_terr(loc,({const char*_tmp741="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp741,sizeof(char),_get_zero_arr_size(_tmp741,53));}),_tag_arr(
_tmp740,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4B8;_LL4BD:;_LL4BE:
is_zero_terminated=0;goto _LL4B8;_LL4B8:;}{void*_tmp743=(void*)(Cyc_Absyn_compress_conref(
_tmp6A6))->v;void*_tmp744;void*_tmp745;struct Cyc_Absyn_Exp*_tmp746;void*_tmp747;
void*_tmp748;_LL4C5: if((int)_tmp743 != 0)goto _LL4C7;_LL4C6: goto _LL4C8;_LL4C7: if(
_tmp743 <= (void*)1?1:*((int*)_tmp743)!= 0)goto _LL4C9;_tmp744=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp743)->f1;if((int)_tmp744 != 0)goto _LL4C9;_LL4C8: goto _LL4C4;_LL4C9: if(_tmp743
<= (void*)1?1:*((int*)_tmp743)!= 0)goto _LL4CB;_tmp745=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp743)->f1;if(_tmp745 <= (void*)1?1:*((int*)_tmp745)!= 0)goto _LL4CB;_tmp746=((
struct Cyc_Absyn_Upper_b_struct*)_tmp745)->f1;_LL4CA: Cyc_Tcexp_tcExp(te,0,_tmp746);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp746))({void*_tmp749[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp74A="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp74A,sizeof(char),_get_zero_arr_size(_tmp74A,49));}),_tag_arr(
_tmp749,sizeof(void*),0));});{unsigned int _tmp74C;int _tmp74D;struct _tuple7
_tmp74B=Cyc_Evexp_eval_const_uint_exp(_tmp746);_tmp74C=_tmp74B.f1;_tmp74D=
_tmp74B.f2;if(is_zero_terminated?!_tmp74D?1: _tmp74C < 1: 0)({void*_tmp74E[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp74F="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp74F,sizeof(char),_get_zero_arr_size(_tmp74F,55));}),_tag_arr(
_tmp74E,sizeof(void*),0));});goto _LL4C4;}_LL4CB: if(_tmp743 <= (void*)1?1:*((int*)
_tmp743)!= 0)goto _LL4CD;_tmp747=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp743)->f1;if(_tmp747 <= (void*)1?1:*((int*)_tmp747)!= 1)goto _LL4CD;_tmp748=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp747)->f1;_LL4CC: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp748);goto _LL4C4;_LL4CD: if(_tmp743 <= (void*)1?1:*((int*)
_tmp743)!= 1)goto _LL4C4;_LL4CE:({void*_tmp750[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp751="forward constraint";
_tag_arr(_tmp751,sizeof(char),_get_zero_arr_size(_tmp751,19));}),_tag_arr(
_tmp750,sizeof(void*),0));});_LL4C4:;}goto _LL45D;}_LL46E: if(_tmp68C <= (void*)3?1:*((
int*)_tmp68C)!= 17)goto _LL470;_tmp6A8=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp68C)->f1;_LL46F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp6A8);goto _LL45D;_LL470: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 14)goto
_LL472;_tmp6A9=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp68C)->f1;_LL471:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp6A9);goto _LL45D;
_LL472: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 5)goto _LL474;_LL473: goto _LL475;
_LL474: if((int)_tmp68C != 1)goto _LL476;_LL475: goto _LL477;_LL476: if(_tmp68C <= (
void*)3?1:*((int*)_tmp68C)!= 6)goto _LL478;_LL477: goto _LL45D;_LL478: if(_tmp68C <= (
void*)3?1:*((int*)_tmp68C)!= 7)goto _LL47A;_tmp6AA=((struct Cyc_Absyn_ArrayType_struct*)
_tmp68C)->f1;_tmp6AB=(void*)_tmp6AA.elt_type;_tmp6AC=_tmp6AA.tq;_tmp6AD=_tmp6AA.num_elts;
_tmp6AE=_tmp6AA.zero_term;_LL479: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp6AB);{int is_zero_terminated;{void*_tmp752=(void*)(((struct
Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp6AE))->v;int _tmp753;_LL4D0: if((int)_tmp752 != 0)goto _LL4D2;_LL4D1:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp6AE,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4CF;
_LL4D2: if(_tmp752 <= (void*)1?1:*((int*)_tmp752)!= 0)goto _LL4D4;_tmp753=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp752)->f1;if(_tmp753 != 1)goto _LL4D4;_LL4D3:
if(!Cyc_Tcutil_admits_zero(_tmp6AB))({struct Cyc_String_pa_struct _tmp756;_tmp756.tag=
0;_tmp756.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp6AB));{void*_tmp754[1]={& _tmp756};Cyc_Tcutil_terr(loc,({const char*_tmp755="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp755,sizeof(char),_get_zero_arr_size(_tmp755,51));}),_tag_arr(
_tmp754,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4CF;_LL4D4:;_LL4D5:
is_zero_terminated=0;goto _LL4CF;_LL4CF:;}if(_tmp6AD == 0)({void*_tmp757[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp758="an explicit array bound is required here";_tag_arr(
_tmp758,sizeof(char),_get_zero_arr_size(_tmp758,41));}),_tag_arr(_tmp757,sizeof(
void*),0));});else{Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(
_tmp6AD));if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(
_tmp6AD)))({void*_tmp759[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp75A="array bounds expression is not constant";
_tag_arr(_tmp75A,sizeof(char),_get_zero_arr_size(_tmp75A,40));}),_tag_arr(
_tmp759,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6AD)))({void*_tmp75B[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp75C="array bounds expression is not an unsigned int";_tag_arr(_tmp75C,
sizeof(char),_get_zero_arr_size(_tmp75C,47));}),_tag_arr(_tmp75B,sizeof(void*),0));});{
unsigned int _tmp75E;int _tmp75F;struct _tuple7 _tmp75D=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp6AD));_tmp75E=_tmp75D.f1;_tmp75F=_tmp75D.f2;
if((is_zero_terminated?_tmp75F: 0)?_tmp75E < 1: 0)({void*_tmp760[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp761="zero terminated array cannot have zero elements";
_tag_arr(_tmp761,sizeof(char),_get_zero_arr_size(_tmp761,48));}),_tag_arr(
_tmp760,sizeof(void*),0));});}}goto _LL45D;}_LL47A: if(_tmp68C <= (void*)3?1:*((int*)
_tmp68C)!= 8)goto _LL47C;_tmp6AF=((struct Cyc_Absyn_FnType_struct*)_tmp68C)->f1;
_tmp6B0=_tmp6AF.tvars;_tmp6B1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp68C)->f1).tvars;_tmp6B2=_tmp6AF.effect;_tmp6B3=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp68C)->f1).effect;_tmp6B4=(void*)_tmp6AF.ret_typ;
_tmp6B5=_tmp6AF.args;_tmp6B6=_tmp6AF.c_varargs;_tmp6B7=_tmp6AF.cyc_varargs;
_tmp6B8=_tmp6AF.rgn_po;_tmp6B9=_tmp6AF.attributes;_LL47B: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp6B9 != 0;_tmp6B9=_tmp6B9->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp6B9->hd))({struct Cyc_String_pa_struct _tmp764;
_tmp764.tag=0;_tmp764.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp6B9->hd));{void*_tmp762[1]={& _tmp764};Cyc_Tcutil_terr(loc,({const char*
_tmp763="bad function type attribute %s";_tag_arr(_tmp763,sizeof(char),
_get_zero_arr_size(_tmp763,31));}),_tag_arr(_tmp762,sizeof(void*),1));}});{void*
_tmp765=(void*)_tmp6B9->hd;void*_tmp766;int _tmp767;int _tmp768;_LL4D7: if((int)
_tmp765 != 0)goto _LL4D9;_LL4D8: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL4D6;_LL4D9: if((int)_tmp765 != 1)goto _LL4DB;_LL4DA: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL4D6;_LL4DB: if((int)_tmp765 != 2)goto _LL4DD;_LL4DC: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL4D6;_LL4DD: if(_tmp765 <= (void*)
17?1:*((int*)_tmp765)!= 3)goto _LL4DF;_tmp766=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp765)->f1;_tmp767=((struct Cyc_Absyn_Format_att_struct*)_tmp765)->f2;_tmp768=((
struct Cyc_Absyn_Format_att_struct*)_tmp765)->f3;_LL4DE: if(!seen_format){
seen_format=1;ft=_tmp766;fmt_desc_arg=_tmp767;fmt_arg_start=_tmp768;}else{({void*
_tmp769[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp76A="function can't have multiple format attributes";
_tag_arr(_tmp76A,sizeof(char),_get_zero_arr_size(_tmp76A,47));}),_tag_arr(
_tmp769,sizeof(void*),0));});}goto _LL4D6;_LL4DF:;_LL4E0: goto _LL4D6;_LL4D6:;}}if(
num_convs > 1)({void*_tmp76B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp76C="function can't have multiple calling conventions";
_tag_arr(_tmp76C,sizeof(char),_get_zero_arr_size(_tmp76C,49));}),_tag_arr(
_tmp76B,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp6B1);{struct
Cyc_List_List*b=*_tmp6B1;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp76D=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp76E;_LL4E2: if(*((int*)_tmp76D)!= 0)goto
_LL4E4;_tmp76E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp76D)->f1;if((int)
_tmp76E != 1)goto _LL4E4;_LL4E3:({struct Cyc_String_pa_struct _tmp771;_tmp771.tag=0;
_tmp771.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp76F[1]={& _tmp771};Cyc_Tcutil_terr(loc,({const char*_tmp770="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp770,sizeof(char),_get_zero_arr_size(_tmp770,51));}),_tag_arr(
_tmp76F,sizeof(void*),1));}});goto _LL4E1;_LL4E4:;_LL4E5: goto _LL4E1;_LL4E1:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp772=({struct Cyc_Tcutil_CVTEnv _tmp7EC;_tmp7EC.kind_env=
cvtenv.kind_env;_tmp7EC.free_vars=0;_tmp7EC.free_evars=0;_tmp7EC.generalize_evars=
cvtenv.generalize_evars;_tmp7EC.fn_result=1;_tmp7EC;});_tmp772=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp772,(void*)1,_tmp6B4);_tmp772.fn_result=0;{struct Cyc_List_List*a=
_tmp6B5;for(0;a != 0;a=a->tl){_tmp772=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp772,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp6B7 != 0){if(_tmp6B6)({
void*_tmp773[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp774="both c_vararg and cyc_vararg";_tag_arr(_tmp774,sizeof(char),
_get_zero_arr_size(_tmp774,29));}),_tag_arr(_tmp773,sizeof(void*),0));});{void*
_tmp776;int _tmp777;struct Cyc_Absyn_VarargInfo _tmp775=*_tmp6B7;_tmp776=(void*)
_tmp775.type;_tmp777=_tmp775.inject;_tmp772=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp772,(void*)1,_tmp776);if(_tmp777){void*_tmp778=Cyc_Tcutil_compress(_tmp776);
struct Cyc_Absyn_TunionInfo _tmp779;void*_tmp77A;_LL4E7: if(_tmp778 <= (void*)3?1:*((
int*)_tmp778)!= 2)goto _LL4E9;_tmp779=((struct Cyc_Absyn_TunionType_struct*)
_tmp778)->f1;_tmp77A=(void*)_tmp779.tunion_info;if(*((int*)_tmp77A)!= 1)goto
_LL4E9;_LL4E8: goto _LL4E6;_LL4E9:;_LL4EA:({void*_tmp77B[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp77C="can't inject a non-[x]tunion type";_tag_arr(_tmp77C,sizeof(
char),_get_zero_arr_size(_tmp77C,34));}),_tag_arr(_tmp77B,sizeof(void*),0));});
goto _LL4E6;_LL4E6:;}}}if(seen_format){int _tmp77D=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp6B5);if((((fmt_desc_arg < 0?1: fmt_desc_arg > _tmp77D)?1:
fmt_arg_start < 0)?1:(_tmp6B7 == 0?fmt_arg_start != 0: 0))?1:(_tmp6B7 != 0?
fmt_arg_start != _tmp77D + 1: 0))({void*_tmp77E[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp77F="bad format descriptor";_tag_arr(_tmp77F,sizeof(char),
_get_zero_arr_size(_tmp77F,22));}),_tag_arr(_tmp77E,sizeof(void*),0));});else{
void*_tmp781;struct _tuple2 _tmp780=*((struct _tuple2*(*)(struct Cyc_List_List*x,int
n))Cyc_List_nth)(_tmp6B5,fmt_desc_arg - 1);_tmp781=_tmp780.f3;{void*_tmp782=Cyc_Tcutil_compress(
_tmp781);struct Cyc_Absyn_PtrInfo _tmp783;void*_tmp784;struct Cyc_Absyn_PtrAtts
_tmp785;struct Cyc_Absyn_Conref*_tmp786;struct Cyc_Absyn_Conref*_tmp787;_LL4EC: if(
_tmp782 <= (void*)3?1:*((int*)_tmp782)!= 4)goto _LL4EE;_tmp783=((struct Cyc_Absyn_PointerType_struct*)
_tmp782)->f1;_tmp784=(void*)_tmp783.elt_typ;_tmp785=_tmp783.ptr_atts;_tmp786=
_tmp785.bounds;_tmp787=_tmp785.zero_term;_LL4ED:{struct _tuple6 _tmp789=({struct
_tuple6 _tmp788;_tmp788.f1=Cyc_Tcutil_compress(_tmp784);_tmp788.f2=Cyc_Absyn_conref_def((
void*)0,_tmp786);_tmp788;});void*_tmp78A;void*_tmp78B;void*_tmp78C;void*_tmp78D;
_LL4F1: _tmp78A=_tmp789.f1;if(_tmp78A <= (void*)3?1:*((int*)_tmp78A)!= 5)goto
_LL4F3;_tmp78B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp78A)->f1;if((int)
_tmp78B != 2)goto _LL4F3;_tmp78C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp78A)->f2;
if((int)_tmp78C != 0)goto _LL4F3;_tmp78D=_tmp789.f2;if((int)_tmp78D != 0)goto _LL4F3;
_LL4F2: goto _LL4F0;_LL4F3:;_LL4F4:({void*_tmp78E[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp78F="format descriptor is not a char ? type";_tag_arr(_tmp78F,
sizeof(char),_get_zero_arr_size(_tmp78F,39));}),_tag_arr(_tmp78E,sizeof(void*),0));});
goto _LL4F0;_LL4F0:;}goto _LL4EB;_LL4EE:;_LL4EF:({void*_tmp790[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp791="format descriptor is not a char ? type";_tag_arr(
_tmp791,sizeof(char),_get_zero_arr_size(_tmp791,39));}),_tag_arr(_tmp790,sizeof(
void*),0));});goto _LL4EB;_LL4EB:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp793=({struct _tuple6 _tmp792;_tmp792.f1=ft;_tmp792.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp6B7))->type);_tmp792;});void*
_tmp794;void*_tmp795;struct Cyc_Absyn_TunionInfo _tmp796;void*_tmp797;struct Cyc_Absyn_Tuniondecl**
_tmp798;struct Cyc_Absyn_Tuniondecl*_tmp799;void*_tmp79A;void*_tmp79B;struct Cyc_Absyn_TunionInfo
_tmp79C;void*_tmp79D;struct Cyc_Absyn_Tuniondecl**_tmp79E;struct Cyc_Absyn_Tuniondecl*
_tmp79F;_LL4F6: _tmp794=_tmp793.f1;if((int)_tmp794 != 0)goto _LL4F8;_tmp795=_tmp793.f2;
if(_tmp795 <= (void*)3?1:*((int*)_tmp795)!= 2)goto _LL4F8;_tmp796=((struct Cyc_Absyn_TunionType_struct*)
_tmp795)->f1;_tmp797=(void*)_tmp796.tunion_info;if(*((int*)_tmp797)!= 1)goto
_LL4F8;_tmp798=((struct Cyc_Absyn_KnownTunion_struct*)_tmp797)->f1;_tmp799=*
_tmp798;_LL4F7: problem=Cyc_Absyn_qvar_cmp(_tmp799->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL4F5;_LL4F8: _tmp79A=_tmp793.f1;if((int)_tmp79A != 1)goto _LL4FA;_tmp79B=
_tmp793.f2;if(_tmp79B <= (void*)3?1:*((int*)_tmp79B)!= 2)goto _LL4FA;_tmp79C=((
struct Cyc_Absyn_TunionType_struct*)_tmp79B)->f1;_tmp79D=(void*)_tmp79C.tunion_info;
if(*((int*)_tmp79D)!= 1)goto _LL4FA;_tmp79E=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp79D)->f1;_tmp79F=*_tmp79E;_LL4F9: problem=Cyc_Absyn_qvar_cmp(_tmp79F->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4F5;_LL4FA:;_LL4FB: problem=1;goto
_LL4F5;_LL4F5:;}if(problem)({void*_tmp7A0[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp7A1="format attribute and vararg types don't match";_tag_arr(_tmp7A1,sizeof(
char),_get_zero_arr_size(_tmp7A1,46));}),_tag_arr(_tmp7A0,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp6B8;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp7A3;
void*_tmp7A4;void*_tmp7A5;struct _tuple6*_tmp7A2=(struct _tuple6*)rpo->hd;_tmp7A3=*
_tmp7A2;_tmp7A4=_tmp7A3.f1;_tmp7A5=_tmp7A3.f2;_tmp772=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp772,(void*)4,_tmp7A4);_tmp772=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp772,(void*)3,_tmp7A5);}}if(*_tmp6B3 != 0)_tmp772=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp772,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp6B3))->v);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp772.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp7A6=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp7A7;struct Cyc_Core_Opt**_tmp7A8;void*
_tmp7A9;void*_tmp7AA;void*_tmp7AB;void*_tmp7AC;struct Cyc_Core_Opt*_tmp7AD;struct
Cyc_Core_Opt**_tmp7AE;void*_tmp7AF;void*_tmp7B0;struct Cyc_Core_Opt*_tmp7B1;
struct Cyc_Core_Opt**_tmp7B2;_LL4FD: if(*((int*)_tmp7A6)!= 2)goto _LL4FF;_tmp7A7=((
struct Cyc_Absyn_Less_kb_struct*)_tmp7A6)->f1;_tmp7A8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp7A6)->f1;_tmp7A9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7A6)->f2;if((int)_tmp7A9 != 3)goto _LL4FF;_LL4FE:*_tmp7A8=({struct Cyc_Core_Opt*
_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7B5;_tmp7B5.tag=0;_tmp7B5.f1=(void*)((void*)3);_tmp7B5;});_tmp7B4;}));
_tmp7B3;});goto _LL500;_LL4FF: if(*((int*)_tmp7A6)!= 0)goto _LL501;_tmp7AA=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp7A6)->f1;if((int)_tmp7AA != 3)goto _LL501;_LL500:
effect=({struct Cyc_List_List*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp7B7=_cycalloc(sizeof(*
_tmp7B7));_tmp7B7[0]=({struct Cyc_Absyn_AccessEff_struct _tmp7B8;_tmp7B8.tag=19;
_tmp7B8.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7B9=_cycalloc(
sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Absyn_VarType_struct _tmp7BA;_tmp7BA.tag=
1;_tmp7BA.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp7BA;});_tmp7B9;}));_tmp7B8;});
_tmp7B7;}));_tmp7B6->tl=effect;_tmp7B6;});goto _LL4FC;_LL501: if(*((int*)_tmp7A6)
!= 2)goto _LL503;_tmp7AB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7A6)->f2;
if((int)_tmp7AB != 5)goto _LL503;_LL502: goto _LL504;_LL503: if(*((int*)_tmp7A6)!= 0)
goto _LL505;_tmp7AC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7A6)->f1;if((int)
_tmp7AC != 5)goto _LL505;_LL504: goto _LL4FC;_LL505: if(*((int*)_tmp7A6)!= 2)goto
_LL507;_tmp7AD=((struct Cyc_Absyn_Less_kb_struct*)_tmp7A6)->f1;_tmp7AE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp7A6)->f1;_tmp7AF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7A6)->f2;if((int)_tmp7AF != 4)goto _LL507;_LL506:*_tmp7AE=({struct Cyc_Core_Opt*
_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7BD;_tmp7BD.tag=0;_tmp7BD.f1=(void*)((void*)4);_tmp7BD;});_tmp7BC;}));
_tmp7BB;});goto _LL508;_LL507: if(*((int*)_tmp7A6)!= 0)goto _LL509;_tmp7B0=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp7A6)->f1;if((int)_tmp7B0 != 4)goto _LL509;_LL508:
effect=({struct Cyc_List_List*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));
_tmp7BF[0]=({struct Cyc_Absyn_VarType_struct _tmp7C0;_tmp7C0.tag=1;_tmp7C0.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp7C0;});_tmp7BF;}));_tmp7BE->tl=effect;_tmp7BE;});
goto _LL4FC;_LL509: if(*((int*)_tmp7A6)!= 1)goto _LL50B;_tmp7B1=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7A6)->f1;_tmp7B2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7A6)->f1;_LL50A:*_tmp7B2=({struct Cyc_Core_Opt*_tmp7C1=_cycalloc(sizeof(*
_tmp7C1));_tmp7C1->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7C2=
_cycalloc(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7C3;
_tmp7C3.tag=2;_tmp7C3.f1=0;_tmp7C3.f2=(void*)((void*)0);_tmp7C3;});_tmp7C2;}));
_tmp7C1;});goto _LL50C;_LL50B:;_LL50C: effect=({struct Cyc_List_List*_tmp7C4=
_cycalloc(sizeof(*_tmp7C4));_tmp7C4->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp7C6;_tmp7C6.tag=21;_tmp7C6.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp7C7=_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Absyn_VarType_struct
_tmp7C8;_tmp7C8.tag=1;_tmp7C8.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp7C8;});
_tmp7C7;}));_tmp7C6;});_tmp7C5;}));_tmp7C4->tl=effect;_tmp7C4;});goto _LL4FC;
_LL4FC:;}}{struct Cyc_List_List*ts=_tmp772.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp7C9=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL50E: if((int)_tmp7C9 != 3)goto
_LL510;_LL50F: effect=({struct Cyc_List_List*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));
_tmp7CA->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp7CB=_cycalloc(
sizeof(*_tmp7CB));_tmp7CB[0]=({struct Cyc_Absyn_AccessEff_struct _tmp7CC;_tmp7CC.tag=
19;_tmp7CC.f1=(void*)((void*)ts->hd);_tmp7CC;});_tmp7CB;}));_tmp7CA->tl=effect;
_tmp7CA;});goto _LL50D;_LL510: if((int)_tmp7C9 != 4)goto _LL512;_LL511: effect=({
struct Cyc_List_List*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD->hd=(void*)((
void*)ts->hd);_tmp7CD->tl=effect;_tmp7CD;});goto _LL50D;_LL512: if((int)_tmp7C9 != 
5)goto _LL514;_LL513: goto _LL50D;_LL514:;_LL515: effect=({struct Cyc_List_List*
_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp7D0;_tmp7D0.tag=21;_tmp7D0.f1=(void*)((void*)ts->hd);_tmp7D0;});_tmp7CF;}));
_tmp7CE->tl=effect;_tmp7CE;});goto _LL50D;_LL50D:;}}*_tmp6B3=({struct Cyc_Core_Opt*
_tmp7D1=_cycalloc(sizeof(*_tmp7D1));_tmp7D1->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp7D3;_tmp7D3.tag=20;_tmp7D3.f1=effect;_tmp7D3;});_tmp7D2;}));_tmp7D1;});}if(*
_tmp6B1 != 0){struct Cyc_List_List*bs=*_tmp6B1;for(0;bs != 0;bs=bs->tl){void*
_tmp7D4=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp7D5;struct Cyc_Core_Opt**_tmp7D6;struct Cyc_Core_Opt*
_tmp7D7;struct Cyc_Core_Opt**_tmp7D8;void*_tmp7D9;struct Cyc_Core_Opt*_tmp7DA;
struct Cyc_Core_Opt**_tmp7DB;void*_tmp7DC;struct Cyc_Core_Opt*_tmp7DD;struct Cyc_Core_Opt**
_tmp7DE;void*_tmp7DF;void*_tmp7E0;_LL517: if(*((int*)_tmp7D4)!= 1)goto _LL519;
_tmp7D5=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7D4)->f1;_tmp7D6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp7D4)->f1;_LL518:({struct Cyc_String_pa_struct
_tmp7E3;_tmp7E3.tag=0;_tmp7E3.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp7E1[1]={& _tmp7E3};Cyc_Tcutil_warn(
loc,({const char*_tmp7E2="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp7E2,sizeof(char),_get_zero_arr_size(_tmp7E2,47));}),_tag_arr(
_tmp7E1,sizeof(void*),1));}});_tmp7D8=_tmp7D6;goto _LL51A;_LL519: if(*((int*)
_tmp7D4)!= 2)goto _LL51B;_tmp7D7=((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f1;
_tmp7D8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f1;
_tmp7D9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f2;if((int)_tmp7D9 != 
1)goto _LL51B;_LL51A: _tmp7DB=_tmp7D8;goto _LL51C;_LL51B: if(*((int*)_tmp7D4)!= 2)
goto _LL51D;_tmp7DA=((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f1;_tmp7DB=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f1;_tmp7DC=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f2;if((int)_tmp7DC != 0)goto
_LL51D;_LL51C:*_tmp7DB=({struct Cyc_Core_Opt*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));
_tmp7E4->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7E5=_cycalloc(
sizeof(*_tmp7E5));_tmp7E5[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7E6;_tmp7E6.tag=0;
_tmp7E6.f1=(void*)((void*)2);_tmp7E6;});_tmp7E5;}));_tmp7E4;});goto _LL516;_LL51D:
if(*((int*)_tmp7D4)!= 2)goto _LL51F;_tmp7DD=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7D4)->f1;_tmp7DE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7D4)->f1;_tmp7DF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7D4)->f2;
_LL51E:*_tmp7DE=({struct Cyc_Core_Opt*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));
_tmp7E8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp7E9;_tmp7E9.tag=0;_tmp7E9.f1=(void*)
_tmp7DF;_tmp7E9;});_tmp7E8;}));_tmp7E7;});goto _LL516;_LL51F: if(*((int*)_tmp7D4)
!= 0)goto _LL521;_tmp7E0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7D4)->f1;if((
int)_tmp7E0 != 1)goto _LL521;_LL520:({void*_tmp7EA[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7EB="functions can't abstract M types";_tag_arr(_tmp7EB,sizeof(
char),_get_zero_arr_size(_tmp7EB,33));}),_tag_arr(_tmp7EA,sizeof(void*),0));});
goto _LL516;_LL521:;_LL522: goto _LL516;_LL516:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp772.kind_env,*_tmp6B1);_tmp772.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp772.free_vars,*_tmp6B1);{struct Cyc_List_List*tvs=_tmp772.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp772.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL45D;}}_LL47C: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 
9)goto _LL47E;_tmp6BA=((struct Cyc_Absyn_TupleType_struct*)_tmp68C)->f1;_LL47D:
for(0;_tmp6BA != 0;_tmp6BA=_tmp6BA->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp6BA->hd)).f2);}goto _LL45D;_LL47E: if(
_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 11)goto _LL480;_tmp6BB=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp68C)->f1;_tmp6BC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp68C)->f2;_LL47F:{
struct _RegionHandle _tmp7ED=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp7ED;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp6BC != 0;_tmp6BC=_tmp6BC->tl){struct Cyc_Absyn_Aggrfield _tmp7EF;struct
_tagged_arr*_tmp7F0;struct Cyc_Absyn_Tqual _tmp7F1;void*_tmp7F2;struct Cyc_Absyn_Exp*
_tmp7F3;struct Cyc_List_List*_tmp7F4;struct Cyc_Absyn_Aggrfield*_tmp7EE=(struct Cyc_Absyn_Aggrfield*)
_tmp6BC->hd;_tmp7EF=*_tmp7EE;_tmp7F0=_tmp7EF.name;_tmp7F1=_tmp7EF.tq;_tmp7F2=(
void*)_tmp7EF.type;_tmp7F3=_tmp7EF.width;_tmp7F4=_tmp7EF.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7F0))({struct
Cyc_String_pa_struct _tmp7F7;_tmp7F7.tag=0;_tmp7F7.f1=(struct _tagged_arr)((struct
_tagged_arr)*_tmp7F0);{void*_tmp7F5[1]={& _tmp7F7};Cyc_Tcutil_terr(loc,({const
char*_tmp7F6="duplicate field %s";_tag_arr(_tmp7F6,sizeof(char),
_get_zero_arr_size(_tmp7F6,19));}),_tag_arr(_tmp7F5,sizeof(void*),1));}});if(Cyc_strcmp((
struct _tagged_arr)*_tmp7F0,({const char*_tmp7F8="";_tag_arr(_tmp7F8,sizeof(char),
_get_zero_arr_size(_tmp7F8,1));}))!= 0)prev_fields=({struct Cyc_List_List*_tmp7F9=
_region_malloc(aprev_rgn,sizeof(*_tmp7F9));_tmp7F9->hd=_tmp7F0;_tmp7F9->tl=
prev_fields;_tmp7F9;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp7F2);if(_tmp6BB == (void*)1?!Cyc_Tcutil_bits_only(_tmp7F2): 0)({struct Cyc_String_pa_struct
_tmp7FC;_tmp7FC.tag=0;_tmp7FC.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp7F0);{
void*_tmp7FA[1]={& _tmp7FC};Cyc_Tcutil_terr(loc,({const char*_tmp7FB="union member %s has a non-integral type";
_tag_arr(_tmp7FB,sizeof(char),_get_zero_arr_size(_tmp7FB,40));}),_tag_arr(
_tmp7FA,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp7F2,_tmp7F3,
_tmp7F0);Cyc_Tcutil_check_field_atts(loc,_tmp7F0,_tmp7F4);}};_pop_region(
aprev_rgn);}goto _LL45D;_LL480: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 10)goto
_LL482;_tmp6BD=((struct Cyc_Absyn_AggrType_struct*)_tmp68C)->f1;_tmp6BE=(void*)
_tmp6BD.aggr_info;_tmp6BF=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp68C)->f1).aggr_info;
_tmp6C0=_tmp6BD.targs;_tmp6C1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp68C)->f1).targs;_LL481:{void*_tmp7FD=*_tmp6BF;void*_tmp7FE;struct _tuple1*
_tmp7FF;struct Cyc_Absyn_Aggrdecl**_tmp800;struct Cyc_Absyn_Aggrdecl*_tmp801;
_LL524: if(*((int*)_tmp7FD)!= 0)goto _LL526;_tmp7FE=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7FD)->f1;_tmp7FF=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7FD)->f2;_LL525: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp802;_push_handler(& _tmp802);{
int _tmp804=0;if(setjmp(_tmp802.handler))_tmp804=1;if(!_tmp804){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp7FF);*_tmp6BF=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp805=
_cycalloc(sizeof(*_tmp805));_tmp805[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp806;
_tmp806.tag=1;_tmp806.f1=adp;_tmp806;});_tmp805;});;_pop_handler();}else{void*
_tmp803=(void*)_exn_thrown;void*_tmp808=_tmp803;_LL529: if(_tmp808 != Cyc_Dict_Absent)
goto _LL52B;_LL52A: {struct Cyc_Tcenv_Genv*_tmp809=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp80A=({struct Cyc_Absyn_Aggrdecl*_tmp810=_cycalloc(
sizeof(*_tmp810));_tmp810->kind=(void*)_tmp7FE;_tmp810->sc=(void*)((void*)3);
_tmp810->name=_tmp7FF;_tmp810->tvs=0;_tmp810->impl=0;_tmp810->attributes=0;
_tmp810;});Cyc_Tc_tcAggrdecl(te,_tmp809,loc,_tmp80A);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp7FF);*_tmp6BF=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp80B=
_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp80C;
_tmp80C.tag=1;_tmp80C.f1=adp;_tmp80C;});_tmp80B;});if(*_tmp6C1 != 0){({struct Cyc_String_pa_struct
_tmp80F;_tmp80F.tag=0;_tmp80F.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp7FF));{void*_tmp80D[1]={& _tmp80F};Cyc_Tcutil_terr(loc,({const char*_tmp80E="declare parameterized type %s before using";
_tag_arr(_tmp80E,sizeof(char),_get_zero_arr_size(_tmp80E,43));}),_tag_arr(
_tmp80D,sizeof(void*),1));}});return cvtenv;}goto _LL528;}_LL52B:;_LL52C:(void)
_throw(_tmp808);_LL528:;}}}_tmp801=*adp;goto _LL527;}_LL526: if(*((int*)_tmp7FD)!= 
1)goto _LL523;_tmp800=((struct Cyc_Absyn_KnownAggr_struct*)_tmp7FD)->f1;_tmp801=*
_tmp800;_LL527: {struct Cyc_List_List*tvs=_tmp801->tvs;struct Cyc_List_List*ts=*
_tmp6C1;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_String_pa_struct _tmp813;_tmp813.tag=
0;_tmp813.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp801->name));{void*_tmp811[1]={& _tmp813};Cyc_Tcutil_terr(loc,({const char*
_tmp812="too many parameters for type %s";_tag_arr(_tmp812,sizeof(char),
_get_zero_arr_size(_tmp812,32));}),_tag_arr(_tmp811,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814->hd=(void*)e;_tmp814->tl=
hidden_ts;_tmp814;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp6C1=Cyc_List_imp_append(*_tmp6C1,Cyc_List_imp_rev(hidden_ts));}}_LL523:;}
goto _LL45D;_LL482: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 16)goto _LL484;
_tmp6C2=((struct Cyc_Absyn_TypedefType_struct*)_tmp68C)->f1;_tmp6C3=((struct Cyc_Absyn_TypedefType_struct*)
_tmp68C)->f2;_tmp6C4=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp68C)->f2;_tmp6C5=((struct Cyc_Absyn_TypedefType_struct*)_tmp68C)->f3;_tmp6C6=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp68C)->f3;
_tmp6C7=((struct Cyc_Absyn_TypedefType_struct*)_tmp68C)->f4;_tmp6C8=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp68C)->f4;_LL483: {struct Cyc_List_List*
targs=*_tmp6C4;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp815;
_push_handler(& _tmp815);{int _tmp817=0;if(setjmp(_tmp815.handler))_tmp817=1;if(!
_tmp817){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp6C2);;_pop_handler();}else{
void*_tmp816=(void*)_exn_thrown;void*_tmp819=_tmp816;_LL52E: if(_tmp819 != Cyc_Dict_Absent)
goto _LL530;_LL52F:({struct Cyc_String_pa_struct _tmp81C;_tmp81C.tag=0;_tmp81C.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6C2));{void*
_tmp81A[1]={& _tmp81C};Cyc_Tcutil_terr(loc,({const char*_tmp81B="unbound typedef name %s";
_tag_arr(_tmp81B,sizeof(char),_get_zero_arr_size(_tmp81B,24));}),_tag_arr(
_tmp81A,sizeof(void*),1));}});return cvtenv;_LL530:;_LL531:(void)_throw(_tmp819);
_LL52D:;}}}*_tmp6C6=(struct Cyc_Absyn_Typedefdecl*)td;_tmp6C2[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp81D=_cycalloc(
sizeof(*_tmp81D));_tmp81D->hd=({struct _tuple8*_tmp81E=_cycalloc(sizeof(*_tmp81E));
_tmp81E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp81E->f2=(void*)ts->hd;_tmp81E;});
_tmp81D->tl=inst;_tmp81D;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp821;
_tmp821.tag=0;_tmp821.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6C2));{void*_tmp81F[1]={& _tmp821};Cyc_Tcutil_terr(loc,({const char*_tmp820="too many parameters for typedef %s";
_tag_arr(_tmp820,sizeof(char),_get_zero_arr_size(_tmp820,35));}),_tag_arr(
_tmp81F,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp822=_cycalloc(
sizeof(*_tmp822));_tmp822->hd=(void*)e;_tmp822->tl=hidden_ts;_tmp822;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->hd=({struct _tuple8*_tmp824=
_cycalloc(sizeof(*_tmp824));_tmp824->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp824->f2=
e;_tmp824;});_tmp823->tl=inst;_tmp823;});}*_tmp6C4=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp6C8=({void**_tmp825=
_cycalloc(sizeof(*_tmp825));_tmp825[0]=new_typ;_tmp825;});}goto _LL45D;}}_LL484:
if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 18)goto _LL486;_LL485: goto _LL487;
_LL486: if((int)_tmp68C != 2)goto _LL488;_LL487: goto _LL45D;_LL488: if(_tmp68C <= (
void*)3?1:*((int*)_tmp68C)!= 15)goto _LL48A;_tmp6C9=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp68C)->f1;_LL489: _tmp6CA=_tmp6C9;goto _LL48B;_LL48A: if(_tmp68C <= (void*)3?1:*((
int*)_tmp68C)!= 19)goto _LL48C;_tmp6CA=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp68C)->f1;_LL48B: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp6CA);goto _LL45D;_LL48C: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 21)goto
_LL48E;_tmp6CB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp68C)->f1;_LL48D:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp6CB);goto _LL45D;
_LL48E: if(_tmp68C <= (void*)3?1:*((int*)_tmp68C)!= 20)goto _LL45D;_tmp6CC=((struct
Cyc_Absyn_JoinEff_struct*)_tmp68C)->f1;_LL48F: for(0;_tmp6CC != 0;_tmp6CC=_tmp6CC->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp6CC->hd);}
goto _LL45D;_LL45D:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_String_pa_struct _tmp82A;_tmp82A.tag=0;_tmp82A.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct
_tmp829;_tmp829.tag=0;_tmp829.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp828;_tmp828.tag=0;
_tmp828.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp826[3]={& _tmp828,& _tmp829,& _tmp82A};Cyc_Tcutil_terr(loc,({const char*
_tmp827="type %s has kind %s but as used here needs kind %s";_tag_arr(_tmp827,
sizeof(char),_get_zero_arr_size(_tmp827,51));}),_tag_arr(_tmp826,sizeof(void*),3));}}}});
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp82B=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp834;_tmp834.kind_env=kind_env;_tmp834.free_vars=
0;_tmp834.free_evars=0;_tmp834.generalize_evars=generalize_evars;_tmp834.fn_result=
0;_tmp834;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp82B.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp82B.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp82B.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp82C=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp82D;struct Cyc_Core_Opt**_tmp82E;_LL533: if(_tmp82C <= (void*)3?1:*((int*)
_tmp82C)!= 0)goto _LL535;_tmp82D=((struct Cyc_Absyn_Evar_struct*)_tmp82C)->f4;
_tmp82E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp82C)->f4;
_LL534: if(*_tmp82E == 0)*_tmp82E=({struct Cyc_Core_Opt*_tmp82F=_cycalloc(sizeof(*
_tmp82F));_tmp82F->v=kind_env;_tmp82F;});else{struct Cyc_List_List*_tmp830=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp82E))->v;struct Cyc_List_List*
_tmp831=0;for(0;_tmp830 != 0;_tmp830=_tmp830->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp830->hd))_tmp831=({struct Cyc_List_List*_tmp832=_cycalloc(sizeof(*_tmp832));
_tmp832->hd=(struct Cyc_Absyn_Tvar*)_tmp830->hd;_tmp832->tl=_tmp831;_tmp832;});}*
_tmp82E=({struct Cyc_Core_Opt*_tmp833=_cycalloc(sizeof(*_tmp833));_tmp833->v=
_tmp831;_tmp833;});}goto _LL532;_LL535:;_LL536: goto _LL532;_LL532:;}}return _tmp82B;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp835=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp836=_tmp835.free_vars;struct Cyc_List_List*
_tmp837=_tmp835.free_evars;{struct Cyc_List_List*x=_tmp836;for(0;x != 0;x=x->tl){
void*_tmp838=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp839;struct Cyc_Core_Opt**_tmp83A;struct Cyc_Core_Opt*
_tmp83B;struct Cyc_Core_Opt**_tmp83C;void*_tmp83D;struct Cyc_Core_Opt*_tmp83E;
struct Cyc_Core_Opt**_tmp83F;void*_tmp840;struct Cyc_Core_Opt*_tmp841;struct Cyc_Core_Opt**
_tmp842;void*_tmp843;void*_tmp844;_LL538: if(*((int*)_tmp838)!= 1)goto _LL53A;
_tmp839=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp838)->f1;_tmp83A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp838)->f1;_LL539: _tmp83C=_tmp83A;goto
_LL53B;_LL53A: if(*((int*)_tmp838)!= 2)goto _LL53C;_tmp83B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp838)->f1;_tmp83C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp838)->f1;_tmp83D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp838)->f2;if((
int)_tmp83D != 1)goto _LL53C;_LL53B: _tmp83F=_tmp83C;goto _LL53D;_LL53C: if(*((int*)
_tmp838)!= 2)goto _LL53E;_tmp83E=((struct Cyc_Absyn_Less_kb_struct*)_tmp838)->f1;
_tmp83F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp838)->f1;
_tmp840=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp838)->f2;if((int)_tmp840 != 
0)goto _LL53E;_LL53D:*_tmp83F=({struct Cyc_Core_Opt*_tmp845=_cycalloc(sizeof(*
_tmp845));_tmp845->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp846=
_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp847;
_tmp847.tag=0;_tmp847.f1=(void*)((void*)2);_tmp847;});_tmp846;}));_tmp845;});
goto _LL537;_LL53E: if(*((int*)_tmp838)!= 2)goto _LL540;_tmp841=((struct Cyc_Absyn_Less_kb_struct*)
_tmp838)->f1;_tmp842=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp838)->f1;_tmp843=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp838)->f2;
_LL53F:*_tmp842=({struct Cyc_Core_Opt*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp849=_cycalloc(sizeof(*_tmp849));
_tmp849[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp84A;_tmp84A.tag=0;_tmp84A.f1=(void*)
_tmp843;_tmp84A;});_tmp849;}));_tmp848;});goto _LL537;_LL540: if(*((int*)_tmp838)
!= 0)goto _LL542;_tmp844=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp838)->f1;if((
int)_tmp844 != 1)goto _LL542;_LL541:({struct Cyc_String_pa_struct _tmp84D;_tmp84D.tag=
0;_tmp84D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp84B[1]={& _tmp84D};Cyc_Tcutil_terr(loc,({
const char*_tmp84C="type variable %s cannot have kind M";_tag_arr(_tmp84C,sizeof(
char),_get_zero_arr_size(_tmp84C,36));}),_tag_arr(_tmp84B,sizeof(void*),1));}});
goto _LL537;_LL542:;_LL543: goto _LL537;_LL537:;}}if(_tmp836 != 0?1: _tmp837 != 0){{
void*_tmp84E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp84F;struct Cyc_List_List*
_tmp850;struct Cyc_List_List**_tmp851;struct Cyc_Core_Opt*_tmp852;void*_tmp853;
struct Cyc_List_List*_tmp854;int _tmp855;struct Cyc_Absyn_VarargInfo*_tmp856;struct
Cyc_List_List*_tmp857;struct Cyc_List_List*_tmp858;_LL545: if(_tmp84E <= (void*)3?1:*((
int*)_tmp84E)!= 8)goto _LL547;_tmp84F=((struct Cyc_Absyn_FnType_struct*)_tmp84E)->f1;
_tmp850=_tmp84F.tvars;_tmp851=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp84E)->f1).tvars;_tmp852=_tmp84F.effect;_tmp853=(void*)_tmp84F.ret_typ;
_tmp854=_tmp84F.args;_tmp855=_tmp84F.c_varargs;_tmp856=_tmp84F.cyc_varargs;
_tmp857=_tmp84F.rgn_po;_tmp858=_tmp84F.attributes;_LL546: if(*_tmp851 == 0){*
_tmp851=_tmp836;_tmp836=0;}goto _LL544;_LL547:;_LL548: goto _LL544;_LL544:;}if(
_tmp836 != 0)({struct Cyc_String_pa_struct _tmp85B;_tmp85B.tag=0;_tmp85B.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp836->hd)->name);{
void*_tmp859[1]={& _tmp85B};Cyc_Tcutil_terr(loc,({const char*_tmp85A="unbound type variable %s";
_tag_arr(_tmp85A,sizeof(char),_get_zero_arr_size(_tmp85A,25));}),_tag_arr(
_tmp859,sizeof(void*),1));}});if(_tmp837 != 0)for(0;_tmp837 != 0;_tmp837=_tmp837->tl){
void*e=(void*)_tmp837->hd;void*_tmp85C=Cyc_Tcutil_typ_kind(e);_LL54A: if((int)
_tmp85C != 3)goto _LL54C;_LL54B: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp85D[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp85E="can't unify evar with heap!";_tag_arr(_tmp85E,sizeof(char),
_get_zero_arr_size(_tmp85E,28));}),_tag_arr(_tmp85D,sizeof(void*),0));});goto
_LL549;_LL54C: if((int)_tmp85C != 4)goto _LL54E;_LL54D: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp85F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp860="can't unify evar with {}!";_tag_arr(_tmp860,sizeof(char),
_get_zero_arr_size(_tmp860,26));}),_tag_arr(_tmp85F,sizeof(void*),0));});goto
_LL549;_LL54E:;_LL54F:({struct Cyc_String_pa_struct _tmp864;_tmp864.tag=0;_tmp864.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp863;_tmp863.tag=0;_tmp863.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp861[2]={& _tmp863,& _tmp864};Cyc_Tcutil_terr(loc,({const char*_tmp862="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp862,sizeof(char),_get_zero_arr_size(_tmp862,52));}),_tag_arr(
_tmp861,sizeof(void*),2));}}});goto _LL549;_LL549:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp865=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp866;struct Cyc_List_List*
_tmp867;struct Cyc_Core_Opt*_tmp868;_LL551: if(_tmp865 <= (void*)3?1:*((int*)
_tmp865)!= 8)goto _LL553;_tmp866=((struct Cyc_Absyn_FnType_struct*)_tmp865)->f1;
_tmp867=_tmp866.tvars;_tmp868=_tmp866.effect;_LL552: fd->tvs=_tmp867;fd->effect=
_tmp868;goto _LL550;_LL553:;_LL554:({void*_tmp869[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp86A="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp86A,sizeof(char),_get_zero_arr_size(_tmp86A,38));}),_tag_arr(
_tmp869,sizeof(void*),0));});_LL550:;}{struct _RegionHandle _tmp86B=_new_region("r");
struct _RegionHandle*r=& _tmp86B;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp86C="function declaration has repeated parameter";
_tag_arr(_tmp86C,sizeof(char),_get_zero_arr_size(_tmp86C,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp86D=_cycalloc(sizeof(*_tmp86D));_tmp86D->v=(
void*)t;_tmp86D;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp86E=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp86F=Cyc_Tcutil_remove_bound_tvars(
_tmp86E.free_vars,bound_tvars);struct Cyc_List_List*_tmp870=_tmp86E.free_evars;{
struct Cyc_List_List*fs=_tmp86F;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp871=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp875;_tmp875.tag=
0;_tmp875.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp874;_tmp874.tag=0;_tmp874.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp871);{void*_tmp872[2]={& _tmp874,& _tmp875};Cyc_Tcutil_terr(
loc,({const char*_tmp873="unbound type variable %s in type %s";_tag_arr(_tmp873,
sizeof(char),_get_zero_arr_size(_tmp873,36));}),_tag_arr(_tmp872,sizeof(void*),2));}}});}}
if(!allow_evars?_tmp870 != 0: 0)for(0;_tmp870 != 0;_tmp870=_tmp870->tl){void*e=(
void*)_tmp870->hd;void*_tmp876=Cyc_Tcutil_typ_kind(e);_LL556: if((int)_tmp876 != 3)
goto _LL558;_LL557: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp877[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp878="can't unify evar with heap!";
_tag_arr(_tmp878,sizeof(char),_get_zero_arr_size(_tmp878,28));}),_tag_arr(
_tmp877,sizeof(void*),0));});goto _LL555;_LL558: if((int)_tmp876 != 4)goto _LL55A;
_LL559: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp879[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp87A="can't unify evar with {}!";
_tag_arr(_tmp87A,sizeof(char),_get_zero_arr_size(_tmp87A,26));}),_tag_arr(
_tmp879,sizeof(void*),0));});goto _LL555;_LL55A:;_LL55B:({struct Cyc_String_pa_struct
_tmp87E;_tmp87E.tag=0;_tmp87E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp87D;_tmp87D.tag=0;_tmp87D.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp87B[2]={&
_tmp87D,& _tmp87E};Cyc_Tcutil_terr(loc,({const char*_tmp87C="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp87C,sizeof(char),_get_zero_arr_size(_tmp87C,52));}),_tag_arr(
_tmp87B,sizeof(void*),2));}}});goto _LL555;_LL555:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp882;_tmp882.tag=0;_tmp882.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp881;_tmp881.tag=0;_tmp881.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp87F[2]={& _tmp881,& _tmp882};
Cyc_Tcutil_terr(loc,({const char*_tmp880="%s: %s";_tag_arr(_tmp880,sizeof(char),
_get_zero_arr_size(_tmp880,7));}),_tag_arr(_tmp87F,sizeof(void*),2));}}});}}}
static struct _tagged_arr Cyc_Tcutil_strptr2string(struct _tagged_arr*s){return*s;}
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr msg){((void(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({
const char*_tmp883="duplicate type variable";_tag_arr(_tmp883,sizeof(char),
_get_zero_arr_size(_tmp883,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp884="";_tag_arr(_tmp884,sizeof(char),_get_zero_arr_size(_tmp884,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885->hd=({
struct _tuple18*_tmp886=_cycalloc(sizeof(*_tmp886));_tmp886->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp886->f2=0;_tmp886;});_tmp885->tl=fields;_tmp885;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp888;struct Cyc_List_List*
_tmp889;void*_tmp88A;struct _tuple19*_tmp887=(struct _tuple19*)des->hd;_tmp888=*
_tmp887;_tmp889=_tmp888.f1;_tmp88A=_tmp888.f2;if(_tmp889 == 0){struct Cyc_List_List*
_tmp88B=fields;for(0;_tmp88B != 0;_tmp88B=_tmp88B->tl){if(!(*((struct _tuple18*)
_tmp88B->hd)).f2){(*((struct _tuple18*)_tmp88B->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp88C=_cycalloc(sizeof(*_tmp88C));
_tmp88C->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp88D=_cycalloc(
sizeof(*_tmp88D));_tmp88D[0]=({struct Cyc_Absyn_FieldName_struct _tmp88E;_tmp88E.tag=
1;_tmp88E.f1=((*((struct _tuple18*)_tmp88B->hd)).f1)->name;_tmp88E;});_tmp88D;}));
_tmp88C->tl=0;_tmp88C;});ans=({struct Cyc_List_List*_tmp88F=_region_malloc(rgn,
sizeof(*_tmp88F));_tmp88F->hd=({struct _tuple20*_tmp890=_region_malloc(rgn,
sizeof(*_tmp890));_tmp890->f1=(*((struct _tuple18*)_tmp88B->hd)).f1;_tmp890->f2=
_tmp88A;_tmp890;});_tmp88F->tl=ans;_tmp88F;});break;}}if(_tmp88B == 0)({void*
_tmp891[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp892="too many arguments to struct";
_tag_arr(_tmp892,sizeof(char),_get_zero_arr_size(_tmp892,29));}),_tag_arr(
_tmp891,sizeof(void*),0));});}else{if(_tmp889->tl != 0)({void*_tmp893[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp894="multiple designators are not supported";_tag_arr(
_tmp894,sizeof(char),_get_zero_arr_size(_tmp894,39));}),_tag_arr(_tmp893,sizeof(
void*),0));});else{void*_tmp895=(void*)_tmp889->hd;struct _tagged_arr*_tmp896;
_LL55D: if(*((int*)_tmp895)!= 0)goto _LL55F;_LL55E:({void*_tmp897[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp898="array designator used in argument to struct";_tag_arr(
_tmp898,sizeof(char),_get_zero_arr_size(_tmp898,44));}),_tag_arr(_tmp897,sizeof(
void*),0));});goto _LL55C;_LL55F: if(*((int*)_tmp895)!= 1)goto _LL55C;_tmp896=((
struct Cyc_Absyn_FieldName_struct*)_tmp895)->f1;_LL560: {struct Cyc_List_List*
_tmp899=fields;for(0;_tmp899 != 0;_tmp899=_tmp899->tl){if(Cyc_strptrcmp(_tmp896,((*((
struct _tuple18*)_tmp899->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp899->hd)).f2)({
struct Cyc_String_pa_struct _tmp89C;_tmp89C.tag=0;_tmp89C.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp896);{void*_tmp89A[1]={& _tmp89C};Cyc_Tcutil_terr(loc,({
const char*_tmp89B="field %s has already been used as an argument";_tag_arr(
_tmp89B,sizeof(char),_get_zero_arr_size(_tmp89B,46));}),_tag_arr(_tmp89A,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp899->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp89D=_region_malloc(rgn,sizeof(*_tmp89D));_tmp89D->hd=({struct _tuple20*
_tmp89E=_region_malloc(rgn,sizeof(*_tmp89E));_tmp89E->f1=(*((struct _tuple18*)
_tmp899->hd)).f1;_tmp89E->f2=_tmp88A;_tmp89E;});_tmp89D->tl=ans;_tmp89D;});
break;}}if(_tmp899 == 0)({struct Cyc_String_pa_struct _tmp8A1;_tmp8A1.tag=0;_tmp8A1.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp896);{void*_tmp89F[1]={& _tmp8A1};Cyc_Tcutil_terr(
loc,({const char*_tmp8A0="bad field designator %s";_tag_arr(_tmp8A0,sizeof(char),
_get_zero_arr_size(_tmp8A0,24));}),_tag_arr(_tmp89F,sizeof(void*),1));}});goto
_LL55C;}_LL55C:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp8A2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp8A3="too few arguments to struct";
_tag_arr(_tmp8A3,sizeof(char),_get_zero_arr_size(_tmp8A3,28));}),_tag_arr(
_tmp8A2,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp8A4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp8A5;
void*_tmp8A6;struct Cyc_Absyn_PtrAtts _tmp8A7;struct Cyc_Absyn_Conref*_tmp8A8;
_LL562: if(_tmp8A4 <= (void*)3?1:*((int*)_tmp8A4)!= 4)goto _LL564;_tmp8A5=((struct
Cyc_Absyn_PointerType_struct*)_tmp8A4)->f1;_tmp8A6=(void*)_tmp8A5.elt_typ;
_tmp8A7=_tmp8A5.ptr_atts;_tmp8A8=_tmp8A7.bounds;_LL563: {struct Cyc_Absyn_Conref*
_tmp8A9=Cyc_Absyn_compress_conref(_tmp8A8);void*_tmp8AA=(void*)(Cyc_Absyn_compress_conref(
_tmp8A9))->v;void*_tmp8AB;_LL567: if(_tmp8AA <= (void*)1?1:*((int*)_tmp8AA)!= 0)
goto _LL569;_tmp8AB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp8AA)->f1;if((
int)_tmp8AB != 0)goto _LL569;_LL568:*elt_typ_dest=_tmp8A6;return 1;_LL569: if((int)
_tmp8AA != 0)goto _LL56B;_LL56A:(void*)(_tmp8A9->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp8AD;_tmp8AD.tag=0;_tmp8AD.f1=(void*)((void*)0);_tmp8AD;});_tmp8AC;})));*
elt_typ_dest=_tmp8A6;return 1;_LL56B:;_LL56C: return 0;_LL566:;}_LL564:;_LL565:
return 0;_LL561:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8AE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp8AF;void*_tmp8B0;
struct Cyc_Absyn_PtrAtts _tmp8B1;struct Cyc_Absyn_Conref*_tmp8B2;_LL56E: if(_tmp8AE
<= (void*)3?1:*((int*)_tmp8AE)!= 4)goto _LL570;_tmp8AF=((struct Cyc_Absyn_PointerType_struct*)
_tmp8AE)->f1;_tmp8B0=(void*)_tmp8AF.elt_typ;_tmp8B1=_tmp8AF.ptr_atts;_tmp8B2=
_tmp8B1.zero_term;_LL56F:*elt_typ_dest=_tmp8B0;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp8B2);_LL570:;_LL571: return 0;_LL56D:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp8F6;_tmp8F6.f1=0;
_tmp8F6.f2=(void*)2;_tmp8F6;});void*_tmp8B3=(void*)e->r;struct _tuple1*_tmp8B4;
void*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B6;struct _tagged_arr*_tmp8B7;struct Cyc_Absyn_Exp*
_tmp8B8;struct _tagged_arr*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8BA;struct Cyc_Absyn_Exp*
_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BC;_LL573: if(*((int*)_tmp8B3)!= 1)goto _LL575;
_tmp8B4=((struct Cyc_Absyn_Var_e_struct*)_tmp8B3)->f1;_tmp8B5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp8B3)->f2;_LL574: {void*_tmp8BD=_tmp8B5;struct Cyc_Absyn_Vardecl*_tmp8BE;
struct Cyc_Absyn_Vardecl*_tmp8BF;struct Cyc_Absyn_Vardecl*_tmp8C0;struct Cyc_Absyn_Vardecl*
_tmp8C1;_LL580: if((int)_tmp8BD != 0)goto _LL582;_LL581: goto _LL583;_LL582: if(
_tmp8BD <= (void*)1?1:*((int*)_tmp8BD)!= 1)goto _LL584;_LL583: return bogus_ans;
_LL584: if(_tmp8BD <= (void*)1?1:*((int*)_tmp8BD)!= 0)goto _LL586;_tmp8BE=((struct
Cyc_Absyn_Global_b_struct*)_tmp8BD)->f1;_LL585: {void*_tmp8C2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL58D: if(_tmp8C2 <= (void*)
3?1:*((int*)_tmp8C2)!= 7)goto _LL58F;_LL58E: return({struct _tuple10 _tmp8C3;_tmp8C3.f1=
1;_tmp8C3.f2=(void*)2;_tmp8C3;});_LL58F:;_LL590: return({struct _tuple10 _tmp8C4;
_tmp8C4.f1=(_tmp8BE->tq).q_const;_tmp8C4.f2=(void*)2;_tmp8C4;});_LL58C:;}_LL586:
if(_tmp8BD <= (void*)1?1:*((int*)_tmp8BD)!= 3)goto _LL588;_tmp8BF=((struct Cyc_Absyn_Local_b_struct*)
_tmp8BD)->f1;_LL587: {void*_tmp8C5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL592: if(_tmp8C5 <= (void*)3?1:*((int*)_tmp8C5)!= 7)
goto _LL594;_LL593: return({struct _tuple10 _tmp8C6;_tmp8C6.f1=1;_tmp8C6.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp8BF->rgn))->v;_tmp8C6;});_LL594:;_LL595:
_tmp8BF->escapes=1;return({struct _tuple10 _tmp8C7;_tmp8C7.f1=(_tmp8BF->tq).q_const;
_tmp8C7.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8BF->rgn))->v;_tmp8C7;});
_LL591:;}_LL588: if(_tmp8BD <= (void*)1?1:*((int*)_tmp8BD)!= 4)goto _LL58A;_tmp8C0=((
struct Cyc_Absyn_Pat_b_struct*)_tmp8BD)->f1;_LL589: _tmp8C1=_tmp8C0;goto _LL58B;
_LL58A: if(_tmp8BD <= (void*)1?1:*((int*)_tmp8BD)!= 2)goto _LL57F;_tmp8C1=((struct
Cyc_Absyn_Param_b_struct*)_tmp8BD)->f1;_LL58B: _tmp8C1->escapes=1;return({struct
_tuple10 _tmp8C8;_tmp8C8.f1=(_tmp8C1->tq).q_const;_tmp8C8.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8C1->rgn))->v;_tmp8C8;});_LL57F:;}_LL575: if(*((int*)_tmp8B3)!= 
21)goto _LL577;_tmp8B6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8B3)->f1;
_tmp8B7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8B3)->f2;_LL576: {void*
_tmp8C9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp8B6->topt))->v);
struct Cyc_List_List*_tmp8CA;struct Cyc_Absyn_AggrInfo _tmp8CB;void*_tmp8CC;struct
Cyc_Absyn_Aggrdecl**_tmp8CD;struct Cyc_Absyn_Aggrdecl*_tmp8CE;_LL597: if(_tmp8C9 <= (
void*)3?1:*((int*)_tmp8C9)!= 11)goto _LL599;_tmp8CA=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp8C9)->f2;_LL598: {struct Cyc_Absyn_Aggrfield*_tmp8CF=Cyc_Absyn_lookup_field(
_tmp8CA,_tmp8B7);if(_tmp8CF != 0?_tmp8CF->width != 0: 0)return({struct _tuple10
_tmp8D0;_tmp8D0.f1=(_tmp8CF->tq).q_const;_tmp8D0.f2=(Cyc_Tcutil_addressof_props(
te,_tmp8B6)).f2;_tmp8D0;});return bogus_ans;}_LL599: if(_tmp8C9 <= (void*)3?1:*((
int*)_tmp8C9)!= 10)goto _LL59B;_tmp8CB=((struct Cyc_Absyn_AggrType_struct*)_tmp8C9)->f1;
_tmp8CC=(void*)_tmp8CB.aggr_info;if(*((int*)_tmp8CC)!= 1)goto _LL59B;_tmp8CD=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp8CC)->f1;_tmp8CE=*_tmp8CD;_LL59A: {struct
Cyc_Absyn_Aggrfield*_tmp8D1=Cyc_Absyn_lookup_decl_field(_tmp8CE,_tmp8B7);if(
_tmp8D1 != 0?_tmp8D1->width != 0: 0)return({struct _tuple10 _tmp8D2;_tmp8D2.f1=(
_tmp8D1->tq).q_const;_tmp8D2.f2=(Cyc_Tcutil_addressof_props(te,_tmp8B6)).f2;
_tmp8D2;});return bogus_ans;}_LL59B:;_LL59C: return bogus_ans;_LL596:;}_LL577: if(*((
int*)_tmp8B3)!= 22)goto _LL579;_tmp8B8=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp8B3)->f1;_tmp8B9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp8B3)->f2;_LL578: {
void*_tmp8D3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp8B8->topt))->v);struct Cyc_Absyn_PtrInfo _tmp8D4;void*_tmp8D5;struct Cyc_Absyn_PtrAtts
_tmp8D6;void*_tmp8D7;_LL59E: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 4)goto
_LL5A0;_tmp8D4=((struct Cyc_Absyn_PointerType_struct*)_tmp8D3)->f1;_tmp8D5=(void*)
_tmp8D4.elt_typ;_tmp8D6=_tmp8D4.ptr_atts;_tmp8D7=(void*)_tmp8D6.rgn;_LL59F: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp8D8=Cyc_Tcutil_compress(_tmp8D5);
struct Cyc_List_List*_tmp8D9;struct Cyc_Absyn_AggrInfo _tmp8DA;void*_tmp8DB;struct
Cyc_Absyn_Aggrdecl**_tmp8DC;struct Cyc_Absyn_Aggrdecl*_tmp8DD;_LL5A3: if(_tmp8D8 <= (
void*)3?1:*((int*)_tmp8D8)!= 11)goto _LL5A5;_tmp8D9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp8D8)->f2;_LL5A4: finfo=Cyc_Absyn_lookup_field(_tmp8D9,_tmp8B9);goto _LL5A2;
_LL5A5: if(_tmp8D8 <= (void*)3?1:*((int*)_tmp8D8)!= 10)goto _LL5A7;_tmp8DA=((struct
Cyc_Absyn_AggrType_struct*)_tmp8D8)->f1;_tmp8DB=(void*)_tmp8DA.aggr_info;if(*((
int*)_tmp8DB)!= 1)goto _LL5A7;_tmp8DC=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8DB)->f1;
_tmp8DD=*_tmp8DC;_LL5A6: finfo=Cyc_Absyn_lookup_decl_field(_tmp8DD,_tmp8B9);goto
_LL5A2;_LL5A7:;_LL5A8: return bogus_ans;_LL5A2:;}if(finfo != 0?finfo->width != 0: 0)
return({struct _tuple10 _tmp8DE;_tmp8DE.f1=(finfo->tq).q_const;_tmp8DE.f2=_tmp8D7;
_tmp8DE;});return bogus_ans;}_LL5A0:;_LL5A1: return bogus_ans;_LL59D:;}_LL579: if(*((
int*)_tmp8B3)!= 20)goto _LL57B;_tmp8BA=((struct Cyc_Absyn_Deref_e_struct*)_tmp8B3)->f1;
_LL57A: {void*_tmp8DF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8BA->topt))->v);struct Cyc_Absyn_PtrInfo _tmp8E0;struct Cyc_Absyn_Tqual
_tmp8E1;struct Cyc_Absyn_PtrAtts _tmp8E2;void*_tmp8E3;_LL5AA: if(_tmp8DF <= (void*)3?
1:*((int*)_tmp8DF)!= 4)goto _LL5AC;_tmp8E0=((struct Cyc_Absyn_PointerType_struct*)
_tmp8DF)->f1;_tmp8E1=_tmp8E0.elt_tq;_tmp8E2=_tmp8E0.ptr_atts;_tmp8E3=(void*)
_tmp8E2.rgn;_LL5AB: return({struct _tuple10 _tmp8E4;_tmp8E4.f1=_tmp8E1.q_const;
_tmp8E4.f2=_tmp8E3;_tmp8E4;});_LL5AC:;_LL5AD: return bogus_ans;_LL5A9:;}_LL57B: if(*((
int*)_tmp8B3)!= 23)goto _LL57D;_tmp8BB=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp8B3)->f1;_tmp8BC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8B3)->f2;_LL57C: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp8BB->topt))->v);
void*_tmp8E5=t;struct Cyc_List_List*_tmp8E6;struct Cyc_Absyn_PtrInfo _tmp8E7;struct
Cyc_Absyn_Tqual _tmp8E8;struct Cyc_Absyn_PtrAtts _tmp8E9;void*_tmp8EA;struct Cyc_Absyn_ArrayInfo
_tmp8EB;struct Cyc_Absyn_Tqual _tmp8EC;_LL5AF: if(_tmp8E5 <= (void*)3?1:*((int*)
_tmp8E5)!= 9)goto _LL5B1;_tmp8E6=((struct Cyc_Absyn_TupleType_struct*)_tmp8E5)->f1;
_LL5B0: {unsigned int _tmp8EE;int _tmp8EF;struct _tuple7 _tmp8ED=Cyc_Evexp_eval_const_uint_exp(
_tmp8BC);_tmp8EE=_tmp8ED.f1;_tmp8EF=_tmp8ED.f2;if(!_tmp8EF)return bogus_ans;{
struct _tuple4*_tmp8F0=Cyc_Absyn_lookup_tuple_field(_tmp8E6,(int)_tmp8EE);if(
_tmp8F0 != 0)return({struct _tuple10 _tmp8F1;_tmp8F1.f1=((*_tmp8F0).f1).q_const;
_tmp8F1.f2=(Cyc_Tcutil_addressof_props(te,_tmp8BB)).f2;_tmp8F1;});return
bogus_ans;}}_LL5B1: if(_tmp8E5 <= (void*)3?1:*((int*)_tmp8E5)!= 4)goto _LL5B3;
_tmp8E7=((struct Cyc_Absyn_PointerType_struct*)_tmp8E5)->f1;_tmp8E8=_tmp8E7.elt_tq;
_tmp8E9=_tmp8E7.ptr_atts;_tmp8EA=(void*)_tmp8E9.rgn;_LL5B2: return({struct
_tuple10 _tmp8F2;_tmp8F2.f1=_tmp8E8.q_const;_tmp8F2.f2=_tmp8EA;_tmp8F2;});_LL5B3:
if(_tmp8E5 <= (void*)3?1:*((int*)_tmp8E5)!= 7)goto _LL5B5;_tmp8EB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp8E5)->f1;_tmp8EC=_tmp8EB.tq;_LL5B4: return({struct _tuple10 _tmp8F3;_tmp8F3.f1=
_tmp8EC.q_const;_tmp8F3.f2=(Cyc_Tcutil_addressof_props(te,_tmp8BB)).f2;_tmp8F3;});
_LL5B5:;_LL5B6: return bogus_ans;_LL5AE:;}_LL57D:;_LL57E:({void*_tmp8F4[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp8F5="unary & applied to non-lvalue";_tag_arr(_tmp8F5,
sizeof(char),_get_zero_arr_size(_tmp8F5,30));}),_tag_arr(_tmp8F4,sizeof(void*),0));});
return bogus_ans;_LL572:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,
void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp8F7=Cyc_Tcutil_compress(e_typ);struct
Cyc_Absyn_ArrayInfo _tmp8F8;void*_tmp8F9;struct Cyc_Absyn_Tqual _tmp8FA;struct Cyc_Absyn_Conref*
_tmp8FB;_LL5B8: if(_tmp8F7 <= (void*)3?1:*((int*)_tmp8F7)!= 7)goto _LL5BA;_tmp8F8=((
struct Cyc_Absyn_ArrayType_struct*)_tmp8F7)->f1;_tmp8F9=(void*)_tmp8F8.elt_type;
_tmp8FA=_tmp8F8.tq;_tmp8FB=_tmp8F8.zero_term;_LL5B9: {void*_tmp8FD;struct
_tuple10 _tmp8FC=Cyc_Tcutil_addressof_props(te,e);_tmp8FD=_tmp8FC.f2;return Cyc_Absyn_atb_typ(
_tmp8F9,_tmp8FD,_tmp8FA,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8FE=
_cycalloc(sizeof(*_tmp8FE));_tmp8FE[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8FF;
_tmp8FF.tag=0;_tmp8FF.f1=e;_tmp8FF;});_tmp8FE;}),_tmp8FB);}_LL5BA:;_LL5BB: return
e_typ;_LL5B7:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp900=(void*)b->v;void*_tmp901;void*_tmp902;struct Cyc_Absyn_Exp*_tmp903;void*
_tmp904;_LL5BD: if(_tmp900 <= (void*)1?1:*((int*)_tmp900)!= 0)goto _LL5BF;_tmp901=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp900)->f1;if((int)_tmp901 != 0)goto
_LL5BF;_LL5BE: return;_LL5BF: if(_tmp900 <= (void*)1?1:*((int*)_tmp900)!= 0)goto
_LL5C1;_tmp902=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp900)->f1;if(
_tmp902 <= (void*)1?1:*((int*)_tmp902)!= 0)goto _LL5C1;_tmp903=((struct Cyc_Absyn_Upper_b_struct*)
_tmp902)->f1;_LL5C0: {unsigned int _tmp906;int _tmp907;struct _tuple7 _tmp905=Cyc_Evexp_eval_const_uint_exp(
_tmp903);_tmp906=_tmp905.f1;_tmp907=_tmp905.f2;if(_tmp907?_tmp906 <= i: 0)({struct
Cyc_Int_pa_struct _tmp90B;_tmp90B.tag=1;_tmp90B.f1=(unsigned int)((int)i);{struct
Cyc_Int_pa_struct _tmp90A;_tmp90A.tag=1;_tmp90A.f1=(unsigned int)((int)_tmp906);{
void*_tmp908[2]={& _tmp90A,& _tmp90B};Cyc_Tcutil_terr(loc,({const char*_tmp909="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp909,sizeof(char),_get_zero_arr_size(_tmp909,39));}),_tag_arr(
_tmp908,sizeof(void*),2));}}});return;}_LL5C1: if(_tmp900 <= (void*)1?1:*((int*)
_tmp900)!= 0)goto _LL5C3;_tmp904=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp900)->f1;if(_tmp904 <= (void*)1?1:*((int*)_tmp904)!= 1)goto _LL5C3;_LL5C2:
return;_LL5C3: if((int)_tmp900 != 0)goto _LL5C5;_LL5C4:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp90C=_cycalloc(sizeof(*_tmp90C));_tmp90C[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp90D;_tmp90D.tag=0;_tmp90D.f1=(void*)((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp90F;_tmp90F.tag=0;_tmp90F.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp90F;});_tmp90E;}));_tmp90D;});_tmp90C;})));return;_LL5C5:;_LL5C6:({
void*_tmp910[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp911="check_bound -- bad compressed conref";_tag_arr(_tmp911,
sizeof(char),_get_zero_arr_size(_tmp911,37));}),_tag_arr(_tmp910,sizeof(void*),0));});
_LL5BC:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp912=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp913;struct Cyc_Absyn_Exp*_tmp914;_LL5C8: if(_tmp912 <= (void*)1?1:*((int*)
_tmp912)!= 0)goto _LL5CA;_tmp913=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp912)->f1;if(_tmp913 <= (void*)1?1:*((int*)_tmp913)!= 0)goto _LL5CA;_tmp914=((
struct Cyc_Absyn_Upper_b_struct*)_tmp913)->f1;_LL5C9: {unsigned int _tmp916;int
_tmp917;struct _tuple7 _tmp915=Cyc_Evexp_eval_const_uint_exp(_tmp914);_tmp916=
_tmp915.f1;_tmp917=_tmp915.f2;return _tmp917?_tmp916 == 1: 0;}_LL5CA:;_LL5CB: return
0;_LL5C7:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp918=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp919;void*_tmp91A;struct Cyc_Absyn_Conref*_tmp91B;
struct Cyc_List_List*_tmp91C;struct Cyc_Absyn_AggrInfo _tmp91D;void*_tmp91E;void*
_tmp91F;struct Cyc_Absyn_AggrInfo _tmp920;void*_tmp921;void*_tmp922;struct Cyc_Absyn_AggrInfo
_tmp923;void*_tmp924;struct Cyc_Absyn_Aggrdecl**_tmp925;struct Cyc_Absyn_Aggrdecl*
_tmp926;struct Cyc_List_List*_tmp927;struct Cyc_List_List*_tmp928;_LL5CD: if((int)
_tmp918 != 0)goto _LL5CF;_LL5CE: goto _LL5D0;_LL5CF: if(_tmp918 <= (void*)3?1:*((int*)
_tmp918)!= 5)goto _LL5D1;_LL5D0: goto _LL5D2;_LL5D1: if((int)_tmp918 != 1)goto _LL5D3;
_LL5D2: goto _LL5D4;_LL5D3: if(_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 6)goto _LL5D5;
_LL5D4: return 1;_LL5D5: if(_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 12)goto _LL5D7;
_LL5D6: goto _LL5D8;_LL5D7: if(_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 13)goto
_LL5D9;_LL5D8: return 0;_LL5D9: if(_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 7)goto
_LL5DB;_tmp919=((struct Cyc_Absyn_ArrayType_struct*)_tmp918)->f1;_tmp91A=(void*)
_tmp919.elt_type;_tmp91B=_tmp919.zero_term;_LL5DA: return !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp91B)?Cyc_Tcutil_bits_only(_tmp91A): 0;_LL5DB: if(
_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 9)goto _LL5DD;_tmp91C=((struct Cyc_Absyn_TupleType_struct*)
_tmp918)->f1;_LL5DC: for(0;_tmp91C != 0;_tmp91C=_tmp91C->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp91C->hd)).f2))return 0;}return 1;_LL5DD: if(_tmp918 <= (void*)3?1:*((
int*)_tmp918)!= 10)goto _LL5DF;_tmp91D=((struct Cyc_Absyn_AggrType_struct*)_tmp918)->f1;
_tmp91E=(void*)_tmp91D.aggr_info;if(*((int*)_tmp91E)!= 0)goto _LL5DF;_tmp91F=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp91E)->f1;if((int)_tmp91F != 1)
goto _LL5DF;_LL5DE: return 1;_LL5DF: if(_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 10)
goto _LL5E1;_tmp920=((struct Cyc_Absyn_AggrType_struct*)_tmp918)->f1;_tmp921=(void*)
_tmp920.aggr_info;if(*((int*)_tmp921)!= 0)goto _LL5E1;_tmp922=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp921)->f1;if((int)_tmp922 != 0)goto _LL5E1;_LL5E0: return 0;_LL5E1: if(_tmp918 <= (
void*)3?1:*((int*)_tmp918)!= 10)goto _LL5E3;_tmp923=((struct Cyc_Absyn_AggrType_struct*)
_tmp918)->f1;_tmp924=(void*)_tmp923.aggr_info;if(*((int*)_tmp924)!= 1)goto _LL5E3;
_tmp925=((struct Cyc_Absyn_KnownAggr_struct*)_tmp924)->f1;_tmp926=*_tmp925;
_tmp927=_tmp923.targs;_LL5E2: if((void*)_tmp926->kind == (void*)1)return 1;if(
_tmp926->impl == 0)return 0;{struct _RegionHandle _tmp929=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp929;_push_region(rgn);{struct Cyc_List_List*_tmp92A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp926->tvs,_tmp927);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp926->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp92A,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp92B=0;_npop_handler(0);
return _tmp92B;}}}{int _tmp92C=1;_npop_handler(0);return _tmp92C;}};_pop_region(rgn);}
_LL5E3: if(_tmp918 <= (void*)3?1:*((int*)_tmp918)!= 11)goto _LL5E5;_tmp928=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp918)->f2;_LL5E4: for(0;_tmp928 != 0;_tmp928=
_tmp928->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp928->hd)->type))return 0;}return 1;_LL5E5:;_LL5E6: return 0;_LL5CC:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp92D=(void*)e->r;
struct _tuple1*_tmp92E;void*_tmp92F;struct Cyc_Absyn_Exp*_tmp930;struct Cyc_Absyn_Exp*
_tmp931;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Exp*
_tmp934;struct Cyc_Absyn_Exp*_tmp935;struct Cyc_Absyn_Exp*_tmp936;void*_tmp937;
struct Cyc_Absyn_Exp*_tmp938;struct Cyc_Absyn_Exp*_tmp939;struct Cyc_Absyn_Exp*
_tmp93A;struct Cyc_Absyn_Exp*_tmp93B;struct Cyc_List_List*_tmp93C;struct Cyc_List_List*
_tmp93D;struct Cyc_List_List*_tmp93E;void*_tmp93F;struct Cyc_List_List*_tmp940;
struct Cyc_List_List*_tmp941;struct Cyc_List_List*_tmp942;_LL5E8: if(*((int*)
_tmp92D)!= 0)goto _LL5EA;_LL5E9: goto _LL5EB;_LL5EA: if(*((int*)_tmp92D)!= 16)goto
_LL5EC;_LL5EB: goto _LL5ED;_LL5EC: if(*((int*)_tmp92D)!= 17)goto _LL5EE;_LL5ED: goto
_LL5EF;_LL5EE: if(*((int*)_tmp92D)!= 18)goto _LL5F0;_LL5EF: goto _LL5F1;_LL5F0: if(*((
int*)_tmp92D)!= 19)goto _LL5F2;_LL5F1: goto _LL5F3;_LL5F2: if(*((int*)_tmp92D)!= 31)
goto _LL5F4;_LL5F3: goto _LL5F5;_LL5F4: if(*((int*)_tmp92D)!= 32)goto _LL5F6;_LL5F5:
return 1;_LL5F6: if(*((int*)_tmp92D)!= 1)goto _LL5F8;_tmp92E=((struct Cyc_Absyn_Var_e_struct*)
_tmp92D)->f1;_tmp92F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp92D)->f2;_LL5F7: {
void*_tmp943=_tmp92F;struct Cyc_Absyn_Vardecl*_tmp944;_LL615: if(_tmp943 <= (void*)
1?1:*((int*)_tmp943)!= 1)goto _LL617;_LL616: return 1;_LL617: if(_tmp943 <= (void*)1?
1:*((int*)_tmp943)!= 0)goto _LL619;_tmp944=((struct Cyc_Absyn_Global_b_struct*)
_tmp943)->f1;_LL618: {void*_tmp945=Cyc_Tcutil_compress((void*)_tmp944->type);
_LL61E: if(_tmp945 <= (void*)3?1:*((int*)_tmp945)!= 7)goto _LL620;_LL61F: goto _LL621;
_LL620: if(_tmp945 <= (void*)3?1:*((int*)_tmp945)!= 8)goto _LL622;_LL621: return 1;
_LL622:;_LL623: return var_okay;_LL61D:;}_LL619: if((int)_tmp943 != 0)goto _LL61B;
_LL61A: return 0;_LL61B:;_LL61C: return var_okay;_LL614:;}_LL5F8: if(*((int*)_tmp92D)
!= 6)goto _LL5FA;_tmp930=((struct Cyc_Absyn_Conditional_e_struct*)_tmp92D)->f1;
_tmp931=((struct Cyc_Absyn_Conditional_e_struct*)_tmp92D)->f2;_tmp932=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp92D)->f3;_LL5F9: return(Cyc_Tcutil_cnst_exp(te,0,_tmp930)?Cyc_Tcutil_cnst_exp(
te,0,_tmp931): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp932): 0;_LL5FA: if(*((int*)_tmp92D)
!= 7)goto _LL5FC;_tmp933=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp92D)->f1;_tmp934=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp92D)->f2;_LL5FB: return Cyc_Tcutil_cnst_exp(
te,0,_tmp933)?Cyc_Tcutil_cnst_exp(te,0,_tmp934): 0;_LL5FC: if(*((int*)_tmp92D)!= 
11)goto _LL5FE;_tmp935=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp92D)->f1;
_LL5FD: _tmp936=_tmp935;goto _LL5FF;_LL5FE: if(*((int*)_tmp92D)!= 12)goto _LL600;
_tmp936=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp92D)->f1;_LL5FF: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp936);_LL600: if(*((int*)_tmp92D)!= 13)goto _LL602;_tmp937=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp92D)->f1;_tmp938=((struct Cyc_Absyn_Cast_e_struct*)
_tmp92D)->f2;_LL601: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp938);_LL602: if(*((
int*)_tmp92D)!= 14)goto _LL604;_tmp939=((struct Cyc_Absyn_Address_e_struct*)
_tmp92D)->f1;_LL603: return Cyc_Tcutil_cnst_exp(te,1,_tmp939);_LL604: if(*((int*)
_tmp92D)!= 27)goto _LL606;_tmp93A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp92D)->f2;_tmp93B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp92D)->f3;
_LL605: return Cyc_Tcutil_cnst_exp(te,0,_tmp93A)?Cyc_Tcutil_cnst_exp(te,0,_tmp93B):
0;_LL606: if(*((int*)_tmp92D)!= 26)goto _LL608;_tmp93C=((struct Cyc_Absyn_Array_e_struct*)
_tmp92D)->f1;_LL607: _tmp93D=_tmp93C;goto _LL609;_LL608: if(*((int*)_tmp92D)!= 29)
goto _LL60A;_tmp93D=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp92D)->f2;_LL609:
_tmp93E=_tmp93D;goto _LL60B;_LL60A: if(*((int*)_tmp92D)!= 28)goto _LL60C;_tmp93E=((
struct Cyc_Absyn_Struct_e_struct*)_tmp92D)->f3;_LL60B: for(0;_tmp93E != 0;_tmp93E=
_tmp93E->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp93E->hd)).f2))
return 0;}return 1;_LL60C: if(*((int*)_tmp92D)!= 3)goto _LL60E;_tmp93F=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp92D)->f1;_tmp940=((struct Cyc_Absyn_Primop_e_struct*)
_tmp92D)->f2;_LL60D: _tmp941=_tmp940;goto _LL60F;_LL60E: if(*((int*)_tmp92D)!= 24)
goto _LL610;_tmp941=((struct Cyc_Absyn_Tuple_e_struct*)_tmp92D)->f1;_LL60F: _tmp942=
_tmp941;goto _LL611;_LL610: if(*((int*)_tmp92D)!= 30)goto _LL612;_tmp942=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp92D)->f1;_LL611: for(0;_tmp942 != 0;_tmp942=_tmp942->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp942->hd))return 0;}return 1;
_LL612:;_LL613: return 0;_LL5E7:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp946=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp947;
struct Cyc_Absyn_PtrAtts _tmp948;struct Cyc_Absyn_Conref*_tmp949;struct Cyc_Absyn_Conref*
_tmp94A;struct Cyc_Absyn_ArrayInfo _tmp94B;void*_tmp94C;struct Cyc_List_List*
_tmp94D;struct Cyc_Absyn_AggrInfo _tmp94E;void*_tmp94F;struct Cyc_List_List*_tmp950;
struct Cyc_List_List*_tmp951;_LL625: if((int)_tmp946 != 0)goto _LL627;_LL626: goto
_LL628;_LL627: if(_tmp946 <= (void*)3?1:*((int*)_tmp946)!= 5)goto _LL629;_LL628:
goto _LL62A;_LL629: if((int)_tmp946 != 1)goto _LL62B;_LL62A: goto _LL62C;_LL62B: if(
_tmp946 <= (void*)3?1:*((int*)_tmp946)!= 6)goto _LL62D;_LL62C: return 1;_LL62D: if(
_tmp946 <= (void*)3?1:*((int*)_tmp946)!= 4)goto _LL62F;_tmp947=((struct Cyc_Absyn_PointerType_struct*)
_tmp946)->f1;_tmp948=_tmp947.ptr_atts;_tmp949=_tmp948.nullable;_tmp94A=_tmp948.bounds;
_LL62E: {void*_tmp952=(void*)(Cyc_Absyn_compress_conref(_tmp94A))->v;void*
_tmp953;_LL63E: if(_tmp952 <= (void*)1?1:*((int*)_tmp952)!= 0)goto _LL640;_tmp953=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp952)->f1;if((int)_tmp953 != 0)goto
_LL640;_LL63F: return 1;_LL640: if(_tmp952 <= (void*)1?1:*((int*)_tmp952)!= 0)goto
_LL642;_LL641: {void*_tmp954=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp949))->v;int _tmp955;_LL645: if(_tmp954 <= (void*)
1?1:*((int*)_tmp954)!= 0)goto _LL647;_tmp955=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp954)->f1;_LL646: return _tmp955;_LL647:;_LL648: return 0;_LL644:;}_LL642:;_LL643:
return 0;_LL63D:;}_LL62F: if(_tmp946 <= (void*)3?1:*((int*)_tmp946)!= 7)goto _LL631;
_tmp94B=((struct Cyc_Absyn_ArrayType_struct*)_tmp946)->f1;_tmp94C=(void*)_tmp94B.elt_type;
_LL630: return Cyc_Tcutil_supports_default(_tmp94C);_LL631: if(_tmp946 <= (void*)3?1:*((
int*)_tmp946)!= 9)goto _LL633;_tmp94D=((struct Cyc_Absyn_TupleType_struct*)_tmp946)->f1;
_LL632: for(0;_tmp94D != 0;_tmp94D=_tmp94D->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple4*)_tmp94D->hd)).f2))return 0;}return 1;_LL633: if(_tmp946 <= (void*)3?1:*((
int*)_tmp946)!= 10)goto _LL635;_tmp94E=((struct Cyc_Absyn_AggrType_struct*)_tmp946)->f1;
_tmp94F=(void*)_tmp94E.aggr_info;_tmp950=_tmp94E.targs;_LL634: {struct Cyc_Absyn_Aggrdecl*
_tmp956=Cyc_Absyn_get_known_aggrdecl(_tmp94F);if(_tmp956->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp956->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmp956->tvs,_tmp950,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp956->impl))->fields);}_LL635: if(_tmp946 <= (void*)3?1:*((int*)
_tmp946)!= 11)goto _LL637;_tmp951=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp946)->f2;
_LL636: return Cyc_Tcutil_fields_support_default(0,0,_tmp951);_LL637: if(_tmp946 <= (
void*)3?1:*((int*)_tmp946)!= 13)goto _LL639;_LL638: goto _LL63A;_LL639: if(_tmp946 <= (
void*)3?1:*((int*)_tmp946)!= 12)goto _LL63B;_LL63A: return 1;_LL63B:;_LL63C: return 0;
_LL624:;}static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,
struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp957=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp957;_push_region(rgn);{struct Cyc_List_List*
_tmp958=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp958,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp959=0;_npop_handler(0);
return _tmp959;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){
void*_tmp95A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp95B;struct Cyc_Absyn_PtrAtts
_tmp95C;struct Cyc_Absyn_Conref*_tmp95D;struct Cyc_Absyn_Conref*_tmp95E;_LL64A: if(
_tmp95A <= (void*)3?1:*((int*)_tmp95A)!= 5)goto _LL64C;_LL64B: goto _LL64D;_LL64C:
if((int)_tmp95A != 1)goto _LL64E;_LL64D: goto _LL64F;_LL64E: if(_tmp95A <= (void*)3?1:*((
int*)_tmp95A)!= 6)goto _LL650;_LL64F: return 1;_LL650: if(_tmp95A <= (void*)3?1:*((
int*)_tmp95A)!= 4)goto _LL652;_tmp95B=((struct Cyc_Absyn_PointerType_struct*)
_tmp95A)->f1;_tmp95C=_tmp95B.ptr_atts;_tmp95D=_tmp95C.nullable;_tmp95E=_tmp95C.bounds;
_LL651: {void*_tmp95F=(void*)(Cyc_Absyn_compress_conref(_tmp95E))->v;void*
_tmp960;_LL655: if(_tmp95F <= (void*)1?1:*((int*)_tmp95F)!= 0)goto _LL657;_tmp960=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp95F)->f1;if((int)_tmp960 != 0)goto
_LL657;_LL656: return 0;_LL657: if(_tmp95F <= (void*)1?1:*((int*)_tmp95F)!= 0)goto
_LL659;_LL658: {void*_tmp961=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp95D))->v;int _tmp962;_LL65C: if(_tmp961 <= (void*)
1?1:*((int*)_tmp961)!= 0)goto _LL65E;_tmp962=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp961)->f1;_LL65D: return _tmp962;_LL65E:;_LL65F: return 0;_LL65B:;}_LL659:;_LL65A:
return 0;_LL654:;}_LL652:;_LL653: return 0;_LL649:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp963=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp964;void*
_tmp965;struct Cyc_Absyn_FnInfo _tmp966;struct Cyc_List_List*_tmp967;_LL661: if(
_tmp963 <= (void*)3?1:*((int*)_tmp963)!= 4)goto _LL663;_tmp964=((struct Cyc_Absyn_PointerType_struct*)
_tmp963)->f1;_tmp965=(void*)_tmp964.elt_typ;_LL662: return Cyc_Tcutil_is_noreturn(
_tmp965);_LL663: if(_tmp963 <= (void*)3?1:*((int*)_tmp963)!= 8)goto _LL665;_tmp966=((
struct Cyc_Absyn_FnType_struct*)_tmp963)->f1;_tmp967=_tmp966.attributes;_LL664:
for(0;_tmp967 != 0;_tmp967=_tmp967->tl){void*_tmp968=(void*)_tmp967->hd;_LL668:
if((int)_tmp968 != 3)goto _LL66A;_LL669: return 1;_LL66A:;_LL66B: continue;_LL667:;}
goto _LL660;_LL665:;_LL666: goto _LL660;_LL660:;}return 0;}
