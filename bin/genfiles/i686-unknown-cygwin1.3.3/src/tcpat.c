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
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static inline void *
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#ifdef _INLINE_FUNCTIONS
static inline char *
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
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
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.base) _throw_null();
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
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
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tag_arr(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr *
_init_tag_arr(struct _tagged_arr *arr_ptr,
              void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _tagged_arr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_untag_arr(struct _tagged_arr arr, unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline unsigned
_get_arr_size(struct _tagged_arr arr,unsigned elt_sz) {
  struct _tagged_arr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_plus(struct _tagged_arr arr,unsigned elt_sz,int change) {
  struct _tagged_arr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus_post(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  struct _tagged_arr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
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
extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct
_RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(struct Cyc_Set_Set*
s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];
int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static void Cyc_Tcpat_resolve_pat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp0=(void*)p->r;struct
_tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*_tmp3;struct Cyc_Absyn_AggrInfo
_tmp4;void*_tmp5;void*_tmp6;struct _tuple1*_tmp7;struct Cyc_List_List*_tmp8;struct
Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;_LL1: if(_tmp0 <= (void*)2?1:*((int*)
_tmp0)!= 11)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{
struct _handler_cons _tmpB;_push_handler(& _tmpB);{int _tmpD=0;if(setjmp(_tmpB.handler))
_tmpD=1;if(!_tmpD){{void*_tmpE=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp1);struct
Cyc_Absyn_Tuniondecl*_tmpF;struct Cyc_Absyn_Tunionfield*_tmp10;struct Cyc_Absyn_Enumdecl*
_tmp11;struct Cyc_Absyn_Enumfield*_tmp12;void*_tmp13;struct Cyc_Absyn_Enumfield*
_tmp14;_LLA: if(*((int*)_tmpE)!= 1)goto _LLC;_LLB:({void*_tmp15[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp16="struct tag used without arguments in pattern";
_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,45));}),_tag_arr(_tmp15,
sizeof(void*),0));});_npop_handler(0);return;_LLC: if(*((int*)_tmpE)!= 2)goto _LLE;
_tmpF=((struct Cyc_Tcenv_TunionRes_struct*)_tmpE)->f1;_tmp10=((struct Cyc_Tcenv_TunionRes_struct*)
_tmpE)->f2;_LLD:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp18;_tmp18.tag=8;_tmp18.f1=_tmpF;_tmp18.f2=_tmp10;_tmp18.f3=0;_tmp18;});
_tmp17;})));_npop_handler(0);return;_LLE: if(*((int*)_tmpE)!= 3)goto _LL10;_tmp11=((
struct Cyc_Tcenv_EnumRes_struct*)_tmpE)->f1;_tmp12=((struct Cyc_Tcenv_EnumRes_struct*)
_tmpE)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1A;_tmp1A.tag=9;_tmp1A.f1=_tmp11;_tmp1A.f2=_tmp12;_tmp1A;});_tmp19;})));
_npop_handler(0);return;_LL10: if(*((int*)_tmpE)!= 4)goto _LL12;_tmp13=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpE)->f1;_tmp14=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmpE)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp1C;_tmp1C.tag=10;_tmp1C.f1=(void*)_tmp13;_tmp1C.f2=_tmp14;_tmp1C;});_tmp1B;})));
_npop_handler(0);return;_LL12: if(*((int*)_tmpE)!= 0)goto _LL9;_LL13: goto _LL9;_LL9:;};
_pop_handler();}else{void*_tmpC=(void*)_exn_thrown;void*_tmp1E=_tmpC;_LL15: if(
_tmp1E != Cyc_Dict_Absent)goto _LL17;_LL16: goto _LL14;_LL17:;_LL18:(void)_throw(
_tmp1E);_LL14:;}}}{void*_tmp1F=(*_tmp1).f1;struct Cyc_List_List*_tmp20;_LL1A: if((
int)_tmp1F != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp1F <= (void*)1?1:*((int*)
_tmp1F)!= 0)goto _LL1E;_tmp20=((struct Cyc_Absyn_Rel_n_struct*)_tmp1F)->f1;if(
_tmp20 != 0)goto _LL1E;_LL1D:(*_tmp1).f1=(void*)0;(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({
struct Cyc_Absyn_Var_p_struct _tmp22;_tmp22.tag=0;_tmp22.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp22;});_tmp21;})));return;_LL1E:;_LL1F:({void*_tmp23[0]={};
Cyc_Tcutil_terr(p->loc,({const char*_tmp24="qualified variable in pattern";
_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,30));}),_tag_arr(_tmp23,
sizeof(void*),0));});return;_LL19:;}_LL3: if(_tmp0 <= (void*)2?1:*((int*)_tmp0)!= 
12)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_LL4:{struct _handler_cons _tmp25;
_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!
_tmp27){{void*_tmp28=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*
_tmp29;struct Cyc_Absyn_Tuniondecl*_tmp2A;struct Cyc_Absyn_Tunionfield*_tmp2B;
_LL21: if(*((int*)_tmp28)!= 1)goto _LL23;_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp28)->f1;_LL22: if((void*)_tmp29->kind == (void*)1){({void*_tmp2C[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp2D="cannot pattern-match a union";_tag_arr(_tmp2D,sizeof(
char),_get_zero_arr_size(_tmp2D,29));}),_tag_arr(_tmp2C,sizeof(void*),0));});
_npop_handler(0);return;}{struct Cyc_List_List*_tmp2E=0;for(0;_tmp3 != 0;_tmp3=
_tmp3->tl){_tmp2E=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->hd=({
struct _tuple7*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->f1=0;_tmp30->f2=(struct
Cyc_Absyn_Pat*)_tmp3->hd;_tmp30;});_tmp2F->tl=_tmp2E;_tmp2F;});}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp32;_tmp32.tag=7;_tmp32.f1=({struct
Cyc_Absyn_AggrInfo _tmp33;_tmp33.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp35;_tmp35.tag=1;_tmp35.f1=({struct Cyc_Absyn_Aggrdecl**_tmp36=_cycalloc(
sizeof(*_tmp36));_tmp36[0]=_tmp29;_tmp36;});_tmp35;});_tmp34;}));_tmp33.targs=0;
_tmp33;});_tmp32.f2=0;_tmp32.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp2E);_tmp32;});_tmp31;})));_npop_handler(0);return;}_LL23:
if(*((int*)_tmp28)!= 2)goto _LL25;_tmp2A=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp28)->f1;_tmp2B=((struct Cyc_Tcenv_TunionRes_struct*)_tmp28)->f2;_LL24:(void*)(
p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp37=_cycalloc(sizeof(*
_tmp37));_tmp37[0]=({struct Cyc_Absyn_Tunion_p_struct _tmp38;_tmp38.tag=8;_tmp38.f1=
_tmp2A;_tmp38.f2=_tmp2B;_tmp38.f3=_tmp3;_tmp38;});_tmp37;})));_npop_handler(0);
return;_LL25: if(*((int*)_tmp28)!= 3)goto _LL27;_LL26: goto _LL28;_LL27: if(*((int*)
_tmp28)!= 4)goto _LL29;_LL28:({void*_tmp39[0]={};Cyc_Tcutil_terr(p->loc,({const
char*_tmp3A="enum tag used with arguments in pattern";_tag_arr(_tmp3A,sizeof(
char),_get_zero_arr_size(_tmp3A,40));}),_tag_arr(_tmp39,sizeof(void*),0));});
_npop_handler(0);return;_LL29: if(*((int*)_tmp28)!= 0)goto _LL20;_LL2A: goto _LL20;
_LL20:;};_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp3C=_tmp26;
_LL2C: if(_tmp3C != Cyc_Dict_Absent)goto _LL2E;_LL2D: goto _LL2B;_LL2E:;_LL2F:(void)
_throw(_tmp3C);_LL2B:;}}}({struct Cyc_String_pa_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp2));{void*
_tmp3D[1]={& _tmp3F};Cyc_Tcutil_terr(p->loc,({const char*_tmp3E="%s is not a constructor in pattern";
_tag_arr(_tmp3E,sizeof(char),_get_zero_arr_size(_tmp3E,35));}),_tag_arr(_tmp3D,
sizeof(void*),1));}});return;_LL5: if(_tmp0 <= (void*)2?1:*((int*)_tmp0)!= 7)goto
_LL7;_tmp4=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1;_tmp5=(void*)_tmp4.aggr_info;
if(*((int*)_tmp5)!= 0)goto _LL7;_tmp6=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp5)->f1;_tmp7=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp5)->f2;_tmp8=_tmp4.targs;
_tmp9=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_LL6:{struct _handler_cons _tmp40;_push_handler(& _tmp40);{int _tmp42=0;
if(setjmp(_tmp40.handler))_tmp42=1;if(!_tmp42){{struct Cyc_Absyn_Aggrdecl**_tmp43=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp7);struct Cyc_Absyn_Aggrdecl*_tmp44=*
_tmp43;if((void*)_tmp44->kind == (void*)1){({void*_tmp45[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp46="cannot pattern-match a union";_tag_arr(_tmp46,sizeof(char),
_get_zero_arr_size(_tmp46,29));}),_tag_arr(_tmp45,sizeof(void*),0));});
_npop_handler(0);return;}if(_tmp44->impl == 0){({void*_tmp47[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp48="can't destructure an abstract struct";_tag_arr(_tmp48,
sizeof(char),_get_zero_arr_size(_tmp48,37));}),_tag_arr(_tmp47,sizeof(void*),0));});
_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->exist_vars)- ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp9);if(more_exists != 0)({void*_tmp49[
0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4A="wrong number of existentially bound type variables";
_tag_arr(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,51));}),_tag_arr(_tmp49,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp4C;_tmp4C.tag=7;_tmp4C.f1=({struct Cyc_Absyn_AggrInfo _tmp4D;_tmp4D.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));
_tmp4E[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp4F;_tmp4F.tag=1;_tmp4F.f1=_tmp43;
_tmp4F;});_tmp4E;}));_tmp4D.targs=_tmp8;_tmp4D;});_tmp4C.f2=_tmp9;_tmp4C.f3=
_tmpA;_tmp4C;});_tmp4B;})));}};_pop_handler();}else{void*_tmp41=(void*)
_exn_thrown;void*_tmp51=_tmp41;_LL31: if(_tmp51 != Cyc_Dict_Absent)goto _LL33;_LL32:({
void*_tmp52[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp53="Non-struct name has designator patterns";
_tag_arr(_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,40));}),_tag_arr(_tmp52,
sizeof(void*),0));});goto _LL30;_LL33:;_LL34:(void)_throw(_tmp51);_LL30:;}}}
return;_LL7:;_LL8: return;_LL0:;}static struct _tagged_arr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp54=_cycalloc(sizeof(*
_tmp54));_tmp54->v=s;_tmp54;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0?Cyc_Tcutil_coerceable(*topt): 0)return*topt;{void*_tmp55=Cyc_Tcutil_compress(
numt);_LL36: if(_tmp55 <= (void*)3?1:*((int*)_tmp55)!= 12)goto _LL38;_LL37: if(topt
!= 0)return*topt;goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}return numt;}static void
Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,
void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual();*v_result_ptr=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));
_tmp56->hd=vd;_tmp56->tl=*v_result_ptr;_tmp56;});}static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult
res2){struct _tuple6*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_Tcpat_TcPatResult
_tmp57=res1;_tmp58=_tmp57.tvars_and_bounds_opt;_tmp59=_tmp57.patvars;{struct
_tuple6*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_Tcpat_TcPatResult _tmp5A=res2;
_tmp5B=_tmp5A.tvars_and_bounds_opt;_tmp5C=_tmp5A.patvars;if(_tmp58 != 0?1: _tmp5B
!= 0){if(_tmp58 == 0)_tmp58=({struct _tuple6*_tmp5D=_cycalloc(sizeof(*_tmp5D));
_tmp5D->f1=0;_tmp5D->f2=0;_tmp5D;});if(_tmp5B == 0)_tmp5B=({struct _tuple6*_tmp5E=
_cycalloc(sizeof(*_tmp5E));_tmp5E->f1=0;_tmp5E->f2=0;_tmp5E;});return({struct Cyc_Tcpat_TcPatResult
_tmp5F;_tmp5F.tvars_and_bounds_opt=({struct _tuple6*_tmp60=_cycalloc(sizeof(*
_tmp60));_tmp60->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)((*((struct _tuple6*)_check_null(_tmp58))).f1,(*((struct
_tuple6*)_check_null(_tmp5B))).f1);_tmp60->f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp58))).f2,(*((struct _tuple6*)_check_null(_tmp5B))).f2);_tmp60;});
_tmp5F.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp59,_tmp5C);_tmp5F;});}return({struct Cyc_Tcpat_TcPatResult
_tmp61;_tmp61.tvars_and_bounds_opt=0;_tmp61.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp59,_tmp5C);
_tmp61;});}}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{void*t;struct Cyc_Tcpat_TcPatResult res=({
struct Cyc_Tcpat_TcPatResult _tmp10F;_tmp10F.tvars_and_bounds_opt=0;_tmp10F.patvars=
0;_tmp10F;});{void*_tmp62=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp63;struct Cyc_Absyn_Vardecl*
_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;struct Cyc_Absyn_Enumdecl*_tmp68;void*
_tmp69;struct Cyc_Absyn_Pat*_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_Absyn_AggrInfo
_tmp6C;void*_tmp6D;struct Cyc_Absyn_Aggrdecl**_tmp6E;struct Cyc_Absyn_Aggrdecl*
_tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List**_tmp71;struct Cyc_List_List*
_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_Absyn_Tuniondecl*_tmp74;struct Cyc_Absyn_Tunionfield*
_tmp75;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_AggrInfo _tmp77;void*_tmp78;
_LL3B: if((int)_tmp62 != 0)goto _LL3D;_LL3C: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3A;_LL3D: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 0)goto _LL3F;
_tmp63=((struct Cyc_Absyn_Var_p_struct*)_tmp62)->f1;_LL3E: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(_tmp63,& res.patvars,t);
goto _LL3A;_LL3F: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 6)goto _LL41;_tmp64=((
struct Cyc_Absyn_Reference_p_struct*)_tmp62)->f1;_LL40: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(rgn_opt == 0){({void*_tmp79[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp7A="* pattern would point into an unknown/unallowed region";_tag_arr(_tmp7A,
sizeof(char),_get_zero_arr_size(_tmp7A,55));}),_tag_arr(_tmp79,sizeof(void*),0));});
goto _LL3A;}Cyc_Tcpat_set_vd(_tmp64,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7C;_tmp7C.tag=4;_tmp7C.f1=({struct Cyc_Absyn_PtrInfo _tmp7D;_tmp7D.elt_typ=(
void*)t;_tmp7D.elt_tq=Cyc_Absyn_empty_tqual();_tmp7D.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7E;_tmp7E.rgn=(void*)*rgn_opt;_tmp7E.nullable=Cyc_Absyn_false_conref;_tmp7E.bounds=
Cyc_Absyn_empty_conref();_tmp7E.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp7E;});_tmp7D;});_tmp7C;});_tmp7B;}));goto _LL3A;_LL41: if(_tmp62 <= (void*)2?1:*((
int*)_tmp62)!= 1)goto _LL43;_tmp65=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp62)->f1;
if((int)_tmp65 != 1)goto _LL43;_LL42: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3A;_LL43: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 1)goto _LL45;_tmp66=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp62)->f1;if((int)_tmp66 != 2)goto _LL45;
_LL44: goto _LL46;_LL45: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 1)goto _LL47;
_tmp67=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp62)->f1;if((int)_tmp67 != 0)
goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3A;_LL47:
if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 2)goto _LL49;_LL48: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_char_typ);goto _LL3A;_LL49: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)
!= 3)goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL3A;
_LL4B: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 9)goto _LL4D;_tmp68=((struct Cyc_Absyn_Enum_p_struct*)
_tmp62)->f1;_LL4C: t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_EnumType_struct
_tmp80;_tmp80.tag=12;_tmp80.f1=_tmp68->name;_tmp80.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp68;_tmp80;});_tmp7F;}));goto _LL3A;_LL4D: if(_tmp62 <= (void*)2?1:*((int*)
_tmp62)!= 10)goto _LL4F;_tmp69=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp62)->f1;
_LL4E: t=Cyc_Tcpat_num_type(topt,_tmp69);goto _LL3A;_LL4F: if((int)_tmp62 != 1)goto
_LL51;_LL50: if(topt != 0){void*_tmp81=Cyc_Tcutil_compress(*topt);_LL60: if(_tmp81
<= (void*)3?1:*((int*)_tmp81)!= 4)goto _LL62;_LL61: t=*topt;goto tcpat_end;_LL62:;
_LL63: goto _LL5F;_LL5F:;}{struct Cyc_List_List*_tmp82=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp83=_cycalloc(sizeof(*
_tmp83));_tmp83[0]=({struct Cyc_Absyn_PointerType_struct _tmp84;_tmp84.tag=4;
_tmp84.f1=({struct Cyc_Absyn_PtrInfo _tmp85;_tmp85.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp88=_cycalloc(sizeof(*
_tmp88));_tmp88->v=_tmp82;_tmp88;}));_tmp85.elt_tq=Cyc_Absyn_empty_tqual();
_tmp85.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp86;_tmp86.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87->v=_tmp82;_tmp87;}));_tmp86.nullable=Cyc_Absyn_true_conref;
_tmp86.bounds=Cyc_Absyn_empty_conref();_tmp86.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp86;});_tmp85;});_tmp84;});_tmp83;});goto _LL3A;}
_LL51: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 5)goto _LL53;_tmp6A=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp62)->f1;_LL52: {void*inner_typ=(void*)0;void**_tmp89=0;if(topt != 0){void*
_tmp8A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp8B;void*_tmp8C;
_LL65: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 4)goto _LL67;_tmp8B=((struct Cyc_Absyn_PointerType_struct*)
_tmp8A)->f1;_tmp8C=(void*)_tmp8B.elt_typ;_LL66: inner_typ=_tmp8C;_tmp89=(void**)&
inner_typ;goto _LL64;_LL67:;_LL68: goto _LL64;_LL64:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1->v=Cyc_Tcenv_lookup_type_vars(te);_tmpA1;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp6A,_tmp89,(void**)& ptr_rgn));if(_tmp89 == 0){void*
_tmp8D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmp8E;void*_tmp8F;struct Cyc_Absyn_Tuniondecl*
_tmp90;struct Cyc_Absyn_Tunionfield*_tmp91;struct Cyc_List_List*_tmp92;_LL6A: if(
_tmp8D <= (void*)3?1:*((int*)_tmp8D)!= 3)goto _LL6C;_tmp8E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp8D)->f1;_tmp8F=(void*)_tmp8E.field_info;if(*((int*)_tmp8F)!= 1)goto _LL6C;
_tmp90=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp8F)->f1;_tmp91=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp8F)->f2;_tmp92=_tmp8E.targs;_LL6B: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_TunionType_struct
_tmp94;_tmp94.tag=2;_tmp94.f1=({struct Cyc_Absyn_TunionInfo _tmp95;_tmp95.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp96=_cycalloc(sizeof(*
_tmp96));_tmp96[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp97;_tmp97.tag=1;
_tmp97.f1=({struct Cyc_Absyn_Tuniondecl**_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[
0]=_tmp90;_tmp98;});_tmp97;});_tmp96;}));_tmp95.targs=_tmp92;_tmp95.rgn=(void*)
ptr_rgn;_tmp95;});_tmp94;});_tmp93;});goto _LL69;_LL6C:;_LL6D: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({
struct Cyc_Absyn_PointerType_struct _tmp9A;_tmp9A.tag=4;_tmp9A.f1=({struct Cyc_Absyn_PtrInfo
_tmp9B;_tmp9B.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A->topt))->v);
_tmp9B.elt_tq=Cyc_Absyn_empty_tqual();_tmp9B.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp9C;_tmp9C.rgn=(void*)ptr_rgn;_tmp9C.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp9C.bounds=Cyc_Absyn_empty_conref();_tmp9C.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp9C;});_tmp9B;});
_tmp9A;});_tmp99;});goto _LL69;_LL69:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9E;_tmp9E.tag=4;_tmp9E.f1=({struct Cyc_Absyn_PtrInfo _tmp9F;_tmp9F.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A->topt))->v);_tmp9F.elt_tq=
Cyc_Absyn_empty_tqual();_tmp9F.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA0;_tmpA0.rgn=(
void*)ptr_rgn;_tmpA0.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpA0.bounds=Cyc_Absyn_empty_conref();_tmpA0.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA0;});_tmp9F;});_tmp9E;});_tmp9D;});}goto _LL3A;}}
_LL53: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 4)goto _LL55;_tmp6B=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp62)->f1;_LL54: {struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;
if(topt != 0){void*_tmpA2=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpA3;
_LL6F: if(_tmpA2 <= (void*)3?1:*((int*)_tmpA2)!= 9)goto _LL71;_tmpA3=((struct Cyc_Absyn_TupleType_struct*)
_tmpA2)->f1;_LL70: topt_ts=_tmpA3;goto _LL6E;_LL71:;_LL72: goto _LL6E;_LL6E:;}for(0;
_tmp6B != 0;_tmp6B=_tmp6B->tl){void**_tmpA4=0;if(topt_ts != 0){_tmpA4=(void**)&(*((
struct _tuple8*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp6B->hd,_tmpA4,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=({
struct _tuple8*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->f1=Cyc_Absyn_empty_tqual();
_tmpA6->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp6B->hd)->topt))->v;_tmpA6;});_tmpA5->tl=pat_ts;_tmpA5;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct
Cyc_Absyn_TupleType_struct _tmpA8;_tmpA8.tag=9;_tmpA8.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpA8;});_tmpA7;});goto _LL3A;}
_LL55: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 7)goto _LL57;_tmp6C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp62)->f1;_tmp6D=(void*)_tmp6C.aggr_info;if(*((int*)_tmp6D)!= 1)goto _LL57;
_tmp6E=((struct Cyc_Absyn_KnownAggr_struct*)_tmp6D)->f1;_tmp6F=*_tmp6E;_tmp70=
_tmp6C.targs;_tmp71=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)
_tmp62)->f1).targs;_tmp72=((struct Cyc_Absyn_Aggr_p_struct*)_tmp62)->f2;_tmp73=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp62)->f3;_LL56: if(_tmp6F->impl == 0){({void*
_tmpA9[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpAA="can't destructure an abstract struct";
_tag_arr(_tmpAA,sizeof(char),_get_zero_arr_size(_tmpAA,37));}),_tag_arr(_tmpA9,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpAB=_cycalloc(
sizeof(*_tmpAB));_tmpAB->v=Cyc_Tcenv_lookup_type_vars(te);_tmpAB;}));goto _LL3A;}{
struct _RegionHandle _tmpAC=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAC;
_push_region(rgn);{struct Cyc_List_List*_tmpAD=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpAE=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6F->impl))->exist_vars;{struct Cyc_List_List*t=_tmp72;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpAE))->hd;_tmpAE=
_tmpAE->tl;{void*_tmpAF=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpB0=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpB1=_tmpB0;void*_tmpB2;void*_tmpB3;
_LL74: if(*((int*)_tmpB1)!= 2)goto _LL76;_tmpB2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpB1)->f2;_LL75: _tmpB3=_tmpB2;goto _LL77;_LL76: if(*((int*)_tmpB1)!= 0)goto _LL78;
_tmpB3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpB1)->f1;_LL77: k2=_tmpB3;goto
_LL73;_LL78:;_LL79:({void*_tmpB4[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpB5="unconstrained existential type variable in struct";
_tag_arr(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,50));}),_tag_arr(_tmpB4,
sizeof(void*),0));});_LL73:;}{void*_tmpB6=_tmpAF;void*_tmpB7;struct Cyc_Core_Opt*
_tmpB8;struct Cyc_Core_Opt**_tmpB9;void*_tmpBA;struct Cyc_Core_Opt*_tmpBB;struct
Cyc_Core_Opt**_tmpBC;_LL7B: if(*((int*)_tmpB6)!= 0)goto _LL7D;_tmpB7=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpB6)->f1;_LL7C: if(!Cyc_Tcutil_kind_leq(k2,
_tmpB7))error=1;goto _LL7A;_LL7D: if(*((int*)_tmpB6)!= 2)goto _LL7F;_tmpB8=((struct
Cyc_Absyn_Less_kb_struct*)_tmpB6)->f1;_tmpB9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpB6)->f1;_tmpBA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpB6)->f2;_LL7E:
_tmpBC=_tmpB9;goto _LL80;_LL7F: if(*((int*)_tmpB6)!= 1)goto _LL7A;_tmpBB=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpB6)->f1;_tmpBC=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpB6)->f1;_LL80:*_tmpBC=({struct Cyc_Core_Opt*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->v=(void*)_tmpB0;_tmpBD;});goto _LL7A;
_LL7A:;}if(error)({struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(k2));{struct Cyc_String_pa_struct
_tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
_tmpAF));{struct Cyc_String_pa_struct _tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmpBE[3]={& _tmpC0,& _tmpC1,&
_tmpC2};Cyc_Tcutil_terr(p->loc,({const char*_tmpBF="type variable %s has kind %s but must have at least kind %s";
_tag_arr(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,60));}),_tag_arr(_tmpBE,
sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_VarType_struct
_tmpC7;_tmpC7.tag=1;_tmpC7.f1=tv;_tmpC7;});_tmpC6;});_tmpAD=({struct Cyc_List_List*
_tmpC3=_region_malloc(rgn,sizeof(*_tmpC3));_tmpC3->hd=(void*)((void*)vartype);
_tmpC3->tl=_tmpAD;_tmpC3;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=({struct _tuple0*_tmpC5=_cycalloc(
sizeof(*_tmpC5));_tmpC5->f1=Cyc_Absyn_empty_effect;_tmpC5->f2=vartype;_tmpC5;});
_tmpC4->tl=outlives_constraints;_tmpC4;});}}}}_tmpAD=Cyc_List_imp_rev(_tmpAD);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp72);struct Cyc_List_List*
_tmpC8=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpC9=({struct _tuple4 _tmpE4;
_tmpE4.f1=_tmpC8;_tmpE4.f2=rgn;_tmpE4;});struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpC9,_tmp6F->tvs);struct Cyc_List_List*_tmpCB=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6F->impl))->exist_vars,
_tmpAD);struct Cyc_List_List*_tmpCC=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpCA);struct Cyc_List_List*_tmpCD=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpCB);struct Cyc_List_List*_tmpCE=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpCA,_tmpCB);
if(_tmp72 != 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6F->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->f1=0;_tmpCF->f2=0;_tmpCF;});(*((struct
_tuple6*)_check_null(res.tvars_and_bounds_opt))).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(res.tvars_and_bounds_opt))).f1,_tmp72);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,outlives_constraints);{struct Cyc_List_List*_tmpD0=0;{
struct Cyc_List_List*_tmpD1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6F->impl))->rgn_po;
for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){_tmpD0=({struct Cyc_List_List*_tmpD2=
_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=({struct _tuple0*_tmpD3=_cycalloc(sizeof(*
_tmpD3));_tmpD3->f1=Cyc_Tcutil_rsubstitute(rgn,_tmpCE,(*((struct _tuple0*)_tmpD1->hd)).f1);
_tmpD3->f2=Cyc_Tcutil_rsubstitute(rgn,_tmpCE,(*((struct _tuple0*)_tmpD1->hd)).f2);
_tmpD3;});_tmpD2->tl=_tmpD0;_tmpD2;});}}_tmpD0=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD0);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,_tmpD0);}}*_tmp71=_tmpCC;t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AggrType_struct
_tmpD5;_tmpD5.tag=10;_tmpD5.f1=({struct Cyc_Absyn_AggrInfo _tmpD6;_tmpD6.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));
_tmpD7[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpD8;_tmpD8.tag=1;_tmpD8.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=_tmp6F;
_tmpD9;});_tmpD8;});_tmpD7;}));_tmpD6.targs=*_tmp71;_tmpD6;});_tmpD5;});_tmpD4;});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmp73,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6F->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple9 _tmpDB;struct Cyc_Absyn_Aggrfield*_tmpDC;struct Cyc_Absyn_Pat*_tmpDD;struct
_tuple9*_tmpDA=(struct _tuple9*)fields->hd;_tmpDB=*_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=
_tmpDB.f2;{void*_tmpDE=Cyc_Tcutil_rsubstitute(rgn,_tmpCE,(void*)_tmpDC->type);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmpDD,(void**)& _tmpDE,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpDD->topt))->v,
_tmpDE))({struct Cyc_String_pa_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpDD->topt))->v));{struct Cyc_String_pa_struct _tmpE2;_tmpE2.tag=0;
_tmpE2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpDE));{
struct Cyc_String_pa_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmpDC->name);{void*_tmpDF[3]={& _tmpE1,& _tmpE2,& _tmpE3};Cyc_Tcutil_terr(
p->loc,({const char*_tmpE0="field %s of struct pattern expects type %s != %s";
_tag_arr(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,49));}),_tag_arr(_tmpDF,
sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}goto _LL3A;_LL57: if(_tmp62 <= (
void*)2?1:*((int*)_tmp62)!= 8)goto _LL59;_tmp74=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp62)->f1;_tmp75=((struct Cyc_Absyn_Tunion_p_struct*)_tmp62)->f2;_tmp76=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp62)->f3;_LL58:{struct _RegionHandle _tmpE5=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpE5;_push_region(rgn);{struct Cyc_List_List*
tqts=_tmp75->typs;struct Cyc_List_List*_tmpE6=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple4 _tmpE7=({struct _tuple4 _tmp10C;_tmp10C.f1=_tmpE6;_tmp10C.f2=rgn;
_tmp10C;});struct Cyc_List_List*_tmpE8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpE7,_tmp74->tvs);struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpE8);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_TunionType_struct
_tmpEB;_tmpEB.tag=2;_tmpEB.f1=({struct Cyc_Absyn_TunionInfo _tmpEC;_tmpEC.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpEE=_cycalloc(sizeof(*
_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpEF;_tmpEF.tag=1;
_tmpEF.f1=({struct Cyc_Absyn_Tuniondecl**_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[
0]=_tmp74;_tmpF0;});_tmpEF;});_tmpEE;}));_tmpEC.targs=_tmpE9;_tmpEC.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->v=_tmpE6;_tmpED;}));_tmpEC;});_tmpEB;});
_tmpEA;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpF1=
_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpF2;_tmpF2.tag=3;_tmpF2.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpF3;_tmpF3.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpF4=_cycalloc(sizeof(*
_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpF5;_tmpF5.tag=1;
_tmpF5.f1=_tmp74;_tmpF5.f2=_tmp75;_tmpF5;});_tmpF4;}));_tmpF3.targs=_tmpE9;
_tmpF3;});_tmpF2;});_tmpF1;});}if(topt != 0?tqts == 0: 0){void*_tmpF6=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmpF7;struct Cyc_List_List*_tmpF8;_LL82: if(
_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 3)goto _LL84;_LL83: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpFA;_tmpFA.tag=3;_tmpFA.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpFB;_tmpFB.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpFC=_cycalloc(sizeof(*
_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpFD;_tmpFD.tag=1;
_tmpFD.f1=_tmp74;_tmpFD.f2=_tmp75;_tmpFD;});_tmpFC;}));_tmpFB.targs=_tmpE9;
_tmpFB;});_tmpFA;});_tmpF9;});goto _LL81;_LL84: if(_tmpF6 <= (void*)3?1:*((int*)
_tmpF6)!= 2)goto _LL86;_tmpF7=((struct Cyc_Absyn_TunionType_struct*)_tmpF6)->f1;
_tmpF8=_tmpF7.targs;_LL85: {struct Cyc_List_List*_tmpFE=_tmpE9;for(0;_tmpFE != 0?
_tmpF8 != 0: 0;(_tmpFE=_tmpFE->tl,_tmpF8=_tmpF8->tl)){Cyc_Tcutil_unify((void*)
_tmpFE->hd,(void*)_tmpF8->hd);}goto _LL81;}_LL86:;_LL87: goto _LL81;_LL81:;}for(0;
_tmp76 != 0?tqts != 0: 0;(_tmp76=_tmp76->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmpFF=(struct Cyc_Absyn_Pat*)_tmp76->hd;void*_tmp100=Cyc_Tcutil_rsubstitute(rgn,
_tmpE8,(*((struct _tuple8*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmpFF,(void**)& _tmp100,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpFF->topt))->v,_tmp100))({struct Cyc_String_pa_struct _tmp105;
_tmp105.tag=0;_tmp105.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpFF->topt))->v));{struct Cyc_String_pa_struct
_tmp104;_tmp104.tag=0;_tmp104.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp100));{struct Cyc_String_pa_struct _tmp103;_tmp103.tag=0;_tmp103.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp74->name));{void*
_tmp101[3]={& _tmp103,& _tmp104,& _tmp105};Cyc_Tcutil_terr(_tmpFF->loc,({const char*
_tmp102="%s expects argument type %s, not %s";_tag_arr(_tmp102,sizeof(char),
_get_zero_arr_size(_tmp102,36));}),_tag_arr(_tmp101,sizeof(void*),3));}}}});}if(
_tmp76 != 0)({struct Cyc_String_pa_struct _tmp108;_tmp108.tag=0;_tmp108.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp74->name));{void*
_tmp106[1]={& _tmp108};Cyc_Tcutil_terr(p->loc,({const char*_tmp107="too many arguments for tunion constructor %s";
_tag_arr(_tmp107,sizeof(char),_get_zero_arr_size(_tmp107,45));}),_tag_arr(
_tmp106,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct _tmp10B;
_tmp10B.tag=0;_tmp10B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp74->name));{void*_tmp109[1]={& _tmp10B};Cyc_Tcutil_terr(p->loc,({const char*
_tmp10A="too few arguments for tunion constructor %s";_tag_arr(_tmp10A,sizeof(
char),_get_zero_arr_size(_tmp10A,44));}),_tag_arr(_tmp109,sizeof(void*),1));}});};
_pop_region(rgn);}goto _LL3A;_LL59: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 7)
goto _LL5B;_tmp77=((struct Cyc_Absyn_Aggr_p_struct*)_tmp62)->f1;_tmp78=(void*)
_tmp77.aggr_info;if(*((int*)_tmp78)!= 0)goto _LL5B;_LL5A: goto _LL5C;_LL5B: if(
_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 11)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if(
_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 12)goto _LL3A;_LL5E: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp10D;}));goto _LL3A;_LL3A:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E->v=(void*)t;_tmp10E;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp110=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp111=_new_region("r");struct _RegionHandle*r=& _tmp111;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp110.patvars),p->loc,({const char*
_tmp112="pattern contains a repeated variable";_tag_arr(_tmp112,sizeof(char),
_get_zero_arr_size(_tmp112,37));}));;_pop_region(r);}return _tmp110;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp113=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_List_List*_tmp116;
struct Cyc_List_List*_tmp117;_LL89: if(_tmp113 <= (void*)2?1:*((int*)_tmp113)!= 5)
goto _LL8B;_tmp114=((struct Cyc_Absyn_Pointer_p_struct*)_tmp113)->f1;_LL8A: Cyc_Tcpat_check_pat_regions(
te,_tmp114);{void*_tmp118=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp119;struct Cyc_Absyn_PtrAtts _tmp11A;void*_tmp11B;
struct Cyc_Absyn_TunionInfo _tmp11C;void*_tmp11D;_LL94: if(_tmp118 <= (void*)3?1:*((
int*)_tmp118)!= 4)goto _LL96;_tmp119=((struct Cyc_Absyn_PointerType_struct*)
_tmp118)->f1;_tmp11A=_tmp119.ptr_atts;_tmp11B=(void*)_tmp11A.rgn;_LL95: _tmp11D=
_tmp11B;goto _LL97;_LL96: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 2)goto _LL98;
_tmp11C=((struct Cyc_Absyn_TunionType_struct*)_tmp118)->f1;_tmp11D=(void*)_tmp11C.rgn;
_LL97: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp11D);return;_LL98:;_LL99:({
void*_tmp11E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp11F="check_pat_regions: bad pointer type";_tag_arr(_tmp11F,sizeof(
char),_get_zero_arr_size(_tmp11F,36));}),_tag_arr(_tmp11E,sizeof(void*),0));});
_LL93:;}_LL8B: if(_tmp113 <= (void*)2?1:*((int*)_tmp113)!= 8)goto _LL8D;_tmp115=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp113)->f3;_LL8C: for(0;_tmp115 != 0;_tmp115=
_tmp115->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp115->hd);}{
void*_tmp120=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo
_tmp121;void*_tmp122;_LL9B: if(_tmp120 <= (void*)3?1:*((int*)_tmp120)!= 2)goto
_LL9D;_tmp121=((struct Cyc_Absyn_TunionType_struct*)_tmp120)->f1;_tmp122=(void*)
_tmp121.rgn;_LL9C: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp122);return;_LL9D:
if(_tmp120 <= (void*)3?1:*((int*)_tmp120)!= 3)goto _LL9F;_LL9E: return;_LL9F:;_LLA0:({
void*_tmp123[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp124="check_pat_regions: bad tunion type";_tag_arr(_tmp124,sizeof(
char),_get_zero_arr_size(_tmp124,35));}),_tag_arr(_tmp123,sizeof(void*),0));});
_LL9A:;}_LL8D: if(_tmp113 <= (void*)2?1:*((int*)_tmp113)!= 7)goto _LL8F;_tmp116=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp113)->f3;_LL8E: for(0;_tmp116 != 0;_tmp116=
_tmp116->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct _tuple7*)_tmp116->hd)).f2);}
return;_LL8F: if(_tmp113 <= (void*)2?1:*((int*)_tmp113)!= 4)goto _LL91;_tmp117=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp113)->f1;_LL90: for(0;_tmp117 != 0;_tmp117=
_tmp117->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp117->hd);}
return;_LL91:;_LL92: return;_LL88:;}struct Cyc_Tcpat_Name_v_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct Cyc_Tcpat_Con_s{
void*name;int arity;struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{int tag;
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){void*_tmp125=(void*)c1->name;
struct _tagged_arr _tmp126;int _tmp127;_LLA2: if(*((int*)_tmp125)!= 0)goto _LLA4;
_tmp126=((struct Cyc_Tcpat_Name_v_struct*)_tmp125)->f1;_LLA3: {void*_tmp128=(void*)
c2->name;struct _tagged_arr _tmp129;_LLA7: if(*((int*)_tmp128)!= 0)goto _LLA9;
_tmp129=((struct Cyc_Tcpat_Name_v_struct*)_tmp128)->f1;_LLA8: return Cyc_strcmp((
struct _tagged_arr)_tmp126,(struct _tagged_arr)_tmp129);_LLA9: if(*((int*)_tmp128)
!= 1)goto _LLA6;_LLAA: return - 1;_LLA6:;}_LLA4: if(*((int*)_tmp125)!= 1)goto _LLA1;
_tmp127=((struct Cyc_Tcpat_Int_v_struct*)_tmp125)->f1;_LLA5: {void*_tmp12A=(void*)
c2->name;int _tmp12B;_LLAC: if(*((int*)_tmp12A)!= 0)goto _LLAE;_LLAD: return 1;_LLAE:
if(*((int*)_tmp12A)!= 1)goto _LLAB;_tmp12B=((struct Cyc_Tcpat_Int_v_struct*)
_tmp12A)->f1;_LLAF: return _tmp127 - _tmp12B;_LLAB:;}_LLA1:;}static struct Cyc_Set_Set*
Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))
Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(void*)2};static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;static char _tmp12D[
5]="NULL";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={0,{
_tmp12D,_tmp12D,_tmp12D + 5}};static char _tmp12F[4]="ptr";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_ptr_name_value={0,{_tmp12F,_tmp12F,_tmp12F + 4}};static char _tmp131[6]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={0,{_tmp131,
_tmp131,_tmp131 + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(void*)((
void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp133=_cycalloc_atomic(sizeof(*
_tmp133));_tmp133[0]=({struct Cyc_Tcpat_Int_v_struct _tmp134;_tmp134.tag=1;_tmp134.f1=
i;_tmp134;});_tmp133;}));_tmp132->arity=0;_tmp132->span=0;_tmp132;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=f;_tmp137;});_tmp136;}));_tmp135->arity=0;
_tmp135->span=0;_tmp135;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c){
return({struct Cyc_Tcpat_Con_s*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp13A=_cycalloc_atomic(sizeof(*
_tmp13A));_tmp13A[0]=({struct Cyc_Tcpat_Int_v_struct _tmp13B;_tmp13B.tag=1;_tmp13B.f1=(
int)c;_tmp13B;});_tmp13A;}));_tmp138->arity=0;_tmp138->span=({struct Cyc_Core_Opt*
_tmp139=_cycalloc_atomic(sizeof(*_tmp139));_tmp139->v=(void*)256;_tmp139;});
_tmp138;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i){return({struct
Cyc_Tcpat_Con_s*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->name=(void*)((void*)&
Cyc_Tcpat_tuple_name_value);_tmp13C->arity=i;_tmp13C->span=Cyc_Tcpat_one_opt_ptr;
_tmp13C;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D[0]=({struct Cyc_Tcpat_Con_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=Cyc_Tcpat_null_con;_tmp13E.f2=0;_tmp13E;});
_tmp13D;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=({struct Cyc_Tcpat_Con_struct
_tmp140;_tmp140.tag=0;_tmp140.f1=Cyc_Tcpat_int_con(i);_tmp140.f2=0;_tmp140;});
_tmp13F;});}static void*Cyc_Tcpat_char_pat(char c){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=({struct Cyc_Tcpat_Con_struct
_tmp142;_tmp142.tag=0;_tmp142.f1=Cyc_Tcpat_char_con(c);_tmp142.f2=0;_tmp142;});
_tmp141;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({
struct Cyc_Tcpat_Con_struct _tmp144;_tmp144.tag=0;_tmp144.f1=Cyc_Tcpat_float_con(f);
_tmp144.f2=0;_tmp144;});_tmp143;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp145=_cycalloc(sizeof(*_tmp145));
_tmp145[0]=({struct Cyc_Tcpat_Con_struct _tmp146;_tmp146.tag=0;_tmp146.f1=Cyc_Tcpat_null_ptr_con;
_tmp146.f2=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->hd=(
void*)p;_tmp147->tl=0;_tmp147;});_tmp146;});_tmp145;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148[0]=({struct Cyc_Tcpat_Con_struct _tmp149;_tmp149.tag=0;_tmp149.f1=
Cyc_Tcpat_ptr_con;_tmp149.f2=({struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*
_tmp14A));_tmp14A->hd=(void*)p;_tmp14A->tl=0;_tmp14A;});_tmp149;});_tmp148;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Tcpat_Con_struct
_tmp14C;_tmp14C.tag=0;_tmp14C.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp14C.f2=ss;_tmp14C;});_tmp14B;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp150=_cycalloc(sizeof(*_tmp150));
_tmp150[0]=({struct Cyc_Tcpat_Name_v_struct _tmp151;_tmp151.tag=0;_tmp151.f1=
con_name;_tmp151;});_tmp150;}));_tmp14F->arity=Cyc_List_length(ps);_tmp14F->span=
span;_tmp14F;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp14D=_cycalloc(
sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Tcpat_Con_struct _tmp14E;_tmp14E.tag=0;
_tmp14E.f1=c;_tmp14E.f2=ps;_tmp14E;});_tmp14D;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp152=(void*)p->r;void*_tmp153;int _tmp154;
char _tmp155;struct _tagged_arr _tmp156;struct Cyc_Absyn_Pat*_tmp157;struct Cyc_Absyn_Tuniondecl*
_tmp158;struct Cyc_Absyn_Tunionfield*_tmp159;struct Cyc_List_List*_tmp15A;struct
Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_AggrInfo
_tmp15D;void*_tmp15E;struct Cyc_Absyn_Aggrdecl**_tmp15F;struct Cyc_Absyn_Aggrdecl*
_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Enumdecl*_tmp162;struct Cyc_Absyn_Enumfield*
_tmp163;void*_tmp164;struct Cyc_Absyn_Enumfield*_tmp165;_LLB1: if((int)_tmp152 != 0)
goto _LLB3;_LLB2: s=(void*)0;goto _LLB0;_LLB3: if(_tmp152 <= (void*)2?1:*((int*)
_tmp152)!= 0)goto _LLB5;_LLB4: s=(void*)0;goto _LLB0;_LLB5: if((int)_tmp152 != 1)goto
_LLB7;_LLB6: s=Cyc_Tcpat_null_pat();goto _LLB0;_LLB7: if(_tmp152 <= (void*)2?1:*((
int*)_tmp152)!= 1)goto _LLB9;_tmp153=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp152)->f1;_tmp154=((struct Cyc_Absyn_Int_p_struct*)_tmp152)->f2;_LLB8: s=Cyc_Tcpat_int_pat(
_tmp154);goto _LLB0;_LLB9: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 2)goto _LLBB;
_tmp155=((struct Cyc_Absyn_Char_p_struct*)_tmp152)->f1;_LLBA: s=Cyc_Tcpat_char_pat(
_tmp155);goto _LLB0;_LLBB: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 3)goto _LLBD;
_tmp156=((struct Cyc_Absyn_Float_p_struct*)_tmp152)->f1;_LLBC: s=Cyc_Tcpat_float_pat(
_tmp156);goto _LLB0;_LLBD: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 6)goto _LLBF;
_LLBE: s=(void*)0;goto _LLB0;_LLBF: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 5)
goto _LLC1;_tmp157=((struct Cyc_Absyn_Pointer_p_struct*)_tmp152)->f1;_LLC0:{void*
_tmp166=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp167;struct Cyc_Absyn_PtrAtts _tmp168;struct Cyc_Absyn_Conref*
_tmp169;_LLD4: if(_tmp166 <= (void*)3?1:*((int*)_tmp166)!= 4)goto _LLD6;_tmp167=((
struct Cyc_Absyn_PointerType_struct*)_tmp166)->f1;_tmp168=_tmp167.ptr_atts;
_tmp169=_tmp168.nullable;_LLD5: {int is_nullable=0;int still_working=1;while(
still_working){void*_tmp16A=(void*)_tmp169->v;struct Cyc_Absyn_Conref*_tmp16B;int
_tmp16C;_LLDB: if(_tmp16A <= (void*)1?1:*((int*)_tmp16A)!= 1)goto _LLDD;_tmp16B=((
struct Cyc_Absyn_Forward_constr_struct*)_tmp16A)->f1;_LLDC:(void*)(_tmp169->v=(
void*)((void*)_tmp16B->v));continue;_LLDD: if((int)_tmp16A != 0)goto _LLDF;_LLDE:(
void*)(_tmp169->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp16D=
_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp16E;
_tmp16E.tag=0;_tmp16E.f1=(void*)0;_tmp16E;});_tmp16D;})));is_nullable=0;
still_working=0;goto _LLDA;_LLDF: if(_tmp16A <= (void*)1?1:*((int*)_tmp16A)!= 0)
goto _LLDA;_tmp16C=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp16A)->f1;_LLE0:
is_nullable=(int)_tmp16C;still_working=0;goto _LLDA;_LLDA:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp157);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(ss);else{s=Cyc_Tcpat_ptr_pat(ss);}
goto _LLD3;}}_LLD6: if(_tmp166 <= (void*)3?1:*((int*)_tmp166)!= 2)goto _LLD8;_LLD7:{
void*_tmp16F=(void*)_tmp157->r;struct Cyc_Absyn_Tuniondecl*_tmp170;struct Cyc_Absyn_Tunionfield*
_tmp171;struct Cyc_List_List*_tmp172;_LLE2: if(_tmp16F <= (void*)2?1:*((int*)
_tmp16F)!= 8)goto _LLE4;_tmp170=((struct Cyc_Absyn_Tunion_p_struct*)_tmp16F)->f1;
_tmp171=((struct Cyc_Absyn_Tunion_p_struct*)_tmp16F)->f2;_tmp172=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp16F)->f3;_LLE3: {struct Cyc_Core_Opt*span;if(_tmp170->is_xtunion)span=0;else{
span=({struct Cyc_Core_Opt*_tmp173=_cycalloc_atomic(sizeof(*_tmp173));_tmp173->v=(
void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp170->fields))->v);_tmp173;});}s=Cyc_Tcpat_con_pat(*(*
_tmp171->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp172));goto _LLE1;}
_LLE4:;_LLE5:({void*_tmp174[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp175="non-[x]tunion pattern has tunion type";
_tag_arr(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,38));}),_tag_arr(
_tmp174,sizeof(void*),0));});_LLE1:;}goto _LLD3;_LLD8:;_LLD9:({void*_tmp176[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp177="pointer pattern does not have pointer type";_tag_arr(_tmp177,sizeof(
char),_get_zero_arr_size(_tmp177,43));}),_tag_arr(_tmp176,sizeof(void*),0));});
_LLD3:;}goto _LLB0;_LLC1: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 8)goto _LLC3;
_tmp158=((struct Cyc_Absyn_Tunion_p_struct*)_tmp152)->f1;_tmp159=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp152)->f2;_tmp15A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp152)->f3;if(_tmp15A
!= 0)goto _LLC3;_LLC2: {struct Cyc_Core_Opt*span;{void*_tmp178=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLE7: if(_tmp178 <= (void*)
3?1:*((int*)_tmp178)!= 2)goto _LLE9;_LLE8: if(_tmp158->is_xtunion)span=0;else{span=({
struct Cyc_Core_Opt*_tmp179=_cycalloc_atomic(sizeof(*_tmp179));_tmp179->v=(void*)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp158->fields))->v);_tmp179;});}goto _LLE6;_LLE9: if(_tmp178 <= (void*)
3?1:*((int*)_tmp178)!= 3)goto _LLEB;_LLEA: span=({struct Cyc_Core_Opt*_tmp17A=
_cycalloc_atomic(sizeof(*_tmp17A));_tmp17A->v=(void*)1;_tmp17A;});goto _LLE6;
_LLEB:;_LLEC: span=({void*_tmp17B[0]={};((struct Cyc_Core_Opt*(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp17C="void tunion pattern has bad type";
_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,33));}),_tag_arr(
_tmp17B,sizeof(void*),0));});goto _LLE6;_LLE6:;}s=Cyc_Tcpat_con_pat(*(*_tmp159->name).f2,
span,0);goto _LLB0;}_LLC3: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 8)goto _LLC5;
_tmp15B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp152)->f3;_LLC4: _tmp15C=_tmp15B;
goto _LLC6;_LLC5: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 4)goto _LLC7;_tmp15C=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp152)->f1;_LLC6: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp15C));goto _LLB0;_LLC7: if(_tmp152 <= (void*)
2?1:*((int*)_tmp152)!= 7)goto _LLC9;_tmp15D=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp152)->f1;_tmp15E=(void*)_tmp15D.aggr_info;if(*((int*)_tmp15E)!= 1)goto _LLC9;
_tmp15F=((struct Cyc_Absyn_KnownAggr_struct*)_tmp15E)->f1;_tmp160=*_tmp15F;
_tmp161=((struct Cyc_Absyn_Aggr_p_struct*)_tmp152)->f3;_LLC8: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp160->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*
_tmp18B="";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,1));}))== 0;{
struct Cyc_List_List*dlps0=_tmp161;for(0;!found?dlps0 != 0: 0;dlps0=dlps0->tl){
struct _tuple7 _tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_Absyn_Pat*_tmp180;
struct _tuple7*_tmp17D=(struct _tuple7*)dlps0->hd;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.f1;
_tmp180=_tmp17E.f2;{struct Cyc_List_List*_tmp181=_tmp17F;struct Cyc_List_List
_tmp182;void*_tmp183;struct _tagged_arr*_tmp184;struct Cyc_List_List*_tmp185;_LLEE:
if(_tmp181 == 0)goto _LLF0;_tmp182=*_tmp181;_tmp183=(void*)_tmp182.hd;if(*((int*)
_tmp183)!= 1)goto _LLF0;_tmp184=((struct Cyc_Absyn_FieldName_struct*)_tmp183)->f1;
_tmp185=_tmp182.tl;if(_tmp185 != 0)goto _LLF0;_LLEF: if(Cyc_strptrcmp(_tmp184,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp180);_tmp186->tl=ps;_tmp186;});found=1;}goto _LLED;_LLF0:;_LLF1:({void*
_tmp187[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp188="bad designator(s)";_tag_arr(_tmp188,sizeof(char),
_get_zero_arr_size(_tmp188,18));}),_tag_arr(_tmp187,sizeof(void*),0));});_LLED:;}}}
if(!found)({void*_tmp189[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp18A="bad designator";_tag_arr(_tmp18A,sizeof(
char),_get_zero_arr_size(_tmp18A,15));}),_tag_arr(_tmp189,sizeof(void*),0));});}}
s=Cyc_Tcpat_tuple_pat(ps);goto _LLB0;}_LLC9: if(_tmp152 <= (void*)2?1:*((int*)
_tmp152)!= 9)goto _LLCB;_tmp162=((struct Cyc_Absyn_Enum_p_struct*)_tmp152)->f1;
_tmp163=((struct Cyc_Absyn_Enum_p_struct*)_tmp152)->f2;_LLCA: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp162->fields))->v);s=Cyc_Tcpat_con_pat(*(*_tmp163->name).f2,({
struct Cyc_Core_Opt*_tmp18C=_cycalloc_atomic(sizeof(*_tmp18C));_tmp18C->v=(void*)
span;_tmp18C;}),0);goto _LLB0;}_LLCB: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 
10)goto _LLCD;_tmp164=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp152)->f1;
_tmp165=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp152)->f2;_LLCC: {struct Cyc_List_List*
fields;{void*_tmp18D=Cyc_Tcutil_compress(_tmp164);struct Cyc_List_List*_tmp18E;
_LLF3: if(_tmp18D <= (void*)3?1:*((int*)_tmp18D)!= 13)goto _LLF5;_tmp18E=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp18D)->f1;_LLF4: fields=_tmp18E;goto _LLF2;_LLF5:;
_LLF6:({void*_tmp18F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp190="bad type in AnonEnum_p";_tag_arr(_tmp190,
sizeof(char),_get_zero_arr_size(_tmp190,23));}),_tag_arr(_tmp18F,sizeof(void*),0));});
_LLF2:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*
_tmp165->name).f2,({struct Cyc_Core_Opt*_tmp191=_cycalloc_atomic(sizeof(*_tmp191));
_tmp191->v=(void*)span;_tmp191;}),0);goto _LLB0;}}_LLCD: if(_tmp152 <= (void*)2?1:*((
int*)_tmp152)!= 11)goto _LLCF;_LLCE:({void*_tmp192[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp193="compile_pat: unknownid";
_tag_arr(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,23));}),_tag_arr(
_tmp192,sizeof(void*),0));});_LLCF: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 12)
goto _LLD1;_LLD0:({void*_tmp194[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp195="compile_pat: unknowncall";
_tag_arr(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,25));}),_tag_arr(
_tmp194,sizeof(void*),0));});_LLD1: if(_tmp152 <= (void*)2?1:*((int*)_tmp152)!= 7)
goto _LLB0;_LLD2:({void*_tmp196[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp197="compile_pat: bad aggr";
_tag_arr(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,22));}),_tag_arr(
_tmp196,sizeof(void*),0));});_LLB0:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int
tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Success_struct{int tag;void*f1;};
struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*
f2;void*f3;void*f4;};struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp198=td;struct Cyc_Set_Set*_tmp199;_LLF8:
if(*((int*)_tmp198)!= 1)goto _LLFA;_tmp199=((struct Cyc_Tcpat_Neg_struct*)_tmp198)->f1;
_LLF9: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp199,c))({void*_tmp19A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp19B="add_neg called when constructor already in set";
_tag_arr(_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,47));}),_tag_arr(
_tmp19A,sizeof(void*),0));});if(c->span != 0?((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp199)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v: 0)({void*
_tmp19C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp19D="add_neg called when |cs U {c}| >= span(c)";_tag_arr(_tmp19D,
sizeof(char),_get_zero_arr_size(_tmp19D,42));}),_tag_arr(_tmp19C,sizeof(void*),0));});
return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp19E=_region_malloc(r,sizeof(*
_tmp19E));_tmp19E[0]=({struct Cyc_Tcpat_Neg_struct _tmp19F;_tmp19F.tag=1;_tmp19F.f1=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_rinsert)(r,_tmp199,c);_tmp19F;});_tmp19E;});_LLFA: if(*((int*)
_tmp198)!= 0)goto _LLF7;_LLFB:({void*_tmp1A0[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A1="add_neg called when td is Positive";
_tag_arr(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,35));}),_tag_arr(
_tmp1A0,sizeof(void*),0));});_LLF7:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp1A2=td;struct Cyc_Tcpat_Con_s*_tmp1A3;struct Cyc_Set_Set*
_tmp1A4;_LLFD: if(*((int*)_tmp1A2)!= 0)goto _LLFF;_tmp1A3=((struct Cyc_Tcpat_Pos_struct*)
_tmp1A2)->f1;_LLFE: if(Cyc_Tcpat_compare_con(c,_tmp1A3)== 0)return(void*)0;else{
return(void*)1;}_LLFF: if(*((int*)_tmp1A2)!= 1)goto _LLFC;_tmp1A4=((struct Cyc_Tcpat_Neg_struct*)
_tmp1A2)->f1;_LL100: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1A4,c))return(void*)1;else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1A4)+ 1: 0)return(void*)0;else{return(void*)2;}}_LLFC:;}struct _tuple11{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1A5=ctxt;struct Cyc_List_List _tmp1A6;struct _tuple11*_tmp1A7;struct _tuple11
_tmp1A8;struct Cyc_Tcpat_Con_s*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*
_tmp1AB;_LL102: if(_tmp1A5 != 0)goto _LL104;_LL103: return 0;_LL104: if(_tmp1A5 == 0)
goto _LL101;_tmp1A6=*_tmp1A5;_tmp1A7=(struct _tuple11*)_tmp1A6.hd;_tmp1A8=*_tmp1A7;
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;_tmp1AB=_tmp1A6.tl;_LL105: return({struct
Cyc_List_List*_tmp1AC=_region_malloc(r,sizeof(*_tmp1AC));_tmp1AC->hd=({struct
_tuple11*_tmp1AD=_region_malloc(r,sizeof(*_tmp1AD));_tmp1AD->f1=_tmp1A9;_tmp1AD->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1AE=_region_malloc(r,sizeof(*
_tmp1AE));_tmp1AE->hd=(void*)dsc;_tmp1AE->tl=_tmp1AA;_tmp1AE;});_tmp1AD;});
_tmp1AC->tl=_tmp1AB;_tmp1AC;});_LL101:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1AF=ctxt;
struct Cyc_List_List _tmp1B0;struct _tuple11*_tmp1B1;struct _tuple11 _tmp1B2;struct
Cyc_Tcpat_Con_s*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;
_LL107: if(_tmp1AF != 0)goto _LL109;_LL108:({void*_tmp1B6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1B7="norm_context: empty context";
_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,28));}),_tag_arr(
_tmp1B6,sizeof(void*),0));});_LL109: if(_tmp1AF == 0)goto _LL106;_tmp1B0=*_tmp1AF;
_tmp1B1=(struct _tuple11*)_tmp1B0.hd;_tmp1B2=*_tmp1B1;_tmp1B3=_tmp1B2.f1;_tmp1B4=
_tmp1B2.f2;_tmp1B5=_tmp1B0.tl;_LL10A: return Cyc_Tcpat_augment(r,_tmp1B5,(void*)({
struct Cyc_Tcpat_Pos_struct*_tmp1B8=_region_malloc(r,sizeof(*_tmp1B8));_tmp1B8[0]=({
struct Cyc_Tcpat_Pos_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=_tmp1B3;_tmp1B9.f2=
Cyc_List_rrev(r,_tmp1B4);_tmp1B9;});_tmp1B8;}));_LL106:;}static void*Cyc_Tcpat_build_desc(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1BB=({struct _tuple6 _tmp1BA;_tmp1BA.f1=ctxt;_tmp1BA.f2=work;
_tmp1BA;});struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_List_List*
_tmp1BE;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List
_tmp1C1;struct _tuple11*_tmp1C2;struct _tuple11 _tmp1C3;struct Cyc_Tcpat_Con_s*
_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_List_List*
_tmp1C7;struct Cyc_List_List _tmp1C8;struct _tuple10*_tmp1C9;struct _tuple10 _tmp1CA;
struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;_LL10C: _tmp1BC=_tmp1BB.f1;
if(_tmp1BC != 0)goto _LL10E;_tmp1BD=_tmp1BB.f2;if(_tmp1BD != 0)goto _LL10E;_LL10D:
return dsc;_LL10E: _tmp1BE=_tmp1BB.f1;if(_tmp1BE != 0)goto _LL110;_LL10F: goto _LL111;
_LL110: _tmp1BF=_tmp1BB.f2;if(_tmp1BF != 0)goto _LL112;_LL111:({void*_tmp1CD[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp1CE="build_desc: ctxt and work don't match";_tag_arr(_tmp1CE,sizeof(char),
_get_zero_arr_size(_tmp1CE,38));}),_tag_arr(_tmp1CD,sizeof(void*),0));});_LL112:
_tmp1C0=_tmp1BB.f1;if(_tmp1C0 == 0)goto _LL10B;_tmp1C1=*_tmp1C0;_tmp1C2=(struct
_tuple11*)_tmp1C1.hd;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;
_tmp1C6=_tmp1C1.tl;_tmp1C7=_tmp1BB.f2;if(_tmp1C7 == 0)goto _LL10B;_tmp1C8=*_tmp1C7;
_tmp1C9=(struct _tuple10*)_tmp1C8.hd;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.f3;_tmp1CC=
_tmp1C8.tl;_LL113: {struct Cyc_Tcpat_Pos_struct*_tmp1CF=({struct Cyc_Tcpat_Pos_struct*
_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1D1;_tmp1D1.tag=0;_tmp1D1.f1=_tmp1C4;_tmp1D1.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp1C5),({struct Cyc_List_List*_tmp1D2=_region_malloc(r,sizeof(*_tmp1D2));
_tmp1D2->hd=(void*)dsc;_tmp1D2->tl=_tmp1CB;_tmp1D2;}));_tmp1D1;});_tmp1D0;});
return Cyc_Tcpat_build_desc(r,_tmp1C6,(void*)_tmp1CF,_tmp1CC);}_LL10B:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*,void*,struct Cyc_List_List*,void*,
struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_List_List*);static void*
Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp1D3=allmrules;struct Cyc_List_List _tmp1D4;struct _tuple0*
_tmp1D5;struct _tuple0 _tmp1D6;void*_tmp1D7;void*_tmp1D8;struct Cyc_List_List*
_tmp1D9;_LL115: if(_tmp1D3 != 0)goto _LL117;_LL116: return(void*)0;_LL117: if(_tmp1D3
== 0)goto _LL114;_tmp1D4=*_tmp1D3;_tmp1D5=(struct _tuple0*)_tmp1D4.hd;_tmp1D6=*
_tmp1D5;_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;_tmp1D9=_tmp1D4.tl;_LL118: return
Cyc_Tcpat_match(r,_tmp1D7,0,dsc,0,0,_tmp1D8,_tmp1D9);_LL114:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));
_tmp1DA[0]=({struct Cyc_Tcpat_Neg_struct _tmp1DB;_tmp1DB.tag=1;_tmp1DB.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1DB;});_tmp1DA;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp1DC=work;
struct Cyc_List_List _tmp1DD;struct _tuple10*_tmp1DE;struct _tuple10 _tmp1DF;struct
Cyc_List_List*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E2;
struct Cyc_List_List*_tmp1E3;struct Cyc_List_List _tmp1E4;struct _tuple10*_tmp1E5;
struct _tuple10 _tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;
struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1EA;_LL11A: if(_tmp1DC != 0)
goto _LL11C;_LL11B: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp1EB=
_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Tcpat_Success_struct
_tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=(void*)right_hand_side;_tmp1EC;});_tmp1EB;});
_LL11C: if(_tmp1DC == 0)goto _LL11E;_tmp1DD=*_tmp1DC;_tmp1DE=(struct _tuple10*)
_tmp1DD.hd;_tmp1DF=*_tmp1DE;_tmp1E0=_tmp1DF.f1;if(_tmp1E0 != 0)goto _LL11E;_tmp1E1=
_tmp1DF.f2;if(_tmp1E1 != 0)goto _LL11E;_tmp1E2=_tmp1DF.f3;if(_tmp1E2 != 0)goto
_LL11E;_tmp1E3=_tmp1DD.tl;_LL11D: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp1E3,right_hand_side,rules);_LL11E: if(_tmp1DC == 0)goto _LL119;_tmp1E4=*
_tmp1DC;_tmp1E5=(struct _tuple10*)_tmp1E4.hd;_tmp1E6=*_tmp1E5;_tmp1E7=_tmp1E6.f1;
_tmp1E8=_tmp1E6.f2;_tmp1E9=_tmp1E6.f3;_tmp1EA=_tmp1E4.tl;_LL11F: if((_tmp1E7 == 0?
1: _tmp1E8 == 0)?1: _tmp1E9 == 0)({void*_tmp1ED[0]={};Cyc_Tcutil_impos(({const char*
_tmp1EE="tcpat:and_match: malformed work frame";_tag_arr(_tmp1EE,sizeof(char),
_get_zero_arr_size(_tmp1EE,38));}),_tag_arr(_tmp1ED,sizeof(void*),0));});{struct
Cyc_List_List _tmp1F0;void*_tmp1F1;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*
_tmp1EF=(struct Cyc_List_List*)_check_null(_tmp1E7);_tmp1F0=*_tmp1EF;_tmp1F1=(
void*)_tmp1F0.hd;_tmp1F2=_tmp1F0.tl;{struct Cyc_List_List _tmp1F4;struct Cyc_List_List*
_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F3=(struct Cyc_List_List*)
_check_null(_tmp1E8);_tmp1F4=*_tmp1F3;_tmp1F5=(struct Cyc_List_List*)_tmp1F4.hd;
_tmp1F6=_tmp1F4.tl;{struct Cyc_List_List _tmp1F8;void*_tmp1F9;struct Cyc_List_List*
_tmp1FA;struct Cyc_List_List*_tmp1F7=(struct Cyc_List_List*)_check_null(_tmp1E9);
_tmp1F8=*_tmp1F7;_tmp1F9=(void*)_tmp1F8.hd;_tmp1FA=_tmp1F8.tl;{struct _tuple10*
_tmp1FB=({struct _tuple10*_tmp1FD=_region_malloc(r,sizeof(*_tmp1FD));_tmp1FD->f1=
_tmp1F2;_tmp1FD->f2=_tmp1F6;_tmp1FD->f3=_tmp1FA;_tmp1FD;});return Cyc_Tcpat_match(
r,_tmp1F1,_tmp1F5,_tmp1F9,ctx,({struct Cyc_List_List*_tmp1FC=_region_malloc(r,
sizeof(*_tmp1FC));_tmp1FC->hd=_tmp1FB;_tmp1FC->tl=_tmp1EA;_tmp1FC;}),
right_hand_side,rules);}}}}_LL119:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp1FE=dsc;
struct Cyc_Set_Set*_tmp1FF;struct Cyc_List_List*_tmp200;_LL121: if(*((int*)_tmp1FE)
!= 1)goto _LL123;_tmp1FF=((struct Cyc_Tcpat_Neg_struct*)_tmp1FE)->f1;_LL122: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp203=_region_malloc(r,sizeof(*_tmp203));
_tmp203[0]=({struct Cyc_Tcpat_Neg_struct _tmp204;_tmp204.tag=1;_tmp204.f1=Cyc_Tcpat_empty_con_set(
r);_tmp204;});_tmp203;});struct Cyc_List_List*_tmp201=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp201=({struct Cyc_List_List*_tmp202=_region_malloc(r,sizeof(*_tmp202));
_tmp202->hd=(void*)any;_tmp202->tl=_tmp201;_tmp202;});}}return _tmp201;}_LL123:
if(*((int*)_tmp1FE)!= 0)goto _LL120;_tmp200=((struct Cyc_Tcpat_Pos_struct*)_tmp1FE)->f2;
_LL124: return _tmp200;_LL120:;}struct _tuple12{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple12*env,int i){struct
_tuple12 _tmp206;struct _RegionHandle*_tmp207;struct Cyc_List_List*_tmp208;struct
_tuple12*_tmp205=env;_tmp206=*_tmp205;_tmp207=_tmp206.f1;_tmp208=_tmp206.f2;
return({struct Cyc_List_List*_tmp209=_region_malloc(_tmp207,sizeof(*_tmp209));
_tmp209->hd=(void*)(i + 1);_tmp209->tl=_tmp208;_tmp209;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple12 _tmp20A=({struct _tuple12 _tmp20B;_tmp20B.f1=r;_tmp20B.f2=obj;
_tmp20B;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple12*,int),struct _tuple12*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp20A);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp20C=p;struct Cyc_Tcpat_Con_s*
_tmp20D;struct Cyc_List_List*_tmp20E;_LL126: if((int)_tmp20C != 0)goto _LL128;_LL127:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL128: if(_tmp20C <= (void*)1?1:*((int*)_tmp20C)!= 0)goto _LL125;_tmp20D=((
struct Cyc_Tcpat_Con_struct*)_tmp20C)->f1;_tmp20E=((struct Cyc_Tcpat_Con_struct*)
_tmp20C)->f2;_LL129: {void*_tmp20F=Cyc_Tcpat_static_match(_tmp20D,dsc);_LL12B:
if((int)_tmp20F != 0)goto _LL12D;_LL12C: {struct Cyc_List_List*_tmp210=({struct Cyc_List_List*
_tmp215=_region_malloc(r,sizeof(*_tmp215));_tmp215->hd=({struct _tuple11*_tmp216=
_region_malloc(r,sizeof(*_tmp216));_tmp216->f1=_tmp20D;_tmp216->f2=0;_tmp216;});
_tmp215->tl=ctx;_tmp215;});struct _tuple10*_tmp211=({struct _tuple10*_tmp214=
_region_malloc(r,sizeof(*_tmp214));_tmp214->f1=_tmp20E;_tmp214->f2=Cyc_Tcpat_getoargs(
r,_tmp20D,obj);_tmp214->f3=Cyc_Tcpat_getdargs(r,_tmp20D,dsc);_tmp214;});struct
Cyc_List_List*_tmp212=({struct Cyc_List_List*_tmp213=_region_malloc(r,sizeof(*
_tmp213));_tmp213->hd=_tmp211;_tmp213->tl=work;_tmp213;});return Cyc_Tcpat_and_match(
r,_tmp210,_tmp212,right_hand_side,rules);}_LL12D: if((int)_tmp20F != 1)goto _LL12F;
_LL12E: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL12F: if((int)_tmp20F != 2)goto _LL12A;_LL130: {struct Cyc_List_List*_tmp217=({
struct Cyc_List_List*_tmp220=_region_malloc(r,sizeof(*_tmp220));_tmp220->hd=({
struct _tuple11*_tmp221=_region_malloc(r,sizeof(*_tmp221));_tmp221->f1=_tmp20D;
_tmp221->f2=0;_tmp221;});_tmp220->tl=ctx;_tmp220;});struct _tuple10*_tmp218=({
struct _tuple10*_tmp21F=_region_malloc(r,sizeof(*_tmp21F));_tmp21F->f1=_tmp20E;
_tmp21F->f2=Cyc_Tcpat_getoargs(r,_tmp20D,obj);_tmp21F->f3=Cyc_Tcpat_getdargs(r,
_tmp20D,dsc);_tmp21F;});struct Cyc_List_List*_tmp219=({struct Cyc_List_List*
_tmp21E=_region_malloc(r,sizeof(*_tmp21E));_tmp21E->hd=_tmp218;_tmp21E->tl=work;
_tmp21E;});void*_tmp21A=Cyc_Tcpat_and_match(r,_tmp217,_tmp219,right_hand_side,
rules);void*_tmp21B=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp20D),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp21C=
_region_malloc(r,sizeof(*_tmp21C));_tmp21C[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp21D;_tmp21D.tag=1;_tmp21D.f1=obj;_tmp21D.f2=_tmp20D;_tmp21D.f3=(void*)
_tmp21A;_tmp21D.f4=(void*)_tmp21B;_tmp21D;});_tmp21C;});}_LL12A:;}_LL125:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp222=d;void*_tmp223;void*
_tmp224;void*_tmp225;_LL132: if((int)_tmp222 != 0)goto _LL134;_LL133: not_exhaust(
env1);goto _LL131;_LL134: if(_tmp222 <= (void*)1?1:*((int*)_tmp222)!= 0)goto _LL136;
_tmp223=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp222)->f1;_LL135: rhs_appears(
env2,_tmp223);goto _LL131;_LL136: if(_tmp222 <= (void*)1?1:*((int*)_tmp222)!= 1)
goto _LL131;_tmp224=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp222)->f3;_tmp225=(
void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp222)->f4;_LL137: Cyc_Tcpat_check_exhaust_overlap(
_tmp224,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp225,not_exhaust,env1,rhs_appears,env2);goto _LL131;_LL131:;}struct _tuple13{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple14{void*f1;struct _tuple13*f2;}
;struct _tuple15{struct _RegionHandle*f1;int*f2;};static struct _tuple14*Cyc_Tcpat_get_match(
struct _tuple15*env,struct Cyc_Absyn_Switch_clause*swc){struct _tuple15 _tmp227;
struct _RegionHandle*_tmp228;int*_tmp229;struct _tuple15*_tmp226=env;_tmp227=*
_tmp226;_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;{void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp22F=_region_malloc(_tmp228,
sizeof(*_tmp22F));_tmp22F->f1=0;_tmp22F->f2=(swc->pattern)->loc;_tmp22F;});void*
sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A->hd=(void*)sp0;_tmp22A->tl=({struct
Cyc_List_List*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp229);_tmp22B->tl=0;_tmp22B;});_tmp22A;}));*_tmp229=*_tmp229 + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->hd=(void*)sp0;
_tmp22C->tl=({struct Cyc_List_List*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->hd=(
void*)((void*)0);_tmp22D->tl=0;_tmp22D;});_tmp22C;}));}return({struct _tuple14*
_tmp22E=_region_malloc(_tmp228,sizeof(*_tmp22E));_tmp22E->f1=sp;_tmp22E->f2=rhs;
_tmp22E;});}}static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({
void*_tmp230[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp231="patterns may not be exhaustive";
_tag_arr(_tmp231,sizeof(char),_get_zero_arr_size(_tmp231,31));}),_tag_arr(
_tmp230,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple13*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp232=_new_region("r");struct
_RegionHandle*r=& _tmp232;_push_region(r);{int _tmp233=0;struct _tuple15 _tmp234=({
struct _tuple15 _tmp23F;_tmp23F.f1=r;_tmp23F.f2=& _tmp233;_tmp23F;});struct Cyc_List_List*
_tmp235=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(
struct _tuple15*,struct Cyc_Absyn_Switch_clause*),struct _tuple15*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp234,swcs);void*_tmp236=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp235);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct
Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp236,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp235 != 0;
_tmp235=_tmp235->tl){struct _tuple14 _tmp238;struct _tuple13*_tmp239;struct _tuple13
_tmp23A;int _tmp23B;struct Cyc_Position_Segment*_tmp23C;struct _tuple14*_tmp237=(
struct _tuple14*)_tmp235->hd;_tmp238=*_tmp237;_tmp239=_tmp238.f2;_tmp23A=*_tmp239;
_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;if(!_tmp23B)({void*_tmp23D[0]={};Cyc_Tcutil_terr(
_tmp23C,({const char*_tmp23E="redundant pattern";_tag_arr(_tmp23E,sizeof(char),
_get_zero_arr_size(_tmp23E,18));}),_tag_arr(_tmp23D,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple16{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple16*pr){(*pr).f2=0;({void*_tmp240[0]={};
Cyc_Tcutil_warn((*pr).f1,({const char*_tmp241="pattern not exhaustive";_tag_arr(
_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,23));}),_tag_arr(_tmp240,sizeof(
void*),0));});}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct _tuple17{
void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp242=_new_region("r");struct
_RegionHandle*r=& _tmp242;_push_region(r);{struct Cyc_List_List*_tmp243=({struct
Cyc_List_List*_tmp248=_region_malloc(r,sizeof(*_tmp248));_tmp248->hd=({struct
_tuple17*_tmp249=_region_malloc(r,sizeof(*_tmp249));_tmp249->f1=Cyc_Tcpat_compile_pat(
p);_tmp249->f2=0;_tmp249;});_tmp248->tl=0;_tmp248;});void*_tmp244=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp243);struct _tuple16 _tmp245=({struct _tuple16 _tmp247;_tmp247.f1=loc;_tmp247.f2=
1;_tmp247;});((void(*)(void*d,void(*not_exhaust)(struct _tuple16*),struct _tuple16*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp244,Cyc_Tcpat_not_exhaust_warn,& _tmp245,Cyc_Tcpat_dummy_fn,0);{int _tmp246=
_tmp245.f2;_npop_handler(0);return _tmp246;}};_pop_region(r);}static struct
_tuple14*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp24B=_cycalloc(sizeof(*
_tmp24B));_tmp24B->f1=0;_tmp24B->f2=(swc->pattern)->loc;_tmp24B;});return({
struct _tuple14*_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A->f1=sp0;_tmp24A->f2=
rhs;_tmp24A;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp24C=((struct Cyc_List_List*(*)(struct
_tuple14*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp24D=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp24C);((
void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp24D,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp24C != 
0;_tmp24C=_tmp24C->tl){struct _tuple14 _tmp24F;struct _tuple13*_tmp250;struct
_tuple13 _tmp251;int _tmp252;struct Cyc_Position_Segment*_tmp253;struct _tuple14*
_tmp24E=(struct _tuple14*)_tmp24C->hd;_tmp24F=*_tmp24E;_tmp250=_tmp24F.f2;_tmp251=*
_tmp250;_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;if(!_tmp252)({void*_tmp254[0]={};
Cyc_Tcutil_terr(_tmp253,({const char*_tmp255="redundant pattern";_tag_arr(_tmp255,
sizeof(char),_get_zero_arr_size(_tmp255,18));}),_tag_arr(_tmp254,sizeof(void*),0));});}}
