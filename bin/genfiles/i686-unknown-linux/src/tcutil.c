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
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_printf(struct _tagged_arr,struct _tagged_arr);
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(
void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
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
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
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
struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
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
Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=
0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=0;struct
_tagged_arr Cyc_Tcutil_failure_reason=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct _tagged_arr s1=(struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure));{
struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)(Cyc_Tcutil_tq1_const?({
const char*_tmp23="const ";_tag_arr(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,
7));}):({const char*_tmp24="";_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(
_tmp24,1));}));{void*_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";
_tag_arr(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,5));}),_tag_arr(_tmp1F,
sizeof(void*),2));}}});struct _tagged_arr s2=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure));{struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(
struct _tagged_arr)(Cyc_Tcutil_tq2_const?({const char*_tmp1D="const ";_tag_arr(
_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,7));}):({const char*_tmp1E="";
_tag_arr(_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,1));}));{void*_tmp19[2]={&
_tmp1B,& _tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";_tag_arr(_tmp1A,sizeof(
char),_get_zero_arr_size(_tmp1A,5));}),_tag_arr(_tmp19,sizeof(void*),2));}}});
int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct
_tagged_arr)((struct _tagged_arr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({
const char*_tmp1="  %s";_tag_arr(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,5));}),
_tag_arr(_tmp0,sizeof(void*),1));}});pos +=_get_arr_size(s1,sizeof(char));if(pos + 
5 >= 80){({void*_tmp3[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp4="\n\t";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,3));}),_tag_arr(_tmp3,
sizeof(void*),0));});pos=8;}else{({void*_tmp5[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp6=" ";_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,2));}),
_tag_arr(_tmp5,sizeof(void*),0));});pos ++;}({void*_tmp7[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8="and ";_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,5));}),
_tag_arr(_tmp7,sizeof(void*),0));});pos +=4;if(pos + _get_arr_size(s2,sizeof(char))
>= 80){({void*_tmp9[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpA="\n\t";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,3));}),_tag_arr(_tmp9,
sizeof(void*),0));});pos=8;}({struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(
struct _tagged_arr)((struct _tagged_arr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC="%s ";_tag_arr(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,4));}),_tag_arr(_tmpB,sizeof(void*),1));}});pos +=
_get_arr_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){({void*_tmpE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpF="\n\t";_tag_arr(_tmpF,sizeof(char),
_get_zero_arr_size(_tmpF,3));}),_tag_arr(_tmpE,sizeof(void*),0));});pos=8;}({
void*_tmp10[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp11="are not compatible. ";
_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,21));}),_tag_arr(_tmp10,
sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){if(pos + Cyc_strlen((struct _tagged_arr)Cyc_Tcutil_failure_reason)
>= 80)({void*_tmp12[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp13="\n\t";
_tag_arr(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,3));}),_tag_arr(_tmp12,
sizeof(void*),0));});({struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_failure_reason);{void*_tmp14[1]={&
_tmp16};Cyc_fprintf(Cyc_stderr,({const char*_tmp15="%s";_tag_arr(_tmp15,sizeof(
char),_get_zero_arr_size(_tmp15,3));}),_tag_arr(_tmp14,sizeof(void*),1));}});}({
void*_tmp17[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="\n";_tag_arr(_tmp18,
sizeof(char),_get_zero_arr_size(_tmp18,2));}),_tag_arr(_tmp17,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)((struct _tagged_arr)msg);{void*
_tmp25[1]={& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="Compiler Error (Tcutil::impos): %s\n";
_tag_arr(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,36));}),_tag_arr(_tmp25,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp28=_cycalloc(sizeof(*_tmp28));
_tmp28[0]=({struct Cyc_Core_Impossible_struct _tmp29;_tmp29.tag=Cyc_Core_Impossible;
_tmp29.f1=msg;_tmp29;});_tmp28;}));}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;
_tmp2D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp2C;
_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd));{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B="%s::%s ";_tag_arr(_tmp2B,sizeof(char),
_get_zero_arr_size(_tmp2B,8));}),_tag_arr(_tmp2A,sizeof(void*),2));}}});}({void*
_tmp2E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F="\n";_tag_arr(_tmp2F,
sizeof(char),_get_zero_arr_size(_tmp2F,2));}),_tag_arr(_tmp2E,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){struct
_tagged_arr msg=(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({
struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=sg;_tmp30->tl=
Cyc_Tcutil_warning_segs;_tmp30;});Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*
_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct _tagged_arr*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=msg;_tmp32;});_tmp31->tl=Cyc_Tcutil_warning_msgs;
_tmp31;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;({void*_tmp33[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp34="***Warnings***\n";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,16));}),_tag_arr(_tmp33,
sizeof(void*),0));});{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=
0;_tmp39.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd));{void*_tmp36[2]={&
_tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({const char*_tmp37="%s: %s\n";_tag_arr(
_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,8));}),_tag_arr(_tmp36,sizeof(void*),
2));}}});_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp3A[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3B="**************\n";_tag_arr(_tmp3B,sizeof(char),
_get_zero_arr_size(_tmp3B,16));}),_tag_arr(_tmp3A,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return*((int*)_check_null(tv1->identity))- *((int*)_check_null(tv2->identity));}
void*Cyc_Tcutil_compress(void*t){void*_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**
_tmp3E;void**_tmp3F;void***_tmp40;struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**
_tmp42;_LL1: if(_tmp3C <= (void*)3?1:*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp3C <= (void*)3?1:*((int*)_tmp3C)!= 16)goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp3C <= (void*)3?1:*((
int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_LL6: {void*
t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((void**)
_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=
t2;_tmp43;});return t2;}_LL7: if(_tmp3C <= (void*)3?1:*((int*)_tmp3C)!= 0)goto _LL9;
_tmp41=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp45=(void*)c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((int)_tmp45
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp45 <= (void*)1?1:*((
int*)_tmp45)!= 0)goto _LL10;_tmp46=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp45)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if(_tmp45 <= (void*)1?1:*((
int*)_tmp45)!= 1)goto _LLB;_tmp47=((struct Cyc_Absyn_Forward_constr_struct*)_tmp45)->f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;void*_tmp4A;_LL13: if(*((
int*)_tmp48)!= 0)goto _LL15;_tmp49=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp48)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4B=_cycalloc(sizeof(*
_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(
void*)_tmp49;_tmp4C;});_tmp4B;});_LL15: if(*((int*)_tmp48)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp4E;_tmp4E.tag=1;_tmp4E.f1=0;
_tmp4E;});_tmp4D;});_LL17: if(*((int*)_tmp48)!= 2)goto _LL12;_tmp4A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp50;_tmp50.tag=2;_tmp50.f1=0;_tmp50.f2=(void*)_tmp4A;_tmp50;});_tmp4F;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name;
_tmp51->identity=0;_tmp51->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp51;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp53;struct Cyc_Core_Opt*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;
struct _tuple2*_tmp52=arg;_tmp53=*_tmp52;_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_tmp56=
_tmp53.f3;return({struct _tuple2*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=
_tmp54;_tmp57->f2=_tmp55;_tmp57->f3=Cyc_Tcutil_copy_type(_tmp56);_tmp57;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp59;
struct Cyc_Absyn_Tqual _tmp5A;void*_tmp5B;struct _tuple4*_tmp58=arg;_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;return({struct _tuple4*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->f1=_tmp5A;_tmp5C->f2=Cyc_Tcutil_copy_type(_tmp5B);_tmp5C;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=
f->name;_tmp5D->tq=f->tq;_tmp5D->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp5D->width=f->width;_tmp5D->attributes=f->attributes;_tmp5D;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp5F;void*_tmp60;
void*_tmp61;struct _tuple6*_tmp5E=x;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
return({struct _tuple6*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=Cyc_Tcutil_copy_type(
_tmp60);_tmp62->f2=Cyc_Tcutil_copy_type(_tmp61);_tmp62;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->name=f->name;_tmp63->tag=f->tag;_tmp63->loc=
f->loc;_tmp63;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;void*_tmp67;
struct Cyc_List_List*_tmp68;void*_tmp69;struct Cyc_Absyn_TunionFieldInfo _tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;
struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts _tmp70;void*_tmp71;struct Cyc_Absyn_Conref*
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;void*_tmp75;
void*_tmp76;int _tmp77;struct Cyc_Absyn_ArrayInfo _tmp78;void*_tmp79;struct Cyc_Absyn_Tqual
_tmp7A;struct Cyc_Absyn_Exp*_tmp7B;struct Cyc_Absyn_Conref*_tmp7C;struct Cyc_Absyn_FnInfo
_tmp7D;struct Cyc_List_List*_tmp7E;struct Cyc_Core_Opt*_tmp7F;void*_tmp80;struct
Cyc_List_List*_tmp81;int _tmp82;struct Cyc_Absyn_VarargInfo*_tmp83;struct Cyc_List_List*
_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_AggrInfo
_tmp87;void*_tmp88;void*_tmp89;struct _tuple1*_tmp8A;struct Cyc_List_List*_tmp8B;
struct Cyc_Absyn_AggrInfo _tmp8C;void*_tmp8D;struct Cyc_Absyn_Aggrdecl**_tmp8E;
struct Cyc_List_List*_tmp8F;void*_tmp90;struct Cyc_List_List*_tmp91;struct _tuple1*
_tmp92;struct Cyc_Absyn_Enumdecl*_tmp93;struct Cyc_List_List*_tmp94;void*_tmp95;
int _tmp96;void*_tmp97;void*_tmp98;struct _tuple1*_tmp99;struct Cyc_List_List*
_tmp9A;struct Cyc_Absyn_Typedefdecl*_tmp9B;void*_tmp9C;struct Cyc_List_List*_tmp9D;
void*_tmp9E;_LL1A: if((int)_tmp64 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (
void*)3?1:*((int*)_tmp64)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(_tmp64 <= (void*)3?
1:*((int*)_tmp64)!= 1)goto _LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;
_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_VarType_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=
Cyc_Tcutil_copy_tvar(_tmp65);_tmpA0;});_tmp9F;});_LL20: if(_tmp64 <= (void*)3?1:*((
int*)_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;
_tmp67=(void*)_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)_tmp66.rgn;
_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_TunionType_struct _tmpA2;_tmpA2.tag=2;_tmpA2.f1=({
struct Cyc_Absyn_TunionInfo _tmpA3;_tmpA3.tunion_info=(void*)_tmp67;_tmpA3.targs=
Cyc_Tcutil_copy_types(_tmp68);_tmpA3.rgn=(void*)Cyc_Tcutil_copy_type(_tmp69);
_tmpA3;});_tmpA2;});_tmpA1;});_LL22: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 3)
goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;_tmp6B=(
void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpA5;_tmpA5.tag=3;_tmpA5.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpA6;_tmpA6.field_info=(
void*)_tmp6B;_tmpA6.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpA6;});_tmpA5;});
_tmpA4;});_LL24: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((
struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;
_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;_tmp72=
_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_LL25: {void*_tmpA7=
Cyc_Tcutil_copy_type(_tmp6E);void*_tmpA8=Cyc_Tcutil_copy_type(_tmp71);struct Cyc_Absyn_Conref*
_tmpA9=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp72);struct Cyc_Absyn_Tqual _tmpAA=_tmp6F;struct Cyc_Absyn_Conref*_tmpAB=Cyc_Tcutil_copy_conref(
_tmp73);struct Cyc_Absyn_Conref*_tmpAC=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_PointerType_struct
_tmpAE;_tmpAE.tag=4;_tmpAE.f1=({struct Cyc_Absyn_PtrInfo _tmpAF;_tmpAF.elt_typ=(
void*)_tmpA7;_tmpAF.elt_tq=_tmpAA;_tmpAF.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB0;_tmpB0.rgn=(void*)_tmpA8;_tmpB0.nullable=_tmpA9;_tmpB0.bounds=_tmpAB;
_tmpB0.zero_term=_tmpAC;_tmpB0;});_tmpAF;});_tmpAE;});_tmpAD;});}_LL26: if(_tmp64
<= (void*)3?1:*((int*)_tmp64)!= 5)goto _LL28;_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f1;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));
_tmpB1[0]=({struct Cyc_Absyn_IntType_struct _tmpB2;_tmpB2.tag=5;_tmpB2.f1=(void*)
_tmp75;_tmpB2.f2=(void*)_tmp76;_tmpB2;});_tmpB1;});_LL28: if((int)_tmp64 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 6)goto _LL2C;
_tmp77=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;_LL2B: return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmpB3=_cycalloc_atomic(sizeof(*_tmpB3));
_tmpB3[0]=({struct Cyc_Absyn_DoubleType_struct _tmpB4;_tmpB4.tag=6;_tmpB4.f1=
_tmp77;_tmpB4;});_tmpB3;});_LL2C: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 7)goto
_LL2E;_tmp78=((struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;_tmp79=(void*)
_tmp78.elt_type;_tmp7A=_tmp78.tq;_tmp7B=_tmp78.num_elts;_tmp7C=_tmp78.zero_term;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB5=_cycalloc(sizeof(*
_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB6;_tmpB6.tag=7;_tmpB6.f1=({
struct Cyc_Absyn_ArrayInfo _tmpB7;_tmpB7.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp79);_tmpB7.tq=_tmp7A;_tmpB7.num_elts=_tmp7B;_tmpB7.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp7C);_tmpB7;});_tmpB6;});
_tmpB5;});_LL2E: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 8)goto _LL30;_tmp7D=((
struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp7E=_tmp7D.tvars;_tmp7F=_tmp7D.effect;
_tmp80=(void*)_tmp7D.ret_typ;_tmp81=_tmp7D.args;_tmp82=_tmp7D.c_varargs;_tmp83=
_tmp7D.cyc_varargs;_tmp84=_tmp7D.rgn_po;_tmp85=_tmp7D.attributes;_LL2F: {struct
Cyc_List_List*_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7E);struct Cyc_Core_Opt*_tmpB9=_tmp7F == 0?0:({struct Cyc_Core_Opt*_tmpC3=
_cycalloc(sizeof(*_tmpC3));_tmpC3->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp7F->v);
_tmpC3;});void*_tmpBA=Cyc_Tcutil_copy_type(_tmp80);struct Cyc_List_List*_tmpBB=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp81);int _tmpBC=_tmp82;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp83 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp83;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpBD=_cycalloc(sizeof(*_tmpBD));
_tmpBD->name=cv->name;_tmpBD->tq=cv->tq;_tmpBD->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpBD->inject=cv->inject;_tmpBD;});}{struct Cyc_List_List*_tmpBE=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp84);struct Cyc_List_List*_tmpBF=_tmp85;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_FnType_struct _tmpC1;_tmpC1.tag=8;_tmpC1.f1=({struct
Cyc_Absyn_FnInfo _tmpC2;_tmpC2.tvars=_tmpB8;_tmpC2.effect=_tmpB9;_tmpC2.ret_typ=(
void*)_tmpBA;_tmpC2.args=_tmpBB;_tmpC2.c_varargs=_tmpBC;_tmpC2.cyc_varargs=
cyc_varargs2;_tmpC2.rgn_po=_tmpBE;_tmpC2.attributes=_tmpBF;_tmpC2;});_tmpC1;});
_tmpC0;});}}_LL30: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 9)goto _LL32;_tmp86=((
struct Cyc_Absyn_TupleType_struct*)_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_TupleType_struct
_tmpC5;_tmpC5.tag=9;_tmpC5.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp86);
_tmpC5;});_tmpC4;});_LL32: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 10)goto _LL34;
_tmp87=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;_tmp88=(void*)_tmp87.aggr_info;
if(*((int*)_tmp88)!= 0)goto _LL34;_tmp89=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp88)->f1;_tmp8A=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp88)->f2;_tmp8B=
_tmp87.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AggrType_struct _tmpC7;
_tmpC7.tag=10;_tmpC7.f1=({struct Cyc_Absyn_AggrInfo _tmpC8;_tmpC8.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));
_tmpC9[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(
void*)_tmp89;_tmpCA.f2=_tmp8A;_tmpCA;});_tmpC9;}));_tmpC8.targs=Cyc_Tcutil_copy_types(
_tmp8B);_tmpC8;});_tmpC7;});_tmpC6;});_LL34: if(_tmp64 <= (void*)3?1:*((int*)
_tmp64)!= 10)goto _LL36;_tmp8C=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;
_tmp8D=(void*)_tmp8C.aggr_info;if(*((int*)_tmp8D)!= 1)goto _LL36;_tmp8E=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp8D)->f1;_tmp8F=_tmp8C.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_AggrType_struct _tmpCC;_tmpCC.tag=10;_tmpCC.f1=({struct Cyc_Absyn_AggrInfo
_tmpCD;_tmpCD.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpCF;
_tmpCF.tag=1;_tmpCF.f1=_tmp8E;_tmpCF;});_tmpCE;}));_tmpCD.targs=Cyc_Tcutil_copy_types(
_tmp8F);_tmpCD;});_tmpCC;});_tmpCB;});_LL36: if(_tmp64 <= (void*)3?1:*((int*)
_tmp64)!= 11)goto _LL38;_tmp90=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64)->f1;_tmp91=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD0=_cycalloc(sizeof(*
_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpD1;_tmpD1.tag=11;
_tmpD1.f1=(void*)_tmp90;_tmpD1.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp91);_tmpD1;});_tmpD0;});_LL38: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 12)
goto _LL3A;_tmp92=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD3;_tmpD3.tag=12;_tmpD3.f1=_tmp92;_tmpD3.f2=_tmp93;_tmpD3;});_tmpD2;});_LL3A:
if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 13)goto _LL3C;_tmp94=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpD4=
_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpD5;
_tmpD5.tag=13;_tmpD5.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp94);_tmpD5;});_tmpD4;});_LL3C: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 14)
goto _LL3E;_tmp95=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));
_tmpD6[0]=({struct Cyc_Absyn_SizeofType_struct _tmpD7;_tmpD7.tag=14;_tmpD7.f1=(
void*)Cyc_Tcutil_copy_type(_tmp95);_tmpD7;});_tmpD6;});_LL3E: if(_tmp64 <= (void*)
3?1:*((int*)_tmp64)!= 18)goto _LL40;_tmp96=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpD8=
_cycalloc_atomic(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpD9;_tmpD9.tag=18;_tmpD9.f1=_tmp96;_tmpD9;});_tmpD8;});_LL40: if(_tmp64 <= (
void*)3?1:*((int*)_tmp64)!= 17)goto _LL42;_tmp97=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp64)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDA=_cycalloc(
sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TagType_struct _tmpDB;_tmpDB.tag=17;
_tmpDB.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpDB;});_tmpDA;});_LL42: if(
_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 15)goto _LL44;_tmp98=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpDD;_tmpDD.tag=15;_tmpDD.f1=(void*)Cyc_Tcutil_copy_type(_tmp98);_tmpDD;});
_tmpDC;});_LL44: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 16)goto _LL46;_tmp99=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;_tmp9A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f2;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpDE=_cycalloc(sizeof(*
_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TypedefType_struct _tmpDF;_tmpDF.tag=16;
_tmpDF.f1=_tmp99;_tmpDF.f2=Cyc_Tcutil_copy_types(_tmp9A);_tmpDF.f3=_tmp9B;_tmpDF.f4=
0;_tmpDF;});_tmpDE;});_LL46: if((int)_tmp64 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 19)goto _LL4A;_tmp9C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE1;
_tmpE1.tag=19;_tmpE1.f1=(void*)Cyc_Tcutil_copy_type(_tmp9C);_tmpE1;});_tmpE0;});
_LL4A: if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 20)goto _LL4C;_tmp9D=((struct Cyc_Absyn_JoinEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_JoinEff_struct _tmpE3;_tmpE3.tag=20;
_tmpE3.f1=Cyc_Tcutil_copy_types(_tmp9D);_tmpE3;});_tmpE2;});_LL4C: if(_tmp64 <= (
void*)3?1:*((int*)_tmp64)!= 21)goto _LL19;_tmp9E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpE4=_cycalloc(
sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpE5;_tmpE5.tag=21;
_tmpE5.f1=(void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE5;});_tmpE4;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpE7=({struct _tuple6 _tmpE6;
_tmpE6.f1=k1;_tmpE6.f2=k2;_tmpE6;});void*_tmpE8;void*_tmpE9;void*_tmpEA;void*
_tmpEB;void*_tmpEC;void*_tmpED;_LL4F: _tmpE8=_tmpE7.f1;if((int)_tmpE8 != 2)goto
_LL51;_tmpE9=_tmpE7.f2;if((int)_tmpE9 != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpEA=
_tmpE7.f1;if((int)_tmpEA != 2)goto _LL53;_tmpEB=_tmpE7.f2;if((int)_tmpEB != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpEC=_tmpE7.f1;if((int)_tmpEC != 1)goto _LL55;_tmpED=
_tmpE7.f2;if((int)_tmpED != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEE=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpEF;void*_tmpF0;_LL58: if(*((int*)_tmpEE)!= 0)goto _LL5A;
_tmpEF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEE)->f1;_LL59: return _tmpEF;
_LL5A: if(*((int*)_tmpEE)!= 2)goto _LL5C;_tmpF0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEE)->f2;_LL5B: return _tmpF0;_LL5C:;_LL5D:({void*_tmpF1[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF2="kind not suitably constrained!";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,31));}),_tag_arr(_tmpF1,
sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF3=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpF4;struct Cyc_Core_Opt*_tmpF5;struct Cyc_Absyn_Tvar*
_tmpF6;void*_tmpF7;struct Cyc_Absyn_TunionFieldInfo _tmpF8;void*_tmpF9;struct Cyc_Absyn_Tunionfield*
_tmpFA;struct Cyc_Absyn_TunionFieldInfo _tmpFB;void*_tmpFC;struct Cyc_Absyn_Enumdecl*
_tmpFD;struct Cyc_Absyn_AggrInfo _tmpFE;void*_tmpFF;struct Cyc_Absyn_AggrInfo
_tmp100;void*_tmp101;struct Cyc_Absyn_Aggrdecl**_tmp102;struct Cyc_Absyn_Aggrdecl*
_tmp103;struct Cyc_Absyn_Aggrdecl _tmp104;struct Cyc_Absyn_AggrdeclImpl*_tmp105;
struct Cyc_Absyn_Enumdecl*_tmp106;struct Cyc_Absyn_PtrInfo _tmp107;struct Cyc_Absyn_Typedefdecl*
_tmp108;_LL5F: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 0)goto _LL61;_tmpF4=((
struct Cyc_Absyn_Evar_struct*)_tmpF3)->f1;_tmpF5=((struct Cyc_Absyn_Evar_struct*)
_tmpF3)->f2;_LL60: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4))->v;
_LL61: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 1)goto _LL63;_tmpF6=((struct Cyc_Absyn_VarType_struct*)
_tmpF3)->f1;_LL62: return Cyc_Tcutil_tvar_kind(_tmpF6);_LL63: if((int)_tmpF3 != 0)
goto _LL65;_LL64: return(void*)1;_LL65: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 5)
goto _LL67;_tmpF7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF3)->f2;_LL66:
return _tmpF7 == (void*)2?(void*)2:(void*)1;_LL67: if((int)_tmpF3 != 1)goto _LL69;
_LL68: goto _LL6A;_LL69: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 6)goto _LL6B;_LL6A:
goto _LL6C;_LL6B: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 8)goto _LL6D;_LL6C:
return(void*)1;_LL6D: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 15)goto _LL6F;_LL6E:
return(void*)2;_LL6F: if((int)_tmpF3 != 2)goto _LL71;_LL70: return(void*)3;_LL71: if(
_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 2)goto _LL73;_LL72: return(void*)2;_LL73: if(
_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 3)goto _LL75;_tmpF8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF3)->f1;_tmpF9=(void*)_tmpF8.field_info;if(*((int*)_tmpF9)!= 1)goto _LL75;
_tmpFA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpF9)->f2;_LL74: if(_tmpFA->typs
== 0)return(void*)2;else{return(void*)1;}_LL75: if(_tmpF3 <= (void*)3?1:*((int*)
_tmpF3)!= 3)goto _LL77;_tmpFB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF3)->f1;
_tmpFC=(void*)_tmpFB.field_info;if(*((int*)_tmpFC)!= 0)goto _LL77;_LL76:({void*
_tmp109[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp10A="typ_kind: Unresolved TunionFieldType";_tag_arr(_tmp10A,
sizeof(char),_get_zero_arr_size(_tmp10A,37));}),_tag_arr(_tmp109,sizeof(void*),0));});
_LL77: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 12)goto _LL79;_tmpFD=((struct Cyc_Absyn_EnumType_struct*)
_tmpF3)->f2;if(_tmpFD != 0)goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpF3 <= (void*)3?1:*((
int*)_tmpF3)!= 10)goto _LL7B;_tmpFE=((struct Cyc_Absyn_AggrType_struct*)_tmpF3)->f1;
_tmpFF=(void*)_tmpFE.aggr_info;if(*((int*)_tmpFF)!= 0)goto _LL7B;_LL7A: return(
void*)0;_LL7B: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 10)goto _LL7D;_tmp100=((
struct Cyc_Absyn_AggrType_struct*)_tmpF3)->f1;_tmp101=(void*)_tmp100.aggr_info;
if(*((int*)_tmp101)!= 1)goto _LL7D;_tmp102=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp101)->f1;_tmp103=*_tmp102;_tmp104=*_tmp103;_tmp105=_tmp104.impl;_LL7C: return
_tmp105 == 0?(void*)0:(void*)1;_LL7D: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 11)
goto _LL7F;_LL7E: goto _LL80;_LL7F: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 13)goto
_LL81;_LL80: return(void*)1;_LL81: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 12)
goto _LL83;_tmp106=((struct Cyc_Absyn_EnumType_struct*)_tmpF3)->f2;_LL82: if(
_tmp106->fields == 0)return(void*)0;else{return(void*)1;}_LL83: if(_tmpF3 <= (void*)
3?1:*((int*)_tmpF3)!= 4)goto _LL85;_tmp107=((struct Cyc_Absyn_PointerType_struct*)
_tmpF3)->f1;_LL84: {void*_tmp10B=(void*)(Cyc_Absyn_compress_conref((_tmp107.ptr_atts).bounds))->v;
void*_tmp10C;void*_tmp10D;void*_tmp10E;_LL98: if((int)_tmp10B != 0)goto _LL9A;_LL99:
goto _LL9B;_LL9A: if(_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 0)goto _LL9C;_tmp10C=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10B)->f1;if((int)_tmp10C != 0)goto
_LL9C;_LL9B: return(void*)1;_LL9C: if(_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 0)
goto _LL9E;_tmp10D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10B)->f1;if(
_tmp10D <= (void*)1?1:*((int*)_tmp10D)!= 0)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(
_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 0)goto _LLA0;_tmp10E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10B)->f1;if(_tmp10E <= (void*)1?1:*((int*)_tmp10E)!= 1)goto _LLA0;_LL9F: return(
void*)2;_LLA0: if(_tmp10B <= (void*)1?1:*((int*)_tmp10B)!= 1)goto _LL97;_LLA1:({
void*_tmp10F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp110="typ_kind: forward constr in ptr bounds";_tag_arr(_tmp110,
sizeof(char),_get_zero_arr_size(_tmp110,39));}),_tag_arr(_tmp10F,sizeof(void*),0));});
_LL97:;}_LL85: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 14)goto _LL87;_LL86: return(
void*)2;_LL87: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 18)goto _LL89;_LL88: return(
void*)5;_LL89: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 17)goto _LL8B;_LL8A: return(
void*)2;_LL8B: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 7)goto _LL8D;_LL8C: goto
_LL8E;_LL8D: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 9)goto _LL8F;_LL8E: return(
void*)1;_LL8F: if(_tmpF3 <= (void*)3?1:*((int*)_tmpF3)!= 16)goto _LL91;_tmp108=((
struct Cyc_Absyn_TypedefType_struct*)_tmpF3)->f3;_LL90: if(_tmp108 == 0?1: _tmp108->kind
== 0)({struct Cyc_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp111[1]={&
_tmp113};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp112="typ_kind: typedef found: %s";_tag_arr(_tmp112,sizeof(char),
_get_zero_arr_size(_tmp112,28));}),_tag_arr(_tmp111,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->kind))->v;_LL91: if(_tmpF3 <= (
void*)3?1:*((int*)_tmpF3)!= 19)goto _LL93;_LL92: goto _LL94;_LL93: if(_tmpF3 <= (void*)
3?1:*((int*)_tmpF3)!= 20)goto _LL95;_LL94: goto _LL96;_LL95: if(_tmpF3 <= (void*)3?1:*((
int*)_tmpF3)!= 21)goto _LL5E;_LL96: return(void*)4;_LL5E:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=
0;if(setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp117=1;_npop_handler(0);return _tmp117;};_pop_handler();}else{void*_tmp115=(
void*)_exn_thrown;void*_tmp119=_tmp115;_LLA3: if(_tmp119 != Cyc_Tcutil_Unify)goto
_LLA5;_LLA4: return 0;_LLA5:;_LLA6:(void)_throw(_tmp119);_LLA2:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp11A=t;struct Cyc_Absyn_Tvar*
_tmp11B;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Core_Opt*_tmp11D;struct Cyc_Core_Opt**
_tmp11E;struct Cyc_Absyn_PtrInfo _tmp11F;struct Cyc_Absyn_ArrayInfo _tmp120;void*
_tmp121;struct Cyc_Absyn_FnInfo _tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Core_Opt*
_tmp124;void*_tmp125;struct Cyc_List_List*_tmp126;int _tmp127;struct Cyc_Absyn_VarargInfo*
_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*
_tmp12B;struct Cyc_Absyn_TunionInfo _tmp12C;struct Cyc_List_List*_tmp12D;void*
_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_TunionFieldInfo _tmp130;
struct Cyc_List_List*_tmp131;struct Cyc_Absyn_AggrInfo _tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_List_List*_tmp134;void*_tmp135;void*_tmp136;void*_tmp137;void*
_tmp138;void*_tmp139;struct Cyc_List_List*_tmp13A;_LLA8: if(_tmp11A <= (void*)3?1:*((
int*)_tmp11A)!= 1)goto _LLAA;_tmp11B=((struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;
_LLA9: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp11B)){Cyc_Tcutil_failure_reason=({const char*_tmp13B="(type variable would escape scope)";
_tag_arr(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,35));});(int)_throw((
void*)Cyc_Tcutil_Unify);}goto _LLA7;_LLAA: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)
!= 0)goto _LLAC;_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f4;_tmp11E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f4;_LLAB: if(t == evar){Cyc_Tcutil_failure_reason=({
const char*_tmp13C="(occurs check)";_tag_arr(_tmp13C,sizeof(char),
_get_zero_arr_size(_tmp13C,15));});(int)_throw((void*)Cyc_Tcutil_Unify);}else{
if(_tmp11C != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp11C->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp11E))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp13D=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp11E))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd))_tmp13D=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=(
struct Cyc_Absyn_Tvar*)s->hd;_tmp13E->tl=_tmp13D;_tmp13E;});}}*_tmp11E=({struct
Cyc_Core_Opt*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->v=_tmp13D;_tmp13F;});}}}
goto _LLA7;_LLAC: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 4)goto _LLAE;_tmp11F=((
struct Cyc_Absyn_PointerType_struct*)_tmp11A)->f1;_LLAD: Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp11F.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp11F.ptr_atts).rgn);{
void*_tmp140=(void*)(Cyc_Absyn_compress_conref((_tmp11F.ptr_atts).bounds))->v;
void*_tmp141;void*_tmp142;_LLCD: if(_tmp140 <= (void*)1?1:*((int*)_tmp140)!= 0)
goto _LLCF;_tmp141=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp140)->f1;if(
_tmp141 <= (void*)1?1:*((int*)_tmp141)!= 1)goto _LLCF;_tmp142=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp141)->f1;_LLCE: Cyc_Tcutil_occurs(evar,r,env,_tmp142);goto _LLCC;_LLCF:;_LLD0:
goto _LLCC;_LLCC:;}goto _LLA7;_LLAE: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 7)
goto _LLB0;_tmp120=((struct Cyc_Absyn_ArrayType_struct*)_tmp11A)->f1;_tmp121=(void*)
_tmp120.elt_type;_LLAF: Cyc_Tcutil_occurs(evar,r,env,_tmp121);goto _LLA7;_LLB0: if(
_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 8)goto _LLB2;_tmp122=((struct Cyc_Absyn_FnType_struct*)
_tmp11A)->f1;_tmp123=_tmp122.tvars;_tmp124=_tmp122.effect;_tmp125=(void*)_tmp122.ret_typ;
_tmp126=_tmp122.args;_tmp127=_tmp122.c_varargs;_tmp128=_tmp122.cyc_varargs;
_tmp129=_tmp122.rgn_po;_tmp12A=_tmp122.attributes;_LLB1: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp123,env);if(_tmp124 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp124->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp125);for(0;_tmp126 != 0;_tmp126=_tmp126->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp126->hd)).f3);}if(_tmp128 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp128->type);for(0;_tmp129 != 0;_tmp129=_tmp129->tl){struct
_tuple6 _tmp144;void*_tmp145;void*_tmp146;struct _tuple6*_tmp143=(struct _tuple6*)
_tmp129->hd;_tmp144=*_tmp143;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp145);Cyc_Tcutil_occurs(evar,r,env,_tmp146);}goto _LLA7;_LLB2: if(
_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 9)goto _LLB4;_tmp12B=((struct Cyc_Absyn_TupleType_struct*)
_tmp11A)->f1;_LLB3: for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp12B->hd)).f2);}goto _LLA7;_LLB4: if(_tmp11A <= (void*)
3?1:*((int*)_tmp11A)!= 2)goto _LLB6;_tmp12C=((struct Cyc_Absyn_TunionType_struct*)
_tmp11A)->f1;_tmp12D=_tmp12C.targs;_tmp12E=(void*)_tmp12C.rgn;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,_tmp12E);Cyc_Tcutil_occurslist(evar,r,env,_tmp12D);goto _LLA7;_LLB6:
if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 16)goto _LLB8;_tmp12F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_LLB7: _tmp131=_tmp12F;goto _LLB9;_LLB8: if(_tmp11A <= (void*)3?1:*((
int*)_tmp11A)!= 3)goto _LLBA;_tmp130=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11A)->f1;_tmp131=_tmp130.targs;_LLB9: _tmp133=_tmp131;goto _LLBB;_LLBA: if(
_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 10)goto _LLBC;_tmp132=((struct Cyc_Absyn_AggrType_struct*)
_tmp11A)->f1;_tmp133=_tmp132.targs;_LLBB: Cyc_Tcutil_occurslist(evar,r,env,
_tmp133);goto _LLA7;_LLBC: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 11)goto _LLBE;
_tmp134=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLBD: for(0;_tmp134
!= 0;_tmp134=_tmp134->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp134->hd)->type);}goto _LLA7;_LLBE: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 
17)goto _LLC0;_tmp135=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLBF:
_tmp136=_tmp135;goto _LLC1;_LLC0: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 14)
goto _LLC2;_tmp136=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11A)->f1;_LLC1:
_tmp137=_tmp136;goto _LLC3;_LLC2: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 19)
goto _LLC4;_tmp137=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11A)->f1;_LLC3:
_tmp138=_tmp137;goto _LLC5;_LLC4: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 15)
goto _LLC6;_tmp138=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;
_LLC5: _tmp139=_tmp138;goto _LLC7;_LLC6: if(_tmp11A <= (void*)3?1:*((int*)_tmp11A)!= 
21)goto _LLC8;_tmp139=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLC7:
Cyc_Tcutil_occurs(evar,r,env,_tmp139);goto _LLA7;_LLC8: if(_tmp11A <= (void*)3?1:*((
int*)_tmp11A)!= 20)goto _LLCA;_tmp13A=((struct Cyc_Absyn_JoinEff_struct*)_tmp11A)->f1;
_LLC9: Cyc_Tcutil_occurslist(evar,r,env,_tmp13A);goto _LLA7;_LLCA:;_LLCB: goto _LLA7;
_LLA7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if((tq1.q_const != tq2.q_const?1: tq1.q_volatile != tq2.q_volatile)?1:
tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.q_const;Cyc_Tcutil_tq2_const=tq2.q_const;Cyc_Tcutil_failure_reason=({
const char*_tmp147="(qualifiers don't match)";_tag_arr(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?tq1.q_volatile == tq2.q_volatile:
0)?tq1.q_restrict == tq2.q_restrict: 0;}static void Cyc_Tcutil_unify_it_conrefs(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp148=(void*)x->v;void*_tmp149;_LLD2: if((int)_tmp148 != 0)
goto _LLD4;_LLD3:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp14B;_tmp14B.tag=1;_tmp14B.f1=y;_tmp14B;});_tmp14A;})));return;_LLD4: if(
_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 0)goto _LLD6;_tmp149=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp148)->f1;_LLD5: {void*_tmp14C=(void*)y->v;void*_tmp14D;_LLD9: if((int)_tmp14C
!= 0)goto _LLDB;_LLDA:(void*)(y->v=(void*)((void*)x->v));return;_LLDB: if(_tmp14C
<= (void*)1?1:*((int*)_tmp14C)!= 0)goto _LLDD;_tmp14D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14C)->f1;_LLDC: if(cmp(_tmp149,_tmp14D)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLDD: if(_tmp14C <= (void*)1?1:*((int*)
_tmp14C)!= 1)goto _LLD8;_LLDE:({void*_tmp14E[0]={};Cyc_Tcutil_impos(({const char*
_tmp14F="unify_conref: forward after compress(2)";_tag_arr(_tmp14F,sizeof(char),
_get_zero_arr_size(_tmp14F,40));}),_tag_arr(_tmp14E,sizeof(void*),0));});_LLD8:;}
_LLD6: if(_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 1)goto _LLD1;_LLD7:({void*
_tmp150[0]={};Cyc_Tcutil_impos(({const char*_tmp151="unify_conref: forward after compress";
_tag_arr(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,37));}),_tag_arr(
_tmp150,sizeof(void*),0));});_LLD1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp152;_push_handler(& _tmp152);{int _tmp154=0;if(setjmp(_tmp152.handler))
_tmp154=1;if(!_tmp154){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp155=1;_npop_handler(0);return _tmp155;};_pop_handler();}
else{void*_tmp153=(void*)_exn_thrown;void*_tmp157=_tmp153;_LLE0: if(_tmp157 != Cyc_Tcutil_Unify)
goto _LLE2;_LLE1: return 0;_LLE2:;_LLE3:(void)_throw(_tmp157);_LLDF:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp159=({struct _tuple6
_tmp158;_tmp158.f1=b1;_tmp158.f2=b2;_tmp158;});void*_tmp15A;void*_tmp15B;void*
_tmp15C;void*_tmp15D;void*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;void*_tmp160;
struct Cyc_Absyn_Exp*_tmp161;void*_tmp162;void*_tmp163;void*_tmp164;void*_tmp165;
void*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;_LLE5: _tmp15A=_tmp159.f1;if((
int)_tmp15A != 0)goto _LLE7;_tmp15B=_tmp159.f2;if((int)_tmp15B != 0)goto _LLE7;_LLE6:
return 0;_LLE7: _tmp15C=_tmp159.f1;if((int)_tmp15C != 0)goto _LLE9;_LLE8: return - 1;
_LLE9: _tmp15D=_tmp159.f2;if((int)_tmp15D != 0)goto _LLEB;_LLEA: return 1;_LLEB:
_tmp15E=_tmp159.f1;if(_tmp15E <= (void*)1?1:*((int*)_tmp15E)!= 0)goto _LLED;
_tmp15F=((struct Cyc_Absyn_Upper_b_struct*)_tmp15E)->f1;_tmp160=_tmp159.f2;if(
_tmp160 <= (void*)1?1:*((int*)_tmp160)!= 0)goto _LLED;_tmp161=((struct Cyc_Absyn_Upper_b_struct*)
_tmp160)->f1;_LLEC: return Cyc_Evexp_const_exp_cmp(_tmp15F,_tmp161);_LLED: _tmp162=
_tmp159.f1;if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 0)goto _LLEF;_tmp163=
_tmp159.f2;if(_tmp163 <= (void*)1?1:*((int*)_tmp163)!= 1)goto _LLEF;_LLEE: return - 1;
_LLEF: _tmp164=_tmp159.f1;if(_tmp164 <= (void*)1?1:*((int*)_tmp164)!= 1)goto _LLF1;
_tmp165=_tmp159.f2;if(_tmp165 <= (void*)1?1:*((int*)_tmp165)!= 0)goto _LLF1;_LLF0:
return 1;_LLF1: _tmp166=_tmp159.f1;if(_tmp166 <= (void*)1?1:*((int*)_tmp166)!= 1)
goto _LLE4;_tmp167=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp166)->f1;
_tmp168=_tmp159.f2;if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 1)goto _LLE4;
_tmp169=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp168)->f1;_LLF2: if(Cyc_Tcutil_unify(
_tmp167,_tmp169))return 0;return Cyc_Tcutil_typecmp(_tmp167,_tmp169);_LLE4:;}
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp16A=att;_LLF4: if(
_tmp16A <= (void*)17?1:*((int*)_tmp16A)!= 0)goto _LLF6;_LLF5: return 0;_LLF6: if((int)
_tmp16A != 0)goto _LLF8;_LLF7: return 1;_LLF8: if((int)_tmp16A != 1)goto _LLFA;_LLF9:
return 2;_LLFA: if((int)_tmp16A != 2)goto _LLFC;_LLFB: return 3;_LLFC: if((int)_tmp16A
!= 3)goto _LLFE;_LLFD: return 4;_LLFE: if((int)_tmp16A != 4)goto _LL100;_LLFF: return 5;
_LL100: if(_tmp16A <= (void*)17?1:*((int*)_tmp16A)!= 1)goto _LL102;_LL101: return 6;
_LL102: if((int)_tmp16A != 5)goto _LL104;_LL103: return 7;_LL104: if(_tmp16A <= (void*)
17?1:*((int*)_tmp16A)!= 2)goto _LL106;_LL105: return 8;_LL106: if((int)_tmp16A != 6)
goto _LL108;_LL107: return 9;_LL108: if((int)_tmp16A != 7)goto _LL10A;_LL109: return 10;
_LL10A: if((int)_tmp16A != 8)goto _LL10C;_LL10B: return 11;_LL10C: if((int)_tmp16A != 9)
goto _LL10E;_LL10D: return 12;_LL10E: if((int)_tmp16A != 10)goto _LL110;_LL10F: return
13;_LL110: if((int)_tmp16A != 11)goto _LL112;_LL111: return 14;_LL112: if((int)_tmp16A
!= 12)goto _LL114;_LL113: return 15;_LL114: if((int)_tmp16A != 13)goto _LL116;_LL115:
return 16;_LL116: if((int)_tmp16A != 14)goto _LL118;_LL117: return 17;_LL118: if((int)
_tmp16A != 15)goto _LL11A;_LL119: return 18;_LL11A: if(_tmp16A <= (void*)17?1:*((int*)
_tmp16A)!= 3)goto _LL11C;_LL11B: return 19;_LL11C: if(_tmp16A <= (void*)17?1:*((int*)
_tmp16A)!= 4)goto _LL11E;_LL11D: return 20;_LL11E:;_LL11F: return 21;_LLF3:;}static
int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct _tuple6 _tmp16C=({struct
_tuple6 _tmp16B;_tmp16B.f1=att1;_tmp16B.f2=att2;_tmp16B;});void*_tmp16D;int
_tmp16E;void*_tmp16F;int _tmp170;void*_tmp171;int _tmp172;void*_tmp173;int _tmp174;
void*_tmp175;int _tmp176;void*_tmp177;int _tmp178;void*_tmp179;struct _tagged_arr
_tmp17A;void*_tmp17B;struct _tagged_arr _tmp17C;void*_tmp17D;void*_tmp17E;int
_tmp17F;int _tmp180;void*_tmp181;void*_tmp182;int _tmp183;int _tmp184;_LL121:
_tmp16D=_tmp16C.f1;if(_tmp16D <= (void*)17?1:*((int*)_tmp16D)!= 0)goto _LL123;
_tmp16E=((struct Cyc_Absyn_Regparm_att_struct*)_tmp16D)->f1;_tmp16F=_tmp16C.f2;
if(_tmp16F <= (void*)17?1:*((int*)_tmp16F)!= 0)goto _LL123;_tmp170=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp16F)->f1;_LL122: _tmp172=_tmp16E;_tmp174=_tmp170;goto _LL124;_LL123: _tmp171=
_tmp16C.f1;if(_tmp171 <= (void*)17?1:*((int*)_tmp171)!= 4)goto _LL125;_tmp172=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp171)->f1;_tmp173=_tmp16C.f2;if(
_tmp173 <= (void*)17?1:*((int*)_tmp173)!= 4)goto _LL125;_tmp174=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp173)->f1;_LL124: _tmp176=_tmp172;_tmp178=_tmp174;goto _LL126;_LL125: _tmp175=
_tmp16C.f1;if(_tmp175 <= (void*)17?1:*((int*)_tmp175)!= 1)goto _LL127;_tmp176=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp175)->f1;_tmp177=_tmp16C.f2;if(_tmp177 <= (
void*)17?1:*((int*)_tmp177)!= 1)goto _LL127;_tmp178=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp177)->f1;_LL126: return Cyc_Core_intcmp(_tmp176,_tmp178);_LL127: _tmp179=
_tmp16C.f1;if(_tmp179 <= (void*)17?1:*((int*)_tmp179)!= 2)goto _LL129;_tmp17A=((
struct Cyc_Absyn_Section_att_struct*)_tmp179)->f1;_tmp17B=_tmp16C.f2;if(_tmp17B <= (
void*)17?1:*((int*)_tmp17B)!= 2)goto _LL129;_tmp17C=((struct Cyc_Absyn_Section_att_struct*)
_tmp17B)->f1;_LL128: return Cyc_strcmp((struct _tagged_arr)_tmp17A,(struct
_tagged_arr)_tmp17C);_LL129: _tmp17D=_tmp16C.f1;if(_tmp17D <= (void*)17?1:*((int*)
_tmp17D)!= 3)goto _LL12B;_tmp17E=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp17D)->f1;_tmp17F=((struct Cyc_Absyn_Format_att_struct*)_tmp17D)->f2;_tmp180=((
struct Cyc_Absyn_Format_att_struct*)_tmp17D)->f3;_tmp181=_tmp16C.f2;if(_tmp181 <= (
void*)17?1:*((int*)_tmp181)!= 3)goto _LL12B;_tmp182=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp181)->f1;_tmp183=((struct Cyc_Absyn_Format_att_struct*)_tmp181)->f2;_tmp184=((
struct Cyc_Absyn_Format_att_struct*)_tmp181)->f3;_LL12A: {int _tmp185=Cyc_Core_intcmp((
int)((unsigned int)_tmp17E),(int)((unsigned int)_tmp182));if(_tmp185 != 0)return
_tmp185;{int _tmp186=Cyc_Core_intcmp(_tmp17F,_tmp183);if(_tmp186 != 0)return
_tmp186;return Cyc_Core_intcmp(_tmp180,_tmp184);}}_LL12B:;_LL12C: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL120:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp187=Cyc_Tcutil_tvar_kind(tv);_LL12E:
if((int)_tmp187 != 3)goto _LL130;_LL12F: t=(void*)2;goto _LL12D;_LL130: if((int)
_tmp187 != 4)goto _LL132;_LL131: t=Cyc_Absyn_empty_effect;goto _LL12D;_LL132: if((int)
_tmp187 != 5)goto _LL134;_LL133: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp188=
_cycalloc_atomic(sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp189;_tmp189.tag=18;_tmp189.f1=0;_tmp189;});_tmp188;});goto _LL12D;_LL134:;
_LL135: t=Cyc_Absyn_sint_typ;goto _LL12D;_LL12D:;}return({struct _tuple8*_tmp18A=
_cycalloc(sizeof(*_tmp18A));_tmp18A->f1=tv;_tmp18A->f2=t;_tmp18A;});}static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp18B=Cyc_Tcutil_compress(t);void*_tmp18C;
struct Cyc_Absyn_TunionInfo _tmp18D;struct Cyc_List_List*_tmp18E;void*_tmp18F;
struct Cyc_Absyn_PtrInfo _tmp190;void*_tmp191;struct Cyc_Absyn_PtrAtts _tmp192;void*
_tmp193;struct Cyc_Absyn_ArrayInfo _tmp194;void*_tmp195;struct Cyc_List_List*
_tmp196;struct Cyc_Absyn_TunionFieldInfo _tmp197;struct Cyc_List_List*_tmp198;
struct Cyc_Absyn_AggrInfo _tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_List_List*
_tmp19B;void*_tmp19C;struct Cyc_Absyn_FnInfo _tmp19D;struct Cyc_List_List*_tmp19E;
struct Cyc_Core_Opt*_tmp19F;void*_tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_Absyn_VarargInfo*
_tmp1A2;struct Cyc_List_List*_tmp1A3;void*_tmp1A4;struct Cyc_List_List*_tmp1A5;
_LL137: if((int)_tmp18B != 0)goto _LL139;_LL138: goto _LL13A;_LL139: if((int)_tmp18B != 
1)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 
6)goto _LL13D;_LL13C: goto _LL13E;_LL13D: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 
12)goto _LL13F;_LL13E: goto _LL140;_LL13F: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)
!= 13)goto _LL141;_LL140: goto _LL142;_LL141: if(_tmp18B <= (void*)3?1:*((int*)
_tmp18B)!= 18)goto _LL143;_LL142: goto _LL144;_LL143: if(_tmp18B <= (void*)3?1:*((int*)
_tmp18B)!= 5)goto _LL145;_LL144: return Cyc_Absyn_empty_effect;_LL145: if(_tmp18B <= (
void*)3?1:*((int*)_tmp18B)!= 0)goto _LL147;_LL146: goto _LL148;_LL147: if(_tmp18B <= (
void*)3?1:*((int*)_tmp18B)!= 1)goto _LL149;_LL148: {void*_tmp1A6=Cyc_Tcutil_typ_kind(
t);_LL16A: if((int)_tmp1A6 != 3)goto _LL16C;_LL16B: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1A8;_tmp1A8.tag=19;_tmp1A8.f1=(void*)t;_tmp1A8;});_tmp1A7;});_LL16C: if((int)
_tmp1A6 != 4)goto _LL16E;_LL16D: return t;_LL16E: if((int)_tmp1A6 != 5)goto _LL170;
_LL16F: return Cyc_Absyn_empty_effect;_LL170:;_LL171: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1AA;_tmp1AA.tag=21;_tmp1AA.f1=(void*)t;_tmp1AA;});_tmp1A9;});_LL169:;}_LL149:
if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 15)goto _LL14B;_tmp18C=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp18B)->f1;_LL14A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1AC;_tmp1AC.tag=19;_tmp1AC.f1=(void*)_tmp18C;_tmp1AC;});_tmp1AB;});_LL14B:
if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 2)goto _LL14D;_tmp18D=((struct Cyc_Absyn_TunionType_struct*)
_tmp18B)->f1;_tmp18E=_tmp18D.targs;_tmp18F=(void*)_tmp18D.rgn;_LL14C: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));
_tmp1B0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1B1;_tmp1B1.tag=19;_tmp1B1.f1=(
void*)_tmp18F;_tmp1B1;});_tmp1B0;}));_tmp1AF->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp18E);_tmp1AF;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1AE;_tmp1AE.tag=20;_tmp1AE.f1=ts;_tmp1AE;});_tmp1AD;}));}_LL14D: if(_tmp18B <= (
void*)3?1:*((int*)_tmp18B)!= 4)goto _LL14F;_tmp190=((struct Cyc_Absyn_PointerType_struct*)
_tmp18B)->f1;_tmp191=(void*)_tmp190.elt_typ;_tmp192=_tmp190.ptr_atts;_tmp193=(
void*)_tmp192.rgn;_LL14E: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1B3;_tmp1B3.tag=20;_tmp1B3.f1=({void*_tmp1B4[2];_tmp1B4[1]=Cyc_Tcutil_rgns_of(
_tmp191);_tmp1B4[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1B5=_cycalloc(
sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1B6;_tmp1B6.tag=
19;_tmp1B6.f1=(void*)_tmp193;_tmp1B6;});_tmp1B5;});Cyc_List_list(_tag_arr(
_tmp1B4,sizeof(void*),2));});_tmp1B3;});_tmp1B2;}));_LL14F: if(_tmp18B <= (void*)3?
1:*((int*)_tmp18B)!= 7)goto _LL151;_tmp194=((struct Cyc_Absyn_ArrayType_struct*)
_tmp18B)->f1;_tmp195=(void*)_tmp194.elt_type;_LL150: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp195));_LL151: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 9)
goto _LL153;_tmp196=((struct Cyc_Absyn_TupleType_struct*)_tmp18B)->f1;_LL152: {
struct Cyc_List_List*_tmp1B7=0;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){_tmp1B7=({
struct Cyc_List_List*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->hd=(void*)(*((
struct _tuple4*)_tmp196->hd)).f2;_tmp1B8->tl=_tmp1B7;_tmp1B8;});}_tmp198=_tmp1B7;
goto _LL154;}_LL153: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 3)goto _LL155;
_tmp197=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp18B)->f1;_tmp198=_tmp197.targs;
_LL154: _tmp19A=_tmp198;goto _LL156;_LL155: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)
!= 10)goto _LL157;_tmp199=((struct Cyc_Absyn_AggrType_struct*)_tmp18B)->f1;_tmp19A=
_tmp199.targs;_LL156: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1BA;_tmp1BA.tag=20;_tmp1BA.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp19A);
_tmp1BA;});_tmp1B9;}));_LL157: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 11)goto
_LL159;_tmp19B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp18B)->f2;_LL158: return
Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1BB=
_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1BC;
_tmp1BC.tag=20;_tmp1BC.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp19B);_tmp1BC;});
_tmp1BB;}));_LL159: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 14)goto _LL15B;
_tmp19C=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp18B)->f1;_LL15A: return
Cyc_Tcutil_rgns_of(_tmp19C);_LL15B: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 17)
goto _LL15D;_LL15C: return Cyc_Tcutil_rgns_of(t);_LL15D: if(_tmp18B <= (void*)3?1:*((
int*)_tmp18B)!= 8)goto _LL15F;_tmp19D=((struct Cyc_Absyn_FnType_struct*)_tmp18B)->f1;
_tmp19E=_tmp19D.tvars;_tmp19F=_tmp19D.effect;_tmp1A0=(void*)_tmp19D.ret_typ;
_tmp1A1=_tmp19D.args;_tmp1A2=_tmp19D.cyc_varargs;_tmp1A3=_tmp19D.rgn_po;_LL15E: {
void*_tmp1BD=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp19E),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp19F))->v);return Cyc_Tcutil_normalize_effect(
_tmp1BD);}_LL15F: if((int)_tmp18B != 2)goto _LL161;_LL160: return Cyc_Absyn_empty_effect;
_LL161: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 19)goto _LL163;_LL162: goto
_LL164;_LL163: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 20)goto _LL165;_LL164:
return t;_LL165: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 21)goto _LL167;_tmp1A4=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp18B)->f1;_LL166: return Cyc_Tcutil_rgns_of(
_tmp1A4);_LL167: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 16)goto _LL136;_tmp1A5=((
struct Cyc_Absyn_TypedefType_struct*)_tmp18B)->f2;_LL168: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));
_tmp1BE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1BF;_tmp1BF.tag=20;_tmp1BF.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1A5);_tmp1BF;});_tmp1BE;}));_LL136:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1C0=e;struct Cyc_List_List*_tmp1C1;
struct Cyc_List_List**_tmp1C2;void*_tmp1C3;_LL173: if(_tmp1C0 <= (void*)3?1:*((int*)
_tmp1C0)!= 20)goto _LL175;_tmp1C1=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C0)->f1;
_tmp1C2=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp1C0)->f1;
_LL174: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp1C2;for(0;effs != 0;effs=
effs->tl){void*_tmp1C4=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp1C4)));{void*_tmp1C5=(void*)effs->hd;void*
_tmp1C6;_LL17A: if(_tmp1C5 <= (void*)3?1:*((int*)_tmp1C5)!= 20)goto _LL17C;_LL17B:
goto _LL17D;_LL17C: if(_tmp1C5 <= (void*)3?1:*((int*)_tmp1C5)!= 19)goto _LL17E;
_tmp1C6=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1C5)->f1;if((int)_tmp1C6
!= 2)goto _LL17E;_LL17D: redo_join=1;goto _LL179;_LL17E:;_LL17F: goto _LL179;_LL179:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp1C2;for(0;effs != 0;effs=effs->tl){void*_tmp1C7=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp1C8;void*_tmp1C9;_LL181: if(_tmp1C7 <= (void*)3?
1:*((int*)_tmp1C7)!= 20)goto _LL183;_tmp1C8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1C7)->f1;_LL182: effects=Cyc_List_revappend(_tmp1C8,effects);goto _LL180;
_LL183: if(_tmp1C7 <= (void*)3?1:*((int*)_tmp1C7)!= 19)goto _LL185;_tmp1C9=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1C7)->f1;if((int)_tmp1C9 != 2)goto _LL185;
_LL184: goto _LL180;_LL185:;_LL186: effects=({struct Cyc_List_List*_tmp1CA=_cycalloc(
sizeof(*_tmp1CA));_tmp1CA->hd=(void*)_tmp1C7;_tmp1CA->tl=effects;_tmp1CA;});goto
_LL180;_LL180:;}}*_tmp1C2=Cyc_List_imp_rev(effects);return e;}}_LL175: if(_tmp1C0
<= (void*)3?1:*((int*)_tmp1C0)!= 21)goto _LL177;_tmp1C3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1C0)->f1;_LL176: {void*_tmp1CB=Cyc_Tcutil_compress(_tmp1C3);_LL188: if(
_tmp1CB <= (void*)3?1:*((int*)_tmp1CB)!= 0)goto _LL18A;_LL189: goto _LL18B;_LL18A:
if(_tmp1CB <= (void*)3?1:*((int*)_tmp1CB)!= 1)goto _LL18C;_LL18B: return e;_LL18C:;
_LL18D: return Cyc_Tcutil_rgns_of(_tmp1C3);_LL187:;}_LL177:;_LL178: return e;_LL172:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp1CC=({struct Cyc_Absyn_FnType_struct*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Absyn_FnType_struct
_tmp1CE;_tmp1CE.tag=8;_tmp1CE.f1=({struct Cyc_Absyn_FnInfo _tmp1CF;_tmp1CF.tvars=0;
_tmp1CF.effect=({struct Cyc_Core_Opt*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->v=(
void*)eff;_tmp1D0;});_tmp1CF.ret_typ=(void*)((void*)0);_tmp1CF.args=0;_tmp1CF.c_varargs=
0;_tmp1CF.cyc_varargs=0;_tmp1CF.rgn_po=0;_tmp1CF.attributes=0;_tmp1CF;});_tmp1CE;});
_tmp1CD;});return Cyc_Absyn_atb_typ((void*)_tmp1CC,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*
_tmp1D1=Cyc_Tcutil_compress(e);void*_tmp1D2;struct Cyc_List_List*_tmp1D3;void*
_tmp1D4;struct Cyc_Core_Opt*_tmp1D5;struct Cyc_Core_Opt*_tmp1D6;struct Cyc_Core_Opt**
_tmp1D7;struct Cyc_Core_Opt*_tmp1D8;_LL18F: if(_tmp1D1 <= (void*)3?1:*((int*)
_tmp1D1)!= 19)goto _LL191;_tmp1D2=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1D1)->f1;_LL190: if(constrain)return Cyc_Tcutil_unify(r,_tmp1D2);_tmp1D2=Cyc_Tcutil_compress(
_tmp1D2);if(r == _tmp1D2)return 1;{struct _tuple6 _tmp1DA=({struct _tuple6 _tmp1D9;
_tmp1D9.f1=r;_tmp1D9.f2=_tmp1D2;_tmp1D9;});void*_tmp1DB;struct Cyc_Absyn_Tvar*
_tmp1DC;void*_tmp1DD;struct Cyc_Absyn_Tvar*_tmp1DE;_LL19A: _tmp1DB=_tmp1DA.f1;if(
_tmp1DB <= (void*)3?1:*((int*)_tmp1DB)!= 1)goto _LL19C;_tmp1DC=((struct Cyc_Absyn_VarType_struct*)
_tmp1DB)->f1;_tmp1DD=_tmp1DA.f2;if(_tmp1DD <= (void*)3?1:*((int*)_tmp1DD)!= 1)
goto _LL19C;_tmp1DE=((struct Cyc_Absyn_VarType_struct*)_tmp1DD)->f1;_LL19B: return
Cyc_Absyn_tvar_cmp(_tmp1DC,_tmp1DE)== 0;_LL19C:;_LL19D: return 0;_LL199:;}_LL191:
if(_tmp1D1 <= (void*)3?1:*((int*)_tmp1D1)!= 20)goto _LL193;_tmp1D3=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D1)->f1;_LL192: for(0;_tmp1D3 != 0;_tmp1D3=_tmp1D3->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1D3->hd))return 1;}return 0;_LL193: if(_tmp1D1 <= (void*)3?1:*((
int*)_tmp1D1)!= 21)goto _LL195;_tmp1D4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1D1)->f1;_LL194: {void*_tmp1DF=Cyc_Tcutil_rgns_of(_tmp1D4);void*_tmp1E0;
_LL19F: if(_tmp1DF <= (void*)3?1:*((int*)_tmp1DF)!= 21)goto _LL1A1;_tmp1E0=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1DF)->f1;_LL1A0: if(!constrain)return 0;{void*
_tmp1E1=Cyc_Tcutil_compress(_tmp1E0);struct Cyc_Core_Opt*_tmp1E2;struct Cyc_Core_Opt*
_tmp1E3;struct Cyc_Core_Opt**_tmp1E4;struct Cyc_Core_Opt*_tmp1E5;_LL1A4: if(_tmp1E1
<= (void*)3?1:*((int*)_tmp1E1)!= 0)goto _LL1A6;_tmp1E2=((struct Cyc_Absyn_Evar_struct*)
_tmp1E1)->f1;_tmp1E3=((struct Cyc_Absyn_Evar_struct*)_tmp1E1)->f2;_tmp1E4=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1E1)->f2;_tmp1E5=((struct Cyc_Absyn_Evar_struct*)
_tmp1E1)->f4;_LL1A5: {void*_tmp1E6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1E5);Cyc_Tcutil_occurs(_tmp1E6,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E5))->v,r);{void*_tmp1E7=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));
_tmp1E9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EA;_tmp1EA.tag=20;_tmp1EA.f1=({
void*_tmp1EB[2];_tmp1EB[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1EC=
_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1ED;
_tmp1ED.tag=19;_tmp1ED.f1=(void*)r;_tmp1ED;});_tmp1EC;});_tmp1EB[0]=_tmp1E6;Cyc_List_list(
_tag_arr(_tmp1EB,sizeof(void*),2));});_tmp1EA;});_tmp1E9;}));*_tmp1E4=({struct
Cyc_Core_Opt*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->v=(void*)_tmp1E7;
_tmp1E8;});return 1;}}_LL1A6:;_LL1A7: return 0;_LL1A3:;}_LL1A1:;_LL1A2: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1DF);_LL19E:;}_LL195: if(_tmp1D1 <= (void*)3?1:*((int*)_tmp1D1)!= 
0)goto _LL197;_tmp1D5=((struct Cyc_Absyn_Evar_struct*)_tmp1D1)->f1;_tmp1D6=((
struct Cyc_Absyn_Evar_struct*)_tmp1D1)->f2;_tmp1D7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1D1)->f2;_tmp1D8=((struct Cyc_Absyn_Evar_struct*)
_tmp1D1)->f4;_LL196: if(_tmp1D5 == 0?1:(void*)_tmp1D5->v != (void*)4)({void*_tmp1EE[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp1EF="effect evar has wrong kind";_tag_arr(_tmp1EF,sizeof(char),
_get_zero_arr_size(_tmp1EF,27));}),_tag_arr(_tmp1EE,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp1F0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1D8);Cyc_Tcutil_occurs(_tmp1F0,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D8))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1F1=({struct Cyc_Absyn_JoinEff_struct*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F4;_tmp1F4.tag=20;_tmp1F4.f1=({
struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=(void*)
_tmp1F0;_tmp1F5->tl=({struct Cyc_List_List*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));
_tmp1F6->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1F7=_cycalloc(
sizeof(*_tmp1F7));_tmp1F7[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1F8;_tmp1F8.tag=
19;_tmp1F8.f1=(void*)r;_tmp1F8;});_tmp1F7;}));_tmp1F6->tl=0;_tmp1F6;});_tmp1F5;});
_tmp1F4;});_tmp1F3;});*_tmp1D7=({struct Cyc_Core_Opt*_tmp1F2=_cycalloc(sizeof(*
_tmp1F2));_tmp1F2->v=(void*)((void*)_tmp1F1);_tmp1F2;});return 1;}}_LL197:;_LL198:
return 0;_LL18E:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1F9=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp1FA;void*_tmp1FB;struct Cyc_Core_Opt*_tmp1FC;struct
Cyc_Core_Opt*_tmp1FD;struct Cyc_Core_Opt**_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;
_LL1A9: if(_tmp1F9 <= (void*)3?1:*((int*)_tmp1F9)!= 19)goto _LL1AB;_LL1AA: return 0;
_LL1AB: if(_tmp1F9 <= (void*)3?1:*((int*)_tmp1F9)!= 20)goto _LL1AD;_tmp1FA=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1F9)->f1;_LL1AC: for(0;_tmp1FA != 0;_tmp1FA=_tmp1FA->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1FA->hd))return 1;}
return 0;_LL1AD: if(_tmp1F9 <= (void*)3?1:*((int*)_tmp1F9)!= 21)goto _LL1AF;_tmp1FB=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1F9)->f1;_LL1AE: _tmp1FB=Cyc_Tcutil_compress(
_tmp1FB);if(t == _tmp1FB)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp1FB);{void*_tmp200=Cyc_Tcutil_rgns_of(t);void*_tmp201;_LL1B4: if(_tmp200 <= (
void*)3?1:*((int*)_tmp200)!= 21)goto _LL1B6;_tmp201=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp200)->f1;_LL1B5: {struct _tuple6 _tmp203=({struct _tuple6 _tmp202;_tmp202.f1=Cyc_Tcutil_compress(
_tmp201);_tmp202.f2=_tmp1FB;_tmp202;});void*_tmp204;struct Cyc_Absyn_Tvar*_tmp205;
void*_tmp206;struct Cyc_Absyn_Tvar*_tmp207;_LL1B9: _tmp204=_tmp203.f1;if(_tmp204 <= (
void*)3?1:*((int*)_tmp204)!= 1)goto _LL1BB;_tmp205=((struct Cyc_Absyn_VarType_struct*)
_tmp204)->f1;_tmp206=_tmp203.f2;if(_tmp206 <= (void*)3?1:*((int*)_tmp206)!= 1)
goto _LL1BB;_tmp207=((struct Cyc_Absyn_VarType_struct*)_tmp206)->f1;_LL1BA: return
Cyc_Tcutil_unify(t,_tmp1FB);_LL1BB:;_LL1BC: return _tmp201 == _tmp1FB;_LL1B8:;}
_LL1B6:;_LL1B7: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp200);
_LL1B3:;}_LL1AF: if(_tmp1F9 <= (void*)3?1:*((int*)_tmp1F9)!= 0)goto _LL1B1;_tmp1FC=((
struct Cyc_Absyn_Evar_struct*)_tmp1F9)->f1;_tmp1FD=((struct Cyc_Absyn_Evar_struct*)
_tmp1F9)->f2;_tmp1FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1F9)->f2;_tmp1FF=((struct Cyc_Absyn_Evar_struct*)_tmp1F9)->f4;_LL1B0: if(
_tmp1FC == 0?1:(void*)_tmp1FC->v != (void*)4)({void*_tmp208[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp209="effect evar has wrong kind";
_tag_arr(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,27));}),_tag_arr(
_tmp208,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp20A=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1FF);Cyc_Tcutil_occurs(_tmp20A,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1FF))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp20B=({struct Cyc_Absyn_JoinEff_struct*_tmp20D=
_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp20E;
_tmp20E.tag=20;_tmp20E.f1=({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*
_tmp20F));_tmp20F->hd=(void*)_tmp20A;_tmp20F->tl=({struct Cyc_List_List*_tmp210=
_cycalloc(sizeof(*_tmp210));_tmp210->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp212;_tmp212.tag=21;_tmp212.f1=(void*)t;_tmp212;});_tmp211;}));_tmp210->tl=0;
_tmp210;});_tmp20F;});_tmp20E;});_tmp20D;});*_tmp1FE=({struct Cyc_Core_Opt*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->v=(void*)((void*)_tmp20B);_tmp20C;});
return 1;}}_LL1B1:;_LL1B2: return 0;_LL1A8:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp213=e;struct Cyc_Absyn_Tvar*_tmp214;struct Cyc_List_List*_tmp215;void*
_tmp216;struct Cyc_Core_Opt*_tmp217;struct Cyc_Core_Opt*_tmp218;struct Cyc_Core_Opt**
_tmp219;struct Cyc_Core_Opt*_tmp21A;_LL1BE: if(_tmp213 <= (void*)3?1:*((int*)
_tmp213)!= 1)goto _LL1C0;_tmp214=((struct Cyc_Absyn_VarType_struct*)_tmp213)->f1;
_LL1BF: return Cyc_Absyn_tvar_cmp(v,_tmp214)== 0;_LL1C0: if(_tmp213 <= (void*)3?1:*((
int*)_tmp213)!= 20)goto _LL1C2;_tmp215=((struct Cyc_Absyn_JoinEff_struct*)_tmp213)->f1;
_LL1C1: for(0;_tmp215 != 0;_tmp215=_tmp215->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp215->hd))return 1;}return 0;_LL1C2: if(_tmp213 <= (
void*)3?1:*((int*)_tmp213)!= 21)goto _LL1C4;_tmp216=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp213)->f1;_LL1C3: {void*_tmp21B=Cyc_Tcutil_rgns_of(_tmp216);void*_tmp21C;
_LL1C9: if(_tmp21B <= (void*)3?1:*((int*)_tmp21B)!= 21)goto _LL1CB;_tmp21C=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp21B)->f1;_LL1CA: if(!may_constrain_evars)
return 0;{void*_tmp21D=Cyc_Tcutil_compress(_tmp21C);struct Cyc_Core_Opt*_tmp21E;
struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**_tmp220;struct Cyc_Core_Opt*
_tmp221;_LL1CE: if(_tmp21D <= (void*)3?1:*((int*)_tmp21D)!= 0)goto _LL1D0;_tmp21E=((
struct Cyc_Absyn_Evar_struct*)_tmp21D)->f1;_tmp21F=((struct Cyc_Absyn_Evar_struct*)
_tmp21D)->f2;_tmp220=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp21D)->f2;_tmp221=((struct Cyc_Absyn_Evar_struct*)_tmp21D)->f4;_LL1CF: {void*
_tmp222=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp221);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp221))->v,v))return 0;{void*_tmp223=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp225=_cycalloc(sizeof(*_tmp225));
_tmp225[0]=({struct Cyc_Absyn_JoinEff_struct _tmp226;_tmp226.tag=20;_tmp226.f1=({
void*_tmp227[2];_tmp227[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp228=
_cycalloc(sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_VarType_struct _tmp229;
_tmp229.tag=1;_tmp229.f1=v;_tmp229;});_tmp228;});_tmp227[0]=_tmp222;Cyc_List_list(
_tag_arr(_tmp227,sizeof(void*),2));});_tmp226;});_tmp225;}));*_tmp220=({struct
Cyc_Core_Opt*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->v=(void*)_tmp223;
_tmp224;});return 1;}}_LL1D0:;_LL1D1: return 0;_LL1CD:;}_LL1CB:;_LL1CC: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp21B);_LL1C8:;}_LL1C4: if(_tmp213 <= (void*)3?1:*((int*)
_tmp213)!= 0)goto _LL1C6;_tmp217=((struct Cyc_Absyn_Evar_struct*)_tmp213)->f1;
_tmp218=((struct Cyc_Absyn_Evar_struct*)_tmp213)->f2;_tmp219=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp213)->f2;_tmp21A=((struct Cyc_Absyn_Evar_struct*)
_tmp213)->f4;_LL1C5: if(_tmp217 == 0?1:(void*)_tmp217->v != (void*)4)({void*_tmp22A[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp22B="effect evar has wrong kind";_tag_arr(_tmp22B,sizeof(char),
_get_zero_arr_size(_tmp22B,27));}),_tag_arr(_tmp22A,sizeof(void*),0));});{void*
_tmp22C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp21A);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp21A))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp22D=({struct Cyc_Absyn_JoinEff_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));
_tmp22F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp230;_tmp230.tag=20;_tmp230.f1=({
struct Cyc_List_List*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->hd=(void*)
_tmp22C;_tmp231->tl=({struct Cyc_List_List*_tmp232=_cycalloc(sizeof(*_tmp232));
_tmp232->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp233=_cycalloc(
sizeof(*_tmp233));_tmp233[0]=({struct Cyc_Absyn_VarType_struct _tmp234;_tmp234.tag=
1;_tmp234.f1=v;_tmp234;});_tmp233;}));_tmp232->tl=0;_tmp232;});_tmp231;});
_tmp230;});_tmp22F;});*_tmp219=({struct Cyc_Core_Opt*_tmp22E=_cycalloc(sizeof(*
_tmp22E));_tmp22E->v=(void*)((void*)_tmp22D);_tmp22E;});return 1;}}_LL1C6:;_LL1C7:
return 0;_LL1BD:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp235=e;struct Cyc_List_List*_tmp236;void*_tmp237;_LL1D3: if(_tmp235 <= (
void*)3?1:*((int*)_tmp235)!= 20)goto _LL1D5;_tmp236=((struct Cyc_Absyn_JoinEff_struct*)
_tmp235)->f1;_LL1D4: for(0;_tmp236 != 0;_tmp236=_tmp236->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp236->hd))return 1;}return 0;_LL1D5: if(_tmp235 <= (void*)3?1:*((int*)
_tmp235)!= 21)goto _LL1D7;_tmp237=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp235)->f1;_LL1D6: {void*_tmp238=Cyc_Tcutil_rgns_of(_tmp237);void*_tmp239;
_LL1DC: if(_tmp238 <= (void*)3?1:*((int*)_tmp238)!= 21)goto _LL1DE;_tmp239=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp238)->f1;_LL1DD: return 0;_LL1DE:;_LL1DF:
return Cyc_Tcutil_evar_in_effect(evar,_tmp238);_LL1DB:;}_LL1D7: if(_tmp235 <= (void*)
3?1:*((int*)_tmp235)!= 0)goto _LL1D9;_LL1D8: return evar == e;_LL1D9:;_LL1DA: return 0;
_LL1D2:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp23A=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp23B;void*_tmp23C;
struct Cyc_Absyn_Tvar*_tmp23D;void*_tmp23E;struct Cyc_Core_Opt*_tmp23F;struct Cyc_Core_Opt**
_tmp240;struct Cyc_Core_Opt*_tmp241;_LL1E1: if(_tmp23A <= (void*)3?1:*((int*)
_tmp23A)!= 20)goto _LL1E3;_tmp23B=((struct Cyc_Absyn_JoinEff_struct*)_tmp23A)->f1;
_LL1E2: for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp23B->hd,e2))return 0;}return 1;_LL1E3: if(_tmp23A <= (
void*)3?1:*((int*)_tmp23A)!= 19)goto _LL1E5;_tmp23C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp23A)->f1;_LL1E4: return Cyc_Tcutil_region_in_effect(0,_tmp23C,e2)?1:(
may_constrain_evars?Cyc_Tcutil_unify(_tmp23C,(void*)2): 0);_LL1E5: if(_tmp23A <= (
void*)3?1:*((int*)_tmp23A)!= 1)goto _LL1E7;_tmp23D=((struct Cyc_Absyn_VarType_struct*)
_tmp23A)->f1;_LL1E6: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp23D,e2);_LL1E7: if(_tmp23A <= (void*)3?1:*((int*)_tmp23A)!= 21)goto _LL1E9;
_tmp23E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp23A)->f1;_LL1E8: {void*
_tmp242=Cyc_Tcutil_rgns_of(_tmp23E);void*_tmp243;_LL1EE: if(_tmp242 <= (void*)3?1:*((
int*)_tmp242)!= 21)goto _LL1F0;_tmp243=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp242)->f1;_LL1EF: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp243,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp243,Cyc_Absyn_sint_typ): 0);
_LL1F0:;_LL1F1: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp242,e2);
_LL1ED:;}_LL1E9: if(_tmp23A <= (void*)3?1:*((int*)_tmp23A)!= 0)goto _LL1EB;_tmp23F=((
struct Cyc_Absyn_Evar_struct*)_tmp23A)->f2;_tmp240=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp23A)->f2;_tmp241=((struct Cyc_Absyn_Evar_struct*)
_tmp23A)->f4;_LL1EA: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp240=({struct Cyc_Core_Opt*
_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244->v=(void*)Cyc_Absyn_empty_effect;
_tmp244;});return 1;_LL1EB:;_LL1EC:({struct Cyc_String_pa_struct _tmp247;_tmp247.tag=
0;_tmp247.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp245[1]={& _tmp247};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp246="subset_effect: bad effect: %s";_tag_arr(
_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,30));}),_tag_arr(_tmp245,sizeof(
void*),1));}});_LL1E0:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(
0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)?Cyc_Tcutil_subset_effect(
1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple6 _tmp249;void*_tmp24A;void*_tmp24B;struct _tuple6*_tmp248=(struct
_tuple6*)r1->hd;_tmp249=*_tmp248;_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;{int found=
_tmp24A == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){
struct _tuple6 _tmp24D;void*_tmp24E;void*_tmp24F;struct _tuple6*_tmp24C=(struct
_tuple6*)r2->hd;_tmp24D=*_tmp24C;_tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;if(Cyc_Tcutil_unify(
_tmp24A,_tmp24E)?Cyc_Tcutil_unify(_tmp24B,_tmp24F): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp250=t1;struct Cyc_Core_Opt*_tmp251;struct Cyc_Core_Opt*
_tmp252;struct Cyc_Core_Opt**_tmp253;struct Cyc_Core_Opt*_tmp254;_LL1F3: if(_tmp250
<= (void*)3?1:*((int*)_tmp250)!= 0)goto _LL1F5;_tmp251=((struct Cyc_Absyn_Evar_struct*)
_tmp250)->f1;_tmp252=((struct Cyc_Absyn_Evar_struct*)_tmp250)->f2;_tmp253=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp250)->f2;_tmp254=((struct Cyc_Absyn_Evar_struct*)
_tmp250)->f4;_LL1F4: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp254))->v,t2);{void*_tmp255=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp255,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp251))->v)){*_tmp253=({struct Cyc_Core_Opt*_tmp256=_cycalloc(sizeof(*_tmp256));
_tmp256->v=(void*)t2;_tmp256;});return;}else{{void*_tmp257=t2;struct Cyc_Core_Opt*
_tmp258;struct Cyc_Core_Opt**_tmp259;struct Cyc_Core_Opt*_tmp25A;struct Cyc_Absyn_PtrInfo
_tmp25B;_LL1F8: if(_tmp257 <= (void*)3?1:*((int*)_tmp257)!= 0)goto _LL1FA;_tmp258=((
struct Cyc_Absyn_Evar_struct*)_tmp257)->f2;_tmp259=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp257)->f2;_tmp25A=((struct Cyc_Absyn_Evar_struct*)
_tmp257)->f4;_LL1F9: {struct Cyc_List_List*_tmp25C=(struct Cyc_List_List*)_tmp254->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp25A))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp25C,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp25D="(type variable would escape scope)";_tag_arr(_tmp25D,sizeof(
char),_get_zero_arr_size(_tmp25D,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp251->v,_tmp255)){*_tmp259=({struct Cyc_Core_Opt*
_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->v=(void*)t1;_tmp25E;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp25F="(kinds are incompatible)";_tag_arr(_tmp25F,sizeof(char),
_get_zero_arr_size(_tmp25F,25));});goto _LL1F7;}_LL1FA: if(_tmp257 <= (void*)3?1:*((
int*)_tmp257)!= 4)goto _LL1FC;_tmp25B=((struct Cyc_Absyn_PointerType_struct*)
_tmp257)->f1;if(!((void*)_tmp251->v == (void*)2))goto _LL1FC;_LL1FB: {struct Cyc_Absyn_Conref*
_tmp260=Cyc_Absyn_compress_conref((_tmp25B.ptr_atts).bounds);{void*_tmp261=(void*)
_tmp260->v;_LL1FF: if((int)_tmp261 != 0)goto _LL201;_LL200:(void*)(_tmp260->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp262=_cycalloc(sizeof(*_tmp262));
_tmp262[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp263;_tmp263.tag=0;_tmp263.f1=(
void*)Cyc_Absyn_bounds_one;_tmp263;});_tmp262;})));*_tmp253=({struct Cyc_Core_Opt*
_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264->v=(void*)t2;_tmp264;});return;
_LL201:;_LL202: goto _LL1FE;_LL1FE:;}goto _LL1F7;}_LL1FC:;_LL1FD: goto _LL1F7;_LL1F7:;}
Cyc_Tcutil_failure_reason=({const char*_tmp265="(kinds are incompatible)";
_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL1F5:;_LL1F6: goto _LL1F2;_LL1F2:;}{struct _tuple6
_tmp267=({struct _tuple6 _tmp266;_tmp266.f1=t2;_tmp266.f2=t1;_tmp266;});void*
_tmp268;void*_tmp269;void*_tmp26A;void*_tmp26B;struct Cyc_Absyn_Tvar*_tmp26C;void*
_tmp26D;struct Cyc_Absyn_Tvar*_tmp26E;void*_tmp26F;struct Cyc_Absyn_AggrInfo
_tmp270;void*_tmp271;struct Cyc_List_List*_tmp272;void*_tmp273;struct Cyc_Absyn_AggrInfo
_tmp274;void*_tmp275;struct Cyc_List_List*_tmp276;void*_tmp277;struct _tuple1*
_tmp278;void*_tmp279;struct _tuple1*_tmp27A;void*_tmp27B;struct Cyc_List_List*
_tmp27C;void*_tmp27D;struct Cyc_List_List*_tmp27E;void*_tmp27F;struct Cyc_Absyn_TunionInfo
_tmp280;void*_tmp281;struct Cyc_Absyn_Tuniondecl**_tmp282;struct Cyc_Absyn_Tuniondecl*
_tmp283;struct Cyc_List_List*_tmp284;void*_tmp285;void*_tmp286;struct Cyc_Absyn_TunionInfo
_tmp287;void*_tmp288;struct Cyc_Absyn_Tuniondecl**_tmp289;struct Cyc_Absyn_Tuniondecl*
_tmp28A;struct Cyc_List_List*_tmp28B;void*_tmp28C;void*_tmp28D;struct Cyc_Absyn_TunionFieldInfo
_tmp28E;void*_tmp28F;struct Cyc_Absyn_Tuniondecl*_tmp290;struct Cyc_Absyn_Tunionfield*
_tmp291;struct Cyc_List_List*_tmp292;void*_tmp293;struct Cyc_Absyn_TunionFieldInfo
_tmp294;void*_tmp295;struct Cyc_Absyn_Tuniondecl*_tmp296;struct Cyc_Absyn_Tunionfield*
_tmp297;struct Cyc_List_List*_tmp298;void*_tmp299;struct Cyc_Absyn_PtrInfo _tmp29A;
void*_tmp29B;struct Cyc_Absyn_Tqual _tmp29C;struct Cyc_Absyn_PtrAtts _tmp29D;void*
_tmp29E;struct Cyc_Absyn_Conref*_tmp29F;struct Cyc_Absyn_Conref*_tmp2A0;struct Cyc_Absyn_Conref*
_tmp2A1;void*_tmp2A2;struct Cyc_Absyn_PtrInfo _tmp2A3;void*_tmp2A4;struct Cyc_Absyn_Tqual
_tmp2A5;struct Cyc_Absyn_PtrAtts _tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Conref*
_tmp2A8;struct Cyc_Absyn_Conref*_tmp2A9;struct Cyc_Absyn_Conref*_tmp2AA;void*
_tmp2AB;void*_tmp2AC;void*_tmp2AD;void*_tmp2AE;void*_tmp2AF;void*_tmp2B0;void*
_tmp2B1;void*_tmp2B2;void*_tmp2B3;int _tmp2B4;void*_tmp2B5;int _tmp2B6;void*
_tmp2B7;void*_tmp2B8;void*_tmp2B9;void*_tmp2BA;void*_tmp2BB;int _tmp2BC;void*
_tmp2BD;int _tmp2BE;void*_tmp2BF;void*_tmp2C0;void*_tmp2C1;void*_tmp2C2;void*
_tmp2C3;struct Cyc_Absyn_ArrayInfo _tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Tqual
_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_Absyn_Conref*_tmp2C8;void*_tmp2C9;
struct Cyc_Absyn_ArrayInfo _tmp2CA;void*_tmp2CB;struct Cyc_Absyn_Tqual _tmp2CC;
struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Conref*_tmp2CE;void*_tmp2CF;struct
Cyc_Absyn_FnInfo _tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_Core_Opt*_tmp2D2;
void*_tmp2D3;struct Cyc_List_List*_tmp2D4;int _tmp2D5;struct Cyc_Absyn_VarargInfo*
_tmp2D6;struct Cyc_List_List*_tmp2D7;struct Cyc_List_List*_tmp2D8;void*_tmp2D9;
struct Cyc_Absyn_FnInfo _tmp2DA;struct Cyc_List_List*_tmp2DB;struct Cyc_Core_Opt*
_tmp2DC;void*_tmp2DD;struct Cyc_List_List*_tmp2DE;int _tmp2DF;struct Cyc_Absyn_VarargInfo*
_tmp2E0;struct Cyc_List_List*_tmp2E1;struct Cyc_List_List*_tmp2E2;void*_tmp2E3;
struct Cyc_List_List*_tmp2E4;void*_tmp2E5;struct Cyc_List_List*_tmp2E6;void*
_tmp2E7;void*_tmp2E8;struct Cyc_List_List*_tmp2E9;void*_tmp2EA;void*_tmp2EB;
struct Cyc_List_List*_tmp2EC;void*_tmp2ED;void*_tmp2EE;void*_tmp2EF;void*_tmp2F0;
void*_tmp2F1;void*_tmp2F2;void*_tmp2F3;void*_tmp2F4;void*_tmp2F5;void*_tmp2F6;
void*_tmp2F7;void*_tmp2F8;_LL204: _tmp268=_tmp267.f1;if(_tmp268 <= (void*)3?1:*((
int*)_tmp268)!= 0)goto _LL206;_LL205: Cyc_Tcutil_unify_it(t2,t1);return;_LL206:
_tmp269=_tmp267.f1;if((int)_tmp269 != 0)goto _LL208;_tmp26A=_tmp267.f2;if((int)
_tmp26A != 0)goto _LL208;_LL207: return;_LL208: _tmp26B=_tmp267.f1;if(_tmp26B <= (
void*)3?1:*((int*)_tmp26B)!= 1)goto _LL20A;_tmp26C=((struct Cyc_Absyn_VarType_struct*)
_tmp26B)->f1;_tmp26D=_tmp267.f2;if(_tmp26D <= (void*)3?1:*((int*)_tmp26D)!= 1)
goto _LL20A;_tmp26E=((struct Cyc_Absyn_VarType_struct*)_tmp26D)->f1;_LL209: {
struct _tagged_arr*_tmp2F9=_tmp26C->name;struct _tagged_arr*_tmp2FA=_tmp26E->name;
int _tmp2FB=*((int*)_check_null(_tmp26C->identity));int _tmp2FC=*((int*)
_check_null(_tmp26E->identity));void*_tmp2FD=Cyc_Tcutil_tvar_kind(_tmp26C);void*
_tmp2FE=Cyc_Tcutil_tvar_kind(_tmp26E);if(_tmp2FC == _tmp2FB?Cyc_strptrcmp(_tmp2F9,
_tmp2FA)== 0: 0){if(_tmp2FD != _tmp2FE)({struct Cyc_String_pa_struct _tmp303;_tmp303.tag=
0;_tmp303.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2FE));{struct Cyc_String_pa_struct _tmp302;_tmp302.tag=0;_tmp302.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp2FD));{struct Cyc_String_pa_struct
_tmp301;_tmp301.tag=0;_tmp301.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp2F9);{
void*_tmp2FF[3]={& _tmp301,& _tmp302,& _tmp303};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp300="same type variable %s has kinds %s and %s";
_tag_arr(_tmp300,sizeof(char),_get_zero_arr_size(_tmp300,42));}),_tag_arr(
_tmp2FF,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp304="(variable types are not the same)";_tag_arr(_tmp304,sizeof(char),
_get_zero_arr_size(_tmp304,34));});goto _LL203;}_LL20A: _tmp26F=_tmp267.f1;if(
_tmp26F <= (void*)3?1:*((int*)_tmp26F)!= 10)goto _LL20C;_tmp270=((struct Cyc_Absyn_AggrType_struct*)
_tmp26F)->f1;_tmp271=(void*)_tmp270.aggr_info;_tmp272=_tmp270.targs;_tmp273=
_tmp267.f2;if(_tmp273 <= (void*)3?1:*((int*)_tmp273)!= 10)goto _LL20C;_tmp274=((
struct Cyc_Absyn_AggrType_struct*)_tmp273)->f1;_tmp275=(void*)_tmp274.aggr_info;
_tmp276=_tmp274.targs;_LL20B: {void*_tmp306;struct _tuple1*_tmp307;struct _tuple5
_tmp305=Cyc_Absyn_aggr_kinded_name(_tmp275);_tmp306=_tmp305.f1;_tmp307=_tmp305.f2;{
void*_tmp309;struct _tuple1*_tmp30A;struct _tuple5 _tmp308=Cyc_Absyn_aggr_kinded_name(
_tmp271);_tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;if(_tmp306 != _tmp309){Cyc_Tcutil_failure_reason=({
const char*_tmp30B="(struct and union type)";_tag_arr(_tmp30B,sizeof(char),
_get_zero_arr_size(_tmp30B,24));});goto _LL203;}if(Cyc_Absyn_qvar_cmp(_tmp307,
_tmp30A)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp30C="(different type name)";
_tag_arr(_tmp30C,sizeof(char),_get_zero_arr_size(_tmp30C,22));});goto _LL203;}Cyc_Tcutil_unify_list(
_tmp276,_tmp272);return;}}_LL20C: _tmp277=_tmp267.f1;if(_tmp277 <= (void*)3?1:*((
int*)_tmp277)!= 12)goto _LL20E;_tmp278=((struct Cyc_Absyn_EnumType_struct*)_tmp277)->f1;
_tmp279=_tmp267.f2;if(_tmp279 <= (void*)3?1:*((int*)_tmp279)!= 12)goto _LL20E;
_tmp27A=((struct Cyc_Absyn_EnumType_struct*)_tmp279)->f1;_LL20D: if(Cyc_Absyn_qvar_cmp(
_tmp278,_tmp27A)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp30D="(different enum types)";
_tag_arr(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,23));});goto _LL203;
_LL20E: _tmp27B=_tmp267.f1;if(_tmp27B <= (void*)3?1:*((int*)_tmp27B)!= 13)goto
_LL210;_tmp27C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp27B)->f1;_tmp27D=
_tmp267.f2;if(_tmp27D <= (void*)3?1:*((int*)_tmp27D)!= 13)goto _LL210;_tmp27E=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp27D)->f1;_LL20F: {int bad=0;for(0;
_tmp27C != 0?_tmp27E != 0: 0;(_tmp27C=_tmp27C->tl,_tmp27E=_tmp27E->tl)){struct Cyc_Absyn_Enumfield*
_tmp30E=(struct Cyc_Absyn_Enumfield*)_tmp27C->hd;struct Cyc_Absyn_Enumfield*
_tmp30F=(struct Cyc_Absyn_Enumfield*)_tmp27E->hd;if(Cyc_Absyn_qvar_cmp(_tmp30E->name,
_tmp30F->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp310="(different names for enum fields)";
_tag_arr(_tmp310,sizeof(char),_get_zero_arr_size(_tmp310,34));});bad=1;break;}
if(_tmp30E->tag == _tmp30F->tag)continue;if(_tmp30E->tag == 0?1: _tmp30F->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp311="(different tag values for enum fields)";
_tag_arr(_tmp311,sizeof(char),_get_zero_arr_size(_tmp311,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp30E->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp30F->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp312="(different tag values for enum fields)";_tag_arr(_tmp312,
sizeof(char),_get_zero_arr_size(_tmp312,39));});bad=1;break;}}if(bad)goto _LL203;
if(_tmp27C == 0?_tmp27E == 0: 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp313="(different number of fields for enums)";_tag_arr(_tmp313,sizeof(char),
_get_zero_arr_size(_tmp313,39));});goto _LL203;}_LL210: _tmp27F=_tmp267.f1;if(
_tmp27F <= (void*)3?1:*((int*)_tmp27F)!= 2)goto _LL212;_tmp280=((struct Cyc_Absyn_TunionType_struct*)
_tmp27F)->f1;_tmp281=(void*)_tmp280.tunion_info;if(*((int*)_tmp281)!= 1)goto
_LL212;_tmp282=((struct Cyc_Absyn_KnownTunion_struct*)_tmp281)->f1;_tmp283=*
_tmp282;_tmp284=_tmp280.targs;_tmp285=(void*)_tmp280.rgn;_tmp286=_tmp267.f2;if(
_tmp286 <= (void*)3?1:*((int*)_tmp286)!= 2)goto _LL212;_tmp287=((struct Cyc_Absyn_TunionType_struct*)
_tmp286)->f1;_tmp288=(void*)_tmp287.tunion_info;if(*((int*)_tmp288)!= 1)goto
_LL212;_tmp289=((struct Cyc_Absyn_KnownTunion_struct*)_tmp288)->f1;_tmp28A=*
_tmp289;_tmp28B=_tmp287.targs;_tmp28C=(void*)_tmp287.rgn;_LL211: if(_tmp283 == 
_tmp28A?1: Cyc_Absyn_qvar_cmp(_tmp283->name,_tmp28A->name)== 0){Cyc_Tcutil_unify_it(
_tmp28C,_tmp285);Cyc_Tcutil_unify_list(_tmp28B,_tmp284);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp314="(different tunion types)";_tag_arr(_tmp314,sizeof(char),
_get_zero_arr_size(_tmp314,25));});goto _LL203;_LL212: _tmp28D=_tmp267.f1;if(
_tmp28D <= (void*)3?1:*((int*)_tmp28D)!= 3)goto _LL214;_tmp28E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp28D)->f1;_tmp28F=(void*)_tmp28E.field_info;if(*((int*)_tmp28F)!= 1)goto
_LL214;_tmp290=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp28F)->f1;_tmp291=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp28F)->f2;_tmp292=_tmp28E.targs;
_tmp293=_tmp267.f2;if(_tmp293 <= (void*)3?1:*((int*)_tmp293)!= 3)goto _LL214;
_tmp294=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp293)->f1;_tmp295=(void*)
_tmp294.field_info;if(*((int*)_tmp295)!= 1)goto _LL214;_tmp296=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp295)->f1;_tmp297=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp295)->f2;
_tmp298=_tmp294.targs;_LL213: if((_tmp290 == _tmp296?1: Cyc_Absyn_qvar_cmp(_tmp290->name,
_tmp296->name)== 0)?_tmp291 == _tmp297?1: Cyc_Absyn_qvar_cmp(_tmp291->name,_tmp297->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp298,_tmp292);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp315="(different tunion field types)";_tag_arr(_tmp315,sizeof(char),
_get_zero_arr_size(_tmp315,31));});goto _LL203;_LL214: _tmp299=_tmp267.f1;if(
_tmp299 <= (void*)3?1:*((int*)_tmp299)!= 4)goto _LL216;_tmp29A=((struct Cyc_Absyn_PointerType_struct*)
_tmp299)->f1;_tmp29B=(void*)_tmp29A.elt_typ;_tmp29C=_tmp29A.elt_tq;_tmp29D=
_tmp29A.ptr_atts;_tmp29E=(void*)_tmp29D.rgn;_tmp29F=_tmp29D.nullable;_tmp2A0=
_tmp29D.bounds;_tmp2A1=_tmp29D.zero_term;_tmp2A2=_tmp267.f2;if(_tmp2A2 <= (void*)
3?1:*((int*)_tmp2A2)!= 4)goto _LL216;_tmp2A3=((struct Cyc_Absyn_PointerType_struct*)
_tmp2A2)->f1;_tmp2A4=(void*)_tmp2A3.elt_typ;_tmp2A5=_tmp2A3.elt_tq;_tmp2A6=
_tmp2A3.ptr_atts;_tmp2A7=(void*)_tmp2A6.rgn;_tmp2A8=_tmp2A6.nullable;_tmp2A9=
_tmp2A6.bounds;_tmp2AA=_tmp2A6.zero_term;_LL215: Cyc_Tcutil_unify_it(_tmp2A4,
_tmp29B);Cyc_Tcutil_unify_it(_tmp29E,_tmp2A7);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2AA,
_tmp2A1,({const char*_tmp316="(not both zero terminated)";_tag_arr(_tmp316,
sizeof(char),_get_zero_arr_size(_tmp316,27));}));Cyc_Tcutil_unify_tqual(_tmp2A5,
_tmp2A4,_tmp29C,_tmp29B);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_boundscmp,
_tmp2A9,_tmp2A0,({const char*_tmp317="(different pointer bounds)";_tag_arr(
_tmp317,sizeof(char),_get_zero_arr_size(_tmp317,27));}));{void*_tmp318=(void*)(
Cyc_Absyn_compress_conref(_tmp2A9))->v;void*_tmp319;_LL23D: if(_tmp318 <= (void*)1?
1:*((int*)_tmp318)!= 0)goto _LL23F;_tmp319=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp318)->f1;if((int)_tmp319 != 0)goto _LL23F;_LL23E: return;_LL23F:;_LL240: goto
_LL23C;_LL23C:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2A8,
_tmp29F,({const char*_tmp31A="(incompatible pointer types)";_tag_arr(_tmp31A,
sizeof(char),_get_zero_arr_size(_tmp31A,29));}));return;_LL216: _tmp2AB=_tmp267.f1;
if(_tmp2AB <= (void*)3?1:*((int*)_tmp2AB)!= 5)goto _LL218;_tmp2AC=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2AB)->f1;_tmp2AD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2AB)->f2;_tmp2AE=_tmp267.f2;if(_tmp2AE <= (void*)3?1:*((int*)_tmp2AE)!= 5)
goto _LL218;_tmp2AF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2AE)->f1;_tmp2B0=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2AE)->f2;_LL217: if(_tmp2AF == _tmp2AC?
_tmp2B0 == _tmp2AD: 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp31B="(different integral types)";
_tag_arr(_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,27));});goto _LL203;
_LL218: _tmp2B1=_tmp267.f1;if((int)_tmp2B1 != 1)goto _LL21A;_tmp2B2=_tmp267.f2;if((
int)_tmp2B2 != 1)goto _LL21A;_LL219: return;_LL21A: _tmp2B3=_tmp267.f1;if(_tmp2B3 <= (
void*)3?1:*((int*)_tmp2B3)!= 6)goto _LL21C;_tmp2B4=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2B3)->f1;_tmp2B5=_tmp267.f2;if(_tmp2B5 <= (void*)3?1:*((int*)_tmp2B5)!= 6)
goto _LL21C;_tmp2B6=((struct Cyc_Absyn_DoubleType_struct*)_tmp2B5)->f1;_LL21B: if(
_tmp2B4 == _tmp2B6)return;goto _LL203;_LL21C: _tmp2B7=_tmp267.f1;if(_tmp2B7 <= (void*)
3?1:*((int*)_tmp2B7)!= 14)goto _LL21E;_tmp2B8=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2B7)->f1;_tmp2B9=_tmp267.f2;if(_tmp2B9 <= (void*)3?1:*((int*)_tmp2B9)!= 14)
goto _LL21E;_tmp2BA=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2B9)->f1;
_LL21D: Cyc_Tcutil_unify_it(_tmp2B8,_tmp2BA);return;_LL21E: _tmp2BB=_tmp267.f1;if(
_tmp2BB <= (void*)3?1:*((int*)_tmp2BB)!= 18)goto _LL220;_tmp2BC=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2BB)->f1;_tmp2BD=_tmp267.f2;if(_tmp2BD <= (void*)3?1:*((int*)_tmp2BD)!= 18)
goto _LL220;_tmp2BE=((struct Cyc_Absyn_TypeInt_struct*)_tmp2BD)->f1;_LL21F: if(
_tmp2BC == _tmp2BE)return;Cyc_Tcutil_failure_reason=({const char*_tmp31C="(different type integers)";
_tag_arr(_tmp31C,sizeof(char),_get_zero_arr_size(_tmp31C,26));});goto _LL203;
_LL220: _tmp2BF=_tmp267.f1;if(_tmp2BF <= (void*)3?1:*((int*)_tmp2BF)!= 17)goto
_LL222;_tmp2C0=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2BF)->f1;_tmp2C1=
_tmp267.f2;if(_tmp2C1 <= (void*)3?1:*((int*)_tmp2C1)!= 17)goto _LL222;_tmp2C2=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2C1)->f1;_LL221: Cyc_Tcutil_unify_it(
_tmp2C0,_tmp2C2);return;_LL222: _tmp2C3=_tmp267.f1;if(_tmp2C3 <= (void*)3?1:*((int*)
_tmp2C3)!= 7)goto _LL224;_tmp2C4=((struct Cyc_Absyn_ArrayType_struct*)_tmp2C3)->f1;
_tmp2C5=(void*)_tmp2C4.elt_type;_tmp2C6=_tmp2C4.tq;_tmp2C7=_tmp2C4.num_elts;
_tmp2C8=_tmp2C4.zero_term;_tmp2C9=_tmp267.f2;if(_tmp2C9 <= (void*)3?1:*((int*)
_tmp2C9)!= 7)goto _LL224;_tmp2CA=((struct Cyc_Absyn_ArrayType_struct*)_tmp2C9)->f1;
_tmp2CB=(void*)_tmp2CA.elt_type;_tmp2CC=_tmp2CA.tq;_tmp2CD=_tmp2CA.num_elts;
_tmp2CE=_tmp2CA.zero_term;_LL223: Cyc_Tcutil_unify_it(_tmp2CB,_tmp2C5);Cyc_Tcutil_unify_tqual(
_tmp2CC,_tmp2CB,_tmp2C6,_tmp2C5);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C8,
_tmp2CE,({const char*_tmp31D="(not both zero terminated)";_tag_arr(_tmp31D,
sizeof(char),_get_zero_arr_size(_tmp31D,27));}));if(_tmp2C7 == _tmp2CD)return;if(
_tmp2C7 == 0?1: _tmp2CD == 0)goto _LL203;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2C7,(struct Cyc_Absyn_Exp*)_tmp2CD))return;Cyc_Tcutil_failure_reason=({const
char*_tmp31E="(different array sizes)";_tag_arr(_tmp31E,sizeof(char),
_get_zero_arr_size(_tmp31E,24));});goto _LL203;_LL224: _tmp2CF=_tmp267.f1;if(
_tmp2CF <= (void*)3?1:*((int*)_tmp2CF)!= 8)goto _LL226;_tmp2D0=((struct Cyc_Absyn_FnType_struct*)
_tmp2CF)->f1;_tmp2D1=_tmp2D0.tvars;_tmp2D2=_tmp2D0.effect;_tmp2D3=(void*)_tmp2D0.ret_typ;
_tmp2D4=_tmp2D0.args;_tmp2D5=_tmp2D0.c_varargs;_tmp2D6=_tmp2D0.cyc_varargs;
_tmp2D7=_tmp2D0.rgn_po;_tmp2D8=_tmp2D0.attributes;_tmp2D9=_tmp267.f2;if(_tmp2D9
<= (void*)3?1:*((int*)_tmp2D9)!= 8)goto _LL226;_tmp2DA=((struct Cyc_Absyn_FnType_struct*)
_tmp2D9)->f1;_tmp2DB=_tmp2DA.tvars;_tmp2DC=_tmp2DA.effect;_tmp2DD=(void*)_tmp2DA.ret_typ;
_tmp2DE=_tmp2DA.args;_tmp2DF=_tmp2DA.c_varargs;_tmp2E0=_tmp2DA.cyc_varargs;
_tmp2E1=_tmp2DA.rgn_po;_tmp2E2=_tmp2DA.attributes;_LL225: {int done=0;{struct
_RegionHandle _tmp31F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp31F;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2DB != 0){if(_tmp2D1 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp320="(second function type has too few type variables)";
_tag_arr(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp321=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2DB->hd);void*_tmp322=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2D1->hd);
if(_tmp321 != _tmp322){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp327;_tmp327.tag=0;_tmp327.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp322));{struct Cyc_String_pa_struct _tmp326;_tmp326.tag=0;_tmp326.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp321));{struct Cyc_String_pa_struct
_tmp325;_tmp325.tag=0;_tmp325.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2DB->hd));{void*_tmp323[3]={& _tmp325,& _tmp326,& _tmp327};
Cyc_aprintf(({const char*_tmp324="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,44));}),_tag_arr(
_tmp323,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp328=_region_malloc(rgn,sizeof(*_tmp328));_tmp328->hd=({
struct _tuple8*_tmp329=_region_malloc(rgn,sizeof(*_tmp329));_tmp329->f1=(struct
Cyc_Absyn_Tvar*)_tmp2D1->hd;_tmp329->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A[0]=({struct Cyc_Absyn_VarType_struct
_tmp32B;_tmp32B.tag=1;_tmp32B.f1=(struct Cyc_Absyn_Tvar*)_tmp2DB->hd;_tmp32B;});
_tmp32A;});_tmp329;});_tmp328->tl=inst;_tmp328;});_tmp2DB=_tmp2DB->tl;_tmp2D1=
_tmp2D1->tl;}}if(_tmp2D1 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp32C="(second function type has too many type variables)";
_tag_arr(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,51));});_npop_handler(0);
goto _LL203;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({struct Cyc_Absyn_FnType_struct
_tmp32E;_tmp32E.tag=8;_tmp32E.f1=({struct Cyc_Absyn_FnInfo _tmp32F;_tmp32F.tvars=0;
_tmp32F.effect=_tmp2DC;_tmp32F.ret_typ=(void*)_tmp2DD;_tmp32F.args=_tmp2DE;
_tmp32F.c_varargs=_tmp2DF;_tmp32F.cyc_varargs=_tmp2E0;_tmp32F.rgn_po=_tmp2E1;
_tmp32F.attributes=_tmp2E2;_tmp32F;});_tmp32E;});_tmp32D;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp330=_cycalloc(sizeof(*
_tmp330));_tmp330[0]=({struct Cyc_Absyn_FnType_struct _tmp331;_tmp331.tag=8;
_tmp331.f1=({struct Cyc_Absyn_FnInfo _tmp332;_tmp332.tvars=0;_tmp332.effect=
_tmp2D2;_tmp332.ret_typ=(void*)_tmp2D3;_tmp332.args=_tmp2D4;_tmp332.c_varargs=
_tmp2D5;_tmp332.cyc_varargs=_tmp2D6;_tmp332.rgn_po=_tmp2D7;_tmp332.attributes=
_tmp2D8;_tmp332;});_tmp331;});_tmp330;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2DD,_tmp2D3);for(0;_tmp2DE != 0?_tmp2D4 != 0: 0;(
_tmp2DE=_tmp2DE->tl,_tmp2D4=_tmp2D4->tl)){struct Cyc_Absyn_Tqual _tmp334;void*
_tmp335;struct _tuple2 _tmp333=*((struct _tuple2*)_tmp2DE->hd);_tmp334=_tmp333.f2;
_tmp335=_tmp333.f3;{struct Cyc_Absyn_Tqual _tmp337;void*_tmp338;struct _tuple2
_tmp336=*((struct _tuple2*)_tmp2D4->hd);_tmp337=_tmp336.f2;_tmp338=_tmp336.f3;Cyc_Tcutil_unify_it(
_tmp335,_tmp338);Cyc_Tcutil_unify_tqual(_tmp334,_tmp335,_tmp337,_tmp338);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2DE != 0?1: _tmp2D4 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp339="(function types have different number of arguments)";_tag_arr(
_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,52));});goto _LL203;}if(_tmp2DF != 
_tmp2D5){Cyc_Tcutil_failure_reason=({const char*_tmp33A="(only one function type takes C varargs)";
_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,41));});goto _LL203;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp33C=({struct _tuple11 _tmp33B;_tmp33B.f1=
_tmp2E0;_tmp33B.f2=_tmp2D6;_tmp33B;});struct Cyc_Absyn_VarargInfo*_tmp33D;struct
Cyc_Absyn_VarargInfo*_tmp33E;struct Cyc_Absyn_VarargInfo*_tmp33F;struct Cyc_Absyn_VarargInfo*
_tmp340;struct Cyc_Absyn_VarargInfo*_tmp341;struct Cyc_Absyn_VarargInfo _tmp342;
struct Cyc_Core_Opt*_tmp343;struct Cyc_Absyn_Tqual _tmp344;void*_tmp345;int _tmp346;
struct Cyc_Absyn_VarargInfo*_tmp347;struct Cyc_Absyn_VarargInfo _tmp348;struct Cyc_Core_Opt*
_tmp349;struct Cyc_Absyn_Tqual _tmp34A;void*_tmp34B;int _tmp34C;_LL242: _tmp33D=
_tmp33C.f1;if(_tmp33D != 0)goto _LL244;_tmp33E=_tmp33C.f2;if(_tmp33E != 0)goto
_LL244;_LL243: goto _LL241;_LL244: _tmp33F=_tmp33C.f1;if(_tmp33F != 0)goto _LL246;
_LL245: goto _LL247;_LL246: _tmp340=_tmp33C.f2;if(_tmp340 != 0)goto _LL248;_LL247:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp34D="(only one function type takes varargs)";
_tag_arr(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,39));});goto _LL241;
_LL248: _tmp341=_tmp33C.f1;if(_tmp341 == 0)goto _LL241;_tmp342=*_tmp341;_tmp343=
_tmp342.name;_tmp344=_tmp342.tq;_tmp345=(void*)_tmp342.type;_tmp346=_tmp342.inject;
_tmp347=_tmp33C.f2;if(_tmp347 == 0)goto _LL241;_tmp348=*_tmp347;_tmp349=_tmp348.name;
_tmp34A=_tmp348.tq;_tmp34B=(void*)_tmp348.type;_tmp34C=_tmp348.inject;_LL249: Cyc_Tcutil_unify_it(
_tmp345,_tmp34B);Cyc_Tcutil_unify_tqual(_tmp344,_tmp345,_tmp34A,_tmp34B);if(
_tmp346 != _tmp34C){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*
_tmp34E="(only one function type injects varargs)";_tag_arr(_tmp34E,sizeof(char),
_get_zero_arr_size(_tmp34E,41));});}goto _LL241;_LL241:;}if(bad_cyc_vararg)goto
_LL203;{int bad_effect=0;{struct _tuple12 _tmp350=({struct _tuple12 _tmp34F;_tmp34F.f1=
_tmp2DC;_tmp34F.f2=_tmp2D2;_tmp34F;});struct Cyc_Core_Opt*_tmp351;struct Cyc_Core_Opt*
_tmp352;struct Cyc_Core_Opt*_tmp353;struct Cyc_Core_Opt*_tmp354;_LL24B: _tmp351=
_tmp350.f1;if(_tmp351 != 0)goto _LL24D;_tmp352=_tmp350.f2;if(_tmp352 != 0)goto
_LL24D;_LL24C: goto _LL24A;_LL24D: _tmp353=_tmp350.f1;if(_tmp353 != 0)goto _LL24F;
_LL24E: goto _LL250;_LL24F: _tmp354=_tmp350.f2;if(_tmp354 != 0)goto _LL251;_LL250:
bad_effect=1;goto _LL24A;_LL251:;_LL252: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2DC))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D2))->v);goto _LL24A;_LL24A:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp355="(function type effects do not unify)";
_tag_arr(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,37));});goto _LL203;}if(
!Cyc_Tcutil_same_atts(_tmp2D8,_tmp2E2)){Cyc_Tcutil_failure_reason=({const char*
_tmp356="(function types have different attributes)";_tag_arr(_tmp356,sizeof(
char),_get_zero_arr_size(_tmp356,43));});goto _LL203;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2D7,_tmp2E1)){Cyc_Tcutil_failure_reason=({const char*_tmp357="(function types have different region lifetime orderings)";
_tag_arr(_tmp357,sizeof(char),_get_zero_arr_size(_tmp357,58));});goto _LL203;}
return;}}}_LL226: _tmp2E3=_tmp267.f1;if(_tmp2E3 <= (void*)3?1:*((int*)_tmp2E3)!= 9)
goto _LL228;_tmp2E4=((struct Cyc_Absyn_TupleType_struct*)_tmp2E3)->f1;_tmp2E5=
_tmp267.f2;if(_tmp2E5 <= (void*)3?1:*((int*)_tmp2E5)!= 9)goto _LL228;_tmp2E6=((
struct Cyc_Absyn_TupleType_struct*)_tmp2E5)->f1;_LL227: for(0;_tmp2E6 != 0?_tmp2E4
!= 0: 0;(_tmp2E6=_tmp2E6->tl,_tmp2E4=_tmp2E4->tl)){struct Cyc_Absyn_Tqual _tmp359;
void*_tmp35A;struct _tuple4 _tmp358=*((struct _tuple4*)_tmp2E6->hd);_tmp359=_tmp358.f1;
_tmp35A=_tmp358.f2;{struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35D;struct _tuple4
_tmp35B=*((struct _tuple4*)_tmp2E4->hd);_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;Cyc_Tcutil_unify_it(
_tmp35A,_tmp35D);Cyc_Tcutil_unify_tqual(_tmp359,_tmp35A,_tmp35C,_tmp35D);}}if(
_tmp2E6 == 0?_tmp2E4 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp35E="(tuple types have different numbers of components)";
_tag_arr(_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,51));});goto _LL203;
_LL228: _tmp2E7=_tmp267.f1;if(_tmp2E7 <= (void*)3?1:*((int*)_tmp2E7)!= 11)goto
_LL22A;_tmp2E8=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2E7)->f1;
_tmp2E9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2E7)->f2;_tmp2EA=_tmp267.f2;
if(_tmp2EA <= (void*)3?1:*((int*)_tmp2EA)!= 11)goto _LL22A;_tmp2EB=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp2EA)->f1;_tmp2EC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2EA)->f2;_LL229: if(_tmp2EB != _tmp2E8){Cyc_Tcutil_failure_reason=({const char*
_tmp35F="(struct and union type)";_tag_arr(_tmp35F,sizeof(char),
_get_zero_arr_size(_tmp35F,24));});goto _LL203;}for(0;_tmp2EC != 0?_tmp2E9 != 0: 0;(
_tmp2EC=_tmp2EC->tl,_tmp2E9=_tmp2E9->tl)){struct Cyc_Absyn_Aggrfield*_tmp360=(
struct Cyc_Absyn_Aggrfield*)_tmp2EC->hd;struct Cyc_Absyn_Aggrfield*_tmp361=(struct
Cyc_Absyn_Aggrfield*)_tmp2E9->hd;if(Cyc_strptrcmp(_tmp360->name,_tmp361->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp362="(different member names)";
_tag_arr(_tmp362,sizeof(char),_get_zero_arr_size(_tmp362,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp360->type,(void*)_tmp361->type);
Cyc_Tcutil_unify_tqual(_tmp360->tq,(void*)_tmp360->type,_tmp361->tq,(void*)
_tmp361->type);if(!Cyc_Tcutil_same_atts(_tmp360->attributes,_tmp361->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp363="(different attributes on member)";_tag_arr(_tmp363,sizeof(
char),_get_zero_arr_size(_tmp363,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if(((_tmp360->width != 0?_tmp361->width == 0: 0)?1:(_tmp361->width != 0?_tmp360->width
== 0: 0))?1:((_tmp360->width != 0?_tmp361->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp360->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp361->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp364="(different bitfield widths on member)";
_tag_arr(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp2EC == 0?_tmp2E9 == 0: 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp365="(different number of members)";
_tag_arr(_tmp365,sizeof(char),_get_zero_arr_size(_tmp365,30));});goto _LL203;
_LL22A: _tmp2ED=_tmp267.f1;if((int)_tmp2ED != 2)goto _LL22C;_tmp2EE=_tmp267.f2;if((
int)_tmp2EE != 2)goto _LL22C;_LL22B: return;_LL22C: _tmp2EF=_tmp267.f1;if(_tmp2EF <= (
void*)3?1:*((int*)_tmp2EF)!= 15)goto _LL22E;_tmp2F0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp2EF)->f1;_tmp2F1=_tmp267.f2;if(_tmp2F1 <= (void*)3?1:*((int*)_tmp2F1)!= 15)
goto _LL22E;_tmp2F2=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F1)->f1;
_LL22D: Cyc_Tcutil_unify_it(_tmp2F0,_tmp2F2);return;_LL22E: _tmp2F3=_tmp267.f1;if(
_tmp2F3 <= (void*)3?1:*((int*)_tmp2F3)!= 20)goto _LL230;_LL22F: goto _LL231;_LL230:
_tmp2F4=_tmp267.f2;if(_tmp2F4 <= (void*)3?1:*((int*)_tmp2F4)!= 20)goto _LL232;
_LL231: goto _LL233;_LL232: _tmp2F5=_tmp267.f1;if(_tmp2F5 <= (void*)3?1:*((int*)
_tmp2F5)!= 19)goto _LL234;_LL233: goto _LL235;_LL234: _tmp2F6=_tmp267.f1;if(_tmp2F6
<= (void*)3?1:*((int*)_tmp2F6)!= 21)goto _LL236;_LL235: goto _LL237;_LL236: _tmp2F7=
_tmp267.f2;if(_tmp2F7 <= (void*)3?1:*((int*)_tmp2F7)!= 21)goto _LL238;_LL237: goto
_LL239;_LL238: _tmp2F8=_tmp267.f2;if(_tmp2F8 <= (void*)3?1:*((int*)_tmp2F8)!= 19)
goto _LL23A;_LL239: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp366="(effects don't unify)";_tag_arr(_tmp366,sizeof(char),
_get_zero_arr_size(_tmp366,22));});goto _LL203;_LL23A:;_LL23B: goto _LL203;_LL203:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0:
0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp367=(tq1.q_const + (tq1.q_volatile
<< 1))+ (tq1.q_restrict << 2);int _tmp368=(tq2.q_const + (tq2.q_volatile << 1))+ (
tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp367,_tmp368);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp369=(void*)x->v;
void*_tmp36A;_LL254: if((int)_tmp369 != 0)goto _LL256;_LL255: return - 1;_LL256: if(
_tmp369 <= (void*)1?1:*((int*)_tmp369)!= 0)goto _LL258;_tmp36A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp369)->f1;_LL257: {void*_tmp36B=(void*)y->v;void*_tmp36C;_LL25B: if((int)
_tmp36B != 0)goto _LL25D;_LL25C: return 1;_LL25D: if(_tmp36B <= (void*)1?1:*((int*)
_tmp36B)!= 0)goto _LL25F;_tmp36C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp36B)->f1;_LL25E: return cmp(_tmp36A,_tmp36C);_LL25F: if(_tmp36B <= (void*)1?1:*((
int*)_tmp36B)!= 1)goto _LL25A;_LL260:({void*_tmp36D[0]={};Cyc_Tcutil_impos(({
const char*_tmp36E="unify_conref: forward after compress(2)";_tag_arr(_tmp36E,
sizeof(char),_get_zero_arr_size(_tmp36E,40));}),_tag_arr(_tmp36D,sizeof(void*),0));});
_LL25A:;}_LL258: if(_tmp369 <= (void*)1?1:*((int*)_tmp369)!= 1)goto _LL253;_LL259:({
void*_tmp36F[0]={};Cyc_Tcutil_impos(({const char*_tmp370="unify_conref: forward after compress";
_tag_arr(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,37));}),_tag_arr(
_tmp36F,sizeof(void*),0));});_LL253:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp372;struct Cyc_Absyn_Tqual
_tmp373;void*_tmp374;struct _tuple4*_tmp371=tqt1;_tmp372=*_tmp371;_tmp373=_tmp372.f1;
_tmp374=_tmp372.f2;{struct _tuple4 _tmp376;struct Cyc_Absyn_Tqual _tmp377;void*
_tmp378;struct _tuple4*_tmp375=tqt2;_tmp376=*_tmp375;_tmp377=_tmp376.f1;_tmp378=
_tmp376.f2;{int _tmp379=Cyc_Tcutil_tqual_cmp(_tmp373,_tmp377);if(_tmp379 != 0)
return _tmp379;return Cyc_Tcutil_typecmp(_tmp374,_tmp378);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp37A=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp37A != 0)return _tmp37A;{int _tmp37B=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp37B != 0)return _tmp37B;{int _tmp37C=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp37C != 0)return _tmp37C;{int _tmp37D=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp37D != 0)return
_tmp37D;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp37E=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp37E != 0)return _tmp37E;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp37F=t;_LL262: if((int)_tmp37F != 0)goto _LL264;_LL263: return 0;
_LL264: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 0)goto _LL266;_LL265: return 1;
_LL266: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 1)goto _LL268;_LL267: return 2;
_LL268: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 2)goto _LL26A;_LL269: return 3;
_LL26A: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 3)goto _LL26C;_LL26B: return 4;
_LL26C: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 4)goto _LL26E;_LL26D: return 5;
_LL26E: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 5)goto _LL270;_LL26F: return 6;
_LL270: if((int)_tmp37F != 1)goto _LL272;_LL271: return 7;_LL272: if(_tmp37F <= (void*)
3?1:*((int*)_tmp37F)!= 6)goto _LL274;_LL273: return 8;_LL274: if(_tmp37F <= (void*)3?
1:*((int*)_tmp37F)!= 7)goto _LL276;_LL275: return 9;_LL276: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 8)goto _LL278;_LL277: return 10;_LL278: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 9)goto _LL27A;_LL279: return 11;_LL27A: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 10)goto _LL27C;_LL27B: return 12;_LL27C: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 11)goto _LL27E;_LL27D: return 14;_LL27E: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 12)goto _LL280;_LL27F: return 16;_LL280: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 13)goto _LL282;_LL281: return 17;_LL282: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 15)goto _LL284;_LL283: return 18;_LL284: if(_tmp37F <= (void*)3?1:*((
int*)_tmp37F)!= 16)goto _LL286;_LL285: return 19;_LL286: if((int)_tmp37F != 2)goto
_LL288;_LL287: return 20;_LL288: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 19)goto
_LL28A;_LL289: return 21;_LL28A: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 20)goto
_LL28C;_LL28B: return 22;_LL28C: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 21)goto
_LL28E;_LL28D: return 23;_LL28E: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 14)goto
_LL290;_LL28F: return 24;_LL290: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 18)goto
_LL292;_LL291: return 25;_LL292: if(_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 17)goto
_LL261;_LL293: return 26;_LL261:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp380=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp380 != 0)
return _tmp380;{struct _tuple6 _tmp382=({struct _tuple6 _tmp381;_tmp381.f1=t2;_tmp381.f2=
t1;_tmp381;});void*_tmp383;void*_tmp384;void*_tmp385;struct Cyc_Absyn_Tvar*
_tmp386;void*_tmp387;struct Cyc_Absyn_Tvar*_tmp388;void*_tmp389;struct Cyc_Absyn_AggrInfo
_tmp38A;void*_tmp38B;struct Cyc_List_List*_tmp38C;void*_tmp38D;struct Cyc_Absyn_AggrInfo
_tmp38E;void*_tmp38F;struct Cyc_List_List*_tmp390;void*_tmp391;struct _tuple1*
_tmp392;void*_tmp393;struct _tuple1*_tmp394;void*_tmp395;struct Cyc_List_List*
_tmp396;void*_tmp397;struct Cyc_List_List*_tmp398;void*_tmp399;struct Cyc_Absyn_TunionInfo
_tmp39A;void*_tmp39B;struct Cyc_Absyn_Tuniondecl**_tmp39C;struct Cyc_Absyn_Tuniondecl*
_tmp39D;struct Cyc_List_List*_tmp39E;void*_tmp39F;void*_tmp3A0;struct Cyc_Absyn_TunionInfo
_tmp3A1;void*_tmp3A2;struct Cyc_Absyn_Tuniondecl**_tmp3A3;struct Cyc_Absyn_Tuniondecl*
_tmp3A4;struct Cyc_List_List*_tmp3A5;void*_tmp3A6;void*_tmp3A7;struct Cyc_Absyn_TunionFieldInfo
_tmp3A8;void*_tmp3A9;struct Cyc_Absyn_Tuniondecl*_tmp3AA;struct Cyc_Absyn_Tunionfield*
_tmp3AB;struct Cyc_List_List*_tmp3AC;void*_tmp3AD;struct Cyc_Absyn_TunionFieldInfo
_tmp3AE;void*_tmp3AF;struct Cyc_Absyn_Tuniondecl*_tmp3B0;struct Cyc_Absyn_Tunionfield*
_tmp3B1;struct Cyc_List_List*_tmp3B2;void*_tmp3B3;struct Cyc_Absyn_PtrInfo _tmp3B4;
void*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B6;struct Cyc_Absyn_PtrAtts _tmp3B7;void*
_tmp3B8;struct Cyc_Absyn_Conref*_tmp3B9;struct Cyc_Absyn_Conref*_tmp3BA;struct Cyc_Absyn_Conref*
_tmp3BB;void*_tmp3BC;struct Cyc_Absyn_PtrInfo _tmp3BD;void*_tmp3BE;struct Cyc_Absyn_Tqual
_tmp3BF;struct Cyc_Absyn_PtrAtts _tmp3C0;void*_tmp3C1;struct Cyc_Absyn_Conref*
_tmp3C2;struct Cyc_Absyn_Conref*_tmp3C3;struct Cyc_Absyn_Conref*_tmp3C4;void*
_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*
_tmp3CB;void*_tmp3CC;void*_tmp3CD;int _tmp3CE;void*_tmp3CF;int _tmp3D0;void*
_tmp3D1;struct Cyc_Absyn_ArrayInfo _tmp3D2;void*_tmp3D3;struct Cyc_Absyn_Tqual
_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Conref*_tmp3D6;void*_tmp3D7;
struct Cyc_Absyn_ArrayInfo _tmp3D8;void*_tmp3D9;struct Cyc_Absyn_Tqual _tmp3DA;
struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Conref*_tmp3DC;void*_tmp3DD;struct
Cyc_Absyn_FnInfo _tmp3DE;struct Cyc_List_List*_tmp3DF;struct Cyc_Core_Opt*_tmp3E0;
void*_tmp3E1;struct Cyc_List_List*_tmp3E2;int _tmp3E3;struct Cyc_Absyn_VarargInfo*
_tmp3E4;struct Cyc_List_List*_tmp3E5;struct Cyc_List_List*_tmp3E6;void*_tmp3E7;
struct Cyc_Absyn_FnInfo _tmp3E8;struct Cyc_List_List*_tmp3E9;struct Cyc_Core_Opt*
_tmp3EA;void*_tmp3EB;struct Cyc_List_List*_tmp3EC;int _tmp3ED;struct Cyc_Absyn_VarargInfo*
_tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_List_List*_tmp3F0;void*_tmp3F1;
struct Cyc_List_List*_tmp3F2;void*_tmp3F3;struct Cyc_List_List*_tmp3F4;void*
_tmp3F5;void*_tmp3F6;struct Cyc_List_List*_tmp3F7;void*_tmp3F8;void*_tmp3F9;
struct Cyc_List_List*_tmp3FA;void*_tmp3FB;void*_tmp3FC;void*_tmp3FD;void*_tmp3FE;
void*_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;
void*_tmp405;void*_tmp406;void*_tmp407;int _tmp408;void*_tmp409;int _tmp40A;void*
_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;_LL295:
_tmp383=_tmp382.f1;if(_tmp383 <= (void*)3?1:*((int*)_tmp383)!= 0)goto _LL297;
_tmp384=_tmp382.f2;if(_tmp384 <= (void*)3?1:*((int*)_tmp384)!= 0)goto _LL297;
_LL296:({void*_tmp411[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp412="typecmp: can only compare closed types";
_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,39));}),_tag_arr(
_tmp411,sizeof(void*),0));});_LL297: _tmp385=_tmp382.f1;if(_tmp385 <= (void*)3?1:*((
int*)_tmp385)!= 1)goto _LL299;_tmp386=((struct Cyc_Absyn_VarType_struct*)_tmp385)->f1;
_tmp387=_tmp382.f2;if(_tmp387 <= (void*)3?1:*((int*)_tmp387)!= 1)goto _LL299;
_tmp388=((struct Cyc_Absyn_VarType_struct*)_tmp387)->f1;_LL298: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp388->identity)),*((int*)_check_null(_tmp386->identity)));
_LL299: _tmp389=_tmp382.f1;if(_tmp389 <= (void*)3?1:*((int*)_tmp389)!= 10)goto
_LL29B;_tmp38A=((struct Cyc_Absyn_AggrType_struct*)_tmp389)->f1;_tmp38B=(void*)
_tmp38A.aggr_info;_tmp38C=_tmp38A.targs;_tmp38D=_tmp382.f2;if(_tmp38D <= (void*)3?
1:*((int*)_tmp38D)!= 10)goto _LL29B;_tmp38E=((struct Cyc_Absyn_AggrType_struct*)
_tmp38D)->f1;_tmp38F=(void*)_tmp38E.aggr_info;_tmp390=_tmp38E.targs;_LL29A: {
struct _tuple1*_tmp414;struct _tuple5 _tmp413=Cyc_Absyn_aggr_kinded_name(_tmp38B);
_tmp414=_tmp413.f2;{struct _tuple1*_tmp416;struct _tuple5 _tmp415=Cyc_Absyn_aggr_kinded_name(
_tmp38F);_tmp416=_tmp415.f2;{int _tmp417=Cyc_Absyn_qvar_cmp(_tmp414,_tmp416);if(
_tmp417 != 0)return _tmp417;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp38C,_tmp390);}}}}_LL29B: _tmp391=_tmp382.f1;if(_tmp391 <= (void*)3?1:*((int*)
_tmp391)!= 12)goto _LL29D;_tmp392=((struct Cyc_Absyn_EnumType_struct*)_tmp391)->f1;
_tmp393=_tmp382.f2;if(_tmp393 <= (void*)3?1:*((int*)_tmp393)!= 12)goto _LL29D;
_tmp394=((struct Cyc_Absyn_EnumType_struct*)_tmp393)->f1;_LL29C: return Cyc_Absyn_qvar_cmp(
_tmp392,_tmp394);_LL29D: _tmp395=_tmp382.f1;if(_tmp395 <= (void*)3?1:*((int*)
_tmp395)!= 13)goto _LL29F;_tmp396=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp395)->f1;
_tmp397=_tmp382.f2;if(_tmp397 <= (void*)3?1:*((int*)_tmp397)!= 13)goto _LL29F;
_tmp398=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp397)->f1;_LL29E: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp396,
_tmp398);_LL29F: _tmp399=_tmp382.f1;if(_tmp399 <= (void*)3?1:*((int*)_tmp399)!= 2)
goto _LL2A1;_tmp39A=((struct Cyc_Absyn_TunionType_struct*)_tmp399)->f1;_tmp39B=(
void*)_tmp39A.tunion_info;if(*((int*)_tmp39B)!= 1)goto _LL2A1;_tmp39C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp39B)->f1;_tmp39D=*_tmp39C;_tmp39E=_tmp39A.targs;_tmp39F=(void*)_tmp39A.rgn;
_tmp3A0=_tmp382.f2;if(_tmp3A0 <= (void*)3?1:*((int*)_tmp3A0)!= 2)goto _LL2A1;
_tmp3A1=((struct Cyc_Absyn_TunionType_struct*)_tmp3A0)->f1;_tmp3A2=(void*)_tmp3A1.tunion_info;
if(*((int*)_tmp3A2)!= 1)goto _LL2A1;_tmp3A3=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3A2)->f1;_tmp3A4=*_tmp3A3;_tmp3A5=_tmp3A1.targs;_tmp3A6=(void*)_tmp3A1.rgn;
_LL2A0: if(_tmp3A4 == _tmp39D)return 0;{int _tmp418=Cyc_Absyn_qvar_cmp(_tmp3A4->name,
_tmp39D->name);if(_tmp418 != 0)return _tmp418;{int _tmp419=Cyc_Tcutil_typecmp(
_tmp3A6,_tmp39F);if(_tmp419 != 0)return _tmp419;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3A5,_tmp39E);}}_LL2A1: _tmp3A7=_tmp382.f1;if(_tmp3A7 <= (void*)3?1:*((int*)
_tmp3A7)!= 3)goto _LL2A3;_tmp3A8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3A7)->f1;_tmp3A9=(void*)_tmp3A8.field_info;if(*((int*)_tmp3A9)!= 1)goto
_LL2A3;_tmp3AA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3A9)->f1;_tmp3AB=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3A9)->f2;_tmp3AC=_tmp3A8.targs;
_tmp3AD=_tmp382.f2;if(_tmp3AD <= (void*)3?1:*((int*)_tmp3AD)!= 3)goto _LL2A3;
_tmp3AE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3AD)->f1;_tmp3AF=(void*)
_tmp3AE.field_info;if(*((int*)_tmp3AF)!= 1)goto _LL2A3;_tmp3B0=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3AF)->f1;_tmp3B1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3AF)->f2;
_tmp3B2=_tmp3AE.targs;_LL2A2: if(_tmp3B0 == _tmp3AA)return 0;{int _tmp41A=Cyc_Absyn_qvar_cmp(
_tmp3AA->name,_tmp3B0->name);if(_tmp41A != 0)return _tmp41A;{int _tmp41B=Cyc_Absyn_qvar_cmp(
_tmp3AB->name,_tmp3B1->name);if(_tmp41B != 0)return _tmp41B;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3B2,_tmp3AC);}}_LL2A3: _tmp3B3=_tmp382.f1;if(_tmp3B3 <= (
void*)3?1:*((int*)_tmp3B3)!= 4)goto _LL2A5;_tmp3B4=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B3)->f1;_tmp3B5=(void*)_tmp3B4.elt_typ;_tmp3B6=_tmp3B4.elt_tq;_tmp3B7=
_tmp3B4.ptr_atts;_tmp3B8=(void*)_tmp3B7.rgn;_tmp3B9=_tmp3B7.nullable;_tmp3BA=
_tmp3B7.bounds;_tmp3BB=_tmp3B7.zero_term;_tmp3BC=_tmp382.f2;if(_tmp3BC <= (void*)
3?1:*((int*)_tmp3BC)!= 4)goto _LL2A5;_tmp3BD=((struct Cyc_Absyn_PointerType_struct*)
_tmp3BC)->f1;_tmp3BE=(void*)_tmp3BD.elt_typ;_tmp3BF=_tmp3BD.elt_tq;_tmp3C0=
_tmp3BD.ptr_atts;_tmp3C1=(void*)_tmp3C0.rgn;_tmp3C2=_tmp3C0.nullable;_tmp3C3=
_tmp3C0.bounds;_tmp3C4=_tmp3C0.zero_term;_LL2A4: {int _tmp41C=Cyc_Tcutil_typecmp(
_tmp3BE,_tmp3B5);if(_tmp41C != 0)return _tmp41C;{int _tmp41D=Cyc_Tcutil_typecmp(
_tmp3C1,_tmp3B8);if(_tmp41D != 0)return _tmp41D;{int _tmp41E=Cyc_Tcutil_tqual_cmp(
_tmp3BF,_tmp3B6);if(_tmp41E != 0)return _tmp41E;{int _tmp41F=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3C3,_tmp3BA);if(_tmp41F != 0)return _tmp41F;{int _tmp420=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3C4,_tmp3BB);if(_tmp420 != 0)return _tmp420;{void*_tmp421=(
void*)(Cyc_Absyn_compress_conref(_tmp3C3))->v;void*_tmp422;_LL2CA: if(_tmp421 <= (
void*)1?1:*((int*)_tmp421)!= 0)goto _LL2CC;_tmp422=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp421)->f1;if((int)_tmp422 != 0)goto _LL2CC;_LL2CB: return 0;_LL2CC:;_LL2CD: goto
_LL2C9;_LL2C9:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3C2,
_tmp3B9);}}}}}_LL2A5: _tmp3C5=_tmp382.f1;if(_tmp3C5 <= (void*)3?1:*((int*)_tmp3C5)
!= 5)goto _LL2A7;_tmp3C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C5)->f1;
_tmp3C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C5)->f2;_tmp3C8=_tmp382.f2;
if(_tmp3C8 <= (void*)3?1:*((int*)_tmp3C8)!= 5)goto _LL2A7;_tmp3C9=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3C8)->f1;_tmp3CA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C8)->f2;_LL2A6: if(_tmp3C9 != _tmp3C6)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3C9),(int)((unsigned int)_tmp3C6));if(_tmp3CA != _tmp3C7)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3CA),(int)((unsigned int)_tmp3C7));
return 0;_LL2A7: _tmp3CB=_tmp382.f1;if((int)_tmp3CB != 1)goto _LL2A9;_tmp3CC=_tmp382.f2;
if((int)_tmp3CC != 1)goto _LL2A9;_LL2A8: return 0;_LL2A9: _tmp3CD=_tmp382.f1;if(
_tmp3CD <= (void*)3?1:*((int*)_tmp3CD)!= 6)goto _LL2AB;_tmp3CE=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3CD)->f1;_tmp3CF=_tmp382.f2;if(_tmp3CF <= (void*)3?1:*((int*)_tmp3CF)!= 6)
goto _LL2AB;_tmp3D0=((struct Cyc_Absyn_DoubleType_struct*)_tmp3CF)->f1;_LL2AA: if(
_tmp3CE == _tmp3D0)return 0;else{if(_tmp3CE)return - 1;else{return 1;}}_LL2AB: _tmp3D1=
_tmp382.f1;if(_tmp3D1 <= (void*)3?1:*((int*)_tmp3D1)!= 7)goto _LL2AD;_tmp3D2=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3D1)->f1;_tmp3D3=(void*)_tmp3D2.elt_type;
_tmp3D4=_tmp3D2.tq;_tmp3D5=_tmp3D2.num_elts;_tmp3D6=_tmp3D2.zero_term;_tmp3D7=
_tmp382.f2;if(_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 7)goto _LL2AD;_tmp3D8=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3D7)->f1;_tmp3D9=(void*)_tmp3D8.elt_type;
_tmp3DA=_tmp3D8.tq;_tmp3DB=_tmp3D8.num_elts;_tmp3DC=_tmp3D8.zero_term;_LL2AC: {
int _tmp423=Cyc_Tcutil_tqual_cmp(_tmp3DA,_tmp3D4);if(_tmp423 != 0)return _tmp423;{
int _tmp424=Cyc_Tcutil_typecmp(_tmp3D9,_tmp3D3);if(_tmp424 != 0)return _tmp424;{int
_tmp425=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3D6,_tmp3DC);if(_tmp425 != 0)
return _tmp425;if(_tmp3D5 == _tmp3DB)return 0;if(_tmp3D5 == 0?1: _tmp3DB == 0)({void*
_tmp426[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp427="missing expression in array index";_tag_arr(_tmp427,sizeof(
char),_get_zero_arr_size(_tmp427,34));}),_tag_arr(_tmp426,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3D5,
_tmp3DB);}}}_LL2AD: _tmp3DD=_tmp382.f1;if(_tmp3DD <= (void*)3?1:*((int*)_tmp3DD)!= 
8)goto _LL2AF;_tmp3DE=((struct Cyc_Absyn_FnType_struct*)_tmp3DD)->f1;_tmp3DF=
_tmp3DE.tvars;_tmp3E0=_tmp3DE.effect;_tmp3E1=(void*)_tmp3DE.ret_typ;_tmp3E2=
_tmp3DE.args;_tmp3E3=_tmp3DE.c_varargs;_tmp3E4=_tmp3DE.cyc_varargs;_tmp3E5=
_tmp3DE.rgn_po;_tmp3E6=_tmp3DE.attributes;_tmp3E7=_tmp382.f2;if(_tmp3E7 <= (void*)
3?1:*((int*)_tmp3E7)!= 8)goto _LL2AF;_tmp3E8=((struct Cyc_Absyn_FnType_struct*)
_tmp3E7)->f1;_tmp3E9=_tmp3E8.tvars;_tmp3EA=_tmp3E8.effect;_tmp3EB=(void*)_tmp3E8.ret_typ;
_tmp3EC=_tmp3E8.args;_tmp3ED=_tmp3E8.c_varargs;_tmp3EE=_tmp3E8.cyc_varargs;
_tmp3EF=_tmp3E8.rgn_po;_tmp3F0=_tmp3E8.attributes;_LL2AE:({void*_tmp428[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp429="typecmp: function types not handled";_tag_arr(_tmp429,sizeof(char),
_get_zero_arr_size(_tmp429,36));}),_tag_arr(_tmp428,sizeof(void*),0));});_LL2AF:
_tmp3F1=_tmp382.f1;if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 9)goto _LL2B1;
_tmp3F2=((struct Cyc_Absyn_TupleType_struct*)_tmp3F1)->f1;_tmp3F3=_tmp382.f2;if(
_tmp3F3 <= (void*)3?1:*((int*)_tmp3F3)!= 9)goto _LL2B1;_tmp3F4=((struct Cyc_Absyn_TupleType_struct*)
_tmp3F3)->f1;_LL2B0: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp3F4,_tmp3F2);_LL2B1: _tmp3F5=_tmp382.f1;if(_tmp3F5 <= (void*)3?1:*((int*)
_tmp3F5)!= 11)goto _LL2B3;_tmp3F6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3F5)->f1;_tmp3F7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3F5)->f2;_tmp3F8=
_tmp382.f2;if(_tmp3F8 <= (void*)3?1:*((int*)_tmp3F8)!= 11)goto _LL2B3;_tmp3F9=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3F8)->f1;_tmp3FA=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3F8)->f2;_LL2B2: if(_tmp3F9 != _tmp3F6){if(_tmp3F9 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp3FA,_tmp3F7);_LL2B3: _tmp3FB=_tmp382.f1;if(_tmp3FB <= (void*)3?1:*((int*)
_tmp3FB)!= 15)goto _LL2B5;_tmp3FC=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3FB)->f1;_tmp3FD=_tmp382.f2;if(_tmp3FD <= (void*)3?1:*((int*)_tmp3FD)!= 15)
goto _LL2B5;_tmp3FE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3FD)->f1;
_LL2B4: return Cyc_Tcutil_typecmp(_tmp3FC,_tmp3FE);_LL2B5: _tmp3FF=_tmp382.f1;if(
_tmp3FF <= (void*)3?1:*((int*)_tmp3FF)!= 14)goto _LL2B7;_tmp400=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp3FF)->f1;_tmp401=_tmp382.f2;if(_tmp401 <= (void*)3?1:*((int*)_tmp401)!= 14)
goto _LL2B7;_tmp402=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp401)->f1;
_LL2B6: return Cyc_Tcutil_typecmp(_tmp400,_tmp402);_LL2B7: _tmp403=_tmp382.f1;if(
_tmp403 <= (void*)3?1:*((int*)_tmp403)!= 17)goto _LL2B9;_tmp404=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp403)->f1;_tmp405=_tmp382.f2;if(_tmp405 <= (void*)3?1:*((int*)_tmp405)!= 17)
goto _LL2B9;_tmp406=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp405)->f1;_LL2B8:
return Cyc_Tcutil_typecmp(_tmp404,_tmp406);_LL2B9: _tmp407=_tmp382.f1;if(_tmp407 <= (
void*)3?1:*((int*)_tmp407)!= 18)goto _LL2BB;_tmp408=((struct Cyc_Absyn_TypeInt_struct*)
_tmp407)->f1;_tmp409=_tmp382.f2;if(_tmp409 <= (void*)3?1:*((int*)_tmp409)!= 18)
goto _LL2BB;_tmp40A=((struct Cyc_Absyn_TypeInt_struct*)_tmp409)->f1;_LL2BA: return
Cyc_Core_intcmp(_tmp408,_tmp40A);_LL2BB: _tmp40B=_tmp382.f1;if(_tmp40B <= (void*)3?
1:*((int*)_tmp40B)!= 20)goto _LL2BD;_LL2BC: goto _LL2BE;_LL2BD: _tmp40C=_tmp382.f2;
if(_tmp40C <= (void*)3?1:*((int*)_tmp40C)!= 20)goto _LL2BF;_LL2BE: goto _LL2C0;
_LL2BF: _tmp40D=_tmp382.f1;if(_tmp40D <= (void*)3?1:*((int*)_tmp40D)!= 19)goto
_LL2C1;_LL2C0: goto _LL2C2;_LL2C1: _tmp40E=_tmp382.f1;if(_tmp40E <= (void*)3?1:*((
int*)_tmp40E)!= 21)goto _LL2C3;_LL2C2: goto _LL2C4;_LL2C3: _tmp40F=_tmp382.f2;if(
_tmp40F <= (void*)3?1:*((int*)_tmp40F)!= 21)goto _LL2C5;_LL2C4: goto _LL2C6;_LL2C5:
_tmp410=_tmp382.f2;if(_tmp410 <= (void*)3?1:*((int*)_tmp410)!= 19)goto _LL2C7;
_LL2C6:({void*_tmp42A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp42B="typecmp: effects not handled";_tag_arr(
_tmp42B,sizeof(char),_get_zero_arr_size(_tmp42B,29));}),_tag_arr(_tmp42A,sizeof(
void*),0));});_LL2C7:;_LL2C8:({void*_tmp42C[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp42D="Unmatched case in typecmp";
_tag_arr(_tmp42D,sizeof(char),_get_zero_arr_size(_tmp42D,26));}),_tag_arr(
_tmp42C,sizeof(void*),0));});_LL294:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp42E=Cyc_Tcutil_compress(t);_LL2CF: if(_tmp42E <= (void*)3?1:*((int*)
_tmp42E)!= 5)goto _LL2D1;_LL2D0: goto _LL2D2;_LL2D1: if((int)_tmp42E != 1)goto _LL2D3;
_LL2D2: goto _LL2D4;_LL2D3: if(_tmp42E <= (void*)3?1:*((int*)_tmp42E)!= 6)goto _LL2D5;
_LL2D4: goto _LL2D6;_LL2D5: if(_tmp42E <= (void*)3?1:*((int*)_tmp42E)!= 12)goto
_LL2D7;_LL2D6: goto _LL2D8;_LL2D7: if(_tmp42E <= (void*)3?1:*((int*)_tmp42E)!= 13)
goto _LL2D9;_LL2D8: return 1;_LL2D9:;_LL2DA: return 0;_LL2CE:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp430=({struct _tuple6 _tmp42F;_tmp42F.f1=Cyc_Tcutil_compress(
t1);_tmp42F.f2=Cyc_Tcutil_compress(t2);_tmp42F;});void*_tmp431;int _tmp432;void*
_tmp433;int _tmp434;void*_tmp435;void*_tmp436;void*_tmp437;void*_tmp438;void*
_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;void*_tmp43E;void*
_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;void*_tmp443;void*_tmp444;void*
_tmp445;void*_tmp446;void*_tmp447;void*_tmp448;void*_tmp449;void*_tmp44A;void*
_tmp44B;void*_tmp44C;void*_tmp44D;void*_tmp44E;void*_tmp44F;void*_tmp450;void*
_tmp451;void*_tmp452;void*_tmp453;void*_tmp454;void*_tmp455;void*_tmp456;void*
_tmp457;void*_tmp458;void*_tmp459;void*_tmp45A;void*_tmp45B;void*_tmp45C;void*
_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*_tmp461;void*_tmp462;void*
_tmp463;_LL2DC: _tmp431=_tmp430.f1;if(_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 6)
goto _LL2DE;_tmp432=((struct Cyc_Absyn_DoubleType_struct*)_tmp431)->f1;_tmp433=
_tmp430.f2;if(_tmp433 <= (void*)3?1:*((int*)_tmp433)!= 6)goto _LL2DE;_tmp434=((
struct Cyc_Absyn_DoubleType_struct*)_tmp433)->f1;_LL2DD: return !_tmp434?_tmp432: 0;
_LL2DE: _tmp435=_tmp430.f1;if(_tmp435 <= (void*)3?1:*((int*)_tmp435)!= 6)goto
_LL2E0;_tmp436=_tmp430.f2;if((int)_tmp436 != 1)goto _LL2E0;_LL2DF: goto _LL2E1;
_LL2E0: _tmp437=_tmp430.f1;if(_tmp437 <= (void*)3?1:*((int*)_tmp437)!= 6)goto
_LL2E2;_tmp438=_tmp430.f2;if(_tmp438 <= (void*)3?1:*((int*)_tmp438)!= 5)goto
_LL2E2;_LL2E1: goto _LL2E3;_LL2E2: _tmp439=_tmp430.f1;if(_tmp439 <= (void*)3?1:*((
int*)_tmp439)!= 6)goto _LL2E4;_tmp43A=_tmp430.f2;if(_tmp43A <= (void*)3?1:*((int*)
_tmp43A)!= 14)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4: _tmp43B=_tmp430.f1;if((int)
_tmp43B != 1)goto _LL2E6;_tmp43C=_tmp430.f2;if(_tmp43C <= (void*)3?1:*((int*)
_tmp43C)!= 14)goto _LL2E6;_LL2E5: goto _LL2E7;_LL2E6: _tmp43D=_tmp430.f1;if(_tmp43D
<= (void*)3?1:*((int*)_tmp43D)!= 6)goto _LL2E8;_tmp43E=_tmp430.f2;if(_tmp43E <= (
void*)3?1:*((int*)_tmp43E)!= 17)goto _LL2E8;_LL2E7: goto _LL2E9;_LL2E8: _tmp43F=
_tmp430.f1;if((int)_tmp43F != 1)goto _LL2EA;_tmp440=_tmp430.f2;if(_tmp440 <= (void*)
3?1:*((int*)_tmp440)!= 17)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp441=_tmp430.f1;
if((int)_tmp441 != 1)goto _LL2EC;_tmp442=_tmp430.f2;if(_tmp442 <= (void*)3?1:*((int*)
_tmp442)!= 5)goto _LL2EC;_LL2EB: return 1;_LL2EC: _tmp443=_tmp430.f1;if(_tmp443 <= (
void*)3?1:*((int*)_tmp443)!= 5)goto _LL2EE;_tmp444=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp443)->f2;if((int)_tmp444 != 3)goto _LL2EE;_tmp445=_tmp430.f2;if(_tmp445 <= (
void*)3?1:*((int*)_tmp445)!= 5)goto _LL2EE;_tmp446=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp445)->f2;if((int)_tmp446 != 3)goto _LL2EE;_LL2ED: return 0;_LL2EE: _tmp447=
_tmp430.f1;if(_tmp447 <= (void*)3?1:*((int*)_tmp447)!= 5)goto _LL2F0;_tmp448=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp447)->f2;if((int)_tmp448 != 3)goto _LL2F0;
_LL2EF: goto _LL2F1;_LL2F0: _tmp449=_tmp430.f1;if(_tmp449 <= (void*)3?1:*((int*)
_tmp449)!= 5)goto _LL2F2;_tmp44A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp449)->f2;
if((int)_tmp44A != 2)goto _LL2F2;_tmp44B=_tmp430.f2;if((int)_tmp44B != 1)goto _LL2F2;
_LL2F1: goto _LL2F3;_LL2F2: _tmp44C=_tmp430.f1;if(_tmp44C <= (void*)3?1:*((int*)
_tmp44C)!= 5)goto _LL2F4;_tmp44D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp44C)->f2;
if((int)_tmp44D != 2)goto _LL2F4;_tmp44E=_tmp430.f2;if(_tmp44E <= (void*)3?1:*((int*)
_tmp44E)!= 5)goto _LL2F4;_tmp44F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp44E)->f2;
if((int)_tmp44F != 1)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: _tmp450=_tmp430.f1;if(
_tmp450 <= (void*)3?1:*((int*)_tmp450)!= 5)goto _LL2F6;_tmp451=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp450)->f2;if((int)_tmp451 != 2)goto _LL2F6;_tmp452=_tmp430.f2;if(_tmp452 <= (
void*)3?1:*((int*)_tmp452)!= 5)goto _LL2F6;_tmp453=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp452)->f2;if((int)_tmp453 != 0)goto _LL2F6;_LL2F5: goto _LL2F7;_LL2F6: _tmp454=
_tmp430.f1;if(_tmp454 <= (void*)3?1:*((int*)_tmp454)!= 5)goto _LL2F8;_tmp455=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp454)->f2;if((int)_tmp455 != 1)goto _LL2F8;
_tmp456=_tmp430.f2;if(_tmp456 <= (void*)3?1:*((int*)_tmp456)!= 5)goto _LL2F8;
_tmp457=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp456)->f2;if((int)_tmp457 != 
0)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: _tmp458=_tmp430.f1;if(_tmp458 <= (void*)3?1:*((
int*)_tmp458)!= 17)goto _LL2FA;_tmp459=_tmp430.f2;if(_tmp459 <= (void*)3?1:*((int*)
_tmp459)!= 5)goto _LL2FA;_tmp45A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp459)->f2;
if((int)_tmp45A != 1)goto _LL2FA;_LL2F9: goto _LL2FB;_LL2FA: _tmp45B=_tmp430.f1;if(
_tmp45B <= (void*)3?1:*((int*)_tmp45B)!= 17)goto _LL2FC;_tmp45C=_tmp430.f2;if(
_tmp45C <= (void*)3?1:*((int*)_tmp45C)!= 5)goto _LL2FC;_tmp45D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp45C)->f2;if((int)_tmp45D != 0)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: _tmp45E=
_tmp430.f1;if(_tmp45E <= (void*)3?1:*((int*)_tmp45E)!= 14)goto _LL2FE;_tmp45F=
_tmp430.f2;if(_tmp45F <= (void*)3?1:*((int*)_tmp45F)!= 5)goto _LL2FE;_tmp460=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp45F)->f2;if((int)_tmp460 != 1)goto _LL2FE;
_LL2FD: goto _LL2FF;_LL2FE: _tmp461=_tmp430.f1;if(_tmp461 <= (void*)3?1:*((int*)
_tmp461)!= 14)goto _LL300;_tmp462=_tmp430.f2;if(_tmp462 <= (void*)3?1:*((int*)
_tmp462)!= 5)goto _LL300;_tmp463=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp462)->f2;
if((int)_tmp463 != 0)goto _LL300;_LL2FF: return 1;_LL300:;_LL301: return 0;_LL2DB:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->v=(
void*)t1;_tmp464;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)
max_arith_type->v))return 0;}{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({struct Cyc_String_pa_struct
_tmp468;_tmp468.tag=0;_tmp468.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{
struct Cyc_String_pa_struct _tmp467;_tmp467.tag=0;_tmp467.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t));{void*_tmp465[2]={& _tmp467,& _tmp468};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp466="type mismatch: expecting %s but found %s";
_tag_arr(_tmp466,sizeof(char),_get_zero_arr_size(_tmp466,41));}),_tag_arr(
_tmp465,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp469=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL303: if(_tmp469 <= (void*)3?1:*((int*)_tmp469)!= 4)goto _LL305;_LL304: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL302;_LL305:;_LL306: return 0;_LL302:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp46A=Cyc_Tcutil_compress(
t);_LL308: if(_tmp46A <= (void*)3?1:*((int*)_tmp46A)!= 5)goto _LL30A;_LL309: goto
_LL30B;_LL30A: if(_tmp46A <= (void*)3?1:*((int*)_tmp46A)!= 14)goto _LL30C;_LL30B:
goto _LL30D;_LL30C: if(_tmp46A <= (void*)3?1:*((int*)_tmp46A)!= 17)goto _LL30E;
_LL30D: goto _LL30F;_LL30E: if(_tmp46A <= (void*)3?1:*((int*)_tmp46A)!= 12)goto
_LL310;_LL30F: goto _LL311;_LL310: if(_tmp46A <= (void*)3?1:*((int*)_tmp46A)!= 13)
goto _LL312;_LL311: return 1;_LL312:;_LL313: return 0;_LL307:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp46B[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp46C="integral size mismatch; conversion supplied";
_tag_arr(_tmp46C,sizeof(char),_get_zero_arr_size(_tmp46C,44));}),_tag_arr(
_tmp46B,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp46D[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp46E="integral size mismatch; conversion supplied";
_tag_arr(_tmp46E,sizeof(char),_get_zero_arr_size(_tmp46E,44));}),_tag_arr(
_tmp46D,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp470=({struct _tuple6 _tmp46F;_tmp46F.f1=
t1;_tmp46F.f2=t2;_tmp46F;});void*_tmp471;struct Cyc_Absyn_PtrInfo _tmp472;void*
_tmp473;struct Cyc_Absyn_PtrInfo _tmp474;void*_tmp475;struct Cyc_Absyn_ArrayInfo
_tmp476;void*_tmp477;struct Cyc_Absyn_Tqual _tmp478;struct Cyc_Absyn_Exp*_tmp479;
struct Cyc_Absyn_Conref*_tmp47A;void*_tmp47B;struct Cyc_Absyn_ArrayInfo _tmp47C;
void*_tmp47D;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Conref*
_tmp480;void*_tmp481;struct Cyc_Absyn_TunionFieldInfo _tmp482;void*_tmp483;struct
Cyc_Absyn_Tuniondecl*_tmp484;struct Cyc_Absyn_Tunionfield*_tmp485;struct Cyc_List_List*
_tmp486;void*_tmp487;struct Cyc_Absyn_TunionInfo _tmp488;void*_tmp489;struct Cyc_Absyn_Tuniondecl**
_tmp48A;struct Cyc_Absyn_Tuniondecl*_tmp48B;struct Cyc_List_List*_tmp48C;void*
_tmp48D;struct Cyc_Absyn_PtrInfo _tmp48E;void*_tmp48F;struct Cyc_Absyn_Tqual _tmp490;
struct Cyc_Absyn_PtrAtts _tmp491;void*_tmp492;struct Cyc_Absyn_Conref*_tmp493;
struct Cyc_Absyn_Conref*_tmp494;struct Cyc_Absyn_Conref*_tmp495;void*_tmp496;
struct Cyc_Absyn_TunionInfo _tmp497;void*_tmp498;struct Cyc_Absyn_Tuniondecl**
_tmp499;struct Cyc_Absyn_Tuniondecl*_tmp49A;struct Cyc_List_List*_tmp49B;void*
_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*_tmp4A1;_LL315:
_tmp471=_tmp470.f1;if(_tmp471 <= (void*)3?1:*((int*)_tmp471)!= 4)goto _LL317;
_tmp472=((struct Cyc_Absyn_PointerType_struct*)_tmp471)->f1;_tmp473=_tmp470.f2;
if(_tmp473 <= (void*)3?1:*((int*)_tmp473)!= 4)goto _LL317;_tmp474=((struct Cyc_Absyn_PointerType_struct*)
_tmp473)->f1;_LL316: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp472.ptr_atts).nullable,(
_tmp474.ptr_atts).nullable)){void*_tmp4A2=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp472.ptr_atts).nullable))->v;
int _tmp4A3;_LL324: if(_tmp4A2 <= (void*)1?1:*((int*)_tmp4A2)!= 0)goto _LL326;
_tmp4A3=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4A2)->f1;_LL325: okay=!
_tmp4A3;goto _LL323;_LL326:;_LL327:({void*_tmp4A4[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4A5="silent_castable conref not eq";
_tag_arr(_tmp4A5,sizeof(char),_get_zero_arr_size(_tmp4A5,30));}),_tag_arr(
_tmp4A4,sizeof(void*),0));});_LL323:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,(
_tmp472.ptr_atts).bounds,(_tmp474.ptr_atts).bounds)){struct _tuple6 _tmp4A7=({
struct _tuple6 _tmp4A6;_tmp4A6.f1=(void*)(Cyc_Absyn_compress_conref((_tmp472.ptr_atts).bounds))->v;
_tmp4A6.f2=(void*)(Cyc_Absyn_compress_conref((_tmp474.ptr_atts).bounds))->v;
_tmp4A6;});void*_tmp4A8;void*_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*_tmp4AC;
_LL329: _tmp4A8=_tmp4A7.f1;if(_tmp4A8 <= (void*)1?1:*((int*)_tmp4A8)!= 0)goto
_LL32B;_tmp4A9=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4A8)->f1;_tmp4AA=
_tmp4A7.f2;if(_tmp4AA <= (void*)1?1:*((int*)_tmp4AA)!= 0)goto _LL32B;_tmp4AB=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp4AA)->f1;_LL32A:{struct _tuple6 _tmp4AE=({
struct _tuple6 _tmp4AD;_tmp4AD.f1=_tmp4A9;_tmp4AD.f2=_tmp4AB;_tmp4AD;});void*
_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*_tmp4B2;void*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B4;void*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;void*_tmp4B7;void*_tmp4B8;
struct Cyc_Absyn_Exp*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;void*_tmp4BD;
void*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;
void*_tmp4C3;_LL330: _tmp4AF=_tmp4AE.f1;if(_tmp4AF <= (void*)1?1:*((int*)_tmp4AF)
!= 0)goto _LL332;_tmp4B0=_tmp4AE.f2;if((int)_tmp4B0 != 0)goto _LL332;_LL331: goto
_LL333;_LL332: _tmp4B1=_tmp4AE.f1;if((int)_tmp4B1 != 0)goto _LL334;_tmp4B2=_tmp4AE.f2;
if((int)_tmp4B2 != 0)goto _LL334;_LL333: okay=1;goto _LL32F;_LL334: _tmp4B3=_tmp4AE.f1;
if(_tmp4B3 <= (void*)1?1:*((int*)_tmp4B3)!= 0)goto _LL336;_tmp4B4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4B3)->f1;_tmp4B5=_tmp4AE.f2;if(_tmp4B5 <= (void*)1?1:*((int*)_tmp4B5)!= 0)
goto _LL336;_tmp4B6=((struct Cyc_Absyn_Upper_b_struct*)_tmp4B5)->f1;_LL335: okay=
okay?Cyc_Evexp_lte_const_exp(_tmp4B6,_tmp4B4): 0;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp474.ptr_atts).zero_term))({void*_tmp4C4[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4C5="implicit cast to shorter array";
_tag_arr(_tmp4C5,sizeof(char),_get_zero_arr_size(_tmp4C5,31));}),_tag_arr(
_tmp4C4,sizeof(void*),0));});goto _LL32F;_LL336: _tmp4B7=_tmp4AE.f1;if((int)
_tmp4B7 != 0)goto _LL338;_tmp4B8=_tmp4AE.f2;if(_tmp4B8 <= (void*)1?1:*((int*)
_tmp4B8)!= 0)goto _LL338;_tmp4B9=((struct Cyc_Absyn_Upper_b_struct*)_tmp4B8)->f1;
_LL337: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp472.ptr_atts).zero_term)?Cyc_Tcutil_is_bound_one((_tmp474.ptr_atts).bounds):
0)goto _LL32F;okay=0;goto _LL32F;_LL338: _tmp4BA=_tmp4AE.f1;if(_tmp4BA <= (void*)1?1:*((
int*)_tmp4BA)!= 1)goto _LL33A;_tmp4BB=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4BA)->f1;_tmp4BC=_tmp4AE.f2;if(_tmp4BC <= (void*)1?1:*((int*)_tmp4BC)!= 1)
goto _LL33A;_tmp4BD=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4BC)->f1;
_LL339: if(!Cyc_Tcutil_unify(_tmp4BB,_tmp4BD)){struct _tuple6 _tmp4C7=({struct
_tuple6 _tmp4C6;_tmp4C6.f1=Cyc_Tcutil_compress(_tmp4BB);_tmp4C6.f2=Cyc_Tcutil_compress(
_tmp4BD);_tmp4C6;});void*_tmp4C8;int _tmp4C9;void*_tmp4CA;int _tmp4CB;_LL341:
_tmp4C8=_tmp4C7.f1;if(_tmp4C8 <= (void*)3?1:*((int*)_tmp4C8)!= 18)goto _LL343;
_tmp4C9=((struct Cyc_Absyn_TypeInt_struct*)_tmp4C8)->f1;_tmp4CA=_tmp4C7.f2;if(
_tmp4CA <= (void*)3?1:*((int*)_tmp4CA)!= 18)goto _LL343;_tmp4CB=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4CA)->f1;_LL342: if(_tmp4C9 > _tmp4CB)okay=0;goto _LL340;_LL343:;_LL344: okay=0;
goto _LL340;_LL340:;}goto _LL32F;_LL33A: _tmp4BE=_tmp4AE.f1;if(_tmp4BE <= (void*)1?1:*((
int*)_tmp4BE)!= 0)goto _LL33C;_tmp4BF=((struct Cyc_Absyn_Upper_b_struct*)_tmp4BE)->f1;
_tmp4C0=_tmp4AE.f2;if(_tmp4C0 <= (void*)1?1:*((int*)_tmp4C0)!= 1)goto _LL33C;
_tmp4C1=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4C0)->f1;_LL33B: {
unsigned int _tmp4CD;int _tmp4CE;struct _tuple7 _tmp4CC=Cyc_Evexp_eval_const_uint_exp(
_tmp4BF);_tmp4CD=_tmp4CC.f1;_tmp4CE=_tmp4CC.f2;if(!_tmp4CE){okay=0;goto _LL32F;}{
void*_tmp4CF=Cyc_Tcutil_compress(_tmp4C1);int _tmp4D0;_LL346: if(_tmp4CF <= (void*)
3?1:*((int*)_tmp4CF)!= 18)goto _LL348;_tmp4D0=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4CF)->f1;_LL347: if(_tmp4CD > _tmp4D0)okay=0;goto _LL345;_LL348:;_LL349: okay=0;
goto _LL345;_LL345:;}goto _LL32F;}_LL33C: _tmp4C2=_tmp4AE.f1;if(_tmp4C2 <= (void*)1?
1:*((int*)_tmp4C2)!= 1)goto _LL33E;_LL33D: goto _LL33F;_LL33E: _tmp4C3=_tmp4AE.f2;
if(_tmp4C3 <= (void*)1?1:*((int*)_tmp4C3)!= 1)goto _LL32F;_LL33F: okay=0;goto _LL32F;
_LL32F:;}goto _LL328;_LL32B: _tmp4AC=_tmp4A7.f1;if((int)_tmp4AC != 0)goto _LL32D;
_LL32C: okay=0;goto _LL328;_LL32D:;_LL32E:({void*_tmp4D1[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4D2="silent_castable conrefs didn't unify";
_tag_arr(_tmp4D2,sizeof(char),_get_zero_arr_size(_tmp4D2,37));}),_tag_arr(
_tmp4D1,sizeof(void*),0));});_LL328:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp472.elt_typ,(
void*)_tmp474.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)(_tmp472.ptr_atts).rgn,(
void*)(_tmp474.ptr_atts).rgn)?1: Cyc_Tcenv_region_outlives(te,(void*)(_tmp472.ptr_atts).rgn,(
void*)(_tmp474.ptr_atts).rgn): 0;okay=okay?!(_tmp472.elt_tq).q_const?1:(_tmp474.elt_tq).q_const:
0;okay=okay?((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp472.ptr_atts).zero_term,(
_tmp474.ptr_atts).zero_term)?1:(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp472.ptr_atts).zero_term)?(_tmp474.elt_tq).q_const: 0): 0;return okay;}_LL317:
_tmp475=_tmp470.f1;if(_tmp475 <= (void*)3?1:*((int*)_tmp475)!= 7)goto _LL319;
_tmp476=((struct Cyc_Absyn_ArrayType_struct*)_tmp475)->f1;_tmp477=(void*)_tmp476.elt_type;
_tmp478=_tmp476.tq;_tmp479=_tmp476.num_elts;_tmp47A=_tmp476.zero_term;_tmp47B=
_tmp470.f2;if(_tmp47B <= (void*)3?1:*((int*)_tmp47B)!= 7)goto _LL319;_tmp47C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp47B)->f1;_tmp47D=(void*)_tmp47C.elt_type;
_tmp47E=_tmp47C.tq;_tmp47F=_tmp47C.num_elts;_tmp480=_tmp47C.zero_term;_LL318: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp47A,_tmp480)?(_tmp479 != 0?
_tmp47F != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp479,(struct Cyc_Absyn_Exp*)
_tmp47F): 0: 0;return(okay?Cyc_Tcutil_unify(_tmp477,_tmp47D): 0)?!_tmp478.q_const?1:
_tmp47E.q_const: 0;}_LL319: _tmp481=_tmp470.f1;if(_tmp481 <= (void*)3?1:*((int*)
_tmp481)!= 3)goto _LL31B;_tmp482=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp481)->f1;_tmp483=(void*)_tmp482.field_info;if(*((int*)_tmp483)!= 1)goto
_LL31B;_tmp484=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp483)->f1;_tmp485=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp483)->f2;_tmp486=_tmp482.targs;
_tmp487=_tmp470.f2;if(_tmp487 <= (void*)3?1:*((int*)_tmp487)!= 2)goto _LL31B;
_tmp488=((struct Cyc_Absyn_TunionType_struct*)_tmp487)->f1;_tmp489=(void*)_tmp488.tunion_info;
if(*((int*)_tmp489)!= 1)goto _LL31B;_tmp48A=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp489)->f1;_tmp48B=*_tmp48A;_tmp48C=_tmp488.targs;_LL31A: if((_tmp484 == _tmp48B?
1: Cyc_Absyn_qvar_cmp(_tmp484->name,_tmp48B->name)== 0)?_tmp485->typs == 0: 0){for(
0;_tmp486 != 0?_tmp48C != 0: 0;(_tmp486=_tmp486->tl,_tmp48C=_tmp48C->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp486->hd,(void*)_tmp48C->hd))break;}if(_tmp486 == 0?_tmp48C == 0: 0)return
1;}return 0;_LL31B: _tmp48D=_tmp470.f1;if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 
4)goto _LL31D;_tmp48E=((struct Cyc_Absyn_PointerType_struct*)_tmp48D)->f1;_tmp48F=(
void*)_tmp48E.elt_typ;_tmp490=_tmp48E.elt_tq;_tmp491=_tmp48E.ptr_atts;_tmp492=(
void*)_tmp491.rgn;_tmp493=_tmp491.nullable;_tmp494=_tmp491.bounds;_tmp495=
_tmp491.zero_term;_tmp496=_tmp470.f2;if(_tmp496 <= (void*)3?1:*((int*)_tmp496)!= 
2)goto _LL31D;_tmp497=((struct Cyc_Absyn_TunionType_struct*)_tmp496)->f1;_tmp498=(
void*)_tmp497.tunion_info;if(*((int*)_tmp498)!= 1)goto _LL31D;_tmp499=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp498)->f1;_tmp49A=*_tmp499;_tmp49B=_tmp497.targs;_tmp49C=(void*)_tmp497.rgn;
_LL31C:{void*_tmp4D3=Cyc_Tcutil_compress(_tmp48F);struct Cyc_Absyn_TunionFieldInfo
_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_Tuniondecl*_tmp4D6;struct Cyc_Absyn_Tunionfield*
_tmp4D7;struct Cyc_List_List*_tmp4D8;_LL34B: if(_tmp4D3 <= (void*)3?1:*((int*)
_tmp4D3)!= 3)goto _LL34D;_tmp4D4=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4D3)->f1;_tmp4D5=(void*)_tmp4D4.field_info;if(*((int*)_tmp4D5)!= 1)goto
_LL34D;_tmp4D6=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4D5)->f1;_tmp4D7=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4D5)->f2;_tmp4D8=_tmp4D4.targs;
_LL34C: if(!Cyc_Tcutil_unify(_tmp492,_tmp49C)?!Cyc_Tcenv_region_outlives(te,
_tmp492,_tmp49C): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp493,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp494,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp495,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp49A->name,
_tmp4D6->name)== 0?_tmp4D7->typs != 0: 0){int okay=1;for(0;_tmp4D8 != 0?_tmp49B != 0:
0;(_tmp4D8=_tmp4D8->tl,_tmp49B=_tmp49B->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4D8->hd,(
void*)_tmp49B->hd)){okay=0;break;}}if((!okay?1: _tmp4D8 != 0)?1: _tmp49B != 0)return
0;return 1;}goto _LL34A;_LL34D:;_LL34E: goto _LL34A;_LL34A:;}return 0;_LL31D: _tmp49D=
_tmp470.f1;if(_tmp49D <= (void*)3?1:*((int*)_tmp49D)!= 14)goto _LL31F;_tmp49E=
_tmp470.f2;if(_tmp49E <= (void*)3?1:*((int*)_tmp49E)!= 5)goto _LL31F;_tmp49F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp49E)->f2;if((int)_tmp49F != 2)goto _LL31F;
_LL31E: goto _LL320;_LL31F: _tmp4A0=_tmp470.f1;if(_tmp4A0 <= (void*)3?1:*((int*)
_tmp4A0)!= 17)goto _LL321;_tmp4A1=_tmp470.f2;if(_tmp4A1 <= (void*)3?1:*((int*)
_tmp4A1)!= 5)goto _LL321;_LL320: return 1;_LL321:;_LL322: return Cyc_Tcutil_unify(t1,
t2);_LL314:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4D9=Cyc_Tcutil_compress(
t);_LL350: if(_tmp4D9 <= (void*)3?1:*((int*)_tmp4D9)!= 4)goto _LL352;_LL351: return 1;
_LL352:;_LL353: return 0;_LL34F:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4DA=(void*)e->r;void*_tmp4DB;int _tmp4DC;void*_tmp4DD;char _tmp4DE;void*
_tmp4DF;short _tmp4E0;void*_tmp4E1;long long _tmp4E2;void*_tmp4E3;struct Cyc_Absyn_Exp*
_tmp4E4;_LL355: if(*((int*)_tmp4DA)!= 0)goto _LL357;_tmp4DB=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4DA)->f1;if(_tmp4DB <= (void*)1?1:*((int*)_tmp4DB)!= 2)goto _LL357;_tmp4DC=((
struct Cyc_Absyn_Int_c_struct*)_tmp4DB)->f2;if(_tmp4DC != 0)goto _LL357;_LL356: goto
_LL358;_LL357: if(*((int*)_tmp4DA)!= 0)goto _LL359;_tmp4DD=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4DA)->f1;if(_tmp4DD <= (void*)1?1:*((int*)_tmp4DD)!= 0)goto _LL359;_tmp4DE=((
struct Cyc_Absyn_Char_c_struct*)_tmp4DD)->f2;if(_tmp4DE != 0)goto _LL359;_LL358:
goto _LL35A;_LL359: if(*((int*)_tmp4DA)!= 0)goto _LL35B;_tmp4DF=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4DA)->f1;if(_tmp4DF <= (void*)1?1:*((int*)_tmp4DF)!= 1)goto _LL35B;_tmp4E0=((
struct Cyc_Absyn_Short_c_struct*)_tmp4DF)->f2;if(_tmp4E0 != 0)goto _LL35B;_LL35A:
goto _LL35C;_LL35B: if(*((int*)_tmp4DA)!= 0)goto _LL35D;_tmp4E1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp4DA)->f1;if(_tmp4E1 <= (void*)1?1:*((int*)_tmp4E1)!= 3)goto _LL35D;_tmp4E2=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp4E1)->f2;if(_tmp4E2 != 0)goto _LL35D;_LL35C:
return 1;_LL35D: if(*((int*)_tmp4DA)!= 13)goto _LL35F;_tmp4E3=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4DA)->f1;_tmp4E4=((struct Cyc_Absyn_Cast_e_struct*)_tmp4DA)->f2;_LL35E: return
Cyc_Tcutil_is_zero(_tmp4E4)?Cyc_Tcutil_admits_zero(_tmp4E3): 0;_LL35F:;_LL360:
return 0;_LL354:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5[0]=({struct Cyc_Absyn_Const_e_struct
_tmp4E6;_tmp4E6.tag=0;_tmp4E6.f1=(void*)((void*)0);_tmp4E6;});_tmp4E5;})));{
struct Cyc_List_List*_tmp4E7=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp4E8=({struct Cyc_Absyn_PointerType_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));
_tmp4E9[0]=({struct Cyc_Absyn_PointerType_struct _tmp4EA;_tmp4EA.tag=4;_tmp4EA.f1=({
struct Cyc_Absyn_PtrInfo _tmp4EB;_tmp4EB.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp4EE=_cycalloc(sizeof(*
_tmp4EE));_tmp4EE->v=_tmp4E7;_tmp4EE;}));_tmp4EB.elt_tq=Cyc_Absyn_empty_tqual();
_tmp4EB.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp4EC;_tmp4EC.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp4ED=_cycalloc(
sizeof(*_tmp4ED));_tmp4ED->v=_tmp4E7;_tmp4ED;}));_tmp4EC.nullable=Cyc_Absyn_true_conref;
_tmp4EC.bounds=Cyc_Absyn_empty_conref();_tmp4EC.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp4EC;});_tmp4EB;});_tmp4EA;});_tmp4E9;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp4E8));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _tagged_arr Cyc_Tcutil_coercion2string(void*c){void*
_tmp4EF=c;_LL362: if((int)_tmp4EF != 0)goto _LL364;_LL363: return({const char*_tmp4F0="unknown";
_tag_arr(_tmp4F0,sizeof(char),_get_zero_arr_size(_tmp4F0,8));});_LL364: if((int)
_tmp4EF != 1)goto _LL366;_LL365: return({const char*_tmp4F1="no coercion";_tag_arr(
_tmp4F1,sizeof(char),_get_zero_arr_size(_tmp4F1,12));});_LL366: if((int)_tmp4EF != 
2)goto _LL368;_LL367: return({const char*_tmp4F2="null check";_tag_arr(_tmp4F2,
sizeof(char),_get_zero_arr_size(_tmp4F2,11));});_LL368: if((int)_tmp4EF != 3)goto
_LL361;_LL369: return({const char*_tmp4F3="other coercion";_tag_arr(_tmp4F3,
sizeof(char),_get_zero_arr_size(_tmp4F3,15));});_LL361:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2)?Cyc_Tcutil_is_arithmetic_type(
t1): 0){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp4F7;_tmp4F7.tag=0;_tmp4F7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp4F6;_tmp4F6.tag=0;_tmp4F6.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp4F4[2]={&
_tmp4F6,& _tmp4F7};Cyc_Tcutil_warn(e->loc,({const char*_tmp4F5="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp4F5,sizeof(char),_get_zero_arr_size(_tmp4F5,53));}),_tag_arr(
_tmp4F4,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);
return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp4FB;_tmp4FB.tag=0;
_tmp4FB.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp4FA;_tmp4FA.tag=0;_tmp4FA.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp4F8[2]={& _tmp4FA,&
_tmp4FB};Cyc_Tcutil_warn(e->loc,({const char*_tmp4F9="implicit cast from %s to %s";
_tag_arr(_tmp4F9,sizeof(char),_get_zero_arr_size(_tmp4F9,28));}),_tag_arr(
_tmp4F8,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp4FC=Cyc_Tcutil_compress(t);
_LL36B: if(_tmp4FC <= (void*)3?1:*((int*)_tmp4FC)!= 5)goto _LL36D;_LL36C: goto _LL36E;
_LL36D: if((int)_tmp4FC != 1)goto _LL36F;_LL36E: goto _LL370;_LL36F: if(_tmp4FC <= (
void*)3?1:*((int*)_tmp4FC)!= 6)goto _LL371;_LL370: return 1;_LL371:;_LL372: return 0;
_LL36A:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp4FD=_cycalloc(sizeof(*
_tmp4FD));_tmp4FD->f1=x->tq;_tmp4FD->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp4FD;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp4FE=t1;struct Cyc_List_List*
_tmp4FF;struct Cyc_Absyn_AggrInfo _tmp500;void*_tmp501;struct Cyc_Absyn_Aggrdecl**
_tmp502;struct Cyc_Absyn_Aggrdecl*_tmp503;struct Cyc_List_List*_tmp504;void*
_tmp505;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_FnInfo _tmp507;_LL374: if((int)
_tmp4FE != 0)goto _LL376;_LL375: return 0;_LL376: if(_tmp4FE <= (void*)3?1:*((int*)
_tmp4FE)!= 9)goto _LL378;_tmp4FF=((struct Cyc_Absyn_TupleType_struct*)_tmp4FE)->f1;
_LL377: return _tmp4FF;_LL378: if(_tmp4FE <= (void*)3?1:*((int*)_tmp4FE)!= 10)goto
_LL37A;_tmp500=((struct Cyc_Absyn_AggrType_struct*)_tmp4FE)->f1;_tmp501=(void*)
_tmp500.aggr_info;if(*((int*)_tmp501)!= 1)goto _LL37A;_tmp502=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp501)->f1;_tmp503=*_tmp502;_tmp504=_tmp500.targs;_LL379: if((((void*)_tmp503->kind
== (void*)1?1: _tmp503->impl == 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp503->impl))->exist_vars != 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp503->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp508=_cycalloc(
sizeof(*_tmp508));_tmp508->hd=({struct _tuple4*_tmp509=_cycalloc(sizeof(*_tmp509));
_tmp509->f1=Cyc_Absyn_empty_tqual();_tmp509->f2=t1;_tmp509;});_tmp508->tl=0;
_tmp508;});{struct Cyc_List_List*_tmp50A=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp503->tvs,_tmp504);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp50A,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp503->impl))->fields);}_LL37A: if(
_tmp4FE <= (void*)3?1:*((int*)_tmp4FE)!= 11)goto _LL37C;_tmp505=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4FE)->f1;if((int)_tmp505 != 0)goto _LL37C;_tmp506=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4FE)->f2;_LL37B: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp506);_LL37C: if(_tmp4FE <= (void*)3?1:*((int*)
_tmp4FE)!= 8)goto _LL37E;_tmp507=((struct Cyc_Absyn_FnType_struct*)_tmp4FE)->f1;
_LL37D: return({struct Cyc_List_List*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->hd=({
struct _tuple4*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->f1=Cyc_Absyn_const_tqual();
_tmp50C->f2=t1;_tmp50C;});_tmp50B->tl=0;_tmp50B;});_LL37E:;_LL37F: return({struct
Cyc_List_List*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->hd=({struct _tuple4*
_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=Cyc_Absyn_empty_tqual();_tmp50E->f2=
t1;_tmp50E;});_tmp50D->tl=0;_tmp50D;});_LL373:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp50F=(void*)t->hd;_LL381: if((int)_tmp50F != 16)goto _LL383;
_LL382: goto _LL384;_LL383: if((int)_tmp50F != 3)goto _LL385;_LL384: goto _LL386;_LL385:
if(_tmp50F <= (void*)17?1:*((int*)_tmp50F)!= 4)goto _LL387;_LL386: continue;_LL387:;
_LL388: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL380:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp511=({struct _tuple6 _tmp510;_tmp510.f1=t1;_tmp510.f2=t2;_tmp510;});void*
_tmp512;struct Cyc_Absyn_PtrInfo _tmp513;void*_tmp514;struct Cyc_Absyn_Tqual _tmp515;
struct Cyc_Absyn_PtrAtts _tmp516;void*_tmp517;struct Cyc_Absyn_Conref*_tmp518;
struct Cyc_Absyn_Conref*_tmp519;struct Cyc_Absyn_Conref*_tmp51A;void*_tmp51B;
struct Cyc_Absyn_PtrInfo _tmp51C;void*_tmp51D;struct Cyc_Absyn_Tqual _tmp51E;struct
Cyc_Absyn_PtrAtts _tmp51F;void*_tmp520;struct Cyc_Absyn_Conref*_tmp521;struct Cyc_Absyn_Conref*
_tmp522;struct Cyc_Absyn_Conref*_tmp523;void*_tmp524;struct Cyc_Absyn_FnInfo
_tmp525;void*_tmp526;struct Cyc_Absyn_FnInfo _tmp527;_LL38A: _tmp512=_tmp511.f1;if(
_tmp512 <= (void*)3?1:*((int*)_tmp512)!= 4)goto _LL38C;_tmp513=((struct Cyc_Absyn_PointerType_struct*)
_tmp512)->f1;_tmp514=(void*)_tmp513.elt_typ;_tmp515=_tmp513.elt_tq;_tmp516=
_tmp513.ptr_atts;_tmp517=(void*)_tmp516.rgn;_tmp518=_tmp516.nullable;_tmp519=
_tmp516.bounds;_tmp51A=_tmp516.zero_term;_tmp51B=_tmp511.f2;if(_tmp51B <= (void*)
3?1:*((int*)_tmp51B)!= 4)goto _LL38C;_tmp51C=((struct Cyc_Absyn_PointerType_struct*)
_tmp51B)->f1;_tmp51D=(void*)_tmp51C.elt_typ;_tmp51E=_tmp51C.elt_tq;_tmp51F=
_tmp51C.ptr_atts;_tmp520=(void*)_tmp51F.rgn;_tmp521=_tmp51F.nullable;_tmp522=
_tmp51F.bounds;_tmp523=_tmp51F.zero_term;_LL38B: if(_tmp515.q_const?!_tmp51E.q_const:
0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp518,_tmp521)?((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp518): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp521): 0)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp51A,
_tmp523)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp51A): 0)?((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp523): 0)return 0;if(!
Cyc_Tcutil_unify(_tmp517,_tmp520)?!Cyc_Tcenv_region_outlives(te,_tmp517,_tmp520):
0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp519,_tmp522)){
struct _tuple6 _tmp529=({struct _tuple6 _tmp528;_tmp528.f1=Cyc_Absyn_conref_val(
_tmp519);_tmp528.f2=Cyc_Absyn_conref_val(_tmp522);_tmp528;});void*_tmp52A;void*
_tmp52B;void*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;void*_tmp52E;struct Cyc_Absyn_Exp*
_tmp52F;_LL391: _tmp52A=_tmp529.f1;if(_tmp52A <= (void*)1?1:*((int*)_tmp52A)!= 0)
goto _LL393;_tmp52B=_tmp529.f2;if((int)_tmp52B != 0)goto _LL393;_LL392: goto _LL390;
_LL393: _tmp52C=_tmp529.f1;if(_tmp52C <= (void*)1?1:*((int*)_tmp52C)!= 0)goto
_LL395;_tmp52D=((struct Cyc_Absyn_Upper_b_struct*)_tmp52C)->f1;_tmp52E=_tmp529.f2;
if(_tmp52E <= (void*)1?1:*((int*)_tmp52E)!= 0)goto _LL395;_tmp52F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp52E)->f1;_LL394: if(!Cyc_Evexp_lte_const_exp(_tmp52F,_tmp52D))return 0;goto
_LL390;_LL395:;_LL396: return 0;_LL390:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->hd=({struct _tuple6*
_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->f1=t1;_tmp531->f2=t2;_tmp531;});
_tmp530->tl=assume;_tmp530;}),_tmp514,_tmp51D);_LL38C: _tmp524=_tmp511.f1;if(
_tmp524 <= (void*)3?1:*((int*)_tmp524)!= 8)goto _LL38E;_tmp525=((struct Cyc_Absyn_FnType_struct*)
_tmp524)->f1;_tmp526=_tmp511.f2;if(_tmp526 <= (void*)3?1:*((int*)_tmp526)!= 8)
goto _LL38E;_tmp527=((struct Cyc_Absyn_FnType_struct*)_tmp526)->f1;_LL38D: if(
_tmp525.tvars != 0?1: _tmp527.tvars != 0){struct Cyc_List_List*_tmp532=_tmp525.tvars;
struct Cyc_List_List*_tmp533=_tmp527.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp532)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp533))return 0;{
struct _RegionHandle _tmp534=_new_region("r");struct _RegionHandle*r=& _tmp534;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp532 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp532->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp533))->hd)){int _tmp535=0;_npop_handler(0);
return _tmp535;}inst=({struct Cyc_List_List*_tmp536=_region_malloc(r,sizeof(*
_tmp536));_tmp536->hd=({struct _tuple8*_tmp537=_region_malloc(r,sizeof(*_tmp537));
_tmp537->f1=(struct Cyc_Absyn_Tvar*)_tmp533->hd;_tmp537->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538[0]=({struct Cyc_Absyn_VarType_struct
_tmp539;_tmp539.tag=1;_tmp539.f1=(struct Cyc_Absyn_Tvar*)_tmp532->hd;_tmp539;});
_tmp538;});_tmp537;});_tmp536->tl=inst;_tmp536;});_tmp532=_tmp532->tl;_tmp533=
_tmp533->tl;}if(inst != 0){_tmp525.tvars=0;_tmp527.tvars=0;{int _tmp53E=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp53A=_cycalloc(sizeof(*
_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_FnType_struct _tmp53B;_tmp53B.tag=8;
_tmp53B.f1=_tmp525;_tmp53B;});_tmp53A;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Absyn_FnType_struct
_tmp53D;_tmp53D.tag=8;_tmp53D.f1=_tmp527;_tmp53D;});_tmp53C;}));_npop_handler(0);
return _tmp53E;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp525.ret_typ,(void*)_tmp527.ret_typ))return 0;{struct Cyc_List_List*_tmp53F=
_tmp525.args;struct Cyc_List_List*_tmp540=_tmp527.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp53F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp540))return 0;for(0;_tmp53F != 0;(_tmp53F=_tmp53F->tl,_tmp540=_tmp540->tl)){
struct Cyc_Absyn_Tqual _tmp542;void*_tmp543;struct _tuple2 _tmp541=*((struct _tuple2*)
_tmp53F->hd);_tmp542=_tmp541.f2;_tmp543=_tmp541.f3;{struct Cyc_Absyn_Tqual _tmp545;
void*_tmp546;struct _tuple2 _tmp544=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp540))->hd);_tmp545=_tmp544.f2;_tmp546=_tmp544.f3;if((_tmp545.q_const?
!_tmp542.q_const: 0)?1: !Cyc_Tcutil_subtype(te,assume,_tmp546,_tmp543))return 0;}}
if(_tmp525.c_varargs != _tmp527.c_varargs)return 0;if(_tmp525.cyc_varargs != 0?
_tmp527.cyc_varargs != 0: 0){struct Cyc_Absyn_VarargInfo _tmp547=*_tmp525.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp548=*_tmp527.cyc_varargs;if(((_tmp548.tq).q_const?
!(_tmp547.tq).q_const: 0)?1: !Cyc_Tcutil_subtype(te,assume,(void*)_tmp548.type,(
void*)_tmp547.type))return 0;}else{if(_tmp525.cyc_varargs != 0?1: _tmp527.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp525.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp527.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp525.rgn_po,_tmp527.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp525.attributes,_tmp527.attributes))return 0;return 1;}_LL38E:;_LL38F: return 0;
_LL389:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp54A=({
struct _tuple6 _tmp549;_tmp549.f1=Cyc_Tcutil_compress(t1);_tmp549.f2=Cyc_Tcutil_compress(
t2);_tmp549;});void*_tmp54B;void*_tmp54C;void*_tmp54D;void*_tmp54E;_LL398:
_tmp54B=_tmp54A.f1;if(_tmp54B <= (void*)3?1:*((int*)_tmp54B)!= 5)goto _LL39A;
_tmp54C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp54B)->f2;_tmp54D=_tmp54A.f2;
if(_tmp54D <= (void*)3?1:*((int*)_tmp54D)!= 5)goto _LL39A;_tmp54E=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp54D)->f2;_LL399: return _tmp54C == _tmp54E;_LL39A:;
_LL39B: return 0;_LL397:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp550;struct
Cyc_Absyn_Tqual _tmp551;void*_tmp552;struct _tuple4*_tmp54F=(struct _tuple4*)tqs1->hd;
_tmp550=*_tmp54F;_tmp551=_tmp550.f1;_tmp552=_tmp550.f2;{struct _tuple4 _tmp554;
struct Cyc_Absyn_Tqual _tmp555;void*_tmp556;struct _tuple4*_tmp553=(struct _tuple4*)
tqs2->hd;_tmp554=*_tmp553;_tmp555=_tmp554.f1;_tmp556=_tmp554.f2;if(_tmp555.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp552,_tmp556): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp552,_tmp556))continue;else{if(Cyc_Tcutil_isomorphic(_tmp552,_tmp556))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp557=Cyc_Tcutil_compress(t);void*_tmp558;_LL39D: if(_tmp557 <= (void*)3?1:*((
int*)_tmp557)!= 5)goto _LL39F;_tmp558=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp557)->f2;if((int)_tmp558 != 0)goto _LL39F;_LL39E: return 1;_LL39F:;_LL3A0: return
0;_LL39C:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp559=t2;
void*_tmp55A;_LL3A2: if(_tmp559 <= (void*)3?1:*((int*)_tmp559)!= 5)goto _LL3A4;
_tmp55A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp559)->f2;if((int)_tmp55A != 
2)goto _LL3A4;_LL3A3: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto
_LL3A1;_LL3A4:;_LL3A5: goto _LL3A1;_LL3A1:;}{void*_tmp55B=t1;struct Cyc_Absyn_PtrInfo
_tmp55C;void*_tmp55D;struct Cyc_Absyn_Tqual _tmp55E;struct Cyc_Absyn_PtrAtts _tmp55F;
void*_tmp560;struct Cyc_Absyn_Conref*_tmp561;struct Cyc_Absyn_Conref*_tmp562;
struct Cyc_Absyn_Conref*_tmp563;struct Cyc_Absyn_ArrayInfo _tmp564;void*_tmp565;
struct Cyc_Absyn_Tqual _tmp566;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Conref*
_tmp568;struct Cyc_Absyn_Enumdecl*_tmp569;_LL3A7: if(_tmp55B <= (void*)3?1:*((int*)
_tmp55B)!= 4)goto _LL3A9;_tmp55C=((struct Cyc_Absyn_PointerType_struct*)_tmp55B)->f1;
_tmp55D=(void*)_tmp55C.elt_typ;_tmp55E=_tmp55C.elt_tq;_tmp55F=_tmp55C.ptr_atts;
_tmp560=(void*)_tmp55F.rgn;_tmp561=_tmp55F.nullable;_tmp562=_tmp55F.bounds;
_tmp563=_tmp55F.zero_term;_LL3A8:{void*_tmp56A=t2;struct Cyc_Absyn_PtrInfo _tmp56B;
void*_tmp56C;struct Cyc_Absyn_Tqual _tmp56D;struct Cyc_Absyn_PtrAtts _tmp56E;void*
_tmp56F;struct Cyc_Absyn_Conref*_tmp570;struct Cyc_Absyn_Conref*_tmp571;struct Cyc_Absyn_Conref*
_tmp572;_LL3B6: if(_tmp56A <= (void*)3?1:*((int*)_tmp56A)!= 4)goto _LL3B8;_tmp56B=((
struct Cyc_Absyn_PointerType_struct*)_tmp56A)->f1;_tmp56C=(void*)_tmp56B.elt_typ;
_tmp56D=_tmp56B.elt_tq;_tmp56E=_tmp56B.ptr_atts;_tmp56F=(void*)_tmp56E.rgn;
_tmp570=_tmp56E.nullable;_tmp571=_tmp56E.bounds;_tmp572=_tmp56E.zero_term;_LL3B7: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp573=({struct Cyc_List_List*_tmp57E=
_cycalloc(sizeof(*_tmp57E));_tmp57E->hd=({struct _tuple6*_tmp57F=_cycalloc(
sizeof(*_tmp57F));_tmp57F->f1=t1;_tmp57F->f2=t2;_tmp57F;});_tmp57E->tl=0;_tmp57E;});
int _tmp574=Cyc_Tcutil_ptrsubtype(te,_tmp573,_tmp55D,_tmp56C)?!_tmp55E.q_const?1:
_tmp56D.q_const: 0;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp563,_tmp572)?1: !((int(*)(struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp572);int _tmp575=_tmp574?0:(((Cyc_Tcutil_bits_only(
_tmp55D)?Cyc_Tcutil_is_char_type(_tmp56C): 0)?!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp572): 0)?_tmp56D.q_const?1: !_tmp55E.q_const: 0);int
bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp562,_tmp571);if(!
bounds_ok?!_tmp575: 0){struct _tuple6 _tmp577=({struct _tuple6 _tmp576;_tmp576.f1=Cyc_Absyn_conref_val(
_tmp562);_tmp576.f2=Cyc_Absyn_conref_val(_tmp571);_tmp576;});void*_tmp578;struct
Cyc_Absyn_Exp*_tmp579;void*_tmp57A;struct Cyc_Absyn_Exp*_tmp57B;void*_tmp57C;void*
_tmp57D;_LL3BB: _tmp578=_tmp577.f1;if(_tmp578 <= (void*)1?1:*((int*)_tmp578)!= 0)
goto _LL3BD;_tmp579=((struct Cyc_Absyn_Upper_b_struct*)_tmp578)->f1;_tmp57A=
_tmp577.f2;if(_tmp57A <= (void*)1?1:*((int*)_tmp57A)!= 0)goto _LL3BD;_tmp57B=((
struct Cyc_Absyn_Upper_b_struct*)_tmp57A)->f1;_LL3BC: if(Cyc_Evexp_lte_const_exp(
_tmp57B,_tmp579))bounds_ok=1;goto _LL3BA;_LL3BD: _tmp57C=_tmp577.f1;if(_tmp57C <= (
void*)1?1:*((int*)_tmp57C)!= 1)goto _LL3BF;_LL3BE: goto _LL3C0;_LL3BF: _tmp57D=
_tmp577.f2;if(_tmp57D <= (void*)1?1:*((int*)_tmp57D)!= 1)goto _LL3C1;_LL3C0:
bounds_ok=0;goto _LL3BA;_LL3C1:;_LL3C2: bounds_ok=1;goto _LL3BA;_LL3BA:;}if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp561)?!((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp570): 0)coercion=(void*)2;
if(((bounds_ok?zeroterm_ok: 0)?_tmp574?1: _tmp575: 0)?Cyc_Tcutil_unify(_tmp560,
_tmp56F)?1: Cyc_Tcenv_region_outlives(te,_tmp560,_tmp56F): 0)return coercion;else{
return(void*)0;}}}_LL3B8:;_LL3B9: goto _LL3B5;_LL3B5:;}return(void*)0;_LL3A9: if(
_tmp55B <= (void*)3?1:*((int*)_tmp55B)!= 7)goto _LL3AB;_tmp564=((struct Cyc_Absyn_ArrayType_struct*)
_tmp55B)->f1;_tmp565=(void*)_tmp564.elt_type;_tmp566=_tmp564.tq;_tmp567=_tmp564.num_elts;
_tmp568=_tmp564.zero_term;_LL3AA:{void*_tmp580=t2;struct Cyc_Absyn_ArrayInfo
_tmp581;void*_tmp582;struct Cyc_Absyn_Tqual _tmp583;struct Cyc_Absyn_Exp*_tmp584;
struct Cyc_Absyn_Conref*_tmp585;_LL3C4: if(_tmp580 <= (void*)3?1:*((int*)_tmp580)!= 
7)goto _LL3C6;_tmp581=((struct Cyc_Absyn_ArrayType_struct*)_tmp580)->f1;_tmp582=(
void*)_tmp581.elt_type;_tmp583=_tmp581.tq;_tmp584=_tmp581.num_elts;_tmp585=
_tmp581.zero_term;_LL3C5: {int okay;okay=((_tmp567 != 0?_tmp584 != 0: 0)?((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp568,_tmp585): 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)
_tmp584,(struct Cyc_Absyn_Exp*)_tmp567): 0;return((okay?Cyc_Tcutil_unify(_tmp565,
_tmp582): 0)?!_tmp566.q_const?1: _tmp583.q_const: 0)?(void*)3:(void*)0;}_LL3C6:;
_LL3C7: return(void*)0;_LL3C3:;}return(void*)0;_LL3AB: if(_tmp55B <= (void*)3?1:*((
int*)_tmp55B)!= 12)goto _LL3AD;_tmp569=((struct Cyc_Absyn_EnumType_struct*)_tmp55B)->f2;
_LL3AC:{void*_tmp586=t2;struct Cyc_Absyn_Enumdecl*_tmp587;_LL3C9: if(_tmp586 <= (
void*)3?1:*((int*)_tmp586)!= 12)goto _LL3CB;_tmp587=((struct Cyc_Absyn_EnumType_struct*)
_tmp586)->f2;_LL3CA: if((_tmp569->fields != 0?_tmp587->fields != 0: 0)?((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp569->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp587->fields))->v): 0)
return(void*)1;goto _LL3C8;_LL3CB:;_LL3CC: goto _LL3C8;_LL3C8:;}goto _LL3AE;_LL3AD:
if(_tmp55B <= (void*)3?1:*((int*)_tmp55B)!= 5)goto _LL3AF;_LL3AE: goto _LL3B0;_LL3AF:
if((int)_tmp55B != 1)goto _LL3B1;_LL3B0: goto _LL3B2;_LL3B1: if(_tmp55B <= (void*)3?1:*((
int*)_tmp55B)!= 6)goto _LL3B3;_LL3B2: return Cyc_Tcutil_coerceable(t2)?(void*)1:(
void*)0;_LL3B3:;_LL3B4: return(void*)0;_LL3A6:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp588=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp58A;_tmp58A.tag=13;_tmp58A.f1=(void*)t;_tmp58A.f2=_tmp588;_tmp58A.f3=0;
_tmp58A.f4=(void*)c;_tmp58A;});_tmp589;})));e->topt=({struct Cyc_Core_Opt*_tmp58B=
_cycalloc(sizeof(*_tmp58B));_tmp58B->v=(void*)t;_tmp58B;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp58C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3CE: if(_tmp58C <= (void*)3?1:*((int*)_tmp58C)!= 5)
goto _LL3D0;_LL3CF: goto _LL3D1;_LL3D0: if(_tmp58C <= (void*)3?1:*((int*)_tmp58C)!= 
12)goto _LL3D2;_LL3D1: goto _LL3D3;_LL3D2: if(_tmp58C <= (void*)3?1:*((int*)_tmp58C)
!= 13)goto _LL3D4;_LL3D3: goto _LL3D5;_LL3D4: if(_tmp58C <= (void*)3?1:*((int*)
_tmp58C)!= 17)goto _LL3D6;_LL3D5: goto _LL3D7;_LL3D6: if(_tmp58C <= (void*)3?1:*((int*)
_tmp58C)!= 14)goto _LL3D8;_LL3D7: return 1;_LL3D8: if(_tmp58C <= (void*)3?1:*((int*)
_tmp58C)!= 0)goto _LL3DA;_LL3D9: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3DA:;_LL3DB: return 0;_LL3CD:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp58D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3DD: if((int)_tmp58D != 1)goto _LL3DF;_LL3DE: goto _LL3E0;_LL3DF: if(_tmp58D <= (
void*)3?1:*((int*)_tmp58D)!= 6)goto _LL3E1;_LL3E0: return 1;_LL3E1:;_LL3E2: return 0;
_LL3DC:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp58E=Cyc_Tcutil_compress(
t);_LL3E4: if(_tmp58E <= (void*)3?1:*((int*)_tmp58E)!= 8)goto _LL3E6;_LL3E5: return 1;
_LL3E6:;_LL3E7: return 0;_LL3E3:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*
t2){struct _tuple6 _tmp590=({struct _tuple6 _tmp58F;_tmp58F.f1=t1;_tmp58F.f2=t2;
_tmp58F;});void*_tmp591;int _tmp592;void*_tmp593;int _tmp594;void*_tmp595;void*
_tmp596;void*_tmp597;void*_tmp598;void*_tmp599;void*_tmp59A;void*_tmp59B;void*
_tmp59C;void*_tmp59D;void*_tmp59E;void*_tmp59F;void*_tmp5A0;void*_tmp5A1;void*
_tmp5A2;void*_tmp5A3;void*_tmp5A4;void*_tmp5A5;void*_tmp5A6;void*_tmp5A7;void*
_tmp5A8;void*_tmp5A9;void*_tmp5AA;void*_tmp5AB;void*_tmp5AC;void*_tmp5AD;void*
_tmp5AE;void*_tmp5AF;void*_tmp5B0;_LL3E9: _tmp591=_tmp590.f1;if(_tmp591 <= (void*)
3?1:*((int*)_tmp591)!= 6)goto _LL3EB;_tmp592=((struct Cyc_Absyn_DoubleType_struct*)
_tmp591)->f1;_tmp593=_tmp590.f2;if(_tmp593 <= (void*)3?1:*((int*)_tmp593)!= 6)
goto _LL3EB;_tmp594=((struct Cyc_Absyn_DoubleType_struct*)_tmp593)->f1;_LL3EA: if(
_tmp592)return t1;else{return t2;}_LL3EB: _tmp595=_tmp590.f1;if(_tmp595 <= (void*)3?
1:*((int*)_tmp595)!= 6)goto _LL3ED;_LL3EC: return t1;_LL3ED: _tmp596=_tmp590.f2;if(
_tmp596 <= (void*)3?1:*((int*)_tmp596)!= 6)goto _LL3EF;_LL3EE: return t2;_LL3EF:
_tmp597=_tmp590.f1;if((int)_tmp597 != 1)goto _LL3F1;_LL3F0: goto _LL3F2;_LL3F1:
_tmp598=_tmp590.f2;if((int)_tmp598 != 1)goto _LL3F3;_LL3F2: return(void*)1;_LL3F3:
_tmp599=_tmp590.f1;if(_tmp599 <= (void*)3?1:*((int*)_tmp599)!= 5)goto _LL3F5;
_tmp59A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp599)->f1;if((int)_tmp59A != 
1)goto _LL3F5;_tmp59B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp599)->f2;if((
int)_tmp59B != 3)goto _LL3F5;_LL3F4: goto _LL3F6;_LL3F5: _tmp59C=_tmp590.f2;if(
_tmp59C <= (void*)3?1:*((int*)_tmp59C)!= 5)goto _LL3F7;_tmp59D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp59C)->f1;if((int)_tmp59D != 1)goto _LL3F7;_tmp59E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp59C)->f2;if((int)_tmp59E != 3)goto _LL3F7;_LL3F6: return Cyc_Absyn_ulonglong_typ;
_LL3F7: _tmp59F=_tmp590.f1;if(_tmp59F <= (void*)3?1:*((int*)_tmp59F)!= 5)goto
_LL3F9;_tmp5A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59F)->f1;if((int)
_tmp5A0 != 2)goto _LL3F9;_tmp5A1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59F)->f2;
if((int)_tmp5A1 != 3)goto _LL3F9;_LL3F8: goto _LL3FA;_LL3F9: _tmp5A2=_tmp590.f2;if(
_tmp5A2 <= (void*)3?1:*((int*)_tmp5A2)!= 5)goto _LL3FB;_tmp5A3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5A2)->f1;if((int)_tmp5A3 != 2)goto _LL3FB;_tmp5A4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5A2)->f2;if((int)_tmp5A4 != 3)goto _LL3FB;_LL3FA: goto _LL3FC;_LL3FB: _tmp5A5=
_tmp590.f1;if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 5)goto _LL3FD;_tmp5A6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5A5)->f1;if((int)_tmp5A6 != 0)goto _LL3FD;
_tmp5A7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5A5)->f2;if((int)_tmp5A7 != 
3)goto _LL3FD;_LL3FC: goto _LL3FE;_LL3FD: _tmp5A8=_tmp590.f2;if(_tmp5A8 <= (void*)3?1:*((
int*)_tmp5A8)!= 5)goto _LL3FF;_tmp5A9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5A8)->f1;if((int)_tmp5A9 != 0)goto _LL3FF;_tmp5AA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5A8)->f2;if((int)_tmp5AA != 3)goto _LL3FF;_LL3FE: return Cyc_Absyn_slonglong_typ;
_LL3FF: _tmp5AB=_tmp590.f1;if(_tmp5AB <= (void*)3?1:*((int*)_tmp5AB)!= 5)goto
_LL401;_tmp5AC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5AB)->f1;if((int)
_tmp5AC != 1)goto _LL401;_tmp5AD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5AB)->f2;
if((int)_tmp5AD != 2)goto _LL401;_LL400: goto _LL402;_LL401: _tmp5AE=_tmp590.f2;if(
_tmp5AE <= (void*)3?1:*((int*)_tmp5AE)!= 5)goto _LL403;_tmp5AF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5AE)->f1;if((int)_tmp5AF != 1)goto _LL403;_tmp5B0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5AE)->f2;if((int)_tmp5B0 != 2)goto _LL403;_LL402: return Cyc_Absyn_uint_typ;
_LL403:;_LL404: return Cyc_Absyn_sint_typ;_LL3E8:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5B1=(void*)e->r;struct Cyc_Core_Opt*_tmp5B2;
_LL406: if(*((int*)_tmp5B1)!= 4)goto _LL408;_tmp5B2=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5B1)->f2;if(_tmp5B2 != 0)goto _LL408;_LL407:({void*_tmp5B3[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp5B4="assignment in test";_tag_arr(_tmp5B4,sizeof(char),
_get_zero_arr_size(_tmp5B4,19));}),_tag_arr(_tmp5B3,sizeof(void*),0));});goto
_LL405;_LL408:;_LL409: goto _LL405;_LL405:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp5B6=({struct _tuple6 _tmp5B5;_tmp5B5.f1=c1;_tmp5B5.f2=c2;_tmp5B5;});
void*_tmp5B7;void*_tmp5B8;void*_tmp5B9;void*_tmp5BA;void*_tmp5BB;struct Cyc_Core_Opt*
_tmp5BC;struct Cyc_Core_Opt**_tmp5BD;void*_tmp5BE;struct Cyc_Core_Opt*_tmp5BF;
struct Cyc_Core_Opt**_tmp5C0;void*_tmp5C1;struct Cyc_Core_Opt*_tmp5C2;struct Cyc_Core_Opt**
_tmp5C3;void*_tmp5C4;void*_tmp5C5;void*_tmp5C6;void*_tmp5C7;void*_tmp5C8;void*
_tmp5C9;struct Cyc_Core_Opt*_tmp5CA;struct Cyc_Core_Opt**_tmp5CB;void*_tmp5CC;void*
_tmp5CD;struct Cyc_Core_Opt*_tmp5CE;struct Cyc_Core_Opt**_tmp5CF;void*_tmp5D0;void*
_tmp5D1;struct Cyc_Core_Opt*_tmp5D2;struct Cyc_Core_Opt**_tmp5D3;void*_tmp5D4;
_LL40B: _tmp5B7=_tmp5B6.f1;if(*((int*)_tmp5B7)!= 0)goto _LL40D;_tmp5B8=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5B7)->f1;_tmp5B9=_tmp5B6.f2;if(*((int*)_tmp5B9)
!= 0)goto _LL40D;_tmp5BA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5B9)->f1;
_LL40C: return _tmp5B8 == _tmp5BA;_LL40D: _tmp5BB=_tmp5B6.f2;if(*((int*)_tmp5BB)!= 1)
goto _LL40F;_tmp5BC=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5BB)->f1;_tmp5BD=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5BB)->f1;_LL40E:*
_tmp5BD=({struct Cyc_Core_Opt*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->v=(
void*)c1;_tmp5D5;});return 1;_LL40F: _tmp5BE=_tmp5B6.f1;if(*((int*)_tmp5BE)!= 1)
goto _LL411;_tmp5BF=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5BE)->f1;_tmp5C0=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5BE)->f1;_LL410:*
_tmp5C0=({struct Cyc_Core_Opt*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->v=(
void*)c2;_tmp5D6;});return 1;_LL411: _tmp5C1=_tmp5B6.f1;if(*((int*)_tmp5C1)!= 2)
goto _LL413;_tmp5C2=((struct Cyc_Absyn_Less_kb_struct*)_tmp5C1)->f1;_tmp5C3=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5C1)->f1;_tmp5C4=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5C1)->f2;_tmp5C5=_tmp5B6.f2;if(*((
int*)_tmp5C5)!= 0)goto _LL413;_tmp5C6=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5C5)->f1;_LL412: if(Cyc_Tcutil_kind_leq(_tmp5C6,_tmp5C4)){*_tmp5C3=({struct
Cyc_Core_Opt*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->v=(void*)c2;_tmp5D7;});
return 1;}else{return 0;}_LL413: _tmp5C7=_tmp5B6.f1;if(*((int*)_tmp5C7)!= 0)goto
_LL415;_tmp5C8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5C7)->f1;_tmp5C9=
_tmp5B6.f2;if(*((int*)_tmp5C9)!= 2)goto _LL415;_tmp5CA=((struct Cyc_Absyn_Less_kb_struct*)
_tmp5C9)->f1;_tmp5CB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp5C9)->f1;_tmp5CC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5C9)->f2;
_LL414: if(Cyc_Tcutil_kind_leq(_tmp5C8,_tmp5CC)){*_tmp5CB=({struct Cyc_Core_Opt*
_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->v=(void*)c1;_tmp5D8;});return 1;}
else{return 0;}_LL415: _tmp5CD=_tmp5B6.f1;if(*((int*)_tmp5CD)!= 2)goto _LL40A;
_tmp5CE=((struct Cyc_Absyn_Less_kb_struct*)_tmp5CD)->f1;_tmp5CF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5CD)->f1;_tmp5D0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5CD)->f2;_tmp5D1=_tmp5B6.f2;if(*((int*)_tmp5D1)!= 2)goto _LL40A;_tmp5D2=((
struct Cyc_Absyn_Less_kb_struct*)_tmp5D1)->f1;_tmp5D3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5D1)->f1;_tmp5D4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5D1)->f2;_LL416: if(Cyc_Tcutil_kind_leq(_tmp5D0,_tmp5D4)){*_tmp5D3=({struct
Cyc_Core_Opt*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->v=(void*)c1;_tmp5D9;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp5D4,_tmp5D0)){*_tmp5CF=({struct Cyc_Core_Opt*
_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->v=(void*)c2;_tmp5DA;});return 1;}
else{return 0;}}_LL40A:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp5DB=_cycalloc_atomic(sizeof(*_tmp5DB));_tmp5DB[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp5DB;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp5E0;_tmp5E0.tag=1;_tmp5E0.f1=(unsigned int)i;{void*
_tmp5DE[1]={& _tmp5E0};Cyc_aprintf(({const char*_tmp5DF="#%d";_tag_arr(_tmp5DF,
sizeof(char),_get_zero_arr_size(_tmp5DF,4));}),_tag_arr(_tmp5DE,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->name=({
struct _tagged_arr*_tmp5DD=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp5DD[0]=s;
_tmp5DD;});_tmp5DC->identity=0;_tmp5DC->kind=(void*)k;_tmp5DC;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp5E1=*t->name;return*((const char*)
_check_unknown_subscript(_tmp5E1,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp5E4;_tmp5E4.tag=0;
_tmp5E4.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp5E2[1]={&
_tmp5E4};Cyc_printf(({const char*_tmp5E3="%s";_tag_arr(_tmp5E3,sizeof(char),
_get_zero_arr_size(_tmp5E3,3));}),_tag_arr(_tmp5E2,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp5E5=Cyc_strconcat(({const char*_tmp5EA="`";
_tag_arr(_tmp5EA,sizeof(char),_get_zero_arr_size(_tmp5EA,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp5E6=_tagged_arr_plus(_tmp5E5,
sizeof(char),1);char _tmp5E7=*((char*)_check_unknown_subscript(_tmp5E6,sizeof(
char),0));char _tmp5E8='t';if(_get_arr_size(_tmp5E6,sizeof(char))== 1?_tmp5E7 == '\000'?
_tmp5E8 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp5E6.curr)=_tmp5E8;});t->name=({
struct _tagged_arr*_tmp5E9=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp5E9[0]=(
struct _tagged_arr)_tmp5E5;_tmp5E9;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));
_tmp5EB->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp5EC=_cycalloc(sizeof(*
_tmp5EC));_tmp5EC->v=(*x).f1;_tmp5EC;});_tmp5EB->f2=(*x).f2;_tmp5EB->f3=(*x).f3;
_tmp5EB;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp5ED=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp5ED=({
struct Cyc_List_List*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=(void*)((
void*)atts->hd);_tmp5EE->tl=_tmp5ED;_tmp5EE;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF[0]=({struct Cyc_Absyn_FnType_struct
_tmp5F0;_tmp5F0.tag=8;_tmp5F0.f1=({struct Cyc_Absyn_FnInfo _tmp5F1;_tmp5F1.tvars=
fd->tvs;_tmp5F1.effect=fd->effect;_tmp5F1.ret_typ=(void*)((void*)fd->ret_type);
_tmp5F1.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5F1.c_varargs=
fd->c_varargs;_tmp5F1.cyc_varargs=fd->cyc_varargs;_tmp5F1.rgn_po=fd->rgn_po;
_tmp5F1.attributes=_tmp5ED;_tmp5F1;});_tmp5F0;});_tmp5EF;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->f1=(*pr).f1;_tmp5F2->f2=t;_tmp5F2;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp5F3=_region_malloc(
rgn,sizeof(*_tmp5F3));_tmp5F3->f1=({struct _tuple15*_tmp5F4=_region_malloc(rgn,
sizeof(*_tmp5F4));_tmp5F4->f1=(*y).f1;_tmp5F4->f2=(*y).f2;_tmp5F4;});_tmp5F3->f2=(*
y).f3;_tmp5F3;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp5F6;void*_tmp5F7;struct _tuple16 _tmp5F5=*w;_tmp5F6=_tmp5F5.f1;
_tmp5F7=_tmp5F5.f2;{struct Cyc_Core_Opt*_tmp5F9;struct Cyc_Absyn_Tqual _tmp5FA;
struct _tuple15 _tmp5F8=*_tmp5F6;_tmp5F9=_tmp5F8.f1;_tmp5FA=_tmp5F8.f2;return({
struct _tuple2*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->f1=_tmp5F9;_tmp5FB->f2=
_tmp5FA;_tmp5FB->f3=_tmp5F7;_tmp5FB;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp5FC=
_cycalloc(sizeof(*_tmp5FC));_tmp5FC->name=f->name;_tmp5FC->tq=f->tq;_tmp5FC->type=(
void*)t;_tmp5FC->width=f->width;_tmp5FC->attributes=f->attributes;_tmp5FC;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp5FD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp5FE;struct Cyc_Absyn_AggrInfo _tmp5FF;void*_tmp600;struct Cyc_List_List*_tmp601;
struct Cyc_Absyn_TunionInfo _tmp602;void*_tmp603;struct Cyc_List_List*_tmp604;void*
_tmp605;struct Cyc_Absyn_TunionFieldInfo _tmp606;void*_tmp607;struct Cyc_List_List*
_tmp608;struct _tuple1*_tmp609;struct Cyc_List_List*_tmp60A;struct Cyc_Absyn_Typedefdecl*
_tmp60B;void**_tmp60C;struct Cyc_Absyn_ArrayInfo _tmp60D;void*_tmp60E;struct Cyc_Absyn_Tqual
_tmp60F;struct Cyc_Absyn_Exp*_tmp610;struct Cyc_Absyn_Conref*_tmp611;struct Cyc_Absyn_PtrInfo
_tmp612;void*_tmp613;struct Cyc_Absyn_Tqual _tmp614;struct Cyc_Absyn_PtrAtts _tmp615;
void*_tmp616;struct Cyc_Absyn_Conref*_tmp617;struct Cyc_Absyn_Conref*_tmp618;
struct Cyc_Absyn_Conref*_tmp619;struct Cyc_Absyn_FnInfo _tmp61A;struct Cyc_List_List*
_tmp61B;struct Cyc_Core_Opt*_tmp61C;void*_tmp61D;struct Cyc_List_List*_tmp61E;int
_tmp61F;struct Cyc_Absyn_VarargInfo*_tmp620;struct Cyc_List_List*_tmp621;struct Cyc_List_List*
_tmp622;struct Cyc_List_List*_tmp623;void*_tmp624;struct Cyc_List_List*_tmp625;
struct Cyc_Core_Opt*_tmp626;void*_tmp627;void*_tmp628;void*_tmp629;void*_tmp62A;
void*_tmp62B;struct Cyc_List_List*_tmp62C;_LL418: if(_tmp5FD <= (void*)3?1:*((int*)
_tmp5FD)!= 1)goto _LL41A;_tmp5FE=((struct Cyc_Absyn_VarType_struct*)_tmp5FD)->f1;
_LL419: {struct _handler_cons _tmp62D;_push_handler(& _tmp62D);{int _tmp62F=0;if(
setjmp(_tmp62D.handler))_tmp62F=1;if(!_tmp62F){{void*_tmp630=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5FE);_npop_handler(0);return
_tmp630;};_pop_handler();}else{void*_tmp62E=(void*)_exn_thrown;void*_tmp632=
_tmp62E;_LL44B: if(_tmp632 != Cyc_Core_Not_found)goto _LL44D;_LL44C: return t;_LL44D:;
_LL44E:(void)_throw(_tmp632);_LL44A:;}}}_LL41A: if(_tmp5FD <= (void*)3?1:*((int*)
_tmp5FD)!= 10)goto _LL41C;_tmp5FF=((struct Cyc_Absyn_AggrType_struct*)_tmp5FD)->f1;
_tmp600=(void*)_tmp5FF.aggr_info;_tmp601=_tmp5FF.targs;_LL41B: {struct Cyc_List_List*
_tmp633=Cyc_Tcutil_substs(rgn,inst,_tmp601);return _tmp633 == _tmp601?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({
struct Cyc_Absyn_AggrType_struct _tmp635;_tmp635.tag=10;_tmp635.f1=({struct Cyc_Absyn_AggrInfo
_tmp636;_tmp636.aggr_info=(void*)_tmp600;_tmp636.targs=_tmp633;_tmp636;});
_tmp635;});_tmp634;});}_LL41C: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 2)goto
_LL41E;_tmp602=((struct Cyc_Absyn_TunionType_struct*)_tmp5FD)->f1;_tmp603=(void*)
_tmp602.tunion_info;_tmp604=_tmp602.targs;_tmp605=(void*)_tmp602.rgn;_LL41D: {
struct Cyc_List_List*_tmp637=Cyc_Tcutil_substs(rgn,inst,_tmp604);void*_tmp638=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp605);return(_tmp637 == _tmp604?_tmp638 == _tmp605: 0)?t:(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({
struct Cyc_Absyn_TunionType_struct _tmp63A;_tmp63A.tag=2;_tmp63A.f1=({struct Cyc_Absyn_TunionInfo
_tmp63B;_tmp63B.tunion_info=(void*)_tmp603;_tmp63B.targs=_tmp637;_tmp63B.rgn=(
void*)_tmp638;_tmp63B;});_tmp63A;});_tmp639;});}_LL41E: if(_tmp5FD <= (void*)3?1:*((
int*)_tmp5FD)!= 3)goto _LL420;_tmp606=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5FD)->f1;_tmp607=(void*)_tmp606.field_info;_tmp608=_tmp606.targs;_LL41F: {
struct Cyc_List_List*_tmp63C=Cyc_Tcutil_substs(rgn,inst,_tmp608);return _tmp63C == 
_tmp608?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp63D=_cycalloc(
sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp63E;
_tmp63E.tag=3;_tmp63E.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp63F;_tmp63F.field_info=(
void*)_tmp607;_tmp63F.targs=_tmp63C;_tmp63F;});_tmp63E;});_tmp63D;});}_LL420: if(
_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 16)goto _LL422;_tmp609=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5FD)->f1;_tmp60A=((struct Cyc_Absyn_TypedefType_struct*)_tmp5FD)->f2;_tmp60B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp5FD)->f3;_tmp60C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5FD)->f4;_LL421: {struct Cyc_List_List*_tmp640=Cyc_Tcutil_substs(rgn,inst,
_tmp60A);return _tmp640 == _tmp60A?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp642;_tmp642.tag=16;_tmp642.f1=_tmp609;_tmp642.f2=_tmp640;_tmp642.f3=_tmp60B;
_tmp642.f4=_tmp60C;_tmp642;});_tmp641;});}_LL422: if(_tmp5FD <= (void*)3?1:*((int*)
_tmp5FD)!= 7)goto _LL424;_tmp60D=((struct Cyc_Absyn_ArrayType_struct*)_tmp5FD)->f1;
_tmp60E=(void*)_tmp60D.elt_type;_tmp60F=_tmp60D.tq;_tmp610=_tmp60D.num_elts;
_tmp611=_tmp60D.zero_term;_LL423: {void*_tmp643=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp60E);return _tmp643 == _tmp60E?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp645;_tmp645.tag=7;_tmp645.f1=({struct Cyc_Absyn_ArrayInfo _tmp646;_tmp646.elt_type=(
void*)_tmp643;_tmp646.tq=_tmp60F;_tmp646.num_elts=_tmp610;_tmp646.zero_term=
_tmp611;_tmp646;});_tmp645;});_tmp644;});}_LL424: if(_tmp5FD <= (void*)3?1:*((int*)
_tmp5FD)!= 4)goto _LL426;_tmp612=((struct Cyc_Absyn_PointerType_struct*)_tmp5FD)->f1;
_tmp613=(void*)_tmp612.elt_typ;_tmp614=_tmp612.elt_tq;_tmp615=_tmp612.ptr_atts;
_tmp616=(void*)_tmp615.rgn;_tmp617=_tmp615.nullable;_tmp618=_tmp615.bounds;
_tmp619=_tmp615.zero_term;_LL425: {void*_tmp647=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp613);void*_tmp648=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp616);struct Cyc_Absyn_Conref*
_tmp649=_tmp618;{void*_tmp64A=(void*)(Cyc_Absyn_compress_conref(_tmp618))->v;
void*_tmp64B;void*_tmp64C;_LL450: if(_tmp64A <= (void*)1?1:*((int*)_tmp64A)!= 0)
goto _LL452;_tmp64B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp64A)->f1;if(
_tmp64B <= (void*)1?1:*((int*)_tmp64B)!= 1)goto _LL452;_tmp64C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp64B)->f1;_LL451: {void*_tmp64D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64C);if(
_tmp64C != _tmp64D)_tmp649=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp64F;_tmp64F.tag=1;_tmp64F.f1=(void*)_tmp64D;_tmp64F;});_tmp64E;}));goto
_LL44F;}_LL452:;_LL453: goto _LL44F;_LL44F:;}if((_tmp647 == _tmp613?_tmp648 == 
_tmp616: 0)?_tmp649 == _tmp618: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=({struct Cyc_Absyn_PointerType_struct
_tmp651;_tmp651.tag=4;_tmp651.f1=({struct Cyc_Absyn_PtrInfo _tmp652;_tmp652.elt_typ=(
void*)_tmp647;_tmp652.elt_tq=_tmp614;_tmp652.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp653;_tmp653.rgn=(void*)_tmp648;_tmp653.nullable=_tmp617;_tmp653.bounds=
_tmp649;_tmp653.zero_term=_tmp619;_tmp653;});_tmp652;});_tmp651;});_tmp650;});}
_LL426: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 8)goto _LL428;_tmp61A=((struct
Cyc_Absyn_FnType_struct*)_tmp5FD)->f1;_tmp61B=_tmp61A.tvars;_tmp61C=_tmp61A.effect;
_tmp61D=(void*)_tmp61A.ret_typ;_tmp61E=_tmp61A.args;_tmp61F=_tmp61A.c_varargs;
_tmp620=_tmp61A.cyc_varargs;_tmp621=_tmp61A.rgn_po;_tmp622=_tmp61A.attributes;
_LL427:{struct Cyc_List_List*_tmp654=_tmp61B;for(0;_tmp654 != 0;_tmp654=_tmp654->tl){
inst=({struct Cyc_List_List*_tmp655=_region_malloc(rgn,sizeof(*_tmp655));_tmp655->hd=({
struct _tuple8*_tmp656=_region_malloc(rgn,sizeof(*_tmp656));_tmp656->f1=(struct
Cyc_Absyn_Tvar*)_tmp654->hd;_tmp656->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657[0]=({struct Cyc_Absyn_VarType_struct
_tmp658;_tmp658.tag=1;_tmp658.f1=(struct Cyc_Absyn_Tvar*)_tmp654->hd;_tmp658;});
_tmp657;});_tmp656;});_tmp655->tl=inst;_tmp655;});}}{struct Cyc_List_List*_tmp65A;
struct Cyc_List_List*_tmp65B;struct _tuple0 _tmp659=((struct _tuple0(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(
struct _RegionHandle*,struct _tuple2*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp61E));_tmp65A=_tmp659.f1;
_tmp65B=_tmp659.f2;{struct Cyc_List_List*_tmp65C=Cyc_Tcutil_substs(rgn,inst,
_tmp65B);struct Cyc_List_List*_tmp65D=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp65A,_tmp65C));struct Cyc_Core_Opt*
eff2;if(_tmp61C == 0)eff2=0;else{void*_tmp65E=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp61C->v);if(_tmp65E == (void*)_tmp61C->v)eff2=_tmp61C;else{eff2=({struct
Cyc_Core_Opt*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->v=(void*)_tmp65E;
_tmp65F;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp620 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp661;struct Cyc_Absyn_Tqual _tmp662;void*
_tmp663;int _tmp664;struct Cyc_Absyn_VarargInfo _tmp660=*_tmp620;_tmp661=_tmp660.name;
_tmp662=_tmp660.tq;_tmp663=(void*)_tmp660.type;_tmp664=_tmp660.inject;{void*
_tmp665=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp663);if(_tmp665 == _tmp663)
cyc_varargs2=_tmp620;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp666=
_cycalloc(sizeof(*_tmp666));_tmp666->name=_tmp661;_tmp666->tq=_tmp662;_tmp666->type=(
void*)_tmp665;_tmp666->inject=_tmp664;_tmp666;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp668;struct Cyc_List_List*_tmp669;struct _tuple0 _tmp667=Cyc_List_rsplit(
rgn,rgn,_tmp621);_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;{struct Cyc_List_List*
_tmp66A=Cyc_Tcutil_substs(rgn,inst,_tmp668);struct Cyc_List_List*_tmp66B=Cyc_Tcutil_substs(
rgn,inst,_tmp669);if(_tmp66A == _tmp668?_tmp66B == _tmp669: 0)rgn_po2=_tmp621;else{
rgn_po2=Cyc_List_zip(_tmp66A,_tmp66B);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Absyn_FnType_struct
_tmp66D;_tmp66D.tag=8;_tmp66D.f1=({struct Cyc_Absyn_FnInfo _tmp66E;_tmp66E.tvars=
_tmp61B;_tmp66E.effect=eff2;_tmp66E.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp61D);_tmp66E.args=_tmp65D;_tmp66E.c_varargs=_tmp61F;_tmp66E.cyc_varargs=
cyc_varargs2;_tmp66E.rgn_po=rgn_po2;_tmp66E.attributes=_tmp622;_tmp66E;});
_tmp66D;});_tmp66C;});}}}}}_LL428: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 9)
goto _LL42A;_tmp623=((struct Cyc_Absyn_TupleType_struct*)_tmp5FD)->f1;_LL429: {
struct Cyc_List_List*_tmp66F=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp623);struct Cyc_List_List*_tmp670=Cyc_Tcutil_substs(rgn,inst,_tmp66F);if(
_tmp670 == _tmp66F)return t;{struct Cyc_List_List*_tmp671=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp623,_tmp670);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=({struct Cyc_Absyn_TupleType_struct
_tmp673;_tmp673.tag=9;_tmp673.f1=_tmp671;_tmp673;});_tmp672;});}}_LL42A: if(
_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 11)goto _LL42C;_tmp624=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5FD)->f1;_tmp625=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5FD)->f2;_LL42B: {
struct Cyc_List_List*_tmp674=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp625);struct Cyc_List_List*_tmp675=Cyc_Tcutil_substs(rgn,inst,_tmp674);if(
_tmp675 == _tmp674)return t;{struct Cyc_List_List*_tmp676=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp625,
_tmp675);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp677=_cycalloc(
sizeof(*_tmp677));_tmp677[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp678;
_tmp678.tag=11;_tmp678.f1=(void*)_tmp624;_tmp678.f2=_tmp676;_tmp678;});_tmp677;});}}
_LL42C: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 0)goto _LL42E;_tmp626=((struct
Cyc_Absyn_Evar_struct*)_tmp5FD)->f2;_LL42D: if(_tmp626 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp626->v);else{return t;}_LL42E: if(_tmp5FD <= (void*)3?1:*((int*)
_tmp5FD)!= 15)goto _LL430;_tmp627=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5FD)->f1;_LL42F: {void*_tmp679=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp627);
return _tmp679 == _tmp627?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp67A=
_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp67B;_tmp67B.tag=15;_tmp67B.f1=(void*)_tmp679;_tmp67B;});_tmp67A;});}_LL430:
if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 14)goto _LL432;_tmp628=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp5FD)->f1;_LL431: {void*_tmp67C=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp628);return _tmp67C == _tmp628?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp67E;_tmp67E.tag=14;_tmp67E.f1=(void*)_tmp67C;_tmp67E;});_tmp67D;});}_LL432:
if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 17)goto _LL434;_tmp629=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp5FD)->f1;_LL433: {void*_tmp67F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp629);return _tmp67F == _tmp629?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_TagType_struct
_tmp681;_tmp681.tag=17;_tmp681.f1=(void*)_tmp67F;_tmp681;});_tmp680;});}_LL434:
if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 18)goto _LL436;_LL435: goto _LL437;
_LL436: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 12)goto _LL438;_LL437: goto
_LL439;_LL438: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 13)goto _LL43A;_LL439:
goto _LL43B;_LL43A: if((int)_tmp5FD != 0)goto _LL43C;_LL43B: goto _LL43D;_LL43C: if(
_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 5)goto _LL43E;_LL43D: goto _LL43F;_LL43E:
if((int)_tmp5FD != 1)goto _LL440;_LL43F: goto _LL441;_LL440: if(_tmp5FD <= (void*)3?1:*((
int*)_tmp5FD)!= 6)goto _LL442;_LL441: goto _LL443;_LL442: if((int)_tmp5FD != 2)goto
_LL444;_LL443: return t;_LL444: if(_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 21)goto
_LL446;_tmp62A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5FD)->f1;_LL445: {
void*_tmp682=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp62A);return _tmp682 == _tmp62A?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp683=_cycalloc(sizeof(*_tmp683));
_tmp683[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp684;_tmp684.tag=21;_tmp684.f1=(
void*)_tmp682;_tmp684;});_tmp683;});}_LL446: if(_tmp5FD <= (void*)3?1:*((int*)
_tmp5FD)!= 19)goto _LL448;_tmp62B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5FD)->f1;_LL447: {void*_tmp685=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp62B);
return _tmp685 == _tmp62B?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp686=
_cycalloc(sizeof(*_tmp686));_tmp686[0]=({struct Cyc_Absyn_AccessEff_struct _tmp687;
_tmp687.tag=19;_tmp687.f1=(void*)_tmp685;_tmp687;});_tmp686;});}_LL448: if(
_tmp5FD <= (void*)3?1:*((int*)_tmp5FD)!= 20)goto _LL417;_tmp62C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp5FD)->f1;_LL449: {struct Cyc_List_List*_tmp688=Cyc_Tcutil_substs(rgn,inst,
_tmp62C);return _tmp688 == _tmp62C?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp68A;_tmp68A.tag=20;_tmp68A.f1=_tmp688;_tmp68A;});_tmp689;});}_LL417:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp68B=(void*)ts->hd;
struct Cyc_List_List*_tmp68C=ts->tl;void*_tmp68D=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp68B);struct Cyc_List_List*_tmp68E=Cyc_Tcutil_substs(rgn,inst,_tmp68C);if(
_tmp68B == _tmp68D?_tmp68C == _tmp68E: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->hd=(void*)
_tmp68D;_tmp68F->tl=_tmp68E;_tmp68F;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp690=_cycalloc(
sizeof(*_tmp690));_tmp690->f1=tv;_tmp690->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->v=(void*)k;_tmp691;}),({struct Cyc_Core_Opt*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->v=s;_tmp692;}));_tmp690;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp694;struct Cyc_List_List*_tmp695;struct _RegionHandle*_tmp696;
struct _tuple9*_tmp693=env;_tmp694=*_tmp693;_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp697=_region_malloc(
_tmp696,sizeof(*_tmp697));_tmp697->f1=tv;_tmp697->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->v=(void*)k;_tmp698;}),({
struct Cyc_Core_Opt*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->v=_tmp695;
_tmp699;}));_tmp697;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp69E;_tmp69E.tag=0;_tmp69E.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2));{struct Cyc_String_pa_struct
_tmp69D;_tmp69D.tag=0;_tmp69D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1));{struct Cyc_String_pa_struct _tmp69C;_tmp69C.tag=0;_tmp69C.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmp69A[3]={& _tmp69C,& _tmp69D,&
_tmp69E};Cyc_Tcutil_terr(loc,({const char*_tmp69B="type variable %s is used with inconsistent kinds %s and %s";
_tag_arr(_tmp69B,sizeof(char),_get_zero_arr_size(_tmp69B,59));}),_tag_arr(
_tmp69A,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))!= *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*_tmp69F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6A0="same type variable has different identity!";_tag_arr(_tmp6A0,sizeof(
char),_get_zero_arr_size(_tmp6A0,43));}),_tag_arr(_tmp69F,sizeof(void*),0));});}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*
_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->hd=tv;_tmp6A1->tl=tvs;_tmp6A1;});}
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp6A2[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp6A3="fast_add_free_tvar: bad identity in tv";
_tag_arr(_tmp6A3,sizeof(char),_get_zero_arr_size(_tmp6A3,39));}),_tag_arr(
_tmp6A2,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp6A4[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6A5="fast_add_free_tvar: bad identity in tvs2";_tag_arr(_tmp6A5,sizeof(char),
_get_zero_arr_size(_tmp6A5,41));}),_tag_arr(_tmp6A4,sizeof(void*),0));});if(*((
int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)
_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*_tmp6A6=
_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=tv;_tmp6A6->tl=tvs;_tmp6A6;});}static
struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*
tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp6A9;_tmp6A9.tag=0;
_tmp6A9.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string(tv));{
void*_tmp6A7[1]={& _tmp6A9};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp6A8="bound tvar id for %s is NULL";_tag_arr(
_tmp6A8,sizeof(char),_get_zero_arr_size(_tmp6A8,29));}),_tag_arr(_tmp6A7,sizeof(
void*),1));}});return({struct Cyc_List_List*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));
_tmp6AA->hd=tv;_tmp6AA->tl=tvs;_tmp6AA;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmp6AB=Cyc_Tcutil_compress(e);int _tmp6AC;
_LL455: if(_tmp6AB <= (void*)3?1:*((int*)_tmp6AB)!= 0)goto _LL457;_tmp6AC=((struct
Cyc_Absyn_Evar_struct*)_tmp6AB)->f3;_LL456:{struct Cyc_List_List*es2=es;for(0;es2
!= 0;es2=es2->tl){void*_tmp6AD=Cyc_Tcutil_compress((void*)es2->hd);int _tmp6AE;
_LL45A: if(_tmp6AD <= (void*)3?1:*((int*)_tmp6AD)!= 0)goto _LL45C;_tmp6AE=((struct
Cyc_Absyn_Evar_struct*)_tmp6AD)->f3;_LL45B: if(_tmp6AC == _tmp6AE)return es;goto
_LL459;_LL45C:;_LL45D: goto _LL459;_LL459:;}}return({struct Cyc_List_List*_tmp6AF=
_cycalloc(sizeof(*_tmp6AF));_tmp6AF->hd=(void*)e;_tmp6AF->tl=es;_tmp6AF;});
_LL457:;_LL458: return es;_LL454:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6B0->tl=r;_tmp6B0;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp6B3;_tmp6B3.tag=
0;_tmp6B3.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{void*_tmp6B1[1]={&
_tmp6B3};Cyc_Tcutil_terr(loc,({const char*_tmp6B2="bitfield %s does not have constant width";
_tag_arr(_tmp6B2,sizeof(char),_get_zero_arr_size(_tmp6B2,41));}),_tag_arr(
_tmp6B1,sizeof(void*),1));}});else{unsigned int _tmp6B5;int _tmp6B6;struct _tuple7
_tmp6B4=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmp6B5=
_tmp6B4.f1;_tmp6B6=_tmp6B4.f2;if(!_tmp6B6)({void*_tmp6B7[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp6B8="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp6B8,sizeof(char),_get_zero_arr_size(_tmp6B8,45));}),_tag_arr(_tmp6B7,sizeof(
void*),0));});w=_tmp6B5;}{void*_tmp6B9=Cyc_Tcutil_compress(field_typ);void*
_tmp6BA;_LL45F: if(_tmp6B9 <= (void*)3?1:*((int*)_tmp6B9)!= 5)goto _LL461;_tmp6BA=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B9)->f2;_LL460:{void*_tmp6BB=
_tmp6BA;_LL464: if((int)_tmp6BB != 0)goto _LL466;_LL465: if(w > 8)({void*_tmp6BC[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp6BD="bitfield larger than type";_tag_arr(
_tmp6BD,sizeof(char),_get_zero_arr_size(_tmp6BD,26));}),_tag_arr(_tmp6BC,sizeof(
void*),0));});goto _LL463;_LL466: if((int)_tmp6BB != 1)goto _LL468;_LL467: if(w > 16)({
void*_tmp6BE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6BF="bitfield larger than type";
_tag_arr(_tmp6BF,sizeof(char),_get_zero_arr_size(_tmp6BF,26));}),_tag_arr(
_tmp6BE,sizeof(void*),0));});goto _LL463;_LL468: if((int)_tmp6BB != 2)goto _LL46A;
_LL469: if(w > 32)({void*_tmp6C0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6C1="bitfield larger than type";
_tag_arr(_tmp6C1,sizeof(char),_get_zero_arr_size(_tmp6C1,26));}),_tag_arr(
_tmp6C0,sizeof(void*),0));});goto _LL463;_LL46A: if((int)_tmp6BB != 3)goto _LL463;
_LL46B: if(w > 64)({void*_tmp6C2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6C3="bitfield larger than type";
_tag_arr(_tmp6C3,sizeof(char),_get_zero_arr_size(_tmp6C3,26));}),_tag_arr(
_tmp6C2,sizeof(void*),0));});goto _LL463;_LL463:;}goto _LL45E;_LL461:;_LL462:({
struct Cyc_String_pa_struct _tmp6C7;_tmp6C7.tag=0;_tmp6C7.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ));{struct Cyc_String_pa_struct
_tmp6C6;_tmp6C6.tag=0;_tmp6C6.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
void*_tmp6C4[2]={& _tmp6C6,& _tmp6C7};Cyc_Tcutil_terr(loc,({const char*_tmp6C5="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp6C5,sizeof(char),_get_zero_arr_size(_tmp6C5,52));}),_tag_arr(
_tmp6C4,sizeof(void*),2));}}});goto _LL45E;_LL45E:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp6C8=(void*)atts->hd;_LL46D: if((int)
_tmp6C8 != 5)goto _LL46F;_LL46E: continue;_LL46F: if(_tmp6C8 <= (void*)17?1:*((int*)
_tmp6C8)!= 1)goto _LL471;_LL470: continue;_LL471:;_LL472:({struct Cyc_String_pa_struct
_tmp6CC;_tmp6CC.tag=0;_tmp6CC.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_String_pa_struct _tmp6CB;_tmp6CB.tag=0;_tmp6CB.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp6C9[2]={&
_tmp6CB,& _tmp6CC};Cyc_Tcutil_terr(loc,({const char*_tmp6CA="bad attribute %s on member %s";
_tag_arr(_tmp6CA,sizeof(char),_get_zero_arr_size(_tmp6CA,30));}),_tag_arr(
_tmp6C9,sizeof(void*),2));}}});_LL46C:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp6CD=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp6CE;struct Cyc_Core_Opt**_tmp6CF;struct Cyc_Core_Opt*
_tmp6D0;struct Cyc_Core_Opt**_tmp6D1;struct Cyc_Absyn_Tvar*_tmp6D2;struct Cyc_List_List*
_tmp6D3;struct _tuple1*_tmp6D4;struct Cyc_Absyn_Enumdecl*_tmp6D5;struct Cyc_Absyn_Enumdecl**
_tmp6D6;struct Cyc_Absyn_TunionInfo _tmp6D7;void*_tmp6D8;void**_tmp6D9;struct Cyc_List_List*
_tmp6DA;struct Cyc_List_List**_tmp6DB;void*_tmp6DC;struct Cyc_Absyn_TunionFieldInfo
_tmp6DD;void*_tmp6DE;void**_tmp6DF;struct Cyc_List_List*_tmp6E0;struct Cyc_Absyn_PtrInfo
_tmp6E1;void*_tmp6E2;struct Cyc_Absyn_Tqual _tmp6E3;struct Cyc_Absyn_PtrAtts _tmp6E4;
void*_tmp6E5;struct Cyc_Absyn_Conref*_tmp6E6;struct Cyc_Absyn_Conref*_tmp6E7;
struct Cyc_Absyn_Conref*_tmp6E8;void*_tmp6E9;void*_tmp6EA;struct Cyc_Absyn_ArrayInfo
_tmp6EB;void*_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;struct Cyc_Absyn_Exp*_tmp6EE;
struct Cyc_Absyn_Conref*_tmp6EF;struct Cyc_Absyn_FnInfo _tmp6F0;struct Cyc_List_List*
_tmp6F1;struct Cyc_List_List**_tmp6F2;struct Cyc_Core_Opt*_tmp6F3;struct Cyc_Core_Opt**
_tmp6F4;void*_tmp6F5;struct Cyc_List_List*_tmp6F6;int _tmp6F7;struct Cyc_Absyn_VarargInfo*
_tmp6F8;struct Cyc_List_List*_tmp6F9;struct Cyc_List_List*_tmp6FA;struct Cyc_List_List*
_tmp6FB;void*_tmp6FC;struct Cyc_List_List*_tmp6FD;struct Cyc_Absyn_AggrInfo _tmp6FE;
void*_tmp6FF;void**_tmp700;struct Cyc_List_List*_tmp701;struct Cyc_List_List**
_tmp702;struct _tuple1*_tmp703;struct Cyc_List_List*_tmp704;struct Cyc_List_List**
_tmp705;struct Cyc_Absyn_Typedefdecl*_tmp706;struct Cyc_Absyn_Typedefdecl**_tmp707;
void**_tmp708;void***_tmp709;void*_tmp70A;void*_tmp70B;void*_tmp70C;struct Cyc_List_List*
_tmp70D;_LL474: if((int)_tmp6CD != 0)goto _LL476;_LL475: goto _LL473;_LL476: if(
_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 0)goto _LL478;_tmp6CE=((struct Cyc_Absyn_Evar_struct*)
_tmp6CD)->f1;_tmp6CF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp6CD)->f1;_tmp6D0=((struct Cyc_Absyn_Evar_struct*)_tmp6CD)->f2;_tmp6D1=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6CD)->f2;_LL477: if(*_tmp6CF == 
0)*_tmp6CF=({struct Cyc_Core_Opt*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->v=(
void*)expected_kind;_tmp70E;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp6D1=({struct Cyc_Core_Opt*_tmp70F=_cycalloc(
sizeof(*_tmp70F));_tmp70F->v=(void*)((void*)2);_tmp70F;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp710=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp715;_tmp715.tag=2;_tmp715.f1=0;_tmp715.f2=(void*)expected_kind;_tmp715;});
_tmp714;}));*_tmp6D1=({struct Cyc_Core_Opt*_tmp711=_cycalloc(sizeof(*_tmp711));
_tmp711->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp712=_cycalloc(
sizeof(*_tmp712));_tmp712[0]=({struct Cyc_Absyn_VarType_struct _tmp713;_tmp713.tag=
1;_tmp713.f1=_tmp710;_tmp713;});_tmp712;}));_tmp711;});_tmp6D2=_tmp710;goto
_LL479;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL473;_LL478: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 1)goto _LL47A;
_tmp6D2=((struct Cyc_Absyn_VarType_struct*)_tmp6CD)->f1;_LL479:{void*_tmp716=Cyc_Absyn_compress_kb((
void*)_tmp6D2->kind);struct Cyc_Core_Opt*_tmp717;struct Cyc_Core_Opt**_tmp718;
_LL4A7: if(*((int*)_tmp716)!= 1)goto _LL4A9;_tmp717=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp716)->f1;_tmp718=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp716)->f1;_LL4A8:*_tmp718=({struct Cyc_Core_Opt*_tmp719=_cycalloc(sizeof(*
_tmp719));_tmp719->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Absyn_Less_kb_struct _tmp71B;
_tmp71B.tag=2;_tmp71B.f1=0;_tmp71B.f2=(void*)expected_kind;_tmp71B;});_tmp71A;}));
_tmp719;});goto _LL4A6;_LL4A9:;_LL4AA: goto _LL4A6;_LL4A6:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp6D2);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp6D2);goto _LL473;_LL47A: if(_tmp6CD <= (void*)3?1:*((int*)
_tmp6CD)!= 13)goto _LL47C;_tmp6D3=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6CD)->f1;
_LL47B: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp71C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp71C;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp6D3 != 0;_tmp6D3=_tmp6D3->tl){struct Cyc_Absyn_Enumfield*_tmp71D=(
struct Cyc_Absyn_Enumfield*)_tmp6D3->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp71D->name).f2))({struct Cyc_String_pa_struct
_tmp720;_tmp720.tag=0;_tmp720.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp71D->name).f2);{void*_tmp71E[1]={& _tmp720};Cyc_Tcutil_terr(_tmp71D->loc,({
const char*_tmp71F="duplicate enum field name %s";_tag_arr(_tmp71F,sizeof(char),
_get_zero_arr_size(_tmp71F,29));}),_tag_arr(_tmp71E,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp721=_region_malloc(uprev_rgn,sizeof(*
_tmp721));_tmp721->hd=(*_tmp71D->name).f2;_tmp721->tl=prev_fields;_tmp721;});}
if(_tmp71D->tag == 0)_tmp71D->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp71D->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp71D->tag)))({struct Cyc_String_pa_struct _tmp724;_tmp724.tag=0;
_tmp724.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp71D->name).f2);{void*
_tmp722[1]={& _tmp724};Cyc_Tcutil_terr(loc,({const char*_tmp723="enum field %s: expression is not constant";
_tag_arr(_tmp723,sizeof(char),_get_zero_arr_size(_tmp723,42));}),_tag_arr(
_tmp722,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp71D->tag))).f1;tag_count=t1 + 1;(*_tmp71D->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp726;_tmp726.tag=1;_tmp726.f1=te->ns;_tmp726;});
_tmp725;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp71D->name).f2,({
struct _tuple17*_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp729;_tmp729.tag=4;_tmp729.f1=(void*)t;
_tmp729.f2=_tmp71D;_tmp729;});_tmp728;});_tmp727->f2=1;_tmp727;}));}}};
_pop_region(uprev_rgn);}goto _LL473;}_LL47C: if(_tmp6CD <= (void*)3?1:*((int*)
_tmp6CD)!= 12)goto _LL47E;_tmp6D4=((struct Cyc_Absyn_EnumType_struct*)_tmp6CD)->f1;
_tmp6D5=((struct Cyc_Absyn_EnumType_struct*)_tmp6CD)->f2;_tmp6D6=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp6CD)->f2;_LL47D: if(*_tmp6D6 == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp6D6))->fields == 0){struct _handler_cons
_tmp72A;_push_handler(& _tmp72A);{int _tmp72C=0;if(setjmp(_tmp72A.handler))_tmp72C=
1;if(!_tmp72C){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp6D4);*_tmp6D6=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp72B=(void*)_exn_thrown;void*_tmp72E=_tmp72B;_LL4AC: if(_tmp72E != Cyc_Dict_Absent)
goto _LL4AE;_LL4AD: {struct Cyc_Tcenv_Genv*_tmp72F=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp730=({struct Cyc_Absyn_Enumdecl*_tmp731=_cycalloc(
sizeof(*_tmp731));_tmp731->sc=(void*)((void*)3);_tmp731->name=_tmp6D4;_tmp731->fields=
0;_tmp731;});Cyc_Tc_tcEnumdecl(te,_tmp72F,loc,_tmp730);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D4);*_tmp6D6=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL4AB;}}_LL4AE:;_LL4AF:(void)_throw(_tmp72E);_LL4AB:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6D6);*_tmp6D4=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL473;}_LL47E: if(_tmp6CD <= (void*)3?1:*((
int*)_tmp6CD)!= 2)goto _LL480;_tmp6D7=((struct Cyc_Absyn_TunionType_struct*)
_tmp6CD)->f1;_tmp6D8=(void*)_tmp6D7.tunion_info;_tmp6D9=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp6CD)->f1).tunion_info;_tmp6DA=_tmp6D7.targs;_tmp6DB=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp6CD)->f1).targs;_tmp6DC=(void*)_tmp6D7.rgn;
_LL47F: {struct Cyc_List_List*_tmp732=*_tmp6DB;{void*_tmp733=*_tmp6D9;struct Cyc_Absyn_UnknownTunionInfo
_tmp734;struct _tuple1*_tmp735;int _tmp736;struct Cyc_Absyn_Tuniondecl**_tmp737;
struct Cyc_Absyn_Tuniondecl*_tmp738;_LL4B1: if(*((int*)_tmp733)!= 0)goto _LL4B3;
_tmp734=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp733)->f1;_tmp735=_tmp734.name;
_tmp736=_tmp734.is_xtunion;_LL4B2: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp739;_push_handler(& _tmp739);{int _tmp73B=0;if(setjmp(_tmp739.handler))
_tmp73B=1;if(!_tmp73B){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp735);;
_pop_handler();}else{void*_tmp73A=(void*)_exn_thrown;void*_tmp73D=_tmp73A;_LL4B6:
if(_tmp73D != Cyc_Dict_Absent)goto _LL4B8;_LL4B7: {struct Cyc_Tcenv_Genv*_tmp73E=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp73F=({struct Cyc_Absyn_Tuniondecl*
_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->sc=(void*)((void*)3);_tmp746->name=
_tmp735;_tmp746->tvs=0;_tmp746->fields=0;_tmp746->is_xtunion=_tmp736;_tmp746;});
Cyc_Tc_tcTuniondecl(te,_tmp73E,loc,_tmp73F);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp735);if(_tmp732 != 0){({struct Cyc_String_pa_struct _tmp743;_tmp743.tag=0;
_tmp743.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp735));{struct Cyc_String_pa_struct _tmp742;_tmp742.tag=0;_tmp742.f1=(struct
_tagged_arr)(_tmp736?({const char*_tmp744="xtunion";_tag_arr(_tmp744,sizeof(char),
_get_zero_arr_size(_tmp744,8));}):({const char*_tmp745="tunion";_tag_arr(_tmp745,
sizeof(char),_get_zero_arr_size(_tmp745,7));}));{void*_tmp740[2]={& _tmp742,&
_tmp743};Cyc_Tcutil_terr(loc,({const char*_tmp741="declare parameterized %s %s before using";
_tag_arr(_tmp741,sizeof(char),_get_zero_arr_size(_tmp741,41));}),_tag_arr(
_tmp740,sizeof(void*),2));}}});return cvtenv;}goto _LL4B5;}_LL4B8:;_LL4B9:(void)
_throw(_tmp73D);_LL4B5:;}}}if((*tudp)->is_xtunion != _tmp736)({struct Cyc_String_pa_struct
_tmp749;_tmp749.tag=0;_tmp749.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp735));{void*_tmp747[1]={& _tmp749};Cyc_Tcutil_terr(loc,({const char*_tmp748="[x]tunion is different from type declaration %s";
_tag_arr(_tmp748,sizeof(char),_get_zero_arr_size(_tmp748,48));}),_tag_arr(
_tmp747,sizeof(void*),1));}});*_tmp6D9=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp74B;_tmp74B.tag=1;_tmp74B.f1=tudp;_tmp74B;});_tmp74A;});_tmp738=*tudp;goto
_LL4B4;}_LL4B3: if(*((int*)_tmp733)!= 1)goto _LL4B0;_tmp737=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp733)->f1;_tmp738=*_tmp737;_LL4B4: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp6DC);{struct Cyc_List_List*tvs=_tmp738->tvs;for(0;_tmp732 != 0?
tvs != 0: 0;(_tmp732=_tmp732->tl,tvs=tvs->tl)){void*t1=(void*)_tmp732->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp732 != 0)({struct Cyc_String_pa_struct _tmp74E;_tmp74E.tag=
0;_tmp74E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp738->name));{void*_tmp74C[1]={& _tmp74E};Cyc_Tcutil_terr(loc,({const char*
_tmp74D="too many type arguments for tunion %s";_tag_arr(_tmp74D,sizeof(char),
_get_zero_arr_size(_tmp74D,38));}),_tag_arr(_tmp74C,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=(void*)e;_tmp74F->tl=
hidden_ts;_tmp74F;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp6DB=Cyc_List_imp_append(*_tmp6DB,Cyc_List_imp_rev(hidden_ts));}goto _LL4B0;}
_LL4B0:;}goto _LL473;}_LL480: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 3)goto
_LL482;_tmp6DD=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6CD)->f1;_tmp6DE=(
void*)_tmp6DD.field_info;_tmp6DF=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6CD)->f1).field_info;_tmp6E0=_tmp6DD.targs;_LL481:{void*_tmp750=*_tmp6DF;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp751;struct _tuple1*_tmp752;struct
_tuple1*_tmp753;int _tmp754;struct Cyc_Absyn_Tuniondecl*_tmp755;struct Cyc_Absyn_Tunionfield*
_tmp756;_LL4BB: if(*((int*)_tmp750)!= 0)goto _LL4BD;_tmp751=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp750)->f1;_tmp752=_tmp751.tunion_name;_tmp753=_tmp751.field_name;_tmp754=
_tmp751.is_xtunion;_LL4BC: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp757;_push_handler(& _tmp757);{int _tmp759=0;if(setjmp(
_tmp757.handler))_tmp759=1;if(!_tmp759){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp752);;_pop_handler();}else{void*_tmp758=(void*)_exn_thrown;void*_tmp75B=
_tmp758;_LL4C0: if(_tmp75B != Cyc_Dict_Absent)goto _LL4C2;_LL4C1:({struct Cyc_String_pa_struct
_tmp75E;_tmp75E.tag=0;_tmp75E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp752));{void*_tmp75C[1]={& _tmp75E};Cyc_Tcutil_terr(loc,({const char*_tmp75D="unbound type tunion %s";
_tag_arr(_tmp75D,sizeof(char),_get_zero_arr_size(_tmp75D,23));}),_tag_arr(
_tmp75C,sizeof(void*),1));}});return cvtenv;_LL4C2:;_LL4C3:(void)_throw(_tmp75B);
_LL4BF:;}}}{struct _handler_cons _tmp75F;_push_handler(& _tmp75F);{int _tmp761=0;if(
setjmp(_tmp75F.handler))_tmp761=1;if(!_tmp761){{void*_tmp762=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp753);struct Cyc_Absyn_Tuniondecl*_tmp763;struct Cyc_Absyn_Tunionfield*
_tmp764;_LL4C5: if(*((int*)_tmp762)!= 2)goto _LL4C7;_tmp763=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp762)->f1;_tmp764=((struct Cyc_Tcenv_TunionRes_struct*)_tmp762)->f2;_LL4C6: tuf=
_tmp764;tud=_tmp763;if(tud->is_xtunion != _tmp754)({struct Cyc_String_pa_struct
_tmp767;_tmp767.tag=0;_tmp767.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp752));{void*_tmp765[1]={& _tmp767};Cyc_Tcutil_terr(loc,({const char*_tmp766="[x]tunion is different from type declaration %s";
_tag_arr(_tmp766,sizeof(char),_get_zero_arr_size(_tmp766,48));}),_tag_arr(
_tmp765,sizeof(void*),1));}});goto _LL4C4;_LL4C7:;_LL4C8:({struct Cyc_String_pa_struct
_tmp76B;_tmp76B.tag=0;_tmp76B.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp752));{struct Cyc_String_pa_struct _tmp76A;_tmp76A.tag=0;_tmp76A.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp753));{void*_tmp768[
2]={& _tmp76A,& _tmp76B};Cyc_Tcutil_terr(loc,({const char*_tmp769="unbound field %s in type tunion %s";
_tag_arr(_tmp769,sizeof(char),_get_zero_arr_size(_tmp769,35));}),_tag_arr(
_tmp768,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp76C=cvtenv;
_npop_handler(0);return _tmp76C;}_LL4C4:;};_pop_handler();}else{void*_tmp760=(
void*)_exn_thrown;void*_tmp76E=_tmp760;_LL4CA: if(_tmp76E != Cyc_Dict_Absent)goto
_LL4CC;_LL4CB:({struct Cyc_String_pa_struct _tmp772;_tmp772.tag=0;_tmp772.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp752));{struct
Cyc_String_pa_struct _tmp771;_tmp771.tag=0;_tmp771.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp753));{void*_tmp76F[2]={& _tmp771,&
_tmp772};Cyc_Tcutil_terr(loc,({const char*_tmp770="unbound field %s in type tunion %s";
_tag_arr(_tmp770,sizeof(char),_get_zero_arr_size(_tmp770,35));}),_tag_arr(
_tmp76F,sizeof(void*),2));}}});return cvtenv;_LL4CC:;_LL4CD:(void)_throw(_tmp76E);
_LL4C9:;}}}*_tmp6DF=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp773=
_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp774;_tmp774.tag=1;_tmp774.f1=tud;_tmp774.f2=tuf;_tmp774;});_tmp773;});
_tmp755=tud;_tmp756=tuf;goto _LL4BE;}_LL4BD: if(*((int*)_tmp750)!= 1)goto _LL4BA;
_tmp755=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp750)->f1;_tmp756=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp750)->f2;_LL4BE: {struct Cyc_List_List*tvs=
_tmp755->tvs;for(0;_tmp6E0 != 0?tvs != 0: 0;(_tmp6E0=_tmp6E0->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp6E0->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp6E0 != 0)({
struct Cyc_String_pa_struct _tmp778;_tmp778.tag=0;_tmp778.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp756->name));{struct Cyc_String_pa_struct
_tmp777;_tmp777.tag=0;_tmp777.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp755->name));{void*_tmp775[2]={& _tmp777,& _tmp778};Cyc_Tcutil_terr(loc,({const
char*_tmp776="too many type arguments for tunion %s.%s";_tag_arr(_tmp776,sizeof(
char),_get_zero_arr_size(_tmp776,41));}),_tag_arr(_tmp775,sizeof(void*),2));}}});
if(tvs != 0)({struct Cyc_String_pa_struct _tmp77C;_tmp77C.tag=0;_tmp77C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp756->name));{struct
Cyc_String_pa_struct _tmp77B;_tmp77B.tag=0;_tmp77B.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp755->name));{void*_tmp779[2]={& _tmp77B,&
_tmp77C};Cyc_Tcutil_terr(loc,({const char*_tmp77A="too few type arguments for tunion %s.%s";
_tag_arr(_tmp77A,sizeof(char),_get_zero_arr_size(_tmp77A,40));}),_tag_arr(
_tmp779,sizeof(void*),2));}}});goto _LL4BA;}_LL4BA:;}goto _LL473;_LL482: if(_tmp6CD
<= (void*)3?1:*((int*)_tmp6CD)!= 4)goto _LL484;_tmp6E1=((struct Cyc_Absyn_PointerType_struct*)
_tmp6CD)->f1;_tmp6E2=(void*)_tmp6E1.elt_typ;_tmp6E3=_tmp6E1.elt_tq;_tmp6E4=
_tmp6E1.ptr_atts;_tmp6E5=(void*)_tmp6E4.rgn;_tmp6E6=_tmp6E4.nullable;_tmp6E7=
_tmp6E4.bounds;_tmp6E8=_tmp6E4.zero_term;_LL483: {int is_zero_terminated;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp6E2);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp6E5);{void*_tmp77D=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp6E8))->v;int _tmp77E;
_LL4CF: if((int)_tmp77D != 0)goto _LL4D1;_LL4D0:{void*_tmp77F=Cyc_Tcutil_compress(
_tmp6E2);void*_tmp780;_LL4D6: if(_tmp77F <= (void*)3?1:*((int*)_tmp77F)!= 5)goto
_LL4D8;_tmp780=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp77F)->f2;if((int)
_tmp780 != 0)goto _LL4D8;_LL4D7:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6E8,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4D5;_LL4D8:;_LL4D9:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp6E8,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4D5;
_LL4D5:;}goto _LL4CE;_LL4D1: if(_tmp77D <= (void*)1?1:*((int*)_tmp77D)!= 0)goto
_LL4D3;_tmp77E=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp77D)->f1;if(_tmp77E
!= 1)goto _LL4D3;_LL4D2: if(!Cyc_Tcutil_admits_zero(_tmp6E2))({struct Cyc_String_pa_struct
_tmp783;_tmp783.tag=0;_tmp783.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp6E2));{void*_tmp781[1]={& _tmp783};Cyc_Tcutil_terr(loc,({const char*_tmp782="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp782,sizeof(char),_get_zero_arr_size(_tmp782,53));}),_tag_arr(
_tmp781,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4CE;_LL4D3:;_LL4D4:
is_zero_terminated=0;goto _LL4CE;_LL4CE:;}{void*_tmp784=(void*)(Cyc_Absyn_compress_conref(
_tmp6E7))->v;void*_tmp785;void*_tmp786;struct Cyc_Absyn_Exp*_tmp787;void*_tmp788;
void*_tmp789;_LL4DB: if((int)_tmp784 != 0)goto _LL4DD;_LL4DC: goto _LL4DE;_LL4DD: if(
_tmp784 <= (void*)1?1:*((int*)_tmp784)!= 0)goto _LL4DF;_tmp785=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp784)->f1;if((int)_tmp785 != 0)goto _LL4DF;_LL4DE: goto _LL4DA;_LL4DF: if(_tmp784
<= (void*)1?1:*((int*)_tmp784)!= 0)goto _LL4E1;_tmp786=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp784)->f1;if(_tmp786 <= (void*)1?1:*((int*)_tmp786)!= 0)goto _LL4E1;_tmp787=((
struct Cyc_Absyn_Upper_b_struct*)_tmp786)->f1;_LL4E0: Cyc_Tcexp_tcExp(te,0,_tmp787);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp787))({void*_tmp78A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp78B="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp78B,sizeof(char),_get_zero_arr_size(_tmp78B,49));}),_tag_arr(
_tmp78A,sizeof(void*),0));});{unsigned int _tmp78D;int _tmp78E;struct _tuple7
_tmp78C=Cyc_Evexp_eval_const_uint_exp(_tmp787);_tmp78D=_tmp78C.f1;_tmp78E=
_tmp78C.f2;if(is_zero_terminated?!_tmp78E?1: _tmp78D < 1: 0)({void*_tmp78F[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp790="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp790,sizeof(char),_get_zero_arr_size(_tmp790,55));}),_tag_arr(
_tmp78F,sizeof(void*),0));});goto _LL4DA;}_LL4E1: if(_tmp784 <= (void*)1?1:*((int*)
_tmp784)!= 0)goto _LL4E3;_tmp788=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp784)->f1;if(_tmp788 <= (void*)1?1:*((int*)_tmp788)!= 1)goto _LL4E3;_tmp789=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp788)->f1;_LL4E2: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp789);goto _LL4DA;_LL4E3: if(_tmp784 <= (void*)1?1:*((int*)
_tmp784)!= 1)goto _LL4DA;_LL4E4:({void*_tmp791[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp792="forward constraint";
_tag_arr(_tmp792,sizeof(char),_get_zero_arr_size(_tmp792,19));}),_tag_arr(
_tmp791,sizeof(void*),0));});_LL4DA:;}goto _LL473;}_LL484: if(_tmp6CD <= (void*)3?1:*((
int*)_tmp6CD)!= 17)goto _LL486;_tmp6E9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6CD)->f1;_LL485: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp6E9);goto _LL473;_LL486: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 14)goto
_LL488;_tmp6EA=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp6CD)->f1;_LL487:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp6EA);goto _LL473;
_LL488: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 5)goto _LL48A;_LL489: goto _LL48B;
_LL48A: if((int)_tmp6CD != 1)goto _LL48C;_LL48B: goto _LL48D;_LL48C: if(_tmp6CD <= (
void*)3?1:*((int*)_tmp6CD)!= 6)goto _LL48E;_LL48D: goto _LL473;_LL48E: if(_tmp6CD <= (
void*)3?1:*((int*)_tmp6CD)!= 7)goto _LL490;_tmp6EB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6CD)->f1;_tmp6EC=(void*)_tmp6EB.elt_type;_tmp6ED=_tmp6EB.tq;_tmp6EE=_tmp6EB.num_elts;
_tmp6EF=_tmp6EB.zero_term;_LL48F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp6EC);{int is_zero_terminated;{void*_tmp793=(void*)(((struct
Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp6EF))->v;int _tmp794;_LL4E6: if((int)_tmp793 != 0)goto _LL4E8;_LL4E7:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp6EF,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4E5;
_LL4E8: if(_tmp793 <= (void*)1?1:*((int*)_tmp793)!= 0)goto _LL4EA;_tmp794=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp793)->f1;if(_tmp794 != 1)goto _LL4EA;_LL4E9:
if(!Cyc_Tcutil_admits_zero(_tmp6EC))({struct Cyc_String_pa_struct _tmp797;_tmp797.tag=
0;_tmp797.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp6EC));{void*_tmp795[1]={& _tmp797};Cyc_Tcutil_terr(loc,({const char*_tmp796="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp796,sizeof(char),_get_zero_arr_size(_tmp796,51));}),_tag_arr(
_tmp795,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4E5;_LL4EA:;_LL4EB:
is_zero_terminated=0;goto _LL4E5;_LL4E5:;}if(_tmp6EE == 0)({void*_tmp798[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp799="an explicit array bound is required here";_tag_arr(
_tmp799,sizeof(char),_get_zero_arr_size(_tmp799,41));}),_tag_arr(_tmp798,sizeof(
void*),0));});else{Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp6EE);if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_tmp6EE))({void*_tmp79A[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp79B="array bounds expression is not constant";_tag_arr(_tmp79B,
sizeof(char),_get_zero_arr_size(_tmp79B,40));}),_tag_arr(_tmp79A,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp6EE))({void*_tmp79C[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp79D="array bounds expression is not an unsigned int";
_tag_arr(_tmp79D,sizeof(char),_get_zero_arr_size(_tmp79D,47));}),_tag_arr(
_tmp79C,sizeof(void*),0));});{unsigned int _tmp79F;int _tmp7A0;struct _tuple7
_tmp79E=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp6EE);_tmp79F=
_tmp79E.f1;_tmp7A0=_tmp79E.f2;if((is_zero_terminated?_tmp7A0: 0)?_tmp79F < 1: 0)({
void*_tmp7A1[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7A2="zero terminated array cannot have zero elements";
_tag_arr(_tmp7A2,sizeof(char),_get_zero_arr_size(_tmp7A2,48));}),_tag_arr(
_tmp7A1,sizeof(void*),0));});}}goto _LL473;}_LL490: if(_tmp6CD <= (void*)3?1:*((int*)
_tmp6CD)!= 8)goto _LL492;_tmp6F0=((struct Cyc_Absyn_FnType_struct*)_tmp6CD)->f1;
_tmp6F1=_tmp6F0.tvars;_tmp6F2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp6CD)->f1).tvars;_tmp6F3=_tmp6F0.effect;_tmp6F4=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp6CD)->f1).effect;_tmp6F5=(void*)_tmp6F0.ret_typ;
_tmp6F6=_tmp6F0.args;_tmp6F7=_tmp6F0.c_varargs;_tmp6F8=_tmp6F0.cyc_varargs;
_tmp6F9=_tmp6F0.rgn_po;_tmp6FA=_tmp6F0.attributes;_LL491: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp6FA != 0;_tmp6FA=_tmp6FA->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp6FA->hd))({struct Cyc_String_pa_struct _tmp7A5;
_tmp7A5.tag=0;_tmp7A5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp6FA->hd));{void*_tmp7A3[1]={& _tmp7A5};Cyc_Tcutil_terr(loc,({const char*
_tmp7A4="bad function type attribute %s";_tag_arr(_tmp7A4,sizeof(char),
_get_zero_arr_size(_tmp7A4,31));}),_tag_arr(_tmp7A3,sizeof(void*),1));}});{void*
_tmp7A6=(void*)_tmp6FA->hd;void*_tmp7A7;int _tmp7A8;int _tmp7A9;_LL4ED: if((int)
_tmp7A6 != 0)goto _LL4EF;_LL4EE: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL4EC;_LL4EF: if((int)_tmp7A6 != 1)goto _LL4F1;_LL4F0: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL4EC;_LL4F1: if((int)_tmp7A6 != 2)goto _LL4F3;_LL4F2: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL4EC;_LL4F3: if(_tmp7A6 <= (void*)
17?1:*((int*)_tmp7A6)!= 3)goto _LL4F5;_tmp7A7=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp7A6)->f1;_tmp7A8=((struct Cyc_Absyn_Format_att_struct*)_tmp7A6)->f2;_tmp7A9=((
struct Cyc_Absyn_Format_att_struct*)_tmp7A6)->f3;_LL4F4: if(!seen_format){
seen_format=1;ft=_tmp7A7;fmt_desc_arg=_tmp7A8;fmt_arg_start=_tmp7A9;}else{({void*
_tmp7AA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7AB="function can't have multiple format attributes";
_tag_arr(_tmp7AB,sizeof(char),_get_zero_arr_size(_tmp7AB,47));}),_tag_arr(
_tmp7AA,sizeof(void*),0));});}goto _LL4EC;_LL4F5:;_LL4F6: goto _LL4EC;_LL4EC:;}}if(
num_convs > 1)({void*_tmp7AC[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7AD="function can't have multiple calling conventions";
_tag_arr(_tmp7AD,sizeof(char),_get_zero_arr_size(_tmp7AD,49));}),_tag_arr(
_tmp7AC,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp6F2);{struct
Cyc_List_List*b=*_tmp6F2;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp7AE=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7AF;_LL4F8: if(*((int*)_tmp7AE)!= 0)goto
_LL4FA;_tmp7AF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7AE)->f1;if((int)
_tmp7AF != 1)goto _LL4FA;_LL4F9:({struct Cyc_String_pa_struct _tmp7B2;_tmp7B2.tag=0;
_tmp7B2.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp7B0[1]={& _tmp7B2};Cyc_Tcutil_terr(loc,({const char*_tmp7B1="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp7B1,sizeof(char),_get_zero_arr_size(_tmp7B1,51));}),_tag_arr(
_tmp7B0,sizeof(void*),1));}});goto _LL4F7;_LL4FA:;_LL4FB: goto _LL4F7;_LL4F7:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp7B3=({struct Cyc_Tcutil_CVTEnv _tmp82D;_tmp82D.kind_env=
cvtenv.kind_env;_tmp82D.free_vars=0;_tmp82D.free_evars=0;_tmp82D.generalize_evars=
cvtenv.generalize_evars;_tmp82D.fn_result=1;_tmp82D;});_tmp7B3=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7B3,(void*)1,_tmp6F5);_tmp7B3.fn_result=0;{struct Cyc_List_List*a=
_tmp6F6;for(0;a != 0;a=a->tl){_tmp7B3=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7B3,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp6F8 != 0){if(_tmp6F7)({
void*_tmp7B4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp7B5="both c_vararg and cyc_vararg";_tag_arr(_tmp7B5,sizeof(char),
_get_zero_arr_size(_tmp7B5,29));}),_tag_arr(_tmp7B4,sizeof(void*),0));});{void*
_tmp7B7;int _tmp7B8;struct Cyc_Absyn_VarargInfo _tmp7B6=*_tmp6F8;_tmp7B7=(void*)
_tmp7B6.type;_tmp7B8=_tmp7B6.inject;_tmp7B3=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7B3,(void*)1,_tmp7B7);if(_tmp7B8){void*_tmp7B9=Cyc_Tcutil_compress(_tmp7B7);
struct Cyc_Absyn_TunionInfo _tmp7BA;void*_tmp7BB;_LL4FD: if(_tmp7B9 <= (void*)3?1:*((
int*)_tmp7B9)!= 2)goto _LL4FF;_tmp7BA=((struct Cyc_Absyn_TunionType_struct*)
_tmp7B9)->f1;_tmp7BB=(void*)_tmp7BA.tunion_info;if(*((int*)_tmp7BB)!= 1)goto
_LL4FF;_LL4FE: goto _LL4FC;_LL4FF:;_LL500:({void*_tmp7BC[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7BD="can't inject a non-[x]tunion type";_tag_arr(_tmp7BD,sizeof(
char),_get_zero_arr_size(_tmp7BD,34));}),_tag_arr(_tmp7BC,sizeof(void*),0));});
goto _LL4FC;_LL4FC:;}}}if(seen_format){int _tmp7BE=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp6F6);if((((fmt_desc_arg < 0?1: fmt_desc_arg > _tmp7BE)?1:
fmt_arg_start < 0)?1:(_tmp6F8 == 0?fmt_arg_start != 0: 0))?1:(_tmp6F8 != 0?
fmt_arg_start != _tmp7BE + 1: 0))({void*_tmp7BF[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp7C0="bad format descriptor";_tag_arr(_tmp7C0,sizeof(char),
_get_zero_arr_size(_tmp7C0,22));}),_tag_arr(_tmp7BF,sizeof(void*),0));});else{
void*_tmp7C2;struct _tuple2 _tmp7C1=*((struct _tuple2*(*)(struct Cyc_List_List*x,int
n))Cyc_List_nth)(_tmp6F6,fmt_desc_arg - 1);_tmp7C2=_tmp7C1.f3;{void*_tmp7C3=Cyc_Tcutil_compress(
_tmp7C2);struct Cyc_Absyn_PtrInfo _tmp7C4;void*_tmp7C5;struct Cyc_Absyn_PtrAtts
_tmp7C6;struct Cyc_Absyn_Conref*_tmp7C7;struct Cyc_Absyn_Conref*_tmp7C8;_LL502: if(
_tmp7C3 <= (void*)3?1:*((int*)_tmp7C3)!= 4)goto _LL504;_tmp7C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp7C3)->f1;_tmp7C5=(void*)_tmp7C4.elt_typ;_tmp7C6=_tmp7C4.ptr_atts;_tmp7C7=
_tmp7C6.bounds;_tmp7C8=_tmp7C6.zero_term;_LL503:{struct _tuple6 _tmp7CA=({struct
_tuple6 _tmp7C9;_tmp7C9.f1=Cyc_Tcutil_compress(_tmp7C5);_tmp7C9.f2=Cyc_Absyn_conref_def((
void*)0,_tmp7C7);_tmp7C9;});void*_tmp7CB;void*_tmp7CC;void*_tmp7CD;void*_tmp7CE;
_LL507: _tmp7CB=_tmp7CA.f1;if(_tmp7CB <= (void*)3?1:*((int*)_tmp7CB)!= 5)goto
_LL509;_tmp7CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7CB)->f1;if((int)
_tmp7CC != 2)goto _LL509;_tmp7CD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7CB)->f2;
if((int)_tmp7CD != 0)goto _LL509;_tmp7CE=_tmp7CA.f2;if((int)_tmp7CE != 0)goto _LL509;
_LL508: goto _LL506;_LL509:;_LL50A:({void*_tmp7CF[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7D0="format descriptor is not a char ? type";_tag_arr(_tmp7D0,
sizeof(char),_get_zero_arr_size(_tmp7D0,39));}),_tag_arr(_tmp7CF,sizeof(void*),0));});
goto _LL506;_LL506:;}goto _LL501;_LL504:;_LL505:({void*_tmp7D1[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7D2="format descriptor is not a char ? type";_tag_arr(
_tmp7D2,sizeof(char),_get_zero_arr_size(_tmp7D2,39));}),_tag_arr(_tmp7D1,sizeof(
void*),0));});goto _LL501;_LL501:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp7D4=({struct _tuple6 _tmp7D3;_tmp7D3.f1=ft;_tmp7D3.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp6F8))->type);_tmp7D3;});void*
_tmp7D5;void*_tmp7D6;struct Cyc_Absyn_TunionInfo _tmp7D7;void*_tmp7D8;struct Cyc_Absyn_Tuniondecl**
_tmp7D9;struct Cyc_Absyn_Tuniondecl*_tmp7DA;void*_tmp7DB;void*_tmp7DC;struct Cyc_Absyn_TunionInfo
_tmp7DD;void*_tmp7DE;struct Cyc_Absyn_Tuniondecl**_tmp7DF;struct Cyc_Absyn_Tuniondecl*
_tmp7E0;_LL50C: _tmp7D5=_tmp7D4.f1;if((int)_tmp7D5 != 0)goto _LL50E;_tmp7D6=_tmp7D4.f2;
if(_tmp7D6 <= (void*)3?1:*((int*)_tmp7D6)!= 2)goto _LL50E;_tmp7D7=((struct Cyc_Absyn_TunionType_struct*)
_tmp7D6)->f1;_tmp7D8=(void*)_tmp7D7.tunion_info;if(*((int*)_tmp7D8)!= 1)goto
_LL50E;_tmp7D9=((struct Cyc_Absyn_KnownTunion_struct*)_tmp7D8)->f1;_tmp7DA=*
_tmp7D9;_LL50D: problem=Cyc_Absyn_qvar_cmp(_tmp7DA->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL50B;_LL50E: _tmp7DB=_tmp7D4.f1;if((int)_tmp7DB != 1)goto _LL510;_tmp7DC=
_tmp7D4.f2;if(_tmp7DC <= (void*)3?1:*((int*)_tmp7DC)!= 2)goto _LL510;_tmp7DD=((
struct Cyc_Absyn_TunionType_struct*)_tmp7DC)->f1;_tmp7DE=(void*)_tmp7DD.tunion_info;
if(*((int*)_tmp7DE)!= 1)goto _LL510;_tmp7DF=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp7DE)->f1;_tmp7E0=*_tmp7DF;_LL50F: problem=Cyc_Absyn_qvar_cmp(_tmp7E0->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL50B;_LL510:;_LL511: problem=1;goto
_LL50B;_LL50B:;}if(problem)({void*_tmp7E1[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp7E2="format attribute and vararg types don't match";_tag_arr(_tmp7E2,sizeof(
char),_get_zero_arr_size(_tmp7E2,46));}),_tag_arr(_tmp7E1,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp6F9;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp7E4;
void*_tmp7E5;void*_tmp7E6;struct _tuple6*_tmp7E3=(struct _tuple6*)rpo->hd;_tmp7E4=*
_tmp7E3;_tmp7E5=_tmp7E4.f1;_tmp7E6=_tmp7E4.f2;_tmp7B3=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7B3,(void*)4,_tmp7E5);_tmp7B3=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7B3,(void*)3,_tmp7E6);}}if(*_tmp6F4 != 0)_tmp7B3=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7B3,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp6F4))->v);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp7B3.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp7E7=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp7E8;struct Cyc_Core_Opt**_tmp7E9;void*
_tmp7EA;void*_tmp7EB;void*_tmp7EC;void*_tmp7ED;struct Cyc_Core_Opt*_tmp7EE;struct
Cyc_Core_Opt**_tmp7EF;void*_tmp7F0;void*_tmp7F1;struct Cyc_Core_Opt*_tmp7F2;
struct Cyc_Core_Opt**_tmp7F3;_LL513: if(*((int*)_tmp7E7)!= 2)goto _LL515;_tmp7E8=((
struct Cyc_Absyn_Less_kb_struct*)_tmp7E7)->f1;_tmp7E9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp7E7)->f1;_tmp7EA=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7E7)->f2;if((int)_tmp7EA != 3)goto _LL515;_LL514:*_tmp7E9=({struct Cyc_Core_Opt*
_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7F6;_tmp7F6.tag=0;_tmp7F6.f1=(void*)((void*)3);_tmp7F6;});_tmp7F5;}));
_tmp7F4;});goto _LL516;_LL515: if(*((int*)_tmp7E7)!= 0)goto _LL517;_tmp7EB=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp7E7)->f1;if((int)_tmp7EB != 3)goto _LL517;_LL516:
effect=({struct Cyc_List_List*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp7F8=_cycalloc(sizeof(*
_tmp7F8));_tmp7F8[0]=({struct Cyc_Absyn_AccessEff_struct _tmp7F9;_tmp7F9.tag=19;
_tmp7F9.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7FA=_cycalloc(
sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_Absyn_VarType_struct _tmp7FB;_tmp7FB.tag=
1;_tmp7FB.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp7FB;});_tmp7FA;}));_tmp7F9;});
_tmp7F8;}));_tmp7F7->tl=effect;_tmp7F7;});goto _LL512;_LL517: if(*((int*)_tmp7E7)
!= 2)goto _LL519;_tmp7EC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7E7)->f2;
if((int)_tmp7EC != 5)goto _LL519;_LL518: goto _LL51A;_LL519: if(*((int*)_tmp7E7)!= 0)
goto _LL51B;_tmp7ED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7E7)->f1;if((int)
_tmp7ED != 5)goto _LL51B;_LL51A: goto _LL512;_LL51B: if(*((int*)_tmp7E7)!= 2)goto
_LL51D;_tmp7EE=((struct Cyc_Absyn_Less_kb_struct*)_tmp7E7)->f1;_tmp7EF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp7E7)->f1;_tmp7F0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7E7)->f2;if((int)_tmp7F0 != 4)goto _LL51D;_LL51C:*_tmp7EF=({struct Cyc_Core_Opt*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp7FE;_tmp7FE.tag=0;_tmp7FE.f1=(void*)((void*)4);_tmp7FE;});_tmp7FD;}));
_tmp7FC;});goto _LL51E;_LL51D: if(*((int*)_tmp7E7)!= 0)goto _LL51F;_tmp7F1=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp7E7)->f1;if((int)_tmp7F1 != 4)goto _LL51F;_LL51E:
effect=({struct Cyc_List_List*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp800=_cycalloc(sizeof(*_tmp800));
_tmp800[0]=({struct Cyc_Absyn_VarType_struct _tmp801;_tmp801.tag=1;_tmp801.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp801;});_tmp800;}));_tmp7FF->tl=effect;_tmp7FF;});
goto _LL512;_LL51F: if(*((int*)_tmp7E7)!= 1)goto _LL521;_tmp7F2=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7E7)->f1;_tmp7F3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7E7)->f1;_LL520:*_tmp7F3=({struct Cyc_Core_Opt*_tmp802=_cycalloc(sizeof(*
_tmp802));_tmp802->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp803=
_cycalloc(sizeof(*_tmp803));_tmp803[0]=({struct Cyc_Absyn_Less_kb_struct _tmp804;
_tmp804.tag=2;_tmp804.f1=0;_tmp804.f2=(void*)((void*)0);_tmp804;});_tmp803;}));
_tmp802;});goto _LL522;_LL521:;_LL522: effect=({struct Cyc_List_List*_tmp805=
_cycalloc(sizeof(*_tmp805));_tmp805->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp807;_tmp807.tag=21;_tmp807.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({struct Cyc_Absyn_VarType_struct
_tmp809;_tmp809.tag=1;_tmp809.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp809;});
_tmp808;}));_tmp807;});_tmp806;}));_tmp805->tl=effect;_tmp805;});goto _LL512;
_LL512:;}}{struct Cyc_List_List*ts=_tmp7B3.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp80A=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL524: if((int)_tmp80A != 3)goto
_LL526;_LL525: effect=({struct Cyc_List_List*_tmp80B=_cycalloc(sizeof(*_tmp80B));
_tmp80B->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp80C=_cycalloc(
sizeof(*_tmp80C));_tmp80C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp80D;_tmp80D.tag=
19;_tmp80D.f1=(void*)((void*)ts->hd);_tmp80D;});_tmp80C;}));_tmp80B->tl=effect;
_tmp80B;});goto _LL523;_LL526: if((int)_tmp80A != 4)goto _LL528;_LL527: effect=({
struct Cyc_List_List*_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E->hd=(void*)((
void*)ts->hd);_tmp80E->tl=effect;_tmp80E;});goto _LL523;_LL528: if((int)_tmp80A != 
5)goto _LL52A;_LL529: goto _LL523;_LL52A:;_LL52B: effect=({struct Cyc_List_List*
_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp811;_tmp811.tag=21;_tmp811.f1=(void*)((void*)ts->hd);_tmp811;});_tmp810;}));
_tmp80F->tl=effect;_tmp80F;});goto _LL523;_LL523:;}}*_tmp6F4=({struct Cyc_Core_Opt*
_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp814;_tmp814.tag=20;_tmp814.f1=effect;_tmp814;});_tmp813;}));_tmp812;});}if(*
_tmp6F2 != 0){struct Cyc_List_List*bs=*_tmp6F2;for(0;bs != 0;bs=bs->tl){void*
_tmp815=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp816;struct Cyc_Core_Opt**_tmp817;struct Cyc_Core_Opt*
_tmp818;struct Cyc_Core_Opt**_tmp819;void*_tmp81A;struct Cyc_Core_Opt*_tmp81B;
struct Cyc_Core_Opt**_tmp81C;void*_tmp81D;struct Cyc_Core_Opt*_tmp81E;struct Cyc_Core_Opt**
_tmp81F;void*_tmp820;void*_tmp821;_LL52D: if(*((int*)_tmp815)!= 1)goto _LL52F;
_tmp816=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp815)->f1;_tmp817=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp815)->f1;_LL52E:({struct Cyc_String_pa_struct
_tmp824;_tmp824.tag=0;_tmp824.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp822[1]={& _tmp824};Cyc_Tcutil_warn(
loc,({const char*_tmp823="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp823,sizeof(char),_get_zero_arr_size(_tmp823,47));}),_tag_arr(
_tmp822,sizeof(void*),1));}});_tmp819=_tmp817;goto _LL530;_LL52F: if(*((int*)
_tmp815)!= 2)goto _LL531;_tmp818=((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;
_tmp819=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;
_tmp81A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f2;if((int)_tmp81A != 
1)goto _LL531;_LL530: _tmp81C=_tmp819;goto _LL532;_LL531: if(*((int*)_tmp815)!= 2)
goto _LL533;_tmp81B=((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;_tmp81C=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;_tmp81D=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f2;if((int)_tmp81D != 0)goto
_LL533;_LL532:*_tmp81C=({struct Cyc_Core_Opt*_tmp825=_cycalloc(sizeof(*_tmp825));
_tmp825->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp826=_cycalloc(
sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp827;_tmp827.tag=0;
_tmp827.f1=(void*)((void*)2);_tmp827;});_tmp826;}));_tmp825;});goto _LL52C;_LL533:
if(*((int*)_tmp815)!= 2)goto _LL535;_tmp81E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp815)->f1;_tmp81F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp815)->f1;_tmp820=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f2;
_LL534:*_tmp81F=({struct Cyc_Core_Opt*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp829=_cycalloc(sizeof(*_tmp829));
_tmp829[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp82A;_tmp82A.tag=0;_tmp82A.f1=(void*)
_tmp820;_tmp82A;});_tmp829;}));_tmp828;});goto _LL52C;_LL535: if(*((int*)_tmp815)
!= 0)goto _LL537;_tmp821=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp815)->f1;if((
int)_tmp821 != 1)goto _LL537;_LL536:({void*_tmp82B[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp82C="functions can't abstract M types";_tag_arr(_tmp82C,sizeof(
char),_get_zero_arr_size(_tmp82C,33));}),_tag_arr(_tmp82B,sizeof(void*),0));});
goto _LL52C;_LL537:;_LL538: goto _LL52C;_LL52C:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp7B3.kind_env,*_tmp6F2);_tmp7B3.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp7B3.free_vars,*_tmp6F2);{struct Cyc_List_List*tvs=_tmp7B3.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp7B3.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL473;}}_LL492: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 
9)goto _LL494;_tmp6FB=((struct Cyc_Absyn_TupleType_struct*)_tmp6CD)->f1;_LL493:
for(0;_tmp6FB != 0;_tmp6FB=_tmp6FB->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp6FB->hd)).f2);}goto _LL473;_LL494: if(
_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 11)goto _LL496;_tmp6FC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6CD)->f1;_tmp6FD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6CD)->f2;_LL495:{
struct _RegionHandle _tmp82E=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp82E;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp6FD != 0;_tmp6FD=_tmp6FD->tl){struct Cyc_Absyn_Aggrfield _tmp830;struct
_tagged_arr*_tmp831;struct Cyc_Absyn_Tqual _tmp832;void*_tmp833;struct Cyc_Absyn_Exp*
_tmp834;struct Cyc_List_List*_tmp835;struct Cyc_Absyn_Aggrfield*_tmp82F=(struct Cyc_Absyn_Aggrfield*)
_tmp6FD->hd;_tmp830=*_tmp82F;_tmp831=_tmp830.name;_tmp832=_tmp830.tq;_tmp833=(
void*)_tmp830.type;_tmp834=_tmp830.width;_tmp835=_tmp830.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp831))({struct
Cyc_String_pa_struct _tmp838;_tmp838.tag=0;_tmp838.f1=(struct _tagged_arr)((struct
_tagged_arr)*_tmp831);{void*_tmp836[1]={& _tmp838};Cyc_Tcutil_terr(loc,({const
char*_tmp837="duplicate field %s";_tag_arr(_tmp837,sizeof(char),
_get_zero_arr_size(_tmp837,19));}),_tag_arr(_tmp836,sizeof(void*),1));}});if(Cyc_strcmp((
struct _tagged_arr)*_tmp831,({const char*_tmp839="";_tag_arr(_tmp839,sizeof(char),
_get_zero_arr_size(_tmp839,1));}))!= 0)prev_fields=({struct Cyc_List_List*_tmp83A=
_region_malloc(aprev_rgn,sizeof(*_tmp83A));_tmp83A->hd=_tmp831;_tmp83A->tl=
prev_fields;_tmp83A;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp833);if(_tmp6FC == (void*)1?!Cyc_Tcutil_bits_only(_tmp833): 0)({struct Cyc_String_pa_struct
_tmp83D;_tmp83D.tag=0;_tmp83D.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp831);{
void*_tmp83B[1]={& _tmp83D};Cyc_Tcutil_terr(loc,({const char*_tmp83C="union member %s has a non-integral type";
_tag_arr(_tmp83C,sizeof(char),_get_zero_arr_size(_tmp83C,40));}),_tag_arr(
_tmp83B,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp833,_tmp834,
_tmp831);Cyc_Tcutil_check_field_atts(loc,_tmp831,_tmp835);}};_pop_region(
aprev_rgn);}goto _LL473;_LL496: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 10)goto
_LL498;_tmp6FE=((struct Cyc_Absyn_AggrType_struct*)_tmp6CD)->f1;_tmp6FF=(void*)
_tmp6FE.aggr_info;_tmp700=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp6CD)->f1).aggr_info;
_tmp701=_tmp6FE.targs;_tmp702=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp6CD)->f1).targs;_LL497:{void*_tmp83E=*_tmp700;void*_tmp83F;struct _tuple1*
_tmp840;struct Cyc_Absyn_Aggrdecl**_tmp841;struct Cyc_Absyn_Aggrdecl*_tmp842;
_LL53A: if(*((int*)_tmp83E)!= 0)goto _LL53C;_tmp83F=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp83E)->f1;_tmp840=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp83E)->f2;_LL53B: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp843;_push_handler(& _tmp843);{
int _tmp845=0;if(setjmp(_tmp843.handler))_tmp845=1;if(!_tmp845){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp840);*_tmp700=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp846=
_cycalloc(sizeof(*_tmp846));_tmp846[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp847;
_tmp847.tag=1;_tmp847.f1=adp;_tmp847;});_tmp846;});;_pop_handler();}else{void*
_tmp844=(void*)_exn_thrown;void*_tmp849=_tmp844;_LL53F: if(_tmp849 != Cyc_Dict_Absent)
goto _LL541;_LL540: {struct Cyc_Tcenv_Genv*_tmp84A=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp84B=({struct Cyc_Absyn_Aggrdecl*_tmp851=_cycalloc(
sizeof(*_tmp851));_tmp851->kind=(void*)_tmp83F;_tmp851->sc=(void*)((void*)3);
_tmp851->name=_tmp840;_tmp851->tvs=0;_tmp851->impl=0;_tmp851->attributes=0;
_tmp851;});Cyc_Tc_tcAggrdecl(te,_tmp84A,loc,_tmp84B);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp840);*_tmp700=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp84C=
_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp84D;
_tmp84D.tag=1;_tmp84D.f1=adp;_tmp84D;});_tmp84C;});if(*_tmp702 != 0){({struct Cyc_String_pa_struct
_tmp850;_tmp850.tag=0;_tmp850.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp840));{void*_tmp84E[1]={& _tmp850};Cyc_Tcutil_terr(loc,({const char*_tmp84F="declare parameterized type %s before using";
_tag_arr(_tmp84F,sizeof(char),_get_zero_arr_size(_tmp84F,43));}),_tag_arr(
_tmp84E,sizeof(void*),1));}});return cvtenv;}goto _LL53E;}_LL541:;_LL542:(void)
_throw(_tmp849);_LL53E:;}}}_tmp842=*adp;goto _LL53D;}_LL53C: if(*((int*)_tmp83E)!= 
1)goto _LL539;_tmp841=((struct Cyc_Absyn_KnownAggr_struct*)_tmp83E)->f1;_tmp842=*
_tmp841;_LL53D: {struct Cyc_List_List*tvs=_tmp842->tvs;struct Cyc_List_List*ts=*
_tmp702;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_String_pa_struct _tmp854;_tmp854.tag=
0;_tmp854.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp842->name));{void*_tmp852[1]={& _tmp854};Cyc_Tcutil_terr(loc,({const char*
_tmp853="too many parameters for type %s";_tag_arr(_tmp853,sizeof(char),
_get_zero_arr_size(_tmp853,32));}),_tag_arr(_tmp852,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855->hd=(void*)e;_tmp855->tl=
hidden_ts;_tmp855;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp702=Cyc_List_imp_append(*_tmp702,Cyc_List_imp_rev(hidden_ts));}}_LL539:;}
goto _LL473;_LL498: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 16)goto _LL49A;
_tmp703=((struct Cyc_Absyn_TypedefType_struct*)_tmp6CD)->f1;_tmp704=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6CD)->f2;_tmp705=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp6CD)->f2;_tmp706=((struct Cyc_Absyn_TypedefType_struct*)_tmp6CD)->f3;_tmp707=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp6CD)->f3;
_tmp708=((struct Cyc_Absyn_TypedefType_struct*)_tmp6CD)->f4;_tmp709=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp6CD)->f4;_LL499: {struct Cyc_List_List*
targs=*_tmp705;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp856;
_push_handler(& _tmp856);{int _tmp858=0;if(setjmp(_tmp856.handler))_tmp858=1;if(!
_tmp858){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp703);;_pop_handler();}else{
void*_tmp857=(void*)_exn_thrown;void*_tmp85A=_tmp857;_LL544: if(_tmp85A != Cyc_Dict_Absent)
goto _LL546;_LL545:({struct Cyc_String_pa_struct _tmp85D;_tmp85D.tag=0;_tmp85D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp703));{void*
_tmp85B[1]={& _tmp85D};Cyc_Tcutil_terr(loc,({const char*_tmp85C="unbound typedef name %s";
_tag_arr(_tmp85C,sizeof(char),_get_zero_arr_size(_tmp85C,24));}),_tag_arr(
_tmp85B,sizeof(void*),1));}});return cvtenv;_LL546:;_LL547:(void)_throw(_tmp85A);
_LL543:;}}}*_tmp707=(struct Cyc_Absyn_Typedefdecl*)td;_tmp703[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp85E=_cycalloc(
sizeof(*_tmp85E));_tmp85E->hd=({struct _tuple8*_tmp85F=_cycalloc(sizeof(*_tmp85F));
_tmp85F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp85F->f2=(void*)ts->hd;_tmp85F;});
_tmp85E->tl=inst;_tmp85E;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp862;
_tmp862.tag=0;_tmp862.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp703));{void*_tmp860[1]={& _tmp862};Cyc_Tcutil_terr(loc,({const char*_tmp861="too many parameters for typedef %s";
_tag_arr(_tmp861,sizeof(char),_get_zero_arr_size(_tmp861,35));}),_tag_arr(
_tmp860,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp863=_cycalloc(
sizeof(*_tmp863));_tmp863->hd=(void*)e;_tmp863->tl=hidden_ts;_tmp863;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864->hd=({struct _tuple8*_tmp865=
_cycalloc(sizeof(*_tmp865));_tmp865->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp865->f2=
e;_tmp865;});_tmp864->tl=inst;_tmp864;});}*_tmp705=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp709=({void**_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866[0]=new_typ;_tmp866;});}goto _LL473;}}_LL49A:
if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 18)goto _LL49C;_LL49B: goto _LL49D;
_LL49C: if((int)_tmp6CD != 2)goto _LL49E;_LL49D: goto _LL473;_LL49E: if(_tmp6CD <= (
void*)3?1:*((int*)_tmp6CD)!= 15)goto _LL4A0;_tmp70A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6CD)->f1;_LL49F: _tmp70B=_tmp70A;goto _LL4A1;_LL4A0: if(_tmp6CD <= (void*)3?1:*((
int*)_tmp6CD)!= 19)goto _LL4A2;_tmp70B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6CD)->f1;_LL4A1: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp70B);goto _LL473;_LL4A2: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 21)goto
_LL4A4;_tmp70C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6CD)->f1;_LL4A3:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp70C);goto _LL473;
_LL4A4: if(_tmp6CD <= (void*)3?1:*((int*)_tmp6CD)!= 20)goto _LL473;_tmp70D=((struct
Cyc_Absyn_JoinEff_struct*)_tmp6CD)->f1;_LL4A5: for(0;_tmp70D != 0;_tmp70D=_tmp70D->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp70D->hd);}
goto _LL473;_LL473:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_String_pa_struct _tmp86B;_tmp86B.tag=0;_tmp86B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct
_tmp86A;_tmp86A.tag=0;_tmp86A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp869;_tmp869.tag=0;
_tmp869.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
void*_tmp867[3]={& _tmp869,& _tmp86A,& _tmp86B};Cyc_Tcutil_terr(loc,({const char*
_tmp868="type %s has kind %s but as used here needs kind %s";_tag_arr(_tmp868,
sizeof(char),_get_zero_arr_size(_tmp868,51));}),_tag_arr(_tmp867,sizeof(void*),3));}}}});
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp86C=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp875;_tmp875.kind_env=kind_env;_tmp875.free_vars=
0;_tmp875.free_evars=0;_tmp875.generalize_evars=generalize_evars;_tmp875.fn_result=
0;_tmp875;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp86C.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp86C.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp86C.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp86D=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp86E;struct Cyc_Core_Opt**_tmp86F;_LL549: if(_tmp86D <= (void*)3?1:*((int*)
_tmp86D)!= 0)goto _LL54B;_tmp86E=((struct Cyc_Absyn_Evar_struct*)_tmp86D)->f4;
_tmp86F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp86D)->f4;
_LL54A: if(*_tmp86F == 0)*_tmp86F=({struct Cyc_Core_Opt*_tmp870=_cycalloc(sizeof(*
_tmp870));_tmp870->v=kind_env;_tmp870;});else{struct Cyc_List_List*_tmp871=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp86F))->v;struct Cyc_List_List*
_tmp872=0;for(0;_tmp871 != 0;_tmp871=_tmp871->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp871->hd))_tmp872=({struct Cyc_List_List*_tmp873=_cycalloc(sizeof(*_tmp873));
_tmp873->hd=(struct Cyc_Absyn_Tvar*)_tmp871->hd;_tmp873->tl=_tmp872;_tmp873;});}*
_tmp86F=({struct Cyc_Core_Opt*_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874->v=
_tmp872;_tmp874;});}goto _LL548;_LL54B:;_LL54C: goto _LL548;_LL548:;}}return _tmp86C;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp876=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp877=_tmp876.free_vars;struct Cyc_List_List*
_tmp878=_tmp876.free_evars;{struct Cyc_List_List*x=_tmp877;for(0;x != 0;x=x->tl){
void*_tmp879=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp87A;struct Cyc_Core_Opt**_tmp87B;struct Cyc_Core_Opt*
_tmp87C;struct Cyc_Core_Opt**_tmp87D;void*_tmp87E;struct Cyc_Core_Opt*_tmp87F;
struct Cyc_Core_Opt**_tmp880;void*_tmp881;struct Cyc_Core_Opt*_tmp882;struct Cyc_Core_Opt**
_tmp883;void*_tmp884;void*_tmp885;_LL54E: if(*((int*)_tmp879)!= 1)goto _LL550;
_tmp87A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp879)->f1;_tmp87B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp879)->f1;_LL54F: _tmp87D=_tmp87B;goto
_LL551;_LL550: if(*((int*)_tmp879)!= 2)goto _LL552;_tmp87C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp879)->f1;_tmp87D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp879)->f1;_tmp87E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp879)->f2;if((
int)_tmp87E != 1)goto _LL552;_LL551: _tmp880=_tmp87D;goto _LL553;_LL552: if(*((int*)
_tmp879)!= 2)goto _LL554;_tmp87F=((struct Cyc_Absyn_Less_kb_struct*)_tmp879)->f1;
_tmp880=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp879)->f1;
_tmp881=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp879)->f2;if((int)_tmp881 != 
0)goto _LL554;_LL553:*_tmp880=({struct Cyc_Core_Opt*_tmp886=_cycalloc(sizeof(*
_tmp886));_tmp886->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp887=
_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp888;
_tmp888.tag=0;_tmp888.f1=(void*)((void*)2);_tmp888;});_tmp887;}));_tmp886;});
goto _LL54D;_LL554: if(*((int*)_tmp879)!= 2)goto _LL556;_tmp882=((struct Cyc_Absyn_Less_kb_struct*)
_tmp879)->f1;_tmp883=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp879)->f1;_tmp884=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp879)->f2;
_LL555:*_tmp883=({struct Cyc_Core_Opt*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp88A=_cycalloc(sizeof(*_tmp88A));
_tmp88A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp88B;_tmp88B.tag=0;_tmp88B.f1=(void*)
_tmp884;_tmp88B;});_tmp88A;}));_tmp889;});goto _LL54D;_LL556: if(*((int*)_tmp879)
!= 0)goto _LL558;_tmp885=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp879)->f1;if((
int)_tmp885 != 1)goto _LL558;_LL557:({struct Cyc_String_pa_struct _tmp88E;_tmp88E.tag=
0;_tmp88E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp88C[1]={& _tmp88E};Cyc_Tcutil_terr(loc,({
const char*_tmp88D="type variable %s cannot have kind M";_tag_arr(_tmp88D,sizeof(
char),_get_zero_arr_size(_tmp88D,36));}),_tag_arr(_tmp88C,sizeof(void*),1));}});
goto _LL54D;_LL558:;_LL559: goto _LL54D;_LL54D:;}}if(_tmp877 != 0?1: _tmp878 != 0){{
void*_tmp88F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp890;struct Cyc_List_List*
_tmp891;struct Cyc_List_List**_tmp892;struct Cyc_Core_Opt*_tmp893;void*_tmp894;
struct Cyc_List_List*_tmp895;int _tmp896;struct Cyc_Absyn_VarargInfo*_tmp897;struct
Cyc_List_List*_tmp898;struct Cyc_List_List*_tmp899;_LL55B: if(_tmp88F <= (void*)3?1:*((
int*)_tmp88F)!= 8)goto _LL55D;_tmp890=((struct Cyc_Absyn_FnType_struct*)_tmp88F)->f1;
_tmp891=_tmp890.tvars;_tmp892=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp88F)->f1).tvars;_tmp893=_tmp890.effect;_tmp894=(void*)_tmp890.ret_typ;
_tmp895=_tmp890.args;_tmp896=_tmp890.c_varargs;_tmp897=_tmp890.cyc_varargs;
_tmp898=_tmp890.rgn_po;_tmp899=_tmp890.attributes;_LL55C: if(*_tmp892 == 0){*
_tmp892=_tmp877;_tmp877=0;}goto _LL55A;_LL55D:;_LL55E: goto _LL55A;_LL55A:;}if(
_tmp877 != 0)({struct Cyc_String_pa_struct _tmp89C;_tmp89C.tag=0;_tmp89C.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp877->hd)->name);{
void*_tmp89A[1]={& _tmp89C};Cyc_Tcutil_terr(loc,({const char*_tmp89B="unbound type variable %s";
_tag_arr(_tmp89B,sizeof(char),_get_zero_arr_size(_tmp89B,25));}),_tag_arr(
_tmp89A,sizeof(void*),1));}});if(_tmp878 != 0)for(0;_tmp878 != 0;_tmp878=_tmp878->tl){
void*e=(void*)_tmp878->hd;void*_tmp89D=Cyc_Tcutil_typ_kind(e);_LL560: if((int)
_tmp89D != 3)goto _LL562;_LL561: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp89E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp89F="can't unify evar with heap!";_tag_arr(_tmp89F,sizeof(char),
_get_zero_arr_size(_tmp89F,28));}),_tag_arr(_tmp89E,sizeof(void*),0));});goto
_LL55F;_LL562: if((int)_tmp89D != 4)goto _LL564;_LL563: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp8A0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8A1="can't unify evar with {}!";_tag_arr(_tmp8A1,sizeof(char),
_get_zero_arr_size(_tmp8A1,26));}),_tag_arr(_tmp8A0,sizeof(void*),0));});goto
_LL55F;_LL564:;_LL565:({struct Cyc_String_pa_struct _tmp8A5;_tmp8A5.tag=0;_tmp8A5.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp8A4;_tmp8A4.tag=0;_tmp8A4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp8A2[2]={& _tmp8A4,& _tmp8A5};Cyc_Tcutil_terr(loc,({const char*_tmp8A3="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8A3,sizeof(char),_get_zero_arr_size(_tmp8A3,52));}),_tag_arr(
_tmp8A2,sizeof(void*),2));}}});goto _LL55F;_LL55F:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp8A6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8A7;struct Cyc_List_List*
_tmp8A8;struct Cyc_Core_Opt*_tmp8A9;_LL567: if(_tmp8A6 <= (void*)3?1:*((int*)
_tmp8A6)!= 8)goto _LL569;_tmp8A7=((struct Cyc_Absyn_FnType_struct*)_tmp8A6)->f1;
_tmp8A8=_tmp8A7.tvars;_tmp8A9=_tmp8A7.effect;_LL568: fd->tvs=_tmp8A8;fd->effect=
_tmp8A9;goto _LL566;_LL569:;_LL56A:({void*_tmp8AA[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8AB="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp8AB,sizeof(char),_get_zero_arr_size(_tmp8AB,38));}),_tag_arr(
_tmp8AA,sizeof(void*),0));});_LL566:;}{struct _RegionHandle _tmp8AC=_new_region("r");
struct _RegionHandle*r=& _tmp8AC;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp8AD="function declaration has repeated parameter";
_tag_arr(_tmp8AD,sizeof(char),_get_zero_arr_size(_tmp8AD,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp8AE=_cycalloc(sizeof(*_tmp8AE));_tmp8AE->v=(
void*)t;_tmp8AE;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp8AF=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp8B0=Cyc_Tcutil_remove_bound_tvars(
_tmp8AF.free_vars,bound_tvars);struct Cyc_List_List*_tmp8B1=_tmp8AF.free_evars;{
struct Cyc_List_List*fs=_tmp8B0;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp8B2=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp8B6;_tmp8B6.tag=
0;_tmp8B6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp8B5;_tmp8B5.tag=0;_tmp8B5.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8B2);{void*_tmp8B3[2]={& _tmp8B5,& _tmp8B6};Cyc_Tcutil_terr(
loc,({const char*_tmp8B4="unbound type variable %s in type %s";_tag_arr(_tmp8B4,
sizeof(char),_get_zero_arr_size(_tmp8B4,36));}),_tag_arr(_tmp8B3,sizeof(void*),2));}}});}}
if(!allow_evars?_tmp8B1 != 0: 0)for(0;_tmp8B1 != 0;_tmp8B1=_tmp8B1->tl){void*e=(
void*)_tmp8B1->hd;void*_tmp8B7=Cyc_Tcutil_typ_kind(e);_LL56C: if((int)_tmp8B7 != 3)
goto _LL56E;_LL56D: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8B8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8B9="can't unify evar with heap!";
_tag_arr(_tmp8B9,sizeof(char),_get_zero_arr_size(_tmp8B9,28));}),_tag_arr(
_tmp8B8,sizeof(void*),0));});goto _LL56B;_LL56E: if((int)_tmp8B7 != 4)goto _LL570;
_LL56F: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp8BA[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8BB="can't unify evar with {}!";
_tag_arr(_tmp8BB,sizeof(char),_get_zero_arr_size(_tmp8BB,26));}),_tag_arr(
_tmp8BA,sizeof(void*),0));});goto _LL56B;_LL570:;_LL571:({struct Cyc_String_pa_struct
_tmp8BF;_tmp8BF.tag=0;_tmp8BF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp8BE;_tmp8BE.tag=0;_tmp8BE.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp8BC[2]={&
_tmp8BE,& _tmp8BF};Cyc_Tcutil_terr(loc,({const char*_tmp8BD="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8BD,sizeof(char),_get_zero_arr_size(_tmp8BD,52));}),_tag_arr(
_tmp8BC,sizeof(void*),2));}}});goto _LL56B;_LL56B:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp8C3;_tmp8C3.tag=0;_tmp8C3.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp8C2;_tmp8C2.tag=0;_tmp8C2.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp8C0[2]={& _tmp8C2,& _tmp8C3};
Cyc_Tcutil_terr(loc,({const char*_tmp8C1="%s: %s";_tag_arr(_tmp8C1,sizeof(char),
_get_zero_arr_size(_tmp8C1,7));}),_tag_arr(_tmp8C0,sizeof(void*),2));}}});}}}
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
const char*_tmp8C4="duplicate type variable";_tag_arr(_tmp8C4,sizeof(char),
_get_zero_arr_size(_tmp8C4,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp8C5="";_tag_arr(_tmp8C5,sizeof(char),_get_zero_arr_size(_tmp8C5,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp8C6=_cycalloc(sizeof(*_tmp8C6));_tmp8C6->hd=({
struct _tuple18*_tmp8C7=_cycalloc(sizeof(*_tmp8C7));_tmp8C7->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp8C7->f2=0;_tmp8C7;});_tmp8C6->tl=fields;_tmp8C6;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp8C9;struct Cyc_List_List*
_tmp8CA;void*_tmp8CB;struct _tuple19*_tmp8C8=(struct _tuple19*)des->hd;_tmp8C9=*
_tmp8C8;_tmp8CA=_tmp8C9.f1;_tmp8CB=_tmp8C9.f2;if(_tmp8CA == 0){struct Cyc_List_List*
_tmp8CC=fields;for(0;_tmp8CC != 0;_tmp8CC=_tmp8CC->tl){if(!(*((struct _tuple18*)
_tmp8CC->hd)).f2){(*((struct _tuple18*)_tmp8CC->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp8CD=_cycalloc(sizeof(*_tmp8CD));
_tmp8CD->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp8CE=_cycalloc(
sizeof(*_tmp8CE));_tmp8CE[0]=({struct Cyc_Absyn_FieldName_struct _tmp8CF;_tmp8CF.tag=
1;_tmp8CF.f1=((*((struct _tuple18*)_tmp8CC->hd)).f1)->name;_tmp8CF;});_tmp8CE;}));
_tmp8CD->tl=0;_tmp8CD;});ans=({struct Cyc_List_List*_tmp8D0=_region_malloc(rgn,
sizeof(*_tmp8D0));_tmp8D0->hd=({struct _tuple20*_tmp8D1=_region_malloc(rgn,
sizeof(*_tmp8D1));_tmp8D1->f1=(*((struct _tuple18*)_tmp8CC->hd)).f1;_tmp8D1->f2=
_tmp8CB;_tmp8D1;});_tmp8D0->tl=ans;_tmp8D0;});break;}}if(_tmp8CC == 0)({void*
_tmp8D2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp8D3="too many arguments to struct";
_tag_arr(_tmp8D3,sizeof(char),_get_zero_arr_size(_tmp8D3,29));}),_tag_arr(
_tmp8D2,sizeof(void*),0));});}else{if(_tmp8CA->tl != 0)({void*_tmp8D4[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8D5="multiple designators are not supported";_tag_arr(
_tmp8D5,sizeof(char),_get_zero_arr_size(_tmp8D5,39));}),_tag_arr(_tmp8D4,sizeof(
void*),0));});else{void*_tmp8D6=(void*)_tmp8CA->hd;struct _tagged_arr*_tmp8D7;
_LL573: if(*((int*)_tmp8D6)!= 0)goto _LL575;_LL574:({void*_tmp8D8[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8D9="array designator used in argument to struct";_tag_arr(
_tmp8D9,sizeof(char),_get_zero_arr_size(_tmp8D9,44));}),_tag_arr(_tmp8D8,sizeof(
void*),0));});goto _LL572;_LL575: if(*((int*)_tmp8D6)!= 1)goto _LL572;_tmp8D7=((
struct Cyc_Absyn_FieldName_struct*)_tmp8D6)->f1;_LL576: {struct Cyc_List_List*
_tmp8DA=fields;for(0;_tmp8DA != 0;_tmp8DA=_tmp8DA->tl){if(Cyc_strptrcmp(_tmp8D7,((*((
struct _tuple18*)_tmp8DA->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp8DA->hd)).f2)({
struct Cyc_String_pa_struct _tmp8DD;_tmp8DD.tag=0;_tmp8DD.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8D7);{void*_tmp8DB[1]={& _tmp8DD};Cyc_Tcutil_terr(loc,({
const char*_tmp8DC="field %s has already been used as an argument";_tag_arr(
_tmp8DC,sizeof(char),_get_zero_arr_size(_tmp8DC,46));}),_tag_arr(_tmp8DB,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp8DA->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp8DE=_region_malloc(rgn,sizeof(*_tmp8DE));_tmp8DE->hd=({struct _tuple20*
_tmp8DF=_region_malloc(rgn,sizeof(*_tmp8DF));_tmp8DF->f1=(*((struct _tuple18*)
_tmp8DA->hd)).f1;_tmp8DF->f2=_tmp8CB;_tmp8DF;});_tmp8DE->tl=ans;_tmp8DE;});
break;}}if(_tmp8DA == 0)({struct Cyc_String_pa_struct _tmp8E2;_tmp8E2.tag=0;_tmp8E2.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp8D7);{void*_tmp8E0[1]={& _tmp8E2};Cyc_Tcutil_terr(
loc,({const char*_tmp8E1="bad field designator %s";_tag_arr(_tmp8E1,sizeof(char),
_get_zero_arr_size(_tmp8E1,24));}),_tag_arr(_tmp8E0,sizeof(void*),1));}});goto
_LL572;}_LL572:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp8E3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp8E4="too few arguments to struct";
_tag_arr(_tmp8E4,sizeof(char),_get_zero_arr_size(_tmp8E4,28));}),_tag_arr(
_tmp8E3,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp8E5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp8E6;
void*_tmp8E7;struct Cyc_Absyn_PtrAtts _tmp8E8;struct Cyc_Absyn_Conref*_tmp8E9;
_LL578: if(_tmp8E5 <= (void*)3?1:*((int*)_tmp8E5)!= 4)goto _LL57A;_tmp8E6=((struct
Cyc_Absyn_PointerType_struct*)_tmp8E5)->f1;_tmp8E7=(void*)_tmp8E6.elt_typ;
_tmp8E8=_tmp8E6.ptr_atts;_tmp8E9=_tmp8E8.bounds;_LL579: {struct Cyc_Absyn_Conref*
_tmp8EA=Cyc_Absyn_compress_conref(_tmp8E9);void*_tmp8EB=(void*)(Cyc_Absyn_compress_conref(
_tmp8EA))->v;void*_tmp8EC;_LL57D: if(_tmp8EB <= (void*)1?1:*((int*)_tmp8EB)!= 0)
goto _LL57F;_tmp8EC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp8EB)->f1;if((
int)_tmp8EC != 0)goto _LL57F;_LL57E:*elt_typ_dest=_tmp8E7;return 1;_LL57F: if((int)
_tmp8EB != 0)goto _LL581;_LL580:(void*)(_tmp8EA->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp8ED=_cycalloc(sizeof(*_tmp8ED));_tmp8ED[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp8EE;_tmp8EE.tag=0;_tmp8EE.f1=(void*)((void*)0);_tmp8EE;});_tmp8ED;})));*
elt_typ_dest=_tmp8E7;return 1;_LL581:;_LL582: return 0;_LL57C:;}_LL57A:;_LL57B:
return 0;_LL577:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8EF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp8F0;void*_tmp8F1;
struct Cyc_Absyn_PtrAtts _tmp8F2;struct Cyc_Absyn_Conref*_tmp8F3;_LL584: if(_tmp8EF
<= (void*)3?1:*((int*)_tmp8EF)!= 4)goto _LL586;_tmp8F0=((struct Cyc_Absyn_PointerType_struct*)
_tmp8EF)->f1;_tmp8F1=(void*)_tmp8F0.elt_typ;_tmp8F2=_tmp8F0.ptr_atts;_tmp8F3=
_tmp8F2.zero_term;_LL585:*elt_typ_dest=_tmp8F1;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp8F3);_LL586:;_LL587: return 0;_LL583:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp937;_tmp937.f1=0;
_tmp937.f2=(void*)2;_tmp937;});void*_tmp8F4=(void*)e->r;struct _tuple1*_tmp8F5;
void*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F7;struct _tagged_arr*_tmp8F8;struct Cyc_Absyn_Exp*
_tmp8F9;struct _tagged_arr*_tmp8FA;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_Absyn_Exp*
_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FD;_LL589: if(*((int*)_tmp8F4)!= 1)goto _LL58B;
_tmp8F5=((struct Cyc_Absyn_Var_e_struct*)_tmp8F4)->f1;_tmp8F6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp8F4)->f2;_LL58A: {void*_tmp8FE=_tmp8F6;struct Cyc_Absyn_Vardecl*_tmp8FF;
struct Cyc_Absyn_Vardecl*_tmp900;struct Cyc_Absyn_Vardecl*_tmp901;struct Cyc_Absyn_Vardecl*
_tmp902;_LL596: if((int)_tmp8FE != 0)goto _LL598;_LL597: goto _LL599;_LL598: if(
_tmp8FE <= (void*)1?1:*((int*)_tmp8FE)!= 1)goto _LL59A;_LL599: return bogus_ans;
_LL59A: if(_tmp8FE <= (void*)1?1:*((int*)_tmp8FE)!= 0)goto _LL59C;_tmp8FF=((struct
Cyc_Absyn_Global_b_struct*)_tmp8FE)->f1;_LL59B: {void*_tmp903=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5A3: if(_tmp903 <= (void*)
3?1:*((int*)_tmp903)!= 7)goto _LL5A5;_LL5A4: return({struct _tuple10 _tmp904;_tmp904.f1=
1;_tmp904.f2=(void*)2;_tmp904;});_LL5A5:;_LL5A6: return({struct _tuple10 _tmp905;
_tmp905.f1=(_tmp8FF->tq).q_const;_tmp905.f2=(void*)2;_tmp905;});_LL5A2:;}_LL59C:
if(_tmp8FE <= (void*)1?1:*((int*)_tmp8FE)!= 3)goto _LL59E;_tmp900=((struct Cyc_Absyn_Local_b_struct*)
_tmp8FE)->f1;_LL59D: {void*_tmp906=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL5A8: if(_tmp906 <= (void*)3?1:*((int*)_tmp906)!= 7)
goto _LL5AA;_LL5A9: return({struct _tuple10 _tmp907;_tmp907.f1=1;_tmp907.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp900->rgn))->v;_tmp907;});_LL5AA:;_LL5AB:
_tmp900->escapes=1;return({struct _tuple10 _tmp908;_tmp908.f1=(_tmp900->tq).q_const;
_tmp908.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp900->rgn))->v;_tmp908;});
_LL5A7:;}_LL59E: if(_tmp8FE <= (void*)1?1:*((int*)_tmp8FE)!= 4)goto _LL5A0;_tmp901=((
struct Cyc_Absyn_Pat_b_struct*)_tmp8FE)->f1;_LL59F: _tmp902=_tmp901;goto _LL5A1;
_LL5A0: if(_tmp8FE <= (void*)1?1:*((int*)_tmp8FE)!= 2)goto _LL595;_tmp902=((struct
Cyc_Absyn_Param_b_struct*)_tmp8FE)->f1;_LL5A1: _tmp902->escapes=1;return({struct
_tuple10 _tmp909;_tmp909.f1=(_tmp902->tq).q_const;_tmp909.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp902->rgn))->v;_tmp909;});_LL595:;}_LL58B: if(*((int*)_tmp8F4)!= 
21)goto _LL58D;_tmp8F7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8F4)->f1;
_tmp8F8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp8F4)->f2;_LL58C: {void*
_tmp90A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp8F7->topt))->v);
struct Cyc_List_List*_tmp90B;struct Cyc_Absyn_AggrInfo _tmp90C;void*_tmp90D;struct
Cyc_Absyn_Aggrdecl**_tmp90E;struct Cyc_Absyn_Aggrdecl*_tmp90F;_LL5AD: if(_tmp90A <= (
void*)3?1:*((int*)_tmp90A)!= 11)goto _LL5AF;_tmp90B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp90A)->f2;_LL5AE: {struct Cyc_Absyn_Aggrfield*_tmp910=Cyc_Absyn_lookup_field(
_tmp90B,_tmp8F8);if(_tmp910 != 0?_tmp910->width != 0: 0)return({struct _tuple10
_tmp911;_tmp911.f1=(_tmp910->tq).q_const;_tmp911.f2=(Cyc_Tcutil_addressof_props(
te,_tmp8F7)).f2;_tmp911;});return bogus_ans;}_LL5AF: if(_tmp90A <= (void*)3?1:*((
int*)_tmp90A)!= 10)goto _LL5B1;_tmp90C=((struct Cyc_Absyn_AggrType_struct*)_tmp90A)->f1;
_tmp90D=(void*)_tmp90C.aggr_info;if(*((int*)_tmp90D)!= 1)goto _LL5B1;_tmp90E=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp90D)->f1;_tmp90F=*_tmp90E;_LL5B0: {struct
Cyc_Absyn_Aggrfield*_tmp912=Cyc_Absyn_lookup_decl_field(_tmp90F,_tmp8F8);if(
_tmp912 != 0?_tmp912->width != 0: 0)return({struct _tuple10 _tmp913;_tmp913.f1=(
_tmp912->tq).q_const;_tmp913.f2=(Cyc_Tcutil_addressof_props(te,_tmp8F7)).f2;
_tmp913;});return bogus_ans;}_LL5B1:;_LL5B2: return bogus_ans;_LL5AC:;}_LL58D: if(*((
int*)_tmp8F4)!= 22)goto _LL58F;_tmp8F9=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp8F4)->f1;_tmp8FA=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp8F4)->f2;_LL58E: {
void*_tmp914=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp8F9->topt))->v);struct Cyc_Absyn_PtrInfo _tmp915;void*_tmp916;struct Cyc_Absyn_PtrAtts
_tmp917;void*_tmp918;_LL5B4: if(_tmp914 <= (void*)3?1:*((int*)_tmp914)!= 4)goto
_LL5B6;_tmp915=((struct Cyc_Absyn_PointerType_struct*)_tmp914)->f1;_tmp916=(void*)
_tmp915.elt_typ;_tmp917=_tmp915.ptr_atts;_tmp918=(void*)_tmp917.rgn;_LL5B5: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp919=Cyc_Tcutil_compress(_tmp916);
struct Cyc_List_List*_tmp91A;struct Cyc_Absyn_AggrInfo _tmp91B;void*_tmp91C;struct
Cyc_Absyn_Aggrdecl**_tmp91D;struct Cyc_Absyn_Aggrdecl*_tmp91E;_LL5B9: if(_tmp919 <= (
void*)3?1:*((int*)_tmp919)!= 11)goto _LL5BB;_tmp91A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp919)->f2;_LL5BA: finfo=Cyc_Absyn_lookup_field(_tmp91A,_tmp8FA);goto _LL5B8;
_LL5BB: if(_tmp919 <= (void*)3?1:*((int*)_tmp919)!= 10)goto _LL5BD;_tmp91B=((struct
Cyc_Absyn_AggrType_struct*)_tmp919)->f1;_tmp91C=(void*)_tmp91B.aggr_info;if(*((
int*)_tmp91C)!= 1)goto _LL5BD;_tmp91D=((struct Cyc_Absyn_KnownAggr_struct*)_tmp91C)->f1;
_tmp91E=*_tmp91D;_LL5BC: finfo=Cyc_Absyn_lookup_decl_field(_tmp91E,_tmp8FA);goto
_LL5B8;_LL5BD:;_LL5BE: return bogus_ans;_LL5B8:;}if(finfo != 0?finfo->width != 0: 0)
return({struct _tuple10 _tmp91F;_tmp91F.f1=(finfo->tq).q_const;_tmp91F.f2=_tmp918;
_tmp91F;});return bogus_ans;}_LL5B6:;_LL5B7: return bogus_ans;_LL5B3:;}_LL58F: if(*((
int*)_tmp8F4)!= 20)goto _LL591;_tmp8FB=((struct Cyc_Absyn_Deref_e_struct*)_tmp8F4)->f1;
_LL590: {void*_tmp920=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8FB->topt))->v);struct Cyc_Absyn_PtrInfo _tmp921;struct Cyc_Absyn_Tqual
_tmp922;struct Cyc_Absyn_PtrAtts _tmp923;void*_tmp924;_LL5C0: if(_tmp920 <= (void*)3?
1:*((int*)_tmp920)!= 4)goto _LL5C2;_tmp921=((struct Cyc_Absyn_PointerType_struct*)
_tmp920)->f1;_tmp922=_tmp921.elt_tq;_tmp923=_tmp921.ptr_atts;_tmp924=(void*)
_tmp923.rgn;_LL5C1: return({struct _tuple10 _tmp925;_tmp925.f1=_tmp922.q_const;
_tmp925.f2=_tmp924;_tmp925;});_LL5C2:;_LL5C3: return bogus_ans;_LL5BF:;}_LL591: if(*((
int*)_tmp8F4)!= 23)goto _LL593;_tmp8FC=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp8F4)->f1;_tmp8FD=((struct Cyc_Absyn_Subscript_e_struct*)_tmp8F4)->f2;_LL592: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp8FC->topt))->v);
void*_tmp926=t;struct Cyc_List_List*_tmp927;struct Cyc_Absyn_PtrInfo _tmp928;struct
Cyc_Absyn_Tqual _tmp929;struct Cyc_Absyn_PtrAtts _tmp92A;void*_tmp92B;struct Cyc_Absyn_ArrayInfo
_tmp92C;struct Cyc_Absyn_Tqual _tmp92D;_LL5C5: if(_tmp926 <= (void*)3?1:*((int*)
_tmp926)!= 9)goto _LL5C7;_tmp927=((struct Cyc_Absyn_TupleType_struct*)_tmp926)->f1;
_LL5C6: {unsigned int _tmp92F;int _tmp930;struct _tuple7 _tmp92E=Cyc_Evexp_eval_const_uint_exp(
_tmp8FD);_tmp92F=_tmp92E.f1;_tmp930=_tmp92E.f2;if(!_tmp930)return bogus_ans;{
struct _tuple4*_tmp931=Cyc_Absyn_lookup_tuple_field(_tmp927,(int)_tmp92F);if(
_tmp931 != 0)return({struct _tuple10 _tmp932;_tmp932.f1=((*_tmp931).f1).q_const;
_tmp932.f2=(Cyc_Tcutil_addressof_props(te,_tmp8FC)).f2;_tmp932;});return
bogus_ans;}}_LL5C7: if(_tmp926 <= (void*)3?1:*((int*)_tmp926)!= 4)goto _LL5C9;
_tmp928=((struct Cyc_Absyn_PointerType_struct*)_tmp926)->f1;_tmp929=_tmp928.elt_tq;
_tmp92A=_tmp928.ptr_atts;_tmp92B=(void*)_tmp92A.rgn;_LL5C8: return({struct
_tuple10 _tmp933;_tmp933.f1=_tmp929.q_const;_tmp933.f2=_tmp92B;_tmp933;});_LL5C9:
if(_tmp926 <= (void*)3?1:*((int*)_tmp926)!= 7)goto _LL5CB;_tmp92C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp926)->f1;_tmp92D=_tmp92C.tq;_LL5CA: return({struct _tuple10 _tmp934;_tmp934.f1=
_tmp92D.q_const;_tmp934.f2=(Cyc_Tcutil_addressof_props(te,_tmp8FC)).f2;_tmp934;});
_LL5CB:;_LL5CC: return bogus_ans;_LL5C4:;}_LL593:;_LL594:({void*_tmp935[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp936="unary & applied to non-lvalue";_tag_arr(_tmp936,
sizeof(char),_get_zero_arr_size(_tmp936,30));}),_tag_arr(_tmp935,sizeof(void*),0));});
return bogus_ans;_LL588:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,
void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp938=Cyc_Tcutil_compress(e_typ);struct
Cyc_Absyn_ArrayInfo _tmp939;void*_tmp93A;struct Cyc_Absyn_Tqual _tmp93B;struct Cyc_Absyn_Conref*
_tmp93C;_LL5CE: if(_tmp938 <= (void*)3?1:*((int*)_tmp938)!= 7)goto _LL5D0;_tmp939=((
struct Cyc_Absyn_ArrayType_struct*)_tmp938)->f1;_tmp93A=(void*)_tmp939.elt_type;
_tmp93B=_tmp939.tq;_tmp93C=_tmp939.zero_term;_LL5CF: {void*_tmp93E;struct
_tuple10 _tmp93D=Cyc_Tcutil_addressof_props(te,e);_tmp93E=_tmp93D.f2;return Cyc_Absyn_atb_typ(
_tmp93A,_tmp93E,_tmp93B,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp93F=
_cycalloc(sizeof(*_tmp93F));_tmp93F[0]=({struct Cyc_Absyn_Upper_b_struct _tmp940;
_tmp940.tag=0;_tmp940.f1=e;_tmp940;});_tmp93F;}),_tmp93C);}_LL5D0:;_LL5D1: return
e_typ;_LL5CD:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp941=(void*)b->v;void*_tmp942;void*_tmp943;struct Cyc_Absyn_Exp*_tmp944;void*
_tmp945;_LL5D3: if(_tmp941 <= (void*)1?1:*((int*)_tmp941)!= 0)goto _LL5D5;_tmp942=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp941)->f1;if((int)_tmp942 != 0)goto
_LL5D5;_LL5D4: return;_LL5D5: if(_tmp941 <= (void*)1?1:*((int*)_tmp941)!= 0)goto
_LL5D7;_tmp943=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp941)->f1;if(
_tmp943 <= (void*)1?1:*((int*)_tmp943)!= 0)goto _LL5D7;_tmp944=((struct Cyc_Absyn_Upper_b_struct*)
_tmp943)->f1;_LL5D6: {unsigned int _tmp947;int _tmp948;struct _tuple7 _tmp946=Cyc_Evexp_eval_const_uint_exp(
_tmp944);_tmp947=_tmp946.f1;_tmp948=_tmp946.f2;if(_tmp948?_tmp947 <= i: 0)({struct
Cyc_Int_pa_struct _tmp94C;_tmp94C.tag=1;_tmp94C.f1=(unsigned int)((int)i);{struct
Cyc_Int_pa_struct _tmp94B;_tmp94B.tag=1;_tmp94B.f1=(unsigned int)((int)_tmp947);{
void*_tmp949[2]={& _tmp94B,& _tmp94C};Cyc_Tcutil_terr(loc,({const char*_tmp94A="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp94A,sizeof(char),_get_zero_arr_size(_tmp94A,39));}),_tag_arr(
_tmp949,sizeof(void*),2));}}});return;}_LL5D7: if(_tmp941 <= (void*)1?1:*((int*)
_tmp941)!= 0)goto _LL5D9;_tmp945=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp941)->f1;if(_tmp945 <= (void*)1?1:*((int*)_tmp945)!= 1)goto _LL5D9;_LL5D8:
return;_LL5D9: if((int)_tmp941 != 0)goto _LL5DB;_LL5DA:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp94D=_cycalloc(sizeof(*_tmp94D));_tmp94D[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp94E;_tmp94E.tag=0;_tmp94E.f1=(void*)((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp94F=_cycalloc(sizeof(*_tmp94F));_tmp94F[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp950;_tmp950.tag=0;_tmp950.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp950;});_tmp94F;}));_tmp94E;});_tmp94D;})));return;_LL5DB:;_LL5DC:({
void*_tmp951[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp952="check_bound -- bad compressed conref";_tag_arr(_tmp952,
sizeof(char),_get_zero_arr_size(_tmp952,37));}),_tag_arr(_tmp951,sizeof(void*),0));});
_LL5D2:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp953=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp954;struct Cyc_Absyn_Exp*_tmp955;_LL5DE: if(_tmp953 <= (void*)1?1:*((int*)
_tmp953)!= 0)goto _LL5E0;_tmp954=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp953)->f1;if(_tmp954 <= (void*)1?1:*((int*)_tmp954)!= 0)goto _LL5E0;_tmp955=((
struct Cyc_Absyn_Upper_b_struct*)_tmp954)->f1;_LL5DF: {unsigned int _tmp957;int
_tmp958;struct _tuple7 _tmp956=Cyc_Evexp_eval_const_uint_exp(_tmp955);_tmp957=
_tmp956.f1;_tmp958=_tmp956.f2;return _tmp958?_tmp957 == 1: 0;}_LL5E0:;_LL5E1: return
0;_LL5DD:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp959=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp95A;void*_tmp95B;struct Cyc_Absyn_Conref*_tmp95C;
struct Cyc_List_List*_tmp95D;struct Cyc_Absyn_AggrInfo _tmp95E;void*_tmp95F;void*
_tmp960;struct Cyc_Absyn_AggrInfo _tmp961;void*_tmp962;void*_tmp963;struct Cyc_Absyn_AggrInfo
_tmp964;void*_tmp965;struct Cyc_Absyn_Aggrdecl**_tmp966;struct Cyc_Absyn_Aggrdecl*
_tmp967;struct Cyc_List_List*_tmp968;struct Cyc_List_List*_tmp969;_LL5E3: if((int)
_tmp959 != 0)goto _LL5E5;_LL5E4: goto _LL5E6;_LL5E5: if(_tmp959 <= (void*)3?1:*((int*)
_tmp959)!= 5)goto _LL5E7;_LL5E6: goto _LL5E8;_LL5E7: if((int)_tmp959 != 1)goto _LL5E9;
_LL5E8: goto _LL5EA;_LL5E9: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 6)goto _LL5EB;
_LL5EA: return 1;_LL5EB: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 12)goto _LL5ED;
_LL5EC: goto _LL5EE;_LL5ED: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 13)goto
_LL5EF;_LL5EE: return 0;_LL5EF: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 7)goto
_LL5F1;_tmp95A=((struct Cyc_Absyn_ArrayType_struct*)_tmp959)->f1;_tmp95B=(void*)
_tmp95A.elt_type;_tmp95C=_tmp95A.zero_term;_LL5F0: return !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp95C)?Cyc_Tcutil_bits_only(_tmp95B): 0;_LL5F1: if(
_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 9)goto _LL5F3;_tmp95D=((struct Cyc_Absyn_TupleType_struct*)
_tmp959)->f1;_LL5F2: for(0;_tmp95D != 0;_tmp95D=_tmp95D->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp95D->hd)).f2))return 0;}return 1;_LL5F3: if(_tmp959 <= (void*)3?1:*((
int*)_tmp959)!= 10)goto _LL5F5;_tmp95E=((struct Cyc_Absyn_AggrType_struct*)_tmp959)->f1;
_tmp95F=(void*)_tmp95E.aggr_info;if(*((int*)_tmp95F)!= 0)goto _LL5F5;_tmp960=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp95F)->f1;if((int)_tmp960 != 1)
goto _LL5F5;_LL5F4: return 1;_LL5F5: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 10)
goto _LL5F7;_tmp961=((struct Cyc_Absyn_AggrType_struct*)_tmp959)->f1;_tmp962=(void*)
_tmp961.aggr_info;if(*((int*)_tmp962)!= 0)goto _LL5F7;_tmp963=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp962)->f1;if((int)_tmp963 != 0)goto _LL5F7;_LL5F6: return 0;_LL5F7: if(_tmp959 <= (
void*)3?1:*((int*)_tmp959)!= 10)goto _LL5F9;_tmp964=((struct Cyc_Absyn_AggrType_struct*)
_tmp959)->f1;_tmp965=(void*)_tmp964.aggr_info;if(*((int*)_tmp965)!= 1)goto _LL5F9;
_tmp966=((struct Cyc_Absyn_KnownAggr_struct*)_tmp965)->f1;_tmp967=*_tmp966;
_tmp968=_tmp964.targs;_LL5F8: if((void*)_tmp967->kind == (void*)1)return 1;if(
_tmp967->impl == 0)return 0;{struct _RegionHandle _tmp96A=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp96A;_push_region(rgn);{struct Cyc_List_List*_tmp96B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp967->tvs,_tmp968);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp967->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp96B,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp96C=0;_npop_handler(0);
return _tmp96C;}}}{int _tmp96D=1;_npop_handler(0);return _tmp96D;}};_pop_region(rgn);}
_LL5F9: if(_tmp959 <= (void*)3?1:*((int*)_tmp959)!= 11)goto _LL5FB;_tmp969=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp959)->f2;_LL5FA: for(0;_tmp969 != 0;_tmp969=
_tmp969->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp969->hd)->type))return 0;}return 1;_LL5FB:;_LL5FC: return 0;_LL5E2:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp96E=(void*)e->r;
struct _tuple1*_tmp96F;void*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*
_tmp972;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*
_tmp975;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*_tmp977;void*_tmp978;
struct Cyc_Absyn_Exp*_tmp979;void*_tmp97A;void*_tmp97B;struct Cyc_Absyn_Exp*
_tmp97C;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*
_tmp97F;struct Cyc_List_List*_tmp980;struct Cyc_List_List*_tmp981;struct Cyc_List_List*
_tmp982;void*_tmp983;struct Cyc_List_List*_tmp984;struct Cyc_List_List*_tmp985;
struct Cyc_List_List*_tmp986;_LL5FE: if(*((int*)_tmp96E)!= 0)goto _LL600;_LL5FF:
goto _LL601;_LL600: if(*((int*)_tmp96E)!= 16)goto _LL602;_LL601: goto _LL603;_LL602:
if(*((int*)_tmp96E)!= 17)goto _LL604;_LL603: goto _LL605;_LL604: if(*((int*)_tmp96E)
!= 18)goto _LL606;_LL605: goto _LL607;_LL606: if(*((int*)_tmp96E)!= 19)goto _LL608;
_LL607: goto _LL609;_LL608: if(*((int*)_tmp96E)!= 31)goto _LL60A;_LL609: goto _LL60B;
_LL60A: if(*((int*)_tmp96E)!= 32)goto _LL60C;_LL60B: return 1;_LL60C: if(*((int*)
_tmp96E)!= 1)goto _LL60E;_tmp96F=((struct Cyc_Absyn_Var_e_struct*)_tmp96E)->f1;
_tmp970=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp96E)->f2;_LL60D: {void*
_tmp987=_tmp970;struct Cyc_Absyn_Vardecl*_tmp988;_LL62D: if(_tmp987 <= (void*)1?1:*((
int*)_tmp987)!= 1)goto _LL62F;_LL62E: return 1;_LL62F: if(_tmp987 <= (void*)1?1:*((
int*)_tmp987)!= 0)goto _LL631;_tmp988=((struct Cyc_Absyn_Global_b_struct*)_tmp987)->f1;
_LL630: {void*_tmp989=Cyc_Tcutil_compress((void*)_tmp988->type);_LL636: if(
_tmp989 <= (void*)3?1:*((int*)_tmp989)!= 7)goto _LL638;_LL637: goto _LL639;_LL638:
if(_tmp989 <= (void*)3?1:*((int*)_tmp989)!= 8)goto _LL63A;_LL639: return 1;_LL63A:;
_LL63B: return var_okay;_LL635:;}_LL631: if((int)_tmp987 != 0)goto _LL633;_LL632:
return 0;_LL633:;_LL634: return var_okay;_LL62C:;}_LL60E: if(*((int*)_tmp96E)!= 6)
goto _LL610;_tmp971=((struct Cyc_Absyn_Conditional_e_struct*)_tmp96E)->f1;_tmp972=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp96E)->f2;_tmp973=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp96E)->f3;_LL60F: return(Cyc_Tcutil_cnst_exp(te,0,_tmp971)?Cyc_Tcutil_cnst_exp(
te,0,_tmp972): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp973): 0;_LL610: if(*((int*)_tmp96E)
!= 7)goto _LL612;_tmp974=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp96E)->f1;_tmp975=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp96E)->f2;_LL611: return Cyc_Tcutil_cnst_exp(
te,0,_tmp974)?Cyc_Tcutil_cnst_exp(te,0,_tmp975): 0;_LL612: if(*((int*)_tmp96E)!= 
11)goto _LL614;_tmp976=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp96E)->f1;
_LL613: _tmp977=_tmp976;goto _LL615;_LL614: if(*((int*)_tmp96E)!= 12)goto _LL616;
_tmp977=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp96E)->f1;_LL615: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp977);_LL616: if(*((int*)_tmp96E)!= 13)goto _LL618;_tmp978=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp96E)->f1;_tmp979=((struct Cyc_Absyn_Cast_e_struct*)
_tmp96E)->f2;_tmp97A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp96E)->f4;if((
int)_tmp97A != 1)goto _LL618;_LL617: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp979);
_LL618: if(*((int*)_tmp96E)!= 13)goto _LL61A;_tmp97B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp96E)->f1;_tmp97C=((struct Cyc_Absyn_Cast_e_struct*)_tmp96E)->f2;_LL619: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmp97C);_LL61A: if(*((int*)_tmp96E)!= 14)goto
_LL61C;_tmp97D=((struct Cyc_Absyn_Address_e_struct*)_tmp96E)->f1;_LL61B: return Cyc_Tcutil_cnst_exp(
te,1,_tmp97D);_LL61C: if(*((int*)_tmp96E)!= 27)goto _LL61E;_tmp97E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp96E)->f2;_tmp97F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp96E)->f3;
_LL61D: return Cyc_Tcutil_cnst_exp(te,0,_tmp97E)?Cyc_Tcutil_cnst_exp(te,0,_tmp97F):
0;_LL61E: if(*((int*)_tmp96E)!= 26)goto _LL620;_tmp980=((struct Cyc_Absyn_Array_e_struct*)
_tmp96E)->f1;_LL61F: _tmp981=_tmp980;goto _LL621;_LL620: if(*((int*)_tmp96E)!= 29)
goto _LL622;_tmp981=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp96E)->f2;_LL621:
_tmp982=_tmp981;goto _LL623;_LL622: if(*((int*)_tmp96E)!= 28)goto _LL624;_tmp982=((
struct Cyc_Absyn_Struct_e_struct*)_tmp96E)->f3;_LL623: for(0;_tmp982 != 0;_tmp982=
_tmp982->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp982->hd)).f2))
return 0;}return 1;_LL624: if(*((int*)_tmp96E)!= 3)goto _LL626;_tmp983=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp96E)->f1;_tmp984=((struct Cyc_Absyn_Primop_e_struct*)
_tmp96E)->f2;_LL625: _tmp985=_tmp984;goto _LL627;_LL626: if(*((int*)_tmp96E)!= 24)
goto _LL628;_tmp985=((struct Cyc_Absyn_Tuple_e_struct*)_tmp96E)->f1;_LL627: _tmp986=
_tmp985;goto _LL629;_LL628: if(*((int*)_tmp96E)!= 30)goto _LL62A;_tmp986=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp96E)->f1;_LL629: for(0;_tmp986 != 0;_tmp986=_tmp986->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp986->hd))return 0;}return 1;
_LL62A:;_LL62B: return 0;_LL5FD:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp98A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp98B;
struct Cyc_Absyn_PtrAtts _tmp98C;struct Cyc_Absyn_Conref*_tmp98D;struct Cyc_Absyn_Conref*
_tmp98E;struct Cyc_Absyn_ArrayInfo _tmp98F;void*_tmp990;struct Cyc_List_List*
_tmp991;struct Cyc_Absyn_AggrInfo _tmp992;void*_tmp993;struct Cyc_List_List*_tmp994;
struct Cyc_List_List*_tmp995;_LL63D: if((int)_tmp98A != 0)goto _LL63F;_LL63E: goto
_LL640;_LL63F: if(_tmp98A <= (void*)3?1:*((int*)_tmp98A)!= 5)goto _LL641;_LL640:
goto _LL642;_LL641: if((int)_tmp98A != 1)goto _LL643;_LL642: goto _LL644;_LL643: if(
_tmp98A <= (void*)3?1:*((int*)_tmp98A)!= 6)goto _LL645;_LL644: return 1;_LL645: if(
_tmp98A <= (void*)3?1:*((int*)_tmp98A)!= 4)goto _LL647;_tmp98B=((struct Cyc_Absyn_PointerType_struct*)
_tmp98A)->f1;_tmp98C=_tmp98B.ptr_atts;_tmp98D=_tmp98C.nullable;_tmp98E=_tmp98C.bounds;
_LL646: {void*_tmp996=(void*)(Cyc_Absyn_compress_conref(_tmp98E))->v;void*
_tmp997;_LL656: if(_tmp996 <= (void*)1?1:*((int*)_tmp996)!= 0)goto _LL658;_tmp997=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp996)->f1;if((int)_tmp997 != 0)goto
_LL658;_LL657: return 1;_LL658: if(_tmp996 <= (void*)1?1:*((int*)_tmp996)!= 0)goto
_LL65A;_LL659: {void*_tmp998=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp98D))->v;int _tmp999;_LL65D: if(_tmp998 <= (void*)
1?1:*((int*)_tmp998)!= 0)goto _LL65F;_tmp999=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp998)->f1;_LL65E: return _tmp999;_LL65F:;_LL660: return 0;_LL65C:;}_LL65A:;_LL65B:
return 0;_LL655:;}_LL647: if(_tmp98A <= (void*)3?1:*((int*)_tmp98A)!= 7)goto _LL649;
_tmp98F=((struct Cyc_Absyn_ArrayType_struct*)_tmp98A)->f1;_tmp990=(void*)_tmp98F.elt_type;
_LL648: return Cyc_Tcutil_supports_default(_tmp990);_LL649: if(_tmp98A <= (void*)3?1:*((
int*)_tmp98A)!= 9)goto _LL64B;_tmp991=((struct Cyc_Absyn_TupleType_struct*)_tmp98A)->f1;
_LL64A: for(0;_tmp991 != 0;_tmp991=_tmp991->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple4*)_tmp991->hd)).f2))return 0;}return 1;_LL64B: if(_tmp98A <= (void*)3?1:*((
int*)_tmp98A)!= 10)goto _LL64D;_tmp992=((struct Cyc_Absyn_AggrType_struct*)_tmp98A)->f1;
_tmp993=(void*)_tmp992.aggr_info;_tmp994=_tmp992.targs;_LL64C: {struct Cyc_Absyn_Aggrdecl*
_tmp99A=Cyc_Absyn_get_known_aggrdecl(_tmp993);if(_tmp99A->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp99A->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmp99A->tvs,_tmp994,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp99A->impl))->fields);}_LL64D: if(_tmp98A <= (void*)3?1:*((int*)
_tmp98A)!= 11)goto _LL64F;_tmp995=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp98A)->f2;
_LL64E: return Cyc_Tcutil_fields_support_default(0,0,_tmp995);_LL64F: if(_tmp98A <= (
void*)3?1:*((int*)_tmp98A)!= 13)goto _LL651;_LL650: goto _LL652;_LL651: if(_tmp98A <= (
void*)3?1:*((int*)_tmp98A)!= 12)goto _LL653;_LL652: return 1;_LL653:;_LL654: return 0;
_LL63C:;}static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,
struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp99B=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp99B;_push_region(rgn);{struct Cyc_List_List*
_tmp99C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp99C,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp99D=0;_npop_handler(0);
return _tmp99D;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){
void*_tmp99E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp99F;struct Cyc_Absyn_PtrAtts
_tmp9A0;struct Cyc_Absyn_Conref*_tmp9A1;struct Cyc_Absyn_Conref*_tmp9A2;_LL662: if(
_tmp99E <= (void*)3?1:*((int*)_tmp99E)!= 5)goto _LL664;_LL663: goto _LL665;_LL664:
if((int)_tmp99E != 1)goto _LL666;_LL665: goto _LL667;_LL666: if(_tmp99E <= (void*)3?1:*((
int*)_tmp99E)!= 6)goto _LL668;_LL667: return 1;_LL668: if(_tmp99E <= (void*)3?1:*((
int*)_tmp99E)!= 4)goto _LL66A;_tmp99F=((struct Cyc_Absyn_PointerType_struct*)
_tmp99E)->f1;_tmp9A0=_tmp99F.ptr_atts;_tmp9A1=_tmp9A0.nullable;_tmp9A2=_tmp9A0.bounds;
_LL669: {void*_tmp9A3=(void*)(Cyc_Absyn_compress_conref(_tmp9A2))->v;void*
_tmp9A4;_LL66D: if(_tmp9A3 <= (void*)1?1:*((int*)_tmp9A3)!= 0)goto _LL66F;_tmp9A4=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9A3)->f1;if((int)_tmp9A4 != 0)goto
_LL66F;_LL66E: return 0;_LL66F: if(_tmp9A3 <= (void*)1?1:*((int*)_tmp9A3)!= 0)goto
_LL671;_LL670: {void*_tmp9A5=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9A1))->v;int _tmp9A6;_LL674: if(_tmp9A5 <= (void*)
1?1:*((int*)_tmp9A5)!= 0)goto _LL676;_tmp9A6=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9A5)->f1;_LL675: return _tmp9A6;_LL676:;_LL677: return 0;_LL673:;}_LL671:;_LL672:
return 0;_LL66C:;}_LL66A:;_LL66B: return 0;_LL661:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp9A7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9A8;void*
_tmp9A9;struct Cyc_Absyn_FnInfo _tmp9AA;struct Cyc_List_List*_tmp9AB;_LL679: if(
_tmp9A7 <= (void*)3?1:*((int*)_tmp9A7)!= 4)goto _LL67B;_tmp9A8=((struct Cyc_Absyn_PointerType_struct*)
_tmp9A7)->f1;_tmp9A9=(void*)_tmp9A8.elt_typ;_LL67A: return Cyc_Tcutil_is_noreturn(
_tmp9A9);_LL67B: if(_tmp9A7 <= (void*)3?1:*((int*)_tmp9A7)!= 8)goto _LL67D;_tmp9AA=((
struct Cyc_Absyn_FnType_struct*)_tmp9A7)->f1;_tmp9AB=_tmp9AA.attributes;_LL67C:
for(0;_tmp9AB != 0;_tmp9AB=_tmp9AB->tl){void*_tmp9AC=(void*)_tmp9AB->hd;_LL680:
if((int)_tmp9AC != 3)goto _LL682;_LL681: return 1;_LL682:;_LL683: continue;_LL67F:;}
goto _LL678;_LL67D:;_LL67E: goto _LL678;_LL678:;}return 0;}
