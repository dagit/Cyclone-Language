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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _tagged_arr);int Cyc_List_length(struct
Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*,void*(*f)(void*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_app(void*(*f)(void*),
struct Cyc_List_List*x);void Cyc_List_app_c(void*(*f)(void*,void*),void*,struct Cyc_List_List*
x);void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y);void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);void
Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,
struct Cyc_List_List*y);void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,
struct Cyc_List_List*x);void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),
void*,void*accum,struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,
void*),struct Cyc_List_List*x,void*accum);void*Cyc_List_fold_right_c(void*(*f)(
void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);struct Cyc_List_List*
Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rrevappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rflatten(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_merge_sort(
int(*cmp)(void*,void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmerge_sort(
struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rimp_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);struct Cyc_List_List*Cyc_List_imp_merge(
int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);extern char
Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*
Cyc_List_nth_tail(struct Cyc_List_List*x,int i);int Cyc_List_forall(int(*pred)(void*),
struct Cyc_List_List*x);int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x);int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(
struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct Cyc_List_List*
Cyc_List_rzip3(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x);struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);struct
_tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct
_RegionHandle*r5,struct Cyc_List_List*x);int Cyc_List_memq(struct Cyc_List_List*l,
void*x);int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);void*Cyc_List_assoc_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_mem_assoc(struct Cyc_List_List*
l,void*x);struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*
x);struct _tagged_arr Cyc_List_to_array(struct Cyc_List_List*x);struct _tagged_arr
Cyc_List_rto_array(struct _RegionHandle*r,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_from_array(struct _tagged_arr arr);struct Cyc_List_List*Cyc_List_rfrom_array(
struct _RegionHandle*r2,struct _tagged_arr arr);struct Cyc_List_List*Cyc_List_tabulate(
int n,void*(*f)(int));struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,
int),void*env);struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,
void*(*f)(int));struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,
int n,void*(*f)(void*,int),void*env);int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rfilter(
struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*
x);int Cyc_List_length(struct Cyc_List_List*x){int i=0;while(x != 0){++ i;x=x->tl;}
return i;}void*Cyc_List_hd(struct Cyc_List_List*x){if(x == 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct
Cyc_Core_Failure_struct _tmp1;_tmp1.tag=Cyc_Core_Failure;_tmp1.f1=({const char*
_tmp2="hd";_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,3));});_tmp1;});
_tmp0;}));return(void*)x->hd;}struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*
x){if(x == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Core_Failure_struct _tmp4;_tmp4.tag=Cyc_Core_Failure;
_tmp4.f1=({const char*_tmp5="tl";_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(
_tmp5,3));});_tmp4;});_tmp3;}));return x->tl;}struct Cyc_List_List*Cyc_List_rlist(
struct _RegionHandle*r,struct _tagged_arr argv){struct Cyc_List_List*result=0;{int i=(
int)(_get_arr_size(argv,sizeof(void*))- 1);for(0;i >= 0;i --){result=({struct Cyc_List_List*
_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->hd=(void*)*((void**)
_check_unknown_subscript(argv,sizeof(void*),i));_tmp6->tl=result;_tmp6;});}}
return result;}struct Cyc_List_List*Cyc_List_list(struct _tagged_arr argv){struct Cyc_List_List*
result=0;{int i=(int)(_get_arr_size(argv,sizeof(void*))- 1);for(0;i >= 0;i --){
result=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->hd=(void*)*((
void**)_check_unknown_subscript(argv,sizeof(void*),i));_tmp7->tl=result;_tmp7;});}}
return result;}struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*r2,struct
Cyc_List_List*x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)
return 0;result=({struct Cyc_List_List*_tmp8=_region_malloc(r2,sizeof(*_tmp8));
_tmp8->hd=(void*)((void*)x->hd);_tmp8->tl=0;_tmp8;});prev=result;for(x=x->tl;x != 
0;x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp9=_region_malloc(r2,sizeof(*_tmp9));_tmp9->hd=(void*)((void*)x->hd);_tmp9->tl=
0;_tmp9;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}return result;}
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x){return Cyc_List_rcopy(
Cyc_Core_heap_region,x);}struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*
r2,void*(*f)(void*),struct Cyc_List_List*x){struct Cyc_List_List*result;struct Cyc_List_List*
prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmpA=_region_malloc(r2,
sizeof(*_tmpA));_tmpA->hd=(void*)f((void*)x->hd);_tmpA->tl=0;_tmpA;});prev=
result;for(x=x->tl;x != 0;x=x->tl){struct Cyc_List_List*temp=({struct Cyc_List_List*
_tmpB=_region_malloc(r2,sizeof(*_tmpB));_tmpB->hd=(void*)f((void*)x->hd);_tmpB->tl=
0;_tmpB;});((struct Cyc_List_List*)_check_null(prev))->tl=temp;prev=temp;}return
result;}struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x){
return Cyc_List_rmap(Cyc_Core_heap_region,f,x);}struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmpC=_region_malloc(r2,sizeof(*_tmpC));_tmpC->hd=(void*)f(
env,(void*)x->hd);_tmpC->tl=0;_tmpC;});prev=result;for(x=x->tl;x != 0;x=x->tl){((
struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmpD=
_region_malloc(r2,sizeof(*_tmpD));_tmpD->hd=(void*)f(env,(void*)x->hd);_tmpD->tl=
0;_tmpD;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}return result;}
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){return Cyc_List_rmap_c(Cyc_Core_heap_region,f,env,x);}char Cyc_List_List_mismatch[
18]="\000\000\000\000List_mismatch\000";struct Cyc_List_List*Cyc_List_rmap2(
struct _RegionHandle*r3,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0?y == 0: 0)return 0;
if(x == 0?1: y == 0)(int)_throw((void*)Cyc_List_List_mismatch);result=({struct Cyc_List_List*
_tmpE=_region_malloc(r3,sizeof(*_tmpE));_tmpE->hd=(void*)f((void*)x->hd,(void*)y->hd);
_tmpE->tl=0;_tmpE;});prev=result;x=x->tl;y=y->tl;while(x != 0?y != 0: 0){((struct
Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmpF=_region_malloc(
r3,sizeof(*_tmpF));_tmpF->hd=(void*)f((void*)x->hd,(void*)y->hd);_tmpF->tl=0;
_tmpF;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;x=x->tl;y=y->tl;}if(
x != 0?1: y != 0)(int)_throw((void*)Cyc_List_List_mismatch);return result;}struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}void Cyc_List_app(void*(*f)(
void*),struct Cyc_List_List*x){while(x != 0){f((void*)x->hd);x=x->tl;}}void Cyc_List_app_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){while(x != 0){f(env,(void*)
x->hd);x=x->tl;}}void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){while(
x != 0){f((void*)x->hd);x=x->tl;}}void Cyc_List_iter_c(void(*f)(void*,void*),void*
env,struct Cyc_List_List*x){while(x != 0){f(env,(void*)x->hd);x=x->tl;}}void Cyc_List_app2(
void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){while(x != 0?
y != 0: 0){f((void*)x->hd,(void*)y->hd);x=x->tl;y=y->tl;}if(x != 0?1: y != 0)(int)
_throw((void*)Cyc_List_List_mismatch);}void Cyc_List_app2_c(void*(*f)(void*,void*,
void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){while(x != 0?y != 0: 0){
f(env,(void*)x->hd,(void*)y->hd);x=x->tl;y=y->tl;}if(x != 0?1: y != 0)(int)_throw((
void*)Cyc_List_List_mismatch);}void Cyc_List_iter2(void(*f)(void*,void*),struct
Cyc_List_List*x,struct Cyc_List_List*y){while(x != 0?y != 0: 0){f((void*)x->hd,(void*)
y->hd);x=x->tl;y=y->tl;}if(x != 0?1: y != 0)(int)_throw((void*)Cyc_List_List_mismatch);}
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,
struct Cyc_List_List*y){while(x != 0?y != 0: 0){f(env,(void*)x->hd,(void*)y->hd);x=x->tl;
y=y->tl;}if(x != 0?1: y != 0)(int)_throw((void*)Cyc_List_List_mismatch);}void*Cyc_List_fold_left(
void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){while(x != 0){accum=f(
accum,(void*)x->hd);x=x->tl;}return accum;}void*Cyc_List_fold_left_c(void*(*f)(
void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){while(x != 0){accum=
f(env,accum,(void*)x->hd);x=x->tl;}return accum;}void*Cyc_List_fold_right(void*(*
f)(void*,void*),struct Cyc_List_List*x,void*accum){if(x == 0)return accum;else{
return f((void*)x->hd,Cyc_List_fold_right(f,x->tl,accum));}}void*Cyc_List_fold_right_c(
void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){if(x == 0)
return accum;else{return f(env,(void*)x->hd,Cyc_List_fold_right_c(f,env,x->tl,
accum));}}struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct
Cyc_List_List*x,struct Cyc_List_List*y){while(x != 0){y=({struct Cyc_List_List*
_tmp10=_region_malloc(r2,sizeof(*_tmp10));_tmp10->hd=(void*)((void*)x->hd);
_tmp10->tl=y;_tmp10;});x=x->tl;}return y;}struct Cyc_List_List*Cyc_List_revappend(
struct Cyc_List_List*x,struct Cyc_List_List*y){return Cyc_List_rrevappend(Cyc_Core_heap_region,
x,y);}struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*
x){if(x == 0)return 0;return Cyc_List_rrevappend(r2,x,0);}struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x){return Cyc_List_rrev(Cyc_Core_heap_region,x);}struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x){if(x == 0)return x;else{struct Cyc_List_List*
first=x;struct Cyc_List_List*second=x->tl;x->tl=0;while(second != 0){struct Cyc_List_List*
temp=second->tl;second->tl=first;first=second;second=temp;}return first;}}struct
Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,
struct Cyc_List_List*y){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x
== 0)return y;if(y == 0)return Cyc_List_rcopy(r2,x);result=({struct Cyc_List_List*
_tmp11=_region_malloc(r2,sizeof(*_tmp11));_tmp11->hd=(void*)((void*)x->hd);
_tmp11->tl=0;_tmp11;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp12=_region_malloc(r2,sizeof(*
_tmp12));_tmp12->hd=(void*)((void*)x->hd);_tmp12->tl=0;_tmp12;});prev=((struct
Cyc_List_List*)_check_null(prev))->tl;}((struct Cyc_List_List*)_check_null(prev))->tl=
y;return result;}struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct
Cyc_List_List*y){return Cyc_List_rappend(Cyc_Core_heap_region,x,y);}struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*
z;if(x == 0)return y;if(y == 0)return x;for(z=x;z->tl != 0;z=z->tl){;}z->tl=y;return x;}
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r3,struct Cyc_List_List*
x){return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,
struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_List_List*
x,struct Cyc_List_List*accum))Cyc_List_fold_right_c)(Cyc_List_rappend,r3,x,0);}
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){return Cyc_List_rflatten(
Cyc_Core_heap_region,x);}struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(
void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){struct Cyc_List_List*c;
struct Cyc_List_List*d;if(a == 0)return b;if(b == 0)return a;if(less_eq((void*)a->hd,(
void*)b->hd)<= 0){c=a;a=a->tl;}else{c=b;b=b->tl;}d=c;while(a != 0?b != 0: 0){if(
less_eq((void*)a->hd,(void*)b->hd)<= 0){c->tl=a;c=a;a=a->tl;}else{c->tl=b;c=b;b=
b->tl;}}if(a == 0)c->tl=b;else{c->tl=a;}return d;}struct Cyc_List_List*Cyc_List_rimp_merge_sort(
int(*less_eq)(void*,void*),struct Cyc_List_List*x){if(x == 0?1: x->tl == 0)return x;{
struct Cyc_List_List*a=x;struct Cyc_List_List*aptr=a;struct Cyc_List_List*b=x->tl;
struct Cyc_List_List*bptr=b;x=((struct Cyc_List_List*)_check_null(b))->tl;while(x
!= 0){aptr->tl=x;aptr=x;x=x->tl;if(x != 0){((struct Cyc_List_List*)_check_null(
bptr))->tl=x;bptr=x;x=x->tl;}}aptr->tl=0;((struct Cyc_List_List*)_check_null(bptr))->tl=
0;return Cyc_List_imp_merge(less_eq,Cyc_List_rimp_merge_sort(less_eq,a),Cyc_List_rimp_merge_sort(
less_eq,b));}}struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r2,
int(*less_eq)(void*,void*),struct Cyc_List_List*x){return Cyc_List_rimp_merge_sort(
less_eq,Cyc_List_rcopy(r2,x));}struct Cyc_List_List*Cyc_List_rmerge(struct
_RegionHandle*r3,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*
b){struct Cyc_List_List*c;struct Cyc_List_List*d;if(a == 0)return Cyc_List_rcopy(r3,
b);if(b == 0)return Cyc_List_rcopy(r3,a);if(less_eq((void*)a->hd,(void*)b->hd)<= 0){
c=({struct Cyc_List_List*_tmp13=_region_malloc(r3,sizeof(*_tmp13));_tmp13->hd=(
void*)((void*)a->hd);_tmp13->tl=0;_tmp13;});a=a->tl;}else{c=({struct Cyc_List_List*
_tmp14=_region_malloc(r3,sizeof(*_tmp14));_tmp14->hd=(void*)((void*)b->hd);
_tmp14->tl=0;_tmp14;});b=b->tl;}d=c;while(a != 0?b != 0: 0){if(less_eq((void*)a->hd,(
void*)b->hd)<= 0){((struct Cyc_List_List*)_check_null(c))->tl=({struct Cyc_List_List*
_tmp15=_region_malloc(r3,sizeof(*_tmp15));_tmp15->hd=(void*)((void*)a->hd);
_tmp15->tl=0;_tmp15;});c=c->tl;a=a->tl;}else{((struct Cyc_List_List*)_check_null(
c))->tl=({struct Cyc_List_List*_tmp16=_region_malloc(r3,sizeof(*_tmp16));_tmp16->hd=(
void*)((void*)b->hd);_tmp16->tl=0;_tmp16;});c=c->tl;b=b->tl;}}if(a == 0)((struct
Cyc_List_List*)_check_null(c))->tl=Cyc_List_rcopy(r3,b);else{((struct Cyc_List_List*)
_check_null(c))->tl=Cyc_List_rcopy(r3,a);}return d;}struct Cyc_List_List*Cyc_List_merge_sort(
int(*less_eq)(void*,void*),struct Cyc_List_List*x){return Cyc_List_rmerge_sort(Cyc_Core_heap_region,
less_eq,x);}struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct
Cyc_List_List*a,struct Cyc_List_List*b){return Cyc_List_rmerge(Cyc_Core_heap_region,
less_eq,a,b);}char Cyc_List_Nth[8]="\000\000\000\000Nth\000";void*Cyc_List_nth(
struct Cyc_List_List*x,int i){while(i > 0?x != 0: 0){-- i;x=x->tl;}if(i < 0?1: x == 0)(
int)_throw((void*)Cyc_List_Nth);return(void*)x->hd;}struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i){if(i < 0)(int)_throw((void*)Cyc_List_Nth);while(i != 0){
if(x == 0)(int)_throw((void*)Cyc_List_Nth);x=x->tl;-- i;}return x;}int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x){while(x != 0?pred((void*)x->hd): 0){x=x->tl;}
return x == 0;}int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x){while(x != 0?pred(env,(void*)x->hd): 0){x=x->tl;}return x == 0;}int Cyc_List_exists(
int(*pred)(void*),struct Cyc_List_List*x){while(x != 0?!pred((void*)x->hd): 0){x=x->tl;}
return x != 0;}int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x){while(x != 0?!pred(env,(void*)x->hd): 0){x=x->tl;}return x != 0;}struct _tuple2{
void*f1;void*f2;};struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,
struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*
result;struct Cyc_List_List*prev;if(x == 0?y == 0: 0)return 0;if(x == 0?1: y == 0)(int)
_throw((void*)Cyc_List_List_mismatch);result=({struct Cyc_List_List*_tmp17=
_region_malloc(r3,sizeof(*_tmp17));_tmp17->hd=({struct _tuple2*_tmp18=
_region_malloc(r4,sizeof(*_tmp18));_tmp18->f1=(void*)x->hd;_tmp18->f2=(void*)y->hd;
_tmp18;});_tmp17->tl=0;_tmp17;});prev=result;x=x->tl;y=y->tl;while(x != 0?y != 0: 0){((
struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp19=
_region_malloc(r3,sizeof(*_tmp19));_tmp19->hd=({struct _tuple2*_tmp1A=
_region_malloc(r4,sizeof(*_tmp1A));_tmp1A->f1=(void*)x->hd;_tmp1A->f2=(void*)y->hd;
_tmp1A;});_tmp19->tl=0;_tmp19;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;
x=x->tl;y=y->tl;}if(x != 0?1: y != 0)(int)_throw((void*)Cyc_List_List_mismatch);
return result;}struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*
y){return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct
_tuple3{void*f1;void*f2;void*f3;};struct Cyc_List_List*Cyc_List_rzip3(struct
_RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*
y,struct Cyc_List_List*z){struct Cyc_List_List*result;struct Cyc_List_List*prev;if((
x == 0?y == 0: 0)?z == 0: 0)return 0;if((x == 0?1: y == 0)?1: z == 0)(int)_throw((void*)Cyc_List_List_mismatch);
result=({struct Cyc_List_List*_tmp1B=_region_malloc(r3,sizeof(*_tmp1B));_tmp1B->hd=({
struct _tuple3*_tmp1C=_region_malloc(r4,sizeof(*_tmp1C));_tmp1C->f1=(void*)x->hd;
_tmp1C->f2=(void*)y->hd;_tmp1C->f3=(void*)z->hd;_tmp1C;});_tmp1B->tl=0;_tmp1B;});
prev=result;x=x->tl;y=y->tl;z=z->tl;while((x != 0?y != 0: 0)?z != 0: 0){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp1D=_region_malloc(r3,sizeof(*
_tmp1D));_tmp1D->hd=({struct _tuple3*_tmp1E=_region_malloc(r4,sizeof(*_tmp1E));
_tmp1E->f1=(void*)x->hd;_tmp1E->f2=(void*)y->hd;_tmp1E->f3=(void*)z->hd;_tmp1E;});
_tmp1D->tl=0;_tmp1D;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;x=x->tl;
y=y->tl;z=z->tl;}if((x != 0?1: y != 0)?1: z != 0)(int)_throw((void*)Cyc_List_List_mismatch);
return result;}struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*
y,struct Cyc_List_List*z){return Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,
x,y,z);}struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*
r4,struct Cyc_List_List*x){struct Cyc_List_List*result1;struct Cyc_List_List*prev1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;if(x == 0)return({struct
_tuple0 _tmp1F;_tmp1F.f1=0;_tmp1F.f2=0;_tmp1F;});prev1=(result1=({struct Cyc_List_List*
_tmp20=_region_malloc(r3,sizeof(*_tmp20));_tmp20->hd=(void*)(((struct _tuple2*)x->hd)[
_check_known_subscript_notnull(1,0)]).f1;_tmp20->tl=0;_tmp20;}));prev2=(result2=({
struct Cyc_List_List*_tmp21=_region_malloc(r4,sizeof(*_tmp21));_tmp21->hd=(void*)(((
struct _tuple2*)x->hd)[_check_known_subscript_notnull(1,0)]).f2;_tmp21->tl=0;
_tmp21;}));for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)_check_null(prev1))->tl=({
struct Cyc_List_List*_tmp22=_region_malloc(r3,sizeof(*_tmp22));_tmp22->hd=(void*)(((
struct _tuple2*)x->hd)[_check_known_subscript_notnull(1,0)]).f1;_tmp22->tl=0;
_tmp22;});((struct Cyc_List_List*)_check_null(prev2))->tl=({struct Cyc_List_List*
_tmp23=_region_malloc(r4,sizeof(*_tmp23));_tmp23->hd=(void*)(((struct _tuple2*)x->hd)[
_check_known_subscript_notnull(1,0)]).f2;_tmp23->tl=0;_tmp23;});prev1=((struct
Cyc_List_List*)_check_null(prev1))->tl;prev2=((struct Cyc_List_List*)_check_null(
prev2))->tl;}return({struct _tuple0 _tmp24;_tmp24.f1=result1;_tmp24.f2=result2;
_tmp24;});}struct _tuple0 Cyc_List_split(struct Cyc_List_List*x){return Cyc_List_rsplit(
Cyc_Core_heap_region,Cyc_Core_heap_region,x);}struct _tuple1 Cyc_List_rsplit3(
struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*
x){struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*
result2;struct Cyc_List_List*prev2;struct Cyc_List_List*result3;struct Cyc_List_List*
prev3;if(x == 0)return({struct _tuple1 _tmp25;_tmp25.f1=0;_tmp25.f2=0;_tmp25.f3=0;
_tmp25;});prev1=(result1=({struct Cyc_List_List*_tmp26=_region_malloc(r3,sizeof(*
_tmp26));_tmp26->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f1;_tmp26->tl=0;_tmp26;}));prev2=(result2=({
struct Cyc_List_List*_tmp27=_region_malloc(r4,sizeof(*_tmp27));_tmp27->hd=(void*)(((
struct _tuple3*)x->hd)[_check_known_subscript_notnull(1,0)]).f2;_tmp27->tl=0;
_tmp27;}));prev3=(result3=({struct Cyc_List_List*_tmp28=_region_malloc(r5,sizeof(*
_tmp28));_tmp28->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f3;_tmp28->tl=0;_tmp28;}));for(x=x->tl;x != 
0;x=x->tl){((struct Cyc_List_List*)_check_null(prev1))->tl=({struct Cyc_List_List*
_tmp29=_region_malloc(r3,sizeof(*_tmp29));_tmp29->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f1;_tmp29->tl=0;_tmp29;});((struct Cyc_List_List*)
_check_null(prev2))->tl=({struct Cyc_List_List*_tmp2A=_region_malloc(r4,sizeof(*
_tmp2A));_tmp2A->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f2;_tmp2A->tl=0;_tmp2A;});((struct Cyc_List_List*)
_check_null(prev3))->tl=({struct Cyc_List_List*_tmp2B=_region_malloc(r5,sizeof(*
_tmp2B));_tmp2B->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f3;_tmp2B->tl=0;_tmp2B;});prev1=((struct
Cyc_List_List*)_check_null(prev1))->tl;prev2=((struct Cyc_List_List*)_check_null(
prev2))->tl;prev3=((struct Cyc_List_List*)_check_null(prev3))->tl;}return({struct
_tuple1 _tmp2C;_tmp2C.f1=result1;_tmp2C.f2=result2;_tmp2C.f3=result3;_tmp2C;});}
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){return Cyc_List_rsplit3(Cyc_Core_heap_region,
Cyc_Core_heap_region,Cyc_Core_heap_region,x);}int Cyc_List_memq(struct Cyc_List_List*
l,void*x){while(l != 0){if((void*)l->hd == x)return 1;l=l->tl;}return 0;}int Cyc_List_mem(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){while(l != 0){if(cmp((void*)
l->hd,x)== 0)return 1;l=l->tl;}return 0;}void*Cyc_List_assoc(struct Cyc_List_List*l,
void*x){while(l != 0){if((((struct _tuple2*)l->hd)[_check_known_subscript_notnull(
1,0)]).f1 == x)return(((struct _tuple2*)l->hd)[_check_known_subscript_notnull(1,0)]).f2;
l=l->tl;}(int)_throw((void*)Cyc_Core_Not_found);}void*Cyc_List_assoc_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l,void*x){while(l != 0){if(cmp((*((struct
_tuple2*)l->hd)).f1,x)== 0)return(*((struct _tuple2*)l->hd)).f2;l=l->tl;}(int)
_throw((void*)Cyc_Core_Not_found);}struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*
l,void*x){struct Cyc_List_List*_tmp2D=l;struct Cyc_List_List*_tmp2E=l;while(_tmp2E
!= 0){if((void*)_tmp2E->hd == x){if(_tmp2D == l)return((struct Cyc_List_List*)
_check_null(l))->tl;else{((struct Cyc_List_List*)_check_null(_tmp2D))->tl=_tmp2E->tl;
return l;}}_tmp2D=_tmp2E;_tmp2E=_tmp2E->tl;}(int)_throw((void*)Cyc_Core_Not_found);}
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){while(l != 0){if((((struct
_tuple2*)l->hd)[_check_known_subscript_notnull(1,0)]).f1 == x)return 1;l=l->tl;}
return 0;}struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct
Cyc_List_List*x){while(x != 0){if(x->tl != 0){if(cmp((void*)x->hd,(void*)((struct
Cyc_List_List*)_check_null(x->tl))->hd)== 0)return({struct Cyc_Core_Opt*_tmp2F=
_cycalloc(sizeof(*_tmp2F));_tmp2F->v=(void*)((void*)x->hd);_tmp2F;});}x=x->tl;}
return 0;}struct _tagged_arr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*
x){int s;struct _tagged_arr arr;s=Cyc_List_length(x);arr=({unsigned int _tmp30=(
unsigned int)s;void**_tmp31=(void**)_region_malloc(r2,_check_times(sizeof(void*),
_tmp30));struct _tagged_arr _tmp34=_tag_arr(_tmp31,sizeof(void*),_tmp30);{
unsigned int _tmp32=_tmp30;unsigned int i;for(i=0;i < _tmp32;i ++){_tmp31[i]=({void*
_tmp33=(void*)((struct Cyc_List_List*)_check_null(x))->hd;x=x->tl;_tmp33;});}}
_tmp34;});return arr;}struct _tagged_arr Cyc_List_to_array(struct Cyc_List_List*x){
return Cyc_List_rto_array(Cyc_Core_heap_region,x);}struct Cyc_List_List*Cyc_List_rfrom_array(
struct _RegionHandle*r2,struct _tagged_arr arr){struct Cyc_List_List*ans=0;{int i=(
int)(_get_arr_size(arr,sizeof(void*))- 1);for(0;i >= 0;-- i){ans=({struct Cyc_List_List*
_tmp35=_region_malloc(r2,sizeof(*_tmp35));_tmp35->hd=(void*)*((void**)
_check_unknown_subscript(arr,sizeof(void*),i));_tmp35->tl=ans;_tmp35;});}}return
ans;}struct Cyc_List_List*Cyc_List_from_array(struct _tagged_arr arr){return Cyc_List_rfrom_array(
Cyc_Core_heap_region,arr);}struct Cyc_List_List*Cyc_List_rtabulate(struct
_RegionHandle*r,int n,void*(*f)(int)){struct Cyc_List_List*res=0;{int i=0;for(0;i < 
n;++ i){res=({struct Cyc_List_List*_tmp36=_region_malloc(r,sizeof(*_tmp36));_tmp36->hd=(
void*)f(i);_tmp36->tl=res;_tmp36;});}}return Cyc_List_imp_rev(res);}struct Cyc_List_List*
Cyc_List_tabulate(int n,void*(*f)(int)){return Cyc_List_rtabulate(Cyc_Core_heap_region,
n,f);}struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*
f)(void*,int),void*env){struct Cyc_List_List*res=0;{int i=0;for(0;i < n;++ i){res=({
struct Cyc_List_List*_tmp37=_region_malloc(r,sizeof(*_tmp37));_tmp37->hd=(void*)f(
env,i);_tmp37->tl=res;_tmp37;});}}return Cyc_List_imp_rev(res);}struct Cyc_List_List*
Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){return Cyc_List_rtabulate_c(
Cyc_Core_heap_region,n,f,env);}int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,
l2=l2->tl)){if((unsigned int)l1 == (unsigned int)l2)return 0;{int _tmp38=cmp((void*)
l1->hd,(void*)l2->hd);if(_tmp38 != 0)return _tmp38;}}if(l1 != 0)return 1;if(l2 != 0)
return - 1;return 0;}int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2){for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){if((
unsigned int)l1 == (unsigned int)l2)return 1;{int _tmp39=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp39 != 0)return 0;}}return l1 == 0;}struct Cyc_List_List*Cyc_List_rfilter_c(
struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){if(l
== 0)return 0;{struct Cyc_List_List*result=({struct Cyc_List_List*_tmp3B=
_region_malloc(r,sizeof(*_tmp3B));_tmp3B->hd=(void*)((void*)l->hd);_tmp3B->tl=0;
_tmp3B;});struct Cyc_List_List*end=result;for(0;l != 0;l=l->tl){if(f(env,(void*)l->hd)){((
struct Cyc_List_List*)_check_null(end))->tl=({struct Cyc_List_List*_tmp3A=
_region_malloc(r,sizeof(*_tmp3A));_tmp3A->hd=(void*)((void*)l->hd);_tmp3A->tl=0;
_tmp3A;});end=((struct Cyc_List_List*)_check_null(end))->tl;}}return result->tl;}}
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*
l){return Cyc_List_rfilter_c(Cyc_Core_heap_region,f,env,l);}struct Cyc_List_List*
Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){if(
l == 0)return 0;{struct Cyc_List_List*result=({struct Cyc_List_List*_tmp3D=
_region_malloc(r,sizeof(*_tmp3D));_tmp3D->hd=(void*)((void*)l->hd);_tmp3D->tl=0;
_tmp3D;});struct Cyc_List_List*end=result;for(0;l != 0;l=l->tl){if(f((void*)l->hd)){((
struct Cyc_List_List*)_check_null(end))->tl=({struct Cyc_List_List*_tmp3C=
_region_malloc(r,sizeof(*_tmp3C));_tmp3C->hd=(void*)((void*)l->hd);_tmp3C->tl=0;
_tmp3C;});end=((struct Cyc_List_List*)_check_null(end))->tl;}}return result->tl;}}
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){return
Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}
