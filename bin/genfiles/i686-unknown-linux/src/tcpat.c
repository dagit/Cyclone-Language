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
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;
int q_restrict: 1;int real_const: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
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
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;
};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _tagged_arr f1;};struct
Cyc_Absyn_String_c_struct{int tag;struct _tagged_arr f1;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _tagged_arr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
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
struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
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
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
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
fields);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;struct Cyc_Absyn_AggrInfo _tmp4;void*_tmp5;void*_tmp6;struct _tuple1*_tmp7;
struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;
struct Cyc_Absyn_Exp*_tmpB;_LL1: if(_tmp0 <= (void*)2  || *((int*)_tmp0)!= 12)goto
_LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct
_handler_cons _tmpC;_push_handler(& _tmpC);{int _tmpE=0;if(setjmp(_tmpC.handler))
_tmpE=1;if(!_tmpE){{void*_tmpF=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp1);struct
Cyc_Absyn_Tuniondecl*_tmp10;struct Cyc_Absyn_Tunionfield*_tmp11;struct Cyc_Absyn_Enumdecl*
_tmp12;struct Cyc_Absyn_Enumfield*_tmp13;void*_tmp14;struct Cyc_Absyn_Enumfield*
_tmp15;_LLC: if(*((int*)_tmpF)!= 1)goto _LLE;_LLD:({void*_tmp16[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp17="struct tag used without arguments in pattern";
_tag_arr(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,45));}),_tag_arr(_tmp16,
sizeof(void*),0));});_npop_handler(0);return;_LLE: if(*((int*)_tmpF)!= 2)goto
_LL10;_tmp10=((struct Cyc_Tcenv_TunionRes_struct*)_tmpF)->f1;_tmp11=((struct Cyc_Tcenv_TunionRes_struct*)
_tmpF)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp19;_tmp19.tag=6;_tmp19.f1=_tmp10;_tmp19.f2=_tmp11;_tmp19.f3=0;_tmp19;});
_tmp18;})));_npop_handler(0);return;_LL10: if(*((int*)_tmpF)!= 3)goto _LL12;_tmp12=((
struct Cyc_Tcenv_EnumRes_struct*)_tmpF)->f1;_tmp13=((struct Cyc_Tcenv_EnumRes_struct*)
_tmpF)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1B;_tmp1B.tag=10;_tmp1B.f1=_tmp12;_tmp1B.f2=_tmp13;_tmp1B;});_tmp1A;})));
_npop_handler(0);return;_LL12: if(*((int*)_tmpF)!= 4)goto _LL14;_tmp14=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpF)->f1;_tmp15=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmpF)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp1D;_tmp1D.tag=11;_tmp1D.f1=(void*)_tmp14;_tmp1D.f2=_tmp15;_tmp1D;});_tmp1C;})));
_npop_handler(0);return;_LL14: if(*((int*)_tmpF)!= 0)goto _LLB;_LL15: goto _LLB;_LLB:;};
_pop_handler();}else{void*_tmpD=(void*)_exn_thrown;void*_tmp1F=_tmpD;_LL17: if(
_tmp1F != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;_LL1A:(void)_throw(
_tmp1F);_LL16:;}}}{void*_tmp20=(*_tmp1).f1;struct Cyc_List_List*_tmp21;_LL1C: if((
int)_tmp20 != 0)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(_tmp20 <= (void*)1  || *((int*)
_tmp20)!= 0)goto _LL20;_tmp21=((struct Cyc_Absyn_Rel_n_struct*)_tmp20)->f1;if(
_tmp21 != 0)goto _LL20;_LL1F:(*_tmp1).f1=(void*)0;(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({
struct Cyc_Absyn_Var_p_struct _tmp23;_tmp23.tag=0;_tmp23.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp23;});_tmp22;})));return;_LL20:;_LL21:({void*_tmp24[0]={};
Cyc_Tcutil_terr(p->loc,({const char*_tmp25="qualified variable in pattern";
_tag_arr(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,30));}),_tag_arr(_tmp24,
sizeof(void*),0));});return;_LL1B:;}_LL3: if(_tmp0 <= (void*)2  || *((int*)_tmp0)!= 
13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_LL4:{struct _handler_cons _tmp26;
_push_handler(& _tmp26);{int _tmp28=0;if(setjmp(_tmp26.handler))_tmp28=1;if(!
_tmp28){{void*_tmp29=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*
_tmp2A;struct Cyc_Absyn_Tuniondecl*_tmp2B;struct Cyc_Absyn_Tunionfield*_tmp2C;
_LL23: if(*((int*)_tmp29)!= 1)goto _LL25;_tmp2A=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp29)->f1;_LL24: if((void*)_tmp2A->kind == (void*)1){({void*_tmp2D[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp2E="cannot pattern-match a union";_tag_arr(_tmp2E,sizeof(
char),_get_zero_arr_size(_tmp2E,29));}),_tag_arr(_tmp2D,sizeof(void*),0));});
_npop_handler(0);return;}{struct Cyc_List_List*_tmp2F=0;for(0;_tmp3 != 0;_tmp3=
_tmp3->tl){_tmp2F=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=({
struct _tuple8*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->f1=0;_tmp31->f2=(struct
Cyc_Absyn_Pat*)_tmp3->hd;_tmp31;});_tmp30->tl=_tmp2F;_tmp30;});}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp33;_tmp33.tag=5;_tmp33.f1=({struct
Cyc_Absyn_AggrInfo _tmp34;_tmp34.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp36;_tmp36.tag=1;_tmp36.f1=({struct Cyc_Absyn_Aggrdecl**_tmp37=_cycalloc(
sizeof(*_tmp37));_tmp37[0]=_tmp2A;_tmp37;});_tmp36;});_tmp35;}));_tmp34.targs=0;
_tmp34;});_tmp33.f2=0;_tmp33.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp2F);_tmp33;});_tmp32;})));_npop_handler(0);return;}_LL25:
if(*((int*)_tmp29)!= 2)goto _LL27;_tmp2B=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp29)->f1;_tmp2C=((struct Cyc_Tcenv_TunionRes_struct*)_tmp29)->f2;_LL26:(void*)(
p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp38=_cycalloc(sizeof(*
_tmp38));_tmp38[0]=({struct Cyc_Absyn_Tunion_p_struct _tmp39;_tmp39.tag=6;_tmp39.f1=
_tmp2B;_tmp39.f2=_tmp2C;_tmp39.f3=_tmp3;_tmp39;});_tmp38;})));_npop_handler(0);
return;_LL27: if(*((int*)_tmp29)!= 3)goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)
_tmp29)!= 4)goto _LL2B;_LL2A:({void*_tmp3A[0]={};Cyc_Tcutil_terr(p->loc,({const
char*_tmp3B="enum tag used with arguments in pattern";_tag_arr(_tmp3B,sizeof(
char),_get_zero_arr_size(_tmp3B,40));}),_tag_arr(_tmp3A,sizeof(void*),0));});
_npop_handler(0);return;_LL2B: if(*((int*)_tmp29)!= 0)goto _LL22;_LL2C: goto _LL22;
_LL22:;};_pop_handler();}else{void*_tmp27=(void*)_exn_thrown;void*_tmp3D=_tmp27;
_LL2E: if(_tmp3D != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;_LL30:;_LL31:(void)
_throw(_tmp3D);_LL2D:;}}}({struct Cyc_String_pa_struct _tmp40;_tmp40.tag=0;_tmp40.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp2));{void*
_tmp3E[1]={& _tmp40};Cyc_Tcutil_terr(p->loc,({const char*_tmp3F="%s is not a constructor in pattern";
_tag_arr(_tmp3F,sizeof(char),_get_zero_arr_size(_tmp3F,35));}),_tag_arr(_tmp3E,
sizeof(void*),1));}});return;_LL5: if(_tmp0 <= (void*)2  || *((int*)_tmp0)!= 5)goto
_LL7;_tmp4=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1;_tmp5=(void*)_tmp4.aggr_info;
if(*((int*)_tmp5)!= 0)goto _LL7;_tmp6=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp5)->f1;_tmp7=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp5)->f2;_tmp8=_tmp4.targs;
_tmp9=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_LL6:{struct _handler_cons _tmp41;_push_handler(& _tmp41);{int _tmp43=0;
if(setjmp(_tmp41.handler))_tmp43=1;if(!_tmp43){{struct Cyc_Absyn_Aggrdecl**_tmp44=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp7);struct Cyc_Absyn_Aggrdecl*_tmp45=*
_tmp44;if((void*)_tmp45->kind == (void*)1){({void*_tmp46[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp47="cannot pattern-match a union";_tag_arr(_tmp47,sizeof(char),
_get_zero_arr_size(_tmp47,29));}),_tag_arr(_tmp46,sizeof(void*),0));});
_npop_handler(0);return;}if(_tmp45->impl == 0){({void*_tmp48[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp49="can't destructure an abstract struct";_tag_arr(_tmp49,
sizeof(char),_get_zero_arr_size(_tmp49,37));}),_tag_arr(_tmp48,sizeof(void*),0));});
_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp45->impl))->exist_vars)- ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp9);if(more_exists != 0)({void*_tmp4A[
0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4B="wrong number of existentially bound type variables";
_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,51));}),_tag_arr(_tmp4A,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp4D;_tmp4D.tag=5;_tmp4D.f1=({struct Cyc_Absyn_AggrInfo _tmp4E;_tmp4E.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));
_tmp4F[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp50;_tmp50.tag=1;_tmp50.f1=_tmp44;
_tmp50;});_tmp4F;}));_tmp4E.targs=_tmp8;_tmp4E;});_tmp4D.f2=_tmp9;_tmp4D.f3=
_tmpA;_tmp4D;});_tmp4C;})));}};_pop_handler();}else{void*_tmp42=(void*)
_exn_thrown;void*_tmp52=_tmp42;_LL33: if(_tmp52 != Cyc_Dict_Absent)goto _LL35;_LL34:({
void*_tmp53[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp54="Non-struct name has designator patterns";
_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,40));}),_tag_arr(_tmp53,
sizeof(void*),0));});goto _LL32;_LL35:;_LL36:(void)_throw(_tmp52);_LL32:;}}}
return;_LL7: if(_tmp0 <= (void*)2  || *((int*)_tmp0)!= 14)goto _LL9;_tmpB=((struct
Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(te,0,_tmpB);if(!Cyc_Tcutil_is_const_exp(
te,_tmpB))({void*_tmp55[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp56="non-constant expression in case pattern";
_tag_arr(_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,40));}),_tag_arr(_tmp55,
sizeof(void*),0));});{unsigned int _tmp58;int _tmp59;struct _tuple7 _tmp57=Cyc_Evexp_eval_const_uint_exp(
_tmpB);_tmp58=_tmp57.f1;_tmp59=_tmp57.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({struct Cyc_Absyn_Int_p_struct _tmp5B;
_tmp5B.tag=7;_tmp5B.f1=(void*)((void*)2);_tmp5B.f2=(int)_tmp58;_tmp5B;});_tmp5A;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _tagged_arr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->v=s;_tmp5C;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp5D=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp5D <= (void*)3  || *((int*)_tmp5D)!= 12)goto _LL3A;_LL39: if(topt
!= 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return numt;}static void
Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,
void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual();*v_result_ptr=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp5E=_cycalloc(sizeof(*_tmp5E));
_tmp5E->hd=vd;_tmp5E->tl=*v_result_ptr;_tmp5E;});}static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult
res2){struct _tuple6*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Tcpat_TcPatResult
_tmp5F=res1;_tmp60=_tmp5F.tvars_and_bounds_opt;_tmp61=_tmp5F.patvars;{struct
_tuple6*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Tcpat_TcPatResult _tmp62=res2;
_tmp63=_tmp62.tvars_and_bounds_opt;_tmp64=_tmp62.patvars;if(_tmp60 != 0  || _tmp63
!= 0){if(_tmp60 == 0)_tmp60=({struct _tuple6*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65->f1=0;_tmp65->f2=0;_tmp65;});if(_tmp63 == 0)_tmp63=({struct _tuple6*_tmp66=
_cycalloc(sizeof(*_tmp66));_tmp66->f1=0;_tmp66->f2=0;_tmp66;});return({struct Cyc_Tcpat_TcPatResult
_tmp67;_tmp67.tvars_and_bounds_opt=({struct _tuple6*_tmp68=_cycalloc(sizeof(*
_tmp68));_tmp68->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)((*((struct _tuple6*)_check_null(_tmp60))).f1,(*((struct
_tuple6*)_check_null(_tmp63))).f1);_tmp68->f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp60))).f2,(*((struct _tuple6*)_check_null(_tmp63))).f2);_tmp68;});
_tmp67.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp61,_tmp64);_tmp67;});}return({struct Cyc_Tcpat_TcPatResult
_tmp69;_tmp69.tvars_and_bounds_opt=0;_tmp69.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp61,_tmp64);
_tmp69;});}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{void*t;struct Cyc_Tcpat_TcPatResult res=({
struct Cyc_Tcpat_TcPatResult _tmp11C;_tmp11C.tvars_and_bounds_opt=0;_tmp11C.patvars=
0;_tmp11C;});{void*_tmp6A=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp6B;struct Cyc_Absyn_Vardecl*
_tmp6C;struct Cyc_Absyn_Tvar*_tmp6D;struct Cyc_Absyn_Vardecl*_tmp6E;void*_tmp6F;
void*_tmp70;void*_tmp71;struct Cyc_Absyn_Enumdecl*_tmp72;void*_tmp73;struct Cyc_Absyn_Pat*
_tmp74;struct Cyc_List_List*_tmp75;struct Cyc_Absyn_AggrInfo _tmp76;void*_tmp77;
struct Cyc_Absyn_Aggrdecl**_tmp78;struct Cyc_Absyn_Aggrdecl*_tmp79;struct Cyc_List_List*
_tmp7A;struct Cyc_List_List**_tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*
_tmp7D;struct Cyc_Absyn_Tuniondecl*_tmp7E;struct Cyc_Absyn_Tunionfield*_tmp7F;
struct Cyc_List_List*_tmp80;struct Cyc_Absyn_AggrInfo _tmp81;void*_tmp82;_LL3D: if((
int)_tmp6A != 0)goto _LL3F;_LL3E: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3C;_LL3F: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 0)goto _LL41;
_tmp6B=((struct Cyc_Absyn_Var_p_struct*)_tmp6A)->f1;_LL40: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(_tmp6B,& res.patvars,t);
goto _LL3C;_LL41: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 1)goto _LL43;_tmp6C=((
struct Cyc_Absyn_Reference_p_struct*)_tmp6A)->f1;_LL42: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(rgn_opt == 0){({void*_tmp83[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp84="* pattern would point into an unknown/unallowed region";_tag_arr(_tmp84,
sizeof(char),_get_zero_arr_size(_tmp84,55));}),_tag_arr(_tmp83,sizeof(void*),0));});
goto _LL3C;}Cyc_Tcpat_set_vd(_tmp6C,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_PointerType_struct
_tmp86;_tmp86.tag=4;_tmp86.f1=({struct Cyc_Absyn_PtrInfo _tmp87;_tmp87.elt_typ=(
void*)t;_tmp87.elt_tq=Cyc_Absyn_empty_tqual();_tmp87.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp88;_tmp88.rgn=(void*)*rgn_opt;_tmp88.nullable=Cyc_Absyn_false_conref;_tmp88.bounds=
Cyc_Absyn_empty_conref();_tmp88.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp88;});_tmp87;});_tmp86;});_tmp85;}));goto _LL3C;_LL43: if(_tmp6A <= (void*)2
 || *((int*)_tmp6A)!= 2)goto _LL45;_tmp6D=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp6A)->f1;_tmp6E=((struct Cyc_Absyn_TagInt_p_struct*)_tmp6A)->f2;_LL44: Cyc_Tcpat_set_vd(
_tmp6E,& res.patvars,(void*)_tmp6E->type);Cyc_Tcenv_add_type_vars(p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp89[1];_tmp89[0]=_tmp6D;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp89,sizeof(struct Cyc_Absyn_Tvar*),1));}));
if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*_tmp8A=
_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=0;_tmp8A->f2=0;_tmp8A;});(*res.tvars_and_bounds_opt).f1=({
struct Cyc_List_List*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->hd=_tmp6D;_tmp8B->tl=(*
res.tvars_and_bounds_opt).f1;_tmp8B;});t=Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(
_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 7)goto _LL47;_tmp6F=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp6A)->f1;if((int)_tmp6F != 1)goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3C;_LL47: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 7)goto _LL49;_tmp70=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6A)->f1;if((int)_tmp70 != 2)goto _LL49;
_LL48: goto _LL4A;_LL49: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 7)goto _LL4B;
_tmp71=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6A)->f1;if((int)_tmp71 != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_char_typ);goto _LL3C;_LL4D: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)
!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL3C;
_LL4F: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 10)goto _LL51;_tmp72=((struct Cyc_Absyn_Enum_p_struct*)
_tmp6A)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_EnumType_struct
_tmp8D;_tmp8D.tag=12;_tmp8D.f1=_tmp72->name;_tmp8D.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp72;_tmp8D;});_tmp8C;}));goto _LL3C;_LL51: if(_tmp6A <= (void*)2  || *((int*)
_tmp6A)!= 11)goto _LL53;_tmp73=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp6A)->f1;
_LL52: t=Cyc_Tcpat_num_type(topt,_tmp73);goto _LL3C;_LL53: if((int)_tmp6A != 1)goto
_LL55;_LL54: if(topt != 0){void*_tmp8E=Cyc_Tcutil_compress(*topt);_LL66: if(_tmp8E
<= (void*)3  || *((int*)_tmp8E)!= 4)goto _LL68;_LL67: t=*topt;goto tcpat_end;_LL68:;
_LL69: goto _LL65;_LL65:;}{struct Cyc_List_List*_tmp8F=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90[0]=({struct Cyc_Absyn_PointerType_struct _tmp91;_tmp91.tag=4;
_tmp91.f1=({struct Cyc_Absyn_PtrInfo _tmp92;_tmp92.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*
_tmp95));_tmp95->v=_tmp8F;_tmp95;}));_tmp92.elt_tq=Cyc_Absyn_empty_tqual();
_tmp92.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp93;_tmp93.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp94=_cycalloc(sizeof(*
_tmp94));_tmp94->v=_tmp8F;_tmp94;}));_tmp93.nullable=Cyc_Absyn_true_conref;
_tmp93.bounds=Cyc_Absyn_empty_conref();_tmp93.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp93;});_tmp92;});_tmp91;});_tmp90;});goto _LL3C;}
_LL55: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 4)goto _LL57;_tmp74=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp6A)->f1;_LL56: {void*inner_typ=(void*)0;void**_tmp96=0;if(topt != 0){void*
_tmp97=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp98;void*_tmp99;
_LL6B: if(_tmp97 <= (void*)3  || *((int*)_tmp97)!= 4)goto _LL6D;_tmp98=((struct Cyc_Absyn_PointerType_struct*)
_tmp97)->f1;_tmp99=(void*)_tmp98.elt_typ;_LL6C: inner_typ=_tmp99;_tmp96=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpAE;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp74,_tmp96,(void**)& ptr_rgn));if(_tmp96 == 0){void*
_tmp9A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp74->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmp9B;void*_tmp9C;struct Cyc_Absyn_Tuniondecl*
_tmp9D;struct Cyc_Absyn_Tunionfield*_tmp9E;struct Cyc_List_List*_tmp9F;_LL70: if(
_tmp9A <= (void*)3  || *((int*)_tmp9A)!= 3)goto _LL72;_tmp9B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp9A)->f1;_tmp9C=(void*)_tmp9B.field_info;if(*((int*)_tmp9C)!= 1)goto _LL72;
_tmp9D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp9C)->f1;_tmp9E=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp9C)->f2;_tmp9F=_tmp9B.targs;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_TunionType_struct
_tmpA1;_tmpA1.tag=2;_tmpA1.f1=({struct Cyc_Absyn_TunionInfo _tmpA2;_tmpA2.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpA3=_cycalloc(sizeof(*
_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpA4;_tmpA4.tag=1;
_tmpA4.f1=({struct Cyc_Absyn_Tuniondecl**_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[
0]=_tmp9D;_tmpA5;});_tmpA4;});_tmpA3;}));_tmpA2.targs=_tmp9F;_tmpA2.rgn=(void*)
ptr_rgn;_tmpA2;});_tmpA1;});_tmpA0;});goto _LL6F;_LL72:;_LL73: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({
struct Cyc_Absyn_PointerType_struct _tmpA7;_tmpA7.tag=4;_tmpA7.f1=({struct Cyc_Absyn_PtrInfo
_tmpA8;_tmpA8.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp74->topt))->v);
_tmpA8.elt_tq=Cyc_Absyn_empty_tqual();_tmpA8.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpA9;_tmpA9.rgn=(void*)ptr_rgn;_tmpA9.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA9.bounds=Cyc_Absyn_empty_conref();_tmpA9.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpA9;});_tmpA8;});
_tmpA7;});_tmpA6;});goto _LL6F;_LL6F:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_PointerType_struct
_tmpAB;_tmpAB.tag=4;_tmpAB.f1=({struct Cyc_Absyn_PtrInfo _tmpAC;_tmpAC.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp74->topt))->v);_tmpAC.elt_tq=
Cyc_Absyn_empty_tqual();_tmpAC.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpAD;_tmpAD.rgn=(
void*)ptr_rgn;_tmpAD.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpAD.bounds=Cyc_Absyn_empty_conref();_tmpAD.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpAD;});_tmpAC;});_tmpAB;});_tmpAA;});}goto _LL3C;}}
_LL57: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 3)goto _LL59;_tmp75=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp6A)->f1;_LL58: {struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;
if(topt != 0){void*_tmpAF=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpB0;
_LL75: if(_tmpAF <= (void*)3  || *((int*)_tmpAF)!= 9)goto _LL77;_tmpB0=((struct Cyc_Absyn_TupleType_struct*)
_tmpAF)->f1;_LL76: topt_ts=_tmpB0;goto _LL74;_LL77:;_LL78: goto _LL74;_LL74:;}for(0;
_tmp75 != 0;_tmp75=_tmp75->tl){void**_tmpB1=0;if(topt_ts != 0){_tmpB1=(void**)&(*((
struct _tuple9*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp75->hd,_tmpB1,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->hd=({
struct _tuple9*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->f1=Cyc_Absyn_empty_tqual();
_tmpB3->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp75->hd)->topt))->v;_tmpB3;});_tmpB2->tl=pat_ts;_tmpB2;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct
Cyc_Absyn_TupleType_struct _tmpB5;_tmpB5.tag=9;_tmpB5.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpB5;});_tmpB4;});goto _LL3C;}
_LL59: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 5)goto _LL5B;_tmp76=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6A)->f1;_tmp77=(void*)_tmp76.aggr_info;if(*((int*)_tmp77)!= 1)goto _LL5B;
_tmp78=((struct Cyc_Absyn_KnownAggr_struct*)_tmp77)->f1;_tmp79=*_tmp78;_tmp7A=
_tmp76.targs;_tmp7B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6A)->f1).targs;_tmp7C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6A)->f2;_tmp7D=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp6A)->f3;_LL5A: if(_tmp79->impl == 0){({void*
_tmpB6[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpB7="can't destructure an abstract struct";
_tag_arr(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,37));}),_tag_arr(_tmpB6,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpB8=_cycalloc(
sizeof(*_tmpB8));_tmpB8->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB8;}));goto _LL3C;}{
struct _RegionHandle _tmpB9=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB9;
_push_region(rgn);{struct Cyc_List_List*_tmpBA=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpBB=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp79->impl))->exist_vars;{struct Cyc_List_List*t=_tmp7C;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpBB))->hd;_tmpBB=
_tmpBB->tl;{void*_tmpBC=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpBD=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpBE=_tmpBD;void*_tmpBF;void*_tmpC0;
_LL7A: if(*((int*)_tmpBE)!= 2)goto _LL7C;_tmpBF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpBE)->f2;_LL7B: _tmpC0=_tmpBF;goto _LL7D;_LL7C: if(*((int*)_tmpBE)!= 0)goto _LL7E;
_tmpC0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpBE)->f1;_LL7D: k2=_tmpC0;goto
_LL79;_LL7E:;_LL7F:({void*_tmpC1[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpC2="unconstrained existential type variable in struct";
_tag_arr(_tmpC2,sizeof(char),_get_zero_arr_size(_tmpC2,50));}),_tag_arr(_tmpC1,
sizeof(void*),0));});_LL79:;}{void*_tmpC3=_tmpBC;void*_tmpC4;struct Cyc_Core_Opt*
_tmpC5;struct Cyc_Core_Opt**_tmpC6;void*_tmpC7;struct Cyc_Core_Opt*_tmpC8;struct
Cyc_Core_Opt**_tmpC9;_LL81: if(*((int*)_tmpC3)!= 0)goto _LL83;_tmpC4=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpC3)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(k2,
_tmpC4))error=1;goto _LL80;_LL83: if(*((int*)_tmpC3)!= 2)goto _LL85;_tmpC5=((struct
Cyc_Absyn_Less_kb_struct*)_tmpC3)->f1;_tmpC6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpC3)->f1;_tmpC7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpC3)->f2;_LL84:
_tmpC9=_tmpC6;goto _LL86;_LL85: if(*((int*)_tmpC3)!= 1)goto _LL80;_tmpC8=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpC3)->f1;_tmpC9=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpC3)->f1;_LL86:*_tmpC9=({struct Cyc_Core_Opt*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=(void*)_tmpBD;_tmpCA;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpCF;_tmpCF.tag=0;_tmpCF.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(k2));{struct Cyc_String_pa_struct
_tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
_tmpBC));{struct Cyc_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmpCB[3]={& _tmpCD,& _tmpCE,&
_tmpCF};Cyc_Tcutil_terr(p->loc,({const char*_tmpCC="type variable %s has kind %s but must have at least kind %s";
_tag_arr(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,60));}),_tag_arr(_tmpCB,
sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_VarType_struct
_tmpD4;_tmpD4.tag=1;_tmpD4.f1=tv;_tmpD4;});_tmpD3;});_tmpBA=({struct Cyc_List_List*
_tmpD0=_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=(void*)((void*)vartype);
_tmpD0->tl=_tmpBA;_tmpD0;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=({struct _tuple0*_tmpD2=_cycalloc(
sizeof(*_tmpD2));_tmpD2->f1=Cyc_Absyn_empty_effect;_tmpD2->f2=vartype;_tmpD2;});
_tmpD1->tl=outlives_constraints;_tmpD1;});}}}}_tmpBA=Cyc_List_imp_rev(_tmpBA);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp7C);struct Cyc_List_List*
_tmpD5=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpD6=({struct _tuple4 _tmpF1;
_tmpF1.f1=_tmpD5;_tmpF1.f2=rgn;_tmpF1;});struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpD6,_tmp79->tvs);struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->exist_vars,
_tmpBA);struct Cyc_List_List*_tmpD9=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpD7);struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpD8);struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpD7,_tmpD8);
if(_tmp7C != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=0;_tmpDC->f2=0;_tmpDC;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp7C);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpDD=0;{struct Cyc_List_List*_tmpDE=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->rgn_po;for(0;_tmpDE != 
0;_tmpDE=_tmpDE->tl){_tmpDD=({struct Cyc_List_List*_tmpDF=_cycalloc(sizeof(*
_tmpDF));_tmpDF->hd=({struct _tuple0*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpDB,(*((struct _tuple0*)_tmpDE->hd)).f1);_tmpE0->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpDB,(*((struct _tuple0*)_tmpDE->hd)).f2);_tmpE0;});
_tmpDF->tl=_tmpDD;_tmpDF;});}}_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpDD);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpDD);}}*_tmp7B=_tmpD9;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpE1=
_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_AggrType_struct _tmpE2;
_tmpE2.tag=10;_tmpE2.f1=({struct Cyc_Absyn_AggrInfo _tmpE3;_tmpE3.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));
_tmpE4[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpE5;_tmpE5.tag=1;_tmpE5.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=_tmp79;
_tmpE6;});_tmpE5;});_tmpE4;}));_tmpE3.targs=*_tmp7B;_tmpE3;});_tmpE2;});_tmpE1;});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmp7D,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp79->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple10 _tmpE8;struct Cyc_Absyn_Aggrfield*_tmpE9;struct Cyc_Absyn_Pat*_tmpEA;
struct _tuple10*_tmpE7=(struct _tuple10*)fields->hd;_tmpE8=*_tmpE7;_tmpE9=_tmpE8.f1;
_tmpEA=_tmpE8.f2;{void*_tmpEB=Cyc_Tcutil_rsubstitute(rgn,_tmpDB,(void*)_tmpE9->type);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmpEA,(void**)& _tmpEB,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEA->topt))->v,
_tmpEB))({struct Cyc_String_pa_struct _tmpF0;_tmpF0.tag=0;_tmpF0.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEA->topt))->v));{struct Cyc_String_pa_struct _tmpEF;_tmpEF.tag=0;
_tmpEF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpEB));{
struct Cyc_String_pa_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmpE9->name);{void*_tmpEC[3]={& _tmpEE,& _tmpEF,& _tmpF0};Cyc_Tcutil_terr(
p->loc,({const char*_tmpED="field %s of struct pattern expects type %s != %s";
_tag_arr(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,49));}),_tag_arr(_tmpEC,
sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}goto _LL3C;_LL5B: if(_tmp6A <= (
void*)2  || *((int*)_tmp6A)!= 6)goto _LL5D;_tmp7E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6A)->f1;_tmp7F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6A)->f2;_tmp80=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6A)->f3;_LL5C:{struct _RegionHandle _tmpF2=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpF2;_push_region(rgn);{struct Cyc_List_List*
tqts=_tmp7F->typs;struct Cyc_List_List*_tmpF3=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple4 _tmpF4=({struct _tuple4 _tmp119;_tmp119.f1=_tmpF3;_tmp119.f2=rgn;
_tmp119;});struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpF4,_tmp7E->tvs);struct Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpF5);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_TunionType_struct
_tmpF8;_tmpF8.tag=2;_tmpF8.f1=({struct Cyc_Absyn_TunionInfo _tmpF9;_tmpF9.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpFB=_cycalloc(sizeof(*
_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpFC;_tmpFC.tag=1;
_tmpFC.f1=({struct Cyc_Absyn_Tuniondecl**_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[
0]=_tmp7E;_tmpFD;});_tmpFC;});_tmpFB;}));_tmpF9.targs=_tmpF6;_tmpF9.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->v=_tmpF3;_tmpFA;}));_tmpF9;});_tmpF8;});
_tmpF7;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpFE=
_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpFF;_tmpFF.tag=3;_tmpFF.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp100;_tmp100.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp101=_cycalloc(
sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmp102;
_tmp102.tag=1;_tmp102.f1=_tmp7E;_tmp102.f2=_tmp7F;_tmp102;});_tmp101;}));_tmp100.targs=
_tmpF6;_tmp100;});_tmpFF;});_tmpFE;});}if(topt != 0  && tqts == 0){void*_tmp103=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmp104;struct Cyc_List_List*_tmp105;_LL88: if(
_tmp103 <= (void*)3  || *((int*)_tmp103)!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp107;_tmp107.tag=3;_tmp107.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp108;
_tmp108.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp10A;_tmp10A.tag=1;_tmp10A.f1=_tmp7E;_tmp10A.f2=_tmp7F;_tmp10A;});_tmp109;}));
_tmp108.targs=_tmpF6;_tmp108;});_tmp107;});_tmp106;});goto _LL87;_LL8A: if(_tmp103
<= (void*)3  || *((int*)_tmp103)!= 2)goto _LL8C;_tmp104=((struct Cyc_Absyn_TunionType_struct*)
_tmp103)->f1;_tmp105=_tmp104.targs;_LL8B: {struct Cyc_List_List*_tmp10B=_tmpF6;
for(0;_tmp10B != 0  && _tmp105 != 0;(_tmp10B=_tmp10B->tl,_tmp105=_tmp105->tl)){Cyc_Tcutil_unify((
void*)_tmp10B->hd,(void*)_tmp105->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}
for(0;_tmp80 != 0  && tqts != 0;(_tmp80=_tmp80->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp10C=(struct Cyc_Absyn_Pat*)_tmp80->hd;void*_tmp10D=Cyc_Tcutil_rsubstitute(rgn,
_tmpF5,(*((struct _tuple9*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp10C,(void**)& _tmp10D,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10C->topt))->v,_tmp10D))({struct Cyc_String_pa_struct _tmp112;
_tmp112.tag=0;_tmp112.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10C->topt))->v));{struct Cyc_String_pa_struct
_tmp111;_tmp111.tag=0;_tmp111.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp10D));{struct Cyc_String_pa_struct _tmp110;_tmp110.tag=0;_tmp110.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp7E->name));{void*
_tmp10E[3]={& _tmp110,& _tmp111,& _tmp112};Cyc_Tcutil_terr(_tmp10C->loc,({const char*
_tmp10F="%s expects argument type %s, not %s";_tag_arr(_tmp10F,sizeof(char),
_get_zero_arr_size(_tmp10F,36));}),_tag_arr(_tmp10E,sizeof(void*),3));}}}});}if(
_tmp80 != 0)({struct Cyc_String_pa_struct _tmp115;_tmp115.tag=0;_tmp115.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp7E->name));{void*
_tmp113[1]={& _tmp115};Cyc_Tcutil_terr(p->loc,({const char*_tmp114="too many arguments for tunion constructor %s";
_tag_arr(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,45));}),_tag_arr(
_tmp113,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct _tmp118;
_tmp118.tag=0;_tmp118.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp7E->name));{void*_tmp116[1]={& _tmp118};Cyc_Tcutil_terr(p->loc,({const char*
_tmp117="too few arguments for tunion constructor %s";_tag_arr(_tmp117,sizeof(
char),_get_zero_arr_size(_tmp117,44));}),_tag_arr(_tmp116,sizeof(void*),1));}});};
_pop_region(rgn);}goto _LL3C;_LL5D: if(_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 5)
goto _LL5F;_tmp81=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6A)->f1;_tmp82=(void*)
_tmp81.aggr_info;if(*((int*)_tmp82)!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(
_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(
_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(
_tmp6A <= (void*)2  || *((int*)_tmp6A)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp11A;}));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp11B=
_cycalloc(sizeof(*_tmp11B));_tmp11B->v=(void*)t;_tmp11B;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp11D=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp11E=_new_region("r");struct _RegionHandle*r=& _tmp11E;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp11D.patvars),p->loc,({const char*
_tmp11F="pattern contains a repeated variable";_tag_arr(_tmp11F,sizeof(char),
_get_zero_arr_size(_tmp11F,37));}));;_pop_region(r);}return _tmp11D;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp120=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp121;struct Cyc_List_List*_tmp122;struct Cyc_List_List*_tmp123;
struct Cyc_List_List*_tmp124;_LL8F: if(_tmp120 <= (void*)2  || *((int*)_tmp120)!= 4)
goto _LL91;_tmp121=((struct Cyc_Absyn_Pointer_p_struct*)_tmp120)->f1;_LL90: Cyc_Tcpat_check_pat_regions(
te,_tmp121);{void*_tmp125=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_PtrAtts _tmp127;void*_tmp128;
struct Cyc_Absyn_TunionInfo _tmp129;void*_tmp12A;_LL9A: if(_tmp125 <= (void*)3  || *((
int*)_tmp125)!= 4)goto _LL9C;_tmp126=((struct Cyc_Absyn_PointerType_struct*)
_tmp125)->f1;_tmp127=_tmp126.ptr_atts;_tmp128=(void*)_tmp127.rgn;_LL9B: _tmp12A=
_tmp128;goto _LL9D;_LL9C: if(_tmp125 <= (void*)3  || *((int*)_tmp125)!= 2)goto _LL9E;
_tmp129=((struct Cyc_Absyn_TunionType_struct*)_tmp125)->f1;_tmp12A=(void*)_tmp129.rgn;
_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp12A);return;_LL9E:;_LL9F:({
void*_tmp12B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp12C="check_pat_regions: bad pointer type";_tag_arr(_tmp12C,sizeof(
char),_get_zero_arr_size(_tmp12C,36));}),_tag_arr(_tmp12B,sizeof(void*),0));});
_LL99:;}_LL91: if(_tmp120 <= (void*)2  || *((int*)_tmp120)!= 6)goto _LL93;_tmp122=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp120)->f3;_LL92: for(0;_tmp122 != 0;_tmp122=
_tmp122->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp122->hd);}{
void*_tmp12D=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo
_tmp12E;void*_tmp12F;_LLA1: if(_tmp12D <= (void*)3  || *((int*)_tmp12D)!= 2)goto
_LLA3;_tmp12E=((struct Cyc_Absyn_TunionType_struct*)_tmp12D)->f1;_tmp12F=(void*)
_tmp12E.rgn;_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp12F);return;_LLA3:
if(_tmp12D <= (void*)3  || *((int*)_tmp12D)!= 3)goto _LLA5;_LLA4: return;_LLA5:;
_LLA6:({void*_tmp130[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp131="check_pat_regions: bad tunion type";
_tag_arr(_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,35));}),_tag_arr(
_tmp130,sizeof(void*),0));});_LLA0:;}_LL93: if(_tmp120 <= (void*)2  || *((int*)
_tmp120)!= 5)goto _LL95;_tmp123=((struct Cyc_Absyn_Aggr_p_struct*)_tmp120)->f3;
_LL94: for(0;_tmp123 != 0;_tmp123=_tmp123->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple8*)_tmp123->hd)).f2);}return;_LL95: if(_tmp120 <= (void*)2  || *((int*)
_tmp120)!= 3)goto _LL97;_tmp124=((struct Cyc_Absyn_Tuple_p_struct*)_tmp120)->f1;
_LL96: for(0;_tmp124 != 0;_tmp124=_tmp124->tl){Cyc_Tcpat_check_pat_regions(te,(
struct Cyc_Absyn_Pat*)_tmp124->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct
Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){void*_tmp132=(void*)c1->name;
struct _tagged_arr _tmp133;int _tmp134;_LLA8: if(*((int*)_tmp132)!= 0)goto _LLAA;
_tmp133=((struct Cyc_Tcpat_Name_v_struct*)_tmp132)->f1;_LLA9: {void*_tmp135=(void*)
c2->name;struct _tagged_arr _tmp136;_LLAD: if(*((int*)_tmp135)!= 0)goto _LLAF;
_tmp136=((struct Cyc_Tcpat_Name_v_struct*)_tmp135)->f1;_LLAE: return Cyc_strcmp((
struct _tagged_arr)_tmp133,(struct _tagged_arr)_tmp136);_LLAF: if(*((int*)_tmp135)
!= 1)goto _LLAC;_LLB0: return - 1;_LLAC:;}_LLAA: if(*((int*)_tmp132)!= 1)goto _LLA7;
_tmp134=((struct Cyc_Tcpat_Int_v_struct*)_tmp132)->f1;_LLAB: {void*_tmp137=(void*)
c2->name;int _tmp138;_LLB2: if(*((int*)_tmp137)!= 0)goto _LLB4;_LLB3: return 1;_LLB4:
if(*((int*)_tmp137)!= 1)goto _LLB1;_tmp138=((struct Cyc_Tcpat_Int_v_struct*)
_tmp137)->f1;_LLB5: return _tmp134 - _tmp138;_LLB1:;}_LLA7:;}static struct Cyc_Set_Set*
Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))
Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(void*)2};static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;static char _tmp13A[
5]="NULL";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={0,{
_tmp13A,_tmp13A,_tmp13A + 5}};static char _tmp13C[4]="ptr";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_ptr_name_value={0,{_tmp13C,_tmp13C,_tmp13C + 4}};static char _tmp13E[6]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={0,{_tmp13E,
_tmp13E,_tmp13E + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(void*)((
void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp140=_cycalloc_atomic(sizeof(*
_tmp140));_tmp140[0]=({struct Cyc_Tcpat_Int_v_struct _tmp141;_tmp141.tag=1;_tmp141.f1=
i;_tmp141;});_tmp140;}));_tmp13F->arity=0;_tmp13F->span=0;_tmp13F;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp144;_tmp144.tag=0;_tmp144.f1=f;_tmp144;});_tmp143;}));_tmp142->arity=0;
_tmp142->span=0;_tmp142;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c){
return({struct Cyc_Tcpat_Con_s*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp147=_cycalloc_atomic(sizeof(*
_tmp147));_tmp147[0]=({struct Cyc_Tcpat_Int_v_struct _tmp148;_tmp148.tag=1;_tmp148.f1=(
int)c;_tmp148;});_tmp147;}));_tmp145->arity=0;_tmp145->span=({struct Cyc_Core_Opt*
_tmp146=_cycalloc_atomic(sizeof(*_tmp146));_tmp146->v=(void*)256;_tmp146;});
_tmp145;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i){return({struct
Cyc_Tcpat_Con_s*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->name=(void*)((void*)&
Cyc_Tcpat_tuple_name_value);_tmp149->arity=i;_tmp149->span=Cyc_Tcpat_one_opt_ptr;
_tmp149;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=({struct Cyc_Tcpat_Con_struct
_tmp14B;_tmp14B.tag=0;_tmp14B.f1=Cyc_Tcpat_null_con;_tmp14B.f2=0;_tmp14B;});
_tmp14A;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_Tcpat_Con_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=Cyc_Tcpat_int_con(i);_tmp14D.f2=0;_tmp14D;});
_tmp14C;});}static void*Cyc_Tcpat_char_pat(char c){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Tcpat_Con_struct
_tmp14F;_tmp14F.tag=0;_tmp14F.f1=Cyc_Tcpat_char_con(c);_tmp14F.f2=0;_tmp14F;});
_tmp14E;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({
struct Cyc_Tcpat_Con_struct _tmp151;_tmp151.tag=0;_tmp151.f1=Cyc_Tcpat_float_con(f);
_tmp151.f2=0;_tmp151;});_tmp150;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp152=_cycalloc(sizeof(*_tmp152));
_tmp152[0]=({struct Cyc_Tcpat_Con_struct _tmp153;_tmp153.tag=0;_tmp153.f1=Cyc_Tcpat_null_ptr_con;
_tmp153.f2=({struct Cyc_List_List*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->hd=(
void*)p;_tmp154->tl=0;_tmp154;});_tmp153;});_tmp152;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp155=_cycalloc(sizeof(*
_tmp155));_tmp155[0]=({struct Cyc_Tcpat_Con_struct _tmp156;_tmp156.tag=0;_tmp156.f1=
Cyc_Tcpat_ptr_con;_tmp156.f2=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*
_tmp157));_tmp157->hd=(void*)p;_tmp157->tl=0;_tmp157;});_tmp156;});_tmp155;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Tcpat_Con_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp159.f2=ss;_tmp159;});_tmp158;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));
_tmp15D[0]=({struct Cyc_Tcpat_Name_v_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=
con_name;_tmp15E;});_tmp15D;}));_tmp15C->arity=Cyc_List_length(ps);_tmp15C->span=
span;_tmp15C;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp15A=_cycalloc(
sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Tcpat_Con_struct _tmp15B;_tmp15B.tag=0;
_tmp15B.f1=c;_tmp15B.f2=ps;_tmp15B;});_tmp15A;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp15F=(void*)p->r;void*_tmp160;int _tmp161;
char _tmp162;struct _tagged_arr _tmp163;struct Cyc_Absyn_Pat*_tmp164;struct Cyc_Absyn_Tuniondecl*
_tmp165;struct Cyc_Absyn_Tunionfield*_tmp166;struct Cyc_List_List*_tmp167;struct
Cyc_List_List*_tmp168;struct Cyc_List_List*_tmp169;struct Cyc_Absyn_AggrInfo
_tmp16A;void*_tmp16B;struct Cyc_Absyn_Aggrdecl**_tmp16C;struct Cyc_Absyn_Aggrdecl*
_tmp16D;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Enumdecl*_tmp16F;struct Cyc_Absyn_Enumfield*
_tmp170;void*_tmp171;struct Cyc_Absyn_Enumfield*_tmp172;_LLB7: if((int)_tmp15F != 0)
goto _LLB9;_LLB8: goto _LLBA;_LLB9: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 0)
goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 2)
goto _LLBD;_LLBC: s=(void*)0;goto _LLB6;_LLBD: if((int)_tmp15F != 1)goto _LLBF;_LLBE: s=
Cyc_Tcpat_null_pat();goto _LLB6;_LLBF: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 
7)goto _LLC1;_tmp160=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp15F)->f1;_tmp161=((
struct Cyc_Absyn_Int_p_struct*)_tmp15F)->f2;_LLC0: s=Cyc_Tcpat_int_pat(_tmp161);
goto _LLB6;_LLC1: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 8)goto _LLC3;_tmp162=((
struct Cyc_Absyn_Char_p_struct*)_tmp15F)->f1;_LLC2: s=Cyc_Tcpat_char_pat(_tmp162);
goto _LLB6;_LLC3: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 9)goto _LLC5;_tmp163=((
struct Cyc_Absyn_Float_p_struct*)_tmp15F)->f1;_LLC4: s=Cyc_Tcpat_float_pat(_tmp163);
goto _LLB6;_LLC5: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 1)goto _LLC7;_LLC6: s=(
void*)0;goto _LLB6;_LLC7: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 4)goto _LLC9;
_tmp164=((struct Cyc_Absyn_Pointer_p_struct*)_tmp15F)->f1;_LLC8:{void*_tmp173=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp174;struct Cyc_Absyn_PtrAtts _tmp175;struct Cyc_Absyn_Conref*_tmp176;_LLDE: if(
_tmp173 <= (void*)3  || *((int*)_tmp173)!= 4)goto _LLE0;_tmp174=((struct Cyc_Absyn_PointerType_struct*)
_tmp173)->f1;_tmp175=_tmp174.ptr_atts;_tmp176=_tmp175.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){void*_tmp177=(void*)
_tmp176->v;struct Cyc_Absyn_Conref*_tmp178;int _tmp179;_LLE5: if(_tmp177 <= (void*)1
 || *((int*)_tmp177)!= 1)goto _LLE7;_tmp178=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp177)->f1;_LLE6:(void*)(_tmp176->v=(void*)((void*)_tmp178->v));continue;_LLE7:
