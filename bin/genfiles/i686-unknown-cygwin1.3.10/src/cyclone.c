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
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct _dynforward_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*tag;struct _dynforward_ptr
f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();
};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dynforward_ptr);};
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dynforward_ptr,
struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dynforward_ptr);};void Cyc_Arg_parse(
struct Cyc_List_List*specs,void(*anonfun)(struct _dynforward_ptr),struct
_dynforward_ptr errmsg,struct _dynforward_ptr args);struct Cyc___cycFILE;extern
struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,
const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct
_dynforward_ptr);unsigned int Cyc_fread(struct _dynforward_ptr,unsigned int,
unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};unsigned int Cyc_fwrite(struct _dynforward_ptr,
unsigned int,unsigned int,struct Cyc___cycFILE*);int Cyc_printf(struct
_dynforward_ptr,struct _dynforward_ptr);int remove(const char*);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dynforward_ptr);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,
int ofs,unsigned int n);struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,
char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct _dynforward_ptr Cyc_Filename_concat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_dirname(struct
_dynforward_ptr);struct _dynforward_ptr Cyc_Filename_basename(struct
_dynforward_ptr);int Cyc_Filename_check_suffix(struct _dynforward_ptr,struct
_dynforward_ptr);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dynforward_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
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
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();void Cyc_Tc_tc(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessSize_struct
LessSize;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqSize_struct
LessEqSize;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple11{struct _dynforward_ptr f1;struct _dynforward_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);void Cyc_Port_port(
struct Cyc_List_List*);void Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int
Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=
0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=
0;static int Cyc_elim_se_r=0;static int Cyc_tovc_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=
0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=
0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=
0;static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int
Cyc_print_all_kinds_r=0;static int Cyc_print_all_effects_r=0;void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=
1;}static struct _dynforward_ptr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _dynforward_ptr s){Cyc_output_file=({struct _dynforward_ptr*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0[0]=s;_tmp0;});}extern char*Cdef_inc_path;extern char*
Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;static char
_tmp1[1]="";static struct _dynforward_ptr Cyc_cpp={_tmp1,_tmp1 + 1};static void Cyc_set_cpp(
struct _dynforward_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dynforward_ptr s){Cyc_cppargs=({struct Cyc_List_List*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=({struct _dynforward_ptr*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3[0]=s;_tmp3;});_tmp2->tl=Cyc_cppargs;_tmp2;});}
static struct _dynforward_ptr Cyc_target_arch=(struct _dynforward_ptr){(void*)0,(
void*)(0 + 0)};static void Cyc_set_target_arch(struct _dynforward_ptr s){Cyc_toc_r=1;
Cyc_target_arch=s;}static void Cyc_print_version(){({struct Cyc_String_pa_struct
_tmp6;_tmp6.tag=0;_tmp6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Cversion));{void*_tmp4[1]={& _tmp6};Cyc_printf(({const char*
_tmp5="The Cyclone compiler, version %s\n";_tag_dynforward(_tmp5,sizeof(char),
_get_zero_arr_size(_tmp5,34));}),_tag_dynforward(_tmp4,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Ccomp));{struct Cyc_String_pa_struct _tmp9;
_tmp9.tag=0;_tmp9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Carch));{void*_tmp7[2]={& _tmp9,& _tmpA};Cyc_printf(({const char*
_tmp8="Compiled for architecture: %s CC=\"%s\"\n";_tag_dynforward(_tmp8,sizeof(
char),_get_zero_arr_size(_tmp8,39));}),_tag_dynforward(_tmp7,sizeof(void*),2));}}});({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path));{void*_tmpB[1]={& _tmpD};
Cyc_printf(({const char*_tmpC="Standard library directory: %s\n";_tag_dynforward(
_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,32));}),_tag_dynforward(_tmpB,
sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path));{
void*_tmpE[1]={& _tmp10};Cyc_printf(({const char*_tmpF="Standard include directory: %s\n";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,32));}),
_tag_dynforward(_tmpE,sizeof(void*),1));}});exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _dynforward_ptr s){unsigned int _tmp11=Cyc_strlen((struct _dynforward_ptr)s);
if(_tmp11 <= 4)return 0;else{return Cyc_strcmp((struct _dynforward_ptr)
_dynforward_ptr_plus(s,sizeof(char),(int)(_tmp11 - 4)),({const char*_tmp12=".cyc";
_tag_dynforward(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,5));}))== 0;}}
static struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct
_dynforward_ptr s){Cyc_ccargs=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13->hd=({struct _dynforward_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));
_tmp14[0]=s;_tmp14;});_tmp13->tl=Cyc_ccargs;_tmp13;});}static struct Cyc_List_List*
Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(struct
_dynforward_ptr s){unsigned int _tmp15=Cyc_strlen((struct _dynforward_ptr)s);if(
_tmp15 <= 2)return;{struct _dynforward_ptr _tmp16=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)s,2,_tmp15 - 2);Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct _dynforward_ptr*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18[0]=_tmp16;_tmp18;});_tmp17->tl=Cyc_cyclone_exec_path;
_tmp17;});}}static struct Cyc_List_List*Cyc_libargs=0;static void Cyc_add_libarg(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp19="-lxml";_tag_dynforward(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,6));}))
== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1A=_cycalloc(
sizeof(*_tmp1A));_tmp1A->hd=({struct _dynforward_ptr*_tmp1B=_cycalloc(sizeof(*
_tmp1B));_tmp1B[0]=s;_tmp1B;});_tmp1A->tl=Cyc_libargs;_tmp1A;});}}static void Cyc_add_marg(
struct _dynforward_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){Cyc_save_temps_r=
1;Cyc_add_ccarg(({const char*_tmp1C="-save-temps";_tag_dynforward(_tmp1C,sizeof(
char),_get_zero_arr_size(_tmp1C,12));}));}static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;Cyc_add_cpparg(({const char*_tmp1D="-M";_tag_dynforward(_tmp1D,sizeof(char),
_get_zero_arr_size(_tmp1D,3));}));}static struct _dynforward_ptr*Cyc_dependencies_target=
0;static void Cyc_set_dependencies_target(struct _dynforward_ptr s){Cyc_dependencies_target=({
struct _dynforward_ptr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=s;_tmp1E;});}
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({
const char*_tmp1F="-c";_tag_dynforward(_tmp1F,sizeof(char),_get_zero_arr_size(
_tmp1F,3));}));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({const char*
_tmp20="-no-cpp-precomp";_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size(
_tmp20,16));}));Cyc_add_ccarg(({const char*_tmp21="-no-cpp-precomp";
_tag_dynforward(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,16));}));}static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_tovc(){Cyc_elim_se_r=1;Cyc_tovc_r=1;}static void Cyc_set_inline_functions(){
Cyc_add_ccarg(({const char*_tmp22="-D_INLINE_FUNCTIONS";_tag_dynforward(_tmp22,
sizeof(char),_get_zero_arr_size(_tmp22,20));}));}static void Cyc_set_noboundschecks(){
Cyc_add_ccarg(({const char*_tmp23="-DNO_CYC_BOUNDS_CHECKS";_tag_dynforward(_tmp23,
sizeof(char),_get_zero_arr_size(_tmp23,23));}));}static void Cyc_set_nonullchecks(){
Cyc_add_ccarg(({const char*_tmp24="-DNO_CYC_NULL_CHECKS";_tag_dynforward(_tmp24,
sizeof(char),_get_zero_arr_size(_tmp24,21));}));}static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;Cyc_add_ccarg(({const char*_tmp25="-DNO_CYC_PREFIX";
_tag_dynforward(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,16));}));}static
void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({const char*_tmp26="-DCYC_REGION_PROFILE";
_tag_dynforward(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,21));}));}static
void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({const char*_tmp27="-pg";
_tag_dynforward(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,4));}));}static
void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(({
const char*_tmp28="-S";_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size(
_tmp28,3));}));}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;}static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp29="cyc";_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,4));}))
== 0)Cyc_intype=(void*)1;else{if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp2A="none";_tag_dynforward(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,5));}))
== 0)Cyc_intype=(void*)0;else{({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;
_tmp2D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp2B[1]={&
_tmp2D};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C="Input type '%s' not supported\n";
_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,31));}),
_tag_dynforward(_tmp2B,sizeof(void*),1));}});}}}struct _dynforward_ptr Cyc_make_base_filename(
struct _dynforward_ptr s,struct _dynforward_ptr*output_file){struct _dynforward_ptr
_tmp2E=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct
_dynforward_ptr _tmp2F=Cyc_Filename_chop_extension((struct _dynforward_ptr)Cyc_Filename_basename(
s));struct _dynforward_ptr _tmp30=Cyc_strlen((struct _dynforward_ptr)_tmp2E)> 0?Cyc_Filename_concat((
struct _dynforward_ptr)_tmp2E,(struct _dynforward_ptr)_tmp2F): _tmp2F;return(struct
_dynforward_ptr)_tmp30;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void
Cyc_add_other(struct _dynforward_ptr s){if(Cyc_intype == (void*)1  || Cyc_is_cyclone_sourcefile(
s)){Cyc_cyclone_files=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31->hd=({struct _dynforward_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=s;
_tmp32;});_tmp31->tl=Cyc_cyclone_files;_tmp31;});{struct _dynforward_ptr _tmp33=
Cyc_make_base_filename(s,Cyc_output_file);struct _dynforward_ptr _tmp34=Cyc_strconcat((
struct _dynforward_ptr)_tmp33,({const char*_tmp35=".c";_tag_dynforward(_tmp35,
sizeof(char),_get_zero_arr_size(_tmp35,3));}));Cyc_add_ccarg((struct
_dynforward_ptr)_tmp34);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dynforward_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dynforward_ptr(s,sizeof(char),1));}}int Cyc_compile_failure=0;struct Cyc___cycFILE*
Cyc_try_file_open(struct _dynforward_ptr filename,struct _dynforward_ptr mode,struct
_dynforward_ptr msg_part){struct _handler_cons _tmp36;_push_handler(& _tmp36);{int
_tmp38=0;if(setjmp(_tmp36.handler))_tmp38=1;if(!_tmp38){{struct Cyc___cycFILE*
_tmp39=(struct Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);
return _tmp39;};_pop_handler();}else{void*_tmp37=(void*)_exn_thrown;void*_tmp3B=
_tmp37;_LL1:;_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct _tmp3F;
_tmp3F.tag=0;_tmp3F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)filename);{
struct Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)msg_part);{void*_tmp3C[2]={& _tmp3E,& _tmp3F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp3D="\nError: couldn't open %s %s\n";_tag_dynforward(
_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,29));}),_tag_dynforward(_tmp3C,
sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;
_LL4:(void)_throw(_tmp3B);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),
void*env,struct Cyc_List_List*tds){struct _dynforward_ptr exn_string=({const char*
_tmp5D="";_tag_dynforward(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,1));});
struct _dynforward_ptr explain_string=({const char*_tmp5C="";_tag_dynforward(_tmp5C,
sizeof(char),_get_zero_arr_size(_tmp5C,1));});int other_exn=0;void*ex=(void*)({
struct Cyc_Core_Impossible_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({
struct Cyc_Core_Impossible_struct _tmp5A;_tmp5A.tag=Cyc_Core_Impossible;_tmp5A.f1=({
const char*_tmp5B="";_tag_dynforward(_tmp5B,sizeof(char),_get_zero_arr_size(
_tmp5B,1));});_tmp5A;});_tmp59;});struct Cyc_List_List*_tmp40=0;{struct
_handler_cons _tmp41;_push_handler(& _tmp41);{int _tmp43=0;if(setjmp(_tmp41.handler))
_tmp43=1;if(!_tmp43){_tmp40=f(env,tds);;_pop_handler();}else{void*_tmp42=(void*)
_exn_thrown;void*_tmp45=_tmp42;struct _dynforward_ptr _tmp46;struct _dynforward_ptr
_tmp47;struct _dynforward_ptr _tmp48;_LL6: if(*((void**)_tmp45)!= Cyc_Core_Impossible)
goto _LL8;_tmp46=((struct Cyc_Core_Impossible_struct*)_tmp45)->f1;_LL7: exn_string=({
const char*_tmp49="Exception Core::Impossible";_tag_dynforward(_tmp49,sizeof(char),
_get_zero_arr_size(_tmp49,27));});explain_string=_tmp46;goto _LL5;_LL8: if(_tmp45
!= Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp4A="Exception Dict::Absent";
_tag_dynforward(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,23));});goto _LL5;
_LLA: if(*((void**)_tmp45)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp47=((struct
Cyc_Core_Invalid_argument_struct*)_tmp45)->f1;_LLB: exn_string=({const char*_tmp4B="Exception Core::Invalid_argument";
_tag_dynforward(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,33));});
explain_string=_tmp47;goto _LL5;_LLC: if(*((void**)_tmp45)!= Cyc_Core_Failure)goto
_LLE;_tmp48=((struct Cyc_Core_Failure_struct*)_tmp45)->f1;_LLD: exn_string=({const
char*_tmp4C="Exception Core::Failure";_tag_dynforward(_tmp4C,sizeof(char),
_get_zero_arr_size(_tmp4C,24));});explain_string=_tmp48;goto _LL5;_LLE:;_LLF: ex=
_tmp45;other_exn=1;exn_string=({const char*_tmp4D="Uncaught exception";
_tag_dynforward(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,19));});goto _LL5;
_LL10:;_LL11:(void)_throw(_tmp45);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=
1;if(Cyc_strcmp((struct _dynforward_ptr)exn_string,({const char*_tmp4E="";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,1));}))!= 0){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)explain_string);{struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)exn_string);{void*_tmp4F[3]={& _tmp51,&
_tmp52,& _tmp53};Cyc_fprintf(Cyc_stderr,({const char*_tmp50="\n%s thrown during %s: %s";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,25));}),
_tag_dynforward(_tmp4F,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({void*
_tmp54[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp55="\nCOMPILATION FAILED!\n";
_tag_dynforward(_tmp55,sizeof(char),_get_zero_arr_size(_tmp55,22));}),
_tag_dynforward(_tmp54,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
if(other_exn)(int)_throw(ex);return _tmp40;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct
_tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{void*_tmp56[1]={& _tmp58};Cyc_fprintf(Cyc_stderr,({const char*_tmp57="%s completed.\n";
_tag_dynforward(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,15));}),
_tag_dynforward(_tmp56,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return _tmp40;}}return _tmp40;}struct Cyc_List_List*Cyc_do_parse(struct
Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*
_tmp5E=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);return _tmp5E;}}struct _tuple12{
struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(
struct _tuple12*tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp60;struct
Cyc_Tcenv_Tenv*_tmp61;struct _tuple12 _tmp5F=*tcenv;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
Cyc_Tc_tc(_tmp60,_tmp61,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp61,tds);
return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);return tds;}struct _tuple13{struct Cyc_Tcenv_Tenv*
f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(
struct _tuple13*params,struct Cyc_List_List*tds){struct _tuple13 _tmp63;struct Cyc_Tcenv_Tenv*
_tmp64;struct Cyc___cycFILE*_tmp65;struct Cyc___cycFILE*_tmp66;struct _tuple13*
_tmp62=params;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;_tmp66=_tmp63.f3;{
struct Cyc_Interface_I*_tmp67=Cyc_Interface_extract(_tmp64->ae);if(_tmp65 == 0)Cyc_Interface_save(
_tmp67,_tmp66);else{struct Cyc_Interface_I*_tmp68=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp65);if(!Cyc_Interface_is_subinterface(_tmp68,_tmp67,({struct _tuple11*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->f1=({const char*_tmp6B="written interface";
_tag_dynforward(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,18));});_tmp69->f2=({
const char*_tmp6A="maximal interface";_tag_dynforward(_tmp6A,sizeof(char),
_get_zero_arr_size(_tmp6A,18));});_tmp69;})))Cyc_compile_failure=1;else{Cyc_Interface_save(
_tmp68,_tmp66);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=Cyc_tovc_r;
return Cyc_Tovc_tovc(tds);}static char _tmp6C[1]="";static struct _dynforward_ptr Cyc_cyc_include={
_tmp6C,_tmp6C + 1};static void Cyc_set_cyc_include(struct _dynforward_ptr f){Cyc_cyc_include=
f;}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*
tds){struct Cyc_Absynpp_Params _tmp6D=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmp6D.expand_typedefs=!Cyc_noexpand_r;_tmp6D.to_VC=Cyc_tovc_r;_tmp6D.add_cyc_prefix=
Cyc_add_cyc_namespace_r;_tmp6D.generate_line_directives=Cyc_generate_line_directives_r;
_tmp6D.print_full_evars=Cyc_print_full_evars_r;_tmp6D.print_all_tvars=Cyc_print_all_tvars_r;
_tmp6D.print_all_kinds=Cyc_print_all_kinds_r;_tmp6D.print_all_effects=Cyc_print_all_effects_r;{
struct Cyc___cycFILE*_tmp6E=Cyc_try_file_open(Cyc_cyc_include,({const char*_tmp74="r";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,2));}),({const char*
_tmp75="internal compiler file";_tag_dynforward(_tmp75,sizeof(char),
_get_zero_arr_size(_tmp75,23));}));if(_tmp6E == 0)return tds;{unsigned int n_read=
1024;unsigned int n_written;char buf[1024];{unsigned int _tmp73=1024;unsigned int i;
for(i=0;i < _tmp73;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=Cyc_fread(
_tag_dynforward(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)_tmp6E);if(
n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp6E)){Cyc_compile_failure=1;({
void*_tmp6F[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp70="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,39));}),
_tag_dynforward(_tmp6F,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}n_written=Cyc_fwrite(_tag_dynforward(buf,sizeof(char),1024),1,n_read,
out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp71[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp72="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,39));}),
_tag_dynforward(_tmp71,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}}Cyc_fclose((struct Cyc___cycFILE*)_tmp6E);if(Cyc_pp_r){Cyc_Absynpp_set_params(&
_tmp6D);Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(&
_tmp6D);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}}}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dynforward_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dynforward_ptr s,char c){if(s.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmp76=0;unsigned int
_tmp77=Cyc_strlen((struct _dynforward_ptr)s);while(_tmp77 > 0){struct
_dynforward_ptr _tmp78=Cyc_strchr(s,c);if(_tmp78.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr){_tmp76=({struct Cyc_List_List*_tmp79=_cycalloc(
sizeof(*_tmp79));_tmp79->hd=({struct _dynforward_ptr*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A[0]=s;_tmp7A;});_tmp79->tl=_tmp76;_tmp79;});break;}else{_tmp76=({
struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->hd=({struct
_dynforward_ptr*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=(struct
_dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,0,(unsigned int)((((
struct _dynforward_ptr)_tmp78).curr - s.curr)/ sizeof(char)));_tmp7C;});_tmp7B->tl=
_tmp76;_tmp7B;});_tmp77 -=(((struct _dynforward_ptr)_tmp78).curr - s.curr)/ sizeof(
char);s=_dynforward_ptr_plus(_tmp78,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp76);}}static int Cyc_file_exists(
struct _dynforward_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmp7D;
_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))_tmp7F=1;if(!
_tmp7F){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmp80="r";
_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,2));}));;
_pop_handler();}else{void*_tmp7E=(void*)_exn_thrown;void*_tmp82=_tmp7E;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmp82);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}static struct _dynforward_ptr
Cyc_sprint_list(struct Cyc_List_List*dirs){struct _dynforward_ptr tmp=({const char*
_tmp85="";_tag_dynforward(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,1));});
for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmp83=*((struct
_dynforward_ptr*)dirs->hd);if(_tmp83.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmp83)== 0)
continue;_tmp83=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)
_tmp83,({const char*_tmp84=":";_tag_dynforward(_tmp84,sizeof(char),
_get_zero_arr_size(_tmp84,2));}));tmp=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)_tmp83,(struct _dynforward_ptr)tmp);}return tmp;}static
struct _dynforward_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dynforward_ptr
file){if(file.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return
0;for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmp86=*((struct
_dynforward_ptr*)dirs->hd);if(_tmp86.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmp86)== 0)
continue;{struct _dynforward_ptr s=(struct _dynforward_ptr)Cyc_Filename_concat(
_tmp86,file);if(Cyc_file_exists(s))return({struct _dynforward_ptr*_tmp87=
_cycalloc(sizeof(*_tmp87));_tmp87[0]=s;_tmp87;});}}return 0;}static struct
_dynforward_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dynforward_ptr file){
struct _dynforward_ptr*_tmp88=Cyc_find(dirs,file);if(_tmp88 == 0){Cyc_compile_failure=
1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{struct Cyc_String_pa_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmp89[2]={& _tmp8B,& _tmp8C};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8A="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,56));}),
_tag_dynforward(_tmp89,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp8D=_cycalloc(
sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Core_Failure_struct _tmp8E;_tmp8E.tag=Cyc_Core_Failure;
_tmp8E.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp92;_tmp92.tag=0;
_tmp92.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{
struct Cyc_String_pa_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp8F[2]={& _tmp91,& _tmp92};Cyc_aprintf(({
const char*_tmp90="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,56));}),
_tag_dynforward(_tmp8F,sizeof(void*),2));}}});_tmp8E;});_tmp8D;}));}return*
_tmp88;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_dynforward_ptr subdir){struct Cyc_List_List*_tmp93=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp93=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->hd=({
struct _dynforward_ptr*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),subdir);
_tmp95;});_tmp94->tl=_tmp93;_tmp94;});}_tmp93=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp93);return _tmp93;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*
_tmp96=0;for(0;dirs != 0;dirs=dirs->tl){_tmp96=({struct Cyc_List_List*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97->hd=(struct _dynforward_ptr*)dirs->hd;_tmp97->tl=
_tmp96;_tmp97;});_tmp96=({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98->hd=({struct _dynforward_ptr*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=(
struct _dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),
subdir);_tmp99;});_tmp98->tl=_tmp96;_tmp98;});}_tmp96=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp96);return _tmp96;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s){unsigned int _tmp9A=Cyc_strlen((struct _dynforward_ptr)s);
int _tmp9B=0;int _tmp9C=0;{int i=0;for(0;i < _tmp9A;i ++){char _tmp9D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmp9D == '\'')_tmp9B ++;else{
if(Cyc_is_other_special(_tmp9D))_tmp9C ++;}}}if(_tmp9B == 0  && _tmp9C == 0)return s;
if(_tmp9B == 0)return(struct _dynforward_ptr)Cyc_strconcat_l(({struct
_dynforward_ptr*_tmp9E[3];_tmp9E[2]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2);_tmp9E[1]=({struct _dynforward_ptr*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=(struct _dynforward_ptr)s;_tmpA0;});_tmp9E[0]=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp9E,sizeof(struct _dynforward_ptr*),3));}));{unsigned int _tmpA2=(_tmp9A + 
_tmp9B)+ _tmp9C;struct _dynforward_ptr s2=({unsigned int _tmpAC=_tmpA2 + 1;char*
_tmpAD=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpAC + 1));struct
_dynforward_ptr _tmpAF=_tag_dynforward(_tmpAD,sizeof(char),_tmpAC + 1);{
unsigned int _tmpAE=_tmpAC;unsigned int i;for(i=0;i < _tmpAE;i ++){_tmpAD[i]='\000';}
_tmpAD[_tmpAE]=(char)0;}_tmpAF;});int _tmpA3=0;int _tmpA4=0;for(0;_tmpA3 < _tmp9A;
_tmpA3 ++){char _tmpA5=*((const char*)_check_dynforward_subscript(s,sizeof(char),
_tmpA3));if(_tmpA5 == '\''  || Cyc_is_other_special(_tmpA5))({struct
_dynforward_ptr _tmpA6=_dynforward_ptr_plus(s2,sizeof(char),_tmpA4 ++);char _tmpA7=*((
char*)_check_dynforward_subscript(_tmpA6,sizeof(char),0));char _tmpA8='\\';if(
_get_dynforward_size(_tmpA6,sizeof(char))== 1  && (_tmpA7 == '\000'  && _tmpA8 != '\000'))
_throw_arraybounds();*((char*)_tmpA6.curr)=_tmpA8;});({struct _dynforward_ptr
_tmpA9=_dynforward_ptr_plus(s2,sizeof(char),_tmpA4 ++);char _tmpAA=*((char*)
_check_dynforward_subscript(_tmpA9,sizeof(char),0));char _tmpAB=_tmpA5;if(
_get_dynforward_size(_tmpA9,sizeof(char))== 1  && (_tmpAA == '\000'  && _tmpAB != '\000'))
_throw_arraybounds();*((char*)_tmpA9.curr)=_tmpAB;});}return(struct
_dynforward_ptr)s2;}}static struct _dynforward_ptr*Cyc_sh_escape_stringptr(struct
_dynforward_ptr*sp){return({struct _dynforward_ptr*_tmpB0=_cycalloc(sizeof(*
_tmpB0));_tmpB0[0]=Cyc_sh_escape_string(*sp);_tmpB0;});}static void Cyc_process_file(
struct _dynforward_ptr filename){struct _dynforward_ptr _tmpB1=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dynforward_ptr _tmpB2=Cyc_strconcat((struct
_dynforward_ptr)_tmpB1,({const char*_tmp10C=".cyp";_tag_dynforward(_tmp10C,
sizeof(char),_get_zero_arr_size(_tmp10C,5));}));struct _dynforward_ptr _tmpB3=Cyc_strconcat((
struct _dynforward_ptr)_tmpB1,({const char*_tmp10B=".cyci";_tag_dynforward(_tmp10B,
sizeof(char),_get_zero_arr_size(_tmp10B,6));}));struct _dynforward_ptr _tmpB4=Cyc_strconcat((
struct _dynforward_ptr)_tmpB1,({const char*_tmp10A=".cycio";_tag_dynforward(
_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,7));}));struct _dynforward_ptr
_tmpB5=Cyc_strconcat((struct _dynforward_ptr)_tmpB1,({const char*_tmp109=".c";
_tag_dynforward(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,3));}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)filename);{void*_tmpB6[1]={& _tmpB8};Cyc_fprintf(Cyc_stderr,({
const char*_tmpB7="Compiling %s\n";_tag_dynforward(_tmpB7,sizeof(char),
_get_zero_arr_size(_tmpB7,14));}),_tag_dynforward(_tmpB6,sizeof(void*),1));}});{
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp107="r";
_tag_dynforward(_tmp107,sizeof(char),_get_zero_arr_size(_tmp107,2));}),({const
char*_tmp108="input file";_tag_dynforward(_tmp108,sizeof(char),
_get_zero_arr_size(_tmp108,11));}));if(Cyc_compile_failure  || !((unsigned int)f0))
return;Cyc_fclose((struct Cyc___cycFILE*)f0);{struct _dynforward_ptr _tmpB9=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->hd=({struct
_dynforward_ptr*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=(struct
_dynforward_ptr)({const char*_tmp105="";_tag_dynforward(_tmp105,sizeof(char),
_get_zero_arr_size(_tmp105,1));});_tmp104;});_tmp103->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmp103;}),({const char*_tmp106=" ";_tag_dynforward(_tmp106,sizeof(
char),_get_zero_arr_size(_tmp106,2));}));struct _dynforward_ptr def_inc_path=(
struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmpBA=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmpBA=Cyc_add_subdir(
_tmpBA,({const char*_tmpBB="include";_tag_dynforward(_tmpBB,sizeof(char),
_get_zero_arr_size(_tmpBB,8));}));if(Cyc_strlen((struct _dynforward_ptr)
def_inc_path)> 0)_tmpBA=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));
_tmpBC->hd=({struct _dynforward_ptr*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=
def_inc_path;_tmpBD;});_tmpBC->tl=_tmpBA;_tmpBC;});{char*_tmpBE=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpBE != 0)_tmpBA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpBF=_tmpBE;_tag_dynforward(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,
1));}),':'),_tmpBA);{struct _dynforward_ptr stdinc_string=(struct _dynforward_ptr)
Cyc_str_sepstr(({struct Cyc_List_List*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->hd=({
struct _dynforward_ptr*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=(struct
_dynforward_ptr)({const char*_tmp101="";_tag_dynforward(_tmp101,sizeof(char),
_get_zero_arr_size(_tmp101,1));});_tmp100;});_tmpFF->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpBA);_tmpFF;}),({const char*_tmp102=" -I";
_tag_dynforward(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,4));}));struct
_dynforward_ptr ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)
ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=
0;_tmpC2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)));{void*_tmpC0[1]={& _tmpC2};Cyc_aprintf(({
const char*_tmpC1=" > %s";_tag_dynforward(_tmpC1,sizeof(char),_get_zero_arr_size(
_tmpC1,6));}),_tag_dynforward(_tmpC0,sizeof(void*),1));}});else{ofile_string=({
const char*_tmpC3="";_tag_dynforward(_tmpC3,sizeof(char),_get_zero_arr_size(
_tmpC3,1));});}}else{ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmpC6;_tmpC6.tag=0;_tmpC6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string((
struct _dynforward_ptr)_tmpB2));{void*_tmpC4[1]={& _tmpC6};Cyc_aprintf(({const char*
_tmpC5=" > %s";_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,6));}),
_tag_dynforward(_tmpC4,sizeof(void*),1));}});}{struct _dynforward_ptr fixup_string;
if(Cyc_produce_dependencies){if(Cyc_dependencies_target == 0)fixup_string=({const
char*_tmpC7=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dynforward(_tmpC7,
sizeof(char),_get_zero_arr_size(_tmpC7,35));});else{fixup_string=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_null(Cyc_dependencies_target)));{void*_tmpC8[1]={& _tmpCA};Cyc_aprintf(({
const char*_tmpC9=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dynforward(_tmpC9,sizeof(
char),_get_zero_arr_size(_tmpC9,29));}),_tag_dynforward(_tmpC8,sizeof(void*),1));}});}}
else{fixup_string=({const char*_tmpCB="";_tag_dynforward(_tmpCB,sizeof(char),
_get_zero_arr_size(_tmpCB,1));});}{struct _dynforward_ptr _tmpCC=({struct Cyc_String_pa_struct
_tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
ofile_string);{struct Cyc_String_pa_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)fixup_string);{struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(
filename));{struct Cyc_String_pa_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdinc_string);{struct Cyc_String_pa_struct
_tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpB9);{struct Cyc_String_pa_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_cpp);{void*_tmpF7[6]={& _tmpF9,&
_tmpFA,& _tmpFB,& _tmpFC,& _tmpFD,& _tmpFE};Cyc_aprintf(({const char*_tmpF8="%s %s%s %s%s%s";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,15));}),
_tag_dynforward(_tmpF7,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmpCF;_tmpCF.tag=0;_tmpCF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpCC);{void*_tmpCD[1]={& _tmpCF};Cyc_fprintf(Cyc_stderr,({const char*_tmpCE="%s\n";
_tag_dynforward(_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,4));}),
_tag_dynforward(_tmpCD,sizeof(void*),1));}});if(system((const char*)
_untag_dynforward_ptr(_tmpCC,sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*
_tmpD0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpD1="\nError: preprocessing\n";
_tag_dynforward(_tmpD1,sizeof(char),_get_zero_arr_size(_tmpD1,23));}),
_tag_dynforward(_tmpD0,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dynforward_ptr)_tmpB2);{struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpB2,({const char*_tmpF5="r";
_tag_dynforward(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,2));}),({const char*
_tmpF6="file";_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,5));}));
if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{struct _handler_cons
_tmpD2;_push_handler(& _tmpD2);{int _tmpD4=0;if(setjmp(_tmpD2.handler))_tmpD4=1;
if(!_tmpD4){tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct
Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD5="parsing";
_tag_dynforward(_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,8));}),Cyc_do_parse,(
struct Cyc___cycFILE*)_check_null(in_file),tds);;_pop_handler();}else{void*_tmpD3=(
void*)_exn_thrown;void*_tmpD7=_tmpD3;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dynforward_ptr)_tmpB2);(int)_throw(
_tmpD7);_LL29:;_LL2A:(void)_throw(_tmpD7);_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpB2);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmpD8=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmpD8;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmpD9=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r){_npop_handler(0);goto PRINTC;}{struct _tuple12 _tmpDA=({struct
_tuple12 _tmpE2;_tmpE2.f1=tc_rgn;_tmpE2.f2=_tmpD9;_tmpE2;});tds=((struct Cyc_List_List*(*)(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple12*,struct
Cyc_List_List*),struct _tuple12*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpDB="type checking";_tag_dynforward(_tmpDB,sizeof(char),
_get_zero_arr_size(_tmpDB,14));}),Cyc_do_typecheck,& _tmpDA,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpB2);_npop_handler(0);return;}if(Cyc_tc_r){
_npop_handler(0);goto PRINTC;}tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpDC="control-flow checking";_tag_dynforward(
_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,22));}),Cyc_do_cfcheck,1,tds);if(
Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpB2);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dynforward_ptr(_tmpB3,sizeof(char),1)),(const char*)"r");
struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dynforward_ptr)
_tmpB4,({const char*_tmpE0="w";_tag_dynforward(_tmpE0,sizeof(char),
_get_zero_arr_size(_tmpE0,2));}),({const char*_tmpE1="interface object file";
_tag_dynforward(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,22));}));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dynforward_ptr)_tmpB3);{struct _tuple13 _tmpDD=({struct _tuple13 _tmpDF;
_tmpDF.f1=_tmpD9;_tmpDF.f2=inter_file;_tmpDF.f3=(struct Cyc___cycFILE*)
inter_objfile;_tmpDF;});tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(struct _tuple13*,struct Cyc_List_List*),struct
_tuple13*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpDE="interface checking";
_tag_dynforward(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,19));}),Cyc_do_interface,&
_tmpDD,tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);}}}};_pop_region(tc_rgn);}
if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpE3="translation to C";_tag_dynforward(_tmpE3,
sizeof(char),_get_zero_arr_size(_tmpE3,17));}),Cyc_do_translate,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpB2);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dynforward_ptr)_tmpB2);if(
Cyc_compile_failure)return;if(Cyc_tovc_r  || Cyc_elim_se_r)tds=((struct Cyc_List_List*(*)(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpE4="post-pass to VC";
_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,16));}),Cyc_do_tovc,
1,tds);if(Cyc_compile_failure)return;PRINTC: {struct Cyc___cycFILE*out_file;if((
Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmpE5="w";
_tag_dynforward(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,2));}),({const char*
_tmpE6="output file";_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size(
_tmpE6,12));}));else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmpE7="w";_tag_dynforward(_tmpE7,
sizeof(char),_get_zero_arr_size(_tmpE7,2));}),({const char*_tmpE8="output file";
_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,12));}));else{
out_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpB5,({const char*_tmpE9="w";
_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,2));}),({const char*
_tmpEA="output file";_tag_dynforward(_tmpEA,sizeof(char),_get_zero_arr_size(
_tmpEA,12));}));}}if(Cyc_compile_failure  || !((unsigned int)out_file))return;{
struct _handler_cons _tmpEB;_push_handler(& _tmpEB);{int _tmpED=0;if(setjmp(_tmpEB.handler))
_tmpED=1;if(!_tmpED){if(!Cyc_noprint_r)tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpEE="printing";_tag_dynforward(_tmpEE,sizeof(char),
_get_zero_arr_size(_tmpEE,9));}),Cyc_do_print,(struct Cyc___cycFILE*)out_file,tds);;
_pop_handler();}else{void*_tmpEC=(void*)_exn_thrown;void*_tmpF0=_tmpEC;_LL2C:;
_LL2D: Cyc_compile_failure=1;Cyc_file_close((struct Cyc___cycFILE*)out_file);Cyc_cfiles=({
struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=({struct
_dynforward_ptr*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=(struct
_dynforward_ptr)_tmpB5;_tmpF2;});_tmpF1->tl=Cyc_cfiles;_tmpF1;});(int)_throw(
_tmpF0);_LL2E:;_LL2F:(void)_throw(_tmpF0);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)
out_file);Cyc_cfiles=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));
_tmpF3->hd=({struct _dynforward_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=(
struct _dynforward_ptr)_tmpB5;_tmpF4;});_tmpF3->tl=Cyc_cfiles;_tmpF3;});}}}}}}}}}}
static char _tmp10D[8]="<final>";static struct _dynforward_ptr Cyc_final_str={_tmp10D,
_tmp10D + 8};static struct _dynforward_ptr*Cyc_final_strptr=& Cyc_final_str;static
struct Cyc_Interface_I*Cyc_read_cycio(struct _dynforward_ptr*n){if(n == (struct
_dynforward_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{struct
_dynforward_ptr basename;{struct _handler_cons _tmp10E;_push_handler(& _tmp10E);{int
_tmp110=0;if(setjmp(_tmp10E.handler))_tmp110=1;if(!_tmp110){basename=(struct
_dynforward_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp10F=(void*)_exn_thrown;void*_tmp112=_tmp10F;_LL31: if(*((void**)_tmp112)!= 
Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp112);_LL30:;}}}{struct _dynforward_ptr _tmp113=Cyc_strconcat((
struct _dynforward_ptr)basename,({const char*_tmp118=".cycio";_tag_dynforward(
_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,7));}));struct Cyc___cycFILE*
_tmp114=Cyc_try_file_open((struct _dynforward_ptr)_tmp113,({const char*_tmp116="rb";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,3));}),({const
char*_tmp117="interface object file";_tag_dynforward(_tmp117,sizeof(char),
_get_zero_arr_size(_tmp117,22));}));if(_tmp114 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _dynforward_ptr)_tmp113);{struct Cyc_Interface_I*
_tmp115=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp114);Cyc_file_close((struct
Cyc___cycFILE*)_tmp114);return _tmp115;}}}}static int Cyc_is_cfile(struct
_dynforward_ptr*n){return*((const char*)_check_dynforward_subscript(*n,sizeof(
char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple14{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv){GC_blacklist_warn_clear();{
struct _RegionHandle _tmp119=_new_region("r");struct _RegionHandle*r=& _tmp119;
_push_region(r);{struct Cyc_List_List*options=({struct _tuple14*_tmp11B[62];
_tmp11B[61]=({struct _tuple14*_tmp28A=_region_malloc(r,sizeof(*_tmp28A));_tmp28A->f1=({
const char*_tmp28F="-noregions";_tag_dynforward(_tmp28F,sizeof(char),
_get_zero_arr_size(_tmp28F,11));});_tmp28A->f2=0;_tmp28A->f3=({const char*_tmp28E="";
_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size(_tmp28E,1));});_tmp28A->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28C=_region_malloc(r,sizeof(*_tmp28C));
_tmp28C[0]=({struct Cyc_Arg_Set_spec_struct _tmp28D;_tmp28D.tag=3;_tmp28D.f1=& Cyc_Absyn_no_regions;
_tmp28D;});_tmp28C;});_tmp28A->f5=({const char*_tmp28B="Generate code that doesn't use regions";
_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,39));});_tmp28A;});
_tmp11B[60]=({struct _tuple14*_tmp284=_region_malloc(r,sizeof(*_tmp284));_tmp284->f1=({
const char*_tmp289="-port";_tag_dynforward(_tmp289,sizeof(char),
_get_zero_arr_size(_tmp289,6));});_tmp284->f2=0;_tmp284->f3=({const char*_tmp288="";
_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,1));});_tmp284->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp286=_region_malloc(r,sizeof(*_tmp286));
_tmp286[0]=({struct Cyc_Arg_Unit_spec_struct _tmp287;_tmp287.tag=0;_tmp287.f1=Cyc_set_port_c_code;
_tmp287;});_tmp286;});_tmp284->f5=({const char*_tmp285="Suggest how to port C code to Cyclone";
_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size(_tmp285,38));});_tmp284;});
_tmp11B[59]=({struct _tuple14*_tmp27E=_region_malloc(r,sizeof(*_tmp27E));_tmp27E->f1=({
const char*_tmp283="-detailedlocation";_tag_dynforward(_tmp283,sizeof(char),
_get_zero_arr_size(_tmp283,18));});_tmp27E->f2=0;_tmp27E->f3=({const char*_tmp282="";
_tag_dynforward(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,1));});_tmp27E->f4=(
void*)({struct Cyc_Arg_Clear_spec_struct*_tmp280=_region_malloc(r,sizeof(*_tmp280));
_tmp280[0]=({struct Cyc_Arg_Clear_spec_struct _tmp281;_tmp281.tag=4;_tmp281.f1=&
Cyc_Position_use_gcc_style_location;_tmp281;});_tmp280;});_tmp27E->f5=({const
char*_tmp27F="Try to give more detailed location information for errors";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,58));});_tmp27E;});
_tmp11B[58]=({struct _tuple14*_tmp278=_region_malloc(r,sizeof(*_tmp278));_tmp278->f1=({
const char*_tmp27D="-noregister";_tag_dynforward(_tmp27D,sizeof(char),
_get_zero_arr_size(_tmp27D,12));});_tmp278->f2=0;_tmp278->f3=({const char*_tmp27C="";
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,1));});_tmp278->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp27A=_region_malloc(r,sizeof(*_tmp27A));
_tmp27A[0]=({struct Cyc_Arg_Set_spec_struct _tmp27B;_tmp27B.tag=3;_tmp27B.f1=& Cyc_Parse_no_register;
_tmp27B;});_tmp27A;});_tmp278->f5=({const char*_tmp279="Treat register storage class as public";
_tag_dynforward(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,39));});_tmp278;});
_tmp11B[57]=({struct _tuple14*_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272->f1=({
const char*_tmp277="-warnnullchecks";_tag_dynforward(_tmp277,sizeof(char),
_get_zero_arr_size(_tmp277,16));});_tmp272->f2=0;_tmp272->f3=({const char*_tmp276="";
_tag_dynforward(_tmp276,sizeof(char),_get_zero_arr_size(_tmp276,1));});_tmp272->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp274=_region_malloc(r,sizeof(*_tmp274));
_tmp274[0]=({struct Cyc_Arg_Set_spec_struct _tmp275;_tmp275.tag=3;_tmp275.f1=& Cyc_Toc_warn_all_null_deref;
_tmp275;});_tmp274;});_tmp272->f5=({const char*_tmp273="Warn when any null check can't be eliminated";
_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,45));});_tmp272;});
_tmp11B[56]=({struct _tuple14*_tmp26C=_region_malloc(r,sizeof(*_tmp26C));_tmp26C->f1=({
const char*_tmp271="-warnboundschecks";_tag_dynforward(_tmp271,sizeof(char),
_get_zero_arr_size(_tmp271,18));});_tmp26C->f2=0;_tmp26C->f3=({const char*_tmp270="";
_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,1));});_tmp26C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp26E=_region_malloc(r,sizeof(*_tmp26E));
_tmp26E[0]=({struct Cyc_Arg_Set_spec_struct _tmp26F;_tmp26F.tag=3;_tmp26F.f1=& Cyc_Toc_warn_bounds_checks;
_tmp26F;});_tmp26E;});_tmp26C->f5=({const char*_tmp26D="Warn when bounds checks can't be eliminated";
_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,44));});_tmp26C;});
_tmp11B[55]=({struct _tuple14*_tmp266=_region_malloc(r,sizeof(*_tmp266));_tmp266->f1=({
const char*_tmp26B="-CI";_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(
_tmp26B,4));});_tmp266->f2=0;_tmp266->f3=({const char*_tmp26A=" <file>";
_tag_dynforward(_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,8));});_tmp266->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp268=_region_malloc(r,sizeof(*
_tmp268));_tmp268[0]=({struct Cyc_Arg_String_spec_struct _tmp269;_tmp269.tag=5;
_tmp269.f1=Cyc_set_cyc_include;_tmp269;});_tmp268;});_tmp266->f5=({const char*
_tmp267="Set cyc_include.h to be <file>";_tag_dynforward(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,31));});_tmp266;});_tmp11B[54]=({struct _tuple14*
_tmp260=_region_malloc(r,sizeof(*_tmp260));_tmp260->f1=({const char*_tmp265="-printalleffects";
_tag_dynforward(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,17));});_tmp260->f2=
0;_tmp260->f3=({const char*_tmp264="";_tag_dynforward(_tmp264,sizeof(char),
_get_zero_arr_size(_tmp264,1));});_tmp260->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp262=_region_malloc(r,sizeof(*_tmp262));_tmp262[0]=({struct Cyc_Arg_Set_spec_struct
_tmp263;_tmp263.tag=3;_tmp263.f1=& Cyc_print_all_effects_r;_tmp263;});_tmp262;});
_tmp260->f5=({const char*_tmp261="Print effects for functions (type debugging)";
_tag_dynforward(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,45));});_tmp260;});
_tmp11B[53]=({struct _tuple14*_tmp25A=_region_malloc(r,sizeof(*_tmp25A));_tmp25A->f1=({
const char*_tmp25F="-printfullevars";_tag_dynforward(_tmp25F,sizeof(char),
_get_zero_arr_size(_tmp25F,16));});_tmp25A->f2=0;_tmp25A->f3=({const char*_tmp25E="";
_tag_dynforward(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,1));});_tmp25A->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp25C=_region_malloc(r,sizeof(*_tmp25C));
_tmp25C[0]=({struct Cyc_Arg_Set_spec_struct _tmp25D;_tmp25D.tag=3;_tmp25D.f1=& Cyc_print_full_evars_r;
_tmp25D;});_tmp25C;});_tmp25A->f5=({const char*_tmp25B="Print full information for evars (type debugging)";
_tag_dynforward(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,50));});_tmp25A;});
_tmp11B[52]=({struct _tuple14*_tmp254=_region_malloc(r,sizeof(*_tmp254));_tmp254->f1=({
const char*_tmp259="-printallkinds";_tag_dynforward(_tmp259,sizeof(char),
_get_zero_arr_size(_tmp259,15));});_tmp254->f2=0;_tmp254->f3=({const char*_tmp258="";
_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,1));});_tmp254->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp256=_region_malloc(r,sizeof(*_tmp256));
_tmp256[0]=({struct Cyc_Arg_Set_spec_struct _tmp257;_tmp257.tag=3;_tmp257.f1=& Cyc_print_all_kinds_r;
_tmp257;});_tmp256;});_tmp254->f5=({const char*_tmp255="Always print kinds of type variables";
_tag_dynforward(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,37));});_tmp254;});
_tmp11B[51]=({struct _tuple14*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E->f1=({
const char*_tmp253="-printalltvars";_tag_dynforward(_tmp253,sizeof(char),
_get_zero_arr_size(_tmp253,15));});_tmp24E->f2=0;_tmp24E->f3=({const char*_tmp252="";
_tag_dynforward(_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,1));});_tmp24E->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp250=_region_malloc(r,sizeof(*_tmp250));
_tmp250[0]=({struct Cyc_Arg_Set_spec_struct _tmp251;_tmp251.tag=3;_tmp251.f1=& Cyc_print_all_tvars_r;
_tmp251;});_tmp250;});_tmp24E->f5=({const char*_tmp24F="Print all type variables (even implicit default effects)";
_tag_dynforward(_tmp24F,sizeof(char),_get_zero_arr_size(_tmp24F,57));});_tmp24E;});
_tmp11B[50]=({struct _tuple14*_tmp248=_region_malloc(r,sizeof(*_tmp248));_tmp248->f1=({
const char*_tmp24D="-noexpandtypedefs";_tag_dynforward(_tmp24D,sizeof(char),
_get_zero_arr_size(_tmp24D,18));});_tmp248->f2=0;_tmp248->f3=({const char*_tmp24C="";
_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,1));});_tmp248->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp24A=_region_malloc(r,sizeof(*_tmp24A));
_tmp24A[0]=({struct Cyc_Arg_Set_spec_struct _tmp24B;_tmp24B.tag=3;_tmp24B.f1=& Cyc_noexpand_r;
_tmp24B;});_tmp24A;});_tmp248->f5=({const char*_tmp249="Don't expand typedefs in pretty printing";
_tag_dynforward(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,41));});_tmp248;});
_tmp11B[49]=({struct _tuple14*_tmp242=_region_malloc(r,sizeof(*_tmp242));_tmp242->f1=({
const char*_tmp247="-noremoveunused";_tag_dynforward(_tmp247,sizeof(char),
_get_zero_arr_size(_tmp247,16));});_tmp242->f2=0;_tmp242->f3=({const char*_tmp246="";
_tag_dynforward(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,1));});_tmp242->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp244=_region_malloc(r,sizeof(*_tmp244));
_tmp244[0]=({struct Cyc_Arg_Set_spec_struct _tmp245;_tmp245.tag=3;_tmp245.f1=& Cyc_noshake_r;
_tmp245;});_tmp244;});_tmp242->f5=({const char*_tmp243="Don't remove externed variables that aren't used";
_tag_dynforward(_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,49));});_tmp242;});
_tmp11B[48]=({struct _tuple14*_tmp23C=_region_malloc(r,sizeof(*_tmp23C));_tmp23C->f1=({
const char*_tmp241="-nogc";_tag_dynforward(_tmp241,sizeof(char),
_get_zero_arr_size(_tmp241,6));});_tmp23C->f2=0;_tmp23C->f3=({const char*_tmp240="";
_tag_dynforward(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,1));});_tmp23C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp23E=_region_malloc(r,sizeof(*_tmp23E));
_tmp23E[0]=({struct Cyc_Arg_Set_spec_struct _tmp23F;_tmp23F.tag=3;_tmp23F.f1=& Cyc_nogc_r;
_tmp23F;});_tmp23E;});_tmp23C->f5=({const char*_tmp23D="Don't link in the garbage collector";
_tag_dynforward(_tmp23D,sizeof(char),_get_zero_arr_size(_tmp23D,36));});_tmp23C;});
_tmp11B[47]=({struct _tuple14*_tmp236=_region_malloc(r,sizeof(*_tmp236));_tmp236->f1=({
const char*_tmp23B="-nocyc";_tag_dynforward(_tmp23B,sizeof(char),
_get_zero_arr_size(_tmp23B,7));});_tmp236->f2=0;_tmp236->f3=({const char*_tmp23A="";
_tag_dynforward(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,1));});_tmp236->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp238=_region_malloc(r,sizeof(*_tmp238));
_tmp238[0]=({struct Cyc_Arg_Unit_spec_struct _tmp239;_tmp239.tag=0;_tmp239.f1=Cyc_set_nocyc;
_tmp239;});_tmp238;});_tmp236->f5=({const char*_tmp237="Don't add implicit namespace Cyc";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,33));});_tmp236;});
_tmp11B[46]=({struct _tuple14*_tmp230=_region_malloc(r,sizeof(*_tmp230));_tmp230->f1=({
const char*_tmp235="-no-cpp-precomp";_tag_dynforward(_tmp235,sizeof(char),
_get_zero_arr_size(_tmp235,16));});_tmp230->f2=0;_tmp230->f3=({const char*_tmp234="";
_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,1));});_tmp230->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp232=_region_malloc(r,sizeof(*_tmp232));
_tmp232[0]=({struct Cyc_Arg_Unit_spec_struct _tmp233;_tmp233.tag=0;_tmp233.f1=Cyc_set_nocppprecomp;
_tmp233;});_tmp232;});_tmp230->f5=({const char*_tmp231="Don't do smart preprocessing (mac only)";
_tag_dynforward(_tmp231,sizeof(char),_get_zero_arr_size(_tmp231,40));});_tmp230;});
_tmp11B[45]=({struct _tuple14*_tmp22A=_region_malloc(r,sizeof(*_tmp22A));_tmp22A->f1=({
const char*_tmp22F="-use-cpp";_tag_dynforward(_tmp22F,sizeof(char),
_get_zero_arr_size(_tmp22F,9));});_tmp22A->f2=0;_tmp22A->f3=({const char*_tmp22E="<path>";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,7));});_tmp22A->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp22C=_region_malloc(r,sizeof(*
_tmp22C));_tmp22C[0]=({struct Cyc_Arg_String_spec_struct _tmp22D;_tmp22D.tag=5;
_tmp22D.f1=Cyc_set_cpp;_tmp22D;});_tmp22C;});_tmp22A->f5=({const char*_tmp22B="Indicate which preprocessor to use";
_tag_dynforward(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,35));});_tmp22A;});
_tmp11B[44]=({struct _tuple14*_tmp224=_region_malloc(r,sizeof(*_tmp224));_tmp224->f1=({
const char*_tmp229="--inline-checks";_tag_dynforward(_tmp229,sizeof(char),
_get_zero_arr_size(_tmp229,16));});_tmp224->f2=0;_tmp224->f3=({const char*_tmp228="";
_tag_dynforward(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,1));});_tmp224->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp226=_region_malloc(r,sizeof(*_tmp226));
_tmp226[0]=({struct Cyc_Arg_Unit_spec_struct _tmp227;_tmp227.tag=0;_tmp227.f1=Cyc_set_inline_functions;
_tmp227;});_tmp226;});_tmp224->f5=({const char*_tmp225="Inline bounds checks instead of #define";
_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,40));});_tmp224;});
_tmp11B[43]=({struct _tuple14*_tmp21E=_region_malloc(r,sizeof(*_tmp21E));_tmp21E->f1=({
const char*_tmp223="--noboundschecks";_tag_dynforward(_tmp223,sizeof(char),
_get_zero_arr_size(_tmp223,17));});_tmp21E->f2=0;_tmp21E->f3=({const char*_tmp222="";
_tag_dynforward(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,1));});_tmp21E->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp220=_region_malloc(r,sizeof(*_tmp220));
_tmp220[0]=({struct Cyc_Arg_Unit_spec_struct _tmp221;_tmp221.tag=0;_tmp221.f1=Cyc_set_noboundschecks;
_tmp221;});_tmp220;});_tmp21E->f5=({const char*_tmp21F="Disable bounds checks";
_tag_dynforward(_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,22));});_tmp21E;});
_tmp11B[42]=({struct _tuple14*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->f1=({
const char*_tmp21D="--nonullchecks";_tag_dynforward(_tmp21D,sizeof(char),
_get_zero_arr_size(_tmp21D,15));});_tmp218->f2=0;_tmp218->f3=({const char*_tmp21C="";
_tag_dynforward(_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,1));});_tmp218->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp21A=_region_malloc(r,sizeof(*_tmp21A));
_tmp21A[0]=({struct Cyc_Arg_Unit_spec_struct _tmp21B;_tmp21B.tag=0;_tmp21B.f1=Cyc_set_nonullchecks;
_tmp21B;});_tmp21A;});_tmp218->f5=({const char*_tmp219="Disable null checks";
_tag_dynforward(_tmp219,sizeof(char),_get_zero_arr_size(_tmp219,20));});_tmp218;});
_tmp11B[41]=({struct _tuple14*_tmp212=_region_malloc(r,sizeof(*_tmp212));_tmp212->f1=({
const char*_tmp217="--nochecks";_tag_dynforward(_tmp217,sizeof(char),
_get_zero_arr_size(_tmp217,11));});_tmp212->f2=0;_tmp212->f3=({const char*_tmp216="";
_tag_dynforward(_tmp216,sizeof(char),_get_zero_arr_size(_tmp216,1));});_tmp212->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp214=_region_malloc(r,sizeof(*_tmp214));
_tmp214[0]=({struct Cyc_Arg_Unit_spec_struct _tmp215;_tmp215.tag=0;_tmp215.f1=Cyc_set_nochecks;
_tmp215;});_tmp214;});_tmp212->f5=({const char*_tmp213="Disable bounds/null checks";
_tag_dynforward(_tmp213,sizeof(char),_get_zero_arr_size(_tmp213,27));});_tmp212;});
_tmp11B[40]=({struct _tuple14*_tmp20C=_region_malloc(r,sizeof(*_tmp20C));_tmp20C->f1=({
const char*_tmp211="--lineno";_tag_dynforward(_tmp211,sizeof(char),
_get_zero_arr_size(_tmp211,9));});_tmp20C->f2=0;_tmp20C->f3=({const char*_tmp210="";
_tag_dynforward(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,1));});_tmp20C->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp20E=_region_malloc(r,sizeof(*_tmp20E));
_tmp20E[0]=({struct Cyc_Arg_Unit_spec_struct _tmp20F;_tmp20F.tag=0;_tmp20F.f1=Cyc_set_lineno;
_tmp20F;});_tmp20E;});_tmp20C->f5=({const char*_tmp20D="Generate line numbers for debugging";
_tag_dynforward(_tmp20D,sizeof(char),_get_zero_arr_size(_tmp20D,36));});_tmp20C;});
_tmp11B[39]=({struct _tuple14*_tmp206=_region_malloc(r,sizeof(*_tmp206));_tmp206->f1=({
const char*_tmp20B="-save-c";_tag_dynforward(_tmp20B,sizeof(char),
_get_zero_arr_size(_tmp20B,8));});_tmp206->f2=0;_tmp206->f3=({const char*_tmp20A="";
_tag_dynforward(_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,1));});_tmp206->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp208=_region_malloc(r,sizeof(*_tmp208));
_tmp208[0]=({struct Cyc_Arg_Set_spec_struct _tmp209;_tmp209.tag=3;_tmp209.f1=& Cyc_save_c_r;
_tmp209;});_tmp208;});_tmp206->f5=({const char*_tmp207="Don't delete temporary C files";
_tag_dynforward(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,31));});_tmp206;});
_tmp11B[38]=({struct _tuple14*_tmp200=_region_malloc(r,sizeof(*_tmp200));_tmp200->f1=({
const char*_tmp205="-save-temps";_tag_dynforward(_tmp205,sizeof(char),
_get_zero_arr_size(_tmp205,12));});_tmp200->f2=0;_tmp200->f3=({const char*_tmp204="";
_tag_dynforward(_tmp204,sizeof(char),_get_zero_arr_size(_tmp204,1));});_tmp200->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp202=_region_malloc(r,sizeof(*_tmp202));
_tmp202[0]=({struct Cyc_Arg_Unit_spec_struct _tmp203;_tmp203.tag=0;_tmp203.f1=Cyc_set_save_temps;
_tmp203;});_tmp202;});_tmp200->f5=({const char*_tmp201="Don't delete temporary files";
_tag_dynforward(_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,29));});_tmp200;});
_tmp11B[37]=({struct _tuple14*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));_tmp1FA->f1=({
const char*_tmp1FF="-tovc";_tag_dynforward(_tmp1FF,sizeof(char),
_get_zero_arr_size(_tmp1FF,6));});_tmp1FA->f2=0;_tmp1FA->f3=({const char*_tmp1FE="";
_tag_dynforward(_tmp1FE,sizeof(char),_get_zero_arr_size(_tmp1FE,1));});_tmp1FA->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));
_tmp1FC[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1FD;_tmp1FD.tag=0;_tmp1FD.f1=Cyc_set_tovc;
_tmp1FD;});_tmp1FC;});_tmp1FA->f5=({const char*_tmp1FB="Avoid gcc extensions in C output";
_tag_dynforward(_tmp1FB,sizeof(char),_get_zero_arr_size(_tmp1FB,33));});_tmp1FA;});
_tmp11B[36]=({struct _tuple14*_tmp1F4=_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4->f1=({
const char*_tmp1F9="-elim-statement-expressions";_tag_dynforward(_tmp1F9,sizeof(
char),_get_zero_arr_size(_tmp1F9,28));});_tmp1F4->f2=0;_tmp1F4->f3=({const char*
_tmp1F8="";_tag_dynforward(_tmp1F8,sizeof(char),_get_zero_arr_size(_tmp1F8,1));});
_tmp1F4->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp1F6=_region_malloc(r,
sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Arg_Set_spec_struct _tmp1F7;_tmp1F7.tag=
3;_tmp1F7.f1=& Cyc_elim_se_r;_tmp1F7;});_tmp1F6;});_tmp1F4->f5=({const char*
_tmp1F5="Avoid statement expressions in C output";_tag_dynforward(_tmp1F5,
sizeof(char),_get_zero_arr_size(_tmp1F5,40));});_tmp1F4;});_tmp11B[35]=({struct
_tuple14*_tmp1EE=_region_malloc(r,sizeof(*_tmp1EE));_tmp1EE->f1=({const char*
_tmp1F3="-up";_tag_dynforward(_tmp1F3,sizeof(char),_get_zero_arr_size(_tmp1F3,4));});
_tmp1EE->f2=0;_tmp1EE->f3=({const char*_tmp1F2="";_tag_dynforward(_tmp1F2,sizeof(
char),_get_zero_arr_size(_tmp1F2,1));});_tmp1EE->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp1F1;_tmp1F1.tag=4;_tmp1F1.f1=& Cyc_pp_r;_tmp1F1;});_tmp1F0;});_tmp1EE->f5=({
const char*_tmp1EF="Ugly print";_tag_dynforward(_tmp1EF,sizeof(char),
_get_zero_arr_size(_tmp1EF,11));});_tmp1EE;});_tmp11B[34]=({struct _tuple14*
_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));_tmp1E8->f1=({const char*_tmp1ED="-pp";
_tag_dynforward(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,4));});_tmp1E8->f2=
0;_tmp1E8->f3=({const char*_tmp1EC="";_tag_dynforward(_tmp1EC,sizeof(char),
_get_zero_arr_size(_tmp1EC,1));});_tmp1E8->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp1EA=_region_malloc(r,sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1EB;_tmp1EB.tag=3;_tmp1EB.f1=& Cyc_pp_r;_tmp1EB;});_tmp1EA;});_tmp1E8->f5=({
const char*_tmp1E9="Pretty print";_tag_dynforward(_tmp1E9,sizeof(char),
_get_zero_arr_size(_tmp1E9,13));});_tmp1E8;});_tmp11B[33]=({struct _tuple14*
_tmp1E2=_region_malloc(r,sizeof(*_tmp1E2));_tmp1E2->f1=({const char*_tmp1E7="-ic";
_tag_dynforward(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,4));});_tmp1E2->f2=
0;_tmp1E2->f3=({const char*_tmp1E6="";_tag_dynforward(_tmp1E6,sizeof(char),
_get_zero_arr_size(_tmp1E6,1));});_tmp1E2->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp1E4=_region_malloc(r,sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1E5;_tmp1E5.tag=3;_tmp1E5.f1=& Cyc_ic_r;_tmp1E5;});_tmp1E4;});_tmp1E2->f5=({
const char*_tmp1E3="Activate the link-checker";_tag_dynforward(_tmp1E3,sizeof(
char),_get_zero_arr_size(_tmp1E3,26));});_tmp1E2;});_tmp11B[32]=({struct _tuple14*
_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));_tmp1DC->f1=({const char*_tmp1E1="-stopafter-toc";
_tag_dynforward(_tmp1E1,sizeof(char),_get_zero_arr_size(_tmp1E1,15));});_tmp1DC->f2=
0;_tmp1DC->f3=({const char*_tmp1E0="";_tag_dynforward(_tmp1E0,sizeof(char),
_get_zero_arr_size(_tmp1E0,1));});_tmp1DC->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1DF;_tmp1DF.tag=3;_tmp1DF.f1=& Cyc_toc_r;_tmp1DF;});_tmp1DE;});_tmp1DC->f5=({
const char*_tmp1DD="Stop after translation to C";_tag_dynforward(_tmp1DD,sizeof(
char),_get_zero_arr_size(_tmp1DD,28));});_tmp1DC;});_tmp11B[31]=({struct _tuple14*
_tmp1D6=_region_malloc(r,sizeof(*_tmp1D6));_tmp1D6->f1=({const char*_tmp1DB="-stopafter-cf";
_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,14));});_tmp1D6->f2=
0;_tmp1D6->f3=({const char*_tmp1DA="";_tag_dynforward(_tmp1DA,sizeof(char),
_get_zero_arr_size(_tmp1DA,1));});_tmp1D6->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp1D8=_region_malloc(r,sizeof(*_tmp1D8));_tmp1D8[0]=({struct Cyc_Arg_Set_spec_struct
_tmp1D9;_tmp1D9.tag=3;_tmp1D9.f1=& Cyc_cf_r;_tmp1D9;});_tmp1D8;});_tmp1D6->f5=({
const char*_tmp1D7="Stop after control-flow checking";_tag_dynforward(_tmp1D7,
sizeof(char),_get_zero_arr_size(_tmp1D7,33));});_tmp1D6;});_tmp11B[30]=({struct
_tuple14*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));_tmp1D0->f1=({const char*
_tmp1D5="-noprint";_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(
_tmp1D5,9));});_tmp1D0->f2=0;_tmp1D0->f3=({const char*_tmp1D4="";_tag_dynforward(
_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,1));});_tmp1D0->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp1D2=_region_malloc(r,sizeof(*_tmp1D2));_tmp1D2[
0]=({struct Cyc_Arg_Set_spec_struct _tmp1D3;_tmp1D3.tag=3;_tmp1D3.f1=& Cyc_noprint_r;
_tmp1D3;});_tmp1D2;});_tmp1D0->f5=({const char*_tmp1D1="Do not print output (when stopping early)";
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(_tmp1D1,42));});_tmp1D0;});
_tmp11B[29]=({struct _tuple14*_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));_tmp1CA->f1=({
const char*_tmp1CF="-stopafter-tc";_tag_dynforward(_tmp1CF,sizeof(char),
_get_zero_arr_size(_tmp1CF,14));});_tmp1CA->f2=0;_tmp1CA->f3=({const char*_tmp1CE="";
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,1));});_tmp1CA->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp1CC=_region_malloc(r,sizeof(*_tmp1CC));
_tmp1CC[0]=({struct Cyc_Arg_Set_spec_struct _tmp1CD;_tmp1CD.tag=3;_tmp1CD.f1=& Cyc_tc_r;
_tmp1CD;});_tmp1CC;});_tmp1CA->f5=({const char*_tmp1CB="Stop after type checking";
_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size(_tmp1CB,25));});_tmp1CA;});
_tmp11B[28]=({struct _tuple14*_tmp1C4=_region_malloc(r,sizeof(*_tmp1C4));_tmp1C4->f1=({
const char*_tmp1C9="-stopafter-parse";_tag_dynforward(_tmp1C9,sizeof(char),
_get_zero_arr_size(_tmp1C9,17));});_tmp1C4->f2=0;_tmp1C4->f3=({const char*_tmp1C8="";
_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,1));});_tmp1C4->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp1C6=_region_malloc(r,sizeof(*_tmp1C6));
_tmp1C6[0]=({struct Cyc_Arg_Set_spec_struct _tmp1C7;_tmp1C7.tag=3;_tmp1C7.f1=& Cyc_parseonly_r;
_tmp1C7;});_tmp1C6;});_tmp1C4->f5=({const char*_tmp1C5="Stop after parsing";
_tag_dynforward(_tmp1C5,sizeof(char),_get_zero_arr_size(_tmp1C5,19));});_tmp1C4;});
_tmp11B[27]=({struct _tuple14*_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE->f1=({
const char*_tmp1C3="-E";_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(
_tmp1C3,3));});_tmp1BE->f2=0;_tmp1BE->f3=({const char*_tmp1C2="";_tag_dynforward(
_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,1));});_tmp1BE->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp1C0=_region_malloc(r,sizeof(*_tmp1C0));_tmp1C0[
0]=({struct Cyc_Arg_Set_spec_struct _tmp1C1;_tmp1C1.tag=3;_tmp1C1.f1=& Cyc_stop_after_cpp_r;
_tmp1C1;});_tmp1C0;});_tmp1BE->f5=({const char*_tmp1BF="Stop after preprocessing";
_tag_dynforward(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,25));});_tmp1BE;});
_tmp11B[26]=({struct _tuple14*_tmp1B8=_region_malloc(r,sizeof(*_tmp1B8));_tmp1B8->f1=({
const char*_tmp1BD="-Wall";_tag_dynforward(_tmp1BD,sizeof(char),
_get_zero_arr_size(_tmp1BD,6));});_tmp1B8->f2=0;_tmp1B8->f3=({const char*_tmp1BC="";
_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(_tmp1BC,1));});_tmp1B8->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp1BA=_region_malloc(r,sizeof(*_tmp1BA));
_tmp1BA[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=Cyc_set_all_warnings;
_tmp1BB;});_tmp1BA;});_tmp1B8->f5=({const char*_tmp1B9="Turn on all warnings";
_tag_dynforward(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,21));});_tmp1B8;});
_tmp11B[25]=({struct _tuple14*_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2->f1=({
const char*_tmp1B7="-b";_tag_dynforward(_tmp1B7,sizeof(char),_get_zero_arr_size(
_tmp1B7,3));});_tmp1B2->f2=0;_tmp1B2->f3=({const char*_tmp1B6="<arch>";
_tag_dynforward(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,7));});_tmp1B2->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1B4=_region_malloc(r,sizeof(*
_tmp1B4));_tmp1B4[0]=({struct Cyc_Arg_String_spec_struct _tmp1B5;_tmp1B5.tag=5;
_tmp1B5.f1=Cyc_set_target_arch;_tmp1B5;});_tmp1B4;});_tmp1B2->f5=({const char*
_tmp1B3="Set target architecture; implies -stopafter-toc";_tag_dynforward(
_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,48));});_tmp1B2;});_tmp11B[24]=({
struct _tuple14*_tmp1AC=_region_malloc(r,sizeof(*_tmp1AC));_tmp1AC->f1=({const
char*_tmp1B1="-MT";_tag_dynforward(_tmp1B1,sizeof(char),_get_zero_arr_size(
_tmp1B1,4));});_tmp1AC->f2=0;_tmp1AC->f3=({const char*_tmp1B0=" <target>";
_tag_dynforward(_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,10));});_tmp1AC->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1AE=_region_malloc(r,sizeof(*
_tmp1AE));_tmp1AE[0]=({struct Cyc_Arg_String_spec_struct _tmp1AF;_tmp1AF.tag=5;
_tmp1AF.f1=Cyc_set_dependencies_target;_tmp1AF;});_tmp1AE;});_tmp1AC->f5=({const
char*_tmp1AD="Give target for dependencies";_tag_dynforward(_tmp1AD,sizeof(char),
_get_zero_arr_size(_tmp1AD,29));});_tmp1AC;});_tmp11B[23]=({struct _tuple14*
_tmp1A6=_region_malloc(r,sizeof(*_tmp1A6));_tmp1A6->f1=({const char*_tmp1AB="-MG";
_tag_dynforward(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,4));});_tmp1A6->f2=
0;_tmp1A6->f3=({const char*_tmp1AA="";_tag_dynforward(_tmp1AA,sizeof(char),
_get_zero_arr_size(_tmp1AA,1));});_tmp1A6->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1A8=_region_malloc(r,sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1A9;_tmp1A9.tag=1;_tmp1A9.f1=Cyc_add_cpparg;_tmp1A9;});_tmp1A8;});_tmp1A6->f5=({
const char*_tmp1A7="When producing dependencies assume that missing files are generated";
_tag_dynforward(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,68));});_tmp1A6;});
_tmp11B[22]=({struct _tuple14*_tmp1A0=_region_malloc(r,sizeof(*_tmp1A0));_tmp1A0->f1=({
const char*_tmp1A5="-M";_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size(
_tmp1A5,3));});_tmp1A0->f2=0;_tmp1A0->f3=({const char*_tmp1A4="";_tag_dynforward(
_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,1));});_tmp1A0->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1A2=_region_malloc(r,sizeof(*_tmp1A2));
_tmp1A2[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=Cyc_set_produce_dependencies;
_tmp1A3;});_tmp1A2;});_tmp1A0->f5=({const char*_tmp1A1="Produce dependencies";
_tag_dynforward(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,21));});_tmp1A0;});
_tmp11B[21]=({struct _tuple14*_tmp19A=_region_malloc(r,sizeof(*_tmp19A));_tmp19A->f1=({
const char*_tmp19F="-S";_tag_dynforward(_tmp19F,sizeof(char),_get_zero_arr_size(
_tmp19F,3));});_tmp19A->f2=0;_tmp19A->f3=({const char*_tmp19E="";_tag_dynforward(
_tmp19E,sizeof(char),_get_zero_arr_size(_tmp19E,1));});_tmp19A->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp19C=_region_malloc(r,sizeof(*_tmp19C));
_tmp19C[0]=({struct Cyc_Arg_Unit_spec_struct _tmp19D;_tmp19D.tag=0;_tmp19D.f1=Cyc_set_stop_after_asmfile;
_tmp19D;});_tmp19C;});_tmp19A->f5=({const char*_tmp19B="Stop after producing assembly code";
_tag_dynforward(_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,35));});_tmp19A;});
_tmp11B[20]=({struct _tuple14*_tmp194=_region_malloc(r,sizeof(*_tmp194));_tmp194->f1=({
const char*_tmp199="-pa";_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size(
_tmp199,4));});_tmp194->f2=0;_tmp194->f3=({const char*_tmp198="";_tag_dynforward(
_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,1));});_tmp194->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp196=_region_malloc(r,sizeof(*_tmp196));
_tmp196[0]=({struct Cyc_Arg_Unit_spec_struct _tmp197;_tmp197.tag=0;_tmp197.f1=Cyc_set_pa;
_tmp197;});_tmp196;});_tmp194->f5=({const char*_tmp195="Compile for profiling with aprof";
_tag_dynforward(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,33));});_tmp194;});
_tmp11B[19]=({struct _tuple14*_tmp18E=_region_malloc(r,sizeof(*_tmp18E));_tmp18E->f1=({
const char*_tmp193="-pg";_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(
_tmp193,4));});_tmp18E->f2=0;_tmp18E->f3=({const char*_tmp192="";_tag_dynforward(
_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,1));});_tmp18E->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp190=_region_malloc(r,sizeof(*_tmp190));
_tmp190[0]=({struct Cyc_Arg_Unit_spec_struct _tmp191;_tmp191.tag=0;_tmp191.f1=Cyc_set_pg;
_tmp191;});_tmp190;});_tmp18E->f5=({const char*_tmp18F="Compile for profiling with gprof";
_tag_dynforward(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,33));});_tmp18E;});
_tmp11B[18]=({struct _tuple14*_tmp188=_region_malloc(r,sizeof(*_tmp188));_tmp188->f1=({
const char*_tmp18D="-p";_tag_dynforward(_tmp18D,sizeof(char),_get_zero_arr_size(
_tmp18D,3));});_tmp188->f2=0;_tmp188->f3=({const char*_tmp18C="";_tag_dynforward(
_tmp18C,sizeof(char),_get_zero_arr_size(_tmp18C,1));});_tmp188->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp18A=_region_malloc(r,sizeof(*_tmp18A));
_tmp18A[0]=({struct Cyc_Arg_Flag_spec_struct _tmp18B;_tmp18B.tag=1;_tmp18B.f1=Cyc_add_ccarg;
_tmp18B;});_tmp18A;});_tmp188->f5=({const char*_tmp189="Compile for profiling with prof";
_tag_dynforward(_tmp189,sizeof(char),_get_zero_arr_size(_tmp189,32));});_tmp188;});
_tmp11B[17]=({struct _tuple14*_tmp182=_region_malloc(r,sizeof(*_tmp182));_tmp182->f1=({
const char*_tmp187="-g";_tag_dynforward(_tmp187,sizeof(char),_get_zero_arr_size(
_tmp187,3));});_tmp182->f2=0;_tmp182->f3=({const char*_tmp186="";_tag_dynforward(
_tmp186,sizeof(char),_get_zero_arr_size(_tmp186,1));});_tmp182->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp184=_region_malloc(r,sizeof(*_tmp184));
_tmp184[0]=({struct Cyc_Arg_Flag_spec_struct _tmp185;_tmp185.tag=1;_tmp185.f1=Cyc_add_ccarg;
_tmp185;});_tmp184;});_tmp182->f5=({const char*_tmp183="Compile for debugging";
_tag_dynforward(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,22));});_tmp182;});
_tmp11B[16]=({struct _tuple14*_tmp17C=_region_malloc(r,sizeof(*_tmp17C));_tmp17C->f1=({
const char*_tmp181="-fomit-frame-pointer";_tag_dynforward(_tmp181,sizeof(char),
_get_zero_arr_size(_tmp181,21));});_tmp17C->f2=0;_tmp17C->f3=({const char*_tmp180="";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,1));});_tmp17C->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp17E=_region_malloc(r,sizeof(*_tmp17E));
_tmp17E[0]=({struct Cyc_Arg_Flag_spec_struct _tmp17F;_tmp17F.tag=1;_tmp17F.f1=Cyc_add_ccarg;
_tmp17F;});_tmp17E;});_tmp17C->f5=({const char*_tmp17D="Omit frame pointer";
_tag_dynforward(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,19));});_tmp17C;});
_tmp11B[15]=({struct _tuple14*_tmp176=_region_malloc(r,sizeof(*_tmp176));_tmp176->f1=({
const char*_tmp17B="-O3";_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size(
_tmp17B,4));});_tmp176->f2=0;_tmp176->f3=({const char*_tmp17A="";_tag_dynforward(
_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,1));});_tmp176->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp178=_region_malloc(r,sizeof(*_tmp178));
_tmp178[0]=({struct Cyc_Arg_Flag_spec_struct _tmp179;_tmp179.tag=1;_tmp179.f1=Cyc_add_ccarg;
_tmp179;});_tmp178;});_tmp176->f5=({const char*_tmp177="Even more optimization";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,23));});_tmp176;});
_tmp11B[14]=({struct _tuple14*_tmp170=_region_malloc(r,sizeof(*_tmp170));_tmp170->f1=({
const char*_tmp175="-O2";_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(
_tmp175,4));});_tmp170->f2=0;_tmp170->f3=({const char*_tmp174="";_tag_dynforward(
_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,1));});_tmp170->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp172=_region_malloc(r,sizeof(*_tmp172));
_tmp172[0]=({struct Cyc_Arg_Flag_spec_struct _tmp173;_tmp173.tag=1;_tmp173.f1=Cyc_add_ccarg;
_tmp173;});_tmp172;});_tmp170->f5=({const char*_tmp171="A higher level of optimization";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,31));});_tmp170;});
_tmp11B[13]=({struct _tuple14*_tmp16A=_region_malloc(r,sizeof(*_tmp16A));_tmp16A->f1=({
const char*_tmp16F="-O";_tag_dynforward(_tmp16F,sizeof(char),_get_zero_arr_size(
_tmp16F,3));});_tmp16A->f2=0;_tmp16A->f3=({const char*_tmp16E="";_tag_dynforward(
_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,1));});_tmp16A->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp16C=_region_malloc(r,sizeof(*_tmp16C));
_tmp16C[0]=({struct Cyc_Arg_Flag_spec_struct _tmp16D;_tmp16D.tag=1;_tmp16D.f1=Cyc_add_ccarg;
_tmp16D;});_tmp16C;});_tmp16A->f5=({const char*_tmp16B="Optimize";_tag_dynforward(
_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,9));});_tmp16A;});_tmp11B[12]=({
struct _tuple14*_tmp164=_region_malloc(r,sizeof(*_tmp164));_tmp164->f1=({const
char*_tmp169="-O0";_tag_dynforward(_tmp169,sizeof(char),_get_zero_arr_size(
_tmp169,4));});_tmp164->f2=0;_tmp164->f3=({const char*_tmp168="";_tag_dynforward(
_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,1));});_tmp164->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp166=_region_malloc(r,sizeof(*_tmp166));
_tmp166[0]=({struct Cyc_Arg_Flag_spec_struct _tmp167;_tmp167.tag=1;_tmp167.f1=Cyc_add_ccarg;
_tmp167;});_tmp166;});_tmp164->f5=({const char*_tmp165="Don't optimize";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,15));});_tmp164;});
_tmp11B[11]=({struct _tuple14*_tmp15E=_region_malloc(r,sizeof(*_tmp15E));_tmp15E->f1=({
const char*_tmp163="-s";_tag_dynforward(_tmp163,sizeof(char),_get_zero_arr_size(
_tmp163,3));});_tmp15E->f2=0;_tmp15E->f3=({const char*_tmp162="";_tag_dynforward(
_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,1));});_tmp15E->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp160=_region_malloc(r,sizeof(*_tmp160));
_tmp160[0]=({struct Cyc_Arg_Flag_spec_struct _tmp161;_tmp161.tag=1;_tmp161.f1=Cyc_add_ccarg;
_tmp161;});_tmp160;});_tmp15E->f5=({const char*_tmp15F="Remove all symbol table and relocation info from executable";
_tag_dynforward(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,60));});_tmp15E;});
_tmp11B[10]=({struct _tuple14*_tmp158=_region_malloc(r,sizeof(*_tmp158));_tmp158->f1=({
const char*_tmp15D="-x";_tag_dynforward(_tmp15D,sizeof(char),_get_zero_arr_size(
_tmp15D,3));});_tmp158->f2=0;_tmp158->f3=({const char*_tmp15C=" <language>";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,12));});_tmp158->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp15A=_region_malloc(r,sizeof(*
_tmp15A));_tmp15A[0]=({struct Cyc_Arg_String_spec_struct _tmp15B;_tmp15B.tag=5;
_tmp15B.f1=Cyc_set_inputtype;_tmp15B;});_tmp15A;});_tmp158->f5=({const char*
_tmp159="Specify <language> for the following input files";_tag_dynforward(
_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,49));});_tmp158;});_tmp11B[9]=({
struct _tuple14*_tmp152=_region_malloc(r,sizeof(*_tmp152));_tmp152->f1=({const
char*_tmp157="-c";_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(
_tmp157,3));});_tmp152->f2=0;_tmp152->f3=({const char*_tmp156="";_tag_dynforward(
_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,1));});_tmp152->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp154=_region_malloc(r,sizeof(*_tmp154));
_tmp154[0]=({struct Cyc_Arg_Unit_spec_struct _tmp155;_tmp155.tag=0;_tmp155.f1=Cyc_set_stop_after_objectfile;
_tmp155;});_tmp154;});_tmp152->f5=({const char*_tmp153="Produce an object file instead of an executable; do not link";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,61));});_tmp152;});
_tmp11B[8]=({struct _tuple14*_tmp14C=_region_malloc(r,sizeof(*_tmp14C));_tmp14C->f1=({
const char*_tmp151="-m";_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(
_tmp151,3));});_tmp14C->f2=1;_tmp14C->f3=({const char*_tmp150="<option>";
_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,9));});_tmp14C->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp14E=_region_malloc(r,sizeof(*_tmp14E));
_tmp14E[0]=({struct Cyc_Arg_Flag_spec_struct _tmp14F;_tmp14F.tag=1;_tmp14F.f1=Cyc_add_marg;
_tmp14F;});_tmp14E;});_tmp14C->f5=({const char*_tmp14D="GCC specific: pass machine-dependent flag on to gcc";
_tag_dynforward(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,52));});_tmp14C;});
_tmp11B[7]=({struct _tuple14*_tmp146=_region_malloc(r,sizeof(*_tmp146));_tmp146->f1=({
const char*_tmp14B="-l";_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(
_tmp14B,3));});_tmp146->f2=1;_tmp146->f3=({const char*_tmp14A="<libname>";
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,10));});_tmp146->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp148=_region_malloc(r,sizeof(*_tmp148));
_tmp148[0]=({struct Cyc_Arg_Flag_spec_struct _tmp149;_tmp149.tag=1;_tmp149.f1=Cyc_add_libarg;
_tmp149;});_tmp148;});_tmp146->f5=({const char*_tmp147="Library file";
_tag_dynforward(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,13));});_tmp146;});
_tmp11B[6]=({struct _tuple14*_tmp140=_region_malloc(r,sizeof(*_tmp140));_tmp140->f1=({
const char*_tmp145="-L";_tag_dynforward(_tmp145,sizeof(char),_get_zero_arr_size(
_tmp145,3));});_tmp140->f2=1;_tmp140->f3=({const char*_tmp144="<dir>";
_tag_dynforward(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,6));});_tmp140->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp142=_region_malloc(r,sizeof(*_tmp142));
_tmp142[0]=({struct Cyc_Arg_Flag_spec_struct _tmp143;_tmp143.tag=1;_tmp143.f1=Cyc_add_ccarg;
_tmp143;});_tmp142;});_tmp140->f5=({const char*_tmp141="Add to the list of directories for -l";
_tag_dynforward(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,38));});_tmp140;});
_tmp11B[5]=({struct _tuple14*_tmp13A=_region_malloc(r,sizeof(*_tmp13A));_tmp13A->f1=({
const char*_tmp13F="-I";_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(
_tmp13F,3));});_tmp13A->f2=1;_tmp13A->f3=({const char*_tmp13E="<dir>";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,6));});_tmp13A->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp13C=_region_malloc(r,sizeof(*_tmp13C));
_tmp13C[0]=({struct Cyc_Arg_Flag_spec_struct _tmp13D;_tmp13D.tag=1;_tmp13D.f1=Cyc_add_cpparg;
_tmp13D;});_tmp13C;});_tmp13A->f5=({const char*_tmp13B="Add to the list of directories to search for include files";
_tag_dynforward(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,59));});_tmp13A;});
_tmp11B[4]=({struct _tuple14*_tmp134=_region_malloc(r,sizeof(*_tmp134));_tmp134->f1=({
const char*_tmp139="-B";_tag_dynforward(_tmp139,sizeof(char),_get_zero_arr_size(
_tmp139,3));});_tmp134->f2=1;_tmp134->f3=({const char*_tmp138="<file>";
_tag_dynforward(_tmp138,sizeof(char),_get_zero_arr_size(_tmp138,7));});_tmp134->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp136=_region_malloc(r,sizeof(*_tmp136));
_tmp136[0]=({struct Cyc_Arg_Flag_spec_struct _tmp137;_tmp137.tag=1;_tmp137.f1=Cyc_add_cyclone_exec_path;
_tmp137;});_tmp136;});_tmp134->f5=({const char*_tmp135="Add to the list of directories to search for compiler files";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,60));});_tmp134;});
_tmp11B[3]=({struct _tuple14*_tmp12E=_region_malloc(r,sizeof(*_tmp12E));_tmp12E->f1=({
const char*_tmp133="-D";_tag_dynforward(_tmp133,sizeof(char),_get_zero_arr_size(
_tmp133,3));});_tmp12E->f2=1;_tmp12E->f3=({const char*_tmp132="<name>[=<value>]";
_tag_dynforward(_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,17));});_tmp12E->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp130=_region_malloc(r,sizeof(*_tmp130));
_tmp130[0]=({struct Cyc_Arg_Flag_spec_struct _tmp131;_tmp131.tag=1;_tmp131.f1=Cyc_add_cpparg;
_tmp131;});_tmp130;});_tmp12E->f5=({const char*_tmp12F="Pass definition to preprocessor";
_tag_dynforward(_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,32));});_tmp12E;});
_tmp11B[2]=({struct _tuple14*_tmp128=_region_malloc(r,sizeof(*_tmp128));_tmp128->f1=({
const char*_tmp12D="-o";_tag_dynforward(_tmp12D,sizeof(char),_get_zero_arr_size(
_tmp12D,3));});_tmp128->f2=0;_tmp128->f3=({const char*_tmp12C=" <file>";
_tag_dynforward(_tmp12C,sizeof(char),_get_zero_arr_size(_tmp12C,8));});_tmp128->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp12A=_region_malloc(r,sizeof(*
_tmp12A));_tmp12A[0]=({struct Cyc_Arg_String_spec_struct _tmp12B;_tmp12B.tag=5;
_tmp12B.f1=Cyc_set_output_file;_tmp12B;});_tmp12A;});_tmp128->f5=({const char*
_tmp129="Set the output file name to <file>";_tag_dynforward(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,35));});_tmp128;});_tmp11B[1]=({struct _tuple14*
_tmp122=_region_malloc(r,sizeof(*_tmp122));_tmp122->f1=({const char*_tmp127="--version";
_tag_dynforward(_tmp127,sizeof(char),_get_zero_arr_size(_tmp127,10));});_tmp122->f2=
0;_tmp122->f3=({const char*_tmp126="";_tag_dynforward(_tmp126,sizeof(char),
_get_zero_arr_size(_tmp126,1));});_tmp122->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp124=_region_malloc(r,sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp125;_tmp125.tag=0;_tmp125.f1=Cyc_print_version;_tmp125;});_tmp124;});_tmp122->f5=({
const char*_tmp123="Print version information and exit";_tag_dynforward(_tmp123,
sizeof(char),_get_zero_arr_size(_tmp123,35));});_tmp122;});_tmp11B[0]=({struct
_tuple14*_tmp11C=_region_malloc(r,sizeof(*_tmp11C));_tmp11C->f1=({const char*
_tmp121="-v";_tag_dynforward(_tmp121,sizeof(char),_get_zero_arr_size(_tmp121,3));});
_tmp11C->f2=0;_tmp11C->f3=({const char*_tmp120="";_tag_dynforward(_tmp120,sizeof(
char),_get_zero_arr_size(_tmp120,1));});_tmp11C->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp11E=_region_malloc(r,sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Arg_Set_spec_struct
_tmp11F;_tmp11F.tag=3;_tmp11F.f1=& Cyc_v_r;_tmp11F;});_tmp11E;});_tmp11C->f5=({
const char*_tmp11D="Print compilation stages verbosely";_tag_dynforward(_tmp11D,
sizeof(char),_get_zero_arr_size(_tmp11D,35));});_tmp11C;});((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(r,_tag_dynforward(
_tmp11B,sizeof(struct _tuple14*),62));});struct _dynforward_ptr optstring=({const
char*_tmp11A="Options:";_tag_dynforward(_tmp11A,sizeof(char),_get_zero_arr_size(
_tmp11A,9));});Cyc_Arg_parse(options,Cyc_add_other,optstring,argv);};_pop_region(
r);}if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){({void*_tmp290[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp291="missing file\n";_tag_dynforward(_tmp291,sizeof(
char),_get_zero_arr_size(_tmp291,14));}),_tag_dynforward(_tmp290,sizeof(void*),0));});
exit(1);}if(Cyc_target_arch.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_target_arch=(struct _dynforward_ptr)Cstring_to_string(Carch);{struct
_dynforward_ptr cyclone_exec_prefix=({char*_tmp2FA=getenv((const char*)"CYCLONE_EXEC_PREFIX");
_tag_dynforward(_tmp2FA,sizeof(char),_get_zero_arr_size(_tmp2FA,1));});if(
cyclone_exec_prefix.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp292=_cycalloc(sizeof(*_tmp292));
_tmp292->hd=({struct _dynforward_ptr*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293[
0]=cyclone_exec_prefix;_tmp293;});_tmp292->tl=Cyc_cyclone_exec_path;_tmp292;});{
struct _dynforward_ptr def_lib_path=(struct _dynforward_ptr)Cstring_to_string(
Cdef_lib_path);if(Cyc_strlen((struct _dynforward_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294->hd=({struct
_dynforward_ptr*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(def_lib_path,({const char*_tmp296="cyc-lib";
_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,8));}));_tmp295;});
_tmp294->tl=Cyc_cyclone_exec_path;_tmp294;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{struct Cyc_List_List*
_tmp297=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);struct
_dynforward_ptr comp=(struct _dynforward_ptr)Cstring_to_string(Ccomp);if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_cpp,({const char*_tmp298="";_tag_dynforward(_tmp298,
sizeof(char),_get_zero_arr_size(_tmp298,1));}))== 0)Cyc_set_cpp((struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp29C;_tmp29C.tag=0;_tmp29C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_do_find(_tmp297,({const char*
_tmp29D="cycspecs";_tag_dynforward(_tmp29D,sizeof(char),_get_zero_arr_size(
_tmp29D,9));})));{struct Cyc_String_pa_struct _tmp29B;_tmp29B.tag=0;_tmp29B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)comp);{void*_tmp299[2]={& _tmp29B,&
_tmp29C};Cyc_aprintf(({const char*_tmp29A="%s -x c -E -specs %s";_tag_dynforward(
_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,21));}),_tag_dynforward(_tmp299,
sizeof(void*),2));}}}));if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dynforward_ptr)Cyc_cyc_include)== 0)Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,({
const char*_tmp29E="cyc_include.h";_tag_dynforward(_tmp29E,sizeof(char),
_get_zero_arr_size(_tmp29E,14));}));{struct Cyc_List_List*_tmp29F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp29F != 0;
_tmp29F=_tmp29F->tl){Cyc_process_file(*((struct _dynforward_ptr*)_tmp29F->hd));
if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp2A0="-L";_tag_dynforward(
_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,3));}),(struct _dynforward_ptr)
def_lib_path));Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_ccargs);{struct _dynforward_ptr _tmp2A1=Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp2F9=" ";_tag_dynforward(
_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _dynforward_ptr _tmp2A2=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp2F6->tl=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs);_tmp2F6;}),({const char*_tmp2F8=" ";_tag_dynforward(_tmp2F8,sizeof(
char),_get_zero_arr_size(_tmp2F8,2));}));struct Cyc_List_List*stdlib;struct
_dynforward_ptr stdlib_string;int _tmp2A3=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmp2F4=".a";_tag_dynforward(_tmp2F4,
sizeof(char),_get_zero_arr_size(_tmp2F4,3));}))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp2F5=".lib";
_tag_dynforward(_tmp2F5,sizeof(char),_get_zero_arr_size(_tmp2F5,5));}));if(
_tmp2A3){stdlib=0;stdlib_string=(struct _dynforward_ptr)({const char*_tmp2A4="";
_tag_dynforward(_tmp2A4,sizeof(char),_get_zero_arr_size(_tmp2A4,1));});}else{
struct _dynforward_ptr libcyc_flag;struct _dynforward_ptr nogc_filename;struct
_dynforward_ptr runtime_filename;struct _dynforward_ptr gc_filename;if(Cyc_pa_r){
libcyc_flag=({const char*_tmp2A5="-lcyc_a";_tag_dynforward(_tmp2A5,sizeof(char),
_get_zero_arr_size(_tmp2A5,8));});nogc_filename=({const char*_tmp2A6="nogc_a.a";
_tag_dynforward(_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,9));});
runtime_filename=({const char*_tmp2A7="runtime_cyc_a.o";_tag_dynforward(_tmp2A7,
sizeof(char),_get_zero_arr_size(_tmp2A7,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp2A8="-lcyc_nocheck";_tag_dynforward(_tmp2A8,sizeof(
char),_get_zero_arr_size(_tmp2A8,14));});nogc_filename=({const char*_tmp2A9="nogc_nocheck.a";
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size(_tmp2A9,15));});
runtime_filename=({const char*_tmp2AA="runtime_cyc.o";_tag_dynforward(_tmp2AA,
sizeof(char),_get_zero_arr_size(_tmp2AA,14));});}else{if(Cyc_pg_r){libcyc_flag=({
const char*_tmp2AB="-lcyc_pg";_tag_dynforward(_tmp2AB,sizeof(char),
_get_zero_arr_size(_tmp2AB,9));});runtime_filename=({const char*_tmp2AC="runtime_cyc_pg.o";
_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,17));});
nogc_filename=({const char*_tmp2AD="nogc_pg.a";_tag_dynforward(_tmp2AD,sizeof(
char),_get_zero_arr_size(_tmp2AD,10));});}else{libcyc_flag=({const char*_tmp2AE="-lcyc";
_tag_dynforward(_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,6));});
nogc_filename=({const char*_tmp2AF="nogc.a";_tag_dynforward(_tmp2AF,sizeof(char),
_get_zero_arr_size(_tmp2AF,7));});runtime_filename=({const char*_tmp2B0="runtime_cyc.o";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,14));});}}}
gc_filename=({const char*_tmp2B1="gc.a";_tag_dynforward(_tmp2B1,sizeof(char),
_get_zero_arr_size(_tmp2B1,5));});{struct _dynforward_ptr _tmp2B2=Cyc_nogc_r?Cyc_do_find(
_tmp297,nogc_filename): Cyc_do_find(_tmp297,gc_filename);struct _dynforward_ptr
_tmp2B3=Cyc_do_find(_tmp297,runtime_filename);stdlib=0;stdlib_string=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2B2);{struct Cyc_String_pa_struct
_tmp2B7;_tmp2B7.tag=0;_tmp2B7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
libcyc_flag);{struct Cyc_String_pa_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2B3);{void*_tmp2B4[3]={& _tmp2B6,&
_tmp2B7,& _tmp2B8};Cyc_aprintf(({const char*_tmp2B5=" %s %s %s";_tag_dynforward(
_tmp2B5,sizeof(char),_get_zero_arr_size(_tmp2B5,10));}),_tag_dynforward(_tmp2B4,
sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp2B9;_push_handler(&
_tmp2B9);{int _tmp2BB=0;if(setjmp(_tmp2B9.handler))_tmp2BB=1;if(!_tmp2BB){Cyc_ccargs=((
struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))
Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_libargs);{struct Cyc_List_List*_tmp2BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp2A3)_tmp2BC=({struct Cyc_List_List*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));
_tmp2BD->hd=Cyc_final_strptr;_tmp2BD->tl=_tmp2BC;_tmp2BD;});{struct Cyc_Interface_I*
_tmp2BE=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dynforward_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp2BC,_tmp2BC);if(_tmp2BE == 0){({void*_tmp2BF[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2C0="Error: interfaces incompatible\n";
_tag_dynforward(_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,32));}),
_tag_dynforward(_tmp2BF,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp2C1=1;_npop_handler(0);return _tmp2C1;}}if(_tmp2A3){if(Cyc_output_file != 0){
struct _dynforward_ptr _tmp2C2=({struct Cyc_String_pa_struct _tmp2C9;_tmp2C9.tag=0;
_tmp2C9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Filename_chop_extension(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp2C7[1]={&
_tmp2C9};Cyc_aprintf(({const char*_tmp2C8="%s.cycio";_tag_dynforward(_tmp2C8,
sizeof(char),_get_zero_arr_size(_tmp2C8,9));}),_tag_dynforward(_tmp2C7,sizeof(
void*),1));}});struct Cyc___cycFILE*_tmp2C3=Cyc_try_file_open((struct
_dynforward_ptr)_tmp2C2,({const char*_tmp2C5="wb";_tag_dynforward(_tmp2C5,sizeof(
char),_get_zero_arr_size(_tmp2C5,3));}),({const char*_tmp2C6="interface object file";
_tag_dynforward(_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,22));}));if(
_tmp2C3 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp2C4=1;
_npop_handler(0);return _tmp2C4;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp2BE,(struct Cyc___cycFILE*)_tmp2C3);Cyc_file_close((struct Cyc___cycFILE*)
_tmp2C3);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp2BE,({struct _tuple11*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));
_tmp2CA->f1=({const char*_tmp2CC="empty interface";_tag_dynforward(_tmp2CC,
sizeof(char),_get_zero_arr_size(_tmp2CC,16));});_tmp2CA->f2=({const char*_tmp2CB="global interface";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,17));});_tmp2CA;}))){({
void*_tmp2CD[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2CE="Error: some objects are still undefined\n";
_tag_dynforward(_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,41));}),
_tag_dynforward(_tmp2CD,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp2CF=1;_npop_handler(0);return _tmp2CF;}}}}};_pop_handler();}else{void*
_tmp2BA=(void*)_exn_thrown;void*_tmp2D1=_tmp2BA;_LL36:;_LL37:{void*_tmp2D2=
_tmp2D1;struct _dynforward_ptr _tmp2D3;struct _dynforward_ptr _tmp2D4;struct
_dynforward_ptr _tmp2D5;_LL3B: if(*((void**)_tmp2D2)!= Cyc_Core_Failure)goto _LL3D;
_tmp2D3=((struct Cyc_Core_Failure_struct*)_tmp2D2)->f1;_LL3C:({struct Cyc_String_pa_struct
_tmp2D8;_tmp2D8.tag=0;_tmp2D8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2D3);{void*_tmp2D6[1]={& _tmp2D8};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D7="Exception Core::Failure %s\n";
_tag_dynforward(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,28));}),
_tag_dynforward(_tmp2D6,sizeof(void*),1));}});goto _LL3A;_LL3D: if(*((void**)
_tmp2D2)!= Cyc_Core_Impossible)goto _LL3F;_tmp2D4=((struct Cyc_Core_Impossible_struct*)
_tmp2D2)->f1;_LL3E:({struct Cyc_String_pa_struct _tmp2DB;_tmp2DB.tag=0;_tmp2DB.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2D4);{void*_tmp2D9[1]={&
_tmp2DB};Cyc_fprintf(Cyc_stderr,({const char*_tmp2DA="Exception Core::Impossible %s\n";
_tag_dynforward(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,31));}),
_tag_dynforward(_tmp2D9,sizeof(void*),1));}});goto _LL3A;_LL3F: if(_tmp2D2 != Cyc_Dict_Absent)
goto _LL41;_LL40:({void*_tmp2DC[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2DD="Exception Dict::Absent\n";
_tag_dynforward(_tmp2DD,sizeof(char),_get_zero_arr_size(_tmp2DD,24));}),
_tag_dynforward(_tmp2DC,sizeof(void*),0));});goto _LL3A;_LL41: if(*((void**)
_tmp2D2)!= Cyc_Core_Invalid_argument)goto _LL43;_tmp2D5=((struct Cyc_Core_Invalid_argument_struct*)
_tmp2D2)->f1;_LL42:({struct Cyc_String_pa_struct _tmp2E0;_tmp2E0.tag=0;_tmp2E0.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2D5);{void*_tmp2DE[1]={&
_tmp2E0};Cyc_fprintf(Cyc_stderr,({const char*_tmp2DF="Exception Core::Invalid_argument %s\n";
_tag_dynforward(_tmp2DF,sizeof(char),_get_zero_arr_size(_tmp2DF,37));}),
_tag_dynforward(_tmp2DE,sizeof(void*),1));}});goto _LL3A;_LL43:;_LL44:({void*
_tmp2E1[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2E2="Uncaught exception\n";
_tag_dynforward(_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,20));}),
_tag_dynforward(_tmp2E1,sizeof(void*),0));});goto _LL3A;_LL3A:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL38:;_LL39:(void)_throw(_tmp2D1);_LL35:;}}}{
struct _dynforward_ptr outfile_str=({const char*_tmp2F3="";_tag_dynforward(_tmp2F3,
sizeof(char),_get_zero_arr_size(_tmp2F3,1));});if(Cyc_output_file != 0)
outfile_str=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2E5;_tmp2E5.tag=
0;_tmp2E5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp2E3[1]={&
_tmp2E5};Cyc_aprintf(({const char*_tmp2E4=" -o %s";_tag_dynforward(_tmp2E4,
sizeof(char),_get_zero_arr_size(_tmp2E4,7));}),_tag_dynforward(_tmp2E3,sizeof(
void*),1));}});{struct _dynforward_ptr _tmp2E6=({struct Cyc_String_pa_struct _tmp2F2;
_tmp2F2.tag=0;_tmp2F2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2A2);{
struct Cyc_String_pa_struct _tmp2F1;_tmp2F1.tag=0;_tmp2F1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdlib_string);{struct Cyc_String_pa_struct
_tmp2F0;_tmp2F0.tag=0;_tmp2F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2A1);{struct Cyc_String_pa_struct _tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)outfile_str);{struct Cyc_String_pa_struct
_tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp2EC[5]={& _tmp2EE,& _tmp2EF,& _tmp2F0,& _tmp2F1,& _tmp2F2};Cyc_aprintf(({
const char*_tmp2ED="%s %s %s%s%s";_tag_dynforward(_tmp2ED,sizeof(char),
_get_zero_arr_size(_tmp2ED,13));}),_tag_dynforward(_tmp2EC,sizeof(void*),5));}}}}}});
if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp2E9;_tmp2E9.tag=0;_tmp2E9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2E6);{void*_tmp2E7[1]={& _tmp2E9};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2E8="%s\n";_tag_dynforward(_tmp2E8,sizeof(char),
_get_zero_arr_size(_tmp2E8,4));}),_tag_dynforward(_tmp2E7,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dynforward_ptr(_tmp2E6,sizeof(char),1))!= 0){({void*_tmp2EA[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2EB="Error: C compiler failed\n";_tag_dynforward(
_tmp2EB,sizeof(char),_get_zero_arr_size(_tmp2EB,26));}),_tag_dynforward(_tmp2EA,
sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();
return Cyc_compile_failure?1: 0;}}}}}}}}
