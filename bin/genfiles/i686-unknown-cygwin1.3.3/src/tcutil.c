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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;}
;struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fflush(
struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,
struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct _tagged_arr
Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);struct _tuple5{
void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(void*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);int Cyc_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);
struct _tagged_arr Cyc_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);void*
Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
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
struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)s1;{void*
_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\t%s and ";_tag_arr(
_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,9));}),_tag_arr(_tmp0,sizeof(void*),
1));}});pos +=_get_arr_size(s1,sizeof(char))+ 5;if(pos >= 80){({void*_tmp3[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp4="\n\t";_tag_arr(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));}),_tag_arr(_tmp3,sizeof(void*),0));});pos=8;}({
struct Cyc_String_pa_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _tagged_arr)s2;{void*
_tmp5[1]={& _tmp7};Cyc_fprintf(Cyc_stderr,({const char*_tmp6="%s ";_tag_arr(_tmp6,
sizeof(char),_get_zero_arr_size(_tmp6,4));}),_tag_arr(_tmp5,sizeof(void*),1));}});
pos +=_get_arr_size(s2,sizeof(char))+ 1;if(pos >= 80){({void*_tmp8[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9="\n\t";_tag_arr(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,3));}),_tag_arr(_tmp8,sizeof(void*),0));});pos=8;}({
void*_tmpA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB="failed to unify. ";
_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,18));}),_tag_arr(_tmpA,
sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){if(pos >= 80){({void*_tmpC[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpD="\n\t";_tag_arr(_tmpD,sizeof(char),
_get_zero_arr_size(_tmpD,3));}),_tag_arr(_tmpC,sizeof(void*),0));});pos=8;}({
struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _tagged_arr)Cyc_Tcutil_failure_reason;{
void*_tmpE[1]={& _tmp10};Cyc_fprintf(Cyc_stderr,({const char*_tmpF="%s";_tag_arr(
_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,3));}),_tag_arr(_tmpE,sizeof(void*),
1));}});}({void*_tmp11[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";
_tag_arr(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,2));}),_tag_arr(_tmp11,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap){
struct _tagged_arr msg=(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);({
struct Cyc_String_pa_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _tagged_arr)msg;{
void*_tmp13[1]={& _tmp15};Cyc_fprintf(Cyc_stderr,({const char*_tmp14="Compiler Error (Tcutil::impos): %s\n";
_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,36));}),_tag_arr(_tmp13,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp16=_cycalloc(sizeof(*_tmp16));
_tmp16[0]=({struct Cyc_Core_Impossible_struct _tmp17;_tmp17.tag=Cyc_Core_Impossible;
_tmp17.f1=msg;_tmp17;});_tmp16;}));}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*((struct _tagged_arr*)tv->name);}void Cyc_Tcutil_print_tvars(
struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_String_pa_struct
_tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);{struct Cyc_String_pa_struct _tmp1A;
_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd);{void*_tmp18[2]={& _tmp1A,& _tmp1B};Cyc_fprintf(Cyc_stderr,({const char*
_tmp19="%s::%s ";_tag_arr(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,8));}),
_tag_arr(_tmp18,sizeof(void*),2));}}});}({void*_tmp1C[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp1D="\n";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,2));}),
_tag_arr(_tmp1C,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({
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
0;_tmp27.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct
_tmp26;_tmp26.tag=0;_tmp26.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
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
void*Cyc_Tcutil_compress(void*t){void*_tmp2A=t;struct Cyc_Core_Opt*_tmp2B;struct
Cyc_Core_Opt*_tmp2C;struct Cyc_Core_Opt*_tmp2D;struct Cyc_Core_Opt**_tmp2E;struct
Cyc_Core_Opt*_tmp2F;struct Cyc_Core_Opt**_tmp30;_LL1: if(_tmp2A <= (void*)3?1:*((
int*)_tmp2A)!= 0)goto _LL3;_tmp2B=((struct Cyc_Absyn_Evar_struct*)_tmp2A)->f2;if(
_tmp2B != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 
16)goto _LL5;_tmp2C=((struct Cyc_Absyn_TypedefType_struct*)_tmp2A)->f4;if(_tmp2C != 
0)goto _LL5;_LL4: return t;_LL5: if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 16)goto
_LL7;_tmp2D=((struct Cyc_Absyn_TypedefType_struct*)_tmp2A)->f4;_tmp2E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp2A)->f4;_LL6: _tmp30=_tmp2E;goto _LL8;_LL7:
if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 0)goto _LL9;_tmp2F=((struct Cyc_Absyn_Evar_struct*)
_tmp2A)->f2;_tmp30=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp2A)->f2;
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*((
struct Cyc_Core_Opt**)_tmp30)))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*((struct Cyc_Core_Opt**)_tmp30)))->v)*((struct Cyc_Core_Opt**)_tmp30)=({
struct Cyc_Core_Opt*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->v=(void*)t2;_tmp31;});
return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(void*t);static
struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return Cyc_List_map(
Cyc_Tcutil_copy_type,ts);}static struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(
struct Cyc_Absyn_Conref*c){void*_tmp32=(void*)c->v;void*_tmp33;struct Cyc_Absyn_Conref*
_tmp34;_LLC: if((int)_tmp32 != 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE:
if(_tmp32 <= (void*)1?1:*((int*)_tmp32)!= 0)goto _LL10;_tmp33=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp32)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp33);_LL10: if(_tmp32 <= (void*)1?1:*((
int*)_tmp32)!= 1)goto _LLB;_tmp34=((struct Cyc_Absyn_Forward_constr_struct*)_tmp32)->f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp34);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp35=Cyc_Absyn_compress_kb(kb);void*_tmp36;void*_tmp37;_LL13: if(*((
int*)_tmp35)!= 0)goto _LL15;_tmp36=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp35)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp38=_cycalloc(sizeof(*
_tmp38));_tmp38[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(
void*)_tmp36;_tmp39;});_tmp38;});_LL15: if(*((int*)_tmp35)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp3B;_tmp3B.tag=1;_tmp3B.f1=0;
_tmp3B;});_tmp3A;});_LL17: if(*((int*)_tmp35)!= 2)goto _LL12;_tmp37=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp3D;_tmp3D.tag=2;_tmp3D.f1=0;_tmp3D.f2=(void*)_tmp37;_tmp3D;});_tmp3C;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->name=tv->name;
_tmp3E->identity=0;_tmp3E->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp3E;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp40;struct Cyc_Core_Opt*_tmp41;struct Cyc_Absyn_Tqual _tmp42;void*_tmp43;
struct _tuple2*_tmp3F=arg;_tmp40=*_tmp3F;_tmp41=_tmp40.f1;_tmp42=_tmp40.f2;_tmp43=
_tmp40.f3;return({struct _tuple2*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->f1=
_tmp41;_tmp44->f2=_tmp42;_tmp44->f3=Cyc_Tcutil_copy_type(_tmp43);_tmp44;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp46;
struct Cyc_Absyn_Tqual _tmp47;void*_tmp48;struct _tuple4*_tmp45=arg;_tmp46=*_tmp45;
_tmp47=_tmp46.f1;_tmp48=_tmp46.f2;return({struct _tuple4*_tmp49=_cycalloc(sizeof(*
_tmp49));_tmp49->f1=_tmp47;_tmp49->f2=Cyc_Tcutil_copy_type(_tmp48);_tmp49;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->name=
f->name;_tmp4A->tq=f->tq;_tmp4A->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp4A->width=f->width;_tmp4A->attributes=f->attributes;_tmp4A;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp4C;void*_tmp4D;
void*_tmp4E;struct _tuple6*_tmp4B=x;_tmp4C=*_tmp4B;_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;
return({struct _tuple6*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=Cyc_Tcutil_copy_type(
_tmp4D);_tmp4F->f2=Cyc_Tcutil_copy_type(_tmp4E);_tmp4F;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->name=f->name;_tmp50->tag=f->tag;_tmp50->loc=
f->loc;_tmp50;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp51=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp52;struct Cyc_Absyn_TunionInfo _tmp53;void*_tmp54;
struct Cyc_List_List*_tmp55;void*_tmp56;struct Cyc_Absyn_TunionFieldInfo _tmp57;
void*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_Absyn_PtrInfo _tmp5A;void*_tmp5B;
void*_tmp5C;struct Cyc_Absyn_Conref*_tmp5D;struct Cyc_Absyn_Tqual _tmp5E;struct Cyc_Absyn_Conref*
_tmp5F;struct Cyc_Absyn_Conref*_tmp60;void*_tmp61;void*_tmp62;int _tmp63;struct Cyc_Absyn_ArrayInfo
_tmp64;void*_tmp65;struct Cyc_Absyn_Tqual _tmp66;struct Cyc_Absyn_Exp*_tmp67;struct
Cyc_Absyn_Conref*_tmp68;struct Cyc_Absyn_FnInfo _tmp69;struct Cyc_List_List*_tmp6A;
struct Cyc_Core_Opt*_tmp6B;void*_tmp6C;struct Cyc_List_List*_tmp6D;int _tmp6E;
struct Cyc_Absyn_VarargInfo*_tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List*
_tmp71;struct Cyc_List_List*_tmp72;struct Cyc_Absyn_AggrInfo _tmp73;void*_tmp74;
void*_tmp75;struct _tuple1*_tmp76;struct Cyc_List_List*_tmp77;struct Cyc_Absyn_AggrInfo
_tmp78;void*_tmp79;struct Cyc_Absyn_Aggrdecl**_tmp7A;struct Cyc_List_List*_tmp7B;
void*_tmp7C;struct Cyc_List_List*_tmp7D;struct _tuple1*_tmp7E;struct Cyc_Absyn_Enumdecl*
_tmp7F;struct Cyc_List_List*_tmp80;void*_tmp81;int _tmp82;void*_tmp83;void*_tmp84;
struct _tuple1*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_Typedefdecl*
_tmp87;void*_tmp88;struct Cyc_List_List*_tmp89;void*_tmp8A;_LL1A: if((int)_tmp51 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 0)
goto _LL1E;_LL1D: return t;_LL1E: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 1)goto
_LL20;_tmp52=((struct Cyc_Absyn_VarType_struct*)_tmp51)->f1;_LL1F: return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({
struct Cyc_Absyn_VarType_struct _tmp8C;_tmp8C.tag=1;_tmp8C.f1=Cyc_Tcutil_copy_tvar(
_tmp52);_tmp8C;});_tmp8B;});_LL20: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 2)
goto _LL22;_tmp53=((struct Cyc_Absyn_TunionType_struct*)_tmp51)->f1;_tmp54=(void*)
_tmp53.tunion_info;_tmp55=_tmp53.targs;_tmp56=(void*)_tmp53.rgn;_LL21: return(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D[0]=({struct Cyc_Absyn_TunionType_struct _tmp8E;_tmp8E.tag=2;_tmp8E.f1=({
struct Cyc_Absyn_TunionInfo _tmp8F;_tmp8F.tunion_info=(void*)_tmp54;_tmp8F.targs=
Cyc_Tcutil_copy_types(_tmp55);_tmp8F.rgn=(void*)Cyc_Tcutil_copy_type(_tmp56);
_tmp8F;});_tmp8E;});_tmp8D;});_LL22: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 3)
goto _LL24;_tmp57=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp51)->f1;_tmp58=(
void*)_tmp57.field_info;_tmp59=_tmp57.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp91;_tmp91.tag=3;_tmp91.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp92;_tmp92.field_info=(
void*)_tmp58;_tmp92.targs=Cyc_Tcutil_copy_types(_tmp59);_tmp92;});_tmp91;});
_tmp90;});_LL24: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 4)goto _LL26;_tmp5A=((
struct Cyc_Absyn_PointerType_struct*)_tmp51)->f1;_tmp5B=(void*)_tmp5A.elt_typ;
_tmp5C=(void*)_tmp5A.rgn_typ;_tmp5D=_tmp5A.nullable;_tmp5E=_tmp5A.tq;_tmp5F=
_tmp5A.bounds;_tmp60=_tmp5A.zero_term;_LL25: {void*_tmp93=Cyc_Tcutil_copy_type(
_tmp5B);void*_tmp94=Cyc_Tcutil_copy_type(_tmp5C);struct Cyc_Absyn_Conref*_tmp95=((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp5D);struct Cyc_Absyn_Tqual _tmp96=_tmp5E;struct Cyc_Absyn_Conref*_tmp97=Cyc_Tcutil_copy_conref(
_tmp5F);struct Cyc_Absyn_Conref*_tmp98=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp60);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9A;_tmp9A.tag=4;_tmp9A.f1=({struct Cyc_Absyn_PtrInfo _tmp9B;_tmp9B.elt_typ=(
void*)_tmp93;_tmp9B.rgn_typ=(void*)_tmp94;_tmp9B.nullable=_tmp95;_tmp9B.tq=
_tmp96;_tmp9B.bounds=_tmp97;_tmp9B.zero_term=_tmp98;_tmp9B;});_tmp9A;});_tmp99;});}
_LL26: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 5)goto _LL28;_tmp61=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp51)->f1;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp51)->f2;_LL27: return(void*)({struct Cyc_Absyn_IntType_struct*_tmp9C=_cycalloc(
sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_IntType_struct _tmp9D;_tmp9D.tag=5;
_tmp9D.f1=(void*)_tmp61;_tmp9D.f2=(void*)_tmp62;_tmp9D;});_tmp9C;});_LL28: if((
int)_tmp51 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp51 <= (void*)3?1:*((int*)
_tmp51)!= 6)goto _LL2C;_tmp63=((struct Cyc_Absyn_DoubleType_struct*)_tmp51)->f1;
_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmp9E=_cycalloc_atomic(
sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_DoubleType_struct _tmp9F;_tmp9F.tag=
6;_tmp9F.f1=_tmp63;_tmp9F;});_tmp9E;});_LL2C: if(_tmp51 <= (void*)3?1:*((int*)
_tmp51)!= 7)goto _LL2E;_tmp64=((struct Cyc_Absyn_ArrayType_struct*)_tmp51)->f1;
_tmp65=(void*)_tmp64.elt_type;_tmp66=_tmp64.tq;_tmp67=_tmp64.num_elts;_tmp68=
_tmp64.zero_term;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_ArrayType_struct _tmpA1;
_tmpA1.tag=7;_tmpA1.f1=({struct Cyc_Absyn_ArrayInfo _tmpA2;_tmpA2.elt_type=(void*)
Cyc_Tcutil_copy_type(_tmp65);_tmpA2.tq=_tmp66;_tmpA2.num_elts=_tmp67;_tmpA2.zero_term=((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp68);_tmpA2;});_tmpA1;});_tmpA0;});_LL2E: if(_tmp51 <= (void*)3?1:*((int*)
_tmp51)!= 8)goto _LL30;_tmp69=((struct Cyc_Absyn_FnType_struct*)_tmp51)->f1;_tmp6A=
_tmp69.tvars;_tmp6B=_tmp69.effect;_tmp6C=(void*)_tmp69.ret_typ;_tmp6D=_tmp69.args;
_tmp6E=_tmp69.c_varargs;_tmp6F=_tmp69.cyc_varargs;_tmp70=_tmp69.rgn_po;_tmp71=
_tmp69.attributes;_LL2F: {struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp6A);struct Cyc_Core_Opt*_tmpA4=_tmp6B == 0?0:({struct Cyc_Core_Opt*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp6B->v);_tmpAE;});void*_tmpA5=Cyc_Tcutil_copy_type(_tmp6C);struct Cyc_List_List*
_tmpA6=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp6D);int _tmpA7=_tmp6E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp6F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp6F);cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA8=_cycalloc(
sizeof(*_tmpA8));_tmpA8->name=cv->name;_tmpA8->tq=cv->tq;_tmpA8->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpA8->inject=cv->inject;_tmpA8;});}{struct Cyc_List_List*_tmpA9=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp70);struct Cyc_List_List*_tmpAA=_tmp71;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));
_tmpAB[0]=({struct Cyc_Absyn_FnType_struct _tmpAC;_tmpAC.tag=8;_tmpAC.f1=({struct
Cyc_Absyn_FnInfo _tmpAD;_tmpAD.tvars=_tmpA3;_tmpAD.effect=_tmpA4;_tmpAD.ret_typ=(
void*)_tmpA5;_tmpAD.args=_tmpA6;_tmpAD.c_varargs=_tmpA7;_tmpAD.cyc_varargs=
cyc_varargs2;_tmpAD.rgn_po=_tmpA9;_tmpAD.attributes=_tmpAA;_tmpAD;});_tmpAC;});
_tmpAB;});}}_LL30: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 9)goto _LL32;_tmp72=((
struct Cyc_Absyn_TupleType_struct*)_tmp51)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_TupleType_struct
_tmpB0;_tmpB0.tag=9;_tmpB0.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp72);
_tmpB0;});_tmpAF;});_LL32: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 10)goto _LL34;
_tmp73=((struct Cyc_Absyn_AggrType_struct*)_tmp51)->f1;_tmp74=(void*)_tmp73.aggr_info;
if(*((int*)_tmp74)!= 0)goto _LL34;_tmp75=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp74)->f1;_tmp76=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp74)->f2;_tmp77=
_tmp73.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_AggrType_struct _tmpB2;
_tmpB2.tag=10;_tmpB2.f1=({struct Cyc_Absyn_AggrInfo _tmpB3;_tmpB3.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));
_tmpB4[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(
void*)_tmp75;_tmpB5.f2=_tmp76;_tmpB5;});_tmpB4;}));_tmpB3.targs=Cyc_Tcutil_copy_types(
_tmp77);_tmpB3;});_tmpB2;});_tmpB1;});_LL34: if(_tmp51 <= (void*)3?1:*((int*)
_tmp51)!= 10)goto _LL36;_tmp78=((struct Cyc_Absyn_AggrType_struct*)_tmp51)->f1;
_tmp79=(void*)_tmp78.aggr_info;if(*((int*)_tmp79)!= 1)goto _LL36;_tmp7A=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp79)->f1;_tmp7B=_tmp78.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({
struct Cyc_Absyn_AggrType_struct _tmpB7;_tmpB7.tag=10;_tmpB7.f1=({struct Cyc_Absyn_AggrInfo
_tmpB8;_tmpB8.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpBA;
_tmpBA.tag=1;_tmpBA.f1=_tmp7A;_tmpBA;});_tmpB9;}));_tmpB8.targs=Cyc_Tcutil_copy_types(
_tmp7B);_tmpB8;});_tmpB7;});_tmpB6;});_LL36: if(_tmp51 <= (void*)3?1:*((int*)
_tmp51)!= 11)goto _LL38;_tmp7C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp51)->f1;_tmp7D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp51)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpBC;_tmpBC.tag=11;
_tmpBC.f1=(void*)_tmp7C;_tmpBC.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp7D);_tmpBC;});_tmpBB;});_LL38: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 12)
goto _LL3A;_tmp7E=((struct Cyc_Absyn_EnumType_struct*)_tmp51)->f1;_tmp7F=((struct
Cyc_Absyn_EnumType_struct*)_tmp51)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_EnumType_struct
_tmpBE;_tmpBE.tag=12;_tmpBE.f1=_tmp7E;_tmpBE.f2=_tmp7F;_tmpBE;});_tmpBD;});_LL3A:
if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 13)goto _LL3C;_tmp80=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp51)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpC0;
_tmpC0.tag=13;_tmpC0.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp80);_tmpC0;});_tmpBF;});_LL3C: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 14)
goto _LL3E;_tmp81=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp51)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));
_tmpC1[0]=({struct Cyc_Absyn_SizeofType_struct _tmpC2;_tmpC2.tag=14;_tmpC2.f1=(
void*)Cyc_Tcutil_copy_type(_tmp81);_tmpC2;});_tmpC1;});_LL3E: if(_tmp51 <= (void*)
3?1:*((int*)_tmp51)!= 18)goto _LL40;_tmp82=((struct Cyc_Absyn_TypeInt_struct*)
_tmp51)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpC3=
_cycalloc_atomic(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpC4;_tmpC4.tag=18;_tmpC4.f1=_tmp82;_tmpC4;});_tmpC3;});_LL40: if(_tmp51 <= (
void*)3?1:*((int*)_tmp51)!= 17)goto _LL42;_tmp83=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp51)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpC5=_cycalloc(
sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_TagType_struct _tmpC6;_tmpC6.tag=17;
_tmpC6.f1=(void*)Cyc_Tcutil_copy_type(_tmp83);_tmpC6;});_tmpC5;});_LL42: if(
_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 15)goto _LL44;_tmp84=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp51)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpC7=
_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpC8;_tmpC8.tag=15;_tmpC8.f1=(void*)Cyc_Tcutil_copy_type(_tmp84);_tmpC8;});
_tmpC7;});_LL44: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 16)goto _LL46;_tmp85=((
struct Cyc_Absyn_TypedefType_struct*)_tmp51)->f1;_tmp86=((struct Cyc_Absyn_TypedefType_struct*)
_tmp51)->f2;_tmp87=((struct Cyc_Absyn_TypedefType_struct*)_tmp51)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpC9=_cycalloc(sizeof(*
_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_TypedefType_struct _tmpCA;_tmpCA.tag=16;
_tmpCA.f1=_tmp85;_tmpCA.f2=Cyc_Tcutil_copy_types(_tmp86);_tmpCA.f3=_tmp87;_tmpCA.f4=
0;_tmpCA;});_tmpC9;});_LL46: if((int)_tmp51 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 19)goto _LL4A;_tmp88=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp51)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpCB=
_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_AccessEff_struct _tmpCC;
_tmpCC.tag=19;_tmpCC.f1=(void*)Cyc_Tcutil_copy_type(_tmp88);_tmpCC;});_tmpCB;});
_LL4A: if(_tmp51 <= (void*)3?1:*((int*)_tmp51)!= 20)goto _LL4C;_tmp89=((struct Cyc_Absyn_JoinEff_struct*)
_tmp51)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpCD=_cycalloc(
sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_JoinEff_struct _tmpCE;_tmpCE.tag=20;
_tmpCE.f1=Cyc_Tcutil_copy_types(_tmp89);_tmpCE;});_tmpCD;});_LL4C: if(_tmp51 <= (
void*)3?1:*((int*)_tmp51)!= 21)goto _LL19;_tmp8A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp51)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpCF=_cycalloc(
sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpD0;_tmpD0.tag=21;
_tmpD0.f1=(void*)Cyc_Tcutil_copy_type(_tmp8A);_tmpD0;});_tmpCF;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpD2=({struct _tuple6 _tmpD1;
_tmpD1.f1=k1;_tmpD1.f2=k2;_tmpD1;});void*_tmpD3;void*_tmpD4;void*_tmpD5;void*
_tmpD6;void*_tmpD7;void*_tmpD8;_LL4F: _tmpD3=_tmpD2.f1;if((int)_tmpD3 != 2)goto
_LL51;_tmpD4=_tmpD2.f2;if((int)_tmpD4 != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpD5=
_tmpD2.f1;if((int)_tmpD5 != 2)goto _LL53;_tmpD6=_tmpD2.f2;if((int)_tmpD6 != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpD7=_tmpD2.f1;if((int)_tmpD7 != 1)goto _LL55;_tmpD8=
_tmpD2.f2;if((int)_tmpD8 != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpD9=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpDA;void*_tmpDB;_LL58: if(*((int*)_tmpD9)!= 0)goto _LL5A;
_tmpDA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD9)->f1;_LL59: return _tmpDA;
_LL5A: if(*((int*)_tmpD9)!= 2)goto _LL5C;_tmpDB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD9)->f2;_LL5B: return _tmpDB;_LL5C:;_LL5D:({void*_tmpDC[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpDD="kind not suitably constrained!";
_tag_arr(_tmpDD,sizeof(char),_get_zero_arr_size(_tmpDD,31));}),_tag_arr(_tmpDC,
sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpDE=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_Absyn_Tvar*
_tmpE1;void*_tmpE2;struct Cyc_Absyn_TunionFieldInfo _tmpE3;void*_tmpE4;struct Cyc_Absyn_Tunionfield*
_tmpE5;struct Cyc_Absyn_TunionFieldInfo _tmpE6;void*_tmpE7;struct Cyc_Absyn_Enumdecl*
_tmpE8;struct Cyc_Absyn_AggrInfo _tmpE9;void*_tmpEA;struct Cyc_Absyn_AggrInfo _tmpEB;
void*_tmpEC;struct Cyc_Absyn_Aggrdecl**_tmpED;struct Cyc_Absyn_Aggrdecl*_tmpEE;
struct Cyc_Absyn_Aggrdecl _tmpEF;struct Cyc_Absyn_AggrdeclImpl*_tmpF0;struct Cyc_Absyn_Enumdecl*
_tmpF1;struct Cyc_Absyn_PtrInfo _tmpF2;struct Cyc_Absyn_Typedefdecl*_tmpF3;_LL5F:
if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 0)goto _LL61;_tmpDF=((struct Cyc_Absyn_Evar_struct*)
_tmpDE)->f1;_tmpE0=((struct Cyc_Absyn_Evar_struct*)_tmpDE)->f2;_LL60: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpDF))->v;_LL61: if(_tmpDE <= (void*)3?1:*((int*)
_tmpDE)!= 1)goto _LL63;_tmpE1=((struct Cyc_Absyn_VarType_struct*)_tmpDE)->f1;_LL62:
return Cyc_Tcutil_tvar_kind(_tmpE1);_LL63: if((int)_tmpDE != 0)goto _LL65;_LL64:
return(void*)1;_LL65: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 5)goto _LL67;_tmpE2=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpDE)->f2;_LL66: return _tmpE2 == (void*)
2?(void*)2:(void*)1;_LL67: if((int)_tmpDE != 1)goto _LL69;_LL68: goto _LL6A;_LL69: if(
_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 6)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if(
_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 8)goto _LL6D;_LL6C: return(void*)1;_LL6D: if(
_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 15)goto _LL6F;_LL6E: return(void*)2;_LL6F:
if((int)_tmpDE != 2)goto _LL71;_LL70: return(void*)3;_LL71: if(_tmpDE <= (void*)3?1:*((
int*)_tmpDE)!= 2)goto _LL73;_LL72: return(void*)2;_LL73: if(_tmpDE <= (void*)3?1:*((
int*)_tmpDE)!= 3)goto _LL75;_tmpE3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpDE)->f1;_tmpE4=(void*)_tmpE3.field_info;if(*((int*)_tmpE4)!= 1)goto _LL75;
_tmpE5=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpE4)->f2;_LL74: if(_tmpE5->typs
== 0)return(void*)2;else{return(void*)1;}_LL75: if(_tmpDE <= (void*)3?1:*((int*)
_tmpDE)!= 3)goto _LL77;_tmpE6=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpDE)->f1;
_tmpE7=(void*)_tmpE6.field_info;if(*((int*)_tmpE7)!= 0)goto _LL77;_LL76:({void*
_tmpF4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmpF5="typ_kind: Unresolved TunionFieldType";_tag_arr(_tmpF5,sizeof(
char),_get_zero_arr_size(_tmpF5,37));}),_tag_arr(_tmpF4,sizeof(void*),0));});
_LL77: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 12)goto _LL79;_tmpE8=((struct Cyc_Absyn_EnumType_struct*)
_tmpDE)->f2;if(_tmpE8 != 0)goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpDE <= (void*)3?1:*((
int*)_tmpDE)!= 10)goto _LL7B;_tmpE9=((struct Cyc_Absyn_AggrType_struct*)_tmpDE)->f1;
_tmpEA=(void*)_tmpE9.aggr_info;if(*((int*)_tmpEA)!= 0)goto _LL7B;_LL7A: return(
void*)0;_LL7B: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 10)goto _LL7D;_tmpEB=((
struct Cyc_Absyn_AggrType_struct*)_tmpDE)->f1;_tmpEC=(void*)_tmpEB.aggr_info;if(*((
int*)_tmpEC)!= 1)goto _LL7D;_tmpED=((struct Cyc_Absyn_KnownAggr_struct*)_tmpEC)->f1;
_tmpEE=*_tmpED;_tmpEF=*_tmpEE;_tmpF0=_tmpEF.impl;_LL7C: return _tmpF0 == 0?(void*)0:(
void*)1;_LL7D: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 11)goto _LL7F;_LL7E: goto
_LL80;_LL7F: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 13)goto _LL81;_LL80: return(
void*)1;_LL81: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 12)goto _LL83;_tmpF1=((
struct Cyc_Absyn_EnumType_struct*)_tmpDE)->f2;_LL82: if(_tmpF1->fields == 0)return(
void*)0;else{return(void*)1;}_LL83: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 4)
goto _LL85;_tmpF2=((struct Cyc_Absyn_PointerType_struct*)_tmpDE)->f1;_LL84: {void*
_tmpF6=(void*)(Cyc_Absyn_compress_conref(_tmpF2.bounds))->v;void*_tmpF7;void*
_tmpF8;_LL98: if(_tmpF6 <= (void*)1?1:*((int*)_tmpF6)!= 0)goto _LL9A;_tmpF7=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpF6)->f1;if((int)_tmpF7 != 0)goto _LL9A;_LL99:
return(void*)1;_LL9A: if(_tmpF6 <= (void*)1?1:*((int*)_tmpF6)!= 0)goto _LL9C;_tmpF8=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpF6)->f1;if(_tmpF8 <= (void*)1?1:*((
int*)_tmpF8)!= 0)goto _LL9C;_LL9B: return(void*)2;_LL9C: if((int)_tmpF6 != 0)goto
_LL9E;_LL9D: return(void*)1;_LL9E: if(_tmpF6 <= (void*)1?1:*((int*)_tmpF6)!= 1)goto
_LL97;_LL9F:({void*_tmpF9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmpFA="typ_kind: forward constraint in ptr bounds";
_tag_arr(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,43));}),_tag_arr(_tmpF9,
sizeof(void*),0));});_LL97:;}_LL85: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 14)
goto _LL87;_LL86: return(void*)2;_LL87: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 18)
goto _LL89;_LL88: return(void*)5;_LL89: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 17)
goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 7)
goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 9)goto
_LL8F;_LL8E: return(void*)1;_LL8F: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 16)
goto _LL91;_tmpF3=((struct Cyc_Absyn_TypedefType_struct*)_tmpDE)->f3;_LL90: if(
_tmpF3 == 0?1: _tmpF3->kind == 0)({struct Cyc_String_pa_struct _tmpFD;_tmpFD.tag=0;
_tmpFD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpFB[1]={& _tmpFD};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmpFC="typ_kind: typedef found: %s";_tag_arr(_tmpFC,sizeof(char),
_get_zero_arr_size(_tmpFC,28));}),_tag_arr(_tmpFB,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF3->kind))->v;_LL91: if(_tmpDE <= (
void*)3?1:*((int*)_tmpDE)!= 19)goto _LL93;_LL92: goto _LL94;_LL93: if(_tmpDE <= (void*)
3?1:*((int*)_tmpDE)!= 20)goto _LL95;_LL94: goto _LL96;_LL95: if(_tmpDE <= (void*)3?1:*((
int*)_tmpDE)!= 21)goto _LL5E;_LL96: return(void*)4;_LL5E:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmpFE;_push_handler(& _tmpFE);{int _tmp100=0;
if(setjmp(_tmpFE.handler))_tmp100=1;if(!_tmp100){Cyc_Tcutil_unify_it(t1,t2);{int
_tmp101=1;_npop_handler(0);return _tmp101;};_pop_handler();}else{void*_tmpFF=(
void*)_exn_thrown;void*_tmp103=_tmpFF;_LLA1: if(_tmp103 != Cyc_Tcutil_Unify)goto
_LLA3;_LLA2: return 0;_LLA3:;_LLA4:(void)_throw(_tmp103);_LLA0:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){void*_tmp104=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp105;
struct Cyc_Core_Opt*_tmp106;struct Cyc_Core_Opt*_tmp107;struct Cyc_Core_Opt**
_tmp108;struct Cyc_Absyn_PtrInfo _tmp109;struct Cyc_Absyn_ArrayInfo _tmp10A;void*
_tmp10B;struct Cyc_Absyn_FnInfo _tmp10C;struct Cyc_List_List*_tmp10D;struct Cyc_Core_Opt*
_tmp10E;void*_tmp10F;struct Cyc_List_List*_tmp110;int _tmp111;struct Cyc_Absyn_VarargInfo*
_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*
_tmp115;struct Cyc_Absyn_TunionInfo _tmp116;struct Cyc_List_List*_tmp117;void*
_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_Core_Opt*_tmp11A;struct Cyc_Absyn_TunionFieldInfo
_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_AggrInfo _tmp11D;struct Cyc_List_List*
_tmp11E;struct Cyc_List_List*_tmp11F;void*_tmp120;void*_tmp121;void*_tmp122;void*
_tmp123;struct Cyc_List_List*_tmp124;_LLA6: if(_tmp104 <= (void*)3?1:*((int*)
_tmp104)!= 1)goto _LLA8;_tmp105=((struct Cyc_Absyn_VarType_struct*)_tmp104)->f1;
_LLA7: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp105)){Cyc_Tcutil_failure_reason=({const char*_tmp125="(type variable would escape scope)";
_tag_arr(_tmp125,sizeof(char),_get_zero_arr_size(_tmp125,35));});(int)_throw((
void*)Cyc_Tcutil_Unify);}goto _LLA5;_LLA8: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)
!= 0)goto _LLAA;_tmp106=((struct Cyc_Absyn_Evar_struct*)_tmp104)->f2;_tmp107=((
struct Cyc_Absyn_Evar_struct*)_tmp104)->f4;_tmp108=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp104)->f4;_LLA9: if(t == evar){Cyc_Tcutil_failure_reason=({
const char*_tmp126="(occurs check)";_tag_arr(_tmp126,sizeof(char),
_get_zero_arr_size(_tmp126,15));});(int)_throw((void*)Cyc_Tcutil_Unify);}else{
if(_tmp106 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp106->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*((
struct Cyc_Core_Opt**)_tmp108)))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*
compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp127=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*((struct Cyc_Core_Opt**)
_tmp108)))->v;for(0;s != 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp127=({struct Cyc_List_List*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=(struct Cyc_Absyn_Tvar*)s->hd;
_tmp128->tl=_tmp127;_tmp128;});}}*((struct Cyc_Core_Opt**)_tmp108)=({struct Cyc_Core_Opt*
_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->v=_tmp127;_tmp129;});}}}goto _LLA5;
_LLAA: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 4)goto _LLAC;_tmp109=((struct Cyc_Absyn_PointerType_struct*)
_tmp104)->f1;_LLAB: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp109.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp109.rgn_typ);goto _LLA5;_LLAC: if(_tmp104 <= (void*)3?1:*((
int*)_tmp104)!= 7)goto _LLAE;_tmp10A=((struct Cyc_Absyn_ArrayType_struct*)_tmp104)->f1;
_tmp10B=(void*)_tmp10A.elt_type;_LLAD: Cyc_Tcutil_occurs(evar,r,env,_tmp10B);goto
_LLA5;_LLAE: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 8)goto _LLB0;_tmp10C=((
struct Cyc_Absyn_FnType_struct*)_tmp104)->f1;_tmp10D=_tmp10C.tvars;_tmp10E=
_tmp10C.effect;_tmp10F=(void*)_tmp10C.ret_typ;_tmp110=_tmp10C.args;_tmp111=
_tmp10C.c_varargs;_tmp112=_tmp10C.cyc_varargs;_tmp113=_tmp10C.rgn_po;_tmp114=
_tmp10C.attributes;_LLAF: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp10D,env);
if(_tmp10E != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp10E->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp10F);for(0;_tmp110 != 0;_tmp110=_tmp110->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)((struct _tuple2*)_tmp110->hd))).f3);}if(_tmp112 != 
0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp112->type);for(0;_tmp113 != 0;_tmp113=
_tmp113->tl){struct _tuple6 _tmp12B;void*_tmp12C;void*_tmp12D;struct _tuple6*
_tmp12A=(struct _tuple6*)_tmp113->hd;_tmp12B=*_tmp12A;_tmp12C=_tmp12B.f1;_tmp12D=
_tmp12B.f2;Cyc_Tcutil_occurs(evar,r,env,_tmp12C);Cyc_Tcutil_occurs(evar,r,env,
_tmp12D);}goto _LLA5;_LLB0: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 9)goto _LLB2;
_tmp115=((struct Cyc_Absyn_TupleType_struct*)_tmp104)->f1;_LLB1: for(0;_tmp115 != 0;
_tmp115=_tmp115->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple4*)((struct
_tuple4*)_tmp115->hd))).f2);}goto _LLA5;_LLB2: if(_tmp104 <= (void*)3?1:*((int*)
_tmp104)!= 2)goto _LLB4;_tmp116=((struct Cyc_Absyn_TunionType_struct*)_tmp104)->f1;
_tmp117=_tmp116.targs;_tmp118=(void*)_tmp116.rgn;_LLB3: Cyc_Tcutil_occurs(evar,r,
env,_tmp118);Cyc_Tcutil_occurslist(evar,r,env,_tmp117);goto _LLA5;_LLB4: if(
_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 16)goto _LLB6;_tmp119=((struct Cyc_Absyn_TypedefType_struct*)
_tmp104)->f2;_tmp11A=((struct Cyc_Absyn_TypedefType_struct*)_tmp104)->f4;_LLB5:
_tmp11C=_tmp119;goto _LLB7;_LLB6: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 3)
goto _LLB8;_tmp11B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp104)->f1;_tmp11C=
_tmp11B.targs;_LLB7: _tmp11E=_tmp11C;goto _LLB9;_LLB8: if(_tmp104 <= (void*)3?1:*((
int*)_tmp104)!= 10)goto _LLBA;_tmp11D=((struct Cyc_Absyn_AggrType_struct*)_tmp104)->f1;
_tmp11E=_tmp11D.targs;_LLB9: Cyc_Tcutil_occurslist(evar,r,env,_tmp11E);goto _LLA5;
_LLBA: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 11)goto _LLBC;_tmp11F=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp104)->f2;_LLBB: for(0;_tmp11F != 0;_tmp11F=
_tmp11F->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp11F->hd)->type);}goto _LLA5;_LLBC: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 
15)goto _LLBE;_tmp120=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp104)->f1;
_LLBD: _tmp121=_tmp120;goto _LLBF;_LLBE: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 
14)goto _LLC0;_tmp121=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp104)->f1;
_LLBF: _tmp122=_tmp121;goto _LLC1;_LLC0: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 
19)goto _LLC2;_tmp122=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp104)->f1;
_LLC1: _tmp123=_tmp122;goto _LLC3;_LLC2: if(_tmp104 <= (void*)3?1:*((int*)_tmp104)!= 
21)goto _LLC4;_tmp123=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp104)->f1;_LLC3:
Cyc_Tcutil_occurs(evar,r,env,_tmp123);goto _LLA5;_LLC4: if(_tmp104 <= (void*)3?1:*((
int*)_tmp104)!= 20)goto _LLC6;_tmp124=((struct Cyc_Absyn_JoinEff_struct*)_tmp104)->f1;
_LLC5: Cyc_Tcutil_occurslist(evar,r,env,_tmp124);goto _LLA5;_LLC6:;_LLC7: goto _LLA5;
_LLA5:;}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){if((tq1.q_const != tq2.q_const?1: tq1.q_volatile != tq2.q_volatile)?1: tq1.q_restrict
!= tq2.q_restrict){Cyc_Tcutil_failure_reason=({const char*_tmp12E="(qualifiers don't match)";
_tag_arr(_tmp12E,sizeof(char),_get_zero_arr_size(_tmp12E,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?tq1.q_volatile == tq2.q_volatile:
0)?tq1.q_restrict == tq2.q_restrict: 0;}static void Cyc_Tcutil_unify_it_conrefs(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp12F=(void*)x->v;void*_tmp130;_LLC9: if((int)_tmp12F != 0)
goto _LLCB;_LLCA:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp132;_tmp132.tag=1;_tmp132.f1=y;_tmp132;});_tmp131;})));return;_LLCB: if(
_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 1)goto _LLCD;_LLCC:({void*_tmp133[0]={};
Cyc_Tcutil_impos(({const char*_tmp134="unify_conref: forward after compress";
_tag_arr(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,37));}),_tag_arr(
_tmp133,sizeof(void*),0));});_LLCD: if(_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 0)
goto _LLC8;_tmp130=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp12F)->f1;_LLCE: {
void*_tmp135=(void*)y->v;void*_tmp136;_LLD0: if((int)_tmp135 != 0)goto _LLD2;_LLD1:(
void*)(y->v=(void*)((void*)x->v));return;_LLD2: if(_tmp135 <= (void*)1?1:*((int*)
_tmp135)!= 1)goto _LLD4;_LLD3:({void*_tmp137[0]={};Cyc_Tcutil_impos(({const char*
_tmp138="unify_conref: forward after compress(2)";_tag_arr(_tmp138,sizeof(char),
_get_zero_arr_size(_tmp138,40));}),_tag_arr(_tmp137,sizeof(void*),0));});_LLD4:
if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 0)goto _LLCF;_tmp136=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp135)->f1;_LLD5: if(cmp(_tmp130,_tmp136)!= 0){Cyc_Tcutil_failure_reason=
reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLCF:;}_LLC8:;}}static int
Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct
Cyc_Absyn_Conref*y){struct _handler_cons _tmp139;_push_handler(& _tmp139);{int
_tmp13B=0;if(setjmp(_tmp139.handler))_tmp13B=1;if(!_tmp13B){Cyc_Tcutil_unify_it_conrefs(
cmp,x,y,(struct _tagged_arr)_tag_arr(0,0,0));{int _tmp13C=1;_npop_handler(0);
return _tmp13C;};_pop_handler();}else{void*_tmp13A=(void*)_exn_thrown;void*
_tmp13E=_tmp13A;_LLD7: if(_tmp13E != Cyc_Tcutil_Unify)goto _LLD9;_LLD8: return 0;
_LLD9:;_LLDA:(void)_throw(_tmp13E);_LLD6:;}}}static int Cyc_Tcutil_boundscmp(void*
b1,void*b2){struct _tuple6 _tmp140=({struct _tuple6 _tmp13F;_tmp13F.f1=b1;_tmp13F.f2=
b2;_tmp13F;});void*_tmp141;void*_tmp142;void*_tmp143;void*_tmp144;void*_tmp145;
struct Cyc_Absyn_Exp*_tmp146;void*_tmp147;struct Cyc_Absyn_Exp*_tmp148;_LLDC:
_tmp141=_tmp140.f1;if((int)_tmp141 != 0)goto _LLDE;_tmp142=_tmp140.f2;if((int)
_tmp142 != 0)goto _LLDE;_LLDD: return 0;_LLDE: _tmp143=_tmp140.f1;if((int)_tmp143 != 0)
goto _LLE0;_LLDF: return - 1;_LLE0: _tmp144=_tmp140.f2;if((int)_tmp144 != 0)goto _LLE2;
_LLE1: return 1;_LLE2: _tmp145=_tmp140.f1;if(_tmp145 <= (void*)1?1:*((int*)_tmp145)
!= 0)goto _LLDB;_tmp146=((struct Cyc_Absyn_Upper_b_struct*)_tmp145)->f1;_tmp147=
_tmp140.f2;if(_tmp147 <= (void*)1?1:*((int*)_tmp147)!= 0)goto _LLDB;_tmp148=((
struct Cyc_Absyn_Upper_b_struct*)_tmp147)->f1;_LLE3: return Cyc_Evexp_const_exp_cmp(
_tmp146,_tmp148);_LLDB:;}static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp149=att;_LLE5: if(_tmp149 <= (void*)17?1:*((int*)_tmp149)!= 0)goto _LLE7;
_LLE6: return 0;_LLE7: if((int)_tmp149 != 0)goto _LLE9;_LLE8: return 1;_LLE9: if((int)
_tmp149 != 1)goto _LLEB;_LLEA: return 2;_LLEB: if((int)_tmp149 != 2)goto _LLED;_LLEC:
return 3;_LLED: if((int)_tmp149 != 3)goto _LLEF;_LLEE: return 4;_LLEF: if((int)_tmp149
!= 4)goto _LLF1;_LLF0: return 5;_LLF1: if(_tmp149 <= (void*)17?1:*((int*)_tmp149)!= 1)
goto _LLF3;_LLF2: return 6;_LLF3: if((int)_tmp149 != 5)goto _LLF5;_LLF4: return 7;_LLF5:
if(_tmp149 <= (void*)17?1:*((int*)_tmp149)!= 2)goto _LLF7;_LLF6: return 8;_LLF7: if((
int)_tmp149 != 6)goto _LLF9;_LLF8: return 9;_LLF9: if((int)_tmp149 != 7)goto _LLFB;
_LLFA: return 10;_LLFB: if((int)_tmp149 != 8)goto _LLFD;_LLFC: return 11;_LLFD: if((int)
_tmp149 != 9)goto _LLFF;_LLFE: return 12;_LLFF: if((int)_tmp149 != 10)goto _LL101;
_LL100: return 13;_LL101: if((int)_tmp149 != 11)goto _LL103;_LL102: return 14;_LL103:
if((int)_tmp149 != 12)goto _LL105;_LL104: return 15;_LL105: if((int)_tmp149 != 13)goto
_LL107;_LL106: return 16;_LL107: if((int)_tmp149 != 14)goto _LL109;_LL108: return 17;
_LL109: if((int)_tmp149 != 15)goto _LL10B;_LL10A: return 18;_LL10B: if(_tmp149 <= (void*)
17?1:*((int*)_tmp149)!= 3)goto _LL10D;_LL10C: return 19;_LL10D: if(_tmp149 <= (void*)
17?1:*((int*)_tmp149)!= 4)goto _LL10F;_LL10E: return 20;_LL10F:;_LL110: return 21;
_LLE4:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct _tuple6
_tmp14B=({struct _tuple6 _tmp14A;_tmp14A.f1=att1;_tmp14A.f2=att2;_tmp14A;});void*
_tmp14C;int _tmp14D;void*_tmp14E;int _tmp14F;void*_tmp150;int _tmp151;void*_tmp152;
int _tmp153;void*_tmp154;int _tmp155;void*_tmp156;int _tmp157;void*_tmp158;struct
_tagged_arr _tmp159;void*_tmp15A;struct _tagged_arr _tmp15B;void*_tmp15C;void*
_tmp15D;int _tmp15E;int _tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;
_LL112: _tmp14C=_tmp14B.f1;if(_tmp14C <= (void*)17?1:*((int*)_tmp14C)!= 0)goto
_LL114;_tmp14D=((struct Cyc_Absyn_Regparm_att_struct*)_tmp14C)->f1;_tmp14E=
_tmp14B.f2;if(_tmp14E <= (void*)17?1:*((int*)_tmp14E)!= 0)goto _LL114;_tmp14F=((
struct Cyc_Absyn_Regparm_att_struct*)_tmp14E)->f1;_LL113: _tmp151=_tmp14D;_tmp153=
_tmp14F;goto _LL115;_LL114: _tmp150=_tmp14B.f1;if(_tmp150 <= (void*)17?1:*((int*)
_tmp150)!= 4)goto _LL116;_tmp151=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp150)->f1;_tmp152=_tmp14B.f2;if(_tmp152 <= (void*)17?1:*((int*)_tmp152)!= 4)
goto _LL116;_tmp153=((struct Cyc_Absyn_Initializes_att_struct*)_tmp152)->f1;_LL115:
_tmp155=_tmp151;_tmp157=_tmp153;goto _LL117;_LL116: _tmp154=_tmp14B.f1;if(_tmp154
<= (void*)17?1:*((int*)_tmp154)!= 1)goto _LL118;_tmp155=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp154)->f1;_tmp156=_tmp14B.f2;if(_tmp156 <= (void*)17?1:*((int*)_tmp156)!= 1)
goto _LL118;_tmp157=((struct Cyc_Absyn_Aligned_att_struct*)_tmp156)->f1;_LL117:
return Cyc_Core_intcmp(_tmp155,_tmp157);_LL118: _tmp158=_tmp14B.f1;if(_tmp158 <= (
void*)17?1:*((int*)_tmp158)!= 2)goto _LL11A;_tmp159=((struct Cyc_Absyn_Section_att_struct*)
_tmp158)->f1;_tmp15A=_tmp14B.f2;if(_tmp15A <= (void*)17?1:*((int*)_tmp15A)!= 2)
goto _LL11A;_tmp15B=((struct Cyc_Absyn_Section_att_struct*)_tmp15A)->f1;_LL119:
return Cyc_strcmp((struct _tagged_arr)_tmp159,(struct _tagged_arr)_tmp15B);_LL11A:
_tmp15C=_tmp14B.f1;if(_tmp15C <= (void*)17?1:*((int*)_tmp15C)!= 3)goto _LL11C;
_tmp15D=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp15C)->f1;_tmp15E=((
struct Cyc_Absyn_Format_att_struct*)_tmp15C)->f2;_tmp15F=((struct Cyc_Absyn_Format_att_struct*)
_tmp15C)->f3;_tmp160=_tmp14B.f2;if(_tmp160 <= (void*)17?1:*((int*)_tmp160)!= 3)
goto _LL11C;_tmp161=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp160)->f1;
_tmp162=((struct Cyc_Absyn_Format_att_struct*)_tmp160)->f2;_tmp163=((struct Cyc_Absyn_Format_att_struct*)
_tmp160)->f3;_LL11B: {int _tmp164=Cyc_Core_intcmp((int)((unsigned int)_tmp15D),(
int)((unsigned int)_tmp161));if(_tmp164 != 0)return _tmp164;{int _tmp165=Cyc_Core_intcmp(
_tmp15E,_tmp162);if(_tmp165 != 0)return _tmp165;return Cyc_Core_intcmp(_tmp15F,
_tmp163);}}_LL11C:;_LL11D: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL111:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp166=Cyc_Tcutil_tvar_kind(tv);_LL11F: if((int)_tmp166 != 3)
goto _LL121;_LL120: t=(void*)2;goto _LL11E;_LL121: if((int)_tmp166 != 4)goto _LL123;
_LL122: t=Cyc_Absyn_empty_effect;goto _LL11E;_LL123:;_LL124: t=Cyc_Absyn_sint_typ;
goto _LL11E;_LL11E:;}return({struct _tuple8*_tmp167=_cycalloc(sizeof(*_tmp167));
_tmp167->f1=tv;_tmp167->f2=t;_tmp167;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp168=Cyc_Tcutil_compress(t);void*_tmp169;struct Cyc_Absyn_TunionInfo
_tmp16A;struct Cyc_List_List*_tmp16B;void*_tmp16C;struct Cyc_Absyn_PtrInfo _tmp16D;
void*_tmp16E;void*_tmp16F;struct Cyc_Absyn_ArrayInfo _tmp170;void*_tmp171;struct
Cyc_List_List*_tmp172;struct Cyc_Absyn_TunionFieldInfo _tmp173;struct Cyc_List_List*
_tmp174;struct Cyc_Absyn_AggrInfo _tmp175;struct Cyc_List_List*_tmp176;struct Cyc_List_List*
_tmp177;void*_tmp178;struct Cyc_Absyn_FnInfo _tmp179;struct Cyc_List_List*_tmp17A;
struct Cyc_Core_Opt*_tmp17B;void*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_VarargInfo*
_tmp17E;struct Cyc_List_List*_tmp17F;void*_tmp180;struct Cyc_List_List*_tmp181;
_LL126: if((int)_tmp168 != 0)goto _LL128;_LL127: goto _LL129;_LL128: if((int)_tmp168 != 
1)goto _LL12A;_LL129: goto _LL12B;_LL12A: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 
6)goto _LL12C;_LL12B: goto _LL12D;_LL12C: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 
12)goto _LL12E;_LL12D: goto _LL12F;_LL12E: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)
!= 13)goto _LL130;_LL12F: goto _LL131;_LL130: if(_tmp168 <= (void*)3?1:*((int*)
_tmp168)!= 18)goto _LL132;_LL131: goto _LL133;_LL132: if(_tmp168 <= (void*)3?1:*((int*)
_tmp168)!= 5)goto _LL134;_LL133: return Cyc_Absyn_empty_effect;_LL134: if(_tmp168 <= (
void*)3?1:*((int*)_tmp168)!= 0)goto _LL136;_LL135: goto _LL137;_LL136: if(_tmp168 <= (
void*)3?1:*((int*)_tmp168)!= 1)goto _LL138;_LL137: {void*_tmp182=Cyc_Tcutil_typ_kind(
t);_LL159: if((int)_tmp182 != 3)goto _LL15B;_LL15A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp184;_tmp184.tag=19;_tmp184.f1=(void*)t;_tmp184;});_tmp183;});_LL15B: if((int)
_tmp182 != 4)goto _LL15D;_LL15C: return t;_LL15D: if((int)_tmp182 != 5)goto _LL15F;
_LL15E: return Cyc_Absyn_empty_effect;_LL15F:;_LL160: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp186;_tmp186.tag=21;_tmp186.f1=(void*)t;_tmp186;});_tmp185;});_LL158:;}_LL138:
if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 15)goto _LL13A;_tmp169=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp168)->f1;_LL139: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp188;_tmp188.tag=19;_tmp188.f1=(void*)_tmp169;_tmp188;});_tmp187;});_LL13A:
if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 2)goto _LL13C;_tmp16A=((struct Cyc_Absyn_TunionType_struct*)
_tmp168)->f1;_tmp16B=_tmp16A.targs;_tmp16C=(void*)_tmp16A.rgn;_LL13B: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp18C=_cycalloc(sizeof(*_tmp18C));
_tmp18C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp18D;_tmp18D.tag=19;_tmp18D.f1=(
void*)_tmp16C;_tmp18D;});_tmp18C;}));_tmp18B->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp16B);_tmp18B;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp18A;_tmp18A.tag=20;_tmp18A.f1=ts;_tmp18A;});_tmp189;}));}_LL13C: if(_tmp168 <= (
void*)3?1:*((int*)_tmp168)!= 4)goto _LL13E;_tmp16D=((struct Cyc_Absyn_PointerType_struct*)
_tmp168)->f1;_tmp16E=(void*)_tmp16D.elt_typ;_tmp16F=(void*)_tmp16D.rgn_typ;
_LL13D: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp18F;_tmp18F.tag=20;_tmp18F.f1=({void*_tmp190[2];_tmp190[1]=Cyc_Tcutil_rgns_of(
_tmp16E);_tmp190[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp191=_cycalloc(
sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Absyn_AccessEff_struct _tmp192;_tmp192.tag=
19;_tmp192.f1=(void*)_tmp16F;_tmp192;});_tmp191;});Cyc_List_list(_tag_arr(
_tmp190,sizeof(void*),2));});_tmp18F;});_tmp18E;}));_LL13E: if(_tmp168 <= (void*)3?
1:*((int*)_tmp168)!= 7)goto _LL140;_tmp170=((struct Cyc_Absyn_ArrayType_struct*)
_tmp168)->f1;_tmp171=(void*)_tmp170.elt_type;_LL13F: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp171));_LL140: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 9)
goto _LL142;_tmp172=((struct Cyc_Absyn_TupleType_struct*)_tmp168)->f1;_LL141: {
struct Cyc_List_List*_tmp193=0;for(0;_tmp172 != 0;_tmp172=_tmp172->tl){_tmp193=({
struct Cyc_List_List*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->hd=(void*)(*((
struct _tuple4*)((struct _tuple4*)_tmp172->hd))).f2;_tmp194->tl=_tmp193;_tmp194;});}
_tmp174=_tmp193;goto _LL143;}_LL142: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 3)
goto _LL144;_tmp173=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp168)->f1;
_tmp174=_tmp173.targs;_LL143: _tmp176=_tmp174;goto _LL145;_LL144: if(_tmp168 <= (
void*)3?1:*((int*)_tmp168)!= 10)goto _LL146;_tmp175=((struct Cyc_Absyn_AggrType_struct*)
_tmp168)->f1;_tmp176=_tmp175.targs;_LL145: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp195=_cycalloc(sizeof(*_tmp195));
_tmp195[0]=({struct Cyc_Absyn_JoinEff_struct _tmp196;_tmp196.tag=20;_tmp196.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp176);_tmp196;});_tmp195;}));_LL146: if(_tmp168 <= (void*)3?
1:*((int*)_tmp168)!= 11)goto _LL148;_tmp177=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp168)->f2;_LL147: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp198;_tmp198.tag=20;_tmp198.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp177);_tmp198;});
_tmp197;}));_LL148: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 14)goto _LL14A;
_tmp178=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp168)->f1;_LL149: return
Cyc_Tcutil_rgns_of(_tmp178);_LL14A: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 17)
goto _LL14C;_LL14B: return Cyc_Tcutil_rgns_of(t);_LL14C: if(_tmp168 <= (void*)3?1:*((
int*)_tmp168)!= 8)goto _LL14E;_tmp179=((struct Cyc_Absyn_FnType_struct*)_tmp168)->f1;
_tmp17A=_tmp179.tvars;_tmp17B=_tmp179.effect;_tmp17C=(void*)_tmp179.ret_typ;
_tmp17D=_tmp179.args;_tmp17E=_tmp179.cyc_varargs;_tmp17F=_tmp179.rgn_po;_LL14D: {
void*_tmp199=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp17A),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp17B))->v);return Cyc_Tcutil_normalize_effect(
_tmp199);}_LL14E: if((int)_tmp168 != 2)goto _LL150;_LL14F: return Cyc_Absyn_empty_effect;
_LL150: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 19)goto _LL152;_LL151: goto
_LL153;_LL152: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 20)goto _LL154;_LL153:
return t;_LL154: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 21)goto _LL156;_tmp180=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp168)->f1;_LL155: return Cyc_Tcutil_rgns_of(
_tmp180);_LL156: if(_tmp168 <= (void*)3?1:*((int*)_tmp168)!= 16)goto _LL125;_tmp181=((
struct Cyc_Absyn_TypedefType_struct*)_tmp168)->f2;_LL157: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));
_tmp19A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19B;_tmp19B.tag=20;_tmp19B.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp181);_tmp19B;});_tmp19A;}));_LL125:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp19C=e;struct Cyc_List_List*_tmp19D;
struct Cyc_List_List**_tmp19E;void*_tmp19F;_LL162: if(_tmp19C <= (void*)3?1:*((int*)
_tmp19C)!= 20)goto _LL164;_tmp19D=((struct Cyc_Absyn_JoinEff_struct*)_tmp19C)->f1;
_tmp19E=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp19C)->f1;
_LL163: {int redo_join=0;{struct Cyc_List_List*effs=*((struct Cyc_List_List**)
_tmp19E);for(0;effs != 0;effs=effs->tl){void*_tmp1A0=(void*)effs->hd;(void*)(effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1A0)));{void*_tmp1A1=(
void*)effs->hd;void*_tmp1A2;_LL169: if(_tmp1A1 <= (void*)3?1:*((int*)_tmp1A1)!= 20)
goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(_tmp1A1 <= (void*)3?1:*((int*)_tmp1A1)!= 
19)goto _LL16D;_tmp1A2=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1A1)->f1;
if((int)_tmp1A2 != 2)goto _LL16D;_LL16C: redo_join=1;goto _LL168;_LL16D:;_LL16E: goto
_LL168;_LL168:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*((struct Cyc_List_List**)_tmp19E);for(0;effs != 0;effs=effs->tl){
void*_tmp1A3=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp1A4;
void*_tmp1A5;_LL170: if(_tmp1A3 <= (void*)3?1:*((int*)_tmp1A3)!= 20)goto _LL172;
_tmp1A4=((struct Cyc_Absyn_JoinEff_struct*)_tmp1A3)->f1;_LL171: effects=Cyc_List_revappend(
_tmp1A4,effects);goto _LL16F;_LL172: if(_tmp1A3 <= (void*)3?1:*((int*)_tmp1A3)!= 19)
goto _LL174;_tmp1A5=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1A3)->f1;if((
int)_tmp1A5 != 2)goto _LL174;_LL173: goto _LL16F;_LL174:;_LL175: effects=({struct Cyc_List_List*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->hd=(void*)_tmp1A3;_tmp1A6->tl=
effects;_tmp1A6;});goto _LL16F;_LL16F:;}}*((struct Cyc_List_List**)_tmp19E)=Cyc_List_imp_rev(
effects);return e;}}_LL164: if(_tmp19C <= (void*)3?1:*((int*)_tmp19C)!= 21)goto
_LL166;_tmp19F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp19C)->f1;_LL165: {
void*_tmp1A7=Cyc_Tcutil_compress(_tmp19F);_LL177: if(_tmp1A7 <= (void*)3?1:*((int*)
_tmp1A7)!= 0)goto _LL179;_LL178: goto _LL17A;_LL179: if(_tmp1A7 <= (void*)3?1:*((int*)
_tmp1A7)!= 1)goto _LL17B;_LL17A: return e;_LL17B:;_LL17C: return Cyc_Tcutil_rgns_of(
_tmp19F);_LL176:;}_LL166:;_LL167: return e;_LL161:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1A8=({struct Cyc_Absyn_FnType_struct*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));
_tmp1A9[0]=({struct Cyc_Absyn_FnType_struct _tmp1AA;_tmp1AA.tag=8;_tmp1AA.f1=({
struct Cyc_Absyn_FnInfo _tmp1AB;_tmp1AB.tvars=0;_tmp1AB.effect=({struct Cyc_Core_Opt*
_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->v=(void*)eff;_tmp1AC;});_tmp1AB.ret_typ=(
void*)((void*)0);_tmp1AB.args=0;_tmp1AB.c_varargs=0;_tmp1AB.cyc_varargs=0;
_tmp1AB.rgn_po=0;_tmp1AB.attributes=0;_tmp1AB;});_tmp1AA;});_tmp1A9;});return Cyc_Absyn_atb_typ((
void*)_tmp1A8,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp1AD=Cyc_Tcutil_compress(e);void*_tmp1AE;
struct Cyc_List_List*_tmp1AF;void*_tmp1B0;struct Cyc_Core_Opt*_tmp1B1;struct Cyc_Core_Opt*
_tmp1B2;struct Cyc_Core_Opt**_tmp1B3;struct Cyc_Core_Opt*_tmp1B4;_LL17E: if(_tmp1AD
<= (void*)3?1:*((int*)_tmp1AD)!= 19)goto _LL180;_tmp1AE=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1AD)->f1;_LL17F: if(constrain)return Cyc_Tcutil_unify(r,_tmp1AE);_tmp1AE=Cyc_Tcutil_compress(
_tmp1AE);if(r == _tmp1AE)return 1;{struct _tuple6 _tmp1B6=({struct _tuple6 _tmp1B5;
_tmp1B5.f1=r;_tmp1B5.f2=_tmp1AE;_tmp1B5;});void*_tmp1B7;struct Cyc_Absyn_Tvar*
_tmp1B8;void*_tmp1B9;struct Cyc_Absyn_Tvar*_tmp1BA;_LL189: _tmp1B7=_tmp1B6.f1;if(
_tmp1B7 <= (void*)3?1:*((int*)_tmp1B7)!= 1)goto _LL18B;_tmp1B8=((struct Cyc_Absyn_VarType_struct*)
_tmp1B7)->f1;_tmp1B9=_tmp1B6.f2;if(_tmp1B9 <= (void*)3?1:*((int*)_tmp1B9)!= 1)
goto _LL18B;_tmp1BA=((struct Cyc_Absyn_VarType_struct*)_tmp1B9)->f1;_LL18A: return
Cyc_Absyn_tvar_cmp(_tmp1B8,_tmp1BA)== 0;_LL18B:;_LL18C: return 0;_LL188:;}_LL180:
if(_tmp1AD <= (void*)3?1:*((int*)_tmp1AD)!= 20)goto _LL182;_tmp1AF=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1AD)->f1;_LL181: for(0;_tmp1AF != 0;_tmp1AF=_tmp1AF->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1AF->hd))return 1;}return 0;_LL182: if(_tmp1AD <= (void*)3?1:*((
int*)_tmp1AD)!= 21)goto _LL184;_tmp1B0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1AD)->f1;_LL183: {void*_tmp1BB=Cyc_Tcutil_rgns_of(_tmp1B0);void*_tmp1BC;
_LL18E: if(_tmp1BB <= (void*)3?1:*((int*)_tmp1BB)!= 21)goto _LL190;_tmp1BC=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1BB)->f1;_LL18F: if(!constrain)return 0;{void*
_tmp1BD=Cyc_Tcutil_compress(_tmp1BC);struct Cyc_Core_Opt*_tmp1BE;struct Cyc_Core_Opt*
_tmp1BF;struct Cyc_Core_Opt**_tmp1C0;struct Cyc_Core_Opt*_tmp1C1;_LL193: if(_tmp1BD
<= (void*)3?1:*((int*)_tmp1BD)!= 0)goto _LL195;_tmp1BE=((struct Cyc_Absyn_Evar_struct*)
_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_Evar_struct*)_tmp1BD)->f2;_tmp1C0=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1BD)->f2;_tmp1C1=((struct Cyc_Absyn_Evar_struct*)
_tmp1BD)->f4;_LL194: {void*_tmp1C2=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1C1);Cyc_Tcutil_occurs(_tmp1C2,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C1))->v,r);{void*_tmp1C3=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));
_tmp1C5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1C6;_tmp1C6.tag=20;_tmp1C6.f1=({
void*_tmp1C7[2];_tmp1C7[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1C8=
_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1C9;
_tmp1C9.tag=19;_tmp1C9.f1=(void*)r;_tmp1C9;});_tmp1C8;});_tmp1C7[0]=_tmp1C2;Cyc_List_list(
_tag_arr(_tmp1C7,sizeof(void*),2));});_tmp1C6;});_tmp1C5;}));*((struct Cyc_Core_Opt**)
_tmp1C0)=({struct Cyc_Core_Opt*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->v=(
void*)_tmp1C3;_tmp1C4;});return 1;}}_LL195:;_LL196: return 0;_LL192:;}_LL190:;
_LL191: return Cyc_Tcutil_region_in_effect(constrain,r,_tmp1BB);_LL18D:;}_LL184:
if(_tmp1AD <= (void*)3?1:*((int*)_tmp1AD)!= 0)goto _LL186;_tmp1B1=((struct Cyc_Absyn_Evar_struct*)
_tmp1AD)->f1;_tmp1B2=((struct Cyc_Absyn_Evar_struct*)_tmp1AD)->f2;_tmp1B3=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1AD)->f2;_tmp1B4=((struct Cyc_Absyn_Evar_struct*)
_tmp1AD)->f4;_LL185: if(_tmp1B1 == 0?1:(void*)_tmp1B1->v != (void*)4)({void*_tmp1CA[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1CB="effect evar has wrong kind";_tag_arr(_tmp1CB,sizeof(char),
_get_zero_arr_size(_tmp1CB,27));}),_tag_arr(_tmp1CA,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp1CC=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1B4);Cyc_Tcutil_occurs(_tmp1CC,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B4))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1CD=({struct Cyc_Absyn_JoinEff_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));
_tmp1CF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1D0;_tmp1D0.tag=20;_tmp1D0.f1=({
struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->hd=(void*)
_tmp1CC;_tmp1D1->tl=({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));
_tmp1D2->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D3=_cycalloc(
sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D4;_tmp1D4.tag=
19;_tmp1D4.f1=(void*)r;_tmp1D4;});_tmp1D3;}));_tmp1D2->tl=0;_tmp1D2;});_tmp1D1;});
_tmp1D0;});_tmp1CF;});*((struct Cyc_Core_Opt**)_tmp1B3)=({struct Cyc_Core_Opt*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->v=(void*)((void*)_tmp1CD);_tmp1CE;});
return 1;}}_LL186:;_LL187: return 0;_LL17D:;}}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1D5=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp1D6;void*_tmp1D7;struct Cyc_Core_Opt*
_tmp1D8;struct Cyc_Core_Opt*_tmp1D9;struct Cyc_Core_Opt**_tmp1DA;struct Cyc_Core_Opt*
_tmp1DB;_LL198: if(_tmp1D5 <= (void*)3?1:*((int*)_tmp1D5)!= 19)goto _LL19A;_LL199:
return 0;_LL19A: if(_tmp1D5 <= (void*)3?1:*((int*)_tmp1D5)!= 20)goto _LL19C;_tmp1D6=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1D5)->f1;_LL19B: for(0;_tmp1D6 != 0;_tmp1D6=
_tmp1D6->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1D6->hd))
return 1;}return 0;_LL19C: if(_tmp1D5 <= (void*)3?1:*((int*)_tmp1D5)!= 21)goto _LL19E;
_tmp1D7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1D5)->f1;_LL19D: _tmp1D7=Cyc_Tcutil_compress(
_tmp1D7);if(t == _tmp1D7)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp1D7);{void*_tmp1DC=Cyc_Tcutil_rgns_of(t);void*_tmp1DD;_LL1A3: if(_tmp1DC <= (
void*)3?1:*((int*)_tmp1DC)!= 21)goto _LL1A5;_tmp1DD=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1DC)->f1;_LL1A4: {struct _tuple6 _tmp1DF=({struct _tuple6 _tmp1DE;_tmp1DE.f1=Cyc_Tcutil_compress(
_tmp1DD);_tmp1DE.f2=_tmp1D7;_tmp1DE;});void*_tmp1E0;struct Cyc_Absyn_Tvar*_tmp1E1;
void*_tmp1E2;struct Cyc_Absyn_Tvar*_tmp1E3;_LL1A8: _tmp1E0=_tmp1DF.f1;if(_tmp1E0 <= (
void*)3?1:*((int*)_tmp1E0)!= 1)goto _LL1AA;_tmp1E1=((struct Cyc_Absyn_VarType_struct*)
_tmp1E0)->f1;_tmp1E2=_tmp1DF.f2;if(_tmp1E2 <= (void*)3?1:*((int*)_tmp1E2)!= 1)
goto _LL1AA;_tmp1E3=((struct Cyc_Absyn_VarType_struct*)_tmp1E2)->f1;_LL1A9: return
Cyc_Tcutil_unify(t,_tmp1D7);_LL1AA:;_LL1AB: return _tmp1DD == _tmp1D7;_LL1A7:;}
_LL1A5:;_LL1A6: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1DC);
_LL1A2:;}_LL19E: if(_tmp1D5 <= (void*)3?1:*((int*)_tmp1D5)!= 0)goto _LL1A0;_tmp1D8=((
struct Cyc_Absyn_Evar_struct*)_tmp1D5)->f1;_tmp1D9=((struct Cyc_Absyn_Evar_struct*)
_tmp1D5)->f2;_tmp1DA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1D5)->f2;_tmp1DB=((struct Cyc_Absyn_Evar_struct*)_tmp1D5)->f4;_LL19F: if(
_tmp1D8 == 0?1:(void*)_tmp1D8->v != (void*)4)({void*_tmp1E4[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1E5="effect evar has wrong kind";
_tag_arr(_tmp1E5,sizeof(char),_get_zero_arr_size(_tmp1E5,27));}),_tag_arr(
_tmp1E4,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp1E6=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1DB);Cyc_Tcutil_occurs(_tmp1E6,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1DB))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp1E7=({struct Cyc_Absyn_JoinEff_struct*_tmp1E9=
_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EA;
_tmp1EA.tag=20;_tmp1EA.f1=({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*
_tmp1EB));_tmp1EB->hd=(void*)_tmp1E6;_tmp1EB->tl=({struct Cyc_List_List*_tmp1EC=
_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1EE;_tmp1EE.tag=21;_tmp1EE.f1=(void*)t;_tmp1EE;});_tmp1ED;}));_tmp1EC->tl=0;
_tmp1EC;});_tmp1EB;});_tmp1EA;});_tmp1E9;});*((struct Cyc_Core_Opt**)_tmp1DA)=({
struct Cyc_Core_Opt*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->v=(void*)((void*)
_tmp1E7);_tmp1E8;});return 1;}}_LL1A0:;_LL1A1: return 0;_LL197:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp1EF=e;struct Cyc_Absyn_Tvar*_tmp1F0;struct Cyc_List_List*_tmp1F1;void*
_tmp1F2;struct Cyc_Core_Opt*_tmp1F3;struct Cyc_Core_Opt*_tmp1F4;struct Cyc_Core_Opt**
_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;_LL1AD: if(_tmp1EF <= (void*)3?1:*((int*)
_tmp1EF)!= 1)goto _LL1AF;_tmp1F0=((struct Cyc_Absyn_VarType_struct*)_tmp1EF)->f1;
_LL1AE: return Cyc_Absyn_tvar_cmp(v,_tmp1F0)== 0;_LL1AF: if(_tmp1EF <= (void*)3?1:*((
int*)_tmp1EF)!= 20)goto _LL1B1;_tmp1F1=((struct Cyc_Absyn_JoinEff_struct*)_tmp1EF)->f1;
_LL1B0: for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp1F1->hd))return 1;}return 0;_LL1B1: if(_tmp1EF <= (
void*)3?1:*((int*)_tmp1EF)!= 21)goto _LL1B3;_tmp1F2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EF)->f1;_LL1B2: {void*_tmp1F7=Cyc_Tcutil_rgns_of(_tmp1F2);void*_tmp1F8;
_LL1B8: if(_tmp1F7 <= (void*)3?1:*((int*)_tmp1F7)!= 21)goto _LL1BA;_tmp1F8=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1F7)->f1;_LL1B9: if(!may_constrain_evars)
return 0;{void*_tmp1F9=Cyc_Tcutil_compress(_tmp1F8);struct Cyc_Core_Opt*_tmp1FA;
struct Cyc_Core_Opt*_tmp1FB;struct Cyc_Core_Opt**_tmp1FC;struct Cyc_Core_Opt*
_tmp1FD;_LL1BD: if(_tmp1F9 <= (void*)3?1:*((int*)_tmp1F9)!= 0)goto _LL1BF;_tmp1FA=((
struct Cyc_Absyn_Evar_struct*)_tmp1F9)->f1;_tmp1FB=((struct Cyc_Absyn_Evar_struct*)
_tmp1F9)->f2;_tmp1FC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1F9)->f2;_tmp1FD=((struct Cyc_Absyn_Evar_struct*)_tmp1F9)->f4;_LL1BE: {void*
_tmp1FE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1FD);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1FD))->v,v))return 0;{void*_tmp1FF=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp201=_cycalloc(sizeof(*_tmp201));
_tmp201[0]=({struct Cyc_Absyn_JoinEff_struct _tmp202;_tmp202.tag=20;_tmp202.f1=({
void*_tmp203[2];_tmp203[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp204=
_cycalloc(sizeof(*_tmp204));_tmp204[0]=({struct Cyc_Absyn_VarType_struct _tmp205;
_tmp205.tag=1;_tmp205.f1=v;_tmp205;});_tmp204;});_tmp203[0]=_tmp1FE;Cyc_List_list(
_tag_arr(_tmp203,sizeof(void*),2));});_tmp202;});_tmp201;}));*((struct Cyc_Core_Opt**)
_tmp1FC)=({struct Cyc_Core_Opt*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->v=(
void*)_tmp1FF;_tmp200;});return 1;}}_LL1BF:;_LL1C0: return 0;_LL1BC:;}_LL1BA:;
_LL1BB: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp1F7);_LL1B7:;}
_LL1B3: if(_tmp1EF <= (void*)3?1:*((int*)_tmp1EF)!= 0)goto _LL1B5;_tmp1F3=((struct
Cyc_Absyn_Evar_struct*)_tmp1EF)->f1;_tmp1F4=((struct Cyc_Absyn_Evar_struct*)
_tmp1EF)->f2;_tmp1F5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1EF)->f2;_tmp1F6=((struct Cyc_Absyn_Evar_struct*)_tmp1EF)->f4;_LL1B4: if(
_tmp1F3 == 0?1:(void*)_tmp1F3->v != (void*)4)({void*_tmp206[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp207="effect evar has wrong kind";
_tag_arr(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,27));}),_tag_arr(
_tmp206,sizeof(void*),0));});{void*_tmp208=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp1F6);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1F6))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct*_tmp209=({struct Cyc_Absyn_JoinEff_struct*_tmp20B=
_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp20C;
_tmp20C.tag=20;_tmp20C.f1=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*
_tmp20D));_tmp20D->hd=(void*)_tmp208;_tmp20D->tl=({struct Cyc_List_List*_tmp20E=
_cycalloc(sizeof(*_tmp20E));_tmp20E->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F[0]=({struct Cyc_Absyn_VarType_struct
_tmp210;_tmp210.tag=1;_tmp210.f1=v;_tmp210;});_tmp20F;}));_tmp20E->tl=0;_tmp20E;});
_tmp20D;});_tmp20C;});_tmp20B;});*((struct Cyc_Core_Opt**)_tmp1F5)=({struct Cyc_Core_Opt*
_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->v=(void*)((void*)_tmp209);_tmp20A;});
return 1;}}_LL1B5:;_LL1B6: return 0;_LL1AC:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp211=e;struct Cyc_List_List*
_tmp212;void*_tmp213;_LL1C2: if(_tmp211 <= (void*)3?1:*((int*)_tmp211)!= 20)goto
_LL1C4;_tmp212=((struct Cyc_Absyn_JoinEff_struct*)_tmp211)->f1;_LL1C3: for(0;
_tmp212 != 0;_tmp212=_tmp212->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp212->hd))return 1;}return 0;_LL1C4: if(_tmp211 <= (void*)3?1:*((int*)_tmp211)!= 
21)goto _LL1C6;_tmp213=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp211)->f1;
_LL1C5: {void*_tmp214=Cyc_Tcutil_rgns_of(_tmp213);void*_tmp215;_LL1CB: if(_tmp214
<= (void*)3?1:*((int*)_tmp214)!= 21)goto _LL1CD;_tmp215=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp214)->f1;_LL1CC: return 0;_LL1CD:;_LL1CE: return Cyc_Tcutil_evar_in_effect(evar,
_tmp214);_LL1CA:;}_LL1C6: if(_tmp211 <= (void*)3?1:*((int*)_tmp211)!= 0)goto _LL1C8;
_LL1C7: return evar == e;_LL1C8:;_LL1C9: return 0;_LL1C1:;}}int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2){void*_tmp216=Cyc_Tcutil_compress(e1);
struct Cyc_List_List*_tmp217;void*_tmp218;struct Cyc_Absyn_Tvar*_tmp219;void*
_tmp21A;struct Cyc_Core_Opt*_tmp21B;struct Cyc_Core_Opt**_tmp21C;struct Cyc_Core_Opt*
_tmp21D;_LL1D0: if(_tmp216 <= (void*)3?1:*((int*)_tmp216)!= 20)goto _LL1D2;_tmp217=((
struct Cyc_Absyn_JoinEff_struct*)_tmp216)->f1;_LL1D1: for(0;_tmp217 != 0;_tmp217=
_tmp217->tl){if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp217->hd,
e2))return 0;}return 1;_LL1D2: if(_tmp216 <= (void*)3?1:*((int*)_tmp216)!= 19)goto
_LL1D4;_tmp218=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp216)->f1;_LL1D3:
return Cyc_Tcutil_region_in_effect(0,_tmp218,e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(
_tmp218,(void*)2): 0);_LL1D4: if(_tmp216 <= (void*)3?1:*((int*)_tmp216)!= 1)goto
_LL1D6;_tmp219=((struct Cyc_Absyn_VarType_struct*)_tmp216)->f1;_LL1D5: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,_tmp219,e2);_LL1D6: if(_tmp216 <= (void*)3?1:*((int*)_tmp216)
!= 21)goto _LL1D8;_tmp21A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp216)->f1;
_LL1D7: {void*_tmp21E=Cyc_Tcutil_rgns_of(_tmp21A);void*_tmp21F;_LL1DD: if(_tmp21E
<= (void*)3?1:*((int*)_tmp21E)!= 21)goto _LL1DF;_tmp21F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp21E)->f1;_LL1DE: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp21F,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp21F,Cyc_Absyn_sint_typ): 0);
_LL1DF:;_LL1E0: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp21E,e2);
_LL1DC:;}_LL1D8: if(_tmp216 <= (void*)3?1:*((int*)_tmp216)!= 0)goto _LL1DA;_tmp21B=((
struct Cyc_Absyn_Evar_struct*)_tmp216)->f2;_tmp21C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp216)->f2;_tmp21D=((struct Cyc_Absyn_Evar_struct*)
_tmp216)->f4;_LL1D9: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*((struct Cyc_Core_Opt**)
_tmp21C)=({struct Cyc_Core_Opt*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->v=(
void*)Cyc_Absyn_empty_effect;_tmp220;});return 1;_LL1DA:;_LL1DB:({struct Cyc_String_pa_struct
_tmp223;_tmp223.tag=0;_tmp223.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{
void*_tmp221[1]={& _tmp223};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp222="subset_effect: bad effect: %s";_tag_arr(
_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,30));}),_tag_arr(_tmp221,sizeof(
void*),1));}});_LL1CF:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(
0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)?Cyc_Tcutil_subset_effect(
1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple6 _tmp225;void*_tmp226;void*_tmp227;struct _tuple6*_tmp224=(struct
_tuple6*)r1->hd;_tmp225=*_tmp224;_tmp226=_tmp225.f1;_tmp227=_tmp225.f2;{int found=
_tmp226 == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){
struct _tuple6 _tmp229;void*_tmp22A;void*_tmp22B;struct _tuple6*_tmp228=(struct
_tuple6*)r2->hd;_tmp229=*_tmp228;_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;if(Cyc_Tcutil_unify(
_tmp226,_tmp22A)?Cyc_Tcutil_unify(_tmp227,_tmp22B): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp22C=t1;struct Cyc_Core_Opt*_tmp22D;struct Cyc_Core_Opt*
_tmp22E;struct Cyc_Core_Opt**_tmp22F;struct Cyc_Core_Opt*_tmp230;_LL1E2: if(_tmp22C
<= (void*)3?1:*((int*)_tmp22C)!= 0)goto _LL1E4;_tmp22D=((struct Cyc_Absyn_Evar_struct*)
_tmp22C)->f1;_tmp22E=((struct Cyc_Absyn_Evar_struct*)_tmp22C)->f2;_tmp22F=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp22C)->f2;_tmp230=((struct Cyc_Absyn_Evar_struct*)
_tmp22C)->f4;_LL1E3: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp230))->v,t2);{void*_tmp231=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp231,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp22D))->v)){*((struct Cyc_Core_Opt**)_tmp22F)=({struct Cyc_Core_Opt*_tmp232=
_cycalloc(sizeof(*_tmp232));_tmp232->v=(void*)t2;_tmp232;});return;}else{{void*
_tmp233=t2;struct Cyc_Core_Opt*_tmp234;struct Cyc_Core_Opt**_tmp235;struct Cyc_Core_Opt*
_tmp236;struct Cyc_Absyn_PtrInfo _tmp237;_LL1E7: if(_tmp233 <= (void*)3?1:*((int*)
_tmp233)!= 0)goto _LL1E9;_tmp234=((struct Cyc_Absyn_Evar_struct*)_tmp233)->f2;
_tmp235=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp233)->f2;
_tmp236=((struct Cyc_Absyn_Evar_struct*)_tmp233)->f4;_LL1E8: {struct Cyc_List_List*
_tmp238=(struct Cyc_List_List*)_tmp230->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp236))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp238,(struct
Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({const char*_tmp239="(type variable would escape scope)";
_tag_arr(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,35));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp22D->v,_tmp231)){*((
struct Cyc_Core_Opt**)_tmp235)=({struct Cyc_Core_Opt*_tmp23A=_cycalloc(sizeof(*
_tmp23A));_tmp23A->v=(void*)t1;_tmp23A;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp23B="(kinds are incompatible)";_tag_arr(_tmp23B,sizeof(char),
_get_zero_arr_size(_tmp23B,25));});goto _LL1E6;}_LL1E9: if(_tmp233 <= (void*)3?1:*((
int*)_tmp233)!= 4)goto _LL1EB;_tmp237=((struct Cyc_Absyn_PointerType_struct*)
_tmp233)->f1;if(!((void*)_tmp22D->v == (void*)2))goto _LL1EB;_LL1EA: {struct Cyc_Absyn_Conref*
_tmp23C=Cyc_Absyn_compress_conref(_tmp237.bounds);{void*_tmp23D=(void*)_tmp23C->v;
_LL1EE: if((int)_tmp23D != 0)goto _LL1F0;_LL1EF:(void*)(_tmp23C->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp23F;_tmp23F.tag=0;_tmp23F.f1=(void*)((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp241;_tmp241.tag=0;_tmp241.f1=Cyc_Absyn_signed_int_exp(
1,0);_tmp241;});_tmp240;}));_tmp23F;});_tmp23E;})));*((struct Cyc_Core_Opt**)
_tmp22F)=({struct Cyc_Core_Opt*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->v=(
void*)t2;_tmp242;});return;_LL1F0:;_LL1F1: goto _LL1ED;_LL1ED:;}goto _LL1E6;}_LL1EB:;
_LL1EC: goto _LL1E6;_LL1E6:;}Cyc_Tcutil_failure_reason=({const char*_tmp243="(kinds are incompatible)";
_tag_arr(_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL1E4:;_LL1E5: goto _LL1E1;_LL1E1:;}{struct _tuple6
_tmp245=({struct _tuple6 _tmp244;_tmp244.f1=t2;_tmp244.f2=t1;_tmp244;});void*
_tmp246;void*_tmp247;void*_tmp248;void*_tmp249;struct Cyc_Absyn_Tvar*_tmp24A;void*
_tmp24B;struct Cyc_Absyn_Tvar*_tmp24C;void*_tmp24D;struct Cyc_Absyn_AggrInfo
_tmp24E;void*_tmp24F;struct Cyc_List_List*_tmp250;void*_tmp251;struct Cyc_Absyn_AggrInfo
_tmp252;void*_tmp253;struct Cyc_List_List*_tmp254;void*_tmp255;struct _tuple1*
_tmp256;void*_tmp257;struct _tuple1*_tmp258;void*_tmp259;struct Cyc_List_List*
_tmp25A;void*_tmp25B;struct Cyc_List_List*_tmp25C;void*_tmp25D;struct Cyc_Absyn_TunionInfo
_tmp25E;void*_tmp25F;struct Cyc_Absyn_Tuniondecl**_tmp260;struct Cyc_Absyn_Tuniondecl*
_tmp261;struct Cyc_List_List*_tmp262;void*_tmp263;void*_tmp264;struct Cyc_Absyn_TunionInfo
_tmp265;void*_tmp266;struct Cyc_Absyn_Tuniondecl**_tmp267;struct Cyc_Absyn_Tuniondecl*
_tmp268;struct Cyc_List_List*_tmp269;void*_tmp26A;void*_tmp26B;struct Cyc_Absyn_TunionFieldInfo
_tmp26C;void*_tmp26D;struct Cyc_Absyn_Tuniondecl*_tmp26E;struct Cyc_Absyn_Tunionfield*
_tmp26F;struct Cyc_List_List*_tmp270;void*_tmp271;struct Cyc_Absyn_TunionFieldInfo
_tmp272;void*_tmp273;struct Cyc_Absyn_Tuniondecl*_tmp274;struct Cyc_Absyn_Tunionfield*
_tmp275;struct Cyc_List_List*_tmp276;void*_tmp277;struct Cyc_Absyn_PtrInfo _tmp278;
void*_tmp279;void*_tmp27A;struct Cyc_Absyn_Conref*_tmp27B;struct Cyc_Absyn_Tqual
_tmp27C;struct Cyc_Absyn_Conref*_tmp27D;struct Cyc_Absyn_Conref*_tmp27E;void*
_tmp27F;struct Cyc_Absyn_PtrInfo _tmp280;void*_tmp281;void*_tmp282;struct Cyc_Absyn_Conref*
_tmp283;struct Cyc_Absyn_Tqual _tmp284;struct Cyc_Absyn_Conref*_tmp285;struct Cyc_Absyn_Conref*
_tmp286;void*_tmp287;void*_tmp288;void*_tmp289;void*_tmp28A;void*_tmp28B;void*
_tmp28C;void*_tmp28D;void*_tmp28E;void*_tmp28F;int _tmp290;void*_tmp291;int
_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;void*_tmp296;void*_tmp297;int
_tmp298;void*_tmp299;int _tmp29A;void*_tmp29B;void*_tmp29C;void*_tmp29D;void*
_tmp29E;void*_tmp29F;struct Cyc_Absyn_ArrayInfo _tmp2A0;void*_tmp2A1;struct Cyc_Absyn_Tqual
_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Conref*_tmp2A4;void*_tmp2A5;
struct Cyc_Absyn_ArrayInfo _tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Tqual _tmp2A8;
struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Conref*_tmp2AA;void*_tmp2AB;struct
Cyc_Absyn_FnInfo _tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_Core_Opt*_tmp2AE;
void*_tmp2AF;struct Cyc_List_List*_tmp2B0;int _tmp2B1;struct Cyc_Absyn_VarargInfo*
_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B4;void*_tmp2B5;
struct Cyc_Absyn_FnInfo _tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_Core_Opt*
_tmp2B8;void*_tmp2B9;struct Cyc_List_List*_tmp2BA;int _tmp2BB;struct Cyc_Absyn_VarargInfo*
_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BE;void*_tmp2BF;
struct Cyc_List_List*_tmp2C0;void*_tmp2C1;struct Cyc_List_List*_tmp2C2;void*
_tmp2C3;void*_tmp2C4;struct Cyc_List_List*_tmp2C5;void*_tmp2C6;void*_tmp2C7;
struct Cyc_List_List*_tmp2C8;void*_tmp2C9;void*_tmp2CA;void*_tmp2CB;void*_tmp2CC;
void*_tmp2CD;void*_tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*_tmp2D2;
void*_tmp2D3;void*_tmp2D4;_LL1F3: _tmp246=_tmp245.f1;if(_tmp246 <= (void*)3?1:*((
int*)_tmp246)!= 0)goto _LL1F5;_LL1F4: Cyc_Tcutil_unify_it(t2,t1);return;_LL1F5:
_tmp247=_tmp245.f1;if((int)_tmp247 != 0)goto _LL1F7;_tmp248=_tmp245.f2;if((int)
_tmp248 != 0)goto _LL1F7;_LL1F6: return;_LL1F7: _tmp249=_tmp245.f1;if(_tmp249 <= (
void*)3?1:*((int*)_tmp249)!= 1)goto _LL1F9;_tmp24A=((struct Cyc_Absyn_VarType_struct*)
_tmp249)->f1;_tmp24B=_tmp245.f2;if(_tmp24B <= (void*)3?1:*((int*)_tmp24B)!= 1)
goto _LL1F9;_tmp24C=((struct Cyc_Absyn_VarType_struct*)_tmp24B)->f1;_LL1F8: {
struct _tagged_arr*_tmp2D5=_tmp24A->name;struct _tagged_arr*_tmp2D6=_tmp24C->name;
int _tmp2D7=*((int*)_check_null(_tmp24A->identity));int _tmp2D8=*((int*)
_check_null(_tmp24C->identity));void*_tmp2D9=Cyc_Tcutil_tvar_kind(_tmp24A);void*
_tmp2DA=Cyc_Tcutil_tvar_kind(_tmp24C);if(_tmp2D8 == _tmp2D7?Cyc_strptrcmp(_tmp2D5,
_tmp2D6)== 0: 0){if(_tmp2D9 != _tmp2DA)({struct Cyc_String_pa_struct _tmp2DF;_tmp2DF.tag=
0;_tmp2DF.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp2DA);{struct Cyc_String_pa_struct
_tmp2DE;_tmp2DE.tag=0;_tmp2DE.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2D9);{struct Cyc_String_pa_struct _tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=(struct
_tagged_arr)*((struct _tagged_arr*)_tmp2D5);{void*_tmp2DB[3]={& _tmp2DD,& _tmp2DE,&
_tmp2DF};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp2DC="same type variable %s has kinds %s and %s";_tag_arr(_tmp2DC,
sizeof(char),_get_zero_arr_size(_tmp2DC,42));}),_tag_arr(_tmp2DB,sizeof(void*),3));}}}});
return;}Cyc_Tcutil_failure_reason=({const char*_tmp2E0="(variable types are not the same)";
_tag_arr(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,34));});goto _LL1F2;}
_LL1F9: _tmp24D=_tmp245.f1;if(_tmp24D <= (void*)3?1:*((int*)_tmp24D)!= 10)goto
_LL1FB;_tmp24E=((struct Cyc_Absyn_AggrType_struct*)_tmp24D)->f1;_tmp24F=(void*)
_tmp24E.aggr_info;_tmp250=_tmp24E.targs;_tmp251=_tmp245.f2;if(_tmp251 <= (void*)3?
1:*((int*)_tmp251)!= 10)goto _LL1FB;_tmp252=((struct Cyc_Absyn_AggrType_struct*)
_tmp251)->f1;_tmp253=(void*)_tmp252.aggr_info;_tmp254=_tmp252.targs;_LL1FA: {
void*_tmp2E2;struct _tuple1*_tmp2E3;struct _tuple5 _tmp2E1=Cyc_Absyn_aggr_kinded_name(
_tmp253);_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;{void*_tmp2E5;struct _tuple1*
_tmp2E6;struct _tuple5 _tmp2E4=Cyc_Absyn_aggr_kinded_name(_tmp24F);_tmp2E5=_tmp2E4.f1;
_tmp2E6=_tmp2E4.f2;if(_tmp2E2 != _tmp2E5){Cyc_Tcutil_failure_reason=({const char*
_tmp2E7="(struct and union type)";_tag_arr(_tmp2E7,sizeof(char),
_get_zero_arr_size(_tmp2E7,24));});goto _LL1F2;}if(Cyc_Absyn_qvar_cmp(_tmp2E3,
_tmp2E6)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp2E8="(different type name)";
_tag_arr(_tmp2E8,sizeof(char),_get_zero_arr_size(_tmp2E8,22));});goto _LL1F2;}Cyc_Tcutil_unify_list(
_tmp254,_tmp250);return;}}_LL1FB: _tmp255=_tmp245.f1;if(_tmp255 <= (void*)3?1:*((
int*)_tmp255)!= 12)goto _LL1FD;_tmp256=((struct Cyc_Absyn_EnumType_struct*)_tmp255)->f1;
_tmp257=_tmp245.f2;if(_tmp257 <= (void*)3?1:*((int*)_tmp257)!= 12)goto _LL1FD;
_tmp258=((struct Cyc_Absyn_EnumType_struct*)_tmp257)->f1;_LL1FC: if(Cyc_Absyn_qvar_cmp(
_tmp256,_tmp258)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp2E9="(different enum types)";
_tag_arr(_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,23));});goto _LL1F2;
_LL1FD: _tmp259=_tmp245.f1;if(_tmp259 <= (void*)3?1:*((int*)_tmp259)!= 13)goto
_LL1FF;_tmp25A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp259)->f1;_tmp25B=
_tmp245.f2;if(_tmp25B <= (void*)3?1:*((int*)_tmp25B)!= 13)goto _LL1FF;_tmp25C=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp25B)->f1;_LL1FE: {int bad=0;for(0;
_tmp25A != 0?_tmp25C != 0: 0;(_tmp25A=_tmp25A->tl,_tmp25C=_tmp25C->tl)){struct Cyc_Absyn_Enumfield*
_tmp2EA=(struct Cyc_Absyn_Enumfield*)_tmp25A->hd;struct Cyc_Absyn_Enumfield*
_tmp2EB=(struct Cyc_Absyn_Enumfield*)_tmp25C->hd;if(Cyc_Absyn_qvar_cmp(_tmp2EA->name,
_tmp2EB->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp2EC="(different names for enum fields)";
_tag_arr(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,34));});bad=1;break;}
if(_tmp2EA->tag == _tmp2EB->tag)continue;if(_tmp2EA->tag == 0?1: _tmp2EB->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2ED="(different tag values for enum fields)";
_tag_arr(_tmp2ED,sizeof(char),_get_zero_arr_size(_tmp2ED,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp2EA->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp2EB->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp2EE="(different tag values for enum fields)";_tag_arr(_tmp2EE,
sizeof(char),_get_zero_arr_size(_tmp2EE,39));});bad=1;break;}}if(bad)goto _LL1F2;
if(_tmp25A == 0?_tmp25C == 0: 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp2EF="(different number of fields for enums)";_tag_arr(_tmp2EF,sizeof(char),
_get_zero_arr_size(_tmp2EF,39));});goto _LL1F2;}_LL1FF: _tmp25D=_tmp245.f1;if(
_tmp25D <= (void*)3?1:*((int*)_tmp25D)!= 2)goto _LL201;_tmp25E=((struct Cyc_Absyn_TunionType_struct*)
_tmp25D)->f1;_tmp25F=(void*)_tmp25E.tunion_info;if(*((int*)_tmp25F)!= 1)goto
_LL201;_tmp260=((struct Cyc_Absyn_KnownTunion_struct*)_tmp25F)->f1;_tmp261=*
_tmp260;_tmp262=_tmp25E.targs;_tmp263=(void*)_tmp25E.rgn;_tmp264=_tmp245.f2;if(
_tmp264 <= (void*)3?1:*((int*)_tmp264)!= 2)goto _LL201;_tmp265=((struct Cyc_Absyn_TunionType_struct*)
_tmp264)->f1;_tmp266=(void*)_tmp265.tunion_info;if(*((int*)_tmp266)!= 1)goto
_LL201;_tmp267=((struct Cyc_Absyn_KnownTunion_struct*)_tmp266)->f1;_tmp268=*
_tmp267;_tmp269=_tmp265.targs;_tmp26A=(void*)_tmp265.rgn;_LL200: if(_tmp261 == 
_tmp268?1: Cyc_Absyn_qvar_cmp(_tmp261->name,_tmp268->name)== 0){Cyc_Tcutil_unify_it(
_tmp26A,_tmp263);Cyc_Tcutil_unify_list(_tmp269,_tmp262);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp2F0="(different tunion types)";_tag_arr(_tmp2F0,sizeof(char),
_get_zero_arr_size(_tmp2F0,25));});goto _LL1F2;_LL201: _tmp26B=_tmp245.f1;if(
_tmp26B <= (void*)3?1:*((int*)_tmp26B)!= 3)goto _LL203;_tmp26C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp26B)->f1;_tmp26D=(void*)_tmp26C.field_info;if(*((int*)_tmp26D)!= 1)goto
_LL203;_tmp26E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp26D)->f1;_tmp26F=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp26D)->f2;_tmp270=_tmp26C.targs;
_tmp271=_tmp245.f2;if(_tmp271 <= (void*)3?1:*((int*)_tmp271)!= 3)goto _LL203;
_tmp272=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp271)->f1;_tmp273=(void*)
_tmp272.field_info;if(*((int*)_tmp273)!= 1)goto _LL203;_tmp274=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp273)->f1;_tmp275=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp273)->f2;
_tmp276=_tmp272.targs;_LL202: if((_tmp26E == _tmp274?1: Cyc_Absyn_qvar_cmp(_tmp26E->name,
_tmp274->name)== 0)?_tmp26F == _tmp275?1: Cyc_Absyn_qvar_cmp(_tmp26F->name,_tmp275->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp276,_tmp270);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp2F1="(different tunion field types)";_tag_arr(_tmp2F1,sizeof(char),
_get_zero_arr_size(_tmp2F1,31));});goto _LL1F2;_LL203: _tmp277=_tmp245.f1;if(
_tmp277 <= (void*)3?1:*((int*)_tmp277)!= 4)goto _LL205;_tmp278=((struct Cyc_Absyn_PointerType_struct*)
_tmp277)->f1;_tmp279=(void*)_tmp278.elt_typ;_tmp27A=(void*)_tmp278.rgn_typ;
_tmp27B=_tmp278.nullable;_tmp27C=_tmp278.tq;_tmp27D=_tmp278.bounds;_tmp27E=
_tmp278.zero_term;_tmp27F=_tmp245.f2;if(_tmp27F <= (void*)3?1:*((int*)_tmp27F)!= 
4)goto _LL205;_tmp280=((struct Cyc_Absyn_PointerType_struct*)_tmp27F)->f1;_tmp281=(
void*)_tmp280.elt_typ;_tmp282=(void*)_tmp280.rgn_typ;_tmp283=_tmp280.nullable;
_tmp284=_tmp280.tq;_tmp285=_tmp280.bounds;_tmp286=_tmp280.zero_term;_LL204: Cyc_Tcutil_unify_it(
_tmp281,_tmp279);Cyc_Tcutil_unify_it(_tmp27A,_tmp282);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp284,_tmp27C);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp285,_tmp27D,({const char*_tmp2F2="(different pointer bounds)";
_tag_arr(_tmp2F2,sizeof(char),_get_zero_arr_size(_tmp2F2,27));}));((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp286,_tmp27E,({
const char*_tmp2F3="(not both zero terminated)";_tag_arr(_tmp2F3,sizeof(char),
_get_zero_arr_size(_tmp2F3,27));}));{void*_tmp2F4=(void*)(Cyc_Absyn_compress_conref(
_tmp285))->v;void*_tmp2F5;_LL22C: if(_tmp2F4 <= (void*)1?1:*((int*)_tmp2F4)!= 0)
goto _LL22E;_tmp2F5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2F4)->f1;if((
int)_tmp2F5 != 0)goto _LL22E;_LL22D: return;_LL22E:;_LL22F: goto _LL22B;_LL22B:;}((
void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,
struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp283,
_tmp27B,({const char*_tmp2F6="(different pointer types)";_tag_arr(_tmp2F6,sizeof(
char),_get_zero_arr_size(_tmp2F6,26));}));return;_LL205: _tmp287=_tmp245.f1;if(
_tmp287 <= (void*)3?1:*((int*)_tmp287)!= 5)goto _LL207;_tmp288=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp287)->f1;_tmp289=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp287)->f2;
_tmp28A=_tmp245.f2;if(_tmp28A <= (void*)3?1:*((int*)_tmp28A)!= 5)goto _LL207;
_tmp28B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp28A)->f1;_tmp28C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp28A)->f2;_LL206: if(_tmp28B == _tmp288?_tmp28C
== _tmp289: 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp2F7="(different integral types)";
_tag_arr(_tmp2F7,sizeof(char),_get_zero_arr_size(_tmp2F7,27));});goto _LL1F2;
_LL207: _tmp28D=_tmp245.f1;if((int)_tmp28D != 1)goto _LL209;_tmp28E=_tmp245.f2;if((
int)_tmp28E != 1)goto _LL209;_LL208: return;_LL209: _tmp28F=_tmp245.f1;if(_tmp28F <= (
void*)3?1:*((int*)_tmp28F)!= 6)goto _LL20B;_tmp290=((struct Cyc_Absyn_DoubleType_struct*)
_tmp28F)->f1;_tmp291=_tmp245.f2;if(_tmp291 <= (void*)3?1:*((int*)_tmp291)!= 6)
goto _LL20B;_tmp292=((struct Cyc_Absyn_DoubleType_struct*)_tmp291)->f1;_LL20A: if(
_tmp290 == _tmp292)return;goto _LL1F2;_LL20B: _tmp293=_tmp245.f1;if(_tmp293 <= (void*)
3?1:*((int*)_tmp293)!= 14)goto _LL20D;_tmp294=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp293)->f1;_tmp295=_tmp245.f2;if(_tmp295 <= (void*)3?1:*((int*)_tmp295)!= 14)
goto _LL20D;_tmp296=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp295)->f1;
_LL20C: Cyc_Tcutil_unify_it(_tmp294,_tmp296);return;_LL20D: _tmp297=_tmp245.f1;if(
_tmp297 <= (void*)3?1:*((int*)_tmp297)!= 18)goto _LL20F;_tmp298=((struct Cyc_Absyn_TypeInt_struct*)
_tmp297)->f1;_tmp299=_tmp245.f2;if(_tmp299 <= (void*)3?1:*((int*)_tmp299)!= 18)
goto _LL20F;_tmp29A=((struct Cyc_Absyn_TypeInt_struct*)_tmp299)->f1;_LL20E: if(
_tmp298 == _tmp29A)return;Cyc_Tcutil_failure_reason=({const char*_tmp2F8="(different type integers)";
_tag_arr(_tmp2F8,sizeof(char),_get_zero_arr_size(_tmp2F8,26));});goto _LL1F2;
_LL20F: _tmp29B=_tmp245.f1;if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 17)goto
_LL211;_tmp29C=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp29B)->f1;_tmp29D=
_tmp245.f2;if(_tmp29D <= (void*)3?1:*((int*)_tmp29D)!= 17)goto _LL211;_tmp29E=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp29D)->f1;_LL210: Cyc_Tcutil_unify_it(
_tmp29C,_tmp29E);return;_LL211: _tmp29F=_tmp245.f1;if(_tmp29F <= (void*)3?1:*((int*)
_tmp29F)!= 7)goto _LL213;_tmp2A0=((struct Cyc_Absyn_ArrayType_struct*)_tmp29F)->f1;
_tmp2A1=(void*)_tmp2A0.elt_type;_tmp2A2=_tmp2A0.tq;_tmp2A3=_tmp2A0.num_elts;
_tmp2A4=_tmp2A0.zero_term;_tmp2A5=_tmp245.f2;if(_tmp2A5 <= (void*)3?1:*((int*)
_tmp2A5)!= 7)goto _LL213;_tmp2A6=((struct Cyc_Absyn_ArrayType_struct*)_tmp2A5)->f1;
_tmp2A7=(void*)_tmp2A6.elt_type;_tmp2A8=_tmp2A6.tq;_tmp2A9=_tmp2A6.num_elts;
_tmp2AA=_tmp2A6.zero_term;_LL212: Cyc_Tcutil_unify_tqual(_tmp2A8,_tmp2A2);Cyc_Tcutil_unify_it(
_tmp2A7,_tmp2A1);((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2A4,
_tmp2AA,({const char*_tmp2F9="(not both zero terminated)";_tag_arr(_tmp2F9,
sizeof(char),_get_zero_arr_size(_tmp2F9,27));}));if(_tmp2A3 == _tmp2A9)return;if(
_tmp2A3 == 0?1: _tmp2A9 == 0)goto _LL1F2;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2A3),(struct Cyc_Absyn_Exp*)_check_null(_tmp2A9)))return;Cyc_Tcutil_failure_reason=({
const char*_tmp2FA="(different array sizes)";_tag_arr(_tmp2FA,sizeof(char),
_get_zero_arr_size(_tmp2FA,24));});goto _LL1F2;_LL213: _tmp2AB=_tmp245.f1;if(
_tmp2AB <= (void*)3?1:*((int*)_tmp2AB)!= 8)goto _LL215;_tmp2AC=((struct Cyc_Absyn_FnType_struct*)
_tmp2AB)->f1;_tmp2AD=_tmp2AC.tvars;_tmp2AE=_tmp2AC.effect;_tmp2AF=(void*)_tmp2AC.ret_typ;
_tmp2B0=_tmp2AC.args;_tmp2B1=_tmp2AC.c_varargs;_tmp2B2=_tmp2AC.cyc_varargs;
_tmp2B3=_tmp2AC.rgn_po;_tmp2B4=_tmp2AC.attributes;_tmp2B5=_tmp245.f2;if(_tmp2B5
<= (void*)3?1:*((int*)_tmp2B5)!= 8)goto _LL215;_tmp2B6=((struct Cyc_Absyn_FnType_struct*)
_tmp2B5)->f1;_tmp2B7=_tmp2B6.tvars;_tmp2B8=_tmp2B6.effect;_tmp2B9=(void*)_tmp2B6.ret_typ;
_tmp2BA=_tmp2B6.args;_tmp2BB=_tmp2B6.c_varargs;_tmp2BC=_tmp2B6.cyc_varargs;
_tmp2BD=_tmp2B6.rgn_po;_tmp2BE=_tmp2B6.attributes;_LL214: {int done=0;{struct
_RegionHandle _tmp2FB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2FB;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2B7 != 0){if(_tmp2AD == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2FC="(second function type has too few type variables)";
_tag_arr(_tmp2FC,sizeof(char),_get_zero_arr_size(_tmp2FC,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp2FD=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2B7->hd);void*_tmp2FE=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2AD->hd);
if(_tmp2FD != _tmp2FE){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp303;_tmp303.tag=0;_tmp303.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2FE);{struct Cyc_String_pa_struct _tmp302;_tmp302.tag=0;_tmp302.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2FD);{struct Cyc_String_pa_struct _tmp301;
_tmp301.tag=0;_tmp301.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp2B7->hd);{void*_tmp2FF[3]={& _tmp301,& _tmp302,& _tmp303};Cyc_aprintf(({const
char*_tmp300="(type var %s has different kinds %s and %s)";_tag_arr(_tmp300,
sizeof(char),_get_zero_arr_size(_tmp300,44));}),_tag_arr(_tmp2FF,sizeof(void*),3));}}}});(
int)_throw((void*)Cyc_Tcutil_Unify);}inst=({struct Cyc_List_List*_tmp304=
_region_malloc(rgn,sizeof(*_tmp304));_tmp304->hd=({struct _tuple8*_tmp305=
_region_malloc(rgn,sizeof(*_tmp305));_tmp305->f1=(struct Cyc_Absyn_Tvar*)_tmp2AD->hd;
_tmp305->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp306=_cycalloc(sizeof(*
_tmp306));_tmp306[0]=({struct Cyc_Absyn_VarType_struct _tmp307;_tmp307.tag=1;
_tmp307.f1=(struct Cyc_Absyn_Tvar*)_tmp2B7->hd;_tmp307;});_tmp306;});_tmp305;});
_tmp304->tl=inst;_tmp304;});_tmp2B7=_tmp2B7->tl;_tmp2AD=_tmp2AD->tl;}}if(_tmp2AD
!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp308="(second function type has too many type variables)";
_tag_arr(_tmp308,sizeof(char),_get_zero_arr_size(_tmp308,51));});_npop_handler(0);
goto _LL1F2;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_Absyn_FnType_struct
_tmp30A;_tmp30A.tag=8;_tmp30A.f1=({struct Cyc_Absyn_FnInfo _tmp30B;_tmp30B.tvars=0;
_tmp30B.effect=_tmp2B8;_tmp30B.ret_typ=(void*)_tmp2B9;_tmp30B.args=_tmp2BA;
_tmp30B.c_varargs=_tmp2BB;_tmp30B.cyc_varargs=_tmp2BC;_tmp30B.rgn_po=_tmp2BD;
_tmp30B.attributes=_tmp2BE;_tmp30B;});_tmp30A;});_tmp309;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp30C=_cycalloc(sizeof(*
_tmp30C));_tmp30C[0]=({struct Cyc_Absyn_FnType_struct _tmp30D;_tmp30D.tag=8;
_tmp30D.f1=({struct Cyc_Absyn_FnInfo _tmp30E;_tmp30E.tvars=0;_tmp30E.effect=
_tmp2AE;_tmp30E.ret_typ=(void*)_tmp2AF;_tmp30E.args=_tmp2B0;_tmp30E.c_varargs=
_tmp2B1;_tmp30E.cyc_varargs=_tmp2B2;_tmp30E.rgn_po=_tmp2B3;_tmp30E.attributes=
_tmp2B4;_tmp30E;});_tmp30D;});_tmp30C;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2B9,_tmp2AF);for(0;_tmp2BA != 0?_tmp2B0 != 0: 0;(
_tmp2BA=_tmp2BA->tl,_tmp2B0=_tmp2B0->tl)){Cyc_Tcutil_unify_tqual((*((struct
_tuple2*)((struct _tuple2*)_tmp2BA->hd))).f2,(*((struct _tuple2*)((struct _tuple2*)
_tmp2B0->hd))).f2);Cyc_Tcutil_unify_it((*((struct _tuple2*)((struct _tuple2*)
_tmp2BA->hd))).f3,(*((struct _tuple2*)((struct _tuple2*)_tmp2B0->hd))).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2BA != 0?1: _tmp2B0 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp30F="(function types have different number of arguments)";_tag_arr(
_tmp30F,sizeof(char),_get_zero_arr_size(_tmp30F,52));});goto _LL1F2;}if(_tmp2BB != 
_tmp2B1){Cyc_Tcutil_failure_reason=({const char*_tmp310="(only one function type takes C varargs)";
_tag_arr(_tmp310,sizeof(char),_get_zero_arr_size(_tmp310,41));});goto _LL1F2;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp312=({struct _tuple11 _tmp311;_tmp311.f1=
_tmp2BC;_tmp311.f2=_tmp2B2;_tmp311;});struct Cyc_Absyn_VarargInfo*_tmp313;struct
Cyc_Absyn_VarargInfo*_tmp314;struct Cyc_Absyn_VarargInfo*_tmp315;struct Cyc_Absyn_VarargInfo*
_tmp316;struct Cyc_Absyn_VarargInfo*_tmp317;struct Cyc_Absyn_VarargInfo _tmp318;
struct Cyc_Core_Opt*_tmp319;struct Cyc_Absyn_Tqual _tmp31A;void*_tmp31B;int _tmp31C;
struct Cyc_Absyn_VarargInfo*_tmp31D;struct Cyc_Absyn_VarargInfo _tmp31E;struct Cyc_Core_Opt*
_tmp31F;struct Cyc_Absyn_Tqual _tmp320;void*_tmp321;int _tmp322;_LL231: _tmp313=
_tmp312.f1;if(_tmp313 != 0)goto _LL233;_tmp314=_tmp312.f2;if(_tmp314 != 0)goto
_LL233;_LL232: goto _LL230;_LL233: _tmp315=_tmp312.f1;if(_tmp315 != 0)goto _LL235;
_LL234: goto _LL236;_LL235: _tmp316=_tmp312.f2;if(_tmp316 != 0)goto _LL237;_LL236:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp323="(only one function type takes varargs)";
_tag_arr(_tmp323,sizeof(char),_get_zero_arr_size(_tmp323,39));});goto _LL230;
_LL237: _tmp317=_tmp312.f1;if(_tmp317 == 0)goto _LL230;_tmp318=*_tmp317;_tmp319=
_tmp318.name;_tmp31A=_tmp318.tq;_tmp31B=(void*)_tmp318.type;_tmp31C=_tmp318.inject;
_tmp31D=_tmp312.f2;if(_tmp31D == 0)goto _LL230;_tmp31E=*_tmp31D;_tmp31F=_tmp31E.name;
_tmp320=_tmp31E.tq;_tmp321=(void*)_tmp31E.type;_tmp322=_tmp31E.inject;_LL238: Cyc_Tcutil_unify_tqual(
_tmp31A,_tmp320);Cyc_Tcutil_unify_it(_tmp31B,_tmp321);if(_tmp31C != _tmp322){
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp324="(only one function type injects varargs)";
_tag_arr(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,41));});}goto _LL230;
_LL230:;}if(bad_cyc_vararg)goto _LL1F2;{int bad_effect=0;{struct _tuple12 _tmp326=({
struct _tuple12 _tmp325;_tmp325.f1=_tmp2B8;_tmp325.f2=_tmp2AE;_tmp325;});struct Cyc_Core_Opt*
_tmp327;struct Cyc_Core_Opt*_tmp328;struct Cyc_Core_Opt*_tmp329;struct Cyc_Core_Opt*
_tmp32A;_LL23A: _tmp327=_tmp326.f1;if(_tmp327 != 0)goto _LL23C;_tmp328=_tmp326.f2;
if(_tmp328 != 0)goto _LL23C;_LL23B: goto _LL239;_LL23C: _tmp329=_tmp326.f1;if(_tmp329
!= 0)goto _LL23E;_LL23D: goto _LL23F;_LL23E: _tmp32A=_tmp326.f2;if(_tmp32A != 0)goto
_LL240;_LL23F: bad_effect=1;goto _LL239;_LL240:;_LL241: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B8))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2AE))->v);goto _LL239;_LL239:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp32B="(function type effects do not unify)";
_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,37));});goto _LL1F2;}if(
!Cyc_Tcutil_same_atts(_tmp2B4,_tmp2BE)){Cyc_Tcutil_failure_reason=({const char*
_tmp32C="(function types have different attributes)";_tag_arr(_tmp32C,sizeof(
char),_get_zero_arr_size(_tmp32C,43));});goto _LL1F2;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2B3,_tmp2BD)){Cyc_Tcutil_failure_reason=({const char*_tmp32D="(function types have different region lifetime orderings)";
_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,58));});goto _LL1F2;}
return;}}}_LL215: _tmp2BF=_tmp245.f1;if(_tmp2BF <= (void*)3?1:*((int*)_tmp2BF)!= 9)
goto _LL217;_tmp2C0=((struct Cyc_Absyn_TupleType_struct*)_tmp2BF)->f1;_tmp2C1=
_tmp245.f2;if(_tmp2C1 <= (void*)3?1:*((int*)_tmp2C1)!= 9)goto _LL217;_tmp2C2=((
struct Cyc_Absyn_TupleType_struct*)_tmp2C1)->f1;_LL216: for(0;_tmp2C2 != 0?_tmp2C0
!= 0: 0;(_tmp2C2=_tmp2C2->tl,_tmp2C0=_tmp2C0->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)((struct _tuple4*)_tmp2C2->hd))).f1,(*((struct _tuple4*)((struct
_tuple4*)_tmp2C0->hd))).f1);Cyc_Tcutil_unify_it((*((struct _tuple4*)((struct
_tuple4*)_tmp2C2->hd))).f2,(*((struct _tuple4*)((struct _tuple4*)_tmp2C0->hd))).f2);}
if(_tmp2C2 == 0?_tmp2C0 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp32E="(tuple types have different numbers of components)";
_tag_arr(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,51));});goto _LL1F2;
_LL217: _tmp2C3=_tmp245.f1;if(_tmp2C3 <= (void*)3?1:*((int*)_tmp2C3)!= 11)goto
_LL219;_tmp2C4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2C3)->f1;
_tmp2C5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2C3)->f2;_tmp2C6=_tmp245.f2;
if(_tmp2C6 <= (void*)3?1:*((int*)_tmp2C6)!= 11)goto _LL219;_tmp2C7=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp2C6)->f1;_tmp2C8=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C6)->f2;_LL218: if(_tmp2C7 != _tmp2C4){Cyc_Tcutil_failure_reason=({const char*
_tmp32F="(struct and union type)";_tag_arr(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,24));});goto _LL1F2;}for(0;_tmp2C8 != 0?_tmp2C5 != 0: 0;(
_tmp2C8=_tmp2C8->tl,_tmp2C5=_tmp2C5->tl)){struct Cyc_Absyn_Aggrfield*_tmp330=(
struct Cyc_Absyn_Aggrfield*)_tmp2C8->hd;struct Cyc_Absyn_Aggrfield*_tmp331=(struct
Cyc_Absyn_Aggrfield*)_tmp2C5->hd;if(Cyc_strptrcmp(_tmp330->name,_tmp331->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp332="(different member names)";
_tag_arr(_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_tqual(_tmp330->tq,_tmp331->tq);Cyc_Tcutil_unify_it((
void*)_tmp330->type,(void*)_tmp331->type);if(!Cyc_Tcutil_same_atts(_tmp330->attributes,
_tmp331->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp333="(different attributes on member)";_tag_arr(_tmp333,sizeof(
char),_get_zero_arr_size(_tmp333,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if(((_tmp330->width != 0?_tmp331->width == 0: 0)?1:(_tmp331->width != 0?_tmp330->width
== 0: 0))?1:((_tmp330->width != 0?_tmp331->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp330->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp331->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp334="(different bitfield widths on member)";
_tag_arr(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp2C8 == 0?_tmp2C5 == 0: 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp335="(different number of members)";
_tag_arr(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,30));});goto _LL1F2;
_LL219: _tmp2C9=_tmp245.f1;if((int)_tmp2C9 != 2)goto _LL21B;_tmp2CA=_tmp245.f2;if((
int)_tmp2CA != 2)goto _LL21B;_LL21A: return;_LL21B: _tmp2CB=_tmp245.f1;if(_tmp2CB <= (
void*)3?1:*((int*)_tmp2CB)!= 15)goto _LL21D;_tmp2CC=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp2CB)->f1;_tmp2CD=_tmp245.f2;if(_tmp2CD <= (void*)3?1:*((int*)_tmp2CD)!= 15)
goto _LL21D;_tmp2CE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2CD)->f1;
_LL21C: Cyc_Tcutil_unify_it(_tmp2CC,_tmp2CE);return;_LL21D: _tmp2CF=_tmp245.f1;if(
_tmp2CF <= (void*)3?1:*((int*)_tmp2CF)!= 20)goto _LL21F;_LL21E: goto _LL220;_LL21F:
_tmp2D0=_tmp245.f2;if(_tmp2D0 <= (void*)3?1:*((int*)_tmp2D0)!= 20)goto _LL221;
_LL220: goto _LL222;_LL221: _tmp2D1=_tmp245.f1;if(_tmp2D1 <= (void*)3?1:*((int*)
_tmp2D1)!= 19)goto _LL223;_LL222: goto _LL224;_LL223: _tmp2D2=_tmp245.f1;if(_tmp2D2
<= (void*)3?1:*((int*)_tmp2D2)!= 21)goto _LL225;_LL224: goto _LL226;_LL225: _tmp2D3=
_tmp245.f2;if(_tmp2D3 <= (void*)3?1:*((int*)_tmp2D3)!= 21)goto _LL227;_LL226: goto
_LL228;_LL227: _tmp2D4=_tmp245.f2;if(_tmp2D4 <= (void*)3?1:*((int*)_tmp2D4)!= 19)
goto _LL229;_LL228: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp336="(effects don't unify)";_tag_arr(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,22));});goto _LL1F2;_LL229:;_LL22A: goto _LL1F2;_LL1F2:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0:
0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp337=(tq1.q_const + (tq1.q_volatile
<< 1))+ (tq1.q_restrict << 2);int _tmp338=(tq2.q_const + (tq2.q_volatile << 1))+ (
tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp337,_tmp338);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp339=(void*)x->v;
void*_tmp33A;_LL243: if((int)_tmp339 != 0)goto _LL245;_LL244: return - 1;_LL245: if(
_tmp339 <= (void*)1?1:*((int*)_tmp339)!= 0)goto _LL247;_tmp33A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp339)->f1;_LL246: {void*_tmp33B=(void*)y->v;void*_tmp33C;_LL24A: if((int)
_tmp33B != 0)goto _LL24C;_LL24B: return 1;_LL24C: if(_tmp33B <= (void*)1?1:*((int*)
_tmp33B)!= 0)goto _LL24E;_tmp33C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp33B)->f1;_LL24D: return cmp(_tmp33A,_tmp33C);_LL24E: if(_tmp33B <= (void*)1?1:*((
int*)_tmp33B)!= 1)goto _LL249;_LL24F:({void*_tmp33D[0]={};Cyc_Tcutil_impos(({
const char*_tmp33E="unify_conref: forward after compress(2)";_tag_arr(_tmp33E,
sizeof(char),_get_zero_arr_size(_tmp33E,40));}),_tag_arr(_tmp33D,sizeof(void*),0));});
_LL249:;}_LL247: if(_tmp339 <= (void*)1?1:*((int*)_tmp339)!= 1)goto _LL242;_LL248:({
void*_tmp33F[0]={};Cyc_Tcutil_impos(({const char*_tmp340="unify_conref: forward after compress";
_tag_arr(_tmp340,sizeof(char),_get_zero_arr_size(_tmp340,37));}),_tag_arr(
_tmp33F,sizeof(void*),0));});_LL242:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp342;struct Cyc_Absyn_Tqual
_tmp343;void*_tmp344;struct _tuple4*_tmp341=tqt1;_tmp342=*_tmp341;_tmp343=_tmp342.f1;
_tmp344=_tmp342.f2;{struct _tuple4 _tmp346;struct Cyc_Absyn_Tqual _tmp347;void*
_tmp348;struct _tuple4*_tmp345=tqt2;_tmp346=*_tmp345;_tmp347=_tmp346.f1;_tmp348=
_tmp346.f2;{int _tmp349=Cyc_Tcutil_tqual_cmp(_tmp343,_tmp347);if(_tmp349 != 0)
return _tmp349;return Cyc_Tcutil_typecmp(_tmp344,_tmp348);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp34A=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp34A != 0)return _tmp34A;{int _tmp34B=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp34B != 0)return _tmp34B;{int _tmp34C=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp34C != 0)return _tmp34C;{int _tmp34D=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp34D != 0)return
_tmp34D;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp34E=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp34E != 0)return _tmp34E;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp34F=t;_LL251: if((int)_tmp34F != 0)goto _LL253;_LL252: return 0;
_LL253: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 0)goto _LL255;_LL254: return 1;
_LL255: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 1)goto _LL257;_LL256: return 2;
_LL257: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 2)goto _LL259;_LL258: return 3;
_LL259: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 3)goto _LL25B;_LL25A: return 4;
_LL25B: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 4)goto _LL25D;_LL25C: return 5;
_LL25D: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 5)goto _LL25F;_LL25E: return 6;
_LL25F: if((int)_tmp34F != 1)goto _LL261;_LL260: return 7;_LL261: if(_tmp34F <= (void*)
3?1:*((int*)_tmp34F)!= 6)goto _LL263;_LL262: return 8;_LL263: if(_tmp34F <= (void*)3?
1:*((int*)_tmp34F)!= 7)goto _LL265;_LL264: return 9;_LL265: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 8)goto _LL267;_LL266: return 10;_LL267: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 9)goto _LL269;_LL268: return 11;_LL269: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 10)goto _LL26B;_LL26A: return 12;_LL26B: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 11)goto _LL26D;_LL26C: return 14;_LL26D: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 12)goto _LL26F;_LL26E: return 16;_LL26F: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 13)goto _LL271;_LL270: return 17;_LL271: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 15)goto _LL273;_LL272: return 18;_LL273: if(_tmp34F <= (void*)3?1:*((
int*)_tmp34F)!= 16)goto _LL275;_LL274: return 19;_LL275: if((int)_tmp34F != 2)goto
_LL277;_LL276: return 20;_LL277: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 19)goto
_LL279;_LL278: return 21;_LL279: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 20)goto
_LL27B;_LL27A: return 22;_LL27B: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 21)goto
_LL27D;_LL27C: return 23;_LL27D: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 14)goto
_LL27F;_LL27E: return 24;_LL27F: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 18)goto
_LL281;_LL280: return 25;_LL281: if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 17)goto
_LL250;_LL282: return 26;_LL250:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp350=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp350 != 0)
return _tmp350;{struct _tuple6 _tmp352=({struct _tuple6 _tmp351;_tmp351.f1=t2;_tmp351.f2=
t1;_tmp351;});void*_tmp353;void*_tmp354;void*_tmp355;void*_tmp356;void*_tmp357;
struct Cyc_Absyn_Tvar*_tmp358;void*_tmp359;struct Cyc_Absyn_Tvar*_tmp35A;void*
_tmp35B;struct Cyc_Absyn_AggrInfo _tmp35C;void*_tmp35D;struct Cyc_List_List*_tmp35E;
void*_tmp35F;struct Cyc_Absyn_AggrInfo _tmp360;void*_tmp361;struct Cyc_List_List*
_tmp362;void*_tmp363;struct _tuple1*_tmp364;void*_tmp365;struct _tuple1*_tmp366;
void*_tmp367;struct Cyc_List_List*_tmp368;void*_tmp369;struct Cyc_List_List*
_tmp36A;void*_tmp36B;struct Cyc_Absyn_TunionInfo _tmp36C;void*_tmp36D;struct Cyc_Absyn_Tuniondecl**
_tmp36E;struct Cyc_Absyn_Tuniondecl*_tmp36F;struct Cyc_List_List*_tmp370;void*
_tmp371;void*_tmp372;struct Cyc_Absyn_TunionInfo _tmp373;void*_tmp374;struct Cyc_Absyn_Tuniondecl**
_tmp375;struct Cyc_Absyn_Tuniondecl*_tmp376;struct Cyc_List_List*_tmp377;void*
_tmp378;void*_tmp379;struct Cyc_Absyn_TunionFieldInfo _tmp37A;void*_tmp37B;struct
Cyc_Absyn_Tuniondecl*_tmp37C;struct Cyc_Absyn_Tunionfield*_tmp37D;struct Cyc_List_List*
_tmp37E;void*_tmp37F;struct Cyc_Absyn_TunionFieldInfo _tmp380;void*_tmp381;struct
Cyc_Absyn_Tuniondecl*_tmp382;struct Cyc_Absyn_Tunionfield*_tmp383;struct Cyc_List_List*
_tmp384;void*_tmp385;struct Cyc_Absyn_PtrInfo _tmp386;void*_tmp387;void*_tmp388;
struct Cyc_Absyn_Conref*_tmp389;struct Cyc_Absyn_Tqual _tmp38A;struct Cyc_Absyn_Conref*
_tmp38B;struct Cyc_Absyn_Conref*_tmp38C;void*_tmp38D;struct Cyc_Absyn_PtrInfo
_tmp38E;void*_tmp38F;void*_tmp390;struct Cyc_Absyn_Conref*_tmp391;struct Cyc_Absyn_Tqual
_tmp392;struct Cyc_Absyn_Conref*_tmp393;struct Cyc_Absyn_Conref*_tmp394;void*
_tmp395;void*_tmp396;void*_tmp397;void*_tmp398;void*_tmp399;void*_tmp39A;void*
_tmp39B;void*_tmp39C;void*_tmp39D;int _tmp39E;void*_tmp39F;int _tmp3A0;void*
_tmp3A1;struct Cyc_Absyn_ArrayInfo _tmp3A2;void*_tmp3A3;struct Cyc_Absyn_Tqual
_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Conref*_tmp3A6;void*_tmp3A7;
struct Cyc_Absyn_ArrayInfo _tmp3A8;void*_tmp3A9;struct Cyc_Absyn_Tqual _tmp3AA;
struct Cyc_Absyn_Exp*_tmp3AB;struct Cyc_Absyn_Conref*_tmp3AC;void*_tmp3AD;struct
Cyc_Absyn_FnInfo _tmp3AE;struct Cyc_List_List*_tmp3AF;struct Cyc_Core_Opt*_tmp3B0;
void*_tmp3B1;struct Cyc_List_List*_tmp3B2;int _tmp3B3;struct Cyc_Absyn_VarargInfo*
_tmp3B4;struct Cyc_List_List*_tmp3B5;struct Cyc_List_List*_tmp3B6;void*_tmp3B7;
struct Cyc_Absyn_FnInfo _tmp3B8;struct Cyc_List_List*_tmp3B9;struct Cyc_Core_Opt*
_tmp3BA;void*_tmp3BB;struct Cyc_List_List*_tmp3BC;int _tmp3BD;struct Cyc_Absyn_VarargInfo*
_tmp3BE;struct Cyc_List_List*_tmp3BF;struct Cyc_List_List*_tmp3C0;void*_tmp3C1;
struct Cyc_List_List*_tmp3C2;void*_tmp3C3;struct Cyc_List_List*_tmp3C4;void*
_tmp3C5;void*_tmp3C6;struct Cyc_List_List*_tmp3C7;void*_tmp3C8;void*_tmp3C9;
struct Cyc_List_List*_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*_tmp3CD;void*_tmp3CE;
void*_tmp3CF;void*_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;
void*_tmp3D5;void*_tmp3D6;void*_tmp3D7;void*_tmp3D8;void*_tmp3D9;int _tmp3DA;void*
_tmp3DB;int _tmp3DC;void*_tmp3DD;void*_tmp3DE;void*_tmp3DF;void*_tmp3E0;void*
_tmp3E1;void*_tmp3E2;_LL284: _tmp353=_tmp352.f1;if(_tmp353 <= (void*)3?1:*((int*)
_tmp353)!= 0)goto _LL286;_tmp354=_tmp352.f2;if(_tmp354 <= (void*)3?1:*((int*)
_tmp354)!= 0)goto _LL286;_LL285:({void*_tmp3E3[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3E4="typecmp: can only compare closed types";
_tag_arr(_tmp3E4,sizeof(char),_get_zero_arr_size(_tmp3E4,39));}),_tag_arr(
_tmp3E3,sizeof(void*),0));});_LL286: _tmp355=_tmp352.f1;if((int)_tmp355 != 0)goto
_LL288;_tmp356=_tmp352.f2;if((int)_tmp356 != 0)goto _LL288;_LL287: return 0;_LL288:
_tmp357=_tmp352.f1;if(_tmp357 <= (void*)3?1:*((int*)_tmp357)!= 1)goto _LL28A;
_tmp358=((struct Cyc_Absyn_VarType_struct*)_tmp357)->f1;_tmp359=_tmp352.f2;if(
_tmp359 <= (void*)3?1:*((int*)_tmp359)!= 1)goto _LL28A;_tmp35A=((struct Cyc_Absyn_VarType_struct*)
_tmp359)->f1;_LL289: return Cyc_Core_intcmp(*((int*)_check_null(_tmp35A->identity)),*((
int*)_check_null(_tmp358->identity)));_LL28A: _tmp35B=_tmp352.f1;if(_tmp35B <= (
void*)3?1:*((int*)_tmp35B)!= 10)goto _LL28C;_tmp35C=((struct Cyc_Absyn_AggrType_struct*)
_tmp35B)->f1;_tmp35D=(void*)_tmp35C.aggr_info;_tmp35E=_tmp35C.targs;_tmp35F=
_tmp352.f2;if(_tmp35F <= (void*)3?1:*((int*)_tmp35F)!= 10)goto _LL28C;_tmp360=((
struct Cyc_Absyn_AggrType_struct*)_tmp35F)->f1;_tmp361=(void*)_tmp360.aggr_info;
_tmp362=_tmp360.targs;_LL28B: {struct _tuple1*_tmp3E6;struct _tuple5 _tmp3E5=Cyc_Absyn_aggr_kinded_name(
_tmp35D);_tmp3E6=_tmp3E5.f2;{struct _tuple1*_tmp3E8;struct _tuple5 _tmp3E7=Cyc_Absyn_aggr_kinded_name(
_tmp361);_tmp3E8=_tmp3E7.f2;{int _tmp3E9=Cyc_Absyn_qvar_cmp(_tmp3E6,_tmp3E8);if(
_tmp3E9 != 0)return _tmp3E9;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp35E,_tmp362);}}}}_LL28C: _tmp363=_tmp352.f1;if(_tmp363 <= (void*)3?1:*((int*)
_tmp363)!= 12)goto _LL28E;_tmp364=((struct Cyc_Absyn_EnumType_struct*)_tmp363)->f1;
_tmp365=_tmp352.f2;if(_tmp365 <= (void*)3?1:*((int*)_tmp365)!= 12)goto _LL28E;
_tmp366=((struct Cyc_Absyn_EnumType_struct*)_tmp365)->f1;_LL28D: return Cyc_Absyn_qvar_cmp(
_tmp364,_tmp366);_LL28E: _tmp367=_tmp352.f1;if(_tmp367 <= (void*)3?1:*((int*)
_tmp367)!= 13)goto _LL290;_tmp368=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp367)->f1;
_tmp369=_tmp352.f2;if(_tmp369 <= (void*)3?1:*((int*)_tmp369)!= 13)goto _LL290;
_tmp36A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp369)->f1;_LL28F: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp368,
_tmp36A);_LL290: _tmp36B=_tmp352.f1;if(_tmp36B <= (void*)3?1:*((int*)_tmp36B)!= 2)
goto _LL292;_tmp36C=((struct Cyc_Absyn_TunionType_struct*)_tmp36B)->f1;_tmp36D=(
void*)_tmp36C.tunion_info;if(*((int*)_tmp36D)!= 1)goto _LL292;_tmp36E=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp36D)->f1;_tmp36F=*_tmp36E;_tmp370=_tmp36C.targs;_tmp371=(void*)_tmp36C.rgn;
_tmp372=_tmp352.f2;if(_tmp372 <= (void*)3?1:*((int*)_tmp372)!= 2)goto _LL292;
_tmp373=((struct Cyc_Absyn_TunionType_struct*)_tmp372)->f1;_tmp374=(void*)_tmp373.tunion_info;
if(*((int*)_tmp374)!= 1)goto _LL292;_tmp375=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp374)->f1;_tmp376=*_tmp375;_tmp377=_tmp373.targs;_tmp378=(void*)_tmp373.rgn;
_LL291: if(_tmp376 == _tmp36F)return 0;{int _tmp3EA=Cyc_Absyn_qvar_cmp(_tmp376->name,
_tmp36F->name);if(_tmp3EA != 0)return _tmp3EA;{int _tmp3EB=Cyc_Tcutil_typecmp(
_tmp378,_tmp371);if(_tmp3EB != 0)return _tmp3EB;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp377,_tmp370);}}_LL292: _tmp379=_tmp352.f1;if(_tmp379 <= (void*)3?1:*((int*)
_tmp379)!= 3)goto _LL294;_tmp37A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp379)->f1;_tmp37B=(void*)_tmp37A.field_info;if(*((int*)_tmp37B)!= 1)goto
_LL294;_tmp37C=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp37B)->f1;_tmp37D=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp37B)->f2;_tmp37E=_tmp37A.targs;
_tmp37F=_tmp352.f2;if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 3)goto _LL294;
_tmp380=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp37F)->f1;_tmp381=(void*)
_tmp380.field_info;if(*((int*)_tmp381)!= 1)goto _LL294;_tmp382=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp381)->f1;_tmp383=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp381)->f2;
_tmp384=_tmp380.targs;_LL293: if(_tmp382 == _tmp37C)return 0;{int _tmp3EC=Cyc_Absyn_qvar_cmp(
_tmp37C->name,_tmp382->name);if(_tmp3EC != 0)return _tmp3EC;{int _tmp3ED=Cyc_Absyn_qvar_cmp(
_tmp37D->name,_tmp383->name);if(_tmp3ED != 0)return _tmp3ED;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp384,_tmp37E);}}_LL294: _tmp385=_tmp352.f1;if(_tmp385 <= (
void*)3?1:*((int*)_tmp385)!= 4)goto _LL296;_tmp386=((struct Cyc_Absyn_PointerType_struct*)
_tmp385)->f1;_tmp387=(void*)_tmp386.elt_typ;_tmp388=(void*)_tmp386.rgn_typ;
_tmp389=_tmp386.nullable;_tmp38A=_tmp386.tq;_tmp38B=_tmp386.bounds;_tmp38C=
_tmp386.zero_term;_tmp38D=_tmp352.f2;if(_tmp38D <= (void*)3?1:*((int*)_tmp38D)!= 
4)goto _LL296;_tmp38E=((struct Cyc_Absyn_PointerType_struct*)_tmp38D)->f1;_tmp38F=(
void*)_tmp38E.elt_typ;_tmp390=(void*)_tmp38E.rgn_typ;_tmp391=_tmp38E.nullable;
_tmp392=_tmp38E.tq;_tmp393=_tmp38E.bounds;_tmp394=_tmp38E.zero_term;_LL295: {int
_tmp3EE=Cyc_Tcutil_typecmp(_tmp38F,_tmp387);if(_tmp3EE != 0)return _tmp3EE;{int
_tmp3EF=Cyc_Tcutil_typecmp(_tmp390,_tmp388);if(_tmp3EF != 0)return _tmp3EF;{int
_tmp3F0=Cyc_Tcutil_tqual_cmp(_tmp392,_tmp38A);if(_tmp3F0 != 0)return _tmp3F0;{int
_tmp3F1=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,_tmp393,_tmp38B);if(_tmp3F1
!= 0)return _tmp3F1;{int _tmp3F2=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp394,
_tmp38C);if(_tmp3F2 != 0)return _tmp3F2;{void*_tmp3F3=(void*)(Cyc_Absyn_compress_conref(
_tmp393))->v;void*_tmp3F4;_LL2BD: if(_tmp3F3 <= (void*)1?1:*((int*)_tmp3F3)!= 0)
goto _LL2BF;_tmp3F4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp3F3)->f1;if((
int)_tmp3F4 != 0)goto _LL2BF;_LL2BE: return 0;_LL2BF:;_LL2C0: goto _LL2BC;_LL2BC:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp391,_tmp389);}}}}}_LL296: _tmp395=
_tmp352.f1;if(_tmp395 <= (void*)3?1:*((int*)_tmp395)!= 5)goto _LL298;_tmp396=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp395)->f1;_tmp397=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp395)->f2;_tmp398=_tmp352.f2;if(_tmp398 <= (void*)3?1:*((int*)_tmp398)!= 5)
goto _LL298;_tmp399=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp398)->f1;_tmp39A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp398)->f2;_LL297: if(_tmp399 != _tmp396)
return Cyc_Core_intcmp((int)((unsigned int)_tmp399),(int)((unsigned int)_tmp396));
if(_tmp39A != _tmp397)return Cyc_Core_intcmp((int)((unsigned int)_tmp39A),(int)((
unsigned int)_tmp397));return 0;_LL298: _tmp39B=_tmp352.f1;if((int)_tmp39B != 1)
goto _LL29A;_tmp39C=_tmp352.f2;if((int)_tmp39C != 1)goto _LL29A;_LL299: return 0;
_LL29A: _tmp39D=_tmp352.f1;if(_tmp39D <= (void*)3?1:*((int*)_tmp39D)!= 6)goto
_LL29C;_tmp39E=((struct Cyc_Absyn_DoubleType_struct*)_tmp39D)->f1;_tmp39F=_tmp352.f2;
if(_tmp39F <= (void*)3?1:*((int*)_tmp39F)!= 6)goto _LL29C;_tmp3A0=((struct Cyc_Absyn_DoubleType_struct*)
_tmp39F)->f1;_LL29B: if(_tmp39E == _tmp3A0)return 0;else{if(_tmp39E)return - 1;else{
return 1;}}_LL29C: _tmp3A1=_tmp352.f1;if(_tmp3A1 <= (void*)3?1:*((int*)_tmp3A1)!= 7)
goto _LL29E;_tmp3A2=((struct Cyc_Absyn_ArrayType_struct*)_tmp3A1)->f1;_tmp3A3=(
void*)_tmp3A2.elt_type;_tmp3A4=_tmp3A2.tq;_tmp3A5=_tmp3A2.num_elts;_tmp3A6=
_tmp3A2.zero_term;_tmp3A7=_tmp352.f2;if(_tmp3A7 <= (void*)3?1:*((int*)_tmp3A7)!= 
7)goto _LL29E;_tmp3A8=((struct Cyc_Absyn_ArrayType_struct*)_tmp3A7)->f1;_tmp3A9=(
void*)_tmp3A8.elt_type;_tmp3AA=_tmp3A8.tq;_tmp3AB=_tmp3A8.num_elts;_tmp3AC=
_tmp3A8.zero_term;_LL29D: {int _tmp3F5=Cyc_Tcutil_tqual_cmp(_tmp3AA,_tmp3A4);if(
_tmp3F5 != 0)return _tmp3F5;{int _tmp3F6=Cyc_Tcutil_typecmp(_tmp3A9,_tmp3A3);if(
_tmp3F6 != 0)return _tmp3F6;{int _tmp3F7=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3A6,
_tmp3AC);if(_tmp3F7 != 0)return _tmp3F7;if(_tmp3A5 == _tmp3AB)return 0;if(_tmp3A5 == 
0?1: _tmp3AB == 0)({void*_tmp3F8[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3F9="missing expression in array index";
_tag_arr(_tmp3F9,sizeof(char),_get_zero_arr_size(_tmp3F9,34));}),_tag_arr(
_tmp3F8,sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,_tmp3A5,_tmp3AB);}}}_LL29E: _tmp3AD=_tmp352.f1;if(_tmp3AD
<= (void*)3?1:*((int*)_tmp3AD)!= 8)goto _LL2A0;_tmp3AE=((struct Cyc_Absyn_FnType_struct*)
_tmp3AD)->f1;_tmp3AF=_tmp3AE.tvars;_tmp3B0=_tmp3AE.effect;_tmp3B1=(void*)_tmp3AE.ret_typ;
_tmp3B2=_tmp3AE.args;_tmp3B3=_tmp3AE.c_varargs;_tmp3B4=_tmp3AE.cyc_varargs;
_tmp3B5=_tmp3AE.rgn_po;_tmp3B6=_tmp3AE.attributes;_tmp3B7=_tmp352.f2;if(_tmp3B7
<= (void*)3?1:*((int*)_tmp3B7)!= 8)goto _LL2A0;_tmp3B8=((struct Cyc_Absyn_FnType_struct*)
_tmp3B7)->f1;_tmp3B9=_tmp3B8.tvars;_tmp3BA=_tmp3B8.effect;_tmp3BB=(void*)_tmp3B8.ret_typ;
_tmp3BC=_tmp3B8.args;_tmp3BD=_tmp3B8.c_varargs;_tmp3BE=_tmp3B8.cyc_varargs;
_tmp3BF=_tmp3B8.rgn_po;_tmp3C0=_tmp3B8.attributes;_LL29F:({void*_tmp3FA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp3FB="typecmp: function types not handled";_tag_arr(_tmp3FB,sizeof(char),
_get_zero_arr_size(_tmp3FB,36));}),_tag_arr(_tmp3FA,sizeof(void*),0));});_LL2A0:
_tmp3C1=_tmp352.f1;if(_tmp3C1 <= (void*)3?1:*((int*)_tmp3C1)!= 9)goto _LL2A2;
_tmp3C2=((struct Cyc_Absyn_TupleType_struct*)_tmp3C1)->f1;_tmp3C3=_tmp352.f2;if(
_tmp3C3 <= (void*)3?1:*((int*)_tmp3C3)!= 9)goto _LL2A2;_tmp3C4=((struct Cyc_Absyn_TupleType_struct*)
_tmp3C3)->f1;_LL2A1: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp3C4,_tmp3C2);_LL2A2: _tmp3C5=_tmp352.f1;if(_tmp3C5 <= (void*)3?1:*((int*)
_tmp3C5)!= 11)goto _LL2A4;_tmp3C6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3C5)->f1;_tmp3C7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3C5)->f2;_tmp3C8=
_tmp352.f2;if(_tmp3C8 <= (void*)3?1:*((int*)_tmp3C8)!= 11)goto _LL2A4;_tmp3C9=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3C8)->f1;_tmp3CA=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3C8)->f2;_LL2A3: if(_tmp3C9 != _tmp3C6){if(_tmp3C9 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp3CA,_tmp3C7);_LL2A4: _tmp3CB=_tmp352.f1;if((int)_tmp3CB != 2)goto _LL2A6;
_tmp3CC=_tmp352.f2;if((int)_tmp3CC != 2)goto _LL2A6;_LL2A5: return 0;_LL2A6: _tmp3CD=
_tmp352.f1;if(_tmp3CD <= (void*)3?1:*((int*)_tmp3CD)!= 15)goto _LL2A8;_tmp3CE=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3CD)->f1;_tmp3CF=_tmp352.f2;
if(_tmp3CF <= (void*)3?1:*((int*)_tmp3CF)!= 15)goto _LL2A8;_tmp3D0=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3CF)->f1;_LL2A7: return Cyc_Tcutil_typecmp(
_tmp3CE,_tmp3D0);_LL2A8: _tmp3D1=_tmp352.f1;if(_tmp3D1 <= (void*)3?1:*((int*)
_tmp3D1)!= 14)goto _LL2AA;_tmp3D2=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp3D1)->f1;_tmp3D3=_tmp352.f2;if(_tmp3D3 <= (void*)3?1:*((int*)_tmp3D3)!= 14)
goto _LL2AA;_tmp3D4=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp3D3)->f1;
_LL2A9: return Cyc_Tcutil_typecmp(_tmp3D2,_tmp3D4);_LL2AA: _tmp3D5=_tmp352.f1;if(
_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 17)goto _LL2AC;_tmp3D6=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp3D5)->f1;_tmp3D7=_tmp352.f2;if(_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 17)
goto _LL2AC;_tmp3D8=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp3D7)->f1;_LL2AB:
return Cyc_Tcutil_typecmp(_tmp3D6,_tmp3D8);_LL2AC: _tmp3D9=_tmp352.f1;if(_tmp3D9 <= (
void*)3?1:*((int*)_tmp3D9)!= 18)goto _LL2AE;_tmp3DA=((struct Cyc_Absyn_TypeInt_struct*)
_tmp3D9)->f1;_tmp3DB=_tmp352.f2;if(_tmp3DB <= (void*)3?1:*((int*)_tmp3DB)!= 18)
goto _LL2AE;_tmp3DC=((struct Cyc_Absyn_TypeInt_struct*)_tmp3DB)->f1;_LL2AD: return
Cyc_Core_intcmp(_tmp3DA,_tmp3DC);_LL2AE: _tmp3DD=_tmp352.f1;if(_tmp3DD <= (void*)3?
1:*((int*)_tmp3DD)!= 20)goto _LL2B0;_LL2AF: goto _LL2B1;_LL2B0: _tmp3DE=_tmp352.f2;
if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 20)goto _LL2B2;_LL2B1: goto _LL2B3;
_LL2B2: _tmp3DF=_tmp352.f1;if(_tmp3DF <= (void*)3?1:*((int*)_tmp3DF)!= 19)goto
_LL2B4;_LL2B3: goto _LL2B5;_LL2B4: _tmp3E0=_tmp352.f1;if(_tmp3E0 <= (void*)3?1:*((
int*)_tmp3E0)!= 21)goto _LL2B6;_LL2B5: goto _LL2B7;_LL2B6: _tmp3E1=_tmp352.f2;if(
_tmp3E1 <= (void*)3?1:*((int*)_tmp3E1)!= 21)goto _LL2B8;_LL2B7: goto _LL2B9;_LL2B8:
_tmp3E2=_tmp352.f2;if(_tmp3E2 <= (void*)3?1:*((int*)_tmp3E2)!= 19)goto _LL2BA;
_LL2B9:({void*_tmp3FC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp3FD="typecmp: effects not handled";_tag_arr(
_tmp3FD,sizeof(char),_get_zero_arr_size(_tmp3FD,29));}),_tag_arr(_tmp3FC,sizeof(
void*),0));});_LL2BA:;_LL2BB:({void*_tmp3FE[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp3FF="Unmatched case in typecmp";
_tag_arr(_tmp3FF,sizeof(char),_get_zero_arr_size(_tmp3FF,26));}),_tag_arr(
_tmp3FE,sizeof(void*),0));});_LL283:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp400=Cyc_Tcutil_compress(t);_LL2C2: if(_tmp400 <= (void*)3?1:*((int*)
_tmp400)!= 5)goto _LL2C4;_LL2C3: goto _LL2C5;_LL2C4: if((int)_tmp400 != 1)goto _LL2C6;
_LL2C5: goto _LL2C7;_LL2C6: if(_tmp400 <= (void*)3?1:*((int*)_tmp400)!= 6)goto _LL2C8;
_LL2C7: goto _LL2C9;_LL2C8: if(_tmp400 <= (void*)3?1:*((int*)_tmp400)!= 12)goto
_LL2CA;_LL2C9: goto _LL2CB;_LL2CA: if(_tmp400 <= (void*)3?1:*((int*)_tmp400)!= 13)
goto _LL2CC;_LL2CB: return 1;_LL2CC:;_LL2CD: return 0;_LL2C1:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp402=({struct _tuple6 _tmp401;_tmp401.f1=Cyc_Tcutil_compress(
t1);_tmp401.f2=Cyc_Tcutil_compress(t2);_tmp401;});void*_tmp403;int _tmp404;void*
_tmp405;int _tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;void*
_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;void*
_tmp411;void*_tmp412;void*_tmp413;void*_tmp414;void*_tmp415;void*_tmp416;void*
_tmp417;void*_tmp418;void*_tmp419;void*_tmp41A;void*_tmp41B;void*_tmp41C;void*
_tmp41D;void*_tmp41E;void*_tmp41F;void*_tmp420;void*_tmp421;void*_tmp422;void*
_tmp423;void*_tmp424;void*_tmp425;void*_tmp426;void*_tmp427;void*_tmp428;void*
_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;void*_tmp42D;void*_tmp42E;void*
_tmp42F;void*_tmp430;void*_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;void*
_tmp435;_LL2CF: _tmp403=_tmp402.f1;if(_tmp403 <= (void*)3?1:*((int*)_tmp403)!= 6)
goto _LL2D1;_tmp404=((struct Cyc_Absyn_DoubleType_struct*)_tmp403)->f1;_tmp405=
_tmp402.f2;if(_tmp405 <= (void*)3?1:*((int*)_tmp405)!= 6)goto _LL2D1;_tmp406=((
struct Cyc_Absyn_DoubleType_struct*)_tmp405)->f1;_LL2D0: return !_tmp406?_tmp404: 0;
_LL2D1: _tmp407=_tmp402.f1;if(_tmp407 <= (void*)3?1:*((int*)_tmp407)!= 6)goto
_LL2D3;_tmp408=_tmp402.f2;if((int)_tmp408 != 1)goto _LL2D3;_LL2D2: goto _LL2D4;
_LL2D3: _tmp409=_tmp402.f1;if(_tmp409 <= (void*)3?1:*((int*)_tmp409)!= 6)goto
_LL2D5;_tmp40A=_tmp402.f2;if(_tmp40A <= (void*)3?1:*((int*)_tmp40A)!= 5)goto
_LL2D5;_LL2D4: goto _LL2D6;_LL2D5: _tmp40B=_tmp402.f1;if(_tmp40B <= (void*)3?1:*((
int*)_tmp40B)!= 6)goto _LL2D7;_tmp40C=_tmp402.f2;if(_tmp40C <= (void*)3?1:*((int*)
_tmp40C)!= 14)goto _LL2D7;_LL2D6: goto _LL2D8;_LL2D7: _tmp40D=_tmp402.f1;if((int)
_tmp40D != 1)goto _LL2D9;_tmp40E=_tmp402.f2;if(_tmp40E <= (void*)3?1:*((int*)
_tmp40E)!= 14)goto _LL2D9;_LL2D8: goto _LL2DA;_LL2D9: _tmp40F=_tmp402.f1;if(_tmp40F
<= (void*)3?1:*((int*)_tmp40F)!= 6)goto _LL2DB;_tmp410=_tmp402.f2;if(_tmp410 <= (
void*)3?1:*((int*)_tmp410)!= 17)goto _LL2DB;_LL2DA: goto _LL2DC;_LL2DB: _tmp411=
_tmp402.f1;if((int)_tmp411 != 1)goto _LL2DD;_tmp412=_tmp402.f2;if(_tmp412 <= (void*)
3?1:*((int*)_tmp412)!= 17)goto _LL2DD;_LL2DC: goto _LL2DE;_LL2DD: _tmp413=_tmp402.f1;
if((int)_tmp413 != 1)goto _LL2DF;_tmp414=_tmp402.f2;if(_tmp414 <= (void*)3?1:*((int*)
_tmp414)!= 5)goto _LL2DF;_LL2DE: return 1;_LL2DF: _tmp415=_tmp402.f1;if(_tmp415 <= (
void*)3?1:*((int*)_tmp415)!= 5)goto _LL2E1;_tmp416=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp415)->f2;if((int)_tmp416 != 3)goto _LL2E1;_tmp417=_tmp402.f2;if(_tmp417 <= (
void*)3?1:*((int*)_tmp417)!= 5)goto _LL2E1;_tmp418=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp417)->f2;if((int)_tmp418 != 3)goto _LL2E1;_LL2E0: return 0;_LL2E1: _tmp419=
_tmp402.f1;if(_tmp419 <= (void*)3?1:*((int*)_tmp419)!= 5)goto _LL2E3;_tmp41A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp419)->f2;if((int)_tmp41A != 3)goto _LL2E3;
_LL2E2: goto _LL2E4;_LL2E3: _tmp41B=_tmp402.f1;if(_tmp41B <= (void*)3?1:*((int*)
_tmp41B)!= 5)goto _LL2E5;_tmp41C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41B)->f2;
if((int)_tmp41C != 2)goto _LL2E5;_tmp41D=_tmp402.f2;if((int)_tmp41D != 1)goto _LL2E5;
_LL2E4: goto _LL2E6;_LL2E5: _tmp41E=_tmp402.f1;if(_tmp41E <= (void*)3?1:*((int*)
_tmp41E)!= 5)goto _LL2E7;_tmp41F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41E)->f2;
if((int)_tmp41F != 2)goto _LL2E7;_tmp420=_tmp402.f2;if(_tmp420 <= (void*)3?1:*((int*)
_tmp420)!= 5)goto _LL2E7;_tmp421=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp420)->f2;
if((int)_tmp421 != 1)goto _LL2E7;_LL2E6: goto _LL2E8;_LL2E7: _tmp422=_tmp402.f1;if(
_tmp422 <= (void*)3?1:*((int*)_tmp422)!= 5)goto _LL2E9;_tmp423=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp422)->f2;if((int)_tmp423 != 2)goto _LL2E9;_tmp424=_tmp402.f2;if(_tmp424 <= (
void*)3?1:*((int*)_tmp424)!= 5)goto _LL2E9;_tmp425=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp424)->f2;if((int)_tmp425 != 0)goto _LL2E9;_LL2E8: goto _LL2EA;_LL2E9: _tmp426=
_tmp402.f1;if(_tmp426 <= (void*)3?1:*((int*)_tmp426)!= 5)goto _LL2EB;_tmp427=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp426)->f2;if((int)_tmp427 != 1)goto _LL2EB;
_tmp428=_tmp402.f2;if(_tmp428 <= (void*)3?1:*((int*)_tmp428)!= 5)goto _LL2EB;
_tmp429=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp428)->f2;if((int)_tmp429 != 
0)goto _LL2EB;_LL2EA: goto _LL2EC;_LL2EB: _tmp42A=_tmp402.f1;if(_tmp42A <= (void*)3?1:*((
int*)_tmp42A)!= 17)goto _LL2ED;_tmp42B=_tmp402.f2;if(_tmp42B <= (void*)3?1:*((int*)
_tmp42B)!= 5)goto _LL2ED;_tmp42C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp42B)->f2;
if((int)_tmp42C != 1)goto _LL2ED;_LL2EC: goto _LL2EE;_LL2ED: _tmp42D=_tmp402.f1;if(
_tmp42D <= (void*)3?1:*((int*)_tmp42D)!= 17)goto _LL2EF;_tmp42E=_tmp402.f2;if(
_tmp42E <= (void*)3?1:*((int*)_tmp42E)!= 5)goto _LL2EF;_tmp42F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp42E)->f2;if((int)_tmp42F != 0)goto _LL2EF;_LL2EE: goto _LL2F0;_LL2EF: _tmp430=
_tmp402.f1;if(_tmp430 <= (void*)3?1:*((int*)_tmp430)!= 14)goto _LL2F1;_tmp431=
_tmp402.f2;if(_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 5)goto _LL2F1;_tmp432=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp431)->f2;if((int)_tmp432 != 1)goto _LL2F1;
_LL2F0: goto _LL2F2;_LL2F1: _tmp433=_tmp402.f1;if(_tmp433 <= (void*)3?1:*((int*)
_tmp433)!= 14)goto _LL2F3;_tmp434=_tmp402.f2;if(_tmp434 <= (void*)3?1:*((int*)
_tmp434)!= 5)goto _LL2F3;_tmp435=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp434)->f2;
if((int)_tmp435 != 0)goto _LL2F3;_LL2F2: return 1;_LL2F3:;_LL2F4: return 0;_LL2CE:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->v=(
void*)t1;_tmp436;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp43A;_tmp43A.tag=0;_tmp43A.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_String_pa_struct _tmp439;
_tmp439.tag=0;_tmp439.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp437[2]={& _tmp439,& _tmp43A};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({
const char*_tmp438="type mismatch: expecting %s but found %s";_tag_arr(_tmp438,
sizeof(char),_get_zero_arr_size(_tmp438,41));}),_tag_arr(_tmp437,sizeof(void*),2));}}});
return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp43B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL2F6: if(_tmp43B <= (void*)
3?1:*((int*)_tmp43B)!= 4)goto _LL2F8;_LL2F7: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ);
goto _LL2F5;_LL2F8:;_LL2F9: return 0;_LL2F5:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp43C=Cyc_Tcutil_compress(t);_LL2FB: if(_tmp43C <= (void*)3?1:*((
int*)_tmp43C)!= 5)goto _LL2FD;_LL2FC: goto _LL2FE;_LL2FD: if(_tmp43C <= (void*)3?1:*((
int*)_tmp43C)!= 14)goto _LL2FF;_LL2FE: goto _LL300;_LL2FF: if(_tmp43C <= (void*)3?1:*((
int*)_tmp43C)!= 17)goto _LL301;_LL300: goto _LL302;_LL301: if(_tmp43C <= (void*)3?1:*((
int*)_tmp43C)!= 12)goto _LL303;_LL302: goto _LL304;_LL303: if(_tmp43C <= (void*)3?1:*((
int*)_tmp43C)!= 13)goto _LL305;_LL304: return 1;_LL305:;_LL306: return 0;_LL2FA:;}int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;
if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){
if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_uint_typ))({void*_tmp43D[0]={};Cyc_Tcutil_warn(e->loc,({const char*
_tmp43E="integral size mismatch; conversion supplied";_tag_arr(_tmp43E,sizeof(
char),_get_zero_arr_size(_tmp43E,44));}),_tag_arr(_tmp43D,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp43F[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp440="integral size mismatch; conversion supplied";
_tag_arr(_tmp440,sizeof(char),_get_zero_arr_size(_tmp440,44));}),_tag_arr(
_tmp43F,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ);
return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);{struct _tuple6 _tmp442=({struct _tuple6 _tmp441;_tmp441.f1=t1;_tmp441.f2=t2;
_tmp441;});void*_tmp443;struct Cyc_Absyn_PtrInfo _tmp444;void*_tmp445;struct Cyc_Absyn_PtrInfo
_tmp446;void*_tmp447;struct Cyc_Absyn_ArrayInfo _tmp448;void*_tmp449;struct Cyc_Absyn_Tqual
_tmp44A;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Conref*_tmp44C;void*_tmp44D;
struct Cyc_Absyn_ArrayInfo _tmp44E;void*_tmp44F;struct Cyc_Absyn_Tqual _tmp450;
struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Conref*_tmp452;void*_tmp453;struct
Cyc_Absyn_TunionFieldInfo _tmp454;void*_tmp455;struct Cyc_Absyn_Tuniondecl*_tmp456;
struct Cyc_Absyn_Tunionfield*_tmp457;struct Cyc_List_List*_tmp458;void*_tmp459;
struct Cyc_Absyn_TunionInfo _tmp45A;void*_tmp45B;struct Cyc_Absyn_Tuniondecl**
_tmp45C;struct Cyc_Absyn_Tuniondecl*_tmp45D;struct Cyc_List_List*_tmp45E;void*
_tmp45F;struct Cyc_Absyn_PtrInfo _tmp460;void*_tmp461;void*_tmp462;struct Cyc_Absyn_Conref*
_tmp463;struct Cyc_Absyn_Tqual _tmp464;struct Cyc_Absyn_Conref*_tmp465;struct Cyc_Absyn_Conref*
_tmp466;void*_tmp467;struct Cyc_Absyn_TunionInfo _tmp468;void*_tmp469;struct Cyc_Absyn_Tuniondecl**
_tmp46A;struct Cyc_Absyn_Tuniondecl*_tmp46B;struct Cyc_List_List*_tmp46C;void*
_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*
_tmp473;_LL308: _tmp443=_tmp442.f1;if(_tmp443 <= (void*)3?1:*((int*)_tmp443)!= 4)
goto _LL30A;_tmp444=((struct Cyc_Absyn_PointerType_struct*)_tmp443)->f1;_tmp445=
_tmp442.f2;if(_tmp445 <= (void*)3?1:*((int*)_tmp445)!= 4)goto _LL30A;_tmp446=((
struct Cyc_Absyn_PointerType_struct*)_tmp445)->f1;_LL309: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp444.nullable,_tmp446.nullable)){void*_tmp474=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp444.nullable))->v;int _tmp475;_LL317: if(_tmp474 <= (void*)1?1:*((int*)_tmp474)
!= 0)goto _LL319;_tmp475=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp474)->f1;
_LL318: okay=!_tmp475;goto _LL316;_LL319:;_LL31A:({void*_tmp476[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp477="silent_castable conref not eq";
_tag_arr(_tmp477,sizeof(char),_get_zero_arr_size(_tmp477,30));}),_tag_arr(
_tmp476,sizeof(void*),0));});_LL316:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp444.bounds,_tmp446.bounds)){struct _tuple6 _tmp479=({struct _tuple6 _tmp478;
_tmp478.f1=(void*)(Cyc_Absyn_compress_conref(_tmp444.bounds))->v;_tmp478.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp446.bounds))->v;_tmp478;});void*_tmp47A;
void*_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;struct Cyc_Absyn_Exp*
_tmp480;void*_tmp481;void*_tmp482;struct Cyc_Absyn_Exp*_tmp483;void*_tmp484;void*
_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;void*_tmp489;void*_tmp48A;void*
_tmp48B;_LL31C: _tmp47A=_tmp479.f1;if(_tmp47A <= (void*)1?1:*((int*)_tmp47A)!= 0)
goto _LL31E;_tmp47B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp47A)->f1;if(
_tmp47B <= (void*)1?1:*((int*)_tmp47B)!= 0)goto _LL31E;_tmp47C=_tmp479.f2;if(
_tmp47C <= (void*)1?1:*((int*)_tmp47C)!= 0)goto _LL31E;_tmp47D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp47C)->f1;if((int)_tmp47D != 0)goto _LL31E;_LL31D: okay=1;goto _LL31B;_LL31E:
_tmp47E=_tmp479.f1;if(_tmp47E <= (void*)1?1:*((int*)_tmp47E)!= 0)goto _LL320;
_tmp47F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp47E)->f1;if(_tmp47F <= (
void*)1?1:*((int*)_tmp47F)!= 0)goto _LL320;_tmp480=((struct Cyc_Absyn_Upper_b_struct*)
_tmp47F)->f1;_tmp481=_tmp479.f2;if(_tmp481 <= (void*)1?1:*((int*)_tmp481)!= 0)
goto _LL320;_tmp482=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp481)->f1;if(
_tmp482 <= (void*)1?1:*((int*)_tmp482)!= 0)goto _LL320;_tmp483=((struct Cyc_Absyn_Upper_b_struct*)
_tmp482)->f1;_LL31F: okay=okay?Cyc_Evexp_lte_const_exp(_tmp483,_tmp480): 0;if(!((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp446.zero_term))({
void*_tmp48C[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp48D="implicit cast to shorter array";
_tag_arr(_tmp48D,sizeof(char),_get_zero_arr_size(_tmp48D,31));}),_tag_arr(
_tmp48C,sizeof(void*),0));});goto _LL31B;_LL320: _tmp484=_tmp479.f1;if(_tmp484 <= (
void*)1?1:*((int*)_tmp484)!= 0)goto _LL322;_tmp485=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp484)->f1;if((int)_tmp485 != 0)goto _LL322;_tmp486=_tmp479.f2;if(_tmp486 <= (
void*)1?1:*((int*)_tmp486)!= 0)goto _LL322;_tmp487=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp486)->f1;if(_tmp487 <= (void*)1?1:*((int*)_tmp487)!= 0)goto _LL322;_LL321: if(!((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp444.zero_term))
okay=0;goto _LL31B;_LL322: _tmp488=_tmp479.f1;if(_tmp488 <= (void*)1?1:*((int*)
_tmp488)!= 0)goto _LL324;_tmp489=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp488)->f1;if((int)_tmp489 != 0)goto _LL324;_tmp48A=_tmp479.f2;if(_tmp48A <= (
void*)1?1:*((int*)_tmp48A)!= 0)goto _LL324;_tmp48B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp48A)->f1;if((int)_tmp48B != 0)goto _LL324;_LL323: okay=1;goto _LL31B;_LL324:;
_LL325: okay=0;goto _LL31B;_LL31B:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp444.elt_typ,(
void*)_tmp446.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)_tmp444.rgn_typ,(void*)
_tmp446.rgn_typ)?1: Cyc_Tcenv_region_outlives(te,(void*)_tmp444.rgn_typ,(void*)
_tmp446.rgn_typ): 0;okay=okay?!(_tmp444.tq).q_const?1:(_tmp446.tq).q_const: 0;okay=
okay?((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp444.zero_term,_tmp446.zero_term)?
1:(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp444.zero_term)?(
_tmp446.tq).q_const: 0): 0;return okay;}_LL30A: _tmp447=_tmp442.f1;if(_tmp447 <= (
void*)3?1:*((int*)_tmp447)!= 7)goto _LL30C;_tmp448=((struct Cyc_Absyn_ArrayType_struct*)
_tmp447)->f1;_tmp449=(void*)_tmp448.elt_type;_tmp44A=_tmp448.tq;_tmp44B=_tmp448.num_elts;
_tmp44C=_tmp448.zero_term;_tmp44D=_tmp442.f2;if(_tmp44D <= (void*)3?1:*((int*)
_tmp44D)!= 7)goto _LL30C;_tmp44E=((struct Cyc_Absyn_ArrayType_struct*)_tmp44D)->f1;
_tmp44F=(void*)_tmp44E.elt_type;_tmp450=_tmp44E.tq;_tmp451=_tmp44E.num_elts;
_tmp452=_tmp44E.zero_term;_LL30B: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp44C,_tmp452)?(_tmp44B != 0?_tmp451 != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp44B),(struct Cyc_Absyn_Exp*)_check_null(_tmp451)): 0: 0;return(okay?
Cyc_Tcutil_unify(_tmp449,_tmp44F): 0)?!_tmp44A.q_const?1: _tmp450.q_const: 0;}
_LL30C: _tmp453=_tmp442.f1;if(_tmp453 <= (void*)3?1:*((int*)_tmp453)!= 3)goto
_LL30E;_tmp454=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp453)->f1;_tmp455=(
void*)_tmp454.field_info;if(*((int*)_tmp455)!= 1)goto _LL30E;_tmp456=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp455)->f1;_tmp457=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp455)->f2;
_tmp458=_tmp454.targs;_tmp459=_tmp442.f2;if(_tmp459 <= (void*)3?1:*((int*)_tmp459)
!= 2)goto _LL30E;_tmp45A=((struct Cyc_Absyn_TunionType_struct*)_tmp459)->f1;
_tmp45B=(void*)_tmp45A.tunion_info;if(*((int*)_tmp45B)!= 1)goto _LL30E;_tmp45C=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp45B)->f1;_tmp45D=*_tmp45C;_tmp45E=
_tmp45A.targs;_LL30D: if((_tmp456 == _tmp45D?1: Cyc_Absyn_qvar_cmp(_tmp456->name,
_tmp45D->name)== 0)?_tmp457->typs == 0: 0){for(0;_tmp458 != 0?_tmp45E != 0: 0;(
_tmp458=_tmp458->tl,_tmp45E=_tmp45E->tl)){if(!Cyc_Tcutil_unify((void*)_tmp458->hd,(
void*)_tmp45E->hd))break;}if(_tmp458 == 0?_tmp45E == 0: 0)return 1;}return 0;_LL30E:
_tmp45F=_tmp442.f1;if(_tmp45F <= (void*)3?1:*((int*)_tmp45F)!= 4)goto _LL310;
_tmp460=((struct Cyc_Absyn_PointerType_struct*)_tmp45F)->f1;_tmp461=(void*)
_tmp460.elt_typ;_tmp462=(void*)_tmp460.rgn_typ;_tmp463=_tmp460.nullable;_tmp464=
_tmp460.tq;_tmp465=_tmp460.bounds;_tmp466=_tmp460.zero_term;_tmp467=_tmp442.f2;
if(_tmp467 <= (void*)3?1:*((int*)_tmp467)!= 2)goto _LL310;_tmp468=((struct Cyc_Absyn_TunionType_struct*)
_tmp467)->f1;_tmp469=(void*)_tmp468.tunion_info;if(*((int*)_tmp469)!= 1)goto
_LL310;_tmp46A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp469)->f1;_tmp46B=*
_tmp46A;_tmp46C=_tmp468.targs;_tmp46D=(void*)_tmp468.rgn;_LL30F:{void*_tmp48E=
Cyc_Tcutil_compress(_tmp461);struct Cyc_Absyn_TunionFieldInfo _tmp48F;void*_tmp490;
struct Cyc_Absyn_Tuniondecl*_tmp491;struct Cyc_Absyn_Tunionfield*_tmp492;struct Cyc_List_List*
_tmp493;_LL327: if(_tmp48E <= (void*)3?1:*((int*)_tmp48E)!= 3)goto _LL329;_tmp48F=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp48E)->f1;_tmp490=(void*)_tmp48F.field_info;
if(*((int*)_tmp490)!= 1)goto _LL329;_tmp491=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp490)->f1;_tmp492=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp490)->f2;
_tmp493=_tmp48F.targs;_LL328: if(!Cyc_Tcutil_unify(_tmp462,_tmp46D)?!Cyc_Tcenv_region_outlives(
te,_tmp462,_tmp46D): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp463,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp465,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;if(!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp466,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(
_tmp46B->name,_tmp491->name)== 0?_tmp492->typs != 0: 0){int okay=1;for(0;_tmp493 != 
0?_tmp46C != 0: 0;(_tmp493=_tmp493->tl,_tmp46C=_tmp46C->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp493->hd,(void*)_tmp46C->hd)){okay=0;break;}}if((!okay?1: _tmp493 != 0)?1:
_tmp46C != 0)return 0;return 1;}goto _LL326;_LL329:;_LL32A: goto _LL326;_LL326:;}
return 0;_LL310: _tmp46E=_tmp442.f1;if(_tmp46E <= (void*)3?1:*((int*)_tmp46E)!= 14)
goto _LL312;_tmp46F=_tmp442.f2;if(_tmp46F <= (void*)3?1:*((int*)_tmp46F)!= 5)goto
_LL312;_tmp470=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46F)->f2;if((int)
_tmp470 != 2)goto _LL312;_LL311: goto _LL313;_LL312: _tmp471=_tmp442.f1;if(_tmp471 <= (
void*)3?1:*((int*)_tmp471)!= 17)goto _LL314;_tmp472=_tmp442.f2;if(_tmp472 <= (void*)
3?1:*((int*)_tmp472)!= 5)goto _LL314;_tmp473=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp472)->f2;if((int)_tmp473 != 2)goto _LL314;_LL313: return 1;_LL314:;_LL315: return
Cyc_Tcutil_unify(t1,t2);_LL307:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp494=Cyc_Tcutil_compress(t);_LL32C: if(_tmp494 <= (void*)3?1:*((int*)_tmp494)!= 
4)goto _LL32E;_LL32D: return 1;_LL32E:;_LL32F: return 0;_LL32B:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp495=(void*)e->r;void*_tmp496;int _tmp497;void*
_tmp498;char _tmp499;void*_tmp49A;short _tmp49B;void*_tmp49C;long long _tmp49D;void*
_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;_LL331: if(*((int*)_tmp495)!= 0)goto _LL333;
_tmp496=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp495)->f1;if(_tmp496 <= (void*)
1?1:*((int*)_tmp496)!= 2)goto _LL333;_tmp497=((struct Cyc_Absyn_Int_c_struct*)
_tmp496)->f2;if(_tmp497 != 0)goto _LL333;_LL332: return 1;_LL333: if(*((int*)_tmp495)
!= 0)goto _LL335;_tmp498=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp495)->f1;
if(_tmp498 <= (void*)1?1:*((int*)_tmp498)!= 0)goto _LL335;_tmp499=((struct Cyc_Absyn_Char_c_struct*)
_tmp498)->f2;if(_tmp499 != 0)goto _LL335;_LL334: return 1;_LL335: if(*((int*)_tmp495)
!= 0)goto _LL337;_tmp49A=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp495)->f1;
if(_tmp49A <= (void*)1?1:*((int*)_tmp49A)!= 1)goto _LL337;_tmp49B=((struct Cyc_Absyn_Short_c_struct*)
_tmp49A)->f2;if(_tmp49B != 0)goto _LL337;_LL336: return 1;_LL337: if(*((int*)_tmp495)
!= 0)goto _LL339;_tmp49C=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp495)->f1;
if(_tmp49C <= (void*)1?1:*((int*)_tmp49C)!= 3)goto _LL339;_tmp49D=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp49C)->f2;if(_tmp49D != 0)goto _LL339;_LL338: return 1;_LL339: if(*((int*)_tmp495)
!= 13)goto _LL33B;_tmp49E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp495)->f1;
_tmp49F=((struct Cyc_Absyn_Cast_e_struct*)_tmp495)->f2;_LL33A: return Cyc_Tcutil_is_zero(
_tmp49F)?Cyc_Tcutil_admits_zero(_tmp49E): 0;_LL33B:;_LL33C: return 0;_LL330:;}
struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2)?Cyc_Tcutil_is_zero(
e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp4A0=
_cycalloc(sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_Absyn_Const_e_struct _tmp4A1;
_tmp4A1.tag=0;_tmp4A1.f1=(void*)((void*)0);_tmp4A1;});_tmp4A0;})));{struct Cyc_List_List*
_tmp4A2=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp4A3=({struct Cyc_Absyn_PointerType_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));
_tmp4A4[0]=({struct Cyc_Absyn_PointerType_struct _tmp4A5;_tmp4A5.tag=4;_tmp4A5.f1=({
struct Cyc_Absyn_PtrInfo _tmp4A6;_tmp4A6.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp4A8=_cycalloc(sizeof(*
_tmp4A8));_tmp4A8->v=_tmp4A2;_tmp4A8;}));_tmp4A6.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp4A7=_cycalloc(
sizeof(*_tmp4A7));_tmp4A7->v=_tmp4A2;_tmp4A7;}));_tmp4A6.nullable=Cyc_Absyn_true_conref;
_tmp4A6.tq=Cyc_Absyn_empty_tqual();_tmp4A6.bounds=Cyc_Absyn_empty_conref();
_tmp4A6.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp4A6;});_tmp4A5;});_tmp4A4;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(
void*)((void*)_tmp4A3));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}static
int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp4A9=Cyc_Tcutil_compress(t1);
_LL33E: if(_tmp4A9 <= (void*)3?1:*((int*)_tmp4A9)!= 14)goto _LL340;_LL33F: return 1;
_LL340:;_LL341: return 0;_LL33D:;}static int Cyc_Tcutil_is_tag_type(void*t1){void*
_tmp4AA=Cyc_Tcutil_compress(t1);_LL343: if(_tmp4AA <= (void*)3?1:*((int*)_tmp4AA)
!= 17)goto _LL345;_LL344: return 1;_LL345:;_LL346: return 0;_LL342:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);if(Cyc_Tcutil_unify(t1,t2))
return 1;if(((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2):
0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0))?1:(Cyc_Tcutil_is_tag_type(
t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(t1,
t2))({struct Cyc_String_pa_struct _tmp4AE;_tmp4AE.tag=0;_tmp4AE.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_String_pa_struct _tmp4AD;
_tmp4AD.tag=0;_tmp4AD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp4AB[2]={& _tmp4AD,& _tmp4AE};Cyc_Tcutil_warn(e->loc,({const char*_tmp4AC="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp4AC,sizeof(char),_get_zero_arr_size(_tmp4AC,53));}),_tag_arr(
_tmp4AB,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2);return 1;}else{
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,
t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_String_pa_struct
_tmp4B2;_tmp4B2.tag=0;_tmp4B2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_String_pa_struct _tmp4B1;_tmp4B1.tag=0;_tmp4B1.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t1);{void*_tmp4AF[2]={& _tmp4B1,& _tmp4B2};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4B0="implicit cast from %s to %s";_tag_arr(_tmp4B0,
sizeof(char),_get_zero_arr_size(_tmp4B0,28));}),_tag_arr(_tmp4AF,sizeof(void*),2));}}});
return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp4B3=Cyc_Tcutil_compress(t);_LL348: if(_tmp4B3 <= (void*)3?1:*((
int*)_tmp4B3)!= 5)goto _LL34A;_LL349: goto _LL34B;_LL34A: if((int)_tmp4B3 != 1)goto
_LL34C;_LL34B: goto _LL34D;_LL34C: if(_tmp4B3 <= (void*)3?1:*((int*)_tmp4B3)!= 6)
goto _LL34E;_LL34D: return 1;_LL34E:;_LL34F: return 0;_LL347:;}static struct _tuple4*
Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*x){
return({struct _tuple4*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->f1=x->tq;
_tmp4B4->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);_tmp4B4;});}static struct
Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(
t1);{void*_tmp4B5=t1;struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_AggrInfo _tmp4B7;
void*_tmp4B8;struct Cyc_Absyn_Aggrdecl**_tmp4B9;struct Cyc_Absyn_Aggrdecl*_tmp4BA;
struct Cyc_List_List*_tmp4BB;void*_tmp4BC;struct Cyc_List_List*_tmp4BD;_LL351: if((
int)_tmp4B5 != 0)goto _LL353;_LL352: return 0;_LL353: if(_tmp4B5 <= (void*)3?1:*((int*)
_tmp4B5)!= 9)goto _LL355;_tmp4B6=((struct Cyc_Absyn_TupleType_struct*)_tmp4B5)->f1;
_LL354: return _tmp4B6;_LL355: if(_tmp4B5 <= (void*)3?1:*((int*)_tmp4B5)!= 10)goto
_LL357;_tmp4B7=((struct Cyc_Absyn_AggrType_struct*)_tmp4B5)->f1;_tmp4B8=(void*)
_tmp4B7.aggr_info;if(*((int*)_tmp4B8)!= 1)goto _LL357;_tmp4B9=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp4B8)->f1;_tmp4BA=*_tmp4B9;_tmp4BB=_tmp4B7.targs;_LL356: if((((void*)_tmp4BA->kind
== (void*)1?1: _tmp4BA->impl == 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4BA->impl))->exist_vars != 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4BA->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp4BE=_cycalloc(
sizeof(*_tmp4BE));_tmp4BE->hd=({struct _tuple4*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));
_tmp4BF->f1=Cyc_Absyn_empty_tqual();_tmp4BF->f2=t1;_tmp4BF;});_tmp4BE->tl=0;
_tmp4BE;});{struct Cyc_List_List*_tmp4C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp4BA->tvs,_tmp4BB);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp4C0,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BA->impl))->fields);}_LL357: if(
_tmp4B5 <= (void*)3?1:*((int*)_tmp4B5)!= 11)goto _LL359;_tmp4BC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4B5)->f1;if((int)_tmp4BC != 0)goto _LL359;_tmp4BD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4B5)->f2;_LL358: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp4BD);_LL359:;_LL35A: return({struct Cyc_List_List*
_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->hd=({struct _tuple4*_tmp4C2=
_cycalloc(sizeof(*_tmp4C2));_tmp4C2->f1=Cyc_Absyn_empty_tqual();_tmp4C2->f2=t1;
_tmp4C2;});_tmp4C1->tl=0;_tmp4C1;});_LL350:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)((struct _tuple6*)a->hd))).f1)?Cyc_Tcutil_unify(t2,(*((
struct _tuple6*)((struct _tuple6*)a->hd))).f2): 0)return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp4C4=({struct _tuple6 _tmp4C3;
_tmp4C3.f1=t1;_tmp4C3.f2=t2;_tmp4C3;});void*_tmp4C5;struct Cyc_Absyn_PtrInfo
_tmp4C6;void*_tmp4C7;void*_tmp4C8;struct Cyc_Absyn_Conref*_tmp4C9;struct Cyc_Absyn_Tqual
_tmp4CA;struct Cyc_Absyn_Conref*_tmp4CB;struct Cyc_Absyn_Conref*_tmp4CC;void*
_tmp4CD;struct Cyc_Absyn_PtrInfo _tmp4CE;void*_tmp4CF;void*_tmp4D0;struct Cyc_Absyn_Conref*
_tmp4D1;struct Cyc_Absyn_Tqual _tmp4D2;struct Cyc_Absyn_Conref*_tmp4D3;struct Cyc_Absyn_Conref*
_tmp4D4;_LL35C: _tmp4C5=_tmp4C4.f1;if(_tmp4C5 <= (void*)3?1:*((int*)_tmp4C5)!= 4)
goto _LL35E;_tmp4C6=((struct Cyc_Absyn_PointerType_struct*)_tmp4C5)->f1;_tmp4C7=(
void*)_tmp4C6.elt_typ;_tmp4C8=(void*)_tmp4C6.rgn_typ;_tmp4C9=_tmp4C6.nullable;
_tmp4CA=_tmp4C6.tq;_tmp4CB=_tmp4C6.bounds;_tmp4CC=_tmp4C6.zero_term;_tmp4CD=
_tmp4C4.f2;if(_tmp4CD <= (void*)3?1:*((int*)_tmp4CD)!= 4)goto _LL35E;_tmp4CE=((
struct Cyc_Absyn_PointerType_struct*)_tmp4CD)->f1;_tmp4CF=(void*)_tmp4CE.elt_typ;
_tmp4D0=(void*)_tmp4CE.rgn_typ;_tmp4D1=_tmp4CE.nullable;_tmp4D2=_tmp4CE.tq;
_tmp4D3=_tmp4CE.bounds;_tmp4D4=_tmp4CE.zero_term;_LL35D: if(_tmp4CA.q_const?!
_tmp4D2.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4C9,
_tmp4D1)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp4C9): 0)?!((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp4D1): 0)return 0;if((!((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp4CC,_tmp4D4)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp4CC): 0)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp4D4): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp4C8,_tmp4D0)?!Cyc_Tcenv_region_outlives(te,
_tmp4C8,_tmp4D0): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp4CB,_tmp4D3)){struct _tuple6 _tmp4D6=({struct _tuple6 _tmp4D5;_tmp4D5.f1=Cyc_Absyn_conref_val(
_tmp4CB);_tmp4D5.f2=Cyc_Absyn_conref_val(_tmp4D3);_tmp4D5;});void*_tmp4D7;void*
_tmp4D8;void*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;void*_tmp4DB;struct Cyc_Absyn_Exp*
_tmp4DC;_LL361: _tmp4D7=_tmp4D6.f1;if(_tmp4D7 <= (void*)1?1:*((int*)_tmp4D7)!= 0)
goto _LL363;_tmp4D8=_tmp4D6.f2;if((int)_tmp4D8 != 0)goto _LL363;_LL362: goto _LL360;
_LL363: _tmp4D9=_tmp4D6.f1;if(_tmp4D9 <= (void*)1?1:*((int*)_tmp4D9)!= 0)goto
_LL365;_tmp4DA=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D9)->f1;_tmp4DB=_tmp4D6.f2;
if(_tmp4DB <= (void*)1?1:*((int*)_tmp4DB)!= 0)goto _LL365;_tmp4DC=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4DB)->f1;_LL364: if(!Cyc_Evexp_lte_const_exp(_tmp4DC,_tmp4DA))return 0;goto
_LL360;_LL365:;_LL366: return 0;_LL360:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->hd=({struct _tuple6*
_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->f1=t1;_tmp4DE->f2=t2;_tmp4DE;});
_tmp4DD->tl=assume;_tmp4DD;}),_tmp4C7,_tmp4CF);_LL35E:;_LL35F: return 0;_LL35B:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp4E0=({struct
_tuple6 _tmp4DF;_tmp4DF.f1=Cyc_Tcutil_compress(t1);_tmp4DF.f2=Cyc_Tcutil_compress(
t2);_tmp4DF;});void*_tmp4E1;void*_tmp4E2;void*_tmp4E3;void*_tmp4E4;_LL368:
_tmp4E1=_tmp4E0.f1;if(_tmp4E1 <= (void*)3?1:*((int*)_tmp4E1)!= 5)goto _LL36A;
_tmp4E2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4E1)->f2;_tmp4E3=_tmp4E0.f2;
if(_tmp4E3 <= (void*)3?1:*((int*)_tmp4E3)!= 5)goto _LL36A;_tmp4E4=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp4E3)->f2;_LL369: return _tmp4E2 == _tmp4E4;_LL36A:;
_LL36B: return 0;_LL367:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp4E6;struct
Cyc_Absyn_Tqual _tmp4E7;void*_tmp4E8;struct _tuple4*_tmp4E5=(struct _tuple4*)tqs1->hd;
_tmp4E6=*_tmp4E5;_tmp4E7=_tmp4E6.f1;_tmp4E8=_tmp4E6.f2;{struct _tuple4 _tmp4EA;
struct Cyc_Absyn_Tqual _tmp4EB;void*_tmp4EC;struct _tuple4*_tmp4E9=(struct _tuple4*)
tqs2->hd;_tmp4EA=*_tmp4E9;_tmp4EB=_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;if(_tmp4EB.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp4E8,_tmp4EC): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp4E8,_tmp4EC))continue;else{if(Cyc_Tcutil_isomorphic(_tmp4E8,_tmp4EC))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp4ED=Cyc_Tcutil_compress(t);void*_tmp4EE;_LL36D: if(_tmp4ED <= (void*)3?1:*((
int*)_tmp4ED)!= 5)goto _LL36F;_tmp4EE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4ED)->f2;if((int)_tmp4EE != 0)goto _LL36F;_LL36E: return 1;_LL36F:;_LL370: return
0;_LL36C:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp4EF=t2;void*_tmp4F0;_LL372: if(_tmp4EF <= (
void*)3?1:*((int*)_tmp4EF)!= 5)goto _LL374;_tmp4F0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4EF)->f2;if((int)_tmp4F0 != 2)goto _LL374;_LL373: if(Cyc_Tcutil_typ_kind(t1)== (
void*)2)return 1;goto _LL371;_LL374:;_LL375: goto _LL371;_LL371:;}{void*_tmp4F1=t1;
struct Cyc_Absyn_PtrInfo _tmp4F2;void*_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_Conref*
_tmp4F5;struct Cyc_Absyn_Tqual _tmp4F6;struct Cyc_Absyn_Conref*_tmp4F7;struct Cyc_Absyn_Conref*
_tmp4F8;struct Cyc_Absyn_ArrayInfo _tmp4F9;void*_tmp4FA;struct Cyc_Absyn_Tqual
_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Conref*_tmp4FD;struct Cyc_Absyn_Enumdecl*
_tmp4FE;_LL377: if(_tmp4F1 <= (void*)3?1:*((int*)_tmp4F1)!= 4)goto _LL379;_tmp4F2=((
struct Cyc_Absyn_PointerType_struct*)_tmp4F1)->f1;_tmp4F3=(void*)_tmp4F2.elt_typ;
_tmp4F4=(void*)_tmp4F2.rgn_typ;_tmp4F5=_tmp4F2.nullable;_tmp4F6=_tmp4F2.tq;
_tmp4F7=_tmp4F2.bounds;_tmp4F8=_tmp4F2.zero_term;_LL378:{void*_tmp4FF=t2;struct
Cyc_Absyn_PtrInfo _tmp500;void*_tmp501;void*_tmp502;struct Cyc_Absyn_Conref*
_tmp503;struct Cyc_Absyn_Tqual _tmp504;struct Cyc_Absyn_Conref*_tmp505;struct Cyc_Absyn_Conref*
_tmp506;_LL388: if(_tmp4FF <= (void*)3?1:*((int*)_tmp4FF)!= 4)goto _LL38A;_tmp500=((
struct Cyc_Absyn_PointerType_struct*)_tmp4FF)->f1;_tmp501=(void*)_tmp500.elt_typ;
_tmp502=(void*)_tmp500.rgn_typ;_tmp503=_tmp500.nullable;_tmp504=_tmp500.tq;
_tmp505=_tmp500.bounds;_tmp506=_tmp500.zero_term;_LL389: {struct Cyc_List_List*
_tmp507=({struct Cyc_List_List*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->hd=({
struct _tuple6*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->f1=t1;_tmp511->f2=t2;
_tmp511;});_tmp510->tl=0;_tmp510;});int _tmp508=Cyc_Tcutil_ptrsubtype(te,_tmp507,
_tmp4F3,_tmp501)?!_tmp4F6.q_const?1: _tmp504.q_const: 0;int zeroterm_ok=((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp4F8,_tmp506)?1: !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp506);int _tmp509=_tmp508?0:(((Cyc_Tcutil_bits_only(_tmp4F3)?Cyc_Tcutil_is_char_type(
_tmp501): 0)?!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp506): 0)?_tmp504.q_const?1: !_tmp4F6.q_const: 0);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp4F7,_tmp505);if(!bounds_ok?!_tmp509: 0){struct _tuple6
_tmp50B=({struct _tuple6 _tmp50A;_tmp50A.f1=Cyc_Absyn_conref_val(_tmp4F7);_tmp50A.f2=
Cyc_Absyn_conref_val(_tmp505);_tmp50A;});void*_tmp50C;struct Cyc_Absyn_Exp*
_tmp50D;void*_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;_LL38F: _tmp50C=_tmp50B.f1;if(
_tmp50C <= (void*)1?1:*((int*)_tmp50C)!= 0)goto _LL391;_tmp50D=((struct Cyc_Absyn_Upper_b_struct*)
_tmp50C)->f1;_tmp50E=_tmp50B.f2;if(_tmp50E <= (void*)1?1:*((int*)_tmp50E)!= 0)
goto _LL391;_tmp50F=((struct Cyc_Absyn_Upper_b_struct*)_tmp50E)->f1;_LL390: if(Cyc_Evexp_lte_const_exp(
_tmp50F,_tmp50D))bounds_ok=1;goto _LL38E;_LL391:;_LL392: bounds_ok=1;goto _LL38E;
_LL38E:;}return((bounds_ok?zeroterm_ok: 0)?_tmp508?1: _tmp509: 0)?Cyc_Tcutil_unify(
_tmp4F4,_tmp502)?1: Cyc_Tcenv_region_outlives(te,_tmp4F4,_tmp502): 0;}_LL38A: if((
int)_tmp4FF != 0)goto _LL38C;_LL38B: return 1;_LL38C:;_LL38D: goto _LL387;_LL387:;}
return 0;_LL379: if(_tmp4F1 <= (void*)3?1:*((int*)_tmp4F1)!= 7)goto _LL37B;_tmp4F9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4F1)->f1;_tmp4FA=(void*)_tmp4F9.elt_type;
_tmp4FB=_tmp4F9.tq;_tmp4FC=_tmp4F9.num_elts;_tmp4FD=_tmp4F9.zero_term;_LL37A:{
void*_tmp512=t2;struct Cyc_Absyn_ArrayInfo _tmp513;void*_tmp514;struct Cyc_Absyn_Tqual
_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Conref*_tmp517;_LL394: if(
_tmp512 <= (void*)3?1:*((int*)_tmp512)!= 7)goto _LL396;_tmp513=((struct Cyc_Absyn_ArrayType_struct*)
_tmp512)->f1;_tmp514=(void*)_tmp513.elt_type;_tmp515=_tmp513.tq;_tmp516=_tmp513.num_elts;
_tmp517=_tmp513.zero_term;_LL395: {int okay;okay=((_tmp4FC != 0?_tmp516 != 0: 0)?((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp4FD,_tmp517): 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp516),(struct Cyc_Absyn_Exp*)_check_null(_tmp4FC)): 0;return(okay?
Cyc_Tcutil_unify(_tmp4FA,_tmp514): 0)?!_tmp4FB.q_const?1: _tmp515.q_const: 0;}
_LL396:;_LL397: return 0;_LL393:;}return 0;_LL37B: if(_tmp4F1 <= (void*)3?1:*((int*)
_tmp4F1)!= 12)goto _LL37D;_tmp4FE=((struct Cyc_Absyn_EnumType_struct*)_tmp4F1)->f2;
_LL37C:{void*_tmp518=t2;struct Cyc_Absyn_Enumdecl*_tmp519;_LL399: if(_tmp518 <= (
void*)3?1:*((int*)_tmp518)!= 12)goto _LL39B;_tmp519=((struct Cyc_Absyn_EnumType_struct*)
_tmp518)->f2;_LL39A: if((_tmp4FE->fields != 0?_tmp519->fields != 0: 0)?((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp4FE->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp519->fields))->v): 0)
return 1;goto _LL398;_LL39B:;_LL39C: goto _LL398;_LL398:;}goto _LL37E;_LL37D: if(
_tmp4F1 <= (void*)3?1:*((int*)_tmp4F1)!= 5)goto _LL37F;_LL37E: goto _LL380;_LL37F:
if((int)_tmp4F1 != 1)goto _LL381;_LL380: goto _LL382;_LL381: if(_tmp4F1 <= (void*)3?1:*((
int*)_tmp4F1)!= 6)goto _LL383;_LL382: return Cyc_Tcutil_coerceable(t2)?1: t2 == (void*)
0;_LL383: if((int)_tmp4F1 != 0)goto _LL385;_LL384: return t2 == (void*)0;_LL385:;
_LL386: return 0;_LL376:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp51A=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp51B=_cycalloc(
sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_Absyn_Cast_e_struct _tmp51C;_tmp51C.tag=
13;_tmp51C.f1=(void*)t;_tmp51C.f2=_tmp51A;_tmp51C;});_tmp51B;})));e->topt=({
struct Cyc_Core_Opt*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->v=(void*)t;
_tmp51D;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp51E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL39E: if(_tmp51E <= (void*)
3?1:*((int*)_tmp51E)!= 5)goto _LL3A0;_LL39F: goto _LL3A1;_LL3A0: if(_tmp51E <= (void*)
3?1:*((int*)_tmp51E)!= 12)goto _LL3A2;_LL3A1: goto _LL3A3;_LL3A2: if(_tmp51E <= (void*)
3?1:*((int*)_tmp51E)!= 13)goto _LL3A4;_LL3A3: goto _LL3A5;_LL3A4: if(_tmp51E <= (void*)
3?1:*((int*)_tmp51E)!= 17)goto _LL3A6;_LL3A5: goto _LL3A7;_LL3A6: if(_tmp51E <= (void*)
3?1:*((int*)_tmp51E)!= 14)goto _LL3A8;_LL3A7: return 1;_LL3A8: if(_tmp51E <= (void*)3?
1:*((int*)_tmp51E)!= 0)goto _LL3AA;_LL3A9: return Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3AA:;_LL3AB: return
0;_LL39D:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp51F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3AD: if((int)_tmp51F != 1)goto _LL3AF;_LL3AE: goto
_LL3B0;_LL3AF: if(_tmp51F <= (void*)3?1:*((int*)_tmp51F)!= 6)goto _LL3B1;_LL3B0:
return 1;_LL3B1:;_LL3B2: return 0;_LL3AC:;}}int Cyc_Tcutil_is_function_type(void*t){
void*_tmp520=Cyc_Tcutil_compress(t);_LL3B4: if(_tmp520 <= (void*)3?1:*((int*)
_tmp520)!= 8)goto _LL3B6;_LL3B5: return 1;_LL3B6:;_LL3B7: return 0;_LL3B3:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple6 _tmp522=({struct _tuple6 _tmp521;_tmp521.f1=t1;
_tmp521.f2=t2;_tmp521;});void*_tmp523;int _tmp524;void*_tmp525;int _tmp526;void*
_tmp527;void*_tmp528;void*_tmp529;void*_tmp52A;void*_tmp52B;void*_tmp52C;void*
_tmp52D;void*_tmp52E;void*_tmp52F;void*_tmp530;void*_tmp531;void*_tmp532;void*
_tmp533;void*_tmp534;void*_tmp535;void*_tmp536;void*_tmp537;void*_tmp538;void*
_tmp539;void*_tmp53A;void*_tmp53B;void*_tmp53C;void*_tmp53D;void*_tmp53E;void*
_tmp53F;void*_tmp540;void*_tmp541;void*_tmp542;_LL3B9: _tmp523=_tmp522.f1;if(
_tmp523 <= (void*)3?1:*((int*)_tmp523)!= 6)goto _LL3BB;_tmp524=((struct Cyc_Absyn_DoubleType_struct*)
_tmp523)->f1;_tmp525=_tmp522.f2;if(_tmp525 <= (void*)3?1:*((int*)_tmp525)!= 6)
goto _LL3BB;_tmp526=((struct Cyc_Absyn_DoubleType_struct*)_tmp525)->f1;_LL3BA: if(
_tmp524)return t1;else{return t2;}_LL3BB: _tmp527=_tmp522.f1;if(_tmp527 <= (void*)3?
1:*((int*)_tmp527)!= 6)goto _LL3BD;_LL3BC: return t1;_LL3BD: _tmp528=_tmp522.f2;if(
_tmp528 <= (void*)3?1:*((int*)_tmp528)!= 6)goto _LL3BF;_LL3BE: return t2;_LL3BF:
_tmp529=_tmp522.f1;if((int)_tmp529 != 1)goto _LL3C1;_LL3C0: goto _LL3C2;_LL3C1:
_tmp52A=_tmp522.f2;if((int)_tmp52A != 1)goto _LL3C3;_LL3C2: return(void*)1;_LL3C3:
_tmp52B=_tmp522.f1;if(_tmp52B <= (void*)3?1:*((int*)_tmp52B)!= 5)goto _LL3C5;
_tmp52C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp52B)->f1;if((int)_tmp52C != 
1)goto _LL3C5;_tmp52D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp52B)->f2;if((
int)_tmp52D != 3)goto _LL3C5;_LL3C4: goto _LL3C6;_LL3C5: _tmp52E=_tmp522.f2;if(
_tmp52E <= (void*)3?1:*((int*)_tmp52E)!= 5)goto _LL3C7;_tmp52F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp52E)->f1;if((int)_tmp52F != 1)goto _LL3C7;_tmp530=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp52E)->f2;if((int)_tmp530 != 3)goto _LL3C7;_LL3C6: return Cyc_Absyn_ulonglong_typ;
_LL3C7: _tmp531=_tmp522.f1;if(_tmp531 <= (void*)3?1:*((int*)_tmp531)!= 5)goto
_LL3C9;_tmp532=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp531)->f1;if((int)
_tmp532 != 2)goto _LL3C9;_tmp533=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp531)->f2;
if((int)_tmp533 != 3)goto _LL3C9;_LL3C8: goto _LL3CA;_LL3C9: _tmp534=_tmp522.f2;if(
_tmp534 <= (void*)3?1:*((int*)_tmp534)!= 5)goto _LL3CB;_tmp535=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp534)->f1;if((int)_tmp535 != 2)goto _LL3CB;_tmp536=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp534)->f2;if((int)_tmp536 != 3)goto _LL3CB;_LL3CA: goto _LL3CC;_LL3CB: _tmp537=
_tmp522.f1;if(_tmp537 <= (void*)3?1:*((int*)_tmp537)!= 5)goto _LL3CD;_tmp538=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp537)->f1;if((int)_tmp538 != 0)goto _LL3CD;
_tmp539=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp537)->f2;if((int)_tmp539 != 
3)goto _LL3CD;_LL3CC: goto _LL3CE;_LL3CD: _tmp53A=_tmp522.f2;if(_tmp53A <= (void*)3?1:*((
int*)_tmp53A)!= 5)goto _LL3CF;_tmp53B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp53A)->f1;if((int)_tmp53B != 0)goto _LL3CF;_tmp53C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp53A)->f2;if((int)_tmp53C != 3)goto _LL3CF;_LL3CE: return Cyc_Absyn_slonglong_typ;
_LL3CF: _tmp53D=_tmp522.f1;if(_tmp53D <= (void*)3?1:*((int*)_tmp53D)!= 5)goto
_LL3D1;_tmp53E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp53D)->f1;if((int)
_tmp53E != 1)goto _LL3D1;_tmp53F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp53D)->f2;
if((int)_tmp53F != 2)goto _LL3D1;_LL3D0: goto _LL3D2;_LL3D1: _tmp540=_tmp522.f2;if(
_tmp540 <= (void*)3?1:*((int*)_tmp540)!= 5)goto _LL3D3;_tmp541=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp540)->f1;if((int)_tmp541 != 1)goto _LL3D3;_tmp542=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp540)->f2;if((int)_tmp542 != 2)goto _LL3D3;_LL3D2: return Cyc_Absyn_uint_typ;
_LL3D3:;_LL3D4: return Cyc_Absyn_sint_typ;_LL3B8:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp543=(void*)e->r;struct Cyc_Core_Opt*_tmp544;
_LL3D6: if(*((int*)_tmp543)!= 4)goto _LL3D8;_tmp544=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp543)->f2;if(_tmp544 != 0)goto _LL3D8;_LL3D7:({void*_tmp545[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp546="assignment in test";_tag_arr(_tmp546,sizeof(char),
_get_zero_arr_size(_tmp546,19));}),_tag_arr(_tmp545,sizeof(void*),0));});goto
_LL3D5;_LL3D8:;_LL3D9: goto _LL3D5;_LL3D5:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp548=({struct _tuple6 _tmp547;_tmp547.f1=c1;_tmp547.f2=c2;_tmp547;});
void*_tmp549;void*_tmp54A;void*_tmp54B;void*_tmp54C;void*_tmp54D;struct Cyc_Core_Opt*
_tmp54E;struct Cyc_Core_Opt**_tmp54F;void*_tmp550;struct Cyc_Core_Opt*_tmp551;
struct Cyc_Core_Opt**_tmp552;void*_tmp553;struct Cyc_Core_Opt*_tmp554;struct Cyc_Core_Opt**
_tmp555;void*_tmp556;void*_tmp557;void*_tmp558;void*_tmp559;void*_tmp55A;void*
_tmp55B;struct Cyc_Core_Opt*_tmp55C;struct Cyc_Core_Opt**_tmp55D;void*_tmp55E;void*
_tmp55F;struct Cyc_Core_Opt*_tmp560;struct Cyc_Core_Opt**_tmp561;void*_tmp562;void*
_tmp563;struct Cyc_Core_Opt*_tmp564;struct Cyc_Core_Opt**_tmp565;void*_tmp566;
_LL3DB: _tmp549=_tmp548.f1;if(*((int*)_tmp549)!= 0)goto _LL3DD;_tmp54A=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp549)->f1;_tmp54B=_tmp548.f2;if(*((int*)_tmp54B)
!= 0)goto _LL3DD;_tmp54C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp54B)->f1;
_LL3DC: return _tmp54A == _tmp54C;_LL3DD: _tmp54D=_tmp548.f2;if(*((int*)_tmp54D)!= 1)
goto _LL3DF;_tmp54E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp54D)->f1;_tmp54F=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp54D)->f1;_LL3DE:*((
struct Cyc_Core_Opt**)_tmp54F)=({struct Cyc_Core_Opt*_tmp567=_cycalloc(sizeof(*
_tmp567));_tmp567->v=(void*)c1;_tmp567;});return 1;_LL3DF: _tmp550=_tmp548.f1;if(*((
int*)_tmp550)!= 1)goto _LL3E1;_tmp551=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp550)->f1;_tmp552=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp550)->f1;_LL3E0:*((struct Cyc_Core_Opt**)_tmp552)=({struct Cyc_Core_Opt*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->v=(void*)c2;_tmp568;});return 1;
_LL3E1: _tmp553=_tmp548.f1;if(*((int*)_tmp553)!= 2)goto _LL3E3;_tmp554=((struct Cyc_Absyn_Less_kb_struct*)
_tmp553)->f1;_tmp555=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp553)->f1;_tmp556=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp553)->f2;
_tmp557=_tmp548.f2;if(*((int*)_tmp557)!= 0)goto _LL3E3;_tmp558=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp557)->f1;_LL3E2: if(Cyc_Tcutil_kind_leq(_tmp558,_tmp556)){*((struct Cyc_Core_Opt**)
_tmp555)=({struct Cyc_Core_Opt*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->v=(
void*)c2;_tmp569;});return 1;}else{return 0;}_LL3E3: _tmp559=_tmp548.f1;if(*((int*)
_tmp559)!= 0)goto _LL3E5;_tmp55A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp559)->f1;
_tmp55B=_tmp548.f2;if(*((int*)_tmp55B)!= 2)goto _LL3E5;_tmp55C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp55B)->f1;_tmp55D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp55B)->f1;_tmp55E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp55B)->f2;
_LL3E4: if(Cyc_Tcutil_kind_leq(_tmp55A,_tmp55E)){*((struct Cyc_Core_Opt**)_tmp55D)=({
struct Cyc_Core_Opt*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->v=(void*)c1;
_tmp56A;});return 1;}else{return 0;}_LL3E5: _tmp55F=_tmp548.f1;if(*((int*)_tmp55F)
!= 2)goto _LL3DA;_tmp560=((struct Cyc_Absyn_Less_kb_struct*)_tmp55F)->f1;_tmp561=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp55F)->f1;_tmp562=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp55F)->f2;_tmp563=_tmp548.f2;if(*((
int*)_tmp563)!= 2)goto _LL3DA;_tmp564=((struct Cyc_Absyn_Less_kb_struct*)_tmp563)->f1;
_tmp565=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp563)->f1;
_tmp566=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp563)->f2;_LL3E6: if(Cyc_Tcutil_kind_leq(
_tmp562,_tmp566)){*((struct Cyc_Core_Opt**)_tmp565)=({struct Cyc_Core_Opt*_tmp56B=
_cycalloc(sizeof(*_tmp56B));_tmp56B->v=(void*)c1;_tmp56B;});return 1;}else{if(Cyc_Tcutil_kind_leq(
_tmp566,_tmp562)){*((struct Cyc_Core_Opt**)_tmp561)=({struct Cyc_Core_Opt*_tmp56C=
_cycalloc(sizeof(*_tmp56C));_tmp56C->v=(void*)c2;_tmp56C;});return 1;}else{return
0;}}_LL3DA:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp56D=_cycalloc_atomic(sizeof(*_tmp56D));_tmp56D[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp56D;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp572;_tmp572.tag=1;_tmp572.f1=(int)((unsigned int)i);{
void*_tmp570[1]={& _tmp572};Cyc_aprintf(({const char*_tmp571="#%d";_tag_arr(
_tmp571,sizeof(char),_get_zero_arr_size(_tmp571,4));}),_tag_arr(_tmp570,sizeof(
void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp56E=_cycalloc(sizeof(*_tmp56E));
_tmp56E->name=({struct _tagged_arr*_tmp56F=_cycalloc(sizeof(struct _tagged_arr)* 1);
_tmp56F[0]=s;_tmp56F;});_tmp56E->identity=0;_tmp56E->kind=(void*)k;_tmp56E;});}
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp573=*((
struct _tagged_arr*)t->name);return*((const char*)_check_unknown_subscript(_tmp573,
sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){({
struct Cyc_String_pa_struct _tmp576;_tmp576.tag=0;_tmp576.f1=(struct _tagged_arr)*((
struct _tagged_arr*)t->name);{void*_tmp574[1]={& _tmp576};Cyc_printf(({const char*
_tmp575="%s";_tag_arr(_tmp575,sizeof(char),_get_zero_arr_size(_tmp575,3));}),
_tag_arr(_tmp574,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _tagged_arr _tmp577=Cyc_strconcat(({const char*_tmp57C="`";_tag_arr(_tmp57C,
sizeof(char),_get_zero_arr_size(_tmp57C,2));}),(struct _tagged_arr)*((struct
_tagged_arr*)t->name));({struct _tagged_arr _tmp578=_tagged_arr_plus(_tmp577,
sizeof(char),1);char _tmp579=*((char*)_check_unknown_subscript(_tmp578,sizeof(
char),0));char _tmp57A='t';if(_get_arr_size(_tmp578,sizeof(char))== 1?_tmp579 == '\000'?
_tmp57A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp578.curr)=_tmp57A;});t->name=({
struct _tagged_arr*_tmp57B=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp57B[0]=(
struct _tagged_arr)_tmp577;_tmp57B;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp57D=_cycalloc(sizeof(*_tmp57D));
_tmp57D->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp57E=_cycalloc(sizeof(*
_tmp57E));_tmp57E->v=(*((struct _tuple13*)x)).f1;_tmp57E;});_tmp57D->f2=(*((
struct _tuple13*)x)).f2;_tmp57D->f3=(*((struct _tuple13*)x)).f3;_tmp57D;});}void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct
Cyc_List_List*_tmp57F=0;{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp57F=({struct Cyc_List_List*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->hd=(void*)((void*)atts->hd);_tmp580->tl=
_tmp57F;_tmp580;});}}return(void*)({struct Cyc_Absyn_FnType_struct*_tmp581=
_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_Absyn_FnType_struct _tmp582;
_tmp582.tag=8;_tmp582.f1=({struct Cyc_Absyn_FnInfo _tmp583;_tmp583.tvars=fd->tvs;
_tmp583.effect=fd->effect;_tmp583.ret_typ=(void*)((void*)fd->ret_type);_tmp583.args=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp583.c_varargs=fd->c_varargs;
_tmp583.cyc_varargs=fd->cyc_varargs;_tmp583.rgn_po=fd->rgn_po;_tmp583.attributes=
_tmp57F;_tmp583;});_tmp582;});_tmp581;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*t){return(*((struct
_tuple14*)t)).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*((struct
_tuple4*)t)).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){
return({struct _tuple4*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->f1=(*((struct
_tuple4*)pr)).f1;_tmp584->f2=t;_tmp584;});}struct _tuple15{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple16{struct _tuple15*f1;void*f2;};static
struct _tuple16*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple2*y){
return({struct _tuple16*_tmp585=_region_malloc(rgn,sizeof(*_tmp585));_tmp585->f1=({
struct _tuple15*_tmp586=_region_malloc(rgn,sizeof(*_tmp586));_tmp586->f1=(*((
struct _tuple2*)y)).f1;_tmp586->f2=(*((struct _tuple2*)y)).f2;_tmp586;});_tmp585->f2=(*((
struct _tuple2*)y)).f3;_tmp585;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){struct _tuple15*_tmp588;void*_tmp589;struct _tuple16 _tmp587=*((
struct _tuple16*)w);_tmp588=_tmp587.f1;_tmp589=_tmp587.f2;{struct Cyc_Core_Opt*
_tmp58B;struct Cyc_Absyn_Tqual _tmp58C;struct _tuple15 _tmp58A=*((struct _tuple15*)
_tmp588);_tmp58B=_tmp58A.f1;_tmp58C=_tmp58A.f2;return({struct _tuple2*_tmp58D=
_cycalloc(sizeof(*_tmp58D));_tmp58D->f1=_tmp58B;_tmp58D->f2=_tmp58C;_tmp58D->f3=
_tmp589;_tmp58D;});}}static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp58E=
_cycalloc(sizeof(*_tmp58E));_tmp58E->name=f->name;_tmp58E->tq=f->tq;_tmp58E->type=(
void*)t;_tmp58E->width=f->width;_tmp58E->attributes=f->attributes;_tmp58E;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp58F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp590;struct Cyc_Absyn_AggrInfo _tmp591;void*_tmp592;struct Cyc_List_List*_tmp593;
struct Cyc_Absyn_TunionInfo _tmp594;void*_tmp595;struct Cyc_List_List*_tmp596;void*
_tmp597;struct Cyc_Absyn_TunionFieldInfo _tmp598;void*_tmp599;struct Cyc_List_List*
_tmp59A;struct _tuple1*_tmp59B;struct Cyc_List_List*_tmp59C;struct Cyc_Absyn_Typedefdecl*
_tmp59D;struct Cyc_Core_Opt*_tmp59E;struct Cyc_Absyn_ArrayInfo _tmp59F;void*_tmp5A0;
struct Cyc_Absyn_Tqual _tmp5A1;struct Cyc_Absyn_Exp*_tmp5A2;struct Cyc_Absyn_Conref*
_tmp5A3;struct Cyc_Absyn_PtrInfo _tmp5A4;void*_tmp5A5;void*_tmp5A6;struct Cyc_Absyn_Conref*
_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;struct Cyc_Absyn_Conref*_tmp5A9;struct Cyc_Absyn_Conref*
_tmp5AA;struct Cyc_Absyn_FnInfo _tmp5AB;struct Cyc_List_List*_tmp5AC;struct Cyc_Core_Opt*
_tmp5AD;void*_tmp5AE;struct Cyc_List_List*_tmp5AF;int _tmp5B0;struct Cyc_Absyn_VarargInfo*
_tmp5B1;struct Cyc_List_List*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct Cyc_List_List*
_tmp5B4;void*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct Cyc_Core_Opt*_tmp5B7;void*
_tmp5B8;void*_tmp5B9;void*_tmp5BA;void*_tmp5BB;void*_tmp5BC;struct Cyc_List_List*
_tmp5BD;_LL3E8: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 1)goto _LL3EA;_tmp590=((
struct Cyc_Absyn_VarType_struct*)_tmp58F)->f1;_LL3E9: {struct _handler_cons _tmp5BE;
_push_handler(& _tmp5BE);{int _tmp5C0=0;if(setjmp(_tmp5BE.handler))_tmp5C0=1;if(!
_tmp5C0){{void*_tmp5C1=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp590);_npop_handler(0);return _tmp5C1;};_pop_handler();}else{void*_tmp5BF=(
void*)_exn_thrown;void*_tmp5C3=_tmp5BF;_LL41B: if(_tmp5C3 != Cyc_Core_Not_found)
goto _LL41D;_LL41C: return t;_LL41D:;_LL41E:(void)_throw(_tmp5C3);_LL41A:;}}}_LL3EA:
if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 10)goto _LL3EC;_tmp591=((struct Cyc_Absyn_AggrType_struct*)
_tmp58F)->f1;_tmp592=(void*)_tmp591.aggr_info;_tmp593=_tmp591.targs;_LL3EB: {
struct Cyc_List_List*_tmp5C4=Cyc_Tcutil_substs(rgn,inst,_tmp593);return _tmp5C4 == 
_tmp593?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp5C5=_cycalloc(sizeof(*
_tmp5C5));_tmp5C5[0]=({struct Cyc_Absyn_AggrType_struct _tmp5C6;_tmp5C6.tag=10;
_tmp5C6.f1=({struct Cyc_Absyn_AggrInfo _tmp5C7;_tmp5C7.aggr_info=(void*)_tmp592;
_tmp5C7.targs=_tmp5C4;_tmp5C7;});_tmp5C6;});_tmp5C5;});}_LL3EC: if(_tmp58F <= (
void*)3?1:*((int*)_tmp58F)!= 2)goto _LL3EE;_tmp594=((struct Cyc_Absyn_TunionType_struct*)
_tmp58F)->f1;_tmp595=(void*)_tmp594.tunion_info;_tmp596=_tmp594.targs;_tmp597=(
void*)_tmp594.rgn;_LL3ED: {struct Cyc_List_List*_tmp5C8=Cyc_Tcutil_substs(rgn,
inst,_tmp596);void*_tmp5C9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp597);return(
_tmp5C8 == _tmp596?_tmp5C9 == _tmp597: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=({struct Cyc_Absyn_TunionType_struct
_tmp5CB;_tmp5CB.tag=2;_tmp5CB.f1=({struct Cyc_Absyn_TunionInfo _tmp5CC;_tmp5CC.tunion_info=(
void*)_tmp595;_tmp5CC.targs=_tmp5C8;_tmp5CC.rgn=(void*)_tmp5C9;_tmp5CC;});
_tmp5CB;});_tmp5CA;});}_LL3EE: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 3)goto
_LL3F0;_tmp598=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp58F)->f1;_tmp599=(
void*)_tmp598.field_info;_tmp59A=_tmp598.targs;_LL3EF: {struct Cyc_List_List*
_tmp5CD=Cyc_Tcutil_substs(rgn,inst,_tmp59A);return _tmp5CD == _tmp59A?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));
_tmp5CE[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp5CF;_tmp5CF.tag=3;_tmp5CF.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp5D0;_tmp5D0.field_info=(void*)_tmp599;_tmp5D0.targs=
_tmp5CD;_tmp5D0;});_tmp5CF;});_tmp5CE;});}_LL3F0: if(_tmp58F <= (void*)3?1:*((int*)
_tmp58F)!= 16)goto _LL3F2;_tmp59B=((struct Cyc_Absyn_TypedefType_struct*)_tmp58F)->f1;
_tmp59C=((struct Cyc_Absyn_TypedefType_struct*)_tmp58F)->f2;_tmp59D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp58F)->f3;_tmp59E=((struct Cyc_Absyn_TypedefType_struct*)_tmp58F)->f4;_LL3F1: {
struct Cyc_List_List*_tmp5D1=Cyc_Tcutil_substs(rgn,inst,_tmp59C);return _tmp5D1 == 
_tmp59C?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp5D2=_cycalloc(sizeof(*
_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_TypedefType_struct _tmp5D3;_tmp5D3.tag=16;
_tmp5D3.f1=_tmp59B;_tmp5D3.f2=_tmp5D1;_tmp5D3.f3=_tmp59D;_tmp5D3.f4=_tmp59E;
_tmp5D3;});_tmp5D2;});}_LL3F2: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 7)goto
_LL3F4;_tmp59F=((struct Cyc_Absyn_ArrayType_struct*)_tmp58F)->f1;_tmp5A0=(void*)
_tmp59F.elt_type;_tmp5A1=_tmp59F.tq;_tmp5A2=_tmp59F.num_elts;_tmp5A3=_tmp59F.zero_term;
_LL3F3: {void*_tmp5D4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5A0);return _tmp5D4 == 
_tmp5A0?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp5D5=_cycalloc(sizeof(*
_tmp5D5));_tmp5D5[0]=({struct Cyc_Absyn_ArrayType_struct _tmp5D6;_tmp5D6.tag=7;
_tmp5D6.f1=({struct Cyc_Absyn_ArrayInfo _tmp5D7;_tmp5D7.elt_type=(void*)_tmp5D4;
_tmp5D7.tq=_tmp5A1;_tmp5D7.num_elts=_tmp5A2;_tmp5D7.zero_term=_tmp5A3;_tmp5D7;});
_tmp5D6;});_tmp5D5;});}_LL3F4: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 4)goto
_LL3F6;_tmp5A4=((struct Cyc_Absyn_PointerType_struct*)_tmp58F)->f1;_tmp5A5=(void*)
_tmp5A4.elt_typ;_tmp5A6=(void*)_tmp5A4.rgn_typ;_tmp5A7=_tmp5A4.nullable;_tmp5A8=
_tmp5A4.tq;_tmp5A9=_tmp5A4.bounds;_tmp5AA=_tmp5A4.zero_term;_LL3F5: {void*
_tmp5D8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5A5);void*_tmp5D9=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp5A6);if(_tmp5D8 == _tmp5A5?_tmp5D9 == _tmp5A6: 0)return t;return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA[0]=({
struct Cyc_Absyn_PointerType_struct _tmp5DB;_tmp5DB.tag=4;_tmp5DB.f1=({struct Cyc_Absyn_PtrInfo
_tmp5DC;_tmp5DC.elt_typ=(void*)_tmp5D8;_tmp5DC.rgn_typ=(void*)_tmp5D9;_tmp5DC.nullable=
_tmp5A7;_tmp5DC.tq=_tmp5A8;_tmp5DC.bounds=_tmp5A9;_tmp5DC.zero_term=_tmp5AA;
_tmp5DC;});_tmp5DB;});_tmp5DA;});}_LL3F6: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)
!= 8)goto _LL3F8;_tmp5AB=((struct Cyc_Absyn_FnType_struct*)_tmp58F)->f1;_tmp5AC=
_tmp5AB.tvars;_tmp5AD=_tmp5AB.effect;_tmp5AE=(void*)_tmp5AB.ret_typ;_tmp5AF=
_tmp5AB.args;_tmp5B0=_tmp5AB.c_varargs;_tmp5B1=_tmp5AB.cyc_varargs;_tmp5B2=
_tmp5AB.rgn_po;_tmp5B3=_tmp5AB.attributes;_LL3F7:{struct Cyc_List_List*_tmp5DD=
_tmp5AC;for(0;_tmp5DD != 0;_tmp5DD=_tmp5DD->tl){inst=({struct Cyc_List_List*
_tmp5DE=_region_malloc(rgn,sizeof(*_tmp5DE));_tmp5DE->hd=({struct _tuple8*_tmp5DF=
_region_malloc(rgn,sizeof(*_tmp5DF));_tmp5DF->f1=(struct Cyc_Absyn_Tvar*)_tmp5DD->hd;
_tmp5DF->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp5E0=_cycalloc(sizeof(*
_tmp5E0));_tmp5E0[0]=({struct Cyc_Absyn_VarType_struct _tmp5E1;_tmp5E1.tag=1;
_tmp5E1.f1=(struct Cyc_Absyn_Tvar*)_tmp5DD->hd;_tmp5E1;});_tmp5E0;});_tmp5DF;});
_tmp5DE->tl=inst;_tmp5DE;});}}{struct Cyc_List_List*_tmp5E3;struct Cyc_List_List*
_tmp5E4;struct _tuple0 _tmp5E2=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp5AF));_tmp5E3=_tmp5E2.f1;_tmp5E4=_tmp5E2.f2;{struct Cyc_List_List*_tmp5E5=
Cyc_Tcutil_substs(rgn,inst,_tmp5E4);struct Cyc_List_List*_tmp5E6=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp5E3,_tmp5E5));struct Cyc_Core_Opt*
eff2;if(_tmp5AD == 0)eff2=0;else{void*_tmp5E7=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp5AD->v);if(_tmp5E7 == (void*)_tmp5AD->v)eff2=_tmp5AD;else{eff2=({struct
Cyc_Core_Opt*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->v=(void*)_tmp5E7;
_tmp5E8;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp5B1 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp5EA;struct Cyc_Absyn_Tqual _tmp5EB;void*
_tmp5EC;int _tmp5ED;struct Cyc_Absyn_VarargInfo _tmp5E9=*((struct Cyc_Absyn_VarargInfo*)
_tmp5B1);_tmp5EA=_tmp5E9.name;_tmp5EB=_tmp5E9.tq;_tmp5EC=(void*)_tmp5E9.type;
_tmp5ED=_tmp5E9.inject;{void*_tmp5EE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5EC);
if(_tmp5EE == _tmp5EC)cyc_varargs2=_tmp5B1;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*
_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->name=_tmp5EA;_tmp5EF->tq=_tmp5EB;
_tmp5EF->type=(void*)_tmp5EE;_tmp5EF->inject=_tmp5ED;_tmp5EF;});}}}{struct Cyc_List_List*
rgn_po2;struct Cyc_List_List*_tmp5F1;struct Cyc_List_List*_tmp5F2;struct _tuple0
_tmp5F0=Cyc_List_rsplit(rgn,rgn,_tmp5B2);_tmp5F1=_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;{
struct Cyc_List_List*_tmp5F3=Cyc_Tcutil_substs(rgn,inst,_tmp5F1);struct Cyc_List_List*
_tmp5F4=Cyc_Tcutil_substs(rgn,inst,_tmp5F2);if(_tmp5F3 == _tmp5F1?_tmp5F4 == 
_tmp5F2: 0)rgn_po2=_tmp5B2;else{rgn_po2=Cyc_List_zip(_tmp5F3,_tmp5F4);}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));
_tmp5F5[0]=({struct Cyc_Absyn_FnType_struct _tmp5F6;_tmp5F6.tag=8;_tmp5F6.f1=({
struct Cyc_Absyn_FnInfo _tmp5F7;_tmp5F7.tvars=_tmp5AC;_tmp5F7.effect=eff2;_tmp5F7.ret_typ=(
void*)Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5AE);_tmp5F7.args=_tmp5E6;_tmp5F7.c_varargs=
_tmp5B0;_tmp5F7.cyc_varargs=cyc_varargs2;_tmp5F7.rgn_po=rgn_po2;_tmp5F7.attributes=
_tmp5B3;_tmp5F7;});_tmp5F6;});_tmp5F5;});}}}}}_LL3F8: if(_tmp58F <= (void*)3?1:*((
int*)_tmp58F)!= 9)goto _LL3FA;_tmp5B4=((struct Cyc_Absyn_TupleType_struct*)_tmp58F)->f1;
_LL3F9: {struct Cyc_List_List*_tmp5F8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_snd_tqt,_tmp5B4);struct Cyc_List_List*_tmp5F9=Cyc_Tcutil_substs(
rgn,inst,_tmp5F8);if(_tmp5F9 == _tmp5F8)return t;{struct Cyc_List_List*_tmp5FA=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp5B4,_tmp5F9);
return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp5FB=_cycalloc(sizeof(*
_tmp5FB));_tmp5FB[0]=({struct Cyc_Absyn_TupleType_struct _tmp5FC;_tmp5FC.tag=9;
_tmp5FC.f1=_tmp5FA;_tmp5FC;});_tmp5FB;});}}_LL3FA: if(_tmp58F <= (void*)3?1:*((int*)
_tmp58F)!= 11)goto _LL3FC;_tmp5B5=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp58F)->f1;_tmp5B6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp58F)->f2;_LL3FB: {
struct Cyc_List_List*_tmp5FD=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp5B6);struct Cyc_List_List*_tmp5FE=Cyc_Tcutil_substs(rgn,inst,_tmp5FD);if(
_tmp5FE == _tmp5FD)return t;{struct Cyc_List_List*_tmp5FF=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp5B6,
_tmp5FE);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp600=_cycalloc(
sizeof(*_tmp600));_tmp600[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp601;
_tmp601.tag=11;_tmp601.f1=(void*)_tmp5B5;_tmp601.f2=_tmp5FF;_tmp601;});_tmp600;});}}
_LL3FC: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 0)goto _LL3FE;_tmp5B7=((struct
Cyc_Absyn_Evar_struct*)_tmp58F)->f2;_LL3FD: if(_tmp5B7 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp5B7->v);else{return t;}_LL3FE: if(_tmp58F <= (void*)3?1:*((int*)
_tmp58F)!= 15)goto _LL400;_tmp5B8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp58F)->f1;_LL3FF: {void*_tmp602=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B8);
return _tmp602 == _tmp5B8?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp603=
_cycalloc(sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp604;_tmp604.tag=15;_tmp604.f1=(void*)_tmp602;_tmp604;});_tmp603;});}_LL400:
if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 14)goto _LL402;_tmp5B9=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp58F)->f1;_LL401: {void*_tmp605=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp5B9);return _tmp605 == _tmp5B9?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp607;_tmp607.tag=14;_tmp607.f1=(void*)_tmp605;_tmp607;});_tmp606;});}_LL402:
if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 17)goto _LL404;_tmp5BA=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp58F)->f1;_LL403: {void*_tmp608=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp5BA);return _tmp608 == _tmp5BA?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_TagType_struct
_tmp60A;_tmp60A.tag=17;_tmp60A.f1=(void*)_tmp608;_tmp60A;});_tmp609;});}_LL404:
if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 18)goto _LL406;_LL405: goto _LL407;
_LL406: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 12)goto _LL408;_LL407: goto
_LL409;_LL408: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 13)goto _LL40A;_LL409:
goto _LL40B;_LL40A: if((int)_tmp58F != 0)goto _LL40C;_LL40B: goto _LL40D;_LL40C: if(
_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 5)goto _LL40E;_LL40D: goto _LL40F;_LL40E:
if((int)_tmp58F != 1)goto _LL410;_LL40F: goto _LL411;_LL410: if(_tmp58F <= (void*)3?1:*((
int*)_tmp58F)!= 6)goto _LL412;_LL411: goto _LL413;_LL412: if((int)_tmp58F != 2)goto
_LL414;_LL413: return t;_LL414: if(_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 21)goto
_LL416;_tmp5BB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp58F)->f1;_LL415: {
void*_tmp60B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BB);return _tmp60B == _tmp5BB?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));
_tmp60C[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp60D;_tmp60D.tag=21;_tmp60D.f1=(
void*)_tmp60B;_tmp60D;});_tmp60C;});}_LL416: if(_tmp58F <= (void*)3?1:*((int*)
_tmp58F)!= 19)goto _LL418;_tmp5BC=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp58F)->f1;_LL417: {void*_tmp60E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BC);
return _tmp60E == _tmp5BC?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp60F=
_cycalloc(sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp610;
_tmp610.tag=19;_tmp610.f1=(void*)_tmp60E;_tmp610;});_tmp60F;});}_LL418: if(
_tmp58F <= (void*)3?1:*((int*)_tmp58F)!= 20)goto _LL3E7;_tmp5BD=((struct Cyc_Absyn_JoinEff_struct*)
_tmp58F)->f1;_LL419: {struct Cyc_List_List*_tmp611=Cyc_Tcutil_substs(rgn,inst,
_tmp5BD);return _tmp611 == _tmp5BD?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp613;_tmp613.tag=20;_tmp613.f1=_tmp611;_tmp613;});_tmp612;});}_LL3E7:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp614=(void*)ts->hd;
struct Cyc_List_List*_tmp615=ts->tl;void*_tmp616=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp614);struct Cyc_List_List*_tmp617=Cyc_Tcutil_substs(rgn,inst,_tmp615);if(
_tmp614 == _tmp616?_tmp615 == _tmp617: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->hd=(void*)
_tmp616;_tmp618->tl=_tmp617;_tmp618;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp619=_cycalloc(
sizeof(*_tmp619));_tmp619->f1=tv;_tmp619->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->v=(void*)k;_tmp61A;}),({struct Cyc_Core_Opt*
_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->v=s;_tmp61B;}));_tmp619;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp61D;struct Cyc_List_List*_tmp61E;struct _RegionHandle*_tmp61F;
struct _tuple9*_tmp61C=env;_tmp61D=*_tmp61C;_tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp620=_region_malloc(
_tmp61F,sizeof(*_tmp620));_tmp620->f1=tv;_tmp620->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->v=(void*)k;_tmp621;}),({
struct Cyc_Core_Opt*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->v=_tmp61E;
_tmp622;}));_tmp620;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp627;_tmp627.tag=0;_tmp627.f1=(struct _tagged_arr)
Cyc_Absynpp_kindbound2string(k2);{struct Cyc_String_pa_struct _tmp626;_tmp626.tag=
0;_tmp626.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(k1);{struct Cyc_String_pa_struct
_tmp625;_tmp625.tag=0;_tmp625.f1=(struct _tagged_arr)*((struct _tagged_arr*)tv->name);{
void*_tmp623[3]={& _tmp625,& _tmp626,& _tmp627};Cyc_Tcutil_terr(loc,({const char*
_tmp624="type variable %s is used with inconsistent kinds %s and %s";_tag_arr(
_tmp624,sizeof(char),_get_zero_arr_size(_tmp624,59));}),_tag_arr(_tmp623,sizeof(
void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(*((int*)_check_null(tv->identity))!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity)))({void*_tmp628[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp629="same type variable has different identity!";
_tag_arr(_tmp629,sizeof(char),_get_zero_arr_size(_tmp629,43));}),_tag_arr(
_tmp628,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=tv;
_tmp62A->tl=tvs;_tmp62A;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp62B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp62C="fast_add_free_tvar: bad identity in tv";_tag_arr(_tmp62C,
sizeof(char),_get_zero_arr_size(_tmp62C,39));}),_tag_arr(_tmp62B,sizeof(void*),0));});{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity == 0)({void*_tmp62D[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp62E="fast_add_free_tvar: bad identity in tvs2";
_tag_arr(_tmp62E,sizeof(char),_get_zero_arr_size(_tmp62E,41));}),_tag_arr(
_tmp62D,sizeof(void*),0));});if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->hd=tv;_tmp62F->tl=tvs;_tmp62F;});}
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp632;
_tmp632.tag=0;_tmp632.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{void*
_tmp630[1]={& _tmp632};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp631="bound tvar id for %s is NULL";_tag_arr(_tmp631,sizeof(char),
_get_zero_arr_size(_tmp631,29));}),_tag_arr(_tmp630,sizeof(void*),1));}});return({
struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->hd=tv;_tmp633->tl=
tvs;_tmp633;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*
es,void*e){void*_tmp634=Cyc_Tcutil_compress(e);int _tmp635;_LL420: if(_tmp634 <= (
void*)3?1:*((int*)_tmp634)!= 0)goto _LL422;_tmp635=((struct Cyc_Absyn_Evar_struct*)
_tmp634)->f3;_LL421:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*
_tmp636=Cyc_Tcutil_compress((void*)es2->hd);int _tmp637;_LL425: if(_tmp636 <= (void*)
3?1:*((int*)_tmp636)!= 0)goto _LL427;_tmp637=((struct Cyc_Absyn_Evar_struct*)
_tmp636)->f3;_LL426: if(_tmp635 == _tmp637)return es;goto _LL424;_LL427:;_LL428: goto
_LL424;_LL424:;}}return({struct Cyc_List_List*_tmp638=_cycalloc(sizeof(*_tmp638));
_tmp638->hd=(void*)e;_tmp638->tl=es;_tmp638;});_LL422:;_LL423: return es;_LL41F:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!present)r=({
struct Cyc_List_List*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp639->tl=r;_tmp639;});}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_String_pa_struct _tmp63C;
_tmp63C.tag=0;_tmp63C.f1=(struct _tagged_arr)*((struct _tagged_arr*)fn);{void*
_tmp63A[1]={& _tmp63C};Cyc_Tcutil_terr(loc,({const char*_tmp63B="bitfield %s does not have constant width";
_tag_arr(_tmp63B,sizeof(char),_get_zero_arr_size(_tmp63B,41));}),_tag_arr(
_tmp63A,sizeof(void*),1));}});else{unsigned int _tmp63E;int _tmp63F;struct _tuple7
_tmp63D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(width));
_tmp63E=_tmp63D.f1;_tmp63F=_tmp63D.f2;if(!_tmp63F)({void*_tmp640[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp641="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp641,sizeof(char),_get_zero_arr_size(_tmp641,45));}),_tag_arr(_tmp640,sizeof(
void*),0));});w=_tmp63E;}{void*_tmp642=Cyc_Tcutil_compress(field_typ);void*
_tmp643;_LL42A: if(_tmp642 <= (void*)3?1:*((int*)_tmp642)!= 5)goto _LL42C;_tmp643=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp642)->f2;_LL42B:{void*_tmp644=
_tmp643;_LL42F: if((int)_tmp644 != 0)goto _LL431;_LL430: if(w > 8)({void*_tmp645[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp646="bitfield larger than type";_tag_arr(
_tmp646,sizeof(char),_get_zero_arr_size(_tmp646,26));}),_tag_arr(_tmp645,sizeof(
void*),0));});goto _LL42E;_LL431: if((int)_tmp644 != 1)goto _LL433;_LL432: if(w > 16)({
void*_tmp647[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp648="bitfield larger than type";
_tag_arr(_tmp648,sizeof(char),_get_zero_arr_size(_tmp648,26));}),_tag_arr(
_tmp647,sizeof(void*),0));});goto _LL42E;_LL433: if((int)_tmp644 != 2)goto _LL435;
_LL434: if(w > 32)({void*_tmp649[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp64A="bitfield larger than type";
_tag_arr(_tmp64A,sizeof(char),_get_zero_arr_size(_tmp64A,26));}),_tag_arr(
_tmp649,sizeof(void*),0));});goto _LL42E;_LL435: if((int)_tmp644 != 3)goto _LL42E;
_LL436: if(w > 64)({void*_tmp64B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp64C="bitfield larger than type";
_tag_arr(_tmp64C,sizeof(char),_get_zero_arr_size(_tmp64C,26));}),_tag_arr(
_tmp64B,sizeof(void*),0));});goto _LL42E;_LL42E:;}goto _LL429;_LL42C:;_LL42D:({
struct Cyc_String_pa_struct _tmp650;_tmp650.tag=0;_tmp650.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(field_typ);{struct Cyc_String_pa_struct _tmp64F;_tmp64F.tag=
0;_tmp64F.f1=(struct _tagged_arr)*((struct _tagged_arr*)fn);{void*_tmp64D[2]={&
_tmp64F,& _tmp650};Cyc_Tcutil_terr(loc,({const char*_tmp64E="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp64E,sizeof(char),_get_zero_arr_size(_tmp64E,52));}),_tag_arr(
_tmp64D,sizeof(void*),2));}}});goto _LL429;_LL429:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp651=(void*)atts->hd;_LL438: if((int)
_tmp651 != 5)goto _LL43A;_LL439: continue;_LL43A: if(_tmp651 <= (void*)17?1:*((int*)
_tmp651)!= 1)goto _LL43C;_LL43B: continue;_LL43C:;_LL43D:({struct Cyc_String_pa_struct
_tmp655;_tmp655.tag=0;_tmp655.f1=(struct _tagged_arr)*((struct _tagged_arr*)fn);{
struct Cyc_String_pa_struct _tmp654;_tmp654.tag=0;_tmp654.f1=(struct _tagged_arr)
Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp652[2]={& _tmp654,& _tmp655};
Cyc_Tcutil_terr(loc,({const char*_tmp653="bad attribute %s on member %s";_tag_arr(
_tmp653,sizeof(char),_get_zero_arr_size(_tmp653,30));}),_tag_arr(_tmp652,sizeof(
void*),2));}}});_LL437:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*kind_env;
struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;
int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t){{void*_tmp656=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp657;struct Cyc_Core_Opt**_tmp658;struct Cyc_Core_Opt*_tmp659;struct Cyc_Core_Opt**
_tmp65A;struct Cyc_Absyn_Tvar*_tmp65B;struct Cyc_List_List*_tmp65C;struct _tuple1*
_tmp65D;struct Cyc_Absyn_Enumdecl*_tmp65E;struct Cyc_Absyn_Enumdecl**_tmp65F;
struct Cyc_Absyn_TunionInfo _tmp660;void*_tmp661;void**_tmp662;struct Cyc_List_List*
_tmp663;struct Cyc_List_List**_tmp664;void*_tmp665;struct Cyc_Absyn_TunionFieldInfo
_tmp666;void*_tmp667;void**_tmp668;struct Cyc_List_List*_tmp669;struct Cyc_Absyn_PtrInfo
_tmp66A;void*_tmp66B;void*_tmp66C;struct Cyc_Absyn_Conref*_tmp66D;struct Cyc_Absyn_Tqual
_tmp66E;struct Cyc_Absyn_Conref*_tmp66F;struct Cyc_Absyn_Conref*_tmp670;void*
_tmp671;void*_tmp672;struct Cyc_Absyn_ArrayInfo _tmp673;void*_tmp674;struct Cyc_Absyn_Tqual
_tmp675;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Conref*_tmp677;struct Cyc_Absyn_FnInfo
_tmp678;struct Cyc_List_List*_tmp679;struct Cyc_List_List**_tmp67A;struct Cyc_Core_Opt*
_tmp67B;struct Cyc_Core_Opt**_tmp67C;void*_tmp67D;struct Cyc_List_List*_tmp67E;int
_tmp67F;struct Cyc_Absyn_VarargInfo*_tmp680;struct Cyc_List_List*_tmp681;struct Cyc_List_List*
_tmp682;struct Cyc_List_List*_tmp683;void*_tmp684;struct Cyc_List_List*_tmp685;
struct Cyc_Absyn_AggrInfo _tmp686;void*_tmp687;void**_tmp688;struct Cyc_List_List*
_tmp689;struct Cyc_List_List**_tmp68A;struct _tuple1*_tmp68B;struct Cyc_List_List*
_tmp68C;struct Cyc_List_List**_tmp68D;struct Cyc_Absyn_Typedefdecl*_tmp68E;struct
Cyc_Absyn_Typedefdecl**_tmp68F;struct Cyc_Core_Opt*_tmp690;struct Cyc_Core_Opt**
_tmp691;void*_tmp692;void*_tmp693;void*_tmp694;struct Cyc_List_List*_tmp695;
_LL43F: if((int)_tmp656 != 0)goto _LL441;_LL440: goto _LL43E;_LL441: if(_tmp656 <= (
void*)3?1:*((int*)_tmp656)!= 0)goto _LL443;_tmp657=((struct Cyc_Absyn_Evar_struct*)
_tmp656)->f1;_tmp658=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp656)->f1;_tmp659=((struct Cyc_Absyn_Evar_struct*)_tmp656)->f2;_tmp65A=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp656)->f2;_LL442: if(*((struct
Cyc_Core_Opt**)_tmp658)== 0)*((struct Cyc_Core_Opt**)_tmp658)=({struct Cyc_Core_Opt*
_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->v=(void*)expected_kind;_tmp696;});
if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?expected_kind == (void*)3: 0)*((
struct Cyc_Core_Opt**)_tmp65A)=({struct Cyc_Core_Opt*_tmp697=_cycalloc(sizeof(*
_tmp697));_tmp697->v=(void*)((void*)2);_tmp697;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp698=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp69D;_tmp69D.tag=2;_tmp69D.f1=0;_tmp69D.f2=(void*)expected_kind;_tmp69D;});
_tmp69C;}));*((struct Cyc_Core_Opt**)_tmp65A)=({struct Cyc_Core_Opt*_tmp699=
_cycalloc(sizeof(*_tmp699));_tmp699->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_VarType_struct
_tmp69B;_tmp69B.tag=1;_tmp69B.f1=_tmp698;_tmp69B;});_tmp69A;}));_tmp699;});
_tmp65B=_tmp698;goto _LL444;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,t);}}goto _LL43E;_LL443: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)
!= 1)goto _LL445;_tmp65B=((struct Cyc_Absyn_VarType_struct*)_tmp656)->f1;_LL444:{
void*_tmp69E=Cyc_Absyn_compress_kb((void*)_tmp65B->kind);struct Cyc_Core_Opt*
_tmp69F;struct Cyc_Core_Opt**_tmp6A0;_LL472: if(*((int*)_tmp69E)!= 1)goto _LL474;
_tmp69F=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp69E)->f1;_tmp6A0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp69E)->f1;_LL473:*((struct Cyc_Core_Opt**)
_tmp6A0)=({struct Cyc_Core_Opt*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));
_tmp6A2[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6A3;_tmp6A3.tag=2;_tmp6A3.f1=0;
_tmp6A3.f2=(void*)expected_kind;_tmp6A3;});_tmp6A2;}));_tmp6A1;});goto _LL471;
_LL474:;_LL475: goto _LL471;_LL471:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmp65B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,
_tmp65B);goto _LL43E;_LL445: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 13)goto
_LL447;_tmp65C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp656)->f1;_LL446: {
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle _tmp6A4=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp6A4;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;for(0;
_tmp65C != 0;_tmp65C=_tmp65C->tl){struct Cyc_Absyn_Enumfield*_tmp6A5=(struct Cyc_Absyn_Enumfield*)
_tmp65C->hd;if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,(*((struct _tuple1*)_tmp6A5->name)).f2))({struct Cyc_String_pa_struct
_tmp6A8;_tmp6A8.tag=0;_tmp6A8.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((
struct _tuple1*)_tmp6A5->name)).f2);{void*_tmp6A6[1]={& _tmp6A8};Cyc_Tcutil_terr(
_tmp6A5->loc,({const char*_tmp6A7="duplicate enum field name %s";_tag_arr(_tmp6A7,
sizeof(char),_get_zero_arr_size(_tmp6A7,29));}),_tag_arr(_tmp6A6,sizeof(void*),1));}});
else{prev_fields=({struct Cyc_List_List*_tmp6A9=_region_malloc(uprev_rgn,sizeof(*
_tmp6A9));_tmp6A9->hd=(*((struct _tuple1*)_tmp6A5->name)).f2;_tmp6A9->tl=
prev_fields;_tmp6A9;});}if(_tmp6A5->tag == 0)_tmp6A5->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp6A5->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp6A5->tag)))({struct Cyc_String_pa_struct
_tmp6AC;_tmp6AC.tag=0;_tmp6AC.f1=(struct _tagged_arr)*((struct _tagged_arr*)(*((
struct _tuple1*)_tmp6A5->name)).f2);{void*_tmp6AA[1]={& _tmp6AC};Cyc_Tcutil_terr(
loc,({const char*_tmp6AB="enum field %s: expression is not constant";_tag_arr(
_tmp6AB,sizeof(char),_get_zero_arr_size(_tmp6AB,42));}),_tag_arr(_tmp6AA,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp6A5->tag))).f1;tag_count=t1 + 1;(*((struct _tuple1*)_tmp6A5->name)).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp6AE;_tmp6AE.tag=1;_tmp6AE.f1=te->ns;_tmp6AE;});
_tmp6AD;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*((struct
_tuple1*)_tmp6A5->name)).f2,({struct _tuple17*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));
_tmp6AF->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp6B0=_cycalloc(
sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp6B1;_tmp6B1.tag=
4;_tmp6B1.f1=(void*)t;_tmp6B1.f2=_tmp6A5;_tmp6B1;});_tmp6B0;});_tmp6AF->f2=1;
_tmp6AF;}));}}};_pop_region(uprev_rgn);}goto _LL43E;}_LL447: if(_tmp656 <= (void*)3?
1:*((int*)_tmp656)!= 12)goto _LL449;_tmp65D=((struct Cyc_Absyn_EnumType_struct*)
_tmp656)->f1;_tmp65E=((struct Cyc_Absyn_EnumType_struct*)_tmp656)->f2;_tmp65F=(
struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)_tmp656)->f2;
_LL448: if(*((struct Cyc_Absyn_Enumdecl**)_tmp65F)== 0?1:((struct Cyc_Absyn_Enumdecl*)
_check_null(*((struct Cyc_Absyn_Enumdecl**)_tmp65F)))->fields == 0){struct
_handler_cons _tmp6B2;_push_handler(& _tmp6B2);{int _tmp6B4=0;if(setjmp(_tmp6B2.handler))
_tmp6B4=1;if(!_tmp6B4){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp65D);*((struct Cyc_Absyn_Enumdecl**)_tmp65F)=(struct Cyc_Absyn_Enumdecl*)*((
struct Cyc_Absyn_Enumdecl**)ed);};_pop_handler();}else{void*_tmp6B3=(void*)
_exn_thrown;void*_tmp6B6=_tmp6B3;_LL477: if(_tmp6B6 != Cyc_Dict_Absent)goto _LL479;
_LL478: {struct Cyc_Tcenv_Genv*_tmp6B7=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*
_tmp6B8=({struct Cyc_Absyn_Enumdecl*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->sc=(
void*)((void*)3);_tmp6B9->name=_tmp65D;_tmp6B9->fields=0;_tmp6B9;});Cyc_Tc_tcEnumdecl(
te,_tmp6B7,loc,_tmp6B8);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp65D);*((struct Cyc_Absyn_Enumdecl**)_tmp65F)=(struct Cyc_Absyn_Enumdecl*)*((
struct Cyc_Absyn_Enumdecl**)ed);goto _LL476;}}_LL479:;_LL47A:(void)_throw(_tmp6B6);
_LL476:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*((
struct Cyc_Absyn_Enumdecl**)_tmp65F));*((struct _tuple1*)_tmp65D)=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL43E;}_LL449: if(_tmp656 <= (void*)3?1:*((
int*)_tmp656)!= 2)goto _LL44B;_tmp660=((struct Cyc_Absyn_TunionType_struct*)
_tmp656)->f1;_tmp661=(void*)_tmp660.tunion_info;_tmp662=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp656)->f1).tunion_info;_tmp663=_tmp660.targs;_tmp664=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp656)->f1).targs;_tmp665=(void*)_tmp660.rgn;
_LL44A: {struct Cyc_List_List*_tmp6BA=*((struct Cyc_List_List**)_tmp664);{void*
_tmp6BB=*((void**)_tmp662);struct Cyc_Absyn_UnknownTunionInfo _tmp6BC;struct
_tuple1*_tmp6BD;int _tmp6BE;struct Cyc_Absyn_Tuniondecl**_tmp6BF;struct Cyc_Absyn_Tuniondecl*
_tmp6C0;_LL47C: if(*((int*)_tmp6BB)!= 0)goto _LL47E;_tmp6BC=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp6BB)->f1;_tmp6BD=_tmp6BC.name;_tmp6BE=_tmp6BC.is_xtunion;_LL47D: {struct Cyc_Absyn_Tuniondecl**
tudp;{struct _handler_cons _tmp6C1;_push_handler(& _tmp6C1);{int _tmp6C3=0;if(setjmp(
_tmp6C1.handler))_tmp6C3=1;if(!_tmp6C3){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp6BD);;_pop_handler();}else{void*_tmp6C2=(void*)_exn_thrown;void*_tmp6C5=
_tmp6C2;_LL481: if(_tmp6C5 != Cyc_Dict_Absent)goto _LL483;_LL482: {struct Cyc_Tcenv_Genv*
_tmp6C6=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp6C7=({struct Cyc_Absyn_Tuniondecl*
_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->sc=(void*)((void*)3);_tmp6CE->name=
_tmp6BD;_tmp6CE->tvs=0;_tmp6CE->fields=0;_tmp6CE->is_xtunion=_tmp6BE;_tmp6CE;});
Cyc_Tc_tcTuniondecl(te,_tmp6C6,loc,_tmp6C7);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp6BD);if(_tmp6BA != 0){({struct Cyc_String_pa_struct _tmp6CB;_tmp6CB.tag=0;
_tmp6CB.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6BD);{struct Cyc_String_pa_struct
_tmp6CA;_tmp6CA.tag=0;_tmp6CA.f1=(struct _tagged_arr)(_tmp6BE?({const char*_tmp6CC="xtunion";
_tag_arr(_tmp6CC,sizeof(char),_get_zero_arr_size(_tmp6CC,8));}):({const char*
_tmp6CD="tunion";_tag_arr(_tmp6CD,sizeof(char),_get_zero_arr_size(_tmp6CD,7));}));{
void*_tmp6C8[2]={& _tmp6CA,& _tmp6CB};Cyc_Tcutil_terr(loc,({const char*_tmp6C9="please declare parameterized %s %s before using";
_tag_arr(_tmp6C9,sizeof(char),_get_zero_arr_size(_tmp6C9,48));}),_tag_arr(
_tmp6C8,sizeof(void*),2));}}});return cvtenv;}goto _LL480;}_LL483:;_LL484:(void)
_throw(_tmp6C5);_LL480:;}}}if((*((struct Cyc_Absyn_Tuniondecl**)tudp))->is_xtunion
!= _tmp6BE)({struct Cyc_String_pa_struct _tmp6D1;_tmp6D1.tag=0;_tmp6D1.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp6BD);{void*_tmp6CF[1]={& _tmp6D1};Cyc_Tcutil_terr(
loc,({const char*_tmp6D0="[x]tunion is different from type declaration %s";
_tag_arr(_tmp6D0,sizeof(char),_get_zero_arr_size(_tmp6D0,48));}),_tag_arr(
_tmp6CF,sizeof(void*),1));}});*((void**)_tmp662)=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp6D3;_tmp6D3.tag=1;_tmp6D3.f1=tudp;_tmp6D3;});_tmp6D2;});_tmp6C0=*((struct Cyc_Absyn_Tuniondecl**)
tudp);goto _LL47F;}_LL47E: if(*((int*)_tmp6BB)!= 1)goto _LL47B;_tmp6BF=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6BB)->f1;_tmp6C0=*_tmp6BF;_LL47F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp665);{struct Cyc_List_List*tvs=_tmp6C0->tvs;for(0;_tmp6BA != 0?
tvs != 0: 0;(_tmp6BA=_tmp6BA->tl,tvs=tvs->tl)){void*t1=(void*)_tmp6BA->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp6BA != 0)({struct Cyc_String_pa_struct _tmp6D6;_tmp6D6.tag=
0;_tmp6D6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6C0->name);{void*
_tmp6D4[1]={& _tmp6D6};Cyc_Tcutil_terr(loc,({const char*_tmp6D5="too many type arguments for tunion %s";
_tag_arr(_tmp6D5,sizeof(char),_get_zero_arr_size(_tmp6D5,38));}),_tag_arr(
_tmp6D4,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp6D7=_cycalloc(
sizeof(*_tmp6D7));_tmp6D7->hd=(void*)e;_tmp6D7->tl=hidden_ts;_tmp6D7;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*((struct Cyc_List_List**)
_tmp664)=Cyc_List_imp_append(*((struct Cyc_List_List**)_tmp664),Cyc_List_imp_rev(
hidden_ts));}goto _LL47B;}_LL47B:;}goto _LL43E;}_LL44B: if(_tmp656 <= (void*)3?1:*((
int*)_tmp656)!= 3)goto _LL44D;_tmp666=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp656)->f1;_tmp667=(void*)_tmp666.field_info;_tmp668=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp656)->f1).field_info;_tmp669=_tmp666.targs;_LL44C:{void*_tmp6D8=*((void**)
_tmp668);struct Cyc_Absyn_UnknownTunionFieldInfo _tmp6D9;struct _tuple1*_tmp6DA;
struct _tuple1*_tmp6DB;int _tmp6DC;struct Cyc_Absyn_Tuniondecl*_tmp6DD;struct Cyc_Absyn_Tunionfield*
_tmp6DE;_LL486: if(*((int*)_tmp6D8)!= 0)goto _LL488;_tmp6D9=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp6D8)->f1;_tmp6DA=_tmp6D9.tunion_name;_tmp6DB=_tmp6D9.field_name;_tmp6DC=
_tmp6D9.is_xtunion;_LL487: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp6DF;_push_handler(& _tmp6DF);{int _tmp6E1=0;if(setjmp(
_tmp6DF.handler))_tmp6E1=1;if(!_tmp6E1){*((struct Cyc_Absyn_Tuniondecl**)Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp6DA));;_pop_handler();}else{void*_tmp6E0=(void*)_exn_thrown;void*
_tmp6E3=_tmp6E0;_LL48B: if(_tmp6E3 != Cyc_Dict_Absent)goto _LL48D;_LL48C:({struct
Cyc_String_pa_struct _tmp6E6;_tmp6E6.tag=0;_tmp6E6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DA);{void*_tmp6E4[1]={& _tmp6E6};Cyc_Tcutil_terr(loc,({const char*_tmp6E5="unbound type tunion %s";
_tag_arr(_tmp6E5,sizeof(char),_get_zero_arr_size(_tmp6E5,23));}),_tag_arr(
_tmp6E4,sizeof(void*),1));}});return cvtenv;_LL48D:;_LL48E:(void)_throw(_tmp6E3);
_LL48A:;}}}{struct _handler_cons _tmp6E7;_push_handler(& _tmp6E7);{int _tmp6E9=0;if(
setjmp(_tmp6E7.handler))_tmp6E9=1;if(!_tmp6E9){{void*_tmp6EA=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp6DB);struct Cyc_Absyn_Tuniondecl*_tmp6EB;struct Cyc_Absyn_Tunionfield*
_tmp6EC;_LL490: if(*((int*)_tmp6EA)!= 2)goto _LL492;_tmp6EB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp6EA)->f1;_tmp6EC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp6EA)->f2;_LL491: tuf=
_tmp6EC;tud=_tmp6EB;if(tud->is_xtunion != _tmp6DC)({struct Cyc_String_pa_struct
_tmp6EF;_tmp6EF.tag=0;_tmp6EF.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DA);{void*_tmp6ED[1]={& _tmp6EF};Cyc_Tcutil_terr(loc,({const char*_tmp6EE="[x]tunion is different from type declaration %s";
_tag_arr(_tmp6EE,sizeof(char),_get_zero_arr_size(_tmp6EE,48));}),_tag_arr(
_tmp6ED,sizeof(void*),1));}});goto _LL48F;_LL492:;_LL493:({struct Cyc_String_pa_struct
_tmp6F3;_tmp6F3.tag=0;_tmp6F3.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DA);{struct Cyc_String_pa_struct _tmp6F2;_tmp6F2.tag=0;_tmp6F2.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp6DB);{void*_tmp6F0[2]={& _tmp6F2,& _tmp6F3};
Cyc_Tcutil_terr(loc,({const char*_tmp6F1="unbound field %s in type tunion %s";
_tag_arr(_tmp6F1,sizeof(char),_get_zero_arr_size(_tmp6F1,35));}),_tag_arr(
_tmp6F0,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp6F4=cvtenv;
_npop_handler(0);return _tmp6F4;}_LL48F:;};_pop_handler();}else{void*_tmp6E8=(
void*)_exn_thrown;void*_tmp6F6=_tmp6E8;_LL495: if(_tmp6F6 != Cyc_Dict_Absent)goto
_LL497;_LL496:({struct Cyc_String_pa_struct _tmp6FA;_tmp6FA.tag=0;_tmp6FA.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6DA);{struct Cyc_String_pa_struct
_tmp6F9;_tmp6F9.tag=0;_tmp6F9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DB);{void*_tmp6F7[2]={& _tmp6F9,& _tmp6FA};Cyc_Tcutil_terr(loc,({const char*
_tmp6F8="unbound field %s in type tunion %s";_tag_arr(_tmp6F8,sizeof(char),
_get_zero_arr_size(_tmp6F8,35));}),_tag_arr(_tmp6F7,sizeof(void*),2));}}});
return cvtenv;_LL497:;_LL498:(void)_throw(_tmp6F6);_LL494:;}}}*((void**)_tmp668)=(
void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp6FB=_cycalloc(sizeof(*
_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmp6FC;_tmp6FC.tag=
1;_tmp6FC.f1=tud;_tmp6FC.f2=tuf;_tmp6FC;});_tmp6FB;});_tmp6DD=tud;_tmp6DE=tuf;
goto _LL489;}_LL488: if(*((int*)_tmp6D8)!= 1)goto _LL485;_tmp6DD=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp6D8)->f1;_tmp6DE=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp6D8)->f2;
_LL489: {struct Cyc_List_List*tvs=_tmp6DD->tvs;for(0;_tmp669 != 0?tvs != 0: 0;(
_tmp669=_tmp669->tl,tvs=tvs->tl)){void*t1=(void*)_tmp669->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1);}if(_tmp669 != 0)({struct Cyc_String_pa_struct _tmp700;_tmp700.tag=0;
_tmp700.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6DE->name);{struct Cyc_String_pa_struct
_tmp6FF;_tmp6FF.tag=0;_tmp6FF.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DD->name);{void*_tmp6FD[2]={& _tmp6FF,& _tmp700};Cyc_Tcutil_terr(loc,({const
char*_tmp6FE="too many type arguments for tunion %s.%s";_tag_arr(_tmp6FE,sizeof(
char),_get_zero_arr_size(_tmp6FE,41));}),_tag_arr(_tmp6FD,sizeof(void*),2));}}});
if(tvs != 0)({struct Cyc_String_pa_struct _tmp704;_tmp704.tag=0;_tmp704.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp6DE->name);{struct Cyc_String_pa_struct
_tmp703;_tmp703.tag=0;_tmp703.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DD->name);{void*_tmp701[2]={& _tmp703,& _tmp704};Cyc_Tcutil_terr(loc,({const
char*_tmp702="too few type arguments for tunion %s.%s";_tag_arr(_tmp702,sizeof(
char),_get_zero_arr_size(_tmp702,40));}),_tag_arr(_tmp701,sizeof(void*),2));}}});
goto _LL485;}_LL485:;}goto _LL43E;_LL44D: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)
!= 4)goto _LL44F;_tmp66A=((struct Cyc_Absyn_PointerType_struct*)_tmp656)->f1;
_tmp66B=(void*)_tmp66A.elt_typ;_tmp66C=(void*)_tmp66A.rgn_typ;_tmp66D=_tmp66A.nullable;
_tmp66E=_tmp66A.tq;_tmp66F=_tmp66A.bounds;_tmp670=_tmp66A.zero_term;_LL44E: {int
is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp66B);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp66C);{
void*_tmp705=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp670))->v;int _tmp706;_LL49A: if((int)_tmp705 != 0)goto _LL49C;_LL49B:{void*
_tmp707=Cyc_Tcutil_compress(_tmp66B);void*_tmp708;_LL4A1: if(_tmp707 <= (void*)3?1:*((
int*)_tmp707)!= 5)goto _LL4A3;_tmp708=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp707)->f2;if((int)_tmp708 != 0)goto _LL4A3;_LL4A2:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp670,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4A0;_LL4A3:;_LL4A4:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp670,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4A0;
_LL4A0:;}goto _LL499;_LL49C: if(_tmp705 <= (void*)1?1:*((int*)_tmp705)!= 0)goto
_LL49E;_tmp706=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp705)->f1;if(_tmp706
!= 1)goto _LL49E;_LL49D: if(!Cyc_Tcutil_admits_zero(_tmp66B))({struct Cyc_String_pa_struct
_tmp70B;_tmp70B.tag=0;_tmp70B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp66B);{void*_tmp709[1]={& _tmp70B};Cyc_Tcutil_terr(loc,({const char*_tmp70A="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp70A,sizeof(char),_get_zero_arr_size(_tmp70A,53));}),_tag_arr(
_tmp709,sizeof(void*),1));}});is_zero_terminated=1;goto _LL499;_LL49E:;_LL49F:
is_zero_terminated=0;goto _LL499;_LL499:;}{void*_tmp70C=(void*)(Cyc_Absyn_compress_conref(
_tmp66F))->v;void*_tmp70D;struct Cyc_Absyn_Exp*_tmp70E;_LL4A6: if(_tmp70C <= (void*)
1?1:*((int*)_tmp70C)!= 0)goto _LL4A8;_tmp70D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp70C)->f1;if(_tmp70D <= (void*)1?1:*((int*)_tmp70D)!= 0)goto _LL4A8;_tmp70E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp70D)->f1;_LL4A7: Cyc_Tcexp_tcExp(te,0,_tmp70E);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp70E))({void*_tmp70F[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp710="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp710,sizeof(char),_get_zero_arr_size(_tmp710,49));}),_tag_arr(
_tmp70F,sizeof(void*),0));});{unsigned int _tmp712;int _tmp713;struct _tuple7
_tmp711=Cyc_Evexp_eval_const_uint_exp(_tmp70E);_tmp712=_tmp711.f1;_tmp713=
_tmp711.f2;if(is_zero_terminated?!_tmp713?1: _tmp712 < 1: 0)({void*_tmp714[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp715="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp715,sizeof(char),_get_zero_arr_size(_tmp715,55));}),_tag_arr(
_tmp714,sizeof(void*),0));});goto _LL4A5;}_LL4A8:;_LL4A9: goto _LL4A5;_LL4A5:;}goto
_LL43E;}_LL44F: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 14)goto _LL451;_tmp671=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp656)->f1;_LL450: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp671);goto _LL43E;_LL451: if(_tmp656 <= (void*)3?1:*((int*)
_tmp656)!= 17)goto _LL453;_tmp672=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp656)->f1;_LL452: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp672);goto _LL43E;_LL453: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 18)goto
_LL455;_LL454: goto _LL456;_LL455: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 5)
goto _LL457;_LL456: goto _LL458;_LL457: if((int)_tmp656 != 1)goto _LL459;_LL458: goto
_LL45A;_LL459: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 6)goto _LL45B;_LL45A:
goto _LL43E;_LL45B: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 7)goto _LL45D;
_tmp673=((struct Cyc_Absyn_ArrayType_struct*)_tmp656)->f1;_tmp674=(void*)_tmp673.elt_type;
_tmp675=_tmp673.tq;_tmp676=_tmp673.num_elts;_tmp677=_tmp673.zero_term;_LL45C:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp674);{int
is_zero_terminated;{void*_tmp716=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp677))->v;int _tmp717;_LL4AB: if((int)_tmp716 != 0)
goto _LL4AD;_LL4AC:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp677,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL4AA;_LL4AD: if(_tmp716 <= (void*)1?1:*((int*)_tmp716)
!= 0)goto _LL4AF;_tmp717=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp716)->f1;
if(_tmp717 != 1)goto _LL4AF;_LL4AE: if(!Cyc_Tcutil_admits_zero(_tmp674))({struct Cyc_String_pa_struct
_tmp71A;_tmp71A.tag=0;_tmp71A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp674);{void*_tmp718[1]={& _tmp71A};Cyc_Tcutil_terr(loc,({const char*_tmp719="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp719,sizeof(char),_get_zero_arr_size(_tmp719,51));}),_tag_arr(
_tmp718,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4AA;_LL4AF:;_LL4B0:
is_zero_terminated=0;goto _LL4AA;_LL4AA:;}if(_tmp676 == 0)({void*_tmp71B[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp71C="an explicit array bound is required here";_tag_arr(
_tmp71C,sizeof(char),_get_zero_arr_size(_tmp71C,41));}),_tag_arr(_tmp71B,sizeof(
void*),0));});else{Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(
_tmp676));if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(
_tmp676)))({void*_tmp71D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp71E="array bounds expression is not constant";
_tag_arr(_tmp71E,sizeof(char),_get_zero_arr_size(_tmp71E,40));}),_tag_arr(
_tmp71D,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp676)))({void*_tmp71F[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp720="array bounds expression is not an unsigned int";_tag_arr(_tmp720,
sizeof(char),_get_zero_arr_size(_tmp720,47));}),_tag_arr(_tmp71F,sizeof(void*),0));});{
unsigned int _tmp722;int _tmp723;struct _tuple7 _tmp721=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp676));_tmp722=_tmp721.f1;_tmp723=_tmp721.f2;
if((is_zero_terminated?_tmp723: 0)?_tmp722 < 1: 0)({void*_tmp724[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp725="zero terminated array cannot have zero elements";
_tag_arr(_tmp725,sizeof(char),_get_zero_arr_size(_tmp725,48));}),_tag_arr(
_tmp724,sizeof(void*),0));});}}goto _LL43E;}_LL45D: if(_tmp656 <= (void*)3?1:*((int*)
_tmp656)!= 8)goto _LL45F;_tmp678=((struct Cyc_Absyn_FnType_struct*)_tmp656)->f1;
_tmp679=_tmp678.tvars;_tmp67A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp656)->f1).tvars;_tmp67B=_tmp678.effect;_tmp67C=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp656)->f1).effect;_tmp67D=(void*)_tmp678.ret_typ;
_tmp67E=_tmp678.args;_tmp67F=_tmp678.c_varargs;_tmp680=_tmp678.cyc_varargs;
_tmp681=_tmp678.rgn_po;_tmp682=_tmp678.attributes;_LL45E: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp682 != 0;_tmp682=_tmp682->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp682->hd))({struct Cyc_String_pa_struct _tmp728;
_tmp728.tag=0;_tmp728.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)
_tmp682->hd);{void*_tmp726[1]={& _tmp728};Cyc_Tcutil_terr(loc,({const char*_tmp727="bad function type attribute %s";
_tag_arr(_tmp727,sizeof(char),_get_zero_arr_size(_tmp727,31));}),_tag_arr(
_tmp726,sizeof(void*),1));}});{void*_tmp729=(void*)_tmp682->hd;void*_tmp72A;int
_tmp72B;int _tmp72C;_LL4B2: if((int)_tmp729 != 0)goto _LL4B4;_LL4B3: if(!seen_stdcall){
seen_stdcall=1;++ num_convs;}goto _LL4B1;_LL4B4: if((int)_tmp729 != 1)goto _LL4B6;
_LL4B5: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL4B1;_LL4B6: if((int)
_tmp729 != 2)goto _LL4B8;_LL4B7: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}
goto _LL4B1;_LL4B8: if(_tmp729 <= (void*)17?1:*((int*)_tmp729)!= 3)goto _LL4BA;
_tmp72A=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp729)->f1;_tmp72B=((
struct Cyc_Absyn_Format_att_struct*)_tmp729)->f2;_tmp72C=((struct Cyc_Absyn_Format_att_struct*)
_tmp729)->f3;_LL4B9: if(!seen_format){seen_format=1;ft=_tmp72A;fmt_desc_arg=
_tmp72B;fmt_arg_start=_tmp72C;}else{({void*_tmp72D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp72E="function can't have multiple format attributes";_tag_arr(
_tmp72E,sizeof(char),_get_zero_arr_size(_tmp72E,47));}),_tag_arr(_tmp72D,sizeof(
void*),0));});}goto _LL4B1;_LL4BA:;_LL4BB: goto _LL4B1;_LL4B1:;}}if(num_convs > 1)({
void*_tmp72F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp730="function can't have multiple calling conventions";
_tag_arr(_tmp730,sizeof(char),_get_zero_arr_size(_tmp730,49));}),_tag_arr(
_tmp72F,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*((struct Cyc_List_List**)
_tmp67A));{struct Cyc_List_List*b=*((struct Cyc_List_List**)_tmp67A);for(0;b != 0;b=
b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=
Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*
_tmp731=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*
_tmp732;_LL4BD: if(*((int*)_tmp731)!= 0)goto _LL4BF;_tmp732=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp731)->f1;if((int)_tmp732 != 1)goto _LL4BF;_LL4BE:({struct Cyc_String_pa_struct
_tmp735;_tmp735.tag=0;_tmp735.f1=(struct _tagged_arr)*((struct _tagged_arr*)((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp733[1]={& _tmp735};Cyc_Tcutil_terr(
loc,({const char*_tmp734="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp734,sizeof(char),_get_zero_arr_size(_tmp734,51));}),_tag_arr(
_tmp733,sizeof(void*),1));}});goto _LL4BC;_LL4BF:;_LL4C0: goto _LL4BC;_LL4BC:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp736=({struct Cyc_Tcutil_CVTEnv _tmp7AD;_tmp7AD.kind_env=
cvtenv.kind_env;_tmp7AD.free_vars=0;_tmp7AD.free_evars=0;_tmp7AD.generalize_evars=
cvtenv.generalize_evars;_tmp7AD.fn_result=1;_tmp7AD;});_tmp736=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp736,(void*)1,_tmp67D);_tmp736.fn_result=0;{struct Cyc_List_List*a=
_tmp67E;for(0;a != 0;a=a->tl){_tmp736=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp736,(void*)1,(*((struct _tuple2*)((struct _tuple2*)a->hd))).f3);}}if(_tmp680 != 
0){if(_tmp67F)({void*_tmp737[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp738="both c_vararg and cyc_vararg";
_tag_arr(_tmp738,sizeof(char),_get_zero_arr_size(_tmp738,29));}),_tag_arr(
_tmp737,sizeof(void*),0));});{void*_tmp73A;int _tmp73B;struct Cyc_Absyn_VarargInfo
_tmp739=*((struct Cyc_Absyn_VarargInfo*)_tmp680);_tmp73A=(void*)_tmp739.type;
_tmp73B=_tmp739.inject;_tmp736=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp736,(
void*)1,_tmp73A);if(_tmp73B){void*_tmp73C=Cyc_Tcutil_compress(_tmp73A);struct Cyc_Absyn_TunionInfo
_tmp73D;void*_tmp73E;_LL4C2: if(_tmp73C <= (void*)3?1:*((int*)_tmp73C)!= 2)goto
_LL4C4;_tmp73D=((struct Cyc_Absyn_TunionType_struct*)_tmp73C)->f1;_tmp73E=(void*)
_tmp73D.tunion_info;if(*((int*)_tmp73E)!= 1)goto _LL4C4;_LL4C3: goto _LL4C1;_LL4C4:;
_LL4C5:({void*_tmp73F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp740="can't inject a non-[x]tunion type";
_tag_arr(_tmp740,sizeof(char),_get_zero_arr_size(_tmp740,34));}),_tag_arr(
_tmp73F,sizeof(void*),0));});goto _LL4C1;_LL4C1:;}}}if(seen_format){int _tmp741=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67E);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp741)?1: fmt_arg_start < 0)?1:(_tmp680 == 0?fmt_arg_start != 0: 0))?
1:(_tmp680 != 0?fmt_arg_start != _tmp741 + 1: 0))({void*_tmp742[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp743="bad format descriptor";_tag_arr(_tmp743,sizeof(char),
_get_zero_arr_size(_tmp743,22));}),_tag_arr(_tmp742,sizeof(void*),0));});else{
void*_tmp745;struct _tuple2 _tmp744=*((struct _tuple2*)((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp67E,fmt_desc_arg - 1));_tmp745=_tmp744.f3;{
void*_tmp746=Cyc_Tcutil_compress(_tmp745);struct Cyc_Absyn_PtrInfo _tmp747;void*
_tmp748;struct Cyc_Absyn_Conref*_tmp749;struct Cyc_Absyn_Conref*_tmp74A;_LL4C7: if(
_tmp746 <= (void*)3?1:*((int*)_tmp746)!= 4)goto _LL4C9;_tmp747=((struct Cyc_Absyn_PointerType_struct*)
_tmp746)->f1;_tmp748=(void*)_tmp747.elt_typ;_tmp749=_tmp747.bounds;_tmp74A=
_tmp747.zero_term;_LL4C8:{struct _tuple6 _tmp74C=({struct _tuple6 _tmp74B;_tmp74B.f1=
Cyc_Tcutil_compress(_tmp748);_tmp74B.f2=Cyc_Absyn_conref_def((void*)0,_tmp749);
_tmp74B;});void*_tmp74D;void*_tmp74E;void*_tmp74F;void*_tmp750;_LL4CC: _tmp74D=
_tmp74C.f1;if(_tmp74D <= (void*)3?1:*((int*)_tmp74D)!= 5)goto _LL4CE;_tmp74E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp74D)->f1;if((int)_tmp74E != 2)goto _LL4CE;
_tmp74F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74D)->f2;if((int)_tmp74F != 
0)goto _LL4CE;_tmp750=_tmp74C.f2;if((int)_tmp750 != 0)goto _LL4CE;_LL4CD: goto _LL4CB;
_LL4CE:;_LL4CF:({void*_tmp751[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp752="format descriptor is not a char ? type";
_tag_arr(_tmp752,sizeof(char),_get_zero_arr_size(_tmp752,39));}),_tag_arr(
_tmp751,sizeof(void*),0));});goto _LL4CB;_LL4CB:;}goto _LL4C6;_LL4C9:;_LL4CA:({
void*_tmp753[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp754="format descriptor is not a char ? type";
_tag_arr(_tmp754,sizeof(char),_get_zero_arr_size(_tmp754,39));}),_tag_arr(
_tmp753,sizeof(void*),0));});goto _LL4C6;_LL4C6:;}if(fmt_arg_start != 0){int
problem;{struct _tuple6 _tmp756=({struct _tuple6 _tmp755;_tmp755.f1=ft;_tmp755.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp680))->type);
_tmp755;});void*_tmp757;void*_tmp758;struct Cyc_Absyn_TunionInfo _tmp759;void*
_tmp75A;struct Cyc_Absyn_Tuniondecl**_tmp75B;struct Cyc_Absyn_Tuniondecl*_tmp75C;
void*_tmp75D;void*_tmp75E;struct Cyc_Absyn_TunionInfo _tmp75F;void*_tmp760;struct
Cyc_Absyn_Tuniondecl**_tmp761;struct Cyc_Absyn_Tuniondecl*_tmp762;_LL4D1: _tmp757=
_tmp756.f1;if((int)_tmp757 != 0)goto _LL4D3;_tmp758=_tmp756.f2;if(_tmp758 <= (void*)
3?1:*((int*)_tmp758)!= 2)goto _LL4D3;_tmp759=((struct Cyc_Absyn_TunionType_struct*)
_tmp758)->f1;_tmp75A=(void*)_tmp759.tunion_info;if(*((int*)_tmp75A)!= 1)goto
_LL4D3;_tmp75B=((struct Cyc_Absyn_KnownTunion_struct*)_tmp75A)->f1;_tmp75C=*
_tmp75B;_LL4D2: problem=Cyc_Absyn_qvar_cmp(_tmp75C->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL4D0;_LL4D3: _tmp75D=_tmp756.f1;if((int)_tmp75D != 1)goto _LL4D5;_tmp75E=
_tmp756.f2;if(_tmp75E <= (void*)3?1:*((int*)_tmp75E)!= 2)goto _LL4D5;_tmp75F=((
struct Cyc_Absyn_TunionType_struct*)_tmp75E)->f1;_tmp760=(void*)_tmp75F.tunion_info;
if(*((int*)_tmp760)!= 1)goto _LL4D5;_tmp761=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp760)->f1;_tmp762=*_tmp761;_LL4D4: problem=Cyc_Absyn_qvar_cmp(_tmp762->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4D0;_LL4D5:;_LL4D6: problem=1;goto
_LL4D0;_LL4D0:;}if(problem)({void*_tmp763[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp764="format attribute and vararg types don't match";_tag_arr(_tmp764,sizeof(
char),_get_zero_arr_size(_tmp764,46));}),_tag_arr(_tmp763,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp681;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp766;
void*_tmp767;void*_tmp768;struct _tuple6*_tmp765=(struct _tuple6*)rpo->hd;_tmp766=*
_tmp765;_tmp767=_tmp766.f1;_tmp768=_tmp766.f2;_tmp736=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp736,(void*)4,_tmp767);_tmp736=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp736,(void*)3,_tmp768);}}if(*((struct Cyc_Core_Opt**)_tmp67C)!= 0)_tmp736=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp736,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*((struct Cyc_Core_Opt**)
_tmp67C)))->v);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=
_tmp736.free_vars;for(0;tvs != 0;tvs=tvs->tl){void*_tmp769=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp76A;struct
Cyc_Core_Opt**_tmp76B;void*_tmp76C;void*_tmp76D;struct Cyc_Core_Opt*_tmp76E;
struct Cyc_Core_Opt**_tmp76F;void*_tmp770;void*_tmp771;struct Cyc_Core_Opt*_tmp772;
struct Cyc_Core_Opt**_tmp773;_LL4D8: if(*((int*)_tmp769)!= 2)goto _LL4DA;_tmp76A=((
struct Cyc_Absyn_Less_kb_struct*)_tmp769)->f1;_tmp76B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp769)->f1;_tmp76C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp769)->f2;if((int)_tmp76C != 3)goto _LL4DA;_LL4D9:*((struct Cyc_Core_Opt**)
_tmp76B)=({struct Cyc_Core_Opt*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp775=_cycalloc(sizeof(*_tmp775));
_tmp775[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp776;_tmp776.tag=0;_tmp776.f1=(void*)((
void*)3);_tmp776;});_tmp775;}));_tmp774;});goto _LL4DB;_LL4DA: if(*((int*)_tmp769)
!= 0)goto _LL4DC;_tmp76D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp769)->f1;if((
int)_tmp76D != 3)goto _LL4DC;_LL4DB: effect=({struct Cyc_List_List*_tmp777=_cycalloc(
sizeof(*_tmp777));_tmp777->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp779;_tmp779.tag=19;_tmp779.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({struct Cyc_Absyn_VarType_struct
_tmp77B;_tmp77B.tag=1;_tmp77B.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp77B;});
_tmp77A;}));_tmp779;});_tmp778;}));_tmp777->tl=effect;_tmp777;});goto _LL4D7;
_LL4DC: if(*((int*)_tmp769)!= 2)goto _LL4DE;_tmp76E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp769)->f1;_tmp76F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp769)->f1;_tmp770=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp769)->f2;if((
int)_tmp770 != 4)goto _LL4DE;_LL4DD:*((struct Cyc_Core_Opt**)_tmp76F)=({struct Cyc_Core_Opt*
_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp77E;_tmp77E.tag=0;_tmp77E.f1=(void*)((void*)4);_tmp77E;});_tmp77D;}));
_tmp77C;});goto _LL4DF;_LL4DE: if(*((int*)_tmp769)!= 0)goto _LL4E0;_tmp771=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp769)->f1;if((int)_tmp771 != 4)goto _LL4E0;_LL4DF:
effect=({struct Cyc_List_List*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp780=_cycalloc(sizeof(*_tmp780));
_tmp780[0]=({struct Cyc_Absyn_VarType_struct _tmp781;_tmp781.tag=1;_tmp781.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp781;});_tmp780;}));_tmp77F->tl=effect;_tmp77F;});
goto _LL4D7;_LL4E0: if(*((int*)_tmp769)!= 1)goto _LL4E2;_tmp772=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp769)->f1;_tmp773=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp769)->f1;_LL4E1:*((struct Cyc_Core_Opt**)_tmp773)=({struct Cyc_Core_Opt*
_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp784;_tmp784.tag=2;_tmp784.f1=0;_tmp784.f2=(void*)((void*)0);_tmp784;});
_tmp783;}));_tmp782;});goto _LL4E3;_LL4E2:;_LL4E3: effect=({struct Cyc_List_List*
_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp787;_tmp787.tag=21;_tmp787.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp788=_cycalloc(sizeof(*_tmp788));_tmp788[0]=({struct Cyc_Absyn_VarType_struct
_tmp789;_tmp789.tag=1;_tmp789.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp789;});
_tmp788;}));_tmp787;});_tmp786;}));_tmp785->tl=effect;_tmp785;});goto _LL4D7;
_LL4D7:;}}{struct Cyc_List_List*ts=_tmp736.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp78A=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL4E5: if((int)_tmp78A != 3)goto
_LL4E7;_LL4E6: effect=({struct Cyc_List_List*_tmp78B=_cycalloc(sizeof(*_tmp78B));
_tmp78B->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp78C=_cycalloc(
sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp78D;_tmp78D.tag=
19;_tmp78D.f1=(void*)((void*)ts->hd);_tmp78D;});_tmp78C;}));_tmp78B->tl=effect;
_tmp78B;});goto _LL4E4;_LL4E7: if((int)_tmp78A != 4)goto _LL4E9;_LL4E8: effect=({
struct Cyc_List_List*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E->hd=(void*)((
void*)ts->hd);_tmp78E->tl=effect;_tmp78E;});goto _LL4E4;_LL4E9:;_LL4EA: effect=({
struct Cyc_List_List*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F->hd=(void*)((
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp790=_cycalloc(sizeof(*_tmp790));
_tmp790[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp791;_tmp791.tag=21;_tmp791.f1=(
void*)((void*)ts->hd);_tmp791;});_tmp790;}));_tmp78F->tl=effect;_tmp78F;});goto
_LL4E4;_LL4E4:;}}*((struct Cyc_Core_Opt**)_tmp67C)=({struct Cyc_Core_Opt*_tmp792=
_cycalloc(sizeof(*_tmp792));_tmp792->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp794;_tmp794.tag=20;_tmp794.f1=effect;_tmp794;});_tmp793;}));_tmp792;});}if(*((
struct Cyc_List_List**)_tmp67A)!= 0){struct Cyc_List_List*bs=*((struct Cyc_List_List**)
_tmp67A);for(0;bs != 0;bs=bs->tl){void*_tmp795=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp796;struct Cyc_Core_Opt**
_tmp797;struct Cyc_Core_Opt*_tmp798;struct Cyc_Core_Opt**_tmp799;void*_tmp79A;
struct Cyc_Core_Opt*_tmp79B;struct Cyc_Core_Opt**_tmp79C;void*_tmp79D;struct Cyc_Core_Opt*
_tmp79E;struct Cyc_Core_Opt**_tmp79F;void*_tmp7A0;void*_tmp7A1;_LL4EC: if(*((int*)
_tmp795)!= 1)goto _LL4EE;_tmp796=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp795)->f1;
_tmp797=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp795)->f1;
_LL4ED:({struct Cyc_String_pa_struct _tmp7A4;_tmp7A4.tag=0;_tmp7A4.f1=(struct
_tagged_arr)*((struct _tagged_arr*)((struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*
_tmp7A2[1]={& _tmp7A4};Cyc_Tcutil_warn(loc,({const char*_tmp7A3="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp7A3,sizeof(char),_get_zero_arr_size(_tmp7A3,47));}),_tag_arr(
_tmp7A2,sizeof(void*),1));}});_tmp799=_tmp797;goto _LL4EF;_LL4EE: if(*((int*)
_tmp795)!= 2)goto _LL4F0;_tmp798=((struct Cyc_Absyn_Less_kb_struct*)_tmp795)->f1;
_tmp799=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp795)->f1;
_tmp79A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp795)->f2;if((int)_tmp79A != 
1)goto _LL4F0;_LL4EF: _tmp79C=_tmp799;goto _LL4F1;_LL4F0: if(*((int*)_tmp795)!= 2)
goto _LL4F2;_tmp79B=((struct Cyc_Absyn_Less_kb_struct*)_tmp795)->f1;_tmp79C=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp795)->f1;_tmp79D=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp795)->f2;if((int)_tmp79D != 0)goto
_LL4F2;_LL4F1:*((struct Cyc_Core_Opt**)_tmp79C)=({struct Cyc_Core_Opt*_tmp7A5=
_cycalloc(sizeof(*_tmp7A5));_tmp7A5->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7A7;_tmp7A7.tag=0;_tmp7A7.f1=(void*)((void*)2);_tmp7A7;});_tmp7A6;}));
_tmp7A5;});goto _LL4EB;_LL4F2: if(*((int*)_tmp795)!= 2)goto _LL4F4;_tmp79E=((struct
Cyc_Absyn_Less_kb_struct*)_tmp795)->f1;_tmp79F=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp795)->f1;_tmp7A0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp795)->f2;_LL4F3:*((struct Cyc_Core_Opt**)_tmp79F)=({struct Cyc_Core_Opt*
_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7AA;_tmp7AA.tag=0;_tmp7AA.f1=(void*)_tmp7A0;_tmp7AA;});_tmp7A9;}));_tmp7A8;});
goto _LL4EB;_LL4F4: if(*((int*)_tmp795)!= 0)goto _LL4F6;_tmp7A1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp795)->f1;if((int)_tmp7A1 != 1)goto _LL4F6;_LL4F5:({void*_tmp7AB[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7AC="functions can't abstract M types";_tag_arr(_tmp7AC,
sizeof(char),_get_zero_arr_size(_tmp7AC,33));}),_tag_arr(_tmp7AB,sizeof(void*),0));});
goto _LL4EB;_LL4F6:;_LL4F7: goto _LL4EB;_LL4EB:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp736.kind_env,*((struct Cyc_List_List**)_tmp67A));_tmp736.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp736.free_vars,*((struct Cyc_List_List**)_tmp67A));{struct Cyc_List_List*tvs=
_tmp736.free_vars;for(0;tvs != 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,(struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=
_tmp736.free_evars;for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,(void*)evs->hd);}}goto _LL43E;}}_LL45F: if(_tmp656 <= (void*)3?1:*((
int*)_tmp656)!= 9)goto _LL461;_tmp683=((struct Cyc_Absyn_TupleType_struct*)_tmp656)->f1;
_LL460: for(0;_tmp683 != 0;_tmp683=_tmp683->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,(*((struct _tuple4*)((struct _tuple4*)_tmp683->hd))).f2);}
goto _LL43E;_LL461: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 11)goto _LL463;
_tmp684=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp656)->f1;_tmp685=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp656)->f2;_LL462:{struct _RegionHandle
_tmp7AE=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp7AE;
_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;for(0;_tmp685 != 0;
_tmp685=_tmp685->tl){struct Cyc_Absyn_Aggrfield _tmp7B0;struct _tagged_arr*_tmp7B1;
struct Cyc_Absyn_Tqual _tmp7B2;void*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_List_List*
_tmp7B5;struct Cyc_Absyn_Aggrfield*_tmp7AF=(struct Cyc_Absyn_Aggrfield*)_tmp685->hd;
_tmp7B0=*_tmp7AF;_tmp7B1=_tmp7B0.name;_tmp7B2=_tmp7B0.tq;_tmp7B3=(void*)_tmp7B0.type;
_tmp7B4=_tmp7B0.width;_tmp7B5=_tmp7B0.attributes;if(((int(*)(int(*compare)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))
Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7B1))({struct Cyc_String_pa_struct
_tmp7B8;_tmp7B8.tag=0;_tmp7B8.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp7B1);{void*_tmp7B6[1]={& _tmp7B8};Cyc_Tcutil_terr(loc,({const char*_tmp7B7="duplicate field %s";
_tag_arr(_tmp7B7,sizeof(char),_get_zero_arr_size(_tmp7B7,19));}),_tag_arr(
_tmp7B6,sizeof(void*),1));}});if(Cyc_strcmp((struct _tagged_arr)*((struct
_tagged_arr*)_tmp7B1),({const char*_tmp7B9="";_tag_arr(_tmp7B9,sizeof(char),
_get_zero_arr_size(_tmp7B9,1));}))!= 0)prev_fields=({struct Cyc_List_List*_tmp7BA=
_region_malloc(aprev_rgn,sizeof(*_tmp7BA));_tmp7BA->hd=_tmp7B1;_tmp7BA->tl=
prev_fields;_tmp7BA;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp7B3);if(_tmp684 == (void*)1?!Cyc_Tcutil_bits_only(_tmp7B3): 0)({struct Cyc_String_pa_struct
_tmp7BD;_tmp7BD.tag=0;_tmp7BD.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp7B1);{void*_tmp7BB[1]={& _tmp7BD};Cyc_Tcutil_terr(loc,({const char*_tmp7BC="union member %s has a non-integral type";
_tag_arr(_tmp7BC,sizeof(char),_get_zero_arr_size(_tmp7BC,40));}),_tag_arr(
_tmp7BB,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp7B3,_tmp7B4,
_tmp7B1);Cyc_Tcutil_check_field_atts(loc,_tmp7B1,_tmp7B5);}};_pop_region(
aprev_rgn);}goto _LL43E;_LL463: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 10)goto
_LL465;_tmp686=((struct Cyc_Absyn_AggrType_struct*)_tmp656)->f1;_tmp687=(void*)
_tmp686.aggr_info;_tmp688=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp656)->f1).aggr_info;
_tmp689=_tmp686.targs;_tmp68A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp656)->f1).targs;_LL464:{void*_tmp7BE=*((void**)_tmp688);void*_tmp7BF;struct
_tuple1*_tmp7C0;struct Cyc_Absyn_Aggrdecl**_tmp7C1;struct Cyc_Absyn_Aggrdecl*
_tmp7C2;_LL4F9: if(*((int*)_tmp7BE)!= 0)goto _LL4FB;_tmp7BF=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7BE)->f1;_tmp7C0=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7BE)->f2;_LL4FA: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp7C3;_push_handler(& _tmp7C3);{
int _tmp7C5=0;if(setjmp(_tmp7C3.handler))_tmp7C5=1;if(!_tmp7C5){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp7C0);*((void**)_tmp688)=(void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp7C7;_tmp7C7.tag=1;_tmp7C7.f1=adp;_tmp7C7;});_tmp7C6;});;_pop_handler();}
else{void*_tmp7C4=(void*)_exn_thrown;void*_tmp7C9=_tmp7C4;_LL4FE: if(_tmp7C9 != 
Cyc_Dict_Absent)goto _LL500;_LL4FF: {struct Cyc_Tcenv_Genv*_tmp7CA=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp7CB=({struct Cyc_Absyn_Aggrdecl*_tmp7D1=_cycalloc(
sizeof(*_tmp7D1));_tmp7D1->kind=(void*)_tmp7BF;_tmp7D1->sc=(void*)((void*)3);
_tmp7D1->name=_tmp7C0;_tmp7D1->tvs=0;_tmp7D1->impl=0;_tmp7D1->attributes=0;
_tmp7D1;});Cyc_Tc_tcAggrdecl(te,_tmp7CA,loc,_tmp7CB);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp7C0);*((void**)_tmp688)=(void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp7CC=_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp7CD;_tmp7CD.tag=1;_tmp7CD.f1=adp;_tmp7CD;});_tmp7CC;});if(*((struct Cyc_List_List**)
_tmp68A)!= 0){({struct Cyc_String_pa_struct _tmp7D0;_tmp7D0.tag=0;_tmp7D0.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp7C0);{void*_tmp7CE[1]={& _tmp7D0};
Cyc_Tcutil_terr(loc,({const char*_tmp7CF="declare parameterized type %s before using";
_tag_arr(_tmp7CF,sizeof(char),_get_zero_arr_size(_tmp7CF,43));}),_tag_arr(
_tmp7CE,sizeof(void*),1));}});return cvtenv;}goto _LL4FD;}_LL500:;_LL501:(void)
_throw(_tmp7C9);_LL4FD:;}}}_tmp7C2=*((struct Cyc_Absyn_Aggrdecl**)adp);goto _LL4FC;}
_LL4FB: if(*((int*)_tmp7BE)!= 1)goto _LL4F8;_tmp7C1=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7BE)->f1;_tmp7C2=*_tmp7C1;_LL4FC: {struct Cyc_List_List*tvs=_tmp7C2->tvs;
struct Cyc_List_List*ts=*((struct Cyc_List_List**)_tmp68A);for(0;ts != 0?tvs != 0: 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({
struct Cyc_String_pa_struct _tmp7D4;_tmp7D4.tag=0;_tmp7D4.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp7C2->name);{void*_tmp7D2[1]={& _tmp7D4};Cyc_Tcutil_terr(
loc,({const char*_tmp7D3="too many parameters for type %s";_tag_arr(_tmp7D3,
sizeof(char),_get_zero_arr_size(_tmp7D3,32));}),_tag_arr(_tmp7D2,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5->hd=(
void*)e;_tmp7D5->tl=hidden_ts;_tmp7D5;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e);}*((struct Cyc_List_List**)_tmp68A)=Cyc_List_imp_append(*((
struct Cyc_List_List**)_tmp68A),Cyc_List_imp_rev(hidden_ts));}}_LL4F8:;}goto
_LL43E;_LL465: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 16)goto _LL467;_tmp68B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp656)->f1;_tmp68C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp656)->f2;_tmp68D=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp656)->f2;_tmp68E=((struct Cyc_Absyn_TypedefType_struct*)_tmp656)->f3;_tmp68F=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp656)->f3;
_tmp690=((struct Cyc_Absyn_TypedefType_struct*)_tmp656)->f4;_tmp691=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp656)->f4;_LL466: {struct Cyc_List_List*
targs=*((struct Cyc_List_List**)_tmp68D);struct Cyc_Absyn_Typedefdecl*td;{struct
_handler_cons _tmp7D6;_push_handler(& _tmp7D6);{int _tmp7D8=0;if(setjmp(_tmp7D6.handler))
_tmp7D8=1;if(!_tmp7D8){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp68B);;
_pop_handler();}else{void*_tmp7D7=(void*)_exn_thrown;void*_tmp7DA=_tmp7D7;_LL503:
if(_tmp7DA != Cyc_Dict_Absent)goto _LL505;_LL504:({struct Cyc_String_pa_struct
_tmp7DD;_tmp7DD.tag=0;_tmp7DD.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp68B);{void*_tmp7DB[1]={& _tmp7DD};Cyc_Tcutil_terr(loc,({const char*_tmp7DC="unbound typedef name %s";
_tag_arr(_tmp7DC,sizeof(char),_get_zero_arr_size(_tmp7DC,24));}),_tag_arr(
_tmp7DB,sizeof(void*),1));}});return cvtenv;_LL505:;_LL506:(void)_throw(_tmp7DA);
_LL502:;}}}*((struct Cyc_Absyn_Typedefdecl**)_tmp68F)=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp68B[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp7DE=_cycalloc(sizeof(*_tmp7DE));_tmp7DE->hd=({
struct _tuple8*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp7DF->f2=(void*)ts->hd;_tmp7DF;});_tmp7DE->tl=inst;_tmp7DE;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp7E2;_tmp7E2.tag=0;_tmp7E2.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp68B);{void*_tmp7E0[1]={& _tmp7E2};Cyc_Tcutil_terr(
loc,({const char*_tmp7E1="too many parameters for typedef %s";_tag_arr(_tmp7E1,
sizeof(char),_get_zero_arr_size(_tmp7E1,35));}),_tag_arr(_tmp7E0,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3->hd=(
void*)e;_tmp7E3->tl=hidden_ts;_tmp7E3;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));
_tmp7E4->hd=({struct _tuple8*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5->f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp7E5->f2=e;_tmp7E5;});_tmp7E4->tl=inst;_tmp7E4;});}*((
struct Cyc_List_List**)_tmp68D)=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*((struct Cyc_Core_Opt**)_tmp691)=({
struct Cyc_Core_Opt*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6->v=(void*)new_typ;
_tmp7E6;});}goto _LL43E;}}_LL467: if((int)_tmp656 != 2)goto _LL469;_LL468: goto _LL43E;
_LL469: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 15)goto _LL46B;_tmp692=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp656)->f1;_LL46A: _tmp693=_tmp692;goto
_LL46C;_LL46B: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 19)goto _LL46D;_tmp693=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp656)->f1;_LL46C: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp693);goto _LL43E;_LL46D: if(_tmp656 <= (void*)3?1:*((int*)
_tmp656)!= 21)goto _LL46F;_tmp694=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp656)->f1;_LL46E: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp694);goto _LL43E;_LL46F: if(_tmp656 <= (void*)3?1:*((int*)_tmp656)!= 20)goto
_LL43E;_tmp695=((struct Cyc_Absyn_JoinEff_struct*)_tmp656)->f1;_LL470: for(0;
_tmp695 != 0;_tmp695=_tmp695->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)4,(void*)_tmp695->hd);}goto _LL43E;_LL43E:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_String_pa_struct _tmp7EB;
_tmp7EB.tag=0;_tmp7EB.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
expected_kind);{struct Cyc_String_pa_struct _tmp7EA;_tmp7EA.tag=0;_tmp7EA.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t));{struct Cyc_String_pa_struct
_tmp7E9;_tmp7E9.tag=0;_tmp7E9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp7E7[3]={& _tmp7E9,& _tmp7EA,& _tmp7EB};Cyc_Tcutil_terr(loc,({const char*
_tmp7E8="type %s has kind %s but as used here needs kind %s";_tag_arr(_tmp7E8,
sizeof(char),_get_zero_arr_size(_tmp7E8,51));}),_tag_arr(_tmp7E7,sizeof(void*),3));}}}});
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp7EC=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp7F5;_tmp7F5.kind_env=kind_env;_tmp7F5.free_vars=
0;_tmp7F5.free_evars=0;_tmp7F5.generalize_evars=generalize_evars;_tmp7F5.fn_result=
0;_tmp7F5;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp7EC.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp7EC.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp7EC.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp7ED=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp7EE;struct Cyc_Core_Opt**_tmp7EF;_LL508: if(_tmp7ED <= (void*)3?1:*((int*)
_tmp7ED)!= 0)goto _LL50A;_tmp7EE=((struct Cyc_Absyn_Evar_struct*)_tmp7ED)->f4;
_tmp7EF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp7ED)->f4;
_LL509: if(*((struct Cyc_Core_Opt**)_tmp7EF)== 0)*((struct Cyc_Core_Opt**)_tmp7EF)=({
struct Cyc_Core_Opt*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0->v=kind_env;
_tmp7F0;});else{struct Cyc_List_List*_tmp7F1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*((struct Cyc_Core_Opt**)_tmp7EF)))->v;struct Cyc_List_List*_tmp7F2=0;
for(0;_tmp7F1 != 0;_tmp7F1=_tmp7F1->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)_tmp7F1->hd))_tmp7F2=({
struct Cyc_List_List*_tmp7F3=_cycalloc(sizeof(*_tmp7F3));_tmp7F3->hd=(struct Cyc_Absyn_Tvar*)
_tmp7F1->hd;_tmp7F3->tl=_tmp7F2;_tmp7F3;});}*((struct Cyc_Core_Opt**)_tmp7EF)=({
struct Cyc_Core_Opt*_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4->v=_tmp7F2;
_tmp7F4;});}goto _LL507;_LL50A:;_LL50B: goto _LL507;_LL507:;}}return _tmp7EC;}void
Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_Tcutil_CVTEnv
_tmp7F6=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,generalize_evars,t);struct
Cyc_List_List*_tmp7F7=_tmp7F6.free_vars;struct Cyc_List_List*_tmp7F8=_tmp7F6.free_evars;{
struct Cyc_List_List*x=_tmp7F7;for(0;x != 0;x=x->tl){void*_tmp7F9=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp7FA;struct Cyc_Core_Opt**
_tmp7FB;struct Cyc_Core_Opt*_tmp7FC;struct Cyc_Core_Opt**_tmp7FD;void*_tmp7FE;
struct Cyc_Core_Opt*_tmp7FF;struct Cyc_Core_Opt**_tmp800;void*_tmp801;struct Cyc_Core_Opt*
_tmp802;struct Cyc_Core_Opt**_tmp803;void*_tmp804;void*_tmp805;_LL50D: if(*((int*)
_tmp7F9)!= 1)goto _LL50F;_tmp7FA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F9)->f1;
_tmp7FB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F9)->f1;
_LL50E: _tmp7FD=_tmp7FB;goto _LL510;_LL50F: if(*((int*)_tmp7F9)!= 2)goto _LL511;
_tmp7FC=((struct Cyc_Absyn_Less_kb_struct*)_tmp7F9)->f1;_tmp7FD=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp7F9)->f1;_tmp7FE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7F9)->f2;if((int)_tmp7FE != 1)goto _LL511;_LL510: _tmp800=_tmp7FD;goto _LL512;
_LL511: if(*((int*)_tmp7F9)!= 2)goto _LL513;_tmp7FF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7F9)->f1;_tmp800=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7F9)->f1;_tmp801=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7F9)->f2;if((
int)_tmp801 != 0)goto _LL513;_LL512:*((struct Cyc_Core_Opt**)_tmp800)=({struct Cyc_Core_Opt*
_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp808;_tmp808.tag=0;_tmp808.f1=(void*)((void*)2);_tmp808;});_tmp807;}));
_tmp806;});goto _LL50C;_LL513: if(*((int*)_tmp7F9)!= 2)goto _LL515;_tmp802=((struct
Cyc_Absyn_Less_kb_struct*)_tmp7F9)->f1;_tmp803=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp7F9)->f1;_tmp804=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7F9)->f2;_LL514:*((struct Cyc_Core_Opt**)_tmp803)=({struct Cyc_Core_Opt*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp80B;_tmp80B.tag=0;_tmp80B.f1=(void*)_tmp804;_tmp80B;});_tmp80A;}));_tmp809;});
goto _LL50C;_LL515: if(*((int*)_tmp7F9)!= 0)goto _LL517;_tmp805=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp7F9)->f1;if((int)_tmp805 != 1)goto _LL517;_LL516:({struct Cyc_String_pa_struct
_tmp80E;_tmp80E.tag=0;_tmp80E.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd);{void*_tmp80C[1]={& _tmp80E};Cyc_Tcutil_terr(loc,({
const char*_tmp80D="type variable %s cannot have kind M";_tag_arr(_tmp80D,sizeof(
char),_get_zero_arr_size(_tmp80D,36));}),_tag_arr(_tmp80C,sizeof(void*),1));}});
goto _LL50C;_LL517:;_LL518: goto _LL50C;_LL50C:;}}if(_tmp7F7 != 0?1: _tmp7F8 != 0){{
void*_tmp80F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp810;struct Cyc_List_List*
_tmp811;struct Cyc_List_List**_tmp812;struct Cyc_Core_Opt*_tmp813;void*_tmp814;
struct Cyc_List_List*_tmp815;int _tmp816;struct Cyc_Absyn_VarargInfo*_tmp817;struct
Cyc_List_List*_tmp818;struct Cyc_List_List*_tmp819;_LL51A: if(_tmp80F <= (void*)3?1:*((
int*)_tmp80F)!= 8)goto _LL51C;_tmp810=((struct Cyc_Absyn_FnType_struct*)_tmp80F)->f1;
_tmp811=_tmp810.tvars;_tmp812=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp80F)->f1).tvars;_tmp813=_tmp810.effect;_tmp814=(void*)_tmp810.ret_typ;
_tmp815=_tmp810.args;_tmp816=_tmp810.c_varargs;_tmp817=_tmp810.cyc_varargs;
_tmp818=_tmp810.rgn_po;_tmp819=_tmp810.attributes;_LL51B: if(*((struct Cyc_List_List**)
_tmp812)== 0){*((struct Cyc_List_List**)_tmp812)=_tmp7F7;_tmp7F7=0;}goto _LL519;
_LL51C:;_LL51D: goto _LL519;_LL519:;}if(_tmp7F7 != 0)({struct Cyc_String_pa_struct
_tmp81C;_tmp81C.tag=0;_tmp81C.f1=(struct _tagged_arr)*((struct _tagged_arr*)((
struct Cyc_Absyn_Tvar*)_tmp7F7->hd)->name);{void*_tmp81A[1]={& _tmp81C};Cyc_Tcutil_terr(
loc,({const char*_tmp81B="unbound type variable %s";_tag_arr(_tmp81B,sizeof(char),
_get_zero_arr_size(_tmp81B,25));}),_tag_arr(_tmp81A,sizeof(void*),1));}});if(
_tmp7F8 != 0)for(0;_tmp7F8 != 0;_tmp7F8=_tmp7F8->tl){void*e=(void*)_tmp7F8->hd;
void*_tmp81D=Cyc_Tcutil_typ_kind(e);_LL51F: if((int)_tmp81D != 3)goto _LL521;_LL520:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp81E[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp81F="can't unify evar with heap!";
_tag_arr(_tmp81F,sizeof(char),_get_zero_arr_size(_tmp81F,28));}),_tag_arr(
_tmp81E,sizeof(void*),0));});goto _LL51E;_LL521: if((int)_tmp81D != 4)goto _LL523;
_LL522: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp820[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp821="can't unify evar with {}!";
_tag_arr(_tmp821,sizeof(char),_get_zero_arr_size(_tmp821,26));}),_tag_arr(
_tmp820,sizeof(void*),0));});goto _LL51E;_LL523:;_LL524:({struct Cyc_String_pa_struct
_tmp825;_tmp825.tag=0;_tmp825.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_String_pa_struct _tmp824;_tmp824.tag=0;_tmp824.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(e);{void*_tmp822[2]={& _tmp824,& _tmp825};Cyc_Tcutil_terr(
loc,({const char*_tmp823="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp823,sizeof(char),_get_zero_arr_size(_tmp823,52));}),_tag_arr(
_tmp822,sizeof(void*),2));}}});goto _LL51E;_LL51E:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp826=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp827;struct Cyc_List_List*
_tmp828;struct Cyc_Core_Opt*_tmp829;_LL526: if(_tmp826 <= (void*)3?1:*((int*)
_tmp826)!= 8)goto _LL528;_tmp827=((struct Cyc_Absyn_FnType_struct*)_tmp826)->f1;
_tmp828=_tmp827.tvars;_tmp829=_tmp827.effect;_LL527: fd->tvs=_tmp828;fd->effect=
_tmp829;goto _LL525;_LL528:;_LL529:({void*_tmp82A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp82B="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp82B,sizeof(char),_get_zero_arr_size(_tmp82B,38));}),_tag_arr(
_tmp82A,sizeof(void*),0));});_LL525:;}{struct _RegionHandle _tmp82C=_new_region("r");
struct _RegionHandle*r=& _tmp82C;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp82D="function declaration has repeated parameter";
_tag_arr(_tmp82D,sizeof(char),_get_zero_arr_size(_tmp82D,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->v=(
void*)t;_tmp82E;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp82F=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp830=Cyc_Tcutil_remove_bound_tvars(
_tmp82F.free_vars,bound_tvars);struct Cyc_List_List*_tmp831=_tmp82F.free_evars;{
struct Cyc_List_List*fs=_tmp830;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp832=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp836;_tmp836.tag=
0;_tmp836.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_String_pa_struct
_tmp835;_tmp835.tag=0;_tmp835.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp832);{void*_tmp833[2]={& _tmp835,& _tmp836};Cyc_Tcutil_terr(loc,({const char*
_tmp834="unbound type variable %s in type %s";_tag_arr(_tmp834,sizeof(char),
_get_zero_arr_size(_tmp834,36));}),_tag_arr(_tmp833,sizeof(void*),2));}}});}}if(
!allow_evars?_tmp831 != 0: 0)for(0;_tmp831 != 0;_tmp831=_tmp831->tl){void*e=(void*)
_tmp831->hd;void*_tmp837=Cyc_Tcutil_typ_kind(e);_LL52B: if((int)_tmp837 != 3)goto
_LL52D;_LL52C: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp838[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp839="can't unify evar with heap!";
_tag_arr(_tmp839,sizeof(char),_get_zero_arr_size(_tmp839,28));}),_tag_arr(
_tmp838,sizeof(void*),0));});goto _LL52A;_LL52D: if((int)_tmp837 != 4)goto _LL52F;
_LL52E: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp83A[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp83B="can't unify evar with {}!";
_tag_arr(_tmp83B,sizeof(char),_get_zero_arr_size(_tmp83B,26));}),_tag_arr(
_tmp83A,sizeof(void*),0));});goto _LL52A;_LL52F:;_LL530:({struct Cyc_String_pa_struct
_tmp83F;_tmp83F.tag=0;_tmp83F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_String_pa_struct _tmp83E;_tmp83E.tag=0;_tmp83E.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(e);{void*_tmp83C[2]={& _tmp83E,& _tmp83F};Cyc_Tcutil_terr(
loc,({const char*_tmp83D="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp83D,sizeof(char),_get_zero_arr_size(_tmp83D,52));}),_tag_arr(
_tmp83C,sizeof(void*),2));}}});goto _LL52A;_LL52A:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp843;_tmp843.tag=0;_tmp843.f1=(struct _tagged_arr)a2string((void*)vs->hd);{
struct Cyc_String_pa_struct _tmp842;_tmp842.tag=0;_tmp842.f1=(struct _tagged_arr)
msg;{void*_tmp840[2]={& _tmp842,& _tmp843};Cyc_Tcutil_terr(loc,({const char*_tmp841="%s: %s";
_tag_arr(_tmp841,sizeof(char),_get_zero_arr_size(_tmp841,7));}),_tag_arr(_tmp840,
sizeof(void*),2));}}});}}}static struct _tagged_arr Cyc_Tcutil_strptr2string(struct
_tagged_arr*s){return*((struct _tagged_arr*)s);}void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr msg){((
void(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(struct _tagged_arr*),
struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,
loc,Cyc_Tcutil_tvar2string,({const char*_tmp844="duplicate type variable";
_tag_arr(_tmp844,sizeof(char),_get_zero_arr_size(_tmp844,24));}));}struct
_tuple18{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple19{struct Cyc_List_List*
f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name),({const char*_tmp845="";_tag_arr(
_tmp845,sizeof(char),_get_zero_arr_size(_tmp845,1));}))!= 0)fields=({struct Cyc_List_List*
_tmp846=_cycalloc(sizeof(*_tmp846));_tmp846->hd=({struct _tuple18*_tmp847=
_cycalloc(sizeof(*_tmp847));_tmp847->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;
_tmp847->f2=0;_tmp847;});_tmp846->tl=fields;_tmp846;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple19 _tmp849;struct Cyc_List_List*_tmp84A;void*
_tmp84B;struct _tuple19*_tmp848=(struct _tuple19*)des->hd;_tmp849=*_tmp848;_tmp84A=
_tmp849.f1;_tmp84B=_tmp849.f2;if(_tmp84A == 0){struct Cyc_List_List*_tmp84C=fields;
for(0;_tmp84C != 0;_tmp84C=_tmp84C->tl){if(!(*((struct _tuple18*)((struct _tuple18*)
_tmp84C->hd))).f2){(*((struct _tuple18*)((struct _tuple18*)_tmp84C->hd))).f2=1;(*((
struct _tuple19*)((struct _tuple19*)des->hd))).f1=(struct Cyc_List_List*)({struct
Cyc_List_List*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D->hd=(void*)((void*)({
struct Cyc_Absyn_FieldName_struct*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=({
struct Cyc_Absyn_FieldName_struct _tmp84F;_tmp84F.tag=1;_tmp84F.f1=((*((struct
_tuple18*)((struct _tuple18*)_tmp84C->hd))).f1)->name;_tmp84F;});_tmp84E;}));
_tmp84D->tl=0;_tmp84D;});ans=({struct Cyc_List_List*_tmp850=_region_malloc(rgn,
sizeof(*_tmp850));_tmp850->hd=({struct _tuple20*_tmp851=_region_malloc(rgn,
sizeof(*_tmp851));_tmp851->f1=(*((struct _tuple18*)((struct _tuple18*)_tmp84C->hd))).f1;
_tmp851->f2=_tmp84B;_tmp851;});_tmp850->tl=ans;_tmp850;});break;}}if(_tmp84C == 0)({
void*_tmp852[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp853="too many arguments to struct";
_tag_arr(_tmp853,sizeof(char),_get_zero_arr_size(_tmp853,29));}),_tag_arr(
_tmp852,sizeof(void*),0));});}else{if(_tmp84A->tl != 0)({void*_tmp854[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp855="multiple designators are not supported";_tag_arr(
_tmp855,sizeof(char),_get_zero_arr_size(_tmp855,39));}),_tag_arr(_tmp854,sizeof(
void*),0));});else{void*_tmp856=(void*)_tmp84A->hd;struct _tagged_arr*_tmp857;
_LL532: if(*((int*)_tmp856)!= 0)goto _LL534;_LL533:({void*_tmp858[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp859="array designator used in argument to struct";_tag_arr(
_tmp859,sizeof(char),_get_zero_arr_size(_tmp859,44));}),_tag_arr(_tmp858,sizeof(
void*),0));});goto _LL531;_LL534: if(*((int*)_tmp856)!= 1)goto _LL531;_tmp857=((
struct Cyc_Absyn_FieldName_struct*)_tmp856)->f1;_LL535: {struct Cyc_List_List*
_tmp85A=fields;for(0;_tmp85A != 0;_tmp85A=_tmp85A->tl){if(Cyc_strptrcmp(_tmp857,((*((
struct _tuple18*)((struct _tuple18*)_tmp85A->hd))).f1)->name)== 0){if((*((struct
_tuple18*)((struct _tuple18*)_tmp85A->hd))).f2)({struct Cyc_String_pa_struct
_tmp85D;_tmp85D.tag=0;_tmp85D.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp857);{void*_tmp85B[1]={& _tmp85D};Cyc_Tcutil_terr(loc,({const char*_tmp85C="field %s has already been used as an argument";
_tag_arr(_tmp85C,sizeof(char),_get_zero_arr_size(_tmp85C,46));}),_tag_arr(
_tmp85B,sizeof(void*),1));}});(*((struct _tuple18*)((struct _tuple18*)_tmp85A->hd))).f2=
1;ans=({struct Cyc_List_List*_tmp85E=_region_malloc(rgn,sizeof(*_tmp85E));_tmp85E->hd=({
struct _tuple20*_tmp85F=_region_malloc(rgn,sizeof(*_tmp85F));_tmp85F->f1=(*((
struct _tuple18*)((struct _tuple18*)_tmp85A->hd))).f1;_tmp85F->f2=_tmp84B;_tmp85F;});
_tmp85E->tl=ans;_tmp85E;});break;}}if(_tmp85A == 0)({struct Cyc_String_pa_struct
_tmp862;_tmp862.tag=0;_tmp862.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_tmp857);{void*_tmp860[1]={& _tmp862};Cyc_Tcutil_terr(loc,({const char*_tmp861="bad field designator %s";
_tag_arr(_tmp861,sizeof(char),_get_zero_arr_size(_tmp861,24));}),_tag_arr(
_tmp860,sizeof(void*),1));}});goto _LL531;}_LL531:;}}}for(0;fields != 0;fields=
fields->tl){if(!(*((struct _tuple18*)((struct _tuple18*)fields->hd))).f2){({void*
_tmp863[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp864="too few arguments to struct";
_tag_arr(_tmp864,sizeof(char),_get_zero_arr_size(_tmp864,28));}),_tag_arr(
_tmp863,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp865=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp866;
void*_tmp867;struct Cyc_Absyn_Conref*_tmp868;_LL537: if(_tmp865 <= (void*)3?1:*((
int*)_tmp865)!= 4)goto _LL539;_tmp866=((struct Cyc_Absyn_PointerType_struct*)
_tmp865)->f1;_tmp867=(void*)_tmp866.elt_typ;_tmp868=_tmp866.bounds;_LL538: {
struct Cyc_Absyn_Conref*_tmp869=Cyc_Absyn_compress_conref(_tmp868);void*_tmp86A=(
void*)(Cyc_Absyn_compress_conref(_tmp869))->v;void*_tmp86B;_LL53C: if(_tmp86A <= (
void*)1?1:*((int*)_tmp86A)!= 0)goto _LL53E;_tmp86B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp86A)->f1;if((int)_tmp86B != 0)goto _LL53E;_LL53D:*((void**)elt_typ_dest)=
_tmp867;return 1;_LL53E: if((int)_tmp86A != 0)goto _LL540;_LL53F:(void*)(_tmp869->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp86C=_cycalloc(sizeof(*
_tmp86C));_tmp86C[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp86D;_tmp86D.tag=0;
_tmp86D.f1=(void*)((void*)0);_tmp86D;});_tmp86C;})));*((void**)elt_typ_dest)=
_tmp867;return 1;_LL540:;_LL541: return 0;_LL53B:;}_LL539:;_LL53A: return 0;_LL536:;}
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){void*_tmp86E=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp86F;void*_tmp870;struct Cyc_Absyn_Conref*
_tmp871;_LL543: if(_tmp86E <= (void*)3?1:*((int*)_tmp86E)!= 4)goto _LL545;_tmp86F=((
struct Cyc_Absyn_PointerType_struct*)_tmp86E)->f1;_tmp870=(void*)_tmp86F.elt_typ;
_tmp871=_tmp86F.zero_term;_LL544:*((void**)elt_typ_dest)=_tmp870;return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp871);_LL545:;_LL546:
return 0;_LL542:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)
0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct
_tuple10 _tmp8B4;_tmp8B4.f1=0;_tmp8B4.f2=(void*)2;_tmp8B4;});void*_tmp872=(void*)
e->r;struct _tuple1*_tmp873;void*_tmp874;struct Cyc_Absyn_Exp*_tmp875;struct
_tagged_arr*_tmp876;struct Cyc_Absyn_Exp*_tmp877;struct _tagged_arr*_tmp878;struct
Cyc_Absyn_Exp*_tmp879;struct Cyc_Absyn_Exp*_tmp87A;struct Cyc_Absyn_Exp*_tmp87B;
_LL548: if(*((int*)_tmp872)!= 1)goto _LL54A;_tmp873=((struct Cyc_Absyn_Var_e_struct*)
_tmp872)->f1;_tmp874=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp872)->f2;_LL549: {
void*_tmp87C=_tmp874;struct Cyc_Absyn_Vardecl*_tmp87D;struct Cyc_Absyn_Vardecl*
_tmp87E;struct Cyc_Absyn_Vardecl*_tmp87F;struct Cyc_Absyn_Vardecl*_tmp880;_LL555:
if((int)_tmp87C != 0)goto _LL557;_LL556: goto _LL558;_LL557: if(_tmp87C <= (void*)1?1:*((
int*)_tmp87C)!= 1)goto _LL559;_LL558: return bogus_ans;_LL559: if(_tmp87C <= (void*)1?
1:*((int*)_tmp87C)!= 0)goto _LL55B;_tmp87D=((struct Cyc_Absyn_Global_b_struct*)
_tmp87C)->f1;_LL55A: {void*_tmp881=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL562: if(_tmp881 <= (void*)3?1:*((int*)_tmp881)!= 7)
goto _LL564;_LL563: return({struct _tuple10 _tmp882;_tmp882.f1=1;_tmp882.f2=(void*)2;
_tmp882;});_LL564:;_LL565: return({struct _tuple10 _tmp883;_tmp883.f1=(_tmp87D->tq).q_const;
_tmp883.f2=(void*)2;_tmp883;});_LL561:;}_LL55B: if(_tmp87C <= (void*)1?1:*((int*)
_tmp87C)!= 3)goto _LL55D;_tmp87E=((struct Cyc_Absyn_Local_b_struct*)_tmp87C)->f1;
_LL55C: {void*_tmp884=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL567: if(_tmp884 <= (void*)3?1:*((int*)_tmp884)!= 7)
goto _LL569;_LL568: return({struct _tuple10 _tmp885;_tmp885.f1=1;_tmp885.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp87E->rgn))->v;_tmp885;});_LL569:;_LL56A:
_tmp87E->escapes=1;return({struct _tuple10 _tmp886;_tmp886.f1=(_tmp87E->tq).q_const;
_tmp886.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp87E->rgn))->v;_tmp886;});
_LL566:;}_LL55D: if(_tmp87C <= (void*)1?1:*((int*)_tmp87C)!= 4)goto _LL55F;_tmp87F=((
struct Cyc_Absyn_Pat_b_struct*)_tmp87C)->f1;_LL55E: _tmp880=_tmp87F;goto _LL560;
_LL55F: if(_tmp87C <= (void*)1?1:*((int*)_tmp87C)!= 2)goto _LL554;_tmp880=((struct
Cyc_Absyn_Param_b_struct*)_tmp87C)->f1;_LL560: _tmp880->escapes=1;return({struct
_tuple10 _tmp887;_tmp887.f1=(_tmp880->tq).q_const;_tmp887.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp880->rgn))->v;_tmp887;});_LL554:;}_LL54A: if(*((int*)_tmp872)!= 
21)goto _LL54C;_tmp875=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp872)->f1;
_tmp876=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp872)->f2;_LL54B: {void*
_tmp888=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp875->topt))->v);
struct Cyc_List_List*_tmp889;struct Cyc_Absyn_AggrInfo _tmp88A;void*_tmp88B;struct
Cyc_Absyn_Aggrdecl**_tmp88C;struct Cyc_Absyn_Aggrdecl*_tmp88D;_LL56C: if(_tmp888 <= (
void*)3?1:*((int*)_tmp888)!= 11)goto _LL56E;_tmp889=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp888)->f2;_LL56D: {struct Cyc_Absyn_Aggrfield*_tmp88E=Cyc_Absyn_lookup_field(
_tmp889,_tmp876);if(_tmp88E != 0?_tmp88E->width != 0: 0)return({struct _tuple10
_tmp88F;_tmp88F.f1=(_tmp88E->tq).q_const;_tmp88F.f2=(Cyc_Tcutil_addressof_props(
te,_tmp875)).f2;_tmp88F;});return bogus_ans;}_LL56E: if(_tmp888 <= (void*)3?1:*((
int*)_tmp888)!= 10)goto _LL570;_tmp88A=((struct Cyc_Absyn_AggrType_struct*)_tmp888)->f1;
_tmp88B=(void*)_tmp88A.aggr_info;if(*((int*)_tmp88B)!= 1)goto _LL570;_tmp88C=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp88B)->f1;_tmp88D=*_tmp88C;_LL56F: {struct
Cyc_Absyn_Aggrfield*_tmp890=Cyc_Absyn_lookup_decl_field(_tmp88D,_tmp876);if(
_tmp890 != 0?_tmp890->width != 0: 0)return({struct _tuple10 _tmp891;_tmp891.f1=(
_tmp890->tq).q_const;_tmp891.f2=(Cyc_Tcutil_addressof_props(te,_tmp875)).f2;
_tmp891;});return bogus_ans;}_LL570:;_LL571: return bogus_ans;_LL56B:;}_LL54C: if(*((
int*)_tmp872)!= 22)goto _LL54E;_tmp877=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp872)->f1;_tmp878=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp872)->f2;_LL54D: {
void*_tmp892=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp877->topt))->v);struct Cyc_Absyn_PtrInfo _tmp893;void*_tmp894;void*_tmp895;
_LL573: if(_tmp892 <= (void*)3?1:*((int*)_tmp892)!= 4)goto _LL575;_tmp893=((struct
Cyc_Absyn_PointerType_struct*)_tmp892)->f1;_tmp894=(void*)_tmp893.elt_typ;
_tmp895=(void*)_tmp893.rgn_typ;_LL574: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp896=Cyc_Tcutil_compress(_tmp894);struct Cyc_List_List*_tmp897;struct Cyc_Absyn_AggrInfo
_tmp898;void*_tmp899;struct Cyc_Absyn_Aggrdecl**_tmp89A;struct Cyc_Absyn_Aggrdecl*
_tmp89B;_LL578: if(_tmp896 <= (void*)3?1:*((int*)_tmp896)!= 11)goto _LL57A;_tmp897=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp896)->f2;_LL579: finfo=Cyc_Absyn_lookup_field(
_tmp897,_tmp878);goto _LL577;_LL57A: if(_tmp896 <= (void*)3?1:*((int*)_tmp896)!= 10)
goto _LL57C;_tmp898=((struct Cyc_Absyn_AggrType_struct*)_tmp896)->f1;_tmp899=(void*)
_tmp898.aggr_info;if(*((int*)_tmp899)!= 1)goto _LL57C;_tmp89A=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp899)->f1;_tmp89B=*_tmp89A;_LL57B: finfo=Cyc_Absyn_lookup_decl_field(_tmp89B,
_tmp878);goto _LL577;_LL57C:;_LL57D: return bogus_ans;_LL577:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple10 _tmp89C;_tmp89C.f1=(finfo->tq).q_const;_tmp89C.f2=
_tmp895;_tmp89C;});return bogus_ans;}_LL575:;_LL576: return bogus_ans;_LL572:;}
_LL54E: if(*((int*)_tmp872)!= 20)goto _LL550;_tmp879=((struct Cyc_Absyn_Deref_e_struct*)
_tmp872)->f1;_LL54F: {void*_tmp89D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp879->topt))->v);struct Cyc_Absyn_PtrInfo _tmp89E;void*_tmp89F;
struct Cyc_Absyn_Tqual _tmp8A0;_LL57F: if(_tmp89D <= (void*)3?1:*((int*)_tmp89D)!= 4)
goto _LL581;_tmp89E=((struct Cyc_Absyn_PointerType_struct*)_tmp89D)->f1;_tmp89F=(
void*)_tmp89E.rgn_typ;_tmp8A0=_tmp89E.tq;_LL580: return({struct _tuple10 _tmp8A1;
_tmp8A1.f1=_tmp8A0.q_const;_tmp8A1.f2=_tmp89F;_tmp8A1;});_LL581:;_LL582: return
bogus_ans;_LL57E:;}_LL550: if(*((int*)_tmp872)!= 23)goto _LL552;_tmp87A=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp872)->f1;_tmp87B=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp872)->f2;_LL551: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp87A->topt))->v);void*_tmp8A2=t;struct Cyc_List_List*_tmp8A3;
struct Cyc_Absyn_PtrInfo _tmp8A4;void*_tmp8A5;void*_tmp8A6;struct Cyc_Absyn_Tqual
_tmp8A7;struct Cyc_Absyn_Conref*_tmp8A8;struct Cyc_Absyn_ArrayInfo _tmp8A9;struct
Cyc_Absyn_Tqual _tmp8AA;_LL584: if(_tmp8A2 <= (void*)3?1:*((int*)_tmp8A2)!= 9)goto
_LL586;_tmp8A3=((struct Cyc_Absyn_TupleType_struct*)_tmp8A2)->f1;_LL585: {
unsigned int _tmp8AC;int _tmp8AD;struct _tuple7 _tmp8AB=Cyc_Evexp_eval_const_uint_exp(
_tmp87B);_tmp8AC=_tmp8AB.f1;_tmp8AD=_tmp8AB.f2;if(!_tmp8AD)return bogus_ans;{
struct _tuple4*_tmp8AE=Cyc_Absyn_lookup_tuple_field(_tmp8A3,(int)_tmp8AC);if(
_tmp8AE != 0)return({struct _tuple10 _tmp8AF;_tmp8AF.f1=((*((struct _tuple4*)_tmp8AE)).f1).q_const;
_tmp8AF.f2=(Cyc_Tcutil_addressof_props(te,_tmp87A)).f2;_tmp8AF;});return
bogus_ans;}}_LL586: if(_tmp8A2 <= (void*)3?1:*((int*)_tmp8A2)!= 4)goto _LL588;
_tmp8A4=((struct Cyc_Absyn_PointerType_struct*)_tmp8A2)->f1;_tmp8A5=(void*)
_tmp8A4.elt_typ;_tmp8A6=(void*)_tmp8A4.rgn_typ;_tmp8A7=_tmp8A4.tq;_tmp8A8=
_tmp8A4.bounds;_LL587: return({struct _tuple10 _tmp8B0;_tmp8B0.f1=_tmp8A7.q_const;
_tmp8B0.f2=_tmp8A6;_tmp8B0;});_LL588: if(_tmp8A2 <= (void*)3?1:*((int*)_tmp8A2)!= 
7)goto _LL58A;_tmp8A9=((struct Cyc_Absyn_ArrayType_struct*)_tmp8A2)->f1;_tmp8AA=
_tmp8A9.tq;_LL589: return({struct _tuple10 _tmp8B1;_tmp8B1.f1=_tmp8AA.q_const;
_tmp8B1.f2=(Cyc_Tcutil_addressof_props(te,_tmp87A)).f2;_tmp8B1;});_LL58A:;_LL58B:
return bogus_ans;_LL583:;}_LL552:;_LL553:({void*_tmp8B2[0]={};Cyc_Tcutil_terr(e->loc,({
const char*_tmp8B3="unary & applied to non-lvalue";_tag_arr(_tmp8B3,sizeof(char),
_get_zero_arr_size(_tmp8B3,30));}),_tag_arr(_tmp8B2,sizeof(void*),0));});return
bogus_ans;_LL547:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*
e_typ,struct Cyc_Absyn_Exp*e){void*_tmp8B5=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_ArrayInfo
_tmp8B6;void*_tmp8B7;struct Cyc_Absyn_Tqual _tmp8B8;struct Cyc_Absyn_Conref*_tmp8B9;
_LL58D: if(_tmp8B5 <= (void*)3?1:*((int*)_tmp8B5)!= 7)goto _LL58F;_tmp8B6=((struct
Cyc_Absyn_ArrayType_struct*)_tmp8B5)->f1;_tmp8B7=(void*)_tmp8B6.elt_type;_tmp8B8=
_tmp8B6.tq;_tmp8B9=_tmp8B6.zero_term;_LL58E: {void*_tmp8BB;struct _tuple10 _tmp8BA=
Cyc_Tcutil_addressof_props(te,e);_tmp8BB=_tmp8BA.f2;return Cyc_Absyn_atb_typ(
_tmp8B7,_tmp8BB,_tmp8B8,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8BC=
_cycalloc(sizeof(*_tmp8BC));_tmp8BC[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8BD;
_tmp8BD.tag=0;_tmp8BD.f1=e;_tmp8BD;});_tmp8BC;}),_tmp8B9);}_LL58F:;_LL590: return
e_typ;_LL58C:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp8BE=(void*)b->v;void*_tmp8BF;void*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8C1;_LL592:
if(_tmp8BE <= (void*)1?1:*((int*)_tmp8BE)!= 0)goto _LL594;_tmp8BF=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp8BE)->f1;if((int)_tmp8BF != 0)goto _LL594;_LL593:
return;_LL594: if(_tmp8BE <= (void*)1?1:*((int*)_tmp8BE)!= 0)goto _LL596;_tmp8C0=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp8BE)->f1;if(_tmp8C0 <= (void*)1?1:*((
int*)_tmp8C0)!= 0)goto _LL596;_tmp8C1=((struct Cyc_Absyn_Upper_b_struct*)_tmp8C0)->f1;
_LL595: {unsigned int _tmp8C3;int _tmp8C4;struct _tuple7 _tmp8C2=Cyc_Evexp_eval_const_uint_exp(
_tmp8C1);_tmp8C3=_tmp8C2.f1;_tmp8C4=_tmp8C2.f2;if(_tmp8C4?_tmp8C3 <= i: 0)({struct
Cyc_Int_pa_struct _tmp8C8;_tmp8C8.tag=1;_tmp8C8.f1=(int)i;{struct Cyc_Int_pa_struct
_tmp8C7;_tmp8C7.tag=1;_tmp8C7.f1=(int)_tmp8C3;{void*_tmp8C5[2]={& _tmp8C7,&
_tmp8C8};Cyc_Tcutil_terr(loc,({const char*_tmp8C6="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp8C6,sizeof(char),_get_zero_arr_size(_tmp8C6,39));}),_tag_arr(
_tmp8C5,sizeof(void*),2));}}});return;}_LL596:;_LL597:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp8CA;_tmp8CA.tag=0;_tmp8CA.f1=(void*)((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp8CC;_tmp8CC.tag=0;_tmp8CC.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp8CC;});_tmp8CB;}));_tmp8CA;});_tmp8C9;})));return;_LL591:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){void*_tmp8CD=(
void*)(Cyc_Absyn_compress_conref(b))->v;void*_tmp8CE;struct Cyc_Absyn_Exp*_tmp8CF;
_LL599: if(_tmp8CD <= (void*)1?1:*((int*)_tmp8CD)!= 0)goto _LL59B;_tmp8CE=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp8CD)->f1;if(_tmp8CE <= (void*)1?1:*((int*)
_tmp8CE)!= 0)goto _LL59B;_tmp8CF=((struct Cyc_Absyn_Upper_b_struct*)_tmp8CE)->f1;
_LL59A: {unsigned int _tmp8D1;int _tmp8D2;struct _tuple7 _tmp8D0=Cyc_Evexp_eval_const_uint_exp(
_tmp8CF);_tmp8D1=_tmp8D0.f1;_tmp8D2=_tmp8D0.f2;return _tmp8D2?_tmp8D1 == 1: 0;}
_LL59B:;_LL59C: return 0;_LL598:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp8D3=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp8D4;void*_tmp8D5;struct Cyc_Absyn_Conref*_tmp8D6;
struct Cyc_List_List*_tmp8D7;struct Cyc_Absyn_AggrInfo _tmp8D8;void*_tmp8D9;void*
_tmp8DA;struct Cyc_Absyn_AggrInfo _tmp8DB;void*_tmp8DC;void*_tmp8DD;struct Cyc_Absyn_AggrInfo
_tmp8DE;void*_tmp8DF;struct Cyc_Absyn_Aggrdecl**_tmp8E0;struct Cyc_Absyn_Aggrdecl*
_tmp8E1;struct Cyc_List_List*_tmp8E2;struct Cyc_List_List*_tmp8E3;_LL59E: if((int)
_tmp8D3 != 0)goto _LL5A0;_LL59F: goto _LL5A1;_LL5A0: if(_tmp8D3 <= (void*)3?1:*((int*)
_tmp8D3)!= 5)goto _LL5A2;_LL5A1: goto _LL5A3;_LL5A2: if((int)_tmp8D3 != 1)goto _LL5A4;
_LL5A3: goto _LL5A5;_LL5A4: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 6)goto _LL5A6;
_LL5A5: return 1;_LL5A6: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 12)goto _LL5A8;
_LL5A7: goto _LL5A9;_LL5A8: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 13)goto
_LL5AA;_LL5A9: return 0;_LL5AA: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 7)goto
_LL5AC;_tmp8D4=((struct Cyc_Absyn_ArrayType_struct*)_tmp8D3)->f1;_tmp8D5=(void*)
_tmp8D4.elt_type;_tmp8D6=_tmp8D4.zero_term;_LL5AB: return !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp8D6)?Cyc_Tcutil_bits_only(_tmp8D5): 0;_LL5AC: if(
_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 9)goto _LL5AE;_tmp8D7=((struct Cyc_Absyn_TupleType_struct*)
_tmp8D3)->f1;_LL5AD: for(0;_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)((struct _tuple4*)_tmp8D7->hd))).f2))return 0;}return 1;_LL5AE: if(
_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 10)goto _LL5B0;_tmp8D8=((struct Cyc_Absyn_AggrType_struct*)
_tmp8D3)->f1;_tmp8D9=(void*)_tmp8D8.aggr_info;if(*((int*)_tmp8D9)!= 0)goto _LL5B0;
_tmp8DA=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8D9)->f1;if((int)
_tmp8DA != 1)goto _LL5B0;_LL5AF: return 1;_LL5B0: if(_tmp8D3 <= (void*)3?1:*((int*)
_tmp8D3)!= 10)goto _LL5B2;_tmp8DB=((struct Cyc_Absyn_AggrType_struct*)_tmp8D3)->f1;
_tmp8DC=(void*)_tmp8DB.aggr_info;if(*((int*)_tmp8DC)!= 0)goto _LL5B2;_tmp8DD=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8DC)->f1;if((int)_tmp8DD != 0)
goto _LL5B2;_LL5B1: return 0;_LL5B2: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 10)
goto _LL5B4;_tmp8DE=((struct Cyc_Absyn_AggrType_struct*)_tmp8D3)->f1;_tmp8DF=(void*)
_tmp8DE.aggr_info;if(*((int*)_tmp8DF)!= 1)goto _LL5B4;_tmp8E0=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp8DF)->f1;_tmp8E1=*_tmp8E0;_tmp8E2=_tmp8DE.targs;_LL5B3: if((void*)_tmp8E1->kind
== (void*)1)return 1;if(_tmp8E1->impl == 0)return 0;{struct _RegionHandle _tmp8E4=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp8E4;_push_region(rgn);{struct Cyc_List_List*
_tmp8E5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp8E1->tvs,
_tmp8E2);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp8E1->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(
rgn,_tmp8E5,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp8E6=0;
_npop_handler(0);return _tmp8E6;}}}{int _tmp8E7=1;_npop_handler(0);return _tmp8E7;}};
_pop_region(rgn);}_LL5B4: if(_tmp8D3 <= (void*)3?1:*((int*)_tmp8D3)!= 11)goto
_LL5B6;_tmp8E3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp8D3)->f2;_LL5B5: for(0;
_tmp8E3 != 0;_tmp8E3=_tmp8E3->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp8E3->hd)->type))return 0;}return 1;_LL5B6:;_LL5B7: return 0;_LL59D:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp8E8=(void*)e->r;
struct _tuple1*_tmp8E9;void*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_Absyn_Exp*
_tmp8EC;struct Cyc_Absyn_Exp*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_Absyn_Exp*
_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8F1;void*_tmp8F2;
struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F4;struct Cyc_Absyn_Exp*
_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F6;struct Cyc_List_List*_tmp8F7;struct Cyc_List_List*
_tmp8F8;struct Cyc_List_List*_tmp8F9;void*_tmp8FA;struct Cyc_List_List*_tmp8FB;
struct Cyc_List_List*_tmp8FC;struct Cyc_List_List*_tmp8FD;_LL5B9: if(*((int*)
_tmp8E8)!= 0)goto _LL5BB;_LL5BA: goto _LL5BC;_LL5BB: if(*((int*)_tmp8E8)!= 16)goto
_LL5BD;_LL5BC: goto _LL5BE;_LL5BD: if(*((int*)_tmp8E8)!= 17)goto _LL5BF;_LL5BE: goto
_LL5C0;_LL5BF: if(*((int*)_tmp8E8)!= 18)goto _LL5C1;_LL5C0: goto _LL5C2;_LL5C1: if(*((
int*)_tmp8E8)!= 19)goto _LL5C3;_LL5C2: goto _LL5C4;_LL5C3: if(*((int*)_tmp8E8)!= 31)
goto _LL5C5;_LL5C4: goto _LL5C6;_LL5C5: if(*((int*)_tmp8E8)!= 32)goto _LL5C7;_LL5C6:
return 1;_LL5C7: if(*((int*)_tmp8E8)!= 1)goto _LL5C9;_tmp8E9=((struct Cyc_Absyn_Var_e_struct*)
_tmp8E8)->f1;_tmp8EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp8E8)->f2;_LL5C8: {
void*_tmp8FE=_tmp8EA;struct Cyc_Absyn_Vardecl*_tmp8FF;_LL5E6: if(_tmp8FE <= (void*)
1?1:*((int*)_tmp8FE)!= 1)goto _LL5E8;_LL5E7: return 1;_LL5E8: if(_tmp8FE <= (void*)1?
1:*((int*)_tmp8FE)!= 0)goto _LL5EA;_tmp8FF=((struct Cyc_Absyn_Global_b_struct*)
_tmp8FE)->f1;_LL5E9: {void*_tmp900=Cyc_Tcutil_compress((void*)_tmp8FF->type);
_LL5EF: if(_tmp900 <= (void*)3?1:*((int*)_tmp900)!= 7)goto _LL5F1;_LL5F0: goto _LL5F2;
_LL5F1: if(_tmp900 <= (void*)3?1:*((int*)_tmp900)!= 8)goto _LL5F3;_LL5F2: return 1;
_LL5F3:;_LL5F4: return var_okay;_LL5EE:;}_LL5EA: if((int)_tmp8FE != 0)goto _LL5EC;
_LL5EB: return 0;_LL5EC:;_LL5ED: return var_okay;_LL5E5:;}_LL5C9: if(*((int*)_tmp8E8)
!= 6)goto _LL5CB;_tmp8EB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8E8)->f1;
_tmp8EC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8E8)->f2;_tmp8ED=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8E8)->f3;_LL5CA: return(Cyc_Tcutil_cnst_exp(te,0,_tmp8EB)?Cyc_Tcutil_cnst_exp(
te,0,_tmp8EC): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp8ED): 0;_LL5CB: if(*((int*)_tmp8E8)
!= 7)goto _LL5CD;_tmp8EE=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp8E8)->f1;_tmp8EF=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp8E8)->f2;_LL5CC: return Cyc_Tcutil_cnst_exp(
te,0,_tmp8EE)?Cyc_Tcutil_cnst_exp(te,0,_tmp8EF): 0;_LL5CD: if(*((int*)_tmp8E8)!= 
11)goto _LL5CF;_tmp8F0=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp8E8)->f1;
_LL5CE: _tmp8F1=_tmp8F0;goto _LL5D0;_LL5CF: if(*((int*)_tmp8E8)!= 12)goto _LL5D1;
_tmp8F1=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp8E8)->f1;_LL5D0: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp8F1);_LL5D1: if(*((int*)_tmp8E8)!= 13)goto _LL5D3;_tmp8F2=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp8E8)->f1;_tmp8F3=((struct Cyc_Absyn_Cast_e_struct*)
_tmp8E8)->f2;_LL5D2: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp8F3);_LL5D3: if(*((
int*)_tmp8E8)!= 14)goto _LL5D5;_tmp8F4=((struct Cyc_Absyn_Address_e_struct*)
_tmp8E8)->f1;_LL5D4: return Cyc_Tcutil_cnst_exp(te,1,_tmp8F4);_LL5D5: if(*((int*)
_tmp8E8)!= 27)goto _LL5D7;_tmp8F5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp8E8)->f2;_tmp8F6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp8E8)->f3;
_LL5D6: return Cyc_Tcutil_cnst_exp(te,0,_tmp8F5)?Cyc_Tcutil_cnst_exp(te,0,_tmp8F6):
0;_LL5D7: if(*((int*)_tmp8E8)!= 26)goto _LL5D9;_tmp8F7=((struct Cyc_Absyn_Array_e_struct*)
_tmp8E8)->f1;_LL5D8: _tmp8F8=_tmp8F7;goto _LL5DA;_LL5D9: if(*((int*)_tmp8E8)!= 29)
goto _LL5DB;_tmp8F8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp8E8)->f2;_LL5DA:
_tmp8F9=_tmp8F8;goto _LL5DC;_LL5DB: if(*((int*)_tmp8E8)!= 28)goto _LL5DD;_tmp8F9=((
struct Cyc_Absyn_Struct_e_struct*)_tmp8E8)->f3;_LL5DC: for(0;_tmp8F9 != 0;_tmp8F9=
_tmp8F9->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)((struct _tuple21*)
_tmp8F9->hd))).f2))return 0;}return 1;_LL5DD: if(*((int*)_tmp8E8)!= 3)goto _LL5DF;
_tmp8FA=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp8E8)->f1;_tmp8FB=((struct
Cyc_Absyn_Primop_e_struct*)_tmp8E8)->f2;_LL5DE: _tmp8FC=_tmp8FB;goto _LL5E0;_LL5DF:
if(*((int*)_tmp8E8)!= 24)goto _LL5E1;_tmp8FC=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp8E8)->f1;_LL5E0: _tmp8FD=_tmp8FC;goto _LL5E2;_LL5E1: if(*((int*)_tmp8E8)!= 30)
goto _LL5E3;_tmp8FD=((struct Cyc_Absyn_Tunion_e_struct*)_tmp8E8)->f1;_LL5E2: for(0;
_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)
_tmp8FD->hd))return 0;}return 1;_LL5E3:;_LL5E4: return 0;_LL5B8:;}int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(void*t){void*_tmp901=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp902;struct Cyc_Absyn_Conref*
_tmp903;struct Cyc_Absyn_Conref*_tmp904;struct Cyc_Absyn_ArrayInfo _tmp905;void*
_tmp906;struct Cyc_List_List*_tmp907;struct Cyc_Absyn_AggrInfo _tmp908;void*_tmp909;
struct Cyc_List_List*_tmp90A;struct Cyc_List_List*_tmp90B;_LL5F6: if((int)_tmp901 != 
0)goto _LL5F8;_LL5F7: goto _LL5F9;_LL5F8: if(_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 
5)goto _LL5FA;_LL5F9: goto _LL5FB;_LL5FA: if((int)_tmp901 != 1)goto _LL5FC;_LL5FB: goto
_LL5FD;_LL5FC: if(_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 6)goto _LL5FE;_LL5FD:
return 1;_LL5FE: if(_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 4)goto _LL600;_tmp902=((
struct Cyc_Absyn_PointerType_struct*)_tmp901)->f1;_tmp903=_tmp902.nullable;
_tmp904=_tmp902.bounds;_LL5FF: {void*_tmp90C=(void*)(Cyc_Absyn_compress_conref(
_tmp904))->v;void*_tmp90D;void*_tmp90E;_LL60F: if(_tmp90C <= (void*)1?1:*((int*)
_tmp90C)!= 0)goto _LL611;_tmp90D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp90C)->f1;if((int)_tmp90D != 0)goto _LL611;_LL610: return 1;_LL611: if(_tmp90C <= (
void*)1?1:*((int*)_tmp90C)!= 0)goto _LL613;_tmp90E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp90C)->f1;if(_tmp90E <= (void*)1?1:*((int*)_tmp90E)!= 0)goto _LL613;_LL612: {
void*_tmp90F=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp903))->v;int _tmp910;_LL616: if(_tmp90F <= (void*)1?1:*((int*)_tmp90F)!= 0)goto
_LL618;_tmp910=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp90F)->f1;_LL617:
return _tmp910;_LL618:;_LL619: return 0;_LL615:;}_LL613:;_LL614: return 0;_LL60E:;}
_LL600: if(_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 7)goto _LL602;_tmp905=((struct
Cyc_Absyn_ArrayType_struct*)_tmp901)->f1;_tmp906=(void*)_tmp905.elt_type;_LL601:
return Cyc_Tcutil_supports_default(_tmp906);_LL602: if(_tmp901 <= (void*)3?1:*((int*)
_tmp901)!= 9)goto _LL604;_tmp907=((struct Cyc_Absyn_TupleType_struct*)_tmp901)->f1;
_LL603: for(0;_tmp907 != 0;_tmp907=_tmp907->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple4*)((struct _tuple4*)_tmp907->hd))).f2))return 0;}return 1;_LL604: if(
_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 10)goto _LL606;_tmp908=((struct Cyc_Absyn_AggrType_struct*)
_tmp901)->f1;_tmp909=(void*)_tmp908.aggr_info;_tmp90A=_tmp908.targs;_LL605: {
struct Cyc_Absyn_Aggrdecl*_tmp911=Cyc_Absyn_get_known_aggrdecl(_tmp909);if(
_tmp911->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp911->impl))->exist_vars
!= 0)return 0;return Cyc_Tcutil_fields_support_default(_tmp911->tvs,_tmp90A,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp911->impl))->fields);}_LL606: if(
_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 11)goto _LL608;_tmp90B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp901)->f2;_LL607: return Cyc_Tcutil_fields_support_default(0,0,_tmp90B);_LL608:
if(_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 13)goto _LL60A;_LL609: goto _LL60B;
_LL60A: if(_tmp901 <= (void*)3?1:*((int*)_tmp901)!= 12)goto _LL60C;_LL60B: return 1;
_LL60C:;_LL60D: return 0;_LL5F5:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp912=_new_region("rgn");struct _RegionHandle*rgn=& _tmp912;
_push_region(rgn);{struct Cyc_List_List*_tmp913=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp913,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp914=0;_npop_handler(0);return _tmp914;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_admits_zero(void*t){void*_tmp915=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp916;struct Cyc_Absyn_Conref*_tmp917;struct Cyc_Absyn_Conref*_tmp918;_LL61B: if(
_tmp915 <= (void*)3?1:*((int*)_tmp915)!= 5)goto _LL61D;_LL61C: goto _LL61E;_LL61D:
if((int)_tmp915 != 1)goto _LL61F;_LL61E: goto _LL620;_LL61F: if(_tmp915 <= (void*)3?1:*((
int*)_tmp915)!= 6)goto _LL621;_LL620: return 1;_LL621: if(_tmp915 <= (void*)3?1:*((
int*)_tmp915)!= 4)goto _LL623;_tmp916=((struct Cyc_Absyn_PointerType_struct*)
_tmp915)->f1;_tmp917=_tmp916.nullable;_tmp918=_tmp916.bounds;_LL622: {void*
_tmp919=(void*)(Cyc_Absyn_compress_conref(_tmp918))->v;void*_tmp91A;void*_tmp91B;
_LL626: if(_tmp919 <= (void*)1?1:*((int*)_tmp919)!= 0)goto _LL628;_tmp91A=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp919)->f1;if((int)_tmp91A != 0)goto _LL628;
_LL627: return 0;_LL628: if(_tmp919 <= (void*)1?1:*((int*)_tmp919)!= 0)goto _LL62A;
_tmp91B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp919)->f1;if(_tmp91B <= (
void*)1?1:*((int*)_tmp91B)!= 0)goto _LL62A;_LL629: {void*_tmp91C=(void*)(((struct
Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp917))->v;int _tmp91D;_LL62D: if(_tmp91C <= (void*)1?1:*((int*)_tmp91C)!= 0)goto
_LL62F;_tmp91D=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp91C)->f1;_LL62E:
return _tmp91D;_LL62F:;_LL630: return 0;_LL62C:;}_LL62A:;_LL62B: return 0;_LL625:;}
_LL623:;_LL624: return 0;_LL61A:;}int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp91E=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp91F;void*_tmp920;struct Cyc_Absyn_FnInfo
_tmp921;struct Cyc_List_List*_tmp922;_LL632: if(_tmp91E <= (void*)3?1:*((int*)
_tmp91E)!= 4)goto _LL634;_tmp91F=((struct Cyc_Absyn_PointerType_struct*)_tmp91E)->f1;
_tmp920=(void*)_tmp91F.elt_typ;_LL633: return Cyc_Tcutil_is_noreturn(_tmp920);
_LL634: if(_tmp91E <= (void*)3?1:*((int*)_tmp91E)!= 8)goto _LL636;_tmp921=((struct
Cyc_Absyn_FnType_struct*)_tmp91E)->f1;_tmp922=_tmp921.attributes;_LL635: for(0;
_tmp922 != 0;_tmp922=_tmp922->tl){void*_tmp923=(void*)_tmp922->hd;_LL639: if((int)
_tmp923 != 3)goto _LL63B;_LL63A: return 1;_LL63B:;_LL63C: continue;_LL638:;}goto
_LL631;_LL636:;_LL637: goto _LL631;_LL631:;}return 0;}
