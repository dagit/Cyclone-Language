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
#else
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
static inline unsigned char *
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
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(
void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);
};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
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
Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Reference_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _tagged_arr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
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
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
_tmp0)!= 12)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{
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
_tmp18;_tmp18.tag=6;_tmp18.f1=_tmpF;_tmp18.f2=_tmp10;_tmp18.f3=0;_tmp18;});
_tmp17;})));_npop_handler(0);return;_LLE: if(*((int*)_tmpE)!= 3)goto _LL10;_tmp11=((
struct Cyc_Tcenv_EnumRes_struct*)_tmpE)->f1;_tmp12=((struct Cyc_Tcenv_EnumRes_struct*)
_tmpE)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1A;_tmp1A.tag=10;_tmp1A.f1=_tmp11;_tmp1A.f2=_tmp12;_tmp1A;});_tmp19;})));
_npop_handler(0);return;_LL10: if(*((int*)_tmpE)!= 4)goto _LL12;_tmp13=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpE)->f1;_tmp14=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmpE)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp1C;_tmp1C.tag=11;_tmp1C.f1=(void*)_tmp13;_tmp1C.f2=_tmp14;_tmp1C;});_tmp1B;})));
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
13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((
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
_tmp31[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp32;_tmp32.tag=5;_tmp32.f1=({struct
Cyc_Absyn_AggrInfo _tmp33;_tmp33.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp35;_tmp35.tag=1;_tmp35.f1=({struct Cyc_Absyn_Aggrdecl**_tmp36=_cycalloc(
sizeof(*_tmp36));_tmp36[0]=_tmp29;_tmp36;});_tmp35;});_tmp34;}));_tmp33.targs=0;
_tmp33;});_tmp32.f2=0;_tmp32.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp2E);_tmp32;});_tmp31;})));_npop_handler(0);return;}_LL23:
if(*((int*)_tmp28)!= 2)goto _LL25;_tmp2A=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp28)->f1;_tmp2B=((struct Cyc_Tcenv_TunionRes_struct*)_tmp28)->f2;_LL24:(void*)(
p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp37=_cycalloc(sizeof(*
_tmp37));_tmp37[0]=({struct Cyc_Absyn_Tunion_p_struct _tmp38;_tmp38.tag=6;_tmp38.f1=
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
sizeof(void*),1));}});return;_LL5: if(_tmp0 <= (void*)2?1:*((int*)_tmp0)!= 5)goto
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
_tmp4C;_tmp4C.tag=5;_tmp4C.f1=({struct Cyc_Absyn_AggrInfo _tmp4D;_tmp4D.aggr_info=(
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
struct Cyc_Tcpat_TcPatResult _tmp114;_tmp114.tvars_and_bounds_opt=0;_tmp114.patvars=
0;_tmp114;});{void*_tmp62=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp63;struct Cyc_Absyn_Vardecl*
_tmp64;struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_Vardecl*_tmp66;void*_tmp67;
void*_tmp68;void*_tmp69;struct Cyc_Absyn_Enumdecl*_tmp6A;void*_tmp6B;struct Cyc_Absyn_Pat*
_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_Absyn_AggrInfo _tmp6E;void*_tmp6F;
struct Cyc_Absyn_Aggrdecl**_tmp70;struct Cyc_Absyn_Aggrdecl*_tmp71;struct Cyc_List_List*
_tmp72;struct Cyc_List_List**_tmp73;struct Cyc_List_List*_tmp74;struct Cyc_List_List*
_tmp75;struct Cyc_Absyn_Tuniondecl*_tmp76;struct Cyc_Absyn_Tunionfield*_tmp77;
struct Cyc_List_List*_tmp78;struct Cyc_Absyn_AggrInfo _tmp79;void*_tmp7A;_LL3B: if((
int)_tmp62 != 0)goto _LL3D;_LL3C: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3A;_LL3D: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 0)goto _LL3F;
_tmp63=((struct Cyc_Absyn_Var_p_struct*)_tmp62)->f1;_LL3E: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(_tmp63,& res.patvars,t);
goto _LL3A;_LL3F: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 1)goto _LL41;_tmp64=((
struct Cyc_Absyn_Reference_p_struct*)_tmp62)->f1;_LL40: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(rgn_opt == 0){({void*_tmp7B[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp7C="* pattern would point into an unknown/unallowed region";_tag_arr(_tmp7C,
sizeof(char),_get_zero_arr_size(_tmp7C,55));}),_tag_arr(_tmp7B,sizeof(void*),0));});
goto _LL3A;}Cyc_Tcpat_set_vd(_tmp64,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7E;_tmp7E.tag=4;_tmp7E.f1=({struct Cyc_Absyn_PtrInfo _tmp7F;_tmp7F.elt_typ=(
void*)t;_tmp7F.elt_tq=Cyc_Absyn_empty_tqual();_tmp7F.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp80;_tmp80.rgn=(void*)*rgn_opt;_tmp80.nullable=Cyc_Absyn_false_conref;_tmp80.bounds=
Cyc_Absyn_empty_conref();_tmp80.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp80;});_tmp7F;});_tmp7E;});_tmp7D;}));goto _LL3A;_LL41: if(_tmp62 <= (void*)2?1:*((
int*)_tmp62)!= 2)goto _LL43;_tmp65=((struct Cyc_Absyn_TagInt_p_struct*)_tmp62)->f1;
_tmp66=((struct Cyc_Absyn_TagInt_p_struct*)_tmp62)->f2;_LL42: Cyc_Tcpat_set_vd(
_tmp66,& res.patvars,(void*)_tmp66->type);Cyc_Tcenv_add_type_vars(p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp81[1];_tmp81[0]=_tmp65;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp81,sizeof(struct Cyc_Absyn_Tvar*),1));}));
if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*_tmp82=
_cycalloc(sizeof(*_tmp82));_tmp82->f1=0;_tmp82->f2=0;_tmp82;});(*res.tvars_and_bounds_opt).f1=({
struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=_tmp65;_tmp83->tl=(*
res.tvars_and_bounds_opt).f1;_tmp83;});t=Cyc_Absyn_uint_typ;goto _LL3A;_LL43: if(
_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 7)goto _LL45;_tmp67=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp62)->f1;if((int)_tmp67 != 1)goto _LL45;_LL44: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3A;_LL45: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 7)goto _LL47;_tmp68=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp62)->f1;if((int)_tmp68 != 2)goto _LL47;
_LL46: goto _LL48;_LL47: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 7)goto _LL49;
_tmp69=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp62)->f1;if((int)_tmp69 != 0)
goto _LL49;_LL48: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3A;_LL49:
if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 8)goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_char_typ);goto _LL3A;_LL4B: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)
!= 9)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL3A;
_LL4D: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 10)goto _LL4F;_tmp6A=((struct Cyc_Absyn_Enum_p_struct*)
_tmp62)->f1;_LL4E: t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_EnumType_struct
_tmp85;_tmp85.tag=12;_tmp85.f1=_tmp6A->name;_tmp85.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp6A;_tmp85;});_tmp84;}));goto _LL3A;_LL4F: if(_tmp62 <= (void*)2?1:*((int*)
_tmp62)!= 11)goto _LL51;_tmp6B=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp62)->f1;
_LL50: t=Cyc_Tcpat_num_type(topt,_tmp6B);goto _LL3A;_LL51: if((int)_tmp62 != 1)goto
_LL53;_LL52: if(topt != 0){void*_tmp86=Cyc_Tcutil_compress(*topt);_LL62: if(_tmp86
<= (void*)3?1:*((int*)_tmp86)!= 4)goto _LL64;_LL63: t=*topt;goto tcpat_end;_LL64:;
_LL65: goto _LL61;_LL61:;}{struct Cyc_List_List*_tmp87=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp88=_cycalloc(sizeof(*
_tmp88));_tmp88[0]=({struct Cyc_Absyn_PointerType_struct _tmp89;_tmp89.tag=4;
_tmp89.f1=({struct Cyc_Absyn_PtrInfo _tmp8A;_tmp8A.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp8D=_cycalloc(sizeof(*
_tmp8D));_tmp8D->v=_tmp87;_tmp8D;}));_tmp8A.elt_tq=Cyc_Absyn_empty_tqual();
_tmp8A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp8B;_tmp8B.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp8C=_cycalloc(sizeof(*
_tmp8C));_tmp8C->v=_tmp87;_tmp8C;}));_tmp8B.nullable=Cyc_Absyn_true_conref;
_tmp8B.bounds=Cyc_Absyn_empty_conref();_tmp8B.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp8B;});_tmp8A;});_tmp89;});_tmp88;});goto _LL3A;}
_LL53: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 4)goto _LL55;_tmp6C=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp62)->f1;_LL54: {void*inner_typ=(void*)0;void**_tmp8E=0;if(topt != 0){void*
_tmp8F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp90;void*_tmp91;
_LL67: if(_tmp8F <= (void*)3?1:*((int*)_tmp8F)!= 4)goto _LL69;_tmp90=((struct Cyc_Absyn_PointerType_struct*)
_tmp8F)->f1;_tmp91=(void*)_tmp90.elt_typ;_LL68: inner_typ=_tmp91;_tmp8E=(void**)&
inner_typ;goto _LL66;_LL69:;_LL6A: goto _LL66;_LL66:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpA6=_cycalloc(sizeof(*
_tmpA6));_tmpA6->v=Cyc_Tcenv_lookup_type_vars(te);_tmpA6;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp6C,_tmp8E,(void**)& ptr_rgn));if(_tmp8E == 0){void*
_tmp92=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6C->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmp93;void*_tmp94;struct Cyc_Absyn_Tuniondecl*
_tmp95;struct Cyc_Absyn_Tunionfield*_tmp96;struct Cyc_List_List*_tmp97;_LL6C: if(
_tmp92 <= (void*)3?1:*((int*)_tmp92)!= 3)goto _LL6E;_tmp93=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp92)->f1;_tmp94=(void*)_tmp93.field_info;if(*((int*)_tmp94)!= 1)goto _LL6E;
_tmp95=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp94)->f1;_tmp96=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp94)->f2;_tmp97=_tmp93.targs;_LL6D: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Absyn_TunionType_struct
_tmp99;_tmp99.tag=2;_tmp99.f1=({struct Cyc_Absyn_TunionInfo _tmp9A;_tmp9A.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp9B=_cycalloc(sizeof(*
_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp9C;_tmp9C.tag=1;
_tmp9C.f1=({struct Cyc_Absyn_Tuniondecl**_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[
0]=_tmp95;_tmp9D;});_tmp9C;});_tmp9B;}));_tmp9A.targs=_tmp97;_tmp9A.rgn=(void*)
ptr_rgn;_tmp9A;});_tmp99;});_tmp98;});goto _LL6B;_LL6E:;_LL6F: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_Absyn_PointerType_struct _tmp9F;_tmp9F.tag=4;_tmp9F.f1=({struct Cyc_Absyn_PtrInfo
_tmpA0;_tmpA0.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6C->topt))->v);
_tmpA0.elt_tq=Cyc_Absyn_empty_tqual();_tmpA0.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpA1;_tmpA1.rgn=(void*)ptr_rgn;_tmpA1.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA1.bounds=Cyc_Absyn_empty_conref();_tmpA1.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpA1;});_tmpA0;});
_tmp9F;});_tmp9E;});goto _LL6B;_LL6B:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_PointerType_struct
_tmpA3;_tmpA3.tag=4;_tmpA3.f1=({struct Cyc_Absyn_PtrInfo _tmpA4;_tmpA4.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6C->topt))->v);_tmpA4.elt_tq=
Cyc_Absyn_empty_tqual();_tmpA4.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA5;_tmpA5.rgn=(
void*)ptr_rgn;_tmpA5.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpA5.bounds=Cyc_Absyn_empty_conref();_tmpA5.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA5;});_tmpA4;});_tmpA3;});_tmpA2;});}goto _LL3A;}}
_LL55: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 3)goto _LL57;_tmp6D=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp62)->f1;_LL56: {struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;
if(topt != 0){void*_tmpA7=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpA8;
_LL71: if(_tmpA7 <= (void*)3?1:*((int*)_tmpA7)!= 9)goto _LL73;_tmpA8=((struct Cyc_Absyn_TupleType_struct*)
_tmpA7)->f1;_LL72: topt_ts=_tmpA8;goto _LL70;_LL73:;_LL74: goto _LL70;_LL70:;}for(0;
_tmp6D != 0;_tmp6D=_tmp6D->tl){void**_tmpA9=0;if(topt_ts != 0){_tmpA9=(void**)&(*((
struct _tuple8*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp6D->hd,_tmpA9,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=({
struct _tuple8*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=Cyc_Absyn_empty_tqual();
_tmpAB->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp6D->hd)->topt))->v;_tmpAB;});_tmpAA->tl=pat_ts;_tmpAA;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct
Cyc_Absyn_TupleType_struct _tmpAD;_tmpAD.tag=9;_tmpAD.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpAD;});_tmpAC;});goto _LL3A;}
_LL57: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 5)goto _LL59;_tmp6E=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp62)->f1;_tmp6F=(void*)_tmp6E.aggr_info;if(*((int*)_tmp6F)!= 1)goto _LL59;
_tmp70=((struct Cyc_Absyn_KnownAggr_struct*)_tmp6F)->f1;_tmp71=*_tmp70;_tmp72=
_tmp6E.targs;_tmp73=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)
_tmp62)->f1).targs;_tmp74=((struct Cyc_Absyn_Aggr_p_struct*)_tmp62)->f2;_tmp75=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp62)->f3;_LL58: if(_tmp71->impl == 0){({void*
_tmpAE[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpAF="can't destructure an abstract struct";
_tag_arr(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,37));}),_tag_arr(_tmpAE,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpB0=_cycalloc(
sizeof(*_tmpB0));_tmpB0->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB0;}));goto _LL3A;}{
struct _RegionHandle _tmpB1=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB1;
_push_region(rgn);{struct Cyc_List_List*_tmpB2=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpB3=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp71->impl))->exist_vars;{struct Cyc_List_List*t=_tmp74;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpB3))->hd;_tmpB3=
_tmpB3->tl;{void*_tmpB4=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpB5=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpB6=_tmpB5;void*_tmpB7;void*_tmpB8;
_LL76: if(*((int*)_tmpB6)!= 2)goto _LL78;_tmpB7=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpB6)->f2;_LL77: _tmpB8=_tmpB7;goto _LL79;_LL78: if(*((int*)_tmpB6)!= 0)goto _LL7A;
_tmpB8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpB6)->f1;_LL79: k2=_tmpB8;goto
_LL75;_LL7A:;_LL7B:({void*_tmpB9[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpBA="unconstrained existential type variable in struct";
_tag_arr(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,50));}),_tag_arr(_tmpB9,
sizeof(void*),0));});_LL75:;}{void*_tmpBB=_tmpB4;void*_tmpBC;struct Cyc_Core_Opt*
_tmpBD;struct Cyc_Core_Opt**_tmpBE;void*_tmpBF;struct Cyc_Core_Opt*_tmpC0;struct
Cyc_Core_Opt**_tmpC1;_LL7D: if(*((int*)_tmpBB)!= 0)goto _LL7F;_tmpBC=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpBB)->f1;_LL7E: if(!Cyc_Tcutil_kind_leq(k2,
_tmpBC))error=1;goto _LL7C;_LL7F: if(*((int*)_tmpBB)!= 2)goto _LL81;_tmpBD=((struct
Cyc_Absyn_Less_kb_struct*)_tmpBB)->f1;_tmpBE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpBB)->f1;_tmpBF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpBB)->f2;_LL80:
_tmpC1=_tmpBE;goto _LL82;_LL81: if(*((int*)_tmpBB)!= 1)goto _LL7C;_tmpC0=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpBB)->f1;_tmpC1=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpBB)->f1;_LL82:*_tmpC1=({struct Cyc_Core_Opt*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->v=(void*)_tmpB5;_tmpC2;});goto _LL7C;
_LL7C:;}if(error)({struct Cyc_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(k2));{struct Cyc_String_pa_struct
_tmpC6;_tmpC6.tag=0;_tmpC6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
_tmpB4));{struct Cyc_String_pa_struct _tmpC5;_tmpC5.tag=0;_tmpC5.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmpC3[3]={& _tmpC5,& _tmpC6,&
_tmpC7};Cyc_Tcutil_terr(p->loc,({const char*_tmpC4="type variable %s has kind %s but must have at least kind %s";
_tag_arr(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,60));}),_tag_arr(_tmpC3,
sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_VarType_struct
_tmpCC;_tmpCC.tag=1;_tmpCC.f1=tv;_tmpCC;});_tmpCB;});_tmpB2=({struct Cyc_List_List*
_tmpC8=_region_malloc(rgn,sizeof(*_tmpC8));_tmpC8->hd=(void*)((void*)vartype);
_tmpC8->tl=_tmpB2;_tmpC8;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->hd=({struct _tuple0*_tmpCA=_cycalloc(
sizeof(*_tmpCA));_tmpCA->f1=Cyc_Absyn_empty_effect;_tmpCA->f2=vartype;_tmpCA;});
_tmpC9->tl=outlives_constraints;_tmpC9;});}}}}_tmpB2=Cyc_List_imp_rev(_tmpB2);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp74);struct Cyc_List_List*
_tmpCD=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpCE=({struct _tuple4 _tmpE9;
_tmpE9.f1=_tmpCD;_tmpE9.f2=rgn;_tmpE9;});struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpCE,_tmp71->tvs);struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp71->impl))->exist_vars,
_tmpB2);struct Cyc_List_List*_tmpD1=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpCF);struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpD0);struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpCF,_tmpD0);
if(_tmp74 != 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp71->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=0;_tmpD4->f2=0;_tmpD4;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp74);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpD5=0;{struct Cyc_List_List*_tmpD6=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp71->impl))->rgn_po;for(0;_tmpD6 != 
0;_tmpD6=_tmpD6->tl){_tmpD5=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*
_tmpD7));_tmpD7->hd=({struct _tuple0*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpD3,(*((struct _tuple0*)_tmpD6->hd)).f1);_tmpD8->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpD3,(*((struct _tuple0*)_tmpD6->hd)).f2);_tmpD8;});
_tmpD7->tl=_tmpD5;_tmpD7;});}}_tmpD5=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpD5);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpD5);}}*_tmp73=_tmpD1;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpD9=
_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_AggrType_struct _tmpDA;
_tmpDA.tag=10;_tmpDA.f1=({struct Cyc_Absyn_AggrInfo _tmpDB;_tmpDB.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));
_tmpDC[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpDD;_tmpDD.tag=1;_tmpDD.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=_tmp71;
_tmpDE;});_tmpDD;});_tmpDC;}));_tmpDB.targs=*_tmp73;_tmpDB;});_tmpDA;});_tmpD9;});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmp75,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp71->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple9 _tmpE0;struct Cyc_Absyn_Aggrfield*_tmpE1;struct Cyc_Absyn_Pat*_tmpE2;struct
_tuple9*_tmpDF=(struct _tuple9*)fields->hd;_tmpE0=*_tmpDF;_tmpE1=_tmpE0.f1;_tmpE2=
_tmpE0.f2;{void*_tmpE3=Cyc_Tcutil_rsubstitute(rgn,_tmpD3,(void*)_tmpE1->type);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmpE2,(void**)& _tmpE3,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE2->topt))->v,
_tmpE3))({struct Cyc_String_pa_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE2->topt))->v));{struct Cyc_String_pa_struct _tmpE7;_tmpE7.tag=0;
_tmpE7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpE3));{
struct Cyc_String_pa_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmpE1->name);{void*_tmpE4[3]={& _tmpE6,& _tmpE7,& _tmpE8};Cyc_Tcutil_terr(
p->loc,({const char*_tmpE5="field %s of struct pattern expects type %s != %s";
_tag_arr(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,49));}),_tag_arr(_tmpE4,
sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}goto _LL3A;_LL59: if(_tmp62 <= (
void*)2?1:*((int*)_tmp62)!= 6)goto _LL5B;_tmp76=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp62)->f1;_tmp77=((struct Cyc_Absyn_Tunion_p_struct*)_tmp62)->f2;_tmp78=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp62)->f3;_LL5A:{struct _RegionHandle _tmpEA=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpEA;_push_region(rgn);{struct Cyc_List_List*
tqts=_tmp77->typs;struct Cyc_List_List*_tmpEB=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple4 _tmpEC=({struct _tuple4 _tmp111;_tmp111.f1=_tmpEB;_tmp111.f2=rgn;
_tmp111;});struct Cyc_List_List*_tmpED=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpEC,_tmp76->tvs);struct Cyc_List_List*_tmpEE=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpED);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_TunionType_struct
_tmpF0;_tmpF0.tag=2;_tmpF0.f1=({struct Cyc_Absyn_TunionInfo _tmpF1;_tmpF1.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpF3=_cycalloc(sizeof(*
_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpF4;_tmpF4.tag=1;
_tmpF4.f1=({struct Cyc_Absyn_Tuniondecl**_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[
0]=_tmp76;_tmpF5;});_tmpF4;});_tmpF3;}));_tmpF1.targs=_tmpEE;_tmpF1.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->v=_tmpEB;_tmpF2;}));_tmpF1;});_tmpF0;});
_tmpEF;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpF6=
_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpF7;_tmpF7.tag=3;_tmpF7.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpF8;_tmpF8.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpF9=_cycalloc(sizeof(*
_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpFA;_tmpFA.tag=1;
_tmpFA.f1=_tmp76;_tmpFA.f2=_tmp77;_tmpFA;});_tmpF9;}));_tmpF8.targs=_tmpEE;
_tmpF8;});_tmpF7;});_tmpF6;});}if(topt != 0?tqts == 0: 0){void*_tmpFB=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmpFC;struct Cyc_List_List*_tmpFD;_LL84: if(
_tmpFB <= (void*)3?1:*((int*)_tmpFB)!= 3)goto _LL86;_LL85: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpFF;_tmpFF.tag=3;_tmpFF.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp100;_tmp100.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp101=_cycalloc(
sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmp102;
_tmp102.tag=1;_tmp102.f1=_tmp76;_tmp102.f2=_tmp77;_tmp102;});_tmp101;}));_tmp100.targs=
_tmpEE;_tmp100;});_tmpFF;});_tmpFE;});goto _LL83;_LL86: if(_tmpFB <= (void*)3?1:*((
int*)_tmpFB)!= 2)goto _LL88;_tmpFC=((struct Cyc_Absyn_TunionType_struct*)_tmpFB)->f1;
_tmpFD=_tmpFC.targs;_LL87: {struct Cyc_List_List*_tmp103=_tmpEE;for(0;_tmp103 != 0?
_tmpFD != 0: 0;(_tmp103=_tmp103->tl,_tmpFD=_tmpFD->tl)){Cyc_Tcutil_unify((void*)
_tmp103->hd,(void*)_tmpFD->hd);}goto _LL83;}_LL88:;_LL89: goto _LL83;_LL83:;}for(0;
_tmp78 != 0?tqts != 0: 0;(_tmp78=_tmp78->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp104=(struct Cyc_Absyn_Pat*)_tmp78->hd;void*_tmp105=Cyc_Tcutil_rsubstitute(rgn,
_tmpED,(*((struct _tuple8*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp104,(void**)& _tmp105,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp104->topt))->v,_tmp105))({struct Cyc_String_pa_struct _tmp10A;
_tmp10A.tag=0;_tmp10A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp104->topt))->v));{struct Cyc_String_pa_struct
_tmp109;_tmp109.tag=0;_tmp109.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp105));{struct Cyc_String_pa_struct _tmp108;_tmp108.tag=0;_tmp108.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp76->name));{void*
_tmp106[3]={& _tmp108,& _tmp109,& _tmp10A};Cyc_Tcutil_terr(_tmp104->loc,({const char*
_tmp107="%s expects argument type %s, not %s";_tag_arr(_tmp107,sizeof(char),
_get_zero_arr_size(_tmp107,36));}),_tag_arr(_tmp106,sizeof(void*),3));}}}});}if(
_tmp78 != 0)({struct Cyc_String_pa_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp76->name));{void*
_tmp10B[1]={& _tmp10D};Cyc_Tcutil_terr(p->loc,({const char*_tmp10C="too many arguments for tunion constructor %s";
_tag_arr(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,45));}),_tag_arr(
_tmp10B,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct _tmp110;
_tmp110.tag=0;_tmp110.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp76->name));{void*_tmp10E[1]={& _tmp110};Cyc_Tcutil_terr(p->loc,({const char*
_tmp10F="too few arguments for tunion constructor %s";_tag_arr(_tmp10F,sizeof(
char),_get_zero_arr_size(_tmp10F,44));}),_tag_arr(_tmp10E,sizeof(void*),1));}});};
_pop_region(rgn);}goto _LL3A;_LL5B: if(_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 5)
goto _LL5D;_tmp79=((struct Cyc_Absyn_Aggr_p_struct*)_tmp62)->f1;_tmp7A=(void*)
_tmp79.aggr_info;if(*((int*)_tmp7A)!= 0)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if(
_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 12)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(
_tmp62 <= (void*)2?1:*((int*)_tmp62)!= 13)goto _LL3A;_LL60: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp112;}));goto _LL3A;_LL3A:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113->v=(void*)t;_tmp113;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp115=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp116=_new_region("r");struct _RegionHandle*r=& _tmp116;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp115.patvars),p->loc,({const char*
_tmp117="pattern contains a repeated variable";_tag_arr(_tmp117,sizeof(char),
_get_zero_arr_size(_tmp117,37));}));;_pop_region(r);}return _tmp115;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp118=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp11B;
struct Cyc_List_List*_tmp11C;_LL8B: if(_tmp118 <= (void*)2?1:*((int*)_tmp118)!= 4)
goto _LL8D;_tmp119=((struct Cyc_Absyn_Pointer_p_struct*)_tmp118)->f1;_LL8C: Cyc_Tcpat_check_pat_regions(
te,_tmp119);{void*_tmp11D=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp11E;struct Cyc_Absyn_PtrAtts _tmp11F;void*_tmp120;
struct Cyc_Absyn_TunionInfo _tmp121;void*_tmp122;_LL96: if(_tmp11D <= (void*)3?1:*((
int*)_tmp11D)!= 4)goto _LL98;_tmp11E=((struct Cyc_Absyn_PointerType_struct*)
_tmp11D)->f1;_tmp11F=_tmp11E.ptr_atts;_tmp120=(void*)_tmp11F.rgn;_LL97: _tmp122=
_tmp120;goto _LL99;_LL98: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 2)goto _LL9A;
_tmp121=((struct Cyc_Absyn_TunionType_struct*)_tmp11D)->f1;_tmp122=(void*)_tmp121.rgn;
_LL99: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp122);return;_LL9A:;_LL9B:({
void*_tmp123[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp124="check_pat_regions: bad pointer type";_tag_arr(_tmp124,sizeof(
char),_get_zero_arr_size(_tmp124,36));}),_tag_arr(_tmp123,sizeof(void*),0));});
_LL95:;}_LL8D: if(_tmp118 <= (void*)2?1:*((int*)_tmp118)!= 6)goto _LL8F;_tmp11A=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp118)->f3;_LL8E: for(0;_tmp11A != 0;_tmp11A=
_tmp11A->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp11A->hd);}{
void*_tmp125=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo
_tmp126;void*_tmp127;_LL9D: if(_tmp125 <= (void*)3?1:*((int*)_tmp125)!= 2)goto
_LL9F;_tmp126=((struct Cyc_Absyn_TunionType_struct*)_tmp125)->f1;_tmp127=(void*)
_tmp126.rgn;_LL9E: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp127);return;_LL9F:
if(_tmp125 <= (void*)3?1:*((int*)_tmp125)!= 3)goto _LLA1;_LLA0: return;_LLA1:;_LLA2:({
void*_tmp128[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp129="check_pat_regions: bad tunion type";_tag_arr(_tmp129,sizeof(
char),_get_zero_arr_size(_tmp129,35));}),_tag_arr(_tmp128,sizeof(void*),0));});
_LL9C:;}_LL8F: if(_tmp118 <= (void*)2?1:*((int*)_tmp118)!= 5)goto _LL91;_tmp11B=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp118)->f3;_LL90: for(0;_tmp11B != 0;_tmp11B=
_tmp11B->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct _tuple7*)_tmp11B->hd)).f2);}
return;_LL91: if(_tmp118 <= (void*)2?1:*((int*)_tmp118)!= 3)goto _LL93;_tmp11C=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp118)->f1;_LL92: for(0;_tmp11C != 0;_tmp11C=
_tmp11C->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp11C->hd);}
return;_LL93:;_LL94: return;_LL8A:;}struct Cyc_Tcpat_Name_v_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct Cyc_Tcpat_Con_s{
void*name;int arity;struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{int tag;
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){void*_tmp12A=(void*)c1->name;
struct _tagged_arr _tmp12B;int _tmp12C;_LLA4: if(*((int*)_tmp12A)!= 0)goto _LLA6;
_tmp12B=((struct Cyc_Tcpat_Name_v_struct*)_tmp12A)->f1;_LLA5: {void*_tmp12D=(void*)
c2->name;struct _tagged_arr _tmp12E;_LLA9: if(*((int*)_tmp12D)!= 0)goto _LLAB;
_tmp12E=((struct Cyc_Tcpat_Name_v_struct*)_tmp12D)->f1;_LLAA: return Cyc_strcmp((
struct _tagged_arr)_tmp12B,(struct _tagged_arr)_tmp12E);_LLAB: if(*((int*)_tmp12D)
!= 1)goto _LLA8;_LLAC: return - 1;_LLA8:;}_LLA6: if(*((int*)_tmp12A)!= 1)goto _LLA3;
_tmp12C=((struct Cyc_Tcpat_Int_v_struct*)_tmp12A)->f1;_LLA7: {void*_tmp12F=(void*)
c2->name;int _tmp130;_LLAE: if(*((int*)_tmp12F)!= 0)goto _LLB0;_LLAF: return 1;_LLB0:
if(*((int*)_tmp12F)!= 1)goto _LLAD;_tmp130=((struct Cyc_Tcpat_Int_v_struct*)
_tmp12F)->f1;_LLB1: return _tmp12C - _tmp130;_LLAD:;}_LLA3:;}static struct Cyc_Set_Set*
Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))
Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(void*)2};static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;static char _tmp132[
5]="NULL";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={0,{
_tmp132,_tmp132,_tmp132 + 5}};static char _tmp134[4]="ptr";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_ptr_name_value={0,{_tmp134,_tmp134,_tmp134 + 4}};static char _tmp136[6]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={0,{_tmp136,
_tmp136,_tmp136 + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(void*)((
void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp138=_cycalloc_atomic(sizeof(*
_tmp138));_tmp138[0]=({struct Cyc_Tcpat_Int_v_struct _tmp139;_tmp139.tag=1;_tmp139.f1=
i;_tmp139;});_tmp138;}));_tmp137->arity=0;_tmp137->span=0;_tmp137;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=f;_tmp13C;});_tmp13B;}));_tmp13A->arity=0;
_tmp13A->span=0;_tmp13A;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c){
return({struct Cyc_Tcpat_Con_s*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp13F=_cycalloc_atomic(sizeof(*
_tmp13F));_tmp13F[0]=({struct Cyc_Tcpat_Int_v_struct _tmp140;_tmp140.tag=1;_tmp140.f1=(
int)c;_tmp140;});_tmp13F;}));_tmp13D->arity=0;_tmp13D->span=({struct Cyc_Core_Opt*
_tmp13E=_cycalloc_atomic(sizeof(*_tmp13E));_tmp13E->v=(void*)256;_tmp13E;});
_tmp13D;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i){return({struct
Cyc_Tcpat_Con_s*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->name=(void*)((void*)&
Cyc_Tcpat_tuple_name_value);_tmp141->arity=i;_tmp141->span=Cyc_Tcpat_one_opt_ptr;
_tmp141;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Tcpat_Con_struct
_tmp143;_tmp143.tag=0;_tmp143.f1=Cyc_Tcpat_null_con;_tmp143.f2=0;_tmp143;});
_tmp142;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144[0]=({struct Cyc_Tcpat_Con_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=Cyc_Tcpat_int_con(i);_tmp145.f2=0;_tmp145;});
_tmp144;});}static void*Cyc_Tcpat_char_pat(char c){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({struct Cyc_Tcpat_Con_struct
_tmp147;_tmp147.tag=0;_tmp147.f1=Cyc_Tcpat_char_con(c);_tmp147.f2=0;_tmp147;});
_tmp146;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=({
struct Cyc_Tcpat_Con_struct _tmp149;_tmp149.tag=0;_tmp149.f1=Cyc_Tcpat_float_con(f);
_tmp149.f2=0;_tmp149;});_tmp148;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp14A=_cycalloc(sizeof(*_tmp14A));
_tmp14A[0]=({struct Cyc_Tcpat_Con_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=Cyc_Tcpat_null_ptr_con;
_tmp14B.f2=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(
void*)p;_tmp14C->tl=0;_tmp14C;});_tmp14B;});_tmp14A;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp14D=_cycalloc(sizeof(*
_tmp14D));_tmp14D[0]=({struct Cyc_Tcpat_Con_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=
Cyc_Tcpat_ptr_con;_tmp14E.f2=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*
_tmp14F));_tmp14F->hd=(void*)p;_tmp14F->tl=0;_tmp14F;});_tmp14E;});_tmp14D;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Tcpat_Con_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp151.f2=ss;_tmp151;});_tmp150;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp155=_cycalloc(sizeof(*_tmp155));
_tmp155[0]=({struct Cyc_Tcpat_Name_v_struct _tmp156;_tmp156.tag=0;_tmp156.f1=
con_name;_tmp156;});_tmp155;}));_tmp154->arity=Cyc_List_length(ps);_tmp154->span=
span;_tmp154;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp152=_cycalloc(
sizeof(*_tmp152));_tmp152[0]=({struct Cyc_Tcpat_Con_struct _tmp153;_tmp153.tag=0;
_tmp153.f1=c;_tmp153.f2=ps;_tmp153;});_tmp152;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp157=(void*)p->r;void*_tmp158;int _tmp159;
char _tmp15A;struct _tagged_arr _tmp15B;struct Cyc_Absyn_Pat*_tmp15C;struct Cyc_Absyn_Tuniondecl*
_tmp15D;struct Cyc_Absyn_Tunionfield*_tmp15E;struct Cyc_List_List*_tmp15F;struct
Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_AggrInfo
_tmp162;void*_tmp163;struct Cyc_Absyn_Aggrdecl**_tmp164;struct Cyc_Absyn_Aggrdecl*
_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Enumdecl*_tmp167;struct Cyc_Absyn_Enumfield*
_tmp168;void*_tmp169;struct Cyc_Absyn_Enumfield*_tmp16A;_LLB3: if((int)_tmp157 != 0)
goto _LLB5;_LLB4: goto _LLB6;_LLB5: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 0)
goto _LLB7;_LLB6: goto _LLB8;_LLB7: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 2)
goto _LLB9;_LLB8: s=(void*)0;goto _LLB2;_LLB9: if((int)_tmp157 != 1)goto _LLBB;_LLBA: s=
Cyc_Tcpat_null_pat();goto _LLB2;_LLBB: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 
7)goto _LLBD;_tmp158=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp157)->f1;_tmp159=((
struct Cyc_Absyn_Int_p_struct*)_tmp157)->f2;_LLBC: s=Cyc_Tcpat_int_pat(_tmp159);
goto _LLB2;_LLBD: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 8)goto _LLBF;_tmp15A=((
struct Cyc_Absyn_Char_p_struct*)_tmp157)->f1;_LLBE: s=Cyc_Tcpat_char_pat(_tmp15A);
goto _LLB2;_LLBF: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 9)goto _LLC1;_tmp15B=((
struct Cyc_Absyn_Float_p_struct*)_tmp157)->f1;_LLC0: s=Cyc_Tcpat_float_pat(_tmp15B);
goto _LLB2;_LLC1: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 1)goto _LLC3;_LLC2: s=(
void*)0;goto _LLB2;_LLC3: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 4)goto _LLC5;
_tmp15C=((struct Cyc_Absyn_Pointer_p_struct*)_tmp157)->f1;_LLC4:{void*_tmp16B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp16C;struct Cyc_Absyn_PtrAtts _tmp16D;struct Cyc_Absyn_Conref*_tmp16E;_LLD8: if(
_tmp16B <= (void*)3?1:*((int*)_tmp16B)!= 4)goto _LLDA;_tmp16C=((struct Cyc_Absyn_PointerType_struct*)
_tmp16B)->f1;_tmp16D=_tmp16C.ptr_atts;_tmp16E=_tmp16D.nullable;_LLD9: {int
is_nullable=0;int still_working=1;while(still_working){void*_tmp16F=(void*)
_tmp16E->v;struct Cyc_Absyn_Conref*_tmp170;int _tmp171;_LLDF: if(_tmp16F <= (void*)1?
1:*((int*)_tmp16F)!= 1)goto _LLE1;_tmp170=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp16F)->f1;_LLE0:(void*)(_tmp16E->v=(void*)((void*)_tmp170->v));continue;_LLE1:
if((int)_tmp16F != 0)goto _LLE3;_LLE2:(void*)(_tmp16E->v=(void*)((void*)({struct
Cyc_Absyn_Eq_constr_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp173;_tmp173.tag=0;_tmp173.f1=(void*)0;
_tmp173;});_tmp172;})));is_nullable=0;still_working=0;goto _LLDE;_LLE3: if(_tmp16F
<= (void*)1?1:*((int*)_tmp16F)!= 0)goto _LLDE;_tmp171=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp16F)->f1;_LLE4: is_nullable=(int)_tmp171;still_working=0;goto _LLDE;_LLDE:;}{
void*ss=Cyc_Tcpat_compile_pat(_tmp15C);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(
ss);else{s=Cyc_Tcpat_ptr_pat(ss);}goto _LLD7;}}_LLDA: if(_tmp16B <= (void*)3?1:*((
int*)_tmp16B)!= 2)goto _LLDC;_LLDB:{void*_tmp174=(void*)_tmp15C->r;struct Cyc_Absyn_Tuniondecl*
_tmp175;struct Cyc_Absyn_Tunionfield*_tmp176;struct Cyc_List_List*_tmp177;_LLE6:
if(_tmp174 <= (void*)2?1:*((int*)_tmp174)!= 6)goto _LLE8;_tmp175=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp174)->f1;_tmp176=((struct Cyc_Absyn_Tunion_p_struct*)_tmp174)->f2;_tmp177=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp174)->f3;_LLE7: {struct Cyc_Core_Opt*span;
if(_tmp175->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*_tmp178=
_cycalloc_atomic(sizeof(*_tmp178));_tmp178->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp175->fields))->v);_tmp178;});}s=Cyc_Tcpat_con_pat(*(*_tmp176->name).f2,span,((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp177));goto _LLE5;}_LLE8:;_LLE9:({void*
_tmp179[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp17A="non-[x]tunion pattern has tunion type";_tag_arr(_tmp17A,
sizeof(char),_get_zero_arr_size(_tmp17A,38));}),_tag_arr(_tmp179,sizeof(void*),0));});
_LLE5:;}goto _LLD7;_LLDC:;_LLDD:({void*_tmp17B[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp17C="pointer pattern does not have pointer type";
_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,43));}),_tag_arr(
_tmp17B,sizeof(void*),0));});_LLD7:;}goto _LLB2;_LLC5: if(_tmp157 <= (void*)2?1:*((
int*)_tmp157)!= 6)goto _LLC7;_tmp15D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp157)->f1;
_tmp15E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp157)->f2;_tmp15F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp157)->f3;if(_tmp15F != 0)goto _LLC7;_LLC6: {struct Cyc_Core_Opt*span;{void*
_tmp17D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
_LLEB: if(_tmp17D <= (void*)3?1:*((int*)_tmp17D)!= 2)goto _LLED;_LLEC: if(_tmp15D->is_xtunion)
span=0;else{span=({struct Cyc_Core_Opt*_tmp17E=_cycalloc_atomic(sizeof(*_tmp17E));
_tmp17E->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp15D->fields))->v);_tmp17E;});}goto _LLEA;
_LLED: if(_tmp17D <= (void*)3?1:*((int*)_tmp17D)!= 3)goto _LLEF;_LLEE: span=({struct
Cyc_Core_Opt*_tmp17F=_cycalloc_atomic(sizeof(*_tmp17F));_tmp17F->v=(void*)1;
_tmp17F;});goto _LLEA;_LLEF:;_LLF0: span=({void*_tmp180[0]={};((struct Cyc_Core_Opt*(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp181="void tunion pattern has bad type";
_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,33));}),_tag_arr(
_tmp180,sizeof(void*),0));});goto _LLEA;_LLEA:;}s=Cyc_Tcpat_con_pat(*(*_tmp15E->name).f2,
span,0);goto _LLB2;}_LLC7: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 6)goto _LLC9;
_tmp160=((struct Cyc_Absyn_Tunion_p_struct*)_tmp157)->f3;_LLC8: _tmp161=_tmp160;
goto _LLCA;_LLC9: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 3)goto _LLCB;_tmp161=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp157)->f1;_LLCA: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp161));goto _LLB2;_LLCB: if(_tmp157 <= (void*)
2?1:*((int*)_tmp157)!= 5)goto _LLCD;_tmp162=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp157)->f1;_tmp163=(void*)_tmp162.aggr_info;if(*((int*)_tmp163)!= 1)goto _LLCD;
_tmp164=((struct Cyc_Absyn_KnownAggr_struct*)_tmp163)->f1;_tmp165=*_tmp164;
_tmp166=((struct Cyc_Absyn_Aggr_p_struct*)_tmp157)->f3;_LLCC: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp165->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*
_tmp190="";_tag_arr(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,1));}))== 0;{
struct Cyc_List_List*dlps0=_tmp166;for(0;!found?dlps0 != 0: 0;dlps0=dlps0->tl){
struct _tuple7 _tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Pat*_tmp185;
struct _tuple7*_tmp182=(struct _tuple7*)dlps0->hd;_tmp183=*_tmp182;_tmp184=_tmp183.f1;
_tmp185=_tmp183.f2;{struct Cyc_List_List*_tmp186=_tmp184;struct Cyc_List_List
_tmp187;void*_tmp188;struct _tagged_arr*_tmp189;struct Cyc_List_List*_tmp18A;_LLF2:
if(_tmp186 == 0)goto _LLF4;_tmp187=*_tmp186;_tmp188=(void*)_tmp187.hd;if(*((int*)
_tmp188)!= 1)goto _LLF4;_tmp189=((struct Cyc_Absyn_FieldName_struct*)_tmp188)->f1;
_tmp18A=_tmp187.tl;if(_tmp18A != 0)goto _LLF4;_LLF3: if(Cyc_strptrcmp(_tmp189,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp185);_tmp18B->tl=ps;_tmp18B;});found=1;}goto _LLF1;_LLF4:;_LLF5:({void*
_tmp18C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp18D="bad designator(s)";_tag_arr(_tmp18D,sizeof(char),
_get_zero_arr_size(_tmp18D,18));}),_tag_arr(_tmp18C,sizeof(void*),0));});_LLF1:;}}}
if(!found)({void*_tmp18E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp18F="bad designator";_tag_arr(_tmp18F,sizeof(
char),_get_zero_arr_size(_tmp18F,15));}),_tag_arr(_tmp18E,sizeof(void*),0));});}}
s=Cyc_Tcpat_tuple_pat(ps);goto _LLB2;}_LLCD: if(_tmp157 <= (void*)2?1:*((int*)
_tmp157)!= 10)goto _LLCF;_tmp167=((struct Cyc_Absyn_Enum_p_struct*)_tmp157)->f1;
_tmp168=((struct Cyc_Absyn_Enum_p_struct*)_tmp157)->f2;_LLCE: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp167->fields))->v);s=Cyc_Tcpat_con_pat(*(*_tmp168->name).f2,({
struct Cyc_Core_Opt*_tmp191=_cycalloc_atomic(sizeof(*_tmp191));_tmp191->v=(void*)
span;_tmp191;}),0);goto _LLB2;}_LLCF: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 
11)goto _LLD1;_tmp169=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp157)->f1;
_tmp16A=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp157)->f2;_LLD0: {struct Cyc_List_List*
fields;{void*_tmp192=Cyc_Tcutil_compress(_tmp169);struct Cyc_List_List*_tmp193;
_LLF7: if(_tmp192 <= (void*)3?1:*((int*)_tmp192)!= 13)goto _LLF9;_tmp193=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp192)->f1;_LLF8: fields=_tmp193;goto _LLF6;_LLF9:;
_LLFA:({void*_tmp194[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp195="bad type in AnonEnum_p";_tag_arr(_tmp195,
sizeof(char),_get_zero_arr_size(_tmp195,23));}),_tag_arr(_tmp194,sizeof(void*),0));});
_LLF6:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*
_tmp16A->name).f2,({struct Cyc_Core_Opt*_tmp196=_cycalloc_atomic(sizeof(*_tmp196));
_tmp196->v=(void*)span;_tmp196;}),0);goto _LLB2;}}_LLD1: if(_tmp157 <= (void*)2?1:*((
int*)_tmp157)!= 12)goto _LLD3;_LLD2:({void*_tmp197[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp198="compile_pat: unknownid";
_tag_arr(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,23));}),_tag_arr(
_tmp197,sizeof(void*),0));});_LLD3: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 13)
goto _LLD5;_LLD4:({void*_tmp199[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp19A="compile_pat: unknowncall";
_tag_arr(_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,25));}),_tag_arr(
_tmp199,sizeof(void*),0));});_LLD5: if(_tmp157 <= (void*)2?1:*((int*)_tmp157)!= 5)
goto _LLB2;_LLD6:({void*_tmp19B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp19C="compile_pat: bad aggr";
_tag_arr(_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,22));}),_tag_arr(
_tmp19B,sizeof(void*),0));});_LLB2:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int
tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Success_struct{int tag;void*f1;};
struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*
f2;void*f3;void*f4;};struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp19D=td;struct Cyc_Set_Set*_tmp19E;_LLFC:
if(*((int*)_tmp19D)!= 1)goto _LLFE;_tmp19E=((struct Cyc_Tcpat_Neg_struct*)_tmp19D)->f1;
_LLFD: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp19E,c))({void*_tmp19F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp1A0="add_neg called when constructor already in set";
_tag_arr(_tmp1A0,sizeof(char),_get_zero_arr_size(_tmp1A0,47));}),_tag_arr(
_tmp19F,sizeof(void*),0));});if(c->span != 0?((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp19E)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v: 0)({void*
_tmp1A1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1A2="add_neg called when |cs U {c}| >= span(c)";_tag_arr(_tmp1A2,
sizeof(char),_get_zero_arr_size(_tmp1A2,42));}),_tag_arr(_tmp1A1,sizeof(void*),0));});
return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1A3=_region_malloc(r,sizeof(*
_tmp1A3));_tmp1A3[0]=({struct Cyc_Tcpat_Neg_struct _tmp1A4;_tmp1A4.tag=1;_tmp1A4.f1=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_rinsert)(r,_tmp19E,c);_tmp1A4;});_tmp1A3;});_LLFE: if(*((int*)
_tmp19D)!= 0)goto _LLFB;_LLFF:({void*_tmp1A5[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A6="add_neg called when td is Positive";
_tag_arr(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,35));}),_tag_arr(
_tmp1A5,sizeof(void*),0));});_LLFB:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp1A7=td;struct Cyc_Tcpat_Con_s*_tmp1A8;struct Cyc_Set_Set*
_tmp1A9;_LL101: if(*((int*)_tmp1A7)!= 0)goto _LL103;_tmp1A8=((struct Cyc_Tcpat_Pos_struct*)
_tmp1A7)->f1;_LL102: if(Cyc_Tcpat_compare_con(c,_tmp1A8)== 0)return(void*)0;else{
return(void*)1;}_LL103: if(*((int*)_tmp1A7)!= 1)goto _LL100;_tmp1A9=((struct Cyc_Tcpat_Neg_struct*)
_tmp1A7)->f1;_LL104: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1A9,c))return(void*)1;else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1A9)+ 1: 0)return(void*)0;else{return(void*)2;}}_LL100:;}struct _tuple11{
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1AA=ctxt;struct Cyc_List_List _tmp1AB;struct _tuple11*_tmp1AC;struct _tuple11
_tmp1AD;struct Cyc_Tcpat_Con_s*_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_List_List*
_tmp1B0;_LL106: if(_tmp1AA != 0)goto _LL108;_LL107: return 0;_LL108: if(_tmp1AA == 0)
goto _LL105;_tmp1AB=*_tmp1AA;_tmp1AC=(struct _tuple11*)_tmp1AB.hd;_tmp1AD=*_tmp1AC;
_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;_tmp1B0=_tmp1AB.tl;_LL109: return({struct
Cyc_List_List*_tmp1B1=_region_malloc(r,sizeof(*_tmp1B1));_tmp1B1->hd=({struct
_tuple11*_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2->f1=_tmp1AE;_tmp1B2->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1B3=_region_malloc(r,sizeof(*
_tmp1B3));_tmp1B3->hd=(void*)dsc;_tmp1B3->tl=_tmp1AF;_tmp1B3;});_tmp1B2;});
_tmp1B1->tl=_tmp1B0;_tmp1B1;});_LL105:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1B4=ctxt;
struct Cyc_List_List _tmp1B5;struct _tuple11*_tmp1B6;struct _tuple11 _tmp1B7;struct
Cyc_Tcpat_Con_s*_tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_List_List*_tmp1BA;
_LL10B: if(_tmp1B4 != 0)goto _LL10D;_LL10C:({void*_tmp1BB[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1BC="norm_context: empty context";
_tag_arr(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,28));}),_tag_arr(
_tmp1BB,sizeof(void*),0));});_LL10D: if(_tmp1B4 == 0)goto _LL10A;_tmp1B5=*_tmp1B4;
_tmp1B6=(struct _tuple11*)_tmp1B5.hd;_tmp1B7=*_tmp1B6;_tmp1B8=_tmp1B7.f1;_tmp1B9=
_tmp1B7.f2;_tmp1BA=_tmp1B5.tl;_LL10E: return Cyc_Tcpat_augment(r,_tmp1BA,(void*)({
struct Cyc_Tcpat_Pos_struct*_tmp1BD=_region_malloc(r,sizeof(*_tmp1BD));_tmp1BD[0]=({
struct Cyc_Tcpat_Pos_struct _tmp1BE;_tmp1BE.tag=0;_tmp1BE.f1=_tmp1B8;_tmp1BE.f2=
Cyc_List_rrev(r,_tmp1B9);_tmp1BE;});_tmp1BD;}));_LL10A:;}static void*Cyc_Tcpat_build_desc(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1C0=({struct _tuple6 _tmp1BF;_tmp1BF.f1=ctxt;_tmp1BF.f2=work;
_tmp1BF;});struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C2;struct Cyc_List_List*
_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List
_tmp1C6;struct _tuple11*_tmp1C7;struct _tuple11 _tmp1C8;struct Cyc_Tcpat_Con_s*
_tmp1C9;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*
_tmp1CC;struct Cyc_List_List _tmp1CD;struct _tuple10*_tmp1CE;struct _tuple10 _tmp1CF;
struct Cyc_List_List*_tmp1D0;struct Cyc_List_List*_tmp1D1;_LL110: _tmp1C1=_tmp1C0.f1;
if(_tmp1C1 != 0)goto _LL112;_tmp1C2=_tmp1C0.f2;if(_tmp1C2 != 0)goto _LL112;_LL111:
return dsc;_LL112: _tmp1C3=_tmp1C0.f1;if(_tmp1C3 != 0)goto _LL114;_LL113: goto _LL115;
_LL114: _tmp1C4=_tmp1C0.f2;if(_tmp1C4 != 0)goto _LL116;_LL115:({void*_tmp1D2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp1D3="build_desc: ctxt and work don't match";_tag_arr(_tmp1D3,sizeof(char),
_get_zero_arr_size(_tmp1D3,38));}),_tag_arr(_tmp1D2,sizeof(void*),0));});_LL116:
_tmp1C5=_tmp1C0.f1;if(_tmp1C5 == 0)goto _LL10F;_tmp1C6=*_tmp1C5;_tmp1C7=(struct
_tuple11*)_tmp1C6.hd;_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C8.f1;_tmp1CA=_tmp1C8.f2;
_tmp1CB=_tmp1C6.tl;_tmp1CC=_tmp1C0.f2;if(_tmp1CC == 0)goto _LL10F;_tmp1CD=*_tmp1CC;
_tmp1CE=(struct _tuple10*)_tmp1CD.hd;_tmp1CF=*_tmp1CE;_tmp1D0=_tmp1CF.f3;_tmp1D1=
_tmp1CD.tl;_LL117: {struct Cyc_Tcpat_Pos_struct*_tmp1D4=({struct Cyc_Tcpat_Pos_struct*
_tmp1D5=_region_malloc(r,sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=_tmp1C9;_tmp1D6.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp1CA),({struct Cyc_List_List*_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));
_tmp1D7->hd=(void*)dsc;_tmp1D7->tl=_tmp1D0;_tmp1D7;}));_tmp1D6;});_tmp1D5;});
return Cyc_Tcpat_build_desc(r,_tmp1CB,(void*)_tmp1D4,_tmp1D1);}_LL10F:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*,void*,struct Cyc_List_List*,void*,
struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_List_List*);static void*
Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp1D8=allmrules;struct Cyc_List_List _tmp1D9;struct _tuple0*
_tmp1DA;struct _tuple0 _tmp1DB;void*_tmp1DC;void*_tmp1DD;struct Cyc_List_List*
_tmp1DE;_LL119: if(_tmp1D8 != 0)goto _LL11B;_LL11A: return(void*)0;_LL11B: if(_tmp1D8
== 0)goto _LL118;_tmp1D9=*_tmp1D8;_tmp1DA=(struct _tuple0*)_tmp1D9.hd;_tmp1DB=*
_tmp1DA;_tmp1DC=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;_tmp1DE=_tmp1D9.tl;_LL11C: return
Cyc_Tcpat_match(r,_tmp1DC,0,dsc,0,0,_tmp1DD,_tmp1DE);_LL118:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));
_tmp1DF[0]=({struct Cyc_Tcpat_Neg_struct _tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1E0;});_tmp1DF;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp1E1=work;
struct Cyc_List_List _tmp1E2;struct _tuple10*_tmp1E3;struct _tuple10 _tmp1E4;struct
Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E7;
struct Cyc_List_List*_tmp1E8;struct Cyc_List_List _tmp1E9;struct _tuple10*_tmp1EA;
struct _tuple10 _tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_List_List*_tmp1ED;
struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1EF;_LL11E: if(_tmp1E1 != 0)
goto _LL120;_LL11F: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp1F0=
_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Tcpat_Success_struct
_tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=(void*)right_hand_side;_tmp1F1;});_tmp1F0;});
_LL120: if(_tmp1E1 == 0)goto _LL122;_tmp1E2=*_tmp1E1;_tmp1E3=(struct _tuple10*)
_tmp1E2.hd;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.f1;if(_tmp1E5 != 0)goto _LL122;_tmp1E6=
_tmp1E4.f2;if(_tmp1E6 != 0)goto _LL122;_tmp1E7=_tmp1E4.f3;if(_tmp1E7 != 0)goto
_LL122;_tmp1E8=_tmp1E2.tl;_LL121: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp1E8,right_hand_side,rules);_LL122: if(_tmp1E1 == 0)goto _LL11D;_tmp1E9=*
_tmp1E1;_tmp1EA=(struct _tuple10*)_tmp1E9.hd;_tmp1EB=*_tmp1EA;_tmp1EC=_tmp1EB.f1;
_tmp1ED=_tmp1EB.f2;_tmp1EE=_tmp1EB.f3;_tmp1EF=_tmp1E9.tl;_LL123: if((_tmp1EC == 0?
1: _tmp1ED == 0)?1: _tmp1EE == 0)({void*_tmp1F2[0]={};Cyc_Tcutil_impos(({const char*
_tmp1F3="tcpat:and_match: malformed work frame";_tag_arr(_tmp1F3,sizeof(char),
_get_zero_arr_size(_tmp1F3,38));}),_tag_arr(_tmp1F2,sizeof(void*),0));});{struct
Cyc_List_List _tmp1F5;void*_tmp1F6;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*
_tmp1F4=(struct Cyc_List_List*)_tmp1EC;_tmp1F5=*_tmp1F4;_tmp1F6=(void*)_tmp1F5.hd;
_tmp1F7=_tmp1F5.tl;{struct Cyc_List_List _tmp1F9;struct Cyc_List_List*_tmp1FA;
struct Cyc_List_List*_tmp1FB;struct Cyc_List_List*_tmp1F8=(struct Cyc_List_List*)
_tmp1ED;_tmp1F9=*_tmp1F8;_tmp1FA=(struct Cyc_List_List*)_tmp1F9.hd;_tmp1FB=
_tmp1F9.tl;{struct Cyc_List_List _tmp1FD;void*_tmp1FE;struct Cyc_List_List*_tmp1FF;
struct Cyc_List_List*_tmp1FC=(struct Cyc_List_List*)_tmp1EE;_tmp1FD=*_tmp1FC;
_tmp1FE=(void*)_tmp1FD.hd;_tmp1FF=_tmp1FD.tl;{struct _tuple10*_tmp200=({struct
_tuple10*_tmp202=_region_malloc(r,sizeof(*_tmp202));_tmp202->f1=_tmp1F7;_tmp202->f2=
_tmp1FB;_tmp202->f3=_tmp1FF;_tmp202;});return Cyc_Tcpat_match(r,_tmp1F6,_tmp1FA,
_tmp1FE,ctx,({struct Cyc_List_List*_tmp201=_region_malloc(r,sizeof(*_tmp201));
_tmp201->hd=_tmp200;_tmp201->tl=_tmp1EF;_tmp201;}),right_hand_side,rules);}}}}
_LL11D:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp203=dsc;struct Cyc_Set_Set*_tmp204;
struct Cyc_List_List*_tmp205;_LL125: if(*((int*)_tmp203)!= 1)goto _LL127;_tmp204=((
struct Cyc_Tcpat_Neg_struct*)_tmp203)->f1;_LL126: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Tcpat_Neg_struct
_tmp209;_tmp209.tag=1;_tmp209.f1=Cyc_Tcpat_empty_con_set(r);_tmp209;});_tmp208;});
struct Cyc_List_List*_tmp206=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp206=({struct
Cyc_List_List*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207->hd=(void*)any;
_tmp207->tl=_tmp206;_tmp207;});}}return _tmp206;}_LL127: if(*((int*)_tmp203)!= 0)
goto _LL124;_tmp205=((struct Cyc_Tcpat_Pos_struct*)_tmp203)->f2;_LL128: return
_tmp205;_LL124:;}struct _tuple12{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple12*env,int i){struct
_tuple12 _tmp20B;struct _RegionHandle*_tmp20C;struct Cyc_List_List*_tmp20D;struct
_tuple12*_tmp20A=env;_tmp20B=*_tmp20A;_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;
return({struct Cyc_List_List*_tmp20E=_region_malloc(_tmp20C,sizeof(*_tmp20E));
_tmp20E->hd=(void*)(i + 1);_tmp20E->tl=_tmp20D;_tmp20E;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple12 _tmp20F=({struct _tuple12 _tmp210;_tmp210.f1=r;_tmp210.f2=obj;
_tmp210;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple12*,int),struct _tuple12*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp20F);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp211=p;struct Cyc_Tcpat_Con_s*
_tmp212;struct Cyc_List_List*_tmp213;_LL12A: if((int)_tmp211 != 0)goto _LL12C;_LL12B:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL12C: if(_tmp211 <= (void*)1?1:*((int*)_tmp211)!= 0)goto _LL129;_tmp212=((
struct Cyc_Tcpat_Con_struct*)_tmp211)->f1;_tmp213=((struct Cyc_Tcpat_Con_struct*)
_tmp211)->f2;_LL12D: {void*_tmp214=Cyc_Tcpat_static_match(_tmp212,dsc);_LL12F:
if((int)_tmp214 != 0)goto _LL131;_LL130: {struct Cyc_List_List*_tmp215=({struct Cyc_List_List*
_tmp21A=_region_malloc(r,sizeof(*_tmp21A));_tmp21A->hd=({struct _tuple11*_tmp21B=
_region_malloc(r,sizeof(*_tmp21B));_tmp21B->f1=_tmp212;_tmp21B->f2=0;_tmp21B;});
_tmp21A->tl=ctx;_tmp21A;});struct _tuple10*_tmp216=({struct _tuple10*_tmp219=
_region_malloc(r,sizeof(*_tmp219));_tmp219->f1=_tmp213;_tmp219->f2=Cyc_Tcpat_getoargs(
r,_tmp212,obj);_tmp219->f3=Cyc_Tcpat_getdargs(r,_tmp212,dsc);_tmp219;});struct
Cyc_List_List*_tmp217=({struct Cyc_List_List*_tmp218=_region_malloc(r,sizeof(*
_tmp218));_tmp218->hd=_tmp216;_tmp218->tl=work;_tmp218;});return Cyc_Tcpat_and_match(
r,_tmp215,_tmp217,right_hand_side,rules);}_LL131: if((int)_tmp214 != 1)goto _LL133;
_LL132: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL133: if((int)_tmp214 != 2)goto _LL12E;_LL134: {struct Cyc_List_List*_tmp21C=({
struct Cyc_List_List*_tmp225=_region_malloc(r,sizeof(*_tmp225));_tmp225->hd=({
struct _tuple11*_tmp226=_region_malloc(r,sizeof(*_tmp226));_tmp226->f1=_tmp212;
_tmp226->f2=0;_tmp226;});_tmp225->tl=ctx;_tmp225;});struct _tuple10*_tmp21D=({
struct _tuple10*_tmp224=_region_malloc(r,sizeof(*_tmp224));_tmp224->f1=_tmp213;
_tmp224->f2=Cyc_Tcpat_getoargs(r,_tmp212,obj);_tmp224->f3=Cyc_Tcpat_getdargs(r,
_tmp212,dsc);_tmp224;});struct Cyc_List_List*_tmp21E=({struct Cyc_List_List*
_tmp223=_region_malloc(r,sizeof(*_tmp223));_tmp223->hd=_tmp21D;_tmp223->tl=work;
_tmp223;});void*_tmp21F=Cyc_Tcpat_and_match(r,_tmp21C,_tmp21E,right_hand_side,
rules);void*_tmp220=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp212),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp221=
_region_malloc(r,sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp222;_tmp222.tag=1;_tmp222.f1=obj;_tmp222.f2=_tmp212;_tmp222.f3=(void*)
_tmp21F;_tmp222.f4=(void*)_tmp220;_tmp222;});_tmp221;});}_LL12E:;}_LL129:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp227=d;void*_tmp228;void*
_tmp229;void*_tmp22A;_LL136: if((int)_tmp227 != 0)goto _LL138;_LL137: not_exhaust(
env1);goto _LL135;_LL138: if(_tmp227 <= (void*)1?1:*((int*)_tmp227)!= 0)goto _LL13A;
_tmp228=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp227)->f1;_LL139: rhs_appears(
env2,_tmp228);goto _LL135;_LL13A: if(_tmp227 <= (void*)1?1:*((int*)_tmp227)!= 1)
goto _LL135;_tmp229=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp227)->f3;_tmp22A=(
void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp227)->f4;_LL13B: Cyc_Tcpat_check_exhaust_overlap(
_tmp229,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp22A,not_exhaust,env1,rhs_appears,env2);goto _LL135;_LL135:;}struct _tuple13{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple14{void*f1;struct _tuple13*f2;}
;struct _tuple15{struct _RegionHandle*f1;int*f2;};static struct _tuple14*Cyc_Tcpat_get_match(
struct _tuple15*env,struct Cyc_Absyn_Switch_clause*swc){struct _tuple15 _tmp22C;
struct _RegionHandle*_tmp22D;int*_tmp22E;struct _tuple15*_tmp22B=env;_tmp22C=*
_tmp22B;_tmp22D=_tmp22C.f1;_tmp22E=_tmp22C.f2;{void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp234=_region_malloc(_tmp22D,
sizeof(*_tmp234));_tmp234->f1=0;_tmp234->f2=(swc->pattern)->loc;_tmp234;});void*
sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->hd=(void*)sp0;_tmp22F->tl=({struct
Cyc_List_List*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp22E);_tmp230->tl=0;_tmp230;});_tmp22F;}));*_tmp22E=*_tmp22E + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->hd=(void*)sp0;
_tmp231->tl=({struct Cyc_List_List*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->hd=(
void*)((void*)0);_tmp232->tl=0;_tmp232;});_tmp231;}));}return({struct _tuple14*
_tmp233=_region_malloc(_tmp22D,sizeof(*_tmp233));_tmp233->f1=sp;_tmp233->f2=rhs;
_tmp233;});}}static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({
void*_tmp235[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp236="patterns may not be exhaustive";
_tag_arr(_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,31));}),_tag_arr(
_tmp235,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple13*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp237=_new_region("r");struct
_RegionHandle*r=& _tmp237;_push_region(r);{int _tmp238=0;struct _tuple15 _tmp239=({
struct _tuple15 _tmp244;_tmp244.f1=r;_tmp244.f2=& _tmp238;_tmp244;});struct Cyc_List_List*
_tmp23A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(
struct _tuple15*,struct Cyc_Absyn_Switch_clause*),struct _tuple15*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp239,swcs);void*_tmp23B=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp23A);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct
Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp23B,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp23A != 0;
_tmp23A=_tmp23A->tl){struct _tuple14 _tmp23D;struct _tuple13*_tmp23E;struct _tuple13
_tmp23F;int _tmp240;struct Cyc_Position_Segment*_tmp241;struct _tuple14*_tmp23C=(
struct _tuple14*)_tmp23A->hd;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.f2;_tmp23F=*_tmp23E;
_tmp240=_tmp23F.f1;_tmp241=_tmp23F.f2;if(!_tmp240)({void*_tmp242[0]={};Cyc_Tcutil_terr(
_tmp241,({const char*_tmp243="redundant pattern";_tag_arr(_tmp243,sizeof(char),
_get_zero_arr_size(_tmp243,18));}),_tag_arr(_tmp242,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple16{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple16*pr){(*pr).f2=0;({void*_tmp245[0]={};
Cyc_Tcutil_warn((*pr).f1,({const char*_tmp246="pattern not exhaustive";_tag_arr(
_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,23));}),_tag_arr(_tmp245,sizeof(
void*),0));});}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct _tuple17{
void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp247=_new_region("r");struct
_RegionHandle*r=& _tmp247;_push_region(r);{struct Cyc_List_List*_tmp248=({struct
Cyc_List_List*_tmp24D=_region_malloc(r,sizeof(*_tmp24D));_tmp24D->hd=({struct
_tuple17*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E->f1=Cyc_Tcpat_compile_pat(
p);_tmp24E->f2=0;_tmp24E;});_tmp24D->tl=0;_tmp24D;});void*_tmp249=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp248);struct _tuple16 _tmp24A=({struct _tuple16 _tmp24C;_tmp24C.f1=loc;_tmp24C.f2=
1;_tmp24C;});((void(*)(void*d,void(*not_exhaust)(struct _tuple16*),struct _tuple16*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp249,Cyc_Tcpat_not_exhaust_warn,& _tmp24A,Cyc_Tcpat_dummy_fn,0);{int _tmp24B=
_tmp24A.f2;_npop_handler(0);return _tmp24B;}};_pop_region(r);}static struct
_tuple14*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp250=_cycalloc(sizeof(*
_tmp250));_tmp250->f1=0;_tmp250->f2=(swc->pattern)->loc;_tmp250;});return({
struct _tuple14*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->f1=sp0;_tmp24F->f2=
rhs;_tmp24F;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp251=((struct Cyc_List_List*(*)(struct
_tuple14*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp252=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp251);((
void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp252,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp251 != 
0;_tmp251=_tmp251->tl){struct _tuple14 _tmp254;struct _tuple13*_tmp255;struct
_tuple13 _tmp256;int _tmp257;struct Cyc_Position_Segment*_tmp258;struct _tuple14*
_tmp253=(struct _tuple14*)_tmp251->hd;_tmp254=*_tmp253;_tmp255=_tmp254.f2;_tmp256=*
_tmp255;_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;if(!_tmp257)({void*_tmp259[0]={};
Cyc_Tcutil_terr(_tmp258,({const char*_tmp25A="redundant pattern";_tag_arr(_tmp25A,
sizeof(char),_get_zero_arr_size(_tmp25A,18));}),_tag_arr(_tmp259,sizeof(void*),0));});}}
