/* This is a C header file to be used by the output of the Cyclone
   to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dynforward_ptr {
  unsigned char *curr;
  unsigned char *last_plus_one;
};

struct _dyneither_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
};

/* Regions */
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

/* Exceptions */
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

/* Built-in Exceptions */
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

/* Built-in Run-time Checks and company */
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

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
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

/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
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

/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus. */
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  


/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
static inline unsigned char *
_check_dynforward_subscript(struct _dynforward_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dynforward_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#define _check_dynforward_subscript(arr,elt_sz,index) ({ \
  struct _dynforward_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.base) _throw_null();
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
static inline unsigned char *
_check_dynforward_subscript(struct _dynforward_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dynforward_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.last_plus_one) _throw_null();
  if (_cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _check_dynforward_subscript(arr,elt_sz,index) ({ \
  struct _dynforward_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.last_plus_one) _throw_null(); \
  if (_cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
static inline struct _dynforward_ptr
_tag_dynforward(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dynforward_ptr _tag_arr_ans;
  _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.curr + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#define _tag_dynforward(tcurr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr _tag_arr_ans; \
  _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.curr + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
static inline struct _dynforward_ptr *
_init_dynforward_ptr(struct _dynforward_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#define _init_dynforward_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dynforward_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
static inline unsigned char *
_untag_dynforward_ptr(struct _dynforward_ptr arr, 
                      unsigned elt_sz,unsigned num_elts) {
  struct _dynforward_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#define _untag_dynforward_ptr(arr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
static inline unsigned
_get_dynforward_size(struct _dynforward_ptr arr,unsigned elt_sz) {
  struct _dynforward_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#define _get_dynforward_size(arr,elt_sz) \
  ({struct _dynforward_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
/* Here we have to worry about wrapping around, so if we go past the
 * end, we set the end to 0. */
static inline struct _dynforward_ptr
_dynforward_ptr_plus(struct _dynforward_ptr arr,unsigned elt_sz,int change) {
  struct _dynforward_ptr _ans = (arr);
  unsigned int _dfpp_elts = (((unsigned)_ans.last_plus_one) - 
                             ((unsigned)_ans.curr)) / elt_sz;
  if (change < 0 || ((unsigned)change) > _dfpp_elts)
    _ans.last_plus_one = 0;
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#define _dynforward_ptr_plus(arr,elt_sz,change) ({ \
  struct _dynforward_ptr _ans = (arr); \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_ans.last_plus_one) - \
                            ((unsigned)_ans.curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _ans.last_plus_one = 0; \
  _ans.curr += ((int)(_dfpp_elt_sz))*(_dfpp_change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
static inline struct _dynforward_ptr
_dynforward_ptr_inplace_plus(struct _dynforward_ptr *arr_ptr,unsigned elt_sz,
                             int change) {
  struct _dynforward_ptr * _arr_ptr = (arr_ptr);
  unsigned int _dfpp_elts = (((unsigned)_ans->last_plus_one) - 
                             ((unsigned)_ans->curr)) / elt_sz;
  if (change < 0 || ((unsigned)change) > _dfpp_elts) 
    _arr_ptr->last_plus_one = 0;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#define _dynforward_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dynforward_ptr * _arr_ptr = (arr_ptr); \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - \
                            ((unsigned)_arr_ptr->curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _arr_ptr->last_plus_one = 0; \
  _arr_ptr->curr += ((int)(_dfpp_elt_sz))*(_dfpp_change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
static inline struct _dynforward_ptr
_dynforward_ptr_inplace_plus_post(struct _dynforward_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dynforward_ptr * _arr_ptr = (arr_ptr);
  struct _dynforward_ptr _ans = *_arr_ptr;
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - 
                            ((unsigned)_arr_ptr->curr)) / elt_sz; 
  if (change < 0 || ((unsigned)change) > _dfpp_elts) 
    _arr_ptr->last_plus_one = 0; 
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#define _dynforward_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dynforward_ptr * _arr_ptr = (arr_ptr); \
  struct _dynforward_ptr _ans = *_arr_ptr; \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - \
                            ((unsigned)_arr_ptr->curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _arr_ptr->last_plus_one = 0; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}
static struct 
_dynforward_ptr _dynforward_ptr_decrease_size(struct _dynforward_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  if (x.last_plus_one != 0)
    x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Convert between the two forms of dynamic pointers */
#ifdef _INLINE_FUNCTIONS 
static struct _dynforward_ptr
_dyneither_to_dynforward(struct _dyneither_ptr p) {
  struct _dynforward_ptr res;
  res.curr = p.curr;
  res.last_plus_one = (p.base == 0) ? 0 : p.last_plus_one;
  return res;
}
static struct _dyneither_ptr
_dynforward_to_dyneither(struct _dynforward_ptr p) {
  struct _dyneither_ptr res;
  res.base = res.curr = p.curr;
  res.last_plus_one = p.last_plus_one;
  if (p.last_plus_one == 0) 
    res.base = 0;
  return res;
}
#else 
#define _dyneither_to_dynforward(_dnfptr) ({ \
  struct _dyneither_ptr _dnfp = (_dnfptr); \
  struct _dynforward_ptr _dnfpres; \
  _dnfpres.curr = _dnfp.curr; \
  _dnfpres.last_plus_one = (_dnfp.base == 0) ? 0 : _dnfp.last_plus_one; \
  _dnfpres; })
#define _dynforward_to_dyneither(_dfnptr) ({ \
  struct _dynforward_ptr _dfnp = (_dfnptr); \
  struct _dyneither_ptr _dfnres; \
  _dfnres.base = _dfnres.curr = _dfnp.curr; \
  _dfnres.last_plus_one = _dfnp.last_plus_one; \
  if (_dfnp.last_plus_one == 0) \
    _dfnres.base = 0; \
  _dfnres; })
#endif 


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
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int*identity;void*
kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;
struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*
f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct
Cyc_Absyn_Conref*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _dynforward_ptr f1;};
struct Cyc_Absyn_String_c_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dynforward_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
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
_dynforward_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dynforward_ptr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
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
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};struct _dynforward_ptr Cyc_vrprintf(struct
_RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_List_List*le;};void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(
struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple4{struct
Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;
void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct
_dynforward_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct _dynforward_ptr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict*consumed;
struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;
};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct
_dynforward_ptr msg,struct _dynforward_ptr ap){({void*_tmp0[0]={};Cyc_Tcutil_terr(
loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dynforward(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp1;}));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons
_tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))_tmp4=1;if(!
_tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*
_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;
struct Cyc_Absyn_Tuniondecl*_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((
int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;
_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(
sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=
q;_tmpE.f2=(void*)_tmp6;_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)
goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=33;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=34;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=32;_tmp14.f1=0;
_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9: if(*((int*)
_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*
_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,31));}),
_tag_dynforward(_tmp15,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({
struct Cyc_Absyn_Var_e_struct _tmp19;_tmp19.tag=1;_tmp19.f1=q;_tmp19.f2=(void*)((
void*)0);_tmp19;});_tmp18;})));goto _LL0;_LL0:;};_pop_handler();}else{void*_tmp3=(
void*)_exn_thrown;void*_tmp1B=_tmp3;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto _LLE;
_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1C=_cycalloc(
sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Var_e_struct _tmp1D;_tmp1D.tag=1;
_tmp1D.f1=q;_tmp1D.f2=(void*)((void*)0);_tmp1D;});_tmp1C;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=0;_tmp1E->f2=
e;_tmp1E;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(
void*)e1->r;struct _tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp1F)->f1;_LL12: {struct _handler_cons
_tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))_tmp23=1;
if(!_tmp23){{void*_tmp24=Cyc_Tcenv_lookup_ordinary(te,e1->loc,_tmp20);void*
_tmp25;struct Cyc_Absyn_Tuniondecl*_tmp26;struct Cyc_Absyn_Tunionfield*_tmp27;
struct Cyc_Absyn_Aggrdecl*_tmp28;_LL16: if(*((int*)_tmp24)!= 0)goto _LL18;_tmp25=(
void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp24)->f1;_LL17:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2A;_tmp2A.tag=11;_tmp2A.f1=e1;_tmp2A.f2=
es;_tmp2A.f3=0;_tmp2A;});_tmp29;})));_npop_handler(0);return;_LL18: if(*((int*)
_tmp24)!= 2)goto _LL1A;_tmp26=((struct Cyc_Tcenv_TunionRes_struct*)_tmp24)->f1;
_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp24)->f2;_LL19: if(_tmp27->typs == 0)({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp27->name));{void*_tmp2B[1]={&
_tmp2D};Cyc_Tcutil_terr(e->loc,({const char*_tmp2C="%s is a constant, not a function";
_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,33));}),
_tag_dynforward(_tmp2B,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_e_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({
struct Cyc_Absyn_Tunion_e_struct _tmp2F;_tmp2F.tag=32;_tmp2F.f1=es;_tmp2F.f2=
_tmp26;_tmp2F.f3=_tmp27;_tmp2F;});_tmp2E;})));_npop_handler(0);return;_LL1A: if(*((
int*)_tmp24)!= 1)goto _LL1C;_tmp28=((struct Cyc_Tcenv_AggrRes_struct*)_tmp24)->f1;
_LL1B: {struct Cyc_List_List*_tmp30=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp31=
_cycalloc(sizeof(*_tmp31));_tmp31[0]=({struct Cyc_Absyn_Struct_e_struct _tmp32;
_tmp32.tag=30;_tmp32.f1=_tmp28->name;_tmp32.f2=0;_tmp32.f3=_tmp30;_tmp32.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp28;_tmp32;});_tmp31;})));_npop_handler(0);return;}
_LL1C: if(*((int*)_tmp24)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp24)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp20));{
void*_tmp33[1]={& _tmp35};Cyc_Tcutil_terr(e->loc,({const char*_tmp34="%s is an enum constructor, not a function";
_tag_dynforward(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,42));}),
_tag_dynforward(_tmp33,sizeof(void*),1));}});_npop_handler(0);return;_LL15:;};
_pop_handler();}else{void*_tmp22=(void*)_exn_thrown;void*_tmp37=_tmp22;_LL21: if(
_tmp37 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp39;_tmp39.tag=11;_tmp39.f1=e1;_tmp39.f2=es;_tmp39.f3=0;_tmp39;});_tmp38;})));
return;_LL23:;_LL24:(void)_throw(_tmp37);_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=11;_tmp3B.f1=e1;
_tmp3B.f2=es;_tmp3B.f3=0;_tmp3B;});_tmp3A;})));return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3D;_tmp3D.tag=28;_tmp3D.f1=des;_tmp3D;});_tmp3C;})));return;}{void*t=*topt;
void*_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3F;void*_tmp40;
struct Cyc_Absyn_ArrayInfo _tmp41;void*_tmp42;struct Cyc_Absyn_Tqual _tmp43;_LL26:
if(_tmp3E <= (void*)4)goto _LL2C;if(*((int*)_tmp3E)!= 10)goto _LL28;_tmp3F=((struct
Cyc_Absyn_AggrType_struct*)_tmp3E)->f1;_tmp40=(void*)_tmp3F.aggr_info;_LL27:{
void*_tmp44=_tmp40;struct Cyc_Absyn_Aggrdecl**_tmp45;struct Cyc_Absyn_Aggrdecl*
_tmp46;_LL2F: if(*((int*)_tmp44)!= 0)goto _LL31;_LL30:({void*_tmp47[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp48="struct type not properly set";_tag_dynforward(_tmp48,sizeof(char),
_get_zero_arr_size(_tmp48,29));}),_tag_dynforward(_tmp47,sizeof(void*),0));});
_LL31: if(*((int*)_tmp44)!= 1)goto _LL2E;_tmp45=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp44)->f1;_tmp46=*_tmp45;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp4A;_tmp4A.tag=30;_tmp4A.f1=_tmp46->name;_tmp4A.f2=0;_tmp4A.f3=des;_tmp4A.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp46;_tmp4A;});_tmp49;})));_LL2E:;}goto _LL25;_LL28:
if(*((int*)_tmp3E)!= 7)goto _LL2A;_tmp41=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E)->f1;_tmp42=(void*)_tmp41.elt_type;_tmp43=_tmp41.tq;_LL29:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));
_tmp4B[0]=({struct Cyc_Absyn_Array_e_struct _tmp4C;_tmp4C.tag=28;_tmp4C.f1=des;
_tmp4C;});_tmp4B;})));goto _LL25;_LL2A: if(*((int*)_tmp3E)!= 11)goto _LL2C;_LL2B:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp4D=
_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp4E;
_tmp4E.tag=31;_tmp4E.f1=(void*)t;_tmp4E.f2=des;_tmp4E;});_tmp4D;})));goto _LL25;
_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4F=
_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Array_e_struct _tmp50;
_tmp50.tag=28;_tmp50.f1=des;_tmp50;});_tmp4F;})));goto _LL25;_LL25:;}}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp51[2]={& _tmp53,& _tmp54};Cyc_Tcutil_terr(e->loc,({const char*
_tmp52="test of %s has type %s instead of integral or * type";_tag_dynforward(
_tmp52,sizeof(char),_get_zero_arr_size(_tmp52,53));}),_tag_dynforward(_tmp51,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{
void*_tmp55=c;void*_tmp56;void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*
_tmp5B;int _tmp5C;struct _dynforward_ptr _tmp5D;_LL34: if(_tmp55 <= (void*)1)goto
_LL44;if(*((int*)_tmp55)!= 0)goto _LL36;_tmp56=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp55)->f1;if((int)_tmp56 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;
_LL36: if(*((int*)_tmp55)!= 0)goto _LL38;_tmp57=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp55)->f1;if((int)_tmp57 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;
_LL38: if(*((int*)_tmp55)!= 0)goto _LL3A;_tmp58=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp55)->f1;if((int)_tmp58 != 2)goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;
_LL3A: if(*((int*)_tmp55)!= 1)goto _LL3C;_tmp59=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp55)->f1;_LL3B: t=_tmp59 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
goto _LL33;_LL3C: if(*((int*)_tmp55)!= 3)goto _LL3E;_tmp5A=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp55)->f1;_LL3D: t=_tmp5A == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL33;_LL3E: if(*((int*)_tmp55)!= 4)goto _LL40;_LL3F: t=Cyc_Absyn_float_typ;
goto _LL33;_LL40: if(*((int*)_tmp55)!= 2)goto _LL42;_tmp5B=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp55)->f1;_tmp5C=((struct Cyc_Absyn_Int_c_struct*)_tmp55)->f2;_LL41: if(topt == 0)
t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp5E=Cyc_Tcutil_compress(*
topt);void*_tmp5F;void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;
void*_tmp65;void*_tmp66;void*_tmp67;_LL47: if(_tmp5E <= (void*)4)goto _LL53;if(*((
int*)_tmp5E)!= 5)goto _LL49;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E)->f1;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)
_tmp60 != 0)goto _LL49;_LL48:{void*_tmp68=_tmp5F;_LL56: if((int)_tmp68 != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp68 != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp68 != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6A;_tmp6A.tag=0;_tmp6A.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_Char_c_struct
_tmp6C;_tmp6C.tag=0;_tmp6C.f1=(void*)_tmp5F;_tmp6C.f2=(char)_tmp5C;_tmp6C;});
_tmp6B;}));_tmp6A;});_tmp69;})));goto _LL46;_LL49: if(*((int*)_tmp5E)!= 5)goto
_LL4B;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f1;_tmp62=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)_tmp62 != 1)goto _LL4B;_LL4A: t=
_tmp61 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Const_e_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[
0]=({struct Cyc_Absyn_Const_e_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(void*)((void*)({
struct Cyc_Absyn_Short_c_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({
struct Cyc_Absyn_Short_c_struct _tmp70;_tmp70.tag=1;_tmp70.f1=(void*)_tmp61;_tmp70.f2=(
short)_tmp5C;_tmp70;});_tmp6F;}));_tmp6E;});_tmp6D;})));goto _LL46;_LL4B: if(*((
int*)_tmp5E)!= 5)goto _LL4D;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E)->f1;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)
_tmp64 != 2)goto _LL4D;_LL4C: t=_tmp63 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: if(*((int*)_tmp5E)!= 5)goto _LL4F;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E)->f1;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E)->f2;if((int)
_tmp66 != 3)goto _LL4F;_LL4E: t=_tmp65 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F: if(*((int*)_tmp5E)!= 4)goto _LL51;if(!(_tmp5C == 0))goto _LL51;
_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp71=
_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_Const_e_struct _tmp72;
_tmp72.tag=0;_tmp72.f1=(void*)((void*)0);_tmp72;});_tmp71;})));{struct Cyc_List_List*
_tmp73=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_PointerType_struct
_tmp75;_tmp75.tag=4;_tmp75.f1=({struct Cyc_Absyn_PtrInfo _tmp76;_tmp76.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->v=_tmp73;_tmp79;}));_tmp76.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp76.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp77;_tmp77.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp78=_cycalloc(sizeof(*
_tmp78));_tmp78->v=_tmp73;_tmp78;}));_tmp77.nullable=Cyc_Absyn_true_conref;
_tmp77.bounds=Cyc_Absyn_empty_conref();_tmp77.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp77.ptrloc=0;_tmp77;});_tmp76;});_tmp75;});_tmp74;});
goto _LL46;}_LL51: if(*((int*)_tmp5E)!= 17)goto _LL53;_tmp67=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp5E)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp7A=({struct Cyc_Absyn_TypeInt_struct*
_tmp81=_cycalloc_atomic(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp82;_tmp82.tag=18;_tmp82.f1=_tmp5C;_tmp82;});_tmp81;});if(!Cyc_Tcutil_unify(
_tmp67,(void*)_tmp7A)){({struct Cyc_String_pa_struct _tmp7E;_tmp7E.tag=0;_tmp7E.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp7A));{struct Cyc_String_pa_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp67));{void*
_tmp7B[2]={& _tmp7D,& _tmp7E};Cyc_Tcutil_terr(loc,({const char*_tmp7C="expecting %s but found %s";
_tag_dynforward(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,26));}),
_tag_dynforward(_tmp7B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[
0]=({struct Cyc_Absyn_TagType_struct _tmp80;_tmp80.tag=17;_tmp80.f1=(void*)((void*)
_tmp7A);_tmp80;});_tmp7F;});goto _LL46;}_LL53:;_LL54: t=_tmp5B == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if(*((int*)_tmp55)!= 5)goto
_LL44;_tmp5D=((struct Cyc_Absyn_String_c_struct*)_tmp55)->f1;_LL43: {int len=(int)
_get_dynforward_size(_tmp5D,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({
struct Cyc_Absyn_Int_c_struct _tmp8F;_tmp8F.tag=2;_tmp8F.f1=(void*)((void*)1);
_tmp8F.f2=len;_tmp8F;});_tmp8E;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp83=
_cycalloc(sizeof(*_tmp83));_tmp83->v=(void*)Cyc_Absyn_uint_typ;_tmp83;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp85;_tmp85.tag=0;_tmp85.f1=elen;_tmp85;});_tmp84;}),Cyc_Absyn_true_conref);
if(topt != 0){void*_tmp86=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo
_tmp87;struct Cyc_Absyn_Tqual _tmp88;_LL5D: if(_tmp86 <= (void*)4)goto _LL61;if(*((
int*)_tmp86)!= 7)goto _LL5F;_tmp87=((struct Cyc_Absyn_ArrayType_struct*)_tmp86)->f1;
_tmp88=_tmp87.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp88,(
struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp86)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->v=(void*)t;_tmp89;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A->v=Cyc_Tcenv_lookup_type_vars(te);_tmp8A;})),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=elen;
_tmp8C;});_tmp8B;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D->v=(void*)t;_tmp8D;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((int)_tmp55 != 
0)goto _LL33;_LL45: {struct Cyc_List_List*_tmp90=Cyc_Tcenv_lookup_type_vars(te);t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91[0]=({struct Cyc_Absyn_PointerType_struct _tmp92;_tmp92.tag=4;_tmp92.f1=({
struct Cyc_Absyn_PtrInfo _tmp93;_tmp93.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp96=_cycalloc(sizeof(*
_tmp96));_tmp96->v=_tmp90;_tmp96;}));_tmp93.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp93.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp94;_tmp94.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*
_tmp95));_tmp95->v=_tmp90;_tmp95;}));_tmp94.nullable=Cyc_Absyn_true_conref;
_tmp94.bounds=Cyc_Absyn_empty_conref();_tmp94.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp94.ptrloc=0;_tmp94;});_tmp93;});_tmp92;});_tmp91;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp97=b;struct Cyc_Absyn_Vardecl*
_tmp98;struct Cyc_Absyn_Fndecl*_tmp99;struct Cyc_Absyn_Vardecl*_tmp9A;struct Cyc_Absyn_Vardecl*
_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9C;_LL64: if((int)_tmp97 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp9D[
1]={& _tmp9F};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp9E="undeclared identifier: %s";
_tag_dynforward(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,26));}),
_tag_dynforward(_tmp9D,sizeof(void*),1));}});_LL66: if(_tmp97 <= (void*)1)goto
_LL68;if(*((int*)_tmp97)!= 0)goto _LL68;_tmp98=((struct Cyc_Absyn_Global_b_struct*)
_tmp97)->f1;_LL67:*q=*_tmp98->name;return(void*)_tmp98->type;_LL68: if(_tmp97 <= (
void*)1)goto _LL6A;if(*((int*)_tmp97)!= 1)goto _LL6A;_tmp99=((struct Cyc_Absyn_Funname_b_struct*)
_tmp97)->f1;_LL69:*q=*_tmp99->name;return Cyc_Tcutil_fndecl2typ(_tmp99);_LL6A: if(
_tmp97 <= (void*)1)goto _LL6C;if(*((int*)_tmp97)!= 4)goto _LL6C;_tmp9A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp97)->f1;_LL6B: _tmp9B=_tmp9A;goto _LL6D;_LL6C: if(_tmp97 <= (void*)1)goto _LL6E;
if(*((int*)_tmp97)!= 3)goto _LL6E;_tmp9B=((struct Cyc_Absyn_Local_b_struct*)_tmp97)->f1;
_LL6D: _tmp9C=_tmp9B;goto _LL6F;_LL6E: if(_tmp97 <= (void*)1)goto _LL63;if(*((int*)
_tmp97)!= 2)goto _LL63;_tmp9C=((struct Cyc_Absyn_Param_b_struct*)_tmp97)->f1;_LL6F:(*
q).f1=(void*)0;return(void*)_tmp9C->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpA0=(void*)
fmt->r;void*_tmpA1;struct _dynforward_ptr _tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct
Cyc_Absyn_Exp _tmpA4;void*_tmpA5;void*_tmpA6;struct _dynforward_ptr _tmpA7;_LL71:
if(*((int*)_tmpA0)!= 0)goto _LL73;_tmpA1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpA0)->f1;if(_tmpA1 <= (void*)1)goto _LL73;if(*((int*)_tmpA1)!= 5)goto _LL73;
_tmpA2=((struct Cyc_Absyn_String_c_struct*)_tmpA1)->f1;_LL72: _tmpA7=_tmpA2;goto
_LL74;_LL73: if(*((int*)_tmpA0)!= 15)goto _LL75;_tmpA3=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA0)->f2;_tmpA4=*_tmpA3;_tmpA5=(void*)_tmpA4.r;if(*((int*)_tmpA5)!= 0)goto
_LL75;_tmpA6=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpA5)->f1;if(_tmpA6 <= (
void*)1)goto _LL75;if(*((int*)_tmpA6)!= 5)goto _LL75;_tmpA7=((struct Cyc_Absyn_String_c_struct*)
_tmpA6)->f1;_LL74: desc_types=type_getter(te,(struct _dynforward_ptr)_tmpA7,fmt->loc);
goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct Cyc_List_List*_tmpA8=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpA8 != 0;_tmpA8=_tmpA8->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpA8->hd);}}return;_LL70:;}if(opt_args != 0){struct Cyc_List_List*_tmpA9=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0  && _tmpA9 != 0;(desc_types=
desc_types->tl,_tmpA9=_tmpA9->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmpA9->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmpAC;
_tmpAC.tag=0;_tmpAC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpAA[2]={& _tmpAC,& _tmpAD};Cyc_Tcutil_terr(e->loc,({const char*_tmpAB="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,51));}),
_tag_dynforward(_tmpAA,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(
desc_types != 0)({void*_tmpAE[0]={};Cyc_Tcutil_terr(fmt->loc,({const char*_tmpAF="too few arguments";
_tag_dynforward(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,18));}),
_tag_dynforward(_tmpAE,sizeof(void*),0));});if(_tmpA9 != 0)({void*_tmpB0[0]={};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA9->hd)->loc,({const char*_tmpB1="too many arguments";
_tag_dynforward(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,19));}),
_tag_dynforward(_tmpB0,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB2=p;_LL78: if((int)_tmpB2 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB2 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB3[1]={& _tmpB5};Cyc_Tcutil_terr(
loc,({const char*_tmpB4="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,39));}),_tag_dynforward(_tmpB3,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB2 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB8;_tmpB8.tag=
0;_tmpB8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB6[1]={& _tmpB8};Cyc_Tcutil_terr(loc,({const char*_tmpB7="expecting integral or * type but found %s";
_tag_dynforward(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,42));}),
_tag_dynforward(_tmpB6,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB2 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpBB;_tmpBB.tag=0;_tmpBB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB9[1]={& _tmpBB};Cyc_Tcutil_terr(loc,({const char*_tmpBA="expecting integral type but found %s";
_tag_dynforward(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,37));}),
_tag_dynforward(_tmpB9,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB2 != 19)goto _LL82;_LL81:{void*_tmpBC=
t;struct Cyc_Absyn_PtrInfo _tmpBD;struct Cyc_Absyn_PtrAtts _tmpBE;struct Cyc_Absyn_Conref*
_tmpBF;_LL85: if(_tmpBC <= (void*)4)goto _LL89;if(*((int*)_tmpBC)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpBC)!= 4)goto _LL89;_tmpBD=((struct Cyc_Absyn_PointerType_struct*)
_tmpBC)->f1;_tmpBE=_tmpBD.ptr_atts;_tmpBF=_tmpBE.bounds;_LL88:{void*_tmpC0=(void*)(
Cyc_Absyn_compress_conref(_tmpBF))->v;void*_tmpC1;void*_tmpC2;void*_tmpC3;_LL8C:
if(_tmpC0 <= (void*)1)goto _LL92;if(*((int*)_tmpC0)!= 0)goto _LL8E;_tmpC1=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpC0)->f1;if((int)_tmpC1 != 0)goto _LL8E;_LL8D:
goto _LL8F;_LL8E: if(*((int*)_tmpC0)!= 0)goto _LL90;_tmpC2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpC0)->f1;if((int)_tmpC2 != 1)goto _LL90;_LL8F: goto _LL91;_LL90: if(*((int*)_tmpC0)
!= 0)goto _LL92;_tmpC3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpC0)->f1;if(
_tmpC3 <= (void*)2)goto _LL92;if(*((int*)_tmpC3)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC4[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC5="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,53));}),
_tag_dynforward(_tmpC4,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC6[1]={& _tmpC8};Cyc_Tcutil_terr(
loc,({const char*_tmpC7="size requires pointer or array type, not %s";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,44));}),
_tag_dynforward(_tmpC6,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpC9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCA="Non-unary primop";
_tag_dynforward(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,17));}),
_tag_dynforward(_tmpC9,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpCB[1]={& _tmpCD};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpCC="type %s cannot be used here";
_tag_dynforward(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,28));}),
_tag_dynforward(_tmpCB,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpCE;}));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD1;_tmpD1.tag=0;
_tmpD1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpCF[1]={& _tmpD1};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpD0="type %s cannot be used here";
_tag_dynforward(_tmpD0,sizeof(char),_get_zero_arr_size(_tmpD0,28));}),
_tag_dynforward(_tmpCF,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD2;}));}{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static
void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
void*_tmpD3=t1;struct Cyc_Absyn_PtrInfo _tmpD4;void*_tmpD5;struct Cyc_Absyn_Tqual
_tmpD6;struct Cyc_Absyn_PtrAtts _tmpD7;void*_tmpD8;struct Cyc_Absyn_Conref*_tmpD9;
struct Cyc_Absyn_Conref*_tmpDA;struct Cyc_Absyn_Conref*_tmpDB;_LL95: if(_tmpD3 <= (
void*)4)goto _LL97;if(*((int*)_tmpD3)!= 4)goto _LL97;_tmpD4=((struct Cyc_Absyn_PointerType_struct*)
_tmpD3)->f1;_tmpD5=(void*)_tmpD4.elt_typ;_tmpD6=_tmpD4.elt_tq;_tmpD7=_tmpD4.ptr_atts;
_tmpD8=(void*)_tmpD7.rgn;_tmpD9=_tmpD7.nullable;_tmpDA=_tmpD7.bounds;_tmpDB=
_tmpD7.zero_term;_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD5),(void*)
1))({void*_tmpDC[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDD="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpDD,sizeof(char),_get_zero_arr_size(_tmpDD,50));}),
_tag_dynforward(_tmpDC,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpDE[1]={& _tmpE0};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpDF="expecting int but found %s";_tag_dynforward(_tmpDF,
sizeof(char),_get_zero_arr_size(_tmpDF,27));}),_tag_dynforward(_tmpDE,sizeof(
void*),1));}});_tmpDA=Cyc_Absyn_compress_conref(_tmpDA);{void*_tmpE1=(void*)
_tmpDA->v;void*_tmpE2;void*_tmpE3;void*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;void*
_tmpE6;_LL9A: if(_tmpE1 <= (void*)1)goto _LLA2;if(*((int*)_tmpE1)!= 0)goto _LL9C;
_tmpE2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE1)->f1;if((int)_tmpE2 != 0)
goto _LL9C;_LL9B: return t1;_LL9C: if(*((int*)_tmpE1)!= 0)goto _LL9E;_tmpE3=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpE1)->f1;if((int)_tmpE3 != 1)goto _LL9E;_LL9D:
return t1;_LL9E: if(*((int*)_tmpE1)!= 0)goto _LLA0;_tmpE4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpE1)->f1;if(_tmpE4 <= (void*)2)goto _LLA0;if(*((int*)_tmpE4)!= 0)goto _LLA0;
_tmpE5=((struct Cyc_Absyn_Upper_b_struct*)_tmpE4)->f1;_LL9F: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpDB))({void*_tmpE7[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmpE8="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,70));}),
_tag_dynforward(_tmpE7,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpE9=({struct Cyc_Absyn_PointerType_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));
_tmpEA[0]=({struct Cyc_Absyn_PointerType_struct _tmpEB;_tmpEB.tag=4;_tmpEB.f1=({
struct Cyc_Absyn_PtrInfo _tmpEC;_tmpEC.elt_typ=(void*)_tmpD5;_tmpEC.elt_tq=_tmpD6;
_tmpEC.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpED;_tmpED.rgn=(void*)_tmpD8;_tmpED.nullable=
Cyc_Absyn_true_conref;_tmpED.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpED.zero_term=
_tmpDB;_tmpED.ptrloc=0;_tmpED;});_tmpEC;});_tmpEB;});_tmpEA;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpE9,(void*)3);return(void*)_tmpE9;}_LLA0: if(*((int*)_tmpE1)!= 0)
goto _LLA2;_tmpE6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE1)->f1;if(
_tmpE6 <= (void*)2)goto _LLA2;if(*((int*)_tmpE6)!= 1)goto _LLA2;_LLA1:({void*_tmpEE[
0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpEF="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,64));}),
_tag_dynforward(_tmpEE,sizeof(void*),0));});return t1;_LLA2:;_LLA3:(void*)(_tmpDA->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));
_tmpF0[0]=({struct Cyc_Absyn_Eq_constr_struct _tmpF1;_tmpF1.tag=0;_tmpF1.f1=(void*)((
void*)0);_tmpF1;});_tmpF0;})));return t1;_LL99:;}_LL97:;_LL98: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF5;
_tmpF5.tag=0;_tmpF5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF2[2]={& _tmpF4,&
_tmpF5};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF3="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,59));}),
_tag_dynforward(_tmpF2,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpF6[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpF7="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,43));}),
_tag_dynforward(_tmpF6,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpF8[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF9="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,50));}),
_tag_dynforward(_tmpF8,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({
struct Cyc_String_pa_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmpFA[2]={& _tmpFC,& _tmpFD};Cyc_Tcutil_terr(e2->loc,({const char*
_tmpFB="expecting either %s or int but found %s";_tag_dynforward(_tmpFB,sizeof(
char),_get_zero_arr_size(_tmpFB,40));}),_tag_dynforward(_tmpFA,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*
_tmpFE));_tmpFE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpFE;}))))return Cyc_Absyn_sint_typ;
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(te,
loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmp100=({struct _tuple0 _tmpFF;_tmpFF.f1=
Cyc_Tcutil_compress(t1);_tmpFF.f2=Cyc_Tcutil_compress(t2);_tmpFF;});void*_tmp101;
struct Cyc_Absyn_PtrInfo _tmp102;void*_tmp103;void*_tmp104;struct Cyc_Absyn_PtrInfo
_tmp105;void*_tmp106;_LLA5: _tmp101=_tmp100.f1;if(_tmp101 <= (void*)4)goto _LLA7;
if(*((int*)_tmp101)!= 4)goto _LLA7;_tmp102=((struct Cyc_Absyn_PointerType_struct*)
_tmp101)->f1;_tmp103=(void*)_tmp102.elt_typ;_tmp104=_tmp100.f2;if(_tmp104 <= (
void*)4)goto _LLA7;if(*((int*)_tmp104)!= 4)goto _LLA7;_tmp105=((struct Cyc_Absyn_PointerType_struct*)
_tmp104)->f1;_tmp106=(void*)_tmp105.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp103,
_tmp106))return Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({
struct Cyc_String_pa_struct _tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp109;_tmp109.tag=0;_tmp109.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp107[2]={& _tmp109,& _tmp10A};Cyc_Tcutil_terr(
loc,({const char*_tmp108="comparison not allowed between %s and %s";
_tag_dynforward(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,41));}),
_tag_dynforward(_tmp107,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp10B=_cycalloc(sizeof(*_tmp10B));
_tmp10B->v=Cyc_Tcenv_lookup_type_vars(te);_tmp10B;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp10C=p;_LLAA: if((int)_tmp10C != 
0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLAC: if((int)_tmp10C != 2)
goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLAE: if((int)_tmp10C != 1)goto
_LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp10C != 3)goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLB2: if((int)_tmp10C != 4)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((int)_tmp10C != 13)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp10C
!= 14)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp10C != 15)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if((int)_tmp10C != 16)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp10C
!= 17)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp10C != 18)goto _LLC0;_LLBF:
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);_LLC0: if((int)
_tmp10C != 5)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp10C != 6)goto _LLC4;_LLC3:
goto _LLC5;_LLC4: if((int)_tmp10C != 7)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)
_tmp10C != 8)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp10C != 9)goto _LLCA;_LLC9:
goto _LLCB;_LLCA: if((int)_tmp10C != 10)goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLCC:;_LLCD:({void*_tmp10D[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp10E="bad binary primop";
_tag_dynforward(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,18));}),
_tag_dynforward(_tmp10D,sizeof(void*),0));});_LLA9:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCE: return({void*_tmp10F[0]={};
Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp110="primitive operator has 0 arguments";
_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,35));}),
_tag_dynforward(_tmp10F,sizeof(void*),0));});case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLD1: return({void*_tmp111[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp112="primitive operator has > 2 arguments";_tag_dynforward(_tmp112,sizeof(
char),_get_zero_arr_size(_tmp112,37));}),_tag_dynforward(_tmp111,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp113=(void*)e->r;void*
_tmp114;struct Cyc_Absyn_Vardecl*_tmp115;void*_tmp116;struct Cyc_Absyn_Vardecl*
_tmp117;void*_tmp118;struct Cyc_Absyn_Vardecl*_tmp119;void*_tmp11A;struct Cyc_Absyn_Vardecl*
_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*
_tmp11E;struct _dynforward_ptr*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct
_dynforward_ptr*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;
struct Cyc_Absyn_Exp*_tmp124;_LLD4: if(*((int*)_tmp113)!= 1)goto _LLD6;_tmp114=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp113)->f2;if(_tmp114 <= (void*)1)goto
_LLD6;if(*((int*)_tmp114)!= 2)goto _LLD6;_tmp115=((struct Cyc_Absyn_Param_b_struct*)
_tmp114)->f1;_LLD5: _tmp117=_tmp115;goto _LLD7;_LLD6: if(*((int*)_tmp113)!= 1)goto
_LLD8;_tmp116=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp113)->f2;if(_tmp116 <= (
void*)1)goto _LLD8;if(*((int*)_tmp116)!= 3)goto _LLD8;_tmp117=((struct Cyc_Absyn_Local_b_struct*)
_tmp116)->f1;_LLD7: _tmp119=_tmp117;goto _LLD9;_LLD8: if(*((int*)_tmp113)!= 1)goto
_LLDA;_tmp118=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp113)->f2;if(_tmp118 <= (
void*)1)goto _LLDA;if(*((int*)_tmp118)!= 4)goto _LLDA;_tmp119=((struct Cyc_Absyn_Pat_b_struct*)
_tmp118)->f1;_LLD9: _tmp11B=_tmp119;goto _LLDB;_LLDA: if(*((int*)_tmp113)!= 1)goto
_LLDC;_tmp11A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp113)->f2;if(_tmp11A <= (
void*)1)goto _LLDC;if(*((int*)_tmp11A)!= 0)goto _LLDC;_tmp11B=((struct Cyc_Absyn_Global_b_struct*)
_tmp11A)->f1;_LLDB: if(!(_tmp11B->tq).real_const)return;goto _LLD3;_LLDC: if(*((int*)
_tmp113)!= 25)goto _LLDE;_tmp11C=((struct Cyc_Absyn_Subscript_e_struct*)_tmp113)->f1;
_tmp11D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp113)->f2;_LLDD:{void*_tmp125=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp11C->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_Tqual _tmp127;struct Cyc_Absyn_ArrayInfo
_tmp128;struct Cyc_Absyn_Tqual _tmp129;struct Cyc_List_List*_tmp12A;_LLEB: if(
_tmp125 <= (void*)4)goto _LLF1;if(*((int*)_tmp125)!= 4)goto _LLED;_tmp126=((struct
Cyc_Absyn_PointerType_struct*)_tmp125)->f1;_tmp127=_tmp126.elt_tq;_LLEC: _tmp129=
_tmp127;goto _LLEE;_LLED: if(*((int*)_tmp125)!= 7)goto _LLEF;_tmp128=((struct Cyc_Absyn_ArrayType_struct*)
_tmp125)->f1;_tmp129=_tmp128.tq;_LLEE: if(!_tmp129.real_const)return;goto _LLEA;
_LLEF: if(*((int*)_tmp125)!= 9)goto _LLF1;_tmp12A=((struct Cyc_Absyn_TupleType_struct*)
_tmp125)->f1;_LLF0: {unsigned int _tmp12C;int _tmp12D;struct _tuple7 _tmp12B=Cyc_Evexp_eval_const_uint_exp(
_tmp11D);_tmp12C=_tmp12B.f1;_tmp12D=_tmp12B.f2;if(!_tmp12D){({void*_tmp12E[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp12F="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,47));}),
_tag_dynforward(_tmp12E,sizeof(void*),0));});return;}{struct _handler_cons _tmp130;
_push_handler(& _tmp130);{int _tmp132=0;if(setjmp(_tmp130.handler))_tmp132=1;if(!
_tmp132){{struct _tuple9 _tmp134;struct Cyc_Absyn_Tqual _tmp135;struct _tuple9*
_tmp133=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp12A,(
int)_tmp12C);_tmp134=*_tmp133;_tmp135=_tmp134.f1;if(!_tmp135.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp131=(void*)_exn_thrown;
void*_tmp137=_tmp131;_LLF4: if(_tmp137 != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp137);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp113)!= 23)goto _LLE0;_tmp11E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp113)->f1;_tmp11F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp113)->f2;_LLDF:{
void*_tmp138=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11E->topt))->v);struct Cyc_Absyn_AggrInfo _tmp139;void*_tmp13A;struct Cyc_Absyn_Aggrdecl**
_tmp13B;struct Cyc_List_List*_tmp13C;_LLF9: if(_tmp138 <= (void*)4)goto _LLFD;if(*((
int*)_tmp138)!= 10)goto _LLFB;_tmp139=((struct Cyc_Absyn_AggrType_struct*)_tmp138)->f1;
_tmp13A=(void*)_tmp139.aggr_info;if(*((int*)_tmp13A)!= 1)goto _LLFB;_tmp13B=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp13A)->f1;_LLFA: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13B == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13B,_tmp11F);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFB: if(*((
int*)_tmp138)!= 11)goto _LLFD;_tmp13C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp138)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp13C,
_tmp11F);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp113)!= 24)goto _LLE2;_tmp120=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp113)->f1;_tmp121=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp113)->f2;_LLE1:{void*_tmp13D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp120->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13E;void*_tmp13F;
_LL100: if(_tmp13D <= (void*)4)goto _LL102;if(*((int*)_tmp13D)!= 4)goto _LL102;
_tmp13E=((struct Cyc_Absyn_PointerType_struct*)_tmp13D)->f1;_tmp13F=(void*)
_tmp13E.elt_typ;_LL101:{void*_tmp140=Cyc_Tcutil_compress(_tmp13F);struct Cyc_Absyn_AggrInfo
_tmp141;void*_tmp142;struct Cyc_Absyn_Aggrdecl**_tmp143;struct Cyc_List_List*
_tmp144;_LL105: if(_tmp140 <= (void*)4)goto _LL109;if(*((int*)_tmp140)!= 10)goto
_LL107;_tmp141=((struct Cyc_Absyn_AggrType_struct*)_tmp140)->f1;_tmp142=(void*)
_tmp141.aggr_info;if(*((int*)_tmp142)!= 1)goto _LL107;_tmp143=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp142)->f1;_LL106: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp143 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp143,_tmp121);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL104;}_LL107: if(*((int*)_tmp140)!= 11)goto _LL109;_tmp144=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp140)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp144,_tmp121);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL104;}_LL109:;_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103:
goto _LLFF;_LLFF:;}goto _LLD3;_LLE2: if(*((int*)_tmp113)!= 22)goto _LLE4;_tmp122=((
struct Cyc_Absyn_Deref_e_struct*)_tmp113)->f1;_LLE3:{void*_tmp145=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp122->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp146;struct Cyc_Absyn_Tqual _tmp147;struct Cyc_Absyn_ArrayInfo _tmp148;struct Cyc_Absyn_Tqual
_tmp149;_LL10C: if(_tmp145 <= (void*)4)goto _LL110;if(*((int*)_tmp145)!= 4)goto
_LL10E;_tmp146=((struct Cyc_Absyn_PointerType_struct*)_tmp145)->f1;_tmp147=
_tmp146.elt_tq;_LL10D: _tmp149=_tmp147;goto _LL10F;_LL10E: if(*((int*)_tmp145)!= 7)
goto _LL110;_tmp148=((struct Cyc_Absyn_ArrayType_struct*)_tmp145)->f1;_tmp149=
_tmp148.tq;_LL10F: if(!_tmp149.real_const)return;goto _LL10B;_LL110:;_LL111: goto
_LL10B;_LL10B:;}goto _LLD3;_LLE4: if(*((int*)_tmp113)!= 13)goto _LLE6;_tmp123=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp113)->f1;_LLE5: _tmp124=_tmp123;goto
_LLE7;_LLE6: if(*((int*)_tmp113)!= 14)goto _LLE8;_tmp124=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp113)->f1;_LLE7: Cyc_Tcexp_check_writable(te,_tmp124);return;_LLE8:;_LLE9: goto
_LLD3;_LLD3:;}({struct Cyc_String_pa_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp14A[1]={& _tmp14C};Cyc_Tcutil_terr(e->loc,({const char*_tmp14B="attempt to write a const location: %s";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,38));}),
_tag_dynforward(_tmp14A,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp14D[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp14E="increment/decrement of non-lvalue";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,34));}),
_tag_dynforward(_tmp14D,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp14F[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp150="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,50));}),
_tag_dynforward(_tmp14F,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp151[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp152="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp152,
sizeof(char),_get_zero_arr_size(_tmp152,43));}),_tag_dynforward(_tmp151,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp153[1]={& _tmp155};Cyc_Tcutil_terr(e->loc,({const char*_tmp154="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,44));}),
_tag_dynforward(_tmp153,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp156="conditional expression";_tag_dynforward(_tmp156,sizeof(char),
_get_zero_arr_size(_tmp156,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp15F;}));struct Cyc_List_List _tmp157=({struct Cyc_List_List _tmp15E;_tmp15E.hd=
e3;_tmp15E.tl=0;_tmp15E;});struct Cyc_List_List _tmp158=({struct Cyc_List_List
_tmp15D;_tmp15D.hd=e2;_tmp15D.tl=(struct Cyc_List_List*)& _tmp157;_tmp15D;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp158)){({struct Cyc_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{
struct Cyc_String_pa_struct _tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp159[2]={& _tmp15B,& _tmp15C};
Cyc_Tcutil_terr(loc,({const char*_tmp15A="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,48));}),
_tag_dynforward(_tmp159,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp160="logical-and expression";_tag_dynforward(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp161="logical-and expression";
_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size(_tmp161,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp162="logical-or expression";_tag_dynforward(_tmp162,sizeof(char),
_get_zero_arr_size(_tmp162,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp163="logical-or expression";
_tag_dynforward(_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
te),0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp164=Cyc_Tcutil_compress(t1);_LL113: if(_tmp164 <= (void*)4)goto _LL115;if(*((
int*)_tmp164)!= 7)goto _LL115;_LL114:({void*_tmp165[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp166="cannot assign to an array";_tag_dynforward(_tmp166,sizeof(
char),_get_zero_arr_size(_tmp166,26));}),_tag_dynforward(_tmp165,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp167[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp168="type is abstract (can't determine size).";
_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,41));}),
_tag_dynforward(_tmp167,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp169[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16A="assignment to non-lvalue";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,25));}),
_tag_dynforward(_tmp169,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp16B=({struct Cyc_String_pa_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp16E;_tmp16E.tag=0;
_tmp16E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp16C[2]={& _tmp16E,& _tmp16F};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp16D="type mismatch: %s != %s";_tag_dynforward(_tmp16D,sizeof(char),
_get_zero_arr_size(_tmp16D,24));}),_tag_dynforward(_tmp16C,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp16B;}}else{void*
_tmp170=(void*)po->v;void*_tmp171=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp170,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp171,t1) || Cyc_Tcutil_coerceable(_tmp171) && Cyc_Tcutil_coerceable(
t1))){void*_tmp172=({struct Cyc_String_pa_struct _tmp176;_tmp176.tag=0;_tmp176.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp173[2]={& _tmp175,& _tmp176};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp174="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,82));}),
_tag_dynforward(_tmp173,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp171,t1);Cyc_Tcutil_explain_failure();
return _tmp172;}return _tmp171;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp177=Cyc_Tcutil_compress(
t1);void*_tmp178;void*_tmp179;_LL118: if((int)_tmp177 != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp177 <= (void*)4)goto
_LL11E;if(*((int*)_tmp177)!= 5)goto _LL11C;_tmp178=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp177)->f2;if((int)_tmp178 != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp177)!= 5)goto _LL11E;_tmp179=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp177)->f2;
if((int)_tmp179 != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp17B;struct Cyc_List_List*
_tmp17C;struct Cyc_Position_Segment*_tmp17D;void*_tmp17E;struct Cyc_Absyn_Tunionfield
_tmp17A=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp17B=_tmp17A.name;_tmp17C=
_tmp17A.typs;_tmp17D=_tmp17A.loc;_tmp17E=(void*)_tmp17A.sc;if(_tmp17C == 0  || 
_tmp17C->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp17C->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp180;struct Cyc_List_List*_tmp181;struct Cyc_Position_Segment*
_tmp182;void*_tmp183;struct Cyc_Absyn_Tunionfield _tmp17F=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp180=_tmp17F.name;_tmp181=_tmp17F.typs;_tmp182=_tmp17F.loc;
_tmp183=(void*)_tmp17F.sc;if(_tmp181 == 0  || _tmp181->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp181->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp187;_tmp187.tag=0;_tmp187.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp186;_tmp186.tag=0;_tmp186.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp184[2]={& _tmp186,& _tmp187};Cyc_Tcutil_terr(e->loc,({const char*_tmp185="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,54));}),
_tag_dynforward(_tmp184,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp189=args;te=Cyc_Tcenv_new_block(loc,te);Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp18A=t;struct Cyc_Absyn_PtrInfo _tmp18B;void*_tmp18C;struct Cyc_Absyn_Tqual
_tmp18D;struct Cyc_Absyn_PtrAtts _tmp18E;void*_tmp18F;struct Cyc_Absyn_Conref*
_tmp190;struct Cyc_Absyn_Conref*_tmp191;struct Cyc_Absyn_Conref*_tmp192;_LL121: if(
_tmp18A <= (void*)4)goto _LL123;if(*((int*)_tmp18A)!= 4)goto _LL123;_tmp18B=((
struct Cyc_Absyn_PointerType_struct*)_tmp18A)->f1;_tmp18C=(void*)_tmp18B.elt_typ;
_tmp18D=_tmp18B.elt_tq;_tmp18E=_tmp18B.ptr_atts;_tmp18F=(void*)_tmp18E.rgn;
_tmp190=_tmp18E.nullable;_tmp191=_tmp18E.bounds;_tmp192=_tmp18E.zero_term;_LL122:
Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp18F);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp191);{void*_tmp193=Cyc_Tcutil_compress(_tmp18C);struct Cyc_Absyn_FnInfo
_tmp194;struct Cyc_List_List*_tmp195;struct Cyc_Core_Opt*_tmp196;void*_tmp197;
struct Cyc_List_List*_tmp198;int _tmp199;struct Cyc_Absyn_VarargInfo*_tmp19A;struct
Cyc_List_List*_tmp19B;struct Cyc_List_List*_tmp19C;_LL126: if(_tmp193 <= (void*)4)
goto _LL128;if(*((int*)_tmp193)!= 8)goto _LL128;_tmp194=((struct Cyc_Absyn_FnType_struct*)
_tmp193)->f1;_tmp195=_tmp194.tvars;_tmp196=_tmp194.effect;_tmp197=(void*)_tmp194.ret_typ;
_tmp198=_tmp194.args;_tmp199=_tmp194.c_varargs;_tmp19A=_tmp194.cyc_varargs;
_tmp19B=_tmp194.rgn_po;_tmp19C=_tmp194.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(
_tmp197,*topt);while(_tmp189 != 0  && _tmp198 != 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp189->hd;void*t2=(*((struct _tuple2*)_tmp198->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_String_pa_struct
_tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp19F;_tmp19F.tag=0;
_tmp19F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp19D[2]={&
_tmp19F,& _tmp1A0};Cyc_Tcutil_terr(e1->loc,({const char*_tmp19E="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp19E,sizeof(char),_get_zero_arr_size(_tmp19E,57));}),
_tag_dynforward(_tmp19D,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
_tmp189=_tmp189->tl;_tmp198=_tmp198->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp19C;for(0;a != 0;a=a->tl){void*_tmp1A1=(void*)a->hd;void*_tmp1A2;int _tmp1A3;
int _tmp1A4;_LL12B: if(_tmp1A1 <= (void*)17)goto _LL12D;if(*((int*)_tmp1A1)!= 3)goto
_LL12D;_tmp1A2=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f1;_tmp1A3=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f2;_tmp1A4=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A1)->f3;_LL12C:{struct _handler_cons _tmp1A5;_push_handler(& _tmp1A5);{int
_tmp1A7=0;if(setjmp(_tmp1A5.handler))_tmp1A7=1;if(!_tmp1A7){{struct Cyc_Absyn_Exp*
_tmp1A8=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A3 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A4 == 0)fmt_args=0;else{fmt_args=({
struct Cyc_Core_Opt*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A4 - 1);_tmp1A9;});}
args_already_checked=1;{void*_tmp1AA=_tmp1A2;_LL130: if((int)_tmp1AA != 0)goto
_LL132;_LL131: Cyc_Tcexp_check_format_args(te,_tmp1A8,fmt_args,Cyc_Formatstr_get_format_typs);
goto _LL12F;_LL132: if((int)_tmp1AA != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(
te,_tmp1A8,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;}};
_pop_handler();}else{void*_tmp1A6=(void*)_exn_thrown;void*_tmp1AC=_tmp1A6;_LL135:
if(_tmp1AC != Cyc_List_Nth)goto _LL137;_LL136:({void*_tmp1AD[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1AE="bad format arguments";_tag_dynforward(_tmp1AE,sizeof(
char),_get_zero_arr_size(_tmp1AE,21));}),_tag_dynforward(_tmp1AD,sizeof(void*),0));});
goto _LL134;_LL137:;_LL138:(void)_throw(_tmp1AC);_LL134:;}}}goto _LL12A;_LL12D:;
_LL12E: goto _LL12A;_LL12A:;}}if(_tmp198 != 0)({void*_tmp1AF[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1B0="too few arguments for function";_tag_dynforward(_tmp1B0,
sizeof(char),_get_zero_arr_size(_tmp1B0,31));}),_tag_dynforward(_tmp1AF,sizeof(
void*),0));});else{if((_tmp189 != 0  || _tmp199) || _tmp19A != 0){if(_tmp199)for(0;
_tmp189 != 0;_tmp189=_tmp189->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp189->hd);}else{if(_tmp19A == 0)({void*_tmp1B1[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1B2="too many arguments for function";_tag_dynforward(_tmp1B2,
sizeof(char),_get_zero_arr_size(_tmp1B2,32));}),_tag_dynforward(_tmp1B1,sizeof(
void*),0));});else{void*_tmp1B4;int _tmp1B5;struct Cyc_Absyn_VarargInfo _tmp1B3=*
_tmp19A;_tmp1B4=(void*)_tmp1B3.type;_tmp1B5=_tmp1B3.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1B6=({struct Cyc_Absyn_VarargCallInfo*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));
_tmp1CE->num_varargs=0;_tmp1CE->injectors=0;_tmp1CE->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp19A;_tmp1CE;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1B6;
if(!_tmp1B5)for(0;_tmp189 != 0;_tmp189=_tmp189->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp189->hd;_tmp1B6->num_varargs ++;Cyc_Tcexp_tcExp(te,(void**)&
_tmp1B4,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp1B4)){({struct Cyc_String_pa_struct
_tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1B4));{void*
_tmp1B7[2]={& _tmp1B9,& _tmp1BA};Cyc_Tcutil_terr(loc,({const char*_tmp1B8="vararg requires type %s but argument has type %s";
_tag_dynforward(_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,49));}),
_tag_dynforward(_tmp1B7,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp1BB=Cyc_Tcutil_compress(_tmp1B4);struct Cyc_Absyn_TunionInfo _tmp1BC;
void*_tmp1BD;struct Cyc_Absyn_Tuniondecl**_tmp1BE;struct Cyc_Absyn_Tuniondecl*
_tmp1BF;struct Cyc_List_List*_tmp1C0;void*_tmp1C1;_LL13A: if(_tmp1BB <= (void*)4)
goto _LL13C;if(*((int*)_tmp1BB)!= 2)goto _LL13C;_tmp1BC=((struct Cyc_Absyn_TunionType_struct*)
_tmp1BB)->f1;_tmp1BD=(void*)_tmp1BC.tunion_info;if(*((int*)_tmp1BD)!= 1)goto
_LL13C;_tmp1BE=((struct Cyc_Absyn_KnownTunion_struct*)_tmp1BD)->f1;_tmp1BF=*
_tmp1BE;_tmp1C0=_tmp1BC.targs;_tmp1C1=(void*)_tmp1BC.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1C2=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp1BF->name);struct Cyc_List_List*
fields=0;if(_tmp1C2->fields == 0)({struct Cyc_String_pa_struct _tmp1C5;_tmp1C5.tag=
0;_tmp1C5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1B4));{void*_tmp1C3[1]={& _tmp1C5};Cyc_Tcutil_terr(loc,({const char*_tmp1C4="can't inject into %s";
_tag_dynforward(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,21));}),
_tag_dynforward(_tmp1C3,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C2->fields))->v;}if(!Cyc_Tcutil_unify(
_tmp1C1,Cyc_Tcenv_curr_rgn(te)))({void*_tmp1C6[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp1C7="bad region for injected varargs";_tag_dynforward(_tmp1C7,sizeof(
char),_get_zero_arr_size(_tmp1C7,32));}),_tag_dynforward(_tmp1C6,sizeof(void*),0));});{
struct _RegionHandle _tmp1C8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1C8;
_push_region(rgn);{struct Cyc_List_List*_tmp1C9=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1C2->tvs,_tmp1C0);for(0;_tmp189 != 0;_tmp189=
_tmp189->tl){_tmp1B6->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp189->hd;if(!args_already_checked)Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp1CA=Cyc_Tcexp_tcInjection(te,e1,_tmp1B4,rgn,_tmp1C9,fields);if(_tmp1CA != 0)
_tmp1B6->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1B6->injectors,({struct Cyc_List_List*_tmp1CB=_cycalloc(
sizeof(*_tmp1CB));_tmp1CB->hd=(struct Cyc_Absyn_Tunionfield*)_tmp1CA;_tmp1CB->tl=
0;_tmp1CB;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;_LL13D:({void*_tmp1CC[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp1CD="bad inject vararg type";_tag_dynforward(
_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,23));}),_tag_dynforward(_tmp1CC,
sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp196))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp19B);return _tmp197;}_LL128:;_LL129: return({void*_tmp1CF[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1D0="expected pointer to function";_tag_dynforward(
_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,29));}),_tag_dynforward(_tmp1CF,
sizeof(void*),0));});_LL125:;}_LL123:;_LL124: return({void*_tmp1D1[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1D2="expected pointer to function";_tag_dynforward(
_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,29));}),_tag_dynforward(_tmp1D1,
sizeof(void*),0));});_LL120:;}}static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({
struct Cyc_String_pa_struct _tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1D3[1]={& _tmp1D5};Cyc_Tcutil_terr(
loc,({const char*_tmp1D4="expected xtunion exn but found %s";_tag_dynforward(
_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,34));}),_tag_dynforward(_tmp1D3,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1D6=
_cycalloc(sizeof(*_tmp1D6));_tmp1D6->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1D6;}));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1D7=t1;struct Cyc_Absyn_PtrInfo
_tmp1D8;void*_tmp1D9;struct Cyc_Absyn_Tqual _tmp1DA;struct Cyc_Absyn_PtrAtts _tmp1DB;
void*_tmp1DC;struct Cyc_Absyn_Conref*_tmp1DD;struct Cyc_Absyn_Conref*_tmp1DE;
struct Cyc_Absyn_Conref*_tmp1DF;_LL13F: if(_tmp1D7 <= (void*)4)goto _LL141;if(*((int*)
_tmp1D7)!= 4)goto _LL141;_tmp1D8=((struct Cyc_Absyn_PointerType_struct*)_tmp1D7)->f1;
_tmp1D9=(void*)_tmp1D8.elt_typ;_tmp1DA=_tmp1D8.elt_tq;_tmp1DB=_tmp1D8.ptr_atts;
_tmp1DC=(void*)_tmp1DB.rgn;_tmp1DD=_tmp1DB.nullable;_tmp1DE=_tmp1DB.bounds;
_tmp1DF=_tmp1DB.zero_term;_LL140:{void*_tmp1E0=Cyc_Tcutil_compress(_tmp1D9);
struct Cyc_Absyn_FnInfo _tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_Core_Opt*
_tmp1E3;void*_tmp1E4;struct Cyc_List_List*_tmp1E5;int _tmp1E6;struct Cyc_Absyn_VarargInfo*
_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;_LL144: if(
_tmp1E0 <= (void*)4)goto _LL146;if(*((int*)_tmp1E0)!= 8)goto _LL146;_tmp1E1=((
struct Cyc_Absyn_FnType_struct*)_tmp1E0)->f1;_tmp1E2=_tmp1E1.tvars;_tmp1E3=
_tmp1E1.effect;_tmp1E4=(void*)_tmp1E1.ret_typ;_tmp1E5=_tmp1E1.args;_tmp1E6=
_tmp1E1.c_varargs;_tmp1E7=_tmp1E1.cyc_varargs;_tmp1E8=_tmp1E1.rgn_po;_tmp1E9=
_tmp1E1.attributes;_LL145: {struct Cyc_List_List*instantiation=0;for(0;ts != 0  && 
_tmp1E2 != 0;(ts=ts->tl,_tmp1E2=_tmp1E2->tl)){void*k=Cyc_Tcutil_tvar_kind((struct
Cyc_Absyn_Tvar*)_tmp1E2->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp1EA=_cycalloc(
sizeof(*_tmp1EA));_tmp1EA->hd=({struct _tuple5*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));
_tmp1EB->f1=(struct Cyc_Absyn_Tvar*)_tmp1E2->hd;_tmp1EB->f2=(void*)ts->hd;_tmp1EB;});
_tmp1EA->tl=instantiation;_tmp1EA;});}if(ts != 0)return({void*_tmp1EC[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1ED="too many type variables in instantiation";
_tag_dynforward(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,41));}),
_tag_dynforward(_tmp1EC,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp1F2=_cycalloc(sizeof(*
_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_FnType_struct _tmp1F3;_tmp1F3.tag=8;
_tmp1F3.f1=({struct Cyc_Absyn_FnInfo _tmp1F4;_tmp1F4.tvars=_tmp1E2;_tmp1F4.effect=
_tmp1E3;_tmp1F4.ret_typ=(void*)_tmp1E4;_tmp1F4.args=_tmp1E5;_tmp1F4.c_varargs=
_tmp1E6;_tmp1F4.cyc_varargs=_tmp1E7;_tmp1F4.rgn_po=_tmp1E8;_tmp1F4.attributes=
_tmp1E9;_tmp1F4;});_tmp1F3;});_tmp1F2;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_PointerType_struct
_tmp1EF;_tmp1EF.tag=4;_tmp1EF.f1=({struct Cyc_Absyn_PtrInfo _tmp1F0;_tmp1F0.elt_typ=(
void*)new_fn_typ;_tmp1F0.elt_tq=_tmp1DA;_tmp1F0.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp1F1;_tmp1F1.rgn=(void*)_tmp1DC;_tmp1F1.nullable=_tmp1DD;_tmp1F1.bounds=
_tmp1DE;_tmp1F1.zero_term=_tmp1DF;_tmp1F1.ptrloc=0;_tmp1F1;});_tmp1F0;});_tmp1EF;});
_tmp1EE;});}}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;_LL142: goto
_LL13E;_LL13E:;}return({struct Cyc_String_pa_struct _tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp1F5[1]={& _tmp1F7};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1F6="expecting polymorphic type but found %s";
_tag_dynforward(_tmp1F6,sizeof(char),_get_zero_arr_size(_tmp1F6,40));}),
_tag_dynforward(_tmp1F5,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp1F8=({
struct Cyc_String_pa_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp1F9[2]={& _tmp1FB,& _tmp1FC};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp1FA="cannot cast %s to %s";_tag_dynforward(
_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,21));}),_tag_dynforward(_tmp1F9,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp1F8;}}}}{struct
_tuple0 _tmp1FE=({struct _tuple0 _tmp1FD;_tmp1FD.f1=(void*)e->r;_tmp1FD.f2=Cyc_Tcutil_compress(
t);_tmp1FD;});void*_tmp1FF;struct Cyc_Absyn_MallocInfo _tmp200;int _tmp201;void*
_tmp202;struct Cyc_Absyn_PtrInfo _tmp203;struct Cyc_Absyn_PtrAtts _tmp204;struct Cyc_Absyn_Conref*
_tmp205;struct Cyc_Absyn_Conref*_tmp206;struct Cyc_Absyn_Conref*_tmp207;_LL149:
_tmp1FF=_tmp1FE.f1;if(*((int*)_tmp1FF)!= 35)goto _LL14B;_tmp200=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1FF)->f1;_tmp201=_tmp200.fat_result;_tmp202=_tmp1FE.f2;if(_tmp202 <= (void*)4)
goto _LL14B;if(*((int*)_tmp202)!= 4)goto _LL14B;_tmp203=((struct Cyc_Absyn_PointerType_struct*)
_tmp202)->f1;_tmp204=_tmp203.ptr_atts;_tmp205=_tmp204.nullable;_tmp206=_tmp204.bounds;
_tmp207=_tmp204.zero_term;_LL14A: if((_tmp201  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp207)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp205)){void*_tmp208=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp206);struct Cyc_Absyn_Exp*_tmp209;_LL14E: if(_tmp208 <= (void*)2)goto _LL150;if(*((
int*)_tmp208)!= 0)goto _LL150;_tmp209=((struct Cyc_Absyn_Upper_b_struct*)_tmp208)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp209)).f1 == 1)({void*_tmp20A[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp20B="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp20B,sizeof(char),_get_zero_arr_size(_tmp20B,60));}),
_tag_dynforward(_tmp20A,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp20C=0;struct
Cyc_Absyn_Tqual _tmp20D=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp20E=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp20F;void*_tmp210;struct Cyc_Absyn_Tqual _tmp211;
struct Cyc_Absyn_PtrAtts _tmp212;struct Cyc_Absyn_Conref*_tmp213;_LL153: if(_tmp20E
<= (void*)4)goto _LL155;if(*((int*)_tmp20E)!= 4)goto _LL155;_tmp20F=((struct Cyc_Absyn_PointerType_struct*)
_tmp20E)->f1;_tmp210=(void*)_tmp20F.elt_typ;_tmp211=_tmp20F.elt_tq;_tmp212=
_tmp20F.ptr_atts;_tmp213=_tmp212.zero_term;_LL154: _tmp20C=({void**_tmp214=
_cycalloc(sizeof(*_tmp214));_tmp214[0]=_tmp210;_tmp214;});_tmp20D=_tmp211;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
te),_tmp20C,e);{void*_tmp215=(void*)e->r;struct Cyc_Absyn_Exp*_tmp216;struct Cyc_Absyn_Exp*
_tmp217;_LL158: if(*((int*)_tmp215)!= 25)goto _LL15A;_tmp216=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp215)->f1;_tmp217=((struct Cyc_Absyn_Subscript_e_struct*)_tmp215)->f2;_LL159:{
void*_tmp218=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp216->topt))->v);_LL15D: if(_tmp218 <= (void*)4)goto _LL15F;if(*((int*)_tmp218)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp216,_tmp217,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp216,
_tmp217);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp21A;void*
_tmp21B;struct _tuple6 _tmp219=Cyc_Tcutil_addressof_props(te,e);_tmp21A=_tmp219.f1;
_tmp21B=_tmp219.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp21A){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp21B,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp21E;
_tmp21E.tag=0;_tmp21E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp21C[1]={& _tmp21E};Cyc_Tcutil_terr(loc,({const char*_tmp21D="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,55));}),
_tag_dynforward(_tmp21C,sizeof(void*),1));}});if(topt != 0){void*_tmp21F=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp21F <= (void*)4)goto _LL164;if(*((int*)_tmp21F)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp220=_cycalloc(
sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Absyn_SizeofType_struct _tmp221;_tmp221.tag=
14;_tmp221.f1=(void*)t;_tmp221;});_tmp220;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp222=n;struct _dynforward_ptr*_tmp223;unsigned int _tmp224;_LL167: if(*((int*)
_tmp222)!= 0)goto _LL169;_tmp223=((struct Cyc_Absyn_StructField_struct*)_tmp222)->f1;
_LL168: {int bad_type=1;{void*_tmp225=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp226;void*_tmp227;struct Cyc_Absyn_Aggrdecl**_tmp228;struct Cyc_List_List*
_tmp229;_LL16C: if(_tmp225 <= (void*)4)goto _LL170;if(*((int*)_tmp225)!= 10)goto
_LL16E;_tmp226=((struct Cyc_Absyn_AggrType_struct*)_tmp225)->f1;_tmp227=(void*)
_tmp226.aggr_info;if(*((int*)_tmp227)!= 1)goto _LL16E;_tmp228=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp227)->f1;_LL16D: if((*_tmp228)->impl == 0)goto _LL16B;if(!((int(*)(int(*pred)(
struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp223,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp228)->impl))->fields))({struct
Cyc_String_pa_struct _tmp22C;_tmp22C.tag=0;_tmp22C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp223);{void*_tmp22A[1]={& _tmp22C};Cyc_Tcutil_terr(loc,({
const char*_tmp22B="no field of struct/union has name %s";_tag_dynforward(_tmp22B,
sizeof(char),_get_zero_arr_size(_tmp22B,37));}),_tag_dynforward(_tmp22A,sizeof(
void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((int*)_tmp225)!= 11)goto _LL170;
_tmp229=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp225)->f2;_LL16F: if(!((int(*)(
int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp223,_tmp229))({struct Cyc_String_pa_struct _tmp22F;_tmp22F.tag=0;_tmp22F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp223);{void*_tmp22D[1]={&
_tmp22F};Cyc_Tcutil_terr(loc,({const char*_tmp22E="no field of struct/union has name %s";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,37));}),
_tag_dynforward(_tmp22D,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp232;
_tmp232.tag=0;_tmp232.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp230[1]={& _tmp232};Cyc_Tcutil_terr(loc,({const char*_tmp231="%s is not a known struct/union type";
_tag_dynforward(_tmp231,sizeof(char),_get_zero_arr_size(_tmp231,36));}),
_tag_dynforward(_tmp230,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp222)!= 1)goto _LL166;_tmp224=((struct Cyc_Absyn_TupleIndex_struct*)_tmp222)->f1;
_LL16A: {int bad_type=1;{void*_tmp233=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp234;void*_tmp235;struct Cyc_Absyn_Aggrdecl**_tmp236;struct Cyc_List_List*
_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_TunionFieldInfo _tmp239;void*
_tmp23A;struct Cyc_Absyn_Tunionfield*_tmp23B;_LL173: if(_tmp233 <= (void*)4)goto
_LL17B;if(*((int*)_tmp233)!= 10)goto _LL175;_tmp234=((struct Cyc_Absyn_AggrType_struct*)
_tmp233)->f1;_tmp235=(void*)_tmp234.aggr_info;if(*((int*)_tmp235)!= 1)goto _LL175;
_tmp236=((struct Cyc_Absyn_KnownAggr_struct*)_tmp235)->f1;_LL174: if((*_tmp236)->impl
== 0)goto _LL172;_tmp237=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp236)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp233)!= 11)goto _LL177;_tmp237=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp233)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp237)
<= _tmp224)({struct Cyc_Int_pa_struct _tmp23F;_tmp23F.tag=1;_tmp23F.f1=(
unsigned long)((int)_tmp224);{struct Cyc_Int_pa_struct _tmp23E;_tmp23E.tag=1;
_tmp23E.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp237);{void*_tmp23C[2]={& _tmp23E,& _tmp23F};Cyc_Tcutil_terr(loc,({const char*
_tmp23D="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp23D,
sizeof(char),_get_zero_arr_size(_tmp23D,46));}),_tag_dynforward(_tmp23C,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp233)!= 9)goto _LL179;
_tmp238=((struct Cyc_Absyn_TupleType_struct*)_tmp233)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp238)<= _tmp224)({struct Cyc_Int_pa_struct
_tmp243;_tmp243.tag=1;_tmp243.f1=(unsigned long)((int)_tmp224);{struct Cyc_Int_pa_struct
_tmp242;_tmp242.tag=1;_tmp242.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp238);{void*_tmp240[2]={& _tmp242,& _tmp243};Cyc_Tcutil_terr(
loc,({const char*_tmp241="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,39));}),_tag_dynforward(_tmp240,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp233)!= 3)goto
_LL17B;_tmp239=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp233)->f1;_tmp23A=(
void*)_tmp239.field_info;if(*((int*)_tmp23A)!= 1)goto _LL17B;_tmp23B=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp23A)->f2;_LL17A: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp23B->typs)
< _tmp224)({struct Cyc_Int_pa_struct _tmp247;_tmp247.tag=1;_tmp247.f1=(
unsigned long)((int)_tmp224);{struct Cyc_Int_pa_struct _tmp246;_tmp246.tag=1;
_tmp246.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp23B->typs);{void*_tmp244[2]={& _tmp246,& _tmp247};Cyc_Tcutil_terr(loc,({const
char*_tmp245="tunionfield has too few components: %d < %d";_tag_dynforward(
_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,44));}),_tag_dynforward(_tmp244,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;_LL17C: goto _LL172;_LL172:;}
if(bad_type)({struct Cyc_String_pa_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp248[
1]={& _tmp24A};Cyc_Tcutil_terr(loc,({const char*_tmp249="%s is not a known type";
_tag_dynforward(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,23));}),
_tag_dynforward(_tmp248,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){te=Cyc_Tcenv_clear_notreadctxt(te);Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp24B=t;struct Cyc_Absyn_PtrInfo _tmp24C;void*_tmp24D;struct Cyc_Absyn_PtrAtts
_tmp24E;void*_tmp24F;struct Cyc_Absyn_Conref*_tmp250;struct Cyc_Absyn_Conref*
_tmp251;_LL17E: if(_tmp24B <= (void*)4)goto _LL180;if(*((int*)_tmp24B)!= 4)goto
_LL180;_tmp24C=((struct Cyc_Absyn_PointerType_struct*)_tmp24B)->f1;_tmp24D=(void*)
_tmp24C.elt_typ;_tmp24E=_tmp24C.ptr_atts;_tmp24F=(void*)_tmp24E.rgn;_tmp250=
_tmp24E.bounds;_tmp251=_tmp24E.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp24F);Cyc_Tcutil_check_nonzero_bound(loc,_tmp250);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp24D),(void*)1))({void*_tmp252[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp253="can't dereference abstract pointer type";_tag_dynforward(
_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,40));}),_tag_dynforward(_tmp252,
sizeof(void*),0));});return _tmp24D;_LL180:;_LL181: return({struct Cyc_String_pa_struct
_tmp256;_tmp256.tag=0;_tmp256.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp254[1]={& _tmp256};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp255="expecting * or @ type but found %s";_tag_dynforward(
_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,35));}),_tag_dynforward(_tmp254,
sizeof(void*),1));}});_LL17D:;}}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct
Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*
_tmp257=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp258;void*_tmp259;struct Cyc_Absyn_Aggrdecl**_tmp25A;
struct Cyc_Absyn_Aggrdecl*_tmp25B;struct Cyc_List_List*_tmp25C;void*_tmp25D;struct
Cyc_List_List*_tmp25E;_LL183: if(_tmp257 <= (void*)4)goto _LL18B;if(*((int*)_tmp257)
!= 10)goto _LL185;_tmp258=((struct Cyc_Absyn_AggrType_struct*)_tmp257)->f1;_tmp259=(
void*)_tmp258.aggr_info;if(*((int*)_tmp259)!= 1)goto _LL185;_tmp25A=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp259)->f1;_tmp25B=*_tmp25A;_tmp25C=_tmp258.targs;_LL184: {struct Cyc_Absyn_Aggrfield*
_tmp261=Cyc_Absyn_lookup_decl_field(_tmp25B,f);if(_tmp261 == 0)return({struct Cyc_String_pa_struct
_tmp265;_tmp265.tag=0;_tmp265.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{struct Cyc_String_pa_struct _tmp264;_tmp264.tag=0;_tmp264.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp25B->name));{
void*_tmp262[2]={& _tmp264,& _tmp265};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp263="type %s has no %s field";_tag_dynforward(_tmp263,sizeof(char),
_get_zero_arr_size(_tmp263,24));}),_tag_dynforward(_tmp262,sizeof(void*),2));}}});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25B->impl))->exist_vars != 0)
return({void*_tmp266[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp267="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,64));}),
_tag_dynforward(_tmp266,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp268=_new_region("rgn");struct _RegionHandle*rgn=& _tmp268;_push_region(rgn);{
struct Cyc_List_List*_tmp269=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp25B->tvs,_tmp25C);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp269,(void*)
_tmp261->type);if(((void*)_tmp25B->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp26D=({struct Cyc_String_pa_struct
_tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp26A[1]={& _tmp26C};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp26B="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,56));}),
_tag_dynforward(_tmp26A,sizeof(void*),1));}});_npop_handler(0);return _tmp26D;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp257)!= 11)goto _LL187;_tmp25D=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp257)->f1;_tmp25E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp257)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp26E=Cyc_Absyn_lookup_field(
_tmp25E,f);if(_tmp26E == 0)return({struct Cyc_String_pa_struct _tmp271;_tmp271.tag=
0;_tmp271.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp26F[1]={&
_tmp271};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp270="type has no %s field";
_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,21));}),
_tag_dynforward(_tmp26F,sizeof(void*),1));}});if((_tmp25D == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp26E->type))return({struct Cyc_String_pa_struct
_tmp274;_tmp274.tag=0;_tmp274.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp272[1]={& _tmp274};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp273="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,56));}),
_tag_dynforward(_tmp272,sizeof(void*),1));}});return(void*)_tmp26E->type;}_LL187:
if(*((int*)_tmp257)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp25F="size";_tag_dynforward(_tmp25F,sizeof(char),_get_zero_arr_size(
_tmp25F,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp257)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp260="size";
_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp276;_tmp276.tag=3;_tmp276.f1=(void*)((void*)19);_tmp276.f2=({struct Cyc_List_List*
_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->hd=e;_tmp277->tl=0;_tmp277;});
_tmp276;});_tmp275;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp278="size";_tag_dynforward(_tmp278,
sizeof(char),_get_zero_arr_size(_tmp278,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp27B;_tmp27B.tag=0;_tmp27B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp279[1]={& _tmp27B};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp27A="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,44));}),
_tag_dynforward(_tmp279,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp27E;_tmp27E.tag=0;_tmp27E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp27C[1]={& _tmp27E};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp27D="expecting struct or union, found %s";
_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,36));}),
_tag_dynforward(_tmp27C,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(te),0,e);{
void*_tmp27F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp280;void*_tmp281;struct Cyc_Absyn_PtrAtts _tmp282;void*
_tmp283;struct Cyc_Absyn_Conref*_tmp284;struct Cyc_Absyn_Conref*_tmp285;_LL18E: if(
_tmp27F <= (void*)4)goto _LL190;if(*((int*)_tmp27F)!= 4)goto _LL190;_tmp280=((
struct Cyc_Absyn_PointerType_struct*)_tmp27F)->f1;_tmp281=(void*)_tmp280.elt_typ;
_tmp282=_tmp280.ptr_atts;_tmp283=(void*)_tmp282.rgn;_tmp284=_tmp282.bounds;
_tmp285=_tmp282.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp284);{
void*_tmp286=Cyc_Tcutil_compress(_tmp281);struct Cyc_Absyn_AggrInfo _tmp287;void*
_tmp288;struct Cyc_Absyn_Aggrdecl**_tmp289;struct Cyc_Absyn_Aggrdecl*_tmp28A;
struct Cyc_List_List*_tmp28B;void*_tmp28C;struct Cyc_List_List*_tmp28D;_LL193: if(
_tmp286 <= (void*)4)goto _LL197;if(*((int*)_tmp286)!= 10)goto _LL195;_tmp287=((
struct Cyc_Absyn_AggrType_struct*)_tmp286)->f1;_tmp288=(void*)_tmp287.aggr_info;
if(*((int*)_tmp288)!= 1)goto _LL195;_tmp289=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp288)->f1;_tmp28A=*_tmp289;_tmp28B=_tmp287.targs;_LL194: {struct Cyc_Absyn_Aggrfield*
_tmp28E=Cyc_Absyn_lookup_decl_field(_tmp28A,f);if(_tmp28E == 0)return({struct Cyc_String_pa_struct
_tmp292;_tmp292.tag=0;_tmp292.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{struct Cyc_String_pa_struct _tmp291;_tmp291.tag=0;_tmp291.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp28A->name));{
void*_tmp28F[2]={& _tmp291,& _tmp292};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp290="type %s has no %s field";_tag_dynforward(_tmp290,sizeof(char),
_get_zero_arr_size(_tmp290,24));}),_tag_dynforward(_tmp28F,sizeof(void*),2));}}});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp28A->impl))->exist_vars != 0)
return({void*_tmp293[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp294="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,64));}),
_tag_dynforward(_tmp293,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp295=_new_region("rgn");struct _RegionHandle*rgn=& _tmp295;_push_region(rgn);{
struct Cyc_List_List*_tmp296=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp28A->tvs,_tmp28B);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp296,(void*)
_tmp28E->type);};_pop_region(rgn);}if(((void*)_tmp28A->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp299;
_tmp299.tag=0;_tmp299.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp297[1]={& _tmp299};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp298="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,56));}),
_tag_dynforward(_tmp297,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp286)!= 11)goto _LL197;_tmp28C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp286)->f1;_tmp28D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp286)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp29A=Cyc_Absyn_lookup_field(_tmp28D,f);if(_tmp29A == 
0)return({struct Cyc_String_pa_struct _tmp29D;_tmp29D.tag=0;_tmp29D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp29B[1]={& _tmp29D};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp29C="type has no %s field";_tag_dynforward(_tmp29C,
sizeof(char),_get_zero_arr_size(_tmp29C,21));}),_tag_dynforward(_tmp29B,sizeof(
void*),1));}});if((_tmp28C == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp29A->type))return({struct Cyc_String_pa_struct _tmp2A0;_tmp2A0.tag=0;
_tmp2A0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp29E[1]={&
_tmp2A0};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp29F="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,56));}),
_tag_dynforward(_tmp29E,sizeof(void*),1));}});return(void*)_tmp29A->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2A1[1]={& _tmp2A3};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A2="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,44));}),
_tag_dynforward(_tmp2A1,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2A5;int _tmp2A6;struct _tuple7 _tmp2A4=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;if(!
_tmp2A6)return({void*_tmp2A7[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2A8="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2A8,
sizeof(char),_get_zero_arr_size(_tmp2A8,47));}),_tag_dynforward(_tmp2A7,sizeof(
void*),0));});{struct _handler_cons _tmp2A9;_push_handler(& _tmp2A9);{int _tmp2AB=0;
if(setjmp(_tmp2A9.handler))_tmp2AB=1;if(!_tmp2AB){{void*_tmp2AC=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2A5)).f2;
_npop_handler(0);return _tmp2AC;};_pop_handler();}else{void*_tmp2AA=(void*)
_exn_thrown;void*_tmp2AE=_tmp2AA;_LL19A: if(_tmp2AE != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2B2;_tmp2B2.tag=1;_tmp2B2.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2B1;_tmp2B1.tag=1;_tmp2B1.f1=(unsigned long)((int)_tmp2A5);{void*_tmp2AF[2]={&
_tmp2B1,& _tmp2B2};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2B0="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,41));}),
_tag_dynforward(_tmp2AF,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2AE);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){te=Cyc_Tcenv_clear_notreadctxt(te);Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(
te,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(te,e2))return({struct
Cyc_String_pa_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmp2B3[1]={& _tmp2B5};
Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp2B4="expecting int subscript, found %s";
_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,34));}),
_tag_dynforward(_tmp2B3,sizeof(void*),1));}});{void*_tmp2B6=t1;struct Cyc_Absyn_PtrInfo
_tmp2B7;void*_tmp2B8;struct Cyc_Absyn_Tqual _tmp2B9;struct Cyc_Absyn_PtrAtts _tmp2BA;
void*_tmp2BB;struct Cyc_Absyn_Conref*_tmp2BC;struct Cyc_Absyn_Conref*_tmp2BD;
struct Cyc_List_List*_tmp2BE;_LL19F: if(_tmp2B6 <= (void*)4)goto _LL1A3;if(*((int*)
_tmp2B6)!= 4)goto _LL1A1;_tmp2B7=((struct Cyc_Absyn_PointerType_struct*)_tmp2B6)->f1;
_tmp2B8=(void*)_tmp2B7.elt_typ;_tmp2B9=_tmp2B7.elt_tq;_tmp2BA=_tmp2B7.ptr_atts;
_tmp2BB=(void*)_tmp2BA.rgn;_tmp2BC=_tmp2BA.bounds;_tmp2BD=_tmp2BA.zero_term;
_LL1A0: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2BD)){
int emit_warning=1;{void*_tmp2BF=(void*)(Cyc_Absyn_compress_conref(_tmp2BC))->v;
void*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2C1;void*_tmp2C2;void*_tmp2C3;_LL1A6: if(
_tmp2BF <= (void*)1)goto _LL1AC;if(*((int*)_tmp2BF)!= 0)goto _LL1A8;_tmp2C0=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2BF)->f1;if(_tmp2C0 <= (void*)2)goto _LL1A8;
if(*((int*)_tmp2C0)!= 0)goto _LL1A8;_tmp2C1=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2C0)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(te,e2)){unsigned int _tmp2C5;int
_tmp2C6;struct _tuple7 _tmp2C4=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2C5=_tmp2C4.f1;
_tmp2C6=_tmp2C4.f2;if(_tmp2C6){unsigned int _tmp2C8;int _tmp2C9;struct _tuple7
_tmp2C7=Cyc_Evexp_eval_const_uint_exp(_tmp2C1);_tmp2C8=_tmp2C7.f1;_tmp2C9=
_tmp2C7.f2;if(_tmp2C9  && _tmp2C8 > _tmp2C5)emit_warning=0;}}goto _LL1A5;_LL1A8: if(*((
int*)_tmp2BF)!= 0)goto _LL1AA;_tmp2C2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2BF)->f1;if((int)_tmp2C2 != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;
_LL1AA: if(*((int*)_tmp2BF)!= 0)goto _LL1AC;_tmp2C3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2BF)->f1;if((int)_tmp2C3 != 1)goto _LL1AC;_LL1AB: emit_warning=0;goto _LL1A5;
_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(emit_warning)({void*_tmp2CA[0]={};Cyc_Tcutil_warn(
e2->loc,({const char*_tmp2CB="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,63));}),
_tag_dynforward(_tmp2CA,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(te,
e2)){unsigned int _tmp2CD;int _tmp2CE;struct _tuple7 _tmp2CC=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;if(_tmp2CE)Cyc_Tcutil_check_bound(loc,
_tmp2CD,_tmp2BC);}else{if(Cyc_Tcutil_is_bound_one(_tmp2BC) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2BD))({void*_tmp2CF[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2D0="subscript applied to pointer to one object";
_tag_dynforward(_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,43));}),
_tag_dynforward(_tmp2CF,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2BC);}}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp2BB);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2B8),(void*)1))({void*_tmp2D1[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2D2="can't subscript an abstract pointer";_tag_dynforward(_tmp2D2,
sizeof(char),_get_zero_arr_size(_tmp2D2,36));}),_tag_dynforward(_tmp2D1,sizeof(
void*),0));});return _tmp2B8;_LL1A1: if(*((int*)_tmp2B6)!= 9)goto _LL1A3;_tmp2BE=((
struct Cyc_Absyn_TupleType_struct*)_tmp2B6)->f1;_LL1A2: return Cyc_Tcexp_ithTupleType(
te,loc,_tmp2BE,e2);_LL1A3:;_LL1A4: return({struct Cyc_String_pa_struct _tmp2D5;
_tmp2D5.tag=0;_tmp2D5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp2D3[1]={& _tmp2D5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp2D4="subscript applied to %s";_tag_dynforward(_tmp2D4,sizeof(char),
_get_zero_arr_size(_tmp2D4,24));}),_tag_dynforward(_tmp2D3,sizeof(void*),1));}});
_LL19E:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2D6=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2D7;
_LL1AF: if(_tmp2D6 <= (void*)4)goto _LL1B1;if(*((int*)_tmp2D6)!= 9)goto _LL1B1;
_tmp2D7=((struct Cyc_Absyn_TupleType_struct*)_tmp2D6)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2D7)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2D7=_tmp2D7->tl)){
void*_tmp2D8=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2D7))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2D8,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2D7->hd)).f2);fields=({
struct Cyc_List_List*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9->hd=({struct
_tuple9*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));_tmp2DA->f1=(*((struct _tuple9*)
_tmp2D7->hd)).f1;_tmp2DA->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2DA;});_tmp2D9->tl=fields;_tmp2D9;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB->hd=({struct _tuple9*_tmp2DC=
_cycalloc(sizeof(*_tmp2DC));_tmp2DC->f1=Cyc_Absyn_empty_tqual(0);_tmp2DC->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2DC;});_tmp2DB->tl=fields;_tmp2DB;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2DE;_tmp2DE.tag=9;_tmp2DE.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2DE;});_tmp2DD;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2DF[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp2E0="tcCompoundLit";_tag_dynforward(_tmp2E0,sizeof(char),
_get_zero_arr_size(_tmp2E0,14));}),_tag_dynforward(_tmp2DF,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))Cyc_Core_snd,des);void*
res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp2FB;}));int _tmp2E1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
struct Cyc_Absyn_Const_e_struct*_tmp2E2=({struct Cyc_Absyn_Const_e_struct*_tmp2F7=
_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct Cyc_Absyn_Const_e_struct _tmp2F8;
_tmp2F8.tag=0;_tmp2F8.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp2F9=
_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({struct Cyc_Absyn_Int_c_struct _tmp2FA;
_tmp2FA.tag=2;_tmp2FA.f1=(void*)((void*)1);_tmp2FA.f2=_tmp2E1;_tmp2FA;});_tmp2F9;}));
_tmp2F8;});_tmp2F7;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2E2,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2E3=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2E1 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2E3))({void*_tmp2E4[0]={};Cyc_Tcutil_terr(_tmp2E3->loc,({const char*_tmp2E5="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp2E5,sizeof(char),_get_zero_arr_size(_tmp2E5,45));}),
_tag_dynforward(_tmp2E4,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp2E6=_cycalloc(sizeof(*_tmp2E6));_tmp2E6->v=(void*)Cyc_Absyn_uint_typ;_tmp2E6;});{
void*res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp2E9;_tmp2E9.tag=0;_tmp2E9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp2E7[1]={& _tmp2E9};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp2E8="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp2E8,sizeof(char),_get_zero_arr_size(_tmp2E8,53));}),
_tag_dynforward(_tmp2E7,sizeof(void*),1));}});{int offset=0;for(0;des != 0;(offset
++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 
0){void*_tmp2EA=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp2EB;_LL1B4: if(*((int*)
_tmp2EA)!= 1)goto _LL1B6;_LL1B5:({void*_tmp2EC[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp2ED="only array index designators are supported";_tag_dynforward(
_tmp2ED,sizeof(char),_get_zero_arr_size(_tmp2ED,43));}),_tag_dynforward(_tmp2EC,
sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)_tmp2EA)!= 0)goto _LL1B3;
_tmp2EB=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2EA)->f1;_LL1B7: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp2EB);{unsigned int _tmp2EF;int _tmp2F0;struct _tuple7 _tmp2EE=Cyc_Evexp_eval_const_uint_exp(
_tmp2EB);_tmp2EF=_tmp2EE.f1;_tmp2F0=_tmp2EE.f2;if(!_tmp2F0)({void*_tmp2F1[0]={};
Cyc_Tcutil_terr(_tmp2EB->loc,({const char*_tmp2F2="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp2F2,sizeof(char),_get_zero_arr_size(_tmp2F2,47));}),
_tag_dynforward(_tmp2F1,sizeof(void*),0));});else{if(_tmp2EF != offset)({struct
Cyc_Int_pa_struct _tmp2F6;_tmp2F6.tag=1;_tmp2F6.f1=(unsigned long)((int)_tmp2EF);{
struct Cyc_Int_pa_struct _tmp2F5;_tmp2F5.tag=1;_tmp2F5.f1=(unsigned long)offset;{
void*_tmp2F3[2]={& _tmp2F5,& _tmp2F6};Cyc_Tcutil_terr(_tmp2EB->loc,({const char*
_tmp2F4="expecting index designator %d but found %d";_tag_dynforward(_tmp2F4,
sizeof(char),_get_zero_arr_size(_tmp2F4,43));}),_tag_dynforward(_tmp2F3,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp2FC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp2FC <= (void*)4)goto _LL1BB;if(*((int*)
_tmp2FC)!= 17)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp2FF;_tmp2FF.tag=0;_tmp2FF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp2FD[1]={& _tmp2FF};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp2FE="expecting unsigned int, found %s";
_tag_dynforward(_tmp2FE,sizeof(char),_get_zero_arr_size(_tmp2FE,33));}),
_tag_dynforward(_tmp2FD,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp300[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp301="comprehension index variable is not declared const!";
_tag_dynforward(_tmp301,sizeof(char),_get_zero_arr_size(_tmp301,52));}),
_tag_dynforward(_tmp300,sizeof(void*),0));});if(te->le != 0){te=Cyc_Tcenv_new_block(
loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo pinfo;
void**_tmp302=0;struct Cyc_Absyn_Tqual*_tmp303=0;struct Cyc_Absyn_Conref**_tmp304=
0;if(topt != 0){void*_tmp305=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp306;struct Cyc_Absyn_ArrayInfo _tmp307;void*_tmp308;void**_tmp309;struct Cyc_Absyn_Tqual
_tmp30A;struct Cyc_Absyn_Tqual*_tmp30B;struct Cyc_Absyn_Exp*_tmp30C;struct Cyc_Absyn_Conref*
_tmp30D;struct Cyc_Absyn_Conref**_tmp30E;_LL1BE: if(_tmp305 <= (void*)4)goto _LL1C2;
if(*((int*)_tmp305)!= 4)goto _LL1C0;_tmp306=((struct Cyc_Absyn_PointerType_struct*)
_tmp305)->f1;_LL1BF: pinfo=_tmp306;_tmp302=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp303=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp304=(struct Cyc_Absyn_Conref**)&(
pinfo.ptr_atts).zero_term;goto _LL1BD;_LL1C0: if(*((int*)_tmp305)!= 7)goto _LL1C2;
_tmp307=((struct Cyc_Absyn_ArrayType_struct*)_tmp305)->f1;_tmp308=(void*)_tmp307.elt_type;
_tmp309=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp305)->f1).elt_type;
_tmp30A=_tmp307.tq;_tmp30B=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp305)->f1).tq;_tmp30C=_tmp307.num_elts;_tmp30D=_tmp307.zero_term;_tmp30E=(
struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp305)->f1).zero_term;
_LL1C1: _tmp302=(void**)_tmp309;_tmp303=(struct Cyc_Absyn_Tqual*)_tmp30B;_tmp304=(
struct Cyc_Absyn_Conref**)_tmp30E;goto _LL1BD;_LL1C2:;_LL1C3: goto _LL1BD;_LL1BD:;}{
void*t=Cyc_Tcexp_tcExp(te,_tmp302,body);if(te->le == 0){if(!Cyc_Tcutil_is_const_exp(
te,bound))({void*_tmp30F[0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp310="bound is not constant";
_tag_dynforward(_tmp310,sizeof(char),_get_zero_arr_size(_tmp310,22));}),
_tag_dynforward(_tmp30F,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(te,body))({
void*_tmp311[0]={};Cyc_Tcutil_terr(bound->loc,({const char*_tmp312="body is not constant";
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,21));}),
_tag_dynforward(_tmp311,sizeof(void*),0));});}if(_tmp304 != 0  && ((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,*_tmp304)){struct Cyc_Absyn_Exp*
_tmp313=Cyc_Absyn_uint_exp(1,0);_tmp313->topt=({struct Cyc_Core_Opt*_tmp314=
_cycalloc(sizeof(*_tmp314));_tmp314->v=(void*)Cyc_Absyn_uint_typ;_tmp314;});
bound=Cyc_Absyn_add_exp(bound,_tmp313,0);bound->topt=({struct Cyc_Core_Opt*
_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->v=(void*)Cyc_Absyn_uint_typ;_tmp315;});*
is_zero_term=1;}return Cyc_Absyn_array_typ(t,_tmp303 == 0?Cyc_Absyn_empty_tqual(0):*
_tmp303,(struct Cyc_Absyn_Exp*)bound,_tmp304 == 0?Cyc_Absyn_false_conref:*_tmp304,
0);}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*
args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 
0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{struct _handler_cons
_tmp316;_push_handler(& _tmp316);{int _tmp318=0;if(setjmp(_tmp316.handler))_tmp318=
1;if(!_tmp318){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{
void*_tmp317=(void*)_exn_thrown;void*_tmp31A=_tmp317;_LL1C5: if(_tmp31A != Cyc_Dict_Absent)
goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct _tmp31D;_tmp31D.tag=0;_tmp31D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{
void*_tmp31B[1]={& _tmp31D};Cyc_Tcutil_terr(loc,({const char*_tmp31C="unbound struct name %s";
_tag_dynforward(_tmp31C,sizeof(char),_get_zero_arr_size(_tmp31C,23));}),
_tag_dynforward(_tmp31B,sizeof(void*),1));}});return topt != 0?*topt:(void*)0;
_LL1C7:;_LL1C8:(void)_throw(_tmp31A);_LL1C4:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp31E[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp31F="expecting struct but found union";_tag_dynforward(
_tmp31F,sizeof(char),_get_zero_arr_size(_tmp31F,33));}),_tag_dynforward(_tmp31E,
sizeof(void*),0));});if(ad->impl == 0){({void*_tmp320[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp321="can't build abstract struct";_tag_dynforward(_tmp321,sizeof(
char),_get_zero_arr_size(_tmp321,28));}),_tag_dynforward(_tmp320,sizeof(void*),0));});
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp322=_cycalloc(sizeof(*_tmp322));
_tmp322->v=Cyc_Tcenv_lookup_type_vars(te);_tmp322;}));}{struct _RegionHandle
_tmp323=_new_region("rgn");struct _RegionHandle*rgn=& _tmp323;_push_region(rgn);{
struct _tuple4 _tmp324=({struct _tuple4 _tmp345;_tmp345.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp345.f2=rgn;_tmp345;});struct Cyc_List_List*_tmp325=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp324,ad->tvs);struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp324,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp327=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp325);struct Cyc_List_List*_tmp328=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp326);struct Cyc_List_List*_tmp329=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp325,_tmp326);struct Cyc_Absyn_AggrType_struct*_tmp32A=({struct Cyc_Absyn_AggrType_struct*
_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F[0]=({struct Cyc_Absyn_AggrType_struct
_tmp340;_tmp340.tag=10;_tmp340.f1=({struct Cyc_Absyn_AggrInfo _tmp341;_tmp341.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp342=_cycalloc(sizeof(*
_tmp342));_tmp342[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp343;_tmp343.tag=1;
_tmp343.f1=({struct Cyc_Absyn_Aggrdecl**_tmp344=_cycalloc(sizeof(*_tmp344));
_tmp344[0]=ad;_tmp344;});_tmp343;});_tmp342;}));_tmp341.targs=_tmp327;_tmp341;});
_tmp340;});_tmp33F;});struct Cyc_List_List*_tmp32B=*ts;struct Cyc_List_List*
_tmp32C=_tmp328;while(_tmp32B != 0  && _tmp32C != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp32B->hd);Cyc_Tcutil_unify((
void*)_tmp32B->hd,(void*)_tmp32C->hd);_tmp32B=_tmp32B->tl;_tmp32C=_tmp32C->tl;}
if(_tmp32B != 0)({void*_tmp32D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp32E="too many explicit witness types";
_tag_dynforward(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,32));}),
_tag_dynforward(_tmp32D,sizeof(void*),0));});*ts=_tmp328;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp32A,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp330;struct Cyc_Absyn_Aggrfield*_tmp331;
struct Cyc_Absyn_Exp*_tmp332;struct _tuple10*_tmp32F=(struct _tuple10*)fields->hd;
_tmp330=*_tmp32F;_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;{void*_tmp333=Cyc_Tcutil_rsubstitute(
rgn,_tmp329,(void*)_tmp331->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp333,
_tmp332);if(!Cyc_Tcutil_coerce_arg(te,_tmp332,_tmp333)){({struct Cyc_String_pa_struct
_tmp339;_tmp339.tag=0;_tmp339.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp332->topt))->v));{
struct Cyc_String_pa_struct _tmp338;_tmp338.tag=0;_tmp338.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp333));{struct
Cyc_String_pa_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp336;_tmp336.tag=0;_tmp336.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp331->name);{void*_tmp334[4]={& _tmp336,& _tmp337,& _tmp338,& _tmp339};Cyc_Tcutil_terr(
_tmp332->loc,({const char*_tmp335="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp335,sizeof(char),_get_zero_arr_size(_tmp335,44));}),
_tag_dynforward(_tmp334,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp33A=0;{struct Cyc_List_List*_tmp33B=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp33B != 0;_tmp33B=_tmp33B->tl){_tmp33A=({
struct Cyc_List_List*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->hd=({struct
_tuple0*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp329,(*((struct _tuple0*)_tmp33B->hd)).f1);_tmp33D->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp329,(*((struct _tuple0*)_tmp33B->hd)).f2);_tmp33D;});_tmp33C->tl=_tmp33A;
_tmp33C;});}}_tmp33A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp33A);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp33A);{void*_tmp33E=(void*)
_tmp32A;_npop_handler(0);return _tmp33E;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp346=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp346;_push_region(rgn);{void*_tmp347=Cyc_Tcutil_compress(ts);void*_tmp348;
struct Cyc_List_List*_tmp349;_LL1CA: if(_tmp347 <= (void*)4)goto _LL1CC;if(*((int*)
_tmp347)!= 11)goto _LL1CC;_tmp348=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp347)->f1;_tmp349=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp347)->f2;_LL1CB:
if(_tmp348 == (void*)1)({void*_tmp34A[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp34B="expecting struct but found union";_tag_dynforward(_tmp34B,sizeof(char),
_get_zero_arr_size(_tmp34B,33));}),_tag_dynforward(_tmp34A,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp349);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp34D;struct Cyc_Absyn_Aggrfield*
_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;struct _tuple10*_tmp34C=(struct _tuple10*)
fields->hd;_tmp34D=*_tmp34C;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp34E->type)),_tmp34F);if(!Cyc_Tcutil_coerce_arg(
te,_tmp34F,(void*)_tmp34E->type)){({struct Cyc_String_pa_struct _tmp354;_tmp354.tag=
0;_tmp354.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp34F->topt))->v));{struct Cyc_String_pa_struct
_tmp353;_tmp353.tag=0;_tmp353.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp34E->type));{struct Cyc_String_pa_struct _tmp352;
_tmp352.tag=0;_tmp352.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp34E->name);{
void*_tmp350[3]={& _tmp352,& _tmp353,& _tmp354};Cyc_Tcutil_terr(_tmp34F->loc,({
const char*_tmp351="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,41));}),_tag_dynforward(_tmp350,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp355[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp356="tcAnonStruct: wrong type";
_tag_dynforward(_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,25));}),
_tag_dynforward(_tmp355,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp357=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp357;_push_region(rgn);{struct _tuple4 _tmp358=({
struct _tuple4 _tmp37B;_tmp37B.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp37B.f2=rgn;
_tmp37B;});struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp358,tud->tvs);struct Cyc_List_List*_tmp35A=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp359);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp377;_tmp377.tag=3;_tmp377.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp378;
_tmp378.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp37A;_tmp37A.tag=1;_tmp37A.f1=tud;_tmp37A.f2=tuf;_tmp37A;});_tmp379;}));
_tmp378.targs=_tmp35A;_tmp378;});_tmp377;});_tmp376;});if(topt != 0){void*_tmp35B=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp35C;struct Cyc_List_List*
_tmp35D;void*_tmp35E;_LL1CF: if(_tmp35B <= (void*)4)goto _LL1D3;if(*((int*)_tmp35B)
!= 3)goto _LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;_LL1D1: if(*((int*)
_tmp35B)!= 2)goto _LL1D3;_tmp35C=((struct Cyc_Absyn_TunionType_struct*)_tmp35B)->f1;
_tmp35D=_tmp35C.targs;_tmp35E=(void*)_tmp35C.rgn;_LL1D2:{struct Cyc_List_List*a=
_tmp35A;for(0;a != 0  && _tmp35D != 0;(a=a->tl,_tmp35D=_tmp35D->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp35D->hd);}}if(tuf->typs == 0  && es == 0){e->topt=({struct
Cyc_Core_Opt*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->v=(void*)res;_tmp35F;});
res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp360=_cycalloc(sizeof(*_tmp360));
_tmp360[0]=({struct Cyc_Absyn_TunionType_struct _tmp361;_tmp361.tag=2;_tmp361.f1=({
struct Cyc_Absyn_TunionInfo _tmp362;_tmp362.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp364;_tmp364.tag=1;_tmp364.f1=({struct Cyc_Absyn_Tuniondecl**_tmp365=_cycalloc(
sizeof(*_tmp365));_tmp365[0]=tud;_tmp365;});_tmp364;});_tmp363;}));_tmp362.targs=
_tmp35A;_tmp362.rgn=(void*)_tmp35E;_tmp362;});_tmp361;});_tmp360;});Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);{void*_tmp366=res;_npop_handler(0);return _tmp366;}}goto _LL1CE;
_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp359,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp36B;_tmp36B.tag=0;_tmp36B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp36C="?";_tag_dynforward(_tmp36C,sizeof(char),_get_zero_arr_size(
_tmp36C,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp36A;_tmp36A.tag=0;_tmp36A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp369;_tmp369.tag=0;_tmp369.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp367[3]={& _tmp369,& _tmp36A,& _tmp36B};
Cyc_Tcutil_terr(e->loc,({const char*_tmp368="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp368,sizeof(char),_get_zero_arr_size(_tmp368,80));}),
_tag_dynforward(_tmp367,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp370=({struct Cyc_String_pa_struct _tmp36F;_tmp36F.tag=0;
_tmp36F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp36D[1]={& _tmp36F};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp36E="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp36E,sizeof(char),_get_zero_arr_size(_tmp36E,45));}),
_tag_dynforward(_tmp36D,sizeof(void*),1));}});_npop_handler(0);return _tmp370;}
if(ts != 0){void*_tmp374=({struct Cyc_String_pa_struct _tmp373;_tmp373.tag=0;
_tmp373.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp371[1]={& _tmp373};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp372="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp372,sizeof(char),_get_zero_arr_size(_tmp372,44));}),_tag_dynforward(_tmp371,
sizeof(void*),1));}});_npop_handler(0);return _tmp374;}{void*_tmp375=res;
_npop_handler(0);return _tmp375;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp37C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp37D;
struct Cyc_Absyn_PtrAtts _tmp37E;struct Cyc_Absyn_Conref*_tmp37F;struct Cyc_Absyn_ArrayInfo
_tmp380;void*_tmp381;struct Cyc_List_List*_tmp382;struct Cyc_Absyn_AggrInfo _tmp383;
void*_tmp384;struct Cyc_List_List*_tmp385;struct Cyc_List_List*_tmp386;_LL1D6: if((
int)_tmp37C != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp37C <= (void*)4)goto
_LL1E4;if(*((int*)_tmp37C)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA: if(*((int*)
_tmp37C)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp37C)!= 2)goto
_LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp37C)!= 3)goto _LL1E0;_LL1DF: return
0;_LL1E0: if(*((int*)_tmp37C)!= 4)goto _LL1E2;_tmp37D=((struct Cyc_Absyn_PointerType_struct*)
_tmp37C)->f1;_tmp37E=_tmp37D.ptr_atts;_tmp37F=_tmp37E.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp37F);_LL1E2: if(*((
int*)_tmp37C)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp37C != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp37C <= (void*)4)goto _LL200;if(*((int*)
_tmp37C)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp37C)!= 7)goto _LL1EA;
_tmp380=((struct Cyc_Absyn_ArrayType_struct*)_tmp37C)->f1;_tmp381=(void*)_tmp380.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp381);_LL1EA: if(*((int*)_tmp37C)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp37C)!= 9)goto _LL1EE;_tmp382=((
struct Cyc_Absyn_TupleType_struct*)_tmp37C)->f1;_LL1ED: for(0;(unsigned int)
_tmp382;_tmp382=_tmp382->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp382->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp37C)!= 10)goto _LL1F0;
_tmp383=((struct Cyc_Absyn_AggrType_struct*)_tmp37C)->f1;_tmp384=(void*)_tmp383.aggr_info;
_tmp385=_tmp383.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp387=Cyc_Absyn_get_known_aggrdecl(
_tmp384);if(_tmp387->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp387->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp388=_new_region("r");
struct _RegionHandle*r=& _tmp388;_push_region(r);{struct Cyc_List_List*_tmp389=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp387->tvs,_tmp385);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp387->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp389,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp38A=0;_npop_handler(0);
return _tmp38A;}}}{int _tmp38B=1;_npop_handler(0);return _tmp38B;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp37C)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp37C)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp37C)!= 17)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp37C)!= 11)goto _LL1F8;_tmp386=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp37C)->f2;_LL1F7: for(0;_tmp386 != 0;
_tmp386=_tmp386->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp386->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp37C)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp37C)!= 16)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp37C)!= 15)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp37C)!= 
18)goto _LL200;_LL1FF: goto _LL201;_LL200: if((int)_tmp37C != 2)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp37C != 3)goto _LL204;_LL203: goto _LL205;_LL204: if(
_tmp37C <= (void*)4)goto _LL206;if(*((int*)_tmp37C)!= 19)goto _LL206;_LL205: goto
_LL207;_LL206: if(_tmp37C <= (void*)4)goto _LL208;if(*((int*)_tmp37C)!= 20)goto
_LL208;_LL207: goto _LL209;_LL208: if(_tmp37C <= (void*)4)goto _LL1D5;if(*((int*)
_tmp37C)!= 21)goto _LL1D5;_LL209:({struct Cyc_String_pa_struct _tmp38E;_tmp38E.tag=
0;_tmp38E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp38C[1]={& _tmp38E};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp38D="bad type `%s' in zeroable type";
_tag_dynforward(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,31));}),
_tag_dynforward(_tmp38C,sizeof(void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp38F=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp390;void*_tmp391;_LL20B:
if(_tmp38F <= (void*)4)goto _LL20D;if(*((int*)_tmp38F)!= 4)goto _LL20D;_tmp390=((
struct Cyc_Absyn_PointerType_struct*)_tmp38F)->f1;_tmp391=(void*)_tmp390.elt_typ;
_LL20C: Cyc_Tcutil_unify(_tmp391,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL20A;_LL20D:;_LL20E: goto _LL20A;_LL20A:;}({
struct Cyc_String_pa_struct _tmp395;_tmp395.tag=0;_tmp395.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp394;_tmp394.tag=0;_tmp394.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(
allow_zero?(struct _dynforward_ptr)({const char*_tmp396="calloc";_tag_dynforward(
_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,7));}):(struct _dynforward_ptr)({
const char*_tmp397="malloc";_tag_dynforward(_tmp397,sizeof(char),
_get_zero_arr_size(_tmp397,7));})));{void*_tmp392[2]={& _tmp394,& _tmp395};Cyc_Tcutil_terr(
loc,({const char*_tmp393="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,60));}),
_tag_dynforward(_tmp392,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp39E;_tmp39E.tag=15;_tmp39E.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp39F;}));_tmp39E;});_tmp39D;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp398=Cyc_Tcutil_compress(handle_type);void*_tmp399;_LL210: if(_tmp398 <= (
void*)4)goto _LL212;if(*((int*)_tmp398)!= 15)goto _LL212;_tmp399=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp398)->f1;_LL211: rgn=_tmp399;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL20F;_LL212:;_LL213:({struct Cyc_String_pa_struct _tmp39C;
_tmp39C.tag=0;_tmp39C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp39A[1]={& _tmp39C};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp39B="expecting region_t type but found %s";_tag_dynforward(_tmp39B,sizeof(
char),_get_zero_arr_size(_tmp39B,37));}),_tag_dynforward(_tmp39A,sizeof(void*),1));}});
goto _LL20F;_LL20F:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3A0[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3A1="calloc with empty type";_tag_dynforward(_tmp3A1,sizeof(char),
_get_zero_arr_size(_tmp3A1,23));}),_tag_dynforward(_tmp3A0,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3A2=(void*)(*e)->r;void*_tmp3A3;void*
_tmp3A4;struct Cyc_List_List*_tmp3A5;struct Cyc_List_List _tmp3A6;struct Cyc_Absyn_Exp*
_tmp3A7;struct Cyc_List_List*_tmp3A8;struct Cyc_List_List _tmp3A9;struct Cyc_Absyn_Exp*
_tmp3AA;struct Cyc_List_List*_tmp3AB;_LL215: if(*((int*)_tmp3A2)!= 18)goto _LL217;
_tmp3A3=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3A2)->f1;_LL216:
elt_type=_tmp3A3;*t=(void**)({void**_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC[
0]=elt_type;_tmp3AC;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL214;_LL217: if(*((int*)_tmp3A2)!= 3)
goto _LL219;_tmp3A4=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3A2)->f1;if((
int)_tmp3A4 != 1)goto _LL219;_tmp3A5=((struct Cyc_Absyn_Primop_e_struct*)_tmp3A2)->f2;
if(_tmp3A5 == 0)goto _LL219;_tmp3A6=*_tmp3A5;_tmp3A7=(struct Cyc_Absyn_Exp*)_tmp3A6.hd;
_tmp3A8=_tmp3A6.tl;if(_tmp3A8 == 0)goto _LL219;_tmp3A9=*_tmp3A8;_tmp3AA=(struct Cyc_Absyn_Exp*)
_tmp3A9.hd;_tmp3AB=_tmp3A9.tl;if(_tmp3AB != 0)goto _LL219;_LL218:{struct _tuple0
_tmp3AE=({struct _tuple0 _tmp3AD;_tmp3AD.f1=(void*)_tmp3A7->r;_tmp3AD.f2=(void*)
_tmp3AA->r;_tmp3AD;});void*_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;_LL21C:
_tmp3AF=_tmp3AE.f1;if(*((int*)_tmp3AF)!= 18)goto _LL21E;_tmp3B0=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3AF)->f1;_LL21D: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3B0);elt_type=_tmp3B0;*t=(void**)({void**_tmp3B3=_cycalloc(sizeof(*
_tmp3B3));_tmp3B3[0]=elt_type;_tmp3B3;});num_elts=_tmp3AA;one_elt=0;goto _LL21B;
_LL21E: _tmp3B1=_tmp3AE.f2;if(*((int*)_tmp3B1)!= 18)goto _LL220;_tmp3B2=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B1)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3B2);elt_type=_tmp3B2;*t=(void**)({void**_tmp3B4=_cycalloc(
sizeof(*_tmp3B4));_tmp3B4[0]=elt_type;_tmp3B4;});num_elts=_tmp3A7;one_elt=0;goto
_LL21B;_LL220:;_LL221: goto No_sizeof;_LL21B:;}goto _LL214;_LL219:;_LL21A: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3B5=_cycalloc(sizeof(*_tmp3B5));
_tmp3B5[0]=elt_type;_tmp3B5;});num_elts=*e;one_elt=0;goto _LL214;_LL214:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3B6=elt_type;struct Cyc_Absyn_AggrInfo _tmp3B7;
void*_tmp3B8;struct Cyc_Absyn_Aggrdecl**_tmp3B9;struct Cyc_Absyn_Aggrdecl*_tmp3BA;
_LL223: if(_tmp3B6 <= (void*)4)goto _LL225;if(*((int*)_tmp3B6)!= 10)goto _LL225;
_tmp3B7=((struct Cyc_Absyn_AggrType_struct*)_tmp3B6)->f1;_tmp3B8=(void*)_tmp3B7.aggr_info;
if(*((int*)_tmp3B8)!= 1)goto _LL225;_tmp3B9=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp3B8)->f1;_tmp3BA=*_tmp3B9;_LL224: if(_tmp3BA->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3BA->impl))->exist_vars != 0)({void*_tmp3BB[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp3BC="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3BC,sizeof(char),_get_zero_arr_size(_tmp3BC,50));}),
_tag_dynforward(_tmp3BB,sizeof(void*),0));});goto _LL222;_LL225:;_LL226: goto
_LL222;_LL222:;}{void*(*_tmp3BD)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3BE=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3BF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3C0;struct Cyc_Absyn_PtrAtts _tmp3C1;struct Cyc_Absyn_Conref*_tmp3C2;struct Cyc_Absyn_Conref*
_tmp3C3;_LL228: if(_tmp3BF <= (void*)4)goto _LL22A;if(*((int*)_tmp3BF)!= 4)goto
_LL22A;_tmp3C0=((struct Cyc_Absyn_PointerType_struct*)_tmp3BF)->f1;_tmp3C1=
_tmp3C0.ptr_atts;_tmp3C2=_tmp3C1.nullable;_tmp3C3=_tmp3C1.zero_term;_LL229:
_tmp3BE=_tmp3C3;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3C2))_tmp3BD=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3C3) && !(*is_calloc)){({void*_tmp3C4[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3C5="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3C5,sizeof(char),_get_zero_arr_size(_tmp3C5,55));}),
_tag_dynforward(_tmp3C4,sizeof(void*),0));});*is_calloc=1;}goto _LL227;_LL22A:;
_LL22B: goto _LL227;_LL227:;}if(!one_elt)_tmp3BD=Cyc_Absyn_dynforward_typ;return
_tmp3BD(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3BE);}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(te,s),s,1);Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp3C6=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;struct Cyc_Absyn_Decl*
_tmp3CA;struct Cyc_Absyn_Stmt*_tmp3CB;_LL22D: if(_tmp3C6 <= (void*)1)goto _LL233;if(*((
int*)_tmp3C6)!= 0)goto _LL22F;_tmp3C7=((struct Cyc_Absyn_Exp_s_struct*)_tmp3C6)->f1;
_LL22E: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3C7->topt))->v;_LL22F:
if(*((int*)_tmp3C6)!= 1)goto _LL231;_tmp3C8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3C6)->f1;_tmp3C9=((struct Cyc_Absyn_Seq_s_struct*)_tmp3C6)->f2;_LL230: s=
_tmp3C9;continue;_LL231: if(*((int*)_tmp3C6)!= 11)goto _LL233;_tmp3CA=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3C6)->f1;_tmp3CB=((struct Cyc_Absyn_Decl_s_struct*)_tmp3C6)->f2;_LL232: s=
_tmp3CB;continue;_LL233:;_LL234: return({void*_tmp3CC[0]={};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp3CD="statement expression must end with expression";
_tag_dynforward(_tmp3CD,sizeof(char),_get_zero_arr_size(_tmp3CD,46));}),
_tag_dynforward(_tmp3CC,sizeof(void*),0));});_LL22C:;}}static void*Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(
rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3D4;_tmp3D4.tag=15;_tmp3D4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3D5;}));_tmp3D4;});_tmp3D3;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp3CE=Cyc_Tcutil_compress(handle_type);void*_tmp3CF;_LL236:
if(_tmp3CE <= (void*)4)goto _LL238;if(*((int*)_tmp3CE)!= 15)goto _LL238;_tmp3CF=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3CE)->f1;_LL237: rgn=_tmp3CF;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL235;_LL238:;_LL239:({struct Cyc_String_pa_struct
_tmp3D2;_tmp3D2.tag=0;_tmp3D2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp3D0[1]={& _tmp3D2};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp3D1="expecting region_t type but found %s";
_tag_dynforward(_tmp3D1,sizeof(char),_get_zero_arr_size(_tmp3D1,37));}),
_tag_dynforward(_tmp3D0,sizeof(void*),1));}});goto _LL235;_LL235:;}{void*_tmp3D6=(
void*)e1->r;struct Cyc_Core_Opt*_tmp3D7;struct Cyc_List_List*_tmp3D8;struct Cyc_List_List*
_tmp3D9;void*_tmp3DA;struct _dynforward_ptr _tmp3DB;_LL23B: if(*((int*)_tmp3D6)!= 
29)goto _LL23D;_LL23C: {void*_tmp3DC=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*
_tmp3DD=Cyc_Tcutil_compress(_tmp3DC);struct Cyc_Absyn_ArrayInfo _tmp3DE;void*
_tmp3DF;struct Cyc_Absyn_Tqual _tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Conref*
_tmp3E2;_LL246: if(_tmp3DD <= (void*)4)goto _LL248;if(*((int*)_tmp3DD)!= 7)goto
_LL248;_tmp3DE=((struct Cyc_Absyn_ArrayType_struct*)_tmp3DD)->f1;_tmp3DF=(void*)
_tmp3DE.elt_type;_tmp3E0=_tmp3DE.tq;_tmp3E1=_tmp3DE.num_elts;_tmp3E2=_tmp3DE.zero_term;
_LL247: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp3E1);void*
b;{void*_tmp3E3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp3E4;_LL24B: if(_tmp3E3 <= (void*)4)goto _LL24D;if(*((int*)_tmp3E3)!= 17)
goto _LL24D;_tmp3E4=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp3E3)->f1;_LL24C:
b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));
_tmp3E5[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp3E6;_tmp3E6.tag=1;_tmp3E6.f1=(
void*)_tmp3E4;_tmp3E6;});_tmp3E5;});goto _LL24A;_LL24D:;_LL24E: if(Cyc_Tcutil_is_const_exp(
te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp3E7=_cycalloc(sizeof(*
_tmp3E7));_tmp3E7[0]=({struct Cyc_Absyn_Upper_b_struct _tmp3E8;_tmp3E8.tag=0;
_tmp3E8.f1=bnd;_tmp3E8;});_tmp3E7;});else{b=(void*)0;}_LL24A:;}{void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));
_tmp3EA[0]=({struct Cyc_Absyn_PointerType_struct _tmp3EB;_tmp3EB.tag=4;_tmp3EB.f1=({
struct Cyc_Absyn_PtrInfo _tmp3EC;_tmp3EC.elt_typ=(void*)_tmp3DF;_tmp3EC.elt_tq=
_tmp3E0;_tmp3EC.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp3ED;_tmp3ED.rgn=(void*)rgn;
_tmp3ED.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp3ED.bounds=Cyc_Absyn_new_conref(b);_tmp3ED.zero_term=_tmp3E2;_tmp3ED.ptrloc=
0;_tmp3ED;});_tmp3EC;});_tmp3EB;});_tmp3EA;});if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({
struct Cyc_Core_Opt*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->v=(void*)res_typ;
_tmp3E9;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*topt;}}return
res_typ;}}_LL248:;_LL249:({void*_tmp3EE[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3EF="tcNew: comprehension returned non-array type";
_tag_dynforward(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,45));}),
_tag_dynforward(_tmp3EE,sizeof(void*),0));});_LL245:;}_LL23D: if(*((int*)_tmp3D6)
!= 36)goto _LL23F;_tmp3D7=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3D6)->f1;
_tmp3D8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3D6)->f2;_LL23E:(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));
_tmp3F0[0]=({struct Cyc_Absyn_Array_e_struct _tmp3F1;_tmp3F1.tag=28;_tmp3F1.f1=
_tmp3D8;_tmp3F1;});_tmp3F0;})));_tmp3D9=_tmp3D8;goto _LL240;_LL23F: if(*((int*)
_tmp3D6)!= 28)goto _LL241;_tmp3D9=((struct Cyc_Absyn_Array_e_struct*)_tmp3D6)->f1;
_LL240: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp3F2=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3F3;void*_tmp3F4;void**_tmp3F5;struct Cyc_Absyn_Tqual
_tmp3F6;struct Cyc_Absyn_PtrAtts _tmp3F7;struct Cyc_Absyn_Conref*_tmp3F8;_LL250: if(
_tmp3F2 <= (void*)4)goto _LL252;if(*((int*)_tmp3F2)!= 4)goto _LL252;_tmp3F3=((
struct Cyc_Absyn_PointerType_struct*)_tmp3F2)->f1;_tmp3F4=(void*)_tmp3F3.elt_typ;
_tmp3F5=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp3F2)->f1).elt_typ;
_tmp3F6=_tmp3F3.elt_tq;_tmp3F7=_tmp3F3.ptr_atts;_tmp3F8=_tmp3F7.zero_term;_LL251:
elt_typ_opt=(void**)_tmp3F5;zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3F8);goto _LL24F;_LL252:;_LL253: goto _LL24F;_LL24F:;}{void*_tmp3F9=Cyc_Tcexp_tcArray(
te,e1->loc,elt_typ_opt,zero_term,_tmp3D9);e1->topt=({struct Cyc_Core_Opt*_tmp3FA=
_cycalloc(sizeof(*_tmp3FA));_tmp3FA->v=(void*)_tmp3F9;_tmp3FA;});{void*res_typ;{
void*_tmp3FB=Cyc_Tcutil_compress(_tmp3F9);struct Cyc_Absyn_ArrayInfo _tmp3FC;void*
_tmp3FD;struct Cyc_Absyn_Tqual _tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Conref*
_tmp400;_LL255: if(_tmp3FB <= (void*)4)goto _LL257;if(*((int*)_tmp3FB)!= 7)goto
_LL257;_tmp3FC=((struct Cyc_Absyn_ArrayType_struct*)_tmp3FB)->f1;_tmp3FD=(void*)
_tmp3FC.elt_type;_tmp3FE=_tmp3FC.tq;_tmp3FF=_tmp3FC.num_elts;_tmp400=_tmp3FC.zero_term;
_LL256: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp401=_cycalloc(
sizeof(*_tmp401));_tmp401[0]=({struct Cyc_Absyn_PointerType_struct _tmp402;_tmp402.tag=
4;_tmp402.f1=({struct Cyc_Absyn_PtrInfo _tmp403;_tmp403.elt_typ=(void*)_tmp3FD;
_tmp403.elt_tq=_tmp3FE;_tmp403.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp404;_tmp404.rgn=(
void*)rgn;_tmp404.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp404.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp406;_tmp406.tag=0;_tmp406.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp3FF);
_tmp406;});_tmp405;}));_tmp404.zero_term=_tmp400;_tmp404.ptrloc=0;_tmp404;});
_tmp403;});_tmp402;});_tmp401;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->v=(void*)res_typ;_tmp407;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL254;_LL257:;_LL258:({void*_tmp408[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp409="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,50));}),
_tag_dynforward(_tmp408,sizeof(void*),0));});_LL254:;}return res_typ;}}}_LL241:
if(*((int*)_tmp3D6)!= 0)goto _LL243;_tmp3DA=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp3D6)->f1;if(_tmp3DA <= (void*)1)goto _LL243;if(*((int*)_tmp3DA)!= 5)goto _LL243;
_tmp3DB=((struct Cyc_Absyn_String_c_struct*)_tmp3DA)->f1;_LL242: {void*_tmp40A=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp40B=Cyc_Tcexp_tcExp(te,(void**)& _tmp40A,e1);return Cyc_Absyn_atb_typ(
_tmp40B,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp40D;_tmp40D.tag=0;_tmp40D.f1=Cyc_Absyn_uint_exp(1,0);_tmp40D;});_tmp40C;}),
Cyc_Absyn_false_conref);}_LL243:;_LL244: {void**topt2=0;if(topt != 0){void*
_tmp40E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp40F;void*_tmp410;
void**_tmp411;struct Cyc_Absyn_Tqual _tmp412;_LL25A: if(_tmp40E <= (void*)4)goto
_LL25E;if(*((int*)_tmp40E)!= 4)goto _LL25C;_tmp40F=((struct Cyc_Absyn_PointerType_struct*)
_tmp40E)->f1;_tmp410=(void*)_tmp40F.elt_typ;_tmp411=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp40E)->f1).elt_typ;_tmp412=_tmp40F.elt_tq;_LL25B: topt2=(void**)_tmp411;goto
_LL259;_LL25C: if(*((int*)_tmp40E)!= 2)goto _LL25E;_LL25D: topt2=({void**_tmp413=
_cycalloc(sizeof(*_tmp413));_tmp413[0]=*topt;_tmp413;});goto _LL259;_LL25E:;
_LL25F: goto _LL259;_LL259:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp415=_cycalloc(sizeof(*_tmp415));
_tmp415[0]=({struct Cyc_Absyn_PointerType_struct _tmp416;_tmp416.tag=4;_tmp416.f1=({
struct Cyc_Absyn_PtrInfo _tmp417;_tmp417.elt_typ=(void*)telt;_tmp417.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp417.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp418;_tmp418.rgn=(void*)rgn;
_tmp418.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp418.bounds=Cyc_Absyn_bounds_one_conref;_tmp418.zero_term=Cyc_Absyn_false_conref;
_tmp418.ptrloc=0;_tmp418;});_tmp417;});_tmp416;});_tmp415;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->v=(
void*)res_typ;_tmp414;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL23A:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp419=t;struct Cyc_Absyn_ArrayInfo _tmp41A;void*_tmp41B;struct
Cyc_Absyn_Tqual _tmp41C;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Conref*
_tmp41E;_LL261: if(_tmp419 <= (void*)4)goto _LL263;if(*((int*)_tmp419)!= 7)goto
_LL263;_tmp41A=((struct Cyc_Absyn_ArrayType_struct*)_tmp419)->f1;_tmp41B=(void*)
_tmp41A.elt_type;_tmp41C=_tmp41A.tq;_tmp41D=_tmp41A.num_elts;_tmp41E=_tmp41A.zero_term;
_LL262: {void*_tmp420;struct _tuple6 _tmp41F=Cyc_Tcutil_addressof_props(te,e);
_tmp420=_tmp41F.f2;{void*_tmp421=_tmp41D == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp423;_tmp423.tag=0;_tmp423.f1=(struct Cyc_Absyn_Exp*)
_tmp41D;_tmp423;});_tmp422;});t=Cyc_Absyn_atb_typ(_tmp41B,_tmp420,_tmp41C,
_tmp421,_tmp41E);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL263:;_LL264: return t;_LL260:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);void*_tmp424=(void*)e->r;void*_tmp425;_LL266: if(*((int*)_tmp424)!= 28)
goto _LL268;_LL267: goto _LL269;_LL268: if(*((int*)_tmp424)!= 29)goto _LL26A;_LL269:
goto _LL26B;_LL26A: if(*((int*)_tmp424)!= 0)goto _LL26C;_tmp425=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp424)->f1;if(_tmp425 <= (void*)1)goto _LL26C;if(*((int*)_tmp425)!= 5)goto _LL26C;
_LL26B: return t;_LL26C:;_LL26D: t=Cyc_Tcutil_compress(t);{void*_tmp426=t;struct Cyc_Absyn_ArrayInfo
_tmp427;void*_tmp428;struct Cyc_Absyn_Tqual _tmp429;struct Cyc_Absyn_Exp*_tmp42A;
struct Cyc_Absyn_Conref*_tmp42B;_LL26F: if(_tmp426 <= (void*)4)goto _LL271;if(*((int*)
_tmp426)!= 7)goto _LL271;_tmp427=((struct Cyc_Absyn_ArrayType_struct*)_tmp426)->f1;
_tmp428=(void*)_tmp427.elt_type;_tmp429=_tmp427.tq;_tmp42A=_tmp427.num_elts;
_tmp42B=_tmp427.zero_term;_LL270: {void*_tmp42D;struct _tuple6 _tmp42C=Cyc_Tcutil_addressof_props(
te,e);_tmp42D=_tmp42C.f2;{void*b=_tmp42A == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp42F;_tmp42F.tag=0;_tmp42F.f1=(struct Cyc_Absyn_Exp*)
_tmp42A;_tmp42F;});_tmp42E;});t=Cyc_Absyn_atb_typ(_tmp428,_tmp42D,_tmp429,b,
_tmp42B);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL271:;_LL272:
return t;_LL26E:;}_LL265:;}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp430=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp431;_LL274: if(*((int*)_tmp430)!= 13)goto _LL276;_tmp431=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp430)->f1;_LL275: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp431);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp431->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp431->topt))->v,0));e->topt=_tmp431->topt;goto
_LL273;_LL276:;_LL277: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp432=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp433;void*
_tmp434;struct Cyc_Absyn_Tqual _tmp435;struct Cyc_Absyn_PtrAtts _tmp436;void*_tmp437;
struct Cyc_Absyn_Conref*_tmp438;struct Cyc_Absyn_Conref*_tmp439;struct Cyc_Absyn_Conref*
_tmp43A;_LL279: if(_tmp432 <= (void*)4)goto _LL27B;if(*((int*)_tmp432)!= 4)goto
_LL27B;_tmp433=((struct Cyc_Absyn_PointerType_struct*)_tmp432)->f1;_tmp434=(void*)
_tmp433.elt_typ;_tmp435=_tmp433.elt_tq;_tmp436=_tmp433.ptr_atts;_tmp437=(void*)
_tmp436.rgn;_tmp438=_tmp436.nullable;_tmp439=_tmp436.bounds;_tmp43A=_tmp436.zero_term;
_LL27A:{void*_tmp43B=Cyc_Tcutil_compress(_tmp434);struct Cyc_Absyn_FnInfo _tmp43C;
struct Cyc_List_List*_tmp43D;struct Cyc_Core_Opt*_tmp43E;void*_tmp43F;struct Cyc_List_List*
_tmp440;int _tmp441;struct Cyc_Absyn_VarargInfo*_tmp442;struct Cyc_List_List*
_tmp443;struct Cyc_List_List*_tmp444;_LL27E: if(_tmp43B <= (void*)4)goto _LL280;if(*((
int*)_tmp43B)!= 8)goto _LL280;_tmp43C=((struct Cyc_Absyn_FnType_struct*)_tmp43B)->f1;
_tmp43D=_tmp43C.tvars;_tmp43E=_tmp43C.effect;_tmp43F=(void*)_tmp43C.ret_typ;
_tmp440=_tmp43C.args;_tmp441=_tmp43C.c_varargs;_tmp442=_tmp43C.cyc_varargs;
_tmp443=_tmp43C.rgn_po;_tmp444=_tmp43C.attributes;_LL27F: if(_tmp43D != 0){struct
_RegionHandle _tmp445=_new_region("rgn");struct _RegionHandle*rgn=& _tmp445;
_push_region(rgn);{struct _tuple4 _tmp446=({struct _tuple4 _tmp453;_tmp453.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp453.f2=rgn;_tmp453;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp446,_tmp43D);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_Absyn_FnType_struct
_tmp451;_tmp451.tag=8;_tmp451.f1=({struct Cyc_Absyn_FnInfo _tmp452;_tmp452.tvars=0;
_tmp452.effect=_tmp43E;_tmp452.ret_typ=(void*)_tmp43F;_tmp452.args=_tmp440;
_tmp452.c_varargs=_tmp441;_tmp452.cyc_varargs=_tmp442;_tmp452.rgn_po=_tmp443;
_tmp452.attributes=_tmp444;_tmp452;});_tmp451;});_tmp450;}));struct Cyc_Absyn_PointerType_struct*
_tmp447=({struct Cyc_Absyn_PointerType_struct*_tmp44C=_cycalloc(sizeof(*_tmp44C));
_tmp44C[0]=({struct Cyc_Absyn_PointerType_struct _tmp44D;_tmp44D.tag=4;_tmp44D.f1=({
struct Cyc_Absyn_PtrInfo _tmp44E;_tmp44E.elt_typ=(void*)ftyp;_tmp44E.elt_tq=
_tmp435;_tmp44E.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp44F;_tmp44F.rgn=(void*)
_tmp437;_tmp44F.nullable=_tmp438;_tmp44F.bounds=_tmp439;_tmp44F.zero_term=
_tmp43A;_tmp44F.ptrloc=0;_tmp44F;});_tmp44E;});_tmp44D;});_tmp44C;});struct Cyc_Absyn_Exp*
_tmp448=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp44A;_tmp44A.tag=14;_tmp44A.f1=_tmp448;_tmp44A.f2=ts;_tmp44A;});_tmp449;})));
e->topt=({struct Cyc_Core_Opt*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->v=(
void*)((void*)_tmp447);_tmp44B;});};_pop_region(rgn);}goto _LL27D;_LL280:;_LL281:
goto _LL27D;_LL27D:;}goto _LL278;_LL27B:;_LL27C: goto _LL278;_LL278:;}goto _LL273;
_LL273:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp454=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp455;struct _tuple1*_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_List_List*
_tmp458;struct Cyc_Core_Opt*_tmp459;struct Cyc_List_List*_tmp45A;void*_tmp45B;
struct _tuple1*_tmp45C;void*_tmp45D;void*_tmp45E;struct Cyc_List_List*_tmp45F;
struct Cyc_Absyn_Exp*_tmp460;void*_tmp461;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Core_Opt*
_tmp463;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Exp*_tmp465;struct Cyc_Absyn_Exp*
_tmp466;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*
_tmp469;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*
_tmp46C;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_List_List*
_tmp46F;struct Cyc_Absyn_VarargCallInfo*_tmp470;struct Cyc_Absyn_VarargCallInfo**
_tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_List_List*
_tmp474;void*_tmp475;struct Cyc_Absyn_Exp*_tmp476;void*_tmp477;void**_tmp478;
struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*_tmp47A;struct Cyc_Absyn_Exp*
_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;
struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct _dynforward_ptr*
_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct _dynforward_ptr*_tmp484;struct Cyc_Absyn_Exp*
_tmp485;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_List_List*_tmp487;struct _tuple2*
_tmp488;struct Cyc_List_List*_tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_Absyn_Stmt*
_tmp48B;struct Cyc_Absyn_Vardecl*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*
_tmp48E;int _tmp48F;int*_tmp490;struct _tuple1*_tmp491;struct _tuple1**_tmp492;
struct Cyc_List_List*_tmp493;struct Cyc_List_List**_tmp494;struct Cyc_List_List*
_tmp495;struct Cyc_Absyn_Aggrdecl*_tmp496;struct Cyc_Absyn_Aggrdecl**_tmp497;void*
_tmp498;struct Cyc_List_List*_tmp499;struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Tuniondecl*
_tmp49B;struct Cyc_Absyn_Tunionfield*_tmp49C;struct _tuple1*_tmp49D;struct _tuple1**
_tmp49E;struct Cyc_Absyn_Enumdecl*_tmp49F;struct Cyc_Absyn_Enumfield*_tmp4A0;
struct _tuple1*_tmp4A1;struct _tuple1**_tmp4A2;void*_tmp4A3;struct Cyc_Absyn_Enumfield*
_tmp4A4;struct Cyc_Absyn_MallocInfo _tmp4A5;int _tmp4A6;int*_tmp4A7;struct Cyc_Absyn_Exp*
_tmp4A8;void**_tmp4A9;void***_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp**
_tmp4AC;int _tmp4AD;int*_tmp4AE;_LL283: if(*((int*)_tmp454)!= 13)goto _LL285;
_tmp455=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp454)->f1;_LL284: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp455);return;_LL285: if(*((int*)_tmp454)!= 2)goto _LL287;_tmp456=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp454)->f1;_LL286: Cyc_Tcexp_resolve_unknown_id(
te,e,_tmp456);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL287: if(*((int*)_tmp454)
!= 10)goto _LL289;_tmp457=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp454)->f1;
_tmp458=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp454)->f2;_LL288: Cyc_Tcexp_resolve_unknown_fn(
te,e,_tmp457,_tmp458);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL289: if(*((int*)
_tmp454)!= 36)goto _LL28B;_tmp459=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp454)->f1;_tmp45A=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp454)->f2;
_LL28A: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp45A);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL28B: if(*((int*)_tmp454)!= 0)goto _LL28D;_tmp45B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp454)->f1;_LL28C: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp45B,e);goto _LL282;_LL28D: if(*((int*)_tmp454)!= 1)goto _LL28F;_tmp45C=((
struct Cyc_Absyn_Var_e_struct*)_tmp454)->f1;_tmp45D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp454)->f2;_LL28E: t=Cyc_Tcexp_tcVar(te,loc,_tmp45C,_tmp45D);goto _LL282;_LL28F:
if(*((int*)_tmp454)!= 3)goto _LL291;_tmp45E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp454)->f1;_tmp45F=((struct Cyc_Absyn_Primop_e_struct*)_tmp454)->f2;_LL290: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp45E,_tmp45F);goto _LL282;_LL291: if(*((int*)
_tmp454)!= 5)goto _LL293;_tmp460=((struct Cyc_Absyn_Increment_e_struct*)_tmp454)->f1;
_tmp461=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp454)->f2;_LL292: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp460,_tmp461);goto _LL282;_LL293: if(*((int*)_tmp454)!= 4)goto
_LL295;_tmp462=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp454)->f1;_tmp463=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp454)->f2;_tmp464=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp454)->f3;_LL294: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp462,_tmp463,_tmp464);
goto _LL282;_LL295: if(*((int*)_tmp454)!= 6)goto _LL297;_tmp465=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp454)->f1;_tmp466=((struct Cyc_Absyn_Conditional_e_struct*)_tmp454)->f2;
_tmp467=((struct Cyc_Absyn_Conditional_e_struct*)_tmp454)->f3;_LL296: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp465,_tmp466,_tmp467);goto _LL282;_LL297: if(*((int*)_tmp454)!= 7)
goto _LL299;_tmp468=((struct Cyc_Absyn_And_e_struct*)_tmp454)->f1;_tmp469=((struct
Cyc_Absyn_And_e_struct*)_tmp454)->f2;_LL298: t=Cyc_Tcexp_tcAnd(te,loc,_tmp468,
_tmp469);goto _LL282;_LL299: if(*((int*)_tmp454)!= 8)goto _LL29B;_tmp46A=((struct
Cyc_Absyn_Or_e_struct*)_tmp454)->f1;_tmp46B=((struct Cyc_Absyn_Or_e_struct*)
_tmp454)->f2;_LL29A: t=Cyc_Tcexp_tcOr(te,loc,_tmp46A,_tmp46B);goto _LL282;_LL29B:
if(*((int*)_tmp454)!= 9)goto _LL29D;_tmp46C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp454)->f1;_tmp46D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp454)->f2;_LL29C: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp46C,_tmp46D);goto _LL282;_LL29D: if(*((int*)
_tmp454)!= 11)goto _LL29F;_tmp46E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp454)->f1;
_tmp46F=((struct Cyc_Absyn_FnCall_e_struct*)_tmp454)->f2;_tmp470=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp454)->f3;_tmp471=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp454)->f3;_LL29E: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp46E,_tmp46F,_tmp471);
goto _LL282;_LL29F: if(*((int*)_tmp454)!= 12)goto _LL2A1;_tmp472=((struct Cyc_Absyn_Throw_e_struct*)
_tmp454)->f1;_LL2A0: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp472);goto _LL282;_LL2A1:
if(*((int*)_tmp454)!= 14)goto _LL2A3;_tmp473=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp454)->f1;_tmp474=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp454)->f2;_LL2A2:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp473,_tmp474);goto _LL282;_LL2A3: if(*((
int*)_tmp454)!= 15)goto _LL2A5;_tmp475=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp454)->f1;_tmp476=((struct Cyc_Absyn_Cast_e_struct*)_tmp454)->f2;_tmp477=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp454)->f4;_tmp478=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp454)->f4);_LL2A4: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp475,_tmp476,(void**)
_tmp478);goto _LL282;_LL2A5: if(*((int*)_tmp454)!= 16)goto _LL2A7;_tmp479=((struct
Cyc_Absyn_Address_e_struct*)_tmp454)->f1;_LL2A6: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp479);goto _LL282;_LL2A7: if(*((int*)_tmp454)!= 17)goto _LL2A9;_tmp47A=((
struct Cyc_Absyn_New_e_struct*)_tmp454)->f1;_tmp47B=((struct Cyc_Absyn_New_e_struct*)
_tmp454)->f2;_LL2A8: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp47A,e,_tmp47B);goto _LL282;
_LL2A9: if(*((int*)_tmp454)!= 19)goto _LL2AB;_tmp47C=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp454)->f1;_LL2AA: {void*_tmp4AF=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp47C);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4AF);goto _LL282;}_LL2AB: if(*((int*)_tmp454)!= 18)goto _LL2AD;
_tmp47D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp454)->f1;_LL2AC: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp47D);goto _LL282;_LL2AD: if(*((int*)_tmp454)!= 20)goto _LL2AF;
_tmp47E=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp454)->f1;_tmp47F=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp454)->f2;_LL2AE: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp47E,_tmp47F);goto _LL282;_LL2AF: if(*((int*)_tmp454)!= 21)goto _LL2B1;
_LL2B0:({void*_tmp4B0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4B1="gen() not in top-level initializer";
_tag_dynforward(_tmp4B1,sizeof(char),_get_zero_arr_size(_tmp4B1,35));}),
_tag_dynforward(_tmp4B0,sizeof(void*),0));});return;_LL2B1: if(*((int*)_tmp454)!= 
22)goto _LL2B3;_tmp480=((struct Cyc_Absyn_Deref_e_struct*)_tmp454)->f1;_LL2B2: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp480);goto _LL282;_LL2B3: if(*((int*)_tmp454)!= 
23)goto _LL2B5;_tmp481=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp454)->f1;
_tmp482=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp454)->f2;_LL2B4: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp481,_tmp482);goto _LL282;_LL2B5: if(*((int*)_tmp454)!= 24)goto
_LL2B7;_tmp483=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp454)->f1;_tmp484=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp454)->f2;_LL2B6: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp483,_tmp484);goto _LL282;_LL2B7: if(*((int*)_tmp454)!= 25)goto
_LL2B9;_tmp485=((struct Cyc_Absyn_Subscript_e_struct*)_tmp454)->f1;_tmp486=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp454)->f2;_LL2B8: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp485,_tmp486);goto _LL282;_LL2B9: if(*((int*)_tmp454)!= 26)goto
_LL2BB;_tmp487=((struct Cyc_Absyn_Tuple_e_struct*)_tmp454)->f1;_LL2BA: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp487);goto _LL282;_LL2BB: if(*((int*)_tmp454)!= 27)goto _LL2BD;
_tmp488=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp454)->f1;_tmp489=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp454)->f2;_LL2BC: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp488,_tmp489);goto
_LL282;_LL2BD: if(*((int*)_tmp454)!= 28)goto _LL2BF;_tmp48A=((struct Cyc_Absyn_Array_e_struct*)
_tmp454)->f1;_LL2BE: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4B2=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4B3;void*_tmp4B4;void**
_tmp4B5;struct Cyc_Absyn_Conref*_tmp4B6;_LL2D0: if(_tmp4B2 <= (void*)4)goto _LL2D2;
if(*((int*)_tmp4B2)!= 7)goto _LL2D2;_tmp4B3=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4B2)->f1;_tmp4B4=(void*)_tmp4B3.elt_type;_tmp4B5=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4B2)->f1).elt_type;_tmp4B6=_tmp4B3.zero_term;_LL2D1: elt_topt=(void**)_tmp4B5;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4B6);
goto _LL2CF;_LL2D2:;_LL2D3: goto _LL2CF;_LL2CF:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp48A);goto _LL282;}_LL2BF: if(*((int*)_tmp454)!= 37)goto
_LL2C1;_tmp48B=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp454)->f1;_LL2C0: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp48B);goto _LL282;_LL2C1: if(*((int*)_tmp454)!= 29)goto _LL2C3;
_tmp48C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp454)->f1;_tmp48D=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp454)->f2;_tmp48E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp454)->f3;_tmp48F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp454)->f4;
_tmp490=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp454)->f4;_LL2C2: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp48C,_tmp48D,_tmp48E,_tmp490);goto
_LL282;_LL2C3: if(*((int*)_tmp454)!= 30)goto _LL2C5;_tmp491=((struct Cyc_Absyn_Struct_e_struct*)
_tmp454)->f1;_tmp492=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp454)->f1;_tmp493=((struct Cyc_Absyn_Struct_e_struct*)_tmp454)->f2;_tmp494=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp454)->f2;_tmp495=((
struct Cyc_Absyn_Struct_e_struct*)_tmp454)->f3;_tmp496=((struct Cyc_Absyn_Struct_e_struct*)
_tmp454)->f4;_tmp497=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp454)->f4;_LL2C4: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp492,_tmp494,_tmp495,
_tmp497);goto _LL282;_LL2C5: if(*((int*)_tmp454)!= 31)goto _LL2C7;_tmp498=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp454)->f1;_tmp499=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp454)->f2;_LL2C6: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp498,_tmp499);goto _LL282;
_LL2C7: if(*((int*)_tmp454)!= 32)goto _LL2C9;_tmp49A=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp454)->f1;_tmp49B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp454)->f2;_tmp49C=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp454)->f3;_LL2C8: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp49A,_tmp49B,_tmp49C);goto _LL282;_LL2C9: if(*((int*)_tmp454)!= 33)goto
_LL2CB;_tmp49D=((struct Cyc_Absyn_Enum_e_struct*)_tmp454)->f1;_tmp49E=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp454)->f1;_tmp49F=((struct Cyc_Absyn_Enum_e_struct*)
_tmp454)->f2;_tmp4A0=((struct Cyc_Absyn_Enum_e_struct*)_tmp454)->f3;_LL2CA:*
_tmp49E=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4A0))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4B8;_tmp4B8.tag=12;_tmp4B8.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp49F))->name;_tmp4B8.f2=_tmp49F;_tmp4B8;});_tmp4B7;});goto _LL282;
_LL2CB: if(*((int*)_tmp454)!= 34)goto _LL2CD;_tmp4A1=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp454)->f1;_tmp4A2=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp454)->f1;_tmp4A3=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp454)->f2;
_tmp4A4=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp454)->f3;_LL2CC:*_tmp4A2=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4A4))->name;t=_tmp4A3;goto _LL282;
_LL2CD: if(*((int*)_tmp454)!= 35)goto _LL282;_tmp4A5=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp454)->f1;_tmp4A6=_tmp4A5.is_calloc;_tmp4A7=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp454)->f1).is_calloc;_tmp4A8=_tmp4A5.rgn;_tmp4A9=_tmp4A5.elt_type;_tmp4AA=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp454)->f1).elt_type;_tmp4AB=
_tmp4A5.num_elts;_tmp4AC=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp454)->f1).num_elts;_tmp4AD=_tmp4A5.fat_result;_tmp4AE=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp454)->f1).fat_result;_LL2CE: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4A8,_tmp4AA,
_tmp4AC,_tmp4A7,_tmp4AE);goto _LL282;_LL282:;}e->topt=({struct Cyc_Core_Opt*
_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->v=(void*)t;_tmp4B9;});}
