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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(struct
_RegionHandle*,struct _dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*
t);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_unique_pointer(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessSize_struct
LessSize;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqSize_struct
LessEqSize;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
_dynforward_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_List_List*,void*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_empty_flowdict();struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Dict_Dict Cyc_CfFlowInfo_mt_place_set();int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict*
all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;
};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(struct
Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;union Cyc_CfFlowInfo_FlowInfo_union flow;
};static char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot\000";struct
Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){(void*)(enclosee->annot=(void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct
_tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot
_tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2.flow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3;(_tmp3.BottomFL).tag=0;_tmp3;});_tmp2;});
_tmp1;});_tmp0;})));}static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp4=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: if(*((void**)_tmp4)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp5=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp4)->f1;_tmp6=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp4)->f1;_LL2: return _tmp6;_LL3:;_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Core_Impossible_struct _tmp8;
_tmp8.tag=Cyc_Core_Impossible;_tmp8.f1=({const char*_tmp9="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,37));});_tmp8;});
_tmp7;}));_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;};static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);
static struct _tuple5 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static struct _tuple7 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(
s);_tmpA->flow=Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,_tmpA->flow,1);
_tmpA->visited=env->iteration_num;return _tmpA;}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpB=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpB,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct Cyc_Core_Impossible_struct _tmpD;
_tmpD.tag=Cyc_Core_Impossible;_tmpD.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmpE[1]={& _tmp10};Cyc_aprintf(({const char*_tmpF="get_field_index_fs failed: %s";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,30));}),
_tag_dynforward(_tmpE,sizeof(void*),1));}});_tmpD;});_tmpC;}));}static int Cyc_NewControlFlow_get_field_index(
void*t,struct _dynforward_ptr*f){void*_tmp11=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp12;union Cyc_Absyn_AggrInfoU_union _tmp13;struct Cyc_List_List*_tmp14;_LL6: if(
_tmp11 <= (void*)4)goto _LLA;if(*((int*)_tmp11)!= 10)goto _LL8;_tmp12=((struct Cyc_Absyn_AggrType_struct*)
_tmp11)->f1;_tmp13=_tmp12.aggr_info;_LL7: {struct Cyc_List_List*_tmp15=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp13))->impl))->fields;
_tmp14=_tmp15;goto _LL9;}_LL8: if(*((int*)_tmp11)!= 11)goto _LLA;_tmp14=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp14,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp16=_cycalloc(
sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Core_Impossible_struct _tmp17;_tmp17.tag=
Cyc_Core_Impossible;_tmp17.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp18[1]={& _tmp1A};Cyc_aprintf(({const char*_tmp19="get_field_index failed: %s";
_tag_dynforward(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,27));}),
_tag_dynforward(_tmp18,sizeof(void*),1));}});_tmp17;});_tmp16;}));_LL5:;}static
void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->all_changed,new_flow,env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1B=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union _tmp1C=Cyc_CfFlowInfo_join_flow(env->all_changed,
flow,_tmp1B->flow,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C,_tmp1B->flow)){
_tmp1B->flow=_tmp1C;if(_tmp1B->visited == env->iteration_num)env->iterate_again=1;}}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc){
union Cyc_CfFlowInfo_FlowInfo_union _tmp1D=inflow;struct Cyc_Dict_Dict _tmp1E;struct
Cyc_List_List*_tmp1F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp20;_LLD: if((_tmp1D.BottomFL).tag
!= 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp21;(_tmp21.BottomFL).tag=0;_tmp21;});_LLF: if((_tmp1D.ReachableFL).tag != 1)
goto _LLC;_tmp1E=(_tmp1D.ReachableFL).f1;_tmp1F=(_tmp1D.ReachableFL).f2;_tmp20=(
_tmp1D.ReachableFL).f3;_LL10: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp22=({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp23=_cycalloc(sizeof(*_tmp23));
_tmp23[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(
struct Cyc_Absyn_Vardecl*)vds->hd;_tmp24;});_tmp23;});_tmp1E=Cyc_Dict_insert(
_tmp1E,(void*)_tmp22,Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp25;(_tmp25.ReachableFL).tag=1;(_tmp25.ReachableFL).f1=_tmp1E;(_tmp25.ReachableFL).f2=
_tmp1F;(_tmp25.ReachableFL).f3=_tmp20;_tmp25;});_LLC:;}struct _tuple8{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp27;
struct Cyc_Dict_Dict _tmp28;struct _tuple6 _tmp26=Cyc_CfFlowInfo_save_consume_info(
outflow,0);_tmp27=_tmp26.f1;_tmp28=_tmp27.consumed;{struct _RegionHandle _tmp29=
_new_region("r");struct _RegionHandle*r=& _tmp29;_push_region(r);{struct Cyc_Iter_Iter
_tmp2A=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))
Cyc_Dict_make_iter)(r,*((struct Cyc_Dict_Dict*)_check_null(out_unique_places)));
struct _tuple8 _tmp2B=*((struct _tuple8*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(r,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple8*))Cyc_Iter_next)(_tmp2A,& _tmp2B)){struct Cyc_CfFlowInfo_Place*
_tmp2C=_tmp2B.f1;if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(_tmp28,_tmp2C))({struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;
_tmp2F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp2C));{void*_tmp2D[1]={& _tmp2F};Cyc_Tcutil_terr(_tmp2B.f2,({const char*_tmp2E="Failed to consume unique variable %s";
_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,37));}),
_tag_dynforward(_tmp2D,sizeof(void*),1));}});}};_pop_region(r);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp30=inflow;struct Cyc_Dict_Dict
_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_CfFlowInfo_ConsumeInfo _tmp33;_LL12:
if((_tmp30.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp34;(_tmp34.BottomFL).tag=0;_tmp34;});_LL14:
if((_tmp30.ReachableFL).tag != 1)goto _LL11;_tmp31=(_tmp30.ReachableFL).f1;_tmp32=(
_tmp30.ReachableFL).f2;_tmp33=(_tmp30.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp31,r)!= (void*)2)({void*_tmp35[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp36="expression may not be fully initialized";
_tag_dynforward(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,40));}),
_tag_dynforward(_tmp35,sizeof(void*),0));});{struct Cyc_Dict_Dict _tmp37=Cyc_CfFlowInfo_escape_deref(
_tmp31,env->all_changed,r);if(_tmp31.t == _tmp37.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp38=({union Cyc_CfFlowInfo_FlowInfo_union _tmp39;(_tmp39.ReachableFL).tag=1;(
_tmp39.ReachableFL).f1=_tmp37;(_tmp39.ReachableFL).f2=_tmp32;(_tmp39.ReachableFL).f3=
_tmp33;_tmp39;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp38);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp38;}}_LL11:;}struct _tuple9{
union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static struct
_tuple9 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*es,int
arg1_unconsumed){if(es == 0)return({struct _tuple9 _tmp3A;_tmp3A.f1=inflow;_tmp3A.f2=
0;_tmp3A;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union _tmp3C;void*_tmp3D;
struct _tuple5 _tmp3B=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;return({struct _tuple9 _tmp3E;_tmp3E.f1=
_tmp3C;_tmp3E.f2=({struct Cyc_List_List*_tmp3F=_region_malloc(rgn,sizeof(*_tmp3F));
_tmp3F->hd=(void*)_tmp3D;_tmp3F->tl=0;_tmp3F;});_tmp3E;});}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union
old_inflow;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_List_List*rvals;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp41;union Cyc_CfFlowInfo_FlowInfo_union _tmp42;
struct _tuple6 _tmp40=Cyc_CfFlowInfo_save_consume_info(inflow,1);_tmp41=_tmp40.f1;
_tmp42=_tmp40.f2;{struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp57;_tmp57.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp57.may_consume=0;_tmp57;});
int init_consume=0;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();_tmp42=Cyc_CfFlowInfo_restore_consume_info(
_tmp42,_tmp41);env->all_changed=({struct Cyc_Dict_Dict*_tmp43=_cycalloc(sizeof(*
_tmp43));_tmp43[0]=Cyc_CfFlowInfo_mt_place_set();_tmp43;});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp45;void*_tmp46;struct _tuple5 _tmp44=Cyc_NewControlFlow_anal_Rexp(env,_tmp42,(
struct Cyc_Absyn_Exp*)es->hd);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;outflow=_tmp45;
rvals=({struct Cyc_List_List*_tmp47=_region_malloc(rgn,sizeof(*_tmp47));_tmp47->hd=(
void*)_tmp46;_tmp47->tl=0;_tmp47;});this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp48=outflow;struct Cyc_Dict_Dict
_tmp49;struct Cyc_List_List*_tmp4A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4B;_LL17:
if((_tmp48.BottomFL).tag != 0)goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp48.ReachableFL).tag
!= 1)goto _LL16;_tmp49=(_tmp48.ReachableFL).f1;_tmp4A=(_tmp48.ReachableFL).f2;
_tmp4B=(_tmp48.ReachableFL).f3;_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C;(_tmp4C.ReachableFL).tag=1;(_tmp4C.ReachableFL).f1=
_tmp49;(_tmp4C.ReachableFL).f2=_tmp4A;(_tmp4C.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4D;_tmp4D.consumed=_tmp4B.consumed;_tmp4D.may_consume=_tmp41.may_consume;
_tmp4D;});_tmp4C;});_LL16:;}{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=
es2->tl){env->all_changed=({struct Cyc_Dict_Dict*_tmp4E=_cycalloc(sizeof(*_tmp4E));
_tmp4E[0]=Cyc_CfFlowInfo_mt_place_set();_tmp4E;});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp50;void*_tmp51;struct _tuple5 _tmp4F=Cyc_NewControlFlow_anal_Rexp(env,_tmp42,(
struct Cyc_Absyn_Exp*)es2->hd);_tmp50=_tmp4F.f1;_tmp51=_tmp4F.f2;rvals=({struct
Cyc_List_List*_tmp52=_region_malloc(rgn,sizeof(*_tmp52));_tmp52->hd=(void*)
_tmp51;_tmp52->tl=rvals;_tmp52;});outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp50,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp54;struct _tuple6 _tmp53=Cyc_CfFlowInfo_save_consume_info(
outflow,0);_tmp54=_tmp53.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp42,outflow_consume);else{old_inflow=_tmp42;}init_consume=1;outflow_consume=
_tmp54;_tmp42=Cyc_CfFlowInfo_join_flow(outer_all_changed,_tmp42,outflow,1);}}}while(
!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp42,old_inflow));if(outer_all_changed == 
0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*_tmp55=
_cycalloc(sizeof(*_tmp55));_tmp55[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp55;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple9 _tmp56;_tmp56.f1=outflow;_tmp56.f2=Cyc_List_imp_rev(
rvals);_tmp56;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp58=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp58;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5A;struct Cyc_List_List*_tmp5B;struct _tuple9 _tmp59=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5C=_tmp5A;struct Cyc_Dict_Dict _tmp5D;_LL1C: if((_tmp5C.ReachableFL).tag != 1)
goto _LL1E;_tmp5D=(_tmp5C.ReachableFL).f1;_LL1D: for(0;_tmp5B != 0;(_tmp5B=_tmp5B->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp5D,(void*)_tmp5B->hd)== (void*)0)({void*_tmp5E[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp5F="expression may not be initialized";_tag_dynforward(_tmp5F,sizeof(
char),_get_zero_arr_size(_tmp5F,34));}),_tag_dynforward(_tmp5E,sizeof(void*),0));});}
goto _LL1B;_LL1E: if((_tmp5C.BottomFL).tag != 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{
struct _tuple5 _tmp61=({struct _tuple5 _tmp60;_tmp60.f1=_tmp5A;_tmp60.f2=Cyc_CfFlowInfo_unknown_all;
_tmp60;});_npop_handler(0);return _tmp61;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp62=outflow;struct Cyc_Dict_Dict _tmp63;
struct Cyc_List_List*_tmp64;struct Cyc_CfFlowInfo_ConsumeInfo _tmp65;_LL21: if((
_tmp62.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp62.ReachableFL).tag
!= 1)goto _LL20;_tmp63=(_tmp62.ReachableFL).f1;_tmp64=(_tmp62.ReachableFL).f2;
_tmp65=(_tmp62.ReachableFL).f3;_LL24: {void*_tmp66=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp67;_LL26: if((int)_tmp66 != 0)
goto _LL28;_LL27: return outflow;_LL28: if(_tmp66 <= (void*)1)goto _LL25;if(*((int*)
_tmp66)!= 0)goto _LL25;_tmp67=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp66)->f1;
_LL29: {void*nzval=il == (void*)2?(void*)1:(void*)2;return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp68;(_tmp68.ReachableFL).tag=1;(_tmp68.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp63,env->all_changed,_tmp67,nzval);(_tmp68.ReachableFL).f2=
_tmp64;(_tmp68.ReachableFL).f3=_tmp65;_tmp68;});}_LL25:;}_LL20:;}static struct
_tuple7 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp69=outflow;
struct Cyc_Dict_Dict _tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6C;_LL2B: if((_tmp69.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple7
_tmp6D;_tmp6D.f1=outflow;_tmp6D.f2=outflow;_tmp6D;});_LL2D: if((_tmp69.ReachableFL).tag
!= 1)goto _LL2A;_tmp6A=(_tmp69.ReachableFL).f1;_tmp6B=(_tmp69.ReachableFL).f2;
_tmp6C=(_tmp69.ReachableFL).f3;_LL2E: {void*_tmp6E=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6F;_LL30: if((int)_tmp6E != 0)
goto _LL32;_LL31: return({struct _tuple7 _tmp70;_tmp70.f1=outflow;_tmp70.f2=outflow;
_tmp70;});_LL32: if(_tmp6E <= (void*)1)goto _LL2F;if(*((int*)_tmp6E)!= 0)goto _LL2F;
_tmp6F=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp6E)->f1;_LL33: {void*nzval=il == (
void*)2?(void*)1:(void*)2;return({struct _tuple7 _tmp71;_tmp71.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp73;(_tmp73.ReachableFL).tag=1;(_tmp73.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp6A,env->all_changed,_tmp6F,nzval);(_tmp73.ReachableFL).f2=
_tmp6B;(_tmp73.ReachableFL).f3=_tmp6C;_tmp73;});_tmp71.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp72;(_tmp72.ReachableFL).tag=1;(_tmp72.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp6A,env->all_changed,_tmp6F,(void*)0);(
_tmp72.ReachableFL).f2=_tmp6B;(_tmp72.ReachableFL).f3=_tmp6C;_tmp72;});_tmp71;});}
_LL2F:;}_LL2A:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow,
struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo_union
_tmp74=flow;struct Cyc_Dict_Dict _tmp75;struct Cyc_List_List*_tmp76;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp77;_LL35: if((_tmp74.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((
_tmp74.ReachableFL).tag != 1)goto _LL34;_tmp75=(_tmp74.ReachableFL).f1;_tmp76=(
_tmp74.ReachableFL).f2;_tmp77=(_tmp74.ReachableFL).f3;_LL38: {void*_tmp78=r2;
struct Cyc_List_List*_tmp79;_LL3A: if(_tmp78 <= (void*)3)goto _LL3C;if(*((int*)
_tmp78)!= 3)goto _LL3C;_tmp79=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp78)->f1;
_LL3B: {void*_tmp7A=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp7B;_LL3F: if((int)_tmp7A != 0)goto _LL41;_LL40: return flow;_LL41: if(_tmp7A <= (
void*)1)goto _LL3E;if(*((int*)_tmp7A)!= 0)goto _LL3E;_tmp7B=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp7A)->f1;_LL42: {struct Cyc_List_List*new_cl;{void*_tmp7C=r1;struct Cyc_List_List*
_tmp7D;void*_tmp7E;_LL44: if(_tmp7C <= (void*)3)goto _LL48;if(*((int*)_tmp7C)!= 3)
goto _LL46;_tmp7D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7C)->f1;_LL45: new_cl=
_tmp7D;goto _LL43;_LL46: if(*((int*)_tmp7C)!= 0)goto _LL48;_tmp7E=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp7C)->f1;if((int)_tmp7E != 2)goto _LL48;_LL47:
goto _LL49;_LL48: if((int)_tmp7C != 0)goto _LL4A;_LL49: goto _LL4B;_LL4A: if((int)
_tmp7C != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D: return flow;_LL43:;}
for(0;_tmp79 != 0;_tmp79=_tmp79->tl){void*new_cmp;{struct _tuple0 _tmp80=({struct
_tuple0 _tmp7F;_tmp7F.f1=p;_tmp7F.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp79->hd)->cmp;
_tmp7F;});void*_tmp81;void*_tmp82;void*_tmp83;void*_tmp84;_LL4F: _tmp81=_tmp80.f1;
if((int)_tmp81 != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp82=_tmp80.f2;if((int)
_tmp82 != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53: _tmp83=_tmp80.f1;if((
int)_tmp83 != 5)goto _LL55;_tmp84=_tmp80.f2;if((int)_tmp84 != 5)goto _LL55;_LL54:
new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto _LL4E;_LL4E:;}
new_cl=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=({
struct Cyc_CfFlowInfo_TagCmp*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->cmp=(void*)
new_cmp;_tmp86->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp79->hd)->bd);
_tmp86;});_tmp85->tl=new_cl;_tmp85;});}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp87;(_tmp87.ReachableFL).tag=1;(_tmp87.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(e1->loc,_tmp75,env->all_changed,_tmp7B,(void*)({
struct Cyc_CfFlowInfo_TagCmps_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_CfFlowInfo_TagCmps_struct _tmp89;_tmp89.tag=3;_tmp89.f1=new_cl;_tmp89;});
_tmp88;}));(_tmp87.ReachableFL).f2=_tmp76;(_tmp87.ReachableFL).f3=_tmp77;_tmp87;});}
_LL3E:;}_LL3C:;_LL3D: return flow;_LL39:;}_LL34:;}static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp8A=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp8B;void*_tmp8C;struct Cyc_Absyn_PtrAtts _tmp8D;struct
Cyc_Absyn_Conref*_tmp8E;struct Cyc_Absyn_Conref*_tmp8F;_LL58: if(_tmp8A <= (void*)4)
goto _LL5A;if(*((int*)_tmp8A)!= 4)goto _LL5A;_tmp8B=((struct Cyc_Absyn_PointerType_struct*)
_tmp8A)->f1;_tmp8C=(void*)_tmp8B.elt_typ;_tmp8D=_tmp8B.ptr_atts;_tmp8E=_tmp8D.bounds;
_tmp8F=_tmp8D.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp90=f;
struct Cyc_Dict_Dict _tmp91;struct Cyc_List_List*_tmp92;_LL5D: if((_tmp90.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp93;_tmp93.f1=f;_tmp93.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp8C,Cyc_CfFlowInfo_unknown_all);_tmp93;});_LL5F: if((_tmp90.ReachableFL).tag != 
1)goto _LL5C;_tmp91=(_tmp90.ReachableFL).f1;_tmp92=(_tmp90.ReachableFL).f2;_LL60:
if(Cyc_Tcutil_is_bound_one(_tmp8E)){void*_tmp94=r;struct Cyc_CfFlowInfo_Place*
_tmp95;void*_tmp96;_LL62: if((int)_tmp94 != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp94 != 2)goto _LL66;_LL65:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp98;_tmp98.tag=Cyc_CfFlowInfo_NotZero;_tmp98.f1=_tmp92;_tmp98;});_tmp97;})));
goto _LL61;_LL66: if(_tmp94 <= (void*)3)goto _LL68;if(*((int*)_tmp94)!= 2)goto _LL68;
_tmp95=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp94)->f1;_LL67:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp9A;_tmp9A.tag=Cyc_CfFlowInfo_NotZero;
_tmp9A.f1=_tmp92;_tmp9A;});_tmp99;})));return({struct _tuple5 _tmp9B;_tmp9B.f1=f;
_tmp9B.f2=Cyc_CfFlowInfo_lookup_place(_tmp91,_tmp95);_tmp9B;});_LL68: if((int)
_tmp94 != 0)goto _LL6A;_LL69:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple5 _tmp9C;_tmp9C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp9D;(_tmp9D.BottomFL).tag=0;_tmp9D;});
_tmp9C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp8C,Cyc_CfFlowInfo_unknown_all);
_tmp9C;});_LL6A: if(_tmp94 <= (void*)3)goto _LL6C;if(*((int*)_tmp94)!= 0)goto _LL6C;
_tmp96=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp94)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp96);goto _LL6D;_LL6C:;_LL6D:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp9F;_tmp9F.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp9F.f1=_tmp92;_tmp9F;});_tmp9E;})));_LL61:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));
_tmpA0[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpA1;_tmpA1.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpA1.f1=_tmp92;_tmpA1;});_tmpA0;})));}{void*_tmpA2=Cyc_CfFlowInfo_initlevel(
_tmp91,r);_LL6F: if((int)_tmpA2 != 0)goto _LL71;_LL70:({void*_tmpA3[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmpA4="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,46));}),
_tag_dynforward(_tmpA3,sizeof(void*),0));});goto _LL72;_LL71: if((int)_tmpA2 != 2)
goto _LL73;_LL72: return({struct _tuple5 _tmpA5;_tmpA5.f1=f;_tmpA5.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp8C,Cyc_CfFlowInfo_unknown_all);_tmpA5;});_LL73: if((int)_tmpA2 != 1)goto _LL6E;
_LL74: return({struct _tuple5 _tmpA6;_tmpA6.f1=f;_tmpA6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp8C,Cyc_CfFlowInfo_unknown_none);_tmpA6;});_LL6E:;}_LL5C:;}_LL5A:;_LL5B:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7[0]=({struct Cyc_Core_Impossible_struct _tmpA8;_tmpA8.tag=Cyc_Core_Impossible;
_tmpA8.f1=({const char*_tmpA9="right deref of non-pointer-type";_tag_dynforward(
_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,32));});_tmpA8;});_tmpA7;}));_LL57:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpAA=(void*)e1->r;
void*_tmpAB;struct Cyc_Absyn_Vardecl*_tmpAC;void*_tmpAD;struct Cyc_Absyn_Vardecl*
_tmpAE;void*_tmpAF;struct Cyc_Absyn_Vardecl*_tmpB0;void*_tmpB1;struct Cyc_Absyn_Vardecl*
_tmpB2;_LL76: if(*((int*)_tmpAA)!= 1)goto _LL78;_tmpAB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpAB <= (void*)1)goto _LL78;if(*((int*)_tmpAB)!= 4)goto _LL78;
_tmpAC=((struct Cyc_Absyn_Pat_b_struct*)_tmpAB)->f1;_LL77: _tmpAE=_tmpAC;goto _LL79;
_LL78: if(*((int*)_tmpAA)!= 1)goto _LL7A;_tmpAD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpAD <= (void*)1)goto _LL7A;if(*((int*)_tmpAD)!= 3)goto _LL7A;
_tmpAE=((struct Cyc_Absyn_Local_b_struct*)_tmpAD)->f1;_LL79: _tmpB0=_tmpAE;goto
_LL7B;_LL7A: if(*((int*)_tmpAA)!= 1)goto _LL7C;_tmpAF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpAF <= (void*)1)goto _LL7C;if(*((int*)_tmpAF)!= 2)goto _LL7C;
_tmpB0=((struct Cyc_Absyn_Param_b_struct*)_tmpAF)->f1;_LL7B: _tmpB2=_tmpB0;goto
_LL7D;_LL7C: if(*((int*)_tmpAA)!= 1)goto _LL7E;_tmpB1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f2;if(_tmpB1 <= (void*)1)goto _LL7E;if(*((int*)_tmpB1)!= 0)goto _LL7E;
_tmpB2=((struct Cyc_Absyn_Global_b_struct*)_tmpB1)->f1;_LL7D: if(!_tmpB2->escapes){
void*_tmpB3=(void*)e2->r;void*_tmpB4;struct Cyc_Absyn_Vardecl*_tmpB5;void*_tmpB6;
struct Cyc_Absyn_Vardecl*_tmpB7;void*_tmpB8;struct Cyc_Absyn_Vardecl*_tmpB9;void*
_tmpBA;struct Cyc_Absyn_Vardecl*_tmpBB;_LL81: if(*((int*)_tmpB3)!= 1)goto _LL83;
_tmpB4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpB4 <= (void*)1)
goto _LL83;if(*((int*)_tmpB4)!= 4)goto _LL83;_tmpB5=((struct Cyc_Absyn_Pat_b_struct*)
_tmpB4)->f1;_LL82: _tmpB7=_tmpB5;goto _LL84;_LL83: if(*((int*)_tmpB3)!= 1)goto _LL85;
_tmpB6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpB6 <= (void*)1)
goto _LL85;if(*((int*)_tmpB6)!= 3)goto _LL85;_tmpB7=((struct Cyc_Absyn_Local_b_struct*)
_tmpB6)->f1;_LL84: _tmpB9=_tmpB7;goto _LL86;_LL85: if(*((int*)_tmpB3)!= 1)goto _LL87;
_tmpB8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpB8 <= (void*)1)
goto _LL87;if(*((int*)_tmpB8)!= 2)goto _LL87;_tmpB9=((struct Cyc_Absyn_Param_b_struct*)
_tmpB8)->f1;_LL86: _tmpBB=_tmpB9;goto _LL88;_LL87: if(*((int*)_tmpB3)!= 1)goto _LL89;
_tmpBA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB3)->f2;if(_tmpBA <= (void*)1)
goto _LL89;if(*((int*)_tmpBA)!= 0)goto _LL89;_tmpBB=((struct Cyc_Absyn_Global_b_struct*)
_tmpBA)->f1;_LL88: if(!_tmpBB->escapes){int found=0;{struct Cyc_List_List*_tmpBC=
relns;for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){struct Cyc_CfFlowInfo_Reln*_tmpBD=(
struct Cyc_CfFlowInfo_Reln*)_tmpBC->hd;if(_tmpBD->vd == _tmpBB){union Cyc_CfFlowInfo_RelnOp_union
_tmpBE=_tmpBD->rop;struct Cyc_Absyn_Vardecl*_tmpBF;_LL8C: if((_tmpBE.LessSize).tag
!= 2)goto _LL8E;_tmpBF=(_tmpBE.LessSize).f1;if(!(_tmpBF == _tmpB2))goto _LL8E;_LL8D:
return relns;_LL8E:;_LL8F: continue;_LL8B:;}}}if(!found)return({struct Cyc_List_List*
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->hd=({struct Cyc_CfFlowInfo_Reln*_tmpC1=
_cycalloc(sizeof(*_tmpC1));_tmpC1->vd=_tmpBB;_tmpC1->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmpC2;(_tmpC2.LessSize).tag=2;(_tmpC2.LessSize).f1=
_tmpB2;_tmpC2;});_tmpC1;});_tmpC0->tl=relns;_tmpC0;});}return relns;_LL89:;_LL8A:
return relns;_LL80:;}else{return relns;}_LL7E:;_LL7F: return relns;_LL75:;}struct
_tuple10{void*f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple11{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmpC3=inflow;struct Cyc_Dict_Dict _tmpC4;
struct Cyc_List_List*_tmpC5;struct Cyc_CfFlowInfo_ConsumeInfo _tmpC6;_LL91: if((
_tmpC3.BottomFL).tag != 0)goto _LL93;_LL92: return({struct _tuple5 _tmpC7;_tmpC7.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpC8;(
_tmpC8.BottomFL).tag=0;_tmpC8;});_tmpC7.f2=Cyc_CfFlowInfo_unknown_all;_tmpC7;});
_LL93: if((_tmpC3.ReachableFL).tag != 1)goto _LL90;_tmpC4=(_tmpC3.ReachableFL).f1;
_tmpC5=(_tmpC3.ReachableFL).f2;_tmpC6=(_tmpC3.ReachableFL).f3;_LL94: d=_tmpC4;
relns=_tmpC5;cinfo=_tmpC6;_LL90:;}{void*_tmpC9=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpCA;void*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct
Cyc_Absyn_Exp*_tmpCE;union Cyc_Absyn_Cnst_union _tmpCF;union Cyc_Absyn_Cnst_union
_tmpD0;int _tmpD1;union Cyc_Absyn_Cnst_union _tmpD2;void*_tmpD3;struct Cyc_List_List*
_tmpD4;void*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Vardecl*_tmpD7;void*_tmpD8;struct
Cyc_Absyn_Vardecl*_tmpD9;void*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;
struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;
struct Cyc_Core_Opt*_tmpE0;struct Cyc_Core_Opt _tmpE1;struct Cyc_Absyn_Exp*_tmpE2;
struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;
struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;
struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_MallocInfo
_tmpEB;int _tmpEC;struct Cyc_Absyn_Exp*_tmpED;void**_tmpEE;struct Cyc_Absyn_Exp*
_tmpEF;int _tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*
_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct _dynforward_ptr*_tmpF8;struct Cyc_Absyn_Exp*
_tmpF9;struct _dynforward_ptr*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*
_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*
_tmpFF;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*
_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_Absyn_Tuniondecl*
_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_List_List*
_tmp108;struct Cyc_Absyn_Aggrdecl*_tmp109;struct Cyc_Absyn_Aggrdecl _tmp10A;struct
Cyc_Absyn_AggrdeclImpl*_tmp10B;struct Cyc_Absyn_AggrdeclImpl _tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Vardecl*_tmp10F;struct Cyc_Absyn_Exp*
_tmp110;struct Cyc_Absyn_Exp*_tmp111;int _tmp112;struct Cyc_Absyn_Stmt*_tmp113;void*
_tmp114;_LL96: if(*((int*)_tmpC9)!= 15)goto _LL98;_tmpCA=((struct Cyc_Absyn_Cast_e_struct*)
_tmpC9)->f2;_tmpCB=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpC9)->f4;if((int)
_tmpCB != 2)goto _LL98;_LL97: {union Cyc_CfFlowInfo_FlowInfo_union _tmp116;void*
_tmp117;struct _tuple5 _tmp115=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpCA);
_tmp116=_tmp115.f1;_tmp117=_tmp115.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp119;
void*_tmp11A;struct _tuple5 _tmp118=Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp116,_tmpCA,_tmp117);_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;return({struct
_tuple5 _tmp11B;_tmp11B.f1=_tmp119;_tmp11B.f2=_tmp117;_tmp11B;});}}_LL98: if(*((
int*)_tmpC9)!= 15)goto _LL9A;_tmpCC=((struct Cyc_Absyn_Cast_e_struct*)_tmpC9)->f2;
_LL99: _tmpCD=_tmpCC;goto _LL9B;_LL9A: if(*((int*)_tmpC9)!= 13)goto _LL9C;_tmpCD=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpC9)->f1;_LL9B: _tmpCE=_tmpCD;goto
_LL9D;_LL9C: if(*((int*)_tmpC9)!= 14)goto _LL9E;_tmpCE=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpC9)->f1;_LL9D: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpCE);_LL9E:
if(*((int*)_tmpC9)!= 0)goto _LLA0;_tmpCF=((struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1).Null_c).tag != 6)goto _LLA0;
_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmpC9)!= 0)goto _LLA2;_tmpD0=((struct Cyc_Absyn_Const_e_struct*)
_tmpC9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1).Int_c).tag != 2)
goto _LLA2;_tmpD1=(_tmpD0.Int_c).f2;if(_tmpD1 != 0)goto _LLA2;_LLA1: return({struct
_tuple5 _tmp11C;_tmp11C.f1=inflow;_tmp11C.f2=(void*)0;_tmp11C;});_LLA2: if(*((int*)
_tmpC9)!= 0)goto _LLA4;_tmpD2=((struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpC9)->f1).Int_c).tag != 2)goto _LLA4;_LLA3: goto
_LLA5;_LLA4: if(*((int*)_tmpC9)!= 1)goto _LLA6;_tmpD3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD3 <= (void*)1)goto _LLA6;if(*((int*)_tmpD3)!= 1)goto _LLA6;
_LLA5: return({struct _tuple5 _tmp11D;_tmp11D.f1=inflow;_tmp11D.f2=(void*)1;_tmp11D;});
_LLA6: if(*((int*)_tmpC9)!= 32)goto _LLA8;_tmpD4=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpC9)->f1;if(_tmpD4 != 0)goto _LLA8;_LLA7: goto _LLA9;_LLA8: if(*((int*)_tmpC9)!= 0)
goto _LLAA;_LLA9: goto _LLAB;_LLAA: if(*((int*)_tmpC9)!= 19)goto _LLAC;_LLAB: goto
_LLAD;_LLAC: if(*((int*)_tmpC9)!= 18)goto _LLAE;_LLAD: goto _LLAF;_LLAE: if(*((int*)
_tmpC9)!= 20)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if(*((int*)_tmpC9)!= 21)goto _LLB2;
_LLB1: goto _LLB3;_LLB2: if(*((int*)_tmpC9)!= 34)goto _LLB4;_LLB3: goto _LLB5;_LLB4:
if(*((int*)_tmpC9)!= 33)goto _LLB6;_LLB5: return({struct _tuple5 _tmp11E;_tmp11E.f1=
inflow;_tmp11E.f2=Cyc_CfFlowInfo_unknown_all;_tmp11E;});_LLB6: if(*((int*)_tmpC9)
!= 1)goto _LLB8;_tmpD5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC9)->f2;if(
_tmpD5 <= (void*)1)goto _LLB8;if(*((int*)_tmpD5)!= 0)goto _LLB8;_LLB7: return({
struct _tuple5 _tmp11F;_tmp11F.f1=inflow;_tmp11F.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp11F;});_LLB8: if(*((int*)_tmpC9)!= 1)goto _LLBA;_tmpD6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD6 <= (void*)1)goto _LLBA;if(*((int*)_tmpD6)!= 2)goto _LLBA;
_tmpD7=((struct Cyc_Absyn_Param_b_struct*)_tmpD6)->f1;_LLB9: _tmpD9=_tmpD7;goto
_LLBB;_LLBA: if(*((int*)_tmpC9)!= 1)goto _LLBC;_tmpD8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD8 <= (void*)1)goto _LLBC;if(*((int*)_tmpD8)!= 3)goto _LLBC;
_tmpD9=((struct Cyc_Absyn_Local_b_struct*)_tmpD8)->f1;_LLBB: _tmpDB=_tmpD9;goto
_LLBD;_LLBC: if(*((int*)_tmpC9)!= 1)goto _LLBE;_tmpDA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpDA <= (void*)1)goto _LLBE;if(*((int*)_tmpDA)!= 4)goto _LLBE;
_tmpDB=((struct Cyc_Absyn_Pat_b_struct*)_tmpDA)->f1;_LLBD: if(Cyc_Tcutil_is_unique_pointer((
void*)_tmpDB->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp120=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=_tmpDB;_tmp126;});_tmp125;});struct Cyc_CfFlowInfo_Place*
_tmp121=({struct Cyc_CfFlowInfo_Place*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->root=(
void*)((void*)_tmp120);_tmp124->fields=0;_tmp124;});cinfo.may_consume=((struct
Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->hd=_tmp121;
_tmp122->tl=0;_tmp122;}),cinfo.may_consume);inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp123;(_tmp123.ReachableFL).tag=1;(_tmp123.ReachableFL).f1=
d;(_tmp123.ReachableFL).f2=relns;(_tmp123.ReachableFL).f3=cinfo;_tmp123;});}
return({struct _tuple5 _tmp127;_tmp127.f1=inflow;_tmp127.f2=Cyc_Dict_lookup(d,(
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp128=_cycalloc(sizeof(*_tmp128));
_tmp128[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp129;_tmp129.tag=0;_tmp129.f1=
_tmpDB;_tmp129;});_tmp128;}));_tmp127;});_LLBE: if(*((int*)_tmpC9)!= 3)goto _LLC0;
_tmpDC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpC9)->f1;_tmpDD=((struct Cyc_Absyn_Primop_e_struct*)
_tmpC9)->f2;_LLBF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp12B;void*_tmp12C;
struct _tuple5 _tmp12A=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmpDD,0);
_tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;{void*_tmp12D=_tmpDC;_LLFD: if((int)_tmp12D
!= 0)goto _LLFF;_LLFE: goto _LL100;_LLFF: if((int)_tmp12D != 2)goto _LL101;_LL100: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpDD))->hd)->loc,
_tmp12B);goto _LLFC;_LL101:;_LL102: _tmp12B=Cyc_CfFlowInfo_readthrough_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpDD))->hd)->loc,
_tmp12B);goto _LLFC;_LLFC:;}return({struct _tuple5 _tmp12E;_tmp12E.f1=_tmp12B;
_tmp12E.f2=_tmp12C;_tmp12E;});}_LLC0: if(*((int*)_tmpC9)!= 5)goto _LLC2;_tmpDE=((
struct Cyc_Absyn_Increment_e_struct*)_tmpC9)->f1;_LLC1: {struct Cyc_List_List
_tmp12F=({struct Cyc_List_List _tmp13E;_tmp13E.hd=_tmpDE;_tmp13E.tl=0;_tmp13E;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp131;struct _tuple5 _tmp130=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp12F,0);_tmp131=_tmp130.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmpDE->loc,_tmp131);{void*_tmp133;struct _tuple5 _tmp132=Cyc_NewControlFlow_anal_Lexp(
env,_tmp131,_tmpDE);_tmp133=_tmp132.f2;{struct _tuple10 _tmp135=({struct _tuple10
_tmp134;_tmp134.f1=_tmp133;_tmp134.f2=_tmp131;_tmp134;});void*_tmp136;struct Cyc_CfFlowInfo_Place*
_tmp137;union Cyc_CfFlowInfo_FlowInfo_union _tmp138;struct Cyc_Dict_Dict _tmp139;
struct Cyc_List_List*_tmp13A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp13B;_LL104:
_tmp136=_tmp135.f1;if(_tmp136 <= (void*)1)goto _LL106;if(*((int*)_tmp136)!= 0)goto
_LL106;_tmp137=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp136)->f1;_tmp138=
_tmp135.f2;if(((_tmp135.f2).ReachableFL).tag != 1)goto _LL106;_tmp139=(_tmp138.ReachableFL).f1;
_tmp13A=(_tmp138.ReachableFL).f2;_tmp13B=(_tmp138.ReachableFL).f3;_LL105: _tmp13A=
Cyc_CfFlowInfo_reln_kill_exp(_tmp13A,_tmpDE);_tmp131=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp13C;(_tmp13C.ReachableFL).tag=1;(_tmp13C.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(_tmpDE->loc,_tmp139,env->all_changed,_tmp137,Cyc_CfFlowInfo_unknown_all);(
_tmp13C.ReachableFL).f2=_tmp13A;(_tmp13C.ReachableFL).f3=_tmp13B;_tmp13C;});goto
_LL103;_LL106:;_LL107: goto _LL103;_LL103:;}return({struct _tuple5 _tmp13D;_tmp13D.f1=
_tmp131;_tmp13D.f2=Cyc_CfFlowInfo_unknown_all;_tmp13D;});}}_LLC2: if(*((int*)
_tmpC9)!= 4)goto _LLC4;_tmpDF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f1;
_tmpE0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f2;if(_tmpE0 == 0)goto _LLC4;
_tmpE1=*_tmpE0;_tmpE2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f3;_LLC3: {
struct Cyc_List_List _tmp13F=({struct Cyc_List_List _tmp15D;_tmp15D.hd=_tmpE2;
_tmp15D.tl=0;_tmp15D;});struct Cyc_List_List _tmp140=({struct Cyc_List_List _tmp15C;
_tmp15C.hd=_tmpDF;_tmp15C.tl=(struct Cyc_List_List*)& _tmp13F;_tmp15C;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp142;struct _tuple5 _tmp141=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp140,1);_tmp142=_tmp141.f1;{void*_tmp144;struct _tuple5 _tmp143=
Cyc_NewControlFlow_anal_Lexp(env,_tmp142,e);_tmp144=_tmp143.f2;_tmp142=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp142);{struct _tuple10 _tmp146=({struct _tuple10 _tmp145;_tmp145.f1=
_tmp144;_tmp145.f2=_tmp142;_tmp145;});void*_tmp147;struct Cyc_CfFlowInfo_Place*
_tmp148;union Cyc_CfFlowInfo_FlowInfo_union _tmp149;struct Cyc_Dict_Dict _tmp14A;
struct Cyc_List_List*_tmp14B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp14C;_LL109:
_tmp147=_tmp146.f1;if(_tmp147 <= (void*)1)goto _LL10B;if(*((int*)_tmp147)!= 0)goto
_LL10B;_tmp148=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp147)->f1;_tmp149=
_tmp146.f2;if(((_tmp146.f2).ReachableFL).tag != 1)goto _LL10B;_tmp14A=(_tmp149.ReachableFL).f1;
_tmp14B=(_tmp149.ReachableFL).f2;_tmp14C=(_tmp149.ReachableFL).f3;_LL10A: _tmp14B=
Cyc_CfFlowInfo_reln_kill_exp(_tmp14B,_tmpDF);_tmp14A=Cyc_CfFlowInfo_assign_place(
_tmpDF->loc,_tmp14A,env->all_changed,_tmp148,Cyc_CfFlowInfo_unknown_all);if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDF->topt))->v)){_tmp14C.may_consume=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=_tmp148;
_tmp14D->tl=0;_tmp14D;}),_tmp14C.may_consume);if(!((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp14C.consumed,_tmp148)){struct
Cyc_CfFlowInfo_Place _tmp14F;void*_tmp150;struct Cyc_CfFlowInfo_Place*_tmp14E=
_tmp148;_tmp14F=*_tmp14E;_tmp150=(void*)_tmp14F.root;{void*_tmp151=_tmp150;
struct Cyc_Absyn_Vardecl*_tmp152;_LL10E: if(*((int*)_tmp151)!= 0)goto _LL110;
_tmp152=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp151)->f1;_LL10F: {struct
_dynforward_ptr _tmp153=Cyc_Absynpp_qvar2string(_tmp152->name);({struct Cyc_String_pa_struct
_tmp156;_tmp156.tag=0;_tmp156.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp153);{void*_tmp154[1]={& _tmp156};Cyc_Tcutil_warn(_tmpDF->loc,({const char*
_tmp155="may clobber unique pointer %s";_tag_dynforward(_tmp155,sizeof(char),
_get_zero_arr_size(_tmp155,30));}),_tag_dynforward(_tmp154,sizeof(void*),1));}});
goto _LL10D;}_LL110:;_LL111:({void*_tmp157[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp158="warning locations not for VarRoots";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,35));}),
_tag_dynforward(_tmp157,sizeof(void*),0));});_LL10D:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict _tmp159=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct
Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(_tmp14C.consumed,_tmp148);if((_tmp14C.consumed).t
!= _tmp159.t);_tmp14C.consumed=_tmp159;}}_tmp142=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp15A;(_tmp15A.ReachableFL).tag=1;(_tmp15A.ReachableFL).f1=
_tmp14A;(_tmp15A.ReachableFL).f2=_tmp14B;(_tmp15A.ReachableFL).f3=_tmp14C;
_tmp15A;});goto _LL108;_LL10B:;_LL10C: goto _LL108;_LL108:;}return({struct _tuple5
_tmp15B;_tmp15B.f1=_tmp142;_tmp15B.f2=Cyc_CfFlowInfo_unknown_all;_tmp15B;});}}
_LLC4: if(*((int*)_tmpC9)!= 4)goto _LLC6;_tmpE3=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpC9)->f1;_tmpE4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f2;if(_tmpE4 != 
0)goto _LLC6;_tmpE5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC9)->f3;_LLC5: {
struct Cyc_Dict_Dict*_tmp15E=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp160;union Cyc_CfFlowInfo_FlowInfo_union _tmp161;struct _tuple6 _tmp15F=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp18A;_tmp18A.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp18A.may_consume=0;_tmp18A;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp162;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp164;void*_tmp165;struct _tuple5
_tmp163=Cyc_NewControlFlow_anal_Lexp(env,_tmp161,_tmpE3);_tmp164=_tmp163.f1;
_tmp165=_tmp163.f2;{struct Cyc_Dict_Dict _tmp166=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp167=
_cycalloc(sizeof(*_tmp167));_tmp167[0]=Cyc_CfFlowInfo_mt_place_set();_tmp167;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp169;void*_tmp16A;struct _tuple5 _tmp168=Cyc_NewControlFlow_anal_Rexp(
env,_tmp161,_tmpE5);_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;{struct Cyc_Dict_Dict
_tmp16B=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp16C=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict*)& _tmp166,_tmp164,_tmp169,
_tmp166,_tmp16B);union Cyc_CfFlowInfo_FlowInfo_union _tmp16D=Cyc_CfFlowInfo_join_flow(
_tmp15E,_tmp161,_tmp16C,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp16F;struct _tuple6
_tmp16E=Cyc_CfFlowInfo_save_consume_info(_tmp16D,0);_tmp16F=_tmp16E.f1;if(
init_consume)_tmp161=Cyc_CfFlowInfo_restore_consume_info(_tmp161,outflow_consume);
outflow_consume=_tmp16F;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp16D,_tmp161)){if(_tmp15E == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp15E,Cyc_CfFlowInfo_union_place_set(
_tmp166,_tmp16B,0),0);}_tmp16C=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp16C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp170=_tmp16C;struct Cyc_Dict_Dict
_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_CfFlowInfo_ConsumeInfo _tmp173;
_LL113: if((_tmp170.BottomFL).tag != 0)goto _LL115;_LL114: return({struct _tuple5
_tmp174;_tmp174.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp175;(_tmp175.BottomFL).tag=0;_tmp175;});_tmp174.f2=_tmp16A;_tmp174;});_LL115:
if((_tmp170.ReachableFL).tag != 1)goto _LL112;_tmp171=(_tmp170.ReachableFL).f1;
_tmp172=(_tmp170.ReachableFL).f2;_tmp173=(_tmp170.ReachableFL).f3;_LL116: _tmp173=
Cyc_CfFlowInfo_and_consume(_tmp160,_tmp173);{void*_tmp176=_tmp165;struct Cyc_CfFlowInfo_Place*
_tmp177;_LL118: if(_tmp176 <= (void*)1)goto _LL11A;if(*((int*)_tmp176)!= 0)goto
_LL11A;_tmp177=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp176)->f1;_LL119: _tmp171=
Cyc_CfFlowInfo_assign_place(e->loc,_tmp171,env->all_changed,_tmp177,_tmp16A);if(
Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE3->topt))->v)){
_tmp173.may_consume=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(
Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*_tmp178=_cycalloc(sizeof(*_tmp178));
_tmp178->hd=_tmp177;_tmp178->tl=0;_tmp178;}),_tmp173.may_consume);if(!((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp173.consumed,
_tmp177)){struct Cyc_CfFlowInfo_Place _tmp17A;void*_tmp17B;struct Cyc_CfFlowInfo_Place*
_tmp179=_tmp177;_tmp17A=*_tmp179;_tmp17B=(void*)_tmp17A.root;{void*_tmp17C=
_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17D;_LL11D: if(*((int*)_tmp17C)!= 0)goto
_LL11F;_tmp17D=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp17C)->f1;_LL11E: {
struct _dynforward_ptr _tmp17E=Cyc_Absynpp_qvar2string(_tmp17D->name);({struct Cyc_String_pa_struct
_tmp181;_tmp181.tag=0;_tmp181.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp17E);{void*_tmp17F[1]={& _tmp181};Cyc_Tcutil_warn(_tmpE3->loc,({const char*
_tmp180="may clobber unique pointer %s";_tag_dynforward(_tmp180,sizeof(char),
_get_zero_arr_size(_tmp180,30));}),_tag_dynforward(_tmp17F,sizeof(void*),1));}});
goto _LL11C;}_LL11F:;_LL120:({void*_tmp182[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp183="warning locations not for VarRoots";
_tag_dynforward(_tmp183,sizeof(char),_get_zero_arr_size(_tmp183,35));}),
_tag_dynforward(_tmp182,sizeof(void*),0));});_LL11C:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict _tmp184=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct
Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(_tmp173.consumed,_tmp177);if((_tmp173.consumed).t
!= _tmp184.t);_tmp173.consumed=_tmp184;}}_tmp172=Cyc_CfFlowInfo_reln_assign_exp(
_tmp172,_tmpE3,_tmpE5);_tmp16C=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp185;(_tmp185.ReachableFL).tag=1;(_tmp185.ReachableFL).f1=_tmp171;(_tmp185.ReachableFL).f2=
_tmp172;(_tmp185.ReachableFL).f3=_tmp173;_tmp185;});Cyc_NewControlFlow_update_tryflow(
env,_tmp16C);return({struct _tuple5 _tmp186;_tmp186.f1=_tmp16C;_tmp186.f2=_tmp16A;
_tmp186;});_LL11A: if((int)_tmp176 != 0)goto _LL117;_LL11B: if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE3->topt))->v))({void*_tmp187[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp188="dest of unique assignment is unknown!";_tag_dynforward(
_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,38));}),_tag_dynforward(_tmp187,
sizeof(void*),0));});return({struct _tuple5 _tmp189;_tmp189.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpE5->loc,_tmp16C,_tmp16A);_tmp189.f2=_tmp16A;_tmp189;});_LL117:;}_LL112:;}}
_tmp161=Cyc_CfFlowInfo_restore_consume_info(_tmp16D,empty_consume);}}}}}}}_LLC6:
if(*((int*)_tmpC9)!= 9)goto _LLC8;_tmpE6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpC9)->f1;_tmpE7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpC9)->f2;_LLC7: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp18C;void*_tmp18D;struct _tuple5 _tmp18B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE6);_tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;_tmp18C=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpE6->loc,_tmp18C);return Cyc_NewControlFlow_anal_Rexp(env,_tmp18C,_tmpE7);}
_LLC8: if(*((int*)_tmpC9)!= 12)goto _LLCA;_tmpE8=((struct Cyc_Absyn_Throw_e_struct*)
_tmpC9)->f1;_LLC9: {union Cyc_CfFlowInfo_FlowInfo_union _tmp18F;void*_tmp190;
struct _tuple5 _tmp18E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpE8);_tmp18F=
_tmp18E.f1;_tmp190=_tmp18E.f2;_tmp18F=Cyc_CfFlowInfo_consume_unique_rvals(_tmpE8->loc,
_tmp18F);Cyc_NewControlFlow_use_Rval(env,_tmpE8->loc,_tmp18F,_tmp190);return({
struct _tuple5 _tmp191;_tmp191.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp192;(_tmp192.BottomFL).tag=0;_tmp192;});_tmp191.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp191;});}_LLCA: if(*((int*)_tmpC9)!= 11)goto _LLCC;_tmpE9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpC9)->f1;_tmpEA=((struct Cyc_Absyn_FnCall_e_struct*)_tmpC9)->f2;_LLCB: {struct
_RegionHandle _tmp193=_new_region("temp");struct _RegionHandle*temp=& _tmp193;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp196;struct Cyc_List_List*
_tmp197;struct _tuple9 _tmp195=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp194=_region_malloc(temp,sizeof(*_tmp194));
_tmp194->hd=_tmpE9;_tmp194->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmpEA);_tmp194;}),1);_tmp196=
_tmp195.f1;_tmp197=_tmp195.f2;_tmp196=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp196);{union Cyc_CfFlowInfo_FlowInfo_union _tmp198=Cyc_NewControlFlow_use_Rval(
env,_tmpE9->loc,_tmp196,(void*)((struct Cyc_List_List*)_check_null(_tmp197))->hd);
_tmp197=_tmp197->tl;{struct Cyc_List_List*init_params=0;{void*_tmp199=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp19A;void*_tmp19B;_LL122: if(_tmp199 <= (void*)4)goto _LL124;if(*((int*)_tmp199)
!= 4)goto _LL124;_tmp19A=((struct Cyc_Absyn_PointerType_struct*)_tmp199)->f1;
_tmp19B=(void*)_tmp19A.elt_typ;_LL123:{void*_tmp19C=Cyc_Tcutil_compress(_tmp19B);
struct Cyc_Absyn_FnInfo _tmp19D;struct Cyc_List_List*_tmp19E;_LL127: if(_tmp19C <= (
void*)4)goto _LL129;if(*((int*)_tmp19C)!= 8)goto _LL129;_tmp19D=((struct Cyc_Absyn_FnType_struct*)
_tmp19C)->f1;_tmp19E=_tmp19D.attributes;_LL128: for(0;_tmp19E != 0;_tmp19E=_tmp19E->tl){
void*_tmp19F=(void*)_tmp19E->hd;int _tmp1A0;_LL12C: if(_tmp19F <= (void*)17)goto
_LL12E;if(*((int*)_tmp19F)!= 4)goto _LL12E;_tmp1A0=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp19F)->f1;_LL12D: init_params=({struct Cyc_List_List*_tmp1A1=_region_malloc(
temp,sizeof(*_tmp1A1));_tmp1A1->hd=(void*)_tmp1A0;_tmp1A1->tl=init_params;
_tmp1A1;});goto _LL12B;_LL12E:;_LL12F: goto _LL12B;_LL12B:;}goto _LL126;_LL129:;
_LL12A:({void*_tmp1A2[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A3="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,29));}),
_tag_dynforward(_tmp1A2,sizeof(void*),0));});_LL126:;}goto _LL121;_LL124:;_LL125:({
void*_tmp1A4[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1A5="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,29));}),
_tag_dynforward(_tmp1A4,sizeof(void*),0));});_LL121:;}{int i=1;for(0;_tmp197 != 0;(((
_tmp197=_tmp197->tl,_tmpEA=((struct Cyc_List_List*)_check_null(_tmpEA))->tl)),++ i)){
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp198=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpEA))->hd)->loc,_tmp198,(void*)_tmp197->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A6=_tmp196;struct Cyc_Dict_Dict _tmp1A7;_LL131: if((_tmp1A6.BottomFL).tag != 0)
goto _LL133;_LL132: goto _LL130;_LL133: if((_tmp1A6.ReachableFL).tag != 1)goto _LL130;
_tmp1A7=(_tmp1A6.ReachableFL).f1;_LL134: if(Cyc_CfFlowInfo_initlevel(_tmp1A7,(
void*)_tmp197->hd)== (void*)0)({void*_tmp1A8[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpEA))->hd)->loc,({const char*_tmp1A9="expression may not be initialized";
_tag_dynforward(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,34));}),
_tag_dynforward(_tmp1A8,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AA=_tmp198;struct Cyc_Dict_Dict _tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1AD;_LL136: if((_tmp1AA.BottomFL).tag != 0)goto _LL138;_LL137: goto _LL135;_LL138:
if((_tmp1AA.ReachableFL).tag != 1)goto _LL135;_tmp1AB=(_tmp1AA.ReachableFL).f1;
_tmp1AC=(_tmp1AA.ReachableFL).f2;_tmp1AD=(_tmp1AA.ReachableFL).f3;_LL139: {
struct Cyc_Dict_Dict _tmp1AE=Cyc_CfFlowInfo_escape_deref(_tmp1AB,env->all_changed,(
void*)_tmp197->hd);{void*_tmp1AF=(void*)_tmp197->hd;struct Cyc_CfFlowInfo_Place*
_tmp1B0;_LL13B: if(_tmp1AF <= (void*)3)goto _LL13D;if(*((int*)_tmp1AF)!= 2)goto
_LL13D;_tmp1B0=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1AF)->f1;_LL13C:{
void*_tmp1B1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpEA))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1B2;void*_tmp1B3;_LL140: if(_tmp1B1 <= (void*)4)goto
_LL142;if(*((int*)_tmp1B1)!= 4)goto _LL142;_tmp1B2=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B1)->f1;_tmp1B3=(void*)_tmp1B2.elt_typ;_LL141: _tmp1AE=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,_tmp1AE,env->all_changed,_tmp1B0,Cyc_CfFlowInfo_typ_to_absrval(
_tmp1B3,Cyc_CfFlowInfo_esc_all));goto _LL13F;_LL142:;_LL143:({void*_tmp1B4[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1B5="anal_Rexp:bad type for initialized arg";_tag_dynforward(
_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,39));}),_tag_dynforward(_tmp1B4,
sizeof(void*),0));});_LL13F:;}goto _LL13A;_LL13D:;_LL13E: goto _LL13A;_LL13A:;}
_tmp198=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B6;(_tmp1B6.ReachableFL).tag=1;(_tmp1B6.ReachableFL).f1=_tmp1AE;(_tmp1B6.ReachableFL).f2=
_tmp1AC;(_tmp1B6.ReachableFL).f3=_tmp1AD;_tmp1B6;});goto _LL135;}_LL135:;}goto
_LL130;_LL130:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE9->topt))->v)){struct _tuple5 _tmp1B9=({struct _tuple5 _tmp1B7;
_tmp1B7.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B8;(_tmp1B8.BottomFL).tag=0;_tmp1B8;});_tmp1B7.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1B7;});_npop_handler(0);return _tmp1B9;}else{struct _tuple5 _tmp1BB=({struct
_tuple5 _tmp1BA;_tmp1BA.f1=_tmp198;_tmp1BA.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1BA;});
_npop_handler(0);return _tmp1BB;}}}};_pop_region(temp);}_LLCC: if(*((int*)_tmpC9)
!= 35)goto _LLCE;_tmpEB=((struct Cyc_Absyn_Malloc_e_struct*)_tmpC9)->f1;_tmpEC=
_tmpEB.is_calloc;_tmpED=_tmpEB.rgn;_tmpEE=_tmpEB.elt_type;_tmpEF=_tmpEB.num_elts;
_tmpF0=_tmpEB.fat_result;_LLCD: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=_tmpEF;_tmp1CE.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1CE;});_tmp1CD;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->root=(
void*)root;_tmp1CC->fields=0;_tmp1CC;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1CB;_tmp1CB.tag=2;_tmp1CB.f1=place;_tmp1CB;});_tmp1CA;});void*place_val=
_tmpF0?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpEE)),Cyc_CfFlowInfo_unknown_none);
union Cyc_CfFlowInfo_FlowInfo_union outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);if(_tmpED != 0){struct _RegionHandle _tmp1BC=_new_region("temp");struct
_RegionHandle*temp=& _tmp1BC;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BF;struct Cyc_List_List*_tmp1C0;struct _tuple9 _tmp1BE=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1BD[2];_tmp1BD[1]=_tmpEF;_tmp1BD[0]=(
struct Cyc_Absyn_Exp*)_tmpED;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(_tmp1BD,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp1BF=_tmp1BE.f1;_tmp1C0=_tmp1BE.f2;outflow=
_tmp1BF;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C2;
struct _tuple5 _tmp1C1=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpEF);_tmp1C2=
_tmp1C1.f1;outflow=_tmp1C2;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEF->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C3=outflow;struct
Cyc_Dict_Dict _tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1C6;_LL145: if((_tmp1C3.BottomFL).tag != 0)goto _LL147;_LL146: return({struct
_tuple5 _tmp1C7;_tmp1C7.f1=outflow;_tmp1C7.f2=rval;_tmp1C7;});_LL147: if((_tmp1C3.ReachableFL).tag
!= 1)goto _LL144;_tmp1C4=(_tmp1C3.ReachableFL).f1;_tmp1C5=(_tmp1C3.ReachableFL).f2;
_tmp1C6=(_tmp1C3.ReachableFL).f3;_LL148: return({struct _tuple5 _tmp1C8;_tmp1C8.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1C9;(
_tmp1C9.ReachableFL).tag=1;(_tmp1C9.ReachableFL).f1=Cyc_Dict_insert(_tmp1C4,root,
place_val);(_tmp1C9.ReachableFL).f2=_tmp1C5;(_tmp1C9.ReachableFL).f3=_tmp1C6;
_tmp1C9;});_tmp1C8.f2=rval;_tmp1C8;});_LL144:;}}_LLCE: if(*((int*)_tmpC9)!= 36)
goto _LLD0;_tmpF1=((struct Cyc_Absyn_Swap_e_struct*)_tmpC9)->f1;_tmpF2=((struct Cyc_Absyn_Swap_e_struct*)
_tmpC9)->f2;_LLCF: {void*left_rval;void*right_rval;union Cyc_CfFlowInfo_FlowInfo_union
outflow;{struct _RegionHandle _tmp1CF=_new_region("temp");struct _RegionHandle*temp=&
_tmp1CF;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D2;struct Cyc_List_List*
_tmp1D3;struct _tuple9 _tmp1D1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1D0[2];_tmp1D0[1]=_tmpF2;_tmp1D0[0]=_tmpF1;((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp1D0,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1D2=
_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;left_rval=(void*)((struct Cyc_List_List*)
_check_null(_tmp1D3))->hd;right_rval=(void*)((struct Cyc_List_List*)_check_null(
_tmp1D3->tl))->hd;outflow=_tmp1D2;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,outflow);{void*_tmp1D5;struct _tuple5 _tmp1D4=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmpF1);_tmp1D5=_tmp1D4.f2;{void*_tmp1D7;struct _tuple5 _tmp1D6=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmpF2);_tmp1D7=_tmp1D6.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D8=outflow;struct Cyc_Dict_Dict _tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1DB;_LL14A: if((_tmp1D8.ReachableFL).tag != 1)goto _LL14C;_tmp1D9=(_tmp1D8.ReachableFL).f1;
_tmp1DA=(_tmp1D8.ReachableFL).f2;_tmp1DB=(_tmp1D8.ReachableFL).f3;_LL14B:{void*
_tmp1DC=_tmp1D5;struct Cyc_CfFlowInfo_Place*_tmp1DD;_LL14F: if(_tmp1DC <= (void*)1)
goto _LL151;if(*((int*)_tmp1DC)!= 0)goto _LL151;_tmp1DD=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp1DC)->f1;_LL150: _tmp1D9=Cyc_CfFlowInfo_assign_place(_tmpF1->loc,_tmp1D9,env->all_changed,
_tmp1DD,right_rval);goto _LL14E;_LL151: if((int)_tmp1DC != 0)goto _LL14E;_LL152: goto
_LL14E;_LL14E:;}{void*_tmp1DE=_tmp1D7;struct Cyc_CfFlowInfo_Place*_tmp1DF;_LL154:
if(_tmp1DE <= (void*)1)goto _LL156;if(*((int*)_tmp1DE)!= 0)goto _LL156;_tmp1DF=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp1DE)->f1;_LL155: _tmp1D9=Cyc_CfFlowInfo_assign_place(
_tmpF2->loc,_tmp1D9,env->all_changed,_tmp1DF,left_rval);goto _LL153;_LL156: if((
int)_tmp1DE != 0)goto _LL153;_LL157: goto _LL153;_LL153:;}_tmp1DA=Cyc_CfFlowInfo_reln_kill_exp(
_tmp1DA,_tmpF1);_tmp1DA=Cyc_CfFlowInfo_reln_kill_exp(_tmp1DA,_tmpF2);outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1E0;(
_tmp1E0.ReachableFL).tag=1;(_tmp1E0.ReachableFL).f1=_tmp1D9;(_tmp1E0.ReachableFL).f2=
_tmp1DA;(_tmp1E0.ReachableFL).f3=_tmp1DB;_tmp1E0;});goto _LL149;_LL14C:;_LL14D:
goto _LL149;_LL149:;}return({struct _tuple5 _tmp1E1;_tmp1E1.f1=outflow;_tmp1E1.f2=
Cyc_CfFlowInfo_unknown_all;_tmp1E1;});}}}_LLD0: if(*((int*)_tmpC9)!= 17)goto _LLD2;
_tmpF3=((struct Cyc_Absyn_New_e_struct*)_tmpC9)->f1;_tmpF4=((struct Cyc_Absyn_New_e_struct*)
_tmpC9)->f2;_LLD1: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1F5;_tmp1F5.tag=1;_tmp1F5.f1=_tmpF4;_tmp1F5.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1F5;});_tmp1F4;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->root=(
void*)root;_tmp1F3->fields=0;_tmp1F3;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1F2;_tmp1F2.tag=2;_tmp1F2.f1=place;_tmp1F2;});_tmp1F1;});Cyc_CfFlowInfo_update_place_set(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*
place_val;if(_tmpF3 != 0){struct _RegionHandle _tmp1E2=_new_region("temp");struct
_RegionHandle*temp=& _tmp1E2;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E5;struct Cyc_List_List*_tmp1E6;struct _tuple9 _tmp1E4=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1E3[2];_tmp1E3[1]=_tmpF4;_tmp1E3[0]=(
struct Cyc_Absyn_Exp*)_tmpF3;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(_tmp1E3,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp1E5=_tmp1E4.f1;_tmp1E6=_tmp1E4.f2;outflow=
_tmp1E5;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp1E6))->tl))->hd;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E8;void*_tmp1E9;struct _tuple5 _tmp1E7=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpF4);_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;outflow=_tmp1E8;place_val=
_tmp1E9;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpF4->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1EA=outflow;struct Cyc_Dict_Dict _tmp1EB;
struct Cyc_List_List*_tmp1EC;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1ED;_LL159: if((
_tmp1EA.BottomFL).tag != 0)goto _LL15B;_LL15A: return({struct _tuple5 _tmp1EE;_tmp1EE.f1=
outflow;_tmp1EE.f2=rval;_tmp1EE;});_LL15B: if((_tmp1EA.ReachableFL).tag != 1)goto
_LL158;_tmp1EB=(_tmp1EA.ReachableFL).f1;_tmp1EC=(_tmp1EA.ReachableFL).f2;_tmp1ED=(
_tmp1EA.ReachableFL).f3;_LL15C: return({struct _tuple5 _tmp1EF;_tmp1EF.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1F0;(_tmp1F0.ReachableFL).tag=1;(_tmp1F0.ReachableFL).f1=
Cyc_Dict_insert(_tmp1EB,root,place_val);(_tmp1F0.ReachableFL).f2=_tmp1EC;(
_tmp1F0.ReachableFL).f3=_tmp1ED;_tmp1F0;});_tmp1EF.f2=rval;_tmp1EF;});_LL158:;}}}
_LLD2: if(*((int*)_tmpC9)!= 16)goto _LLD4;_tmpF5=((struct Cyc_Absyn_Address_e_struct*)
_tmpC9)->f1;_LLD3: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1F7;void*_tmp1F8;
struct _tuple5 _tmp1F6=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmpF5);_tmp1F7=
_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;{void*_tmp1F9=_tmp1F8;struct Cyc_CfFlowInfo_Place*
_tmp1FA;_LL15E: if((int)_tmp1F9 != 0)goto _LL160;_LL15F: return({struct _tuple5
_tmp1FB;_tmp1FB.f1=_tmp1F7;_tmp1FB.f2=(void*)1;_tmp1FB;});_LL160: if(_tmp1F9 <= (
void*)1)goto _LL15D;if(*((int*)_tmp1F9)!= 0)goto _LL15D;_tmp1FA=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp1F9)->f1;_LL161: return({struct _tuple5 _tmp1FC;_tmp1FC.f1=_tmp1F7;_tmp1FC.f2=(
void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));
_tmp1FD[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1FE;_tmp1FE.tag=2;_tmp1FE.f1=
_tmp1FA;_tmp1FE;});_tmp1FD;});_tmp1FC;});_LL15D:;}}_LLD4: if(*((int*)_tmpC9)!= 22)
goto _LLD6;_tmpF6=((struct Cyc_Absyn_Deref_e_struct*)_tmpC9)->f1;_LLD5: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp200;void*_tmp201;struct _tuple5 _tmp1FF=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpF6);_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;_tmp200=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp200);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp200,_tmpF6,
_tmp201);}_LLD6: if(*((int*)_tmpC9)!= 23)goto _LLD8;_tmpF7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpC9)->f1;_tmpF8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpC9)->f2;_LLD7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp203;void*_tmp204;struct _tuple5 _tmp202=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF7);_tmp203=_tmp202.f1;_tmp204=_tmp202.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF7->topt))->v))return({struct _tuple5
_tmp205;_tmp205.f1=_tmp203;_tmp205.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp205;});{
void*_tmp206=_tmp204;struct _dynforward_ptr _tmp207;_LL163: if(_tmp206 <= (void*)3)
goto _LL165;if(*((int*)_tmp206)!= 4)goto _LL165;_tmp207=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp206)->f1;_LL164: {int _tmp208=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF7->topt))->v,_tmpF8);return({struct _tuple5
_tmp209;_tmp209.f1=_tmp203;_tmp209.f2=*((void**)_check_dynforward_subscript(
_tmp207,sizeof(void*),_tmp208));_tmp209;});}_LL165:;_LL166:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=({
struct Cyc_Core_Impossible_struct _tmp20B;_tmp20B.tag=Cyc_Core_Impossible;_tmp20B.f1=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp20E;_tmp20E.tag=0;_tmp20E.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp20C[1]={& _tmp20E};Cyc_aprintf(({const char*_tmp20D="anal_Rexp: AggrMember: %s";
_tag_dynforward(_tmp20D,sizeof(char),_get_zero_arr_size(_tmp20D,26));}),
_tag_dynforward(_tmp20C,sizeof(void*),1));}});_tmp20B;});_tmp20A;}));_LL162:;}}
_LLD8: if(*((int*)_tmpC9)!= 24)goto _LLDA;_tmpF9=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpC9)->f1;_tmpFA=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpC9)->f2;_LLD9: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp210;void*_tmp211;struct _tuple5 _tmp20F=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF9);_tmp210=_tmp20F.f1;_tmp211=_tmp20F.f2;_tmp210=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp210);{union Cyc_CfFlowInfo_FlowInfo_union _tmp213;void*_tmp214;struct
_tuple5 _tmp212=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp210,_tmpF9,_tmp211);
_tmp213=_tmp212.f1;_tmp214=_tmp212.f2;{void*_tmp215=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF9->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp216;void*_tmp217;_LL168: if(_tmp215 <= (void*)4)goto _LL16A;if(*((int*)_tmp215)
!= 4)goto _LL16A;_tmp216=((struct Cyc_Absyn_PointerType_struct*)_tmp215)->f1;
_tmp217=(void*)_tmp216.elt_typ;_LL169: if(Cyc_Absyn_is_union_type(_tmp217))return({
struct _tuple5 _tmp218;_tmp218.f1=_tmp213;_tmp218.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp218;});{void*_tmp219=_tmp214;struct _dynforward_ptr _tmp21A;_LL16D: if(_tmp219
<= (void*)3)goto _LL16F;if(*((int*)_tmp219)!= 4)goto _LL16F;_tmp21A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp219)->f1;_LL16E: {int _tmp21B=Cyc_NewControlFlow_get_field_index(_tmp217,
_tmpFA);return({struct _tuple5 _tmp21C;_tmp21C.f1=_tmp213;_tmp21C.f2=*((void**)
_check_dynforward_subscript(_tmp21A,sizeof(void*),_tmp21B));_tmp21C;});}_LL16F:;
_LL170:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21D=_cycalloc(
sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Core_Impossible_struct _tmp21E;_tmp21E.tag=
Cyc_Core_Impossible;_tmp21E.f1=({const char*_tmp21F="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,21));});_tmp21E;});
_tmp21D;}));_LL16C:;}_LL16A:;_LL16B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Core_Impossible_struct
_tmp221;_tmp221.tag=Cyc_Core_Impossible;_tmp221.f1=({const char*_tmp222="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,25));});_tmp221;});
_tmp220;}));_LL167:;}}}_LLDA: if(*((int*)_tmpC9)!= 6)goto _LLDC;_tmpFB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpC9)->f1;_tmpFC=((struct Cyc_Absyn_Conditional_e_struct*)_tmpC9)->f2;_tmpFD=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpC9)->f3;_LLDB: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp224;union Cyc_CfFlowInfo_FlowInfo_union _tmp225;struct _tuple7 _tmp223=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpFB);_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;_tmp224=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpFB->loc,_tmp224);_tmp225=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFB->loc,
_tmp225);{struct _tuple5 _tmp226=Cyc_NewControlFlow_anal_Rexp(env,_tmp224,_tmpFC);
struct _tuple5 _tmp227=Cyc_NewControlFlow_anal_Rexp(env,_tmp225,_tmpFD);return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp226,_tmp227,1);}}_LLDC: if(*((int*)_tmpC9)!= 7)goto _LLDE;
_tmpFE=((struct Cyc_Absyn_And_e_struct*)_tmpC9)->f1;_tmpFF=((struct Cyc_Absyn_And_e_struct*)
_tmpC9)->f2;_LLDD: {union Cyc_CfFlowInfo_FlowInfo_union _tmp229;union Cyc_CfFlowInfo_FlowInfo_union
_tmp22A;struct _tuple7 _tmp228=Cyc_NewControlFlow_anal_test(env,inflow,_tmpFE);
_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;_tmp229=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpFE->loc,_tmp229);_tmp22A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFE->loc,
_tmp22A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22C;void*_tmp22D;struct _tuple5
_tmp22B=Cyc_NewControlFlow_anal_Rexp(env,_tmp229,_tmpFF);_tmp22C=_tmp22B.f1;
_tmp22D=_tmp22B.f2;_tmp22C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpFF->loc,
_tmp22C);{struct _tuple5 _tmp22E=({struct _tuple5 _tmp231;_tmp231.f1=_tmp22C;_tmp231.f2=
_tmp22D;_tmp231;});struct _tuple5 _tmp22F=({struct _tuple5 _tmp230;_tmp230.f1=
_tmp22A;_tmp230.f2=(void*)((void*)0);_tmp230;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp22E,_tmp22F,0);}}}_LLDE: if(*((int*)_tmpC9)!= 8)goto _LLE0;
_tmp100=((struct Cyc_Absyn_Or_e_struct*)_tmpC9)->f1;_tmp101=((struct Cyc_Absyn_Or_e_struct*)
_tmpC9)->f2;_LLDF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp233;union Cyc_CfFlowInfo_FlowInfo_union
_tmp234;struct _tuple7 _tmp232=Cyc_NewControlFlow_anal_test(env,inflow,_tmp100);
_tmp233=_tmp232.f1;_tmp234=_tmp232.f2;_tmp233=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp100->loc,_tmp233);_tmp234=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp100->loc,
_tmp234);{union Cyc_CfFlowInfo_FlowInfo_union _tmp236;void*_tmp237;struct _tuple5
_tmp235=Cyc_NewControlFlow_anal_Rexp(env,_tmp234,_tmp101);_tmp236=_tmp235.f1;
_tmp237=_tmp235.f2;_tmp236=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp101->loc,
_tmp236);{struct _tuple5 _tmp238=({struct _tuple5 _tmp23B;_tmp23B.f1=_tmp236;_tmp23B.f2=
_tmp237;_tmp23B;});struct _tuple5 _tmp239=({struct _tuple5 _tmp23A;_tmp23A.f1=
_tmp233;_tmp23A.f2=(void*)((void*)1);_tmp23A;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp238,_tmp239,0);}}}_LLE0: if(*((int*)_tmpC9)!= 25)goto _LLE2;
_tmp102=((struct Cyc_Absyn_Subscript_e_struct*)_tmpC9)->f1;_tmp103=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpC9)->f2;_LLE1: {struct _RegionHandle _tmp23C=_new_region("temp");struct
_RegionHandle*temp=& _tmp23C;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp23F;struct Cyc_List_List*_tmp240;struct _tuple9 _tmp23E=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp23D[2];_tmp23D[1]=_tmp103;_tmp23D[0]=
_tmp102;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp23D,sizeof(struct Cyc_Absyn_Exp*),2));}),
1);_tmp23F=_tmp23E.f1;_tmp240=_tmp23E.f2;_tmp23F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp103->loc,_tmp23F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp241=_tmp23F;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp242=_tmp23F;struct Cyc_Dict_Dict _tmp243;struct
Cyc_List_List*_tmp244;struct Cyc_CfFlowInfo_ConsumeInfo _tmp245;_LL172: if((_tmp242.ReachableFL).tag
!= 1)goto _LL174;_tmp243=(_tmp242.ReachableFL).f1;_tmp244=(_tmp242.ReachableFL).f2;
_tmp245=(_tmp242.ReachableFL).f3;_LL173: if(Cyc_CfFlowInfo_initlevel(_tmp243,(
void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(
_tmp240))->tl))->hd)== (void*)0)({void*_tmp246[0]={};Cyc_Tcutil_terr(_tmp103->loc,({
const char*_tmp247="expression may not be initialized";_tag_dynforward(_tmp247,
sizeof(char),_get_zero_arr_size(_tmp247,34));}),_tag_dynforward(_tmp246,sizeof(
void*),0));});{struct Cyc_List_List*_tmp248=Cyc_NewControlFlow_add_subscript_reln(
_tmp244,_tmp102,_tmp103);if(_tmp244 != _tmp248)_tmp241=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp249;(_tmp249.ReachableFL).tag=1;(_tmp249.ReachableFL).f1=
_tmp243;(_tmp249.ReachableFL).f2=_tmp248;(_tmp249.ReachableFL).f3=_tmp245;
_tmp249;});goto _LL171;}_LL174:;_LL175: goto _LL171;_LL171:;}{void*_tmp24A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp102->topt))->v);struct Cyc_List_List*
_tmp24B;struct Cyc_Absyn_PtrInfo _tmp24C;struct Cyc_Absyn_PtrAtts _tmp24D;struct Cyc_Absyn_Conref*
_tmp24E;_LL177: if(_tmp24A <= (void*)4)goto _LL17B;if(*((int*)_tmp24A)!= 9)goto
_LL179;_tmp24B=((struct Cyc_Absyn_TupleType_struct*)_tmp24A)->f1;_LL178: {void*
_tmp24F=(void*)((struct Cyc_List_List*)_check_null(_tmp240))->hd;struct
_dynforward_ptr _tmp250;_LL17E: if(_tmp24F <= (void*)3)goto _LL180;if(*((int*)
_tmp24F)!= 4)goto _LL180;_tmp250=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp24F)->f1;
_LL17F: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp103)).f1;struct _tuple5
_tmp252=({struct _tuple5 _tmp251;_tmp251.f1=_tmp241;_tmp251.f2=*((void**)
_check_dynforward_subscript(_tmp250,sizeof(void*),(int)i));_tmp251;});
_npop_handler(0);return _tmp252;}_LL180:;_LL181:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253[0]=({struct Cyc_Core_Impossible_struct
_tmp254;_tmp254.tag=Cyc_Core_Impossible;_tmp254.f1=({const char*_tmp255="anal_Rexp: Subscript";
_tag_dynforward(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,21));});_tmp254;});
_tmp253;}));_LL17D:;}_LL179: if(*((int*)_tmp24A)!= 4)goto _LL17B;_tmp24C=((struct
Cyc_Absyn_PointerType_struct*)_tmp24A)->f1;_tmp24D=_tmp24C.ptr_atts;_tmp24E=
_tmp24D.bounds;_LL17A:{union Cyc_Absyn_Constraint_union _tmp256=(Cyc_Absyn_compress_conref(
_tmp24E))->v;void*_tmp257;_LL183: if((_tmp256.Eq_constr).tag != 0)goto _LL185;
_tmp257=(_tmp256.Eq_constr).f1;if(_tmp257 <= (void*)2)goto _LL185;if(*((int*)
_tmp257)!= 1)goto _LL185;_LL184:{void*_tmp258=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp240))->tl))->hd;struct Cyc_List_List*
_tmp259;_LL188: if(_tmp258 <= (void*)3)goto _LL18A;if(*((int*)_tmp258)!= 3)goto
_LL18A;_tmp259=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp258)->f1;_LL189:(void*)(
_tmp103->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp25A=
_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp25B;_tmp25B.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp25B.f1=_tmp259;_tmp25B;});
_tmp25A;})));goto _LL187;_LL18A:;_LL18B: goto _LL187;_LL187:;}goto _LL182;_LL185:;
_LL186: goto _LL182;_LL182:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp25D;void*
_tmp25E;struct _tuple5 _tmp25C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp23F,
_tmp102,(void*)((struct Cyc_List_List*)_check_null(_tmp240))->hd);_tmp25D=_tmp25C.f1;
_tmp25E=_tmp25C.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp25F=_tmp25D;_LL18D: if((
_tmp25F.BottomFL).tag != 0)goto _LL18F;_LL18E: {struct _tuple5 _tmp261=({struct
_tuple5 _tmp260;_tmp260.f1=_tmp25D;_tmp260.f2=_tmp25E;_tmp260;});_npop_handler(0);
return _tmp261;}_LL18F:;_LL190: {struct _tuple5 _tmp263=({struct _tuple5 _tmp262;
_tmp262.f1=_tmp241;_tmp262.f2=_tmp25E;_tmp262;});_npop_handler(0);return _tmp263;}
_LL18C:;}}_LL17B:;_LL17C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Core_Impossible_struct
_tmp265;_tmp265.tag=Cyc_Core_Impossible;_tmp265.f1=({const char*_tmp266="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,33));});_tmp265;});
_tmp264;}));_LL176:;}}};_pop_region(temp);}_LLE2: if(*((int*)_tmpC9)!= 32)goto
_LLE4;_tmp104=((struct Cyc_Absyn_Tunion_e_struct*)_tmpC9)->f1;_tmp105=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpC9)->f2;_LLE3: if(_tmp105->is_flat){struct _RegionHandle _tmp267=_new_region("temp");
struct _RegionHandle*temp=& _tmp267;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp269;struct Cyc_List_List*_tmp26A;struct _tuple9 _tmp268=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp104,0);_tmp269=_tmp268.f1;_tmp26A=_tmp268.f2;_tmp269=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp269);for(0;(unsigned int)_tmp104;(_tmp104=_tmp104->tl,_tmp26A=_tmp26A->tl)){
_tmp269=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp104->hd)->loc,
_tmp269,(void*)((struct Cyc_List_List*)_check_null(_tmp26A))->hd);}{struct _tuple5
_tmp26C=({struct _tuple5 _tmp26B;_tmp26B.f1=_tmp269;_tmp26B.f2=Cyc_CfFlowInfo_unknown_all;
_tmp26B;});_npop_handler(0);return _tmp26C;}};_pop_region(temp);}_tmp106=_tmp104;
goto _LLE5;_LLE4: if(*((int*)_tmpC9)!= 26)goto _LLE6;_tmp106=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpC9)->f1;_LLE5: {struct _RegionHandle _tmp26D=_new_region("temp");struct
_RegionHandle*temp=& _tmp26D;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp26F;struct Cyc_List_List*_tmp270;struct _tuple9 _tmp26E=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp106,0);_tmp26F=_tmp26E.f1;_tmp270=_tmp26E.f2;_tmp26F=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp26F);{struct _dynforward_ptr aggrdict=({unsigned int _tmp275=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp106);void**
_tmp276=(void**)_cycalloc(_check_times(sizeof(void*),_tmp275));struct
_dynforward_ptr _tmp279=_tag_dynforward(_tmp276,sizeof(void*),_tmp275);{
unsigned int _tmp277=_tmp275;unsigned int i;for(i=0;i < _tmp277;i ++){_tmp276[i]=({
void*_tmp278=(void*)((struct Cyc_List_List*)_check_null(_tmp270))->hd;_tmp270=
_tmp270->tl;_tmp278;});}}_tmp279;});struct _tuple5 _tmp274=({struct _tuple5 _tmp271;
_tmp271.f1=_tmp26F;_tmp271.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp273;_tmp273.tag=4;_tmp273.f1=aggrdict;_tmp273;});_tmp272;});_tmp271;});
_npop_handler(0);return _tmp274;}};_pop_region(temp);}_LLE6: if(*((int*)_tmpC9)!= 
31)goto _LLE8;_tmp107=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpC9)->f2;_LLE7: {
struct Cyc_List_List*fs;{void*_tmp27A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp27B;_LL192: if(_tmp27A <= (void*)
4)goto _LL194;if(*((int*)_tmp27A)!= 11)goto _LL194;_tmp27B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27A)->f2;_LL193: fs=_tmp27B;goto _LL191;_LL194:;_LL195:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C[0]=({
struct Cyc_Core_Impossible_struct _tmp27D;_tmp27D.tag=Cyc_Core_Impossible;_tmp27D.f1=({
const char*_tmp27E="anal_Rexp:anon struct has bad type";_tag_dynforward(_tmp27E,
sizeof(char),_get_zero_arr_size(_tmp27E,35));});_tmp27D;});_tmp27C;}));_LL191:;}
_tmp108=_tmp107;_tmp10D=fs;goto _LLE9;}_LLE8: if(*((int*)_tmpC9)!= 30)goto _LLEA;
_tmp108=((struct Cyc_Absyn_Struct_e_struct*)_tmpC9)->f3;_tmp109=((struct Cyc_Absyn_Struct_e_struct*)
_tmpC9)->f4;if(_tmp109 == 0)goto _LLEA;_tmp10A=*_tmp109;_tmp10B=_tmp10A.impl;if(
_tmp10B == 0)goto _LLEA;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.fields;_LLE9: {struct
_RegionHandle _tmp27F=_new_region("temp");struct _RegionHandle*temp=& _tmp27F;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp281;struct Cyc_List_List*
_tmp282;struct _tuple9 _tmp280=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple11*))Cyc_Core_snd,_tmp108),0);_tmp281=_tmp280.f1;_tmp282=_tmp280.f2;
_tmp281=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp281);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp10D,Cyc_CfFlowInfo_unknown_all);{
int i=0;for(0;_tmp282 != 0;(((_tmp282=_tmp282->tl,_tmp108=_tmp108->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(
_tmp108))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp283=(void*)ds->hd;struct
_dynforward_ptr*_tmp284;_LL197: if(*((int*)_tmp283)!= 0)goto _LL199;_LL198:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp285=_cycalloc(sizeof(*
_tmp285));_tmp285[0]=({struct Cyc_Core_Impossible_struct _tmp286;_tmp286.tag=Cyc_Core_Impossible;
_tmp286.f1=({const char*_tmp287="anal_Rexp:Struct_e";_tag_dynforward(_tmp287,
sizeof(char),_get_zero_arr_size(_tmp287,19));});_tmp286;});_tmp285;}));_LL199:
if(*((int*)_tmp283)!= 1)goto _LL196;_tmp284=((struct Cyc_Absyn_FieldName_struct*)
_tmp283)->f1;_LL19A: {int _tmp288=Cyc_NewControlFlow_get_field_index_fs(_tmp10D,
_tmp284);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp288))=(
void*)_tmp282->hd;}_LL196:;}}}{struct _tuple5 _tmp28C=({struct _tuple5 _tmp289;
_tmp289.f1=_tmp281;_tmp289.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp28B;_tmp28B.tag=4;_tmp28B.f1=aggrdict;_tmp28B;});_tmp28A;});_tmp289;});
_npop_handler(0);return _tmp28C;}}};_pop_region(temp);}_LLEA: if(*((int*)_tmpC9)!= 
30)goto _LLEC;_LLEB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp28D=
_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({struct Cyc_Core_Impossible_struct _tmp28E;
_tmp28E.tag=Cyc_Core_Impossible;_tmp28E.f1=({const char*_tmp28F="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,31));});_tmp28E;});
_tmp28D;}));_LLEC: if(*((int*)_tmpC9)!= 28)goto _LLEE;_tmp10E=((struct Cyc_Absyn_Array_e_struct*)
_tmpC9)->f1;_LLED: {struct _RegionHandle _tmp290=_new_region("temp");struct
_RegionHandle*temp=& _tmp290;_push_region(temp);{struct Cyc_List_List*_tmp291=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple11*))Cyc_Core_snd,_tmp10E);union Cyc_CfFlowInfo_FlowInfo_union _tmp293;
struct Cyc_List_List*_tmp294;struct _tuple9 _tmp292=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp291,0);_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;_tmp293=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp293);for(0;_tmp294 != 0;(_tmp294=_tmp294->tl,_tmp291=_tmp291->tl)){
_tmp293=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp291))->hd)->loc,_tmp293,(void*)_tmp294->hd);}{struct _tuple5
_tmp296=({struct _tuple5 _tmp295;_tmp295.f1=_tmp293;_tmp295.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp295;});_npop_handler(0);return _tmp296;}};_pop_region(temp);}_LLEE: if(*((int*)
_tmpC9)!= 29)goto _LLF0;_tmp10F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpC9)->f1;
_tmp110=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpC9)->f2;_tmp111=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmpC9)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpC9)->f4;_LLEF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp298;void*_tmp299;
struct _tuple5 _tmp297=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp110);_tmp298=
_tmp297.f1;_tmp299=_tmp297.f2;_tmp298=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp110->loc,_tmp298);{union Cyc_CfFlowInfo_FlowInfo_union _tmp29A=_tmp298;struct
Cyc_Dict_Dict _tmp29B;struct Cyc_List_List*_tmp29C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp29D;_LL19C: if((_tmp29A.BottomFL).tag != 0)goto _LL19E;_LL19D: return({struct
_tuple5 _tmp29E;_tmp29E.f1=_tmp298;_tmp29E.f2=Cyc_CfFlowInfo_unknown_all;_tmp29E;});
_LL19E: if((_tmp29A.ReachableFL).tag != 1)goto _LL19B;_tmp29B=(_tmp29A.ReachableFL).f1;
_tmp29C=(_tmp29A.ReachableFL).f2;_tmp29D=(_tmp29A.ReachableFL).f3;_LL19F: if(Cyc_CfFlowInfo_initlevel(
_tmp29B,_tmp299)== (void*)0)({void*_tmp29F[0]={};Cyc_Tcutil_terr(_tmp110->loc,({
const char*_tmp2A0="expression may not be initialized";_tag_dynforward(_tmp2A0,
sizeof(char),_get_zero_arr_size(_tmp2A0,34));}),_tag_dynforward(_tmp29F,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp29C;comp_loop: {void*_tmp2A1=(
void*)_tmp110->r;struct Cyc_Absyn_Exp*_tmp2A2;void*_tmp2A3;struct Cyc_Absyn_Vardecl*
_tmp2A4;void*_tmp2A5;struct Cyc_Absyn_Vardecl*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Vardecl*
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Vardecl*_tmp2AA;union Cyc_Absyn_Cnst_union
_tmp2AB;int _tmp2AC;void*_tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List
_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;_LL1A1: if(*((int*)_tmp2A1)!= 15)goto _LL1A3;
_tmp2A2=((struct Cyc_Absyn_Cast_e_struct*)_tmp2A1)->f2;_LL1A2: _tmp110=_tmp2A2;
goto comp_loop;_LL1A3: if(*((int*)_tmp2A1)!= 1)goto _LL1A5;_tmp2A3=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2A1)->f2;if(_tmp2A3 <= (void*)1)goto _LL1A5;if(*((int*)
_tmp2A3)!= 0)goto _LL1A5;_tmp2A4=((struct Cyc_Absyn_Global_b_struct*)_tmp2A3)->f1;
if(!(!_tmp2A4->escapes))goto _LL1A5;_LL1A4: _tmp2A6=_tmp2A4;goto _LL1A6;_LL1A5: if(*((
int*)_tmp2A1)!= 1)goto _LL1A7;_tmp2A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A1)->f2;if(_tmp2A5 <= (void*)1)goto _LL1A7;if(*((int*)_tmp2A5)!= 3)goto _LL1A7;
_tmp2A6=((struct Cyc_Absyn_Local_b_struct*)_tmp2A5)->f1;if(!(!_tmp2A6->escapes))
goto _LL1A7;_LL1A6: _tmp2A8=_tmp2A6;goto _LL1A8;_LL1A7: if(*((int*)_tmp2A1)!= 1)goto
_LL1A9;_tmp2A7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A1)->f2;if(_tmp2A7 <= (
void*)1)goto _LL1A9;if(*((int*)_tmp2A7)!= 4)goto _LL1A9;_tmp2A8=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2A7)->f1;if(!(!_tmp2A8->escapes))goto _LL1A9;_LL1A8: _tmp2AA=_tmp2A8;goto
_LL1AA;_LL1A9: if(*((int*)_tmp2A1)!= 1)goto _LL1AB;_tmp2A9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A1)->f2;if(_tmp2A9 <= (void*)1)goto _LL1AB;if(*((int*)_tmp2A9)!= 2)goto _LL1AB;
_tmp2AA=((struct Cyc_Absyn_Param_b_struct*)_tmp2A9)->f1;if(!(!_tmp2AA->escapes))
goto _LL1AB;_LL1AA: new_relns=({struct Cyc_List_List*_tmp2B1=_cycalloc(sizeof(*
_tmp2B1));_tmp2B1->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2B2=_cycalloc(sizeof(*
_tmp2B2));_tmp2B2->vd=_tmp10F;_tmp2B2->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2B3;(_tmp2B3.LessVar).tag=1;(_tmp2B3.LessVar).f1=
_tmp2AA;_tmp2B3;});_tmp2B2;});_tmp2B1->tl=_tmp29C;_tmp2B1;});goto _LL1A0;_LL1AB:
if(*((int*)_tmp2A1)!= 0)goto _LL1AD;_tmp2AB=((struct Cyc_Absyn_Const_e_struct*)
_tmp2A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2A1)->f1).Int_c).tag != 2)
goto _LL1AD;_tmp2AC=(_tmp2AB.Int_c).f2;_LL1AC: new_relns=({struct Cyc_List_List*
_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->vd=_tmp10F;_tmp2B5->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2B6;(_tmp2B6.LessConst).tag=3;(_tmp2B6.LessConst).f1=(
unsigned int)_tmp2AC;_tmp2B6;});_tmp2B5;});_tmp2B4->tl=_tmp29C;_tmp2B4;});goto
_LL1A0;_LL1AD: if(*((int*)_tmp2A1)!= 3)goto _LL1AF;_tmp2AD=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2A1)->f1;if((int)_tmp2AD != 19)goto _LL1AF;_tmp2AE=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2A1)->f2;if(_tmp2AE == 0)goto _LL1AF;_tmp2AF=*_tmp2AE;_tmp2B0=(struct Cyc_Absyn_Exp*)
_tmp2AF.hd;_LL1AE:{void*_tmp2B7=(void*)_tmp2B0->r;void*_tmp2B8;struct Cyc_Absyn_Vardecl*
_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_Vardecl*_tmp2BB;void*_tmp2BC;struct Cyc_Absyn_Vardecl*
_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_Vardecl*_tmp2BF;_LL1B2: if(*((int*)_tmp2B7)
!= 1)goto _LL1B4;_tmp2B8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B7)->f2;if(
_tmp2B8 <= (void*)1)goto _LL1B4;if(*((int*)_tmp2B8)!= 0)goto _LL1B4;_tmp2B9=((
struct Cyc_Absyn_Global_b_struct*)_tmp2B8)->f1;if(!(!_tmp2B9->escapes))goto _LL1B4;
_LL1B3: _tmp2BB=_tmp2B9;goto _LL1B5;_LL1B4: if(*((int*)_tmp2B7)!= 1)goto _LL1B6;
_tmp2BA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B7)->f2;if(_tmp2BA <= (void*)
1)goto _LL1B6;if(*((int*)_tmp2BA)!= 3)goto _LL1B6;_tmp2BB=((struct Cyc_Absyn_Local_b_struct*)
_tmp2BA)->f1;if(!(!_tmp2BB->escapes))goto _LL1B6;_LL1B5: _tmp2BD=_tmp2BB;goto
_LL1B7;_LL1B6: if(*((int*)_tmp2B7)!= 1)goto _LL1B8;_tmp2BC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B7)->f2;if(_tmp2BC <= (void*)1)goto _LL1B8;if(*((int*)_tmp2BC)!= 4)goto _LL1B8;
_tmp2BD=((struct Cyc_Absyn_Pat_b_struct*)_tmp2BC)->f1;if(!(!_tmp2BD->escapes))
goto _LL1B8;_LL1B7: _tmp2BF=_tmp2BD;goto _LL1B9;_LL1B8: if(*((int*)_tmp2B7)!= 1)goto
_LL1BA;_tmp2BE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B7)->f2;if(_tmp2BE <= (
void*)1)goto _LL1BA;if(*((int*)_tmp2BE)!= 2)goto _LL1BA;_tmp2BF=((struct Cyc_Absyn_Param_b_struct*)
_tmp2BE)->f1;if(!(!_tmp2BF->escapes))goto _LL1BA;_LL1B9: new_relns=({struct Cyc_List_List*
_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->vd=_tmp10F;_tmp2C1->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2C2;(_tmp2C2.LessSize).tag=2;(_tmp2C2.LessSize).f1=
_tmp2BF;_tmp2C2;});_tmp2C1;});_tmp2C0->tl=_tmp29C;_tmp2C0;});goto _LL1B1;_LL1BA:;
_LL1BB: goto _LL1B1;_LL1B1:;}goto _LL1A0;_LL1AF:;_LL1B0: goto _LL1A0;_LL1A0:;}if(
_tmp29C != new_relns)_tmp298=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C3;(_tmp2C3.ReachableFL).tag=1;(_tmp2C3.ReachableFL).f1=_tmp29B;(_tmp2C3.ReachableFL).f2=
new_relns;(_tmp2C3.ReachableFL).f3=_tmp29D;_tmp2C3;});{void*_tmp2C4=_tmp299;
_LL1BD: if((int)_tmp2C4 != 0)goto _LL1BF;_LL1BE: return({struct _tuple5 _tmp2C5;
_tmp2C5.f1=_tmp298;_tmp2C5.f2=Cyc_CfFlowInfo_unknown_all;_tmp2C5;});_LL1BF: if((
int)_tmp2C4 != 2)goto _LL1C1;_LL1C0: goto _LL1C2;_LL1C1: if((int)_tmp2C4 != 1)goto
_LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if(_tmp2C4 <= (void*)3)goto _LL1C5;if(*((int*)
_tmp2C4)!= 2)goto _LL1C5;_LL1C4: {struct Cyc_List_List _tmp2C6=({struct Cyc_List_List
_tmp2D2;_tmp2D2.hd=_tmp10F;_tmp2D2.tl=0;_tmp2D2;});_tmp298=Cyc_NewControlFlow_add_vars(
_tmp298,(struct Cyc_List_List*)& _tmp2C6,Cyc_CfFlowInfo_unknown_all,e->loc);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp2C8;void*_tmp2C9;struct _tuple5 _tmp2C7=Cyc_NewControlFlow_anal_Rexp(
env,_tmp298,_tmp111);_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;_tmp2C8=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp111->loc,_tmp2C8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2CA=_tmp2C8;struct
Cyc_Dict_Dict _tmp2CB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2CC;_LL1C8: if((_tmp2CA.BottomFL).tag
!= 0)goto _LL1CA;_LL1C9: return({struct _tuple5 _tmp2CD;_tmp2CD.f1=_tmp2C8;_tmp2CD.f2=
Cyc_CfFlowInfo_unknown_all;_tmp2CD;});_LL1CA: if((_tmp2CA.ReachableFL).tag != 1)
goto _LL1C7;_tmp2CB=(_tmp2CA.ReachableFL).f1;_tmp2CC=(_tmp2CA.ReachableFL).f3;
_LL1CB: if(Cyc_CfFlowInfo_initlevel(_tmp2CB,_tmp2C9)!= (void*)2){({void*_tmp2CE[0]={};
Cyc_Tcutil_terr(_tmp110->loc,({const char*_tmp2CF="expression may not be initialized";
_tag_dynforward(_tmp2CF,sizeof(char),_get_zero_arr_size(_tmp2CF,34));}),
_tag_dynforward(_tmp2CE,sizeof(void*),0));});return({struct _tuple5 _tmp2D0;
_tmp2D0.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D1;(_tmp2D1.BottomFL).tag=0;_tmp2D1;});_tmp2D0.f2=Cyc_CfFlowInfo_unknown_all;
_tmp2D0;});}_LL1C7:;}_tmp298=_tmp2C8;goto _LL1C6;}}_LL1C5:;_LL1C6: while(1){struct
Cyc_List_List _tmp2D3=({struct Cyc_List_List _tmp2DF;_tmp2DF.hd=_tmp10F;_tmp2DF.tl=
0;_tmp2DF;});_tmp298=Cyc_NewControlFlow_add_vars(_tmp298,(struct Cyc_List_List*)&
_tmp2D3,Cyc_CfFlowInfo_unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D5;void*_tmp2D6;struct _tuple5 _tmp2D4=Cyc_NewControlFlow_anal_Rexp(env,
_tmp298,_tmp111);_tmp2D5=_tmp2D4.f1;_tmp2D6=_tmp2D4.f2;_tmp2D5=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp111->loc,_tmp2D5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2D7=_tmp2D5;struct
Cyc_Dict_Dict _tmp2D8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2D9;_LL1CD: if((_tmp2D7.BottomFL).tag
!= 0)goto _LL1CF;_LL1CE: goto _LL1CC;_LL1CF: if((_tmp2D7.ReachableFL).tag != 1)goto
_LL1CC;_tmp2D8=(_tmp2D7.ReachableFL).f1;_tmp2D9=(_tmp2D7.ReachableFL).f3;_LL1D0:
if(Cyc_CfFlowInfo_initlevel(_tmp2D8,_tmp2D6)!= (void*)2){({void*_tmp2DA[0]={};
Cyc_Tcutil_terr(_tmp110->loc,({const char*_tmp2DB="expression may not be initialized";
_tag_dynforward(_tmp2DB,sizeof(char),_get_zero_arr_size(_tmp2DB,34));}),
_tag_dynforward(_tmp2DA,sizeof(void*),0));});return({struct _tuple5 _tmp2DC;
_tmp2DC.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DD;(_tmp2DD.BottomFL).tag=0;_tmp2DD;});_tmp2DC.f2=Cyc_CfFlowInfo_unknown_all;
_tmp2DC;});}_LL1CC:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DE=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp298,_tmp2D5,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp2DE,_tmp298))break;_tmp298=_tmp2DE;}}}return({struct _tuple5 _tmp2E0;_tmp2E0.f1=
_tmp298;_tmp2E0.f2=Cyc_CfFlowInfo_unknown_all;_tmp2E0;});_LL1BC:;}}_LL19B:;}}
_LLF0: if(*((int*)_tmpC9)!= 38)goto _LLF2;_tmp113=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpC9)->f1;_LLF1: while(1){inflow=(Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp113))->flow;{void*_tmp2E1=(void*)_tmp113->r;struct Cyc_Absyn_Stmt*_tmp2E2;
struct Cyc_Absyn_Stmt*_tmp2E3;struct Cyc_Absyn_Decl*_tmp2E4;struct Cyc_Absyn_Stmt*
_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;_LL1D2: if(_tmp2E1 <= (void*)1)goto _LL1D8;if(*((
int*)_tmp2E1)!= 1)goto _LL1D4;_tmp2E2=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E1)->f1;
_tmp2E3=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E1)->f2;_LL1D3: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp2E2);_tmp113=_tmp2E3;goto _LL1D1;_LL1D4: if(*((int*)_tmp2E1)!= 11)
goto _LL1D6;_tmp2E4=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E1)->f1;_tmp2E5=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2E1)->f2;_LL1D5: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp2E4);_tmp113=_tmp2E5;goto _LL1D1;_LL1D6: if(*((int*)_tmp2E1)!= 0)
goto _LL1D8;_tmp2E6=((struct Cyc_Absyn_Exp_s_struct*)_tmp2E1)->f1;_LL1D7: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp2E6);_LL1D8:;_LL1D9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7[0]=({struct Cyc_Core_Impossible_struct
_tmp2E8;_tmp2E8.tag=Cyc_Core_Impossible;_tmp2E8.f1=({const char*_tmp2E9="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,33));});_tmp2E8;});
_tmp2E7;}));_LL1D1:;}}_LLF2: if(*((int*)_tmpC9)!= 1)goto _LLF4;_tmp114=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpC9)->f2;if((int)_tmp114 != 0)goto _LLF4;_LLF3:
goto _LLF5;_LLF4: if(*((int*)_tmpC9)!= 2)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if(*((int*)
_tmpC9)!= 10)goto _LLF8;_LLF7: goto _LLF9;_LLF8: if(*((int*)_tmpC9)!= 37)goto _LLFA;
_LLF9: goto _LLFB;_LLFA: if(*((int*)_tmpC9)!= 27)goto _LL95;_LLFB:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA[0]=({
struct Cyc_Core_Impossible_struct _tmp2EB;_tmp2EB.tag=Cyc_Core_Impossible;_tmp2EB.f1=({
const char*_tmp2EC="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp2EC,
sizeof(char),_get_zero_arr_size(_tmp2EC,31));});_tmp2EB;});_tmp2EA;}));_LL95:;}}
static struct _tuple5 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*_tmp2ED=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2EE;void*_tmp2EF;struct Cyc_Absyn_PtrAtts _tmp2F0;struct Cyc_Absyn_Conref*
_tmp2F1;struct Cyc_Absyn_Conref*_tmp2F2;_LL1DB: if(_tmp2ED <= (void*)4)goto _LL1DD;
if(*((int*)_tmp2ED)!= 4)goto _LL1DD;_tmp2EE=((struct Cyc_Absyn_PointerType_struct*)
_tmp2ED)->f1;_tmp2EF=(void*)_tmp2EE.elt_typ;_tmp2F0=_tmp2EE.ptr_atts;_tmp2F1=
_tmp2F0.bounds;_tmp2F2=_tmp2F0.zero_term;_LL1DC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2F3=f;struct Cyc_Dict_Dict _tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2F6;_LL1E0: if((_tmp2F3.BottomFL).tag != 0)goto _LL1E2;_LL1E1: return({struct
_tuple5 _tmp2F7;_tmp2F7.f1=f;_tmp2F7.f2=(void*)0;_tmp2F7;});_LL1E2: if((_tmp2F3.ReachableFL).tag
!= 1)goto _LL1DF;_tmp2F4=(_tmp2F3.ReachableFL).f1;_tmp2F5=(_tmp2F3.ReachableFL).f2;
_tmp2F6=(_tmp2F3.ReachableFL).f3;_LL1E3: if(Cyc_Tcutil_is_bound_one(_tmp2F1)){
void*_tmp2F8=r;struct Cyc_CfFlowInfo_Place*_tmp2F9;struct Cyc_CfFlowInfo_Place
_tmp2FA;void*_tmp2FB;struct Cyc_List_List*_tmp2FC;void*_tmp2FD;_LL1E5: if((int)
_tmp2F8 != 1)goto _LL1E7;_LL1E6: goto _LL1E8;_LL1E7: if((int)_tmp2F8 != 2)goto _LL1E9;
_LL1E8:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp2FF;_tmp2FF.tag=Cyc_CfFlowInfo_NotZero;_tmp2FF.f1=_tmp2F5;_tmp2FF;});_tmp2FE;})));
goto _LL1E4;_LL1E9: if(_tmp2F8 <= (void*)3)goto _LL1EB;if(*((int*)_tmp2F8)!= 2)goto
_LL1EB;_tmp2F9=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2F8)->f1;_tmp2FA=*
_tmp2F9;_tmp2FB=(void*)_tmp2FA.root;_tmp2FC=_tmp2FA.fields;_LL1EA:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp300=_cycalloc(sizeof(*
_tmp300));_tmp300[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp301;_tmp301.tag=
Cyc_CfFlowInfo_NotZero;_tmp301.f1=_tmp2F5;_tmp301;});_tmp300;})));return({struct
_tuple5 _tmp302;_tmp302.f1=f;_tmp302.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp304;_tmp304.tag=0;_tmp304.f1=({struct Cyc_CfFlowInfo_Place*_tmp305=_cycalloc(
sizeof(*_tmp305));_tmp305->root=(void*)_tmp2FB;_tmp305->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2FC,flds);
_tmp305;});_tmp304;});_tmp303;});_tmp302;});_LL1EB: if((int)_tmp2F8 != 0)goto
_LL1ED;_LL1EC:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple5 _tmp306;_tmp306.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp307;(_tmp307.BottomFL).tag=0;_tmp307;});_tmp306.f2=(void*)0;_tmp306;});
_LL1ED: if(_tmp2F8 <= (void*)3)goto _LL1EF;if(*((int*)_tmp2F8)!= 0)goto _LL1EF;
_tmp2FD=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2F8)->f1;_LL1EE: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp2FD);goto _LL1F0;_LL1EF:;_LL1F0:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp309;_tmp309.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp309.f1=_tmp2F5;_tmp309;});_tmp308;})));_LL1E4:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp30A=_cycalloc(sizeof(*_tmp30A));
_tmp30A[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp30B;_tmp30B.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp30B.f1=_tmp2F5;_tmp30B;});_tmp30A;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2F4,
r)== (void*)0)({void*_tmp30C[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp30D="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,46));}),
_tag_dynforward(_tmp30C,sizeof(void*),0));});return({struct _tuple5 _tmp30E;
_tmp30E.f1=f;_tmp30E.f2=(void*)0;_tmp30E;});_LL1DF:;}_LL1DD:;_LL1DE:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));
_tmp30F[0]=({struct Cyc_Core_Impossible_struct _tmp310;_tmp310.tag=Cyc_Core_Impossible;
_tmp310.f1=({const char*_tmp311="left deref of non-pointer-type";_tag_dynforward(
_tmp311,sizeof(char),_get_zero_arr_size(_tmp311,31));});_tmp310;});_tmp30F;}));
_LL1DA:;}static struct _tuple5 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;{union Cyc_CfFlowInfo_FlowInfo_union _tmp312=inflow;
struct Cyc_Dict_Dict _tmp313;struct Cyc_List_List*_tmp314;_LL1F2: if((_tmp312.BottomFL).tag
!= 0)goto _LL1F4;_LL1F3: return({struct _tuple5 _tmp315;_tmp315.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp316;(_tmp316.BottomFL).tag=0;_tmp316;});
_tmp315.f2=(void*)0;_tmp315;});_LL1F4: if((_tmp312.ReachableFL).tag != 1)goto
_LL1F1;_tmp313=(_tmp312.ReachableFL).f1;_tmp314=(_tmp312.ReachableFL).f2;_LL1F5:
d=_tmp313;_LL1F1:;}{void*_tmp317=(void*)e->r;struct Cyc_Absyn_Exp*_tmp318;struct
Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp31A;void*_tmp31B;struct Cyc_Absyn_Vardecl*
_tmp31C;void*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31E;void*_tmp31F;struct Cyc_Absyn_Vardecl*
_tmp320;void*_tmp321;struct Cyc_Absyn_Vardecl*_tmp322;struct Cyc_Absyn_Exp*_tmp323;
struct _dynforward_ptr*_tmp324;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Exp*
_tmp326;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Exp*_tmp328;struct
_dynforward_ptr*_tmp329;_LL1F7: if(*((int*)_tmp317)!= 15)goto _LL1F9;_tmp318=((
struct Cyc_Absyn_Cast_e_struct*)_tmp317)->f2;_LL1F8: _tmp319=_tmp318;goto _LL1FA;
_LL1F9: if(*((int*)_tmp317)!= 13)goto _LL1FB;_tmp319=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp317)->f1;_LL1FA: _tmp31A=_tmp319;goto _LL1FC;_LL1FB: if(*((int*)_tmp317)!= 14)
goto _LL1FD;_tmp31A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp317)->f1;_LL1FC:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp31A,flds);_LL1FD: if(*((int*)
_tmp317)!= 1)goto _LL1FF;_tmp31B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp317)->f2;
if(_tmp31B <= (void*)1)goto _LL1FF;if(*((int*)_tmp31B)!= 2)goto _LL1FF;_tmp31C=((
struct Cyc_Absyn_Param_b_struct*)_tmp31B)->f1;_LL1FE: _tmp31E=_tmp31C;goto _LL200;
_LL1FF: if(*((int*)_tmp317)!= 1)goto _LL201;_tmp31D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp317)->f2;if(_tmp31D <= (void*)1)goto _LL201;if(*((int*)_tmp31D)!= 3)goto _LL201;
_tmp31E=((struct Cyc_Absyn_Local_b_struct*)_tmp31D)->f1;_LL200: _tmp320=_tmp31E;
goto _LL202;_LL201: if(*((int*)_tmp317)!= 1)goto _LL203;_tmp31F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp317)->f2;if(_tmp31F <= (void*)1)goto _LL203;if(*((int*)_tmp31F)!= 4)goto _LL203;
_tmp320=((struct Cyc_Absyn_Pat_b_struct*)_tmp31F)->f1;_LL202: return({struct
_tuple5 _tmp32A;_tmp32A.f1=inflow;_tmp32A.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp32C;_tmp32C.tag=0;_tmp32C.f1=({struct Cyc_CfFlowInfo_Place*_tmp32D=_cycalloc(
sizeof(*_tmp32D));_tmp32D->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp32F;_tmp32F.tag=0;_tmp32F.f1=_tmp320;_tmp32F;});_tmp32E;}));_tmp32D->fields=
flds;_tmp32D;});_tmp32C;});_tmp32B;});_tmp32A;});_LL203: if(*((int*)_tmp317)!= 1)
goto _LL205;_tmp321=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp317)->f2;if(
_tmp321 <= (void*)1)goto _LL205;if(*((int*)_tmp321)!= 0)goto _LL205;_tmp322=((
struct Cyc_Absyn_Global_b_struct*)_tmp321)->f1;_LL204: return({struct _tuple5
_tmp330;_tmp330.f1=inflow;_tmp330.f2=(void*)0;_tmp330;});_LL205: if(*((int*)
_tmp317)!= 24)goto _LL207;_tmp323=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp317)->f1;
_tmp324=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp317)->f2;_LL206:{void*_tmp331=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp323->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp332;void*_tmp333;_LL210: if(_tmp331 <= (void*)4)goto
_LL212;if(*((int*)_tmp331)!= 4)goto _LL212;_tmp332=((struct Cyc_Absyn_PointerType_struct*)
_tmp331)->f1;_tmp333=(void*)_tmp332.elt_typ;_LL211: if(!Cyc_Absyn_is_union_type(
_tmp333))flds=({struct Cyc_List_List*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp333,_tmp324);_tmp334->tl=flds;
_tmp334;});goto _LL20F;_LL212:;_LL213:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335[0]=({struct Cyc_Core_Impossible_struct
_tmp336;_tmp336.tag=Cyc_Core_Impossible;_tmp336.f1=({const char*_tmp337="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp337,sizeof(char),_get_zero_arr_size(_tmp337,25));});_tmp336;});
_tmp335;}));_LL20F:;}_tmp325=_tmp323;goto _LL208;_LL207: if(*((int*)_tmp317)!= 22)
goto _LL209;_tmp325=((struct Cyc_Absyn_Deref_e_struct*)_tmp317)->f1;_LL208: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp339;void*_tmp33A;struct _tuple5 _tmp338=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp325);_tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;_tmp339=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp339);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp339,_tmp325,
_tmp33A,flds);}_LL209: if(*((int*)_tmp317)!= 25)goto _LL20B;_tmp326=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp317)->f1;_tmp327=((struct Cyc_Absyn_Subscript_e_struct*)_tmp317)->f2;_LL20A: {
void*_tmp33B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp326->topt))->v);struct Cyc_Absyn_PtrInfo _tmp33C;struct Cyc_Absyn_PtrAtts
_tmp33D;struct Cyc_Absyn_Conref*_tmp33E;_LL215: if(_tmp33B <= (void*)4)goto _LL219;
if(*((int*)_tmp33B)!= 9)goto _LL217;_LL216: {unsigned int _tmp33F=(Cyc_Evexp_eval_const_uint_exp(
_tmp327)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp326,({struct
Cyc_List_List*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->hd=(void*)((int)
_tmp33F);_tmp340->tl=flds;_tmp340;}));}_LL217: if(*((int*)_tmp33B)!= 4)goto _LL219;
_tmp33C=((struct Cyc_Absyn_PointerType_struct*)_tmp33B)->f1;_tmp33D=_tmp33C.ptr_atts;
_tmp33E=_tmp33D.bounds;_LL218: {struct _RegionHandle _tmp341=_new_region("temp");
struct _RegionHandle*temp=& _tmp341;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp344;struct Cyc_List_List*_tmp345;struct _tuple9 _tmp343=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp342[2];_tmp342[1]=_tmp327;_tmp342[0]=
_tmp326;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp342,sizeof(struct Cyc_Absyn_Exp*),2));}),
1);_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;_tmp344=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp327->loc,_tmp344);{union Cyc_CfFlowInfo_FlowInfo_union _tmp346=_tmp344;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp347=_tmp344;struct Cyc_Dict_Dict _tmp348;struct
Cyc_List_List*_tmp349;struct Cyc_CfFlowInfo_ConsumeInfo _tmp34A;_LL21C: if((_tmp347.ReachableFL).tag
!= 1)goto _LL21E;_tmp348=(_tmp347.ReachableFL).f1;_tmp349=(_tmp347.ReachableFL).f2;
_tmp34A=(_tmp347.ReachableFL).f3;_LL21D: if(Cyc_CfFlowInfo_initlevel(_tmp348,(
void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(
_tmp345))->tl))->hd)== (void*)0)({void*_tmp34B[0]={};Cyc_Tcutil_terr(_tmp327->loc,({
const char*_tmp34C="expression may not be initialized";_tag_dynforward(_tmp34C,
sizeof(char),_get_zero_arr_size(_tmp34C,34));}),_tag_dynforward(_tmp34B,sizeof(
void*),0));});{struct Cyc_List_List*_tmp34D=Cyc_NewControlFlow_add_subscript_reln(
_tmp349,_tmp326,_tmp327);if(_tmp349 != _tmp34D)_tmp346=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp34E;(_tmp34E.ReachableFL).tag=1;(_tmp34E.ReachableFL).f1=
_tmp348;(_tmp34E.ReachableFL).f2=_tmp34D;(_tmp34E.ReachableFL).f3=_tmp34A;
_tmp34E;});goto _LL21B;}_LL21E:;_LL21F: goto _LL21B;_LL21B:;}{union Cyc_Absyn_Constraint_union
_tmp34F=(Cyc_Absyn_compress_conref(_tmp33E))->v;void*_tmp350;_LL221: if((_tmp34F.Eq_constr).tag
!= 0)goto _LL223;_tmp350=(_tmp34F.Eq_constr).f1;if(_tmp350 <= (void*)2)goto _LL223;
if(*((int*)_tmp350)!= 1)goto _LL223;_LL222:{void*_tmp351=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp345))->tl))->hd;struct Cyc_List_List*
_tmp352;_LL226: if(_tmp351 <= (void*)3)goto _LL228;if(*((int*)_tmp351)!= 3)goto
_LL228;_tmp352=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp351)->f1;_LL227:(void*)(
_tmp327->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp353=
_cycalloc(sizeof(*_tmp353));_tmp353[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp354;_tmp354.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp354.f1=_tmp352;_tmp354;});
_tmp353;})));goto _LL225;_LL228:;_LL229: goto _LL225;_LL225:;}goto _LL220;_LL223:;
_LL224: goto _LL220;_LL220:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp356;void*
_tmp357;struct _tuple5 _tmp355=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp344,
_tmp326,(void*)((struct Cyc_List_List*)_check_null(_tmp345))->hd,flds);_tmp356=
_tmp355.f1;_tmp357=_tmp355.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp358=_tmp356;
_LL22B: if((_tmp358.BottomFL).tag != 0)goto _LL22D;_LL22C: {struct _tuple5 _tmp35A=({
struct _tuple5 _tmp359;_tmp359.f1=_tmp356;_tmp359.f2=_tmp357;_tmp359;});
_npop_handler(0);return _tmp35A;}_LL22D:;_LL22E: {struct _tuple5 _tmp35C=({struct
_tuple5 _tmp35B;_tmp35B.f1=_tmp346;_tmp35B.f2=_tmp357;_tmp35B;});_npop_handler(0);
return _tmp35C;}_LL22A:;}}}};_pop_region(temp);}_LL219:;_LL21A:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D[0]=({
struct Cyc_Core_Impossible_struct _tmp35E;_tmp35E.tag=Cyc_Core_Impossible;_tmp35E.f1=({
const char*_tmp35F="anal_Lexp: Subscript -- bad type";_tag_dynforward(_tmp35F,
sizeof(char),_get_zero_arr_size(_tmp35F,33));});_tmp35E;});_tmp35D;}));_LL214:;}
_LL20B: if(*((int*)_tmp317)!= 23)goto _LL20D;_tmp328=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp317)->f1;_tmp329=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp317)->f2;_LL20C:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp328->topt))->v))
return({struct _tuple5 _tmp360;_tmp360.f1=inflow;_tmp360.f2=(void*)0;_tmp360;});
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp328,({struct Cyc_List_List*
_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->hd=(void*)Cyc_NewControlFlow_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp328->topt))->v,_tmp329);_tmp361->tl=
flds;_tmp361;}));_LL20D:;_LL20E: return({struct _tuple5 _tmp362;_tmp362.f1=(union
Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp363;(
_tmp363.BottomFL).tag=0;_tmp363;});_tmp362.f2=(void*)0;_tmp362;});_LL1F6:;}}
static struct _tuple5 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp365;void*_tmp366;struct _tuple5 _tmp364=Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,e,0);_tmp365=_tmp364.f1;_tmp366=_tmp364.f2;return({struct _tuple5 _tmp367;
_tmp367.f1=_tmp365;_tmp367.f2=_tmp366;_tmp367;});}static struct _tuple7 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){{void*_tmp368=(void*)e->r;struct Cyc_Absyn_Exp*_tmp369;
struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct Cyc_Absyn_Exp*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*
_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp371;void*_tmp372;
struct Cyc_List_List*_tmp373;struct Cyc_List_List _tmp374;struct Cyc_Absyn_Exp*
_tmp375;struct Cyc_List_List*_tmp376;void*_tmp377;struct Cyc_List_List*_tmp378;
_LL230: if(*((int*)_tmp368)!= 6)goto _LL232;_tmp369=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp368)->f1;_tmp36A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp368)->f2;
_tmp36B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp368)->f3;_LL231: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp37A;union Cyc_CfFlowInfo_FlowInfo_union _tmp37B;struct _tuple7 _tmp379=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp369);_tmp37A=_tmp379.f1;_tmp37B=_tmp379.f2;_tmp37A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp369->loc,_tmp37A);_tmp37B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp369->loc,
_tmp37B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp37D;union Cyc_CfFlowInfo_FlowInfo_union
_tmp37E;struct _tuple7 _tmp37C=Cyc_NewControlFlow_anal_test(env,_tmp37A,_tmp36A);
_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp380;
union Cyc_CfFlowInfo_FlowInfo_union _tmp381;struct _tuple7 _tmp37F=Cyc_NewControlFlow_anal_test(
env,_tmp37B,_tmp36B);_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;return({struct _tuple7
_tmp382;_tmp382.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp37D,_tmp380,1);
_tmp382.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp37E,_tmp381,1);_tmp382;});}}}
_LL232: if(*((int*)_tmp368)!= 7)goto _LL234;_tmp36C=((struct Cyc_Absyn_And_e_struct*)
_tmp368)->f1;_tmp36D=((struct Cyc_Absyn_And_e_struct*)_tmp368)->f2;_LL233: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp384;union Cyc_CfFlowInfo_FlowInfo_union _tmp385;
struct _tuple7 _tmp383=Cyc_NewControlFlow_anal_test(env,inflow,_tmp36C);_tmp384=
_tmp383.f1;_tmp385=_tmp383.f2;_tmp384=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp36C->loc,_tmp384);_tmp385=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp36C->loc,
_tmp385);{union Cyc_CfFlowInfo_FlowInfo_union _tmp387;union Cyc_CfFlowInfo_FlowInfo_union
_tmp388;struct _tuple7 _tmp386=Cyc_NewControlFlow_anal_test(env,_tmp384,_tmp36D);
_tmp387=_tmp386.f1;_tmp388=_tmp386.f2;_tmp387=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp36D->loc,_tmp387);_tmp388=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp36D->loc,
_tmp388);return({struct _tuple7 _tmp389;_tmp389.f1=_tmp387;_tmp389.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp385,_tmp388,0);_tmp389;});}}_LL234: if(*((int*)_tmp368)!= 8)
goto _LL236;_tmp36E=((struct Cyc_Absyn_Or_e_struct*)_tmp368)->f1;_tmp36F=((struct
Cyc_Absyn_Or_e_struct*)_tmp368)->f2;_LL235: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp38B;union Cyc_CfFlowInfo_FlowInfo_union _tmp38C;struct _tuple7 _tmp38A=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp36E);_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;_tmp38B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp36E->loc,_tmp38B);_tmp38C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp36E->loc,
_tmp38C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp38E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp38F;struct _tuple7 _tmp38D=Cyc_NewControlFlow_anal_test(env,_tmp38C,_tmp36F);
_tmp38E=_tmp38D.f1;_tmp38F=_tmp38D.f2;_tmp38E=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp36F->loc,_tmp38E);_tmp38F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp36F->loc,
_tmp38F);return({struct _tuple7 _tmp390;_tmp390.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp38B,_tmp38E,0);_tmp390.f2=_tmp38F;_tmp390;});}}_LL236: if(*((int*)_tmp368)!= 
9)goto _LL238;_tmp370=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp368)->f1;_tmp371=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp368)->f2;_LL237: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp392;void*_tmp393;struct _tuple5 _tmp391=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp370);_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_tmp392=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp370->loc,_tmp392);return Cyc_NewControlFlow_anal_test(env,_tmp392,_tmp371);}
_LL238: if(*((int*)_tmp368)!= 3)goto _LL23A;_tmp372=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp368)->f1;if((int)_tmp372 != 11)goto _LL23A;_tmp373=((struct Cyc_Absyn_Primop_e_struct*)
_tmp368)->f2;if(_tmp373 == 0)goto _LL23A;_tmp374=*_tmp373;_tmp375=(struct Cyc_Absyn_Exp*)
_tmp374.hd;_tmp376=_tmp374.tl;if(_tmp376 != 0)goto _LL23A;_LL239: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp395;union Cyc_CfFlowInfo_FlowInfo_union _tmp396;struct _tuple7 _tmp394=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp375);_tmp395=_tmp394.f1;_tmp396=_tmp394.f2;return({struct _tuple7
_tmp397;_tmp397.f1=_tmp396;_tmp397.f2=_tmp395;_tmp397;});}_LL23A: if(*((int*)
_tmp368)!= 3)goto _LL23C;_tmp377=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp368)->f1;_tmp378=((struct Cyc_Absyn_Primop_e_struct*)_tmp368)->f2;_LL23B: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp398=
_new_region("temp");struct _RegionHandle*temp=& _tmp398;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp39A;struct Cyc_List_List*_tmp39B;struct _tuple9
_tmp399=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp378,0);
_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp39B))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp39B->tl))->hd;
f=_tmp39A;};_pop_region(temp);}{void*_tmp39C=_tmp377;_LL23F: if((int)_tmp39C != 0)
goto _LL241;_LL240: goto _LL242;_LL241: if((int)_tmp39C != 2)goto _LL243;_LL242: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp378))->hd)->loc,f);
goto _LL23E;_LL243:;_LL244: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp378))->hd)->loc,f);goto _LL23E;_LL23E:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp39D=f;struct Cyc_Dict_Dict _tmp39E;struct Cyc_List_List*
_tmp39F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A0;_LL246: if((_tmp39D.BottomFL).tag
!= 0)goto _LL248;_LL247: return({struct _tuple7 _tmp3A1;_tmp3A1.f1=f;_tmp3A1.f2=f;
_tmp3A1;});_LL248: if((_tmp39D.ReachableFL).tag != 1)goto _LL245;_tmp39E=(_tmp39D.ReachableFL).f1;
_tmp39F=(_tmp39D.ReachableFL).f2;_tmp3A0=(_tmp39D.ReachableFL).f3;_LL249: {
struct Cyc_Absyn_Exp*_tmp3A2=(struct Cyc_Absyn_Exp*)_tmp378->hd;struct Cyc_Absyn_Exp*
_tmp3A3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp378->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp39E,r1)== (void*)0)({void*_tmp3A4[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp378->hd)->loc,({const char*_tmp3A5="expression may not be initialized";
_tag_dynforward(_tmp3A5,sizeof(char),_get_zero_arr_size(_tmp3A5,34));}),
_tag_dynforward(_tmp3A4,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp39E,
r2)== (void*)0)({void*_tmp3A6[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp378->tl))->hd)->loc,({const char*_tmp3A7="expression may not be initialized";
_tag_dynforward(_tmp3A7,sizeof(char),_get_zero_arr_size(_tmp3A7,34));}),
_tag_dynforward(_tmp3A6,sizeof(void*),0));});if(_tmp377 == (void*)5  || _tmp377 == (
void*)6){struct _tuple0 _tmp3A9=({struct _tuple0 _tmp3A8;_tmp3A8.f1=r1;_tmp3A8.f2=r2;
_tmp3A8;});void*_tmp3AA;void*_tmp3AB;void*_tmp3AC;void*_tmp3AD;void*_tmp3AE;void*
_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;void*_tmp3B4;void*
_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;void*
_tmp3BB;void*_tmp3BC;void*_tmp3BD;_LL24B: _tmp3AA=_tmp3A9.f1;if(_tmp3AA <= (void*)
3)goto _LL24D;if(*((int*)_tmp3AA)!= 0)goto _LL24D;_tmp3AB=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3AA)->f1;_tmp3AC=_tmp3A9.f2;if((int)_tmp3AC != 0)goto _LL24D;_LL24C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3BF;union Cyc_CfFlowInfo_FlowInfo_union _tmp3C0;struct _tuple7 _tmp3BE=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3A2,_tmp3AB);_tmp3BF=_tmp3BE.f1;_tmp3C0=_tmp3BE.f2;{void*_tmp3C1=
_tmp377;_LL260: if((int)_tmp3C1 != 5)goto _LL262;_LL261: return({struct _tuple7
_tmp3C2;_tmp3C2.f1=_tmp3C0;_tmp3C2.f2=_tmp3BF;_tmp3C2;});_LL262: if((int)_tmp3C1
!= 6)goto _LL264;_LL263: return({struct _tuple7 _tmp3C3;_tmp3C3.f1=_tmp3BF;_tmp3C3.f2=
_tmp3C0;_tmp3C3;});_LL264:;_LL265:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4[0]=({struct Cyc_Core_Impossible_struct
_tmp3C5;_tmp3C5.tag=Cyc_Core_Impossible;_tmp3C5.f1=({const char*_tmp3C6="anal_test, zero-split";
_tag_dynforward(_tmp3C6,sizeof(char),_get_zero_arr_size(_tmp3C6,22));});_tmp3C5;});
_tmp3C4;}));_LL25F:;}}_LL24D: _tmp3AD=_tmp3A9.f1;if((int)_tmp3AD != 0)goto _LL24F;
_tmp3AE=_tmp3A9.f2;if(_tmp3AE <= (void*)3)goto _LL24F;if(*((int*)_tmp3AE)!= 0)goto
_LL24F;_tmp3AF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3AE)->f1;
_LL24E: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3C8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C9;struct _tuple7 _tmp3C7=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3A3,
_tmp3AF);_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;{void*_tmp3CA=_tmp377;_LL267: if((
int)_tmp3CA != 5)goto _LL269;_LL268: return({struct _tuple7 _tmp3CB;_tmp3CB.f1=
_tmp3C9;_tmp3CB.f2=_tmp3C8;_tmp3CB;});_LL269: if((int)_tmp3CA != 6)goto _LL26B;
_LL26A: return({struct _tuple7 _tmp3CC;_tmp3CC.f1=_tmp3C8;_tmp3CC.f2=_tmp3C9;
_tmp3CC;});_LL26B:;_LL26C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD[0]=({struct Cyc_Core_Impossible_struct
_tmp3CE;_tmp3CE.tag=Cyc_Core_Impossible;_tmp3CE.f1=({const char*_tmp3CF="anal_test, zero-split";
_tag_dynforward(_tmp3CF,sizeof(char),_get_zero_arr_size(_tmp3CF,22));});_tmp3CE;});
_tmp3CD;}));_LL266:;}}_LL24F: _tmp3B0=_tmp3A9.f1;if((int)_tmp3B0 != 0)goto _LL251;
_tmp3B1=_tmp3A9.f2;if((int)_tmp3B1 != 0)goto _LL251;_LL250: if(_tmp377 == (void*)5)
return({struct _tuple7 _tmp3D0;_tmp3D0.f1=f;_tmp3D0.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D1;(_tmp3D1.BottomFL).tag=0;_tmp3D1;});
_tmp3D0;});else{return({struct _tuple7 _tmp3D2;_tmp3D2.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D3;(_tmp3D3.BottomFL).tag=0;_tmp3D3;});
_tmp3D2.f2=f;_tmp3D2;});}_LL251: _tmp3B2=_tmp3A9.f1;if((int)_tmp3B2 != 0)goto
_LL253;_tmp3B3=_tmp3A9.f2;if((int)_tmp3B3 != 1)goto _LL253;_LL252: goto _LL254;
_LL253: _tmp3B4=_tmp3A9.f1;if((int)_tmp3B4 != 0)goto _LL255;_tmp3B5=_tmp3A9.f2;if((
int)_tmp3B5 != 2)goto _LL255;_LL254: goto _LL256;_LL255: _tmp3B6=_tmp3A9.f1;if((int)
_tmp3B6 != 0)goto _LL257;_tmp3B7=_tmp3A9.f2;if(_tmp3B7 <= (void*)3)goto _LL257;if(*((
int*)_tmp3B7)!= 2)goto _LL257;_LL256: goto _LL258;_LL257: _tmp3B8=_tmp3A9.f1;if((int)
_tmp3B8 != 1)goto _LL259;_tmp3B9=_tmp3A9.f2;if((int)_tmp3B9 != 0)goto _LL259;_LL258:
goto _LL25A;_LL259: _tmp3BA=_tmp3A9.f1;if((int)_tmp3BA != 2)goto _LL25B;_tmp3BB=
_tmp3A9.f2;if((int)_tmp3BB != 0)goto _LL25B;_LL25A: goto _LL25C;_LL25B: _tmp3BC=
_tmp3A9.f1;if(_tmp3BC <= (void*)3)goto _LL25D;if(*((int*)_tmp3BC)!= 2)goto _LL25D;
_tmp3BD=_tmp3A9.f2;if((int)_tmp3BD != 0)goto _LL25D;_LL25C: if(_tmp377 == (void*)6)
return({struct _tuple7 _tmp3D4;_tmp3D4.f1=f;_tmp3D4.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D5;(_tmp3D5.BottomFL).tag=0;_tmp3D5;});
_tmp3D4;});else{return({struct _tuple7 _tmp3D6;_tmp3D6.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D7;(_tmp3D7.BottomFL).tag=0;_tmp3D7;});
_tmp3D6.f2=f;_tmp3D6;});}_LL25D:;_LL25E: goto _LL24A;_LL24A:;}{struct _tuple0
_tmp3D9=({struct _tuple0 _tmp3D8;_tmp3D8.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A2->topt))->v);_tmp3D8.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3A3->topt))->v);_tmp3D8;});void*_tmp3DA;void*
_tmp3DB;void*_tmp3DC;void*_tmp3DD;void*_tmp3DE;void*_tmp3DF;_LL26E: _tmp3DA=
_tmp3D9.f1;if(_tmp3DA <= (void*)4)goto _LL270;if(*((int*)_tmp3DA)!= 5)goto _LL270;
_tmp3DB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DA)->f1;if((int)_tmp3DB != 
1)goto _LL270;_LL26F: goto _LL271;_LL270: _tmp3DC=_tmp3D9.f2;if(_tmp3DC <= (void*)4)
goto _LL272;if(*((int*)_tmp3DC)!= 5)goto _LL272;_tmp3DD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3DC)->f1;if((int)_tmp3DD != 1)goto _LL272;_LL271: goto _LL273;_LL272: _tmp3DE=
_tmp3D9.f1;if(_tmp3DE <= (void*)4)goto _LL274;if(*((int*)_tmp3DE)!= 18)goto _LL274;
_LL273: goto _LL275;_LL274: _tmp3DF=_tmp3D9.f2;if(_tmp3DF <= (void*)4)goto _LL276;if(*((
int*)_tmp3DF)!= 18)goto _LL276;_LL275: goto _LL26D;_LL276:;_LL277: return({struct
_tuple7 _tmp3E0;_tmp3E0.f1=f;_tmp3E0.f2=f;_tmp3E0;});_LL26D:;}{void*_tmp3E1=
_tmp377;_LL279: if((int)_tmp3E1 != 5)goto _LL27B;_LL27A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E2=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A2,r1,(void*)5,r2);_tmp3E2=Cyc_NewControlFlow_if_tagcmp(
env,_tmp3E2,_tmp3A3,r2,(void*)5,r1);return({struct _tuple7 _tmp3E3;_tmp3E3.f1=
_tmp3E2;_tmp3E3.f2=f;_tmp3E3;});}_LL27B: if((int)_tmp3E1 != 6)goto _LL27D;_LL27C: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E4=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3A2,r1,(void*)5,r2);_tmp3E4=Cyc_NewControlFlow_if_tagcmp(env,_tmp3E4,_tmp3A3,
r2,(void*)5,r1);return({struct _tuple7 _tmp3E5;_tmp3E5.f1=f;_tmp3E5.f2=_tmp3E4;
_tmp3E5;});}_LL27D: if((int)_tmp3E1 != 7)goto _LL27F;_LL27E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E6=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A3,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E7=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A2,r1,(void*)10,r2);return({
struct _tuple7 _tmp3E8;_tmp3E8.f1=_tmp3E6;_tmp3E8.f2=_tmp3E7;_tmp3E8;});}_LL27F:
if((int)_tmp3E1 != 9)goto _LL281;_LL280: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E9=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A3,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3EA=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A2,r1,(void*)8,r2);return({
struct _tuple7 _tmp3EB;_tmp3EB.f1=_tmp3E9;_tmp3EB.f2=_tmp3EA;_tmp3EB;});}_LL281:
if((int)_tmp3E1 != 8)goto _LL283;_LL282: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3EC=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A2,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3ED=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A3,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3EE=_tmp3EC;struct Cyc_Dict_Dict _tmp3EF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3F0;_LL288: if((_tmp3EE.BottomFL).tag != 0)goto _LL28A;_LL289:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1[0]=({
struct Cyc_Core_Impossible_struct _tmp3F2;_tmp3F2.tag=Cyc_Core_Impossible;_tmp3F2.f1=({
const char*_tmp3F3="anal_test, Lt";_tag_dynforward(_tmp3F3,sizeof(char),
_get_zero_arr_size(_tmp3F3,14));});_tmp3F2;});_tmp3F1;}));_LL28A: if((_tmp3EE.ReachableFL).tag
!= 1)goto _LL287;_tmp3EF=(_tmp3EE.ReachableFL).f1;_tmp3F0=(_tmp3EE.ReachableFL).f3;
_LL28B: _tmp39E=_tmp3EF;_tmp3A0=_tmp3F0;_LL287:;}{void*_tmp3F4=(void*)_tmp3A2->r;
void*_tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F6;void*_tmp3F7;struct Cyc_Absyn_Vardecl*
_tmp3F8;void*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3FA;void*_tmp3FB;struct Cyc_Absyn_Vardecl*
_tmp3FC;_LL28D: if(*((int*)_tmp3F4)!= 1)goto _LL28F;_tmp3F5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3F4)->f2;if(_tmp3F5 <= (void*)1)goto _LL28F;if(*((int*)_tmp3F5)!= 0)goto _LL28F;
_tmp3F6=((struct Cyc_Absyn_Global_b_struct*)_tmp3F5)->f1;if(!(!_tmp3F6->escapes))
goto _LL28F;_LL28E: _tmp3F8=_tmp3F6;goto _LL290;_LL28F: if(*((int*)_tmp3F4)!= 1)goto
_LL291;_tmp3F7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F4)->f2;if(_tmp3F7 <= (
void*)1)goto _LL291;if(*((int*)_tmp3F7)!= 3)goto _LL291;_tmp3F8=((struct Cyc_Absyn_Local_b_struct*)
_tmp3F7)->f1;if(!(!_tmp3F8->escapes))goto _LL291;_LL290: _tmp3FA=_tmp3F8;goto
_LL292;_LL291: if(*((int*)_tmp3F4)!= 1)goto _LL293;_tmp3F9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3F4)->f2;if(_tmp3F9 <= (void*)1)goto _LL293;if(*((int*)_tmp3F9)!= 4)goto _LL293;
_tmp3FA=((struct Cyc_Absyn_Pat_b_struct*)_tmp3F9)->f1;if(!(!_tmp3FA->escapes))
goto _LL293;_LL292: _tmp3FC=_tmp3FA;goto _LL294;_LL293: if(*((int*)_tmp3F4)!= 1)goto
_LL295;_tmp3FB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F4)->f2;if(_tmp3FB <= (
void*)1)goto _LL295;if(*((int*)_tmp3FB)!= 2)goto _LL295;_tmp3FC=((struct Cyc_Absyn_Param_b_struct*)
_tmp3FB)->f1;if(!(!_tmp3FC->escapes))goto _LL295;_LL294: {void*_tmp3FD=(void*)
_tmp3A3->r;void*_tmp3FE;struct Cyc_Absyn_Vardecl*_tmp3FF;void*_tmp400;struct Cyc_Absyn_Vardecl*
_tmp401;void*_tmp402;struct Cyc_Absyn_Vardecl*_tmp403;void*_tmp404;struct Cyc_Absyn_Vardecl*
_tmp405;union Cyc_Absyn_Cnst_union _tmp406;int _tmp407;void*_tmp408;struct Cyc_List_List*
_tmp409;struct Cyc_List_List _tmp40A;struct Cyc_Absyn_Exp*_tmp40B;_LL298: if(*((int*)
_tmp3FD)!= 1)goto _LL29A;_tmp3FE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FD)->f2;
if(_tmp3FE <= (void*)1)goto _LL29A;if(*((int*)_tmp3FE)!= 0)goto _LL29A;_tmp3FF=((
struct Cyc_Absyn_Global_b_struct*)_tmp3FE)->f1;if(!(!_tmp3FF->escapes))goto _LL29A;
_LL299: _tmp401=_tmp3FF;goto _LL29B;_LL29A: if(*((int*)_tmp3FD)!= 1)goto _LL29C;
_tmp400=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FD)->f2;if(_tmp400 <= (void*)
1)goto _LL29C;if(*((int*)_tmp400)!= 3)goto _LL29C;_tmp401=((struct Cyc_Absyn_Local_b_struct*)
_tmp400)->f1;if(!(!_tmp401->escapes))goto _LL29C;_LL29B: _tmp403=_tmp401;goto
_LL29D;_LL29C: if(*((int*)_tmp3FD)!= 1)goto _LL29E;_tmp402=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3FD)->f2;if(_tmp402 <= (void*)1)goto _LL29E;if(*((int*)_tmp402)!= 4)goto _LL29E;
_tmp403=((struct Cyc_Absyn_Pat_b_struct*)_tmp402)->f1;if(!(!_tmp403->escapes))
goto _LL29E;_LL29D: _tmp405=_tmp403;goto _LL29F;_LL29E: if(*((int*)_tmp3FD)!= 1)goto
_LL2A0;_tmp404=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FD)->f2;if(_tmp404 <= (
void*)1)goto _LL2A0;if(*((int*)_tmp404)!= 2)goto _LL2A0;_tmp405=((struct Cyc_Absyn_Param_b_struct*)
_tmp404)->f1;if(!(!_tmp405->escapes))goto _LL2A0;_LL29F: {struct Cyc_List_List*
_tmp40C=({struct Cyc_List_List*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->vd=
_tmp3FC;_tmp410->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp411;(_tmp411.LessVar).tag=1;(_tmp411.LessVar).f1=_tmp405;_tmp411;});_tmp410;});
_tmp40F->tl=_tmp39F;_tmp40F;});return({struct _tuple7 _tmp40D;_tmp40D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp40E;(_tmp40E.ReachableFL).tag=1;(_tmp40E.ReachableFL).f1=
_tmp39E;(_tmp40E.ReachableFL).f2=_tmp40C;(_tmp40E.ReachableFL).f3=_tmp3A0;
_tmp40E;});_tmp40D.f2=_tmp3ED;_tmp40D;});}_LL2A0: if(*((int*)_tmp3FD)!= 0)goto
_LL2A2;_tmp406=((struct Cyc_Absyn_Const_e_struct*)_tmp3FD)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp3FD)->f1).Int_c).tag != 2)goto _LL2A2;_tmp407=(_tmp406.Int_c).f2;_LL2A1: {
struct Cyc_List_List*_tmp412=({struct Cyc_List_List*_tmp415=_cycalloc(sizeof(*
_tmp415));_tmp415->hd=({struct Cyc_CfFlowInfo_Reln*_tmp416=_cycalloc(sizeof(*
_tmp416));_tmp416->vd=_tmp3FC;_tmp416->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp417;(_tmp417.LessConst).tag=3;(_tmp417.LessConst).f1=(
unsigned int)_tmp407;_tmp417;});_tmp416;});_tmp415->tl=_tmp39F;_tmp415;});return({
struct _tuple7 _tmp413;_tmp413.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp414;(_tmp414.ReachableFL).tag=1;(_tmp414.ReachableFL).f1=_tmp39E;(_tmp414.ReachableFL).f2=
_tmp412;(_tmp414.ReachableFL).f3=_tmp3A0;_tmp414;});_tmp413.f2=_tmp3ED;_tmp413;});}
_LL2A2: if(*((int*)_tmp3FD)!= 3)goto _LL2A4;_tmp408=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3FD)->f1;if((int)_tmp408 != 19)goto _LL2A4;_tmp409=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3FD)->f2;if(_tmp409 == 0)goto _LL2A4;_tmp40A=*_tmp409;_tmp40B=(struct Cyc_Absyn_Exp*)
_tmp40A.hd;_LL2A3: {void*_tmp418=(void*)_tmp40B->r;void*_tmp419;struct Cyc_Absyn_Vardecl*
_tmp41A;void*_tmp41B;struct Cyc_Absyn_Vardecl*_tmp41C;void*_tmp41D;struct Cyc_Absyn_Vardecl*
_tmp41E;void*_tmp41F;struct Cyc_Absyn_Vardecl*_tmp420;_LL2A7: if(*((int*)_tmp418)
!= 1)goto _LL2A9;_tmp419=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp418)->f2;if(
_tmp419 <= (void*)1)goto _LL2A9;if(*((int*)_tmp419)!= 0)goto _LL2A9;_tmp41A=((
struct Cyc_Absyn_Global_b_struct*)_tmp419)->f1;if(!(!_tmp41A->escapes))goto _LL2A9;
_LL2A8: _tmp41C=_tmp41A;goto _LL2AA;_LL2A9: if(*((int*)_tmp418)!= 1)goto _LL2AB;
_tmp41B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp418)->f2;if(_tmp41B <= (void*)
1)goto _LL2AB;if(*((int*)_tmp41B)!= 3)goto _LL2AB;_tmp41C=((struct Cyc_Absyn_Local_b_struct*)
_tmp41B)->f1;if(!(!_tmp41C->escapes))goto _LL2AB;_LL2AA: _tmp41E=_tmp41C;goto
_LL2AC;_LL2AB: if(*((int*)_tmp418)!= 1)goto _LL2AD;_tmp41D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp418)->f2;if(_tmp41D <= (void*)1)goto _LL2AD;if(*((int*)_tmp41D)!= 4)goto _LL2AD;
_tmp41E=((struct Cyc_Absyn_Pat_b_struct*)_tmp41D)->f1;if(!(!_tmp41E->escapes))
goto _LL2AD;_LL2AC: _tmp420=_tmp41E;goto _LL2AE;_LL2AD: if(*((int*)_tmp418)!= 1)goto
_LL2AF;_tmp41F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp418)->f2;if(_tmp41F <= (
void*)1)goto _LL2AF;if(*((int*)_tmp41F)!= 2)goto _LL2AF;_tmp420=((struct Cyc_Absyn_Param_b_struct*)
_tmp41F)->f1;if(!(!_tmp420->escapes))goto _LL2AF;_LL2AE: {struct Cyc_List_List*
_tmp421=({struct Cyc_List_List*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->vd=
_tmp3FC;_tmp425->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp426;(_tmp426.LessSize).tag=2;(_tmp426.LessSize).f1=_tmp420;_tmp426;});
_tmp425;});_tmp424->tl=_tmp39F;_tmp424;});return({struct _tuple7 _tmp422;_tmp422.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp423;(
_tmp423.ReachableFL).tag=1;(_tmp423.ReachableFL).f1=_tmp39E;(_tmp423.ReachableFL).f2=
_tmp421;(_tmp423.ReachableFL).f3=_tmp3A0;_tmp423;});_tmp422.f2=_tmp3ED;_tmp422;});}
_LL2AF:;_LL2B0: return({struct _tuple7 _tmp427;_tmp427.f1=_tmp3EC;_tmp427.f2=
_tmp3ED;_tmp427;});_LL2A6:;}_LL2A4:;_LL2A5: return({struct _tuple7 _tmp428;_tmp428.f1=
_tmp3EC;_tmp428.f2=_tmp3ED;_tmp428;});_LL297:;}_LL295:;_LL296: return({struct
_tuple7 _tmp429;_tmp429.f1=_tmp3EC;_tmp429.f2=_tmp3ED;_tmp429;});_LL28C:;}}_LL283:
if((int)_tmp3E1 != 10)goto _LL285;_LL284: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp42A=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A2,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp42B=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3A3,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp42C=_tmp42A;struct Cyc_Dict_Dict _tmp42D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp42E;_LL2B2: if((_tmp42C.BottomFL).tag != 0)goto _LL2B4;_LL2B3:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({
struct Cyc_Core_Impossible_struct _tmp430;_tmp430.tag=Cyc_Core_Impossible;_tmp430.f1=({
const char*_tmp431="anal_test, Lte";_tag_dynforward(_tmp431,sizeof(char),
_get_zero_arr_size(_tmp431,15));});_tmp430;});_tmp42F;}));_LL2B4: if((_tmp42C.ReachableFL).tag
!= 1)goto _LL2B1;_tmp42D=(_tmp42C.ReachableFL).f1;_tmp42E=(_tmp42C.ReachableFL).f3;
_LL2B5: _tmp39E=_tmp42D;_tmp3A0=_tmp42E;_LL2B1:;}{void*_tmp432=(void*)_tmp3A2->r;
void*_tmp433;struct Cyc_Absyn_Vardecl*_tmp434;void*_tmp435;struct Cyc_Absyn_Vardecl*
_tmp436;void*_tmp437;struct Cyc_Absyn_Vardecl*_tmp438;void*_tmp439;struct Cyc_Absyn_Vardecl*
_tmp43A;_LL2B7: if(*((int*)_tmp432)!= 1)goto _LL2B9;_tmp433=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp432)->f2;if(_tmp433 <= (void*)1)goto _LL2B9;if(*((int*)_tmp433)!= 0)goto _LL2B9;
_tmp434=((struct Cyc_Absyn_Global_b_struct*)_tmp433)->f1;if(!(!_tmp434->escapes))
goto _LL2B9;_LL2B8: _tmp436=_tmp434;goto _LL2BA;_LL2B9: if(*((int*)_tmp432)!= 1)goto
_LL2BB;_tmp435=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp432)->f2;if(_tmp435 <= (
void*)1)goto _LL2BB;if(*((int*)_tmp435)!= 3)goto _LL2BB;_tmp436=((struct Cyc_Absyn_Local_b_struct*)
_tmp435)->f1;if(!(!_tmp436->escapes))goto _LL2BB;_LL2BA: _tmp438=_tmp436;goto
_LL2BC;_LL2BB: if(*((int*)_tmp432)!= 1)goto _LL2BD;_tmp437=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp432)->f2;if(_tmp437 <= (void*)1)goto _LL2BD;if(*((int*)_tmp437)!= 4)goto _LL2BD;
_tmp438=((struct Cyc_Absyn_Pat_b_struct*)_tmp437)->f1;if(!(!_tmp438->escapes))
goto _LL2BD;_LL2BC: _tmp43A=_tmp438;goto _LL2BE;_LL2BD: if(*((int*)_tmp432)!= 1)goto
_LL2BF;_tmp439=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp432)->f2;if(_tmp439 <= (
void*)1)goto _LL2BF;if(*((int*)_tmp439)!= 2)goto _LL2BF;_tmp43A=((struct Cyc_Absyn_Param_b_struct*)
_tmp439)->f1;if(!(!_tmp43A->escapes))goto _LL2BF;_LL2BE: {void*_tmp43B=(void*)
_tmp3A3->r;void*_tmp43C;struct Cyc_List_List*_tmp43D;struct Cyc_List_List _tmp43E;
struct Cyc_Absyn_Exp*_tmp43F;_LL2C2: if(*((int*)_tmp43B)!= 3)goto _LL2C4;_tmp43C=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp43B)->f1;if((int)_tmp43C != 19)goto
_LL2C4;_tmp43D=((struct Cyc_Absyn_Primop_e_struct*)_tmp43B)->f2;if(_tmp43D == 0)
goto _LL2C4;_tmp43E=*_tmp43D;_tmp43F=(struct Cyc_Absyn_Exp*)_tmp43E.hd;_LL2C3: {
void*_tmp440=(void*)_tmp43F->r;void*_tmp441;struct Cyc_Absyn_Vardecl*_tmp442;void*
_tmp443;struct Cyc_Absyn_Vardecl*_tmp444;void*_tmp445;struct Cyc_Absyn_Vardecl*
_tmp446;void*_tmp447;struct Cyc_Absyn_Vardecl*_tmp448;_LL2C7: if(*((int*)_tmp440)
!= 1)goto _LL2C9;_tmp441=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(
_tmp441 <= (void*)1)goto _LL2C9;if(*((int*)_tmp441)!= 0)goto _LL2C9;_tmp442=((
struct Cyc_Absyn_Global_b_struct*)_tmp441)->f1;if(!(!_tmp442->escapes))goto _LL2C9;
_LL2C8: _tmp444=_tmp442;goto _LL2CA;_LL2C9: if(*((int*)_tmp440)!= 1)goto _LL2CB;
_tmp443=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(_tmp443 <= (void*)
1)goto _LL2CB;if(*((int*)_tmp443)!= 3)goto _LL2CB;_tmp444=((struct Cyc_Absyn_Local_b_struct*)
_tmp443)->f1;if(!(!_tmp444->escapes))goto _LL2CB;_LL2CA: _tmp446=_tmp444;goto
_LL2CC;_LL2CB: if(*((int*)_tmp440)!= 1)goto _LL2CD;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp440)->f2;if(_tmp445 <= (void*)1)goto _LL2CD;if(*((int*)_tmp445)!= 4)goto _LL2CD;
_tmp446=((struct Cyc_Absyn_Pat_b_struct*)_tmp445)->f1;if(!(!_tmp446->escapes))
goto _LL2CD;_LL2CC: _tmp448=_tmp446;goto _LL2CE;_LL2CD: if(*((int*)_tmp440)!= 1)goto
_LL2CF;_tmp447=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(_tmp447 <= (
void*)1)goto _LL2CF;if(*((int*)_tmp447)!= 2)goto _LL2CF;_tmp448=((struct Cyc_Absyn_Param_b_struct*)
_tmp447)->f1;if(!(!_tmp448->escapes))goto _LL2CF;_LL2CE: {struct Cyc_List_List*
_tmp449=({struct Cyc_List_List*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->vd=
_tmp43A;_tmp44D->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp44E;(_tmp44E.LessEqSize).tag=4;(_tmp44E.LessEqSize).f1=_tmp448;_tmp44E;});
_tmp44D;});_tmp44C->tl=_tmp39F;_tmp44C;});return({struct _tuple7 _tmp44A;_tmp44A.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp44B;(
_tmp44B.ReachableFL).tag=1;(_tmp44B.ReachableFL).f1=_tmp39E;(_tmp44B.ReachableFL).f2=
_tmp449;(_tmp44B.ReachableFL).f3=_tmp3A0;_tmp44B;});_tmp44A.f2=_tmp42B;_tmp44A;});}
_LL2CF:;_LL2D0: return({struct _tuple7 _tmp44F;_tmp44F.f1=_tmp42A;_tmp44F.f2=
_tmp42B;_tmp44F;});_LL2C6:;}_LL2C4:;_LL2C5: return({struct _tuple7 _tmp450;_tmp450.f1=
_tmp42A;_tmp450.f2=_tmp42B;_tmp450;});_LL2C1:;}_LL2BF:;_LL2C0: return({struct
_tuple7 _tmp451;_tmp451.f1=_tmp42A;_tmp451.f2=_tmp42B;_tmp451;});_LL2B6:;}}_LL285:;
_LL286: return({struct _tuple7 _tmp452;_tmp452.f1=f;_tmp452.f2=f;_tmp452;});_LL278:;}}
_LL245:;}}_LL23C:;_LL23D: goto _LL22F;_LL22F:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp454;void*_tmp455;struct _tuple5 _tmp453=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp454=_tmp453.f1;_tmp455=_tmp453.f2;_tmp454=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp454);{union Cyc_CfFlowInfo_FlowInfo_union _tmp456=_tmp454;struct Cyc_Dict_Dict
_tmp457;_LL2D2: if((_tmp456.BottomFL).tag != 0)goto _LL2D4;_LL2D3: return({struct
_tuple7 _tmp458;_tmp458.f1=_tmp454;_tmp458.f2=_tmp454;_tmp458;});_LL2D4: if((
_tmp456.ReachableFL).tag != 1)goto _LL2D1;_tmp457=(_tmp456.ReachableFL).f1;_LL2D5: {
void*_tmp459=_tmp455;void*_tmp45A;void*_tmp45B;void*_tmp45C;_LL2D7: if((int)
_tmp459 != 0)goto _LL2D9;_LL2D8: return({struct _tuple7 _tmp45D;_tmp45D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp45E;(_tmp45E.BottomFL).tag=0;_tmp45E;});
_tmp45D.f2=_tmp454;_tmp45D;});_LL2D9: if((int)_tmp459 != 2)goto _LL2DB;_LL2DA: goto
_LL2DC;_LL2DB: if((int)_tmp459 != 1)goto _LL2DD;_LL2DC: goto _LL2DE;_LL2DD: if(_tmp459
<= (void*)3)goto _LL2DF;if(*((int*)_tmp459)!= 2)goto _LL2DF;_LL2DE: return({struct
_tuple7 _tmp45F;_tmp45F.f1=_tmp454;_tmp45F.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp460;(_tmp460.BottomFL).tag=0;_tmp460;});
_tmp45F;});_LL2DF: if(_tmp459 <= (void*)3)goto _LL2E1;if(*((int*)_tmp459)!= 0)goto
_LL2E1;_tmp45A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp459)->f1;if((
int)_tmp45A != 0)goto _LL2E1;_LL2E0: goto _LL2E2;_LL2E1: if(_tmp459 <= (void*)3)goto
_LL2E3;if(*((int*)_tmp459)!= 1)goto _LL2E3;_tmp45B=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp459)->f1;if((int)_tmp45B != 0)goto _LL2E3;_LL2E2:({void*_tmp461[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp462="expression may not be initialized";_tag_dynforward(
_tmp462,sizeof(char),_get_zero_arr_size(_tmp462,34));}),_tag_dynforward(_tmp461,
sizeof(void*),0));});return({struct _tuple7 _tmp463;_tmp463.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp465;(_tmp465.BottomFL).tag=0;_tmp465;});
_tmp463.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp464;(_tmp464.BottomFL).tag=0;_tmp464;});_tmp463;});_LL2E3: if(_tmp459 <= (void*)
3)goto _LL2E5;if(*((int*)_tmp459)!= 0)goto _LL2E5;_tmp45C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp459)->f1;_LL2E4: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp454,e,
_tmp45C);_LL2E5: if(_tmp459 <= (void*)3)goto _LL2E7;if(*((int*)_tmp459)!= 1)goto
_LL2E7;_LL2E6: return({struct _tuple7 _tmp466;_tmp466.f1=_tmp454;_tmp466.f2=_tmp454;
_tmp466;});_LL2E7: if(_tmp459 <= (void*)3)goto _LL2E9;if(*((int*)_tmp459)!= 3)goto
_LL2E9;_LL2E8: return({struct _tuple7 _tmp467;_tmp467.f1=_tmp454;_tmp467.f2=_tmp454;
_tmp467;});_LL2E9: if(_tmp459 <= (void*)3)goto _LL2D6;if(*((int*)_tmp459)!= 4)goto
_LL2D6;_LL2EA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp468=
_cycalloc(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_Core_Impossible_struct _tmp469;
_tmp469.tag=Cyc_Core_Impossible;_tmp469.f1=({const char*_tmp46A="anal_test";
_tag_dynforward(_tmp46A,sizeof(char),_get_zero_arr_size(_tmp46A,10));});_tmp469;});
_tmp468;}));_LL2D6:;}_LL2D1:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp46B=flow;struct Cyc_Dict_Dict _tmp46C;
_LL2EC: if((_tmp46B.BottomFL).tag != 0)goto _LL2EE;_LL2ED: return;_LL2EE: if((_tmp46B.ReachableFL).tag
!= 1)goto _LL2EB;_tmp46C=(_tmp46B.ReachableFL).f1;_LL2EF:{struct Cyc_List_List*
_tmp46D=env->param_roots;for(0;_tmp46D != 0;_tmp46D=_tmp46D->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp46C,Cyc_CfFlowInfo_lookup_place(_tmp46C,(struct Cyc_CfFlowInfo_Place*)_tmp46D->hd))
!= (void*)2)({void*_tmp46E[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp46F="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,76));}),
_tag_dynforward(_tmp46E,sizeof(void*),0));});}}return;_LL2EB:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp471;struct Cyc_Core_Opt*_tmp472;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Stmt*
_tmp474;struct Cyc_Position_Segment*_tmp475;struct Cyc_Absyn_Switch_clause*_tmp470=(
struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp471=*_tmp470;_tmp472=_tmp471.pat_vars;
_tmp473=_tmp471.where_clause;_tmp474=_tmp471.body;_tmp475=_tmp471.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp472))->v,Cyc_CfFlowInfo_unknown_all,_tmp475);union
Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp473 != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp477;union Cyc_CfFlowInfo_FlowInfo_union _tmp478;struct _tuple7 _tmp476=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp473);_tmp477=_tmp476.f1;_tmp478=
_tmp476.f2;inflow=_tmp478;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp477,_tmp474);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp474);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp479=
clause_outflow;_LL2F1: if((_tmp479.BottomFL).tag != 0)goto _LL2F3;_LL2F2: goto _LL2F0;
_LL2F3:;_LL2F4: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp47A[0]={};Cyc_Tcutil_terr(
_tmp474->loc,({const char*_tmp47B="switch clause may implicitly fallthru";
_tag_dynforward(_tmp47B,sizeof(char),_get_zero_arr_size(_tmp47B,38));}),
_tag_dynforward(_tmp47A,sizeof(void*),0));});else{({void*_tmp47C[0]={};Cyc_Tcutil_warn(
_tmp474->loc,({const char*_tmp47D="switch clause may implicitly fallthru";
_tag_dynforward(_tmp47D,sizeof(char),_get_zero_arr_size(_tmp47D,38));}),
_tag_dynforward(_tmp47C,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2F0;_LL2F0:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp47E;(_tmp47E.BottomFL).tag=0;_tmp47E;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp47F=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);inflow=_tmp47F->flow;{void*_tmp480=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Stmt*
_tmp484;struct Cyc_Absyn_Stmt*_tmp485;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Stmt*
_tmp487;struct Cyc_Absyn_Stmt*_tmp488;struct _tuple3 _tmp489;struct Cyc_Absyn_Exp*
_tmp48A;struct Cyc_Absyn_Stmt*_tmp48B;struct Cyc_Absyn_Stmt*_tmp48C;struct Cyc_Absyn_Stmt*
_tmp48D;struct _tuple3 _tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Stmt*
_tmp490;struct Cyc_Absyn_Exp*_tmp491;struct _tuple3 _tmp492;struct Cyc_Absyn_Exp*
_tmp493;struct Cyc_Absyn_Stmt*_tmp494;struct _tuple3 _tmp495;struct Cyc_Absyn_Exp*
_tmp496;struct Cyc_Absyn_Stmt*_tmp497;struct Cyc_Absyn_Stmt*_tmp498;struct Cyc_Absyn_Stmt*
_tmp499;struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Switch_clause**_tmp49B;
struct Cyc_Absyn_Switch_clause*_tmp49C;struct Cyc_Absyn_Stmt*_tmp49D;struct Cyc_Absyn_Stmt*
_tmp49E;struct Cyc_Absyn_Stmt*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_List_List*
_tmp4A1;struct Cyc_Absyn_Stmt*_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Decl*
_tmp4A4;struct Cyc_Absyn_Stmt*_tmp4A5;struct Cyc_Absyn_Stmt*_tmp4A6;struct Cyc_Absyn_Tvar*
_tmp4A7;struct Cyc_Absyn_Vardecl*_tmp4A8;int _tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;
struct Cyc_Absyn_Stmt*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;_LL2F6: if((int)_tmp480
!= 0)goto _LL2F8;_LL2F7: return inflow;_LL2F8: if(_tmp480 <= (void*)1)goto _LL31E;if(*((
int*)_tmp480)!= 2)goto _LL2FA;_tmp481=((struct Cyc_Absyn_Return_s_struct*)_tmp480)->f1;
if(_tmp481 != 0)goto _LL2FA;_LL2F9: if(env->noreturn)({void*_tmp4AD[0]={};Cyc_Tcutil_terr(
s->loc,({const char*_tmp4AE="`noreturn' function might return";_tag_dynforward(
_tmp4AE,sizeof(char),_get_zero_arr_size(_tmp4AE,33));}),_tag_dynforward(_tmp4AD,
sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4AF;(_tmp4AF.BottomFL).tag=0;_tmp4AF;});_LL2FA: if(*((int*)_tmp480)!= 2)goto
_LL2FC;_tmp482=((struct Cyc_Absyn_Return_s_struct*)_tmp480)->f1;_LL2FB: if(env->noreturn)({
void*_tmp4B0[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4B1="`noreturn' function might return";
_tag_dynforward(_tmp4B1,sizeof(char),_get_zero_arr_size(_tmp4B1,33));}),
_tag_dynforward(_tmp4B0,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B3;void*_tmp4B4;struct _tuple5 _tmp4B2=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp482));_tmp4B3=_tmp4B2.f1;_tmp4B4=
_tmp4B2.f2;_tmp4B3=Cyc_CfFlowInfo_consume_unique_rvals(_tmp482->loc,_tmp4B3);
_tmp4B3=Cyc_NewControlFlow_use_Rval(env,_tmp482->loc,_tmp4B3,_tmp4B4);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp4B3);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B5;(_tmp4B5.BottomFL).tag=0;_tmp4B5;});}_LL2FC: if(*((int*)_tmp480)!= 0)goto
_LL2FE;_tmp483=((struct Cyc_Absyn_Exp_s_struct*)_tmp480)->f1;_LL2FD: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp483)).f1;goto _LL2F5;_LL2FE: if(*((int*)_tmp480)!= 1)goto _LL300;
_tmp484=((struct Cyc_Absyn_Seq_s_struct*)_tmp480)->f1;_tmp485=((struct Cyc_Absyn_Seq_s_struct*)
_tmp480)->f2;_LL2FF: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp484),_tmp485);goto _LL2F5;_LL300: if(*((int*)_tmp480)!= 3)goto
_LL302;_tmp486=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp480)->f1;_tmp487=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp480)->f2;_tmp488=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp480)->f3;_LL301: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4B7;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B8;struct _tuple7 _tmp4B6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp486);
_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;_tmp4B7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp486->loc,_tmp4B7);_tmp4B8=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp486->loc,
_tmp4B8);outflow=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp4B7,_tmp487),Cyc_NewControlFlow_anal_stmt(env,_tmp4B8,_tmp488),1);goto
_LL2F5;}_LL302: if(*((int*)_tmp480)!= 4)goto _LL304;_tmp489=((struct Cyc_Absyn_While_s_struct*)
_tmp480)->f1;_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;_tmp48C=((struct Cyc_Absyn_While_s_struct*)
_tmp480)->f2;_LL303: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4B9=(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp48B))->flow;union Cyc_CfFlowInfo_FlowInfo_union _tmp4BB;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4BC;struct _tuple7 _tmp4BA=Cyc_NewControlFlow_anal_test(env,_tmp4B9,_tmp48A);
_tmp4BB=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;_tmp4BB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp48A->loc,_tmp4BB);_tmp4BC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp48A->loc,
_tmp4BC);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4BD=Cyc_NewControlFlow_anal_stmt(
env,_tmp4BB,_tmp48C);Cyc_NewControlFlow_update_flow(env,_tmp48B,_tmp4BD);outflow=
_tmp4BC;goto _LL2F5;}}_LL304: if(*((int*)_tmp480)!= 13)goto _LL306;_tmp48D=((struct
Cyc_Absyn_Do_s_struct*)_tmp480)->f1;_tmp48E=((struct Cyc_Absyn_Do_s_struct*)
_tmp480)->f2;_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_LL305: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4BE=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp48D);union Cyc_CfFlowInfo_FlowInfo_union
_tmp4BF=(Cyc_NewControlFlow_pre_stmt_check(env,_tmp4BE,_tmp490))->flow;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C1;union Cyc_CfFlowInfo_FlowInfo_union _tmp4C2;struct _tuple7 _tmp4C0=Cyc_NewControlFlow_anal_test(
env,_tmp4BF,_tmp48F);_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;Cyc_NewControlFlow_update_flow(
env,_tmp48D,_tmp4C1);outflow=_tmp4C2;goto _LL2F5;}_LL306: if(*((int*)_tmp480)!= 8)
goto _LL308;_tmp491=((struct Cyc_Absyn_For_s_struct*)_tmp480)->f1;_tmp492=((struct
Cyc_Absyn_For_s_struct*)_tmp480)->f2;_tmp493=_tmp492.f1;_tmp494=_tmp492.f2;
_tmp495=((struct Cyc_Absyn_For_s_struct*)_tmp480)->f3;_tmp496=_tmp495.f1;_tmp497=
_tmp495.f2;_tmp498=((struct Cyc_Absyn_For_s_struct*)_tmp480)->f4;_LL307: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp4C3=(Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp491)).f1;_tmp4C3=Cyc_CfFlowInfo_drop_unique_rvals(_tmp491->loc,_tmp4C3);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C4=(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp4C3,_tmp494))->flow;union Cyc_CfFlowInfo_FlowInfo_union _tmp4C6;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C7;struct _tuple7 _tmp4C5=Cyc_NewControlFlow_anal_test(env,_tmp4C4,_tmp493);
_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4C8=
Cyc_NewControlFlow_anal_stmt(env,_tmp4C6,_tmp498);union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C9=(Cyc_NewControlFlow_pre_stmt_check(env,_tmp4C8,_tmp497))->flow;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CA=(Cyc_NewControlFlow_anal_Rexp(env,_tmp4C9,_tmp496)).f1;_tmp4CA=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp496->loc,_tmp4CA);Cyc_NewControlFlow_update_flow(env,_tmp494,_tmp4CA);
outflow=_tmp4C7;goto _LL2F5;}}}_LL308: if(*((int*)_tmp480)!= 5)goto _LL30A;_tmp499=((
struct Cyc_Absyn_Break_s_struct*)_tmp480)->f1;if(_tmp499 != 0)goto _LL30A;_LL309:
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CB;(_tmp4CB.BottomFL).tag=0;_tmp4CB;});_LL30A: if(*((int*)_tmp480)!= 10)goto
_LL30C;_tmp49A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp480)->f1;_tmp49B=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp480)->f2;if(_tmp49B == 0)goto _LL30C;
_tmp49C=*_tmp49B;_LL30B: {struct _RegionHandle _tmp4CC=_new_region("temp");struct
_RegionHandle*temp=& _tmp4CC;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CE;struct Cyc_List_List*_tmp4CF;struct _tuple9 _tmp4CD=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp49A,0);_tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;for(0;_tmp4CF != 
0;(_tmp4CF=_tmp4CF->tl,_tmp49A=_tmp49A->tl)){_tmp4CE=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49A))->hd)->loc,
_tmp4CE,(void*)_tmp4CF->hd);}_tmp4CE=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp4CE);_tmp4CE=Cyc_NewControlFlow_add_vars(_tmp4CE,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp49C->pat_vars))->v,Cyc_CfFlowInfo_unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp49C->body,
_tmp4CE);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4D1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D0;(_tmp4D0.BottomFL).tag=0;_tmp4D0;});
_npop_handler(0);return _tmp4D1;}};_pop_region(temp);}_LL30C: if(*((int*)_tmp480)
!= 5)goto _LL30E;_tmp49D=((struct Cyc_Absyn_Break_s_struct*)_tmp480)->f1;_LL30D:
_tmp49E=_tmp49D;goto _LL30F;_LL30E: if(*((int*)_tmp480)!= 6)goto _LL310;_tmp49E=((
struct Cyc_Absyn_Continue_s_struct*)_tmp480)->f1;_LL30F: _tmp49F=_tmp49E;goto
_LL311;_LL310: if(*((int*)_tmp480)!= 7)goto _LL312;_tmp49F=((struct Cyc_Absyn_Goto_s_struct*)
_tmp480)->f2;_LL311: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp4D2=
_tmp47F->encloser;struct Cyc_Absyn_Stmt*_tmp4D3=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp49F)))->encloser;while(_tmp4D3 != _tmp4D2){
struct Cyc_Absyn_Stmt*_tmp4D4=(Cyc_NewControlFlow_get_stmt_annot(_tmp4D2))->encloser;
if(_tmp4D4 == _tmp4D2){({void*_tmp4D5[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp4D6="goto enters local scope or exception handler";_tag_dynforward(_tmp4D6,
sizeof(char),_get_zero_arr_size(_tmp4D6,45));}),_tag_dynforward(_tmp4D5,sizeof(
void*),0));});break;}_tmp4D2=_tmp4D4;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp49F),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D7;(_tmp4D7.BottomFL).tag=0;_tmp4D7;});
_LL312: if(*((int*)_tmp480)!= 9)goto _LL314;_tmp4A0=((struct Cyc_Absyn_Switch_s_struct*)
_tmp480)->f1;_tmp4A1=((struct Cyc_Absyn_Switch_s_struct*)_tmp480)->f2;_LL313: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D9;void*_tmp4DA;struct _tuple5 _tmp4D8=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4A0);_tmp4D9=_tmp4D8.f1;_tmp4DA=_tmp4D8.f2;_tmp4D9=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4A0->loc,_tmp4D9);_tmp4D9=Cyc_NewControlFlow_use_Rval(env,_tmp4A0->loc,
_tmp4D9,_tmp4DA);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp4D9,_tmp4A1);goto
_LL2F5;}_LL314: if(*((int*)_tmp480)!= 14)goto _LL316;_tmp4A2=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp480)->f1;_tmp4A3=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp480)->f2;_LL315: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4A2);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4A3);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4DB=scs_outflow;
_LL321: if((_tmp4DB.BottomFL).tag != 0)goto _LL323;_LL322: goto _LL320;_LL323:;_LL324:({
void*_tmp4DC[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4DD="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp4DD,sizeof(char),_get_zero_arr_size(_tmp4DD,42));}),
_tag_dynforward(_tmp4DC,sizeof(void*),0));});_LL320:;}outflow=s1_outflow;goto
_LL2F5;}}}_LL316: if(*((int*)_tmp480)!= 11)goto _LL318;_tmp4A4=((struct Cyc_Absyn_Decl_s_struct*)
_tmp480)->f1;_tmp4A5=((struct Cyc_Absyn_Decl_s_struct*)_tmp480)->f2;_LL317:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4A4),_tmp4A5);goto _LL2F5;_LL318: if(*((int*)_tmp480)!= 12)goto _LL31A;_tmp4A6=((
struct Cyc_Absyn_Label_s_struct*)_tmp480)->f2;_LL319: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4A6);goto _LL2F5;_LL31A: if(*((int*)_tmp480)!= 15)goto _LL31C;
_tmp4A7=((struct Cyc_Absyn_Region_s_struct*)_tmp480)->f1;_tmp4A8=((struct Cyc_Absyn_Region_s_struct*)
_tmp480)->f2;_tmp4A9=((struct Cyc_Absyn_Region_s_struct*)_tmp480)->f3;_tmp4AA=((
struct Cyc_Absyn_Region_s_struct*)_tmp480)->f4;_tmp4AB=((struct Cyc_Absyn_Region_s_struct*)
_tmp480)->f5;_LL31B: if(_tmp4AA != 0){struct Cyc_Absyn_Exp*_tmp4DE=(struct Cyc_Absyn_Exp*)
_tmp4AA;union Cyc_CfFlowInfo_FlowInfo_union _tmp4E0;void*_tmp4E1;struct _tuple5
_tmp4DF=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4DE);_tmp4E0=_tmp4DF.f1;
_tmp4E1=_tmp4DF.f2;_tmp4E0=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4DE->loc,
_tmp4E0);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp4DE->loc,_tmp4E0,_tmp4E1);}{
struct Cyc_List_List _tmp4E2=({struct Cyc_List_List _tmp4E3;_tmp4E3.hd=_tmp4A8;
_tmp4E3.tl=0;_tmp4E3;});inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)&
_tmp4E2,Cyc_CfFlowInfo_unknown_all,_tmp4AB->loc);outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4AB);goto _LL2F5;}_LL31C: if(*((int*)_tmp480)!= 16)goto _LL31E;
_tmp4AC=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp480)->f1;_LL31D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E5;void*_tmp4E6;struct _tuple5 _tmp4E4=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp4AC);_tmp4E5=_tmp4E4.f1;_tmp4E6=_tmp4E4.f2;_tmp4E5=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4AC->loc,_tmp4E5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E7=Cyc_NewControlFlow_use_Rval(
env,_tmp4AC->loc,_tmp4E5,_tmp4E6);{void*_tmp4E8=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4AC->topt))->v);void*_tmp4E9;_LL326: if(
_tmp4E8 <= (void*)4)goto _LL328;if(*((int*)_tmp4E8)!= 15)goto _LL328;_tmp4E9=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4E8)->f1;_LL327: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4E5,_tmp4E9);goto _LL325;_LL328:;_LL329:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=({struct Cyc_Core_Impossible_struct
_tmp4EB;_tmp4EB.tag=Cyc_Core_Impossible;_tmp4EB.f1=({const char*_tmp4EC="anal_stmt -- reset_region";
_tag_dynforward(_tmp4EC,sizeof(char),_get_zero_arr_size(_tmp4EC,26));});_tmp4EB;});
_tmp4EA;}));_LL325:;}goto _LL2F5;}}_LL31E:;_LL31F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({struct Cyc_Core_Impossible_struct
_tmp4EE;_tmp4EE.tag=Cyc_Core_Impossible;_tmp4EE.f1=({const char*_tmp4EF="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp4EF,sizeof(char),_get_zero_arr_size(_tmp4EF,56));});_tmp4EE;});
_tmp4ED;}));_LL2F5:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4F0=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4F1;_LL32B: if((_tmp4F0.ReachableFL).tag != 1)goto _LL32D;_tmp4F1=(_tmp4F0.ReachableFL).f3;
_LL32C: goto _LL32A;_LL32D:;_LL32E: goto _LL32A;_LL32A:;}return outflow;}static void
Cyc_NewControlFlow_check_nested_fun(union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp4F2=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp4F3;struct Cyc_Core_Opt*_tmp4F4;struct Cyc_Core_Opt _tmp4F5;struct Cyc_List_List*
_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Fndecl*
_tmp4F9;_LL330: if(_tmp4F2 <= (void*)2)goto _LL338;if(*((int*)_tmp4F2)!= 0)goto
_LL332;_tmp4F3=((struct Cyc_Absyn_Var_d_struct*)_tmp4F2)->f1;_LL331: {struct Cyc_List_List
_tmp4FA=({struct Cyc_List_List _tmp509;_tmp509.hd=_tmp4F3;_tmp509.tl=0;_tmp509;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp4FA,Cyc_CfFlowInfo_unknown_none,
decl->loc);{struct Cyc_Absyn_Exp*_tmp4FB=_tmp4F3->initializer;if(_tmp4FB == 0)
return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4FD;void*_tmp4FE;struct
_tuple5 _tmp4FC=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_tmp4FB);_tmp4FD=_tmp4FC.f1;_tmp4FE=_tmp4FC.f2;_tmp4FD=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4FB->loc,_tmp4FD);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4FF=_tmp4FD;struct
Cyc_Dict_Dict _tmp500;struct Cyc_List_List*_tmp501;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp502;_LL33B: if((_tmp4FF.BottomFL).tag != 0)goto _LL33D;_LL33C: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp503;(_tmp503.BottomFL).tag=0;_tmp503;});
_LL33D: if((_tmp4FF.ReachableFL).tag != 1)goto _LL33A;_tmp500=(_tmp4FF.ReachableFL).f1;
_tmp501=(_tmp4FF.ReachableFL).f2;_tmp502=(_tmp4FF.ReachableFL).f3;_LL33E: _tmp500=
Cyc_CfFlowInfo_assign_place(decl->loc,_tmp500,env->all_changed,({struct Cyc_CfFlowInfo_Place*
_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp506;_tmp506.tag=0;_tmp506.f1=_tmp4F3;_tmp506;});_tmp505;}));_tmp504->fields=
0;_tmp504;}),_tmp4FE);_tmp501=Cyc_CfFlowInfo_reln_assign_var(_tmp501,_tmp4F3,(
struct Cyc_Absyn_Exp*)_check_null(_tmp4F3->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp507=({union Cyc_CfFlowInfo_FlowInfo_union _tmp508;(_tmp508.ReachableFL).tag=1;(
_tmp508.ReachableFL).f1=_tmp500;(_tmp508.ReachableFL).f2=_tmp501;(_tmp508.ReachableFL).f3=
_tmp502;_tmp508;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp507);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp507;}_LL33A:;}}}}_LL332:
if(*((int*)_tmp4F2)!= 2)goto _LL334;_tmp4F4=((struct Cyc_Absyn_Let_d_struct*)
_tmp4F2)->f2;if(_tmp4F4 == 0)goto _LL334;_tmp4F5=*_tmp4F4;_tmp4F6=(struct Cyc_List_List*)
_tmp4F5.v;_tmp4F7=((struct Cyc_Absyn_Let_d_struct*)_tmp4F2)->f3;_LL333: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp50B;void*_tmp50C;struct _tuple5 _tmp50A=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp4F7);_tmp50B=_tmp50A.f1;_tmp50C=_tmp50A.f2;_tmp50B=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F7->loc,_tmp50B);_tmp50B=Cyc_NewControlFlow_use_Rval(env,_tmp4F7->loc,
_tmp50B,_tmp50C);return Cyc_NewControlFlow_add_vars(_tmp50B,_tmp4F6,Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL334: if(*((int*)_tmp4F2)!= 3)goto _LL336;_tmp4F8=((struct Cyc_Absyn_Letv_d_struct*)
_tmp4F2)->f1;_LL335: return Cyc_NewControlFlow_add_vars(inflow,_tmp4F8,Cyc_CfFlowInfo_unknown_none,
decl->loc);_LL336: if(*((int*)_tmp4F2)!= 1)goto _LL338;_tmp4F9=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4F2)->f1;_LL337: Cyc_NewControlFlow_check_nested_fun(inflow,_tmp4F9);{void*
_tmp50D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F9->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp50E=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp4F9->fn_vardecl);
return Cyc_NewControlFlow_add_vars(inflow,({struct Cyc_List_List*_tmp50F=_cycalloc(
sizeof(*_tmp50F));_tmp50F->hd=_tmp50E;_tmp50F->tl=0;_tmp50F;}),Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL338:;_LL339:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Core_Impossible_struct
_tmp511;_tmp511.tag=Cyc_Core_Impossible;_tmp511.f1=({const char*_tmp512="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp512,sizeof(char),_get_zero_arr_size(_tmp512,35));});_tmp511;});
_tmp510;}));_LL32F:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp513=(void*)s->r;
struct _tuple3 _tmp514;struct Cyc_Absyn_Stmt*_tmp515;struct _tuple3 _tmp516;struct Cyc_Absyn_Stmt*
_tmp517;struct Cyc_Absyn_Stmt*_tmp518;struct Cyc_Absyn_Stmt*_tmp519;struct Cyc_Absyn_Stmt*
_tmp51A;struct _tuple3 _tmp51B;struct Cyc_Absyn_Stmt*_tmp51C;struct Cyc_Absyn_Stmt*
_tmp51D;struct Cyc_Absyn_Stmt*_tmp51E;struct _tuple3 _tmp51F;struct Cyc_Absyn_Stmt*
_tmp520;struct Cyc_Absyn_Stmt*_tmp521;struct Cyc_Absyn_Stmt*_tmp522;struct Cyc_Absyn_Stmt*
_tmp523;struct Cyc_Absyn_Stmt*_tmp524;struct Cyc_Absyn_Stmt*_tmp525;struct Cyc_Absyn_Stmt*
_tmp526;struct Cyc_List_List*_tmp527;struct Cyc_List_List*_tmp528;_LL340: if(
_tmp513 <= (void*)1)goto _LL354;if(*((int*)_tmp513)!= 8)goto _LL342;_tmp514=((
struct Cyc_Absyn_For_s_struct*)_tmp513)->f2;_tmp515=_tmp514.f2;_tmp516=((struct
Cyc_Absyn_For_s_struct*)_tmp513)->f3;_tmp517=_tmp516.f2;_tmp518=((struct Cyc_Absyn_For_s_struct*)
_tmp513)->f4;_LL341: Cyc_NewControlFlow_postpass(_tmp517);_tmp519=_tmp518;_tmp51A=
_tmp515;goto _LL343;_LL342: if(*((int*)_tmp513)!= 1)goto _LL344;_tmp519=((struct Cyc_Absyn_Seq_s_struct*)
_tmp513)->f1;_tmp51A=((struct Cyc_Absyn_Seq_s_struct*)_tmp513)->f2;_LL343: _tmp51C=
_tmp519;_tmp51D=_tmp51A;goto _LL345;_LL344: if(*((int*)_tmp513)!= 4)goto _LL346;
_tmp51B=((struct Cyc_Absyn_While_s_struct*)_tmp513)->f1;_tmp51C=_tmp51B.f2;
_tmp51D=((struct Cyc_Absyn_While_s_struct*)_tmp513)->f2;_LL345: _tmp51E=_tmp51D;
_tmp520=_tmp51C;goto _LL347;_LL346: if(*((int*)_tmp513)!= 13)goto _LL348;_tmp51E=((
struct Cyc_Absyn_Do_s_struct*)_tmp513)->f1;_tmp51F=((struct Cyc_Absyn_Do_s_struct*)
_tmp513)->f2;_tmp520=_tmp51F.f2;_LL347: _tmp521=_tmp51E;_tmp522=_tmp520;goto
_LL349;_LL348: if(*((int*)_tmp513)!= 3)goto _LL34A;_tmp521=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp513)->f2;_tmp522=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp513)->f3;_LL349:
Cyc_NewControlFlow_postpass(_tmp521);_tmp523=_tmp522;goto _LL34B;_LL34A: if(*((int*)
_tmp513)!= 15)goto _LL34C;_tmp523=((struct Cyc_Absyn_Region_s_struct*)_tmp513)->f5;
_LL34B: _tmp524=_tmp523;goto _LL34D;_LL34C: if(*((int*)_tmp513)!= 12)goto _LL34E;
_tmp524=((struct Cyc_Absyn_Label_s_struct*)_tmp513)->f2;_LL34D: _tmp525=_tmp524;
goto _LL34F;_LL34E: if(*((int*)_tmp513)!= 11)goto _LL350;_tmp525=((struct Cyc_Absyn_Decl_s_struct*)
_tmp513)->f2;_LL34F: Cyc_NewControlFlow_postpass(_tmp525);return;_LL350: if(*((int*)
_tmp513)!= 14)goto _LL352;_tmp526=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp513)->f1;
_tmp527=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp513)->f2;_LL351: Cyc_NewControlFlow_postpass(
_tmp526);_tmp528=_tmp527;goto _LL353;_LL352: if(*((int*)_tmp513)!= 9)goto _LL354;
_tmp528=((struct Cyc_Absyn_Switch_s_struct*)_tmp513)->f2;_LL353: for(0;_tmp528 != 0;
_tmp528=_tmp528->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp528->hd)->body);}return;_LL354:;_LL355: return;_LL33F:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){Cyc_NewControlFlow_check_nested_fun((union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp529;(_tmp529.ReachableFL).tag=1;(_tmp529.ReachableFL).f1=
Cyc_CfFlowInfo_empty_flowdict();(_tmp529.ReachableFL).f2=0;(_tmp529.ReachableFL).f3=({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp52A;_tmp52A.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp52A.may_consume=0;_tmp52A;});_tmp529;}),fd);}static void Cyc_NewControlFlow_check_nested_fun(
union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){{struct
_RegionHandle _tmp52B=_new_region("frgn");struct _RegionHandle*frgn=& _tmp52B;
_push_region(frgn);{struct Cyc_Position_Segment*_tmp52C=(fd->body)->loc;inflow=
Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all,_tmp52C);{struct
Cyc_List_List*param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp52D=inflow;
struct Cyc_Dict_Dict _tmp52E;struct Cyc_List_List*_tmp52F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp530;_LL357: if((_tmp52D.BottomFL).tag != 0)goto _LL359;_LL358:({void*_tmp531[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp532="check_fun";_tag_dynforward(_tmp532,sizeof(char),
_get_zero_arr_size(_tmp532,10));}),_tag_dynforward(_tmp531,sizeof(void*),0));});
_LL359: if((_tmp52D.ReachableFL).tag != 1)goto _LL356;_tmp52E=(_tmp52D.ReachableFL).f1;
_tmp52F=(_tmp52D.ReachableFL).f2;_tmp530=(_tmp52D.ReachableFL).f3;_LL35A: {
struct Cyc_List_List*atts;{void*_tmp533=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp534;struct Cyc_List_List*
_tmp535;_LL35C: if(_tmp533 <= (void*)4)goto _LL35E;if(*((int*)_tmp533)!= 8)goto
_LL35E;_tmp534=((struct Cyc_Absyn_FnType_struct*)_tmp533)->f1;_tmp535=_tmp534.attributes;
_LL35D: atts=_tmp535;goto _LL35B;_LL35E:;_LL35F:({void*_tmp536[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp537="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp537,sizeof(char),_get_zero_arr_size(_tmp537,50));}),_tag_dynforward(_tmp536,
sizeof(void*),0));});_LL35B:;}for(0;atts != 0;atts=atts->tl){void*_tmp538=(void*)
atts->hd;int _tmp539;_LL361: if(_tmp538 <= (void*)17)goto _LL363;if(*((int*)_tmp538)
!= 4)goto _LL363;_tmp539=((struct Cyc_Absyn_Initializes_att_struct*)_tmp538)->f1;
_LL362: {unsigned int j=(unsigned int)_tmp539;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp53A[0]={};Cyc_Tcutil_terr(_tmp52C,({const char*_tmp53B="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp53B,sizeof(char),_get_zero_arr_size(_tmp53B,51));}),
_tag_dynforward(_tmp53A,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp53C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp53D=Cyc_Tcutil_compress((void*)_tmp53C->type);struct Cyc_Absyn_PtrInfo
_tmp53E;void*_tmp53F;struct Cyc_Absyn_PtrAtts _tmp540;struct Cyc_Absyn_Conref*
_tmp541;struct Cyc_Absyn_Conref*_tmp542;struct Cyc_Absyn_Conref*_tmp543;_LL366: if(
_tmp53D <= (void*)4)goto _LL368;if(*((int*)_tmp53D)!= 4)goto _LL368;_tmp53E=((
struct Cyc_Absyn_PointerType_struct*)_tmp53D)->f1;_tmp53F=(void*)_tmp53E.elt_typ;
_tmp540=_tmp53E.ptr_atts;_tmp541=_tmp540.nullable;_tmp542=_tmp540.bounds;_tmp543=
_tmp540.zero_term;_LL367: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp541))({void*_tmp544[0]={};Cyc_Tcutil_terr(_tmp52C,({const char*_tmp545="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp545,sizeof(char),_get_zero_arr_size(_tmp545,55));}),
_tag_dynforward(_tmp544,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp542))({
void*_tmp546[0]={};Cyc_Tcutil_terr(_tmp52C,({const char*_tmp547="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp547,sizeof(char),_get_zero_arr_size(_tmp547,57));}),
_tag_dynforward(_tmp546,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp543))({void*_tmp548[0]={};Cyc_Tcutil_terr(_tmp52C,({
const char*_tmp549="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp549,sizeof(char),_get_zero_arr_size(_tmp549,77));}),
_tag_dynforward(_tmp548,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp54A=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp552=_cycalloc(sizeof(*
_tmp552));_tmp552[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp553;_tmp553.tag=
2;_tmp553.f1=(int)j;_tmp553.f2=(void*)_tmp53F;_tmp553;});_tmp552;});struct Cyc_CfFlowInfo_Place*
_tmp54B=({struct Cyc_CfFlowInfo_Place*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->root=(
void*)((void*)_tmp54A);_tmp551->fields=0;_tmp551;});_tmp52E=Cyc_Dict_insert(
_tmp52E,(void*)_tmp54A,Cyc_CfFlowInfo_typ_to_absrval(_tmp53F,Cyc_CfFlowInfo_esc_none));
_tmp52E=Cyc_Dict_insert(_tmp52E,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp54D;_tmp54D.tag=0;_tmp54D.f1=_tmp53C;_tmp54D;});_tmp54C;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp54F;_tmp54F.tag=2;_tmp54F.f1=_tmp54B;
_tmp54F;});_tmp54E;}));param_roots=({struct Cyc_List_List*_tmp550=_region_malloc(
frgn,sizeof(*_tmp550));_tmp550->hd=_tmp54B;_tmp550->tl=param_roots;_tmp550;});
goto _LL365;}_LL368:;_LL369:({void*_tmp554[0]={};Cyc_Tcutil_terr(_tmp52C,({const
char*_tmp555="initializes attribute on non-pointer";_tag_dynforward(_tmp555,
sizeof(char),_get_zero_arr_size(_tmp555,37));}),_tag_dynforward(_tmp554,sizeof(
void*),0));});_LL365:;}goto _LL360;}}_LL363:;_LL364: goto _LL360;_LL360:;}inflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp556;(
_tmp556.ReachableFL).tag=1;(_tmp556.ReachableFL).f1=_tmp52E;(_tmp556.ReachableFL).f2=
_tmp52F;(_tmp556.ReachableFL).f3=_tmp530;_tmp556;});}_LL356:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp55F=_region_malloc(frgn,sizeof(*_tmp55F));_tmp55F->iterate_again=1;_tmp55F->iteration_num=
0;_tmp55F->in_try=0;_tmp55F->tryflow=inflow;_tmp55F->all_changed=0;_tmp55F->noreturn=
noreturn;_tmp55F->param_roots=param_roots;_tmp55F;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp557=outflow;_LL36B: if((_tmp557.BottomFL).tag != 0)goto _LL36D;_LL36C: goto
_LL36A;_LL36D:;_LL36E: Cyc_NewControlFlow_check_init_params(_tmp52C,env,outflow);
if(noreturn)({void*_tmp558[0]={};Cyc_Tcutil_terr(_tmp52C,({const char*_tmp559="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp559,sizeof(char),_get_zero_arr_size(_tmp559,46));}),
_tag_dynforward(_tmp558,sizeof(void*),0));});else{void*_tmp55A=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL370: if((int)_tmp55A != 0)goto _LL372;_LL371: goto _LL36F;
_LL372: if(_tmp55A <= (void*)4)goto _LL374;if(*((int*)_tmp55A)!= 6)goto _LL374;
_LL373: goto _LL375;_LL374: if((int)_tmp55A != 1)goto _LL376;_LL375: goto _LL377;_LL376:
if(_tmp55A <= (void*)4)goto _LL378;if(*((int*)_tmp55A)!= 5)goto _LL378;_LL377:({
void*_tmp55B[0]={};Cyc_Tcutil_warn(_tmp52C,({const char*_tmp55C="function may not return a value";
_tag_dynforward(_tmp55C,sizeof(char),_get_zero_arr_size(_tmp55C,32));}),
_tag_dynforward(_tmp55B,sizeof(void*),0));});goto _LL36F;_LL378:;_LL379:({void*
_tmp55D[0]={};Cyc_Tcutil_terr(_tmp52C,({const char*_tmp55E="function may not return a value";
_tag_dynforward(_tmp55E,sizeof(char),_get_zero_arr_size(_tmp55E,32));}),
_tag_dynforward(_tmp55D,sizeof(void*),0));});goto _LL36F;_LL36F:;}goto _LL36A;
_LL36A:;}}}};_pop_region(frgn);}Cyc_NewControlFlow_postpass(fd->body);}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp560=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List*
_tmp563;struct Cyc_List_List*_tmp564;_LL37B: if(_tmp560 <= (void*)2)goto _LL385;if(*((
int*)_tmp560)!= 1)goto _LL37D;_tmp561=((struct Cyc_Absyn_Fn_d_struct*)_tmp560)->f1;
_LL37C: Cyc_NewControlFlow_check_fun(_tmp561);goto _LL37A;_LL37D: if(*((int*)
_tmp560)!= 10)goto _LL37F;_tmp562=((struct Cyc_Absyn_ExternC_d_struct*)_tmp560)->f1;
_LL37E: _tmp563=_tmp562;goto _LL380;_LL37F: if(*((int*)_tmp560)!= 9)goto _LL381;
_tmp563=((struct Cyc_Absyn_Using_d_struct*)_tmp560)->f2;_LL380: _tmp564=_tmp563;
goto _LL382;_LL381: if(*((int*)_tmp560)!= 8)goto _LL383;_tmp564=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp560)->f2;_LL382: Cyc_NewControlFlow_cf_check(_tmp564);goto _LL37A;_LL383: if(*((
int*)_tmp560)!= 11)goto _LL385;_LL384: goto _LL37A;_LL385:;_LL386: goto _LL37A;_LL37A:;}}
