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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};struct _dynforward_ptr Cyc_vrprintf(struct
_RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dynforward_ptr,int*is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_unique_pointer(
void*t);int Cyc_Tcutil_is_unique_path(struct Cyc_Absyn_Exp*e);struct _tuple6{int f1;
void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct _tuple7{unsigned int f1;int
f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(
void*t);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dynforward_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
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
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dynforward_ptr msg,struct _dynforward_ptr ap){({void*_tmp0[0]={};
Cyc_Tcutil_terr(loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dynforward(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle _tmp4=_new_region("r");struct
_RegionHandle*r=& _tmp4;_push_region(r);{void*_tmp5=Cyc_Tcenv_lookup_ordinary(r,
te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*
_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*
_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;
_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;_LL2:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({
struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;
_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct
Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=33;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=34;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=32;_tmp14.f1=0;
_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9: if(*((int*)
_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*
_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,31));}),
_tag_dynforward(_tmp15,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({
struct Cyc_Absyn_Var_e_struct _tmp19;_tmp19.tag=1;_tmp19.f1=q;_tmp19.f2=(void*)((
void*)0);_tmp19;});_tmp18;})));goto _LL0;_LL0:;};_pop_region(r);};_pop_handler();}
else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(_tmp1B != Cyc_Dict_Absent)
goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1C=
_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Var_e_struct _tmp1D;_tmp1D.tag=
1;_tmp1D.f1=q;_tmp1D.f2=(void*)((void*)0);_tmp1D;});_tmp1C;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=0;_tmp1E->f2=
e;_tmp1E;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(
void*)e1->r;struct _tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp1F)->f1;_LL12: {struct _handler_cons
_tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))_tmp23=1;
if(!_tmp23){{struct _RegionHandle _tmp24=_new_region("r");struct _RegionHandle*r=&
_tmp24;_push_region(r);{void*_tmp25=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,
_tmp20);void*_tmp26;struct Cyc_Absyn_Tuniondecl*_tmp27;struct Cyc_Absyn_Tunionfield*
_tmp28;struct Cyc_Absyn_Aggrdecl*_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;
_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp25)->f1;_LL17:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));
_tmp2A[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2B;_tmp2B.tag=11;_tmp2B.f1=e1;
_tmp2B.f2=es;_tmp2B.f3=0;_tmp2B;});_tmp2A;})));_npop_handler(1);return;_LL18: if(*((
int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f1;
_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f2;_LL19: if(_tmp28->typs == 0)({
struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp28->name));{void*_tmp2C[1]={&
_tmp2E};Cyc_Tcutil_terr(e->loc,({const char*_tmp2D="%s is a constant, not a function";
_tag_dynforward(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,33));}),
_tag_dynforward(_tmp2C,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_e_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({
struct Cyc_Absyn_Tunion_e_struct _tmp30;_tmp30.tag=32;_tmp30.f1=es;_tmp30.f2=
_tmp27;_tmp30.f3=_tmp28;_tmp30;});_tmp2F;})));_npop_handler(1);return;_LL1A: if(*((
int*)_tmp25)!= 1)goto _LL1C;_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;
_LL1B: {struct Cyc_List_List*_tmp31=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Struct_e_struct _tmp33;
_tmp33.tag=30;_tmp33.f1=_tmp29->name;_tmp33.f2=0;_tmp33.f3=_tmp31;_tmp33.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp29;_tmp33;});_tmp32;})));_npop_handler(1);return;}
_LL1C: if(*((int*)_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp20));{
void*_tmp34[1]={& _tmp36};Cyc_Tcutil_terr(e->loc,({const char*_tmp35="%s is an enum constructor, not a function";
_tag_dynforward(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,42));}),
_tag_dynforward(_tmp34,sizeof(void*),1));}});_npop_handler(1);return;_LL15:;};
_pop_region(r);};_pop_handler();}else{void*_tmp22=(void*)_exn_thrown;void*_tmp38=
_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3A;_tmp3A.tag=11;_tmp3A.f1=e1;_tmp3A.f2=
es;_tmp3A.f3=0;_tmp3A;});_tmp39;})));return;_LL23:;_LL24:(void)_throw(_tmp38);
_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp3C;_tmp3C.tag=11;_tmp3C.f1=e1;_tmp3C.f2=es;_tmp3C.f3=0;_tmp3C;});_tmp3B;})));
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3E;_tmp3E.tag=28;_tmp3E.f1=des;_tmp3E;});_tmp3D;})));return;}{void*t=*topt;
void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;union Cyc_Absyn_AggrInfoU_union
_tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;struct Cyc_Absyn_Tqual _tmp44;
_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)_tmp3F)!= 10)goto _LL28;_tmp40=((
struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LL27:{
union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;struct Cyc_Absyn_Aggrdecl**_tmp46;
struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((_tmp45.UnknownAggr).tag != 0)goto _LL31;
_LL30:({void*_tmp48[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49="struct type not properly set";
_tag_dynforward(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,29));}),
_tag_dynforward(_tmp48,sizeof(void*),0));});_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp4B;_tmp4B.tag=30;_tmp4B.f1=_tmp47->name;
_tmp4B.f2=0;_tmp4B.f3=des;_tmp4B.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp47;_tmp4B;});
_tmp4A;})));_LL2E:;}goto _LL25;_LL28: if(*((int*)_tmp3F)!= 7)goto _LL2A;_tmp42=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;_tmp43=(void*)_tmp42.elt_type;
_tmp44=_tmp42.tq;_LL29:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4D;_tmp4D.tag=28;_tmp4D.f1=des;_tmp4D;});_tmp4C;})));goto _LL25;_LL2A: if(*((
int*)_tmp3F)!= 11)goto _LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp4F;_tmp4F.tag=31;_tmp4F.f1=(void*)t;_tmp4F.f2=des;_tmp4F;});_tmp4E;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Array_e_struct
_tmp51;_tmp51.tag=28;_tmp51.f1=des;_tmp51;});_tmp50;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_Tcutil_terr(e->loc,({const char*
_tmp53="test of %s has type %s instead of integral or * type";_tag_dynforward(
_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,53));}),_tag_dynforward(_tmp52,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*
e){void*t;{union Cyc_Absyn_Cnst_union _tmp56=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*
_tmp5C;int _tmp5D;struct _dynforward_ptr _tmp5E;_LL34: if((_tmp56.Char_c).tag != 0)
goto _LL36;_tmp57=(_tmp56.Char_c).f1;if((int)_tmp57 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;
goto _LL33;_LL36: if((_tmp56.Char_c).tag != 0)goto _LL38;_tmp58=(_tmp56.Char_c).f1;
if((int)_tmp58 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if((
_tmp56.Char_c).tag != 0)goto _LL3A;_tmp59=(_tmp56.Char_c).f1;if((int)_tmp59 != 2)
goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((_tmp56.Short_c).tag != 1)
goto _LL3C;_tmp5A=(_tmp56.Short_c).f1;_LL3B: t=_tmp5A == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp56.LongLong_c).tag != 3)goto _LL3E;
_tmp5B=(_tmp56.LongLong_c).f1;_LL3D: t=_tmp5B == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if((_tmp56.Float_c).tag != 4)goto _LL40;
_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;_LL40: if((_tmp56.Int_c).tag != 2)goto _LL42;
_tmp5C=(_tmp56.Int_c).f1;_tmp5D=(_tmp56.Int_c).f2;_LL41: if(topt == 0)t=_tmp5C == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp5F=Cyc_Tcutil_compress(*
topt);void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;
void*_tmp66;void*_tmp67;void*_tmp68;_LL47: if(_tmp5F <= (void*)4)goto _LL53;if(*((
int*)_tmp5F)!= 5)goto _LL49;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp61 != 0)goto _LL49;_LL48:{void*_tmp69=_tmp60;_LL56: if((int)_tmp69 != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp69 != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp69 != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6A;(_tmp6A.Char_c).tag=0;(_tmp6A.Char_c).f1=(void*)_tmp60;(_tmp6A.Char_c).f2=(
char)_tmp5D;_tmp6A;});goto _LL46;_LL49: if(*((int*)_tmp5F)!= 5)goto _LL4B;_tmp62=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp63 != 1)goto _LL4B;_LL4A: t=_tmp62 == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6B;(_tmp6B.Short_c).tag=1;(_tmp6B.Short_c).f1=(void*)_tmp62;(_tmp6B.Short_c).f2=(
short)_tmp5D;_tmp6B;});goto _LL46;_LL4B: if(*((int*)_tmp5F)!= 5)goto _LL4D;_tmp64=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp65 != 2)goto _LL4D;_LL4C: t=_tmp64 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp5F)!= 5)goto _LL4F;_tmp66=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp67 != 3)goto _LL4F;_LL4E: t=_tmp66 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp5F)!= 4)goto _LL51;if(!(_tmp5D
== 0))goto _LL51;_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6D;_tmp6D.tag=0;_tmp6D.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6E;(_tmp6E.Null_c).tag=6;_tmp6E;});_tmp6D;});_tmp6C;})));{struct Cyc_List_List*
_tmp6F=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_PointerType_struct
_tmp71;_tmp71.tag=4;_tmp71.f1=({struct Cyc_Absyn_PtrInfo _tmp72;_tmp72.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->v=_tmp6F;_tmp75;}));_tmp72.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp72.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp73;_tmp73.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp74=_cycalloc(
sizeof(*_tmp74));_tmp74->v=_tmp6F;_tmp74;}));_tmp73.nullable=Cyc_Absyn_true_conref;
_tmp73.bounds=Cyc_Absyn_empty_conref();_tmp73.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp73.ptrloc=0;_tmp73;});_tmp72;});_tmp71;});_tmp70;});
goto _LL46;}_LL51: if(*((int*)_tmp5F)!= 18)goto _LL53;_tmp68=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp5F)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp76=({struct Cyc_Absyn_TypeInt_struct*
_tmp7D=_cycalloc_atomic(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp7E;_tmp7E.tag=19;_tmp7E.f1=_tmp5D;_tmp7E;});_tmp7D;});if(!Cyc_Tcutil_unify(
_tmp68,(void*)_tmp76)){({struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp76));{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp68));{void*
_tmp77[2]={& _tmp79,& _tmp7A};Cyc_Tcutil_terr(loc,({const char*_tmp78="expecting %s but found %s";
_tag_dynforward(_tmp78,sizeof(char),_get_zero_arr_size(_tmp78,26));}),
_tag_dynforward(_tmp77,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[
0]=({struct Cyc_Absyn_TagType_struct _tmp7C;_tmp7C.tag=18;_tmp7C.f1=(void*)((void*)
_tmp76);_tmp7C;});_tmp7B;});goto _LL46;}_LL53:;_LL54: t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if((_tmp56.String_c).tag != 
5)goto _LL44;_tmp5E=(_tmp56.String_c).f1;_LL43: {int len=(int)_get_dynforward_size(
_tmp5E,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp89;(_tmp89.Int_c).tag=2;(_tmp89.Int_c).f1=(void*)((
void*)1);(_tmp89.Int_c).f2=len;_tmp89;}),loc);elen->topt=({struct Cyc_Core_Opt*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->v=(void*)Cyc_Absyn_uint_typ;_tmp7F;});
t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp81;_tmp81.tag=0;_tmp81.f1=elen;_tmp81;});
_tmp80;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmp82=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmp83;struct Cyc_Absyn_Tqual _tmp84;_LL5D: if(
_tmp82 <= (void*)4)goto _LL61;if(*((int*)_tmp82)!= 7)goto _LL5F;_tmp83=((struct Cyc_Absyn_ArrayType_struct*)
_tmp82)->f1;_tmp84=_tmp83.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmp84,(struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp82)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->v=(void*)t;_tmp85;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86[0]=({struct Cyc_Absyn_Upper_b_struct _tmp87;_tmp87.tag=0;_tmp87.f1=elen;
_tmp87;});_tmp86;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88->v=(void*)t;_tmp88;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((_tmp56.Null_c).tag
!= 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8A=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B[0]=({struct Cyc_Absyn_PointerType_struct _tmp8C;_tmp8C.tag=4;_tmp8C.f1=({
struct Cyc_Absyn_PtrInfo _tmp8D;_tmp8D.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90->v=_tmp8A;_tmp90;}));_tmp8D.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp8D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp8E;_tmp8E.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp8F=_cycalloc(
sizeof(*_tmp8F));_tmp8F->v=_tmp8A;_tmp8F;}));_tmp8E.nullable=Cyc_Absyn_true_conref;
_tmp8E.bounds=Cyc_Absyn_empty_conref();_tmp8E.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp8E.ptrloc=0;_tmp8E;});_tmp8D;});_tmp8C;});_tmp8B;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp91=b;struct Cyc_Absyn_Vardecl*
_tmp92;struct Cyc_Absyn_Fndecl*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;_LL64: if((int)_tmp91 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp97[
1]={& _tmp99};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp98="undeclared identifier: %s";
_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,26));}),
_tag_dynforward(_tmp97,sizeof(void*),1));}});_LL66: if(_tmp91 <= (void*)1)goto
_LL68;if(*((int*)_tmp91)!= 0)goto _LL68;_tmp92=((struct Cyc_Absyn_Global_b_struct*)
_tmp91)->f1;_LL67:*q=*_tmp92->name;return(void*)_tmp92->type;_LL68: if(_tmp91 <= (
void*)1)goto _LL6A;if(*((int*)_tmp91)!= 1)goto _LL6A;_tmp93=((struct Cyc_Absyn_Funname_b_struct*)
_tmp91)->f1;_LL69:*q=*_tmp93->name;return Cyc_Tcutil_fndecl2typ(_tmp93);_LL6A: if(
_tmp91 <= (void*)1)goto _LL6C;if(*((int*)_tmp91)!= 4)goto _LL6C;_tmp94=((struct Cyc_Absyn_Pat_b_struct*)
_tmp91)->f1;_LL6B: _tmp95=_tmp94;goto _LL6D;_LL6C: if(_tmp91 <= (void*)1)goto _LL6E;
if(*((int*)_tmp91)!= 3)goto _LL6E;_tmp95=((struct Cyc_Absyn_Local_b_struct*)_tmp91)->f1;
_LL6D: _tmp96=_tmp95;goto _LL6F;_LL6E: if(_tmp91 <= (void*)1)goto _LL63;if(*((int*)
_tmp91)!= 2)goto _LL63;_tmp96=((struct Cyc_Absyn_Param_b_struct*)_tmp91)->f1;_LL6F:(*
q).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp9A;(_tmp9A.Loc_n).tag=
0;_tmp9A;});return(void*)_tmp96->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmp9B=(void*)fmt->r;union Cyc_Absyn_Cnst_union
_tmp9C;struct _dynforward_ptr _tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp
_tmp9F;void*_tmpA0;union Cyc_Absyn_Cnst_union _tmpA1;struct _dynforward_ptr _tmpA2;
_LL71: if(*((int*)_tmp9B)!= 0)goto _LL73;_tmp9C=((struct Cyc_Absyn_Const_e_struct*)
_tmp9B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp9B)->f1).String_c).tag != 
5)goto _LL73;_tmp9D=(_tmp9C.String_c).f1;_LL72: _tmpA2=_tmp9D;goto _LL74;_LL73: if(*((
int*)_tmp9B)!= 15)goto _LL75;_tmp9E=((struct Cyc_Absyn_Cast_e_struct*)_tmp9B)->f2;
_tmp9F=*_tmp9E;_tmpA0=(void*)_tmp9F.r;if(*((int*)_tmpA0)!= 0)goto _LL75;_tmpA1=((
struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpA0)->f1).String_c).tag != 5)goto _LL75;_tmpA2=(_tmpA1.String_c).f1;_LL74:
desc_types=type_getter(temp,te,(struct _dynforward_ptr)_tmpA2,fmt->loc);goto _LL70;
_LL75:;_LL76: if(opt_args != 0){struct Cyc_List_List*_tmpA3=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpA3->hd);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->topt))->v) && !Cyc_Tcutil_is_unique_path((
struct Cyc_Absyn_Exp*)_tmpA3->hd))({struct Cyc_Int_pa_struct _tmpA6;_tmpA6.tag=1;
_tmpA6.f1=(unsigned long)333;{void*_tmpA4[1]={& _tmpA6};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA3->hd)->loc,({const char*_tmpA5="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmpA5,sizeof(char),_get_zero_arr_size(_tmpA5,41));}),
_tag_dynforward(_tmpA4,sizeof(void*),1));}});}}return;_LL70:;}if(opt_args != 0){
struct Cyc_List_List*_tmpA7=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 
0  && _tmpA7 != 0;(desc_types=desc_types->tl,_tmpA7=_tmpA7->tl)){void*t=(void*)
desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpA7->hd;Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpA8[2]={& _tmpAA,& _tmpAB};Cyc_Tcutil_terr(e->loc,({const char*_tmpA9="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpA9,sizeof(char),_get_zero_arr_size(_tmpA9,51));}),
_tag_dynforward(_tmpA8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(
Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e))({struct Cyc_Int_pa_struct
_tmpAE;_tmpAE.tag=1;_tmpAE.f1=(unsigned long)352;{void*_tmpAC[1]={& _tmpAE};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmpA7->hd)->loc,({const char*_tmpAD="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,41));}),
_tag_dynforward(_tmpAC,sizeof(void*),1));}});}if(desc_types != 0)({void*_tmpAF[0]={};
Cyc_Tcutil_terr(fmt->loc,({const char*_tmpB0="too few arguments";_tag_dynforward(
_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,18));}),_tag_dynforward(_tmpAF,
sizeof(void*),0));});if(_tmpA7 != 0)({void*_tmpB1[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA7->hd)->loc,({const char*_tmpB2="too many arguments";
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,19));}),
_tag_dynforward(_tmpB1,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB3=p;_LL78: if((int)_tmpB3 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB3 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB4[1]={& _tmpB6};Cyc_Tcutil_terr(
loc,({const char*_tmpB5="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,39));}),_tag_dynforward(_tmpB4,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB3 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=
0;_tmpB9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB7[1]={& _tmpB9};Cyc_Tcutil_terr(loc,({const char*_tmpB8="expecting integral or * type but found %s";
_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,42));}),
_tag_dynforward(_tmpB7,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB3 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpBA[1]={& _tmpBC};Cyc_Tcutil_terr(loc,({const char*_tmpBB="expecting integral type but found %s";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,37));}),
_tag_dynforward(_tmpBA,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB3 != 19)goto _LL82;_LL81:{void*_tmpBD=
t;struct Cyc_Absyn_PtrInfo _tmpBE;struct Cyc_Absyn_PtrAtts _tmpBF;struct Cyc_Absyn_Conref*
_tmpC0;_LL85: if(_tmpBD <= (void*)4)goto _LL89;if(*((int*)_tmpBD)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpBD)!= 4)goto _LL89;_tmpBE=((struct Cyc_Absyn_PointerType_struct*)
_tmpBD)->f1;_tmpBF=_tmpBE.ptr_atts;_tmpC0=_tmpBF.bounds;_LL88:{union Cyc_Absyn_Constraint_union
_tmpC1=(Cyc_Absyn_compress_conref(_tmpC0))->v;void*_tmpC2;void*_tmpC3;void*
_tmpC4;_LL8C: if((_tmpC1.Eq_constr).tag != 0)goto _LL8E;_tmpC2=(_tmpC1.Eq_constr).f1;
if((int)_tmpC2 != 0)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if((_tmpC1.Eq_constr).tag != 0)
goto _LL90;_tmpC3=(_tmpC1.Eq_constr).f1;if((int)_tmpC3 != 1)goto _LL90;_LL8F: goto
_LL91;_LL90: if((_tmpC1.Eq_constr).tag != 0)goto _LL92;_tmpC4=(_tmpC1.Eq_constr).f1;
if(_tmpC4 <= (void*)2)goto _LL92;if(*((int*)_tmpC4)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC5[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC6="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,53));}),
_tag_dynforward(_tmpC5,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC7[1]={& _tmpC9};Cyc_Tcutil_terr(
loc,({const char*_tmpC8="size requires pointer or array type, not %s";
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,44));}),
_tag_dynforward(_tmpC7,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpCA[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCB="Non-unary primop";
_tag_dynforward(_tmpCB,sizeof(char),_get_zero_arr_size(_tmpCB,17));}),
_tag_dynforward(_tmpCA,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpCC[1]={& _tmpCE};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpCD="type %s cannot be used here";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,28));}),
_tag_dynforward(_tmpCC,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD1;_tmpD1.tag=0;_tmpD1.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpCF[1]={& _tmpD1};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpD0="type %s cannot be used here";_tag_dynforward(_tmpD0,
sizeof(char),_get_zero_arr_size(_tmpD0,28));}),_tag_dynforward(_tmpCF,sizeof(
void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpD2=t1;struct Cyc_Absyn_PtrInfo
_tmpD3;void*_tmpD4;struct Cyc_Absyn_Tqual _tmpD5;struct Cyc_Absyn_PtrAtts _tmpD6;
void*_tmpD7;struct Cyc_Absyn_Conref*_tmpD8;struct Cyc_Absyn_Conref*_tmpD9;struct
Cyc_Absyn_Conref*_tmpDA;_LL95: if(_tmpD2 <= (void*)4)goto _LL97;if(*((int*)_tmpD2)
!= 4)goto _LL97;_tmpD3=((struct Cyc_Absyn_PointerType_struct*)_tmpD2)->f1;_tmpD4=(
void*)_tmpD3.elt_typ;_tmpD5=_tmpD3.elt_tq;_tmpD6=_tmpD3.ptr_atts;_tmpD7=(void*)
_tmpD6.rgn;_tmpD8=_tmpD6.nullable;_tmpD9=_tmpD6.bounds;_tmpDA=_tmpD6.zero_term;
_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD4),(void*)1))({void*
_tmpDB[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDC="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,50));}),
_tag_dynforward(_tmpDB,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpDD[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDE="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,48));}),
_tag_dynforward(_tmpDD,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpDF[1]={& _tmpE1};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpE0="expecting int but found %s";_tag_dynforward(_tmpE0,
sizeof(char),_get_zero_arr_size(_tmpE0,27));}),_tag_dynforward(_tmpDF,sizeof(
void*),1));}});_tmpD9=Cyc_Absyn_compress_conref(_tmpD9);{union Cyc_Absyn_Constraint_union
_tmpE2=_tmpD9->v;void*_tmpE3;void*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;
void*_tmpE7;_LL9A: if((_tmpE2.Eq_constr).tag != 0)goto _LL9C;_tmpE3=(_tmpE2.Eq_constr).f1;
if((int)_tmpE3 != 0)goto _LL9C;_LL9B: return t1;_LL9C: if((_tmpE2.Eq_constr).tag != 0)
goto _LL9E;_tmpE4=(_tmpE2.Eq_constr).f1;if((int)_tmpE4 != 1)goto _LL9E;_LL9D: return
t1;_LL9E: if((_tmpE2.Eq_constr).tag != 0)goto _LLA0;_tmpE5=(_tmpE2.Eq_constr).f1;
if(_tmpE5 <= (void*)2)goto _LLA0;if(*((int*)_tmpE5)!= 0)goto _LLA0;_tmpE6=((struct
Cyc_Absyn_Upper_b_struct*)_tmpE5)->f1;_LL9F: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpDA))({void*_tmpE8[0]={};Cyc_Tcutil_warn(e1->loc,({
const char*_tmpE9="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,70));}),
_tag_dynforward(_tmpE8,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpEA=({struct Cyc_Absyn_PointerType_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));
_tmpEB[0]=({struct Cyc_Absyn_PointerType_struct _tmpEC;_tmpEC.tag=4;_tmpEC.f1=({
struct Cyc_Absyn_PtrInfo _tmpED;_tmpED.elt_typ=(void*)_tmpD4;_tmpED.elt_tq=_tmpD5;
_tmpED.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpEE;_tmpEE.rgn=(void*)_tmpD7;_tmpEE.nullable=
Cyc_Absyn_true_conref;_tmpEE.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpEE.zero_term=
_tmpDA;_tmpEE.ptrloc=0;_tmpEE;});_tmpED;});_tmpEC;});_tmpEB;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpEA,(void*)3);return(void*)_tmpEA;}_LLA0: if((_tmpE2.Eq_constr).tag
!= 0)goto _LLA2;_tmpE7=(_tmpE2.Eq_constr).f1;if(_tmpE7 <= (void*)2)goto _LLA2;if(*((
int*)_tmpE7)!= 1)goto _LLA2;_LLA1:({void*_tmpEF[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpF0="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,64));}),
_tag_dynforward(_tmpEF,sizeof(void*),0));});return t1;_LLA2:;_LLA3: _tmpD9->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmpF1;(_tmpF1.Eq_constr).tag=
0;(_tmpF1.Eq_constr).f1=(void*)((void*)0);_tmpF1;});return t1;_LL99:;}_LL97:;
_LL98: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF5;
_tmpF5.tag=0;_tmpF5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF2[2]={& _tmpF4,&
_tmpF5};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF3="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,59));}),
_tag_dynforward(_tmpF2,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpF6[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpF7="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,43));}),
_tag_dynforward(_tmpF6,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpF8[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF9="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,50));}),
_tag_dynforward(_tmpF8,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpFA[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFB="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,48));}),
_tag_dynforward(_tmpFA,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({
struct Cyc_String_pa_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmpFC[2]={& _tmpFE,& _tmpFF};Cyc_Tcutil_terr(e2->loc,({const char*
_tmpFD="expecting either %s or int but found %s";_tag_dynforward(_tmpFD,sizeof(
char),_get_zero_arr_size(_tmpFD,40));}),_tag_dynforward(_tmpFC,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && (Cyc_Tcutil_typ_kind(t1)== (
void*)2  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,
Cyc_Tcenv_lookup_opt_type_vars(te)))))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(
te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{{struct
_tuple0 _tmp101=({struct _tuple0 _tmp100;_tmp100.f1=Cyc_Tcutil_compress(t1);_tmp100.f2=
Cyc_Tcutil_compress(t2);_tmp100;});void*_tmp102;struct Cyc_Absyn_PtrInfo _tmp103;
void*_tmp104;void*_tmp105;struct Cyc_Absyn_PtrInfo _tmp106;void*_tmp107;_LLA5:
_tmp102=_tmp101.f1;if(_tmp102 <= (void*)4)goto _LLA7;if(*((int*)_tmp102)!= 4)goto
_LLA7;_tmp103=((struct Cyc_Absyn_PointerType_struct*)_tmp102)->f1;_tmp104=(void*)
_tmp103.elt_typ;_tmp105=_tmp101.f2;if(_tmp105 <= (void*)4)goto _LLA7;if(*((int*)
_tmp105)!= 4)goto _LLA7;_tmp106=((struct Cyc_Absyn_PointerType_struct*)_tmp105)->f1;
_tmp107=(void*)_tmp106.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp104,_tmp107))return
Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({struct Cyc_String_pa_struct
_tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp10A;_tmp10A.tag=0;
_tmp10A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp108[2]={& _tmp10A,& _tmp10B};Cyc_Tcutil_terr(loc,({const char*
_tmp109="comparison not allowed between %s and %s";_tag_dynforward(_tmp109,
sizeof(char),_get_zero_arr_size(_tmp109,41));}),_tag_dynforward(_tmp108,sizeof(
void*),2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*
_tmp10C=p;_LLAA: if((int)_tmp10C != 0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,
e2);_LLAC: if((int)_tmp10C != 2)goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);
_LLAE: if((int)_tmp10C != 1)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp10C != 3)
goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLB2: if((int)_tmp10C != 4)goto _LLB4;_LLB3: goto _LLB5;_LLB4: if((int)_tmp10C != 13)
goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp10C != 14)goto _LLB8;_LLB7: goto _LLB9;
_LLB8: if((int)_tmp10C != 15)goto _LLBA;_LLB9: goto _LLBB;_LLBA: if((int)_tmp10C != 16)
goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp10C != 17)goto _LLBE;_LLBD: goto _LLBF;
_LLBE: if((int)_tmp10C != 18)goto _LLC0;_LLBF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_integral);_LLC0: if((int)_tmp10C != 5)goto _LLC2;_LLC1: goto _LLC3;
_LLC2: if((int)_tmp10C != 6)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp10C != 7)
goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)_tmp10C != 8)goto _LLC8;_LLC7: goto _LLC9;
_LLC8: if((int)_tmp10C != 9)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)_tmp10C != 10)
goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLCC:;_LLCD:({void*
_tmp10D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp10E="bad binary primop";_tag_dynforward(_tmp10E,sizeof(char),
_get_zero_arr_size(_tmp10E,18));}),_tag_dynforward(_tmp10D,sizeof(void*),0));});
_LLA9:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLCE: return({void*_tmp10F[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp110="primitive operator has 0 arguments";_tag_dynforward(_tmp110,sizeof(char),
_get_zero_arr_size(_tmp110,35));}),_tag_dynforward(_tmp10F,sizeof(void*),0));});
case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLD1: return({void*_tmp111[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp112="primitive operator has > 2 arguments";
_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,37));}),
_tag_dynforward(_tmp111,sizeof(void*),0));});}return t;}}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){{void*_tmp113=(void*)e->r;void*_tmp114;struct Cyc_Absyn_Vardecl*
_tmp115;void*_tmp116;struct Cyc_Absyn_Vardecl*_tmp117;void*_tmp118;struct Cyc_Absyn_Vardecl*
_tmp119;void*_tmp11A;struct Cyc_Absyn_Vardecl*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;
struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct _dynforward_ptr*
_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct _dynforward_ptr*_tmp121;struct Cyc_Absyn_Exp*
_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;_LLD4: if(*((int*)
_tmp113)!= 1)goto _LLD6;_tmp114=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp113)->f2;
if(_tmp114 <= (void*)1)goto _LLD6;if(*((int*)_tmp114)!= 2)goto _LLD6;_tmp115=((
struct Cyc_Absyn_Param_b_struct*)_tmp114)->f1;_LLD5: _tmp117=_tmp115;goto _LLD7;
_LLD6: if(*((int*)_tmp113)!= 1)goto _LLD8;_tmp116=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp113)->f2;if(_tmp116 <= (void*)1)goto _LLD8;if(*((int*)_tmp116)!= 3)goto _LLD8;
_tmp117=((struct Cyc_Absyn_Local_b_struct*)_tmp116)->f1;_LLD7: _tmp119=_tmp117;
goto _LLD9;_LLD8: if(*((int*)_tmp113)!= 1)goto _LLDA;_tmp118=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp113)->f2;if(_tmp118 <= (void*)1)goto _LLDA;if(*((int*)_tmp118)!= 4)goto _LLDA;
_tmp119=((struct Cyc_Absyn_Pat_b_struct*)_tmp118)->f1;_LLD9: _tmp11B=_tmp119;goto
_LLDB;_LLDA: if(*((int*)_tmp113)!= 1)goto _LLDC;_tmp11A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp113)->f2;if(_tmp11A <= (void*)1)goto _LLDC;if(*((int*)_tmp11A)!= 0)goto _LLDC;
_tmp11B=((struct Cyc_Absyn_Global_b_struct*)_tmp11A)->f1;_LLDB: if(!(_tmp11B->tq).real_const)
return;goto _LLD3;_LLDC: if(*((int*)_tmp113)!= 25)goto _LLDE;_tmp11C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp113)->f1;_tmp11D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp113)->f2;_LLDD:{
void*_tmp125=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_Tqual _tmp127;
struct Cyc_Absyn_ArrayInfo _tmp128;struct Cyc_Absyn_Tqual _tmp129;struct Cyc_List_List*
_tmp12A;_LLEB: if(_tmp125 <= (void*)4)goto _LLF1;if(*((int*)_tmp125)!= 4)goto _LLED;
_tmp126=((struct Cyc_Absyn_PointerType_struct*)_tmp125)->f1;_tmp127=_tmp126.elt_tq;
_LLEC: _tmp129=_tmp127;goto _LLEE;_LLED: if(*((int*)_tmp125)!= 7)goto _LLEF;_tmp128=((
struct Cyc_Absyn_ArrayType_struct*)_tmp125)->f1;_tmp129=_tmp128.tq;_LLEE: if(!
_tmp129.real_const)return;goto _LLEA;_LLEF: if(*((int*)_tmp125)!= 9)goto _LLF1;
_tmp12A=((struct Cyc_Absyn_TupleType_struct*)_tmp125)->f1;_LLF0: {unsigned int
_tmp12C;int _tmp12D;struct _tuple7 _tmp12B=Cyc_Evexp_eval_const_uint_exp(_tmp11D);
_tmp12C=_tmp12B.f1;_tmp12D=_tmp12B.f2;if(!_tmp12D){({void*_tmp12E[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp12F="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,47));}),
_tag_dynforward(_tmp12E,sizeof(void*),0));});return;}{struct _handler_cons _tmp130;
_push_handler(& _tmp130);{int _tmp132=0;if(setjmp(_tmp130.handler))_tmp132=1;if(!
_tmp132){{struct _tuple9 _tmp134;struct Cyc_Absyn_Tqual _tmp135;struct _tuple9*
_tmp133=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp12A,(
int)_tmp12C);_tmp134=*_tmp133;_tmp135=_tmp134.f1;if(!_tmp135.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp131=(void*)_exn_thrown;
void*_tmp137=_tmp131;_LLF4: if(_tmp137 != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp137);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp113)!= 23)goto _LLE0;_tmp11E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp113)->f1;_tmp11F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp113)->f2;_LLDF:{
void*_tmp138=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11E->topt))->v);struct Cyc_Absyn_AggrInfo _tmp139;union Cyc_Absyn_AggrInfoU_union
_tmp13A;struct Cyc_Absyn_Aggrdecl**_tmp13B;struct Cyc_List_List*_tmp13C;_LLF9: if(
_tmp138 <= (void*)4)goto _LLFD;if(*((int*)_tmp138)!= 10)goto _LLFB;_tmp139=((struct
Cyc_Absyn_AggrType_struct*)_tmp138)->f1;_tmp13A=_tmp139.aggr_info;if((((((struct
Cyc_Absyn_AggrType_struct*)_tmp138)->f1).aggr_info).KnownAggr).tag != 1)goto _LLFB;
_tmp13B=(_tmp13A.KnownAggr).f1;_LLFA: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp13B == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp13B,_tmp11F);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF8;}_LLFB: if(*((int*)_tmp138)!= 11)goto _LLFD;_tmp13C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp138)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp13C,
_tmp11F);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp113)!= 24)goto _LLE2;_tmp120=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp113)->f1;_tmp121=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp113)->f2;_LLE1:{void*_tmp13D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp120->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13E;void*_tmp13F;
_LL100: if(_tmp13D <= (void*)4)goto _LL102;if(*((int*)_tmp13D)!= 4)goto _LL102;
_tmp13E=((struct Cyc_Absyn_PointerType_struct*)_tmp13D)->f1;_tmp13F=(void*)
_tmp13E.elt_typ;_LL101:{void*_tmp140=Cyc_Tcutil_compress(_tmp13F);struct Cyc_Absyn_AggrInfo
_tmp141;union Cyc_Absyn_AggrInfoU_union _tmp142;struct Cyc_Absyn_Aggrdecl**_tmp143;
struct Cyc_List_List*_tmp144;_LL105: if(_tmp140 <= (void*)4)goto _LL109;if(*((int*)
_tmp140)!= 10)goto _LL107;_tmp141=((struct Cyc_Absyn_AggrType_struct*)_tmp140)->f1;
_tmp142=_tmp141.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp140)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL107;_tmp143=(_tmp142.KnownAggr).f1;_LL106: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp143 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp143,_tmp121);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL107: if(*((
int*)_tmp140)!= 11)goto _LL109;_tmp144=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp140)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp144,_tmp121);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL109:;
_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103: goto _LLFF;_LLFF:;}goto _LLD3;
_LLE2: if(*((int*)_tmp113)!= 22)goto _LLE4;_tmp122=((struct Cyc_Absyn_Deref_e_struct*)
_tmp113)->f1;_LLE3:{void*_tmp145=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp122->topt))->v);struct Cyc_Absyn_PtrInfo _tmp146;struct Cyc_Absyn_Tqual
_tmp147;struct Cyc_Absyn_ArrayInfo _tmp148;struct Cyc_Absyn_Tqual _tmp149;_LL10C: if(
_tmp145 <= (void*)4)goto _LL110;if(*((int*)_tmp145)!= 4)goto _LL10E;_tmp146=((
struct Cyc_Absyn_PointerType_struct*)_tmp145)->f1;_tmp147=_tmp146.elt_tq;_LL10D:
_tmp149=_tmp147;goto _LL10F;_LL10E: if(*((int*)_tmp145)!= 7)goto _LL110;_tmp148=((
struct Cyc_Absyn_ArrayType_struct*)_tmp145)->f1;_tmp149=_tmp148.tq;_LL10F: if(!
_tmp149.real_const)return;goto _LL10B;_LL110:;_LL111: goto _LL10B;_LL10B:;}goto
_LLD3;_LLE4: if(*((int*)_tmp113)!= 13)goto _LLE6;_tmp123=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp113)->f1;_LLE5: _tmp124=_tmp123;goto _LLE7;_LLE6: if(*((int*)_tmp113)!= 14)goto
_LLE8;_tmp124=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp113)->f1;_LLE7: Cyc_Tcexp_check_writable(
te,_tmp124);return;_LLE8:;_LLE9: goto _LLD3;_LLD3:;}({struct Cyc_String_pa_struct
_tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp14A[1]={& _tmp14C};Cyc_Tcutil_terr(e->loc,({
const char*_tmp14B="attempt to write a const location: %s";_tag_dynforward(
_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,38));}),_tag_dynforward(_tmp14A,
sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp14D[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp14E="increment/decrement of non-lvalue";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,34));}),
_tag_dynforward(_tmp14D,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp14F[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp150="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,50));}),
_tag_dynforward(_tmp14F,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t))({
void*_tmp151[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp152="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,48));}),
_tag_dynforward(_tmp151,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp153[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp154="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp154,
sizeof(char),_get_zero_arr_size(_tmp154,43));}),_tag_dynforward(_tmp153,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp155[1]={& _tmp157};Cyc_Tcutil_terr(e->loc,({const char*_tmp156="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,44));}),
_tag_dynforward(_tmp155,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp158="conditional expression";_tag_dynforward(_tmp158,sizeof(char),
_get_zero_arr_size(_tmp158,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp159=({struct Cyc_List_List _tmp160;_tmp160.hd=e3;
_tmp160.tl=0;_tmp160;});struct Cyc_List_List _tmp15A=({struct Cyc_List_List _tmp15F;
_tmp15F.hd=e2;_tmp15F.tl=(struct Cyc_List_List*)& _tmp159;_tmp15F;});if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp15A)){({struct Cyc_String_pa_struct _tmp15E;
_tmp15E.tag=0;_tmp15E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp15B[2]={& _tmp15D,& _tmp15E};Cyc_Tcutil_terr(loc,({const char*_tmp15C="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,48));}),
_tag_dynforward(_tmp15B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp161="logical-and expression";_tag_dynforward(_tmp161,sizeof(char),
_get_zero_arr_size(_tmp161,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp162="logical-and expression";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp163="logical-or expression";_tag_dynforward(_tmp163,sizeof(char),
_get_zero_arr_size(_tmp163,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp164="logical-or expression";
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp165=_new_region("r");struct
_RegionHandle*r=& _tmp165;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp166=Cyc_Tcutil_compress(t1);_LL113: if(_tmp166 <= (void*)4)goto _LL115;if(*((
int*)_tmp166)!= 7)goto _LL115;_LL114:({void*_tmp167[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp168="cannot assign to an array";_tag_dynforward(_tmp168,sizeof(
char),_get_zero_arr_size(_tmp168,26));}),_tag_dynforward(_tmp167,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp169[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp16A="type is abstract (can't determine size).";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,41));}),
_tag_dynforward(_tmp169,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp16B[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16C="assignment to non-lvalue";
_tag_dynforward(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,25));}),
_tag_dynforward(_tmp16B,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(Cyc_Tcutil_is_unique_pointer(t2) && !Cyc_Tcutil_is_unique_path(e2))({
struct Cyc_Int_pa_struct _tmp16F;_tmp16F.tag=1;_tmp16F.f1=(unsigned long)746;{void*
_tmp16D[1]={& _tmp16F};Cyc_Tcutil_terr(e2->loc,({const char*_tmp16E="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,41));}),
_tag_dynforward(_tmp16D,sizeof(void*),1));}});if(!Cyc_Tcutil_coerce_assign(te,e2,
t1)){void*_tmp170=({struct Cyc_String_pa_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp173;_tmp173.tag=0;_tmp173.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp171[2]={& _tmp173,& _tmp174};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp172="type mismatch: %s != %s";_tag_dynforward(_tmp172,sizeof(char),
_get_zero_arr_size(_tmp172,24));}),_tag_dynforward(_tmp171,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp170;}}else{void*
_tmp175=(void*)po->v;void*_tmp176=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp175,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp176,t1) || Cyc_Tcutil_coerceable(_tmp176) && Cyc_Tcutil_coerceable(
t1))){void*_tmp177=({struct Cyc_String_pa_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp178[2]={& _tmp17A,& _tmp17B};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp179="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,82));}),
_tag_dynforward(_tmp178,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp176,t1);Cyc_Tcutil_explain_failure();
return _tmp177;}return _tmp176;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp17C=Cyc_Tcutil_compress(
t1);void*_tmp17D;void*_tmp17E;_LL118: if((int)_tmp17C != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp17C <= (void*)4)goto
_LL11E;if(*((int*)_tmp17C)!= 5)goto _LL11C;_tmp17D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp17C)->f2;if((int)_tmp17D != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp17C)!= 5)goto _LL11E;_tmp17E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp17C)->f2;
if((int)_tmp17E != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp180;struct Cyc_List_List*
_tmp181;struct Cyc_Position_Segment*_tmp182;void*_tmp183;struct Cyc_Absyn_Tunionfield
_tmp17F=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp180=_tmp17F.name;_tmp181=
_tmp17F.typs;_tmp182=_tmp17F.loc;_tmp183=(void*)_tmp17F.sc;if(_tmp181 == 0  || 
_tmp181->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp181->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Position_Segment*
_tmp187;void*_tmp188;struct Cyc_Absyn_Tunionfield _tmp184=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp185=_tmp184.name;_tmp186=_tmp184.typs;_tmp187=_tmp184.loc;
_tmp188=(void*)_tmp184.sc;if(_tmp186 == 0  || _tmp186->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp186->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp189[2]={& _tmp18B,& _tmp18C};Cyc_Tcutil_terr(e->loc,({const char*_tmp18A="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,54));}),
_tag_dynforward(_tmp189,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp18E=args;struct _RegionHandle _tmp18F=_new_region("ter");
struct _RegionHandle*ter=& _tmp18F;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp190=
Cyc_Tcenv_new_block(ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp190,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp191=t;struct Cyc_Absyn_PtrInfo
_tmp192;void*_tmp193;struct Cyc_Absyn_Tqual _tmp194;struct Cyc_Absyn_PtrAtts _tmp195;
void*_tmp196;struct Cyc_Absyn_Conref*_tmp197;struct Cyc_Absyn_Conref*_tmp198;
struct Cyc_Absyn_Conref*_tmp199;_LL121: if(_tmp191 <= (void*)4)goto _LL123;if(*((int*)
_tmp191)!= 4)goto _LL123;_tmp192=((struct Cyc_Absyn_PointerType_struct*)_tmp191)->f1;
_tmp193=(void*)_tmp192.elt_typ;_tmp194=_tmp192.elt_tq;_tmp195=_tmp192.ptr_atts;
_tmp196=(void*)_tmp195.rgn;_tmp197=_tmp195.nullable;_tmp198=_tmp195.bounds;
_tmp199=_tmp195.zero_term;_LL122: Cyc_Tcenv_check_rgn_accessible(_tmp190,loc,
_tmp196);Cyc_Tcutil_check_nonzero_bound(loc,_tmp198);{void*_tmp19A=Cyc_Tcutil_compress(
_tmp193);struct Cyc_Absyn_FnInfo _tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_Core_Opt*
_tmp19D;void*_tmp19E;struct Cyc_List_List*_tmp19F;int _tmp1A0;struct Cyc_Absyn_VarargInfo*
_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A3;_LL126: if(
_tmp19A <= (void*)4)goto _LL128;if(*((int*)_tmp19A)!= 8)goto _LL128;_tmp19B=((
struct Cyc_Absyn_FnType_struct*)_tmp19A)->f1;_tmp19C=_tmp19B.tvars;_tmp19D=
_tmp19B.effect;_tmp19E=(void*)_tmp19B.ret_typ;_tmp19F=_tmp19B.args;_tmp1A0=
_tmp19B.c_varargs;_tmp1A1=_tmp19B.cyc_varargs;_tmp1A2=_tmp19B.rgn_po;_tmp1A3=
_tmp19B.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(_tmp19E,*topt);while(
_tmp18E != 0  && _tmp19F != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18E->hd;
void*t2=(*((struct _tuple2*)_tmp19F->hd)).f3;Cyc_Tcexp_tcExp(_tmp190,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp190,e1,t2)){({struct Cyc_String_pa_struct
_tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1A6;_tmp1A6.tag=0;
_tmp1A6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1A4[2]={&
_tmp1A6,& _tmp1A7};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1A5="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,57));}),
_tag_dynforward(_tmp1A4,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_unique_pointer(
t2) && !Cyc_Tcutil_is_unique_path(e1))({struct Cyc_Int_pa_struct _tmp1AA;_tmp1AA.tag=
1;_tmp1AA.f1=(unsigned long)863;{void*_tmp1A8[1]={& _tmp1AA};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp1A9="Cannot consume paths; do swap instead %d";_tag_dynforward(
_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,41));}),_tag_dynforward(_tmp1A8,
sizeof(void*),1));}});_tmp18E=_tmp18E->tl;_tmp19F=_tmp19F->tl;}{int
args_already_checked=0;{struct Cyc_List_List*a=_tmp1A3;for(0;a != 0;a=a->tl){void*
_tmp1AB=(void*)a->hd;void*_tmp1AC;int _tmp1AD;int _tmp1AE;_LL12B: if(_tmp1AB <= (
void*)17)goto _LL12D;if(*((int*)_tmp1AB)!= 3)goto _LL12D;_tmp1AC=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1AB)->f1;_tmp1AD=((struct Cyc_Absyn_Format_att_struct*)
_tmp1AB)->f2;_tmp1AE=((struct Cyc_Absyn_Format_att_struct*)_tmp1AB)->f3;_LL12C:{
struct _handler_cons _tmp1AF;_push_handler(& _tmp1AF);{int _tmp1B1=0;if(setjmp(
_tmp1AF.handler))_tmp1B1=1;if(!_tmp1B1){{struct Cyc_Absyn_Exp*_tmp1B2=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1AD - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp1AE == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp1B3=
_cycalloc(sizeof(*_tmp1B3));_tmp1B3->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp1AE - 1);_tmp1B3;});}args_already_checked=1;{
struct _RegionHandle _tmp1B4=_new_region("temp");struct _RegionHandle*temp=& _tmp1B4;
_push_region(temp);{void*_tmp1B5=_tmp1AC;_LL130: if((int)_tmp1B5 != 0)goto _LL132;
_LL131: Cyc_Tcexp_check_format_args(_tmp190,_tmp1B2,fmt_args,temp,Cyc_Formatstr_get_format_typs);
goto _LL12F;_LL132: if((int)_tmp1B5 != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(
_tmp190,_tmp1B2,fmt_args,temp,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;};
_pop_region(temp);}};_pop_handler();}else{void*_tmp1B0=(void*)_exn_thrown;void*
_tmp1B7=_tmp1B0;_LL135: if(_tmp1B7 != Cyc_List_Nth)goto _LL137;_LL136:({void*
_tmp1B8[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1B9="bad format arguments";
_tag_dynforward(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,21));}),
_tag_dynforward(_tmp1B8,sizeof(void*),0));});goto _LL134;_LL137:;_LL138:(void)
_throw(_tmp1B7);_LL134:;}}}goto _LL12A;_LL12D:;_LL12E: goto _LL12A;_LL12A:;}}if(
_tmp19F != 0)({void*_tmp1BA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1BB="too few arguments for function";
_tag_dynforward(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,31));}),
_tag_dynforward(_tmp1BA,sizeof(void*),0));});else{if((_tmp18E != 0  || _tmp1A0)
 || _tmp1A1 != 0){if(_tmp1A0)for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){Cyc_Tcexp_tcExp(
_tmp190,0,(struct Cyc_Absyn_Exp*)_tmp18E->hd);}else{if(_tmp1A1 == 0)({void*_tmp1BC[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1BD="too many arguments for function";
_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,32));}),
_tag_dynforward(_tmp1BC,sizeof(void*),0));});else{void*_tmp1BF;int _tmp1C0;struct
Cyc_Absyn_VarargInfo _tmp1BE=*_tmp1A1;_tmp1BF=(void*)_tmp1BE.type;_tmp1C0=_tmp1BE.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp1C1=({struct Cyc_Absyn_VarargCallInfo*_tmp1DF=
_cycalloc(sizeof(*_tmp1DF));_tmp1DF->num_varargs=0;_tmp1DF->injectors=0;_tmp1DF->vai=(
struct Cyc_Absyn_VarargInfo*)_tmp1A1;_tmp1DF;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1C1;if(!_tmp1C0)for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp18E->hd;_tmp1C1->num_varargs ++;Cyc_Tcexp_tcExp(
_tmp190,(void**)& _tmp1BF,e1);if(!Cyc_Tcutil_coerce_arg(_tmp190,e1,_tmp1BF)){({
struct Cyc_String_pa_struct _tmp1C5;_tmp1C5.tag=0;_tmp1C5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v));{struct Cyc_String_pa_struct _tmp1C4;
_tmp1C4.tag=0;_tmp1C4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1BF));{void*_tmp1C2[2]={& _tmp1C4,& _tmp1C5};Cyc_Tcutil_terr(loc,({const char*
_tmp1C3="vararg requires type %s but argument has type %s";_tag_dynforward(
_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,49));}),_tag_dynforward(_tmp1C2,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_unique_pointer(
_tmp1BF) && !Cyc_Tcutil_is_unique_path(e1))({struct Cyc_Int_pa_struct _tmp1C8;
_tmp1C8.tag=1;_tmp1C8.f1=(unsigned long)928;{void*_tmp1C6[1]={& _tmp1C8};Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1C7="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,41));}),
_tag_dynforward(_tmp1C6,sizeof(void*),1));}});}else{void*_tmp1C9=Cyc_Tcutil_compress(
_tmp1BF);struct Cyc_Absyn_TunionInfo _tmp1CA;union Cyc_Absyn_TunionInfoU_union
_tmp1CB;struct Cyc_Absyn_Tuniondecl**_tmp1CC;struct Cyc_Absyn_Tuniondecl*_tmp1CD;
struct Cyc_List_List*_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;_LL13A: if(_tmp1C9 <= (void*)
4)goto _LL13C;if(*((int*)_tmp1C9)!= 2)goto _LL13C;_tmp1CA=((struct Cyc_Absyn_TunionType_struct*)
_tmp1C9)->f1;_tmp1CB=_tmp1CA.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp1C9)->f1).tunion_info).KnownTunion).tag != 1)goto _LL13C;_tmp1CC=(_tmp1CB.KnownTunion).f1;
_tmp1CD=*_tmp1CC;_tmp1CE=_tmp1CA.targs;_tmp1CF=_tmp1CA.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1D0=*Cyc_Tcenv_lookup_tuniondecl(_tmp190,loc,_tmp1CD->name);struct Cyc_List_List*
fields=0;if(_tmp1D0->fields == 0)({struct Cyc_String_pa_struct _tmp1D3;_tmp1D3.tag=
0;_tmp1D3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1BF));{void*_tmp1D1[1]={& _tmp1D3};Cyc_Tcutil_terr(loc,({const char*_tmp1D2="can't inject into %s";
_tag_dynforward(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,21));}),
_tag_dynforward(_tmp1D1,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D0->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CF))->v,Cyc_Tcenv_curr_rgn(_tmp190)))({void*
_tmp1D4[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1D5="bad region for injected varargs";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,32));}),
_tag_dynforward(_tmp1D4,sizeof(void*),0));});{struct _RegionHandle _tmp1D6=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp1D6;_push_region(rgn);{struct Cyc_List_List*
_tmp1D7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1D0->tvs,
_tmp1CE);for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){_tmp1C1->num_varargs ++;{struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18E->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp190,0,e1);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v) && !Cyc_Tcutil_is_unique_path(e1))({struct Cyc_Int_pa_struct
_tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=(unsigned long)954;{void*_tmp1D8[1]={& _tmp1DA};
Cyc_Tcutil_terr(e1->loc,({const char*_tmp1D9="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,41));}),
_tag_dynforward(_tmp1D8,sizeof(void*),1));}});}{struct Cyc_Absyn_Tunionfield*
_tmp1DB=Cyc_Tcexp_tcInjection(_tmp190,e1,_tmp1BF,rgn,_tmp1D7,fields);if(_tmp1DB
!= 0)_tmp1C1->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp1C1->injectors,({struct Cyc_List_List*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1DB;_tmp1DC->tl=0;_tmp1DC;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;
_LL13D:({void*_tmp1DD[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1DE="bad inject vararg type";
_tag_dynforward(_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,23));}),
_tag_dynforward(_tmp1DD,sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp190,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp19D))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp190,loc,_tmp1A2);{void*_tmp1E0=_tmp19E;_npop_handler(0);return _tmp1E0;}}
_LL128:;_LL129: {void*_tmp1E3=({void*_tmp1E1[0]={};Cyc_Tcexp_expr_err(_tmp190,
loc,topt,({const char*_tmp1E2="expected pointer to function";_tag_dynforward(
_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,29));}),_tag_dynforward(_tmp1E1,
sizeof(void*),0));});_npop_handler(0);return _tmp1E3;}_LL125:;}_LL123:;_LL124: {
void*_tmp1E6=({void*_tmp1E4[0]={};Cyc_Tcexp_expr_err(_tmp190,loc,topt,({const
char*_tmp1E5="expected pointer to function";_tag_dynforward(_tmp1E5,sizeof(char),
_get_zero_arr_size(_tmp1E5,29));}),_tag_dynforward(_tmp1E4,sizeof(void*),0));});
_npop_handler(0);return _tmp1E6;}_LL120:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1E7[1]={& _tmp1E9};Cyc_Tcutil_terr(
loc,({const char*_tmp1E8="expected xtunion exn but found %s";_tag_dynforward(
_tmp1E8,sizeof(char),_get_zero_arr_size(_tmp1E8,34));}),_tag_dynforward(_tmp1E7,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1EA=t1;struct Cyc_Absyn_PtrInfo
_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Tqual _tmp1ED;struct Cyc_Absyn_PtrAtts _tmp1EE;
void*_tmp1EF;struct Cyc_Absyn_Conref*_tmp1F0;struct Cyc_Absyn_Conref*_tmp1F1;
struct Cyc_Absyn_Conref*_tmp1F2;_LL13F: if(_tmp1EA <= (void*)4)goto _LL141;if(*((int*)
_tmp1EA)!= 4)goto _LL141;_tmp1EB=((struct Cyc_Absyn_PointerType_struct*)_tmp1EA)->f1;
_tmp1EC=(void*)_tmp1EB.elt_typ;_tmp1ED=_tmp1EB.elt_tq;_tmp1EE=_tmp1EB.ptr_atts;
_tmp1EF=(void*)_tmp1EE.rgn;_tmp1F0=_tmp1EE.nullable;_tmp1F1=_tmp1EE.bounds;
_tmp1F2=_tmp1EE.zero_term;_LL140:{void*_tmp1F3=Cyc_Tcutil_compress(_tmp1EC);
struct Cyc_Absyn_FnInfo _tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_Core_Opt*
_tmp1F6;void*_tmp1F7;struct Cyc_List_List*_tmp1F8;int _tmp1F9;struct Cyc_Absyn_VarargInfo*
_tmp1FA;struct Cyc_List_List*_tmp1FB;struct Cyc_List_List*_tmp1FC;_LL144: if(
_tmp1F3 <= (void*)4)goto _LL146;if(*((int*)_tmp1F3)!= 8)goto _LL146;_tmp1F4=((
struct Cyc_Absyn_FnType_struct*)_tmp1F3)->f1;_tmp1F5=_tmp1F4.tvars;_tmp1F6=
_tmp1F4.effect;_tmp1F7=(void*)_tmp1F4.ret_typ;_tmp1F8=_tmp1F4.args;_tmp1F9=
_tmp1F4.c_varargs;_tmp1FA=_tmp1F4.cyc_varargs;_tmp1FB=_tmp1F4.rgn_po;_tmp1FC=
_tmp1F4.attributes;_LL145: {struct _RegionHandle _tmp1FD=_new_region("temp");
struct _RegionHandle*temp=& _tmp1FD;_push_region(temp);{struct Cyc_List_List*
instantiation=0;for(0;ts != 0  && _tmp1F5 != 0;(ts=ts->tl,_tmp1F5=_tmp1F5->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1F5->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp1FE=_region_malloc(temp,sizeof(*_tmp1FE));_tmp1FE->hd=({struct
_tuple5*_tmp1FF=_region_malloc(temp,sizeof(*_tmp1FF));_tmp1FF->f1=(struct Cyc_Absyn_Tvar*)
_tmp1F5->hd;_tmp1FF->f2=(void*)ts->hd;_tmp1FF;});_tmp1FE->tl=instantiation;
_tmp1FE;});}if(ts != 0){void*_tmp202=({void*_tmp200[0]={};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp201="too many type variables in instantiation";
_tag_dynforward(_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,41));}),
_tag_dynforward(_tmp200,sizeof(void*),0));});_npop_handler(0);return _tmp202;}{
void*new_fn_typ=Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Absyn_FnType_struct
_tmp209;_tmp209.tag=8;_tmp209.f1=({struct Cyc_Absyn_FnInfo _tmp20A;_tmp20A.tvars=
_tmp1F5;_tmp20A.effect=_tmp1F6;_tmp20A.ret_typ=(void*)_tmp1F7;_tmp20A.args=
_tmp1F8;_tmp20A.c_varargs=_tmp1F9;_tmp20A.cyc_varargs=_tmp1FA;_tmp20A.rgn_po=
_tmp1FB;_tmp20A.attributes=_tmp1FC;_tmp20A;});_tmp209;});_tmp208;}));void*
_tmp207=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp203=_cycalloc(sizeof(*
_tmp203));_tmp203[0]=({struct Cyc_Absyn_PointerType_struct _tmp204;_tmp204.tag=4;
_tmp204.f1=({struct Cyc_Absyn_PtrInfo _tmp205;_tmp205.elt_typ=(void*)new_fn_typ;
_tmp205.elt_tq=_tmp1ED;_tmp205.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp206;_tmp206.rgn=(
void*)_tmp1EF;_tmp206.nullable=_tmp1F0;_tmp206.bounds=_tmp1F1;_tmp206.zero_term=
_tmp1F2;_tmp206.ptrloc=0;_tmp206;});_tmp205;});_tmp204;});_tmp203;});
_npop_handler(0);return _tmp207;}};_pop_region(temp);}_LL146:;_LL147: goto _LL143;
_LL143:;}goto _LL13E;_LL141:;_LL142: goto _LL13E;_LL13E:;}return({struct Cyc_String_pa_struct
_tmp20D;_tmp20D.tag=0;_tmp20D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp20B[1]={& _tmp20D};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp20C="expecting polymorphic type but found %s";
_tag_dynforward(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,40));}),
_tag_dynforward(_tmp20B,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp20E=({
struct Cyc_String_pa_struct _tmp212;_tmp212.tag=0;_tmp212.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp211;_tmp211.tag=0;_tmp211.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp20F[2]={& _tmp211,& _tmp212};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp210="cannot cast %s to %s";_tag_dynforward(
_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,21));}),_tag_dynforward(_tmp20F,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp20E;}}}}{struct
_tuple0 _tmp214=({struct _tuple0 _tmp213;_tmp213.f1=(void*)e->r;_tmp213.f2=Cyc_Tcutil_compress(
t);_tmp213;});void*_tmp215;struct Cyc_Absyn_MallocInfo _tmp216;int _tmp217;void*
_tmp218;struct Cyc_Absyn_PtrInfo _tmp219;struct Cyc_Absyn_PtrAtts _tmp21A;struct Cyc_Absyn_Conref*
_tmp21B;struct Cyc_Absyn_Conref*_tmp21C;struct Cyc_Absyn_Conref*_tmp21D;_LL149:
_tmp215=_tmp214.f1;if(*((int*)_tmp215)!= 35)goto _LL14B;_tmp216=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp215)->f1;_tmp217=_tmp216.fat_result;_tmp218=_tmp214.f2;if(_tmp218 <= (void*)4)
goto _LL14B;if(*((int*)_tmp218)!= 4)goto _LL14B;_tmp219=((struct Cyc_Absyn_PointerType_struct*)
_tmp218)->f1;_tmp21A=_tmp219.ptr_atts;_tmp21B=_tmp21A.nullable;_tmp21C=_tmp21A.bounds;
_tmp21D=_tmp21A.zero_term;_LL14A: if((_tmp217  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp21D)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp21B)){void*_tmp21E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp21C);struct Cyc_Absyn_Exp*_tmp21F;_LL14E: if(_tmp21E <= (void*)2)goto _LL150;if(*((
int*)_tmp21E)!= 0)goto _LL150;_tmp21F=((struct Cyc_Absyn_Upper_b_struct*)_tmp21E)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp21F)).f1 == 1)({void*_tmp220[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp221="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,60));}),
_tag_dynforward(_tmp220,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp222=0;struct
Cyc_Absyn_Tqual _tmp223=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp224=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp225;void*_tmp226;struct Cyc_Absyn_Tqual _tmp227;
struct Cyc_Absyn_PtrAtts _tmp228;struct Cyc_Absyn_Conref*_tmp229;_LL153: if(_tmp224
<= (void*)4)goto _LL155;if(*((int*)_tmp224)!= 4)goto _LL155;_tmp225=((struct Cyc_Absyn_PointerType_struct*)
_tmp224)->f1;_tmp226=(void*)_tmp225.elt_typ;_tmp227=_tmp225.elt_tq;_tmp228=
_tmp225.ptr_atts;_tmp229=_tmp228.zero_term;_LL154: _tmp222=({void**_tmp22A=
_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=_tmp226;_tmp22A;});_tmp223=_tmp227;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle _tmp22B=
_new_region("r");struct _RegionHandle*r=& _tmp22B;_push_region(r);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(r,te),_tmp222,e);;_pop_region(r);}{void*_tmp22C=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22E;_LL158: if(*((
int*)_tmp22C)!= 25)goto _LL15A;_tmp22D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp22C)->f1;_tmp22E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp22C)->f2;_LL159:{
void*_tmp22F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp22D->topt))->v);_LL15D: if(_tmp22F <= (void*)4)goto _LL15F;if(*((int*)_tmp22F)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp22D,_tmp22E,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp22D,
_tmp22E);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp231;void*
_tmp232;struct _tuple6 _tmp230=Cyc_Tcutil_addressof_props(te,e);_tmp231=_tmp230.f1;
_tmp232=_tmp230.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp231){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp232,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp235;
_tmp235.tag=0;_tmp235.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp233[1]={& _tmp235};Cyc_Tcutil_terr(loc,({const char*_tmp234="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,55));}),
_tag_dynforward(_tmp233,sizeof(void*),1));}});if(topt != 0){void*_tmp236=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp236 <= (void*)4)goto _LL164;if(*((int*)_tmp236)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp237=_cycalloc(
sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Absyn_SizeofType_struct _tmp238;_tmp238.tag=
14;_tmp238.f1=(void*)t;_tmp238;});_tmp237;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp239=n;struct _dynforward_ptr*_tmp23A;unsigned int _tmp23B;_LL167: if(*((int*)
_tmp239)!= 0)goto _LL169;_tmp23A=((struct Cyc_Absyn_StructField_struct*)_tmp239)->f1;
_LL168: {int bad_type=1;{void*_tmp23C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23D;union Cyc_Absyn_AggrInfoU_union _tmp23E;struct Cyc_Absyn_Aggrdecl**_tmp23F;
struct Cyc_List_List*_tmp240;_LL16C: if(_tmp23C <= (void*)4)goto _LL170;if(*((int*)
_tmp23C)!= 10)goto _LL16E;_tmp23D=((struct Cyc_Absyn_AggrType_struct*)_tmp23C)->f1;
_tmp23E=_tmp23D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp23C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16E;_tmp23F=(_tmp23E.KnownAggr).f1;_LL16D: if((*_tmp23F)->impl == 0)
goto _LL16B;if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp23A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp23F)->impl))->fields))({
struct Cyc_String_pa_struct _tmp243;_tmp243.tag=0;_tmp243.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp23A);{void*_tmp241[1]={& _tmp243};
Cyc_Tcutil_terr(loc,({const char*_tmp242="no field of struct/union has name %s";
_tag_dynforward(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,37));}),
_tag_dynforward(_tmp241,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((
int*)_tmp23C)!= 11)goto _LL170;_tmp240=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23C)->f2;_LL16F: if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp23A,_tmp240))({struct Cyc_String_pa_struct _tmp246;_tmp246.tag=0;_tmp246.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp23A);{void*_tmp244[1]={&
_tmp246};Cyc_Tcutil_terr(loc,({const char*_tmp245="no field of struct/union has name %s";
_tag_dynforward(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,37));}),
_tag_dynforward(_tmp244,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp249;
_tmp249.tag=0;_tmp249.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp247[1]={& _tmp249};Cyc_Tcutil_terr(loc,({const char*_tmp248="%s is not a known struct/union type";
_tag_dynforward(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,36));}),
_tag_dynforward(_tmp247,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp239)!= 1)goto _LL166;_tmp23B=((struct Cyc_Absyn_TupleIndex_struct*)_tmp239)->f1;
_LL16A: {int bad_type=1;{void*_tmp24A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp24B;union Cyc_Absyn_AggrInfoU_union _tmp24C;struct Cyc_Absyn_Aggrdecl**_tmp24D;
struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_Absyn_TunionFieldInfo
_tmp250;union Cyc_Absyn_TunionFieldInfoU_union _tmp251;struct Cyc_Absyn_Tunionfield*
_tmp252;_LL173: if(_tmp24A <= (void*)4)goto _LL17B;if(*((int*)_tmp24A)!= 10)goto
_LL175;_tmp24B=((struct Cyc_Absyn_AggrType_struct*)_tmp24A)->f1;_tmp24C=_tmp24B.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp24A)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL175;_tmp24D=(_tmp24C.KnownAggr).f1;_LL174: if((*_tmp24D)->impl == 0)
goto _LL172;_tmp24E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp24D)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp24A)!= 11)goto _LL177;_tmp24E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp24A)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24E)
<= _tmp23B)({struct Cyc_Int_pa_struct _tmp256;_tmp256.tag=1;_tmp256.f1=(
unsigned long)((int)_tmp23B);{struct Cyc_Int_pa_struct _tmp255;_tmp255.tag=1;
_tmp255.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp24E);{void*_tmp253[2]={& _tmp255,& _tmp256};Cyc_Tcutil_terr(loc,({const char*
_tmp254="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp254,
sizeof(char),_get_zero_arr_size(_tmp254,46));}),_tag_dynforward(_tmp253,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp24A)!= 9)goto _LL179;
_tmp24F=((struct Cyc_Absyn_TupleType_struct*)_tmp24A)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp24F)<= _tmp23B)({struct Cyc_Int_pa_struct
_tmp25A;_tmp25A.tag=1;_tmp25A.f1=(unsigned long)((int)_tmp23B);{struct Cyc_Int_pa_struct
_tmp259;_tmp259.tag=1;_tmp259.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp24F);{void*_tmp257[2]={& _tmp259,& _tmp25A};Cyc_Tcutil_terr(
loc,({const char*_tmp258="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,39));}),_tag_dynforward(_tmp257,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp24A)!= 3)goto
_LL17B;_tmp250=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24A)->f1;_tmp251=
_tmp250.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24A)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL17B;_tmp252=(_tmp251.KnownTunionfield).f2;_LL17A: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp252->typs)< _tmp23B)({struct Cyc_Int_pa_struct
_tmp25E;_tmp25E.tag=1;_tmp25E.f1=(unsigned long)((int)_tmp23B);{struct Cyc_Int_pa_struct
_tmp25D;_tmp25D.tag=1;_tmp25D.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp252->typs);{void*_tmp25B[2]={& _tmp25D,& _tmp25E};Cyc_Tcutil_terr(
loc,({const char*_tmp25C="tunionfield has too few components: %d < %d";
_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,44));}),
_tag_dynforward(_tmp25B,sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;
_LL17C: goto _LL172;_LL172:;}if(bad_type)({struct Cyc_String_pa_struct _tmp261;
_tmp261.tag=0;_tmp261.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp25F[1]={& _tmp261};Cyc_Tcutil_terr(loc,({const char*_tmp260="%s is not a known type";
_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,23));}),
_tag_dynforward(_tmp25F,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp262=_new_region("r");
struct _RegionHandle*r=& _tmp262;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp263=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp263,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp264=t;struct Cyc_Absyn_PtrInfo
_tmp265;void*_tmp266;struct Cyc_Absyn_PtrAtts _tmp267;void*_tmp268;struct Cyc_Absyn_Conref*
_tmp269;struct Cyc_Absyn_Conref*_tmp26A;_LL17E: if(_tmp264 <= (void*)4)goto _LL180;
if(*((int*)_tmp264)!= 4)goto _LL180;_tmp265=((struct Cyc_Absyn_PointerType_struct*)
_tmp264)->f1;_tmp266=(void*)_tmp265.elt_typ;_tmp267=_tmp265.ptr_atts;_tmp268=(
void*)_tmp267.rgn;_tmp269=_tmp267.bounds;_tmp26A=_tmp267.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
_tmp263,loc,_tmp268);Cyc_Tcutil_check_nonzero_bound(loc,_tmp269);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp266),(void*)1))({void*_tmp26B[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp26C="can't dereference abstract pointer type";_tag_dynforward(
_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,40));}),_tag_dynforward(_tmp26B,
sizeof(void*),0));});{void*_tmp26D=_tmp266;_npop_handler(0);return _tmp26D;}
_LL180:;_LL181: {void*_tmp271=({struct Cyc_String_pa_struct _tmp270;_tmp270.tag=0;
_tmp270.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp26E[1]={& _tmp270};Cyc_Tcexp_expr_err(_tmp263,loc,topt,({const char*
_tmp26F="expecting * or @ type but found %s";_tag_dynforward(_tmp26F,sizeof(char),
_get_zero_arr_size(_tmp26F,35));}),_tag_dynforward(_tmp26E,sizeof(void*),1));}});
_npop_handler(0);return _tmp271;}_LL17D:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp272=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp273;union Cyc_Absyn_AggrInfoU_union
_tmp274;struct Cyc_Absyn_Aggrdecl**_tmp275;struct Cyc_Absyn_Aggrdecl*_tmp276;
struct Cyc_List_List*_tmp277;void*_tmp278;struct Cyc_List_List*_tmp279;_LL183: if(
_tmp272 <= (void*)4)goto _LL18B;if(*((int*)_tmp272)!= 10)goto _LL185;_tmp273=((
struct Cyc_Absyn_AggrType_struct*)_tmp272)->f1;_tmp274=_tmp273.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp272)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL185;_tmp275=(_tmp274.KnownAggr).f1;_tmp276=*_tmp275;_tmp277=_tmp273.targs;
_LL184: {struct Cyc_Absyn_Aggrfield*_tmp27C=Cyc_Absyn_lookup_decl_field(_tmp276,f);
if(_tmp27C == 0)return({struct Cyc_String_pa_struct _tmp280;_tmp280.tag=0;_tmp280.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp27F;_tmp27F.tag=0;_tmp27F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp276->name));{void*_tmp27D[2]={& _tmp27F,& _tmp280};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp27E="type %s has no %s field";_tag_dynforward(
_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,24));}),_tag_dynforward(_tmp27D,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp276->impl))->exist_vars
!= 0)return({void*_tmp281[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp282="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,64));}),
_tag_dynforward(_tmp281,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp283=_new_region("rgn");struct _RegionHandle*rgn=& _tmp283;_push_region(rgn);{
struct Cyc_List_List*_tmp284=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp276->tvs,_tmp277);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp284,(void*)
_tmp27C->type);if(((void*)_tmp276->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp288=({struct Cyc_String_pa_struct
_tmp287;_tmp287.tag=0;_tmp287.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp285[1]={& _tmp287};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp286="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,56));}),
_tag_dynforward(_tmp285,sizeof(void*),1));}});_npop_handler(0);return _tmp288;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp272)!= 11)goto _LL187;_tmp278=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp272)->f1;_tmp279=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp272)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp289=Cyc_Absyn_lookup_field(
_tmp279,f);if(_tmp289 == 0)return({struct Cyc_String_pa_struct _tmp28C;_tmp28C.tag=
0;_tmp28C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp28A[1]={&
_tmp28C};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp28B="type has no %s field";
_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,21));}),
_tag_dynforward(_tmp28A,sizeof(void*),1));}});if((_tmp278 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp289->type))return({struct Cyc_String_pa_struct
_tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp28D[1]={& _tmp28F};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp28E="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size(_tmp28E,56));}),
_tag_dynforward(_tmp28D,sizeof(void*),1));}});return(void*)_tmp289->type;}_LL187:
if(*((int*)_tmp272)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp27A="size";_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(
_tmp27A,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp272)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp27B="size";
_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp291;_tmp291.tag=3;_tmp291.f1=(void*)((void*)19);_tmp291.f2=({struct Cyc_List_List*
_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->hd=e;_tmp292->tl=0;_tmp292;});
_tmp291;});_tmp290;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp293="size";_tag_dynforward(_tmp293,
sizeof(char),_get_zero_arr_size(_tmp293,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp296;_tmp296.tag=0;_tmp296.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp294[1]={& _tmp296};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp295="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,44));}),
_tag_dynforward(_tmp294,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp299;_tmp299.tag=0;_tmp299.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp297[1]={& _tmp299};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp298="expecting struct or union, found %s";
_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,36));}),
_tag_dynforward(_tmp297,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){{struct _RegionHandle _tmp29A=_new_region("r");struct
_RegionHandle*r=& _tmp29A;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp29B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp29C;void*_tmp29D;struct Cyc_Absyn_PtrAtts
_tmp29E;void*_tmp29F;struct Cyc_Absyn_Conref*_tmp2A0;struct Cyc_Absyn_Conref*
_tmp2A1;_LL18E: if(_tmp29B <= (void*)4)goto _LL190;if(*((int*)_tmp29B)!= 4)goto
_LL190;_tmp29C=((struct Cyc_Absyn_PointerType_struct*)_tmp29B)->f1;_tmp29D=(void*)
_tmp29C.elt_typ;_tmp29E=_tmp29C.ptr_atts;_tmp29F=(void*)_tmp29E.rgn;_tmp2A0=
_tmp29E.bounds;_tmp2A1=_tmp29E.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2A0);{void*_tmp2A2=Cyc_Tcutil_compress(_tmp29D);struct Cyc_Absyn_AggrInfo
_tmp2A3;union Cyc_Absyn_AggrInfoU_union _tmp2A4;struct Cyc_Absyn_Aggrdecl**_tmp2A5;
struct Cyc_Absyn_Aggrdecl*_tmp2A6;struct Cyc_List_List*_tmp2A7;void*_tmp2A8;struct
Cyc_List_List*_tmp2A9;_LL193: if(_tmp2A2 <= (void*)4)goto _LL197;if(*((int*)_tmp2A2)
!= 10)goto _LL195;_tmp2A3=((struct Cyc_Absyn_AggrType_struct*)_tmp2A2)->f1;_tmp2A4=
_tmp2A3.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2A2)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL195;_tmp2A5=(_tmp2A4.KnownAggr).f1;_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A3.targs;
_LL194: {struct Cyc_Absyn_Aggrfield*_tmp2AA=Cyc_Absyn_lookup_decl_field(_tmp2A6,f);
if(_tmp2AA == 0)return({struct Cyc_String_pa_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp2AD;_tmp2AD.tag=0;_tmp2AD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp2A6->name));{void*_tmp2AB[2]={& _tmp2AD,& _tmp2AE};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2AC="type %s has no %s field";_tag_dynforward(
_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,24));}),_tag_dynforward(_tmp2AB,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A6->impl))->exist_vars
!= 0)return({void*_tmp2AF[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp2B0="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,64));}),
_tag_dynforward(_tmp2AF,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp2B1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2B1;_push_region(rgn);{
struct Cyc_List_List*_tmp2B2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp2A6->tvs,_tmp2A7);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2B2,(void*)
_tmp2AA->type);};_pop_region(rgn);}if(((void*)_tmp2A6->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp2B5;
_tmp2B5.tag=0;_tmp2B5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp2B3[1]={& _tmp2B5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2B4="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,56));}),
_tag_dynforward(_tmp2B3,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp2A2)!= 11)goto _LL197;_tmp2A8=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2A2)->f1;_tmp2A9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2A2)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp2B6=Cyc_Absyn_lookup_field(_tmp2A9,f);if(_tmp2B6 == 
0)return({struct Cyc_String_pa_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2B7[1]={& _tmp2B9};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B8="type has no %s field";_tag_dynforward(_tmp2B8,
sizeof(char),_get_zero_arr_size(_tmp2B8,21));}),_tag_dynforward(_tmp2B7,sizeof(
void*),1));}});if((_tmp2A8 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2B6->type))return({struct Cyc_String_pa_struct _tmp2BC;_tmp2BC.tag=0;
_tmp2BC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2BA[1]={&
_tmp2BC};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2BB="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,56));}),
_tag_dynforward(_tmp2BA,sizeof(void*),1));}});return(void*)_tmp2B6->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2BD[1]={& _tmp2BF};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2BE="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,44));}),
_tag_dynforward(_tmp2BD,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2C1;int _tmp2C2;struct _tuple7 _tmp2C0=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;if(!
_tmp2C2)return({void*_tmp2C3[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2C4="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2C4,
sizeof(char),_get_zero_arr_size(_tmp2C4,47));}),_tag_dynforward(_tmp2C3,sizeof(
void*),0));});{struct _handler_cons _tmp2C5;_push_handler(& _tmp2C5);{int _tmp2C7=0;
if(setjmp(_tmp2C5.handler))_tmp2C7=1;if(!_tmp2C7){{void*_tmp2C8=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2C1)).f2;
_npop_handler(0);return _tmp2C8;};_pop_handler();}else{void*_tmp2C6=(void*)
_exn_thrown;void*_tmp2CA=_tmp2C6;_LL19A: if(_tmp2CA != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2CE;_tmp2CE.tag=1;_tmp2CE.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2CD;_tmp2CD.tag=1;_tmp2CD.f1=(unsigned long)((int)_tmp2C1);{void*_tmp2CB[2]={&
_tmp2CD,& _tmp2CE};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2CC="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,41));}),
_tag_dynforward(_tmp2CB,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2CA);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2CF=_new_region("r");struct
_RegionHandle*r=& _tmp2CF;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2D0=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2D0,0,e1);Cyc_Tcexp_tcExp(_tmp2D0,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2D0,e2)){void*_tmp2D4=({struct Cyc_String_pa_struct
_tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2D1[1]={& _tmp2D3};Cyc_Tcexp_expr_err(
_tmp2D0,e2->loc,topt,({const char*_tmp2D2="expecting int subscript, found %s";
_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,34));}),
_tag_dynforward(_tmp2D1,sizeof(void*),1));}});_npop_handler(0);return _tmp2D4;}{
void*_tmp2D5=t1;struct Cyc_Absyn_PtrInfo _tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Tqual
_tmp2D8;struct Cyc_Absyn_PtrAtts _tmp2D9;void*_tmp2DA;struct Cyc_Absyn_Conref*
_tmp2DB;struct Cyc_Absyn_Conref*_tmp2DC;struct Cyc_List_List*_tmp2DD;_LL19F: if(
_tmp2D5 <= (void*)4)goto _LL1A3;if(*((int*)_tmp2D5)!= 4)goto _LL1A1;_tmp2D6=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D5)->f1;_tmp2D7=(void*)_tmp2D6.elt_typ;
_tmp2D8=_tmp2D6.elt_tq;_tmp2D9=_tmp2D6.ptr_atts;_tmp2DA=(void*)_tmp2D9.rgn;
_tmp2DB=_tmp2D9.bounds;_tmp2DC=_tmp2D9.zero_term;_LL1A0: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2DC)){int emit_warning=1;{union
Cyc_Absyn_Constraint_union _tmp2DE=(Cyc_Absyn_compress_conref(_tmp2DB))->v;void*
_tmp2DF;struct Cyc_Absyn_Exp*_tmp2E0;void*_tmp2E1;void*_tmp2E2;_LL1A6: if((_tmp2DE.Eq_constr).tag
!= 0)goto _LL1A8;_tmp2DF=(_tmp2DE.Eq_constr).f1;if(_tmp2DF <= (void*)2)goto _LL1A8;
if(*((int*)_tmp2DF)!= 0)goto _LL1A8;_tmp2E0=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2DF)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(_tmp2D0,e2)){unsigned int _tmp2E4;
int _tmp2E5;struct _tuple7 _tmp2E3=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2E4=
_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;if(_tmp2E5){unsigned int _tmp2E7;int _tmp2E8;struct
_tuple7 _tmp2E6=Cyc_Evexp_eval_const_uint_exp(_tmp2E0);_tmp2E7=_tmp2E6.f1;_tmp2E8=
_tmp2E6.f2;if(_tmp2E8  && _tmp2E7 > _tmp2E4)emit_warning=0;}}goto _LL1A5;_LL1A8: if((
_tmp2DE.Eq_constr).tag != 0)goto _LL1AA;_tmp2E1=(_tmp2DE.Eq_constr).f1;if((int)
_tmp2E1 != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;_LL1AA: if((_tmp2DE.Eq_constr).tag
!= 0)goto _LL1AC;_tmp2E2=(_tmp2DE.Eq_constr).f1;if((int)_tmp2E2 != 1)goto _LL1AC;
_LL1AB: emit_warning=0;goto _LL1A5;_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(
emit_warning)({void*_tmp2E9[0]={};Cyc_Tcutil_warn(e2->loc,({const char*_tmp2EA="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2EA,sizeof(char),_get_zero_arr_size(_tmp2EA,63));}),
_tag_dynforward(_tmp2E9,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(
_tmp2D0,e2)){unsigned int _tmp2EC;int _tmp2ED;struct _tuple7 _tmp2EB=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2EC=_tmp2EB.f1;_tmp2ED=_tmp2EB.f2;if(_tmp2ED)Cyc_Tcutil_check_bound(loc,
_tmp2EC,_tmp2DB);}else{if(Cyc_Tcutil_is_bound_one(_tmp2DB) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2DC))({void*_tmp2EE[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2EF="subscript applied to pointer to one object";
_tag_dynforward(_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,43));}),
_tag_dynforward(_tmp2EE,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2DB);}}Cyc_Tcenv_check_rgn_accessible(_tmp2D0,loc,_tmp2DA);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2D7),(void*)1))({void*_tmp2F0[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2F1="can't subscript an abstract pointer";_tag_dynforward(_tmp2F1,
sizeof(char),_get_zero_arr_size(_tmp2F1,36));}),_tag_dynforward(_tmp2F0,sizeof(
void*),0));});{void*_tmp2F2=_tmp2D7;_npop_handler(0);return _tmp2F2;}_LL1A1: if(*((
int*)_tmp2D5)!= 9)goto _LL1A3;_tmp2DD=((struct Cyc_Absyn_TupleType_struct*)_tmp2D5)->f1;
_LL1A2: {void*_tmp2F3=Cyc_Tcexp_ithTupleType(_tmp2D0,loc,_tmp2DD,e2);
_npop_handler(0);return _tmp2F3;}_LL1A3:;_LL1A4: {void*_tmp2F7=({struct Cyc_String_pa_struct
_tmp2F6;_tmp2F6.tag=0;_tmp2F6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp2F4[1]={& _tmp2F6};Cyc_Tcexp_expr_err(
_tmp2D0,loc,topt,({const char*_tmp2F5="subscript applied to %s";_tag_dynforward(
_tmp2F5,sizeof(char),_get_zero_arr_size(_tmp2F5,24));}),_tag_dynforward(_tmp2F4,
sizeof(void*),1));}});_npop_handler(0);return _tmp2F7;}_LL19E:;}}};_pop_region(r);}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2F8=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2F9;
_LL1AF: if(_tmp2F8 <= (void*)4)goto _LL1B1;if(*((int*)_tmp2F8)!= 9)goto _LL1B1;
_tmp2F9=((struct Cyc_Absyn_TupleType_struct*)_tmp2F8)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2F9)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2F9=_tmp2F9->tl)){
void*_tmp2FA=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2F9))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2FA,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2F9->hd)).f2);fields=({
struct Cyc_List_List*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->hd=({struct
_tuple9*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->f1=(*((struct _tuple9*)
_tmp2F9->hd)).f1;_tmp2FC->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2FC;});_tmp2FB->tl=fields;_tmp2FB;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->hd=({struct _tuple9*_tmp2FE=
_cycalloc(sizeof(*_tmp2FE));_tmp2FE->f1=Cyc_Absyn_empty_tqual(0);_tmp2FE->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2FE;});_tmp2FD->tl=fields;_tmp2FD;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF[0]=({struct Cyc_Absyn_TupleType_struct
_tmp300;_tmp300.tag=9;_tmp300.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp300;});_tmp2FF;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp301[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp302="tcCompoundLit";_tag_dynforward(_tmp302,sizeof(char),
_get_zero_arr_size(_tmp302,14));}),_tag_dynforward(_tmp301,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){void*res_t2;{struct
_RegionHandle _tmp303=_new_region("r");struct _RegionHandle*r=& _tmp303;
_push_region(r);{int _tmp304=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp305=({struct Cyc_Absyn_Const_e_struct*_tmp30D=_cycalloc(sizeof(*_tmp30D));
_tmp30D[0]=({struct Cyc_Absyn_Const_e_struct _tmp30E;_tmp30E.tag=0;_tmp30E.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp30F;(_tmp30F.Int_c).tag=
2;(_tmp30F.Int_c).f1=(void*)((void*)1);(_tmp30F.Int_c).f2=_tmp304;_tmp30F;});
_tmp30E;});_tmp30D;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp305,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp306=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp304 - 1);if(!Cyc_Tcutil_is_zero(
_tmp306))({void*_tmp307[0]={};Cyc_Tcutil_terr(_tmp306->loc,({const char*_tmp308="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp308,sizeof(char),_get_zero_arr_size(_tmp308,45));}),
_tag_dynforward(_tmp307,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->v=(void*)Cyc_Absyn_uint_typ;_tmp309;});
res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp30C;_tmp30C.tag=0;_tmp30C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp30A[1]={& _tmp30C};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp30B="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp30B,sizeof(char),_get_zero_arr_size(_tmp30B,53));}),
_tag_dynforward(_tmp30A,sizeof(void*),1));}});};_pop_region(r);}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)
des->hd)).f1;if(ds != 0){void*_tmp310=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp311;
_LL1B4: if(*((int*)_tmp310)!= 1)goto _LL1B6;_LL1B5:({void*_tmp312[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp313="only array index designators are supported";
_tag_dynforward(_tmp313,sizeof(char),_get_zero_arr_size(_tmp313,43));}),
_tag_dynforward(_tmp312,sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)
_tmp310)!= 0)goto _LL1B3;_tmp311=((struct Cyc_Absyn_ArrayElement_struct*)_tmp310)->f1;
_LL1B7: Cyc_Tcexp_tcExpInitializer(te,0,_tmp311);{unsigned int _tmp315;int _tmp316;
struct _tuple7 _tmp314=Cyc_Evexp_eval_const_uint_exp(_tmp311);_tmp315=_tmp314.f1;
_tmp316=_tmp314.f2;if(!_tmp316)({void*_tmp317[0]={};Cyc_Tcutil_terr(_tmp311->loc,({
const char*_tmp318="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp318,sizeof(char),_get_zero_arr_size(_tmp318,47));}),
_tag_dynforward(_tmp317,sizeof(void*),0));});else{if(_tmp315 != offset)({struct
Cyc_Int_pa_struct _tmp31C;_tmp31C.tag=1;_tmp31C.f1=(unsigned long)((int)_tmp315);{
struct Cyc_Int_pa_struct _tmp31B;_tmp31B.tag=1;_tmp31B.f1=(unsigned long)offset;{
void*_tmp319[2]={& _tmp31B,& _tmp31C};Cyc_Tcutil_terr(_tmp311->loc,({const char*
_tmp31A="expecting index designator %d but found %d";_tag_dynforward(_tmp31A,
sizeof(char),_get_zero_arr_size(_tmp31A,43));}),_tag_dynforward(_tmp319,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp31D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp31D <= (void*)4)goto _LL1BB;if(*((int*)
_tmp31D)!= 18)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp320;_tmp320.tag=0;_tmp320.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp31E[1]={& _tmp320};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp31F="expecting unsigned int, found %s";
_tag_dynforward(_tmp31F,sizeof(char),_get_zero_arr_size(_tmp31F,33));}),
_tag_dynforward(_tmp31E,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp321[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp322="comprehension index variable is not declared const!";
_tag_dynforward(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,52));}),
_tag_dynforward(_tmp321,sizeof(void*),0));});{struct _RegionHandle _tmp323=
_new_region("r");struct _RegionHandle*r=& _tmp323;_push_region(r);{struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp324=te2;
struct Cyc_Absyn_PtrInfo pinfo;void**_tmp325=0;struct Cyc_Absyn_Tqual*_tmp326=0;
struct Cyc_Absyn_Conref**_tmp327=0;if(topt != 0){void*_tmp328=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp329;struct Cyc_Absyn_ArrayInfo _tmp32A;void*
_tmp32B;void**_tmp32C;struct Cyc_Absyn_Tqual _tmp32D;struct Cyc_Absyn_Tqual*_tmp32E;
struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Conref*_tmp330;struct Cyc_Absyn_Conref**
_tmp331;_LL1BE: if(_tmp328 <= (void*)4)goto _LL1C2;if(*((int*)_tmp328)!= 4)goto
_LL1C0;_tmp329=((struct Cyc_Absyn_PointerType_struct*)_tmp328)->f1;_LL1BF: pinfo=
_tmp329;_tmp325=(void**)((void**)((void*)& pinfo.elt_typ));_tmp326=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp327=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1BD;_LL1C0: if(*((int*)_tmp328)!= 7)goto _LL1C2;_tmp32A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp328)->f1;_tmp32B=(void*)_tmp32A.elt_type;_tmp32C=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp328)->f1).elt_type;_tmp32D=_tmp32A.tq;_tmp32E=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp328)->f1).tq;_tmp32F=_tmp32A.num_elts;
_tmp330=_tmp32A.zero_term;_tmp331=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp328)->f1).zero_term;_LL1C1: _tmp325=(void**)_tmp32C;_tmp326=(struct Cyc_Absyn_Tqual*)
_tmp32E;_tmp327=(struct Cyc_Absyn_Conref**)_tmp331;goto _LL1BD;_LL1C2:;_LL1C3: goto
_LL1BD;_LL1BD:;}{void*t=Cyc_Tcexp_tcExp(_tmp324,_tmp325,body);if(_tmp324->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp324,bound))({void*_tmp332[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp333="bound is not constant";_tag_dynforward(_tmp333,
sizeof(char),_get_zero_arr_size(_tmp333,22));}),_tag_dynforward(_tmp332,sizeof(
void*),0));});if(!Cyc_Tcutil_is_const_exp(_tmp324,body))({void*_tmp334[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp335="body is not constant";_tag_dynforward(_tmp335,
sizeof(char),_get_zero_arr_size(_tmp335,21));}),_tag_dynforward(_tmp334,sizeof(
void*),0));});}if(_tmp327 != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp327)){struct Cyc_Absyn_Exp*_tmp336=Cyc_Absyn_uint_exp(1,0);_tmp336->topt=({
struct Cyc_Core_Opt*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->v=(void*)Cyc_Absyn_uint_typ;
_tmp337;});bound=Cyc_Absyn_add_exp(bound,_tmp336,0);bound->topt=({struct Cyc_Core_Opt*
_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->v=(void*)Cyc_Absyn_uint_typ;_tmp338;});*
is_zero_term=1;}{void*_tmp339=Cyc_Absyn_array_typ(t,_tmp326 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp326,(struct Cyc_Absyn_Exp*)bound,_tmp327 == 0?Cyc_Absyn_false_conref:*
_tmp327,0);_npop_handler(0);return _tmp339;}}}};_pop_region(r);}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp33A;_push_handler(& _tmp33A);{
int _tmp33C=0;if(setjmp(_tmp33A.handler))_tmp33C=1;if(!_tmp33C){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp33B=(void*)_exn_thrown;void*_tmp33E=
_tmp33B;_LL1C5: if(_tmp33E != Cyc_Dict_Absent)goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct
_tmp341;_tmp341.tag=0;_tmp341.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp33F[1]={& _tmp341};Cyc_Tcutil_terr(loc,({
const char*_tmp340="unbound struct name %s";_tag_dynforward(_tmp340,sizeof(char),
_get_zero_arr_size(_tmp340,23));}),_tag_dynforward(_tmp33F,sizeof(void*),1));}});
return topt != 0?*topt:(void*)0;_LL1C7:;_LL1C8:(void)_throw(_tmp33E);_LL1C4:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({
void*_tmp342[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp343="expecting struct but found union";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,33));}),
_tag_dynforward(_tmp342,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp344[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp345="can't build abstract struct";
_tag_dynforward(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,28));}),
_tag_dynforward(_tmp344,sizeof(void*),0));});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{struct _RegionHandle _tmp346=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp346;_push_region(rgn);{struct _tuple4 _tmp347=({struct _tuple4 _tmp367;_tmp367.f1=
Cyc_Tcenv_lookup_type_vars(te);_tmp367.f2=rgn;_tmp367;});struct Cyc_List_List*
_tmp348=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp347,ad->tvs);struct Cyc_List_List*_tmp349=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp347,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp34A=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp348);struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp349);struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp348,_tmp349);struct Cyc_Absyn_AggrType_struct*_tmp34D=({struct Cyc_Absyn_AggrType_struct*
_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362[0]=({struct Cyc_Absyn_AggrType_struct
_tmp363;_tmp363.tag=10;_tmp363.f1=({struct Cyc_Absyn_AggrInfo _tmp364;_tmp364.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp365;(_tmp365.KnownAggr).tag=
1;(_tmp365.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp366=_cycalloc(sizeof(*
_tmp366));_tmp366[0]=ad;_tmp366;});_tmp365;});_tmp364.targs=_tmp34A;_tmp364;});
_tmp363;});_tmp362;});struct Cyc_List_List*_tmp34E=*ts;struct Cyc_List_List*
_tmp34F=_tmp34B;while(_tmp34E != 0  && _tmp34F != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp34E->hd);Cyc_Tcutil_unify((
void*)_tmp34E->hd,(void*)_tmp34F->hd);_tmp34E=_tmp34E->tl;_tmp34F=_tmp34F->tl;}
if(_tmp34E != 0)({void*_tmp350[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp351="too many explicit witness types";
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,32));}),
_tag_dynforward(_tmp350,sizeof(void*),0));});*ts=_tmp34B;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp34D,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp353;struct Cyc_Absyn_Aggrfield*_tmp354;
struct Cyc_Absyn_Exp*_tmp355;struct _tuple10*_tmp352=(struct _tuple10*)fields->hd;
_tmp353=*_tmp352;_tmp354=_tmp353.f1;_tmp355=_tmp353.f2;{void*_tmp356=Cyc_Tcutil_rsubstitute(
rgn,_tmp34C,(void*)_tmp354->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp356,
_tmp355);if(!Cyc_Tcutil_coerce_arg(te,_tmp355,_tmp356)){({struct Cyc_String_pa_struct
_tmp35C;_tmp35C.tag=0;_tmp35C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp355->topt))->v));{
struct Cyc_String_pa_struct _tmp35B;_tmp35B.tag=0;_tmp35B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp356));{struct
Cyc_String_pa_struct _tmp35A;_tmp35A.tag=0;_tmp35A.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp359;_tmp359.tag=0;_tmp359.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp354->name);{void*_tmp357[4]={& _tmp359,& _tmp35A,& _tmp35B,& _tmp35C};Cyc_Tcutil_terr(
_tmp355->loc,({const char*_tmp358="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp358,sizeof(char),_get_zero_arr_size(_tmp358,44));}),
_tag_dynforward(_tmp357,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp35D=0;{struct Cyc_List_List*_tmp35E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp35E != 0;_tmp35E=_tmp35E->tl){_tmp35D=({
struct Cyc_List_List*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->hd=({struct
_tuple0*_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp34C,(*((struct _tuple0*)_tmp35E->hd)).f1);_tmp360->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp34C,(*((struct _tuple0*)_tmp35E->hd)).f2);_tmp360;});_tmp35F->tl=_tmp35D;
_tmp35F;});}}_tmp35D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp35D);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp35D);{void*_tmp361=(void*)
_tmp34D;_npop_handler(0);return _tmp361;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp368=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp368;_push_region(rgn);{void*_tmp369=Cyc_Tcutil_compress(ts);void*_tmp36A;
struct Cyc_List_List*_tmp36B;_LL1CA: if(_tmp369 <= (void*)4)goto _LL1CC;if(*((int*)
_tmp369)!= 11)goto _LL1CC;_tmp36A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp369)->f1;_tmp36B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp369)->f2;_LL1CB:
if(_tmp36A == (void*)1)({void*_tmp36C[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp36D="expecting struct but found union";_tag_dynforward(_tmp36D,sizeof(char),
_get_zero_arr_size(_tmp36D,33));}),_tag_dynforward(_tmp36C,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp36B);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp36F;struct Cyc_Absyn_Aggrfield*
_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct _tuple10*_tmp36E=(struct _tuple10*)
fields->hd;_tmp36F=*_tmp36E;_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp370->type)),_tmp371);if(!Cyc_Tcutil_coerce_arg(
te,_tmp371,(void*)_tmp370->type)){({struct Cyc_String_pa_struct _tmp376;_tmp376.tag=
0;_tmp376.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp371->topt))->v));{struct Cyc_String_pa_struct
_tmp375;_tmp375.tag=0;_tmp375.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp370->type));{struct Cyc_String_pa_struct _tmp374;
_tmp374.tag=0;_tmp374.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp370->name);{
void*_tmp372[3]={& _tmp374,& _tmp375,& _tmp376};Cyc_Tcutil_terr(_tmp371->loc,({
const char*_tmp373="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp373,sizeof(char),_get_zero_arr_size(_tmp373,41));}),_tag_dynforward(_tmp372,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp377[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp378="tcAnonStruct: wrong type";
_tag_dynforward(_tmp378,sizeof(char),_get_zero_arr_size(_tmp378,25));}),
_tag_dynforward(_tmp377,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp379=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp379;_push_region(rgn);{struct _tuple4 _tmp37A=({
struct _tuple4 _tmp39A;_tmp39A.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp39A.f2=rgn;
_tmp39A;});struct Cyc_List_List*_tmp37B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp37A,tud->tvs);struct Cyc_List_List*_tmp37C=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp37B);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp397;_tmp397.tag=3;_tmp397.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp398;
_tmp398.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp399;(_tmp399.KnownTunionfield).tag=1;(_tmp399.KnownTunionfield).f1=tud;(
_tmp399.KnownTunionfield).f2=tuf;_tmp399;});_tmp398.targs=_tmp37C;_tmp398;});
_tmp397;});_tmp396;});if(topt != 0){void*_tmp37D=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_TunionInfo _tmp37E;struct Cyc_List_List*_tmp37F;struct Cyc_Core_Opt*
_tmp380;_LL1CF: if(_tmp37D <= (void*)4)goto _LL1D3;if(*((int*)_tmp37D)!= 3)goto
_LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;_LL1D1: if(*((int*)_tmp37D)
!= 2)goto _LL1D3;_tmp37E=((struct Cyc_Absyn_TunionType_struct*)_tmp37D)->f1;
_tmp37F=_tmp37E.targs;_tmp380=_tmp37E.rgn;_LL1D2:{struct Cyc_List_List*a=_tmp37C;
for(0;a != 0  && _tmp37F != 0;(a=a->tl,_tmp37F=_tmp37F->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp37F->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381->v=(void*)res;
_tmp381;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp382=_cycalloc(
sizeof(*_tmp382));_tmp382[0]=({struct Cyc_Absyn_TunionType_struct _tmp383;_tmp383.tag=
2;_tmp383.f1=({struct Cyc_Absyn_TunionInfo _tmp384;_tmp384.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp385;(_tmp385.KnownTunion).tag=1;(_tmp385.KnownTunion).f1=({
struct Cyc_Absyn_Tuniondecl**_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=tud;
_tmp386;});_tmp385;});_tmp384.targs=_tmp37C;_tmp384.rgn=_tmp380;_tmp384;});
_tmp383;});_tmp382;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1CE;
_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp37B,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp38B;_tmp38B.tag=0;_tmp38B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp38C="?";_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size(
_tmp38C,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp38A;_tmp38A.tag=0;_tmp38A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp389;_tmp389.tag=0;_tmp389.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp387[3]={& _tmp389,& _tmp38A,& _tmp38B};
Cyc_Tcutil_terr(e->loc,({const char*_tmp388="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,80));}),
_tag_dynforward(_tmp387,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp390=({struct Cyc_String_pa_struct _tmp38F;_tmp38F.tag=0;
_tmp38F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp38D[1]={& _tmp38F};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp38E="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,45));}),
_tag_dynforward(_tmp38D,sizeof(void*),1));}});_npop_handler(0);return _tmp390;}
if(ts != 0){void*_tmp394=({struct Cyc_String_pa_struct _tmp393;_tmp393.tag=0;
_tmp393.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp391[1]={& _tmp393};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp392="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,44));}),_tag_dynforward(_tmp391,
sizeof(void*),1));}});_npop_handler(0);return _tmp394;}{void*_tmp395=res;
_npop_handler(0);return _tmp395;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp39B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp39C;
struct Cyc_Absyn_PtrAtts _tmp39D;struct Cyc_Absyn_Conref*_tmp39E;struct Cyc_Absyn_ArrayInfo
_tmp39F;void*_tmp3A0;struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_AggrInfo _tmp3A2;
union Cyc_Absyn_AggrInfoU_union _tmp3A3;struct Cyc_List_List*_tmp3A4;struct Cyc_List_List*
_tmp3A5;_LL1D6: if((int)_tmp39B != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp39B <= (
void*)4)goto _LL1E4;if(*((int*)_tmp39B)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA:
if(*((int*)_tmp39B)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp39B)
!= 2)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp39B)!= 3)goto _LL1E0;
_LL1DF: return 0;_LL1E0: if(*((int*)_tmp39B)!= 4)goto _LL1E2;_tmp39C=((struct Cyc_Absyn_PointerType_struct*)
_tmp39B)->f1;_tmp39D=_tmp39C.ptr_atts;_tmp39E=_tmp39D.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp39E);_LL1E2: if(*((
int*)_tmp39B)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp39B != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp39B <= (void*)4)goto _LL202;if(*((int*)
_tmp39B)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp39B)!= 7)goto _LL1EA;
_tmp39F=((struct Cyc_Absyn_ArrayType_struct*)_tmp39B)->f1;_tmp3A0=(void*)_tmp39F.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp3A0);_LL1EA: if(*((int*)_tmp39B)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp39B)!= 9)goto _LL1EE;_tmp3A1=((
struct Cyc_Absyn_TupleType_struct*)_tmp39B)->f1;_LL1ED: for(0;(unsigned int)
_tmp3A1;_tmp3A1=_tmp3A1->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp3A1->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp39B)!= 10)goto _LL1F0;
_tmp3A2=((struct Cyc_Absyn_AggrType_struct*)_tmp39B)->f1;_tmp3A3=_tmp3A2.aggr_info;
_tmp3A4=_tmp3A2.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp3A6=Cyc_Absyn_get_known_aggrdecl(
_tmp3A3);if(_tmp3A6->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3A6->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3A7=_new_region("r");
struct _RegionHandle*r=& _tmp3A7;_push_region(r);{struct Cyc_List_List*_tmp3A8=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3A6->tvs,_tmp3A4);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A6->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3A8,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3A9=0;_npop_handler(0);
return _tmp3A9;}}}{int _tmp3AA=1;_npop_handler(0);return _tmp3AA;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp39B)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp39B)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp39B)!= 18)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp39B)!= 11)goto _LL1F8;_tmp3A5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp39B)->f2;_LL1F7: for(0;_tmp3A5 != 0;
_tmp3A5=_tmp3A5->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp3A5->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp39B)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp39B)!= 17)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp39B)!= 16)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp39B)!= 
15)goto _LL200;_LL1FF: return 0;_LL200: if(*((int*)_tmp39B)!= 19)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp39B != 2)goto _LL204;_LL203: goto _LL205;_LL204: if((
int)_tmp39B != 3)goto _LL206;_LL205: goto _LL207;_LL206: if(_tmp39B <= (void*)4)goto
_LL208;if(*((int*)_tmp39B)!= 20)goto _LL208;_LL207: goto _LL209;_LL208: if(_tmp39B <= (
void*)4)goto _LL20A;if(*((int*)_tmp39B)!= 21)goto _LL20A;_LL209: goto _LL20B;_LL20A:
if(_tmp39B <= (void*)4)goto _LL1D5;if(*((int*)_tmp39B)!= 22)goto _LL1D5;_LL20B:({
struct Cyc_String_pa_struct _tmp3AD;_tmp3AD.tag=0;_tmp3AD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp3AB[
1]={& _tmp3AD};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3AC="bad type `%s' in zeroable type";_tag_dynforward(_tmp3AC,
sizeof(char),_get_zero_arr_size(_tmp3AC,31));}),_tag_dynforward(_tmp3AB,sizeof(
void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3AE=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3AF;void*_tmp3B0;_LL20D: if(_tmp3AE <= (void*)4)
goto _LL20F;if(*((int*)_tmp3AE)!= 4)goto _LL20F;_tmp3AF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3AE)->f1;_tmp3B0=(void*)_tmp3AF.elt_typ;_LL20E: Cyc_Tcutil_unify(_tmp3B0,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20C;_LL20F:;_LL210: goto _LL20C;_LL20C:;}({struct Cyc_String_pa_struct
_tmp3B4;_tmp3B4.tag=0;_tmp3B4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp3B3;_tmp3B3.tag=0;
_tmp3B3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(allow_zero?(struct
_dynforward_ptr)({const char*_tmp3B5="calloc";_tag_dynforward(_tmp3B5,sizeof(char),
_get_zero_arr_size(_tmp3B5,7));}):(struct _dynforward_ptr)({const char*_tmp3B6="malloc";
_tag_dynforward(_tmp3B6,sizeof(char),_get_zero_arr_size(_tmp3B6,7));})));{void*
_tmp3B1[2]={& _tmp3B3,& _tmp3B4};Cyc_Tcutil_terr(loc,({const char*_tmp3B2="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp3B2,sizeof(char),_get_zero_arr_size(_tmp3B2,60));}),
_tag_dynforward(_tmp3B1,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3BD;_tmp3BD.tag=15;_tmp3BD.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3BD;});_tmp3BC;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3B7=Cyc_Tcutil_compress(handle_type);void*_tmp3B8;_LL212: if(_tmp3B7 <= (
void*)4)goto _LL214;if(*((int*)_tmp3B7)!= 15)goto _LL214;_tmp3B8=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3B7)->f1;_LL213: rgn=_tmp3B8;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL211;_LL214:;_LL215:({struct Cyc_String_pa_struct _tmp3BB;
_tmp3BB.tag=0;_tmp3BB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3B9[1]={& _tmp3BB};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3BA="expecting region_t type but found %s";_tag_dynforward(_tmp3BA,sizeof(
char),_get_zero_arr_size(_tmp3BA,37));}),_tag_dynforward(_tmp3B9,sizeof(void*),1));}});
goto _LL211;_LL211:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3BE[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3BF="calloc with empty type";_tag_dynforward(_tmp3BF,sizeof(char),
_get_zero_arr_size(_tmp3BF,23));}),_tag_dynforward(_tmp3BE,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3C0=(void*)(*e)->r;void*_tmp3C1;void*
_tmp3C2;struct Cyc_List_List*_tmp3C3;struct Cyc_List_List _tmp3C4;struct Cyc_Absyn_Exp*
_tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_List_List _tmp3C7;struct Cyc_Absyn_Exp*
_tmp3C8;struct Cyc_List_List*_tmp3C9;_LL217: if(*((int*)_tmp3C0)!= 18)goto _LL219;
_tmp3C1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C0)->f1;_LL218:
elt_type=_tmp3C1;*t=(void**)({void**_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA[
0]=elt_type;_tmp3CA;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL216;_LL219: if(*((int*)_tmp3C0)!= 3)
goto _LL21B;_tmp3C2=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3C0)->f1;if((
int)_tmp3C2 != 1)goto _LL21B;_tmp3C3=((struct Cyc_Absyn_Primop_e_struct*)_tmp3C0)->f2;
if(_tmp3C3 == 0)goto _LL21B;_tmp3C4=*_tmp3C3;_tmp3C5=(struct Cyc_Absyn_Exp*)_tmp3C4.hd;
_tmp3C6=_tmp3C4.tl;if(_tmp3C6 == 0)goto _LL21B;_tmp3C7=*_tmp3C6;_tmp3C8=(struct Cyc_Absyn_Exp*)
_tmp3C7.hd;_tmp3C9=_tmp3C7.tl;if(_tmp3C9 != 0)goto _LL21B;_LL21A:{struct _tuple0
_tmp3CC=({struct _tuple0 _tmp3CB;_tmp3CB.f1=(void*)_tmp3C5->r;_tmp3CB.f2=(void*)
_tmp3C8->r;_tmp3CB;});void*_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*_tmp3D0;_LL21E:
_tmp3CD=_tmp3CC.f1;if(*((int*)_tmp3CD)!= 18)goto _LL220;_tmp3CE=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3CD)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3CE);elt_type=_tmp3CE;*t=(void**)({void**_tmp3D1=_cycalloc(sizeof(*
_tmp3D1));_tmp3D1[0]=elt_type;_tmp3D1;});num_elts=_tmp3C8;one_elt=0;goto _LL21D;
_LL220: _tmp3CF=_tmp3CC.f2;if(*((int*)_tmp3CF)!= 18)goto _LL222;_tmp3D0=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3CF)->f1;_LL221: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3D0);elt_type=_tmp3D0;*t=(void**)({void**_tmp3D2=_cycalloc(
sizeof(*_tmp3D2));_tmp3D2[0]=elt_type;_tmp3D2;});num_elts=_tmp3C5;one_elt=0;goto
_LL21D;_LL222:;_LL223: goto No_sizeof;_LL21D:;}goto _LL216;_LL21B:;_LL21C: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3D3=_cycalloc(sizeof(*_tmp3D3));
_tmp3D3[0]=elt_type;_tmp3D3;});num_elts=*e;one_elt=0;goto _LL216;_LL216:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3D4=elt_type;struct Cyc_Absyn_AggrInfo _tmp3D5;
union Cyc_Absyn_AggrInfoU_union _tmp3D6;struct Cyc_Absyn_Aggrdecl**_tmp3D7;struct
Cyc_Absyn_Aggrdecl*_tmp3D8;_LL225: if(_tmp3D4 <= (void*)4)goto _LL227;if(*((int*)
_tmp3D4)!= 10)goto _LL227;_tmp3D5=((struct Cyc_Absyn_AggrType_struct*)_tmp3D4)->f1;
_tmp3D6=_tmp3D5.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3D4)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL227;_tmp3D7=(_tmp3D6.KnownAggr).f1;_tmp3D8=*_tmp3D7;_LL226: if(
_tmp3D8->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D8->impl))->exist_vars
!= 0)({void*_tmp3D9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3DA="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3DA,sizeof(char),_get_zero_arr_size(_tmp3DA,50));}),
_tag_dynforward(_tmp3D9,sizeof(void*),0));});goto _LL224;_LL227:;_LL228: goto
_LL224;_LL224:;}{void*(*_tmp3DB)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3DC=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3DD=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3DE;struct Cyc_Absyn_PtrAtts _tmp3DF;struct Cyc_Absyn_Conref*_tmp3E0;struct Cyc_Absyn_Conref*
_tmp3E1;_LL22A: if(_tmp3DD <= (void*)4)goto _LL22C;if(*((int*)_tmp3DD)!= 4)goto
_LL22C;_tmp3DE=((struct Cyc_Absyn_PointerType_struct*)_tmp3DD)->f1;_tmp3DF=
_tmp3DE.ptr_atts;_tmp3E0=_tmp3DF.nullable;_tmp3E1=_tmp3DF.zero_term;_LL22B:
_tmp3DC=_tmp3E1;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3E0))_tmp3DB=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3E1) && !(*is_calloc)){({void*_tmp3E2[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3E3="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3E3,sizeof(char),_get_zero_arr_size(_tmp3E3,55));}),
_tag_dynforward(_tmp3E2,sizeof(void*),0));});*is_calloc=1;}goto _LL229;_LL22C:;
_LL22D: goto _LL229;_LL229:;}if(!one_elt)_tmp3DB=Cyc_Absyn_dynforward_typ;return
_tmp3DB(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3DC);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3E4=Cyc_Tcutil_compress(t1);_LL22F: if(
_tmp3E4 <= (void*)4)goto _LL231;if(*((int*)_tmp3E4)!= 7)goto _LL231;_LL230:({void*
_tmp3E5[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3E6="cannot assign to an array";
_tag_dynforward(_tmp3E6,sizeof(char),_get_zero_arr_size(_tmp3E6,26));}),
_tag_dynforward(_tmp3E5,sizeof(void*),0));});goto _LL22E;_LL231:;_LL232: goto
_LL22E;_LL22E:;}{int ign_1=0;int ign_2=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,&
ign_1,& ign_2))({void*_tmp3E7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3E8="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dynforward(_tmp3E8,sizeof(char),_get_zero_arr_size(_tmp3E8,58));}),
_tag_dynforward(_tmp3E7,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp3E9[0]={};Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp3EA="swap non-lvalue";
_tag_dynforward(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,16));}),
_tag_dynforward(_tmp3E9,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e2))return({
void*_tmp3EB[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp3EC="swap non-lvalue";
_tag_dynforward(_tmp3EC,sizeof(char),_get_zero_arr_size(_tmp3EC,16));}),
_tag_dynforward(_tmp3EB,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;int b_ign2=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,&
b_ign1,& b_ign2,& t_ign2))return({void*_tmp3ED[0]={};Cyc_Tcexp_expr_err(te,e1->loc,
topt,({const char*_tmp3EE="swap value in zeroterm array";_tag_dynforward(_tmp3EE,
sizeof(char),_get_zero_arr_size(_tmp3EE,29));}),_tag_dynforward(_tmp3ED,sizeof(
void*),0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& b_ign2,& t_ign2))
return({void*_tmp3EF[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp3F0="swap value in zeroterm array";_tag_dynforward(_tmp3F0,sizeof(char),
_get_zero_arr_size(_tmp3F0,29));}),_tag_dynforward(_tmp3EF,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp3F1=({struct Cyc_String_pa_struct _tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp3F4;_tmp3F4.tag=0;_tmp3F4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp3F2[2]={& _tmp3F4,& _tmp3F5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp3F3="type mismatch: %s != %s";_tag_dynforward(_tmp3F3,sizeof(char),
_get_zero_arr_size(_tmp3F3,24));}),_tag_dynforward(_tmp3F2,sizeof(void*),2));}}});
return _tmp3F1;}return(void*)0;}}}}static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*s){{struct
_RegionHandle _tmp3F6=_new_region("r");struct _RegionHandle*r=& _tmp3F6;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);;
_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp3F7=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Stmt*
_tmp3F9;struct Cyc_Absyn_Stmt*_tmp3FA;struct Cyc_Absyn_Decl*_tmp3FB;struct Cyc_Absyn_Stmt*
_tmp3FC;_LL234: if(_tmp3F7 <= (void*)1)goto _LL23A;if(*((int*)_tmp3F7)!= 0)goto
_LL236;_tmp3F8=((struct Cyc_Absyn_Exp_s_struct*)_tmp3F7)->f1;_LL235: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F8->topt))->v;_LL236: if(*((int*)_tmp3F7)!= 
1)goto _LL238;_tmp3F9=((struct Cyc_Absyn_Seq_s_struct*)_tmp3F7)->f1;_tmp3FA=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3F7)->f2;_LL237: s=_tmp3FA;continue;_LL238: if(*((
int*)_tmp3F7)!= 11)goto _LL23A;_tmp3FB=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F7)->f1;
_tmp3FC=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F7)->f2;_LL239: s=_tmp3FC;continue;
_LL23A:;_LL23B: return({void*_tmp3FD[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp3FE="statement expression must end with expression";_tag_dynforward(
_tmp3FE,sizeof(char),_get_zero_arr_size(_tmp3FE,46));}),_tag_dynforward(_tmp3FD,
sizeof(void*),0));});_LL233:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp404=
_cycalloc(sizeof(*_tmp404));_tmp404[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp405;_tmp405.tag=15;_tmp405.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp405;});_tmp404;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp3FF=Cyc_Tcutil_compress(handle_type);void*_tmp400;_LL23D:
if(_tmp3FF <= (void*)4)goto _LL23F;if(*((int*)_tmp3FF)!= 15)goto _LL23F;_tmp400=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3FF)->f1;_LL23E: rgn=_tmp400;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL23C;_LL23F:;_LL240:({struct Cyc_String_pa_struct
_tmp403;_tmp403.tag=0;_tmp403.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp401[1]={& _tmp403};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp402="expecting region_t type but found %s";
_tag_dynforward(_tmp402,sizeof(char),_get_zero_arr_size(_tmp402,37));}),
_tag_dynforward(_tmp401,sizeof(void*),1));}});goto _LL23C;_LL23C:;}{void*_tmp406=(
void*)e1->r;struct Cyc_Core_Opt*_tmp407;struct Cyc_List_List*_tmp408;struct Cyc_List_List*
_tmp409;union Cyc_Absyn_Cnst_union _tmp40A;struct _dynforward_ptr _tmp40B;_LL242: if(*((
int*)_tmp406)!= 29)goto _LL244;_LL243: {void*_tmp40C=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp40D=Cyc_Tcutil_compress(_tmp40C);struct Cyc_Absyn_ArrayInfo
_tmp40E;void*_tmp40F;struct Cyc_Absyn_Tqual _tmp410;struct Cyc_Absyn_Exp*_tmp411;
struct Cyc_Absyn_Conref*_tmp412;_LL24D: if(_tmp40D <= (void*)4)goto _LL24F;if(*((int*)
_tmp40D)!= 7)goto _LL24F;_tmp40E=((struct Cyc_Absyn_ArrayType_struct*)_tmp40D)->f1;
_tmp40F=(void*)_tmp40E.elt_type;_tmp410=_tmp40E.tq;_tmp411=_tmp40E.num_elts;
_tmp412=_tmp40E.zero_term;_LL24E: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp411);void*b;{void*_tmp413=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp414;_LL252: if(_tmp413 <= (void*)4)goto _LL254;
if(*((int*)_tmp413)!= 18)goto _LL254;_tmp414=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp413)->f1;_LL253: b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp415=
_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp416;_tmp416.tag=1;_tmp416.f1=(void*)_tmp414;_tmp416;});_tmp415;});goto _LL251;
_LL254:;_LL255: if(Cyc_Tcutil_is_const_exp(te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp418;_tmp418.tag=0;_tmp418.f1=bnd;_tmp418;});_tmp417;});else{b=(void*)0;}
_LL251:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp41A=
_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_Absyn_PointerType_struct
_tmp41B;_tmp41B.tag=4;_tmp41B.f1=({struct Cyc_Absyn_PtrInfo _tmp41C;_tmp41C.elt_typ=(
void*)_tmp40F;_tmp41C.elt_tq=_tmp410;_tmp41C.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp41D;_tmp41D.rgn=(void*)rgn;_tmp41D.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp41D.bounds=Cyc_Absyn_new_conref(b);_tmp41D.zero_term=
_tmp412;_tmp41D.ptrloc=0;_tmp41D;});_tmp41C;});_tmp41B;});_tmp41A;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->v=(
void*)res_typ;_tmp419;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL24F:;_LL250:({void*_tmp41E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp41F="tcNew: comprehension returned non-array type";_tag_dynforward(_tmp41F,
sizeof(char),_get_zero_arr_size(_tmp41F,45));}),_tag_dynforward(_tmp41E,sizeof(
void*),0));});_LL24C:;}_LL244: if(*((int*)_tmp406)!= 37)goto _LL246;_tmp407=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp406)->f1;_tmp408=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp406)->f2;_LL245:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420[0]=({struct Cyc_Absyn_Array_e_struct
_tmp421;_tmp421.tag=28;_tmp421.f1=_tmp408;_tmp421;});_tmp420;})));_tmp409=
_tmp408;goto _LL247;_LL246: if(*((int*)_tmp406)!= 28)goto _LL248;_tmp409=((struct
Cyc_Absyn_Array_e_struct*)_tmp406)->f1;_LL247: {void**elt_typ_opt=0;int zero_term=
0;if(topt != 0){void*_tmp422=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp423;void*_tmp424;void**_tmp425;struct Cyc_Absyn_Tqual _tmp426;struct Cyc_Absyn_PtrAtts
_tmp427;struct Cyc_Absyn_Conref*_tmp428;_LL257: if(_tmp422 <= (void*)4)goto _LL259;
if(*((int*)_tmp422)!= 4)goto _LL259;_tmp423=((struct Cyc_Absyn_PointerType_struct*)
_tmp422)->f1;_tmp424=(void*)_tmp423.elt_typ;_tmp425=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp422)->f1).elt_typ;_tmp426=_tmp423.elt_tq;_tmp427=_tmp423.ptr_atts;_tmp428=
_tmp427.zero_term;_LL258: elt_typ_opt=(void**)_tmp425;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp428);goto _LL256;_LL259:;
_LL25A: goto _LL256;_LL256:;}{void*_tmp429=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp409);e1->topt=({struct Cyc_Core_Opt*_tmp42A=_cycalloc(
sizeof(*_tmp42A));_tmp42A->v=(void*)_tmp429;_tmp42A;});{void*res_typ;{void*
_tmp42B=Cyc_Tcutil_compress(_tmp429);struct Cyc_Absyn_ArrayInfo _tmp42C;void*
_tmp42D;struct Cyc_Absyn_Tqual _tmp42E;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_Absyn_Conref*
_tmp430;_LL25C: if(_tmp42B <= (void*)4)goto _LL25E;if(*((int*)_tmp42B)!= 7)goto
_LL25E;_tmp42C=((struct Cyc_Absyn_ArrayType_struct*)_tmp42B)->f1;_tmp42D=(void*)
_tmp42C.elt_type;_tmp42E=_tmp42C.tq;_tmp42F=_tmp42C.num_elts;_tmp430=_tmp42C.zero_term;
_LL25D: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp431=_cycalloc(
sizeof(*_tmp431));_tmp431[0]=({struct Cyc_Absyn_PointerType_struct _tmp432;_tmp432.tag=
4;_tmp432.f1=({struct Cyc_Absyn_PtrInfo _tmp433;_tmp433.elt_typ=(void*)_tmp42D;
_tmp433.elt_tq=_tmp42E;_tmp433.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp434;_tmp434.rgn=(
void*)rgn;_tmp434.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp434.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp436;_tmp436.tag=0;_tmp436.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp42F);
_tmp436;});_tmp435;}));_tmp434.zero_term=_tmp430;_tmp434.ptrloc=0;_tmp434;});
_tmp433;});_tmp432;});_tmp431;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->v=(void*)res_typ;_tmp437;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25B;_LL25E:;_LL25F:({void*_tmp438[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp439="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp439,sizeof(char),_get_zero_arr_size(_tmp439,50));}),
_tag_dynforward(_tmp438,sizeof(void*),0));});_LL25B:;}return res_typ;}}}_LL248:
if(*((int*)_tmp406)!= 0)goto _LL24A;_tmp40A=((struct Cyc_Absyn_Const_e_struct*)
_tmp406)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp406)->f1).String_c).tag
!= 5)goto _LL24A;_tmp40B=(_tmp40A.String_c).f1;_LL249: {void*_tmp43A=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp43B=Cyc_Tcexp_tcExp(te,(void**)& _tmp43A,e1);return Cyc_Absyn_atb_typ(
_tmp43B,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp43D;_tmp43D.tag=0;_tmp43D.f1=Cyc_Absyn_uint_exp(1,0);_tmp43D;});_tmp43C;}),
Cyc_Absyn_false_conref);}_LL24A:;_LL24B: {void**topt2=0;if(topt != 0){void*
_tmp43E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp43F;void*_tmp440;
void**_tmp441;struct Cyc_Absyn_Tqual _tmp442;_LL261: if(_tmp43E <= (void*)4)goto
_LL265;if(*((int*)_tmp43E)!= 4)goto _LL263;_tmp43F=((struct Cyc_Absyn_PointerType_struct*)
_tmp43E)->f1;_tmp440=(void*)_tmp43F.elt_typ;_tmp441=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp43E)->f1).elt_typ;_tmp442=_tmp43F.elt_tq;_LL262: topt2=(void**)_tmp441;goto
_LL260;_LL263: if(*((int*)_tmp43E)!= 2)goto _LL265;_LL264: topt2=({void**_tmp443=
_cycalloc(sizeof(*_tmp443));_tmp443[0]=*topt;_tmp443;});goto _LL260;_LL265:;
_LL266: goto _LL260;_LL260:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp445=_cycalloc(sizeof(*_tmp445));
_tmp445[0]=({struct Cyc_Absyn_PointerType_struct _tmp446;_tmp446.tag=4;_tmp446.f1=({
struct Cyc_Absyn_PtrInfo _tmp447;_tmp447.elt_typ=(void*)telt;_tmp447.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp447.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp448;_tmp448.rgn=(void*)rgn;
_tmp448.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp448.bounds=Cyc_Absyn_bounds_one_conref;_tmp448.zero_term=Cyc_Absyn_false_conref;
_tmp448.ptrloc=0;_tmp448;});_tmp447;});_tmp446;});_tmp445;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->v=(
void*)res_typ;_tmp444;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL241:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp449=t;struct Cyc_Absyn_ArrayInfo _tmp44A;void*_tmp44B;struct
Cyc_Absyn_Tqual _tmp44C;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Conref*
_tmp44E;_LL268: if(_tmp449 <= (void*)4)goto _LL26A;if(*((int*)_tmp449)!= 7)goto
_LL26A;_tmp44A=((struct Cyc_Absyn_ArrayType_struct*)_tmp449)->f1;_tmp44B=(void*)
_tmp44A.elt_type;_tmp44C=_tmp44A.tq;_tmp44D=_tmp44A.num_elts;_tmp44E=_tmp44A.zero_term;
_LL269: {void*_tmp450;struct _tuple6 _tmp44F=Cyc_Tcutil_addressof_props(te,e);
_tmp450=_tmp44F.f2;{void*_tmp451=_tmp44D == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp453;_tmp453.tag=0;_tmp453.f1=(struct Cyc_Absyn_Exp*)
_tmp44D;_tmp453;});_tmp452;});t=Cyc_Absyn_atb_typ(_tmp44B,_tmp450,_tmp44C,
_tmp451,_tmp44E);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26A:;_LL26B: return t;_LL267:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);if(Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e)){({
struct Cyc_String_pa_struct _tmp458;_tmp458.tag=0;_tmp458.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct
_tmp457;_tmp457.tag=0;_tmp457.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
struct Cyc_String_pa_struct _tmp456;_tmp456.tag=0;_tmp456.f1=(struct
_dynforward_ptr)(topt == 0?({const char*_tmp459="NULL";_tag_dynforward(_tmp459,
sizeof(char),_get_zero_arr_size(_tmp459,5));}):(struct _dynforward_ptr)Cyc_Absynpp_typ2string(*
topt));{void*_tmp454[3]={& _tmp456,& _tmp457,& _tmp458};Cyc_fprintf(Cyc_stderr,({
const char*_tmp455="topt=%s, e->topt->v=%s, e=%s\n";_tag_dynforward(_tmp455,
sizeof(char),_get_zero_arr_size(_tmp455,30));}),_tag_dynforward(_tmp454,sizeof(
void*),3));}}}});({struct Cyc_Int_pa_struct _tmp45C;_tmp45C.tag=1;_tmp45C.f1=(
unsigned long)2126;{void*_tmp45A[1]={& _tmp45C};Cyc_Tcutil_terr(e->loc,({const
char*_tmp45B="Cannot consume paths; do swap instead %d";_tag_dynforward(_tmp45B,
sizeof(char),_get_zero_arr_size(_tmp45B,41));}),_tag_dynforward(_tmp45A,sizeof(
void*),1));}});}{void*_tmp45D=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp45E;
_LL26D: if(*((int*)_tmp45D)!= 28)goto _LL26F;_LL26E: goto _LL270;_LL26F: if(*((int*)
_tmp45D)!= 29)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp45D)!= 0)goto
_LL273;_tmp45E=((struct Cyc_Absyn_Const_e_struct*)_tmp45D)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp45D)->f1).String_c).tag != 5)goto _LL273;_LL272: return t;_LL273:;_LL274: t=Cyc_Tcutil_compress(
t);{void*_tmp45F=t;struct Cyc_Absyn_ArrayInfo _tmp460;void*_tmp461;struct Cyc_Absyn_Tqual
_tmp462;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Conref*_tmp464;_LL276: if(
_tmp45F <= (void*)4)goto _LL278;if(*((int*)_tmp45F)!= 7)goto _LL278;_tmp460=((
struct Cyc_Absyn_ArrayType_struct*)_tmp45F)->f1;_tmp461=(void*)_tmp460.elt_type;
_tmp462=_tmp460.tq;_tmp463=_tmp460.num_elts;_tmp464=_tmp460.zero_term;_LL277: {
void*_tmp466;struct _tuple6 _tmp465=Cyc_Tcutil_addressof_props(te,e);_tmp466=
_tmp465.f2;{void*b=_tmp463 == 0?(void*)((void*)0):(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp468;_tmp468.tag=0;_tmp468.f1=(struct Cyc_Absyn_Exp*)_tmp463;_tmp468;});
_tmp467;});t=Cyc_Absyn_atb_typ(_tmp461,_tmp466,_tmp462,b,_tmp464);Cyc_Tcutil_unchecked_cast(
te,e,t,(void*)3);return t;}}_LL278:;_LL279: return t;_LL275:;}_LL26C:;}}static void*
Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp469=(void*)e->r;struct Cyc_Absyn_Exp*_tmp46A;_LL27B: if(*((int*)
_tmp469)!= 13)goto _LL27D;_tmp46A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp469)->f1;_LL27C: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp46A);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp46A->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp46A->topt))->v,0));e->topt=_tmp46A->topt;goto
_LL27A;_LL27D:;_LL27E: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp46B=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp46C;void*
_tmp46D;struct Cyc_Absyn_Tqual _tmp46E;struct Cyc_Absyn_PtrAtts _tmp46F;void*_tmp470;
struct Cyc_Absyn_Conref*_tmp471;struct Cyc_Absyn_Conref*_tmp472;struct Cyc_Absyn_Conref*
_tmp473;_LL280: if(_tmp46B <= (void*)4)goto _LL282;if(*((int*)_tmp46B)!= 4)goto
_LL282;_tmp46C=((struct Cyc_Absyn_PointerType_struct*)_tmp46B)->f1;_tmp46D=(void*)
_tmp46C.elt_typ;_tmp46E=_tmp46C.elt_tq;_tmp46F=_tmp46C.ptr_atts;_tmp470=(void*)
_tmp46F.rgn;_tmp471=_tmp46F.nullable;_tmp472=_tmp46F.bounds;_tmp473=_tmp46F.zero_term;
_LL281:{void*_tmp474=Cyc_Tcutil_compress(_tmp46D);struct Cyc_Absyn_FnInfo _tmp475;
struct Cyc_List_List*_tmp476;struct Cyc_Core_Opt*_tmp477;void*_tmp478;struct Cyc_List_List*
_tmp479;int _tmp47A;struct Cyc_Absyn_VarargInfo*_tmp47B;struct Cyc_List_List*
_tmp47C;struct Cyc_List_List*_tmp47D;_LL285: if(_tmp474 <= (void*)4)goto _LL287;if(*((
int*)_tmp474)!= 8)goto _LL287;_tmp475=((struct Cyc_Absyn_FnType_struct*)_tmp474)->f1;
_tmp476=_tmp475.tvars;_tmp477=_tmp475.effect;_tmp478=(void*)_tmp475.ret_typ;
_tmp479=_tmp475.args;_tmp47A=_tmp475.c_varargs;_tmp47B=_tmp475.cyc_varargs;
_tmp47C=_tmp475.rgn_po;_tmp47D=_tmp475.attributes;_LL286: if(_tmp476 != 0){struct
_RegionHandle _tmp47E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp47E;
_push_region(rgn);{struct _tuple4 _tmp47F=({struct _tuple4 _tmp48C;_tmp48C.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp48C.f2=rgn;_tmp48C;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp47F,_tmp476);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Absyn_FnType_struct
_tmp48A;_tmp48A.tag=8;_tmp48A.f1=({struct Cyc_Absyn_FnInfo _tmp48B;_tmp48B.tvars=0;
_tmp48B.effect=_tmp477;_tmp48B.ret_typ=(void*)_tmp478;_tmp48B.args=_tmp479;
_tmp48B.c_varargs=_tmp47A;_tmp48B.cyc_varargs=_tmp47B;_tmp48B.rgn_po=_tmp47C;
_tmp48B.attributes=_tmp47D;_tmp48B;});_tmp48A;});_tmp489;}));struct Cyc_Absyn_PointerType_struct*
_tmp480=({struct Cyc_Absyn_PointerType_struct*_tmp485=_cycalloc(sizeof(*_tmp485));
_tmp485[0]=({struct Cyc_Absyn_PointerType_struct _tmp486;_tmp486.tag=4;_tmp486.f1=({
struct Cyc_Absyn_PtrInfo _tmp487;_tmp487.elt_typ=(void*)ftyp;_tmp487.elt_tq=
_tmp46E;_tmp487.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp488;_tmp488.rgn=(void*)
_tmp470;_tmp488.nullable=_tmp471;_tmp488.bounds=_tmp472;_tmp488.zero_term=
_tmp473;_tmp488.ptrloc=0;_tmp488;});_tmp487;});_tmp486;});_tmp485;});struct Cyc_Absyn_Exp*
_tmp481=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp483;_tmp483.tag=14;_tmp483.f1=_tmp481;_tmp483.f2=ts;_tmp483;});_tmp482;})));
e->topt=({struct Cyc_Core_Opt*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->v=(
void*)((void*)_tmp480);_tmp484;});};_pop_region(rgn);}goto _LL284;_LL287:;_LL288:
goto _LL284;_LL284:;}goto _LL27F;_LL282:;_LL283: goto _LL27F;_LL27F:;}goto _LL27A;
_LL27A:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp48D=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp48E;struct _tuple1*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_List_List*
_tmp491;struct Cyc_Core_Opt*_tmp492;struct Cyc_List_List*_tmp493;union Cyc_Absyn_Cnst_union
_tmp494;union Cyc_Absyn_Cnst_union*_tmp495;struct _tuple1*_tmp496;void*_tmp497;
void*_tmp498;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;void*
_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Core_Opt*_tmp49D;struct Cyc_Absyn_Exp*
_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*
_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*
_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*
_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_List_List*_tmp4A9;struct Cyc_Absyn_VarargCallInfo*
_tmp4AA;struct Cyc_Absyn_VarargCallInfo**_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;
struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_List_List*_tmp4AE;void*_tmp4AF;struct Cyc_Absyn_Exp*
_tmp4B0;void*_tmp4B1;void**_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;void*_tmp4B7;
void*_tmp4B8;void*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Exp*
_tmp4BB;struct _dynforward_ptr*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;struct
_dynforward_ptr*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;
struct Cyc_List_List*_tmp4C1;struct _tuple2*_tmp4C2;struct Cyc_List_List*_tmp4C3;
struct Cyc_List_List*_tmp4C4;struct Cyc_Absyn_Stmt*_tmp4C5;struct Cyc_Absyn_Vardecl*
_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;int _tmp4C9;int*
_tmp4CA;struct _tuple1*_tmp4CB;struct _tuple1**_tmp4CC;struct Cyc_List_List*_tmp4CD;
struct Cyc_List_List**_tmp4CE;struct Cyc_List_List*_tmp4CF;struct Cyc_Absyn_Aggrdecl*
_tmp4D0;struct Cyc_Absyn_Aggrdecl**_tmp4D1;void*_tmp4D2;struct Cyc_List_List*
_tmp4D3;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_Tuniondecl*_tmp4D5;struct Cyc_Absyn_Tunionfield*
_tmp4D6;struct _tuple1*_tmp4D7;struct _tuple1**_tmp4D8;struct Cyc_Absyn_Enumdecl*
_tmp4D9;struct Cyc_Absyn_Enumfield*_tmp4DA;struct _tuple1*_tmp4DB;struct _tuple1**
_tmp4DC;void*_tmp4DD;struct Cyc_Absyn_Enumfield*_tmp4DE;struct Cyc_Absyn_MallocInfo
_tmp4DF;int _tmp4E0;int*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;void**_tmp4E3;void***
_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp**_tmp4E6;int _tmp4E7;int*
_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;_LL28A: if(*((int*)
_tmp48D)!= 13)goto _LL28C;_tmp48E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp48D)->f1;_LL28B: Cyc_Tcexp_tcExpNoInst(te,0,_tmp48E);return;_LL28C: if(*((int*)
_tmp48D)!= 2)goto _LL28E;_tmp48F=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp48D)->f1;
_LL28D: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp48F);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL28E: if(*((int*)_tmp48D)!= 10)goto _LL290;_tmp490=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp48D)->f1;_tmp491=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp48D)->f2;_LL28F:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp490,_tmp491);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp48D)!= 37)goto _LL292;_tmp492=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp48D)->f1;_tmp493=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp48D)->f2;
_LL291: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp493);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL292: if(*((int*)_tmp48D)!= 0)goto _LL294;_tmp494=((struct Cyc_Absyn_Const_e_struct*)
_tmp48D)->f1;_tmp495=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp48D)->f1;_LL293: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp495,e);goto _LL289;_LL294: if(*((int*)_tmp48D)!= 1)goto _LL296;_tmp496=((struct
Cyc_Absyn_Var_e_struct*)_tmp48D)->f1;_tmp497=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp48D)->f2;_LL295: t=Cyc_Tcexp_tcVar(te,loc,_tmp496,_tmp497);goto _LL289;_LL296:
if(*((int*)_tmp48D)!= 3)goto _LL298;_tmp498=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp48D)->f1;_tmp499=((struct Cyc_Absyn_Primop_e_struct*)_tmp48D)->f2;_LL297: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp498,_tmp499);goto _LL289;_LL298: if(*((int*)
_tmp48D)!= 5)goto _LL29A;_tmp49A=((struct Cyc_Absyn_Increment_e_struct*)_tmp48D)->f1;
_tmp49B=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp48D)->f2;_LL299: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp49A,_tmp49B);goto _LL289;_LL29A: if(*((int*)_tmp48D)!= 4)goto
_LL29C;_tmp49C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp48D)->f1;_tmp49D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp48D)->f2;_tmp49E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp48D)->f3;_LL29B: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp49C,_tmp49D,_tmp49E);
goto _LL289;_LL29C: if(*((int*)_tmp48D)!= 6)goto _LL29E;_tmp49F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp48D)->f1;_tmp4A0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp48D)->f2;
_tmp4A1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp48D)->f3;_LL29D: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp49F,_tmp4A0,_tmp4A1);goto _LL289;_LL29E: if(*((int*)_tmp48D)!= 7)
goto _LL2A0;_tmp4A2=((struct Cyc_Absyn_And_e_struct*)_tmp48D)->f1;_tmp4A3=((struct
Cyc_Absyn_And_e_struct*)_tmp48D)->f2;_LL29F: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4A2,
_tmp4A3);goto _LL289;_LL2A0: if(*((int*)_tmp48D)!= 8)goto _LL2A2;_tmp4A4=((struct
Cyc_Absyn_Or_e_struct*)_tmp48D)->f1;_tmp4A5=((struct Cyc_Absyn_Or_e_struct*)
_tmp48D)->f2;_LL2A1: t=Cyc_Tcexp_tcOr(te,loc,_tmp4A4,_tmp4A5);goto _LL289;_LL2A2:
if(*((int*)_tmp48D)!= 9)goto _LL2A4;_tmp4A6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp48D)->f1;_tmp4A7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp48D)->f2;_LL2A3: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4A6,_tmp4A7);goto _LL289;_LL2A4: if(*((int*)
_tmp48D)!= 11)goto _LL2A6;_tmp4A8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp48D)->f1;
_tmp4A9=((struct Cyc_Absyn_FnCall_e_struct*)_tmp48D)->f2;_tmp4AA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp48D)->f3;_tmp4AB=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp48D)->f3;_LL2A5: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4A8,_tmp4A9,_tmp4AB);
goto _LL289;_LL2A6: if(*((int*)_tmp48D)!= 12)goto _LL2A8;_tmp4AC=((struct Cyc_Absyn_Throw_e_struct*)
_tmp48D)->f1;_LL2A7: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4AC);goto _LL289;_LL2A8:
if(*((int*)_tmp48D)!= 14)goto _LL2AA;_tmp4AD=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp48D)->f1;_tmp4AE=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp48D)->f2;_LL2A9:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4AD,_tmp4AE);goto _LL289;_LL2AA: if(*((
int*)_tmp48D)!= 15)goto _LL2AC;_tmp4AF=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp48D)->f1;_tmp4B0=((struct Cyc_Absyn_Cast_e_struct*)_tmp48D)->f2;_tmp4B1=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp48D)->f4;_tmp4B2=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp48D)->f4);_LL2AB: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4AF,_tmp4B0,(void**)
_tmp4B2);goto _LL289;_LL2AC: if(*((int*)_tmp48D)!= 16)goto _LL2AE;_tmp4B3=((struct
Cyc_Absyn_Address_e_struct*)_tmp48D)->f1;_LL2AD: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4B3);goto _LL289;_LL2AE: if(*((int*)_tmp48D)!= 17)goto _LL2B0;_tmp4B4=((
struct Cyc_Absyn_New_e_struct*)_tmp48D)->f1;_tmp4B5=((struct Cyc_Absyn_New_e_struct*)
_tmp48D)->f2;_LL2AF: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4B4,e,_tmp4B5);goto _LL289;
_LL2B0: if(*((int*)_tmp48D)!= 19)goto _LL2B2;_tmp4B6=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp48D)->f1;_LL2B1: {void*_tmp4EB=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4B6);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4EB);goto _LL289;}_LL2B2: if(*((int*)_tmp48D)!= 18)goto _LL2B4;
_tmp4B7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp48D)->f1;_LL2B3: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4B7);goto _LL289;_LL2B4: if(*((int*)_tmp48D)!= 20)goto _LL2B6;
_tmp4B8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp48D)->f1;_tmp4B9=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp48D)->f2;_LL2B5: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4B8,_tmp4B9);goto _LL289;_LL2B6: if(*((int*)_tmp48D)!= 21)goto _LL2B8;
_LL2B7:({void*_tmp4EC[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4ED="gen() not in top-level initializer";
_tag_dynforward(_tmp4ED,sizeof(char),_get_zero_arr_size(_tmp4ED,35));}),
_tag_dynforward(_tmp4EC,sizeof(void*),0));});return;_LL2B8: if(*((int*)_tmp48D)!= 
22)goto _LL2BA;_tmp4BA=((struct Cyc_Absyn_Deref_e_struct*)_tmp48D)->f1;_LL2B9: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4BA);goto _LL289;_LL2BA: if(*((int*)_tmp48D)!= 
23)goto _LL2BC;_tmp4BB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp48D)->f1;
_tmp4BC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp48D)->f2;_LL2BB: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp4BB,_tmp4BC);goto _LL289;_LL2BC: if(*((int*)_tmp48D)!= 24)goto
_LL2BE;_tmp4BD=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp48D)->f1;_tmp4BE=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp48D)->f2;_LL2BD: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp4BD,_tmp4BE);goto _LL289;_LL2BE: if(*((int*)_tmp48D)!= 25)goto
_LL2C0;_tmp4BF=((struct Cyc_Absyn_Subscript_e_struct*)_tmp48D)->f1;_tmp4C0=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp48D)->f2;_LL2BF: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4BF,_tmp4C0);goto _LL289;_LL2C0: if(*((int*)_tmp48D)!= 26)goto
_LL2C2;_tmp4C1=((struct Cyc_Absyn_Tuple_e_struct*)_tmp48D)->f1;_LL2C1: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4C1);goto _LL289;_LL2C2: if(*((int*)_tmp48D)!= 27)goto _LL2C4;
_tmp4C2=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp48D)->f1;_tmp4C3=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp48D)->f2;_LL2C3: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4C2,_tmp4C3);goto
_LL289;_LL2C4: if(*((int*)_tmp48D)!= 28)goto _LL2C6;_tmp4C4=((struct Cyc_Absyn_Array_e_struct*)
_tmp48D)->f1;_LL2C5: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4EE=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4EF;void*_tmp4F0;void**
_tmp4F1;struct Cyc_Absyn_Conref*_tmp4F2;_LL2D9: if(_tmp4EE <= (void*)4)goto _LL2DB;
if(*((int*)_tmp4EE)!= 7)goto _LL2DB;_tmp4EF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4EE)->f1;_tmp4F0=(void*)_tmp4EF.elt_type;_tmp4F1=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4EE)->f1).elt_type;_tmp4F2=_tmp4EF.zero_term;_LL2DA: elt_topt=(void**)_tmp4F1;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4F2);
goto _LL2D8;_LL2DB:;_LL2DC: goto _LL2D8;_LL2D8:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4C4);goto _LL289;}_LL2C6: if(*((int*)_tmp48D)!= 38)goto
_LL2C8;_tmp4C5=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp48D)->f1;_LL2C7: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4C5);goto _LL289;_LL2C8: if(*((int*)_tmp48D)!= 29)goto _LL2CA;
_tmp4C6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48D)->f1;_tmp4C7=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp48D)->f2;_tmp4C8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48D)->f3;_tmp4C9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48D)->f4;
_tmp4CA=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48D)->f4;_LL2C9: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4C6,_tmp4C7,_tmp4C8,_tmp4CA);goto
_LL289;_LL2CA: if(*((int*)_tmp48D)!= 30)goto _LL2CC;_tmp4CB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48D)->f1;_tmp4CC=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp48D)->f1;_tmp4CD=((struct Cyc_Absyn_Struct_e_struct*)_tmp48D)->f2;_tmp4CE=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp48D)->f2;_tmp4CF=((
struct Cyc_Absyn_Struct_e_struct*)_tmp48D)->f3;_tmp4D0=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48D)->f4;_tmp4D1=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp48D)->f4;_LL2CB: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4CC,_tmp4CE,_tmp4CF,
_tmp4D1);goto _LL289;_LL2CC: if(*((int*)_tmp48D)!= 31)goto _LL2CE;_tmp4D2=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp48D)->f1;_tmp4D3=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp48D)->f2;_LL2CD: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4D2,_tmp4D3);goto _LL289;
_LL2CE: if(*((int*)_tmp48D)!= 32)goto _LL2D0;_tmp4D4=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp48D)->f1;_tmp4D5=((struct Cyc_Absyn_Tunion_e_struct*)_tmp48D)->f2;_tmp4D6=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp48D)->f3;_LL2CF: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4D4,_tmp4D5,_tmp4D6);goto _LL289;_LL2D0: if(*((int*)_tmp48D)!= 33)goto
_LL2D2;_tmp4D7=((struct Cyc_Absyn_Enum_e_struct*)_tmp48D)->f1;_tmp4D8=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp48D)->f1;_tmp4D9=((struct Cyc_Absyn_Enum_e_struct*)
_tmp48D)->f2;_tmp4DA=((struct Cyc_Absyn_Enum_e_struct*)_tmp48D)->f3;_LL2D1:*
_tmp4D8=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4DA))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4F4;_tmp4F4.tag=12;_tmp4F4.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4D9))->name;_tmp4F4.f2=_tmp4D9;_tmp4F4;});_tmp4F3;});goto _LL289;
_LL2D2: if(*((int*)_tmp48D)!= 34)goto _LL2D4;_tmp4DB=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp48D)->f1;_tmp4DC=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp48D)->f1;_tmp4DD=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp48D)->f2;
_tmp4DE=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp48D)->f3;_LL2D3:*_tmp4DC=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4DE))->name;t=_tmp4DD;goto _LL289;
_LL2D4: if(*((int*)_tmp48D)!= 35)goto _LL2D6;_tmp4DF=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48D)->f1;_tmp4E0=_tmp4DF.is_calloc;_tmp4E1=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48D)->f1).is_calloc;_tmp4E2=_tmp4DF.rgn;_tmp4E3=_tmp4DF.elt_type;_tmp4E4=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp48D)->f1).elt_type;_tmp4E5=
_tmp4DF.num_elts;_tmp4E6=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48D)->f1).num_elts;_tmp4E7=_tmp4DF.fat_result;_tmp4E8=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48D)->f1).fat_result;_LL2D5: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4E2,_tmp4E4,
_tmp4E6,_tmp4E1,_tmp4E8);goto _LL289;_LL2D6: if(*((int*)_tmp48D)!= 36)goto _LL289;
_tmp4E9=((struct Cyc_Absyn_Swap_e_struct*)_tmp48D)->f1;_tmp4EA=((struct Cyc_Absyn_Swap_e_struct*)
_tmp48D)->f2;_LL2D7: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E9,_tmp4EA);goto _LL289;
_LL289:;}e->topt=({struct Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));
_tmp4F5->v=(void*)t;_tmp4F5;});}
