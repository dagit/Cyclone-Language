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
16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
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
Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
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
static int Cyc_Absyndump_expand_typedefs;static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix;static int Cyc_Absyndump_generate_line_directives;
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
_LL2D: if((int)_tmp12 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(({const char*_tmp17="UR";
_tag_dynforward(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,3));}));return;
_LL2F: if((int)_tmp12 != 5)goto _LL31;_LL30: Cyc_Absyndump_dump(({const char*_tmp18="TR";
_tag_dynforward(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,3));}));return;
_LL31: if((int)_tmp12 != 6)goto _LL33;_LL32: Cyc_Absyndump_dump(({const char*_tmp19="E";
_tag_dynforward(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,2));}));return;
_LL33: if((int)_tmp12 != 7)goto _LL24;_LL34: Cyc_Absyndump_dump(({const char*_tmp1A="I";
_tag_dynforward(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,2));}));return;
_LL24:;}void Cyc_Absyndump_dumpaggr_kind(void*k){void*_tmp1B=k;_LL36: if((int)
_tmp1B != 0)goto _LL38;_LL37: Cyc_Absyndump_dump(({const char*_tmp1C="struct";
_tag_dynforward(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,7));}));return;
_LL38: if((int)_tmp1B != 1)goto _LL35;_LL39: Cyc_Absyndump_dump(({const char*_tmp1D="union";
_tag_dynforward(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,6));}));return;
_LL35:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumptyp,ts,({const char*_tmp1E="<";_tag_dynforward(_tmp1E,sizeof(
char),_get_zero_arr_size(_tmp1E,2));}),({const char*_tmp1F=">";_tag_dynforward(
_tmp1F,sizeof(char),_get_zero_arr_size(_tmp1F,2));}),({const char*_tmp20=",";
_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,2));}));}void Cyc_Absyndump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp21=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp22;void*_tmp23;_LL3B: if(*((int*)_tmp21)!= 0)goto _LL3D;
_tmp22=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;if((int)_tmp22 != 2)
goto _LL3D;_LL3C: goto _LL3E;_LL3D: if(*((int*)_tmp21)!= 2)goto _LL3F;_LL3E: goto _LL40;
_LL3F: if(*((int*)_tmp21)!= 1)goto _LL41;_LL40: Cyc_Absyndump_dump(({const char*
_tmp24="::?";_tag_dynforward(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,4));}));
goto _LL3A;_LL41: if(*((int*)_tmp21)!= 0)goto _LL3A;_tmp23=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp21)->f1;_LL42: Cyc_Absyndump_dump(({const char*_tmp25="::";_tag_dynforward(
_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,3));}));Cyc_Absyndump_dumpkind(
_tmp23);goto _LL3A;_LL3A:;}}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,({const char*_tmp26="<";_tag_dynforward(_tmp26,sizeof(
char),_get_zero_arr_size(_tmp26,2));}),({const char*_tmp27=">";_tag_dynforward(
_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,2));}),({const char*_tmp28=",";
_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,2));}));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp29="<";
_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,2));}),({const char*
_tmp2A=">";_tag_dynforward(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,2));}),({
const char*_tmp2B=",";_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size(
_tmp2B,2));}));}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple8*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,({const char*_tmp2C="(";
_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,2));}),({const char*
_tmp2D=")";_tag_dynforward(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,2));}),({
const char*_tmp2E=",";_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(
_tmp2E,2));}));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){for(0;
atts != 0;atts=atts->tl){void*_tmp2F=(void*)atts->hd;_LL44: if((int)_tmp2F != 0)
goto _LL46;_LL45: Cyc_Absyndump_dump(({const char*_tmp30="_stdcall";_tag_dynforward(
_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,9));}));return;_LL46: if((int)
_tmp2F != 1)goto _LL48;_LL47: Cyc_Absyndump_dump(({const char*_tmp31="_cdecl";
_tag_dynforward(_tmp31,sizeof(char),_get_zero_arr_size(_tmp31,7));}));return;
_LL48: if((int)_tmp2F != 2)goto _LL4A;_LL49: Cyc_Absyndump_dump(({const char*_tmp32="_fastcall";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,10));}));return;
_LL4A:;_LL4B: goto _LL43;_LL43:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp33=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL4D: if((int)
_tmp33 != 0)goto _LL4F;_LL4E: goto _LL50;_LL4F: if((int)_tmp33 != 1)goto _LL51;_LL50:
goto _LL52;_LL51: if((int)_tmp33 != 2)goto _LL53;_LL52: goto _LL4C;_LL53:;_LL54: hasatt=
1;goto _LL4C;_LL4C:;}}if(!hasatt)return;Cyc_Absyndump_dump(({const char*_tmp34="__declspec(";
_tag_dynforward(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,12));}));for(0;
atts != 0;atts=atts->tl){void*_tmp35=(void*)atts->hd;_LL56: if((int)_tmp35 != 0)
goto _LL58;_LL57: goto _LL59;_LL58: if((int)_tmp35 != 1)goto _LL5A;_LL59: goto _LL5B;
_LL5A: if((int)_tmp35 != 2)goto _LL5C;_LL5B: goto _LL55;_LL5C:;_LL5D: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL55;_LL55:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){if(atts == 0)
return;if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(atts);return;}Cyc_Absyndump_dump(({
const char*_tmp36=" __attribute__((";_tag_dynforward(_tmp36,sizeof(char),
_get_zero_arr_size(_tmp36,17));}));for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(({
const char*_tmp37=",";_tag_dynforward(_tmp37,sizeof(char),_get_zero_arr_size(
_tmp37,2));}));}Cyc_Absyndump_dump(({const char*_tmp38=")) ";_tag_dynforward(
_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,4));}));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp39=(void*)tms->hd;_LL5F:
if(*((int*)_tmp39)!= 2)goto _LL61;_LL60: return 1;_LL61:;_LL62: return 0;_LL5E:;}}
static void Cyc_Absyndump_dumprgn(void*t){void*_tmp3A=Cyc_Tcutil_compress(t);_LL64:
if((int)_tmp3A != 2)goto _LL66;_LL65: Cyc_Absyndump_dump(({const char*_tmp3B="`H";
_tag_dynforward(_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,3));}));goto _LL63;
_LL66:;_LL67: Cyc_Absyndump_dumpntyp(t);goto _LL63;_LL63:;}struct _tuple9{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};static struct _tuple9 Cyc_Absyndump_effects_split(void*
t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp3C=Cyc_Tcutil_compress(
t);void*_tmp3D;struct Cyc_List_List*_tmp3E;_LL69: if(_tmp3C <= (void*)4)goto _LL6D;
if(*((int*)_tmp3C)!= 20)goto _LL6B;_tmp3D=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp3C)->f1;_LL6A: rgions=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));
_tmp3F->hd=(void*)_tmp3D;_tmp3F->tl=rgions;_tmp3F;});goto _LL68;_LL6B: if(*((int*)
_tmp3C)!= 21)goto _LL6D;_tmp3E=((struct Cyc_Absyn_JoinEff_struct*)_tmp3C)->f1;
_LL6C: for(0;_tmp3E != 0;_tmp3E=_tmp3E->tl){struct Cyc_List_List*_tmp41;struct Cyc_List_List*
_tmp42;struct _tuple9 _tmp40=Cyc_Absyndump_effects_split((void*)_tmp3E->hd);_tmp41=
_tmp40.f1;_tmp42=_tmp40.f2;rgions=Cyc_List_imp_append(_tmp41,rgions);effects=Cyc_List_imp_append(
_tmp42,effects);}goto _LL68;_LL6D:;_LL6E: effects=({struct Cyc_List_List*_tmp43=
_cycalloc(sizeof(*_tmp43));_tmp43->hd=(void*)t;_tmp43->tl=effects;_tmp43;});goto
_LL68;_LL68:;}return({struct _tuple9 _tmp44;_tmp44.f1=rgions;_tmp44.f2=effects;
_tmp44;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp46;
struct Cyc_List_List*_tmp47;struct _tuple9 _tmp45=Cyc_Absyndump_effects_split(t);
_tmp46=_tmp45.f1;_tmp47=_tmp45.f2;_tmp46=Cyc_List_imp_rev(_tmp46);_tmp47=Cyc_List_imp_rev(
_tmp47);for(0;_tmp47 != 0;_tmp47=_tmp47->tl){Cyc_Absyndump_dumpntyp((void*)_tmp47->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp46 != 0;_tmp46=_tmp46->tl){Cyc_Absyndump_dumprgn((void*)_tmp46->hd);if(_tmp46->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp48=t;struct Cyc_Absyn_Tvar*_tmp49;struct
Cyc_Core_Opt*_tmp4A;struct Cyc_Core_Opt*_tmp4B;int _tmp4C;struct Cyc_Core_Opt*
_tmp4D;struct Cyc_Core_Opt*_tmp4E;struct Cyc_Core_Opt _tmp4F;void*_tmp50;int _tmp51;
struct Cyc_Absyn_TunionInfo _tmp52;union Cyc_Absyn_TunionInfoU_union _tmp53;struct
Cyc_List_List*_tmp54;struct Cyc_Core_Opt*_tmp55;struct Cyc_Absyn_TunionFieldInfo
_tmp56;union Cyc_Absyn_TunionFieldInfoU_union _tmp57;struct Cyc_List_List*_tmp58;
void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;void*_tmp5D;void*_tmp5E;void*
_tmp5F;void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;
void*_tmp66;void*_tmp67;void*_tmp68;void*_tmp69;void*_tmp6A;void*_tmp6B;void*
_tmp6C;void*_tmp6D;void*_tmp6E;void*_tmp6F;void*_tmp70;void*_tmp71;void*_tmp72;
void*_tmp73;void*_tmp74;void*_tmp75;void*_tmp76;void*_tmp77;void*_tmp78;void*
_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;int _tmp7D;struct Cyc_List_List*_tmp7E;
struct Cyc_Absyn_AggrInfo _tmp7F;union Cyc_Absyn_AggrInfoU_union _tmp80;struct Cyc_List_List*
_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;struct _tuple0*_tmp84;struct Cyc_List_List*
_tmp85;struct _tuple0*_tmp86;struct Cyc_List_List*_tmp87;void*_tmp88;void*_tmp89;
void*_tmp8A;void*_tmp8B;int _tmp8C;void*_tmp8D;_LL70: if(_tmp48 <= (void*)4)goto
_LL76;if(*((int*)_tmp48)!= 7)goto _LL72;_LL71: goto _LL73;_LL72: if(*((int*)_tmp48)
!= 8)goto _LL74;_LL73: goto _LL75;_LL74: if(*((int*)_tmp48)!= 4)goto _LL76;_LL75:
return;_LL76: if((int)_tmp48 != 0)goto _LL78;_LL77: Cyc_Absyndump_dump(({const char*
_tmp8E="void";_tag_dynforward(_tmp8E,sizeof(char),_get_zero_arr_size(_tmp8E,5));}));
return;_LL78: if(_tmp48 <= (void*)4)goto _LLA6;if(*((int*)_tmp48)!= 1)goto _LL7A;
_tmp49=((struct Cyc_Absyn_VarType_struct*)_tmp48)->f1;_LL79: Cyc_Absyndump_dump_str(
_tmp49->name);return;_LL7A: if(*((int*)_tmp48)!= 0)goto _LL7C;_tmp4A=((struct Cyc_Absyn_Evar_struct*)
_tmp48)->f1;_tmp4B=((struct Cyc_Absyn_Evar_struct*)_tmp48)->f2;if(_tmp4B != 0)goto
_LL7C;_tmp4C=((struct Cyc_Absyn_Evar_struct*)_tmp48)->f3;_LL7B: Cyc_Absyndump_dump(({
const char*_tmp8F="%";_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(
_tmp8F,2));}));if(_tmp4A == 0)Cyc_Absyndump_dump(({const char*_tmp90="?";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,2));}));else{Cyc_Absyndump_dumpkind((
void*)_tmp4A->v);}Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp93;_tmp93.tag=1;_tmp93.f1=(unsigned long)_tmp4C;{void*_tmp91[1]={& _tmp93};
Cyc_aprintf(({const char*_tmp92="(%d)";_tag_dynforward(_tmp92,sizeof(char),
_get_zero_arr_size(_tmp92,5));}),_tag_dynforward(_tmp91,sizeof(void*),1));}}));
return;_LL7C: if(*((int*)_tmp48)!= 0)goto _LL7E;_tmp4D=((struct Cyc_Absyn_Evar_struct*)
_tmp48)->f1;_tmp4E=((struct Cyc_Absyn_Evar_struct*)_tmp48)->f2;if(_tmp4E == 0)goto
_LL7E;_tmp4F=*_tmp4E;_tmp50=(void*)_tmp4F.v;_tmp51=((struct Cyc_Absyn_Evar_struct*)
_tmp48)->f3;_LL7D: Cyc_Absyndump_dumpntyp(_tmp50);return;_LL7E: if(*((int*)_tmp48)
!= 2)goto _LL80;_tmp52=((struct Cyc_Absyn_TunionType_struct*)_tmp48)->f1;_tmp53=
_tmp52.tunion_info;_tmp54=_tmp52.targs;_tmp55=_tmp52.rgn;_LL7F:{union Cyc_Absyn_TunionInfoU_union
_tmp94=_tmp53;struct Cyc_Absyn_UnknownTunionInfo _tmp95;struct _tuple0*_tmp96;int
_tmp97;struct Cyc_Absyn_Tuniondecl**_tmp98;struct Cyc_Absyn_Tuniondecl*_tmp99;
struct Cyc_Absyn_Tuniondecl _tmp9A;struct _tuple0*_tmp9B;int _tmp9C;_LLCB: if((_tmp94.UnknownTunion).tag
!= 0)goto _LLCD;_tmp95=(_tmp94.UnknownTunion).f1;_tmp96=_tmp95.name;_tmp97=_tmp95.is_xtunion;
_LLCC: _tmp9B=_tmp96;_tmp9C=_tmp97;goto _LLCE;_LLCD: if((_tmp94.KnownTunion).tag != 
1)goto _LLCA;_tmp98=(_tmp94.KnownTunion).f1;_tmp99=*_tmp98;_tmp9A=*_tmp99;_tmp9B=
_tmp9A.name;_tmp9C=_tmp9A.is_xtunion;_LLCE: if(_tmp9C)Cyc_Absyndump_dump(({const
char*_tmp9D="xtunion ";_tag_dynforward(_tmp9D,sizeof(char),_get_zero_arr_size(
_tmp9D,9));}));else{Cyc_Absyndump_dump(({const char*_tmp9E="tunion ";
_tag_dynforward(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,8));}));}{void*r=(
unsigned int)_tmp55?(void*)_tmp55->v:(void*)2;{void*_tmp9F=Cyc_Tcutil_compress(r);
_LLD0: if((int)_tmp9F != 2)goto _LLD2;_LLD1: goto _LLCF;_LLD2:;_LLD3: Cyc_Absyndump_dumptyp(
r);Cyc_Absyndump_dump(({const char*_tmpA0=" ";_tag_dynforward(_tmpA0,sizeof(char),
_get_zero_arr_size(_tmpA0,2));}));goto _LLCF;_LLCF:;}Cyc_Absyndump_dumpqvar(
_tmp9B);Cyc_Absyndump_dumptps(_tmp54);goto _LLCA;}_LLCA:;}goto _LL6F;_LL80: if(*((
int*)_tmp48)!= 3)goto _LL82;_tmp56=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp48)->f1;_tmp57=_tmp56.field_info;_tmp58=_tmp56.targs;_LL81:{union Cyc_Absyn_TunionFieldInfoU_union
_tmpA1=_tmp57;struct Cyc_Absyn_UnknownTunionFieldInfo _tmpA2;struct _tuple0*_tmpA3;
struct _tuple0*_tmpA4;int _tmpA5;struct Cyc_Absyn_Tuniondecl*_tmpA6;struct Cyc_Absyn_Tuniondecl
_tmpA7;struct _tuple0*_tmpA8;int _tmpA9;struct Cyc_Absyn_Tunionfield*_tmpAA;struct
Cyc_Absyn_Tunionfield _tmpAB;struct _tuple0*_tmpAC;_LLD5: if((_tmpA1.UnknownTunionfield).tag
!= 0)goto _LLD7;_tmpA2=(_tmpA1.UnknownTunionfield).f1;_tmpA3=_tmpA2.tunion_name;
_tmpA4=_tmpA2.field_name;_tmpA5=_tmpA2.is_xtunion;_LLD6: _tmpA8=_tmpA3;_tmpA9=
_tmpA5;_tmpAC=_tmpA4;goto _LLD8;_LLD7: if((_tmpA1.KnownTunionfield).tag != 1)goto
_LLD4;_tmpA6=(_tmpA1.KnownTunionfield).f1;_tmpA7=*_tmpA6;_tmpA8=_tmpA7.name;
_tmpA9=_tmpA7.is_xtunion;_tmpAA=(_tmpA1.KnownTunionfield).f2;_tmpAB=*_tmpAA;
_tmpAC=_tmpAB.name;_LLD8: if(_tmpA9)Cyc_Absyndump_dump(({const char*_tmpAD="xtunion ";
_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,9));}));else{Cyc_Absyndump_dump(({
const char*_tmpAE="tunion ";_tag_dynforward(_tmpAE,sizeof(char),
_get_zero_arr_size(_tmpAE,8));}));}Cyc_Absyndump_dumpqvar(_tmpA8);Cyc_Absyndump_dump(({
const char*_tmpAF=".";_tag_dynforward(_tmpAF,sizeof(char),_get_zero_arr_size(
_tmpAF,2));}));Cyc_Absyndump_dumpqvar(_tmpAC);Cyc_Absyndump_dumptps(_tmp58);goto
_LLD4;_LLD4:;}goto _LL6F;_LL82: if(*((int*)_tmp48)!= 5)goto _LL84;_tmp59=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)_tmp59 != 2)goto _LL84;_tmp5A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;if((int)_tmp5A != 2)goto _LL84;
_LL83: goto _LL85;_LL84: if(*((int*)_tmp48)!= 5)goto _LL86;_tmp5B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f1;if((int)_tmp5B != 0)goto _LL86;_tmp5C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp5C != 2)goto _LL86;_LL85: Cyc_Absyndump_dump(({const char*
_tmpB0="int";_tag_dynforward(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,4));}));
return;_LL86: if(*((int*)_tmp48)!= 5)goto _LL88;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f1;if((int)_tmp5D != 2)goto _LL88;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp5E != 3)goto _LL88;_LL87: goto _LL89;_LL88: if(*((int*)_tmp48)
!= 5)goto _LL8A;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((
int)_tmp5F != 0)goto _LL8A;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;
if((int)_tmp60 != 3)goto _LL8A;_LL89: Cyc_Absyndump_dump(({const char*_tmpB1="long";
_tag_dynforward(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,5));}));return;
_LL8A: if(*((int*)_tmp48)!= 5)goto _LL8C;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f1;if((int)_tmp61 != 2)goto _LL8C;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp62 != 0)goto _LL8C;_LL8B: Cyc_Absyndump_dump(({const char*
_tmpB2="char";_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,5));}));
return;_LL8C: if(*((int*)_tmp48)!= 5)goto _LL8E;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f1;if((int)_tmp63 != 0)goto _LL8E;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp64 != 0)goto _LL8E;_LL8D: Cyc_Absyndump_dump(({const char*
_tmpB3="signed char";_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size(
_tmpB3,12));}));return;_LL8E: if(*((int*)_tmp48)!= 5)goto _LL90;_tmp65=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)_tmp65 != 1)goto _LL90;_tmp66=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;if((int)_tmp66 != 0)goto _LL90;
_LL8F: Cyc_Absyndump_dump(({const char*_tmpB4="unsigned char";_tag_dynforward(
_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,14));}));return;_LL90: if(*((int*)
_tmp48)!= 5)goto _LL92;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;
if((int)_tmp67 != 2)goto _LL92;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp68 != 1)goto _LL92;_LL91: goto _LL93;_LL92: if(*((int*)_tmp48)
!= 5)goto _LL94;_tmp69=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((
int)_tmp69 != 0)goto _LL94;_tmp6A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;
if((int)_tmp6A != 1)goto _LL94;_LL93: Cyc_Absyndump_dump(({const char*_tmpB5="short";
_tag_dynforward(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,6));}));return;
_LL94: if(*((int*)_tmp48)!= 5)goto _LL96;_tmp6B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f1;if((int)_tmp6B != 1)goto _LL96;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp6C != 1)goto _LL96;_LL95: Cyc_Absyndump_dump(({const char*
_tmpB6="unsigned short";_tag_dynforward(_tmpB6,sizeof(char),_get_zero_arr_size(
_tmpB6,15));}));return;_LL96: if(*((int*)_tmp48)!= 5)goto _LL98;_tmp6D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)_tmp6D != 1)goto _LL98;_tmp6E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;if((int)_tmp6E != 2)goto _LL98;
_LL97: Cyc_Absyndump_dump(({const char*_tmpB7="unsigned int";_tag_dynforward(
_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,13));}));return;_LL98: if(*((int*)
_tmp48)!= 5)goto _LL9A;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;
if((int)_tmp6F != 1)goto _LL9A;_tmp70=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp70 != 3)goto _LL9A;_LL99: Cyc_Absyndump_dump(({const char*
_tmpB8="unsigned long";_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size(
_tmpB8,14));}));return;_LL9A: if(*((int*)_tmp48)!= 5)goto _LL9C;_tmp71=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)_tmp71 != 2)goto _LL9C;_tmp72=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;if((int)_tmp72 != 4)goto _LL9C;
if(!Cyc_Absyndump_to_VC)goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp48)!= 5)
goto _LL9E;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)
_tmp73 != 0)goto _LL9E;_tmp74=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;
if((int)_tmp74 != 4)goto _LL9E;if(!Cyc_Absyndump_to_VC)goto _LL9E;_LL9D: Cyc_Absyndump_dump(({
const char*_tmpB9="__int64";_tag_dynforward(_tmpB9,sizeof(char),
_get_zero_arr_size(_tmpB9,8));}));return;_LL9E: if(*((int*)_tmp48)!= 5)goto _LLA0;
_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)_tmp75 != 2)
goto _LLA0;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;if((int)
_tmp76 != 4)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmp48)!= 5)goto _LLA2;
_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((int)_tmp77 != 0)
goto _LLA2;_tmp78=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;if((int)
_tmp78 != 4)goto _LLA2;_LLA1: Cyc_Absyndump_dump(({const char*_tmpBA="long long";
_tag_dynforward(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,10));}));return;
_LLA2: if(*((int*)_tmp48)!= 5)goto _LLA4;_tmp79=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f1;if((int)_tmp79 != 1)goto _LLA4;_tmp7A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48)->f2;if((int)_tmp7A != 4)goto _LLA4;if(!Cyc_Absyndump_to_VC)goto _LLA4;_LLA3:
Cyc_Absyndump_dump(({const char*_tmpBB="unsigned __int64";_tag_dynforward(_tmpBB,
sizeof(char),_get_zero_arr_size(_tmpBB,17));}));return;_LLA4: if(*((int*)_tmp48)
!= 5)goto _LLA6;_tmp7B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f1;if((
int)_tmp7B != 1)goto _LLA6;_tmp7C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48)->f2;
if((int)_tmp7C != 4)goto _LLA6;_LLA5: Cyc_Absyndump_dump(({const char*_tmpBC="unsigned long long";
_tag_dynforward(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,19));}));return;
_LLA6: if((int)_tmp48 != 1)goto _LLA8;_LLA7: Cyc_Absyndump_dump(({const char*_tmpBD="float";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,6));}));return;
_LLA8: if(_tmp48 <= (void*)4)goto _LLC0;if(*((int*)_tmp48)!= 6)goto _LLAA;_tmp7D=((
struct Cyc_Absyn_DoubleType_struct*)_tmp48)->f1;_LLA9: if(_tmp7D)Cyc_Absyndump_dump(({
const char*_tmpBE="long double";_tag_dynforward(_tmpBE,sizeof(char),
_get_zero_arr_size(_tmpBE,12));}));else{Cyc_Absyndump_dump(({const char*_tmpBF="double";
_tag_dynforward(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,7));}));}return;
_LLAA: if(*((int*)_tmp48)!= 9)goto _LLAC;_tmp7E=((struct Cyc_Absyn_TupleType_struct*)
_tmp48)->f1;_LLAB: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp7E);return;_LLAC: if(*((int*)_tmp48)!= 10)goto _LLAE;_tmp7F=((struct Cyc_Absyn_AggrType_struct*)
_tmp48)->f1;_tmp80=_tmp7F.aggr_info;_tmp81=_tmp7F.targs;_LLAD: {void*_tmpC1;
struct _tuple0*_tmpC2;struct _tuple3 _tmpC0=Cyc_Absyn_aggr_kinded_name(_tmp80);
_tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;Cyc_Absyndump_dumpaggr_kind(_tmpC1);Cyc_Absyndump_dumpqvar(
_tmpC2);Cyc_Absyndump_dumptps(_tmp81);return;}_LLAE: if(*((int*)_tmp48)!= 11)goto
_LLB0;_tmp82=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp48)->f1;_tmp83=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp48)->f2;_LLAF: Cyc_Absyndump_dumpaggr_kind(
_tmp82);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp83);
Cyc_Absyndump_dump_char((int)'}');return;_LLB0: if(*((int*)_tmp48)!= 12)goto _LLB2;
_tmp84=((struct Cyc_Absyn_EnumType_struct*)_tmp48)->f1;_LLB1: Cyc_Absyndump_dump(({
const char*_tmpC3="enum ";_tag_dynforward(_tmpC3,sizeof(char),_get_zero_arr_size(
_tmpC3,6));}));Cyc_Absyndump_dumpqvar(_tmp84);return;_LLB2: if(*((int*)_tmp48)!= 
13)goto _LLB4;_tmp85=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp48)->f1;_LLB3: Cyc_Absyndump_dump(({
const char*_tmpC4="enum {";_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size(
_tmpC4,7));}));Cyc_Absyndump_dumpenumfields(_tmp85);Cyc_Absyndump_dump(({const
char*_tmpC5="}";_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,2));}));
return;_LLB4: if(*((int*)_tmp48)!= 17)goto _LLB6;_tmp86=((struct Cyc_Absyn_TypedefType_struct*)
_tmp48)->f1;_tmp87=((struct Cyc_Absyn_TypedefType_struct*)_tmp48)->f2;_LLB5:(Cyc_Absyndump_dumpqvar(
_tmp86),Cyc_Absyndump_dumptps(_tmp87));return;_LLB6: if(*((int*)_tmp48)!= 14)goto
_LLB8;_tmp88=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp48)->f1;_LLB7: Cyc_Absyndump_dump(({
const char*_tmpC6="sizeof_t<";_tag_dynforward(_tmpC6,sizeof(char),
_get_zero_arr_size(_tmpC6,10));}));Cyc_Absyndump_dumpntyp(_tmp88);Cyc_Absyndump_dump(({
const char*_tmpC7=">";_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(
_tmpC7,2));}));return;_LLB8: if(*((int*)_tmp48)!= 15)goto _LLBA;_tmp89=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp48)->f1;_LLB9: Cyc_Absyndump_dump(({
const char*_tmpC8="region_t<";_tag_dynforward(_tmpC8,sizeof(char),
_get_zero_arr_size(_tmpC8,10));}));Cyc_Absyndump_dumprgn(_tmp89);Cyc_Absyndump_dump(({
const char*_tmpC9=">";_tag_dynforward(_tmpC9,sizeof(char),_get_zero_arr_size(
_tmpC9,2));}));return;_LLBA: if(*((int*)_tmp48)!= 16)goto _LLBC;_tmp8A=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp48)->f1;_tmp8B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp48)->f2;_LLBB: Cyc_Absyndump_dump(({const char*_tmpCA="dynregion_t<";
_tag_dynforward(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,13));}));Cyc_Absyndump_dumprgn(
_tmp8A);Cyc_Absyndump_dump(({const char*_tmpCB=",";_tag_dynforward(_tmpCB,sizeof(
char),_get_zero_arr_size(_tmpCB,2));}));Cyc_Absyndump_dumprgn(_tmp8B);Cyc_Absyndump_dump(({
const char*_tmpCC=">";_tag_dynforward(_tmpCC,sizeof(char),_get_zero_arr_size(
_tmpCC,2));}));return;_LLBC: if(*((int*)_tmp48)!= 19)goto _LLBE;_tmp8C=((struct Cyc_Absyn_TypeInt_struct*)
_tmp48)->f1;_LLBD: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpCF;_tmpCF.tag=1;_tmpCF.f1=(unsigned long)_tmp8C;{void*_tmpCD[1]={& _tmpCF};
Cyc_aprintf(({const char*_tmpCE="`%d";_tag_dynforward(_tmpCE,sizeof(char),
_get_zero_arr_size(_tmpCE,4));}),_tag_dynforward(_tmpCD,sizeof(void*),1));}}));
return;_LLBE: if(*((int*)_tmp48)!= 18)goto _LLC0;_tmp8D=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp48)->f1;_LLBF: Cyc_Absyndump_dump(({const char*_tmpD0="tag_t<";_tag_dynforward(
_tmpD0,sizeof(char),_get_zero_arr_size(_tmpD0,7));}));Cyc_Absyndump_dumpntyp(
_tmp8D);Cyc_Absyndump_dump(({const char*_tmpD1=">";_tag_dynforward(_tmpD1,sizeof(
char),_get_zero_arr_size(_tmpD1,2));}));return;_LLC0: if((int)_tmp48 != 3)goto
_LLC2;_LLC1: Cyc_Absyndump_dump(({const char*_tmpD2="`U";_tag_dynforward(_tmpD2,
sizeof(char),_get_zero_arr_size(_tmpD2,3));}));goto _LL6F;_LLC2: if((int)_tmp48 != 
2)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if(_tmp48 <= (void*)4)goto _LLC6;if(*((int*)
_tmp48)!= 20)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if(_tmp48 <= (void*)4)goto _LLC8;if(*((
int*)_tmp48)!= 22)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if(_tmp48 <= (void*)4)goto _LL6F;
if(*((int*)_tmp48)!= 21)goto _LL6F;_LLC9: return;_LL6F:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((struct _dynforward_ptr*)
vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void Cyc_Absyndump_dump_rgncmp(
struct _tuple7*cmp){struct _tuple7 _tmpD4;void*_tmpD5;void*_tmpD6;struct _tuple7*
_tmpD3=cmp;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;Cyc_Absyndump_dumpeff(
_tmpD5);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD6);}void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*
l,struct _dynforward_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,({const char*_tmpD7=",";_tag_dynforward(_tmpD7,sizeof(char),
_get_zero_arr_size(_tmpD7,2));}));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args
!= 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)args->hd);if((args->tl
!= 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}if(
c_varargs)Cyc_Absyndump_dump(({const char*_tmpD8="...";_tag_dynforward(_tmpD8,
sizeof(char),_get_zero_arr_size(_tmpD8,4));}));else{if(cyc_varargs != 0){struct
_tuple1*_tmpD9=({struct _tuple1*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=
cyc_varargs->name;_tmpDC->f2=cyc_varargs->tq;_tmpDC->f3=(void*)cyc_varargs->type;
_tmpDC;});Cyc_Absyndump_dump(({const char*_tmpDA="...";_tag_dynforward(_tmpDA,
sizeof(char),_get_zero_arr_size(_tmpDA,4));}));if(cyc_varargs->inject)Cyc_Absyndump_dump(({
const char*_tmpDB=" inject ";_tag_dynforward(_tmpDB,sizeof(char),
_get_zero_arr_size(_tmpDB,9));}));Cyc_Absyndump_dumpfunarg(_tmpD9);}}if(effopt != 
0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po
!= 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(
int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(void*d){void*
_tmpDD=d;struct Cyc_Absyn_Exp*_tmpDE;struct _dynforward_ptr*_tmpDF;_LLDA: if(*((int*)
_tmpDD)!= 0)goto _LLDC;_tmpDE=((struct Cyc_Absyn_ArrayElement_struct*)_tmpDD)->f1;
_LLDB: Cyc_Absyndump_dump(({const char*_tmpE0=".[";_tag_dynforward(_tmpE0,sizeof(
char),_get_zero_arr_size(_tmpE0,3));}));Cyc_Absyndump_dumpexp(_tmpDE);Cyc_Absyndump_dump_char((
int)']');goto _LLD9;_LLDC: if(*((int*)_tmpDD)!= 1)goto _LLD9;_tmpDF=((struct Cyc_Absyn_FieldName_struct*)
_tmpDD)->f1;_LLDD: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmpDF);goto _LLD9;_LLD9:;}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple10*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*
de).f1,({const char*_tmpE1="";_tag_dynforward(_tmpE1,sizeof(char),
_get_zero_arr_size(_tmpE1,1));}),({const char*_tmpE2="=";_tag_dynforward(_tmpE2,
sizeof(char),_get_zero_arr_size(_tmpE2,2));}),({const char*_tmpE3="=";
_tag_dynforward(_tmpE3,sizeof(char),_get_zero_arr_size(_tmpE3,2));}));Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_tmpE4="";_tag_dynforward(
_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,1));}),({const char*_tmpE5="";
_tag_dynforward(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,1));}),({const char*
_tmpE6=",";_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,2));}));}
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(
e);if(inprec >= myprec)Cyc_Absyndump_dump_nospace(({const char*_tmpE7="(";
_tag_dynforward(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,2));}));{void*
_tmpE8=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpE9;void*_tmpEA;char _tmpEB;union
Cyc_Absyn_Cnst_union _tmpEC;void*_tmpED;short _tmpEE;union Cyc_Absyn_Cnst_union
_tmpEF;void*_tmpF0;int _tmpF1;union Cyc_Absyn_Cnst_union _tmpF2;void*_tmpF3;int
_tmpF4;union Cyc_Absyn_Cnst_union _tmpF5;void*_tmpF6;int _tmpF7;union Cyc_Absyn_Cnst_union
_tmpF8;void*_tmpF9;long long _tmpFA;union Cyc_Absyn_Cnst_union _tmpFB;struct
_dynforward_ptr _tmpFC;union Cyc_Absyn_Cnst_union _tmpFD;union Cyc_Absyn_Cnst_union
_tmpFE;struct _dynforward_ptr _tmpFF;struct _tuple0*_tmp100;struct _tuple0*_tmp101;
void*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Core_Opt*
_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp107;void*_tmp108;
struct Cyc_Absyn_Exp*_tmp109;void*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;void*
_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;void*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;
struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*
_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*
_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*
_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_List_List*
_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*
_tmp11E;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;
struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;void*_tmp124;struct Cyc_Absyn_Exp*
_tmp125;void*_tmp126;void*_tmp127;struct _dynforward_ptr*_tmp128;void*_tmp129;
void*_tmp12A;unsigned int _tmp12B;struct Cyc_List_List*_tmp12C;void*_tmp12D;struct
Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct _dynforward_ptr*_tmp130;
struct Cyc_Absyn_Exp*_tmp131;struct _dynforward_ptr*_tmp132;struct Cyc_Absyn_Exp*
_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;struct _tuple1*
_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Vardecl*
_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13B;struct _tuple0*
_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*
_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Tunionfield*_tmp141;struct
_tuple0*_tmp142;struct _tuple0*_tmp143;struct Cyc_Absyn_MallocInfo _tmp144;int
_tmp145;struct Cyc_Absyn_Exp*_tmp146;void**_tmp147;struct Cyc_Absyn_Exp*_tmp148;
struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Core_Opt*
_tmp14B;struct Cyc_List_List*_tmp14C;struct Cyc_Absyn_Stmt*_tmp14D;_LLDF: if(*((int*)
_tmpE8)!= 0)goto _LLE1;_tmpE9=((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Char_c).tag != 0)goto _LLE1;_tmpEA=(
_tmpE9.Char_c).f1;_tmpEB=(_tmpE9.Char_c).f2;_LLE0: Cyc_Absyndump_dump_char((int)'\'');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmpEB));Cyc_Absyndump_dump_char((
int)'\'');goto _LLDE;_LLE1: if(*((int*)_tmpE8)!= 0)goto _LLE3;_tmpEC=((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Short_c).tag != 1)
goto _LLE3;_tmpED=(_tmpEC.Short_c).f1;_tmpEE=(_tmpEC.Short_c).f2;_LLE2: Cyc_Absyndump_dump((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp150;_tmp150.tag=1;_tmp150.f1=(
unsigned long)((int)_tmpEE);{void*_tmp14E[1]={& _tmp150};Cyc_aprintf(({const char*
_tmp14F="%d";_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,3));}),
_tag_dynforward(_tmp14E,sizeof(void*),1));}}));goto _LLDE;_LLE3: if(*((int*)_tmpE8)
!= 0)goto _LLE5;_tmpEF=((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Int_c).tag != 2)goto _LLE5;_tmpF0=(
_tmpEF.Int_c).f1;if((int)_tmpF0 != 2)goto _LLE5;_tmpF1=(_tmpEF.Int_c).f2;_LLE4:
_tmpF4=_tmpF1;goto _LLE6;_LLE5: if(*((int*)_tmpE8)!= 0)goto _LLE7;_tmpF2=((struct
Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1).Int_c).tag != 2)goto _LLE7;_tmpF3=(_tmpF2.Int_c).f1;if((int)_tmpF3 != 
0)goto _LLE7;_tmpF4=(_tmpF2.Int_c).f2;_LLE6: Cyc_Absyndump_dump((struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp153;_tmp153.tag=1;_tmp153.f1=(
unsigned long)_tmpF4;{void*_tmp151[1]={& _tmp153};Cyc_aprintf(({const char*_tmp152="%d";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,3));}),
_tag_dynforward(_tmp151,sizeof(void*),1));}}));goto _LLDE;_LLE7: if(*((int*)_tmpE8)
!= 0)goto _LLE9;_tmpF5=((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Int_c).tag != 2)goto _LLE9;_tmpF6=(
_tmpF5.Int_c).f1;if((int)_tmpF6 != 1)goto _LLE9;_tmpF7=(_tmpF5.Int_c).f2;_LLE8: Cyc_Absyndump_dump((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp156;_tmp156.tag=1;_tmp156.f1=(
unsigned int)_tmpF7;{void*_tmp154[1]={& _tmp156};Cyc_aprintf(({const char*_tmp155="%u";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,3));}),
_tag_dynforward(_tmp154,sizeof(void*),1));}}));goto _LLDE;_LLE9: if(*((int*)_tmpE8)
!= 0)goto _LLEB;_tmpF8=((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).LongLong_c).tag != 3)goto _LLEB;
_tmpF9=(_tmpF8.LongLong_c).f1;_tmpFA=(_tmpF8.LongLong_c).f2;_LLEA: Cyc_Absyndump_dump(({
const char*_tmp157="<<FIX LONG LONG CONSTANT>>";_tag_dynforward(_tmp157,sizeof(
char),_get_zero_arr_size(_tmp157,27));}));goto _LLDE;_LLEB: if(*((int*)_tmpE8)!= 0)
goto _LLED;_tmpFB=((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1).Float_c).tag != 4)goto _LLED;_tmpFC=(_tmpFB.Float_c).f1;_LLEC: Cyc_Absyndump_dump(
_tmpFC);goto _LLDE;_LLED: if(*((int*)_tmpE8)!= 0)goto _LLEF;_tmpFD=((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Null_c).tag != 6)
goto _LLEF;_LLEE: Cyc_Absyndump_dump(({const char*_tmp158="NULL";_tag_dynforward(
_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,5));}));goto _LLDE;_LLEF: if(*((
int*)_tmpE8)!= 0)goto _LLF1;_tmpFE=((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).String_c).tag != 5)goto _LLF1;
_tmpFF=(_tmpFE.String_c).f1;_LLF0: Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape(_tmpFF));Cyc_Absyndump_dump_char((int)'"');goto _LLDE;
_LLF1: if(*((int*)_tmpE8)!= 2)goto _LLF3;_tmp100=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpE8)->f1;_LLF2: _tmp101=_tmp100;goto _LLF4;_LLF3: if(*((int*)_tmpE8)!= 1)goto
_LLF5;_tmp101=((struct Cyc_Absyn_Var_e_struct*)_tmpE8)->f1;_LLF4: Cyc_Absyndump_dumpqvar(
_tmp101);goto _LLDE;_LLF5: if(*((int*)_tmpE8)!= 3)goto _LLF7;_tmp102=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpE8)->f1;_tmp103=((struct Cyc_Absyn_Primop_e_struct*)
_tmpE8)->f2;_LLF6: {struct _dynforward_ptr _tmp159=Cyc_Absynpp_prim2str(_tmp102);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp103)){case 1: _LL145:
if(_tmp102 == (void*)((void*)19)){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp103))->hd);Cyc_Absyndump_dump(({const char*
_tmp15A=".size";_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,
6));}));}else{Cyc_Absyndump_dump(_tmp159);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp103))->hd);}break;
case 2: _LL146: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp103))->hd);Cyc_Absyndump_dump(_tmp159);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp103->tl))->hd);break;default: _LL147:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct
Cyc_Core_Failure_struct _tmp15C;_tmp15C.tag=Cyc_Core_Failure;_tmp15C.f1=({const
char*_tmp15D="Absyndump -- Bad number of arguments to primop";_tag_dynforward(
_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,47));});_tmp15C;});_tmp15B;}));}
goto _LLDE;}_LLF7: if(*((int*)_tmpE8)!= 4)goto _LLF9;_tmp104=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpE8)->f1;_tmp105=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f2;_tmp106=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f3;_LLF8: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp104);if(_tmp105 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp105->v));Cyc_Absyndump_dump_nospace(({const char*_tmp15E="=";_tag_dynforward(
_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,2));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp106);goto _LLDE;_LLF9: if(*((int*)_tmpE8)!= 5)goto _LLFB;_tmp107=((
struct Cyc_Absyn_Increment_e_struct*)_tmpE8)->f1;_tmp108=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpE8)->f2;if((int)_tmp108 != 0)goto _LLFB;_LLFA: Cyc_Absyndump_dump(({const char*
_tmp15F="++";_tag_dynforward(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,3));}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp107);goto _LLDE;_LLFB: if(*((int*)_tmpE8)!= 
5)goto _LLFD;_tmp109=((struct Cyc_Absyn_Increment_e_struct*)_tmpE8)->f1;_tmp10A=(
void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE8)->f2;if((int)_tmp10A != 2)goto
_LLFD;_LLFC: Cyc_Absyndump_dump(({const char*_tmp160="--";_tag_dynforward(_tmp160,
sizeof(char),_get_zero_arr_size(_tmp160,3));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp109);goto _LLDE;_LLFD: if(*((int*)_tmpE8)!= 5)goto _LLFF;_tmp10B=((
struct Cyc_Absyn_Increment_e_struct*)_tmpE8)->f1;_tmp10C=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpE8)->f2;if((int)_tmp10C != 1)goto _LLFF;_LLFE: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp10B);Cyc_Absyndump_dump(({const char*_tmp161="++";_tag_dynforward(
_tmp161,sizeof(char),_get_zero_arr_size(_tmp161,3));}));goto _LLDE;_LLFF: if(*((
int*)_tmpE8)!= 5)goto _LL101;_tmp10D=((struct Cyc_Absyn_Increment_e_struct*)_tmpE8)->f1;
_tmp10E=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE8)->f2;if((int)_tmp10E
!= 3)goto _LL101;_LL100: Cyc_Absyndump_dumpexp_prec(myprec,_tmp10D);Cyc_Absyndump_dump(({
const char*_tmp162="--";_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(
_tmp162,3));}));goto _LLDE;_LL101: if(*((int*)_tmpE8)!= 6)goto _LL103;_tmp10F=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpE8)->f1;_tmp110=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE8)->f2;_tmp111=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE8)->f3;_LL102:
Cyc_Absyndump_dumpexp_prec(myprec,_tmp10F);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(
0,_tmp110);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp111);goto _LLDE;_LL103: if(*((int*)_tmpE8)!= 7)goto _LL105;_tmp112=((struct Cyc_Absyn_And_e_struct*)
_tmpE8)->f1;_tmp113=((struct Cyc_Absyn_And_e_struct*)_tmpE8)->f2;_LL104: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp112);Cyc_Absyndump_dump(({const char*_tmp163=" && ";_tag_dynforward(
_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,5));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp113);goto _LLDE;_LL105: if(*((int*)_tmpE8)!= 8)goto _LL107;_tmp114=((
struct Cyc_Absyn_Or_e_struct*)_tmpE8)->f1;_tmp115=((struct Cyc_Absyn_Or_e_struct*)
_tmpE8)->f2;_LL106: Cyc_Absyndump_dumpexp_prec(myprec,_tmp114);Cyc_Absyndump_dump(({
const char*_tmp164=" || ";_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size(
_tmp164,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp115);goto _LLDE;_LL107: if(*((
int*)_tmpE8)!= 9)goto _LL109;_tmp116=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE8)->f1;
_tmp117=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE8)->f2;_LL108: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp116);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp117);Cyc_Absyndump_dump_char((int)')');
goto _LLDE;_LL109: if(*((int*)_tmpE8)!= 10)goto _LL10B;_tmp118=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpE8)->f1;_tmp119=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpE8)->f2;_LL10A:
_tmp11A=_tmp118;_tmp11B=_tmp119;goto _LL10C;_LL10B: if(*((int*)_tmpE8)!= 11)goto
_LL10D;_tmp11A=((struct Cyc_Absyn_FnCall_e_struct*)_tmpE8)->f1;_tmp11B=((struct
Cyc_Absyn_FnCall_e_struct*)_tmpE8)->f2;_LL10C: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp11A);Cyc_Absyndump_dump_nospace(({const char*_tmp165="(";_tag_dynforward(
_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,2));}));Cyc_Absyndump_dumpexps_prec(
20,_tmp11B);Cyc_Absyndump_dump_nospace(({const char*_tmp166=")";_tag_dynforward(
_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,2));}));goto _LLDE;_LL10D: if(*((
int*)_tmpE8)!= 12)goto _LL10F;_tmp11C=((struct Cyc_Absyn_Throw_e_struct*)_tmpE8)->f1;
_LL10E: Cyc_Absyndump_dump(({const char*_tmp167="throw";_tag_dynforward(_tmp167,
sizeof(char),_get_zero_arr_size(_tmp167,6));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11C);goto _LLDE;_LL10F: if(*((int*)_tmpE8)!= 13)goto _LL111;_tmp11D=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE8)->f1;_LL110: _tmp11E=_tmp11D;goto
_LL112;_LL111: if(*((int*)_tmpE8)!= 14)goto _LL113;_tmp11E=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpE8)->f1;_LL112: Cyc_Absyndump_dumpexp_prec(inprec,_tmp11E);goto _LLDE;_LL113:
if(*((int*)_tmpE8)!= 15)goto _LL115;_tmp11F=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpE8)->f1;_tmp120=((struct Cyc_Absyn_Cast_e_struct*)_tmpE8)->f2;_LL114: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp11F);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp120);goto _LLDE;_LL115: if(*((int*)_tmpE8)!= 16)goto _LL117;_tmp121=((
struct Cyc_Absyn_Address_e_struct*)_tmpE8)->f1;_LL116: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp121);goto _LLDE;_LL117: if(*((int*)
_tmpE8)!= 17)goto _LL119;_tmp122=((struct Cyc_Absyn_New_e_struct*)_tmpE8)->f1;
_tmp123=((struct Cyc_Absyn_New_e_struct*)_tmpE8)->f2;_LL118: Cyc_Absyndump_dump(({
const char*_tmp168="new ";_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(
_tmp168,5));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp123);goto _LLDE;_LL119: if(*((
int*)_tmpE8)!= 18)goto _LL11B;_tmp124=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpE8)->f1;_LL11A: Cyc_Absyndump_dump(({const char*_tmp169="sizeof(";
_tag_dynforward(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,8));}));Cyc_Absyndump_dumptyp(
_tmp124);Cyc_Absyndump_dump_char((int)')');goto _LLDE;_LL11B: if(*((int*)_tmpE8)!= 
19)goto _LL11D;_tmp125=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpE8)->f1;_LL11C:
Cyc_Absyndump_dump(({const char*_tmp16A="sizeof(";_tag_dynforward(_tmp16A,sizeof(
char),_get_zero_arr_size(_tmp16A,8));}));Cyc_Absyndump_dumpexp(_tmp125);Cyc_Absyndump_dump_char((
int)')');goto _LLDE;_LL11D: if(*((int*)_tmpE8)!= 20)goto _LL11F;_tmp126=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmpE8)->f1;_tmp127=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpE8)->f2;if(*((int*)_tmp127)!= 0)goto _LL11F;_tmp128=((struct Cyc_Absyn_StructField_struct*)
_tmp127)->f1;_LL11E: Cyc_Absyndump_dump(({const char*_tmp16B="offsetof(";
_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,10));}));Cyc_Absyndump_dumptyp(
_tmp126);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp128);
Cyc_Absyndump_dump_char((int)')');goto _LLDE;_LL11F: if(*((int*)_tmpE8)!= 20)goto
_LL121;_tmp129=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE8)->f1;_tmp12A=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE8)->f2;if(*((int*)_tmp12A)!= 1)
goto _LL121;_tmp12B=((struct Cyc_Absyn_TupleIndex_struct*)_tmp12A)->f1;_LL120: Cyc_Absyndump_dump(({
const char*_tmp16C="offsetof(";_tag_dynforward(_tmp16C,sizeof(char),
_get_zero_arr_size(_tmp16C,10));}));Cyc_Absyndump_dumptyp(_tmp129);Cyc_Absyndump_dump_char((
int)',');Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp16F;_tmp16F.tag=1;_tmp16F.f1=(unsigned long)((int)_tmp12B);{void*_tmp16D[1]={&
_tmp16F};Cyc_aprintf(({const char*_tmp16E="%d";_tag_dynforward(_tmp16E,sizeof(
char),_get_zero_arr_size(_tmp16E,3));}),_tag_dynforward(_tmp16D,sizeof(void*),1));}}));
Cyc_Absyndump_dump_char((int)')');goto _LLDE;_LL121: if(*((int*)_tmpE8)!= 21)goto
_LL123;_tmp12C=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpE8)->f1;_tmp12D=(void*)((
struct Cyc_Absyn_Gentyp_e_struct*)_tmpE8)->f2;_LL122: Cyc_Absyndump_dump(({const
char*_tmp170="__gen(";_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,7));}));Cyc_Absyndump_dumptvars(_tmp12C);Cyc_Absyndump_dumptyp(_tmp12D);
Cyc_Absyndump_dump_char((int)')');goto _LLDE;_LL123: if(*((int*)_tmpE8)!= 22)goto
_LL125;_tmp12E=((struct Cyc_Absyn_Deref_e_struct*)_tmpE8)->f1;_LL124: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp12E);goto _LLDE;_LL125: if(*((int*)
_tmpE8)!= 23)goto _LL127;_tmp12F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE8)->f1;
_tmp130=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE8)->f2;_LL126: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp12F);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp130);goto _LLDE;_LL127: if(*((int*)_tmpE8)!= 24)goto _LL129;_tmp131=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE8)->f1;_tmp132=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpE8)->f2;_LL128: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp131);Cyc_Absyndump_dump_nospace(({const char*_tmp171="->";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,3));}));Cyc_Absyndump_dump_nospace(*
_tmp132);goto _LLDE;_LL129: if(*((int*)_tmpE8)!= 25)goto _LL12B;_tmp133=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpE8)->f1;_tmp134=((struct Cyc_Absyn_Subscript_e_struct*)_tmpE8)->f2;_LL12A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp133);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp134);
Cyc_Absyndump_dump_char((int)']');goto _LLDE;_LL12B: if(*((int*)_tmpE8)!= 26)goto
_LL12D;_tmp135=((struct Cyc_Absyn_Tuple_e_struct*)_tmpE8)->f1;_LL12C: Cyc_Absyndump_dump(({
const char*_tmp172="$(";_tag_dynforward(_tmp172,sizeof(char),_get_zero_arr_size(
_tmp172,3));}));Cyc_Absyndump_dumpexps_prec(20,_tmp135);Cyc_Absyndump_dump_char((
int)')');goto _LLDE;_LL12D: if(*((int*)_tmpE8)!= 27)goto _LL12F;_tmp136=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpE8)->f1;_tmp137=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpE8)->f2;_LL12E:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*_tmp136).f3);Cyc_Absyndump_dump_char((
int)')');((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp137,({const char*_tmp173="{";_tag_dynforward(_tmp173,
sizeof(char),_get_zero_arr_size(_tmp173,2));}),({const char*_tmp174="}";
_tag_dynforward(_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,2));}),({const
char*_tmp175=",";_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,
2));}));goto _LLDE;_LL12F: if(*((int*)_tmpE8)!= 28)goto _LL131;_tmp138=((struct Cyc_Absyn_Array_e_struct*)
_tmpE8)->f1;_LL130:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp138,({const char*_tmp176="{";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,2));}),({const
char*_tmp177="}";_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,
2));}),({const char*_tmp178=",";_tag_dynforward(_tmp178,sizeof(char),
_get_zero_arr_size(_tmp178,2));}));goto _LLDE;_LL131: if(*((int*)_tmpE8)!= 29)goto
_LL133;_tmp139=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE8)->f1;_tmp13A=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpE8)->f2;_tmp13B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpE8)->f3;_LL132: Cyc_Absyndump_dump(({const char*_tmp179="new {for";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,9));}));Cyc_Absyndump_dump_str((*
_tmp139->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(
_tmp13A);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp13B);Cyc_Absyndump_dump_char((
int)'}');goto _LLDE;_LL133: if(*((int*)_tmpE8)!= 30)goto _LL135;_tmp13C=((struct Cyc_Absyn_Struct_e_struct*)
_tmpE8)->f1;_tmp13D=((struct Cyc_Absyn_Struct_e_struct*)_tmpE8)->f2;_tmp13E=((
struct Cyc_Absyn_Struct_e_struct*)_tmpE8)->f3;_LL134: Cyc_Absyndump_dumpqvar(
_tmp13C);Cyc_Absyndump_dump_char((int)'{');if(_tmp13D != 0)Cyc_Absyndump_dumptps(
_tmp13D);((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp13E,({const char*_tmp17A="";_tag_dynforward(_tmp17A,
sizeof(char),_get_zero_arr_size(_tmp17A,1));}),({const char*_tmp17B="}";
_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size(_tmp17B,2));}),({const
char*_tmp17C=",";_tag_dynforward(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,
2));}));goto _LLDE;_LL135: if(*((int*)_tmpE8)!= 31)goto _LL137;_tmp13F=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpE8)->f2;_LL136:((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp13F,({const char*_tmp17D="{";
_tag_dynforward(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,2));}),({const
char*_tmp17E="}";_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,
2));}),({const char*_tmp17F=",";_tag_dynforward(_tmp17F,sizeof(char),
_get_zero_arr_size(_tmp17F,2));}));goto _LLDE;_LL137: if(*((int*)_tmpE8)!= 32)goto
_LL139;_tmp140=((struct Cyc_Absyn_Tunion_e_struct*)_tmpE8)->f1;_tmp141=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpE8)->f3;_LL138: Cyc_Absyndump_dumpqvar(_tmp141->name);
if(_tmp140 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp140,({const char*_tmp180="(";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,2));}),({const
char*_tmp181=")";_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,
2));}),({const char*_tmp182=",";_tag_dynforward(_tmp182,sizeof(char),
_get_zero_arr_size(_tmp182,2));}));goto _LLDE;_LL139: if(*((int*)_tmpE8)!= 33)goto
_LL13B;_tmp142=((struct Cyc_Absyn_Enum_e_struct*)_tmpE8)->f1;_LL13A: Cyc_Absyndump_dumpqvar(
_tmp142);goto _LLDE;_LL13B: if(*((int*)_tmpE8)!= 34)goto _LL13D;_tmp143=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpE8)->f1;_LL13C: Cyc_Absyndump_dumpqvar(_tmp143);goto _LLDE;_LL13D: if(*((int*)
_tmpE8)!= 35)goto _LL13F;_tmp144=((struct Cyc_Absyn_Malloc_e_struct*)_tmpE8)->f1;
_tmp145=_tmp144.is_calloc;_tmp146=_tmp144.rgn;_tmp147=_tmp144.elt_type;_tmp148=
_tmp144.num_elts;_LL13E: if(_tmp145){if(_tmp146 != 0){Cyc_Absyndump_dump(({const
char*_tmp183="rcalloc(";_tag_dynforward(_tmp183,sizeof(char),_get_zero_arr_size(
_tmp183,9));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp146);Cyc_Absyndump_dump(({
const char*_tmp184=",";_tag_dynforward(_tmp184,sizeof(char),_get_zero_arr_size(
_tmp184,2));}));}else{Cyc_Absyndump_dump(({const char*_tmp185="calloc";
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,7));}));}Cyc_Absyndump_dumpexp(
_tmp148);Cyc_Absyndump_dump(({const char*_tmp186=",";_tag_dynforward(_tmp186,
sizeof(char),_get_zero_arr_size(_tmp186,2));}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp147)),0));Cyc_Absyndump_dump(({const char*_tmp187=")";
_tag_dynforward(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,2));}));}else{
if(_tmp146 != 0){Cyc_Absyndump_dump(({const char*_tmp188="rmalloc(";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp146);Cyc_Absyndump_dump(({const char*_tmp189=",";
_tag_dynforward(_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,2));}));}else{
Cyc_Absyndump_dump(({const char*_tmp18A="malloc(";_tag_dynforward(_tmp18A,sizeof(
char),_get_zero_arr_size(_tmp18A,8));}));}if(_tmp147 != 0)Cyc_Absyndump_dumpexp(
Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp147,0),_tmp148,0));else{Cyc_Absyndump_dumpexp(
_tmp148);}Cyc_Absyndump_dump(({const char*_tmp18B=")";_tag_dynforward(_tmp18B,
sizeof(char),_get_zero_arr_size(_tmp18B,2));}));}goto _LLDE;_LL13F: if(*((int*)
_tmpE8)!= 36)goto _LL141;_tmp149=((struct Cyc_Absyn_Swap_e_struct*)_tmpE8)->f1;
_tmp14A=((struct Cyc_Absyn_Swap_e_struct*)_tmpE8)->f2;_LL140: Cyc_Absyndump_dump(({
const char*_tmp18C="cycswap(";_tag_dynforward(_tmp18C,sizeof(char),
_get_zero_arr_size(_tmp18C,9));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp149);
Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp14A);Cyc_Absyndump_dump_char((
int)')');goto _LLDE;_LL141: if(*((int*)_tmpE8)!= 37)goto _LL143;_tmp14B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpE8)->f1;_tmp14C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE8)->f2;_LL142:((
void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _dynforward_ptr
start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp14C,({const char*_tmp18D="{";_tag_dynforward(_tmp18D,
sizeof(char),_get_zero_arr_size(_tmp18D,2));}),({const char*_tmp18E="}";
_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,2));}),({const
char*_tmp18F=",";_tag_dynforward(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,
2));}));goto _LLDE;_LL143: if(*((int*)_tmpE8)!= 38)goto _LLDE;_tmp14D=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpE8)->f1;_LL144: Cyc_Absyndump_dump_nospace(({const char*_tmp190="({";
_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,3));}));Cyc_Absyndump_dumpstmt(
_tmp14D);Cyc_Absyndump_dump_nospace(({const char*_tmp191="})";_tag_dynforward(
_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,3));}));goto _LLDE;_LLDE:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp192=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp192->where_clause == 0  && (
void*)(_tmp192->pattern)->r == (void*)((void*)0))Cyc_Absyndump_dump(({const char*
_tmp193="default:";_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(
_tmp193,9));}));else{Cyc_Absyndump_dump(({const char*_tmp194="case";
_tag_dynforward(_tmp194,sizeof(char),_get_zero_arr_size(_tmp194,5));}));Cyc_Absyndump_dumppat(
_tmp192->pattern);if(_tmp192->where_clause != 0){Cyc_Absyndump_dump(({const char*
_tmp195="&&";_tag_dynforward(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,3));}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp192->where_clause));}
Cyc_Absyndump_dump_nospace(({const char*_tmp196=":";_tag_dynforward(_tmp196,
sizeof(char),_get_zero_arr_size(_tmp196,2));}));}Cyc_Absyndump_dumpstmt(_tmp192->body);}}
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmp197=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Stmt*_tmp199;struct Cyc_Absyn_Stmt*
_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*
_tmp19D;struct Cyc_Absyn_Stmt*_tmp19E;struct Cyc_Absyn_Stmt*_tmp19F;struct _tuple2
_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Stmt*_tmp1A2;struct
_dynforward_ptr*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct _tuple2 _tmp1A5;struct
Cyc_Absyn_Exp*_tmp1A6;struct _tuple2 _tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Stmt*
_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_Absyn_Decl*
_tmp1AC;struct Cyc_Absyn_Stmt*_tmp1AD;struct _dynforward_ptr*_tmp1AE;struct Cyc_Absyn_Stmt*
_tmp1AF;struct Cyc_Absyn_Stmt*_tmp1B0;struct _tuple2 _tmp1B1;struct Cyc_Absyn_Exp*
_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Stmt*
_tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_Absyn_Tvar*_tmp1B7;struct Cyc_Absyn_Vardecl*
_tmp1B8;int _tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Stmt*_tmp1BB;
struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Tvar*
_tmp1BE;struct Cyc_Absyn_Vardecl*_tmp1BF;struct Cyc_Absyn_Stmt*_tmp1C0;_LL14A: if((
int)_tmp197 != 0)goto _LL14C;_LL14B: Cyc_Absyndump_dump_semi();goto _LL149;_LL14C:
if(_tmp197 <= (void*)1)goto _LL14E;if(*((int*)_tmp197)!= 0)goto _LL14E;_tmp198=((
struct Cyc_Absyn_Exp_s_struct*)_tmp197)->f1;_LL14D: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dumpexp(_tmp198);Cyc_Absyndump_dump_semi();goto _LL149;_LL14E: if(
_tmp197 <= (void*)1)goto _LL150;if(*((int*)_tmp197)!= 1)goto _LL150;_tmp199=((
struct Cyc_Absyn_Seq_s_struct*)_tmp197)->f1;_tmp19A=((struct Cyc_Absyn_Seq_s_struct*)
_tmp197)->f2;_LL14F: if(Cyc_Absynpp_is_declaration(_tmp199)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp199);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dumpstmt(_tmp199);}if(Cyc_Absynpp_is_declaration(_tmp19A)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp19A);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp19A);}goto _LL149;_LL150: if(_tmp197 <= (
void*)1)goto _LL152;if(*((int*)_tmp197)!= 2)goto _LL152;_tmp19B=((struct Cyc_Absyn_Return_s_struct*)
_tmp197)->f1;if(_tmp19B != 0)goto _LL152;_LL151: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1C1="return;";_tag_dynforward(_tmp1C1,sizeof(char),
_get_zero_arr_size(_tmp1C1,8));}));goto _LL149;_LL152: if(_tmp197 <= (void*)1)goto
_LL154;if(*((int*)_tmp197)!= 2)goto _LL154;_tmp19C=((struct Cyc_Absyn_Return_s_struct*)
_tmp197)->f1;_LL153: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1C2="return";_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,
7));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp19C));Cyc_Absyndump_dump_semi();
goto _LL149;_LL154: if(_tmp197 <= (void*)1)goto _LL156;if(*((int*)_tmp197)!= 3)goto
_LL156;_tmp19D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp197)->f1;_tmp19E=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp197)->f2;_tmp19F=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp197)->f3;_LL155: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1C3="if(";_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,4));}));
Cyc_Absyndump_dumpexp(_tmp19D);{void*_tmp1C4=(void*)_tmp19E->r;_LL175: if(_tmp1C4
<= (void*)1)goto _LL17D;if(*((int*)_tmp1C4)!= 1)goto _LL177;_LL176: goto _LL178;
_LL177: if(*((int*)_tmp1C4)!= 11)goto _LL179;_LL178: goto _LL17A;_LL179: if(*((int*)
_tmp1C4)!= 3)goto _LL17B;_LL17A: goto _LL17C;_LL17B: if(*((int*)_tmp1C4)!= 12)goto
_LL17D;_LL17C: Cyc_Absyndump_dump_nospace(({const char*_tmp1C5="){";
_tag_dynforward(_tmp1C5,sizeof(char),_get_zero_arr_size(_tmp1C5,3));}));Cyc_Absyndump_dumpstmt(
_tmp19E);Cyc_Absyndump_dump_char((int)'}');goto _LL174;_LL17D:;_LL17E: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmp19E);_LL174:;}{void*_tmp1C6=(void*)_tmp19F->r;
_LL180: if((int)_tmp1C6 != 0)goto _LL182;_LL181: goto _LL17F;_LL182:;_LL183: Cyc_Absyndump_dump(({
const char*_tmp1C7="else{";_tag_dynforward(_tmp1C7,sizeof(char),
_get_zero_arr_size(_tmp1C7,6));}));Cyc_Absyndump_dumpstmt(_tmp19F);Cyc_Absyndump_dump_char((
int)'}');goto _LL17F;_LL17F:;}goto _LL149;_LL156: if(_tmp197 <= (void*)1)goto _LL158;
if(*((int*)_tmp197)!= 4)goto _LL158;_tmp1A0=((struct Cyc_Absyn_While_s_struct*)
_tmp197)->f1;_tmp1A1=_tmp1A0.f1;_tmp1A2=((struct Cyc_Absyn_While_s_struct*)
_tmp197)->f2;_LL157: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1C8="while(";_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,
7));}));Cyc_Absyndump_dumpexp(_tmp1A1);Cyc_Absyndump_dump_nospace(({const char*
_tmp1C9="){";_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,3));}));
Cyc_Absyndump_dumpstmt(_tmp1A2);Cyc_Absyndump_dump_char((int)'}');goto _LL149;
_LL158: if(_tmp197 <= (void*)1)goto _LL15A;if(*((int*)_tmp197)!= 5)goto _LL15A;
_LL159: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1CA="break;";
_tag_dynforward(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,7));}));goto
_LL149;_LL15A: if(_tmp197 <= (void*)1)goto _LL15C;if(*((int*)_tmp197)!= 6)goto
_LL15C;_LL15B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1CB="continue;";_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size(
_tmp1CB,10));}));goto _LL149;_LL15C: if(_tmp197 <= (void*)1)goto _LL15E;if(*((int*)
_tmp197)!= 7)goto _LL15E;_tmp1A3=((struct Cyc_Absyn_Goto_s_struct*)_tmp197)->f1;
_LL15D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1CC="goto";
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,5));}));Cyc_Absyndump_dump_str(
_tmp1A3);Cyc_Absyndump_dump_semi();goto _LL149;_LL15E: if(_tmp197 <= (void*)1)goto
_LL160;if(*((int*)_tmp197)!= 8)goto _LL160;_tmp1A4=((struct Cyc_Absyn_For_s_struct*)
_tmp197)->f1;_tmp1A5=((struct Cyc_Absyn_For_s_struct*)_tmp197)->f2;_tmp1A6=
_tmp1A5.f1;_tmp1A7=((struct Cyc_Absyn_For_s_struct*)_tmp197)->f3;_tmp1A8=_tmp1A7.f1;
_tmp1A9=((struct Cyc_Absyn_For_s_struct*)_tmp197)->f4;_LL15F: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1CD="for(";_tag_dynforward(_tmp1CD,
sizeof(char),_get_zero_arr_size(_tmp1CD,5));}));Cyc_Absyndump_dumpexp(_tmp1A4);
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1A6);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp1A8);Cyc_Absyndump_dump_nospace(({const char*_tmp1CE="){";
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,3));}));Cyc_Absyndump_dumpstmt(
_tmp1A9);Cyc_Absyndump_dump_char((int)'}');goto _LL149;_LL160: if(_tmp197 <= (void*)
1)goto _LL162;if(*((int*)_tmp197)!= 9)goto _LL162;_tmp1AA=((struct Cyc_Absyn_Switch_s_struct*)
_tmp197)->f1;_tmp1AB=((struct Cyc_Absyn_Switch_s_struct*)_tmp197)->f2;_LL161: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1CF="switch(";_tag_dynforward(_tmp1CF,
sizeof(char),_get_zero_arr_size(_tmp1CF,8));}));Cyc_Absyndump_dumpexp(_tmp1AA);
Cyc_Absyndump_dump_nospace(({const char*_tmp1D0="){";_tag_dynforward(_tmp1D0,
sizeof(char),_get_zero_arr_size(_tmp1D0,3));}));Cyc_Absyndump_dumpswitchclauses(
_tmp1AB);Cyc_Absyndump_dump_char((int)'}');goto _LL149;_LL162: if(_tmp197 <= (void*)
1)goto _LL164;if(*((int*)_tmp197)!= 11)goto _LL164;_tmp1AC=((struct Cyc_Absyn_Decl_s_struct*)
_tmp197)->f1;_tmp1AD=((struct Cyc_Absyn_Decl_s_struct*)_tmp197)->f2;_LL163: Cyc_Absyndump_dumpdecl(
_tmp1AC);Cyc_Absyndump_dumpstmt(_tmp1AD);goto _LL149;_LL164: if(_tmp197 <= (void*)1)
goto _LL166;if(*((int*)_tmp197)!= 12)goto _LL166;_tmp1AE=((struct Cyc_Absyn_Label_s_struct*)
_tmp197)->f1;_tmp1AF=((struct Cyc_Absyn_Label_s_struct*)_tmp197)->f2;_LL165: if(
Cyc_Absynpp_is_declaration(_tmp1AF)){Cyc_Absyndump_dump_str(_tmp1AE);Cyc_Absyndump_dump_nospace(({
const char*_tmp1D1=": {";_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(
_tmp1D1,4));}));Cyc_Absyndump_dumpstmt(_tmp1AF);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dump_str(_tmp1AE);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(
_tmp1AF);}goto _LL149;_LL166: if(_tmp197 <= (void*)1)goto _LL168;if(*((int*)_tmp197)
!= 13)goto _LL168;_tmp1B0=((struct Cyc_Absyn_Do_s_struct*)_tmp197)->f1;_tmp1B1=((
struct Cyc_Absyn_Do_s_struct*)_tmp197)->f2;_tmp1B2=_tmp1B1.f1;_LL167: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1D2="do{";_tag_dynforward(_tmp1D2,
sizeof(char),_get_zero_arr_size(_tmp1D2,4));}));Cyc_Absyndump_dumpstmt(_tmp1B0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1D3="}while(";_tag_dynforward(_tmp1D3,
sizeof(char),_get_zero_arr_size(_tmp1D3,8));}));Cyc_Absyndump_dumpexp(_tmp1B2);
Cyc_Absyndump_dump_nospace(({const char*_tmp1D4=");";_tag_dynforward(_tmp1D4,
sizeof(char),_get_zero_arr_size(_tmp1D4,3));}));goto _LL149;_LL168: if(_tmp197 <= (
void*)1)goto _LL16A;if(*((int*)_tmp197)!= 10)goto _LL16A;_tmp1B3=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp197)->f1;if(_tmp1B3 != 0)goto _LL16A;_LL169: Cyc_Absyndump_dump(({const char*
_tmp1D5="fallthru;";_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(
_tmp1D5,10));}));goto _LL149;_LL16A: if(_tmp197 <= (void*)1)goto _LL16C;if(*((int*)
_tmp197)!= 10)goto _LL16C;_tmp1B4=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp197)->f1;
_LL16B: Cyc_Absyndump_dump(({const char*_tmp1D6="fallthru(";_tag_dynforward(
_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,10));}));Cyc_Absyndump_dumpexps_prec(
20,_tmp1B4);Cyc_Absyndump_dump_nospace(({const char*_tmp1D7=");";_tag_dynforward(
_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,3));}));goto _LL149;_LL16C: if(
_tmp197 <= (void*)1)goto _LL16E;if(*((int*)_tmp197)!= 14)goto _LL16E;_tmp1B5=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp197)->f1;_tmp1B6=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp197)->f2;_LL16D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1D8="try";_tag_dynforward(_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,4));}));
Cyc_Absyndump_dumpstmt(_tmp1B5);Cyc_Absyndump_dump(({const char*_tmp1D9="catch{";
_tag_dynforward(_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,7));}));Cyc_Absyndump_dumpswitchclauses(
_tmp1B6);Cyc_Absyndump_dump_char((int)'}');goto _LL149;_LL16E: if(_tmp197 <= (void*)
1)goto _LL170;if(*((int*)_tmp197)!= 15)goto _LL170;_tmp1B7=((struct Cyc_Absyn_Region_s_struct*)
_tmp197)->f1;_tmp1B8=((struct Cyc_Absyn_Region_s_struct*)_tmp197)->f2;_tmp1B9=((
struct Cyc_Absyn_Region_s_struct*)_tmp197)->f3;_tmp1BA=((struct Cyc_Absyn_Region_s_struct*)
_tmp197)->f4;_tmp1BB=((struct Cyc_Absyn_Region_s_struct*)_tmp197)->f5;_LL16F: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1DA="region";_tag_dynforward(_tmp1DA,
sizeof(char),_get_zero_arr_size(_tmp1DA,7));}));if(_tmp1B9)Cyc_Absyndump_dump(({
const char*_tmp1DB="[resetable]";_tag_dynforward(_tmp1DB,sizeof(char),
_get_zero_arr_size(_tmp1DB,12));}));Cyc_Absyndump_dump(({const char*_tmp1DC="<";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,2));}));Cyc_Absyndump_dumptvar(
_tmp1B7);Cyc_Absyndump_dump(({const char*_tmp1DD="> ";_tag_dynforward(_tmp1DD,
sizeof(char),_get_zero_arr_size(_tmp1DD,3));}));Cyc_Absyndump_dumpqvar(_tmp1B8->name);
if(_tmp1BA != 0){Cyc_Absyndump_dump(({const char*_tmp1DE=" = open(";
_tag_dynforward(_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp1BA);Cyc_Absyndump_dump(({const char*_tmp1DF=")";
_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,2));}));}Cyc_Absyndump_dump(({
const char*_tmp1E0="{";_tag_dynforward(_tmp1E0,sizeof(char),_get_zero_arr_size(
_tmp1E0,2));}));Cyc_Absyndump_dumpstmt(_tmp1BB);Cyc_Absyndump_dump(({const char*
_tmp1E1="}";_tag_dynforward(_tmp1E1,sizeof(char),_get_zero_arr_size(_tmp1E1,2));}));
goto _LL149;_LL170: if(_tmp197 <= (void*)1)goto _LL172;if(*((int*)_tmp197)!= 16)goto
_LL172;_tmp1BC=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp197)->f1;_LL171: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1E2="reset_region(";_tag_dynforward(
_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,14));}));Cyc_Absyndump_dumpexp(
_tmp1BC);Cyc_Absyndump_dump(({const char*_tmp1E3=");";_tag_dynforward(_tmp1E3,
sizeof(char),_get_zero_arr_size(_tmp1E3,3));}));goto _LL149;_LL172: if(_tmp197 <= (
void*)1)goto _LL149;if(*((int*)_tmp197)!= 17)goto _LL149;_tmp1BD=((struct Cyc_Absyn_Alias_s_struct*)
_tmp197)->f1;_tmp1BE=((struct Cyc_Absyn_Alias_s_struct*)_tmp197)->f2;_tmp1BF=((
struct Cyc_Absyn_Alias_s_struct*)_tmp197)->f3;_tmp1C0=((struct Cyc_Absyn_Alias_s_struct*)
_tmp197)->f4;_LL173: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1E4="alias(";_tag_dynforward(_tmp1E4,sizeof(char),_get_zero_arr_size(_tmp1E4,
7));}));Cyc_Absyndump_dumpexp(_tmp1BD);Cyc_Absyndump_dump(({const char*_tmp1E5=") = ";
_tag_dynforward(_tmp1E5,sizeof(char),_get_zero_arr_size(_tmp1E5,5));}));Cyc_Absyndump_dump(({
const char*_tmp1E6="<";_tag_dynforward(_tmp1E6,sizeof(char),_get_zero_arr_size(
_tmp1E6,2));}));Cyc_Absyndump_dumptvar(_tmp1BE);Cyc_Absyndump_dump(({const char*
_tmp1E7=">";_tag_dynforward(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,2));}));
Cyc_Absyndump_dumpqvar(_tmp1BF->name);Cyc_Absyndump_dump(({const char*_tmp1E8="{";
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size(_tmp1E8,2));}));Cyc_Absyndump_dumpstmt(
_tmp1C0);Cyc_Absyndump_dump(({const char*_tmp1E9="}";_tag_dynforward(_tmp1E9,
sizeof(char),_get_zero_arr_size(_tmp1E9,2));}));goto _LL149;_LL149:;}struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple11*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,({
const char*_tmp1EA="";_tag_dynforward(_tmp1EA,sizeof(char),_get_zero_arr_size(
_tmp1EA,1));}),({const char*_tmp1EB="=";_tag_dynforward(_tmp1EB,sizeof(char),
_get_zero_arr_size(_tmp1EB,2));}),({const char*_tmp1EC="=";_tag_dynforward(
_tmp1EC,sizeof(char),_get_zero_arr_size(_tmp1EC,2));}));Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp1ED=(void*)p->r;
void*_tmp1EE;int _tmp1EF;void*_tmp1F0;int _tmp1F1;void*_tmp1F2;int _tmp1F3;char
_tmp1F4;struct _dynforward_ptr _tmp1F5;struct Cyc_Absyn_Vardecl*_tmp1F6;struct Cyc_Absyn_Pat*
_tmp1F7;struct Cyc_Absyn_Pat _tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Vardecl*_tmp1FA;
struct Cyc_Absyn_Pat*_tmp1FB;struct Cyc_List_List*_tmp1FC;int _tmp1FD;struct Cyc_Absyn_Pat*
_tmp1FE;struct Cyc_Absyn_Vardecl*_tmp1FF;struct Cyc_Absyn_Pat*_tmp200;struct Cyc_Absyn_Pat
_tmp201;void*_tmp202;struct Cyc_Absyn_Vardecl*_tmp203;struct Cyc_Absyn_Pat*_tmp204;
struct Cyc_Absyn_Tvar*_tmp205;struct Cyc_Absyn_Vardecl*_tmp206;struct _tuple0*
_tmp207;struct _tuple0*_tmp208;struct Cyc_List_List*_tmp209;int _tmp20A;struct Cyc_Absyn_AggrInfo
_tmp20B;union Cyc_Absyn_AggrInfoU_union _tmp20C;struct Cyc_List_List*_tmp20D;struct
Cyc_List_List*_tmp20E;int _tmp20F;struct Cyc_Absyn_Tunionfield*_tmp210;struct Cyc_List_List*
_tmp211;int _tmp212;struct Cyc_Absyn_Enumfield*_tmp213;struct Cyc_Absyn_Enumfield*
_tmp214;struct Cyc_Absyn_Exp*_tmp215;_LL185: if((int)_tmp1ED != 0)goto _LL187;_LL186:
Cyc_Absyndump_dump_char((int)'_');goto _LL184;_LL187: if((int)_tmp1ED != 1)goto
_LL189;_LL188: Cyc_Absyndump_dump(({const char*_tmp216="NULL";_tag_dynforward(
_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,5));}));goto _LL184;_LL189: if(
_tmp1ED <= (void*)2)goto _LL18B;if(*((int*)_tmp1ED)!= 7)goto _LL18B;_tmp1EE=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp1ED)->f1;if((int)_tmp1EE != 2)goto _LL18B;
_tmp1EF=((struct Cyc_Absyn_Int_p_struct*)_tmp1ED)->f2;_LL18A: _tmp1F1=_tmp1EF;goto
_LL18C;_LL18B: if(_tmp1ED <= (void*)2)goto _LL18D;if(*((int*)_tmp1ED)!= 7)goto
_LL18D;_tmp1F0=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1ED)->f1;if((int)
_tmp1F0 != 0)goto _LL18D;_tmp1F1=((struct Cyc_Absyn_Int_p_struct*)_tmp1ED)->f2;
_LL18C: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp219;_tmp219.tag=1;_tmp219.f1=(unsigned long)_tmp1F1;{void*_tmp217[1]={&
_tmp219};Cyc_aprintf(({const char*_tmp218="%d";_tag_dynforward(_tmp218,sizeof(
char),_get_zero_arr_size(_tmp218,3));}),_tag_dynforward(_tmp217,sizeof(void*),1));}}));
goto _LL184;_LL18D: if(_tmp1ED <= (void*)2)goto _LL18F;if(*((int*)_tmp1ED)!= 7)goto
_LL18F;_tmp1F2=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1ED)->f1;if((int)
_tmp1F2 != 1)goto _LL18F;_tmp1F3=((struct Cyc_Absyn_Int_p_struct*)_tmp1ED)->f2;
_LL18E: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp21C;_tmp21C.tag=1;_tmp21C.f1=(unsigned int)_tmp1F3;{void*_tmp21A[1]={&
_tmp21C};Cyc_aprintf(({const char*_tmp21B="%u";_tag_dynforward(_tmp21B,sizeof(
char),_get_zero_arr_size(_tmp21B,3));}),_tag_dynforward(_tmp21A,sizeof(void*),1));}}));
goto _LL184;_LL18F: if(_tmp1ED <= (void*)2)goto _LL191;if(*((int*)_tmp1ED)!= 8)goto
_LL191;_tmp1F4=((struct Cyc_Absyn_Char_p_struct*)_tmp1ED)->f1;_LL190: Cyc_Absyndump_dump(({
const char*_tmp21D="'";_tag_dynforward(_tmp21D,sizeof(char),_get_zero_arr_size(
_tmp21D,2));}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1F4));Cyc_Absyndump_dump_nospace(({
const char*_tmp21E="'";_tag_dynforward(_tmp21E,sizeof(char),_get_zero_arr_size(
_tmp21E,2));}));goto _LL184;_LL191: if(_tmp1ED <= (void*)2)goto _LL193;if(*((int*)
_tmp1ED)!= 9)goto _LL193;_tmp1F5=((struct Cyc_Absyn_Float_p_struct*)_tmp1ED)->f1;
_LL192: Cyc_Absyndump_dump(_tmp1F5);goto _LL184;_LL193: if(_tmp1ED <= (void*)2)goto
_LL195;if(*((int*)_tmp1ED)!= 0)goto _LL195;_tmp1F6=((struct Cyc_Absyn_Var_p_struct*)
_tmp1ED)->f1;_tmp1F7=((struct Cyc_Absyn_Var_p_struct*)_tmp1ED)->f2;_tmp1F8=*
_tmp1F7;_tmp1F9=(void*)_tmp1F8.r;if((int)_tmp1F9 != 0)goto _LL195;_LL194: Cyc_Absyndump_dumpqvar(
_tmp1F6->name);goto _LL184;_LL195: if(_tmp1ED <= (void*)2)goto _LL197;if(*((int*)
_tmp1ED)!= 0)goto _LL197;_tmp1FA=((struct Cyc_Absyn_Var_p_struct*)_tmp1ED)->f1;
_tmp1FB=((struct Cyc_Absyn_Var_p_struct*)_tmp1ED)->f2;_LL196: Cyc_Absyndump_dumpqvar(
_tmp1FA->name);Cyc_Absyndump_dump(({const char*_tmp21F=" as ";_tag_dynforward(
_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,5));}));Cyc_Absyndump_dumppat(
_tmp1FB);goto _LL184;_LL197: if(_tmp1ED <= (void*)2)goto _LL199;if(*((int*)_tmp1ED)
!= 3)goto _LL199;_tmp1FC=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1ED)->f1;_tmp1FD=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp1ED)->f2;_LL198: {struct _dynforward_ptr term=
_tmp1FD?({const char*_tmp222=", ...)";_tag_dynforward(_tmp222,sizeof(char),
_get_zero_arr_size(_tmp222,7));}):({const char*_tmp223=")";_tag_dynforward(
_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,2));});((void(*)(void(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp1FC,({const char*_tmp220="$(";_tag_dynforward(_tmp220,sizeof(char),
_get_zero_arr_size(_tmp220,3));}),term,({const char*_tmp221=",";_tag_dynforward(
_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,2));}));goto _LL184;}_LL199: if(
_tmp1ED <= (void*)2)goto _LL19B;if(*((int*)_tmp1ED)!= 4)goto _LL19B;_tmp1FE=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp1ED)->f1;_LL19A: Cyc_Absyndump_dump(({const
char*_tmp224="&";_tag_dynforward(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,
2));}));Cyc_Absyndump_dumppat(_tmp1FE);goto _LL184;_LL19B: if(_tmp1ED <= (void*)2)
goto _LL19D;if(*((int*)_tmp1ED)!= 1)goto _LL19D;_tmp1FF=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1ED)->f1;_tmp200=((struct Cyc_Absyn_Reference_p_struct*)_tmp1ED)->f2;_tmp201=*
_tmp200;_tmp202=(void*)_tmp201.r;if((int)_tmp202 != 0)goto _LL19D;_LL19C: Cyc_Absyndump_dump(({
const char*_tmp225="*";_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(
_tmp225,2));}));Cyc_Absyndump_dumpqvar(_tmp1FF->name);goto _LL184;_LL19D: if(
_tmp1ED <= (void*)2)goto _LL19F;if(*((int*)_tmp1ED)!= 1)goto _LL19F;_tmp203=((
struct Cyc_Absyn_Reference_p_struct*)_tmp1ED)->f1;_tmp204=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1ED)->f2;_LL19E: Cyc_Absyndump_dump(({const char*_tmp226="*";_tag_dynforward(
_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,2));}));Cyc_Absyndump_dumpqvar(
_tmp203->name);Cyc_Absyndump_dump(({const char*_tmp227=" as ";_tag_dynforward(
_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,5));}));Cyc_Absyndump_dumppat(
_tmp204);goto _LL184;_LL19F: if(_tmp1ED <= (void*)2)goto _LL1A1;if(*((int*)_tmp1ED)
!= 2)goto _LL1A1;_tmp205=((struct Cyc_Absyn_TagInt_p_struct*)_tmp1ED)->f1;_tmp206=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp1ED)->f2;_LL1A0: Cyc_Absyndump_dumpqvar(
_tmp206->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp205);
Cyc_Absyndump_dump_char((int)'>');goto _LL184;_LL1A1: if(_tmp1ED <= (void*)2)goto
_LL1A3;if(*((int*)_tmp1ED)!= 12)goto _LL1A3;_tmp207=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1ED)->f1;_LL1A2: Cyc_Absyndump_dumpqvar(_tmp207);goto _LL184;_LL1A3: if(_tmp1ED
<= (void*)2)goto _LL1A5;if(*((int*)_tmp1ED)!= 13)goto _LL1A5;_tmp208=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1ED)->f1;_tmp209=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1ED)->f2;
_tmp20A=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1ED)->f3;_LL1A4: {struct
_dynforward_ptr term=_tmp20A?({const char*_tmp22A=", ...)";_tag_dynforward(_tmp22A,
sizeof(char),_get_zero_arr_size(_tmp22A,7));}):({const char*_tmp22B=")";
_tag_dynforward(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,2));});Cyc_Absyndump_dumpqvar(
_tmp208);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp209,({const char*_tmp228="(";_tag_dynforward(_tmp228,
sizeof(char),_get_zero_arr_size(_tmp228,2));}),term,({const char*_tmp229=",";
_tag_dynforward(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,2));}));goto
_LL184;}_LL1A5: if(_tmp1ED <= (void*)2)goto _LL1A7;if(*((int*)_tmp1ED)!= 5)goto
_LL1A7;_tmp20B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1ED)->f1;_tmp20C=_tmp20B.aggr_info;
_tmp20D=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1ED)->f2;_tmp20E=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1ED)->f3;_tmp20F=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1ED)->f4;_LL1A6: {
struct _tuple0*_tmp22D;struct _tuple3 _tmp22C=Cyc_Absyn_aggr_kinded_name(_tmp20C);
_tmp22D=_tmp22C.f2;{struct _dynforward_ptr term=_tmp20F?({const char*_tmp233=", ...)";
_tag_dynforward(_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,7));}):({const
char*_tmp234=")";_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,
2));});Cyc_Absyndump_dumpqvar(_tmp22D);Cyc_Absyndump_dump_char((int)'{');((void(*)(
void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp20D,({const char*_tmp22E="[";_tag_dynforward(_tmp22E,sizeof(char),
_get_zero_arr_size(_tmp22E,2));}),({const char*_tmp22F="]";_tag_dynforward(
_tmp22F,sizeof(char),_get_zero_arr_size(_tmp22F,2));}),({const char*_tmp230=",";
_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,2));}));((void(*)(
void(*f)(struct _tuple11*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp20E,({const char*_tmp231="";_tag_dynforward(_tmp231,sizeof(char),
_get_zero_arr_size(_tmp231,1));}),term,({const char*_tmp232=",";_tag_dynforward(
_tmp232,sizeof(char),_get_zero_arr_size(_tmp232,2));}));goto _LL184;}}_LL1A7: if(
_tmp1ED <= (void*)2)goto _LL1A9;if(*((int*)_tmp1ED)!= 6)goto _LL1A9;_tmp210=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp1ED)->f2;_tmp211=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1ED)->f3;_tmp212=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1ED)->f4;_LL1A8: {
struct _dynforward_ptr term=_tmp212?({const char*_tmp237=", ...)";_tag_dynforward(
_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,7));}):({const char*_tmp238=")";
_tag_dynforward(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,2));});Cyc_Absyndump_dumpqvar(
_tmp210->name);if(_tmp211 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp211,({const char*_tmp235="(";
_tag_dynforward(_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,2));}),term,({
const char*_tmp236=",";_tag_dynforward(_tmp236,sizeof(char),_get_zero_arr_size(
_tmp236,2));}));goto _LL184;}_LL1A9: if(_tmp1ED <= (void*)2)goto _LL1AB;if(*((int*)
_tmp1ED)!= 10)goto _LL1AB;_tmp213=((struct Cyc_Absyn_Enum_p_struct*)_tmp1ED)->f2;
_LL1AA: _tmp214=_tmp213;goto _LL1AC;_LL1AB: if(_tmp1ED <= (void*)2)goto _LL1AD;if(*((
int*)_tmp1ED)!= 11)goto _LL1AD;_tmp214=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp1ED)->f2;_LL1AC: Cyc_Absyndump_dumpqvar(_tmp214->name);goto _LL184;_LL1AD: if(
_tmp1ED <= (void*)2)goto _LL184;if(*((int*)_tmp1ED)!= 14)goto _LL184;_tmp215=((
struct Cyc_Absyn_Exp_p_struct*)_tmp1ED)->f1;_LL1AE: Cyc_Absyndump_dumpexp(_tmp215);
goto _LL184;_LL184:;}void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _dynforward_ptr sep))
Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,fields,({const char*_tmp239=",";
_tag_dynforward(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,2));}));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp23A=" = ";_tag_dynforward(_tmp23A,sizeof(char),
_get_zero_arr_size(_tmp23A,4));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*
fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,
struct _dynforward_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,({const char*_tmp23B=",";_tag_dynforward(_tmp23B,sizeof(char),
_get_zero_arr_size(_tmp23B,2));}));}void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*
fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp23D;
struct _dynforward_ptr*_tmp23E;struct Cyc_Absyn_Tqual _tmp23F;void*_tmp240;struct
Cyc_Absyn_Exp*_tmp241;struct Cyc_List_List*_tmp242;struct Cyc_Absyn_Aggrfield*
_tmp23C=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.name;
_tmp23F=_tmp23D.tq;_tmp240=(void*)_tmp23D.type;_tmp241=_tmp23D.width;_tmp242=
_tmp23D.attributes;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct
_dynforward_ptr*),struct _dynforward_ptr*))Cyc_Absyndump_dumptqtd)(_tmp23F,
_tmp240,Cyc_Absyndump_dump_str,_tmp23E);Cyc_Absyndump_dumpatts(_tmp242);if(
_tmp241 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp241);}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(struct
Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple3*pr){{void*_tmp243=(*pr).f1;_LL1B0: if((int)_tmp243 != 8)goto _LL1B2;
_LL1B1: goto _LL1AF;_LL1B2: if((int)_tmp243 != 0)goto _LL1B4;_LL1B3: Cyc_Absyndump_dump(({
const char*_tmp244="_stdcall";_tag_dynforward(_tmp244,sizeof(char),
_get_zero_arr_size(_tmp244,9));}));goto _LL1AF;_LL1B4: if((int)_tmp243 != 1)goto
_LL1B6;_LL1B5: Cyc_Absyndump_dump(({const char*_tmp245="_cdecl";_tag_dynforward(
_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,7));}));goto _LL1AF;_LL1B6: if((
int)_tmp243 != 2)goto _LL1B8;_LL1B7: Cyc_Absyndump_dump(({const char*_tmp246="_fastcall";
_tag_dynforward(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,10));}));goto
_LL1AF;_LL1B8:;_LL1B9: goto _LL1AF;_LL1AF:;}Cyc_Absyndump_dumpqvar((*pr).f2);}
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dumpids(
struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp248;void*_tmp249;struct _tuple0*_tmp24A;struct Cyc_Absyn_Tqual
_tmp24B;void*_tmp24C;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_List_List*_tmp24E;
struct Cyc_Absyn_Vardecl*_tmp247=vd;_tmp248=*_tmp247;_tmp249=(void*)_tmp248.sc;
_tmp24A=_tmp248.name;_tmp24B=_tmp248.tq;_tmp24C=(void*)_tmp248.type;_tmp24D=
_tmp248.initializer;_tmp24E=_tmp248.attributes;Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts(_tmp24E);Cyc_Absyndump_dumpscope(_tmp249);{struct
_RegionHandle _tmp24F=_new_region("temp");struct _RegionHandle*temp=& _tmp24F;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp251;void*_tmp252;struct Cyc_List_List*
_tmp253;struct _tuple5 _tmp250=Cyc_Absynpp_to_tms(temp,_tmp24B,_tmp24C);_tmp251=
_tmp250.f1;_tmp252=_tmp250.f2;_tmp253=_tmp250.f3;{void*call_conv=(void*)8;{
struct Cyc_List_List*tms2=_tmp253;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp254=(
void*)tms2->hd;struct Cyc_List_List*_tmp255;_LL1BB: if(*((int*)_tmp254)!= 5)goto
_LL1BD;_tmp255=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp254)->f2;_LL1BC: for(
0;_tmp255 != 0;_tmp255=_tmp255->tl){void*_tmp256=(void*)_tmp255->hd;_LL1C0: if((
int)_tmp256 != 0)goto _LL1C2;_LL1C1: call_conv=(void*)0;goto _LL1BF;_LL1C2: if((int)
_tmp256 != 1)goto _LL1C4;_LL1C3: call_conv=(void*)1;goto _LL1BF;_LL1C4: if((int)
_tmp256 != 2)goto _LL1C6;_LL1C5: call_conv=(void*)2;goto _LL1BF;_LL1C6:;_LL1C7: goto
_LL1BF;_LL1BF:;}goto _LL1BA;_LL1BD:;_LL1BE: goto _LL1BA;_LL1BA:;}}Cyc_Absyndump_dumptq(
_tmp251);Cyc_Absyndump_dumpntyp(_tmp252);{struct _tuple3 _tmp257=({struct _tuple3
_tmp258;_tmp258.f1=call_conv;_tmp258.f2=_tmp24A;_tmp258;});((void(*)(struct Cyc_List_List*
tms,void(*f)(struct _tuple3*),struct _tuple3*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(
_tmp253),Cyc_Absyndump_dump_callconv_qvar,& _tmp257);}}};_pop_region(temp);}}
else{if(_tmp249 == (void*)3  && Cyc_Absyndump_qvar_to_Cids){void*_tmp259=Cyc_Tcutil_compress(
_tmp24C);_LL1C9: if(_tmp259 <= (void*)4)goto _LL1CB;if(*((int*)_tmp259)!= 8)goto
_LL1CB;_LL1CA: goto _LL1C8;_LL1CB:;_LL1CC: Cyc_Absyndump_dumpscope(_tmp249);_LL1C8:;}
else{Cyc_Absyndump_dumpscope(_tmp249);}((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp24B,_tmp24C,
Cyc_Absyndump_dumpqvar,_tmp24A);Cyc_Absyndump_dumpatts(_tmp24E);}if(_tmp24D != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp24D);}Cyc_Absyndump_dump_semi();}struct _tuple12{struct Cyc_Position_Segment*
f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
void*_tmp25A=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp25B;struct Cyc_Absyn_Fndecl*
_tmp25C;struct Cyc_Absyn_Aggrdecl*_tmp25D;struct Cyc_Absyn_Tuniondecl*_tmp25E;
struct Cyc_Absyn_Tuniondecl _tmp25F;void*_tmp260;struct _tuple0*_tmp261;struct Cyc_List_List*
_tmp262;struct Cyc_Core_Opt*_tmp263;int _tmp264;int _tmp265;struct Cyc_Absyn_Enumdecl*
_tmp266;struct Cyc_Absyn_Enumdecl _tmp267;void*_tmp268;struct _tuple0*_tmp269;
struct Cyc_Core_Opt*_tmp26A;struct Cyc_Absyn_Pat*_tmp26B;struct Cyc_Absyn_Exp*
_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_Absyn_Typedefdecl*_tmp26E;struct
_dynforward_ptr*_tmp26F;struct Cyc_List_List*_tmp270;struct _tuple0*_tmp271;struct
Cyc_List_List*_tmp272;struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp274;
struct Cyc_List_List*_tmp275;_LL1CE: if(_tmp25A <= (void*)2)goto _LL1E6;if(*((int*)
_tmp25A)!= 0)goto _LL1D0;_tmp25B=((struct Cyc_Absyn_Var_d_struct*)_tmp25A)->f1;
_LL1CF: Cyc_Absyndump_dumpvardecl(_tmp25B,d->loc);goto _LL1CD;_LL1D0: if(*((int*)
_tmp25A)!= 1)goto _LL1D2;_tmp25C=((struct Cyc_Absyn_Fn_d_struct*)_tmp25A)->f1;
_LL1D1: Cyc_Absyndump_dumploc(d->loc);if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(
_tmp25C->attributes);if(_tmp25C->is_inline){if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(({
const char*_tmp276="__inline";_tag_dynforward(_tmp276,sizeof(char),
_get_zero_arr_size(_tmp276,9));}));else{Cyc_Absyndump_dump(({const char*_tmp277="inline";
_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,7));}));}}Cyc_Absyndump_dumpscope((
void*)_tmp25C->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp278=
_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_FnType_struct _tmp279;
_tmp279.tag=8;_tmp279.f1=({struct Cyc_Absyn_FnInfo _tmp27A;_tmp27A.tvars=_tmp25C->tvs;
_tmp27A.effect=_tmp25C->effect;_tmp27A.ret_typ=(void*)((void*)_tmp25C->ret_type);
_tmp27A.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp25C->args);_tmp27A.c_varargs=
_tmp25C->c_varargs;_tmp27A.cyc_varargs=_tmp25C->cyc_varargs;_tmp27A.rgn_po=
_tmp25C->rgn_po;_tmp27A.attributes=0;_tmp27A;});_tmp279;});_tmp278;});((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_to_VC?Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar,_tmp25C);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp25C->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;}_LL1D2: if(*((int*)
_tmp25A)!= 4)goto _LL1D4;_tmp25D=((struct Cyc_Absyn_Aggr_d_struct*)_tmp25A)->f1;
_LL1D3: Cyc_Absyndump_dumpscope((void*)_tmp25D->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp25D->kind);Cyc_Absyndump_dumpqvar(_tmp25D->name);Cyc_Absyndump_dumptvars(
_tmp25D->tvs);if(_tmp25D->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25D->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25D->impl))->exist_vars,({
const char*_tmp27B="<";_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(
_tmp27B,2));}),({const char*_tmp27C=">";_tag_dynforward(_tmp27C,sizeof(char),
_get_zero_arr_size(_tmp27C,2));}),({const char*_tmp27D=",";_tag_dynforward(
_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,2));}));if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp25D->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25D->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25D->impl))->fields);Cyc_Absyndump_dump(({
const char*_tmp27E="}";_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(
_tmp27E,2));}));Cyc_Absyndump_dumpatts(_tmp25D->attributes);Cyc_Absyndump_dump(({
const char*_tmp27F=";";_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(
_tmp27F,2));}));}goto _LL1CD;_LL1D4: if(*((int*)_tmp25A)!= 5)goto _LL1D6;_tmp25E=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp25A)->f1;_tmp25F=*_tmp25E;_tmp260=(void*)
_tmp25F.sc;_tmp261=_tmp25F.name;_tmp262=_tmp25F.tvs;_tmp263=_tmp25F.fields;
_tmp264=_tmp25F.is_xtunion;_tmp265=_tmp25F.is_flat;_LL1D5: Cyc_Absyndump_dumpscope(
_tmp260);Cyc_Absyndump_dump(_tmp264?({const char*_tmp280="xtunion";
_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,8));}):({const
char*_tmp281="tunion";_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(
_tmp281,7));}));if(_tmp265)Cyc_Absyndump_dump_nospace(({const char*_tmp282=" __attribute__((flat))";
_tag_dynforward(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,23));}));Cyc_Absyndump_dumpqvar(
_tmp261);Cyc_Absyndump_dumptvars(_tmp262);if(_tmp263 == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)
_tmp263->v);Cyc_Absyndump_dump_nospace(({const char*_tmp283="};";_tag_dynforward(
_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,3));}));}goto _LL1CD;_LL1D6: if(*((
int*)_tmp25A)!= 6)goto _LL1D8;_tmp266=((struct Cyc_Absyn_Enum_d_struct*)_tmp25A)->f1;
_tmp267=*_tmp266;_tmp268=(void*)_tmp267.sc;_tmp269=_tmp267.name;_tmp26A=_tmp267.fields;
_LL1D7: Cyc_Absyndump_dumpscope(_tmp268);Cyc_Absyndump_dump(({const char*_tmp284="enum ";
_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,6));}));Cyc_Absyndump_dumpqvar(
_tmp269);if(_tmp26A == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp26A->v);Cyc_Absyndump_dump_nospace(({
const char*_tmp285="};";_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size(
_tmp285,3));}));}return;_LL1D8: if(*((int*)_tmp25A)!= 2)goto _LL1DA;_tmp26B=((
struct Cyc_Absyn_Let_d_struct*)_tmp25A)->f1;_tmp26C=((struct Cyc_Absyn_Let_d_struct*)
_tmp25A)->f3;_LL1D9: Cyc_Absyndump_dump(({const char*_tmp286="let";_tag_dynforward(
_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,4));}));Cyc_Absyndump_dumppat(
_tmp26B);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp26C);Cyc_Absyndump_dump_semi();
goto _LL1CD;_LL1DA: if(*((int*)_tmp25A)!= 3)goto _LL1DC;_tmp26D=((struct Cyc_Absyn_Letv_d_struct*)
_tmp25A)->f1;_LL1DB: Cyc_Absyndump_dump(({const char*_tmp287="let ";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,5));}));Cyc_Absyndump_dumpids(
_tmp26D);Cyc_Absyndump_dump_semi();goto _LL1CD;_LL1DC: if(*((int*)_tmp25A)!= 7)
goto _LL1DE;_tmp26E=((struct Cyc_Absyn_Typedef_d_struct*)_tmp25A)->f1;_LL1DD: if(!
Cyc_Absyndump_expand_typedefs  || _tmp26E->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp26E->defn))->v)){Cyc_Absyndump_dump(({
const char*_tmp288="typedef";_tag_dynforward(_tmp288,sizeof(char),
_get_zero_arr_size(_tmp288,8));}));{void*t;if(_tmp26E->defn == 0)t=Cyc_Absyn_new_evar(
_tmp26E->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp26E->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp26E->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp26E);Cyc_Absyndump_dumpatts(_tmp26E->atts);Cyc_Absyndump_dump_semi();}}goto
_LL1CD;_LL1DE: if(*((int*)_tmp25A)!= 8)goto _LL1E0;_tmp26F=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp25A)->f1;_tmp270=((struct Cyc_Absyn_Namespace_d_struct*)_tmp25A)->f2;_LL1DF:
Cyc_Absyndump_dump(({const char*_tmp289="namespace";_tag_dynforward(_tmp289,
sizeof(char),_get_zero_arr_size(_tmp289,10));}));Cyc_Absyndump_dump_str(_tmp26F);
Cyc_Absyndump_dump_char((int)'{');for(0;_tmp270 != 0;_tmp270=_tmp270->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp270->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E0: if(*((int*)_tmp25A)!= 9)goto _LL1E2;_tmp271=((struct Cyc_Absyn_Using_d_struct*)
_tmp25A)->f1;_tmp272=((struct Cyc_Absyn_Using_d_struct*)_tmp25A)->f2;_LL1E1: Cyc_Absyndump_dump(({
const char*_tmp28A="using";_tag_dynforward(_tmp28A,sizeof(char),
_get_zero_arr_size(_tmp28A,6));}));Cyc_Absyndump_dumpqvar(_tmp271);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp272 != 0;_tmp272=_tmp272->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp272->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;
_LL1E2: if(*((int*)_tmp25A)!= 10)goto _LL1E4;_tmp273=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp25A)->f1;_LL1E3: Cyc_Absyndump_dump(({const char*_tmp28B="extern \"C\" {";
_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,13));}));for(0;
_tmp273 != 0;_tmp273=_tmp273->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp273->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1CD;_LL1E4: if(*((int*)
_tmp25A)!= 11)goto _LL1E6;_tmp274=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp25A)->f1;_tmp275=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp25A)->f2;
_LL1E5: Cyc_Absyndump_dump(({const char*_tmp28C="extern \"C include\" {";
_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,21));}));for(0;
_tmp274 != 0;_tmp274=_tmp274->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp274->hd);}Cyc_Absyndump_dump_char((int)'}');if(_tmp275 != 0){Cyc_Absyndump_dump(({
const char*_tmp28D=" export {";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,10));}));for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
struct _tuple0*_tmp28F;struct _tuple12 _tmp28E=*((struct _tuple12*)_tmp275->hd);
_tmp28F=_tmp28E.f2;Cyc_Absyndump_dumpqvar(_tmp28F);if(_tmp275->tl != 0)Cyc_Absyndump_dump_char((
int)',');}Cyc_Absyndump_dump(({const char*_tmp290="}";_tag_dynforward(_tmp290,
sizeof(char),_get_zero_arr_size(_tmp290,2));}));}goto _LL1CD;_LL1E6: if((int)
_tmp25A != 0)goto _LL1E8;_LL1E7: Cyc_Absyndump_dump(({const char*_tmp291=" __cyclone_port_on__; ";
_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,23));}));goto
_LL1CD;_LL1E8: if((int)_tmp25A != 1)goto _LL1CD;_LL1E9: Cyc_Absyndump_dump(({const
char*_tmp292=" __cyclone_port_off__; ";_tag_dynforward(_tmp292,sizeof(char),
_get_zero_arr_size(_tmp292,24));}));goto _LL1CD;_LL1CD:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e){struct _tuple6 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1
!= 1  || !pr.f2){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp293=(void*)tms->hd;struct Cyc_Absyn_PtrAtts
_tmp294;void*_tmp295;struct Cyc_Absyn_Conref*_tmp296;struct Cyc_Absyn_Conref*
_tmp297;struct Cyc_Absyn_Conref*_tmp298;struct Cyc_Absyn_Tqual _tmp299;_LL1EB: if(*((
int*)_tmp293)!= 2)goto _LL1ED;_tmp294=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp293)->f1;_tmp295=(void*)_tmp294.rgn;_tmp296=_tmp294.nullable;_tmp297=_tmp294.bounds;
_tmp298=_tmp294.zero_term;_tmp299=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp293)->f2;
_LL1EC:{void*_tmp29A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp297);struct
Cyc_Absyn_Exp*_tmp29B;void*_tmp29C;_LL1F0: if((int)_tmp29A != 0)goto _LL1F2;_LL1F1:
Cyc_Absyndump_dump_char((int)'?');goto _LL1EF;_LL1F2: if((int)_tmp29A != 1)goto
_LL1F4;_LL1F3: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_char((int)'-');
goto _LL1EF;_LL1F4: if(_tmp29A <= (void*)2)goto _LL1F6;if(*((int*)_tmp29A)!= 0)goto
_LL1F6;_tmp29B=((struct Cyc_Absyn_Upper_b_struct*)_tmp29A)->f1;_LL1F5: Cyc_Absyndump_dump_char((
int)(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp296)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp29B);goto _LL1EF;_LL1F6: if(_tmp29A <= (void*)2)
goto _LL1EF;if(*((int*)_tmp29A)!= 1)goto _LL1EF;_tmp29C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp29A)->f1;_LL1F7: Cyc_Absyndump_dump_char((int)(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(1,_tmp296)?'*':'@'));Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumptyp(_tmp29C);Cyc_Absyndump_dump_char((int)'}');_LL1EF:;}if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp298))Cyc_Absyndump_dump(({
const char*_tmp29D="ZEROTERM";_tag_dynforward(_tmp29D,sizeof(char),
_get_zero_arr_size(_tmp29D,9));}));{void*_tmp29E=Cyc_Tcutil_compress(_tmp295);
struct Cyc_Absyn_Tvar*_tmp29F;struct Cyc_Core_Opt*_tmp2A0;_LL1F9: if((int)_tmp29E != 
2)goto _LL1FB;_LL1FA: goto _LL1F8;_LL1FB: if(_tmp29E <= (void*)4)goto _LL1FF;if(*((int*)
_tmp29E)!= 1)goto _LL1FD;_tmp29F=((struct Cyc_Absyn_VarType_struct*)_tmp29E)->f1;
_LL1FC: Cyc_Absyndump_dump_str(_tmp29F->name);goto _LL1F8;_LL1FD: if(*((int*)
_tmp29E)!= 0)goto _LL1FF;_tmp2A0=((struct Cyc_Absyn_Evar_struct*)_tmp29E)->f2;if(
_tmp2A0 != 0)goto _LL1FF;_LL1FE: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp295));
goto _LL1F8;_LL1FF:;_LL200:({void*_tmp2A1[0]={};Cyc_Tcutil_impos(({const char*
_tmp2A2="dumptms: bad rgn type in Pointer_mod";_tag_dynforward(_tmp2A2,sizeof(
char),_get_zero_arr_size(_tmp2A2,37));}),_tag_dynforward(_tmp2A1,sizeof(void*),0));});
_LL1F8:;}Cyc_Absyndump_dumptq(_tmp299);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1ED:;_LL1EE: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp2A3=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL202: if(*((int*)_tmp2A3)!= 2)
goto _LL204;_LL203: next_is_pointer=1;goto _LL201;_LL204:;_LL205: goto _LL201;_LL201:;}
if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp2A4=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_Absyn_Conref*
_tmp2A7;void*_tmp2A8;struct Cyc_List_List*_tmp2A9;int _tmp2AA;struct Cyc_Absyn_VarargInfo*
_tmp2AB;struct Cyc_Core_Opt*_tmp2AC;struct Cyc_List_List*_tmp2AD;void*_tmp2AE;
struct Cyc_List_List*_tmp2AF;struct Cyc_Position_Segment*_tmp2B0;struct Cyc_List_List*
_tmp2B1;struct Cyc_Position_Segment*_tmp2B2;int _tmp2B3;struct Cyc_List_List*
_tmp2B4;_LL207: if(*((int*)_tmp2A4)!= 0)goto _LL209;_tmp2A5=((struct Cyc_Absyn_Carray_mod_struct*)
_tmp2A4)->f1;_LL208: Cyc_Absyndump_dump(({const char*_tmp2B5="[]";_tag_dynforward(
_tmp2B5,sizeof(char),_get_zero_arr_size(_tmp2B5,3));}));if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2A5))Cyc_Absyndump_dump(({const
char*_tmp2B6="ZEROTERM";_tag_dynforward(_tmp2B6,sizeof(char),_get_zero_arr_size(
_tmp2B6,9));}));goto _LL206;_LL209: if(*((int*)_tmp2A4)!= 1)goto _LL20B;_tmp2A6=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmp2A4)->f1;_tmp2A7=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp2A4)->f2;_LL20A: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(
_tmp2A6);Cyc_Absyndump_dump_char((int)']');if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp2A7))Cyc_Absyndump_dump(({const char*_tmp2B7="ZEROTERM";
_tag_dynforward(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,9));}));goto
_LL206;_LL20B: if(*((int*)_tmp2A4)!= 3)goto _LL20D;_tmp2A8=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp2A4)->f1;if(*((int*)_tmp2A8)!= 1)goto _LL20D;_tmp2A9=((struct Cyc_Absyn_WithTypes_struct*)
_tmp2A8)->f1;_tmp2AA=((struct Cyc_Absyn_WithTypes_struct*)_tmp2A8)->f2;_tmp2AB=((
struct Cyc_Absyn_WithTypes_struct*)_tmp2A8)->f3;_tmp2AC=((struct Cyc_Absyn_WithTypes_struct*)
_tmp2A8)->f4;_tmp2AD=((struct Cyc_Absyn_WithTypes_struct*)_tmp2A8)->f5;_LL20C: Cyc_Absyndump_dumpfunargs(
_tmp2A9,_tmp2AA,_tmp2AB,_tmp2AC,_tmp2AD);goto _LL206;_LL20D: if(*((int*)_tmp2A4)!= 
3)goto _LL20F;_tmp2AE=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp2A4)->f1;
if(*((int*)_tmp2AE)!= 0)goto _LL20F;_tmp2AF=((struct Cyc_Absyn_NoTypes_struct*)
_tmp2AE)->f1;_tmp2B0=((struct Cyc_Absyn_NoTypes_struct*)_tmp2AE)->f2;_LL20E:((
void(*)(void(*f)(struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp2AF,({const char*_tmp2B8="(";_tag_dynforward(_tmp2B8,
sizeof(char),_get_zero_arr_size(_tmp2B8,2));}),({const char*_tmp2B9=")";
_tag_dynforward(_tmp2B9,sizeof(char),_get_zero_arr_size(_tmp2B9,2));}),({const
char*_tmp2BA=",";_tag_dynforward(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,
2));}));goto _LL206;_LL20F: if(*((int*)_tmp2A4)!= 4)goto _LL211;_tmp2B1=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp2A4)->f1;_tmp2B2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp2A4)->f2;
_tmp2B3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp2A4)->f3;_LL210: if(_tmp2B3)
Cyc_Absyndump_dumpkindedtvars(_tmp2B1);else{Cyc_Absyndump_dumptvars(_tmp2B1);}
goto _LL206;_LL211: if(*((int*)_tmp2A4)!= 5)goto _LL213;_tmp2B4=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp2A4)->f2;_LL212: Cyc_Absyndump_dumpatts(_tmp2B4);goto _LL206;_LL213: if(*((int*)
_tmp2A4)!= 2)goto _LL206;_LL214:({void*_tmp2BB[0]={};Cyc_Tcutil_impos(({const char*
_tmp2BC="dumptms";_tag_dynforward(_tmp2BC,sizeof(char),_get_zero_arr_size(
_tmp2BC,8));}),_tag_dynforward(_tmp2BB,sizeof(void*),0));});_LL206:;}return;}
_LL1EA:;}}void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(
void*),void*a){struct _RegionHandle _tmp2BD=_new_region("temp");struct
_RegionHandle*temp=& _tmp2BD;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp2BF;
void*_tmp2C0;struct Cyc_List_List*_tmp2C1;struct _tuple5 _tmp2BE=Cyc_Absynpp_to_tms(
temp,tq,t);_tmp2BF=_tmp2BE.f1;_tmp2C0=_tmp2BE.f2;_tmp2C1=_tmp2BE.f3;Cyc_Absyndump_dumptq(
_tmp2BF);Cyc_Absyndump_dumpntyp(_tmp2C0);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(
_tmp2C1),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;
tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp2C2[0]={};Cyc_fprintf(f,({const char*_tmp2C3="\n";_tag_dynforward(
_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,2));}),_tag_dynforward(_tmp2C2,
sizeof(void*),0));});}
