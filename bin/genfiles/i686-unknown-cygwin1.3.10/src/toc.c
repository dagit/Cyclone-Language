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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion
Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];void Cyc_Core_free_dynregion(struct _DynRegionHandle*);struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,
struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,
void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*
r;struct _dynforward_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _dynforward_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcutil_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dynforward_ptr,int*is_dyneither_ptr);int Cyc_Tcutil_unify(void*,
void*);int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);struct _tuple4{struct
Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct
_tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dynforward_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**tunions_so_far;
struct Cyc_Dict_Dict*xtunions_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple6{
struct _tuple1*f1;struct _dynforward_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple6*
x,struct _tuple6*y){struct _tuple1*_tmp1;struct _dynforward_ptr _tmp2;struct _tuple6
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct
_dynforward_ptr _tmp5;struct _tuple6 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=
Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct
_dynforward_ptr)_tmp2,(struct _dynforward_ptr)_tmp5);}}}struct _tuple7{struct Cyc_Absyn_Aggrdecl*
f1;void*f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(
struct _tuple1*)){struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct
Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct
_RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct _tuple7**v=((struct _tuple7**(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple7 _tmpC;
void*_tmpD;struct _tuple7*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmpF[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp10="\n";_tag_dynforward(
_tmp10,sizeof(char),_get_zero_arr_size(_tmp10,2));}),_tag_dynforward(_tmpF,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp11[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";_tag_dynforward(
_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,2));}),_tag_dynforward(_tmp11,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp13[5]="curr";static struct _dynforward_ptr Cyc_Toc_curr_string={
_tmp13,_tmp13 + 5};static struct _dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp14[4]="tag";static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp14,
_tmp14 + 4};static struct _dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static
char _tmp15[4]="val";static struct _dynforward_ptr Cyc_Toc_val_string={_tmp15,_tmp15
+ 4};static struct _dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char
_tmp16[14]="_handler_cons";static struct _dynforward_ptr Cyc_Toc__handler_cons_string={
_tmp16,_tmp16 + 14};static struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp17[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp17,_tmp17 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp18[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp18,_tmp18 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp19[17]="_DynRegionHandle";static struct _dynforward_ptr Cyc_Toc__DynRegionHandle_string={
_tmp19,_tmp19 + 17};static struct _dynforward_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1A[16]="_DynRegionFrame";static struct _dynforward_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1A,_tmp1A + 16};static struct _dynforward_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dynforward_ptr Cyc_Toc_globals={(void*)0,(void*)(0 + 0)};static char _tmp1B[7]="_throw";
static struct _dynforward_ptr Cyc_Toc__throw_str={_tmp1B,_tmp1B + 7};static struct
_tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,&
Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp1E[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmp1E,_tmp1E + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp21[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp21,
_tmp21 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp27[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp2A[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp2D[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp2D,_tmp2D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30 + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp33[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp33,_tmp33 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp36,
_tmp36 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp39[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp39,_tmp39 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp3C[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp3C,_tmp3C + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp3F[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp3F,_tmp3F + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp42[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp42,_tmp42 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp45[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp45,_tmp45 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp48[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp48,_tmp48 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp4B[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp4B,_tmp4B + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp4E[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp4E,_tmp4E + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp51[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp51,_tmp51 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp54[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp54,_tmp54 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp57[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp57,_tmp57 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp5A[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp5A,_tmp5A + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp5D[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp5D,_tmp5D + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp60[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp60,_tmp60 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp63[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp63,_tmp63 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp66[29]="_dynforward_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp66,
_tmp66 + 29};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,&
Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp69[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp69,
_tmp69 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp6C[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp6C,_tmp6C + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp6F[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp6F,_tmp6F + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp72[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp72,
_tmp72 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp75[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp75,_tmp75 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmp78[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmp78,_tmp78 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmp7B[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp7B,_tmp7B + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp7E[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp7E,_tmp7E + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmp81[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp81,_tmp81 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp84[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmp84,_tmp84 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp87[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp87,_tmp87 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp8A[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp8A,_tmp8A + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp8D[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmp8D,_tmp8D + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp90[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp90,_tmp90 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp93[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmp93,_tmp93 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp96[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp96,_tmp96 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp99[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmp99,_tmp99 + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp9C[16]="_open_dynregion";static struct _dynforward_ptr Cyc_Toc__open_dynregion_str={
_tmp9C,_tmp9C + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((void*)& Cyc_Toc__open_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=&
Cyc_Toc__open_dynregion_ev;static char _tmp9F[16]="_push_dynregion";static struct
_dynforward_ptr Cyc_Toc__push_dynregion_str={_tmp9F,_tmp9F + 16};static struct
_tuple1 Cyc_Toc__push_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={
0,(void*)((void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmpA2[15]="_pop_dynregion";static struct _dynforward_ptr Cyc_Toc__pop_dynregion_str={
_tmpA2,_tmpA2 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((void*)& Cyc_Toc__pop_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=&
Cyc_Toc__pop_dynregion_ev;static char _tmpA5[14]="_reset_region";static struct
_dynforward_ptr Cyc_Toc__reset_region_str={_tmpA5,_tmpA5 + 14};static struct _tuple1
Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmpA8[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmpA8,_tmpA8 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpAB[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmpAB,_tmpAB + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmpAE[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpAE,
_tmpAE + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpB1[11]="_swap_word";static
struct _dynforward_ptr Cyc_Toc__swap_word_str={_tmpB1,_tmpB1 + 11};static struct
_tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmpB4[17]="_swap_dynforward";static struct _dynforward_ptr Cyc_Toc__swap_dynforward_str={
_tmpB4,_tmpB4 + 17};static struct _tuple1 Cyc_Toc__swap_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__swap_dynforward_re={1,& Cyc_Toc__swap_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__swap_dynforward_ev={0,(void*)((void*)& Cyc_Toc__swap_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dynforward_e=&
Cyc_Toc__swap_dynforward_ev;static char _tmpB7[16]="_swap_dyneither";static struct
_dynforward_ptr Cyc_Toc__swap_dyneither_str={_tmpB7,_tmpB7 + 16};static struct
_tuple1 Cyc_Toc__swap_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={
1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr}),
0}};static void*Cyc_Toc_dynforward_ptr_typ=(void*)& Cyc_Toc_dynforward_ptr_typ_v;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmpBE=_cycalloc(sizeof(*
_tmpBE));_tmpBE[0]=Cyc_Absyn_skip_stmt(0);_tmpBE;});return*skip_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(
t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));
_tmpBF[0]=({struct Cyc_Absyn_Cast_e_struct _tmpC0;_tmpC0.tag=15;_tmpC0.f1=(void*)t;
_tmpC0.f2=e;_tmpC0.f3=0;_tmpC0.f4=(void*)((void*)1);_tmpC0;});_tmpBF;});}static
void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Deref_e_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_Deref_e_struct
_tmpC2;_tmpC2.tag=22;_tmpC2.f1=e;_tmpC2;});_tmpC1;});}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmpC4;_tmpC4.tag=25;_tmpC4.f1=e1;_tmpC4.f2=e2;_tmpC4;});_tmpC3;});}static void*
Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmpC6;_tmpC6.tag=38;_tmpC6.f1=s;_tmpC6;});_tmpC5;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpC7=_cycalloc(
sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC8;_tmpC8.tag=
18;_tmpC8.f1=(void*)t;_tmpC8;});_tmpC7;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpCA;_tmpCA.tag=11;_tmpCA.f1=e;_tmpCA.f2=es;_tmpCA.f3=0;_tmpCA;});_tmpC9;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_Exp_s_struct _tmpCC;
_tmpCC.tag=0;_tmpCC.f1=e;_tmpCC;});_tmpCB;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Seq_s_struct _tmpCE;
_tmpCE.tag=1;_tmpCE.f1=s1;_tmpCE.f2=s2;_tmpCE;});_tmpCD;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));
_tmpCF[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpD0;_tmpD0.tag=6;_tmpD0.f1=e1;
_tmpD0.f2=e2;_tmpD0.f3=e3;_tmpD0;});_tmpCF;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpD2;_tmpD2.tag=23;_tmpD2.f1=e;_tmpD2.f2=n;_tmpD2;});_tmpD1;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpD4;_tmpD4.tag=24;_tmpD4.f1=e;_tmpD4.f2=n;_tmpD4;});_tmpD3;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD6;_tmpD6.tag=37;_tmpD6.f1=tdopt;_tmpD6.f2=ds;_tmpD6;});_tmpD5;});}static
void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){return(
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpD8;_tmpD8.tag=7;_tmpD8.f1=v;_tmpD8.f2=s;_tmpD8;});
_tmpD7;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpDB=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpDC;char _tmpDD;union Cyc_Absyn_Cnst_union
_tmpDE;short _tmpDF;union Cyc_Absyn_Cnst_union _tmpE0;int _tmpE1;union Cyc_Absyn_Cnst_union
_tmpE2;long long _tmpE3;union Cyc_Absyn_Cnst_union _tmpE4;struct Cyc_Absyn_Exp*
_tmpE5;struct Cyc_List_List*_tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_List_List*
_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpEA;_LL1: if(*((int*)
_tmpDB)!= 0)goto _LL3;_tmpDC=((struct Cyc_Absyn_Const_e_struct*)_tmpDB)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpDB)->f1).Char_c).tag != 0)goto _LL3;_tmpDD=(
_tmpDC.Char_c).f2;_LL2: return _tmpDD == '\000';_LL3: if(*((int*)_tmpDB)!= 0)goto
_LL5;_tmpDE=((struct Cyc_Absyn_Const_e_struct*)_tmpDB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpDB)->f1).Short_c).tag != 1)goto _LL5;_tmpDF=(_tmpDE.Short_c).f2;_LL4: return
_tmpDF == 0;_LL5: if(*((int*)_tmpDB)!= 0)goto _LL7;_tmpE0=((struct Cyc_Absyn_Const_e_struct*)
_tmpDB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpDB)->f1).Int_c).tag != 2)
goto _LL7;_tmpE1=(_tmpE0.Int_c).f2;_LL6: return _tmpE1 == 0;_LL7: if(*((int*)_tmpDB)
!= 0)goto _LL9;_tmpE2=((struct Cyc_Absyn_Const_e_struct*)_tmpDB)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmpDB)->f1).LongLong_c).tag != 3)goto _LL9;_tmpE3=(
_tmpE2.LongLong_c).f2;_LL8: return _tmpE3 == 0;_LL9: if(*((int*)_tmpDB)!= 0)goto _LLB;
_tmpE4=((struct Cyc_Absyn_Const_e_struct*)_tmpDB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpDB)->f1).Null_c).tag != 6)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmpDB)!= 15)
goto _LLD;_tmpE5=((struct Cyc_Absyn_Cast_e_struct*)_tmpDB)->f2;_LLC: return Cyc_Toc_is_zero(
_tmpE5);_LLD: if(*((int*)_tmpDB)!= 26)goto _LLF;_tmpE6=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpDB)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpE6);_LLF: if(*((int*)_tmpDB)!= 28)goto
_LL11;_tmpE7=((struct Cyc_Absyn_Array_e_struct*)_tmpDB)->f1;_LL10: _tmpE8=_tmpE7;
goto _LL12;_LL11: if(*((int*)_tmpDB)!= 30)goto _LL13;_tmpE8=((struct Cyc_Absyn_Struct_e_struct*)
_tmpDB)->f3;_LL12: _tmpE9=_tmpE8;goto _LL14;_LL13: if(*((int*)_tmpDB)!= 27)goto
_LL15;_tmpE9=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpDB)->f2;_LL14: _tmpEA=
_tmpE9;goto _LL16;_LL15: if(*((int*)_tmpDB)!= 37)goto _LL17;_tmpEA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpDB)->f2;_LL16: for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple8*)_tmpEA->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpEB=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpEC;struct Cyc_Absyn_PtrAtts _tmpED;struct Cyc_Absyn_Conref*
_tmpEE;_LL1A: if(_tmpEB <= (void*)4)goto _LL1C;if(*((int*)_tmpEB)!= 4)goto _LL1C;
_tmpEC=((struct Cyc_Absyn_PointerType_struct*)_tmpEB)->f1;_tmpED=_tmpEC.ptr_atts;
_tmpEE=_tmpED.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpEE);_LL1C:;_LL1D:({void*_tmpEF[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpF0="is_nullable";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,12));}),
_tag_dynforward(_tmpEF,sizeof(void*),0));});_LL19:;}static char _tmpFD[1]="";
static char _tmpFF[8]="*bogus*";static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){struct _DynRegionHandle*_tmpF2;struct
Cyc_Dict_Dict*_tmpF3;struct Cyc_Toc_TocState _tmpF1=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmpF2=_tmpF1.dyn;_tmpF3=_tmpF1.qvar_tags;{
static struct _dynforward_ptr bogus_string={_tmpFF,_tmpFF + 8};static struct _tuple1
bogus_qvar={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),&
bogus_string};static struct _tuple6 pair={& bogus_qvar,{_tmpFD,_tmpFD + 1}};pair=({
struct _tuple6 _tmpF4;_tmpF4.f1=x;_tmpF4.f2=tag;_tmpF4;});{struct _DynRegionFrame
_tmpF5;struct _RegionHandle*d=_open_dynregion(& _tmpF5,_tmpF2);{struct _tuple1**
_tmpF6=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple6*k))Cyc_Dict_lookup_opt)(*
_tmpF3,(struct _tuple6*)& pair);if(_tmpF6 != 0){struct _tuple1*_tmpF7=*_tmpF6;
_npop_handler(0);return _tmpF7;}{struct _tuple6*_tmpF8=({struct _tuple6*_tmpFC=
_cycalloc(sizeof(*_tmpFC));_tmpFC->f1=x;_tmpFC->f2=tag;_tmpFC;});struct _tuple1*
res=({struct _tuple1*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->f1=(*x).f1;_tmpFA->f2=({
struct _dynforward_ptr*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=(struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)*(*x).f2,(struct
_dynforward_ptr)tag);_tmpFB;});_tmpFA;});*_tmpF3=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _tuple6*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpF3,(struct
_tuple6*)_tmpF8,res);{struct _tuple1*_tmpF9=res;_npop_handler(0);return _tmpF9;}}};
_pop_dynregion(d);}}}struct _tuple9{void*f1;struct Cyc_List_List*f2;};static void*
Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp101;
struct Cyc_List_List**_tmp102;struct Cyc_Toc_TocState _tmp100=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp101=_tmp100.dyn;_tmp102=_tmp100.tuple_types;{
struct _DynRegionFrame _tmp103;struct _RegionHandle*d=_open_dynregion(& _tmp103,
_tmp101);{struct Cyc_List_List*_tmp104=*_tmp102;for(0;_tmp104 != 0;_tmp104=_tmp104->tl){
struct _tuple9 _tmp106;void*_tmp107;struct Cyc_List_List*_tmp108;struct _tuple9*
_tmp105=(struct _tuple9*)_tmp104->hd;_tmp106=*_tmp105;_tmp107=_tmp106.f1;_tmp108=
_tmp106.f2;{struct Cyc_List_List*_tmp109=tqs0;for(0;_tmp109 != 0  && _tmp108 != 0;(
_tmp109=_tmp109->tl,_tmp108=_tmp108->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)
_tmp109->hd)).f2,(void*)_tmp108->hd))break;}if(_tmp109 == 0  && _tmp108 == 0){void*
_tmp10A=_tmp107;_npop_handler(0);return _tmp10A;}}}}{struct _dynforward_ptr*xname=({
struct _dynforward_ptr*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp11C;_tmp11C.tag=1;_tmp11C.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp11A[1]={& _tmp11C};Cyc_aprintf(({
const char*_tmp11B="_tuple%d";_tag_dynforward(_tmp11B,sizeof(char),
_get_zero_arr_size(_tmp11B,9));}),_tag_dynforward(_tmp11A,sizeof(void*),1));}});
_tmp119;});void*x=Cyc_Absyn_strct(xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(
d,Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp10B=0;struct Cyc_List_List*ts2=
ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){_tmp10B=({struct Cyc_List_List*
_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=({struct Cyc_Absyn_Aggrfield*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->name=Cyc_Absyn_fieldname(i);_tmp10D->tq=
Cyc_Toc_mt_tq;_tmp10D->type=(void*)((void*)ts2->hd);_tmp10D->width=0;_tmp10D->attributes=
0;_tmp10D;});_tmp10C->tl=_tmp10B;_tmp10C;});}}_tmp10B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10B);{struct Cyc_Absyn_Aggrdecl*
_tmp10E=({struct Cyc_Absyn_Aggrdecl*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->kind=(
void*)((void*)0);_tmp115->sc=(void*)((void*)2);_tmp115->name=({struct _tuple1*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp118;(_tmp118.Rel_n).tag=1;(_tmp118.Rel_n).f1=0;
_tmp118;});_tmp117->f2=xname;_tmp117;});_tmp115->tvs=0;_tmp115->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->exist_vars=0;_tmp116->rgn_po=0;
_tmp116->fields=_tmp10B;_tmp116;});_tmp115->attributes=0;_tmp115;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp110=_cycalloc(sizeof(*_tmp110));
_tmp110[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp111;_tmp111.tag=4;_tmp111.f1=
_tmp10E;_tmp111;});_tmp110;}),0);_tmp10F->tl=Cyc_Toc_result_decls;_tmp10F;});*
_tmp102=({struct Cyc_List_List*_tmp112=_region_malloc(d,sizeof(*_tmp112));_tmp112->hd=({
struct _tuple9*_tmp113=_region_malloc(d,sizeof(*_tmp113));_tmp113->f1=x;_tmp113->f2=
ts;_tmp113;});_tmp112->tl=*_tmp102;_tmp112;});{void*_tmp114=x;_npop_handler(0);
return _tmp114;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){int
_tmp11D=Cyc_Toc_temp_var_counter ++;struct _tuple1*res=({struct _tuple1*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp123;(_tmp123.Loc_n).tag=0;_tmp123;});_tmp11E->f2=({struct _dynforward_ptr*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=(struct _dynforward_ptr)({struct
Cyc_Int_pa_struct _tmp122;_tmp122.tag=1;_tmp122.f1=(unsigned int)_tmp11D;{void*
_tmp120[1]={& _tmp122};Cyc_aprintf(({const char*_tmp121="_tmp%X";_tag_dynforward(
_tmp121,sizeof(char),_get_zero_arr_size(_tmp121,7));}),_tag_dynforward(_tmp120,
sizeof(void*),1));}});_tmp11F;});_tmp11E;});return res;}static struct
_dynforward_ptr*Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp125;struct Cyc_Xarray_Xarray*
_tmp126;struct Cyc_Toc_TocState _tmp124=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp125=_tmp124.dyn;_tmp126=_tmp124.temp_labels;{struct _DynRegionFrame _tmp127;
struct _RegionHandle*d=_open_dynregion(& _tmp127,_tmp125);{int _tmp128=Cyc_Toc_fresh_label_counter
++;if(_tmp128 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp126)){
struct _dynforward_ptr*_tmp129=((struct _dynforward_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp126,_tmp128);_npop_handler(0);return _tmp129;}{struct
_dynforward_ptr*res=({struct _dynforward_ptr*_tmp12D=_cycalloc(sizeof(*_tmp12D));
_tmp12D[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp130;_tmp130.tag=1;
_tmp130.f1=(unsigned int)_tmp128;{void*_tmp12E[1]={& _tmp130};Cyc_aprintf(({const
char*_tmp12F="_LL%X";_tag_dynforward(_tmp12F,sizeof(char),_get_zero_arr_size(
_tmp12F,6));}),_tag_dynforward(_tmp12E,sizeof(void*),1));}});_tmp12D;});if(((int(*)(
struct Cyc_Xarray_Xarray*,struct _dynforward_ptr*))Cyc_Xarray_add_ind)(_tmp126,res)
!= _tmp128)({void*_tmp12A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp12B="fresh_label: add_ind returned bad index...";
_tag_dynforward(_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,43));}),
_tag_dynforward(_tmp12A,sizeof(void*),0));});{struct _dynforward_ptr*_tmp12C=res;
_npop_handler(0);return _tmp12C;}}};_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct _tuple1*name,int
carries_value){int ans=0;struct Cyc_List_List*_tmp131=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp131))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp131->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp131->hd)->typs == 0)
++ ans;_tmp131=_tmp131->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmp132=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp132 != 0;_tmp132=_tmp132->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp132->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp134;struct Cyc_Absyn_Tqual
_tmp135;void*_tmp136;struct _tuple2 _tmp133=*a;_tmp134=_tmp133.f1;_tmp135=_tmp133.f2;
_tmp136=_tmp133.f3;return({struct _tuple2*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137->f1=_tmp134;_tmp137->f2=_tmp135;_tmp137->f3=Cyc_Toc_typ_to_c(_tmp136);
_tmp137;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp139;void*_tmp13A;struct _tuple4 _tmp138=*x;_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;
return({struct _tuple4*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->f1=_tmp139;
_tmp13B->f2=Cyc_Toc_typ_to_c(_tmp13A);_tmp13B;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp13C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp13D;
void*_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Conref*
_tmp141;struct Cyc_Position_Segment*_tmp142;struct Cyc_Core_Opt*_tmp143;struct Cyc_Core_Opt
_tmp144;void*_tmp145;_LL1F: if(_tmp13C <= (void*)4)goto _LL23;if(*((int*)_tmp13C)!= 
7)goto _LL21;_tmp13D=((struct Cyc_Absyn_ArrayType_struct*)_tmp13C)->f1;_tmp13E=(
void*)_tmp13D.elt_type;_tmp13F=_tmp13D.tq;_tmp140=_tmp13D.num_elts;_tmp141=
_tmp13D.zero_term;_tmp142=_tmp13D.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp13E),_tmp13F,_tmp140,Cyc_Absyn_false_conref,_tmp142);_LL21: if(*((int*)
_tmp13C)!= 0)goto _LL23;_tmp143=((struct Cyc_Absyn_Evar_struct*)_tmp13C)->f2;if(
_tmp143 == 0)goto _LL23;_tmp144=*_tmp143;_tmp145=(void*)_tmp144.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmp145);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->name=f->name;_tmp146->tq=Cyc_Toc_mt_tq;
_tmp146->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp146->width=f->width;
_tmp146->attributes=f->attributes;_tmp146;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){static void**
cs=0;if(cs == 0)cs=({void**_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=Cyc_Absyn_star_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp147;});
return*cs;}static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0)r=({void**
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp148;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;if(r == 0)r=({void**_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);
_tmp149;});return*r;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp14A=t;struct
Cyc_Core_Opt*_tmp14B;struct Cyc_Core_Opt*_tmp14C;struct Cyc_Core_Opt _tmp14D;void*
_tmp14E;struct Cyc_Absyn_Tvar*_tmp14F;struct Cyc_Absyn_TunionInfo _tmp150;union Cyc_Absyn_TunionInfoU_union
_tmp151;struct Cyc_Absyn_Tuniondecl**_tmp152;struct Cyc_Absyn_Tuniondecl*_tmp153;
struct Cyc_Absyn_TunionFieldInfo _tmp154;union Cyc_Absyn_TunionFieldInfoU_union
_tmp155;struct Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Tunionfield*_tmp157;
struct Cyc_Absyn_PtrInfo _tmp158;void*_tmp159;struct Cyc_Absyn_Tqual _tmp15A;struct
Cyc_Absyn_PtrAtts _tmp15B;struct Cyc_Absyn_Conref*_tmp15C;struct Cyc_Absyn_ArrayInfo
_tmp15D;void*_tmp15E;struct Cyc_Absyn_Tqual _tmp15F;struct Cyc_Absyn_Exp*_tmp160;
struct Cyc_Position_Segment*_tmp161;struct Cyc_Absyn_FnInfo _tmp162;void*_tmp163;
struct Cyc_List_List*_tmp164;int _tmp165;struct Cyc_Absyn_VarargInfo*_tmp166;struct
Cyc_List_List*_tmp167;struct Cyc_List_List*_tmp168;void*_tmp169;struct Cyc_List_List*
_tmp16A;struct Cyc_Absyn_AggrInfo _tmp16B;union Cyc_Absyn_AggrInfoU_union _tmp16C;
struct Cyc_List_List*_tmp16D;struct _tuple1*_tmp16E;struct Cyc_List_List*_tmp16F;
struct _tuple1*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Typedefdecl*
_tmp172;void**_tmp173;void*_tmp174;_LL26: if((int)_tmp14A != 0)goto _LL28;_LL27:
return t;_LL28: if(_tmp14A <= (void*)4)goto _LL3A;if(*((int*)_tmp14A)!= 0)goto _LL2A;
_tmp14B=((struct Cyc_Absyn_Evar_struct*)_tmp14A)->f2;if(_tmp14B != 0)goto _LL2A;
_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(*((int*)_tmp14A)!= 0)goto _LL2C;_tmp14C=((
struct Cyc_Absyn_Evar_struct*)_tmp14A)->f2;if(_tmp14C == 0)goto _LL2C;_tmp14D=*
_tmp14C;_tmp14E=(void*)_tmp14D.v;_LL2B: return Cyc_Toc_typ_to_c(_tmp14E);_LL2C: if(*((
int*)_tmp14A)!= 1)goto _LL2E;_tmp14F=((struct Cyc_Absyn_VarType_struct*)_tmp14A)->f1;
_LL2D: if(Cyc_Tcutil_tvar_kind(_tmp14F)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL2E: if(*((int*)_tmp14A)!= 2)goto _LL30;_tmp150=((struct Cyc_Absyn_TunionType_struct*)
_tmp14A)->f1;_tmp151=_tmp150.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp14A)->f1).tunion_info).KnownTunion).tag != 1)goto _LL30;_tmp152=(_tmp151.KnownTunion).f1;
_tmp153=*_tmp152;_LL2F: if(_tmp153->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp153->name,({const char*_tmp175="_union";_tag_dynforward(_tmp175,sizeof(char),
_get_zero_arr_size(_tmp175,7));})));else{return Cyc_Absyn_void_star_typ();}_LL30:
if(*((int*)_tmp14A)!= 2)goto _LL32;_LL31:({void*_tmp176[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp177="unresolved TunionType";_tag_dynforward(_tmp177,sizeof(char),
_get_zero_arr_size(_tmp177,22));}),_tag_dynforward(_tmp176,sizeof(void*),0));});
_LL32: if(*((int*)_tmp14A)!= 3)goto _LL34;_tmp154=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp14A)->f1;_tmp155=_tmp154.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp14A)->f1).field_info).KnownTunionfield).tag != 1)goto _LL34;_tmp156=(_tmp155.KnownTunionfield).f1;
_tmp157=(_tmp155.KnownTunionfield).f2;_LL33: if(_tmp156->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp156->name,({const char*_tmp178="_union";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,7));})));if(
_tmp157->typs == 0){if(_tmp156->is_xtunion)return Cyc_Toc_char_star_typ();else{
return Cyc_Absyn_uint_typ;}}else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp157->name,({const char*_tmp179="_struct";_tag_dynforward(_tmp179,sizeof(char),
_get_zero_arr_size(_tmp179,8));})));}_LL34: if(*((int*)_tmp14A)!= 3)goto _LL36;
_LL35:({void*_tmp17A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp17B="unresolved TunionFieldType";
_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size(_tmp17B,27));}),
_tag_dynforward(_tmp17A,sizeof(void*),0));});_LL36: if(*((int*)_tmp14A)!= 4)goto
_LL38;_tmp158=((struct Cyc_Absyn_PointerType_struct*)_tmp14A)->f1;_tmp159=(void*)
_tmp158.elt_typ;_tmp15A=_tmp158.elt_tq;_tmp15B=_tmp158.ptr_atts;_tmp15C=_tmp15B.bounds;
_LL37: _tmp159=Cyc_Toc_typ_to_c_array(_tmp159);{union Cyc_Absyn_Constraint_union
_tmp17C=(Cyc_Absyn_compress_conref(_tmp15C))->v;void*_tmp17D;void*_tmp17E;_LL63:
if((_tmp17C.No_constr).tag != 2)goto _LL65;_LL64: goto _LL66;_LL65: if((_tmp17C.Eq_constr).tag
!= 0)goto _LL67;_tmp17D=(_tmp17C.Eq_constr).f1;if((int)_tmp17D != 0)goto _LL67;
_LL66: return Cyc_Toc_dynforward_ptr_typ;_LL67: if((_tmp17C.Eq_constr).tag != 0)goto
_LL69;_tmp17E=(_tmp17C.Eq_constr).f1;if((int)_tmp17E != 1)goto _LL69;_LL68: return
Cyc_Toc_dyneither_ptr_typ;_LL69:;_LL6A: return Cyc_Absyn_star_typ(_tmp159,(void*)2,
_tmp15A,Cyc_Absyn_false_conref);_LL62:;}_LL38: if(*((int*)_tmp14A)!= 5)goto _LL3A;
_LL39: goto _LL3B;_LL3A: if((int)_tmp14A != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(
_tmp14A <= (void*)4)goto _LL58;if(*((int*)_tmp14A)!= 6)goto _LL3E;_LL3D: return t;
_LL3E: if(*((int*)_tmp14A)!= 7)goto _LL40;_tmp15D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp14A)->f1;_tmp15E=(void*)_tmp15D.elt_type;_tmp15F=_tmp15D.tq;_tmp160=_tmp15D.num_elts;
_tmp161=_tmp15D.zt_loc;_LL3F: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp15E),_tmp15F,_tmp160,Cyc_Absyn_false_conref,_tmp161);_LL40: if(*((int*)
_tmp14A)!= 8)goto _LL42;_tmp162=((struct Cyc_Absyn_FnType_struct*)_tmp14A)->f1;
_tmp163=(void*)_tmp162.ret_typ;_tmp164=_tmp162.args;_tmp165=_tmp162.c_varargs;
_tmp166=_tmp162.cyc_varargs;_tmp167=_tmp162.attributes;_LL41: {struct Cyc_List_List*
_tmp17F=0;for(0;_tmp167 != 0;_tmp167=_tmp167->tl){void*_tmp180=(void*)_tmp167->hd;
_LL6C: if((int)_tmp180 != 3)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmp180 != 4)
goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp180 <= (void*)17)goto _LL74;if(*((int*)
_tmp180)!= 3)goto _LL72;_LL71: continue;_LL72: if(*((int*)_tmp180)!= 4)goto _LL74;
_LL73: continue;_LL74:;_LL75: _tmp17F=({struct Cyc_List_List*_tmp181=_cycalloc(
sizeof(*_tmp181));_tmp181->hd=(void*)((void*)_tmp167->hd);_tmp181->tl=_tmp17F;
_tmp181;});goto _LL6B;_LL6B:;}{struct Cyc_List_List*_tmp182=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp164);if(_tmp166 != 0){void*_tmp183=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp166->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp184=({struct _tuple2*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->f1=
_tmp166->name;_tmp186->f2=_tmp166->tq;_tmp186->f3=_tmp183;_tmp186;});_tmp182=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp182,({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->hd=
_tmp184;_tmp185->tl=0;_tmp185;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_FnType_struct
_tmp188;_tmp188.tag=8;_tmp188.f1=({struct Cyc_Absyn_FnInfo _tmp189;_tmp189.tvars=0;
_tmp189.effect=0;_tmp189.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp163);_tmp189.args=
_tmp182;_tmp189.c_varargs=_tmp165;_tmp189.cyc_varargs=0;_tmp189.rgn_po=0;_tmp189.attributes=
_tmp17F;_tmp189;});_tmp188;});_tmp187;});}}_LL42: if(*((int*)_tmp14A)!= 9)goto
_LL44;_tmp168=((struct Cyc_Absyn_TupleType_struct*)_tmp14A)->f1;_LL43: _tmp168=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp168);return Cyc_Toc_add_tuple_type(_tmp168);
_LL44: if(*((int*)_tmp14A)!= 11)goto _LL46;_tmp169=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14A)->f1;_tmp16A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp14A)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp18A=_cycalloc(sizeof(*
_tmp18A));_tmp18A[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp18B;_tmp18B.tag=11;
_tmp18B.f1=(void*)_tmp169;_tmp18B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp16A);_tmp18B;});_tmp18A;});_LL46: if(*((int*)_tmp14A)!= 10)goto _LL48;_tmp16B=((
struct Cyc_Absyn_AggrType_struct*)_tmp14A)->f1;_tmp16C=_tmp16B.aggr_info;_tmp16D=
_tmp16B.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmp18C=Cyc_Absyn_get_known_aggrdecl(
_tmp16C);if((void*)_tmp18C->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp18C->name,
Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp18C->name,Cyc_Absyn_strctq);}}
_LL48: if(*((int*)_tmp14A)!= 12)goto _LL4A;_tmp16E=((struct Cyc_Absyn_EnumType_struct*)
_tmp14A)->f1;_LL49: return t;_LL4A: if(*((int*)_tmp14A)!= 13)goto _LL4C;_tmp16F=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp14A)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmp16F);return t;_LL4C: if(*((int*)_tmp14A)!= 17)goto _LL4E;_tmp170=((struct Cyc_Absyn_TypedefType_struct*)
_tmp14A)->f1;_tmp171=((struct Cyc_Absyn_TypedefType_struct*)_tmp14A)->f2;_tmp172=((
struct Cyc_Absyn_TypedefType_struct*)_tmp14A)->f3;_tmp173=((struct Cyc_Absyn_TypedefType_struct*)
_tmp14A)->f4;_LL4D: if(_tmp173 == 0  || Cyc_noexpand_r){if(_tmp171 != 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp18E;_tmp18E.tag=17;_tmp18E.f1=_tmp170;
_tmp18E.f2=0;_tmp18E.f3=_tmp172;_tmp18E.f4=0;_tmp18E;});_tmp18D;});else{return t;}}
else{return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp18F=_cycalloc(sizeof(*
_tmp18F));_tmp18F[0]=({struct Cyc_Absyn_TypedefType_struct _tmp190;_tmp190.tag=17;
_tmp190.f1=_tmp170;_tmp190.f2=0;_tmp190.f3=_tmp172;_tmp190.f4=({void**_tmp191=
_cycalloc(sizeof(*_tmp191));_tmp191[0]=Cyc_Toc_typ_to_c_array(*_tmp173);_tmp191;});
_tmp190;});_tmp18F;});}_LL4E: if(*((int*)_tmp14A)!= 14)goto _LL50;_LL4F: goto _LL51;
_LL50: if(*((int*)_tmp14A)!= 18)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;_LL52:
if(*((int*)_tmp14A)!= 15)goto _LL54;_tmp174=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp14A)->f1;_LL53: return Cyc_Toc_rgn_typ();_LL54: if(*((int*)_tmp14A)!= 16)goto
_LL56;_LL55: return Cyc_Toc_dyn_rgn_typ();_LL56: if(*((int*)_tmp14A)!= 19)goto _LL58;
_LL57:({void*_tmp192[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp193="Toc::typ_to_c: type translation passed a type integer";
_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,54));}),
_tag_dynforward(_tmp192,sizeof(void*),0));});_LL58: if((int)_tmp14A != 2)goto _LL5A;
_LL59:({void*_tmp194[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp195="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,55));}),
_tag_dynforward(_tmp194,sizeof(void*),0));});_LL5A: if((int)_tmp14A != 3)goto _LL5C;
_LL5B:({void*_tmp196[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp197="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,57));}),
_tag_dynforward(_tmp196,sizeof(void*),0));});_LL5C: if(_tmp14A <= (void*)4)goto
_LL5E;if(*((int*)_tmp14A)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmp14A <= (
void*)4)goto _LL60;if(*((int*)_tmp14A)!= 21)goto _LL60;_LL5F: goto _LL61;_LL60: if(
_tmp14A <= (void*)4)goto _LL25;if(*((int*)_tmp14A)!= 22)goto _LL25;_LL61:({void*
_tmp198[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp199="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size(_tmp199,49));}),
_tag_dynforward(_tmp198,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp19A=t;struct Cyc_Absyn_ArrayInfo _tmp19B;void*_tmp19C;struct Cyc_Absyn_Tqual
_tmp19D;_LL77: if(_tmp19A <= (void*)4)goto _LL79;if(*((int*)_tmp19A)!= 7)goto _LL79;
_tmp19B=((struct Cyc_Absyn_ArrayType_struct*)_tmp19A)->f1;_tmp19C=(void*)_tmp19B.elt_type;
_tmp19D=_tmp19B.tq;_LL78: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp19C,(void*)
2,_tmp19D,Cyc_Absyn_false_conref),e);_LL79:;_LL7A: return Cyc_Toc_cast_it(t,e);
_LL76:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp19E=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp19F;void*_tmp1A0;struct Cyc_Absyn_AggrInfo _tmp1A1;
union Cyc_Absyn_AggrInfoU_union _tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_TunionFieldInfo
_tmp1A4;union Cyc_Absyn_TunionFieldInfoU_union _tmp1A5;struct Cyc_Absyn_Tuniondecl*
_tmp1A6;struct Cyc_Absyn_Tunionfield*_tmp1A7;struct Cyc_List_List*_tmp1A8;_LL7C:
if((int)_tmp19E != 0)goto _LL7E;_LL7D: return 1;_LL7E: if(_tmp19E <= (void*)4)goto
_LL86;if(*((int*)_tmp19E)!= 1)goto _LL80;_LL7F: return 0;_LL80: if(*((int*)_tmp19E)
!= 5)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp19E)!= 12)goto _LL84;_LL83:
goto _LL85;_LL84: if(*((int*)_tmp19E)!= 13)goto _LL86;_LL85: goto _LL87;_LL86: if((int)
_tmp19E != 1)goto _LL88;_LL87: goto _LL89;_LL88: if(_tmp19E <= (void*)4)goto _LLA0;if(*((
int*)_tmp19E)!= 6)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)_tmp19E)!= 8)goto
_LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp19E)!= 18)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if(*((int*)_tmp19E)!= 14)goto _LL90;_LL8F: return 1;_LL90: if(*((int*)_tmp19E)
!= 7)goto _LL92;_tmp19F=((struct Cyc_Absyn_ArrayType_struct*)_tmp19E)->f1;_tmp1A0=(
void*)_tmp19F.elt_type;_LL91: return Cyc_Toc_atomic_typ(_tmp1A0);_LL92: if(*((int*)
_tmp19E)!= 10)goto _LL94;_tmp1A1=((struct Cyc_Absyn_AggrType_struct*)_tmp19E)->f1;
_tmp1A2=_tmp1A1.aggr_info;_LL93:{union Cyc_Absyn_AggrInfoU_union _tmp1A9=_tmp1A2;
_LLA3: if((_tmp1A9.UnknownAggr).tag != 0)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6: goto
_LLA2;_LLA2:;}{struct Cyc_Absyn_Aggrdecl*_tmp1AA=Cyc_Absyn_get_known_aggrdecl(
_tmp1A2);if(_tmp1AA->impl == 0)return 0;{struct Cyc_List_List*_tmp1AB=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1AA->impl))->fields;for(0;_tmp1AB != 0;_tmp1AB=_tmp1AB->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp1AB->hd)->type))
return 0;}}return 1;}_LL94: if(*((int*)_tmp19E)!= 11)goto _LL96;_tmp1A3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19E)->f2;_LL95: for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp1A3->hd)->type))return 0;}return 1;_LL96:
if(*((int*)_tmp19E)!= 3)goto _LL98;_tmp1A4=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp19E)->f1;_tmp1A5=_tmp1A4.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp19E)->f1).field_info).KnownTunionfield).tag != 1)goto _LL98;_tmp1A6=(_tmp1A5.KnownTunionfield).f1;
_tmp1A7=(_tmp1A5.KnownTunionfield).f2;_LL97: _tmp1A8=_tmp1A7->typs;goto _LL99;
_LL98: if(*((int*)_tmp19E)!= 9)goto _LL9A;_tmp1A8=((struct Cyc_Absyn_TupleType_struct*)
_tmp19E)->f1;_LL99: for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp1A8->hd)).f2))return 0;}return 1;_LL9A: if(*((int*)_tmp19E)!= 2)
goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp19E)!= 4)goto _LL9E;_LL9D: goto
_LL9F;_LL9E: if(*((int*)_tmp19E)!= 15)goto _LLA0;_LL9F: return 0;_LLA0:;_LLA1:({
struct Cyc_String_pa_struct _tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp1AC[
1]={& _tmp1AE};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1AD="atomic_typ:  bad type %s";_tag_dynforward(_tmp1AD,sizeof(char),
_get_zero_arr_size(_tmp1AD,25));}),_tag_dynforward(_tmp1AC,sizeof(void*),1));}});
_LL7B:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1AF=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp1B0;void*_tmp1B1;_LLA8: if(_tmp1AF <= (void*)4)goto
_LLAA;if(*((int*)_tmp1AF)!= 4)goto _LLAA;_tmp1B0=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AF)->f1;_tmp1B1=(void*)_tmp1B0.elt_typ;_LLA9: {void*_tmp1B2=Cyc_Tcutil_compress(
_tmp1B1);_LLAD: if((int)_tmp1B2 != 0)goto _LLAF;_LLAE: return 1;_LLAF:;_LLB0: return 0;
_LLAC:;}_LLAA:;_LLAB: return 0;_LLA7:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp1B3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1B4;union Cyc_Absyn_AggrInfoU_union _tmp1B5;struct Cyc_List_List*_tmp1B6;_LLB2:
if(_tmp1B3 <= (void*)4)goto _LLB6;if(*((int*)_tmp1B3)!= 10)goto _LLB4;_tmp1B4=((
struct Cyc_Absyn_AggrType_struct*)_tmp1B3)->f1;_tmp1B5=_tmp1B4.aggr_info;_LLB3: {
struct Cyc_Absyn_Aggrdecl*_tmp1B7=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);if(
_tmp1B7->impl == 0)({void*_tmp1B8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1B9="is_poly_field: type missing fields";
_tag_dynforward(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,35));}),
_tag_dynforward(_tmp1B8,sizeof(void*),0));});_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1B7->impl))->fields;goto _LLB5;}_LLB4: if(*((int*)_tmp1B3)!= 11)
goto _LLB6;_tmp1B6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1B3)->f2;_LLB5: {
struct Cyc_Absyn_Aggrfield*_tmp1BA=Cyc_Absyn_lookup_field(_tmp1B6,f);if(_tmp1BA == 
0)({struct Cyc_String_pa_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp1BB[1]={& _tmp1BD};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1BC="is_poly_field: bad field %s";_tag_dynforward(_tmp1BC,sizeof(char),
_get_zero_arr_size(_tmp1BC,28));}),_tag_dynforward(_tmp1BB,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp1BA->type);}_LLB6:;_LLB7:({struct Cyc_String_pa_struct
_tmp1C0;_tmp1C0.tag=0;_tmp1C0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp1BE[1]={& _tmp1C0};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1BF="is_poly_field: bad type %s";_tag_dynforward(_tmp1BF,sizeof(char),
_get_zero_arr_size(_tmp1BF,27));}),_tag_dynforward(_tmp1BE,sizeof(void*),1));}});
_LLB1:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp1C1=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp1C2;struct _dynforward_ptr*_tmp1C3;struct Cyc_Absyn_Exp*
_tmp1C4;struct _dynforward_ptr*_tmp1C5;_LLB9: if(*((int*)_tmp1C1)!= 23)goto _LLBB;
_tmp1C2=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1C1)->f1;_tmp1C3=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1C1)->f2;_LLBA: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C2->topt))->v,_tmp1C3);_LLBB: if(*((int*)_tmp1C1)!= 24)goto _LLBD;
_tmp1C4=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1C1)->f1;_tmp1C5=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1C1)->f2;_LLBC: {void*_tmp1C6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C4->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1C7;void*_tmp1C8;
_LLC0: if(_tmp1C6 <= (void*)4)goto _LLC2;if(*((int*)_tmp1C6)!= 4)goto _LLC2;_tmp1C7=((
struct Cyc_Absyn_PointerType_struct*)_tmp1C6)->f1;_tmp1C8=(void*)_tmp1C7.elt_typ;
_LLC1: return Cyc_Toc_is_poly_field(_tmp1C8,_tmp1C5);_LLC2:;_LLC3:({struct Cyc_String_pa_struct
_tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C4->topt))->v));{
void*_tmp1C9[1]={& _tmp1CB};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1CA="is_poly_project: bad type %s";
_tag_dynforward(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,29));}),
_tag_dynforward(_tmp1C9,sizeof(void*),1));}});_LLBF:;}_LLBD:;_LLBE: return 0;_LLB8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));
_tmp1CC->hd=s;_tmp1CC->tl=0;_tmp1CC;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=s;_tmp1CD->tl=
0;_tmp1CD;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp1CE[2];_tmp1CE[1]=s;_tmp1CE[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1CE,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp1CF[2];_tmp1CF[1]=n;
_tmp1CF[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1CF,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp1D0[2];_tmp1D0[1]=n;_tmp1D0[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp1D0,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp1D1[3];_tmp1D1[2]=n;_tmp1D1[1]=
s;_tmp1D1[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1D1,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->hd=e;_tmp1D2->tl=
0;_tmp1D2;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp1D3=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1D4;_LLC5: if(*((int*)
_tmp1D3)!= 0)goto _LLC7;_tmp1D4=((struct Cyc_Absyn_Const_e_struct*)_tmp1D3)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp1D3)->f1).String_c).tag != 5)goto _LLC7;
_LLC6: is_string=1;goto _LLC4;_LLC7:;_LLC8: goto _LLC4;_LLC4:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({
struct Cyc_Absyn_Var_d_struct _tmp1D7;_tmp1D7.tag=0;_tmp1D7.f1=vd;_tmp1D7;});
_tmp1D6;}),0);_tmp1D5->tl=Cyc_Toc_result_decls;_tmp1D5;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple8*_tmp1D8[3];_tmp1D8[2]=({struct _tuple8*_tmp1DB=_cycalloc(
sizeof(*_tmp1DB));_tmp1DB->f1=0;_tmp1DB->f2=xplussz;_tmp1DB;});_tmp1D8[1]=({
struct _tuple8*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=0;_tmp1DA->f2=xexp;
_tmp1DA;});_tmp1D8[0]=({struct _tuple8*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));
_tmp1D9->f1=0;_tmp1D9->f2=xexp;_tmp1D9;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1D8,sizeof(struct _tuple8*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple8*_tmp1DC[2];_tmp1DC[
1]=({struct _tuple8*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->f1=0;_tmp1DE->f2=
xplussz;_tmp1DE;});_tmp1DC[0]=({struct _tuple8*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));
_tmp1DD->f1=0;_tmp1DD->f2=xexp;_tmp1DD;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1DC,sizeof(struct _tuple8*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp1E0;int _tmp1E1;struct Cyc_Toc_Env*
_tmp1DF=nv;_tmp1E0=*_tmp1DF;_tmp1E1=_tmp1E0.toplevel;return _tmp1E1;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp1E3;struct Cyc_Dict_Dict _tmp1E4;struct Cyc_Toc_Env*_tmp1E2=nv;
_tmp1E3=*_tmp1E2;_tmp1E4=_tmp1E3.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1E4,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp1E5=
_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=(struct _dynforward_ptr**)0;
_tmp1E5->continue_lab=(struct _dynforward_ptr**)0;_tmp1E5->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp1E5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp1E5->toplevel=(int)1;_tmp1E5;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1E7;struct _dynforward_ptr**_tmp1E8;struct _dynforward_ptr**_tmp1E9;struct Cyc_Toc_FallthruInfo*
_tmp1EA;struct Cyc_Dict_Dict _tmp1EB;int _tmp1EC;struct Cyc_Toc_Env*_tmp1E6=e;
_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E7.break_lab;_tmp1E9=_tmp1E7.continue_lab;_tmp1EA=
_tmp1E7.fallthru_info;_tmp1EB=_tmp1E7.varmap;_tmp1EC=_tmp1E7.toplevel;return({
struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1E8);_tmp1ED->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1E9);_tmp1ED->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1EA;_tmp1ED->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1EB);_tmp1ED->toplevel=(int)_tmp1EC;_tmp1ED;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1EF;struct _dynforward_ptr**_tmp1F0;struct _dynforward_ptr**_tmp1F1;struct Cyc_Toc_FallthruInfo*
_tmp1F2;struct Cyc_Dict_Dict _tmp1F3;int _tmp1F4;struct Cyc_Toc_Env*_tmp1EE=e;
_tmp1EF=*_tmp1EE;_tmp1F0=_tmp1EF.break_lab;_tmp1F1=_tmp1EF.continue_lab;_tmp1F2=
_tmp1EF.fallthru_info;_tmp1F3=_tmp1EF.varmap;_tmp1F4=_tmp1EF.toplevel;return({
struct Cyc_Toc_Env*_tmp1F5=_region_malloc(r,sizeof(*_tmp1F5));_tmp1F5->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1F0);_tmp1F5->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1F1);_tmp1F5->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1F2;_tmp1F5->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1F3);_tmp1F5->toplevel=(int)0;_tmp1F5;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1F7;struct
_dynforward_ptr**_tmp1F8;struct _dynforward_ptr**_tmp1F9;struct Cyc_Toc_FallthruInfo*
_tmp1FA;struct Cyc_Dict_Dict _tmp1FB;int _tmp1FC;struct Cyc_Toc_Env*_tmp1F6=e;
_tmp1F7=*_tmp1F6;_tmp1F8=_tmp1F7.break_lab;_tmp1F9=_tmp1F7.continue_lab;_tmp1FA=
_tmp1F7.fallthru_info;_tmp1FB=_tmp1F7.varmap;_tmp1FC=_tmp1F7.toplevel;return({
struct Cyc_Toc_Env*_tmp1FD=_region_malloc(r,sizeof(*_tmp1FD));_tmp1FD->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1F8);_tmp1FD->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1F9);_tmp1FD->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1FA;_tmp1FD->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1FB);_tmp1FD->toplevel=(int)1;_tmp1FD;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp1FE=(*x).f1;_LLCA: if((_tmp1FE.Rel_n).tag != 1)
goto _LLCC;_LLCB:({struct Cyc_String_pa_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp1FF[1]={& _tmp201};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp200="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,30));}),
_tag_dynforward(_tmp1FF,sizeof(void*),1));}});_LLCC:;_LLCD: goto _LLC9;_LLC9:;}{
struct Cyc_Toc_Env _tmp203;struct _dynforward_ptr**_tmp204;struct _dynforward_ptr**
_tmp205;struct Cyc_Toc_FallthruInfo*_tmp206;struct Cyc_Dict_Dict _tmp207;int _tmp208;
struct Cyc_Toc_Env*_tmp202=e;_tmp203=*_tmp202;_tmp204=_tmp203.break_lab;_tmp205=
_tmp203.continue_lab;_tmp206=_tmp203.fallthru_info;_tmp207=_tmp203.varmap;
_tmp208=_tmp203.toplevel;{struct Cyc_Dict_Dict _tmp209=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp207),x,y);return({struct Cyc_Toc_Env*_tmp20A=_region_malloc(r,sizeof(*
_tmp20A));_tmp20A->break_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp204);_tmp20A->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp205);_tmp20A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp206;_tmp20A->varmap=(
struct Cyc_Dict_Dict)_tmp209;_tmp20A->toplevel=(int)_tmp208;_tmp20A;});}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp20C;struct _dynforward_ptr**_tmp20D;struct _dynforward_ptr**
_tmp20E;struct Cyc_Toc_FallthruInfo*_tmp20F;struct Cyc_Dict_Dict _tmp210;int _tmp211;
struct Cyc_Toc_Env*_tmp20B=e;_tmp20C=*_tmp20B;_tmp20D=_tmp20C.break_lab;_tmp20E=
_tmp20C.continue_lab;_tmp20F=_tmp20C.fallthru_info;_tmp210=_tmp20C.varmap;
_tmp211=_tmp20C.toplevel;return({struct Cyc_Toc_Env*_tmp212=_region_malloc(r,
sizeof(*_tmp212));_tmp212->break_lab=(struct _dynforward_ptr**)0;_tmp212->continue_lab=(
struct _dynforward_ptr**)0;_tmp212->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp20F;_tmp212->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp210);_tmp212->toplevel=(
int)_tmp211;_tmp212;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct
_dynforward_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp213=_region_malloc(r,sizeof(*_tmp213));_tmp213->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp213->tl=fallthru_vars;_tmp213;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp215;struct _dynforward_ptr**_tmp216;struct _dynforward_ptr**
_tmp217;struct Cyc_Toc_FallthruInfo*_tmp218;struct Cyc_Dict_Dict _tmp219;int _tmp21A;
struct Cyc_Toc_Env*_tmp214=e;_tmp215=*_tmp214;_tmp216=_tmp215.break_lab;_tmp217=
_tmp215.continue_lab;_tmp218=_tmp215.fallthru_info;_tmp219=_tmp215.varmap;
_tmp21A=_tmp215.toplevel;{struct Cyc_Toc_Env _tmp21C;struct Cyc_Dict_Dict _tmp21D;
struct Cyc_Toc_Env*_tmp21B=next_case_env;_tmp21C=*_tmp21B;_tmp21D=_tmp21C.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp220=
_region_malloc(r,sizeof(*_tmp220));_tmp220->label=fallthru_l;_tmp220->binders=
fallthru_vars;_tmp220->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp21D);_tmp220;});
return({struct Cyc_Toc_Env*_tmp21E=_region_malloc(r,sizeof(*_tmp21E));_tmp21E->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp21F=_region_malloc(r,
sizeof(*_tmp21F));_tmp21F[0]=break_l;_tmp21F;});_tmp21E->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp217);_tmp21E->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp21E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp219);_tmp21E->toplevel=(
int)_tmp21A;_tmp21E;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp222;struct _dynforward_ptr**_tmp223;struct _dynforward_ptr**_tmp224;
struct Cyc_Toc_FallthruInfo*_tmp225;struct Cyc_Dict_Dict _tmp226;int _tmp227;struct
Cyc_Toc_Env*_tmp221=e;_tmp222=*_tmp221;_tmp223=_tmp222.break_lab;_tmp224=_tmp222.continue_lab;
_tmp225=_tmp222.fallthru_info;_tmp226=_tmp222.varmap;_tmp227=_tmp222.toplevel;
return({struct Cyc_Toc_Env*_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp229=_region_malloc(r,
sizeof(*_tmp229));_tmp229[0]=break_l;_tmp229;});_tmp228->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp224);_tmp228->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp228->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp226);_tmp228->toplevel=(
int)_tmp227;_tmp228;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp22B;struct _dynforward_ptr**_tmp22C;struct _dynforward_ptr**_tmp22D;
struct Cyc_Toc_FallthruInfo*_tmp22E;struct Cyc_Dict_Dict _tmp22F;int _tmp230;struct
Cyc_Toc_Env*_tmp22A=e;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.break_lab;_tmp22D=_tmp22B.continue_lab;
_tmp22E=_tmp22B.fallthru_info;_tmp22F=_tmp22B.varmap;_tmp230=_tmp22B.toplevel;
return({struct Cyc_Toc_Env*_tmp231=_region_malloc(r,sizeof(*_tmp231));_tmp231->break_lab=(
struct _dynforward_ptr**)0;_tmp231->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp22D);_tmp231->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp232=_region_malloc(r,sizeof(*_tmp232));_tmp232->label=
next_l;_tmp232->binders=0;_tmp232->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp232;});_tmp231->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22F);_tmp231->toplevel=(
int)_tmp230;_tmp231;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp233=(void*)e->annot;
_LLCF: if(*((void**)_tmp233)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD1;_LLD0: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD1: if(*((void**)_tmp233)
!= Cyc_CfFlowInfo_NotZero)goto _LLD3;_LLD2: return 0;_LLD3: if(_tmp233 != Cyc_CfFlowInfo_IsZero)
goto _LLD5;_LLD4:({void*_tmp234[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp235="dereference of NULL pointer";
_tag_dynforward(_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,28));}),
_tag_dynforward(_tmp234,sizeof(void*),0));});return 0;_LLD5: if(_tmp233 != Cyc_Absyn_EmptyAnnot)
goto _LLD7;_LLD6: return 0;_LLD7: if(*((void**)_tmp233)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD9;_LLD8:({void*_tmp236[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp237="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,46));}),
_tag_dynforward(_tmp236,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD9:;_LLDA:({void*_tmp238[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp239="need_null_check";_tag_dynforward(_tmp239,sizeof(char),
_get_zero_arr_size(_tmp239,16));}),_tag_dynforward(_tmp238,sizeof(void*),0));});
_LLCE:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp23A=(void*)e->annot;struct Cyc_List_List*_tmp23B;struct Cyc_List_List*_tmp23C;
_LLDC: if(*((void**)_tmp23A)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDE;_tmp23B=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp23A)->f1;_LLDD: return _tmp23B;_LLDE: if(*((
void**)_tmp23A)!= Cyc_CfFlowInfo_NotZero)goto _LLE0;_tmp23C=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp23A)->f1;_LLDF: return _tmp23C;_LLE0: if(_tmp23A != Cyc_CfFlowInfo_IsZero)goto
_LLE2;_LLE1:({void*_tmp23D[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp23E="dereference of NULL pointer";
_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,28));}),
_tag_dynforward(_tmp23D,sizeof(void*),0));});return 0;_LLE2: if(*((void**)_tmp23A)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp23A != Cyc_Absyn_EmptyAnnot)
goto _LLE6;_LLE5: return 0;_LLE6:;_LLE7:({void*_tmp23F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp240="get_relns";_tag_dynforward(_tmp240,sizeof(char),_get_zero_arr_size(
_tmp240,10));}),_tag_dynforward(_tmp23F,sizeof(void*),0));});_LLDB:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp241=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp242;struct Cyc_Absyn_PtrAtts _tmp243;struct Cyc_Absyn_Conref*
_tmp244;struct Cyc_Absyn_Conref*_tmp245;struct Cyc_Absyn_ArrayInfo _tmp246;struct
Cyc_Absyn_Exp*_tmp247;_LLE9: if(_tmp241 <= (void*)4)goto _LLED;if(*((int*)_tmp241)
!= 4)goto _LLEB;_tmp242=((struct Cyc_Absyn_PointerType_struct*)_tmp241)->f1;
_tmp243=_tmp242.ptr_atts;_tmp244=_tmp243.bounds;_tmp245=_tmp243.zero_term;_LLEA: {
void*_tmp248=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp244);struct Cyc_Absyn_Exp*
_tmp249;_LLF0: if((int)_tmp248 != 0)goto _LLF2;_LLF1: return 0;_LLF2: if((int)_tmp248
!= 1)goto _LLF4;_LLF3: return 0;_LLF4: if(_tmp248 <= (void*)2)goto _LLF6;if(*((int*)
_tmp248)!= 0)goto _LLF6;_tmp249=((struct Cyc_Absyn_Upper_b_struct*)_tmp248)->f1;
_LLF5: {unsigned int _tmp24B;int _tmp24C;struct _tuple5 _tmp24A=Cyc_Evexp_eval_const_uint_exp(
_tmp249);_tmp24B=_tmp24A.f1;_tmp24C=_tmp24A.f2;return _tmp24C  && i <= _tmp24B;}
_LLF6: if(_tmp248 <= (void*)2)goto _LLEF;if(*((int*)_tmp248)!= 1)goto _LLEF;_LLF7:({
void*_tmp24D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp24E="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,30));}),
_tag_dynforward(_tmp24D,sizeof(void*),0));});_LLEF:;}_LLEB: if(*((int*)_tmp241)!= 
7)goto _LLED;_tmp246=((struct Cyc_Absyn_ArrayType_struct*)_tmp241)->f1;_tmp247=
_tmp246.num_elts;_LLEC: if(_tmp247 == 0)return 0;{unsigned int _tmp250;int _tmp251;
struct _tuple5 _tmp24F=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp247);
_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;return _tmp251  && i <= _tmp250;}_LLED:;_LLEE:
return 0;_LLE8:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp252=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp252->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp253=_tmp252->rop;
struct Cyc_Absyn_Vardecl*_tmp254;struct Cyc_Absyn_Vardecl*_tmp255;_LLF9: if((
_tmp253.LessSize).tag != 2)goto _LLFB;_tmp254=(_tmp253.LessSize).f1;_LLFA: _tmp255=
_tmp254;goto _LLFC;_LLFB: if((_tmp253.LessEqSize).tag != 4)goto _LLFD;_tmp255=(
_tmp253.LessEqSize).f1;_LLFC: if(_tmp255 == v)return 1;else{goto _LLF8;}_LLFD:;_LLFE:
continue;_LLF8:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp256=(void*)e->r;
void*_tmp257;struct Cyc_Absyn_Vardecl*_tmp258;void*_tmp259;struct Cyc_Absyn_Vardecl*
_tmp25A;void*_tmp25B;struct Cyc_Absyn_Vardecl*_tmp25C;void*_tmp25D;struct Cyc_Absyn_Vardecl*
_tmp25E;void*_tmp25F;struct Cyc_List_List*_tmp260;struct Cyc_List_List _tmp261;
struct Cyc_Absyn_Exp*_tmp262;_LL100: if(*((int*)_tmp256)!= 1)goto _LL102;_tmp257=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp256)->f2;if(_tmp257 <= (void*)1)goto
_LL102;if(*((int*)_tmp257)!= 4)goto _LL102;_tmp258=((struct Cyc_Absyn_Pat_b_struct*)
_tmp257)->f1;_LL101: _tmp25A=_tmp258;goto _LL103;_LL102: if(*((int*)_tmp256)!= 1)
goto _LL104;_tmp259=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp256)->f2;if(
_tmp259 <= (void*)1)goto _LL104;if(*((int*)_tmp259)!= 3)goto _LL104;_tmp25A=((
struct Cyc_Absyn_Local_b_struct*)_tmp259)->f1;_LL103: _tmp25C=_tmp25A;goto _LL105;
_LL104: if(*((int*)_tmp256)!= 1)goto _LL106;_tmp25B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp256)->f2;if(_tmp25B <= (void*)1)goto _LL106;if(*((int*)_tmp25B)!= 0)goto _LL106;
_tmp25C=((struct Cyc_Absyn_Global_b_struct*)_tmp25B)->f1;_LL105: _tmp25E=_tmp25C;
goto _LL107;_LL106: if(*((int*)_tmp256)!= 1)goto _LL108;_tmp25D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp256)->f2;if(_tmp25D <= (void*)1)goto _LL108;if(*((int*)_tmp25D)!= 2)goto _LL108;
_tmp25E=((struct Cyc_Absyn_Param_b_struct*)_tmp25D)->f1;_LL107: if(_tmp25E->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp25E))return 1;goto _LLFF;_LL108:
if(*((int*)_tmp256)!= 3)goto _LL10A;_tmp25F=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp256)->f1;if((int)_tmp25F != 19)goto _LL10A;_tmp260=((struct Cyc_Absyn_Primop_e_struct*)
_tmp256)->f2;if(_tmp260 == 0)goto _LL10A;_tmp261=*_tmp260;_tmp262=(struct Cyc_Absyn_Exp*)
_tmp261.hd;_LL109:{void*_tmp263=(void*)_tmp262->r;void*_tmp264;struct Cyc_Absyn_Vardecl*
_tmp265;void*_tmp266;struct Cyc_Absyn_Vardecl*_tmp267;void*_tmp268;struct Cyc_Absyn_Vardecl*
_tmp269;void*_tmp26A;struct Cyc_Absyn_Vardecl*_tmp26B;_LL10D: if(*((int*)_tmp263)
!= 1)goto _LL10F;_tmp264=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;if(
_tmp264 <= (void*)1)goto _LL10F;if(*((int*)_tmp264)!= 4)goto _LL10F;_tmp265=((
struct Cyc_Absyn_Pat_b_struct*)_tmp264)->f1;_LL10E: _tmp267=_tmp265;goto _LL110;
_LL10F: if(*((int*)_tmp263)!= 1)goto _LL111;_tmp266=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp266 <= (void*)1)goto _LL111;if(*((int*)_tmp266)!= 3)goto _LL111;
_tmp267=((struct Cyc_Absyn_Local_b_struct*)_tmp266)->f1;_LL110: _tmp269=_tmp267;
goto _LL112;_LL111: if(*((int*)_tmp263)!= 1)goto _LL113;_tmp268=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp268 <= (void*)1)goto _LL113;if(*((int*)_tmp268)!= 0)goto _LL113;
_tmp269=((struct Cyc_Absyn_Global_b_struct*)_tmp268)->f1;_LL112: _tmp26B=_tmp269;
goto _LL114;_LL113: if(*((int*)_tmp263)!= 1)goto _LL115;_tmp26A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp26A <= (void*)1)goto _LL115;if(*((int*)_tmp26A)!= 2)goto _LL115;
_tmp26B=((struct Cyc_Absyn_Param_b_struct*)_tmp26A)->f1;_LL114: return _tmp26B == v;
_LL115:;_LL116: goto _LL10C;_LL10C:;}goto _LLFF;_LL10A:;_LL10B: goto _LLFF;_LLFF:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp26C=(void*)a->r;void*_tmp26D;struct Cyc_Absyn_Vardecl*
_tmp26E;void*_tmp26F;struct Cyc_Absyn_Vardecl*_tmp270;void*_tmp271;struct Cyc_Absyn_Vardecl*
_tmp272;void*_tmp273;struct Cyc_Absyn_Vardecl*_tmp274;_LL118: if(*((int*)_tmp26C)
!= 1)goto _LL11A;_tmp26D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp26C)->f2;if(
_tmp26D <= (void*)1)goto _LL11A;if(*((int*)_tmp26D)!= 4)goto _LL11A;_tmp26E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp26D)->f1;_LL119: _tmp270=_tmp26E;goto _LL11B;
_LL11A: if(*((int*)_tmp26C)!= 1)goto _LL11C;_tmp26F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp26F <= (void*)1)goto _LL11C;if(*((int*)_tmp26F)!= 3)goto _LL11C;
_tmp270=((struct Cyc_Absyn_Local_b_struct*)_tmp26F)->f1;_LL11B: _tmp272=_tmp270;
goto _LL11D;_LL11C: if(*((int*)_tmp26C)!= 1)goto _LL11E;_tmp271=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp271 <= (void*)1)goto _LL11E;if(*((int*)_tmp271)!= 0)goto _LL11E;
_tmp272=((struct Cyc_Absyn_Global_b_struct*)_tmp271)->f1;_LL11D: _tmp274=_tmp272;
goto _LL11F;_LL11E: if(*((int*)_tmp26C)!= 1)goto _LL120;_tmp273=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp273 <= (void*)1)goto _LL120;if(*((int*)_tmp273)!= 2)goto _LL120;
_tmp274=((struct Cyc_Absyn_Param_b_struct*)_tmp273)->f1;_LL11F: if(_tmp274->escapes)
return 0;inner_loop: {void*_tmp275=(void*)i->r;void*_tmp276;struct Cyc_Absyn_Exp*
_tmp277;union Cyc_Absyn_Cnst_union _tmp278;void*_tmp279;int _tmp27A;union Cyc_Absyn_Cnst_union
_tmp27B;void*_tmp27C;int _tmp27D;union Cyc_Absyn_Cnst_union _tmp27E;void*_tmp27F;
int _tmp280;void*_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_List_List _tmp283;
struct Cyc_Absyn_Exp*_tmp284;struct Cyc_List_List*_tmp285;struct Cyc_List_List
_tmp286;struct Cyc_Absyn_Exp*_tmp287;void*_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;
void*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Vardecl*
_tmp28D;void*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;_LL123: if(*((int*)_tmp275)
!= 15)goto _LL125;_tmp276=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp275)->f1;
_tmp277=((struct Cyc_Absyn_Cast_e_struct*)_tmp275)->f2;_LL124: i=_tmp277;goto
inner_loop;_LL125: if(*((int*)_tmp275)!= 0)goto _LL127;_tmp278=((struct Cyc_Absyn_Const_e_struct*)
_tmp275)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp275)->f1).Int_c).tag != 2)
goto _LL127;_tmp279=(_tmp278.Int_c).f1;if((int)_tmp279 != 2)goto _LL127;_tmp27A=(
_tmp278.Int_c).f2;_LL126: _tmp27D=_tmp27A;goto _LL128;_LL127: if(*((int*)_tmp275)!= 
0)goto _LL129;_tmp27B=((struct Cyc_Absyn_Const_e_struct*)_tmp275)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp275)->f1).Int_c).tag != 2)goto _LL129;_tmp27C=(
_tmp27B.Int_c).f1;if((int)_tmp27C != 0)goto _LL129;_tmp27D=(_tmp27B.Int_c).f2;
_LL128: return _tmp27D >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp27D + 1),(
void*)_tmp274->type);_LL129: if(*((int*)_tmp275)!= 0)goto _LL12B;_tmp27E=((struct
Cyc_Absyn_Const_e_struct*)_tmp275)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp275)->f1).Int_c).tag != 2)goto _LL12B;_tmp27F=(_tmp27E.Int_c).f1;if((int)
_tmp27F != 1)goto _LL12B;_tmp280=(_tmp27E.Int_c).f2;_LL12A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp280 + 1),(void*)_tmp274->type);_LL12B: if(*((int*)_tmp275)!= 3)
goto _LL12D;_tmp281=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp275)->f1;if((
int)_tmp281 != 4)goto _LL12D;_tmp282=((struct Cyc_Absyn_Primop_e_struct*)_tmp275)->f2;
if(_tmp282 == 0)goto _LL12D;_tmp283=*_tmp282;_tmp284=(struct Cyc_Absyn_Exp*)_tmp283.hd;
_tmp285=_tmp283.tl;if(_tmp285 == 0)goto _LL12D;_tmp286=*_tmp285;_tmp287=(struct Cyc_Absyn_Exp*)
_tmp286.hd;_LL12C: return Cyc_Toc_check_leq_size(relns,_tmp274,_tmp287);_LL12D: if(*((
int*)_tmp275)!= 1)goto _LL12F;_tmp288=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp288 <= (void*)1)goto _LL12F;if(*((int*)_tmp288)!= 4)goto _LL12F;
_tmp289=((struct Cyc_Absyn_Pat_b_struct*)_tmp288)->f1;_LL12E: _tmp28B=_tmp289;goto
_LL130;_LL12F: if(*((int*)_tmp275)!= 1)goto _LL131;_tmp28A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp28A <= (void*)1)goto _LL131;if(*((int*)_tmp28A)!= 3)goto _LL131;
_tmp28B=((struct Cyc_Absyn_Local_b_struct*)_tmp28A)->f1;_LL130: _tmp28D=_tmp28B;
goto _LL132;_LL131: if(*((int*)_tmp275)!= 1)goto _LL133;_tmp28C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp28C <= (void*)1)goto _LL133;if(*((int*)_tmp28C)!= 0)goto _LL133;
_tmp28D=((struct Cyc_Absyn_Global_b_struct*)_tmp28C)->f1;_LL132: _tmp28F=_tmp28D;
goto _LL134;_LL133: if(*((int*)_tmp275)!= 1)goto _LL135;_tmp28E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp28E <= (void*)1)goto _LL135;if(*((int*)_tmp28E)!= 2)goto _LL135;
_tmp28F=((struct Cyc_Absyn_Param_b_struct*)_tmp28E)->f1;_LL134: if(_tmp28F->escapes)
return 0;{struct Cyc_List_List*_tmp290=relns;for(0;_tmp290 != 0;_tmp290=_tmp290->tl){
struct Cyc_CfFlowInfo_Reln*_tmp291=(struct Cyc_CfFlowInfo_Reln*)_tmp290->hd;if(
_tmp291->vd == _tmp28F){union Cyc_CfFlowInfo_RelnOp_union _tmp292=_tmp291->rop;
struct Cyc_Absyn_Vardecl*_tmp293;struct Cyc_Absyn_Vardecl*_tmp294;unsigned int
_tmp295;_LL138: if((_tmp292.LessSize).tag != 2)goto _LL13A;_tmp293=(_tmp292.LessSize).f1;
_LL139: if(_tmp274 == _tmp293)return 1;else{goto _LL137;}_LL13A: if((_tmp292.LessVar).tag
!= 1)goto _LL13C;_tmp294=(_tmp292.LessVar).f1;_LL13B:{struct Cyc_List_List*_tmp296=
relns;for(0;_tmp296 != 0;_tmp296=_tmp296->tl){struct Cyc_CfFlowInfo_Reln*_tmp297=(
struct Cyc_CfFlowInfo_Reln*)_tmp296->hd;if(_tmp297->vd == _tmp294){union Cyc_CfFlowInfo_RelnOp_union
_tmp298=_tmp297->rop;struct Cyc_Absyn_Vardecl*_tmp299;struct Cyc_Absyn_Vardecl*
_tmp29A;unsigned int _tmp29B;struct Cyc_Absyn_Vardecl*_tmp29C;_LL141: if((_tmp298.LessEqSize).tag
!= 4)goto _LL143;_tmp299=(_tmp298.LessEqSize).f1;_LL142: _tmp29A=_tmp299;goto
_LL144;_LL143: if((_tmp298.LessSize).tag != 2)goto _LL145;_tmp29A=(_tmp298.LessSize).f1;
_LL144: if(_tmp274 == _tmp29A)return 1;goto _LL140;_LL145: if((_tmp298.EqualConst).tag
!= 0)goto _LL147;_tmp29B=(_tmp298.EqualConst).f1;_LL146: return Cyc_Toc_check_const_array(
_tmp29B,(void*)_tmp274->type);_LL147: if((_tmp298.LessVar).tag != 1)goto _LL149;
_tmp29C=(_tmp298.LessVar).f1;_LL148: if(Cyc_Toc_check_leq_size_var(relns,_tmp274,
_tmp29C))return 1;goto _LL140;_LL149:;_LL14A: goto _LL140;_LL140:;}}}goto _LL137;
_LL13C: if((_tmp292.LessConst).tag != 3)goto _LL13E;_tmp295=(_tmp292.LessConst).f1;
_LL13D: return Cyc_Toc_check_const_array(_tmp295,(void*)_tmp274->type);_LL13E:;
_LL13F: goto _LL137;_LL137:;}}}goto _LL122;_LL135:;_LL136: goto _LL122;_LL122:;}goto
_LL117;_LL120:;_LL121: goto _LL117;_LL117:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp29D[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp29E="Missing type in primop ";_tag_dynforward(_tmp29E,sizeof(char),
_get_zero_arr_size(_tmp29E,24));}),_tag_dynforward(_tmp29D,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp29F[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2A0="Missing type in primop ";_tag_dynforward(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,24));}),_tag_dynforward(_tmp29F,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp2A1=_cycalloc(
sizeof(*_tmp2A1));_tmp2A1->f1=Cyc_Toc_mt_tq;_tmp2A1->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp2A1;});}static struct _tuple8*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple8*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->f1=0;
_tmp2A2->f2=e;_tmp2A2;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp2A3=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp2A3): Cyc_Toc_malloc_ptr(_tmp2A3));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp2A3);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp2A4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp2A4 != 0;_tmp2A4=_tmp2A4->tl){struct _tuple8 _tmp2A6;struct Cyc_List_List*
_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A8;struct _tuple8*_tmp2A5=(struct _tuple8*)
_tmp2A4->hd;_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp2A7 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp2A7->tl != 0)({void*_tmp2A9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2AA="multiple designators in array";
_tag_dynforward(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,30));}),
_tag_dynforward(_tmp2A9,sizeof(void*),0));});{void*_tmp2AB=(void*)_tmp2A7->hd;
void*_tmp2AC=_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AD;_LL14C: if(*((int*)_tmp2AC)!= 0)
goto _LL14E;_tmp2AD=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2AC)->f1;_LL14D:
Cyc_Toc_exp_to_c(nv,_tmp2AD);e_index=_tmp2AD;goto _LL14B;_LL14E: if(*((int*)
_tmp2AC)!= 1)goto _LL14B;_LL14F:({void*_tmp2AE[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2AF="field name designators in array";
_tag_dynforward(_tmp2AF,sizeof(char),_get_zero_arr_size(_tmp2AF,32));}),
_tag_dynforward(_tmp2AE,sizeof(void*),0));});_LL14B:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp2B0=(void*)_tmp2A8->r;struct Cyc_List_List*
_tmp2B1;struct Cyc_Absyn_Vardecl*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*
_tmp2B4;int _tmp2B5;void*_tmp2B6;struct Cyc_List_List*_tmp2B7;_LL151: if(*((int*)
_tmp2B0)!= 28)goto _LL153;_tmp2B1=((struct Cyc_Absyn_Array_e_struct*)_tmp2B0)->f1;
_LL152: s=Cyc_Toc_init_array(nv,lval,_tmp2B1,s);goto _LL150;_LL153: if(*((int*)
_tmp2B0)!= 29)goto _LL155;_tmp2B2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2B0)->f1;_tmp2B3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B0)->f2;
_tmp2B4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B0)->f3;_tmp2B5=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2B0)->f4;_LL154: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2B2,_tmp2B3,_tmp2B4,_tmp2B5,s,0);goto _LL150;_LL155: if(*((int*)
_tmp2B0)!= 31)goto _LL157;_tmp2B6=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2B0)->f1;_tmp2B7=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2B0)->f2;_LL156:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2B6,_tmp2B7,s);goto _LL150;_LL157:;_LL158:
Cyc_Toc_exp_to_c(nv,_tmp2A8);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp2A8,0),s,0);goto _LL150;_LL150:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp2B8=vd->name;
void*_tmp2B9=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp2BA=
_new_region("r2");struct _RegionHandle*r2=& _tmp2BA;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp2B8,Cyc_Absyn_varb_exp(_tmp2B8,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2C5;_tmp2C5.tag=3;_tmp2C5.f1=vd;_tmp2C5;});_tmp2C4;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp2B8,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2B8,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2B8,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2B8,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp2BB=(void*)e2->r;struct Cyc_List_List*
_tmp2BC;struct Cyc_Absyn_Vardecl*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*
_tmp2BF;int _tmp2C0;void*_tmp2C1;struct Cyc_List_List*_tmp2C2;_LL15A: if(*((int*)
_tmp2BB)!= 28)goto _LL15C;_tmp2BC=((struct Cyc_Absyn_Array_e_struct*)_tmp2BB)->f1;
_LL15B: body=Cyc_Toc_init_array(nv2,lval,_tmp2BC,Cyc_Toc_skip_stmt_dl());goto
_LL159;_LL15C: if(*((int*)_tmp2BB)!= 29)goto _LL15E;_tmp2BD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2BB)->f1;_tmp2BE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2BB)->f2;
_tmp2BF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2BB)->f3;_tmp2C0=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2BB)->f4;_LL15D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp2BD,_tmp2BE,_tmp2BF,_tmp2C0,Cyc_Toc_skip_stmt_dl(),0);goto _LL159;
_LL15E: if(*((int*)_tmp2BB)!= 31)goto _LL160;_tmp2C1=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2BB)->f1;_tmp2C2=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2BB)->f2;_LL15F:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp2C1,_tmp2C2,Cyc_Toc_skip_stmt_dl());
goto _LL159;_LL160:;_LL161: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL159;_LL159:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp2B9,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp2C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp2B8,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp2C3;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp2C6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp2C6 != 0;_tmp2C6=_tmp2C6->tl){struct _tuple8 _tmp2C8;
struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;struct _tuple8*_tmp2C7=(
struct _tuple8*)_tmp2C6->hd;_tmp2C8=*_tmp2C7;_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;
if(_tmp2C9 == 0)({void*_tmp2CB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2CC="empty designator list";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,22));}),
_tag_dynforward(_tmp2CB,sizeof(void*),0));});if(_tmp2C9->tl != 0)({void*_tmp2CD[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2CE="too many designators in anonymous struct";_tag_dynforward(
_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,41));}),_tag_dynforward(_tmp2CD,
sizeof(void*),0));});{void*_tmp2CF=(void*)_tmp2C9->hd;struct _dynforward_ptr*
_tmp2D0;_LL163: if(*((int*)_tmp2CF)!= 1)goto _LL165;_tmp2D0=((struct Cyc_Absyn_FieldName_struct*)
_tmp2CF)->f1;_LL164: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp2D0,0);{void*_tmp2D1=(void*)_tmp2CA->r;struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Vardecl*
_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D5;int _tmp2D6;void*
_tmp2D7;struct Cyc_List_List*_tmp2D8;_LL168: if(*((int*)_tmp2D1)!= 28)goto _LL16A;
_tmp2D2=((struct Cyc_Absyn_Array_e_struct*)_tmp2D1)->f1;_LL169: s=Cyc_Toc_init_array(
nv,lval,_tmp2D2,s);goto _LL167;_LL16A: if(*((int*)_tmp2D1)!= 29)goto _LL16C;_tmp2D3=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D1)->f1;_tmp2D4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2D1)->f2;_tmp2D5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D1)->f3;
_tmp2D6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D1)->f4;_LL16B: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2D3,_tmp2D4,_tmp2D5,_tmp2D6,s,0);goto _LL167;_LL16C: if(*((int*)
_tmp2D1)!= 31)goto _LL16E;_tmp2D7=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2D1)->f1;_tmp2D8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2D1)->f2;_LL16D:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2D7,_tmp2D8,s);goto _LL167;_LL16E:;_LL16F:
Cyc_Toc_exp_to_c(nv,_tmp2CA);if(Cyc_Toc_is_poly_field(struct_type,_tmp2D0))
_tmp2CA=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp2CA);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp2CA,0),0),s,0);goto _LL167;
_LL167:;}goto _LL162;}_LL165:;_LL166:({void*_tmp2D9[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2DA="array designator in struct";_tag_dynforward(_tmp2DA,sizeof(char),
_get_zero_arr_size(_tmp2DA,27));}),_tag_dynforward(_tmp2D9,sizeof(void*),0));});
_LL162:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp2DB=_new_region("r");struct _RegionHandle*r=& _tmp2DB;
_push_region(r);{struct Cyc_List_List*_tmp2DC=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp2DD=Cyc_Toc_add_tuple_type(
_tmp2DC);struct _tuple1*_tmp2DE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2DF=
Cyc_Absyn_var_exp(_tmp2DE,0);struct Cyc_Absyn_Stmt*_tmp2E0=Cyc_Absyn_exp_stmt(
_tmp2DF,0);struct Cyc_Absyn_Exp*(*_tmp2E1)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp2E2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2E2);for(0;_tmp2E2 != 0;(_tmp2E2=
_tmp2E2->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2E2->hd;struct
Cyc_Absyn_Exp*lval=_tmp2E1(_tmp2DF,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2E3=(void*)e->r;struct Cyc_List_List*_tmp2E4;struct Cyc_Absyn_Vardecl*
_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E7;int _tmp2E8;
_LL171: if(*((int*)_tmp2E3)!= 28)goto _LL173;_tmp2E4=((struct Cyc_Absyn_Array_e_struct*)
_tmp2E3)->f1;_LL172: _tmp2E0=Cyc_Toc_init_array(nv,lval,_tmp2E4,_tmp2E0);goto
_LL170;_LL173: if(*((int*)_tmp2E3)!= 29)goto _LL175;_tmp2E5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2E3)->f1;_tmp2E6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2E3)->f2;
_tmp2E7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2E3)->f3;_tmp2E8=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2E3)->f4;_LL174: _tmp2E0=Cyc_Toc_init_comprehension(
nv,lval,_tmp2E5,_tmp2E6,_tmp2E7,_tmp2E8,_tmp2E0,0);goto _LL170;_LL175:;_LL176: Cyc_Toc_exp_to_c(
nv,e);_tmp2E0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2E1(
_tmp2DF,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp2E0,0);goto _LL170;_LL170:;}}}{
struct Cyc_Absyn_Exp*_tmp2E9=Cyc_Toc_make_struct(nv,_tmp2DE,_tmp2DD,_tmp2E0,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp2E9;}};_pop_region(r);}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*
struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple1*tdn){struct _tuple1*_tmp2EA=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp2EB=Cyc_Absyn_var_exp(_tmp2EA,0);struct Cyc_Absyn_Stmt*_tmp2EC=Cyc_Absyn_exp_stmt(
_tmp2EB,0);struct Cyc_Absyn_Exp*(*_tmp2ED)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp2EE=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp2EF=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp2F0;union Cyc_Absyn_AggrInfoU_union
_tmp2F1;_LL178: if(_tmp2EF <= (void*)4)goto _LL17A;if(*((int*)_tmp2EF)!= 10)goto
_LL17A;_tmp2F0=((struct Cyc_Absyn_AggrType_struct*)_tmp2EF)->f1;_tmp2F1=_tmp2F0.aggr_info;
_LL179: ad=Cyc_Absyn_get_known_aggrdecl(_tmp2F1);goto _LL177;_LL17A:;_LL17B:({void*
_tmp2F2[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2F3="init_struct: bad struct type";_tag_dynforward(_tmp2F3,sizeof(
char),_get_zero_arr_size(_tmp2F3,29));}),_tag_dynforward(_tmp2F2,sizeof(void*),0));});
_LL177:;}{struct _RegionHandle _tmp2F4=_new_region("r");struct _RegionHandle*r=&
_tmp2F4;_push_region(r);{struct Cyc_List_List*_tmp2F5=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp2F5
!= 0;_tmp2F5=_tmp2F5->tl){struct _tuple8 _tmp2F7;struct Cyc_List_List*_tmp2F8;
struct Cyc_Absyn_Exp*_tmp2F9;struct _tuple8*_tmp2F6=(struct _tuple8*)_tmp2F5->hd;
_tmp2F7=*_tmp2F6;_tmp2F8=_tmp2F7.f1;_tmp2F9=_tmp2F7.f2;is_atomic=is_atomic  && 
Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F9->topt))->v);
if(_tmp2F8 == 0)({void*_tmp2FA[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2FB="empty designator list";
_tag_dynforward(_tmp2FB,sizeof(char),_get_zero_arr_size(_tmp2FB,22));}),
_tag_dynforward(_tmp2FA,sizeof(void*),0));});if(_tmp2F8->tl != 0){struct _tuple1*
_tmp2FC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2FD=Cyc_Absyn_var_exp(_tmp2FC,
0);for(0;_tmp2F8 != 0;_tmp2F8=_tmp2F8->tl){void*_tmp2FE=(void*)_tmp2F8->hd;struct
_dynforward_ptr*_tmp2FF;_LL17D: if(*((int*)_tmp2FE)!= 1)goto _LL17F;_tmp2FF=((
struct Cyc_Absyn_FieldName_struct*)_tmp2FE)->f1;_LL17E: if(Cyc_Toc_is_poly_field(
struct_type,_tmp2FF))_tmp2FD=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp2FD);
_tmp2EC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2ED(
_tmp2EB,_tmp2FF,0),_tmp2FD,0),0),_tmp2EC,0);goto _LL17C;_LL17F:;_LL180:({void*
_tmp300[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp301="array designator in struct";_tag_dynforward(_tmp301,sizeof(
char),_get_zero_arr_size(_tmp301,27));}),_tag_dynforward(_tmp300,sizeof(void*),0));});
_LL17C:;}Cyc_Toc_exp_to_c(nv,_tmp2F9);_tmp2EC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp2FD,_tmp2F9,0),0),_tmp2EC,0);}else{void*_tmp302=(void*)
_tmp2F8->hd;struct _dynforward_ptr*_tmp303;_LL182: if(*((int*)_tmp302)!= 1)goto
_LL184;_tmp303=((struct Cyc_Absyn_FieldName_struct*)_tmp302)->f1;_LL183: {struct
Cyc_Absyn_Exp*lval=_tmp2ED(_tmp2EB,_tmp303,0);{void*_tmp304=(void*)_tmp2F9->r;
struct Cyc_List_List*_tmp305;struct Cyc_Absyn_Vardecl*_tmp306;struct Cyc_Absyn_Exp*
_tmp307;struct Cyc_Absyn_Exp*_tmp308;int _tmp309;void*_tmp30A;struct Cyc_List_List*
_tmp30B;_LL187: if(*((int*)_tmp304)!= 28)goto _LL189;_tmp305=((struct Cyc_Absyn_Array_e_struct*)
_tmp304)->f1;_LL188: _tmp2EC=Cyc_Toc_init_array(nv,lval,_tmp305,_tmp2EC);goto
_LL186;_LL189: if(*((int*)_tmp304)!= 29)goto _LL18B;_tmp306=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp304)->f1;_tmp307=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp304)->f2;
_tmp308=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp304)->f3;_tmp309=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp304)->f4;_LL18A: _tmp2EC=Cyc_Toc_init_comprehension(
nv,lval,_tmp306,_tmp307,_tmp308,_tmp309,_tmp2EC,0);goto _LL186;_LL18B: if(*((int*)
_tmp304)!= 31)goto _LL18D;_tmp30A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp304)->f1;_tmp30B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp304)->f2;_LL18C:
_tmp2EC=Cyc_Toc_init_anon_struct(nv,lval,_tmp30A,_tmp30B,_tmp2EC);goto _LL186;
_LL18D:;_LL18E: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F9->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp2F9);{struct Cyc_Absyn_Aggrfield*
_tmp30C=Cyc_Absyn_lookup_decl_field(ad,_tmp303);if(Cyc_Toc_is_poly_field(
struct_type,_tmp303) && !was_ptr_type)_tmp2F9=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2F9);if(has_exists)_tmp2F9=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp30C))->type,_tmp2F9);_tmp2EC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp2F9,0),0),_tmp2EC,0);goto _LL186;}}_LL186:;}goto
_LL181;}_LL184:;_LL185:({void*_tmp30D[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30E="array designator in struct";
_tag_dynforward(_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,27));}),
_tag_dynforward(_tmp30D,sizeof(void*),0));});_LL181:;}}}{struct Cyc_Absyn_Exp*
_tmp30F=Cyc_Toc_make_struct(nv,_tmp2EA,_tmp2EE,_tmp2EC,pointer,rgnopt,is_atomic);
_npop_handler(0);return _tmp30F;};_pop_region(r);}}struct _tuple10{struct Cyc_Core_Opt*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple10*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*
pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp311;_tmp311.tag=5;_tmp311.f1=e1;_tmp311.f2=(void*)incr;_tmp311;});_tmp310;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp312=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp313;void*_tmp314;struct Cyc_Absyn_Exp*_tmp315;struct Cyc_Absyn_Exp*_tmp316;
struct _dynforward_ptr*_tmp317;_LL190: if(*((int*)_tmp312)!= 38)goto _LL192;_tmp313=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp312)->f1;_LL191: Cyc_Toc_lvalue_assign_stmt(
_tmp313,fs,f,f_env);goto _LL18F;_LL192: if(*((int*)_tmp312)!= 15)goto _LL194;
_tmp314=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp312)->f1;_tmp315=((struct Cyc_Absyn_Cast_e_struct*)
_tmp312)->f2;_LL193: Cyc_Toc_lvalue_assign(_tmp315,fs,f,f_env);goto _LL18F;_LL194:
if(*((int*)_tmp312)!= 23)goto _LL196;_tmp316=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp312)->f1;_tmp317=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp312)->f2;_LL195:(
void*)(e1->r=(void*)((void*)_tmp316->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->hd=_tmp317;
_tmp318->tl=fs;_tmp318;}),f,f_env);goto _LL18F;_LL196:;_LL197: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((
void*)(f(e1_copy,f_env))->r));goto _LL18F;}_LL18F:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp319=(void*)s->r;struct Cyc_Absyn_Exp*_tmp31A;struct
Cyc_Absyn_Decl*_tmp31B;struct Cyc_Absyn_Stmt*_tmp31C;struct Cyc_Absyn_Stmt*_tmp31D;
_LL199: if(_tmp319 <= (void*)1)goto _LL19F;if(*((int*)_tmp319)!= 0)goto _LL19B;
_tmp31A=((struct Cyc_Absyn_Exp_s_struct*)_tmp319)->f1;_LL19A: Cyc_Toc_lvalue_assign(
_tmp31A,fs,f,f_env);goto _LL198;_LL19B: if(*((int*)_tmp319)!= 11)goto _LL19D;
_tmp31B=((struct Cyc_Absyn_Decl_s_struct*)_tmp319)->f1;_tmp31C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp319)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(_tmp31C,fs,f,f_env);goto _LL198;
_LL19D: if(*((int*)_tmp319)!= 1)goto _LL19F;_tmp31D=((struct Cyc_Absyn_Seq_s_struct*)
_tmp319)->f2;_LL19E: Cyc_Toc_lvalue_assign_stmt(_tmp31D,fs,f,f_env);goto _LL198;
_LL19F:;_LL1A0:({struct Cyc_String_pa_struct _tmp320;_tmp320.tag=0;_tmp320.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp31E[1]={& _tmp320};Cyc_Toc_toc_impos(({const char*_tmp31F="lvalue_assign_stmt: %s";
_tag_dynforward(_tmp31F,sizeof(char),_get_zero_arr_size(_tmp31F,23));}),
_tag_dynforward(_tmp31E,sizeof(void*),1));}});_LL198:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp321=_region_malloc(r2,sizeof(*_tmp321));_tmp321->hd=(
void*)f((void*)x->hd,env);_tmp321->tl=0;_tmp321;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp322=_region_malloc(r2,sizeof(*_tmp322));_tmp322->hd=(void*)f((void*)x->hd,
env);_tmp322->tl=0;_tmp322;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple8*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323->f1=0;_tmp323->f2=e;_tmp323;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp324=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp325;_LL1A2: if(_tmp324 <= (void*)4)goto _LL1A4;if(*((
int*)_tmp324)!= 4)goto _LL1A4;_tmp325=((struct Cyc_Absyn_PointerType_struct*)
_tmp324)->f1;_LL1A3: return _tmp325;_LL1A4:;_LL1A5:({void*_tmp326[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp327="get_ptr_typ: not a pointer!";_tag_dynforward(_tmp327,sizeof(char),
_get_zero_arr_size(_tmp327,28));}),_tag_dynforward(_tmp326,sizeof(void*),0));});
_LL1A1:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp328=Cyc_Tcutil_compress(t);void*_tmp329;void*_tmp32A;void*_tmp32B;
void*_tmp32C;void*_tmp32D;void*_tmp32E;void*_tmp32F;void*_tmp330;void*_tmp331;
void*_tmp332;_LL1A7: if(_tmp328 <= (void*)4)goto _LL1B7;if(*((int*)_tmp328)!= 4)
goto _LL1A9;_LL1A8: res=Cyc_Absyn_null_exp(0);goto _LL1A6;_LL1A9: if(*((int*)_tmp328)
!= 5)goto _LL1AB;_tmp329=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f1;
_tmp32A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f2;if((int)_tmp32A != 
0)goto _LL1AB;_LL1AA: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp333;(_tmp333.Char_c).tag=0;(_tmp333.Char_c).f1=(void*)
_tmp329;(_tmp333.Char_c).f2='\000';_tmp333;}),0);goto _LL1A6;_LL1AB: if(*((int*)
_tmp328)!= 5)goto _LL1AD;_tmp32B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f1;
_tmp32C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f2;if((int)_tmp32C != 
1)goto _LL1AD;_LL1AC: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp334;(_tmp334.Short_c).tag=1;(_tmp334.Short_c).f1=(void*)
_tmp32B;(_tmp334.Short_c).f2=0;_tmp334;}),0);goto _LL1A6;_LL1AD: if(*((int*)
_tmp328)!= 12)goto _LL1AF;_LL1AE: goto _LL1B0;_LL1AF: if(*((int*)_tmp328)!= 13)goto
_LL1B1;_LL1B0: _tmp32D=(void*)1;goto _LL1B2;_LL1B1: if(*((int*)_tmp328)!= 5)goto
_LL1B3;_tmp32D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f1;_tmp32E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f2;if((int)_tmp32E != 2)goto
_LL1B3;_LL1B2: _tmp32F=_tmp32D;goto _LL1B4;_LL1B3: if(*((int*)_tmp328)!= 5)goto
_LL1B5;_tmp32F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f1;_tmp330=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f2;if((int)_tmp330 != 3)goto
_LL1B5;_LL1B4: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp335;(_tmp335.Int_c).tag=2;(_tmp335.Int_c).f1=(void*)_tmp32F;(_tmp335.Int_c).f2=
0;_tmp335;}),0);goto _LL1A6;_LL1B5: if(*((int*)_tmp328)!= 5)goto _LL1B7;_tmp331=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp328)->f1;_tmp332=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp328)->f2;if((int)_tmp332 != 4)goto _LL1B7;_LL1B6: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp336;(_tmp336.LongLong_c).tag=
3;(_tmp336.LongLong_c).f1=(void*)_tmp331;(_tmp336.LongLong_c).f2=(long long)0;
_tmp336;}),0);goto _LL1A6;_LL1B7: if((int)_tmp328 != 1)goto _LL1B9;_LL1B8: goto _LL1BA;
_LL1B9: if(_tmp328 <= (void*)4)goto _LL1BB;if(*((int*)_tmp328)!= 6)goto _LL1BB;
_LL1BA: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp337;(_tmp337.Float_c).tag=4;(_tmp337.Float_c).f1=({const char*_tmp338="0.0";
_tag_dynforward(_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,4));});_tmp337;}),
0);goto _LL1A6;_LL1BB:;_LL1BC:({struct Cyc_String_pa_struct _tmp33B;_tmp33B.tag=0;
_tmp33B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp339[1]={& _tmp33B};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp33A="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,40));}),
_tag_dynforward(_tmp339,sizeof(void*),1));}});_LL1A6:;}res->topt=({struct Cyc_Core_Opt*
_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->v=(void*)t;_tmp33C;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp33D=Cyc_Toc_typ_to_c(
elt_type);void*_tmp33E=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp33F=Cyc_Absyn_cstar_typ(
_tmp33D,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp340=({struct Cyc_Core_Opt*_tmp37E=
_cycalloc(sizeof(*_tmp37E));_tmp37E->v=(void*)_tmp33F;_tmp37E;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp341=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*
_tmp343;struct Cyc_Absyn_Exp*_tmp344;_LL1BE: if(*((int*)_tmp341)!= 22)goto _LL1C0;
_tmp342=((struct Cyc_Absyn_Deref_e_struct*)_tmp341)->f1;_LL1BF: if(!is_fat){
_tmp342=Cyc_Toc_cast_it(fat_ptr_type,_tmp342);_tmp342->topt=({struct Cyc_Core_Opt*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->v=(void*)fat_ptr_type;_tmp345;});}
Cyc_Toc_exp_to_c(nv,_tmp342);xinit=_tmp342;goto _LL1BD;_LL1C0: if(*((int*)_tmp341)
!= 25)goto _LL1C2;_tmp343=((struct Cyc_Absyn_Subscript_e_struct*)_tmp341)->f1;
_tmp344=((struct Cyc_Absyn_Subscript_e_struct*)_tmp341)->f2;_LL1C1: if(!is_fat){
_tmp343=Cyc_Toc_cast_it(fat_ptr_type,_tmp343);_tmp343->topt=({struct Cyc_Core_Opt*
_tmp346=_cycalloc(sizeof(*_tmp346));_tmp346->v=(void*)fat_ptr_type;_tmp346;});}
Cyc_Toc_exp_to_c(nv,_tmp343);Cyc_Toc_exp_to_c(nv,_tmp344);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp347[3];_tmp347[2]=_tmp344;_tmp347[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp347[0]=_tmp343;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp347,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp348[3];_tmp348[2]=_tmp344;_tmp348[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp348[0]=_tmp343;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp348,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1BD;_LL1C2:;_LL1C3:({void*_tmp349[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp34A="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,53));}),
_tag_dynforward(_tmp349,sizeof(void*),0));});_LL1BD:;}{struct _tuple1*_tmp34B=Cyc_Toc_temp_var();
struct _RegionHandle _tmp34C=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp34C;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp34D=Cyc_Toc_add_varmap(rgn2,nv,_tmp34B,
Cyc_Absyn_var_exp(_tmp34B,0));struct Cyc_Absyn_Vardecl*_tmp34E=({struct Cyc_Absyn_Vardecl*
_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->sc=(void*)((void*)2);_tmp37D->name=
_tmp34B;_tmp37D->tq=Cyc_Toc_mt_tq;_tmp37D->type=(void*)_tmp33E;_tmp37D->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp37D->rgn=0;_tmp37D->attributes=0;_tmp37D->escapes=
0;_tmp37D;});struct Cyc_Absyn_Local_b_struct*_tmp34F=({struct Cyc_Absyn_Local_b_struct*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_Absyn_Local_b_struct
_tmp37C;_tmp37C.tag=3;_tmp37C.f1=_tmp34E;_tmp37C;});_tmp37B;});struct Cyc_Absyn_Exp*
_tmp350=Cyc_Absyn_varb_exp(_tmp34B,(void*)_tmp34F,0);_tmp350->topt=({struct Cyc_Core_Opt*
_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->v=(void*)fat_ptr_type;_tmp351;});{
struct Cyc_Absyn_Exp*_tmp352=Cyc_Absyn_deref_exp(_tmp350,0);_tmp352->topt=({
struct Cyc_Core_Opt*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->v=(void*)
elt_type;_tmp353;});Cyc_Toc_exp_to_c(_tmp34D,_tmp352);{struct _tuple1*_tmp354=Cyc_Toc_temp_var();
_tmp34D=Cyc_Toc_add_varmap(rgn2,_tmp34D,_tmp354,Cyc_Absyn_var_exp(_tmp354,0));{
struct Cyc_Absyn_Vardecl*_tmp355=({struct Cyc_Absyn_Vardecl*_tmp37A=_cycalloc(
sizeof(*_tmp37A));_tmp37A->sc=(void*)((void*)2);_tmp37A->name=_tmp354;_tmp37A->tq=
Cyc_Toc_mt_tq;_tmp37A->type=(void*)_tmp33D;_tmp37A->initializer=(struct Cyc_Absyn_Exp*)
_tmp352;_tmp37A->rgn=0;_tmp37A->attributes=0;_tmp37A->escapes=0;_tmp37A;});
struct Cyc_Absyn_Local_b_struct*_tmp356=({struct Cyc_Absyn_Local_b_struct*_tmp378=
_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_Absyn_Local_b_struct _tmp379;
_tmp379.tag=3;_tmp379.f1=_tmp355;_tmp379;});_tmp378;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp357=Cyc_Absyn_varb_exp(_tmp354,(
void*)_tmp356,0);_tmp357->topt=_tmp352->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp357,e2,0);z_init->topt=_tmp357->topt;}Cyc_Toc_exp_to_c(_tmp34D,
z_init);{struct _tuple1*_tmp358=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp359=({struct Cyc_Absyn_Vardecl*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->sc=(
void*)((void*)2);_tmp377->name=_tmp358;_tmp377->tq=Cyc_Toc_mt_tq;_tmp377->type=(
void*)_tmp33D;_tmp377->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp377->rgn=0;
_tmp377->attributes=0;_tmp377->escapes=0;_tmp377;});struct Cyc_Absyn_Local_b_struct*
_tmp35A=({struct Cyc_Absyn_Local_b_struct*_tmp375=_cycalloc(sizeof(*_tmp375));
_tmp375[0]=({struct Cyc_Absyn_Local_b_struct _tmp376;_tmp376.tag=3;_tmp376.f1=
_tmp359;_tmp376;});_tmp375;});_tmp34D=Cyc_Toc_add_varmap(rgn2,_tmp34D,_tmp358,
Cyc_Absyn_var_exp(_tmp358,0));{struct Cyc_Absyn_Exp*_tmp35B=Cyc_Absyn_varb_exp(
_tmp354,(void*)_tmp356,0);_tmp35B->topt=_tmp352->topt;{struct Cyc_Absyn_Exp*
_tmp35C=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp35D=Cyc_Absyn_prim2_exp((
void*)5,_tmp35B,_tmp35C,0);_tmp35D->topt=({struct Cyc_Core_Opt*_tmp35E=_cycalloc(
sizeof(*_tmp35E));_tmp35E->v=(void*)Cyc_Absyn_sint_typ;_tmp35E;});Cyc_Toc_exp_to_c(
_tmp34D,_tmp35D);{struct Cyc_Absyn_Exp*_tmp35F=Cyc_Absyn_varb_exp(_tmp358,(void*)
_tmp35A,0);_tmp35F->topt=_tmp352->topt;{struct Cyc_Absyn_Exp*_tmp360=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp361=Cyc_Absyn_prim2_exp((void*)6,_tmp35F,
_tmp360,0);_tmp361->topt=({struct Cyc_Core_Opt*_tmp362=_cycalloc(sizeof(*_tmp362));
_tmp362->v=(void*)Cyc_Absyn_sint_typ;_tmp362;});Cyc_Toc_exp_to_c(_tmp34D,_tmp361);{
struct Cyc_List_List*_tmp363=({struct Cyc_Absyn_Exp*_tmp374[2];_tmp374[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp374[0]=Cyc_Absyn_varb_exp(_tmp34B,(void*)
_tmp34F,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp374,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp364=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=
Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,
_tmp363,0),_tmp364,0);else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp363,0),_tmp364,0);}{struct Cyc_Absyn_Exp*
_tmp365=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp35D,_tmp361,0),0);struct
Cyc_Absyn_Stmt*_tmp366=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,
0,0),0);struct Cyc_Absyn_Exp*_tmp367=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp34B,(void*)_tmp34F,0),Cyc_Toc_curr_sp,0);_tmp367=Cyc_Toc_cast_it(_tmp33F,
_tmp367);{struct Cyc_Absyn_Exp*_tmp368=Cyc_Absyn_deref_exp(_tmp367,0);struct Cyc_Absyn_Exp*
_tmp369=Cyc_Absyn_assign_exp(_tmp368,Cyc_Absyn_var_exp(_tmp358,0),0);struct Cyc_Absyn_Stmt*
_tmp36A=Cyc_Absyn_exp_stmt(_tmp369,0);_tmp36A=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp365,_tmp366,Cyc_Absyn_skip_stmt(0),0),_tmp36A,0);_tmp36A=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C[
0]=({struct Cyc_Absyn_Var_d_struct _tmp36D;_tmp36D.tag=0;_tmp36D.f1=_tmp359;
_tmp36D;});_tmp36C;}));_tmp36B->loc=0;_tmp36B;}),_tmp36A,0);_tmp36A=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F[
0]=({struct Cyc_Absyn_Var_d_struct _tmp370;_tmp370.tag=0;_tmp370.f1=_tmp355;
_tmp370;});_tmp36F;}));_tmp36E->loc=0;_tmp36E;}),_tmp36A,0);_tmp36A=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[
0]=({struct Cyc_Absyn_Var_d_struct _tmp373;_tmp373.tag=0;_tmp373.f1=_tmp34E;
_tmp373;});_tmp372;}));_tmp371->loc=0;_tmp371;}),_tmp36A,0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(_tmp36A));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple11{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple12{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp37F=(void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp382;_tmp382.tag=0;_tmp382.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp380[1]={& _tmp382};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp381="exp_to_c: no type for %s";_tag_dynforward(_tmp381,sizeof(char),
_get_zero_arr_size(_tmp381,25));}),_tag_dynforward(_tmp380,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp383=
_tmp37F;union Cyc_Absyn_Cnst_union _tmp384;struct _tuple1*_tmp385;void*_tmp386;
struct _tuple1*_tmp387;void*_tmp388;struct Cyc_List_List*_tmp389;struct Cyc_Absyn_Exp*
_tmp38A;void*_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;struct Cyc_Core_Opt*_tmp38D;
struct Cyc_Absyn_Exp*_tmp38E;struct Cyc_Absyn_Exp*_tmp38F;struct Cyc_Absyn_Exp*
_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*_tmp392;struct Cyc_Absyn_Exp*
_tmp393;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct Cyc_Absyn_Exp*
_tmp396;struct Cyc_Absyn_Exp*_tmp397;struct Cyc_Absyn_Exp*_tmp398;struct Cyc_List_List*
_tmp399;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_List_List*_tmp39B;struct Cyc_Absyn_VarargCallInfo*
_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_VarargCallInfo*
_tmp39F;struct Cyc_Absyn_VarargCallInfo _tmp3A0;int _tmp3A1;struct Cyc_List_List*
_tmp3A2;struct Cyc_Absyn_VarargInfo*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Exp*
_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_List_List*_tmp3A7;void*_tmp3A8;
void**_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AA;int _tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Exp*
_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*
_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;struct _dynforward_ptr*_tmp3B4;
void*_tmp3B5;void*_tmp3B6;unsigned int _tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;struct
Cyc_Absyn_Exp*_tmp3B9;struct _dynforward_ptr*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;
struct _dynforward_ptr*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_List_List*_tmp3BF;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Vardecl*
_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;int _tmp3C4;
struct _tuple1*_tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_List_List*_tmp3C7;
struct Cyc_Absyn_Aggrdecl*_tmp3C8;void*_tmp3C9;struct Cyc_List_List*_tmp3CA;struct
Cyc_List_List*_tmp3CB;struct Cyc_Absyn_Tuniondecl*_tmp3CC;struct Cyc_Absyn_Tunionfield*
_tmp3CD;struct Cyc_List_List*_tmp3CE;struct Cyc_Absyn_Tuniondecl*_tmp3CF;struct Cyc_Absyn_Tunionfield*
_tmp3D0;struct Cyc_Absyn_MallocInfo _tmp3D1;int _tmp3D2;struct Cyc_Absyn_Exp*_tmp3D3;
void**_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;int _tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;
struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D9;_LL1C5: if(*((int*)
_tmp383)!= 0)goto _LL1C7;_tmp384=((struct Cyc_Absyn_Const_e_struct*)_tmp383)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp383)->f1).Null_c).tag != 6)goto _LL1C7;
_LL1C6: {struct Cyc_Absyn_Exp*_tmp3DA=Cyc_Absyn_uint_exp(0,0);int forward_only=0;
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp3DA,
_tmp3DA,forward_only))->r));else{if(forward_only)(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp3DB[3];_tmp3DB[2]=_tmp3DA;
_tmp3DB[1]=_tmp3DA;_tmp3DB[0]=_tmp3DA;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DB,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3DC[3];_tmp3DC[2]=_tmp3DA;_tmp3DC[1]=_tmp3DA;_tmp3DC[0]=
_tmp3DA;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3DC,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1C4;}_LL1C7: if(*((int*)_tmp383)!= 0)
goto _LL1C9;_LL1C8: goto _LL1C4;_LL1C9: if(*((int*)_tmp383)!= 1)goto _LL1CB;_tmp385=((
struct Cyc_Absyn_Var_e_struct*)_tmp383)->f1;_tmp386=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp383)->f2;_LL1CA:{struct _handler_cons _tmp3DD;_push_handler(& _tmp3DD);{int
_tmp3DF=0;if(setjmp(_tmp3DD.handler))_tmp3DF=1;if(!_tmp3DF){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp385))->r));;_pop_handler();}else{void*
_tmp3DE=(void*)_exn_thrown;void*_tmp3E1=_tmp3DE;_LL21C: if(_tmp3E1 != Cyc_Dict_Absent)
goto _LL21E;_LL21D:({struct Cyc_String_pa_struct _tmp3E4;_tmp3E4.tag=0;_tmp3E4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp385));{
void*_tmp3E2[1]={& _tmp3E4};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3E3="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp3E3,sizeof(char),_get_zero_arr_size(_tmp3E3,32));}),
_tag_dynforward(_tmp3E2,sizeof(void*),1));}});_LL21E:;_LL21F:(void)_throw(
_tmp3E1);_LL21B:;}}}goto _LL1C4;_LL1CB: if(*((int*)_tmp383)!= 2)goto _LL1CD;_tmp387=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp383)->f1;_LL1CC:({void*_tmp3E5[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3E6="unknownid";_tag_dynforward(_tmp3E6,sizeof(char),
_get_zero_arr_size(_tmp3E6,10));}),_tag_dynforward(_tmp3E5,sizeof(void*),0));});
_LL1CD: if(*((int*)_tmp383)!= 3)goto _LL1CF;_tmp388=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp383)->f1;_tmp389=((struct Cyc_Absyn_Primop_e_struct*)_tmp383)->f2;_LL1CE: {
struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp389);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp389);{void*_tmp3E8=_tmp388;_LL221:
if((int)_tmp3E8 != 19)goto _LL223;_LL222: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp389))->hd;{void*_tmp3E9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;struct Cyc_Absyn_PtrInfo _tmp3EC;void*_tmp3ED;
struct Cyc_Absyn_PtrAtts _tmp3EE;struct Cyc_Absyn_Conref*_tmp3EF;struct Cyc_Absyn_Conref*
_tmp3F0;struct Cyc_Absyn_Conref*_tmp3F1;_LL236: if(_tmp3E9 <= (void*)4)goto _LL23A;
if(*((int*)_tmp3E9)!= 7)goto _LL238;_tmp3EA=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E9)->f1;_tmp3EB=_tmp3EA.num_elts;_LL237:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp3EB))->r));goto _LL235;_LL238: if(*((int*)
_tmp3E9)!= 4)goto _LL23A;_tmp3EC=((struct Cyc_Absyn_PointerType_struct*)_tmp3E9)->f1;
_tmp3ED=(void*)_tmp3EC.elt_typ;_tmp3EE=_tmp3EC.ptr_atts;_tmp3EF=_tmp3EE.nullable;
_tmp3F0=_tmp3EE.bounds;_tmp3F1=_tmp3EE.zero_term;_LL239:{void*_tmp3F2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3F0);struct Cyc_Absyn_Exp*_tmp3F3;_LL23D: if((int)_tmp3F2
!= 0)goto _LL23F;_LL23E:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp3F4[2];_tmp3F4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3ED),0);_tmp3F4[0]=(struct Cyc_Absyn_Exp*)_tmp389->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3F4,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL23C;_LL23F: if((int)_tmp3F2 != 1)goto _LL241;_LL240:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp3F5[2];_tmp3F5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3ED),0);
_tmp3F5[0]=(struct Cyc_Absyn_Exp*)_tmp389->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3F5,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL23C;_LL241: if(_tmp3F2 <= (void*)2)goto _LL243;if(*((int*)_tmp3F2)
!= 0)goto _LL243;_tmp3F3=((struct Cyc_Absyn_Upper_b_struct*)_tmp3F2)->f1;_LL242:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3F1))(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp3F6[2];_tmp3F6[1]=_tmp3F3;_tmp3F6[0]=(struct Cyc_Absyn_Exp*)_tmp389->hd;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3F6,sizeof(struct Cyc_Absyn_Exp*),2));})));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp3EF))(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp3F3,Cyc_Absyn_uint_exp(0,0)));else{(void*)(e->r=(void*)((void*)_tmp3F3->r));
goto _LL23C;}}goto _LL23C;_LL243: if(_tmp3F2 <= (void*)2)goto _LL23C;if(*((int*)
_tmp3F2)!= 1)goto _LL23C;_LL244:({void*_tmp3F7[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3F8="toc: size of AbsUpper_b";
_tag_dynforward(_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,24));}),
_tag_dynforward(_tmp3F7,sizeof(void*),0));});_LL23C:;}goto _LL235;_LL23A:;_LL23B:({
struct Cyc_String_pa_struct _tmp3FC;_tmp3FC.tag=0;_tmp3FC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp3FB;
_tmp3FB.tag=0;_tmp3FB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp3F9[2]={&
_tmp3FB,& _tmp3FC};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3FA="size primop applied to non-array %s (%s)";_tag_dynforward(
_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,41));}),_tag_dynforward(_tmp3F9,
sizeof(void*),2));}}});_LL235:;}goto _LL220;}_LL223: if((int)_tmp3E8 != 0)goto
_LL225;_LL224:{void*_tmp3FD=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)
_check_null(_tmp3E7))->hd);struct Cyc_Absyn_PtrInfo _tmp3FE;void*_tmp3FF;struct Cyc_Absyn_PtrAtts
_tmp400;struct Cyc_Absyn_Conref*_tmp401;struct Cyc_Absyn_Conref*_tmp402;_LL246: if(
_tmp3FD <= (void*)4)goto _LL248;if(*((int*)_tmp3FD)!= 4)goto _LL248;_tmp3FE=((
struct Cyc_Absyn_PointerType_struct*)_tmp3FD)->f1;_tmp3FF=(void*)_tmp3FE.elt_typ;
_tmp400=_tmp3FE.ptr_atts;_tmp401=_tmp400.bounds;_tmp402=_tmp400.zero_term;_LL247:{
void*_tmp403=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp401);struct Cyc_Absyn_Exp*
_tmp404;_LL24B: if((int)_tmp403 != 0)goto _LL24D;_LL24C: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp389))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp389->tl))->hd;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,({struct
Cyc_Absyn_Exp*_tmp405[3];_tmp405[2]=e2;_tmp405[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3FF),0);_tmp405[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp405,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL24A;}_LL24D:
if((int)_tmp403 != 1)goto _LL24F;_LL24E: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp389))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp389->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp406[3];_tmp406[2]=e2;
_tmp406[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3FF),0);_tmp406[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp406,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL24A;}_LL24F: if(_tmp403 <= (
void*)2)goto _LL251;if(*((int*)_tmp403)!= 0)goto _LL251;_tmp404=((struct Cyc_Absyn_Upper_b_struct*)
_tmp403)->f1;_LL250: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp402)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp389))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp389->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp407[3];_tmp407[2]=e2;_tmp407[
1]=_tmp404;_tmp407[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp407,sizeof(struct Cyc_Absyn_Exp*),3));})));}goto _LL24A;_LL251:
if(_tmp403 <= (void*)2)goto _LL24A;if(*((int*)_tmp403)!= 1)goto _LL24A;_LL252:({
void*_tmp408[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp409="toc: plus on AbsUpper_b";_tag_dynforward(
_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,24));}),_tag_dynforward(_tmp408,
sizeof(void*),0));});_LL24A:;}goto _LL245;_LL248:;_LL249: goto _LL245;_LL245:;}goto
_LL220;_LL225: if((int)_tmp3E8 != 2)goto _LL227;_LL226: {void*elt_typ=(void*)0;int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp3E7))->hd,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp389))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp389->tl))->hd;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp3E7->tl))->hd,& forward_only)){(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(
Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp40A[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp40B="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,50));}),
_tag_dynforward(_tmp40A,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp40C[3];_tmp40C[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp40C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp40C[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp40C,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL220;}
_LL227: if((int)_tmp3E8 != 5)goto _LL229;_LL228: goto _LL22A;_LL229: if((int)_tmp3E8 != 
6)goto _LL22B;_LL22A: goto _LL22C;_LL22B: if((int)_tmp3E8 != 7)goto _LL22D;_LL22C: goto
_LL22E;_LL22D: if((int)_tmp3E8 != 9)goto _LL22F;_LL22E: goto _LL230;_LL22F: if((int)
_tmp3E8 != 8)goto _LL231;_LL230: goto _LL232;_LL231: if((int)_tmp3E8 != 10)goto _LL233;
_LL232: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp389))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp389->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3E7))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp3E7->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL220;}_LL233:;_LL234: goto _LL220;_LL220:;}
goto _LL1C4;}_LL1CF: if(*((int*)_tmp383)!= 5)goto _LL1D1;_tmp38A=((struct Cyc_Absyn_Increment_e_struct*)
_tmp383)->f1;_tmp38B=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp383)->f2;
_LL1D0: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp38A->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp418="increment";_tag_dynforward(
_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,10));});if(_tmp38B == (void*)2
 || _tmp38B == (void*)3)incr_str=({const char*_tmp40D="decrement";_tag_dynforward(
_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,10));});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp38A,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp410;_tmp410.tag=0;_tmp410.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp40E[1]={& _tmp410};Cyc_Tcutil_terr(e->loc,({const char*_tmp40F="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp40F,sizeof(char),_get_zero_arr_size(_tmp40F,74));}),
_tag_dynforward(_tmp40E,sizeof(void*),1));}});({void*_tmp411[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp412="in-place inc on zero-term";_tag_dynforward(_tmp412,sizeof(char),
_get_zero_arr_size(_tmp412,26));}),_tag_dynforward(_tmp411,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp38A);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp38B == (void*)3  || _tmp38B == (void*)2)({void*_tmp413[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp414="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,43));}),
_tag_dynforward(_tmp413,sizeof(void*),0));});fn_e=_tmp38B == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp38B == (void*)1  || _tmp38B
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp38B == (void*)2  || _tmp38B == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp415[3];_tmp415[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp415[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp415[0]=
Cyc_Absyn_address_exp(_tmp38A,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp415,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp416=_tmp38B == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e: Cyc_Toc__zero_arr_inplace_plus_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp416,({struct Cyc_Absyn_Exp*_tmp417[2];
_tmp417[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp417[0]=_tmp38A;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp417,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp38A)){Cyc_Toc_lvalue_assign(
_tmp38A,0,Cyc_Toc_incr_lvalue,_tmp38B);(void*)(e->r=(void*)((void*)_tmp38A->r));}}}
goto _LL1C4;}}_LL1D1: if(*((int*)_tmp383)!= 4)goto _LL1D3;_tmp38C=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp383)->f1;_tmp38D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp383)->f2;_tmp38E=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp383)->f3;_LL1D2: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp38C,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp38C,_tmp38D,_tmp38E,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp38C);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp38C->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp38E->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp38C);
Cyc_Toc_exp_to_c(nv,_tmp38E);{int done=0;if(_tmp38D != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp419=(void*)_tmp38D->v;_LL254:
if((int)_tmp419 != 0)goto _LL256;_LL255: change=_tmp38E;goto _LL253;_LL256: if((int)
_tmp419 != 2)goto _LL258;_LL257: if(forward_only)({void*_tmp41A[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp41B="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp41B,sizeof(char),_get_zero_arr_size(_tmp41B,46));}),
_tag_dynforward(_tmp41A,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp38E,0);goto _LL253;_LL258:;_LL259:({void*_tmp41C[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp41D="bad t ? pointer arithmetic";_tag_dynforward(_tmp41D,sizeof(char),
_get_zero_arr_size(_tmp41D,27));}),_tag_dynforward(_tmp41C,sizeof(void*),0));});
_LL253:;}done=1;{struct Cyc_Absyn_Exp*_tmp41E=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp41E,({struct Cyc_Absyn_Exp*_tmp41F[3];_tmp41F[2]=change;_tmp41F[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp41F[0]=Cyc_Absyn_address_exp(_tmp38C,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp41F,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp420=(void*)_tmp38D->v;_LL25B: if((int)_tmp420 != 0)
goto _LL25D;_LL25C: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp421[2];_tmp421[1]=_tmp38E;_tmp421[0]=_tmp38C;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp421,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL25A;_LL25D:;_LL25E:({void*_tmp422[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp423="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp423,sizeof(
char),_get_zero_arr_size(_tmp423,39));}),_tag_dynforward(_tmp422,sizeof(void*),0));});
_LL25A:;}}}if(!done){if(e1_poly)(void*)(_tmp38E->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp38E->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp38C)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple10*),struct _tuple10*f_env))Cyc_Toc_lvalue_assign)(
_tmp38C,0,Cyc_Toc_assignop_lvalue,({struct _tuple10*_tmp424=_cycalloc(sizeof(
struct _tuple10)* 1);_tmp424[0]=({struct _tuple10 _tmp425;_tmp425.f1=_tmp38D;
_tmp425.f2=_tmp38E;_tmp425;});_tmp424;}));(void*)(e->r=(void*)((void*)_tmp38C->r));}}
goto _LL1C4;}}}_LL1D3: if(*((int*)_tmp383)!= 6)goto _LL1D5;_tmp38F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp383)->f1;_tmp390=((struct Cyc_Absyn_Conditional_e_struct*)_tmp383)->f2;
_tmp391=((struct Cyc_Absyn_Conditional_e_struct*)_tmp383)->f3;_LL1D4: Cyc_Toc_exp_to_c(
nv,_tmp38F);Cyc_Toc_exp_to_c(nv,_tmp390);Cyc_Toc_exp_to_c(nv,_tmp391);goto _LL1C4;
_LL1D5: if(*((int*)_tmp383)!= 7)goto _LL1D7;_tmp392=((struct Cyc_Absyn_And_e_struct*)
_tmp383)->f1;_tmp393=((struct Cyc_Absyn_And_e_struct*)_tmp383)->f2;_LL1D6: Cyc_Toc_exp_to_c(
nv,_tmp392);Cyc_Toc_exp_to_c(nv,_tmp393);goto _LL1C4;_LL1D7: if(*((int*)_tmp383)!= 
8)goto _LL1D9;_tmp394=((struct Cyc_Absyn_Or_e_struct*)_tmp383)->f1;_tmp395=((
struct Cyc_Absyn_Or_e_struct*)_tmp383)->f2;_LL1D8: Cyc_Toc_exp_to_c(nv,_tmp394);
Cyc_Toc_exp_to_c(nv,_tmp395);goto _LL1C4;_LL1D9: if(*((int*)_tmp383)!= 9)goto
_LL1DB;_tmp396=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp383)->f1;_tmp397=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp383)->f2;_LL1DA: Cyc_Toc_exp_to_c(nv,_tmp396);Cyc_Toc_exp_to_c(
nv,_tmp397);goto _LL1C4;_LL1DB: if(*((int*)_tmp383)!= 10)goto _LL1DD;_tmp398=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp383)->f1;_tmp399=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp383)->f2;_LL1DC: _tmp39A=_tmp398;_tmp39B=_tmp399;goto _LL1DE;_LL1DD: if(*((int*)
_tmp383)!= 11)goto _LL1DF;_tmp39A=((struct Cyc_Absyn_FnCall_e_struct*)_tmp383)->f1;
_tmp39B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp383)->f2;_tmp39C=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp383)->f3;if(_tmp39C != 0)goto _LL1DF;_LL1DE: Cyc_Toc_exp_to_c(nv,_tmp39A);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp39B);goto _LL1C4;
_LL1DF: if(*((int*)_tmp383)!= 11)goto _LL1E1;_tmp39D=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp383)->f1;_tmp39E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp383)->f2;_tmp39F=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp383)->f3;if(_tmp39F == 0)goto _LL1E1;_tmp3A0=*
_tmp39F;_tmp3A1=_tmp3A0.num_varargs;_tmp3A2=_tmp3A0.injectors;_tmp3A3=_tmp3A0.vai;
_LL1E0:{struct _RegionHandle _tmp426=_new_region("r");struct _RegionHandle*r=&
_tmp426;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp3A1,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp3A3->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp39E);int num_normargs=num_args - _tmp3A1;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp39E=_tmp39E->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp39E))->hd);
new_args=({struct Cyc_List_List*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->hd=(
struct Cyc_Absyn_Exp*)_tmp39E->hd;_tmp427->tl=new_args;_tmp427;});}}new_args=({
struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp429[3];_tmp429[2]=
num_varargs_exp;_tmp429[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp429[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp429,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp428->tl=
new_args;_tmp428;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp39D);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp39D,new_args,0),0);if(_tmp3A3->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp42A=Cyc_Tcutil_compress((void*)_tmp3A3->type);
struct Cyc_Absyn_TunionInfo _tmp42B;union Cyc_Absyn_TunionInfoU_union _tmp42C;struct
Cyc_Absyn_Tuniondecl**_tmp42D;struct Cyc_Absyn_Tuniondecl*_tmp42E;_LL260: if(
_tmp42A <= (void*)4)goto _LL262;if(*((int*)_tmp42A)!= 2)goto _LL262;_tmp42B=((
struct Cyc_Absyn_TunionType_struct*)_tmp42A)->f1;_tmp42C=_tmp42B.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp42A)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL262;_tmp42D=(_tmp42C.KnownTunion).f1;_tmp42E=*_tmp42D;_LL261: tud=
_tmp42E;goto _LL25F;_LL262:;_LL263:({void*_tmp42F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp430="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp430,
sizeof(char),_get_zero_arr_size(_tmp430,42));}),_tag_dynforward(_tmp42F,sizeof(
void*),0));});_LL25F:;}{struct _dynforward_ptr vs=({unsigned int _tmp438=(
unsigned int)_tmp3A1;struct _tuple1**_tmp439=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp438));struct _dynforward_ptr _tmp43B=
_tag_dynforward(_tmp439,sizeof(struct _tuple1*),_tmp438);{unsigned int _tmp43A=
_tmp438;unsigned int i;for(i=0;i < _tmp43A;i ++){_tmp439[i]=Cyc_Toc_temp_var();}}
_tmp43B;});struct Cyc_List_List*_tmp431=0;{int i=_tmp3A1 - 1;for(0;i >= 0;-- i){
_tmp431=({struct Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp432->tl=
_tmp431;_tmp432;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp431,0),s,0);{int i=0;for(0;_tmp39E != 0;(((
_tmp39E=_tmp39E->tl,_tmp3A2=_tmp3A2->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp39E->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp434;struct _tuple1*
_tmp435;struct Cyc_List_List*_tmp436;struct Cyc_Absyn_Tunionfield*_tmp433=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp3A2))->hd;_tmp434=*
_tmp433;_tmp435=_tmp434.name;_tmp436=_tmp434.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp436))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp435,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp435,({const char*_tmp437="_struct";
_tag_dynforward(_tmp437,sizeof(char),_get_zero_arr_size(_tmp437,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp39E != 0;(_tmp39E=_tmp39E->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp39E->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp39E->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1C4;_LL1E1: if(*((int*)_tmp383)!= 12)goto _LL1E3;
_tmp3A4=((struct Cyc_Absyn_Throw_e_struct*)_tmp383)->f1;_LL1E2: Cyc_Toc_exp_to_c(
nv,_tmp3A4);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp3A4),0))->r));goto _LL1C4;_LL1E3: if(*((int*)
_tmp383)!= 13)goto _LL1E5;_tmp3A5=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp383)->f1;_LL1E4: Cyc_Toc_exp_to_c(nv,_tmp3A5);goto _LL1C4;_LL1E5: if(*((int*)
_tmp383)!= 14)goto _LL1E7;_tmp3A6=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp383)->f1;
_tmp3A7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp383)->f2;_LL1E6: Cyc_Toc_exp_to_c(
nv,_tmp3A6);for(0;_tmp3A7 != 0;_tmp3A7=_tmp3A7->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp3A7->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp43C=Cyc_Tcutil_compress((void*)_tmp3A7->hd);_LL265: if(_tmp43C
<= (void*)4)goto _LL269;if(*((int*)_tmp43C)!= 1)goto _LL267;_LL266: goto _LL268;
_LL267: if(*((int*)_tmp43C)!= 2)goto _LL269;_LL268: continue;_LL269:;_LL26A:(void*)(
e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp3A6,0))->r));goto _LL264;_LL264:;}
break;}}goto _LL1C4;_LL1E7: if(*((int*)_tmp383)!= 15)goto _LL1E9;_tmp3A8=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp383)->f1;_tmp3A9=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp383)->f1);_tmp3AA=((struct Cyc_Absyn_Cast_e_struct*)_tmp383)->f2;_tmp3AB=((
struct Cyc_Absyn_Cast_e_struct*)_tmp383)->f3;_tmp3AC=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp383)->f4;_LL1E8: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3AA->topt))->v;void*new_typ=*_tmp3A9;*_tmp3A9=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(
nv,_tmp3AA);{struct _tuple0 _tmp43E=({struct _tuple0 _tmp43D;_tmp43D.f1=Cyc_Tcutil_compress(
old_t2);_tmp43D.f2=Cyc_Tcutil_compress(new_typ);_tmp43D;});void*_tmp43F;struct
Cyc_Absyn_PtrInfo _tmp440;void*_tmp441;struct Cyc_Absyn_PtrInfo _tmp442;void*
_tmp443;struct Cyc_Absyn_PtrInfo _tmp444;void*_tmp445;_LL26C: _tmp43F=_tmp43E.f1;
if(_tmp43F <= (void*)4)goto _LL26E;if(*((int*)_tmp43F)!= 4)goto _LL26E;_tmp440=((
struct Cyc_Absyn_PointerType_struct*)_tmp43F)->f1;_tmp441=_tmp43E.f2;if(_tmp441 <= (
void*)4)goto _LL26E;if(*((int*)_tmp441)!= 4)goto _LL26E;_tmp442=((struct Cyc_Absyn_PointerType_struct*)
_tmp441)->f1;_LL26D: {int _tmp446=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp440.ptr_atts).nullable);int _tmp447=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp442.ptr_atts).nullable);void*_tmp448=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp440.ptr_atts).bounds);void*_tmp449=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp442.ptr_atts).bounds);int _tmp44A=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp440.ptr_atts).zero_term);int
_tmp44B=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp442.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp44D=({struct _tuple0
_tmp44C;_tmp44C.f1=_tmp448;_tmp44C.f2=_tmp449;_tmp44C;});void*_tmp44E;struct Cyc_Absyn_Exp*
_tmp44F;void*_tmp450;struct Cyc_Absyn_Exp*_tmp451;void*_tmp452;struct Cyc_Absyn_Exp*
_tmp453;void*_tmp454;void*_tmp455;struct Cyc_Absyn_Exp*_tmp456;void*_tmp457;void*
_tmp458;void*_tmp459;struct Cyc_Absyn_Exp*_tmp45A;void*_tmp45B;void*_tmp45C;
struct Cyc_Absyn_Exp*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*_tmp461;
void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*_tmp466;void*_tmp467;
void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;_LL273: _tmp44E=_tmp44D.f1;
if(_tmp44E <= (void*)2)goto _LL275;if(*((int*)_tmp44E)!= 0)goto _LL275;_tmp44F=((
struct Cyc_Absyn_Upper_b_struct*)_tmp44E)->f1;_tmp450=_tmp44D.f2;if(_tmp450 <= (
void*)2)goto _LL275;if(*((int*)_tmp450)!= 0)goto _LL275;_tmp451=((struct Cyc_Absyn_Upper_b_struct*)
_tmp450)->f1;_LL274: if(_tmp446  && !_tmp447){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp46C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp46D="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp46D,sizeof(char),_get_zero_arr_size(_tmp46D,44));}),_tag_dynforward(_tmp46C,
sizeof(void*),0));});if(_tmp3AC != (void*)2)({struct Cyc_String_pa_struct _tmp470;
_tmp470.tag=0;_tmp470.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp46E[1]={& _tmp470};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp46F="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,41));}),
_tag_dynforward(_tmp46E,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp3AA);if(do_null_check){if(!_tmp3AB)({void*_tmp471[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp472="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp472,sizeof(char),_get_zero_arr_size(_tmp472,58));}),
_tag_dynforward(_tmp471,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp3A9,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp473=
_cycalloc(sizeof(*_tmp473));_tmp473->hd=_tmp3AA;_tmp473->tl=0;_tmp473;}),0)));}}}
goto _LL272;_LL275: _tmp452=_tmp44D.f1;if(_tmp452 <= (void*)2)goto _LL277;if(*((int*)
_tmp452)!= 0)goto _LL277;_tmp453=((struct Cyc_Absyn_Upper_b_struct*)_tmp452)->f1;
_tmp454=_tmp44D.f2;if((int)_tmp454 != 0)goto _LL277;_LL276: p2_forward_only=1;
_tmp456=_tmp453;goto _LL278;_LL277: _tmp455=_tmp44D.f1;if(_tmp455 <= (void*)2)goto
_LL279;if(*((int*)_tmp455)!= 0)goto _LL279;_tmp456=((struct Cyc_Absyn_Upper_b_struct*)
_tmp455)->f1;_tmp457=_tmp44D.f2;if((int)_tmp457 != 1)goto _LL279;_LL278: if(_tmp3AC
== (void*)2)({struct Cyc_String_pa_struct _tmp476;_tmp476.tag=0;_tmp476.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp474[
1]={& _tmp476};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp475="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp475,sizeof(char),_get_zero_arr_size(_tmp475,44));}),_tag_dynforward(_tmp474,
sizeof(void*),1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp44A  && !(_tmp442.elt_tq).real_const)
 && !_tmp44B)_tmp456=Cyc_Absyn_prim2_exp((void*)2,_tmp456,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp456,
_tmp3AA,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp477=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp44A){struct _tuple1*
_tmp478=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_var_exp(_tmp478,
0);struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp47F[2];_tmp47F[1]=_tmp456;_tmp47F[0]=_tmp479;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp47F,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp44B  && !(_tmp442.elt_tq).real_const)_tmp47A=Cyc_Absyn_prim2_exp((
void*)2,_tmp47A,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp47B=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp442.elt_typ),0);struct Cyc_Absyn_Exp*_tmp47C=Cyc_Absyn_fncall_exp(
_tmp477,({struct Cyc_Absyn_Exp*_tmp47E[3];_tmp47E[2]=_tmp47A;_tmp47E[1]=_tmp47B;
_tmp47E[0]=_tmp479;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp47E,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp47D=Cyc_Absyn_exp_stmt(_tmp47C,0);_tmp47D=Cyc_Absyn_declare_stmt(_tmp478,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp3AA,_tmp47D,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp47D));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp477,({struct Cyc_Absyn_Exp*
_tmp480[3];_tmp480[2]=_tmp456;_tmp480[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp442.elt_typ),0);_tmp480[0]=_tmp3AA;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp480,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL272;_LL279: _tmp458=_tmp44D.f1;if((int)_tmp458 != 0)goto _LL27B;
_tmp459=_tmp44D.f2;if(_tmp459 <= (void*)2)goto _LL27B;if(*((int*)_tmp459)!= 0)goto
_LL27B;_tmp45A=((struct Cyc_Absyn_Upper_b_struct*)_tmp459)->f1;_LL27A:
p1_forward_only=1;_tmp45D=_tmp45A;goto _LL27C;_LL27B: _tmp45B=_tmp44D.f1;if((int)
_tmp45B != 1)goto _LL27D;_tmp45C=_tmp44D.f2;if(_tmp45C <= (void*)2)goto _LL27D;if(*((
int*)_tmp45C)!= 0)goto _LL27D;_tmp45D=((struct Cyc_Absyn_Upper_b_struct*)_tmp45C)->f1;
_LL27C: if(Cyc_Toc_is_toplevel(nv))({void*_tmp481[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp482="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp482,sizeof(char),_get_zero_arr_size(_tmp482,45));}),
_tag_dynforward(_tmp481,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp483=
_tmp45D;if(_tmp44A  && !_tmp44B)_tmp483=Cyc_Absyn_add_exp(_tmp45D,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp484=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp485=Cyc_Absyn_fncall_exp(
_tmp484,({struct Cyc_Absyn_Exp*_tmp487[3];_tmp487[2]=_tmp483;_tmp487[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp440.elt_typ),0);_tmp487[0]=_tmp3AA;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp487,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp447)(void*)(_tmp485->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->hd=Cyc_Absyn_copy_exp(
_tmp485);_tmp486->tl=0;_tmp486;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp3A9,_tmp485));goto _LL272;}}_LL27D: _tmp45E=_tmp44D.f1;if((int)_tmp45E != 0)
goto _LL27F;_tmp45F=_tmp44D.f2;if((int)_tmp45F != 0)goto _LL27F;_LL27E:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL27F: _tmp460=_tmp44D.f1;if((
int)_tmp460 != 0)goto _LL281;_tmp461=_tmp44D.f2;if((int)_tmp461 != 1)goto _LL281;
_LL280: p1_forward_only=1;goto DynCast;_LL281: _tmp462=_tmp44D.f1;if((int)_tmp462 != 
1)goto _LL283;_tmp463=_tmp44D.f2;if((int)_tmp463 != 0)goto _LL283;_LL282:
p2_forward_only=1;goto DynCast;_LL283: _tmp464=_tmp44D.f1;if((int)_tmp464 != 1)goto
_LL285;_tmp465=_tmp44D.f2;if((int)_tmp465 != 1)goto _LL285;_LL284: DynCast: if((
_tmp44A  && !_tmp44B) && !(_tmp442.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp488[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp489="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp489,sizeof(char),_get_zero_arr_size(_tmp489,70));}),
_tag_dynforward(_tmp488,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp48A=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp48A,({struct Cyc_Absyn_Exp*_tmp48B[3];
_tmp48B[2]=Cyc_Absyn_uint_exp(1,0);_tmp48B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp440.elt_typ),0);_tmp48B[0]=_tmp3AA;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp48B,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp48C[1];_tmp48C[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp48C,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp48D[1];_tmp48D[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp48D,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(
nv))({void*_tmp48E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp48F="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp48F,sizeof(char),_get_zero_arr_size(_tmp48F,43));}),
_tag_dynforward(_tmp48E,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp490[1];_tmp490[0]=_tmp3AA;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp490,sizeof(struct Cyc_Absyn_Exp*),1));})));
else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp491[1];_tmp491[0]=_tmp3AA;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp491,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}goto _LL272;_LL285: _tmp466=_tmp44D.f1;if(_tmp466 <= (void*)2)goto
_LL287;if(*((int*)_tmp466)!= 1)goto _LL287;_tmp467=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp466)->f1;_tmp468=_tmp44D.f2;if(_tmp468 <= (void*)2)goto _LL287;if(*((int*)
_tmp468)!= 1)goto _LL287;_tmp469=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp468)->f1;_LL286: if(_tmp446  && !_tmp447){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp492[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp493="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp493,sizeof(char),_get_zero_arr_size(_tmp493,44));}),_tag_dynforward(_tmp492,
sizeof(void*),0));});if(_tmp3AC != (void*)2)({struct Cyc_String_pa_struct _tmp496;
_tmp496.tag=0;_tmp496.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp494[1]={& _tmp496};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp495="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp495,sizeof(char),_get_zero_arr_size(_tmp495,41));}),
_tag_dynforward(_tmp494,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp3AA);if(do_null_check){if(!_tmp3AB)({void*_tmp497[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp498="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp498,sizeof(char),_get_zero_arr_size(_tmp498,58));}),
_tag_dynforward(_tmp497,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp3A9,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp499=
_cycalloc(sizeof(*_tmp499));_tmp499->hd=_tmp3AA;_tmp499->tl=0;_tmp499;}),0)));}}}
goto _LL272;_LL287: _tmp46A=_tmp44D.f1;if(_tmp46A <= (void*)2)goto _LL289;if(*((int*)
_tmp46A)!= 1)goto _LL289;_LL288:({struct Cyc_String_pa_struct _tmp49C;_tmp49C.tag=0;
_tmp49C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp3AA->loc));{void*_tmp49A[1]={& _tmp49C};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp49B="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp49B,sizeof(char),_get_zero_arr_size(_tmp49B,30));}),
_tag_dynforward(_tmp49A,sizeof(void*),1));}});_LL289: _tmp46B=_tmp44D.f2;if(
_tmp46B <= (void*)2)goto _LL272;if(*((int*)_tmp46B)!= 1)goto _LL272;_LL28A: goto
_LL272;_LL272:;}goto _LL26B;}_LL26E: _tmp443=_tmp43E.f1;if(_tmp443 <= (void*)4)goto
_LL270;if(*((int*)_tmp443)!= 4)goto _LL270;_tmp444=((struct Cyc_Absyn_PointerType_struct*)
_tmp443)->f1;_tmp445=_tmp43E.f2;if(_tmp445 <= (void*)4)goto _LL270;if(*((int*)
_tmp445)!= 5)goto _LL270;_LL26F:{void*_tmp49D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp444.ptr_atts).bounds);_LL28C: if((int)_tmp49D != 0)goto _LL28E;_LL28D: goto
_LL28F;_LL28E: if((int)_tmp49D != 1)goto _LL290;_LL28F:(void*)(_tmp3AA->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp3AA->r,_tmp3AA->loc),Cyc_Toc_curr_sp));
goto _LL28B;_LL290:;_LL291: goto _LL28B;_LL28B:;}goto _LL26B;_LL270:;_LL271: goto
_LL26B;_LL26B:;}goto _LL1C4;}_LL1E9: if(*((int*)_tmp383)!= 16)goto _LL1EB;_tmp3AD=((
struct Cyc_Absyn_Address_e_struct*)_tmp383)->f1;_LL1EA:{void*_tmp49E=(void*)
_tmp3AD->r;struct _tuple1*_tmp49F;struct Cyc_List_List*_tmp4A0;struct Cyc_List_List*
_tmp4A1;struct Cyc_List_List*_tmp4A2;_LL293: if(*((int*)_tmp49E)!= 30)goto _LL295;
_tmp49F=((struct Cyc_Absyn_Struct_e_struct*)_tmp49E)->f1;_tmp4A0=((struct Cyc_Absyn_Struct_e_struct*)
_tmp49E)->f2;_tmp4A1=((struct Cyc_Absyn_Struct_e_struct*)_tmp49E)->f3;_LL294: if(
Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp4A5;_tmp4A5.tag=0;
_tmp4A5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp3AD->loc));{void*_tmp4A3[1]={& _tmp4A5};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4A4="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp4A4,sizeof(char),_get_zero_arr_size(_tmp4A4,42));}),
_tag_dynforward(_tmp4A3,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AD->topt))->v,_tmp4A0 != 0,1,0,
_tmp4A1,_tmp49F))->r));goto _LL292;_LL295: if(*((int*)_tmp49E)!= 26)goto _LL297;
_tmp4A2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp49E)->f1;_LL296: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp4A8;_tmp4A8.tag=0;_tmp4A8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp3AD->loc));{
void*_tmp4A6[1]={& _tmp4A8};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4A7="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp4A7,sizeof(char),_get_zero_arr_size(_tmp4A7,42));}),
_tag_dynforward(_tmp4A6,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp4A2))->r));goto _LL292;_LL297:;_LL298: Cyc_Toc_exp_to_c(nv,_tmp3AD);if(
!Cyc_Absyn_is_lvalue(_tmp3AD)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp3AD,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp3AD));}goto _LL292;
_LL292:;}goto _LL1C4;_LL1EB: if(*((int*)_tmp383)!= 17)goto _LL1ED;_tmp3AE=((struct
Cyc_Absyn_New_e_struct*)_tmp383)->f1;_tmp3AF=((struct Cyc_Absyn_New_e_struct*)
_tmp383)->f2;_LL1EC: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp4AB;_tmp4AB.tag=0;_tmp4AB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Position_string_of_segment(_tmp3AF->loc));{void*_tmp4A9[1]={& _tmp4AB};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4AA="%s: new at top-level";_tag_dynforward(_tmp4AA,sizeof(char),
_get_zero_arr_size(_tmp4AA,21));}),_tag_dynforward(_tmp4A9,sizeof(void*),1));}});{
void*_tmp4AC=(void*)_tmp3AF->r;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Vardecl*
_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;int _tmp4B1;
struct _tuple1*_tmp4B2;struct Cyc_List_List*_tmp4B3;struct Cyc_List_List*_tmp4B4;
struct Cyc_List_List*_tmp4B5;_LL29A: if(*((int*)_tmp4AC)!= 28)goto _LL29C;_tmp4AD=((
struct Cyc_Absyn_Array_e_struct*)_tmp4AC)->f1;_LL29B: {struct _tuple1*_tmp4B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B7=Cyc_Absyn_var_exp(_tmp4B6,0);struct Cyc_Absyn_Stmt*
_tmp4B8=Cyc_Toc_init_array(nv,_tmp4B7,_tmp4AD,Cyc_Absyn_exp_stmt(_tmp4B7,0));
void*old_elt_typ;{void*_tmp4B9=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4BA;void*_tmp4BB;struct Cyc_Absyn_Tqual _tmp4BC;struct Cyc_Absyn_PtrAtts _tmp4BD;
struct Cyc_Absyn_Conref*_tmp4BE;_LL2A5: if(_tmp4B9 <= (void*)4)goto _LL2A7;if(*((int*)
_tmp4B9)!= 4)goto _LL2A7;_tmp4BA=((struct Cyc_Absyn_PointerType_struct*)_tmp4B9)->f1;
_tmp4BB=(void*)_tmp4BA.elt_typ;_tmp4BC=_tmp4BA.elt_tq;_tmp4BD=_tmp4BA.ptr_atts;
_tmp4BE=_tmp4BD.zero_term;_LL2A6: old_elt_typ=_tmp4BB;goto _LL2A4;_LL2A7:;_LL2A8:
old_elt_typ=({void*_tmp4BF[0]={};Cyc_Toc_toc_impos(({const char*_tmp4C0="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp4C0,sizeof(char),_get_zero_arr_size(_tmp4C0,52));}),
_tag_dynforward(_tmp4BF,sizeof(void*),0));});_LL2A4:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp4C1=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4C2=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4AD),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp3AE == 0  || Cyc_Absyn_no_regions)e1=Cyc_Toc_malloc_exp(old_elt_typ,
_tmp4C2);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp3AE;Cyc_Toc_exp_to_c(
nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp4C2);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp4B6,_tmp4C1,(struct Cyc_Absyn_Exp*)e1,_tmp4B8,0)));
goto _LL299;}}_LL29C: if(*((int*)_tmp4AC)!= 29)goto _LL29E;_tmp4AE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4AC)->f1;_tmp4AF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4AC)->f2;
_tmp4B0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4AC)->f3;_tmp4B1=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp4AC)->f4;_LL29D: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp4C3=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4C4;void*_tmp4C5;struct Cyc_Absyn_Tqual _tmp4C6;struct Cyc_Absyn_PtrAtts _tmp4C7;
struct Cyc_Absyn_Conref*_tmp4C8;struct Cyc_Absyn_Conref*_tmp4C9;_LL2AA: if(_tmp4C3
<= (void*)4)goto _LL2AC;if(*((int*)_tmp4C3)!= 4)goto _LL2AC;_tmp4C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4C3)->f1;_tmp4C5=(void*)_tmp4C4.elt_typ;_tmp4C6=_tmp4C4.elt_tq;_tmp4C7=
_tmp4C4.ptr_atts;_tmp4C8=_tmp4C7.bounds;_tmp4C9=_tmp4C7.zero_term;_LL2AB:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4C8)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4C8)== (void*)1;
goto _LL2A9;_LL2AC:;_LL2AD:({void*_tmp4CA[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4CB="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp4CB,sizeof(char),_get_zero_arr_size(_tmp4CB,42));}),
_tag_dynforward(_tmp4CA,sizeof(void*),0));});_LL2A9:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B0->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp4AF);{
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_var_exp(max,0);if(_tmp4B1)_tmp4CC=Cyc_Absyn_add_exp(
_tmp4CC,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp4AE,Cyc_Absyn_var_exp(max,0),_tmp4B0,_tmp4B1,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp4CD=_new_region("r");struct _RegionHandle*r=& _tmp4CD;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4DE=
_region_malloc(r,sizeof(*_tmp4DE));_tmp4DE->hd=({struct _tuple11*_tmp4DF=
_region_malloc(r,sizeof(*_tmp4DF));_tmp4DF->f1=max;_tmp4DF->f2=Cyc_Absyn_uint_typ;
_tmp4DF->f3=(struct Cyc_Absyn_Exp*)_tmp4AF;_tmp4DF;});_tmp4DE->tl=0;_tmp4DE;});
struct Cyc_Absyn_Exp*ai;if(_tmp3AE == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp4CE[2];_tmp4CE[1]=_tmp4CC;_tmp4CE[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp4CE,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp3AE;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp4CF[2];_tmp4CF[1]=_tmp4CC;
_tmp4CF[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4CF,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0));_tmp4D0->hd=({
struct _tuple11*_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1));_tmp4D1->f1=a;_tmp4D1->f2=
ptr_typ;_tmp4D1->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp4D1;});_tmp4D0->tl=decls;
_tmp4D0;});if(is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp4D2=Cyc_Toc_temp_var();
void*_tmp4D3=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp4D4=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp4D5=Cyc_Absyn_fncall_exp(_tmp4D4,({struct Cyc_Absyn_Exp*_tmp4D8[3];_tmp4D8[2]=
_tmp4CC;_tmp4D8[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp4D8[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4D8,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6));_tmp4D6->hd=({struct _tuple11*_tmp4D7=
_region_malloc(r,sizeof(*_tmp4D7));_tmp4D7->f1=_tmp4D2;_tmp4D7->f2=_tmp4D3;
_tmp4D7->f3=(struct Cyc_Absyn_Exp*)_tmp4D5;_tmp4D7;});_tmp4D6->tl=decls;_tmp4D6;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4D2,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp4D9=decls;for(0;_tmp4D9 != 0;_tmp4D9=_tmp4D9->tl){struct
_tuple1*_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct _tuple11 _tmp4DA=*((
struct _tuple11*)_tmp4D9->hd);_tmp4DB=_tmp4DA.f1;_tmp4DC=_tmp4DA.f2;_tmp4DD=
_tmp4DA.f3;s=Cyc_Absyn_declare_stmt(_tmp4DB,_tmp4DC,_tmp4DD,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL299;}}}}_LL29E: if(*((int*)
_tmp4AC)!= 30)goto _LL2A0;_tmp4B2=((struct Cyc_Absyn_Struct_e_struct*)_tmp4AC)->f1;
_tmp4B3=((struct Cyc_Absyn_Struct_e_struct*)_tmp4AC)->f2;_tmp4B4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4AC)->f3;_LL29F:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v,_tmp4B3 != 0,1,_tmp3AE,_tmp4B4,
_tmp4B2))->r));goto _LL299;_LL2A0: if(*((int*)_tmp4AC)!= 26)goto _LL2A2;_tmp4B5=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp4AC)->f1;_LL2A1:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp3AE,_tmp4B5))->r));goto _LL299;_LL2A2:;_LL2A3: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp4E0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4E1=Cyc_Absyn_var_exp(_tmp4E0,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp4E1,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp3AE == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp3AE;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*
_tmp4E2=(void*)_tmp3AF->r;void*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;_LL2AF: if(*((
int*)_tmp4E2)!= 15)goto _LL2B1;_tmp4E3=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4E2)->f1;_tmp4E4=((struct Cyc_Absyn_Cast_e_struct*)_tmp4E2)->f2;_LL2B0:{
struct _tuple0 _tmp4E6=({struct _tuple0 _tmp4E5;_tmp4E5.f1=Cyc_Tcutil_compress(
_tmp4E3);_tmp4E5.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4E4->topt))->v);_tmp4E5;});void*_tmp4E7;struct Cyc_Absyn_PtrInfo _tmp4E8;void*
_tmp4E9;struct Cyc_Absyn_PtrAtts _tmp4EA;struct Cyc_Absyn_Conref*_tmp4EB;void*
_tmp4EC;struct Cyc_Absyn_PtrInfo _tmp4ED;struct Cyc_Absyn_PtrAtts _tmp4EE;struct Cyc_Absyn_Conref*
_tmp4EF;_LL2B4: _tmp4E7=_tmp4E6.f1;if(_tmp4E7 <= (void*)4)goto _LL2B6;if(*((int*)
_tmp4E7)!= 4)goto _LL2B6;_tmp4E8=((struct Cyc_Absyn_PointerType_struct*)_tmp4E7)->f1;
_tmp4E9=(void*)_tmp4E8.elt_typ;_tmp4EA=_tmp4E8.ptr_atts;_tmp4EB=_tmp4EA.bounds;
_tmp4EC=_tmp4E6.f2;if(_tmp4EC <= (void*)4)goto _LL2B6;if(*((int*)_tmp4EC)!= 4)goto
_LL2B6;_tmp4ED=((struct Cyc_Absyn_PointerType_struct*)_tmp4EC)->f1;_tmp4EE=
_tmp4ED.ptr_atts;_tmp4EF=_tmp4EE.bounds;_LL2B5:{struct _tuple0 _tmp4F1=({struct
_tuple0 _tmp4F0;_tmp4F0.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4EB);
_tmp4F0.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4EF);_tmp4F0;});void*
_tmp4F2;void*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F4;void*_tmp4F5;void*_tmp4F6;
struct Cyc_Absyn_Exp*_tmp4F7;_LL2B9: _tmp4F2=_tmp4F1.f1;if((int)_tmp4F2 != 0)goto
_LL2BB;_tmp4F3=_tmp4F1.f2;if(_tmp4F3 <= (void*)2)goto _LL2BB;if(*((int*)_tmp4F3)!= 
0)goto _LL2BB;_tmp4F4=((struct Cyc_Absyn_Upper_b_struct*)_tmp4F3)->f1;_LL2BA:
forward_only=1;_tmp4F7=_tmp4F4;goto _LL2BC;_LL2BB: _tmp4F5=_tmp4F1.f1;if((int)
_tmp4F5 != 1)goto _LL2BD;_tmp4F6=_tmp4F1.f2;if(_tmp4F6 <= (void*)2)goto _LL2BD;if(*((
int*)_tmp4F6)!= 0)goto _LL2BD;_tmp4F7=((struct Cyc_Absyn_Upper_b_struct*)_tmp4F6)->f1;
_LL2BC: Cyc_Toc_exp_to_c(nv,_tmp4E4);(void*)(inner_mexp->r=(void*)Cyc_Toc_sizeoftyp_exp_r(
elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp4F8=forward_only?Cyc_Toc__init_dynforward_ptr_e:
Cyc_Toc__init_dyneither_ptr_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4F8,({
struct Cyc_Absyn_Exp*_tmp4F9[4];_tmp4F9[3]=_tmp4F7;_tmp4F9[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp4E9),0);_tmp4F9[1]=_tmp4E4;_tmp4F9[0]=mexp;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4F9,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2B8;}_LL2BD:;_LL2BE: goto _LL2B8;_LL2B8:;}goto _LL2B3;_LL2B6:;
_LL2B7: goto _LL2B3;_LL2B3:;}goto _LL2AE;_LL2B1:;_LL2B2: goto _LL2AE;_LL2AE:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp4FA=Cyc_Absyn_exp_stmt(_tmp4E1,0);struct Cyc_Absyn_Exp*
_tmp4FB=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp3AF);_tmp4FA=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp4E1,_tmp4FB,0),_tmp3AF,0),
_tmp4FA,0);{void*_tmp4FC=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4E0,_tmp4FC,(struct Cyc_Absyn_Exp*)
mexp,_tmp4FA,0)));}}goto _LL299;}}_LL299:;}goto _LL1C4;_LL1ED: if(*((int*)_tmp383)
!= 19)goto _LL1EF;_tmp3B0=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp383)->f1;
_LL1EE: Cyc_Toc_exp_to_c(nv,_tmp3B0);goto _LL1C4;_LL1EF: if(*((int*)_tmp383)!= 18)
goto _LL1F1;_tmp3B1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp383)->f1;
_LL1F0:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4FD=
_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp4FE;_tmp4FE.tag=18;_tmp4FE.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3B1);_tmp4FE;});
_tmp4FD;})));goto _LL1C4;_LL1F1: if(*((int*)_tmp383)!= 21)goto _LL1F3;_LL1F2:({void*
_tmp4FF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp500="__gen() in code generator";_tag_dynforward(_tmp500,sizeof(
char),_get_zero_arr_size(_tmp500,26));}),_tag_dynforward(_tmp4FF,sizeof(void*),0));});
_LL1F3: if(*((int*)_tmp383)!= 20)goto _LL1F5;_tmp3B2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp383)->f1;_tmp3B3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp383)->f2;
if(*((int*)_tmp3B3)!= 0)goto _LL1F5;_tmp3B4=((struct Cyc_Absyn_StructField_struct*)
_tmp3B3)->f1;_LL1F4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp502;_tmp502.tag=20;_tmp502.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3B2);_tmp502.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp503=_cycalloc(sizeof(*
_tmp503));_tmp503[0]=({struct Cyc_Absyn_StructField_struct _tmp504;_tmp504.tag=0;
_tmp504.f1=_tmp3B4;_tmp504;});_tmp503;}));_tmp502;});_tmp501;})));goto _LL1C4;
_LL1F5: if(*((int*)_tmp383)!= 20)goto _LL1F7;_tmp3B5=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp383)->f1;_tmp3B6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp383)->f2;
if(*((int*)_tmp3B6)!= 1)goto _LL1F7;_tmp3B7=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp3B6)->f1;_LL1F6:{void*_tmp505=Cyc_Tcutil_compress(_tmp3B5);struct Cyc_Absyn_AggrInfo
_tmp506;union Cyc_Absyn_AggrInfoU_union _tmp507;struct Cyc_List_List*_tmp508;_LL2C0:
if(_tmp505 <= (void*)4)goto _LL2C8;if(*((int*)_tmp505)!= 10)goto _LL2C2;_tmp506=((
struct Cyc_Absyn_AggrType_struct*)_tmp505)->f1;_tmp507=_tmp506.aggr_info;_LL2C1: {
struct Cyc_Absyn_Aggrdecl*_tmp509=Cyc_Absyn_get_known_aggrdecl(_tmp507);if(
_tmp509->impl == 0)({void*_tmp50A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp50B="struct fields must be known";
_tag_dynforward(_tmp50B,sizeof(char),_get_zero_arr_size(_tmp50B,28));}),
_tag_dynforward(_tmp50A,sizeof(void*),0));});_tmp508=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp509->impl))->fields;goto _LL2C3;}_LL2C2: if(*((int*)_tmp505)!= 11)
goto _LL2C4;_tmp508=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp505)->f2;_LL2C3: {
struct Cyc_Absyn_Aggrfield*_tmp50C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp508,(int)_tmp3B7);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp50E;_tmp50E.tag=20;_tmp50E.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp3B5);_tmp50E.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp50F=
_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({struct Cyc_Absyn_StructField_struct
_tmp510;_tmp510.tag=0;_tmp510.f1=_tmp50C->name;_tmp510;});_tmp50F;}));_tmp50E;});
_tmp50D;})));goto _LL2BF;}_LL2C4: if(*((int*)_tmp505)!= 9)goto _LL2C6;_LL2C5:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp511=_cycalloc(
sizeof(*_tmp511));_tmp511[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp512;_tmp512.tag=
20;_tmp512.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3B5);_tmp512.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513[0]=({
struct Cyc_Absyn_StructField_struct _tmp514;_tmp514.tag=0;_tmp514.f1=Cyc_Absyn_fieldname((
int)(_tmp3B7 + 1));_tmp514;});_tmp513;}));_tmp512;});_tmp511;})));goto _LL2BF;
_LL2C6: if(*((int*)_tmp505)!= 3)goto _LL2C8;_LL2C7: if(_tmp3B7 == 0)(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp515=_cycalloc(sizeof(*
_tmp515));_tmp515[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp516;_tmp516.tag=20;
_tmp516.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3B5);_tmp516.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517[0]=({
struct Cyc_Absyn_StructField_struct _tmp518;_tmp518.tag=0;_tmp518.f1=Cyc_Toc_tag_sp;
_tmp518;});_tmp517;}));_tmp516;});_tmp515;})));else{(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp51A;_tmp51A.tag=20;_tmp51A.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp3B5);_tmp51A.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp51B=
_cycalloc(sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_Absyn_StructField_struct
_tmp51C;_tmp51C.tag=0;_tmp51C.f1=Cyc_Absyn_fieldname((int)_tmp3B7);_tmp51C;});
_tmp51B;}));_tmp51A;});_tmp519;})));}goto _LL2BF;_LL2C8:;_LL2C9:({void*_tmp51D[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp51E="impossible type for offsetof tuple index";_tag_dynforward(
_tmp51E,sizeof(char),_get_zero_arr_size(_tmp51E,41));}),_tag_dynforward(_tmp51D,
sizeof(void*),0));});_LL2BF:;}goto _LL1C4;_LL1F7: if(*((int*)_tmp383)!= 22)goto
_LL1F9;_tmp3B8=((struct Cyc_Absyn_Deref_e_struct*)_tmp383)->f1;_LL1F8: {void*
_tmp51F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B8->topt))->v);{
void*_tmp520=_tmp51F;struct Cyc_Absyn_PtrInfo _tmp521;void*_tmp522;struct Cyc_Absyn_Tqual
_tmp523;struct Cyc_Absyn_PtrAtts _tmp524;void*_tmp525;struct Cyc_Absyn_Conref*
_tmp526;struct Cyc_Absyn_Conref*_tmp527;struct Cyc_Absyn_Conref*_tmp528;_LL2CB: if(
_tmp520 <= (void*)4)goto _LL2CD;if(*((int*)_tmp520)!= 4)goto _LL2CD;_tmp521=((
struct Cyc_Absyn_PointerType_struct*)_tmp520)->f1;_tmp522=(void*)_tmp521.elt_typ;
_tmp523=_tmp521.elt_tq;_tmp524=_tmp521.ptr_atts;_tmp525=(void*)_tmp524.rgn;
_tmp526=_tmp524.nullable;_tmp527=_tmp524.bounds;_tmp528=_tmp524.zero_term;_LL2CC:{
void*_tmp529=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp527);_LL2D0: if(
_tmp529 <= (void*)2)goto _LL2D2;if(*((int*)_tmp529)!= 0)goto _LL2D2;_LL2D1: {int
do_null_check=Cyc_Toc_need_null_check(_tmp3B8);Cyc_Toc_exp_to_c(nv,_tmp3B8);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp52A[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp52B="inserted null check due to dereference";
_tag_dynforward(_tmp52B,sizeof(char),_get_zero_arr_size(_tmp52B,39));}),
_tag_dynforward(_tmp52A,sizeof(void*),0));});(void*)(_tmp3B8->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c(_tmp51F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->hd=Cyc_Absyn_copy_exp(_tmp3B8);
_tmp52C->tl=0;_tmp52C;}),0)));}goto _LL2CF;}_LL2D2: if((int)_tmp529 != 0)goto _LL2D4;
_LL2D3: goto _LL2D5;_LL2D4: if((int)_tmp529 != 1)goto _LL2D6;_LL2D5: {struct Cyc_Absyn_Exp*
_tmp52D=Cyc_Absyn_uint_exp(0,0);_tmp52D->topt=({struct Cyc_Core_Opt*_tmp52E=
_cycalloc(sizeof(*_tmp52E));_tmp52E->v=(void*)Cyc_Absyn_uint_typ;_tmp52E;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp3B8,_tmp52D));Cyc_Toc_exp_to_c(nv,
e);goto _LL2CF;}_LL2D6: if(_tmp529 <= (void*)2)goto _LL2CF;if(*((int*)_tmp529)!= 1)
goto _LL2CF;_LL2D7:({void*_tmp52F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp530="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp530,sizeof(char),_get_zero_arr_size(_tmp530,30));}),
_tag_dynforward(_tmp52F,sizeof(void*),0));});_LL2CF:;}goto _LL2CA;_LL2CD:;_LL2CE:({
void*_tmp531[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp532="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp532,sizeof(char),_get_zero_arr_size(_tmp532,29));}),
_tag_dynforward(_tmp531,sizeof(void*),0));});_LL2CA:;}goto _LL1C4;}_LL1F9: if(*((
int*)_tmp383)!= 23)goto _LL1FB;_tmp3B9=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp383)->f1;_tmp3BA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp383)->f2;_LL1FA:
Cyc_Toc_exp_to_c(nv,_tmp3B9);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1C4;
_LL1FB: if(*((int*)_tmp383)!= 24)goto _LL1FD;_tmp3BB=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp383)->f1;_tmp3BC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp383)->f2;_LL1FC: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3BB->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp3BB);Cyc_Toc_exp_to_c(nv,_tmp3BB);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp534;struct Cyc_Absyn_Tqual
_tmp535;struct Cyc_Absyn_PtrAtts _tmp536;void*_tmp537;struct Cyc_Absyn_Conref*
_tmp538;struct Cyc_Absyn_Conref*_tmp539;struct Cyc_Absyn_Conref*_tmp53A;struct Cyc_Absyn_PtrInfo
_tmp533=Cyc_Toc_get_ptr_type(e1typ);_tmp534=(void*)_tmp533.elt_typ;_tmp535=
_tmp533.elt_tq;_tmp536=_tmp533.ptr_atts;_tmp537=(void*)_tmp536.rgn;_tmp538=
_tmp536.nullable;_tmp539=_tmp536.bounds;_tmp53A=_tmp536.zero_term;{void*_tmp53B=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp539);struct Cyc_Absyn_Exp*_tmp53C;
_LL2D9: if(_tmp53B <= (void*)2)goto _LL2DB;if(*((int*)_tmp53B)!= 0)goto _LL2DB;
_tmp53C=((struct Cyc_Absyn_Upper_b_struct*)_tmp53B)->f1;_LL2DA: {unsigned int
_tmp53E;int _tmp53F;struct _tuple5 _tmp53D=Cyc_Evexp_eval_const_uint_exp(_tmp53C);
_tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;if(_tmp53F){if(_tmp53E < 1)({void*_tmp540[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp541="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp541,sizeof(char),_get_zero_arr_size(_tmp541,44));}),_tag_dynforward(_tmp540,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp542[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp543="inserted null check due to dereference";
_tag_dynforward(_tmp543,sizeof(char),_get_zero_arr_size(_tmp543,39));}),
_tag_dynforward(_tmp542,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3BB->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->hd=_tmp3BB;_tmp544->tl=0;_tmp544;}),
0)),_tmp3BC));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3BB->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp545[4];_tmp545[3]=Cyc_Absyn_uint_exp(0,0);_tmp545[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp534,0);_tmp545[1]=_tmp53C;_tmp545[0]=_tmp3BB;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp545,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp3BC));}goto _LL2D8;}_LL2DB: if((int)_tmp53B != 0)goto _LL2DD;_LL2DC:
forward_only=1;goto _LL2DE;_LL2DD: if((int)_tmp53B != 1)goto _LL2DF;_LL2DE: {struct
Cyc_Absyn_Exp*_tmp546=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp534);(void*)(_tmp3BB->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp535),Cyc_Absyn_fncall_exp(_tmp546,({struct Cyc_Absyn_Exp*
_tmp547[3];_tmp547[2]=Cyc_Absyn_uint_exp(0,0);_tmp547[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp547[0]=Cyc_Absyn_copy_exp(_tmp3BB);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp547,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL2D8;}_LL2DF: if(_tmp53B <= (void*)2)goto _LL2D8;if(*((int*)
_tmp53B)!= 1)goto _LL2D8;_LL2E0:({void*_tmp548[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp549="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp549,sizeof(char),_get_zero_arr_size(_tmp549,34));}),
_tag_dynforward(_tmp548,sizeof(void*),0));});_LL2D8:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1C4;}}
_LL1FD: if(*((int*)_tmp383)!= 25)goto _LL1FF;_tmp3BD=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp383)->f1;_tmp3BE=((struct Cyc_Absyn_Subscript_e_struct*)_tmp383)->f2;_LL1FE: {
void*_tmp54A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3BD->topt))->v);{void*_tmp54B=_tmp54A;struct Cyc_List_List*_tmp54C;struct Cyc_Absyn_PtrInfo
_tmp54D;void*_tmp54E;struct Cyc_Absyn_Tqual _tmp54F;struct Cyc_Absyn_PtrAtts _tmp550;
void*_tmp551;struct Cyc_Absyn_Conref*_tmp552;struct Cyc_Absyn_Conref*_tmp553;
struct Cyc_Absyn_Conref*_tmp554;_LL2E2: if(_tmp54B <= (void*)4)goto _LL2E6;if(*((int*)
_tmp54B)!= 9)goto _LL2E4;_tmp54C=((struct Cyc_Absyn_TupleType_struct*)_tmp54B)->f1;
_LL2E3: Cyc_Toc_exp_to_c(nv,_tmp3BD);Cyc_Toc_exp_to_c(nv,_tmp3BE);{unsigned int
_tmp556;int _tmp557;struct _tuple5 _tmp555=Cyc_Evexp_eval_const_uint_exp(_tmp3BE);
_tmp556=_tmp555.f1;_tmp557=_tmp555.f2;if(!_tmp557)({void*_tmp558[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp559="unknown tuple subscript in translation to C";_tag_dynforward(_tmp559,
sizeof(char),_get_zero_arr_size(_tmp559,44));}),_tag_dynforward(_tmp558,sizeof(
void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp3BD,Cyc_Absyn_fieldname((
int)(_tmp556 + 1))));goto _LL2E1;}_LL2E4: if(*((int*)_tmp54B)!= 4)goto _LL2E6;
_tmp54D=((struct Cyc_Absyn_PointerType_struct*)_tmp54B)->f1;_tmp54E=(void*)
_tmp54D.elt_typ;_tmp54F=_tmp54D.elt_tq;_tmp550=_tmp54D.ptr_atts;_tmp551=(void*)
_tmp550.rgn;_tmp552=_tmp550.nullable;_tmp553=_tmp550.bounds;_tmp554=_tmp550.zero_term;
_LL2E5: {struct Cyc_List_List*_tmp55A=Cyc_Toc_get_relns(_tmp3BD);int in_bnds=0;{
void*_tmp55B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp553);_LL2E9: if(
_tmp55B <= (void*)2)goto _LL2EB;if(*((int*)_tmp55B)!= 1)goto _LL2EB;_LL2EA: goto
_LL2E8;_LL2EB:;_LL2EC: in_bnds=Cyc_Toc_check_bounds(_tmp55A,_tmp3BD,_tmp3BE);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp55E;
_tmp55E.tag=0;_tmp55E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp55C[1]={& _tmp55E};Cyc_Tcutil_warn(e->loc,({const char*_tmp55D="bounds check necessary for %s";
_tag_dynforward(_tmp55D,sizeof(char),_get_zero_arr_size(_tmp55D,30));}),
_tag_dynforward(_tmp55C,sizeof(void*),1));}});_LL2E8:;}Cyc_Toc_exp_to_c(nv,
_tmp3BD);Cyc_Toc_exp_to_c(nv,_tmp3BE);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp55F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp553);
struct Cyc_Absyn_Exp*_tmp560;void*_tmp561;_LL2EE: if(_tmp55F <= (void*)2)goto _LL2F0;
if(*((int*)_tmp55F)!= 0)goto _LL2F0;_tmp560=((struct Cyc_Absyn_Upper_b_struct*)
_tmp55F)->f1;_LL2EF: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp552);void*ta1=Cyc_Toc_typ_to_c(_tmp54E);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp54F);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp554))(void*)(e->r=(void*)
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp562[3];_tmp562[2]=_tmp3BE;_tmp562[1]=_tmp560;_tmp562[0]=
_tmp3BD;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp562,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp563[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp564="inserted null check due to dereference";
_tag_dynforward(_tmp564,sizeof(char),_get_zero_arr_size(_tmp564,39));}),
_tag_dynforward(_tmp563,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp565[4];_tmp565[3]=_tmp3BE;_tmp565[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp565[1]=_tmp560;_tmp565[0]=_tmp3BD;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp565,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp3BE->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp566[2];_tmp566[1]=Cyc_Absyn_copy_exp(_tmp3BE);_tmp566[0]=
_tmp560;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp566,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL2ED;}
_LL2F0: if((int)_tmp55F != 0)goto _LL2F2;_LL2F1: forward_only=1;goto _LL2F3;_LL2F2:
if((int)_tmp55F != 1)goto _LL2F4;_LL2F3: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp54E);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp54F),Cyc_Absyn_aggrmember_exp(
_tmp3BD,Cyc_Toc_curr_sp,0)),_tmp3BE));}else{struct Cyc_Absyn_Exp*_tmp567=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp54F),Cyc_Absyn_fncall_exp(_tmp567,({struct Cyc_Absyn_Exp*_tmp568[3];_tmp568[2]=
_tmp3BE;_tmp568[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp568[0]=_tmp3BD;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp568,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL2ED;}_LL2F4: if(_tmp55F <= (void*)2)goto _LL2ED;if(*((int*)
_tmp55F)!= 1)goto _LL2ED;_tmp561=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp55F)->f1;_LL2F5:{void*_tmp569=(void*)_tmp3BE->annot;struct Cyc_List_List*
_tmp56A;_LL2F7: if(*((void**)_tmp569)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL2F9;
_tmp56A=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp569)->f1;_LL2F8: for(0;
_tmp56A != 0;_tmp56A=_tmp56A->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp56A->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp56A->hd)->bd,_tmp561)== 0){in_bnds=1;break;}}goto _LL2F6;_LL2F9:;_LL2FA: goto
_LL2F6;_LL2F6:;}if(!in_bnds)({void*_tmp56B[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp56C="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp56C,sizeof(char),_get_zero_arr_size(_tmp56C,45));}),_tag_dynforward(_tmp56B,
sizeof(void*),0));});goto _LL2ED;_LL2ED:;}goto _LL2E1;}}_LL2E6:;_LL2E7:({void*
_tmp56D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp56E="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp56E,sizeof(char),_get_zero_arr_size(_tmp56E,49));}),
_tag_dynforward(_tmp56D,sizeof(void*),0));});_LL2E1:;}goto _LL1C4;}_LL1FF: if(*((
int*)_tmp383)!= 26)goto _LL201;_tmp3BF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp383)->f1;
_LL200: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp3BF))->r));else{struct Cyc_List_List*_tmp56F=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp3BF);void*_tmp570=Cyc_Toc_add_tuple_type(_tmp56F);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp3BF != 0;(_tmp3BF=_tmp3BF->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp3BF->hd);dles=({struct Cyc_List_List*_tmp571=_cycalloc(
sizeof(*_tmp571));_tmp571->hd=({struct _tuple8*_tmp572=_cycalloc(sizeof(*_tmp572));
_tmp572->f1=0;_tmp572->f2=(struct Cyc_Absyn_Exp*)_tmp3BF->hd;_tmp572;});_tmp571->tl=
dles;_tmp571;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));}goto _LL1C4;
_LL201: if(*((int*)_tmp383)!= 28)goto _LL203;_tmp3C0=((struct Cyc_Absyn_Array_e_struct*)
_tmp383)->f1;_LL202:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp3C0));{
struct Cyc_List_List*_tmp573=_tmp3C0;for(0;_tmp573 != 0;_tmp573=_tmp573->tl){
struct _tuple8 _tmp575;struct Cyc_Absyn_Exp*_tmp576;struct _tuple8*_tmp574=(struct
_tuple8*)_tmp573->hd;_tmp575=*_tmp574;_tmp576=_tmp575.f2;Cyc_Toc_exp_to_c(nv,
_tmp576);}}goto _LL1C4;_LL203: if(*((int*)_tmp383)!= 29)goto _LL205;_tmp3C1=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp383)->f1;_tmp3C2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp383)->f2;_tmp3C3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp383)->f3;
_tmp3C4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp383)->f4;_LL204: {
unsigned int _tmp578;int _tmp579;struct _tuple5 _tmp577=Cyc_Evexp_eval_const_uint_exp(
_tmp3C2);_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;{void*_tmp57A=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3C3->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3C3);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp3C3)){if(!_tmp579)({
void*_tmp57B[0]={};Cyc_Tcutil_terr(_tmp3C2->loc,({const char*_tmp57C="cannot determine value of constant";
_tag_dynforward(_tmp57C,sizeof(char),_get_zero_arr_size(_tmp57C,35));}),
_tag_dynforward(_tmp57B,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp578;i
++){es=({struct Cyc_List_List*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->hd=({
struct _tuple8*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->f1=0;_tmp57E->f2=
_tmp3C3;_tmp57E;});_tmp57D->tl=es;_tmp57D;});}}if(_tmp3C4){struct Cyc_Absyn_Exp*
_tmp57F=Cyc_Toc_cast_it(_tmp57A,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->hd=({struct _tuple8*_tmp581=
_cycalloc(sizeof(*_tmp581));_tmp581->f1=0;_tmp581->f2=_tmp57F;_tmp581;});_tmp580->tl=
0;_tmp580;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1C4;}}}_LL205: if(*((int*)_tmp383)!= 30)goto _LL207;_tmp3C5=((struct Cyc_Absyn_Struct_e_struct*)
_tmp383)->f1;_tmp3C6=((struct Cyc_Absyn_Struct_e_struct*)_tmp383)->f2;_tmp3C7=((
struct Cyc_Absyn_Struct_e_struct*)_tmp383)->f3;_tmp3C8=((struct Cyc_Absyn_Struct_e_struct*)
_tmp383)->f4;_LL206: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp3C6 != 0,0,0,_tmp3C7,_tmp3C5))->r));else{if(_tmp3C8 == 0)({void*
_tmp582[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp583="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp583,
sizeof(char),_get_zero_arr_size(_tmp583,35));}),_tag_dynforward(_tmp582,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp3C8;
struct _RegionHandle _tmp584=_new_region("rgn");struct _RegionHandle*rgn=& _tmp584;
_push_region(rgn);{struct Cyc_List_List*_tmp585=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp3C7,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp586=0;struct Cyc_List_List*_tmp587=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp587 != 0;_tmp587=_tmp587->tl){struct Cyc_List_List*
_tmp588=_tmp585;for(0;_tmp588 != 0;_tmp588=_tmp588->tl){if((*((struct _tuple12*)
_tmp588->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp587->hd){struct _tuple12
_tmp58A;struct Cyc_Absyn_Aggrfield*_tmp58B;struct Cyc_Absyn_Exp*_tmp58C;struct
_tuple12*_tmp589=(struct _tuple12*)_tmp588->hd;_tmp58A=*_tmp589;_tmp58B=_tmp58A.f1;
_tmp58C=_tmp58A.f2;{void*_tmp58D=(void*)_tmp58B->type;Cyc_Toc_exp_to_c(nv,
_tmp58C);if(Cyc_Toc_is_void_star(_tmp58D))(void*)(_tmp58C->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp58C->r,0)));_tmp586=({
struct Cyc_List_List*_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->hd=({struct
_tuple8*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->f1=0;_tmp58F->f2=_tmp58C;
_tmp58F;});_tmp58E->tl=_tmp586;_tmp58E;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp586)));};
_pop_region(rgn);}}goto _LL1C4;_LL207: if(*((int*)_tmp383)!= 31)goto _LL209;_tmp3C9=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp383)->f1;_tmp3CA=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp383)->f2;_LL208: {struct Cyc_List_List*fs;{void*_tmp590=Cyc_Tcutil_compress(
_tmp3C9);struct Cyc_List_List*_tmp591;_LL2FC: if(_tmp590 <= (void*)4)goto _LL2FE;if(*((
int*)_tmp590)!= 11)goto _LL2FE;_tmp591=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp590)->f2;_LL2FD: fs=_tmp591;goto _LL2FB;_LL2FE:;_LL2FF:({struct Cyc_String_pa_struct
_tmp594;_tmp594.tag=0;_tmp594.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp3C9));{void*_tmp592[1]={& _tmp594};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp593="anon struct has type %s";_tag_dynforward(_tmp593,sizeof(char),
_get_zero_arr_size(_tmp593,24));}),_tag_dynforward(_tmp592,sizeof(void*),1));}});
_LL2FB:;}{struct _RegionHandle _tmp595=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp595;_push_region(rgn);{struct Cyc_List_List*_tmp596=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp3CA,fs);for(0;_tmp596 != 0;_tmp596=_tmp596->tl){struct _tuple12 _tmp598;struct
Cyc_Absyn_Aggrfield*_tmp599;struct Cyc_Absyn_Exp*_tmp59A;struct _tuple12*_tmp597=(
struct _tuple12*)_tmp596->hd;_tmp598=*_tmp597;_tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;{
void*_tmp59B=(void*)_tmp599->type;Cyc_Toc_exp_to_c(nv,_tmp59A);if(Cyc_Toc_is_void_star(
_tmp59B))(void*)(_tmp59A->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp59A->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3CA));};_pop_region(rgn);}goto _LL1C4;}_LL209: if(*((int*)_tmp383)!= 32)goto
_LL20B;_tmp3CB=((struct Cyc_Absyn_Tunion_e_struct*)_tmp383)->f1;if(_tmp3CB != 0)
goto _LL20B;_tmp3CC=((struct Cyc_Absyn_Tunion_e_struct*)_tmp383)->f2;_tmp3CD=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp383)->f3;if(!(!_tmp3CC->is_flat))goto _LL20B;
_LL20A: {struct _tuple1*qv=_tmp3CD->name;struct Cyc_Absyn_Exp*tag_exp=_tmp3CC->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp3CC,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1C4;}_LL20B: if(*((int*)_tmp383)!= 32)goto _LL20D;
_tmp3CE=((struct Cyc_Absyn_Tunion_e_struct*)_tmp383)->f1;_tmp3CF=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp383)->f2;_tmp3D0=((struct Cyc_Absyn_Tunion_e_struct*)_tmp383)->f3;_LL20C: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp59C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp59D=Cyc_Absyn_var_exp(_tmp59C,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp3CF->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp3CF->name,({const char*_tmp59E="_union";_tag_dynforward(_tmp59E,sizeof(char),
_get_zero_arr_size(_tmp59E,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp3CF,_tmp3D0->name,
1);{struct _tuple1 _tmp5A0;struct _dynforward_ptr*_tmp5A1;struct _tuple1*_tmp59F=
_tmp3D0->name;_tmp5A0=*_tmp59F;_tmp5A1=_tmp5A0.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp59D,_tmp5A1,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3D0->name,({const char*_tmp5A2="_struct";_tag_dynforward(_tmp5A2,sizeof(char),
_get_zero_arr_size(_tmp5A2,8));})));tag_exp=_tmp3CF->is_xtunion?Cyc_Absyn_var_exp(
_tmp3D0->name,0): Cyc_Toc_tunion_tag(_tmp3CF,_tmp3D0->name,1);member_exp=_tmp59D;}{
struct Cyc_List_List*_tmp5A3=_tmp3D0->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp3CE != 0;(_tmp3CE=_tmp3CE->tl,_tmp5A3=_tmp5A3->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp3CE->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp5A3))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->hd=({
struct _tuple8*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->f1=0;_tmp5A5->f2=
cur_e;_tmp5A5;});_tmp5A4->tl=dles;_tmp5A4;});}dles=({struct Cyc_List_List*_tmp5A6=
_cycalloc(sizeof(*_tmp5A6));_tmp5A6->hd=({struct _tuple8*_tmp5A7=_cycalloc(
sizeof(*_tmp5A7));_tmp5A7->f1=0;_tmp5A7->f2=tag_exp;_tmp5A7;});_tmp5A6->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp5A6;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp3CF->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3D0->name,({const char*_tmp5A8="_struct";_tag_dynforward(_tmp5A8,sizeof(char),
_get_zero_arr_size(_tmp5A8,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp5A9=({struct Cyc_List_List*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp5AF->tl=0;_tmp5AF;});{int i=1;for(0;_tmp3CE != 0;(((_tmp3CE=_tmp3CE->tl,
i ++)),_tmp5A3=_tmp5A3->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp3CE->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp5A3))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp5AA=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp5A9=({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));
_tmp5AB->hd=_tmp5AA;_tmp5AB->tl=_tmp5A9;_tmp5AB;});}}}{struct Cyc_Absyn_Stmt*
_tmp5AC=Cyc_Absyn_exp_stmt(_tmp59D,0);struct Cyc_Absyn_Stmt*_tmp5AD=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->hd=_tmp5AC;_tmp5AE->tl=_tmp5A9;
_tmp5AE;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp59C,tunion_ctype,0,_tmp5AD,0)));}}goto _LL1C4;}}_LL20D: if(*((int*)_tmp383)!= 
33)goto _LL20F;_LL20E: goto _LL210;_LL20F: if(*((int*)_tmp383)!= 34)goto _LL211;
_LL210: goto _LL1C4;_LL211: if(*((int*)_tmp383)!= 35)goto _LL213;_tmp3D1=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp383)->f1;_tmp3D2=_tmp3D1.is_calloc;_tmp3D3=_tmp3D1.rgn;_tmp3D4=_tmp3D1.elt_type;
_tmp3D5=_tmp3D1.num_elts;_tmp3D6=_tmp3D1.fat_result;_LL212: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp3D4)));Cyc_Toc_exp_to_c(nv,_tmp3D5);if(_tmp3D6){struct
_tuple1*_tmp5B0=Cyc_Toc_temp_var();struct _tuple1*_tmp5B1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp3D2){xexp=_tmp3D5;if(_tmp3D3 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp3D3;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(
rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp5B0,0));}else{pexp=Cyc_Toc_calloc_exp(*
_tmp3D4,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp5B0,0));}rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp5B2[3];_tmp5B2[2]=Cyc_Absyn_var_exp(
_tmp5B0,0);_tmp5B2[1]=Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp5B2[0]=Cyc_Absyn_var_exp(
_tmp5B1,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp5B2,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp3D5,0);if(_tmp3D3 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp3D3;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp5B0,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp3D4,Cyc_Absyn_var_exp(_tmp5B0,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp5B3[3];_tmp5B3[2]=Cyc_Absyn_var_exp(_tmp5B0,0);_tmp5B3[1]=
Cyc_Absyn_uint_exp(1,0);_tmp5B3[0]=Cyc_Absyn_var_exp(_tmp5B1,0);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5B3,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp5B4=Cyc_Absyn_declare_stmt(_tmp5B0,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp5B1,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp5B4));}}else{if(_tmp3D3 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp3D3;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp3D4,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1C4;}_LL213: if(*((int*)_tmp383)!= 36)goto _LL215;_tmp3D7=((
struct Cyc_Absyn_Swap_e_struct*)_tmp383)->f1;_tmp3D8=((struct Cyc_Absyn_Swap_e_struct*)
_tmp383)->f2;_LL214: {int is_dynforward_ptr=0;int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D7->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dynforward_ptr,& is_dyneither_ptr))({void*_tmp5B5[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5B6="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dynforward(_tmp5B6,sizeof(char),_get_zero_arr_size(_tmp5B6,57));}),
_tag_dynforward(_tmp5B5,sizeof(void*),0));});{struct Cyc_Absyn_Exp*swap_fn;if(
is_dynforward_ptr)swap_fn=Cyc_Toc__swap_dynforward_e;else{if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{swap_fn=Cyc_Toc__swap_word_e;}}if(!Cyc_Absyn_is_lvalue(
_tmp3D7))({struct Cyc_String_pa_struct _tmp5B9;_tmp5B9.tag=0;_tmp5B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp3D7));{void*
_tmp5B7[1]={& _tmp5B9};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp5B8="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp5B8,sizeof(char),_get_zero_arr_size(_tmp5B8,30));}),
_tag_dynforward(_tmp5B7,sizeof(void*),1));}});if(!Cyc_Absyn_is_lvalue(_tmp3D8))({
struct Cyc_String_pa_struct _tmp5BC;_tmp5BC.tag=0;_tmp5BC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp3D8));{void*
_tmp5BA[1]={& _tmp5BC};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp5BB="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp5BB,sizeof(char),_get_zero_arr_size(_tmp5BB,30));}),
_tag_dynforward(_tmp5BA,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp3D7);Cyc_Toc_exp_to_c(
nv,_tmp3D8);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*
_tmp5BD[2];_tmp5BD[1]=Cyc_Absyn_address_exp(_tmp3D8,0);_tmp5BD[0]=Cyc_Absyn_address_exp(
_tmp3D7,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp5BD,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL1C4;}}
_LL215: if(*((int*)_tmp383)!= 38)goto _LL217;_tmp3D9=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp383)->f1;_LL216: Cyc_Toc_stmt_to_c(nv,_tmp3D9);goto _LL1C4;_LL217: if(*((int*)
_tmp383)!= 37)goto _LL219;_LL218:({void*_tmp5BE[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5BF="UnresolvedMem";_tag_dynforward(_tmp5BF,sizeof(char),_get_zero_arr_size(
_tmp5BF,14));}),_tag_dynforward(_tmp5BE,sizeof(void*),0));});_LL219: if(*((int*)
_tmp383)!= 27)goto _LL1C4;_LL21A:({void*_tmp5C0[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5C1="compoundlit";
_tag_dynforward(_tmp5C1,sizeof(char),_get_zero_arr_size(_tmp5C1,12));}),
_tag_dynforward(_tmp5C0,sizeof(void*),0));});_LL1C4:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple13{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple14{struct _tuple1*f1;void*f2;};struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple13 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp5C2=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp5C3;struct Cyc_Absyn_Vardecl
_tmp5C4;struct _tuple1*_tmp5C5;struct Cyc_Absyn_Pat*_tmp5C6;struct Cyc_Absyn_Vardecl*
_tmp5C7;struct Cyc_Absyn_Vardecl _tmp5C8;struct _tuple1*_tmp5C9;struct Cyc_Absyn_Vardecl*
_tmp5CA;struct Cyc_Absyn_Pat*_tmp5CB;void*_tmp5CC;int _tmp5CD;char _tmp5CE;struct
_dynforward_ptr _tmp5CF;struct Cyc_Absyn_Enumdecl*_tmp5D0;struct Cyc_Absyn_Enumfield*
_tmp5D1;void*_tmp5D2;struct Cyc_Absyn_Enumfield*_tmp5D3;struct Cyc_Absyn_Tuniondecl*
_tmp5D4;struct Cyc_Absyn_Tunionfield*_tmp5D5;struct Cyc_List_List*_tmp5D6;struct
Cyc_Absyn_Pat*_tmp5D7;struct Cyc_Absyn_Pat _tmp5D8;void*_tmp5D9;struct Cyc_Absyn_Tuniondecl*
_tmp5DA;struct Cyc_Absyn_Tunionfield*_tmp5DB;struct Cyc_List_List*_tmp5DC;struct
Cyc_Absyn_Tuniondecl*_tmp5DD;struct Cyc_Absyn_Tunionfield*_tmp5DE;struct Cyc_List_List*
_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5E1;struct Cyc_Absyn_AggrInfo
_tmp5E2;union Cyc_Absyn_AggrInfoU_union _tmp5E3;struct Cyc_List_List*_tmp5E4;struct
Cyc_Absyn_Pat*_tmp5E5;_LL301: if(_tmp5C2 <= (void*)2)goto _LL305;if(*((int*)_tmp5C2)
!= 0)goto _LL303;_tmp5C3=((struct Cyc_Absyn_Var_p_struct*)_tmp5C2)->f1;_tmp5C4=*
_tmp5C3;_tmp5C5=_tmp5C4.name;_tmp5C6=((struct Cyc_Absyn_Var_p_struct*)_tmp5C2)->f2;
_LL302: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp5C5,r),rgn,t,r,path,
_tmp5C6,tag_fail_stmt,fail_stmt,decls);_LL303: if(*((int*)_tmp5C2)!= 2)goto _LL305;
_tmp5C7=((struct Cyc_Absyn_TagInt_p_struct*)_tmp5C2)->f2;_tmp5C8=*_tmp5C7;_tmp5C9=
_tmp5C8.name;_LL304: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp5C9,r);goto _LL306;_LL305:
if((int)_tmp5C2 != 0)goto _LL307;_LL306: s=Cyc_Toc_skip_stmt_dl();goto _LL300;_LL307:
if(_tmp5C2 <= (void*)2)goto _LL309;if(*((int*)_tmp5C2)!= 1)goto _LL309;_tmp5CA=((
struct Cyc_Absyn_Reference_p_struct*)_tmp5C2)->f1;_tmp5CB=((struct Cyc_Absyn_Reference_p_struct*)
_tmp5C2)->f2;_LL308: {struct _tuple1*_tmp5E6=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp5E7=_region_malloc(rgn,sizeof(*_tmp5E7));_tmp5E7->hd=({struct _tuple14*
_tmp5E8=_region_malloc(rgn,sizeof(*_tmp5E8));_tmp5E8->f1=_tmp5E6;_tmp5E8->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp5E8;});_tmp5E7->tl=decls;_tmp5E7;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp5CA->name,Cyc_Absyn_var_exp(_tmp5E6,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp5E6,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple13 _tmp5E9=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp5CB,tag_fail_stmt,fail_stmt,decls);_tmp5E9.f3=Cyc_Absyn_seq_stmt(
s,_tmp5E9.f3,0);return _tmp5E9;}}_LL309: if((int)_tmp5C2 != 1)goto _LL30B;_LL30A: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL300;_LL30B:
if(_tmp5C2 <= (void*)2)goto _LL30D;if(*((int*)_tmp5C2)!= 7)goto _LL30D;_tmp5CC=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp5C2)->f1;_tmp5CD=((struct Cyc_Absyn_Int_p_struct*)
_tmp5C2)->f2;_LL30C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp5CC,_tmp5CD,0),
fail_stmt);goto _LL300;_LL30D: if(_tmp5C2 <= (void*)2)goto _LL30F;if(*((int*)_tmp5C2)
!= 8)goto _LL30F;_tmp5CE=((struct Cyc_Absyn_Char_p_struct*)_tmp5C2)->f1;_LL30E: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp5CE,0),fail_stmt);goto _LL300;_LL30F:
if(_tmp5C2 <= (void*)2)goto _LL311;if(*((int*)_tmp5C2)!= 9)goto _LL311;_tmp5CF=((
struct Cyc_Absyn_Float_p_struct*)_tmp5C2)->f1;_LL310: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp5CF,0),fail_stmt);goto _LL300;_LL311: if(_tmp5C2 <= (void*)2)goto _LL313;if(*((
int*)_tmp5C2)!= 10)goto _LL313;_tmp5D0=((struct Cyc_Absyn_Enum_p_struct*)_tmp5C2)->f1;
_tmp5D1=((struct Cyc_Absyn_Enum_p_struct*)_tmp5C2)->f2;_LL312: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp5EA=_cycalloc(
sizeof(*_tmp5EA));_tmp5EA[0]=({struct Cyc_Absyn_Enum_e_struct _tmp5EB;_tmp5EB.tag=
33;_tmp5EB.f1=_tmp5D1->name;_tmp5EB.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5D0;
_tmp5EB.f3=(struct Cyc_Absyn_Enumfield*)_tmp5D1;_tmp5EB;});_tmp5EA;}),0),
fail_stmt);goto _LL300;_LL313: if(_tmp5C2 <= (void*)2)goto _LL315;if(*((int*)_tmp5C2)
!= 11)goto _LL315;_tmp5D2=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5C2)->f1;
_tmp5D3=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5C2)->f2;_LL314: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp5EC=_cycalloc(
sizeof(*_tmp5EC));_tmp5EC[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp5ED;_tmp5ED.tag=
34;_tmp5ED.f1=_tmp5D3->name;_tmp5ED.f2=(void*)_tmp5D2;_tmp5ED.f3=(struct Cyc_Absyn_Enumfield*)
_tmp5D3;_tmp5ED;});_tmp5EC;}),0),fail_stmt);goto _LL300;_LL315: if(_tmp5C2 <= (void*)
2)goto _LL317;if(*((int*)_tmp5C2)!= 6)goto _LL317;_tmp5D4=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5C2)->f1;_tmp5D5=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5C2)->f2;_tmp5D6=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5C2)->f3;if(_tmp5D6 != 0)goto _LL317;if(!(!
_tmp5D4->is_flat))goto _LL317;_LL316: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp5D4->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp5D5->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp5D4,_tmp5D5->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL300;}_LL317: if(_tmp5C2 <= (void*)2)goto _LL319;if(*((
int*)_tmp5C2)!= 4)goto _LL319;_tmp5D7=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5C2)->f1;
_tmp5D8=*_tmp5D7;_tmp5D9=(void*)_tmp5D8.r;if(_tmp5D9 <= (void*)2)goto _LL319;if(*((
int*)_tmp5D9)!= 6)goto _LL319;_tmp5DA=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5D9)->f1;
_tmp5DB=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5D9)->f2;_tmp5DC=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5D9)->f3;if(!(_tmp5DC != 0  && !_tmp5DA->is_flat))goto _LL319;_LL318: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp5DB->name,({const
char*_tmp5FA="_struct";_tag_dynforward(_tmp5FA,sizeof(char),_get_zero_arr_size(
_tmp5FA,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp5EE=_tmp5DB->typs;
for(0;_tmp5DC != 0;(((_tmp5DC=_tmp5DC->tl,_tmp5EE=((struct Cyc_List_List*)
_check_null(_tmp5EE))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp5EF=(struct Cyc_Absyn_Pat*)
_tmp5DC->hd;if((void*)_tmp5EF->r == (void*)0)continue;{void*_tmp5F0=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5EE))->hd)).f2;struct _tuple1*
_tmp5F1=Cyc_Toc_temp_var();void*_tmp5F2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5EF->topt))->v;void*_tmp5F3=Cyc_Toc_typ_to_c(_tmp5F2);struct Cyc_Absyn_Exp*
_tmp5F4=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp5F0)))_tmp5F4=Cyc_Toc_cast_it(_tmp5F3,_tmp5F4);decls=({
struct Cyc_List_List*_tmp5F5=_region_malloc(rgn,sizeof(*_tmp5F5));_tmp5F5->hd=({
struct _tuple14*_tmp5F6=_region_malloc(rgn,sizeof(*_tmp5F6));_tmp5F6->f1=_tmp5F1;
_tmp5F6->f2=_tmp5F3;_tmp5F6;});_tmp5F5->tl=decls;_tmp5F5;});{struct _tuple13
_tmp5F7=Cyc_Toc_xlate_pat(nv,rgn,_tmp5F2,Cyc_Absyn_var_exp(_tmp5F1,0),_tmp5F4,
_tmp5EF,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5F7.f1;decls=
_tmp5F7.f2;{struct Cyc_Absyn_Stmt*_tmp5F8=_tmp5F7.f3;struct Cyc_Absyn_Stmt*_tmp5F9=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5F1,0),_tmp5F4,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5F9,_tmp5F8,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp5DA->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp5DB->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp5DA,_tmp5DB->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp5DA);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL300;}}_LL319: if(_tmp5C2 <= (void*)2)goto
_LL31B;if(*((int*)_tmp5C2)!= 6)goto _LL31B;_tmp5DD=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5C2)->f1;_tmp5DE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5C2)->f2;_tmp5DF=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5C2)->f3;if(!_tmp5DD->is_flat)goto _LL31B;
_LL31A: {struct _tuple1 _tmp5FC;struct _dynforward_ptr*_tmp5FD;struct _tuple1*
_tmp5FB=_tmp5DE->name;_tmp5FC=*_tmp5FB;_tmp5FD=_tmp5FC.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp5FD,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp5FD,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp5DD,_tmp5DE->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp5DF != 0;(_tmp5DF=_tmp5DF->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5FE=(struct Cyc_Absyn_Pat*)
_tmp5DF->hd;if((void*)_tmp5FE->r == (void*)0)continue;{struct _tuple1*_tmp5FF=Cyc_Toc_temp_var();
void*_tmp600=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5FE->topt))->v;decls=({
struct Cyc_List_List*_tmp601=_region_malloc(rgn,sizeof(*_tmp601));_tmp601->hd=({
struct _tuple14*_tmp602=_region_malloc(rgn,sizeof(*_tmp602));_tmp602->f1=_tmp5FF;
_tmp602->f2=Cyc_Toc_typ_to_c(_tmp600);_tmp602;});_tmp601->tl=decls;_tmp601;});{
struct _tuple13 _tmp603=Cyc_Toc_xlate_pat(nv,rgn,_tmp600,Cyc_Absyn_var_exp(_tmp5FF,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5FE,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp603.f1;decls=_tmp603.f2;{struct Cyc_Absyn_Stmt*
_tmp604=_tmp603.f3;struct Cyc_Absyn_Stmt*_tmp605=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5FF,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp605,_tmp604,0),0);}}}}goto _LL300;}}_LL31B: if(_tmp5C2 <= (
void*)2)goto _LL31D;if(*((int*)_tmp5C2)!= 6)goto _LL31D;_tmp5E0=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5C2)->f3;_LL31C: _tmp5E1=_tmp5E0;goto _LL31E;_LL31D: if(_tmp5C2 <= (void*)2)goto
_LL31F;if(*((int*)_tmp5C2)!= 3)goto _LL31F;_tmp5E1=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp5C2)->f1;_LL31E: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp5E1 != 0;(
_tmp5E1=_tmp5E1->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp606=(struct Cyc_Absyn_Pat*)
_tmp5E1->hd;if((void*)_tmp606->r == (void*)0)continue;{struct _tuple1*_tmp607=Cyc_Toc_temp_var();
void*_tmp608=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp606->topt))->v;decls=({
struct Cyc_List_List*_tmp609=_region_malloc(rgn,sizeof(*_tmp609));_tmp609->hd=({
struct _tuple14*_tmp60A=_region_malloc(rgn,sizeof(*_tmp60A));_tmp60A->f1=_tmp607;
_tmp60A->f2=Cyc_Toc_typ_to_c(_tmp608);_tmp60A;});_tmp609->tl=decls;_tmp609;});{
struct _tuple13 _tmp60B=Cyc_Toc_xlate_pat(nv,rgn,_tmp608,Cyc_Absyn_var_exp(_tmp607,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp606,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp60B.f1;decls=_tmp60B.f2;{struct Cyc_Absyn_Stmt*
_tmp60C=_tmp60B.f3;struct Cyc_Absyn_Stmt*_tmp60D=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp607,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp60D,_tmp60C,0),0);}}}}goto _LL300;}_LL31F: if(_tmp5C2 <= (
void*)2)goto _LL321;if(*((int*)_tmp5C2)!= 5)goto _LL321;_tmp5E2=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5C2)->f1;_tmp5E3=_tmp5E2.aggr_info;_tmp5E4=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5C2)->f3;_LL320: {struct Cyc_Absyn_Aggrdecl*_tmp60E=Cyc_Absyn_get_known_aggrdecl(
_tmp5E3);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp5E4 != 0;_tmp5E4=_tmp5E4->tl){struct
_tuple15*_tmp60F=(struct _tuple15*)_tmp5E4->hd;struct Cyc_Absyn_Pat*_tmp610=(*
_tmp60F).f2;if((void*)_tmp610->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp611=(void*)((struct Cyc_List_List*)_check_null((*_tmp60F).f1))->hd;
struct _dynforward_ptr*_tmp612;_LL32A: if(*((int*)_tmp611)!= 1)goto _LL32C;_tmp612=((
struct Cyc_Absyn_FieldName_struct*)_tmp611)->f1;_LL32B: f=_tmp612;goto _LL329;
_LL32C:;_LL32D:(int)_throw((void*)Cyc_Toc_Match_error);_LL329:;}{struct _tuple1*
_tmp613=Cyc_Toc_temp_var();void*_tmp614=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp610->topt))->v;void*_tmp615=Cyc_Toc_typ_to_c(_tmp614);decls=({struct Cyc_List_List*
_tmp616=_region_malloc(rgn,sizeof(*_tmp616));_tmp616->hd=({struct _tuple14*
_tmp617=_region_malloc(rgn,sizeof(*_tmp617));_tmp617->f1=_tmp613;_tmp617->f2=
_tmp615;_tmp617;});_tmp616->tl=decls;_tmp616;});{struct _tuple13 _tmp618=Cyc_Toc_xlate_pat(
nv,rgn,_tmp614,Cyc_Absyn_var_exp(_tmp613,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp610,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp618.f1;decls=
_tmp618.f2;{struct Cyc_Absyn_Exp*_tmp619=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp60E->impl))->fields,f)))->type))_tmp619=
Cyc_Toc_cast_it(_tmp615,_tmp619);{struct Cyc_Absyn_Stmt*_tmp61A=_tmp618.f3;struct
Cyc_Absyn_Stmt*_tmp61B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp613,0),
_tmp619,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp61B,_tmp61A,0),0);}}}}}}
goto _LL300;}_LL321: if(_tmp5C2 <= (void*)2)goto _LL323;if(*((int*)_tmp5C2)!= 4)goto
_LL323;_tmp5E5=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5C2)->f1;_LL322: {struct
_tuple1*_tmp61C=Cyc_Toc_temp_var();void*_tmp61D=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5E5->topt))->v;decls=({struct Cyc_List_List*_tmp61E=
_region_malloc(rgn,sizeof(*_tmp61E));_tmp61E->hd=({struct _tuple14*_tmp61F=
_region_malloc(rgn,sizeof(*_tmp61F));_tmp61F->f1=_tmp61C;_tmp61F->f2=Cyc_Toc_typ_to_c(
_tmp61D);_tmp61F;});_tmp61E->tl=decls;_tmp61E;});{struct _tuple13 _tmp620=Cyc_Toc_xlate_pat(
nv,rgn,_tmp61D,Cyc_Absyn_var_exp(_tmp61C,0),Cyc_Absyn_deref_exp(path,0),_tmp5E5,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp620.f1;decls=_tmp620.f2;{
struct Cyc_Absyn_Stmt*_tmp621=_tmp620.f3;struct Cyc_Absyn_Stmt*_tmp622=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp61C,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp621,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp622,0);else{s=_tmp622;}goto _LL300;}}}_LL323: if(_tmp5C2 <= (void*)2)goto
_LL325;if(*((int*)_tmp5C2)!= 12)goto _LL325;_LL324:({void*_tmp623[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp624="unknownid pat";_tag_dynforward(_tmp624,sizeof(char),_get_zero_arr_size(
_tmp624,14));}),_tag_dynforward(_tmp623,sizeof(void*),0));});_LL325: if(_tmp5C2 <= (
void*)2)goto _LL327;if(*((int*)_tmp5C2)!= 13)goto _LL327;_LL326:({void*_tmp625[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp626="unknowncall pat";_tag_dynforward(_tmp626,sizeof(char),
_get_zero_arr_size(_tmp626,16));}),_tag_dynforward(_tmp625,sizeof(void*),0));});
_LL327: if(_tmp5C2 <= (void*)2)goto _LL300;if(*((int*)_tmp5C2)!= 14)goto _LL300;
_LL328:({void*_tmp627[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp628="exp pat";
_tag_dynforward(_tmp628,sizeof(char),_get_zero_arr_size(_tmp628,8));}),
_tag_dynforward(_tmp627,sizeof(void*),0));});_LL300:;}return({struct _tuple13
_tmp629;_tmp629.f1=nv;_tmp629.f2=decls;_tmp629.f3=s;_tmp629;});}struct _tuple16{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple16*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple16*_tmp62A=_region_malloc(r,sizeof(*_tmp62A));_tmp62A->f1=
Cyc_Toc_fresh_label();_tmp62A->f2=Cyc_Toc_fresh_label();_tmp62A->f3=sc;_tmp62A;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp62B=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp62C;union Cyc_Absyn_TunionInfoU_union _tmp62D;struct
Cyc_Absyn_Tuniondecl**_tmp62E;struct Cyc_Absyn_Tuniondecl*_tmp62F;_LL32F: if(
_tmp62B <= (void*)4)goto _LL331;if(*((int*)_tmp62B)!= 2)goto _LL331;_tmp62C=((
struct Cyc_Absyn_TunionType_struct*)_tmp62B)->f1;_tmp62D=_tmp62C.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp62B)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL331;_tmp62E=(_tmp62D.KnownTunion).f1;_tmp62F=*_tmp62E;if(!(!_tmp62F->is_flat))
goto _LL331;_LL330: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp630=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp62F->fields))->v;
for(0;(unsigned int)_tmp630;_tmp630=_tmp630->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp630->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL32E;}_LL331:;_LL332: goto _LL32E;_LL32E:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp631=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp632;struct Cyc_Absyn_Pat*_tmp633;struct Cyc_Absyn_Pat
_tmp634;void*_tmp635;struct Cyc_Absyn_Tuniondecl*_tmp636;struct Cyc_Absyn_Tunionfield*
_tmp637;struct Cyc_List_List*_tmp638;_LL334: if(_tmp631 <= (void*)2)goto _LL338;if(*((
int*)_tmp631)!= 0)goto _LL336;_tmp632=((struct Cyc_Absyn_Var_p_struct*)_tmp631)->f2;
_LL335: return Cyc_Toc_no_tag_test(_tmp632);_LL336: if(*((int*)_tmp631)!= 4)goto
_LL338;_tmp633=((struct Cyc_Absyn_Pointer_p_struct*)_tmp631)->f1;_tmp634=*_tmp633;
_tmp635=(void*)_tmp634.r;if(_tmp635 <= (void*)2)goto _LL338;if(*((int*)_tmp635)!= 
6)goto _LL338;_tmp636=((struct Cyc_Absyn_Tunion_p_struct*)_tmp635)->f1;_tmp637=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp635)->f2;_tmp638=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp635)->f3;if(!(_tmp638 != 0  && !_tmp636->is_flat))goto _LL338;_LL337: return 0;
_LL338:;_LL339: return 1;_LL333:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp639=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp63A=Cyc_Tcutil_compress(_tmp639);_LL33B: if(_tmp63A
<= (void*)4)goto _LL33F;if(*((int*)_tmp63A)!= 5)goto _LL33D;_LL33C: goto _LL33E;
_LL33D: if(*((int*)_tmp63A)!= 12)goto _LL33F;_LL33E: leave_as_switch=1;goto _LL33A;
_LL33F:;_LL340: leave_as_switch=0;goto _LL33A;_LL33A:;}{struct Cyc_List_List*
_tmp63B=scs;for(0;_tmp63B != 0;_tmp63B=_tmp63B->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp63B->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp63B->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp63C=scs;for(0;_tmp63C != 0;_tmp63C=_tmp63C->tl){struct Cyc_Absyn_Stmt*
_tmp63D=((struct Cyc_Absyn_Switch_clause*)_tmp63C->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp63C->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp63D,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp63E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp63E;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp63D);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp63F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp63F;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp640=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp639);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp641=lscs;for(0;_tmp641 != 0;_tmp641=_tmp641->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple16*)_tmp641->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp641->tl
== 0?end_l:(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp641->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp642=_tmp641->tl;for(0;(unsigned int)
_tmp642;_tmp642=_tmp642->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple16*)_tmp642->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp643=_region_malloc(rgn,sizeof(*_tmp643));
_tmp643[0]=Cyc_Absyn_goto_stmt((*((struct _tuple16*)_tmp642->hd)).f1,0);_tmp643;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp644=_region_malloc(rgn,sizeof(*_tmp644));_tmp644[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp644;});}{struct Cyc_Toc_Env*_tmp646;struct Cyc_List_List*_tmp647;
struct Cyc_Absyn_Stmt*_tmp648;struct _tuple13 _tmp645=Cyc_Toc_xlate_pat(_tmp640,rgn,
_tmp639,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp646=_tmp645.f1;_tmp647=_tmp645.f2;_tmp648=_tmp645.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp649=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp646,_tmp649);_tmp648=Cyc_Absyn_seq_stmt(_tmp648,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp649,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp647;nvs=({struct Cyc_List_List*_tmp64A=_region_malloc(rgn,
sizeof(*_tmp64A));_tmp64A->hd=_tmp646;_tmp64A->tl=nvs;_tmp64A;});test_stmts=({
struct Cyc_List_List*_tmp64B=_region_malloc(rgn,sizeof(*_tmp64B));_tmp64B->hd=
_tmp648;_tmp64B->tl=test_stmts;_tmp64B;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple16 _tmp64D;struct _dynforward_ptr*_tmp64E;struct _dynforward_ptr*
_tmp64F;struct Cyc_Absyn_Switch_clause*_tmp650;struct _tuple16*_tmp64C=(struct
_tuple16*)lscs->hd;_tmp64D=*_tmp64C;_tmp64E=_tmp64D.f1;_tmp64F=_tmp64D.f2;
_tmp650=_tmp64D.f3;{struct Cyc_Toc_Env*_tmp651=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp650->body;{struct _RegionHandle
_tmp652=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp652;_push_region(rgn2);
if(lscs->tl != 0){struct _tuple16 _tmp654;struct _dynforward_ptr*_tmp655;struct Cyc_Absyn_Switch_clause*
_tmp656;struct _tuple16*_tmp653=(struct _tuple16*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp654=*_tmp653;_tmp655=_tmp654.f2;_tmp656=_tmp654.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp651,end_l,_tmp655,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp656->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp651,end_l),s);};
_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp64E,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp64F,s,0),0);stmts=({struct Cyc_List_List*_tmp657=_region_malloc(rgn,sizeof(*
_tmp657));_tmp657->hd=s;_tmp657->tl=stmts;_tmp657;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple14 _tmp659;struct _tuple1*
_tmp65A;void*_tmp65B;struct _tuple14*_tmp658=(struct _tuple14*)decls->hd;_tmp659=*
_tmp658;_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;res=Cyc_Absyn_declare_stmt(_tmp65A,
_tmp65B,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp65C=
_cycalloc(sizeof(*_tmp65C));_tmp65C->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp65C->tl=0;_tmp65C;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp65D=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp65E;struct Cyc_Absyn_Stmt*_tmp65F;struct Cyc_Absyn_Stmt*_tmp660;struct Cyc_Absyn_Exp*
_tmp661;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Stmt*_tmp663;struct Cyc_Absyn_Stmt*
_tmp664;struct _tuple3 _tmp665;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Stmt*
_tmp667;struct Cyc_Absyn_Stmt*_tmp668;struct Cyc_Absyn_Stmt*_tmp669;struct Cyc_Absyn_Stmt*
_tmp66A;struct Cyc_Absyn_Exp*_tmp66B;struct _tuple3 _tmp66C;struct Cyc_Absyn_Exp*
_tmp66D;struct _tuple3 _tmp66E;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Stmt*
_tmp670;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_List_List*_tmp672;struct Cyc_List_List*
_tmp673;struct Cyc_Absyn_Switch_clause**_tmp674;struct Cyc_Absyn_Decl*_tmp675;
struct Cyc_Absyn_Stmt*_tmp676;struct _dynforward_ptr*_tmp677;struct Cyc_Absyn_Stmt*
_tmp678;struct Cyc_Absyn_Stmt*_tmp679;struct _tuple3 _tmp67A;struct Cyc_Absyn_Exp*
_tmp67B;struct Cyc_Absyn_Stmt*_tmp67C;struct Cyc_List_List*_tmp67D;struct Cyc_Absyn_Tvar*
_tmp67E;struct Cyc_Absyn_Vardecl*_tmp67F;int _tmp680;struct Cyc_Absyn_Exp*_tmp681;
struct Cyc_Absyn_Stmt*_tmp682;struct Cyc_Absyn_Exp*_tmp683;struct Cyc_Absyn_Exp*
_tmp684;struct Cyc_Absyn_Tvar*_tmp685;struct Cyc_Absyn_Vardecl*_tmp686;struct Cyc_Absyn_Stmt*
_tmp687;_LL342: if((int)_tmp65D != 0)goto _LL344;_LL343: return;_LL344: if(_tmp65D <= (
void*)1)goto _LL346;if(*((int*)_tmp65D)!= 0)goto _LL346;_tmp65E=((struct Cyc_Absyn_Exp_s_struct*)
_tmp65D)->f1;_LL345: Cyc_Toc_exp_to_c(nv,_tmp65E);return;_LL346: if(_tmp65D <= (
void*)1)goto _LL348;if(*((int*)_tmp65D)!= 1)goto _LL348;_tmp65F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp65D)->f1;_tmp660=((struct Cyc_Absyn_Seq_s_struct*)_tmp65D)->f2;_LL347: Cyc_Toc_stmt_to_c(
nv,_tmp65F);s=_tmp660;continue;_LL348: if(_tmp65D <= (void*)1)goto _LL34A;if(*((int*)
_tmp65D)!= 2)goto _LL34A;_tmp661=((struct Cyc_Absyn_Return_s_struct*)_tmp65D)->f1;
_LL349: {struct Cyc_Core_Opt*topt=0;if(_tmp661 != 0){topt=({struct Cyc_Core_Opt*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp661->topt))->v);_tmp688;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp661);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp689=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp68A=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp689,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp689,(void*)topt->v,_tmp661,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp68A,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL34A: if(_tmp65D <= (void*)1)goto _LL34C;if(*((int*)_tmp65D)!= 3)goto
_LL34C;_tmp662=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp65D)->f1;_tmp663=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp65D)->f2;_tmp664=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp65D)->f3;_LL34B: Cyc_Toc_exp_to_c(nv,_tmp662);Cyc_Toc_stmt_to_c(nv,_tmp663);s=
_tmp664;continue;_LL34C: if(_tmp65D <= (void*)1)goto _LL34E;if(*((int*)_tmp65D)!= 4)
goto _LL34E;_tmp665=((struct Cyc_Absyn_While_s_struct*)_tmp65D)->f1;_tmp666=
_tmp665.f1;_tmp667=((struct Cyc_Absyn_While_s_struct*)_tmp65D)->f2;_LL34D: Cyc_Toc_exp_to_c(
nv,_tmp666);{struct _RegionHandle _tmp68B=_new_region("temp");struct _RegionHandle*
temp=& _tmp68B;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp667);;_pop_region(temp);}return;_LL34E: if(_tmp65D <= (void*)1)goto _LL350;if(*((
int*)_tmp65D)!= 5)goto _LL350;_tmp668=((struct Cyc_Absyn_Break_s_struct*)_tmp65D)->f1;
_LL34F: {struct Cyc_Toc_Env _tmp68D;struct _dynforward_ptr**_tmp68E;struct Cyc_Toc_Env*
_tmp68C=nv;_tmp68D=*_tmp68C;_tmp68E=_tmp68D.break_lab;if(_tmp68E != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp68E,0));{int dest_depth=_tmp668 == 0?0: _tmp668->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL350: if(_tmp65D <= (
void*)1)goto _LL352;if(*((int*)_tmp65D)!= 6)goto _LL352;_tmp669=((struct Cyc_Absyn_Continue_s_struct*)
_tmp65D)->f1;_LL351: {struct Cyc_Toc_Env _tmp690;struct _dynforward_ptr**_tmp691;
struct Cyc_Toc_Env*_tmp68F=nv;_tmp690=*_tmp68F;_tmp691=_tmp690.continue_lab;if(
_tmp691 != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp691,0));_tmp66A=_tmp669;
goto _LL353;}_LL352: if(_tmp65D <= (void*)1)goto _LL354;if(*((int*)_tmp65D)!= 7)goto
_LL354;_tmp66A=((struct Cyc_Absyn_Goto_s_struct*)_tmp65D)->f2;_LL353: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp66A))->try_depth,s);
return;_LL354: if(_tmp65D <= (void*)1)goto _LL356;if(*((int*)_tmp65D)!= 8)goto
_LL356;_tmp66B=((struct Cyc_Absyn_For_s_struct*)_tmp65D)->f1;_tmp66C=((struct Cyc_Absyn_For_s_struct*)
_tmp65D)->f2;_tmp66D=_tmp66C.f1;_tmp66E=((struct Cyc_Absyn_For_s_struct*)_tmp65D)->f3;
_tmp66F=_tmp66E.f1;_tmp670=((struct Cyc_Absyn_For_s_struct*)_tmp65D)->f4;_LL355:
Cyc_Toc_exp_to_c(nv,_tmp66B);Cyc_Toc_exp_to_c(nv,_tmp66D);Cyc_Toc_exp_to_c(nv,
_tmp66F);{struct _RegionHandle _tmp692=_new_region("temp");struct _RegionHandle*
temp=& _tmp692;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp670);;_pop_region(temp);}return;_LL356: if(_tmp65D <= (void*)1)goto _LL358;if(*((
int*)_tmp65D)!= 9)goto _LL358;_tmp671=((struct Cyc_Absyn_Switch_s_struct*)_tmp65D)->f1;
_tmp672=((struct Cyc_Absyn_Switch_s_struct*)_tmp65D)->f2;_LL357: Cyc_Toc_xlate_switch(
nv,s,_tmp671,_tmp672);return;_LL358: if(_tmp65D <= (void*)1)goto _LL35A;if(*((int*)
_tmp65D)!= 10)goto _LL35A;_tmp673=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp65D)->f1;
_tmp674=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp65D)->f2;_LL359: {struct Cyc_Toc_Env
_tmp694;struct Cyc_Toc_FallthruInfo*_tmp695;struct Cyc_Toc_Env*_tmp693=nv;_tmp694=*
_tmp693;_tmp695=_tmp694.fallthru_info;if(_tmp695 == 0)({void*_tmp696[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp697="fallthru in unexpected place";_tag_dynforward(_tmp697,sizeof(char),
_get_zero_arr_size(_tmp697,29));}),_tag_dynforward(_tmp696,sizeof(void*),0));});{
struct _dynforward_ptr*_tmp699;struct Cyc_List_List*_tmp69A;struct Cyc_Dict_Dict
_tmp69B;struct Cyc_Toc_FallthruInfo _tmp698=*_tmp695;_tmp699=_tmp698.label;_tmp69A=
_tmp698.binders;_tmp69B=_tmp698.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp699,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp674)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp69C=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp69A);struct Cyc_List_List*
_tmp69D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp673);
for(0;_tmp69C != 0;(_tmp69C=_tmp69C->tl,_tmp69D=_tmp69D->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp69D))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp69B,(struct _tuple1*)_tmp69C->hd),(struct Cyc_Absyn_Exp*)
_tmp69D->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}}_LL35A: if(
_tmp65D <= (void*)1)goto _LL35C;if(*((int*)_tmp65D)!= 11)goto _LL35C;_tmp675=((
struct Cyc_Absyn_Decl_s_struct*)_tmp65D)->f1;_tmp676=((struct Cyc_Absyn_Decl_s_struct*)
_tmp65D)->f2;_LL35B:{void*_tmp69E=(void*)_tmp675->r;struct Cyc_Absyn_Vardecl*
_tmp69F;struct Cyc_Absyn_Pat*_tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_List_List*
_tmp6A2;struct Cyc_Absyn_Fndecl*_tmp6A3;_LL369: if(_tmp69E <= (void*)2)goto _LL371;
if(*((int*)_tmp69E)!= 0)goto _LL36B;_tmp69F=((struct Cyc_Absyn_Var_d_struct*)
_tmp69E)->f1;_LL36A:{struct _RegionHandle _tmp6A4=_new_region("temp");struct
_RegionHandle*temp=& _tmp6A4;_push_region(temp);{struct Cyc_Toc_Env*_tmp6A5=Cyc_Toc_add_varmap(
temp,nv,_tmp69F->name,Cyc_Absyn_varb_exp(_tmp69F->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_Local_b_struct
_tmp6A7;_tmp6A7.tag=3;_tmp6A7.f1=_tmp69F;_tmp6A7;});_tmp6A6;}),0));Cyc_Toc_local_decl_to_c(
_tmp6A5,_tmp6A5,_tmp69F,_tmp676);};_pop_region(temp);}goto _LL368;_LL36B: if(*((
int*)_tmp69E)!= 2)goto _LL36D;_tmp6A0=((struct Cyc_Absyn_Let_d_struct*)_tmp69E)->f1;
_tmp6A1=((struct Cyc_Absyn_Let_d_struct*)_tmp69E)->f3;_LL36C:{void*_tmp6A8=(void*)
_tmp6A0->r;struct Cyc_Absyn_Vardecl*_tmp6A9;struct Cyc_Absyn_Pat*_tmp6AA;struct Cyc_Absyn_Pat
_tmp6AB;void*_tmp6AC;_LL374: if(_tmp6A8 <= (void*)2)goto _LL376;if(*((int*)_tmp6A8)
!= 0)goto _LL376;_tmp6A9=((struct Cyc_Absyn_Var_p_struct*)_tmp6A8)->f1;_tmp6AA=((
struct Cyc_Absyn_Var_p_struct*)_tmp6A8)->f2;_tmp6AB=*_tmp6AA;_tmp6AC=(void*)
_tmp6AB.r;if((int)_tmp6AC != 0)goto _LL376;_LL375: {struct _tuple1*old_name=_tmp6A9->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp6A9->name=new_name;_tmp6A9->initializer=(
struct Cyc_Absyn_Exp*)_tmp6A1;(void*)(_tmp675->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_Var_d_struct
_tmp6AE;_tmp6AE.tag=0;_tmp6AE.f1=_tmp6A9;_tmp6AE;});_tmp6AD;})));{struct
_RegionHandle _tmp6AF=_new_region("temp");struct _RegionHandle*temp=& _tmp6AF;
_push_region(temp);{struct Cyc_Toc_Env*_tmp6B0=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_Local_b_struct
_tmp6B2;_tmp6B2.tag=3;_tmp6B2.f1=_tmp6A9;_tmp6B2;});_tmp6B1;}),0));Cyc_Toc_local_decl_to_c(
_tmp6B0,nv,_tmp6A9,_tmp676);};_pop_region(temp);}goto _LL373;}_LL376:;_LL377:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp6A0,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp6A1->topt))->v,_tmp6A1,_tmp676))->r));goto _LL373;_LL373:;}goto
_LL368;_LL36D: if(*((int*)_tmp69E)!= 3)goto _LL36F;_tmp6A2=((struct Cyc_Absyn_Letv_d_struct*)
_tmp69E)->f1;_LL36E: {struct Cyc_List_List*_tmp6B3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6A2);if(_tmp6B3 == 0)({void*_tmp6B4[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp6B5="empty Letv_d";_tag_dynforward(_tmp6B5,sizeof(char),
_get_zero_arr_size(_tmp6B5,13));}),_tag_dynforward(_tmp6B4,sizeof(void*),0));});(
void*)(_tmp675->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp6B6=
_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_Var_d_struct _tmp6B7;
_tmp6B7.tag=0;_tmp6B7.f1=(struct Cyc_Absyn_Vardecl*)_tmp6B3->hd;_tmp6B7;});
_tmp6B6;})));_tmp6B3=_tmp6B3->tl;for(0;_tmp6B3 != 0;_tmp6B3=_tmp6B3->tl){struct
Cyc_Absyn_Decl*_tmp6B8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_Var_d_struct
_tmp6BA;_tmp6BA.tag=0;_tmp6BA.f1=(struct Cyc_Absyn_Vardecl*)_tmp6B3->hd;_tmp6BA;});
_tmp6B9;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp6B8,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL368;}_LL36F: if(*((int*)
_tmp69E)!= 1)goto _LL371;_tmp6A3=((struct Cyc_Absyn_Fn_d_struct*)_tmp69E)->f1;
_LL370: {struct _tuple1*_tmp6BB=_tmp6A3->name;{struct _RegionHandle _tmp6BC=
_new_region("temp");struct _RegionHandle*temp=& _tmp6BC;_push_region(temp);{struct
Cyc_Toc_Env*_tmp6BD=Cyc_Toc_add_varmap(temp,nv,_tmp6A3->name,Cyc_Absyn_var_exp(
_tmp6BB,0));Cyc_Toc_fndecl_to_c(_tmp6BD,_tmp6A3,0);Cyc_Toc_stmt_to_c(_tmp6BD,
_tmp676);};_pop_region(temp);}goto _LL368;}_LL371:;_LL372:({void*_tmp6BE[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp6BF="bad nested declaration within function";_tag_dynforward(
_tmp6BF,sizeof(char),_get_zero_arr_size(_tmp6BF,39));}),_tag_dynforward(_tmp6BE,
sizeof(void*),0));});_LL368:;}return;_LL35C: if(_tmp65D <= (void*)1)goto _LL35E;if(*((
int*)_tmp65D)!= 12)goto _LL35E;_tmp677=((struct Cyc_Absyn_Label_s_struct*)_tmp65D)->f1;
_tmp678=((struct Cyc_Absyn_Label_s_struct*)_tmp65D)->f2;_LL35D: s=_tmp678;
continue;_LL35E: if(_tmp65D <= (void*)1)goto _LL360;if(*((int*)_tmp65D)!= 13)goto
_LL360;_tmp679=((struct Cyc_Absyn_Do_s_struct*)_tmp65D)->f1;_tmp67A=((struct Cyc_Absyn_Do_s_struct*)
_tmp65D)->f2;_tmp67B=_tmp67A.f1;_LL35F:{struct _RegionHandle _tmp6C0=_new_region("temp");
struct _RegionHandle*temp=& _tmp6C0;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp679);Cyc_Toc_exp_to_c(nv,_tmp67B);;_pop_region(temp);}return;_LL360:
if(_tmp65D <= (void*)1)goto _LL362;if(*((int*)_tmp65D)!= 14)goto _LL362;_tmp67C=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp65D)->f1;_tmp67D=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp65D)->f2;_LL361: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->v=(
void*)e_typ;_tmp6C1;});{struct _RegionHandle _tmp6C2=_new_region("temp");struct
_RegionHandle*temp=& _tmp6C2;_push_region(temp);{struct Cyc_Toc_Env*_tmp6C3=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp6C3,_tmp67C);{struct Cyc_Absyn_Stmt*
_tmp6C4=Cyc_Absyn_seq_stmt(_tmp67C,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp6C5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp6C6=
Cyc_Absyn_var_exp(_tmp6C5,0);struct Cyc_Absyn_Vardecl*_tmp6C7=Cyc_Absyn_new_vardecl(
_tmp6C5,Cyc_Absyn_exn_typ,0);_tmp6C6->topt=({struct Cyc_Core_Opt*_tmp6C8=
_cycalloc(sizeof(*_tmp6C8));_tmp6C8->v=(void*)Cyc_Absyn_exn_typ;_tmp6C8;});{
struct Cyc_Absyn_Pat*_tmp6C9=({struct Cyc_Absyn_Pat*_tmp6D9=_cycalloc(sizeof(*
_tmp6D9));_tmp6D9->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp6DB=
_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({struct Cyc_Absyn_Var_p_struct _tmp6DC;
_tmp6DC.tag=0;_tmp6DC.f1=_tmp6C7;_tmp6DC.f2=({struct Cyc_Absyn_Pat*_tmp6DD=
_cycalloc(sizeof(*_tmp6DD));_tmp6DD->r=(void*)((void*)0);_tmp6DD->topt=({struct
Cyc_Core_Opt*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->v=(void*)Cyc_Absyn_exn_typ;
_tmp6DE;});_tmp6DD->loc=0;_tmp6DD;});_tmp6DC;});_tmp6DB;}));_tmp6D9->topt=({
struct Cyc_Core_Opt*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA->v=(void*)Cyc_Absyn_exn_typ;
_tmp6DA;});_tmp6D9->loc=0;_tmp6D9;});struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Absyn_throw_exp(
_tmp6C6,0);_tmp6CA->topt=({struct Cyc_Core_Opt*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));
_tmp6CB->v=(void*)((void*)0);_tmp6CB;});{struct Cyc_Absyn_Stmt*_tmp6CC=Cyc_Absyn_exp_stmt(
_tmp6CA,0);struct Cyc_Absyn_Switch_clause*_tmp6CD=({struct Cyc_Absyn_Switch_clause*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->pattern=_tmp6C9;_tmp6D6->pat_vars=({
struct Cyc_Core_Opt*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->v=({struct Cyc_List_List*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=_tmp6C7;_tmp6D8->tl=0;_tmp6D8;});
_tmp6D7;});_tmp6D6->where_clause=0;_tmp6D6->body=_tmp6CC;_tmp6D6->loc=0;_tmp6D6;});
struct Cyc_Absyn_Stmt*_tmp6CE=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp67D,({struct
Cyc_List_List*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->hd=_tmp6CD;_tmp6D5->tl=
0;_tmp6D5;})),0);Cyc_Toc_stmt_to_c(_tmp6C3,_tmp6CE);{struct Cyc_Absyn_Exp*_tmp6CF=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp6D4=_cycalloc(
sizeof(*_tmp6D4));_tmp6D4->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp6D4->tl=0;_tmp6D4;}),0);struct Cyc_Absyn_Stmt*_tmp6D0=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp6D3=
_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp6D3->tl=
0;_tmp6D3;}),0),0);struct Cyc_Absyn_Exp*_tmp6D1=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp6D2=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp6D0,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp6D1,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp6CF,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp6D2,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp6C4,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp6CE,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL362: if(_tmp65D <= (void*)1)goto _LL364;if(*((int*)
_tmp65D)!= 15)goto _LL364;_tmp67E=((struct Cyc_Absyn_Region_s_struct*)_tmp65D)->f1;
_tmp67F=((struct Cyc_Absyn_Region_s_struct*)_tmp65D)->f2;_tmp680=((struct Cyc_Absyn_Region_s_struct*)
_tmp65D)->f3;_tmp681=((struct Cyc_Absyn_Region_s_struct*)_tmp65D)->f4;_tmp682=((
struct Cyc_Absyn_Region_s_struct*)_tmp65D)->f5;_LL363: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp67F->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);{struct _RegionHandle
_tmp6DF=_new_region("temp");struct _RegionHandle*temp=& _tmp6DF;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp682);;_pop_region(
temp);}if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp682,0))->r));
else{if(_tmp681 == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,
rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp6E0->tl=0;_tmp6E0;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp6E1[1];_tmp6E1[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6E1,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp682,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp6E2[1];_tmp6E2[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6E2,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp681);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6E3[2];_tmp6E3[1]=(struct Cyc_Absyn_Exp*)_tmp681;_tmp6E3[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6E3,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp682,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6E4[1];_tmp6E4[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6E4,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}}return;}_LL364: if(_tmp65D <= (void*)1)goto _LL366;
if(*((int*)_tmp65D)!= 16)goto _LL366;_tmp683=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp65D)->f1;_LL365: if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)0));
else{Cyc_Toc_exp_to_c(nv,_tmp683);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));
_tmp6E5->hd=_tmp683;_tmp6E5->tl=0;_tmp6E5;}),0)));}return;_LL366: if(_tmp65D <= (
void*)1)goto _LL341;if(*((int*)_tmp65D)!= 17)goto _LL341;_tmp684=((struct Cyc_Absyn_Alias_s_struct*)
_tmp65D)->f1;_tmp685=((struct Cyc_Absyn_Alias_s_struct*)_tmp65D)->f2;_tmp686=((
struct Cyc_Absyn_Alias_s_struct*)_tmp65D)->f3;_tmp687=((struct Cyc_Absyn_Alias_s_struct*)
_tmp65D)->f4;_LL367: {struct _tuple1*old_name=_tmp686->name;struct _tuple1*
new_name=Cyc_Toc_temp_var();_tmp686->name=new_name;_tmp686->initializer=(struct
Cyc_Absyn_Exp*)_tmp684;(void*)(s->r=(void*)((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp6E7;_tmp6E7.tag=11;_tmp6E7.f1=({struct Cyc_Absyn_Decl*_tmp6E8=_cycalloc(
sizeof(*_tmp6E8));_tmp6E8->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({struct Cyc_Absyn_Var_d_struct
_tmp6EA;_tmp6EA.tag=0;_tmp6EA.f1=_tmp686;_tmp6EA;});_tmp6E9;}));_tmp6E8->loc=0;
_tmp6E8;});_tmp6E7.f2=_tmp687;_tmp6E7;});_tmp6E6;})));{struct _RegionHandle
_tmp6EB=_new_region("temp");struct _RegionHandle*temp=& _tmp6EB;_push_region(temp);{
struct Cyc_Toc_Env*_tmp6EC=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp6ED=_cycalloc(sizeof(*
_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_Local_b_struct _tmp6EE;_tmp6EE.tag=3;
_tmp6EE.f1=_tmp686;_tmp6EE;});_tmp6ED;}),0));Cyc_Toc_local_decl_to_c(_tmp6EC,nv,
_tmp686,_tmp687);};_pop_region(temp);}return;}_LL341:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple17{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp6EF=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp6EF;_push_region(frgn);{struct Cyc_Toc_Env*_tmp6F0=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp6F1=f->args;for(0;_tmp6F1 != 0;_tmp6F1=_tmp6F1->tl){
struct _tuple1*_tmp6F2=({struct _tuple1*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));
_tmp6F3->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp6F4;(_tmp6F4.Loc_n).tag=0;_tmp6F4;}));_tmp6F3->f2=(*((
struct _tuple17*)_tmp6F1->hd)).f1;_tmp6F3;});(*((struct _tuple17*)_tmp6F1->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp6F1->hd)).f3);_tmp6F0=Cyc_Toc_add_varmap(
frgn,_tmp6F0,_tmp6F2,Cyc_Absyn_var_exp(_tmp6F2,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp6F6;struct Cyc_Absyn_Tqual
_tmp6F7;void*_tmp6F8;int _tmp6F9;struct Cyc_Absyn_VarargInfo _tmp6F5=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp6F6=_tmp6F5.name;_tmp6F7=_tmp6F5.tq;_tmp6F8=(
void*)_tmp6F5.type;_tmp6F9=_tmp6F5.inject;{void*_tmp6FA=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp6F8,(void*)2,_tmp6F7,Cyc_Absyn_false_conref));struct _tuple1*_tmp6FB=({struct
_tuple1*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp6FF;(_tmp6FF.Loc_n).tag=
0;_tmp6FF;}));_tmp6FE->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp6F6))->v;_tmp6FE;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp6FC=
_cycalloc(sizeof(*_tmp6FC));_tmp6FC->hd=({struct _tuple17*_tmp6FD=_cycalloc(
sizeof(*_tmp6FD));_tmp6FD->f1=(struct _dynforward_ptr*)_tmp6F6->v;_tmp6FD->f2=
_tmp6F7;_tmp6FD->f3=_tmp6FA;_tmp6FD;});_tmp6FC->tl=0;_tmp6FC;}));_tmp6F0=Cyc_Toc_add_varmap(
frgn,_tmp6F0,_tmp6FB,Cyc_Absyn_var_exp(_tmp6FB,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp700=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp700 != 0;_tmp700=_tmp700->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp700->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp700->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp6F0),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp701=s;_LL379:
if((int)_tmp701 != 1)goto _LL37B;_LL37A: return(void*)2;_LL37B: if((int)_tmp701 != 4)
goto _LL37D;_LL37C: return(void*)3;_LL37D:;_LL37E: return s;_LL378:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*_tmp702=ad->name;struct
_DynRegionHandle*_tmp704;struct Cyc_Dict_Dict*_tmp705;struct Cyc_Toc_TocState
_tmp703=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp704=
_tmp703.dyn;_tmp705=_tmp703.aggrs_so_far;{struct _DynRegionFrame _tmp706;struct
_RegionHandle*d=_open_dynregion(& _tmp706,_tmp704);{int seen_defn_before;struct
_tuple7**_tmp707=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp705,_tmp702);if(_tmp707 == 0){seen_defn_before=0;{struct _tuple7*v;if((void*)
ad->kind == (void*)0)v=({struct _tuple7*_tmp708=_region_malloc(d,sizeof(*_tmp708));
_tmp708->f1=ad;_tmp708->f2=Cyc_Absyn_strctq(ad->name);_tmp708;});else{v=({struct
_tuple7*_tmp709=_region_malloc(d,sizeof(*_tmp709));_tmp709->f1=ad;_tmp709->f2=
Cyc_Absyn_unionq_typ(ad->name);_tmp709;});}*_tmp705=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp705,
_tmp702,v);}}else{struct _tuple7 _tmp70B;struct Cyc_Absyn_Aggrdecl*_tmp70C;void*
_tmp70D;struct _tuple7*_tmp70A=*_tmp707;_tmp70B=*_tmp70A;_tmp70C=_tmp70B.f1;
_tmp70D=_tmp70B.f2;if(_tmp70C->impl == 0){*_tmp705=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp705,
_tmp702,({struct _tuple7*_tmp70E=_region_malloc(d,sizeof(*_tmp70E));_tmp70E->f1=
ad;_tmp70E->f2=_tmp70D;_tmp70E;}));seen_defn_before=0;}else{seen_defn_before=1;}}(
void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp70F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp70F != 0;_tmp70F=_tmp70F->tl){((struct Cyc_Absyn_Aggrfield*)_tmp70F->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp70F->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp70F->hd)->type));}}}};_pop_dynregion(d);}}
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){struct
_DynRegionHandle*_tmp711;struct Cyc_Set_Set**_tmp712;struct Cyc_Toc_TocState
_tmp710=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp711=
_tmp710.dyn;_tmp712=_tmp710.tunions_so_far;{struct _DynRegionFrame _tmp713;struct
_RegionHandle*d=_open_dynregion(& _tmp713,_tmp711);{struct _tuple1*_tmp714=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*
_tmp712,_tmp714)){_npop_handler(0);return;}*_tmp712=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp712,_tmp714);};_pop_dynregion(d);}{struct Cyc_List_List*flat_structs=0;{
struct Cyc_List_List*_tmp715=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp715 != 0;_tmp715=_tmp715->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp715->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp716=0;int i=1;{struct Cyc_List_List*_tmp717=f->typs;for(0;
_tmp717 != 0;(_tmp717=_tmp717->tl,i ++)){struct _dynforward_ptr*_tmp718=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp719=({struct Cyc_Absyn_Aggrfield*_tmp71B=
_cycalloc(sizeof(*_tmp71B));_tmp71B->name=_tmp718;_tmp71B->tq=(*((struct _tuple4*)
_tmp717->hd)).f1;_tmp71B->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp717->hd)).f2);_tmp71B->width=0;_tmp71B->attributes=0;_tmp71B;});_tmp716=({
struct Cyc_List_List*_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A->hd=_tmp719;
_tmp71A->tl=_tmp716;_tmp71A;});}}_tmp716=({struct Cyc_List_List*_tmp71C=_cycalloc(
sizeof(*_tmp71C));_tmp71C->hd=({struct Cyc_Absyn_Aggrfield*_tmp71D=_cycalloc(
sizeof(*_tmp71D));_tmp71D->name=Cyc_Toc_tag_sp;_tmp71D->tq=Cyc_Toc_mt_tq;_tmp71D->type=(
void*)Cyc_Absyn_sint_typ;_tmp71D->width=0;_tmp71D->attributes=0;_tmp71D;});
_tmp71C->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp716);_tmp71C;});{struct Cyc_Absyn_Aggrdecl*_tmp71E=({struct Cyc_Absyn_Aggrdecl*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A->kind=(void*)((void*)0);_tmp72A->sc=(
void*)((void*)2);_tmp72A->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp72C="_struct";_tag_dynforward(_tmp72C,sizeof(char),_get_zero_arr_size(
_tmp72C,8));}));_tmp72A->tvs=0;_tmp72A->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B->exist_vars=0;_tmp72B->rgn_po=0;
_tmp72B->fields=_tmp716;_tmp72B;});_tmp72A->attributes=0;_tmp72A;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp720=_cycalloc(sizeof(*_tmp720));
_tmp720[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp721;_tmp721.tag=4;_tmp721.f1=
_tmp71E;_tmp721;});_tmp720;}),0);_tmp71F->tl=Cyc_Toc_result_decls;_tmp71F;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp722=({struct Cyc_Absyn_Aggrfield*
_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->name=(*f->name).f2;_tmp724->tq=Cyc_Toc_mt_tq;
_tmp724->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp725=_cycalloc(
sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Absyn_AggrType_struct _tmp726;_tmp726.tag=
10;_tmp726.f1=({struct Cyc_Absyn_AggrInfo _tmp727;_tmp727.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp728;(_tmp728.KnownAggr).tag=1;(_tmp728.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729[0]=_tmp71E;
_tmp729;});_tmp728;});_tmp727.targs=0;_tmp727;});_tmp726;});_tmp725;}));_tmp724->width=
0;_tmp724->attributes=0;_tmp724;});flat_structs=({struct Cyc_List_List*_tmp723=
_cycalloc(sizeof(*_tmp723));_tmp723->hd=_tmp722;_tmp723->tl=flat_structs;_tmp723;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp72D=({struct Cyc_Absyn_Aggrdecl*
_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->kind=(void*)((void*)1);_tmp731->sc=(
void*)((void*)2);_tmp731->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp733="_union";_tag_dynforward(_tmp733,sizeof(char),_get_zero_arr_size(_tmp733,
7));}));_tmp731->tvs=0;_tmp731->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp732=
_cycalloc(sizeof(*_tmp732));_tmp732->exist_vars=0;_tmp732->rgn_po=0;_tmp732->fields=
flat_structs;_tmp732;});_tmp731->attributes=0;_tmp731;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp72E=_cycalloc(sizeof(*_tmp72E));_tmp72E->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp72F=_cycalloc(sizeof(*_tmp72F));
_tmp72F[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp730;_tmp730.tag=4;_tmp730.f1=
_tmp72D;_tmp730;});_tmp72F;}),0);_tmp72E->tl=Cyc_Toc_result_decls;_tmp72E;});}}}}
static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp735;struct Cyc_Dict_Dict*_tmp736;struct
Cyc_Toc_TocState _tmp734=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp735=_tmp734.dyn;_tmp736=_tmp734.xtunions_so_far;{struct _DynRegionFrame
_tmp737;struct _RegionHandle*d=_open_dynregion(& _tmp737,_tmp735);{struct _tuple1*
_tmp738=xd->name;struct Cyc_List_List*_tmp739=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp739 != 0;_tmp739=_tmp739->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp739->hd;struct _dynforward_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp73A=Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(
char))+ 4,0);void*_tmp73B=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp73A,Cyc_Absyn_false_conref,0);int*_tmp73C=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp736,f->name);int
_tmp73D;_LL380: if(_tmp73C != 0)goto _LL382;_LL381: {struct Cyc_Absyn_Exp*initopt=0;
if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp744;_tmp744.tag=0;_tmp744.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct
_tmp743;_tmp743.tag=1;_tmp743.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp742;_tmp742.tag=1;_tmp742.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp741;_tmp741.tag=1;_tmp741.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp740;_tmp740.tag=1;_tmp740.f1=(unsigned long)((int)zero);{void*_tmp73E[5]={&
_tmp740,& _tmp741,& _tmp742,& _tmp743,& _tmp744};Cyc_aprintf(({const char*_tmp73F="%c%c%c%c%s";
_tag_dynforward(_tmp73F,sizeof(char),_get_zero_arr_size(_tmp73F,11));}),
_tag_dynforward(_tmp73E,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*
_tmp745=Cyc_Absyn_new_vardecl(f->name,_tmp73B,initopt);(void*)(_tmp745->sc=(void*)((
void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp746=_cycalloc(
sizeof(*_tmp746));_tmp746->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747[0]=({struct Cyc_Absyn_Var_d_struct
_tmp748;_tmp748.tag=0;_tmp748.f1=_tmp745;_tmp748;});_tmp747;}),0);_tmp746->tl=
Cyc_Toc_result_decls;_tmp746;});*_tmp736=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp736,f->name,(void*)f->sc != (void*)
3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp749=f->typs;for(0;_tmp749 != 0;(_tmp749=_tmp749->tl,i ++)){struct
_dynforward_ptr*_tmp74A=({struct _dynforward_ptr*_tmp74E=_cycalloc(sizeof(*
_tmp74E));_tmp74E[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp751;
_tmp751.tag=1;_tmp751.f1=(unsigned long)i;{void*_tmp74F[1]={& _tmp751};Cyc_aprintf(({
const char*_tmp750="f%d";_tag_dynforward(_tmp750,sizeof(char),_get_zero_arr_size(
_tmp750,4));}),_tag_dynforward(_tmp74F,sizeof(void*),1));}});_tmp74E;});struct
Cyc_Absyn_Aggrfield*_tmp74B=({struct Cyc_Absyn_Aggrfield*_tmp74D=_cycalloc(
sizeof(*_tmp74D));_tmp74D->name=_tmp74A;_tmp74D->tq=(*((struct _tuple4*)_tmp749->hd)).f1;
_tmp74D->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp749->hd)).f2);
_tmp74D->width=0;_tmp74D->attributes=0;_tmp74D;});fields=({struct Cyc_List_List*
_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->hd=_tmp74B;_tmp74C->tl=fields;
_tmp74C;});}}fields=({struct Cyc_List_List*_tmp752=_cycalloc(sizeof(*_tmp752));
_tmp752->hd=({struct Cyc_Absyn_Aggrfield*_tmp753=_cycalloc(sizeof(*_tmp753));
_tmp753->name=Cyc_Toc_tag_sp;_tmp753->tq=Cyc_Toc_mt_tq;_tmp753->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp753->width=0;_tmp753->attributes=0;_tmp753;});
_tmp752->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp752;});{struct Cyc_Absyn_Aggrdecl*_tmp754=({struct Cyc_Absyn_Aggrdecl*
_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758->kind=(void*)((void*)0);_tmp758->sc=(
void*)((void*)2);_tmp758->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp75A="_struct";_tag_dynforward(_tmp75A,sizeof(char),_get_zero_arr_size(
_tmp75A,8));}));_tmp758->tvs=0;_tmp758->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759->exist_vars=0;_tmp759->rgn_po=0;
_tmp759->fields=fields;_tmp759;});_tmp758->attributes=0;_tmp758;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp756=_cycalloc(sizeof(*_tmp756));
_tmp756[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp757;_tmp757.tag=4;_tmp757.f1=
_tmp754;_tmp757;});_tmp756;}),0);_tmp755->tl=Cyc_Toc_result_decls;_tmp755;});}}
goto _LL37F;}}_LL382: if(_tmp73C == 0)goto _LL384;_tmp73D=*_tmp73C;if(_tmp73D != 0)
goto _LL384;_LL383: if((void*)f->sc != (void*)3){char zero='\000';struct Cyc_Absyn_Exp*
_tmp75B=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp766;_tmp766.tag=0;_tmp766.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
fn);{struct Cyc_Int_pa_struct _tmp765;_tmp765.tag=1;_tmp765.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp764;_tmp764.tag=1;_tmp764.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp763;_tmp763.tag=1;_tmp763.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp762;_tmp762.tag=1;_tmp762.f1=(
unsigned long)((int)zero);{void*_tmp760[5]={& _tmp762,& _tmp763,& _tmp764,& _tmp765,&
_tmp766};Cyc_aprintf(({const char*_tmp761="%c%c%c%c%s";_tag_dynforward(_tmp761,
sizeof(char),_get_zero_arr_size(_tmp761,11));}),_tag_dynforward(_tmp760,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp75C=Cyc_Absyn_new_vardecl(f->name,
_tmp73B,(struct Cyc_Absyn_Exp*)_tmp75B);(void*)(_tmp75C->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75D=_cycalloc(sizeof(*_tmp75D));
_tmp75D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp75E=
_cycalloc(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Absyn_Var_d_struct _tmp75F;
_tmp75F.tag=0;_tmp75F.f1=_tmp75C;_tmp75F;});_tmp75E;}),0);_tmp75D->tl=Cyc_Toc_result_decls;
_tmp75D;});*_tmp736=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k,int v))Cyc_Dict_insert)(*_tmp736,f->name,1);}goto _LL37F;_LL384:;_LL385: goto
_LL37F;_LL37F:;}};_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)2));if(ed->fields
!= 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp767=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp768;struct Cyc_Absyn_Exp*_tmp769;struct Cyc_Absyn_Exp*_tmp76A;int _tmp76B;
_LL387: if(*((int*)_tmp767)!= 29)goto _LL389;_tmp768=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp767)->f1;_tmp769=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp767)->f2;
_tmp76A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp767)->f3;_tmp76B=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp767)->f4;_LL388: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp768,_tmp769,_tmp76A,_tmp76B,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL386;_LL389:;_LL38A: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp76C=_new_region("temp");struct _RegionHandle*temp=& _tmp76C;_push_region(temp);{
struct Cyc_Toc_Env*_tmp76D=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp76D,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL386;_LL386:;}else{void*_tmp76E=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp76F;void*_tmp770;struct Cyc_Absyn_Exp*_tmp771;struct Cyc_Absyn_Conref*_tmp772;
_LL38C: if(_tmp76E <= (void*)4)goto _LL38E;if(*((int*)_tmp76E)!= 7)goto _LL38E;
_tmp76F=((struct Cyc_Absyn_ArrayType_struct*)_tmp76E)->f1;_tmp770=(void*)_tmp76F.elt_type;
_tmp771=_tmp76F.num_elts;_tmp772=_tmp76F.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp772))goto _LL38E;_LL38D: if(_tmp771 == 0)({void*
_tmp773[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp774="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp774,sizeof(char),_get_zero_arr_size(_tmp774,55));}),
_tag_dynforward(_tmp773,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp771;struct Cyc_Absyn_Exp*_tmp775=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp776=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp775,_tmp776,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL38B;}_LL38E:;_LL38F: goto _LL38B;
_LL38B:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp777=_cycalloc(
sizeof(*_tmp777));_tmp777[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp777;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp778=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp778;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp779=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp77A=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp77C;struct Cyc_List_List*_tmp77D;struct Cyc_Absyn_Stmt*
_tmp77E;struct _tuple13 _tmp77B=Cyc_Toc_xlate_pat(_tmp77A,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp779,Cyc_Toc_throw_match_stmt(),
0);_tmp77C=_tmp77B.f1;_tmp77D=_tmp77B.f2;_tmp77E=_tmp77B.f3;Cyc_Toc_stmt_to_c(
_tmp77C,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp77E,s,0),0);for(0;_tmp77D != 0;_tmp77D=_tmp77D->tl){
struct _tuple14 _tmp780;struct _tuple1*_tmp781;void*_tmp782;struct _tuple14*_tmp77F=(
struct _tuple14*)_tmp77D->hd;_tmp780=*_tmp77F;_tmp781=_tmp780.f1;_tmp782=_tmp780.f2;
s=Cyc_Absyn_declare_stmt(_tmp781,_tmp782,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp783=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp784;struct Cyc_Absyn_Exp*_tmp785;struct Cyc_Absyn_Exp*
_tmp786;struct Cyc_Absyn_Exp*_tmp787;struct Cyc_Absyn_Exp*_tmp788;struct Cyc_Absyn_Exp*
_tmp789;struct Cyc_Absyn_Exp*_tmp78A;struct Cyc_Absyn_Exp*_tmp78B;struct Cyc_List_List*
_tmp78C;struct Cyc_Absyn_Exp*_tmp78D;struct Cyc_Absyn_Exp*_tmp78E;struct Cyc_Absyn_Exp*
_tmp78F;struct Cyc_Absyn_Exp*_tmp790;struct Cyc_Absyn_Exp*_tmp791;struct Cyc_Absyn_Exp*
_tmp792;struct Cyc_Absyn_Exp*_tmp793;struct Cyc_Absyn_Exp*_tmp794;struct Cyc_Absyn_Exp*
_tmp795;struct Cyc_Absyn_Exp*_tmp796;struct Cyc_Absyn_Exp*_tmp797;struct Cyc_Absyn_Exp*
_tmp798;struct Cyc_Absyn_Exp*_tmp799;struct Cyc_Absyn_Exp*_tmp79A;struct Cyc_Absyn_Exp*
_tmp79B;struct Cyc_Absyn_Exp*_tmp79C;struct Cyc_List_List*_tmp79D;struct Cyc_Absyn_Exp*
_tmp79E;struct Cyc_List_List*_tmp79F;void*_tmp7A0;void**_tmp7A1;struct Cyc_Absyn_Exp*
_tmp7A2;struct _tuple2*_tmp7A3;struct _tuple2 _tmp7A4;void*_tmp7A5;void**_tmp7A6;
struct Cyc_List_List*_tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_List_List*
_tmp7A9;void*_tmp7AA;void**_tmp7AB;void*_tmp7AC;void**_tmp7AD;struct Cyc_Absyn_Stmt*
_tmp7AE;struct Cyc_Absyn_MallocInfo _tmp7AF;struct Cyc_Absyn_MallocInfo*_tmp7B0;
_LL391: if(*((int*)_tmp783)!= 22)goto _LL393;_tmp784=((struct Cyc_Absyn_Deref_e_struct*)
_tmp783)->f1;_LL392: _tmp785=_tmp784;goto _LL394;_LL393: if(*((int*)_tmp783)!= 23)
goto _LL395;_tmp785=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp783)->f1;_LL394:
_tmp786=_tmp785;goto _LL396;_LL395: if(*((int*)_tmp783)!= 24)goto _LL397;_tmp786=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp783)->f1;_LL396: _tmp787=_tmp786;goto
_LL398;_LL397: if(*((int*)_tmp783)!= 16)goto _LL399;_tmp787=((struct Cyc_Absyn_Address_e_struct*)
_tmp783)->f1;_LL398: _tmp788=_tmp787;goto _LL39A;_LL399: if(*((int*)_tmp783)!= 12)
goto _LL39B;_tmp788=((struct Cyc_Absyn_Throw_e_struct*)_tmp783)->f1;_LL39A: _tmp789=
_tmp788;goto _LL39C;_LL39B: if(*((int*)_tmp783)!= 13)goto _LL39D;_tmp789=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp783)->f1;_LL39C: _tmp78A=_tmp789;goto _LL39E;
_LL39D: if(*((int*)_tmp783)!= 19)goto _LL39F;_tmp78A=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp783)->f1;_LL39E: _tmp78B=_tmp78A;goto _LL3A0;_LL39F: if(*((int*)_tmp783)!= 5)
goto _LL3A1;_tmp78B=((struct Cyc_Absyn_Increment_e_struct*)_tmp783)->f1;_LL3A0: Cyc_Toc_exptypes_to_c(
_tmp78B);goto _LL390;_LL3A1: if(*((int*)_tmp783)!= 3)goto _LL3A3;_tmp78C=((struct
Cyc_Absyn_Primop_e_struct*)_tmp783)->f2;_LL3A2:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp78C);goto _LL390;
_LL3A3: if(*((int*)_tmp783)!= 7)goto _LL3A5;_tmp78D=((struct Cyc_Absyn_And_e_struct*)
_tmp783)->f1;_tmp78E=((struct Cyc_Absyn_And_e_struct*)_tmp783)->f2;_LL3A4: _tmp78F=
_tmp78D;_tmp790=_tmp78E;goto _LL3A6;_LL3A5: if(*((int*)_tmp783)!= 8)goto _LL3A7;
_tmp78F=((struct Cyc_Absyn_Or_e_struct*)_tmp783)->f1;_tmp790=((struct Cyc_Absyn_Or_e_struct*)
_tmp783)->f2;_LL3A6: _tmp791=_tmp78F;_tmp792=_tmp790;goto _LL3A8;_LL3A7: if(*((int*)
_tmp783)!= 9)goto _LL3A9;_tmp791=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp783)->f1;
_tmp792=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp783)->f2;_LL3A8: _tmp793=_tmp791;
_tmp794=_tmp792;goto _LL3AA;_LL3A9: if(*((int*)_tmp783)!= 25)goto _LL3AB;_tmp793=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp783)->f1;_tmp794=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp783)->f2;_LL3AA: _tmp795=_tmp793;_tmp796=_tmp794;goto _LL3AC;_LL3AB: if(*((int*)
_tmp783)!= 36)goto _LL3AD;_tmp795=((struct Cyc_Absyn_Swap_e_struct*)_tmp783)->f1;
_tmp796=((struct Cyc_Absyn_Swap_e_struct*)_tmp783)->f2;_LL3AC: _tmp797=_tmp795;
_tmp798=_tmp796;goto _LL3AE;_LL3AD: if(*((int*)_tmp783)!= 4)goto _LL3AF;_tmp797=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp783)->f1;_tmp798=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp783)->f3;_LL3AE: Cyc_Toc_exptypes_to_c(_tmp797);Cyc_Toc_exptypes_to_c(_tmp798);
goto _LL390;_LL3AF: if(*((int*)_tmp783)!= 6)goto _LL3B1;_tmp799=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp783)->f1;_tmp79A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp783)->f2;
_tmp79B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp783)->f3;_LL3B0: Cyc_Toc_exptypes_to_c(
_tmp799);Cyc_Toc_exptypes_to_c(_tmp79A);Cyc_Toc_exptypes_to_c(_tmp79B);goto
_LL390;_LL3B1: if(*((int*)_tmp783)!= 11)goto _LL3B3;_tmp79C=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp783)->f1;_tmp79D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp783)->f2;_LL3B2:
_tmp79E=_tmp79C;_tmp79F=_tmp79D;goto _LL3B4;_LL3B3: if(*((int*)_tmp783)!= 10)goto
_LL3B5;_tmp79E=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp783)->f1;_tmp79F=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp783)->f2;_LL3B4: Cyc_Toc_exptypes_to_c(
_tmp79E);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp79F);goto _LL390;_LL3B5: if(*((int*)_tmp783)!= 15)goto
_LL3B7;_tmp7A0=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp783)->f1;_tmp7A1=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp783)->f1);_tmp7A2=((struct
Cyc_Absyn_Cast_e_struct*)_tmp783)->f2;_LL3B6:*_tmp7A1=Cyc_Toc_typ_to_c(*_tmp7A1);
Cyc_Toc_exptypes_to_c(_tmp7A2);goto _LL390;_LL3B7: if(*((int*)_tmp783)!= 27)goto
_LL3B9;_tmp7A3=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp783)->f1;_tmp7A4=*
_tmp7A3;_tmp7A5=_tmp7A4.f3;_tmp7A6=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp783)->f1).f3;_tmp7A7=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp783)->f2;
_LL3B8:*_tmp7A6=Cyc_Toc_typ_to_c(*_tmp7A6);_tmp7A8=_tmp7A7;goto _LL3BA;_LL3B9: if(*((
int*)_tmp783)!= 37)goto _LL3BB;_tmp7A8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp783)->f2;_LL3BA: _tmp7A9=_tmp7A8;goto _LL3BC;_LL3BB: if(*((int*)_tmp783)!= 28)
goto _LL3BD;_tmp7A9=((struct Cyc_Absyn_Array_e_struct*)_tmp783)->f1;_LL3BC: for(0;
_tmp7A9 != 0;_tmp7A9=_tmp7A9->tl){struct Cyc_Absyn_Exp*_tmp7B2;struct _tuple8
_tmp7B1=*((struct _tuple8*)_tmp7A9->hd);_tmp7B2=_tmp7B1.f2;Cyc_Toc_exptypes_to_c(
_tmp7B2);}goto _LL390;_LL3BD: if(*((int*)_tmp783)!= 20)goto _LL3BF;_tmp7AA=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp783)->f1;_tmp7AB=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp783)->f1);_LL3BE: _tmp7AD=_tmp7AB;goto
_LL3C0;_LL3BF: if(*((int*)_tmp783)!= 18)goto _LL3C1;_tmp7AC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp783)->f1;_tmp7AD=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp783)->f1);_LL3C0:*_tmp7AD=Cyc_Toc_typ_to_c(*_tmp7AD);goto _LL390;_LL3C1: if(*((
int*)_tmp783)!= 38)goto _LL3C3;_tmp7AE=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp783)->f1;_LL3C2: Cyc_Toc_stmttypes_to_c(_tmp7AE);goto _LL390;_LL3C3: if(*((int*)
_tmp783)!= 35)goto _LL3C5;_tmp7AF=((struct Cyc_Absyn_Malloc_e_struct*)_tmp783)->f1;
_tmp7B0=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp783)->f1;
_LL3C4: if(_tmp7B0->elt_type != 0)_tmp7B0->elt_type=({void**_tmp7B3=_cycalloc(
sizeof(*_tmp7B3));_tmp7B3[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp7B0->elt_type)));
_tmp7B3;});Cyc_Toc_exptypes_to_c(_tmp7B0->num_elts);goto _LL390;_LL3C5: if(*((int*)
_tmp783)!= 0)goto _LL3C7;_LL3C6: goto _LL3C8;_LL3C7: if(*((int*)_tmp783)!= 1)goto
_LL3C9;_LL3C8: goto _LL3CA;_LL3C9: if(*((int*)_tmp783)!= 2)goto _LL3CB;_LL3CA: goto
_LL3CC;_LL3CB: if(*((int*)_tmp783)!= 33)goto _LL3CD;_LL3CC: goto _LL3CE;_LL3CD: if(*((
int*)_tmp783)!= 34)goto _LL3CF;_LL3CE: goto _LL390;_LL3CF: if(*((int*)_tmp783)!= 31)
goto _LL3D1;_LL3D0: goto _LL3D2;_LL3D1: if(*((int*)_tmp783)!= 32)goto _LL3D3;_LL3D2:
goto _LL3D4;_LL3D3: if(*((int*)_tmp783)!= 30)goto _LL3D5;_LL3D4: goto _LL3D6;_LL3D5:
if(*((int*)_tmp783)!= 29)goto _LL3D7;_LL3D6: goto _LL3D8;_LL3D7: if(*((int*)_tmp783)
!= 26)goto _LL3D9;_LL3D8: goto _LL3DA;_LL3D9: if(*((int*)_tmp783)!= 14)goto _LL3DB;
_LL3DA: goto _LL3DC;_LL3DB: if(*((int*)_tmp783)!= 17)goto _LL3DD;_LL3DC: goto _LL3DE;
_LL3DD: if(*((int*)_tmp783)!= 21)goto _LL390;_LL3DE:({void*_tmp7B4[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp7B5="Cyclone expression within C code";_tag_dynforward(
_tmp7B5,sizeof(char),_get_zero_arr_size(_tmp7B5,33));}),_tag_dynforward(_tmp7B4,
sizeof(void*),0));});goto _LL390;_LL390:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp7B6=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp7B7;
struct Cyc_Absyn_Fndecl*_tmp7B8;struct Cyc_Absyn_Aggrdecl*_tmp7B9;struct Cyc_Absyn_Enumdecl*
_tmp7BA;struct Cyc_Absyn_Typedefdecl*_tmp7BB;_LL3E0: if(_tmp7B6 <= (void*)2)goto
_LL3F8;if(*((int*)_tmp7B6)!= 0)goto _LL3E2;_tmp7B7=((struct Cyc_Absyn_Var_d_struct*)
_tmp7B6)->f1;_LL3E1:(void*)(_tmp7B7->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp7B7->type));
if(_tmp7B7->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp7B7->initializer));goto _LL3DF;_LL3E2: if(*((int*)_tmp7B6)!= 1)
goto _LL3E4;_tmp7B8=((struct Cyc_Absyn_Fn_d_struct*)_tmp7B6)->f1;_LL3E3:(void*)(
_tmp7B8->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp7B8->ret_type));{struct Cyc_List_List*
_tmp7BC=_tmp7B8->args;for(0;_tmp7BC != 0;_tmp7BC=_tmp7BC->tl){(*((struct _tuple17*)
_tmp7BC->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp7BC->hd)).f3);}}goto
_LL3DF;_LL3E4: if(*((int*)_tmp7B6)!= 4)goto _LL3E6;_tmp7B9=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp7B6)->f1;_LL3E5: Cyc_Toc_aggrdecl_to_c(_tmp7B9);goto _LL3DF;_LL3E6: if(*((int*)
_tmp7B6)!= 6)goto _LL3E8;_tmp7BA=((struct Cyc_Absyn_Enum_d_struct*)_tmp7B6)->f1;
_LL3E7: if(_tmp7BA->fields != 0){struct Cyc_List_List*_tmp7BD=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp7BA->fields))->v;for(0;_tmp7BD != 0;_tmp7BD=
_tmp7BD->tl){struct Cyc_Absyn_Enumfield*_tmp7BE=(struct Cyc_Absyn_Enumfield*)
_tmp7BD->hd;if(_tmp7BE->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp7BE->tag));}}goto _LL3DF;_LL3E8: if(*((int*)_tmp7B6)!= 7)goto
_LL3EA;_tmp7BB=((struct Cyc_Absyn_Typedef_d_struct*)_tmp7B6)->f1;_LL3E9: _tmp7BB->defn=({
struct Cyc_Core_Opt*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7BB->defn))->v);_tmp7BF;});goto
_LL3DF;_LL3EA: if(*((int*)_tmp7B6)!= 2)goto _LL3EC;_LL3EB: goto _LL3ED;_LL3EC: if(*((
int*)_tmp7B6)!= 3)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE: if(*((int*)_tmp7B6)!= 5)
goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp7B6)!= 8)goto _LL3F2;_LL3F1:
goto _LL3F3;_LL3F2: if(*((int*)_tmp7B6)!= 9)goto _LL3F4;_LL3F3: goto _LL3F5;_LL3F4:
if(*((int*)_tmp7B6)!= 10)goto _LL3F6;_LL3F5: goto _LL3F7;_LL3F6: if(*((int*)_tmp7B6)
!= 11)goto _LL3F8;_LL3F7:({void*_tmp7C0[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp7C1="Cyclone declaration within C code";_tag_dynforward(_tmp7C1,sizeof(char),
_get_zero_arr_size(_tmp7C1,34));}),_tag_dynforward(_tmp7C0,sizeof(void*),0));});
goto _LL3DF;_LL3F8: if((int)_tmp7B6 != 0)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if((
int)_tmp7B6 != 1)goto _LL3DF;_LL3FB: goto _LL3DF;_LL3DF:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp7C2=(void*)s->r;struct Cyc_Absyn_Exp*_tmp7C3;
struct Cyc_Absyn_Stmt*_tmp7C4;struct Cyc_Absyn_Stmt*_tmp7C5;struct Cyc_Absyn_Exp*
_tmp7C6;struct Cyc_Absyn_Exp*_tmp7C7;struct Cyc_Absyn_Stmt*_tmp7C8;struct Cyc_Absyn_Stmt*
_tmp7C9;struct _tuple3 _tmp7CA;struct Cyc_Absyn_Exp*_tmp7CB;struct Cyc_Absyn_Stmt*
_tmp7CC;struct Cyc_Absyn_Exp*_tmp7CD;struct _tuple3 _tmp7CE;struct Cyc_Absyn_Exp*
_tmp7CF;struct _tuple3 _tmp7D0;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_Absyn_Stmt*
_tmp7D2;struct Cyc_Absyn_Exp*_tmp7D3;struct Cyc_List_List*_tmp7D4;struct Cyc_Absyn_Decl*
_tmp7D5;struct Cyc_Absyn_Stmt*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D7;struct _tuple3
_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D9;_LL3FD: if(_tmp7C2 <= (void*)1)goto _LL40F;if(*((
int*)_tmp7C2)!= 0)goto _LL3FF;_tmp7C3=((struct Cyc_Absyn_Exp_s_struct*)_tmp7C2)->f1;
_LL3FE: Cyc_Toc_exptypes_to_c(_tmp7C3);goto _LL3FC;_LL3FF: if(*((int*)_tmp7C2)!= 1)
goto _LL401;_tmp7C4=((struct Cyc_Absyn_Seq_s_struct*)_tmp7C2)->f1;_tmp7C5=((struct
Cyc_Absyn_Seq_s_struct*)_tmp7C2)->f2;_LL400: Cyc_Toc_stmttypes_to_c(_tmp7C4);Cyc_Toc_stmttypes_to_c(
_tmp7C5);goto _LL3FC;_LL401: if(*((int*)_tmp7C2)!= 2)goto _LL403;_tmp7C6=((struct
Cyc_Absyn_Return_s_struct*)_tmp7C2)->f1;_LL402: if(_tmp7C6 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp7C6);goto _LL3FC;_LL403: if(*((int*)_tmp7C2)!= 3)goto
_LL405;_tmp7C7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7C2)->f1;_tmp7C8=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7C2)->f2;_tmp7C9=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp7C2)->f3;_LL404: Cyc_Toc_exptypes_to_c(_tmp7C7);Cyc_Toc_stmttypes_to_c(
_tmp7C8);Cyc_Toc_stmttypes_to_c(_tmp7C9);goto _LL3FC;_LL405: if(*((int*)_tmp7C2)!= 
4)goto _LL407;_tmp7CA=((struct Cyc_Absyn_While_s_struct*)_tmp7C2)->f1;_tmp7CB=
_tmp7CA.f1;_tmp7CC=((struct Cyc_Absyn_While_s_struct*)_tmp7C2)->f2;_LL406: Cyc_Toc_exptypes_to_c(
_tmp7CB);Cyc_Toc_stmttypes_to_c(_tmp7CC);goto _LL3FC;_LL407: if(*((int*)_tmp7C2)!= 
8)goto _LL409;_tmp7CD=((struct Cyc_Absyn_For_s_struct*)_tmp7C2)->f1;_tmp7CE=((
struct Cyc_Absyn_For_s_struct*)_tmp7C2)->f2;_tmp7CF=_tmp7CE.f1;_tmp7D0=((struct
Cyc_Absyn_For_s_struct*)_tmp7C2)->f3;_tmp7D1=_tmp7D0.f1;_tmp7D2=((struct Cyc_Absyn_For_s_struct*)
_tmp7C2)->f4;_LL408: Cyc_Toc_exptypes_to_c(_tmp7CD);Cyc_Toc_exptypes_to_c(_tmp7CF);
Cyc_Toc_exptypes_to_c(_tmp7D1);Cyc_Toc_stmttypes_to_c(_tmp7D2);goto _LL3FC;_LL409:
if(*((int*)_tmp7C2)!= 9)goto _LL40B;_tmp7D3=((struct Cyc_Absyn_Switch_s_struct*)
_tmp7C2)->f1;_tmp7D4=((struct Cyc_Absyn_Switch_s_struct*)_tmp7C2)->f2;_LL40A: Cyc_Toc_exptypes_to_c(
_tmp7D3);for(0;_tmp7D4 != 0;_tmp7D4=_tmp7D4->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp7D4->hd)->body);}goto _LL3FC;_LL40B: if(*((int*)
_tmp7C2)!= 11)goto _LL40D;_tmp7D5=((struct Cyc_Absyn_Decl_s_struct*)_tmp7C2)->f1;
_tmp7D6=((struct Cyc_Absyn_Decl_s_struct*)_tmp7C2)->f2;_LL40C: Cyc_Toc_decltypes_to_c(
_tmp7D5);Cyc_Toc_stmttypes_to_c(_tmp7D6);goto _LL3FC;_LL40D: if(*((int*)_tmp7C2)!= 
13)goto _LL40F;_tmp7D7=((struct Cyc_Absyn_Do_s_struct*)_tmp7C2)->f1;_tmp7D8=((
struct Cyc_Absyn_Do_s_struct*)_tmp7C2)->f2;_tmp7D9=_tmp7D8.f1;_LL40E: Cyc_Toc_stmttypes_to_c(
_tmp7D7);Cyc_Toc_exptypes_to_c(_tmp7D9);goto _LL3FC;_LL40F: if((int)_tmp7C2 != 0)
goto _LL411;_LL410: goto _LL412;_LL411: if(_tmp7C2 <= (void*)1)goto _LL413;if(*((int*)
_tmp7C2)!= 5)goto _LL413;_LL412: goto _LL414;_LL413: if(_tmp7C2 <= (void*)1)goto
_LL415;if(*((int*)_tmp7C2)!= 6)goto _LL415;_LL414: goto _LL416;_LL415: if(_tmp7C2 <= (
void*)1)goto _LL417;if(*((int*)_tmp7C2)!= 7)goto _LL417;_LL416: goto _LL3FC;_LL417:
if(_tmp7C2 <= (void*)1)goto _LL419;if(*((int*)_tmp7C2)!= 10)goto _LL419;_LL418: goto
_LL41A;_LL419: if(_tmp7C2 <= (void*)1)goto _LL41B;if(*((int*)_tmp7C2)!= 12)goto
_LL41B;_LL41A: goto _LL41C;_LL41B: if(_tmp7C2 <= (void*)1)goto _LL41D;if(*((int*)
_tmp7C2)!= 14)goto _LL41D;_LL41C: goto _LL41E;_LL41D: if(_tmp7C2 <= (void*)1)goto
_LL41F;if(*((int*)_tmp7C2)!= 15)goto _LL41F;_LL41E: goto _LL420;_LL41F: if(_tmp7C2 <= (
void*)1)goto _LL421;if(*((int*)_tmp7C2)!= 17)goto _LL421;_LL420: goto _LL422;_LL421:
if(_tmp7C2 <= (void*)1)goto _LL3FC;if(*((int*)_tmp7C2)!= 16)goto _LL3FC;_LL422:({
void*_tmp7DA[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp7DB="Cyclone statement in C code";
_tag_dynforward(_tmp7DB,sizeof(char),_get_zero_arr_size(_tmp7DB,28));}),
_tag_dynforward(_tmp7DA,sizeof(void*),0));});goto _LL3FC;_LL3FC:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({
void*_tmp7DC[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp7DD="decls_to_c: not at toplevel!";
_tag_dynforward(_tmp7DD,sizeof(char),_get_zero_arr_size(_tmp7DD,29));}),
_tag_dynforward(_tmp7DC,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp7DE=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp7DF;struct Cyc_Absyn_Fndecl*
_tmp7E0;struct Cyc_Absyn_Aggrdecl*_tmp7E1;struct Cyc_Absyn_Tuniondecl*_tmp7E2;
struct Cyc_Absyn_Enumdecl*_tmp7E3;struct Cyc_Absyn_Typedefdecl*_tmp7E4;struct Cyc_List_List*
_tmp7E5;struct Cyc_List_List*_tmp7E6;struct Cyc_List_List*_tmp7E7;struct Cyc_List_List*
_tmp7E8;_LL424: if(_tmp7DE <= (void*)2)goto _LL434;if(*((int*)_tmp7DE)!= 0)goto
_LL426;_tmp7DF=((struct Cyc_Absyn_Var_d_struct*)_tmp7DE)->f1;_LL425: {struct
_tuple1*_tmp7E9=_tmp7DF->name;if((void*)_tmp7DF->sc == (void*)4)_tmp7E9=({struct
_tuple1*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7EB;(_tmp7EB.Rel_n).tag=1;(_tmp7EB.Rel_n).f1=0;
_tmp7EB;});_tmp7EA->f2=(*_tmp7E9).f2;_tmp7EA;});if(_tmp7DF->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp7DF->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp7DF->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp7DF->name,Cyc_Absyn_varb_exp(_tmp7E9,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=({
struct Cyc_Absyn_Global_b_struct _tmp7ED;_tmp7ED.tag=0;_tmp7ED.f1=_tmp7DF;_tmp7ED;});
_tmp7EC;}),0));_tmp7DF->name=_tmp7E9;(void*)(_tmp7DF->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp7DF->sc));(void*)(_tmp7DF->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp7DF->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7EE=_cycalloc(
sizeof(*_tmp7EE));_tmp7EE->hd=d;_tmp7EE->tl=Cyc_Toc_result_decls;_tmp7EE;});goto
_LL423;}_LL426: if(*((int*)_tmp7DE)!= 1)goto _LL428;_tmp7E0=((struct Cyc_Absyn_Fn_d_struct*)
_tmp7DE)->f1;_LL427: {struct _tuple1*_tmp7EF=_tmp7E0->name;if((void*)_tmp7E0->sc
== (void*)4){_tmp7EF=({struct _tuple1*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7F1;(_tmp7F1.Rel_n).tag=
1;(_tmp7F1.Rel_n).f1=0;_tmp7F1;});_tmp7F0->f2=(*_tmp7EF).f2;_tmp7F0;});(void*)(
_tmp7E0->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp7E0->name,Cyc_Absyn_var_exp(
_tmp7EF,0));_tmp7E0->name=_tmp7EF;Cyc_Toc_fndecl_to_c(nv,_tmp7E0,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2->hd=d;_tmp7F2->tl=
Cyc_Toc_result_decls;_tmp7F2;});goto _LL423;}_LL428: if(*((int*)_tmp7DE)!= 2)goto
_LL42A;_LL429: goto _LL42B;_LL42A: if(*((int*)_tmp7DE)!= 3)goto _LL42C;_LL42B:({void*
_tmp7F3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp7F4="letdecl at toplevel";_tag_dynforward(_tmp7F4,sizeof(char),
_get_zero_arr_size(_tmp7F4,20));}),_tag_dynforward(_tmp7F3,sizeof(void*),0));});
_LL42C: if(*((int*)_tmp7DE)!= 4)goto _LL42E;_tmp7E1=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp7DE)->f1;_LL42D: Cyc_Toc_aggrdecl_to_c(_tmp7E1);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5->hd=d;_tmp7F5->tl=Cyc_Toc_result_decls;
_tmp7F5;});goto _LL423;_LL42E: if(*((int*)_tmp7DE)!= 5)goto _LL430;_tmp7E2=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp7DE)->f1;_LL42F: if(_tmp7E2->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp7E2);else{Cyc_Toc_tuniondecl_to_c(_tmp7E2);}goto _LL423;_LL430: if(*((int*)
_tmp7DE)!= 6)goto _LL432;_tmp7E3=((struct Cyc_Absyn_Enum_d_struct*)_tmp7DE)->f1;
_LL431: Cyc_Toc_enumdecl_to_c(nv,_tmp7E3);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6->hd=d;_tmp7F6->tl=Cyc_Toc_result_decls;
_tmp7F6;});goto _LL423;_LL432: if(*((int*)_tmp7DE)!= 7)goto _LL434;_tmp7E4=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp7DE)->f1;_LL433: _tmp7E4->name=_tmp7E4->name;
_tmp7E4->tvs=0;if(_tmp7E4->defn != 0)_tmp7E4->defn=({struct Cyc_Core_Opt*_tmp7F7=
_cycalloc(sizeof(*_tmp7F7));_tmp7F7->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7E4->defn))->v);_tmp7F7;});else{void*_tmp7F8=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7E4->kind))->v;_LL441: if((int)
_tmp7F8 != 2)goto _LL443;_LL442: _tmp7E4->defn=({struct Cyc_Core_Opt*_tmp7F9=
_cycalloc(sizeof(*_tmp7F9));_tmp7F9->v=(void*)Cyc_Absyn_void_star_typ();_tmp7F9;});
goto _LL440;_LL443:;_LL444: _tmp7E4->defn=({struct Cyc_Core_Opt*_tmp7FA=_cycalloc(
sizeof(*_tmp7FA));_tmp7FA->v=(void*)((void*)0);_tmp7FA;});goto _LL440;_LL440:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7FB=_cycalloc(sizeof(*_tmp7FB));
_tmp7FB->hd=d;_tmp7FB->tl=Cyc_Toc_result_decls;_tmp7FB;});goto _LL423;_LL434: if((
int)_tmp7DE != 0)goto _LL436;_LL435: goto _LL437;_LL436: if((int)_tmp7DE != 1)goto
_LL438;_LL437: goto _LL423;_LL438: if(_tmp7DE <= (void*)2)goto _LL43A;if(*((int*)
_tmp7DE)!= 8)goto _LL43A;_tmp7E5=((struct Cyc_Absyn_Namespace_d_struct*)_tmp7DE)->f2;
_LL439: _tmp7E6=_tmp7E5;goto _LL43B;_LL43A: if(_tmp7DE <= (void*)2)goto _LL43C;if(*((
int*)_tmp7DE)!= 9)goto _LL43C;_tmp7E6=((struct Cyc_Absyn_Using_d_struct*)_tmp7DE)->f2;
_LL43B: _tmp7E7=_tmp7E6;goto _LL43D;_LL43C: if(_tmp7DE <= (void*)2)goto _LL43E;if(*((
int*)_tmp7DE)!= 10)goto _LL43E;_tmp7E7=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp7DE)->f1;_LL43D: nv=Cyc_Toc_decls_to_c(r,nv,_tmp7E7,top,cinclude);goto _LL423;
_LL43E: if(_tmp7DE <= (void*)2)goto _LL423;if(*((int*)_tmp7DE)!= 11)goto _LL423;
_tmp7E8=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp7DE)->f1;_LL43F: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp7E8,top,1);goto _LL423;_LL423:;}}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp7FD;struct Cyc_Core_NewRegion _tmp7FC=Cyc_Core_new_dynregion();
_tmp7FD=_tmp7FC.dynregion;{struct _DynRegionFrame _tmp7FE;struct _RegionHandle*d=
_open_dynregion(& _tmp7FE,_tmp7FD);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*
_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF->dyn=(struct _DynRegionHandle*)
_tmp7FD;_tmp7FF->tuple_types=(struct Cyc_List_List**)({struct Cyc_List_List**
_tmp804=_region_malloc(d,sizeof(*_tmp804));_tmp804[0]=0;_tmp804;});_tmp7FF->aggrs_so_far=(
struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmp803=_region_malloc(d,sizeof(*
_tmp803));_tmp803[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmp803;});
_tmp7FF->tunions_so_far=(struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmp802=
_region_malloc(d,sizeof(*_tmp802));_tmp802[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmp802;});_tmp7FF->xtunions_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmp801=_region_malloc(d,sizeof(*_tmp801));_tmp801[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp);_tmp801;});_tmp7FF->qvar_tags=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmp800=_region_malloc(d,sizeof(*_tmp800));_tmp800[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple6*,struct
_tuple6*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp800;});_tmp7FF->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d);_tmp7FF;});;_pop_dynregion(d);}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;Cyc_Toc_globals=_tag_dynforward(({struct
_dynforward_ptr**_tmp805=_cycalloc(sizeof(struct _dynforward_ptr*)* 50);_tmp805[0]=&
Cyc_Toc__throw_str;_tmp805[1]=& Cyc_Toc_setjmp_str;_tmp805[2]=& Cyc_Toc__push_handler_str;
_tmp805[3]=& Cyc_Toc__pop_handler_str;_tmp805[4]=& Cyc_Toc__exn_thrown_str;_tmp805[
5]=& Cyc_Toc__npop_handler_str;_tmp805[6]=& Cyc_Toc__check_null_str;_tmp805[7]=&
Cyc_Toc__check_known_subscript_null_str;_tmp805[8]=& Cyc_Toc__check_known_subscript_notnull_str;
_tmp805[9]=& Cyc_Toc__check_dynforward_subscript_str;_tmp805[10]=& Cyc_Toc__check_dyneither_subscript_str;
_tmp805[11]=& Cyc_Toc__dynforward_ptr_str;_tmp805[12]=& Cyc_Toc__dyneither_ptr_str;
_tmp805[13]=& Cyc_Toc__tag_dynforward_str;_tmp805[14]=& Cyc_Toc__tag_dyneither_str;
_tmp805[15]=& Cyc_Toc__init_dynforward_ptr_str;_tmp805[16]=& Cyc_Toc__init_dyneither_ptr_str;
_tmp805[17]=& Cyc_Toc__untag_dynforward_ptr_str;_tmp805[18]=& Cyc_Toc__untag_dyneither_ptr_str;
_tmp805[19]=& Cyc_Toc__get_dynforward_size_str;_tmp805[20]=& Cyc_Toc__get_dyneither_size_str;
_tmp805[21]=& Cyc_Toc__get_zero_arr_size_str;_tmp805[22]=& Cyc_Toc__dynforward_ptr_plus_str;
_tmp805[23]=& Cyc_Toc__dyneither_ptr_plus_str;_tmp805[24]=& Cyc_Toc__zero_arr_plus_str;
_tmp805[25]=& Cyc_Toc__dynforward_ptr_inplace_plus_str;_tmp805[26]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;
_tmp805[27]=& Cyc_Toc__zero_arr_inplace_plus_str;_tmp805[28]=& Cyc_Toc__dynforward_ptr_inplace_plus_post_str;
_tmp805[29]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp805[30]=& Cyc_Toc__dynforward_to_dyneither_str;
_tmp805[31]=& Cyc_Toc__dyneither_to_dynforward_str;_tmp805[32]=& Cyc_Toc__zero_arr_inplace_plus_post_str;
_tmp805[33]=& Cyc_Toc__cycalloc_str;_tmp805[34]=& Cyc_Toc__cyccalloc_str;_tmp805[
35]=& Cyc_Toc__cycalloc_atomic_str;_tmp805[36]=& Cyc_Toc__cyccalloc_atomic_str;
_tmp805[37]=& Cyc_Toc__region_malloc_str;_tmp805[38]=& Cyc_Toc__region_calloc_str;
_tmp805[39]=& Cyc_Toc__check_times_str;_tmp805[40]=& Cyc_Toc__new_region_str;
_tmp805[41]=& Cyc_Toc__push_region_str;_tmp805[42]=& Cyc_Toc__pop_region_str;
_tmp805[43]=& Cyc_Toc__open_dynregion_str;_tmp805[44]=& Cyc_Toc__push_dynregion_str;
_tmp805[45]=& Cyc_Toc__pop_dynregion_str;_tmp805[46]=& Cyc_Toc__reset_region_str;
_tmp805[47]=& Cyc_Toc__throw_arraybounds_str;_tmp805[48]=& Cyc_Toc__dynforward_ptr_decrease_size_str;
_tmp805[49]=& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp805;}),sizeof(struct
_dynforward_ptr*),50);}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{struct _RegionHandle _tmp806=_new_region("start");struct
_RegionHandle*start=& _tmp806;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmp808;struct Cyc_Toc_TocState _tmp807=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp808=_tmp807.dyn;Cyc_Core_free_dynregion(
_tmp808);};_pop_region(start);}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
