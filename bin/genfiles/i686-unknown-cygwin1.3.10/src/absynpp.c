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
 struct Cyc_Core_Opt{void*v;};struct _dynforward_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dynforward_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dynforward_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dynforward_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct
_dynforward_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dynforward_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(
struct Cyc_PP_Doc*(*pp)(void*),struct _dynforward_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dynforward_ptr sep,struct Cyc_List_List*l0);
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct
_dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(struct
_dynforward_ptr start,struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dynforward_ptr start,
struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
struct Cyc_Buffer_t;unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _tuple4{unsigned int f1;int f2;}
;struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_scopes;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dynforward_ptr Cyc_Absynpp_prim2string(void*p);struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc);
int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _dynforward_ptr Cyc_Absynpp_cyc_string;
extern struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple6{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*
fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es);struct _tuple9{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct
Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*
fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);static
int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[
4]="Cyc";struct _dynforward_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0 + 4};struct
_dynforward_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static int Cyc_Absynpp_add_cyc_prefix;
static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars;static int Cyc_Absynpp_print_all_kinds;static
int Cyc_Absynpp_print_all_effects;static int Cyc_Absynpp_print_using_stmts;static
int Cyc_Absynpp_print_externC_stmts;static int Cyc_Absynpp_print_full_evars;static
int Cyc_Absynpp_generate_line_directives;static int Cyc_Absynpp_use_curr_namespace;
static int Cyc_Absynpp_print_zeroterm;static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dynforward_ptr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dynforward_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0:
return({const char*_tmp2="\\a";_tag_dynforward(_tmp2,sizeof(char),
_get_zero_arr_size(_tmp2,3));});case '\b': _LL1: return({const char*_tmp3="\\b";
_tag_dynforward(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,3));});case '\f': _LL2:
return({const char*_tmp4="\\f";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));});case '\n': _LL3: return({const char*_tmp5="\\n";
_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,3));});case '\r': _LL4:
return({const char*_tmp6="\\r";_tag_dynforward(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,3));});case '\t': _LL5: return({const char*_tmp7="\\t";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,3));});case '\v': _LL6:
return({const char*_tmp8="\\v";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,3));});case '\\': _LL7: return({const char*_tmp9="\\\\";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,3));});case '"': _LL8:
return({const char*_tmpA="\"";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size(_tmpA,2));});case '\'': _LL9: return({const char*_tmpB="\\'";
_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,3));});default: _LLA:
if(c >= ' '  && c <= '~'){struct _dynforward_ptr _tmpC=Cyc_Core_new_string(2);({
struct _dynforward_ptr _tmpD=_dynforward_ptr_plus(_tmpC,sizeof(char),0);char _tmpE=*((
char*)_check_dynforward_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(
_get_dynforward_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});return(struct _dynforward_ptr)
_tmpC;}else{struct _dynforward_ptr _tmp10=Cyc_Core_new_string(5);int j=0;({struct
_dynforward_ptr _tmp11=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((
char*)_check_dynforward_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_dynforward_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _dynforward_ptr
_tmp14=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_dynforward_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_dynforward_size(_tmp14,sizeof(char))== 1  && (
_tmp15 == '\000'  && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=
_tmp16;});({struct _dynforward_ptr _tmp17=_dynforward_ptr_plus(_tmp10,sizeof(char),
j ++);char _tmp18=*((char*)_check_dynforward_subscript(_tmp17,sizeof(char),0));
char _tmp19=(char)('0' + (c >> 3 & 7));if(_get_dynforward_size(_tmp17,sizeof(char))
== 1  && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)
_tmp17.curr)=_tmp19;});({struct _dynforward_ptr _tmp1A=_dynforward_ptr_plus(_tmp10,
sizeof(char),j ++);char _tmp1B=*((char*)_check_dynforward_subscript(_tmp1A,sizeof(
char),0));char _tmp1C=(char)('0' + (c & 7));if(_get_dynforward_size(_tmp1A,sizeof(
char))== 1  && (_tmp1B == '\000'  && _tmp1C != '\000'))_throw_arraybounds();*((char*)
_tmp1A.curr)=_tmp1C;});return(struct _dynforward_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dynforward_ptr s){int sz=(int)(_get_dynforward_size(s,sizeof(char))- 1);{
int i=0;for(0;i < sz;i ++){char c=*((const char*)_check_dynforward_subscript(s,
sizeof(char),i));if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')return 1;}}
return 0;}struct _dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr s){
if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_dynforward_size(s,sizeof(
char))- 1);if(n > 0  && *((const char*)_check_dynforward_subscript(s,sizeof(char),n))
== '\000')n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;
_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '  && _tmp1D <= '~')len ++;else{len +=4;}goto
_LLC;_LLC:;}}{struct _dynforward_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;{int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;
_LL23:({struct _dynforward_ptr _tmp1F=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp20=*((char*)_check_dynforward_subscript(_tmp1F,sizeof(char),0));char
_tmp21='\\';if(_get_dynforward_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'
 && _tmp21 != '\000'))_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({
struct _dynforward_ptr _tmp22=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp23=*((
char*)_check_dynforward_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(
_get_dynforward_size(_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _dynforward_ptr _tmp25=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp26=*((char*)_check_dynforward_subscript(_tmp25,sizeof(char),0));
char _tmp27='\\';if(_get_dynforward_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'
 && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({
struct _dynforward_ptr _tmp28=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((
char*)_check_dynforward_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(
_get_dynforward_size(_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _dynforward_ptr _tmp2B=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp2C=*((char*)_check_dynforward_subscript(_tmp2B,sizeof(char),0));
char _tmp2D='\\';if(_get_dynforward_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'
 && _tmp2D != '\000'))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({
struct _dynforward_ptr _tmp2E=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((
char*)_check_dynforward_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(
_get_dynforward_size(_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _dynforward_ptr _tmp31=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp32=*((char*)_check_dynforward_subscript(_tmp31,sizeof(char),0));
char _tmp33='\\';if(_get_dynforward_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'
 && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({
struct _dynforward_ptr _tmp34=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((
char*)_check_dynforward_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(
_get_dynforward_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _dynforward_ptr _tmp37=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp38=*((char*)_check_dynforward_subscript(_tmp37,sizeof(char),0));
char _tmp39='\\';if(_get_dynforward_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'
 && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({
struct _dynforward_ptr _tmp3A=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((
char*)_check_dynforward_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(
_get_dynforward_size(_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _dynforward_ptr _tmp3D=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp3E=*((char*)_check_dynforward_subscript(_tmp3D,sizeof(char),0));
char _tmp3F='\\';if(_get_dynforward_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'
 && _tmp3F != '\000'))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({
struct _dynforward_ptr _tmp40=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((
char*)_check_dynforward_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(
_get_dynforward_size(_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _dynforward_ptr _tmp43=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp44=*((char*)_check_dynforward_subscript(_tmp43,sizeof(char),0));
char _tmp45='\\';if(_get_dynforward_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'
 && _tmp45 != '\000'))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({
struct _dynforward_ptr _tmp46=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((
char*)_check_dynforward_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(
_get_dynforward_size(_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _dynforward_ptr _tmp49=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp4A=*((char*)_check_dynforward_subscript(_tmp49,sizeof(char),0));
char _tmp4B='\\';if(_get_dynforward_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'
 && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({
struct _dynforward_ptr _tmp4C=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((
char*)_check_dynforward_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_dynforward_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _dynforward_ptr _tmp4F=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp50=*((char*)_check_dynforward_subscript(_tmp4F,sizeof(char),0));
char _tmp51='\\';if(_get_dynforward_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'
 && _tmp51 != '\000'))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({
struct _dynforward_ptr _tmp52=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((
char*)_check_dynforward_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(
_get_dynforward_size(_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _dynforward_ptr _tmp55=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp56=*((char*)_check_dynforward_subscript(_tmp55,
sizeof(char),0));char _tmp57=_tmp1E;if(_get_dynforward_size(_tmp55,sizeof(char))
== 1  && (_tmp56 == '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)
_tmp55.curr)=_tmp57;});else{({struct _dynforward_ptr _tmp58=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp59=*((char*)_check_dynforward_subscript(_tmp58,sizeof(
char),0));char _tmp5A='\\';if(_get_dynforward_size(_tmp58,sizeof(char))== 1  && (
_tmp59 == '\000'  && _tmp5A != '\000'))_throw_arraybounds();*((char*)_tmp58.curr)=
_tmp5A;});({struct _dynforward_ptr _tmp5B=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp5C=*((char*)_check_dynforward_subscript(_tmp5B,sizeof(char),0));char
_tmp5D=(char)('0' + (_tmp1E >> 6 & 7));if(_get_dynforward_size(_tmp5B,sizeof(char))
== 1  && (_tmp5C == '\000'  && _tmp5D != '\000'))_throw_arraybounds();*((char*)
_tmp5B.curr)=_tmp5D;});({struct _dynforward_ptr _tmp5E=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp5F=*((char*)_check_dynforward_subscript(_tmp5E,sizeof(
char),0));char _tmp60=(char)('0' + (_tmp1E >> 3 & 7));if(_get_dynforward_size(_tmp5E,
sizeof(char))== 1  && (_tmp5F == '\000'  && _tmp60 != '\000'))_throw_arraybounds();*((
char*)_tmp5E.curr)=_tmp60;});({struct _dynforward_ptr _tmp61=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp62=*((char*)_check_dynforward_subscript(_tmp61,
sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 7));if(_get_dynforward_size(
_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'  && _tmp63 != '\000'))
_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}return(
struct _dynforward_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dynforward_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64 + 9};static char _tmp65[9]="volatile";
static struct _dynforward_ptr Cyc_Absynpp_volatile_string={_tmp65,_tmp65 + 9};static
char _tmp66[6]="const";static struct _dynforward_ptr Cyc_Absynpp_const_str={_tmp66,
_tmp66 + 6};static struct _dynforward_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dynforward_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dynforward_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct
Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=
0;if(tq.q_restrict)l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->hd=Cyc_Absynpp_restrict_sp;_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({
struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;
_tmp68->tl=l;_tmp68;});if(tq.print_const)l=({struct Cyc_List_List*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;_tmp69->tl=l;_tmp69;});
return Cyc_PP_egroup(({const char*_tmp6A="";_tag_dynforward(_tmp6A,sizeof(char),
_get_zero_arr_size(_tmp6A,1));}),({const char*_tmp6B=" ";_tag_dynforward(_tmp6B,
sizeof(char),_get_zero_arr_size(_tmp6B,2));}),({const char*_tmp6C=" ";
_tag_dynforward(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}struct _dynforward_ptr Cyc_Absynpp_kind2string(void*k){void*
_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: return({const char*_tmp6E="A";
_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,2));});_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: return({const char*_tmp6F="M";_tag_dynforward(
_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,2));});_LL3B: if((int)_tmp6D != 2)
goto _LL3D;_LL3C: return({const char*_tmp70="B";_tag_dynforward(_tmp70,sizeof(char),
_get_zero_arr_size(_tmp70,2));});_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: return({
const char*_tmp71="R";_tag_dynforward(_tmp71,sizeof(char),_get_zero_arr_size(
_tmp71,2));});_LL3F: if((int)_tmp6D != 4)goto _LL41;_LL40: return({const char*_tmp72="E";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,2));});_LL41: if((
int)_tmp6D != 5)goto _LL36;_LL42: return({const char*_tmp73="I";_tag_dynforward(
_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,2));});_LL36:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
struct _dynforward_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp74=Cyc_Absyn_compress_kb(
c);void*_tmp75;void*_tmp76;_LL44: if(*((int*)_tmp74)!= 0)goto _LL46;_tmp75=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp74)->f1;_LL45: return Cyc_Absynpp_kind2string(
_tmp75);_LL46: if(*((int*)_tmp74)!= 1)goto _LL48;_LL47: if(Cyc_PP_tex_output)return({
const char*_tmp77="{?}";_tag_dynforward(_tmp77,sizeof(char),_get_zero_arr_size(
_tmp77,4));});else{return({const char*_tmp78="?";_tag_dynforward(_tmp78,sizeof(
char),_get_zero_arr_size(_tmp78,2));});}_LL48: if(*((int*)_tmp74)!= 2)goto _LL43;
_tmp76=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp74)->f2;_LL49: return Cyc_Absynpp_kind2string(
_tmp76);_LL43:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp79=
Cyc_Absyn_compress_kb(c);void*_tmp7A;void*_tmp7B;_LL4B: if(*((int*)_tmp79)!= 0)
goto _LL4D;_tmp7A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp79)->f1;_LL4C: return
Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7A));_LL4D: if(*((int*)_tmp79)!= 1)goto
_LL4F;_LL4E: if(Cyc_PP_tex_output)return Cyc_PP_text_width(({const char*_tmp7C="{?}";
_tag_dynforward(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,4));}),1);else{
return Cyc_PP_text(({const char*_tmp7D="?";_tag_dynforward(_tmp7D,sizeof(char),
_get_zero_arr_size(_tmp7D,2));}));}_LL4F: if(*((int*)_tmp79)!= 2)goto _LL4A;_tmp7B=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp79)->f2;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(
_tmp7B));_LL4A:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return Cyc_PP_egroup(({const char*_tmp7E="<";_tag_dynforward(_tmp7E,sizeof(char),
_get_zero_arr_size(_tmp7E,2));}),({const char*_tmp7F=">";_tag_dynforward(_tmp7F,
sizeof(char),_get_zero_arr_size(_tmp7F,2));}),({const char*_tmp80=",";
_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp81=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp82;void*_tmp83;void*
_tmp84;_LL52: if(*((int*)_tmp81)!= 1)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)
_tmp81)!= 0)goto _LL56;_tmp82=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;
if((int)_tmp82 != 2)goto _LL56;_LL55: return Cyc_PP_textptr(tv->name);_LL56: if(*((
int*)_tmp81)!= 2)goto _LL58;_tmp83=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp81)->f2;_LL57: _tmp84=_tmp83;goto _LL59;_LL58: if(*((int*)_tmp81)!= 0)goto _LL51;
_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;_LL59: return({struct
Cyc_PP_Doc*_tmp85[3];_tmp85[2]=Cyc_Absynpp_kind2doc(_tmp84);_tmp85[1]=Cyc_PP_text(({
const char*_tmp86="::";_tag_dynforward(_tmp86,sizeof(char),_get_zero_arr_size(
_tmp86,3));}));_tmp85[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_dynforward(
_tmp85,sizeof(struct Cyc_PP_Doc*),3));});_LL51:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){return Cyc_PP_egroup(({const char*_tmp87="<";
_tag_dynforward(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,2));}),({const char*
_tmp88=">";_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,2));}),({
const char*_tmp89=",";_tag_dynforward(_tmp89,sizeof(char),_get_zero_arr_size(
_tmp89,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dynforward_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({const char*_tmp8A="<";
_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,2));}),({const char*
_tmp8B=">";_tag_dynforward(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,2));}),({
const char*_tmp8C=",";_tag_dynforward(_tmp8C,sizeof(char),_get_zero_arr_size(
_tmp8C,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct
Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmp8D="(";_tag_dynforward(_tmp8D,sizeof(char),
_get_zero_arr_size(_tmp8D,2));}),({const char*_tmp8E=")";_tag_dynforward(_tmp8E,
sizeof(char),_get_zero_arr_size(_tmp8E,2));}),({const char*_tmp8F=",";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp90=att;
_LL5B: if((int)_tmp90 != 0)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((int)_tmp90 != 1)goto
_LL5F;_LL5E: goto _LL60;_LL5F: if((int)_tmp90 != 2)goto _LL61;_LL60: return Cyc_PP_nil_doc();
_LL61:;_LL62: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL5A:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp91=(void*)atts->hd;_LL64: if((int)_tmp91 != 0)goto _LL66;
_LL65: return Cyc_PP_text(({const char*_tmp92=" _stdcall ";_tag_dynforward(_tmp92,
sizeof(char),_get_zero_arr_size(_tmp92,11));}));_LL66: if((int)_tmp91 != 1)goto
_LL68;_LL67: return Cyc_PP_text(({const char*_tmp93=" _cdecl ";_tag_dynforward(
_tmp93,sizeof(char),_get_zero_arr_size(_tmp93,9));}));_LL68: if((int)_tmp91 != 2)
goto _LL6A;_LL69: return Cyc_PP_text(({const char*_tmp94=" _fastcall ";
_tag_dynforward(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,12));}));_LL6A:;
_LL6B: goto _LL63;_LL63:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp95=(void*)atts2->hd;_LL6D: if((int)_tmp95 != 0)
goto _LL6F;_LL6E: goto _LL70;_LL6F: if((int)_tmp95 != 1)goto _LL71;_LL70: goto _LL72;
_LL71: if((int)_tmp95 != 2)goto _LL73;_LL72: goto _LL6C;_LL73:;_LL74: hasatt=1;goto
_LL6C;_LL6C:;}}if(!hasatt)return Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp96[
3];_tmp96[2]=Cyc_PP_text(({const char*_tmp9B=")";_tag_dynforward(_tmp9B,sizeof(
char),_get_zero_arr_size(_tmp9B,2));}));_tmp96[1]=Cyc_PP_group(({const char*
_tmp98="";_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,1));}),({
const char*_tmp99="";_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size(
_tmp99,1));}),({const char*_tmp9A=" ";_tag_dynforward(_tmp9A,sizeof(char),
_get_zero_arr_size(_tmp9A,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));
_tmp96[0]=Cyc_PP_text(({const char*_tmp97="__declspec(";_tag_dynforward(_tmp97,
sizeof(char),_get_zero_arr_size(_tmp97,12));}));Cyc_PP_cat(_tag_dynforward(
_tmp96,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp9C[2];
_tmp9C[1]=Cyc_PP_group(({const char*_tmp9E="((";_tag_dynforward(_tmp9E,sizeof(
char),_get_zero_arr_size(_tmp9E,3));}),({const char*_tmp9F="))";_tag_dynforward(
_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,3));}),({const char*_tmpA0=",";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp9C[0]=Cyc_PP_text(({const char*_tmp9D=" __attribute__";_tag_dynforward(
_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,15));}));Cyc_PP_cat(
_tag_dynforward(_tmp9C,sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpA1=(void*)tms->hd;_LL76:
if(*((int*)_tmpA1)!= 2)goto _LL78;_LL77: return 1;_LL78: if(*((int*)_tmpA1)!= 5)goto
_LL7A;_LL79: if(!Cyc_Absynpp_to_VC)return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);
_LL7A:;_LL7B: return 0;_LL75:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA2="{?}";_tag_dynforward(
_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,4));}),1);else{Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA3="?";_tag_dynforward(_tmpA3,
sizeof(char),_get_zero_arr_size(_tmpA3,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA4="{\\lb}";_tag_dynforward(_tmpA4,sizeof(char),_get_zero_arr_size(
_tmpA4,6));}),1);else{Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpA5="{";_tag_dynforward(_tmpA5,sizeof(char),_get_zero_arr_size(
_tmpA5,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA6="{\\rb}";_tag_dynforward(
_tmpA6,sizeof(char),_get_zero_arr_size(_tmpA6,6));}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA7="}";_tag_dynforward(_tmpA7,
sizeof(char),_get_zero_arr_size(_tmpA7,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA8="\\$";_tag_dynforward(_tmpA8,sizeof(char),_get_zero_arr_size(
_tmpA8,3));}),1);else{Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpA9="$";_tag_dynforward(_tmpA9,sizeof(char),_get_zero_arr_size(
_tmpA9,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dynforward_ptr sep,struct Cyc_List_List*
ss){return({struct Cyc_PP_Doc*_tmpAA[3];_tmpAA[2]=Cyc_Absynpp_rb();_tmpAA[1]=Cyc_PP_seq(
sep,ss);_tmpAA[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmpAA,sizeof(
struct Cyc_PP_Doc*),3));});}static void Cyc_Absynpp_print_tms(struct Cyc_List_List*
tms){for(0;tms != 0;tms=tms->tl){void*_tmpAB=(void*)tms->hd;void*_tmpAC;struct Cyc_List_List*
_tmpAD;_LL7D: if(*((int*)_tmpAB)!= 0)goto _LL7F;_LL7E:({void*_tmpAE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpAF="Carray_mod ";_tag_dynforward(_tmpAF,sizeof(char),
_get_zero_arr_size(_tmpAF,12));}),_tag_dynforward(_tmpAE,sizeof(void*),0));});
goto _LL7C;_LL7F: if(*((int*)_tmpAB)!= 1)goto _LL81;_LL80:({void*_tmpB0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpB1="ConstArray_mod ";_tag_dynforward(_tmpB1,sizeof(
char),_get_zero_arr_size(_tmpB1,16));}),_tag_dynforward(_tmpB0,sizeof(void*),0));});
goto _LL7C;_LL81: if(*((int*)_tmpAB)!= 3)goto _LL83;_tmpAC=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAB)->f1;if(*((int*)_tmpAC)!= 1)goto _LL83;_tmpAD=((struct Cyc_Absyn_WithTypes_struct*)
_tmpAC)->f1;_LL82:({void*_tmpB2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB3="Function_mod(";
_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,14));}),
_tag_dynforward(_tmpB2,sizeof(void*),0));});for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){
struct Cyc_Core_Opt*_tmpB4=(*((struct _tuple1*)_tmpAD->hd)).f1;if(_tmpB4 == 0)({
void*_tmpB5[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB6="?";_tag_dynforward(
_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,2));}),_tag_dynforward(_tmpB5,
sizeof(void*),0));});else{({void*_tmpB7[0]={};Cyc_fprintf(Cyc_stderr,*((struct
_dynforward_ptr*)_tmpB4->v),_tag_dynforward(_tmpB7,sizeof(void*),0));});}if(
_tmpAD->tl != 0)({void*_tmpB8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB9=",";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,2));}),
_tag_dynforward(_tmpB8,sizeof(void*),0));});}({void*_tmpBA[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBB=") ";_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(
_tmpBB,3));}),_tag_dynforward(_tmpBA,sizeof(void*),0));});goto _LL7C;_LL83: if(*((
int*)_tmpAB)!= 3)goto _LL85;_LL84:({void*_tmpBC[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBD="Function_mod()";_tag_dynforward(_tmpBD,sizeof(char),
_get_zero_arr_size(_tmpBD,15));}),_tag_dynforward(_tmpBC,sizeof(void*),0));});
goto _LL7C;_LL85: if(*((int*)_tmpAB)!= 5)goto _LL87;_LL86:({void*_tmpBE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpBF="Attributes_mod ";_tag_dynforward(_tmpBF,sizeof(
char),_get_zero_arr_size(_tmpBF,16));}),_tag_dynforward(_tmpBE,sizeof(void*),0));});
goto _LL7C;_LL87: if(*((int*)_tmpAB)!= 4)goto _LL89;_LL88:({void*_tmpC0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC1="TypeParams_mod ";_tag_dynforward(_tmpC1,sizeof(
char),_get_zero_arr_size(_tmpC1,16));}),_tag_dynforward(_tmpC0,sizeof(void*),0));});
goto _LL7C;_LL89: if(*((int*)_tmpAB)!= 2)goto _LL7C;_LL8A:({void*_tmpC2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC3="Pointer_mod ";_tag_dynforward(_tmpC3,sizeof(char),
_get_zero_arr_size(_tmpC3,13));}),_tag_dynforward(_tmpC2,sizeof(void*),0));});
goto _LL7C;_LL7C:;}({void*_tmpC4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpC5="\n";
_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,2));}),
_tag_dynforward(_tmpC4,sizeof(void*),0));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({
struct Cyc_PP_Doc*_tmp104[3];_tmp104[2]=Cyc_PP_text(({const char*_tmp106=")";
_tag_dynforward(_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,2));}));_tmp104[
1]=rest;_tmp104[0]=Cyc_PP_text(({const char*_tmp105="(";_tag_dynforward(_tmp105,
sizeof(char),_get_zero_arr_size(_tmp105,2));}));Cyc_PP_cat(_tag_dynforward(
_tmp104,sizeof(struct Cyc_PP_Doc*),3));});void*_tmpC6=(void*)tms->hd;struct Cyc_Absyn_Conref*
_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Conref*_tmpC9;void*_tmpCA;
struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCC;struct Cyc_Position_Segment*
_tmpCD;int _tmpCE;struct Cyc_Absyn_PtrAtts _tmpCF;void*_tmpD0;struct Cyc_Absyn_Conref*
_tmpD1;struct Cyc_Absyn_Conref*_tmpD2;struct Cyc_Absyn_Conref*_tmpD3;struct Cyc_Absyn_Tqual
_tmpD4;_LL8C: if(*((int*)_tmpC6)!= 0)goto _LL8E;_tmpC7=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpC6)->f1;_LL8D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmpD5[2];_tmpD5[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpC7)?Cyc_PP_text(({const char*_tmpD6="[]ZEROTERM ";_tag_dynforward(_tmpD6,
sizeof(char),_get_zero_arr_size(_tmpD6,12));})): Cyc_PP_text(({const char*_tmpD7="[]";
_tag_dynforward(_tmpD7,sizeof(char),_get_zero_arr_size(_tmpD7,3));}));_tmpD5[0]=
rest;Cyc_PP_cat(_tag_dynforward(_tmpD5,sizeof(struct Cyc_PP_Doc*),2));});_LL8E:
if(*((int*)_tmpC6)!= 1)goto _LL90;_tmpC8=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC6)->f1;_tmpC9=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpC6)->f2;_LL8F:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpD8[4];_tmpD8[3]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpC9)?Cyc_PP_text(({const char*_tmpDA="]ZEROTERM ";_tag_dynforward(_tmpDA,
sizeof(char),_get_zero_arr_size(_tmpDA,11));})): Cyc_PP_text(({const char*_tmpDB="]";
_tag_dynforward(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,2));}));_tmpD8[2]=
Cyc_Absynpp_exp2doc(_tmpC8);_tmpD8[1]=Cyc_PP_text(({const char*_tmpD9="[";
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,2));}));_tmpD8[0]=
rest;Cyc_PP_cat(_tag_dynforward(_tmpD8,sizeof(struct Cyc_PP_Doc*),4));});_LL90:
if(*((int*)_tmpC6)!= 3)goto _LL92;_tmpCA=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpC6)->f1;_LL91: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpDC=_tmpCA;struct Cyc_List_List*_tmpDD;int _tmpDE;struct Cyc_Absyn_VarargInfo*
_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*
_tmpE2;struct Cyc_Position_Segment*_tmpE3;_LL99: if(*((int*)_tmpDC)!= 1)goto _LL9B;
_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f1;_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f3;_tmpE0=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f4;_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f5;_LL9A: return({struct Cyc_PP_Doc*_tmpE4[2];_tmpE4[1]=Cyc_Absynpp_funargs2doc(
_tmpDD,_tmpDE,_tmpDF,_tmpE0,_tmpE1);_tmpE4[0]=rest;Cyc_PP_cat(_tag_dynforward(
_tmpE4,sizeof(struct Cyc_PP_Doc*),2));});_LL9B: if(*((int*)_tmpDC)!= 0)goto _LL98;
_tmpE2=((struct Cyc_Absyn_NoTypes_struct*)_tmpDC)->f1;_tmpE3=((struct Cyc_Absyn_NoTypes_struct*)
_tmpDC)->f2;_LL9C: return({struct Cyc_PP_Doc*_tmpE5[2];_tmpE5[1]=Cyc_PP_group(({
const char*_tmpE6="(";_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size(
_tmpE6,2));}),({const char*_tmpE7=")";_tag_dynforward(_tmpE7,sizeof(char),
_get_zero_arr_size(_tmpE7,2));}),({const char*_tmpE8=",";_tag_dynforward(_tmpE8,
sizeof(char),_get_zero_arr_size(_tmpE8,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,
_tmpE2));_tmpE5[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpE5,sizeof(struct Cyc_PP_Doc*),
2));});_LL98:;}_LL92: if(*((int*)_tmpC6)!= 5)goto _LL94;_tmpCB=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC6)->f2;_LL93: if(!Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;return({struct Cyc_PP_Doc*_tmpE9[2];_tmpE9[1]=Cyc_Absynpp_atts2doc(
_tmpCB);_tmpE9[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpE9,sizeof(struct Cyc_PP_Doc*),
2));});}else{if(Cyc_Absynpp_next_is_pointer(tms->tl))return({struct Cyc_PP_Doc*
_tmpEA[2];_tmpEA[1]=rest;_tmpEA[0]=Cyc_Absynpp_callconv2doc(_tmpCB);Cyc_PP_cat(
_tag_dynforward(_tmpEA,sizeof(struct Cyc_PP_Doc*),2));});return rest;}_LL94: if(*((
int*)_tmpC6)!= 4)goto _LL96;_tmpCC=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC6)->f1;_tmpCD=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC6)->f2;_tmpCE=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC6)->f3;_LL95: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpCE)return({struct Cyc_PP_Doc*_tmpEB[2];_tmpEB[1]=Cyc_Absynpp_ktvars2doc(
_tmpCC);_tmpEB[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEB,sizeof(struct Cyc_PP_Doc*),
2));});else{return({struct Cyc_PP_Doc*_tmpEC[2];_tmpEC[1]=Cyc_Absynpp_tvars2doc(
_tmpCC);_tmpEC[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEC,sizeof(struct Cyc_PP_Doc*),
2));});}_LL96: if(*((int*)_tmpC6)!= 2)goto _LL8B;_tmpCF=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC6)->f1;_tmpD0=(void*)_tmpCF.rgn;_tmpD1=_tmpCF.nullable;_tmpD2=_tmpCF.bounds;
_tmpD3=_tmpCF.zero_term;_tmpD4=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpC6)->f2;
_LL97: {struct Cyc_PP_Doc*ptr;{void*_tmpED=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD2);struct Cyc_Absyn_Exp*_tmpEE;void*_tmpEF;_LL9E: if((int)_tmpED != 0)goto
_LLA0;_LL9F: ptr=Cyc_Absynpp_question();goto _LL9D;_LLA0: if((int)_tmpED != 1)goto
_LLA2;_LLA1: ptr=({struct Cyc_PP_Doc*_tmpF0[2];_tmpF0[1]=Cyc_PP_text(({const char*
_tmpF1="-";_tag_dynforward(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,2));}));
_tmpF0[0]=Cyc_Absynpp_question();Cyc_PP_cat(_tag_dynforward(_tmpF0,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL9D;_LLA2: if(_tmpED <= (void*)2)goto _LLA4;if(*((int*)
_tmpED)!= 0)goto _LLA4;_tmpEE=((struct Cyc_Absyn_Upper_b_struct*)_tmpED)->f1;_LLA3:
ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,
_tmpD1)?({const char*_tmpF2="*";_tag_dynforward(_tmpF2,sizeof(char),
_get_zero_arr_size(_tmpF2,2));}):({const char*_tmpF3="@";_tag_dynforward(_tmpF3,
sizeof(char),_get_zero_arr_size(_tmpF3,2));}));{unsigned int _tmpF5;int _tmpF6;
struct _tuple4 _tmpF4=Cyc_Evexp_eval_const_uint_exp(_tmpEE);_tmpF5=_tmpF4.f1;
_tmpF6=_tmpF4.f2;if(!_tmpF6  || _tmpF5 != 1)ptr=({struct Cyc_PP_Doc*_tmpF7[4];
_tmpF7[3]=Cyc_Absynpp_rb();_tmpF7[2]=Cyc_Absynpp_exp2doc(_tmpEE);_tmpF7[1]=Cyc_Absynpp_lb();
_tmpF7[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmpF7,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL9D;}_LLA4: if(_tmpED <= (void*)2)goto _LL9D;if(*((int*)_tmpED)!= 1)goto _LL9D;
_tmpEF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmpED)->f1;_LLA5: ptr=Cyc_PP_text(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpD1)?({const char*
_tmpF8="*";_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,2));}):({
const char*_tmpF9="@";_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(
_tmpF9,2));}));ptr=({struct Cyc_PP_Doc*_tmpFA[4];_tmpFA[3]=Cyc_Absynpp_rb();
_tmpFA[2]=Cyc_Absynpp_typ2doc(_tmpEF);_tmpFA[1]=Cyc_Absynpp_lb();_tmpFA[0]=ptr;
Cyc_PP_cat(_tag_dynforward(_tmpFA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL9D;
_LL9D:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD3))ptr=({struct Cyc_PP_Doc*_tmpFB[2];_tmpFB[1]=
Cyc_PP_text(({const char*_tmpFC="ZEROTERM ";_tag_dynforward(_tmpFC,sizeof(char),
_get_zero_arr_size(_tmpFC,10));}));_tmpFB[0]=ptr;Cyc_PP_cat(_tag_dynforward(
_tmpFB,sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr  && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD3))ptr=({struct Cyc_PP_Doc*
_tmpFD[2];_tmpFD[1]=Cyc_PP_text(({const char*_tmpFE="NOZEROTERM ";_tag_dynforward(
_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,12));}));_tmpFD[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmpFD,sizeof(struct Cyc_PP_Doc*),2));});}}{void*_tmpFF=Cyc_Tcutil_compress(
_tmpD0);_LLA7: if((int)_tmpFF != 2)goto _LLA9;_LLA8: goto _LLA6;_LLA9:;_LLAA: ptr=({
struct Cyc_PP_Doc*_tmp100[3];_tmp100[2]=Cyc_PP_text(({const char*_tmp101=" ";
_tag_dynforward(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,2));}));_tmp100[
1]=Cyc_Absynpp_typ2doc(_tmpD0);_tmp100[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp100,
sizeof(struct Cyc_PP_Doc*),3));});_LLA6:;}ptr=({struct Cyc_PP_Doc*_tmp102[2];
_tmp102[1]=Cyc_Absynpp_tqual2doc(_tmpD4);_tmp102[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});return({struct Cyc_PP_Doc*
_tmp103[2];_tmp103[1]=rest;_tmp103[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp103,
sizeof(struct Cyc_PP_Doc*),2));});}_LL8B:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp107=Cyc_Tcutil_compress(t);_LLAC: if((int)_tmp107 != 2)goto _LLAE;
_LLAD: return Cyc_PP_text(({const char*_tmp108="`H";_tag_dynforward(_tmp108,sizeof(
char),_get_zero_arr_size(_tmp108,3));}));_LLAE: if((int)_tmp107 != 3)goto _LLB0;
_LLAF: return Cyc_PP_text(({const char*_tmp109="`U";_tag_dynforward(_tmp109,sizeof(
char),_get_zero_arr_size(_tmp109,3));}));_LLB0:;_LLB1: return Cyc_Absynpp_ntyp2doc(
t);_LLAB:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp10A=Cyc_Tcutil_compress(t);void*_tmp10B;
struct Cyc_List_List*_tmp10C;_LLB3: if(_tmp10A <= (void*)4)goto _LLB7;if(*((int*)
_tmp10A)!= 20)goto _LLB5;_tmp10B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp10A)->f1;_LLB4:*rgions=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*
_tmp10D));_tmp10D->hd=Cyc_Absynpp_rgn2doc(_tmp10B);_tmp10D->tl=*rgions;_tmp10D;});
goto _LLB2;_LLB5: if(*((int*)_tmp10A)!= 21)goto _LLB7;_tmp10C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp10A)->f1;_LLB6: for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp10C->hd);}goto _LLB2;_LLB7:;_LLB8:*effects=({struct Cyc_List_List*
_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=Cyc_Absynpp_typ2doc(t);_tmp10E->tl=*
effects;_tmp10E;});goto _LLB2;_LLB2:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp10F="";
_tag_dynforward(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,1));}),({const
char*_tmp110="";_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,
1));}),({const char*_tmp111="+";_tag_dynforward(_tmp111,sizeof(char),
_get_zero_arr_size(_tmp111,2));}),effects);else{struct Cyc_PP_Doc*_tmp112=Cyc_Absynpp_group_braces(({
const char*_tmp117=",";_tag_dynforward(_tmp117,sizeof(char),_get_zero_arr_size(
_tmp117,2));}),rgions);return Cyc_PP_group(({const char*_tmp113="";_tag_dynforward(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,1));}),({const char*_tmp114="";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,1));}),({const
char*_tmp115="+";_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,
2));}),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*
_tmp116));_tmp116->hd=_tmp112;_tmp116->tl=0;_tmp116;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmp118=k;_LLBA: if((int)_tmp118 != 0)goto _LLBC;_LLBB: return Cyc_PP_text(({
const char*_tmp119="struct ";_tag_dynforward(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,8));}));_LLBC: if((int)_tmp118 != 1)goto _LLB9;_LLBD:
return Cyc_PP_text(({const char*_tmp11A="union ";_tag_dynforward(_tmp11A,sizeof(
char),_get_zero_arr_size(_tmp11A,7));}));_LLB9:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t){struct Cyc_PP_Doc*s;{void*_tmp11B=t;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Core_Opt*
_tmp11D;int _tmp11E;struct Cyc_Core_Opt*_tmp11F;struct Cyc_Absyn_Tvar*_tmp120;
struct Cyc_Absyn_TunionInfo _tmp121;union Cyc_Absyn_TunionInfoU_union _tmp122;struct
Cyc_List_List*_tmp123;struct Cyc_Core_Opt*_tmp124;struct Cyc_Absyn_TunionFieldInfo
_tmp125;union Cyc_Absyn_TunionFieldInfoU_union _tmp126;struct Cyc_List_List*_tmp127;
void*_tmp128;void*_tmp129;int _tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_Absyn_AggrInfo
_tmp12C;union Cyc_Absyn_AggrInfoU_union _tmp12D;struct Cyc_List_List*_tmp12E;void*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;struct _tuple0*
_tmp132;void*_tmp133;struct _tuple0*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_Typedefdecl*
_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;int _tmp13A;void*_tmp13B;void*
_tmp13C;_LLBF: if(_tmp11B <= (void*)4)goto _LLC5;if(*((int*)_tmp11B)!= 7)goto _LLC1;
_LLC0: return Cyc_PP_text(({const char*_tmp13D="[[[array]]]";_tag_dynforward(
_tmp13D,sizeof(char),_get_zero_arr_size(_tmp13D,12));}));_LLC1: if(*((int*)
_tmp11B)!= 8)goto _LLC3;_LLC2: return Cyc_PP_nil_doc();_LLC3: if(*((int*)_tmp11B)!= 
4)goto _LLC5;_LLC4: return Cyc_PP_nil_doc();_LLC5: if((int)_tmp11B != 0)goto _LLC7;
_LLC6: s=Cyc_PP_text(({const char*_tmp13E="void";_tag_dynforward(_tmp13E,sizeof(
char),_get_zero_arr_size(_tmp13E,5));}));goto _LLBE;_LLC7: if(_tmp11B <= (void*)4)
goto _LLD1;if(*((int*)_tmp11B)!= 0)goto _LLC9;_tmp11C=((struct Cyc_Absyn_Evar_struct*)
_tmp11B)->f1;_tmp11D=((struct Cyc_Absyn_Evar_struct*)_tmp11B)->f2;_tmp11E=((
struct Cyc_Absyn_Evar_struct*)_tmp11B)->f3;_tmp11F=((struct Cyc_Absyn_Evar_struct*)
_tmp11B)->f4;_LLC8: if(_tmp11D != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11D->v);
else{void*_tmp13F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp11C))->v;_LLF6:
if((int)_tmp13F != 3)goto _LLF8;_LLF7: s=({struct Cyc_PP_Doc*_tmp140[2];_tmp140[1]=
Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp144;_tmp144.tag=
1;_tmp144.f1=(unsigned long)_tmp11E;{void*_tmp142[1]={& _tmp144};Cyc_aprintf(({
const char*_tmp143="%d";_tag_dynforward(_tmp143,sizeof(char),_get_zero_arr_size(
_tmp143,3));}),_tag_dynforward(_tmp142,sizeof(void*),1));}}));_tmp140[0]=Cyc_PP_text(({
const char*_tmp141="`r";_tag_dynforward(_tmp141,sizeof(char),_get_zero_arr_size(
_tmp141,3));}));Cyc_PP_cat(_tag_dynforward(_tmp140,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLF5;_LLF8:;_LLF9: s=({struct Cyc_PP_Doc*_tmp145[6];_tmp145[5]=_tmp11C == 0?
Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((void*)_tmp11C->v);_tmp145[4]=Cyc_PP_text(({
const char*_tmp14B=")::";_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(
_tmp14B,4));}));_tmp145[3]=(!Cyc_Absynpp_print_full_evars  || _tmp11F == 0)?Cyc_PP_nil_doc():
Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp11F->v);_tmp145[2]=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp14A;_tmp14A.tag=1;_tmp14A.f1=(
unsigned long)_tmp11E;{void*_tmp148[1]={& _tmp14A};Cyc_aprintf(({const char*
_tmp149="%d";_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,3));}),
_tag_dynforward(_tmp148,sizeof(void*),1));}}));_tmp145[1]=Cyc_PP_text(({const
char*_tmp147="(";_tag_dynforward(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,
2));}));_tmp145[0]=Cyc_PP_text(({const char*_tmp146="%";_tag_dynforward(_tmp146,
sizeof(char),_get_zero_arr_size(_tmp146,2));}));Cyc_PP_cat(_tag_dynforward(
_tmp145,sizeof(struct Cyc_PP_Doc*),6));});_LLF5:;}goto _LLBE;_LLC9: if(*((int*)
_tmp11B)!= 1)goto _LLCB;_tmp120=((struct Cyc_Absyn_VarType_struct*)_tmp11B)->f1;
_LLCA: s=Cyc_PP_textptr(_tmp120->name);if(Cyc_Absynpp_print_all_kinds)s=({struct
Cyc_PP_Doc*_tmp14C[3];_tmp14C[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp120->kind);
_tmp14C[1]=Cyc_PP_text(({const char*_tmp14D="::";_tag_dynforward(_tmp14D,sizeof(
char),_get_zero_arr_size(_tmp14D,3));}));_tmp14C[0]=s;Cyc_PP_cat(_tag_dynforward(
_tmp14C,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars  && 
Cyc_Tcutil_is_temp_tvar(_tmp120))s=({struct Cyc_PP_Doc*_tmp14E[3];_tmp14E[2]=Cyc_PP_text(({
const char*_tmp150=" */";_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(
_tmp150,4));}));_tmp14E[1]=s;_tmp14E[0]=Cyc_PP_text(({const char*_tmp14F="_ /* ";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,6));}));Cyc_PP_cat(
_tag_dynforward(_tmp14E,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBE;_LLCB: if(*((
int*)_tmp11B)!= 2)goto _LLCD;_tmp121=((struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1;
_tmp122=_tmp121.tunion_info;_tmp123=_tmp121.targs;_tmp124=_tmp121.rgn;_LLCC:{
union Cyc_Absyn_TunionInfoU_union _tmp151=_tmp122;struct Cyc_Absyn_UnknownTunionInfo
_tmp152;struct _tuple0*_tmp153;int _tmp154;struct Cyc_Absyn_Tuniondecl**_tmp155;
struct Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Tuniondecl _tmp157;struct
_tuple0*_tmp158;int _tmp159;_LLFB: if((_tmp151.UnknownTunion).tag != 0)goto _LLFD;
_tmp152=(_tmp151.UnknownTunion).f1;_tmp153=_tmp152.name;_tmp154=_tmp152.is_xtunion;
_LLFC: _tmp158=_tmp153;_tmp159=_tmp154;goto _LLFE;_LLFD: if((_tmp151.KnownTunion).tag
!= 1)goto _LLFA;_tmp155=(_tmp151.KnownTunion).f1;_tmp156=*_tmp155;_tmp157=*
_tmp156;_tmp158=_tmp157.name;_tmp159=_tmp157.is_xtunion;_LLFE: {struct Cyc_PP_Doc*
_tmp15A=Cyc_PP_text(_tmp159?({const char*_tmp15F="xtunion ";_tag_dynforward(
_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,9));}):({const char*_tmp160="tunion ";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,8));}));void*r=(
unsigned int)_tmp124?(void*)_tmp124->v:(void*)2;{void*_tmp15B=Cyc_Tcutil_compress(
r);_LL100: if((int)_tmp15B != 2)goto _LL102;_LL101: s=({struct Cyc_PP_Doc*_tmp15C[3];
_tmp15C[2]=Cyc_Absynpp_tps2doc(_tmp123);_tmp15C[1]=Cyc_Absynpp_qvar2doc(_tmp158);
_tmp15C[0]=_tmp15A;Cyc_PP_cat(_tag_dynforward(_tmp15C,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLFF;_LL102:;_LL103: s=({struct Cyc_PP_Doc*_tmp15D[5];_tmp15D[4]=Cyc_Absynpp_tps2doc(
_tmp123);_tmp15D[3]=Cyc_Absynpp_qvar2doc(_tmp158);_tmp15D[2]=Cyc_PP_text(({const
char*_tmp15E=" ";_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,
2));}));_tmp15D[1]=Cyc_Absynpp_typ2doc(r);_tmp15D[0]=_tmp15A;Cyc_PP_cat(
_tag_dynforward(_tmp15D,sizeof(struct Cyc_PP_Doc*),5));});goto _LLFF;_LLFF:;}goto
_LLFA;}_LLFA:;}goto _LLBE;_LLCD: if(*((int*)_tmp11B)!= 3)goto _LLCF;_tmp125=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp11B)->f1;_tmp126=_tmp125.field_info;
_tmp127=_tmp125.targs;_LLCE:{union Cyc_Absyn_TunionFieldInfoU_union _tmp161=
_tmp126;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp162;struct _tuple0*_tmp163;
struct _tuple0*_tmp164;int _tmp165;struct Cyc_Absyn_Tuniondecl*_tmp166;struct Cyc_Absyn_Tuniondecl
_tmp167;struct _tuple0*_tmp168;int _tmp169;struct Cyc_Absyn_Tunionfield*_tmp16A;
struct Cyc_Absyn_Tunionfield _tmp16B;struct _tuple0*_tmp16C;_LL105: if((_tmp161.UnknownTunionfield).tag
!= 0)goto _LL107;_tmp162=(_tmp161.UnknownTunionfield).f1;_tmp163=_tmp162.tunion_name;
_tmp164=_tmp162.field_name;_tmp165=_tmp162.is_xtunion;_LL106: _tmp168=_tmp163;
_tmp169=_tmp165;_tmp16C=_tmp164;goto _LL108;_LL107: if((_tmp161.KnownTunionfield).tag
!= 1)goto _LL104;_tmp166=(_tmp161.KnownTunionfield).f1;_tmp167=*_tmp166;_tmp168=
_tmp167.name;_tmp169=_tmp167.is_xtunion;_tmp16A=(_tmp161.KnownTunionfield).f2;
_tmp16B=*_tmp16A;_tmp16C=_tmp16B.name;_LL108: {struct Cyc_PP_Doc*_tmp16D=Cyc_PP_text(
_tmp169?({const char*_tmp170="xtunion ";_tag_dynforward(_tmp170,sizeof(char),
_get_zero_arr_size(_tmp170,9));}):({const char*_tmp171="tunion ";_tag_dynforward(
_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,8));}));s=({struct Cyc_PP_Doc*
_tmp16E[4];_tmp16E[3]=Cyc_Absynpp_qvar2doc(_tmp16C);_tmp16E[2]=Cyc_PP_text(({
const char*_tmp16F=".";_tag_dynforward(_tmp16F,sizeof(char),_get_zero_arr_size(
_tmp16F,2));}));_tmp16E[1]=Cyc_Absynpp_qvar2doc(_tmp168);_tmp16E[0]=_tmp16D;Cyc_PP_cat(
_tag_dynforward(_tmp16E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL104;}_LL104:;}
goto _LLBE;_LLCF: if(*((int*)_tmp11B)!= 5)goto _LLD1;_tmp128=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp11B)->f1;_tmp129=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11B)->f2;_LLD0: {
struct _dynforward_ptr sns;struct _dynforward_ptr ts;{void*_tmp172=_tmp128;_LL10A:
if((int)_tmp172 != 2)goto _LL10C;_LL10B: goto _LL10D;_LL10C: if((int)_tmp172 != 0)goto
_LL10E;_LL10D: sns=({const char*_tmp173="";_tag_dynforward(_tmp173,sizeof(char),
_get_zero_arr_size(_tmp173,1));});goto _LL109;_LL10E: if((int)_tmp172 != 1)goto
_LL109;_LL10F: sns=({const char*_tmp174="unsigned ";_tag_dynforward(_tmp174,
sizeof(char),_get_zero_arr_size(_tmp174,10));});goto _LL109;_LL109:;}{void*
_tmp175=_tmp129;_LL111: if((int)_tmp175 != 0)goto _LL113;_LL112:{void*_tmp176=
_tmp128;_LL11C: if((int)_tmp176 != 2)goto _LL11E;_LL11D: sns=({const char*_tmp177="";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,1));});goto
_LL11B;_LL11E: if((int)_tmp176 != 0)goto _LL120;_LL11F: sns=({const char*_tmp178="signed ";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,8));});goto
_LL11B;_LL120: if((int)_tmp176 != 1)goto _LL11B;_LL121: sns=({const char*_tmp179="unsigned ";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,10));});goto
_LL11B;_LL11B:;}ts=({const char*_tmp17A="char";_tag_dynforward(_tmp17A,sizeof(
char),_get_zero_arr_size(_tmp17A,5));});goto _LL110;_LL113: if((int)_tmp175 != 1)
goto _LL115;_LL114: ts=({const char*_tmp17B="short";_tag_dynforward(_tmp17B,sizeof(
char),_get_zero_arr_size(_tmp17B,6));});goto _LL110;_LL115: if((int)_tmp175 != 2)
goto _LL117;_LL116: ts=({const char*_tmp17C="int";_tag_dynforward(_tmp17C,sizeof(
char),_get_zero_arr_size(_tmp17C,4));});goto _LL110;_LL117: if((int)_tmp175 != 3)
goto _LL119;_LL118: ts=({const char*_tmp17D="long";_tag_dynforward(_tmp17D,sizeof(
char),_get_zero_arr_size(_tmp17D,5));});goto _LL110;_LL119: if((int)_tmp175 != 4)
goto _LL110;_LL11A: ts=Cyc_Absynpp_to_VC?({const char*_tmp17E="__int64";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,8));}):({const
char*_tmp17F="long long";_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size(
_tmp17F,10));});goto _LL110;_LL110:;}s=Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ts);{struct Cyc_String_pa_struct _tmp182;
_tmp182.tag=0;_tmp182.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)sns);{
void*_tmp180[2]={& _tmp182,& _tmp183};Cyc_aprintf(({const char*_tmp181="%s%s";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,5));}),
_tag_dynforward(_tmp180,sizeof(void*),2));}}}));goto _LLBE;}_LLD1: if((int)_tmp11B
!= 1)goto _LLD3;_LLD2: s=Cyc_PP_text(({const char*_tmp184="float";_tag_dynforward(
_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,6));}));goto _LLBE;_LLD3: if(
_tmp11B <= (void*)4)goto _LLEB;if(*((int*)_tmp11B)!= 6)goto _LLD5;_tmp12A=((struct
Cyc_Absyn_DoubleType_struct*)_tmp11B)->f1;_LLD4: if(_tmp12A)s=Cyc_PP_text(({const
char*_tmp185="long double";_tag_dynforward(_tmp185,sizeof(char),
_get_zero_arr_size(_tmp185,12));}));else{s=Cyc_PP_text(({const char*_tmp186="double";
_tag_dynforward(_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,7));}));}goto
_LLBE;_LLD5: if(*((int*)_tmp11B)!= 9)goto _LLD7;_tmp12B=((struct Cyc_Absyn_TupleType_struct*)
_tmp11B)->f1;_LLD6: s=({struct Cyc_PP_Doc*_tmp187[2];_tmp187[1]=Cyc_Absynpp_args2doc(
_tmp12B);_tmp187[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp187,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBE;_LLD7: if(*((int*)_tmp11B)!= 10)goto
_LLD9;_tmp12C=((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1;_tmp12D=_tmp12C.aggr_info;
_tmp12E=_tmp12C.targs;_LLD8: {void*_tmp189;struct _tuple0*_tmp18A;struct _tuple3
_tmp188=Cyc_Absyn_aggr_kinded_name(_tmp12D);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;
s=({struct Cyc_PP_Doc*_tmp18B[3];_tmp18B[2]=Cyc_Absynpp_tps2doc(_tmp12E);_tmp18B[
1]=Cyc_Absynpp_qvar2doc(_tmp18A);_tmp18B[0]=Cyc_Absynpp_aggr_kind2doc(_tmp189);
Cyc_PP_cat(_tag_dynforward(_tmp18B,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBE;}
_LLD9: if(*((int*)_tmp11B)!= 11)goto _LLDB;_tmp12F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11B)->f1;_tmp130=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11B)->f2;_LLDA: s=({
struct Cyc_PP_Doc*_tmp18C[4];_tmp18C[3]=Cyc_Absynpp_rb();_tmp18C[2]=Cyc_PP_nest(2,
Cyc_Absynpp_aggrfields2doc(_tmp130));_tmp18C[1]=Cyc_Absynpp_lb();_tmp18C[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp12F);Cyc_PP_cat(_tag_dynforward(_tmp18C,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLBE;_LLDB: if(*((int*)_tmp11B)!= 13)goto _LLDD;_tmp131=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLDC: s=({struct Cyc_PP_Doc*_tmp18D[4];_tmp18D[3]=Cyc_Absynpp_rb();
_tmp18D[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp131));_tmp18D[1]=Cyc_Absynpp_lb();
_tmp18D[0]=Cyc_PP_text(({const char*_tmp18E="enum ";_tag_dynforward(_tmp18E,
sizeof(char),_get_zero_arr_size(_tmp18E,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp18D,sizeof(struct Cyc_PP_Doc*),4));});goto _LLBE;_LLDD: if(*((int*)_tmp11B)!= 
12)goto _LLDF;_tmp132=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;_LLDE: s=({
struct Cyc_PP_Doc*_tmp18F[2];_tmp18F[1]=Cyc_Absynpp_qvar2doc(_tmp132);_tmp18F[0]=
Cyc_PP_text(({const char*_tmp190="enum ";_tag_dynforward(_tmp190,sizeof(char),
_get_zero_arr_size(_tmp190,6));}));Cyc_PP_cat(_tag_dynforward(_tmp18F,sizeof(
struct Cyc_PP_Doc*),2));});goto _LLBE;_LLDF: if(*((int*)_tmp11B)!= 14)goto _LLE1;
_tmp133=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11B)->f1;_LLE0: s=({
struct Cyc_PP_Doc*_tmp191[3];_tmp191[2]=Cyc_PP_text(({const char*_tmp193=">";
_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,2));}));_tmp191[
1]=Cyc_Absynpp_typ2doc(_tmp133);_tmp191[0]=Cyc_PP_text(({const char*_tmp192="sizeof_t<";
_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp191,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBE;_LLE1: if(*((
int*)_tmp11B)!= 17)goto _LLE3;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11B)->f1;_tmp135=((struct Cyc_Absyn_TypedefType_struct*)_tmp11B)->f2;_tmp136=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11B)->f3;_LLE2: s=({struct Cyc_PP_Doc*
_tmp194[2];_tmp194[1]=Cyc_Absynpp_tps2doc(_tmp135);_tmp194[0]=Cyc_Absynpp_qvar2doc(
_tmp134);Cyc_PP_cat(_tag_dynforward(_tmp194,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBE;_LLE3: if(*((int*)_tmp11B)!= 15)goto _LLE5;_tmp137=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11B)->f1;_LLE4: s=({struct Cyc_PP_Doc*_tmp195[3];_tmp195[2]=Cyc_PP_text(({
const char*_tmp197=">";_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size(
_tmp197,2));}));_tmp195[1]=Cyc_Absynpp_rgn2doc(_tmp137);_tmp195[0]=Cyc_PP_text(({
const char*_tmp196="region_t<";_tag_dynforward(_tmp196,sizeof(char),
_get_zero_arr_size(_tmp196,10));}));Cyc_PP_cat(_tag_dynforward(_tmp195,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLBE;_LLE5: if(*((int*)_tmp11B)!= 16)goto _LLE7;
_tmp138=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11B)->f1;_tmp139=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp11B)->f2;_LLE6: s=({struct Cyc_PP_Doc*
_tmp198[5];_tmp198[4]=Cyc_PP_text(({const char*_tmp19B=">";_tag_dynforward(
_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,2));}));_tmp198[3]=Cyc_Absynpp_rgn2doc(
_tmp139);_tmp198[2]=Cyc_PP_text(({const char*_tmp19A=",";_tag_dynforward(_tmp19A,
sizeof(char),_get_zero_arr_size(_tmp19A,2));}));_tmp198[1]=Cyc_Absynpp_rgn2doc(
_tmp138);_tmp198[0]=Cyc_PP_text(({const char*_tmp199="dynregion_t<";
_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size(_tmp199,13));}));Cyc_PP_cat(
_tag_dynforward(_tmp198,sizeof(struct Cyc_PP_Doc*),5));});goto _LLBE;_LLE7: if(*((
int*)_tmp11B)!= 19)goto _LLE9;_tmp13A=((struct Cyc_Absyn_TypeInt_struct*)_tmp11B)->f1;
_LLE8: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp19E;
_tmp19E.tag=1;_tmp19E.f1=(unsigned long)_tmp13A;{void*_tmp19C[1]={& _tmp19E};Cyc_aprintf(({
const char*_tmp19D="`%d";_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size(
_tmp19D,4));}),_tag_dynforward(_tmp19C,sizeof(void*),1));}}));goto _LLBE;_LLE9:
if(*((int*)_tmp11B)!= 18)goto _LLEB;_tmp13B=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11B)->f1;_LLEA: s=({struct Cyc_PP_Doc*_tmp19F[3];_tmp19F[2]=Cyc_PP_text(({
const char*_tmp1A1=">";_tag_dynforward(_tmp1A1,sizeof(char),_get_zero_arr_size(
_tmp1A1,2));}));_tmp19F[1]=Cyc_Absynpp_typ2doc(_tmp13B);_tmp19F[0]=Cyc_PP_text(({
const char*_tmp1A0="tag_t<";_tag_dynforward(_tmp1A0,sizeof(char),
_get_zero_arr_size(_tmp1A0,7));}));Cyc_PP_cat(_tag_dynforward(_tmp19F,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLBE;_LLEB: if((int)_tmp11B != 3)goto _LLED;_LLEC:
goto _LLEE;_LLED: if((int)_tmp11B != 2)goto _LLEF;_LLEE: s=Cyc_Absynpp_rgn2doc(t);
goto _LLBE;_LLEF: if(_tmp11B <= (void*)4)goto _LLF1;if(*((int*)_tmp11B)!= 22)goto
_LLF1;_tmp13C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11B)->f1;_LLF0: s=({
struct Cyc_PP_Doc*_tmp1A2[3];_tmp1A2[2]=Cyc_PP_text(({const char*_tmp1A4=")";
_tag_dynforward(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,2));}));_tmp1A2[
1]=Cyc_Absynpp_typ2doc(_tmp13C);_tmp1A2[0]=Cyc_PP_text(({const char*_tmp1A3="regions(";
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp1A2,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBE;_LLF1: if(
_tmp11B <= (void*)4)goto _LLF3;if(*((int*)_tmp11B)!= 20)goto _LLF3;_LLF2: goto _LLF4;
_LLF3: if(_tmp11B <= (void*)4)goto _LLBE;if(*((int*)_tmp11B)!= 21)goto _LLBE;_LLF4: s=
Cyc_Absynpp_eff2doc(t);goto _LLBE;_LLBE:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dynforward_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*
cmp){struct _tuple5 _tmp1A6;void*_tmp1A7;void*_tmp1A8;struct _tuple5*_tmp1A5=cmp;
_tmp1A6=*_tmp1A5;_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;return({struct Cyc_PP_Doc*
_tmp1A9[3];_tmp1A9[2]=Cyc_Absynpp_rgn2doc(_tmp1A8);_tmp1A9[1]=Cyc_PP_text(({
const char*_tmp1AA=" > ";_tag_dynforward(_tmp1AA,sizeof(char),_get_zero_arr_size(
_tmp1AA,4));}));_tmp1A9[0]=Cyc_Absynpp_rgn2doc(_tmp1A7);Cyc_PP_cat(
_tag_dynforward(_tmp1A9,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(({const char*_tmp1AB="";
_tag_dynforward(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,1));}),({const
char*_tmp1AC="";_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,
1));}),({const char*_tmp1AD=",";_tag_dynforward(_tmp1AD,sizeof(char),
_get_zero_arr_size(_tmp1AD,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*
t).f1 == 0?0:({struct Cyc_Core_Opt*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->v=
Cyc_PP_text(*((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v));
_tmp1AE;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs)_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp1AF,({struct Cyc_List_List*_tmp1B0=
_cycalloc(sizeof(*_tmp1B0));_tmp1B0->hd=Cyc_PP_text(({const char*_tmp1B1="...";
_tag_dynforward(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,4));}));_tmp1B0->tl=
0;_tmp1B0;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmp1B2=({struct Cyc_PP_Doc*
_tmp1B4[3];_tmp1B4[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmp1B8=_cycalloc(
sizeof(*_tmp1B8));_tmp1B8->f1=cyc_varargs->name;_tmp1B8->f2=cyc_varargs->tq;
_tmp1B8->f3=(void*)cyc_varargs->type;_tmp1B8;}));_tmp1B4[1]=cyc_varargs->inject?
Cyc_PP_text(({const char*_tmp1B6=" inject ";_tag_dynforward(_tmp1B6,sizeof(char),
_get_zero_arr_size(_tmp1B6,9));})): Cyc_PP_text(({const char*_tmp1B7=" ";
_tag_dynforward(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,2));}));_tmp1B4[
0]=Cyc_PP_text(({const char*_tmp1B5="...";_tag_dynforward(_tmp1B5,sizeof(char),
_get_zero_arr_size(_tmp1B5,4));}));Cyc_PP_cat(_tag_dynforward(_tmp1B4,sizeof(
struct Cyc_PP_Doc*),3));});_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AF,({struct Cyc_List_List*_tmp1B3=
_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=_tmp1B2;_tmp1B3->tl=0;_tmp1B3;}));}}{
struct Cyc_PP_Doc*_tmp1B9=Cyc_PP_group(({const char*_tmp1C1="";_tag_dynforward(
_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,1));}),({const char*_tmp1C2="";
_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,1));}),({const
char*_tmp1C3=",";_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,
2));}),_tmp1AF);if(effopt != 0  && Cyc_Absynpp_print_all_effects)_tmp1B9=({struct
Cyc_PP_Doc*_tmp1BA[3];_tmp1BA[2]=Cyc_Absynpp_eff2doc((void*)effopt->v);_tmp1BA[1]=
Cyc_PP_text(({const char*_tmp1BB=";";_tag_dynforward(_tmp1BB,sizeof(char),
_get_zero_arr_size(_tmp1BB,2));}));_tmp1BA[0]=_tmp1B9;Cyc_PP_cat(_tag_dynforward(
_tmp1BA,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp1B9=({struct Cyc_PP_Doc*
_tmp1BC[3];_tmp1BC[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1BC[1]=Cyc_PP_text(({
const char*_tmp1BD=":";_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(
_tmp1BD,2));}));_tmp1BC[0]=_tmp1B9;Cyc_PP_cat(_tag_dynforward(_tmp1BC,sizeof(
struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmp1BE[3];_tmp1BE[2]=Cyc_PP_text(({
const char*_tmp1C0=")";_tag_dynforward(_tmp1C0,sizeof(char),_get_zero_arr_size(
_tmp1C0,2));}));_tmp1BE[1]=_tmp1B9;_tmp1BE[0]=Cyc_PP_text(({const char*_tmp1BF="(";
_tag_dynforward(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp1BE,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){return({struct _tuple1*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));
_tmp1C4->f1=({struct Cyc_Core_Opt*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->v=(*
arg).f1;_tmp1C5;});_tmp1C4->f2=(*arg).f2;_tmp1C4->f3=(*arg).f3;_tmp1C4;});}
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dynforward_ptr*v){return Cyc_PP_text(*
v);}struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1C6=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1C7=(*q).f1;struct Cyc_List_List*
_tmp1C8;struct Cyc_List_List*_tmp1C9;_LL123: if((_tmp1C7.Loc_n).tag != 0)goto _LL125;
_LL124: _tmp1C8=0;goto _LL126;_LL125: if((_tmp1C7.Rel_n).tag != 1)goto _LL127;_tmp1C8=(
_tmp1C7.Rel_n).f1;_LL126: match=0;_tmp1C6=_tmp1C8;goto _LL122;_LL127: if((_tmp1C7.Abs_n).tag
!= 2)goto _LL122;_tmp1C9=(_tmp1C7.Abs_n).f1;_LL128: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C9,Cyc_Absynpp_curr_namespace);
_tmp1C6=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1CA->tl=_tmp1C9;_tmp1CA;}): _tmp1C9;goto _LL122;_LL122:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dynforward_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C6,({struct Cyc_List_List*_tmp1CB=
_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=(*q).f2;_tmp1CB->tl=0;_tmp1CB;})),({
const char*_tmp1CC="_";_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(
_tmp1CC,2));}));else{if(match)return*(*q).f2;else{return(struct _dynforward_ptr)
Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1C6,({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*
_tmp1CD));_tmp1CD->hd=(*q).f2;_tmp1CD->tl=0;_tmp1CD;})),({const char*_tmp1CE="::";
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,3));}));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct
_dynforward_ptr _tmp1CF=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)Cyc_strconcat(({const
char*_tmp1D0="\\textbf{";_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size(
_tmp1D0,9));}),(struct _dynforward_ptr)_tmp1CF),({const char*_tmp1D1="}";
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(_tmp1D1,2));})),(int)Cyc_strlen((
struct _dynforward_ptr)_tmp1CF));else{return Cyc_PP_text(_tmp1CF);}}struct
_dynforward_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union
_tmp1D2=(*v).f1;struct Cyc_List_List*_tmp1D3;struct Cyc_List_List*_tmp1D4;_LL12A:
if((_tmp1D2.Loc_n).tag != 0)goto _LL12C;_LL12B: goto _LL12D;_LL12C: if((_tmp1D2.Rel_n).tag
!= 1)goto _LL12E;_tmp1D3=(_tmp1D2.Rel_n).f1;if(_tmp1D3 != 0)goto _LL12E;_LL12D:
return*(*v).f2;_LL12E: if((_tmp1D2.Abs_n).tag != 2)goto _LL130;_tmp1D4=(_tmp1D2.Abs_n).f1;
_LL12F: if(((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp1D4,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL131;}_LL130:;
_LL131: return(struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D5="/* bad namespace : */ ";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,23));}),(struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(v));_LL129:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dynforward_ptr _tmp1D6=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D7="\\textbf{";
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,9));}),(struct
_dynforward_ptr)_tmp1D6),({const char*_tmp1D8="}";_tag_dynforward(_tmp1D8,sizeof(
char),_get_zero_arr_size(_tmp1D8,2));})),(int)Cyc_strlen((struct _dynforward_ptr)
_tmp1D6));else{return Cyc_PP_text(_tmp1D6);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp1D9=(void*)e->r;void*_tmp1DA;struct Cyc_Absyn_Exp*
_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;_LL133: if(*((int*)_tmp1D9)!= 0)goto _LL135;
_LL134: goto _LL136;_LL135: if(*((int*)_tmp1D9)!= 1)goto _LL137;_LL136: goto _LL138;
_LL137: if(*((int*)_tmp1D9)!= 2)goto _LL139;_LL138: return 10000;_LL139: if(*((int*)
_tmp1D9)!= 3)goto _LL13B;_tmp1DA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D9)->f1;_LL13A: {void*_tmp1DD=_tmp1DA;_LL180: if((int)_tmp1DD != 0)goto _LL182;
_LL181: return 100;_LL182: if((int)_tmp1DD != 1)goto _LL184;_LL183: return 110;_LL184:
if((int)_tmp1DD != 2)goto _LL186;_LL185: return 100;_LL186: if((int)_tmp1DD != 3)goto
_LL188;_LL187: goto _LL189;_LL188: if((int)_tmp1DD != 4)goto _LL18A;_LL189: return 110;
_LL18A: if((int)_tmp1DD != 5)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if((int)_tmp1DD != 
6)goto _LL18E;_LL18D: return 70;_LL18E: if((int)_tmp1DD != 7)goto _LL190;_LL18F: goto
_LL191;_LL190: if((int)_tmp1DD != 8)goto _LL192;_LL191: goto _LL193;_LL192: if((int)
_tmp1DD != 9)goto _LL194;_LL193: goto _LL195;_LL194: if((int)_tmp1DD != 10)goto _LL196;
_LL195: return 80;_LL196: if((int)_tmp1DD != 11)goto _LL198;_LL197: goto _LL199;_LL198:
if((int)_tmp1DD != 12)goto _LL19A;_LL199: return 130;_LL19A: if((int)_tmp1DD != 13)
goto _LL19C;_LL19B: return 60;_LL19C: if((int)_tmp1DD != 14)goto _LL19E;_LL19D: return
40;_LL19E: if((int)_tmp1DD != 15)goto _LL1A0;_LL19F: return 50;_LL1A0: if((int)_tmp1DD
!= 16)goto _LL1A2;_LL1A1: return 90;_LL1A2: if((int)_tmp1DD != 17)goto _LL1A4;_LL1A3:
return 80;_LL1A4: if((int)_tmp1DD != 18)goto _LL1A6;_LL1A5: return 80;_LL1A6: if((int)
_tmp1DD != 19)goto _LL17F;_LL1A7: return 140;_LL17F:;}_LL13B: if(*((int*)_tmp1D9)!= 4)
goto _LL13D;_LL13C: return 20;_LL13D: if(*((int*)_tmp1D9)!= 5)goto _LL13F;_LL13E:
return 130;_LL13F: if(*((int*)_tmp1D9)!= 6)goto _LL141;_LL140: return 30;_LL141: if(*((
int*)_tmp1D9)!= 7)goto _LL143;_LL142: return 35;_LL143: if(*((int*)_tmp1D9)!= 8)goto
_LL145;_LL144: return 30;_LL145: if(*((int*)_tmp1D9)!= 9)goto _LL147;_LL146: return 10;
_LL147: if(*((int*)_tmp1D9)!= 10)goto _LL149;_LL148: goto _LL14A;_LL149: if(*((int*)
_tmp1D9)!= 11)goto _LL14B;_LL14A: return 140;_LL14B: if(*((int*)_tmp1D9)!= 12)goto
_LL14D;_LL14C: return 130;_LL14D: if(*((int*)_tmp1D9)!= 13)goto _LL14F;_tmp1DB=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D9)->f1;_LL14E: return Cyc_Absynpp_exp_prec(
_tmp1DB);_LL14F: if(*((int*)_tmp1D9)!= 14)goto _LL151;_tmp1DC=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D9)->f1;_LL150: return Cyc_Absynpp_exp_prec(_tmp1DC);_LL151: if(*((int*)
_tmp1D9)!= 15)goto _LL153;_LL152: return 120;_LL153: if(*((int*)_tmp1D9)!= 17)goto
_LL155;_LL154: return 15;_LL155: if(*((int*)_tmp1D9)!= 16)goto _LL157;_LL156: goto
_LL158;_LL157: if(*((int*)_tmp1D9)!= 18)goto _LL159;_LL158: goto _LL15A;_LL159: if(*((
int*)_tmp1D9)!= 19)goto _LL15B;_LL15A: goto _LL15C;_LL15B: if(*((int*)_tmp1D9)!= 20)
goto _LL15D;_LL15C: goto _LL15E;_LL15D: if(*((int*)_tmp1D9)!= 21)goto _LL15F;_LL15E:
goto _LL160;_LL15F: if(*((int*)_tmp1D9)!= 22)goto _LL161;_LL160: return 130;_LL161:
if(*((int*)_tmp1D9)!= 23)goto _LL163;_LL162: goto _LL164;_LL163: if(*((int*)_tmp1D9)
!= 24)goto _LL165;_LL164: goto _LL166;_LL165: if(*((int*)_tmp1D9)!= 25)goto _LL167;
_LL166: return 140;_LL167: if(*((int*)_tmp1D9)!= 26)goto _LL169;_LL168: return 150;
_LL169: if(*((int*)_tmp1D9)!= 27)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(*((int*)
_tmp1D9)!= 28)goto _LL16D;_LL16C: goto _LL16E;_LL16D: if(*((int*)_tmp1D9)!= 29)goto
_LL16F;_LL16E: goto _LL170;_LL16F: if(*((int*)_tmp1D9)!= 30)goto _LL171;_LL170: goto
_LL172;_LL171: if(*((int*)_tmp1D9)!= 31)goto _LL173;_LL172: goto _LL174;_LL173: if(*((
int*)_tmp1D9)!= 32)goto _LL175;_LL174: goto _LL176;_LL175: if(*((int*)_tmp1D9)!= 33)
goto _LL177;_LL176: goto _LL178;_LL177: if(*((int*)_tmp1D9)!= 34)goto _LL179;_LL178:
goto _LL17A;_LL179: if(*((int*)_tmp1D9)!= 35)goto _LL17B;_LL17A: goto _LL17C;_LL17B:
if(*((int*)_tmp1D9)!= 36)goto _LL17D;_LL17C: return 140;_LL17D: if(*((int*)_tmp1D9)
!= 37)goto _LL132;_LL17E: return 10000;_LL132:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1DE=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1DF;struct _tuple0*
_tmp1E0;struct _tuple0*_tmp1E1;void*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*
_tmp1E4;struct Cyc_Core_Opt*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*
_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;
struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*
_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_Absyn_Exp*
_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;
struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*
_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;
void*_tmp200;void*_tmp201;struct _dynforward_ptr*_tmp202;void*_tmp203;void*
_tmp204;unsigned int _tmp205;struct Cyc_List_List*_tmp206;void*_tmp207;struct Cyc_Absyn_Exp*
_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct _dynforward_ptr*_tmp20A;struct Cyc_Absyn_Exp*
_tmp20B;struct _dynforward_ptr*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*
_tmp20E;struct Cyc_List_List*_tmp20F;struct _tuple1*_tmp210;struct Cyc_List_List*
_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Exp*
_tmp214;struct Cyc_Absyn_Exp*_tmp215;struct _tuple0*_tmp216;struct Cyc_List_List*
_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_List_List*
_tmp21A;struct Cyc_Absyn_Tunionfield*_tmp21B;struct _tuple0*_tmp21C;struct _tuple0*
_tmp21D;struct Cyc_Absyn_MallocInfo _tmp21E;int _tmp21F;struct Cyc_Absyn_Exp*_tmp220;
void**_tmp221;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Core_Opt*_tmp223;struct Cyc_List_List*
_tmp224;struct Cyc_Absyn_Stmt*_tmp225;_LL1A9: if(*((int*)_tmp1DE)!= 0)goto _LL1AB;
_tmp1DF=((struct Cyc_Absyn_Const_e_struct*)_tmp1DE)->f1;_LL1AA: s=Cyc_Absynpp_cnst2doc(
_tmp1DF);goto _LL1A8;_LL1AB: if(*((int*)_tmp1DE)!= 1)goto _LL1AD;_tmp1E0=((struct
Cyc_Absyn_Var_e_struct*)_tmp1DE)->f1;_LL1AC: _tmp1E1=_tmp1E0;goto _LL1AE;_LL1AD:
if(*((int*)_tmp1DE)!= 2)goto _LL1AF;_tmp1E1=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1DE)->f1;_LL1AE: s=Cyc_Absynpp_qvar2doc(_tmp1E1);goto _LL1A8;_LL1AF: if(*((int*)
_tmp1DE)!= 3)goto _LL1B1;_tmp1E2=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1DE)->f1;_tmp1E3=((struct Cyc_Absyn_Primop_e_struct*)_tmp1DE)->f2;_LL1B0: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1E2,_tmp1E3);goto _LL1A8;_LL1B1: if(*((int*)
_tmp1DE)!= 4)goto _LL1B3;_tmp1E4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DE)->f1;
_tmp1E5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DE)->f2;_tmp1E6=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1DE)->f3;_LL1B2: s=({struct Cyc_PP_Doc*_tmp226[5];_tmp226[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E6);_tmp226[3]=Cyc_PP_text(({const char*_tmp228="= ";_tag_dynforward(
_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,3));}));_tmp226[2]=_tmp1E5 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((void*)_tmp1E5->v);_tmp226[1]=Cyc_PP_text(({
const char*_tmp227=" ";_tag_dynforward(_tmp227,sizeof(char),_get_zero_arr_size(
_tmp227,2));}));_tmp226[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4);Cyc_PP_cat(
_tag_dynforward(_tmp226,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A8;_LL1B3: if(*((
int*)_tmp1DE)!= 5)goto _LL1B5;_tmp1E7=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1DE)->f1;_tmp1E8=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1DE)->f2;
_LL1B4: {struct Cyc_PP_Doc*_tmp229=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E7);{void*
_tmp22A=_tmp1E8;_LL1F8: if((int)_tmp22A != 0)goto _LL1FA;_LL1F9: s=({struct Cyc_PP_Doc*
_tmp22B[2];_tmp22B[1]=_tmp229;_tmp22B[0]=Cyc_PP_text(({const char*_tmp22C="++";
_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,3));}));Cyc_PP_cat(
_tag_dynforward(_tmp22B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1F7;_LL1FA: if((
int)_tmp22A != 2)goto _LL1FC;_LL1FB: s=({struct Cyc_PP_Doc*_tmp22D[2];_tmp22D[1]=
_tmp229;_tmp22D[0]=Cyc_PP_text(({const char*_tmp22E="--";_tag_dynforward(_tmp22E,
sizeof(char),_get_zero_arr_size(_tmp22E,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp22D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1F7;_LL1FC: if((int)_tmp22A != 1)
goto _LL1FE;_LL1FD: s=({struct Cyc_PP_Doc*_tmp22F[2];_tmp22F[1]=Cyc_PP_text(({const
char*_tmp230="++";_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size(
_tmp230,3));}));_tmp22F[0]=_tmp229;Cyc_PP_cat(_tag_dynforward(_tmp22F,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1F7;_LL1FE: if((int)_tmp22A != 3)goto _LL1F7;
_LL1FF: s=({struct Cyc_PP_Doc*_tmp231[2];_tmp231[1]=Cyc_PP_text(({const char*
_tmp232="--";_tag_dynforward(_tmp232,sizeof(char),_get_zero_arr_size(_tmp232,3));}));
_tmp231[0]=_tmp229;Cyc_PP_cat(_tag_dynforward(_tmp231,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1F7;_LL1F7:;}goto _LL1A8;}_LL1B5: if(*((int*)_tmp1DE)!= 6)goto _LL1B7;
_tmp1E9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DE)->f1;_tmp1EA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1DE)->f2;_tmp1EB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DE)->f3;_LL1B6:
s=({struct Cyc_PP_Doc*_tmp233[5];_tmp233[4]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1EB);_tmp233[3]=Cyc_PP_text(({const char*_tmp235=" : ";_tag_dynforward(
_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,4));}));_tmp233[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1EA);_tmp233[1]=Cyc_PP_text(({const char*_tmp234=" ? ";_tag_dynforward(
_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,4));}));_tmp233[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E9);Cyc_PP_cat(_tag_dynforward(_tmp233,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1A8;_LL1B7: if(*((int*)_tmp1DE)!= 7)goto _LL1B9;_tmp1EC=((struct Cyc_Absyn_And_e_struct*)
_tmp1DE)->f1;_tmp1ED=((struct Cyc_Absyn_And_e_struct*)_tmp1DE)->f2;_LL1B8: s=({
struct Cyc_PP_Doc*_tmp236[3];_tmp236[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1ED);
_tmp236[1]=Cyc_PP_text(({const char*_tmp237=" && ";_tag_dynforward(_tmp237,
sizeof(char),_get_zero_arr_size(_tmp237,5));}));_tmp236[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EC);Cyc_PP_cat(_tag_dynforward(_tmp236,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A8;_LL1B9: if(*((int*)_tmp1DE)!= 8)goto _LL1BB;_tmp1EE=((struct Cyc_Absyn_Or_e_struct*)
_tmp1DE)->f1;_tmp1EF=((struct Cyc_Absyn_Or_e_struct*)_tmp1DE)->f2;_LL1BA: s=({
struct Cyc_PP_Doc*_tmp238[3];_tmp238[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EF);
_tmp238[1]=Cyc_PP_text(({const char*_tmp239=" || ";_tag_dynforward(_tmp239,
sizeof(char),_get_zero_arr_size(_tmp239,5));}));_tmp238[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EE);Cyc_PP_cat(_tag_dynforward(_tmp238,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A8;_LL1BB: if(*((int*)_tmp1DE)!= 9)goto _LL1BD;_tmp1F0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1DE)->f1;_tmp1F1=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1DE)->f2;_LL1BC: s=({
struct Cyc_PP_Doc*_tmp23A[5];_tmp23A[4]=Cyc_PP_text(({const char*_tmp23D=")";
_tag_dynforward(_tmp23D,sizeof(char),_get_zero_arr_size(_tmp23D,2));}));_tmp23A[
3]=Cyc_Absynpp_exp2doc(_tmp1F1);_tmp23A[2]=Cyc_PP_text(({const char*_tmp23C=", ";
_tag_dynforward(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,3));}));_tmp23A[
1]=Cyc_Absynpp_exp2doc(_tmp1F0);_tmp23A[0]=Cyc_PP_text(({const char*_tmp23B="(";
_tag_dynforward(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp23A,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A8;_LL1BD: if(*((
int*)_tmp1DE)!= 10)goto _LL1BF;_tmp1F2=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1DE)->f1;_tmp1F3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1DE)->f2;_LL1BE:
s=({struct Cyc_PP_Doc*_tmp23E[4];_tmp23E[3]=Cyc_PP_text(({const char*_tmp240=")";
_tag_dynforward(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,2));}));_tmp23E[
2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1F3);_tmp23E[1]=Cyc_PP_text(({const char*
_tmp23F="(";_tag_dynforward(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,2));}));
_tmp23E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F2);Cyc_PP_cat(_tag_dynforward(
_tmp23E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A8;_LL1BF: if(*((int*)_tmp1DE)!= 
11)goto _LL1C1;_tmp1F4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1DE)->f1;_tmp1F5=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp1DE)->f2;_LL1C0: s=({struct Cyc_PP_Doc*
_tmp241[4];_tmp241[3]=Cyc_PP_text(({const char*_tmp243=")";_tag_dynforward(
_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,2));}));_tmp241[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp1F5);_tmp241[1]=Cyc_PP_text(({const char*_tmp242="(";_tag_dynforward(
_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,2));}));_tmp241[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F4);Cyc_PP_cat(_tag_dynforward(_tmp241,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A8;_LL1C1: if(*((int*)_tmp1DE)!= 12)goto _LL1C3;_tmp1F6=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1DE)->f1;_LL1C2: s=({struct Cyc_PP_Doc*_tmp244[2];_tmp244[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F6);_tmp244[0]=Cyc_PP_text(({const char*_tmp245="throw ";
_tag_dynforward(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp244,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A8;_LL1C3: if(*((
int*)_tmp1DE)!= 13)goto _LL1C5;_tmp1F7=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1DE)->f1;_LL1C4: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F7);goto _LL1A8;_LL1C5:
if(*((int*)_tmp1DE)!= 14)goto _LL1C7;_tmp1F8=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1DE)->f1;_LL1C6: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F8);goto _LL1A8;_LL1C7:
if(*((int*)_tmp1DE)!= 15)goto _LL1C9;_tmp1F9=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1DE)->f1;_tmp1FA=((struct Cyc_Absyn_Cast_e_struct*)_tmp1DE)->f2;_LL1C8: s=({
struct Cyc_PP_Doc*_tmp246[4];_tmp246[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FA);
_tmp246[2]=Cyc_PP_text(({const char*_tmp248=")";_tag_dynforward(_tmp248,sizeof(
char),_get_zero_arr_size(_tmp248,2));}));_tmp246[1]=Cyc_Absynpp_typ2doc(_tmp1F9);
_tmp246[0]=Cyc_PP_text(({const char*_tmp247="(";_tag_dynforward(_tmp247,sizeof(
char),_get_zero_arr_size(_tmp247,2));}));Cyc_PP_cat(_tag_dynforward(_tmp246,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A8;_LL1C9: if(*((int*)_tmp1DE)!= 16)goto
_LL1CB;_tmp1FB=((struct Cyc_Absyn_Address_e_struct*)_tmp1DE)->f1;_LL1CA: s=({
struct Cyc_PP_Doc*_tmp249[2];_tmp249[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FB);
_tmp249[0]=Cyc_PP_text(({const char*_tmp24A="&";_tag_dynforward(_tmp24A,sizeof(
char),_get_zero_arr_size(_tmp24A,2));}));Cyc_PP_cat(_tag_dynforward(_tmp249,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A8;_LL1CB: if(*((int*)_tmp1DE)!= 17)goto
_LL1CD;_tmp1FC=((struct Cyc_Absyn_New_e_struct*)_tmp1DE)->f1;_tmp1FD=((struct Cyc_Absyn_New_e_struct*)
_tmp1DE)->f2;_LL1CC: if(_tmp1FC == 0)s=({struct Cyc_PP_Doc*_tmp24B[2];_tmp24B[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FD);_tmp24B[0]=Cyc_PP_text(({const char*
_tmp24C="new ";_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,5));}));
Cyc_PP_cat(_tag_dynforward(_tmp24B,sizeof(struct Cyc_PP_Doc*),2));});else{s=({
struct Cyc_PP_Doc*_tmp24D[4];_tmp24D[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FD);
_tmp24D[2]=Cyc_PP_text(({const char*_tmp24F=") ";_tag_dynforward(_tmp24F,sizeof(
char),_get_zero_arr_size(_tmp24F,3));}));_tmp24D[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp1FC);_tmp24D[0]=Cyc_PP_text(({const char*_tmp24E="rnew(";
_tag_dynforward(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,6));}));Cyc_PP_cat(
_tag_dynforward(_tmp24D,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL1A8;_LL1CD: if(*((
int*)_tmp1DE)!= 18)goto _LL1CF;_tmp1FE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1DE)->f1;_LL1CE: s=({struct Cyc_PP_Doc*_tmp250[3];_tmp250[2]=Cyc_PP_text(({
const char*_tmp252=")";_tag_dynforward(_tmp252,sizeof(char),_get_zero_arr_size(
_tmp252,2));}));_tmp250[1]=Cyc_Absynpp_typ2doc(_tmp1FE);_tmp250[0]=Cyc_PP_text(({
const char*_tmp251="sizeof(";_tag_dynforward(_tmp251,sizeof(char),
_get_zero_arr_size(_tmp251,8));}));Cyc_PP_cat(_tag_dynforward(_tmp250,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL1A8;_LL1CF: if(*((int*)_tmp1DE)!= 19)goto _LL1D1;
_tmp1FF=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1DE)->f1;_LL1D0: s=({struct Cyc_PP_Doc*
_tmp253[3];_tmp253[2]=Cyc_PP_text(({const char*_tmp255=")";_tag_dynforward(
_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,2));}));_tmp253[1]=Cyc_Absynpp_exp2doc(
_tmp1FF);_tmp253[0]=Cyc_PP_text(({const char*_tmp254="sizeof(";_tag_dynforward(
_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp253,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A8;_LL1D1: if(*((
int*)_tmp1DE)!= 20)goto _LL1D3;_tmp200=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1DE)->f1;_tmp201=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f2;
if(*((int*)_tmp201)!= 0)goto _LL1D3;_tmp202=((struct Cyc_Absyn_StructField_struct*)
_tmp201)->f1;_LL1D2: s=({struct Cyc_PP_Doc*_tmp256[5];_tmp256[4]=Cyc_PP_text(({
const char*_tmp259=")";_tag_dynforward(_tmp259,sizeof(char),_get_zero_arr_size(
_tmp259,2));}));_tmp256[3]=Cyc_PP_textptr(_tmp202);_tmp256[2]=Cyc_PP_text(({
const char*_tmp258=",";_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size(
_tmp258,2));}));_tmp256[1]=Cyc_Absynpp_typ2doc(_tmp200);_tmp256[0]=Cyc_PP_text(({
const char*_tmp257="offsetof(";_tag_dynforward(_tmp257,sizeof(char),
_get_zero_arr_size(_tmp257,10));}));Cyc_PP_cat(_tag_dynforward(_tmp256,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A8;_LL1D3: if(*((int*)_tmp1DE)!= 20)goto _LL1D5;
_tmp203=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f1;_tmp204=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f2;if(*((int*)_tmp204)!= 1)goto
_LL1D5;_tmp205=((struct Cyc_Absyn_TupleIndex_struct*)_tmp204)->f1;_LL1D4: s=({
struct Cyc_PP_Doc*_tmp25A[5];_tmp25A[4]=Cyc_PP_text(({const char*_tmp260=")";
_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,2));}));_tmp25A[
3]=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp25F;_tmp25F.tag=
1;_tmp25F.f1=(unsigned long)((int)_tmp205);{void*_tmp25D[1]={& _tmp25F};Cyc_aprintf(({
const char*_tmp25E="%d";_tag_dynforward(_tmp25E,sizeof(char),_get_zero_arr_size(
_tmp25E,3));}),_tag_dynforward(_tmp25D,sizeof(void*),1));}}));_tmp25A[2]=Cyc_PP_text(({
const char*_tmp25C=",";_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size(
_tmp25C,2));}));_tmp25A[1]=Cyc_Absynpp_typ2doc(_tmp203);_tmp25A[0]=Cyc_PP_text(({
const char*_tmp25B="offsetof(";_tag_dynforward(_tmp25B,sizeof(char),
_get_zero_arr_size(_tmp25B,10));}));Cyc_PP_cat(_tag_dynforward(_tmp25A,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A8;_LL1D5: if(*((int*)_tmp1DE)!= 21)goto _LL1D7;
_tmp206=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1DE)->f1;_tmp207=(void*)((struct
Cyc_Absyn_Gentyp_e_struct*)_tmp1DE)->f2;_LL1D6: s=({struct Cyc_PP_Doc*_tmp261[4];
_tmp261[3]=Cyc_PP_text(({const char*_tmp263=")";_tag_dynforward(_tmp263,sizeof(
char),_get_zero_arr_size(_tmp263,2));}));_tmp261[2]=Cyc_Absynpp_typ2doc(_tmp207);
_tmp261[1]=Cyc_Absynpp_tvars2doc(_tmp206);_tmp261[0]=Cyc_PP_text(({const char*
_tmp262="__gen(";_tag_dynforward(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,
7));}));Cyc_PP_cat(_tag_dynforward(_tmp261,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL1A8;_LL1D7: if(*((int*)_tmp1DE)!= 22)goto _LL1D9;_tmp208=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1DE)->f1;_LL1D8: s=({struct Cyc_PP_Doc*_tmp264[2];_tmp264[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp208);_tmp264[0]=Cyc_PP_text(({const char*_tmp265="*";_tag_dynforward(
_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp264,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A8;_LL1D9: if(*((
int*)_tmp1DE)!= 23)goto _LL1DB;_tmp209=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1DE)->f1;_tmp20A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1DE)->f2;_LL1DA:
s=({struct Cyc_PP_Doc*_tmp266[3];_tmp266[2]=Cyc_PP_textptr(_tmp20A);_tmp266[1]=
Cyc_PP_text(({const char*_tmp267=".";_tag_dynforward(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,2));}));_tmp266[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp209);Cyc_PP_cat(_tag_dynforward(_tmp266,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A8;_LL1DB: if(*((int*)_tmp1DE)!= 24)goto _LL1DD;_tmp20B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1DE)->f1;_tmp20C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1DE)->f2;_LL1DC: s=({
struct Cyc_PP_Doc*_tmp268[3];_tmp268[2]=Cyc_PP_textptr(_tmp20C);_tmp268[1]=Cyc_PP_text(({
const char*_tmp269="->";_tag_dynforward(_tmp269,sizeof(char),_get_zero_arr_size(
_tmp269,3));}));_tmp268[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp20B);Cyc_PP_cat(
_tag_dynforward(_tmp268,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A8;_LL1DD: if(*((
int*)_tmp1DE)!= 25)goto _LL1DF;_tmp20D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1DE)->f1;_tmp20E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1DE)->f2;_LL1DE: s=({
struct Cyc_PP_Doc*_tmp26A[4];_tmp26A[3]=Cyc_PP_text(({const char*_tmp26C="]";
_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,2));}));_tmp26A[
2]=Cyc_Absynpp_exp2doc(_tmp20E);_tmp26A[1]=Cyc_PP_text(({const char*_tmp26B="[";
_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,2));}));_tmp26A[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp20D);Cyc_PP_cat(_tag_dynforward(_tmp26A,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A8;_LL1DF: if(*((int*)_tmp1DE)!= 26)goto
_LL1E1;_tmp20F=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1DE)->f1;_LL1E0: s=({struct
Cyc_PP_Doc*_tmp26D[4];_tmp26D[3]=Cyc_PP_text(({const char*_tmp26F=")";
_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,2));}));_tmp26D[
2]=Cyc_Absynpp_exps2doc_prec(20,_tmp20F);_tmp26D[1]=Cyc_PP_text(({const char*
_tmp26E="(";_tag_dynforward(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,2));}));
_tmp26D[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp26D,sizeof(struct
Cyc_PP_Doc*),4));});goto _LL1A8;_LL1E1: if(*((int*)_tmp1DE)!= 27)goto _LL1E3;
_tmp210=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1DE)->f1;_tmp211=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1DE)->f2;_LL1E2: s=({struct Cyc_PP_Doc*_tmp270[4];_tmp270[3]=Cyc_Absynpp_group_braces(({
const char*_tmp273=",";_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(
_tmp273,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp211));_tmp270[2]=Cyc_PP_text(({
const char*_tmp272=")";_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(
_tmp272,2));}));_tmp270[1]=Cyc_Absynpp_typ2doc((*_tmp210).f3);_tmp270[0]=Cyc_PP_text(({
const char*_tmp271="(";_tag_dynforward(_tmp271,sizeof(char),_get_zero_arr_size(
_tmp271,2));}));Cyc_PP_cat(_tag_dynforward(_tmp270,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A8;_LL1E3: if(*((int*)_tmp1DE)!= 28)goto _LL1E5;_tmp212=((struct Cyc_Absyn_Array_e_struct*)
_tmp1DE)->f1;_LL1E4: s=Cyc_Absynpp_group_braces(({const char*_tmp274=",";
_tag_dynforward(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp212));goto _LL1A8;_LL1E5: if(*((int*)_tmp1DE)
!= 29)goto _LL1E7;_tmp213=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f1;
_tmp214=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f2;_tmp215=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f3;_LL1E6: s=({struct Cyc_PP_Doc*
_tmp275[8];_tmp275[7]=Cyc_Absynpp_rb();_tmp275[6]=Cyc_Absynpp_exp2doc(_tmp215);
_tmp275[5]=Cyc_PP_text(({const char*_tmp278=" : ";_tag_dynforward(_tmp278,sizeof(
char),_get_zero_arr_size(_tmp278,4));}));_tmp275[4]=Cyc_Absynpp_exp2doc(_tmp214);
_tmp275[3]=Cyc_PP_text(({const char*_tmp277=" < ";_tag_dynforward(_tmp277,sizeof(
char),_get_zero_arr_size(_tmp277,4));}));_tmp275[2]=Cyc_PP_text(*(*_tmp213->name).f2);
_tmp275[1]=Cyc_PP_text(({const char*_tmp276="for ";_tag_dynforward(_tmp276,
sizeof(char),_get_zero_arr_size(_tmp276,5));}));_tmp275[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dynforward(_tmp275,sizeof(struct Cyc_PP_Doc*),8));});goto _LL1A8;_LL1E7: if(*((
int*)_tmp1DE)!= 30)goto _LL1E9;_tmp216=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DE)->f1;
_tmp217=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DE)->f2;_tmp218=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1DE)->f3;_LL1E8: {struct Cyc_List_List*_tmp279=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp218);s=({struct Cyc_PP_Doc*_tmp27A[2];_tmp27A[1]=Cyc_Absynpp_group_braces(({
const char*_tmp27B=",";_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(
_tmp27B,2));}),_tmp217 != 0?({struct Cyc_List_List*_tmp27C=_cycalloc(sizeof(*
_tmp27C));_tmp27C->hd=Cyc_Absynpp_tps2doc(_tmp217);_tmp27C->tl=_tmp279;_tmp27C;}):
_tmp279);_tmp27A[0]=Cyc_Absynpp_qvar2doc(_tmp216);Cyc_PP_cat(_tag_dynforward(
_tmp27A,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A8;}_LL1E9: if(*((int*)_tmp1DE)
!= 31)goto _LL1EB;_tmp219=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1DE)->f2;
_LL1EA: s=Cyc_Absynpp_group_braces(({const char*_tmp27D=",";_tag_dynforward(
_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp219));goto _LL1A8;_LL1EB: if(*((int*)_tmp1DE)!= 32)goto _LL1ED;_tmp21A=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp1DE)->f1;_tmp21B=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1DE)->f3;_LL1EC: if(_tmp21A == 0)s=Cyc_Absynpp_qvar2doc(_tmp21B->name);else{s=({
struct Cyc_PP_Doc*_tmp27E[2];_tmp27E[1]=Cyc_PP_egroup(({const char*_tmp27F="(";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,2));}),({const
char*_tmp280=")";_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,
2));}),({const char*_tmp281=",";_tag_dynforward(_tmp281,sizeof(char),
_get_zero_arr_size(_tmp281,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp21A));_tmp27E[0]=Cyc_Absynpp_qvar2doc(_tmp21B->name);Cyc_PP_cat(
_tag_dynforward(_tmp27E,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL1A8;_LL1ED: if(*((
int*)_tmp1DE)!= 33)goto _LL1EF;_tmp21C=((struct Cyc_Absyn_Enum_e_struct*)_tmp1DE)->f1;
_LL1EE: s=Cyc_Absynpp_qvar2doc(_tmp21C);goto _LL1A8;_LL1EF: if(*((int*)_tmp1DE)!= 
34)goto _LL1F1;_tmp21D=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1DE)->f1;_LL1F0: s=
Cyc_Absynpp_qvar2doc(_tmp21D);goto _LL1A8;_LL1F1: if(*((int*)_tmp1DE)!= 35)goto
_LL1F3;_tmp21E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1DE)->f1;_tmp21F=_tmp21E.is_calloc;
_tmp220=_tmp21E.rgn;_tmp221=_tmp21E.elt_type;_tmp222=_tmp21E.num_elts;_LL1F2: if(
_tmp21F){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp221)),0);if(_tmp220 == 0)s=({struct Cyc_PP_Doc*_tmp282[5];_tmp282[4]=Cyc_PP_text(({
const char*_tmp285=")";_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size(
_tmp285,2));}));_tmp282[3]=Cyc_Absynpp_exp2doc(st);_tmp282[2]=Cyc_PP_text(({
const char*_tmp284=",";_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size(
_tmp284,2));}));_tmp282[1]=Cyc_Absynpp_exp2doc(_tmp222);_tmp282[0]=Cyc_PP_text(({
const char*_tmp283="calloc(";_tag_dynforward(_tmp283,sizeof(char),
_get_zero_arr_size(_tmp283,8));}));Cyc_PP_cat(_tag_dynforward(_tmp282,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp286[7];_tmp286[6]=Cyc_PP_text(({
const char*_tmp28A=")";_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size(
_tmp28A,2));}));_tmp286[5]=Cyc_Absynpp_exp2doc(st);_tmp286[4]=Cyc_PP_text(({
const char*_tmp289=",";_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size(
_tmp289,2));}));_tmp286[3]=Cyc_Absynpp_exp2doc(_tmp222);_tmp286[2]=Cyc_PP_text(({
const char*_tmp288=",";_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(
_tmp288,2));}));_tmp286[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp220);
_tmp286[0]=Cyc_PP_text(({const char*_tmp287="rcalloc(";_tag_dynforward(_tmp287,
sizeof(char),_get_zero_arr_size(_tmp287,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp286,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*new_e;if(
_tmp221 == 0)new_e=_tmp222;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp221,0),_tmp222,0);}if(_tmp220 == 0)s=({struct Cyc_PP_Doc*_tmp28B[3];_tmp28B[2]=
Cyc_PP_text(({const char*_tmp28D=")";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,2));}));_tmp28B[1]=Cyc_Absynpp_exp2doc(new_e);_tmp28B[
0]=Cyc_PP_text(({const char*_tmp28C="malloc(";_tag_dynforward(_tmp28C,sizeof(char),
_get_zero_arr_size(_tmp28C,8));}));Cyc_PP_cat(_tag_dynforward(_tmp28B,sizeof(
struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*_tmp28E[5];_tmp28E[4]=Cyc_PP_text(({
const char*_tmp291=")";_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size(
_tmp291,2));}));_tmp28E[3]=Cyc_Absynpp_exp2doc(new_e);_tmp28E[2]=Cyc_PP_text(({
const char*_tmp290=",";_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(
_tmp290,2));}));_tmp28E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp220);
_tmp28E[0]=Cyc_PP_text(({const char*_tmp28F="rmalloc(";_tag_dynforward(_tmp28F,
sizeof(char),_get_zero_arr_size(_tmp28F,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp28E,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL1A8;_LL1F3: if(*((int*)_tmp1DE)
!= 36)goto _LL1F5;_tmp223=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1DE)->f1;
_tmp224=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1DE)->f2;_LL1F4: s=Cyc_Absynpp_group_braces(({
const char*_tmp292=",";_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(
_tmp292,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp224));goto _LL1A8;
_LL1F5: if(*((int*)_tmp1DE)!= 37)goto _LL1A8;_tmp225=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp1DE)->f1;_LL1F6: s=({struct Cyc_PP_Doc*_tmp293[7];_tmp293[6]=Cyc_PP_text(({
const char*_tmp295=")";_tag_dynforward(_tmp295,sizeof(char),_get_zero_arr_size(
_tmp295,2));}));_tmp293[5]=Cyc_Absynpp_rb();_tmp293[4]=Cyc_PP_blank_doc();
_tmp293[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp225));_tmp293[2]=Cyc_PP_blank_doc();
_tmp293[1]=Cyc_Absynpp_lb();_tmp293[0]=Cyc_PP_text(({const char*_tmp294="(";
_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp293,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1A8;_LL1A8:;}if(
inprec >= myprec)s=({struct Cyc_PP_Doc*_tmp296[3];_tmp296[2]=Cyc_PP_text(({const
char*_tmp298=")";_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,
2));}));_tmp296[1]=s;_tmp296[0]=Cyc_PP_text(({const char*_tmp297="(";
_tag_dynforward(_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp296,sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp299=d;struct Cyc_Absyn_Exp*_tmp29A;
struct _dynforward_ptr*_tmp29B;_LL201: if(*((int*)_tmp299)!= 0)goto _LL203;_tmp29A=((
struct Cyc_Absyn_ArrayElement_struct*)_tmp299)->f1;_LL202: return({struct Cyc_PP_Doc*
_tmp29C[3];_tmp29C[2]=Cyc_PP_text(({const char*_tmp29E="]";_tag_dynforward(
_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,2));}));_tmp29C[1]=Cyc_Absynpp_exp2doc(
_tmp29A);_tmp29C[0]=Cyc_PP_text(({const char*_tmp29D=".[";_tag_dynforward(_tmp29D,
sizeof(char),_get_zero_arr_size(_tmp29D,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp29C,sizeof(struct Cyc_PP_Doc*),3));});_LL203: if(*((int*)_tmp299)!= 1)goto
_LL200;_tmp29B=((struct Cyc_Absyn_FieldName_struct*)_tmp299)->f1;_LL204: return({
struct Cyc_PP_Doc*_tmp29F[2];_tmp29F[1]=Cyc_PP_textptr(_tmp29B);_tmp29F[0]=Cyc_PP_text(({
const char*_tmp2A0=".";_tag_dynforward(_tmp2A0,sizeof(char),_get_zero_arr_size(
_tmp2A0,2));}));Cyc_PP_cat(_tag_dynforward(_tmp29F,sizeof(struct Cyc_PP_Doc*),2));});
_LL200:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp2A1[2];
_tmp2A1[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp2A1[0]=Cyc_PP_egroup(({const char*
_tmp2A2="";_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,1));}),({
const char*_tmp2A3="=";_tag_dynforward(_tmp2A3,sizeof(char),_get_zero_arr_size(
_tmp2A3,2));}),({const char*_tmp2A4="=";_tag_dynforward(_tmp2A4,sizeof(char),
_get_zero_arr_size(_tmp2A4,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));
Cyc_PP_cat(_tag_dynforward(_tmp2A1,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){return Cyc_PP_group(({
const char*_tmp2A5="";_tag_dynforward(_tmp2A5,sizeof(char),_get_zero_arr_size(
_tmp2A5,1));}),({const char*_tmp2A6="";_tag_dynforward(_tmp2A6,sizeof(char),
_get_zero_arr_size(_tmp2A6,1));}),({const char*_tmp2A7=",";_tag_dynforward(
_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(
union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union _tmp2A8=c;void*_tmp2A9;char
_tmp2AA;void*_tmp2AB;short _tmp2AC;void*_tmp2AD;int _tmp2AE;void*_tmp2AF;int
_tmp2B0;void*_tmp2B1;int _tmp2B2;void*_tmp2B3;long long _tmp2B4;struct
_dynforward_ptr _tmp2B5;struct _dynforward_ptr _tmp2B6;_LL206: if((_tmp2A8.Char_c).tag
!= 0)goto _LL208;_tmp2A9=(_tmp2A8.Char_c).f1;_tmp2AA=(_tmp2A8.Char_c).f2;_LL207:
return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2B9;
_tmp2B9.tag=0;_tmp2B9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_char_escape(
_tmp2AA));{void*_tmp2B7[1]={& _tmp2B9};Cyc_aprintf(({const char*_tmp2B8="'%s'";
_tag_dynforward(_tmp2B8,sizeof(char),_get_zero_arr_size(_tmp2B8,5));}),
_tag_dynforward(_tmp2B7,sizeof(void*),1));}}));_LL208: if((_tmp2A8.Short_c).tag != 
1)goto _LL20A;_tmp2AB=(_tmp2A8.Short_c).f1;_tmp2AC=(_tmp2A8.Short_c).f2;_LL209:
return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2BC;
_tmp2BC.tag=1;_tmp2BC.f1=(unsigned long)((int)_tmp2AC);{void*_tmp2BA[1]={&
_tmp2BC};Cyc_aprintf(({const char*_tmp2BB="%d";_tag_dynforward(_tmp2BB,sizeof(
char),_get_zero_arr_size(_tmp2BB,3));}),_tag_dynforward(_tmp2BA,sizeof(void*),1));}}));
_LL20A: if((_tmp2A8.Int_c).tag != 2)goto _LL20C;_tmp2AD=(_tmp2A8.Int_c).f1;if((int)
_tmp2AD != 2)goto _LL20C;_tmp2AE=(_tmp2A8.Int_c).f2;_LL20B: _tmp2B0=_tmp2AE;goto
_LL20D;_LL20C: if((_tmp2A8.Int_c).tag != 2)goto _LL20E;_tmp2AF=(_tmp2A8.Int_c).f1;
if((int)_tmp2AF != 0)goto _LL20E;_tmp2B0=(_tmp2A8.Int_c).f2;_LL20D: return Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2BF;_tmp2BF.tag=1;_tmp2BF.f1=(
unsigned long)_tmp2B0;{void*_tmp2BD[1]={& _tmp2BF};Cyc_aprintf(({const char*
_tmp2BE="%d";_tag_dynforward(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,3));}),
_tag_dynforward(_tmp2BD,sizeof(void*),1));}}));_LL20E: if((_tmp2A8.Int_c).tag != 2)
goto _LL210;_tmp2B1=(_tmp2A8.Int_c).f1;if((int)_tmp2B1 != 1)goto _LL210;_tmp2B2=(
_tmp2A8.Int_c).f2;_LL20F: return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp2C2;_tmp2C2.tag=1;_tmp2C2.f1=(unsigned int)_tmp2B2;{void*_tmp2C0[1]={&
_tmp2C2};Cyc_aprintf(({const char*_tmp2C1="%u";_tag_dynforward(_tmp2C1,sizeof(
char),_get_zero_arr_size(_tmp2C1,3));}),_tag_dynforward(_tmp2C0,sizeof(void*),1));}}));
_LL210: if((_tmp2A8.LongLong_c).tag != 3)goto _LL212;_tmp2B3=(_tmp2A8.LongLong_c).f1;
_tmp2B4=(_tmp2A8.LongLong_c).f2;_LL211: return Cyc_PP_text(({const char*_tmp2C3="<<FIX LONG LONG CONSTANT>>";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,27));}));_LL212:
if((_tmp2A8.Float_c).tag != 4)goto _LL214;_tmp2B5=(_tmp2A8.Float_c).f1;_LL213:
return Cyc_PP_text(_tmp2B5);_LL214: if((_tmp2A8.Null_c).tag != 6)goto _LL216;_LL215:
return Cyc_PP_text(({const char*_tmp2C4="NULL";_tag_dynforward(_tmp2C4,sizeof(char),
_get_zero_arr_size(_tmp2C4,5));}));_LL216: if((_tmp2A8.String_c).tag != 5)goto
_LL205;_tmp2B6=(_tmp2A8.String_c).f1;_LL217: return({struct Cyc_PP_Doc*_tmp2C5[3];
_tmp2C5[2]=Cyc_PP_text(({const char*_tmp2C7="\"";_tag_dynforward(_tmp2C7,sizeof(
char),_get_zero_arr_size(_tmp2C7,2));}));_tmp2C5[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp2B6));_tmp2C5[0]=Cyc_PP_text(({const char*_tmp2C6="\"";_tag_dynforward(
_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp2C5,sizeof(struct Cyc_PP_Doc*),3));});_LL205:;}struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*
ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0  || es->tl != 0)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C8=_cycalloc(sizeof(*
_tmp2C8));_tmp2C8[0]=({struct Cyc_Core_Failure_struct _tmp2C9;_tmp2C9.tag=Cyc_Core_Failure;
_tmp2C9.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2CC;_tmp2CC.tag=
0;_tmp2CC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2CA[1]={& _tmp2CC};Cyc_aprintf(({const char*_tmp2CB="Absynpp::primapp2doc Size: %s with bad args";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,44));}),
_tag_dynforward(_tmp2CA,sizeof(void*),1));}});_tmp2C9;});_tmp2C8;}));{struct Cyc_PP_Doc*
_tmp2CD=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({
struct Cyc_PP_Doc*_tmp2CE[2];_tmp2CE[1]=Cyc_PP_text(({const char*_tmp2CF=".size";
_tag_dynforward(_tmp2CF,sizeof(char),_get_zero_arr_size(_tmp2CF,6));}));_tmp2CE[
0]=_tmp2CD;Cyc_PP_cat(_tag_dynforward(_tmp2CE,sizeof(struct Cyc_PP_Doc*),2));});}}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2D0=
_cycalloc(sizeof(*_tmp2D0));_tmp2D0[0]=({struct Cyc_Core_Failure_struct _tmp2D1;
_tmp2D1.tag=Cyc_Core_Failure;_tmp2D1.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2D4;_tmp2D4.tag=0;_tmp2D4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_PP_string_of_doc(ps,72));{void*_tmp2D2[1]={& _tmp2D4};Cyc_aprintf(({const char*
_tmp2D3="Absynpp::primapp2doc: %s with no args";_tag_dynforward(_tmp2D3,sizeof(
char),_get_zero_arr_size(_tmp2D3,38));}),_tag_dynforward(_tmp2D2,sizeof(void*),1));}});
_tmp2D1;});_tmp2D0;}));else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp2D5[3];
_tmp2D5[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2D5[1]=Cyc_PP_text(({const char*_tmp2D6=" ";
_tag_dynforward(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,2));}));_tmp2D5[
0]=ps;Cyc_PP_cat(_tag_dynforward(_tmp2D5,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7[0]=({
struct Cyc_Core_Failure_struct _tmp2D8;_tmp2D8.tag=Cyc_Core_Failure;_tmp2D8.f1=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2DB;_tmp2DB.tag=0;_tmp2DB.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(ps,72));{void*
_tmp2D9[1]={& _tmp2DB};Cyc_aprintf(({const char*_tmp2DA="Absynpp::primapp2doc: %s with more than 2 args";
_tag_dynforward(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,47));}),
_tag_dynforward(_tmp2D9,sizeof(void*),1));}});_tmp2D8;});_tmp2D7;}));else{return({
struct Cyc_PP_Doc*_tmp2DC[5];_tmp2DC[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd;_tmp2DC[3]=Cyc_PP_text(({const char*_tmp2DE=" ";
_tag_dynforward(_tmp2DE,sizeof(char),_get_zero_arr_size(_tmp2DE,2));}));_tmp2DC[
2]=ps;_tmp2DC[1]=Cyc_PP_text(({const char*_tmp2DD=" ";_tag_dynforward(_tmp2DD,
sizeof(char),_get_zero_arr_size(_tmp2DD,2));}));_tmp2DC[0]=(struct Cyc_PP_Doc*)ds->hd;
Cyc_PP_cat(_tag_dynforward(_tmp2DC,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p){void*_tmp2DF=p;_LL219: if((int)
_tmp2DF != 0)goto _LL21B;_LL21A: return({const char*_tmp2E0="+";_tag_dynforward(
_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,2));});_LL21B: if((int)_tmp2DF != 
1)goto _LL21D;_LL21C: return({const char*_tmp2E1="*";_tag_dynforward(_tmp2E1,
sizeof(char),_get_zero_arr_size(_tmp2E1,2));});_LL21D: if((int)_tmp2DF != 2)goto
_LL21F;_LL21E: return({const char*_tmp2E2="-";_tag_dynforward(_tmp2E2,sizeof(char),
_get_zero_arr_size(_tmp2E2,2));});_LL21F: if((int)_tmp2DF != 3)goto _LL221;_LL220:
return({const char*_tmp2E3="/";_tag_dynforward(_tmp2E3,sizeof(char),
_get_zero_arr_size(_tmp2E3,2));});_LL221: if((int)_tmp2DF != 4)goto _LL223;_LL222:
return({const char*_tmp2E4="%";_tag_dynforward(_tmp2E4,sizeof(char),
_get_zero_arr_size(_tmp2E4,2));});_LL223: if((int)_tmp2DF != 5)goto _LL225;_LL224:
return({const char*_tmp2E5="==";_tag_dynforward(_tmp2E5,sizeof(char),
_get_zero_arr_size(_tmp2E5,3));});_LL225: if((int)_tmp2DF != 6)goto _LL227;_LL226:
return({const char*_tmp2E6="!=";_tag_dynforward(_tmp2E6,sizeof(char),
_get_zero_arr_size(_tmp2E6,3));});_LL227: if((int)_tmp2DF != 7)goto _LL229;_LL228:
return({const char*_tmp2E7=">";_tag_dynforward(_tmp2E7,sizeof(char),
_get_zero_arr_size(_tmp2E7,2));});_LL229: if((int)_tmp2DF != 8)goto _LL22B;_LL22A:
return({const char*_tmp2E8="<";_tag_dynforward(_tmp2E8,sizeof(char),
_get_zero_arr_size(_tmp2E8,2));});_LL22B: if((int)_tmp2DF != 9)goto _LL22D;_LL22C:
return({const char*_tmp2E9=">=";_tag_dynforward(_tmp2E9,sizeof(char),
_get_zero_arr_size(_tmp2E9,3));});_LL22D: if((int)_tmp2DF != 10)goto _LL22F;_LL22E:
return({const char*_tmp2EA="<=";_tag_dynforward(_tmp2EA,sizeof(char),
_get_zero_arr_size(_tmp2EA,3));});_LL22F: if((int)_tmp2DF != 11)goto _LL231;_LL230:
return({const char*_tmp2EB="!";_tag_dynforward(_tmp2EB,sizeof(char),
_get_zero_arr_size(_tmp2EB,2));});_LL231: if((int)_tmp2DF != 12)goto _LL233;_LL232:
return({const char*_tmp2EC="~";_tag_dynforward(_tmp2EC,sizeof(char),
_get_zero_arr_size(_tmp2EC,2));});_LL233: if((int)_tmp2DF != 13)goto _LL235;_LL234:
return({const char*_tmp2ED="&";_tag_dynforward(_tmp2ED,sizeof(char),
_get_zero_arr_size(_tmp2ED,2));});_LL235: if((int)_tmp2DF != 14)goto _LL237;_LL236:
return({const char*_tmp2EE="|";_tag_dynforward(_tmp2EE,sizeof(char),
_get_zero_arr_size(_tmp2EE,2));});_LL237: if((int)_tmp2DF != 15)goto _LL239;_LL238:
return({const char*_tmp2EF="^";_tag_dynforward(_tmp2EF,sizeof(char),
_get_zero_arr_size(_tmp2EF,2));});_LL239: if((int)_tmp2DF != 16)goto _LL23B;_LL23A:
return({const char*_tmp2F0="<<";_tag_dynforward(_tmp2F0,sizeof(char),
_get_zero_arr_size(_tmp2F0,3));});_LL23B: if((int)_tmp2DF != 17)goto _LL23D;_LL23C:
return({const char*_tmp2F1=">>";_tag_dynforward(_tmp2F1,sizeof(char),
_get_zero_arr_size(_tmp2F1,3));});_LL23D: if((int)_tmp2DF != 18)goto _LL23F;_LL23E:
return({const char*_tmp2F2=">>>";_tag_dynforward(_tmp2F2,sizeof(char),
_get_zero_arr_size(_tmp2F2,4));});_LL23F: if((int)_tmp2DF != 19)goto _LL218;_LL240:
return({const char*_tmp2F3="size";_tag_dynforward(_tmp2F3,sizeof(char),
_get_zero_arr_size(_tmp2F3,5));});_LL218:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp2F4=(void*)s->r;_LL242: if(_tmp2F4 <= (void*)1)
goto _LL244;if(*((int*)_tmp2F4)!= 11)goto _LL244;_LL243: return 1;_LL244:;_LL245:
return 0;_LL241:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){
struct Cyc_PP_Doc*s;{void*_tmp2F5=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F6;struct
Cyc_Absyn_Stmt*_tmp2F7;struct Cyc_Absyn_Stmt*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;
struct Cyc_Absyn_Exp*_tmp2FA;struct Cyc_Absyn_Stmt*_tmp2FB;struct Cyc_Absyn_Stmt*
_tmp2FC;struct _tuple2 _tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Stmt*
_tmp2FF;struct _dynforward_ptr*_tmp300;struct Cyc_Absyn_Exp*_tmp301;struct _tuple2
_tmp302;struct Cyc_Absyn_Exp*_tmp303;struct _tuple2 _tmp304;struct Cyc_Absyn_Exp*
_tmp305;struct Cyc_Absyn_Stmt*_tmp306;struct Cyc_Absyn_Exp*_tmp307;struct Cyc_List_List*
_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_Absyn_Decl*
_tmp30B;struct Cyc_Absyn_Stmt*_tmp30C;struct _dynforward_ptr*_tmp30D;struct Cyc_Absyn_Stmt*
_tmp30E;struct Cyc_Absyn_Stmt*_tmp30F;struct _tuple2 _tmp310;struct Cyc_Absyn_Exp*
_tmp311;struct Cyc_Absyn_Stmt*_tmp312;struct Cyc_List_List*_tmp313;struct Cyc_Absyn_Tvar*
_tmp314;struct Cyc_Absyn_Vardecl*_tmp315;int _tmp316;struct Cyc_Absyn_Exp*_tmp317;
struct Cyc_Absyn_Stmt*_tmp318;struct Cyc_Absyn_Exp*_tmp319;_LL247: if((int)_tmp2F5
!= 0)goto _LL249;_LL248: s=Cyc_PP_text(({const char*_tmp31A=";";_tag_dynforward(
_tmp31A,sizeof(char),_get_zero_arr_size(_tmp31A,2));}));goto _LL246;_LL249: if(
_tmp2F5 <= (void*)1)goto _LL24B;if(*((int*)_tmp2F5)!= 0)goto _LL24B;_tmp2F6=((
struct Cyc_Absyn_Exp_s_struct*)_tmp2F5)->f1;_LL24A: s=({struct Cyc_PP_Doc*_tmp31B[2];
_tmp31B[1]=Cyc_PP_text(({const char*_tmp31C=";";_tag_dynforward(_tmp31C,sizeof(
char),_get_zero_arr_size(_tmp31C,2));}));_tmp31B[0]=Cyc_Absynpp_exp2doc(_tmp2F6);
Cyc_PP_cat(_tag_dynforward(_tmp31B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL246;
_LL24B: if(_tmp2F5 <= (void*)1)goto _LL24D;if(*((int*)_tmp2F5)!= 1)goto _LL24D;
_tmp2F7=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F5)->f1;_tmp2F8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2F5)->f2;_LL24C: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2F7))s=({struct Cyc_PP_Doc*_tmp31D[7];_tmp31D[6]=Cyc_Absynpp_is_declaration(
_tmp2F8)?({struct Cyc_PP_Doc*_tmp31E[5];_tmp31E[4]=Cyc_PP_line_doc();_tmp31E[3]=
Cyc_Absynpp_rb();_tmp31E[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F8));_tmp31E[
1]=Cyc_PP_blank_doc();_tmp31E[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(
_tmp31E,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2F8);_tmp31D[5]=
Cyc_PP_line_doc();_tmp31D[4]=Cyc_Absynpp_rb();_tmp31D[3]=Cyc_PP_line_doc();
_tmp31D[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F7));_tmp31D[1]=Cyc_PP_blank_doc();
_tmp31D[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmp31D,sizeof(struct Cyc_PP_Doc*),
7));});else{if(Cyc_Absynpp_is_declaration(_tmp2F8))s=({struct Cyc_PP_Doc*_tmp31F[
7];_tmp31F[6]=Cyc_PP_line_doc();_tmp31F[5]=Cyc_Absynpp_rb();_tmp31F[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2F8));_tmp31F[3]=Cyc_PP_blank_doc();_tmp31F[2]=Cyc_Absynpp_lb();
_tmp31F[1]=Cyc_PP_line_doc();_tmp31F[0]=Cyc_Absynpp_stmt2doc(_tmp2F7);Cyc_PP_cat(
_tag_dynforward(_tmp31F,sizeof(struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({const char*_tmp320="";_tag_dynforward(
_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,1));}),({struct Cyc_List_List*
_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->hd=_tmp2F7;_tmp321->tl=({struct Cyc_List_List*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->hd=_tmp2F8;_tmp322->tl=0;_tmp322;});
_tmp321;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp323="";_tag_dynforward(_tmp323,sizeof(char),_get_zero_arr_size(
_tmp323,1));}),({struct Cyc_List_List*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->hd=
_tmp2F7;_tmp324->tl=({struct Cyc_List_List*_tmp325=_cycalloc(sizeof(*_tmp325));
_tmp325->hd=_tmp2F8;_tmp325->tl=0;_tmp325;});_tmp324;}));}goto _LL246;_LL24D: if(
_tmp2F5 <= (void*)1)goto _LL24F;if(*((int*)_tmp2F5)!= 2)goto _LL24F;_tmp2F9=((
struct Cyc_Absyn_Return_s_struct*)_tmp2F5)->f1;_LL24E: if(_tmp2F9 == 0)s=Cyc_PP_text(({
const char*_tmp326="return;";_tag_dynforward(_tmp326,sizeof(char),
_get_zero_arr_size(_tmp326,8));}));else{s=({struct Cyc_PP_Doc*_tmp327[3];_tmp327[
2]=Cyc_PP_text(({const char*_tmp329=";";_tag_dynforward(_tmp329,sizeof(char),
_get_zero_arr_size(_tmp329,2));}));_tmp327[1]=_tmp2F9 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2F9);_tmp327[0]=Cyc_PP_text(({const char*_tmp328="return ";
_tag_dynforward(_tmp328,sizeof(char),_get_zero_arr_size(_tmp328,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp327,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL246;_LL24F: if(
_tmp2F5 <= (void*)1)goto _LL251;if(*((int*)_tmp2F5)!= 3)goto _LL251;_tmp2FA=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F5)->f1;_tmp2FB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F5)->f2;_tmp2FC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F5)->f3;_LL250: {
int print_else;{void*_tmp32A=(void*)_tmp2FC->r;_LL26E: if((int)_tmp32A != 0)goto
_LL270;_LL26F: print_else=0;goto _LL26D;_LL270:;_LL271: print_else=1;goto _LL26D;
_LL26D:;}s=({struct Cyc_PP_Doc*_tmp32B[8];_tmp32B[7]=print_else?({struct Cyc_PP_Doc*
_tmp32F[6];_tmp32F[5]=Cyc_Absynpp_rb();_tmp32F[4]=Cyc_PP_line_doc();_tmp32F[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp331[2];_tmp331[1]=Cyc_Absynpp_stmt2doc(
_tmp2FC);_tmp331[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp331,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp32F[2]=Cyc_Absynpp_lb();_tmp32F[1]=Cyc_PP_text(({
const char*_tmp330="else ";_tag_dynforward(_tmp330,sizeof(char),
_get_zero_arr_size(_tmp330,6));}));_tmp32F[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dynforward(_tmp32F,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();
_tmp32B[6]=Cyc_Absynpp_rb();_tmp32B[5]=Cyc_PP_line_doc();_tmp32B[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp32E[2];_tmp32E[1]=Cyc_Absynpp_stmt2doc(_tmp2FB);_tmp32E[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp32E,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp32B[3]=Cyc_Absynpp_lb();_tmp32B[2]=Cyc_PP_text(({const char*_tmp32D=") ";
_tag_dynforward(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,3));}));_tmp32B[
1]=Cyc_Absynpp_exp2doc(_tmp2FA);_tmp32B[0]=Cyc_PP_text(({const char*_tmp32C="if (";
_tag_dynforward(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp32B,sizeof(struct Cyc_PP_Doc*),8));});goto _LL246;}_LL251: if(
_tmp2F5 <= (void*)1)goto _LL253;if(*((int*)_tmp2F5)!= 4)goto _LL253;_tmp2FD=((
struct Cyc_Absyn_While_s_struct*)_tmp2F5)->f1;_tmp2FE=_tmp2FD.f1;_tmp2FF=((struct
Cyc_Absyn_While_s_struct*)_tmp2F5)->f2;_LL252: s=({struct Cyc_PP_Doc*_tmp332[7];
_tmp332[6]=Cyc_Absynpp_rb();_tmp332[5]=Cyc_PP_line_doc();_tmp332[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp335[2];_tmp335[1]=Cyc_Absynpp_stmt2doc(_tmp2FF);_tmp335[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp335,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp332[3]=Cyc_Absynpp_lb();_tmp332[2]=Cyc_PP_text(({const char*_tmp334=") ";
_tag_dynforward(_tmp334,sizeof(char),_get_zero_arr_size(_tmp334,3));}));_tmp332[
1]=Cyc_Absynpp_exp2doc(_tmp2FE);_tmp332[0]=Cyc_PP_text(({const char*_tmp333="while (";
_tag_dynforward(_tmp333,sizeof(char),_get_zero_arr_size(_tmp333,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp332,sizeof(struct Cyc_PP_Doc*),7));});goto _LL246;_LL253: if(
_tmp2F5 <= (void*)1)goto _LL255;if(*((int*)_tmp2F5)!= 5)goto _LL255;_LL254: s=Cyc_PP_text(({
const char*_tmp336="break;";_tag_dynforward(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,7));}));goto _LL246;_LL255: if(_tmp2F5 <= (void*)1)goto
_LL257;if(*((int*)_tmp2F5)!= 6)goto _LL257;_LL256: s=Cyc_PP_text(({const char*
_tmp337="continue;";_tag_dynforward(_tmp337,sizeof(char),_get_zero_arr_size(
_tmp337,10));}));goto _LL246;_LL257: if(_tmp2F5 <= (void*)1)goto _LL259;if(*((int*)
_tmp2F5)!= 7)goto _LL259;_tmp300=((struct Cyc_Absyn_Goto_s_struct*)_tmp2F5)->f1;
_LL258: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp33A;
_tmp33A.tag=0;_tmp33A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp300);{
void*_tmp338[1]={& _tmp33A};Cyc_aprintf(({const char*_tmp339="goto %s;";
_tag_dynforward(_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,9));}),
_tag_dynforward(_tmp338,sizeof(void*),1));}}));goto _LL246;_LL259: if(_tmp2F5 <= (
void*)1)goto _LL25B;if(*((int*)_tmp2F5)!= 8)goto _LL25B;_tmp301=((struct Cyc_Absyn_For_s_struct*)
_tmp2F5)->f1;_tmp302=((struct Cyc_Absyn_For_s_struct*)_tmp2F5)->f2;_tmp303=
_tmp302.f1;_tmp304=((struct Cyc_Absyn_For_s_struct*)_tmp2F5)->f3;_tmp305=_tmp304.f1;
_tmp306=((struct Cyc_Absyn_For_s_struct*)_tmp2F5)->f4;_LL25A: s=({struct Cyc_PP_Doc*
_tmp33B[11];_tmp33B[10]=Cyc_Absynpp_rb();_tmp33B[9]=Cyc_PP_line_doc();_tmp33B[8]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp340[2];_tmp340[1]=Cyc_Absynpp_stmt2doc(
_tmp306);_tmp340[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp340,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp33B[7]=Cyc_Absynpp_lb();_tmp33B[6]=Cyc_PP_text(({
const char*_tmp33F=") ";_tag_dynforward(_tmp33F,sizeof(char),_get_zero_arr_size(
_tmp33F,3));}));_tmp33B[5]=Cyc_Absynpp_exp2doc(_tmp305);_tmp33B[4]=Cyc_PP_text(({
const char*_tmp33E="; ";_tag_dynforward(_tmp33E,sizeof(char),_get_zero_arr_size(
_tmp33E,3));}));_tmp33B[3]=Cyc_Absynpp_exp2doc(_tmp303);_tmp33B[2]=Cyc_PP_text(({
const char*_tmp33D="; ";_tag_dynforward(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,3));}));_tmp33B[1]=Cyc_Absynpp_exp2doc(_tmp301);_tmp33B[0]=Cyc_PP_text(({
const char*_tmp33C="for(";_tag_dynforward(_tmp33C,sizeof(char),_get_zero_arr_size(
_tmp33C,5));}));Cyc_PP_cat(_tag_dynforward(_tmp33B,sizeof(struct Cyc_PP_Doc*),11));});
goto _LL246;_LL25B: if(_tmp2F5 <= (void*)1)goto _LL25D;if(*((int*)_tmp2F5)!= 9)goto
_LL25D;_tmp307=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F5)->f1;_tmp308=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2F5)->f2;_LL25C: s=({struct Cyc_PP_Doc*_tmp341[8];
_tmp341[7]=Cyc_Absynpp_rb();_tmp341[6]=Cyc_PP_line_doc();_tmp341[5]=Cyc_Absynpp_switchclauses2doc(
_tmp308);_tmp341[4]=Cyc_PP_line_doc();_tmp341[3]=Cyc_Absynpp_lb();_tmp341[2]=Cyc_PP_text(({
const char*_tmp343=") ";_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(
_tmp343,3));}));_tmp341[1]=Cyc_Absynpp_exp2doc(_tmp307);_tmp341[0]=Cyc_PP_text(({
const char*_tmp342="switch (";_tag_dynforward(_tmp342,sizeof(char),
_get_zero_arr_size(_tmp342,9));}));Cyc_PP_cat(_tag_dynforward(_tmp341,sizeof(
struct Cyc_PP_Doc*),8));});goto _LL246;_LL25D: if(_tmp2F5 <= (void*)1)goto _LL25F;if(*((
int*)_tmp2F5)!= 10)goto _LL25F;_tmp309=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F5)->f1;if(_tmp309 != 0)goto _LL25F;_LL25E: s=Cyc_PP_text(({const char*_tmp344="fallthru;";
_tag_dynforward(_tmp344,sizeof(char),_get_zero_arr_size(_tmp344,10));}));goto
_LL246;_LL25F: if(_tmp2F5 <= (void*)1)goto _LL261;if(*((int*)_tmp2F5)!= 10)goto
_LL261;_tmp30A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F5)->f1;_LL260: s=({
struct Cyc_PP_Doc*_tmp345[3];_tmp345[2]=Cyc_PP_text(({const char*_tmp347=");";
_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,3));}));_tmp345[
1]=Cyc_Absynpp_exps2doc_prec(20,_tmp30A);_tmp345[0]=Cyc_PP_text(({const char*
_tmp346="fallthru(";_tag_dynforward(_tmp346,sizeof(char),_get_zero_arr_size(
_tmp346,10));}));Cyc_PP_cat(_tag_dynforward(_tmp345,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL246;_LL261: if(_tmp2F5 <= (void*)1)goto _LL263;if(*((int*)_tmp2F5)!= 11)goto
_LL263;_tmp30B=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F5)->f1;_tmp30C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F5)->f2;_LL262: s=({struct Cyc_PP_Doc*_tmp348[3];_tmp348[2]=Cyc_Absynpp_stmt2doc(
_tmp30C);_tmp348[1]=Cyc_PP_line_doc();_tmp348[0]=Cyc_Absynpp_decl2doc(_tmp30B);
Cyc_PP_cat(_tag_dynforward(_tmp348,sizeof(struct Cyc_PP_Doc*),3));});goto _LL246;
_LL263: if(_tmp2F5 <= (void*)1)goto _LL265;if(*((int*)_tmp2F5)!= 12)goto _LL265;
_tmp30D=((struct Cyc_Absyn_Label_s_struct*)_tmp2F5)->f1;_tmp30E=((struct Cyc_Absyn_Label_s_struct*)
_tmp2F5)->f2;_LL264: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp30E))s=({struct Cyc_PP_Doc*_tmp349[7];_tmp349[6]=Cyc_Absynpp_rb();_tmp349[5]=
Cyc_PP_line_doc();_tmp349[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30E));
_tmp349[3]=Cyc_PP_line_doc();_tmp349[2]=Cyc_Absynpp_lb();_tmp349[1]=Cyc_PP_text(({
const char*_tmp34A=": ";_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(
_tmp34A,3));}));_tmp349[0]=Cyc_PP_textptr(_tmp30D);Cyc_PP_cat(_tag_dynforward(
_tmp349,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp34B[3];
_tmp34B[2]=Cyc_Absynpp_stmt2doc(_tmp30E);_tmp34B[1]=Cyc_PP_text(({const char*
_tmp34C=": ";_tag_dynforward(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,3));}));
_tmp34B[0]=Cyc_PP_textptr(_tmp30D);Cyc_PP_cat(_tag_dynforward(_tmp34B,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL246;_LL265: if(_tmp2F5 <= (void*)1)goto _LL267;
if(*((int*)_tmp2F5)!= 13)goto _LL267;_tmp30F=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F5)->f1;_tmp310=((struct Cyc_Absyn_Do_s_struct*)_tmp2F5)->f2;_tmp311=_tmp310.f1;
_LL266: s=({struct Cyc_PP_Doc*_tmp34D[9];_tmp34D[8]=Cyc_PP_text(({const char*
_tmp350=");";_tag_dynforward(_tmp350,sizeof(char),_get_zero_arr_size(_tmp350,3));}));
_tmp34D[7]=Cyc_Absynpp_exp2doc(_tmp311);_tmp34D[6]=Cyc_PP_text(({const char*
_tmp34F=" while (";_tag_dynforward(_tmp34F,sizeof(char),_get_zero_arr_size(
_tmp34F,9));}));_tmp34D[5]=Cyc_Absynpp_rb();_tmp34D[4]=Cyc_PP_line_doc();_tmp34D[
3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30F));_tmp34D[2]=Cyc_PP_line_doc();
_tmp34D[1]=Cyc_Absynpp_lb();_tmp34D[0]=Cyc_PP_text(({const char*_tmp34E="do ";
_tag_dynforward(_tmp34E,sizeof(char),_get_zero_arr_size(_tmp34E,4));}));Cyc_PP_cat(
_tag_dynforward(_tmp34D,sizeof(struct Cyc_PP_Doc*),9));});goto _LL246;_LL267: if(
_tmp2F5 <= (void*)1)goto _LL269;if(*((int*)_tmp2F5)!= 14)goto _LL269;_tmp312=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F5)->f1;_tmp313=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F5)->f2;_LL268: s=({struct Cyc_PP_Doc*_tmp351[12];_tmp351[11]=Cyc_Absynpp_rb();
_tmp351[10]=Cyc_PP_line_doc();_tmp351[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp313));_tmp351[8]=Cyc_PP_line_doc();_tmp351[7]=Cyc_Absynpp_lb();_tmp351[6]=
Cyc_PP_text(({const char*_tmp353=" catch ";_tag_dynforward(_tmp353,sizeof(char),
_get_zero_arr_size(_tmp353,8));}));_tmp351[5]=Cyc_Absynpp_rb();_tmp351[4]=Cyc_PP_line_doc();
_tmp351[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp312));_tmp351[2]=Cyc_PP_line_doc();
_tmp351[1]=Cyc_Absynpp_lb();_tmp351[0]=Cyc_PP_text(({const char*_tmp352="try ";
_tag_dynforward(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp351,sizeof(struct Cyc_PP_Doc*),12));});goto _LL246;_LL269: if(
_tmp2F5 <= (void*)1)goto _LL26B;if(*((int*)_tmp2F5)!= 15)goto _LL26B;_tmp314=((
struct Cyc_Absyn_Region_s_struct*)_tmp2F5)->f1;_tmp315=((struct Cyc_Absyn_Region_s_struct*)
_tmp2F5)->f2;_tmp316=((struct Cyc_Absyn_Region_s_struct*)_tmp2F5)->f3;_tmp317=((
struct Cyc_Absyn_Region_s_struct*)_tmp2F5)->f4;_tmp318=((struct Cyc_Absyn_Region_s_struct*)
_tmp2F5)->f5;_LL26A: s=({struct Cyc_PP_Doc*_tmp354[12];_tmp354[11]=Cyc_Absynpp_rb();
_tmp354[10]=Cyc_PP_line_doc();_tmp354[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp318));_tmp354[8]=Cyc_PP_line_doc();_tmp354[7]=Cyc_Absynpp_lb();_tmp354[6]=
_tmp317 != 0?({struct Cyc_PP_Doc*_tmp359[3];_tmp359[2]=Cyc_PP_text(({const char*
_tmp35B=")";_tag_dynforward(_tmp35B,sizeof(char),_get_zero_arr_size(_tmp35B,2));}));
_tmp359[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp317);_tmp359[0]=Cyc_PP_text(({
const char*_tmp35A=" = open(";_tag_dynforward(_tmp35A,sizeof(char),
_get_zero_arr_size(_tmp35A,9));}));Cyc_PP_cat(_tag_dynforward(_tmp359,sizeof(
struct Cyc_PP_Doc*),3));}): Cyc_PP_nil_doc();_tmp354[5]=Cyc_Absynpp_qvar2doc(
_tmp315->name);_tmp354[4]=Cyc_PP_text(({const char*_tmp358=">";_tag_dynforward(
_tmp358,sizeof(char),_get_zero_arr_size(_tmp358,2));}));_tmp354[3]=Cyc_PP_textptr(
Cyc_Absynpp_get_name(_tmp314));_tmp354[2]=Cyc_PP_text(({const char*_tmp357="<";
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size(_tmp357,2));}));_tmp354[
1]=_tmp316?Cyc_PP_nil_doc(): Cyc_PP_text(({const char*_tmp356="[resetable]";
_tag_dynforward(_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,12));}));_tmp354[
0]=Cyc_PP_text(({const char*_tmp355="region";_tag_dynforward(_tmp355,sizeof(char),
_get_zero_arr_size(_tmp355,7));}));Cyc_PP_cat(_tag_dynforward(_tmp354,sizeof(
struct Cyc_PP_Doc*),12));});goto _LL246;_LL26B: if(_tmp2F5 <= (void*)1)goto _LL246;
if(*((int*)_tmp2F5)!= 16)goto _LL246;_tmp319=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2F5)->f1;_LL26C: s=({struct Cyc_PP_Doc*_tmp35C[3];_tmp35C[2]=Cyc_PP_text(({
const char*_tmp35E=");";_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size(
_tmp35E,3));}));_tmp35C[1]=Cyc_Absynpp_exp2doc(_tmp319);_tmp35C[0]=Cyc_PP_text(({
const char*_tmp35D="reset_region(";_tag_dynforward(_tmp35D,sizeof(char),
_get_zero_arr_size(_tmp35D,14));}));Cyc_PP_cat(_tag_dynforward(_tmp35C,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL246;_LL246:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp35F=(void*)p->r;void*_tmp360;
int _tmp361;char _tmp362;struct _dynforward_ptr _tmp363;struct Cyc_Absyn_Vardecl*
_tmp364;struct Cyc_Absyn_Pat*_tmp365;struct Cyc_Absyn_Pat _tmp366;void*_tmp367;
struct Cyc_Absyn_Vardecl*_tmp368;struct Cyc_Absyn_Pat*_tmp369;struct Cyc_Absyn_Tvar*
_tmp36A;struct Cyc_Absyn_Vardecl*_tmp36B;struct Cyc_List_List*_tmp36C;int _tmp36D;
struct Cyc_Absyn_Pat*_tmp36E;struct Cyc_Absyn_Vardecl*_tmp36F;struct Cyc_Absyn_Pat*
_tmp370;struct Cyc_Absyn_Pat _tmp371;void*_tmp372;struct Cyc_Absyn_Vardecl*_tmp373;
struct Cyc_Absyn_Pat*_tmp374;struct _tuple0*_tmp375;struct _tuple0*_tmp376;struct
Cyc_List_List*_tmp377;int _tmp378;struct Cyc_Absyn_AggrInfo _tmp379;union Cyc_Absyn_AggrInfoU_union
_tmp37A;struct Cyc_List_List*_tmp37B;struct Cyc_List_List*_tmp37C;int _tmp37D;
struct Cyc_Absyn_Enumfield*_tmp37E;struct Cyc_Absyn_Enumfield*_tmp37F;struct Cyc_Absyn_Tunionfield*
_tmp380;struct Cyc_List_List*_tmp381;struct Cyc_Absyn_Tunionfield*_tmp382;struct
Cyc_List_List*_tmp383;int _tmp384;struct Cyc_Absyn_Exp*_tmp385;_LL273: if((int)
_tmp35F != 0)goto _LL275;_LL274: s=Cyc_PP_text(({const char*_tmp386="_";
_tag_dynforward(_tmp386,sizeof(char),_get_zero_arr_size(_tmp386,2));}));goto
_LL272;_LL275: if((int)_tmp35F != 1)goto _LL277;_LL276: s=Cyc_PP_text(({const char*
_tmp387="NULL";_tag_dynforward(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,5));}));
goto _LL272;_LL277: if(_tmp35F <= (void*)2)goto _LL279;if(*((int*)_tmp35F)!= 7)goto
_LL279;_tmp360=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp35F)->f1;_tmp361=((
struct Cyc_Absyn_Int_p_struct*)_tmp35F)->f2;_LL278: if(_tmp360 != (void*)((void*)1))
s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp38A;_tmp38A.tag=
1;_tmp38A.f1=(unsigned long)_tmp361;{void*_tmp388[1]={& _tmp38A};Cyc_aprintf(({
const char*_tmp389="%d";_tag_dynforward(_tmp389,sizeof(char),_get_zero_arr_size(
_tmp389,3));}),_tag_dynforward(_tmp388,sizeof(void*),1));}}));else{s=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp38D;_tmp38D.tag=1;_tmp38D.f1=(
unsigned int)_tmp361;{void*_tmp38B[1]={& _tmp38D};Cyc_aprintf(({const char*_tmp38C="%u";
_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,3));}),
_tag_dynforward(_tmp38B,sizeof(void*),1));}}));}goto _LL272;_LL279: if(_tmp35F <= (
void*)2)goto _LL27B;if(*((int*)_tmp35F)!= 8)goto _LL27B;_tmp362=((struct Cyc_Absyn_Char_p_struct*)
_tmp35F)->f1;_LL27A: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp390;_tmp390.tag=0;_tmp390.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_char_escape(_tmp362));{void*_tmp38E[1]={& _tmp390};Cyc_aprintf(({
const char*_tmp38F="'%s'";_tag_dynforward(_tmp38F,sizeof(char),_get_zero_arr_size(
_tmp38F,5));}),_tag_dynforward(_tmp38E,sizeof(void*),1));}}));goto _LL272;_LL27B:
if(_tmp35F <= (void*)2)goto _LL27D;if(*((int*)_tmp35F)!= 9)goto _LL27D;_tmp363=((
struct Cyc_Absyn_Float_p_struct*)_tmp35F)->f1;_LL27C: s=Cyc_PP_text(_tmp363);goto
_LL272;_LL27D: if(_tmp35F <= (void*)2)goto _LL27F;if(*((int*)_tmp35F)!= 0)goto
_LL27F;_tmp364=((struct Cyc_Absyn_Var_p_struct*)_tmp35F)->f1;_tmp365=((struct Cyc_Absyn_Var_p_struct*)
_tmp35F)->f2;_tmp366=*_tmp365;_tmp367=(void*)_tmp366.r;if((int)_tmp367 != 0)goto
_LL27F;_LL27E: s=Cyc_Absynpp_qvar2doc(_tmp364->name);goto _LL272;_LL27F: if(_tmp35F
<= (void*)2)goto _LL281;if(*((int*)_tmp35F)!= 0)goto _LL281;_tmp368=((struct Cyc_Absyn_Var_p_struct*)
_tmp35F)->f1;_tmp369=((struct Cyc_Absyn_Var_p_struct*)_tmp35F)->f2;_LL280: s=({
struct Cyc_PP_Doc*_tmp391[3];_tmp391[2]=Cyc_Absynpp_pat2doc(_tmp369);_tmp391[1]=
Cyc_PP_text(({const char*_tmp392=" as ";_tag_dynforward(_tmp392,sizeof(char),
_get_zero_arr_size(_tmp392,5));}));_tmp391[0]=Cyc_Absynpp_qvar2doc(_tmp368->name);
Cyc_PP_cat(_tag_dynforward(_tmp391,sizeof(struct Cyc_PP_Doc*),3));});goto _LL272;
_LL281: if(_tmp35F <= (void*)2)goto _LL283;if(*((int*)_tmp35F)!= 2)goto _LL283;
_tmp36A=((struct Cyc_Absyn_TagInt_p_struct*)_tmp35F)->f1;_tmp36B=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp35F)->f2;_LL282: s=({struct Cyc_PP_Doc*_tmp393[4];_tmp393[3]=Cyc_PP_text(({
const char*_tmp395=">";_tag_dynforward(_tmp395,sizeof(char),_get_zero_arr_size(
_tmp395,2));}));_tmp393[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp36A));_tmp393[
1]=Cyc_PP_text(({const char*_tmp394="<";_tag_dynforward(_tmp394,sizeof(char),
_get_zero_arr_size(_tmp394,2));}));_tmp393[0]=Cyc_Absynpp_qvar2doc(_tmp36B->name);
Cyc_PP_cat(_tag_dynforward(_tmp393,sizeof(struct Cyc_PP_Doc*),4));});goto _LL272;
_LL283: if(_tmp35F <= (void*)2)goto _LL285;if(*((int*)_tmp35F)!= 3)goto _LL285;
_tmp36C=((struct Cyc_Absyn_Tuple_p_struct*)_tmp35F)->f1;_tmp36D=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp35F)->f2;_LL284: s=({struct Cyc_PP_Doc*_tmp396[4];_tmp396[3]=_tmp36D?Cyc_PP_text(({
const char*_tmp399=", ...)";_tag_dynforward(_tmp399,sizeof(char),
_get_zero_arr_size(_tmp399,7));})): Cyc_PP_text(({const char*_tmp39A=")";
_tag_dynforward(_tmp39A,sizeof(char),_get_zero_arr_size(_tmp39A,2));}));_tmp396[
2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({
const char*_tmp398=",";_tag_dynforward(_tmp398,sizeof(char),_get_zero_arr_size(
_tmp398,2));}),_tmp36C);_tmp396[1]=Cyc_PP_text(({const char*_tmp397="(";
_tag_dynforward(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,2));}));_tmp396[
0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp396,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL272;_LL285: if(_tmp35F <= (void*)2)goto _LL287;if(*((int*)_tmp35F)!= 
4)goto _LL287;_tmp36E=((struct Cyc_Absyn_Pointer_p_struct*)_tmp35F)->f1;_LL286: s=({
struct Cyc_PP_Doc*_tmp39B[2];_tmp39B[1]=Cyc_Absynpp_pat2doc(_tmp36E);_tmp39B[0]=
Cyc_PP_text(({const char*_tmp39C="&";_tag_dynforward(_tmp39C,sizeof(char),
_get_zero_arr_size(_tmp39C,2));}));Cyc_PP_cat(_tag_dynforward(_tmp39B,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL272;_LL287: if(_tmp35F <= (void*)2)goto _LL289;if(*((
int*)_tmp35F)!= 1)goto _LL289;_tmp36F=((struct Cyc_Absyn_Reference_p_struct*)
_tmp35F)->f1;_tmp370=((struct Cyc_Absyn_Reference_p_struct*)_tmp35F)->f2;_tmp371=*
_tmp370;_tmp372=(void*)_tmp371.r;if((int)_tmp372 != 0)goto _LL289;_LL288: s=({
struct Cyc_PP_Doc*_tmp39D[2];_tmp39D[1]=Cyc_Absynpp_qvar2doc(_tmp36F->name);
_tmp39D[0]=Cyc_PP_text(({const char*_tmp39E="*";_tag_dynforward(_tmp39E,sizeof(
char),_get_zero_arr_size(_tmp39E,2));}));Cyc_PP_cat(_tag_dynforward(_tmp39D,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL272;_LL289: if(_tmp35F <= (void*)2)goto
_LL28B;if(*((int*)_tmp35F)!= 1)goto _LL28B;_tmp373=((struct Cyc_Absyn_Reference_p_struct*)
_tmp35F)->f1;_tmp374=((struct Cyc_Absyn_Reference_p_struct*)_tmp35F)->f2;_LL28A: s=({
struct Cyc_PP_Doc*_tmp39F[4];_tmp39F[3]=Cyc_Absynpp_pat2doc(_tmp374);_tmp39F[2]=
Cyc_PP_text(({const char*_tmp3A1=" as ";_tag_dynforward(_tmp3A1,sizeof(char),
_get_zero_arr_size(_tmp3A1,5));}));_tmp39F[1]=Cyc_Absynpp_qvar2doc(_tmp373->name);
_tmp39F[0]=Cyc_PP_text(({const char*_tmp3A0="*";_tag_dynforward(_tmp3A0,sizeof(
char),_get_zero_arr_size(_tmp3A0,2));}));Cyc_PP_cat(_tag_dynforward(_tmp39F,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL272;_LL28B: if(_tmp35F <= (void*)2)goto
_LL28D;if(*((int*)_tmp35F)!= 12)goto _LL28D;_tmp375=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp35F)->f1;_LL28C: s=Cyc_Absynpp_qvar2doc(_tmp375);goto _LL272;_LL28D: if(_tmp35F
<= (void*)2)goto _LL28F;if(*((int*)_tmp35F)!= 13)goto _LL28F;_tmp376=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp35F)->f1;_tmp377=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp35F)->f2;
_tmp378=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp35F)->f3;_LL28E: {struct
_dynforward_ptr term=_tmp378?({const char*_tmp3A5=", ...)";_tag_dynforward(_tmp3A5,
sizeof(char),_get_zero_arr_size(_tmp3A5,7));}):({const char*_tmp3A6=")";
_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,2));});s=({
struct Cyc_PP_Doc*_tmp3A2[2];_tmp3A2[1]=Cyc_PP_group(({const char*_tmp3A3="(";
_tag_dynforward(_tmp3A3,sizeof(char),_get_zero_arr_size(_tmp3A3,2));}),term,({
const char*_tmp3A4=",";_tag_dynforward(_tmp3A4,sizeof(char),_get_zero_arr_size(
_tmp3A4,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp377));_tmp3A2[0]=
Cyc_Absynpp_qvar2doc(_tmp376);Cyc_PP_cat(_tag_dynforward(_tmp3A2,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL272;}_LL28F: if(_tmp35F <= (void*)2)goto _LL291;if(*((
int*)_tmp35F)!= 5)goto _LL291;_tmp379=((struct Cyc_Absyn_Aggr_p_struct*)_tmp35F)->f1;
_tmp37A=_tmp379.aggr_info;_tmp37B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp35F)->f2;
_tmp37C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp35F)->f3;_tmp37D=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp35F)->f4;_LL290: {struct _dynforward_ptr term=_tmp37D?({const char*_tmp3AF=", ...}";
_tag_dynforward(_tmp3AF,sizeof(char),_get_zero_arr_size(_tmp3AF,7));}):({const
char*_tmp3B0="}";_tag_dynforward(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,
2));});struct _tuple0*_tmp3A8;struct _tuple3 _tmp3A7=Cyc_Absyn_aggr_kinded_name(
_tmp37A);_tmp3A8=_tmp3A7.f2;s=({struct Cyc_PP_Doc*_tmp3A9[4];_tmp3A9[3]=Cyc_PP_group(({
const char*_tmp3AD="";_tag_dynforward(_tmp3AD,sizeof(char),_get_zero_arr_size(
_tmp3AD,1));}),term,({const char*_tmp3AE=",";_tag_dynforward(_tmp3AE,sizeof(char),
_get_zero_arr_size(_tmp3AE,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp37C));
_tmp3A9[2]=Cyc_PP_egroup(({const char*_tmp3AA="[";_tag_dynforward(_tmp3AA,sizeof(
char),_get_zero_arr_size(_tmp3AA,2));}),({const char*_tmp3AB="]";_tag_dynforward(
_tmp3AB,sizeof(char),_get_zero_arr_size(_tmp3AB,2));}),({const char*_tmp3AC=",";
_tag_dynforward(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp37B)));_tmp3A9[1]=Cyc_Absynpp_lb();_tmp3A9[0]=Cyc_Absynpp_qvar2doc(_tmp3A8);
Cyc_PP_cat(_tag_dynforward(_tmp3A9,sizeof(struct Cyc_PP_Doc*),4));});goto _LL272;}
_LL291: if(_tmp35F <= (void*)2)goto _LL293;if(*((int*)_tmp35F)!= 10)goto _LL293;
_tmp37E=((struct Cyc_Absyn_Enum_p_struct*)_tmp35F)->f2;_LL292: _tmp37F=_tmp37E;
goto _LL294;_LL293: if(_tmp35F <= (void*)2)goto _LL295;if(*((int*)_tmp35F)!= 11)goto
_LL295;_tmp37F=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp35F)->f2;_LL294: s=Cyc_Absynpp_qvar2doc(
_tmp37F->name);goto _LL272;_LL295: if(_tmp35F <= (void*)2)goto _LL297;if(*((int*)
_tmp35F)!= 6)goto _LL297;_tmp380=((struct Cyc_Absyn_Tunion_p_struct*)_tmp35F)->f2;
_tmp381=((struct Cyc_Absyn_Tunion_p_struct*)_tmp35F)->f3;if(_tmp381 != 0)goto
_LL297;_LL296: s=Cyc_Absynpp_qvar2doc(_tmp380->name);goto _LL272;_LL297: if(_tmp35F
<= (void*)2)goto _LL299;if(*((int*)_tmp35F)!= 6)goto _LL299;_tmp382=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp35F)->f2;_tmp383=((struct Cyc_Absyn_Tunion_p_struct*)_tmp35F)->f3;_tmp384=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp35F)->f4;_LL298: {struct _dynforward_ptr term=
_tmp384?({const char*_tmp3B4=", ...)";_tag_dynforward(_tmp3B4,sizeof(char),
_get_zero_arr_size(_tmp3B4,7));}):({const char*_tmp3B5=")";_tag_dynforward(
_tmp3B5,sizeof(char),_get_zero_arr_size(_tmp3B5,2));});s=({struct Cyc_PP_Doc*
_tmp3B1[2];_tmp3B1[1]=Cyc_PP_egroup(({const char*_tmp3B2="(";_tag_dynforward(
_tmp3B2,sizeof(char),_get_zero_arr_size(_tmp3B2,2));}),term,({const char*_tmp3B3=",";
_tag_dynforward(_tmp3B3,sizeof(char),_get_zero_arr_size(_tmp3B3,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp383));_tmp3B1[0]=Cyc_Absynpp_qvar2doc(
_tmp382->name);Cyc_PP_cat(_tag_dynforward(_tmp3B1,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL272;}_LL299: if(_tmp35F <= (void*)2)goto _LL272;if(*((int*)_tmp35F)!= 14)
goto _LL272;_tmp385=((struct Cyc_Absyn_Exp_p_struct*)_tmp35F)->f1;_LL29A: s=Cyc_Absynpp_exp2doc(
_tmp385);goto _LL272;_LL272:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp){return({struct Cyc_PP_Doc*_tmp3B6[2];_tmp3B6[1]=Cyc_Absynpp_pat2doc((*
dp).f2);_tmp3B6[0]=Cyc_PP_egroup(({const char*_tmp3B7="";_tag_dynforward(_tmp3B7,
sizeof(char),_get_zero_arr_size(_tmp3B7,1));}),({const char*_tmp3B8="=";
_tag_dynforward(_tmp3B8,sizeof(char),_get_zero_arr_size(_tmp3B8,2));}),({const
char*_tmp3B9="=";_tag_dynforward(_tmp3B9,sizeof(char),_get_zero_arr_size(_tmp3B9,
2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(
_tag_dynforward(_tmp3B6,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (void*)(c->pattern)->r
== (void*)((void*)0))return({struct Cyc_PP_Doc*_tmp3BA[2];_tmp3BA[1]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3BC[2];_tmp3BC[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3BC[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3BC,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3BA[0]=Cyc_PP_text(({const char*_tmp3BB="default: ";_tag_dynforward(
_tmp3BB,sizeof(char),_get_zero_arr_size(_tmp3BB,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp3BA,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause
== 0)return({struct Cyc_PP_Doc*_tmp3BD[4];_tmp3BD[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3C0[2];_tmp3C0[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3C0[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3C0,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3BD[2]=
Cyc_PP_text(({const char*_tmp3BF=": ";_tag_dynforward(_tmp3BF,sizeof(char),
_get_zero_arr_size(_tmp3BF,3));}));_tmp3BD[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3BD[0]=Cyc_PP_text(({const char*_tmp3BE="case ";_tag_dynforward(_tmp3BE,
sizeof(char),_get_zero_arr_size(_tmp3BE,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3BD,sizeof(struct Cyc_PP_Doc*),4));});else{return({struct Cyc_PP_Doc*_tmp3C1[6];
_tmp3C1[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3C5[2];_tmp3C5[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3C5[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3C5,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3C1[4]=Cyc_PP_text(({const char*_tmp3C4=": ";
_tag_dynforward(_tmp3C4,sizeof(char),_get_zero_arr_size(_tmp3C4,3));}));_tmp3C1[
3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp3C1[2]=Cyc_PP_text(({const char*_tmp3C3=" && ";_tag_dynforward(_tmp3C3,
sizeof(char),_get_zero_arr_size(_tmp3C3,5));}));_tmp3C1[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3C1[0]=Cyc_PP_text(({const char*_tmp3C2="case ";_tag_dynforward(_tmp3C2,
sizeof(char),_get_zero_arr_size(_tmp3C2,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3C1,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp3C6="";
_tag_dynforward(_tmp3C6,sizeof(char),_get_zero_arr_size(_tmp3C6,1));}),cs);}
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag
== 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp3C7[3];
_tmp3C7[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3C7[1]=Cyc_PP_text(({const char*_tmp3C8=" = ";_tag_dynforward(_tmp3C8,sizeof(
char),_get_zero_arr_size(_tmp3C8,4));}));_tmp3C7[0]=Cyc_Absynpp_qvar2doc(f->name);
Cyc_PP_cat(_tag_dynforward(_tmp3C7,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*
_tmp3C9=",";_tag_dynforward(_tmp3C9,sizeof(char),_get_zero_arr_size(_tmp3C9,2));}),
fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp3CA=",";_tag_dynforward(_tmp3CA,sizeof(char),_get_zero_arr_size(
_tmp3CA,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp3CC;void*_tmp3CD;struct _tuple0*_tmp3CE;struct Cyc_Absyn_Tqual
_tmp3CF;void*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_List_List*_tmp3D2;
struct Cyc_Absyn_Vardecl*_tmp3CB=vd;_tmp3CC=*_tmp3CB;_tmp3CD=(void*)_tmp3CC.sc;
_tmp3CE=_tmp3CC.name;_tmp3CF=_tmp3CC.tq;_tmp3D0=(void*)_tmp3CC.type;_tmp3D1=
_tmp3CC.initializer;_tmp3D2=_tmp3CC.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3CE);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3D2);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp3D3=Cyc_Tcutil_compress(_tmp3D0);struct Cyc_Absyn_FnInfo
_tmp3D4;struct Cyc_List_List*_tmp3D5;_LL29C: if(_tmp3D3 <= (void*)4)goto _LL29E;if(*((
int*)_tmp3D3)!= 8)goto _LL29E;_tmp3D4=((struct Cyc_Absyn_FnType_struct*)_tmp3D3)->f1;
_tmp3D5=_tmp3D4.attributes;_LL29D: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3D5);
goto _LL29B;_LL29E:;_LL29F: beforenamedoc=Cyc_PP_nil_doc();goto _LL29B;_LL29B:;}s=({
struct Cyc_PP_Doc*_tmp3D6[5];_tmp3D6[4]=Cyc_PP_text(({const char*_tmp3DB=";";
_tag_dynforward(_tmp3DB,sizeof(char),_get_zero_arr_size(_tmp3DB,2));}));_tmp3D6[
3]=_tmp3D1 == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp3D9[2];_tmp3D9[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3D1);_tmp3D9[0]=Cyc_PP_text(({const char*_tmp3DA=" = ";
_tag_dynforward(_tmp3DA,sizeof(char),_get_zero_arr_size(_tmp3DA,4));}));Cyc_PP_cat(
_tag_dynforward(_tmp3D9,sizeof(struct Cyc_PP_Doc*),2));});_tmp3D6[2]=Cyc_Absynpp_tqtd2doc(
_tmp3CF,_tmp3D0,({struct Cyc_Core_Opt*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->v=({
struct Cyc_PP_Doc*_tmp3D8[2];_tmp3D8[1]=sn;_tmp3D8[0]=beforenamedoc;Cyc_PP_cat(
_tag_dynforward(_tmp3D8,sizeof(struct Cyc_PP_Doc*),2));});_tmp3D7;}));_tmp3D6[1]=
Cyc_Absynpp_scope2doc(_tmp3CD);_tmp3D6[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3D6,sizeof(struct Cyc_PP_Doc*),5));});return s;}}
struct _tuple11{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*
Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*_tmp3DD;struct _tuple11
_tmp3DC=*x;_tmp3DD=_tmp3DC.f2;return Cyc_Absynpp_qvar2doc(_tmp3DD);}struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3DE=(
void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3DF;struct Cyc_Absyn_Aggrdecl*_tmp3E0;
struct Cyc_Absyn_Vardecl*_tmp3E1;struct Cyc_Absyn_Tuniondecl*_tmp3E2;struct Cyc_Absyn_Tuniondecl
_tmp3E3;void*_tmp3E4;struct _tuple0*_tmp3E5;struct Cyc_List_List*_tmp3E6;struct Cyc_Core_Opt*
_tmp3E7;int _tmp3E8;int _tmp3E9;struct Cyc_Absyn_Pat*_tmp3EA;struct Cyc_Absyn_Exp*
_tmp3EB;struct Cyc_List_List*_tmp3EC;struct Cyc_Absyn_Enumdecl*_tmp3ED;struct Cyc_Absyn_Enumdecl
_tmp3EE;void*_tmp3EF;struct _tuple0*_tmp3F0;struct Cyc_Core_Opt*_tmp3F1;struct Cyc_Absyn_Typedefdecl*
_tmp3F2;struct _dynforward_ptr*_tmp3F3;struct Cyc_List_List*_tmp3F4;struct _tuple0*
_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F7;struct Cyc_List_List*
_tmp3F8;struct Cyc_List_List*_tmp3F9;_LL2A1: if(_tmp3DE <= (void*)2)goto _LL2B9;if(*((
int*)_tmp3DE)!= 1)goto _LL2A3;_tmp3DF=((struct Cyc_Absyn_Fn_d_struct*)_tmp3DE)->f1;
_LL2A2: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp402=_cycalloc(sizeof(*
_tmp402));_tmp402[0]=({struct Cyc_Absyn_FnType_struct _tmp403;_tmp403.tag=8;
_tmp403.f1=({struct Cyc_Absyn_FnInfo _tmp404;_tmp404.tvars=_tmp3DF->tvs;_tmp404.effect=
_tmp3DF->effect;_tmp404.ret_typ=(void*)((void*)_tmp3DF->ret_type);_tmp404.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3DF->args);_tmp404.c_varargs=_tmp3DF->c_varargs;
_tmp404.cyc_varargs=_tmp3DF->cyc_varargs;_tmp404.rgn_po=_tmp3DF->rgn_po;_tmp404.attributes=
0;_tmp404;});_tmp403;});_tmp402;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3DF->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3DF->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp3FA="__inline ";_tag_dynforward(_tmp3FA,
sizeof(char),_get_zero_arr_size(_tmp3FA,10));}));else{inlinedoc=Cyc_PP_text(({
const char*_tmp3FB="inline ";_tag_dynforward(_tmp3FB,sizeof(char),
_get_zero_arr_size(_tmp3FB,8));}));}}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp3DF->sc);struct Cyc_PP_Doc*
beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(_tmp3DF->attributes):
attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp3DF->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,({
struct Cyc_Core_Opt*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->v=({struct Cyc_PP_Doc*
_tmp401[2];_tmp401[1]=namedoc;_tmp401[0]=beforenamedoc;Cyc_PP_cat(
_tag_dynforward(_tmp401,sizeof(struct Cyc_PP_Doc*),2));});_tmp400;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp3FE[5];_tmp3FE[4]=Cyc_Absynpp_rb();_tmp3FE[3]=Cyc_PP_line_doc();
_tmp3FE[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3FF[2];_tmp3FF[1]=Cyc_Absynpp_stmt2doc(
_tmp3DF->body);_tmp3FF[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3FF,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp3FE[1]=Cyc_Absynpp_lb();_tmp3FE[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3FE,sizeof(struct Cyc_PP_Doc*),5));});s=({struct
Cyc_PP_Doc*_tmp3FC[4];_tmp3FC[3]=bodydoc;_tmp3FC[2]=tqtddoc;_tmp3FC[1]=scopedoc;
_tmp3FC[0]=inlinedoc;Cyc_PP_cat(_tag_dynforward(_tmp3FC,sizeof(struct Cyc_PP_Doc*),
4));});if(Cyc_Absynpp_to_VC)s=({struct Cyc_PP_Doc*_tmp3FD[2];_tmp3FD[1]=s;_tmp3FD[
0]=attsdoc;Cyc_PP_cat(_tag_dynforward(_tmp3FD,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2A0;}}_LL2A3: if(*((int*)_tmp3DE)!= 4)goto _LL2A5;_tmp3E0=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp3DE)->f1;_LL2A4: if(_tmp3E0->impl == 0)s=({struct Cyc_PP_Doc*_tmp405[5];_tmp405[
4]=Cyc_PP_text(({const char*_tmp406=";";_tag_dynforward(_tmp406,sizeof(char),
_get_zero_arr_size(_tmp406,2));}));_tmp405[3]=Cyc_Absynpp_ktvars2doc(_tmp3E0->tvs);
_tmp405[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3E0->name);_tmp405[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp3E0->kind);_tmp405[0]=Cyc_Absynpp_scope2doc((void*)_tmp3E0->sc);Cyc_PP_cat(
_tag_dynforward(_tmp405,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp407[12];_tmp407[11]=Cyc_PP_text(({const char*_tmp40B=";";_tag_dynforward(
_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,2));}));_tmp407[10]=Cyc_Absynpp_atts2doc(
_tmp3E0->attributes);_tmp407[9]=Cyc_Absynpp_rb();_tmp407[8]=Cyc_PP_line_doc();
_tmp407[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp40A[2];_tmp40A[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E0->impl))->fields);_tmp40A[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp40A,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp407[6]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E0->impl))->rgn_po == 
0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp408[2];_tmp408[1]=Cyc_Absynpp_rgnpo2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E0->impl))->rgn_po);_tmp408[0]=
Cyc_PP_text(({const char*_tmp409=":";_tag_dynforward(_tmp409,sizeof(char),
_get_zero_arr_size(_tmp409,2));}));Cyc_PP_cat(_tag_dynforward(_tmp408,sizeof(
struct Cyc_PP_Doc*),2));});_tmp407[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3E0->impl))->exist_vars);_tmp407[4]=Cyc_Absynpp_lb();_tmp407[3]=
Cyc_PP_blank_doc();_tmp407[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3E0->name);_tmp407[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3E0->kind);_tmp407[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3E0->sc);Cyc_PP_cat(_tag_dynforward(_tmp407,sizeof(struct Cyc_PP_Doc*),
12));});}goto _LL2A0;_LL2A5: if(*((int*)_tmp3DE)!= 0)goto _LL2A7;_tmp3E1=((struct
Cyc_Absyn_Var_d_struct*)_tmp3DE)->f1;_LL2A6: s=Cyc_Absynpp_vardecl2doc(_tmp3E1);
goto _LL2A0;_LL2A7: if(*((int*)_tmp3DE)!= 5)goto _LL2A9;_tmp3E2=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp3DE)->f1;_tmp3E3=*_tmp3E2;_tmp3E4=(void*)_tmp3E3.sc;_tmp3E5=_tmp3E3.name;
_tmp3E6=_tmp3E3.tvs;_tmp3E7=_tmp3E3.fields;_tmp3E8=_tmp3E3.is_xtunion;_tmp3E9=
_tmp3E3.is_flat;_LL2A8: if(_tmp3E7 == 0)s=({struct Cyc_PP_Doc*_tmp40C[6];_tmp40C[5]=
Cyc_PP_text(({const char*_tmp410=";";_tag_dynforward(_tmp410,sizeof(char),
_get_zero_arr_size(_tmp410,2));}));_tmp40C[4]=Cyc_Absynpp_ktvars2doc(_tmp3E6);
_tmp40C[3]=_tmp3E8?Cyc_Absynpp_qvar2bolddoc(_tmp3E5): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E5);_tmp40C[2]=_tmp3E9?Cyc_PP_text(({const char*_tmp40F="__attribute__((flat)) ";
_tag_dynforward(_tmp40F,sizeof(char),_get_zero_arr_size(_tmp40F,23));})): Cyc_PP_blank_doc();
_tmp40C[1]=_tmp3E8?Cyc_PP_text(({const char*_tmp40D="xtunion ";_tag_dynforward(
_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,9));})): Cyc_PP_text(({const char*
_tmp40E="tunion ";_tag_dynforward(_tmp40E,sizeof(char),_get_zero_arr_size(
_tmp40E,8));}));_tmp40C[0]=Cyc_Absynpp_scope2doc(_tmp3E4);Cyc_PP_cat(
_tag_dynforward(_tmp40C,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp411[11];_tmp411[10]=Cyc_PP_text(({const char*_tmp416=";";_tag_dynforward(
_tmp416,sizeof(char),_get_zero_arr_size(_tmp416,2));}));_tmp411[9]=Cyc_Absynpp_rb();
_tmp411[8]=Cyc_PP_line_doc();_tmp411[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp415[
2];_tmp415[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)_tmp3E7->v);
_tmp415[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp415,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp411[6]=Cyc_Absynpp_lb();_tmp411[5]=Cyc_PP_blank_doc();_tmp411[4]=
_tmp3E9?Cyc_PP_text(({const char*_tmp414="__attribute__((flat)) ";_tag_dynforward(
_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,23));})): Cyc_PP_blank_doc();
_tmp411[3]=Cyc_Absynpp_ktvars2doc(_tmp3E6);_tmp411[2]=_tmp3E8?Cyc_Absynpp_qvar2bolddoc(
_tmp3E5): Cyc_Absynpp_typedef_name2bolddoc(_tmp3E5);_tmp411[1]=_tmp3E8?Cyc_PP_text(({
const char*_tmp412="xtunion ";_tag_dynforward(_tmp412,sizeof(char),
_get_zero_arr_size(_tmp412,9));})): Cyc_PP_text(({const char*_tmp413="tunion ";
_tag_dynforward(_tmp413,sizeof(char),_get_zero_arr_size(_tmp413,8));}));_tmp411[
0]=Cyc_Absynpp_scope2doc(_tmp3E4);Cyc_PP_cat(_tag_dynforward(_tmp411,sizeof(
struct Cyc_PP_Doc*),11));});}goto _LL2A0;_LL2A9: if(*((int*)_tmp3DE)!= 2)goto _LL2AB;
_tmp3EA=((struct Cyc_Absyn_Let_d_struct*)_tmp3DE)->f1;_tmp3EB=((struct Cyc_Absyn_Let_d_struct*)
_tmp3DE)->f3;_LL2AA: s=({struct Cyc_PP_Doc*_tmp417[5];_tmp417[4]=Cyc_PP_text(({
const char*_tmp41A=";";_tag_dynforward(_tmp41A,sizeof(char),_get_zero_arr_size(
_tmp41A,2));}));_tmp417[3]=Cyc_Absynpp_exp2doc(_tmp3EB);_tmp417[2]=Cyc_PP_text(({
const char*_tmp419=" = ";_tag_dynforward(_tmp419,sizeof(char),_get_zero_arr_size(
_tmp419,4));}));_tmp417[1]=Cyc_Absynpp_pat2doc(_tmp3EA);_tmp417[0]=Cyc_PP_text(({
const char*_tmp418="let ";_tag_dynforward(_tmp418,sizeof(char),_get_zero_arr_size(
_tmp418,5));}));Cyc_PP_cat(_tag_dynforward(_tmp417,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL2A0;_LL2AB: if(*((int*)_tmp3DE)!= 3)goto _LL2AD;_tmp3EC=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3DE)->f1;_LL2AC: s=({struct Cyc_PP_Doc*_tmp41B[3];_tmp41B[2]=Cyc_PP_text(({
const char*_tmp41D=";";_tag_dynforward(_tmp41D,sizeof(char),_get_zero_arr_size(
_tmp41D,2));}));_tmp41B[1]=Cyc_Absynpp_ids2doc(_tmp3EC);_tmp41B[0]=Cyc_PP_text(({
const char*_tmp41C="let ";_tag_dynforward(_tmp41C,sizeof(char),_get_zero_arr_size(
_tmp41C,5));}));Cyc_PP_cat(_tag_dynforward(_tmp41B,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2A0;_LL2AD: if(*((int*)_tmp3DE)!= 6)goto _LL2AF;_tmp3ED=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3DE)->f1;_tmp3EE=*_tmp3ED;_tmp3EF=(void*)_tmp3EE.sc;_tmp3F0=_tmp3EE.name;
_tmp3F1=_tmp3EE.fields;_LL2AE: if(_tmp3F1 == 0)s=({struct Cyc_PP_Doc*_tmp41E[4];
_tmp41E[3]=Cyc_PP_text(({const char*_tmp420=";";_tag_dynforward(_tmp420,sizeof(
char),_get_zero_arr_size(_tmp420,2));}));_tmp41E[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3F0);_tmp41E[1]=Cyc_PP_text(({const char*_tmp41F="enum ";_tag_dynforward(
_tmp41F,sizeof(char),_get_zero_arr_size(_tmp41F,6));}));_tmp41E[0]=Cyc_Absynpp_scope2doc(
_tmp3EF);Cyc_PP_cat(_tag_dynforward(_tmp41E,sizeof(struct Cyc_PP_Doc*),4));});
else{s=({struct Cyc_PP_Doc*_tmp421[9];_tmp421[8]=Cyc_PP_text(({const char*_tmp424=";";
_tag_dynforward(_tmp424,sizeof(char),_get_zero_arr_size(_tmp424,2));}));_tmp421[
7]=Cyc_Absynpp_rb();_tmp421[6]=Cyc_PP_line_doc();_tmp421[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp423[2];_tmp423[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp3F1->v);_tmp423[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp423,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp421[4]=Cyc_Absynpp_lb();_tmp421[3]=Cyc_PP_blank_doc();
_tmp421[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3F0);_tmp421[1]=Cyc_PP_text(({const char*
_tmp422="enum ";_tag_dynforward(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,
6));}));_tmp421[0]=Cyc_Absynpp_scope2doc(_tmp3EF);Cyc_PP_cat(_tag_dynforward(
_tmp421,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2A0;_LL2AF: if(*((int*)_tmp3DE)
!= 7)goto _LL2B1;_tmp3F2=((struct Cyc_Absyn_Typedef_d_struct*)_tmp3DE)->f1;_LL2B0: {
void*t;if(_tmp3F2->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp3F2->kind,0);}s=({struct Cyc_PP_Doc*_tmp425[4];
_tmp425[3]=Cyc_PP_text(({const char*_tmp429=";";_tag_dynforward(_tmp429,sizeof(
char),_get_zero_arr_size(_tmp429,2));}));_tmp425[2]=Cyc_Absynpp_atts2doc(_tmp3F2->atts);
_tmp425[1]=Cyc_Absynpp_tqtd2doc(_tmp3F2->tq,t,({struct Cyc_Core_Opt*_tmp427=
_cycalloc(sizeof(*_tmp427));_tmp427->v=({struct Cyc_PP_Doc*_tmp428[2];_tmp428[1]=
Cyc_Absynpp_tvars2doc(_tmp3F2->tvs);_tmp428[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3F2->name);Cyc_PP_cat(_tag_dynforward(_tmp428,sizeof(struct Cyc_PP_Doc*),2));});
_tmp427;}));_tmp425[0]=Cyc_PP_text(({const char*_tmp426="typedef ";
_tag_dynforward(_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp425,sizeof(struct Cyc_PP_Doc*),4));});goto _LL2A0;}_LL2B1: if(*((
int*)_tmp3DE)!= 8)goto _LL2B3;_tmp3F3=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3DE)->f1;_tmp3F4=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3DE)->f2;_LL2B2:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3F3);s=({
struct Cyc_PP_Doc*_tmp42A[8];_tmp42A[7]=Cyc_Absynpp_rb();_tmp42A[6]=Cyc_PP_line_doc();
_tmp42A[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp42C="";_tag_dynforward(_tmp42C,sizeof(char),_get_zero_arr_size(
_tmp42C,1));}),_tmp3F4);_tmp42A[4]=Cyc_PP_line_doc();_tmp42A[3]=Cyc_Absynpp_lb();
_tmp42A[2]=Cyc_PP_blank_doc();_tmp42A[1]=Cyc_PP_textptr(_tmp3F3);_tmp42A[0]=Cyc_PP_text(({
const char*_tmp42B="namespace ";_tag_dynforward(_tmp42B,sizeof(char),
_get_zero_arr_size(_tmp42B,11));}));Cyc_PP_cat(_tag_dynforward(_tmp42A,sizeof(
struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL2A0;_LL2B3: if(*((int*)_tmp3DE)!= 9)goto _LL2B5;_tmp3F5=((struct Cyc_Absyn_Using_d_struct*)
_tmp3DE)->f1;_tmp3F6=((struct Cyc_Absyn_Using_d_struct*)_tmp3DE)->f2;_LL2B4: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp42D[8];_tmp42D[7]=Cyc_Absynpp_rb();
_tmp42D[6]=Cyc_PP_line_doc();_tmp42D[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp42F="";_tag_dynforward(_tmp42F,sizeof(char),
_get_zero_arr_size(_tmp42F,1));}),_tmp3F6);_tmp42D[4]=Cyc_PP_line_doc();_tmp42D[
3]=Cyc_Absynpp_lb();_tmp42D[2]=Cyc_PP_blank_doc();_tmp42D[1]=Cyc_Absynpp_qvar2doc(
_tmp3F5);_tmp42D[0]=Cyc_PP_text(({const char*_tmp42E="using ";_tag_dynforward(
_tmp42E,sizeof(char),_get_zero_arr_size(_tmp42E,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp42D,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp430[11];_tmp430[10]=Cyc_PP_text(({const char*_tmp435=" */";_tag_dynforward(
_tmp435,sizeof(char),_get_zero_arr_size(_tmp435,4));}));_tmp430[9]=Cyc_Absynpp_rb();
_tmp430[8]=Cyc_PP_text(({const char*_tmp434="/* ";_tag_dynforward(_tmp434,sizeof(
char),_get_zero_arr_size(_tmp434,4));}));_tmp430[7]=Cyc_PP_line_doc();_tmp430[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp433="";_tag_dynforward(_tmp433,sizeof(char),_get_zero_arr_size(
_tmp433,1));}),_tmp3F6);_tmp430[5]=Cyc_PP_line_doc();_tmp430[4]=Cyc_PP_text(({
const char*_tmp432=" */";_tag_dynforward(_tmp432,sizeof(char),_get_zero_arr_size(
_tmp432,4));}));_tmp430[3]=Cyc_Absynpp_lb();_tmp430[2]=Cyc_PP_blank_doc();
_tmp430[1]=Cyc_Absynpp_qvar2doc(_tmp3F5);_tmp430[0]=Cyc_PP_text(({const char*
_tmp431="/* using ";_tag_dynforward(_tmp431,sizeof(char),_get_zero_arr_size(
_tmp431,10));}));Cyc_PP_cat(_tag_dynforward(_tmp430,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL2A0;_LL2B5: if(*((int*)_tmp3DE)!= 10)goto _LL2B7;_tmp3F7=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3DE)->f1;_LL2B6: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*
_tmp436[6];_tmp436[5]=Cyc_Absynpp_rb();_tmp436[4]=Cyc_PP_line_doc();_tmp436[3]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp438="";_tag_dynforward(_tmp438,sizeof(char),_get_zero_arr_size(
_tmp438,1));}),_tmp3F7);_tmp436[2]=Cyc_PP_line_doc();_tmp436[1]=Cyc_Absynpp_lb();
_tmp436[0]=Cyc_PP_text(({const char*_tmp437="extern \"C\" ";_tag_dynforward(
_tmp437,sizeof(char),_get_zero_arr_size(_tmp437,12));}));Cyc_PP_cat(
_tag_dynforward(_tmp436,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp439[9];_tmp439[8]=Cyc_PP_text(({const char*_tmp43E=" */";_tag_dynforward(
_tmp43E,sizeof(char),_get_zero_arr_size(_tmp43E,4));}));_tmp439[7]=Cyc_Absynpp_rb();
_tmp439[6]=Cyc_PP_text(({const char*_tmp43D="/* ";_tag_dynforward(_tmp43D,sizeof(
char),_get_zero_arr_size(_tmp43D,4));}));_tmp439[5]=Cyc_PP_line_doc();_tmp439[4]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp43C="";_tag_dynforward(_tmp43C,sizeof(char),_get_zero_arr_size(
_tmp43C,1));}),_tmp3F7);_tmp439[3]=Cyc_PP_line_doc();_tmp439[2]=Cyc_PP_text(({
const char*_tmp43B=" */";_tag_dynforward(_tmp43B,sizeof(char),_get_zero_arr_size(
_tmp43B,4));}));_tmp439[1]=Cyc_Absynpp_lb();_tmp439[0]=Cyc_PP_text(({const char*
_tmp43A="/* extern \"C\" ";_tag_dynforward(_tmp43A,sizeof(char),
_get_zero_arr_size(_tmp43A,15));}));Cyc_PP_cat(_tag_dynforward(_tmp439,sizeof(
struct Cyc_PP_Doc*),9));});}goto _LL2A0;_LL2B7: if(*((int*)_tmp3DE)!= 11)goto _LL2B9;
_tmp3F8=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3DE)->f1;_tmp3F9=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3DE)->f2;_LL2B8: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp3F9 != 0)exs_doc=({struct Cyc_PP_Doc*_tmp43F[7];
_tmp43F[6]=Cyc_Absynpp_rb();_tmp43F[5]=Cyc_PP_line_doc();_tmp43F[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct _tuple11*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({const char*_tmp441=",";_tag_dynforward(
_tmp441,sizeof(char),_get_zero_arr_size(_tmp441,2));}),_tmp3F9);_tmp43F[3]=Cyc_PP_line_doc();
_tmp43F[2]=Cyc_Absynpp_lb();_tmp43F[1]=Cyc_PP_text(({const char*_tmp440=" export ";
_tag_dynforward(_tmp440,sizeof(char),_get_zero_arr_size(_tmp440,9));}));_tmp43F[
0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dynforward(_tmp43F,sizeof(struct Cyc_PP_Doc*),
7));});else{exs_doc=Cyc_Absynpp_rb();}s=({struct Cyc_PP_Doc*_tmp442[6];_tmp442[5]=
exs_doc;_tmp442[4]=Cyc_PP_line_doc();_tmp442[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp444="";_tag_dynforward(_tmp444,sizeof(char),
_get_zero_arr_size(_tmp444,1));}),_tmp3F8);_tmp442[2]=Cyc_PP_line_doc();_tmp442[
1]=Cyc_Absynpp_lb();_tmp442[0]=Cyc_PP_text(({const char*_tmp443="extern \"C include\" ";
_tag_dynforward(_tmp443,sizeof(char),_get_zero_arr_size(_tmp443,20));}));Cyc_PP_cat(
_tag_dynforward(_tmp442,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({struct Cyc_PP_Doc*
_tmp445[9];_tmp445[8]=Cyc_PP_text(({const char*_tmp44A=" */";_tag_dynforward(
_tmp44A,sizeof(char),_get_zero_arr_size(_tmp44A,4));}));_tmp445[7]=Cyc_Absynpp_rb();
_tmp445[6]=Cyc_PP_text(({const char*_tmp449="/* ";_tag_dynforward(_tmp449,sizeof(
char),_get_zero_arr_size(_tmp449,4));}));_tmp445[5]=Cyc_PP_line_doc();_tmp445[4]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp448="";_tag_dynforward(_tmp448,sizeof(char),_get_zero_arr_size(
_tmp448,1));}),_tmp3F8);_tmp445[3]=Cyc_PP_line_doc();_tmp445[2]=Cyc_PP_text(({
const char*_tmp447=" */";_tag_dynforward(_tmp447,sizeof(char),_get_zero_arr_size(
_tmp447,4));}));_tmp445[1]=Cyc_Absynpp_lb();_tmp445[0]=Cyc_PP_text(({const char*
_tmp446="/* extern \"C include\" ";_tag_dynforward(_tmp446,sizeof(char),
_get_zero_arr_size(_tmp446,23));}));Cyc_PP_cat(_tag_dynforward(_tmp445,sizeof(
struct Cyc_PP_Doc*),9));});}goto _LL2A0;_LL2B9: if((int)_tmp3DE != 0)goto _LL2BB;
_LL2BA: s=({struct Cyc_PP_Doc*_tmp44B[2];_tmp44B[1]=Cyc_Absynpp_lb();_tmp44B[0]=
Cyc_PP_text(({const char*_tmp44C="__cyclone_port_on__;";_tag_dynforward(_tmp44C,
sizeof(char),_get_zero_arr_size(_tmp44C,21));}));Cyc_PP_cat(_tag_dynforward(
_tmp44B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2A0;_LL2BB: if((int)_tmp3DE != 1)
goto _LL2A0;_LL2BC: s=({struct Cyc_PP_Doc*_tmp44D[2];_tmp44D[1]=Cyc_Absynpp_lb();
_tmp44D[0]=Cyc_PP_text(({const char*_tmp44E="__cyclone_port_off__;";
_tag_dynforward(_tmp44E,sizeof(char),_get_zero_arr_size(_tmp44E,22));}));Cyc_PP_cat(
_tag_dynforward(_tmp44D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2A0;_LL2A0:;}
return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc){if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp44F=sc;
_LL2BE: if((int)_tmp44F != 0)goto _LL2C0;_LL2BF: return Cyc_PP_text(({const char*
_tmp450="static ";_tag_dynforward(_tmp450,sizeof(char),_get_zero_arr_size(
_tmp450,8));}));_LL2C0: if((int)_tmp44F != 2)goto _LL2C2;_LL2C1: return Cyc_PP_nil_doc();
_LL2C2: if((int)_tmp44F != 3)goto _LL2C4;_LL2C3: return Cyc_PP_text(({const char*
_tmp451="extern ";_tag_dynforward(_tmp451,sizeof(char),_get_zero_arr_size(
_tmp451,8));}));_LL2C4: if((int)_tmp44F != 4)goto _LL2C6;_LL2C5: return Cyc_PP_text(({
const char*_tmp452="extern \"C\" ";_tag_dynforward(_tmp452,sizeof(char),
_get_zero_arr_size(_tmp452,12));}));_LL2C6: if((int)_tmp44F != 1)goto _LL2C8;_LL2C7:
return Cyc_PP_text(({const char*_tmp453="abstract ";_tag_dynforward(_tmp453,
sizeof(char),_get_zero_arr_size(_tmp453,10));}));_LL2C8: if((int)_tmp44F != 5)goto
_LL2BD;_LL2C9: return Cyc_PP_text(({const char*_tmp454="register ";_tag_dynforward(
_tmp454,sizeof(char),_get_zero_arr_size(_tmp454,10));}));_LL2BD:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp455=t;struct Cyc_Absyn_Tvar*_tmp456;struct Cyc_List_List*_tmp457;
_LL2CB: if(_tmp455 <= (void*)4)goto _LL2CF;if(*((int*)_tmp455)!= 1)goto _LL2CD;
_tmp456=((struct Cyc_Absyn_VarType_struct*)_tmp455)->f1;_LL2CC: return Cyc_Tcutil_is_temp_tvar(
_tmp456);_LL2CD: if(*((int*)_tmp455)!= 21)goto _LL2CF;_tmp457=((struct Cyc_Absyn_JoinEff_struct*)
_tmp455)->f1;_LL2CE: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp457);_LL2CF:;_LL2D0: return 0;_LL2CA:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp458=t;void**_tmp459;void*_tmp45A;_LL2D2: if(_tmp458 <= (void*)4)goto
_LL2D8;if(*((int*)_tmp458)!= 11)goto _LL2D4;_LL2D3: return 1;_LL2D4: if(*((int*)
_tmp458)!= 13)goto _LL2D6;_LL2D5: return 1;_LL2D6: if(*((int*)_tmp458)!= 17)goto
_LL2D8;_tmp459=((struct Cyc_Absyn_TypedefType_struct*)_tmp458)->f4;if(_tmp459 == 0)
goto _LL2D8;_tmp45A=*_tmp459;_LL2D7: return Cyc_Absynpp_is_anon_aggrtype(_tmp45A);
_LL2D8:;_LL2D9: return 0;_LL2D1:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple5 _tmp45C=({struct _tuple5 _tmp45B;_tmp45B.f1=(void*)tms->hd;_tmp45B.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp45B;});void*_tmp45D;
void*_tmp45E;_LL2DB: _tmp45D=_tmp45C.f1;if(*((int*)_tmp45D)!= 2)goto _LL2DD;
_tmp45E=_tmp45C.f2;if(*((int*)_tmp45E)!= 3)goto _LL2DD;_LL2DC: return({struct Cyc_List_List*
_tmp45F=_region_malloc(r,sizeof(*_tmp45F));_tmp45F->hd=(void*)((void*)tms->hd);
_tmp45F->tl=({struct Cyc_List_List*_tmp460=_region_malloc(r,sizeof(*_tmp460));
_tmp460->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);
_tmp460->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)
_check_null(tms->tl))->tl);_tmp460;});_tmp45F;});_LL2DD:;_LL2DE: return({struct
Cyc_List_List*_tmp461=_region_malloc(r,sizeof(*_tmp461));_tmp461->hd=(void*)atts;
_tmp461->tl=tms;_tmp461;});_LL2DA:;}else{return({struct Cyc_List_List*_tmp462=
_region_malloc(r,sizeof(*_tmp462));_tmp462->hd=(void*)atts;_tmp462->tl=tms;
_tmp462;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp463=t;struct Cyc_Absyn_ArrayInfo _tmp464;void*_tmp465;struct
Cyc_Absyn_Tqual _tmp466;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_Absyn_Conref*
_tmp468;struct Cyc_Position_Segment*_tmp469;struct Cyc_Absyn_PtrInfo _tmp46A;void*
_tmp46B;struct Cyc_Absyn_Tqual _tmp46C;struct Cyc_Absyn_PtrAtts _tmp46D;struct Cyc_Absyn_FnInfo
_tmp46E;struct Cyc_List_List*_tmp46F;struct Cyc_Core_Opt*_tmp470;void*_tmp471;
struct Cyc_List_List*_tmp472;int _tmp473;struct Cyc_Absyn_VarargInfo*_tmp474;struct
Cyc_List_List*_tmp475;struct Cyc_List_List*_tmp476;struct Cyc_Core_Opt*_tmp477;
struct Cyc_Core_Opt*_tmp478;int _tmp479;struct _tuple0*_tmp47A;struct Cyc_List_List*
_tmp47B;void**_tmp47C;_LL2E0: if(_tmp463 <= (void*)4)goto _LL2EA;if(*((int*)_tmp463)
!= 7)goto _LL2E2;_tmp464=((struct Cyc_Absyn_ArrayType_struct*)_tmp463)->f1;_tmp465=(
void*)_tmp464.elt_type;_tmp466=_tmp464.tq;_tmp467=_tmp464.num_elts;_tmp468=
_tmp464.zero_term;_tmp469=_tmp464.zt_loc;_LL2E1: {struct Cyc_Absyn_Tqual _tmp47E;
void*_tmp47F;struct Cyc_List_List*_tmp480;struct _tuple7 _tmp47D=Cyc_Absynpp_to_tms(
r,_tmp466,_tmp465);_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;_tmp480=_tmp47D.f3;{
void*tm;if(_tmp467 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp481=
_region_malloc(r,sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp482;_tmp482.tag=0;_tmp482.f1=_tmp468;_tmp482.f2=_tmp469;_tmp482;});_tmp481;});
else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp483=_region_malloc(r,
sizeof(*_tmp483));_tmp483[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp484;
_tmp484.tag=1;_tmp484.f1=(struct Cyc_Absyn_Exp*)_tmp467;_tmp484.f2=_tmp468;
_tmp484.f3=_tmp469;_tmp484;});_tmp483;});}return({struct _tuple7 _tmp485;_tmp485.f1=
_tmp47E;_tmp485.f2=_tmp47F;_tmp485.f3=({struct Cyc_List_List*_tmp486=
_region_malloc(r,sizeof(*_tmp486));_tmp486->hd=(void*)tm;_tmp486->tl=_tmp480;
_tmp486;});_tmp485;});}}_LL2E2: if(*((int*)_tmp463)!= 4)goto _LL2E4;_tmp46A=((
struct Cyc_Absyn_PointerType_struct*)_tmp463)->f1;_tmp46B=(void*)_tmp46A.elt_typ;
_tmp46C=_tmp46A.elt_tq;_tmp46D=_tmp46A.ptr_atts;_LL2E3: {struct Cyc_Absyn_Tqual
_tmp488;void*_tmp489;struct Cyc_List_List*_tmp48A;struct _tuple7 _tmp487=Cyc_Absynpp_to_tms(
r,_tmp46C,_tmp46B);_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;_tmp48A=_tmp487.f3;
_tmp48A=({struct Cyc_List_List*_tmp48B=_region_malloc(r,sizeof(*_tmp48B));_tmp48B->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp48C=_region_malloc(r,
sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp48D;_tmp48D.tag=
2;_tmp48D.f1=_tmp46D;_tmp48D.f2=tq;_tmp48D;});_tmp48C;}));_tmp48B->tl=_tmp48A;
_tmp48B;});return({struct _tuple7 _tmp48E;_tmp48E.f1=_tmp488;_tmp48E.f2=_tmp489;
_tmp48E.f3=_tmp48A;_tmp48E;});}_LL2E4: if(*((int*)_tmp463)!= 8)goto _LL2E6;_tmp46E=((
struct Cyc_Absyn_FnType_struct*)_tmp463)->f1;_tmp46F=_tmp46E.tvars;_tmp470=
_tmp46E.effect;_tmp471=(void*)_tmp46E.ret_typ;_tmp472=_tmp46E.args;_tmp473=
_tmp46E.c_varargs;_tmp474=_tmp46E.cyc_varargs;_tmp475=_tmp46E.rgn_po;_tmp476=
_tmp46E.attributes;_LL2E5: if(!Cyc_Absynpp_print_all_tvars){if(_tmp470 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp470->v)){_tmp470=0;_tmp46F=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp46F);}{struct Cyc_Absyn_Tqual _tmp490;void*
_tmp491;struct Cyc_List_List*_tmp492;struct _tuple7 _tmp48F=Cyc_Absynpp_to_tms(r,
Cyc_Absyn_empty_tqual(0),_tmp471);_tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;_tmp492=
_tmp48F.f3;{struct Cyc_List_List*tms=_tmp492;if(_tmp476 != 0  && !Cyc_Absynpp_to_VC)
tms=Cyc_Absynpp_bubble_attributes(r,(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp493=_region_malloc(r,sizeof(*_tmp493));_tmp493[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp494;_tmp494.tag=5;_tmp494.f1=0;_tmp494.f2=_tmp476;_tmp494;});_tmp493;}),tms);
tms=({struct Cyc_List_List*_tmp495=_region_malloc(r,sizeof(*_tmp495));_tmp495->hd=(
void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp496=_region_malloc(r,
sizeof(*_tmp496));_tmp496[0]=({struct Cyc_Absyn_Function_mod_struct _tmp497;
_tmp497.tag=3;_tmp497.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp498=_region_malloc(r,sizeof(*_tmp498));_tmp498[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp499;_tmp499.tag=1;_tmp499.f1=_tmp472;_tmp499.f2=_tmp473;_tmp499.f3=_tmp474;
_tmp499.f4=_tmp470;_tmp499.f5=_tmp475;_tmp499;});_tmp498;}));_tmp497;});_tmp496;}));
_tmp495->tl=tms;_tmp495;});if(Cyc_Absynpp_to_VC)for(0;_tmp476 != 0;_tmp476=
_tmp476->tl){void*_tmp49A=(void*)_tmp476->hd;_LL2ED: if((int)_tmp49A != 0)goto
_LL2EF;_LL2EE: goto _LL2F0;_LL2EF: if((int)_tmp49A != 1)goto _LL2F1;_LL2F0: goto _LL2F2;
_LL2F1: if((int)_tmp49A != 2)goto _LL2F3;_LL2F2: tms=({struct Cyc_List_List*_tmp49B=
_region_malloc(r,sizeof(*_tmp49B));_tmp49B->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp49C=_region_malloc(r,sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp49D;_tmp49D.tag=5;_tmp49D.f1=0;_tmp49D.f2=({struct Cyc_List_List*_tmp49E=
_cycalloc(sizeof(*_tmp49E));_tmp49E->hd=(void*)((void*)_tmp476->hd);_tmp49E->tl=
0;_tmp49E;});_tmp49D;});_tmp49C;}));_tmp49B->tl=tms;_tmp49B;});goto AfterAtts;
_LL2F3:;_LL2F4: goto _LL2EC;_LL2EC:;}AfterAtts: if(_tmp46F != 0)tms=({struct Cyc_List_List*
_tmp49F=_region_malloc(r,sizeof(*_tmp49F));_tmp49F->hd=(void*)((void*)({struct
Cyc_Absyn_TypeParams_mod_struct*_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0));
_tmp4A0[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4A1;_tmp4A1.tag=4;_tmp4A1.f1=
_tmp46F;_tmp4A1.f2=0;_tmp4A1.f3=1;_tmp4A1;});_tmp4A0;}));_tmp49F->tl=tms;_tmp49F;});
return({struct _tuple7 _tmp4A2;_tmp4A2.f1=_tmp490;_tmp4A2.f2=_tmp491;_tmp4A2.f3=
tms;_tmp4A2;});}}_LL2E6: if(*((int*)_tmp463)!= 0)goto _LL2E8;_tmp477=((struct Cyc_Absyn_Evar_struct*)
_tmp463)->f1;_tmp478=((struct Cyc_Absyn_Evar_struct*)_tmp463)->f2;_tmp479=((
struct Cyc_Absyn_Evar_struct*)_tmp463)->f3;_LL2E7: if(_tmp478 == 0)return({struct
_tuple7 _tmp4A3;_tmp4A3.f1=tq;_tmp4A3.f2=t;_tmp4A3.f3=0;_tmp4A3;});else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp478->v);}_LL2E8: if(*((int*)_tmp463)!= 17)goto _LL2EA;_tmp47A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp463)->f1;_tmp47B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp463)->f2;_tmp47C=((struct Cyc_Absyn_TypedefType_struct*)_tmp463)->f4;_LL2E9:
if((_tmp47C == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp47C))return({struct _tuple7 _tmp4A4;_tmp4A4.f1=tq;_tmp4A4.f2=t;_tmp4A4.f3=0;
_tmp4A4;});else{return Cyc_Absynpp_to_tms(r,tq,*_tmp47C);}_LL2EA:;_LL2EB: return({
struct _tuple7 _tmp4A5;_tmp4A5.f1=tq;_tmp4A5.f2=t;_tmp4A5.f3=0;_tmp4A5;});_LL2DF:;}
static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4A6=t;struct Cyc_Core_Opt*
_tmp4A7;struct Cyc_Core_Opt _tmp4A8;void*_tmp4A9;struct Cyc_Absyn_PtrInfo _tmp4AA;
void*_tmp4AB;_LL2F6: if(_tmp4A6 <= (void*)4)goto _LL2FA;if(*((int*)_tmp4A6)!= 0)
goto _LL2F8;_tmp4A7=((struct Cyc_Absyn_Evar_struct*)_tmp4A6)->f2;if(_tmp4A7 == 0)
goto _LL2F8;_tmp4A8=*_tmp4A7;_tmp4A9=(void*)_tmp4A8.v;_LL2F7: return Cyc_Absynpp_is_char_ptr(
_tmp4A9);_LL2F8: if(*((int*)_tmp4A6)!= 4)goto _LL2FA;_tmp4AA=((struct Cyc_Absyn_PointerType_struct*)
_tmp4A6)->f1;_tmp4AB=(void*)_tmp4AA.elt_typ;_LL2F9: L: {void*_tmp4AC=_tmp4AB;
struct Cyc_Core_Opt*_tmp4AD;struct Cyc_Core_Opt _tmp4AE;void*_tmp4AF;void**_tmp4B0;
void*_tmp4B1;void*_tmp4B2;_LL2FD: if(_tmp4AC <= (void*)4)goto _LL303;if(*((int*)
_tmp4AC)!= 0)goto _LL2FF;_tmp4AD=((struct Cyc_Absyn_Evar_struct*)_tmp4AC)->f2;if(
_tmp4AD == 0)goto _LL2FF;_tmp4AE=*_tmp4AD;_tmp4AF=(void*)_tmp4AE.v;_LL2FE: _tmp4AB=
_tmp4AF;goto L;_LL2FF: if(*((int*)_tmp4AC)!= 17)goto _LL301;_tmp4B0=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4AC)->f4;if(_tmp4B0 == 0)goto _LL301;_tmp4B1=*_tmp4B0;_LL300: _tmp4AB=_tmp4B1;
goto L;_LL301: if(*((int*)_tmp4AC)!= 5)goto _LL303;_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AC)->f2;if((int)_tmp4B2 != 0)goto _LL303;_LL302: return 1;_LL303:;_LL304: return
0;_LL2FC:;}_LL2FA:;_LL2FB: return 0;_LL2F5:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4B3=_new_region("temp");struct _RegionHandle*temp=& _tmp4B3;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4B5;void*_tmp4B6;struct Cyc_List_List*_tmp4B7;struct
_tuple7 _tmp4B4=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4B5=_tmp4B4.f1;_tmp4B6=
_tmp4B4.f2;_tmp4B7=_tmp4B4.f3;_tmp4B7=Cyc_List_imp_rev(_tmp4B7);if(_tmp4B7 == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp4B9=({struct Cyc_PP_Doc*_tmp4B8[2];_tmp4B8[1]=
Cyc_Absynpp_ntyp2doc(_tmp4B6);_tmp4B8[0]=Cyc_Absynpp_tqual2doc(_tmp4B5);Cyc_PP_cat(
_tag_dynforward(_tmp4B8,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return
_tmp4B9;}else{struct Cyc_PP_Doc*_tmp4BC=({struct Cyc_PP_Doc*_tmp4BA[4];_tmp4BA[3]=
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp4B7);_tmp4BA[2]=Cyc_PP_text(({const char*_tmp4BB=" ";
_tag_dynforward(_tmp4BB,sizeof(char),_get_zero_arr_size(_tmp4BB,2));}));_tmp4BA[
1]=Cyc_Absynpp_ntyp2doc(_tmp4B6);_tmp4BA[0]=Cyc_Absynpp_tqual2doc(_tmp4B5);Cyc_PP_cat(
_tag_dynforward(_tmp4BA,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return
_tmp4BC;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp4BD[5];
_tmp4BD[4]=Cyc_PP_text(({const char*_tmp4C0=";";_tag_dynforward(_tmp4C0,sizeof(
char),_get_zero_arr_size(_tmp4C0,2));}));_tmp4BD[3]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4BD[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));
_tmp4BD[1]=Cyc_PP_text(({const char*_tmp4BF=":";_tag_dynforward(_tmp4BF,sizeof(
char),_get_zero_arr_size(_tmp4BF,2));}));_tmp4BD[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->v=
Cyc_PP_textptr(f->name);_tmp4BE;}));Cyc_PP_cat(_tag_dynforward(_tmp4BD,sizeof(
struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4C1[3];_tmp4C1[2]=
Cyc_PP_text(({const char*_tmp4C3=";";_tag_dynforward(_tmp4C3,sizeof(char),
_get_zero_arr_size(_tmp4C3,2));}));_tmp4C1[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4C1[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->v=Cyc_PP_textptr(f->name);_tmp4C2;}));
Cyc_PP_cat(_tag_dynforward(_tmp4C1,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*
_tmp4C4="";_tag_dynforward(_tmp4C4,sizeof(char),_get_zero_arr_size(_tmp4C4,1));}),
fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp4C5[3];_tmp4C5[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp4C5[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp4C5[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_dynforward(
_tmp4C5,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({const char*_tmp4C6=",";
_tag_dynforward(_tmp4C6,sizeof(char),_get_zero_arr_size(_tmp4C6,2));}),fields);}
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4C7[0]={};Cyc_fprintf(f,({const char*_tmp4C8="\n";
_tag_dynforward(_tmp4C8,sizeof(char),_get_zero_arr_size(_tmp4C8,2));}),
_tag_dynforward(_tmp4C7,sizeof(void*),0));});}}struct _dynforward_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*
_tmp4C9="";_tag_dynforward(_tmp4C9,sizeof(char),_get_zero_arr_size(_tmp4C9,1));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s),72);}struct _dynforward_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dynforward_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dynforward_ptr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
