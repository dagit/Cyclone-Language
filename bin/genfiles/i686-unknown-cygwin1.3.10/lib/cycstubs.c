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
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

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
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - 
                             ((unsigned)_arr_ptr->curr)) / elt_sz;
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

/* the next three routines swap [x] and [y]; not thread safe! */
static inline void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static inline void _swap_dynforward(struct _dynforward_ptr *x, 
				    struct _dynforward_ptr *y) {
  struct _dynforward_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
static inline void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 void exit(int);void*abort();struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct
_dynforward_ptr wrap_Cbuffer_as_buffer(char*,unsigned int);struct Cyc_in_addr{
unsigned int s_addr;};unsigned long Cyc_htonl(unsigned long x);unsigned short Cyc_htons(
unsigned short x);unsigned long Cyc_ntohl(unsigned long x);unsigned short Cyc_ntohs(
unsigned short x);unsigned long __stub_htonl(unsigned long x);unsigned short
__stub_htons(unsigned short x);unsigned long __stub_ntohl(unsigned long x);
unsigned short __stub_ntohs(unsigned short x);unsigned long Cyc_htonl(unsigned long
x){return __stub_htonl(x);}unsigned short Cyc_htons(unsigned short x){return
__stub_htons(x);}unsigned long Cyc_ntohl(unsigned long x){return __stub_ntohl(x);}
unsigned short Cyc_ntohs(unsigned short x){return __stub_ntohs(x);}struct Cyc_dirent{
long d_version;long __d_reserved[2];long d_fd;unsigned long d_ino;char d_name[256];};
struct Cyc___cycDIR;int Cyc_closedir(struct Cyc___cycDIR*);struct Cyc___cycDIR*Cyc_opendir(
const char*);struct Cyc_dirent*Cyc_readdir(struct Cyc___cycDIR*);void Cyc_rewinddir(
struct Cyc___cycDIR*);void Cyc_seekdir(struct Cyc___cycDIR*,long);long Cyc_telldir(
struct Cyc___cycDIR*);struct Cyc_Cdirent___abstractDIR;struct Cyc___cycDIR{struct
Cyc_Cdirent___abstractDIR*dir;};int closedir(struct Cyc_Cdirent___abstractDIR*d);
int Cyc_closedir(struct Cyc___cycDIR*d){return closedir(d->dir);}struct Cyc_Cdirent___abstractDIR*
opendir(const char*);static char _tmp1[32]="opendir called with NULL string";static
struct Cyc_Core_Failure_struct Cyc___opendir_failure={Cyc_Core_Failure,{_tmp1,
_tmp1 + 32}};struct Cyc___cycDIR*Cyc_opendir(const char*f){struct Cyc_Cdirent___abstractDIR*
_tmp2=opendir(f);return(unsigned int)_tmp2?({struct Cyc___cycDIR*_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3->dir=(struct Cyc_Cdirent___abstractDIR*)_tmp2;_tmp3;}): 0;}
struct Cyc_dirent*readdir(struct Cyc_Cdirent___abstractDIR*);struct Cyc_dirent*Cyc_readdir(
struct Cyc___cycDIR*d){return readdir(d->dir);}void rewinddir(struct Cyc_Cdirent___abstractDIR*
d);void Cyc_rewinddir(struct Cyc___cycDIR*d){return rewinddir(d->dir);}void seekdir(
struct Cyc_Cdirent___abstractDIR*d,long);void Cyc_seekdir(struct Cyc___cycDIR*d,
long x){return seekdir(d->dir,x);}long telldir(struct Cyc_Cdirent___abstractDIR*d);
long Cyc_telldir(struct Cyc___cycDIR*d){return telldir(d->dir);}int*__CYCLONE_ERRNO();
struct Cyc_flock{short l_type;short l_whence;long l_start;long l_len;short l_pid;short
l_xxx;};struct Cyc_Flock_struct{int tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{
int tag;long f1;};int Cyc_fcntl(int fd,int cmd,struct _dynforward_ptr);int Cyc_open(
const char*,int,struct _dynforward_ptr);int fcntl(int fd,int cmd);int fcntl_with_arg(
int fd,int cmd,long arg);int fcntl_with_lock(int fd,int cmd,struct Cyc_flock*lock);int
open_without_mode(const char*,int);int open_with_mode(const char*,int,int);static
char _tmp5[21]="fcntl: too many args";static struct Cyc_Core_Failure_struct Cyc___fcntl_failure={
Cyc_Core_Failure,{_tmp5,_tmp5 + 21}};int Cyc_fcntl(int fd,int cmd,struct
_dynforward_ptr argv){if(_get_dynforward_size(argv,sizeof(void*))== 0)return fcntl(
fd,cmd);else{if(_get_dynforward_size(argv,sizeof(void*))!= 1)(int)_throw((void*)&
Cyc___fcntl_failure);else{void*_tmp6=*((void**)_check_dynforward_subscript(argv,
sizeof(void*),0));long _tmp7;struct Cyc_flock*_tmp8;_LL1: if(*((int*)_tmp6)!= 1)
goto _LL3;_tmp7=((struct Cyc_Long_struct*)_tmp6)->f1;_LL2: return fcntl_with_arg(fd,
cmd,_tmp7);_LL3: if(*((int*)_tmp6)!= 0)goto _LL0;_tmp8=((struct Cyc_Flock_struct*)
_tmp6)->f1;_LL4: return fcntl_with_lock(fd,cmd,(struct Cyc_flock*)_tmp8);_LL0:;}}}
int Cyc_open(const char*s,int i,struct _dynforward_ptr ms){if(_get_dynforward_size(ms,
sizeof(int))>= 1)return open_with_mode(s,i,*((int*)_check_dynforward_subscript(ms,
sizeof(int),0)));else{return open_without_mode(s,i);}}enum Cyc___anonymous_enum_31__{
Cyc_IPPORT_ECHO  = 7,Cyc_IPPORT_DISCARD  = 9,Cyc_IPPORT_SYSTAT  = 11,Cyc_IPPORT_DAYTIME
 = 13,Cyc_IPPORT_NETSTAT  = 15,Cyc_IPPORT_FTP  = 21,Cyc_IPPORT_TELNET  = 23,Cyc_IPPORT_SMTP
 = 25,Cyc_IPPORT_TIMESERVER  = 37,Cyc_IPPORT_NAMESERVER  = 42,Cyc_IPPORT_WHOIS  = 
43,Cyc_IPPORT_MTP  = 57,Cyc_IPPORT_TFTP  = 69,Cyc_IPPORT_RJE  = 77,Cyc_IPPORT_FINGER
 = 79,Cyc_IPPORT_TTYLINK  = 87,Cyc_IPPORT_SUPDUP  = 95,Cyc_IPPORT_EXECSERVER  = 512,
Cyc_IPPORT_LOGINSERVER  = 513,Cyc_IPPORT_CMDSERVER  = 514,Cyc_IPPORT_EFSSERVER  = 
520,Cyc_IPPORT_BIFFUDP  = 512,Cyc_IPPORT_WHOSERVER  = 513,Cyc_IPPORT_ROUTESERVER
 = 520,Cyc_IPPORT_RESERVED  = 1024,Cyc_IPPORT_USERRESERVED  = 5000};struct Cyc_servent{
char*s_name;char**s_aliases;short s_port;char*s_proto;};struct Cyc_protoent{char*
p_name;char**p_aliases;short p_proto;};struct Cyc_hostent{char*h_name;char**
h_aliases;short h_addrtype;short h_length;struct Cyc_in_addr**h_addr_list;};enum 
Cyc___anonymous_enum_32__{Cyc_IPPROTO_IP  = 0,Cyc_IPPROTO_ICMP  = 1,Cyc_IPPROTO_IGMP
 = 2,Cyc_IPPROTO_IPIP  = 4,Cyc_IPPROTO_TCP  = 6,Cyc_IPPROTO_EGP  = 8,Cyc_IPPROTO_PUP
 = 12,Cyc_IPPROTO_UDP  = 17,Cyc_IPPROTO_IDP  = 22,Cyc_IPPROTO_RAW  = 255,Cyc_IPPROTO_MAX
 = 256};struct Cyc_sockaddr_in{short sin_family;unsigned short sin_port;struct Cyc_in_addr
sin_addr;unsigned char __pad[((16 - sizeof(short))- sizeof(unsigned short))- 
sizeof(struct Cyc_in_addr)];};struct Cyc_in6_addr{unsigned char s6_addr[16];};
struct Cyc_sockaddr_in6{unsigned short sin6_family;unsigned short sin6_port;
unsigned long sin6_flowinfo;struct Cyc_in6_addr sin6_addr;};struct Cyc_pollfd{int fd;
short events;short revents;};int Cyc_poll(struct _dynforward_ptr,unsigned int,int);
int poll(struct Cyc_pollfd*,unsigned int b,int c);static char _tmpA[22]="poll called with NULL";
static struct Cyc_Core_Failure_struct Cyc___poll_failure_1={Cyc_Core_Failure,{_tmpA,
_tmpA + 22}};static char _tmpC[39]="poll called with out-of-bounds pointer";static
struct Cyc_Core_Failure_struct Cyc___poll_failure_2={Cyc_Core_Failure,{_tmpC,_tmpC
+ 39}};int Cyc_poll(struct _dynforward_ptr a,unsigned int b,int c){if(!((unsigned int)
a.curr))(int)_throw((void*)& Cyc___poll_failure_1);{struct _handler_cons _tmpD;
_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))_tmpF=1;if(!_tmpF){*((
struct Cyc_pollfd*)_check_dynforward_subscript(a,sizeof(struct Cyc_pollfd),0));;
_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*_tmp11=_tmpE;_LL6: if(
_tmp11 != Cyc_Array_bounds)goto _LL8;_LL7:(int)_throw((void*)& Cyc___poll_failure_2);
_LL8:;_LL9:(void)_throw(_tmp11);_LL5:;}}}{int len=(int)_get_dynforward_size(a,
sizeof(struct Cyc_pollfd));return poll((struct Cyc_pollfd*)_untag_dynforward_ptr(a,
sizeof(struct Cyc_pollfd),0),b < (unsigned int)len?b:(unsigned int)len,c);}}struct
Cyc___SIG_BUILTIN_struct{int tag;void*f1;};struct Cyc___SIG_HANDLER_struct{int tag;
void(*f1)(int);};void*Cyc_Csignal_make_SIG_DFL(){return(void*)({struct Cyc___SIG_BUILTIN_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc___SIG_BUILTIN_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=(void*)((void*)0);_tmp13;});_tmp12;});}void*Cyc_Csignal_make_SIG_ERR(){
return(void*)({struct Cyc___SIG_BUILTIN_struct*_tmp14=_cycalloc(sizeof(*_tmp14));
_tmp14[0]=({struct Cyc___SIG_BUILTIN_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(void*)((
void*)1);_tmp15;});_tmp14;});}void*Cyc_Csignal_make_SIG_IGN(){return(void*)({
struct Cyc___SIG_BUILTIN_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc___SIG_BUILTIN_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(void*)((void*)1);
_tmp17;});_tmp16;});}void*Cyc_Csignal_make_SIG_HANDLER(void(*f)(int)){return(
void*)({struct Cyc___SIG_HANDLER_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[
0]=({struct Cyc___SIG_HANDLER_struct _tmp19;_tmp19.tag=1;_tmp19.f1=f;_tmp19;});
_tmp18;});}struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc___cycFILE*
Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf);struct Cyc_String_pa_struct{int
tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};void Cyc_clearerr(struct Cyc___cycFILE*);int Cyc_fclose(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fdopen(int,const char*);int Cyc_feof(
struct Cyc___cycFILE*);int Cyc_ferror(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fgetpos(struct Cyc___cycFILE*,
long*);struct _dynforward_ptr Cyc_fgets(struct _dynforward_ptr,int,struct Cyc___cycFILE*);
int Cyc_fileno(struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,
const char*);int Cyc_fputc(int,struct Cyc___cycFILE*);int Cyc_fputs(const char*,
struct Cyc___cycFILE*);unsigned int Cyc_fread(struct _dynforward_ptr,unsigned int,
unsigned int,struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_freopen(const char*,
const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};int Cyc_fseek(struct Cyc___cycFILE*,long,int);int Cyc_fsetpos(
struct Cyc___cycFILE*,const long*);long Cyc_ftell(struct Cyc___cycFILE*);
unsigned int Cyc_fwrite(struct _dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);
int Cyc_getc(struct Cyc___cycFILE*);int Cyc_getchar();int Cyc_pclose(struct Cyc___cycFILE*);
struct Cyc___cycFILE*Cyc_popen(const char*,const char*);int Cyc_putc(int,struct Cyc___cycFILE*);
void Cyc_rewind(struct Cyc___cycFILE*);int Cyc_setvbuf(struct Cyc___cycFILE*,struct
_dynforward_ptr,int,unsigned int);void Cyc_setbuf(struct Cyc___cycFILE*,struct
_dynforward_ptr);int Cyc_ungetc(int,struct Cyc___cycFILE*);int Cyc_getw(struct Cyc___cycFILE*);
int Cyc_putw(int,struct Cyc___cycFILE*);extern char Cyc_FileCloseError[19];extern
char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct _dynforward_ptr,
struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);struct Cyc_Cstdio___abstractFILE;
struct Cyc___cycFILE{struct Cyc_Cstdio___abstractFILE*file;};struct Cyc___cycFILE*
Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf){return(unsigned int)cf?({
struct Cyc___cycFILE*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->file=(struct Cyc_Cstdio___abstractFILE*)
cf;_tmp1A;}): 0;}void clearerr(struct Cyc_Cstdio___abstractFILE*);void Cyc_clearerr(
struct Cyc___cycFILE*f){return clearerr(f->file);}int fclose(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fclose(struct Cyc___cycFILE*f){return fclose(f->file);}struct Cyc_Cstdio___abstractFILE*
fdopen(int,const char*);struct Cyc___cycFILE*Cyc_fdopen(int i,const char*s){struct
Cyc_Cstdio___abstractFILE*_tmp1B=fdopen(i,s);return(unsigned int)_tmp1B?({struct
Cyc___cycFILE*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp1B;_tmp1C;}): 0;}int feof(struct Cyc_Cstdio___abstractFILE*);int Cyc_feof(struct
Cyc___cycFILE*f){return feof(f->file);}int ferror(struct Cyc_Cstdio___abstractFILE*);
int Cyc_ferror(struct Cyc___cycFILE*f){return ferror(f->file);}int fflush(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fflush(struct Cyc___cycFILE*f){if((unsigned int)f)return fflush((struct Cyc_Cstdio___abstractFILE*)
f->file);else{return fflush(0);}}int fgetc(struct Cyc_Cstdio___abstractFILE*);int
Cyc_fgetc(struct Cyc___cycFILE*f){return fgetc(f->file);}int fgetpos(struct Cyc_Cstdio___abstractFILE*,
long*);int Cyc_fgetpos(struct Cyc___cycFILE*f,long*x){return fgetpos(f->file,x);}
char*fgets(char*,int,struct Cyc_Cstdio___abstractFILE*);static char _tmp1E[30]="fgets called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fgets_failure_1={Cyc_Core_Failure,{
_tmp1E,_tmp1E + 30}};static char _tmp20[27]="fgets: buffer insufficient";static
struct Cyc_Core_Failure_struct Cyc___fgets_failure_2={Cyc_Core_Failure,{_tmp20,
_tmp20 + 27}};struct _dynforward_ptr Cyc_fgets(struct _dynforward_ptr s,int n,struct
Cyc___cycFILE*f){if(!((unsigned int)s.curr))(int)_throw((void*)& Cyc___fgets_failure_1);{
char*result;char*buffer=(char*)_check_null(_untag_dynforward_ptr(s,sizeof(char),
1));unsigned int len=_get_dynforward_size(s,sizeof(char));if(len < n)(int)_throw((
void*)& Cyc___fgets_failure_2);result=fgets(buffer,n,f->file);if(result == 0)
return _tag_dynforward(0,0,0);else{return s;}}}int fileno(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fileno(struct Cyc___cycFILE*f){return fileno(f->file);}struct Cyc_Cstdio___abstractFILE*
fopen(const char*,const char*);struct Cyc___cycFILE*Cyc_fopen(const char*name,const
char*type){struct Cyc_Cstdio___abstractFILE*_tmp21=fopen(name,type);return(
unsigned int)_tmp21?({struct Cyc___cycFILE*_tmp22=_cycalloc(sizeof(*_tmp22));
_tmp22->file=(struct Cyc_Cstdio___abstractFILE*)_tmp21;_tmp22;}): 0;}int fputc(int,
struct Cyc_Cstdio___abstractFILE*);int Cyc_fputc(int x,struct Cyc___cycFILE*f){
return fputc(x,f->file);}int fputs(const char*,struct Cyc_Cstdio___abstractFILE*);
int Cyc_fputs(const char*x,struct Cyc___cycFILE*f){return fputs(x,f->file);}
unsigned int fread(char*,unsigned int,unsigned int,struct Cyc_Cstdio___abstractFILE*);
static char _tmp24[27]="fread: buffer insufficient";static struct Cyc_Core_Failure_struct
Cyc___fread_failure={Cyc_Core_Failure,{_tmp24,_tmp24 + 27}};unsigned int Cyc_fread(
struct _dynforward_ptr ptr,unsigned int size,unsigned int nmemb,struct Cyc___cycFILE*
f){if(size * nmemb > _get_dynforward_size(ptr,sizeof(char)))(int)_throw((void*)&
Cyc___fread_failure);return fread((char*)_check_null(_untag_dynforward_ptr(ptr,
sizeof(char),1)),size,nmemb,f->file);}struct Cyc_Cstdio___abstractFILE*freopen(
const char*,const char*,struct Cyc_Cstdio___abstractFILE*);struct Cyc___cycFILE*Cyc_freopen(
const char*filename,const char*modes,struct Cyc___cycFILE*f){struct Cyc_Cstdio___abstractFILE*
_tmp25=freopen(filename,modes,f->file);return(unsigned int)_tmp25?(struct Cyc___cycFILE*)({
struct Cyc___cycFILE*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp25;_tmp26;}): 0;}int fseek(struct Cyc_Cstdio___abstractFILE*,long,int);int Cyc_fseek(
struct Cyc___cycFILE*a,long b,int c){return fseek(a->file,b,c);}int fsetpos(struct Cyc_Cstdio___abstractFILE*,
const long*);int Cyc_fsetpos(struct Cyc___cycFILE*f,const long*b){return fsetpos(f->file,
b);}long ftell(struct Cyc_Cstdio___abstractFILE*);long Cyc_ftell(struct Cyc___cycFILE*
f){return ftell(f->file);}unsigned int fwrite(const char*,unsigned int,unsigned int,
struct Cyc_Cstdio___abstractFILE*);static char _tmp28[31]="fwrite called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fwrite_failure_1={Cyc_Core_Failure,{
_tmp28,_tmp28 + 31}};static char _tmp2A[28]="fwrite: buffer insufficient";static
struct Cyc_Core_Failure_struct Cyc___fwrite_failure_2={Cyc_Core_Failure,{_tmp2A,
_tmp2A + 28}};unsigned int Cyc_fwrite(struct _dynforward_ptr ptr,unsigned int size,
unsigned int nmemb,struct Cyc___cycFILE*f){if(!((unsigned int)ptr.curr))(int)
_throw((void*)& Cyc___fwrite_failure_1);else{if(size * nmemb > _get_dynforward_size(
ptr,sizeof(char)))(int)_throw((void*)& Cyc___fwrite_failure_2);return fwrite((
const char*)_untag_dynforward_ptr(ptr,sizeof(char),1),size,nmemb,f->file);}}int
getc(struct Cyc_Cstdio___abstractFILE*);int Cyc_getc(struct Cyc___cycFILE*f){return
getc(f->file);}int getchar();int Cyc_getchar(){return getchar();}int pclose(struct
Cyc_Cstdio___abstractFILE*);int Cyc_pclose(struct Cyc___cycFILE*f){return pclose(f->file);}
struct Cyc_Cstdio___abstractFILE*popen(const char*,const char*);struct Cyc___cycFILE*
Cyc_popen(const char*name,const char*type){struct Cyc_Cstdio___abstractFILE*_tmp2B=
popen(name,type);return(unsigned int)_tmp2B?({struct Cyc___cycFILE*_tmp2C=
_cycalloc(sizeof(*_tmp2C));_tmp2C->file=(struct Cyc_Cstdio___abstractFILE*)_tmp2B;
_tmp2C;}): 0;}int putc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_putc(int x,
struct Cyc___cycFILE*f){return putc(x,f->file);}void rewind(struct Cyc_Cstdio___abstractFILE*);
void Cyc_rewind(struct Cyc___cycFILE*f){return rewind(f->file);}int setvbuf(struct
Cyc_Cstdio___abstractFILE*,char*,int,unsigned int);static char _tmp2E[24]="setvbuf: buffer is NULL";
static struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_1={Cyc_Core_Failure,{
_tmp2E,_tmp2E + 24}};static char _tmp30[29]="setvbuf: buffer insufficient";static
struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_2={Cyc_Core_Failure,{_tmp30,
_tmp30 + 29}};int Cyc_setvbuf(struct Cyc___cycFILE*f,struct _dynforward_ptr buf,int
type,unsigned int size){if(!((unsigned int)buf.curr))(int)_throw((void*)& Cyc___setvbuf_failure_1);
if(_get_dynforward_size(buf,sizeof(char))< size)(int)_throw((void*)& Cyc___setvbuf_failure_2);
return setvbuf(f->file,(char*)_check_null(_untag_dynforward_ptr(buf,sizeof(char),
1)),type,size);}void Cyc_setbuf(struct Cyc___cycFILE*f,struct _dynforward_ptr buf){
Cyc_setvbuf(f,buf,(unsigned int)buf.curr?0: 2,1024);return;}int ungetc(int,struct
Cyc_Cstdio___abstractFILE*);int Cyc_ungetc(int x,struct Cyc___cycFILE*f){return
ungetc(x,f->file);}int getw(struct Cyc_Cstdio___abstractFILE*);int Cyc_getw(struct
Cyc___cycFILE*f){return getw(f->file);}int putw(int,struct Cyc_Cstdio___abstractFILE*);
int Cyc_putw(int x,struct Cyc___cycFILE*f){return putw(x,f->file);}char Cyc_FileCloseError[
19]="\000\000\000\000FileCloseError\000";char Cyc_FileOpenError[18]="\000\000\000\000FileOpenError\000";
struct Cyc___cycFILE*Cyc_file_open(struct _dynforward_ptr fname,struct
_dynforward_ptr mode){struct Cyc___cycFILE*f=Cyc_fopen((const char*)
_untag_dynforward_ptr(fname,sizeof(char),1),(const char*)_untag_dynforward_ptr(
mode,sizeof(char),1));if(f == 0){struct _dynforward_ptr fn=({unsigned int _tmp33=
_get_dynforward_size(fname,sizeof(char));char*_tmp34=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp33 + 1));struct _dynforward_ptr _tmp36=
_tag_dynforward(_tmp34,sizeof(char),_tmp33 + 1);{unsigned int _tmp35=_tmp33;
unsigned int i;for(i=0;i < _tmp35;i ++){_tmp34[i]=((const char*)fname.curr)[(int)i];}
_tmp34[_tmp35]=(char)0;}_tmp36;});(int)_throw((void*)({struct Cyc_FileOpenError_struct*
_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=({struct Cyc_FileOpenError_struct
_tmp32;_tmp32.tag=Cyc_FileOpenError;_tmp32.f1=fn;_tmp32;});_tmp31;}));}return(
struct Cyc___cycFILE*)f;}void Cyc_file_close(struct Cyc___cycFILE*f){if(Cyc_fclose(
f)!= 0)(int)_throw((void*)Cyc_FileCloseError);}typedef struct{int quot;int rem;}Cyc_div_t;
typedef struct{long quot;long rem;}Cyc_ldiv_t;void*abort();void exit(int);void Cyc_free(
struct _dynforward_ptr);void Cyc_qsort(struct _dynforward_ptr,unsigned int,
unsigned int,int(*)(const void*,const void*));void Cyc_free(struct _dynforward_ptr
ptr){;}void qsort(void*,unsigned int,unsigned int,int(*)(const void*,const void*));
static char _tmp38[23]="qsort called with NULL";static struct Cyc_Core_Failure_struct
Cyc___qsort_failure_1={Cyc_Core_Failure,{_tmp38,_tmp38 + 23}};static char _tmp3A[42]="qsort called with insufficient array size";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_2={Cyc_Core_Failure,{
_tmp3A,_tmp3A + 42}};void Cyc_qsort(struct _dynforward_ptr tab,unsigned int nmemb,
unsigned int szmemb,int(*compar)(const void*,const void*)){if(!((unsigned int)tab.curr))(
int)_throw((void*)& Cyc___qsort_failure_1);if(_get_dynforward_size(tab,sizeof(
void))< nmemb)(int)_throw((void*)& Cyc___qsort_failure_2);qsort((void*)
_untag_dynforward_ptr(tab,sizeof(void),1),nmemb,(unsigned int)szmemb,compar);}
struct _dynforward_ptr Cyc_mmap(struct _dynforward_ptr,unsigned int length,int prot,
int flags,int fd,long offset);int Cyc_munmap(struct _dynforward_ptr start,unsigned int
length);char*__stub_mmap(char*start,unsigned int length,int prot,int flags,int fd,
long offset);int munmap(const char*start,unsigned int length);static char _tmp3C[42]="mmap called with illegal start/prot/flags";
static struct Cyc_Core_Failure_struct Cyc___mmap_failure={Cyc_Core_Failure,{_tmp3C,
_tmp3C + 42}};struct _dynforward_ptr Cyc_mmap(struct _dynforward_ptr ignored,
unsigned int length,int prot,int flags,int fd,long offset){if((ignored.curr == (
_tag_dynforward(0,0,0)).curr  && prot == 1) && (flags == 2  || flags == 1))return(
struct _dynforward_ptr)wrap_Cbuffer_as_buffer(__stub_mmap(0,length,prot,flags,fd,
offset),length);else{(int)_throw((void*)& Cyc___mmap_failure);}}int Cyc_munmap(
struct _dynforward_ptr start,unsigned int length){return munmap((const char*)
_check_null(_untag_dynforward_ptr(start,sizeof(char),1)),length);}struct Cyc_timeval{
long tv_sec;long tv_usec;};struct Cyc_rlimit{unsigned long rlim_cur;unsigned long
rlim_max;};struct Cyc_rusage{struct Cyc_timeval ru_utime;struct Cyc_timeval ru_stime;
long ru_maxrss;long ru_ixrss;long ru_idrss;long ru_isrss;long ru_minflt;long ru_majflt;
long ru_nswap;long ru_inblock;long ru_oublock;long ru_msgsnd;long ru_msgrcv;long
ru_nsignals;long ru_nvcsw;long ru_nivcsw;};struct Cyc_timespec{long tv_sec;long
tv_nsec;};struct Cyc__types_fd_set{long fds_bits[(64 + (sizeof(long)* 8 - 1))/ (
sizeof(long)* 8)];};int Cyc_select(int a,struct Cyc__types_fd_set*b,struct Cyc__types_fd_set*
c,struct Cyc__types_fd_set*d,struct Cyc_timeval*e);void Cyc_FD_CLR(int,struct Cyc__types_fd_set*);
int Cyc_FD_ISSET(int,struct Cyc__types_fd_set*);void Cyc_FD_SET(int,struct Cyc__types_fd_set*);
void Cyc_FD_ZERO(struct Cyc__types_fd_set*);int select(int,struct Cyc__types_fd_set*,
struct Cyc__types_fd_set*,struct Cyc__types_fd_set*,struct Cyc_timeval*);extern int
Cyc_select(int a,struct Cyc__types_fd_set*b,struct Cyc__types_fd_set*c,struct Cyc__types_fd_set*
d,struct Cyc_timeval*e){if(a < 0  || a > 64){*((int*)_check_null(__CYCLONE_ERRNO()))=
22;return - 1;}return select(a,b,c,d,e);}void __stub_FD_CLR(int,struct Cyc__types_fd_set*);
int __stub_FD_ISSET(int,struct Cyc__types_fd_set*);void __stub_FD_SET(int,struct Cyc__types_fd_set*);
void __stub_FD_ZERO(struct Cyc__types_fd_set*);void Cyc_FD_CLR(int a,struct Cyc__types_fd_set*
b){if(a < 0  || a > 64)return;return __stub_FD_CLR(a,b);}int Cyc_FD_ISSET(int a,struct
Cyc__types_fd_set*b){if(a < 0  || a > 64)return 0;return __stub_FD_ISSET(a,b);}void
Cyc_FD_SET(int a,struct Cyc__types_fd_set*b){if(a < 0  || a > 64)return;return
__stub_FD_SET(a,b);}void Cyc_FD_ZERO(struct Cyc__types_fd_set*a){return
__stub_FD_ZERO(a);}struct Cyc_linger{unsigned short l_onoff;unsigned short l_linger;
};struct Cyc_SA_sockaddr_in_struct{int tag;struct Cyc_sockaddr_in*f1;};struct Cyc_SA_socklenptr_struct{
int tag;int*f1;};struct Cyc_SA_socklen_struct{int tag;int f1;};struct Cyc_SO_int_struct{
int tag;int*f1;};struct Cyc_SO_timeval_struct{int tag;struct Cyc_timeval*f1;};struct
Cyc_SO_socklenptr_struct{int tag;int*f1;};struct Cyc_SO_socklen_struct{int tag;int
f1;};int Cyc_accept(int fd,struct _dynforward_ptr);extern char Cyc_SocketError[16];
int Cyc_bind(int fd,struct _dynforward_ptr);int Cyc_connect(int fd,struct
_dynforward_ptr);int Cyc_getpeername(int fd,struct _dynforward_ptr);int Cyc_getsockname(
int fd,struct _dynforward_ptr);int Cyc_getsockopt(int fd,int level,int optname,struct
_dynforward_ptr);long Cyc_recv(int fd,struct _dynforward_ptr buf,unsigned int n,int
flags);long Cyc_recvfrom(int fd,struct _dynforward_ptr buf,unsigned int n,int flags,
struct _dynforward_ptr);long Cyc_send(int fd,struct _dynforward_ptr buf,unsigned int n,
int flags);long Cyc_sendto(int fd,struct _dynforward_ptr buf,unsigned int n,int flags,
struct _dynforward_ptr);int Cyc_setsockopt(int fd,int level,int optname,struct
_dynforward_ptr);char Cyc_SocketError[16]="\000\000\000\000SocketError\000";int
accept_in(int,const struct Cyc_sockaddr_in*,int*);struct _tuple0{void*f1;void*f2;};
int Cyc_accept(int fd,struct _dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(
void*))!= 2)(int)_throw((void*)Cyc_SocketError);{struct _tuple0 _tmp3E=({struct
_tuple0 _tmp3D;_tmp3D.f1=*((void**)_check_dynforward_subscript(ap,sizeof(void*),0));
_tmp3D.f2=*((void**)_check_dynforward_subscript(ap,sizeof(void*),1));_tmp3D;});
void*_tmp3F;struct Cyc_sockaddr_in*_tmp40;void*_tmp41;int*_tmp42;_LLB: _tmp3F=
_tmp3E.f1;if(*((int*)_tmp3F)!= 0)goto _LLD;_tmp40=((struct Cyc_SA_sockaddr_in_struct*)
_tmp3F)->f1;_tmp41=_tmp3E.f2;if(*((int*)_tmp41)!= 1)goto _LLD;_tmp42=((struct Cyc_SA_socklenptr_struct*)
_tmp41)->f1;_LLC: return accept_in(fd,(const struct Cyc_sockaddr_in*)_tmp40,_tmp42);
_LLD:;_LLE:(int)_throw((void*)Cyc_SocketError);_LLA:;}}int bind_in(int,const
struct Cyc_sockaddr_in*,int);int Cyc_bind(int fd,struct _dynforward_ptr ap){if(
_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{
struct _tuple0 _tmp44=({struct _tuple0 _tmp43;_tmp43.f1=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0));_tmp43.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1));_tmp43;});void*_tmp45;struct Cyc_sockaddr_in*
_tmp46;void*_tmp47;int _tmp48;_LL10: _tmp45=_tmp44.f1;if(*((int*)_tmp45)!= 0)goto
_LL12;_tmp46=((struct Cyc_SA_sockaddr_in_struct*)_tmp45)->f1;_tmp47=_tmp44.f2;if(*((
int*)_tmp47)!= 2)goto _LL12;_tmp48=((struct Cyc_SA_socklen_struct*)_tmp47)->f1;
_LL11: return bind_in(fd,(const struct Cyc_sockaddr_in*)_tmp46,_tmp48);_LL12:;_LL13:(
int)_throw((void*)Cyc_SocketError);_LLF:;}}int connect_in(int,const struct Cyc_sockaddr_in*,
int);int Cyc_connect(int fd,struct _dynforward_ptr ap){if(_get_dynforward_size(ap,
sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{struct _tuple0 _tmp4A=({
struct _tuple0 _tmp49;_tmp49.f1=*((void**)_check_dynforward_subscript(ap,sizeof(
void*),0));_tmp49.f2=*((void**)_check_dynforward_subscript(ap,sizeof(void*),1));
_tmp49;});void*_tmp4B;struct Cyc_sockaddr_in*_tmp4C;void*_tmp4D;int _tmp4E;_LL15:
_tmp4B=_tmp4A.f1;if(*((int*)_tmp4B)!= 0)goto _LL17;_tmp4C=((struct Cyc_SA_sockaddr_in_struct*)
_tmp4B)->f1;_tmp4D=_tmp4A.f2;if(*((int*)_tmp4D)!= 2)goto _LL17;_tmp4E=((struct Cyc_SA_socklen_struct*)
_tmp4D)->f1;_LL16: return connect_in(fd,(const struct Cyc_sockaddr_in*)_tmp4C,_tmp4E);
_LL17:;_LL18:(int)_throw((void*)Cyc_SocketError);_LL14:;}}int getpeername_in(int,
const struct Cyc_sockaddr_in*,int*);int Cyc_getpeername(int fd,struct _dynforward_ptr
ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{
struct _tuple0 _tmp50=({struct _tuple0 _tmp4F;_tmp4F.f1=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0));_tmp4F.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1));_tmp4F;});void*_tmp51;struct Cyc_sockaddr_in*
_tmp52;void*_tmp53;int*_tmp54;_LL1A: _tmp51=_tmp50.f1;if(*((int*)_tmp51)!= 0)goto
_LL1C;_tmp52=((struct Cyc_SA_sockaddr_in_struct*)_tmp51)->f1;_tmp53=_tmp50.f2;if(*((
int*)_tmp53)!= 1)goto _LL1C;_tmp54=((struct Cyc_SA_socklenptr_struct*)_tmp53)->f1;
_LL1B: return getpeername_in(fd,(const struct Cyc_sockaddr_in*)_tmp52,_tmp54);_LL1C:;
_LL1D:(int)_throw((void*)Cyc_SocketError);_LL19:;}}int getsockname_in(int,const
struct Cyc_sockaddr_in*,int*);int Cyc_getsockname(int fd,struct _dynforward_ptr ap){
if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{
struct _tuple0 _tmp56=({struct _tuple0 _tmp55;_tmp55.f1=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0));_tmp55.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1));_tmp55;});void*_tmp57;struct Cyc_sockaddr_in*
_tmp58;void*_tmp59;int*_tmp5A;_LL1F: _tmp57=_tmp56.f1;if(*((int*)_tmp57)!= 0)goto
_LL21;_tmp58=((struct Cyc_SA_sockaddr_in_struct*)_tmp57)->f1;_tmp59=_tmp56.f2;if(*((
int*)_tmp59)!= 1)goto _LL21;_tmp5A=((struct Cyc_SA_socklenptr_struct*)_tmp59)->f1;
_LL20: return getsockname_in(fd,(const struct Cyc_sockaddr_in*)_tmp58,_tmp5A);_LL21:;
_LL22:(int)_throw((void*)Cyc_SocketError);_LL1E:;}}int getsockopt_int(int,int,int,
int*,int*);int getsockopt_timeval(int,int,int,struct Cyc_timeval*,int*);int Cyc_getsockopt(
int fd,int level,int optname,struct _dynforward_ptr ap){if(_get_dynforward_size(ap,
sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{struct _tuple0 _tmp5C=({
struct _tuple0 _tmp5B;_tmp5B.f1=*((void**)_check_dynforward_subscript(ap,sizeof(
void*),0));_tmp5B.f2=*((void**)_check_dynforward_subscript(ap,sizeof(void*),1));
_tmp5B;});void*_tmp5D;int*_tmp5E;void*_tmp5F;int*_tmp60;void*_tmp61;int*_tmp62;
void*_tmp63;int*_tmp64;void*_tmp65;struct Cyc_timeval*_tmp66;void*_tmp67;int*
_tmp68;void*_tmp69;struct Cyc_timeval*_tmp6A;void*_tmp6B;int*_tmp6C;_LL24: _tmp5D=
_tmp5C.f1;if(*((int*)_tmp5D)!= 0)goto _LL26;_tmp5E=((struct Cyc_SO_int_struct*)
_tmp5D)->f1;_tmp5F=_tmp5C.f2;if(*((int*)_tmp5F)!= 2)goto _LL26;_tmp60=((struct Cyc_SO_socklenptr_struct*)
_tmp5F)->f1;_LL25: return getsockopt_int(fd,level,optname,_tmp5E,_tmp60);_LL26:
_tmp61=_tmp5C.f1;if(*((int*)_tmp61)!= 0)goto _LL28;_tmp62=((struct Cyc_SO_int_struct*)
_tmp61)->f1;_tmp63=_tmp5C.f2;if(*((int*)_tmp63)!= 0)goto _LL28;_tmp64=((struct Cyc_SO_int_struct*)
_tmp63)->f1;_LL27: return getsockopt_int(fd,level,optname,_tmp62,(int*)_tmp64);
_LL28: _tmp65=_tmp5C.f1;if(*((int*)_tmp65)!= 1)goto _LL2A;_tmp66=((struct Cyc_SO_timeval_struct*)
_tmp65)->f1;_tmp67=_tmp5C.f2;if(*((int*)_tmp67)!= 2)goto _LL2A;_tmp68=((struct Cyc_SO_socklenptr_struct*)
_tmp67)->f1;_LL29: return getsockopt_timeval(fd,level,optname,_tmp66,_tmp68);_LL2A:
_tmp69=_tmp5C.f1;if(*((int*)_tmp69)!= 1)goto _LL2C;_tmp6A=((struct Cyc_SO_timeval_struct*)
_tmp69)->f1;_tmp6B=_tmp5C.f2;if(*((int*)_tmp6B)!= 0)goto _LL2C;_tmp6C=((struct Cyc_SO_int_struct*)
_tmp6B)->f1;_LL2B: return getsockopt_timeval(fd,level,optname,_tmp6A,(int*)_tmp6C);
_LL2C:;_LL2D:(int)_throw((void*)Cyc_SocketError);_LL23:;}}long recv_wrapped(int,
struct _dynforward_ptr,unsigned int,int);long Cyc_recv(int fd,struct _dynforward_ptr
buf,unsigned int n,int flags){return recv_wrapped(fd,buf,n,flags);}long recvfrom_in(
int,struct _dynforward_ptr,unsigned int,int,const struct Cyc_sockaddr_in*,int*);
long Cyc_recvfrom(int fd,struct _dynforward_ptr buf,unsigned int n,int flags,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmp6E=({struct _tuple0 _tmp6D;_tmp6D.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0));_tmp6D.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1));_tmp6D;});void*_tmp6F;struct Cyc_sockaddr_in*
_tmp70;void*_tmp71;int*_tmp72;_LL2F: _tmp6F=_tmp6E.f1;if(*((int*)_tmp6F)!= 0)goto
_LL31;_tmp70=((struct Cyc_SA_sockaddr_in_struct*)_tmp6F)->f1;_tmp71=_tmp6E.f2;if(*((
int*)_tmp71)!= 1)goto _LL31;_tmp72=((struct Cyc_SA_socklenptr_struct*)_tmp71)->f1;
_LL30: return recvfrom_in(fd,buf,n,flags,(const struct Cyc_sockaddr_in*)_tmp70,
_tmp72);_LL31:;_LL32:(int)_throw((void*)Cyc_SocketError);_LL2E:;}}long
send_wrapped(int,struct _dynforward_ptr,unsigned int,int);long Cyc_send(int fd,
struct _dynforward_ptr buf,unsigned int n,int flags){return send_wrapped(fd,buf,n,
flags);}long sendto_in(int,struct _dynforward_ptr,unsigned int,int,const struct Cyc_sockaddr_in*,
int);long Cyc_sendto(int fd,struct _dynforward_ptr buf,unsigned int n,int flags,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmp74=({struct _tuple0 _tmp73;_tmp73.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0));_tmp73.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1));_tmp73;});void*_tmp75;struct Cyc_sockaddr_in*
_tmp76;void*_tmp77;int _tmp78;_LL34: _tmp75=_tmp74.f1;if(*((int*)_tmp75)!= 0)goto
_LL36;_tmp76=((struct Cyc_SA_sockaddr_in_struct*)_tmp75)->f1;_tmp77=_tmp74.f2;if(*((
int*)_tmp77)!= 2)goto _LL36;_tmp78=((struct Cyc_SA_socklen_struct*)_tmp77)->f1;
_LL35: return sendto_in(fd,buf,n,flags,(const struct Cyc_sockaddr_in*)_tmp76,_tmp78);
_LL36:;_LL37:(int)_throw((void*)Cyc_SocketError);_LL33:;}}int setsockopt_int(int,
int,int,const int*,int);int setsockopt_timeval(int,int,int,const struct Cyc_timeval*,
int);int Cyc_setsockopt(int fd,int level,int optname,struct _dynforward_ptr ap){if(
_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{
struct _tuple0 _tmp7A=({struct _tuple0 _tmp79;_tmp79.f1=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0));_tmp79.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1));_tmp79;});void*_tmp7B;int*
_tmp7C;void*_tmp7D;int _tmp7E;void*_tmp7F;struct Cyc_timeval*_tmp80;void*_tmp81;
int _tmp82;_LL39: _tmp7B=_tmp7A.f1;if(*((int*)_tmp7B)!= 0)goto _LL3B;_tmp7C=((
struct Cyc_SO_int_struct*)_tmp7B)->f1;_tmp7D=_tmp7A.f2;if(*((int*)_tmp7D)!= 3)
goto _LL3B;_tmp7E=((struct Cyc_SO_socklen_struct*)_tmp7D)->f1;_LL3A: return
setsockopt_int(fd,level,optname,(const int*)_tmp7C,_tmp7E);_LL3B: _tmp7F=_tmp7A.f1;
if(*((int*)_tmp7F)!= 1)goto _LL3D;_tmp80=((struct Cyc_SO_timeval_struct*)_tmp7F)->f1;
_tmp81=_tmp7A.f2;if(*((int*)_tmp81)!= 3)goto _LL3D;_tmp82=((struct Cyc_SO_socklen_struct*)
_tmp81)->f1;_LL3C: return setsockopt_timeval(fd,level,optname,(const struct Cyc_timeval*)
_tmp80,_tmp82);_LL3D:;_LL3E:(int)_throw((void*)Cyc_SocketError);_LL38:;}}struct
Cyc_stat{short st_dev;unsigned long st_ino;int st_mode;unsigned short st_nlink;
unsigned short __st_uid16;unsigned short __st_gid16;short st_rdev;long __st_size32;
long st_atime;unsigned long __st_uid32;long st_mtime;unsigned long __st_gid32;long
st_ctime;long st_spare3;long st_blksize;long st_blocks;long long __st_size64;};
struct Cyc_timezone{int tz_minuteswest;int tz_dsttime;};struct Cyc_itimerval{struct
Cyc_timeval it_interval;struct Cyc_timeval it_value;};int Cyc_gettimeofday(struct Cyc_timeval*,
struct Cyc_timezone*);int Cyc_select(int,struct Cyc__types_fd_set*,struct Cyc__types_fd_set*,
struct Cyc__types_fd_set*,struct Cyc_timeval*);void Cyc_FD_CLR(int,struct Cyc__types_fd_set*);
int Cyc_FD_ISSET(int,struct Cyc__types_fd_set*);void Cyc_FD_SET(int,struct Cyc__types_fd_set*);
void Cyc_FD_ZERO(struct Cyc__types_fd_set*);int gettimeofday(struct Cyc_timeval*,
struct Cyc_timezone*);int Cyc_gettimeofday(struct Cyc_timeval*a,struct Cyc_timezone*
b){return gettimeofday(a,0);}struct Cyc_timeb{long time;unsigned short millitm;short
timezone;short dstflag;};struct Cyc_tms{unsigned long tms_utime;unsigned long
tms_stime;unsigned long tms_cutime;unsigned long tms_cstime;};struct Cyc_sockaddr_un{
unsigned short sun_family;char sun_path[108];};struct Cyc_utsname{char sysname[20];
char nodename[20];char release[20];char version[20];char machine[20];};struct Cyc_itimerspec{
struct Cyc_timespec it_interval;struct Cyc_timespec it_value;};struct Cyc_tm{int
tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int
tm_yday;int tm_isdst;};unsigned int Cyc_strftime(struct _dynforward_ptr,
unsigned int,struct _dynforward_ptr,const struct Cyc_tm*);unsigned int strftime(char*,
unsigned int,const char*,const struct Cyc_tm*);static char _tmp84[27]="fgets: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___strftime_failure={Cyc_Core_Failure,{
_tmp84,_tmp84 + 27}};unsigned int Cyc_strftime(struct _dynforward_ptr s,unsigned int
maxsize,struct _dynforward_ptr fmt,const struct Cyc_tm*t){if(maxsize > 
_get_dynforward_size(s,sizeof(char)))(int)_throw((void*)& Cyc___strftime_failure);
return strftime((char*)_check_null(_untag_dynforward_ptr(s,sizeof(char),1)),
maxsize,(const char*)_check_null(_untag_dynforward_ptr(fmt,sizeof(char),1)),t);}
struct Cyc_option{struct _dynforward_ptr name;int has_arg;int*flag;int val;};int Cyc_execlp(
const char*,const char*,struct _dynforward_ptr);int execvp(const char*file,const char**
argv);struct _dynforward_ptr Cyc_getcwd(struct _dynforward_ptr buf,unsigned int size);
int Cyc_gethostname(struct _dynforward_ptr,unsigned int);long Cyc_read(int,struct
_dynforward_ptr,unsigned int);long Cyc_write(int,struct _dynforward_ptr,
unsigned int);static char _tmp86[36]="execlp arg list not NULL-terminated";static
struct Cyc_Core_Failure_struct Cyc___execlp_failure={Cyc_Core_Failure,{_tmp86,
_tmp86 + 36}};int Cyc_execlp(const char*path,const char*arg0,struct _dynforward_ptr
argv){if(*((const char**)_check_dynforward_subscript(argv,sizeof(const char*),(int)(
_get_dynforward_size(argv,sizeof(const char*))- 1)))!= (const char*)0)(int)_throw((
void*)& Cyc___execlp_failure);{const char**newargs=(const char**)
_untag_dynforward_ptr(({unsigned int _tmp8F=_get_dynforward_size(argv,sizeof(
const char*));const char**_tmp90=(const char**)_cycalloc(_check_times(sizeof(const
char*),_tmp8F + 1));struct _dynforward_ptr _tmp92=_tag_dynforward(_tmp90,sizeof(
const char*),_tmp8F + 1);{unsigned int _tmp91=_tmp8F;unsigned int i;for(i=0;i < 
_tmp91;i ++){_tmp90[i]=(const char*)0;}_tmp90[_tmp91]=(const char*)0;}_tmp92;}),
sizeof(const char*),1);({struct _dyneither_ptr _tmp88=_dyneither_ptr_plus(({const
char**_tmp87=newargs;_tag_dyneither(_tmp87,sizeof(const char*),_get_zero_arr_size(
_tmp87,1));}),sizeof(const char*),0);const char*_tmp89=*((const char**)
_check_dyneither_subscript(_tmp88,sizeof(const char*),0));const char*_tmp8A=(const
char*)arg0;if(_get_dyneither_size(_tmp88,sizeof(const char*))== 1  && (_tmp89 == 0
 && _tmp8A != 0))_throw_arraybounds();*((const char**)_tmp88.curr)=_tmp8A;});{int i=
0;for(0;i < _get_dynforward_size(argv,sizeof(const char*))- 1;i ++){({struct
_dyneither_ptr _tmp8C=_dyneither_ptr_plus(({const char**_tmp8B=newargs;
_tag_dyneither(_tmp8B,sizeof(const char*),_get_zero_arr_size(_tmp8B,1));}),
sizeof(const char*),i + 1);const char*_tmp8D=*((const char**)
_check_dyneither_subscript(_tmp8C,sizeof(const char*),0));const char*_tmp8E=*((
const char**)_check_dynforward_subscript(argv,sizeof(const char*),i));if(
_get_dyneither_size(_tmp8C,sizeof(const char*))== 1  && (_tmp8D == 0  && _tmp8E != 0))
_throw_arraybounds();*((const char**)_tmp8C.curr)=_tmp8E;});}}return execvp(path,(
const char**)newargs);}}char*getcwd(char*buf,unsigned int size);static char _tmp94[
29]="getcwd: invalid buf argument";static struct Cyc_Core_Failure_struct Cyc___getcwd_failure={
Cyc_Core_Failure,{_tmp94,_tmp94 + 29}};struct _dynforward_ptr Cyc_getcwd(struct
_dynforward_ptr buf,unsigned int size){if(_get_dynforward_size(buf,sizeof(char))< 
size)(int)_throw((void*)& Cyc___getcwd_failure);{char*_tmp95=getcwd((char*)
_check_null(_untag_dynforward_ptr(buf,sizeof(char),1)),size);return(unsigned int)
_tmp95?buf: _tag_dynforward(0,0,0);}}int gethostname(char*,unsigned int);static
char _tmp97[34]="gethostname: called with NULL buf";static struct Cyc_Core_Failure_struct
Cyc___gethostname_failure_1={Cyc_Core_Failure,{_tmp97,_tmp97 + 34}};static char
_tmp99[42]="gethostname: called with count > buf.size";static struct Cyc_Core_Failure_struct
Cyc___gethostname_failure_2={Cyc_Core_Failure,{_tmp99,_tmp99 + 42}};int Cyc_gethostname(
struct _dynforward_ptr buf,unsigned int count){if(!((unsigned int)buf.curr))(int)
_throw((void*)& Cyc___gethostname_failure_1);else{if(count > _get_dynforward_size(
buf,sizeof(char)))(int)_throw((void*)& Cyc___gethostname_failure_2);return
gethostname((char*)_untag_dynforward_ptr(buf,sizeof(char),1),count);}}long read(
int fd,char*buf,unsigned int count);static char _tmp9B[35]="read: called with count > buf.size";
static struct Cyc_Core_Failure_struct Cyc___read_failure={Cyc_Core_Failure,{_tmp9B,
_tmp9B + 35}};long Cyc_read(int fd,struct _dynforward_ptr buf,unsigned int count){if(
count > _get_dynforward_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___read_failure);
return read(fd,(char*)_check_null(_untag_dynforward_ptr(buf,sizeof(char),1)),
count);}long write(int fd,const char*buf,unsigned int count);static char _tmp9D[36]="write: called with count > buf.size";
static struct Cyc_Core_Failure_struct Cyc___write_failure={Cyc_Core_Failure,{_tmp9D,
_tmp9D + 36}};long Cyc_write(int fd,struct _dynforward_ptr buf,unsigned int count){if(
count > _get_dynforward_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___write_failure);
return write(fd,(const char*)_untag_dynforward_ptr(buf,sizeof(char),1),count);}
struct Cyc_utimbuf{long actime;long modtime;};
