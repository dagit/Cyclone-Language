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
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
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
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr
s2);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct
_dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*
k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern
struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern
struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int
Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;union Cyc_Absyn_AggrInfoU_union _tmp6;
void*_tmp7;struct _tuple1*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*
_tmpA;struct Cyc_List_List**_tmpB;struct Cyc_List_List*_tmpC;int _tmpD;struct Cyc_Absyn_Exp*
_tmpE;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct _handler_cons _tmpF;
_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){{
struct _RegionHandle _tmp12=_new_region("r");struct _RegionHandle*r=& _tmp12;
_push_region(r);{void*_tmp13=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp1);struct
Cyc_Absyn_Tuniondecl*_tmp14;struct Cyc_Absyn_Tunionfield*_tmp15;struct Cyc_Absyn_Enumdecl*
_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;void*_tmp18;struct Cyc_Absyn_Enumfield*
_tmp19;_LLC: if(*((int*)_tmp13)!= 1)goto _LLE;_LLD:(void*)(p->r=(void*)((void*)0));({
void*_tmp1A[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp1B="struct tag used without arguments in pattern";
_tag_dynforward(_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,45));}),
_tag_dynforward(_tmp1A,sizeof(void*),0));});_npop_handler(1);return;_LLE: if(*((
int*)_tmp13)!= 2)goto _LL10;_tmp14=((struct Cyc_Tcenv_TunionRes_struct*)_tmp13)->f1;
_tmp15=((struct Cyc_Tcenv_TunionRes_struct*)_tmp13)->f2;_LLF:(void*)(p->r=(void*)((
void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[
0]=({struct Cyc_Absyn_Tunion_p_struct _tmp1D;_tmp1D.tag=6;_tmp1D.f1=_tmp14;_tmp1D.f2=
_tmp15;_tmp1D.f3=0;_tmp1D.f4=0;_tmp1D;});_tmp1C;})));_npop_handler(1);return;
_LL10: if(*((int*)_tmp13)!= 3)goto _LL12;_tmp16=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp13)->f1;_tmp17=((struct Cyc_Tcenv_EnumRes_struct*)_tmp13)->f2;_LL11:(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Enum_p_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));
_tmp1E[0]=({struct Cyc_Absyn_Enum_p_struct _tmp1F;_tmp1F.tag=10;_tmp1F.f1=_tmp16;
_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));_npop_handler(1);return;_LL12: if(*((int*)
_tmp13)!= 4)goto _LL14;_tmp18=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f1;
_tmp19=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f2;_LL13:(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*_tmp20=_cycalloc(sizeof(*
_tmp20));_tmp20[0]=({struct Cyc_Absyn_AnonEnum_p_struct _tmp21;_tmp21.tag=11;
_tmp21.f1=(void*)_tmp18;_tmp21.f2=_tmp19;_tmp21;});_tmp20;})));_npop_handler(1);
return;_LL14: if(*((int*)_tmp13)!= 0)goto _LLB;_LL15: goto _LLB;_LLB:;};_pop_region(
r);};_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp23=_tmp10;
_LL17: if(_tmp23 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;_LL1A:(void)
_throw(_tmp23);_LL16:;}}}{union Cyc_Absyn_Nmspace_union _tmp24=(*_tmp1).f1;struct
Cyc_List_List*_tmp25;_LL1C: if((_tmp24.Loc_n).tag != 0)goto _LL1E;_LL1D: goto _LL1F;
_LL1E: if((_tmp24.Rel_n).tag != 1)goto _LL20;_tmp25=(_tmp24.Rel_n).f1;if(_tmp25 != 0)
goto _LL20;_LL1F:(*_tmp1).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp26;(_tmp26.Loc_n).tag=0;_tmp26;});(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Absyn_Var_p_struct _tmp28;
_tmp28.tag=0;_tmp28.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0);_tmp28.f2=Cyc_Absyn_new_pat((
void*)0,0);_tmp28;});_tmp27;})));return;_LL20:;_LL21:({void*_tmp29[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp2A="qualified variable in pattern";_tag_dynforward(_tmp2A,
sizeof(char),_get_zero_arr_size(_tmp2A,30));}),_tag_dynforward(_tmp29,sizeof(
void*),0));});return;_LL1B:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct
Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f2;_tmp4=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct
_handler_cons _tmp2B;_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))
_tmp2D=1;if(!_tmp2D){{struct _RegionHandle _tmp2E=_new_region("r");struct
_RegionHandle*r=& _tmp2E;_push_region(r);{void*_tmp2F=Cyc_Tcenv_lookup_ordinary(r,
te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*_tmp30;struct Cyc_Absyn_Tuniondecl*
_tmp31;struct Cyc_Absyn_Tunionfield*_tmp32;_LL23: if(*((int*)_tmp2F)!= 1)goto _LL25;
_tmp30=((struct Cyc_Tcenv_AggrRes_struct*)_tmp2F)->f1;_LL24: if((void*)_tmp30->kind
== (void*)1){(void*)(p->r=(void*)((void*)0));({void*_tmp33[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp34="cannot pattern-match a union";_tag_dynforward(_tmp34,
sizeof(char),_get_zero_arr_size(_tmp34,29));}),_tag_dynforward(_tmp33,sizeof(
void*),0));});_npop_handler(1);return;}{struct Cyc_List_List*_tmp35=0;for(0;_tmp3
!= 0;_tmp3=_tmp3->tl){_tmp35=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*
_tmp36));_tmp36->hd=({struct _tuple8*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=
0;_tmp37->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp37;});_tmp36->tl=_tmp35;_tmp36;});}(
void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp38=_cycalloc(
sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp39;_tmp39.tag=5;
_tmp39.f1=({struct Cyc_Absyn_AggrInfo _tmp3A;_tmp3A.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp3B;(_tmp3B.KnownAggr).tag=1;(_tmp3B.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=_tmp30;
_tmp3C;});_tmp3B;});_tmp3A.targs=0;_tmp3A;});_tmp39.f2=0;_tmp39.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp35);_tmp39.f4=_tmp4;_tmp39;});
_tmp38;})));_npop_handler(1);return;}_LL25: if(*((int*)_tmp2F)!= 2)goto _LL27;
_tmp31=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2F)->f1;_tmp32=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp2F)->f2;_LL26:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp3E;_tmp3E.tag=6;_tmp3E.f1=_tmp31;_tmp3E.f2=_tmp32;_tmp3E.f3=_tmp3;_tmp3E.f4=
_tmp4;_tmp3E;});_tmp3D;})));_npop_handler(1);return;_LL27: if(*((int*)_tmp2F)!= 3)
goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)_tmp2F)!= 4)goto _LL2B;_LL2A:({void*
_tmp3F[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp40="enum tag used with arguments in pattern";
_tag_dynforward(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,40));}),
_tag_dynforward(_tmp3F,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(1);return;_LL2B: if(*((int*)_tmp2F)!= 0)goto _LL22;_LL2C: goto _LL22;
_LL22:;};_pop_region(r);};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;
void*_tmp42=_tmp2C;_LL2E: if(_tmp42 != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;
_LL30:;_LL31:(void)_throw(_tmp42);_LL2D:;}}}({struct Cyc_String_pa_struct _tmp45;
_tmp45.tag=0;_tmp45.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp2));{void*_tmp43[1]={& _tmp45};Cyc_Tcutil_terr(p->loc,({const char*_tmp44="%s is not a constructor in pattern";
_tag_dynforward(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,35));}),
_tag_dynforward(_tmp43,sizeof(void*),1));}});(void*)(p->r=(void*)((void*)0));
return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp6=_tmp5.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL7;_tmp7=(_tmp6.UnknownAggr).f1;_tmp8=(_tmp6.UnknownAggr).f2;_tmp9=
_tmp5.targs;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpB=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_tmpD=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f4;_LL6:{struct
_handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))
_tmp48=1;if(!_tmp48){{struct Cyc_Absyn_Aggrdecl**_tmp49=Cyc_Tcenv_lookup_aggrdecl(
te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp4A=*_tmp49;if((void*)_tmp4A->kind
== (void*)1){({void*_tmp4B[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4C="cannot pattern-match a union";
_tag_dynforward(_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,29));}),
_tag_dynforward(_tmp4B,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(0);return;}if(_tmp4A->impl == 0){({void*_tmp4D[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp4E="can't destructure an abstract struct";_tag_dynforward(
_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,37));}),_tag_dynforward(_tmp4D,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}{
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4A->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*
_tmpB);if(more_exists < 0){({void*_tmp4F[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp50="too many existentially bound type variables in pattern";_tag_dynforward(
_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,55));}),_tag_dynforward(_tmp4F,
sizeof(void*),0));});{struct Cyc_List_List**_tmp51=_tmpB;{int n=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4A->impl))->exist_vars);
for(0;n != 0;n --){_tmp51=&((struct Cyc_List_List*)_check_null(*_tmp51))->tl;}}*
_tmp51=0;}}else{if(more_exists > 0){struct Cyc_List_List*_tmp52=0;for(0;
more_exists != 0;more_exists --){_tmp52=({struct Cyc_List_List*_tmp53=_cycalloc(
sizeof(*_tmp53));_tmp53->hd=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp55;_tmp55.tag=1;_tmp55.f1=0;_tmp55;});_tmp54;}));_tmp53->tl=_tmp52;_tmp53;});}*
_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*
_tmpB,_tmp52);}}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp57;_tmp57.tag=5;_tmp57.f1=({struct Cyc_Absyn_AggrInfo _tmp58;_tmp58.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp59;(_tmp59.KnownAggr).tag=
1;(_tmp59.KnownAggr).f1=_tmp49;_tmp59;});_tmp58.targs=_tmp9;_tmp58;});_tmp57.f2=*
_tmpB;_tmp57.f3=_tmpC;_tmp57.f4=_tmpD;_tmp57;});_tmp56;})));}};_pop_handler();}
else{void*_tmp47=(void*)_exn_thrown;void*_tmp5B=_tmp47;_LL33: if(_tmp5B != Cyc_Dict_Absent)
goto _LL35;_LL34:({void*_tmp5C[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp5D="Non-struct name has designator patterns";
_tag_dynforward(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,40));}),
_tag_dynforward(_tmp5C,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));goto
_LL32;_LL35:;_LL36:(void)_throw(_tmp5B);_LL32:;}}}return;_LL7: if(*((int*)_tmp0)
!= 14)goto _LL9;_tmpE=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(
te,0,_tmpE);if(!Cyc_Tcutil_is_const_exp(te,_tmpE)){({void*_tmp5E[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp5F="non-constant expression in case pattern";
_tag_dynforward(_tmp5F,sizeof(char),_get_zero_arr_size(_tmp5F,40));}),
_tag_dynforward(_tmp5E,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));}{
unsigned int _tmp61;int _tmp62;struct _tuple7 _tmp60=Cyc_Evexp_eval_const_uint_exp(
_tmpE);_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_Int_p_struct _tmp64;
_tmp64.tag=7;_tmp64.f1=(void*)((void*)2);_tmp64.f2=(int)_tmp61;_tmp64;});_tmp63;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _dynforward_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp65=_cycalloc(sizeof(*
_tmp65));_tmp65->v=s;_tmp65;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp66=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp66 <= (void*)4)goto _LL3A;if(*((int*)_tmp66)!= 12)goto _LL3A;
_LL39: if(topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp67=_cycalloc(
sizeof(*_tmp67));_tmp67->hd=vd;_tmp67->tl=*v_result_ptr;_tmp67;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp69;struct Cyc_List_List*_tmp6A;
struct Cyc_Tcpat_TcPatResult _tmp68=res1;_tmp69=_tmp68.tvars_and_bounds_opt;_tmp6A=
_tmp68.patvars;{struct _tuple6*_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_Tcpat_TcPatResult
_tmp6B=res2;_tmp6C=_tmp6B.tvars_and_bounds_opt;_tmp6D=_tmp6B.patvars;if(_tmp69 != 
0  || _tmp6C != 0){if(_tmp69 == 0)_tmp69=({struct _tuple6*_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E->f1=0;_tmp6E->f2=0;_tmp6E;});if(_tmp6C == 0)_tmp6C=({struct
_tuple6*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=0;_tmp6F->f2=0;_tmp6F;});
return({struct Cyc_Tcpat_TcPatResult _tmp70;_tmp70.tvars_and_bounds_opt=({struct
_tuple6*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp69))).f1,(*((struct _tuple6*)_check_null(_tmp6C))).f1);_tmp71->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp69))).f2,(*((struct _tuple6*)_check_null(_tmp6C))).f2);
_tmp71;});_tmp70.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp6A,_tmp6D);_tmp70;});}return({struct Cyc_Tcpat_TcPatResult
_tmp72;_tmp72.tvars_and_bounds_opt=0;_tmp72.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6A,_tmp6D);
_tmp72;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->loc=
loc;_tmp73->topt=0;_tmp73->r=(void*)((void*)0);_tmp73;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp140;
_tmp140.tvars_and_bounds_opt=0;_tmp140.patvars=0;_tmp140;});{void*_tmp74=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp75;struct Cyc_Absyn_Pat*_tmp76;struct Cyc_Absyn_Vardecl*
_tmp77;struct Cyc_Absyn_Pat*_tmp78;struct Cyc_Absyn_Tvar*_tmp79;struct Cyc_Absyn_Vardecl*
_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;struct Cyc_Absyn_Enumdecl*_tmp7E;void*
_tmp7F;struct Cyc_Absyn_Pat*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List**
_tmp82;int _tmp83;struct Cyc_Absyn_AggrInfo _tmp84;union Cyc_Absyn_AggrInfoU_union
_tmp85;struct Cyc_Absyn_Aggrdecl**_tmp86;struct Cyc_Absyn_Aggrdecl*_tmp87;struct
Cyc_List_List*_tmp88;struct Cyc_List_List**_tmp89;struct Cyc_List_List*_tmp8A;
struct Cyc_List_List*_tmp8B;struct Cyc_List_List**_tmp8C;int _tmp8D;struct Cyc_Absyn_Tuniondecl*
_tmp8E;struct Cyc_Absyn_Tunionfield*_tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_List_List**
_tmp91;int _tmp92;struct Cyc_Absyn_AggrInfo _tmp93;union Cyc_Absyn_AggrInfoU_union
_tmp94;_LL3D: if((int)_tmp74 != 0)goto _LL3F;_LL3E: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3C;_LL3F: if(_tmp74 <= (void*)2)goto _LL53;if(*((int*)_tmp74)!= 0)
goto _LL41;_tmp75=((struct Cyc_Absyn_Var_p_struct*)_tmp74)->f1;_tmp76=((struct Cyc_Absyn_Var_p_struct*)
_tmp74)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(te,_tmp76,topt,rgn_opt);t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp76->topt))->v;Cyc_Tcpat_set_vd(_tmp75,& res.patvars,
t);goto _LL3C;_LL41: if(*((int*)_tmp74)!= 1)goto _LL43;_tmp77=((struct Cyc_Absyn_Reference_p_struct*)
_tmp74)->f1;_tmp78=((struct Cyc_Absyn_Reference_p_struct*)_tmp74)->f2;_LL42: res=
Cyc_Tcpat_tcPatRec(te,_tmp78,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp78->topt))->v;if(rgn_opt == 0){({void*_tmp95[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp96="* pattern would point into an unknown/unallowed region";
_tag_dynforward(_tmp96,sizeof(char),_get_zero_arr_size(_tmp96,55));}),
_tag_dynforward(_tmp95,sizeof(void*),0));});goto _LL3C;}Cyc_Tcpat_set_vd(_tmp77,&
res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97[0]=({struct Cyc_Absyn_PointerType_struct _tmp98;_tmp98.tag=4;
_tmp98.f1=({struct Cyc_Absyn_PtrInfo _tmp99;_tmp99.elt_typ=(void*)t;_tmp99.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp99.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9A;_tmp9A.rgn=(
void*)*rgn_opt;_tmp9A.nullable=Cyc_Absyn_false_conref;_tmp9A.bounds=Cyc_Absyn_empty_conref();
_tmp9A.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp9A.ptrloc=
0;_tmp9A;});_tmp99;});_tmp98;});_tmp97;}));goto _LL3C;_LL43: if(*((int*)_tmp74)!= 
2)goto _LL45;_tmp79=((struct Cyc_Absyn_TagInt_p_struct*)_tmp74)->f1;_tmp7A=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp74)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp7A,& res.patvars,(
void*)_tmp7A->type);{struct _RegionHandle _tmp9B=_new_region("r2");struct
_RegionHandle*r2=& _tmp9B;_push_region(r2);Cyc_Tcenv_add_type_vars(r2,p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp9C[1];_tmp9C[0]=_tmp79;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9C,sizeof(struct Cyc_Absyn_Tvar*),
1));}));;_pop_region(r2);}if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=0;_tmp9D->f2=0;_tmp9D;});(*
res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*
_tmp9E));_tmp9E->hd=_tmp79;_tmp9E->tl=(*res.tvars_and_bounds_opt).f1;_tmp9E;});t=
Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(*((int*)_tmp74)!= 7)goto _LL47;_tmp7B=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)_tmp7B != 1)goto _LL47;_LL46: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47: if(*((int*)_tmp74)!= 
7)goto _LL49;_tmp7C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)
_tmp7C != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp74)!= 7)goto _LL4B;
_tmp7D=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)_tmp7D != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(*((int*)_tmp74)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp74)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp74)!= 10)goto _LL51;_tmp7E=((
struct Cyc_Absyn_Enum_p_struct*)_tmp74)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({
struct Cyc_Absyn_EnumType_struct _tmpA0;_tmpA0.tag=12;_tmpA0.f1=_tmp7E->name;
_tmpA0.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7E;_tmpA0;});_tmp9F;}));goto _LL3C;_LL51:
if(*((int*)_tmp74)!= 11)goto _LL53;_tmp7F=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp74)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp7F);goto _LL3C;_LL53: if((int)
_tmp74 != 1)goto _LL55;_LL54: if(topt != 0){void*_tmpA1=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmpA1 <= (void*)4)goto _LL68;if(*((int*)_tmpA1)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_Core_Opt*_tmpA2=Cyc_Tcenv_lookup_opt_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpA3=_cycalloc(sizeof(*
_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_PointerType_struct _tmpA4;_tmpA4.tag=4;
_tmpA4.f1=({struct Cyc_Absyn_PtrInfo _tmpA5;_tmpA5.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmpA2);_tmpA5.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmpA5.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA6;_tmpA6.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpA2);_tmpA6.nullable=Cyc_Absyn_true_conref;
_tmpA6.bounds=Cyc_Absyn_empty_conref();_tmpA6.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA6.ptrloc=0;_tmpA6;});_tmpA5;});_tmpA4;});_tmpA3;});
goto _LL3C;}_LL55: if(_tmp74 <= (void*)2)goto _LL57;if(*((int*)_tmp74)!= 4)goto _LL57;
_tmp80=((struct Cyc_Absyn_Pointer_p_struct*)_tmp74)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA7=0;if(topt != 0){void*_tmpA8=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpA9;void*_tmpAA;_LL6B: if(_tmpA8 <= (void*)4)goto _LL6D;
if(*((int*)_tmpA8)!= 4)goto _LL6D;_tmpA9=((struct Cyc_Absyn_PointerType_struct*)
_tmpA8)->f1;_tmpAA=(void*)_tmpA9.elt_typ;_LL6C: inner_typ=_tmpAA;_tmpA7=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp80,_tmpA7,(void**)& ptr_rgn));if(_tmpA7 == 0){void*
_tmpAB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmpAC;union Cyc_Absyn_TunionFieldInfoU_union
_tmpAD;struct Cyc_Absyn_Tuniondecl*_tmpAE;struct Cyc_Absyn_Tunionfield*_tmpAF;
struct Cyc_List_List*_tmpB0;_LL70: if(_tmpAB <= (void*)4)goto _LL72;if(*((int*)
_tmpAB)!= 3)goto _LL72;_tmpAC=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpAB)->f1;
_tmpAD=_tmpAC.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpAB)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL72;_tmpAE=(_tmpAD.KnownTunionfield).f1;_tmpAF=(_tmpAD.KnownTunionfield).f2;
_tmpB0=_tmpAC.targs;if(!(!_tmpAE->is_flat))goto _LL72;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_TunionType_struct
_tmpB2;_tmpB2.tag=2;_tmpB2.f1=({struct Cyc_Absyn_TunionInfo _tmpB3;_tmpB3.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union _tmpB5;(
_tmpB5.KnownTunion).tag=1;(_tmpB5.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=_tmpAE;_tmpB6;});_tmpB5;});_tmpB3.targs=
_tmpB0;_tmpB3.rgn=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=(
void*)ptr_rgn;_tmpB4;});_tmpB3;});_tmpB2;});_tmpB1;});goto _LL6F;_LL72:;_LL73: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7[0]=({struct Cyc_Absyn_PointerType_struct _tmpB8;_tmpB8.tag=4;_tmpB8.f1=({
struct Cyc_Absyn_PtrInfo _tmpB9;_tmpB9.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp80->topt))->v);_tmpB9.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpB9.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpBA;_tmpBA.rgn=(void*)ptr_rgn;_tmpBA.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBA.bounds=Cyc_Absyn_empty_conref();
_tmpBA.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBA.ptrloc=
0;_tmpBA;});_tmpB9;});_tmpB8;});_tmpB7;});goto _LL6F;_LL6F:;}else{t=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({
struct Cyc_Absyn_PointerType_struct _tmpBC;_tmpBC.tag=4;_tmpBC.f1=({struct Cyc_Absyn_PtrInfo
_tmpBD;_tmpBD.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v);
_tmpBD.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBD.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpBE;_tmpBE.rgn=(void*)ptr_rgn;_tmpBE.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpBE.bounds=Cyc_Absyn_empty_conref();_tmpBE.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBE.ptrloc=0;_tmpBE;});
_tmpBD;});_tmpBC;});_tmpBB;});}goto _LL3C;}}_LL57: if(_tmp74 <= (void*)2)goto _LL59;
if(*((int*)_tmp74)!= 3)goto _LL59;_tmp81=((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f1;
_tmp82=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f1;
_tmp83=((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f2;_LL58: {struct Cyc_List_List*
_tmpBF=*_tmp82;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(
topt != 0){void*_tmpC0=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpC1;
_LL75: if(_tmpC0 <= (void*)4)goto _LL77;if(*((int*)_tmpC0)!= 9)goto _LL77;_tmpC1=((
struct Cyc_Absyn_TupleType_struct*)_tmpC0)->f1;_LL76: topt_ts=_tmpC1;if(_tmp83){
int _tmpC2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBF);int _tmpC3=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1);if(_tmpC2 < _tmpC3){struct
Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC3 - _tmpC2;i ++){wild_ps=({struct Cyc_List_List*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpC4->tl=
wild_ps;_tmpC4;});}}*_tmp82=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpBF,wild_ps);_tmpBF=*_tmp82;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBF)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpC1))({void*_tmpC5[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmpC6="unnecessary ... in tuple pattern";_tag_dynforward(
_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,33));}),_tag_dynforward(_tmpC5,
sizeof(void*),0));});}}goto _LL74;_LL77:;_LL78: goto _LL74;_LL74:;}else{if(_tmp83)({
void*_tmpC7[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpC8="cannot determine missing fields for ... in tuple pattern";
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,57));}),
_tag_dynforward(_tmpC7,sizeof(void*),0));});}for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
void**_tmpC9=0;if(topt_ts != 0){_tmpC9=(void**)&(*((struct _tuple9*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(
struct Cyc_Absyn_Pat*)_tmpBF->hd,_tmpC9,rgn_opt));pat_ts=({struct Cyc_List_List*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->hd=({struct _tuple9*_tmpCB=_cycalloc(
sizeof(*_tmpCB));_tmpCB->f1=Cyc_Absyn_empty_tqual(0);_tmpCB->f2=(void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)_tmpBF->hd)->topt))->v;_tmpCB;});
_tmpCA->tl=pat_ts;_tmpCA;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpCC=
_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({struct Cyc_Absyn_TupleType_struct _tmpCD;
_tmpCD.tag=9;_tmpCD.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmpCD;});_tmpCC;});goto _LL3C;}_LL59: if(_tmp74 <= (void*)2)goto _LL5B;if(*((
int*)_tmp74)!= 5)goto _LL5B;_tmp84=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1;
_tmp85=_tmp84.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL5B;_tmp86=(_tmp85.KnownAggr).f1;_tmp87=*_tmp86;_tmp88=_tmp84.targs;
_tmp89=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).targs;
_tmp8A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f2;_tmp8B=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp74)->f3;_tmp8C=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp74)->f3;_tmp8D=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f4;_LL5A: {struct
Cyc_List_List*_tmpCE=*_tmp8C;if(_tmp87->impl == 0){({void*_tmpCF[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmpD0="can't destructure an abstract struct";_tag_dynforward(
_tmpD0,sizeof(char),_get_zero_arr_size(_tmpD0,37));}),_tag_dynforward(_tmpCF,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL3C;}if(_tmp8A != 0)rgn_opt=0;{struct _RegionHandle _tmpD1=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpD1;_push_region(rgn);{struct Cyc_List_List*_tmpD2=0;
struct Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmpD3=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->exist_vars;{struct Cyc_List_List*
t=_tmp8A;for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmpD3))->hd;_tmpD3=_tmpD3->tl;{void*_tmpD4=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpD5=Cyc_Absyn_compress_kb((void*)uv->kind);int error=0;
void*k2;{void*_tmpD6=_tmpD5;void*_tmpD7;void*_tmpD8;_LL7A: if(*((int*)_tmpD6)!= 2)
goto _LL7C;_tmpD7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD6)->f2;_LL7B:
_tmpD8=_tmpD7;goto _LL7D;_LL7C: if(*((int*)_tmpD6)!= 0)goto _LL7E;_tmpD8=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpD6)->f1;_LL7D: k2=_tmpD8;goto _LL79;_LL7E:;_LL7F:({
void*_tmpD9[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpDA="unconstrained existential type variable in struct";
_tag_dynforward(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,50));}),
_tag_dynforward(_tmpD9,sizeof(void*),0));});_LL79:;}{void*_tmpDB=_tmpD4;void*
_tmpDC;struct Cyc_Core_Opt*_tmpDD;struct Cyc_Core_Opt**_tmpDE;void*_tmpDF;struct
Cyc_Core_Opt*_tmpE0;struct Cyc_Core_Opt**_tmpE1;_LL81: if(*((int*)_tmpDB)!= 0)goto
_LL83;_tmpDC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDB)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(
k2,_tmpDC))error=1;goto _LL80;_LL83: if(*((int*)_tmpDB)!= 2)goto _LL85;_tmpDD=((
struct Cyc_Absyn_Less_kb_struct*)_tmpDB)->f1;_tmpDE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpDB)->f1;_tmpDF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDB)->f2;_LL84: _tmpE1=_tmpDE;goto _LL86;_LL85: if(*((int*)_tmpDB)!= 1)goto _LL80;
_tmpE0=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpDB)->f1;_tmpE1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpDB)->f1;_LL86:*_tmpE1=({struct Cyc_Core_Opt*
_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->v=(void*)_tmpD5;_tmpE2;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(k2));{
struct Cyc_String_pa_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(_tmpD4));{struct Cyc_String_pa_struct
_tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*tv->name);{
void*_tmpE3[3]={& _tmpE5,& _tmpE6,& _tmpE7};Cyc_Tcutil_terr(p->loc,({const char*
_tmpE4="type variable %s has kind %s but must have at least kind %s";
_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,60));}),
_tag_dynforward(_tmpE3,sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_VarType_struct
_tmpEE;_tmpEE.tag=1;_tmpEE.f1=tv;_tmpEE;});_tmpED;});_tmpD2=({struct Cyc_List_List*
_tmpE8=_region_malloc(rgn,sizeof(*_tmpE8));_tmpE8->hd=(void*)((void*)vartype);
_tmpE8->tl=_tmpD2;_tmpE8;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=({struct _tuple0*_tmpEA=_cycalloc(
sizeof(*_tmpEA));_tmpEA->f1=Cyc_Absyn_empty_effect;_tmpEA->f2=vartype;_tmpEA;});
_tmpE9->tl=outlives_constraints;_tmpE9;});else{if(k2 == (void*)4  || k2 == (void*)5)({
void*_tmpEB[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpEC="opened existential had unique or top region kind";
_tag_dynforward(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,49));}),
_tag_dynforward(_tmpEB,sizeof(void*),0));});}}}}}_tmpD2=Cyc_List_imp_rev(_tmpD2);{
struct _RegionHandle _tmpEF=_new_region("r2");struct _RegionHandle*r2=& _tmpEF;
_push_region(r2);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,
_tmp8A);struct Cyc_List_List*_tmpF0=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4
_tmpF1=({struct _tuple4 _tmp111;_tmp111.f1=_tmpF0;_tmp111.f2=rgn;_tmp111;});struct
Cyc_List_List*_tmpF2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpF1,_tmp87->tvs);struct
Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->exist_vars,
_tmpD2);struct Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF2);struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF3);struct Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpF2,_tmpF3);
if(_tmp8A != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->f1=0;_tmpF7->f2=0;_tmpF7;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp8A);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpF8=0;{struct Cyc_List_List*_tmpF9=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->rgn_po;for(0;_tmpF9 != 
0;_tmpF9=_tmpF9->tl){_tmpF8=({struct Cyc_List_List*_tmpFA=_cycalloc(sizeof(*
_tmpFA));_tmpFA->hd=({struct _tuple0*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpF6,(*((struct _tuple0*)_tmpF9->hd)).f1);_tmpFB->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpF6,(*((struct _tuple0*)_tmpF9->hd)).f2);_tmpFB;});
_tmpFA->tl=_tmpF8;_tmpFA;});}}_tmpF8=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpF8);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpF8);}}*_tmp89=_tmpF4;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpFC=
_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_AggrType_struct _tmpFD;
_tmpFD.tag=10;_tmpFD.f1=({struct Cyc_Absyn_AggrInfo _tmpFE;_tmpFE.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpFF;(_tmpFF.KnownAggr).tag=
1;(_tmpFF.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp100=_cycalloc(sizeof(*
_tmp100));_tmp100[0]=_tmp87;_tmp100;});_tmpFF;});_tmpFE.targs=*_tmp89;_tmpFE;});
_tmpFD;});_tmpFC;});if(_tmp8D){int _tmp101=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCE);int _tmp102=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp87->impl))->fields);if(_tmp101 < _tmp102){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp102 - _tmp101;i ++){wild_dps=({struct Cyc_List_List*
_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->hd=({struct _tuple8*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104->f1=0;_tmp104->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp104;});_tmp103->tl=wild_dps;_tmp103;});}}*_tmp8C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCE,
wild_dps);_tmpCE=*_tmp8C;}else{if(_tmp101 == _tmp102)({void*_tmp105[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmp106="unnecessary ... in struct pattern";_tag_dynforward(
_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,34));}),_tag_dynforward(_tmp105,
sizeof(void*),0));});}}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,
_tmpCE,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->fields);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp108;struct Cyc_Absyn_Aggrfield*
_tmp109;struct Cyc_Absyn_Pat*_tmp10A;struct _tuple10*_tmp107=(struct _tuple10*)
fields->hd;_tmp108=*_tmp107;_tmp109=_tmp108.f1;_tmp10A=_tmp108.f2;{void*_tmp10B=
Cyc_Tcutil_rsubstitute(rgn,_tmpF6,(void*)_tmp109->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp10A,(void**)& _tmp10B,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10A->topt))->v,_tmp10B))({struct Cyc_String_pa_struct
_tmp110;_tmp110.tag=0;_tmp110.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp10A->topt))->v));{
struct Cyc_String_pa_struct _tmp10F;_tmp10F.tag=0;_tmp10F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp10B));{struct
Cyc_String_pa_struct _tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp109->name);{void*_tmp10C[3]={& _tmp10E,& _tmp10F,&
_tmp110};Cyc_Tcutil_terr(p->loc,({const char*_tmp10D="field %s of struct pattern expects type %s != %s";
_tag_dynforward(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,49));}),
_tag_dynforward(_tmp10C,sizeof(void*),3));}}}});}}}};_pop_region(r2);}};
_pop_region(rgn);}goto _LL3C;}_LL5B: if(_tmp74 <= (void*)2)goto _LL5D;if(*((int*)
_tmp74)!= 6)goto _LL5D;_tmp8E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f1;
_tmp8F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f2;_tmp90=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp74)->f3;_tmp91=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tunion_p_struct*)
_tmp74)->f3;_tmp92=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f4;_LL5C: {struct
Cyc_List_List*_tmp112=*_tmp91;if(_tmp8E->is_flat)rgn_opt=0;{struct _RegionHandle
_tmp113=_new_region("rgn");struct _RegionHandle*rgn=& _tmp113;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp8F->typs;struct Cyc_List_List*_tmp114=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple4 _tmp115=({struct _tuple4 _tmp13E;_tmp13E.f1=_tmp114;_tmp13E.f2=
rgn;_tmp13E;});struct Cyc_List_List*_tmp116=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp115,_tmp8E->tvs);struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp116);if(tqts == 0  || _tmp8E->is_flat){struct Cyc_Core_Opt*ropt=0;
if(!_tmp8E->is_flat)ropt=({struct Cyc_Core_Opt*_tmp118=_cycalloc(sizeof(*_tmp118));
_tmp118->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->v=_tmp114;
_tmp119;}));_tmp118;});t=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_TunionType_struct
_tmp11B;_tmp11B.tag=2;_tmp11B.f1=({struct Cyc_Absyn_TunionInfo _tmp11C;_tmp11C.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union _tmp11D;(
_tmp11D.KnownTunion).tag=1;(_tmp11D.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=_tmp8E;_tmp11E;});_tmp11D;});
_tmp11C.targs=_tmp117;_tmp11C.rgn=ropt;_tmp11C;});_tmp11B;});_tmp11A;});}else{t=(
void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));
_tmp11F[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp120;_tmp120.tag=3;_tmp120.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp121;_tmp121.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp122;(_tmp122.KnownTunionfield).tag=1;(
_tmp122.KnownTunionfield).f1=_tmp8E;(_tmp122.KnownTunionfield).f2=_tmp8F;_tmp122;});
_tmp121.targs=_tmp117;_tmp121;});_tmp120;});_tmp11F;});}if(topt != 0  && (tqts == 0
 || _tmp8E->is_flat)){void*_tmp123=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp124;struct Cyc_List_List*_tmp125;_LL88: if(_tmp123 <= (void*)4)goto _LL8C;if(*((
int*)_tmp123)!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp127;_tmp127.tag=3;_tmp127.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp128;
_tmp128.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp129;(_tmp129.KnownTunionfield).tag=1;(_tmp129.KnownTunionfield).f1=_tmp8E;(
_tmp129.KnownTunionfield).f2=_tmp8F;_tmp129;});_tmp128.targs=_tmp117;_tmp128;});
_tmp127;});_tmp126;});goto _LL87;_LL8A: if(*((int*)_tmp123)!= 2)goto _LL8C;_tmp124=((
struct Cyc_Absyn_TunionType_struct*)_tmp123)->f1;_tmp125=_tmp124.targs;_LL8B: {
struct Cyc_List_List*_tmp12A=_tmp117;for(0;_tmp12A != 0  && _tmp125 != 0;(_tmp12A=
_tmp12A->tl,_tmp125=_tmp125->tl)){Cyc_Tcutil_unify((void*)_tmp12A->hd,(void*)
_tmp125->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}if(_tmp92){int _tmp12B=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp112);int _tmp12C=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp12B < _tmp12C){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp12C - _tmp12B;i ++){wild_ps=({struct Cyc_List_List*
_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->hd=Cyc_Tcpat_wild_pat(p->loc);
_tmp12D->tl=wild_ps;_tmp12D;});}}*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp112,wild_ps);_tmp112=*_tmp91;}
else{if(_tmp12B == _tmp12C)({struct Cyc_String_pa_struct _tmp130;_tmp130.tag=0;
_tmp130.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8E->name));{void*_tmp12E[1]={& _tmp130};Cyc_Tcutil_warn(p->loc,({const char*
_tmp12F="unnecessary ... in tunion field %s";_tag_dynforward(_tmp12F,sizeof(char),
_get_zero_arr_size(_tmp12F,35));}),_tag_dynforward(_tmp12E,sizeof(void*),1));}});}}
for(0;_tmp112 != 0  && tqts != 0;(_tmp112=_tmp112->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp131=(struct Cyc_Absyn_Pat*)_tmp112->hd;void*_tmp132=Cyc_Tcutil_rsubstitute(
rgn,_tmp116,(*((struct _tuple9*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,
Cyc_Tcpat_tcPatRec(te,_tmp131,(void**)& _tmp132,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp131->topt))->v,_tmp132))({struct Cyc_String_pa_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp131->topt))->v));{
struct Cyc_String_pa_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp132));{struct
Cyc_String_pa_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp133[3]={&
_tmp135,& _tmp136,& _tmp137};Cyc_Tcutil_terr(_tmp131->loc,({const char*_tmp134="%s expects argument type %s, not %s";
_tag_dynforward(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,36));}),
_tag_dynforward(_tmp133,sizeof(void*),3));}}}});}if(_tmp112 != 0)({struct Cyc_String_pa_struct
_tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp138[1]={& _tmp13A};Cyc_Tcutil_terr(
p->loc,({const char*_tmp139="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,45));}),
_tag_dynforward(_tmp138,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp13B[1]={& _tmp13D};Cyc_Tcutil_terr(
p->loc,({const char*_tmp13C="too few arguments for tunion constructor %s";
_tag_dynforward(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,44));}),
_tag_dynforward(_tmp13B,sizeof(void*),1));}});};_pop_region(rgn);}goto _LL3C;}
_LL5D: if(_tmp74 <= (void*)2)goto _LL5F;if(*((int*)_tmp74)!= 5)goto _LL5F;_tmp93=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1;_tmp94=_tmp93.aggr_info;if((((((
struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).aggr_info).UnknownAggr).tag != 0)goto
_LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp74 <= (void*)2)goto _LL61;if(*((int*)_tmp74)!= 
12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp74 <= (void*)2)goto _LL63;if(*((int*)
_tmp74)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp74 <= (void*)2)goto _LL3C;if(*((
int*)_tmp74)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp13F=_cycalloc(
sizeof(*_tmp13F));_tmp13F->v=(void*)t;_tmp13F;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp141=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp142=_new_region("r");struct _RegionHandle*r=& _tmp142;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp141.patvars),p->loc,({const char*
_tmp143="pattern contains a repeated variable";_tag_dynforward(_tmp143,sizeof(
char),_get_zero_arr_size(_tmp143,37));}));;_pop_region(r);}return _tmp141;}void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*
_tmp144=(void*)p->r;struct Cyc_Absyn_Pat*_tmp145;struct Cyc_List_List*_tmp146;
struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp148;_LL8F: if(_tmp144 <= (void*)
2)goto _LL97;if(*((int*)_tmp144)!= 4)goto _LL91;_tmp145=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp144)->f1;_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp145);{void*_tmp149=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp14A;
struct Cyc_Absyn_PtrAtts _tmp14B;void*_tmp14C;struct Cyc_Absyn_TunionInfo _tmp14D;
struct Cyc_Core_Opt*_tmp14E;struct Cyc_Core_Opt _tmp14F;void*_tmp150;_LL9A: if(
_tmp149 <= (void*)4)goto _LL9E;if(*((int*)_tmp149)!= 4)goto _LL9C;_tmp14A=((struct
Cyc_Absyn_PointerType_struct*)_tmp149)->f1;_tmp14B=_tmp14A.ptr_atts;_tmp14C=(
void*)_tmp14B.rgn;_LL9B: _tmp150=_tmp14C;goto _LL9D;_LL9C: if(*((int*)_tmp149)!= 2)
goto _LL9E;_tmp14D=((struct Cyc_Absyn_TunionType_struct*)_tmp149)->f1;_tmp14E=
_tmp14D.rgn;if(_tmp14E == 0)goto _LL9E;_tmp14F=*_tmp14E;_tmp150=(void*)_tmp14F.v;
_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp150);return;_LL9E:;_LL9F:({
void*_tmp151[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp152="check_pat_regions: bad pointer type";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,36));}),
_tag_dynforward(_tmp151,sizeof(void*),0));});_LL99:;}_LL91: if(*((int*)_tmp144)!= 
6)goto _LL93;_tmp146=((struct Cyc_Absyn_Tunion_p_struct*)_tmp144)->f3;_LL92: for(0;
_tmp146 != 0;_tmp146=_tmp146->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp146->hd);}{void*_tmp153=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp154;struct Cyc_Core_Opt*_tmp155;struct Cyc_Absyn_TunionInfo
_tmp156;struct Cyc_Core_Opt*_tmp157;struct Cyc_Core_Opt _tmp158;void*_tmp159;_LLA1:
if(_tmp153 <= (void*)4)goto _LLA7;if(*((int*)_tmp153)!= 2)goto _LLA3;_tmp154=((
struct Cyc_Absyn_TunionType_struct*)_tmp153)->f1;_tmp155=_tmp154.rgn;if(_tmp155 != 
0)goto _LLA3;_LLA2: return;_LLA3: if(*((int*)_tmp153)!= 2)goto _LLA5;_tmp156=((
struct Cyc_Absyn_TunionType_struct*)_tmp153)->f1;_tmp157=_tmp156.rgn;if(_tmp157 == 
0)goto _LLA5;_tmp158=*_tmp157;_tmp159=(void*)_tmp158.v;_LLA4: Cyc_Tcenv_check_rgn_accessible(
te,p->loc,_tmp159);return;_LLA5: if(*((int*)_tmp153)!= 3)goto _LLA7;_LLA6: return;
_LLA7:;_LLA8:({void*_tmp15A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp15B="check_pat_regions: bad tunion type";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,35));}),
_tag_dynforward(_tmp15A,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)_tmp144)!= 
5)goto _LL95;_tmp147=((struct Cyc_Absyn_Aggr_p_struct*)_tmp144)->f3;_LL94: for(0;
_tmp147 != 0;_tmp147=_tmp147->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple8*)_tmp147->hd)).f2);}return;_LL95: if(*((int*)_tmp144)!= 3)goto _LL97;
_tmp148=((struct Cyc_Absyn_Tuple_p_struct*)_tmp144)->f1;_LL96: for(0;_tmp148 != 0;
_tmp148=_tmp148->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp148->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
union Cyc_Tcpat_Name_value_union{struct Cyc_Tcpat_Name_v_struct Name_v;struct Cyc_Tcpat_Int_v_struct
Int_v;};struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value_union name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value_union _tmp15C=c1->name;
struct _dynforward_ptr _tmp15D;int _tmp15E;_LLAA: if((_tmp15C.Name_v).tag != 0)goto
_LLAC;_tmp15D=(_tmp15C.Name_v).f1;_LLAB: {union Cyc_Tcpat_Name_value_union _tmp15F=
c2->name;struct _dynforward_ptr _tmp160;_LLAF: if((_tmp15F.Name_v).tag != 0)goto
_LLB1;_tmp160=(_tmp15F.Name_v).f1;_LLB0: return Cyc_strcmp((struct _dynforward_ptr)
_tmp15D,(struct _dynforward_ptr)_tmp160);_LLB1: if((_tmp15F.Int_v).tag != 1)goto
_LLAE;_LLB2: return - 1;_LLAE:;}_LLAC: if((_tmp15C.Int_v).tag != 1)goto _LLA9;_tmp15E=(
_tmp15C.Int_v).f1;_LLAD: {union Cyc_Tcpat_Name_value_union _tmp161=c2->name;int
_tmp162;_LLB4: if((_tmp161.Name_v).tag != 0)goto _LLB6;_LLB5: return 1;_LLB6: if((
_tmp161.Int_v).tag != 1)goto _LLB3;_tmp162=(_tmp161.Int_v).f1;_LLB7: return _tmp15E - 
_tmp162;_LLB3:;}_LLA9:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static int Cyc_Tcpat_one_opt=1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=
256;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp163=_region_malloc(r,sizeof(*
_tmp163));_tmp163->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp164;(_tmp164.Name_v).tag=0;(_tmp164.Name_v).f1=({const char*_tmp165="NULL";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,5));});_tmp164;});
_tmp163->arity=0;_tmp163->span=(int*)& Cyc_Tcpat_two_opt;_tmp163->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp163;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp166=_region_malloc(r,sizeof(*_tmp166));_tmp166->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp167;(_tmp167.Name_v).tag=0;(_tmp167.Name_v).f1=({
const char*_tmp168="&";_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(
_tmp168,2));});_tmp167;});_tmp166->arity=1;_tmp166->span=(int*)& Cyc_Tcpat_two_opt;
_tmp166->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp166;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp169=_region_malloc(r,sizeof(*_tmp169));_tmp169->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp16A;(_tmp16A.Name_v).tag=0;(_tmp16A.Name_v).f1=({
const char*_tmp16B="&";_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(
_tmp16B,2));});_tmp16A;});_tmp169->arity=1;_tmp169->span=(int*)& Cyc_Tcpat_one_opt;
_tmp169->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp169;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp16C=_region_malloc(r,sizeof(*_tmp16C));_tmp16C->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp16D;(
_tmp16D.Int_v).tag=1;(_tmp16D.Int_v).f1=i;_tmp16D;});_tmp16C->arity=0;_tmp16C->span=
0;_tmp16C->orig_pat=p;_tmp16C;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(
struct _RegionHandle*r,struct _dynforward_ptr f,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp16F;(
_tmp16F.Name_v).tag=0;(_tmp16F.Name_v).f1=f;_tmp16F;});_tmp16E->arity=0;_tmp16E->span=
0;_tmp16E->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16E;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp170=_region_malloc(r,sizeof(*_tmp170));_tmp170->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp171;(
_tmp171.Int_v).tag=1;(_tmp171.Int_v).f1=(int)c;_tmp171;});_tmp170->arity=0;
_tmp170->span=(int*)& Cyc_Tcpat_twofiftysix_opt;_tmp170->orig_pat=(struct Cyc_Absyn_Pat*)
p;_tmp170;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp172=_region_malloc(r,sizeof(*_tmp172));_tmp172->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp173;(_tmp173.Name_v).tag=0;(_tmp173.Name_v).f1=({
const char*_tmp174="$";_tag_dynforward(_tmp174,sizeof(char),_get_zero_arr_size(
_tmp174,2));});_tmp173;});_tmp172->arity=i;_tmp172->span=(int*)& Cyc_Tcpat_one_opt;
_tmp172->orig_pat=p;_tmp172;});}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp175=_region_malloc(r,sizeof(*_tmp175));_tmp175[0]=({struct Cyc_Tcpat_Con_struct
_tmp176;_tmp176.tag=0;_tmp176.f1=Cyc_Tcpat_null_con(r,p);_tmp176.f2=0;_tmp176;});
_tmp175;});}static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp177=_region_malloc(r,sizeof(*
_tmp177));_tmp177[0]=({struct Cyc_Tcpat_Con_struct _tmp178;_tmp178.tag=0;_tmp178.f1=
Cyc_Tcpat_int_con(r,i,p);_tmp178.f2=0;_tmp178;});_tmp177;});}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp179=_region_malloc(r,sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Tcpat_Con_struct
_tmp17A;_tmp17A.tag=0;_tmp17A.f1=Cyc_Tcpat_char_con(r,c,p);_tmp17A.f2=0;_tmp17A;});
_tmp179;});}static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct
_dynforward_ptr f,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp17B=_region_malloc(r,sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_Tcpat_Con_struct
_tmp17C;_tmp17C.tag=0;_tmp17C.f1=Cyc_Tcpat_float_con(r,f,p);_tmp17C.f2=0;_tmp17C;});
_tmp17B;});}static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,
struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17D=
_region_malloc(r,sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Tcpat_Con_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=Cyc_Tcpat_null_ptr_con(r,p0);_tmp17E.f2=({
struct Cyc_List_List*_tmp17F=_region_malloc(r,sizeof(*_tmp17F));_tmp17F->hd=(void*)
p;_tmp17F->tl=0;_tmp17F;});_tmp17E;});_tmp17D;});}static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp180=_region_malloc(r,sizeof(*_tmp180));_tmp180[0]=({struct Cyc_Tcpat_Con_struct
_tmp181;_tmp181.tag=0;_tmp181.f1=Cyc_Tcpat_ptr_con(r,p0);_tmp181.f2=({struct Cyc_List_List*
_tmp182=_region_malloc(r,sizeof(*_tmp182));_tmp182->hd=(void*)p;_tmp182->tl=0;
_tmp182;});_tmp181;});_tmp180;});}static void*Cyc_Tcpat_tuple_pat(struct
_RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp183=_region_malloc(r,sizeof(*_tmp183));_tmp183[0]=({
struct Cyc_Tcpat_Con_struct _tmp184;_tmp184.tag=0;_tmp184.f1=Cyc_Tcpat_tuple_con(r,
Cyc_List_length(ss),p);_tmp184.f2=ss;_tmp184;});_tmp183;});}static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dynforward_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*
_tmp187=_region_malloc(r,sizeof(*_tmp187));_tmp187->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp188;(_tmp188.Name_v).tag=0;(_tmp188.Name_v).f1=
con_name;_tmp188;});_tmp187->arity=Cyc_List_length(ps);_tmp187->span=span;
_tmp187->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp187;});return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp185=_region_malloc(r,sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Tcpat_Con_struct
_tmp186;_tmp186.tag=0;_tmp186.f1=c;_tmp186.f2=ps;_tmp186;});_tmp185;});}static
void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{
void*_tmp189=(void*)p->r;void*_tmp18A;int _tmp18B;char _tmp18C;struct
_dynforward_ptr _tmp18D;struct Cyc_Absyn_Pat*_tmp18E;struct Cyc_Absyn_Pat*_tmp18F;
struct Cyc_Absyn_Pat*_tmp190;struct Cyc_Absyn_Tuniondecl*_tmp191;struct Cyc_Absyn_Tunionfield*
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_AggrInfo
_tmp195;union Cyc_Absyn_AggrInfoU_union _tmp196;struct Cyc_Absyn_Aggrdecl**_tmp197;
struct Cyc_Absyn_Aggrdecl*_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Absyn_Enumdecl*
_tmp19A;struct Cyc_Absyn_Enumfield*_tmp19B;void*_tmp19C;struct Cyc_Absyn_Enumfield*
_tmp19D;_LLB9: if((int)_tmp189 != 0)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp189 <= (
void*)2)goto _LLBD;if(*((int*)_tmp189)!= 2)goto _LLBD;_LLBC: s=(void*)0;goto _LLB8;
_LLBD: if((int)_tmp189 != 1)goto _LLBF;_LLBE: s=Cyc_Tcpat_null_pat(r,p);goto _LLB8;
_LLBF: if(_tmp189 <= (void*)2)goto _LLC1;if(*((int*)_tmp189)!= 7)goto _LLC1;_tmp18A=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp189)->f1;_tmp18B=((struct Cyc_Absyn_Int_p_struct*)
_tmp189)->f2;_LLC0: s=Cyc_Tcpat_int_pat(r,_tmp18B,(struct Cyc_Absyn_Pat*)p);goto
_LLB8;_LLC1: if(_tmp189 <= (void*)2)goto _LLC3;if(*((int*)_tmp189)!= 8)goto _LLC3;
_tmp18C=((struct Cyc_Absyn_Char_p_struct*)_tmp189)->f1;_LLC2: s=Cyc_Tcpat_char_pat(
r,_tmp18C,p);goto _LLB8;_LLC3: if(_tmp189 <= (void*)2)goto _LLC5;if(*((int*)_tmp189)
!= 9)goto _LLC5;_tmp18D=((struct Cyc_Absyn_Float_p_struct*)_tmp189)->f1;_LLC4: s=
Cyc_Tcpat_float_pat(r,_tmp18D,p);goto _LLB8;_LLC5: if(_tmp189 <= (void*)2)goto _LLC7;
if(*((int*)_tmp189)!= 0)goto _LLC7;_tmp18E=((struct Cyc_Absyn_Var_p_struct*)
_tmp189)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(r,_tmp18E);goto _LLB8;_LLC7: if(_tmp189
<= (void*)2)goto _LLC9;if(*((int*)_tmp189)!= 1)goto _LLC9;_tmp18F=((struct Cyc_Absyn_Reference_p_struct*)
_tmp189)->f2;_LLC8: s=Cyc_Tcpat_compile_pat(r,_tmp18F);goto _LLB8;_LLC9: if(_tmp189
<= (void*)2)goto _LLCB;if(*((int*)_tmp189)!= 4)goto _LLCB;_tmp190=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp189)->f1;_LLCA:{void*_tmp19E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp19F;struct Cyc_Absyn_PtrAtts
_tmp1A0;struct Cyc_Absyn_Conref*_tmp1A1;struct Cyc_Absyn_TunionInfo _tmp1A2;struct
Cyc_Core_Opt*_tmp1A3;struct Cyc_Core_Opt _tmp1A4;_LLDE: if(_tmp19E <= (void*)4)goto
_LLE2;if(*((int*)_tmp19E)!= 4)goto _LLE0;_tmp19F=((struct Cyc_Absyn_PointerType_struct*)
_tmp19E)->f1;_tmp1A0=_tmp19F.ptr_atts;_tmp1A1=_tmp1A0.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint_union
_tmp1A5=_tmp1A1->v;struct Cyc_Absyn_Conref*_tmp1A6;int _tmp1A7;_LLE5: if((_tmp1A5.Forward_constr).tag
!= 1)goto _LLE7;_tmp1A6=(_tmp1A5.Forward_constr).f1;_LLE6: _tmp1A1->v=_tmp1A6->v;
continue;_LLE7: if((_tmp1A5.No_constr).tag != 2)goto _LLE9;_LLE8: _tmp1A1->v=(union
Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp1A8;(_tmp1A8.Eq_constr).tag=
0;(_tmp1A8.Eq_constr).f1=(void*)0;_tmp1A8;});is_nullable=0;still_working=0;goto
_LLE4;_LLE9: if((_tmp1A5.Eq_constr).tag != 0)goto _LLE4;_tmp1A7=(_tmp1A5.Eq_constr).f1;
_LLEA: is_nullable=(int)_tmp1A7;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp190);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp19E)!= 2)goto _LLE2;_tmp1A2=((struct Cyc_Absyn_TunionType_struct*)
_tmp19E)->f1;_tmp1A3=_tmp1A2.rgn;if(_tmp1A3 == 0)goto _LLE2;_tmp1A4=*_tmp1A3;_LLE1:{
void*_tmp1A9=(void*)_tmp190->r;struct Cyc_Absyn_Tuniondecl*_tmp1AA;struct Cyc_Absyn_Tunionfield*
_tmp1AB;struct Cyc_List_List*_tmp1AC;_LLEC: if(_tmp1A9 <= (void*)2)goto _LLEE;if(*((
int*)_tmp1A9)!= 6)goto _LLEE;_tmp1AA=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A9)->f1;
_tmp1AB=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A9)->f2;_tmp1AC=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1A9)->f3;_LLED: {int*span;if(_tmp1AA->is_xtunion)span=0;else{span=({int*
_tmp1AD=_region_malloc(r,sizeof(*_tmp1AD));_tmp1AD[0]=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1AA->fields))->v);_tmp1AD;});}s=Cyc_Tcpat_con_pat(r,*(*_tmp1AB->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1AC),p);goto _LLEB;}_LLEE:;
_LLEF:({void*_tmp1AE[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1AF="non-[x]tunion pattern has tunion type";
_tag_dynforward(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,38));}),
_tag_dynforward(_tmp1AE,sizeof(void*),0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({
void*_tmp1B0[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1B1="pointer pattern does not have pointer type";
_tag_dynforward(_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,43));}),
_tag_dynforward(_tmp1B0,sizeof(void*),0));});_LLDD:;}goto _LLB8;_LLCB: if(_tmp189
<= (void*)2)goto _LLCD;if(*((int*)_tmp189)!= 6)goto _LLCD;_tmp191=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp189)->f1;_tmp192=((struct Cyc_Absyn_Tunion_p_struct*)_tmp189)->f2;_tmp193=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp189)->f3;_LLCC: {int*span;{void*_tmp1B2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLF1: if(_tmp1B2 <= (void*)
4)goto _LLF5;if(*((int*)_tmp1B2)!= 2)goto _LLF3;_LLF2: if(_tmp191->is_xtunion)span=
0;else{span=({int*_tmp1B3=_region_malloc(r,sizeof(*_tmp1B3));_tmp1B3[0]=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp191->fields))->v);_tmp1B3;});}goto _LLF0;_LLF3: if(*((int*)_tmp1B2)
!= 3)goto _LLF5;_LLF4: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF0;_LLF5:;_LLF6: span=({
void*_tmp1B4[0]={};((int*(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1B5="void tunion pattern has bad type";
_tag_dynforward(_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,33));}),
_tag_dynforward(_tmp1B4,sizeof(void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(
r,*(*_tmp192->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp193),p);goto _LLB8;}_LLCD: if(
_tmp189 <= (void*)2)goto _LLCF;if(*((int*)_tmp189)!= 3)goto _LLCF;_tmp194=((struct
Cyc_Absyn_Tuple_p_struct*)_tmp189)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp194),(struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLCF: if(_tmp189 <= (void*)2)goto
_LLD1;if(*((int*)_tmp189)!= 5)goto _LLD1;_tmp195=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp189)->f1;_tmp196=_tmp195.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)
_tmp189)->f1).aggr_info).KnownAggr).tag != 1)goto _LLD1;_tmp197=(_tmp196.KnownAggr).f1;
_tmp198=*_tmp197;_tmp199=((struct Cyc_Absyn_Aggr_p_struct*)_tmp189)->f3;_LLD0: {
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp198->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=
Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({
const char*_tmp1C4="";_tag_dynforward(_tmp1C4,sizeof(char),_get_zero_arr_size(
_tmp1C4,1));}))== 0;{struct Cyc_List_List*dlps0=_tmp199;for(0;!found  && dlps0 != 0;
dlps0=dlps0->tl){struct _tuple8 _tmp1B7;struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_Pat*
_tmp1B9;struct _tuple8*_tmp1B6=(struct _tuple8*)dlps0->hd;_tmp1B7=*_tmp1B6;_tmp1B8=
_tmp1B7.f1;_tmp1B9=_tmp1B7.f2;{struct Cyc_List_List*_tmp1BA=_tmp1B8;struct Cyc_List_List
_tmp1BB;void*_tmp1BC;struct _dynforward_ptr*_tmp1BD;struct Cyc_List_List*_tmp1BE;
_LLF8: if(_tmp1BA == 0)goto _LLFA;_tmp1BB=*_tmp1BA;_tmp1BC=(void*)_tmp1BB.hd;if(*((
int*)_tmp1BC)!= 1)goto _LLFA;_tmp1BD=((struct Cyc_Absyn_FieldName_struct*)_tmp1BC)->f1;
_tmp1BE=_tmp1BB.tl;if(_tmp1BE != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1BD,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1BF=_region_malloc(r,sizeof(*_tmp1BF));_tmp1BF->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1B9);_tmp1BF->tl=ps;_tmp1BF;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp1C0[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C1="bad designator(s)";_tag_dynforward(_tmp1C1,sizeof(char),
_get_zero_arr_size(_tmp1C1,18));}),_tag_dynforward(_tmp1C0,sizeof(void*),0));});
_LLF7:;}}}if(!found)({void*_tmp1C2[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C3="bad designator";
_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,15));}),
_tag_dynforward(_tmp1C2,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(r,ps,(
struct Cyc_Absyn_Pat*)p);goto _LLB8;}_LLD1: if(_tmp189 <= (void*)2)goto _LLD3;if(*((
int*)_tmp189)!= 10)goto _LLD3;_tmp19A=((struct Cyc_Absyn_Enum_p_struct*)_tmp189)->f1;
_tmp19B=((struct Cyc_Absyn_Enum_p_struct*)_tmp189)->f2;_LLD2: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp19A->fields))->v);s=Cyc_Tcpat_con_pat(r,*(*_tmp19B->name).f2,({
int*_tmp1C5=_region_malloc(r,sizeof(*_tmp1C5));_tmp1C5[0]=span;_tmp1C5;}),0,p);
goto _LLB8;}_LLD3: if(_tmp189 <= (void*)2)goto _LLD5;if(*((int*)_tmp189)!= 11)goto
_LLD5;_tmp19C=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp189)->f1;_tmp19D=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp189)->f2;_LLD4: {struct Cyc_List_List*
fields;{void*_tmp1C6=Cyc_Tcutil_compress(_tmp19C);struct Cyc_List_List*_tmp1C7;
_LLFD: if(_tmp1C6 <= (void*)4)goto _LLFF;if(*((int*)_tmp1C6)!= 13)goto _LLFF;_tmp1C7=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp1C6)->f1;_LLFE: fields=_tmp1C7;goto _LLFC;
_LLFF:;_LL100:({void*_tmp1C8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C9="bad type in AnonEnum_p";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,23));}),
_tag_dynforward(_tmp1C8,sizeof(void*),0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(r,*(*_tmp19D->name).f2,({int*
_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));_tmp1CA[0]=span;_tmp1CA;}),0,p);goto
_LLB8;}}_LLD5: if(_tmp189 <= (void*)2)goto _LLD7;if(*((int*)_tmp189)!= 12)goto _LLD7;
_LLD6: goto _LLD8;_LLD7: if(_tmp189 <= (void*)2)goto _LLD9;if(*((int*)_tmp189)!= 13)
goto _LLD9;_LLD8: goto _LLDA;_LLD9: if(_tmp189 <= (void*)2)goto _LLDB;if(*((int*)
_tmp189)!= 5)goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp189 <= (void*)2)goto _LLB8;if(*((
int*)_tmp189)!= 14)goto _LLB8;_LLDC: s=(void*)0;_LLB8:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1CB=td;struct Cyc_Set_Set*_tmp1CC;_LL102: if(*((int*)_tmp1CB)!= 1)goto
_LL104;_tmp1CC=((struct Cyc_Tcpat_Neg_struct*)_tmp1CB)->f1;_LL103: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1CC,c))({void*
_tmp1CD[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1CE="add_neg called when constructor already in set";
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,47));}),
_tag_dynforward(_tmp1CD,sizeof(void*),0));});if(c->span != 0  && ((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1CC)+ 1 >= *((int*)_check_null(c->span)))({
void*_tmp1CF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1D0="add_neg called when |cs U {c}| >= span(c)";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,42));}),
_tag_dynforward(_tmp1CF,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1D1=_region_malloc(r,sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1CC,c);
_tmp1D2;});_tmp1D1;});_LL104: if(*((int*)_tmp1CB)!= 0)goto _LL101;_LL105:({void*
_tmp1D3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D4="add_neg called when td is Positive";_tag_dynforward(_tmp1D4,
sizeof(char),_get_zero_arr_size(_tmp1D4,35));}),_tag_dynforward(_tmp1D3,sizeof(
void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,
void*td){void*_tmp1D5=td;struct Cyc_Tcpat_Con_s*_tmp1D6;struct Cyc_Set_Set*_tmp1D7;
_LL107: if(*((int*)_tmp1D5)!= 0)goto _LL109;_tmp1D6=((struct Cyc_Tcpat_Pos_struct*)
_tmp1D5)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1D6)== 0)return(void*)0;else{
return(void*)1;}_LL109: if(*((int*)_tmp1D5)!= 1)goto _LL106;_tmp1D7=((struct Cyc_Tcpat_Neg_struct*)
_tmp1D5)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1D7,c))return(void*)1;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1D7)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1D8=ctxt;struct Cyc_List_List _tmp1D9;struct _tuple12*_tmp1DA;struct _tuple12
_tmp1DB;struct Cyc_Tcpat_Con_s*_tmp1DC;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*
_tmp1DE;_LL10C: if(_tmp1D8 != 0)goto _LL10E;_LL10D: return 0;_LL10E: if(_tmp1D8 == 0)
goto _LL10B;_tmp1D9=*_tmp1D8;_tmp1DA=(struct _tuple12*)_tmp1D9.hd;_tmp1DB=*_tmp1DA;
_tmp1DC=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;_tmp1DE=_tmp1D9.tl;_LL10F: return({struct
Cyc_List_List*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF->hd=({struct
_tuple12*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->f1=_tmp1DC;_tmp1E0->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1E1=_region_malloc(r,sizeof(*
_tmp1E1));_tmp1E1->hd=(void*)dsc;_tmp1E1->tl=_tmp1DD;_tmp1E1;});_tmp1E0;});
_tmp1DF->tl=_tmp1DE;_tmp1DF;});_LL10B:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1E2=ctxt;
struct Cyc_List_List _tmp1E3;struct _tuple12*_tmp1E4;struct _tuple12 _tmp1E5;struct
Cyc_Tcpat_Con_s*_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;
_LL111: if(_tmp1E2 != 0)goto _LL113;_LL112:({void*_tmp1E9[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1EA="norm_context: empty context";_tag_dynforward(_tmp1EA,sizeof(char),
_get_zero_arr_size(_tmp1EA,28));}),_tag_dynforward(_tmp1E9,sizeof(void*),0));});
_LL113: if(_tmp1E2 == 0)goto _LL110;_tmp1E3=*_tmp1E2;_tmp1E4=(struct _tuple12*)
_tmp1E3.hd;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;_tmp1E8=
_tmp1E3.tl;_LL114: return Cyc_Tcpat_augment(r,_tmp1E8,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=_tmp1E6;_tmp1EC.f2=Cyc_List_rrev(r,_tmp1E7);
_tmp1EC;});_tmp1EB;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1EE=({struct _tuple6 _tmp1ED;_tmp1ED.f1=ctxt;_tmp1ED.f2=work;
_tmp1ED;});struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*
_tmp1F1;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List
_tmp1F4;struct _tuple12*_tmp1F5;struct _tuple12 _tmp1F6;struct Cyc_Tcpat_Con_s*
_tmp1F7;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*
_tmp1FA;struct Cyc_List_List _tmp1FB;struct _tuple11*_tmp1FC;struct _tuple11 _tmp1FD;
struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FF;_LL116: _tmp1EF=_tmp1EE.f1;
if(_tmp1EF != 0)goto _LL118;_tmp1F0=_tmp1EE.f2;if(_tmp1F0 != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1F1=_tmp1EE.f1;if(_tmp1F1 != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1F2=_tmp1EE.f2;if(_tmp1F2 != 0)goto _LL11C;_LL11B:({void*_tmp200[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp201="build_desc: ctxt and work don't match";_tag_dynforward(
_tmp201,sizeof(char),_get_zero_arr_size(_tmp201,38));}),_tag_dynforward(_tmp200,
sizeof(void*),0));});_LL11C: _tmp1F3=_tmp1EE.f1;if(_tmp1F3 == 0)goto _LL115;_tmp1F4=*
_tmp1F3;_tmp1F5=(struct _tuple12*)_tmp1F4.hd;_tmp1F6=*_tmp1F5;_tmp1F7=_tmp1F6.f1;
_tmp1F8=_tmp1F6.f2;_tmp1F9=_tmp1F4.tl;_tmp1FA=_tmp1EE.f2;if(_tmp1FA == 0)goto
_LL115;_tmp1FB=*_tmp1FA;_tmp1FC=(struct _tuple11*)_tmp1FB.hd;_tmp1FD=*_tmp1FC;
_tmp1FE=_tmp1FD.f3;_tmp1FF=_tmp1FB.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*
_tmp202=({struct Cyc_Tcpat_Pos_struct*_tmp203=_region_malloc(r,sizeof(*_tmp203));
_tmp203[0]=({struct Cyc_Tcpat_Pos_struct _tmp204;_tmp204.tag=0;_tmp204.f1=_tmp1F7;
_tmp204.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp1F8),({struct Cyc_List_List*
_tmp205=_region_malloc(r,sizeof(*_tmp205));_tmp205->hd=(void*)dsc;_tmp205->tl=
_tmp1FE;_tmp205;}));_tmp204;});_tmp203;});return Cyc_Tcpat_build_desc(r,_tmp1F9,(
void*)_tmp202,_tmp1FF);}_LL115:;}static void*Cyc_Tcpat_match(struct _RegionHandle*
r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(
struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*
_tmp206=allmrules;struct Cyc_List_List _tmp207;struct _tuple0*_tmp208;struct _tuple0
_tmp209;void*_tmp20A;void*_tmp20B;struct Cyc_List_List*_tmp20C;_LL11F: if(_tmp206
!= 0)goto _LL121;_LL120: return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp20D=
_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Tcpat_Failure_struct _tmp20E;
_tmp20E.tag=0;_tmp20E.f1=(void*)dsc;_tmp20E;});_tmp20D;});_LL121: if(_tmp206 == 0)
goto _LL11E;_tmp207=*_tmp206;_tmp208=(struct _tuple0*)_tmp207.hd;_tmp209=*_tmp208;
_tmp20A=_tmp209.f1;_tmp20B=_tmp209.f2;_tmp20C=_tmp207.tl;_LL122: return Cyc_Tcpat_match(
r,_tmp20A,0,dsc,0,0,_tmp20B,_tmp20C);_LL11E:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));
_tmp20F[0]=({struct Cyc_Tcpat_Neg_struct _tmp210;_tmp210.tag=1;_tmp210.f1=Cyc_Tcpat_empty_con_set(
r);_tmp210;});_tmp20F;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp211=work;
struct Cyc_List_List _tmp212;struct _tuple11*_tmp213;struct _tuple11 _tmp214;struct
Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp217;
struct Cyc_List_List*_tmp218;struct Cyc_List_List _tmp219;struct _tuple11*_tmp21A;
struct _tuple11 _tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21D;
struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21F;_LL124: if(_tmp211 != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp220=
_region_malloc(r,sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Tcpat_Success_struct
_tmp221;_tmp221.tag=1;_tmp221.f1=(void*)right_hand_side;_tmp221;});_tmp220;});
_LL126: if(_tmp211 == 0)goto _LL128;_tmp212=*_tmp211;_tmp213=(struct _tuple11*)
_tmp212.hd;_tmp214=*_tmp213;_tmp215=_tmp214.f1;if(_tmp215 != 0)goto _LL128;_tmp216=
_tmp214.f2;if(_tmp216 != 0)goto _LL128;_tmp217=_tmp214.f3;if(_tmp217 != 0)goto
_LL128;_tmp218=_tmp212.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp218,right_hand_side,rules);_LL128: if(_tmp211 == 0)goto _LL123;_tmp219=*
_tmp211;_tmp21A=(struct _tuple11*)_tmp219.hd;_tmp21B=*_tmp21A;_tmp21C=_tmp21B.f1;
_tmp21D=_tmp21B.f2;_tmp21E=_tmp21B.f3;_tmp21F=_tmp219.tl;_LL129: if((_tmp21C == 0
 || _tmp21D == 0) || _tmp21E == 0)({void*_tmp222[0]={};Cyc_Tcutil_impos(({const
char*_tmp223="tcpat:and_match: malformed work frame";_tag_dynforward(_tmp223,
sizeof(char),_get_zero_arr_size(_tmp223,38));}),_tag_dynforward(_tmp222,sizeof(
void*),0));});{struct Cyc_List_List _tmp225;void*_tmp226;struct Cyc_List_List*
_tmp227;struct Cyc_List_List*_tmp224=(struct Cyc_List_List*)_tmp21C;_tmp225=*
_tmp224;_tmp226=(void*)_tmp225.hd;_tmp227=_tmp225.tl;{struct Cyc_List_List _tmp229;
struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*
_tmp228=(struct Cyc_List_List*)_tmp21D;_tmp229=*_tmp228;_tmp22A=(struct Cyc_List_List*)
_tmp229.hd;_tmp22B=_tmp229.tl;{struct Cyc_List_List _tmp22D;void*_tmp22E;struct Cyc_List_List*
_tmp22F;struct Cyc_List_List*_tmp22C=(struct Cyc_List_List*)_tmp21E;_tmp22D=*
_tmp22C;_tmp22E=(void*)_tmp22D.hd;_tmp22F=_tmp22D.tl;{struct _tuple11*_tmp230=({
struct _tuple11*_tmp232=_region_malloc(r,sizeof(*_tmp232));_tmp232->f1=_tmp227;
_tmp232->f2=_tmp22B;_tmp232->f3=_tmp22F;_tmp232;});return Cyc_Tcpat_match(r,
_tmp226,_tmp22A,_tmp22E,ctx,({struct Cyc_List_List*_tmp231=_region_malloc(r,
sizeof(*_tmp231));_tmp231->hd=_tmp230;_tmp231->tl=_tmp21F;_tmp231;}),
right_hand_side,rules);}}}}_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp233=dsc;
struct Cyc_Set_Set*_tmp234;struct Cyc_List_List*_tmp235;_LL12B: if(*((int*)_tmp233)
!= 1)goto _LL12D;_tmp234=((struct Cyc_Tcpat_Neg_struct*)_tmp233)->f1;_LL12C: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp238=_region_malloc(r,sizeof(*_tmp238));
_tmp238[0]=({struct Cyc_Tcpat_Neg_struct _tmp239;_tmp239.tag=1;_tmp239.f1=Cyc_Tcpat_empty_con_set(
r);_tmp239;});_tmp238;});struct Cyc_List_List*_tmp236=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp236=({struct Cyc_List_List*_tmp237=_region_malloc(r,sizeof(*_tmp237));
_tmp237->hd=(void*)any;_tmp237->tl=_tmp236;_tmp237;});}}return _tmp236;}_LL12D:
if(*((int*)_tmp233)!= 0)goto _LL12A;_tmp235=((struct Cyc_Tcpat_Pos_struct*)_tmp233)->f2;
_LL12E: return _tmp235;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp23B;struct _RegionHandle*_tmp23C;struct Cyc_List_List*_tmp23D;struct
_tuple13*_tmp23A=env;_tmp23B=*_tmp23A;_tmp23C=_tmp23B.f1;_tmp23D=_tmp23B.f2;
return({struct Cyc_List_List*_tmp23E=_region_malloc(_tmp23C,sizeof(*_tmp23E));
_tmp23E->hd=(void*)(i + 1);_tmp23E->tl=_tmp23D;_tmp23E;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp23F=({struct _tuple13 _tmp240;_tmp240.f1=r;_tmp240.f2=obj;
_tmp240;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp23F);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp241=p;struct Cyc_Tcpat_Con_s*
_tmp242;struct Cyc_List_List*_tmp243;_LL130: if((int)_tmp241 != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp241 <= (void*)1)goto _LL12F;if(*((int*)_tmp241)!= 0)goto
_LL12F;_tmp242=((struct Cyc_Tcpat_Con_struct*)_tmp241)->f1;_tmp243=((struct Cyc_Tcpat_Con_struct*)
_tmp241)->f2;_LL133: {void*_tmp244=Cyc_Tcpat_static_match(_tmp242,dsc);_LL135:
if((int)_tmp244 != 0)goto _LL137;_LL136: {struct Cyc_List_List*ctx2=({struct Cyc_List_List*
_tmp247=_region_malloc(r,sizeof(*_tmp247));_tmp247->hd=({struct _tuple12*_tmp248=
_region_malloc(r,sizeof(*_tmp248));_tmp248->f1=_tmp242;_tmp248->f2=0;_tmp248;});
_tmp247->tl=ctx;_tmp247;});struct _tuple11*work_frame=({struct _tuple11*_tmp246=
_region_malloc(r,sizeof(*_tmp246));_tmp246->f1=_tmp243;_tmp246->f2=Cyc_Tcpat_getoargs(
r,_tmp242,obj);_tmp246->f3=Cyc_Tcpat_getdargs(r,_tmp242,dsc);_tmp246;});struct
Cyc_List_List*work2=({struct Cyc_List_List*_tmp245=_region_malloc(r,sizeof(*
_tmp245));_tmp245->hd=work_frame;_tmp245->tl=work;_tmp245;});return Cyc_Tcpat_and_match(
r,ctx2,work2,right_hand_side,rules);}_LL137: if((int)_tmp244 != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp244 != 2)goto _LL134;_LL13A: {struct Cyc_List_List*ctx2=({struct
Cyc_List_List*_tmp24F=_region_malloc(r,sizeof(*_tmp24F));_tmp24F->hd=({struct
_tuple12*_tmp250=_region_malloc(r,sizeof(*_tmp250));_tmp250->f1=_tmp242;_tmp250->f2=
0;_tmp250;});_tmp24F->tl=ctx;_tmp24F;});struct _tuple11*work_frame=({struct
_tuple11*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E->f1=_tmp243;_tmp24E->f2=
Cyc_Tcpat_getoargs(r,_tmp242,obj);_tmp24E->f3=Cyc_Tcpat_getdargs(r,_tmp242,dsc);
_tmp24E;});struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp24D=
_region_malloc(r,sizeof(*_tmp24D));_tmp24D->hd=work_frame;_tmp24D->tl=work;
_tmp24D;});void*_tmp249=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp24A=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,
dsc,_tmp242),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp24B=
_region_malloc(r,sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp24C;_tmp24C.tag=2;_tmp24C.f1=obj;_tmp24C.f2=_tmp242;_tmp24C.f3=(void*)
_tmp249;_tmp24C.f4=(void*)_tmp24A;_tmp24C;});_tmp24B;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,
void*),void*env2,int*exhaust_done){void*_tmp251=d;void*_tmp252;void*_tmp253;void*
_tmp254;void*_tmp255;_LL13C: if(*((int*)_tmp251)!= 0)goto _LL13E;_tmp252=(void*)((
struct Cyc_Tcpat_Failure_struct*)_tmp251)->f1;_LL13D: if(!(*exhaust_done)){
not_exhaust(r,env1,_tmp252);*exhaust_done=1;}goto _LL13B;_LL13E: if(*((int*)
_tmp251)!= 1)goto _LL140;_tmp253=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp251)->f1;
_LL13F: rhs_appears(env2,_tmp253);goto _LL13B;_LL140: if(*((int*)_tmp251)!= 2)goto
_LL13B;_tmp254=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp251)->f3;_tmp255=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp251)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp254,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp255,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp257;struct _RegionHandle*_tmp258;int*_tmp259;struct
_tuple16*_tmp256=env;_tmp257=*_tmp256;_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp258,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp25F=_region_malloc(_tmp258,sizeof(*_tmp25F));_tmp25F->f1=0;_tmp25F->f2=(
swc->pattern)->loc;_tmp25F;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(
_tmp258,({struct Cyc_List_List*_tmp25A=_region_malloc(_tmp258,sizeof(*_tmp25A));
_tmp25A->hd=(void*)sp0;_tmp25A->tl=({struct Cyc_List_List*_tmp25B=_region_malloc(
_tmp258,sizeof(*_tmp25B));_tmp25B->hd=(void*)Cyc_Tcpat_int_pat(_tmp258,*_tmp259,
0);_tmp25B->tl=0;_tmp25B;});_tmp25A;}),0);*_tmp259=*_tmp259 + 1;}else{sp=Cyc_Tcpat_tuple_pat(
_tmp258,({struct Cyc_List_List*_tmp25C=_region_malloc(_tmp258,sizeof(*_tmp25C));
_tmp25C->hd=(void*)sp0;_tmp25C->tl=({struct Cyc_List_List*_tmp25D=_region_malloc(
_tmp258,sizeof(*_tmp25D));_tmp25D->hd=(void*)((void*)0);_tmp25D->tl=0;_tmp25D;});
_tmp25C;}),0);}return({struct _tuple15*_tmp25E=_region_malloc(_tmp258,sizeof(*
_tmp25E));_tmp25E->f1=sp;_tmp25E->f2=rhs;_tmp25E;});}}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dynforward_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dynforward_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp260=d;struct Cyc_Tcpat_Con_s*_tmp261;
struct Cyc_List_List*_tmp262;struct Cyc_Set_Set*_tmp263;_LL143: if(*((int*)_tmp260)
!= 0)goto _LL145;_tmp261=((struct Cyc_Tcpat_Pos_struct*)_tmp260)->f1;_tmp262=((
struct Cyc_Tcpat_Pos_struct*)_tmp260)->f2;_LL144: {union Cyc_Tcpat_Name_value_union
_tmp264=_tmp261->name;struct Cyc_Absyn_Pat*_tmp265=_tmp261->orig_pat;if(_tmp265 == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp262))->hd);{void*_tmp266=(void*)_tmp265->r;struct Cyc_Absyn_Vardecl*_tmp267;
struct Cyc_Absyn_Vardecl*_tmp268;struct Cyc_Absyn_Tvar*_tmp269;struct Cyc_Absyn_Vardecl*
_tmp26A;struct Cyc_Absyn_Pat*_tmp26B;struct Cyc_Absyn_Pat _tmp26C;void*_tmp26D;
struct Cyc_Absyn_Tunionfield*_tmp26E;struct Cyc_Absyn_AggrInfo _tmp26F;union Cyc_Absyn_AggrInfoU_union
_tmp270;struct Cyc_Absyn_Aggrdecl**_tmp271;struct Cyc_Absyn_Aggrdecl*_tmp272;
struct Cyc_Absyn_Tunionfield*_tmp273;int _tmp274;char _tmp275;struct _dynforward_ptr
_tmp276;struct Cyc_Absyn_Enumfield*_tmp277;struct Cyc_Absyn_Enumfield*_tmp278;
struct Cyc_Absyn_Exp*_tmp279;_LL148: if((int)_tmp266 != 0)goto _LL14A;_LL149: return({
const char*_tmp27A="_";_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(
_tmp27A,2));});_LL14A: if(_tmp266 <= (void*)2)goto _LL15A;if(*((int*)_tmp266)!= 0)
goto _LL14C;_tmp267=((struct Cyc_Absyn_Var_p_struct*)_tmp266)->f1;_LL14B: return Cyc_Absynpp_qvar2string(
_tmp267->name);_LL14C: if(*((int*)_tmp266)!= 1)goto _LL14E;_tmp268=((struct Cyc_Absyn_Reference_p_struct*)
_tmp266)->f1;_LL14D: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp27D;_tmp27D.tag=0;_tmp27D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp268->name));{void*_tmp27B[1]={& _tmp27D};Cyc_aprintf(({
const char*_tmp27C="*%s";_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(
_tmp27C,4));}),_tag_dynforward(_tmp27B,sizeof(void*),1));}});_LL14E: if(*((int*)
_tmp266)!= 2)goto _LL150;_tmp269=((struct Cyc_Absyn_TagInt_p_struct*)_tmp266)->f1;
_tmp26A=((struct Cyc_Absyn_TagInt_p_struct*)_tmp266)->f2;_LL14F: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp281;_tmp281.tag=0;_tmp281.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp269->name);{struct Cyc_String_pa_struct
_tmp280;_tmp280.tag=0;_tmp280.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26A->name));{void*_tmp27E[2]={& _tmp280,& _tmp281};Cyc_aprintf(({
const char*_tmp27F="%s<`%s>";_tag_dynforward(_tmp27F,sizeof(char),
_get_zero_arr_size(_tmp27F,8));}),_tag_dynforward(_tmp27E,sizeof(void*),2));}}});
_LL150: if(*((int*)_tmp266)!= 3)goto _LL152;_LL151: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(r,_tmp262));{void*
_tmp282[1]={& _tmp284};Cyc_aprintf(({const char*_tmp283="$(%s)";_tag_dynforward(
_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,6));}),_tag_dynforward(_tmp282,
sizeof(void*),1));}});_LL152: if(*((int*)_tmp266)!= 4)goto _LL154;_tmp26B=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp266)->f1;_tmp26C=*_tmp26B;_tmp26D=(void*)_tmp26C.r;
if(_tmp26D <= (void*)2)goto _LL154;if(*((int*)_tmp26D)!= 6)goto _LL154;_tmp26E=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp26D)->f2;_LL153: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp288;_tmp288.tag=0;_tmp288.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(r,_tmp262));{
struct Cyc_String_pa_struct _tmp287;_tmp287.tag=0;_tmp287.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp26E->name));{
void*_tmp285[2]={& _tmp287,& _tmp288};Cyc_aprintf(({const char*_tmp286="&%s(%s)";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,8));}),
_tag_dynforward(_tmp285,sizeof(void*),2));}}});_LL154: if(*((int*)_tmp266)!= 4)
goto _LL156;_LL155: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp28B;_tmp28B.tag=0;_tmp28B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcpat_descs2string(r,_tmp262));{void*_tmp289[1]={& _tmp28B};Cyc_aprintf(({
const char*_tmp28A="&%s";_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size(
_tmp28A,4));}),_tag_dynforward(_tmp289,sizeof(void*),1));}});_LL156: if(*((int*)
_tmp266)!= 5)goto _LL158;_tmp26F=((struct Cyc_Absyn_Aggr_p_struct*)_tmp266)->f1;
_tmp270=_tmp26F.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp266)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL158;_tmp271=(_tmp270.KnownAggr).f1;_tmp272=*_tmp271;_LL157: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(r,_tmp262));{
struct Cyc_String_pa_struct _tmp28E;_tmp28E.tag=0;_tmp28E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp272->name));{
void*_tmp28C[2]={& _tmp28E,& _tmp28F};Cyc_aprintf(({const char*_tmp28D="%s{%s}";
_tag_dynforward(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,7));}),
_tag_dynforward(_tmp28C,sizeof(void*),2));}}});_LL158: if(*((int*)_tmp266)!= 6)
goto _LL15A;_tmp273=((struct Cyc_Absyn_Tunion_p_struct*)_tmp266)->f2;_LL159: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp292;_tmp292.tag=0;_tmp292.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp273->name));{
void*_tmp290[1]={& _tmp292};Cyc_aprintf(({const char*_tmp291="%s";_tag_dynforward(
_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,3));}),_tag_dynforward(_tmp290,
sizeof(void*),1));}});_LL15A: if((int)_tmp266 != 1)goto _LL15C;_LL15B: return({const
char*_tmp293="NULL";_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size(
_tmp293,5));});_LL15C: if(_tmp266 <= (void*)2)goto _LL168;if(*((int*)_tmp266)!= 7)
goto _LL15E;_tmp274=((struct Cyc_Absyn_Int_p_struct*)_tmp266)->f2;_LL15D: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp296;_tmp296.tag=1;_tmp296.f1=(
unsigned long)_tmp274;{void*_tmp294[1]={& _tmp296};Cyc_aprintf(({const char*
_tmp295="%d";_tag_dynforward(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,3));}),
_tag_dynforward(_tmp294,sizeof(void*),1));}});_LL15E: if(*((int*)_tmp266)!= 8)
goto _LL160;_tmp275=((struct Cyc_Absyn_Char_p_struct*)_tmp266)->f1;_LL15F: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp299;_tmp299.tag=1;_tmp299.f1=(
unsigned long)((int)_tmp275);{void*_tmp297[1]={& _tmp299};Cyc_aprintf(({const char*
_tmp298="%d";_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,3));}),
_tag_dynforward(_tmp297,sizeof(void*),1));}});_LL160: if(*((int*)_tmp266)!= 9)
goto _LL162;_tmp276=((struct Cyc_Absyn_Float_p_struct*)_tmp266)->f1;_LL161: return
_tmp276;_LL162: if(*((int*)_tmp266)!= 10)goto _LL164;_tmp277=((struct Cyc_Absyn_Enum_p_struct*)
_tmp266)->f2;_LL163: _tmp278=_tmp277;goto _LL165;_LL164: if(*((int*)_tmp266)!= 11)
goto _LL166;_tmp278=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp266)->f2;_LL165:
return Cyc_Absynpp_qvar2string(_tmp278->name);_LL166: if(*((int*)_tmp266)!= 14)
goto _LL168;_tmp279=((struct Cyc_Absyn_Exp_p_struct*)_tmp266)->f1;_LL167: return Cyc_Absynpp_exp2string(
_tmp279);_LL168:;_LL169:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL147:;}}
_LL145: if(*((int*)_tmp260)!= 1)goto _LL142;_tmp263=((struct Cyc_Tcpat_Neg_struct*)
_tmp260)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp263))
return({const char*_tmp29A="_";_tag_dynforward(_tmp29A,sizeof(char),
_get_zero_arr_size(_tmp29A,2));});{struct Cyc_Tcpat_Con_s*_tmp29B=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp263);struct Cyc_Absyn_Pat*_tmp29C=
_tmp29B->orig_pat;if(_tmp29C == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp29D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp29C->topt))->v);
void*_tmp29E;struct Cyc_Absyn_PtrInfo _tmp29F;struct Cyc_Absyn_PtrAtts _tmp2A0;
struct Cyc_Absyn_TunionInfo _tmp2A1;union Cyc_Absyn_TunionInfoU_union _tmp2A2;struct
Cyc_Absyn_Tuniondecl**_tmp2A3;struct Cyc_Absyn_Tuniondecl*_tmp2A4;_LL16B: if(
_tmp29D <= (void*)4)goto _LL173;if(*((int*)_tmp29D)!= 5)goto _LL16D;_tmp29E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp29D)->f2;if((int)_tmp29E != 0)goto _LL16D;
_LL16C:{int i=0;for(0;i < 256;i ++){struct Cyc_Tcpat_Con_s*_tmp2A5=Cyc_Tcpat_char_con(
r,(char)i,(struct Cyc_Absyn_Pat*)_tmp29C);if(!((int(*)(struct Cyc_Set_Set*s,struct
Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp263,_tmp2A5))return(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2A8;_tmp2A8.tag=1;_tmp2A8.f1=(
unsigned long)i;{void*_tmp2A6[1]={& _tmp2A8};Cyc_aprintf(({const char*_tmp2A7="%d";
_tag_dynforward(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,3));}),
_tag_dynforward(_tmp2A6,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16D: if(*((int*)_tmp29D)!= 5)goto _LL16F;_LL16E:{unsigned int i=0;for(0;i < 0 - 1;
i ++){struct Cyc_Tcpat_Con_s*_tmp2A9=Cyc_Tcpat_int_con(r,(int)i,_tmp29C);if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp263,_tmp2A9))
return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2AC;_tmp2AC.tag=1;
_tmp2AC.f1=(unsigned long)((int)i);{void*_tmp2AA[1]={& _tmp2AC};Cyc_aprintf(({
const char*_tmp2AB="%d";_tag_dynforward(_tmp2AB,sizeof(char),_get_zero_arr_size(
_tmp2AB,3));}),_tag_dynforward(_tmp2AA,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16F: if(*((int*)_tmp29D)!= 4)goto _LL171;_tmp29F=((
struct Cyc_Absyn_PointerType_struct*)_tmp29D)->f1;_tmp2A0=_tmp29F.ptr_atts;_LL170: {
struct Cyc_Absyn_Conref*_tmp2AD=_tmp2A0.nullable;int is_nullable=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2AD);if(is_nullable){if(!((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp263,
Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp29C)))return({const char*_tmp2AE="NULL";
_tag_dynforward(_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,5));});}return({
const char*_tmp2AF="&_";_tag_dynforward(_tmp2AF,sizeof(char),_get_zero_arr_size(
_tmp2AF,3));});}_LL171: if(*((int*)_tmp29D)!= 2)goto _LL173;_tmp2A1=((struct Cyc_Absyn_TunionType_struct*)
_tmp29D)->f1;_tmp2A2=_tmp2A1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp29D)->f1).tunion_info).KnownTunion).tag != 1)goto _LL173;_tmp2A3=(_tmp2A2.KnownTunion).f1;
_tmp2A4=*_tmp2A3;_LL172: if(_tmp2A4->is_xtunion)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2B0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A4->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2B0);for(0;(unsigned int)_tmp2B0;_tmp2B0=_tmp2B0->tl){struct _dynforward_ptr n=*(*((
struct Cyc_Absyn_Tunionfield*)_tmp2B0->hd)->name).f2;struct Cyc_List_List*_tmp2B1=((
struct Cyc_Absyn_Tunionfield*)_tmp2B0->hd)->typs;if(!((int(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp263,(struct Cyc_Tcpat_Con_s*)({
struct Cyc_Tcpat_Con_s*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->name=(union
Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp2B3;(_tmp2B3.Name_v).tag=
0;(_tmp2B3.Name_v).f1=n;_tmp2B3;});_tmp2B2->arity=0;_tmp2B2->span=0;_tmp2B2->orig_pat=
_tmp29C;_tmp2B2;}))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2B1)
== 0)return n;else{if(_tmp2A4->is_flat)return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
n);{void*_tmp2B4[1]={& _tmp2B6};Cyc_aprintf(({const char*_tmp2B5="%s(...)";
_tag_dynforward(_tmp2B5,sizeof(char),_get_zero_arr_size(_tmp2B5,8));}),
_tag_dynforward(_tmp2B4,sizeof(void*),1));}});else{return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)n);{void*_tmp2B7[1]={& _tmp2B9};Cyc_aprintf(({
const char*_tmp2B8="&%s(...)";_tag_dynforward(_tmp2B8,sizeof(char),
_get_zero_arr_size(_tmp2B8,9));}),_tag_dynforward(_tmp2B7,sizeof(void*),1));}});}}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16A:;}}_LL142:;}static struct _dynforward_ptr*Cyc_Tcpat_desc2stringptr(struct
_RegionHandle*r,void*d){return({struct _dynforward_ptr*_tmp2BA=_cycalloc(sizeof(*
_tmp2BA));_tmp2BA[0]=Cyc_Tcpat_desc2string(r,d);_tmp2BA;});}static struct
_dynforward_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2BB=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dynforward_ptr*comma=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2BC=_tmp2BB;for(0;
_tmp2BC != 0;_tmp2BC=((struct Cyc_List_List*)_check_null(_tmp2BC))->tl){if(_tmp2BC->tl
!= 0){_tmp2BC->tl=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2BD=_cycalloc(
sizeof(*_tmp2BD));_tmp2BD->hd=comma;_tmp2BD->tl=_tmp2BC->tl;_tmp2BD;});_tmp2BC=
_tmp2BC->tl;}}}return(struct _dynforward_ptr)Cyc_strconcat_l(_tmp2BB);}static void
Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
void*desc){struct _handler_cons _tmp2BF;_push_handler(& _tmp2BF);{int _tmp2C1=0;if(
setjmp(_tmp2BF.handler))_tmp2C1=1;if(!_tmp2C1){{struct _dynforward_ptr _tmp2C2=Cyc_Tcpat_desc2string(
r,desc);({struct Cyc_String_pa_struct _tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2C2);{void*_tmp2C3[1]={& _tmp2C5};Cyc_Tcutil_terr(
loc,({const char*_tmp2C4="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,53));}),
_tag_dynforward(_tmp2C3,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2C0=(void*)_exn_thrown;void*_tmp2C7=_tmp2C0;_LL176: if(_tmp2C7 != Cyc_Tcpat_Desc2string)
goto _LL178;_LL177:({void*_tmp2C8[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2C9="patterns may not be exhaustive.";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,32));}),
_tag_dynforward(_tmp2C8,sizeof(void*),0));});goto _LL175;_LL178:;_LL179:(void)
_throw(_tmp2C7);_LL175:;}}}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp2CA=_new_region("r");struct
_RegionHandle*r=& _tmp2CA;_push_region(r);{int _tmp2CB=0;struct _tuple16 _tmp2CC=({
struct _tuple16 _tmp2D8;_tmp2D8.f1=r;_tmp2D8.f2=& _tmp2CB;_tmp2D8;});struct Cyc_List_List*
_tmp2CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp2CC,swcs);void*_tmp2CE=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2CD);int _tmp2CF=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(
struct _RegionHandle*,struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple14*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
r,_tmp2CE,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp2CF);for(0;
_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){struct _tuple15 _tmp2D1;struct _tuple14*_tmp2D2;
struct _tuple14 _tmp2D3;int _tmp2D4;struct Cyc_Position_Segment*_tmp2D5;struct
_tuple15*_tmp2D0=(struct _tuple15*)_tmp2CD->hd;_tmp2D1=*_tmp2D0;_tmp2D2=_tmp2D1.f2;
_tmp2D3=*_tmp2D2;_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;if(!_tmp2D4)({void*
_tmp2D6[0]={};Cyc_Tcutil_terr(_tmp2D5,({const char*_tmp2D7="redundant pattern";
_tag_dynforward(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,18));}),
_tag_dynforward(_tmp2D6,sizeof(void*),0));});}};_pop_region(r);}struct _tuple17{
struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _RegionHandle*r,struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2D9;_push_handler(& _tmp2D9);{int _tmp2DB=0;if(setjmp(_tmp2D9.handler))
_tmp2DB=1;if(!_tmp2DB){{struct _dynforward_ptr _tmp2DC=Cyc_Tcpat_desc2string(r,
desc);({struct Cyc_String_pa_struct _tmp2DF;_tmp2DF.tag=0;_tmp2DF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2DC);{void*_tmp2DD[1]={& _tmp2DF};Cyc_Tcutil_warn((*
pr).f1,({const char*_tmp2DE="pattern not exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2DE,sizeof(char),_get_zero_arr_size(_tmp2DE,45));}),
_tag_dynforward(_tmp2DD,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2DA=(void*)_exn_thrown;void*_tmp2E1=_tmp2DA;_LL17B: if(_tmp2E1 != Cyc_Tcpat_Desc2string)
goto _LL17D;_LL17C:({void*_tmp2E2[0]={};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp2E3="pattern not exhaustive.";_tag_dynforward(_tmp2E3,sizeof(char),
_get_zero_arr_size(_tmp2E3,24));}),_tag_dynforward(_tmp2E2,sizeof(void*),0));});
goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2E1);_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2E4=
_new_region("r");struct _RegionHandle*r=& _tmp2E4;_push_region(r);{struct Cyc_List_List*
_tmp2E5=({struct Cyc_List_List*_tmp2EB=_region_malloc(r,sizeof(*_tmp2EB));_tmp2EB->hd=({
struct _tuple18*_tmp2EC=_region_malloc(r,sizeof(*_tmp2EC));_tmp2EC->f1=Cyc_Tcpat_compile_pat(
r,p);_tmp2EC->f2=0;_tmp2EC;});_tmp2EB->tl=0;_tmp2EB;});void*_tmp2E6=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2E5);struct _tuple17 _tmp2E7=({struct _tuple17 _tmp2EA;_tmp2EA.f1=loc;_tmp2EA.f2=
1;_tmp2EA;});int _tmp2E8=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct _tuple17*,void*),struct _tuple17*env1,
void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
r,_tmp2E6,Cyc_Tcpat_not_exhaust_warn,& _tmp2E7,Cyc_Tcpat_dummy_fn,0,& _tmp2E8);{
int _tmp2E9=_tmp2E7.f2;_npop_handler(0);return _tmp2E9;}};_pop_region(r);}static
struct _tuple15*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp2EE=_region_malloc(r,sizeof(*_tmp2EE));_tmp2EE->f1=0;_tmp2EE->f2=(
swc->pattern)->loc;_tmp2EE;});return({struct _tuple15*_tmp2ED=_region_malloc(r,
sizeof(*_tmp2ED));_tmp2ED->f1=sp0;_tmp2ED->f2=rhs;_tmp2ED;});}static void Cyc_Tcpat_not_exhaust_err2(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){struct _RegionHandle
_tmp2EF=_new_region("r");struct _RegionHandle*r=& _tmp2EF;_push_region(r);{struct
Cyc_List_List*_tmp2F0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,
r,swcs);void*_tmp2F1=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(r,_tmp2F0);int _tmp2F2=0;((void(*)(struct
_RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp2F1,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2F2);for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
struct _tuple15 _tmp2F4;struct _tuple14*_tmp2F5;struct _tuple14 _tmp2F6;int _tmp2F7;
struct Cyc_Position_Segment*_tmp2F8;struct _tuple15*_tmp2F3=(struct _tuple15*)
_tmp2F0->hd;_tmp2F4=*_tmp2F3;_tmp2F5=_tmp2F4.f2;_tmp2F6=*_tmp2F5;_tmp2F7=_tmp2F6.f1;
_tmp2F8=_tmp2F6.f2;if(!_tmp2F7)({void*_tmp2F9[0]={};Cyc_Tcutil_terr(_tmp2F8,({
const char*_tmp2FA="redundant pattern";_tag_dynforward(_tmp2FA,sizeof(char),
_get_zero_arr_size(_tmp2FA,18));}),_tag_dynforward(_tmp2F9,sizeof(void*),0));});}};
_pop_region(r);}