if((int)_tmp177 != 0)goto _LLE9;_LLE8:(void*)(_tmp176->v=(void*)((void*)({struct
Cyc_Absyn_Eq_constr_struct*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(void*)0;
_tmp17B;});_tmp17A;})));is_nullable=0;still_working=0;goto _LLE4;_LLE9: if(_tmp177
<= (void*)1  || *((int*)_tmp177)!= 0)goto _LLE4;_tmp179=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp177)->f1;_LLEA: is_nullable=(int)_tmp179;still_working=0;goto _LLE4;_LLE4:;}{
void*ss=Cyc_Tcpat_compile_pat(_tmp164);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(
ss);else{s=Cyc_Tcpat_ptr_pat(ss);}goto _LLDD;}}_LLE0: if(_tmp173 <= (void*)3  || *((
int*)_tmp173)!= 2)goto _LLE2;_LLE1:{void*_tmp17C=(void*)_tmp164->r;struct Cyc_Absyn_Tuniondecl*
_tmp17D;struct Cyc_Absyn_Tunionfield*_tmp17E;struct Cyc_List_List*_tmp17F;_LLEC:
if(_tmp17C <= (void*)2  || *((int*)_tmp17C)!= 6)goto _LLEE;_tmp17D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp17C)->f1;_tmp17E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp17C)->f2;_tmp17F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp17C)->f3;_LLED: {struct Cyc_Core_Opt*span;
if(_tmp17D->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*_tmp180=
_cycalloc_atomic(sizeof(*_tmp180));_tmp180->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp17D->fields))->v);_tmp180;});}s=Cyc_Tcpat_con_pat(*(*_tmp17E->name).f2,span,((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp17F));goto _LLEB;}_LLEE:;_LLEF:({void*
_tmp181[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp182="non-[x]tunion pattern has tunion type";_tag_arr(_tmp182,
sizeof(char),_get_zero_arr_size(_tmp182,38));}),_tag_arr(_tmp181,sizeof(void*),0));});
_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({void*_tmp183[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp184="pointer pattern does not have pointer type";
_tag_arr(_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,43));}),_tag_arr(
_tmp183,sizeof(void*),0));});_LLDD:;}goto _LLB6;_LLC9: if(_tmp15F <= (void*)2  || *((
int*)_tmp15F)!= 6)goto _LLCB;_tmp165=((struct Cyc_Absyn_Tunion_p_struct*)_tmp15F)->f1;
_tmp166=((struct Cyc_Absyn_Tunion_p_struct*)_tmp15F)->f2;_tmp167=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp15F)->f3;if(_tmp167 != 0)goto _LLCB;_LLCA: {struct Cyc_Core_Opt*span;{void*
_tmp185=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
_LLF1: if(_tmp185 <= (void*)3  || *((int*)_tmp185)!= 2)goto _LLF3;_LLF2: if(_tmp165->is_xtunion)
span=0;else{span=({struct Cyc_Core_Opt*_tmp186=_cycalloc_atomic(sizeof(*_tmp186));
_tmp186->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp165->fields))->v);_tmp186;});}goto _LLF0;
_LLF3: if(_tmp185 <= (void*)3  || *((int*)_tmp185)!= 3)goto _LLF5;_LLF4: span=({
struct Cyc_Core_Opt*_tmp187=_cycalloc_atomic(sizeof(*_tmp187));_tmp187->v=(void*)
1;_tmp187;});goto _LLF0;_LLF5:;_LLF6: span=({void*_tmp188[0]={};((struct Cyc_Core_Opt*(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp189="void tunion pattern has bad type";
_tag_arr(_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,33));}),_tag_arr(
_tmp188,sizeof(void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(*(*_tmp166->name).f2,
span,0);goto _LLB6;}_LLCB: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 6)goto _LLCD;
_tmp168=((struct Cyc_Absyn_Tunion_p_struct*)_tmp15F)->f3;_LLCC: _tmp169=_tmp168;
goto _LLCE;_LLCD: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 3)goto _LLCF;_tmp169=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp15F)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp169));goto _LLB6;_LLCF: if(_tmp15F <= (void*)
2  || *((int*)_tmp15F)!= 5)goto _LLD1;_tmp16A=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp15F)->f1;_tmp16B=(void*)_tmp16A.aggr_info;if(*((int*)_tmp16B)!= 1)goto _LLD1;
_tmp16C=((struct Cyc_Absyn_KnownAggr_struct*)_tmp16B)->f1;_tmp16D=*_tmp16C;
_tmp16E=((struct Cyc_Absyn_Aggr_p_struct*)_tmp15F)->f3;_LLD0: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp16D->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*
_tmp198="";_tag_arr(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,1));}))== 0;{
struct Cyc_List_List*dlps0=_tmp16E;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple8 _tmp18B;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Pat*_tmp18D;
struct _tuple8*_tmp18A=(struct _tuple8*)dlps0->hd;_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f1;
_tmp18D=_tmp18B.f2;{struct Cyc_List_List*_tmp18E=_tmp18C;struct Cyc_List_List
_tmp18F;void*_tmp190;struct _tagged_arr*_tmp191;struct Cyc_List_List*_tmp192;_LLF8:
if(_tmp18E == 0)goto _LLFA;_tmp18F=*_tmp18E;_tmp190=(void*)_tmp18F.hd;if(*((int*)
_tmp190)!= 1)goto _LLFA;_tmp191=((struct Cyc_Absyn_FieldName_struct*)_tmp190)->f1;
_tmp192=_tmp18F.tl;if(_tmp192 != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp191,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp18D);_tmp193->tl=ps;_tmp193;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp194[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp195="bad designator(s)";_tag_arr(_tmp195,sizeof(char),
_get_zero_arr_size(_tmp195,18));}),_tag_arr(_tmp194,sizeof(void*),0));});_LLF7:;}}}
if(!found)({void*_tmp196[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp197="bad designator";_tag_arr(_tmp197,sizeof(
char),_get_zero_arr_size(_tmp197,15));}),_tag_arr(_tmp196,sizeof(void*),0));});}}
s=Cyc_Tcpat_tuple_pat(ps);goto _LLB6;}_LLD1: if(_tmp15F <= (void*)2  || *((int*)
_tmp15F)!= 10)goto _LLD3;_tmp16F=((struct Cyc_Absyn_Enum_p_struct*)_tmp15F)->f1;
_tmp170=((struct Cyc_Absyn_Enum_p_struct*)_tmp15F)->f2;_LLD2: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp16F->fields))->v);s=Cyc_Tcpat_con_pat(*(*_tmp170->name).f2,({
struct Cyc_Core_Opt*_tmp199=_cycalloc_atomic(sizeof(*_tmp199));_tmp199->v=(void*)
span;_tmp199;}),0);goto _LLB6;}_LLD3: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 
11)goto _LLD5;_tmp171=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp15F)->f1;
_tmp172=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp15F)->f2;_LLD4: {struct Cyc_List_List*
fields;{void*_tmp19A=Cyc_Tcutil_compress(_tmp171);struct Cyc_List_List*_tmp19B;
_LLFD: if(_tmp19A <= (void*)3  || *((int*)_tmp19A)!= 13)goto _LLFF;_tmp19B=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp19A)->f1;_LLFE: fields=_tmp19B;goto _LLFC;_LLFF:;
_LL100:({void*_tmp19C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp19D="bad type in AnonEnum_p";_tag_arr(_tmp19D,
sizeof(char),_get_zero_arr_size(_tmp19D,23));}),_tag_arr(_tmp19C,sizeof(void*),0));});
_LLFC:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*
_tmp172->name).f2,({struct Cyc_Core_Opt*_tmp19E=_cycalloc_atomic(sizeof(*_tmp19E));
_tmp19E->v=(void*)span;_tmp19E;}),0);goto _LLB6;}}_LLD5: if(_tmp15F <= (void*)2  || *((
int*)_tmp15F)!= 12)goto _LLD7;_LLD6:({void*_tmp19F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A0="compile_pat: unknownid";
_tag_arr(_tmp1A0,sizeof(char),_get_zero_arr_size(_tmp1A0,23));}),_tag_arr(
_tmp19F,sizeof(void*),0));});_LLD7: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 
13)goto _LLD9;_LLD8:({void*_tmp1A1[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A2="compile_pat: unknowncall";
_tag_arr(_tmp1A2,sizeof(char),_get_zero_arr_size(_tmp1A2,25));}),_tag_arr(
_tmp1A1,sizeof(void*),0));});_LLD9: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 5)
goto _LLDB;_LLDA:({void*_tmp1A3[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A4="compile_pat: bad aggr";
_tag_arr(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,22));}),_tag_arr(
_tmp1A3,sizeof(void*),0));});_LLDB: if(_tmp15F <= (void*)2  || *((int*)_tmp15F)!= 
14)goto _LLB6;_LLDC:({void*_tmp1A5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1A6="compile_pat: exp";_tag_arr(
_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,17));}),_tag_arr(_tmp1A5,sizeof(
void*),0));});_LLB6:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*
f1;};struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1A7=td;struct Cyc_Set_Set*_tmp1A8;_LL102: if(*((int*)_tmp1A7)!= 1)goto
_LL104;_tmp1A8=((struct Cyc_Tcpat_Neg_struct*)_tmp1A7)->f1;_LL103: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1A8,c))({void*
_tmp1A9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1AA="add_neg called when constructor already in set";_tag_arr(
_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,47));}),_tag_arr(_tmp1A9,sizeof(
void*),0));});if(c->span != 0  && ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1A8)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v)({void*_tmp1AB[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1AC="add_neg called when |cs U {c}| >= span(c)";_tag_arr(_tmp1AC,
sizeof(char),_get_zero_arr_size(_tmp1AC,42));}),_tag_arr(_tmp1AB,sizeof(void*),0));});
return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1AD=_region_malloc(r,sizeof(*
_tmp1AD));_tmp1AD[0]=({struct Cyc_Tcpat_Neg_struct _tmp1AE;_tmp1AE.tag=1;_tmp1AE.f1=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_rinsert)(r,_tmp1A8,c);_tmp1AE;});_tmp1AD;});_LL104: if(*((int*)
_tmp1A7)!= 0)goto _LL101;_LL105:({void*_tmp1AF[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp1B0="add_neg called when td is Positive";
_tag_arr(_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,35));}),_tag_arr(
_tmp1AF,sizeof(void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct
Cyc_Tcpat_Con_s*c,void*td){void*_tmp1B1=td;struct Cyc_Tcpat_Con_s*_tmp1B2;struct
Cyc_Set_Set*_tmp1B3;_LL107: if(*((int*)_tmp1B1)!= 0)goto _LL109;_tmp1B2=((struct
Cyc_Tcpat_Pos_struct*)_tmp1B1)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1B2)== 
0)return(void*)0;else{return(void*)1;}_LL109: if(*((int*)_tmp1B1)!= 1)goto _LL106;
_tmp1B3=((struct Cyc_Tcpat_Neg_struct*)_tmp1B1)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*
s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1B3,c))return(void*)1;else{if(c->span
!= 0  && (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_cardinality)(_tmp1B3)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}
struct _tuple12{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp1B4=ctxt;struct Cyc_List_List _tmp1B5;struct _tuple12*
_tmp1B6;struct _tuple12 _tmp1B7;struct Cyc_Tcpat_Con_s*_tmp1B8;struct Cyc_List_List*
_tmp1B9;struct Cyc_List_List*_tmp1BA;_LL10C: if(_tmp1B4 != 0)goto _LL10E;_LL10D:
return 0;_LL10E: if(_tmp1B4 == 0)goto _LL10B;_tmp1B5=*_tmp1B4;_tmp1B6=(struct
_tuple12*)_tmp1B5.hd;_tmp1B7=*_tmp1B6;_tmp1B8=_tmp1B7.f1;_tmp1B9=_tmp1B7.f2;
_tmp1BA=_tmp1B5.tl;_LL10F: return({struct Cyc_List_List*_tmp1BB=_region_malloc(r,
sizeof(*_tmp1BB));_tmp1BB->hd=({struct _tuple12*_tmp1BC=_region_malloc(r,sizeof(*
_tmp1BC));_tmp1BC->f1=_tmp1B8;_tmp1BC->f2=(struct Cyc_List_List*)({struct Cyc_List_List*
_tmp1BD=_region_malloc(r,sizeof(*_tmp1BD));_tmp1BD->hd=(void*)dsc;_tmp1BD->tl=
_tmp1B9;_tmp1BD;});_tmp1BC;});_tmp1BB->tl=_tmp1BA;_tmp1BB;});_LL10B:;}static
struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*
ctxt){struct Cyc_List_List*_tmp1BE=ctxt;struct Cyc_List_List _tmp1BF;struct _tuple12*
_tmp1C0;struct _tuple12 _tmp1C1;struct Cyc_Tcpat_Con_s*_tmp1C2;struct Cyc_List_List*
_tmp1C3;struct Cyc_List_List*_tmp1C4;_LL111: if(_tmp1BE != 0)goto _LL113;_LL112:({
void*_tmp1C5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C6="norm_context: empty context";_tag_arr(_tmp1C6,sizeof(char),
_get_zero_arr_size(_tmp1C6,28));}),_tag_arr(_tmp1C5,sizeof(void*),0));});_LL113:
if(_tmp1BE == 0)goto _LL110;_tmp1BF=*_tmp1BE;_tmp1C0=(struct _tuple12*)_tmp1BF.hd;
_tmp1C1=*_tmp1C0;_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;_tmp1C4=_tmp1BF.tl;_LL114:
return Cyc_Tcpat_augment(r,_tmp1C4,(void*)({struct Cyc_Tcpat_Pos_struct*_tmp1C7=
_region_malloc(r,sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1C8;_tmp1C8.tag=0;_tmp1C8.f1=_tmp1C2;_tmp1C8.f2=Cyc_List_rrev(r,_tmp1C3);
_tmp1C8;});_tmp1C7;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1CA=({struct _tuple6 _tmp1C9;_tmp1C9.f1=ctxt;_tmp1C9.f2=work;
_tmp1C9;});struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_List_List*
_tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List
_tmp1D0;struct _tuple12*_tmp1D1;struct _tuple12 _tmp1D2;struct Cyc_Tcpat_Con_s*
_tmp1D3;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*
_tmp1D6;struct Cyc_List_List _tmp1D7;struct _tuple11*_tmp1D8;struct _tuple11 _tmp1D9;
struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1DB;_LL116: _tmp1CB=_tmp1CA.f1;
if(_tmp1CB != 0)goto _LL118;_tmp1CC=_tmp1CA.f2;if(_tmp1CC != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1CD=_tmp1CA.f1;if(_tmp1CD != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1CE=_tmp1CA.f2;if(_tmp1CE != 0)goto _LL11C;_LL11B:({void*_tmp1DC[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp1DD="build_desc: ctxt and work don't match";_tag_arr(_tmp1DD,sizeof(char),
_get_zero_arr_size(_tmp1DD,38));}),_tag_arr(_tmp1DC,sizeof(void*),0));});_LL11C:
_tmp1CF=_tmp1CA.f1;if(_tmp1CF == 0)goto _LL115;_tmp1D0=*_tmp1CF;_tmp1D1=(struct
_tuple12*)_tmp1D0.hd;_tmp1D2=*_tmp1D1;_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;
_tmp1D5=_tmp1D0.tl;_tmp1D6=_tmp1CA.f2;if(_tmp1D6 == 0)goto _LL115;_tmp1D7=*_tmp1D6;
_tmp1D8=(struct _tuple11*)_tmp1D7.hd;_tmp1D9=*_tmp1D8;_tmp1DA=_tmp1D9.f3;_tmp1DB=
_tmp1D7.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*_tmp1DE=({struct Cyc_Tcpat_Pos_struct*
_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1E0;_tmp1E0.tag=0;_tmp1E0.f1=_tmp1D3;_tmp1E0.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp1D4),({struct Cyc_List_List*_tmp1E1=_region_malloc(r,sizeof(*_tmp1E1));
_tmp1E1->hd=(void*)dsc;_tmp1E1->tl=_tmp1DA;_tmp1E1;}));_tmp1E0;});_tmp1DF;});
return Cyc_Tcpat_build_desc(r,_tmp1D5,(void*)_tmp1DE,_tmp1DB);}_LL115:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*,void*,struct Cyc_List_List*,void*,
struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_List_List*);static void*
Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp1E2=allmrules;struct Cyc_List_List _tmp1E3;struct _tuple0*
_tmp1E4;struct _tuple0 _tmp1E5;void*_tmp1E6;void*_tmp1E7;struct Cyc_List_List*
_tmp1E8;_LL11F: if(_tmp1E2 != 0)goto _LL121;_LL120: return(void*)0;_LL121: if(_tmp1E2
== 0)goto _LL11E;_tmp1E3=*_tmp1E2;_tmp1E4=(struct _tuple0*)_tmp1E3.hd;_tmp1E5=*
_tmp1E4;_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;_tmp1E8=_tmp1E3.tl;_LL122: return
Cyc_Tcpat_match(r,_tmp1E6,0,dsc,0,0,_tmp1E7,_tmp1E8);_LL11E:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));
_tmp1E9[0]=({struct Cyc_Tcpat_Neg_struct _tmp1EA;_tmp1EA.tag=1;_tmp1EA.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1EA;});_tmp1E9;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp1EB=work;
struct Cyc_List_List _tmp1EC;struct _tuple11*_tmp1ED;struct _tuple11 _tmp1EE;struct
Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;
struct Cyc_List_List*_tmp1F2;struct Cyc_List_List _tmp1F3;struct _tuple11*_tmp1F4;
struct _tuple11 _tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F7;
struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F9;_LL124: if(_tmp1EB != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp1FA=
_region_malloc(r,sizeof(*_tmp1FA));_tmp1FA[0]=({struct Cyc_Tcpat_Success_struct
_tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(void*)right_hand_side;_tmp1FB;});_tmp1FA;});
_LL126: if(_tmp1EB == 0)goto _LL128;_tmp1EC=*_tmp1EB;_tmp1ED=(struct _tuple11*)
_tmp1EC.hd;_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.f1;if(_tmp1EF != 0)goto _LL128;_tmp1F0=
_tmp1EE.f2;if(_tmp1F0 != 0)goto _LL128;_tmp1F1=_tmp1EE.f3;if(_tmp1F1 != 0)goto
_LL128;_tmp1F2=_tmp1EC.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp1F2,right_hand_side,rules);_LL128: if(_tmp1EB == 0)goto _LL123;_tmp1F3=*
_tmp1EB;_tmp1F4=(struct _tuple11*)_tmp1F3.hd;_tmp1F5=*_tmp1F4;_tmp1F6=_tmp1F5.f1;
_tmp1F7=_tmp1F5.f2;_tmp1F8=_tmp1F5.f3;_tmp1F9=_tmp1F3.tl;_LL129: if((_tmp1F6 == 0
 || _tmp1F7 == 0) || _tmp1F8 == 0)({void*_tmp1FC[0]={};Cyc_Tcutil_impos(({const
char*_tmp1FD="tcpat:and_match: malformed work frame";_tag_arr(_tmp1FD,sizeof(
char),_get_zero_arr_size(_tmp1FD,38));}),_tag_arr(_tmp1FC,sizeof(void*),0));});{
struct Cyc_List_List _tmp1FF;void*_tmp200;struct Cyc_List_List*_tmp201;struct Cyc_List_List*
_tmp1FE=(struct Cyc_List_List*)_tmp1F6;_tmp1FF=*_tmp1FE;_tmp200=(void*)_tmp1FF.hd;
_tmp201=_tmp1FF.tl;{struct Cyc_List_List _tmp203;struct Cyc_List_List*_tmp204;
struct Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp202=(struct Cyc_List_List*)
_tmp1F7;_tmp203=*_tmp202;_tmp204=(struct Cyc_List_List*)_tmp203.hd;_tmp205=
_tmp203.tl;{struct Cyc_List_List _tmp207;void*_tmp208;struct Cyc_List_List*_tmp209;
struct Cyc_List_List*_tmp206=(struct Cyc_List_List*)_tmp1F8;_tmp207=*_tmp206;
_tmp208=(void*)_tmp207.hd;_tmp209=_tmp207.tl;{struct _tuple11*_tmp20A=({struct
_tuple11*_tmp20C=_region_malloc(r,sizeof(*_tmp20C));_tmp20C->f1=_tmp201;_tmp20C->f2=
_tmp205;_tmp20C->f3=_tmp209;_tmp20C;});return Cyc_Tcpat_match(r,_tmp200,_tmp204,
_tmp208,ctx,({struct Cyc_List_List*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));
_tmp20B->hd=_tmp20A;_tmp20B->tl=_tmp1F9;_tmp20B;}),right_hand_side,rules);}}}}
_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp20D=dsc;struct Cyc_Set_Set*_tmp20E;
struct Cyc_List_List*_tmp20F;_LL12B: if(*((int*)_tmp20D)!= 1)goto _LL12D;_tmp20E=((
struct Cyc_Tcpat_Neg_struct*)_tmp20D)->f1;_LL12C: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp212=_region_malloc(r,sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Tcpat_Neg_struct
_tmp213;_tmp213.tag=1;_tmp213.f1=Cyc_Tcpat_empty_con_set(r);_tmp213;});_tmp212;});
struct Cyc_List_List*_tmp210=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp210=({struct
Cyc_List_List*_tmp211=_region_malloc(r,sizeof(*_tmp211));_tmp211->hd=(void*)any;
_tmp211->tl=_tmp210;_tmp211;});}}return _tmp210;}_LL12D: if(*((int*)_tmp20D)!= 0)
goto _LL12A;_tmp20F=((struct Cyc_Tcpat_Pos_struct*)_tmp20D)->f2;_LL12E: return
_tmp20F;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp215;struct _RegionHandle*_tmp216;struct Cyc_List_List*_tmp217;struct
_tuple13*_tmp214=env;_tmp215=*_tmp214;_tmp216=_tmp215.f1;_tmp217=_tmp215.f2;
return({struct Cyc_List_List*_tmp218=_region_malloc(_tmp216,sizeof(*_tmp218));
_tmp218->hd=(void*)(i + 1);_tmp218->tl=_tmp217;_tmp218;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp219=({struct _tuple13 _tmp21A;_tmp21A.f1=r;_tmp21A.f2=obj;
_tmp21A;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp219);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp21B=p;struct Cyc_Tcpat_Con_s*
_tmp21C;struct Cyc_List_List*_tmp21D;_LL130: if((int)_tmp21B != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp21B <= (void*)1  || *((int*)_tmp21B)!= 0)goto _LL12F;_tmp21C=((
struct Cyc_Tcpat_Con_struct*)_tmp21B)->f1;_tmp21D=((struct Cyc_Tcpat_Con_struct*)
_tmp21B)->f2;_LL133: {void*_tmp21E=Cyc_Tcpat_static_match(_tmp21C,dsc);_LL135:
if((int)_tmp21E != 0)goto _LL137;_LL136: {struct Cyc_List_List*_tmp21F=({struct Cyc_List_List*
_tmp224=_region_malloc(r,sizeof(*_tmp224));_tmp224->hd=({struct _tuple12*_tmp225=
_region_malloc(r,sizeof(*_tmp225));_tmp225->f1=_tmp21C;_tmp225->f2=0;_tmp225;});
_tmp224->tl=ctx;_tmp224;});struct _tuple11*_tmp220=({struct _tuple11*_tmp223=
_region_malloc(r,sizeof(*_tmp223));_tmp223->f1=_tmp21D;_tmp223->f2=Cyc_Tcpat_getoargs(
r,_tmp21C,obj);_tmp223->f3=Cyc_Tcpat_getdargs(r,_tmp21C,dsc);_tmp223;});struct
Cyc_List_List*_tmp221=({struct Cyc_List_List*_tmp222=_region_malloc(r,sizeof(*
_tmp222));_tmp222->hd=_tmp220;_tmp222->tl=work;_tmp222;});return Cyc_Tcpat_and_match(
r,_tmp21F,_tmp221,right_hand_side,rules);}_LL137: if((int)_tmp21E != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp21E != 2)goto _LL134;_LL13A: {struct Cyc_List_List*_tmp226=({
struct Cyc_List_List*_tmp22F=_region_malloc(r,sizeof(*_tmp22F));_tmp22F->hd=({
struct _tuple12*_tmp230=_region_malloc(r,sizeof(*_tmp230));_tmp230->f1=_tmp21C;
_tmp230->f2=0;_tmp230;});_tmp22F->tl=ctx;_tmp22F;});struct _tuple11*_tmp227=({
struct _tuple11*_tmp22E=_region_malloc(r,sizeof(*_tmp22E));_tmp22E->f1=_tmp21D;
_tmp22E->f2=Cyc_Tcpat_getoargs(r,_tmp21C,obj);_tmp22E->f3=Cyc_Tcpat_getdargs(r,
_tmp21C,dsc);_tmp22E;});struct Cyc_List_List*_tmp228=({struct Cyc_List_List*
_tmp22D=_region_malloc(r,sizeof(*_tmp22D));_tmp22D->hd=_tmp227;_tmp22D->tl=work;
_tmp22D;});void*_tmp229=Cyc_Tcpat_and_match(r,_tmp226,_tmp228,right_hand_side,
rules);void*_tmp22A=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp21C),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp22B=
_region_malloc(r,sizeof(*_tmp22B));_tmp22B[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp22C;_tmp22C.tag=1;_tmp22C.f1=obj;_tmp22C.f2=_tmp21C;_tmp22C.f3=(void*)
_tmp229;_tmp22C.f4=(void*)_tmp22A;_tmp22C;});_tmp22B;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp231=d;void*_tmp232;void*
_tmp233;void*_tmp234;_LL13C: if((int)_tmp231 != 0)goto _LL13E;_LL13D: not_exhaust(
env1);goto _LL13B;_LL13E: if(_tmp231 <= (void*)1  || *((int*)_tmp231)!= 0)goto _LL140;
_tmp232=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp231)->f1;_LL13F: rhs_appears(
env2,_tmp232);goto _LL13B;_LL140: if(_tmp231 <= (void*)1  || *((int*)_tmp231)!= 1)
goto _LL13B;_tmp233=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp231)->f3;_tmp234=(
void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp231)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
_tmp233,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp234,not_exhaust,env1,rhs_appears,env2);goto _LL13B;_LL13B:;}struct _tuple14{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;struct _tuple14*f2;}
;struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct _tuple15*Cyc_Tcpat_get_match(
struct _tuple16*env,struct Cyc_Absyn_Switch_clause*swc){struct _tuple16 _tmp236;
struct _RegionHandle*_tmp237;int*_tmp238;struct _tuple16*_tmp235=env;_tmp236=*
_tmp235;_tmp237=_tmp236.f1;_tmp238=_tmp236.f2;{void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple14*rhs=({struct _tuple14*_tmp23E=_region_malloc(_tmp237,
sizeof(*_tmp23E));_tmp23E->f1=0;_tmp23E->f2=(swc->pattern)->loc;_tmp23E;});void*
sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->hd=(void*)sp0;_tmp239->tl=({struct
Cyc_List_List*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp238);_tmp23A->tl=0;_tmp23A;});_tmp239;}));*_tmp238=*_tmp238 + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->hd=(void*)sp0;
_tmp23B->tl=({struct Cyc_List_List*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->hd=(
void*)((void*)0);_tmp23C->tl=0;_tmp23C;});_tmp23B;}));}return({struct _tuple15*
_tmp23D=_region_malloc(_tmp237,sizeof(*_tmp23D));_tmp23D->f1=sp;_tmp23D->f2=rhs;
_tmp23D;});}}static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({
void*_tmp23F[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp240="patterns may not be exhaustive";
_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,31));}),_tag_arr(
_tmp23F,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp241=_new_region("r");struct
_RegionHandle*r=& _tmp241;_push_region(r);{int _tmp242=0;struct _tuple16 _tmp243=({
struct _tuple16 _tmp24E;_tmp24E.f1=r;_tmp24E.f2=& _tmp242;_tmp24E;});struct Cyc_List_List*
_tmp244=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp243,swcs);void*_tmp245=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp244);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct
Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp245,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp244 != 0;
_tmp244=_tmp244->tl){struct _tuple15 _tmp247;struct _tuple14*_tmp248;struct _tuple14
_tmp249;int _tmp24A;struct Cyc_Position_Segment*_tmp24B;struct _tuple15*_tmp246=(
struct _tuple15*)_tmp244->hd;_tmp247=*_tmp246;_tmp248=_tmp247.f2;_tmp249=*_tmp248;
_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;if(!_tmp24A)({void*_tmp24C[0]={};Cyc_Tcutil_terr(
_tmp24B,({const char*_tmp24D="redundant pattern";_tag_arr(_tmp24D,sizeof(char),
_get_zero_arr_size(_tmp24D,18));}),_tag_arr(_tmp24C,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple17{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple17*pr){(*pr).f2=0;({void*_tmp24F[0]={};
Cyc_Tcutil_warn((*pr).f1,({const char*_tmp250="pattern not exhaustive";_tag_arr(
_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,23));}),_tag_arr(_tmp24F,sizeof(
void*),0));});}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct _tuple18{
void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp251=_new_region("r");struct
_RegionHandle*r=& _tmp251;_push_region(r);{struct Cyc_List_List*_tmp252=({struct
Cyc_List_List*_tmp257=_region_malloc(r,sizeof(*_tmp257));_tmp257->hd=({struct
_tuple18*_tmp258=_region_malloc(r,sizeof(*_tmp258));_tmp258->f1=Cyc_Tcpat_compile_pat(
p);_tmp258->f2=0;_tmp258;});_tmp257->tl=0;_tmp257;});void*_tmp253=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp252);struct _tuple17 _tmp254=({struct _tuple17 _tmp256;_tmp256.f1=loc;_tmp256.f2=
1;_tmp256;});((void(*)(void*d,void(*not_exhaust)(struct _tuple17*),struct _tuple17*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp253,Cyc_Tcpat_not_exhaust_warn,& _tmp254,Cyc_Tcpat_dummy_fn,0);{int _tmp255=
_tmp254.f2;_npop_handler(0);return _tmp255;}};_pop_region(r);}static struct
_tuple15*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple14*rhs=({struct _tuple14*_tmp25A=_cycalloc(sizeof(*
_tmp25A));_tmp25A->f1=0;_tmp25A->f2=(swc->pattern)->loc;_tmp25A;});return({
struct _tuple15*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->f1=sp0;_tmp259->f2=
rhs;_tmp259;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp25B=((struct Cyc_List_List*(*)(struct
_tuple15*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp25C=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp25B);((
void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple14*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp25C,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp25B != 
0;_tmp25B=_tmp25B->tl){struct _tuple15 _tmp25E;struct _tuple14*_tmp25F;struct
_tuple14 _tmp260;int _tmp261;struct Cyc_Position_Segment*_tmp262;struct _tuple15*
_tmp25D=(struct _tuple15*)_tmp25B->hd;_tmp25E=*_tmp25D;_tmp25F=_tmp25E.f2;_tmp260=*
_tmp25F;_tmp261=_tmp260.f1;_tmp262=_tmp260.f2;if(!_tmp261)({void*_tmp263[0]={};
Cyc_Tcutil_terr(_tmp262,({const char*_tmp264="redundant pattern";_tag_arr(_tmp264,
sizeof(char),_get_zero_arr_size(_tmp264,18));}),_tag_arr(_tmp263,sizeof(void*),0));});}}
