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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,
struct _dynforward_ptr src,int src_offset,int max_count);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);unsigned int Cyc_strlen(
struct _dynforward_ptr s);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;
struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct _dynforward_ptr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_No_constr_struct{int tag;};
union Cyc_Absyn_Constraint_union{struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct
Cyc_Absyn_Forward_constr_struct Forward_constr;struct Cyc_Absyn_No_constr_struct
No_constr;};struct Cyc_Absyn_Conref{union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;
};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
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
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple4{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg);struct _tuple5{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple5 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple6{unsigned int
f1;int f2;};struct _tuple6 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple7{void*f1;void*f2;};struct _tuple7*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple7*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,
int opened);int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);struct Cyc_Tcenv_CList{void*hd;
struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct
Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void*Cyc_Tcutil_compress(void*t);static int Cyc_Absyndump_expand_typedefs;static
int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static int Cyc_Absyndump_generate_line_directives;
static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_dynforward_ptr s){int sz=(int)Cyc_strlen((struct _dynforward_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dynforward_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dynforward_ptr s){int sz=(int)Cyc_strlen((struct _dynforward_ptr)s);Cyc_Absyndump_pos
+=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_dynforward_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dynforward_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);}}void Cyc_Absyndump_dump_str(
struct _dynforward_ptr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),
struct Cyc_List_List*l,struct _dynforward_ptr sep){if(l == 0)return;for(0;l->tl != 0;
l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void
Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,
struct _dynforward_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)
l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(
void(*f)(void*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep){Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep_c(f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep){if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct _tuple0*
v){struct Cyc_List_List*_tmp1=0;struct _dynforward_ptr**prefix=0;{union Cyc_Absyn_Nmspace_union
_tmp2=(*v).f1;struct Cyc_List_List*_tmp3;struct Cyc_List_List*_tmp4;_LL11: if((
_tmp2.Loc_n).tag != 0)goto _LL13;_LL12: _tmp3=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag
!= 1)goto _LL15;_tmp3=(_tmp2.Rel_n).f1;_LL14: _tmp1=_tmp3;goto _LL10;_LL15: if((
_tmp2.Abs_n).tag != 2)goto _LL10;_tmp4=(_tmp2.Abs_n).f1;_LL16: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dynforward_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp4;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({
const char*_tmp5="::";_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,
3));}));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((struct _dynforward_ptr*)
_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({const char*
_tmp6="::";_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,3));}));}
Cyc_Absyndump_dump_nospace(*((struct _dynforward_ptr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_nospace(({const char*_tmp7="_";_tag_dynforward(_tmp7,sizeof(
char),_get_zero_arr_size(_tmp7,2));}));else{Cyc_Absyndump_dump_nospace(({const
char*_tmp8="::";_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,3));}));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*
v).f2);else{Cyc_Absyndump_dump_str((*v).f2);}}}void Cyc_Absyndump_dumptq(struct
Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp9="restrict";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,9));}));if(tq.q_volatile)
Cyc_Absyndump_dump(({const char*_tmpA="volatile";_tag_dynforward(_tmpA,sizeof(
char),_get_zero_arr_size(_tmpA,9));}));if(tq.print_const)Cyc_Absyndump_dump(({
const char*_tmpB="const";_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size(
_tmpB,6));}));}void Cyc_Absyndump_dumpscope(void*sc){void*_tmpC=sc;_LL18: if((int)
_tmpC != 0)goto _LL1A;_LL19: Cyc_Absyndump_dump(({const char*_tmpD="static";
_tag_dynforward(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,7));}));return;_LL1A:
if((int)_tmpC != 2)goto _LL1C;_LL1B: return;_LL1C: if((int)_tmpC != 3)goto _LL1E;_LL1D:
Cyc_Absyndump_dump(({const char*_tmpE="extern";_tag_dynforward(_tmpE,sizeof(char),
_get_zero_arr_size(_tmpE,7));}));return;_LL1E: if((int)_tmpC != 4)goto _LL20;_LL1F:
Cyc_Absyndump_dump(({const char*_tmpF="extern \"C\"";_tag_dynforward(_tmpF,
sizeof(char),_get_zero_arr_size(_tmpF,11));}));return;_LL20: if((int)_tmpC != 1)
goto _LL22;_LL21: Cyc_Absyndump_dump(({const char*_tmp10="abstract";_tag_dynforward(
_tmp10,sizeof(char),_get_zero_arr_size(_tmp10,9));}));return;_LL22: if((int)_tmpC
!= 5)goto _LL17;_LL23: Cyc_Absyndump_dump(({const char*_tmp11="register";
_tag_dynforward(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,9));}));return;
_LL17:;}void Cyc_Absyndump_dumpkind(void*k){void*_tmp12=k;_LL25: if((int)_tmp12 != 
0)goto _LL27;_LL26: Cyc_Absyndump_dump(({const char*_tmp13="A";_tag_dynforward(
_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,2));}));return;_LL27: if((int)
_tmp12 != 1)goto _LL29;_LL28: Cyc_Absyndump_dump(({const char*_tmp14="M";
_tag_dynforward(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,2));}));return;
_LL29: if((int)_tmp12 != 2)goto _LL2B;_LL2A: Cyc_Absyndump_dump(({const char*_tmp15="B";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,2));}));return;
_LL2B: if((int)_tmp12 != 3)goto _LL2D;_LL2C: Cyc_Absyndump_dump(({const char*_tmp16="R";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,2));}));return;
_LL2D: if((int)_tmp12 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(({const char*_tmp17="E";
_tag_dynforward(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,2));}));return;
_LL2F: if((int)_tmp12 != 5)goto _LL24;_LL30: Cyc_Absyndump_dump(({const char*_tmp18="I";
_tag_dynforward(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,2));}));return;
_LL24:;}void Cyc_Absyndump_dumpaggr_kind(void*k){void*_tmp19=k;_LL32: if((int)
_tmp19 != 0)goto _LL34;_LL33: Cyc_Absyndump_dump(({const char*_tmp1A="struct";
_tag_dynforward(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,7));}));return;
_LL34: if((int)_tmp19 != 1)goto _LL31;_LL35: Cyc_Absyndump_dump(({const char*_tmp1B="union";
_tag_dynforward(_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,6));}));return;
_LL31:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumptyp,ts,({const char*_tmp1C="<";_tag_dynforward(_tmp1C,sizeof(
char),_get_zero_arr_size(_tmp1C,2));}),({const char*_tmp1D=">";_tag_dynforward(
_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,2));}),({const char*_tmp1E=",";
_tag_dynforward(_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,2));}));}void Cyc_Absyndump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp1F=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp20;void*_tmp21;_LL37: if(*((int*)_tmp1F)!= 0)goto _LL39;
_tmp20=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp1F)->f1;if((int)_tmp20 != 2)
goto _LL39;_LL38: goto _LL3A;_LL39: if(*((int*)_tmp1F)!= 2)goto _LL3B;_LL3A: goto _LL3C;
_LL3B: if(*((int*)_tmp1F)!= 1)goto _LL3D;_LL3C: Cyc_Absyndump_dump(({const char*
_tmp22="::?";_tag_dynforward(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,4));}));
goto _LL36;_LL3D: if(*((int*)_tmp1F)!= 0)goto _LL36;_tmp21=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp1F)->f1;_LL3E: Cyc_Absyndump_dump(({const char*_tmp23="::";_tag_dynforward(
_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,3));}));Cyc_Absyndump_dumpkind(
_tmp21);goto _LL36;_LL36:;}}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,({const char*_tmp24="<";_tag_dynforward(_tmp24,sizeof(
char),_get_zero_arr_size(_tmp24,2));}),({const char*_tmp25=">";_tag_dynforward(
_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,2));}),({const char*_tmp26=",";
_tag_dynforward(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,2));}));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp27="<";
_tag_dynforward(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,2));}),({const char*
_tmp28=">";_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,2));}),({
const char*_tmp29=",";_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(
_tmp29,2));}));}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple8*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,({const char*_tmp2A="(";
_tag_dynforward(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,2));}),({const char*
_tmp2B=")";_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,2));}),({
const char*_tmp2C=",";_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size(
_tmp2C,2));}));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){for(0;
atts != 0;atts=atts->tl){void*_tmp2D=(void*)atts->hd;_LL40: if((int)_tmp2D != 0)
goto _LL42;_LL41: Cyc_Absyndump_dump(({const char*_tmp2E="_stdcall";_tag_dynforward(
_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,9));}));return;_LL42: if((int)
_tmp2D != 1)goto _LL44;_LL43: Cyc_Absyndump_dump(({const char*_tmp2F="_cdecl";
_tag_dynforward(_tmp2F,sizeof(char),_get_zero_arr_size(_tmp2F,7));}));return;
_LL44: if((int)_tmp2D != 2)goto _LL46;_LL45: Cyc_Absyndump_dump(({const char*_tmp30="_fastcall";
_tag_dynforward(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,10));}));return;
_LL46:;_LL47: goto _LL3F;_LL3F:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp31=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL49: if((int)
_tmp31 != 0)goto _LL4B;_LL4A: goto _LL4C;_LL4B: if((int)_tmp31 != 1)goto _LL4D;_LL4C:
goto _LL4E;_LL4D: if((int)_tmp31 != 2)goto _LL4F;_LL4E: goto _LL48;_LL4F:;_LL50: hasatt=
1;goto _LL48;_LL48:;}}if(!hasatt)return;Cyc_Absyndump_dump(({const char*_tmp32="__declspec(";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,12));}));for(0;
atts != 0;atts=atts->tl){void*_tmp33=(void*)atts->hd;_LL52: if((int)_tmp33 != 0)
goto _LL54;_LL53: goto _LL55;_LL54: if((int)_tmp33 != 1)goto _LL56;_LL55: goto _LL57;
_LL56: if((int)_tmp33 != 2)goto _LL58;_LL57: goto _LL51;_LL58:;_LL59: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL51;_LL51:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){if(atts == 0)
return;if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(atts);return;}Cyc_Absyndump_dump(({
const char*_tmp34=" __attribute__((";_tag_dynforward(_tmp34,sizeof(char),
_get_zero_arr_size(_tmp34,17));}));for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(({
const char*_tmp35=",";_tag_dynforward(_tmp35,sizeof(char),_get_zero_arr_size(
_tmp35,2));}));}Cyc_Absyndump_dump(({const char*_tmp36=")) ";_tag_dynforward(
_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,4));}));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp37=(void*)tms->hd;_LL5B:
if(*((int*)_tmp37)!= 2)goto _LL5D;_LL5C: return 1;_LL5D:;_LL5E: return 0;_LL5A:;}}
static void Cyc_Absyndump_dumprgn(void*t){void*_tmp38=Cyc_Tcutil_compress(t);_LL60:
if((int)_tmp38 != 2)goto _LL62;_LL61: Cyc_Absyndump_dump(({const char*_tmp39="`H";
_tag_dynforward(_tmp39,sizeof(char),_get_zero_arr_size(_tmp39,3));}));goto _LL5F;
_LL62:;_LL63: Cyc_Absyndump_dumpntyp(t);goto _LL5F;_LL5F:;}struct _tuple9{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};static struct _tuple9 Cyc_Absyndump_effects_split(void*
t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp3A=Cyc_Tcutil_compress(
t);void*_tmp3B;struct Cyc_List_List*_tmp3C;_LL65: if(_tmp3A <= (void*)4)goto _LL69;
if(*((int*)_tmp3A)!= 20)goto _LL67;_tmp3B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp3A)->f1;_LL66: rgions=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));
_tmp3D->hd=(void*)_tmp3B;_tmp3D->tl=rgions;_tmp3D;});goto _LL64;_LL67: if(*((int*)
_tmp3A)!= 21)goto _LL69;_tmp3C=((struct Cyc_Absyn_JoinEff_struct*)_tmp3A)->f1;
_LL68: for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){struct Cyc_List_List*_tmp3F;struct Cyc_List_List*
_tmp40;struct _tuple9 _tmp3E=Cyc_Absyndump_effects_split((void*)_tmp3C->hd);_tmp3F=
_tmp3E.f1;_tmp40=_tmp3E.f2;rgions=Cyc_List_imp_append(_tmp3F,rgions);effects=Cyc_List_imp_append(
_tmp40,effects);}goto _LL64;_LL69:;_LL6A: effects=({struct Cyc_List_List*_tmp41=
_cycalloc(sizeof(*_tmp41));_tmp41->hd=(void*)t;_tmp41->tl=effects;_tmp41;});goto
_LL64;_LL64:;}return({struct _tuple9 _tmp42;_tmp42.f1=rgions;_tmp42.f2=effects;
_tmp42;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp44;
struct Cyc_List_List*_tmp45;struct _tuple9 _tmp43=Cyc_Absyndump_effects_split(t);
_tmp44=_tmp43.f1;_tmp45=_tmp43.f2;_tmp44=Cyc_List_imp_rev(_tmp44);_tmp45=Cyc_List_imp_rev(
_tmp45);for(0;_tmp45 != 0;_tmp45=_tmp45->tl){Cyc_Absyndump_dumpntyp((void*)_tmp45->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp44 != 0;_tmp44=_tmp44->tl){Cyc_Absyndump_dumprgn((void*)_tmp44->hd);if(_tmp44->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp46=t;struct Cyc_Absyn_Tvar*_tmp47;struct
Cyc_Core_Opt*_tmp48;struct Cyc_Core_Opt*_tmp49;int _tmp4A;struct Cyc_Core_Opt*
_tmp4B;struct Cyc_Core_Opt*_tmp4C;struct Cyc_Core_Opt _tmp4D;void*_tmp4E;int _tmp4F;
struct Cyc_Absyn_TunionInfo _tmp50;union Cyc_Absyn_TunionInfoU_union _tmp51;struct
Cyc_List_List*_tmp52;struct Cyc_Core_Opt*_tmp53;struct Cyc_Absyn_TunionFieldInfo
_tmp54;union Cyc_Absyn_TunionFieldInfoU_union _tmp55;struct Cyc_List_List*_tmp56;
void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;void*
_tmp5D;void*_tmp5E;void*_tmp5F;void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;
void*_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;void*_tmp68;void*_tmp69;void*
_tmp6A;void*_tmp6B;void*_tmp6C;void*_tmp6D;void*_tmp6E;void*_tmp6F;void*_tmp70;
void*_tmp71;void*_tmp72;void*_tmp73;void*_tmp74;void*_tmp75;void*_tmp76;void*
_tmp77;void*_tmp78;void*_tmp79;void*_tmp7A;int _tmp7B;struct Cyc_List_List*_tmp7C;
struct Cyc_Absyn_AggrInfo _tmp7D;union Cyc_Absyn_AggrInfoU_union _tmp7E;struct Cyc_List_List*
_tmp7F;void*_tmp80;struct Cyc_List_List*_tmp81;struct _tuple0*_tmp82;struct Cyc_List_List*
_tmp83;struct _tuple0*_tmp84;struct Cyc_List_List*_tmp85;void*_tmp86;void*_tmp87;
void*_tmp88;void*_tmp89;int _tmp8A;void*_tmp8B;_LL6C: if(_tmp46 <= (void*)4)goto
_LL72;if(*((int*)_tmp46)!= 7)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(*((int*)_tmp46)
!= 8)goto _LL70;_LL6F: goto _LL71;_LL70: if(*((int*)_tmp46)!= 4)goto _LL72;_LL71:
return;_LL72: if((int)_tmp46 != 0)goto _LL74;_LL73: Cyc_Absyndump_dump(({const char*
_tmp8C="void";_tag_dynforward(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,5));}));
return;_LL74: if(_tmp46 <= (void*)4)goto _LLA2;if(*((int*)_tmp46)!= 1)goto _LL76;
_tmp47=((struct Cyc_Absyn_VarType_struct*)_tmp46)->f1;_LL75: Cyc_Absyndump_dump_str(
_tmp47->name);return;_LL76: if(*((int*)_tmp46)!= 0)goto _LL78;_tmp48=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f1;_tmp49=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f2;if(_tmp49 != 0)goto
_LL78;_tmp4A=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f3;_LL77: Cyc_Absyndump_dump(({
const char*_tmp8D="%";_tag_dynforward(_tmp8D,sizeof(char),_get_zero_arr_size(
_tmp8D,2));}));if(_tmp48 == 0)Cyc_Absyndump_dump(({const char*_tmp8E="?";
_tag_dynforward(_tmp8E,sizeof(char),_get_zero_arr_size(_tmp8E,2));}));else{Cyc_Absyndump_dumpkind((
void*)_tmp48->v);}Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp91;_tmp91.tag=1;_tmp91.f1=(unsigned long)_tmp4A;{void*_tmp8F[1]={& _tmp91};
Cyc_aprintf(({const char*_tmp90="(%d)";_tag_dynforward(_tmp90,sizeof(char),
_get_zero_arr_size(_tmp90,5));}),_tag_dynforward(_tmp8F,sizeof(void*),1));}}));
return;_LL78: if(*((int*)_tmp46)!= 0)goto _LL7A;_tmp4B=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f1;_tmp4C=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f2;if(_tmp4C == 0)goto
_LL7A;_tmp4D=*_tmp4C;_tmp4E=(void*)_tmp4D.v;_tmp4F=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f3;_LL79: Cyc_Absyndump_dumpntyp(_tmp4E);return;_LL7A: if(*((int*)_tmp46)
!= 2)goto _LL7C;_tmp50=((struct Cyc_Absyn_TunionType_struct*)_tmp46)->f1;_tmp51=
_tmp50.tunion_info;_tmp52=_tmp50.targs;_tmp53=_tmp50.rgn;_LL7B:{union Cyc_Absyn_TunionInfoU_union
_tmp92=_tmp51;struct Cyc_Absyn_UnknownTunionInfo _tmp93;struct _tuple0*_tmp94;int
_tmp95;struct Cyc_Absyn_Tuniondecl**_tmp96;struct Cyc_Absyn_Tuniondecl*_tmp97;
struct Cyc_Absyn_Tuniondecl _tmp98;struct _tuple0*_tmp99;int _tmp9A;_LLC7: if((_tmp92.UnknownTunion).tag
!= 0)goto _LLC9;_tmp93=(_tmp92.UnknownTunion).f1;_tmp94=_tmp93.name;_tmp95=_tmp93.is_xtunion;
_LLC8: _tmp99=_tmp94;_tmp9A=_tmp95;goto _LLCA;_LLC9: if((_tmp92.KnownTunion).tag != 
1)goto _LLC6;_tmp96=(_tmp92.KnownTunion).f1;_tmp97=*_tmp96;_tmp98=*_tmp97;_tmp99=
_tmp98.name;_tmp9A=_tmp98.is_xtunion;_LLCA: if(_tmp9A)Cyc_Absyndump_dump(({const
char*_tmp9B="xtunion ";_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size(
_tmp9B,9));}));else{Cyc_Absyndump_dump(({const char*_tmp9C="tunion ";
_tag_dynforward(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,8));}));}{void*r=(
unsigned int)_tmp53?(void*)_tmp53->v:(void*)2;{void*_tmp9D=Cyc_Tcutil_compress(r);
_LLCC: if((int)_tmp9D != 2)goto _LLCE;_LLCD: goto _LLCB;_LLCE:;_LLCF: Cyc_Absyndump_dumptyp(
r);Cyc_Absyndump_dump(({const char*_tmp9E=" ";_tag_dynforward(_tmp9E,sizeof(char),
_get_zero_arr_size(_tmp9E,2));}));goto _LLCB;_LLCB:;}Cyc_Absyndump_dumpqvar(
_tmp99);Cyc_Absyndump_dumptps(_tmp52);goto _LLC6;}_LLC6:;}goto _LL6B;_LL7C: if(*((
int*)_tmp46)!= 3)goto _LL7E;_tmp54=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp46)->f1;_tmp55=_tmp54.field_info;_tmp56=_tmp54.targs;_LL7D:{union Cyc_Absyn_TunionFieldInfoU_union
_tmp9F=_tmp55;struct Cyc_Absyn_UnknownTunionFieldInfo _tmpA0;struct _tuple0*_tmpA1;
struct _tuple0*_tmpA2;int _tmpA3;struct Cyc_Absyn_Tuniondecl*_tmpA4;struct Cyc_Absyn_Tuniondecl
_tmpA5;struct _tuple0*_tmpA6;int _tmpA7;struct Cyc_Absyn_Tunionfield*_tmpA8;struct
Cyc_Absyn_Tunionfield _tmpA9;struct _tuple0*_tmpAA;_LLD1: if((_tmp9F.UnknownTunionfield).tag
!= 0)goto _LLD3;_tmpA0=(_tmp9F.UnknownTunionfield).f1;_tmpA1=_tmpA0.tunion_name;
_tmpA2=_tmpA0.field_name;_tmpA3=_tmpA0.is_xtunion;_LLD2: _tmpA6=_tmpA1;_tmpA7=
_tmpA3;_tmpAA=_tmpA2;goto _LLD4;_LLD3: if((_tmp9F.KnownTunionfield).tag != 1)goto
_LLD0;_tmpA4=(_tmp9F.KnownTunionfield).f1;_tmpA5=*_tmpA4;_tmpA6=_tmpA5.name;
_tmpA7=_tmpA5.is_xtunion;_tmpA8=(_tmp9F.KnownTunionfield).f2;_tmpA9=*_tmpA8;
_tmpAA=_tmpA9.name;_LLD4: if(_tmpA7)Cyc_Absyndump_dump(({const char*_tmpAB="xtunion ";
_tag_dynforward(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,9));}));else{Cyc_Absyndump_dump(({
const char*_tmpAC="tunion ";_tag_dynforward(_tmpAC,sizeof(char),
_get_zero_arr_size(_tmpAC,8));}));}Cyc_Absyndump_dumpqvar(_tmpA6);Cyc_Absyndump_dump(({
const char*_tmpAD=".";_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(
_tmpAD,2));}));Cyc_Absyndump_dumpqvar(_tmpAA);Cyc_Absyndump_dumptps(_tmp56);goto
_LLD0;_LLD0:;}goto _LL6B;_LL7E: if(*((int*)_tmp46)!= 5)goto _LL80;_tmp57=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp57 != 2)goto _LL80;_tmp58=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp58 != 2)goto _LL80;
_LL7F: goto _LL81;_LL80: if(*((int*)_tmp46)!= 5)goto _LL82;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp59 != 0)goto _LL82;_tmp5A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp5A != 2)goto _LL82;_LL81: Cyc_Absyndump_dump(({const char*
_tmpAE="int";_tag_dynforward(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,4));}));
return;_LL82: if(*((int*)_tmp46)!= 5)goto _LL84;_tmp5B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp5B != 2)goto _LL84;_tmp5C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp5C != 3)goto _LL84;_LL83: goto _LL85;_LL84: if(*((int*)_tmp46)
!= 5)goto _LL86;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((
int)_tmp5D != 0)goto _LL86;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;
if((int)_tmp5E != 3)goto _LL86;_LL85: Cyc_Absyndump_dump(({const char*_tmpAF="long";
_tag_dynforward(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,5));}));return;
_LL86: if(*((int*)_tmp46)!= 5)goto _LL88;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp5F != 2)goto _LL88;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp60 != 0)goto _LL88;_LL87: Cyc_Absyndump_dump(({const char*
_tmpB0="char";_tag_dynforward(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,5));}));
return;_LL88: if(*((int*)_tmp46)!= 5)goto _LL8A;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp61 != 0)goto _LL8A;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp62 != 0)goto _LL8A;_LL89: Cyc_Absyndump_dump(({const char*
_tmpB1="signed char";_tag_dynforward(_tmpB1,sizeof(char),_get_zero_arr_size(
_tmpB1,12));}));return;_LL8A: if(*((int*)_tmp46)!= 5)goto _LL8C;_tmp63=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp63 != 1)goto _LL8C;_tmp64=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp64 != 0)goto _LL8C;
_LL8B: Cyc_Absyndump_dump(({const char*_tmpB2="unsigned char";_tag_dynforward(
_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,14));}));return;_LL8C: if(*((int*)
_tmp46)!= 5)goto _LL8E;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp65 != 2)goto _LL8E;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp66 != 1)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if(*((int*)_tmp46)
!= 5)goto _LL90;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((
int)_tmp67 != 0)goto _LL90;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;
if((int)_tmp68 != 1)goto _LL90;_LL8F: Cyc_Absyndump_dump(({const char*_tmpB3="short";
_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,6));}));return;
_LL90: if(*((int*)_tmp46)!= 5)goto _LL92;_tmp69=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp69 != 1)goto _LL92;_tmp6A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp6A != 1)goto _LL92;_LL91: Cyc_Absyndump_dump(({const char*
_tmpB4="unsigned short";_tag_dynforward(_tmpB4,sizeof(char),_get_zero_arr_size(
_tmpB4,15));}));return;_LL92: if(*((int*)_tmp46)!= 5)goto _LL94;_tmp6B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp6B != 1)goto _LL94;_tmp6C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp6C != 2)goto _LL94;
_LL93: Cyc_Absyndump_dump(({const char*_tmpB5="unsigned int";_tag_dynforward(
_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,13));}));return;_LL94: if(*((int*)
_tmp46)!= 5)goto _LL96;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;
if((int)_tmp6D != 1)goto _LL96;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp6E != 3)goto _LL96;_LL95: Cyc_Absyndump_dump(({const char*
_tmpB6="unsigned long";_tag_dynforward(_tmpB6,sizeof(char),_get_zero_arr_size(
_tmpB6,14));}));return;_LL96: if(*((int*)_tmp46)!= 5)goto _LL98;_tmp6F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp6F != 2)goto _LL98;_tmp70=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)_tmp70 != 4)goto _LL98;
if(!Cyc_Absyndump_to_VC)goto _LL98;_LL97: goto _LL99;_LL98: if(*((int*)_tmp46)!= 5)
goto _LL9A;_tmp71=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)
_tmp71 != 0)goto _LL9A;_tmp72=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;
if((int)_tmp72 != 4)goto _LL9A;if(!Cyc_Absyndump_to_VC)goto _LL9A;_LL99: Cyc_Absyndump_dump(({
const char*_tmpB7="__int64";_tag_dynforward(_tmpB7,sizeof(char),
_get_zero_arr_size(_tmpB7,8));}));return;_LL9A: if(*((int*)_tmp46)!= 5)goto _LL9C;
_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp73 != 2)
goto _LL9C;_tmp74=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)
_tmp74 != 4)goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp46)!= 5)goto _LL9E;
_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((int)_tmp75 != 0)
goto _LL9E;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;if((int)
_tmp76 != 4)goto _LL9E;_LL9D: Cyc_Absyndump_dump(({const char*_tmpB8="long long";
_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,10));}));return;
_LL9E: if(*((int*)_tmp46)!= 5)goto _LLA0;_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f1;if((int)_tmp77 != 1)goto _LLA0;_tmp78=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46)->f2;if((int)_tmp78 != 4)goto _LLA0;if(!Cyc_Absyndump_to_VC)goto _LLA0;_LL9F:
Cyc_Absyndump_dump(({const char*_tmpB9="unsigned __int64";_tag_dynforward(_tmpB9,
sizeof(char),_get_zero_arr_size(_tmpB9,17));}));return;_LLA0: if(*((int*)_tmp46)
!= 5)goto _LLA2;_tmp79=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;if((
int)_tmp79 != 1)goto _LLA2;_tmp7A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;
if((int)_tmp7A != 4)goto _LLA2;_LLA1: Cyc_Absyndump_dump(({const char*_tmpBA="unsigned long long";
_tag_dynforward(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,19));}));return;
_LLA2: if((int)_tmp46 != 1)goto _LLA4;_LLA3: Cyc_Absyndump_dump(({const char*_tmpBB="float";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,6));}));return;
_LLA4: if(_tmp46 <= (void*)4)goto _LLBC;if(*((int*)_tmp46)!= 6)goto _LLA6;_tmp7B=((
struct Cyc_Absyn_DoubleType_struct*)_tmp46)->f1;_LLA5: if(_tmp7B)Cyc_Absyndump_dump(({
const char*_tmpBC="long double";_tag_dynforward(_tmpBC,sizeof(char),
_get_zero_arr_size(_tmpBC,12));}));else{Cyc_Absyndump_dump(({const char*_tmpBD="double";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,7));}));}return;
_LLA6: if(*((int*)_tmp46)!= 9)goto _LLA8;_tmp7C=((struct Cyc_Absyn_TupleType_struct*)
_tmp46)->f1;_LLA7: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp7C);return;_LLA8: if(*((int*)_tmp46)!= 10)goto _LLAA;_tmp7D=((struct Cyc_Absyn_AggrType_struct*)
_tmp46)->f1;_tmp7E=_tmp7D.aggr_info;_tmp7F=_tmp7D.targs;_LLA9: {void*_tmpBF;
struct _tuple0*_tmpC0;struct _tuple3 _tmpBE=Cyc_Absyn_aggr_kinded_name(_tmp7E);
_tmpBF=_tmpBE.f1;_tmpC0=_tmpBE.f2;Cyc_Absyndump_dumpaggr_kind(_tmpBF);Cyc_Absyndump_dumpqvar(
_tmpC0);Cyc_Absyndump_dumptps(_tmp7F);return;}_LLAA: if(*((int*)_tmp46)!= 11)goto
_LLAC;_tmp80=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp46)->f1;_tmp81=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp46)->f2;_LLAB: Cyc_Absyndump_dumpaggr_kind(
_tmp80);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp81);
Cyc_Absyndump_dump_char((int)'}');return;_LLAC: if(*((int*)_tmp46)!= 12)goto _LLAE;
_tmp82=((struct Cyc_Absyn_EnumType_struct*)_tmp46)->f1;_LLAD: Cyc_Absyndump_dump(({
const char*_tmpC1="enum ";_tag_dynforward(_tmpC1,sizeof(char),_get_zero_arr_size(
_tmpC1,6));}));Cyc_Absyndump_dumpqvar(_tmp82);return;_LLAE: if(*((int*)_tmp46)!= 
13)goto _LLB0;_tmp83=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp46)->f1;_LLAF: Cyc_Absyndump_dump(({
const char*_tmpC2="enum {";_tag_dynforward(_tmpC2,sizeof(char),_get_zero_arr_size(
_tmpC2,7));}));Cyc_Absyndump_dumpenumfields(_tmp83);Cyc_Absyndump_dump(({const
char*_tmpC3="}";_tag_dynforward(_tmpC3,sizeof(char),_get_zero_arr_size(_tmpC3,2));}));
return;_LLB0: if(*((int*)_tmp46)!= 17)goto _LLB2;_tmp84=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46)->f1;_tmp85=((struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f2;_LLB1:(Cyc_Absyndump_dumpqvar(
_tmp84),Cyc_Absyndump_dumptps(_tmp85));return;_LLB2: if(*((int*)_tmp46)!= 14)goto
_LLB4;_tmp86=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp46)->f1;_LLB3: Cyc_Absyndump_dump(({
const char*_tmpC4="sizeof_t<";_tag_dynforward(_tmpC4,sizeof(char),
_get_zero_arr_size(_tmpC4,10));}));Cyc_Absyndump_dumpntyp(_tmp86);Cyc_Absyndump_dump(({
const char*_tmpC5=">";_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(
_tmpC5,2));}));return;_LLB4: if(*((int*)_tmp46)!= 15)goto _LLB6;_tmp87=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp46)->f1;_LLB5: Cyc_Absyndump_dump(({
const char*_tmpC6="region_t<";_tag_dynforward(_tmpC6,sizeof(char),
_get_zero_arr_size(_tmpC6,10));}));Cyc_Absyndump_dumprgn(_tmp87);Cyc_Absyndump_dump(({
const char*_tmpC7=">";_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(
_tmpC7,2));}));return;_LLB6: if(*((int*)_tmp46)!= 16)goto _LLB8;_tmp88=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp46)->f1;_tmp89=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp46)->f2;_LLB7: Cyc_Absyndump_dump(({const char*_tmpC8="dynregion_t<";
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,13));}));Cyc_Absyndump_dumprgn(
_tmp88);Cyc_Absyndump_dump(({const char*_tmpC9=",";_tag_dynforward(_tmpC9,sizeof(
char),_get_zero_arr_size(_tmpC9,2));}));Cyc_Absyndump_dumprgn(_tmp89);Cyc_Absyndump_dump(({
const char*_tmpCA=">";_tag_dynforward(_tmpCA,sizeof(char),_get_zero_arr_size(
_tmpCA,2));}));return;_LLB8: if(*((int*)_tmp46)!= 19)goto _LLBA;_tmp8A=((struct Cyc_Absyn_TypeInt_struct*)
_tmp46)->f1;_LLB9: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpCD;_tmpCD.tag=1;_tmpCD.f1=(unsigned long)_tmp8A;{void*_tmpCB[1]={& _tmpCD};
Cyc_aprintf(({const char*_tmpCC="`%d";_tag_dynforward(_tmpCC,sizeof(char),
_get_zero_arr_size(_tmpCC,4));}),_tag_dynforward(_tmpCB,sizeof(void*),1));}}));
return;_LLBA: if(*((int*)_tmp46)!= 18)goto _LLBC;_tmp8B=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp46)->f1;_LLBB: Cyc_Absyndump_dump(({const char*_tmpCE="tag_t<";_tag_dynforward(
_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,7));}));Cyc_Absyndump_dumpntyp(
_tmp8B);Cyc_Absyndump_dump(({const char*_tmpCF=">";_tag_dynforward(_tmpCF,sizeof(
char),_get_zero_arr_size(_tmpCF,2));}));return;_LLBC: if((int)_tmp46 != 3)goto
_LLBE;_LLBD: Cyc_Absyndump_dump(({const char*_tmpD0="`U";_tag_dynforward(_tmpD0,
sizeof(char),_get_zero_arr_size(_tmpD0,3));}));goto _LL6B;_LLBE: if((int)_tmp46 != 
2)goto _LLC0;_LLBF: goto _LLC1;_LLC0: if(_tmp46 <= (void*)4)goto _LLC2;if(*((int*)
_tmp46)!= 20)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if(_tmp46 <= (void*)4)goto _LLC4;if(*((
int*)_tmp46)!= 22)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if(_tmp46 <= (void*)4)goto _LL6B;
if(*((int*)_tmp46)!= 21)goto _LL6B;_LLC5: return;_LL6B:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((struct _dynforward_ptr*)
vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void Cyc_Absyndump_dump_rgncmp(
struct _tuple7*cmp){struct _tuple7 _tmpD2;void*_tmpD3;void*_tmpD4;struct _tuple7*
_tmpD1=cmp;_tmpD2=*_tmpD1;_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;Cyc_Absyndump_dumpeff(
_tmpD3);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD4);}void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*
l,struct _dynforward_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,({const char*_tmpD5=",";_tag_dynforward(_tmpD5,sizeof(char),
_get_zero_arr_size(_tmpD5,2));}));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args
!= 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)args->hd);if((args->tl
!= 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}if(
c_varargs)Cyc_Absyndump_dump(({const char*_tmpD6="...";_tag_dynforward(_tmpD6,
sizeof(char),_get_zero_arr_size(_tmpD6,4));}));else{if(cyc_varargs != 0){struct
_tuple1*_tmpD7=({struct _tuple1*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->f1=
cyc_varargs->name;_tmpDA->f2=cyc_varargs->tq;_tmpDA->f3=(void*)cyc_varargs->type;
_tmpDA;});Cyc_Absyndump_dump(({const char*_tmpD8="...";_tag_dynforward(_tmpD8,
sizeof(char),_get_zero_arr_size(_tmpD8,4));}));if(cyc_varargs->inject)Cyc_Absyndump_dump(({
const char*_tmpD9=" inject ";_tag_dynforward(_tmpD9,sizeof(char),
_get_zero_arr_size(_tmpD9,9));}));Cyc_Absyndump_dumpfunarg(_tmpD7);}}if(effopt != 
0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po
!= 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(
int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(void*d){void*
_tmpDB=d;struct Cyc_Absyn_Exp*_tmpDC;struct _dynforward_ptr*_tmpDD;_LLD6: if(*((int*)
_tmpDB)!= 0)goto _LLD8;_tmpDC=((struct Cyc_Absyn_ArrayElement_struct*)_tmpDB)->f1;
_LLD7: Cyc_Absyndump_dump(({const char*_tmpDE=".[";_tag_dynforward(_tmpDE,sizeof(
char),_get_zero_arr_size(_tmpDE,3));}));Cyc_Absyndump_dumpexp(_tmpDC);Cyc_Absyndump_dump_char((
int)']');goto _LLD5;_LLD8: if(*((int*)_tmpDB)!= 1)goto _LLD5;_tmpDD=((struct Cyc_Absyn_FieldName_struct*)
_tmpDB)->f1;_LLD9: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmpDD);goto _LLD5;_LLD5:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple10*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*
de).f1,({const char*_tmpDF="";_tag_dynforward(_tmpDF,sizeof(char),
_get_zero_arr_size(_tmpDF,1));}),({const char*_tmpE0="=";_tag_dynforward(_tmpE0,
sizeof(char),_get_zero_arr_size(_tmpE0,2));}),({const char*_tmpE1="=";
_tag_dynforward(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,2));}));Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_tmpE2="";_tag_dynforward(
_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,1));}),({const char*_tmpE3="";
_tag_dynforward(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,1));}),({const char*
_tmpE4=",";_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,2));}));}
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(
e);if(inprec >= myprec)Cyc_Absyndump_dump_nospace(({const char*_tmpE5="(";
_tag_dynforward(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,2));}));{void*
_tmpE6=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpE7;void*_tmpE8;char _tmpE9;union
Cyc_Absyn_Cnst_union _tmpEA;void*_tmpEB;short _tmpEC;union Cyc_Absyn_Cnst_union
_tmpED;void*_tmpEE;int _tmpEF;union Cyc_Absyn_Cnst_union _tmpF0;void*_tmpF1;int
_tmpF2;union Cyc_Absyn_Cnst_union _tmpF3;void*_tmpF4;int _tmpF5;union Cyc_Absyn_Cnst_union
_tmpF6;void*_tmpF7;long long _tmpF8;union Cyc_Absyn_Cnst_union _tmpF9;struct
_dynforward_ptr _tmpFA;union Cyc_Absyn_Cnst_union _tmpFB;union Cyc_Absyn_Cnst_union
_tmpFC;struct _dynforward_ptr _tmpFD;struct _tuple0*_tmpFE;struct _tuple0*_tmpFF;
void*_tmp100;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Core_Opt*
_tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp105;void*_tmp106;
struct Cyc_Absyn_Exp*_tmp107;void*_tmp108;struct Cyc_Absyn_Exp*_tmp109;void*
_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;void*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;
struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*
_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*
_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*
_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_List_List*
_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*
_tmp11C;void*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;
struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;void*_tmp122;struct Cyc_Absyn_Exp*
_tmp123;void*_tmp124;void*_tmp125;struct _dynforward_ptr*_tmp126;void*_tmp127;
void*_tmp128;unsigned int _tmp129;struct Cyc_List_List*_tmp12A;void*_tmp12B;struct
Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct _dynforward_ptr*_tmp12E;
struct Cyc_Absyn_Exp*_tmp12F;struct _dynforward_ptr*_tmp130;struct Cyc_Absyn_Exp*
_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List*_tmp133;struct _tuple1*
_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Vardecl*
_tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp139;struct _tuple0*
_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*
_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_Tunionfield*_tmp13F;struct
_tuple0*_tmp140;struct _tuple0*_tmp141;struct Cyc_Absyn_MallocInfo _tmp142;int
_tmp143;struct Cyc_Absyn_Exp*_tmp144;void**_tmp145;struct Cyc_Absyn_Exp*_tmp146;
struct Cyc_Core_Opt*_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Stmt*
_tmp149;_LLDB: if(*((int*)_tmpE6)!= 0)goto _LLDD;_tmpE7=((struct Cyc_Absyn_Const_e_struct*)
_tmpE6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).Char_c).tag != 0)
goto _LLDD;_tmpE8=(_tmpE7.Char_c).f1;_tmpE9=(_tmpE7.Char_c).f2;_LLDC: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmpE9));Cyc_Absyndump_dump_char((
int)'\'');goto _LLDA;_LLDD: if(*((int*)_tmpE6)!= 0)goto _LLDF;_tmpEA=((struct Cyc_Absyn_Const_e_struct*)
_tmpE6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).Short_c).tag != 1)
goto _LLDF;_tmpEB=(_tmpEA.Short_c).f1;_tmpEC=(_tmpEA.Short_c).f2;_LLDE: Cyc_Absyndump_dump((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp14C;_tmp14C.tag=1;_tmp14C.f1=(
unsigned long)((int)_tmpEC);{void*_tmp14A[1]={& _tmp14C};Cyc_aprintf(({const char*
_tmp14B="%d";_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,3));}),
_tag_dynforward(_tmp14A,sizeof(void*),1));}}));goto _LLDA;_LLDF: if(*((int*)_tmpE6)
!= 0)goto _LLE1;_tmpED=((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).Int_c).tag != 2)goto _LLE1;_tmpEE=(
_tmpED.Int_c).f1;if((int)_tmpEE != 2)goto _LLE1;_tmpEF=(_tmpED.Int_c).f2;_LLE0:
_tmpF2=_tmpEF;goto _LLE2;_LLE1: if(*((int*)_tmpE6)!= 0)goto _LLE3;_tmpF0=((struct
Cyc_Absyn_Const_e_struct*)_tmpE6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE6)->f1).Int_c).tag != 2)goto _LLE3;_tmpF1=(_tmpF0.Int_c).f1;if((int)_tmpF1 != 
0)goto _LLE3;_tmpF2=(_tmpF0.Int_c).f2;_LLE2: Cyc_Absyndump_dump((struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp14F;_tmp14F.tag=1;_tmp14F.f1=(
unsigned long)_tmpF2;{void*_tmp14D[1]={& _tmp14F};Cyc_aprintf(({const char*_tmp14E="%d";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,3));}),
_tag_dynforward(_tmp14D,sizeof(void*),1));}}));goto _LLDA;_LLE3: if(*((int*)_tmpE6)
!= 0)goto _LLE5;_tmpF3=((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).Int_c).tag != 2)goto _LLE5;_tmpF4=(
_tmpF3.Int_c).f1;if((int)_tmpF4 != 1)goto _LLE5;_tmpF5=(_tmpF3.Int_c).f2;_LLE4: Cyc_Absyndump_dump((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp152;_tmp152.tag=1;_tmp152.f1=(
unsigned int)_tmpF5;{void*_tmp150[1]={& _tmp152};Cyc_aprintf(({const char*_tmp151="%u";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,3));}),
_tag_dynforward(_tmp150,sizeof(void*),1));}}));goto _LLDA;_LLE5: if(*((int*)_tmpE6)
!= 0)goto _LLE7;_tmpF6=((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).LongLong_c).tag != 3)goto _LLE7;
_tmpF7=(_tmpF6.LongLong_c).f1;_tmpF8=(_tmpF6.LongLong_c).f2;_LLE6: Cyc_Absyndump_dump(({
const char*_tmp153="<<FIX LONG LONG CONSTANT>>";_tag_dynforward(_tmp153,sizeof(
char),_get_zero_arr_size(_tmp153,27));}));goto _LLDA;_LLE7: if(*((int*)_tmpE6)!= 0)
goto _LLE9;_tmpF9=((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE6)->f1).Float_c).tag != 4)goto _LLE9;_tmpFA=(_tmpF9.Float_c).f1;_LLE8: Cyc_Absyndump_dump(
_tmpFA);goto _LLDA;_LLE9: if(*((int*)_tmpE6)!= 0)goto _LLEB;_tmpFB=((struct Cyc_Absyn_Const_e_struct*)
_tmpE6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).Null_c).tag != 6)
goto _LLEB;_LLEA: Cyc_Absyndump_dump(({const char*_tmp154="NULL";_tag_dynforward(
_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,5));}));goto _LLDA;_LLEB: if(*((
int*)_tmpE6)!= 0)goto _LLED;_tmpFC=((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE6)->f1).String_c).tag != 5)goto _LLED;
_tmpFD=(_tmpFC.String_c).f1;_LLEC: Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape(_tmpFD));Cyc_Absyndump_dump_char((int)'"');goto _LLDA;
_LLED: if(*((int*)_tmpE6)!= 2)goto _LLEF;_tmpFE=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpE6)->f1;_LLEE: _tmpFF=_tmpFE;goto _LLF0;_LLEF: if(*((int*)_tmpE6)!= 1)goto _LLF1;
_tmpFF=((struct Cyc_Absyn_Var_e_struct*)_tmpE6)->f1;_LLF0: Cyc_Absyndump_dumpqvar(
_tmpFF);goto _LLDA;_LLF1: if(*((int*)_tmpE6)!= 3)goto _LLF3;_tmp100=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmpE6)->f1;_tmp101=((struct Cyc_Absyn_Primop_e_struct*)
_tmpE6)->f2;_LLF2: {struct _dynforward_ptr _tmp155=Cyc_Absynpp_prim2str(_tmp100);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101)){case 1: _LL13F:
if(_tmp100 == (void*)((void*)19)){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp101))->hd);Cyc_Absyndump_dump(({const char*
_tmp156=".size";_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,
6));}));}else{Cyc_Absyndump_dump(_tmp155);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp101))->hd);}break;
case 2: _LL140: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp101))->hd);Cyc_Absyndump_dump(_tmp155);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp101->tl))->hd);break;default: _LL141:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157[0]=({struct
Cyc_Core_Failure_struct _tmp158;_tmp158.tag=Cyc_Core_Failure;_tmp158.f1=({const
char*_tmp159="Absyndump -- Bad number of arguments to primop";_tag_dynforward(
_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,47));});_tmp158;});_tmp157;}));}
goto _LLDA;}_LLF3: if(*((int*)_tmpE6)!= 4)goto _LLF5;_tmp102=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpE6)->f1;_tmp103=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE6)->f2;_tmp104=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpE6)->f3;_LLF4: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp102);if(_tmp103 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp103->v));Cyc_Absyndump_dump_nospace(({const char*_tmp15A="=";_tag_dynforward(
_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,2));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp104);goto _LLDA;_LLF5: if(*((int*)_tmpE6)!= 5)goto _LLF7;_tmp105=((
struct Cyc_Absyn_Increment_e_struct*)_tmpE6)->f1;_tmp106=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpE6)->f2;if((int)_tmp106 != 0)goto _LLF7;_LLF6: Cyc_Absyndump_dump(({const char*
_tmp15B="++";_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,3));}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp105);goto _LLDA;_LLF7: if(*((int*)_tmpE6)!= 
5)goto _LLF9;_tmp107=((struct Cyc_Absyn_Increment_e_struct*)_tmpE6)->f1;_tmp108=(
void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE6)->f2;if((int)_tmp108 != 2)goto
_LLF9;_LLF8: Cyc_Absyndump_dump(({const char*_tmp15C="--";_tag_dynforward(_tmp15C,
sizeof(char),_get_zero_arr_size(_tmp15C,3));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp107);goto _LLDA;_LLF9: if(*((int*)_tmpE6)!= 5)goto _LLFB;_tmp109=((
struct Cyc_Absyn_Increment_e_struct*)_tmpE6)->f1;_tmp10A=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpE6)->f2;if((int)_tmp10A != 1)goto _LLFB;_LLFA: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp109);Cyc_Absyndump_dump(({const char*_tmp15D="++";_tag_dynforward(
_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,3));}));goto _LLDA;_LLFB: if(*((
int*)_tmpE6)!= 5)goto _LLFD;_tmp10B=((struct Cyc_Absyn_Increment_e_struct*)_tmpE6)->f1;
_tmp10C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE6)->f2;if((int)_tmp10C
!= 3)goto _LLFD;_LLFC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp10B);Cyc_Absyndump_dump(({
const char*_tmp15E="--";_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size(
_tmp15E,3));}));goto _LLDA;_LLFD: if(*((int*)_tmpE6)!= 6)goto _LLFF;_tmp10D=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpE6)->f1;_tmp10E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE6)->f2;_tmp10F=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE6)->f3;_LLFE:
Cyc_Absyndump_dumpexp_prec(myprec,_tmp10D);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(
0,_tmp10E);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp10F);goto _LLDA;_LLFF: if(*((int*)_tmpE6)!= 7)goto _LL101;_tmp110=((struct Cyc_Absyn_And_e_struct*)
_tmpE6)->f1;_tmp111=((struct Cyc_Absyn_And_e_struct*)_tmpE6)->f2;_LL100: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp110);Cyc_Absyndump_dump(({const char*_tmp15F=" && ";_tag_dynforward(
_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,5));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp111);goto _LLDA;_LL101: if(*((int*)_tmpE6)!= 8)goto _LL103;_tmp112=((
struct Cyc_Absyn_Or_e_struct*)_tmpE6)->f1;_tmp113=((struct Cyc_Absyn_Or_e_struct*)
_tmpE6)->f2;_LL102: Cyc_Absyndump_dumpexp_prec(myprec,_tmp112);Cyc_Absyndump_dump(({
const char*_tmp160=" || ";_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(
_tmp160,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp113);goto _LLDA;_LL103: if(*((
int*)_tmpE6)!= 9)goto _LL105;_tmp114=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE6)->f1;
_tmp115=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE6)->f2;_LL104: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp114);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp115);Cyc_Absyndump_dump_char((int)')');
goto _LLDA;_LL105: if(*((int*)_tmpE6)!= 10)goto _LL107;_tmp116=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpE6)->f1;_tmp117=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpE6)->f2;_LL106:
_tmp118=_tmp116;_tmp119=_tmp117;goto _LL108;_LL107: if(*((int*)_tmpE6)!= 11)goto
_LL109;_tmp118=((struct Cyc_Absyn_FnCall_e_struct*)_tmpE6)->f1;_tmp119=((struct
Cyc_Absyn_FnCall_e_struct*)_tmpE6)->f2;_LL108: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp118);Cyc_Absyndump_dump_nospace(({const char*_tmp161="(";_tag_dynforward(
_tmp161,sizeof(char),_get_zero_arr_size(_tmp161,2));}));Cyc_Absyndump_dumpexps_prec(
20,_tmp119);Cyc_Absyndump_dump_nospace(({const char*_tmp162=")";_tag_dynforward(
_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,2));}));goto _LLDA;_LL109: if(*((
int*)_tmpE6)!= 12)goto _LL10B;_tmp11A=((struct Cyc_Absyn_Throw_e_struct*)_tmpE6)->f1;
_LL10A: Cyc_Absyndump_dump(({const char*_tmp163="throw";_tag_dynforward(_tmp163,
sizeof(char),_get_zero_arr_size(_tmp163,6));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11A);goto _LLDA;_LL10B: if(*((int*)_tmpE6)!= 13)goto _LL10D;_tmp11B=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE6)->f1;_LL10C: _tmp11C=_tmp11B;goto
_LL10E;_LL10D: if(*((int*)_tmpE6)!= 14)goto _LL10F;_tmp11C=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpE6)->f1;_LL10E: Cyc_Absyndump_dumpexp_prec(inprec,_tmp11C);goto _LLDA;_LL10F:
if(*((int*)_tmpE6)!= 15)goto _LL111;_tmp11D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpE6)->f1;_tmp11E=((struct Cyc_Absyn_Cast_e_struct*)_tmpE6)->f2;_LL110: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp11D);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11E);goto _LLDA;_LL111: if(*((int*)_tmpE6)!= 16)goto _LL113;_tmp11F=((
struct Cyc_Absyn_Address_e_struct*)_tmpE6)->f1;_LL112: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp11F);goto _LLDA;_LL113: if(*((int*)
_tmpE6)!= 17)goto _LL115;_tmp120=((struct Cyc_Absyn_New_e_struct*)_tmpE6)->f1;
_tmp121=((struct Cyc_Absyn_New_e_struct*)_tmpE6)->f2;_LL114: Cyc_Absyndump_dump(({
const char*_tmp164="new ";_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size(
_tmp164,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp121);goto _LLDA;_LL115: if(*((
int*)_tmpE6)!= 18)goto _LL117;_tmp122=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpE6)->f1;_LL116: Cyc_Absyndump_dump(({const char*_tmp165="sizeof(";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,8));}));Cyc_Absyndump_dumptyp(
_tmp122);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL117: if(*((int*)_tmpE6)!= 
19)goto _LL119;_tmp123=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpE6)->f1;_LL118:
Cyc_Absyndump_dump(({const char*_tmp166="sizeof(";_tag_dynforward(_tmp166,sizeof(
char),_get_zero_arr_size(_tmp166,8));}));Cyc_Absyndump_dumpexp(_tmp123);Cyc_Absyndump_dump_char((
int)')');goto _LLDA;_LL119: if(*((int*)_tmpE6)!= 20)goto _LL11B;_tmp124=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmpE6)->f1;_tmp125=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpE6)->f2;if(*((int*)_tmp125)!= 0)goto _LL11B;_tmp126=((struct Cyc_Absyn_StructField_struct*)
_tmp125)->f1;_LL11A: Cyc_Absyndump_dump(({const char*_tmp167="offsetof(";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,10));}));Cyc_Absyndump_dumptyp(
_tmp124);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp126);
Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL11B: if(*((int*)_tmpE6)!= 20)goto
_LL11D;_tmp127=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE6)->f1;_tmp128=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE6)->f2;if(*((int*)_tmp128)!= 1)
goto _LL11D;_tmp129=((struct Cyc_Absyn_TupleIndex_struct*)_tmp128)->f1;_LL11C: Cyc_Absyndump_dump(({
const char*_tmp168="offsetof(";_tag_dynforward(_tmp168,sizeof(char),
_get_zero_arr_size(_tmp168,10));}));Cyc_Absyndump_dumptyp(_tmp127);Cyc_Absyndump_dump_char((
int)',');Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp16B;_tmp16B.tag=1;_tmp16B.f1=(unsigned long)((int)_tmp129);{void*_tmp169[1]={&
_tmp16B};Cyc_aprintf(({const char*_tmp16A="%d";_tag_dynforward(_tmp16A,sizeof(
char),_get_zero_arr_size(_tmp16A,3));}),_tag_dynforward(_tmp169,sizeof(void*),1));}}));
Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL11D: if(*((int*)_tmpE6)!= 21)goto
_LL11F;_tmp12A=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpE6)->f1;_tmp12B=(void*)((
struct Cyc_Absyn_Gentyp_e_struct*)_tmpE6)->f2;_LL11E: Cyc_Absyndump_dump(({const
char*_tmp16C="__gen(";_tag_dynforward(_tmp16C,sizeof(char),_get_zero_arr_size(
_tmp16C,7));}));Cyc_Absyndump_dumptvars(_tmp12A);Cyc_Absyndump_dumptyp(_tmp12B);
Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL11F: if(*((int*)_tmpE6)!= 22)goto
_LL121;_tmp12C=((struct Cyc_Absyn_Deref_e_struct*)_tmpE6)->f1;_LL120: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp12C);goto _LLDA;_LL121: if(*((int*)
_tmpE6)!= 23)goto _LL123;_tmp12D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE6)->f1;
_tmp12E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE6)->f2;_LL122: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp12D);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp12E);goto _LLDA;_LL123: if(*((int*)_tmpE6)!= 24)goto _LL125;_tmp12F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE6)->f1;_tmp130=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpE6)->f2;_LL124: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp12F);Cyc_Absyndump_dump_nospace(({const char*_tmp16D="->";
_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,3));}));Cyc_Absyndump_dump_nospace(*
_tmp130);goto _LLDA;_LL125: if(*((int*)_tmpE6)!= 25)goto _LL127;_tmp131=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpE6)->f1;_tmp132=((struct Cyc_Absyn_Subscript_e_struct*)_tmpE6)->f2;_LL126: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp131);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp132);
Cyc_Absyndump_dump_char((int)']');goto _LLDA;_LL127: if(*((int*)_tmpE6)!= 26)goto
_LL129;_tmp133=((struct Cyc_Absyn_Tuple_e_struct*)_tmpE6)->f1;_LL128: Cyc_Absyndump_dump(({
const char*_tmp16E="$(";_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size(
_tmp16E,3));}));Cyc_Absyndump_dumpexps_prec(20,_tmp133);Cyc_Absyndump_dump_char((
int)')');goto _LLDA;_LL129: if(*((int*)_tmpE6)!= 27)goto _LL12B;_tmp134=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpE6)->f1;_tmp135=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpE6)->f2;_LL12A:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*_tmp134).f3);Cyc_Absyndump_dump_char((
int)')');((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp135,({const char*_tmp16F="{";_tag_dynforward(_tmp16F,
sizeof(char),_get_zero_arr_size(_tmp16F,2));}),({const char*_tmp170="}";
_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size(_tmp170,2));}),({const
char*_tmp171=",";_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,
2));}));goto _LLDA;_LL12B: if(*((int*)_tmpE6)!= 28)goto _LL12D;_tmp136=((struct Cyc_Absyn_Array_e_struct*)
_tmpE6)->f1;_LL12C:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp136,({const char*_tmp172="{";
_tag_dynforward(_tmp172,sizeof(char),_get_zero_arr_size(_tmp172,2));}),({const
char*_tmp173="}";_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,
2));}),({const char*_tmp174=",";_tag_dynforward(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,2));}));goto _LLDA;_LL12D: if(*((int*)_tmpE6)!= 29)goto
_LL12F;_tmp137=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE6)->f1;_tmp138=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpE6)->f2;_tmp139=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpE6)->f3;_LL12E: Cyc_Absyndump_dump(({const char*_tmp175="new {for";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,9));}));Cyc_Absyndump_dump_str((*
_tmp137->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(
_tmp138);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp139);Cyc_Absyndump_dump_char((
int)'}');goto _LLDA;_LL12F: if(*((int*)_tmpE6)!= 30)goto _LL131;_tmp13A=((struct Cyc_Absyn_Struct_e_struct*)
_tmpE6)->f1;_tmp13B=((struct Cyc_Absyn_Struct_e_struct*)_tmpE6)->f2;_tmp13C=((
struct Cyc_Absyn_Struct_e_struct*)_tmpE6)->f3;_LL130: Cyc_Absyndump_dumpqvar(
_tmp13A);Cyc_Absyndump_dump_char((int)'{');if(_tmp13B != 0)Cyc_Absyndump_dumptps(
_tmp13B);((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp13C,({const char*_tmp176="";_tag_dynforward(_tmp176,
sizeof(char),_get_zero_arr_size(_tmp176,1));}),({const char*_tmp177="}";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,2));}),({const
char*_tmp178=",";_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,
2));}));goto _LLDA;_LL131: if(*((int*)_tmpE6)!= 31)goto _LL133;_tmp13D=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpE6)->f2;_LL132:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp13D,({const char*_tmp179="{";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,2));}),({const
char*_tmp17A="}";_tag_dynforward(_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,
2));}),({const char*_tmp17B=",";_tag_dynforward(_tmp17B,sizeof(char),
_get_zero_arr_size(_tmp17B,2));}));goto _LLDA;_LL133: if(*((int*)_tmpE6)!= 32)goto
_LL135;_tmp13E=((struct Cyc_Absyn_Tunion_e_struct*)_tmpE6)->f1;_tmp13F=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpE6)->f3;_LL134: Cyc_Absyndump_dumpqvar(_tmp13F->name);
if(_tmp13E != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp13E,({const char*_tmp17C="(";
_tag_dynforward(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,2));}),({const
char*_tmp17D=")";_tag_dynforward(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,
2));}),({const char*_tmp17E=",";_tag_dynforward(_tmp17E,sizeof(char),
_get_zero_arr_size(_tmp17E,2));}));goto _LLDA;_LL135: if(*((int*)_tmpE6)!= 33)goto
_LL137;_tmp140=((struct Cyc_Absyn_Enum_e_struct*)_tmpE6)->f1;_LL136: Cyc_Absyndump_dumpqvar(
_tmp140);goto _LLDA;_LL137: if(*((int*)_tmpE6)!= 34)goto _LL139;_tmp141=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpE6)->f1;_LL138: Cyc_Absyndump_dumpqvar(_tmp141);goto _LLDA;_LL139: if(*((int*)
_tmpE6)!= 35)goto _LL13B;_tmp142=((struct Cyc_Absyn_Malloc_e_struct*)_tmpE6)->f1;
_tmp143=_tmp142.is_calloc;_tmp144=_tmp142.rgn;_tmp145=_tmp142.elt_type;_tmp146=
_tmp142.num_elts;_LL13A: if(_tmp143){if(_tmp144 != 0){Cyc_Absyndump_dump(({const
char*_tmp17F="rcalloc(";_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size(
_tmp17F,9));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp144);Cyc_Absyndump_dump(({
const char*_tmp180=",";_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(
_tmp180,2));}));}else{Cyc_Absyndump_dump(({const char*_tmp181="calloc";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,7));}));}Cyc_Absyndump_dumpexp(
_tmp146);Cyc_Absyndump_dump(({const char*_tmp182=",";_tag_dynforward(_tmp182,
sizeof(char),_get_zero_arr_size(_tmp182,2));}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp145)),0));Cyc_Absyndump_dump(({const char*_tmp183=")";
_tag_dynforward(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,2));}));}else{
if(_tmp144 != 0){Cyc_Absyndump_dump(({const char*_tmp184="rmalloc(";
_tag_dynforward(_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp144);Cyc_Absyndump_dump(({const char*_tmp185=",";
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,2));}));}else{
Cyc_Absyndump_dump(({const char*_tmp186="malloc(";_tag_dynforward(_tmp186,sizeof(
char),_get_zero_arr_size(_tmp186,8));}));}if(_tmp145 != 0)Cyc_Absyndump_dumpexp(
Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp145,0),_tmp146,0));else{Cyc_Absyndump_dumpexp(
_tmp146);}Cyc_Absyndump_dump(({const char*_tmp187=")";_tag_dynforward(_tmp187,
sizeof(char),_get_zero_arr_size(_tmp187,2));}));}goto _LLDA;_LL13B: if(*((int*)
_tmpE6)!= 36)goto _LL13D;_tmp147=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE6)->f1;
_tmp148=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE6)->f2;_LL13C:((void(*)(
void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp148,({const char*_tmp188="{";_tag_dynforward(_tmp188,sizeof(char),
_get_zero_arr_size(_tmp188,2));}),({const char*_tmp189="}";_tag_dynforward(
_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,2));}),({const char*_tmp18A=",";
_tag_dynforward(_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,2));}));goto
_LLDA;_LL13D: if(*((int*)_tmpE6)!= 37)goto _LLDA;_tmp149=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpE6)->f1;_LL13E: Cyc_Absyndump_dump_nospace(({const char*_tmp18B="({";
_tag_dynforward(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,3));}));Cyc_Absyndump_dumpstmt(
_tmp149);Cyc_Absyndump_dump_nospace(({const char*_tmp18C="})";_tag_dynforward(
_tmp18C,sizeof(char),_get_zero_arr_size(_tmp18C,3));}));goto _LLDA;_LLDA:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp18D=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp18D->where_clause == 0  && (
void*)(_tmp18D->pattern)->r == (void*)((void*)0))Cyc_Absyndump_dump(({const char*
_tmp18E="default:";_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size(
_tmp18E,9));}));else{Cyc_Absyndump_dump(({const char*_tmp18F="case";
_tag_dynforward(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,5));}));Cyc_Absyndump_dumppat(
_tmp18D->pattern);if(_tmp18D->where_clause != 0){Cyc_Absyndump_dump(({const char*
_tmp190="&&";_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,3));}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp18D->where_clause));}
Cyc_Absyndump_dump_nospace(({const char*_tmp191=":";_tag_dynforward(_tmp191,
sizeof(char),_get_zero_arr_size(_tmp191,2));}));}Cyc_Absyndump_dumpstmt(_tmp18D->body);}}
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmp192=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Stmt*_tmp194;struct Cyc_Absyn_Stmt*
_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*
_tmp198;struct Cyc_Absyn_Stmt*_tmp199;struct Cyc_Absyn_Stmt*_tmp19A;struct _tuple2
_tmp19B;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Stmt*_tmp19D;struct
_dynforward_ptr*_tmp19E;struct Cyc_Absyn_Exp*_tmp19F;struct _tuple2 _tmp1A0;struct
Cyc_Absyn_Exp*_tmp1A1;struct _tuple2 _tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Stmt*
_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_List_List*_tmp1A6;struct Cyc_Absyn_Decl*
_tmp1A7;struct Cyc_Absyn_Stmt*_tmp1A8;struct _dynforward_ptr*_tmp1A9;struct Cyc_Absyn_Stmt*
_tmp1AA;struct Cyc_Absyn_Stmt*_tmp1AB;struct _tuple2 _tmp1AC;struct Cyc_Absyn_Exp*
_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Stmt*
_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Tvar*_tmp1B2;struct Cyc_Absyn_Vardecl*
_tmp1B3;int _tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Stmt*_tmp1B6;
struct Cyc_Absyn_Exp*_tmp1B7;_LL144: if((int)_tmp192 != 0)goto _LL146;_LL145: Cyc_Absyndump_dump_semi();
goto _LL143;_LL146: if(_tmp192 <= (void*)1)goto _LL148;if(*((int*)_tmp192)!= 0)goto
_LL148;_tmp193=((struct Cyc_Absyn_Exp_s_struct*)_tmp192)->f1;_LL147: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dumpexp(_tmp193);Cyc_Absyndump_dump_semi();goto _LL143;
_LL148: if(_tmp192 <= (void*)1)goto _LL14A;if(*((int*)_tmp192)!= 1)goto _LL14A;
_tmp194=((struct Cyc_Absyn_Seq_s_struct*)_tmp192)->f1;_tmp195=((struct Cyc_Absyn_Seq_s_struct*)
_tmp192)->f2;_LL149: if(Cyc_Absynpp_is_declaration(_tmp194)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp194);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dumpstmt(_tmp194);}if(Cyc_Absynpp_is_declaration(_tmp195)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp195);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp195);}goto _LL143;_LL14A: if(_tmp192 <= (
void*)1)goto _LL14C;if(*((int*)_tmp192)!= 2)goto _LL14C;_tmp196=((struct Cyc_Absyn_Return_s_struct*)
_tmp192)->f1;if(_tmp196 != 0)goto _LL14C;_LL14B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1B8="return;";_tag_dynforward(_tmp1B8,sizeof(char),
_get_zero_arr_size(_tmp1B8,8));}));goto _LL143;_LL14C: if(_tmp192 <= (void*)1)goto
_LL14E;if(*((int*)_tmp192)!= 2)goto _LL14E;_tmp197=((struct Cyc_Absyn_Return_s_struct*)
_tmp192)->f1;_LL14D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1B9="return";_tag_dynforward(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,
7));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp197));Cyc_Absyndump_dump_semi();
goto _LL143;_LL14E: if(_tmp192 <= (void*)1)goto _LL150;if(*((int*)_tmp192)!= 3)goto
_LL150;_tmp198=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp192)->f1;_tmp199=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp192)->f2;_tmp19A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp192)->f3;_LL14F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1BA="if(";_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,4));}));
Cyc_Absyndump_dumpexp(_tmp198);{void*_tmp1BB=(void*)_tmp199->r;_LL16D: if(_tmp1BB
<= (void*)1)goto _LL175;if(*((int*)_tmp1BB)!= 1)goto _LL16F;_LL16E: goto _LL170;
_LL16F: if(*((int*)_tmp1BB)!= 11)goto _LL171;_LL170: goto _LL172;_LL171: if(*((int*)
_tmp1BB)!= 3)goto _LL173;_LL172: goto _LL174;_LL173: if(*((int*)_tmp1BB)!= 12)goto
_LL175;_LL174: Cyc_Absyndump_dump_nospace(({const char*_tmp1BC="){";
_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,3));}));Cyc_Absyndump_dumpstmt(
_tmp199);Cyc_Absyndump_dump_char((int)'}');goto _LL16C;_LL175:;_LL176: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmp199);_LL16C:;}{void*_tmp1BD=(void*)_tmp19A->r;
_LL178: if((int)_tmp1BD != 0)goto _LL17A;_LL179: goto _LL177;_LL17A:;_LL17B: Cyc_Absyndump_dump(({
const char*_tmp1BE="else{";_tag_dynforward(_tmp1BE,sizeof(char),
_get_zero_arr_size(_tmp1BE,6));}));Cyc_Absyndump_dumpstmt(_tmp19A);Cyc_Absyndump_dump_char((
int)'}');goto _LL177;_LL177:;}goto _LL143;_LL150: if(_tmp192 <= (void*)1)goto _LL152;
if(*((int*)_tmp192)!= 4)goto _LL152;_tmp19B=((struct Cyc_Absyn_While_s_struct*)
_tmp192)->f1;_tmp19C=_tmp19B.f1;_tmp19D=((struct Cyc_Absyn_While_s_struct*)
_tmp192)->f2;_LL151: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1BF="while(";_tag_dynforward(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,
7));}));Cyc_Absyndump_dumpexp(_tmp19C);Cyc_Absyndump_dump_nospace(({const char*
_tmp1C0="){";_tag_dynforward(_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,3));}));
Cyc_Absyndump_dumpstmt(_tmp19D);Cyc_Absyndump_dump_char((int)'}');goto _LL143;
_LL152: if(_tmp192 <= (void*)1)goto _LL154;if(*((int*)_tmp192)!= 5)goto _LL154;
_LL153: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1C1="break;";
_tag_dynforward(_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,7));}));goto
_LL143;_LL154: if(_tmp192 <= (void*)1)goto _LL156;if(*((int*)_tmp192)!= 6)goto
_LL156;_LL155: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1C2="continue;";_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(
_tmp1C2,10));}));goto _LL143;_LL156: if(_tmp192 <= (void*)1)goto _LL158;if(*((int*)
_tmp192)!= 7)goto _LL158;_tmp19E=((struct Cyc_Absyn_Goto_s_struct*)_tmp192)->f1;
_LL157: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1C3="goto";
_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,5));}));Cyc_Absyndump_dump_str(
_tmp19E);Cyc_Absyndump_dump_semi();goto _LL143;_LL158: if(_tmp192 <= (void*)1)goto
_LL15A;if(*((int*)_tmp192)!= 8)goto _LL15A;_tmp19F=((struct Cyc_Absyn_For_s_struct*)
_tmp192)->f1;_tmp1A0=((struct Cyc_Absyn_For_s_struct*)_tmp192)->f2;_tmp1A1=
_tmp1A0.f1;_tmp1A2=((struct Cyc_Absyn_For_s_struct*)_tmp192)->f3;_tmp1A3=_tmp1A2.f1;
_tmp1A4=((struct Cyc_Absyn_For_s_struct*)_tmp192)->f4;_LL159: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C4="for(";_tag_dynforward(_tmp1C4,
sizeof(char),_get_zero_arr_size(_tmp1C4,5));}));Cyc_Absyndump_dumpexp(_tmp19F);
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1A1);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp1A3);Cyc_Absyndump_dump_nospace(({const char*_tmp1C5="){";
_tag_dynforward(_tmp1C5,sizeof(char),_get_zero_arr_size(_tmp1C5,3));}));Cyc_Absyndump_dumpstmt(
_tmp1A4);Cyc_Absyndump_dump_char((int)'}');goto _LL143;_LL15A: if(_tmp192 <= (void*)
1)goto _LL15C;if(*((int*)_tmp192)!= 9)goto _LL15C;_tmp1A5=((struct Cyc_Absyn_Switch_s_struct*)
_tmp192)->f1;_tmp1A6=((struct Cyc_Absyn_Switch_s_struct*)_tmp192)->f2;_LL15B: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C6="switch(";_tag_dynforward(_tmp1C6,
sizeof(char),_get_zero_arr_size(_tmp1C6,8));}));Cyc_Absyndump_dumpexp(_tmp1A5);
Cyc_Absyndump_dump_nospace(({const char*_tmp1C7="){";_tag_dynforward(_tmp1C7,
sizeof(char),_get_zero_arr_size(_tmp1C7,3));}));Cyc_Absyndump_dumpswitchclauses(
_tmp1A6);Cyc_Absyndump_dump_char((int)'}');goto _LL143;_LL15C: if(_tmp192 <= (void*)
1)goto _LL15E;if(*((int*)_tmp192)!= 11)goto _LL15E;_tmp1A7=((struct Cyc_Absyn_Decl_s_struct*)
_tmp192)->f1;_tmp1A8=((struct Cyc_Absyn_Decl_s_struct*)_tmp192)->f2;_LL15D: Cyc_Absyndump_dumpdecl(
_tmp1A7);Cyc_Absyndump_dumpstmt(_tmp1A8);goto _LL143;_LL15E: if(_tmp192 <= (void*)1)
goto _LL160;if(*((int*)_tmp192)!= 12)goto _LL160;_tmp1A9=((struct Cyc_Absyn_Label_s_struct*)
_tmp192)->f1;_tmp1AA=((struct Cyc_Absyn_Label_s_struct*)_tmp192)->f2;_LL15F: if(
Cyc_Absynpp_is_declaration(_tmp1AA)){Cyc_Absyndump_dump_str(_tmp1A9);Cyc_Absyndump_dump_nospace(({
const char*_tmp1C8=": {";_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(
_tmp1C8,4));}));Cyc_Absyndump_dumpstmt(_tmp1AA);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dump_str(_tmp1A9);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(
_tmp1AA);}goto _LL143;_LL160: if(_tmp192 <= (void*)1)goto _LL162;if(*((int*)_tmp192)
!= 13)goto _LL162;_tmp1AB=((struct Cyc_Absyn_Do_s_struct*)_tmp192)->f1;_tmp1AC=((
struct Cyc_Absyn_Do_s_struct*)_tmp192)->f2;_tmp1AD=_tmp1AC.f1;_LL161: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C9="do{";_tag_dynforward(_tmp1C9,
sizeof(char),_get_zero_arr_size(_tmp1C9,4));}));Cyc_Absyndump_dumpstmt(_tmp1AB);
Cyc_Absyndump_dump_nospace(({const char*_tmp1CA="}while(";_tag_dynforward(_tmp1CA,
sizeof(char),_get_zero_arr_size(_tmp1CA,8));}));Cyc_Absyndump_dumpexp(_tmp1AD);
Cyc_Absyndump_dump_nospace(({const char*_tmp1CB=");";_tag_dynforward(_tmp1CB,
sizeof(char),_get_zero_arr_size(_tmp1CB,3));}));goto _LL143;_LL162: if(_tmp192 <= (
void*)1)goto _LL164;if(*((int*)_tmp192)!= 10)goto _LL164;_tmp1AE=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp192)->f1;if(_tmp1AE != 0)goto _LL164;_LL163: Cyc_Absyndump_dump(({const char*
_tmp1CC="fallthru;";_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(
_tmp1CC,10));}));goto _LL143;_LL164: if(_tmp192 <= (void*)1)goto _LL166;if(*((int*)
_tmp192)!= 10)goto _LL166;_tmp1AF=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp192)->f1;
_LL165: Cyc_Absyndump_dump(({const char*_tmp1CD="fallthru(";_tag_dynforward(
_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,10));}));Cyc_Absyndump_dumpexps_prec(
20,_tmp1AF);Cyc_Absyndump_dump_nospace(({const char*_tmp1CE=");";_tag_dynforward(
_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,3));}));goto _LL143;_LL166: if(
_tmp192 <= (void*)1)goto _LL168;if(*((int*)_tmp192)!= 14)goto _LL168;_tmp1B0=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp192)->f1;_tmp1B1=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp192)->f2;_LL167: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1CF="try";_tag_dynforward(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,4));}));
Cyc_Absyndump_dumpstmt(_tmp1B0);Cyc_Absyndump_dump(({const char*_tmp1D0="catch{";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,7));}));Cyc_Absyndump_dumpswitchclauses(
_tmp1B1);Cyc_Absyndump_dump_char((int)'}');goto _LL143;_LL168: if(_tmp192 <= (void*)
1)goto _LL16A;if(*((int*)_tmp192)!= 15)goto _LL16A;_tmp1B2=((struct Cyc_Absyn_Region_s_struct*)
_tmp192)->f1;_tmp1B3=((struct Cyc_Absyn_Region_s_struct*)_tmp192)->f2;_tmp1B4=((
struct Cyc_Absyn_Region_s_struct*)_tmp192)->f3;_tmp1B5=((struct Cyc_Absyn_Region_s_struct*)
_tmp192)->f4;_tmp1B6=((struct Cyc_Absyn_Region_s_struct*)_tmp192)->f5;_LL169: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1D1="region";_tag_dynforward(_tmp1D1,
sizeof(char),_get_zero_arr_size(_tmp1D1,7));}));if(_tmp1B4)Cyc_Absyndump_dump(({
const char*_tmp1D2="[resetable]";_tag_dynforward(_tmp1D2,sizeof(char),
_get_zero_arr_size(_tmp1D2,12));}));Cyc_Absyndump_dump(({const char*_tmp1D3="<";
_tag_dynforward(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,2));}));Cyc_Absyndump_dumptvar(
_tmp1B2);Cyc_Absyndump_dump(({const char*_tmp1D4="> ";_tag_dynforward(_tmp1D4,
sizeof(char),_get_zero_arr_size(_tmp1D4,3));}));Cyc_Absyndump_dumpqvar(_tmp1B3->name);
if(_tmp1B5 != 0){Cyc_Absyndump_dump(({const char*_tmp1D5=" = open(";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp1B5);Cyc_Absyndump_dump(({const char*_tmp1D6=")";
_tag_dynforward(_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,2));}));}Cyc_Absyndump_dump(({
const char*_tmp1D7="{";_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size(
_tmp1D7,2));}));Cyc_Absyndump_dumpstmt(_tmp1B6);Cyc_Absyndump_dump(({const char*
_tmp1D8="}";_tag_dynforward(_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,2));}));
goto _LL143;_LL16A: if(_tmp192 <= (void*)1)goto _LL143;if(*((int*)_tmp192)!= 16)goto
_LL143;_tmp1B7=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp192)->f1;_LL16B: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1D9="reset_region(";_tag_dynforward(
_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,14));}));Cyc_Absyndump_dumpexp(
_tmp1B7);Cyc_Absyndump_dump(({const char*_tmp1DA=");";_tag_dynforward(_tmp1DA,
sizeof(char),_get_zero_arr_size(_tmp1DA,3));}));goto _LL143;_LL143:;}struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple11*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,({
const char*_tmp1DB="";_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size(
_tmp1DB,1));}),({const char*_tmp1DC="=";_tag_dynforward(_tmp1DC,sizeof(char),
_get_zero_arr_size(_tmp1DC,2));}),({const char*_tmp1DD="=";_tag_dynforward(
_tmp1DD,sizeof(char),_get_zero_arr_size(_tmp1DD,2));}));Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp1DE=(void*)p->r;
void*_tmp1DF;int _tmp1E0;void*_tmp1E1;int _tmp1E2;void*_tmp1E3;int _tmp1E4;char
_tmp1E5;struct _dynforward_ptr _tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E7;struct Cyc_Absyn_Pat*
_tmp1E8;struct Cyc_Absyn_Pat _tmp1E9;void*_tmp1EA;struct Cyc_Absyn_Vardecl*_tmp1EB;
struct Cyc_Absyn_Pat*_tmp1EC;struct Cyc_List_List*_tmp1ED;int _tmp1EE;struct Cyc_Absyn_Pat*
_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Pat*_tmp1F1;struct Cyc_Absyn_Pat
_tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Vardecl*_tmp1F4;struct Cyc_Absyn_Pat*_tmp1F5;
struct Cyc_Absyn_Tvar*_tmp1F6;struct Cyc_Absyn_Vardecl*_tmp1F7;struct _tuple0*
_tmp1F8;struct _tuple0*_tmp1F9;struct Cyc_List_List*_tmp1FA;int _tmp1FB;struct Cyc_Absyn_AggrInfo
_tmp1FC;union Cyc_Absyn_AggrInfoU_union _tmp1FD;struct Cyc_List_List*_tmp1FE;struct
Cyc_List_List*_tmp1FF;int _tmp200;struct Cyc_Absyn_Tunionfield*_tmp201;struct Cyc_List_List*
_tmp202;int _tmp203;struct Cyc_Absyn_Enumfield*_tmp204;struct Cyc_Absyn_Enumfield*
_tmp205;struct Cyc_Absyn_Exp*_tmp206;_LL17D: if((int)_tmp1DE != 0)goto _LL17F;_LL17E:
Cyc_Absyndump_dump_char((int)'_');goto _LL17C;_LL17F: if((int)_tmp1DE != 1)goto
_LL181;_LL180: Cyc_Absyndump_dump(({const char*_tmp207="NULL";_tag_dynforward(
_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,5));}));goto _LL17C;_LL181: if(
_tmp1DE <= (void*)2)goto _LL183;if(*((int*)_tmp1DE)!= 7)goto _LL183;_tmp1DF=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f1;if((int)_tmp1DF != 2)goto _LL183;
_tmp1E0=((struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f2;_LL182: _tmp1E2=_tmp1E0;goto
_LL184;_LL183: if(_tmp1DE <= (void*)2)goto _LL185;if(*((int*)_tmp1DE)!= 7)goto
_LL185;_tmp1E1=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f1;if((int)
_tmp1E1 != 0)goto _LL185;_tmp1E2=((struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f2;
_LL184: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp20A;_tmp20A.tag=1;_tmp20A.f1=(unsigned long)_tmp1E2;{void*_tmp208[1]={&
_tmp20A};Cyc_aprintf(({const char*_tmp209="%d";_tag_dynforward(_tmp209,sizeof(
char),_get_zero_arr_size(_tmp209,3));}),_tag_dynforward(_tmp208,sizeof(void*),1));}}));
goto _LL17C;_LL185: if(_tmp1DE <= (void*)2)goto _LL187;if(*((int*)_tmp1DE)!= 7)goto
_LL187;_tmp1E3=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f1;if((int)
_tmp1E3 != 1)goto _LL187;_tmp1E4=((struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f2;
_LL186: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp20D;_tmp20D.tag=1;_tmp20D.f1=(unsigned int)_tmp1E4;{void*_tmp20B[1]={&
_tmp20D};Cyc_aprintf(({const char*_tmp20C="%u";_tag_dynforward(_tmp20C,sizeof(
char),_get_zero_arr_size(_tmp20C,3));}),_tag_dynforward(_tmp20B,sizeof(void*),1));}}));
goto _LL17C;_LL187: if(_tmp1DE <= (void*)2)goto _LL189;if(*((int*)_tmp1DE)!= 8)goto
_LL189;_tmp1E5=((struct Cyc_Absyn_Char_p_struct*)_tmp1DE)->f1;_LL188: Cyc_Absyndump_dump(({
const char*_tmp20E="'";_tag_dynforward(_tmp20E,sizeof(char),_get_zero_arr_size(
_tmp20E,2));}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1E5));Cyc_Absyndump_dump_nospace(({
const char*_tmp20F="'";_tag_dynforward(_tmp20F,sizeof(char),_get_zero_arr_size(
_tmp20F,2));}));goto _LL17C;_LL189: if(_tmp1DE <= (void*)2)goto _LL18B;if(*((int*)
_tmp1DE)!= 9)goto _LL18B;_tmp1E6=((struct Cyc_Absyn_Float_p_struct*)_tmp1DE)->f1;
_LL18A: Cyc_Absyndump_dump(_tmp1E6);goto _LL17C;_LL18B: if(_tmp1DE <= (void*)2)goto
_LL18D;if(*((int*)_tmp1DE)!= 0)goto _LL18D;_tmp1E7=((struct Cyc_Absyn_Var_p_struct*)
_tmp1DE)->f1;_tmp1E8=((struct Cyc_Absyn_Var_p_struct*)_tmp1DE)->f2;_tmp1E9=*
_tmp1E8;_tmp1EA=(void*)_tmp1E9.r;if((int)_tmp1EA != 0)goto _LL18D;_LL18C: Cyc_Absyndump_dumpqvar(
_tmp1E7->name);goto _LL17C;_LL18D: if(_tmp1DE <= (void*)2)goto _LL18F;if(*((int*)
_tmp1DE)!= 0)goto _LL18F;_tmp1EB=((struct Cyc_Absyn_Var_p_struct*)_tmp1DE)->f1;
_tmp1EC=((struct Cyc_Absyn_Var_p_struct*)_tmp1DE)->f2;_LL18E: Cyc_Absyndump_dumpqvar(
_tmp1EB->name);Cyc_Absyndump_dump(({const char*_tmp210=" as ";_tag_dynforward(
_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,5));}));Cyc_Absyndump_dumppat(
_tmp1EC);goto _LL17C;_LL18F: if(_tmp1DE <= (void*)2)goto _LL191;if(*((int*)_tmp1DE)
!= 3)goto _LL191;_tmp1ED=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1DE)->f1;_tmp1EE=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp1DE)->f2;_LL190: {struct _dynforward_ptr term=
_tmp1EE?({const char*_tmp213=", ...)";_tag_dynforward(_tmp213,sizeof(char),
_get_zero_arr_size(_tmp213,7));}):({const char*_tmp214=")";_tag_dynforward(
_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,2));});((void(*)(void(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp1ED,({const char*_tmp211="$(";_tag_dynforward(_tmp211,sizeof(char),
_get_zero_arr_size(_tmp211,3));}),term,({const char*_tmp212=",";_tag_dynforward(
_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,2));}));goto _LL17C;}_LL191: if(
_tmp1DE <= (void*)2)goto _LL193;if(*((int*)_tmp1DE)!= 4)goto _LL193;_tmp1EF=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp1DE)->f1;_LL192: Cyc_Absyndump_dump(({const
char*_tmp215="&";_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,
2));}));Cyc_Absyndump_dumppat(_tmp1EF);goto _LL17C;_LL193: if(_tmp1DE <= (void*)2)
goto _LL195;if(*((int*)_tmp1DE)!= 1)goto _LL195;_tmp1F0=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1DE)->f1;_tmp1F1=((struct Cyc_Absyn_Reference_p_struct*)_tmp1DE)->f2;_tmp1F2=*
_tmp1F1;_tmp1F3=(void*)_tmp1F2.r;if((int)_tmp1F3 != 0)goto _LL195;_LL194: Cyc_Absyndump_dump(({
const char*_tmp216="*";_tag_dynforward(_tmp216,sizeof(char),_get_zero_arr_size(
_tmp216,2));}));Cyc_Absyndump_dumpqvar(_tmp1F0->name);goto _LL17C;_LL195: if(
_tmp1DE <= (void*)2)goto _LL197;if(*((int*)_tmp1DE)!= 1)goto _LL197;_tmp1F4=((
struct Cyc_Absyn_Reference_p_struct*)_tmp1DE)->f1;_tmp1F5=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1DE)->f2;_LL196: Cyc_Absyndump_dump(({const char*_tmp217="*";_tag_dynforward(
_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,2));}));Cyc_Absyndump_dumpqvar(
_tmp1F4->name);Cyc_Absyndump_dump(({const char*_tmp218=" as ";_tag_dynforward(
_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,5));}));Cyc_Absyndump_dumppat(
_tmp1F5);goto _LL17C;_LL197: if(_tmp1DE <= (void*)2)goto _LL199;if(*((int*)_tmp1DE)
!= 2)goto _LL199;_tmp1F6=((struct Cyc_Absyn_TagInt_p_struct*)_tmp1DE)->f1;_tmp1F7=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp1DE)->f2;_LL198: Cyc_Absyndump_dumpqvar(
_tmp1F7->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp1F6);
Cyc_Absyndump_dump_char((int)'>');goto _LL17C;_LL199: if(_tmp1DE <= (void*)2)goto
_LL19B;if(*((int*)_tmp1DE)!= 12)goto _LL19B;_tmp1F8=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1DE)->f1;_LL19A: Cyc_Absyndump_dumpqvar(_tmp1F8);goto _LL17C;_LL19B: if(_tmp1DE
<= (void*)2)goto _LL19D;if(*((int*)_tmp1DE)!= 13)goto _LL19D;_tmp1F9=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1DE)->f1;_tmp1FA=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1DE)->f2;
_tmp1FB=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1DE)->f3;_LL19C: {struct
_dynforward_ptr term=_tmp1FB?({const char*_tmp21B=", ...)";_tag_dynforward(_tmp21B,
sizeof(char),_get_zero_arr_size(_tmp21B,7));}):({const char*_tmp21C=")";
_tag_dynforward(_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,2));});Cyc_Absyndump_dumpqvar(
_tmp1F9);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp1FA,({const char*_tmp219="(";_tag_dynforward(_tmp219,
sizeof(char),_get_zero_arr_size(_tmp219,2));}),term,({const char*_tmp21A=",";
_tag_dynforward(_tmp21A,sizeof(char),_get_zero_arr_size(_tmp21A,2));}));goto
_LL17C;}_LL19D: if(_tmp1DE <= (void*)2)goto _LL19F;if(*((int*)_tmp1DE)!= 5)goto
_LL19F;_tmp1FC=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f1;_tmp1FD=_tmp1FC.aggr_info;
_tmp1FE=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f2;_tmp1FF=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1DE)->f3;_tmp200=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f4;_LL19E: {
struct _tuple0*_tmp21E;struct _tuple3 _tmp21D=Cyc_Absyn_aggr_kinded_name(_tmp1FD);
_tmp21E=_tmp21D.f2;{struct _dynforward_ptr term=_tmp200?({const char*_tmp224=", ...)";
_tag_dynforward(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,7));}):({const
char*_tmp225=")";_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,
2));});Cyc_Absyndump_dumpqvar(_tmp21E);Cyc_Absyndump_dump_char((int)'{');((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp1FE,({const char*_tmp21F="[";_tag_dynforward(_tmp21F,sizeof(char),
_get_zero_arr_size(_tmp21F,2));}),({const char*_tmp220="]";_tag_dynforward(
_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,2));}),({const char*_tmp221=",";
_tag_dynforward(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,2));}));((void(*)(
void(*f)(struct _tuple11*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp1FF,({const char*_tmp222="";_tag_dynforward(_tmp222,sizeof(char),
_get_zero_arr_size(_tmp222,1));}),term,({const char*_tmp223=",";_tag_dynforward(
_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,2));}));goto _LL17C;}}_LL19F: if(
_tmp1DE <= (void*)2)goto _LL1A1;if(*((int*)_tmp1DE)!= 6)goto _LL1A1;_tmp201=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp1DE)->f2;_tmp202=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1DE)->f3;_tmp203=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1DE)->f4;_LL1A0: {
struct _dynforward_ptr term=_tmp203?({const char*_tmp228=", ...)";_tag_dynforward(
_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,7));}):({const char*_tmp229=")";
_tag_dynforward(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,2));});Cyc_Absyndump_dumpqvar(
_tmp201->name);if(_tmp202 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp202,({const char*_tmp226="(";
_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,2));}),term,({
const char*_tmp227=",";_tag_dynforward(_tmp227,sizeof(char),_get_zero_arr_size(
_tmp227,2));}));goto _LL17C;}_LL1A1: if(_tmp1DE <= (void*)2)goto _LL1A3;if(*((int*)
_tmp1DE)!= 10)goto _LL1A3;_tmp204=((struct Cyc_Absyn_Enum_p_struct*)_tmp1DE)->f2;
_LL1A2: _tmp205=_tmp204;goto _LL1A4;_LL1A3: if(_tmp1DE <= (void*)2)goto _LL1A5;if(*((
int*)_tmp1DE)!= 11)goto _LL1A5;_tmp205=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp1DE)->f2;_LL1A4: Cyc_Absyndump_dumpqvar(_tmp205->name);goto _LL17C;_LL1A5: if(
_tmp1DE <= (void*)2)goto _LL17C;if(*((int*)_tmp1DE)!= 14)goto _LL17C;_tmp206=((
struct Cyc_Absyn_Exp_p_struct*)_tmp1DE)->f1;_LL1A6: Cyc_Absyndump_dumpexp(_tmp206);
goto _LL17C;_LL17C:;}void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _dynforward_ptr sep))
Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,fields,({const char*_tmp22A=",";
_tag_dynforward(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,2));}));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp22B=" = ";_tag_dynforward(_tmp22B,sizeof(char),
_get_zero_arr_size(_tmp22B,4));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*
fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,
struct _dynforward_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,({const char*_tmp22C=",";_tag_dynforward(_tmp22C,sizeof(char),
_get_zero_arr_size(_tmp22C,2));}));}void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*
fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp22E;
struct _dynforward_ptr*_tmp22F;struct Cyc_Absyn_Tqual _tmp230;void*_tmp231;struct
Cyc_Absyn_Exp*_tmp232;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_Aggrfield*
_tmp22D=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp22E=*_tmp22D;_tmp22F=_tmp22E.name;
_tmp230=_tmp22E.tq;_tmp231=(void*)_tmp22E.type;_tmp232=_tmp22E.width;_tmp233=
_tmp22E.attributes;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct
_dynforward_ptr*),struct _dynforward_ptr*))Cyc_Absyndump_dumptqtd)(_tmp230,
_tmp231,Cyc_Absyndump_dump_str,_tmp22F);Cyc_Absyndump_dumpatts(_tmp233);if(
_tmp232 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp232);}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(struct
Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple3*pr){{void*_tmp234=(*pr).f1;_LL1A8: if((int)_tmp234 != 8)goto _LL1AA;
_LL1A9: goto _LL1A7;_LL1AA: if((int)_tmp234 != 0)goto _LL1AC;_LL1AB: Cyc_Absyndump_dump(({
const char*_tmp235="_stdcall";_tag_dynforward(_tmp235,sizeof(char),
_get_zero_arr_size(_tmp235,9));}));goto _LL1A7;_LL1AC: if((int)_tmp234 != 1)goto
_LL1AE;_LL1AD: Cyc_Absyndump_dump(({const char*_tmp236="_cdecl";_tag_dynforward(
_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,7));}));goto _LL1A7;_LL1AE: if((
int)_tmp234 != 2)goto _LL1B0;_LL1AF: Cyc_Absyndump_dump(({const char*_tmp237="_fastcall";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,10));}));goto
_LL1A7;_LL1B0:;_LL1B1: goto _LL1A7;_LL1A7:;}Cyc_Absyndump_dumpqvar((*pr).f2);}
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dumpids(
struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp239;void*_tmp23A;struct _tuple0*_tmp23B;struct Cyc_Absyn_Tqual
_tmp23C;void*_tmp23D;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_List_List*_tmp23F;
struct Cyc_Absyn_Vardecl*_tmp238=vd;_tmp239=*_tmp238;_tmp23A=(void*)_tmp239.sc;
_tmp23B=_tmp239.name;_tmp23C=_tmp239.tq;_tmp23D=(void*)_tmp239.type;_tmp23E=
_tmp239.initializer;_tmp23F=_tmp239.attributes;Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts(_tmp23F);Cyc_Absyndump_dumpscope(_tmp23A);{struct
_RegionHandle _tmp240=_new_region("temp");struct _RegionHandle*temp=& _tmp240;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp242;void*_tmp243;struct Cyc_List_List*
_tmp244;struct _tuple5 _tmp241=Cyc_Absynpp_to_tms(temp,_tmp23C,_tmp23D);_tmp242=
_tmp241.f1;_tmp243=_tmp241.f2;_tmp244=_tmp241.f3;{void*call_conv=(void*)8;{
struct Cyc_List_List*tms2=_tmp244;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp245=(
void*)tms2->hd;struct Cyc_List_List*_tmp246;_LL1B3: if(*((int*)_tmp245)!= 5)goto
_LL1B5;_tmp246=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp245)->f2;_LL1B4: for(
0;_tmp246 != 0;_tmp246=_tmp246->tl){void*_tmp247=(void*)_tmp246->hd;_LL1B8: if((
int)_tmp247 != 0)goto _LL1BA;_LL1B9: call_conv=(void*)0;goto _LL1B7;_LL1BA: if((int)
_tmp247 != 1)goto _LL1BC;_LL1BB: call_conv=(void*)1;goto _LL1B7;_LL1BC: if((int)
_tmp247 != 2)goto _LL1BE;_LL1BD: call_conv=(void*)2;goto _LL1B7;_LL1BE:;_LL1BF: goto
_LL1B7;_LL1B7:;}goto _LL1B2;_LL1B5:;_LL1B6: goto _LL1B2;_LL1B2:;}}Cyc_Absyndump_dumptq(
_tmp242);Cyc_Absyndump_dumpntyp(_tmp243);{struct _tuple3 _tmp248=({struct _tuple3
_tmp249;_tmp249.f1=call_conv;_tmp249.f2=_tmp23B;_tmp249;});((void(*)(struct Cyc_List_List*
tms,void(*f)(struct _tuple3*),struct _tuple3*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(
_tmp244),Cyc_Absyndump_dump_callconv_qvar,& _tmp248);}}};_pop_region(temp);}}
else{if(_tmp23A == (void*)3  && Cyc_Absyndump_qvar_to_Cids){void*_tmp24A=Cyc_Tcutil_compress(
_tmp23D);_LL1C1: if(_tmp24A <= (void*)4)goto _LL1C3;if(*((int*)_tmp24A)!= 8)goto
_LL1C3;_LL1C2: goto _LL1C0;_LL1C3:;_LL1C4: Cyc_Absyndump_dumpscope(_tmp23A);_LL1C0:;}
else{Cyc_Absyndump_dumpscope(_tmp23A);}((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp23C,_tmp23D,
Cyc_Absyndump_dumpqvar,_tmp23B);Cyc_Absyndump_dumpatts(_tmp23F);}if(_tmp23E != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp23E);}Cyc_Absyndump_dump_semi();}struct _tuple12{struct Cyc_Position_Segment*
f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
void*_tmp24B=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp24C;struct Cyc_Absyn_Fndecl*
_tmp24D;struct Cyc_Absyn_Aggrdecl*_tmp24E;struct Cyc_Absyn_Tuniondecl*_tmp24F;
struct Cyc_Absyn_Tuniondecl _tmp250;void*_tmp251;struct _tuple0*_tmp252;struct Cyc_List_List*
_tmp253;struct Cyc_Core_Opt*_tmp254;int _tmp255;int _tmp256;struct Cyc_Absyn_Enumdecl*
_tmp257;struct Cyc_Absyn_Enumdecl _tmp258;void*_tmp259;struct _tuple0*_tmp25A;
struct Cyc_Core_Opt*_tmp25B;struct Cyc_Absyn_Pat*_tmp25C;struct Cyc_Absyn_Exp*
_tmp25D;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_Typedefdecl*_tmp25F;struct
_dynforward_ptr*_tmp260;struct Cyc_List_List*_tmp261;struct _tuple0*_tmp262;struct
Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp265;
struct Cyc_List_List*_tmp266;_LL1C6: if(_tmp24B <= (void*)2)goto _LL1DE;if(*((int*)
_tmp24B)!= 0)goto _LL1C8;_tmp24C=((struct Cyc_Absyn_Var_d_struct*)_tmp24B)->f1;
_LL1C7: Cyc_Absyndump_dumpvardecl(_tmp24C,d->loc);goto _LL1C5;_LL1C8: if(*((int*)
_tmp24B)!= 1)goto _LL1CA;_tmp24D=((struct Cyc_Absyn_Fn_d_struct*)_tmp24B)->f1;
_LL1C9: Cyc_Absyndump_dumploc(d->loc);if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(
_tmp24D->attributes);if(_tmp24D->is_inline){if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(({
const char*_tmp267="__inline";_tag_dynforward(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,9));}));else{Cyc_Absyndump_dump(({const char*_tmp268="inline";
_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,7));}));}}Cyc_Absyndump_dumpscope((
void*)_tmp24D->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp269=
_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Absyn_FnType_struct _tmp26A;
_tmp26A.tag=8;_tmp26A.f1=({struct Cyc_Absyn_FnInfo _tmp26B;_tmp26B.tvars=_tmp24D->tvs;
_tmp26B.effect=_tmp24D->effect;_tmp26B.ret_typ=(void*)((void*)_tmp24D->ret_type);
_tmp26B.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp24D->args);_tmp26B.c_varargs=
_tmp24D->c_varargs;_tmp26B.cyc_varargs=_tmp24D->cyc_varargs;_tmp26B.rgn_po=
_tmp24D->rgn_po;_tmp26B.attributes=0;_tmp26B;});_tmp26A;});_tmp269;});((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_to_VC?Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar,_tmp24D);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp24D->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1C5;}_LL1CA: if(*((int*)
_tmp24B)!= 4)goto _LL1CC;_tmp24E=((struct Cyc_Absyn_Aggr_d_struct*)_tmp24B)->f1;
_LL1CB: Cyc_Absyndump_dumpscope((void*)_tmp24E->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp24E->kind);Cyc_Absyndump_dumpqvar(_tmp24E->name);Cyc_Absyndump_dumptvars(
_tmp24E->tvs);if(_tmp24E->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24E->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24E->impl))->exist_vars,({
const char*_tmp26C="<";_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(
_tmp26C,2));}),({const char*_tmp26D=">";_tag_dynforward(_tmp26D,sizeof(char),
_get_zero_arr_size(_tmp26D,2));}),({const char*_tmp26E=",";_tag_dynforward(
_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,2));}));if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp24E->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24E->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24E->impl))->fields);Cyc_Absyndump_dump(({
const char*_tmp26F="}";_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(
_tmp26F,2));}));Cyc_Absyndump_dumpatts(_tmp24E->attributes);Cyc_Absyndump_dump(({
const char*_tmp270=";";_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(
_tmp270,2));}));}goto _LL1C5;_LL1CC: if(*((int*)_tmp24B)!= 5)goto _LL1CE;_tmp24F=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp24B)->f1;_tmp250=*_tmp24F;_tmp251=(void*)
_tmp250.sc;_tmp252=_tmp250.name;_tmp253=_tmp250.tvs;_tmp254=_tmp250.fields;
_tmp255=_tmp250.is_xtunion;_tmp256=_tmp250.is_flat;_LL1CD: Cyc_Absyndump_dumpscope(
_tmp251);Cyc_Absyndump_dump(_tmp255?({const char*_tmp271="xtunion";
_tag_dynforward(_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,8));}):({const
char*_tmp272="tunion";_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(
_tmp272,7));}));if(_tmp256)Cyc_Absyndump_dump_nospace(({const char*_tmp273=" __attribute__((flat))";
_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,23));}));Cyc_Absyndump_dumpqvar(
_tmp252);Cyc_Absyndump_dumptvars(_tmp253);if(_tmp254 == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)
_tmp254->v);Cyc_Absyndump_dump_nospace(({const char*_tmp274="};";_tag_dynforward(
_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,3));}));}goto _LL1C5;_LL1CE: if(*((
int*)_tmp24B)!= 6)goto _LL1D0;_tmp257=((struct Cyc_Absyn_Enum_d_struct*)_tmp24B)->f1;
_tmp258=*_tmp257;_tmp259=(void*)_tmp258.sc;_tmp25A=_tmp258.name;_tmp25B=_tmp258.fields;
_LL1CF: Cyc_Absyndump_dumpscope(_tmp259);Cyc_Absyndump_dump(({const char*_tmp275="enum ";
_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,6));}));Cyc_Absyndump_dumpqvar(
_tmp25A);if(_tmp25B == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp25B->v);Cyc_Absyndump_dump_nospace(({
const char*_tmp276="};";_tag_dynforward(_tmp276,sizeof(char),_get_zero_arr_size(
_tmp276,3));}));}return;_LL1D0: if(*((int*)_tmp24B)!= 2)goto _LL1D2;_tmp25C=((
struct Cyc_Absyn_Let_d_struct*)_tmp24B)->f1;_tmp25D=((struct Cyc_Absyn_Let_d_struct*)
_tmp24B)->f3;_LL1D1: Cyc_Absyndump_dump(({const char*_tmp277="let";_tag_dynforward(
_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,4));}));Cyc_Absyndump_dumppat(
_tmp25C);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp25D);Cyc_Absyndump_dump_semi();
goto _LL1C5;_LL1D2: if(*((int*)_tmp24B)!= 3)goto _LL1D4;_tmp25E=((struct Cyc_Absyn_Letv_d_struct*)
_tmp24B)->f1;_LL1D3: Cyc_Absyndump_dump(({const char*_tmp278="let ";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,5));}));Cyc_Absyndump_dumpids(
_tmp25E);Cyc_Absyndump_dump_semi();goto _LL1C5;_LL1D4: if(*((int*)_tmp24B)!= 7)
goto _LL1D6;_tmp25F=((struct Cyc_Absyn_Typedef_d_struct*)_tmp24B)->f1;_LL1D5: if(!
Cyc_Absyndump_expand_typedefs  || _tmp25F->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp25F->defn))->v)){Cyc_Absyndump_dump(({
const char*_tmp279="typedef";_tag_dynforward(_tmp279,sizeof(char),
_get_zero_arr_size(_tmp279,8));}));{void*t;if(_tmp25F->defn == 0)t=Cyc_Absyn_new_evar(
_tmp25F->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp25F->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp25F->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp25F);Cyc_Absyndump_dumpatts(_tmp25F->atts);Cyc_Absyndump_dump_semi();}}goto
_LL1C5;_LL1D6: if(*((int*)_tmp24B)!= 8)goto _LL1D8;_tmp260=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp24B)->f1;_tmp261=((struct Cyc_Absyn_Namespace_d_struct*)_tmp24B)->f2;_LL1D7:
Cyc_Absyndump_dump(({const char*_tmp27A="namespace";_tag_dynforward(_tmp27A,
sizeof(char),_get_zero_arr_size(_tmp27A,10));}));Cyc_Absyndump_dump_str(_tmp260);
Cyc_Absyndump_dump_char((int)'{');for(0;_tmp261 != 0;_tmp261=_tmp261->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp261->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C5;
_LL1D8: if(*((int*)_tmp24B)!= 9)goto _LL1DA;_tmp262=((struct Cyc_Absyn_Using_d_struct*)
_tmp24B)->f1;_tmp263=((struct Cyc_Absyn_Using_d_struct*)_tmp24B)->f2;_LL1D9: Cyc_Absyndump_dump(({
const char*_tmp27B="using";_tag_dynforward(_tmp27B,sizeof(char),
_get_zero_arr_size(_tmp27B,6));}));Cyc_Absyndump_dumpqvar(_tmp262);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp263 != 0;_tmp263=_tmp263->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp263->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C5;
_LL1DA: if(*((int*)_tmp24B)!= 10)goto _LL1DC;_tmp264=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp24B)->f1;_LL1DB: Cyc_Absyndump_dump(({const char*_tmp27C="extern \"C\" {";
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,13));}));for(0;
_tmp264 != 0;_tmp264=_tmp264->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp264->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C5;_LL1DC: if(*((int*)
_tmp24B)!= 11)goto _LL1DE;_tmp265=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp24B)->f1;_tmp266=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp24B)->f2;
_LL1DD: Cyc_Absyndump_dump(({const char*_tmp27D="extern \"C include\" {";
_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,21));}));for(0;
_tmp265 != 0;_tmp265=_tmp265->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp265->hd);}Cyc_Absyndump_dump_char((int)'}');if(_tmp266 != 0){Cyc_Absyndump_dump(({
const char*_tmp27E=" export {";_tag_dynforward(_tmp27E,sizeof(char),
_get_zero_arr_size(_tmp27E,10));}));for(0;_tmp266 != 0;_tmp266=_tmp266->tl){
struct _tuple0*_tmp280;struct _tuple12 _tmp27F=*((struct _tuple12*)_tmp266->hd);
_tmp280=_tmp27F.f2;Cyc_Absyndump_dumpqvar(_tmp280);if(_tmp266->tl != 0)Cyc_Absyndump_dump_char((
int)',');}Cyc_Absyndump_dump(({const char*_tmp281="}";_tag_dynforward(_tmp281,
sizeof(char),_get_zero_arr_size(_tmp281,2));}));}goto _LL1C5;_LL1DE: if((int)
_tmp24B != 0)goto _LL1E0;_LL1DF: Cyc_Absyndump_dump(({const char*_tmp282=" __cyclone_port_on__; ";
_tag_dynforward(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,23));}));goto
_LL1C5;_LL1E0: if((int)_tmp24B != 1)goto _LL1C5;_LL1E1: Cyc_Absyndump_dump(({const
char*_tmp283=" __cyclone_port_off__; ";_tag_dynforward(_tmp283,sizeof(char),
_get_zero_arr_size(_tmp283,24));}));goto _LL1C5;_LL1C5:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e){struct _tuple6 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1
!= 1  || !pr.f2){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp284=(void*)tms->hd;struct Cyc_Absyn_PtrAtts
_tmp285;void*_tmp286;struct Cyc_Absyn_Conref*_tmp287;struct Cyc_Absyn_Conref*
_tmp288;struct Cyc_Absyn_Conref*_tmp289;struct Cyc_Absyn_Tqual _tmp28A;_LL1E3: if(*((
int*)_tmp284)!= 2)goto _LL1E5;_tmp285=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp284)->f1;_tmp286=(void*)_tmp285.rgn;_tmp287=_tmp285.nullable;_tmp288=_tmp285.bounds;
_tmp289=_tmp285.zero_term;_tmp28A=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp284)->f2;
_LL1E4:{void*_tmp28B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp288);struct
Cyc_Absyn_Exp*_tmp28C;void*_tmp28D;_LL1E8: if((int)_tmp28B != 0)goto _LL1EA;_LL1E9:
Cyc_Absyndump_dump_char((int)'?');goto _LL1E7;_LL1EA: if((int)_tmp28B != 1)goto
_LL1EC;_LL1EB: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_char((int)'-');
goto _LL1E7;_LL1EC: if(_tmp28B <= (void*)2)goto _LL1EE;if(*((int*)_tmp28B)!= 0)goto
_LL1EE;_tmp28C=((struct Cyc_Absyn_Upper_b_struct*)_tmp28B)->f1;_LL1ED: Cyc_Absyndump_dump_char((
int)(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp287)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp28C);goto _LL1E7;_LL1EE: if(_tmp28B <= (void*)2)
goto _LL1E7;if(*((int*)_tmp28B)!= 1)goto _LL1E7;_tmp28D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp28B)->f1;_LL1EF: Cyc_Absyndump_dump_char((int)(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(1,_tmp287)?'*':'@'));Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumptyp(_tmp28D);Cyc_Absyndump_dump_char((int)'}');_LL1E7:;}if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp289))Cyc_Absyndump_dump(({
const char*_tmp28E="ZEROTERM";_tag_dynforward(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,9));}));{void*_tmp28F=Cyc_Tcutil_compress(_tmp286);
struct Cyc_Absyn_Tvar*_tmp290;struct Cyc_Core_Opt*_tmp291;_LL1F1: if((int)_tmp28F != 
2)goto _LL1F3;_LL1F2: goto _LL1F0;_LL1F3: if(_tmp28F <= (void*)4)goto _LL1F7;if(*((int*)
_tmp28F)!= 1)goto _LL1F5;_tmp290=((struct Cyc_Absyn_VarType_struct*)_tmp28F)->f1;
_LL1F4: Cyc_Absyndump_dump_str(_tmp290->name);goto _LL1F0;_LL1F5: if(*((int*)
_tmp28F)!= 0)goto _LL1F7;_tmp291=((struct Cyc_Absyn_Evar_struct*)_tmp28F)->f2;if(
_tmp291 != 0)goto _LL1F7;_LL1F6: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp286));
goto _LL1F0;_LL1F7:;_LL1F8:({void*_tmp292[0]={};Cyc_Tcutil_impos(({const char*
_tmp293="dumptms: bad rgn type in Pointer_mod";_tag_dynforward(_tmp293,sizeof(
char),_get_zero_arr_size(_tmp293,37));}),_tag_dynforward(_tmp292,sizeof(void*),0));});
_LL1F0:;}Cyc_Absyndump_dumptq(_tmp28A);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1E5:;_LL1E6: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp294=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL1FA: if(*((int*)_tmp294)!= 2)
goto _LL1FC;_LL1FB: next_is_pointer=1;goto _LL1F9;_LL1FC:;_LL1FD: goto _LL1F9;_LL1F9:;}
if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp295=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmp296;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Conref*
_tmp298;void*_tmp299;struct Cyc_List_List*_tmp29A;int _tmp29B;struct Cyc_Absyn_VarargInfo*
_tmp29C;struct Cyc_Core_Opt*_tmp29D;struct Cyc_List_List*_tmp29E;void*_tmp29F;
struct Cyc_List_List*_tmp2A0;struct Cyc_Position_Segment*_tmp2A1;struct Cyc_List_List*
_tmp2A2;struct Cyc_Position_Segment*_tmp2A3;int _tmp2A4;struct Cyc_List_List*
_tmp2A5;_LL1FF: if(*((int*)_tmp295)!= 0)goto _LL201;_tmp296=((struct Cyc_Absyn_Carray_mod_struct*)
_tmp295)->f1;_LL200: Cyc_Absyndump_dump(({const char*_tmp2A6="[]";_tag_dynforward(
_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,3));}));if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp296))Cyc_Absyndump_dump(({const
char*_tmp2A7="ZEROTERM";_tag_dynforward(_tmp2A7,sizeof(char),_get_zero_arr_size(
_tmp2A7,9));}));goto _LL1FE;_LL201: if(*((int*)_tmp295)!= 1)goto _LL203;_tmp297=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmp295)->f1;_tmp298=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp295)->f2;_LL202: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(
_tmp297);Cyc_Absyndump_dump_char((int)']');if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp298))Cyc_Absyndump_dump(({const char*_tmp2A8="ZEROTERM";
_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,9));}));goto
_LL1FE;_LL203: if(*((int*)_tmp295)!= 3)goto _LL205;_tmp299=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp295)->f1;if(*((int*)_tmp299)!= 1)goto _LL205;_tmp29A=((struct Cyc_Absyn_WithTypes_struct*)
_tmp299)->f1;_tmp29B=((struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f2;_tmp29C=((
struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f3;_tmp29D=((struct Cyc_Absyn_WithTypes_struct*)
_tmp299)->f4;_tmp29E=((struct Cyc_Absyn_WithTypes_struct*)_tmp299)->f5;_LL204: Cyc_Absyndump_dumpfunargs(
_tmp29A,_tmp29B,_tmp29C,_tmp29D,_tmp29E);goto _LL1FE;_LL205: if(*((int*)_tmp295)!= 
3)goto _LL207;_tmp29F=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp295)->f1;
if(*((int*)_tmp29F)!= 0)goto _LL207;_tmp2A0=((struct Cyc_Absyn_NoTypes_struct*)
_tmp29F)->f1;_tmp2A1=((struct Cyc_Absyn_NoTypes_struct*)_tmp29F)->f2;_LL206:((
void(*)(void(*f)(struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp2A0,({const char*_tmp2A9="(";_tag_dynforward(_tmp2A9,
sizeof(char),_get_zero_arr_size(_tmp2A9,2));}),({const char*_tmp2AA=")";
_tag_dynforward(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,2));}),({const
char*_tmp2AB=",";_tag_dynforward(_tmp2AB,sizeof(char),_get_zero_arr_size(_tmp2AB,
2));}));goto _LL1FE;_LL207: if(*((int*)_tmp295)!= 4)goto _LL209;_tmp2A2=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp295)->f1;_tmp2A3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp295)->f2;
_tmp2A4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp295)->f3;_LL208: if(_tmp2A4)
Cyc_Absyndump_dumpkindedtvars(_tmp2A2);else{Cyc_Absyndump_dumptvars(_tmp2A2);}
goto _LL1FE;_LL209: if(*((int*)_tmp295)!= 5)goto _LL20B;_tmp2A5=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp295)->f2;_LL20A: Cyc_Absyndump_dumpatts(_tmp2A5);goto _LL1FE;_LL20B: if(*((int*)
_tmp295)!= 2)goto _LL1FE;_LL20C:({void*_tmp2AC[0]={};Cyc_Tcutil_impos(({const char*
_tmp2AD="dumptms";_tag_dynforward(_tmp2AD,sizeof(char),_get_zero_arr_size(
_tmp2AD,8));}),_tag_dynforward(_tmp2AC,sizeof(void*),0));});_LL1FE:;}return;}
_LL1E2:;}}void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(
void*),void*a){struct _RegionHandle _tmp2AE=_new_region("temp");struct
_RegionHandle*temp=& _tmp2AE;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp2B0;
void*_tmp2B1;struct Cyc_List_List*_tmp2B2;struct _tuple5 _tmp2AF=Cyc_Absynpp_to_tms(
temp,tq,t);_tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;_tmp2B2=_tmp2AF.f3;Cyc_Absyndump_dumptq(
_tmp2B0);Cyc_Absyndump_dumpntyp(_tmp2B1);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(
_tmp2B2),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;
tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp2B3[0]={};Cyc_fprintf(f,({const char*_tmp2B4="\n";_tag_dynforward(
_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,2));}),_tag_dynforward(_tmp2B3,
sizeof(void*),0));});}
