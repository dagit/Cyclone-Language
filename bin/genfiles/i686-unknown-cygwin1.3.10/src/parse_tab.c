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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*
t);void*Cyc_Tcutil_compress(void*t);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
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
void*t,void*k,int leq){void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp40;struct Cyc_Absyn_Tvar _tmp41;void*_tmp42;void**_tmp43;_LL1C: if(_tmp3F <= (
void*)4)goto _LL1E;if(*((int*)_tmp3F)!= 1)goto _LL1E;_tmp40=((struct Cyc_Absyn_VarType_struct*)
_tmp3F)->f1;_tmp41=*_tmp40;_tmp42=(void*)_tmp41.kind;_tmp43=(void**)&(*((struct
Cyc_Absyn_VarType_struct*)_tmp3F)->f1).kind;_LL1D: {void*_tmp44=Cyc_Absyn_compress_kb(*
_tmp43);_LL21: if(*((int*)_tmp44)!= 1)goto _LL23;_LL22: if(!leq)*_tmp43=(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp46;_tmp46.tag=0;_tmp46.f1=(void*)k;_tmp46;});
_tmp45;});else{*_tmp43=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp47=_cycalloc(
sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Absyn_Less_kb_struct _tmp48;_tmp48.tag=2;
_tmp48.f1=0;_tmp48.f2=(void*)k;_tmp48;});_tmp47;});}return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp49=(void*)tms->hd;void*_tmp4A;
_LL26: if(*((int*)_tmp49)!= 3)goto _LL28;_tmp4A=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp49)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp4B=_tmp4A;struct Cyc_List_List*_tmp4C;_LL2B: if(*((int*)_tmp4B)!= 1)
goto _LL2D;_LL2C:({void*_tmp4D[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4E="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,93));}),
_tag_dynforward(_tmp4D,sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp4B)!= 
0)goto _LL2A;_tmp4C=((struct Cyc_Absyn_NoTypes_struct*)_tmp4B)->f1;_LL2E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp4C)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmp4F[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp50="wrong number of parameter declarations in old-style function declaration";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,73));}),
_tag_dynforward(_tmp4F,sizeof(void*),0));});{struct Cyc_List_List*_tmp51=0;for(0;
_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_List_List*_tmp52=tds;for(0;_tmp52 != 0;
_tmp52=_tmp52->tl){struct Cyc_Absyn_Decl*_tmp53=(struct Cyc_Absyn_Decl*)_tmp52->hd;
void*_tmp54=(void*)_tmp53->r;struct Cyc_Absyn_Vardecl*_tmp55;_LL30: if(_tmp54 <= (
void*)2)goto _LL32;if(*((int*)_tmp54)!= 0)goto _LL32;_tmp55=((struct Cyc_Absyn_Var_d_struct*)
_tmp54)->f1;_LL31: if(Cyc_zstrptrcmp((*_tmp55->name).f2,(struct _dynforward_ptr*)
_tmp4C->hd)!= 0)continue;if(_tmp55->initializer != 0)({void*_tmp56[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Parse_abort)(_tmp53->loc,({const char*_tmp57="initializer found in parameter declaration";
_tag_dynforward(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,43));}),
_tag_dynforward(_tmp56,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(
_tmp55->name))({void*_tmp58[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp53->loc,({const
char*_tmp59="namespaces forbidden in parameter declarations";_tag_dynforward(
_tmp59,sizeof(char),_get_zero_arr_size(_tmp59,47));}),_tag_dynforward(_tmp58,
sizeof(void*),0));});_tmp51=({struct Cyc_List_List*_tmp5A=_cycalloc(sizeof(*
_tmp5A));_tmp5A->hd=({struct _tuple2*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->f1=({
struct Cyc_Core_Opt*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->v=(*_tmp55->name).f2;
_tmp5C;});_tmp5B->f2=_tmp55->tq;_tmp5B->f3=(void*)_tmp55->type;_tmp5B;});_tmp5A->tl=
_tmp51;_tmp5A;});goto L;_LL32:;_LL33:({void*_tmp5D[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp53->loc,({
const char*_tmp5E="nonvariable declaration in parameter type";_tag_dynforward(
_tmp5E,sizeof(char),_get_zero_arr_size(_tmp5E,42));}),_tag_dynforward(_tmp5D,
sizeof(void*),0));});_LL2F:;}L: if(_tmp52 == 0)({struct Cyc_String_pa_struct _tmp61;
_tmp61.tag=0;_tmp61.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmp4C->hd));{void*_tmp5F[1]={& _tmp61};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp60="%s is not given a type";_tag_dynforward(_tmp60,sizeof(char),
_get_zero_arr_size(_tmp60,23));}),_tag_dynforward(_tmp5F,sizeof(void*),1));}});}
return({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63[0]=({struct Cyc_Absyn_Function_mod_struct _tmp64;_tmp64.tag=3;_tmp64.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65[0]=({struct Cyc_Absyn_WithTypes_struct _tmp66;_tmp66.tag=1;_tmp66.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp51);_tmp66.f2=
0;_tmp66.f3=0;_tmp66.f4=0;_tmp66.f5=0;_tmp66;});_tmp65;}));_tmp64;});_tmp63;}));
_tmp62->tl=0;_tmp62;});}_LL2A:;}goto _LL29;_LL28:;_LL29: return({struct Cyc_List_List*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=(void*)((void*)tms->hd);_tmp67->tl=
Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp67;});_LL25:;}}struct _tuple10{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68->id=d->id;_tmp68->tms=Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmp68;});{
void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmp69=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL35: if((int)_tmp69 != 1)goto _LL37;_LL36: sc=(void*)3;goto _LL34;
_LL37: if((int)_tmp69 != 3)goto _LL39;_LL38: sc=(void*)0;goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({
const char*_tmp6A="bad storage class on function";_tag_dynforward(_tmp6A,sizeof(
char),_get_zero_arr_size(_tmp6A,30));}),loc);goto _LL34;_LL34:;}}{void*_tmp6C;
struct Cyc_Core_Opt*_tmp6D;struct _tuple5 _tmp6B=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{struct Cyc_Absyn_Tqual _tmp6F;void*
_tmp70;struct Cyc_List_List*_tmp71;struct Cyc_List_List*_tmp72;struct _tuple6 _tmp6E=
Cyc_Parse_apply_tms(tq,_tmp6C,atts,d->tms);_tmp6F=_tmp6E.f1;_tmp70=_tmp6E.f2;
_tmp71=_tmp6E.f3;_tmp72=_tmp6E.f4;if(_tmp6D != 0)({void*_tmp73[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp74="nested type declaration within function prototype";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,50));}),
_tag_dynforward(_tmp73,sizeof(void*),0));});if(_tmp71 != 0)({void*_tmp75[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp76="bad type params, ignoring";
_tag_dynforward(_tmp76,sizeof(char),_get_zero_arr_size(_tmp76,26));}),
_tag_dynforward(_tmp75,sizeof(void*),0));});{void*_tmp77=_tmp70;struct Cyc_Absyn_FnInfo
_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Core_Opt*_tmp7A;void*_tmp7B;struct
Cyc_List_List*_tmp7C;int _tmp7D;struct Cyc_Absyn_VarargInfo*_tmp7E;struct Cyc_List_List*
_tmp7F;struct Cyc_List_List*_tmp80;_LL3C: if(_tmp77 <= (void*)4)goto _LL3E;if(*((int*)
_tmp77)!= 8)goto _LL3E;_tmp78=((struct Cyc_Absyn_FnType_struct*)_tmp77)->f1;_tmp79=
_tmp78.tvars;_tmp7A=_tmp78.effect;_tmp7B=(void*)_tmp78.ret_typ;_tmp7C=_tmp78.args;
_tmp7D=_tmp78.c_varargs;_tmp7E=_tmp78.cyc_varargs;_tmp7F=_tmp78.rgn_po;_tmp80=
_tmp78.attributes;_LL3D: {struct Cyc_List_List*_tmp81=0;{struct Cyc_List_List*
_tmp82=_tmp7C;for(0;_tmp82 != 0;_tmp82=_tmp82->tl){struct _tuple2 _tmp84;struct Cyc_Core_Opt*
_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct _tuple2*_tmp83=(struct
_tuple2*)_tmp82->hd;_tmp84=*_tmp83;_tmp85=_tmp84.f1;_tmp86=_tmp84.f2;_tmp87=
_tmp84.f3;if(_tmp85 == 0){Cyc_Parse_err(({const char*_tmp88="missing argument variable in function prototype";
_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,48));}),loc);
_tmp81=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=({
struct _tuple10*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=_init_dynforward_ptr(
_cycalloc(sizeof(struct _dynforward_ptr)),"?",sizeof(char),2);_tmp8A->f2=_tmp86;
_tmp8A->f3=_tmp87;_tmp8A;});_tmp89->tl=_tmp81;_tmp89;});}else{_tmp81=({struct Cyc_List_List*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=({struct _tuple10*_tmp8D=_cycalloc(
sizeof(*_tmp8D));_tmp8D->f1=(struct _dynforward_ptr*)_tmp85->v;_tmp8D->f2=_tmp86;
_tmp8D->f3=_tmp87;_tmp8D;});_tmp8C->tl=_tmp81;_tmp8C;});}}}return({struct Cyc_Absyn_Fndecl*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->sc=(void*)sc;_tmp8E->name=d->id;_tmp8E->tvs=
_tmp79;_tmp8E->is_inline=is_inline;_tmp8E->effect=_tmp7A;_tmp8E->ret_type=(void*)
_tmp7B;_tmp8E->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp81);_tmp8E->c_varargs=_tmp7D;_tmp8E->cyc_varargs=_tmp7E;_tmp8E->rgn_po=
_tmp7F;_tmp8E->body=body;_tmp8E->cached_typ=0;_tmp8E->param_vardecls=0;_tmp8E->fn_vardecl=
0;_tmp8E->attributes=Cyc_List_append(_tmp80,_tmp72);_tmp8E;});}_LL3E:;_LL3F:({
void*_tmp8F[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp90="declarator is not a function prototype";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,39));}),
_tag_dynforward(_tmp8F,sizeof(void*),0));});_LL3B:;}}}}}static char _tmp91[52]="at most one type may appear within a type specifier";
static struct _dynforward_ptr Cyc_Parse_msg1={_tmp91,_tmp91 + 52};static char _tmp92[
63]="const or volatile may appear only once within a type specifier";static
struct _dynforward_ptr Cyc_Parse_msg2={_tmp92,_tmp92 + 63};static char _tmp93[50]="type specifier includes more than one declaration";
static struct _dynforward_ptr Cyc_Parse_msg3={_tmp93,_tmp93 + 50};static char _tmp94[
60]="sign specifier may appear only once within a type specifier";static struct
_dynforward_ptr Cyc_Parse_msg4={_tmp94,_tmp94 + 60};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(
void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;
ts=ts->tl){void*_tmp95=(void*)ts->hd;void*_tmp96;struct Cyc_Position_Segment*
_tmp97;struct Cyc_Position_Segment*_tmp98;struct Cyc_Position_Segment*_tmp99;
struct Cyc_Position_Segment*_tmp9A;struct Cyc_Position_Segment*_tmp9B;struct Cyc_Absyn_Decl*
_tmp9C;_LL41: if(*((int*)_tmp95)!= 4)goto _LL43;_tmp96=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp95)->f1;_tmp97=((struct Cyc_Parse_Type_spec_struct*)_tmp95)->f2;_LL42: if(
seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp97);last_loc=_tmp97;seen_type=1;t=
_tmp96;goto _LL40;_LL43: if(*((int*)_tmp95)!= 0)goto _LL45;_tmp98=((struct Cyc_Parse_Signed_spec_struct*)
_tmp95)->f1;_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp98);if(seen_type)
Cyc_Parse_err(({const char*_tmp9D="signed qualifier must come before type";
_tag_dynforward(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,39));}),_tmp98);
last_loc=_tmp98;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp95)!= 1)
goto _LL47;_tmp99=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp95)->f1;_LL46: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp99);if(seen_type)Cyc_Parse_err(({
const char*_tmp9E="signed qualifier must come before type";_tag_dynforward(_tmp9E,
sizeof(char),_get_zero_arr_size(_tmp9E,39));}),_tmp99);last_loc=_tmp99;seen_sign=
1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp95)!= 2)goto _LL49;_tmp9A=((struct
Cyc_Parse_Short_spec_struct*)_tmp95)->f1;_LL48: if(seen_size)Cyc_Parse_err(({
const char*_tmp9F="integral size may appear only once within a type specifier";
_tag_dynforward(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,59));}),_tmp9A);
if(seen_type)Cyc_Parse_err(({const char*_tmpA0="short modifier must come before base type";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,42));}),_tmp9A);
last_loc=_tmp9A;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp95)!= 3)
goto _LL4B;_tmp9B=((struct Cyc_Parse_Long_spec_struct*)_tmp95)->f1;_LL4A: if(
seen_type)Cyc_Parse_err(({const char*_tmpA1="long modifier must come before base type";
_tag_dynforward(_tmpA1,sizeof(char),_get_zero_arr_size(_tmpA1,41));}),_tmp9B);
if(seen_size){void*_tmpA2=sz;_LL4E: if((int)_tmpA2 != 3)goto _LL50;_LL4F: sz=(void*)
4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({const char*_tmpA3="extra long in type specifier";
_tag_dynforward(_tmpA3,sizeof(char),_get_zero_arr_size(_tmpA3,29));}),_tmp9B);
goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp9B;seen_size=1;goto _LL40;_LL4B:
if(*((int*)_tmp95)!= 5)goto _LL40;_tmp9C=((struct Cyc_Parse_Decl_spec_struct*)
_tmp95)->f1;_LL4C: last_loc=_tmp9C->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmpA4=(void*)_tmp9C->r;struct Cyc_Absyn_Aggrdecl*_tmpA5;struct Cyc_Absyn_Tuniondecl*
_tmpA6;struct Cyc_Absyn_Enumdecl*_tmpA7;_LL53: if(_tmpA4 <= (void*)2)goto _LL59;if(*((
int*)_tmpA4)!= 4)goto _LL55;_tmpA5=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA4)->f1;
_LL54: {struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA5->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_AggrType_struct
_tmpAA;_tmpAA.tag=10;_tmpAA.f1=({struct Cyc_Absyn_AggrInfo _tmpAB;_tmpAB.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpAC;(_tmpAC.UnknownAggr).tag=
0;(_tmpAC.UnknownAggr).f1=(void*)((void*)_tmpA5->kind);(_tmpAC.UnknownAggr).f2=
_tmpA5->name;_tmpAC;});_tmpAB.targs=_tmpA8;_tmpAB;});_tmpAA;});_tmpA9;});if(
_tmpA5->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpAD=_cycalloc(sizeof(*_tmpAD));
_tmpAD->v=_tmp9C;_tmpAD;});goto _LL52;}_LL55: if(*((int*)_tmpA4)!= 5)goto _LL57;
_tmpA6=((struct Cyc_Absyn_Tunion_d_struct*)_tmpA4)->f1;_LL56: {struct Cyc_List_List*
_tmpAE=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA6->tvs));struct Cyc_Core_Opt*ropt=_tmpA6->is_flat?0:({struct Cyc_Core_Opt*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=(void*)((void*)2);_tmpB5;});t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_TunionType_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=({struct Cyc_Absyn_TunionInfo
_tmpB1;_tmpB1.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union
_tmpB2;(_tmpB2.KnownTunion).tag=1;(_tmpB2.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=_tmpA6;_tmpB3;});_tmpB2;});_tmpB1.targs=
_tmpAE;_tmpB1.rgn=ropt;_tmpB1;});_tmpB0;});_tmpAF;});if(_tmpA6->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=_tmp9C;
_tmpB4;});goto _LL52;}_LL57: if(*((int*)_tmpA4)!= 6)goto _LL59;_tmpA7=((struct Cyc_Absyn_Enum_d_struct*)
_tmpA4)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB6=_cycalloc(
sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_EnumType_struct _tmpB7;_tmpB7.tag=12;
_tmpB7.f1=_tmpA7->name;_tmpB7.f2=0;_tmpB7;});_tmpB6;});declopt=({struct Cyc_Core_Opt*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->v=_tmp9C;_tmpB8;});goto _LL52;_LL59:;
_LL5A:({void*_tmpB9[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp9C->loc,({const
char*_tmpBA="bad declaration within type specifier";_tag_dynforward(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,38));}),_tag_dynforward(_tmpB9,sizeof(
void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9C->loc);}goto _LL40;
_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpBB[0]={};Cyc_Tcutil_warn(
last_loc,({const char*_tmpBC="missing type within specifier";_tag_dynforward(
_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,30));}),_tag_dynforward(_tmpBB,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_IntType_struct _tmpBE;_tmpBE.tag=5;
_tmpBE.f1=(void*)sgn;_tmpBE.f2=(void*)sz;_tmpBE;});_tmpBD;});}else{if(seen_sign){
void*_tmpBF=t;void*_tmpC0;void*_tmpC1;_LL5C: if(_tmpBF <= (void*)4)goto _LL5E;if(*((
int*)_tmpBF)!= 5)goto _LL5E;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBF)->f1;_tmpC1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBF)->f2;_LL5D:
if(_tmpC0 != sgn)t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC2=_cycalloc(
sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_IntType_struct _tmpC3;_tmpC3.tag=5;
_tmpC3.f1=(void*)sgn;_tmpC3.f2=(void*)_tmpC1;_tmpC3;});_tmpC2;});goto _LL5B;_LL5E:;
_LL5F: Cyc_Parse_err(({const char*_tmpC4="sign specification on non-integral type";
_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,40));}),last_loc);
goto _LL5B;_LL5B:;}if(seen_size){void*_tmpC5=t;void*_tmpC6;void*_tmpC7;_LL61: if(
_tmpC5 <= (void*)4)goto _LL65;if(*((int*)_tmpC5)!= 5)goto _LL63;_tmpC6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpC5)->f1;_tmpC7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC5)->f2;_LL62: if(_tmpC7 != sz)t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_IntType_struct
_tmpC9;_tmpC9.tag=5;_tmpC9.f1=(void*)_tmpC6;_tmpC9.f2=(void*)sz;_tmpC9;});_tmpC8;});
goto _LL60;_LL63: if(*((int*)_tmpC5)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpCA=_cycalloc_atomic(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpCB;_tmpCB.tag=6;_tmpCB.f1=1;_tmpCB;});_tmpCA;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpCC="size qualifier on non-integral type";_tag_dynforward(_tmpCC,
sizeof(char),_get_zero_arr_size(_tmpCC,36));}),last_loc);goto _LL60;_LL60:;}}
return({struct _tuple5 _tmpCD;_tmpCD.f1=t;_tmpCD.f2=declopt;_tmpCD;});}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmpCE=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpCF=_tmpCE->id;struct Cyc_Absyn_Tqual _tmpD1;void*_tmpD2;struct
Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD4;struct _tuple6 _tmpD0=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpCE->tms);_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;_tmpD3=_tmpD0.f3;
_tmpD4=_tmpD0.f4;if(ds->tl == 0)return({struct Cyc_List_List*_tmpD5=_region_malloc(
r,sizeof(*_tmpD5));_tmpD5->hd=({struct _tuple7*_tmpD6=_region_malloc(r,sizeof(*
_tmpD6));_tmpD6->f1=_tmpCF;_tmpD6->f2=_tmpD1;_tmpD6->f3=_tmpD2;_tmpD6->f4=_tmpD3;
_tmpD6->f5=_tmpD4;_tmpD6;});_tmpD5->tl=0;_tmpD5;});else{return({struct Cyc_List_List*
_tmpD7=_region_malloc(r,sizeof(*_tmpD7));_tmpD7->hd=({struct _tuple7*_tmpD8=
_region_malloc(r,sizeof(*_tmpD8));_tmpD8->f1=_tmpCF;_tmpD8->f2=_tmpD1;_tmpD8->f3=
_tmpD2;_tmpD8->f4=_tmpD3;_tmpD8->f5=_tmpD4;_tmpD8;});_tmpD7->tl=Cyc_Parse_apply_tmss(
r,_tmpD1,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD7;});}}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD9;_tmpD9.f1=
tq;_tmpD9.f2=t;_tmpD9.f3=0;_tmpD9.f4=atts;_tmpD9;});{void*_tmpDA=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpDB;struct Cyc_Position_Segment*_tmpDC;struct Cyc_Absyn_Exp*
_tmpDD;struct Cyc_Absyn_Conref*_tmpDE;struct Cyc_Position_Segment*_tmpDF;void*
_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Position_Segment*_tmpE2;struct Cyc_Absyn_PtrAtts
_tmpE3;struct Cyc_Absyn_Tqual _tmpE4;struct Cyc_Position_Segment*_tmpE5;struct Cyc_List_List*
_tmpE6;_LL68: if(*((int*)_tmpDA)!= 0)goto _LL6A;_tmpDB=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Carray_mod_struct*)_tmpDA)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpDB,
_tmpDC),atts,tms->tl);_LL6A: if(*((int*)_tmpDA)!= 1)goto _LL6C;_tmpDD=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpDA)->f1;_tmpDE=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpDA)->f2;_tmpDF=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpDA)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpDD,
_tmpDE,_tmpDF),atts,tms->tl);_LL6C: if(*((int*)_tmpDA)!= 3)goto _LL6E;_tmpE0=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpDA)->f1;_LL6D: {void*_tmpE7=_tmpE0;
struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_Core_Opt*
_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Position_Segment*_tmpED;_LL75: if(*((
int*)_tmpE7)!= 1)goto _LL77;_tmpE8=((struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f1;
_tmpE9=((struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f2;_tmpEA=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE7)->f3;_tmpEB=((struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f4;_tmpEC=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=(void*)((
void*)as->hd);_tmpEE->tl=fn_atts;_tmpEE;});else{new_atts=({struct Cyc_List_List*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->hd=(void*)((void*)as->hd);_tmpEF->tl=
new_atts;_tmpEF;});}}}if(tms->tl != 0){void*_tmpF0=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpF1;_LL7A: if(*((int*)_tmpF0)!= 
4)goto _LL7C;_tmpF1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpF0)->f1;_LL7B:
typvars=_tmpF1;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE9  && _tmpEA == 0) && _tmpE8 != 0) && _tmpE8->tl == 0) && (*((struct _tuple2*)
_tmpE8->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE8->hd)).f3 == (void*)0)_tmpE8=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE8);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpEB,t,_tmpE8,_tmpE9,_tmpEA,_tmpEC,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE7)!= 0)goto _LL74;_tmpED=((struct Cyc_Absyn_NoTypes_struct*)_tmpE7)->f2;_LL78:({
void*_tmpF2[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpED,({const char*_tmpF3="function declaration without parameter types";
_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,45));}),
_tag_dynforward(_tmpF2,sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpDA)!= 4)
goto _LL70;_tmpE1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpDA)->f1;_tmpE2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpDA)->f2;_LL6F: if(tms->tl == 0)return({
struct _tuple6 _tmpF4;_tmpF4.f1=tq;_tmpF4.f2=t;_tmpF4.f3=_tmpE1;_tmpF4.f4=atts;
_tmpF4;});({void*_tmpF5[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpE2,({const char*
_tmpF6="type parameters must appear before function arguments in declarator";
_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,68));}),
_tag_dynforward(_tmpF5,sizeof(void*),0));});_LL70: if(*((int*)_tmpDA)!= 2)goto
_LL72;_tmpE3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpDA)->f1;_tmpE4=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpDA)->f2;_LL71: return Cyc_Parse_apply_tms(_tmpE4,(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));
_tmpF7[0]=({struct Cyc_Absyn_PointerType_struct _tmpF8;_tmpF8.tag=4;_tmpF8.f1=({
struct Cyc_Absyn_PtrInfo _tmpF9;_tmpF9.elt_typ=(void*)t;_tmpF9.elt_tq=tq;_tmpF9.ptr_atts=
_tmpE3;_tmpF9;});_tmpF8;});_tmpF7;}),atts,tms->tl);_LL72: if(*((int*)_tmpDA)!= 5)
goto _LL67;_tmpE5=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpDA)->f1;_tmpE6=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpDA)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpE6),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpFB;struct Cyc_Core_Opt*
_tmpFC;struct _tuple5 _tmpFA=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpFB=
_tmpFA.f1;_tmpFC=_tmpFA.f2;if(_tmpFC != 0)({void*_tmpFD[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmpFE="ignoring nested type declaration(s) in specifier list";
_tag_dynforward(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,54));}),
_tag_dynforward(_tmpFD,sizeof(void*),0));});return _tmpFB;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmp100;struct _tuple1*_tmp101;struct Cyc_Absyn_Tqual _tmp102;void*
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct _tuple7*
_tmpFF=t;_tmp100=*_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmp100.f3;
_tmp104=_tmp100.f4;_tmp105=_tmp100.f5;Cyc_Lex_register_typedef(_tmp101);{struct
Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp106=_tmp103;struct Cyc_Core_Opt*
_tmp107;_LL7F: if(_tmp106 <= (void*)4)goto _LL81;if(*((int*)_tmp106)!= 0)goto _LL81;
_tmp107=((struct Cyc_Absyn_Evar_struct*)_tmp106)->f1;_LL80: type=0;if(_tmp107 == 0)
kind=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->v=(void*)((
void*)2);_tmp108;});else{kind=_tmp107;}goto _LL7E;_LL81:;_LL82: kind=0;type=({
struct Cyc_Core_Opt*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->v=(void*)_tmp103;
_tmp109;});goto _LL7E;_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmp10B;_tmp10B.tag=7;_tmp10B.f1=({struct Cyc_Absyn_Typedefdecl*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C->name=_tmp101;_tmp10C->tvs=_tmp104;_tmp10C->kind=kind;
_tmp10C->defn=type;_tmp10C->atts=_tmp105;_tmp10C->tq=_tmp102;_tmp10C;});_tmp10B;});
_tmp10A;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10E;_tmp10E.tag=11;_tmp10E.f1=d;_tmp10E.f2=s;_tmp10E;});_tmp10D;}),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp10F=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp10F;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp111;struct Cyc_Absyn_Tqual
_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_Parse_Declaration_spec*
_tmp110=ds;_tmp111=*_tmp110;_tmp112=_tmp111.tq;_tmp113=_tmp111.type_specs;
_tmp114=_tmp111.attributes;if(_tmp112.loc == 0)_tmp112.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp115="inline is allowed only on function definitions";
_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,47));}),loc);if(
_tmp113 == 0){Cyc_Parse_err(({const char*_tmp116="missing type specifiers in declaration";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,39));}),loc);{
struct Cyc_List_List*_tmp117=0;_npop_handler(0);return _tmp117;}}{void*s=(void*)2;
int istypedef=0;if(ds->sc != 0){void*_tmp118=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp118 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp118 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp118 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp118 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp118 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp118 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp118 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp11A;struct Cyc_List_List*
_tmp11B;struct _tuple0 _tmp119=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp11B;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple5 _tmp11C=Cyc_Parse_collapse_type_specifiers(_tmp113,loc);
if(_tmp11A == 0){void*_tmp11E;struct Cyc_Core_Opt*_tmp11F;struct _tuple5 _tmp11D=
_tmp11C;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;if(_tmp11F != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp11F->v;{void*_tmp120=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmp121;struct Cyc_Absyn_Aggrdecl*_tmp122;struct Cyc_Absyn_Tuniondecl*_tmp123;
_LL93: if(_tmp120 <= (void*)2)goto _LL99;if(*((int*)_tmp120)!= 6)goto _LL95;_tmp121=((
struct Cyc_Absyn_Enum_d_struct*)_tmp120)->f1;_LL94:(void*)(_tmp121->sc=(void*)s);
if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp124="bad attributes on enum";
_tag_dynforward(_tmp124,sizeof(char),_get_zero_arr_size(_tmp124,23));}),loc);
goto _LL92;_LL95: if(*((int*)_tmp120)!= 4)goto _LL97;_tmp122=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp120)->f1;_LL96:(void*)(_tmp122->sc=(void*)s);_tmp122->attributes=_tmp114;
goto _LL92;_LL97: if(*((int*)_tmp120)!= 5)goto _LL99;_tmp123=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp120)->f1;_LL98:(void*)(_tmp123->sc=(void*)s);if(_tmp114 != 0)Cyc_Parse_err(({
const char*_tmp125="bad attributes on tunion";_tag_dynforward(_tmp125,sizeof(char),
_get_zero_arr_size(_tmp125,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp126="bad declaration";_tag_dynforward(_tmp126,sizeof(char),
_get_zero_arr_size(_tmp126,16));}),loc);{struct Cyc_List_List*_tmp127=0;
_npop_handler(0);return _tmp127;}_LL92:;}{struct Cyc_List_List*_tmp129=({struct Cyc_List_List*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=d;_tmp128->tl=0;_tmp128;});
_npop_handler(0);return _tmp129;}}else{void*_tmp12A=_tmp11E;struct Cyc_Absyn_AggrInfo
_tmp12B;union Cyc_Absyn_AggrInfoU_union _tmp12C;void*_tmp12D;struct _tuple1*_tmp12E;
struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_TunionInfo _tmp130;union Cyc_Absyn_TunionInfoU_union
_tmp131;struct Cyc_Absyn_Tuniondecl**_tmp132;struct Cyc_Absyn_TunionInfo _tmp133;
union Cyc_Absyn_TunionInfoU_union _tmp134;struct Cyc_Absyn_UnknownTunionInfo _tmp135;
struct _tuple1*_tmp136;int _tmp137;int _tmp138;struct Cyc_List_List*_tmp139;struct
_tuple1*_tmp13A;struct Cyc_List_List*_tmp13B;_LL9C: if(_tmp12A <= (void*)4)goto
_LLA6;if(*((int*)_tmp12A)!= 10)goto _LL9E;_tmp12B=((struct Cyc_Absyn_AggrType_struct*)
_tmp12A)->f1;_tmp12C=_tmp12B.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp12A)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL9E;_tmp12D=(_tmp12C.UnknownAggr).f1;
_tmp12E=(_tmp12C.UnknownAggr).f2;_tmp12F=_tmp12B.targs;_LL9D: {struct Cyc_List_List*
_tmp13C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12F);struct Cyc_Absyn_Aggrdecl*_tmp13D=({struct Cyc_Absyn_Aggrdecl*_tmp143=
_cycalloc(sizeof(*_tmp143));_tmp143->kind=(void*)_tmp12D;_tmp143->sc=(void*)s;
_tmp143->name=_tmp12E;_tmp143->tvs=_tmp13C;_tmp143->impl=0;_tmp143->attributes=0;
_tmp143;});if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp13E="bad attributes on type declaration";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,35));}),loc);{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*
_tmp13F));_tmp13F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp141;_tmp141.tag=4;_tmp141.f1=_tmp13D;_tmp141;});_tmp140;}),loc);_tmp13F->tl=
0;_tmp13F;});_npop_handler(0);return _tmp142;}}_LL9E: if(*((int*)_tmp12A)!= 2)goto
_LLA0;_tmp130=((struct Cyc_Absyn_TunionType_struct*)_tmp12A)->f1;_tmp131=_tmp130.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp12A)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LLA0;_tmp132=(_tmp131.KnownTunion).f1;_LL9F: if(_tmp114 != 0)Cyc_Parse_err(({
const char*_tmp144="bad attributes on tunion";_tag_dynforward(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,25));}),loc);{struct Cyc_List_List*_tmp148=({struct Cyc_List_List*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Tunion_d_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({
struct Cyc_Absyn_Tunion_d_struct _tmp147;_tmp147.tag=5;_tmp147.f1=*_tmp132;_tmp147;});
_tmp146;}),loc);_tmp145->tl=0;_tmp145;});_npop_handler(0);return _tmp148;}_LLA0:
if(*((int*)_tmp12A)!= 2)goto _LLA2;_tmp133=((struct Cyc_Absyn_TunionType_struct*)
_tmp12A)->f1;_tmp134=_tmp133.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp12A)->f1).tunion_info).UnknownTunion).tag != 0)goto _LLA2;_tmp135=(_tmp134.UnknownTunion).f1;
_tmp136=_tmp135.name;_tmp137=_tmp135.is_xtunion;_tmp138=_tmp135.is_flat;_tmp139=
_tmp133.targs;_LLA1: {struct Cyc_List_List*_tmp149=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp139);
struct Cyc_Absyn_Decl*_tmp14A=Cyc_Absyn_tunion_decl(s,_tmp136,_tmp149,0,_tmp137,
_tmp138,loc);if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp14B="bad attributes on tunion";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,25));}),loc);{
struct Cyc_List_List*_tmp14D=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*
_tmp14C));_tmp14C->hd=_tmp14A;_tmp14C->tl=0;_tmp14C;});_npop_handler(0);return
_tmp14D;}}_LLA2: if(*((int*)_tmp12A)!= 12)goto _LLA4;_tmp13A=((struct Cyc_Absyn_EnumType_struct*)
_tmp12A)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp14E=({struct Cyc_Absyn_Enumdecl*
_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->sc=(void*)s;_tmp155->name=_tmp13A;
_tmp155->fields=0;_tmp155;});if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp14F="bad attributes on enum";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,23));}),loc);{
struct Cyc_List_List*_tmp154=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*
_tmp150));_tmp150->hd=({struct Cyc_Absyn_Decl*_tmp151=_cycalloc(sizeof(*_tmp151));
_tmp151->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp152=_cycalloc(
sizeof(*_tmp152));_tmp152[0]=({struct Cyc_Absyn_Enum_d_struct _tmp153;_tmp153.tag=
6;_tmp153.f1=_tmp14E;_tmp153;});_tmp152;}));_tmp151->loc=loc;_tmp151;});_tmp150->tl=
0;_tmp150;});_npop_handler(0);return _tmp154;}}_LLA4: if(*((int*)_tmp12A)!= 13)
goto _LLA6;_tmp13B=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp12A)->f1;_LLA5: {
struct Cyc_Absyn_Enumdecl*_tmp156=({struct Cyc_Absyn_Enumdecl*_tmp15D=_cycalloc(
sizeof(*_tmp15D));_tmp15D->sc=(void*)s;_tmp15D->name=Cyc_Parse_gensym_enum();
_tmp15D->fields=({struct Cyc_Core_Opt*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->v=
_tmp13B;_tmp15E;});_tmp15D;});if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp157="bad attributes on enum";
_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,23));}),loc);{
struct Cyc_List_List*_tmp15C=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*
_tmp158));_tmp158->hd=({struct Cyc_Absyn_Decl*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp15A=_cycalloc(
sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Absyn_Enum_d_struct _tmp15B;_tmp15B.tag=
6;_tmp15B.f1=_tmp156;_tmp15B;});_tmp15A;}));_tmp159->loc=loc;_tmp159;});_tmp158->tl=
0;_tmp158;});_npop_handler(0);return _tmp15C;}}_LLA6:;_LLA7: Cyc_Parse_err(({const
char*_tmp15F="missing declarator";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size(_tmp15F,19));}),loc);{struct Cyc_List_List*_tmp160=0;
_npop_handler(0);return _tmp160;}_LL9B:;}}else{void*t=_tmp11C.f1;struct Cyc_List_List*
_tmp161=Cyc_Parse_apply_tmss(mkrgn,_tmp112,t,_tmp11A,_tmp114);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp162="initializer in typedef declaration";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,35));}),loc);{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(
struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp161);
if(_tmp11C.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmp11C.f2)->v;{
void*_tmp163=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp164;struct Cyc_Absyn_Tuniondecl*
_tmp165;struct Cyc_Absyn_Enumdecl*_tmp166;_LLA9: if(_tmp163 <= (void*)2)goto _LLAF;
if(*((int*)_tmp163)!= 4)goto _LLAB;_tmp164=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp163)->f1;_LLAA:(void*)(_tmp164->sc=(void*)s);_tmp164->attributes=_tmp114;
_tmp114=0;goto _LLA8;_LLAB: if(*((int*)_tmp163)!= 5)goto _LLAD;_tmp165=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp163)->f1;_LLAC:(void*)(_tmp165->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp163)!= 6)goto _LLAF;_tmp166=((struct Cyc_Absyn_Enum_d_struct*)_tmp163)->f1;
_LLAE:(void*)(_tmp166->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp167="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,70));}),loc);
goto _LLA8;_LLA8:;}decls=({struct Cyc_List_List*_tmp168=_cycalloc(sizeof(*_tmp168));
_tmp168->hd=d;_tmp168->tl=decls;_tmp168;});}{struct Cyc_List_List*_tmp169=decls;
_npop_handler(0);return _tmp169;}}}else{if(_tmp11C.f2 != 0)Cyc_Parse_unimp(({const
char*_tmp16A="nested type declaration within declarator";_tag_dynforward(_tmp16A,
sizeof(char),_get_zero_arr_size(_tmp16A,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp16B=_tmp161;for(0;_tmp16B != 0;(_tmp16B=_tmp16B->tl,
_tmp11B=_tmp11B->tl)){struct _tuple7 _tmp16D;struct _tuple1*_tmp16E;struct Cyc_Absyn_Tqual
_tmp16F;void*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*_tmp172;
struct _tuple7*_tmp16C=(struct _tuple7*)_tmp16B->hd;_tmp16D=*_tmp16C;_tmp16E=
_tmp16D.f1;_tmp16F=_tmp16D.f2;_tmp170=_tmp16D.f3;_tmp171=_tmp16D.f4;_tmp172=
_tmp16D.f5;if(_tmp171 != 0)({void*_tmp173[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp174="bad type params, ignoring";_tag_dynforward(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,26));}),_tag_dynforward(_tmp173,sizeof(void*),0));});
if(_tmp11B == 0)({void*_tmp175[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp176="unexpected NULL in parse!";_tag_dynforward(_tmp176,sizeof(char),
_get_zero_arr_size(_tmp176,26));}),_tag_dynforward(_tmp175,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp177=(struct Cyc_Absyn_Exp*)_tmp11B->hd;struct Cyc_Absyn_Vardecl*
_tmp178=Cyc_Absyn_new_vardecl(_tmp16E,_tmp170,_tmp177);_tmp178->tq=_tmp16F;(void*)(
_tmp178->sc=(void*)s);_tmp178->attributes=_tmp172;{struct Cyc_Absyn_Decl*_tmp179=({
struct Cyc_Absyn_Decl*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[
0]=({struct Cyc_Absyn_Var_d_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=_tmp178;
_tmp17D;});_tmp17C;}));_tmp17B->loc=loc;_tmp17B;});decls=({struct Cyc_List_List*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=_tmp179;_tmp17A->tl=decls;
_tmp17A;});}}}}{struct Cyc_List_List*_tmp17E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp17E;}}}}}}}}};_pop_region(
mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dynforward_ptr)s)== 1  || Cyc_strlen((struct
_dynforward_ptr)s)== 2)switch(*((const char*)_check_dynforward_subscript(s,
sizeof(char),0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case
'B': _LLB3: return(void*)2;case 'R': _LLB4: return(void*)3;case 'U': _LLB5: if(*((const
char*)_check_dynforward_subscript(s,sizeof(char),1))== 'R')return(void*)4;else{
break;}case 'T': _LLB6: if(*((const char*)_check_dynforward_subscript(s,sizeof(char),
1))== 'R')return(void*)5;else{break;}case 'E': _LLB7: return(void*)6;case 'I': _LLB8:
return(void*)7;default: _LLB9: break;}Cyc_Parse_err((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp182;_tmp182.tag=1;_tmp182.f1=(unsigned long)((int)Cyc_strlen((
struct _dynforward_ptr)s));{struct Cyc_String_pa_struct _tmp181;_tmp181.tag=0;
_tmp181.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp17F[2]={&
_tmp181,& _tmp182};Cyc_aprintf(({const char*_tmp180="bad kind: %s; strlen=%d";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,24));}),
_tag_dynforward(_tmp17F,sizeof(void*),2));}}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp183=(void*)p->r;
struct _tuple1*_tmp184;struct Cyc_Absyn_Vardecl*_tmp185;struct Cyc_Absyn_Pat*
_tmp186;struct Cyc_Absyn_Pat _tmp187;void*_tmp188;struct Cyc_Absyn_Pat*_tmp189;void*
_tmp18A;int _tmp18B;char _tmp18C;struct _dynforward_ptr _tmp18D;struct _tuple1*
_tmp18E;struct Cyc_List_List*_tmp18F;int _tmp190;struct Cyc_Absyn_Exp*_tmp191;_LLBC:
if(_tmp183 <= (void*)2)goto _LLC2;if(*((int*)_tmp183)!= 12)goto _LLBE;_tmp184=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp183)->f1;_LLBD: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp192=_cycalloc(sizeof(*_tmp192));
_tmp192[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp193;_tmp193.tag=2;_tmp193.f1=
_tmp184;_tmp193;});_tmp192;}),p->loc);_LLBE: if(*((int*)_tmp183)!= 1)goto _LLC0;
_tmp185=((struct Cyc_Absyn_Reference_p_struct*)_tmp183)->f1;_tmp186=((struct Cyc_Absyn_Reference_p_struct*)
_tmp183)->f2;_tmp187=*_tmp186;_tmp188=(void*)_tmp187.r;if((int)_tmp188 != 0)goto
_LLC0;_LLBF: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp195;_tmp195.tag=2;_tmp195.f1=_tmp185->name;_tmp195;});_tmp194;}),p->loc),p->loc);
_LLC0: if(*((int*)_tmp183)!= 4)goto _LLC2;_tmp189=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp183)->f1;_LLC1: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp189),p->loc);
_LLC2: if((int)_tmp183 != 1)goto _LLC4;_LLC3: return Cyc_Absyn_null_exp(p->loc);_LLC4:
if(_tmp183 <= (void*)2)goto _LLCE;if(*((int*)_tmp183)!= 7)goto _LLC6;_tmp18A=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp183)->f1;_tmp18B=((struct Cyc_Absyn_Int_p_struct*)
_tmp183)->f2;_LLC5: return Cyc_Absyn_int_exp(_tmp18A,_tmp18B,p->loc);_LLC6: if(*((
int*)_tmp183)!= 8)goto _LLC8;_tmp18C=((struct Cyc_Absyn_Char_p_struct*)_tmp183)->f1;
_LLC7: return Cyc_Absyn_char_exp(_tmp18C,p->loc);_LLC8: if(*((int*)_tmp183)!= 9)
goto _LLCA;_tmp18D=((struct Cyc_Absyn_Float_p_struct*)_tmp183)->f1;_LLC9: return Cyc_Absyn_float_exp(
_tmp18D,p->loc);_LLCA: if(*((int*)_tmp183)!= 13)goto _LLCC;_tmp18E=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp183)->f1;_tmp18F=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp183)->f2;
_tmp190=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp183)->f3;if(_tmp190 != 0)goto
_LLCC;_LLCB: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp197;_tmp197.tag=2;_tmp197.f1=_tmp18E;_tmp197;});_tmp196;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp18F);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCC: if(*((int*)_tmp183)!= 14)goto _LLCE;_tmp191=((struct Cyc_Absyn_Exp_p_struct*)
_tmp183)->f1;_LLCD: return _tmp191;_LLCE:;_LLCF: Cyc_Parse_err(({const char*_tmp198="cannot mix patterns and expressions in case";
_tag_dynforward(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,44));}),p->loc);
return Cyc_Absyn_null_exp(p->loc);_LLBB:;}struct _tuple11{void*f1;int f2;};struct
Cyc_Int_tok_struct{int tag;struct _tuple11 f1;};struct Cyc_Char_tok_struct{int tag;
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
YYINITIALSVAL;};static char _tmp19A[14]="$(sign_t,int)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp19A,_tmp19A + 14}};struct _tuple11 Cyc_yyget_Int_tok(
union Cyc_YYSTYPE_union yy1){struct _tuple11 yyzzz;{union Cyc_YYSTYPE_union _tmp19B=
yy1;struct _tuple11 _tmp19C;_LLD1: if((_tmp19B.Int_tok).tag != 0)goto _LLD3;_tmp19C=(
_tmp19B.Int_tok).f1;_LLD2: yyzzz=_tmp19C;goto _LLD0;_LLD3:;_LLD4:(int)_throw((void*)&
Cyc_yyfail_Int_tok);_LLD0:;}return yyzzz;}static char _tmp19E[5]="char";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp19E,
_tmp19E + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){char yyzzz;{union
Cyc_YYSTYPE_union _tmp19F=yy1;char _tmp1A0;_LLD6: if((_tmp19F.Char_tok).tag != 1)
goto _LLD8;_tmp1A0=(_tmp19F.Char_tok).f1;_LLD7: yyzzz=_tmp1A0;goto _LLD5;_LLD8:;
_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD5:;}return yyzzz;}static char
_tmp1A2[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp1A2,_tmp1A2 + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1){struct _dynforward_ptr yyzzz;{union Cyc_YYSTYPE_union
_tmp1A3=yy1;struct _dynforward_ptr _tmp1A4;_LLDB: if((_tmp1A3.String_tok).tag != 2)
goto _LLDD;_tmp1A4=(_tmp1A3.String_tok).f1;_LLDC: yyzzz=_tmp1A4;goto _LLDA;_LLDD:;
_LLDE:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLDA:;}return yyzzz;}static char
_tmp1A6[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1A6,_tmp1A6 + 54}};
struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{
union Cyc_YYSTYPE_union _tmp1A7=yy1;struct _tuple12*_tmp1A8;_LLE0: if((_tmp1A7.YY1).tag
!= 5)goto _LLE2;_tmp1A8=(_tmp1A7.YY1).f1;_LLE1: yyzzz=_tmp1A8;goto _LLDF;_LLE2:;
_LLE3:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDF:;}return yyzzz;}static char _tmp1AA[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp1AA,_tmp1AA + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union _tmp1AB=yy1;struct Cyc_Absyn_Conref*
_tmp1AC;_LLE5: if((_tmp1AB.YY2).tag != 6)goto _LLE7;_tmp1AC=(_tmp1AB.YY2).f1;_LLE6:
yyzzz=_tmp1AC;goto _LLE4;_LLE7:;_LLE8:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}
return yyzzz;}static char _tmp1AE[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp1AE,_tmp1AE + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union
Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1AF=
yy1;struct Cyc_Absyn_Exp*_tmp1B0;_LLEA: if((_tmp1AF.YY3).tag != 7)goto _LLEC;_tmp1B0=(
_tmp1AF.YY3).f1;_LLEB: yyzzz=_tmp1B0;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)&
Cyc_yyfail_YY3);_LLE9:;}return yyzzz;}static char _tmp1B2[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1B2,_tmp1B2 + 
10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE_union _tmp1B3=yy1;struct Cyc_Absyn_Exp*_tmp1B4;_LLEF: if((
_tmp1B3.YY4).tag != 8)goto _LLF1;_tmp1B4=(_tmp1B3.YY4).f1;_LLF0: yyzzz=_tmp1B4;goto
_LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_YY4);_LLEE:;}return yyzzz;}
static char _tmp1B6[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp1B6,_tmp1B6 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1B7=
yy1;struct Cyc_List_List*_tmp1B8;_LLF4: if((_tmp1B7.YY5).tag != 9)goto _LLF6;_tmp1B8=(
_tmp1B7.YY5).f1;_LLF5: yyzzz=_tmp1B8;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)&
Cyc_yyfail_YY5);_LLF3:;}return yyzzz;}static char _tmp1BA[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1BA,
_tmp1BA + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1BB=yy1;struct Cyc_List_List*
_tmp1BC;_LLF9: if((_tmp1BB.YY6).tag != 10)goto _LLFB;_tmp1BC=(_tmp1BB.YY6).f1;_LLFA:
yyzzz=_tmp1BC;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF8:;}
return yyzzz;}static char _tmp1BE[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1BE,_tmp1BE + 9}};void*Cyc_yyget_YY7(union
Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1BF=yy1;void*_tmp1C0;
_LLFE: if((_tmp1BF.YY7).tag != 11)goto _LL100;_tmp1C0=(_tmp1BF.YY7).f1;_LLFF: yyzzz=
_tmp1C0;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY7);_LLFD:;}
return yyzzz;}static char _tmp1C2[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1C2,_tmp1C2 + 16}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C3=yy1;struct Cyc_Core_Opt*_tmp1C4;_LL103: if((_tmp1C3.YY8).tag != 12)goto
_LL105;_tmp1C4=(_tmp1C3.YY8).f1;_LL104: yyzzz=_tmp1C4;goto _LL102;_LL105:;_LL106:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char _tmp1C6[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1C6,_tmp1C6 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1C7=yy1;struct _tuple1*
_tmp1C8;_LL108: if((_tmp1C7.QualId_tok).tag != 4)goto _LL10A;_tmp1C8=(_tmp1C7.QualId_tok).f1;
_LL109: yyzzz=_tmp1C8;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1CA[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1CA,_tmp1CA + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1CB=yy1;struct Cyc_Absyn_Stmt*_tmp1CC;_LL10D: if((_tmp1CB.YY9).tag != 13)goto
_LL10F;_tmp1CC=(_tmp1CB.YY9).f1;_LL10E: yyzzz=_tmp1CC;goto _LL10C;_LL10F:;_LL110:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10C:;}return yyzzz;}static char _tmp1CE[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CE,
_tmp1CE + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1CF=yy1;struct Cyc_List_List*
_tmp1D0;_LL112: if((_tmp1CF.YY10).tag != 14)goto _LL114;_tmp1D0=(_tmp1CF.YY10).f1;
_LL113: yyzzz=_tmp1D0;goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL111:;}return yyzzz;}static char _tmp1D2[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1D2,_tmp1D2 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1D3=yy1;struct Cyc_Absyn_Pat*_tmp1D4;_LL117: if((_tmp1D3.YY11).tag != 15)goto
_LL119;_tmp1D4=(_tmp1D3.YY11).f1;_LL118: yyzzz=_tmp1D4;goto _LL116;_LL119:;_LL11A:(
int)_throw((void*)& Cyc_yyfail_YY11);_LL116:;}return yyzzz;}static char _tmp1D6[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D6,
_tmp1D6 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D7=yy1;struct _tuple13*_tmp1D8;_LL11C:
if((_tmp1D7.YY12).tag != 16)goto _LL11E;_tmp1D8=(_tmp1D7.YY12).f1;_LL11D: yyzzz=
_tmp1D8;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);_LL11B:;}
return yyzzz;}static char _tmp1DA[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1DA,_tmp1DA + 14}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1DB=yy1;struct Cyc_List_List*_tmp1DC;_LL121: if((_tmp1DB.YY13).tag != 17)goto
_LL123;_tmp1DC=(_tmp1DB.YY13).f1;_LL122: yyzzz=_tmp1DC;goto _LL120;_LL123:;_LL124:(
int)_throw((void*)& Cyc_yyfail_YY13);_LL120:;}return yyzzz;}static char _tmp1DE[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1DE,
_tmp1DE + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){struct
_tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1DF=yy1;struct _tuple14*_tmp1E0;_LL126:
if((_tmp1DF.YY14).tag != 18)goto _LL128;_tmp1E0=(_tmp1DF.YY14).f1;_LL127: yyzzz=
_tmp1E0;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);_LL125:;}
return yyzzz;}static char _tmp1E2[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1E2,
_tmp1E2 + 39}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1E3=yy1;struct Cyc_List_List*
_tmp1E4;_LL12B: if((_tmp1E3.YY15).tag != 19)goto _LL12D;_tmp1E4=(_tmp1E3.YY15).f1;
_LL12C: yyzzz=_tmp1E4;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12A:;}return yyzzz;}static char _tmp1E6[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1E6,
_tmp1E6 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1E7=yy1;struct _tuple13*_tmp1E8;_LL130:
if((_tmp1E7.YY16).tag != 20)goto _LL132;_tmp1E8=(_tmp1E7.YY16).f1;_LL131: yyzzz=
_tmp1E8;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12F:;}
return yyzzz;}static char _tmp1EA[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1EA,_tmp1EA + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1EB=yy1;struct Cyc_Absyn_Fndecl*_tmp1EC;_LL135: if((_tmp1EB.YY17).tag != 21)
goto _LL137;_tmp1EC=(_tmp1EB.YY17).f1;_LL136: yyzzz=_tmp1EC;goto _LL134;_LL137:;
_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);_LL134:;}return yyzzz;}static char
_tmp1EE[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1EE,_tmp1EE + 15}};struct Cyc_List_List*Cyc_yyget_YY18(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1EF=
yy1;struct Cyc_List_List*_tmp1F0;_LL13A: if((_tmp1EF.YY18).tag != 22)goto _LL13C;
_tmp1F0=(_tmp1EF.YY18).f1;_LL13B: yyzzz=_tmp1F0;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY18);_LL139:;}return yyzzz;}static char _tmp1F2[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1F2,
_tmp1F2 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1F3=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1F4;_LL13F: if((_tmp1F3.YY19).tag != 23)goto
_LL141;_tmp1F4=(_tmp1F3.YY19).f1;_LL140: yyzzz=_tmp1F4;goto _LL13E;_LL141:;_LL142:(
int)_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static char _tmp1F6[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1F6,
_tmp1F6 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){struct
_tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1F7=yy1;struct _tuple15*_tmp1F8;_LL144:
if((_tmp1F7.YY20).tag != 24)goto _LL146;_tmp1F8=(_tmp1F7.YY20).f1;_LL145: yyzzz=
_tmp1F8;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);_LL143:;}
return yyzzz;}static char _tmp1FA[35]="list_t<$(declarator_t,exp_opt_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1FA,_tmp1FA + 
35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1FB=yy1;struct Cyc_List_List*_tmp1FC;_LL149: if((
_tmp1FB.YY21).tag != 25)goto _LL14B;_tmp1FC=(_tmp1FB.YY21).f1;_LL14A: yyzzz=_tmp1FC;
goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY21);_LL148:;}return
yyzzz;}static char _tmp1FE[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1FE,_tmp1FE + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1FF=yy1;void*
_tmp200;_LL14E: if((_tmp1FF.YY22).tag != 26)goto _LL150;_tmp200=(_tmp1FF.YY22).f1;
_LL14F: yyzzz=_tmp200;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14D:;}return yyzzz;}static char _tmp202[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp202,_tmp202 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp203=yy1;void*
_tmp204;_LL153: if((_tmp203.YY23).tag != 27)goto _LL155;_tmp204=(_tmp203.YY23).f1;
_LL154: yyzzz=_tmp204;goto _LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL152:;}return yyzzz;}static char _tmp206[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp206,_tmp206 + 12}};void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp207=yy1;void*
_tmp208;_LL158: if((_tmp207.YY24).tag != 28)goto _LL15A;_tmp208=(_tmp207.YY24).f1;
_LL159: yyzzz=_tmp208;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL157:;}return yyzzz;}static char _tmp20A[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp20A,_tmp20A + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp20B=yy1;struct Cyc_Absyn_Tqual _tmp20C;_LL15D: if((_tmp20B.YY25).tag != 29)goto
_LL15F;_tmp20C=(_tmp20B.YY25).f1;_LL15E: yyzzz=_tmp20C;goto _LL15C;_LL15F:;_LL160:(
int)_throw((void*)& Cyc_yyfail_YY25);_LL15C:;}return yyzzz;}static char _tmp20E[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp20E,
_tmp20E + 20}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp20F=yy1;struct Cyc_List_List*
_tmp210;_LL162: if((_tmp20F.YY26).tag != 30)goto _LL164;_tmp210=(_tmp20F.YY26).f1;
_LL163: yyzzz=_tmp210;goto _LL161;_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL161:;}return yyzzz;}static char _tmp212[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp212,_tmp212 + 
28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp213=yy1;struct Cyc_List_List*_tmp214;_LL167: if((
_tmp213.YY27).tag != 31)goto _LL169;_tmp214=(_tmp213.YY27).f1;_LL168: yyzzz=_tmp214;
goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);_LL166:;}return
yyzzz;}static char _tmp216[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp216,_tmp216 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp217=yy1;struct Cyc_List_List*_tmp218;_LL16C: if((_tmp217.YY28).tag != 32)goto
_LL16E;_tmp218=(_tmp217.YY28).f1;_LL16D: yyzzz=_tmp218;goto _LL16B;_LL16E:;_LL16F:(
int)_throw((void*)& Cyc_yyfail_YY28);_LL16B:;}return yyzzz;}static char _tmp21A[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp21A,
_tmp21A + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union _tmp21B=yy1;struct
Cyc_Parse_Declarator*_tmp21C;_LL171: if((_tmp21B.YY29).tag != 33)goto _LL173;
_tmp21C=(_tmp21B.YY29).f1;_LL172: yyzzz=_tmp21C;goto _LL170;_LL173:;_LL174:(int)
_throw((void*)& Cyc_yyfail_YY29);_LL170:;}return yyzzz;}static char _tmp21E[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp21E,
_tmp21E + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union _tmp21F=yy1;
struct Cyc_Parse_Abstractdeclarator*_tmp220;_LL176: if((_tmp21F.YY30).tag != 34)
goto _LL178;_tmp220=(_tmp21F.YY30).f1;_LL177: yyzzz=_tmp220;goto _LL175;_LL178:;
_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);_LL175:;}return yyzzz;}static char
_tmp222[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp222,_tmp222 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1){int yyzzz;{
union Cyc_YYSTYPE_union _tmp223=yy1;int _tmp224;_LL17B: if((_tmp223.YY31).tag != 35)
goto _LL17D;_tmp224=(_tmp223.YY31).f1;_LL17C: yyzzz=_tmp224;goto _LL17A;_LL17D:;
_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);_LL17A:;}return yyzzz;}static char
_tmp226[8]="scope_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{
_tmp226,_tmp226 + 8}};void*Cyc_yyget_YY32(union Cyc_YYSTYPE_union yy1){void*yyzzz;{
union Cyc_YYSTYPE_union _tmp227=yy1;void*_tmp228;_LL180: if((_tmp227.YY32).tag != 36)
goto _LL182;_tmp228=(_tmp227.YY32).f1;_LL181: yyzzz=_tmp228;goto _LL17F;_LL182:;
_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17F:;}return yyzzz;}static char
_tmp22A[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp22A,_tmp22A + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union
_tmp22B=yy1;struct Cyc_Absyn_Tunionfield*_tmp22C;_LL185: if((_tmp22B.YY33).tag != 
37)goto _LL187;_tmp22C=(_tmp22B.YY33).f1;_LL186: yyzzz=_tmp22C;goto _LL184;_LL187:;
_LL188:(int)_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return yyzzz;}static char
_tmp22E[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={
Cyc_Core_Failure,{_tmp22E,_tmp22E + 22}};struct Cyc_List_List*Cyc_yyget_YY34(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp22F=
yy1;struct Cyc_List_List*_tmp230;_LL18A: if((_tmp22F.YY34).tag != 38)goto _LL18C;
_tmp230=(_tmp22F.YY34).f1;_LL18B: yyzzz=_tmp230;goto _LL189;_LL18C:;_LL18D:(int)
_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return yyzzz;}static char _tmp232[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp232,
_tmp232 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){struct
_tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp233=yy1;struct _tuple16*_tmp234;_LL18F:
if((_tmp233.YY35).tag != 39)goto _LL191;_tmp234=(_tmp233.YY35).f1;_LL190: yyzzz=
_tmp234;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);_LL18E:;}
return yyzzz;}static char _tmp236[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp236,_tmp236 + 14}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp237=yy1;struct Cyc_List_List*_tmp238;_LL194: if((_tmp237.YY36).tag != 40)goto
_LL196;_tmp238=(_tmp237.YY36).f1;_LL195: yyzzz=_tmp238;goto _LL193;_LL196:;_LL197:(
int)_throw((void*)& Cyc_yyfail_YY36);_LL193:;}return yyzzz;}static char _tmp23A[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp23A,
_tmp23A + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){struct
_tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp23B=yy1;struct _tuple2*_tmp23C;_LL199:
if((_tmp23B.YY37).tag != 41)goto _LL19B;_tmp23C=(_tmp23B.YY37).f1;_LL19A: yyzzz=
_tmp23C;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY37);_LL198:;}
return yyzzz;}static char _tmp23E[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp23E,
_tmp23E + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp23F=yy1;struct Cyc_List_List*
_tmp240;_LL19E: if((_tmp23F.YY38).tag != 42)goto _LL1A0;_tmp240=(_tmp23F.YY38).f1;
_LL19F: yyzzz=_tmp240;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL19D:;}return yyzzz;}static char _tmp242[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp242,
_tmp242 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){struct
_tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp243=yy1;struct _tuple17*_tmp244;_LL1A3:
if((_tmp243.YY39).tag != 43)goto _LL1A5;_tmp244=(_tmp243.YY39).f1;_LL1A4: yyzzz=
_tmp244;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A2:;}
return yyzzz;}static char _tmp246[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp246,_tmp246 + 15}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp247=yy1;struct Cyc_List_List*_tmp248;_LL1A8: if((_tmp247.YY40).tag != 44)goto
_LL1AA;_tmp248=(_tmp247.YY40).f1;_LL1A9: yyzzz=_tmp248;goto _LL1A7;_LL1AA:;_LL1AB:(
int)_throw((void*)& Cyc_yyfail_YY40);_LL1A7:;}return yyzzz;}static char _tmp24A[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp24A,
_tmp24A + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp24B=yy1;struct Cyc_List_List*
_tmp24C;_LL1AD: if((_tmp24B.YY41).tag != 45)goto _LL1AF;_tmp24C=(_tmp24B.YY41).f1;
_LL1AE: yyzzz=_tmp24C;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AC:;}return yyzzz;}static char _tmp24E[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp24E,_tmp24E + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp24F=yy1;void*
_tmp250;_LL1B2: if((_tmp24F.YY42).tag != 46)goto _LL1B4;_tmp250=(_tmp24F.YY42).f1;
_LL1B3: yyzzz=_tmp250;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B1:;}return yyzzz;}static char _tmp252[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp252,_tmp252 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp253=yy1;void*
_tmp254;_LL1B7: if((_tmp253.YY43).tag != 47)goto _LL1B9;_tmp254=(_tmp253.YY43).f1;
_LL1B8: yyzzz=_tmp254;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B6:;}return yyzzz;}static char _tmp256[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp256,_tmp256 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp257=yy1;void*
_tmp258;_LL1BC: if((_tmp257.YY44).tag != 48)goto _LL1BE;_tmp258=(_tmp257.YY44).f1;
_LL1BD: yyzzz=_tmp258;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp25A[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp25A,_tmp25A + 20}};
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp25B=yy1;struct Cyc_List_List*_tmp25C;_LL1C1: if((
_tmp25B.YY45).tag != 49)goto _LL1C3;_tmp25C=(_tmp25B.YY45).f1;_LL1C2: yyzzz=_tmp25C;
goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C0:;}return
yyzzz;}static char _tmp25E[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp25E,_tmp25E + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp25F=yy1;void*
_tmp260;_LL1C6: if((_tmp25F.YY46).tag != 50)goto _LL1C8;_tmp260=(_tmp25F.YY46).f1;
_LL1C7: yyzzz=_tmp260;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C5:;}return yyzzz;}static char _tmp262[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp262,_tmp262 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp263=yy1;struct Cyc_Absyn_Enumfield*_tmp264;_LL1CB: if((
_tmp263.YY47).tag != 51)goto _LL1CD;_tmp264=(_tmp263.YY47).f1;_LL1CC: yyzzz=_tmp264;
goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CA:;}return
yyzzz;}static char _tmp266[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp266,_tmp266 + 20}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp267=yy1;struct Cyc_List_List*_tmp268;_LL1D0: if((_tmp267.YY48).tag != 52)goto
_LL1D2;_tmp268=(_tmp267.YY48).f1;_LL1D1: yyzzz=_tmp268;goto _LL1CF;_LL1D2:;_LL1D3:(
int)_throw((void*)& Cyc_yyfail_YY48);_LL1CF:;}return yyzzz;}static char _tmp26A[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp26A,
_tmp26A + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp26B=yy1;struct Cyc_Core_Opt*
_tmp26C;_LL1D5: if((_tmp26B.YY49).tag != 53)goto _LL1D7;_tmp26C=(_tmp26B.YY49).f1;
_LL1D6: yyzzz=_tmp26C;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D4:;}return yyzzz;}static char _tmp26E[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp26E,_tmp26E + 
26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp26F=yy1;struct Cyc_List_List*_tmp270;_LL1DA: if((
_tmp26F.YY50).tag != 54)goto _LL1DC;_tmp270=(_tmp26F.YY50).f1;_LL1DB: yyzzz=_tmp270;
goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY50);_LL1D9:;}return
yyzzz;}static char _tmp272[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp272,_tmp272 + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp273=yy1;struct Cyc_Absyn_Conref*_tmp274;_LL1DF: if((_tmp273.YY51).tag != 55)
goto _LL1E1;_tmp274=(_tmp273.YY51).f1;_LL1E0: yyzzz=_tmp274;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}return yyzzz;}static char
_tmp276[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp276,_tmp276 + 40}};struct Cyc_List_List*Cyc_yyget_YY52(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp277=yy1;struct Cyc_List_List*_tmp278;_LL1E4: if((_tmp277.YY52).tag != 56)goto
_LL1E6;_tmp278=(_tmp277.YY52).f1;_LL1E5: yyzzz=_tmp278;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc(){return({struct Cyc_Yyltype _tmp279;_tmp279.timestamp=0;_tmp279.first_line=
0;_tmp279.first_column=0;_tmp279.last_line=0;_tmp279.last_column=0;_tmp279;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[360]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,131,2,2,113,129,127,2,
110,111,119,124,108,122,116,128,2,2,2,2,2,2,2,2,2,2,115,105,114,109,123,121,120,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,117,2,118,126,112,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,106,125,107,130,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104};static char
_tmp27A[2]="$";static char _tmp27B[6]="error";static char _tmp27C[12]="$undefined.";
static char _tmp27D[5]="AUTO";static char _tmp27E[9]="REGISTER";static char _tmp27F[7]="STATIC";
static char _tmp280[7]="EXTERN";static char _tmp281[8]="TYPEDEF";static char _tmp282[5]="VOID";
static char _tmp283[5]="CHAR";static char _tmp284[6]="SHORT";static char _tmp285[4]="INT";
static char _tmp286[5]="LONG";static char _tmp287[6]="FLOAT";static char _tmp288[7]="DOUBLE";
static char _tmp289[7]="SIGNED";static char _tmp28A[9]="UNSIGNED";static char _tmp28B[
6]="CONST";static char _tmp28C[9]="VOLATILE";static char _tmp28D[9]="RESTRICT";
static char _tmp28E[7]="STRUCT";static char _tmp28F[6]="UNION";static char _tmp290[5]="CASE";
static char _tmp291[8]="DEFAULT";static char _tmp292[7]="INLINE";static char _tmp293[7]="SIZEOF";
static char _tmp294[9]="OFFSETOF";static char _tmp295[3]="IF";static char _tmp296[5]="ELSE";
static char _tmp297[7]="SWITCH";static char _tmp298[6]="WHILE";static char _tmp299[3]="DO";
static char _tmp29A[4]="FOR";static char _tmp29B[5]="GOTO";static char _tmp29C[9]="CONTINUE";
static char _tmp29D[6]="BREAK";static char _tmp29E[7]="RETURN";static char _tmp29F[5]="ENUM";
static char _tmp2A0[8]="NULL_kw";static char _tmp2A1[4]="LET";static char _tmp2A2[6]="THROW";
static char _tmp2A3[4]="TRY";static char _tmp2A4[6]="CATCH";static char _tmp2A5[7]="EXPORT";
static char _tmp2A6[4]="NEW";static char _tmp2A7[9]="ABSTRACT";static char _tmp2A8[9]="FALLTHRU";
static char _tmp2A9[6]="USING";static char _tmp2AA[10]="NAMESPACE";static char _tmp2AB[
7]="TUNION";static char _tmp2AC[8]="XTUNION";static char _tmp2AD[7]="MALLOC";static
char _tmp2AE[8]="RMALLOC";static char _tmp2AF[7]="CALLOC";static char _tmp2B0[8]="RCALLOC";
static char _tmp2B1[5]="SWAP";static char _tmp2B2[9]="REGION_T";static char _tmp2B3[9]="SIZEOF_T";
static char _tmp2B4[6]="TAG_T";static char _tmp2B5[7]="REGION";static char _tmp2B6[5]="RNEW";
static char _tmp2B7[8]="REGIONS";static char _tmp2B8[13]="RESET_REGION";static char
_tmp2B9[4]="GEN";static char _tmp2BA[14]="NOZEROTERM_kw";static char _tmp2BB[12]="ZEROTERM_kw";
static char _tmp2BC[7]="PORTON";static char _tmp2BD[8]="PORTOFF";static char _tmp2BE[8]="FLAT_kw";
static char _tmp2BF[12]="DYNREGION_T";static char _tmp2C0[7]="PTR_OP";static char
_tmp2C1[7]="INC_OP";static char _tmp2C2[7]="DEC_OP";static char _tmp2C3[8]="LEFT_OP";
static char _tmp2C4[9]="RIGHT_OP";static char _tmp2C5[6]="LE_OP";static char _tmp2C6[6]="GE_OP";
static char _tmp2C7[6]="EQ_OP";static char _tmp2C8[6]="NE_OP";static char _tmp2C9[7]="AND_OP";
static char _tmp2CA[6]="OR_OP";static char _tmp2CB[11]="MUL_ASSIGN";static char
_tmp2CC[11]="DIV_ASSIGN";static char _tmp2CD[11]="MOD_ASSIGN";static char _tmp2CE[11]="ADD_ASSIGN";
static char _tmp2CF[11]="SUB_ASSIGN";static char _tmp2D0[12]="LEFT_ASSIGN";static
char _tmp2D1[13]="RIGHT_ASSIGN";static char _tmp2D2[11]="AND_ASSIGN";static char
_tmp2D3[11]="XOR_ASSIGN";static char _tmp2D4[10]="OR_ASSIGN";static char _tmp2D5[9]="ELLIPSIS";
static char _tmp2D6[11]="LEFT_RIGHT";static char _tmp2D7[12]="COLON_COLON";static
char _tmp2D8[11]="IDENTIFIER";static char _tmp2D9[17]="INTEGER_CONSTANT";static char
_tmp2DA[7]="STRING";static char _tmp2DB[19]="CHARACTER_CONSTANT";static char _tmp2DC[
18]="FLOATING_CONSTANT";static char _tmp2DD[9]="TYPE_VAR";static char _tmp2DE[13]="TYPEDEF_NAME";
static char _tmp2DF[16]="QUAL_IDENTIFIER";static char _tmp2E0[18]="QUAL_TYPEDEF_NAME";
static char _tmp2E1[13]="TYPE_INTEGER";static char _tmp2E2[10]="ATTRIBUTE";static
char _tmp2E3[4]="';'";static char _tmp2E4[4]="'{'";static char _tmp2E5[4]="'}'";
static char _tmp2E6[4]="','";static char _tmp2E7[4]="'='";static char _tmp2E8[4]="'('";
static char _tmp2E9[4]="')'";static char _tmp2EA[4]="'_'";static char _tmp2EB[4]="'$'";
static char _tmp2EC[4]="'<'";static char _tmp2ED[4]="':'";static char _tmp2EE[4]="'.'";
static char _tmp2EF[4]="'['";static char _tmp2F0[4]="']'";static char _tmp2F1[4]="'*'";
static char _tmp2F2[4]="'@'";static char _tmp2F3[4]="'?'";static char _tmp2F4[4]="'-'";
static char _tmp2F5[4]="'>'";static char _tmp2F6[4]="'+'";static char _tmp2F7[4]="'|'";
static char _tmp2F8[4]="'^'";static char _tmp2F9[4]="'&'";static char _tmp2FA[4]="'/'";
static char _tmp2FB[4]="'%'";static char _tmp2FC[4]="'~'";static char _tmp2FD[4]="'!'";
static char _tmp2FE[5]="prog";static char _tmp2FF[17]="translation_unit";static char
_tmp300[12]="export_list";static char _tmp301[19]="export_list_values";static char
_tmp302[21]="external_declaration";static char _tmp303[20]="function_definition";
static char _tmp304[21]="function_definition2";static char _tmp305[13]="using_action";
static char _tmp306[15]="unusing_action";static char _tmp307[17]="namespace_action";
static char _tmp308[19]="unnamespace_action";static char _tmp309[12]="declaration";
static char _tmp30A[17]="declaration_list";static char _tmp30B[23]="declaration_specifiers";
static char _tmp30C[24]="storage_class_specifier";static char _tmp30D[15]="attributes_opt";
static char _tmp30E[11]="attributes";static char _tmp30F[15]="attribute_list";static
char _tmp310[10]="attribute";static char _tmp311[15]="type_specifier";static char
_tmp312[25]="type_specifier_notypedef";static char _tmp313[5]="kind";static char
_tmp314[15]="type_qualifier";static char _tmp315[15]="enum_specifier";static char
_tmp316[11]="enum_field";static char _tmp317[22]="enum_declaration_list";static
char _tmp318[26]="struct_or_union_specifier";static char _tmp319[16]="type_params_opt";
static char _tmp31A[16]="struct_or_union";static char _tmp31B[24]="struct_declaration_list";
static char _tmp31C[25]="struct_declaration_list0";static char _tmp31D[21]="init_declarator_list";
static char _tmp31E[22]="init_declarator_list0";static char _tmp31F[16]="init_declarator";
static char _tmp320[19]="struct_declaration";static char _tmp321[25]="specifier_qualifier_list";
static char _tmp322[35]="notypedef_specifier_qualifier_list";static char _tmp323[23]="struct_declarator_list";
static char _tmp324[24]="struct_declarator_list0";static char _tmp325[18]="struct_declarator";
static char _tmp326[17]="tunion_specifier";static char _tmp327[9]="flat_opt";static
char _tmp328[18]="tunion_or_xtunion";static char _tmp329[17]="tunionfield_list";
static char _tmp32A[18]="tunionfield_scope";static char _tmp32B[12]="tunionfield";
static char _tmp32C[11]="declarator";static char _tmp32D[23]="declarator_withtypedef";
static char _tmp32E[18]="direct_declarator";static char _tmp32F[30]="direct_declarator_withtypedef";
static char _tmp330[8]="pointer";static char _tmp331[12]="one_pointer";static char
_tmp332[23]="pointer_null_and_bound";static char _tmp333[14]="pointer_bound";
static char _tmp334[18]="zeroterm_qual_opt";static char _tmp335[12]="opt_rgn_opt";
static char _tmp336[8]="rgn_opt";static char _tmp337[11]="tqual_list";static char
_tmp338[20]="parameter_type_list";static char _tmp339[9]="type_var";static char
_tmp33A[16]="optional_effect";static char _tmp33B[19]="optional_rgn_order";static
char _tmp33C[10]="rgn_order";static char _tmp33D[16]="optional_inject";static char
_tmp33E[11]="effect_set";static char _tmp33F[14]="atomic_effect";static char _tmp340[
11]="region_set";static char _tmp341[15]="parameter_list";static char _tmp342[22]="parameter_declaration";
static char _tmp343[16]="identifier_list";static char _tmp344[17]="identifier_list0";
static char _tmp345[12]="initializer";static char _tmp346[18]="array_initializer";
static char _tmp347[17]="initializer_list";static char _tmp348[12]="designation";
static char _tmp349[16]="designator_list";static char _tmp34A[11]="designator";
static char _tmp34B[10]="type_name";static char _tmp34C[14]="any_type_name";static
char _tmp34D[15]="type_name_list";static char _tmp34E[20]="abstract_declarator";
static char _tmp34F[27]="direct_abstract_declarator";static char _tmp350[10]="statement";
static char _tmp351[13]="open_exp_opt";static char _tmp352[18]="labeled_statement";
static char _tmp353[21]="expression_statement";static char _tmp354[19]="compound_statement";
static char _tmp355[16]="block_item_list";static char _tmp356[20]="selection_statement";
static char _tmp357[15]="switch_clauses";static char _tmp358[20]="iteration_statement";
static char _tmp359[15]="jump_statement";static char _tmp35A[12]="exp_pattern";
static char _tmp35B[20]="conditional_pattern";static char _tmp35C[19]="logical_or_pattern";
static char _tmp35D[20]="logical_and_pattern";static char _tmp35E[21]="inclusive_or_pattern";
static char _tmp35F[21]="exclusive_or_pattern";static char _tmp360[12]="and_pattern";
static char _tmp361[17]="equality_pattern";static char _tmp362[19]="relational_pattern";
static char _tmp363[14]="shift_pattern";static char _tmp364[17]="additive_pattern";
static char _tmp365[23]="multiplicative_pattern";static char _tmp366[13]="cast_pattern";
static char _tmp367[14]="unary_pattern";static char _tmp368[16]="postfix_pattern";
static char _tmp369[16]="primary_pattern";static char _tmp36A[8]="pattern";static
char _tmp36B[19]="tuple_pattern_list";static char _tmp36C[20]="tuple_pattern_list0";
static char _tmp36D[14]="field_pattern";static char _tmp36E[19]="field_pattern_list";
static char _tmp36F[20]="field_pattern_list0";static char _tmp370[11]="expression";
static char _tmp371[22]="assignment_expression";static char _tmp372[20]="assignment_operator";
static char _tmp373[23]="conditional_expression";static char _tmp374[20]="constant_expression";
static char _tmp375[22]="logical_or_expression";static char _tmp376[23]="logical_and_expression";
static char _tmp377[24]="inclusive_or_expression";static char _tmp378[24]="exclusive_or_expression";
static char _tmp379[15]="and_expression";static char _tmp37A[20]="equality_expression";
static char _tmp37B[22]="relational_expression";static char _tmp37C[17]="shift_expression";
static char _tmp37D[20]="additive_expression";static char _tmp37E[26]="multiplicative_expression";
static char _tmp37F[16]="cast_expression";static char _tmp380[17]="unary_expression";
static char _tmp381[15]="unary_operator";static char _tmp382[19]="postfix_expression";
static char _tmp383[19]="primary_expression";static char _tmp384[25]="argument_expression_list";
static char _tmp385[26]="argument_expression_list0";static char _tmp386[9]="constant";
static char _tmp387[20]="qual_opt_identifier";static char _tmp388[17]="qual_opt_typedef";
static char _tmp389[18]="struct_union_name";static char _tmp38A[11]="field_name";
static char _tmp38B[12]="right_angle";static struct _dynforward_ptr Cyc_yytname[274]={{
_tmp27A,_tmp27A + 2},{_tmp27B,_tmp27B + 6},{_tmp27C,_tmp27C + 12},{_tmp27D,_tmp27D + 
5},{_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F + 7},{_tmp280,_tmp280 + 7},{_tmp281,
_tmp281 + 8},{_tmp282,_tmp282 + 5},{_tmp283,_tmp283 + 5},{_tmp284,_tmp284 + 6},{
_tmp285,_tmp285 + 4},{_tmp286,_tmp286 + 5},{_tmp287,_tmp287 + 6},{_tmp288,_tmp288 + 
7},{_tmp289,_tmp289 + 7},{_tmp28A,_tmp28A + 9},{_tmp28B,_tmp28B + 6},{_tmp28C,
_tmp28C + 9},{_tmp28D,_tmp28D + 9},{_tmp28E,_tmp28E + 7},{_tmp28F,_tmp28F + 6},{
_tmp290,_tmp290 + 5},{_tmp291,_tmp291 + 8},{_tmp292,_tmp292 + 7},{_tmp293,_tmp293 + 
7},{_tmp294,_tmp294 + 9},{_tmp295,_tmp295 + 3},{_tmp296,_tmp296 + 5},{_tmp297,
_tmp297 + 7},{_tmp298,_tmp298 + 6},{_tmp299,_tmp299 + 3},{_tmp29A,_tmp29A + 4},{
_tmp29B,_tmp29B + 5},{_tmp29C,_tmp29C + 9},{_tmp29D,_tmp29D + 6},{_tmp29E,_tmp29E + 
7},{_tmp29F,_tmp29F + 5},{_tmp2A0,_tmp2A0 + 8},{_tmp2A1,_tmp2A1 + 4},{_tmp2A2,
_tmp2A2 + 6},{_tmp2A3,_tmp2A3 + 4},{_tmp2A4,_tmp2A4 + 6},{_tmp2A5,_tmp2A5 + 7},{
_tmp2A6,_tmp2A6 + 4},{_tmp2A7,_tmp2A7 + 9},{_tmp2A8,_tmp2A8 + 9},{_tmp2A9,_tmp2A9 + 
6},{_tmp2AA,_tmp2AA + 10},{_tmp2AB,_tmp2AB + 7},{_tmp2AC,_tmp2AC + 8},{_tmp2AD,
_tmp2AD + 7},{_tmp2AE,_tmp2AE + 8},{_tmp2AF,_tmp2AF + 7},{_tmp2B0,_tmp2B0 + 8},{
_tmp2B1,_tmp2B1 + 5},{_tmp2B2,_tmp2B2 + 9},{_tmp2B3,_tmp2B3 + 9},{_tmp2B4,_tmp2B4 + 
6},{_tmp2B5,_tmp2B5 + 7},{_tmp2B6,_tmp2B6 + 5},{_tmp2B7,_tmp2B7 + 8},{_tmp2B8,
_tmp2B8 + 13},{_tmp2B9,_tmp2B9 + 4},{_tmp2BA,_tmp2BA + 14},{_tmp2BB,_tmp2BB + 12},{
_tmp2BC,_tmp2BC + 7},{_tmp2BD,_tmp2BD + 8},{_tmp2BE,_tmp2BE + 8},{_tmp2BF,_tmp2BF + 
12},{_tmp2C0,_tmp2C0 + 7},{_tmp2C1,_tmp2C1 + 7},{_tmp2C2,_tmp2C2 + 7},{_tmp2C3,
_tmp2C3 + 8},{_tmp2C4,_tmp2C4 + 9},{_tmp2C5,_tmp2C5 + 6},{_tmp2C6,_tmp2C6 + 6},{
_tmp2C7,_tmp2C7 + 6},{_tmp2C8,_tmp2C8 + 6},{_tmp2C9,_tmp2C9 + 7},{_tmp2CA,_tmp2CA + 
6},{_tmp2CB,_tmp2CB + 11},{_tmp2CC,_tmp2CC + 11},{_tmp2CD,_tmp2CD + 11},{_tmp2CE,
_tmp2CE + 11},{_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0 + 12},{_tmp2D1,_tmp2D1 + 13},{
_tmp2D2,_tmp2D2 + 11},{_tmp2D3,_tmp2D3 + 11},{_tmp2D4,_tmp2D4 + 10},{_tmp2D5,
_tmp2D5 + 9},{_tmp2D6,_tmp2D6 + 11},{_tmp2D7,_tmp2D7 + 12},{_tmp2D8,_tmp2D8 + 11},{
_tmp2D9,_tmp2D9 + 17},{_tmp2DA,_tmp2DA + 7},{_tmp2DB,_tmp2DB + 19},{_tmp2DC,_tmp2DC
+ 18},{_tmp2DD,_tmp2DD + 9},{_tmp2DE,_tmp2DE + 13},{_tmp2DF,_tmp2DF + 16},{_tmp2E0,
_tmp2E0 + 18},{_tmp2E1,_tmp2E1 + 13},{_tmp2E2,_tmp2E2 + 10},{_tmp2E3,_tmp2E3 + 4},{
_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7 + 
4},{_tmp2E8,_tmp2E8 + 4},{_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA + 4},{_tmp2EB,
_tmp2EB + 4},{_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE + 4},{
_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0 + 4},{_tmp2F1,_tmp2F1 + 4},{_tmp2F2,_tmp2F2 + 
4},{_tmp2F3,_tmp2F3 + 4},{_tmp2F4,_tmp2F4 + 4},{_tmp2F5,_tmp2F5 + 4},{_tmp2F6,
_tmp2F6 + 4},{_tmp2F7,_tmp2F7 + 4},{_tmp2F8,_tmp2F8 + 4},{_tmp2F9,_tmp2F9 + 4},{
_tmp2FA,_tmp2FA + 4},{_tmp2FB,_tmp2FB + 4},{_tmp2FC,_tmp2FC + 4},{_tmp2FD,_tmp2FD + 
4},{_tmp2FE,_tmp2FE + 5},{_tmp2FF,_tmp2FF + 17},{_tmp300,_tmp300 + 12},{_tmp301,
_tmp301 + 19},{_tmp302,_tmp302 + 21},{_tmp303,_tmp303 + 20},{_tmp304,_tmp304 + 21},{
_tmp305,_tmp305 + 13},{_tmp306,_tmp306 + 15},{_tmp307,_tmp307 + 17},{_tmp308,
_tmp308 + 19},{_tmp309,_tmp309 + 12},{_tmp30A,_tmp30A + 17},{_tmp30B,_tmp30B + 23},{
_tmp30C,_tmp30C + 24},{_tmp30D,_tmp30D + 15},{_tmp30E,_tmp30E + 11},{_tmp30F,
_tmp30F + 15},{_tmp310,_tmp310 + 10},{_tmp311,_tmp311 + 15},{_tmp312,_tmp312 + 25},{
_tmp313,_tmp313 + 5},{_tmp314,_tmp314 + 15},{_tmp315,_tmp315 + 15},{_tmp316,_tmp316
+ 11},{_tmp317,_tmp317 + 22},{_tmp318,_tmp318 + 26},{_tmp319,_tmp319 + 16},{_tmp31A,
_tmp31A + 16},{_tmp31B,_tmp31B + 24},{_tmp31C,_tmp31C + 25},{_tmp31D,_tmp31D + 21},{
_tmp31E,_tmp31E + 22},{_tmp31F,_tmp31F + 16},{_tmp320,_tmp320 + 19},{_tmp321,
_tmp321 + 25},{_tmp322,_tmp322 + 35},{_tmp323,_tmp323 + 23},{_tmp324,_tmp324 + 24},{
_tmp325,_tmp325 + 18},{_tmp326,_tmp326 + 17},{_tmp327,_tmp327 + 9},{_tmp328,_tmp328
+ 18},{_tmp329,_tmp329 + 17},{_tmp32A,_tmp32A + 18},{_tmp32B,_tmp32B + 12},{_tmp32C,
_tmp32C + 11},{_tmp32D,_tmp32D + 23},{_tmp32E,_tmp32E + 18},{_tmp32F,_tmp32F + 30},{
_tmp330,_tmp330 + 8},{_tmp331,_tmp331 + 12},{_tmp332,_tmp332 + 23},{_tmp333,_tmp333
+ 14},{_tmp334,_tmp334 + 18},{_tmp335,_tmp335 + 12},{_tmp336,_tmp336 + 8},{_tmp337,
_tmp337 + 11},{_tmp338,_tmp338 + 20},{_tmp339,_tmp339 + 9},{_tmp33A,_tmp33A + 16},{
_tmp33B,_tmp33B + 19},{_tmp33C,_tmp33C + 10},{_tmp33D,_tmp33D + 16},{_tmp33E,
_tmp33E + 11},{_tmp33F,_tmp33F + 14},{_tmp340,_tmp340 + 11},{_tmp341,_tmp341 + 15},{
_tmp342,_tmp342 + 22},{_tmp343,_tmp343 + 16},{_tmp344,_tmp344 + 17},{_tmp345,
_tmp345 + 12},{_tmp346,_tmp346 + 18},{_tmp347,_tmp347 + 17},{_tmp348,_tmp348 + 12},{
_tmp349,_tmp349 + 16},{_tmp34A,_tmp34A + 11},{_tmp34B,_tmp34B + 10},{_tmp34C,
_tmp34C + 14},{_tmp34D,_tmp34D + 15},{_tmp34E,_tmp34E + 20},{_tmp34F,_tmp34F + 27},{
_tmp350,_tmp350 + 10},{_tmp351,_tmp351 + 13},{_tmp352,_tmp352 + 18},{_tmp353,
_tmp353 + 21},{_tmp354,_tmp354 + 19},{_tmp355,_tmp355 + 16},{_tmp356,_tmp356 + 20},{
_tmp357,_tmp357 + 15},{_tmp358,_tmp358 + 20},{_tmp359,_tmp359 + 15},{_tmp35A,
_tmp35A + 12},{_tmp35B,_tmp35B + 20},{_tmp35C,_tmp35C + 19},{_tmp35D,_tmp35D + 20},{
_tmp35E,_tmp35E + 21},{_tmp35F,_tmp35F + 21},{_tmp360,_tmp360 + 12},{_tmp361,
_tmp361 + 17},{_tmp362,_tmp362 + 19},{_tmp363,_tmp363 + 14},{_tmp364,_tmp364 + 17},{
_tmp365,_tmp365 + 23},{_tmp366,_tmp366 + 13},{_tmp367,_tmp367 + 14},{_tmp368,
_tmp368 + 16},{_tmp369,_tmp369 + 16},{_tmp36A,_tmp36A + 8},{_tmp36B,_tmp36B + 19},{
_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D + 14},{_tmp36E,_tmp36E + 19},{_tmp36F,
_tmp36F + 20},{_tmp370,_tmp370 + 11},{_tmp371,_tmp371 + 22},{_tmp372,_tmp372 + 20},{
_tmp373,_tmp373 + 23},{_tmp374,_tmp374 + 20},{_tmp375,_tmp375 + 22},{_tmp376,
_tmp376 + 23},{_tmp377,_tmp377 + 24},{_tmp378,_tmp378 + 24},{_tmp379,_tmp379 + 15},{
_tmp37A,_tmp37A + 20},{_tmp37B,_tmp37B + 22},{_tmp37C,_tmp37C + 17},{_tmp37D,
_tmp37D + 20},{_tmp37E,_tmp37E + 26},{_tmp37F,_tmp37F + 16},{_tmp380,_tmp380 + 17},{
_tmp381,_tmp381 + 15},{_tmp382,_tmp382 + 19},{_tmp383,_tmp383 + 19},{_tmp384,
_tmp384 + 25},{_tmp385,_tmp385 + 26},{_tmp386,_tmp386 + 9},{_tmp387,_tmp387 + 20},{
_tmp388,_tmp388 + 17},{_tmp389,_tmp389 + 18},{_tmp38A,_tmp38A + 11},{_tmp38B,
_tmp38B + 12}};static short Cyc_yyr1[500]={0,132,133,133,133,133,133,133,133,133,
133,133,134,134,135,135,136,136,136,137,137,137,137,138,138,139,140,141,142,143,
143,143,143,144,144,145,145,145,145,145,145,145,145,145,145,146,146,146,146,146,
146,146,147,147,148,149,149,150,150,150,150,150,150,151,151,152,152,152,152,152,
152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,153,154,
154,154,155,155,155,156,156,157,157,157,158,158,158,159,159,160,160,161,161,162,
162,163,164,164,165,165,166,167,167,167,167,167,167,168,168,168,168,168,168,169,
170,170,171,171,171,172,172,172,173,173,174,174,175,175,175,175,176,176,176,177,
177,178,178,179,179,180,180,180,180,180,180,180,180,180,180,181,181,181,181,181,
181,181,181,181,181,181,182,182,183,184,184,184,184,185,185,185,186,186,186,187,
187,187,188,188,188,189,189,190,190,190,190,191,191,192,192,193,193,194,194,195,
195,196,196,197,197,197,197,198,198,199,199,200,200,200,201,202,202,203,203,204,
204,204,204,205,205,205,205,206,207,207,208,208,209,209,210,210,210,210,210,211,
211,212,212,212,213,213,213,213,213,213,213,213,213,213,213,214,214,214,214,214,
214,214,214,214,214,214,215,215,216,217,217,218,218,219,219,219,219,219,219,220,
220,220,220,220,220,221,221,221,221,221,221,222,222,222,222,222,222,222,222,222,
222,222,222,222,222,223,223,223,223,223,223,223,223,224,225,225,226,226,227,227,
228,228,229,229,230,230,231,231,231,232,232,232,232,232,233,233,233,234,234,234,
235,235,235,235,236,236,237,237,237,237,237,237,237,238,239,240,240,240,240,240,
240,240,240,240,240,240,240,240,240,241,241,241,242,242,243,243,244,244,244,245,
245,246,246,247,247,248,248,248,248,248,248,248,248,248,248,248,249,249,249,249,
249,249,249,250,251,251,252,252,253,253,254,254,255,255,256,256,256,257,257,257,
257,257,258,258,258,259,259,259,260,260,260,260,261,261,262,262,262,262,262,262,
262,262,262,262,262,262,262,262,262,262,262,263,263,263,264,264,264,264,264,264,
264,264,264,264,265,265,265,265,265,265,265,265,265,266,267,267,268,268,268,268,
269,269,270,270,271,271,272,272,273,273};static short Cyc_yyr2[500]={0,1,2,3,5,3,5,
6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,
1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,4,1,
1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,
1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,
3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,
3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,
4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,4,9,6,5,0,5,3,1,2,2,3,1,2,1,2,
1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,
3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,
1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,
5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,
2,2,2,2,4,2,6,6,5,4,6,9,11,6,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,
1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[998]={0,18,45,46,47,48,50,65,
68,69,70,71,72,73,74,75,89,90,91,105,106,41,0,0,51,0,0,0,0,86,0,0,138,0,490,201,
492,491,493,87,0,0,66,0,183,183,181,1,0,16,0,0,17,0,35,43,37,63,39,76,77,0,78,0,
139,151,0,176,186,79,155,103,49,48,42,0,93,489,0,490,486,487,488,0,365,0,0,0,0,
224,0,367,368,25,27,139,139,139,0,0,139,0,0,0,0,0,139,139,179,180,182,2,0,0,0,0,
29,0,111,112,114,36,44,38,40,139,494,495,103,140,141,189,139,33,139,0,19,139,139,
0,164,152,177,188,187,192,139,64,0,49,97,0,95,0,490,374,0,0,0,0,0,0,0,0,0,0,0,0,
103,0,0,476,139,0,0,463,0,0,461,462,0,391,393,406,414,416,418,420,422,424,427,
432,435,438,442,0,444,464,475,474,0,375,373,32,0,0,103,0,0,0,121,117,119,242,244,
0,0,0,9,10,0,496,497,202,88,0,0,156,67,222,0,219,0,0,0,3,0,5,0,30,0,0,139,20,0,
139,109,0,102,191,0,190,0,0,0,0,0,0,0,0,0,0,0,0,0,490,279,281,287,283,0,285,265,
266,267,0,268,269,270,0,34,21,114,209,225,0,0,205,203,0,249,0,186,0,194,52,193,0,
0,98,94,0,0,369,0,0,139,452,139,408,442,0,409,410,0,0,0,0,0,0,0,139,445,446,139,
0,0,0,448,449,447,0,366,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,396,397,398,399,
400,401,402,403,404,405,395,0,450,0,470,471,0,0,0,478,0,103,381,382,0,379,0,226,
0,0,0,139,245,217,0,122,127,123,125,118,120,139,0,251,243,252,499,498,0,81,84,85,
139,82,58,57,0,55,157,139,221,153,251,223,165,166,139,80,139,185,184,26,0,28,0,
113,115,228,227,22,100,110,0,0,0,129,130,132,0,103,103,0,0,0,0,0,0,139,0,316,317,
318,0,0,320,0,276,0,0,0,0,288,284,114,286,282,280,210,139,0,0,216,204,211,160,0,
0,139,205,162,139,163,158,186,195,53,104,0,99,413,96,92,377,378,0,0,0,229,0,0,
233,0,0,0,238,0,0,0,0,0,0,139,0,0,0,477,484,0,483,392,415,0,417,419,421,423,425,
426,430,431,428,429,433,434,437,436,439,440,441,394,469,466,0,468,0,139,0,370,0,
376,31,388,0,384,389,0,386,371,0,0,246,128,124,126,0,205,0,186,0,253,139,139,0,
264,248,0,0,0,0,121,0,103,139,139,0,175,154,220,4,6,0,133,116,0,0,205,0,136,0,0,
0,295,0,0,0,0,0,315,319,0,0,0,0,0,0,0,0,278,139,23,203,139,213,0,0,206,0,161,209,
197,250,159,195,178,0,7,0,451,0,0,241,0,230,0,234,237,239,456,0,0,0,0,0,0,0,0,0,
443,480,0,0,467,465,0,0,380,383,385,372,0,247,218,260,0,254,255,186,0,205,0,186,
0,83,0,0,0,54,56,0,167,0,205,0,0,186,0,131,134,139,103,146,0,0,0,0,0,0,0,0,0,0,0,
0,0,295,321,0,0,272,0,0,0,24,205,0,214,212,0,205,139,196,0,8,0,0,0,240,231,235,0,
0,0,0,0,411,412,455,482,0,485,407,479,481,0,387,390,259,257,262,0,263,256,186,0,
61,59,60,168,171,0,173,174,169,186,0,137,148,147,0,0,142,289,295,0,0,0,139,0,0,
323,324,326,328,330,332,334,336,339,344,347,350,354,356,363,364,0,139,292,301,0,
0,0,0,0,0,0,0,0,0,322,0,0,0,0,275,199,215,207,198,203,13,0,0,454,453,0,236,457,
139,0,460,472,0,261,258,0,172,170,101,135,149,146,146,0,0,295,139,360,139,0,357,
139,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,358,296,0,303,0,0,0,311,0,0,0,0,0,0,
294,0,271,274,0,0,205,12,14,0,0,0,0,473,0,139,145,144,290,291,0,0,0,0,295,297,
327,0,329,331,333,335,337,338,342,343,340,341,345,346,349,348,351,352,353,0,302,
304,305,0,313,312,0,307,0,0,0,277,0,208,200,15,0,0,139,0,0,293,359,0,355,298,0,
139,306,314,308,309,0,0,232,458,0,62,150,0,0,325,295,299,310,273,0,362,361,300,
459,0,0,0};static short Cyc_yydefgoto[142]={995,47,638,833,48,49,268,50,424,51,426,
52,134,53,54,486,210,408,409,211,57,223,212,59,150,151,60,147,61,244,245,117,118,
119,246,213,391,436,437,438,62,63,131,780,781,782,64,439,65,413,66,67,68,108,145,
250,295,635,566,69,567,478,627,470,474,475,386,288,231,88,89,502,429,503,504,505,
506,214,290,291,568,397,271,615,272,273,274,275,276,708,277,278,790,791,792,793,
794,795,796,797,798,799,800,801,802,803,804,805,375,376,377,556,557,558,279,181,
363,182,492,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,519,520,
198,199,71,834,224,401};static short Cyc_yypact[998]={2630,- -32768,- -32768,- -32768,
- -32768,- 53,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3191,213,1083,- -32768,161,
- 26,- 31,- 9,68,93,98,- -32768,162,- -32768,217,- -32768,- -32768,- -32768,- -32768,248,
525,275,263,279,279,274,- -32768,2508,- -32768,152,220,- -32768,879,3191,3191,3191,
- -32768,3191,- -32768,- -32768,781,- -32768,520,3260,224,153,711,519,- -32768,-
-32768,290,303,318,- -32768,161,316,- -32768,2854,203,- -32768,- -32768,- -32768,5479,
- -32768,332,353,2854,348,359,371,- -32768,288,- -32768,- -32768,5983,5983,5983,2508,
2508,5983,192,377,634,393,192,6240,2006,- -32768,- -32768,- -32768,- -32768,2508,
2751,2508,2751,- -32768,403,428,- -32768,3121,- -32768,- -32768,- -32768,- -32768,6104,
- -32768,- -32768,290,- -32768,- -32768,27,1624,- -32768,3260,879,- -32768,3324,5983,
1160,- -32768,224,- -32768,- -32768,- -32768,59,5983,- -32768,2751,- -32768,444,449,
452,161,- 42,- -32768,2854,89,5697,455,5479,5533,464,479,487,491,496,502,290,5751,
5751,- -32768,2130,507,5805,- -32768,5805,5805,- -32768,- -32768,204,- -32768,- -32768,
- 47,553,516,521,512,621,120,631,44,401,- -32768,708,5805,261,11,- -32768,551,966,
559,- -32768,- -32768,565,5479,290,966,555,131,5998,6346,5998,458,- -32768,37,37,37,
- -32768,- -32768,99,- -32768,- -32768,- -32768,- -32768,43,558,- -32768,- -32768,560,
237,- -32768,561,- 27,586,- -32768,613,- -32768,623,- -32768,634,3959,3260,- -32768,
681,6119,- -32768,784,612,- -32768,161,- -32768,632,322,648,3479,653,705,702,738,
4054,3479,92,60,748,756,- -32768,- -32768,1495,1495,879,1495,- -32768,- -32768,-
-32768,767,- -32768,- -32768,- -32768,254,- -32768,- -32768,768,795,- -32768,77,780,
783,323,790,771,- 4,519,794,- -32768,817,- -32768,- 4,799,161,- -32768,5479,815,-
-32768,801,803,2130,- -32768,6240,- -32768,- -32768,3579,- -32768,845,5479,5479,5479,
5479,5479,5479,818,2130,- -32768,- -32768,1753,816,325,5479,- -32768,- -32768,-
-32768,5479,- -32768,5805,5479,5805,5805,5805,5805,5805,5805,5805,5805,5805,5805,
5805,5805,5805,5805,5805,5805,5805,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,5479,- -32768,192,- -32768,- -32768,
4149,192,5479,- -32768,826,290,- -32768,- -32768,819,829,2854,- -32768,383,838,820,
5983,- -32768,833,835,- -32768,6346,6346,6346,- -32768,- -32768,2888,4244,243,-
-32768,386,- -32768,- -32768,77,- -32768,- -32768,- -32768,5983,- -32768,- -32768,834,
832,837,- -32768,2819,- -32768,482,376,- -32768,- -32768,- -32768,6240,- -32768,2254,-
-32768,- -32768,- -32768,2508,- -32768,2508,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,735,5479,841,839,- -32768,844,573,290,399,5479,5479,842,847,5479,
926,1882,855,- -32768,- -32768,- -32768,389,919,- -32768,4339,853,864,872,5479,3479,
- -32768,- -32768,3121,- -32768,- -32768,- -32768,- -32768,6240,857,230,- -32768,-
-32768,846,- -32768,77,861,6225,783,- -32768,5983,- -32768,- -32768,519,823,- -32768,
- -32768,2386,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,863,860,881,- -32768,
192,5479,- -32768,604,3959,103,- -32768,398,870,874,875,878,497,6240,876,898,5615,
- -32768,- -32768,877,899,- -32768,553,276,516,521,512,621,120,120,631,631,631,631,
44,44,401,401,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,900,- -32768,62,
5983,3864,- -32768,3469,- -32768,- -32768,- -32768,2854,- -32768,- -32768,905,906,-
-32768,154,914,- -32768,- -32768,- -32768,- -32768,907,783,908,519,902,386,3388,5983,
4434,- -32768,- -32768,37,757,910,43,3002,911,290,3324,5983,4529,- -32768,482,-
-32768,- -32768,- -32768,915,- -32768,- -32768,784,5479,783,161,918,505,513,5479,726,
514,916,4624,4719,529,- -32768,- -32768,921,923,920,936,3479,925,928,532,- -32768,
3260,- -32768,932,5983,- -32768,937,77,- -32768,927,- -32768,145,- -32768,771,- -32768,
823,- -32768,933,- -32768,2508,943,312,929,- -32768,934,- -32768,3674,- -32768,-
-32768,- -32768,- -32768,5479,1026,5479,5479,5533,944,943,945,3864,- -32768,- -32768,
5479,5479,- -32768,- -32768,- 4,630,- -32768,- -32768,- -32768,- -32768,4044,- -32768,-
-32768,- -32768,947,- -32768,- -32768,519,948,783,- 4,519,950,- -32768,540,951,954,-
-32768,- -32768,955,- -32768,958,783,959,- 4,519,953,- -32768,- -32768,6104,290,807,
3479,968,964,1015,962,974,3479,5479,4814,547,4909,577,5004,726,- -32768,977,976,-
-32768,995,73,985,- -32768,783,179,- -32768,- -32768,992,783,6240,- -32768,595,-
-32768,981,986,5479,- -32768,- -32768,- -32768,3959,576,988,993,580,- -32768,845,-
-32768,- -32768,741,- -32768,- -32768,- -32768,- -32768,3864,- -32768,- -32768,- -32768,
- -32768,- -32768,989,- -32768,- -32768,519,1001,- -32768,- -32768,- -32768,- -32768,-
-32768,991,- -32768,- -32768,- -32768,519,996,- -32768,- -32768,- -32768,997,161,611,
1077,726,1000,5859,998,2254,5805,1002,- -32768,- 40,- -32768,1028,990,743,802,156,
788,410,418,- -32768,- -32768,- -32768,- -32768,1039,5805,1753,- -32768,- -32768,592,
3479,618,5099,3479,635,5194,5289,642,1012,- -32768,5479,3479,3479,1021,- -32768,-
-32768,- -32768,1018,- -32768,932,- -32768,1022,651,- -32768,- -32768,331,- -32768,-
-32768,6240,1105,- -32768,- -32768,3769,- -32768,- -32768,1024,- -32768,- -32768,-
-32768,- -32768,1023,40,807,3479,1031,726,2130,- -32768,6240,1029,- -32768,1353,
5805,5479,5805,5805,5805,5805,5805,5805,5805,5805,5805,5805,5805,5805,5805,5805,
5805,5805,5805,5479,- -32768,- -32768,1030,- -32768,3479,3479,649,- -32768,3479,3479,
656,3479,693,5384,- -32768,713,- -32768,- -32768,1020,77,783,- -32768,- -32768,280,
5479,1033,1037,- -32768,1046,6240,- -32768,- -32768,- -32768,- -32768,1041,1042,1044,
5805,726,- -32768,553,336,516,521,521,621,120,120,631,631,631,631,44,44,401,401,-
-32768,- -32768,- -32768,349,- -32768,- -32768,- -32768,3479,- -32768,- -32768,3479,-
-32768,3479,3479,715,- -32768,1056,- -32768,- -32768,- -32768,789,1045,6240,1047,717,
- -32768,943,374,- -32768,- -32768,5479,1353,- -32768,- -32768,- -32768,- -32768,3479,
3479,- -32768,- -32768,1049,- -32768,- -32768,1050,1053,- -32768,726,- -32768,- -32768,
- -32768,1054,- -32768,- -32768,- -32768,- -32768,1155,1157,- -32768};static short Cyc_yypgoto[
142]={- -32768,- 23,- -32768,259,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 63,- 108,52,- -32768,- -32768,0,587,- -32768,101,- 144,1063,30,- -32768,-
-32768,- 109,- -32768,- 106,- -32768,469,- -32768,- -32768,- -32768,930,931,238,477,-
-32768,- -32768,578,- -32768,- -32768,- -32768,57,- -32768,- -32768,82,- 152,1106,- 379,
170,- -32768,- -32768,1126,138,- -32768,- -32768,541,- 100,- 116,- 99,134,286,549,556,-
368,- 409,- 104,- 352,- 129,- -32768,- 202,- 143,- 525,- 362,- -32768,685,- 55,129,- 110,-
134,- 274,299,- -32768,- -32768,- -32768,- 38,- 265,- -32768,- 682,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,186,994,- -32768,526,- -32768,-
-32768,266,679,- -32768,- 155,- 348,- 130,- 326,- 324,- 319,862,- 314,- 330,- 150,- 332,-
320,111,433,499,- -32768,- -32768,- 321,- -32768,140,252,- 19,1142,- 349,- 167};static
short Cyc_yytable[6460]={55,133,230,463,464,309,466,522,289,528,529,524,242,534,
535,251,542,525,312,554,545,55,248,666,527,111,136,536,537,296,58,313,576,333,
820,588,297,286,287,428,864,103,127,72,302,778,779,544,55,402,403,58,156,405,55,
55,55,133,55,515,406,588,320,625,55,140,589,389,94,269,398,280,157,74,334,218,
219,412,58,396,421,865,243,95,58,58,58,593,58,235,236,237,238,385,58,415,281,400,
55,55,381,56,856,371,482,96,121,122,123,628,124,398,55,55,55,55,135,325,622,399,
55,571,56,104,483,298,35,589,58,58,488,372,55,750,55,120,613,407,471,249,571,140,
58,58,58,58,491,- 143,55,56,58,642,673,643,458,56,56,56,35,56,399,400,58,91,58,56,
347,824,348,473,331,294,135,398,459,918,35,460,58,280,664,923,97,472,270,226,135,
825,304,480,490,530,531,532,533,341,342,456,98,56,56,305,457,99,431,269,269,404,
269,90,417,388,647,56,56,56,56,282,91,500,501,56,399,400,215,216,217,91,417,220,
35,872,873,56,343,56,233,142,384,469,969,390,55,344,389,389,389,34,699,56,285,
496,70,497,37,34,- 203,112,113,582,- 203,269,37,41,155,672,514,548,55,55,874,55,58,
202,76,92,100,93,400,875,491,705,582,741,473,328,221,329,330,987,828,736,222,70,
135,487,91,156,58,58,70,58,646,400,993,70,364,34,- 225,554,101,- 225,331,126,37,
332,230,157,70,75,270,270,282,270,55,114,115,152,40,35,92,365,366,367,137,597,
599,624,138,92,91,139,303,56,229,418,491,91,419,180,70,70,465,393,58,70,385,620,
102,468,394,473,331,247,70,70,70,70,105,56,56,368,56,106,34,229,270,369,370,831,
36,37,38,395,331,107,607,70,388,388,388,662,583,741,206,417,110,575,207,414,70,
590,133,591,146,152,221,735,92,148,684,581,222,587,149,417,34,440,390,390,390,
417,153,37,55,56,55,621,285,690,484,479,444,331,592,445,517,665,326,331,690,491,
200,740,331,385,908,201,387,55,392,970,92,203,58,694,58,331,655,92,538,539,540,
681,971,55,637,204,221,983,34,380,679,680,222,695,36,37,38,58,205,416,741,247,
692,693,411,225,552,55,40,331,70,394,610,58,572,331,753,416,573,135,442,574,227,
446,331,752,239,649,473,746,560,146,762,598,634,135,91,58,349,91,70,725,747,56,
454,56,773,350,351,730,878,577,879,628,240,880,924,838,930,931,926,885,936,937,
881,882,927,928,56,152,299,740,448,929,300,280,938,939,455,301,583,395,551,308,
56,555,393,129,130,575,326,417,314,394,417,44,45,46,507,414,724,143,144,512,40,
326,587,315,56,307,584,310,310,777,585,316,922,586,523,317,322,323,440,331,318,
310,654,310,310,632,319,331,631,734,703,327,984,34,55,331,331,633,704,709,37,659,
310,40,92,229,335,92,715,41,546,331,55,338,331,336,740,723,44,45,46,337,765,229,
58,766,814,378,34,331,229,373,229,379,36,37,38,416,634,383,416,34,58,410,411,420,
135,36,37,38,70,394,70,44,45,46,817,434,331,331,416,839,331,34,91,842,416,422,91,
36,37,38,339,340,331,675,832,886,345,346,986,677,229,600,601,644,645,604,127,608,
853,229,441,854,423,56,932,933,934,935,331,618,34,888,425,700,904,861,310,37,668,
754,755,56,669,70,443,331,41,269,892,897,706,707,331,440,726,44,45,46,906,331,
447,907,947,619,440,449,331,440,310,950,310,310,310,310,310,310,310,310,310,310,
310,310,310,310,310,310,310,909,234,473,432,352,353,354,355,356,357,358,359,360,
361,450,269,331,92,919,952,920,92,451,55,964,229,91,778,779,761,985,759,362,293,
387,763,331,229,331,955,418,976,771,982,34,44,45,46,416,774,36,37,38,58,40,16,17,
18,452,416,434,91,416,843,844,701,685,686,687,44,45,46,555,461,827,270,876,877,
55,830,563,564,565,310,868,869,462,712,714,467,34,77,241,34,870,871,36,37,38,36,
37,38,125,127,469,70,476,806,58,434,400,978,331,477,435,862,481,846,44,45,46,489,
980,269,56,914,915,485,849,720,270,742,78,884,745,430,40,493,92,494,333,495,516,
513,553,549,559,154,80,310,81,82,550,247,37,547,561,562,579,578,580,594,595,83,
310,84,85,602,603,500,501,605,86,92,596,609,611,614,616,56,87,617,623,640,229,
626,55,629,34,639,641,811,813,650,816,37,819,651,652,116,126,653,656,660,41,430,
940,941,942,508,509,510,511,44,45,46,58,783,837,77,657,518,661,810,34,521,663,
670,35,671,36,37,38,674,676,678,688,691,270,702,434,710,716,717,310,719,718,968,
852,44,45,46,285,958,733,786,787,541,737,727,78,722,518,721,658,729,743,738,77,
180,748,749,374,758,760,154,80,767,81,82,768,769,37,764,770,772,775,56,570,784,
785,83,808,84,85,890,809,821,894,896,86,822,310,899,823,826,829,835,87,78,310,
847,836,840,518,845,841,848,850,851,855,857,866,860,154,80,887,81,82,891,867,37,
863,883,898,902,77,900,901,326,788,903,84,85,905,910,925,912,913,86,944,518,175,
917,789,921,963,87,956,961,178,179,962,965,943,977,229,967,966,916,996,979,997,
981,126,990,991,78,954,992,994,959,689,228,776,427,109,141,698,960,732,433,79,80,
731,81,82,728,430,37,158,159,945,946,957,648,948,949,83,951,84,85,757,77,526,160,
382,86,128,161,807,0,0,0,0,87,162,163,164,165,166,0,0,0,859,167,0,310,168,0,0,0,
430,0,0,0,169,170,0,0,0,0,0,0,0,310,0,0,0,0,0,972,0,0,973,0,974,975,683,34,80,
171,81,82,0,0,37,0,0,0,697,0,0,0,0,172,0,0,173,0,988,989,0,292,174,0,518,175,0,
176,0,0,177,0,0,178,179,0,0,0,0,0,310,0,310,310,310,310,310,310,310,310,310,310,
310,310,310,310,310,310,310,0,0,0,0,0,0,0,0,430,0,0,0,0,0,0,744,0,0,0,0,0,430,0,
0,751,0,0,0,0,0,0,0,0,0,0,0,0,0,310,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,706,707,21,158,159,252,0,253,254,255,256,257,258,259,260,22,77,23,160,
261,0,0,161,24,262,0,0,0,310,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,
0,430,32,33,0,169,170,0,0,0,0,0,0,0,0,430,0,0,0,0,0,0,0,0,0,0,0,0,265,80,171,81,
82,35,36,37,38,39,40,266,132,- 295,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,
0,177,0,0,178,179,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,158,159,252,430,253,254,255,256,257,258,259,260,22,77,23,
160,261,0,0,161,24,262,0,0,- 139,- 139,162,163,164,165,166,27,28,29,263,167,0,264,
168,0,0,0,0,32,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,265,80,171,
81,82,35,36,37,38,39,40,266,132,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,
0,177,0,0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,
159,252,0,253,254,255,256,257,258,259,260,22,77,23,160,261,0,0,161,24,262,0,0,0,
0,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,33,0,169,170,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,265,80,171,81,82,35,36,37,38,39,40,266,132,267,
0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,253,254,255,256,257,258,
259,260,22,77,23,160,261,0,0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,263,
167,0,264,168,0,0,0,0,32,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
265,80,171,81,82,35,36,37,38,39,40,266,132,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,
175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,158,159,0,0,0,0,0,0,0,0,0,0,22,77,23,160,0,0,0,161,24,0,0,0,0,0,162,163,
164,165,166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,606,0,0,0,0,172,0,42,232,0,
0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,
163,164,165,166,27,28,29,0,167,208,0,168,0,0,0,0,32,33,0,169,170,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,209,0,0,0,172,0,42,
232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,324,0,0,0,172,0,
42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,
0,162,163,164,165,166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,169,170,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,0,0,0,0,172,0,
42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
636,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,
41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,
0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,-
11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,
0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,
31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,
39,40,0,0,0,0,0,41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,-
139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,
42,43,0,0,0,0,0,44,45,46,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,77,0,0,0,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,283,0,0,34,0,0,0,0,35,36,37,38,39,40,285,22,0,
0,0,411,- 203,42,43,78,- 203,0,394,0,44,45,46,0,0,0,27,28,29,0,154,80,0,81,82,0,0,
37,32,33,0,0,0,0,0,0,83,0,84,85,0,0,0,0,0,86,0,0,0,0,0,283,0,87,0,0,0,0,0,35,36,
0,38,39,40,285,0,0,0,0,393,- 203,42,43,0,- 203,0,394,0,44,45,46,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 139,
- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,434,0,42,43,0,0,0,0,0,44,
45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,
0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,
32,33,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,35,
36,0,38,39,40,0,132,22,0,241,0,0,42,43,0,24,0,0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,
0,0,0,0,0,0,0,0,32,33,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,35,36,0,38,39,40,0,22,0,23,0,0,0,42,43,24,0,0,0,0,0,0,0,0,0,0,27,28,
29,0,0,0,0,0,0,0,0,0,32,33,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,
0,0,0,0,0,0,0,0,35,36,22,38,39,40,0,132,0,0,0,0,0,42,43,0,0,0,0,0,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,33,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,283,0,0,
284,0,0,0,0,35,36,22,38,39,40,285,0,0,0,0,0,- 203,42,43,0,- 203,0,0,0,0,27,28,29,0,
0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,0,0,0,0,0,0,
0,35,36,0,38,39,40,285,0,0,0,0,0,- 203,42,43,0,- 203,158,159,252,77,253,254,255,
256,257,258,259,260,0,77,0,160,261,0,0,161,0,262,0,0,0,0,162,163,164,165,166,0,0,
0,263,167,0,264,168,0,0,0,0,0,78,0,169,170,0,0,0,0,0,0,0,0,667,0,0,154,80,0,81,
82,0,0,37,0,0,265,80,171,81,82,0,83,37,84,85,0,266,132,0,0,86,172,0,0,173,0,0,0,
87,0,174,0,0,175,0,176,158,159,177,0,0,178,179,498,0,0,0,0,0,77,0,160,0,0,0,161,
0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,311,499,0,0,
172,0,0,173,0,0,500,501,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,
0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,311,
739,0,0,172,0,0,173,0,0,500,501,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,
0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,
0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,
0,311,911,0,0,172,0,0,173,0,0,500,501,0,174,158,159,175,0,176,0,0,177,0,0,178,
179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,
0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,
37,0,0,0,0,311,0,0,0,172,0,0,173,0,0,500,501,0,174,158,159,175,0,176,0,0,177,0,0,
178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,
0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,
0,0,37,0,0,0,0,311,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,77,176,0,0,177,0,
0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,
168,0,0,0,0,0,78,0,169,170,0,0,0,0,0,0,0,0,756,0,0,154,80,0,81,82,0,0,37,0,0,34,
80,171,81,82,0,83,37,84,85,0,453,500,501,0,86,172,0,0,173,0,0,0,87,0,174,158,159,
175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,
166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,543,0,173,0,0,0,0,0,174,158,
159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,
165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,569,174,
158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,
164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,612,0,173,0,0,0,0,0,
174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,
163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,
682,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,
0,696,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,711,0,0,0,0,172,0,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,713,0,0,0,0,172,0,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,812,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,815,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,818,0,0,0,0,172,0,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,889,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,893,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,895,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,953,0,173,0,0,
0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,158,
159,0,0,0,0,0,0,0,0,0,0,0,77,0,34,80,171,81,82,0,0,37,0,0,0,162,163,164,165,166,
172,0,0,173,0,0,0,168,0,174,0,0,175,0,176,169,170,177,0,0,178,179,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,311,158,159,0,172,0,0,173,0,0,0,
0,0,174,77,0,175,0,176,0,0,177,0,0,178,179,0,162,163,164,165,166,0,0,0,0,0,0,0,
168,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,
82,0,0,37,0,0,0,0,658,158,159,0,172,0,0,173,0,0,0,0,0,174,77,0,175,0,176,0,0,177,
0,0,178,179,0,162,163,164,165,166,0,0,0,0,0,0,0,168,0,0,0,0,0,0,0,169,170,0,0,0,
0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,34,80,171,81,82,0,0,37,0,0,0,162,163,
164,165,166,306,0,0,173,0,0,0,168,0,174,0,0,175,0,176,169,170,177,0,0,178,179,0,
158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,34,80,171,81,82,0,0,37,0,0,0,162,163,164,165,
166,321,0,0,173,0,0,0,168,0,174,0,0,175,0,176,169,170,177,0,0,178,179,0,158,159,
0,0,0,0,0,0,0,0,0,0,0,77,0,34,80,171,81,82,0,0,37,0,0,0,162,163,164,165,166,172,
0,0,173,0,0,0,168,0,174,0,0,175,0,176,169,170,177,0,0,178,179,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,858,0,0,173,0,0,0,0,0,174,0,
0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,27,28,
29,0,0,208,0,0,- 139,- 139,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,35,36,0,38,39,40,0,209,0,0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,
0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,27,28,29,0,0,0,0,0,0,0,0,
0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,0,
38,39,40,0,0,- 107,0,0,0,0,42,43,35,36,0,38,39,40,0,0,- 108,0,0,0,0,42,43,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,
0,0,0,0,0,0,32,33,0,0,0,0,0,0,630,0,0,0,0,0,0,0,35,36,0,38,39,40,0,0,0,0,0,0,0,
42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 139,- 139,0,0,0,0,0,27,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,35,0,0,0,39,40,0,0,0,0,0,0,0,42,43};static short Cyc_yycheck[6460]={0,64,
106,268,269,160,271,333,137,339,340,335,120,345,346,131,365,336,161,381,369,21,
128,548,338,48,64,347,348,145,0,161,400,80,716,414,146,137,137,241,80,41,61,96,
153,5,6,368,48,216,217,21,94,220,54,55,56,120,58,324,17,440,168,472,64,65,418,
211,94,132,74,134,114,21,121,98,99,229,48,213,107,121,120,114,54,55,56,435,58,
112,113,114,115,209,64,229,134,124,98,99,206,0,784,92,108,114,54,55,56,477,58,74,
112,113,114,115,64,172,470,123,120,395,21,41,291,148,99,479,98,99,297,120,132,
658,134,53,457,94,61,112,414,141,112,113,114,115,301,107,148,48,120,500,561,501,
94,54,55,56,99,58,123,124,132,23,134,64,122,94,124,285,108,112,120,74,114,857,99,
117,148,242,118,863,114,106,132,103,134,114,99,288,299,341,342,343,344,75,76,105,
105,98,99,112,110,105,242,268,269,108,271,23,229,211,109,112,113,114,115,135,78,
116,117,120,123,124,95,96,97,87,247,100,99,75,76,132,114,134,107,67,107,94,922,
211,242,123,388,389,390,94,596,148,105,306,0,308,101,94,111,105,106,411,115,324,
101,110,78,111,321,373,268,269,114,271,242,87,22,23,114,25,124,123,435,602,434,
645,400,174,94,176,177,971,111,640,100,41,242,295,156,94,268,269,48,271,504,124,
986,53,195,94,105,671,93,108,108,61,101,111,420,114,66,106,268,269,240,271,324,
105,106,75,104,99,78,70,71,72,110,441,442,107,114,87,200,117,156,242,106,108,501,
207,111,83,98,99,270,110,324,103,472,465,110,105,117,477,108,125,112,113,114,115,
93,268,269,110,271,110,94,137,324,116,117,731,100,101,102,213,108,106,449,135,
388,389,390,115,411,755,106,414,122,397,110,229,148,424,465,426,114,153,94,95,
156,106,577,411,100,413,96,434,94,247,388,389,390,440,106,101,424,324,426,465,
105,581,292,108,110,108,434,113,111,547,172,108,592,596,110,645,108,561,115,94,
210,449,212,115,200,105,424,584,426,108,513,207,349,350,351,573,115,465,489,108,
94,95,94,205,572,572,100,585,100,101,102,449,109,229,844,245,584,584,110,110,105,
489,104,108,240,117,105,465,110,108,665,247,114,449,250,117,111,253,108,662,105,
111,626,654,383,114,681,116,486,465,378,489,119,381,270,622,654,424,260,426,695,
128,129,630,122,404,124,903,108,119,864,741,870,871,866,808,876,877,128,129,867,
868,449,299,108,755,255,869,107,620,878,879,261,109,581,393,378,110,465,381,110,
49,50,571,306,592,110,117,595,119,120,121,314,411,620,64,65,319,104,321,588,110,
489,158,110,160,161,701,114,110,863,117,334,110,169,170,434,108,110,174,111,176,
177,482,110,108,480,638,111,110,967,94,620,108,108,485,111,111,101,516,195,104,
378,393,79,381,105,110,370,108,638,127,108,125,844,111,119,120,121,126,108,411,
620,111,105,94,94,108,418,106,420,94,100,101,102,411,634,110,414,94,638,111,110,
110,620,100,101,102,424,117,426,119,120,121,105,110,108,108,434,111,108,94,550,
111,440,107,554,100,101,102,77,78,108,567,107,111,73,74,971,569,470,443,444,107,
108,447,733,449,105,479,106,108,107,620,872,873,874,875,108,461,94,111,107,597,
831,788,301,101,550,107,108,638,554,489,110,108,110,808,111,105,22,23,108,581,
623,119,120,121,105,108,110,108,111,462,592,110,108,595,333,111,335,336,337,338,
339,340,341,342,343,344,345,346,347,348,349,350,351,840,107,903,107,81,82,83,84,
85,86,87,88,89,90,94,863,108,550,858,111,860,554,105,808,913,572,671,5,6,680,970,
678,109,139,581,682,108,584,108,111,108,111,693,111,94,119,120,121,581,696,100,
101,102,808,104,17,18,19,105,592,110,706,595,107,108,598,94,95,96,119,120,121,
671,110,725,808,73,74,863,730,388,389,390,435,126,127,115,606,607,107,94,38,109,
94,77,78,100,101,102,100,101,102,106,907,94,638,111,706,863,110,124,107,108,115,
115,789,111,764,119,120,121,107,962,971,808,853,854,118,775,615,863,650,79,807,
653,241,104,107,671,123,80,123,111,110,91,111,111,94,95,501,97,98,108,700,101,
114,108,107,111,110,108,105,108,110,516,112,113,110,106,116,117,30,119,706,115,
105,42,109,99,863,127,94,110,108,731,124,971,111,94,111,94,710,711,108,713,101,
715,108,108,105,733,108,111,111,110,311,880,881,882,315,316,317,318,119,120,121,
971,703,737,38,107,327,108,709,94,331,111,107,99,108,100,101,102,111,111,118,111,
111,971,106,110,110,106,105,596,94,111,921,781,119,120,121,105,904,106,25,26,363,
114,107,79,118,368,123,106,123,25,118,38,788,111,111,91,111,111,94,95,111,97,98,
111,111,101,118,111,111,118,971,394,106,111,110,115,112,113,814,107,105,817,818,
119,110,654,822,94,105,99,111,127,79,662,95,111,110,420,111,108,111,107,107,28,
106,79,110,94,95,812,97,98,815,125,101,115,79,107,99,38,823,824,858,110,108,112,
113,107,25,865,108,110,119,105,457,122,107,124,111,95,127,123,111,130,131,110,
107,883,94,913,108,111,855,0,111,0,111,907,111,111,79,897,111,111,907,580,105,
700,240,45,66,595,908,634,245,94,95,630,97,98,626,504,101,25,26,888,889,903,505,
892,893,110,895,112,113,671,38,337,40,207,119,61,44,706,- 1,- 1,- 1,- 1,127,51,52,53,
54,55,- 1,- 1,- 1,786,60,- 1,789,63,- 1,- 1,- 1,548,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
807,- 1,- 1,- 1,- 1,- 1,947,- 1,- 1,950,- 1,952,953,574,94,95,96,97,98,- 1,- 1,101,- 1,- 1,-
1,586,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,976,977,- 1,118,119,- 1,602,122,- 1,124,- 1,- 1,
127,- 1,- 1,130,131,- 1,- 1,- 1,- 1,- 1,864,- 1,866,867,868,869,870,871,872,873,874,875,
876,877,878,879,880,881,882,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,645,- 1,- 1,- 1,- 1,- 1,- 1,652,- 1,
- 1,- 1,- 1,- 1,658,- 1,- 1,661,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,921,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,970,51,52,53,54,55,56,57,58,59,60,-
1,62,63,- 1,- 1,- 1,741,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,755,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,99,100,101,102,103,104,105,106,107,- 1,- 1,110,
- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,24,25,26,27,844,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,-
1,49,50,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,99,100,
101,102,103,104,105,106,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,
124,- 1,- 1,127,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,
- 1,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,99,100,101,102,
103,104,105,106,107,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,
- 1,127,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,
52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,99,100,101,102,103,
104,105,106,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,
127,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,55,56,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,99,100,101,102,103,104,
105,- 1,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,127,- 1,
- 1,130,131,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,
- 1,60,61,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,99,100,101,102,103,104,- 1,106,- 1,- 1,- 1,
110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,94,95,96,97,98,99,100,101,102,103,104,- 1,106,- 1,- 1,- 1,110,- 1,112,113,- 1,
- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,
- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,
97,98,99,100,101,102,103,104,- 1,- 1,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,
- 1,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,
- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,
- 1,99,100,101,102,103,104,- 1,- 1,107,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,120,
121,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,
- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,99,100,101,102,103,104,- 1,- 1,
107,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,120,121,0,1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,94,- 1,- 1,- 1,- 1,99,100,101,102,103,104,- 1,- 1,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,
- 1,- 1,119,120,121,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,
- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,99,100,101,102,
103,104,37,- 1,107,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,120,121,- 1,- 1,56,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,38,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,91,- 1,- 1,94,- 1,- 1,- 1,- 1,99,100,101,102,103,104,105,37,- 1,- 1,- 1,
110,111,112,113,79,115,- 1,117,- 1,119,120,121,- 1,- 1,- 1,56,57,58,- 1,94,95,- 1,97,98,
- 1,- 1,101,68,69,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,
- 1,91,- 1,127,- 1,- 1,- 1,- 1,- 1,99,100,- 1,102,103,104,105,- 1,- 1,- 1,- 1,110,111,112,
113,- 1,115,- 1,117,- 1,119,120,121,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,
- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,99,100,101,102,
103,104,- 1,- 1,- 1,- 1,- 1,110,- 1,112,113,- 1,- 1,- 1,- 1,- 1,119,120,121,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,- 1,- 1,- 1,- 1,99,100,- 1,102,103,104,- 1,106,37,- 1,109,- 1,- 1,112,113,- 1,45,- 1,
- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,99,100,- 1,
102,103,104,- 1,37,- 1,39,- 1,- 1,- 1,112,113,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,
58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,37,102,103,104,- 1,106,- 1,- 1,- 1,
- 1,- 1,112,113,- 1,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,94,- 1,- 1,- 1,- 1,
99,100,37,102,103,104,105,- 1,- 1,- 1,- 1,- 1,111,112,113,- 1,115,- 1,- 1,- 1,- 1,56,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,102,103,104,105,- 1,- 1,- 1,- 1,- 1,
111,112,113,- 1,115,25,26,27,38,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,-
1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,79,- 1,71,
72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,94,95,- 1,97,98,- 1,- 1,101,- 1,- 1,94,95,96,97,
98,- 1,110,101,112,113,- 1,105,106,- 1,- 1,119,110,- 1,- 1,113,- 1,- 1,- 1,127,- 1,119,- 1,
- 1,122,- 1,124,25,26,127,- 1,- 1,130,131,32,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,
- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,
- 1,101,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,- 1,- 1,113,- 1,- 1,116,117,- 1,119,25,26,122,- 1,
124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,106,
107,- 1,- 1,110,- 1,- 1,113,- 1,- 1,116,117,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,
130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,- 1,- 1,
113,- 1,- 1,116,117,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,
96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,116,117,- 1,119,
25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,-
1,- 1,- 1,106,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,38,124,- 1,- 1,127,
- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,
- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,79,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,94,95,
- 1,97,98,- 1,- 1,101,- 1,- 1,94,95,96,97,98,- 1,110,101,112,113,- 1,105,116,117,- 1,119,
110,- 1,- 1,113,- 1,- 1,- 1,127,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,113,- 1,- 1,- 1,- 1,-
1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,
- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,
101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,119,25,26,122,- 1,124,-
1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,110,111,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,
118,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,-
1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,
101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,119,25,26,122,- 1,124,-
1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,105,- 1,- 1,- 1,
- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,-
1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,
101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,-
1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,110,111,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,-
1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,
101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,-
1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,110,111,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,113,- 1,- 1,- 1,-
1,- 1,119,25,26,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,
101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,113,- 1,- 1,- 1,- 1,- 1,119,25,26,122,- 1,124,-
1,- 1,127,- 1,- 1,130,131,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,51,52,53,54,
55,110,- 1,- 1,113,- 1,- 1,- 1,63,- 1,119,- 1,- 1,122,- 1,124,71,72,127,- 1,- 1,130,131,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,-
1,106,25,26,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,119,38,- 1,122,- 1,124,- 1,- 1,127,- 1,- 1,
130,131,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,
101,- 1,- 1,- 1,- 1,106,25,26,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,119,38,- 1,122,- 1,124,-
1,- 1,127,- 1,- 1,130,131,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,94,95,
96,97,98,- 1,- 1,101,- 1,- 1,- 1,51,52,53,54,55,110,- 1,- 1,113,- 1,- 1,- 1,63,- 1,119,- 1,-
1,122,- 1,124,71,72,127,- 1,- 1,130,131,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
38,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,51,52,53,54,55,110,- 1,- 1,113,- 1,- 1,- 1,63,
- 1,119,- 1,- 1,122,- 1,124,71,72,127,- 1,- 1,130,131,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,38,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,51,52,53,54,55,110,- 1,- 1,113,- 1,
- 1,- 1,63,- 1,119,- 1,- 1,122,- 1,124,71,72,127,- 1,- 1,130,131,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,95,96,97,98,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,
- 1,113,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,127,- 1,- 1,130,131,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,56,57,58,- 1,- 1,61,- 1,- 1,49,50,- 1,
- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,99,100,- 1,102,103,104,- 1,106,- 1,- 1,- 1,- 1,- 1,112,113,99,100,- 1,102,
103,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,102,
103,104,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,99,100,- 1,102,103,104,- 1,- 1,107,- 1,- 1,- 1,-
1,112,113,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,56,57,58,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,
- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,102,103,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
112,113,99,100,- 1,102,103,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,99,- 1,- 1,- 1,103,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dynforward_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(
int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){
57});static int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple19{struct _dynforward_ptr f1;void*f2;};static char
_tmp42D[8]="stdcall";static char _tmp42E[6]="cdecl";static char _tmp42F[9]="fastcall";
static char _tmp430[9]="noreturn";static char _tmp431[6]="const";static char _tmp432[8]="aligned";
static char _tmp433[7]="packed";static char _tmp434[7]="shared";static char _tmp435[7]="unused";
static char _tmp436[5]="weak";static char _tmp437[10]="dllimport";static char _tmp438[
10]="dllexport";static char _tmp439[23]="no_instrument_function";static char _tmp43A[
12]="constructor";static char _tmp43B[11]="destructor";static char _tmp43C[22]="no_check_memory_usage";
static char _tmp43D[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp38D=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp38D;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8E3=10000;short*_tmp8E4=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8E3));{unsigned int
_tmp8E5=_tmp8E3;unsigned int i;for(i=0;i < _tmp8E5;i ++){_tmp8E4[i]=(short)0;}}
_tmp8E4;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8E0=10000;union Cyc_YYSTYPE_union*_tmp8E1=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8E0));{
unsigned int _tmp8E2=_tmp8E0;unsigned int i;for(i=0;i < _tmp8E2;i ++){_tmp8E1[i]=Cyc_yylval;}}
_tmp8E1;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8DD=10000;struct Cyc_Yyltype*_tmp8DE=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8DD));{
unsigned int _tmp8DF=_tmp8DD;unsigned int i;for(i=0;i < _tmp8DF;i ++){_tmp8DE[i]=Cyc_yynewloc();}}
_tmp8DE;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp38E="parser stack overflow";_tag_dynforward(
_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(998,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 359?(int)Cyc_yytranslate[_check_known_subscript_notnull(360,Cyc_yychar)]:
274;}yyn +=yychar1;if((yyn < 0  || yyn > 6459) || Cyc_yycheck[
_check_known_subscript_notnull(6460,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6460,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 997){int
_tmp38F=0;_npop_handler(0);return _tmp38F;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)yyerrstatus --;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(998,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(500,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E9: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp390;_push_handler(& _tmp390);{int _tmp392=0;if(setjmp(_tmp390.handler))_tmp392=
1;if(!_tmp392){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp391=(void*)_exn_thrown;void*
_tmp394=_tmp391;_LL1EC: if(*((void**)_tmp394)!= Cyc_Core_Failure)goto _LL1EE;
_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(void)_throw(_tmp394);_LL1EB:;}}}{struct
_handler_cons _tmp395;_push_handler(& _tmp395);{int _tmp397=0;if(setjmp(_tmp395.handler))
_tmp397=1;if(!_tmp397){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp396=(void*)_exn_thrown;void*
_tmp399=_tmp396;_LL1F1: if(*((void**)_tmp399)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp399);_LL1F0:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp39A;(_tmp39A.YY18).tag=22;(
_tmp39A.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp39A;});break;}case 3: _LL1EA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp39B;(_tmp39B.YY18).tag=22;(_tmp39B.YY18).f1=({struct
Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->hd=({struct Cyc_Absyn_Decl*
_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E[0]=({struct Cyc_Absyn_Using_d_struct
_tmp39F;_tmp39F.tag=9;_tmp39F.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39F.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39F;});_tmp39E;}));
_tmp39D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp39D;});_tmp39C->tl=0;_tmp39C;});_tmp39B;});Cyc_Lex_leave_using();
break;case 4: _LL1F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A0;(
_tmp3A0.YY18).tag=22;(_tmp3A0.YY18).f1=({struct Cyc_List_List*_tmp3A1=_cycalloc(
sizeof(*_tmp3A1));_tmp3A1->hd=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*
_tmp3A2));_tmp3A2->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp3A3=
_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_Absyn_Using_d_struct _tmp3A4;
_tmp3A4.tag=9;_tmp3A4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp3A4.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A4;});_tmp3A3;}));
_tmp3A2->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3A2;});_tmp3A1->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A1;});_tmp3A0;});
break;case 5: _LL1F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A5;(
_tmp3A5.YY18).tag=22;(_tmp3A5.YY18).f1=({struct Cyc_List_List*_tmp3A6=_cycalloc(
sizeof(*_tmp3A6));_tmp3A6->hd=({struct Cyc_Absyn_Decl*_tmp3A7=_cycalloc(sizeof(*
_tmp3A7));_tmp3A7->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A8=
_cycalloc(sizeof(*_tmp3A8));_tmp3A8[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A9;_tmp3A9.tag=8;_tmp3A9.f1=({struct _dynforward_ptr*_tmp3AA=_cycalloc(
sizeof(*_tmp3AA));_tmp3AA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3AA;});_tmp3A9.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A9;});
_tmp3A8;}));_tmp3A7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3A7;});
_tmp3A6->tl=0;_tmp3A6;});_tmp3A5;});Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {
struct _dynforward_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3AB;_push_handler(& _tmp3AB);{int _tmp3AD=0;if(setjmp(_tmp3AB.handler))
_tmp3AD=1;if(!_tmp3AD){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3AC=(void*)_exn_thrown;void*_tmp3AF=_tmp3AC;_LL1FA: if(*((void**)_tmp3AF)
!= Cyc_Core_Failure)goto _LL1FC;_LL1FB: nspace=({const char*_tmp3B0="";
_tag_dynforward(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,1));});goto
_LL1F9;_LL1FC:;_LL1FD:(void)_throw(_tmp3AF);_LL1F9:;}}}{struct _handler_cons
_tmp3B1;_push_handler(& _tmp3B1);{int _tmp3B3=0;if(setjmp(_tmp3B1.handler))_tmp3B3=
1;if(!_tmp3B3){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp3B2=(void*)_exn_thrown;void*
_tmp3B5=_tmp3B2;_LL1FF: if(*((void**)_tmp3B5)!= Cyc_Core_Failure)goto _LL201;
_LL200: x=0;goto _LL1FE;_LL201:;_LL202:(void)_throw(_tmp3B5);_LL1FE:;}}}{struct
_handler_cons _tmp3B6;_push_handler(& _tmp3B6);{int _tmp3B8=0;if(setjmp(_tmp3B6.handler))
_tmp3B8=1;if(!_tmp3B8){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp3B7=(void*)_exn_thrown;void*
_tmp3BA=_tmp3B7;_LL204: if(*((void**)_tmp3BA)!= Cyc_Core_Failure)goto _LL206;
_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(_tmp3BA);_LL203:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3BB;(_tmp3BB.YY18).tag=22;(
_tmp3BB.YY18).f1=({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));
_tmp3BC->hd=({struct Cyc_Absyn_Decl*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->r=(
void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3BE=_cycalloc(sizeof(*
_tmp3BE));_tmp3BE[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp3BF;_tmp3BF.tag=8;
_tmp3BF.f1=({struct _dynforward_ptr*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0[0]=
nspace;_tmp3C0;});_tmp3BF.f2=x;_tmp3BF;});_tmp3BE;}));_tmp3BD->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3BD;});
_tmp3BC->tl=y;_tmp3BC;});_tmp3BB;});break;}case 7: _LL1F8: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),({const char*_tmp3C1="C";_tag_dynforward(_tmp3C1,sizeof(char),
_get_zero_arr_size(_tmp3C1,2));}))== 0)yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3C2;(_tmp3C2.YY18).tag=22;(_tmp3C2.YY18).f1=({struct Cyc_List_List*_tmp3C3=
_cycalloc(sizeof(*_tmp3C3));_tmp3C3->hd=({struct Cyc_Absyn_Decl*_tmp3C4=_cycalloc(
sizeof(*_tmp3C4));_tmp3C4->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3C6;_tmp3C6.tag=10;_tmp3C6.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C6;});_tmp3C5;}));
_tmp3C4->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3C4;});_tmp3C3->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C3;});_tmp3C2;});else{
if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3C7="C include";
_tag_dynforward(_tmp3C7,sizeof(char),_get_zero_arr_size(_tmp3C7,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3C8="expecting \"C\" or \"C include\"";_tag_dynforward(_tmp3C8,
sizeof(char),_get_zero_arr_size(_tmp3C8,29));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3C9;(_tmp3C9.YY18).tag=22;(_tmp3C9.YY18).f1=({
struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->hd=({struct Cyc_Absyn_Decl*
_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3CD;_tmp3CD.tag=11;_tmp3CD.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3CD.f2=0;_tmp3CD;});
_tmp3CC;}));_tmp3CB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CB;});
_tmp3CA->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CA;});_tmp3C9;});}break;case 8: _LL208: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3CE="C include";_tag_dynforward(_tmp3CE,
sizeof(char),_get_zero_arr_size(_tmp3CE,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp3CF="expecting \"C include\"";_tag_dynforward(_tmp3CF,sizeof(char),
_get_zero_arr_size(_tmp3CF,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D0;(_tmp3D0.YY18).tag=
22;(_tmp3D0.YY18).f1=({struct Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));
_tmp3D1->hd=({struct Cyc_Absyn_Decl*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3D3=_cycalloc(
sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3D4;
_tmp3D4.tag=11;_tmp3D4.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3D4.f2=exs;_tmp3D4;});_tmp3D3;}));_tmp3D2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3D2;});
_tmp3D1->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3D1;});_tmp3D0;});break;}case 9: _LL209: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D5;(_tmp3D5.YY18).tag=22;(_tmp3D5.YY18).f1=({struct
Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->hd=({struct Cyc_Absyn_Decl*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->r=(void*)((void*)0);_tmp3D7->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3D7;});_tmp3D6->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D6;});_tmp3D5;});
break;case 10: _LL20A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D8;(_tmp3D8.YY18).tag=22;(_tmp3D8.YY18).f1=({struct Cyc_List_List*_tmp3D9=
_cycalloc(sizeof(*_tmp3D9));_tmp3D9->hd=({struct Cyc_Absyn_Decl*_tmp3DA=_cycalloc(
sizeof(*_tmp3DA));_tmp3DA->r=(void*)((void*)1);_tmp3DA->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3DA;});
_tmp3D9->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3D9;});_tmp3D8;});break;case 11: _LL20B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3DB;(_tmp3DB.YY18).tag=22;(_tmp3DB.YY18).f1=0;_tmp3DB;});
break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL20D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DC;(_tmp3DC.YY52).tag=56;(_tmp3DC.YY52).f1=0;_tmp3DC;});break;case 14: _LL20E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3DD;(_tmp3DD.YY52).tag=
56;(_tmp3DD.YY52).f1=({struct Cyc_List_List*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));
_tmp3DE->hd=({struct _tuple18*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3DF->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3DF->f3=0;_tmp3DF;});_tmp3DE->tl=0;_tmp3DE;});_tmp3DD;});break;case 15: _LL20F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E0;(_tmp3E0.YY52).tag=
56;(_tmp3E0.YY52).f1=({struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));
_tmp3E1->hd=({struct _tuple18*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3E2->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3E2->f3=0;_tmp3E2;});_tmp3E1->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E1;});_tmp3E0;});
break;case 16: _LL210: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E3;(_tmp3E3.YY18).tag=22;(_tmp3E3.YY18).f1=({struct Cyc_List_List*_tmp3E4=
_cycalloc(sizeof(*_tmp3E4));_tmp3E4->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3E6;_tmp3E6.tag=1;_tmp3E6.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E6;});_tmp3E5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E4->tl=0;
_tmp3E4;});_tmp3E3;});break;case 17: _LL211: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL212: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E7;(_tmp3E7.YY18).tag=22;(
_tmp3E7.YY18).f1=0;_tmp3E7;});break;case 19: _LL213: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3E8;(_tmp3E8.YY17).tag=21;(_tmp3E8.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E8;});
break;case 20: _LL214: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E9;(_tmp3E9.YY17).tag=21;(_tmp3E9.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3EA;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E9;});
break;case 21: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EB;(_tmp3EB.YY17).tag=21;(_tmp3EB.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EB;});
break;case 22: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EC;(_tmp3EC.YY17).tag=21;(_tmp3EC.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3ED;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EC;});
break;case 23: _LL217: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EE;(_tmp3EE.YY17).tag=21;(_tmp3EE.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3EF;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EE;});
break;case 24: _LL218: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F0;(_tmp3F0.YY17).tag=21;(_tmp3F0.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3F1;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F0;});
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B: Cyc_Lex_enter_namespace(({struct _dynforward_ptr*_tmp3F2=
_cycalloc(sizeof(*_tmp3F2));_tmp3F2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3F2;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F3;(_tmp3F3.YY18).tag=22;(_tmp3F3.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3F3;});
break;case 30: _LL21E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F4;(_tmp3F4.YY18).tag=22;(_tmp3F4.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F4;});
break;case 31: _LL21F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F5;(_tmp3F5.YY18).tag=22;(_tmp3F5.YY18).f1=({struct Cyc_List_List*_tmp3F6=
_cycalloc(sizeof(*_tmp3F6));_tmp3F6->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F6->tl=0;
_tmp3F6;});_tmp3F5;});break;case 32: _LL220: {struct Cyc_List_List*_tmp3F7=0;{
struct Cyc_List_List*_tmp3F8=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3F8 != 0;_tmp3F8=_tmp3F8->tl){struct
_dynforward_ptr*_tmp3F9=(struct _dynforward_ptr*)_tmp3F8->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3FD;(_tmp3FD.Rel_n).tag=1;(_tmp3FD.Rel_n).f1=0;
_tmp3FD;});_tmp3FC->f2=_tmp3F9;_tmp3FC;});struct Cyc_Absyn_Vardecl*_tmp3FA=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3F7=({struct Cyc_List_List*_tmp3FB=_cycalloc(
sizeof(*_tmp3FB));_tmp3FB->hd=_tmp3FA;_tmp3FB->tl=_tmp3F7;_tmp3FB;});}}_tmp3F7=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F7);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3FE;(_tmp3FE.YY18).tag=22;(
_tmp3FE.YY18).f1=({struct Cyc_List_List*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));
_tmp3FF->hd=Cyc_Absyn_letv_decl(_tmp3F7,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3FF->tl=0;
_tmp3FF;});_tmp3FE;});break;}case 33: _LL221: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL222: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp400;(_tmp400.YY18).tag=22;(
_tmp400.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp400;});break;case 35: _LL223: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp401;(_tmp401.YY19).tag=23;(_tmp401.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->sc=({
struct Cyc_Core_Opt*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp403;});_tmp402->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp402->type_specs=
0;_tmp402->is_inline=0;_tmp402->attributes=0;_tmp402;});_tmp401;});break;case 36:
_LL224: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp404[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp405="Only one storage class is allowed in a declaration";_tag_dynforward(
_tmp405,sizeof(char),_get_zero_arr_size(_tmp405,51));}),_tag_dynforward(_tmp404,
sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp406;(_tmp406.YY19).tag=23;(_tmp406.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->sc=({struct Cyc_Core_Opt*_tmp408=
_cycalloc(sizeof(*_tmp408));_tmp408->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp408;});_tmp407->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp407->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs;_tmp407->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp407->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp407;});_tmp406;});break;case 37: _LL225: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp409;(_tmp409.YY19).tag=23;(_tmp409.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->sc=0;_tmp40A->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp40A->type_specs=({struct Cyc_List_List*_tmp40B=
_cycalloc(sizeof(*_tmp40B));_tmp40B->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40B->tl=0;_tmp40B;});
_tmp40A->is_inline=0;_tmp40A->attributes=0;_tmp40A;});_tmp409;});break;case 38:
_LL226: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp40C;(_tmp40C.YY19).tag=
23;(_tmp40C.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp40D=_cycalloc(
sizeof(*_tmp40D));_tmp40D->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40D->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp40D->type_specs=({
struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40E->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp40E;});
_tmp40D->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp40D->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp40D;});
_tmp40C;});break;case 39: _LL227: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40F;(_tmp40F.YY19).tag=23;(_tmp40F.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->sc=0;_tmp410->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp410->type_specs=0;
_tmp410->is_inline=0;_tmp410->attributes=0;_tmp410;});_tmp40F;});break;case 40:
_LL228: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp411;(_tmp411.YY19).tag=
23;(_tmp411.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp412=_cycalloc(
sizeof(*_tmp412));_tmp412->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp412->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp412->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp412->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp412->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp412;});
_tmp411;});break;case 41: _LL229: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp413;(_tmp413.YY19).tag=23;(_tmp413.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->sc=0;_tmp414->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp414->type_specs=0;_tmp414->is_inline=1;_tmp414->attributes=
0;_tmp414;});_tmp413;});break;case 42: _LL22A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp415;(_tmp415.YY19).tag=23;(_tmp415.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp416->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp416->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp416->is_inline=1;_tmp416->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp416;});
_tmp415;});break;case 43: _LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp417;(_tmp417.YY19).tag=23;(_tmp417.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->sc=0;_tmp418->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp418->type_specs=0;_tmp418->is_inline=0;_tmp418->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp418;});
_tmp417;});break;case 44: _LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp419;(_tmp419.YY19).tag=23;(_tmp419.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp41A->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp41A->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp41A->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp41A->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp41A;});
_tmp419;});break;case 45: _LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41B;(_tmp41B.YY22).tag=26;(_tmp41B.YY22).f1=(void*)((void*)4);_tmp41B;});
break;case 46: _LL22E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41C;(_tmp41C.YY22).tag=26;(_tmp41C.YY22).f1=(void*)((void*)5);_tmp41C;});
break;case 47: _LL22F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41D;(_tmp41D.YY22).tag=26;(_tmp41D.YY22).f1=(void*)((void*)3);_tmp41D;});
break;case 48: _LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41E;(_tmp41E.YY22).tag=26;(_tmp41E.YY22).f1=(void*)((void*)1);_tmp41E;});
break;case 49: _LL231: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp41F="C";
_tag_dynforward(_tmp41F,sizeof(char),_get_zero_arr_size(_tmp41F,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp420="only extern or extern \"C\" is allowed";_tag_dynforward(
_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp421;(_tmp421.YY22).tag=26;(_tmp421.YY22).f1=(void*)((
void*)2);_tmp421;});break;case 50: _LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp422;(_tmp422.YY22).tag=26;(_tmp422.YY22).f1=(void*)((void*)0);_tmp422;});
break;case 51: _LL233: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp423;(_tmp423.YY22).tag=26;(_tmp423.YY22).f1=(void*)((void*)6);_tmp423;});
break;case 52: _LL234: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp424;(_tmp424.YY45).tag=49;(_tmp424.YY45).f1=0;_tmp424;});break;case 53: _LL235:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54:
_LL236: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 55: _LL237: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp425;(
_tmp425.YY45).tag=49;(_tmp425.YY45).f1=({struct Cyc_List_List*_tmp426=_cycalloc(
sizeof(*_tmp426));_tmp426->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp426->tl=0;_tmp426;});
_tmp425;});break;case 56: _LL238: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp427;(_tmp427.YY45).tag=49;(_tmp427.YY45).f1=({struct Cyc_List_List*_tmp428=
_cycalloc(sizeof(*_tmp428));_tmp428->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp428->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp428;});_tmp427;});
break;case 57: _LL239: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp42D,_tmp42D + 8},(void*)0},{{_tmp42E,
_tmp42E + 6},(void*)1},{{_tmp42F,_tmp42F + 9},(void*)2},{{_tmp430,_tmp430 + 9},(
void*)3},{{_tmp431,_tmp431 + 6},(void*)4},{{_tmp432,_tmp432 + 8},(void*)&
att_aligned},{{_tmp433,_tmp433 + 7},(void*)5},{{_tmp434,_tmp434 + 7},(void*)7},{{
_tmp435,_tmp435 + 7},(void*)8},{{_tmp436,_tmp436 + 5},(void*)9},{{_tmp437,_tmp437 + 
10},(void*)10},{{_tmp438,_tmp438 + 10},(void*)11},{{_tmp439,_tmp439 + 23},(void*)
12},{{_tmp43A,_tmp43A + 12},(void*)13},{{_tmp43B,_tmp43B + 11},(void*)14},{{
_tmp43C,_tmp43C + 22},(void*)15},{{_tmp43D,_tmp43D + 5},(void*)16}};struct
_dynforward_ptr _tmp429=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp429,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp429,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp429,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp429,sizeof(char),(int)(
_get_dynforward_size(_tmp429,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp429,sizeof(char),(int)(_get_dynforward_size(
_tmp429,sizeof(char))- 3)))== '_')_tmp429=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp429,2,_get_dynforward_size(_tmp429,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp429,(struct
_dynforward_ptr)(att_map[i]).f1)== 0){yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp42A;(_tmp42A.YY46).tag=50;(_tmp42A.YY46).f1=(void*)(att_map[i]).f2;_tmp42A;});
break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp42B="unrecognized attribute";
_tag_dynforward(_tmp42B,sizeof(char),_get_zero_arr_size(_tmp42B,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp42C;(_tmp42C.YY46).tag=50;(_tmp42C.YY46).f1=(void*)((
void*)1);_tmp42C;});}break;}}case 58: _LL23A: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp43F;(_tmp43F.YY46).tag=50;(_tmp43F.YY46).f1=(void*)((void*)
4);_tmp43F;});break;case 59: _LL23B: {struct _dynforward_ptr _tmp440=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp442;struct
_tuple11 _tmp441=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp442=_tmp441.f2;{void*a;if(Cyc_zstrcmp((struct
_dynforward_ptr)_tmp440,({const char*_tmp443="regparm";_tag_dynforward(_tmp443,
sizeof(char),_get_zero_arr_size(_tmp443,8));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)_tmp440,({const char*_tmp444="__regparm__";_tag_dynforward(
_tmp444,sizeof(char),_get_zero_arr_size(_tmp444,12));}))== 0){if(_tmp442 < 0  || 
_tmp442 > 3)Cyc_Parse_err(({const char*_tmp445="regparm requires value between 0 and 3";
_tag_dynforward(_tmp445,sizeof(char),_get_zero_arr_size(_tmp445,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp446=_cycalloc_atomic(sizeof(*_tmp446));
_tmp446[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp447;_tmp447.tag=0;_tmp447.f1=
_tmp442;_tmp447;});_tmp446;});}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp440,({const char*_tmp448="aligned";_tag_dynforward(_tmp448,sizeof(char),
_get_zero_arr_size(_tmp448,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp440,({const char*_tmp449="__aligned__";_tag_dynforward(_tmp449,sizeof(char),
_get_zero_arr_size(_tmp449,12));}))== 0){if(_tmp442 < 0)Cyc_Parse_err(({const char*
_tmp44A="aligned requires positive power of two";_tag_dynforward(_tmp44A,sizeof(
char),_get_zero_arr_size(_tmp44A,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp442;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp44B="aligned requires positive power of two";
_tag_dynforward(_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp44C=_cycalloc_atomic(sizeof(*_tmp44C));
_tmp44C[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp44D;_tmp44D.tag=1;_tmp44D.f1=
_tmp442;_tmp44D;});_tmp44C;});}}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp440,({const char*_tmp44E="initializes";_tag_dynforward(_tmp44E,sizeof(char),
_get_zero_arr_size(_tmp44E,12));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp440,({const char*_tmp44F="__initializes__";_tag_dynforward(_tmp44F,sizeof(
char),_get_zero_arr_size(_tmp44F,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp450=_cycalloc_atomic(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp451;_tmp451.tag=4;_tmp451.f1=_tmp442;_tmp451;});_tmp450;});else{Cyc_Parse_err(({
const char*_tmp452="unrecognized attribute";_tag_dynforward(_tmp452,sizeof(char),
_get_zero_arr_size(_tmp452,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp453;(_tmp453.YY46).tag=
50;(_tmp453.YY46).f1=(void*)a;_tmp453;});break;}}case 60: _LL23C: {struct
_dynforward_ptr _tmp454=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp455=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp454,({const char*_tmp456="section";_tag_dynforward(
_tmp456,sizeof(char),_get_zero_arr_size(_tmp456,8));}))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp454,({const char*_tmp457="__section__";_tag_dynforward(
_tmp457,sizeof(char),_get_zero_arr_size(_tmp457,12));}))== 0)a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458[0]=({
struct Cyc_Absyn_Section_att_struct _tmp459;_tmp459.tag=2;_tmp459.f1=_tmp455;
_tmp459;});_tmp458;});else{Cyc_Parse_err(({const char*_tmp45A="unrecognized attribute";
_tag_dynforward(_tmp45A,sizeof(char),_get_zero_arr_size(_tmp45A,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45B;(_tmp45B.YY46).tag=
50;(_tmp45B.YY46).f1=(void*)a;_tmp45B;});break;}case 61: _LL23D: {struct
_dynforward_ptr _tmp45C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp45D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp45C,({const char*_tmp45E="__mode__";_tag_dynforward(
_tmp45E,sizeof(char),_get_zero_arr_size(_tmp45E,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp460;_tmp460.tag=5;_tmp460.f1=_tmp45D;_tmp460;});_tmp45F;});else{Cyc_Parse_err(({
const char*_tmp461="unrecognized attribute";_tag_dynforward(_tmp461,sizeof(char),
_get_zero_arr_size(_tmp461,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp462;(_tmp462.YY46).tag=
50;(_tmp462.YY46).f1=(void*)a;_tmp462;});break;}case 62: _LL23E: {struct
_dynforward_ptr _tmp463=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp464=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp466;struct
_tuple11 _tmp465=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp466=_tmp465.f2;{int _tmp468;struct _tuple11 _tmp467=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp468=_tmp467.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp463,({const char*
_tmp469="format";_tag_dynforward(_tmp469,sizeof(char),_get_zero_arr_size(_tmp469,
7));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp463,({const char*_tmp46A="__format__";
_tag_dynforward(_tmp46A,sizeof(char),_get_zero_arr_size(_tmp46A,11));}))== 0){
if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp464,({const char*_tmp46B="printf";
_tag_dynforward(_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,7));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));
_tmp46C[0]=({struct Cyc_Absyn_Format_att_struct _tmp46D;_tmp46D.tag=3;_tmp46D.f1=(
void*)((void*)0);_tmp46D.f2=_tmp466;_tmp46D.f3=_tmp468;_tmp46D;});_tmp46C;});
else{if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp464,({const char*_tmp46E="scanf";
_tag_dynforward(_tmp46E,sizeof(char),_get_zero_arr_size(_tmp46E,6));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));
_tmp46F[0]=({struct Cyc_Absyn_Format_att_struct _tmp470;_tmp470.tag=3;_tmp470.f1=(
void*)((void*)1);_tmp470.f2=_tmp466;_tmp470.f3=_tmp468;_tmp470;});_tmp46F;});
else{Cyc_Parse_err(({const char*_tmp471="unrecognized format type";
_tag_dynforward(_tmp471,sizeof(char),_get_zero_arr_size(_tmp471,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp472="unrecognized attribute";_tag_dynforward(_tmp472,sizeof(char),
_get_zero_arr_size(_tmp472,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp473;(_tmp473.YY46).tag=50;(_tmp473.YY46).f1=(
void*)a;_tmp473;});break;}}}case 63: _LL23F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL240: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp474;(_tmp474.YY23).tag=27;(
_tmp474.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp476;_tmp476.tag=17;_tmp476.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp476.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp476.f3=0;_tmp476.f4=
0;_tmp476;});_tmp475;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp474;});
break;case 65: _LL241: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp477;(_tmp477.YY23).tag=27;(_tmp477.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp477;});break;case 66: _LL242: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp478;(_tmp478.YY23).tag=27;(_tmp478.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp478;});
break;case 67: _LL243: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp479;(_tmp479.YY23).tag=27;(_tmp479.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->v=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp47A;}),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp479;});
break;case 68: _LL244: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47B;(_tmp47B.YY23).tag=27;(_tmp47B.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp47B;});break;case 69: _LL245: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp47C;(_tmp47C.YY23).tag=27;(_tmp47C.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));
_tmp47D[0]=({struct Cyc_Parse_Short_spec_struct _tmp47E;_tmp47E.tag=2;_tmp47E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp47E;});_tmp47D;}));_tmp47C;});break;case 70: _LL246:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp47F;(_tmp47F.YY23).tag=
27;(_tmp47F.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47F;});
break;case 71: _LL247: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp480;(_tmp480.YY23).tag=27;(_tmp480.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Parse_Long_spec_struct
_tmp482;_tmp482.tag=3;_tmp482.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp482;});
_tmp481;}));_tmp480;});break;case 72: _LL248: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp483;(_tmp483.YY23).tag=27;(_tmp483.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp483;});
break;case 73: _LL249: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp484;(_tmp484.YY23).tag=27;(_tmp484.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp484;});break;case 74: _LL24A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp485;(_tmp485.YY23).tag=27;(_tmp485.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp486=_cycalloc(sizeof(*_tmp486));
_tmp486[0]=({struct Cyc_Parse_Signed_spec_struct _tmp487;_tmp487.tag=0;_tmp487.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp487;});_tmp486;}));_tmp485;});break;case 75: _LL24B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp488;(_tmp488.YY23).tag=
27;(_tmp488.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp48A;_tmp48A.tag=1;_tmp48A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp48A;});
_tmp489;}));_tmp488;});break;case 76: _LL24C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL24D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL24E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL24F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp48B;(_tmp48B.YY23).tag=
27;(_tmp48B.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48B;});
break;case 80: _LL250: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp48C;(_tmp48C.YY23).tag=27;(_tmp48C.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({
struct Cyc_Absyn_TupleType_struct _tmp48E;_tmp48E.tag=9;_tmp48E.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp48E;});_tmp48D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp48C;});break;case 81: _LL251: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp48F;(_tmp48F.YY23).tag=27;(_tmp48F.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp490=_cycalloc(sizeof(*_tmp490));
_tmp490[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp491;_tmp491.tag=15;_tmp491.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp491;});_tmp490;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48F;});
break;case 82: _LL252: {void*_tmp492=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->v=(void*)((void*)3);_tmp496;}),0);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp493;(_tmp493.YY23).tag=
27;(_tmp493.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp495;_tmp495.tag=16;_tmp495.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp495.f2=(void*)
_tmp492;_tmp495;});_tmp494;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp493;});
break;}case 83: _LL253: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp497;(_tmp497.YY23).tag=27;(_tmp497.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp499;_tmp499.tag=16;_tmp499.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp499.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp499;});_tmp498;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp497;});
break;case 84: _LL254: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp49A;(_tmp49A.YY23).tag=27;(_tmp49A.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp49C;_tmp49C.tag=14;_tmp49C.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49C;});_tmp49B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp49A;});break;case 85: _LL255: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp49D;(_tmp49D.YY23).tag=27;(_tmp49D.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));
_tmp49E[0]=({struct Cyc_Absyn_TagType_struct _tmp49F;_tmp49F.tag=18;_tmp49F.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp49F;});_tmp49E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49D;});
break;case 86: _LL256: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A0;(_tmp4A0.YY23).tag=27;(_tmp4A0.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({
struct Cyc_Absyn_TagType_struct _tmp4A2;_tmp4A2.tag=18;_tmp4A2.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->v=(void*)((void*)
7);_tmp4A3;}),0);_tmp4A2;});_tmp4A1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A0;});
break;case 87: _LL257: {int _tmp4A5;struct _tuple11 _tmp4A4=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A5=_tmp4A4.f2;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4A6;(_tmp4A6.YY23).tag=27;(
_tmp4A6.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp4A7=_cycalloc_atomic(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp4A8;_tmp4A8.tag=19;_tmp4A8.f1=_tmp4A5;_tmp4A8;});_tmp4A7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A6;});
break;}case 88: _LL258: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A9;(_tmp4A9.YY43).tag=47;(_tmp4A9.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A9;});
break;case 89: _LL259: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AA;(_tmp4AA.YY25).tag=29;(_tmp4AA.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4AB;
_tmp4AB.print_const=1;_tmp4AB.q_volatile=0;_tmp4AB.q_restrict=0;_tmp4AB.real_const=
1;_tmp4AB.loc=loc;_tmp4AB;});_tmp4AA;});break;}case 90: _LL25A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4AC;(_tmp4AC.YY25).tag=29;(_tmp4AC.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp4AD;_tmp4AD.print_const=0;_tmp4AD.q_volatile=1;_tmp4AD.q_restrict=
0;_tmp4AD.real_const=0;_tmp4AD.loc=0;_tmp4AD;});_tmp4AC;});break;case 91: _LL25B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4AE;(_tmp4AE.YY25).tag=
29;(_tmp4AE.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4AF;_tmp4AF.print_const=0;
_tmp4AF.q_volatile=0;_tmp4AF.q_restrict=1;_tmp4AF.real_const=0;_tmp4AF.loc=0;
_tmp4AF;});_tmp4AE;});break;case 92: _LL25C: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4B0;(_tmp4B0.YY23).tag=27;(_tmp4B0.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4B2;_tmp4B2.tag=5;_tmp4B2.f1=({struct Cyc_Absyn_Decl*
_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4B5;_tmp4B5.tag=6;_tmp4B5.f1=({struct Cyc_Absyn_Enumdecl*_tmp4B6=_cycalloc(
sizeof(*_tmp4B6));_tmp4B6->sc=(void*)((void*)2);_tmp4B6->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4B6->fields=({
struct Cyc_Core_Opt*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B7;});_tmp4B6;});
_tmp4B5;});_tmp4B4;}));_tmp4B3->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B3;});
_tmp4B2;});_tmp4B1;}));_tmp4B0;});break;case 93: _LL25D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B8;(_tmp4B8.YY23).tag=27;(_tmp4B8.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));
_tmp4B9[0]=({struct Cyc_Absyn_EnumType_struct _tmp4BA;_tmp4BA.tag=12;_tmp4BA.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4BA.f2=0;_tmp4BA;});_tmp4B9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B8;});
break;case 94: _LL25E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4BB;(_tmp4BB.YY23).tag=27;(_tmp4BB.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4BD;_tmp4BD.tag=4;_tmp4BD.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4BF;_tmp4BF.tag=13;_tmp4BF.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BF;});_tmp4BE;}));
_tmp4BD.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4BD;});_tmp4BC;}));_tmp4BB;});break;case 95: _LL25F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C0;(_tmp4C0.YY47).tag=
51;(_tmp4C0.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4C1=_cycalloc(sizeof(*
_tmp4C1));_tmp4C1->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4C1->tag=0;_tmp4C1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4C1;});
_tmp4C0;});break;case 96: _LL260: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C2;(_tmp4C2.YY47).tag=51;(_tmp4C2.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C3->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C3->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4C3;});_tmp4C2;});break;case 97: _LL261: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C4;(_tmp4C4.YY48).tag=52;(
_tmp4C4.YY48).f1=({struct Cyc_List_List*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));
_tmp4C5->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C5->tl=0;_tmp4C5;});_tmp4C4;});break;case 98: _LL262: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C6;(_tmp4C6.YY48).tag=52;(
_tmp4C6.YY48).f1=({struct Cyc_List_List*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));
_tmp4C7->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4C7->tl=0;_tmp4C7;});_tmp4C6;});break;case 99: _LL263: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C8;(_tmp4C8.YY48).tag=52;(
_tmp4C8.YY48).f1=({struct Cyc_List_List*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));
_tmp4C9->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4C9->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C9;});_tmp4C8;});
break;case 100: _LL264: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CA;(_tmp4CA.YY23).tag=27;(_tmp4CA.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4CC;_tmp4CC.tag=11;_tmp4CC.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4CC.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4CC;});_tmp4CB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CA;});
break;case 101: _LL265: {struct Cyc_List_List*_tmp4CD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CF;(_tmp4CF.YY23).tag=27;(_tmp4CF.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4D1;_tmp4D1.tag=5;_tmp4D1.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4CD,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4CE,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D1;});
_tmp4D0;}));_tmp4CF;});break;}case 102: _LL266: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D2;(_tmp4D2.YY23).tag=27;(_tmp4D2.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));
_tmp4D3[0]=({struct Cyc_Absyn_AggrType_struct _tmp4D4;_tmp4D4.tag=10;_tmp4D4.f1=({
struct Cyc_Absyn_AggrInfo _tmp4D5;_tmp4D5.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp4D6;(_tmp4D6.UnknownAggr).tag=0;(_tmp4D6.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp4D6.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4D6;});_tmp4D5.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D5;});_tmp4D4;});
_tmp4D3;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4D2;});break;case 103: _LL267: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D7;(_tmp4D7.YY40).tag=44;(_tmp4D7.YY40).f1=0;_tmp4D7;});
break;case 104: _LL268: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D8;(_tmp4D8.YY40).tag=44;(_tmp4D8.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4D8;});break;
case 105: _LL269: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D9;(
_tmp4D9.YY24).tag=28;(_tmp4D9.YY24).f1=(void*)((void*)0);_tmp4D9;});break;case
106: _LL26A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DA;(
_tmp4DA.YY24).tag=28;(_tmp4DA.YY24).f1=(void*)((void*)1);_tmp4DA;});break;case
107: _LL26B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DB;(
_tmp4DB.YY26).tag=30;(_tmp4DB.YY26).f1=0;_tmp4DB;});break;case 108: _LL26C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DC;(_tmp4DC.YY26).tag=30;(
_tmp4DC.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4DC;});break;
case 109: _LL26D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DD;(
_tmp4DD.YY27).tag=31;(_tmp4DD.YY27).f1=({struct Cyc_List_List*_tmp4DE=_cycalloc(
sizeof(*_tmp4DE));_tmp4DE->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4DE->tl=0;_tmp4DE;});_tmp4DD;});break;case 110: _LL26E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DF;(_tmp4DF.YY27).tag=
31;(_tmp4DF.YY27).f1=({struct Cyc_List_List*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));
_tmp4E0->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E0->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4E0;});_tmp4DF;});break;case 111: _LL26F: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E1;(_tmp4E1.YY21).tag=25;(_tmp4E1.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4E1;});break;case
112: _LL270: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E2;(
_tmp4E2.YY21).tag=25;(_tmp4E2.YY21).f1=({struct Cyc_List_List*_tmp4E3=_cycalloc(
sizeof(*_tmp4E3));_tmp4E3->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E3->tl=0;_tmp4E3;});_tmp4E2;});break;case 113: _LL271:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E4;(_tmp4E4.YY21).tag=
25;(_tmp4E4.YY21).f1=({struct Cyc_List_List*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));
_tmp4E5->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E5->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp4E5;});_tmp4E4;});break;case 114: _LL272: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E6;(_tmp4E6.YY20).tag=24;(_tmp4E6.YY20).f1=({
struct _tuple15*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E7->f2=0;_tmp4E7;});
_tmp4E6;});break;case 115: _LL273: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E8;(_tmp4E8.YY20).tag=24;(_tmp4E8.YY20).f1=({struct _tuple15*_tmp4E9=
_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E9->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E9;});
_tmp4E8;});break;case 116: _LL274:{struct _RegionHandle _tmp4EA=_new_region("temp");
struct _RegionHandle*temp=& _tmp4EA;_push_region(temp);{struct _tuple16 _tmp4EC;
struct Cyc_Absyn_Tqual _tmp4ED;struct Cyc_List_List*_tmp4EE;struct Cyc_List_List*
_tmp4EF;struct _tuple16*_tmp4EB=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EC=*_tmp4EB;_tmp4ED=
_tmp4EC.f1;_tmp4EE=_tmp4EC.f2;_tmp4EF=_tmp4EC.f3;if(_tmp4ED.loc == 0)_tmp4ED.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4F1;struct Cyc_List_List*
_tmp4F2;struct _tuple0 _tmp4F0=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4F1=_tmp4F0.f1;
_tmp4F2=_tmp4F0.f2;{void*_tmp4F3=Cyc_Parse_speclist2typ(_tmp4EE,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4F4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4ED,_tmp4F3,_tmp4F1,_tmp4EF),_tmp4F2);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F5;(_tmp4F5.YY26).tag=30;(_tmp4F5.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4F4);
_tmp4F5;});}}};_pop_region(temp);}break;case 117: _LL275: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F6;(_tmp4F6.YY35).tag=39;(_tmp4F6.YY35).f1=({struct
_tuple16*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F7->f2=({struct Cyc_List_List*_tmp4F8=_cycalloc(
sizeof(*_tmp4F8));_tmp4F8->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F8->tl=0;_tmp4F8;});
_tmp4F7->f3=0;_tmp4F7;});_tmp4F6;});break;case 118: _LL276: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F9;(_tmp4F9.YY35).tag=39;(_tmp4F9.YY35).f1=({struct
_tuple16*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4FA->f2=({struct Cyc_List_List*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FB->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4FB;});_tmp4FA->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4FA;});_tmp4F9;});break;case 119: _LL277: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FC;(_tmp4FC.YY35).tag=39;(_tmp4FC.YY35).f1=({struct
_tuple16*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FD->f2=0;_tmp4FD->f3=0;
_tmp4FD;});_tmp4FC;});break;case 120: _LL278: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FE;(_tmp4FE.YY35).tag=39;(_tmp4FE.YY35).f1=({struct
_tuple16*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp4FF->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp4FF->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4FF;});_tmp4FE;});
break;case 121: _LL279: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp500;(_tmp500.YY35).tag=39;(_tmp500.YY35).f1=({struct _tuple16*_tmp501=
_cycalloc(sizeof(*_tmp501));_tmp501->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp501->f2=0;
_tmp501->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp501;});_tmp500;});break;case 122: _LL27A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp502;(_tmp502.YY35).tag=39;(_tmp502.YY35).f1=({struct
_tuple16*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp503->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp503->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp503;});_tmp502;});break;case 123: _LL27B: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp504;(_tmp504.YY35).tag=39;(_tmp504.YY35).f1=({struct
_tuple16*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp505->f2=({struct Cyc_List_List*_tmp506=_cycalloc(
sizeof(*_tmp506));_tmp506->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp506->tl=0;_tmp506;});
_tmp505->f3=0;_tmp505;});_tmp504;});break;case 124: _LL27C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp507;(_tmp507.YY35).tag=39;(_tmp507.YY35).f1=({struct
_tuple16*_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp508->f2=({struct Cyc_List_List*
_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp509->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp509;});_tmp508->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp508;});_tmp507;});break;case 125: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp50A;(_tmp50A.YY35).tag=39;(_tmp50A.YY35).f1=({struct
_tuple16*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50B->f2=0;_tmp50B->f3=0;
_tmp50B;});_tmp50A;});break;case 126: _LL27E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp50C;(_tmp50C.YY35).tag=39;(_tmp50C.YY35).f1=({struct
_tuple16*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp50D->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp50D->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp50D;});_tmp50C;});
break;case 127: _LL27F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp50E;(_tmp50E.YY35).tag=39;(_tmp50E.YY35).f1=({struct _tuple16*_tmp50F=
_cycalloc(sizeof(*_tmp50F));_tmp50F->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp50F->f2=0;
_tmp50F->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp50F;});_tmp50E;});break;case 128: _LL280: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp510;(_tmp510.YY35).tag=39;(_tmp510.YY35).f1=({struct
_tuple16*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp511->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp511->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp511;});_tmp510;});break;case 129: _LL281: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp512;(_tmp512.YY21).tag=25;(_tmp512.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp512;});break;case 130:
_LL282: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp513;(_tmp513.YY21).tag=
25;(_tmp513.YY21).f1=({struct Cyc_List_List*_tmp514=_cycalloc(sizeof(*_tmp514));
_tmp514->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp514->tl=0;_tmp514;});_tmp513;});break;case 131: _LL283: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp515;(_tmp515.YY21).tag=25;(
_tmp515.YY21).f1=({struct Cyc_List_List*_tmp516=_cycalloc(sizeof(*_tmp516));
_tmp516->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp516->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp516;});_tmp515;});break;case 132: _LL284: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp517;(_tmp517.YY20).tag=24;(_tmp517.YY20).f1=({
struct _tuple15*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp518->f2=0;_tmp518;});
_tmp517;});break;case 133: _LL285: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp519;(_tmp519.YY20).tag=24;(_tmp519.YY20).f1=({struct _tuple15*_tmp51A=
_cycalloc(sizeof(*_tmp51A));_tmp51A->f1=({struct Cyc_Parse_Declarator*_tmp51B=
_cycalloc(sizeof(*_tmp51B));_tmp51B->id=({struct _tuple1*_tmp51C=_cycalloc(
sizeof(*_tmp51C));_tmp51C->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp51E;(_tmp51E.Rel_n).tag=1;(_tmp51E.Rel_n).f1=0;_tmp51E;});_tmp51C->f2=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp51C;});_tmp51B->tms=0;_tmp51B;});_tmp51A->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51A;});_tmp519;});
break;case 134: _LL286: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp51F;(_tmp51F.YY20).tag=24;(_tmp51F.YY20).f1=({struct _tuple15*_tmp520=
_cycalloc(sizeof(*_tmp520));_tmp520->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp520->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp520;});
_tmp51F;});break;case 135: _LL287: {struct Cyc_List_List*_tmp521=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp522;(_tmp522.YY23).tag=27;(_tmp522.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp523=_cycalloc(sizeof(*_tmp523));
_tmp523[0]=({struct Cyc_Parse_Decl_spec_struct _tmp524;_tmp524.tag=5;_tmp524.f1=
Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp521,({struct Cyc_Core_Opt*
_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp525;}),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp524;});
_tmp523;}));_tmp522;});break;}case 136: _LL288: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp526;(_tmp526.YY23).tag=27;(_tmp526.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp527=_cycalloc(sizeof(*_tmp527));
_tmp527[0]=({struct Cyc_Absyn_TunionType_struct _tmp528;_tmp528.tag=2;_tmp528.f1=({
struct Cyc_Absyn_TunionInfo _tmp529;_tmp529.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp52A;(_tmp52A.UnknownTunion).tag=0;(_tmp52A.UnknownTunion).f1=({
struct Cyc_Absyn_UnknownTunionInfo _tmp52B;_tmp52B.name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52B.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp52B.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp52B;});_tmp52A;});
_tmp529.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp529.rgn=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp529;});_tmp528;});_tmp527;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp526;});
break;case 137: _LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52C;(_tmp52C.YY23).tag=27;(_tmp52C.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));
_tmp52D[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp52E;_tmp52E.tag=3;_tmp52E.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp52F;_tmp52F.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp530;(_tmp530.UnknownTunionfield).tag=0;(
_tmp530.UnknownTunionfield).f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp531;
_tmp531.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp531.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp531.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp531;});_tmp530;});
_tmp52F.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp52F;});_tmp52E;});_tmp52D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp52C;});
break;case 138: _LL28A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp532;(_tmp532.YY31).tag=35;(_tmp532.YY31).f1=1;_tmp532;});break;case 139:
_LL28B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp533;(_tmp533.YY31).tag=
35;(_tmp533.YY31).f1=0;_tmp533;});break;case 140: _LL28C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp534;(_tmp534.YY31).tag=35;(_tmp534.YY31).f1=0;_tmp534;});
break;case 141: _LL28D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp535;(_tmp535.YY31).tag=35;(_tmp535.YY31).f1=1;_tmp535;});break;case 142:
_LL28E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp536;(_tmp536.YY34).tag=
38;(_tmp536.YY34).f1=({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));
_tmp537->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp537->tl=0;_tmp537;});_tmp536;});break;case 143: _LL28F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp538;(_tmp538.YY34).tag=38;(
_tmp538.YY34).f1=({struct Cyc_List_List*_tmp539=_cycalloc(sizeof(*_tmp539));
_tmp539->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp539->tl=0;_tmp539;});_tmp538;});break;case 144: _LL290:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp53A;(_tmp53A.YY34).tag=
38;(_tmp53A.YY34).f1=({struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*_tmp53B));
_tmp53B->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp53B->tl=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53B;});_tmp53A;});
break;case 145: _LL291: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53C;(_tmp53C.YY34).tag=38;(_tmp53C.YY34).f1=({struct Cyc_List_List*_tmp53D=
_cycalloc(sizeof(*_tmp53D));_tmp53D->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp53D->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53D;});_tmp53C;});
break;case 146: _LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53E;(_tmp53E.YY32).tag=36;(_tmp53E.YY32).f1=(void*)((void*)2);_tmp53E;});
break;case 147: _LL293: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53F;(_tmp53F.YY32).tag=36;(_tmp53F.YY32).f1=(void*)((void*)3);_tmp53F;});
break;case 148: _LL294: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp540;(_tmp540.YY32).tag=36;(_tmp540.YY32).f1=(void*)((void*)0);_tmp540;});
break;case 149: _LL295: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp541;(_tmp541.YY33).tag=37;(_tmp541.YY33).f1=({struct Cyc_Absyn_Tunionfield*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp542->typs=0;_tmp542->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp542->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp542;});_tmp541;});
break;case 150: _LL296: {struct Cyc_List_List*_tmp543=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp544;(_tmp544.YY33).tag=37;(_tmp544.YY33).f1=({struct
Cyc_Absyn_Tunionfield*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp545->typs=
_tmp543;_tmp545->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp545->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp545;});_tmp544;});break;}case 151: _LL297: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 152: _LL298: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp546;(_tmp546.YY29).tag=33;(
_tmp546.YY29).f1=({struct Cyc_Parse_Declarator*_tmp547=_cycalloc(sizeof(*_tmp547));
_tmp547->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp547->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp547;});_tmp546;});
break;case 153: _LL299: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 154: _LL29A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp548;(_tmp548.YY29).tag=33;(_tmp548.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp549->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp549;});
_tmp548;});break;case 155: _LL29B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp54A;(_tmp54A.YY29).tag=33;(_tmp54A.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54B->tms=0;_tmp54B;});
_tmp54A;});break;case 156: _LL29C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 157: _LL29D: {struct Cyc_Parse_Declarator*_tmp54C=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54C->tms=({
struct Cyc_List_List*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));
_tmp54E[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp54F;_tmp54F.tag=5;_tmp54F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp54F.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54F;});_tmp54E;}));
_tmp54D->tl=_tmp54C->tms;_tmp54D;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 158: _LL29E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp550;(_tmp550.YY29).tag=33;(_tmp550.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp551->tms=({
struct Cyc_List_List*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp553=_cycalloc(sizeof(*_tmp553));
_tmp553[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp554;_tmp554.tag=0;_tmp554.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp554.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp554;});_tmp553;}));_tmp552->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp552;});
_tmp551;});_tmp550;});break;case 159: _LL29F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp555;(_tmp555.YY29).tag=33;(_tmp555.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp556->tms=({
struct Cyc_List_List*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp558=_cycalloc(sizeof(*_tmp558));
_tmp558[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp559;_tmp559.tag=1;_tmp559.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp559.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp559.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp559;});_tmp558;}));_tmp557->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp557;});
_tmp556;});_tmp555;});break;case 160: _LL2A0: {struct _tuple17 _tmp55B;struct Cyc_List_List*
_tmp55C;int _tmp55D;struct Cyc_Absyn_VarargInfo*_tmp55E;struct Cyc_Core_Opt*_tmp55F;
struct Cyc_List_List*_tmp560;struct _tuple17*_tmp55A=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55B=*_tmp55A;_tmp55C=
_tmp55B.f1;_tmp55D=_tmp55B.f2;_tmp55E=_tmp55B.f3;_tmp55F=_tmp55B.f4;_tmp560=
_tmp55B.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp561;(
_tmp561.YY29).tag=33;(_tmp561.YY29).f1=({struct Cyc_Parse_Declarator*_tmp562=
_cycalloc(sizeof(*_tmp562));_tmp562->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp562->tms=({
struct Cyc_List_List*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp564=_cycalloc(sizeof(*_tmp564));
_tmp564[0]=({struct Cyc_Absyn_Function_mod_struct _tmp565;_tmp565.tag=3;_tmp565.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp566=_cycalloc(sizeof(*
_tmp566));_tmp566[0]=({struct Cyc_Absyn_WithTypes_struct _tmp567;_tmp567.tag=1;
_tmp567.f1=_tmp55C;_tmp567.f2=_tmp55D;_tmp567.f3=_tmp55E;_tmp567.f4=_tmp55F;
_tmp567.f5=_tmp560;_tmp567;});_tmp566;}));_tmp565;});_tmp564;}));_tmp563->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp563;});_tmp562;});_tmp561;});break;}case 161: _LL2A1: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp568;(_tmp568.YY29).tag=33;(_tmp568.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp569->tms=({
struct Cyc_List_List*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));
_tmp56B[0]=({struct Cyc_Absyn_Function_mod_struct _tmp56C;_tmp56C.tag=3;_tmp56C.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp56D=_cycalloc(sizeof(*
_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_WithTypes_struct _tmp56E;_tmp56E.tag=1;
_tmp56E.f1=0;_tmp56E.f2=0;_tmp56E.f3=0;_tmp56E.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56E.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56E;});_tmp56D;}));
_tmp56C;});_tmp56B;}));_tmp56A->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp56A;});_tmp569;});
_tmp568;});break;case 162: _LL2A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56F;(_tmp56F.YY29).tag=33;(_tmp56F.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp570->tms=({
struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp572=_cycalloc(sizeof(*_tmp572));
_tmp572[0]=({struct Cyc_Absyn_Function_mod_struct _tmp573;_tmp573.tag=3;_tmp573.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp574=_cycalloc(sizeof(*_tmp574));
_tmp574[0]=({struct Cyc_Absyn_NoTypes_struct _tmp575;_tmp575.tag=0;_tmp575.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp575.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp575;});
_tmp574;}));_tmp573;});_tmp572;}));_tmp571->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp571;});_tmp570;});
_tmp56F;});break;case 163: _LL2A3: {struct Cyc_List_List*_tmp576=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp577;(_tmp577.YY29).tag=
33;(_tmp577.YY29).f1=({struct Cyc_Parse_Declarator*_tmp578=_cycalloc(sizeof(*
_tmp578));_tmp578->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp578->tms=({struct Cyc_List_List*_tmp579=_cycalloc(
sizeof(*_tmp579));_tmp579->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp57B;_tmp57B.tag=4;_tmp57B.f1=_tmp576;_tmp57B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57B.f3=0;
_tmp57B;});_tmp57A;}));_tmp579->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp579;});_tmp578;});
_tmp577;});break;}case 164: _LL2A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57C;(_tmp57C.YY29).tag=33;(_tmp57C.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp57D->tms=({
struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));
_tmp57F[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp580;_tmp580.tag=5;_tmp580.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp580.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp580;});_tmp57F;}));
_tmp57E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp57E;});_tmp57D;});_tmp57C;});break;case 165: _LL2A5:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp581;(_tmp581.YY29).tag=
33;(_tmp581.YY29).f1=({struct Cyc_Parse_Declarator*_tmp582=_cycalloc(sizeof(*
_tmp582));_tmp582->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp582->tms=0;_tmp582;});_tmp581;});break;case 166: _LL2A6:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp583;(_tmp583.YY29).tag=
33;(_tmp583.YY29).f1=({struct Cyc_Parse_Declarator*_tmp584=_cycalloc(sizeof(*
_tmp584));_tmp584->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp584->tms=0;_tmp584;});_tmp583;});break;case 167: _LL2A7:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 168:
_LL2A8: {struct Cyc_Parse_Declarator*_tmp585=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp585->tms=({struct Cyc_List_List*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp588;_tmp588.tag=5;_tmp588.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp588.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp588;});_tmp587;}));_tmp586->tl=_tmp585->tms;_tmp586;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 169: _LL2A9:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp589;(_tmp589.YY29).tag=
33;(_tmp589.YY29).f1=({struct Cyc_Parse_Declarator*_tmp58A=_cycalloc(sizeof(*
_tmp58A));_tmp58A->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp58A->tms=({struct Cyc_List_List*_tmp58B=_cycalloc(
sizeof(*_tmp58B));_tmp58B->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp58D;_tmp58D.tag=0;_tmp58D.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58D.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp58D;});
_tmp58C;}));_tmp58B->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp58B;});_tmp58A;});_tmp589;});break;case 170:
_LL2AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp58E;(_tmp58E.YY29).tag=
33;(_tmp58E.YY29).f1=({struct Cyc_Parse_Declarator*_tmp58F=_cycalloc(sizeof(*
_tmp58F));_tmp58F->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp58F->tms=({struct Cyc_List_List*_tmp590=_cycalloc(
sizeof(*_tmp590));_tmp590->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp592;_tmp592.tag=1;_tmp592.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp592.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp592.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp592;});
_tmp591;}));_tmp590->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp590;});_tmp58F;});_tmp58E;});break;case 171:
_LL2AB: {struct _tuple17 _tmp594;struct Cyc_List_List*_tmp595;int _tmp596;struct Cyc_Absyn_VarargInfo*
_tmp597;struct Cyc_Core_Opt*_tmp598;struct Cyc_List_List*_tmp599;struct _tuple17*
_tmp593=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp594=*_tmp593;_tmp595=_tmp594.f1;_tmp596=_tmp594.f2;_tmp597=_tmp594.f3;
_tmp598=_tmp594.f4;_tmp599=_tmp594.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp59A;(_tmp59A.YY29).tag=33;(_tmp59A.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp59B->tms=({
struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));
_tmp59D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59E;_tmp59E.tag=3;_tmp59E.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp59F=_cycalloc(sizeof(*
_tmp59F));_tmp59F[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A0;_tmp5A0.tag=1;
_tmp5A0.f1=_tmp595;_tmp5A0.f2=_tmp596;_tmp5A0.f3=_tmp597;_tmp5A0.f4=_tmp598;
_tmp5A0.f5=_tmp599;_tmp5A0;});_tmp59F;}));_tmp59E;});_tmp59D;}));_tmp59C->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp59C;});_tmp59B;});_tmp59A;});break;}case 172: _LL2AC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5A1;(_tmp5A1.YY29).tag=33;(_tmp5A1.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5A2->tms=({
struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));
_tmp5A4[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5A5;_tmp5A5.tag=3;_tmp5A5.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5A6=_cycalloc(sizeof(*
_tmp5A6));_tmp5A6[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A7;_tmp5A7.tag=1;
_tmp5A7.f1=0;_tmp5A7.f2=0;_tmp5A7.f3=0;_tmp5A7.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A7.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A7;});_tmp5A6;}));
_tmp5A5;});_tmp5A4;}));_tmp5A3->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5A3;});_tmp5A2;});
_tmp5A1;});break;case 173: _LL2AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A8;(_tmp5A8.YY29).tag=33;(_tmp5A8.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5A9->tms=({
struct Cyc_List_List*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));
_tmp5AB[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5AC;_tmp5AC.tag=3;_tmp5AC.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));
_tmp5AD[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5AE;_tmp5AE.tag=0;_tmp5AE.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AE.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AE;});
_tmp5AD;}));_tmp5AC;});_tmp5AB;}));_tmp5AA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5AA;});_tmp5A9;});
_tmp5A8;});break;case 174: _LL2AE: {struct Cyc_List_List*_tmp5AF=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B0;(_tmp5B0.YY29).tag=
33;(_tmp5B0.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B1=_cycalloc(sizeof(*
_tmp5B1));_tmp5B1->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5B1->tms=({struct Cyc_List_List*_tmp5B2=_cycalloc(
sizeof(*_tmp5B2));_tmp5B2->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5B4;_tmp5B4.tag=4;_tmp5B4.f1=_tmp5AF;_tmp5B4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B4.f3=0;
_tmp5B4;});_tmp5B3;}));_tmp5B2->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B2;});_tmp5B1;});
_tmp5B0;});break;}case 175: _LL2AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B5;(_tmp5B5.YY29).tag=33;(_tmp5B5.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5B6->tms=({
struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));
_tmp5B8[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5B9;_tmp5B9.tag=5;_tmp5B9.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5B9.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B9;});_tmp5B8;}));
_tmp5B7->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5B7;});_tmp5B6;});_tmp5B5;});break;case 176: _LL2B0:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 177:
_LL2B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5BA;(_tmp5BA.YY28).tag=
32;(_tmp5BA.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5BA;});break;case 178:
_LL2B2: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5BD;_tmp5BD.tag=5;_tmp5BD.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5BD.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5BD;});_tmp5BC;}));_tmp5BB->tl=ans;_tmp5BB;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5BE=_cycalloc(
sizeof(*_tmp5BE));_tmp5BE->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5BE->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5BE->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5BE;});ans=({struct Cyc_List_List*_tmp5BF=
_cycalloc(sizeof(*_tmp5BF));_tmp5BF->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5C1;_tmp5C1.tag=2;_tmp5C1.f1=({struct Cyc_Absyn_PtrAtts _tmp5C2;_tmp5C2.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5C2.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5C2.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5C2.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5C2.ptrloc=ptrloc;_tmp5C2;});_tmp5C1.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C1;});_tmp5C0;}));
_tmp5BF->tl=ans;_tmp5BF;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C3;(_tmp5C3.YY28).tag=32;(_tmp5C3.YY28).f1=ans;_tmp5C3;});break;}}case 179:
_LL2B3: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C4;(_tmp5C4.YY1).tag=5;(
_tmp5C4.YY1).f1=({struct _tuple12*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->f1=
loc;_tmp5C5->f2=Cyc_Absyn_true_conref;_tmp5C5->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C5;});_tmp5C4;});
break;}case 180: _LL2B4: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C6;(_tmp5C6.YY1).tag=5;(_tmp5C6.YY1).f1=({struct _tuple12*_tmp5C7=_cycalloc(
sizeof(*_tmp5C7));_tmp5C7->f1=loc;_tmp5C7->f2=Cyc_Absyn_false_conref;_tmp5C7->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C7;});
_tmp5C6;});break;}case 181: _LL2B5: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C8;(_tmp5C8.YY1).tag=5;(_tmp5C8.YY1).f1=({struct _tuple12*_tmp5C9=_cycalloc(
sizeof(*_tmp5C9));_tmp5C9->f1=loc;_tmp5C9->f2=Cyc_Absyn_true_conref;_tmp5C9->f3=
Cyc_Absyn_bounds_dynforward_conref;_tmp5C9;});_tmp5C8;});break;}case 182: _LL2B6: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5CA;(_tmp5CA.YY1).tag=5;(
_tmp5CA.YY1).f1=({struct _tuple12*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->f1=
loc;_tmp5CB->f2=Cyc_Absyn_true_conref;_tmp5CB->f3=Cyc_Absyn_bounds_dyneither_conref;
_tmp5CB;});_tmp5CA;});break;}case 183: _LL2B7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5CC;(_tmp5CC.YY2).tag=6;(_tmp5CC.YY2).f1=Cyc_Absyn_bounds_one_conref;
_tmp5CC;});break;case 184: _LL2B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5CD;(_tmp5CD.YY2).tag=6;(_tmp5CD.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp5CF;_tmp5CF.tag=0;_tmp5CF.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CF;});_tmp5CE;}));
_tmp5CD;});break;case 185: _LL2B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D0;(_tmp5D0.YY2).tag=6;(_tmp5D0.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_AbsUpper_b_struct*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1[0]=({
struct Cyc_Absyn_AbsUpper_b_struct _tmp5D2;_tmp5D2.tag=1;_tmp5D2.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D2;});_tmp5D1;}));
_tmp5D0;});break;case 186: _LL2BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D3;(_tmp5D3.YY51).tag=55;(_tmp5D3.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5D3;});break;case 187: _LL2BB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D4;(_tmp5D4.YY51).tag=55;(_tmp5D4.YY51).f1=Cyc_Absyn_true_conref;
_tmp5D4;});break;case 188: _LL2BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D5;(_tmp5D5.YY51).tag=55;(_tmp5D5.YY51).f1=Cyc_Absyn_false_conref;_tmp5D5;});
break;case 189: _LL2BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D6;(_tmp5D6.YY49).tag=53;(_tmp5D6.YY49).f1=0;_tmp5D6;});break;case 190:
_LL2BE: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D7;(_tmp5D7.YY49).tag=53;(_tmp5D7.YY49).f1=({struct
Cyc_Core_Opt*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D8;});_tmp5D7;});
break;case 191: _LL2BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D9;(_tmp5D9.YY49).tag=53;(_tmp5D9.YY49).f1=({struct Cyc_Core_Opt*_tmp5DA=
_cycalloc(sizeof(*_tmp5DA));_tmp5DA->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->v=(void*)((void*)3);_tmp5DB;}),0);
_tmp5DA;});_tmp5D9;});break;case 192: _LL2C0: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp5DC;(_tmp5DC.YY44).tag=48;(_tmp5DC.YY44).f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->v=(void*)((void*)
5);_tmp5DD;}),0);_tmp5DC;});break;case 193: _LL2C1: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 194: _LL2C2: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5DE;(_tmp5DE.YY44).tag=48;(
_tmp5DE.YY44).f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5DF=
_cycalloc(sizeof(*_tmp5DF));_tmp5DF->v=(void*)((void*)5);_tmp5DF;}),0);_tmp5DE;});
break;case 195: _LL2C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E0;(_tmp5E0.YY25).tag=29;(_tmp5E0.YY25).f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp5E0;});
break;case 196: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E1;(_tmp5E1.YY25).tag=29;(_tmp5E1.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5E1;});break;case 197:
_LL2C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E2;(_tmp5E2.YY39).tag=
43;(_tmp5E2.YY39).f1=({struct _tuple17*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));
_tmp5E3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp5E3->f2=0;_tmp5E3->f3=0;_tmp5E3->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E3;});_tmp5E2;});
break;case 198: _LL2C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E4;(_tmp5E4.YY39).tag=43;(_tmp5E4.YY39).f1=({struct _tuple17*_tmp5E5=
_cycalloc(sizeof(*_tmp5E5));_tmp5E5->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp5E5->f2=1;_tmp5E5->f3=0;_tmp5E5->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E5->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E5;});_tmp5E4;});
break;case 199: _LL2C7: {struct _tuple2 _tmp5E7;struct Cyc_Core_Opt*_tmp5E8;struct Cyc_Absyn_Tqual
_tmp5E9;void*_tmp5EA;struct _tuple2*_tmp5E6=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E7=*_tmp5E6;_tmp5E8=
_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;_tmp5EA=_tmp5E7.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5EB=({struct Cyc_Absyn_VarargInfo*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->name=
_tmp5E8;_tmp5EE->tq=_tmp5E9;_tmp5EE->type=(void*)_tmp5EA;_tmp5EE->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5EE;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5EC;(_tmp5EC.YY39).tag=43;(
_tmp5EC.YY39).f1=({struct _tuple17*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->f1=
0;_tmp5ED->f2=0;_tmp5ED->f3=_tmp5EB;_tmp5ED->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5ED->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5ED;});_tmp5EC;});
break;}}case 200: _LL2C8: {struct _tuple2 _tmp5F0;struct Cyc_Core_Opt*_tmp5F1;struct
Cyc_Absyn_Tqual _tmp5F2;void*_tmp5F3;struct _tuple2*_tmp5EF=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5F0=*_tmp5EF;_tmp5F1=
_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;_tmp5F3=_tmp5F0.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5F4=({struct Cyc_Absyn_VarargInfo*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->name=
_tmp5F1;_tmp5F7->tq=_tmp5F2;_tmp5F7->type=(void*)_tmp5F3;_tmp5F7->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5F7;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F5;(_tmp5F5.YY39).tag=43;(
_tmp5F5.YY39).f1=({struct _tuple17*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5F6->f2=0;
_tmp5F6->f3=_tmp5F4;_tmp5F6->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F6->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F6;});_tmp5F5;});
break;}}case 201: _LL2C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F8;(_tmp5F8.YY44).tag=48;(_tmp5F8.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5FA;_tmp5FA.tag=1;_tmp5FA.f1=0;_tmp5FA;});_tmp5F9;}));_tmp5F8;});break;case
202: _LL2CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FB;(
_tmp5FB.YY44).tag=48;(_tmp5FB.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5FD;_tmp5FD.tag=0;_tmp5FD.f1=(void*)Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FD;});_tmp5FC;}));
_tmp5FB;});break;case 203: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FE;(_tmp5FE.YY49).tag=53;(_tmp5FE.YY49).f1=0;_tmp5FE;});break;case 204:
_LL2CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FF;(_tmp5FF.YY49).tag=
53;(_tmp5FF.YY49).f1=({struct Cyc_Core_Opt*_tmp600=_cycalloc(sizeof(*_tmp600));
_tmp600->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp601=_cycalloc(
sizeof(*_tmp601));_tmp601[0]=({struct Cyc_Absyn_JoinEff_struct _tmp602;_tmp602.tag=
21;_tmp602.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp602;});_tmp601;}));_tmp600;});_tmp5FF;});break;case 205:
_LL2CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp603;(_tmp603.YY50).tag=
54;(_tmp603.YY50).f1=0;_tmp603;});break;case 206: _LL2CE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 207: _LL2CF: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp604;(_tmp604.YY50).tag=54;(
_tmp604.YY50).f1=({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));
_tmp605->hd=({struct _tuple4*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->f1=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp609=_cycalloc(sizeof(*_tmp609));
_tmp609[0]=({struct Cyc_Absyn_JoinEff_struct _tmp60A;_tmp60A.tag=21;_tmp60A.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60A;});_tmp609;});
_tmp606->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp608;_tmp608.tag=0;_tmp608.f1=(void*)((void*)3);_tmp608;});_tmp607;}));
_tmp606;});_tmp605->tl=0;_tmp605;});_tmp604;});break;case 208: _LL2D0: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp60B;(_tmp60B.YY50).tag=54;(_tmp60B.YY50).f1=({
struct Cyc_List_List*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=({struct
_tuple4*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp611;_tmp611.tag=21;_tmp611.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp611;});_tmp610;});
_tmp60D->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp60F;_tmp60F.tag=0;_tmp60F.f1=(void*)((void*)3);_tmp60F;});_tmp60E;}));
_tmp60D;});_tmp60C->tl=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp60C;});_tmp60B;});break;case 209: _LL2D1: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp612;(_tmp612.YY31).tag=35;(_tmp612.YY31).f1=0;_tmp612;});
break;case 210: _LL2D2: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp613="inject";
_tag_dynforward(_tmp613,sizeof(char),_get_zero_arr_size(_tmp613,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp614="missing type in function declaration";_tag_dynforward(_tmp614,
sizeof(char),_get_zero_arr_size(_tmp614,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp615;(_tmp615.YY31).tag=35;(_tmp615.YY31).f1=1;_tmp615;});
break;case 211: _LL2D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 212: _LL2D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp616;(_tmp616.YY40).tag=44;(_tmp616.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp616;});break;case 213:
_LL2D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp617;(_tmp617.YY40).tag=
44;(_tmp617.YY40).f1=0;_tmp617;});break;case 214: _LL2D6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 215: _LL2D7:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp618;(_tmp618.YY40).tag=
44;(_tmp618.YY40).f1=({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));
_tmp619->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp61A=_cycalloc(
sizeof(*_tmp61A));_tmp61A[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp61B;_tmp61B.tag=
22;_tmp61B.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp61B;});_tmp61A;}));_tmp619->tl=0;_tmp619;});_tmp618;});
break;case 216: _LL2D8: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp61C;(_tmp61C.YY40).tag=44;(_tmp61C.YY40).f1=({struct
Cyc_List_List*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61D->tl=0;_tmp61D;});
_tmp61C;});break;case 217: _LL2D9: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp61E;(_tmp61E.YY40).tag=44;(_tmp61E.YY40).f1=({struct
Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp621;_tmp621.tag=20;_tmp621.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp621;});_tmp620;}));
_tmp61F->tl=0;_tmp61F;});_tmp61E;});break;case 218: _LL2DA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)3,0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp622;(
_tmp622.YY40).tag=44;(_tmp622.YY40).f1=({struct Cyc_List_List*_tmp623=_cycalloc(
sizeof(*_tmp623));_tmp623->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp625;_tmp625.tag=20;_tmp625.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp625;});_tmp624;}));
_tmp623->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp623;});_tmp622;});break;case 219: _LL2DB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp626;(_tmp626.YY38).tag=42;(_tmp626.YY38).f1=({struct
Cyc_List_List*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp627->tl=0;_tmp627;});
_tmp626;});break;case 220: _LL2DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp628;(_tmp628.YY38).tag=42;(_tmp628.YY38).f1=({struct Cyc_List_List*_tmp629=
_cycalloc(sizeof(*_tmp629));_tmp629->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp629->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp629;});_tmp628;});
break;case 221: _LL2DD: {struct _tuple16 _tmp62B;struct Cyc_Absyn_Tqual _tmp62C;struct
Cyc_List_List*_tmp62D;struct Cyc_List_List*_tmp62E;struct _tuple16*_tmp62A=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62B=*_tmp62A;
_tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;_tmp62E=_tmp62B.f3;if(_tmp62C.loc == 0)
_tmp62C.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp630;struct _tuple1*
_tmp631;struct Cyc_List_List*_tmp632;struct Cyc_Parse_Declarator*_tmp62F=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp630=*_tmp62F;
_tmp631=_tmp630.id;_tmp632=_tmp630.tms;{void*_tmp633=Cyc_Parse_speclist2typ(
_tmp62D,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp635;void*_tmp636;struct
Cyc_List_List*_tmp637;struct Cyc_List_List*_tmp638;struct _tuple6 _tmp634=Cyc_Parse_apply_tms(
_tmp62C,_tmp633,_tmp62E,_tmp632);_tmp635=_tmp634.f1;_tmp636=_tmp634.f2;_tmp637=
_tmp634.f3;_tmp638=_tmp634.f4;if(_tmp637 != 0)Cyc_Parse_err(({const char*_tmp639="parameter with bad type params";
_tag_dynforward(_tmp639,sizeof(char),_get_zero_arr_size(_tmp639,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp631))Cyc_Parse_err(({const char*_tmp63A="parameter cannot be qualified with a namespace";
_tag_dynforward(_tmp63A,sizeof(char),_get_zero_arr_size(_tmp63A,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp63B=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp640=_cycalloc(sizeof(*
_tmp640));_tmp640->v=(*_tmp631).f2;_tmp640;});if(_tmp638 != 0)({void*_tmp63C[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp63D="extra attributes on parameter, ignoring";
_tag_dynforward(_tmp63D,sizeof(char),_get_zero_arr_size(_tmp63D,40));}),
_tag_dynforward(_tmp63C,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp63E;(_tmp63E.YY37).tag=41;(_tmp63E.YY37).f1=({struct
_tuple2*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->f1=_tmp63B;_tmp63F->f2=
_tmp635;_tmp63F->f3=_tmp636;_tmp63F;});_tmp63E;});break;}}}}case 222: _LL2DE: {
struct _tuple16 _tmp642;struct Cyc_Absyn_Tqual _tmp643;struct Cyc_List_List*_tmp644;
struct Cyc_List_List*_tmp645;struct _tuple16*_tmp641=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp642=*_tmp641;_tmp643=
_tmp642.f1;_tmp644=_tmp642.f2;_tmp645=_tmp642.f3;if(_tmp643.loc == 0)_tmp643.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp646=Cyc_Parse_speclist2typ(_tmp644,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp645 != 0)({
void*_tmp647[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp648="bad attributes on parameter, ignoring";_tag_dynforward(_tmp648,sizeof(
char),_get_zero_arr_size(_tmp648,38));}),_tag_dynforward(_tmp647,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp649;(_tmp649.YY37).tag=
41;(_tmp649.YY37).f1=({struct _tuple2*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->f1=
0;_tmp64A->f2=_tmp643;_tmp64A->f3=_tmp646;_tmp64A;});_tmp649;});break;}}case 223:
_LL2DF: {struct _tuple16 _tmp64C;struct Cyc_Absyn_Tqual _tmp64D;struct Cyc_List_List*
_tmp64E;struct Cyc_List_List*_tmp64F;struct _tuple16*_tmp64B=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64C=*_tmp64B;_tmp64D=
_tmp64C.f1;_tmp64E=_tmp64C.f2;_tmp64F=_tmp64C.f3;if(_tmp64D.loc == 0)_tmp64D.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp650=Cyc_Parse_speclist2typ(_tmp64E,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp651=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp653;void*_tmp654;struct Cyc_List_List*_tmp655;struct Cyc_List_List*
_tmp656;struct _tuple6 _tmp652=Cyc_Parse_apply_tms(_tmp64D,_tmp650,_tmp64F,_tmp651);
_tmp653=_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;_tmp656=_tmp652.f4;if(
_tmp655 != 0)({void*_tmp657[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp658="bad type parameters on formal argument, ignoring";_tag_dynforward(
_tmp658,sizeof(char),_get_zero_arr_size(_tmp658,49));}),_tag_dynforward(_tmp657,
sizeof(void*),0));});if(_tmp656 != 0)({void*_tmp659[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp65A="bad attributes on parameter, ignoring";_tag_dynforward(_tmp65A,sizeof(
char),_get_zero_arr_size(_tmp65A,38));}),_tag_dynforward(_tmp659,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp65B;(_tmp65B.YY37).tag=
41;(_tmp65B.YY37).f1=({struct _tuple2*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->f1=
0;_tmp65C->f2=_tmp653;_tmp65C->f3=_tmp654;_tmp65C;});_tmp65B;});break;}}case 224:
_LL2E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp65D;(_tmp65D.YY36).tag=
40;(_tmp65D.YY36).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp65D;});break;case 225: _LL2E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp65E;(_tmp65E.YY36).tag=40;(_tmp65E.YY36).f1=({struct Cyc_List_List*_tmp65F=
_cycalloc(sizeof(*_tmp65F));_tmp65F->hd=({struct _dynforward_ptr*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp660;});_tmp65F->tl=0;
_tmp65F;});_tmp65E;});break;case 226: _LL2E2: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp661;(_tmp661.YY36).tag=40;(_tmp661.YY36).f1=({struct Cyc_List_List*
_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->hd=({struct _dynforward_ptr*_tmp663=
_cycalloc(sizeof(*_tmp663));_tmp663[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp663;});_tmp662->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp662;});_tmp661;});
break;case 227: _LL2E3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 228: _LL2E4: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 229: _LL2E5: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp664;(_tmp664.YY3).tag=7;(_tmp664.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp665=_cycalloc(sizeof(*_tmp665));
_tmp665[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp666;_tmp666.tag=37;
_tmp666.f1=0;_tmp666.f2=0;_tmp666;});_tmp665;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp664;});
break;case 230: _LL2E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp667;(_tmp667.YY3).tag=7;(_tmp667.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp669;_tmp669.tag=37;_tmp669.f1=0;
_tmp669.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp669;});_tmp668;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp667;});
break;case 231: _LL2E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp66A;(_tmp66A.YY3).tag=7;(_tmp66A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp66C;_tmp66C.tag=37;_tmp66C.f1=0;
_tmp66C.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp66C;});_tmp66B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp66A;});
break;case 232: _LL2E8: {struct Cyc_Absyn_Vardecl*_tmp66D=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp673;(_tmp673.Loc_n).tag=0;_tmp673;});_tmp671->f2=({
struct _dynforward_ptr*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp672;});_tmp671;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp66D->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp66E;(_tmp66E.YY3).tag=
7;(_tmp66E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp670;_tmp670.tag=29;_tmp670.f1=_tmp66D;_tmp670.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp670.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp670.f4=0;
_tmp670;});_tmp66F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp66E;});
break;}case 233: _LL2E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp674;(_tmp674.YY6).tag=10;(_tmp674.YY6).f1=({struct Cyc_List_List*_tmp675=
_cycalloc(sizeof(*_tmp675));_tmp675->hd=({struct _tuple20*_tmp676=_cycalloc(
sizeof(*_tmp676));_tmp676->f1=0;_tmp676->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp676;});_tmp675->tl=0;
_tmp675;});_tmp674;});break;case 234: _LL2EA: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp677;(_tmp677.YY6).tag=10;(_tmp677.YY6).f1=({struct Cyc_List_List*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->hd=({struct _tuple20*_tmp679=
_cycalloc(sizeof(*_tmp679));_tmp679->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp679->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp679;});_tmp678->tl=
0;_tmp678;});_tmp677;});break;case 235: _LL2EB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp67A;(_tmp67A.YY6).tag=10;(_tmp67A.YY6).f1=({struct Cyc_List_List*
_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->hd=({struct _tuple20*_tmp67C=
_cycalloc(sizeof(*_tmp67C));_tmp67C->f1=0;_tmp67C->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67C;});_tmp67B->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp67B;});_tmp67A;});
break;case 236: _LL2EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp67D;(_tmp67D.YY6).tag=10;(_tmp67D.YY6).f1=({struct Cyc_List_List*_tmp67E=
_cycalloc(sizeof(*_tmp67E));_tmp67E->hd=({struct _tuple20*_tmp67F=_cycalloc(
sizeof(*_tmp67F));_tmp67F->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp67F->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67F;});_tmp67E->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp67E;});_tmp67D;});
break;case 237: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp680;(_tmp680.YY41).tag=45;(_tmp680.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp680;});break;
case 238: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp681;(
_tmp681.YY41).tag=45;(_tmp681.YY41).f1=({struct Cyc_List_List*_tmp682=_cycalloc(
sizeof(*_tmp682));_tmp682->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp682->tl=0;_tmp682;});
_tmp681;});break;case 239: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp683;(_tmp683.YY41).tag=45;(_tmp683.YY41).f1=({struct Cyc_List_List*_tmp684=
_cycalloc(sizeof(*_tmp684));_tmp684->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp684->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp684;});_tmp683;});
break;case 240: _LL2F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp685;(_tmp685.YY42).tag=46;(_tmp685.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp687;_tmp687.tag=0;_tmp687.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp687;});_tmp686;}));
_tmp685;});break;case 241: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp688;(_tmp688.YY42).tag=46;(_tmp688.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Absyn_FieldName_struct
_tmp68A;_tmp68A.tag=1;_tmp68A.f1=({struct _dynforward_ptr*_tmp68B=_cycalloc(
sizeof(*_tmp68B));_tmp68B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68B;});_tmp68A;});
_tmp689;}));_tmp688;});break;case 242: _LL2F2: {struct _tuple16 _tmp68D;struct Cyc_Absyn_Tqual
_tmp68E;struct Cyc_List_List*_tmp68F;struct Cyc_List_List*_tmp690;struct _tuple16*
_tmp68C=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp68D=*_tmp68C;_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;_tmp690=_tmp68D.f3;{void*
_tmp691=Cyc_Parse_speclist2typ(_tmp68F,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp690 != 0)({
void*_tmp692[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp693="ignoring attributes in type";_tag_dynforward(_tmp693,sizeof(char),
_get_zero_arr_size(_tmp693,28));}),_tag_dynforward(_tmp692,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp694;(_tmp694.YY37).tag=
41;(_tmp694.YY37).f1=({struct _tuple2*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->f1=
0;_tmp695->f2=_tmp68E;_tmp695->f3=_tmp691;_tmp695;});_tmp694;});break;}}case 243:
_LL2F3: {struct _tuple16 _tmp697;struct Cyc_Absyn_Tqual _tmp698;struct Cyc_List_List*
_tmp699;struct Cyc_List_List*_tmp69A;struct _tuple16*_tmp696=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp697=*_tmp696;_tmp698=
_tmp697.f1;_tmp699=_tmp697.f2;_tmp69A=_tmp697.f3;{void*_tmp69B=Cyc_Parse_speclist2typ(
_tmp699,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp69C=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp69D=
Cyc_Parse_apply_tms(_tmp698,_tmp69B,_tmp69A,_tmp69C);if(_tmp69D.f3 != 0)({void*
_tmp69E[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp69F="bad type params, ignoring";_tag_dynforward(_tmp69F,sizeof(char),
_get_zero_arr_size(_tmp69F,26));}),_tag_dynforward(_tmp69E,sizeof(void*),0));});
if(_tmp69D.f4 != 0)({void*_tmp6A0[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6A1="bad specifiers, ignoring";_tag_dynforward(_tmp6A1,sizeof(char),
_get_zero_arr_size(_tmp6A1,25));}),_tag_dynforward(_tmp6A0,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A2;(_tmp6A2.YY37).tag=
41;(_tmp6A2.YY37).f1=({struct _tuple2*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->f1=
0;_tmp6A3->f2=_tmp69D.f1;_tmp6A3->f3=_tmp69D.f2;_tmp6A3;});_tmp6A2;});break;}}
case 244: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A4;(
_tmp6A4.YY44).tag=48;(_tmp6A4.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6A4;});break;case
245: _LL2F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A5;(
_tmp6A5.YY44).tag=48;(_tmp6A5.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6A7;_tmp6A7.tag=21;_tmp6A7.f1=0;_tmp6A7;});_tmp6A6;}));_tmp6A5;});break;case
246: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A8;(
_tmp6A8.YY44).tag=48;(_tmp6A8.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6AA;_tmp6AA.tag=21;_tmp6AA.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AA;});_tmp6A9;}));
_tmp6A8;});break;case 247: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AB;(_tmp6AB.YY44).tag=48;(_tmp6AB.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6AD;_tmp6AD.tag=22;_tmp6AD.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD;});_tmp6AC;}));
_tmp6AB;});break;case 248: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AE;(_tmp6AE.YY44).tag=48;(_tmp6AE.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6B0;_tmp6B0.tag=21;_tmp6B0.f1=({struct Cyc_List_List*_tmp6B1=_cycalloc(
sizeof(*_tmp6B1));_tmp6B1->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B1->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B1;});_tmp6B0;});
_tmp6AF;}));_tmp6AE;});break;case 249: _LL2F9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6B2;(_tmp6B2.YY40).tag=44;(_tmp6B2.YY40).f1=({struct
Cyc_List_List*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B3->tl=0;_tmp6B3;});
_tmp6B2;});break;case 250: _LL2FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B4;(_tmp6B4.YY40).tag=44;(_tmp6B4.YY40).f1=({struct Cyc_List_List*_tmp6B5=
_cycalloc(sizeof(*_tmp6B5));_tmp6B5->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B5->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B5;});_tmp6B4;});
break;case 251: _LL2FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B6;(_tmp6B6.YY30).tag=34;(_tmp6B6.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B7;});_tmp6B6;});
break;case 252: _LL2FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 253: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B8;(_tmp6B8.YY30).tag=34;(_tmp6B8.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6B9;});
_tmp6B8;});break;case 254: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 255: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6BA;(_tmp6BA.YY30).tag=34;(_tmp6BA.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->tms=({struct Cyc_List_List*_tmp6BC=
_cycalloc(sizeof(*_tmp6BC));_tmp6BC->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6BE;_tmp6BE.tag=0;_tmp6BE.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BE.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6BE;});
_tmp6BD;}));_tmp6BC->tl=0;_tmp6BC;});_tmp6BB;});_tmp6BA;});break;case 256: _LL300:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6BF;(_tmp6BF.YY30).tag=
34;(_tmp6BF.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C0=_cycalloc(
sizeof(*_tmp6C0));_tmp6C0->tms=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*
_tmp6C1));_tmp6C1->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6C2=
_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C3;_tmp6C3.tag=0;_tmp6C3.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C3.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6C3;});
_tmp6C2;}));_tmp6C1->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6C1;});_tmp6C0;});_tmp6BF;});break;case 257:
_LL301: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C4;(_tmp6C4.YY30).tag=
34;(_tmp6C4.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C5=_cycalloc(
sizeof(*_tmp6C5));_tmp6C5->tms=({struct Cyc_List_List*_tmp6C6=_cycalloc(sizeof(*
_tmp6C6));_tmp6C6->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6C8;_tmp6C8.tag=1;_tmp6C8.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C8.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C8.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6C8;});
_tmp6C7;}));_tmp6C6->tl=0;_tmp6C6;});_tmp6C5;});_tmp6C4;});break;case 258: _LL302:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C9;(_tmp6C9.YY30).tag=
34;(_tmp6C9.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6CA=_cycalloc(
sizeof(*_tmp6CA));_tmp6CA->tms=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*
_tmp6CB));_tmp6CB->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6CD;_tmp6CD.tag=1;_tmp6CD.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CD.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CD.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6CD;});
_tmp6CC;}));_tmp6CB->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6CB;});_tmp6CA;});_tmp6C9;});break;case 259:
_LL303: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6CE;(_tmp6CE.YY30).tag=
34;(_tmp6CE.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6CF=_cycalloc(
sizeof(*_tmp6CF));_tmp6CF->tms=({struct Cyc_List_List*_tmp6D0=_cycalloc(sizeof(*
_tmp6D0));_tmp6D0->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D2;_tmp6D2.tag=3;_tmp6D2.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D4;_tmp6D4.tag=1;_tmp6D4.f1=0;_tmp6D4.f2=0;_tmp6D4.f3=0;_tmp6D4.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D4.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D4;});_tmp6D3;}));
_tmp6D2;});_tmp6D1;}));_tmp6D0->tl=0;_tmp6D0;});_tmp6CF;});_tmp6CE;});break;case
260: _LL304: {struct _tuple17 _tmp6D6;struct Cyc_List_List*_tmp6D7;int _tmp6D8;struct
Cyc_Absyn_VarargInfo*_tmp6D9;struct Cyc_Core_Opt*_tmp6DA;struct Cyc_List_List*
_tmp6DB;struct _tuple17*_tmp6D5=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D6=*_tmp6D5;_tmp6D7=
_tmp6D6.f1;_tmp6D8=_tmp6D6.f2;_tmp6D9=_tmp6D6.f3;_tmp6DA=_tmp6D6.f4;_tmp6DB=
_tmp6D6.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6DC;(
_tmp6DC.YY30).tag=34;(_tmp6DC.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->tms=({struct Cyc_List_List*_tmp6DE=
_cycalloc(sizeof(*_tmp6DE));_tmp6DE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E0;_tmp6E0.tag=3;_tmp6E0.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E2;_tmp6E2.tag=1;_tmp6E2.f1=_tmp6D7;_tmp6E2.f2=_tmp6D8;_tmp6E2.f3=_tmp6D9;
_tmp6E2.f4=_tmp6DA;_tmp6E2.f5=_tmp6DB;_tmp6E2;});_tmp6E1;}));_tmp6E0;});_tmp6DF;}));
_tmp6DE->tl=0;_tmp6DE;});_tmp6DD;});_tmp6DC;});break;}case 261: _LL305: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6E3;(_tmp6E3.YY30).tag=34;(
_tmp6E3.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6E4=_cycalloc(sizeof(*
_tmp6E4));_tmp6E4->tms=({struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));
_tmp6E5->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6E6=
_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E7;_tmp6E7.tag=3;_tmp6E7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E9;_tmp6E9.tag=1;_tmp6E9.f1=0;_tmp6E9.f2=0;_tmp6E9.f3=0;_tmp6E9.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E9.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E9;});_tmp6E8;}));
_tmp6E7;});_tmp6E6;}));_tmp6E5->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6E5;});_tmp6E4;});
_tmp6E3;});break;case 262: _LL306: {struct _tuple17 _tmp6EB;struct Cyc_List_List*
_tmp6EC;int _tmp6ED;struct Cyc_Absyn_VarargInfo*_tmp6EE;struct Cyc_Core_Opt*_tmp6EF;
struct Cyc_List_List*_tmp6F0;struct _tuple17*_tmp6EA=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EB=*_tmp6EA;_tmp6EC=
_tmp6EB.f1;_tmp6ED=_tmp6EB.f2;_tmp6EE=_tmp6EB.f3;_tmp6EF=_tmp6EB.f4;_tmp6F0=
_tmp6EB.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F1;(
_tmp6F1.YY30).tag=34;(_tmp6F1.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->tms=({struct Cyc_List_List*_tmp6F3=
_cycalloc(sizeof(*_tmp6F3));_tmp6F3->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6F5;_tmp6F5.tag=3;_tmp6F5.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F7;_tmp6F7.tag=1;_tmp6F7.f1=_tmp6EC;_tmp6F7.f2=_tmp6ED;_tmp6F7.f3=_tmp6EE;
_tmp6F7.f4=_tmp6EF;_tmp6F7.f5=_tmp6F0;_tmp6F7;});_tmp6F6;}));_tmp6F5;});_tmp6F4;}));
_tmp6F3->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp6F3;});_tmp6F2;});_tmp6F1;});break;}case 263: _LL307: {
struct Cyc_List_List*_tmp6F8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F9;(_tmp6F9.YY30).tag=
34;(_tmp6F9.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6FA=_cycalloc(
sizeof(*_tmp6FA));_tmp6FA->tms=({struct Cyc_List_List*_tmp6FB=_cycalloc(sizeof(*
_tmp6FB));_tmp6FB->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6FD;_tmp6FD.tag=4;_tmp6FD.f1=_tmp6F8;_tmp6FD.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FD.f3=0;
_tmp6FD;});_tmp6FC;}));_tmp6FB->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6FB;});_tmp6FA;});
_tmp6F9;});break;}case 264: _LL308: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6FE;(_tmp6FE.YY30).tag=34;(_tmp6FE.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF->tms=({struct Cyc_List_List*_tmp700=
_cycalloc(sizeof(*_tmp700));_tmp700->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp702;_tmp702.tag=5;_tmp702.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp702.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp702;});_tmp701;}));
_tmp700->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp700;});_tmp6FF;});_tmp6FE;});break;case 265: _LL309:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 266:
_LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
267: _LL30B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 268: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 269: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 270: _LL30E: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 271: _LL30F: if(Cyc_zstrcmp((struct _dynforward_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp703="`H";_tag_dynforward(_tmp703,sizeof(char),_get_zero_arr_size(
_tmp703,3));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp704="`U";
_tag_dynforward(_tmp704,sizeof(char),_get_zero_arr_size(_tmp704,3));}))== 0)Cyc_Parse_err((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp707;_tmp707.tag=0;_tmp707.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp705[1]={&
_tmp707};Cyc_aprintf(({const char*_tmp706="bad occurrence of heap region %s";
_tag_dynforward(_tmp706,sizeof(char),_get_zero_arr_size(_tmp706,33));}),
_tag_dynforward(_tmp705,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->name=({
struct _dynforward_ptr*_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp715;});_tmp712->identity=
0;_tmp712->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp713=_cycalloc(
sizeof(*_tmp713));_tmp713[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp714;_tmp714.tag=0;
_tmp714.f1=(void*)((void*)3);_tmp714;});_tmp713;}));_tmp712;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({
struct Cyc_Absyn_VarType_struct _tmp711;_tmp711.tag=1;_tmp711.f1=tv;_tmp711;});
_tmp710;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp708;(
_tmp708.YY9).tag=13;(_tmp708.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Absyn_Region_s_struct
_tmp70A;_tmp70A.tag=15;_tmp70A.f1=tv;_tmp70A.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp70D;(_tmp70D.Loc_n).tag=0;_tmp70D;});_tmp70B->f2=({
struct _dynforward_ptr*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70C;});_tmp70B;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));
_tmp70E[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp70F;_tmp70F.tag=15;_tmp70F.f1=(
void*)t;_tmp70F;});_tmp70E;}),0);_tmp70A.f3=0;_tmp70A.f4=0;_tmp70A.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70A;});_tmp709;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp708;});break;}case 272: _LL310: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp716="H";_tag_dynforward(_tmp716,sizeof(
char),_get_zero_arr_size(_tmp716,2));}))== 0)Cyc_Parse_err(({const char*_tmp717="bad occurrence of heap region `H";
_tag_dynforward(_tmp717,sizeof(char),_get_zero_arr_size(_tmp717,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722->name=({
struct _dynforward_ptr*_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp728;_tmp728.tag=0;_tmp728.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp726[1]={&
_tmp728};Cyc_aprintf(({const char*_tmp727="`%s";_tag_dynforward(_tmp727,sizeof(
char),_get_zero_arr_size(_tmp727,4));}),_tag_dynforward(_tmp726,sizeof(void*),1));}});
_tmp725;});_tmp722->identity=0;_tmp722->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp724;_tmp724.tag=0;_tmp724.f1=(void*)((void*)3);_tmp724;});_tmp723;}));
_tmp722;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp720=_cycalloc(
sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Absyn_VarType_struct _tmp721;_tmp721.tag=
1;_tmp721.f1=tv;_tmp721;});_tmp720;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp718;(_tmp718.YY9).tag=13;(_tmp718.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719[0]=({
struct Cyc_Absyn_Region_s_struct _tmp71A;_tmp71A.tag=15;_tmp71A.f1=tv;_tmp71A.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp71B=_cycalloc(sizeof(*_tmp71B));
_tmp71B->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp71D;(
_tmp71D.Loc_n).tag=0;_tmp71D;});_tmp71B->f2=({struct _dynforward_ptr*_tmp71C=
_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71C;});_tmp71B;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp71E=_cycalloc(sizeof(*_tmp71E));
_tmp71E[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp71F;_tmp71F.tag=15;_tmp71F.f1=(
void*)t;_tmp71F;});_tmp71E;}),0);_tmp71A.f3=0;_tmp71A.f4=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71A.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71A;});_tmp719;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp718;});break;}case 273: _LL311: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 6)]),({const char*_tmp729="resetable";_tag_dynforward(_tmp729,
sizeof(char),_get_zero_arr_size(_tmp729,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp72A="expecting [resetable]";_tag_dynforward(_tmp72A,sizeof(char),
_get_zero_arr_size(_tmp72A,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp72B="`H";_tag_dynforward(_tmp72B,
sizeof(char),_get_zero_arr_size(_tmp72B,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp72C="`U";_tag_dynforward(_tmp72C,sizeof(char),
_get_zero_arr_size(_tmp72C,3));})))Cyc_Parse_err((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp72F;_tmp72F.tag=0;_tmp72F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{void*_tmp72D[1]={& _tmp72F};Cyc_aprintf(({const char*
_tmp72E="bad occurrence of heap region %s";_tag_dynforward(_tmp72E,sizeof(char),
_get_zero_arr_size(_tmp72E,33));}),_tag_dynforward(_tmp72D,sizeof(void*),1));}}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A->name=({struct _dynforward_ptr*
_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp73D;});_tmp73A->identity=
0;_tmp73A->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp73B=_cycalloc(
sizeof(*_tmp73B));_tmp73B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp73C;_tmp73C.tag=0;
_tmp73C.f1=(void*)((void*)3);_tmp73C;});_tmp73B;}));_tmp73A;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738[0]=({
struct Cyc_Absyn_VarType_struct _tmp739;_tmp739.tag=1;_tmp739.f1=tv;_tmp739;});
_tmp738;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp730;(
_tmp730.YY9).tag=13;(_tmp730.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731[0]=({struct Cyc_Absyn_Region_s_struct
_tmp732;_tmp732.tag=15;_tmp732.f1=tv;_tmp732.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp735;(_tmp735.Loc_n).tag=0;_tmp735;});_tmp733->f2=({
struct _dynforward_ptr*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp734;});_tmp733;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp736=_cycalloc(sizeof(*_tmp736));
_tmp736[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp737;_tmp737.tag=15;_tmp737.f1=(
void*)t;_tmp737;});_tmp736;}),0);_tmp732.f3=1;_tmp732.f4=0;_tmp732.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp732;});_tmp731;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp730;});break;}case 274: _LL312: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp73E="resetable";_tag_dynforward(_tmp73E,
sizeof(char),_get_zero_arr_size(_tmp73E,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp73F="expecting `resetable'";_tag_dynforward(_tmp73F,sizeof(char),
_get_zero_arr_size(_tmp73F,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp740="H";_tag_dynforward(_tmp740,sizeof(
char),_get_zero_arr_size(_tmp740,2));}))== 0)Cyc_Parse_err(({const char*_tmp741="bad occurrence of heap region `H";
_tag_dynforward(_tmp741,sizeof(char),_get_zero_arr_size(_tmp741,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->name=({
struct _dynforward_ptr*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp752;_tmp752.tag=0;_tmp752.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp750[1]={&
_tmp752};Cyc_aprintf(({const char*_tmp751="`%s";_tag_dynforward(_tmp751,sizeof(
char),_get_zero_arr_size(_tmp751,4));}),_tag_dynforward(_tmp750,sizeof(void*),1));}});
_tmp74F;});_tmp74C->identity=0;_tmp74C->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp74E;_tmp74E.tag=0;_tmp74E.f1=(void*)((void*)3);_tmp74E;});_tmp74D;}));
_tmp74C;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp74A=_cycalloc(
sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Absyn_VarType_struct _tmp74B;_tmp74B.tag=
1;_tmp74B.f1=tv;_tmp74B;});_tmp74A;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp742;(_tmp742.YY9).tag=13;(_tmp742.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=({
struct Cyc_Absyn_Region_s_struct _tmp744;_tmp744.tag=15;_tmp744.f1=tv;_tmp744.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp745=_cycalloc(sizeof(*_tmp745));
_tmp745->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp747;(
_tmp747.Loc_n).tag=0;_tmp747;});_tmp745->f2=({struct _dynforward_ptr*_tmp746=
_cycalloc(sizeof(*_tmp746));_tmp746[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp746;});_tmp745;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp748=_cycalloc(sizeof(*_tmp748));
_tmp748[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp749;_tmp749.tag=15;_tmp749.f1=(
void*)t;_tmp749;});_tmp748;}),0);_tmp744.f3=1;_tmp744.f4=0;_tmp744.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp744;});_tmp743;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp742;});break;}case 275: _LL313: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp753;(_tmp753.YY9).tag=13;(_tmp753.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp754=_cycalloc(sizeof(*_tmp754));
_tmp754[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp755;_tmp755.tag=16;_tmp755.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp755;});_tmp754;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp753;});
break;case 276: _LL314: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp756;(_tmp756.YY4).tag=8;(_tmp756.YY4).f1=0;_tmp756;});break;case 277: _LL315:
if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp757="open";
_tag_dynforward(_tmp757,sizeof(char),_get_zero_arr_size(_tmp757,5));}))!= 0)Cyc_Parse_err(({
const char*_tmp758="expecting `open'";_tag_dynforward(_tmp758,sizeof(char),
_get_zero_arr_size(_tmp758,17));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp759;(_tmp759.YY4).tag=8;(_tmp759.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp759;});break;case 278: _LL316: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75A;(_tmp75A.YY9).tag=13;(_tmp75A.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp75B=_cycalloc(sizeof(*_tmp75B));
_tmp75B[0]=({struct Cyc_Absyn_Label_s_struct _tmp75C;_tmp75C.tag=12;_tmp75C.f1=({
struct _dynforward_ptr*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75D;});_tmp75C.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75C;});
_tmp75B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75A;});break;case 279: _LL317: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75E;(_tmp75E.YY9).tag=13;(_tmp75E.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp75E;});break;case 280: _LL318: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75F;(_tmp75F.YY9).tag=13;(_tmp75F.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75F;});
break;case 281: _LL319: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp760;(_tmp760.YY9).tag=13;(_tmp760.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp760;});
break;case 282: _LL31A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 283: _LL31B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp761;(_tmp761.YY9).tag=13;(_tmp761.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp761;});break;case 284: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp762;(_tmp762.YY9).tag=13;(_tmp762.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp762;});break;case 285:
_LL31D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
286: _LL31E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp763;(
_tmp763.YY9).tag=13;(_tmp763.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp763;});
break;case 287: _LL31F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp764;(_tmp764.YY9).tag=13;(_tmp764.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp766;_tmp766.tag=1;_tmp766.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp766;});_tmp765;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp764;});break;case 288:
_LL320: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp767;(_tmp767.YY9).tag=
13;(_tmp767.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp769;_tmp769.tag=1;_tmp769.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp769;});_tmp768;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp767;});break;case 289: _LL321: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76A;(_tmp76A.YY9).tag=13;(_tmp76A.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp76A;});break;case 290: _LL322: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76B;(_tmp76B.YY9).tag=13;(_tmp76B.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76B;});
break;case 291: _LL323: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76C;(_tmp76C.YY9).tag=13;(_tmp76C.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76C;});
break;case 292: _LL324: {struct Cyc_Absyn_Exp*_tmp76D=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp770;_tmp770.tag=2;_tmp770.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp770;});_tmp76F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76E;(_tmp76E.YY9).tag=13;(_tmp76E.YY9).f1=Cyc_Absyn_switch_stmt(_tmp76D,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76E;});
break;}case 293: _LL325: {struct Cyc_Absyn_Exp*_tmp771=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp772;(_tmp772.YY9).tag=13;(_tmp772.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp771,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp772;});
break;}case 294: _LL326: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp773;(_tmp773.YY9).tag=13;(_tmp773.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp773;});
break;case 295: _LL327: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp774;(_tmp774.YY10).tag=14;(_tmp774.YY10).f1=0;_tmp774;});break;case 296:
_LL328: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp775;(_tmp775.YY10).tag=
14;(_tmp775.YY10).f1=({struct Cyc_List_List*_tmp776=_cycalloc(sizeof(*_tmp776));
_tmp776->hd=({struct Cyc_Absyn_Switch_clause*_tmp777=_cycalloc(sizeof(*_tmp777));
_tmp777->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp777->pat_vars=
0;_tmp777->where_clause=0;_tmp777->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp777->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp777;});
_tmp776->tl=0;_tmp776;});_tmp775;});break;case 297: _LL329: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp778;(_tmp778.YY10).tag=14;(_tmp778.YY10).f1=({struct
Cyc_List_List*_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779->hd=({struct Cyc_Absyn_Switch_clause*
_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp77A->pat_vars=0;
_tmp77A->where_clause=0;_tmp77A->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77A->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp77A;});_tmp779->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp779;});_tmp778;});
break;case 298: _LL32A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77B;(_tmp77B.YY10).tag=14;(_tmp77B.YY10).f1=({struct Cyc_List_List*_tmp77C=
_cycalloc(sizeof(*_tmp77C));_tmp77C->hd=({struct Cyc_Absyn_Switch_clause*_tmp77D=
_cycalloc(sizeof(*_tmp77D));_tmp77D->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp77D->pat_vars=0;
_tmp77D->where_clause=0;_tmp77D->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp77D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp77D;});
_tmp77C->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp77C;});_tmp77B;});break;case 299: _LL32B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77E;(_tmp77E.YY10).tag=14;(_tmp77E.YY10).f1=({struct
Cyc_List_List*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F->hd=({struct Cyc_Absyn_Switch_clause*
_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp780->pat_vars=0;
_tmp780->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp780->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp780->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp780;});
_tmp77F->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp77F;});_tmp77E;});break;case 300: _LL32C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp781;(_tmp781.YY10).tag=14;(_tmp781.YY10).f1=({struct
Cyc_List_List*_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782->hd=({struct Cyc_Absyn_Switch_clause*
_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp783->pat_vars=0;
_tmp783->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp783->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp783->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp783;});
_tmp782->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp782;});_tmp781;});break;case 301: _LL32D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp784;(_tmp784.YY9).tag=13;(_tmp784.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp784;});
break;case 302: _LL32E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp785;(_tmp785.YY9).tag=13;(_tmp785.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp785;});
break;case 303: _LL32F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp786;(_tmp786.YY9).tag=13;(_tmp786.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp786;});
break;case 304: _LL330: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp787;(_tmp787.YY9).tag=13;(_tmp787.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp787;});break;case 305: _LL331: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp788;(_tmp788.YY9).tag=13;(_tmp788.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp788;});
break;case 306: _LL332: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp789;(_tmp789.YY9).tag=13;(_tmp789.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp789;});
break;case 307: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78A;(_tmp78A.YY9).tag=13;(_tmp78A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78A;});
break;case 308: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78B;(_tmp78B.YY9).tag=13;(_tmp78B.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78B;});
break;case 309: _LL335: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78C;(_tmp78C.YY9).tag=13;(_tmp78C.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78C;});
break;case 310: _LL336: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78D;(_tmp78D.YY9).tag=13;(_tmp78D.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78D;});
break;case 311: _LL337: {struct Cyc_List_List*_tmp78E=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp78F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp790;(_tmp790.YY9).tag=13;(_tmp790.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp78E,_tmp78F);_tmp790;});break;}case 312: _LL338: {struct Cyc_List_List*_tmp791=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp792=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp793;(_tmp793.YY9).tag=13;(_tmp793.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp791,_tmp792);_tmp793;});break;}case 313: _LL339: {struct Cyc_List_List*_tmp794=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp795=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp796;(_tmp796.YY9).tag=13;(_tmp796.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp794,_tmp795);_tmp796;});break;}case 314: _LL33A: {struct Cyc_List_List*_tmp797=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp798=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp799;(_tmp799.YY9).tag=13;(_tmp799.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp797,_tmp798);_tmp799;});break;}case 315: _LL33B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79A;(_tmp79A.YY9).tag=13;(_tmp79A.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dynforward_ptr*_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79A;});
break;case 316: _LL33C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79C;(_tmp79C.YY9).tag=13;(_tmp79C.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79C;});
break;case 317: _LL33D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79D;(_tmp79D.YY9).tag=13;(_tmp79D.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79D;});
break;case 318: _LL33E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79E;(_tmp79E.YY9).tag=13;(_tmp79E.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79E;});
break;case 319: _LL33F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79F;(_tmp79F.YY9).tag=13;(_tmp79F.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp79F;});
break;case 320: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A0;(_tmp7A0.YY9).tag=13;(_tmp7A0.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7A0;});
break;case 321: _LL341: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A1;(_tmp7A1.YY9).tag=13;(_tmp7A1.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp7A1;});
break;case 322: _LL342: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A2;(_tmp7A2.YY9).tag=13;(_tmp7A2.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp7A2;});
break;case 323: _LL343: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 325: _LL345: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7A3;(_tmp7A3.YY11).tag=15;(_tmp7A3.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A3;});
break;case 326: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL347: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A4;(_tmp7A4.YY11).tag=15;(_tmp7A4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A4;});break;case 328: _LL348: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 329: _LL349: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7A5;(_tmp7A5.YY11).tag=15;(
_tmp7A5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A5;});
break;case 330: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL34B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A6;(_tmp7A6.YY11).tag=15;(_tmp7A6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A6;});
break;case 332: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL34D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A7;(_tmp7A7.YY11).tag=15;(_tmp7A7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A7;});
break;case 334: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL34F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A8;(_tmp7A8.YY11).tag=15;(_tmp7A8.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A8;});
break;case 336: _LL350: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 337: _LL351: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A9;(_tmp7A9.YY11).tag=15;(_tmp7A9.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A9;});break;case 338: _LL352: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7AA;(_tmp7AA.YY11).tag=15;(_tmp7AA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AA;});
break;case 339: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 340: _LL354: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AB;(_tmp7AB.YY11).tag=15;(_tmp7AB.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7AB;});break;case 341: _LL355: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7AC;(_tmp7AC.YY11).tag=15;(_tmp7AC.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AC;});
break;case 342: _LL356: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AD;(_tmp7AD.YY11).tag=15;(_tmp7AD.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7AD;});break;case 343: _LL357: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7AE;(_tmp7AE.YY11).tag=15;(_tmp7AE.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AE;});
break;case 344: _LL358: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 345: _LL359: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AF;(_tmp7AF.YY11).tag=15;(_tmp7AF.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AF;});
break;case 346: _LL35A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B0;(_tmp7B0.YY11).tag=15;(_tmp7B0.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B0;});
break;case 347: _LL35B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 348: _LL35C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B1;(_tmp7B1.YY11).tag=15;(_tmp7B1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B1;});
break;case 349: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B2;(_tmp7B2.YY11).tag=15;(_tmp7B2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B2;});
break;case 350: _LL35E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 351: _LL35F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B3;(_tmp7B3.YY11).tag=15;(_tmp7B3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B3;});
break;case 352: _LL360: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B4;(_tmp7B4.YY11).tag=15;(_tmp7B4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B4;});
break;case 353: _LL361: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B5;(_tmp7B5.YY11).tag=15;(_tmp7B5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B5;});
break;case 354: _LL362: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL363: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B6;(_tmp7B6.YY11).tag=15;(_tmp7B6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B6;});break;case 356: _LL364: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 357: _LL365: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B7;(_tmp7B7.YY11).tag=15;(
_tmp7B7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7B7;});break;case 358:
_LL366: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B8;(_tmp7B8.YY11).tag=
15;(_tmp7B8.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B8;});
break;case 359: _LL367: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B9;(_tmp7B9.YY11).tag=15;(_tmp7B9.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B9;});break;case 360: _LL368: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7BA;(_tmp7BA.YY11).tag=15;(_tmp7BA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7BA;});break;case 361: _LL369: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7BB;(_tmp7BB.YY11).tag=15;(_tmp7BB.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp7BC=
_cycalloc(sizeof(*_tmp7BC));_tmp7BC[0]=({struct Cyc_Absyn_StructField_struct
_tmp7BD;_tmp7BD.tag=0;_tmp7BD.f1=({struct _dynforward_ptr*_tmp7BE=_cycalloc(
sizeof(*_tmp7BE));_tmp7BE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7BE;});_tmp7BD;});
_tmp7BC;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7BB;});break;case 362: _LL36A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7BF;(_tmp7BF.YY11).tag=15;(_tmp7BF.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7C0=
_cycalloc_atomic(sizeof(*_tmp7C0));_tmp7C0[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp7C1;_tmp7C1.tag=1;_tmp7C1.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7C1;});_tmp7C0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7BF;});break;case 363: _LL36B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 364: _LL36C: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 365: _LL36D:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C2;(_tmp7C2.YY11).tag=
15;(_tmp7C2.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C2;});
break;case 366: _LL36E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7C3;(_tmp7C3.YY11).tag=15;(_tmp7C3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7C3;});break;
case 367: _LL36F: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7C4=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7C5;void*_tmp7C6;char _tmp7C7;union Cyc_Absyn_Cnst_union
_tmp7C8;void*_tmp7C9;short _tmp7CA;union Cyc_Absyn_Cnst_union _tmp7CB;void*_tmp7CC;
int _tmp7CD;union Cyc_Absyn_Cnst_union _tmp7CE;struct _dynforward_ptr _tmp7CF;union
Cyc_Absyn_Cnst_union _tmp7D0;union Cyc_Absyn_Cnst_union _tmp7D1;union Cyc_Absyn_Cnst_union
_tmp7D2;_LL372: if(*((int*)_tmp7C4)!= 0)goto _LL374;_tmp7C5=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1).Char_c).tag != 
0)goto _LL374;_tmp7C6=(_tmp7C5.Char_c).f1;_tmp7C7=(_tmp7C5.Char_c).f2;_LL373:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7D3;(_tmp7D3.YY11).tag=
15;(_tmp7D3.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7D4=_cycalloc_atomic(sizeof(*_tmp7D4));_tmp7D4[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7D5;_tmp7D5.tag=8;_tmp7D5.f1=_tmp7C7;_tmp7D5;});_tmp7D4;}),e->loc);_tmp7D3;});
goto _LL371;_LL374: if(*((int*)_tmp7C4)!= 0)goto _LL376;_tmp7C8=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1).Short_c).tag != 
1)goto _LL376;_tmp7C9=(_tmp7C8.Short_c).f1;_tmp7CA=(_tmp7C8.Short_c).f2;_LL375:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7D6;(_tmp7D6.YY11).tag=
15;(_tmp7D6.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7D7=_cycalloc(sizeof(*_tmp7D7));_tmp7D7[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7D8;_tmp7D8.tag=7;_tmp7D8.f1=(void*)_tmp7C9;_tmp7D8.f2=(int)_tmp7CA;_tmp7D8;});
_tmp7D7;}),e->loc);_tmp7D6;});goto _LL371;_LL376: if(*((int*)_tmp7C4)!= 0)goto
_LL378;_tmp7CB=((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7C4)->f1).Int_c).tag != 2)goto _LL378;_tmp7CC=(_tmp7CB.Int_c).f1;_tmp7CD=(
_tmp7CB.Int_c).f2;_LL377: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7D9;(_tmp7D9.YY11).tag=15;(_tmp7D9.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7DB;_tmp7DB.tag=7;_tmp7DB.f1=(void*)_tmp7CC;
_tmp7DB.f2=_tmp7CD;_tmp7DB;});_tmp7DA;}),e->loc);_tmp7D9;});goto _LL371;_LL378:
if(*((int*)_tmp7C4)!= 0)goto _LL37A;_tmp7CE=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1).Float_c).tag != 
4)goto _LL37A;_tmp7CF=(_tmp7CE.Float_c).f1;_LL379: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7DC;(_tmp7DC.YY11).tag=15;(_tmp7DC.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));
_tmp7DD[0]=({struct Cyc_Absyn_Float_p_struct _tmp7DE;_tmp7DE.tag=9;_tmp7DE.f1=
_tmp7CF;_tmp7DE;});_tmp7DD;}),e->loc);_tmp7DC;});goto _LL371;_LL37A: if(*((int*)
_tmp7C4)!= 0)goto _LL37C;_tmp7D0=((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1).Null_c).tag != 6)goto _LL37C;
_LL37B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7DF;(_tmp7DF.YY11).tag=
15;(_tmp7DF.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7DF;});goto _LL371;
_LL37C: if(*((int*)_tmp7C4)!= 0)goto _LL37E;_tmp7D1=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1).String_c).tag
!= 5)goto _LL37E;_LL37D: Cyc_Parse_err(({const char*_tmp7E0="strings cannot occur within patterns";
_tag_dynforward(_tmp7E0,sizeof(char),_get_zero_arr_size(_tmp7E0,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL371;
_LL37E: if(*((int*)_tmp7C4)!= 0)goto _LL380;_tmp7D2=((struct Cyc_Absyn_Const_e_struct*)
_tmp7C4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7C4)->f1).LongLong_c).tag
!= 3)goto _LL380;_LL37F: Cyc_Parse_unimp(({const char*_tmp7E1="long long's in patterns";
_tag_dynforward(_tmp7E1,sizeof(char),_get_zero_arr_size(_tmp7E1,24));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL371;
_LL380:;_LL381: Cyc_Parse_err(({const char*_tmp7E2="bad constant in case";
_tag_dynforward(_tmp7E2,sizeof(char),_get_zero_arr_size(_tmp7E2,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL371:;}break;}
case 368: _LL370: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7E3;(
_tmp7E3.YY11).tag=15;(_tmp7E3.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7E5;_tmp7E5.tag=12;_tmp7E5.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E5;});_tmp7E4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E3;});
break;case 369: _LL382: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7E6="as";
_tag_dynforward(_tmp7E6,sizeof(char),_get_zero_arr_size(_tmp7E6,3));}))!= 0)Cyc_Parse_err(({
const char*_tmp7E7="expecting `as'";_tag_dynforward(_tmp7E7,sizeof(char),
_get_zero_arr_size(_tmp7E7,15));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7E8;(_tmp7E8.YY11).tag=15;(_tmp7E8.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7E9=_cycalloc(
sizeof(*_tmp7E9));_tmp7E9[0]=({struct Cyc_Absyn_Var_p_struct _tmp7EA;_tmp7EA.tag=0;
_tmp7EA.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7EB=_cycalloc(sizeof(*
_tmp7EB));_tmp7EB->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7ED;(_tmp7ED.Loc_n).tag=0;_tmp7ED;});_tmp7EB->f2=({struct _dynforward_ptr*
_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7EC;});_tmp7EB;}),(
void*)0,0);_tmp7EA.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7EA;});_tmp7E9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7E8;});
break;case 370: _LL383: {struct Cyc_List_List*_tmp7EF;int _tmp7F0;struct _tuple13
_tmp7EE=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7EF=_tmp7EE.f1;_tmp7F0=_tmp7EE.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7F1;(_tmp7F1.YY11).tag=15;(_tmp7F1.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));
_tmp7F2[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7F3;_tmp7F3.tag=3;_tmp7F3.f1=
_tmp7EF;_tmp7F3.f2=_tmp7F0;_tmp7F3;});_tmp7F2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F1;});
break;}case 371: _LL384: {struct Cyc_List_List*_tmp7F5;int _tmp7F6;struct _tuple13
_tmp7F4=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7F5=_tmp7F4.f1;_tmp7F6=_tmp7F4.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7F7;(_tmp7F7.YY11).tag=15;(_tmp7F7.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));
_tmp7F8[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7F9;_tmp7F9.tag=13;_tmp7F9.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7F9.f2=_tmp7F5;_tmp7F9.f3=_tmp7F6;_tmp7F9;});_tmp7F8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F7;});
break;}case 372: _LL385: {struct Cyc_List_List*_tmp7FB;int _tmp7FC;struct _tuple13
_tmp7FA=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7FB=_tmp7FA.f1;_tmp7FC=_tmp7FA.f2;{struct Cyc_List_List*_tmp7FD=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7FE;(_tmp7FE.YY11).tag=15;(_tmp7FE.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp800;_tmp800.tag=5;_tmp800.f1=({struct Cyc_Absyn_AggrInfo
_tmp801;_tmp801.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp802;(_tmp802.UnknownAggr).tag=0;(_tmp802.UnknownAggr).f1=(void*)((void*)0);(
_tmp802.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp802;});_tmp801.targs=0;_tmp801;});_tmp800.f2=
_tmp7FD;_tmp800.f3=_tmp7FB;_tmp800.f4=_tmp7FC;_tmp800;});_tmp7FF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FE;});
break;}}case 373: _LL386: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp803;(_tmp803.YY11).tag=15;(_tmp803.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp805;_tmp805.tag=4;_tmp805.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp805;});_tmp804;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp803;});break;case 374: _LL387: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp806;(_tmp806.YY11).tag=15;(_tmp806.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp807=_cycalloc(sizeof(*_tmp807));
_tmp807[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp808;_tmp808.tag=4;_tmp808.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp809=_cycalloc(
sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp80A;_tmp80A.tag=
4;_tmp80A.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp80A;});_tmp809;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp808;});
_tmp807;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp806;});break;case 375: _LL388: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp80B;(_tmp80B.YY11).tag=15;(_tmp80B.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));
_tmp80C[0]=({struct Cyc_Absyn_Reference_p_struct _tmp80D;_tmp80D.tag=1;_tmp80D.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp810;(
_tmp810.Loc_n).tag=0;_tmp810;});_tmp80E->f2=({struct _dynforward_ptr*_tmp80F=
_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp80F;});_tmp80E;}),(void*)
0,0);_tmp80D.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp80D;});
_tmp80C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp80B;});break;case 376: _LL389: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp811="as";_tag_dynforward(_tmp811,
sizeof(char),_get_zero_arr_size(_tmp811,3));}))!= 0)Cyc_Parse_err(({const char*
_tmp812="expecting `as'";_tag_dynforward(_tmp812,sizeof(char),_get_zero_arr_size(
_tmp812,15));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp813;(_tmp813.YY11).tag=15;(_tmp813.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Reference_p_struct*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp815;_tmp815.tag=1;_tmp815.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp818;(_tmp818.Loc_n).tag=0;_tmp818;});_tmp816->f2=({
struct _dynforward_ptr*_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp817;});_tmp816;}),(
void*)0,0);_tmp815.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp815;});_tmp814;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp813;});
break;case 377: _LL38A: {void*_tmp819=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp823;_tmp823.tag=0;_tmp823.f1=(void*)((void*)7);_tmp823;});_tmp822;}));yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp81A;(_tmp81A.YY11).tag=15;(
_tmp81A.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp81C;_tmp81C.tag=2;_tmp81C.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp819);
_tmp81C.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp81D=_cycalloc(sizeof(*
_tmp81D));_tmp81D->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp81F;(_tmp81F.Loc_n).tag=0;_tmp81F;});_tmp81D->f2=({struct _dynforward_ptr*
_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp81E;});_tmp81D;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp820=_cycalloc(sizeof(*_tmp820));
_tmp820[0]=({struct Cyc_Absyn_TagType_struct _tmp821;_tmp821.tag=18;_tmp821.f1=(
void*)_tmp819;_tmp821;});_tmp820;}),0);_tmp81C;});_tmp81B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp81A;});
break;}case 378: _LL38B: {struct Cyc_Absyn_Tvar*_tmp824=Cyc_Tcutil_new_tvar((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp830;_tmp830.tag=0;_tmp830.f1=(void*)((void*)7);
_tmp830;});_tmp82F;}));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp825;(_tmp825.YY11).tag=15;(_tmp825.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_TagInt_p_struct*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=({
struct Cyc_Absyn_TagInt_p_struct _tmp827;_tmp827.tag=2;_tmp827.f1=_tmp824;_tmp827.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp828=_cycalloc(sizeof(*_tmp828));
_tmp828->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp82A;(
_tmp82A.Loc_n).tag=0;_tmp82A;});_tmp828->f2=({struct _dynforward_ptr*_tmp829=
_cycalloc(sizeof(*_tmp829));_tmp829[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp829;});_tmp828;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp82B=_cycalloc(sizeof(*_tmp82B));
_tmp82B[0]=({struct Cyc_Absyn_TagType_struct _tmp82C;_tmp82C.tag=18;_tmp82C.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp82D=_cycalloc(sizeof(*_tmp82D));
_tmp82D[0]=({struct Cyc_Absyn_VarType_struct _tmp82E;_tmp82E.tag=1;_tmp82E.f1=
_tmp824;_tmp82E;});_tmp82D;}));_tmp82C;});_tmp82B;}),0);_tmp827;});_tmp826;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp825;});break;}case 379: _LL38C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp831;(_tmp831.YY12).tag=16;(_tmp831.YY12).f1=({struct
_tuple13*_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp832->f2=0;_tmp832;});
_tmp831;});break;case 380: _LL38D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp833;(_tmp833.YY12).tag=16;(_tmp833.YY12).f1=({struct _tuple13*_tmp834=
_cycalloc(sizeof(*_tmp834));_tmp834->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp834->f2=1;_tmp834;});_tmp833;});break;case 381: _LL38E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp835;(_tmp835.YY12).tag=
16;(_tmp835.YY12).f1=({struct _tuple13*_tmp836=_cycalloc(sizeof(*_tmp836));
_tmp836->f1=0;_tmp836->f2=1;_tmp836;});_tmp835;});break;case 382: _LL38F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp837;(_tmp837.YY13).tag=17;(
_tmp837.YY13).f1=({struct Cyc_List_List*_tmp838=_cycalloc(sizeof(*_tmp838));
_tmp838->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp838->tl=0;_tmp838;});_tmp837;});break;case 383: _LL390: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp839;(_tmp839.YY13).tag=17;(
_tmp839.YY13).f1=({struct Cyc_List_List*_tmp83A=_cycalloc(sizeof(*_tmp83A));
_tmp83A->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp83A->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp83A;});_tmp839;});break;case 384: _LL391: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp83B;(_tmp83B.YY14).tag=18;(_tmp83B.YY14).f1=({
struct _tuple14*_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C->f1=0;_tmp83C->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp83C;});_tmp83B;});
break;case 385: _LL392: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83D;(_tmp83D.YY14).tag=18;(_tmp83D.YY14).f1=({struct _tuple14*_tmp83E=
_cycalloc(sizeof(*_tmp83E));_tmp83E->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp83E->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp83E;});_tmp83D;});
break;case 386: _LL393: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83F;(_tmp83F.YY16).tag=20;(_tmp83F.YY16).f1=({struct _tuple13*_tmp840=
_cycalloc(sizeof(*_tmp840));_tmp840->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp840->f2=0;_tmp840;});_tmp83F;});break;case 387: _LL394: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp841;(_tmp841.YY16).tag=20;(
_tmp841.YY16).f1=({struct _tuple13*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp842->f2=1;_tmp842;});
_tmp841;});break;case 388: _LL395: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp843;(_tmp843.YY16).tag=20;(_tmp843.YY16).f1=({struct _tuple13*_tmp844=
_cycalloc(sizeof(*_tmp844));_tmp844->f1=0;_tmp844->f2=1;_tmp844;});_tmp843;});
break;case 389: _LL396: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp845;(_tmp845.YY15).tag=19;(_tmp845.YY15).f1=({struct Cyc_List_List*_tmp846=
_cycalloc(sizeof(*_tmp846));_tmp846->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp846->tl=0;_tmp846;});
_tmp845;});break;case 390: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp847;(_tmp847.YY15).tag=19;(_tmp847.YY15).f1=({struct Cyc_List_List*_tmp848=
_cycalloc(sizeof(*_tmp848));_tmp848->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp848->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp848;});_tmp847;});
break;case 391: _LL398: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL399: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp849;(_tmp849.YY3).tag=7;(_tmp849.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
break;case 393: _LL39A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 394: _LL39B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY3).tag=7;(_tmp84A.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
break;case 395: _LL39C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84B;(_tmp84B.YY8).tag=12;(_tmp84B.YY8).f1=0;_tmp84B;});break;case 396: _LL39D:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp84C;(_tmp84C.YY8).tag=
12;(_tmp84C.YY8).f1=({struct Cyc_Core_Opt*_tmp84D=_cycalloc(sizeof(*_tmp84D));
_tmp84D->v=(void*)((void*)1);_tmp84D;});_tmp84C;});break;case 397: _LL39E: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp84E;(_tmp84E.YY8).tag=12;(
_tmp84E.YY8).f1=({struct Cyc_Core_Opt*_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F->v=(
void*)((void*)3);_tmp84F;});_tmp84E;});break;case 398: _LL39F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp850;(_tmp850.YY8).tag=12;(_tmp850.YY8).f1=({struct Cyc_Core_Opt*
_tmp851=_cycalloc(sizeof(*_tmp851));_tmp851->v=(void*)((void*)4);_tmp851;});
_tmp850;});break;case 399: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY8).tag=12;(_tmp852.YY8).f1=({struct Cyc_Core_Opt*_tmp853=
_cycalloc(sizeof(*_tmp853));_tmp853->v=(void*)((void*)0);_tmp853;});_tmp852;});
break;case 400: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY8).tag=12;(_tmp854.YY8).f1=({struct Cyc_Core_Opt*_tmp855=
_cycalloc(sizeof(*_tmp855));_tmp855->v=(void*)((void*)2);_tmp855;});_tmp854;});
break;case 401: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY8).tag=12;(_tmp856.YY8).f1=({struct Cyc_Core_Opt*_tmp857=
_cycalloc(sizeof(*_tmp857));_tmp857->v=(void*)((void*)16);_tmp857;});_tmp856;});
break;case 402: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp858;(_tmp858.YY8).tag=12;(_tmp858.YY8).f1=({struct Cyc_Core_Opt*_tmp859=
_cycalloc(sizeof(*_tmp859));_tmp859->v=(void*)((void*)17);_tmp859;});_tmp858;});
break;case 403: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85A;(_tmp85A.YY8).tag=12;(_tmp85A.YY8).f1=({struct Cyc_Core_Opt*_tmp85B=
_cycalloc(sizeof(*_tmp85B));_tmp85B->v=(void*)((void*)13);_tmp85B;});_tmp85A;});
break;case 404: _LL3A5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85C;(_tmp85C.YY8).tag=12;(_tmp85C.YY8).f1=({struct Cyc_Core_Opt*_tmp85D=
_cycalloc(sizeof(*_tmp85D));_tmp85D->v=(void*)((void*)15);_tmp85D;});_tmp85C;});
break;case 405: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85E;(_tmp85E.YY8).tag=12;(_tmp85E.YY8).f1=({struct Cyc_Core_Opt*_tmp85F=
_cycalloc(sizeof(*_tmp85F));_tmp85F->v=(void*)((void*)14);_tmp85F;});_tmp85E;});
break;case 406: _LL3A7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 407: _LL3A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp860;(_tmp860.YY3).tag=7;(_tmp860.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp860;});
break;case 408: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp861;(_tmp861.YY3).tag=7;(_tmp861.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
break;case 409: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp862;(_tmp862.YY3).tag=7;(_tmp862.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp862;});
break;case 410: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp863;(_tmp863.YY3).tag=7;(_tmp863.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
break;case 411: _LL3AC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp864;(_tmp864.YY3).tag=7;(_tmp864.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp864;});
break;case 412: _LL3AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp865;(_tmp865.YY3).tag=7;(_tmp865.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
break;case 413: _LL3AE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 414: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 415: _LL3B0: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp866;(_tmp866.YY3).tag=7;(_tmp866.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp866;});
break;case 416: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 417: _LL3B2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp867;(_tmp867.YY3).tag=7;(_tmp867.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp867;});
break;case 418: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3B4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp868;(_tmp868.YY3).tag=7;(_tmp868.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp868;});
break;case 420: _LL3B5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp869;(_tmp869.YY3).tag=7;(_tmp869.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp869;});
break;case 422: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 423: _LL3B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86A;(_tmp86A.YY3).tag=7;(_tmp86A.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86A;});
break;case 424: _LL3B9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 425: _LL3BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86B;(_tmp86B.YY3).tag=7;(_tmp86B.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86B;});
break;case 426: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86C;(_tmp86C.YY3).tag=7;(_tmp86C.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86C;});
break;case 427: _LL3BC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 428: _LL3BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86D;(_tmp86D.YY3).tag=7;(_tmp86D.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86D;});
break;case 429: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86E;(_tmp86E.YY3).tag=7;(_tmp86E.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86E;});
break;case 430: _LL3BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86F;(_tmp86F.YY3).tag=7;(_tmp86F.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86F;});
break;case 431: _LL3C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp870;(_tmp870.YY3).tag=7;(_tmp870.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp870;});
break;case 432: _LL3C1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 433: _LL3C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp871;(_tmp871.YY3).tag=7;(_tmp871.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
break;case 434: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp872;(_tmp872.YY3).tag=7;(_tmp872.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp872;});
break;case 435: _LL3C4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 436: _LL3C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp873;(_tmp873.YY3).tag=7;(_tmp873.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp873;});
break;case 437: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp874;(_tmp874.YY3).tag=7;(_tmp874.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp874;});
break;case 438: _LL3C7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 439: _LL3C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp875;(_tmp875.YY3).tag=7;(_tmp875.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp875;});
break;case 440: _LL3C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp876;(_tmp876.YY3).tag=7;(_tmp876.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp876;});
break;case 441: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp877;(_tmp877.YY3).tag=7;(_tmp877.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
break;case 442: _LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 443: _LL3CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp878;(_tmp878.YY3).tag=7;(_tmp878.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp878;});
break;case 444: _LL3CD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 445: _LL3CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp879;(_tmp879.YY3).tag=7;(_tmp879.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp879;});
break;case 446: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87A;(_tmp87A.YY3).tag=7;(_tmp87A.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87A;});
break;case 447: _LL3D0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87B;(_tmp87B.YY3).tag=7;(_tmp87B.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87B;});
break;case 448: _LL3D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87C;(_tmp87C.YY3).tag=7;(_tmp87C.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87C;});
break;case 449: _LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 450: _LL3D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87D;(_tmp87D.YY3).tag=7;(_tmp87D.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87D;});
break;case 451: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87E;(_tmp87E.YY3).tag=7;(_tmp87E.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87E;});
break;case 452: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87F;(_tmp87F.YY3).tag=7;(_tmp87F.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87F;});
break;case 453: _LL3D6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp880;(_tmp880.YY3).tag=7;(_tmp880.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({
struct Cyc_Absyn_StructField_struct _tmp882;_tmp882.tag=0;_tmp882.f1=({struct
_dynforward_ptr*_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp883;});_tmp882;});
_tmp881;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp880;});break;case 454: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp884;(_tmp884.YY3).tag=7;(_tmp884.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp885=_cycalloc_atomic(sizeof(*
_tmp885));_tmp885[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp886;_tmp886.tag=1;
_tmp886.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp886;});_tmp885;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp884;});
break;case 455: _LL3D8: {struct Cyc_Position_Segment*_tmp887=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp888=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp887,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp889;(_tmp889.YY3).tag=
7;(_tmp889.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp888,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp889;});
break;}case 456: _LL3D9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88A;(_tmp88A.YY3).tag=7;(_tmp88A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp88C;_tmp88C.tag=35;_tmp88C.f1=({struct Cyc_Absyn_MallocInfo
_tmp88D;_tmp88D.is_calloc=0;_tmp88D.rgn=0;_tmp88D.elt_type=0;_tmp88D.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp88D.fat_result=0;_tmp88D;});_tmp88C;});_tmp88B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88A;});
break;case 457: _LL3DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88E;(_tmp88E.YY3).tag=7;(_tmp88E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp890;_tmp890.tag=35;_tmp890.f1=({struct Cyc_Absyn_MallocInfo
_tmp891;_tmp891.is_calloc=0;_tmp891.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp891.elt_type=0;
_tmp891.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp891.fat_result=0;_tmp891;});_tmp890;});_tmp88F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88E;});
break;case 458: _LL3DB: {void*_tmp893;struct _tuple2 _tmp892=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp893=_tmp892.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp894;(_tmp894.YY3).tag=7;(
_tmp894.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp896;_tmp896.tag=35;_tmp896.f1=({struct Cyc_Absyn_MallocInfo _tmp897;_tmp897.is_calloc=
1;_tmp897.rgn=0;_tmp897.elt_type=({void**_tmp898=_cycalloc(sizeof(*_tmp898));
_tmp898[0]=_tmp893;_tmp898;});_tmp897.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp897.fat_result=0;
_tmp897;});_tmp896;});_tmp895;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp894;});
break;}case 459: _LL3DC: {void*_tmp89A;struct _tuple2 _tmp899=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp89A=_tmp899.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp89B;(_tmp89B.YY3).tag=7;(
_tmp89B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp89D;_tmp89D.tag=35;_tmp89D.f1=({struct Cyc_Absyn_MallocInfo _tmp89E;_tmp89E.is_calloc=
1;_tmp89E.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp89E.elt_type=({void**
_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=_tmp89A;_tmp89F;});_tmp89E.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp89E.fat_result=0;_tmp89E;});_tmp89D;});_tmp89C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89B;});
break;}case 460: _LL3DD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A0;(_tmp8A0.YY3).tag=7;(_tmp8A0.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Swap_e_struct*_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1[0]=({struct
Cyc_Absyn_Swap_e_struct _tmp8A2;_tmp8A2.tag=36;_tmp8A2.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8A2.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8A2;});_tmp8A1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8A0;});break;case 461: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8A3;(_tmp8A3.YY7).tag=11;(_tmp8A3.YY7).f1=(void*)((
void*)12);_tmp8A3;});break;case 462: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp8A4;(_tmp8A4.YY7).tag=11;(_tmp8A4.YY7).f1=(void*)((void*)11);
_tmp8A4;});break;case 463: _LL3E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A5;(_tmp8A5.YY7).tag=11;(_tmp8A5.YY7).f1=(void*)((void*)2);_tmp8A5;});
break;case 464: _LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 465: _LL3E2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A6;(_tmp8A6.YY3).tag=7;(_tmp8A6.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
break;case 466: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A7;(_tmp8A7.YY3).tag=7;(_tmp8A7.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A7;});
break;case 467: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A8;(_tmp8A8.YY3).tag=7;(_tmp8A8.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A8;});
break;case 468: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A9;(_tmp8A9.YY3).tag=7;(_tmp8A9.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A9;});
break;case 469: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AB;(_tmp8AB.YY3).tag=7;(_tmp8AB.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AB;});
break;case 470: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AD;(_tmp8AD.YY3).tag=7;(_tmp8AD.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AD;});
break;case 471: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AE;(_tmp8AE.YY3).tag=7;(_tmp8AE.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AE;});
break;case 472: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AF;(_tmp8AF.YY3).tag=7;(_tmp8AF.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp8B0=_cycalloc(sizeof(*_tmp8B0));_tmp8B0[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp8B1;_tmp8B1.tag=27;_tmp8B1.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8B1.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8B1;});_tmp8B0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8AF;});break;case 473: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8B2;(_tmp8B2.YY3).tag=7;(_tmp8B2.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8B3=_cycalloc(sizeof(*_tmp8B3));
_tmp8B3[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp8B4;_tmp8B4.tag=27;_tmp8B4.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp8B4.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp8B4;});_tmp8B3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
break;case 474: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B5;(_tmp8B5.YY3).tag=7;(_tmp8B5.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp8B6=_cycalloc(sizeof(*_tmp8B6));_tmp8B6[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp8B7;_tmp8B7.tag=2;_tmp8B7.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B7;});_tmp8B6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8B5;});break;case 475: _LL3EC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL3ED: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8B8;(_tmp8B8.YY3).tag=7;(
_tmp8B8.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B8;});
break;case 477: _LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 478: _LL3EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B9;(_tmp8B9.YY3).tag=7;(_tmp8B9.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B9;});
break;case 479: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8BA;(_tmp8BA.YY3).tag=7;(_tmp8BA.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BA;});
break;case 480: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8BB;(_tmp8BB.YY3).tag=7;(_tmp8BB.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BB;});
break;case 481: _LL3F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8BC;(_tmp8BC.YY3).tag=7;(_tmp8BC.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp8BE;_tmp8BE.tag=30;_tmp8BE.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8BE.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8BE.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8BE.f4=0;_tmp8BE;});
_tmp8BD;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8BC;});break;case 482: _LL3F3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8BF;(_tmp8BF.YY3).tag=7;(_tmp8BF.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BF;});
break;case 483: _LL3F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C0;(_tmp8C0.YY5).tag=9;(_tmp8C0.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp8C0;});break;case 484: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8C1;(_tmp8C1.YY5).tag=9;(_tmp8C1.YY5).f1=({struct Cyc_List_List*
_tmp8C2=_cycalloc(sizeof(*_tmp8C2));_tmp8C2->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C2->tl=0;_tmp8C2;});
_tmp8C1;});break;case 485: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C3;(_tmp8C3.YY5).tag=9;(_tmp8C3.YY5).f1=({struct Cyc_List_List*_tmp8C4=
_cycalloc(sizeof(*_tmp8C4));_tmp8C4->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C4->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8C4;});_tmp8C3;});
break;case 486: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C5;(_tmp8C5.YY3).tag=7;(_tmp8C5.YY3).f1=Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C5;});
break;case 487: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C6;(_tmp8C6.YY3).tag=7;(_tmp8C6.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C6;});
break;case 488: _LL3F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C7;(_tmp8C7.YY3).tag=7;(_tmp8C7.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C7;});
break;case 489: _LL3FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C8;(_tmp8C8.YY3).tag=7;(_tmp8C8.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C8;});
break;case 490: _LL3FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C9;(_tmp8C9.QualId_tok).tag=4;(_tmp8C9.QualId_tok).f1=({struct _tuple1*
_tmp8CA=_cycalloc(sizeof(*_tmp8CA));_tmp8CA->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8CC;(_tmp8CC.Rel_n).tag=1;(_tmp8CC.Rel_n).f1=0;
_tmp8CC;});_tmp8CA->f2=({struct _dynforward_ptr*_tmp8CB=_cycalloc(sizeof(*_tmp8CB));
_tmp8CB[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8CB;});_tmp8CA;});_tmp8C9;});break;case 491: _LL3FC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 492: _LL3FD: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8CD;(_tmp8CD.QualId_tok).tag=
4;(_tmp8CD.QualId_tok).f1=({struct _tuple1*_tmp8CE=_cycalloc(sizeof(*_tmp8CE));
_tmp8CE->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8D0;(
_tmp8D0.Rel_n).tag=1;(_tmp8D0.Rel_n).f1=0;_tmp8D0;});_tmp8CE->f2=({struct
_dynforward_ptr*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));_tmp8CF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8CF;});_tmp8CE;});
_tmp8CD;});break;case 493: _LL3FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 494: _LL3FF: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 495: _LL400: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL401: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497: _LL402:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498:
_LL403: break;case 499: _LL404: yylex_buf->lex_curr_pos -=1;break;default: _LL405:
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(500,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 132)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6459) && Cyc_yycheck[_check_known_subscript_notnull(6460,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6460,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 132)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(998,yystate)];if(yyn > - 32768  && yyn < 6459){int sze=
0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 274 / 
sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6460,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(274,x)])+ 15,count ++);}msg=({unsigned int _tmp8D1=(
unsigned int)(sze + 15);char*_tmp8D2=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8D1 + 1));struct _dynforward_ptr _tmp8D4=_tag_dynforward(_tmp8D2,
sizeof(char),_tmp8D1 + 1);{unsigned int _tmp8D3=_tmp8D1;unsigned int i;for(i=0;i < 
_tmp8D3;i ++){_tmp8D2[i]='\000';}_tmp8D2[_tmp8D3]=(char)0;}_tmp8D4;});Cyc_strcpy(
msg,({const char*_tmp8D5="parse error";_tag_dynforward(_tmp8D5,sizeof(char),
_get_zero_arr_size(_tmp8D5,12));}));if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 
274 / sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6460,x + 
yyn)]== x){Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct
_dynforward_ptr)({const char*_tmp8D6=", expecting `";_tag_dynforward(_tmp8D6,
sizeof(char),_get_zero_arr_size(_tmp8D6,14));}):(struct _dynforward_ptr)({const
char*_tmp8D7=" or `";_tag_dynforward(_tmp8D7,sizeof(char),_get_zero_arr_size(
_tmp8D7,6));})));Cyc_strcat(msg,(struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(274,x)]);Cyc_strcat(msg,({const char*_tmp8D8="'";
_tag_dynforward(_tmp8D8,sizeof(char),_get_zero_arr_size(_tmp8D8,2));}));count ++;}}}
Cyc_yyerror((struct _dynforward_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8D9="parse error";
_tag_dynforward(_tmp8D9,sizeof(char),_get_zero_arr_size(_tmp8D9,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8DA=1;
_npop_handler(0);return _tmp8DA;}Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;
yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp8DB=1;_npop_handler(0);return
_tmp8DB;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(998,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 6459) || Cyc_yycheck[_check_known_subscript_notnull(
6460,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6460,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 997){int
_tmp8DC=0;_npop_handler(0);return _tmp8DC;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union _tmp8E6=v;struct _tuple11
_tmp8E7;int _tmp8E8;char _tmp8E9;struct _dynforward_ptr _tmp8EA;struct _tuple1*
_tmp8EB;struct _tuple1 _tmp8EC;union Cyc_Absyn_Nmspace_union _tmp8ED;struct
_dynforward_ptr*_tmp8EE;_LL408: if((_tmp8E6.Int_tok).tag != 0)goto _LL40A;_tmp8E7=(
_tmp8E6.Int_tok).f1;_tmp8E8=_tmp8E7.f2;_LL409:({struct Cyc_Int_pa_struct _tmp8F1;
_tmp8F1.tag=1;_tmp8F1.f1=(unsigned long)_tmp8E8;{void*_tmp8EF[1]={& _tmp8F1};Cyc_fprintf(
Cyc_stderr,({const char*_tmp8F0="%d";_tag_dynforward(_tmp8F0,sizeof(char),
_get_zero_arr_size(_tmp8F0,3));}),_tag_dynforward(_tmp8EF,sizeof(void*),1));}});
goto _LL407;_LL40A: if((_tmp8E6.Char_tok).tag != 1)goto _LL40C;_tmp8E9=(_tmp8E6.Char_tok).f1;
_LL40B:({struct Cyc_Int_pa_struct _tmp8F4;_tmp8F4.tag=1;_tmp8F4.f1=(unsigned long)((
int)_tmp8E9);{void*_tmp8F2[1]={& _tmp8F4};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8F3="%c";_tag_dynforward(_tmp8F3,sizeof(char),_get_zero_arr_size(_tmp8F3,3));}),
_tag_dynforward(_tmp8F2,sizeof(void*),1));}});goto _LL407;_LL40C: if((_tmp8E6.String_tok).tag
!= 2)goto _LL40E;_tmp8EA=(_tmp8E6.String_tok).f1;_LL40D:({struct Cyc_String_pa_struct
_tmp8F7;_tmp8F7.tag=0;_tmp8F7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp8EA);{void*_tmp8F5[1]={& _tmp8F7};Cyc_fprintf(Cyc_stderr,({const char*_tmp8F6="\"%s\"";
_tag_dynforward(_tmp8F6,sizeof(char),_get_zero_arr_size(_tmp8F6,5));}),
_tag_dynforward(_tmp8F5,sizeof(void*),1));}});goto _LL407;_LL40E: if((_tmp8E6.QualId_tok).tag
!= 4)goto _LL410;_tmp8EB=(_tmp8E6.QualId_tok).f1;_tmp8EC=*_tmp8EB;_tmp8ED=_tmp8EC.f1;
_tmp8EE=_tmp8EC.f2;_LL40F: {struct Cyc_List_List*_tmp8F8=0;{union Cyc_Absyn_Nmspace_union
_tmp8F9=_tmp8ED;struct Cyc_List_List*_tmp8FA;struct Cyc_List_List*_tmp8FB;_LL413:
if((_tmp8F9.Rel_n).tag != 1)goto _LL415;_tmp8FA=(_tmp8F9.Rel_n).f1;_LL414: _tmp8F8=
_tmp8FA;goto _LL412;_LL415: if((_tmp8F9.Abs_n).tag != 2)goto _LL417;_tmp8FB=(_tmp8F9.Abs_n).f1;
_LL416: _tmp8F8=_tmp8FB;goto _LL412;_LL417: if((_tmp8F9.Loc_n).tag != 0)goto _LL412;
_LL418: goto _LL412;_LL412:;}for(0;_tmp8F8 != 0;_tmp8F8=_tmp8F8->tl){({struct Cyc_String_pa_struct
_tmp8FE;_tmp8FE.tag=0;_tmp8FE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_tmp8F8->hd));{void*_tmp8FC[1]={& _tmp8FE};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8FD="%s::";_tag_dynforward(_tmp8FD,sizeof(char),_get_zero_arr_size(
_tmp8FD,5));}),_tag_dynforward(_tmp8FC,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp901;_tmp901.tag=0;_tmp901.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp8EE);{void*_tmp8FF[1]={& _tmp901};Cyc_fprintf(Cyc_stderr,({const char*_tmp900="%s::";
_tag_dynforward(_tmp900,sizeof(char),_get_zero_arr_size(_tmp900,5));}),
_tag_dynforward(_tmp8FF,sizeof(void*),1));}});goto _LL407;}_LL410:;_LL411:({void*
_tmp902[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp903="?";_tag_dynforward(
_tmp903,sizeof(char),_get_zero_arr_size(_tmp903,2));}),_tag_dynforward(_tmp902,
sizeof(void*),0));});goto _LL407;_LL407:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
