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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};extern char Cyc_Core_Open_Region[16];extern char
Cyc_Core_Free_Region[16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,struct
_dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
struct _dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dynforward_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(
struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_zstrcmp(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_zstrptrcmp(struct _dynforward_ptr*,struct
_dynforward_ptr*);struct _dynforward_ptr Cyc_strcat(struct _dynforward_ptr dest,
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strcpy(struct _dynforward_ptr
dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dynforward_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(struct
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
le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(
void*t);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dynforward_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_Parse_err((struct
_dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)
Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _dynforward_ptr msg,struct Cyc_Position_Segment*
sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)msg);{struct Cyc_String_pa_struct _tmp2;
_tmp2.tag=0;_tmp2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
sg));{void*_tmp0[2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,46));}),
_tag_dynforward(_tmp0,sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp4=_cycalloc(
sizeof(*_tmp4));_tmp4->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp9;(_tmp9.Rel_n).tag=1;(_tmp9.Rel_n).f1=0;_tmp9;});_tmp4->f2=({struct
_dynforward_ptr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(unsigned long)Cyc_Parse_enum_counter
++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({const char*_tmp7="__anonymous_enum_%d__";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,22));}),
_tag_dynforward(_tmp6,sizeof(void*),1));}});_tmp5;});_tmp4;});}struct _tuple7{
struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct
Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple8*field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct
_tuple7 _tmpD;struct _tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct
Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;
struct _tuple8*_tmpA=field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=
_tmpD.f1;_tmpF=_tmpD.f2;_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=
_tmpB.f2;if(_tmp11 != 0)Cyc_Parse_err(({const char*_tmp14="bad type params in struct field";
_tag_dynforward(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,32));}),loc);if(
Cyc_Absyn_is_qvar_qualified(_tmpE))Cyc_Parse_err(({const char*_tmp15="struct or union field cannot be qualified with a namespace";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,59));}),loc);
return({struct Cyc_Absyn_Aggrfield*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->name=(*
_tmpE).f2;_tmp16->tq=_tmpF;_tmp16->type=(void*)_tmp10;_tmp16->width=_tmp13;
_tmp16->attributes=_tmp12;_tmp16;});}static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Parse_Type_spec_struct
_tmp18;_tmp18.tag=4;_tmp18.f1=(void*)t;_tmp18.f2=loc;_tmp18;});_tmp17;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: return Cyc_Absyn_starb_typ(
_tmp1B,argposn?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp20=_cycalloc(sizeof(*
_tmp20));_tmp20->v=(void*)((void*)3);_tmp20;}),0):(void*)2,_tmp1C,_tmp1D == 0?(
void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp21=_cycalloc(sizeof(*_tmp21));
_tmp21[0]=({struct Cyc_Absyn_Upper_b_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct
Cyc_Absyn_Exp*)_tmp1D;_tmp22;});_tmp21;}),_tmp1E);_LL3:;_LL4: return t;_LL0:;}
static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t){return({
struct _tuple9*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->f1=(*t).f2;_tmp23->f2=(*
t).f3;_tmp23;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp24=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp25;_LL6: if(_tmp24 <= (void*)
2)goto _LL8;if(*((int*)_tmp24)!= 0)goto _LL8;_tmp25=((struct Cyc_Absyn_Var_d_struct*)
_tmp24)->f1;_LL7: return _tmp25;_LL8:;_LL9:({void*_tmp26[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(d->loc,({
const char*_tmp27="bad declaration in `forarray' statement";_tag_dynforward(
_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,40));}),_tag_dynforward(_tmp26,
sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp28=
tm;_LLB: if(*((int*)_tmp28)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}
static void*Cyc_Parse_id2type(struct _dynforward_ptr s,void*k){if(Cyc_zstrcmp((
struct _dynforward_ptr)s,({const char*_tmp29="`H";_tag_dynforward(_tmp29,sizeof(
char),_get_zero_arr_size(_tmp29,3));}))== 0)return(void*)2;else{if(Cyc_zstrcmp((
struct _dynforward_ptr)s,({const char*_tmp2A="`U";_tag_dynforward(_tmp2A,sizeof(
char),_get_zero_arr_size(_tmp2A,3));}))== 0)return(void*)3;else{return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({
struct Cyc_Absyn_VarType_struct _tmp2C;_tmp2C.tag=1;_tmp2C.f1=({struct Cyc_Absyn_Tvar*
_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=({struct _dynforward_ptr*_tmp2E=
_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=s;_tmp2E;});_tmp2D->identity=0;_tmp2D->kind=(
void*)k;_tmp2D;});_tmp2C;});_tmp2B;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp2F=Cyc_Absyn_compress_kb((void*)t->kind);
void*_tmp30;void*_tmp31;_LL10: if(*((int*)_tmp2F)!= 0)goto _LL12;_tmp30=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp2F)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp33;
_tmp33.tag=0;_tmp33.f1=(void*)_tmp30;_tmp33;});_tmp32;});goto _LLF;_LL12: if(*((
int*)_tmp2F)!= 1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp35;_tmp35.tag=1;_tmp35.f1=0;_tmp35;});_tmp34;});goto _LLF;_LL14: if(*((int*)
_tmp2F)!= 2)goto _LLF;_tmp31=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2F)->f2;
_LL15: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp36=_cycalloc(sizeof(*_tmp36));
_tmp36[0]=({struct Cyc_Absyn_Less_kb_struct _tmp37;_tmp37.tag=2;_tmp37.f1=0;_tmp37.f2=(
void*)_tmp31;_tmp37;});_tmp36;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->name=t->name;_tmp38->identity=0;_tmp38->kind=(
void*)k;_tmp38;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp39=t;struct Cyc_Absyn_Tvar*_tmp3A;_LL17: if(_tmp39 <= (void*)4)
goto _LL19;if(*((int*)_tmp39)!= 1)goto _LL19;_tmp3A=((struct Cyc_Absyn_VarType_struct*)
_tmp39)->f1;_LL18: return _tmp3A;_LL19:;_LL1A:({void*_tmp3B[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(
loc,({const char*_tmp3C="expecting a list of type variables, not types";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,46));}),
_tag_dynforward(_tmp3B,sizeof(void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_VarType_struct _tmp3E;
_tmp3E.tag=1;_tmp3E.f1=pr;_tmp3E;});_tmp3D;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k){void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp40;
struct Cyc_Absyn_Tvar _tmp41;void*_tmp42;void**_tmp43;_LL1C: if(_tmp3F <= (void*)4)
goto _LL1E;if(*((int*)_tmp3F)!= 1)goto _LL1E;_tmp40=((struct Cyc_Absyn_VarType_struct*)
_tmp3F)->f1;_tmp41=*_tmp40;_tmp42=(void*)_tmp41.kind;_tmp43=(void**)&(*((struct
Cyc_Absyn_VarType_struct*)_tmp3F)->f1).kind;_LL1D: {void*_tmp44=Cyc_Absyn_compress_kb(*
_tmp43);_LL21: if(*((int*)_tmp44)!= 1)goto _LL23;_LL22:*_tmp43=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp46;
_tmp46.tag=0;_tmp46.f1=(void*)k;_tmp46;});_tmp45;});return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp47=(void*)tms->hd;void*_tmp48;
_LL26: if(*((int*)_tmp47)!= 3)goto _LL28;_tmp48=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp47)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp49=_tmp48;struct Cyc_List_List*_tmp4A;_LL2B: if(*((int*)_tmp49)!= 1)
goto _LL2D;_LL2C:({void*_tmp4B[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4C="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dynforward(_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,93));}),
_tag_dynforward(_tmp4B,sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp49)!= 
0)goto _LL2A;_tmp4A=((struct Cyc_Absyn_NoTypes_struct*)_tmp49)->f1;_LL2E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp4A)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmp4D[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp4E="wrong number of parameter declarations in old-style function declaration";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,73));}),
_tag_dynforward(_tmp4D,sizeof(void*),0));});{struct Cyc_List_List*_tmp4F=0;for(0;
_tmp4A != 0;_tmp4A=_tmp4A->tl){struct Cyc_List_List*_tmp50=tds;for(0;_tmp50 != 0;
_tmp50=_tmp50->tl){struct Cyc_Absyn_Decl*_tmp51=(struct Cyc_Absyn_Decl*)_tmp50->hd;
void*_tmp52=(void*)_tmp51->r;struct Cyc_Absyn_Vardecl*_tmp53;_LL30: if(_tmp52 <= (
void*)2)goto _LL32;if(*((int*)_tmp52)!= 0)goto _LL32;_tmp53=((struct Cyc_Absyn_Var_d_struct*)
_tmp52)->f1;_LL31: if(Cyc_zstrptrcmp((*_tmp53->name).f2,(struct _dynforward_ptr*)
_tmp4A->hd)!= 0)continue;if(_tmp53->initializer != 0)({void*_tmp54[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Parse_abort)(_tmp51->loc,({const char*_tmp55="initializer found in parameter declaration";
_tag_dynforward(_tmp55,sizeof(char),_get_zero_arr_size(_tmp55,43));}),
_tag_dynforward(_tmp54,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(
_tmp53->name))({void*_tmp56[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp51->loc,({const
char*_tmp57="namespaces forbidden in parameter declarations";_tag_dynforward(
_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,47));}),_tag_dynforward(_tmp56,
sizeof(void*),0));});_tmp4F=({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58->hd=({struct _tuple2*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=({
struct Cyc_Core_Opt*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->v=(*_tmp53->name).f2;
_tmp5A;});_tmp59->f2=_tmp53->tq;_tmp59->f3=(void*)_tmp53->type;_tmp59;});_tmp58->tl=
_tmp4F;_tmp58;});goto L;_LL32:;_LL33:({void*_tmp5B[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp51->loc,({
const char*_tmp5C="nonvariable declaration in parameter type";_tag_dynforward(
_tmp5C,sizeof(char),_get_zero_arr_size(_tmp5C,42));}),_tag_dynforward(_tmp5B,
sizeof(void*),0));});_LL2F:;}L: if(_tmp50 == 0)({struct Cyc_String_pa_struct _tmp5F;
_tmp5F.tag=0;_tmp5F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmp4A->hd));{void*_tmp5D[1]={& _tmp5F};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp5E="%s is not given a type";_tag_dynforward(_tmp5E,sizeof(char),
_get_zero_arr_size(_tmp5E,23));}),_tag_dynforward(_tmp5D,sizeof(void*),1));}});}
return({struct Cyc_List_List*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp61=_cycalloc(sizeof(*_tmp61));
_tmp61[0]=({struct Cyc_Absyn_Function_mod_struct _tmp62;_tmp62.tag=3;_tmp62.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63[0]=({struct Cyc_Absyn_WithTypes_struct _tmp64;_tmp64.tag=1;_tmp64.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F);_tmp64.f2=
0;_tmp64.f3=0;_tmp64.f4=0;_tmp64.f5=0;_tmp64;});_tmp63;}));_tmp62;});_tmp61;}));
_tmp60->tl=0;_tmp60;});}_LL2A:;}goto _LL29;_LL28:;_LL29: return({struct Cyc_List_List*
_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->hd=(void*)((void*)tms->hd);_tmp65->tl=
Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp65;});_LL25:;}}struct _tuple10{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp66=_cycalloc(sizeof(*_tmp66));
_tmp66->id=d->id;_tmp66->tms=Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmp66;});{
void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmp67=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL35: if((int)_tmp67 != 1)goto _LL37;_LL36: sc=(void*)3;goto _LL34;
_LL37: if((int)_tmp67 != 3)goto _LL39;_LL38: sc=(void*)0;goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({
const char*_tmp68="bad storage class on function";_tag_dynforward(_tmp68,sizeof(
char),_get_zero_arr_size(_tmp68,30));}),loc);goto _LL34;_LL34:;}}{void*_tmp6A;
struct Cyc_Core_Opt*_tmp6B;struct _tuple5 _tmp69=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{struct Cyc_Absyn_Tqual _tmp6D;void*
_tmp6E;struct Cyc_List_List*_tmp6F;struct Cyc_List_List*_tmp70;struct _tuple6 _tmp6C=
Cyc_Parse_apply_tms(tq,_tmp6A,atts,d->tms);_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;
_tmp6F=_tmp6C.f3;_tmp70=_tmp6C.f4;if(_tmp6B != 0)({void*_tmp71[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp72="nested type declaration within function prototype";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,50));}),
_tag_dynforward(_tmp71,sizeof(void*),0));});if(_tmp6F != 0)({void*_tmp73[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp74="bad type params, ignoring";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,26));}),
_tag_dynforward(_tmp73,sizeof(void*),0));});{void*_tmp75=_tmp6E;struct Cyc_Absyn_FnInfo
_tmp76;struct Cyc_List_List*_tmp77;struct Cyc_Core_Opt*_tmp78;void*_tmp79;struct
Cyc_List_List*_tmp7A;int _tmp7B;struct Cyc_Absyn_VarargInfo*_tmp7C;struct Cyc_List_List*
_tmp7D;struct Cyc_List_List*_tmp7E;_LL3C: if(_tmp75 <= (void*)4)goto _LL3E;if(*((int*)
_tmp75)!= 8)goto _LL3E;_tmp76=((struct Cyc_Absyn_FnType_struct*)_tmp75)->f1;_tmp77=
_tmp76.tvars;_tmp78=_tmp76.effect;_tmp79=(void*)_tmp76.ret_typ;_tmp7A=_tmp76.args;
_tmp7B=_tmp76.c_varargs;_tmp7C=_tmp76.cyc_varargs;_tmp7D=_tmp76.rgn_po;_tmp7E=
_tmp76.attributes;_LL3D: {struct Cyc_List_List*_tmp7F=0;{struct Cyc_List_List*
_tmp80=_tmp7A;for(0;_tmp80 != 0;_tmp80=_tmp80->tl){struct _tuple2 _tmp82;struct Cyc_Core_Opt*
_tmp83;struct Cyc_Absyn_Tqual _tmp84;void*_tmp85;struct _tuple2*_tmp81=(struct
_tuple2*)_tmp80->hd;_tmp82=*_tmp81;_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_tmp85=
_tmp82.f3;if(_tmp83 == 0){Cyc_Parse_err(({const char*_tmp86="missing argument variable in function prototype";
_tag_dynforward(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,48));}),loc);
_tmp7F=({struct Cyc_List_List*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->hd=({
struct _tuple10*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=_init_dynforward_ptr(
_cycalloc(sizeof(struct _dynforward_ptr)),"?",sizeof(char),2);_tmp88->f2=_tmp84;
_tmp88->f3=_tmp85;_tmp88;});_tmp87->tl=_tmp7F;_tmp87;});}else{_tmp7F=({struct Cyc_List_List*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->hd=({struct _tuple10*_tmp8B=_cycalloc(
sizeof(*_tmp8B));_tmp8B->f1=(struct _dynforward_ptr*)_tmp83->v;_tmp8B->f2=_tmp84;
_tmp8B->f3=_tmp85;_tmp8B;});_tmp8A->tl=_tmp7F;_tmp8A;});}}}return({struct Cyc_Absyn_Fndecl*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->sc=(void*)sc;_tmp8C->name=d->id;_tmp8C->tvs=
_tmp77;_tmp8C->is_inline=is_inline;_tmp8C->effect=_tmp78;_tmp8C->ret_type=(void*)
_tmp79;_tmp8C->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp7F);_tmp8C->c_varargs=_tmp7B;_tmp8C->cyc_varargs=_tmp7C;_tmp8C->rgn_po=
_tmp7D;_tmp8C->body=body;_tmp8C->cached_typ=0;_tmp8C->param_vardecls=0;_tmp8C->fn_vardecl=
0;_tmp8C->attributes=Cyc_List_append(_tmp7E,_tmp70);_tmp8C;});}_LL3E:;_LL3F:({
void*_tmp8D[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp8E="declarator is not a function prototype";
_tag_dynforward(_tmp8E,sizeof(char),_get_zero_arr_size(_tmp8E,39));}),
_tag_dynforward(_tmp8D,sizeof(void*),0));});_LL3B:;}}}}}static char _tmp8F[52]="at most one type may appear within a type specifier";
static struct _dynforward_ptr Cyc_Parse_msg1={_tmp8F,_tmp8F + 52};static char _tmp90[
63]="const or volatile may appear only once within a type specifier";static
struct _dynforward_ptr Cyc_Parse_msg2={_tmp90,_tmp90 + 63};static char _tmp91[50]="type specifier includes more than one declaration";
static struct _dynforward_ptr Cyc_Parse_msg3={_tmp91,_tmp91 + 50};static char _tmp92[
60]="sign specifier may appear only once within a type specifier";static struct
_dynforward_ptr Cyc_Parse_msg4={_tmp92,_tmp92 + 60};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(
void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;
ts=ts->tl){void*_tmp93=(void*)ts->hd;void*_tmp94;struct Cyc_Position_Segment*
_tmp95;struct Cyc_Position_Segment*_tmp96;struct Cyc_Position_Segment*_tmp97;
struct Cyc_Position_Segment*_tmp98;struct Cyc_Position_Segment*_tmp99;struct Cyc_Absyn_Decl*
_tmp9A;_LL41: if(*((int*)_tmp93)!= 4)goto _LL43;_tmp94=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp93)->f1;_tmp95=((struct Cyc_Parse_Type_spec_struct*)_tmp93)->f2;_LL42: if(
seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp95);last_loc=_tmp95;seen_type=1;t=
_tmp94;goto _LL40;_LL43: if(*((int*)_tmp93)!= 0)goto _LL45;_tmp96=((struct Cyc_Parse_Signed_spec_struct*)
_tmp93)->f1;_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp96);if(seen_type)
Cyc_Parse_err(({const char*_tmp9B="signed qualifier must come before type";
_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,39));}),_tmp96);
last_loc=_tmp96;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp93)!= 1)
goto _LL47;_tmp97=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp93)->f1;_LL46: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp97);if(seen_type)Cyc_Parse_err(({
const char*_tmp9C="signed qualifier must come before type";_tag_dynforward(_tmp9C,
sizeof(char),_get_zero_arr_size(_tmp9C,39));}),_tmp97);last_loc=_tmp97;seen_sign=
1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp93)!= 2)goto _LL49;_tmp98=((struct
Cyc_Parse_Short_spec_struct*)_tmp93)->f1;_LL48: if(seen_size)Cyc_Parse_err(({
const char*_tmp9D="integral size may appear only once within a type specifier";
_tag_dynforward(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,59));}),_tmp98);
if(seen_type)Cyc_Parse_err(({const char*_tmp9E="short modifier must come before base type";
_tag_dynforward(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,42));}),_tmp98);
last_loc=_tmp98;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp93)!= 3)
goto _LL4B;_tmp99=((struct Cyc_Parse_Long_spec_struct*)_tmp93)->f1;_LL4A: if(
seen_type)Cyc_Parse_err(({const char*_tmp9F="long modifier must come before base type";
_tag_dynforward(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,41));}),_tmp99);
if(seen_size){void*_tmpA0=sz;_LL4E: if((int)_tmpA0 != 3)goto _LL50;_LL4F: sz=(void*)
4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({const char*_tmpA1="extra long in type specifier";
_tag_dynforward(_tmpA1,sizeof(char),_get_zero_arr_size(_tmpA1,29));}),_tmp99);
goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp99;seen_size=1;goto _LL40;_LL4B:
if(*((int*)_tmp93)!= 5)goto _LL40;_tmp9A=((struct Cyc_Parse_Decl_spec_struct*)
_tmp93)->f1;_LL4C: last_loc=_tmp9A->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmpA2=(void*)_tmp9A->r;struct Cyc_Absyn_Aggrdecl*_tmpA3;struct Cyc_Absyn_Tuniondecl*
_tmpA4;struct Cyc_Absyn_Enumdecl*_tmpA5;_LL53: if(_tmpA2 <= (void*)2)goto _LL59;if(*((
int*)_tmpA2)!= 4)goto _LL55;_tmpA3=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA2)->f1;
_LL54: {struct Cyc_List_List*_tmpA6=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA3->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA8;_tmpA8.tag=10;_tmpA8.f1=({struct Cyc_Absyn_AggrInfo _tmpA9;_tmpA9.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpAA;(_tmpAA.UnknownAggr).tag=
0;(_tmpAA.UnknownAggr).f1=(void*)((void*)_tmpA3->kind);(_tmpAA.UnknownAggr).f2=
_tmpA3->name;_tmpAA;});_tmpA9.targs=_tmpA6;_tmpA9;});_tmpA8;});_tmpA7;});if(
_tmpA3->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*_tmpAB));
_tmpAB->v=_tmp9A;_tmpAB;});goto _LL52;}_LL55: if(*((int*)_tmpA2)!= 5)goto _LL57;
_tmpA4=((struct Cyc_Absyn_Tunion_d_struct*)_tmpA2)->f1;_LL56: {struct Cyc_List_List*
_tmpAC=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA4->tvs));struct Cyc_Core_Opt*ropt=_tmpA4->is_flat?0:({struct Cyc_Core_Opt*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->v=(void*)((void*)2);_tmpB3;});t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Absyn_TunionType_struct _tmpAE;_tmpAE.tag=2;_tmpAE.f1=({struct Cyc_Absyn_TunionInfo
_tmpAF;_tmpAF.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union
_tmpB0;(_tmpB0.KnownTunion).tag=1;(_tmpB0.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=_tmpA4;_tmpB1;});_tmpB0;});_tmpAF.targs=
_tmpAC;_tmpAF.rgn=ropt;_tmpAF;});_tmpAE;});_tmpAD;});if(_tmpA4->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=_tmp9A;
_tmpB2;});goto _LL52;}_LL57: if(*((int*)_tmpA2)!= 6)goto _LL59;_tmpA5=((struct Cyc_Absyn_Enum_d_struct*)
_tmpA2)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_EnumType_struct _tmpB5;_tmpB5.tag=12;
_tmpB5.f1=_tmpA5->name;_tmpB5.f2=0;_tmpB5;});_tmpB4;});declopt=({struct Cyc_Core_Opt*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=_tmp9A;_tmpB6;});goto _LL52;_LL59:;
_LL5A:({void*_tmpB7[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp9A->loc,({const
char*_tmpB8="bad declaration within type specifier";_tag_dynforward(_tmpB8,
sizeof(char),_get_zero_arr_size(_tmpB8,38));}),_tag_dynforward(_tmpB7,sizeof(
void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9A->loc);}goto _LL40;
_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpB9[0]={};Cyc_Tcutil_warn(
last_loc,({const char*_tmpBA="missing type within specifier";_tag_dynforward(
_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,30));}),_tag_dynforward(_tmpB9,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBB=_cycalloc(
sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_IntType_struct _tmpBC;_tmpBC.tag=5;
_tmpBC.f1=(void*)sgn;_tmpBC.f2=(void*)sz;_tmpBC;});_tmpBB;});}else{if(seen_sign){
void*_tmpBD=t;void*_tmpBE;void*_tmpBF;_LL5C: if(_tmpBD <= (void*)4)goto _LL5E;if(*((
int*)_tmpBD)!= 5)goto _LL5E;_tmpBE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBD)->f1;_tmpBF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBD)->f2;_LL5D:
if(_tmpBE != sgn)t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC0=_cycalloc(
sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_IntType_struct _tmpC1;_tmpC1.tag=5;
_tmpC1.f1=(void*)sgn;_tmpC1.f2=(void*)_tmpBF;_tmpC1;});_tmpC0;});goto _LL5B;_LL5E:;
_LL5F: Cyc_Parse_err(({const char*_tmpC2="sign specification on non-integral type";
_tag_dynforward(_tmpC2,sizeof(char),_get_zero_arr_size(_tmpC2,40));}),last_loc);
goto _LL5B;_LL5B:;}if(seen_size){void*_tmpC3=t;void*_tmpC4;void*_tmpC5;_LL61: if(
_tmpC3 <= (void*)4)goto _LL65;if(*((int*)_tmpC3)!= 5)goto _LL63;_tmpC4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpC3)->f1;_tmpC5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC3)->f2;_LL62: if(_tmpC5 != sz)t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_IntType_struct
_tmpC7;_tmpC7.tag=5;_tmpC7.f1=(void*)_tmpC4;_tmpC7.f2=(void*)sz;_tmpC7;});_tmpC6;});
goto _LL60;_LL63: if(*((int*)_tmpC3)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpC8=_cycalloc_atomic(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpC9;_tmpC9.tag=6;_tmpC9.f1=1;_tmpC9;});_tmpC8;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpCA="size qualifier on non-integral type";_tag_dynforward(_tmpCA,
sizeof(char),_get_zero_arr_size(_tmpCA,36));}),last_loc);goto _LL60;_LL60:;}}
return({struct _tuple5 _tmpCB;_tmpCB.f1=t;_tmpCB.f2=declopt;_tmpCB;});}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmpCC=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpCD=_tmpCC->id;struct Cyc_Absyn_Tqual _tmpCF;void*_tmpD0;struct
Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD2;struct _tuple6 _tmpCE=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpCC->tms);_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;_tmpD1=_tmpCE.f3;
_tmpD2=_tmpCE.f4;if(ds->tl == 0)return({struct Cyc_List_List*_tmpD3=_region_malloc(
r,sizeof(*_tmpD3));_tmpD3->hd=({struct _tuple7*_tmpD4=_region_malloc(r,sizeof(*
_tmpD4));_tmpD4->f1=_tmpCD;_tmpD4->f2=_tmpCF;_tmpD4->f3=_tmpD0;_tmpD4->f4=_tmpD1;
_tmpD4->f5=_tmpD2;_tmpD4;});_tmpD3->tl=0;_tmpD3;});else{return({struct Cyc_List_List*
_tmpD5=_region_malloc(r,sizeof(*_tmpD5));_tmpD5->hd=({struct _tuple7*_tmpD6=
_region_malloc(r,sizeof(*_tmpD6));_tmpD6->f1=_tmpCD;_tmpD6->f2=_tmpCF;_tmpD6->f3=
_tmpD0;_tmpD6->f4=_tmpD1;_tmpD6->f5=_tmpD2;_tmpD6;});_tmpD5->tl=Cyc_Parse_apply_tmss(
r,_tmpCF,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD5;});}}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD7;_tmpD7.f1=
tq;_tmpD7.f2=t;_tmpD7.f3=0;_tmpD7.f4=atts;_tmpD7;});{void*_tmpD8=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpD9;struct Cyc_Position_Segment*_tmpDA;struct Cyc_Absyn_Exp*
_tmpDB;struct Cyc_Absyn_Conref*_tmpDC;struct Cyc_Position_Segment*_tmpDD;void*
_tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_Position_Segment*_tmpE0;struct Cyc_Absyn_PtrAtts
_tmpE1;struct Cyc_Absyn_Tqual _tmpE2;struct Cyc_Position_Segment*_tmpE3;struct Cyc_List_List*
_tmpE4;_LL68: if(*((int*)_tmpD8)!= 0)goto _LL6A;_tmpD9=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Carray_mod_struct*)_tmpD8)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpD9,
_tmpDA),atts,tms->tl);_LL6A: if(*((int*)_tmpD8)!= 1)goto _LL6C;_tmpDB=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD8)->f1;_tmpDC=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD8)->f2;_tmpDD=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpD8)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpDB,
_tmpDC,_tmpDD),atts,tms->tl);_LL6C: if(*((int*)_tmpD8)!= 3)goto _LL6E;_tmpDE=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpD8)->f1;_LL6D: {void*_tmpE5=_tmpDE;
struct Cyc_List_List*_tmpE6;int _tmpE7;struct Cyc_Absyn_VarargInfo*_tmpE8;struct Cyc_Core_Opt*
_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_Position_Segment*_tmpEB;_LL75: if(*((
int*)_tmpE5)!= 1)goto _LL77;_tmpE6=((struct Cyc_Absyn_WithTypes_struct*)_tmpE5)->f1;
_tmpE7=((struct Cyc_Absyn_WithTypes_struct*)_tmpE5)->f2;_tmpE8=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE5)->f3;_tmpE9=((struct Cyc_Absyn_WithTypes_struct*)_tmpE5)->f4;_tmpEA=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE5)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->hd=(void*)((
void*)as->hd);_tmpEC->tl=fn_atts;_tmpEC;});else{new_atts=({struct Cyc_List_List*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->hd=(void*)((void*)as->hd);_tmpED->tl=
new_atts;_tmpED;});}}}if(tms->tl != 0){void*_tmpEE=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpEF;_LL7A: if(*((int*)_tmpEE)!= 
4)goto _LL7C;_tmpEF=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpEE)->f1;_LL7B:
typvars=_tmpEF;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE7  && _tmpE8 == 0) && _tmpE6 != 0) && _tmpE6->tl == 0) && (*((struct _tuple2*)
_tmpE6->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE6->hd)).f3 == (void*)0)_tmpE6=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE6);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpE9,t,_tmpE6,_tmpE7,_tmpE8,_tmpEA,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE5)!= 0)goto _LL74;_tmpEB=((struct Cyc_Absyn_NoTypes_struct*)_tmpE5)->f2;_LL78:({
void*_tmpF0[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpEB,({const char*_tmpF1="function declaration without parameter types";
_tag_dynforward(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,45));}),
_tag_dynforward(_tmpF0,sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpD8)!= 4)
goto _LL70;_tmpDF=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD8)->f1;_tmpE0=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpD8)->f2;_LL6F: if(tms->tl == 0)return({
struct _tuple6 _tmpF2;_tmpF2.f1=tq;_tmpF2.f2=t;_tmpF2.f3=_tmpDF;_tmpF2.f4=atts;
_tmpF2;});({void*_tmpF3[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpE0,({const char*
_tmpF4="type parameters must appear before function arguments in declarator";
_tag_dynforward(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,68));}),
_tag_dynforward(_tmpF3,sizeof(void*),0));});_LL70: if(*((int*)_tmpD8)!= 2)goto
_LL72;_tmpE1=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpD8)->f1;_tmpE2=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpD8)->f2;_LL71: return Cyc_Parse_apply_tms(_tmpE2,(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));
_tmpF5[0]=({struct Cyc_Absyn_PointerType_struct _tmpF6;_tmpF6.tag=4;_tmpF6.f1=({
struct Cyc_Absyn_PtrInfo _tmpF7;_tmpF7.elt_typ=(void*)t;_tmpF7.elt_tq=tq;_tmpF7.ptr_atts=
_tmpE1;_tmpF7;});_tmpF6;});_tmpF5;}),atts,tms->tl);_LL72: if(*((int*)_tmpD8)!= 5)
goto _LL67;_tmpE3=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpD8)->f1;_tmpE4=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpD8)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpE4),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpF9;struct Cyc_Core_Opt*
_tmpFA;struct _tuple5 _tmpF8=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpF9=
_tmpF8.f1;_tmpFA=_tmpF8.f2;if(_tmpFA != 0)({void*_tmpFB[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmpFC="ignoring nested type declaration(s) in specifier list";
_tag_dynforward(_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,54));}),
_tag_dynforward(_tmpFB,sizeof(void*),0));});return _tmpF9;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmpFE;struct _tuple1*_tmpFF;struct Cyc_Absyn_Tqual _tmp100;void*
_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_List_List*_tmp103;struct _tuple7*
_tmpFD=t;_tmpFE=*_tmpFD;_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;_tmp101=_tmpFE.f3;
_tmp102=_tmpFE.f4;_tmp103=_tmpFE.f5;Cyc_Lex_register_typedef(_tmpFF);{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmp104=_tmp101;struct Cyc_Core_Opt*_tmp105;
_LL7F: if(_tmp104 <= (void*)4)goto _LL81;if(*((int*)_tmp104)!= 0)goto _LL81;_tmp105=((
struct Cyc_Absyn_Evar_struct*)_tmp104)->f1;_LL80: type=0;if(_tmp105 == 0)kind=({
struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(void*)((void*)
2);_tmp106;});else{kind=_tmp105;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->v=(void*)_tmp101;_tmp107;});goto
_LL7E;_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmp109;_tmp109.tag=7;_tmp109.f1=({struct Cyc_Absyn_Typedefdecl*_tmp10A=_cycalloc(
sizeof(*_tmp10A));_tmp10A->name=_tmpFF;_tmp10A->tvs=_tmp102;_tmp10A->kind=kind;
_tmp10A->defn=type;_tmp10A->atts=_tmp103;_tmp10A->tq=_tmp100;_tmp10A;});_tmp109;});
_tmp108;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10C;_tmp10C.tag=11;_tmp10C.f1=d;_tmp10C.f2=s;_tmp10C;});_tmp10B;}),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp10D=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp10D;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp10F;struct Cyc_Absyn_Tqual
_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp112;struct Cyc_Parse_Declaration_spec*
_tmp10E=ds;_tmp10F=*_tmp10E;_tmp110=_tmp10F.tq;_tmp111=_tmp10F.type_specs;
_tmp112=_tmp10F.attributes;if(_tmp110.loc == 0)_tmp110.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp113="inline is allowed only on function definitions";
_tag_dynforward(_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,47));}),loc);if(
_tmp111 == 0){Cyc_Parse_err(({const char*_tmp114="missing type specifiers in declaration";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,39));}),loc);{
struct Cyc_List_List*_tmp115=0;_npop_handler(0);return _tmp115;}}{void*s=(void*)2;
int istypedef=0;if(ds->sc != 0){void*_tmp116=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp116 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp116 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp116 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp116 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp116 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp116 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp116 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp118;struct Cyc_List_List*
_tmp119;struct _tuple0 _tmp117=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp118=_tmp117.f1;_tmp119=_tmp117.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp119;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple5 _tmp11A=Cyc_Parse_collapse_type_specifiers(_tmp111,loc);
if(_tmp118 == 0){void*_tmp11C;struct Cyc_Core_Opt*_tmp11D;struct _tuple5 _tmp11B=
_tmp11A;_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;if(_tmp11D != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp11D->v;{void*_tmp11E=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmp11F;struct Cyc_Absyn_Aggrdecl*_tmp120;struct Cyc_Absyn_Tuniondecl*_tmp121;
_LL93: if(_tmp11E <= (void*)2)goto _LL99;if(*((int*)_tmp11E)!= 6)goto _LL95;_tmp11F=((
struct Cyc_Absyn_Enum_d_struct*)_tmp11E)->f1;_LL94:(void*)(_tmp11F->sc=(void*)s);
if(_tmp112 != 0)Cyc_Parse_err(({const char*_tmp122="bad attributes on enum";
_tag_dynforward(_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,23));}),loc);
goto _LL92;_LL95: if(*((int*)_tmp11E)!= 4)goto _LL97;_tmp120=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp11E)->f1;_LL96:(void*)(_tmp120->sc=(void*)s);_tmp120->attributes=_tmp112;
goto _LL92;_LL97: if(*((int*)_tmp11E)!= 5)goto _LL99;_tmp121=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp11E)->f1;_LL98:(void*)(_tmp121->sc=(void*)s);if(_tmp112 != 0)Cyc_Parse_err(({
const char*_tmp123="bad attributes on tunion";_tag_dynforward(_tmp123,sizeof(char),
_get_zero_arr_size(_tmp123,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp124="bad declaration";_tag_dynforward(_tmp124,sizeof(char),
_get_zero_arr_size(_tmp124,16));}),loc);{struct Cyc_List_List*_tmp125=0;
_npop_handler(0);return _tmp125;}_LL92:;}{struct Cyc_List_List*_tmp127=({struct Cyc_List_List*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=d;_tmp126->tl=0;_tmp126;});
_npop_handler(0);return _tmp127;}}else{void*_tmp128=_tmp11C;struct Cyc_Absyn_AggrInfo
_tmp129;union Cyc_Absyn_AggrInfoU_union _tmp12A;void*_tmp12B;struct _tuple1*_tmp12C;
struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_TunionInfo _tmp12E;union Cyc_Absyn_TunionInfoU_union
_tmp12F;struct Cyc_Absyn_Tuniondecl**_tmp130;struct Cyc_Absyn_TunionInfo _tmp131;
union Cyc_Absyn_TunionInfoU_union _tmp132;struct Cyc_Absyn_UnknownTunionInfo _tmp133;
struct _tuple1*_tmp134;int _tmp135;int _tmp136;struct Cyc_List_List*_tmp137;struct
_tuple1*_tmp138;struct Cyc_List_List*_tmp139;_LL9C: if(_tmp128 <= (void*)4)goto
_LLA6;if(*((int*)_tmp128)!= 10)goto _LL9E;_tmp129=((struct Cyc_Absyn_AggrType_struct*)
_tmp128)->f1;_tmp12A=_tmp129.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp128)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL9E;_tmp12B=(_tmp12A.UnknownAggr).f1;
_tmp12C=(_tmp12A.UnknownAggr).f2;_tmp12D=_tmp129.targs;_LL9D: {struct Cyc_List_List*
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12D);struct Cyc_Absyn_Aggrdecl*_tmp13B=({struct Cyc_Absyn_Aggrdecl*_tmp141=
_cycalloc(sizeof(*_tmp141));_tmp141->kind=(void*)_tmp12B;_tmp141->sc=(void*)s;
_tmp141->name=_tmp12C;_tmp141->tvs=_tmp13A;_tmp141->impl=0;_tmp141->attributes=0;
_tmp141;});if(_tmp112 != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on type declaration";
_tag_dynforward(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,35));}),loc);{
struct Cyc_List_List*_tmp140=({struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*
_tmp13D));_tmp13D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp13F;_tmp13F.tag=4;_tmp13F.f1=_tmp13B;_tmp13F;});_tmp13E;}),loc);_tmp13D->tl=
0;_tmp13D;});_npop_handler(0);return _tmp140;}}_LL9E: if(*((int*)_tmp128)!= 2)goto
_LLA0;_tmp12E=((struct Cyc_Absyn_TunionType_struct*)_tmp128)->f1;_tmp12F=_tmp12E.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp128)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LLA0;_tmp130=(_tmp12F.KnownTunion).f1;_LL9F: if(_tmp112 != 0)Cyc_Parse_err(({
const char*_tmp142="bad attributes on tunion";_tag_dynforward(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,25));}),loc);{struct Cyc_List_List*_tmp146=({struct Cyc_List_List*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Tunion_d_struct*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144[0]=({
struct Cyc_Absyn_Tunion_d_struct _tmp145;_tmp145.tag=5;_tmp145.f1=*_tmp130;_tmp145;});
_tmp144;}),loc);_tmp143->tl=0;_tmp143;});_npop_handler(0);return _tmp146;}_LLA0:
if(*((int*)_tmp128)!= 2)goto _LLA2;_tmp131=((struct Cyc_Absyn_TunionType_struct*)
_tmp128)->f1;_tmp132=_tmp131.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp128)->f1).tunion_info).UnknownTunion).tag != 0)goto _LLA2;_tmp133=(_tmp132.UnknownTunion).f1;
_tmp134=_tmp133.name;_tmp135=_tmp133.is_xtunion;_tmp136=_tmp133.is_flat;_tmp137=
_tmp131.targs;_LLA1: {struct Cyc_List_List*_tmp147=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp137);
struct Cyc_Absyn_Decl*_tmp148=Cyc_Absyn_tunion_decl(s,_tmp134,_tmp147,0,_tmp135,
_tmp136,loc);if(_tmp112 != 0)Cyc_Parse_err(({const char*_tmp149="bad attributes on tunion";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,25));}),loc);{
struct Cyc_List_List*_tmp14B=({struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*
_tmp14A));_tmp14A->hd=_tmp148;_tmp14A->tl=0;_tmp14A;});_npop_handler(0);return
_tmp14B;}}_LLA2: if(*((int*)_tmp128)!= 12)goto _LLA4;_tmp138=((struct Cyc_Absyn_EnumType_struct*)
_tmp128)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp14C=({struct Cyc_Absyn_Enumdecl*
_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->sc=(void*)s;_tmp153->name=_tmp138;
_tmp153->fields=0;_tmp153;});if(_tmp112 != 0)Cyc_Parse_err(({const char*_tmp14D="bad attributes on enum";
_tag_dynforward(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,23));}),loc);{
struct Cyc_List_List*_tmp152=({struct Cyc_List_List*_tmp14E=_cycalloc(sizeof(*
_tmp14E));_tmp14E->hd=({struct Cyc_Absyn_Decl*_tmp14F=_cycalloc(sizeof(*_tmp14F));
_tmp14F->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp150=_cycalloc(
sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_Enum_d_struct _tmp151;_tmp151.tag=
6;_tmp151.f1=_tmp14C;_tmp151;});_tmp150;}));_tmp14F->loc=loc;_tmp14F;});_tmp14E->tl=
0;_tmp14E;});_npop_handler(0);return _tmp152;}}_LLA4: if(*((int*)_tmp128)!= 13)
goto _LLA6;_tmp139=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp128)->f1;_LLA5: {
struct Cyc_Absyn_Enumdecl*_tmp154=({struct Cyc_Absyn_Enumdecl*_tmp15B=_cycalloc(
sizeof(*_tmp15B));_tmp15B->sc=(void*)s;_tmp15B->name=Cyc_Parse_gensym_enum();
_tmp15B->fields=({struct Cyc_Core_Opt*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->v=
_tmp139;_tmp15C;});_tmp15B;});if(_tmp112 != 0)Cyc_Parse_err(({const char*_tmp155="bad attributes on enum";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,23));}),loc);{
struct Cyc_List_List*_tmp15A=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*
_tmp156));_tmp156->hd=({struct Cyc_Absyn_Decl*_tmp157=_cycalloc(sizeof(*_tmp157));
_tmp157->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp158=_cycalloc(
sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Absyn_Enum_d_struct _tmp159;_tmp159.tag=
6;_tmp159.f1=_tmp154;_tmp159;});_tmp158;}));_tmp157->loc=loc;_tmp157;});_tmp156->tl=
0;_tmp156;});_npop_handler(0);return _tmp15A;}}_LLA6:;_LLA7: Cyc_Parse_err(({const
char*_tmp15D="missing declarator";_tag_dynforward(_tmp15D,sizeof(char),
_get_zero_arr_size(_tmp15D,19));}),loc);{struct Cyc_List_List*_tmp15E=0;
_npop_handler(0);return _tmp15E;}_LL9B:;}}else{void*t=_tmp11A.f1;struct Cyc_List_List*
_tmp15F=Cyc_Parse_apply_tmss(mkrgn,_tmp110,t,_tmp118,_tmp112);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp160="initializer in typedef declaration";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,35));}),loc);{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(
struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp15F);
if(_tmp11A.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmp11A.f2)->v;{
void*_tmp161=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp162;struct Cyc_Absyn_Tuniondecl*
_tmp163;struct Cyc_Absyn_Enumdecl*_tmp164;_LLA9: if(_tmp161 <= (void*)2)goto _LLAF;
if(*((int*)_tmp161)!= 4)goto _LLAB;_tmp162=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp161)->f1;_LLAA:(void*)(_tmp162->sc=(void*)s);_tmp162->attributes=_tmp112;
_tmp112=0;goto _LLA8;_LLAB: if(*((int*)_tmp161)!= 5)goto _LLAD;_tmp163=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp161)->f1;_LLAC:(void*)(_tmp163->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp161)!= 6)goto _LLAF;_tmp164=((struct Cyc_Absyn_Enum_d_struct*)_tmp161)->f1;
_LLAE:(void*)(_tmp164->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp165="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,70));}),loc);
goto _LLA8;_LLA8:;}decls=({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(*_tmp166));
_tmp166->hd=d;_tmp166->tl=decls;_tmp166;});}{struct Cyc_List_List*_tmp167=decls;
_npop_handler(0);return _tmp167;}}}else{if(_tmp11A.f2 != 0)Cyc_Parse_unimp(({const
char*_tmp168="nested type declaration within declarator";_tag_dynforward(_tmp168,
sizeof(char),_get_zero_arr_size(_tmp168,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp169=_tmp15F;for(0;_tmp169 != 0;(_tmp169=_tmp169->tl,
_tmp119=_tmp119->tl)){struct _tuple7 _tmp16B;struct _tuple1*_tmp16C;struct Cyc_Absyn_Tqual
_tmp16D;void*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_List_List*_tmp170;
struct _tuple7*_tmp16A=(struct _tuple7*)_tmp169->hd;_tmp16B=*_tmp16A;_tmp16C=
_tmp16B.f1;_tmp16D=_tmp16B.f2;_tmp16E=_tmp16B.f3;_tmp16F=_tmp16B.f4;_tmp170=
_tmp16B.f5;if(_tmp16F != 0)({void*_tmp171[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp172="bad type params, ignoring";_tag_dynforward(_tmp172,sizeof(char),
_get_zero_arr_size(_tmp172,26));}),_tag_dynforward(_tmp171,sizeof(void*),0));});
if(_tmp119 == 0)({void*_tmp173[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp174="unexpected NULL in parse!";_tag_dynforward(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,26));}),_tag_dynforward(_tmp173,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp175=(struct Cyc_Absyn_Exp*)_tmp119->hd;struct Cyc_Absyn_Vardecl*
_tmp176=Cyc_Absyn_new_vardecl(_tmp16C,_tmp16E,_tmp175);_tmp176->tq=_tmp16D;(void*)(
_tmp176->sc=(void*)s);_tmp176->attributes=_tmp170;{struct Cyc_Absyn_Decl*_tmp177=({
struct Cyc_Absyn_Decl*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[
0]=({struct Cyc_Absyn_Var_d_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=_tmp176;
_tmp17B;});_tmp17A;}));_tmp179->loc=loc;_tmp179;});decls=({struct Cyc_List_List*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->hd=_tmp177;_tmp178->tl=decls;
_tmp178;});}}}}{struct Cyc_List_List*_tmp17C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp17C;}}}}}}}}};_pop_region(
mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dynforward_ptr)s)== 1)switch(*((const char*)
_check_dynforward_subscript(s,sizeof(char),0))){case 'A': _LLB1: return(void*)0;
case 'M': _LLB2: return(void*)1;case 'B': _LLB3: return(void*)2;case 'R': _LLB4: return(
void*)3;case 'E': _LLB5: return(void*)4;case 'I': _LLB6: return(void*)5;default: _LLB7:
break;}Cyc_Parse_err((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp17F;
_tmp17F.tag=0;_tmp17F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*
_tmp17D[1]={& _tmp17F};Cyc_aprintf(({const char*_tmp17E="bad kind: %s";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,13));}),
_tag_dynforward(_tmp17D,sizeof(void*),1));}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp180=(void*)p->r;
struct _tuple1*_tmp181;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Pat*
_tmp183;struct Cyc_Absyn_Pat _tmp184;void*_tmp185;struct Cyc_Absyn_Pat*_tmp186;void*
_tmp187;int _tmp188;char _tmp189;struct _dynforward_ptr _tmp18A;struct _tuple1*
_tmp18B;struct Cyc_List_List*_tmp18C;int _tmp18D;struct Cyc_Absyn_Exp*_tmp18E;_LLBA:
if(_tmp180 <= (void*)2)goto _LLC0;if(*((int*)_tmp180)!= 12)goto _LLBC;_tmp181=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp180)->f1;_LLBB: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp18F=_cycalloc(sizeof(*_tmp18F));
_tmp18F[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp190;_tmp190.tag=2;_tmp190.f1=
_tmp181;_tmp190;});_tmp18F;}),p->loc);_LLBC: if(*((int*)_tmp180)!= 1)goto _LLBE;
_tmp182=((struct Cyc_Absyn_Reference_p_struct*)_tmp180)->f1;_tmp183=((struct Cyc_Absyn_Reference_p_struct*)
_tmp180)->f2;_tmp184=*_tmp183;_tmp185=(void*)_tmp184.r;if((int)_tmp185 != 0)goto
_LLBE;_LLBD: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp192;_tmp192.tag=2;_tmp192.f1=_tmp182->name;_tmp192;});_tmp191;}),p->loc),p->loc);
_LLBE: if(*((int*)_tmp180)!= 4)goto _LLC0;_tmp186=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp180)->f1;_LLBF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp186),p->loc);
_LLC0: if((int)_tmp180 != 1)goto _LLC2;_LLC1: return Cyc_Absyn_null_exp(p->loc);_LLC2:
if(_tmp180 <= (void*)2)goto _LLCC;if(*((int*)_tmp180)!= 7)goto _LLC4;_tmp187=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp180)->f1;_tmp188=((struct Cyc_Absyn_Int_p_struct*)
_tmp180)->f2;_LLC3: return Cyc_Absyn_int_exp(_tmp187,_tmp188,p->loc);_LLC4: if(*((
int*)_tmp180)!= 8)goto _LLC6;_tmp189=((struct Cyc_Absyn_Char_p_struct*)_tmp180)->f1;
_LLC5: return Cyc_Absyn_char_exp(_tmp189,p->loc);_LLC6: if(*((int*)_tmp180)!= 9)
goto _LLC8;_tmp18A=((struct Cyc_Absyn_Float_p_struct*)_tmp180)->f1;_LLC7: return Cyc_Absyn_float_exp(
_tmp18A,p->loc);_LLC8: if(*((int*)_tmp180)!= 13)goto _LLCA;_tmp18B=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp180)->f1;_tmp18C=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp180)->f2;
_tmp18D=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp180)->f3;if(_tmp18D != 0)goto
_LLCA;_LLC9: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp194;_tmp194.tag=2;_tmp194.f1=_tmp18B;_tmp194;});_tmp193;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp18C);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCA: if(*((int*)_tmp180)!= 14)goto _LLCC;_tmp18E=((struct Cyc_Absyn_Exp_p_struct*)
_tmp180)->f1;_LLCB: return _tmp18E;_LLCC:;_LLCD: Cyc_Parse_err(({const char*_tmp195="cannot mix patterns and expressions in case";
_tag_dynforward(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,44));}),p->loc);
return Cyc_Absyn_null_exp(p->loc);_LLB9:;}struct _tuple11{void*f1;int f2;};struct
Cyc_Int_tok_struct{int tag;struct _tuple11*f1;};struct Cyc_Char_tok_struct{int tag;
char f1;};struct Cyc_String_tok_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*
f1;};struct _tuple12{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;
struct Cyc_Absyn_Conref*f3;};struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};
struct Cyc_YY2_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_YY5_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_YY10_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};
struct Cyc_YY12_struct{int tag;struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_YY14_struct{int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};
struct Cyc_YY17_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*
f1;};struct _tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_YY20_struct{int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{
int tag;void*f1;};struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{
int tag;struct Cyc_Absyn_Tqual f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY27_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*
f1;};struct Cyc_YY30_struct{int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct
Cyc_YY31_struct{int tag;int f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{
int tag;struct Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct
YYINITIALSVAL;};static char _tmp197[15]="$(sign_t,int)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp197,_tmp197 + 15}};struct _tuple11*Cyc_yyget_Int_tok(
union Cyc_YYSTYPE_union yy1){struct _tuple11*yyzzz;{union Cyc_YYSTYPE_union _tmp198=
yy1;struct _tuple11*_tmp199;_LLCF: if((_tmp198.Int_tok).tag != 0)goto _LLD1;_tmp199=(
_tmp198.Int_tok).f1;_LLD0: yyzzz=_tmp199;goto _LLCE;_LLD1:;_LLD2:(int)_throw((void*)&
Cyc_yyfail_Int_tok);_LLCE:;}return yyzzz;}static char _tmp19B[5]="char";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp19B,
_tmp19B + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){char yyzzz;{union
Cyc_YYSTYPE_union _tmp19C=yy1;char _tmp19D;_LLD4: if((_tmp19C.Char_tok).tag != 1)
goto _LLD6;_tmp19D=(_tmp19C.Char_tok).f1;_LLD5: yyzzz=_tmp19D;goto _LLD3;_LLD6:;
_LLD7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD3:;}return yyzzz;}static char
_tmp19F[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp19F,_tmp19F + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1){struct _dynforward_ptr yyzzz;{union Cyc_YYSTYPE_union
_tmp1A0=yy1;struct _dynforward_ptr _tmp1A1;_LLD9: if((_tmp1A0.String_tok).tag != 2)
goto _LLDB;_tmp1A1=(_tmp1A0.String_tok).f1;_LLDA: yyzzz=_tmp1A1;goto _LLD8;_LLDB:;
_LLDC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLD8:;}return yyzzz;}static char
_tmp1A3[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1A3,_tmp1A3 + 54}};
struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{
union Cyc_YYSTYPE_union _tmp1A4=yy1;struct _tuple12*_tmp1A5;_LLDE: if((_tmp1A4.YY1).tag
!= 5)goto _LLE0;_tmp1A5=(_tmp1A4.YY1).f1;_LLDF: yyzzz=_tmp1A5;goto _LLDD;_LLE0:;
_LLE1:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDD:;}return yyzzz;}static char _tmp1A7[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp1A7,_tmp1A7 + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union _tmp1A8=yy1;struct Cyc_Absyn_Conref*
_tmp1A9;_LLE3: if((_tmp1A8.YY2).tag != 6)goto _LLE5;_tmp1A9=(_tmp1A8.YY2).f1;_LLE4:
yyzzz=_tmp1A9;goto _LLE2;_LLE5:;_LLE6:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE2:;}
return yyzzz;}static char _tmp1AB[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp1AB,_tmp1AB + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union
Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1AC=
yy1;struct Cyc_Absyn_Exp*_tmp1AD;_LLE8: if((_tmp1AC.YY3).tag != 7)goto _LLEA;_tmp1AD=(
_tmp1AC.YY3).f1;_LLE9: yyzzz=_tmp1AD;goto _LLE7;_LLEA:;_LLEB:(int)_throw((void*)&
Cyc_yyfail_YY3);_LLE7:;}return yyzzz;}static char _tmp1AF[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1AF,_tmp1AF + 
10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE_union _tmp1B0=yy1;struct Cyc_Absyn_Exp*_tmp1B1;_LLED: if((
_tmp1B0.YY4).tag != 8)goto _LLEF;_tmp1B1=(_tmp1B0.YY4).f1;_LLEE: yyzzz=_tmp1B1;goto
_LLEC;_LLEF:;_LLF0:(int)_throw((void*)& Cyc_yyfail_YY4);_LLEC:;}return yyzzz;}
static char _tmp1B3[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp1B3,_tmp1B3 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1B4=
yy1;struct Cyc_List_List*_tmp1B5;_LLF2: if((_tmp1B4.YY5).tag != 9)goto _LLF4;_tmp1B5=(
_tmp1B4.YY5).f1;_LLF3: yyzzz=_tmp1B5;goto _LLF1;_LLF4:;_LLF5:(int)_throw((void*)&
Cyc_yyfail_YY5);_LLF1:;}return yyzzz;}static char _tmp1B7[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1B7,
_tmp1B7 + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1B8=yy1;struct Cyc_List_List*
_tmp1B9;_LLF7: if((_tmp1B8.YY6).tag != 10)goto _LLF9;_tmp1B9=(_tmp1B8.YY6).f1;_LLF8:
yyzzz=_tmp1B9;goto _LLF6;_LLF9:;_LLFA:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF6:;}
return yyzzz;}static char _tmp1BB[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1BB,_tmp1BB + 9}};void*Cyc_yyget_YY7(union
Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1BC=yy1;void*_tmp1BD;
_LLFC: if((_tmp1BC.YY7).tag != 11)goto _LLFE;_tmp1BD=(_tmp1BC.YY7).f1;_LLFD: yyzzz=
_tmp1BD;goto _LLFB;_LLFE:;_LLFF:(int)_throw((void*)& Cyc_yyfail_YY7);_LLFB:;}
return yyzzz;}static char _tmp1BF[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1BF,_tmp1BF + 16}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C0=yy1;struct Cyc_Core_Opt*_tmp1C1;_LL101: if((_tmp1C0.YY8).tag != 12)goto
_LL103;_tmp1C1=(_tmp1C0.YY8).f1;_LL102: yyzzz=_tmp1C1;goto _LL100;_LL103:;_LL104:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL100:;}return yyzzz;}static char _tmp1C3[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1C3,_tmp1C3 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1C4=yy1;struct _tuple1*
_tmp1C5;_LL106: if((_tmp1C4.QualId_tok).tag != 4)goto _LL108;_tmp1C5=(_tmp1C4.QualId_tok).f1;
_LL107: yyzzz=_tmp1C5;goto _LL105;_LL108:;_LL109:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL105:;}return yyzzz;}static char _tmp1C7[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1C7,_tmp1C7 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C8=yy1;struct Cyc_Absyn_Stmt*_tmp1C9;_LL10B: if((_tmp1C8.YY9).tag != 13)goto
_LL10D;_tmp1C9=(_tmp1C8.YY9).f1;_LL10C: yyzzz=_tmp1C9;goto _LL10A;_LL10D:;_LL10E:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10A:;}return yyzzz;}static char _tmp1CB[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CB,
_tmp1CB + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1CC=yy1;struct Cyc_List_List*
_tmp1CD;_LL110: if((_tmp1CC.YY10).tag != 14)goto _LL112;_tmp1CD=(_tmp1CC.YY10).f1;
_LL111: yyzzz=_tmp1CD;goto _LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL10F:;}return yyzzz;}static char _tmp1CF[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1CF,_tmp1CF + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1D0=yy1;struct Cyc_Absyn_Pat*_tmp1D1;_LL115: if((_tmp1D0.YY11).tag != 15)goto
_LL117;_tmp1D1=(_tmp1D0.YY11).f1;_LL116: yyzzz=_tmp1D1;goto _LL114;_LL117:;_LL118:(
int)_throw((void*)& Cyc_yyfail_YY11);_LL114:;}return yyzzz;}static char _tmp1D3[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D3,
_tmp1D3 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D4=yy1;struct _tuple13*_tmp1D5;_LL11A:
if((_tmp1D4.YY12).tag != 16)goto _LL11C;_tmp1D5=(_tmp1D4.YY12).f1;_LL11B: yyzzz=
_tmp1D5;goto _LL119;_LL11C:;_LL11D:(int)_throw((void*)& Cyc_yyfail_YY12);_LL119:;}
return yyzzz;}static char _tmp1D7[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1D7,_tmp1D7 + 14}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1D8=yy1;struct Cyc_List_List*_tmp1D9;_LL11F: if((_tmp1D8.YY13).tag != 17)goto
_LL121;_tmp1D9=(_tmp1D8.YY13).f1;_LL120: yyzzz=_tmp1D9;goto _LL11E;_LL121:;_LL122:(
int)_throw((void*)& Cyc_yyfail_YY13);_LL11E:;}return yyzzz;}static char _tmp1DB[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1DB,
_tmp1DB + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){struct
_tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1DC=yy1;struct _tuple14*_tmp1DD;_LL124:
if((_tmp1DC.YY14).tag != 18)goto _LL126;_tmp1DD=(_tmp1DC.YY14).f1;_LL125: yyzzz=
_tmp1DD;goto _LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_YY14);_LL123:;}
return yyzzz;}static char _tmp1DF[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1DF,
_tmp1DF + 39}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1E0=yy1;struct Cyc_List_List*
_tmp1E1;_LL129: if((_tmp1E0.YY15).tag != 19)goto _LL12B;_tmp1E1=(_tmp1E0.YY15).f1;
_LL12A: yyzzz=_tmp1E1;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL128:;}return yyzzz;}static char _tmp1E3[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1E3,
_tmp1E3 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1E4=yy1;struct _tuple13*_tmp1E5;_LL12E:
if((_tmp1E4.YY16).tag != 20)goto _LL130;_tmp1E5=(_tmp1E4.YY16).f1;_LL12F: yyzzz=
_tmp1E5;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12D:;}
return yyzzz;}static char _tmp1E7[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1E7,_tmp1E7 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1E8=yy1;struct Cyc_Absyn_Fndecl*_tmp1E9;_LL133: if((_tmp1E8.YY17).tag != 21)
goto _LL135;_tmp1E9=(_tmp1E8.YY17).f1;_LL134: yyzzz=_tmp1E9;goto _LL132;_LL135:;
_LL136:(int)_throw((void*)& Cyc_yyfail_YY17);_LL132:;}return yyzzz;}static char
_tmp1EB[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1EB,_tmp1EB + 15}};struct Cyc_List_List*Cyc_yyget_YY18(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1EC=
yy1;struct Cyc_List_List*_tmp1ED;_LL138: if((_tmp1EC.YY18).tag != 22)goto _LL13A;
_tmp1ED=(_tmp1EC.YY18).f1;_LL139: yyzzz=_tmp1ED;goto _LL137;_LL13A:;_LL13B:(int)
_throw((void*)& Cyc_yyfail_YY18);_LL137:;}return yyzzz;}static char _tmp1EF[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1EF,
_tmp1EF + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1F0=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1F1;_LL13D: if((_tmp1F0.YY19).tag != 23)goto
_LL13F;_tmp1F1=(_tmp1F0.YY19).f1;_LL13E: yyzzz=_tmp1F1;goto _LL13C;_LL13F:;_LL140:(
int)_throw((void*)& Cyc_yyfail_YY19);_LL13C:;}return yyzzz;}static char _tmp1F3[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1F3,
_tmp1F3 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){struct
_tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1F4=yy1;struct _tuple15*_tmp1F5;_LL142:
if((_tmp1F4.YY20).tag != 24)goto _LL144;_tmp1F5=(_tmp1F4.YY20).f1;_LL143: yyzzz=
_tmp1F5;goto _LL141;_LL144:;_LL145:(int)_throw((void*)& Cyc_yyfail_YY20);_LL141:;}
return yyzzz;}static char _tmp1F7[35]="list_t<$(declarator_t,exp_opt_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1F7,_tmp1F7 + 
35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1F8=yy1;struct Cyc_List_List*_tmp1F9;_LL147: if((
_tmp1F8.YY21).tag != 25)goto _LL149;_tmp1F9=(_tmp1F8.YY21).f1;_LL148: yyzzz=_tmp1F9;
goto _LL146;_LL149:;_LL14A:(int)_throw((void*)& Cyc_yyfail_YY21);_LL146:;}return
yyzzz;}static char _tmp1FB[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1FB,_tmp1FB + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1FC=yy1;void*
_tmp1FD;_LL14C: if((_tmp1FC.YY22).tag != 26)goto _LL14E;_tmp1FD=(_tmp1FC.YY22).f1;
_LL14D: yyzzz=_tmp1FD;goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14B:;}return yyzzz;}static char _tmp1FF[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1FF,_tmp1FF + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp200=yy1;void*
_tmp201;_LL151: if((_tmp200.YY23).tag != 27)goto _LL153;_tmp201=(_tmp200.YY23).f1;
_LL152: yyzzz=_tmp201;goto _LL150;_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL150:;}return yyzzz;}static char _tmp203[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp203,_tmp203 + 12}};void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp204=yy1;void*
_tmp205;_LL156: if((_tmp204.YY24).tag != 28)goto _LL158;_tmp205=(_tmp204.YY24).f1;
_LL157: yyzzz=_tmp205;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL155:;}return yyzzz;}static char _tmp207[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp207,_tmp207 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp208=yy1;struct Cyc_Absyn_Tqual _tmp209;_LL15B: if((_tmp208.YY25).tag != 29)goto
_LL15D;_tmp209=(_tmp208.YY25).f1;_LL15C: yyzzz=_tmp209;goto _LL15A;_LL15D:;_LL15E:(
int)_throw((void*)& Cyc_yyfail_YY25);_LL15A:;}return yyzzz;}static char _tmp20B[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp20B,
_tmp20B + 20}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp20C=yy1;struct Cyc_List_List*
_tmp20D;_LL160: if((_tmp20C.YY26).tag != 30)goto _LL162;_tmp20D=(_tmp20C.YY26).f1;
_LL161: yyzzz=_tmp20D;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL15F:;}return yyzzz;}static char _tmp20F[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp20F,_tmp20F + 
28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp210=yy1;struct Cyc_List_List*_tmp211;_LL165: if((
_tmp210.YY27).tag != 31)goto _LL167;_tmp211=(_tmp210.YY27).f1;_LL166: yyzzz=_tmp211;
goto _LL164;_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_YY27);_LL164:;}return
yyzzz;}static char _tmp213[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp213,_tmp213 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp214=yy1;struct Cyc_List_List*_tmp215;_LL16A: if((_tmp214.YY28).tag != 32)goto
_LL16C;_tmp215=(_tmp214.YY28).f1;_LL16B: yyzzz=_tmp215;goto _LL169;_LL16C:;_LL16D:(
int)_throw((void*)& Cyc_yyfail_YY28);_LL169:;}return yyzzz;}static char _tmp217[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp217,
_tmp217 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union _tmp218=yy1;struct
Cyc_Parse_Declarator*_tmp219;_LL16F: if((_tmp218.YY29).tag != 33)goto _LL171;
_tmp219=(_tmp218.YY29).f1;_LL170: yyzzz=_tmp219;goto _LL16E;_LL171:;_LL172:(int)
_throw((void*)& Cyc_yyfail_YY29);_LL16E:;}return yyzzz;}static char _tmp21B[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp21B,
_tmp21B + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union _tmp21C=yy1;
struct Cyc_Parse_Abstractdeclarator*_tmp21D;_LL174: if((_tmp21C.YY30).tag != 34)
goto _LL176;_tmp21D=(_tmp21C.YY30).f1;_LL175: yyzzz=_tmp21D;goto _LL173;_LL176:;
_LL177:(int)_throw((void*)& Cyc_yyfail_YY30);_LL173:;}return yyzzz;}static char
_tmp21F[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp21F,_tmp21F + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1){int yyzzz;{
union Cyc_YYSTYPE_union _tmp220=yy1;int _tmp221;_LL179: if((_tmp220.YY31).tag != 35)
goto _LL17B;_tmp221=(_tmp220.YY31).f1;_LL17A: yyzzz=_tmp221;goto _LL178;_LL17B:;
_LL17C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL178:;}return yyzzz;}static char
_tmp223[8]="scope_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{
_tmp223,_tmp223 + 8}};void*Cyc_yyget_YY32(union Cyc_YYSTYPE_union yy1){void*yyzzz;{
union Cyc_YYSTYPE_union _tmp224=yy1;void*_tmp225;_LL17E: if((_tmp224.YY32).tag != 36)
goto _LL180;_tmp225=(_tmp224.YY32).f1;_LL17F: yyzzz=_tmp225;goto _LL17D;_LL180:;
_LL181:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17D:;}return yyzzz;}static char
_tmp227[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp227,_tmp227 + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union
_tmp228=yy1;struct Cyc_Absyn_Tunionfield*_tmp229;_LL183: if((_tmp228.YY33).tag != 
37)goto _LL185;_tmp229=(_tmp228.YY33).f1;_LL184: yyzzz=_tmp229;goto _LL182;_LL185:;
_LL186:(int)_throw((void*)& Cyc_yyfail_YY33);_LL182:;}return yyzzz;}static char
_tmp22B[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={
Cyc_Core_Failure,{_tmp22B,_tmp22B + 22}};struct Cyc_List_List*Cyc_yyget_YY34(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp22C=
yy1;struct Cyc_List_List*_tmp22D;_LL188: if((_tmp22C.YY34).tag != 38)goto _LL18A;
_tmp22D=(_tmp22C.YY34).f1;_LL189: yyzzz=_tmp22D;goto _LL187;_LL18A:;_LL18B:(int)
_throw((void*)& Cyc_yyfail_YY34);_LL187:;}return yyzzz;}static char _tmp22F[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp22F,
_tmp22F + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){struct
_tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp230=yy1;struct _tuple16*_tmp231;_LL18D:
if((_tmp230.YY35).tag != 39)goto _LL18F;_tmp231=(_tmp230.YY35).f1;_LL18E: yyzzz=
_tmp231;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)& Cyc_yyfail_YY35);_LL18C:;}
return yyzzz;}static char _tmp233[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp233,_tmp233 + 14}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp234=yy1;struct Cyc_List_List*_tmp235;_LL192: if((_tmp234.YY36).tag != 40)goto
_LL194;_tmp235=(_tmp234.YY36).f1;_LL193: yyzzz=_tmp235;goto _LL191;_LL194:;_LL195:(
int)_throw((void*)& Cyc_yyfail_YY36);_LL191:;}return yyzzz;}static char _tmp237[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp237,
_tmp237 + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){struct
_tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp238=yy1;struct _tuple2*_tmp239;_LL197:
if((_tmp238.YY37).tag != 41)goto _LL199;_tmp239=(_tmp238.YY37).f1;_LL198: yyzzz=
_tmp239;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_YY37);_LL196:;}
return yyzzz;}static char _tmp23B[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp23B,
_tmp23B + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp23C=yy1;struct Cyc_List_List*
_tmp23D;_LL19C: if((_tmp23C.YY38).tag != 42)goto _LL19E;_tmp23D=(_tmp23C.YY38).f1;
_LL19D: yyzzz=_tmp23D;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL19B:;}return yyzzz;}static char _tmp23F[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp23F,
_tmp23F + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){struct
_tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp240=yy1;struct _tuple17*_tmp241;_LL1A1:
if((_tmp240.YY39).tag != 43)goto _LL1A3;_tmp241=(_tmp240.YY39).f1;_LL1A2: yyzzz=
_tmp241;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A0:;}
return yyzzz;}static char _tmp243[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp243,_tmp243 + 15}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp244=yy1;struct Cyc_List_List*_tmp245;_LL1A6: if((_tmp244.YY40).tag != 44)goto
_LL1A8;_tmp245=(_tmp244.YY40).f1;_LL1A7: yyzzz=_tmp245;goto _LL1A5;_LL1A8:;_LL1A9:(
int)_throw((void*)& Cyc_yyfail_YY40);_LL1A5:;}return yyzzz;}static char _tmp247[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp247,
_tmp247 + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp248=yy1;struct Cyc_List_List*
_tmp249;_LL1AB: if((_tmp248.YY41).tag != 45)goto _LL1AD;_tmp249=(_tmp248.YY41).f1;
_LL1AC: yyzzz=_tmp249;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AA:;}return yyzzz;}static char _tmp24B[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp24B,_tmp24B + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp24C=yy1;void*
_tmp24D;_LL1B0: if((_tmp24C.YY42).tag != 46)goto _LL1B2;_tmp24D=(_tmp24C.YY42).f1;
_LL1B1: yyzzz=_tmp24D;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1AF:;}return yyzzz;}static char _tmp24F[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp24F,_tmp24F + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp250=yy1;void*
_tmp251;_LL1B5: if((_tmp250.YY43).tag != 47)goto _LL1B7;_tmp251=(_tmp250.YY43).f1;
_LL1B6: yyzzz=_tmp251;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B4:;}return yyzzz;}static char _tmp253[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp253,_tmp253 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp254=yy1;void*
_tmp255;_LL1BA: if((_tmp254.YY44).tag != 48)goto _LL1BC;_tmp255=(_tmp254.YY44).f1;
_LL1BB: yyzzz=_tmp255;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1B9:;}return yyzzz;}static char _tmp257[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp257,_tmp257 + 20}};
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp258=yy1;struct Cyc_List_List*_tmp259;_LL1BF: if((
_tmp258.YY45).tag != 49)goto _LL1C1;_tmp259=(_tmp258.YY45).f1;_LL1C0: yyzzz=_tmp259;
goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1BE:;}return
yyzzz;}static char _tmp25B[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp25B,_tmp25B + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp25C=yy1;void*
_tmp25D;_LL1C4: if((_tmp25C.YY46).tag != 50)goto _LL1C6;_tmp25D=(_tmp25C.YY46).f1;
_LL1C5: yyzzz=_tmp25D;goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C3:;}return yyzzz;}static char _tmp25F[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp25F,_tmp25F + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp260=yy1;struct Cyc_Absyn_Enumfield*_tmp261;_LL1C9: if((
_tmp260.YY47).tag != 51)goto _LL1CB;_tmp261=(_tmp260.YY47).f1;_LL1CA: yyzzz=_tmp261;
goto _LL1C8;_LL1CB:;_LL1CC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1C8:;}return
yyzzz;}static char _tmp263[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp263,_tmp263 + 20}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp264=yy1;struct Cyc_List_List*_tmp265;_LL1CE: if((_tmp264.YY48).tag != 52)goto
_LL1D0;_tmp265=(_tmp264.YY48).f1;_LL1CF: yyzzz=_tmp265;goto _LL1CD;_LL1D0:;_LL1D1:(
int)_throw((void*)& Cyc_yyfail_YY48);_LL1CD:;}return yyzzz;}static char _tmp267[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp267,
_tmp267 + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp268=yy1;struct Cyc_Core_Opt*
_tmp269;_LL1D3: if((_tmp268.YY49).tag != 53)goto _LL1D5;_tmp269=(_tmp268.YY49).f1;
_LL1D4: yyzzz=_tmp269;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D2:;}return yyzzz;}static char _tmp26B[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp26B,_tmp26B + 
26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp26C=yy1;struct Cyc_List_List*_tmp26D;_LL1D8: if((
_tmp26C.YY50).tag != 54)goto _LL1DA;_tmp26D=(_tmp26C.YY50).f1;_LL1D9: yyzzz=_tmp26D;
goto _LL1D7;_LL1DA:;_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY50);_LL1D7:;}return
yyzzz;}static char _tmp26F[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp26F,_tmp26F + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp270=yy1;struct Cyc_Absyn_Conref*_tmp271;_LL1DD: if((_tmp270.YY51).tag != 55)
goto _LL1DF;_tmp271=(_tmp270.YY51).f1;_LL1DE: yyzzz=_tmp271;goto _LL1DC;_LL1DF:;
_LL1E0:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DC:;}return yyzzz;}static char
_tmp273[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp273,_tmp273 + 40}};struct Cyc_List_List*Cyc_yyget_YY52(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp274=yy1;struct Cyc_List_List*_tmp275;_LL1E2: if((_tmp274.YY52).tag != 56)goto
_LL1E4;_tmp275=(_tmp274.YY52).f1;_LL1E3: yyzzz=_tmp275;goto _LL1E1;_LL1E4:;_LL1E5:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E1:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc(){return({struct Cyc_Yyltype _tmp276;_tmp276.timestamp=0;_tmp276.first_line=
0;_tmp276.first_column=0;_tmp276.last_line=0;_tmp276.last_column=0;_tmp276;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[359]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,130,2,2,112,128,126,2,
109,110,118,123,107,121,115,127,2,2,2,2,2,2,2,2,2,2,114,104,113,108,122,120,119,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,116,2,117,125,111,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,105,124,106,129,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103};static char _tmp277[
2]="$";static char _tmp278[6]="error";static char _tmp279[12]="$undefined.";static
char _tmp27A[5]="AUTO";static char _tmp27B[9]="REGISTER";static char _tmp27C[7]="STATIC";
static char _tmp27D[7]="EXTERN";static char _tmp27E[8]="TYPEDEF";static char _tmp27F[5]="VOID";
static char _tmp280[5]="CHAR";static char _tmp281[6]="SHORT";static char _tmp282[4]="INT";
static char _tmp283[5]="LONG";static char _tmp284[6]="FLOAT";static char _tmp285[7]="DOUBLE";
static char _tmp286[7]="SIGNED";static char _tmp287[9]="UNSIGNED";static char _tmp288[
6]="CONST";static char _tmp289[9]="VOLATILE";static char _tmp28A[9]="RESTRICT";
static char _tmp28B[7]="STRUCT";static char _tmp28C[6]="UNION";static char _tmp28D[5]="CASE";
static char _tmp28E[8]="DEFAULT";static char _tmp28F[7]="INLINE";static char _tmp290[7]="SIZEOF";
static char _tmp291[9]="OFFSETOF";static char _tmp292[3]="IF";static char _tmp293[5]="ELSE";
static char _tmp294[7]="SWITCH";static char _tmp295[6]="WHILE";static char _tmp296[3]="DO";
static char _tmp297[4]="FOR";static char _tmp298[5]="GOTO";static char _tmp299[9]="CONTINUE";
static char _tmp29A[6]="BREAK";static char _tmp29B[7]="RETURN";static char _tmp29C[5]="ENUM";
static char _tmp29D[8]="NULL_kw";static char _tmp29E[4]="LET";static char _tmp29F[6]="THROW";
static char _tmp2A0[4]="TRY";static char _tmp2A1[6]="CATCH";static char _tmp2A2[7]="EXPORT";
static char _tmp2A3[4]="NEW";static char _tmp2A4[9]="ABSTRACT";static char _tmp2A5[9]="FALLTHRU";
static char _tmp2A6[6]="USING";static char _tmp2A7[10]="NAMESPACE";static char _tmp2A8[
7]="TUNION";static char _tmp2A9[8]="XTUNION";static char _tmp2AA[7]="MALLOC";static
char _tmp2AB[8]="RMALLOC";static char _tmp2AC[7]="CALLOC";static char _tmp2AD[8]="RCALLOC";
static char _tmp2AE[9]="REGION_T";static char _tmp2AF[9]="SIZEOF_T";static char
_tmp2B0[6]="TAG_T";static char _tmp2B1[7]="REGION";static char _tmp2B2[5]="RNEW";
static char _tmp2B3[8]="REGIONS";static char _tmp2B4[13]="RESET_REGION";static char
_tmp2B5[4]="GEN";static char _tmp2B6[14]="NOZEROTERM_kw";static char _tmp2B7[12]="ZEROTERM_kw";
static char _tmp2B8[7]="PORTON";static char _tmp2B9[8]="PORTOFF";static char _tmp2BA[8]="FLAT_kw";
static char _tmp2BB[12]="DYNREGION_T";static char _tmp2BC[7]="PTR_OP";static char
_tmp2BD[7]="INC_OP";static char _tmp2BE[7]="DEC_OP";static char _tmp2BF[8]="LEFT_OP";
static char _tmp2C0[9]="RIGHT_OP";static char _tmp2C1[6]="LE_OP";static char _tmp2C2[6]="GE_OP";
static char _tmp2C3[6]="EQ_OP";static char _tmp2C4[6]="NE_OP";static char _tmp2C5[7]="AND_OP";
static char _tmp2C6[6]="OR_OP";static char _tmp2C7[11]="MUL_ASSIGN";static char
_tmp2C8[11]="DIV_ASSIGN";static char _tmp2C9[11]="MOD_ASSIGN";static char _tmp2CA[11]="ADD_ASSIGN";
static char _tmp2CB[11]="SUB_ASSIGN";static char _tmp2CC[12]="LEFT_ASSIGN";static
char _tmp2CD[13]="RIGHT_ASSIGN";static char _tmp2CE[11]="AND_ASSIGN";static char
_tmp2CF[11]="XOR_ASSIGN";static char _tmp2D0[10]="OR_ASSIGN";static char _tmp2D1[9]="ELLIPSIS";
static char _tmp2D2[11]="LEFT_RIGHT";static char _tmp2D3[12]="COLON_COLON";static
char _tmp2D4[11]="IDENTIFIER";static char _tmp2D5[17]="INTEGER_CONSTANT";static char
_tmp2D6[7]="STRING";static char _tmp2D7[19]="CHARACTER_CONSTANT";static char _tmp2D8[
18]="FLOATING_CONSTANT";static char _tmp2D9[9]="TYPE_VAR";static char _tmp2DA[13]="TYPEDEF_NAME";
static char _tmp2DB[16]="QUAL_IDENTIFIER";static char _tmp2DC[18]="QUAL_TYPEDEF_NAME";
static char _tmp2DD[13]="TYPE_INTEGER";static char _tmp2DE[10]="ATTRIBUTE";static
char _tmp2DF[4]="';'";static char _tmp2E0[4]="'{'";static char _tmp2E1[4]="'}'";
static char _tmp2E2[4]="','";static char _tmp2E3[4]="'='";static char _tmp2E4[4]="'('";
static char _tmp2E5[4]="')'";static char _tmp2E6[4]="'_'";static char _tmp2E7[4]="'$'";
static char _tmp2E8[4]="'<'";static char _tmp2E9[4]="':'";static char _tmp2EA[4]="'.'";
static char _tmp2EB[4]="'['";static char _tmp2EC[4]="']'";static char _tmp2ED[4]="'*'";
static char _tmp2EE[4]="'@'";static char _tmp2EF[4]="'?'";static char _tmp2F0[4]="'-'";
static char _tmp2F1[4]="'>'";static char _tmp2F2[4]="'+'";static char _tmp2F3[4]="'|'";
static char _tmp2F4[4]="'^'";static char _tmp2F5[4]="'&'";static char _tmp2F6[4]="'/'";
static char _tmp2F7[4]="'%'";static char _tmp2F8[4]="'~'";static char _tmp2F9[4]="'!'";
static char _tmp2FA[5]="prog";static char _tmp2FB[17]="translation_unit";static char
_tmp2FC[12]="export_list";static char _tmp2FD[19]="export_list_values";static char
_tmp2FE[21]="external_declaration";static char _tmp2FF[20]="function_definition";
static char _tmp300[21]="function_definition2";static char _tmp301[13]="using_action";
static char _tmp302[15]="unusing_action";static char _tmp303[17]="namespace_action";
static char _tmp304[19]="unnamespace_action";static char _tmp305[12]="declaration";
static char _tmp306[17]="declaration_list";static char _tmp307[23]="declaration_specifiers";
static char _tmp308[24]="storage_class_specifier";static char _tmp309[15]="attributes_opt";
static char _tmp30A[11]="attributes";static char _tmp30B[15]="attribute_list";static
char _tmp30C[10]="attribute";static char _tmp30D[15]="type_specifier";static char
_tmp30E[25]="type_specifier_notypedef";static char _tmp30F[5]="kind";static char
_tmp310[15]="type_qualifier";static char _tmp311[15]="enum_specifier";static char
_tmp312[11]="enum_field";static char _tmp313[22]="enum_declaration_list";static
char _tmp314[26]="struct_or_union_specifier";static char _tmp315[16]="type_params_opt";
static char _tmp316[16]="struct_or_union";static char _tmp317[24]="struct_declaration_list";
static char _tmp318[25]="struct_declaration_list0";static char _tmp319[21]="init_declarator_list";
static char _tmp31A[22]="init_declarator_list0";static char _tmp31B[16]="init_declarator";
static char _tmp31C[19]="struct_declaration";static char _tmp31D[25]="specifier_qualifier_list";
static char _tmp31E[35]="notypedef_specifier_qualifier_list";static char _tmp31F[23]="struct_declarator_list";
static char _tmp320[24]="struct_declarator_list0";static char _tmp321[18]="struct_declarator";
static char _tmp322[17]="tunion_specifier";static char _tmp323[9]="flat_opt";static
char _tmp324[18]="tunion_or_xtunion";static char _tmp325[17]="tunionfield_list";
static char _tmp326[18]="tunionfield_scope";static char _tmp327[12]="tunionfield";
static char _tmp328[11]="declarator";static char _tmp329[23]="declarator_withtypedef";
static char _tmp32A[18]="direct_declarator";static char _tmp32B[30]="direct_declarator_withtypedef";
static char _tmp32C[8]="pointer";static char _tmp32D[12]="one_pointer";static char
_tmp32E[23]="pointer_null_and_bound";static char _tmp32F[14]="pointer_bound";
static char _tmp330[18]="zeroterm_qual_opt";static char _tmp331[12]="opt_rgn_opt";
static char _tmp332[8]="rgn_opt";static char _tmp333[11]="tqual_list";static char
_tmp334[20]="parameter_type_list";static char _tmp335[9]="type_var";static char
_tmp336[16]="optional_effect";static char _tmp337[19]="optional_rgn_order";static
char _tmp338[10]="rgn_order";static char _tmp339[16]="optional_inject";static char
_tmp33A[11]="effect_set";static char _tmp33B[14]="atomic_effect";static char _tmp33C[
11]="region_set";static char _tmp33D[15]="parameter_list";static char _tmp33E[22]="parameter_declaration";
static char _tmp33F[16]="identifier_list";static char _tmp340[17]="identifier_list0";
static char _tmp341[12]="initializer";static char _tmp342[18]="array_initializer";
static char _tmp343[17]="initializer_list";static char _tmp344[12]="designation";
static char _tmp345[16]="designator_list";static char _tmp346[11]="designator";
static char _tmp347[10]="type_name";static char _tmp348[14]="any_type_name";static
char _tmp349[15]="type_name_list";static char _tmp34A[20]="abstract_declarator";
static char _tmp34B[27]="direct_abstract_declarator";static char _tmp34C[10]="statement";
static char _tmp34D[13]="open_exp_opt";static char _tmp34E[18]="labeled_statement";
static char _tmp34F[21]="expression_statement";static char _tmp350[19]="compound_statement";
static char _tmp351[16]="block_item_list";static char _tmp352[20]="selection_statement";
static char _tmp353[15]="switch_clauses";static char _tmp354[20]="iteration_statement";
static char _tmp355[15]="jump_statement";static char _tmp356[12]="exp_pattern";
static char _tmp357[20]="conditional_pattern";static char _tmp358[19]="logical_or_pattern";
static char _tmp359[20]="logical_and_pattern";static char _tmp35A[21]="inclusive_or_pattern";
static char _tmp35B[21]="exclusive_or_pattern";static char _tmp35C[12]="and_pattern";
static char _tmp35D[17]="equality_pattern";static char _tmp35E[19]="relational_pattern";
static char _tmp35F[14]="shift_pattern";static char _tmp360[17]="additive_pattern";
static char _tmp361[23]="multiplicative_pattern";static char _tmp362[13]="cast_pattern";
static char _tmp363[14]="unary_pattern";static char _tmp364[16]="postfix_pattern";
static char _tmp365[16]="primary_pattern";static char _tmp366[8]="pattern";static
char _tmp367[19]="tuple_pattern_list";static char _tmp368[20]="tuple_pattern_list0";
static char _tmp369[14]="field_pattern";static char _tmp36A[19]="field_pattern_list";
static char _tmp36B[20]="field_pattern_list0";static char _tmp36C[11]="expression";
static char _tmp36D[22]="assignment_expression";static char _tmp36E[20]="assignment_operator";
static char _tmp36F[23]="conditional_expression";static char _tmp370[20]="constant_expression";
static char _tmp371[22]="logical_or_expression";static char _tmp372[23]="logical_and_expression";
static char _tmp373[24]="inclusive_or_expression";static char _tmp374[24]="exclusive_or_expression";
static char _tmp375[15]="and_expression";static char _tmp376[20]="equality_expression";
static char _tmp377[22]="relational_expression";static char _tmp378[17]="shift_expression";
static char _tmp379[20]="additive_expression";static char _tmp37A[26]="multiplicative_expression";
static char _tmp37B[16]="cast_expression";static char _tmp37C[17]="unary_expression";
static char _tmp37D[15]="unary_operator";static char _tmp37E[19]="postfix_expression";
static char _tmp37F[19]="primary_expression";static char _tmp380[25]="argument_expression_list";
static char _tmp381[26]="argument_expression_list0";static char _tmp382[9]="constant";
static char _tmp383[20]="qual_opt_identifier";static char _tmp384[17]="qual_opt_typedef";
static char _tmp385[18]="struct_union_name";static char _tmp386[11]="field_name";
static char _tmp387[12]="right_angle";static struct _dynforward_ptr Cyc_yytname[273]={{
_tmp277,_tmp277 + 2},{_tmp278,_tmp278 + 6},{_tmp279,_tmp279 + 12},{_tmp27A,_tmp27A + 
5},{_tmp27B,_tmp27B + 9},{_tmp27C,_tmp27C + 7},{_tmp27D,_tmp27D + 7},{_tmp27E,
_tmp27E + 8},{_tmp27F,_tmp27F + 5},{_tmp280,_tmp280 + 5},{_tmp281,_tmp281 + 6},{
_tmp282,_tmp282 + 4},{_tmp283,_tmp283 + 5},{_tmp284,_tmp284 + 6},{_tmp285,_tmp285 + 
7},{_tmp286,_tmp286 + 7},{_tmp287,_tmp287 + 9},{_tmp288,_tmp288 + 6},{_tmp289,
_tmp289 + 9},{_tmp28A,_tmp28A + 9},{_tmp28B,_tmp28B + 7},{_tmp28C,_tmp28C + 6},{
_tmp28D,_tmp28D + 5},{_tmp28E,_tmp28E + 8},{_tmp28F,_tmp28F + 7},{_tmp290,_tmp290 + 
7},{_tmp291,_tmp291 + 9},{_tmp292,_tmp292 + 3},{_tmp293,_tmp293 + 5},{_tmp294,
_tmp294 + 7},{_tmp295,_tmp295 + 6},{_tmp296,_tmp296 + 3},{_tmp297,_tmp297 + 4},{
_tmp298,_tmp298 + 5},{_tmp299,_tmp299 + 9},{_tmp29A,_tmp29A + 6},{_tmp29B,_tmp29B + 
7},{_tmp29C,_tmp29C + 5},{_tmp29D,_tmp29D + 8},{_tmp29E,_tmp29E + 4},{_tmp29F,
_tmp29F + 6},{_tmp2A0,_tmp2A0 + 4},{_tmp2A1,_tmp2A1 + 6},{_tmp2A2,_tmp2A2 + 7},{
_tmp2A3,_tmp2A3 + 4},{_tmp2A4,_tmp2A4 + 9},{_tmp2A5,_tmp2A5 + 9},{_tmp2A6,_tmp2A6 + 
6},{_tmp2A7,_tmp2A7 + 10},{_tmp2A8,_tmp2A8 + 7},{_tmp2A9,_tmp2A9 + 8},{_tmp2AA,
_tmp2AA + 7},{_tmp2AB,_tmp2AB + 8},{_tmp2AC,_tmp2AC + 7},{_tmp2AD,_tmp2AD + 8},{
_tmp2AE,_tmp2AE + 9},{_tmp2AF,_tmp2AF + 9},{_tmp2B0,_tmp2B0 + 6},{_tmp2B1,_tmp2B1 + 
7},{_tmp2B2,_tmp2B2 + 5},{_tmp2B3,_tmp2B3 + 8},{_tmp2B4,_tmp2B4 + 13},{_tmp2B5,
_tmp2B5 + 4},{_tmp2B6,_tmp2B6 + 14},{_tmp2B7,_tmp2B7 + 12},{_tmp2B8,_tmp2B8 + 7},{
_tmp2B9,_tmp2B9 + 8},{_tmp2BA,_tmp2BA + 8},{_tmp2BB,_tmp2BB + 12},{_tmp2BC,_tmp2BC + 
7},{_tmp2BD,_tmp2BD + 7},{_tmp2BE,_tmp2BE + 7},{_tmp2BF,_tmp2BF + 8},{_tmp2C0,
_tmp2C0 + 9},{_tmp2C1,_tmp2C1 + 6},{_tmp2C2,_tmp2C2 + 6},{_tmp2C3,_tmp2C3 + 6},{
_tmp2C4,_tmp2C4 + 6},{_tmp2C5,_tmp2C5 + 7},{_tmp2C6,_tmp2C6 + 6},{_tmp2C7,_tmp2C7 + 
11},{_tmp2C8,_tmp2C8 + 11},{_tmp2C9,_tmp2C9 + 11},{_tmp2CA,_tmp2CA + 11},{_tmp2CB,
_tmp2CB + 11},{_tmp2CC,_tmp2CC + 12},{_tmp2CD,_tmp2CD + 13},{_tmp2CE,_tmp2CE + 11},{
_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0 + 10},{_tmp2D1,_tmp2D1 + 9},{_tmp2D2,_tmp2D2
+ 11},{_tmp2D3,_tmp2D3 + 12},{_tmp2D4,_tmp2D4 + 11},{_tmp2D5,_tmp2D5 + 17},{_tmp2D6,
_tmp2D6 + 7},{_tmp2D7,_tmp2D7 + 19},{_tmp2D8,_tmp2D8 + 18},{_tmp2D9,_tmp2D9 + 9},{
_tmp2DA,_tmp2DA + 13},{_tmp2DB,_tmp2DB + 16},{_tmp2DC,_tmp2DC + 18},{_tmp2DD,
_tmp2DD + 13},{_tmp2DE,_tmp2DE + 10},{_tmp2DF,_tmp2DF + 4},{_tmp2E0,_tmp2E0 + 4},{
_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3 + 4},{_tmp2E4,_tmp2E4 + 
4},{_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7 + 4},{_tmp2E8,
_tmp2E8 + 4},{_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA + 4},{_tmp2EB,_tmp2EB + 4},{
_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE + 4},{_tmp2EF,_tmp2EF + 
4},{_tmp2F0,_tmp2F0 + 4},{_tmp2F1,_tmp2F1 + 4},{_tmp2F2,_tmp2F2 + 4},{_tmp2F3,
_tmp2F3 + 4},{_tmp2F4,_tmp2F4 + 4},{_tmp2F5,_tmp2F5 + 4},{_tmp2F6,_tmp2F6 + 4},{
_tmp2F7,_tmp2F7 + 4},{_tmp2F8,_tmp2F8 + 4},{_tmp2F9,_tmp2F9 + 4},{_tmp2FA,_tmp2FA + 
5},{_tmp2FB,_tmp2FB + 17},{_tmp2FC,_tmp2FC + 12},{_tmp2FD,_tmp2FD + 19},{_tmp2FE,
_tmp2FE + 21},{_tmp2FF,_tmp2FF + 20},{_tmp300,_tmp300 + 21},{_tmp301,_tmp301 + 13},{
_tmp302,_tmp302 + 15},{_tmp303,_tmp303 + 17},{_tmp304,_tmp304 + 19},{_tmp305,
_tmp305 + 12},{_tmp306,_tmp306 + 17},{_tmp307,_tmp307 + 23},{_tmp308,_tmp308 + 24},{
_tmp309,_tmp309 + 15},{_tmp30A,_tmp30A + 11},{_tmp30B,_tmp30B + 15},{_tmp30C,
_tmp30C + 10},{_tmp30D,_tmp30D + 15},{_tmp30E,_tmp30E + 25},{_tmp30F,_tmp30F + 5},{
_tmp310,_tmp310 + 15},{_tmp311,_tmp311 + 15},{_tmp312,_tmp312 + 11},{_tmp313,
_tmp313 + 22},{_tmp314,_tmp314 + 26},{_tmp315,_tmp315 + 16},{_tmp316,_tmp316 + 16},{
_tmp317,_tmp317 + 24},{_tmp318,_tmp318 + 25},{_tmp319,_tmp319 + 21},{_tmp31A,
_tmp31A + 22},{_tmp31B,_tmp31B + 16},{_tmp31C,_tmp31C + 19},{_tmp31D,_tmp31D + 25},{
_tmp31E,_tmp31E + 35},{_tmp31F,_tmp31F + 23},{_tmp320,_tmp320 + 24},{_tmp321,
_tmp321 + 18},{_tmp322,_tmp322 + 17},{_tmp323,_tmp323 + 9},{_tmp324,_tmp324 + 18},{
_tmp325,_tmp325 + 17},{_tmp326,_tmp326 + 18},{_tmp327,_tmp327 + 12},{_tmp328,
_tmp328 + 11},{_tmp329,_tmp329 + 23},{_tmp32A,_tmp32A + 18},{_tmp32B,_tmp32B + 30},{
_tmp32C,_tmp32C + 8},{_tmp32D,_tmp32D + 12},{_tmp32E,_tmp32E + 23},{_tmp32F,_tmp32F
+ 14},{_tmp330,_tmp330 + 18},{_tmp331,_tmp331 + 12},{_tmp332,_tmp332 + 8},{_tmp333,
_tmp333 + 11},{_tmp334,_tmp334 + 20},{_tmp335,_tmp335 + 9},{_tmp336,_tmp336 + 16},{
_tmp337,_tmp337 + 19},{_tmp338,_tmp338 + 10},{_tmp339,_tmp339 + 16},{_tmp33A,
_tmp33A + 11},{_tmp33B,_tmp33B + 14},{_tmp33C,_tmp33C + 11},{_tmp33D,_tmp33D + 15},{
_tmp33E,_tmp33E + 22},{_tmp33F,_tmp33F + 16},{_tmp340,_tmp340 + 17},{_tmp341,
_tmp341 + 12},{_tmp342,_tmp342 + 18},{_tmp343,_tmp343 + 17},{_tmp344,_tmp344 + 12},{
_tmp345,_tmp345 + 16},{_tmp346,_tmp346 + 11},{_tmp347,_tmp347 + 10},{_tmp348,
_tmp348 + 14},{_tmp349,_tmp349 + 15},{_tmp34A,_tmp34A + 20},{_tmp34B,_tmp34B + 27},{
_tmp34C,_tmp34C + 10},{_tmp34D,_tmp34D + 13},{_tmp34E,_tmp34E + 18},{_tmp34F,
_tmp34F + 21},{_tmp350,_tmp350 + 19},{_tmp351,_tmp351 + 16},{_tmp352,_tmp352 + 20},{
_tmp353,_tmp353 + 15},{_tmp354,_tmp354 + 20},{_tmp355,_tmp355 + 15},{_tmp356,
_tmp356 + 12},{_tmp357,_tmp357 + 20},{_tmp358,_tmp358 + 19},{_tmp359,_tmp359 + 20},{
_tmp35A,_tmp35A + 21},{_tmp35B,_tmp35B + 21},{_tmp35C,_tmp35C + 12},{_tmp35D,
_tmp35D + 17},{_tmp35E,_tmp35E + 19},{_tmp35F,_tmp35F + 14},{_tmp360,_tmp360 + 17},{
_tmp361,_tmp361 + 23},{_tmp362,_tmp362 + 13},{_tmp363,_tmp363 + 14},{_tmp364,
_tmp364 + 16},{_tmp365,_tmp365 + 16},{_tmp366,_tmp366 + 8},{_tmp367,_tmp367 + 19},{
_tmp368,_tmp368 + 20},{_tmp369,_tmp369 + 14},{_tmp36A,_tmp36A + 19},{_tmp36B,
_tmp36B + 20},{_tmp36C,_tmp36C + 11},{_tmp36D,_tmp36D + 22},{_tmp36E,_tmp36E + 20},{
_tmp36F,_tmp36F + 23},{_tmp370,_tmp370 + 20},{_tmp371,_tmp371 + 22},{_tmp372,
_tmp372 + 23},{_tmp373,_tmp373 + 24},{_tmp374,_tmp374 + 24},{_tmp375,_tmp375 + 15},{
_tmp376,_tmp376 + 20},{_tmp377,_tmp377 + 22},{_tmp378,_tmp378 + 17},{_tmp379,
_tmp379 + 20},{_tmp37A,_tmp37A + 26},{_tmp37B,_tmp37B + 16},{_tmp37C,_tmp37C + 17},{
_tmp37D,_tmp37D + 15},{_tmp37E,_tmp37E + 19},{_tmp37F,_tmp37F + 19},{_tmp380,
_tmp380 + 25},{_tmp381,_tmp381 + 26},{_tmp382,_tmp382 + 9},{_tmp383,_tmp383 + 20},{
_tmp384,_tmp384 + 17},{_tmp385,_tmp385 + 18},{_tmp386,_tmp386 + 11},{_tmp387,
_tmp387 + 12}};static short Cyc_yyr1[499]={0,131,132,132,132,132,132,132,132,132,
132,132,133,133,134,134,135,135,135,136,136,136,136,137,137,138,139,140,141,142,
142,142,142,143,143,144,144,144,144,144,144,144,144,144,144,145,145,145,145,145,
145,145,146,146,147,148,148,149,149,149,149,149,149,150,150,151,151,151,151,151,
151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,152,153,
153,153,154,154,154,155,155,156,156,156,157,157,157,158,158,159,159,160,160,161,
161,162,163,163,164,164,165,166,166,166,166,166,166,167,167,167,167,167,167,168,
169,169,170,170,170,171,171,171,172,172,173,173,174,174,174,174,175,175,175,176,
176,177,177,178,178,179,179,179,179,179,179,179,179,179,179,180,180,180,180,180,
180,180,180,180,180,180,181,181,182,183,183,183,183,184,184,184,185,185,185,186,
186,186,187,187,187,188,188,189,189,189,189,190,190,191,191,192,192,193,193,194,
194,195,195,196,196,196,196,197,197,198,198,199,199,199,200,201,201,202,202,203,
203,203,203,204,204,204,204,205,206,206,207,207,208,208,209,209,209,209,209,210,
210,211,211,211,212,212,212,212,212,212,212,212,212,212,212,213,213,213,213,213,
213,213,213,213,213,213,214,214,215,216,216,217,217,218,218,218,218,218,218,219,
219,219,219,219,219,220,220,220,220,220,220,221,221,221,221,221,221,221,221,221,
221,221,221,221,221,222,222,222,222,222,222,222,222,223,224,224,225,225,226,226,
227,227,228,228,229,229,230,230,230,231,231,231,231,231,232,232,232,233,233,233,
234,234,234,234,235,235,236,236,236,236,236,236,236,237,238,239,239,239,239,239,
239,239,239,239,239,239,239,239,239,240,240,240,241,241,242,242,243,243,243,244,
244,245,245,246,246,247,247,247,247,247,247,247,247,247,247,247,248,248,248,248,
248,248,248,249,250,250,251,251,252,252,253,253,254,254,255,255,255,256,256,256,
256,256,257,257,257,258,258,258,259,259,259,259,260,260,261,261,261,261,261,261,
261,261,261,261,261,261,261,261,261,261,262,262,262,263,263,263,263,263,263,263,
263,263,263,264,264,264,264,264,264,264,264,264,265,266,266,267,267,267,267,268,
268,269,269,270,270,271,271,272,272};static short Cyc_yyr2[499]={0,1,2,3,5,3,5,6,7,
3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,
2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,4,1,1,1,
1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,
1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,
4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,
0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,
1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,4,9,6,5,0,5,3,1,2,2,3,1,2,1,2,1,2,
5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,
3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,
3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,
2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,
2,2,4,2,6,6,5,4,6,9,11,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,
1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[992]={0,18,45,46,47,48,50,65,68,69,
70,71,72,73,74,75,89,90,91,105,106,41,0,0,51,0,0,0,0,86,0,0,138,0,489,201,491,
490,492,87,0,0,66,0,183,183,181,1,0,16,0,0,17,0,35,43,37,63,39,76,77,0,78,0,139,
151,0,176,186,79,155,103,49,48,42,0,93,488,0,489,485,486,487,0,365,0,0,0,0,224,0,
367,368,25,27,139,139,139,0,0,139,0,0,0,0,0,139,139,179,180,182,2,0,0,0,0,29,0,
111,112,114,36,44,38,40,139,493,494,103,140,141,189,139,33,139,0,19,139,139,0,
164,152,177,188,187,192,139,64,0,49,97,0,95,0,489,374,0,0,0,0,0,0,0,0,0,0,0,103,
0,0,475,139,0,0,462,0,0,460,461,0,391,393,406,414,416,418,420,422,424,427,432,
435,438,442,0,444,463,474,473,0,375,373,32,0,0,103,0,0,0,121,117,119,242,244,0,0,
0,9,10,0,495,496,202,88,0,0,156,67,222,0,219,0,0,0,3,0,5,0,30,0,0,139,20,0,139,
109,0,102,191,0,190,0,0,0,0,0,0,0,0,0,0,0,0,0,489,279,281,287,283,0,285,265,266,
267,0,268,269,270,0,34,21,114,209,225,0,0,205,203,0,249,0,186,0,194,52,193,0,0,
98,94,0,0,369,0,0,139,452,139,408,442,0,409,410,0,0,0,0,0,0,139,445,446,139,0,0,
0,448,449,447,0,366,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,396,397,398,399,400,
401,402,403,404,405,395,0,450,0,469,470,0,0,0,477,0,103,381,382,0,379,0,226,0,0,
0,139,245,217,0,122,127,123,125,118,120,139,0,251,243,252,498,497,0,81,84,85,139,
82,58,57,0,55,157,139,221,153,251,223,165,166,139,80,139,185,184,26,0,28,0,113,
115,228,227,22,100,110,0,0,0,129,130,132,0,103,103,0,0,0,0,0,0,139,0,316,317,318,
0,0,320,0,276,0,0,0,0,288,284,114,286,282,280,210,139,0,0,216,204,211,160,0,0,
139,205,162,139,163,158,186,195,53,104,0,99,413,96,92,377,378,0,0,0,229,0,0,233,
0,0,0,238,0,0,0,0,0,139,0,0,0,476,483,0,482,392,415,0,417,419,421,423,425,426,
430,431,428,429,433,434,437,436,439,440,441,394,468,465,0,467,0,139,0,370,0,376,
31,388,0,384,389,0,386,371,0,0,246,128,124,126,0,205,0,186,0,253,139,139,0,264,
248,0,0,0,0,121,0,103,139,139,0,175,154,220,4,6,0,133,116,0,0,205,0,136,0,0,0,
295,0,0,0,0,0,315,319,0,0,0,0,0,0,0,0,278,139,23,203,139,213,0,0,206,0,161,209,
197,250,159,195,178,0,7,0,451,0,0,241,0,230,0,234,237,239,456,0,0,0,0,0,0,0,0,
443,479,0,0,466,464,0,0,380,383,385,372,0,247,218,260,0,254,255,186,0,205,0,186,
0,83,0,0,0,54,56,0,167,0,205,0,0,186,0,131,134,139,103,146,0,0,0,0,0,0,0,0,0,0,0,
0,0,295,321,0,0,272,0,0,0,24,205,0,214,212,0,205,139,196,0,8,0,0,0,240,231,235,0,
0,0,0,411,412,455,481,0,484,407,478,480,0,387,390,259,257,262,0,263,256,186,0,61,
59,60,168,171,0,173,174,169,186,0,137,148,147,0,0,142,289,295,0,0,0,139,0,0,323,
324,326,328,330,332,334,336,339,344,347,350,354,356,363,364,0,139,292,301,0,0,0,
0,0,0,0,0,0,0,322,0,0,0,0,275,199,215,207,198,203,13,0,0,454,453,0,236,457,139,0,
471,0,261,258,0,172,170,101,135,149,146,146,0,0,295,139,360,139,0,357,139,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,358,296,0,303,0,0,0,311,0,0,0,0,0,0,294,0,271,
274,0,0,205,12,14,0,0,0,0,472,0,139,145,144,290,291,0,0,0,0,295,297,327,0,329,
331,333,335,337,338,342,343,340,341,345,346,349,348,351,352,353,0,302,304,305,0,
313,312,0,307,0,0,0,277,0,208,200,15,0,0,139,0,0,293,359,0,355,298,0,139,306,314,
308,309,0,0,232,458,0,62,150,0,0,325,295,299,310,273,0,362,361,300,459,0,0,0};
static short Cyc_yydefgoto[142]={989,47,635,828,48,49,267,50,422,51,424,52,134,53,
54,484,209,406,407,210,57,222,211,59,150,151,60,147,61,243,244,117,118,119,245,
212,389,434,435,436,62,63,131,775,776,777,64,437,65,411,66,67,68,108,145,249,294,
632,563,69,564,476,624,468,472,473,384,287,230,88,89,500,427,501,502,503,504,213,
289,290,565,395,270,612,271,272,273,274,275,704,276,277,785,786,787,788,789,790,
791,792,793,794,795,796,797,798,799,800,373,374,375,553,554,555,278,180,361,181,
490,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,516,517,197,198,
71,829,223,399};static short Cyc_yypact[992]={2596,- -32768,- -32768,- -32768,- -32768,
10,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,3083,243,2862,- -32768,251,28,23,
33,39,100,116,- -32768,156,- -32768,200,- -32768,- -32768,- -32768,- -32768,211,483,
234,226,244,244,239,- -32768,2475,- -32768,338,587,- -32768,506,3083,3083,3083,-
-32768,3083,- -32768,- -32768,328,- -32768,717,3151,205,98,629,716,- -32768,- -32768,
242,261,275,- -32768,251,271,- -32768,3242,104,- -32768,- -32768,- -32768,5037,-
-32768,276,317,3242,352,353,370,- -32768,121,- -32768,- -32768,5502,5502,5502,2475,
2475,5502,31,356,558,373,31,5757,1977,- -32768,- -32768,- -32768,- -32768,2475,2716,
2475,2716,- -32768,391,360,- -32768,3014,- -32768,- -32768,- -32768,- -32768,5622,-
-32768,- -32768,242,- -32768,- -32768,125,1726,- -32768,3151,506,- -32768,3214,5502,
3842,- -32768,205,- -32768,- -32768,- -32768,133,5502,- -32768,2716,- -32768,390,393,
396,251,- 21,- -32768,3242,155,5217,409,5037,5091,415,461,463,470,481,242,5271,
5271,- -32768,2100,491,5325,- -32768,5325,5325,- -32768,- -32768,66,- -32768,- -32768,
- 44,467,433,479,486,774,115,793,167,321,- -32768,924,5325,335,- 30,- -32768,515,
2752,503,- -32768,- -32768,521,5037,242,2752,513,231,5517,5862,5517,245,- -32768,- 6,
- 6,- 6,- -32768,- -32768,120,- -32768,- -32768,- -32768,- -32768,17,533,- -32768,- -32768,
553,78,- -32768,554,- 4,542,- -32768,562,- -32768,573,- -32768,558,3936,3151,- -32768,
595,5637,- -32768,755,606,- -32768,251,- -32768,613,139,621,3367,644,646,651,670,
3999,3367,293,- 13,677,603,- -32768,- -32768,1598,1598,506,1598,- -32768,- -32768,-
-32768,688,- -32768,- -32768,- -32768,151,- -32768,- -32768,700,720,- -32768,107,686,
707,385,726,730,- 11,716,709,- -32768,738,- -32768,- 11,724,251,- -32768,5037,752,-
-32768,740,750,2100,- -32768,5757,- -32768,- -32768,3466,- -32768,788,5037,5037,5037,
5037,5037,776,2100,- -32768,- -32768,1854,769,401,5037,- -32768,- -32768,- -32768,
5037,- -32768,5325,5037,5325,5325,5325,5325,5325,5325,5325,5325,5325,5325,5325,
5325,5325,5325,5325,5325,5325,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,5037,- -32768,31,- -32768,- -32768,4062,31,
5037,- -32768,792,242,- -32768,- -32768,799,806,3242,- -32768,436,925,805,5502,-
-32768,826,828,- -32768,5862,5862,5862,- -32768,- -32768,2783,4125,285,- -32768,584,
- -32768,- -32768,107,- -32768,- -32768,- -32768,5502,- -32768,- -32768,827,832,831,-
-32768,1092,- -32768,703,689,- -32768,- -32768,- -32768,5757,- -32768,2223,- -32768,-
-32768,- -32768,2475,- -32768,2475,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,953,5037,835,836,- -32768,830,182,242,189,5037,5037,840,845,5037,915,1275,
849,- -32768,- -32768,- -32768,448,914,- -32768,4219,851,859,867,5037,3367,- -32768,-
-32768,3014,- -32768,- -32768,- -32768,- -32768,5757,852,241,- -32768,- -32768,839,-
-32768,107,854,5742,707,- -32768,5502,- -32768,- -32768,716,782,- -32768,- -32768,
2354,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,857,861,876,- -32768,31,5037,
- -32768,764,3936,146,- -32768,425,863,865,866,446,5757,864,869,5154,- -32768,-
-32768,868,870,- -32768,467,282,433,479,486,774,115,115,793,793,793,793,167,167,
321,321,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,871,- -32768,- 36,5502,
3748,- -32768,3357,- -32768,- -32768,- -32768,3242,- -32768,- -32768,874,878,- -32768,-
41,885,- -32768,- -32768,- -32768,- -32768,877,707,879,716,873,584,3277,5502,4282,-
-32768,- -32768,- 6,745,883,17,2896,887,242,3214,5502,4376,- -32768,703,- -32768,-
-32768,- -32768,605,- -32768,- -32768,755,5037,707,251,881,464,471,5037,855,487,889,
4470,4533,509,- -32768,- -32768,891,872,892,908,3367,895,897,520,- -32768,3151,-
-32768,919,5502,- -32768,921,107,- -32768,906,- -32768,213,- -32768,730,- -32768,782,
- -32768,926,- -32768,2475,928,428,922,- -32768,927,- -32768,3560,- -32768,- -32768,-
-32768,- -32768,5037,1013,5037,5091,929,928,932,3748,- -32768,- -32768,5037,5037,-
-32768,- -32768,- 11,775,- -32768,- -32768,- -32768,- -32768,1313,- -32768,- -32768,-
-32768,935,- -32768,- -32768,716,937,707,- 11,716,931,- -32768,527,939,940,- -32768,-
-32768,945,- -32768,947,707,948,- 11,716,942,- -32768,- -32768,5622,242,886,3367,955,
959,1143,951,968,3367,5037,4596,579,4659,592,4722,855,- -32768,971,969,- -32768,
984,82,975,- -32768,707,154,- -32768,- -32768,982,707,5757,- -32768,589,- -32768,973,
974,5037,- -32768,- -32768,- -32768,3936,622,972,978,- -32768,788,- -32768,- -32768,
787,- -32768,- -32768,- -32768,- -32768,3748,- -32768,- -32768,- -32768,- -32768,- -32768,
977,- -32768,- -32768,716,995,- -32768,- -32768,- -32768,- -32768,- -32768,980,- -32768,
- -32768,- -32768,716,985,- -32768,- -32768,- -32768,986,251,608,1065,855,989,5379,
988,2223,5325,981,- -32768,- 29,- -32768,1020,990,770,821,150,829,270,357,- -32768,-
-32768,- -32768,- -32768,1021,5325,1854,- -32768,- -32768,624,3367,630,4785,3367,653,
4848,4911,657,1009,- -32768,5037,3367,3367,1018,- -32768,- -32768,- -32768,1010,-
-32768,919,- -32768,1012,658,- -32768,- -32768,301,- -32768,- -32768,5757,1095,-
-32768,3654,- -32768,- -32768,1014,- -32768,- -32768,- -32768,- -32768,1015,103,886,
3367,1017,855,2100,- -32768,5757,1016,- -32768,1470,5325,5037,5325,5325,5325,5325,
5325,5325,5325,5325,5325,5325,5325,5325,5325,5325,5325,5325,5325,5037,- -32768,-
-32768,1026,- -32768,3367,3367,666,- -32768,3367,3367,668,3367,708,4974,- -32768,
710,- -32768,- -32768,1011,107,707,- -32768,- -32768,627,5037,1022,1025,- -32768,1037,
5757,- -32768,- -32768,- -32768,- -32768,1030,1027,1031,5325,855,- -32768,467,312,433,
479,479,774,115,115,793,793,793,793,167,167,321,321,- -32768,- -32768,- -32768,324,
- -32768,- -32768,- -32768,3367,- -32768,- -32768,3367,- -32768,3367,3367,718,- -32768,
1046,- -32768,- -32768,- -32768,801,1032,5757,1033,727,- -32768,928,539,- -32768,-
-32768,5037,1470,- -32768,- -32768,- -32768,- -32768,3367,3367,- -32768,- -32768,1034,
- -32768,- -32768,1036,1040,- -32768,855,- -32768,- -32768,- -32768,1043,- -32768,-
-32768,- -32768,- -32768,1141,1154,- -32768};static short Cyc_yypgoto[142]={- -32768,
197,- -32768,254,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6,- 91,19,
- -32768,- -32768,0,581,- -32768,101,- 185,1051,30,- -32768,- -32768,- 144,- -32768,- 115,
- -32768,465,- -32768,- -32768,- -32768,923,920,- 78,457,- -32768,- -32768,571,- -32768,
- -32768,- -32768,64,- -32768,- -32768,- 10,- 175,1099,- 242,363,- -32768,- -32768,1121,-
85,- -32768,- -32768,536,- 101,- 113,- 93,81,273,544,549,- 388,- 431,- 103,- 371,- 111,-
-32768,- 183,- 137,- 531,- 337,- -32768,671,- 133,466,- 80,- 149,- 344,181,- -32768,-
-32768,- -32768,43,- 262,- -32768,- 396,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,473,983,- -32768,510,- -32768,- -32768,- 67,574,- -32768,- 153,
- 315,- 134,- 327,- 318,- 333,843,- 319,- 326,- 159,- 324,- 323,- 15,583,477,- -32768,-
-32768,- 320,- -32768,280,359,- 59,1119,- 330,3};static short Cyc_yytable[5975]={55,
522,127,229,519,461,462,308,464,301,573,525,526,247,662,521,179,524,250,531,532,
55,533,534,311,387,288,312,228,241,58,104,295,539,404,331,285,542,323,622,74,103,
551,120,286,586,541,246,55,568,858,58,318,410,55,55,55,426,55,228,512,369,396,
394,55,140,296,396,568,668,133,329,156,121,122,123,332,124,58,413,456,660,398,
135,58,58,58,625,58,370,379,859,157,225,58,383,480,619,55,55,457,56,419,458,324,
72,586,136,773,774,405,397,55,55,55,55,397,398,590,398,55,94,56,745,220,281,133,
669,58,58,221,385,55,390,55,610,95,378,268,135,279,140,58,58,58,58,96,489,55,56,
58,269,97,135,488,56,56,56,326,56,327,328,58,242,58,56,246,469,639,415,585,471,
494,329,495,819,330,280,58,362,527,528,529,530,640,416,511,415,417,339,340,34,
452,396,478,820,585,156,37,56,56,387,387,387,98,35,482,41,- 225,- 143,386,- 225,470,
56,56,56,56,157,400,401,99,56,403,35,866,867,205,402,341,281,206,35,34,56,579,56,
248,342,324,37,388,55,397,398,293,111,505,279,442,56,509,443,324,303,644,466,545,
579,329,463,135,498,499,868,823,520,304,55,55,100,55,58,869,268,268,34,268,398,
695,701,489,36,37,38,429,471,269,269,345,269,346,432,101,481,485,217,218,58,58,
486,58,543,146,91,595,737,467,732,40,234,235,236,237,228,137,229,815,284,138,643,
102,139,55,- 203,594,596,105,- 203,268,35,551,228,535,536,537,106,34,382,228,35,
228,269,56,37,34,297,489,621,75,107,580,37,58,415,391,146,826,383,91,70,110,392,
471,44,45,46,148,91,56,56,149,56,617,415,597,598,153,651,601,415,605,76,92,850,
93,199,386,386,386,329,228,872,615,873,391,572,658,454,630,228,70,392,455,686,
363,364,365,70,329,578,200,584,70,737,686,902,388,388,388,329,126,34,55,56,55,70,
964,36,37,38,142,329,589,125,152,446,91,92,965,347,489,453,112,113,366,383,92,55,
348,349,367,368,58,604,58,912,202,70,70,736,203,917,70,55,661,224,135,239,675,
133,690,70,70,70,70,874,676,58,204,91,688,673,135,226,875,876,91,55,689,284,677,
228,477,58,70,238,90,298,655,299,385,737,691,228,300,747,618,70,329,625,471,514,
152,741,631,92,742,58,307,580,963,220,731,56,313,56,721,221,921,922,415,918,329,
415,726,646,708,710,924,925,549,879,920,329,923,333,930,931,56,932,933,155,607,
329,833,329,650,334,92,628,201,214,215,216,56,92,219,736,572,981,314,329,315,232,
699,393,34,680,329,316,738,700,772,37,987,584,40,414,56,754,317,412,41,758,329,
916,376,705,70,34,325,44,45,46,335,414,37,769,440,438,116,444,336,711,377,41,329,
55,246,587,371,588,381,279,44,45,46,329,70,302,719,978,220,977,760,55,135,761,
221,806,808,616,811,408,814,671,34,58,420,228,855,34,36,37,38,736,91,152,37,91,
720,631,409,418,748,58,832,41,421,392,127,44,45,46,840,696,44,45,46,423,757,233,
34,809,634,843,329,40,36,37,38,114,115,569,768,827,812,570,34,329,571,430,903,
980,36,37,38,926,927,928,929,439,847,292,432,848,179,460,56,913,34,914,441,44,45,
46,36,37,38,329,445,329,834,898,880,92,56,329,92,448,882,306,884,309,309,888,890,
44,45,46,893,320,321,447,393,449,309,756,309,309,329,891,900,886,329,901,129,130,
414,856,766,414,412,329,450,329,941,309,944,143,144,70,34,70,471,324,459,878,36,
37,38,414,919,716,465,438,474,414,409,16,17,18,822,55,958,392,40,825,240,268,937,
979,581,467,428,329,582,329,946,583,949,475,269,974,948,329,483,91,970,228,487,
91,730,58,416,954,479,976,681,682,683,40,127,560,561,562,70,557,34,548,337,338,
552,398,36,37,38,55,491,934,935,936,492,268,432,343,344,331,574,433,641,642,493,
44,45,46,269,702,703,513,778,749,750,309,428,510,805,58,506,507,508,773,774,837,
838,862,863,864,865,515,962,870,871,518,56,544,92,972,329,546,92,908,909,547,309,
556,309,309,309,309,309,309,309,309,309,309,309,309,309,309,309,309,309,558,559,
538,575,414,577,591,515,438,576,592,593,602,629,91,414,599,600,414,438,606,697,
438,608,613,56,611,614,620,623,77,626,55,567,636,637,638,647,268,648,649,652,653,
713,657,656,952,666,659,91,35,269,667,698,670,881,672,674,885,515,684,70,58,712,
687,706,894,895,715,714,78,350,351,352,353,354,355,356,357,358,359,718,550,309,
717,154,80,664,81,82,284,665,37,92,723,725,515,910,729,360,654,83,733,84,85,739,
743,498,499,744,86,734,753,34,755,759,762,763,87,36,37,38,764,40,765,767,770,779,
92,432,939,940,803,56,942,943,780,945,44,45,46,804,816,428,818,817,821,824,835,
309,830,831,836,722,839,126,841,842,844,845,849,851,857,309,854,860,877,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,861,892,896,897,899,428,904,906,966,911,907,967,
915,968,969,22,938,957,955,950,956,846,959,960,961,971,552,990,973,975,984,679,
985,27,28,29,986,982,983,988,991,953,227,693,685,32,33,771,425,694,431,141,109,
728,781,782,951,727,724,515,645,801,309,752,523,802,128,77,282,0,0,34,0,0,0,380,
35,36,37,38,39,40,284,0,0,0,0,409,- 203,42,43,0,- 203,0,392,0,44,45,46,0,0,0,428,0,
0,0,0,78,0,740,0,0,0,0,428,0,0,746,0,309,0,0,154,80,0,81,82,309,0,37,0,0,0,0,0,0,
0,0,783,0,84,85,0,0,0,0,126,86,0,0,174,0,784,0,0,87,0,0,177,178,0,0,0,0,2,3,4,73,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,0,0,0,0,0,0,0,0,0,428,22,
77,23,160,0,0,0,161,24,0,0,0,428,0,162,163,164,165,27,28,29,0,166,0,0,167,0,0,0,
0,32,33,0,168,169,0,0,0,0,77,0,0,0,0,0,0,0,0,0,0,0,0,853,0,0,309,34,80,170,81,82,
35,36,37,38,39,40,603,0,0,0,0,171,309,42,231,0,0,0,78,0,173,0,0,174,0,175,0,0,
176,0,751,177,178,154,80,0,81,82,0,428,37,0,0,0,0,0,0,0,0,83,0,84,85,0,0,498,499,
0,86,0,0,0,0,0,0,0,87,0,309,0,309,309,309,309,309,309,309,309,309,309,309,309,
309,309,309,309,309,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,702,703,21,158,159,251,309,252,253,254,255,256,257,258,259,22,77,
23,160,260,0,0,161,24,261,0,0,0,0,162,163,164,165,27,28,29,262,166,0,263,167,0,0,
0,0,32,33,0,168,169,0,0,0,0,0,309,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,80,170,81,82,
35,36,37,38,39,40,265,132,- 295,0,0,171,0,42,231,0,0,0,0,0,173,0,0,174,0,175,0,0,
176,0,0,177,178,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,
251,0,252,253,254,255,256,257,258,259,22,77,23,160,260,0,0,161,24,261,0,0,- 139,-
139,162,163,164,165,27,28,29,262,166,0,263,167,0,0,0,0,32,33,0,168,169,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,80,170,81,82,35,36,37,38,39,40,265,132,0,0,0,
171,0,42,231,0,0,0,0,0,173,0,0,174,0,175,0,0,176,0,0,177,178,2,3,4,73,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,251,0,252,253,254,255,256,257,258,
259,22,77,23,160,260,0,0,161,24,261,0,0,0,0,162,163,164,165,27,28,29,262,166,0,
263,167,0,0,0,0,32,33,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,80,
170,81,82,35,36,37,38,39,40,265,132,266,0,0,171,0,42,231,0,0,0,0,0,173,0,0,174,0,
175,0,0,176,0,0,177,178,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
158,159,251,0,252,253,254,255,256,257,258,259,22,77,23,160,260,0,0,161,24,261,0,
0,0,0,162,163,164,165,27,28,29,262,166,0,263,167,0,0,0,0,32,33,0,168,169,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,80,170,81,82,35,36,37,38,39,40,265,132,0,0,
0,171,0,42,231,0,0,0,0,0,173,0,0,174,0,175,0,0,176,0,0,177,178,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,27,28,29,0,166,207,0,167,0,0,0,0,32,33,0,168,169,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,35,36,37,38,39,40,0,208,0,0,0,
171,0,42,231,0,0,0,0,0,173,0,0,174,0,175,0,0,176,0,0,177,178,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,27,28,29,0,166,0,0,167,0,0,0,0,32,33,0,168,169,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,35,36,37,38,39,40,0,322,0,0,0,
171,0,42,231,0,0,0,0,0,173,0,0,174,0,175,0,0,176,0,0,177,178,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,27,28,29,0,166,0,0,167,0,0,0,0,32,33,0,168,169,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,35,36,37,38,39,40,0,0,0,0,0,171,
0,42,231,0,0,0,0,0,173,0,0,174,0,175,0,0,176,0,0,177,178,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
633,0,24,0,25,26,- 139,- 139,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,
41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,
0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,
1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,
32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,
40,0,0,0,0,0,41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,
- 139,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,77,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,42,
43,0,78,0,0,0,44,45,46,0,27,28,29,0,372,0,0,154,80,0,81,82,32,33,37,0,0,0,0,0,0,
0,0,83,0,84,85,0,0,0,0,0,86,0,0,282,0,0,0,0,87,0,0,35,36,0,38,39,40,284,0,0,0,0,
391,- 203,42,43,0,- 203,0,392,77,44,45,46,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,78,0,0,0,0,- 139,- 139,0,0,0,0,27,28,29,
0,79,80,0,81,82,0,0,37,32,33,0,0,0,0,0,0,83,0,84,85,0,0,0,0,0,86,0,0,0,0,0,0,0,
87,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,432,0,42,43,0,0,0,0,0,44,45,46,2,3,4,
73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,0,24,0,0,0,- 139,- 139,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,2,
3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,35,36,0,38,39,40,
0,132,22,0,240,0,0,42,43,0,24,0,0,0,- 139,- 139,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,
32,33,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,35,
36,0,38,39,40,0,22,0,23,0,0,0,42,43,24,0,0,0,0,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,
0,0,32,33,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,
35,36,22,38,39,40,0,132,0,0,0,0,0,42,43,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,77,32,
33,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,282,0,0,283,0,0,0,0,35,
36,22,38,39,40,284,0,78,0,0,0,- 203,42,43,0,- 203,0,0,0,27,28,29,154,80,0,81,82,0,
0,37,0,32,33,0,0,0,0,0,83,0,84,85,0,0,0,0,0,86,0,0,0,0,0,0,282,87,0,0,0,0,0,0,35,
36,0,38,39,40,284,0,0,0,0,0,- 203,42,43,0,- 203,158,159,251,77,252,253,254,255,256,
257,258,259,0,77,0,160,260,0,0,161,0,261,0,0,0,0,162,163,164,165,0,0,0,262,166,0,
263,167,0,0,0,0,0,78,0,168,169,0,0,0,0,0,0,0,0,663,0,0,154,80,0,81,82,0,0,37,0,0,
264,80,170,81,82,0,83,37,84,85,0,265,132,0,0,86,171,0,0,172,0,0,0,87,0,173,0,0,
174,0,175,158,159,176,0,0,177,178,496,0,0,0,0,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,310,497,0,0,171,0,0,172,0,0,
498,499,0,173,158,159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,0,0,
0,0,162,163,164,165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,310,735,0,0,171,0,0,172,0,
0,498,499,0,173,158,159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,0,
0,0,0,162,163,164,165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,310,905,0,0,171,0,0,172,
0,0,498,499,0,173,158,159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,310,0,0,0,171,0,0,172,
0,0,498,499,0,173,158,159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,0,0,0,0,171,0,0,172,0,
0,0,0,291,173,158,159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,0,0,
0,0,162,163,164,165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,160,0,310,0,161,0,171,0,
0,172,0,162,163,164,165,173,0,0,174,166,175,0,167,176,0,0,177,178,0,0,168,169,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,160,451,
0,0,161,0,171,0,0,172,0,162,163,164,165,173,0,0,174,166,175,0,167,176,0,0,177,
178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,
0,37,77,0,160,0,0,0,161,0,171,540,0,172,0,162,163,164,165,173,0,0,174,166,175,0,
167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
170,81,82,0,0,37,0,0,0,0,0,0,0,0,171,0,0,172,0,0,0,0,566,173,158,159,174,0,175,0,
0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,0,0,0,0,166,0,
0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,
170,81,82,0,0,37,77,0,160,0,0,0,161,0,171,609,0,172,0,162,163,164,165,173,0,0,
174,166,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,0,0,0,0,171,0,0,172,0,0,0,0,678,173,158,
159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,
165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,170,81,82,0,0,37,0,0,0,0,0,0,0,0,171,0,0,172,0,0,0,0,692,173,158,
159,174,0,175,0,0,176,0,0,177,178,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,
165,0,0,0,0,166,0,0,167,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,160,707,0,0,161,0,171,0,0,172,0,162,
163,164,165,173,0,0,174,166,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,160,709,0,0,161,0,
171,0,0,172,0,162,163,164,165,173,0,0,174,166,175,0,167,176,0,0,177,178,0,0,168,
169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,
160,0,0,0,161,0,171,807,0,172,0,162,163,164,165,173,0,0,174,166,175,0,167,176,0,
0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,
82,0,0,37,77,0,160,0,0,0,161,0,171,810,0,172,0,162,163,164,165,173,0,0,174,166,
175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,
0,0,34,80,170,81,82,0,0,37,77,0,160,813,0,0,161,0,171,0,0,172,0,162,163,164,165,
173,0,0,174,166,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,160,0,0,0,161,0,171,883,0,172,0,
162,163,164,165,173,0,0,174,166,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,0,160,0,0,0,161,0,
171,887,0,172,0,162,163,164,165,173,0,0,174,166,175,0,167,176,0,0,177,178,0,0,
168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,81,82,0,0,37,77,
0,160,0,0,0,161,0,171,889,0,172,0,162,163,164,165,173,0,0,174,166,175,0,167,176,
0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,80,170,
81,82,0,0,37,77,0,160,0,0,0,161,0,171,947,0,172,0,162,163,164,165,173,0,0,174,
166,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,
0,0,0,77,34,80,170,81,82,0,0,37,0,0,0,0,162,163,164,165,171,0,0,172,0,0,0,167,0,
173,0,0,174,0,175,168,169,176,0,0,177,178,0,0,0,0,0,0,0,0,0,0,0,158,159,0,0,0,34,
80,170,81,82,0,0,37,77,0,0,0,310,0,0,0,171,0,0,172,0,162,163,164,165,173,0,0,174,
0,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,159,
0,0,0,34,80,170,81,82,0,0,37,77,0,0,0,654,0,0,0,171,0,0,172,0,162,163,164,165,
173,0,0,174,0,175,0,167,176,0,0,177,178,0,0,168,169,0,0,0,0,0,0,0,158,159,0,0,0,
0,0,0,0,0,0,0,0,77,34,80,170,81,82,0,0,37,0,0,0,0,162,163,164,165,305,0,0,172,0,
0,0,167,0,173,0,0,174,0,175,168,169,176,0,0,177,178,0,0,158,159,0,0,0,0,0,0,0,0,
0,0,0,77,34,80,170,81,82,0,0,37,0,0,0,0,162,163,164,165,319,0,0,172,0,0,0,167,0,
173,0,0,174,0,175,168,169,176,0,0,177,178,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,
34,80,170,81,82,0,0,37,0,0,0,0,162,163,164,165,171,0,0,172,0,0,0,167,0,173,0,0,
174,0,175,168,169,176,0,0,177,178,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,170,81,
82,0,0,37,0,0,0,0,0,0,0,0,852,0,0,172,0,0,0,0,0,173,0,0,174,0,175,0,0,176,0,0,
177,178,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,27,28,29,0,0,207,0,0,0,- 139,- 139,
0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,0,
38,39,40,0,208,0,0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,
0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,0,38,39,40,0,0,- 107,0,0,0,0,42,43,
35,36,0,38,39,40,0,0,- 108,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,27,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,627,
0,0,0,0,0,0,0,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,
0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,0,0,0,0,0,0,0,0,0,0,- 139,- 139,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,0,39,40,0,0,0,0,0,0,0,
42,43};static short Cyc_yycheck[5975]={0,334,61,106,331,267,268,160,270,153,398,
337,338,128,545,333,83,336,131,343,344,21,345,346,161,210,137,161,106,120,0,41,
145,363,17,79,137,367,171,470,21,41,379,53,137,416,366,125,48,393,79,21,167,228,
54,55,56,240,58,137,322,91,73,212,64,65,146,73,412,110,64,107,93,54,55,56,120,58,
48,228,93,117,123,64,54,55,56,475,58,119,205,120,113,103,64,208,107,468,98,99,
113,0,106,116,171,95,477,64,5,6,93,122,112,113,114,115,122,123,433,123,120,93,21,
654,93,135,120,558,98,99,99,209,132,211,134,455,113,204,132,120,134,141,112,113,
114,115,113,300,148,48,120,132,113,134,298,54,55,56,173,58,175,176,132,120,134,
64,244,60,498,228,412,284,305,107,307,93,110,134,148,194,339,340,341,342,499,107,
319,246,110,74,75,93,259,73,287,113,438,93,100,98,99,386,387,388,104,98,291,109,
104,106,210,107,105,112,113,114,115,113,215,216,104,120,219,98,74,75,105,107,113,
239,109,98,93,132,409,134,111,122,305,100,210,241,122,123,111,48,313,241,109,148,
317,112,319,98,108,104,371,432,107,269,241,115,116,113,110,332,111,267,268,113,
270,241,122,267,268,93,270,123,593,599,433,99,100,101,241,398,267,268,121,270,
123,109,92,290,294,98,99,267,268,296,270,368,113,23,115,642,93,637,103,112,113,
114,115,391,109,418,712,104,113,502,109,116,322,110,439,440,92,114,322,98,667,
409,347,348,349,109,93,106,416,98,418,322,241,100,93,148,499,106,105,105,409,100,
322,412,109,113,727,470,78,0,121,116,475,118,119,120,105,87,267,268,95,270,463,
432,441,442,105,510,445,438,447,22,23,779,25,109,386,387,388,107,468,121,459,123,
109,395,114,104,483,477,41,116,109,578,69,70,71,48,107,409,93,411,53,750,589,114,
386,387,388,107,61,93,422,322,424,66,114,99,100,101,67,107,432,105,75,254,156,78,
114,118,593,260,104,105,109,558,87,447,127,128,115,116,422,447,424,851,104,98,99,
642,107,857,103,463,544,109,447,107,569,463,581,112,113,114,115,118,569,447,108,
199,581,566,463,110,127,128,206,487,581,104,570,569,107,463,135,104,23,107,513,
106,578,838,582,581,108,658,463,148,107,897,623,110,153,650,484,156,650,487,109,
578,916,93,94,422,109,424,619,99,861,862,589,858,107,592,627,110,603,604,864,865,
104,803,860,107,863,78,870,871,447,872,873,78,104,107,737,107,110,124,199,478,87,
95,96,97,463,206,100,750,568,965,109,107,109,107,110,212,93,574,107,109,647,110,
697,100,980,585,103,228,487,674,109,228,109,678,107,857,93,110,239,93,109,118,
119,120,125,246,100,692,249,246,104,252,126,104,93,109,107,617,696,422,105,424,
109,617,118,119,120,107,269,156,110,961,93,94,107,635,617,110,99,706,707,460,709,
110,711,564,93,617,106,727,783,93,99,100,101,838,376,298,100,379,617,631,109,109,
661,635,733,109,106,116,729,118,119,120,759,594,118,119,120,106,677,107,93,104,
487,770,107,103,99,100,101,104,105,109,691,106,104,113,93,107,116,106,835,965,99,
100,101,866,867,868,869,105,104,139,109,107,783,114,617,852,93,854,109,118,119,
120,99,100,101,107,109,107,110,826,110,376,635,107,379,93,110,158,809,160,161,
812,813,118,119,120,817,168,169,109,391,104,173,676,175,176,107,104,104,110,107,
107,49,50,409,784,689,412,409,107,104,107,110,194,110,63,64,422,93,424,897,852,
109,802,99,100,101,432,859,612,106,432,110,438,109,17,18,19,721,803,907,116,103,
726,108,803,877,964,109,93,240,107,113,107,110,116,110,114,803,956,891,107,117,
547,110,907,106,551,635,803,107,902,110,110,93,94,95,103,901,386,387,388,487,381,
93,376,76,77,379,123,99,100,101,857,106,874,875,876,122,857,109,72,73,79,402,114,
106,107,122,118,119,120,857,22,23,110,699,106,107,300,310,109,705,857,314,315,
316,5,6,106,107,125,126,76,77,325,915,72,73,329,803,113,547,106,107,110,551,847,
848,107,331,110,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,
349,107,106,361,109,578,107,104,366,578,110,107,114,30,480,667,589,109,105,592,
589,104,595,592,42,98,857,108,93,109,123,38,110,965,392,110,107,93,107,965,107,
107,110,106,104,107,110,898,106,110,702,98,965,107,105,110,807,110,117,810,418,
110,635,965,105,110,109,818,819,93,110,78,80,81,82,83,84,85,86,87,88,89,117,90,
433,122,93,94,547,96,97,104,551,100,667,106,122,455,849,105,108,105,109,113,111,
112,25,110,115,116,110,118,117,110,93,110,117,110,110,126,99,100,101,110,103,110,
110,117,105,702,109,882,883,114,965,886,887,110,889,118,119,120,106,104,502,93,
109,104,98,109,499,110,110,107,620,110,729,94,110,106,106,28,105,114,513,109,78,
78,8,9,10,11,12,13,14,15,16,17,18,19,20,21,124,106,98,107,106,545,25,107,941,106,
109,944,110,946,947,37,104,94,110,122,109,776,106,110,107,93,667,0,110,110,110,
571,110,55,56,57,110,970,971,110,0,901,105,583,577,67,68,696,239,592,244,66,45,
631,25,26,897,627,623,599,503,702,593,667,335,702,61,38,90,- 1,- 1,93,- 1,- 1,- 1,206,
98,99,100,101,102,103,104,- 1,- 1,- 1,- 1,109,110,111,112,- 1,114,- 1,116,- 1,118,119,
120,- 1,- 1,- 1,642,- 1,- 1,- 1,- 1,78,- 1,649,- 1,- 1,- 1,- 1,654,- 1,- 1,657,- 1,650,- 1,- 1,93,
94,- 1,96,97,658,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,901,
118,- 1,- 1,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,737,37,38,
39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,750,- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,-
1,- 1,67,68,- 1,70,71,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,781,- 1,- 1,
784,93,94,95,96,97,98,99,100,101,102,103,104,- 1,- 1,- 1,- 1,109,802,111,112,- 1,- 1,-
1,78,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,126,- 1,90,129,130,93,94,- 1,96,97,- 1,838,100,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
126,- 1,858,- 1,860,861,862,863,864,865,866,867,868,869,870,871,872,873,874,875,
876,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,915,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,
44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,- 1,70,
71,- 1,- 1,- 1,- 1,- 1,964,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,98,99,100,101,102,103,104,105,106,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,
- 1,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,
46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,104,105,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,
123,- 1,- 1,126,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,
- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,
103,104,105,106,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,
126,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,
53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,104,
105,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,126,- 1,- 1,
129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,59,
60,- 1,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,103,- 1,105,- 1,- 1,- 1,109,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
67,68,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,
94,95,96,97,98,99,100,101,102,103,- 1,105,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,
118,- 1,- 1,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,
100,101,102,103,- 1,- 1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,
123,- 1,- 1,126,- 1,- 1,129,130,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,
47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,
100,101,102,103,- 1,- 1,106,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,106,- 1,- 1,109,- 1,
111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,
45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,
99,100,101,102,103,- 1,- 1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,- 1,- 1,38,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,37,- 1,106,- 1,- 1,109,- 1,
111,112,- 1,78,- 1,- 1,- 1,118,119,120,- 1,55,56,57,- 1,90,- 1,- 1,93,94,- 1,96,97,67,68,
100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,90,- 1,- 1,- 1,
- 1,126,- 1,- 1,98,99,- 1,101,102,103,104,- 1,- 1,- 1,- 1,109,110,111,112,- 1,114,- 1,116,
38,118,119,120,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,
57,- 1,93,94,- 1,96,97,- 1,- 1,100,67,68,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,-
1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,93,- 1,- 1,- 1,- 1,98,99,100,101,102,103,- 1,- 1,- 1,
- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,119,120,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,
- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,
- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,98,
99,- 1,101,102,103,- 1,105,37,- 1,108,- 1,- 1,111,112,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,-
1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,37,- 1,39,- 1,
- 1,- 1,111,112,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
67,68,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,98,99,37,101,102,103,- 1,105,- 1,- 1,- 1,- 1,- 1,111,112,- 1,- 1,- 1,- 1,- 1,55,
56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,67,68,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,93,- 1,- 1,- 1,- 1,98,99,37,101,102,103,104,- 1,78,- 1,-
1,- 1,110,111,112,- 1,114,- 1,- 1,- 1,55,56,57,93,94,- 1,96,97,- 1,- 1,100,- 1,67,68,- 1,-
1,- 1,- 1,- 1,109,- 1,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,90,126,- 1,- 1,- 1,-
1,- 1,- 1,98,99,- 1,101,102,103,104,- 1,- 1,- 1,- 1,- 1,110,111,112,- 1,114,25,26,27,38,
29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,
- 1,- 1,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,78,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,
93,94,- 1,96,97,- 1,- 1,100,- 1,- 1,93,94,95,96,97,- 1,109,100,111,112,- 1,104,105,- 1,-
1,118,109,- 1,- 1,112,- 1,- 1,- 1,126,- 1,118,- 1,- 1,121,- 1,123,25,26,126,- 1,- 1,129,130,
32,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,- 1,109,- 1,-
1,112,- 1,- 1,115,116,- 1,118,25,26,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,
- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,
95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,106,- 1,- 1,109,- 1,- 1,112,- 1,- 1,115,116,- 1,118,
25,26,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,-
1,- 1,105,106,- 1,- 1,109,- 1,- 1,112,- 1,- 1,115,116,- 1,118,25,26,121,- 1,123,- 1,- 1,126,
- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,109,- 1,- 1,
112,- 1,- 1,115,116,- 1,118,25,26,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,
96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,117,118,25,26,
121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,
105,- 1,44,- 1,109,- 1,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,
129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,104,- 1,- 1,44,- 1,109,- 1,- 1,112,- 1,51,52,53,
54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,-
1,- 1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,
129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,
117,118,25,26,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,-
1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,
100,38,- 1,40,- 1,- 1,- 1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,-
1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,
- 1,- 1,- 1,- 1,117,118,25,26,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,-
1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,
97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,117,118,25,26,121,
- 1,123,- 1,- 1,126,- 1,- 1,129,130,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,104,- 1,
- 1,44,- 1,109,- 1,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,
130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,38,- 1,40,104,- 1,- 1,44,- 1,109,- 1,- 1,112,- 1,51,52,53,54,
118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,- 1,-
1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,
130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,- 1,- 1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,
118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,104,- 1,
- 1,44,- 1,109,- 1,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,
130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,- 1,- 1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,
118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,- 1,-
1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,
130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,- 1,- 1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,
118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,40,- 1,- 1,-
1,44,- 1,109,110,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,59,123,- 1,62,126,- 1,- 1,129,
130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,
93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,51,52,53,54,109,- 1,- 1,112,- 1,- 1,- 1,62,- 1,
118,- 1,- 1,121,- 1,123,70,71,126,- 1,- 1,129,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,- 1,- 1,- 1,105,- 1,- 1,- 1,109,- 1,- 1,112,- 1,
51,52,53,54,118,- 1,- 1,121,- 1,123,- 1,62,126,- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,38,-
1,- 1,- 1,105,- 1,- 1,- 1,109,- 1,- 1,112,- 1,51,52,53,54,118,- 1,- 1,121,- 1,123,- 1,62,126,
- 1,- 1,129,130,- 1,- 1,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,38,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,51,52,53,54,109,- 1,- 1,112,- 1,- 1,- 1,
62,- 1,118,- 1,- 1,121,- 1,123,70,71,126,- 1,- 1,129,130,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,38,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,51,52,53,54,109,- 1,- 1,112,
- 1,- 1,- 1,62,- 1,118,- 1,- 1,121,- 1,123,70,71,126,- 1,- 1,129,130,- 1,- 1,25,26,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,51,52,53,54,109,
- 1,- 1,112,- 1,- 1,- 1,62,- 1,118,- 1,- 1,121,- 1,123,70,71,126,- 1,- 1,129,130,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,126,- 1,- 1,129,130,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,55,56,57,- 1,- 1,60,- 1,- 1,
- 1,49,50,- 1,67,68,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,105,- 1,- 1,- 1,- 1,- 1,111,112,98,99,
- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,37,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,55,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,
101,102,103,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,98,99,- 1,101,102,103,- 1,- 1,106,- 1,- 1,-
1,- 1,111,112,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,55,56,57,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,
- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,112,98,99,- 1,101,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,98,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dynforward_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(
int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){
57});static int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple19{struct _dynforward_ptr f1;void*f2;};static char
_tmp429[8]="stdcall";static char _tmp42A[6]="cdecl";static char _tmp42B[9]="fastcall";
static char _tmp42C[9]="noreturn";static char _tmp42D[6]="const";static char _tmp42E[8]="aligned";
static char _tmp42F[7]="packed";static char _tmp430[7]="shared";static char _tmp431[7]="unused";
static char _tmp432[5]="weak";static char _tmp433[10]="dllimport";static char _tmp434[
10]="dllexport";static char _tmp435[23]="no_instrument_function";static char _tmp436[
12]="constructor";static char _tmp437[11]="destructor";static char _tmp438[22]="no_check_memory_usage";
static char _tmp439[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp389=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp389;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8DE=10000;short*_tmp8DF=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8DE));{unsigned int
_tmp8E0=_tmp8DE;unsigned int i;for(i=0;i < _tmp8E0;i ++){_tmp8DF[i]=(short)0;}}
_tmp8DF;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8DB=10000;union Cyc_YYSTYPE_union*_tmp8DC=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8DB));{
unsigned int _tmp8DD=_tmp8DB;unsigned int i;for(i=0;i < _tmp8DD;i ++){_tmp8DC[i]=Cyc_yylval;}}
_tmp8DC;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8D8=10000;struct Cyc_Yyltype*_tmp8D9=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8D8));{
unsigned int _tmp8DA=_tmp8D8;unsigned int i;for(i=0;i < _tmp8DA;i ++){_tmp8D9[i]=Cyc_yynewloc();}}
_tmp8D9;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp38A="parser stack overflow";_tag_dynforward(
_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(992,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 358?(int)Cyc_yytranslate[_check_known_subscript_notnull(359,Cyc_yychar)]:
273;}yyn +=yychar1;if((yyn < 0  || yyn > 5974) || Cyc_yycheck[
_check_known_subscript_notnull(5975,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5975,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 991){int
_tmp38B=0;_npop_handler(0);return _tmp38B;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)yyerrstatus --;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(992,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(499,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E7: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp38C;_push_handler(& _tmp38C);{int _tmp38E=0;if(setjmp(_tmp38C.handler))_tmp38E=
1;if(!_tmp38E){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp38D=(void*)_exn_thrown;void*
_tmp390=_tmp38D;_LL1EA: if(*((void**)_tmp390)!= Cyc_Core_Failure)goto _LL1EC;
_LL1EB: x=0;goto _LL1E9;_LL1EC:;_LL1ED:(void)_throw(_tmp390);_LL1E9:;}}}{struct
_handler_cons _tmp391;_push_handler(& _tmp391);{int _tmp393=0;if(setjmp(_tmp391.handler))
_tmp393=1;if(!_tmp393){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp392=(void*)_exn_thrown;void*
_tmp395=_tmp392;_LL1EF: if(*((void**)_tmp395)!= Cyc_Core_Failure)goto _LL1F1;
_LL1F0: y=0;goto _LL1EE;_LL1F1:;_LL1F2:(void)_throw(_tmp395);_LL1EE:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp396;(_tmp396.YY18).tag=22;(
_tmp396.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp396;});break;}case 3: _LL1E8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp397;(_tmp397.YY18).tag=22;(_tmp397.YY18).f1=({struct
Cyc_List_List*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->hd=({struct Cyc_Absyn_Decl*
_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_Absyn_Using_d_struct
_tmp39B;_tmp39B.tag=9;_tmp39B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39B.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39B;});_tmp39A;}));
_tmp399->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp399;});_tmp398->tl=0;_tmp398;});_tmp397;});Cyc_Lex_leave_using();
break;case 4: _LL1F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp39C;(
_tmp39C.YY18).tag=22;(_tmp39C.YY18).f1=({struct Cyc_List_List*_tmp39D=_cycalloc(
sizeof(*_tmp39D));_tmp39D->hd=({struct Cyc_Absyn_Decl*_tmp39E=_cycalloc(sizeof(*
_tmp39E));_tmp39E->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp39F=
_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=({struct Cyc_Absyn_Using_d_struct _tmp3A0;
_tmp3A0.tag=9;_tmp3A0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp3A0.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A0;});_tmp39F;}));
_tmp39E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp39E;});_tmp39D->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39D;});_tmp39C;});
break;case 5: _LL1F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A1;(
_tmp3A1.YY18).tag=22;(_tmp3A1.YY18).f1=({struct Cyc_List_List*_tmp3A2=_cycalloc(
sizeof(*_tmp3A2));_tmp3A2->hd=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*
_tmp3A3));_tmp3A3->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A4=
_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A5;_tmp3A5.tag=8;_tmp3A5.f1=({struct _dynforward_ptr*_tmp3A6=_cycalloc(
sizeof(*_tmp3A6));_tmp3A6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A6;});_tmp3A5.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A5;});
_tmp3A4;}));_tmp3A3->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3A3;});
_tmp3A2->tl=0;_tmp3A2;});_tmp3A1;});Cyc_Lex_leave_namespace();break;case 6: _LL1F5: {
struct _dynforward_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3A7;_push_handler(& _tmp3A7);{int _tmp3A9=0;if(setjmp(_tmp3A7.handler))
_tmp3A9=1;if(!_tmp3A9){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3A8=(void*)_exn_thrown;void*_tmp3AB=_tmp3A8;_LL1F8: if(*((void**)_tmp3AB)
!= Cyc_Core_Failure)goto _LL1FA;_LL1F9: nspace=({const char*_tmp3AC="";
_tag_dynforward(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,1));});goto
_LL1F7;_LL1FA:;_LL1FB:(void)_throw(_tmp3AB);_LL1F7:;}}}{struct _handler_cons
_tmp3AD;_push_handler(& _tmp3AD);{int _tmp3AF=0;if(setjmp(_tmp3AD.handler))_tmp3AF=
1;if(!_tmp3AF){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp3AE=(void*)_exn_thrown;void*
_tmp3B1=_tmp3AE;_LL1FD: if(*((void**)_tmp3B1)!= Cyc_Core_Failure)goto _LL1FF;
_LL1FE: x=0;goto _LL1FC;_LL1FF:;_LL200:(void)_throw(_tmp3B1);_LL1FC:;}}}{struct
_handler_cons _tmp3B2;_push_handler(& _tmp3B2);{int _tmp3B4=0;if(setjmp(_tmp3B2.handler))
_tmp3B4=1;if(!_tmp3B4){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp3B3=(void*)_exn_thrown;void*
_tmp3B6=_tmp3B3;_LL202: if(*((void**)_tmp3B6)!= Cyc_Core_Failure)goto _LL204;
_LL203: y=0;goto _LL201;_LL204:;_LL205:(void)_throw(_tmp3B6);_LL201:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3B7;(_tmp3B7.YY18).tag=22;(
_tmp3B7.YY18).f1=({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));
_tmp3B8->hd=({struct Cyc_Absyn_Decl*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->r=(
void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3BA=_cycalloc(sizeof(*
_tmp3BA));_tmp3BA[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp3BB;_tmp3BB.tag=8;
_tmp3BB.f1=({struct _dynforward_ptr*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC[0]=
nspace;_tmp3BC;});_tmp3BB.f2=x;_tmp3BB;});_tmp3BA;}));_tmp3B9->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3B9;});
_tmp3B8->tl=y;_tmp3B8;});_tmp3B7;});break;}case 7: _LL1F6: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),({const char*_tmp3BD="C";_tag_dynforward(_tmp3BD,sizeof(char),
_get_zero_arr_size(_tmp3BD,2));}))== 0)yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3BE;(_tmp3BE.YY18).tag=22;(_tmp3BE.YY18).f1=({struct Cyc_List_List*_tmp3BF=
_cycalloc(sizeof(*_tmp3BF));_tmp3BF->hd=({struct Cyc_Absyn_Decl*_tmp3C0=_cycalloc(
sizeof(*_tmp3C0));_tmp3C0->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3C2;_tmp3C2.tag=10;_tmp3C2.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C2;});_tmp3C1;}));
_tmp3C0->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3C0;});_tmp3BF->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BF;});_tmp3BE;});else{
if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3C3="C include";
_tag_dynforward(_tmp3C3,sizeof(char),_get_zero_arr_size(_tmp3C3,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3C4="expecting \"C\" or \"C include\"";_tag_dynforward(_tmp3C4,
sizeof(char),_get_zero_arr_size(_tmp3C4,29));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3C5;(_tmp3C5.YY18).tag=22;(_tmp3C5.YY18).f1=({
struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=({struct Cyc_Absyn_Decl*
_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3C9;_tmp3C9.tag=11;_tmp3C9.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C9.f2=0;_tmp3C9;});
_tmp3C8;}));_tmp3C7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C7;});
_tmp3C6->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C6;});_tmp3C5;});}break;case 8: _LL206: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3CA="C include";_tag_dynforward(_tmp3CA,
sizeof(char),_get_zero_arr_size(_tmp3CA,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp3CB="expecting \"C include\"";_tag_dynforward(_tmp3CB,sizeof(char),
_get_zero_arr_size(_tmp3CB,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3CC;(_tmp3CC.YY18).tag=
22;(_tmp3CC.YY18).f1=({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));
_tmp3CD->hd=({struct Cyc_Absyn_Decl*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3CF=_cycalloc(
sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3D0;
_tmp3D0.tag=11;_tmp3D0.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3D0.f2=exs;_tmp3D0;});_tmp3CF;}));_tmp3CE->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CE;});
_tmp3CD->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CD;});_tmp3CC;});break;}case 9: _LL207: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D1;(_tmp3D1.YY18).tag=22;(_tmp3D1.YY18).f1=({struct
Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->hd=({struct Cyc_Absyn_Decl*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->r=(void*)((void*)0);_tmp3D3->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3D3;});_tmp3D2->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D2;});_tmp3D1;});
break;case 10: _LL208: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D4;(_tmp3D4.YY18).tag=22;(_tmp3D4.YY18).f1=({struct Cyc_List_List*_tmp3D5=
_cycalloc(sizeof(*_tmp3D5));_tmp3D5->hd=({struct Cyc_Absyn_Decl*_tmp3D6=_cycalloc(
sizeof(*_tmp3D6));_tmp3D6->r=(void*)((void*)1);_tmp3D6->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3D6;});
_tmp3D5->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3D5;});_tmp3D4;});break;case 11: _LL209: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D7;(_tmp3D7.YY18).tag=22;(_tmp3D7.YY18).f1=0;_tmp3D7;});
break;case 12: _LL20A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL20B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D8;(_tmp3D8.YY52).tag=56;(_tmp3D8.YY52).f1=0;_tmp3D8;});break;case 14: _LL20C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D9;(_tmp3D9.YY52).tag=
56;(_tmp3D9.YY52).f1=({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));
_tmp3DA->hd=({struct _tuple18*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3DB->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3DB->f3=0;_tmp3DB;});_tmp3DA->tl=0;_tmp3DA;});_tmp3D9;});break;case 15: _LL20D:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3DC;(_tmp3DC.YY52).tag=
56;(_tmp3DC.YY52).f1=({struct Cyc_List_List*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));
_tmp3DD->hd=({struct _tuple18*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3DE->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3DE->f3=0;_tmp3DE;});_tmp3DD->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3DD;});_tmp3DC;});
break;case 16: _LL20E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DF;(_tmp3DF.YY18).tag=22;(_tmp3DF.YY18).f1=({struct Cyc_List_List*_tmp3E0=
_cycalloc(sizeof(*_tmp3E0));_tmp3E0->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3E2;_tmp3E2.tag=1;_tmp3E2.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E2;});_tmp3E1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E0->tl=0;
_tmp3E0;});_tmp3DF;});break;case 17: _LL20F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL210: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E3;(_tmp3E3.YY18).tag=22;(
_tmp3E3.YY18).f1=0;_tmp3E3;});break;case 19: _LL211: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3E4;(_tmp3E4.YY17).tag=21;(_tmp3E4.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E4;});
break;case 20: _LL212: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E5;(_tmp3E5.YY17).tag=21;(_tmp3E5.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E6;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E5;});
break;case 21: _LL213: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E7;(_tmp3E7.YY17).tag=21;(_tmp3E7.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E7;});
break;case 22: _LL214: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E8;(_tmp3E8.YY17).tag=21;(_tmp3E8.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E9;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E8;});
break;case 23: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EA;(_tmp3EA.YY17).tag=21;(_tmp3EA.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3EB;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EA;});
break;case 24: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EC;(_tmp3EC.YY17).tag=21;(_tmp3EC.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3ED;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EC;});
break;case 25: _LL217: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL218: Cyc_Lex_leave_using();
break;case 27: _LL219: Cyc_Lex_enter_namespace(({struct _dynforward_ptr*_tmp3EE=
_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3EE;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21A: Cyc_Lex_leave_namespace();
break;case 29: _LL21B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EF;(_tmp3EF.YY18).tag=22;(_tmp3EF.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3EF;});
break;case 30: _LL21C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F0;(_tmp3F0.YY18).tag=22;(_tmp3F0.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F0;});
break;case 31: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F1;(_tmp3F1.YY18).tag=22;(_tmp3F1.YY18).f1=({struct Cyc_List_List*_tmp3F2=
_cycalloc(sizeof(*_tmp3F2));_tmp3F2->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F2->tl=0;
_tmp3F2;});_tmp3F1;});break;case 32: _LL21E: {struct Cyc_List_List*_tmp3F3=0;{
struct Cyc_List_List*_tmp3F4=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3F4 != 0;_tmp3F4=_tmp3F4->tl){struct
_dynforward_ptr*_tmp3F5=(struct _dynforward_ptr*)_tmp3F4->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3F9;(_tmp3F9.Rel_n).tag=1;(_tmp3F9.Rel_n).f1=0;
_tmp3F9;});_tmp3F8->f2=_tmp3F5;_tmp3F8;});struct Cyc_Absyn_Vardecl*_tmp3F6=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3F3=({struct Cyc_List_List*_tmp3F7=_cycalloc(
sizeof(*_tmp3F7));_tmp3F7->hd=_tmp3F6;_tmp3F7->tl=_tmp3F3;_tmp3F7;});}}_tmp3F3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F3);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3FA;(_tmp3FA.YY18).tag=22;(
_tmp3FA.YY18).f1=({struct Cyc_List_List*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));
_tmp3FB->hd=Cyc_Absyn_letv_decl(_tmp3F3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3FB->tl=0;
_tmp3FB;});_tmp3FA;});break;}case 33: _LL21F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL220: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3FC;(_tmp3FC.YY18).tag=22;(
_tmp3FC.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp3FC;});break;case 35: _LL221: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3FD;(_tmp3FD.YY19).tag=23;(_tmp3FD.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->sc=({
struct Cyc_Core_Opt*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3FF;});_tmp3FE->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3FE->type_specs=
0;_tmp3FE->is_inline=0;_tmp3FE->attributes=0;_tmp3FE;});_tmp3FD;});break;case 36:
_LL222: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp400[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp401="Only one storage class is allowed in a declaration";_tag_dynforward(
_tmp401,sizeof(char),_get_zero_arr_size(_tmp401,51));}),_tag_dynforward(_tmp400,
sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp402;(_tmp402.YY19).tag=23;(_tmp402.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->sc=({struct Cyc_Core_Opt*_tmp404=
_cycalloc(sizeof(*_tmp404));_tmp404->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp404;});_tmp403->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp403->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs;_tmp403->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp403->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp403;});_tmp402;});break;case 37: _LL223: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp405;(_tmp405.YY19).tag=23;(_tmp405.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->sc=0;_tmp406->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp406->type_specs=({struct Cyc_List_List*_tmp407=
_cycalloc(sizeof(*_tmp407));_tmp407->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp407->tl=0;_tmp407;});
_tmp406->is_inline=0;_tmp406->attributes=0;_tmp406;});_tmp405;});break;case 38:
_LL224: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp408;(_tmp408.YY19).tag=
23;(_tmp408.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp409=_cycalloc(
sizeof(*_tmp409));_tmp409->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp409->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp409->type_specs=({
struct Cyc_List_List*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40A->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp40A;});
_tmp409->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp409->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp409;});
_tmp408;});break;case 39: _LL225: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40B;(_tmp40B.YY19).tag=23;(_tmp40B.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->sc=0;_tmp40C->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40C->type_specs=0;
_tmp40C->is_inline=0;_tmp40C->attributes=0;_tmp40C;});_tmp40B;});break;case 40:
_LL226: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp40D;(_tmp40D.YY19).tag=
23;(_tmp40D.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp40E=_cycalloc(
sizeof(*_tmp40E));_tmp40E->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40E->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp40E->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp40E->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp40E->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp40E;});
_tmp40D;});break;case 41: _LL227: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40F;(_tmp40F.YY19).tag=23;(_tmp40F.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->sc=0;_tmp410->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp410->type_specs=0;_tmp410->is_inline=1;_tmp410->attributes=
0;_tmp410;});_tmp40F;});break;case 42: _LL228: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp411;(_tmp411.YY19).tag=23;(_tmp411.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp412->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp412->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp412->is_inline=1;_tmp412->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp412;});
_tmp411;});break;case 43: _LL229: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp413;(_tmp413.YY19).tag=23;(_tmp413.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->sc=0;_tmp414->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp414->type_specs=0;_tmp414->is_inline=0;_tmp414->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp414;});
_tmp413;});break;case 44: _LL22A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp415;(_tmp415.YY19).tag=23;(_tmp415.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp416->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp416->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp416->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp416->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp416;});
_tmp415;});break;case 45: _LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp417;(_tmp417.YY22).tag=26;(_tmp417.YY22).f1=(void*)((void*)4);_tmp417;});
break;case 46: _LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp418;(_tmp418.YY22).tag=26;(_tmp418.YY22).f1=(void*)((void*)5);_tmp418;});
break;case 47: _LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp419;(_tmp419.YY22).tag=26;(_tmp419.YY22).f1=(void*)((void*)3);_tmp419;});
break;case 48: _LL22E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41A;(_tmp41A.YY22).tag=26;(_tmp41A.YY22).f1=(void*)((void*)1);_tmp41A;});
break;case 49: _LL22F: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp41B="C";
_tag_dynforward(_tmp41B,sizeof(char),_get_zero_arr_size(_tmp41B,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp41C="only extern or extern \"C\" is allowed";_tag_dynforward(
_tmp41C,sizeof(char),_get_zero_arr_size(_tmp41C,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp41D;(_tmp41D.YY22).tag=26;(_tmp41D.YY22).f1=(void*)((
void*)2);_tmp41D;});break;case 50: _LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41E;(_tmp41E.YY22).tag=26;(_tmp41E.YY22).f1=(void*)((void*)0);_tmp41E;});
break;case 51: _LL231: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41F;(_tmp41F.YY22).tag=26;(_tmp41F.YY22).f1=(void*)((void*)6);_tmp41F;});
break;case 52: _LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp420;(_tmp420.YY45).tag=49;(_tmp420.YY45).f1=0;_tmp420;});break;case 53: _LL233:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54:
_LL234: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 55: _LL235: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp421;(
_tmp421.YY45).tag=49;(_tmp421.YY45).f1=({struct Cyc_List_List*_tmp422=_cycalloc(
sizeof(*_tmp422));_tmp422->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp422->tl=0;_tmp422;});
_tmp421;});break;case 56: _LL236: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp423;(_tmp423.YY45).tag=49;(_tmp423.YY45).f1=({struct Cyc_List_List*_tmp424=
_cycalloc(sizeof(*_tmp424));_tmp424->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp424->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp424;});_tmp423;});
break;case 57: _LL237: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp429,_tmp429 + 8},(void*)0},{{_tmp42A,
_tmp42A + 6},(void*)1},{{_tmp42B,_tmp42B + 9},(void*)2},{{_tmp42C,_tmp42C + 9},(
void*)3},{{_tmp42D,_tmp42D + 6},(void*)4},{{_tmp42E,_tmp42E + 8},(void*)&
att_aligned},{{_tmp42F,_tmp42F + 7},(void*)5},{{_tmp430,_tmp430 + 7},(void*)7},{{
_tmp431,_tmp431 + 7},(void*)8},{{_tmp432,_tmp432 + 5},(void*)9},{{_tmp433,_tmp433 + 
10},(void*)10},{{_tmp434,_tmp434 + 10},(void*)11},{{_tmp435,_tmp435 + 23},(void*)
12},{{_tmp436,_tmp436 + 12},(void*)13},{{_tmp437,_tmp437 + 11},(void*)14},{{
_tmp438,_tmp438 + 22},(void*)15},{{_tmp439,_tmp439 + 5},(void*)16}};struct
_dynforward_ptr _tmp425=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp425,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp425,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp425,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp425,sizeof(char),(int)(
_get_dynforward_size(_tmp425,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp425,sizeof(char),(int)(_get_dynforward_size(
_tmp425,sizeof(char))- 3)))== '_')_tmp425=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp425,2,_get_dynforward_size(_tmp425,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp425,(struct
_dynforward_ptr)(att_map[i]).f1)== 0){yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp426;(_tmp426.YY46).tag=50;(_tmp426.YY46).f1=(void*)(att_map[i]).f2;_tmp426;});
break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp427="unrecognized attribute";
_tag_dynforward(_tmp427,sizeof(char),_get_zero_arr_size(_tmp427,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp428;(_tmp428.YY46).tag=50;(_tmp428.YY46).f1=(void*)((
void*)1);_tmp428;});}break;}}case 58: _LL238: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp43B;(_tmp43B.YY46).tag=50;(_tmp43B.YY46).f1=(void*)((void*)
4);_tmp43B;});break;case 59: _LL239: {struct _dynforward_ptr _tmp43C=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _tuple11
_tmp43E;int _tmp43F;struct _tuple11*_tmp43D=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp43E=*_tmp43D;_tmp43F=
_tmp43E.f2;{void*a;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp43C,({const char*
_tmp440="regparm";_tag_dynforward(_tmp440,sizeof(char),_get_zero_arr_size(
_tmp440,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp43C,({const char*
_tmp441="__regparm__";_tag_dynforward(_tmp441,sizeof(char),_get_zero_arr_size(
_tmp441,12));}))== 0){if(_tmp43F < 0  || _tmp43F > 3)Cyc_Parse_err(({const char*
_tmp442="regparm requires value between 0 and 3";_tag_dynforward(_tmp442,sizeof(
char),_get_zero_arr_size(_tmp442,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp443=_cycalloc_atomic(sizeof(*_tmp443));
_tmp443[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp444;_tmp444.tag=0;_tmp444.f1=
_tmp43F;_tmp444;});_tmp443;});}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp43C,({const char*_tmp445="aligned";_tag_dynforward(_tmp445,sizeof(char),
_get_zero_arr_size(_tmp445,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp43C,({const char*_tmp446="__aligned__";_tag_dynforward(_tmp446,sizeof(char),
_get_zero_arr_size(_tmp446,12));}))== 0){if(_tmp43F < 0)Cyc_Parse_err(({const char*
_tmp447="aligned requires positive power of two";_tag_dynforward(_tmp447,sizeof(
char),_get_zero_arr_size(_tmp447,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp43F;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp448="aligned requires positive power of two";
_tag_dynforward(_tmp448,sizeof(char),_get_zero_arr_size(_tmp448,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp449=_cycalloc_atomic(sizeof(*_tmp449));
_tmp449[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp44A;_tmp44A.tag=1;_tmp44A.f1=
_tmp43F;_tmp44A;});_tmp449;});}}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp43C,({const char*_tmp44B="initializes";_tag_dynforward(_tmp44B,sizeof(char),
_get_zero_arr_size(_tmp44B,12));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp43C,({const char*_tmp44C="__initializes__";_tag_dynforward(_tmp44C,sizeof(
char),_get_zero_arr_size(_tmp44C,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp44D=_cycalloc_atomic(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp44E;_tmp44E.tag=4;_tmp44E.f1=_tmp43F;_tmp44E;});_tmp44D;});else{Cyc_Parse_err(({
const char*_tmp44F="unrecognized attribute";_tag_dynforward(_tmp44F,sizeof(char),
_get_zero_arr_size(_tmp44F,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp450;(_tmp450.YY46).tag=
50;(_tmp450.YY46).f1=(void*)a;_tmp450;});break;}}case 60: _LL23A: {struct
_dynforward_ptr _tmp451=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp452=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp451,({const char*_tmp453="section";_tag_dynforward(
_tmp453,sizeof(char),_get_zero_arr_size(_tmp453,8));}))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp451,({const char*_tmp454="__section__";_tag_dynforward(
_tmp454,sizeof(char),_get_zero_arr_size(_tmp454,12));}))== 0)a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455[0]=({
struct Cyc_Absyn_Section_att_struct _tmp456;_tmp456.tag=2;_tmp456.f1=_tmp452;
_tmp456;});_tmp455;});else{Cyc_Parse_err(({const char*_tmp457="unrecognized attribute";
_tag_dynforward(_tmp457,sizeof(char),_get_zero_arr_size(_tmp457,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp458;(_tmp458.YY46).tag=
50;(_tmp458.YY46).f1=(void*)a;_tmp458;});break;}case 61: _LL23B: {struct
_dynforward_ptr _tmp459=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp45A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp459,({const char*_tmp45B="__mode__";_tag_dynforward(
_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp45D;_tmp45D.tag=5;_tmp45D.f1=_tmp45A;_tmp45D;});_tmp45C;});else{Cyc_Parse_err(({
const char*_tmp45E="unrecognized attribute";_tag_dynforward(_tmp45E,sizeof(char),
_get_zero_arr_size(_tmp45E,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45F;(_tmp45F.YY46).tag=
50;(_tmp45F.YY46).f1=(void*)a;_tmp45F;});break;}case 62: _LL23C: {struct
_dynforward_ptr _tmp460=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp461=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp463;struct
_tuple11 _tmp462=*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp463=_tmp462.f2;{int _tmp465;struct _tuple11 _tmp464=*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp465=_tmp464.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp460,({const char*
_tmp466="format";_tag_dynforward(_tmp466,sizeof(char),_get_zero_arr_size(_tmp466,
7));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp460,({const char*_tmp467="__format__";
_tag_dynforward(_tmp467,sizeof(char),_get_zero_arr_size(_tmp467,11));}))== 0){
if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp461,({const char*_tmp468="printf";
_tag_dynforward(_tmp468,sizeof(char),_get_zero_arr_size(_tmp468,7));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp469=_cycalloc(sizeof(*_tmp469));
_tmp469[0]=({struct Cyc_Absyn_Format_att_struct _tmp46A;_tmp46A.tag=3;_tmp46A.f1=(
void*)((void*)0);_tmp46A.f2=_tmp463;_tmp46A.f3=_tmp465;_tmp46A;});_tmp469;});
else{if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp461,({const char*_tmp46B="scanf";
_tag_dynforward(_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,6));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));
_tmp46C[0]=({struct Cyc_Absyn_Format_att_struct _tmp46D;_tmp46D.tag=3;_tmp46D.f1=(
void*)((void*)1);_tmp46D.f2=_tmp463;_tmp46D.f3=_tmp465;_tmp46D;});_tmp46C;});
else{Cyc_Parse_err(({const char*_tmp46E="unrecognized format type";
_tag_dynforward(_tmp46E,sizeof(char),_get_zero_arr_size(_tmp46E,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp46F="unrecognized attribute";_tag_dynforward(_tmp46F,sizeof(char),
_get_zero_arr_size(_tmp46F,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp470;(_tmp470.YY46).tag=50;(_tmp470.YY46).f1=(
void*)a;_tmp470;});break;}}}case 63: _LL23D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL23E: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp471;(_tmp471.YY23).tag=27;(
_tmp471.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp473;_tmp473.tag=17;_tmp473.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp473.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp473.f3=0;_tmp473.f4=
0;_tmp473;});_tmp472;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp471;});
break;case 65: _LL23F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp474;(_tmp474.YY23).tag=27;(_tmp474.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp474;});break;case 66: _LL240: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp475;(_tmp475.YY23).tag=27;(_tmp475.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp475;});
break;case 67: _LL241: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp476;(_tmp476.YY23).tag=27;(_tmp476.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->v=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp477;}),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp476;});
break;case 68: _LL242: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp478;(_tmp478.YY23).tag=27;(_tmp478.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp478;});break;case 69: _LL243: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp479;(_tmp479.YY23).tag=27;(_tmp479.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));
_tmp47A[0]=({struct Cyc_Parse_Short_spec_struct _tmp47B;_tmp47B.tag=2;_tmp47B.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp47B;});_tmp47A;}));_tmp479;});break;case 70: _LL244:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp47C;(_tmp47C.YY23).tag=
27;(_tmp47C.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47C;});
break;case 71: _LL245: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47D;(_tmp47D.YY23).tag=27;(_tmp47D.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({struct Cyc_Parse_Long_spec_struct
_tmp47F;_tmp47F.tag=3;_tmp47F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp47F;});
_tmp47E;}));_tmp47D;});break;case 72: _LL246: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp480;(_tmp480.YY23).tag=27;(_tmp480.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp480;});
break;case 73: _LL247: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp481;(_tmp481.YY23).tag=27;(_tmp481.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp481;});break;case 74: _LL248: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp482;(_tmp482.YY23).tag=27;(_tmp482.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp483=_cycalloc(sizeof(*_tmp483));
_tmp483[0]=({struct Cyc_Parse_Signed_spec_struct _tmp484;_tmp484.tag=0;_tmp484.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp484;});_tmp483;}));_tmp482;});break;case 75: _LL249:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp485;(_tmp485.YY23).tag=
27;(_tmp485.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp487;_tmp487.tag=1;_tmp487.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp487;});
_tmp486;}));_tmp485;});break;case 76: _LL24A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL24B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL24C:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL24D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp488;(_tmp488.YY23).tag=
27;(_tmp488.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp488;});
break;case 80: _LL24E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp489;(_tmp489.YY23).tag=27;(_tmp489.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({
struct Cyc_Absyn_TupleType_struct _tmp48B;_tmp48B.tag=9;_tmp48B.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp48B;});_tmp48A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp489;});break;case 81: _LL24F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp48C;(_tmp48C.YY23).tag=27;(_tmp48C.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));
_tmp48D[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp48E;_tmp48E.tag=15;_tmp48E.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp48E;});_tmp48D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48C;});
break;case 82: _LL250: {void*_tmp48F=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->v=(void*)((void*)3);_tmp493;}),0);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp490;(_tmp490.YY23).tag=
27;(_tmp490.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp492;_tmp492.tag=16;_tmp492.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp492.f2=(void*)
_tmp48F;_tmp492;});_tmp491;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp490;});
break;}case 83: _LL251: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp494;(_tmp494.YY23).tag=27;(_tmp494.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp496;_tmp496.tag=16;_tmp496.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp496.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp496;});_tmp495;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp494;});
break;case 84: _LL252: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp497;(_tmp497.YY23).tag=27;(_tmp497.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp499;_tmp499.tag=14;_tmp499.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp499;});_tmp498;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp497;});break;case 85: _LL253: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp49A;(_tmp49A.YY23).tag=27;(_tmp49A.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp49B=_cycalloc(sizeof(*_tmp49B));
_tmp49B[0]=({struct Cyc_Absyn_TagType_struct _tmp49C;_tmp49C.tag=18;_tmp49C.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp49C;});_tmp49B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49A;});
break;case 86: _LL254: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp49D;(_tmp49D.YY23).tag=27;(_tmp49D.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({
struct Cyc_Absyn_TagType_struct _tmp49F;_tmp49F.tag=18;_tmp49F.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->v=(void*)((void*)
5);_tmp4A0;}),0);_tmp49F;});_tmp49E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49D;});
break;case 87: _LL255: {struct _tuple11 _tmp4A2;int _tmp4A3;struct _tuple11*_tmp4A1=
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4A2=*_tmp4A1;_tmp4A3=_tmp4A2.f2;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A4;(_tmp4A4.YY23).tag=27;(_tmp4A4.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TypeInt_struct*_tmp4A5=_cycalloc_atomic(sizeof(*_tmp4A5));
_tmp4A5[0]=({struct Cyc_Absyn_TypeInt_struct _tmp4A6;_tmp4A6.tag=19;_tmp4A6.f1=
_tmp4A3;_tmp4A6;});_tmp4A5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A4;});
break;}case 88: _LL256: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A7;(_tmp4A7.YY43).tag=47;(_tmp4A7.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A7;});
break;case 89: _LL257: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A8;(_tmp4A8.YY25).tag=29;(_tmp4A8.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4A9;
_tmp4A9.print_const=1;_tmp4A9.q_volatile=0;_tmp4A9.q_restrict=0;_tmp4A9.real_const=
1;_tmp4A9.loc=loc;_tmp4A9;});_tmp4A8;});break;}case 90: _LL258: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4AA;(_tmp4AA.YY25).tag=29;(_tmp4AA.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp4AB;_tmp4AB.print_const=0;_tmp4AB.q_volatile=1;_tmp4AB.q_restrict=
0;_tmp4AB.real_const=0;_tmp4AB.loc=0;_tmp4AB;});_tmp4AA;});break;case 91: _LL259:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4AC;(_tmp4AC.YY25).tag=
29;(_tmp4AC.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4AD;_tmp4AD.print_const=0;
_tmp4AD.q_volatile=0;_tmp4AD.q_restrict=1;_tmp4AD.real_const=0;_tmp4AD.loc=0;
_tmp4AD;});_tmp4AC;});break;case 92: _LL25A: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4AE;(_tmp4AE.YY23).tag=27;(_tmp4AE.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4B0;_tmp4B0.tag=5;_tmp4B0.f1=({struct Cyc_Absyn_Decl*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4B3;_tmp4B3.tag=6;_tmp4B3.f1=({struct Cyc_Absyn_Enumdecl*_tmp4B4=_cycalloc(
sizeof(*_tmp4B4));_tmp4B4->sc=(void*)((void*)2);_tmp4B4->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4B4->fields=({
struct Cyc_Core_Opt*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B5;});_tmp4B4;});
_tmp4B3;});_tmp4B2;}));_tmp4B1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B1;});
_tmp4B0;});_tmp4AF;}));_tmp4AE;});break;case 93: _LL25B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B6;(_tmp4B6.YY23).tag=27;(_tmp4B6.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));
_tmp4B7[0]=({struct Cyc_Absyn_EnumType_struct _tmp4B8;_tmp4B8.tag=12;_tmp4B8.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4B8.f2=0;_tmp4B8;});_tmp4B7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B6;});
break;case 94: _LL25C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B9;(_tmp4B9.YY23).tag=27;(_tmp4B9.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4BB;_tmp4BB.tag=4;_tmp4BB.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4BD;_tmp4BD.tag=13;_tmp4BD.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BD;});_tmp4BC;}));
_tmp4BB.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4BB;});_tmp4BA;}));_tmp4B9;});break;case 95: _LL25D:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BE;(_tmp4BE.YY47).tag=
51;(_tmp4BE.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4BF=_cycalloc(sizeof(*
_tmp4BF));_tmp4BF->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4BF->tag=0;_tmp4BF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4BF;});
_tmp4BE;});break;case 96: _LL25E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C0;(_tmp4C0.YY47).tag=51;(_tmp4C0.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C1->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C1->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4C1;});_tmp4C0;});break;case 97: _LL25F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C2;(_tmp4C2.YY48).tag=52;(
_tmp4C2.YY48).f1=({struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));
_tmp4C3->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C3->tl=0;_tmp4C3;});_tmp4C2;});break;case 98: _LL260: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C4;(_tmp4C4.YY48).tag=52;(
_tmp4C4.YY48).f1=({struct Cyc_List_List*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));
_tmp4C5->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4C5->tl=0;_tmp4C5;});_tmp4C4;});break;case 99: _LL261: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C6;(_tmp4C6.YY48).tag=52;(
_tmp4C6.YY48).f1=({struct Cyc_List_List*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));
_tmp4C7->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4C7->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C7;});_tmp4C6;});
break;case 100: _LL262: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C8;(_tmp4C8.YY23).tag=27;(_tmp4C8.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4CA;_tmp4CA.tag=11;_tmp4CA.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4CA.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4CA;});_tmp4C9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C8;});
break;case 101: _LL263: {struct Cyc_List_List*_tmp4CB=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4CC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CD;(_tmp4CD.YY23).tag=27;(_tmp4CD.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4CF;_tmp4CF.tag=5;_tmp4CF.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4CB,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4CC,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CF;});
_tmp4CE;}));_tmp4CD;});break;}case 102: _LL264: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D0;(_tmp4D0.YY23).tag=27;(_tmp4D0.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));
_tmp4D1[0]=({struct Cyc_Absyn_AggrType_struct _tmp4D2;_tmp4D2.tag=10;_tmp4D2.f1=({
struct Cyc_Absyn_AggrInfo _tmp4D3;_tmp4D3.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp4D4;(_tmp4D4.UnknownAggr).tag=0;(_tmp4D4.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp4D4.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4D4;});_tmp4D3.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D3;});_tmp4D2;});
_tmp4D1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4D0;});break;case 103: _LL265: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D5;(_tmp4D5.YY40).tag=44;(_tmp4D5.YY40).f1=0;_tmp4D5;});
break;case 104: _LL266: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D6;(_tmp4D6.YY40).tag=44;(_tmp4D6.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4D6;});break;
case 105: _LL267: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D7;(
_tmp4D7.YY24).tag=28;(_tmp4D7.YY24).f1=(void*)((void*)0);_tmp4D7;});break;case
106: _LL268: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D8;(
_tmp4D8.YY24).tag=28;(_tmp4D8.YY24).f1=(void*)((void*)1);_tmp4D8;});break;case
107: _LL269: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D9;(
_tmp4D9.YY26).tag=30;(_tmp4D9.YY26).f1=0;_tmp4D9;});break;case 108: _LL26A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DA;(_tmp4DA.YY26).tag=30;(
_tmp4DA.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4DA;});break;
case 109: _LL26B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DB;(
_tmp4DB.YY27).tag=31;(_tmp4DB.YY27).f1=({struct Cyc_List_List*_tmp4DC=_cycalloc(
sizeof(*_tmp4DC));_tmp4DC->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4DC->tl=0;_tmp4DC;});_tmp4DB;});break;case 110: _LL26C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DD;(_tmp4DD.YY27).tag=
31;(_tmp4DD.YY27).f1=({struct Cyc_List_List*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));
_tmp4DE->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4DE->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4DE;});_tmp4DD;});break;case 111: _LL26D: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DF;(_tmp4DF.YY21).tag=25;(_tmp4DF.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4DF;});break;case
112: _LL26E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E0;(
_tmp4E0.YY21).tag=25;(_tmp4E0.YY21).f1=({struct Cyc_List_List*_tmp4E1=_cycalloc(
sizeof(*_tmp4E1));_tmp4E1->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E1->tl=0;_tmp4E1;});_tmp4E0;});break;case 113: _LL26F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E2;(_tmp4E2.YY21).tag=
25;(_tmp4E2.YY21).f1=({struct Cyc_List_List*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));
_tmp4E3->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E3->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp4E3;});_tmp4E2;});break;case 114: _LL270: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E4;(_tmp4E4.YY20).tag=24;(_tmp4E4.YY20).f1=({
struct _tuple15*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E5->f2=0;_tmp4E5;});
_tmp4E4;});break;case 115: _LL271: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E6;(_tmp4E6.YY20).tag=24;(_tmp4E6.YY20).f1=({struct _tuple15*_tmp4E7=
_cycalloc(sizeof(*_tmp4E7));_tmp4E7->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E7->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E7;});
_tmp4E6;});break;case 116: _LL272:{struct _RegionHandle _tmp4E8=_new_region("temp");
struct _RegionHandle*temp=& _tmp4E8;_push_region(temp);{struct _tuple16 _tmp4EA;
struct Cyc_Absyn_Tqual _tmp4EB;struct Cyc_List_List*_tmp4EC;struct Cyc_List_List*
_tmp4ED;struct _tuple16*_tmp4E9=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EA=*_tmp4E9;_tmp4EB=
_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;_tmp4ED=_tmp4EA.f3;if(_tmp4EB.loc == 0)_tmp4EB.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4EF;struct Cyc_List_List*
_tmp4F0;struct _tuple0 _tmp4EE=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4EF=_tmp4EE.f1;
_tmp4F0=_tmp4EE.f2;{void*_tmp4F1=Cyc_Parse_speclist2typ(_tmp4EC,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4F2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4EB,_tmp4F1,_tmp4EF,_tmp4ED),_tmp4F0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F3;(_tmp4F3.YY26).tag=30;(_tmp4F3.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4F2);
_tmp4F3;});}}};_pop_region(temp);}break;case 117: _LL273: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F4;(_tmp4F4.YY35).tag=39;(_tmp4F4.YY35).f1=({struct
_tuple16*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F5->f2=({struct Cyc_List_List*_tmp4F6=_cycalloc(
sizeof(*_tmp4F6));_tmp4F6->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F6->tl=0;_tmp4F6;});
_tmp4F5->f3=0;_tmp4F5;});_tmp4F4;});break;case 118: _LL274: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F7;(_tmp4F7.YY35).tag=39;(_tmp4F7.YY35).f1=({struct
_tuple16*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4F8->f2=({struct Cyc_List_List*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F9->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F9;});_tmp4F8->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4F8;});_tmp4F7;});break;case 119: _LL275: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FA;(_tmp4FA.YY35).tag=39;(_tmp4FA.YY35).f1=({struct
_tuple16*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FB->f2=0;_tmp4FB->f3=0;
_tmp4FB;});_tmp4FA;});break;case 120: _LL276: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FC;(_tmp4FC.YY35).tag=39;(_tmp4FC.YY35).f1=({struct
_tuple16*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp4FD->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp4FD->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4FD;});_tmp4FC;});
break;case 121: _LL277: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FE;(_tmp4FE.YY35).tag=39;(_tmp4FE.YY35).f1=({struct _tuple16*_tmp4FF=
_cycalloc(sizeof(*_tmp4FF));_tmp4FF->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FF->f2=0;
_tmp4FF->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4FF;});_tmp4FE;});break;case 122: _LL278: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp500;(_tmp500.YY35).tag=39;(_tmp500.YY35).f1=({struct
_tuple16*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp501->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp501->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp501;});_tmp500;});break;case 123: _LL279: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp502;(_tmp502.YY35).tag=39;(_tmp502.YY35).f1=({struct
_tuple16*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp503->f2=({struct Cyc_List_List*_tmp504=_cycalloc(
sizeof(*_tmp504));_tmp504->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp504->tl=0;_tmp504;});
_tmp503->f3=0;_tmp503;});_tmp502;});break;case 124: _LL27A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp505;(_tmp505.YY35).tag=39;(_tmp505.YY35).f1=({struct
_tuple16*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp506->f2=({struct Cyc_List_List*
_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp507->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp507;});_tmp506->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp506;});_tmp505;});break;case 125: _LL27B: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp508;(_tmp508.YY35).tag=39;(_tmp508.YY35).f1=({struct
_tuple16*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp509->f2=0;_tmp509->f3=0;
_tmp509;});_tmp508;});break;case 126: _LL27C: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp50A;(_tmp50A.YY35).tag=39;(_tmp50A.YY35).f1=({struct
_tuple16*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp50B->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp50B->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp50B;});_tmp50A;});
break;case 127: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp50C;(_tmp50C.YY35).tag=39;(_tmp50C.YY35).f1=({struct _tuple16*_tmp50D=
_cycalloc(sizeof(*_tmp50D));_tmp50D->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp50D->f2=0;
_tmp50D->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp50D;});_tmp50C;});break;case 128: _LL27E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp50E;(_tmp50E.YY35).tag=39;(_tmp50E.YY35).f1=({struct
_tuple16*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp50F->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp50F->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp50F;});_tmp50E;});break;case 129: _LL27F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp510;(_tmp510.YY21).tag=25;(_tmp510.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp510;});break;case 130:
_LL280: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp511;(_tmp511.YY21).tag=
25;(_tmp511.YY21).f1=({struct Cyc_List_List*_tmp512=_cycalloc(sizeof(*_tmp512));
_tmp512->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp512->tl=0;_tmp512;});_tmp511;});break;case 131: _LL281: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp513;(_tmp513.YY21).tag=25;(
_tmp513.YY21).f1=({struct Cyc_List_List*_tmp514=_cycalloc(sizeof(*_tmp514));
_tmp514->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp514->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp514;});_tmp513;});break;case 132: _LL282: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp515;(_tmp515.YY20).tag=24;(_tmp515.YY20).f1=({
struct _tuple15*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp516->f2=0;_tmp516;});
_tmp515;});break;case 133: _LL283: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp517;(_tmp517.YY20).tag=24;(_tmp517.YY20).f1=({struct _tuple15*_tmp518=
_cycalloc(sizeof(*_tmp518));_tmp518->f1=({struct Cyc_Parse_Declarator*_tmp519=
_cycalloc(sizeof(*_tmp519));_tmp519->id=({struct _tuple1*_tmp51A=_cycalloc(
sizeof(*_tmp51A));_tmp51A->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp51C;(_tmp51C.Rel_n).tag=1;(_tmp51C.Rel_n).f1=0;_tmp51C;});_tmp51A->f2=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp51A;});_tmp519->tms=0;_tmp519;});_tmp518->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp518;});_tmp517;});
break;case 134: _LL284: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp51D;(_tmp51D.YY20).tag=24;(_tmp51D.YY20).f1=({struct _tuple15*_tmp51E=
_cycalloc(sizeof(*_tmp51E));_tmp51E->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51E->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51E;});
_tmp51D;});break;case 135: _LL285: {struct Cyc_List_List*_tmp51F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp520;(_tmp520.YY23).tag=27;(_tmp520.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp521=_cycalloc(sizeof(*_tmp521));
_tmp521[0]=({struct Cyc_Parse_Decl_spec_struct _tmp522;_tmp522.tag=5;_tmp522.f1=
Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp51F,({struct Cyc_Core_Opt*
_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp523;}),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp522;});
_tmp521;}));_tmp520;});break;}case 136: _LL286: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp524;(_tmp524.YY23).tag=27;(_tmp524.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp525=_cycalloc(sizeof(*_tmp525));
_tmp525[0]=({struct Cyc_Absyn_TunionType_struct _tmp526;_tmp526.tag=2;_tmp526.f1=({
struct Cyc_Absyn_TunionInfo _tmp527;_tmp527.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp528;(_tmp528.UnknownTunion).tag=0;(_tmp528.UnknownTunion).f1=({
struct Cyc_Absyn_UnknownTunionInfo _tmp529;_tmp529.name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp529.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp529.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp529;});_tmp528;});
_tmp527.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp527.rgn=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp527;});_tmp526;});_tmp525;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp524;});
break;case 137: _LL287: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52A;(_tmp52A.YY23).tag=27;(_tmp52A.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));
_tmp52B[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp52C;_tmp52C.tag=3;_tmp52C.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp52D;_tmp52D.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp52E;(_tmp52E.UnknownTunionfield).tag=0;(
_tmp52E.UnknownTunionfield).f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp52F;
_tmp52F.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp52F.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52F.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp52F;});_tmp52E;});
_tmp52D.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp52D;});_tmp52C;});_tmp52B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp52A;});
break;case 138: _LL288: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp530;(_tmp530.YY31).tag=35;(_tmp530.YY31).f1=1;_tmp530;});break;case 139:
_LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp531;(_tmp531.YY31).tag=
35;(_tmp531.YY31).f1=0;_tmp531;});break;case 140: _LL28A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp532;(_tmp532.YY31).tag=35;(_tmp532.YY31).f1=0;_tmp532;});
break;case 141: _LL28B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp533;(_tmp533.YY31).tag=35;(_tmp533.YY31).f1=1;_tmp533;});break;case 142:
_LL28C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp534;(_tmp534.YY34).tag=
38;(_tmp534.YY34).f1=({struct Cyc_List_List*_tmp535=_cycalloc(sizeof(*_tmp535));
_tmp535->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp535->tl=0;_tmp535;});_tmp534;});break;case 143: _LL28D: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp536;(_tmp536.YY34).tag=38;(
_tmp536.YY34).f1=({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));
_tmp537->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp537->tl=0;_tmp537;});_tmp536;});break;case 144: _LL28E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp538;(_tmp538.YY34).tag=
38;(_tmp538.YY34).f1=({struct Cyc_List_List*_tmp539=_cycalloc(sizeof(*_tmp539));
_tmp539->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp539->tl=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp539;});_tmp538;});
break;case 145: _LL28F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53A;(_tmp53A.YY34).tag=38;(_tmp53A.YY34).f1=({struct Cyc_List_List*_tmp53B=
_cycalloc(sizeof(*_tmp53B));_tmp53B->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp53B->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53B;});_tmp53A;});
break;case 146: _LL290: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53C;(_tmp53C.YY32).tag=36;(_tmp53C.YY32).f1=(void*)((void*)2);_tmp53C;});
break;case 147: _LL291: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53D;(_tmp53D.YY32).tag=36;(_tmp53D.YY32).f1=(void*)((void*)3);_tmp53D;});
break;case 148: _LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53E;(_tmp53E.YY32).tag=36;(_tmp53E.YY32).f1=(void*)((void*)0);_tmp53E;});
break;case 149: _LL293: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53F;(_tmp53F.YY33).tag=37;(_tmp53F.YY33).f1=({struct Cyc_Absyn_Tunionfield*
_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp540->typs=0;_tmp540->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp540->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp540;});_tmp53F;});
break;case 150: _LL294: {struct Cyc_List_List*_tmp541=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp542;(_tmp542.YY33).tag=37;(_tmp542.YY33).f1=({struct
Cyc_Absyn_Tunionfield*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp543->typs=
_tmp541;_tmp543->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp543->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp543;});_tmp542;});break;}case 151: _LL295: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 152: _LL296: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp544;(_tmp544.YY29).tag=33;(
_tmp544.YY29).f1=({struct Cyc_Parse_Declarator*_tmp545=_cycalloc(sizeof(*_tmp545));
_tmp545->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp545->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp545;});_tmp544;});
break;case 153: _LL297: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 154: _LL298: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp546;(_tmp546.YY29).tag=33;(_tmp546.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp547->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp547;});
_tmp546;});break;case 155: _LL299: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp548;(_tmp548.YY29).tag=33;(_tmp548.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp549->tms=0;_tmp549;});
_tmp548;});break;case 156: _LL29A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 157: _LL29B: {struct Cyc_Parse_Declarator*_tmp54A=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54A->tms=({
struct Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp54C=_cycalloc(sizeof(*_tmp54C));
_tmp54C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp54D;_tmp54D.tag=5;_tmp54D.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp54D.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54D;});_tmp54C;}));
_tmp54B->tl=_tmp54A->tms;_tmp54B;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 158: _LL29C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp54E;(_tmp54E.YY29).tag=33;(_tmp54E.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp54F->tms=({
struct Cyc_List_List*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp551=_cycalloc(sizeof(*_tmp551));
_tmp551[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp552;_tmp552.tag=0;_tmp552.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp552.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp552;});_tmp551;}));_tmp550->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp550;});
_tmp54F;});_tmp54E;});break;case 159: _LL29D: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp553;(_tmp553.YY29).tag=33;(_tmp553.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp554->tms=({
struct Cyc_List_List*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp556=_cycalloc(sizeof(*_tmp556));
_tmp556[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp557;_tmp557.tag=1;_tmp557.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp557.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp557.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp557;});_tmp556;}));_tmp555->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp555;});
_tmp554;});_tmp553;});break;case 160: _LL29E: {struct _tuple17 _tmp559;struct Cyc_List_List*
_tmp55A;int _tmp55B;struct Cyc_Absyn_VarargInfo*_tmp55C;struct Cyc_Core_Opt*_tmp55D;
struct Cyc_List_List*_tmp55E;struct _tuple17*_tmp558=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp559=*_tmp558;_tmp55A=
_tmp559.f1;_tmp55B=_tmp559.f2;_tmp55C=_tmp559.f3;_tmp55D=_tmp559.f4;_tmp55E=
_tmp559.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp55F;(
_tmp55F.YY29).tag=33;(_tmp55F.YY29).f1=({struct Cyc_Parse_Declarator*_tmp560=
_cycalloc(sizeof(*_tmp560));_tmp560->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp560->tms=({
struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp562=_cycalloc(sizeof(*_tmp562));
_tmp562[0]=({struct Cyc_Absyn_Function_mod_struct _tmp563;_tmp563.tag=3;_tmp563.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp564=_cycalloc(sizeof(*
_tmp564));_tmp564[0]=({struct Cyc_Absyn_WithTypes_struct _tmp565;_tmp565.tag=1;
_tmp565.f1=_tmp55A;_tmp565.f2=_tmp55B;_tmp565.f3=_tmp55C;_tmp565.f4=_tmp55D;
_tmp565.f5=_tmp55E;_tmp565;});_tmp564;}));_tmp563;});_tmp562;}));_tmp561->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp561;});_tmp560;});_tmp55F;});break;}case 161: _LL29F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp566;(_tmp566.YY29).tag=33;(_tmp566.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp567->tms=({
struct Cyc_List_List*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp569=_cycalloc(sizeof(*_tmp569));
_tmp569[0]=({struct Cyc_Absyn_Function_mod_struct _tmp56A;_tmp56A.tag=3;_tmp56A.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp56B=_cycalloc(sizeof(*
_tmp56B));_tmp56B[0]=({struct Cyc_Absyn_WithTypes_struct _tmp56C;_tmp56C.tag=1;
_tmp56C.f1=0;_tmp56C.f2=0;_tmp56C.f3=0;_tmp56C.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56C.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56C;});_tmp56B;}));
_tmp56A;});_tmp569;}));_tmp568->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp568;});_tmp567;});
_tmp566;});break;case 162: _LL2A0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56D;(_tmp56D.YY29).tag=33;(_tmp56D.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp56E->tms=({
struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp570=_cycalloc(sizeof(*_tmp570));
_tmp570[0]=({struct Cyc_Absyn_Function_mod_struct _tmp571;_tmp571.tag=3;_tmp571.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp572=_cycalloc(sizeof(*_tmp572));
_tmp572[0]=({struct Cyc_Absyn_NoTypes_struct _tmp573;_tmp573.tag=0;_tmp573.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp573.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp573;});
_tmp572;}));_tmp571;});_tmp570;}));_tmp56F->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp56F;});_tmp56E;});
_tmp56D;});break;case 163: _LL2A1: {struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp575;(_tmp575.YY29).tag=
33;(_tmp575.YY29).f1=({struct Cyc_Parse_Declarator*_tmp576=_cycalloc(sizeof(*
_tmp576));_tmp576->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp576->tms=({struct Cyc_List_List*_tmp577=_cycalloc(
sizeof(*_tmp577));_tmp577->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp579;_tmp579.tag=4;_tmp579.f1=_tmp574;_tmp579.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp579.f3=0;
_tmp579;});_tmp578;}));_tmp577->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp577;});_tmp576;});
_tmp575;});break;}case 164: _LL2A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57A;(_tmp57A.YY29).tag=33;(_tmp57A.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp57B->tms=({
struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));
_tmp57D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp57E;_tmp57E.tag=5;_tmp57E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp57E.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57E;});_tmp57D;}));
_tmp57C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp57C;});_tmp57B;});_tmp57A;});break;case 165: _LL2A3:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp57F;(_tmp57F.YY29).tag=
33;(_tmp57F.YY29).f1=({struct Cyc_Parse_Declarator*_tmp580=_cycalloc(sizeof(*
_tmp580));_tmp580->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp580->tms=0;_tmp580;});_tmp57F;});break;case 166: _LL2A4:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp581;(_tmp581.YY29).tag=
33;(_tmp581.YY29).f1=({struct Cyc_Parse_Declarator*_tmp582=_cycalloc(sizeof(*
_tmp582));_tmp582->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp582->tms=0;_tmp582;});_tmp581;});break;case 167: _LL2A5:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 168:
_LL2A6: {struct Cyc_Parse_Declarator*_tmp583=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp583->tms=({struct Cyc_List_List*
_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp586;_tmp586.tag=5;_tmp586.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp586.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp586;});_tmp585;}));_tmp584->tl=_tmp583->tms;_tmp584;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 169: _LL2A7:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp587;(_tmp587.YY29).tag=
33;(_tmp587.YY29).f1=({struct Cyc_Parse_Declarator*_tmp588=_cycalloc(sizeof(*
_tmp588));_tmp588->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp588->tms=({struct Cyc_List_List*_tmp589=_cycalloc(
sizeof(*_tmp589));_tmp589->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp58B;_tmp58B.tag=0;_tmp58B.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp58B;});
_tmp58A;}));_tmp589->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp589;});_tmp588;});_tmp587;});break;case 170:
_LL2A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp58C;(_tmp58C.YY29).tag=
33;(_tmp58C.YY29).f1=({struct Cyc_Parse_Declarator*_tmp58D=_cycalloc(sizeof(*
_tmp58D));_tmp58D->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp58D->tms=({struct Cyc_List_List*_tmp58E=_cycalloc(
sizeof(*_tmp58E));_tmp58E->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp590;_tmp590.tag=1;_tmp590.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp590.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp590.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp590;});
_tmp58F;}));_tmp58E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp58E;});_tmp58D;});_tmp58C;});break;case 171:
_LL2A9: {struct _tuple17 _tmp592;struct Cyc_List_List*_tmp593;int _tmp594;struct Cyc_Absyn_VarargInfo*
_tmp595;struct Cyc_Core_Opt*_tmp596;struct Cyc_List_List*_tmp597;struct _tuple17*
_tmp591=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp592=*_tmp591;_tmp593=_tmp592.f1;_tmp594=_tmp592.f2;_tmp595=_tmp592.f3;
_tmp596=_tmp592.f4;_tmp597=_tmp592.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp598;(_tmp598.YY29).tag=33;(_tmp598.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp599->tms=({
struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));
_tmp59B[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59C;_tmp59C.tag=3;_tmp59C.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp59D=_cycalloc(sizeof(*
_tmp59D));_tmp59D[0]=({struct Cyc_Absyn_WithTypes_struct _tmp59E;_tmp59E.tag=1;
_tmp59E.f1=_tmp593;_tmp59E.f2=_tmp594;_tmp59E.f3=_tmp595;_tmp59E.f4=_tmp596;
_tmp59E.f5=_tmp597;_tmp59E;});_tmp59D;}));_tmp59C;});_tmp59B;}));_tmp59A->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp59A;});_tmp599;});_tmp598;});break;}case 172: _LL2AA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp59F;(_tmp59F.YY29).tag=33;(_tmp59F.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5A0->tms=({
struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));
_tmp5A2[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5A3;_tmp5A3.tag=3;_tmp5A3.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5A4=_cycalloc(sizeof(*
_tmp5A4));_tmp5A4[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A5;_tmp5A5.tag=1;
_tmp5A5.f1=0;_tmp5A5.f2=0;_tmp5A5.f3=0;_tmp5A5.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A5.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A5;});_tmp5A4;}));
_tmp5A3;});_tmp5A2;}));_tmp5A1->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5A1;});_tmp5A0;});
_tmp59F;});break;case 173: _LL2AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A6;(_tmp5A6.YY29).tag=33;(_tmp5A6.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5A7->tms=({
struct Cyc_List_List*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));
_tmp5A9[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5AA;_tmp5AA.tag=3;_tmp5AA.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));
_tmp5AB[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5AC;_tmp5AC.tag=0;_tmp5AC.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AC;});
_tmp5AB;}));_tmp5AA;});_tmp5A9;}));_tmp5A8->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5A8;});_tmp5A7;});
_tmp5A6;});break;case 174: _LL2AC: {struct Cyc_List_List*_tmp5AD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5AE;(_tmp5AE.YY29).tag=
33;(_tmp5AE.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5AF=_cycalloc(sizeof(*
_tmp5AF));_tmp5AF->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5AF->tms=({struct Cyc_List_List*_tmp5B0=_cycalloc(
sizeof(*_tmp5B0));_tmp5B0->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5B2;_tmp5B2.tag=4;_tmp5B2.f1=_tmp5AD;_tmp5B2.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B2.f3=0;
_tmp5B2;});_tmp5B1;}));_tmp5B0->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B0;});_tmp5AF;});
_tmp5AE;});break;}case 175: _LL2AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B3;(_tmp5B3.YY29).tag=33;(_tmp5B3.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5B4->tms=({
struct Cyc_List_List*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));
_tmp5B6[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5B7;_tmp5B7.tag=5;_tmp5B7.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5B7.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B7;});_tmp5B6;}));
_tmp5B5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5B5;});_tmp5B4;});_tmp5B3;});break;case 176: _LL2AE:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 177:
_LL2AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B8;(_tmp5B8.YY28).tag=
32;(_tmp5B8.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5B8;});break;case 178:
_LL2B0: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5BB;_tmp5BB.tag=5;_tmp5BB.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5BB.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5BB;});_tmp5BA;}));_tmp5B9->tl=ans;_tmp5B9;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5BC=_cycalloc(
sizeof(*_tmp5BC));_tmp5BC->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5BC->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5BC->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5BC;});ans=({struct Cyc_List_List*_tmp5BD=
_cycalloc(sizeof(*_tmp5BD));_tmp5BD->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5BF;_tmp5BF.tag=2;_tmp5BF.f1=({struct Cyc_Absyn_PtrAtts _tmp5C0;_tmp5C0.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5C0.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5C0.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5C0.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5C0.ptrloc=ptrloc;_tmp5C0;});_tmp5BF.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BF;});_tmp5BE;}));
_tmp5BD->tl=ans;_tmp5BD;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C1;(_tmp5C1.YY28).tag=32;(_tmp5C1.YY28).f1=ans;_tmp5C1;});break;}}case 179:
_LL2B1: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C2;(_tmp5C2.YY1).tag=5;(
_tmp5C2.YY1).f1=({struct _tuple12*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->f1=
loc;_tmp5C3->f2=Cyc_Absyn_true_conref;_tmp5C3->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C3;});_tmp5C2;});
break;}case 180: _LL2B2: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C4;(_tmp5C4.YY1).tag=5;(_tmp5C4.YY1).f1=({struct _tuple12*_tmp5C5=_cycalloc(
sizeof(*_tmp5C5));_tmp5C5->f1=loc;_tmp5C5->f2=Cyc_Absyn_false_conref;_tmp5C5->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C5;});
_tmp5C4;});break;}case 181: _LL2B3: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C6;(_tmp5C6.YY1).tag=5;(_tmp5C6.YY1).f1=({struct _tuple12*_tmp5C7=_cycalloc(
sizeof(*_tmp5C7));_tmp5C7->f1=loc;_tmp5C7->f2=Cyc_Absyn_true_conref;_tmp5C7->f3=
Cyc_Absyn_bounds_dynforward_conref;_tmp5C7;});_tmp5C6;});break;}case 182: _LL2B4: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C8;(_tmp5C8.YY1).tag=5;(
_tmp5C8.YY1).f1=({struct _tuple12*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->f1=
loc;_tmp5C9->f2=Cyc_Absyn_true_conref;_tmp5C9->f3=Cyc_Absyn_bounds_dyneither_conref;
_tmp5C9;});_tmp5C8;});break;}case 183: _LL2B5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5CA;(_tmp5CA.YY2).tag=6;(_tmp5CA.YY2).f1=Cyc_Absyn_bounds_one_conref;
_tmp5CA;});break;case 184: _LL2B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5CB;(_tmp5CB.YY2).tag=6;(_tmp5CB.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp5CD;_tmp5CD.tag=0;_tmp5CD.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CD;});_tmp5CC;}));
_tmp5CB;});break;case 185: _LL2B7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5CE;(_tmp5CE.YY2).tag=6;(_tmp5CE.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_AbsUpper_b_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({
struct Cyc_Absyn_AbsUpper_b_struct _tmp5D0;_tmp5D0.tag=1;_tmp5D0.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D0;});_tmp5CF;}));
_tmp5CE;});break;case 186: _LL2B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D1;(_tmp5D1.YY51).tag=55;(_tmp5D1.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5D1;});break;case 187: _LL2B9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D2;(_tmp5D2.YY51).tag=55;(_tmp5D2.YY51).f1=Cyc_Absyn_true_conref;
_tmp5D2;});break;case 188: _LL2BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D3;(_tmp5D3.YY51).tag=55;(_tmp5D3.YY51).f1=Cyc_Absyn_false_conref;_tmp5D3;});
break;case 189: _LL2BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D4;(_tmp5D4.YY49).tag=53;(_tmp5D4.YY49).f1=0;_tmp5D4;});break;case 190:
_LL2BC: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D5;(_tmp5D5.YY49).tag=53;(_tmp5D5.YY49).f1=({struct
Cyc_Core_Opt*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D6;});_tmp5D5;});
break;case 191: _LL2BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D7;(_tmp5D7.YY49).tag=53;(_tmp5D7.YY49).f1=({struct Cyc_Core_Opt*_tmp5D8=
_cycalloc(sizeof(*_tmp5D8));_tmp5D8->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->v=(void*)((void*)3);_tmp5D9;}),0);
_tmp5D8;});_tmp5D7;});break;case 192: _LL2BE: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp5DA;(_tmp5DA.YY44).tag=48;(_tmp5DA.YY44).f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->v=(void*)((void*)
3);_tmp5DB;}),0);_tmp5DA;});break;case 193: _LL2BF: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 194: _LL2C0: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5DC;(_tmp5DC.YY44).tag=48;(
_tmp5DC.YY44).f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5DD=
_cycalloc(sizeof(*_tmp5DD));_tmp5DD->v=(void*)((void*)3);_tmp5DD;}),0);_tmp5DC;});
break;case 195: _LL2C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5DE;(_tmp5DE.YY25).tag=29;(_tmp5DE.YY25).f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp5DE;});
break;case 196: _LL2C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5DF;(_tmp5DF.YY25).tag=29;(_tmp5DF.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5DF;});break;case 197:
_LL2C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E0;(_tmp5E0.YY39).tag=
43;(_tmp5E0.YY39).f1=({struct _tuple17*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));
_tmp5E1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp5E1->f2=0;_tmp5E1->f3=0;_tmp5E1->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E1->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E1;});_tmp5E0;});
break;case 198: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E2;(_tmp5E2.YY39).tag=43;(_tmp5E2.YY39).f1=({struct _tuple17*_tmp5E3=
_cycalloc(sizeof(*_tmp5E3));_tmp5E3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp5E3->f2=1;_tmp5E3->f3=0;_tmp5E3->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E3;});_tmp5E2;});
break;case 199: _LL2C5: {struct _tuple2 _tmp5E5;struct Cyc_Core_Opt*_tmp5E6;struct Cyc_Absyn_Tqual
_tmp5E7;void*_tmp5E8;struct _tuple2*_tmp5E4=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E5=*_tmp5E4;_tmp5E6=
_tmp5E5.f1;_tmp5E7=_tmp5E5.f2;_tmp5E8=_tmp5E5.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5E9=({struct Cyc_Absyn_VarargInfo*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->name=
_tmp5E6;_tmp5EC->tq=_tmp5E7;_tmp5EC->type=(void*)_tmp5E8;_tmp5EC->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5EC;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5EA;(_tmp5EA.YY39).tag=43;(
_tmp5EA.YY39).f1=({struct _tuple17*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->f1=
0;_tmp5EB->f2=0;_tmp5EB->f3=_tmp5E9;_tmp5EB->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5EB->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EB;});_tmp5EA;});
break;}}case 200: _LL2C6: {struct _tuple2 _tmp5EE;struct Cyc_Core_Opt*_tmp5EF;struct
Cyc_Absyn_Tqual _tmp5F0;void*_tmp5F1;struct _tuple2*_tmp5ED=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5EE=*_tmp5ED;_tmp5EF=
_tmp5EE.f1;_tmp5F0=_tmp5EE.f2;_tmp5F1=_tmp5EE.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5F2=({struct Cyc_Absyn_VarargInfo*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->name=
_tmp5EF;_tmp5F5->tq=_tmp5F0;_tmp5F5->type=(void*)_tmp5F1;_tmp5F5->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5F5;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F3;(_tmp5F3.YY39).tag=43;(
_tmp5F3.YY39).f1=({struct _tuple17*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5F4->f2=0;
_tmp5F4->f3=_tmp5F2;_tmp5F4->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F4->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F4;});_tmp5F3;});
break;}}case 201: _LL2C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F6;(_tmp5F6.YY44).tag=48;(_tmp5F6.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5F8;_tmp5F8.tag=1;_tmp5F8.f1=0;_tmp5F8;});_tmp5F7;}));_tmp5F6;});break;case
202: _LL2C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F9;(
_tmp5F9.YY44).tag=48;(_tmp5F9.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5FB;_tmp5FB.tag=0;_tmp5FB.f1=(void*)Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FB;});_tmp5FA;}));
_tmp5F9;});break;case 203: _LL2C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FC;(_tmp5FC.YY49).tag=53;(_tmp5FC.YY49).f1=0;_tmp5FC;});break;case 204:
_LL2CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FD;(_tmp5FD.YY49).tag=
53;(_tmp5FD.YY49).f1=({struct Cyc_Core_Opt*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));
_tmp5FE->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5FF=_cycalloc(
sizeof(*_tmp5FF));_tmp5FF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp600;_tmp600.tag=
21;_tmp600.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp600;});_tmp5FF;}));_tmp5FE;});_tmp5FD;});break;case 205:
_LL2CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp601;(_tmp601.YY50).tag=
54;(_tmp601.YY50).f1=0;_tmp601;});break;case 206: _LL2CC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 207: _LL2CD: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp602;(_tmp602.YY50).tag=54;(
_tmp602.YY50).f1=({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));
_tmp603->hd=({struct _tuple4*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->f1=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp607=_cycalloc(sizeof(*_tmp607));
_tmp607[0]=({struct Cyc_Absyn_JoinEff_struct _tmp608;_tmp608.tag=21;_tmp608.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp608;});_tmp607;});
_tmp604->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp606;_tmp606.tag=0;_tmp606.f1=(void*)((void*)3);_tmp606;});_tmp605;}));
_tmp604;});_tmp603->tl=0;_tmp603;});_tmp602;});break;case 208: _LL2CE: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp609;(_tmp609.YY50).tag=54;(_tmp609.YY50).f1=({
struct Cyc_List_List*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->hd=({struct
_tuple4*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp60F;_tmp60F.tag=21;_tmp60F.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp60F;});_tmp60E;});
_tmp60B->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp60D;_tmp60D.tag=0;_tmp60D.f1=(void*)((void*)3);_tmp60D;});_tmp60C;}));
_tmp60B;});_tmp60A->tl=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp60A;});_tmp609;});break;case 209: _LL2CF: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp610;(_tmp610.YY31).tag=35;(_tmp610.YY31).f1=0;_tmp610;});
break;case 210: _LL2D0: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp611="inject";
_tag_dynforward(_tmp611,sizeof(char),_get_zero_arr_size(_tmp611,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp612="missing type in function declaration";_tag_dynforward(_tmp612,
sizeof(char),_get_zero_arr_size(_tmp612,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp613;(_tmp613.YY31).tag=35;(_tmp613.YY31).f1=1;_tmp613;});
break;case 211: _LL2D1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 212: _LL2D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp614;(_tmp614.YY40).tag=44;(_tmp614.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp614;});break;case 213:
_LL2D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp615;(_tmp615.YY40).tag=
44;(_tmp615.YY40).f1=0;_tmp615;});break;case 214: _LL2D4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 215: _LL2D5:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp616;(_tmp616.YY40).tag=
44;(_tmp616.YY40).f1=({struct Cyc_List_List*_tmp617=_cycalloc(sizeof(*_tmp617));
_tmp617->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp618=_cycalloc(
sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp619;_tmp619.tag=
22;_tmp619.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp619;});_tmp618;}));_tmp617->tl=0;_tmp617;});_tmp616;});
break;case 216: _LL2D6: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)4);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp61A;(_tmp61A.YY40).tag=44;(_tmp61A.YY40).f1=({struct
Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61B->tl=0;_tmp61B;});
_tmp61A;});break;case 217: _LL2D7: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp61C;(_tmp61C.YY40).tag=44;(_tmp61C.YY40).f1=({struct
Cyc_List_List*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp61F;_tmp61F.tag=20;_tmp61F.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61F;});_tmp61E;}));
_tmp61D->tl=0;_tmp61D;});_tmp61C;});break;case 218: _LL2D8: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)3);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp620;(_tmp620.YY40).tag=
44;(_tmp620.YY40).f1=({struct Cyc_List_List*_tmp621=_cycalloc(sizeof(*_tmp621));
_tmp621->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp622=_cycalloc(
sizeof(*_tmp622));_tmp622[0]=({struct Cyc_Absyn_AccessEff_struct _tmp623;_tmp623.tag=
20;_tmp623.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp623;});_tmp622;}));_tmp621->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp621;});_tmp620;});
break;case 219: _LL2D9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp624;(_tmp624.YY38).tag=42;(_tmp624.YY38).f1=({struct Cyc_List_List*_tmp625=
_cycalloc(sizeof(*_tmp625));_tmp625->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp625->tl=0;_tmp625;});
_tmp624;});break;case 220: _LL2DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp626;(_tmp626.YY38).tag=42;(_tmp626.YY38).f1=({struct Cyc_List_List*_tmp627=
_cycalloc(sizeof(*_tmp627));_tmp627->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp627->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp627;});_tmp626;});
break;case 221: _LL2DB: {struct _tuple16 _tmp629;struct Cyc_Absyn_Tqual _tmp62A;struct
Cyc_List_List*_tmp62B;struct Cyc_List_List*_tmp62C;struct _tuple16*_tmp628=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp629=*_tmp628;
_tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;if(_tmp62A.loc == 0)
_tmp62A.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp62E;struct _tuple1*
_tmp62F;struct Cyc_List_List*_tmp630;struct Cyc_Parse_Declarator*_tmp62D=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62E=*_tmp62D;
_tmp62F=_tmp62E.id;_tmp630=_tmp62E.tms;{void*_tmp631=Cyc_Parse_speclist2typ(
_tmp62B,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp633;void*_tmp634;struct
Cyc_List_List*_tmp635;struct Cyc_List_List*_tmp636;struct _tuple6 _tmp632=Cyc_Parse_apply_tms(
_tmp62A,_tmp631,_tmp62C,_tmp630);_tmp633=_tmp632.f1;_tmp634=_tmp632.f2;_tmp635=
_tmp632.f3;_tmp636=_tmp632.f4;if(_tmp635 != 0)Cyc_Parse_err(({const char*_tmp637="parameter with bad type params";
_tag_dynforward(_tmp637,sizeof(char),_get_zero_arr_size(_tmp637,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp62F))Cyc_Parse_err(({const char*_tmp638="parameter cannot be qualified with a namespace";
_tag_dynforward(_tmp638,sizeof(char),_get_zero_arr_size(_tmp638,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp639=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp63E=_cycalloc(sizeof(*
_tmp63E));_tmp63E->v=(*_tmp62F).f2;_tmp63E;});if(_tmp636 != 0)({void*_tmp63A[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp63B="extra attributes on parameter, ignoring";
_tag_dynforward(_tmp63B,sizeof(char),_get_zero_arr_size(_tmp63B,40));}),
_tag_dynforward(_tmp63A,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp63C;(_tmp63C.YY37).tag=41;(_tmp63C.YY37).f1=({struct
_tuple2*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=_tmp639;_tmp63D->f2=
_tmp633;_tmp63D->f3=_tmp634;_tmp63D;});_tmp63C;});break;}}}}case 222: _LL2DC: {
struct _tuple16 _tmp640;struct Cyc_Absyn_Tqual _tmp641;struct Cyc_List_List*_tmp642;
struct Cyc_List_List*_tmp643;struct _tuple16*_tmp63F=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp640=*_tmp63F;_tmp641=
_tmp640.f1;_tmp642=_tmp640.f2;_tmp643=_tmp640.f3;if(_tmp641.loc == 0)_tmp641.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp644=Cyc_Parse_speclist2typ(_tmp642,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp643 != 0)({
void*_tmp645[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp646="bad attributes on parameter, ignoring";_tag_dynforward(_tmp646,sizeof(
char),_get_zero_arr_size(_tmp646,38));}),_tag_dynforward(_tmp645,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp647;(_tmp647.YY37).tag=
41;(_tmp647.YY37).f1=({struct _tuple2*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=
0;_tmp648->f2=_tmp641;_tmp648->f3=_tmp644;_tmp648;});_tmp647;});break;}}case 223:
_LL2DD: {struct _tuple16 _tmp64A;struct Cyc_Absyn_Tqual _tmp64B;struct Cyc_List_List*
_tmp64C;struct Cyc_List_List*_tmp64D;struct _tuple16*_tmp649=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64A=*_tmp649;_tmp64B=
_tmp64A.f1;_tmp64C=_tmp64A.f2;_tmp64D=_tmp64A.f3;if(_tmp64B.loc == 0)_tmp64B.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp64E=Cyc_Parse_speclist2typ(_tmp64C,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp64F=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp651;void*_tmp652;struct Cyc_List_List*_tmp653;struct Cyc_List_List*
_tmp654;struct _tuple6 _tmp650=Cyc_Parse_apply_tms(_tmp64B,_tmp64E,_tmp64D,_tmp64F);
_tmp651=_tmp650.f1;_tmp652=_tmp650.f2;_tmp653=_tmp650.f3;_tmp654=_tmp650.f4;if(
_tmp653 != 0)({void*_tmp655[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp656="bad type parameters on formal argument, ignoring";_tag_dynforward(
_tmp656,sizeof(char),_get_zero_arr_size(_tmp656,49));}),_tag_dynforward(_tmp655,
sizeof(void*),0));});if(_tmp654 != 0)({void*_tmp657[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp658="bad attributes on parameter, ignoring";_tag_dynforward(_tmp658,sizeof(
char),_get_zero_arr_size(_tmp658,38));}),_tag_dynforward(_tmp657,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp659;(_tmp659.YY37).tag=
41;(_tmp659.YY37).f1=({struct _tuple2*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->f1=
0;_tmp65A->f2=_tmp651;_tmp65A->f3=_tmp652;_tmp65A;});_tmp659;});break;}}case 224:
_LL2DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp65B;(_tmp65B.YY36).tag=
40;(_tmp65B.YY36).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp65B;});break;case 225: _LL2DF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp65C;(_tmp65C.YY36).tag=40;(_tmp65C.YY36).f1=({struct Cyc_List_List*_tmp65D=
_cycalloc(sizeof(*_tmp65D));_tmp65D->hd=({struct _dynforward_ptr*_tmp65E=
_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65E;});_tmp65D->tl=0;
_tmp65D;});_tmp65C;});break;case 226: _LL2E0: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp65F;(_tmp65F.YY36).tag=40;(_tmp65F.YY36).f1=({struct Cyc_List_List*
_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->hd=({struct _dynforward_ptr*_tmp661=
_cycalloc(sizeof(*_tmp661));_tmp661[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp661;});_tmp660->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp660;});_tmp65F;});
break;case 227: _LL2E1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 228: _LL2E2: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 229: _LL2E3: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp662;(_tmp662.YY3).tag=7;(_tmp662.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp663=_cycalloc(sizeof(*_tmp663));
_tmp663[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp664;_tmp664.tag=36;
_tmp664.f1=0;_tmp664.f2=0;_tmp664;});_tmp663;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp662;});
break;case 230: _LL2E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp665;(_tmp665.YY3).tag=7;(_tmp665.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp667;_tmp667.tag=36;_tmp667.f1=0;
_tmp667.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp667;});_tmp666;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp665;});
break;case 231: _LL2E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp668;(_tmp668.YY3).tag=7;(_tmp668.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp66A;_tmp66A.tag=36;_tmp66A.f1=0;
_tmp66A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp66A;});_tmp669;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp668;});
break;case 232: _LL2E6: {struct Cyc_Absyn_Vardecl*_tmp66B=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp671;(_tmp671.Loc_n).tag=0;_tmp671;});_tmp66F->f2=({
struct _dynforward_ptr*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp670;});_tmp66F;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp66B->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp66C;(_tmp66C.YY3).tag=
7;(_tmp66C.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp66E;_tmp66E.tag=29;_tmp66E.f1=_tmp66B;_tmp66E.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp66E.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66E.f4=0;
_tmp66E;});_tmp66D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp66C;});
break;}case 233: _LL2E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp672;(_tmp672.YY6).tag=10;(_tmp672.YY6).f1=({struct Cyc_List_List*_tmp673=
_cycalloc(sizeof(*_tmp673));_tmp673->hd=({struct _tuple20*_tmp674=_cycalloc(
sizeof(*_tmp674));_tmp674->f1=0;_tmp674->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp674;});_tmp673->tl=0;
_tmp673;});_tmp672;});break;case 234: _LL2E8: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp675;(_tmp675.YY6).tag=10;(_tmp675.YY6).f1=({struct Cyc_List_List*
_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->hd=({struct _tuple20*_tmp677=
_cycalloc(sizeof(*_tmp677));_tmp677->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp677->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp677;});_tmp676->tl=
0;_tmp676;});_tmp675;});break;case 235: _LL2E9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp678;(_tmp678.YY6).tag=10;(_tmp678.YY6).f1=({struct Cyc_List_List*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->hd=({struct _tuple20*_tmp67A=
_cycalloc(sizeof(*_tmp67A));_tmp67A->f1=0;_tmp67A->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67A;});_tmp679->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp679;});_tmp678;});
break;case 236: _LL2EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp67B;(_tmp67B.YY6).tag=10;(_tmp67B.YY6).f1=({struct Cyc_List_List*_tmp67C=
_cycalloc(sizeof(*_tmp67C));_tmp67C->hd=({struct _tuple20*_tmp67D=_cycalloc(
sizeof(*_tmp67D));_tmp67D->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp67D->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67D;});_tmp67C->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp67C;});_tmp67B;});
break;case 237: _LL2EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp67E;(_tmp67E.YY41).tag=45;(_tmp67E.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp67E;});break;
case 238: _LL2EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp67F;(
_tmp67F.YY41).tag=45;(_tmp67F.YY41).f1=({struct Cyc_List_List*_tmp680=_cycalloc(
sizeof(*_tmp680));_tmp680->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp680->tl=0;_tmp680;});
_tmp67F;});break;case 239: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp681;(_tmp681.YY41).tag=45;(_tmp681.YY41).f1=({struct Cyc_List_List*_tmp682=
_cycalloc(sizeof(*_tmp682));_tmp682->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp682->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp682;});_tmp681;});
break;case 240: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp683;(_tmp683.YY42).tag=46;(_tmp683.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp685;_tmp685.tag=0;_tmp685.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp685;});_tmp684;}));
_tmp683;});break;case 241: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp686;(_tmp686.YY42).tag=46;(_tmp686.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_Absyn_FieldName_struct
_tmp688;_tmp688.tag=1;_tmp688.f1=({struct _dynforward_ptr*_tmp689=_cycalloc(
sizeof(*_tmp689));_tmp689[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp689;});_tmp688;});
_tmp687;}));_tmp686;});break;case 242: _LL2F0: {struct _tuple16 _tmp68B;struct Cyc_Absyn_Tqual
_tmp68C;struct Cyc_List_List*_tmp68D;struct Cyc_List_List*_tmp68E;struct _tuple16*
_tmp68A=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp68B=*_tmp68A;_tmp68C=_tmp68B.f1;_tmp68D=_tmp68B.f2;_tmp68E=_tmp68B.f3;{void*
_tmp68F=Cyc_Parse_speclist2typ(_tmp68D,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp68E != 0)({
void*_tmp690[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp691="ignoring attributes in type";_tag_dynforward(_tmp691,sizeof(char),
_get_zero_arr_size(_tmp691,28));}),_tag_dynforward(_tmp690,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp692;(_tmp692.YY37).tag=
41;(_tmp692.YY37).f1=({struct _tuple2*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->f1=
0;_tmp693->f2=_tmp68C;_tmp693->f3=_tmp68F;_tmp693;});_tmp692;});break;}}case 243:
_LL2F1: {struct _tuple16 _tmp695;struct Cyc_Absyn_Tqual _tmp696;struct Cyc_List_List*
_tmp697;struct Cyc_List_List*_tmp698;struct _tuple16*_tmp694=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp695=*_tmp694;_tmp696=
_tmp695.f1;_tmp697=_tmp695.f2;_tmp698=_tmp695.f3;{void*_tmp699=Cyc_Parse_speclist2typ(
_tmp697,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp69A=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp69B=
Cyc_Parse_apply_tms(_tmp696,_tmp699,_tmp698,_tmp69A);if(_tmp69B.f3 != 0)({void*
_tmp69C[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp69D="bad type params, ignoring";_tag_dynforward(_tmp69D,sizeof(char),
_get_zero_arr_size(_tmp69D,26));}),_tag_dynforward(_tmp69C,sizeof(void*),0));});
if(_tmp69B.f4 != 0)({void*_tmp69E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp69F="bad specifiers, ignoring";_tag_dynforward(_tmp69F,sizeof(char),
_get_zero_arr_size(_tmp69F,25));}),_tag_dynforward(_tmp69E,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A0;(_tmp6A0.YY37).tag=
41;(_tmp6A0.YY37).f1=({struct _tuple2*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->f1=
0;_tmp6A1->f2=_tmp69B.f1;_tmp6A1->f3=_tmp69B.f2;_tmp6A1;});_tmp6A0;});break;}}
case 244: _LL2F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A2;(
_tmp6A2.YY44).tag=48;(_tmp6A2.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6A2;});break;case
245: _LL2F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A3;(
_tmp6A3.YY44).tag=48;(_tmp6A3.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6A5;_tmp6A5.tag=21;_tmp6A5.f1=0;_tmp6A5;});_tmp6A4;}));_tmp6A3;});break;case
246: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A6;(
_tmp6A6.YY44).tag=48;(_tmp6A6.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6A8;_tmp6A8.tag=21;_tmp6A8.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A8;});_tmp6A7;}));
_tmp6A6;});break;case 247: _LL2F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A9;(_tmp6A9.YY44).tag=48;(_tmp6A9.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6AB;_tmp6AB.tag=22;_tmp6AB.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AB;});_tmp6AA;}));
_tmp6A9;});break;case 248: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AC;(_tmp6AC.YY44).tag=48;(_tmp6AC.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6AE;_tmp6AE.tag=21;_tmp6AE.f1=({struct Cyc_List_List*_tmp6AF=_cycalloc(
sizeof(*_tmp6AF));_tmp6AF->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AF->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AF;});_tmp6AE;});
_tmp6AD;}));_tmp6AC;});break;case 249: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6B0;(_tmp6B0.YY40).tag=44;(_tmp6B0.YY40).f1=({struct
Cyc_List_List*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B1->tl=0;_tmp6B1;});
_tmp6B0;});break;case 250: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B2;(_tmp6B2.YY40).tag=44;(_tmp6B2.YY40).f1=({struct Cyc_List_List*_tmp6B3=
_cycalloc(sizeof(*_tmp6B3));_tmp6B3->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B3->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B3;});_tmp6B2;});
break;case 251: _LL2F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B4;(_tmp6B4.YY30).tag=34;(_tmp6B4.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B5;});_tmp6B4;});
break;case 252: _LL2FA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 253: _LL2FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B6;(_tmp6B6.YY30).tag=34;(_tmp6B6.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6B7;});
_tmp6B6;});break;case 254: _LL2FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 255: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B8;(_tmp6B8.YY30).tag=34;(_tmp6B8.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tms=({struct Cyc_List_List*_tmp6BA=
_cycalloc(sizeof(*_tmp6BA));_tmp6BA->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6BC;_tmp6BC.tag=0;_tmp6BC.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6BC;});
_tmp6BB;}));_tmp6BA->tl=0;_tmp6BA;});_tmp6B9;});_tmp6B8;});break;case 256: _LL2FE:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6BD;(_tmp6BD.YY30).tag=
34;(_tmp6BD.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6BE=_cycalloc(
sizeof(*_tmp6BE));_tmp6BE->tms=({struct Cyc_List_List*_tmp6BF=_cycalloc(sizeof(*
_tmp6BF));_tmp6BF->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6C0=
_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C1;_tmp6C1.tag=0;_tmp6C1.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C1.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6C1;});
_tmp6C0;}));_tmp6BF->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6BF;});_tmp6BE;});_tmp6BD;});break;case 257:
_LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C2;(_tmp6C2.YY30).tag=
34;(_tmp6C2.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C3=_cycalloc(
sizeof(*_tmp6C3));_tmp6C3->tms=({struct Cyc_List_List*_tmp6C4=_cycalloc(sizeof(*
_tmp6C4));_tmp6C4->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6C6;_tmp6C6.tag=1;_tmp6C6.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C6.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C6.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6C6;});
_tmp6C5;}));_tmp6C4->tl=0;_tmp6C4;});_tmp6C3;});_tmp6C2;});break;case 258: _LL300:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C7;(_tmp6C7.YY30).tag=
34;(_tmp6C7.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C8=_cycalloc(
sizeof(*_tmp6C8));_tmp6C8->tms=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*
_tmp6C9));_tmp6C9->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6CB;_tmp6CB.tag=1;_tmp6CB.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CB.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CB.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6CB;});
_tmp6CA;}));_tmp6C9->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6C9;});_tmp6C8;});_tmp6C7;});break;case 259:
_LL301: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6CC;(_tmp6CC.YY30).tag=
34;(_tmp6CC.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6CD=_cycalloc(
sizeof(*_tmp6CD));_tmp6CD->tms=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*
_tmp6CE));_tmp6CE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D0;_tmp6D0.tag=3;_tmp6D0.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D2;_tmp6D2.tag=1;_tmp6D2.f1=0;_tmp6D2.f2=0;_tmp6D2.f3=0;_tmp6D2.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D2.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D2;});_tmp6D1;}));
_tmp6D0;});_tmp6CF;}));_tmp6CE->tl=0;_tmp6CE;});_tmp6CD;});_tmp6CC;});break;case
260: _LL302: {struct _tuple17 _tmp6D4;struct Cyc_List_List*_tmp6D5;int _tmp6D6;struct
Cyc_Absyn_VarargInfo*_tmp6D7;struct Cyc_Core_Opt*_tmp6D8;struct Cyc_List_List*
_tmp6D9;struct _tuple17*_tmp6D3=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D4=*_tmp6D3;_tmp6D5=
_tmp6D4.f1;_tmp6D6=_tmp6D4.f2;_tmp6D7=_tmp6D4.f3;_tmp6D8=_tmp6D4.f4;_tmp6D9=
_tmp6D4.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6DA;(
_tmp6DA.YY30).tag=34;(_tmp6DA.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB->tms=({struct Cyc_List_List*_tmp6DC=
_cycalloc(sizeof(*_tmp6DC));_tmp6DC->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6DE;_tmp6DE.tag=3;_tmp6DE.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E0;_tmp6E0.tag=1;_tmp6E0.f1=_tmp6D5;_tmp6E0.f2=_tmp6D6;_tmp6E0.f3=_tmp6D7;
_tmp6E0.f4=_tmp6D8;_tmp6E0.f5=_tmp6D9;_tmp6E0;});_tmp6DF;}));_tmp6DE;});_tmp6DD;}));
_tmp6DC->tl=0;_tmp6DC;});_tmp6DB;});_tmp6DA;});break;}case 261: _LL303: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6E1;(_tmp6E1.YY30).tag=34;(
_tmp6E1.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6E2=_cycalloc(sizeof(*
_tmp6E2));_tmp6E2->tms=({struct Cyc_List_List*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));
_tmp6E3->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6E4=
_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E5;_tmp6E5.tag=3;_tmp6E5.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E7;_tmp6E7.tag=1;_tmp6E7.f1=0;_tmp6E7.f2=0;_tmp6E7.f3=0;_tmp6E7.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E7.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E7;});_tmp6E6;}));
_tmp6E5;});_tmp6E4;}));_tmp6E3->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6E3;});_tmp6E2;});
_tmp6E1;});break;case 262: _LL304: {struct _tuple17 _tmp6E9;struct Cyc_List_List*
_tmp6EA;int _tmp6EB;struct Cyc_Absyn_VarargInfo*_tmp6EC;struct Cyc_Core_Opt*_tmp6ED;
struct Cyc_List_List*_tmp6EE;struct _tuple17*_tmp6E8=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E9=*_tmp6E8;_tmp6EA=
_tmp6E9.f1;_tmp6EB=_tmp6E9.f2;_tmp6EC=_tmp6E9.f3;_tmp6ED=_tmp6E9.f4;_tmp6EE=
_tmp6E9.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6EF;(
_tmp6EF.YY30).tag=34;(_tmp6EF.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0->tms=({struct Cyc_List_List*_tmp6F1=
_cycalloc(sizeof(*_tmp6F1));_tmp6F1->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6F3;_tmp6F3.tag=3;_tmp6F3.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F5;_tmp6F5.tag=1;_tmp6F5.f1=_tmp6EA;_tmp6F5.f2=_tmp6EB;_tmp6F5.f3=_tmp6EC;
_tmp6F5.f4=_tmp6ED;_tmp6F5.f5=_tmp6EE;_tmp6F5;});_tmp6F4;}));_tmp6F3;});_tmp6F2;}));
_tmp6F1->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp6F1;});_tmp6F0;});_tmp6EF;});break;}case 263: _LL305: {
struct Cyc_List_List*_tmp6F6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F7;(_tmp6F7.YY30).tag=
34;(_tmp6F7.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F8=_cycalloc(
sizeof(*_tmp6F8));_tmp6F8->tms=({struct Cyc_List_List*_tmp6F9=_cycalloc(sizeof(*
_tmp6F9));_tmp6F9->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6FB;_tmp6FB.tag=4;_tmp6FB.f1=_tmp6F6;_tmp6FB.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FB.f3=0;
_tmp6FB;});_tmp6FA;}));_tmp6F9->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6F9;});_tmp6F8;});
_tmp6F7;});break;}case 264: _LL306: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6FC;(_tmp6FC.YY30).tag=34;(_tmp6FC.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD->tms=({struct Cyc_List_List*_tmp6FE=
_cycalloc(sizeof(*_tmp6FE));_tmp6FE->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp700;_tmp700.tag=5;_tmp700.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp700.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp700;});_tmp6FF;}));
_tmp6FE->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp6FE;});_tmp6FD;});_tmp6FC;});break;case 265: _LL307:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 266:
_LL308: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
267: _LL309: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 268: _LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 269: _LL30B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 270: _LL30C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 271: _LL30D: if(Cyc_zstrcmp((struct _dynforward_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp701="`H";_tag_dynforward(_tmp701,sizeof(char),_get_zero_arr_size(
_tmp701,3));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp702="`U";
_tag_dynforward(_tmp702,sizeof(char),_get_zero_arr_size(_tmp702,3));}))== 0)Cyc_Parse_err((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp705;_tmp705.tag=0;_tmp705.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp703[1]={&
_tmp705};Cyc_aprintf(({const char*_tmp704="bad occurrence of heap region %s";
_tag_dynforward(_tmp704,sizeof(char),_get_zero_arr_size(_tmp704,33));}),
_tag_dynforward(_tmp703,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->name=({
struct _dynforward_ptr*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp713;});_tmp710->identity=
0;_tmp710->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp711=_cycalloc(
sizeof(*_tmp711));_tmp711[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp712;_tmp712.tag=0;
_tmp712.f1=(void*)((void*)3);_tmp712;});_tmp711;}));_tmp710;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({
struct Cyc_Absyn_VarType_struct _tmp70F;_tmp70F.tag=1;_tmp70F.f1=tv;_tmp70F;});
_tmp70E;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp706;(
_tmp706.YY9).tag=13;(_tmp706.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_Region_s_struct
_tmp708;_tmp708.tag=15;_tmp708.f1=tv;_tmp708.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp70B;(_tmp70B.Loc_n).tag=0;_tmp70B;});_tmp709->f2=({
struct _dynforward_ptr*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70A;});_tmp709;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp70C=_cycalloc(sizeof(*_tmp70C));
_tmp70C[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp70D;_tmp70D.tag=15;_tmp70D.f1=(
void*)t;_tmp70D;});_tmp70C;}),0);_tmp708.f3=0;_tmp708.f4=0;_tmp708.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp708;});_tmp707;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp706;});break;}case 272: _LL30E: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp714="H";_tag_dynforward(_tmp714,sizeof(
char),_get_zero_arr_size(_tmp714,2));}))== 0)Cyc_Parse_err(({const char*_tmp715="bad occurrence of heap region `H";
_tag_dynforward(_tmp715,sizeof(char),_get_zero_arr_size(_tmp715,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->name=({
struct _dynforward_ptr*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp726;_tmp726.tag=0;_tmp726.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp724[1]={&
_tmp726};Cyc_aprintf(({const char*_tmp725="`%s";_tag_dynforward(_tmp725,sizeof(
char),_get_zero_arr_size(_tmp725,4));}),_tag_dynforward(_tmp724,sizeof(void*),1));}});
_tmp723;});_tmp720->identity=0;_tmp720->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp722;_tmp722.tag=0;_tmp722.f1=(void*)((void*)3);_tmp722;});_tmp721;}));
_tmp720;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp71E=_cycalloc(
sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Absyn_VarType_struct _tmp71F;_tmp71F.tag=
1;_tmp71F.f1=tv;_tmp71F;});_tmp71E;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp716;(_tmp716.YY9).tag=13;(_tmp716.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=({
struct Cyc_Absyn_Region_s_struct _tmp718;_tmp718.tag=15;_tmp718.f1=tv;_tmp718.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp719=_cycalloc(sizeof(*_tmp719));
_tmp719->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp71B;(
_tmp71B.Loc_n).tag=0;_tmp71B;});_tmp719->f2=({struct _dynforward_ptr*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71A;});_tmp719;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp71C=_cycalloc(sizeof(*_tmp71C));
_tmp71C[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp71D;_tmp71D.tag=15;_tmp71D.f1=(
void*)t;_tmp71D;});_tmp71C;}),0);_tmp718.f3=0;_tmp718.f4=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp718.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp718;});_tmp717;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp716;});break;}case 273: _LL30F: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 6)]),({const char*_tmp727="resetable";_tag_dynforward(_tmp727,
sizeof(char),_get_zero_arr_size(_tmp727,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp728="expecting [resetable]";_tag_dynforward(_tmp728,sizeof(char),
_get_zero_arr_size(_tmp728,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp729="`H";_tag_dynforward(_tmp729,
sizeof(char),_get_zero_arr_size(_tmp729,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp72A="`U";_tag_dynforward(_tmp72A,sizeof(char),
_get_zero_arr_size(_tmp72A,3));})))Cyc_Parse_err((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp72D;_tmp72D.tag=0;_tmp72D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{void*_tmp72B[1]={& _tmp72D};Cyc_aprintf(({const char*
_tmp72C="bad occurrence of heap region %s";_tag_dynforward(_tmp72C,sizeof(char),
_get_zero_arr_size(_tmp72C,33));}),_tag_dynforward(_tmp72B,sizeof(void*),1));}}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738->name=({struct _dynforward_ptr*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp73B;});_tmp738->identity=
0;_tmp738->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp739=_cycalloc(
sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp73A;_tmp73A.tag=0;
_tmp73A.f1=(void*)((void*)3);_tmp73A;});_tmp739;}));_tmp738;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736[0]=({
struct Cyc_Absyn_VarType_struct _tmp737;_tmp737.tag=1;_tmp737.f1=tv;_tmp737;});
_tmp736;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp72E;(
_tmp72E.YY9).tag=13;(_tmp72E.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_Region_s_struct
_tmp730;_tmp730.tag=15;_tmp730.f1=tv;_tmp730.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp733;(_tmp733.Loc_n).tag=0;_tmp733;});_tmp731->f2=({
struct _dynforward_ptr*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp732;});_tmp731;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp734=_cycalloc(sizeof(*_tmp734));
_tmp734[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp735;_tmp735.tag=15;_tmp735.f1=(
void*)t;_tmp735;});_tmp734;}),0);_tmp730.f3=1;_tmp730.f4=0;_tmp730.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp730;});_tmp72F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp72E;});break;}case 274: _LL310: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp73C="resetable";_tag_dynforward(_tmp73C,
sizeof(char),_get_zero_arr_size(_tmp73C,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp73D="expecting `resetable'";_tag_dynforward(_tmp73D,sizeof(char),
_get_zero_arr_size(_tmp73D,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp73E="H";_tag_dynforward(_tmp73E,sizeof(
char),_get_zero_arr_size(_tmp73E,2));}))== 0)Cyc_Parse_err(({const char*_tmp73F="bad occurrence of heap region `H";
_tag_dynforward(_tmp73F,sizeof(char),_get_zero_arr_size(_tmp73F,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->name=({
struct _dynforward_ptr*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp750;_tmp750.tag=0;_tmp750.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp74E[1]={&
_tmp750};Cyc_aprintf(({const char*_tmp74F="`%s";_tag_dynforward(_tmp74F,sizeof(
char),_get_zero_arr_size(_tmp74F,4));}),_tag_dynforward(_tmp74E,sizeof(void*),1));}});
_tmp74D;});_tmp74A->identity=0;_tmp74A->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp74C;_tmp74C.tag=0;_tmp74C.f1=(void*)((void*)3);_tmp74C;});_tmp74B;}));
_tmp74A;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp748=_cycalloc(
sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Absyn_VarType_struct _tmp749;_tmp749.tag=
1;_tmp749.f1=tv;_tmp749;});_tmp748;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp740;(_tmp740.YY9).tag=13;(_tmp740.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({
struct Cyc_Absyn_Region_s_struct _tmp742;_tmp742.tag=15;_tmp742.f1=tv;_tmp742.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp743=_cycalloc(sizeof(*_tmp743));
_tmp743->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp745;(
_tmp745.Loc_n).tag=0;_tmp745;});_tmp743->f2=({struct _dynforward_ptr*_tmp744=
_cycalloc(sizeof(*_tmp744));_tmp744[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp744;});_tmp743;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp746=_cycalloc(sizeof(*_tmp746));
_tmp746[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp747;_tmp747.tag=15;_tmp747.f1=(
void*)t;_tmp747;});_tmp746;}),0);_tmp742.f3=1;_tmp742.f4=0;_tmp742.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp742;});_tmp741;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp740;});break;}case 275: _LL311: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp751;(_tmp751.YY9).tag=13;(_tmp751.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp752=_cycalloc(sizeof(*_tmp752));
_tmp752[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp753;_tmp753.tag=16;_tmp753.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp753;});_tmp752;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp751;});
break;case 276: _LL312: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp754;(_tmp754.YY4).tag=8;(_tmp754.YY4).f1=0;_tmp754;});break;case 277: _LL313:
if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp755="open";
_tag_dynforward(_tmp755,sizeof(char),_get_zero_arr_size(_tmp755,5));}))!= 0)Cyc_Parse_err(({
const char*_tmp756="expecting `open'";_tag_dynforward(_tmp756,sizeof(char),
_get_zero_arr_size(_tmp756,17));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp757;(_tmp757.YY4).tag=8;(_tmp757.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp757;});break;case 278: _LL314: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp758;(_tmp758.YY9).tag=13;(_tmp758.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp759=_cycalloc(sizeof(*_tmp759));
_tmp759[0]=({struct Cyc_Absyn_Label_s_struct _tmp75A;_tmp75A.tag=12;_tmp75A.f1=({
struct _dynforward_ptr*_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75B;});_tmp75A.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75A;});
_tmp759;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp758;});break;case 279: _LL315: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75C;(_tmp75C.YY9).tag=13;(_tmp75C.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75C;});break;case 280: _LL316: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75D;(_tmp75D.YY9).tag=13;(_tmp75D.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75D;});
break;case 281: _LL317: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp75E;(_tmp75E.YY9).tag=13;(_tmp75E.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75E;});
break;case 282: _LL318: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 283: _LL319: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp75F;(_tmp75F.YY9).tag=13;(_tmp75F.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp75F;});break;case 284: _LL31A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp760;(_tmp760.YY9).tag=13;(_tmp760.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp760;});break;case 285:
_LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
286: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp761;(
_tmp761.YY9).tag=13;(_tmp761.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp761;});
break;case 287: _LL31D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp762;(_tmp762.YY9).tag=13;(_tmp762.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp764;_tmp764.tag=1;_tmp764.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp764;});_tmp763;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp762;});break;case 288:
_LL31E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp765;(_tmp765.YY9).tag=
13;(_tmp765.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp767;_tmp767.tag=1;_tmp767.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp767;});_tmp766;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp765;});break;case 289: _LL31F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp768;(_tmp768.YY9).tag=13;(_tmp768.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp768;});break;case 290: _LL320: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp769;(_tmp769.YY9).tag=13;(_tmp769.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp769;});
break;case 291: _LL321: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76A;(_tmp76A.YY9).tag=13;(_tmp76A.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76A;});
break;case 292: _LL322: {struct Cyc_Absyn_Exp*_tmp76B=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp76E;_tmp76E.tag=2;_tmp76E.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp76E;});_tmp76D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76C;(_tmp76C.YY9).tag=13;(_tmp76C.YY9).f1=Cyc_Absyn_switch_stmt(_tmp76B,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76C;});
break;}case 293: _LL323: {struct Cyc_Absyn_Exp*_tmp76F=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp770;(_tmp770.YY9).tag=13;(_tmp770.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp76F,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp770;});
break;}case 294: _LL324: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp771;(_tmp771.YY9).tag=13;(_tmp771.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp771;});
break;case 295: _LL325: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp772;(_tmp772.YY10).tag=14;(_tmp772.YY10).f1=0;_tmp772;});break;case 296:
_LL326: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp773;(_tmp773.YY10).tag=
14;(_tmp773.YY10).f1=({struct Cyc_List_List*_tmp774=_cycalloc(sizeof(*_tmp774));
_tmp774->hd=({struct Cyc_Absyn_Switch_clause*_tmp775=_cycalloc(sizeof(*_tmp775));
_tmp775->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp775->pat_vars=
0;_tmp775->where_clause=0;_tmp775->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp775->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp775;});
_tmp774->tl=0;_tmp774;});_tmp773;});break;case 297: _LL327: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp776;(_tmp776.YY10).tag=14;(_tmp776.YY10).f1=({struct
Cyc_List_List*_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777->hd=({struct Cyc_Absyn_Switch_clause*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp778->pat_vars=0;
_tmp778->where_clause=0;_tmp778->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp778->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp778;});_tmp777->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp777;});_tmp776;});
break;case 298: _LL328: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp779;(_tmp779.YY10).tag=14;(_tmp779.YY10).f1=({struct Cyc_List_List*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A->hd=({struct Cyc_Absyn_Switch_clause*_tmp77B=
_cycalloc(sizeof(*_tmp77B));_tmp77B->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp77B->pat_vars=0;
_tmp77B->where_clause=0;_tmp77B->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp77B->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp77B;});
_tmp77A->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp77A;});_tmp779;});break;case 299: _LL329: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77C;(_tmp77C.YY10).tag=14;(_tmp77C.YY10).f1=({struct
Cyc_List_List*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->hd=({struct Cyc_Absyn_Switch_clause*
_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp77E->pat_vars=0;
_tmp77E->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77E->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp77E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp77E;});
_tmp77D->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp77D;});_tmp77C;});break;case 300: _LL32A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77F;(_tmp77F.YY10).tag=14;(_tmp77F.YY10).f1=({struct
Cyc_List_List*_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780->hd=({struct Cyc_Absyn_Switch_clause*
_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp781->pat_vars=0;
_tmp781->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp781->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp781->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp781;});
_tmp780->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp780;});_tmp77F;});break;case 301: _LL32B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp782;(_tmp782.YY9).tag=13;(_tmp782.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp782;});
break;case 302: _LL32C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp783;(_tmp783.YY9).tag=13;(_tmp783.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp783;});
break;case 303: _LL32D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp784;(_tmp784.YY9).tag=13;(_tmp784.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp784;});
break;case 304: _LL32E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp785;(_tmp785.YY9).tag=13;(_tmp785.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp785;});break;case 305: _LL32F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp786;(_tmp786.YY9).tag=13;(_tmp786.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp786;});
break;case 306: _LL330: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp787;(_tmp787.YY9).tag=13;(_tmp787.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp787;});
break;case 307: _LL331: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp788;(_tmp788.YY9).tag=13;(_tmp788.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp788;});
break;case 308: _LL332: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp789;(_tmp789.YY9).tag=13;(_tmp789.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp789;});
break;case 309: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78A;(_tmp78A.YY9).tag=13;(_tmp78A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78A;});
break;case 310: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78B;(_tmp78B.YY9).tag=13;(_tmp78B.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78B;});
break;case 311: _LL335: {struct Cyc_List_List*_tmp78C=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp78D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78E;(_tmp78E.YY9).tag=13;(_tmp78E.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp78C,_tmp78D);_tmp78E;});break;}case 312: _LL336: {struct Cyc_List_List*_tmp78F=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp790=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp791;(_tmp791.YY9).tag=13;(_tmp791.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp78F,_tmp790);_tmp791;});break;}case 313: _LL337: {struct Cyc_List_List*_tmp792=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp793=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp794;(_tmp794.YY9).tag=13;(_tmp794.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp792,_tmp793);_tmp794;});break;}case 314: _LL338: {struct Cyc_List_List*_tmp795=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp796=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp797;(_tmp797.YY9).tag=13;(_tmp797.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp795,_tmp796);_tmp797;});break;}case 315: _LL339: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp798;(_tmp798.YY9).tag=13;(_tmp798.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dynforward_ptr*_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp799;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp798;});
break;case 316: _LL33A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79A;(_tmp79A.YY9).tag=13;(_tmp79A.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79A;});
break;case 317: _LL33B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79B;(_tmp79B.YY9).tag=13;(_tmp79B.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79B;});
break;case 318: _LL33C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79C;(_tmp79C.YY9).tag=13;(_tmp79C.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79C;});
break;case 319: _LL33D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79D;(_tmp79D.YY9).tag=13;(_tmp79D.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79D;});
break;case 320: _LL33E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79E;(_tmp79E.YY9).tag=13;(_tmp79E.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79E;});
break;case 321: _LL33F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79F;(_tmp79F.YY9).tag=13;(_tmp79F.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp79F;});
break;case 322: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A0;(_tmp7A0.YY9).tag=13;(_tmp7A0.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp7A0;});
break;case 323: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 324: _LL342: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 325: _LL343: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7A1;(_tmp7A1.YY11).tag=15;(_tmp7A1.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A1;});
break;case 326: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL345: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A2;(_tmp7A2.YY11).tag=15;(_tmp7A2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A2;});break;case 328: _LL346: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 329: _LL347: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7A3;(_tmp7A3.YY11).tag=15;(
_tmp7A3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A3;});
break;case 330: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL349: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A4;(_tmp7A4.YY11).tag=15;(_tmp7A4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A4;});
break;case 332: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL34B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A5;(_tmp7A5.YY11).tag=15;(_tmp7A5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A5;});
break;case 334: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL34D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A6;(_tmp7A6.YY11).tag=15;(_tmp7A6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A6;});
break;case 336: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 337: _LL34F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A7;(_tmp7A7.YY11).tag=15;(_tmp7A7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A7;});break;case 338: _LL350: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A8;(_tmp7A8.YY11).tag=15;(_tmp7A8.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A8;});
break;case 339: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 340: _LL352: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A9;(_tmp7A9.YY11).tag=15;(_tmp7A9.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A9;});break;case 341: _LL353: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7AA;(_tmp7AA.YY11).tag=15;(_tmp7AA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AA;});
break;case 342: _LL354: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AB;(_tmp7AB.YY11).tag=15;(_tmp7AB.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7AB;});break;case 343: _LL355: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7AC;(_tmp7AC.YY11).tag=15;(_tmp7AC.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AC;});
break;case 344: _LL356: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 345: _LL357: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AD;(_tmp7AD.YY11).tag=15;(_tmp7AD.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AD;});
break;case 346: _LL358: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AE;(_tmp7AE.YY11).tag=15;(_tmp7AE.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AE;});
break;case 347: _LL359: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 348: _LL35A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AF;(_tmp7AF.YY11).tag=15;(_tmp7AF.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AF;});
break;case 349: _LL35B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B0;(_tmp7B0.YY11).tag=15;(_tmp7B0.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B0;});
break;case 350: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 351: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B1;(_tmp7B1.YY11).tag=15;(_tmp7B1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B1;});
break;case 352: _LL35E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B2;(_tmp7B2.YY11).tag=15;(_tmp7B2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B2;});
break;case 353: _LL35F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B3;(_tmp7B3.YY11).tag=15;(_tmp7B3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B3;});
break;case 354: _LL360: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL361: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B4;(_tmp7B4.YY11).tag=15;(_tmp7B4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B4;});break;case 356: _LL362: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 357: _LL363: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B5;(_tmp7B5.YY11).tag=15;(
_tmp7B5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7B5;});break;case 358:
_LL364: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B6;(_tmp7B6.YY11).tag=
15;(_tmp7B6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B6;});
break;case 359: _LL365: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B7;(_tmp7B7.YY11).tag=15;(_tmp7B7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B7;});break;case 360: _LL366: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B8;(_tmp7B8.YY11).tag=15;(_tmp7B8.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B8;});break;case 361: _LL367: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B9;(_tmp7B9.YY11).tag=15;(_tmp7B9.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp7BA=
_cycalloc(sizeof(*_tmp7BA));_tmp7BA[0]=({struct Cyc_Absyn_StructField_struct
_tmp7BB;_tmp7BB.tag=0;_tmp7BB.f1=({struct _dynforward_ptr*_tmp7BC=_cycalloc(
sizeof(*_tmp7BC));_tmp7BC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7BC;});_tmp7BB;});
_tmp7BA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B9;});break;case 362: _LL368: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7BD;(_tmp7BD.YY11).tag=15;(_tmp7BD.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7BE=
_cycalloc_atomic(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp7BF;_tmp7BF.tag=1;_tmp7BF.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7BF;});_tmp7BE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7BD;});break;case 363: _LL369: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 364: _LL36A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 365: _LL36B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C0;(_tmp7C0.YY11).tag=
15;(_tmp7C0.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C0;});
break;case 366: _LL36C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7C1;(_tmp7C1.YY11).tag=15;(_tmp7C1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7C1;});break;
case 367: _LL36D: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7C2=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7C3;void*_tmp7C4;char _tmp7C5;union Cyc_Absyn_Cnst_union
_tmp7C6;void*_tmp7C7;short _tmp7C8;union Cyc_Absyn_Cnst_union _tmp7C9;void*_tmp7CA;
int _tmp7CB;union Cyc_Absyn_Cnst_union _tmp7CC;struct _dynforward_ptr _tmp7CD;union
Cyc_Absyn_Cnst_union _tmp7CE;union Cyc_Absyn_Cnst_union _tmp7CF;union Cyc_Absyn_Cnst_union
_tmp7D0;_LL370: if(*((int*)_tmp7C2)!= 0)goto _LL372;_tmp7C3=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1).Char_c).tag != 
0)goto _LL372;_tmp7C4=(_tmp7C3.Char_c).f1;_tmp7C5=(_tmp7C3.Char_c).f2;_LL371:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7D1;(_tmp7D1.YY11).tag=
15;(_tmp7D1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7D2=_cycalloc_atomic(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7D3;_tmp7D3.tag=8;_tmp7D3.f1=_tmp7C5;_tmp7D3;});_tmp7D2;}),e->loc);_tmp7D1;});
goto _LL36F;_LL372: if(*((int*)_tmp7C2)!= 0)goto _LL374;_tmp7C6=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1).Short_c).tag != 
1)goto _LL374;_tmp7C7=(_tmp7C6.Short_c).f1;_tmp7C8=(_tmp7C6.Short_c).f2;_LL373:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7D4;(_tmp7D4.YY11).tag=
15;(_tmp7D4.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7D6;_tmp7D6.tag=7;_tmp7D6.f1=(void*)_tmp7C7;_tmp7D6.f2=(int)_tmp7C8;_tmp7D6;});
_tmp7D5;}),e->loc);_tmp7D4;});goto _LL36F;_LL374: if(*((int*)_tmp7C2)!= 0)goto
_LL376;_tmp7C9=((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7C2)->f1).Int_c).tag != 2)goto _LL376;_tmp7CA=(_tmp7C9.Int_c).f1;_tmp7CB=(
_tmp7C9.Int_c).f2;_LL375: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7D7;(_tmp7D7.YY11).tag=15;(_tmp7D7.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));_tmp7D8[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7D9;_tmp7D9.tag=7;_tmp7D9.f1=(void*)_tmp7CA;
_tmp7D9.f2=_tmp7CB;_tmp7D9;});_tmp7D8;}),e->loc);_tmp7D7;});goto _LL36F;_LL376:
if(*((int*)_tmp7C2)!= 0)goto _LL378;_tmp7CC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1).Float_c).tag != 
4)goto _LL378;_tmp7CD=(_tmp7CC.Float_c).f1;_LL377: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7DA;(_tmp7DA.YY11).tag=15;(_tmp7DA.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7DB=_cycalloc(sizeof(*_tmp7DB));
_tmp7DB[0]=({struct Cyc_Absyn_Float_p_struct _tmp7DC;_tmp7DC.tag=9;_tmp7DC.f1=
_tmp7CD;_tmp7DC;});_tmp7DB;}),e->loc);_tmp7DA;});goto _LL36F;_LL378: if(*((int*)
_tmp7C2)!= 0)goto _LL37A;_tmp7CE=((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1).Null_c).tag != 6)goto _LL37A;
_LL379: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7DD;(_tmp7DD.YY11).tag=
15;(_tmp7DD.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7DD;});goto _LL36F;
_LL37A: if(*((int*)_tmp7C2)!= 0)goto _LL37C;_tmp7CF=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1).String_c).tag
!= 5)goto _LL37C;_LL37B: Cyc_Parse_err(({const char*_tmp7DE="strings cannot occur within patterns";
_tag_dynforward(_tmp7DE,sizeof(char),_get_zero_arr_size(_tmp7DE,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL36F;
_LL37C: if(*((int*)_tmp7C2)!= 0)goto _LL37E;_tmp7D0=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C2)->f1).LongLong_c).tag
!= 3)goto _LL37E;_LL37D: Cyc_Parse_unimp(({const char*_tmp7DF="long long's in patterns";
_tag_dynforward(_tmp7DF,sizeof(char),_get_zero_arr_size(_tmp7DF,24));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL36F;
_LL37E:;_LL37F: Cyc_Parse_err(({const char*_tmp7E0="bad constant in case";
_tag_dynforward(_tmp7E0,sizeof(char),_get_zero_arr_size(_tmp7E0,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL36F:;}break;}
case 368: _LL36E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7E1;(
_tmp7E1.YY11).tag=15;(_tmp7E1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7E3;_tmp7E3.tag=12;_tmp7E3.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E3;});_tmp7E2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E1;});
break;case 369: _LL380: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7E4="as";
_tag_dynforward(_tmp7E4,sizeof(char),_get_zero_arr_size(_tmp7E4,3));}))!= 0)Cyc_Parse_err(({
const char*_tmp7E5="expecting `as'";_tag_dynforward(_tmp7E5,sizeof(char),
_get_zero_arr_size(_tmp7E5,15));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7E6;(_tmp7E6.YY11).tag=15;(_tmp7E6.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7E7=_cycalloc(
sizeof(*_tmp7E7));_tmp7E7[0]=({struct Cyc_Absyn_Var_p_struct _tmp7E8;_tmp7E8.tag=0;
_tmp7E8.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7E9=_cycalloc(sizeof(*
_tmp7E9));_tmp7E9->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7EB;(_tmp7EB.Loc_n).tag=0;_tmp7EB;});_tmp7E9->f2=({struct _dynforward_ptr*
_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7EA;});_tmp7E9;}),(
void*)0,0);_tmp7E8.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7E8;});_tmp7E7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7E6;});
break;case 370: _LL381: {struct Cyc_List_List*_tmp7ED;int _tmp7EE;struct _tuple13
_tmp7EC=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7ED=_tmp7EC.f1;_tmp7EE=_tmp7EC.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7EF;(_tmp7EF.YY11).tag=15;(_tmp7EF.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));
_tmp7F0[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7F1;_tmp7F1.tag=3;_tmp7F1.f1=
_tmp7ED;_tmp7F1.f2=_tmp7EE;_tmp7F1;});_tmp7F0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EF;});
break;}case 371: _LL382: {struct Cyc_List_List*_tmp7F3;int _tmp7F4;struct _tuple13
_tmp7F2=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7F3=_tmp7F2.f1;_tmp7F4=_tmp7F2.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7F5;(_tmp7F5.YY11).tag=15;(_tmp7F5.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));
_tmp7F6[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7F7;_tmp7F7.tag=13;_tmp7F7.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7F7.f2=_tmp7F3;_tmp7F7.f3=_tmp7F4;_tmp7F7;});_tmp7F6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F5;});
break;}case 372: _LL383: {struct Cyc_List_List*_tmp7F9;int _tmp7FA;struct _tuple13
_tmp7F8=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7F9=_tmp7F8.f1;_tmp7FA=_tmp7F8.f2;{struct Cyc_List_List*_tmp7FB=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7FC;(_tmp7FC.YY11).tag=15;(_tmp7FC.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp7FE;_tmp7FE.tag=5;_tmp7FE.f1=({struct Cyc_Absyn_AggrInfo
_tmp7FF;_tmp7FF.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp800;(_tmp800.UnknownAggr).tag=0;(_tmp800.UnknownAggr).f1=(void*)((void*)0);(
_tmp800.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp800;});_tmp7FF.targs=0;_tmp7FF;});_tmp7FE.f2=
_tmp7FB;_tmp7FE.f3=_tmp7F9;_tmp7FE.f4=_tmp7FA;_tmp7FE;});_tmp7FD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FC;});
break;}}case 373: _LL384: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp801;(_tmp801.YY11).tag=15;(_tmp801.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp803;_tmp803.tag=4;_tmp803.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp803;});_tmp802;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp801;});break;case 374: _LL385: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp804;(_tmp804.YY11).tag=15;(_tmp804.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp805=_cycalloc(sizeof(*_tmp805));
_tmp805[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp806;_tmp806.tag=4;_tmp806.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp807=_cycalloc(
sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp808;_tmp808.tag=
4;_tmp808.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp808;});_tmp807;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp806;});
_tmp805;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp804;});break;case 375: _LL386: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp809;(_tmp809.YY11).tag=15;(_tmp809.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp80A=_cycalloc(sizeof(*_tmp80A));
_tmp80A[0]=({struct Cyc_Absyn_Reference_p_struct _tmp80B;_tmp80B.tag=1;_tmp80B.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp80C=_cycalloc(sizeof(*_tmp80C));
_tmp80C->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp80E;(
_tmp80E.Loc_n).tag=0;_tmp80E;});_tmp80C->f2=({struct _dynforward_ptr*_tmp80D=
_cycalloc(sizeof(*_tmp80D));_tmp80D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80D;});_tmp80C;}),(void*)
0,0);_tmp80B.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80B;});
_tmp80A;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp809;});break;case 376: _LL387: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp80F="as";_tag_dynforward(_tmp80F,
sizeof(char),_get_zero_arr_size(_tmp80F,3));}))!= 0)Cyc_Parse_err(({const char*
_tmp810="expecting `as'";_tag_dynforward(_tmp810,sizeof(char),_get_zero_arr_size(
_tmp810,15));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp811;(_tmp811.YY11).tag=15;(_tmp811.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Reference_p_struct*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp813;_tmp813.tag=1;_tmp813.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp816;(_tmp816.Loc_n).tag=0;_tmp816;});_tmp814->f2=({
struct _dynforward_ptr*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp815;});_tmp814;}),(
void*)0,0);_tmp813.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp813;});_tmp812;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp811;});
break;case 377: _LL388: {void*_tmp817=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp821;_tmp821.tag=0;_tmp821.f1=(void*)((void*)5);_tmp821;});_tmp820;}));yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp818;(_tmp818.YY11).tag=15;(
_tmp818.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp81A;_tmp81A.tag=2;_tmp81A.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp817);
_tmp81A.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp81B=_cycalloc(sizeof(*
_tmp81B));_tmp81B->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp81D;(_tmp81D.Loc_n).tag=0;_tmp81D;});_tmp81B->f2=({struct _dynforward_ptr*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp81C;});_tmp81B;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp81E=_cycalloc(sizeof(*_tmp81E));
_tmp81E[0]=({struct Cyc_Absyn_TagType_struct _tmp81F;_tmp81F.tag=18;_tmp81F.f1=(
void*)_tmp817;_tmp81F;});_tmp81E;}),0);_tmp81A;});_tmp819;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp818;});
break;}case 378: _LL389: {struct Cyc_Absyn_Tvar*_tmp822=Cyc_Tcutil_new_tvar((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp82E;_tmp82E.tag=0;_tmp82E.f1=(void*)((void*)5);
_tmp82E;});_tmp82D;}));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp823;(_tmp823.YY11).tag=15;(_tmp823.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_TagInt_p_struct*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824[0]=({
struct Cyc_Absyn_TagInt_p_struct _tmp825;_tmp825.tag=2;_tmp825.f1=_tmp822;_tmp825.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp826=_cycalloc(sizeof(*_tmp826));
_tmp826->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp828;(
_tmp828.Loc_n).tag=0;_tmp828;});_tmp826->f2=({struct _dynforward_ptr*_tmp827=
_cycalloc(sizeof(*_tmp827));_tmp827[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp827;});_tmp826;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp829=_cycalloc(sizeof(*_tmp829));
_tmp829[0]=({struct Cyc_Absyn_TagType_struct _tmp82A;_tmp82A.tag=18;_tmp82A.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp82B=_cycalloc(sizeof(*_tmp82B));
_tmp82B[0]=({struct Cyc_Absyn_VarType_struct _tmp82C;_tmp82C.tag=1;_tmp82C.f1=
_tmp822;_tmp82C;});_tmp82B;}));_tmp82A;});_tmp829;}),0);_tmp825;});_tmp824;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp823;});break;}case 379: _LL38A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp82F;(_tmp82F.YY12).tag=16;(_tmp82F.YY12).f1=({struct
_tuple13*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp830->f2=0;_tmp830;});
_tmp82F;});break;case 380: _LL38B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp831;(_tmp831.YY12).tag=16;(_tmp831.YY12).f1=({struct _tuple13*_tmp832=
_cycalloc(sizeof(*_tmp832));_tmp832->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp832->f2=1;_tmp832;});_tmp831;});break;case 381: _LL38C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp833;(_tmp833.YY12).tag=
16;(_tmp833.YY12).f1=({struct _tuple13*_tmp834=_cycalloc(sizeof(*_tmp834));
_tmp834->f1=0;_tmp834->f2=1;_tmp834;});_tmp833;});break;case 382: _LL38D: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp835;(_tmp835.YY13).tag=17;(
_tmp835.YY13).f1=({struct Cyc_List_List*_tmp836=_cycalloc(sizeof(*_tmp836));
_tmp836->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp836->tl=0;_tmp836;});_tmp835;});break;case 383: _LL38E: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp837;(_tmp837.YY13).tag=17;(
_tmp837.YY13).f1=({struct Cyc_List_List*_tmp838=_cycalloc(sizeof(*_tmp838));
_tmp838->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp838->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp838;});_tmp837;});break;case 384: _LL38F: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp839;(_tmp839.YY14).tag=18;(_tmp839.YY14).f1=({
struct _tuple14*_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A->f1=0;_tmp83A->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp83A;});_tmp839;});
break;case 385: _LL390: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83B;(_tmp83B.YY14).tag=18;(_tmp83B.YY14).f1=({struct _tuple14*_tmp83C=
_cycalloc(sizeof(*_tmp83C));_tmp83C->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp83C->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp83C;});_tmp83B;});
break;case 386: _LL391: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83D;(_tmp83D.YY16).tag=20;(_tmp83D.YY16).f1=({struct _tuple13*_tmp83E=
_cycalloc(sizeof(*_tmp83E));_tmp83E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp83E->f2=0;_tmp83E;});_tmp83D;});break;case 387: _LL392: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp83F;(_tmp83F.YY16).tag=20;(
_tmp83F.YY16).f1=({struct _tuple13*_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp840->f2=1;_tmp840;});
_tmp83F;});break;case 388: _LL393: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp841;(_tmp841.YY16).tag=20;(_tmp841.YY16).f1=({struct _tuple13*_tmp842=
_cycalloc(sizeof(*_tmp842));_tmp842->f1=0;_tmp842->f2=1;_tmp842;});_tmp841;});
break;case 389: _LL394: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp843;(_tmp843.YY15).tag=19;(_tmp843.YY15).f1=({struct Cyc_List_List*_tmp844=
_cycalloc(sizeof(*_tmp844));_tmp844->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp844->tl=0;_tmp844;});
_tmp843;});break;case 390: _LL395: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp845;(_tmp845.YY15).tag=19;(_tmp845.YY15).f1=({struct Cyc_List_List*_tmp846=
_cycalloc(sizeof(*_tmp846));_tmp846->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp846->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp846;});_tmp845;});
break;case 391: _LL396: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp847;(_tmp847.YY3).tag=7;(_tmp847.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
break;case 393: _LL398: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 394: _LL399: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY3).tag=7;(_tmp848.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp848;});
break;case 395: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp849;(_tmp849.YY8).tag=12;(_tmp849.YY8).f1=0;_tmp849;});break;case 396: _LL39B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp84A;(_tmp84A.YY8).tag=
12;(_tmp84A.YY8).f1=({struct Cyc_Core_Opt*_tmp84B=_cycalloc(sizeof(*_tmp84B));
_tmp84B->v=(void*)((void*)1);_tmp84B;});_tmp84A;});break;case 397: _LL39C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp84C;(_tmp84C.YY8).tag=12;(
_tmp84C.YY8).f1=({struct Cyc_Core_Opt*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D->v=(
void*)((void*)3);_tmp84D;});_tmp84C;});break;case 398: _LL39D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp84E;(_tmp84E.YY8).tag=12;(_tmp84E.YY8).f1=({struct Cyc_Core_Opt*
_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F->v=(void*)((void*)4);_tmp84F;});
_tmp84E;});break;case 399: _LL39E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp850;(_tmp850.YY8).tag=12;(_tmp850.YY8).f1=({struct Cyc_Core_Opt*_tmp851=
_cycalloc(sizeof(*_tmp851));_tmp851->v=(void*)((void*)0);_tmp851;});_tmp850;});
break;case 400: _LL39F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY8).tag=12;(_tmp852.YY8).f1=({struct Cyc_Core_Opt*_tmp853=
_cycalloc(sizeof(*_tmp853));_tmp853->v=(void*)((void*)2);_tmp853;});_tmp852;});
break;case 401: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY8).tag=12;(_tmp854.YY8).f1=({struct Cyc_Core_Opt*_tmp855=
_cycalloc(sizeof(*_tmp855));_tmp855->v=(void*)((void*)16);_tmp855;});_tmp854;});
break;case 402: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY8).tag=12;(_tmp856.YY8).f1=({struct Cyc_Core_Opt*_tmp857=
_cycalloc(sizeof(*_tmp857));_tmp857->v=(void*)((void*)17);_tmp857;});_tmp856;});
break;case 403: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp858;(_tmp858.YY8).tag=12;(_tmp858.YY8).f1=({struct Cyc_Core_Opt*_tmp859=
_cycalloc(sizeof(*_tmp859));_tmp859->v=(void*)((void*)13);_tmp859;});_tmp858;});
break;case 404: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85A;(_tmp85A.YY8).tag=12;(_tmp85A.YY8).f1=({struct Cyc_Core_Opt*_tmp85B=
_cycalloc(sizeof(*_tmp85B));_tmp85B->v=(void*)((void*)15);_tmp85B;});_tmp85A;});
break;case 405: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85C;(_tmp85C.YY8).tag=12;(_tmp85C.YY8).f1=({struct Cyc_Core_Opt*_tmp85D=
_cycalloc(sizeof(*_tmp85D));_tmp85D->v=(void*)((void*)14);_tmp85D;});_tmp85C;});
break;case 406: _LL3A5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 407: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85E;(_tmp85E.YY3).tag=7;(_tmp85E.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85E;});
break;case 408: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85F;(_tmp85F.YY3).tag=7;(_tmp85F.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
break;case 409: _LL3A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp860;(_tmp860.YY3).tag=7;(_tmp860.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp860;});
break;case 410: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp861;(_tmp861.YY3).tag=7;(_tmp861.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
break;case 411: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp862;(_tmp862.YY3).tag=7;(_tmp862.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp862;});
break;case 412: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp863;(_tmp863.YY3).tag=7;(_tmp863.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
break;case 413: _LL3AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 414: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 415: _LL3AE: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp864;(_tmp864.YY3).tag=7;(_tmp864.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp864;});
break;case 416: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 417: _LL3B0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp865;(_tmp865.YY3).tag=7;(_tmp865.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
break;case 418: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3B2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp866;(_tmp866.YY3).tag=7;(_tmp866.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp866;});
break;case 420: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3B4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp867;(_tmp867.YY3).tag=7;(_tmp867.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp867;});
break;case 422: _LL3B5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 423: _LL3B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp868;(_tmp868.YY3).tag=7;(_tmp868.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp868;});
break;case 424: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 425: _LL3B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp869;(_tmp869.YY3).tag=7;(_tmp869.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp869;});
break;case 426: _LL3B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86A;(_tmp86A.YY3).tag=7;(_tmp86A.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86A;});
break;case 427: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 428: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86B;(_tmp86B.YY3).tag=7;(_tmp86B.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86B;});
break;case 429: _LL3BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86C;(_tmp86C.YY3).tag=7;(_tmp86C.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86C;});
break;case 430: _LL3BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86D;(_tmp86D.YY3).tag=7;(_tmp86D.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86D;});
break;case 431: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86E;(_tmp86E.YY3).tag=7;(_tmp86E.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86E;});
break;case 432: _LL3BF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 433: _LL3C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86F;(_tmp86F.YY3).tag=7;(_tmp86F.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86F;});
break;case 434: _LL3C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp870;(_tmp870.YY3).tag=7;(_tmp870.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp870;});
break;case 435: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 436: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp871;(_tmp871.YY3).tag=7;(_tmp871.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
break;case 437: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp872;(_tmp872.YY3).tag=7;(_tmp872.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp872;});
break;case 438: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 439: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp873;(_tmp873.YY3).tag=7;(_tmp873.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp873;});
break;case 440: _LL3C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp874;(_tmp874.YY3).tag=7;(_tmp874.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp874;});
break;case 441: _LL3C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp875;(_tmp875.YY3).tag=7;(_tmp875.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp875;});
break;case 442: _LL3C9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 443: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp876;(_tmp876.YY3).tag=7;(_tmp876.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp876;});
break;case 444: _LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 445: _LL3CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp877;(_tmp877.YY3).tag=7;(_tmp877.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
break;case 446: _LL3CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp878;(_tmp878.YY3).tag=7;(_tmp878.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp878;});
break;case 447: _LL3CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp879;(_tmp879.YY3).tag=7;(_tmp879.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp879;});
break;case 448: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87A;(_tmp87A.YY3).tag=7;(_tmp87A.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87A;});
break;case 449: _LL3D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 450: _LL3D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87B;(_tmp87B.YY3).tag=7;(_tmp87B.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87B;});
break;case 451: _LL3D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87C;(_tmp87C.YY3).tag=7;(_tmp87C.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87C;});
break;case 452: _LL3D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87D;(_tmp87D.YY3).tag=7;(_tmp87D.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87D;});
break;case 453: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87E;(_tmp87E.YY3).tag=7;(_tmp87E.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({
struct Cyc_Absyn_StructField_struct _tmp880;_tmp880.tag=0;_tmp880.f1=({struct
_dynforward_ptr*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp881;});_tmp880;});
_tmp87F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp87E;});break;case 454: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp882;(_tmp882.YY3).tag=7;(_tmp882.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp883=_cycalloc_atomic(sizeof(*
_tmp883));_tmp883[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp884;_tmp884.tag=1;
_tmp884.f1=(unsigned int)(*Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp884;});_tmp883;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp882;});
break;case 455: _LL3D6: {struct Cyc_Position_Segment*_tmp885=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp886=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp885,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp887;(_tmp887.YY3).tag=
7;(_tmp887.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp886,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
break;}case 456: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp888;(_tmp888.YY3).tag=7;(_tmp888.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp88A;_tmp88A.tag=35;_tmp88A.f1=({struct Cyc_Absyn_MallocInfo
_tmp88B;_tmp88B.is_calloc=0;_tmp88B.rgn=0;_tmp88B.elt_type=0;_tmp88B.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp88B.fat_result=0;_tmp88B;});_tmp88A;});_tmp889;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp888;});
break;case 457: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88C;(_tmp88C.YY3).tag=7;(_tmp88C.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp88D=_cycalloc(sizeof(*_tmp88D));_tmp88D[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp88E;_tmp88E.tag=35;_tmp88E.f1=({struct Cyc_Absyn_MallocInfo
_tmp88F;_tmp88F.is_calloc=0;_tmp88F.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp88F.elt_type=0;
_tmp88F.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp88F.fat_result=0;_tmp88F;});_tmp88E;});_tmp88D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88C;});
break;case 458: _LL3D9: {void*_tmp891;struct _tuple2 _tmp890=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp891=_tmp890.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp892;(_tmp892.YY3).tag=7;(
_tmp892.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp894;_tmp894.tag=35;_tmp894.f1=({struct Cyc_Absyn_MallocInfo _tmp895;_tmp895.is_calloc=
1;_tmp895.rgn=0;_tmp895.elt_type=({void**_tmp896=_cycalloc(sizeof(*_tmp896));
_tmp896[0]=_tmp891;_tmp896;});_tmp895.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp895.fat_result=0;
_tmp895;});_tmp894;});_tmp893;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
break;}case 459: _LL3DA: {void*_tmp898;struct _tuple2 _tmp897=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp898=_tmp897.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp899;(_tmp899.YY3).tag=7;(
_tmp899.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp89B;_tmp89B.tag=35;_tmp89B.f1=({struct Cyc_Absyn_MallocInfo _tmp89C;_tmp89C.is_calloc=
1;_tmp89C.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp89C.elt_type=({void**
_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D[0]=_tmp898;_tmp89D;});_tmp89C.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp89C.fat_result=0;_tmp89C;});_tmp89B;});_tmp89A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
break;}case 460: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89E;(_tmp89E.YY7).tag=11;(_tmp89E.YY7).f1=(void*)((void*)12);_tmp89E;});
break;case 461: _LL3DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89F;(_tmp89F.YY7).tag=11;(_tmp89F.YY7).f1=(void*)((void*)11);_tmp89F;});
break;case 462: _LL3DD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A0;(_tmp8A0.YY7).tag=11;(_tmp8A0.YY7).f1=(void*)((void*)2);_tmp8A0;});
break;case 463: _LL3DE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 464: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A1;(_tmp8A1.YY3).tag=7;(_tmp8A1.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A1;});
break;case 465: _LL3E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A2;(_tmp8A2.YY3).tag=7;(_tmp8A2.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A2;});
break;case 466: _LL3E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A3;(_tmp8A3.YY3).tag=7;(_tmp8A3.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A3;});
break;case 467: _LL3E2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A4;(_tmp8A4.YY3).tag=7;(_tmp8A4.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A4;});
break;case 468: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A6;(_tmp8A6.YY3).tag=7;(_tmp8A6.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
break;case 469: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A8;(_tmp8A8.YY3).tag=7;(_tmp8A8.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A8;});
break;case 470: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A9;(_tmp8A9.YY3).tag=7;(_tmp8A9.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A9;});
break;case 471: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AA;(_tmp8AA.YY3).tag=7;(_tmp8AA.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp8AC;_tmp8AC.tag=27;_tmp8AC.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8AC.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8AC;});_tmp8AB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8AA;});break;case 472: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8AD;(_tmp8AD.YY3).tag=7;(_tmp8AD.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8AE=_cycalloc(sizeof(*_tmp8AE));
_tmp8AE[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp8AF;_tmp8AF.tag=27;_tmp8AF.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp8AF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp8AF;});_tmp8AE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AD;});
break;case 473: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B0;(_tmp8B0.YY3).tag=7;(_tmp8B0.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp8B1=_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp8B2;_tmp8B2.tag=2;_tmp8B2.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B2;});_tmp8B1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8B0;});break;case 474: _LL3E9: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 475: _LL3EA: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8B3;(_tmp8B3.YY3).tag=7;(
_tmp8B3.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B3;});
break;case 476: _LL3EB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 477: _LL3EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B4;(_tmp8B4.YY3).tag=7;(_tmp8B4.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B4;});
break;case 478: _LL3ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B5;(_tmp8B5.YY3).tag=7;(_tmp8B5.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B5;});
break;case 479: _LL3EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B6;(_tmp8B6.YY3).tag=7;(_tmp8B6.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B6;});
break;case 480: _LL3EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B7;(_tmp8B7.YY3).tag=7;(_tmp8B7.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp8B8=_cycalloc(sizeof(*_tmp8B8));_tmp8B8[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp8B9;_tmp8B9.tag=30;_tmp8B9.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8B9.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8B9.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8B9.f4=0;_tmp8B9;});
_tmp8B8;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8B7;});break;case 481: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8BA;(_tmp8BA.YY3).tag=7;(_tmp8BA.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BA;});
break;case 482: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8BB;(_tmp8BB.YY5).tag=9;(_tmp8BB.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp8BB;});break;case 483: _LL3F2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8BC;(_tmp8BC.YY5).tag=9;(_tmp8BC.YY5).f1=({struct Cyc_List_List*
_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8BD->tl=0;_tmp8BD;});
_tmp8BC;});break;case 484: _LL3F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8BE;(_tmp8BE.YY5).tag=9;(_tmp8BE.YY5).f1=({struct Cyc_List_List*_tmp8BF=
_cycalloc(sizeof(*_tmp8BF));_tmp8BF->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8BF->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8BF;});_tmp8BE;});
break;case 485: _LL3F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C0;(_tmp8C0.YY3).tag=7;(_tmp8C0.YY3).f1=Cyc_Absyn_int_exp((*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(*Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C0;});
break;case 486: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C1;(_tmp8C1.YY3).tag=7;(_tmp8C1.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C1;});
break;case 487: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C2;(_tmp8C2.YY3).tag=7;(_tmp8C2.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C2;});
break;case 488: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C3;(_tmp8C3.YY3).tag=7;(_tmp8C3.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C3;});
break;case 489: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C4;(_tmp8C4.QualId_tok).tag=4;(_tmp8C4.QualId_tok).f1=({struct _tuple1*
_tmp8C5=_cycalloc(sizeof(*_tmp8C5));_tmp8C5->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8C7;(_tmp8C7.Rel_n).tag=1;(_tmp8C7.Rel_n).f1=0;
_tmp8C7;});_tmp8C5->f2=({struct _dynforward_ptr*_tmp8C6=_cycalloc(sizeof(*_tmp8C6));
_tmp8C6[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8C6;});_tmp8C5;});_tmp8C4;});break;case 490: _LL3F9: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 491: _LL3FA: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8C8;(_tmp8C8.QualId_tok).tag=
4;(_tmp8C8.QualId_tok).f1=({struct _tuple1*_tmp8C9=_cycalloc(sizeof(*_tmp8C9));
_tmp8C9->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8CB;(
_tmp8CB.Rel_n).tag=1;(_tmp8CB.Rel_n).f1=0;_tmp8CB;});_tmp8C9->f2=({struct
_dynforward_ptr*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));_tmp8CA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8CA;});_tmp8C9;});
_tmp8C8;});break;case 492: _LL3FB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 493: _LL3FC: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 494: _LL3FD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 495: _LL3FE: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL3FF:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497:
_LL400: break;case 498: _LL401: yylex_buf->lex_curr_pos -=1;break;default: _LL402:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(499,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 131)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5974) && Cyc_yycheck[_check_known_subscript_notnull(5975,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5975,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 131)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(992,yystate)];if(yyn > - 32768  && yyn < 5974){int sze=
0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 273 / 
sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5975,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(273,x)])+ 15,count ++);}msg=({unsigned int _tmp8CC=(
unsigned int)(sze + 15);char*_tmp8CD=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8CC + 1));struct _dynforward_ptr _tmp8CF=_tag_dynforward(_tmp8CD,
sizeof(char),_tmp8CC + 1);{unsigned int _tmp8CE=_tmp8CC;unsigned int i;for(i=0;i < 
_tmp8CE;i ++){_tmp8CD[i]='\000';}_tmp8CD[_tmp8CE]=(char)0;}_tmp8CF;});Cyc_strcpy(
msg,({const char*_tmp8D0="parse error";_tag_dynforward(_tmp8D0,sizeof(char),
_get_zero_arr_size(_tmp8D0,12));}));if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 
273 / sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(5975,x + 
yyn)]== x){Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct
_dynforward_ptr)({const char*_tmp8D1=", expecting `";_tag_dynforward(_tmp8D1,
sizeof(char),_get_zero_arr_size(_tmp8D1,14));}):(struct _dynforward_ptr)({const
char*_tmp8D2=" or `";_tag_dynforward(_tmp8D2,sizeof(char),_get_zero_arr_size(
_tmp8D2,6));})));Cyc_strcat(msg,(struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(273,x)]);Cyc_strcat(msg,({const char*_tmp8D3="'";
_tag_dynforward(_tmp8D3,sizeof(char),_get_zero_arr_size(_tmp8D3,2));}));count ++;}}}
Cyc_yyerror((struct _dynforward_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8D4="parse error";
_tag_dynforward(_tmp8D4,sizeof(char),_get_zero_arr_size(_tmp8D4,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8D5=1;
_npop_handler(0);return _tmp8D5;}Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;
yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp8D6=1;_npop_handler(0);return
_tmp8D6;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(992,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 5974) || Cyc_yycheck[_check_known_subscript_notnull(
5975,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5975,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 991){int
_tmp8D7=0;_npop_handler(0);return _tmp8D7;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union _tmp8E1=v;struct _tuple11*
_tmp8E2;struct _tuple11 _tmp8E3;int _tmp8E4;char _tmp8E5;struct _dynforward_ptr
_tmp8E6;struct _tuple1*_tmp8E7;struct _tuple1 _tmp8E8;union Cyc_Absyn_Nmspace_union
_tmp8E9;struct _dynforward_ptr*_tmp8EA;_LL405: if((_tmp8E1.Int_tok).tag != 0)goto
_LL407;_tmp8E2=(_tmp8E1.Int_tok).f1;_tmp8E3=*_tmp8E2;_tmp8E4=_tmp8E3.f2;_LL406:({
struct Cyc_Int_pa_struct _tmp8ED;_tmp8ED.tag=1;_tmp8ED.f1=(unsigned long)_tmp8E4;{
void*_tmp8EB[1]={& _tmp8ED};Cyc_fprintf(Cyc_stderr,({const char*_tmp8EC="%d";
_tag_dynforward(_tmp8EC,sizeof(char),_get_zero_arr_size(_tmp8EC,3));}),
_tag_dynforward(_tmp8EB,sizeof(void*),1));}});goto _LL404;_LL407: if((_tmp8E1.Char_tok).tag
!= 1)goto _LL409;_tmp8E5=(_tmp8E1.Char_tok).f1;_LL408:({struct Cyc_Int_pa_struct
_tmp8F0;_tmp8F0.tag=1;_tmp8F0.f1=(unsigned long)((int)_tmp8E5);{void*_tmp8EE[1]={&
_tmp8F0};Cyc_fprintf(Cyc_stderr,({const char*_tmp8EF="%c";_tag_dynforward(_tmp8EF,
sizeof(char),_get_zero_arr_size(_tmp8EF,3));}),_tag_dynforward(_tmp8EE,sizeof(
void*),1));}});goto _LL404;_LL409: if((_tmp8E1.String_tok).tag != 2)goto _LL40B;
_tmp8E6=(_tmp8E1.String_tok).f1;_LL40A:({struct Cyc_String_pa_struct _tmp8F3;
_tmp8F3.tag=0;_tmp8F3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp8E6);{
void*_tmp8F1[1]={& _tmp8F3};Cyc_fprintf(Cyc_stderr,({const char*_tmp8F2="\"%s\"";
_tag_dynforward(_tmp8F2,sizeof(char),_get_zero_arr_size(_tmp8F2,5));}),
_tag_dynforward(_tmp8F1,sizeof(void*),1));}});goto _LL404;_LL40B: if((_tmp8E1.QualId_tok).tag
!= 4)goto _LL40D;_tmp8E7=(_tmp8E1.QualId_tok).f1;_tmp8E8=*_tmp8E7;_tmp8E9=_tmp8E8.f1;
_tmp8EA=_tmp8E8.f2;_LL40C: {struct Cyc_List_List*_tmp8F4=0;{union Cyc_Absyn_Nmspace_union
_tmp8F5=_tmp8E9;struct Cyc_List_List*_tmp8F6;struct Cyc_List_List*_tmp8F7;_LL410:
if((_tmp8F5.Rel_n).tag != 1)goto _LL412;_tmp8F6=(_tmp8F5.Rel_n).f1;_LL411: _tmp8F4=
_tmp8F6;goto _LL40F;_LL412: if((_tmp8F5.Abs_n).tag != 2)goto _LL414;_tmp8F7=(_tmp8F5.Abs_n).f1;
_LL413: _tmp8F4=_tmp8F7;goto _LL40F;_LL414: if((_tmp8F5.Loc_n).tag != 0)goto _LL40F;
_LL415: goto _LL40F;_LL40F:;}for(0;_tmp8F4 != 0;_tmp8F4=_tmp8F4->tl){({struct Cyc_String_pa_struct
_tmp8FA;_tmp8FA.tag=0;_tmp8FA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_tmp8F4->hd));{void*_tmp8F8[1]={& _tmp8FA};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8F9="%s::";_tag_dynforward(_tmp8F9,sizeof(char),_get_zero_arr_size(
_tmp8F9,5));}),_tag_dynforward(_tmp8F8,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp8FD;_tmp8FD.tag=0;_tmp8FD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp8EA);{void*_tmp8FB[1]={& _tmp8FD};Cyc_fprintf(Cyc_stderr,({const char*_tmp8FC="%s::";
_tag_dynforward(_tmp8FC,sizeof(char),_get_zero_arr_size(_tmp8FC,5));}),
_tag_dynforward(_tmp8FB,sizeof(void*),1));}});goto _LL404;}_LL40D:;_LL40E:({void*
_tmp8FE[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp8FF="?";_tag_dynforward(
_tmp8FF,sizeof(char),_get_zero_arr_size(_tmp8FF,2));}),_tag_dynforward(_tmp8FE,
sizeof(void*),0));});goto _LL404;_LL404:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
