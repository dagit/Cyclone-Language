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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{
struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct
_dynforward_ptr*name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};
struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
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
struct Cyc_Absyn_String_c_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Null_c_struct{
int tag;};union Cyc_Absyn_Cnst_union{struct Cyc_Absyn_Char_c_struct Char_c;struct Cyc_Absyn_Short_c_struct
Short_c;struct Cyc_Absyn_Int_c_struct Int_c;struct Cyc_Absyn_LongLong_c_struct
LongLong_c;struct Cyc_Absyn_Float_c_struct Float_c;struct Cyc_Absyn_String_c_struct
String_c;struct Cyc_Absyn_Null_c_struct Null_c;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dynforward_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst_union f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct
_tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;
};struct Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct
Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;
struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dynforward_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dynforward_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);void*Cyc_Absyn_union_typ(struct _dynforward_ptr*name);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dynforward_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dynforward_ptr*Cyc_Absyn_fieldname(int);
struct _tuple4{void*f1;struct _tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);void Cyc_Absyn_print_decls(
struct Cyc_List_List*);extern int Cyc_Absyn_porting_c_code;struct Cyc_Typerep_Int_struct{
int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;
unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct
_tuple5{unsigned int f1;struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct
_tuple6{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dynforward_ptr f2;};struct _tuple7{unsigned int f1;struct
_dynforward_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr
f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple8{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,struct
_dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcutil_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);
void Cyc_Marshal_print_type(void*rep,void*val);struct _tuple10{struct Cyc_Dict_Dict
f1;int f2;};struct _tuple11{struct _dynforward_ptr f1;int f2;};static int Cyc_Absyn_strlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return
Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace_union f1;
union Cyc_Absyn_Nmspace_union f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct
_tuple0*q2){union Cyc_Absyn_Nmspace_union _tmp0=(*q1).f1;union Cyc_Absyn_Nmspace_union
_tmp1=(*q2).f1;{struct _tuple12 _tmp3=({struct _tuple12 _tmp2;_tmp2.f1=_tmp0;_tmp2.f2=
_tmp1;_tmp2;});union Cyc_Absyn_Nmspace_union _tmp4;union Cyc_Absyn_Nmspace_union
_tmp5;union Cyc_Absyn_Nmspace_union _tmp6;struct Cyc_List_List*_tmp7;union Cyc_Absyn_Nmspace_union
_tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace_union _tmpA;struct Cyc_List_List*
_tmpB;union Cyc_Absyn_Nmspace_union _tmpC;struct Cyc_List_List*_tmpD;union Cyc_Absyn_Nmspace_union
_tmpE;union Cyc_Absyn_Nmspace_union _tmpF;union Cyc_Absyn_Nmspace_union _tmp10;union
Cyc_Absyn_Nmspace_union _tmp11;_LL1: _tmp4=_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)
goto _LL3;_tmp5=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag != 0)goto _LL3;_LL2: goto _LL0;
_LL3: _tmp6=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)goto _LL5;_tmp7=(_tmp6.Rel_n).f1;
_tmp8=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)goto _LL5;_tmp9=(_tmp8.Rel_n).f1;
_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(((_tmp3.f1).Abs_n).tag
!= 2)goto _LL7;_tmpB=(_tmpA.Abs_n).f1;_tmpC=_tmp3.f2;if(((_tmp3.f2).Abs_n).tag != 
2)goto _LL7;_tmpD=(_tmpC.Abs_n).f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);
if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag != 0)goto
_LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)goto _LLD;
_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)goto _LL0;_LLE:
return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity
!= 0  && tv2->identity != 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
union Cyc_Absyn_Nmspace_union Cyc_Absyn_rel_ns_null=(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Rel_n_struct){1,0});int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){union Cyc_Absyn_Nmspace_union _tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;
struct Cyc_List_List*_tmp15;_LL10: if((_tmp13.Rel_n).tag != 1)goto _LL12;_tmp14=(
_tmp13.Rel_n).f1;if(_tmp14 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if((_tmp13.Abs_n).tag
!= 2)goto _LL14;_tmp15=(_tmp13.Abs_n).f1;if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;
_LL14: if((_tmp13.Loc_n).tag != 0)goto _LL16;_LL15: return 0;_LL16:;_LL17: return 1;
_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*env){return(void*)({struct Cyc_Absyn_Evar_struct*_tmp16=
_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=
0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;
_tmp17;});_tmp16;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(void*)((void*)1)};
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp18;_tmp18.print_const=x.print_const  || y.print_const;_tmp18.q_volatile=x.q_volatile
 || y.q_volatile;_tmp18.q_restrict=x.q_restrict  || y.q_restrict;_tmp18.real_const=
x.real_const  || y.real_const;_tmp18.loc=Cyc_Position_segment_join(x.loc,y.loc);
_tmp18;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.print_const=0;_tmp19.q_volatile=
0;_tmp19.q_restrict=0;_tmp19.real_const=0;_tmp19.loc=loc;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp1A;_tmp1A.print_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A.real_const=
1;_tmp1A.loc=loc;_tmp1A;});}static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)1),
1})};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){return({struct Cyc_Absyn_Conref*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp1F;(_tmp1F.Eq_constr).tag=0;(_tmp1F.Eq_constr).f1=(
void*)x;_tmp1F;});_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp21;(_tmp21.No_constr).tag=
2;_tmp21;});_tmp20;});}static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)1})};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)0})};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)& Cyc_Absyn_one_bt)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dynforward_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)0)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref=& Cyc_Absyn_bounds_dynforward_conref_v;
static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dyneither_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)((void*)1)})};struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union
_tmp27=x->v;struct Cyc_Absyn_Conref*_tmp28;_LL19: if((_tmp27.No_constr).tag != 2)
goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((_tmp27.Eq_constr).tag != 0)goto _LL1D;_LL1C:
return x;_LL1D: if((_tmp27.Forward_constr).tag != 1)goto _LL18;_tmp28=(_tmp27.Forward_constr).f1;
_LL1E: {struct Cyc_Absyn_Conref*_tmp29=Cyc_Absyn_compress_conref(_tmp28);x->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp2A;(_tmp2A.Forward_constr).tag=
1;(_tmp2A.Forward_constr).f1=_tmp29;_tmp2A;});return _tmp29;}_LL18:;}void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union _tmp2B=(Cyc_Absyn_compress_conref(
x))->v;void*_tmp2C;_LL20: if((_tmp2B.Eq_constr).tag != 0)goto _LL22;_tmp2C=(_tmp2B.Eq_constr).f1;
_LL21: return _tmp2C;_LL22:;_LL23:({void*_tmp2D[0]={};Cyc_Tcutil_impos(({const char*
_tmp2E="conref_val";_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(
_tmp2E,11));}),_tag_dynforward(_tmp2D,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union _tmp2F=(Cyc_Absyn_compress_conref(
x))->v;void*_tmp30;_LL25: if((_tmp2F.Eq_constr).tag != 0)goto _LL27;_tmp30=(_tmp2F.Eq_constr).f1;
_LL26: return _tmp30;_LL27:;_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*
k){void*_tmp31=k;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt*
_tmp34;struct Cyc_Core_Opt _tmp35;void*_tmp36;void**_tmp37;struct Cyc_Core_Opt*
_tmp38;struct Cyc_Core_Opt _tmp39;void*_tmp3A;void**_tmp3B;_LL2A: if(*((int*)_tmp31)
!= 0)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if(*((int*)_tmp31)!= 1)goto _LL2E;_tmp32=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp31)->f1;if(_tmp32 != 0)goto _LL2E;_LL2D:
goto _LL2F;_LL2E: if(*((int*)_tmp31)!= 2)goto _LL30;_tmp33=((struct Cyc_Absyn_Less_kb_struct*)
_tmp31)->f1;if(_tmp33 != 0)goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp31)!= 1)
goto _LL32;_tmp34=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp31)->f1;if(_tmp34 == 0)
goto _LL32;_tmp35=*_tmp34;_tmp36=(void*)_tmp35.v;_tmp37=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp31)->f1).v;_LL31: _tmp3B=_tmp37;goto _LL33;_LL32: if(*((int*)_tmp31)!= 2)goto
_LL29;_tmp38=((struct Cyc_Absyn_Less_kb_struct*)_tmp31)->f1;if(_tmp38 == 0)goto
_LL29;_tmp39=*_tmp38;_tmp3A=(void*)_tmp39.v;_tmp3B=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp31)->f1).v;_LL33:*_tmp3B=Cyc_Absyn_compress_kb(*_tmp3B);return*_tmp3B;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3C=Cyc_Absyn_compress_kb(kb);void*
_tmp3D;struct Cyc_Core_Opt*_tmp3E;struct Cyc_Core_Opt**_tmp3F;struct Cyc_Core_Opt*
_tmp40;struct Cyc_Core_Opt**_tmp41;void*_tmp42;_LL35: if(*((int*)_tmp3C)!= 0)goto
_LL37;_tmp3D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3C)->f1;_LL36: return
_tmp3D;_LL37: if(*((int*)_tmp3C)!= 1)goto _LL39;_tmp3E=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3C)->f1;_tmp3F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3C)->f1;_LL38: _tmp41=_tmp3F;_tmp42=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3C)!= 2)goto _LL34;_tmp40=((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f1;
_tmp41=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f1;
_tmp42=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f2;_LL3A:*_tmp41=({
struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(void*)_tmp42;_tmp45;});
_tmp44;}));_tmp43;});return _tmp42;_LL34:;}static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_char_tt={5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={5,(void*)((void*)1),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={5,(void*)((void*)1),(void*)((void*)4)};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_schar_tt={5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sint_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slong_tt={5,(void*)((void*)0),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={5,(void*)((void*)0),(void*)((void*)4)};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp51=_cycalloc_atomic(sizeof(*_tmp51));
_tmp51[0]=({struct Cyc_Absyn_DoubleType_struct _tmp52;_tmp52.tag=6;_tmp52.f1=b;
_tmp52;});_tmp51;});}static char _tmp53[4]="exn";static struct _dynforward_ptr Cyc_Absyn_exn_str={
_tmp53,_tmp53 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=&
Cyc_Absyn_exn_name_v;static char _tmp55[15]="Null_Exception";static struct
_dynforward_ptr Cyc_Absyn_Null_Exception_str={_tmp55,_tmp55 + 15};static struct
_tuple0 Cyc_Absyn_Null_Exception_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp57[13]="Array_bounds";
static struct _dynforward_ptr Cyc_Absyn_Array_bounds_str={_tmp57,_tmp57 + 13};static
struct _tuple0 Cyc_Absyn_Array_bounds_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=&
Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={&
Cyc_Absyn_Array_bounds_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp59[16]="Match_Exception";static struct
_dynforward_ptr Cyc_Absyn_Match_Exception_str={_tmp59,_tmp59 + 16};static struct
_tuple0 Cyc_Absyn_Match_Exception_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp5B[
10]="Bad_alloc";static struct _dynforward_ptr Cyc_Absyn_Bad_alloc_str={_tmp5B,
_tmp5B + 10};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=&
Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={&
Cyc_Absyn_Bad_alloc_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=&
Cyc_Absyn_Bad_alloc_tuf_v;static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1,0};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};static struct Cyc_Absyn_TunionType_struct
Cyc_Absyn_exn_typ_tt={2,{(union Cyc_Absyn_TunionInfoU_union)((struct Cyc_Absyn_KnownTunion_struct){
1,& Cyc_Absyn_exn_tud}),0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp5F[9]="PrintArg";static struct
_dynforward_ptr Cyc_Absyn_printarg_str={_tmp5F,_tmp5F + 9};static char _tmp60[9]="ScanfArg";
static struct _dynforward_ptr Cyc_Absyn_scanfarg_str={_tmp60,_tmp60 + 9};static
struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_printarg_str};static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=t;
_tmp63;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=t;_tmp64;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({
struct Cyc_Absyn_PointerType_struct _tmp66;_tmp66.tag=4;_tmp66.f1=({struct Cyc_Absyn_PtrInfo
_tmp67;_tmp67.elt_typ=(void*)t;_tmp67.elt_tq=tq;_tmp67.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp68;_tmp68.rgn=(void*)r;_tmp68.nullable=Cyc_Absyn_true_conref;_tmp68.bounds=
Cyc_Absyn_new_conref(b);_tmp68.zero_term=zeroterm;_tmp68.ptrloc=0;_tmp68;});
_tmp67;});_tmp66;});_tmp65;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6A;_tmp6A.tag=4;_tmp6A.f1=({struct Cyc_Absyn_PtrInfo _tmp6B;_tmp6B.elt_typ=(
void*)t;_tmp6B.elt_tq=tq;_tmp6B.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6C;_tmp6C.rgn=(
void*)r;_tmp6C.nullable=Cyc_Absyn_false_conref;_tmp6C.bounds=Cyc_Absyn_new_conref(
b);_tmp6C.zero_term=zeroterm;_tmp6C.ptrloc=0;_tmp6C;});_tmp6B;});_tmp6A;});
_tmp69;});}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6E;_tmp6E.tag=4;_tmp6E.f1=({struct Cyc_Absyn_PtrInfo _tmp6F;_tmp6F.elt_typ=(
void*)t;_tmp6F.elt_tq=tq;_tmp6F.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp70;_tmp70.rgn=(
void*)r;_tmp70.nullable=Cyc_Absyn_true_conref;_tmp70.bounds=Cyc_Absyn_bounds_one_conref;
_tmp70.zero_term=zeroterm;_tmp70.ptrloc=0;_tmp70;});_tmp6F;});_tmp6E;});_tmp6D;});}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({
struct Cyc_Absyn_PointerType_struct _tmp72;_tmp72.tag=4;_tmp72.f1=({struct Cyc_Absyn_PtrInfo
_tmp73;_tmp73.elt_typ=(void*)t;_tmp73.elt_tq=tq;_tmp73.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp74;_tmp74.rgn=(void*)((void*)2);_tmp74.nullable=Cyc_Absyn_true_conref;_tmp74.bounds=
Cyc_Absyn_bounds_one_conref;_tmp74.zero_term=Cyc_Absyn_false_conref;_tmp74.ptrloc=
0;_tmp74;});_tmp73;});_tmp72;});_tmp71;});}void*Cyc_Absyn_at_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({
struct Cyc_Absyn_PointerType_struct _tmp76;_tmp76.tag=4;_tmp76.f1=({struct Cyc_Absyn_PtrInfo
_tmp77;_tmp77.elt_typ=(void*)t;_tmp77.elt_tq=tq;_tmp77.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp78;_tmp78.rgn=(void*)r;_tmp78.nullable=Cyc_Absyn_false_conref;_tmp78.bounds=
Cyc_Absyn_bounds_one_conref;_tmp78.zero_term=zeroterm;_tmp78.ptrloc=0;_tmp78;});
_tmp77;});_tmp76;});_tmp75;});}void*Cyc_Absyn_dynforward_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({
struct Cyc_Absyn_PointerType_struct _tmp7A;_tmp7A.tag=4;_tmp7A.f1=({struct Cyc_Absyn_PtrInfo
_tmp7B;_tmp7B.elt_typ=(void*)t;_tmp7B.elt_tq=tq;_tmp7B.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7C;_tmp7C.rgn=(void*)r;_tmp7C.nullable=Cyc_Absyn_true_conref;_tmp7C.bounds=
Cyc_Absyn_bounds_dynforward_conref;_tmp7C.zero_term=zeroterm;_tmp7C.ptrloc=0;
_tmp7C;});_tmp7B;});_tmp7A;});_tmp79;});}void*Cyc_Absyn_dyneither_typ(void*t,
void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({
struct Cyc_Absyn_PointerType_struct _tmp7E;_tmp7E.tag=4;_tmp7E.f1=({struct Cyc_Absyn_PtrInfo
_tmp7F;_tmp7F.elt_typ=(void*)t;_tmp7F.elt_tq=tq;_tmp7F.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp80;_tmp80.rgn=(void*)r;_tmp80.nullable=Cyc_Absyn_true_conref;_tmp80.bounds=
Cyc_Absyn_bounds_dyneither_conref;_tmp80.zero_term=zeroterm;_tmp80.ptrloc=0;
_tmp80;});_tmp7F;});_tmp7E;});_tmp7D;});}void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp82;_tmp82.tag=7;_tmp82.f1=({struct Cyc_Absyn_ArrayInfo _tmp83;_tmp83.elt_type=(
void*)elt_type;_tmp83.tq=tq;_tmp83.num_elts=num_elts;_tmp83.zero_term=zero_term;
_tmp83.zt_loc=ztloc;_tmp83;});_tmp82;});_tmp81;});}static char _tmp8D[8]="__sFILE";
void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;static struct _dynforward_ptr
sf_str={_tmp8D,_tmp8D + 8};static struct _dynforward_ptr*sf=& sf_str;if(file_t_opt == 
0){struct _tuple0*file_t_name=({struct _tuple0*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8C;(
_tmp8C.Abs_n).tag=2;(_tmp8C.Abs_n).f1=0;_tmp8C;});_tmp8B->f2=sf;_tmp8B;});struct
Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A->kind=(void*)((void*)0);_tmp8A->sc=(void*)((void*)1);_tmp8A->name=
file_t_name;_tmp8A->tvs=0;_tmp8A->impl=0;_tmp8A->attributes=0;_tmp8A;});void*
file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp85=_cycalloc(
sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_AggrType_struct _tmp86;_tmp86.tag=10;
_tmp86.f1=({struct Cyc_Absyn_AggrInfo _tmp87;_tmp87.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp88;(_tmp88.KnownAggr).tag=1;(_tmp88.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=sd;_tmp89;});
_tmp88;});_tmp87.targs=0;_tmp87;});_tmp86;});_tmp85;});file_t_opt=({void**_tmp84=
_cycalloc(sizeof(*_tmp84));_tmp84[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp84;});}return*file_t_opt;}
void*Cyc_Absyn_void_star_typ(){static void**void_star_t_opt=0;if(void_star_t_opt
== 0)void_star_t_opt=({void**_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=Cyc_Absyn_star_typ((
void*)0,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp8E;});
return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
21,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dynforward_ptr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_AggrType_struct
_tmp91;_tmp91.tag=10;_tmp91.f1=({struct Cyc_Absyn_AggrInfo _tmp92;_tmp92.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp93;(_tmp93.UnknownAggr).tag=
0;(_tmp93.UnknownAggr).f1=(void*)k;(_tmp93.UnknownAggr).f2=({struct _tuple0*
_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=Cyc_Absyn_rel_ns_null;_tmp94->f2=
name;_tmp94;});_tmp93;});_tmp92.targs=0;_tmp92;});_tmp91;});_tmp90;});}void*Cyc_Absyn_strct(
struct _dynforward_ptr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _dynforward_ptr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp95=
_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_AggrType_struct _tmp96;
_tmp96.tag=10;_tmp96.f1=({struct Cyc_Absyn_AggrInfo _tmp97;_tmp97.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp98;(_tmp98.UnknownAggr).tag=
0;(_tmp98.UnknownAggr).f1=(void*)((void*)0);(_tmp98.UnknownAggr).f2=name;_tmp98;});
_tmp97.targs=0;_tmp97;});_tmp96;});_tmp95;});}void*Cyc_Absyn_unionq_typ(struct
_tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp99=_cycalloc(
sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_AggrType_struct _tmp9A;_tmp9A.tag=10;
_tmp9A.f1=({struct Cyc_Absyn_AggrInfo _tmp9B;_tmp9B.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp9C;(_tmp9C.UnknownAggr).tag=0;(_tmp9C.UnknownAggr).f1=(
void*)((void*)1);(_tmp9C.UnknownAggr).f2=name;_tmp9C;});_tmp9B.targs=0;_tmp9B;});
_tmp9A;});_tmp99;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Exp*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->topt=
0;_tmp9D->r=(void*)r;_tmp9D->loc=loc;_tmp9D->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);
_tmp9D;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_New_e_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_Absyn_New_e_struct _tmp9F;_tmp9F.tag=17;_tmp9F.f1=rgn_handle;_tmp9F.f2=
e;_tmp9F;});_tmp9E;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*
e){return({struct Cyc_Absyn_Exp*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=*e;
_tmpA0;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_Const_e_struct
_tmpA2;_tmpA2.tag=0;_tmpA2.f1=c;_tmpA2;});_tmpA1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({
struct Cyc_Absyn_Const_e_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA5;(_tmpA5.Null_c).tag=6;_tmpA5;});_tmpA4;});_tmpA3;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpA6;(_tmpA6.Int_c).tag=2;(_tmpA6.Int_c).f1=(void*)s;(_tmpA6.Int_c).f2=i;
_tmpA6;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)1,(
int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(0,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpA7;(_tmpA7.Char_c).tag=0;(_tmpA7.Char_c).f1=(void*)((void*)2);(_tmpA7.Char_c).f2=
c;_tmpA7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA8;(_tmpA8.Float_c).tag=4;(_tmpA8.Float_c).f1=f;
_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA9;(_tmpA9.String_c).tag=5;(_tmpA9.String_c).f1=s;
_tmpA9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_Var_e_struct _tmpAB;_tmpAB.tag=
1;_tmpAB.f1=q;_tmpAB.f2=(void*)((void*)0);_tmpAB;});_tmpAA;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAC=_cycalloc(sizeof(*
_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_Var_e_struct _tmpAD;_tmpAD.tag=1;_tmpAD.f1=q;
_tmpAD.f2=(void*)b;_tmpAD;});_tmpAC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpAF;_tmpAF.tag=2;_tmpAF.f1=q;_tmpAF;});
_tmpAE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpB1;_tmpB1.tag=3;_tmpB1.f1=(void*)p;_tmpB1.f2=es;_tmpB1;});_tmpB0;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*
_tmpB2));_tmpB2->hd=e;_tmpB2->tl=0;_tmpB2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3->hd=e1;_tmpB3->tl=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*
_tmpB4));_tmpB4->hd=e2;_tmpB4->tl=0;_tmpB4;});_tmpB3;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpB6;
_tmpB6.tag=4;_tmpB6.f1=e1;_tmpB6.f2=popt;_tmpB6.f3=e2;_tmpB6;});_tmpB5;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpB8;_tmpB8.tag=5;_tmpB8.f1=e;_tmpB8.f2=(void*)i;_tmpB8;});_tmpB7;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpBA;_tmpBA.tag=6;_tmpBA.f1=e1;_tmpBA.f2=e2;_tmpBA.f3=e3;_tmpBA;});_tmpB9;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_And_e_struct _tmpBC;
_tmpBC.tag=7;_tmpBC.f1=e1;_tmpBC.f2=e2;_tmpBC;});_tmpBB;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpBD=
_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_Or_e_struct _tmpBE;_tmpBE.tag=
8;_tmpBE.f1=e1;_tmpBE.f2=e2;_tmpBE;});_tmpBD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpC0;
_tmpC0.tag=9;_tmpC0.f1=e1;_tmpC0.f2=e2;_tmpC0;});_tmpBF;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpC2;_tmpC2.tag=10;_tmpC2.f1=e;_tmpC2.f2=es;_tmpC2;});_tmpC1;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpC4;_tmpC4.tag=11;_tmpC4.f1=e;_tmpC4.f2=es;_tmpC4.f3=0;_tmpC4;});_tmpC3;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpC6;_tmpC6.tag=13;_tmpC6.f1=e;_tmpC6;});_tmpC5;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC8;_tmpC8.tag=14;_tmpC8.f1=e;_tmpC8.f2=ts;_tmpC8;});_tmpC7;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpCA;_tmpCA.tag=15;_tmpCA.f1=(void*)t;_tmpCA.f2=e;_tmpCA.f3=user_cast;_tmpCA.f4=(
void*)c;_tmpCA;});_tmpC9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpCC;_tmpCC.tag=12;_tmpCC.f1=e;_tmpCC;});_tmpCB;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpCD=
_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Address_e_struct _tmpCE;
_tmpCE.tag=16;_tmpCE.f1=e;_tmpCE;});_tmpCD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpD0;_tmpD0.tag=18;_tmpD0.f1=(void*)t;_tmpD0;});
_tmpCF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpD2;_tmpD2.tag=19;_tmpD2.f1=e;_tmpD2;});_tmpD1;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));
_tmpD3[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpD4;_tmpD4.tag=20;_tmpD4.f1=(
void*)t;_tmpD4.f2=(void*)of;_tmpD4;});_tmpD3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpD6;_tmpD6.tag=21;_tmpD6.f1=tvs;_tmpD6.f2=(
void*)t;_tmpD6;});_tmpD5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD8;_tmpD8.tag=22;_tmpD8.f1=e;_tmpD8;});_tmpD7;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpDA;_tmpDA.tag=23;_tmpDA.f1=e;_tmpDA.f2=n;
_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));
_tmpDB[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpDC;_tmpDC.tag=24;_tmpDC.f1=e;
_tmpDC.f2=n;_tmpDC;});_tmpDB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpDD=
_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpDE;
_tmpDE.tag=25;_tmpDE.f1=e1;_tmpDE.f2=e2;_tmpDE;});_tmpDD;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpDF=_cycalloc(
sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpE0;_tmpE0.tag=26;
_tmpE0.f1=es;_tmpE0;});_tmpDF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));
_tmpE1[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpE2;_tmpE2.tag=38;_tmpE2.f1=s;
_tmpE2;});_tmpE1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*
_tmpE3));_tmpE3->hd=({struct _tuple13*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->f1=
0;_tmpE4->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE4;});_tmpE3->tl=dles;_tmpE3;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpE5=_cycalloc(
sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_Array_e_struct _tmpE6;_tmpE6.tag=28;
_tmpE6.f1=dles;_tmpE6;});_tmpE5;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpE7=
_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpE8;_tmpE8.tag=37;_tmpE8.f1=n;_tmpE8.f2=dles;_tmpE8;});_tmpE7;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->r=(void*)s;_tmpE9->loc=
loc;_tmpE9->non_local_preds=0;_tmpE9->try_depth=0;_tmpE9->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpE9;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpEA=_cycalloc(
sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_Exp_s_struct _tmpEB;_tmpEB.tag=0;
_tmpEB.f1=e;_tmpEB;});_tmpEA;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpEC=
_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Return_s_struct _tmpED;
_tmpED.tag=2;_tmpED.f1=e;_tmpED;});_tmpEC;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpEF;_tmpEF.tag=3;_tmpEF.f1=e;_tmpEF.f2=s1;_tmpEF.f3=s2;_tmpEF;});_tmpEE;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_While_s_struct
_tmpF1;_tmpF1.tag=4;_tmpF1.f1=({struct _tuple2 _tmpF2;_tmpF2.f1=e;_tmpF2.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpF2;});_tmpF1.f2=s;_tmpF1;});_tmpF0;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_Break_s_struct
_tmpF4;_tmpF4.tag=5;_tmpF4.f1=0;_tmpF4;});_tmpF3;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));
_tmpF5[0]=({struct Cyc_Absyn_Continue_s_struct _tmpF6;_tmpF6.tag=6;_tmpF6.f1=0;
_tmpF6;});_tmpF5;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_For_s_struct _tmpF8;
_tmpF8.tag=8;_tmpF8.f1=e1;_tmpF8.f2=({struct _tuple2 _tmpF9;_tmpF9.f1=e2;_tmpF9.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpF9;});_tmpF8.f3=({struct _tuple2 _tmpFA;_tmpFA.f1=
e3;_tmpFA.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpFA;});_tmpF8.f4=s;_tmpF8;});_tmpF7;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpFC;_tmpFC.tag=9;_tmpFC.f1=e;_tmpFC.f2=scs;_tmpFC;});_tmpFB;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpFE=({struct _tuple9 _tmpFD;
_tmpFD.f1=(void*)s1->r;_tmpFD.f2=(void*)s2->r;_tmpFD;});void*_tmpFF;void*_tmp100;
_LL3C: _tmpFF=_tmpFE.f1;if((int)_tmpFF != 0)goto _LL3E;_LL3D: return s2;_LL3E: _tmp100=
_tmpFE.f2;if((int)_tmp100 != 0)goto _LL40;_LL3F: return s1;_LL40:;_LL41: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[
0]=({struct Cyc_Absyn_Seq_s_struct _tmp102;_tmp102.tag=1;_tmp102.f1=s1;_tmp102.f2=
s2;_tmp102;});_tmp101;}),loc);_LL3B:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Fallthru_s_struct*_tmp103=_cycalloc(sizeof(*_tmp103));
_tmp103[0]=({struct Cyc_Absyn_Fallthru_s_struct _tmp104;_tmp104.tag=10;_tmp104.f1=
el;_tmp104.f2=0;_tmp104;});_tmp103;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*_tmp105=
_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Decl_s_struct _tmp106;
_tmp106.tag=11;_tmp106.f1=d;_tmp106.f2=s;_tmp106;});_tmp105;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[
0]=({struct Cyc_Absyn_Var_d_struct _tmp10A;_tmp10A.tag=0;_tmp10A.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmp10A;});_tmp109;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp108;_tmp108.tag=11;_tmp108.f1=d;_tmp108.f2=s;_tmp108;});_tmp107;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_Label_s_struct
_tmp10C;_tmp10C.tag=12;_tmp10C.f1=v;_tmp10C.f2=s;_tmp10C;});_tmp10B;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_Do_s_struct
_tmp10E;_tmp10E.tag=13;_tmp10E.f1=s;_tmp10E.f2=({struct _tuple2 _tmp10F;_tmp10F.f1=
e;_tmp10F.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp10F;});_tmp10E;});_tmp10D;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp111;_tmp111.tag=14;_tmp111.f1=s;_tmp111.f2=scs;_tmp111;});_tmp110;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp112=
_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_Goto_s_struct _tmp113;
_tmp113.tag=7;_tmp113.f1=lab;_tmp113.f2=0;_tmp113;});_tmp112;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->r=(void*)p;_tmp114->topt=0;_tmp114->loc=
s;_tmp114;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp115=_cycalloc(
sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Exp_p_struct _tmp116;_tmp116.tag=
14;_tmp116.f1=e;_tmp116;});_tmp115;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp117=
_cycalloc(sizeof(*_tmp117));_tmp117->r=(void*)r;_tmp117->loc=loc;_tmp117;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Let_d_struct
_tmp119;_tmp119.tag=2;_tmp119.f1=p;_tmp119.f2=0;_tmp119.f3=e;_tmp119;});_tmp118;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_Letv_d_struct _tmp11B;
_tmp11B.tag=3;_tmp11B.f1=vds;_tmp11B;});_tmp11A;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->sc=(void*)((
void*)2);_tmp11C->name=x;_tmp11C->tq=Cyc_Absyn_empty_tqual(0);_tmp11C->type=(
void*)t;_tmp11C->initializer=init;_tmp11C->rgn=0;_tmp11C->attributes=0;_tmp11C->escapes=
0;_tmp11C;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp11D=
_cycalloc(sizeof(*_tmp11D));_tmp11D->sc=(void*)((void*)0);_tmp11D->name=x;
_tmp11D->tq=Cyc_Absyn_empty_tqual(0);_tmp11D->type=(void*)t;_tmp11D->initializer=
init;_tmp11D->rgn=0;_tmp11D->attributes=0;_tmp11D->escapes=0;_tmp11D;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->exist_vars=exists;_tmp11E->rgn_po=
po;_tmp11E->fields=fs;_tmp11E;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));
_tmp11F[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp120;_tmp120.tag=4;_tmp120.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->kind=(void*)
k;_tmp121->sc=(void*)s;_tmp121->name=n;_tmp121->tvs=ts;_tmp121->impl=i;_tmp121->attributes=
atts;_tmp121;});_tmp120;});_tmp11F;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp122=
_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp123;
_tmp123.tag=5;_tmp123.f1=({struct Cyc_Absyn_Tuniondecl*_tmp124=_cycalloc(sizeof(*
_tmp124));_tmp124->sc=(void*)s;_tmp124->name=n;_tmp124->tvs=ts;_tmp124->fields=
fs;_tmp124->is_xtunion=is_xtunion;_tmp124->is_flat=is_flat;_tmp124;});_tmp123;});
_tmp122;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=(*a).f1;
_tmp125->f2=(*a).f2;_tmp125->f3=Cyc_Absyn_pointer_expand((*a).f3,1);_tmp125;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({
struct Cyc_Absyn_FnType_struct _tmp127;_tmp127.tag=8;_tmp127.f1=({struct Cyc_Absyn_FnInfo
_tmp128;_tmp128.tvars=tvs;_tmp128.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ,0);_tmp128.effect=eff_typ;_tmp128.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp128.c_varargs=c_varargs;_tmp128.cyc_varargs=cyc_varargs;_tmp128.rgn_po=
rgn_po;_tmp128.attributes=atts;_tmp128;});_tmp127;});_tmp126;});}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp129=Cyc_Tcutil_compress(t);_LL43: if(_tmp129 <= (
void*)4)goto _LL45;if(*((int*)_tmp129)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(
t,fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp12A=_cycalloc(sizeof(*
_tmp12A));_tmp12A->v=(void*)((void*)3);_tmp12A;}),0):(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp12B=(void*)e->r;void*_tmp12C;void*_tmp12D;struct
Cyc_Absyn_Vardecl*_tmp12E;void*_tmp12F;struct Cyc_Absyn_Vardecl*_tmp130;struct Cyc_Absyn_Exp*
_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp133;_LL48: if(*((int*)
_tmp12B)!= 1)goto _LL4A;_tmp12C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12B)->f2;
if(_tmp12C <= (void*)1)goto _LL4A;if(*((int*)_tmp12C)!= 1)goto _LL4A;_LL49: return 0;
_LL4A: if(*((int*)_tmp12B)!= 1)goto _LL4C;_tmp12D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12B)->f2;if(_tmp12D <= (void*)1)goto _LL4C;if(*((int*)_tmp12D)!= 0)goto _LL4C;
_tmp12E=((struct Cyc_Absyn_Global_b_struct*)_tmp12D)->f1;_LL4B: _tmp130=_tmp12E;
goto _LL4D;_LL4C: if(*((int*)_tmp12B)!= 1)goto _LL4E;_tmp12F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12B)->f2;if(_tmp12F <= (void*)1)goto _LL4E;if(*((int*)_tmp12F)!= 3)goto _LL4E;
_tmp130=((struct Cyc_Absyn_Local_b_struct*)_tmp12F)->f1;_LL4D: {void*_tmp134=Cyc_Tcutil_compress((
void*)_tmp130->type);_LL5F: if(_tmp134 <= (void*)4)goto _LL61;if(*((int*)_tmp134)!= 
7)goto _LL61;_LL60: return 0;_LL61:;_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp12B)
!= 1)goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp12B)!= 24)goto _LL52;_LL51:
goto _LL53;_LL52: if(*((int*)_tmp12B)!= 22)goto _LL54;_LL53: goto _LL55;_LL54: if(*((
int*)_tmp12B)!= 25)goto _LL56;_LL55: return 1;_LL56: if(*((int*)_tmp12B)!= 23)goto
_LL58;_tmp131=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp12B)->f1;_LL57: return
Cyc_Absyn_is_lvalue(_tmp131);_LL58: if(*((int*)_tmp12B)!= 14)goto _LL5A;_tmp132=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp12B)->f1;_LL59: return Cyc_Absyn_is_lvalue(
_tmp132);_LL5A: if(*((int*)_tmp12B)!= 13)goto _LL5C;_tmp133=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp12B)->f1;_LL5B: return Cyc_Absyn_is_lvalue(_tmp133);_LL5C:;_LL5D: return 0;_LL47:;}
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,
struct _dynforward_ptr*v){{struct Cyc_List_List*_tmp135=fields;for(0;_tmp135 != 0;
_tmp135=_tmp135->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp135->hd)->name,
v)== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp135->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dynforward_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct
_dynforward_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp136=a;int _tmp137;int
_tmp138;struct _dynforward_ptr _tmp139;void*_tmp13A;int _tmp13B;int _tmp13C;void*
_tmp13D;int _tmp13E;int _tmp13F;int _tmp140;struct _dynforward_ptr _tmp141;_LL64: if(
_tmp136 <= (void*)17)goto _LL66;if(*((int*)_tmp136)!= 0)goto _LL66;_tmp137=((struct
Cyc_Absyn_Regparm_att_struct*)_tmp136)->f1;_LL65: return(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp144;_tmp144.tag=1;_tmp144.f1=(unsigned long)_tmp137;{
void*_tmp142[1]={& _tmp144};Cyc_aprintf(({const char*_tmp143="regparm(%d)";
_tag_dynforward(_tmp143,sizeof(char),_get_zero_arr_size(_tmp143,12));}),
_tag_dynforward(_tmp142,sizeof(void*),1));}});_LL66: if((int)_tmp136 != 0)goto
_LL68;_LL67: return({const char*_tmp145="stdcall";_tag_dynforward(_tmp145,sizeof(
char),_get_zero_arr_size(_tmp145,8));});_LL68: if((int)_tmp136 != 1)goto _LL6A;
_LL69: return({const char*_tmp146="cdecl";_tag_dynforward(_tmp146,sizeof(char),
_get_zero_arr_size(_tmp146,6));});_LL6A: if((int)_tmp136 != 2)goto _LL6C;_LL6B:
return({const char*_tmp147="fastcall";_tag_dynforward(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,9));});_LL6C: if((int)_tmp136 != 3)goto _LL6E;_LL6D:
return({const char*_tmp148="noreturn";_tag_dynforward(_tmp148,sizeof(char),
_get_zero_arr_size(_tmp148,9));});_LL6E: if((int)_tmp136 != 4)goto _LL70;_LL6F:
return({const char*_tmp149="const";_tag_dynforward(_tmp149,sizeof(char),
_get_zero_arr_size(_tmp149,6));});_LL70: if(_tmp136 <= (void*)17)goto _LL72;if(*((
int*)_tmp136)!= 1)goto _LL72;_tmp138=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp136)->f1;_LL71: if(_tmp138 == - 1)return({const char*_tmp14A="aligned";
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,8));});else{
return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp14D;_tmp14D.tag=1;
_tmp14D.f1=(unsigned long)_tmp138;{void*_tmp14B[1]={& _tmp14D};Cyc_aprintf(({
const char*_tmp14C="aligned(%d)";_tag_dynforward(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,12));}),_tag_dynforward(_tmp14B,sizeof(void*),1));}});}
_LL72: if((int)_tmp136 != 5)goto _LL74;_LL73: return({const char*_tmp14E="packed";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,7));});_LL74: if(
_tmp136 <= (void*)17)goto _LL76;if(*((int*)_tmp136)!= 2)goto _LL76;_tmp139=((struct
Cyc_Absyn_Section_att_struct*)_tmp136)->f1;_LL75: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp139);{void*_tmp14F[1]={& _tmp151};Cyc_aprintf(({
const char*_tmp150="section(\"%s\")";_tag_dynforward(_tmp150,sizeof(char),
_get_zero_arr_size(_tmp150,14));}),_tag_dynforward(_tmp14F,sizeof(void*),1));}});
_LL76: if((int)_tmp136 != 6)goto _LL78;_LL77: return({const char*_tmp152="nocommon";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,9));});_LL78: if((
int)_tmp136 != 7)goto _LL7A;_LL79: return({const char*_tmp153="shared";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,7));});_LL7A: if((
int)_tmp136 != 8)goto _LL7C;_LL7B: return({const char*_tmp154="unused";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,7));});_LL7C: if((
int)_tmp136 != 9)goto _LL7E;_LL7D: return({const char*_tmp155="weak";_tag_dynforward(
_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,5));});_LL7E: if((int)_tmp136 != 
10)goto _LL80;_LL7F: return({const char*_tmp156="dllimport";_tag_dynforward(_tmp156,
sizeof(char),_get_zero_arr_size(_tmp156,10));});_LL80: if((int)_tmp136 != 11)goto
_LL82;_LL81: return({const char*_tmp157="dllexport";_tag_dynforward(_tmp157,
sizeof(char),_get_zero_arr_size(_tmp157,10));});_LL82: if((int)_tmp136 != 12)goto
_LL84;_LL83: return({const char*_tmp158="no_instrument_function";_tag_dynforward(
_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,23));});_LL84: if((int)_tmp136 != 
13)goto _LL86;_LL85: return({const char*_tmp159="constructor";_tag_dynforward(
_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,12));});_LL86: if((int)_tmp136 != 
14)goto _LL88;_LL87: return({const char*_tmp15A="destructor";_tag_dynforward(
_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,11));});_LL88: if((int)_tmp136 != 
15)goto _LL8A;_LL89: return({const char*_tmp15B="no_check_memory_usage";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,22));});_LL8A:
if(_tmp136 <= (void*)17)goto _LL90;if(*((int*)_tmp136)!= 3)goto _LL8C;_tmp13A=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp136)->f1;if((int)_tmp13A != 0)goto _LL8C;
_tmp13B=((struct Cyc_Absyn_Format_att_struct*)_tmp136)->f2;_tmp13C=((struct Cyc_Absyn_Format_att_struct*)
_tmp136)->f3;_LL8B: return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp15F;
_tmp15F.tag=1;_tmp15F.f1=(unsigned int)_tmp13C;{struct Cyc_Int_pa_struct _tmp15E;
_tmp15E.tag=1;_tmp15E.f1=(unsigned int)_tmp13B;{void*_tmp15C[2]={& _tmp15E,&
_tmp15F};Cyc_aprintf(({const char*_tmp15D="format(printf,%u,%u)";_tag_dynforward(
_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,21));}),_tag_dynforward(_tmp15C,
sizeof(void*),2));}}});_LL8C: if(*((int*)_tmp136)!= 3)goto _LL8E;_tmp13D=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp136)->f1;if((int)_tmp13D != 1)goto _LL8E;
_tmp13E=((struct Cyc_Absyn_Format_att_struct*)_tmp136)->f2;_tmp13F=((struct Cyc_Absyn_Format_att_struct*)
_tmp136)->f3;_LL8D: return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp163;
_tmp163.tag=1;_tmp163.f1=(unsigned int)_tmp13F;{struct Cyc_Int_pa_struct _tmp162;
_tmp162.tag=1;_tmp162.f1=(unsigned int)_tmp13E;{void*_tmp160[2]={& _tmp162,&
_tmp163};Cyc_aprintf(({const char*_tmp161="format(scanf,%u,%u)";_tag_dynforward(
_tmp161,sizeof(char),_get_zero_arr_size(_tmp161,20));}),_tag_dynforward(_tmp160,
sizeof(void*),2));}}});_LL8E: if(*((int*)_tmp136)!= 4)goto _LL90;_tmp140=((struct
Cyc_Absyn_Initializes_att_struct*)_tmp136)->f1;_LL8F: return(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp166;_tmp166.tag=1;_tmp166.f1=(
unsigned long)_tmp140;{void*_tmp164[1]={& _tmp166};Cyc_aprintf(({const char*
_tmp165="initializes(%d)";_tag_dynforward(_tmp165,sizeof(char),
_get_zero_arr_size(_tmp165,16));}),_tag_dynforward(_tmp164,sizeof(void*),1));}});
_LL90: if((int)_tmp136 != 16)goto _LL92;_LL91: return({const char*_tmp167="pure";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,5));});_LL92: if(
_tmp136 <= (void*)17)goto _LL63;if(*((int*)_tmp136)!= 5)goto _LL63;_tmp141=((struct
Cyc_Absyn_Mode_att_struct*)_tmp136)->f1;_LL93: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp141);{void*_tmp168[1]={& _tmp16A};Cyc_aprintf(({
const char*_tmp169="__mode__(\"%s\")";_tag_dynforward(_tmp169,sizeof(char),
_get_zero_arr_size(_tmp169,15));}),_tag_dynforward(_tmp168,sizeof(void*),1));}});
_LL63:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp16B=a;_LL95: if(_tmp16B <= (void*)
17)goto _LL97;if(*((int*)_tmp16B)!= 0)goto _LL97;_LL96: goto _LL98;_LL97: if((int)
_tmp16B != 2)goto _LL99;_LL98: goto _LL9A;_LL99: if((int)_tmp16B != 0)goto _LL9B;_LL9A:
goto _LL9C;_LL9B: if((int)_tmp16B != 1)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if((int)
_tmp16B != 3)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if((int)_tmp16B != 16)goto _LLA1;_LLA0:
goto _LLA2;_LLA1: if(_tmp16B <= (void*)17)goto _LLA3;if(*((int*)_tmp16B)!= 3)goto
_LLA3;_LLA2: goto _LLA4;_LLA3: if((int)_tmp16B != 4)goto _LLA5;_LLA4: return 1;_LLA5:
if(_tmp16B <= (void*)17)goto _LLA7;if(*((int*)_tmp16B)!= 4)goto _LLA7;_LLA6: return 1;
_LLA7:;_LLA8: return 0;_LL94:;}static char _tmp16C[3]="f0";static struct
_dynforward_ptr Cyc_Absyn_f0={_tmp16C,_tmp16C + 3};static struct _dynforward_ptr*Cyc_Absyn_field_names_v[
1]={& Cyc_Absyn_f0};static struct _dynforward_ptr Cyc_Absyn_field_names={(void*)((
struct _dynforward_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dynforward_ptr**)
Cyc_Absyn_field_names_v + 1)};struct _dynforward_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dynforward_size(Cyc_Absyn_field_names,sizeof(struct
_dynforward_ptr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp16D=(
unsigned int)(i + 1);struct _dynforward_ptr**_tmp16E=(struct _dynforward_ptr**)
_cycalloc(_check_times(sizeof(struct _dynforward_ptr*),_tmp16D));struct
_dynforward_ptr _tmp174=_tag_dynforward(_tmp16E,sizeof(struct _dynforward_ptr*),
_tmp16D);{unsigned int _tmp16F=_tmp16D;unsigned int j;for(j=0;j < _tmp16F;j ++){
_tmp16E[j]=j < fsz?*((struct _dynforward_ptr**)_check_dynforward_subscript(Cyc_Absyn_field_names,
sizeof(struct _dynforward_ptr*),(int)j)):({struct _dynforward_ptr*_tmp170=
_cycalloc(sizeof(*_tmp170));_tmp170[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp173;_tmp173.tag=1;_tmp173.f1=(unsigned long)((int)j);{void*_tmp171[1]={&
_tmp173};Cyc_aprintf(({const char*_tmp172="f%d";_tag_dynforward(_tmp172,sizeof(
char),_get_zero_arr_size(_tmp172,4));}),_tag_dynforward(_tmp171,sizeof(void*),1));}});
_tmp170;});}}_tmp174;});return*((struct _dynforward_ptr**)
_check_dynforward_subscript(Cyc_Absyn_field_names,sizeof(struct _dynforward_ptr*),
i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union info){
union Cyc_Absyn_AggrInfoU_union _tmp175=info;void*_tmp176;struct _tuple0*_tmp177;
struct Cyc_Absyn_Aggrdecl**_tmp178;struct Cyc_Absyn_Aggrdecl*_tmp179;struct Cyc_Absyn_Aggrdecl
_tmp17A;void*_tmp17B;struct _tuple0*_tmp17C;_LLAA: if((_tmp175.UnknownAggr).tag != 
0)goto _LLAC;_tmp176=(_tmp175.UnknownAggr).f1;_tmp177=(_tmp175.UnknownAggr).f2;
_LLAB: return({struct _tuple4 _tmp17D;_tmp17D.f1=_tmp176;_tmp17D.f2=_tmp177;_tmp17D;});
_LLAC: if((_tmp175.KnownAggr).tag != 1)goto _LLA9;_tmp178=(_tmp175.KnownAggr).f1;
_tmp179=*_tmp178;_tmp17A=*_tmp179;_tmp17B=(void*)_tmp17A.kind;_tmp17C=_tmp17A.name;
_LLAD: return({struct _tuple4 _tmp17E;_tmp17E.f1=_tmp17B;_tmp17E.f2=_tmp17C;_tmp17E;});
_LLA9:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info){union Cyc_Absyn_AggrInfoU_union _tmp17F=info;void*_tmp180;struct _tuple0*
_tmp181;struct Cyc_Absyn_Aggrdecl**_tmp182;struct Cyc_Absyn_Aggrdecl*_tmp183;_LLAF:
if((_tmp17F.UnknownAggr).tag != 0)goto _LLB1;_tmp180=(_tmp17F.UnknownAggr).f1;
_tmp181=(_tmp17F.UnknownAggr).f2;_LLB0:({void*_tmp184[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp185="unchecked aggrdecl";_tag_dynforward(_tmp185,sizeof(char),
_get_zero_arr_size(_tmp185,19));}),_tag_dynforward(_tmp184,sizeof(void*),0));});
_LLB1: if((_tmp17F.KnownAggr).tag != 1)goto _LLAE;_tmp182=(_tmp17F.KnownAggr).f1;
_tmp183=*_tmp182;_LLB2: return _tmp183;_LLAE:;}int Cyc_Absyn_is_union_type(void*t){
void*_tmp186=Cyc_Tcutil_compress(t);void*_tmp187;struct Cyc_Absyn_AggrInfo _tmp188;
union Cyc_Absyn_AggrInfoU_union _tmp189;_LLB4: if(_tmp186 <= (void*)4)goto _LLB8;if(*((
int*)_tmp186)!= 11)goto _LLB6;_tmp187=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp186)->f1;if((int)_tmp187 != 1)goto _LLB6;_LLB5: return 1;_LLB6: if(*((int*)
_tmp186)!= 10)goto _LLB8;_tmp188=((struct Cyc_Absyn_AggrType_struct*)_tmp186)->f1;
_tmp189=_tmp188.aggr_info;_LLB7: return(Cyc_Absyn_aggr_kinded_name(_tmp189)).f1 == (
void*)1;_LLB8:;_LLB9: return 0;_LLB3:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp18A[0]={};Cyc_printf(({const char*_tmp18B="\n";_tag_dynforward(
_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,2));}),_tag_dynforward(_tmp18A,
sizeof(void*),0));});}int Cyc_Absyn_porting_c_code=0;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_826;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp18D[7]="Static";
static struct _tuple7 Cyc__gentuple_167={0,(struct _dynforward_ptr){_tmp18D,_tmp18D + 
7}};static char _tmp18E[9]="Abstract";static struct _tuple7 Cyc__gentuple_168={1,(
struct _dynforward_ptr){_tmp18E,_tmp18E + 9}};static char _tmp18F[7]="Public";static
struct _tuple7 Cyc__gentuple_169={2,(struct _dynforward_ptr){_tmp18F,_tmp18F + 7}};
static char _tmp190[7]="Extern";static struct _tuple7 Cyc__gentuple_170={3,(struct
_dynforward_ptr){_tmp190,_tmp190 + 7}};static char _tmp191[8]="ExternC";static
struct _tuple7 Cyc__gentuple_171={4,(struct _dynforward_ptr){_tmp191,_tmp191 + 8}};
static char _tmp192[9]="Register";static struct _tuple7 Cyc__gentuple_172={5,(struct
_dynforward_ptr){_tmp192,_tmp192 + 9}};static struct _tuple7*Cyc__genarr_173[6]={&
Cyc__gentuple_167,& Cyc__gentuple_168,& Cyc__gentuple_169,& Cyc__gentuple_170,& Cyc__gentuple_171,&
Cyc__gentuple_172};static struct _tuple5*Cyc__genarr_174[0]={};static char _tmp194[6]="Scope";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={5,(struct _dynforward_ptr){
_tmp194,_tmp194 + 6},{(void*)((struct _tuple7**)Cyc__genarr_173),(void*)((struct
_tuple7**)Cyc__genarr_173 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_174),(void*)((
struct _tuple5**)Cyc__genarr_174 + 0)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_12;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_nmspace_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_18;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14;static struct
Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};static char _tmp198[5]="List";
static struct _dynforward_ptr Cyc__genname_23=(struct _dynforward_ptr){_tmp198,
_tmp198 + 5};static char _tmp199[3]="hd";static struct _tuple5 Cyc__gentuple_20={
offsetof(struct Cyc_List_List,hd),(struct _dynforward_ptr){_tmp199,_tmp199 + 3},(
void*)& Cyc__genrep_13};static char _tmp19A[3]="tl";static struct _tuple5 Cyc__gentuple_21={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp19A,_tmp19A + 3},(
void*)& Cyc__genrep_19};static struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,&
Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_23,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple14{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27 + 2)}};static char _tmp19E[6]="Loc_n";static struct
_tuple7 Cyc__gentuple_16={0,(struct _dynforward_ptr){_tmp19E,_tmp19E + 6}};static
struct _tuple7*Cyc__genarr_17[1]={& Cyc__gentuple_16};static char _tmp19F[6]="Rel_n";
static struct _tuple5 Cyc__gentuple_28={0,(struct _dynforward_ptr){_tmp19F,_tmp19F + 
6},(void*)& Cyc__genrep_18};static char _tmp1A0[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={
1,(struct _dynforward_ptr){_tmp1A0,_tmp1A0 + 6},(void*)& Cyc__genrep_18};static
struct _tuple5*Cyc__genarr_30[2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char
_tmp1A2[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(
struct _dynforward_ptr){_tmp1A2,_tmp1A2 + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(
void*)((struct _tuple7**)Cyc__genarr_17 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_30),(
void*)((struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={1,1,(void*)((void*)& Cyc__genrep_12)};
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;static struct Cyc_Typerep_Int_struct
Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1A7[6]="Tqual";static struct _dynforward_ptr Cyc__genname_166=(struct
_dynforward_ptr){_tmp1A7,_tmp1A7 + 6};static char _tmp1A8[12]="print_const";static
struct _tuple5 Cyc__gentuple_160={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _dynforward_ptr){_tmp1A8,_tmp1A8 + 12},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1A9[11]="q_volatile";static struct _tuple5 Cyc__gentuple_161={
offsetof(struct Cyc_Absyn_Tqual,q_volatile),(struct _dynforward_ptr){_tmp1A9,
_tmp1A9 + 11},(void*)((void*)& Cyc__genrep_10)};static char _tmp1AA[11]="q_restrict";
static struct _tuple5 Cyc__gentuple_162={offsetof(struct Cyc_Absyn_Tqual,q_restrict),(
struct _dynforward_ptr){_tmp1AA,_tmp1AA + 11},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1AB[11]="real_const";static struct _tuple5 Cyc__gentuple_163={
offsetof(struct Cyc_Absyn_Tqual,real_const),(struct _dynforward_ptr){_tmp1AB,
_tmp1AB + 11},(void*)((void*)& Cyc__genrep_10)};static char _tmp1AC[4]="loc";static
struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_Absyn_Tqual,loc),(struct
_dynforward_ptr){_tmp1AC,_tmp1AC + 4},(void*)& Cyc__genrep_2};static struct _tuple5*
Cyc__genarr_165[5]={& Cyc__gentuple_160,& Cyc__gentuple_161,& Cyc__gentuple_162,&
Cyc__gentuple_163,& Cyc__gentuple_164};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_166,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_165),(void*)((struct _tuple5**)Cyc__genarr_165 + 5)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1119;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1124;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1AE[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_188={0,(struct _dynforward_ptr){_tmp1AE,_tmp1AE + 
8}};static char _tmp1AF[8]="MemKind";static struct _tuple7 Cyc__gentuple_189={1,(
struct _dynforward_ptr){_tmp1AF,_tmp1AF + 8}};static char _tmp1B0[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_190={2,(struct _dynforward_ptr){_tmp1B0,_tmp1B0 + 
8}};static char _tmp1B1[8]="RgnKind";static struct _tuple7 Cyc__gentuple_191={3,(
struct _dynforward_ptr){_tmp1B1,_tmp1B1 + 8}};static char _tmp1B2[14]="UniqueRgnKind";
static struct _tuple7 Cyc__gentuple_192={4,(struct _dynforward_ptr){_tmp1B2,_tmp1B2 + 
14}};static char _tmp1B3[11]="TopRgnKind";static struct _tuple7 Cyc__gentuple_193={5,(
struct _dynforward_ptr){_tmp1B3,_tmp1B3 + 11}};static char _tmp1B4[8]="EffKind";
static struct _tuple7 Cyc__gentuple_194={6,(struct _dynforward_ptr){_tmp1B4,_tmp1B4 + 
8}};static char _tmp1B5[8]="IntKind";static struct _tuple7 Cyc__gentuple_195={7,(
struct _dynforward_ptr){_tmp1B5,_tmp1B5 + 8}};static struct _tuple7*Cyc__genarr_196[
8]={& Cyc__gentuple_188,& Cyc__gentuple_189,& Cyc__gentuple_190,& Cyc__gentuple_191,&
Cyc__gentuple_192,& Cyc__gentuple_193,& Cyc__gentuple_194,& Cyc__gentuple_195};
static struct _tuple5*Cyc__genarr_197[0]={};static char _tmp1B7[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,(struct _dynforward_ptr){_tmp1B7,_tmp1B7 + 5},{(void*)((
struct _tuple7**)Cyc__genarr_196),(void*)((struct _tuple7**)Cyc__genarr_196 + 8)},{(
void*)((struct _tuple5**)Cyc__genarr_197),(void*)((struct _tuple5**)Cyc__genarr_197
+ 0)}};static char _tmp1B8[4]="Opt";static struct _dynforward_ptr Cyc__genname_1127=(
struct _dynforward_ptr){_tmp1B8,_tmp1B8 + 4};static char _tmp1B9[2]="v";static struct
_tuple5 Cyc__gentuple_1125={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1B9,_tmp1B9 + 2},(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple5*Cyc__genarr_1126[
1]={& Cyc__gentuple_1125};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1127,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1126),(void*)((struct _tuple5**)Cyc__genarr_1126 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1124={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1BC[4]="Opt";static struct
_dynforward_ptr Cyc__genname_1160=(struct _dynforward_ptr){_tmp1BC,_tmp1BC + 4};
static char _tmp1BD[2]="v";static struct _tuple5 Cyc__gentuple_1158={offsetof(struct
Cyc_Core_Opt,v),(struct _dynforward_ptr){_tmp1BD,_tmp1BD + 2},(void*)& Cyc_Absyn_type_t_rep};
static struct _tuple5*Cyc__genarr_1159[1]={& Cyc__gentuple_1158};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1160,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1159),(void*)((
struct _tuple5**)Cyc__genarr_1159 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1120;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_336;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_185;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_218;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_218={1,1,(void*)((void*)& Cyc__genrep_10)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_210;
static struct _tuple6 Cyc__gentuple_211={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_212={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_213[2]={& Cyc__gentuple_211,& Cyc__gentuple_212};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_210={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_213),(void*)((struct _tuple6**)Cyc__genarr_213
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_206;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_198;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;
static char _tmp1C2[4]="Opt";static struct _dynforward_ptr Cyc__genname_201=(struct
_dynforward_ptr){_tmp1C2,_tmp1C2 + 4};static char _tmp1C3[2]="v";static struct
_tuple5 Cyc__gentuple_199={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1C3,_tmp1C3 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};static struct _tuple5*Cyc__genarr_200[
1]={& Cyc__gentuple_199};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_201,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_200),(void*)((struct _tuple5**)Cyc__genarr_200 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_198={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_207={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_208={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_198};static struct _tuple6*Cyc__genarr_209[
2]={& Cyc__gentuple_207,& Cyc__gentuple_208};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_206={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_209),(
void*)((struct _tuple6**)Cyc__genarr_209 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_187;struct _tuple16{unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_202={offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_203={offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_198};
static struct _tuple6 Cyc__gentuple_204={offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_205[3]={& Cyc__gentuple_202,& Cyc__gentuple_203,&
Cyc__gentuple_204};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_187={4,
sizeof(struct _tuple16),{(void*)((struct _tuple6**)Cyc__genarr_205),(void*)((
struct _tuple6**)Cyc__genarr_205 + 3)}};static struct _tuple7*Cyc__genarr_186[0]={};
static char _tmp1C8[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_214={0,(struct
_dynforward_ptr){_tmp1C8,_tmp1C8 + 6},(void*)& Cyc__genrep_210};static char _tmp1C9[
11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_215={1,(struct _dynforward_ptr){
_tmp1C9,_tmp1C9 + 11},(void*)& Cyc__genrep_206};static char _tmp1CA[8]="Less_kb";
static struct _tuple5 Cyc__gentuple_216={2,(struct _dynforward_ptr){_tmp1CA,_tmp1CA + 
8},(void*)& Cyc__genrep_187};static struct _tuple5*Cyc__genarr_217[3]={& Cyc__gentuple_214,&
Cyc__gentuple_215,& Cyc__gentuple_216};static char _tmp1CC[10]="KindBound";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,(struct _dynforward_ptr){
_tmp1CC,_tmp1CC + 10},{(void*)((struct _tuple7**)Cyc__genarr_186),(void*)((struct
_tuple7**)Cyc__genarr_186 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_217),(void*)((
struct _tuple5**)Cyc__genarr_217 + 3)}};static char _tmp1CD[5]="Tvar";static struct
_dynforward_ptr Cyc__genname_223=(struct _dynforward_ptr){_tmp1CD,_tmp1CD + 5};
static char _tmp1CE[5]="name";static struct _tuple5 Cyc__gentuple_219={offsetof(
struct Cyc_Absyn_Tvar,name),(struct _dynforward_ptr){_tmp1CE,_tmp1CE + 5},(void*)&
Cyc__genrep_13};static char _tmp1CF[9]="identity";static struct _tuple5 Cyc__gentuple_220={
offsetof(struct Cyc_Absyn_Tvar,identity),(struct _dynforward_ptr){_tmp1CF,_tmp1CF + 
9},(void*)& Cyc__genrep_218};static char _tmp1D0[5]="kind";static struct _tuple5 Cyc__gentuple_221={
offsetof(struct Cyc_Absyn_Tvar,kind),(struct _dynforward_ptr){_tmp1D0,_tmp1D0 + 5},(
void*)& Cyc_Absyn_kindbound_t_rep};static struct _tuple5*Cyc__genarr_222[3]={& Cyc__gentuple_219,&
Cyc__gentuple_220,& Cyc__gentuple_221};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_223,sizeof(struct Cyc_Absyn_Tvar),{(void*)((
struct _tuple5**)Cyc__genarr_222),(void*)((struct _tuple5**)Cyc__genarr_222 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185={1,1,(void*)((void*)& Cyc_struct_Absyn_Tvar_rep)};
static char _tmp1D3[5]="List";static struct _dynforward_ptr Cyc__genname_340=(struct
_dynforward_ptr){_tmp1D3,_tmp1D3 + 5};static char _tmp1D4[3]="hd";static struct
_tuple5 Cyc__gentuple_337={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp1D4,_tmp1D4 + 3},(void*)& Cyc__genrep_185};static char _tmp1D5[
3]="tl";static struct _tuple5 Cyc__gentuple_338={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp1D5,_tmp1D5 + 3},(void*)& Cyc__genrep_336};static struct
_tuple5*Cyc__genarr_339[2]={& Cyc__gentuple_337,& Cyc__gentuple_338};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_340,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_339),(void*)((
struct _tuple5**)Cyc__genarr_339 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_336={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1D8[4]="Opt";static struct _dynforward_ptr Cyc__genname_1123=(struct
_dynforward_ptr){_tmp1D8,_tmp1D8 + 4};static char _tmp1D9[2]="v";static struct
_tuple5 Cyc__gentuple_1121={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1D9,_tmp1D9 + 2},(void*)& Cyc__genrep_336};static struct _tuple5*Cyc__genarr_1122[
1]={& Cyc__gentuple_1121};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1123,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1122),(void*)((struct _tuple5**)Cyc__genarr_1122 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1120={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};
struct _tuple17{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;
struct Cyc_Core_Opt*f5;};static struct _tuple6 Cyc__gentuple_1128={offsetof(struct
_tuple17,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1129={
offsetof(struct _tuple17,f2),(void*)& Cyc__genrep_1124};static struct _tuple6 Cyc__gentuple_1130={
offsetof(struct _tuple17,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1131={
offsetof(struct _tuple17,f4),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_1132={offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1120};static
struct _tuple6*Cyc__genarr_1133[5]={& Cyc__gentuple_1128,& Cyc__gentuple_1129,& Cyc__gentuple_1130,&
Cyc__gentuple_1131,& Cyc__gentuple_1132};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1119={
4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1133),(void*)((
struct _tuple6**)Cyc__genarr_1133 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1115;
struct _tuple18{unsigned int f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1116={
offsetof(struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1117={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_185};static struct _tuple6*Cyc__genarr_1118[
2]={& Cyc__gentuple_1116,& Cyc__gentuple_1117};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1115={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1118),(
void*)((struct _tuple6**)Cyc__genarr_1118 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1088;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1095;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1DE[18]="UnknownTunionInfo";
static struct _dynforward_ptr Cyc__genname_1100=(struct _dynforward_ptr){_tmp1DE,
_tmp1DE + 18};static char _tmp1DF[5]="name";static struct _tuple5 Cyc__gentuple_1096={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),(struct _dynforward_ptr){_tmp1DF,
_tmp1DF + 5},(void*)& Cyc__genrep_11};static char _tmp1E0[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1097={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),(
struct _dynforward_ptr){_tmp1E0,_tmp1E0 + 11},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1E1[8]="is_flat";static struct _tuple5 Cyc__gentuple_1098={offsetof(
struct Cyc_Absyn_UnknownTunionInfo,is_flat),(struct _dynforward_ptr){_tmp1E1,
_tmp1E1 + 8},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_1099[
3]={& Cyc__gentuple_1096,& Cyc__gentuple_1097,& Cyc__gentuple_1098};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1100,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1099),(
void*)((struct _tuple5**)Cyc__genarr_1099 + 3)}};struct _tuple19{unsigned int f1;
struct Cyc_Absyn_UnknownTunionInfo f2;};static struct _tuple6 Cyc__gentuple_1101={
offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1102={
offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1103[2]={& Cyc__gentuple_1101,& Cyc__gentuple_1102};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1095={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1103),(void*)((struct _tuple6**)Cyc__genarr_1103
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1090;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1091;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_326;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_327;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_328;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_309;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_310;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_311;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_312;static struct
_tuple6 Cyc__gentuple_313={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_314={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_315[2]={& Cyc__gentuple_313,&
Cyc__gentuple_314};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_312={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_315),(void*)((struct
_tuple6**)Cyc__genarr_315 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_311={
1,1,(void*)((void*)& Cyc__genrep_312)};static char _tmp1E6[5]="List";static struct
_dynforward_ptr Cyc__genname_319=(struct _dynforward_ptr){_tmp1E6,_tmp1E6 + 5};
static char _tmp1E7[3]="hd";static struct _tuple5 Cyc__gentuple_316={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1E7,_tmp1E7 + 3},(void*)& Cyc__genrep_311};
static char _tmp1E8[3]="tl";static struct _tuple5 Cyc__gentuple_317={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1E8,_tmp1E8 + 3},(void*)& Cyc__genrep_310};
static struct _tuple5*Cyc__genarr_318[2]={& Cyc__gentuple_316,& Cyc__gentuple_317};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_319,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_318),(void*)((struct _tuple5**)Cyc__genarr_318 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_310={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1EB[12]="Tunionfield";static struct _dynforward_ptr Cyc__genname_325=(
struct _dynforward_ptr){_tmp1EB,_tmp1EB + 12};static char _tmp1EC[5]="name";static
struct _tuple5 Cyc__gentuple_320={offsetof(struct Cyc_Absyn_Tunionfield,name),(
struct _dynforward_ptr){_tmp1EC,_tmp1EC + 5},(void*)& Cyc__genrep_11};static char
_tmp1ED[5]="typs";static struct _tuple5 Cyc__gentuple_321={offsetof(struct Cyc_Absyn_Tunionfield,typs),(
struct _dynforward_ptr){_tmp1ED,_tmp1ED + 5},(void*)& Cyc__genrep_310};static char
_tmp1EE[4]="loc";static struct _tuple5 Cyc__gentuple_322={offsetof(struct Cyc_Absyn_Tunionfield,loc),(
struct _dynforward_ptr){_tmp1EE,_tmp1EE + 4},(void*)& Cyc__genrep_2};static char
_tmp1EF[3]="sc";static struct _tuple5 Cyc__gentuple_323={offsetof(struct Cyc_Absyn_Tunionfield,sc),(
struct _dynforward_ptr){_tmp1EF,_tmp1EF + 3},(void*)& Cyc_Absyn_scope_t_rep};static
struct _tuple5*Cyc__genarr_324[4]={& Cyc__gentuple_320,& Cyc__gentuple_321,& Cyc__gentuple_322,&
Cyc__gentuple_323};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_325,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_324),(void*)((struct _tuple5**)Cyc__genarr_324
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_309={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tunionfield_rep)};static char _tmp1F2[5]="List";static struct
_dynforward_ptr Cyc__genname_332=(struct _dynforward_ptr){_tmp1F2,_tmp1F2 + 5};
static char _tmp1F3[3]="hd";static struct _tuple5 Cyc__gentuple_329={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1F3,_tmp1F3 + 3},(void*)& Cyc__genrep_309};
static char _tmp1F4[3]="tl";static struct _tuple5 Cyc__gentuple_330={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1F4,_tmp1F4 + 3},(void*)& Cyc__genrep_328};
static struct _tuple5*Cyc__genarr_331[2]={& Cyc__gentuple_329,& Cyc__gentuple_330};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_332,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_331),(void*)((struct _tuple5**)Cyc__genarr_331 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_328={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};
static char _tmp1F7[4]="Opt";static struct _dynforward_ptr Cyc__genname_335=(struct
_dynforward_ptr){_tmp1F7,_tmp1F7 + 4};static char _tmp1F8[2]="v";static struct
_tuple5 Cyc__gentuple_333={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1F8,_tmp1F8 + 2},(void*)& Cyc__genrep_328};static struct _tuple5*Cyc__genarr_334[
1]={& Cyc__gentuple_333};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_335,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_334),(void*)((struct _tuple5**)Cyc__genarr_334 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_327={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1FB[11]="Tuniondecl";static struct _dynforward_ptr Cyc__genname_348=(
struct _dynforward_ptr){_tmp1FB,_tmp1FB + 11};static char _tmp1FC[3]="sc";static
struct _tuple5 Cyc__gentuple_341={offsetof(struct Cyc_Absyn_Tuniondecl,sc),(struct
_dynforward_ptr){_tmp1FC,_tmp1FC + 3},(void*)& Cyc_Absyn_scope_t_rep};static char
_tmp1FD[5]="name";static struct _tuple5 Cyc__gentuple_342={offsetof(struct Cyc_Absyn_Tuniondecl,name),(
struct _dynforward_ptr){_tmp1FD,_tmp1FD + 5},(void*)& Cyc__genrep_11};static char
_tmp1FE[4]="tvs";static struct _tuple5 Cyc__gentuple_343={offsetof(struct Cyc_Absyn_Tuniondecl,tvs),(
struct _dynforward_ptr){_tmp1FE,_tmp1FE + 4},(void*)& Cyc__genrep_336};static char
_tmp1FF[7]="fields";static struct _tuple5 Cyc__gentuple_344={offsetof(struct Cyc_Absyn_Tuniondecl,fields),(
struct _dynforward_ptr){_tmp1FF,_tmp1FF + 7},(void*)& Cyc__genrep_327};static char
_tmp200[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_345={offsetof(struct
Cyc_Absyn_Tuniondecl,is_xtunion),(struct _dynforward_ptr){_tmp200,_tmp200 + 11},(
void*)((void*)& Cyc__genrep_10)};static char _tmp201[8]="is_flat";static struct
_tuple5 Cyc__gentuple_346={offsetof(struct Cyc_Absyn_Tuniondecl,is_flat),(struct
_dynforward_ptr){_tmp201,_tmp201 + 8},(void*)((void*)& Cyc__genrep_10)};static
struct _tuple5*Cyc__genarr_347[6]={& Cyc__gentuple_341,& Cyc__gentuple_342,& Cyc__gentuple_343,&
Cyc__gentuple_344,& Cyc__gentuple_345,& Cyc__gentuple_346};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_348,
sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((struct _tuple5**)Cyc__genarr_347),(
void*)((struct _tuple5**)Cyc__genarr_347 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_326={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1091={1,1,(void*)((void*)& Cyc__genrep_326)};
struct _tuple20{unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct
_tuple6 Cyc__gentuple_1092={offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1093={offsetof(struct _tuple20,f2),(void*)& Cyc__genrep_1091};
static struct _tuple6*Cyc__genarr_1094[2]={& Cyc__gentuple_1092,& Cyc__gentuple_1093};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1090={4,sizeof(struct _tuple20),{(
void*)((struct _tuple6**)Cyc__genarr_1094),(void*)((struct _tuple6**)Cyc__genarr_1094
+ 2)}};static struct _tuple7*Cyc__genarr_1089[0]={};static char _tmp206[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1104={0,(struct _dynforward_ptr){_tmp206,_tmp206
+ 14},(void*)& Cyc__genrep_1095};static char _tmp207[12]="KnownTunion";static struct
_tuple5 Cyc__gentuple_1105={1,(struct _dynforward_ptr){_tmp207,_tmp207 + 12},(void*)&
Cyc__genrep_1090};static struct _tuple5*Cyc__genarr_1106[2]={& Cyc__gentuple_1104,&
Cyc__gentuple_1105};static char _tmp209[12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionInfoU_rep={5,(struct _dynforward_ptr){_tmp209,_tmp209 + 12},{(
void*)((struct _tuple7**)Cyc__genarr_1089),(void*)((struct _tuple7**)Cyc__genarr_1089
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1106),(void*)((struct _tuple5**)Cyc__genarr_1106
+ 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_type_t46H2_rep;static char _tmp20A[5]="List";static
struct _dynforward_ptr Cyc__genname_125=(struct _dynforward_ptr){_tmp20A,_tmp20A + 5};
static char _tmp20B[3]="hd";static struct _tuple5 Cyc__gentuple_122={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp20B,_tmp20B + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp20C[3]="tl";static struct _tuple5 Cyc__gentuple_123={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp20C,_tmp20C + 3},(
void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,&
Cyc__gentuple_123};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};
static char _tmp20F[11]="TunionInfo";static struct _dynforward_ptr Cyc__genname_1111=(
struct _dynforward_ptr){_tmp20F,_tmp20F + 11};static char _tmp210[12]="tunion_info";
static struct _tuple5 Cyc__gentuple_1107={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),(
struct _dynforward_ptr){_tmp210,_tmp210 + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};
static char _tmp211[6]="targs";static struct _tuple5 Cyc__gentuple_1108={offsetof(
struct Cyc_Absyn_TunionInfo,targs),(struct _dynforward_ptr){_tmp211,_tmp211 + 6},(
void*)& Cyc__genrep_121};static char _tmp212[4]="rgn";static struct _tuple5 Cyc__gentuple_1109={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),(struct _dynforward_ptr){_tmp212,_tmp212
+ 4},(void*)& Cyc__genrep_110};static struct _tuple5*Cyc__genarr_1110[3]={& Cyc__gentuple_1107,&
Cyc__gentuple_1108,& Cyc__gentuple_1109};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1111,sizeof(struct Cyc_Absyn_TunionInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1110),(void*)((struct _tuple5**)Cyc__genarr_1110
+ 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo f2;};static struct
_tuple6 Cyc__gentuple_1112={offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1113={offsetof(struct _tuple21,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple6*Cyc__genarr_1114[2]={& Cyc__gentuple_1112,& Cyc__gentuple_1113};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1088={4,sizeof(struct _tuple21),{(
void*)((struct _tuple6**)Cyc__genarr_1114),(void*)((struct _tuple6**)Cyc__genarr_1114
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1062;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1069;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp215[23]="UnknownTunionFieldInfo";
static struct _dynforward_ptr Cyc__genname_1074=(struct _dynforward_ptr){_tmp215,
_tmp215 + 23};static char _tmp216[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1070={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),(struct
_dynforward_ptr){_tmp216,_tmp216 + 12},(void*)& Cyc__genrep_11};static char _tmp217[
11]="field_name";static struct _tuple5 Cyc__gentuple_1071={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),(
struct _dynforward_ptr){_tmp217,_tmp217 + 11},(void*)& Cyc__genrep_11};static char
_tmp218[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_1072={offsetof(struct
Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),(struct _dynforward_ptr){_tmp218,
_tmp218 + 11},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_1073[
3]={& Cyc__gentuple_1070,& Cyc__gentuple_1071,& Cyc__gentuple_1072};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1074,
sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1073),(
void*)((struct _tuple5**)Cyc__genarr_1073 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1075={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1076={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1077[2]={& Cyc__gentuple_1075,& Cyc__gentuple_1076};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1069={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1077),(void*)((struct _tuple6**)Cyc__genarr_1077
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1064;struct _tuple23{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;};
static struct _tuple6 Cyc__gentuple_1065={offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1066={offsetof(struct _tuple23,f2),(void*)((void*)&
Cyc__genrep_326)};static struct _tuple6 Cyc__gentuple_1067={offsetof(struct _tuple23,f3),(
void*)& Cyc__genrep_309};static struct _tuple6*Cyc__genarr_1068[3]={& Cyc__gentuple_1065,&
Cyc__gentuple_1066,& Cyc__gentuple_1067};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1064={
4,sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1068),(void*)((
struct _tuple6**)Cyc__genarr_1068 + 3)}};static struct _tuple7*Cyc__genarr_1063[0]={};
static char _tmp21C[19]="UnknownTunionfield";static struct _tuple5 Cyc__gentuple_1078={
0,(struct _dynforward_ptr){_tmp21C,_tmp21C + 19},(void*)& Cyc__genrep_1069};static
char _tmp21D[17]="KnownTunionfield";static struct _tuple5 Cyc__gentuple_1079={1,(
struct _dynforward_ptr){_tmp21D,_tmp21D + 17},(void*)& Cyc__genrep_1064};static
struct _tuple5*Cyc__genarr_1080[2]={& Cyc__gentuple_1078,& Cyc__gentuple_1079};
static char _tmp21F[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,(struct _dynforward_ptr){_tmp21F,_tmp21F + 17},{(void*)((struct _tuple7**)Cyc__genarr_1063),(
void*)((struct _tuple7**)Cyc__genarr_1063 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1080),(
void*)((struct _tuple5**)Cyc__genarr_1080 + 2)}};static char _tmp220[16]="TunionFieldInfo";
static struct _dynforward_ptr Cyc__genname_1084=(struct _dynforward_ptr){_tmp220,
_tmp220 + 16};static char _tmp221[11]="field_info";static struct _tuple5 Cyc__gentuple_1081={
offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),(struct _dynforward_ptr){
_tmp221,_tmp221 + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char
_tmp222[6]="targs";static struct _tuple5 Cyc__gentuple_1082={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),(
struct _dynforward_ptr){_tmp222,_tmp222 + 6},(void*)& Cyc__genrep_121};static struct
_tuple5*Cyc__genarr_1083[2]={& Cyc__gentuple_1081,& Cyc__gentuple_1082};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1084,
sizeof(struct Cyc_Absyn_TunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1083),(
void*)((struct _tuple5**)Cyc__genarr_1083 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1085={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1086={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1087[2]={& Cyc__gentuple_1085,& Cyc__gentuple_1086};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1062={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1087),(void*)((struct _tuple6**)Cyc__genarr_1087
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1036;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_info_t_rep;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1001;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple25,f2),(
void*)((void*)& Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1004;struct
_tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static struct _tuple6 Cyc__gentuple_1005={
offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1006={
offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_1001};static struct _tuple6*Cyc__genarr_1007[
2]={& Cyc__gentuple_1005,& Cyc__gentuple_1006};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1004={4,sizeof(struct _tuple26),{(void*)((struct _tuple6**)Cyc__genarr_1007),(
void*)((struct _tuple6**)Cyc__genarr_1007 + 2)}};static char _tmp227[10]="No_constr";
static struct _tuple7 Cyc__gentuple_1002={0,(struct _dynforward_ptr){_tmp227,_tmp227
+ 10}};static struct _tuple7*Cyc__genarr_1003[1]={& Cyc__gentuple_1002};static char
_tmp228[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_1008={0,(struct
_dynforward_ptr){_tmp228,_tmp228 + 10},(void*)& Cyc__genrep_85};static char _tmp229[
15]="Forward_constr";static struct _tuple5 Cyc__gentuple_1009={1,(struct
_dynforward_ptr){_tmp229,_tmp229 + 15},(void*)& Cyc__genrep_1004};static struct
_tuple5*Cyc__genarr_1010[2]={& Cyc__gentuple_1008,& Cyc__gentuple_1009};static char
_tmp22B[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={
5,(struct _dynforward_ptr){_tmp22B,_tmp22B + 11},{(void*)((struct _tuple7**)Cyc__genarr_1003),(
void*)((struct _tuple7**)Cyc__genarr_1003 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_1010),(
void*)((struct _tuple5**)Cyc__genarr_1010 + 2)}};static char _tmp22C[7]="Conref";
static struct _dynforward_ptr Cyc__genname_1013=(struct _dynforward_ptr){_tmp22C,
_tmp22C + 7};static char _tmp22D[2]="v";static struct _tuple5 Cyc__gentuple_1011={
offsetof(struct Cyc_Absyn_Conref,v),(struct _dynforward_ptr){_tmp22D,_tmp22D + 2},(
void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};static struct _tuple5*Cyc__genarr_1012[
1]={& Cyc__gentuple_1011};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1013,sizeof(struct Cyc_Absyn_Conref),{(
void*)((struct _tuple5**)Cyc__genarr_1012),(void*)((struct _tuple5**)Cyc__genarr_1012
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1001={1,1,(void*)((void*)&
Cyc_struct_Absyn_Conref0bool2_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1043;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static char _tmp230[7]="Conref";static struct _dynforward_ptr Cyc__genname_1046=(
struct _dynforward_ptr){_tmp230,_tmp230 + 7};static char _tmp231[2]="v";static struct
_tuple5 Cyc__gentuple_1044={offsetof(struct Cyc_Absyn_Conref,v),(struct
_dynforward_ptr){_tmp231,_tmp231 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_1045[1]={& Cyc__gentuple_1044};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1046,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1045),(void*)((
struct _tuple5**)Cyc__genarr_1045 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1043={
1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_1037;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp234[7]="PtrLoc";static struct
_dynforward_ptr Cyc__genname_1042=(struct _dynforward_ptr){_tmp234,_tmp234 + 7};
static char _tmp235[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1038={offsetof(
struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _dynforward_ptr){_tmp235,_tmp235 + 8},(
void*)& Cyc__genrep_2};static char _tmp236[8]="rgn_loc";static struct _tuple5 Cyc__gentuple_1039={
offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct _dynforward_ptr){_tmp236,_tmp236
+ 8},(void*)& Cyc__genrep_2};static char _tmp237[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1040={
offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(struct _dynforward_ptr){_tmp237,_tmp237 + 
7},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1041[3]={& Cyc__gentuple_1038,&
Cyc__gentuple_1039,& Cyc__gentuple_1040};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1042,sizeof(struct Cyc_Absyn_PtrLoc),{(
void*)((struct _tuple5**)Cyc__genarr_1041),(void*)((struct _tuple5**)Cyc__genarr_1041
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1037={1,1,(void*)((void*)&
Cyc_struct_Absyn_PtrLoc_rep)};static char _tmp23A[8]="PtrAtts";static struct
_dynforward_ptr Cyc__genname_1053=(struct _dynforward_ptr){_tmp23A,_tmp23A + 8};
static char _tmp23B[4]="rgn";static struct _tuple5 Cyc__gentuple_1047={offsetof(
struct Cyc_Absyn_PtrAtts,rgn),(struct _dynforward_ptr){_tmp23B,_tmp23B + 4},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp23C[9]="nullable";static struct
_tuple5 Cyc__gentuple_1048={offsetof(struct Cyc_Absyn_PtrAtts,nullable),(struct
_dynforward_ptr){_tmp23C,_tmp23C + 9},(void*)& Cyc__genrep_1001};static char _tmp23D[
7]="bounds";static struct _tuple5 Cyc__gentuple_1049={offsetof(struct Cyc_Absyn_PtrAtts,bounds),(
struct _dynforward_ptr){_tmp23D,_tmp23D + 7},(void*)& Cyc__genrep_1043};static char
_tmp23E[10]="zero_term";static struct _tuple5 Cyc__gentuple_1050={offsetof(struct
Cyc_Absyn_PtrAtts,zero_term),(struct _dynforward_ptr){_tmp23E,_tmp23E + 10},(void*)&
Cyc__genrep_1001};static char _tmp23F[7]="ptrloc";static struct _tuple5 Cyc__gentuple_1051={
offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct _dynforward_ptr){_tmp23F,_tmp23F
+ 7},(void*)& Cyc__genrep_1037};static struct _tuple5*Cyc__genarr_1052[5]={& Cyc__gentuple_1047,&
Cyc__gentuple_1048,& Cyc__gentuple_1049,& Cyc__gentuple_1050,& Cyc__gentuple_1051};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1053,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1052),(void*)((struct _tuple5**)Cyc__genarr_1052 + 5)}};
static char _tmp241[8]="PtrInfo";static struct _dynforward_ptr Cyc__genname_1058=(
struct _dynforward_ptr){_tmp241,_tmp241 + 8};static char _tmp242[8]="elt_typ";static
struct _tuple5 Cyc__gentuple_1054={offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(
struct _dynforward_ptr){_tmp242,_tmp242 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp243[7]="elt_tq";static struct _tuple5 Cyc__gentuple_1055={offsetof(
struct Cyc_Absyn_PtrInfo,elt_tq),(struct _dynforward_ptr){_tmp243,_tmp243 + 7},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp244[9]="ptr_atts";static struct
_tuple5 Cyc__gentuple_1056={offsetof(struct Cyc_Absyn_PtrInfo,ptr_atts),(struct
_dynforward_ptr){_tmp244,_tmp244 + 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static
struct _tuple5*Cyc__genarr_1057[3]={& Cyc__gentuple_1054,& Cyc__gentuple_1055,& Cyc__gentuple_1056};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1058,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1057),(void*)((struct _tuple5**)Cyc__genarr_1057 + 3)}};
struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1059={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1060={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1061[2]={& Cyc__gentuple_1059,& Cyc__gentuple_1060};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1036={4,sizeof(struct _tuple27),{(void*)((
struct _tuple6**)Cyc__genarr_1061),(void*)((struct _tuple6**)Cyc__genarr_1061 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1024;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep;static char _tmp247[7]="Signed";static struct _tuple7 Cyc__gentuple_299={
0,(struct _dynforward_ptr){_tmp247,_tmp247 + 7}};static char _tmp248[9]="Unsigned";
static struct _tuple7 Cyc__gentuple_300={1,(struct _dynforward_ptr){_tmp248,_tmp248 + 
9}};static char _tmp249[5]="None";static struct _tuple7 Cyc__gentuple_301={2,(struct
_dynforward_ptr){_tmp249,_tmp249 + 5}};static struct _tuple7*Cyc__genarr_302[3]={&
Cyc__gentuple_299,& Cyc__gentuple_300,& Cyc__gentuple_301};static struct _tuple5*Cyc__genarr_303[
0]={};static char _tmp24B[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={
5,(struct _dynforward_ptr){_tmp24B,_tmp24B + 5},{(void*)((struct _tuple7**)Cyc__genarr_302),(
void*)((struct _tuple7**)Cyc__genarr_302 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_303),(
void*)((struct _tuple5**)Cyc__genarr_303 + 0)}};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_size_of_t_rep;static char _tmp24C[8]="Char_sz";static struct _tuple7 Cyc__gentuple_1025={
0,(struct _dynforward_ptr){_tmp24C,_tmp24C + 8}};static char _tmp24D[9]="Short_sz";
static struct _tuple7 Cyc__gentuple_1026={1,(struct _dynforward_ptr){_tmp24D,_tmp24D
+ 9}};static char _tmp24E[7]="Int_sz";static struct _tuple7 Cyc__gentuple_1027={2,(
struct _dynforward_ptr){_tmp24E,_tmp24E + 7}};static char _tmp24F[8]="Long_sz";
static struct _tuple7 Cyc__gentuple_1028={3,(struct _dynforward_ptr){_tmp24F,_tmp24F
+ 8}};static char _tmp250[12]="LongLong_sz";static struct _tuple7 Cyc__gentuple_1029={
4,(struct _dynforward_ptr){_tmp250,_tmp250 + 12}};static struct _tuple7*Cyc__genarr_1030[
5]={& Cyc__gentuple_1025,& Cyc__gentuple_1026,& Cyc__gentuple_1027,& Cyc__gentuple_1028,&
Cyc__gentuple_1029};static struct _tuple5*Cyc__genarr_1031[0]={};static char _tmp252[
8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={5,(struct
_dynforward_ptr){_tmp252,_tmp252 + 8},{(void*)((struct _tuple7**)Cyc__genarr_1030),(
void*)((struct _tuple7**)Cyc__genarr_1030 + 5)},{(void*)((struct _tuple5**)Cyc__genarr_1031),(
void*)((struct _tuple5**)Cyc__genarr_1031 + 0)}};struct _tuple28{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1032={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1033={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1034={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1035[3]={& Cyc__gentuple_1032,& Cyc__gentuple_1033,& Cyc__gentuple_1034};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1024={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_1035),(void*)((struct _tuple6**)Cyc__genarr_1035
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1000;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_array_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_875;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_890;struct _tuple29{unsigned int f1;void*f2;char f3;};static struct
_tuple6 Cyc__gentuple_891={offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_892={offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_893={offsetof(struct _tuple29,f3),(void*)((void*)&
Cyc__genrep_15)};static struct _tuple6*Cyc__genarr_894[3]={& Cyc__gentuple_891,& Cyc__gentuple_892,&
Cyc__gentuple_893};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_890={4,
sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_894),(void*)((
struct _tuple6**)Cyc__genarr_894 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_884;
static struct Cyc_Typerep_Int_struct Cyc__genrep_885={0,1,16};struct _tuple30{
unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_886={offsetof(
struct _tuple30,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_887={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_888={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_885};static struct _tuple6*Cyc__genarr_889[
3]={& Cyc__gentuple_886,& Cyc__gentuple_887,& Cyc__gentuple_888};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_884={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_889),(
void*)((struct _tuple6**)Cyc__genarr_889 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_298;struct _tuple31{unsigned int f1;void*f2;int f3;};static struct
_tuple6 Cyc__gentuple_304={offsetof(struct _tuple31,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_305={offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_306={offsetof(struct _tuple31,f3),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_307[3]={& Cyc__gentuple_304,& Cyc__gentuple_305,&
Cyc__gentuple_306};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_298={4,
sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_307),(void*)((
struct _tuple6**)Cyc__genarr_307 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_878;
static struct Cyc_Typerep_Int_struct Cyc__genrep_879={0,1,64};struct _tuple32{
unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_880={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_881={
offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_882={
offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_879};static struct _tuple6*Cyc__genarr_883[
3]={& Cyc__gentuple_880,& Cyc__gentuple_881,& Cyc__gentuple_882};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_878={4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_883),(
void*)((struct _tuple6**)Cyc__genarr_883 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={offsetof(struct _tuple7,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={offsetof(struct
_tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_84[
2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={
4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(void*)((
struct _tuple6**)Cyc__genarr_84 + 2)}};static char _tmp25B[7]="Null_c";static struct
_tuple7 Cyc__gentuple_876={0,(struct _dynforward_ptr){_tmp25B,_tmp25B + 7}};static
struct _tuple7*Cyc__genarr_877[1]={& Cyc__gentuple_876};static char _tmp25C[7]="Char_c";
static struct _tuple5 Cyc__gentuple_895={0,(struct _dynforward_ptr){_tmp25C,_tmp25C + 
7},(void*)& Cyc__genrep_890};static char _tmp25D[8]="Short_c";static struct _tuple5
Cyc__gentuple_896={1,(struct _dynforward_ptr){_tmp25D,_tmp25D + 8},(void*)& Cyc__genrep_884};
static char _tmp25E[6]="Int_c";static struct _tuple5 Cyc__gentuple_897={2,(struct
_dynforward_ptr){_tmp25E,_tmp25E + 6},(void*)& Cyc__genrep_298};static char _tmp25F[
11]="LongLong_c";static struct _tuple5 Cyc__gentuple_898={3,(struct _dynforward_ptr){
_tmp25F,_tmp25F + 11},(void*)& Cyc__genrep_878};static char _tmp260[8]="Float_c";
static struct _tuple5 Cyc__gentuple_899={4,(struct _dynforward_ptr){_tmp260,_tmp260 + 
8},(void*)& Cyc__genrep_81};static char _tmp261[9]="String_c";static struct _tuple5
Cyc__gentuple_900={5,(struct _dynforward_ptr){_tmp261,_tmp261 + 9},(void*)& Cyc__genrep_81};
static struct _tuple5*Cyc__genarr_901[6]={& Cyc__gentuple_895,& Cyc__gentuple_896,&
Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900};
static char _tmp263[5]="Cnst";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={
5,(struct _dynforward_ptr){_tmp263,_tmp263 + 5},{(void*)((struct _tuple7**)Cyc__genarr_877),(
void*)((struct _tuple7**)Cyc__genarr_877 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_901),(
void*)((struct _tuple5**)Cyc__genarr_901 + 6)}};struct _tuple33{unsigned int f1;
union Cyc_Absyn_Cnst_union f2;};static struct _tuple6 Cyc__gentuple_902={offsetof(
struct _tuple33,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_903={
offsetof(struct _tuple33,f2),(void*)& Cyc_Absyn_cnst_t_rep};static struct _tuple6*
Cyc__genarr_904[2]={& Cyc__gentuple_902,& Cyc__gentuple_903};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_875={4,sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_904),(
void*)((struct _tuple6**)Cyc__genarr_904 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_823;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_830;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_831;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_834;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_835;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_836;struct _tuple34{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple6 Cyc__gentuple_837={offsetof(struct _tuple34,f1),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_838={offsetof(struct
_tuple34,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_839={
offsetof(struct _tuple34,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_840[3]={& Cyc__gentuple_837,& Cyc__gentuple_838,& Cyc__gentuple_839};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_836={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_840),(void*)((struct _tuple6**)Cyc__genarr_840
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_835={1,1,(void*)((void*)&
Cyc__genrep_836)};static char _tmp267[5]="List";static struct _dynforward_ptr Cyc__genname_844=(
struct _dynforward_ptr){_tmp267,_tmp267 + 5};static char _tmp268[3]="hd";static
struct _tuple5 Cyc__gentuple_841={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp268,_tmp268 + 3},(void*)& Cyc__genrep_835};static char _tmp269[
3]="tl";static struct _tuple5 Cyc__gentuple_842={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp269,_tmp269 + 3},(void*)& Cyc__genrep_834};static struct
_tuple5*Cyc__genarr_843[2]={& Cyc__gentuple_841,& Cyc__gentuple_842};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_844,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_843),(void*)((struct _tuple5**)Cyc__genarr_843 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_834={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_833;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_683;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp26C[4]="Opt";static struct _dynforward_ptr Cyc__genname_686=(struct
_dynforward_ptr){_tmp26C,_tmp26C + 4};static char _tmp26D[2]="v";static struct
_tuple5 Cyc__gentuple_684={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp26D,_tmp26D + 2},(void*)& Cyc__genrep_13};static struct _tuple5*Cyc__genarr_685[
1]={& Cyc__gentuple_684};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_686,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_685),(void*)((struct _tuple5**)Cyc__genarr_685 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_683={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp270[11]="VarargInfo";static struct _dynforward_ptr Cyc__genname_753=(
struct _dynforward_ptr){_tmp270,_tmp270 + 11};static char _tmp271[5]="name";static
struct _tuple5 Cyc__gentuple_748={offsetof(struct Cyc_Absyn_VarargInfo,name),(
struct _dynforward_ptr){_tmp271,_tmp271 + 5},(void*)& Cyc__genrep_683};static char
_tmp272[3]="tq";static struct _tuple5 Cyc__gentuple_749={offsetof(struct Cyc_Absyn_VarargInfo,tq),(
struct _dynforward_ptr){_tmp272,_tmp272 + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp273[5]="type";static struct _tuple5 Cyc__gentuple_750={offsetof(struct Cyc_Absyn_VarargInfo,type),(
struct _dynforward_ptr){_tmp273,_tmp273 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp274[7]="inject";static struct _tuple5 Cyc__gentuple_751={offsetof(
struct Cyc_Absyn_VarargInfo,inject),(struct _dynforward_ptr){_tmp274,_tmp274 + 7},(
void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_752[4]={& Cyc__gentuple_748,&
Cyc__gentuple_749,& Cyc__gentuple_750,& Cyc__gentuple_751};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_753,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_752),(void*)((
struct _tuple5**)Cyc__genarr_752 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_833={
1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_397;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_398;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_399;static struct _tuple6 Cyc__gentuple_400={offsetof(struct _tuple9,f1),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_401={
offsetof(struct _tuple9,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_402[2]={& Cyc__gentuple_400,& Cyc__gentuple_401};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_399={4,sizeof(struct _tuple9),{(void*)((
struct _tuple6**)Cyc__genarr_402),(void*)((struct _tuple6**)Cyc__genarr_402 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_398={1,1,(void*)((void*)& Cyc__genrep_399)};
static char _tmp279[5]="List";static struct _dynforward_ptr Cyc__genname_406=(struct
_dynforward_ptr){_tmp279,_tmp279 + 5};static char _tmp27A[3]="hd";static struct
_tuple5 Cyc__gentuple_403={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp27A,_tmp27A + 3},(void*)& Cyc__genrep_398};static char _tmp27B[
3]="tl";static struct _tuple5 Cyc__gentuple_404={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp27B,_tmp27B + 3},(void*)& Cyc__genrep_397};static struct
_tuple5*Cyc__genarr_405[2]={& Cyc__gentuple_403,& Cyc__gentuple_404};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_406,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_405),(void*)((struct _tuple5**)Cyc__genarr_405 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_397={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_150;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_231;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_224;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_224={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple35{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_232={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_233={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_224};static struct _tuple6*Cyc__genarr_234[2]={& Cyc__gentuple_232,&
Cyc__gentuple_233};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_231={4,
sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_234),(void*)((
struct _tuple6**)Cyc__genarr_234 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_567;
struct _tuple36{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
static struct _tuple6 Cyc__gentuple_568={offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_569={offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_150};
static struct _tuple6 Cyc__gentuple_570={offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_571[3]={& Cyc__gentuple_568,& Cyc__gentuple_569,&
Cyc__gentuple_570};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_567={4,
sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_571),(void*)((
struct _tuple6**)Cyc__genarr_571 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_563;
static struct _tuple6 Cyc__gentuple_564={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_565={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_146};
static struct _tuple6*Cyc__genarr_566[2]={& Cyc__gentuple_564,& Cyc__gentuple_565};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_563={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_566),(void*)((struct _tuple6**)Cyc__genarr_566
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_557;struct _tuple37{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*f3;struct Cyc_Absyn_Stmt*
f4;};static struct _tuple6 Cyc__gentuple_558={offsetof(struct _tuple37,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_559={offsetof(struct _tuple37,f2),(
void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_560={offsetof(struct
_tuple37,f3),(void*)& Cyc__genrep_150};static struct _tuple6 Cyc__gentuple_561={
offsetof(struct _tuple37,f4),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_562[
4]={& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560,& Cyc__gentuple_561};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_557={4,sizeof(struct _tuple37),{(
void*)((struct _tuple6**)Cyc__genarr_562),(void*)((struct _tuple6**)Cyc__genarr_562
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_552;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_501;static struct _tuple6 Cyc__gentuple_502={offsetof(struct _tuple2,f1),(
void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_503={offsetof(struct
_tuple2,f2),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_504[2]={&
Cyc__gentuple_502,& Cyc__gentuple_503};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_501={
4,sizeof(struct _tuple2),{(void*)((struct _tuple6**)Cyc__genarr_504),(void*)((
struct _tuple6**)Cyc__genarr_504 + 2)}};struct _tuple38{unsigned int f1;struct
_tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_553={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_554={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_501};static struct _tuple6 Cyc__gentuple_555={
offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_556[
3]={& Cyc__gentuple_553,& Cyc__gentuple_554,& Cyc__gentuple_555};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_552={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_556),(
void*)((struct _tuple6**)Cyc__genarr_556 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_548;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_543;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_543={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
struct _tuple39{unsigned int f1;struct Cyc_Absyn_Stmt*f2;};static struct _tuple6 Cyc__gentuple_549={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_550={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_543};static struct _tuple6*Cyc__genarr_551[
2]={& Cyc__gentuple_549,& Cyc__gentuple_550};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_548={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_551),(
void*)((struct _tuple6**)Cyc__genarr_551 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_542;struct _tuple40{unsigned int f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Stmt*
f3;};static struct _tuple6 Cyc__gentuple_544={offsetof(struct _tuple40,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_545={offsetof(struct _tuple40,f2),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_546={offsetof(struct
_tuple40,f3),(void*)& Cyc__genrep_543};static struct _tuple6*Cyc__genarr_547[3]={&
Cyc__gentuple_544,& Cyc__gentuple_545,& Cyc__gentuple_546};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_542={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_547),(
void*)((struct _tuple6**)Cyc__genarr_547 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_535;struct _tuple41{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_536={
offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_537={
offsetof(struct _tuple41,f2),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple41,f3),(void*)& Cyc__genrep_501};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple41,f4),(void*)& Cyc__genrep_501};static struct _tuple6 Cyc__gentuple_540={
offsetof(struct _tuple41,f5),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_541[
5]={& Cyc__gentuple_536,& Cyc__gentuple_537,& Cyc__gentuple_538,& Cyc__gentuple_539,&
Cyc__gentuple_540};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_535={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_541),(void*)((
struct _tuple6**)Cyc__genarr_541 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_530;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_244;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_245;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_255;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_pat_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_459;struct _tuple42{unsigned int
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Pat*f3;};static struct _tuple6 Cyc__gentuple_460={
offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_461={
offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_462={
offsetof(struct _tuple42,f3),(void*)& Cyc__genrep_255};static struct _tuple6*Cyc__genarr_463[
3]={& Cyc__gentuple_460,& Cyc__gentuple_461,& Cyc__gentuple_462};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_459={4,sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_463),(
void*)((struct _tuple6**)Cyc__genarr_463 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_454;struct _tuple43{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*
f3;};static struct _tuple6 Cyc__gentuple_455={offsetof(struct _tuple43,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_456={offsetof(struct _tuple43,f2),(
void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_457={offsetof(struct
_tuple43,f3),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_458[3]={&
Cyc__gentuple_455,& Cyc__gentuple_456,& Cyc__gentuple_457};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_454={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_458),(
void*)((struct _tuple6**)Cyc__genarr_458 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_449;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_260;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;static
char _tmp28B[5]="List";static struct _dynforward_ptr Cyc__genname_264=(struct
_dynforward_ptr){_tmp28B,_tmp28B + 5};static char _tmp28C[3]="hd";static struct
_tuple5 Cyc__gentuple_261={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp28C,_tmp28C + 3},(void*)& Cyc__genrep_255};static char _tmp28D[
3]="tl";static struct _tuple5 Cyc__gentuple_262={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp28D,_tmp28D + 3},(void*)& Cyc__genrep_260};static struct
_tuple5*Cyc__genarr_263[2]={& Cyc__gentuple_261,& Cyc__gentuple_262};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_264,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_263),(void*)((
struct _tuple5**)Cyc__genarr_263 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_260={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};struct _tuple44{
unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6 Cyc__gentuple_450={
offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_451={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_260};static struct _tuple6 Cyc__gentuple_452={
offsetof(struct _tuple44,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_453[3]={& Cyc__gentuple_450,& Cyc__gentuple_451,& Cyc__gentuple_452};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_449={4,sizeof(struct _tuple44),{(
void*)((struct _tuple6**)Cyc__genarr_453),(void*)((struct _tuple6**)Cyc__genarr_453
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_445;struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_446={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_447={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_255};static struct _tuple6*Cyc__genarr_448[
2]={& Cyc__gentuple_446,& Cyc__gentuple_447};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_445={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_448),(
void*)((struct _tuple6**)Cyc__genarr_448 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_355;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_427;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static char _tmp292[8]="StructA";static struct _tuple7 Cyc__gentuple_412={0,(struct
_dynforward_ptr){_tmp292,_tmp292 + 8}};static char _tmp293[7]="UnionA";static struct
_tuple7 Cyc__gentuple_413={1,(struct _dynforward_ptr){_tmp293,_tmp293 + 7}};static
struct _tuple7*Cyc__genarr_414[2]={& Cyc__gentuple_412,& Cyc__gentuple_413};static
struct _tuple5*Cyc__genarr_415[0]={};static char _tmp295[9]="AggrKind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep={5,(struct _dynforward_ptr){_tmp295,_tmp295 + 9},{(void*)((
struct _tuple7**)Cyc__genarr_414),(void*)((struct _tuple7**)Cyc__genarr_414 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_415),(void*)((struct _tuple5**)Cyc__genarr_415
+ 0)}};struct _tuple46{unsigned int f1;void*f2;struct _tuple0*f3;};static struct
_tuple6 Cyc__gentuple_428={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_429={offsetof(struct _tuple46,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_430={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_431[3]={& Cyc__gentuple_428,& Cyc__gentuple_429,&
Cyc__gentuple_430};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_427={4,
sizeof(struct _tuple46),{(void*)((struct _tuple6**)Cyc__genarr_431),(void*)((
struct _tuple6**)Cyc__genarr_431 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_380;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_382;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_383;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_AggrdeclImpl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_384;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_385;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Aggrfield_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_89;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;
static char _tmp297[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_dynforward_ptr){_tmp297,_tmp297 + 10}};static char _tmp298[9]="Scanf_ft";static
struct _tuple7 Cyc__gentuple_91={1,(struct _dynforward_ptr){_tmp298,_tmp298 + 9}};
static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp29A[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,(struct
_dynforward_ptr){_tmp29A,_tmp29A + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple47{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple47,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple47,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple47,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_97={offsetof(struct _tuple47,f4),(void*)((void*)& Cyc__genrep_10)};
static struct _tuple6*Cyc__genarr_98[4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,&
Cyc__gentuple_97};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(
struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)
Cyc__genarr_98 + 4)}};static char _tmp29C[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_63={
0,(struct _dynforward_ptr){_tmp29C,_tmp29C + 12}};static char _tmp29D[10]="Cdecl_att";
static struct _tuple7 Cyc__gentuple_64={1,(struct _dynforward_ptr){_tmp29D,_tmp29D + 
10}};static char _tmp29E[13]="Fastcall_att";static struct _tuple7 Cyc__gentuple_65={2,(
struct _dynforward_ptr){_tmp29E,_tmp29E + 13}};static char _tmp29F[13]="Noreturn_att";
static struct _tuple7 Cyc__gentuple_66={3,(struct _dynforward_ptr){_tmp29F,_tmp29F + 
13}};static char _tmp2A0[10]="Const_att";static struct _tuple7 Cyc__gentuple_67={4,(
struct _dynforward_ptr){_tmp2A0,_tmp2A0 + 10}};static char _tmp2A1[11]="Packed_att";
static struct _tuple7 Cyc__gentuple_68={5,(struct _dynforward_ptr){_tmp2A1,_tmp2A1 + 
11}};static char _tmp2A2[13]="Nocommon_att";static struct _tuple7 Cyc__gentuple_69={6,(
struct _dynforward_ptr){_tmp2A2,_tmp2A2 + 13}};static char _tmp2A3[11]="Shared_att";
static struct _tuple7 Cyc__gentuple_70={7,(struct _dynforward_ptr){_tmp2A3,_tmp2A3 + 
11}};static char _tmp2A4[11]="Unused_att";static struct _tuple7 Cyc__gentuple_71={8,(
struct _dynforward_ptr){_tmp2A4,_tmp2A4 + 11}};static char _tmp2A5[9]="Weak_att";
static struct _tuple7 Cyc__gentuple_72={9,(struct _dynforward_ptr){_tmp2A5,_tmp2A5 + 
9}};static char _tmp2A6[14]="Dllimport_att";static struct _tuple7 Cyc__gentuple_73={
10,(struct _dynforward_ptr){_tmp2A6,_tmp2A6 + 14}};static char _tmp2A7[14]="Dllexport_att";
static struct _tuple7 Cyc__gentuple_74={11,(struct _dynforward_ptr){_tmp2A7,_tmp2A7 + 
14}};static char _tmp2A8[27]="No_instrument_function_att";static struct _tuple7 Cyc__gentuple_75={
12,(struct _dynforward_ptr){_tmp2A8,_tmp2A8 + 27}};static char _tmp2A9[16]="Constructor_att";
static struct _tuple7 Cyc__gentuple_76={13,(struct _dynforward_ptr){_tmp2A9,_tmp2A9 + 
16}};static char _tmp2AA[15]="Destructor_att";static struct _tuple7 Cyc__gentuple_77={
14,(struct _dynforward_ptr){_tmp2AA,_tmp2AA + 15}};static char _tmp2AB[26]="No_check_memory_usage_att";
static struct _tuple7 Cyc__gentuple_78={15,(struct _dynforward_ptr){_tmp2AB,_tmp2AB + 
26}};static char _tmp2AC[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(
struct _dynforward_ptr){_tmp2AC,_tmp2AC + 9}};static struct _tuple7*Cyc__genarr_80[
17]={& Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,&
Cyc__gentuple_67,& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2AD[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_99={0,(struct _dynforward_ptr){_tmp2AD,_tmp2AD + 
12},(void*)& Cyc__genrep_85};static char _tmp2AE[12]="Aligned_att";static struct
_tuple5 Cyc__gentuple_100={1,(struct _dynforward_ptr){_tmp2AE,_tmp2AE + 12},(void*)&
Cyc__genrep_85};static char _tmp2AF[12]="Section_att";static struct _tuple5 Cyc__gentuple_101={
2,(struct _dynforward_ptr){_tmp2AF,_tmp2AF + 12},(void*)& Cyc__genrep_81};static
char _tmp2B0[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={3,(struct
_dynforward_ptr){_tmp2B0,_tmp2B0 + 11},(void*)& Cyc__genrep_89};static char _tmp2B1[
16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(struct
_dynforward_ptr){_tmp2B1,_tmp2B1 + 16},(void*)& Cyc__genrep_85};static char _tmp2B2[
9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct _dynforward_ptr){
_tmp2B2,_tmp2B2 + 9},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_105[
6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,& Cyc__gentuple_102,&
Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2B4[10]="Attribute";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct _dynforward_ptr){
_tmp2B4,_tmp2B4 + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(void*)((struct
_tuple7**)Cyc__genarr_80 + 17)},{(void*)((struct _tuple5**)Cyc__genarr_105),(void*)((
struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2B5[5]="List";static struct
_dynforward_ptr Cyc__genname_109=(struct _dynforward_ptr){_tmp2B5,_tmp2B5 + 5};
static char _tmp2B6[3]="hd";static struct _tuple5 Cyc__gentuple_106={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2B6,_tmp2B6 + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static char _tmp2B7[3]="tl";static struct _tuple5 Cyc__gentuple_107={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2B7,_tmp2B7 + 3},(void*)& Cyc__genrep_62};
static struct _tuple5*Cyc__genarr_108[2]={& Cyc__gentuple_106,& Cyc__gentuple_107};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};
static char _tmp2BA[10]="Aggrfield";static struct _dynforward_ptr Cyc__genname_392=(
struct _dynforward_ptr){_tmp2BA,_tmp2BA + 10};static char _tmp2BB[5]="name";static
struct _tuple5 Cyc__gentuple_386={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_dynforward_ptr){_tmp2BB,_tmp2BB + 5},(void*)& Cyc__genrep_13};static char _tmp2BC[3]="tq";
static struct _tuple5 Cyc__gentuple_387={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _dynforward_ptr){_tmp2BC,_tmp2BC + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp2BD[5]="type";static struct _tuple5 Cyc__gentuple_388={offsetof(struct Cyc_Absyn_Aggrfield,type),(
struct _dynforward_ptr){_tmp2BD,_tmp2BD + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp2BE[6]="width";static struct _tuple5 Cyc__gentuple_389={offsetof(
struct Cyc_Absyn_Aggrfield,width),(struct _dynforward_ptr){_tmp2BE,_tmp2BE + 6},(
void*)& Cyc__genrep_146};static char _tmp2BF[11]="attributes";static struct _tuple5
Cyc__gentuple_390={offsetof(struct Cyc_Absyn_Aggrfield,attributes),(struct
_dynforward_ptr){_tmp2BF,_tmp2BF + 11},(void*)& Cyc__genrep_62};static struct
_tuple5*Cyc__genarr_391[5]={& Cyc__gentuple_386,& Cyc__gentuple_387,& Cyc__gentuple_388,&
Cyc__gentuple_389,& Cyc__gentuple_390};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_392,sizeof(struct Cyc_Absyn_Aggrfield),{(
void*)((struct _tuple5**)Cyc__genarr_391),(void*)((struct _tuple5**)Cyc__genarr_391
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_385={1,1,(void*)((void*)&
Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp2C2[5]="List";static struct
_dynforward_ptr Cyc__genname_396=(struct _dynforward_ptr){_tmp2C2,_tmp2C2 + 5};
static char _tmp2C3[3]="hd";static struct _tuple5 Cyc__gentuple_393={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2C3,_tmp2C3 + 3},(void*)& Cyc__genrep_385};
static char _tmp2C4[3]="tl";static struct _tuple5 Cyc__gentuple_394={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2C4,_tmp2C4 + 3},(void*)& Cyc__genrep_384};
static struct _tuple5*Cyc__genarr_395[2]={& Cyc__gentuple_393,& Cyc__gentuple_394};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_396,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_395),(void*)((struct _tuple5**)Cyc__genarr_395 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_384={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};
static char _tmp2C7[13]="AggrdeclImpl";static struct _dynforward_ptr Cyc__genname_411=(
struct _dynforward_ptr){_tmp2C7,_tmp2C7 + 13};static char _tmp2C8[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_407={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _dynforward_ptr){_tmp2C8,_tmp2C8 + 11},(void*)& Cyc__genrep_336};static char
_tmp2C9[7]="rgn_po";static struct _tuple5 Cyc__gentuple_408={offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),(
struct _dynforward_ptr){_tmp2C9,_tmp2C9 + 7},(void*)& Cyc__genrep_397};static char
_tmp2CA[7]="fields";static struct _tuple5 Cyc__gentuple_409={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(
struct _dynforward_ptr){_tmp2CA,_tmp2CA + 7},(void*)& Cyc__genrep_384};static struct
_tuple5*Cyc__genarr_410[3]={& Cyc__gentuple_407,& Cyc__gentuple_408,& Cyc__gentuple_409};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_411,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(void*)((
struct _tuple5**)Cyc__genarr_410),(void*)((struct _tuple5**)Cyc__genarr_410 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_383={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};
static char _tmp2CD[9]="Aggrdecl";static struct _dynforward_ptr Cyc__genname_423=(
struct _dynforward_ptr){_tmp2CD,_tmp2CD + 9};static char _tmp2CE[5]="kind";static
struct _tuple5 Cyc__gentuple_416={offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct
_dynforward_ptr){_tmp2CE,_tmp2CE + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};static
char _tmp2CF[3]="sc";static struct _tuple5 Cyc__gentuple_417={offsetof(struct Cyc_Absyn_Aggrdecl,sc),(
struct _dynforward_ptr){_tmp2CF,_tmp2CF + 3},(void*)& Cyc_Absyn_scope_t_rep};static
char _tmp2D0[5]="name";static struct _tuple5 Cyc__gentuple_418={offsetof(struct Cyc_Absyn_Aggrdecl,name),(
struct _dynforward_ptr){_tmp2D0,_tmp2D0 + 5},(void*)& Cyc__genrep_11};static char
_tmp2D1[4]="tvs";static struct _tuple5 Cyc__gentuple_419={offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(
struct _dynforward_ptr){_tmp2D1,_tmp2D1 + 4},(void*)& Cyc__genrep_336};static char
_tmp2D2[5]="impl";static struct _tuple5 Cyc__gentuple_420={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(
struct _dynforward_ptr){_tmp2D2,_tmp2D2 + 5},(void*)& Cyc__genrep_383};static char
_tmp2D3[11]="attributes";static struct _tuple5 Cyc__gentuple_421={offsetof(struct
Cyc_Absyn_Aggrdecl,attributes),(struct _dynforward_ptr){_tmp2D3,_tmp2D3 + 11},(
void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_422[6]={& Cyc__gentuple_416,&
Cyc__gentuple_417,& Cyc__gentuple_418,& Cyc__gentuple_419,& Cyc__gentuple_420,& Cyc__gentuple_421};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_423,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((
struct _tuple5**)Cyc__genarr_422),(void*)((struct _tuple5**)Cyc__genarr_422 + 6)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_382={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381={1,1,(void*)((void*)& Cyc__genrep_382)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6
Cyc__gentuple_424={offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_425={offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_381};
static struct _tuple6*Cyc__genarr_426[2]={& Cyc__gentuple_424,& Cyc__gentuple_425};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_380={4,sizeof(struct _tuple48),{(
void*)((struct _tuple6**)Cyc__genarr_426),(void*)((struct _tuple6**)Cyc__genarr_426
+ 2)}};static struct _tuple7*Cyc__genarr_379[0]={};static char _tmp2D8[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_432={0,(struct _dynforward_ptr){_tmp2D8,_tmp2D8 + 
12},(void*)& Cyc__genrep_427};static char _tmp2D9[10]="KnownAggr";static struct
_tuple5 Cyc__gentuple_433={1,(struct _dynforward_ptr){_tmp2D9,_tmp2D9 + 10},(void*)&
Cyc__genrep_380};static struct _tuple5*Cyc__genarr_434[2]={& Cyc__gentuple_432,& Cyc__gentuple_433};
static char _tmp2DB[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={
5,(struct _dynforward_ptr){_tmp2DB,_tmp2DB + 10},{(void*)((struct _tuple7**)Cyc__genarr_379),(
void*)((struct _tuple7**)Cyc__genarr_379 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_434),(
void*)((struct _tuple5**)Cyc__genarr_434 + 2)}};static char _tmp2DC[9]="AggrInfo";
static struct _dynforward_ptr Cyc__genname_438=(struct _dynforward_ptr){_tmp2DC,
_tmp2DC + 9};static char _tmp2DD[10]="aggr_info";static struct _tuple5 Cyc__gentuple_435={
offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(struct _dynforward_ptr){_tmp2DD,
_tmp2DD + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char _tmp2DE[6]="targs";
static struct _tuple5 Cyc__gentuple_436={offsetof(struct Cyc_Absyn_AggrInfo,targs),(
struct _dynforward_ptr){_tmp2DE,_tmp2DE + 6},(void*)& Cyc__genrep_121};static struct
_tuple5*Cyc__genarr_437[2]={& Cyc__gentuple_435,& Cyc__gentuple_436};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_438,sizeof(
struct Cyc_Absyn_AggrInfo),{(void*)((struct _tuple5**)Cyc__genarr_437),(void*)((
struct _tuple5**)Cyc__genarr_437 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_358;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_359;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_361;struct _tuple49{unsigned int f1;struct _dynforward_ptr*f2;};static
struct _tuple6 Cyc__gentuple_362={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_363={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_364[2]={& Cyc__gentuple_362,& Cyc__gentuple_363};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_361={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_364),(void*)((struct _tuple6**)Cyc__genarr_364
+ 2)}};static struct _tuple7*Cyc__genarr_360[0]={};static char _tmp2E1[13]="ArrayElement";
static struct _tuple5 Cyc__gentuple_365={0,(struct _dynforward_ptr){_tmp2E1,_tmp2E1 + 
13},(void*)& Cyc__genrep_231};static char _tmp2E2[10]="FieldName";static struct
_tuple5 Cyc__gentuple_366={1,(struct _dynforward_ptr){_tmp2E2,_tmp2E2 + 10},(void*)&
Cyc__genrep_361};static struct _tuple5*Cyc__genarr_367[2]={& Cyc__gentuple_365,& Cyc__gentuple_366};
static char _tmp2E4[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _dynforward_ptr){_tmp2E4,_tmp2E4 + 11},{(void*)((struct _tuple7**)Cyc__genarr_360),(
void*)((struct _tuple7**)Cyc__genarr_360 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_367),(
void*)((struct _tuple5**)Cyc__genarr_367 + 2)}};static char _tmp2E5[5]="List";static
struct _dynforward_ptr Cyc__genname_371=(struct _dynforward_ptr){_tmp2E5,_tmp2E5 + 5};
static char _tmp2E6[3]="hd";static struct _tuple5 Cyc__gentuple_368={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2E6,_tmp2E6 + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2E7[3]="tl";static struct _tuple5 Cyc__gentuple_369={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2E7,_tmp2E7 + 3},(void*)& Cyc__genrep_359};
static struct _tuple5*Cyc__genarr_370[2]={& Cyc__gentuple_368,& Cyc__gentuple_369};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_371,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_370),(void*)((struct _tuple5**)Cyc__genarr_370 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_359={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};
struct _tuple50{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct
_tuple6 Cyc__gentuple_372={offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_359};
static struct _tuple6 Cyc__gentuple_373={offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_255};
static struct _tuple6*Cyc__genarr_374[2]={& Cyc__gentuple_372,& Cyc__gentuple_373};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_358={4,sizeof(struct _tuple50),{(
void*)((struct _tuple6**)Cyc__genarr_374),(void*)((struct _tuple6**)Cyc__genarr_374
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357={1,1,(void*)((void*)&
Cyc__genrep_358)};static char _tmp2EC[5]="List";static struct _dynforward_ptr Cyc__genname_378=(
struct _dynforward_ptr){_tmp2EC,_tmp2EC + 5};static char _tmp2ED[3]="hd";static
struct _tuple5 Cyc__gentuple_375={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp2ED,_tmp2ED + 3},(void*)& Cyc__genrep_357};static char _tmp2EE[
3]="tl";static struct _tuple5 Cyc__gentuple_376={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp2EE,_tmp2EE + 3},(void*)& Cyc__genrep_356};static struct
_tuple5*Cyc__genarr_377[2]={& Cyc__gentuple_375,& Cyc__gentuple_376};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_378,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_377),(void*)((struct _tuple5**)Cyc__genarr_377 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356={1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple51{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_439={offsetof(
struct _tuple51,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_440={
offsetof(struct _tuple51,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_441={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_336};
static struct _tuple6 Cyc__gentuple_442={offsetof(struct _tuple51,f4),(void*)& Cyc__genrep_356};
static struct _tuple6 Cyc__gentuple_443={offsetof(struct _tuple51,f5),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_444[5]={& Cyc__gentuple_439,& Cyc__gentuple_440,&
Cyc__gentuple_441,& Cyc__gentuple_442,& Cyc__gentuple_443};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_355={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_444),(
void*)((struct _tuple6**)Cyc__genarr_444 + 5)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_308;struct _tuple52{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*f4;int f5;};static struct
_tuple6 Cyc__gentuple_349={offsetof(struct _tuple52,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_350={offsetof(struct _tuple52,f2),(void*)((void*)&
Cyc__genrep_326)};static struct _tuple6 Cyc__gentuple_351={offsetof(struct _tuple52,f3),(
void*)& Cyc__genrep_309};static struct _tuple6 Cyc__gentuple_352={offsetof(struct
_tuple52,f4),(void*)& Cyc__genrep_260};static struct _tuple6 Cyc__gentuple_353={
offsetof(struct _tuple52,f5),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_354[5]={& Cyc__gentuple_349,& Cyc__gentuple_350,& Cyc__gentuple_351,&
Cyc__gentuple_352,& Cyc__gentuple_353};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_308={
4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_354),(void*)((
struct _tuple6**)Cyc__genarr_354 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_294;
struct _tuple53{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_295={
offsetof(struct _tuple53,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_296={
offsetof(struct _tuple53,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_297[2]={& Cyc__gentuple_295,& Cyc__gentuple_296};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_294={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_297),(
void*)((struct _tuple6**)Cyc__genarr_297 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_279;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_280;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_281;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_145;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep;
static char _tmp2F4[10]="Enumfield";static struct _dynforward_ptr Cyc__genname_955=(
struct _dynforward_ptr){_tmp2F4,_tmp2F4 + 10};static char _tmp2F5[5]="name";static
struct _tuple5 Cyc__gentuple_951={offsetof(struct Cyc_Absyn_Enumfield,name),(struct
_dynforward_ptr){_tmp2F5,_tmp2F5 + 5},(void*)& Cyc__genrep_11};static char _tmp2F6[4]="tag";
static struct _tuple5 Cyc__gentuple_952={offsetof(struct Cyc_Absyn_Enumfield,tag),(
struct _dynforward_ptr){_tmp2F6,_tmp2F6 + 4},(void*)& Cyc__genrep_146};static char
_tmp2F7[4]="loc";static struct _tuple5 Cyc__gentuple_953={offsetof(struct Cyc_Absyn_Enumfield,loc),(
struct _dynforward_ptr){_tmp2F7,_tmp2F7 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_954[3]={& Cyc__gentuple_951,& Cyc__gentuple_952,& Cyc__gentuple_953};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_955,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_954),(void*)((struct _tuple5**)Cyc__genarr_954 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};
static char _tmp2FA[5]="List";static struct _dynforward_ptr Cyc__genname_959=(struct
_dynforward_ptr){_tmp2FA,_tmp2FA + 5};static char _tmp2FB[3]="hd";static struct
_tuple5 Cyc__gentuple_956={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp2FB,_tmp2FB + 3},(void*)& Cyc__genrep_145};static char _tmp2FC[
3]="tl";static struct _tuple5 Cyc__gentuple_957={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp2FC,_tmp2FC + 3},(void*)& Cyc__genrep_144};static struct
_tuple5*Cyc__genarr_958[2]={& Cyc__gentuple_956,& Cyc__gentuple_957};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_959,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_958),(void*)((
struct _tuple5**)Cyc__genarr_958 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp2FF[4]="Opt";static struct _dynforward_ptr Cyc__genname_284=(struct
_dynforward_ptr){_tmp2FF,_tmp2FF + 4};static char _tmp300[2]="v";static struct
_tuple5 Cyc__gentuple_282={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp300,_tmp300 + 2},(void*)& Cyc__genrep_144};static struct _tuple5*Cyc__genarr_283[
1]={& Cyc__gentuple_282};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_284,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_283),(void*)((struct _tuple5**)Cyc__genarr_283 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_281={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp303[9]="Enumdecl";static struct _dynforward_ptr Cyc__genname_289=(
struct _dynforward_ptr){_tmp303,_tmp303 + 9};static char _tmp304[3]="sc";static
struct _tuple5 Cyc__gentuple_285={offsetof(struct Cyc_Absyn_Enumdecl,sc),(struct
_dynforward_ptr){_tmp304,_tmp304 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char
_tmp305[5]="name";static struct _tuple5 Cyc__gentuple_286={offsetof(struct Cyc_Absyn_Enumdecl,name),(
struct _dynforward_ptr){_tmp305,_tmp305 + 5},(void*)& Cyc__genrep_11};static char
_tmp306[7]="fields";static struct _tuple5 Cyc__gentuple_287={offsetof(struct Cyc_Absyn_Enumdecl,fields),(
struct _dynforward_ptr){_tmp306,_tmp306 + 7},(void*)& Cyc__genrep_281};static struct
_tuple5*Cyc__genarr_288[3]={& Cyc__gentuple_285,& Cyc__gentuple_286,& Cyc__gentuple_287};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_289,sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((
struct _tuple5**)Cyc__genarr_288),(void*)((struct _tuple5**)Cyc__genarr_288 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_280={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple54{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_290={offsetof(struct _tuple54,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_291={offsetof(struct _tuple54,f2),(
void*)& Cyc__genrep_280};static struct _tuple6 Cyc__gentuple_292={offsetof(struct
_tuple54,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_293[3]={&
Cyc__gentuple_290,& Cyc__gentuple_291,& Cyc__gentuple_292};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_279={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_293),(
void*)((struct _tuple6**)Cyc__genarr_293 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_274;struct _tuple55{unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_275={offsetof(struct _tuple55,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_276={offsetof(struct _tuple55,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_277={
offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_278[
3]={& Cyc__gentuple_275,& Cyc__gentuple_276,& Cyc__gentuple_277};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_274={4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_278),(
void*)((struct _tuple6**)Cyc__genarr_278 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_270;struct _tuple56{unsigned int f1;struct _tuple0*f2;};static struct
_tuple6 Cyc__gentuple_271={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_272={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_273[2]={& Cyc__gentuple_271,& Cyc__gentuple_272};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_270={4,sizeof(struct _tuple56),{(
void*)((struct _tuple6**)Cyc__genarr_273),(void*)((struct _tuple6**)Cyc__genarr_273
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_259;struct _tuple57{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_265={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_266={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_267={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_260};
static struct _tuple6 Cyc__gentuple_268={offsetof(struct _tuple57,f4),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_269[4]={& Cyc__gentuple_265,& Cyc__gentuple_266,&
Cyc__gentuple_267,& Cyc__gentuple_268};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_259={
4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_269),(void*)((
struct _tuple6**)Cyc__genarr_269 + 4)}};static char _tmp30D[7]="Wild_p";static struct
_tuple7 Cyc__gentuple_256={0,(struct _dynforward_ptr){_tmp30D,_tmp30D + 7}};static
char _tmp30E[7]="Null_p";static struct _tuple7 Cyc__gentuple_257={1,(struct
_dynforward_ptr){_tmp30E,_tmp30E + 7}};static struct _tuple7*Cyc__genarr_258[2]={&
Cyc__gentuple_256,& Cyc__gentuple_257};static char _tmp30F[6]="Var_p";static struct
_tuple5 Cyc__gentuple_464={0,(struct _dynforward_ptr){_tmp30F,_tmp30F + 6},(void*)&
Cyc__genrep_459};static char _tmp310[12]="Reference_p";static struct _tuple5 Cyc__gentuple_465={
1,(struct _dynforward_ptr){_tmp310,_tmp310 + 12},(void*)& Cyc__genrep_459};static
char _tmp311[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_466={2,(struct
_dynforward_ptr){_tmp311,_tmp311 + 9},(void*)& Cyc__genrep_454};static char _tmp312[
8]="Tuple_p";static struct _tuple5 Cyc__gentuple_467={3,(struct _dynforward_ptr){
_tmp312,_tmp312 + 8},(void*)& Cyc__genrep_449};static char _tmp313[10]="Pointer_p";
static struct _tuple5 Cyc__gentuple_468={4,(struct _dynforward_ptr){_tmp313,_tmp313 + 
10},(void*)& Cyc__genrep_445};static char _tmp314[7]="Aggr_p";static struct _tuple5
Cyc__gentuple_469={5,(struct _dynforward_ptr){_tmp314,_tmp314 + 7},(void*)& Cyc__genrep_355};
static char _tmp315[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_470={6,(struct
_dynforward_ptr){_tmp315,_tmp315 + 9},(void*)& Cyc__genrep_308};static char _tmp316[
6]="Int_p";static struct _tuple5 Cyc__gentuple_471={7,(struct _dynforward_ptr){
_tmp316,_tmp316 + 6},(void*)& Cyc__genrep_298};static char _tmp317[7]="Char_p";
static struct _tuple5 Cyc__gentuple_472={8,(struct _dynforward_ptr){_tmp317,_tmp317 + 
7},(void*)& Cyc__genrep_294};static char _tmp318[8]="Float_p";static struct _tuple5
Cyc__gentuple_473={9,(struct _dynforward_ptr){_tmp318,_tmp318 + 8},(void*)& Cyc__genrep_81};
static char _tmp319[7]="Enum_p";static struct _tuple5 Cyc__gentuple_474={10,(struct
_dynforward_ptr){_tmp319,_tmp319 + 7},(void*)& Cyc__genrep_279};static char _tmp31A[
11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_475={11,(struct _dynforward_ptr){
_tmp31A,_tmp31A + 11},(void*)& Cyc__genrep_274};static char _tmp31B[12]="UnknownId_p";
static struct _tuple5 Cyc__gentuple_476={12,(struct _dynforward_ptr){_tmp31B,_tmp31B
+ 12},(void*)& Cyc__genrep_270};static char _tmp31C[14]="UnknownCall_p";static
struct _tuple5 Cyc__gentuple_477={13,(struct _dynforward_ptr){_tmp31C,_tmp31C + 14},(
void*)& Cyc__genrep_259};static char _tmp31D[6]="Exp_p";static struct _tuple5 Cyc__gentuple_478={
14,(struct _dynforward_ptr){_tmp31D,_tmp31D + 6},(void*)& Cyc__genrep_231};static
struct _tuple5*Cyc__genarr_479[15]={& Cyc__gentuple_464,& Cyc__gentuple_465,& Cyc__gentuple_466,&
Cyc__gentuple_467,& Cyc__gentuple_468,& Cyc__gentuple_469,& Cyc__gentuple_470,& Cyc__gentuple_471,&
Cyc__gentuple_472,& Cyc__gentuple_473,& Cyc__gentuple_474,& Cyc__gentuple_475,& Cyc__gentuple_476,&
Cyc__gentuple_477,& Cyc__gentuple_478};static char _tmp31F[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _dynforward_ptr){_tmp31F,_tmp31F + 8},{(void*)((
struct _tuple7**)Cyc__genarr_258),(void*)((struct _tuple7**)Cyc__genarr_258 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_479),(void*)((struct _tuple5**)Cyc__genarr_479
+ 15)}};static char _tmp320[4]="Pat";static struct _dynforward_ptr Cyc__genname_484=(
struct _dynforward_ptr){_tmp320,_tmp320 + 4};static char _tmp321[2]="r";static struct
_tuple5 Cyc__gentuple_480={offsetof(struct Cyc_Absyn_Pat,r),(struct _dynforward_ptr){
_tmp321,_tmp321 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};static char _tmp322[5]="topt";
static struct _tuple5 Cyc__gentuple_481={offsetof(struct Cyc_Absyn_Pat,topt),(struct
_dynforward_ptr){_tmp322,_tmp322 + 5},(void*)& Cyc__genrep_110};static char _tmp323[
4]="loc";static struct _tuple5 Cyc__gentuple_482={offsetof(struct Cyc_Absyn_Pat,loc),(
struct _dynforward_ptr){_tmp323,_tmp323 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_483[3]={& Cyc__gentuple_480,& Cyc__gentuple_481,& Cyc__gentuple_482};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_484,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct
_tuple5**)Cyc__genarr_483),(void*)((struct _tuple5**)Cyc__genarr_483 + 3)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_255={1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_246;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_247;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;
static char _tmp326[5]="List";static struct _dynforward_ptr Cyc__genname_251=(struct
_dynforward_ptr){_tmp326,_tmp326 + 5};static char _tmp327[3]="hd";static struct
_tuple5 Cyc__gentuple_248={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp327,_tmp327 + 3},(void*)& Cyc__genrep_159};static char _tmp328[
3]="tl";static struct _tuple5 Cyc__gentuple_249={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp328,_tmp328 + 3},(void*)& Cyc__genrep_247};static struct
_tuple5*Cyc__genarr_250[2]={& Cyc__gentuple_248,& Cyc__gentuple_249};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_251,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_250),(void*)((
struct _tuple5**)Cyc__genarr_250 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_247={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp32B[4]="Opt";static struct _dynforward_ptr Cyc__genname_254=(struct
_dynforward_ptr){_tmp32B,_tmp32B + 4};static char _tmp32C[2]="v";static struct
_tuple5 Cyc__gentuple_252={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp32C,_tmp32C + 2},(void*)& Cyc__genrep_247};static struct _tuple5*Cyc__genarr_253[
1]={& Cyc__gentuple_252};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_254,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_253),(void*)((struct _tuple5**)Cyc__genarr_253 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_246={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp32F[14]="Switch_clause";static struct _dynforward_ptr Cyc__genname_491=(
struct _dynforward_ptr){_tmp32F,_tmp32F + 14};static char _tmp330[8]="pattern";
static struct _tuple5 Cyc__gentuple_485={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _dynforward_ptr){_tmp330,_tmp330 + 8},(void*)& Cyc__genrep_255};static char
_tmp331[9]="pat_vars";static struct _tuple5 Cyc__gentuple_486={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),(
struct _dynforward_ptr){_tmp331,_tmp331 + 9},(void*)& Cyc__genrep_246};static char
_tmp332[13]="where_clause";static struct _tuple5 Cyc__gentuple_487={offsetof(struct
Cyc_Absyn_Switch_clause,where_clause),(struct _dynforward_ptr){_tmp332,_tmp332 + 
13},(void*)& Cyc__genrep_146};static char _tmp333[5]="body";static struct _tuple5 Cyc__gentuple_488={
offsetof(struct Cyc_Absyn_Switch_clause,body),(struct _dynforward_ptr){_tmp333,
_tmp333 + 5},(void*)& Cyc__genrep_150};static char _tmp334[4]="loc";static struct
_tuple5 Cyc__gentuple_489={offsetof(struct Cyc_Absyn_Switch_clause,loc),(struct
_dynforward_ptr){_tmp334,_tmp334 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*
Cyc__genarr_490[5]={& Cyc__gentuple_485,& Cyc__gentuple_486,& Cyc__gentuple_487,&
Cyc__gentuple_488,& Cyc__gentuple_489};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_491,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_490),(void*)((struct _tuple5**)Cyc__genarr_490
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_245={1,1,(void*)((void*)&
Cyc_struct_Absyn_Switch_clause_rep)};static char _tmp337[5]="List";static struct
_dynforward_ptr Cyc__genname_495=(struct _dynforward_ptr){_tmp337,_tmp337 + 5};
static char _tmp338[3]="hd";static struct _tuple5 Cyc__gentuple_492={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp338,_tmp338 + 3},(void*)((void*)&
Cyc__genrep_245)};static char _tmp339[3]="tl";static struct _tuple5 Cyc__gentuple_493={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp339,_tmp339 + 3},(
void*)& Cyc__genrep_244};static struct _tuple5*Cyc__genarr_494[2]={& Cyc__gentuple_492,&
Cyc__gentuple_493};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_495,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_494),(void*)((struct _tuple5**)Cyc__genarr_494 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_244={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};
struct _tuple58{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_531={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_532={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_224};
static struct _tuple6 Cyc__gentuple_533={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_244};
static struct _tuple6*Cyc__genarr_534[3]={& Cyc__gentuple_531,& Cyc__gentuple_532,&
Cyc__gentuple_533};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_530={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_534),(void*)((
struct _tuple6**)Cyc__genarr_534 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_519;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_521;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_exp_t46H2_rep;static char _tmp33D[5]="List";static
struct _dynforward_ptr Cyc__genname_525=(struct _dynforward_ptr){_tmp33D,_tmp33D + 5};
static char _tmp33E[3]="hd";static struct _tuple5 Cyc__gentuple_522={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp33E,_tmp33E + 3},(void*)& Cyc__genrep_224};
static char _tmp33F[3]="tl";static struct _tuple5 Cyc__gentuple_523={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp33F,_tmp33F + 3},(void*)& Cyc__genrep_521};
static struct _tuple5*Cyc__genarr_524[2]={& Cyc__gentuple_522,& Cyc__gentuple_523};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_525,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_524),(void*)((struct _tuple5**)Cyc__genarr_524 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_521={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_520;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_520={1,1,(void*)((void*)& Cyc__genrep_245)};struct _tuple59{
unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Switch_clause**f3;};static
struct _tuple6 Cyc__gentuple_526={offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_527={offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_521};
static struct _tuple6 Cyc__gentuple_528={offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_520};
static struct _tuple6*Cyc__genarr_529[3]={& Cyc__gentuple_526,& Cyc__gentuple_527,&
Cyc__gentuple_528};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_519={4,
sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_529),(void*)((
struct _tuple6**)Cyc__genarr_529 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_514;
struct _tuple60{unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};
static struct _tuple6 Cyc__gentuple_515={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_516={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_517={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_518[3]={& Cyc__gentuple_515,& Cyc__gentuple_516,&
Cyc__gentuple_517};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_514={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_518),(void*)((
struct _tuple6**)Cyc__genarr_518 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_509;
static struct _tuple6 Cyc__gentuple_510={offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_513[3]={& Cyc__gentuple_510,& Cyc__gentuple_511,&
Cyc__gentuple_512};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_509={4,
sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_513),(void*)((
struct _tuple6**)Cyc__genarr_513 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_500;
struct _tuple61{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static
struct _tuple6 Cyc__gentuple_505={offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_150};
static struct _tuple6 Cyc__gentuple_507={offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_501};
static struct _tuple6*Cyc__genarr_508[3]={& Cyc__gentuple_505,& Cyc__gentuple_506,&
Cyc__gentuple_507};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_500={4,
sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_508),(void*)((
struct _tuple6**)Cyc__genarr_508 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_243;
struct _tuple62{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_496={offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_497={offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_150};
static struct _tuple6 Cyc__gentuple_498={offsetof(struct _tuple62,f3),(void*)& Cyc__genrep_244};
static struct _tuple6*Cyc__genarr_499[3]={& Cyc__gentuple_496,& Cyc__gentuple_497,&
Cyc__gentuple_498};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_243={4,
sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_499),(void*)((
struct _tuple6**)Cyc__genarr_499 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_235;
struct _tuple63{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
int f4;struct Cyc_Absyn_Exp*f5;struct Cyc_Absyn_Stmt*f6;};static struct _tuple6 Cyc__gentuple_236={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_237={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_238={
offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_239={
offsetof(struct _tuple63,f4),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_240={offsetof(struct _tuple63,f5),(void*)& Cyc__genrep_146};static
struct _tuple6 Cyc__gentuple_241={offsetof(struct _tuple63,f6),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_242[6]={& Cyc__gentuple_236,& Cyc__gentuple_237,&
Cyc__gentuple_238,& Cyc__gentuple_239,& Cyc__gentuple_240,& Cyc__gentuple_241};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_235={4,sizeof(struct _tuple63),{(
void*)((struct _tuple6**)Cyc__genarr_242),(void*)((struct _tuple6**)Cyc__genarr_242
+ 6)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_158;struct _tuple64{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Tvar*f3;struct Cyc_Absyn_Vardecl*
f4;struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_225={offsetof(
struct _tuple64,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_226={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_227={
offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_228={
offsetof(struct _tuple64,f4),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_229={
offsetof(struct _tuple64,f5),(void*)& Cyc__genrep_150};static struct _tuple6*Cyc__genarr_230[
5]={& Cyc__gentuple_225,& Cyc__gentuple_226,& Cyc__gentuple_227,& Cyc__gentuple_228,&
Cyc__gentuple_229};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_158={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_230),(void*)((
struct _tuple6**)Cyc__genarr_230 + 5)}};static char _tmp34A[7]="Skip_s";static struct
_tuple7 Cyc__gentuple_156={0,(struct _dynforward_ptr){_tmp34A,_tmp34A + 7}};static
struct _tuple7*Cyc__genarr_157[1]={& Cyc__gentuple_156};static char _tmp34B[6]="Exp_s";
static struct _tuple5 Cyc__gentuple_572={0,(struct _dynforward_ptr){_tmp34B,_tmp34B + 
6},(void*)& Cyc__genrep_231};static char _tmp34C[6]="Seq_s";static struct _tuple5 Cyc__gentuple_573={
1,(struct _dynforward_ptr){_tmp34C,_tmp34C + 6},(void*)& Cyc__genrep_567};static
char _tmp34D[9]="Return_s";static struct _tuple5 Cyc__gentuple_574={2,(struct
_dynforward_ptr){_tmp34D,_tmp34D + 9},(void*)& Cyc__genrep_563};static char _tmp34E[
13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_575={3,(struct
_dynforward_ptr){_tmp34E,_tmp34E + 13},(void*)& Cyc__genrep_557};static char _tmp34F[
8]="While_s";static struct _tuple5 Cyc__gentuple_576={4,(struct _dynforward_ptr){
_tmp34F,_tmp34F + 8},(void*)& Cyc__genrep_552};static char _tmp350[8]="Break_s";
static struct _tuple5 Cyc__gentuple_577={5,(struct _dynforward_ptr){_tmp350,_tmp350 + 
8},(void*)& Cyc__genrep_548};static char _tmp351[11]="Continue_s";static struct
_tuple5 Cyc__gentuple_578={6,(struct _dynforward_ptr){_tmp351,_tmp351 + 11},(void*)&
Cyc__genrep_548};static char _tmp352[7]="Goto_s";static struct _tuple5 Cyc__gentuple_579={
7,(struct _dynforward_ptr){_tmp352,_tmp352 + 7},(void*)& Cyc__genrep_542};static
char _tmp353[6]="For_s";static struct _tuple5 Cyc__gentuple_580={8,(struct
_dynforward_ptr){_tmp353,_tmp353 + 6},(void*)& Cyc__genrep_535};static char _tmp354[
9]="Switch_s";static struct _tuple5 Cyc__gentuple_581={9,(struct _dynforward_ptr){
_tmp354,_tmp354 + 9},(void*)& Cyc__genrep_530};static char _tmp355[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_582={10,(struct _dynforward_ptr){_tmp355,_tmp355
+ 11},(void*)& Cyc__genrep_519};static char _tmp356[7]="Decl_s";static struct _tuple5
Cyc__gentuple_583={11,(struct _dynforward_ptr){_tmp356,_tmp356 + 7},(void*)& Cyc__genrep_514};
static char _tmp357[8]="Label_s";static struct _tuple5 Cyc__gentuple_584={12,(struct
_dynforward_ptr){_tmp357,_tmp357 + 8},(void*)& Cyc__genrep_509};static char _tmp358[
5]="Do_s";static struct _tuple5 Cyc__gentuple_585={13,(struct _dynforward_ptr){
_tmp358,_tmp358 + 5},(void*)& Cyc__genrep_500};static char _tmp359[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_586={14,(struct _dynforward_ptr){_tmp359,_tmp359
+ 11},(void*)& Cyc__genrep_243};static char _tmp35A[9]="Region_s";static struct
_tuple5 Cyc__gentuple_587={15,(struct _dynforward_ptr){_tmp35A,_tmp35A + 9},(void*)&
Cyc__genrep_235};static char _tmp35B[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_588={
16,(struct _dynforward_ptr){_tmp35B,_tmp35B + 14},(void*)& Cyc__genrep_231};static
char _tmp35C[8]="Alias_s";static struct _tuple5 Cyc__gentuple_589={17,(struct
_dynforward_ptr){_tmp35C,_tmp35C + 8},(void*)& Cyc__genrep_158};static struct
_tuple5*Cyc__genarr_590[18]={& Cyc__gentuple_572,& Cyc__gentuple_573,& Cyc__gentuple_574,&
Cyc__gentuple_575,& Cyc__gentuple_576,& Cyc__gentuple_577,& Cyc__gentuple_578,& Cyc__gentuple_579,&
Cyc__gentuple_580,& Cyc__gentuple_581,& Cyc__gentuple_582,& Cyc__gentuple_583,& Cyc__gentuple_584,&
Cyc__gentuple_585,& Cyc__gentuple_586,& Cyc__gentuple_587,& Cyc__gentuple_588,& Cyc__gentuple_589};
static char _tmp35E[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,(struct _dynforward_ptr){_tmp35E,_tmp35E + 9},{(void*)((struct _tuple7**)Cyc__genarr_157),(
void*)((struct _tuple7**)Cyc__genarr_157 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_590),(
void*)((struct _tuple5**)Cyc__genarr_590 + 18)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_151;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static char _tmp35F[5]="List";static struct _dynforward_ptr Cyc__genname_155=(struct
_dynforward_ptr){_tmp35F,_tmp35F + 5};static char _tmp360[3]="hd";static struct
_tuple5 Cyc__gentuple_152={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp360,_tmp360 + 3},(void*)& Cyc__genrep_150};static char _tmp361[
3]="tl";static struct _tuple5 Cyc__gentuple_153={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp361,_tmp361 + 3},(void*)& Cyc__genrep_151};static struct
_tuple5*Cyc__genarr_154[2]={& Cyc__gentuple_152,& Cyc__gentuple_153};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_155,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_154),(void*)((
struct _tuple5**)Cyc__genarr_154 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_151={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_147[0]={};static char
_tmp365[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,(struct _dynforward_ptr){_tmp365,_tmp365 + 11},{(void*)((struct _tuple8**)Cyc__genarr_147),(
void*)((struct _tuple8**)Cyc__genarr_147 + 0)}};static char _tmp366[5]="Stmt";static
struct _dynforward_ptr Cyc__genname_597=(struct _dynforward_ptr){_tmp366,_tmp366 + 5};
static char _tmp367[2]="r";static struct _tuple5 Cyc__gentuple_591={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _dynforward_ptr){_tmp367,_tmp367 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp368[4]="loc";static struct _tuple5 Cyc__gentuple_592={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _dynforward_ptr){_tmp368,_tmp368 + 4},(void*)& Cyc__genrep_2};
static char _tmp369[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_593={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _dynforward_ptr){_tmp369,
_tmp369 + 16},(void*)& Cyc__genrep_151};static char _tmp36A[10]="try_depth";static
struct _tuple5 Cyc__gentuple_594={offsetof(struct Cyc_Absyn_Stmt,try_depth),(struct
_dynforward_ptr){_tmp36A,_tmp36A + 10},(void*)((void*)& Cyc__genrep_10)};static
char _tmp36B[6]="annot";static struct _tuple5 Cyc__gentuple_595={offsetof(struct Cyc_Absyn_Stmt,annot),(
struct _dynforward_ptr){_tmp36B,_tmp36B + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_596[5]={& Cyc__gentuple_591,& Cyc__gentuple_592,&
Cyc__gentuple_593,& Cyc__gentuple_594,& Cyc__gentuple_595};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _dynforward_ptr*)& Cyc__genname_597,sizeof(
struct Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_596),(void*)((struct
_tuple5**)Cyc__genarr_596 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_150={
1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_832;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_832={1,1,(void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)};static char _tmp36F[7]="Fndecl";static struct
_dynforward_ptr Cyc__genname_861=(struct _dynforward_ptr){_tmp36F,_tmp36F + 7};
static char _tmp370[3]="sc";static struct _tuple5 Cyc__gentuple_845={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _dynforward_ptr){_tmp370,_tmp370 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp371[10]="is_inline";static struct _tuple5 Cyc__gentuple_846={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _dynforward_ptr){_tmp371,
_tmp371 + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp372[5]="name";static
struct _tuple5 Cyc__gentuple_847={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_dynforward_ptr){_tmp372,_tmp372 + 5},(void*)& Cyc__genrep_11};static char _tmp373[4]="tvs";
static struct _tuple5 Cyc__gentuple_848={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _dynforward_ptr){_tmp373,_tmp373 + 4},(void*)& Cyc__genrep_336};static char
_tmp374[7]="effect";static struct _tuple5 Cyc__gentuple_849={offsetof(struct Cyc_Absyn_Fndecl,effect),(
struct _dynforward_ptr){_tmp374,_tmp374 + 7},(void*)& Cyc__genrep_110};static char
_tmp375[9]="ret_type";static struct _tuple5 Cyc__gentuple_850={offsetof(struct Cyc_Absyn_Fndecl,ret_type),(
struct _dynforward_ptr){_tmp375,_tmp375 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp376[5]="args";static struct _tuple5 Cyc__gentuple_851={offsetof(
struct Cyc_Absyn_Fndecl,args),(struct _dynforward_ptr){_tmp376,_tmp376 + 5},(void*)&
Cyc__genrep_834};static char _tmp377[10]="c_varargs";static struct _tuple5 Cyc__gentuple_852={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(struct _dynforward_ptr){_tmp377,
_tmp377 + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp378[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_853={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(
struct _dynforward_ptr){_tmp378,_tmp378 + 12},(void*)& Cyc__genrep_833};static char
_tmp379[7]="rgn_po";static struct _tuple5 Cyc__gentuple_854={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),(
struct _dynforward_ptr){_tmp379,_tmp379 + 7},(void*)& Cyc__genrep_397};static char
_tmp37A[5]="body";static struct _tuple5 Cyc__gentuple_855={offsetof(struct Cyc_Absyn_Fndecl,body),(
struct _dynforward_ptr){_tmp37A,_tmp37A + 5},(void*)& Cyc__genrep_150};static char
_tmp37B[11]="cached_typ";static struct _tuple5 Cyc__gentuple_856={offsetof(struct
Cyc_Absyn_Fndecl,cached_typ),(struct _dynforward_ptr){_tmp37B,_tmp37B + 11},(void*)&
Cyc__genrep_110};static char _tmp37C[15]="param_vardecls";static struct _tuple5 Cyc__gentuple_857={
offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(struct _dynforward_ptr){_tmp37C,
_tmp37C + 15},(void*)& Cyc__genrep_246};static char _tmp37D[11]="fn_vardecl";static
struct _tuple5 Cyc__gentuple_858={offsetof(struct Cyc_Absyn_Fndecl,fn_vardecl),(
struct _dynforward_ptr){_tmp37D,_tmp37D + 11},(void*)& Cyc__genrep_832};static char
_tmp37E[11]="attributes";static struct _tuple5 Cyc__gentuple_859={offsetof(struct
Cyc_Absyn_Fndecl,attributes),(struct _dynforward_ptr){_tmp37E,_tmp37E + 11},(void*)&
Cyc__genrep_62};static struct _tuple5*Cyc__genarr_860[15]={& Cyc__gentuple_845,& Cyc__gentuple_846,&
Cyc__gentuple_847,& Cyc__gentuple_848,& Cyc__gentuple_849,& Cyc__gentuple_850,& Cyc__gentuple_851,&
Cyc__gentuple_852,& Cyc__gentuple_853,& Cyc__gentuple_854,& Cyc__gentuple_855,& Cyc__gentuple_856,&
Cyc__gentuple_857,& Cyc__gentuple_858,& Cyc__gentuple_859};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_861,sizeof(
struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_860),(void*)((
struct _tuple5**)Cyc__genarr_860 + 15)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_831={
1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};struct _tuple65{unsigned int f1;
struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_862={offsetof(
struct _tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_863={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_831};static struct _tuple6*Cyc__genarr_864[
2]={& Cyc__gentuple_862,& Cyc__gentuple_863};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_830={4,sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_864),(
void*)((struct _tuple6**)Cyc__genarr_864 + 2)}};static char _tmp382[13]="Unresolved_b";
static struct _tuple7 Cyc__gentuple_824={0,(struct _dynforward_ptr){_tmp382,_tmp382 + 
13}};static struct _tuple7*Cyc__genarr_825[1]={& Cyc__gentuple_824};static char
_tmp383[9]="Global_b";static struct _tuple5 Cyc__gentuple_865={0,(struct
_dynforward_ptr){_tmp383,_tmp383 + 9},(void*)& Cyc__genrep_826};static char _tmp384[
10]="Funname_b";static struct _tuple5 Cyc__gentuple_866={1,(struct _dynforward_ptr){
_tmp384,_tmp384 + 10},(void*)& Cyc__genrep_830};static char _tmp385[8]="Param_b";
static struct _tuple5 Cyc__gentuple_867={2,(struct _dynforward_ptr){_tmp385,_tmp385 + 
8},(void*)& Cyc__genrep_826};static char _tmp386[8]="Local_b";static struct _tuple5
Cyc__gentuple_868={3,(struct _dynforward_ptr){_tmp386,_tmp386 + 8},(void*)& Cyc__genrep_826};
static char _tmp387[6]="Pat_b";static struct _tuple5 Cyc__gentuple_869={4,(struct
_dynforward_ptr){_tmp387,_tmp387 + 6},(void*)& Cyc__genrep_826};static struct
_tuple5*Cyc__genarr_870[5]={& Cyc__gentuple_865,& Cyc__gentuple_866,& Cyc__gentuple_867,&
Cyc__gentuple_868,& Cyc__gentuple_869};static char _tmp389[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,(struct _dynforward_ptr){_tmp389,_tmp389 + 8},{(void*)((
struct _tuple7**)Cyc__genarr_825),(void*)((struct _tuple7**)Cyc__genarr_825 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_870),(void*)((struct _tuple5**)Cyc__genarr_870
+ 5)}};struct _tuple66{unsigned int f1;struct _tuple0*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_871={offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_872={offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_873={offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_binding_t_rep};
static struct _tuple6*Cyc__genarr_874[3]={& Cyc__gentuple_871,& Cyc__gentuple_872,&
Cyc__gentuple_873};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_823={4,
sizeof(struct _tuple66),{(void*)((struct _tuple6**)Cyc__genarr_874),(void*)((
struct _tuple6**)Cyc__genarr_874 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_818;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;static char _tmp38B[5]="Plus";
static struct _tuple7 Cyc__gentuple_788={0,(struct _dynforward_ptr){_tmp38B,_tmp38B + 
5}};static char _tmp38C[6]="Times";static struct _tuple7 Cyc__gentuple_789={1,(struct
_dynforward_ptr){_tmp38C,_tmp38C + 6}};static char _tmp38D[6]="Minus";static struct
_tuple7 Cyc__gentuple_790={2,(struct _dynforward_ptr){_tmp38D,_tmp38D + 6}};static
char _tmp38E[4]="Div";static struct _tuple7 Cyc__gentuple_791={3,(struct
_dynforward_ptr){_tmp38E,_tmp38E + 4}};static char _tmp38F[4]="Mod";static struct
_tuple7 Cyc__gentuple_792={4,(struct _dynforward_ptr){_tmp38F,_tmp38F + 4}};static
char _tmp390[3]="Eq";static struct _tuple7 Cyc__gentuple_793={5,(struct
_dynforward_ptr){_tmp390,_tmp390 + 3}};static char _tmp391[4]="Neq";static struct
_tuple7 Cyc__gentuple_794={6,(struct _dynforward_ptr){_tmp391,_tmp391 + 4}};static
char _tmp392[3]="Gt";static struct _tuple7 Cyc__gentuple_795={7,(struct
_dynforward_ptr){_tmp392,_tmp392 + 3}};static char _tmp393[3]="Lt";static struct
_tuple7 Cyc__gentuple_796={8,(struct _dynforward_ptr){_tmp393,_tmp393 + 3}};static
char _tmp394[4]="Gte";static struct _tuple7 Cyc__gentuple_797={9,(struct
_dynforward_ptr){_tmp394,_tmp394 + 4}};static char _tmp395[4]="Lte";static struct
_tuple7 Cyc__gentuple_798={10,(struct _dynforward_ptr){_tmp395,_tmp395 + 4}};static
char _tmp396[4]="Not";static struct _tuple7 Cyc__gentuple_799={11,(struct
_dynforward_ptr){_tmp396,_tmp396 + 4}};static char _tmp397[7]="Bitnot";static struct
_tuple7 Cyc__gentuple_800={12,(struct _dynforward_ptr){_tmp397,_tmp397 + 7}};static
char _tmp398[7]="Bitand";static struct _tuple7 Cyc__gentuple_801={13,(struct
_dynforward_ptr){_tmp398,_tmp398 + 7}};static char _tmp399[6]="Bitor";static struct
_tuple7 Cyc__gentuple_802={14,(struct _dynforward_ptr){_tmp399,_tmp399 + 6}};static
char _tmp39A[7]="Bitxor";static struct _tuple7 Cyc__gentuple_803={15,(struct
_dynforward_ptr){_tmp39A,_tmp39A + 7}};static char _tmp39B[10]="Bitlshift";static
struct _tuple7 Cyc__gentuple_804={16,(struct _dynforward_ptr){_tmp39B,_tmp39B + 10}};
static char _tmp39C[11]="Bitlrshift";static struct _tuple7 Cyc__gentuple_805={17,(
struct _dynforward_ptr){_tmp39C,_tmp39C + 11}};static char _tmp39D[11]="Bitarshift";
static struct _tuple7 Cyc__gentuple_806={18,(struct _dynforward_ptr){_tmp39D,_tmp39D
+ 11}};static char _tmp39E[5]="Size";static struct _tuple7 Cyc__gentuple_807={19,(
struct _dynforward_ptr){_tmp39E,_tmp39E + 5}};static struct _tuple7*Cyc__genarr_808[
20]={& Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790,& Cyc__gentuple_791,&
Cyc__gentuple_792,& Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,& Cyc__gentuple_796,&
Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,& Cyc__gentuple_801,&
Cyc__gentuple_802,& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,& Cyc__gentuple_806,&
Cyc__gentuple_807};static struct _tuple5*Cyc__genarr_809[0]={};static char _tmp3A0[7]="Primop";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,(struct _dynforward_ptr){
_tmp3A0,_tmp3A0 + 7},{(void*)((struct _tuple7**)Cyc__genarr_808),(void*)((struct
_tuple7**)Cyc__genarr_808 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_809),(void*)((
struct _tuple5**)Cyc__genarr_809 + 0)}};struct _tuple67{unsigned int f1;void*f2;
struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_819={offsetof(struct
_tuple67,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_820={
offsetof(struct _tuple67,f2),(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple6
Cyc__gentuple_821={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_521};static
struct _tuple6*Cyc__genarr_822[3]={& Cyc__gentuple_819,& Cyc__gentuple_820,& Cyc__gentuple_821};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_818={4,sizeof(struct _tuple67),{(
void*)((struct _tuple6**)Cyc__genarr_822),(void*)((struct _tuple6**)Cyc__genarr_822
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_786;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_787;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp3A2[4]="Opt";static struct _dynforward_ptr Cyc__genname_812=(struct
_dynforward_ptr){_tmp3A2,_tmp3A2 + 4};static char _tmp3A3[2]="v";static struct
_tuple5 Cyc__gentuple_810={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp3A3,_tmp3A3 + 2},(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple5*Cyc__genarr_811[
1]={& Cyc__gentuple_810};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_812,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_811),(void*)((struct _tuple5**)Cyc__genarr_811 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_787={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_813={offsetof(struct
_tuple68,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_814={
offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_815={
offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_787};static struct _tuple6 Cyc__gentuple_816={
offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_224};static struct _tuple6*Cyc__genarr_817[
4]={& Cyc__gentuple_813,& Cyc__gentuple_814,& Cyc__gentuple_815,& Cyc__gentuple_816};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_786={4,sizeof(struct _tuple68),{(
void*)((struct _tuple6**)Cyc__genarr_817),(void*)((struct _tuple6**)Cyc__genarr_817
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_775;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_incrementor_t_rep;static char _tmp3A7[7]="PreInc";static struct _tuple7 Cyc__gentuple_776={
0,(struct _dynforward_ptr){_tmp3A7,_tmp3A7 + 7}};static char _tmp3A8[8]="PostInc";
static struct _tuple7 Cyc__gentuple_777={1,(struct _dynforward_ptr){_tmp3A8,_tmp3A8 + 
8}};static char _tmp3A9[7]="PreDec";static struct _tuple7 Cyc__gentuple_778={2,(
struct _dynforward_ptr){_tmp3A9,_tmp3A9 + 7}};static char _tmp3AA[8]="PostDec";
static struct _tuple7 Cyc__gentuple_779={3,(struct _dynforward_ptr){_tmp3AA,_tmp3AA + 
8}};static struct _tuple7*Cyc__genarr_780[4]={& Cyc__gentuple_776,& Cyc__gentuple_777,&
Cyc__gentuple_778,& Cyc__gentuple_779};static struct _tuple5*Cyc__genarr_781[0]={};
static char _tmp3AC[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _dynforward_ptr){_tmp3AC,_tmp3AC + 12},{(void*)((struct _tuple7**)Cyc__genarr_780),(
void*)((struct _tuple7**)Cyc__genarr_780 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_781),(
void*)((struct _tuple5**)Cyc__genarr_781 + 0)}};struct _tuple69{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_782={offsetof(
struct _tuple69,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_783={
offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_784={
offsetof(struct _tuple69,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_785[3]={& Cyc__gentuple_782,& Cyc__gentuple_783,& Cyc__gentuple_784};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_775={4,sizeof(struct _tuple69),{(
void*)((struct _tuple6**)Cyc__genarr_785),(void*)((struct _tuple6**)Cyc__genarr_785
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_769;struct _tuple70{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_770={offsetof(struct _tuple70,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_771={offsetof(struct _tuple70,f2),(
void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_772={offsetof(struct
_tuple70,f3),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_773={
offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_224};static struct _tuple6*Cyc__genarr_774[
4]={& Cyc__gentuple_770,& Cyc__gentuple_771,& Cyc__gentuple_772,& Cyc__gentuple_773};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_769={4,sizeof(struct _tuple70),{(
void*)((struct _tuple6**)Cyc__genarr_774),(void*)((struct _tuple6**)Cyc__genarr_774
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_620;struct _tuple71{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct
_tuple6 Cyc__gentuple_621={offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_622={offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_224};
static struct _tuple6 Cyc__gentuple_623={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_224};
static struct _tuple6*Cyc__genarr_624[3]={& Cyc__gentuple_621,& Cyc__gentuple_622,&
Cyc__gentuple_623};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_620={4,
sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_624),(void*)((
struct _tuple6**)Cyc__genarr_624 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_764;
static struct _tuple6 Cyc__gentuple_765={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_766={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_224};
static struct _tuple6 Cyc__gentuple_767={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_521};
static struct _tuple6*Cyc__genarr_768[3]={& Cyc__gentuple_765,& Cyc__gentuple_766,&
Cyc__gentuple_767};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_764={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_768),(void*)((
struct _tuple6**)Cyc__genarr_768 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_745;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_746;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_747;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_747={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
static char _tmp3B2[15]="VarargCallInfo";static struct _dynforward_ptr Cyc__genname_758=(
struct _dynforward_ptr){_tmp3B2,_tmp3B2 + 15};static char _tmp3B3[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_754={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _dynforward_ptr){_tmp3B3,_tmp3B3 + 12},(void*)((void*)& Cyc__genrep_10)};
static char _tmp3B4[10]="injectors";static struct _tuple5 Cyc__gentuple_755={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(struct _dynforward_ptr){
_tmp3B4,_tmp3B4 + 10},(void*)& Cyc__genrep_328};static char _tmp3B5[4]="vai";static
struct _tuple5 Cyc__gentuple_756={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _dynforward_ptr){_tmp3B5,_tmp3B5 + 4},(void*)& Cyc__genrep_747};static struct
_tuple5*Cyc__genarr_757[3]={& Cyc__gentuple_754,& Cyc__gentuple_755,& Cyc__gentuple_756};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_758,sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((
struct _tuple5**)Cyc__genarr_757),(void*)((struct _tuple5**)Cyc__genarr_757 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_746={1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};
struct _tuple72{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;
struct Cyc_Absyn_VarargCallInfo*f4;};static struct _tuple6 Cyc__gentuple_759={
offsetof(struct _tuple72,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_760={
offsetof(struct _tuple72,f2),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_761={
offsetof(struct _tuple72,f3),(void*)& Cyc__genrep_521};static struct _tuple6 Cyc__gentuple_762={
offsetof(struct _tuple72,f4),(void*)& Cyc__genrep_746};static struct _tuple6*Cyc__genarr_763[
4]={& Cyc__gentuple_759,& Cyc__gentuple_760,& Cyc__gentuple_761,& Cyc__gentuple_762};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_745={4,sizeof(struct _tuple72),{(
void*)((struct _tuple6**)Cyc__genarr_763),(void*)((struct _tuple6**)Cyc__genarr_763
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_740;static struct _tuple6
Cyc__gentuple_741={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_742={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_224};
static struct _tuple6 Cyc__gentuple_743={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_744[3]={& Cyc__gentuple_741,& Cyc__gentuple_742,&
Cyc__gentuple_743};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_740={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_744),(void*)((
struct _tuple6**)Cyc__genarr_744 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_727;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep;static char _tmp3BA[
17]="Unknown_coercion";static struct _tuple7 Cyc__gentuple_728={0,(struct
_dynforward_ptr){_tmp3BA,_tmp3BA + 17}};static char _tmp3BB[12]="No_coercion";
static struct _tuple7 Cyc__gentuple_729={1,(struct _dynforward_ptr){_tmp3BB,_tmp3BB + 
12}};static char _tmp3BC[16]="NonNull_to_Null";static struct _tuple7 Cyc__gentuple_730={
2,(struct _dynforward_ptr){_tmp3BC,_tmp3BC + 16}};static char _tmp3BD[15]="Other_coercion";
static struct _tuple7 Cyc__gentuple_731={3,(struct _dynforward_ptr){_tmp3BD,_tmp3BD + 
15}};static struct _tuple7*Cyc__genarr_732[4]={& Cyc__gentuple_728,& Cyc__gentuple_729,&
Cyc__gentuple_730,& Cyc__gentuple_731};static struct _tuple5*Cyc__genarr_733[0]={};
static char _tmp3BF[9]="Coercion";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={
5,(struct _dynforward_ptr){_tmp3BF,_tmp3BF + 9},{(void*)((struct _tuple7**)Cyc__genarr_732),(
void*)((struct _tuple7**)Cyc__genarr_732 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_733),(
void*)((struct _tuple5**)Cyc__genarr_733 + 0)}};struct _tuple73{unsigned int f1;void*
f2;struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_734={
offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_735={
offsetof(struct _tuple73,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_736={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_224};
static struct _tuple6 Cyc__gentuple_737={offsetof(struct _tuple73,f4),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6 Cyc__gentuple_738={offsetof(struct _tuple73,f5),(
void*)& Cyc_Absyn_coercion_t_rep};static struct _tuple6*Cyc__genarr_739[5]={& Cyc__gentuple_734,&
Cyc__gentuple_735,& Cyc__gentuple_736,& Cyc__gentuple_737,& Cyc__gentuple_738};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_727={4,sizeof(struct _tuple73),{(
void*)((struct _tuple6**)Cyc__genarr_739),(void*)((struct _tuple6**)Cyc__genarr_739
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_722;static struct _tuple6
Cyc__gentuple_723={offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_724={offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_146};
static struct _tuple6 Cyc__gentuple_725={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_224};
static struct _tuple6*Cyc__genarr_726[3]={& Cyc__gentuple_723,& Cyc__gentuple_724,&
Cyc__gentuple_725};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_722={4,
sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_726),(void*)((
struct _tuple6**)Cyc__genarr_726 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;
static struct _tuple6 Cyc__gentuple_117={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_118={offsetof(struct _tuple6,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,&
Cyc__gentuple_118};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,
sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_119),(void*)((struct
_tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_709;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_711;struct _tuple74{unsigned int f1;
unsigned int f2;};static struct _tuple6 Cyc__gentuple_712={offsetof(struct _tuple74,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_713={offsetof(struct
_tuple74,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_714[2]={&
Cyc__gentuple_712,& Cyc__gentuple_713};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_711={
4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_714),(void*)((
struct _tuple6**)Cyc__genarr_714 + 2)}};static struct _tuple7*Cyc__genarr_710[0]={};
static char _tmp3C4[12]="StructField";static struct _tuple5 Cyc__gentuple_715={0,(
struct _dynforward_ptr){_tmp3C4,_tmp3C4 + 12},(void*)& Cyc__genrep_361};static char
_tmp3C5[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_716={1,(struct
_dynforward_ptr){_tmp3C5,_tmp3C5 + 11},(void*)& Cyc__genrep_711};static struct
_tuple5*Cyc__genarr_717[2]={& Cyc__gentuple_715,& Cyc__gentuple_716};static char
_tmp3C7[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={
5,(struct _dynforward_ptr){_tmp3C7,_tmp3C7 + 14},{(void*)((struct _tuple7**)Cyc__genarr_710),(
void*)((struct _tuple7**)Cyc__genarr_710 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_717),(
void*)((struct _tuple5**)Cyc__genarr_717 + 2)}};static struct _tuple6 Cyc__gentuple_718={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_719={
offsetof(struct _tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_720={offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple6*Cyc__genarr_721[3]={& Cyc__gentuple_718,& Cyc__gentuple_719,&
Cyc__gentuple_720};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_709={4,
sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_721),(void*)((
struct _tuple6**)Cyc__genarr_721 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_704;
struct _tuple75{unsigned int f1;struct Cyc_List_List*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_705={offsetof(struct _tuple75,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_706={offsetof(struct _tuple75,f2),(void*)& Cyc__genrep_336};
static struct _tuple6 Cyc__gentuple_707={offsetof(struct _tuple75,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_708[3]={& Cyc__gentuple_705,&
Cyc__gentuple_706,& Cyc__gentuple_707};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_704={
4,sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_708),(void*)((
struct _tuple6**)Cyc__genarr_708 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_699;
struct _tuple76{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _dynforward_ptr*f3;};
static struct _tuple6 Cyc__gentuple_700={offsetof(struct _tuple76,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_701={offsetof(struct _tuple76,f2),(void*)& Cyc__genrep_224};
static struct _tuple6 Cyc__gentuple_702={offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_703[3]={& Cyc__gentuple_700,& Cyc__gentuple_701,&
Cyc__gentuple_702};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_699={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_703),(void*)((
struct _tuple6**)Cyc__genarr_703 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_695;
static struct _tuple6 Cyc__gentuple_696={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_697={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_521};
static struct _tuple6*Cyc__genarr_698[2]={& Cyc__gentuple_696,& Cyc__gentuple_697};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_695={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_698),(void*)((struct _tuple6**)Cyc__genarr_698
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_680;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_681;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_682;static struct
_tuple6 Cyc__gentuple_687={offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_683};
static struct _tuple6 Cyc__gentuple_688={offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_689={offsetof(struct _tuple1,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_690[3]={& Cyc__gentuple_687,&
Cyc__gentuple_688,& Cyc__gentuple_689};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_682={
4,sizeof(struct _tuple1),{(void*)((struct _tuple6**)Cyc__genarr_690),(void*)((
struct _tuple6**)Cyc__genarr_690 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_681={
1,1,(void*)((void*)& Cyc__genrep_682)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_602;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_603;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_604;static struct _tuple6 Cyc__gentuple_605={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_359};static struct _tuple6 Cyc__gentuple_606={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_224};static struct _tuple6*Cyc__genarr_607[2]={&
Cyc__gentuple_605,& Cyc__gentuple_606};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_604={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_607),(void*)((
struct _tuple6**)Cyc__genarr_607 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_603={
1,1,(void*)((void*)& Cyc__genrep_604)};static char _tmp3D0[5]="List";static struct
_dynforward_ptr Cyc__genname_611=(struct _dynforward_ptr){_tmp3D0,_tmp3D0 + 5};
static char _tmp3D1[3]="hd";static struct _tuple5 Cyc__gentuple_608={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp3D1,_tmp3D1 + 3},(void*)& Cyc__genrep_603};
static char _tmp3D2[3]="tl";static struct _tuple5 Cyc__gentuple_609={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp3D2,_tmp3D2 + 3},(void*)& Cyc__genrep_602};
static struct _tuple5*Cyc__genarr_610[2]={& Cyc__gentuple_608,& Cyc__gentuple_609};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_611,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_610),(void*)((struct _tuple5**)Cyc__genarr_610 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_602={1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple77{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_691={offsetof(struct _tuple77,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_692={offsetof(struct _tuple77,f2),(void*)& Cyc__genrep_681};
static struct _tuple6 Cyc__gentuple_693={offsetof(struct _tuple77,f3),(void*)& Cyc__genrep_602};
static struct _tuple6*Cyc__genarr_694[3]={& Cyc__gentuple_691,& Cyc__gentuple_692,&
Cyc__gentuple_693};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_680={4,
sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_694),(void*)((
struct _tuple6**)Cyc__genarr_694 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_676;
static struct _tuple6 Cyc__gentuple_677={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_678={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_602};
static struct _tuple6*Cyc__genarr_679[2]={& Cyc__gentuple_677,& Cyc__gentuple_678};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_676={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_679),(void*)((struct _tuple6**)Cyc__genarr_679
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_669;struct _tuple78{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_670={offsetof(struct _tuple78,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_671={offsetof(struct
_tuple78,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_672={
offsetof(struct _tuple78,f3),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_673={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_224};static struct _tuple6 Cyc__gentuple_674={
offsetof(struct _tuple78,f5),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_675[5]={& Cyc__gentuple_670,& Cyc__gentuple_671,& Cyc__gentuple_672,&
Cyc__gentuple_673,& Cyc__gentuple_674};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_669={
4,sizeof(struct _tuple78),{(void*)((struct _tuple6**)Cyc__genarr_675),(void*)((
struct _tuple6**)Cyc__genarr_675 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_661;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_662;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_662={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct
_tuple79{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_Absyn_Aggrdecl*f5;};static struct _tuple6 Cyc__gentuple_663={offsetof(
struct _tuple79,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_664={
offsetof(struct _tuple79,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_665={
offsetof(struct _tuple79,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_666={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_602};static struct _tuple6 Cyc__gentuple_667={
offsetof(struct _tuple79,f5),(void*)& Cyc__genrep_662};static struct _tuple6*Cyc__genarr_668[
5]={& Cyc__gentuple_663,& Cyc__gentuple_664,& Cyc__gentuple_665,& Cyc__gentuple_666,&
Cyc__gentuple_667};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_661={4,
sizeof(struct _tuple79),{(void*)((struct _tuple6**)Cyc__genarr_668),(void*)((
struct _tuple6**)Cyc__genarr_668 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_656;
static struct _tuple6 Cyc__gentuple_657={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_658={offsetof(struct _tuple67,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_659={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_602};static struct _tuple6*Cyc__genarr_660[3]={&
Cyc__gentuple_657,& Cyc__gentuple_658,& Cyc__gentuple_659};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_656={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_660),(
void*)((struct _tuple6**)Cyc__genarr_660 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_650;struct _tuple80{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*
f3;struct Cyc_Absyn_Tunionfield*f4;};static struct _tuple6 Cyc__gentuple_651={
offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_652={
offsetof(struct _tuple80,f2),(void*)& Cyc__genrep_521};static struct _tuple6 Cyc__gentuple_653={
offsetof(struct _tuple80,f3),(void*)((void*)& Cyc__genrep_326)};static struct
_tuple6 Cyc__gentuple_654={offsetof(struct _tuple80,f4),(void*)& Cyc__genrep_309};
static struct _tuple6*Cyc__genarr_655[4]={& Cyc__gentuple_651,& Cyc__gentuple_652,&
Cyc__gentuple_653,& Cyc__gentuple_654};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_650={
4,sizeof(struct _tuple80),{(void*)((struct _tuple6**)Cyc__genarr_655),(void*)((
struct _tuple6**)Cyc__genarr_655 + 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_643;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_644;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_644={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_637;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_637={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple81{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_645={offsetof(struct _tuple81,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_646={offsetof(struct _tuple81,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_647={offsetof(struct
_tuple81,f3),(void*)& Cyc__genrep_644};static struct _tuple6 Cyc__gentuple_648={
offsetof(struct _tuple81,f4),(void*)& Cyc__genrep_637};static struct _tuple6*Cyc__genarr_649[
4]={& Cyc__gentuple_645,& Cyc__gentuple_646,& Cyc__gentuple_647,& Cyc__gentuple_648};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_643={4,sizeof(struct _tuple81),{(
void*)((struct _tuple6**)Cyc__genarr_649),(void*)((struct _tuple6**)Cyc__genarr_649
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_636;struct _tuple82{
unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*f4;};static
struct _tuple6 Cyc__gentuple_638={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_639={offsetof(struct _tuple82,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_640={offsetof(struct _tuple82,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_641={offsetof(struct
_tuple82,f4),(void*)& Cyc__genrep_637};static struct _tuple6*Cyc__genarr_642[4]={&
Cyc__gentuple_638,& Cyc__gentuple_639,& Cyc__gentuple_640,& Cyc__gentuple_641};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_636={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_642),(void*)((struct _tuple6**)Cyc__genarr_642
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_625;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp3E1[11]="MallocInfo";static struct _dynforward_ptr Cyc__genname_632=(
struct _dynforward_ptr){_tmp3E1,_tmp3E1 + 11};static char _tmp3E2[10]="is_calloc";
static struct _tuple5 Cyc__gentuple_626={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(
struct _dynforward_ptr){_tmp3E2,_tmp3E2 + 10},(void*)((void*)& Cyc__genrep_10)};
static char _tmp3E3[4]="rgn";static struct _tuple5 Cyc__gentuple_627={offsetof(struct
Cyc_Absyn_MallocInfo,rgn),(struct _dynforward_ptr){_tmp3E3,_tmp3E3 + 4},(void*)&
Cyc__genrep_146};static char _tmp3E4[9]="elt_type";static struct _tuple5 Cyc__gentuple_628={
offsetof(struct Cyc_Absyn_MallocInfo,elt_type),(struct _dynforward_ptr){_tmp3E4,
_tmp3E4 + 9},(void*)& Cyc__genrep_130};static char _tmp3E5[9]="num_elts";static
struct _tuple5 Cyc__gentuple_629={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),(
struct _dynforward_ptr){_tmp3E5,_tmp3E5 + 9},(void*)& Cyc__genrep_224};static char
_tmp3E6[11]="fat_result";static struct _tuple5 Cyc__gentuple_630={offsetof(struct
Cyc_Absyn_MallocInfo,fat_result),(struct _dynforward_ptr){_tmp3E6,_tmp3E6 + 11},(
void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_631[5]={& Cyc__gentuple_626,&
Cyc__gentuple_627,& Cyc__gentuple_628,& Cyc__gentuple_629,& Cyc__gentuple_630};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_632,sizeof(struct Cyc_Absyn_MallocInfo),{(void*)((
struct _tuple5**)Cyc__genarr_631),(void*)((struct _tuple5**)Cyc__genarr_631 + 5)}};
struct _tuple83{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_633={offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_634={offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_635[2]={& Cyc__gentuple_633,& Cyc__gentuple_634};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_625={4,sizeof(struct _tuple83),{(
void*)((struct _tuple6**)Cyc__genarr_635),(void*)((struct _tuple6**)Cyc__genarr_635
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_601;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_612;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3E9[4]="Opt";static struct _dynforward_ptr Cyc__genname_615=(struct
_dynforward_ptr){_tmp3E9,_tmp3E9 + 4};static char _tmp3EA[2]="v";static struct
_tuple5 Cyc__gentuple_613={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp3EA,_tmp3EA + 2},(void*)& Cyc__genrep_11};static struct _tuple5*Cyc__genarr_614[
1]={& Cyc__gentuple_613};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_615,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_614),(void*)((struct _tuple5**)Cyc__genarr_614 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_612={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};
struct _tuple84{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_616={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_617={offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_612};
static struct _tuple6 Cyc__gentuple_618={offsetof(struct _tuple84,f3),(void*)& Cyc__genrep_602};
static struct _tuple6*Cyc__genarr_619[3]={& Cyc__gentuple_616,& Cyc__gentuple_617,&
Cyc__gentuple_618};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_601={4,
sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_619),(void*)((
struct _tuple6**)Cyc__genarr_619 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_149;
static struct _tuple6 Cyc__gentuple_598={offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_599={offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_150};
static struct _tuple6*Cyc__genarr_600[2]={& Cyc__gentuple_598,& Cyc__gentuple_599};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_149={4,sizeof(struct _tuple39),{(
void*)((struct _tuple6**)Cyc__genarr_600),(void*)((struct _tuple6**)Cyc__genarr_600
+ 2)}};static struct _tuple7*Cyc__genarr_148[0]={};static char _tmp3EF[8]="Const_e";
static struct _tuple5 Cyc__gentuple_905={0,(struct _dynforward_ptr){_tmp3EF,_tmp3EF + 
8},(void*)& Cyc__genrep_875};static char _tmp3F0[6]="Var_e";static struct _tuple5 Cyc__gentuple_906={
1,(struct _dynforward_ptr){_tmp3F0,_tmp3F0 + 6},(void*)& Cyc__genrep_823};static
char _tmp3F1[12]="UnknownId_e";static struct _tuple5 Cyc__gentuple_907={2,(struct
_dynforward_ptr){_tmp3F1,_tmp3F1 + 12},(void*)& Cyc__genrep_270};static char _tmp3F2[
9]="Primop_e";static struct _tuple5 Cyc__gentuple_908={3,(struct _dynforward_ptr){
_tmp3F2,_tmp3F2 + 9},(void*)& Cyc__genrep_818};static char _tmp3F3[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_909={4,(struct _dynforward_ptr){_tmp3F3,_tmp3F3 + 
11},(void*)& Cyc__genrep_786};static char _tmp3F4[12]="Increment_e";static struct
_tuple5 Cyc__gentuple_910={5,(struct _dynforward_ptr){_tmp3F4,_tmp3F4 + 12},(void*)&
Cyc__genrep_775};static char _tmp3F5[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_911={
6,(struct _dynforward_ptr){_tmp3F5,_tmp3F5 + 14},(void*)& Cyc__genrep_769};static
char _tmp3F6[6]="And_e";static struct _tuple5 Cyc__gentuple_912={7,(struct
_dynforward_ptr){_tmp3F6,_tmp3F6 + 6},(void*)& Cyc__genrep_620};static char _tmp3F7[
5]="Or_e";static struct _tuple5 Cyc__gentuple_913={8,(struct _dynforward_ptr){
_tmp3F7,_tmp3F7 + 5},(void*)& Cyc__genrep_620};static char _tmp3F8[9]="SeqExp_e";
static struct _tuple5 Cyc__gentuple_914={9,(struct _dynforward_ptr){_tmp3F8,_tmp3F8 + 
9},(void*)& Cyc__genrep_620};static char _tmp3F9[14]="UnknownCall_e";static struct
_tuple5 Cyc__gentuple_915={10,(struct _dynforward_ptr){_tmp3F9,_tmp3F9 + 14},(void*)&
Cyc__genrep_764};static char _tmp3FA[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_916={
11,(struct _dynforward_ptr){_tmp3FA,_tmp3FA + 9},(void*)& Cyc__genrep_745};static
char _tmp3FB[8]="Throw_e";static struct _tuple5 Cyc__gentuple_917={12,(struct
_dynforward_ptr){_tmp3FB,_tmp3FB + 8},(void*)& Cyc__genrep_231};static char _tmp3FC[
16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_918={13,(struct
_dynforward_ptr){_tmp3FC,_tmp3FC + 16},(void*)& Cyc__genrep_231};static char _tmp3FD[
14]="Instantiate_e";static struct _tuple5 Cyc__gentuple_919={14,(struct
_dynforward_ptr){_tmp3FD,_tmp3FD + 14},(void*)& Cyc__genrep_740};static char _tmp3FE[
7]="Cast_e";static struct _tuple5 Cyc__gentuple_920={15,(struct _dynforward_ptr){
_tmp3FE,_tmp3FE + 7},(void*)& Cyc__genrep_727};static char _tmp3FF[10]="Address_e";
static struct _tuple5 Cyc__gentuple_921={16,(struct _dynforward_ptr){_tmp3FF,_tmp3FF
+ 10},(void*)& Cyc__genrep_231};static char _tmp400[6]="New_e";static struct _tuple5
Cyc__gentuple_922={17,(struct _dynforward_ptr){_tmp400,_tmp400 + 6},(void*)& Cyc__genrep_722};
static char _tmp401[12]="Sizeoftyp_e";static struct _tuple5 Cyc__gentuple_923={18,(
struct _dynforward_ptr){_tmp401,_tmp401 + 12},(void*)& Cyc__genrep_116};static char
_tmp402[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_924={19,(struct
_dynforward_ptr){_tmp402,_tmp402 + 12},(void*)& Cyc__genrep_231};static char _tmp403[
11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_925={20,(struct _dynforward_ptr){
_tmp403,_tmp403 + 11},(void*)& Cyc__genrep_709};static char _tmp404[9]="Gentyp_e";
static struct _tuple5 Cyc__gentuple_926={21,(struct _dynforward_ptr){_tmp404,_tmp404
+ 9},(void*)& Cyc__genrep_704};static char _tmp405[8]="Deref_e";static struct _tuple5
Cyc__gentuple_927={22,(struct _dynforward_ptr){_tmp405,_tmp405 + 8},(void*)& Cyc__genrep_231};
static char _tmp406[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_928={23,(
struct _dynforward_ptr){_tmp406,_tmp406 + 13},(void*)& Cyc__genrep_699};static char
_tmp407[12]="AggrArrow_e";static struct _tuple5 Cyc__gentuple_929={24,(struct
_dynforward_ptr){_tmp407,_tmp407 + 12},(void*)& Cyc__genrep_699};static char _tmp408[
12]="Subscript_e";static struct _tuple5 Cyc__gentuple_930={25,(struct
_dynforward_ptr){_tmp408,_tmp408 + 12},(void*)& Cyc__genrep_620};static char _tmp409[
8]="Tuple_e";static struct _tuple5 Cyc__gentuple_931={26,(struct _dynforward_ptr){
_tmp409,_tmp409 + 8},(void*)& Cyc__genrep_695};static char _tmp40A[14]="CompoundLit_e";
static struct _tuple5 Cyc__gentuple_932={27,(struct _dynforward_ptr){_tmp40A,_tmp40A
+ 14},(void*)& Cyc__genrep_680};static char _tmp40B[8]="Array_e";static struct
_tuple5 Cyc__gentuple_933={28,(struct _dynforward_ptr){_tmp40B,_tmp40B + 8},(void*)&
Cyc__genrep_676};static char _tmp40C[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_934={
29,(struct _dynforward_ptr){_tmp40C,_tmp40C + 16},(void*)& Cyc__genrep_669};static
char _tmp40D[9]="Struct_e";static struct _tuple5 Cyc__gentuple_935={30,(struct
_dynforward_ptr){_tmp40D,_tmp40D + 9},(void*)& Cyc__genrep_661};static char _tmp40E[
13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_936={31,(struct
_dynforward_ptr){_tmp40E,_tmp40E + 13},(void*)& Cyc__genrep_656};static char _tmp40F[
9]="Tunion_e";static struct _tuple5 Cyc__gentuple_937={32,(struct _dynforward_ptr){
_tmp40F,_tmp40F + 9},(void*)& Cyc__genrep_650};static char _tmp410[7]="Enum_e";
static struct _tuple5 Cyc__gentuple_938={33,(struct _dynforward_ptr){_tmp410,_tmp410
+ 7},(void*)& Cyc__genrep_643};static char _tmp411[11]="AnonEnum_e";static struct
_tuple5 Cyc__gentuple_939={34,(struct _dynforward_ptr){_tmp411,_tmp411 + 11},(void*)&
Cyc__genrep_636};static char _tmp412[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_940={
35,(struct _dynforward_ptr){_tmp412,_tmp412 + 9},(void*)& Cyc__genrep_625};static
char _tmp413[7]="Swap_e";static struct _tuple5 Cyc__gentuple_941={36,(struct
_dynforward_ptr){_tmp413,_tmp413 + 7},(void*)& Cyc__genrep_620};static char _tmp414[
16]="UnresolvedMem_e";static struct _tuple5 Cyc__gentuple_942={37,(struct
_dynforward_ptr){_tmp414,_tmp414 + 16},(void*)& Cyc__genrep_601};static char _tmp415[
10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_943={38,(struct _dynforward_ptr){
_tmp415,_tmp415 + 10},(void*)& Cyc__genrep_149};static struct _tuple5*Cyc__genarr_944[
39]={& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,& Cyc__gentuple_908,&
Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911,& Cyc__gentuple_912,& Cyc__gentuple_913,&
Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917,& Cyc__gentuple_918,&
Cyc__gentuple_919,& Cyc__gentuple_920,& Cyc__gentuple_921,& Cyc__gentuple_922,& Cyc__gentuple_923,&
Cyc__gentuple_924,& Cyc__gentuple_925,& Cyc__gentuple_926,& Cyc__gentuple_927,& Cyc__gentuple_928,&
Cyc__gentuple_929,& Cyc__gentuple_930,& Cyc__gentuple_931,& Cyc__gentuple_932,& Cyc__gentuple_933,&
Cyc__gentuple_934,& Cyc__gentuple_935,& Cyc__gentuple_936,& Cyc__gentuple_937,& Cyc__gentuple_938,&
Cyc__gentuple_939,& Cyc__gentuple_940,& Cyc__gentuple_941,& Cyc__gentuple_942,& Cyc__gentuple_943};
static char _tmp417[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,(struct _dynforward_ptr){_tmp417,_tmp417 + 8},{(void*)((struct _tuple7**)Cyc__genarr_148),(
void*)((struct _tuple7**)Cyc__genarr_148 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_944),(
void*)((struct _tuple5**)Cyc__genarr_944 + 39)}};static char _tmp418[4]="Exp";static
struct _dynforward_ptr Cyc__genname_950=(struct _dynforward_ptr){_tmp418,_tmp418 + 4};
static char _tmp419[5]="topt";static struct _tuple5 Cyc__gentuple_945={offsetof(
struct Cyc_Absyn_Exp,topt),(struct _dynforward_ptr){_tmp419,_tmp419 + 5},(void*)&
Cyc__genrep_110};static char _tmp41A[2]="r";static struct _tuple5 Cyc__gentuple_946={
offsetof(struct Cyc_Absyn_Exp,r),(struct _dynforward_ptr){_tmp41A,_tmp41A + 2},(
void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp41B[4]="loc";static struct _tuple5
Cyc__gentuple_947={offsetof(struct Cyc_Absyn_Exp,loc),(struct _dynforward_ptr){
_tmp41B,_tmp41B + 4},(void*)& Cyc__genrep_2};static char _tmp41C[6]="annot";static
struct _tuple5 Cyc__gentuple_948={offsetof(struct Cyc_Absyn_Exp,annot),(struct
_dynforward_ptr){_tmp41C,_tmp41C + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static
struct _tuple5*Cyc__genarr_949[4]={& Cyc__gentuple_945,& Cyc__gentuple_946,& Cyc__gentuple_947,&
Cyc__gentuple_948};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_950,sizeof(struct Cyc_Absyn_Exp),{(void*)((
struct _tuple5**)Cyc__genarr_949),(void*)((struct _tuple5**)Cyc__genarr_949 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146={1,1,(void*)((void*)& Cyc_struct_Absyn_Exp_rep)};
static char _tmp41F[10]="ArrayInfo";static struct _dynforward_ptr Cyc__genname_1020=(
struct _dynforward_ptr){_tmp41F,_tmp41F + 10};static char _tmp420[9]="elt_type";
static struct _tuple5 Cyc__gentuple_1014={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(
struct _dynforward_ptr){_tmp420,_tmp420 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp421[3]="tq";static struct _tuple5 Cyc__gentuple_1015={offsetof(struct
Cyc_Absyn_ArrayInfo,tq),(struct _dynforward_ptr){_tmp421,_tmp421 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp422[9]="num_elts";static struct _tuple5 Cyc__gentuple_1016={offsetof(
struct Cyc_Absyn_ArrayInfo,num_elts),(struct _dynforward_ptr){_tmp422,_tmp422 + 9},(
void*)& Cyc__genrep_146};static char _tmp423[10]="zero_term";static struct _tuple5 Cyc__gentuple_1017={
offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(struct _dynforward_ptr){_tmp423,
_tmp423 + 10},(void*)& Cyc__genrep_1001};static char _tmp424[7]="zt_loc";static
struct _tuple5 Cyc__gentuple_1018={offsetof(struct Cyc_Absyn_ArrayInfo,zt_loc),(
struct _dynforward_ptr){_tmp424,_tmp424 + 7},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_1019[5]={& Cyc__gentuple_1014,& Cyc__gentuple_1015,& Cyc__gentuple_1016,&
Cyc__gentuple_1017,& Cyc__gentuple_1018};struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1020,sizeof(struct Cyc_Absyn_ArrayInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1019),(void*)((struct _tuple5**)Cyc__genarr_1019
+ 5)}};struct _tuple85{unsigned int f1;struct Cyc_Absyn_ArrayInfo f2;};static struct
_tuple6 Cyc__gentuple_1021={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1022={offsetof(struct _tuple85,f2),(void*)& Cyc_Absyn_array_info_t_rep};
static struct _tuple6*Cyc__genarr_1023[2]={& Cyc__gentuple_1021,& Cyc__gentuple_1022};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1000={4,sizeof(struct _tuple85),{(
void*)((struct _tuple6**)Cyc__genarr_1023),(void*)((struct _tuple6**)Cyc__genarr_1023
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_981;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_982;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp427[5]="List";static struct _dynforward_ptr Cyc__genname_986=(struct
_dynforward_ptr){_tmp427,_tmp427 + 5};static char _tmp428[3]="hd";static struct
_tuple5 Cyc__gentuple_983={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp428,_tmp428 + 3},(void*)& Cyc__genrep_681};static char _tmp429[
3]="tl";static struct _tuple5 Cyc__gentuple_984={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp429,_tmp429 + 3},(void*)& Cyc__genrep_982};static struct
_tuple5*Cyc__genarr_985[2]={& Cyc__gentuple_983,& Cyc__gentuple_984};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_986,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_985),(void*)((struct _tuple5**)Cyc__genarr_985 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_982={1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp42C[7]="FnInfo";static struct _dynforward_ptr Cyc__genname_996=(
struct _dynforward_ptr){_tmp42C,_tmp42C + 7};static char _tmp42D[6]="tvars";static
struct _tuple5 Cyc__gentuple_987={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_dynforward_ptr){_tmp42D,_tmp42D + 6},(void*)& Cyc__genrep_336};static char _tmp42E[
7]="effect";static struct _tuple5 Cyc__gentuple_988={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _dynforward_ptr){_tmp42E,_tmp42E + 7},(void*)& Cyc__genrep_110};static char
_tmp42F[8]="ret_typ";static struct _tuple5 Cyc__gentuple_989={offsetof(struct Cyc_Absyn_FnInfo,ret_typ),(
struct _dynforward_ptr){_tmp42F,_tmp42F + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp430[5]="args";static struct _tuple5 Cyc__gentuple_990={offsetof(
struct Cyc_Absyn_FnInfo,args),(struct _dynforward_ptr){_tmp430,_tmp430 + 5},(void*)&
Cyc__genrep_982};static char _tmp431[10]="c_varargs";static struct _tuple5 Cyc__gentuple_991={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(struct _dynforward_ptr){_tmp431,
_tmp431 + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp432[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_992={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(
struct _dynforward_ptr){_tmp432,_tmp432 + 12},(void*)& Cyc__genrep_833};static char
_tmp433[7]="rgn_po";static struct _tuple5 Cyc__gentuple_993={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(
struct _dynforward_ptr){_tmp433,_tmp433 + 7},(void*)& Cyc__genrep_397};static char
_tmp434[11]="attributes";static struct _tuple5 Cyc__gentuple_994={offsetof(struct
Cyc_Absyn_FnInfo,attributes),(struct _dynforward_ptr){_tmp434,_tmp434 + 11},(void*)&
Cyc__genrep_62};static struct _tuple5*Cyc__genarr_995[8]={& Cyc__gentuple_987,& Cyc__gentuple_988,&
Cyc__gentuple_989,& Cyc__gentuple_990,& Cyc__gentuple_991,& Cyc__gentuple_992,& Cyc__gentuple_993,&
Cyc__gentuple_994};struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_996,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((
struct _tuple5**)Cyc__genarr_995),(void*)((struct _tuple5**)Cyc__genarr_995 + 8)}};
struct _tuple86{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct _tuple6 Cyc__gentuple_997={
offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_998={
offsetof(struct _tuple86,f2),(void*)& Cyc_Absyn_fn_info_t_rep};static struct _tuple6*
Cyc__genarr_999[2]={& Cyc__gentuple_997,& Cyc__gentuple_998};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_981={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_999),(
void*)((struct _tuple6**)Cyc__genarr_999 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_977;static struct _tuple6 Cyc__gentuple_978={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_979={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_310};static struct _tuple6*Cyc__genarr_980[2]={&
Cyc__gentuple_978,& Cyc__gentuple_979};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_977={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_980),(void*)((
struct _tuple6**)Cyc__genarr_980 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_973;
struct _tuple87{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6
Cyc__gentuple_974={offsetof(struct _tuple87,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_975={offsetof(struct _tuple87,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};
static struct _tuple6*Cyc__genarr_976[2]={& Cyc__gentuple_974,& Cyc__gentuple_975};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_973={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_976),(void*)((struct _tuple6**)Cyc__genarr_976
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_968;static struct _tuple6
Cyc__gentuple_969={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_970={offsetof(struct _tuple67,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_971={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_384};
static struct _tuple6*Cyc__genarr_972[3]={& Cyc__gentuple_969,& Cyc__gentuple_970,&
Cyc__gentuple_971};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_968={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_972),(void*)((
struct _tuple6**)Cyc__genarr_972 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_963;
struct _tuple88{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};
static struct _tuple6 Cyc__gentuple_964={offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_965={offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_966={offsetof(struct _tuple88,f3),(void*)& Cyc__genrep_644};
static struct _tuple6*Cyc__genarr_967[3]={& Cyc__gentuple_964,& Cyc__gentuple_965,&
Cyc__gentuple_966};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_963={4,
sizeof(struct _tuple88),{(void*)((struct _tuple6**)Cyc__genarr_967),(void*)((
struct _tuple6**)Cyc__genarr_967 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_143;
static struct _tuple6 Cyc__gentuple_960={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_961={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_144};
static struct _tuple6*Cyc__genarr_962[2]={& Cyc__gentuple_960,& Cyc__gentuple_961};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_143={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_962),(void*)((struct _tuple6**)Cyc__genarr_962
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_138;static struct _tuple6
Cyc__gentuple_139={offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_140={offsetof(struct _tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_141={offsetof(struct _tuple28,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_142[3]={& Cyc__gentuple_139,&
Cyc__gentuple_140,& Cyc__gentuple_141};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_138={
4,sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_142),(void*)((
struct _tuple6**)Cyc__genarr_142 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep;static char _tmp43D[12]="Typedefdecl";static
struct _dynforward_ptr Cyc__genname_1168=(struct _dynforward_ptr){_tmp43D,_tmp43D + 
12};static char _tmp43E[5]="name";static struct _tuple5 Cyc__gentuple_1161={offsetof(
struct Cyc_Absyn_Typedefdecl,name),(struct _dynforward_ptr){_tmp43E,_tmp43E + 5},(
void*)& Cyc__genrep_11};static char _tmp43F[3]="tq";static struct _tuple5 Cyc__gentuple_1162={
offsetof(struct Cyc_Absyn_Typedefdecl,tq),(struct _dynforward_ptr){_tmp43F,_tmp43F
+ 3},(void*)& Cyc_Absyn_tqual_t_rep};static char _tmp440[4]="tvs";static struct
_tuple5 Cyc__gentuple_1163={offsetof(struct Cyc_Absyn_Typedefdecl,tvs),(struct
_dynforward_ptr){_tmp440,_tmp440 + 4},(void*)& Cyc__genrep_336};static char _tmp441[
5]="kind";static struct _tuple5 Cyc__gentuple_1164={offsetof(struct Cyc_Absyn_Typedefdecl,kind),(
struct _dynforward_ptr){_tmp441,_tmp441 + 5},(void*)& Cyc__genrep_1124};static char
_tmp442[5]="defn";static struct _tuple5 Cyc__gentuple_1165={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(
struct _dynforward_ptr){_tmp442,_tmp442 + 5},(void*)& Cyc__genrep_110};static char
_tmp443[5]="atts";static struct _tuple5 Cyc__gentuple_1166={offsetof(struct Cyc_Absyn_Typedefdecl,atts),(
struct _dynforward_ptr){_tmp443,_tmp443 + 5},(void*)& Cyc__genrep_62};static struct
_tuple5*Cyc__genarr_1167[6]={& Cyc__gentuple_1161,& Cyc__gentuple_1162,& Cyc__gentuple_1163,&
Cyc__gentuple_1164,& Cyc__gentuple_1165,& Cyc__gentuple_1166};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1168,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1167),(
void*)((struct _tuple5**)Cyc__genarr_1167 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_131={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple89{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*
f4;void**f5;};static struct _tuple6 Cyc__gentuple_132={offsetof(struct _tuple89,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_133={offsetof(struct
_tuple89,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_134={
offsetof(struct _tuple89,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_135={
offsetof(struct _tuple89,f4),(void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_136={
offsetof(struct _tuple89,f5),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_137[
5]={& Cyc__gentuple_132,& Cyc__gentuple_133,& Cyc__gentuple_134,& Cyc__gentuple_135,&
Cyc__gentuple_136};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_137),(void*)((
struct _tuple6**)Cyc__genarr_137 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;
static struct _tuple6 Cyc__gentuple_126={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_127={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_128[2]={& Cyc__gentuple_126,& Cyc__gentuple_127};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_120={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128
+ 2)}};static char _tmp448[9]="VoidType";static struct _tuple7 Cyc__gentuple_111={0,(
struct _dynforward_ptr){_tmp448,_tmp448 + 9}};static char _tmp449[10]="FloatType";
static struct _tuple7 Cyc__gentuple_112={1,(struct _dynforward_ptr){_tmp449,_tmp449 + 
10}};static char _tmp44A[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_113={2,(
struct _dynforward_ptr){_tmp44A,_tmp44A + 8}};static char _tmp44B[10]="UniqueRgn";
static struct _tuple7 Cyc__gentuple_114={3,(struct _dynforward_ptr){_tmp44B,_tmp44B + 
10}};static struct _tuple7*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static char _tmp44C[5]="Evar";static struct
_tuple5 Cyc__gentuple_1134={0,(struct _dynforward_ptr){_tmp44C,_tmp44C + 5},(void*)&
Cyc__genrep_1119};static char _tmp44D[8]="VarType";static struct _tuple5 Cyc__gentuple_1135={
1,(struct _dynforward_ptr){_tmp44D,_tmp44D + 8},(void*)& Cyc__genrep_1115};static
char _tmp44E[11]="TunionType";static struct _tuple5 Cyc__gentuple_1136={2,(struct
_dynforward_ptr){_tmp44E,_tmp44E + 11},(void*)& Cyc__genrep_1088};static char
_tmp44F[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1137={3,(struct
_dynforward_ptr){_tmp44F,_tmp44F + 16},(void*)& Cyc__genrep_1062};static char
_tmp450[12]="PointerType";static struct _tuple5 Cyc__gentuple_1138={4,(struct
_dynforward_ptr){_tmp450,_tmp450 + 12},(void*)& Cyc__genrep_1036};static char
_tmp451[8]="IntType";static struct _tuple5 Cyc__gentuple_1139={5,(struct
_dynforward_ptr){_tmp451,_tmp451 + 8},(void*)& Cyc__genrep_1024};static char _tmp452[
11]="DoubleType";static struct _tuple5 Cyc__gentuple_1140={6,(struct _dynforward_ptr){
_tmp452,_tmp452 + 11},(void*)& Cyc__genrep_85};static char _tmp453[10]="ArrayType";
static struct _tuple5 Cyc__gentuple_1141={7,(struct _dynforward_ptr){_tmp453,_tmp453
+ 10},(void*)& Cyc__genrep_1000};static char _tmp454[7]="FnType";static struct
_tuple5 Cyc__gentuple_1142={8,(struct _dynforward_ptr){_tmp454,_tmp454 + 7},(void*)&
Cyc__genrep_981};static char _tmp455[10]="TupleType";static struct _tuple5 Cyc__gentuple_1143={
9,(struct _dynforward_ptr){_tmp455,_tmp455 + 10},(void*)& Cyc__genrep_977};static
char _tmp456[9]="AggrType";static struct _tuple5 Cyc__gentuple_1144={10,(struct
_dynforward_ptr){_tmp456,_tmp456 + 9},(void*)& Cyc__genrep_973};static char _tmp457[
13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1145={11,(struct
_dynforward_ptr){_tmp457,_tmp457 + 13},(void*)& Cyc__genrep_968};static char _tmp458[
9]="EnumType";static struct _tuple5 Cyc__gentuple_1146={12,(struct _dynforward_ptr){
_tmp458,_tmp458 + 9},(void*)& Cyc__genrep_963};static char _tmp459[13]="AnonEnumType";
static struct _tuple5 Cyc__gentuple_1147={13,(struct _dynforward_ptr){_tmp459,
_tmp459 + 13},(void*)& Cyc__genrep_143};static char _tmp45A[11]="SizeofType";static
struct _tuple5 Cyc__gentuple_1148={14,(struct _dynforward_ptr){_tmp45A,_tmp45A + 11},(
void*)& Cyc__genrep_116};static char _tmp45B[14]="RgnHandleType";static struct
_tuple5 Cyc__gentuple_1149={15,(struct _dynforward_ptr){_tmp45B,_tmp45B + 14},(void*)&
Cyc__genrep_116};static char _tmp45C[11]="DynRgnType";static struct _tuple5 Cyc__gentuple_1150={
16,(struct _dynforward_ptr){_tmp45C,_tmp45C + 11},(void*)& Cyc__genrep_138};static
char _tmp45D[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1151={17,(struct
_dynforward_ptr){_tmp45D,_tmp45D + 12},(void*)& Cyc__genrep_129};static char _tmp45E[
8]="TagType";static struct _tuple5 Cyc__gentuple_1152={18,(struct _dynforward_ptr){
_tmp45E,_tmp45E + 8},(void*)& Cyc__genrep_116};static char _tmp45F[8]="TypeInt";
static struct _tuple5 Cyc__gentuple_1153={19,(struct _dynforward_ptr){_tmp45F,
_tmp45F + 8},(void*)& Cyc__genrep_85};static char _tmp460[10]="AccessEff";static
struct _tuple5 Cyc__gentuple_1154={20,(struct _dynforward_ptr){_tmp460,_tmp460 + 10},(
void*)& Cyc__genrep_116};static char _tmp461[8]="JoinEff";static struct _tuple5 Cyc__gentuple_1155={
21,(struct _dynforward_ptr){_tmp461,_tmp461 + 8},(void*)& Cyc__genrep_120};static
char _tmp462[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1156={22,(struct
_dynforward_ptr){_tmp462,_tmp462 + 8},(void*)& Cyc__genrep_116};static struct
_tuple5*Cyc__genarr_1157[23]={& Cyc__gentuple_1134,& Cyc__gentuple_1135,& Cyc__gentuple_1136,&
Cyc__gentuple_1137,& Cyc__gentuple_1138,& Cyc__gentuple_1139,& Cyc__gentuple_1140,&
Cyc__gentuple_1141,& Cyc__gentuple_1142,& Cyc__gentuple_1143,& Cyc__gentuple_1144,&
Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,& Cyc__gentuple_1148,&
Cyc__gentuple_1149,& Cyc__gentuple_1150,& Cyc__gentuple_1151,& Cyc__gentuple_1152,&
Cyc__gentuple_1153,& Cyc__gentuple_1154,& Cyc__gentuple_1155,& Cyc__gentuple_1156};
static char _tmp464[5]="Type";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={
5,(struct _dynforward_ptr){_tmp464,_tmp464 + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(
void*)((struct _tuple7**)Cyc__genarr_115 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_1157),(
void*)((struct _tuple5**)Cyc__genarr_1157 + 23)}};static char _tmp465[8]="Vardecl";
static struct _dynforward_ptr Cyc__genname_184=(struct _dynforward_ptr){_tmp465,
_tmp465 + 8};static char _tmp466[3]="sc";static struct _tuple5 Cyc__gentuple_175={
offsetof(struct Cyc_Absyn_Vardecl,sc),(struct _dynforward_ptr){_tmp466,_tmp466 + 3},(
void*)& Cyc_Absyn_scope_t_rep};static char _tmp467[5]="name";static struct _tuple5 Cyc__gentuple_176={
offsetof(struct Cyc_Absyn_Vardecl,name),(struct _dynforward_ptr){_tmp467,_tmp467 + 
5},(void*)& Cyc__genrep_11};static char _tmp468[3]="tq";static struct _tuple5 Cyc__gentuple_177={
offsetof(struct Cyc_Absyn_Vardecl,tq),(struct _dynforward_ptr){_tmp468,_tmp468 + 3},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp469[5]="type";static struct _tuple5 Cyc__gentuple_178={
offsetof(struct Cyc_Absyn_Vardecl,type),(struct _dynforward_ptr){_tmp469,_tmp469 + 
5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp46A[12]="initializer";
static struct _tuple5 Cyc__gentuple_179={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _dynforward_ptr){_tmp46A,_tmp46A + 12},(void*)& Cyc__genrep_146};static char
_tmp46B[4]="rgn";static struct _tuple5 Cyc__gentuple_180={offsetof(struct Cyc_Absyn_Vardecl,rgn),(
struct _dynforward_ptr){_tmp46B,_tmp46B + 4},(void*)& Cyc__genrep_110};static char
_tmp46C[11]="attributes";static struct _tuple5 Cyc__gentuple_181={offsetof(struct
Cyc_Absyn_Vardecl,attributes),(struct _dynforward_ptr){_tmp46C,_tmp46C + 11},(void*)&
Cyc__genrep_62};static char _tmp46D[8]="escapes";static struct _tuple5 Cyc__gentuple_182={
offsetof(struct Cyc_Absyn_Vardecl,escapes),(struct _dynforward_ptr){_tmp46D,
_tmp46D + 8},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_183[
8]={& Cyc__gentuple_175,& Cyc__gentuple_176,& Cyc__gentuple_177,& Cyc__gentuple_178,&
Cyc__gentuple_179,& Cyc__gentuple_180,& Cyc__gentuple_181,& Cyc__gentuple_182};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_184,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_183),(void*)((struct _tuple5**)Cyc__genarr_183 + 8)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};
struct _tuple90{unsigned int f1;struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6
Cyc__gentuple_827={offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_828={offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_829[2]={& Cyc__gentuple_827,& Cyc__gentuple_828};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_826={4,sizeof(struct _tuple90),{(
void*)((struct _tuple6**)Cyc__genarr_829),(void*)((struct _tuple6**)Cyc__genarr_829
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1188;struct _tuple91{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1189={offsetof(struct _tuple91,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1190={offsetof(struct _tuple91,f2),(
void*)& Cyc__genrep_255};static struct _tuple6 Cyc__gentuple_1191={offsetof(struct
_tuple91,f3),(void*)& Cyc__genrep_246};static struct _tuple6 Cyc__gentuple_1192={
offsetof(struct _tuple91,f4),(void*)& Cyc__genrep_224};static struct _tuple6*Cyc__genarr_1193[
4]={& Cyc__gentuple_1189,& Cyc__gentuple_1190,& Cyc__gentuple_1191,& Cyc__gentuple_1192};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1188={4,sizeof(struct _tuple91),{(
void*)((struct _tuple6**)Cyc__genarr_1193),(void*)((struct _tuple6**)Cyc__genarr_1193
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1184;static struct _tuple6
Cyc__gentuple_1185={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_1186={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_247};
static struct _tuple6*Cyc__genarr_1187[2]={& Cyc__gentuple_1185,& Cyc__gentuple_1186};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1184={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_1187),(void*)((struct _tuple6**)Cyc__genarr_1187
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1180;struct _tuple92{
unsigned int f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1181={
offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1182={
offsetof(struct _tuple92,f2),(void*)((void*)& Cyc__genrep_382)};static struct
_tuple6*Cyc__genarr_1183[2]={& Cyc__gentuple_1181,& Cyc__gentuple_1182};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1180={4,sizeof(struct _tuple92),{(void*)((
struct _tuple6**)Cyc__genarr_1183),(void*)((struct _tuple6**)Cyc__genarr_1183 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1176;struct _tuple93{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;};static struct _tuple6 Cyc__gentuple_1177={
offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1178={
offsetof(struct _tuple93,f2),(void*)((void*)& Cyc__genrep_326)};static struct
_tuple6*Cyc__genarr_1179[2]={& Cyc__gentuple_1177,& Cyc__gentuple_1178};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1176={4,sizeof(struct _tuple93),{(void*)((
struct _tuple6**)Cyc__genarr_1179),(void*)((struct _tuple6**)Cyc__genarr_1179 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1172;struct _tuple94{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1173={
offsetof(struct _tuple94,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1174={
offsetof(struct _tuple94,f2),(void*)& Cyc__genrep_280};static struct _tuple6*Cyc__genarr_1175[
2]={& Cyc__gentuple_1173,& Cyc__gentuple_1174};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1172={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_1175),(
void*)((struct _tuple6**)Cyc__genarr_1175 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61;static struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};
struct _tuple95{unsigned int f1;struct Cyc_Absyn_Typedefdecl*f2;};static struct
_tuple6 Cyc__gentuple_1169={offsetof(struct _tuple95,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1170={offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_61};
static struct _tuple6*Cyc__genarr_1171[2]={& Cyc__gentuple_1169,& Cyc__gentuple_1170};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_60={4,sizeof(struct _tuple95),{(
void*)((struct _tuple6**)Cyc__genarr_1171),(void*)((struct _tuple6**)Cyc__genarr_1171
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple96{
unsigned int f1;struct _dynforward_ptr*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_56={offsetof(struct _tuple96,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple96,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple96,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple96),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59 + 
3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_50;struct _tuple97{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_51={
offsetof(struct _tuple97,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={
offsetof(struct _tuple97,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={
offsetof(struct _tuple97,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[
3]={& Cyc__gentuple_51,& Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50={4,sizeof(struct _tuple97),{(void*)((struct _tuple6**)Cyc__genarr_54),(
void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[2]={& Cyc__gentuple_47,&
Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={4,sizeof(
struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)
Cyc__genarr_49 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_9;
struct _tuple98{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};static
struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple98,f1),(void*)& Cyc__genrep_2};
static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple98,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_36={offsetof(struct _tuple98,f3),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_37[3]={& Cyc__gentuple_34,& Cyc__gentuple_35,&
Cyc__gentuple_36};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={4,sizeof(
struct _tuple98),{(void*)((struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)
Cyc__genarr_37 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(
void*)((void*)& Cyc__genrep_9)};static char _tmp47D[5]="List";static struct
_dynforward_ptr Cyc__genname_41=(struct _dynforward_ptr){_tmp47D,_tmp47D + 5};
static char _tmp47E[3]="hd";static struct _tuple5 Cyc__gentuple_38={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp47E,_tmp47E + 3},(void*)& Cyc__genrep_8};
static char _tmp47F[3]="tl";static struct _tuple5 Cyc__gentuple_39={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp47F,_tmp47F + 3},(void*)& Cyc__genrep_7};
static struct _tuple5*Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple99{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple99,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple99,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple99,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple99),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45 + 
3)}};static char _tmp483[9]="Porton_d";static struct _tuple7 Cyc__gentuple_3={0,(
struct _dynforward_ptr){_tmp483,_tmp483 + 9}};static char _tmp484[10]="Portoff_d";
static struct _tuple7 Cyc__gentuple_4={1,(struct _dynforward_ptr){_tmp484,_tmp484 + 
10}};static struct _tuple7*Cyc__genarr_5[2]={& Cyc__gentuple_3,& Cyc__gentuple_4};
static char _tmp485[6]="Var_d";static struct _tuple5 Cyc__gentuple_1194={0,(struct
_dynforward_ptr){_tmp485,_tmp485 + 6},(void*)& Cyc__genrep_826};static char _tmp486[
5]="Fn_d";static struct _tuple5 Cyc__gentuple_1195={1,(struct _dynforward_ptr){
_tmp486,_tmp486 + 5},(void*)& Cyc__genrep_830};static char _tmp487[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1196={2,(struct _dynforward_ptr){_tmp487,_tmp487 + 6},(
void*)& Cyc__genrep_1188};static char _tmp488[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1197={
3,(struct _dynforward_ptr){_tmp488,_tmp488 + 7},(void*)& Cyc__genrep_1184};static
char _tmp489[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1198={4,(struct
_dynforward_ptr){_tmp489,_tmp489 + 7},(void*)& Cyc__genrep_1180};static char _tmp48A[
9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1199={5,(struct _dynforward_ptr){
_tmp48A,_tmp48A + 9},(void*)& Cyc__genrep_1176};static char _tmp48B[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1200={6,(struct _dynforward_ptr){_tmp48B,_tmp48B
+ 7},(void*)& Cyc__genrep_1172};static char _tmp48C[10]="Typedef_d";static struct
_tuple5 Cyc__gentuple_1201={7,(struct _dynforward_ptr){_tmp48C,_tmp48C + 10},(void*)&
Cyc__genrep_60};static char _tmp48D[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1202={
8,(struct _dynforward_ptr){_tmp48D,_tmp48D + 12},(void*)& Cyc__genrep_55};static
char _tmp48E[8]="Using_d";static struct _tuple5 Cyc__gentuple_1203={9,(struct
_dynforward_ptr){_tmp48E,_tmp48E + 8},(void*)& Cyc__genrep_50};static char _tmp48F[
10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1204={10,(struct _dynforward_ptr){
_tmp48F,_tmp48F + 10},(void*)& Cyc__genrep_46};static char _tmp490[17]="ExternCinclude_d";
static struct _tuple5 Cyc__gentuple_1205={11,(struct _dynforward_ptr){_tmp490,
_tmp490 + 17},(void*)& Cyc__genrep_6};static struct _tuple5*Cyc__genarr_1206[12]={&
Cyc__gentuple_1194,& Cyc__gentuple_1195,& Cyc__gentuple_1196,& Cyc__gentuple_1197,&
Cyc__gentuple_1198,& Cyc__gentuple_1199,& Cyc__gentuple_1200,& Cyc__gentuple_1201,&
Cyc__gentuple_1202,& Cyc__gentuple_1203,& Cyc__gentuple_1204,& Cyc__gentuple_1205};
static char _tmp492[9]="Raw_decl";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={
5,(struct _dynforward_ptr){_tmp492,_tmp492 + 9},{(void*)((struct _tuple7**)Cyc__genarr_5),(
void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_1206),(
void*)((struct _tuple5**)Cyc__genarr_1206 + 12)}};static char _tmp493[5]="Decl";
static struct _dynforward_ptr Cyc__genname_1210=(struct _dynforward_ptr){_tmp493,
_tmp493 + 5};static char _tmp494[2]="r";static struct _tuple5 Cyc__gentuple_1207={
offsetof(struct Cyc_Absyn_Decl,r),(struct _dynforward_ptr){_tmp494,_tmp494 + 2},(
void*)& Cyc_Absyn_raw_decl_t_rep};static char _tmp495[4]="loc";static struct _tuple5
Cyc__gentuple_1208={offsetof(struct Cyc_Absyn_Decl,loc),(struct _dynforward_ptr){
_tmp495,_tmp495 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1209[
2]={& Cyc__gentuple_1207,& Cyc__gentuple_1208};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1210,sizeof(struct Cyc_Absyn_Decl),{(void*)((
struct _tuple5**)Cyc__genarr_1209),(void*)((struct _tuple5**)Cyc__genarr_1209 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};
static char _tmp498[5]="List";static struct _dynforward_ptr Cyc__genname_1214=(struct
_dynforward_ptr){_tmp498,_tmp498 + 5};static char _tmp499[3]="hd";static struct
_tuple5 Cyc__gentuple_1211={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp499,_tmp499 + 3},(void*)& Cyc__genrep_1};static char _tmp49A[3]="tl";
static struct _tuple5 Cyc__gentuple_1212={offsetof(struct Cyc_List_List,tl),(struct
_dynforward_ptr){_tmp49A,_tmp49A + 3},(void*)& Cyc__genrep_0};static struct _tuple5*
Cyc__genarr_1213[2]={& Cyc__gentuple_1211,& Cyc__gentuple_1212};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1214,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_1213),(void*)((
struct _tuple5**)Cyc__genarr_1213 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};void*Cyc_decls_rep=(
void*)& Cyc__genrep_0;
