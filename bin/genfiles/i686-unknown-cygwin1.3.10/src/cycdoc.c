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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};struct _dynforward_ptr Cyc_Core_new_string(
unsigned int);struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern char
Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);int Cyc_fputc(
int,struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct
Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;
unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);int puts(const char*);
int remove(const char*);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};void Cyc_file_close(
struct Cyc___cycFILE*);extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dynforward_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dynforward_ptr);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,
int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;
struct _dynforward_ptr line;int line_no;int col;};void Cyc_Lineno_poss_of_abss(struct
_dynforward_ptr filename,struct Cyc_List_List*places);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];typedef struct{int quot;
int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*abort();void
exit(int);int system(const char*);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct _dynforward_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*
tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;
int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dynforward_ptr);
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dynforward_ptr),
struct _dynforward_ptr errmsg,struct _dynforward_ptr args);struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{
struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct
_dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int
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
_tuple1*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct
_tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple1*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;
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
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
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
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{int tag;struct _tuple4 f1;};
struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{int tag;
struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_scopes;struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);void Cyc_Lex_lex_init(int use_cyclone_keywords);struct Cyc_Position_Segment{
int start;int end;};struct Cyc_MatchDecl_struct{int tag;struct _dynforward_ptr f1;};
struct Cyc_Standalone_struct{int tag;struct _dynforward_ptr f1;};struct _tuple11{int
f1;void*f2;};struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);const int
Cyc_lex_base[15]=(const int[15]){0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};const int Cyc_lex_backtrk[
15]=(const int[15]){- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
15]=(const int[15]){1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};const int Cyc_lex_trans[266]=(
const int[266]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,
0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266]=(const int[266]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,
- 1,6,8,- 1,9,10,12,13};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf){int state;int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(
lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(-
state)- 1;}while(1){base=Cyc_lex_base[_check_known_subscript_notnull(15,state)];
if(base < 0)return(- base)- 1;backtrk=Cyc_lex_backtrk[
_check_known_subscript_notnull(15,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dynforward_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(266,base + c)]== 
state)state=Cyc_lex_trans[_check_known_subscript_notnull(266,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(15,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1)(int)_throw((
void*)({struct Cyc_Lexing_Error_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Lexing_Error_struct _tmp1;_tmp1.tag=Cyc_Lexing_Error;_tmp1.f1=({const
char*_tmp2="empty token";_tag_dynforward(_tmp2,sizeof(char),_get_zero_arr_size(
_tmp2,12));});_tmp1;});_tmp0;}));else{return lbuf->lex_last_action;}}else{if(c == 
256)lbuf->lex_eof_reached=0;}}}struct _tuple11*Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: {int _tmp3=lexbuf->lex_start_pos + 5;int _tmp4=(lexbuf->lex_curr_pos - 
lexbuf->lex_start_pos)- 7;return({struct _tuple11*_tmp5=_cycalloc(sizeof(*_tmp5));
_tmp5->f1=Cyc_Lexing_lexeme_start(lexbuf);_tmp5->f2=(void*)({struct Cyc_Standalone_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Standalone_struct _tmp7;
_tmp7.tag=1;_tmp7.f1=Cyc_substring((struct _dynforward_ptr)lexbuf->lex_buffer,
_tmp3,(unsigned int)_tmp4);_tmp7;});_tmp6;});_tmp5;});}case 1: _LL1: {int _tmp8=
lexbuf->lex_start_pos + 4;int _tmp9=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 
6;return({struct _tuple11*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=Cyc_Lexing_lexeme_start(
lexbuf);_tmpA->f2=(void*)({struct Cyc_MatchDecl_struct*_tmpB=_cycalloc(sizeof(*
_tmpB));_tmpB[0]=({struct Cyc_MatchDecl_struct _tmpC;_tmpC.tag=0;_tmpC.f1=Cyc_substring((
struct _dynforward_ptr)lexbuf->lex_buffer,_tmp8,(unsigned int)_tmp9);_tmpC;});
_tmpB;});_tmpA;});}case 2: _LL2: return 0;case 3: _LL3: return Cyc_token(lexbuf);
default: _LL4:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,lexstate);}(
int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmpD=_cycalloc(sizeof(*_tmpD));
_tmpD[0]=({struct Cyc_Lexing_Error_struct _tmpE;_tmpE.tag=Cyc_Lexing_Error;_tmpE.f1=({
const char*_tmpF="some action didn't return!";_tag_dynforward(_tmpF,sizeof(char),
_get_zero_arr_size(_tmpF,27));});_tmpE;});_tmpD;}));}struct _tuple11*Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,0);}struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};static void Cyc_dump_begin(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
static void Cyc_dump_middle(){puts((const char*)"\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
static void Cyc_dump_end(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _dynforward_ptr s){int
depth=0;int len=(int)Cyc_strlen((struct _dynforward_ptr)s);int i=0;for(0;i < len;i ++){
char c=*((const char*)_check_dynforward_subscript(s,sizeof(char),i));if(c != '['){
Cyc_fputc((int)c,outf);continue;}Cyc_fputs((const char*)"\\texttt{",outf);i ++;
depth ++;for(0;i < len;i ++){char c=*((const char*)_check_dynforward_subscript(s,
sizeof(char),i));if(c == ']'){depth --;if(depth == 0){Cyc_fputc((int)'}',outf);
break;}}else{if(c == '[')depth ++;}Cyc_fputc((int)c,outf);}}}static int Cyc_width=50;
static void Cyc_set_width(int w){Cyc_width=w;}static struct Cyc_List_List*Cyc_cycdoc_files=
0;static void Cyc_add_other(struct _dynforward_ptr s){Cyc_cycdoc_files=({struct Cyc_List_List*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->hd=({struct _dynforward_ptr*_tmp11=
_cycalloc(sizeof(*_tmp11));_tmp11[0]=s;_tmp11;});_tmp10->tl=Cyc_cycdoc_files;
_tmp10;});}static struct Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(
struct _dynforward_ptr s){Cyc_cycargs=({struct Cyc_List_List*_tmp12=_cycalloc(
sizeof(*_tmp12));_tmp12->hd=({struct _dynforward_ptr*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13[0]=s;_tmp13;});_tmp12->tl=Cyc_cycargs;_tmp12;});}static char
_tmp14[8]="cyclone";static struct _dynforward_ptr Cyc_cyclone_file={_tmp14,_tmp14 + 
8};static void Cyc_set_cyclone_file(struct _dynforward_ptr s){Cyc_cyclone_file=s;}
static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _dynforward_ptr comment){Cyc_dump_begin();({
struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_PP_string_of_doc(Cyc_Absynpp_decl2doc(d),50));{void*
_tmp15[1]={& _tmp17};Cyc_printf(({const char*_tmp16="%s";_tag_dynforward(_tmp16,
sizeof(char),_get_zero_arr_size(_tmp16,3));}),_tag_dynforward(_tmp15,sizeof(void*),
1));}});Cyc_dump_middle();Cyc_pr_comment(Cyc_stdout,comment);({void*_tmp18[0]={};
Cyc_printf(({const char*_tmp19="\n";_tag_dynforward(_tmp19,sizeof(char),
_get_zero_arr_size(_tmp19,2));}),_tag_dynforward(_tmp18,sizeof(void*),0));});Cyc_dump_end();}
static int Cyc_is_other_special(char c){switch(c){case '\\': _LL6: goto _LL7;case '"':
_LL7: goto _LL8;case ';': _LL8: goto _LL9;case '&': _LL9: goto _LLA;case '(': _LLA: goto _LLB;
case ')': _LLB: goto _LLC;case '|': _LLC: goto _LLD;case '^': _LLD: goto _LLE;case '<': _LLE:
goto _LLF;case '>': _LLF: goto _LL10;case ' ': _LL10: goto _LL11;case '\n': _LL11: goto _LL12;
case '\t': _LL12: return 1;default: _LL13: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s){unsigned int _tmp1A=Cyc_strlen((struct _dynforward_ptr)s);
int _tmp1B=0;int _tmp1C=0;{int i=0;for(0;i < _tmp1A;i ++){char _tmp1D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmp1D == '\'')_tmp1B ++;else{
if(Cyc_is_other_special(_tmp1D))_tmp1C ++;}}}if(_tmp1B == 0  && _tmp1C == 0)return s;
if(_tmp1B == 0)return(struct _dynforward_ptr)Cyc_strconcat_l(({struct
_dynforward_ptr*_tmp1E[3];_tmp1E[2]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2);_tmp1E[1]=({struct _dynforward_ptr*_tmp20=
_cycalloc(sizeof(*_tmp20));_tmp20[0]=(struct _dynforward_ptr)s;_tmp20;});_tmp1E[0]=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp1E,sizeof(struct _dynforward_ptr*),3));}));{unsigned int _tmp22=(_tmp1A + 
_tmp1B)+ _tmp1C;struct _dynforward_ptr s2=({unsigned int _tmp2C=_tmp22 + 1;char*
_tmp2D=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp2C + 1));struct
_dynforward_ptr _tmp2F=_tag_dynforward(_tmp2D,sizeof(char),_tmp2C + 1);{
unsigned int _tmp2E=_tmp2C;unsigned int i;for(i=0;i < _tmp2E;i ++){_tmp2D[i]='\000';}
_tmp2D[_tmp2E]=(char)0;}_tmp2F;});int _tmp23=0;int _tmp24=0;for(0;_tmp23 < _tmp1A;
_tmp23 ++){char _tmp25=*((const char*)_check_dynforward_subscript(s,sizeof(char),
_tmp23));if(_tmp25 == '\''  || Cyc_is_other_special(_tmp25))({struct
_dynforward_ptr _tmp26=_dynforward_ptr_plus(s2,sizeof(char),_tmp24 ++);char _tmp27=*((
char*)_check_dynforward_subscript(_tmp26,sizeof(char),0));char _tmp28='\\';if(
_get_dynforward_size(_tmp26,sizeof(char))== 1  && (_tmp27 == '\000'  && _tmp28 != '\000'))
_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});({struct _dynforward_ptr
_tmp29=_dynforward_ptr_plus(s2,sizeof(char),_tmp24 ++);char _tmp2A=*((char*)
_check_dynforward_subscript(_tmp29,sizeof(char),0));char _tmp2B=_tmp25;if(
_get_dynforward_size(_tmp29,sizeof(char))== 1  && (_tmp2A == '\000'  && _tmp2B != '\000'))
_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});}return(struct
_dynforward_ptr)s2;}}static struct _dynforward_ptr*Cyc_sh_escape_stringptr(struct
_dynforward_ptr*sp){return({struct _dynforward_ptr*_tmp30=_cycalloc(sizeof(*
_tmp30));_tmp30[0]=Cyc_sh_escape_string(*sp);_tmp30;});}static struct Cyc_Lineno_Pos*
Cyc_new_pos(){return({struct Cyc_Lineno_Pos*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31->logical_file=({const char*_tmp32="";_tag_dynforward(_tmp32,sizeof(char),
_get_zero_arr_size(_tmp32,1));});_tmp31->line=Cyc_Core_new_string(0);_tmp31->line_no=
0;_tmp31->col=0;_tmp31;});}struct _tuple12{int f1;struct Cyc_Lineno_Pos*f2;};static
struct _tuple12*Cyc_start2pos(int x){return({struct _tuple12*_tmp33=_cycalloc(
sizeof(*_tmp33));_tmp33->f1=x;_tmp33->f2=Cyc_new_pos();_tmp33;});}static int Cyc_decl2start(
struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)_check_null(d->loc))->start;}
struct _tuple13{struct Cyc_Lineno_Pos*f1;void*f2;};static struct Cyc_List_List*Cyc_this_file(
struct _dynforward_ptr file,struct Cyc_List_List*x){struct Cyc_List_List*_tmp34=0;
for(0;x != 0;x=x->tl){if(Cyc_strcmp((struct _dynforward_ptr)((*((struct _tuple13*)x->hd)).f1)->logical_file,(
struct _dynforward_ptr)file)== 0)_tmp34=({struct Cyc_List_List*_tmp35=_cycalloc(
sizeof(*_tmp35));_tmp35->hd=(struct _tuple13*)x->hd;_tmp35->tl=_tmp34;_tmp35;});}
_tmp34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp34);
return _tmp34;}static int Cyc_lineno(struct Cyc_Lineno_Pos*p){return p->line_no;}
static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*decls){struct Cyc_List_List*
_tmp36=0;while(decls != 0){void*_tmp37=(void*)((struct Cyc_Absyn_Decl*)decls->hd)->r;
struct Cyc_List_List*_tmp38;struct Cyc_List_List*_tmp39;struct Cyc_List_List*_tmp3A;
struct Cyc_List_List*_tmp3B;_LL16: if(_tmp37 <= (void*)2)goto _LL2E;if(*((int*)
_tmp37)!= 8)goto _LL18;_tmp38=((struct Cyc_Absyn_Namespace_d_struct*)_tmp37)->f2;
_LL17: _tmp39=_tmp38;goto _LL19;_LL18: if(*((int*)_tmp37)!= 9)goto _LL1A;_tmp39=((
struct Cyc_Absyn_Using_d_struct*)_tmp37)->f2;_LL19: _tmp3A=_tmp39;goto _LL1B;_LL1A:
if(*((int*)_tmp37)!= 10)goto _LL1C;_tmp3A=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp37)->f1;_LL1B: _tmp3B=_tmp3A;goto _LL1D;_LL1C: if(*((int*)_tmp37)!= 11)goto
_LL1E;_tmp3B=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp37)->f1;_LL1D: decls=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp3B,decls->tl);goto _LL15;_LL1E: if(*((int*)_tmp37)!= 4)goto _LL20;_LL1F: goto
_LL21;_LL20: if(*((int*)_tmp37)!= 0)goto _LL22;_LL21: goto _LL23;_LL22: if(*((int*)
_tmp37)!= 5)goto _LL24;_LL23: goto _LL25;_LL24: if(*((int*)_tmp37)!= 6)goto _LL26;
_LL25: goto _LL27;_LL26: if(*((int*)_tmp37)!= 7)goto _LL28;_LL27: goto _LL29;_LL28: if(*((
int*)_tmp37)!= 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(*((int*)_tmp37)!= 2)goto
_LL2C;_LL2B: goto _LL2D;_LL2C: if(*((int*)_tmp37)!= 3)goto _LL2E;_LL2D: goto _LL2F;
_LL2E: if((int)_tmp37 != 0)goto _LL30;_LL2F: goto _LL31;_LL30: if((int)_tmp37 != 1)goto
_LL15;_LL31: _tmp36=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));
_tmp3C->hd=(struct Cyc_Absyn_Decl*)decls->hd;_tmp3C->tl=_tmp36;_tmp3C;});decls=
decls->tl;goto _LL15;_LL15:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp36);}struct _tuple14{int f1;struct Cyc_Absyn_Decl*f2;};
struct _tuple15{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(
struct _dynforward_ptr filename){struct _dynforward_ptr _tmp3D=Cyc_Filename_chop_extension(
filename);const char*_tmp3E=(const char*)_untag_dynforward_ptr(Cyc_strconcat((
struct _dynforward_ptr)_tmp3D,({const char*_tmp6B=".cyp";_tag_dynforward(_tmp6B,
sizeof(char),_get_zero_arr_size(_tmp6B,5));})),sizeof(char),1);struct
_dynforward_ptr _tmp3F=Cyc_str_sepstr(({struct Cyc_List_List*_tmp67=_cycalloc(
sizeof(*_tmp67));_tmp67->hd=({struct _dynforward_ptr*_tmp68=_cycalloc(sizeof(*
_tmp68));_tmp68[0]=(struct _dynforward_ptr)({const char*_tmp69="";_tag_dynforward(
_tmp69,sizeof(char),_get_zero_arr_size(_tmp69,1));});_tmp68;});_tmp67->tl=((
struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs));_tmp67;}),({const char*_tmp6A=" ";
_tag_dynforward(_tmp6A,sizeof(char),_get_zero_arr_size(_tmp6A,2));}));const char*
_tmp40=(const char*)_untag_dynforward_ptr(({struct Cyc_String_pa_struct _tmp65;
_tmp65.tag=0;_tmp65.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(
filename));{struct Cyc_String_pa_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(({const char*_tmp66=
_tmp3E;_tag_dynforward(_tmp66,sizeof(char),_get_zero_arr_size(_tmp66,1));})));{
struct Cyc_String_pa_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp3F);{struct Cyc_String_pa_struct _tmp62;_tmp62.tag=0;
_tmp62.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_cyclone_file);{void*
_tmp60[4]={& _tmp62,& _tmp63,& _tmp64,& _tmp65};Cyc_aprintf(({const char*_tmp61="%s %s -E -o %s -x cyc %s";
_tag_dynforward(_tmp61,sizeof(char),_get_zero_arr_size(_tmp61,25));}),
_tag_dynforward(_tmp60,sizeof(void*),4));}}}}}),sizeof(char),1);if(system(_tmp40)
!= 0){({void*_tmp41[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp42="\nError: preprocessing\n";
_tag_dynforward(_tmp42,sizeof(char),_get_zero_arr_size(_tmp42,23));}),
_tag_dynforward(_tmp41,sizeof(void*),0));});return;}Cyc_Position_reset_position(({
const char*_tmp43=_tmp3E;_tag_dynforward(_tmp43,sizeof(char),_get_zero_arr_size(
_tmp43,1));}));{struct Cyc___cycFILE*in_file=(struct Cyc___cycFILE*)_check_null(
Cyc_fopen(_tmp3E,(const char*)"r"));Cyc_Lex_lex_init(1);{struct Cyc_List_List*
_tmp44=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init(1);Cyc_file_close((struct
Cyc___cycFILE*)in_file);_tmp44=Cyc_flatten_decls(_tmp44);{struct Cyc_List_List*
_tmp45=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(int),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,_tmp44));Cyc_Lineno_poss_of_abss(({
const char*_tmp46=_tmp3E;_tag_dynforward(_tmp46,sizeof(char),_get_zero_arr_size(
_tmp46,1));}),_tmp45);remove(_tmp3E);{struct Cyc_List_List*_tmp47=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple12*))Cyc_Core_snd,_tmp45),_tmp44);_tmp47=((
struct Cyc_List_List*(*)(struct _dynforward_ptr file,struct Cyc_List_List*x))Cyc_this_file)(
filename,_tmp47);{struct Cyc_List_List*_tmp48=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct
Cyc_Lineno_Pos*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple15*))Cyc_Core_fst,_tmp47)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple15*))Cyc_Core_snd,_tmp47));struct Cyc___cycFILE*
_tmp49=(struct Cyc___cycFILE*)_check_null(Cyc_fopen((const char*)
_untag_dynforward_ptr(filename,sizeof(char),1),(const char*)"r"));struct Cyc_Lexing_lexbuf*
_tmp4A=Cyc_Lexing_from_file(_tmp49);struct Cyc_List_List*_tmp4B=0;struct _tuple11*
comment;while((comment=((struct _tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(
_tmp4A))!= 0){_tmp4B=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C->hd=(struct _tuple11*)_check_null(comment);_tmp4C->tl=_tmp4B;_tmp4C;});}
Cyc_fclose(_tmp49);_tmp4B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4B);{struct Cyc_List_List*_tmp4D=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(
int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(
int(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((int(*)(struct
_tuple11*))Cyc_Core_fst,_tmp4B));Cyc_Lineno_poss_of_abss(filename,_tmp4D);{
struct Cyc_List_List*_tmp4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple12*))Cyc_Core_snd,_tmp4D),((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp4B));_tmp4E=Cyc_this_file(filename,_tmp4E);{struct Cyc_List_List*_tmp4F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple13*))Cyc_Core_fst,_tmp4E)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))
Cyc_Core_snd,_tmp4E));while(_tmp48 != 0  && _tmp4F != 0){struct _tuple14 _tmp51;int
_tmp52;struct Cyc_Absyn_Decl*_tmp53;struct _tuple14*_tmp50=(struct _tuple14*)_tmp48->hd;
_tmp51=*_tmp50;_tmp52=_tmp51.f1;_tmp53=_tmp51.f2;{struct _tuple11 _tmp55;int _tmp56;
void*_tmp57;struct _tuple11*_tmp54=(struct _tuple11*)_tmp4F->hd;_tmp55=*_tmp54;
_tmp56=_tmp55.f1;_tmp57=_tmp55.f2;{void*_tmp58=_tmp57;struct _dynforward_ptr
_tmp59;struct _dynforward_ptr _tmp5A;_LL33: if(*((int*)_tmp58)!= 1)goto _LL35;_tmp59=((
struct Cyc_Standalone_struct*)_tmp58)->f1;_LL34: Cyc_pr_comment(Cyc_stdout,(struct
_dynforward_ptr)_tmp59);({void*_tmp5B[0]={};Cyc_printf(({const char*_tmp5C="\n";
_tag_dynforward(_tmp5C,sizeof(char),_get_zero_arr_size(_tmp5C,2));}),
_tag_dynforward(_tmp5B,sizeof(void*),0));});_tmp4F=_tmp4F->tl;goto _LL32;_LL35:
if(*((int*)_tmp58)!= 0)goto _LL32;_tmp5A=((struct Cyc_MatchDecl_struct*)_tmp58)->f1;
_LL36: if(_tmp56 < _tmp52){_tmp4F=_tmp4F->tl;continue;}if(_tmp48->tl != 0){struct
_tuple14 _tmp5E;int _tmp5F;struct _tuple14*_tmp5D=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(_tmp48->tl))->hd;_tmp5E=*_tmp5D;_tmp5F=_tmp5E.f1;if(_tmp5F < _tmp56){
_tmp48=_tmp48->tl;continue;}}Cyc_dumpdecl(_tmp53,(struct _dynforward_ptr)_tmp5A);
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stdout);_tmp48=_tmp48->tl;_tmp4F=_tmp4F->tl;
goto _LL32;_LL32:;}}}}}}}}}}}}void GC_blacklist_warn_clear();struct _tuple16{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv){GC_blacklist_warn_clear();{
struct Cyc_List_List*options=({struct _tuple16*_tmp6F[5];_tmp6F[4]=({struct
_tuple16*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=({const char*_tmp8D="-B";
_tag_dynforward(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,3));});_tmp88->f2=
1;_tmp88->f3=({const char*_tmp8C="<file>";_tag_dynforward(_tmp8C,sizeof(char),
_get_zero_arr_size(_tmp8C,7));});_tmp88->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp8B;_tmp8B.tag=1;_tmp8B.f1=Cyc_add_cycarg;_tmp8B;});_tmp8A;});_tmp88->f5=({
const char*_tmp89="Add to the list of directories to search for compiler files";
_tag_dynforward(_tmp89,sizeof(char),_get_zero_arr_size(_tmp89,60));});_tmp88;});
_tmp6F[3]=({struct _tuple16*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=({const
char*_tmp87="-I";_tag_dynforward(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,3));});
_tmp82->f2=1;_tmp82->f3=({const char*_tmp86="<dir>";_tag_dynforward(_tmp86,
sizeof(char),_get_zero_arr_size(_tmp86,6));});_tmp82->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp85;_tmp85.tag=1;_tmp85.f1=Cyc_add_cycarg;_tmp85;});_tmp84;});_tmp82->f5=({
const char*_tmp83="Add to the list of directories to search for include files";
_tag_dynforward(_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,59));});_tmp82;});
_tmp6F[2]=({struct _tuple16*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=({const
char*_tmp81="-D";_tag_dynforward(_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,3));});
_tmp7C->f2=1;_tmp7C->f3=({const char*_tmp80="<name>[=<value>]";_tag_dynforward(
_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,17));});_tmp7C->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp7F;_tmp7F.tag=1;_tmp7F.f1=Cyc_add_cycarg;_tmp7F;});_tmp7E;});_tmp7C->f5=({
const char*_tmp7D="Pass definition to preprocessor";_tag_dynforward(_tmp7D,
sizeof(char),_get_zero_arr_size(_tmp7D,32));});_tmp7C;});_tmp6F[1]=({struct
_tuple16*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=({const char*_tmp7B="-w";
_tag_dynforward(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,3));});_tmp76->f2=
0;_tmp76->f3=({const char*_tmp7A=" <width>";_tag_dynforward(_tmp7A,sizeof(char),
_get_zero_arr_size(_tmp7A,9));});_tmp76->f4=(void*)({struct Cyc_Arg_Int_spec_struct*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Arg_Int_spec_struct
_tmp79;_tmp79.tag=6;_tmp79.f1=Cyc_set_width;_tmp79;});_tmp78;});_tmp76->f5=({
const char*_tmp77="Use <width> as the max width for printing declarations";
_tag_dynforward(_tmp77,sizeof(char),_get_zero_arr_size(_tmp77,55));});_tmp76;});
_tmp6F[0]=({struct _tuple16*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=({const
char*_tmp75="-cyclone";_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(
_tmp75,9));});_tmp70->f2=0;_tmp70->f3=({const char*_tmp74=" <file>";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,8));});_tmp70->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp72=_cycalloc(sizeof(*_tmp72));
_tmp72[0]=({struct Cyc_Arg_String_spec_struct _tmp73;_tmp73.tag=5;_tmp73.f1=Cyc_set_cyclone_file;
_tmp73;});_tmp72;});_tmp70->f5=({const char*_tmp71="Use <file> as the cyclone compiler";
_tag_dynforward(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,35));});_tmp70;});((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp6F,sizeof(struct _tuple16*),5));});Cyc_Arg_parse(options,Cyc_add_other,({
const char*_tmp6C="Options:";_tag_dynforward(_tmp6C,sizeof(char),
_get_zero_arr_size(_tmp6C,9));}),argv);if(Cyc_cycdoc_files == 0){Cyc_Arg_usage(
options,({const char*_tmp6D="Usage: cycdoc [options] file1 file2 ...\nOptions:";
_tag_dynforward(_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,49));}));exit(1);}
Cyc_PP_tex_output=1;Cyc_Absynpp_print_scopes=0;{struct Cyc_List_List*_tmp6E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycdoc_files);
for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){Cyc_process_file(*((struct _dynforward_ptr*)
_tmp6E->hd));}}return 0;}}
