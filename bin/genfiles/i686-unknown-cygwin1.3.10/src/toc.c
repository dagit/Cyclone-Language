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
_dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern
char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int
Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct _dynforward_ptr
Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct
_RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;}
;struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct _dynforward_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
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
info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dynforward_ptr Cyc_Toc_globals;int Cyc_Toc_warn_bounds_checks=0;int
Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;static struct Cyc_List_List*Cyc_Toc_tuple_types=0;static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";_tag_dynforward(
_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_dynforward(_tmp0,sizeof(
void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)
Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmp2[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp3="\n";_tag_dynforward(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,2));}),_tag_dynforward(_tmp2,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp4[5]="curr";
static struct _dynforward_ptr Cyc_Toc_curr_string={_tmp4,_tmp4 + 5};static struct
_dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp5[4]="tag";
static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp5,_tmp5 + 4};static struct
_dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp6[4]="val";
static struct _dynforward_ptr Cyc_Toc_val_string={_tmp6,_tmp6 + 4};static struct
_dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp7[14]="_handler_cons";
static struct _dynforward_ptr Cyc_Toc__handler_cons_string={_tmp7,_tmp7 + 14};static
struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp8[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp8,_tmp8 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp9[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp9,_tmp9 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmpA[17]="_DynRegionHandle";static struct _dynforward_ptr Cyc_Toc__DynRegionHandle_string={
_tmpA,_tmpA + 17};static struct _dynforward_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmpB[16]="_DynRegionFrame";static struct _dynforward_ptr Cyc_Toc__DynRegionFrame_string={
_tmpB,_tmpB + 16};static struct _dynforward_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dynforward_ptr Cyc_Toc_globals={(void*)0,(void*)(0 + 0)};static char _tmpC[7]="_throw";
static struct _dynforward_ptr Cyc_Toc__throw_str={_tmpC,_tmpC + 7};static struct
_tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,&
Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpF[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmpF,_tmpF + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp12[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp12,
_tmp12 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp15[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp15,_tmp15 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp18[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp18,_tmp18 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp1B[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp1B,_tmp1B + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp1E[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp1E,_tmp1E + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp21[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp21,_tmp21 + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp24[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp24,_tmp24 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp27[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp27,
_tmp27 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp2A[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp2A,_tmp2A + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp2D[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp2D,_tmp2D + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp30[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp30,_tmp30 + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp33[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp33,_tmp33 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp36[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp36,_tmp36 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp39[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp39,_tmp39 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp3C[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3C,_tmp3C + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp3F[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp3F,_tmp3F + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp42[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp42,_tmp42 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp45[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp45,_tmp45 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp48[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp48,_tmp48 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp4B[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp4B,_tmp4B + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4E[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp4E,_tmp4E + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp51[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp51,_tmp51 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp54[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp54,_tmp54 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp57[29]="_dynforward_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp57,
_tmp57 + 29};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,&
Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp5A[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5A,
_tmp5A + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp5D[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp5D,_tmp5D + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp60[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp60,_tmp60 + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp63[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp63,
_tmp63 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp66[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp66,_tmp66 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmp69[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmp69,_tmp69 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmp6C[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp6C,_tmp6C + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp6F[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp6F,_tmp6F + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmp72[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp72,_tmp72 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp75[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmp75,_tmp75 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp78[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp78,_tmp78 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp7B[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp7B,_tmp7B + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp7E[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmp7E,_tmp7E + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp81[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp81,_tmp81 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp84[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmp84,_tmp84 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp87[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp87,_tmp87 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp8A[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmp8A,_tmp8A + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp8D[16]="_open_dynregion";static struct _dynforward_ptr Cyc_Toc__open_dynregion_str={
_tmp8D,_tmp8D + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((void*)& Cyc_Toc__open_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=&
Cyc_Toc__open_dynregion_ev;static char _tmp90[16]="_push_dynregion";static struct
_dynforward_ptr Cyc_Toc__push_dynregion_str={_tmp90,_tmp90 + 16};static struct
_tuple1 Cyc_Toc__push_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={
0,(void*)((void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmp93[15]="_pop_dynregion";static struct _dynforward_ptr Cyc_Toc__pop_dynregion_str={
_tmp93,_tmp93 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((void*)& Cyc_Toc__pop_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=&
Cyc_Toc__pop_dynregion_ev;static char _tmp96[14]="_reset_region";static struct
_dynforward_ptr Cyc_Toc__reset_region_str={_tmp96,_tmp96 + 14};static struct _tuple1
Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmp99[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmp99,_tmp99 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp9C[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmp9C,_tmp9C + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp9F[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp9F,
_tmp9F + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA2[11]="_swap_word";static
struct _dynforward_ptr Cyc_Toc__swap_word_str={_tmpA2,_tmpA2 + 11};static struct
_tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmpA5[17]="_swap_dynforward";static struct _dynforward_ptr Cyc_Toc__swap_dynforward_str={
_tmpA5,_tmpA5 + 17};static struct _tuple1 Cyc_Toc__swap_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__swap_dynforward_re={1,& Cyc_Toc__swap_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__swap_dynforward_ev={0,(void*)((void*)& Cyc_Toc__swap_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dynforward_e=&
Cyc_Toc__swap_dynforward_ev;static char _tmpA8[16]="_swap_dyneither";static struct
_dynforward_ptr Cyc_Toc__swap_dyneither_str={_tmpA8,_tmpA8 + 16};static struct
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
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmpAF=_cycalloc(sizeof(*
_tmpAF));_tmpAF[0]=Cyc_Absyn_skip_stmt(0);_tmpAF;});return*skip_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(
t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));
_tmpB0[0]=({struct Cyc_Absyn_Cast_e_struct _tmpB1;_tmpB1.tag=15;_tmpB1.f1=(void*)t;
_tmpB1.f2=e;_tmpB1.f3=0;_tmpB1.f4=(void*)((void*)1);_tmpB1;});_tmpB0;});}static
void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Deref_e_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Deref_e_struct
_tmpB3;_tmpB3.tag=22;_tmpB3.f1=e;_tmpB3;});_tmpB2;});}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmpB5;_tmpB5.tag=25;_tmpB5.f1=e1;_tmpB5.f2=e2;_tmpB5;});_tmpB4;});}static void*
Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmpB7;_tmpB7.tag=38;_tmpB7.f1=s;_tmpB7;});_tmpB6;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpB8=_cycalloc(
sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpB9;_tmpB9.tag=
18;_tmpB9.f1=(void*)t;_tmpB9;});_tmpB8;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpBB;_tmpBB.tag=11;_tmpBB.f1=e;_tmpBB.f2=es;_tmpBB.f3=0;_tmpBB;});_tmpBA;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_Exp_s_struct _tmpBD;
_tmpBD.tag=0;_tmpBD.f1=e;_tmpBD;});_tmpBC;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Seq_s_struct _tmpBF;
_tmpBF.tag=1;_tmpBF.f1=s1;_tmpBF.f2=s2;_tmpBF;});_tmpBE;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpC1;_tmpC1.tag=6;_tmpC1.f1=e1;
_tmpC1.f2=e2;_tmpC1.f3=e3;_tmpC1;});_tmpC0;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpC3;_tmpC3.tag=23;_tmpC3.f1=e;_tmpC3.f2=n;_tmpC3;});_tmpC2;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpC5;_tmpC5.tag=24;_tmpC5.f1=e;_tmpC5.f2=n;_tmpC5;});_tmpC4;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpC7;_tmpC7.tag=37;_tmpC7.f1=tdopt;_tmpC7.f2=ds;_tmpC7;});_tmpC6;});}static
void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){return(
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpC9;_tmpC9.tag=7;_tmpC9.f1=v;_tmpC9.f2=s;_tmpC9;});
_tmpC8;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpCC=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpCD;char _tmpCE;union Cyc_Absyn_Cnst_union
_tmpCF;short _tmpD0;union Cyc_Absyn_Cnst_union _tmpD1;int _tmpD2;union Cyc_Absyn_Cnst_union
_tmpD3;long long _tmpD4;union Cyc_Absyn_Cnst_union _tmpD5;struct Cyc_Absyn_Exp*
_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*
_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_List_List*_tmpDB;_LL1: if(*((int*)
_tmpCC)!= 0)goto _LL3;_tmpCD=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1).Char_c).tag != 0)goto _LL3;_tmpCE=(
_tmpCD.Char_c).f2;_LL2: return _tmpCE == '\000';_LL3: if(*((int*)_tmpCC)!= 0)goto
_LL5;_tmpCF=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpCC)->f1).Short_c).tag != 1)goto _LL5;_tmpD0=(_tmpCF.Short_c).f2;_LL4: return
_tmpD0 == 0;_LL5: if(*((int*)_tmpCC)!= 0)goto _LL7;_tmpD1=((struct Cyc_Absyn_Const_e_struct*)
_tmpCC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1).Int_c).tag != 2)
goto _LL7;_tmpD2=(_tmpD1.Int_c).f2;_LL6: return _tmpD2 == 0;_LL7: if(*((int*)_tmpCC)
!= 0)goto _LL9;_tmpD3=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmpCC)->f1).LongLong_c).tag != 3)goto _LL9;_tmpD4=(
_tmpD3.LongLong_c).f2;_LL8: return _tmpD4 == 0;_LL9: if(*((int*)_tmpCC)!= 0)goto _LLB;
_tmpD5=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpCC)->f1).Null_c).tag != 6)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmpCC)!= 15)
goto _LLD;_tmpD6=((struct Cyc_Absyn_Cast_e_struct*)_tmpCC)->f2;_LLC: return Cyc_Toc_is_zero(
_tmpD6);_LLD: if(*((int*)_tmpCC)!= 26)goto _LLF;_tmpD7=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpCC)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpD7);_LLF: if(*((int*)_tmpCC)!= 28)goto
_LL11;_tmpD8=((struct Cyc_Absyn_Array_e_struct*)_tmpCC)->f1;_LL10: _tmpD9=_tmpD8;
goto _LL12;_LL11: if(*((int*)_tmpCC)!= 30)goto _LL13;_tmpD9=((struct Cyc_Absyn_Struct_e_struct*)
_tmpCC)->f3;_LL12: _tmpDA=_tmpD9;goto _LL14;_LL13: if(*((int*)_tmpCC)!= 27)goto
_LL15;_tmpDA=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpCC)->f2;_LL14: _tmpDB=
_tmpDA;goto _LL16;_LL15: if(*((int*)_tmpCC)!= 37)goto _LL17;_tmpDB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpCC)->f2;_LL16: for(0;_tmpDB != 0;_tmpDB=_tmpDB->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple6*)_tmpDB->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpDC=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpDD;struct Cyc_Absyn_PtrAtts _tmpDE;struct Cyc_Absyn_Conref*
_tmpDF;_LL1A: if(_tmpDC <= (void*)4)goto _LL1C;if(*((int*)_tmpDC)!= 4)goto _LL1C;
_tmpDD=((struct Cyc_Absyn_PointerType_struct*)_tmpDC)->f1;_tmpDE=_tmpDD.ptr_atts;
_tmpDF=_tmpDE.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpDF);_LL1C:;_LL1D:({void*_tmpE0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpE1="is_nullable";
_tag_dynforward(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,12));}),
_tag_dynforward(_tmpE0,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){return({struct _tuple1*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2->f1=(*x).f1;_tmpE2->f2=({struct _dynforward_ptr*_tmpE3=
_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)*(*x).f2,(struct _dynforward_ptr)tag);_tmpE3;});_tmpE2;});}
struct _tuple7{struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};static struct
_dynforward_ptr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*
_tmpE4=Cyc_Toc_tuple_types;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){struct _tuple7
_tmpE6;struct _dynforward_ptr*_tmpE7;struct Cyc_List_List*_tmpE8;struct _tuple7*
_tmpE5=(struct _tuple7*)_tmpE4->hd;_tmpE6=*_tmpE5;_tmpE7=_tmpE6.f1;_tmpE8=_tmpE6.f2;{
struct Cyc_List_List*_tmpE9=tqs0;for(0;_tmpE9 != 0  && _tmpE8 != 0;(_tmpE9=_tmpE9->tl,
_tmpE8=_tmpE8->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmpE9->hd)).f2,(
void*)_tmpE8->hd))break;}if(_tmpE9 == 0  && _tmpE8 == 0)return _tmpE7;}}}{struct
_dynforward_ptr*x=({struct _dynforward_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));
_tmpF7[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpFA;_tmpFA.tag=1;
_tmpFA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmpF8[1]={& _tmpFA};
Cyc_aprintf(({const char*_tmpF9="_tuple%d";_tag_dynforward(_tmpF9,sizeof(char),
_get_zero_arr_size(_tmpF9,9));}),_tag_dynforward(_tmpF8,sizeof(void*),1));}});
_tmpF7;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmpEA=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmpEA=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=({
struct Cyc_Absyn_Aggrfield*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->name=Cyc_Absyn_fieldname(
i);_tmpEC->tq=Cyc_Toc_mt_tq;_tmpEC->type=(void*)((void*)ts2->hd);_tmpEC->width=0;
_tmpEC->attributes=0;_tmpEC;});_tmpEB->tl=_tmpEA;_tmpEB;});}}_tmpEA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpEA);{struct Cyc_Absyn_Aggrdecl*
_tmpED=({struct Cyc_Absyn_Aggrdecl*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->kind=(
void*)((void*)0);_tmpF3->sc=(void*)((void*)2);_tmpF3->name=({struct _tuple1*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmpF6;(_tmpF6.Rel_n).tag=1;(_tmpF6.Rel_n).f1=0;
_tmpF6;});_tmpF5->f2=x;_tmpF5;});_tmpF3->tvs=0;_tmpF3->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->exist_vars=0;_tmpF4->rgn_po=0;_tmpF4->fields=
_tmpEA;_tmpF4;});_tmpF3->attributes=0;_tmpF3;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmpF0;_tmpF0.tag=4;_tmpF0.f1=_tmpED;_tmpF0;});_tmpEF;}),0);_tmpEE->tl=Cyc_Toc_result_decls;
_tmpEE;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*
_tmpF1));_tmpF1->hd=({struct _tuple7*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=
x;_tmpF2->f2=ts;_tmpF2;});_tmpF1->tl=Cyc_Toc_tuple_types;_tmpF1;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmpFB=_cycalloc(sizeof(*
_tmpFB));_tmpFB->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp100;(_tmp100.Loc_n).tag=0;_tmp100;});_tmpFB->f2=({struct _dynforward_ptr*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpFF;_tmpFF.tag=1;_tmpFF.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmpFD[1]={& _tmpFF};Cyc_aprintf(({const char*_tmpFE="_tmp%X";_tag_dynforward(
_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,7));}),_tag_dynforward(_tmpFD,
sizeof(void*),1));}});_tmpFC;});_tmpFB;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
return({struct _dynforward_ptr*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp104;_tmp104.tag=1;_tmp104.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmp102[1]={& _tmp104};Cyc_aprintf(({
const char*_tmp103="_LL%X";_tag_dynforward(_tmp103,sizeof(char),
_get_zero_arr_size(_tmp103,6));}),_tag_dynforward(_tmp102,sizeof(void*),1));}});
_tmp101;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp105=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp105))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp105->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp105->hd)->typs == 0)
++ ans;_tmp105=_tmp105->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmp106=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp106 != 0;_tmp106=_tmp106->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp106->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp108;struct Cyc_Absyn_Tqual
_tmp109;void*_tmp10A;struct _tuple2 _tmp107=*a;_tmp108=_tmp107.f1;_tmp109=_tmp107.f2;
_tmp10A=_tmp107.f3;return({struct _tuple2*_tmp10B=_cycalloc(sizeof(*_tmp10B));
_tmp10B->f1=_tmp108;_tmp10B->f2=_tmp109;_tmp10B->f3=Cyc_Toc_typ_to_c(_tmp10A);
_tmp10B;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp10D;void*_tmp10E;struct _tuple4 _tmp10C=*x;_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;
return({struct _tuple4*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->f1=_tmp10D;
_tmp10F->f2=Cyc_Toc_typ_to_c(_tmp10E);_tmp10F;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp110=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp111;
void*_tmp112;struct Cyc_Absyn_Tqual _tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Conref*
_tmp115;struct Cyc_Position_Segment*_tmp116;struct Cyc_Core_Opt*_tmp117;struct Cyc_Core_Opt
_tmp118;void*_tmp119;_LL1F: if(_tmp110 <= (void*)4)goto _LL23;if(*((int*)_tmp110)!= 
7)goto _LL21;_tmp111=((struct Cyc_Absyn_ArrayType_struct*)_tmp110)->f1;_tmp112=(
void*)_tmp111.elt_type;_tmp113=_tmp111.tq;_tmp114=_tmp111.num_elts;_tmp115=
_tmp111.zero_term;_tmp116=_tmp111.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp112),_tmp113,_tmp114,Cyc_Absyn_false_conref,_tmp116);_LL21: if(*((int*)
_tmp110)!= 0)goto _LL23;_tmp117=((struct Cyc_Absyn_Evar_struct*)_tmp110)->f2;if(
_tmp117 == 0)goto _LL23;_tmp118=*_tmp117;_tmp119=(void*)_tmp118.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmp119);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->name=f->name;_tmp11A->tq=Cyc_Toc_mt_tq;
_tmp11A->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp11A->width=f->width;
_tmp11A->attributes=f->attributes;_tmp11A;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp11B=
t;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Core_Opt*_tmp11D;struct Cyc_Core_Opt
_tmp11E;void*_tmp11F;struct Cyc_Absyn_Tvar*_tmp120;struct Cyc_Absyn_TunionInfo
_tmp121;union Cyc_Absyn_TunionInfoU_union _tmp122;struct Cyc_Absyn_Tuniondecl**
_tmp123;struct Cyc_Absyn_Tuniondecl*_tmp124;struct Cyc_Absyn_TunionFieldInfo
_tmp125;union Cyc_Absyn_TunionFieldInfoU_union _tmp126;struct Cyc_Absyn_Tuniondecl*
_tmp127;struct Cyc_Absyn_Tunionfield*_tmp128;struct Cyc_Absyn_PtrInfo _tmp129;void*
_tmp12A;struct Cyc_Absyn_Tqual _tmp12B;struct Cyc_Absyn_PtrAtts _tmp12C;struct Cyc_Absyn_Conref*
_tmp12D;struct Cyc_Absyn_ArrayInfo _tmp12E;void*_tmp12F;struct Cyc_Absyn_Tqual
_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Position_Segment*_tmp132;struct Cyc_Absyn_FnInfo
_tmp133;void*_tmp134;struct Cyc_List_List*_tmp135;int _tmp136;struct Cyc_Absyn_VarargInfo*
_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_List_List*_tmp139;void*_tmp13A;
struct Cyc_List_List*_tmp13B;struct Cyc_Absyn_AggrInfo _tmp13C;union Cyc_Absyn_AggrInfoU_union
_tmp13D;struct Cyc_List_List*_tmp13E;struct _tuple1*_tmp13F;struct Cyc_List_List*
_tmp140;struct _tuple1*_tmp141;struct Cyc_List_List*_tmp142;struct Cyc_Absyn_Typedefdecl*
_tmp143;void**_tmp144;void*_tmp145;_LL26: if((int)_tmp11B != 0)goto _LL28;_LL27:
return t;_LL28: if(_tmp11B <= (void*)4)goto _LL3A;if(*((int*)_tmp11B)!= 0)goto _LL2A;
_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11B)->f2;if(_tmp11C != 0)goto _LL2A;
_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(*((int*)_tmp11B)!= 0)goto _LL2C;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11B)->f2;if(_tmp11D == 0)goto _LL2C;_tmp11E=*
_tmp11D;_tmp11F=(void*)_tmp11E.v;_LL2B: return Cyc_Toc_typ_to_c(_tmp11F);_LL2C: if(*((
int*)_tmp11B)!= 1)goto _LL2E;_tmp120=((struct Cyc_Absyn_VarType_struct*)_tmp11B)->f1;
_LL2D: if(Cyc_Tcutil_tvar_kind(_tmp120)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL2E: if(*((int*)_tmp11B)!= 2)goto _LL30;_tmp121=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp122=_tmp121.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1).tunion_info).KnownTunion).tag != 1)goto _LL30;_tmp123=(_tmp122.KnownTunion).f1;
_tmp124=*_tmp123;_LL2F: if(_tmp124->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp124->name,({const char*_tmp146="_union";_tag_dynforward(_tmp146,sizeof(char),
_get_zero_arr_size(_tmp146,7));})));else{return Cyc_Absyn_void_star_typ();}_LL30:
if(*((int*)_tmp11B)!= 2)goto _LL32;_LL31:({void*_tmp147[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp148="unresolved TunionType";_tag_dynforward(_tmp148,sizeof(char),
_get_zero_arr_size(_tmp148,22));}),_tag_dynforward(_tmp147,sizeof(void*),0));});
_LL32: if(*((int*)_tmp11B)!= 3)goto _LL34;_tmp125=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11B)->f1;_tmp126=_tmp125.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11B)->f1).field_info).KnownTunionfield).tag != 1)goto _LL34;_tmp127=(_tmp126.KnownTunionfield).f1;
_tmp128=(_tmp126.KnownTunionfield).f2;_LL33: if(_tmp127->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp127->name,({const char*_tmp149="_union";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,7));})));if(
_tmp128->typs == 0){if(_tmp127->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp128->name,({const char*
_tmp14A="_struct";_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(
_tmp14A,8));})));}_LL34: if(*((int*)_tmp11B)!= 3)goto _LL36;_LL35:({void*_tmp14B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp14C="unresolved TunionFieldType";_tag_dynforward(_tmp14C,sizeof(
char),_get_zero_arr_size(_tmp14C,27));}),_tag_dynforward(_tmp14B,sizeof(void*),0));});
_LL36: if(*((int*)_tmp11B)!= 4)goto _LL38;_tmp129=((struct Cyc_Absyn_PointerType_struct*)
_tmp11B)->f1;_tmp12A=(void*)_tmp129.elt_typ;_tmp12B=_tmp129.elt_tq;_tmp12C=
_tmp129.ptr_atts;_tmp12D=_tmp12C.bounds;_LL37: _tmp12A=Cyc_Toc_typ_to_c_array(
_tmp12A);{union Cyc_Absyn_Constraint_union _tmp14D=(Cyc_Absyn_compress_conref(
_tmp12D))->v;void*_tmp14E;void*_tmp14F;_LL63: if((_tmp14D.No_constr).tag != 2)goto
_LL65;_LL64: goto _LL66;_LL65: if((_tmp14D.Eq_constr).tag != 0)goto _LL67;_tmp14E=(
_tmp14D.Eq_constr).f1;if((int)_tmp14E != 0)goto _LL67;_LL66: return Cyc_Toc_dynforward_ptr_typ;
_LL67: if((_tmp14D.Eq_constr).tag != 0)goto _LL69;_tmp14F=(_tmp14D.Eq_constr).f1;
if((int)_tmp14F != 1)goto _LL69;_LL68: return Cyc_Toc_dyneither_ptr_typ;_LL69:;_LL6A:
return Cyc_Absyn_star_typ(_tmp12A,(void*)2,_tmp12B,Cyc_Absyn_false_conref);_LL62:;}
_LL38: if(*((int*)_tmp11B)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp11B != 
1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp11B <= (void*)4)goto _LL58;if(*((int*)
_tmp11B)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmp11B)!= 7)goto _LL40;
_tmp12E=((struct Cyc_Absyn_ArrayType_struct*)_tmp11B)->f1;_tmp12F=(void*)_tmp12E.elt_type;
_tmp130=_tmp12E.tq;_tmp131=_tmp12E.num_elts;_tmp132=_tmp12E.zt_loc;_LL3F: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp12F),_tmp130,_tmp131,Cyc_Absyn_false_conref,
_tmp132);_LL40: if(*((int*)_tmp11B)!= 8)goto _LL42;_tmp133=((struct Cyc_Absyn_FnType_struct*)
_tmp11B)->f1;_tmp134=(void*)_tmp133.ret_typ;_tmp135=_tmp133.args;_tmp136=_tmp133.c_varargs;
_tmp137=_tmp133.cyc_varargs;_tmp138=_tmp133.attributes;_LL41: {struct Cyc_List_List*
_tmp150=0;for(0;_tmp138 != 0;_tmp138=_tmp138->tl){void*_tmp151=(void*)_tmp138->hd;
_LL6C: if((int)_tmp151 != 3)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmp151 != 4)
goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp151 <= (void*)17)goto _LL74;if(*((int*)
_tmp151)!= 3)goto _LL72;_LL71: continue;_LL72: if(*((int*)_tmp151)!= 4)goto _LL74;
_LL73: continue;_LL74:;_LL75: _tmp150=({struct Cyc_List_List*_tmp152=_cycalloc(
sizeof(*_tmp152));_tmp152->hd=(void*)((void*)_tmp138->hd);_tmp152->tl=_tmp150;
_tmp152;});goto _LL6B;_LL6B:;}{struct Cyc_List_List*_tmp153=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp135);if(_tmp137 != 0){void*_tmp154=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp137->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp155=({struct _tuple2*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->f1=
_tmp137->name;_tmp157->f2=_tmp137->tq;_tmp157->f3=_tmp154;_tmp157;});_tmp153=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp153,({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->hd=
_tmp155;_tmp156->tl=0;_tmp156;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Absyn_FnType_struct
_tmp159;_tmp159.tag=8;_tmp159.f1=({struct Cyc_Absyn_FnInfo _tmp15A;_tmp15A.tvars=0;
_tmp15A.effect=0;_tmp15A.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp134);_tmp15A.args=
_tmp153;_tmp15A.c_varargs=_tmp136;_tmp15A.cyc_varargs=0;_tmp15A.rgn_po=0;_tmp15A.attributes=
_tmp150;_tmp15A;});_tmp159;});_tmp158;});}}_LL42: if(*((int*)_tmp11B)!= 9)goto
_LL44;_tmp139=((struct Cyc_Absyn_TupleType_struct*)_tmp11B)->f1;_LL43: _tmp139=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp139);{struct _dynforward_ptr*_tmp15B=Cyc_Toc_add_tuple_type(
_tmp139);return Cyc_Absyn_strct(_tmp15B);}_LL44: if(*((int*)_tmp11B)!= 11)goto
_LL46;_tmp13A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11B)->f1;_tmp13B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp11B)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp15D;_tmp15D.tag=11;_tmp15D.f1=(void*)_tmp13A;_tmp15D.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp13B);_tmp15D;});_tmp15C;});_LL46: if(*((
int*)_tmp11B)!= 10)goto _LL48;_tmp13C=((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1;
_tmp13D=_tmp13C.aggr_info;_tmp13E=_tmp13C.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmp15E=Cyc_Absyn_get_known_aggrdecl(_tmp13D);if((void*)_tmp15E->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmp15E->name);return Cyc_Absyn_strctq(_tmp15E->name);}
_LL48: if(*((int*)_tmp11B)!= 12)goto _LL4A;_tmp13F=((struct Cyc_Absyn_EnumType_struct*)
_tmp11B)->f1;_LL49: return t;_LL4A: if(*((int*)_tmp11B)!= 13)goto _LL4C;_tmp140=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp11B)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmp140);return t;_LL4C: if(*((int*)_tmp11B)!= 17)goto _LL4E;_tmp141=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11B)->f1;_tmp142=((struct Cyc_Absyn_TypedefType_struct*)_tmp11B)->f2;_tmp143=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11B)->f3;_tmp144=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11B)->f4;_LL4D: if(_tmp144 == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp160;_tmp160.tag=17;_tmp160.f1=_tmp141;_tmp160.f2=0;_tmp160.f3=_tmp143;
_tmp160.f4=0;_tmp160;});_tmp15F;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp162;_tmp162.tag=17;_tmp162.f1=_tmp141;_tmp162.f2=0;_tmp162.f3=_tmp143;
_tmp162.f4=({void**_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163[0]=Cyc_Toc_typ_to_c_array(*
_tmp144);_tmp163;});_tmp162;});_tmp161;});}_LL4E: if(*((int*)_tmp11B)!= 14)goto
_LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp11B)!= 18)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;
_LL52: if(*((int*)_tmp11B)!= 15)goto _LL54;_tmp145=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11B)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmp11B)!= 16)goto _LL56;_LL55: return Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_LL56: if(*((int*)
_tmp11B)!= 19)goto _LL58;_LL57:({void*_tmp164[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp165="Toc::typ_to_c: type translation passed a type integer";_tag_dynforward(
_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,54));}),_tag_dynforward(_tmp164,
sizeof(void*),0));});_LL58: if((int)_tmp11B != 2)goto _LL5A;_LL59:({void*_tmp166[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp167="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,55));}),
_tag_dynforward(_tmp166,sizeof(void*),0));});_LL5A: if((int)_tmp11B != 3)goto _LL5C;
_LL5B:({void*_tmp168[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp169="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,57));}),
_tag_dynforward(_tmp168,sizeof(void*),0));});_LL5C: if(_tmp11B <= (void*)4)goto
_LL5E;if(*((int*)_tmp11B)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmp11B <= (
void*)4)goto _LL60;if(*((int*)_tmp11B)!= 21)goto _LL60;_LL5F: goto _LL61;_LL60: if(
_tmp11B <= (void*)4)goto _LL25;if(*((int*)_tmp11B)!= 22)goto _LL25;_LL61:({void*
_tmp16A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp16B="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,49));}),
_tag_dynforward(_tmp16A,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp16C=t;struct Cyc_Absyn_ArrayInfo _tmp16D;void*_tmp16E;struct Cyc_Absyn_Tqual
_tmp16F;_LL77: if(_tmp16C <= (void*)4)goto _LL79;if(*((int*)_tmp16C)!= 7)goto _LL79;
_tmp16D=((struct Cyc_Absyn_ArrayType_struct*)_tmp16C)->f1;_tmp16E=(void*)_tmp16D.elt_type;
_tmp16F=_tmp16D.tq;_LL78: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp16E,(void*)
2,_tmp16F,Cyc_Absyn_false_conref),e);_LL79:;_LL7A: return Cyc_Toc_cast_it(t,e);
_LL76:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp170=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp171;void*_tmp172;struct Cyc_Absyn_AggrInfo _tmp173;
union Cyc_Absyn_AggrInfoU_union _tmp174;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_TunionFieldInfo
_tmp176;union Cyc_Absyn_TunionFieldInfoU_union _tmp177;struct Cyc_Absyn_Tuniondecl*
_tmp178;struct Cyc_Absyn_Tunionfield*_tmp179;struct Cyc_List_List*_tmp17A;_LL7C:
if((int)_tmp170 != 0)goto _LL7E;_LL7D: return 1;_LL7E: if(_tmp170 <= (void*)4)goto
_LL86;if(*((int*)_tmp170)!= 1)goto _LL80;_LL7F: return 0;_LL80: if(*((int*)_tmp170)
!= 5)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp170)!= 12)goto _LL84;_LL83:
goto _LL85;_LL84: if(*((int*)_tmp170)!= 13)goto _LL86;_LL85: goto _LL87;_LL86: if((int)
_tmp170 != 1)goto _LL88;_LL87: goto _LL89;_LL88: if(_tmp170 <= (void*)4)goto _LLA0;if(*((
int*)_tmp170)!= 6)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)_tmp170)!= 8)goto
_LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp170)!= 18)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if(*((int*)_tmp170)!= 14)goto _LL90;_LL8F: return 1;_LL90: if(*((int*)_tmp170)
!= 7)goto _LL92;_tmp171=((struct Cyc_Absyn_ArrayType_struct*)_tmp170)->f1;_tmp172=(
void*)_tmp171.elt_type;_LL91: return Cyc_Toc_atomic_typ(_tmp172);_LL92: if(*((int*)
_tmp170)!= 10)goto _LL94;_tmp173=((struct Cyc_Absyn_AggrType_struct*)_tmp170)->f1;
_tmp174=_tmp173.aggr_info;_LL93:{union Cyc_Absyn_AggrInfoU_union _tmp17B=_tmp174;
_LLA3: if((_tmp17B.UnknownAggr).tag != 0)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6: goto
_LLA2;_LLA2:;}{struct Cyc_Absyn_Aggrdecl*_tmp17C=Cyc_Absyn_get_known_aggrdecl(
_tmp174);if(_tmp17C->impl == 0)return 0;{struct Cyc_List_List*_tmp17D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp17C->impl))->fields;for(0;_tmp17D != 0;_tmp17D=_tmp17D->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp17D->hd)->type))
return 0;}}return 1;}_LL94: if(*((int*)_tmp170)!= 11)goto _LL96;_tmp175=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp170)->f2;_LL95: for(0;_tmp175 != 0;_tmp175=_tmp175->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp175->hd)->type))return 0;}return 1;_LL96:
if(*((int*)_tmp170)!= 3)goto _LL98;_tmp176=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp170)->f1;_tmp177=_tmp176.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp170)->f1).field_info).KnownTunionfield).tag != 1)goto _LL98;_tmp178=(_tmp177.KnownTunionfield).f1;
_tmp179=(_tmp177.KnownTunionfield).f2;_LL97: _tmp17A=_tmp179->typs;goto _LL99;
_LL98: if(*((int*)_tmp170)!= 9)goto _LL9A;_tmp17A=((struct Cyc_Absyn_TupleType_struct*)
_tmp170)->f1;_LL99: for(0;_tmp17A != 0;_tmp17A=_tmp17A->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp17A->hd)).f2))return 0;}return 1;_LL9A: if(*((int*)_tmp170)!= 2)
goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp170)!= 4)goto _LL9E;_LL9D: goto
_LL9F;_LL9E: if(*((int*)_tmp170)!= 15)goto _LLA0;_LL9F: return 0;_LLA0:;_LLA1:({
struct Cyc_String_pa_struct _tmp180;_tmp180.tag=0;_tmp180.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp17E[
1]={& _tmp180};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp17F="atomic_typ:  bad type %s";_tag_dynforward(_tmp17F,sizeof(char),
_get_zero_arr_size(_tmp17F,25));}),_tag_dynforward(_tmp17E,sizeof(void*),1));}});
_LL7B:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp181=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp182;void*_tmp183;_LLA8: if(_tmp181 <= (void*)4)goto
_LLAA;if(*((int*)_tmp181)!= 4)goto _LLAA;_tmp182=((struct Cyc_Absyn_PointerType_struct*)
_tmp181)->f1;_tmp183=(void*)_tmp182.elt_typ;_LLA9: {void*_tmp184=Cyc_Tcutil_compress(
_tmp183);_LLAD: if((int)_tmp184 != 0)goto _LLAF;_LLAE: return 1;_LLAF:;_LLB0: return 0;
_LLAC:;}_LLAA:;_LLAB: return 0;_LLA7:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp185=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp186;union Cyc_Absyn_AggrInfoU_union _tmp187;struct Cyc_List_List*_tmp188;_LLB2:
if(_tmp185 <= (void*)4)goto _LLB6;if(*((int*)_tmp185)!= 10)goto _LLB4;_tmp186=((
struct Cyc_Absyn_AggrType_struct*)_tmp185)->f1;_tmp187=_tmp186.aggr_info;_LLB3: {
struct Cyc_Absyn_Aggrdecl*_tmp189=Cyc_Absyn_get_known_aggrdecl(_tmp187);if(
_tmp189->impl == 0)({void*_tmp18A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp18B="is_poly_field: type missing fields";
_tag_dynforward(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,35));}),
_tag_dynforward(_tmp18A,sizeof(void*),0));});_tmp188=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp189->impl))->fields;goto _LLB5;}_LLB4: if(*((int*)_tmp185)!= 11)
goto _LLB6;_tmp188=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp185)->f2;_LLB5: {
struct Cyc_Absyn_Aggrfield*_tmp18C=Cyc_Absyn_lookup_field(_tmp188,f);if(_tmp18C == 
0)({struct Cyc_String_pa_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp18D[1]={& _tmp18F};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp18E="is_poly_field: bad field %s";_tag_dynforward(_tmp18E,sizeof(char),
_get_zero_arr_size(_tmp18E,28));}),_tag_dynforward(_tmp18D,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp18C->type);}_LLB6:;_LLB7:({struct Cyc_String_pa_struct
_tmp192;_tmp192.tag=0;_tmp192.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp190[1]={& _tmp192};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp191="is_poly_field: bad type %s";_tag_dynforward(_tmp191,sizeof(char),
_get_zero_arr_size(_tmp191,27));}),_tag_dynforward(_tmp190,sizeof(void*),1));}});
_LLB1:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp193=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp194;struct _dynforward_ptr*_tmp195;struct Cyc_Absyn_Exp*
_tmp196;struct _dynforward_ptr*_tmp197;_LLB9: if(*((int*)_tmp193)!= 23)goto _LLBB;
_tmp194=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp193)->f1;_tmp195=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp193)->f2;_LLBA: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp194->topt))->v,_tmp195);_LLBB: if(*((int*)_tmp193)!= 24)goto _LLBD;
_tmp196=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp193)->f1;_tmp197=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp193)->f2;_LLBC: {void*_tmp198=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp196->topt))->v);struct Cyc_Absyn_PtrInfo _tmp199;void*_tmp19A;
_LLC0: if(_tmp198 <= (void*)4)goto _LLC2;if(*((int*)_tmp198)!= 4)goto _LLC2;_tmp199=((
struct Cyc_Absyn_PointerType_struct*)_tmp198)->f1;_tmp19A=(void*)_tmp199.elt_typ;
_LLC1: return Cyc_Toc_is_poly_field(_tmp19A,_tmp197);_LLC2:;_LLC3:({struct Cyc_String_pa_struct
_tmp19D;_tmp19D.tag=0;_tmp19D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp196->topt))->v));{
void*_tmp19B[1]={& _tmp19D};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp19C="is_poly_project: bad type %s";
_tag_dynforward(_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,29));}),
_tag_dynforward(_tmp19B,sizeof(void*),1));}});_LLBF:;}_LLBD:;_LLBE: return 0;_LLB8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));
_tmp19E->hd=s;_tmp19E->tl=0;_tmp19E;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=s;_tmp19F->tl=
0;_tmp19F;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp1A0[2];_tmp1A0[1]=s;_tmp1A0[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1A0,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp1A1[2];_tmp1A1[1]=n;
_tmp1A1[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1A1,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp1A2[2];_tmp1A2[1]=n;_tmp1A2[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp1A2,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp1A3[3];_tmp1A3[2]=n;_tmp1A3[1]=
s;_tmp1A3[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1A3,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->hd=e;_tmp1A4->tl=
0;_tmp1A4;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp1A5=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1A6;_LLC5: if(*((int*)
_tmp1A5)!= 0)goto _LLC7;_tmp1A6=((struct Cyc_Absyn_Const_e_struct*)_tmp1A5)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp1A5)->f1).String_c).tag != 5)goto _LLC7;
_LLC6: is_string=1;goto _LLC4;_LLC7:;_LLC8: goto _LLC4;_LLC4:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({
struct Cyc_Absyn_Var_d_struct _tmp1A9;_tmp1A9.tag=0;_tmp1A9.f1=vd;_tmp1A9;});
_tmp1A8;}),0);_tmp1A7->tl=Cyc_Toc_result_decls;_tmp1A7;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp1AA[3];_tmp1AA[2]=({struct _tuple6*_tmp1AD=_cycalloc(
sizeof(*_tmp1AD));_tmp1AD->f1=0;_tmp1AD->f2=xplussz;_tmp1AD;});_tmp1AA[1]=({
struct _tuple6*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->f1=0;_tmp1AC->f2=xexp;
_tmp1AC;});_tmp1AA[0]=({struct _tuple6*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));
_tmp1AB->f1=0;_tmp1AB->f2=xexp;_tmp1AB;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1AA,sizeof(struct _tuple6*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*_tmp1AE[2];_tmp1AE[
1]=({struct _tuple6*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->f1=0;_tmp1B0->f2=
xplussz;_tmp1B0;});_tmp1AE[0]=({struct _tuple6*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));
_tmp1AF->f1=0;_tmp1AF->f2=xexp;_tmp1AF;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1AE,sizeof(struct _tuple6*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp1B2;int _tmp1B3;struct Cyc_Toc_Env*
_tmp1B1=nv;_tmp1B2=*_tmp1B1;_tmp1B3=_tmp1B2.toplevel;return _tmp1B3;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp1B5;struct Cyc_Dict_Dict _tmp1B6;struct Cyc_Toc_Env*_tmp1B4=nv;
_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1B6,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp1B7=
_region_malloc(r,sizeof(*_tmp1B7));_tmp1B7->break_lab=(struct _dynforward_ptr**)0;
_tmp1B7->continue_lab=(struct _dynforward_ptr**)0;_tmp1B7->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp1B7->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp1B7->toplevel=(int)1;_tmp1B7;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1B9;struct _dynforward_ptr**_tmp1BA;struct _dynforward_ptr**_tmp1BB;struct Cyc_Toc_FallthruInfo*
_tmp1BC;struct Cyc_Dict_Dict _tmp1BD;int _tmp1BE;struct Cyc_Toc_Env*_tmp1B8=e;
_tmp1B9=*_tmp1B8;_tmp1BA=_tmp1B9.break_lab;_tmp1BB=_tmp1B9.continue_lab;_tmp1BC=
_tmp1B9.fallthru_info;_tmp1BD=_tmp1B9.varmap;_tmp1BE=_tmp1B9.toplevel;return({
struct Cyc_Toc_Env*_tmp1BF=_region_malloc(r,sizeof(*_tmp1BF));_tmp1BF->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1BA);_tmp1BF->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1BB);_tmp1BF->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1BC;_tmp1BF->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1BD);_tmp1BF->toplevel=(int)_tmp1BE;_tmp1BF;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1C1;struct _dynforward_ptr**_tmp1C2;struct _dynforward_ptr**_tmp1C3;struct Cyc_Toc_FallthruInfo*
_tmp1C4;struct Cyc_Dict_Dict _tmp1C5;int _tmp1C6;struct Cyc_Toc_Env*_tmp1C0=e;
_tmp1C1=*_tmp1C0;_tmp1C2=_tmp1C1.break_lab;_tmp1C3=_tmp1C1.continue_lab;_tmp1C4=
_tmp1C1.fallthru_info;_tmp1C5=_tmp1C1.varmap;_tmp1C6=_tmp1C1.toplevel;return({
struct Cyc_Toc_Env*_tmp1C7=_region_malloc(r,sizeof(*_tmp1C7));_tmp1C7->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C2);_tmp1C7->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C3);_tmp1C7->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1C4;_tmp1C7->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1C5);_tmp1C7->toplevel=(int)0;_tmp1C7;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1C9;struct
_dynforward_ptr**_tmp1CA;struct _dynforward_ptr**_tmp1CB;struct Cyc_Toc_FallthruInfo*
_tmp1CC;struct Cyc_Dict_Dict _tmp1CD;int _tmp1CE;struct Cyc_Toc_Env*_tmp1C8=e;
_tmp1C9=*_tmp1C8;_tmp1CA=_tmp1C9.break_lab;_tmp1CB=_tmp1C9.continue_lab;_tmp1CC=
_tmp1C9.fallthru_info;_tmp1CD=_tmp1C9.varmap;_tmp1CE=_tmp1C9.toplevel;return({
struct Cyc_Toc_Env*_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));_tmp1CF->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1CA);_tmp1CF->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1CB);_tmp1CF->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1CC;_tmp1CF->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1CD);_tmp1CF->toplevel=(int)1;_tmp1CF;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp1D0=(*x).f1;_LLCA: if((_tmp1D0.Rel_n).tag != 1)
goto _LLCC;_LLCB:({struct Cyc_String_pa_struct _tmp1D3;_tmp1D3.tag=0;_tmp1D3.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp1D1[1]={& _tmp1D3};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1D2="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,30));}),
_tag_dynforward(_tmp1D1,sizeof(void*),1));}});_LLCC:;_LLCD: goto _LLC9;_LLC9:;}{
struct Cyc_Toc_Env _tmp1D5;struct _dynforward_ptr**_tmp1D6;struct _dynforward_ptr**
_tmp1D7;struct Cyc_Toc_FallthruInfo*_tmp1D8;struct Cyc_Dict_Dict _tmp1D9;int _tmp1DA;
struct Cyc_Toc_Env*_tmp1D4=e;_tmp1D5=*_tmp1D4;_tmp1D6=_tmp1D5.break_lab;_tmp1D7=
_tmp1D5.continue_lab;_tmp1D8=_tmp1D5.fallthru_info;_tmp1D9=_tmp1D5.varmap;
_tmp1DA=_tmp1D5.toplevel;{struct Cyc_Dict_Dict _tmp1DB=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1D9),x,y);return({struct Cyc_Toc_Env*_tmp1DC=_region_malloc(r,sizeof(*
_tmp1DC));_tmp1DC->break_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1D6);_tmp1DC->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1D7);_tmp1DC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp1D8;_tmp1DC->varmap=(
struct Cyc_Dict_Dict)_tmp1DB;_tmp1DC->toplevel=(int)_tmp1DA;_tmp1DC;});}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp1DE;struct _dynforward_ptr**_tmp1DF;struct _dynforward_ptr**
_tmp1E0;struct Cyc_Toc_FallthruInfo*_tmp1E1;struct Cyc_Dict_Dict _tmp1E2;int _tmp1E3;
struct Cyc_Toc_Env*_tmp1DD=e;_tmp1DE=*_tmp1DD;_tmp1DF=_tmp1DE.break_lab;_tmp1E0=
_tmp1DE.continue_lab;_tmp1E1=_tmp1DE.fallthru_info;_tmp1E2=_tmp1DE.varmap;
_tmp1E3=_tmp1DE.toplevel;return({struct Cyc_Toc_Env*_tmp1E4=_region_malloc(r,
sizeof(*_tmp1E4));_tmp1E4->break_lab=(struct _dynforward_ptr**)0;_tmp1E4->continue_lab=(
struct _dynforward_ptr**)0;_tmp1E4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1E1;_tmp1E4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E2);_tmp1E4->toplevel=(
int)_tmp1E3;_tmp1E4;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct
_dynforward_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp1E5->tl=fallthru_vars;_tmp1E5;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp1E7;struct _dynforward_ptr**_tmp1E8;struct _dynforward_ptr**
_tmp1E9;struct Cyc_Toc_FallthruInfo*_tmp1EA;struct Cyc_Dict_Dict _tmp1EB;int _tmp1EC;
struct Cyc_Toc_Env*_tmp1E6=e;_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E7.break_lab;_tmp1E9=
_tmp1E7.continue_lab;_tmp1EA=_tmp1E7.fallthru_info;_tmp1EB=_tmp1E7.varmap;
_tmp1EC=_tmp1E7.toplevel;{struct Cyc_Toc_Env _tmp1EE;struct Cyc_Dict_Dict _tmp1EF;
struct Cyc_Toc_Env*_tmp1ED=next_case_env;_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1F2=
_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->label=fallthru_l;_tmp1F2->binders=
fallthru_vars;_tmp1F2->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1EF);_tmp1F2;});
return({struct Cyc_Toc_Env*_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1F1=_region_malloc(r,
sizeof(*_tmp1F1));_tmp1F1[0]=break_l;_tmp1F1;});_tmp1F0->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1E9);_tmp1F0->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp1F0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1EB);_tmp1F0->toplevel=(
int)_tmp1EC;_tmp1F0;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp1F4;struct _dynforward_ptr**_tmp1F5;struct _dynforward_ptr**_tmp1F6;
struct Cyc_Toc_FallthruInfo*_tmp1F7;struct Cyc_Dict_Dict _tmp1F8;int _tmp1F9;struct
Cyc_Toc_Env*_tmp1F3=e;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.break_lab;_tmp1F6=_tmp1F4.continue_lab;
_tmp1F7=_tmp1F4.fallthru_info;_tmp1F8=_tmp1F4.varmap;_tmp1F9=_tmp1F4.toplevel;
return({struct Cyc_Toc_Env*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));_tmp1FA->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1FB=_region_malloc(r,
sizeof(*_tmp1FB));_tmp1FB[0]=break_l;_tmp1FB;});_tmp1FA->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1F6);_tmp1FA->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp1FA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1F8);_tmp1FA->toplevel=(
int)_tmp1F9;_tmp1FA;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp1FD;struct _dynforward_ptr**_tmp1FE;struct _dynforward_ptr**_tmp1FF;
struct Cyc_Toc_FallthruInfo*_tmp200;struct Cyc_Dict_Dict _tmp201;int _tmp202;struct
Cyc_Toc_Env*_tmp1FC=e;_tmp1FD=*_tmp1FC;_tmp1FE=_tmp1FD.break_lab;_tmp1FF=_tmp1FD.continue_lab;
_tmp200=_tmp1FD.fallthru_info;_tmp201=_tmp1FD.varmap;_tmp202=_tmp1FD.toplevel;
return({struct Cyc_Toc_Env*_tmp203=_region_malloc(r,sizeof(*_tmp203));_tmp203->break_lab=(
struct _dynforward_ptr**)0;_tmp203->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp1FF);_tmp203->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp204=_region_malloc(r,sizeof(*_tmp204));_tmp204->label=
next_l;_tmp204->binders=0;_tmp204->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp204;});_tmp203->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp201);_tmp203->toplevel=(
int)_tmp202;_tmp203;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp205=(void*)e->annot;
_LLCF: if(*((void**)_tmp205)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD1;_LLD0: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD1: if(*((void**)_tmp205)
!= Cyc_CfFlowInfo_NotZero)goto _LLD3;_LLD2: return 0;_LLD3: if(_tmp205 != Cyc_CfFlowInfo_IsZero)
goto _LLD5;_LLD4:({void*_tmp206[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp207="dereference of NULL pointer";
_tag_dynforward(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,28));}),
_tag_dynforward(_tmp206,sizeof(void*),0));});return 0;_LLD5: if(_tmp205 != Cyc_Absyn_EmptyAnnot)
goto _LLD7;_LLD6: return 0;_LLD7: if(*((void**)_tmp205)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD9;_LLD8:({void*_tmp208[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp209="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,46));}),
_tag_dynforward(_tmp208,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD9:;_LLDA:({void*_tmp20A[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp20B="need_null_check";_tag_dynforward(_tmp20B,sizeof(char),
_get_zero_arr_size(_tmp20B,16));}),_tag_dynforward(_tmp20A,sizeof(void*),0));});
_LLCE:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp20C=(void*)e->annot;struct Cyc_List_List*_tmp20D;struct Cyc_List_List*_tmp20E;
_LLDC: if(*((void**)_tmp20C)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDE;_tmp20D=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp20C)->f1;_LLDD: return _tmp20D;_LLDE: if(*((
void**)_tmp20C)!= Cyc_CfFlowInfo_NotZero)goto _LLE0;_tmp20E=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp20C)->f1;_LLDF: return _tmp20E;_LLE0: if(_tmp20C != Cyc_CfFlowInfo_IsZero)goto
_LLE2;_LLE1:({void*_tmp20F[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp210="dereference of NULL pointer";
_tag_dynforward(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,28));}),
_tag_dynforward(_tmp20F,sizeof(void*),0));});return 0;_LLE2: if(*((void**)_tmp20C)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp20C != Cyc_Absyn_EmptyAnnot)
goto _LLE6;_LLE5: return 0;_LLE6:;_LLE7:({void*_tmp211[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp212="get_relns";_tag_dynforward(_tmp212,sizeof(char),_get_zero_arr_size(
_tmp212,10));}),_tag_dynforward(_tmp211,sizeof(void*),0));});_LLDB:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp213=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp214;struct Cyc_Absyn_PtrAtts _tmp215;struct Cyc_Absyn_Conref*
_tmp216;struct Cyc_Absyn_Conref*_tmp217;struct Cyc_Absyn_ArrayInfo _tmp218;struct
Cyc_Absyn_Exp*_tmp219;_LLE9: if(_tmp213 <= (void*)4)goto _LLED;if(*((int*)_tmp213)
!= 4)goto _LLEB;_tmp214=((struct Cyc_Absyn_PointerType_struct*)_tmp213)->f1;
_tmp215=_tmp214.ptr_atts;_tmp216=_tmp215.bounds;_tmp217=_tmp215.zero_term;_LLEA: {
void*_tmp21A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp216);struct Cyc_Absyn_Exp*
_tmp21B;_LLF0: if((int)_tmp21A != 0)goto _LLF2;_LLF1: return 0;_LLF2: if((int)_tmp21A
!= 1)goto _LLF4;_LLF3: return 0;_LLF4: if(_tmp21A <= (void*)2)goto _LLF6;if(*((int*)
_tmp21A)!= 0)goto _LLF6;_tmp21B=((struct Cyc_Absyn_Upper_b_struct*)_tmp21A)->f1;
_LLF5: {unsigned int _tmp21D;int _tmp21E;struct _tuple5 _tmp21C=Cyc_Evexp_eval_const_uint_exp(
_tmp21B);_tmp21D=_tmp21C.f1;_tmp21E=_tmp21C.f2;return _tmp21E  && i <= _tmp21D;}
_LLF6: if(_tmp21A <= (void*)2)goto _LLEF;if(*((int*)_tmp21A)!= 1)goto _LLEF;_LLF7:({
void*_tmp21F[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp220="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,30));}),
_tag_dynforward(_tmp21F,sizeof(void*),0));});_LLEF:;}_LLEB: if(*((int*)_tmp213)!= 
7)goto _LLED;_tmp218=((struct Cyc_Absyn_ArrayType_struct*)_tmp213)->f1;_tmp219=
_tmp218.num_elts;_LLEC: if(_tmp219 == 0)return 0;{unsigned int _tmp222;int _tmp223;
struct _tuple5 _tmp221=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp219);
_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;return _tmp223  && i <= _tmp222;}_LLED:;_LLEE:
return 0;_LLE8:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp224=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp224->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp225=_tmp224->rop;
struct Cyc_Absyn_Vardecl*_tmp226;struct Cyc_Absyn_Vardecl*_tmp227;_LLF9: if((
_tmp225.LessSize).tag != 2)goto _LLFB;_tmp226=(_tmp225.LessSize).f1;_LLFA: _tmp227=
_tmp226;goto _LLFC;_LLFB: if((_tmp225.LessEqSize).tag != 4)goto _LLFD;_tmp227=(
_tmp225.LessEqSize).f1;_LLFC: if(_tmp227 == v)return 1;else{goto _LLF8;}_LLFD:;_LLFE:
continue;_LLF8:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp228=(void*)e->r;
void*_tmp229;struct Cyc_Absyn_Vardecl*_tmp22A;void*_tmp22B;struct Cyc_Absyn_Vardecl*
_tmp22C;void*_tmp22D;struct Cyc_Absyn_Vardecl*_tmp22E;void*_tmp22F;struct Cyc_Absyn_Vardecl*
_tmp230;void*_tmp231;struct Cyc_List_List*_tmp232;struct Cyc_List_List _tmp233;
struct Cyc_Absyn_Exp*_tmp234;_LL100: if(*((int*)_tmp228)!= 1)goto _LL102;_tmp229=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp228)->f2;if(_tmp229 <= (void*)1)goto
_LL102;if(*((int*)_tmp229)!= 4)goto _LL102;_tmp22A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp229)->f1;_LL101: _tmp22C=_tmp22A;goto _LL103;_LL102: if(*((int*)_tmp228)!= 1)
goto _LL104;_tmp22B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp228)->f2;if(
_tmp22B <= (void*)1)goto _LL104;if(*((int*)_tmp22B)!= 3)goto _LL104;_tmp22C=((
struct Cyc_Absyn_Local_b_struct*)_tmp22B)->f1;_LL103: _tmp22E=_tmp22C;goto _LL105;
_LL104: if(*((int*)_tmp228)!= 1)goto _LL106;_tmp22D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp228)->f2;if(_tmp22D <= (void*)1)goto _LL106;if(*((int*)_tmp22D)!= 0)goto _LL106;
_tmp22E=((struct Cyc_Absyn_Global_b_struct*)_tmp22D)->f1;_LL105: _tmp230=_tmp22E;
goto _LL107;_LL106: if(*((int*)_tmp228)!= 1)goto _LL108;_tmp22F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp228)->f2;if(_tmp22F <= (void*)1)goto _LL108;if(*((int*)_tmp22F)!= 2)goto _LL108;
_tmp230=((struct Cyc_Absyn_Param_b_struct*)_tmp22F)->f1;_LL107: if(_tmp230->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp230))return 1;goto _LLFF;_LL108:
if(*((int*)_tmp228)!= 3)goto _LL10A;_tmp231=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp228)->f1;if((int)_tmp231 != 19)goto _LL10A;_tmp232=((struct Cyc_Absyn_Primop_e_struct*)
_tmp228)->f2;if(_tmp232 == 0)goto _LL10A;_tmp233=*_tmp232;_tmp234=(struct Cyc_Absyn_Exp*)
_tmp233.hd;_LL109:{void*_tmp235=(void*)_tmp234->r;void*_tmp236;struct Cyc_Absyn_Vardecl*
_tmp237;void*_tmp238;struct Cyc_Absyn_Vardecl*_tmp239;void*_tmp23A;struct Cyc_Absyn_Vardecl*
_tmp23B;void*_tmp23C;struct Cyc_Absyn_Vardecl*_tmp23D;_LL10D: if(*((int*)_tmp235)
!= 1)goto _LL10F;_tmp236=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp235)->f2;if(
_tmp236 <= (void*)1)goto _LL10F;if(*((int*)_tmp236)!= 4)goto _LL10F;_tmp237=((
struct Cyc_Absyn_Pat_b_struct*)_tmp236)->f1;_LL10E: _tmp239=_tmp237;goto _LL110;
_LL10F: if(*((int*)_tmp235)!= 1)goto _LL111;_tmp238=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp238 <= (void*)1)goto _LL111;if(*((int*)_tmp238)!= 3)goto _LL111;
_tmp239=((struct Cyc_Absyn_Local_b_struct*)_tmp238)->f1;_LL110: _tmp23B=_tmp239;
goto _LL112;_LL111: if(*((int*)_tmp235)!= 1)goto _LL113;_tmp23A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp23A <= (void*)1)goto _LL113;if(*((int*)_tmp23A)!= 0)goto _LL113;
_tmp23B=((struct Cyc_Absyn_Global_b_struct*)_tmp23A)->f1;_LL112: _tmp23D=_tmp23B;
goto _LL114;_LL113: if(*((int*)_tmp235)!= 1)goto _LL115;_tmp23C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp23C <= (void*)1)goto _LL115;if(*((int*)_tmp23C)!= 2)goto _LL115;
_tmp23D=((struct Cyc_Absyn_Param_b_struct*)_tmp23C)->f1;_LL114: return _tmp23D == v;
_LL115:;_LL116: goto _LL10C;_LL10C:;}goto _LLFF;_LL10A:;_LL10B: goto _LLFF;_LLFF:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp23E=(void*)a->r;void*_tmp23F;struct Cyc_Absyn_Vardecl*
_tmp240;void*_tmp241;struct Cyc_Absyn_Vardecl*_tmp242;void*_tmp243;struct Cyc_Absyn_Vardecl*
_tmp244;void*_tmp245;struct Cyc_Absyn_Vardecl*_tmp246;_LL118: if(*((int*)_tmp23E)
!= 1)goto _LL11A;_tmp23F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23E)->f2;if(
_tmp23F <= (void*)1)goto _LL11A;if(*((int*)_tmp23F)!= 4)goto _LL11A;_tmp240=((
struct Cyc_Absyn_Pat_b_struct*)_tmp23F)->f1;_LL119: _tmp242=_tmp240;goto _LL11B;
_LL11A: if(*((int*)_tmp23E)!= 1)goto _LL11C;_tmp241=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp241 <= (void*)1)goto _LL11C;if(*((int*)_tmp241)!= 3)goto _LL11C;
_tmp242=((struct Cyc_Absyn_Local_b_struct*)_tmp241)->f1;_LL11B: _tmp244=_tmp242;
goto _LL11D;_LL11C: if(*((int*)_tmp23E)!= 1)goto _LL11E;_tmp243=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp243 <= (void*)1)goto _LL11E;if(*((int*)_tmp243)!= 0)goto _LL11E;
_tmp244=((struct Cyc_Absyn_Global_b_struct*)_tmp243)->f1;_LL11D: _tmp246=_tmp244;
goto _LL11F;_LL11E: if(*((int*)_tmp23E)!= 1)goto _LL120;_tmp245=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp245 <= (void*)1)goto _LL120;if(*((int*)_tmp245)!= 2)goto _LL120;
_tmp246=((struct Cyc_Absyn_Param_b_struct*)_tmp245)->f1;_LL11F: if(_tmp246->escapes)
return 0;inner_loop: {void*_tmp247=(void*)i->r;void*_tmp248;struct Cyc_Absyn_Exp*
_tmp249;union Cyc_Absyn_Cnst_union _tmp24A;void*_tmp24B;int _tmp24C;union Cyc_Absyn_Cnst_union
_tmp24D;void*_tmp24E;int _tmp24F;union Cyc_Absyn_Cnst_union _tmp250;void*_tmp251;
int _tmp252;void*_tmp253;struct Cyc_List_List*_tmp254;struct Cyc_List_List _tmp255;
struct Cyc_Absyn_Exp*_tmp256;struct Cyc_List_List*_tmp257;struct Cyc_List_List
_tmp258;struct Cyc_Absyn_Exp*_tmp259;void*_tmp25A;struct Cyc_Absyn_Vardecl*_tmp25B;
void*_tmp25C;struct Cyc_Absyn_Vardecl*_tmp25D;void*_tmp25E;struct Cyc_Absyn_Vardecl*
_tmp25F;void*_tmp260;struct Cyc_Absyn_Vardecl*_tmp261;_LL123: if(*((int*)_tmp247)
!= 15)goto _LL125;_tmp248=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp247)->f1;
_tmp249=((struct Cyc_Absyn_Cast_e_struct*)_tmp247)->f2;_LL124: i=_tmp249;goto
inner_loop;_LL125: if(*((int*)_tmp247)!= 0)goto _LL127;_tmp24A=((struct Cyc_Absyn_Const_e_struct*)
_tmp247)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp247)->f1).Int_c).tag != 2)
goto _LL127;_tmp24B=(_tmp24A.Int_c).f1;if((int)_tmp24B != 2)goto _LL127;_tmp24C=(
_tmp24A.Int_c).f2;_LL126: _tmp24F=_tmp24C;goto _LL128;_LL127: if(*((int*)_tmp247)!= 
0)goto _LL129;_tmp24D=((struct Cyc_Absyn_Const_e_struct*)_tmp247)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp247)->f1).Int_c).tag != 2)goto _LL129;_tmp24E=(
_tmp24D.Int_c).f1;if((int)_tmp24E != 0)goto _LL129;_tmp24F=(_tmp24D.Int_c).f2;
_LL128: return _tmp24F >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp24F + 1),(
void*)_tmp246->type);_LL129: if(*((int*)_tmp247)!= 0)goto _LL12B;_tmp250=((struct
Cyc_Absyn_Const_e_struct*)_tmp247)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp247)->f1).Int_c).tag != 2)goto _LL12B;_tmp251=(_tmp250.Int_c).f1;if((int)
_tmp251 != 1)goto _LL12B;_tmp252=(_tmp250.Int_c).f2;_LL12A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp252 + 1),(void*)_tmp246->type);_LL12B: if(*((int*)_tmp247)!= 3)
goto _LL12D;_tmp253=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp247)->f1;if((
int)_tmp253 != 4)goto _LL12D;_tmp254=((struct Cyc_Absyn_Primop_e_struct*)_tmp247)->f2;
if(_tmp254 == 0)goto _LL12D;_tmp255=*_tmp254;_tmp256=(struct Cyc_Absyn_Exp*)_tmp255.hd;
_tmp257=_tmp255.tl;if(_tmp257 == 0)goto _LL12D;_tmp258=*_tmp257;_tmp259=(struct Cyc_Absyn_Exp*)
_tmp258.hd;_LL12C: return Cyc_Toc_check_leq_size(relns,_tmp246,_tmp259);_LL12D: if(*((
int*)_tmp247)!= 1)goto _LL12F;_tmp25A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp247)->f2;if(_tmp25A <= (void*)1)goto _LL12F;if(*((int*)_tmp25A)!= 4)goto _LL12F;
_tmp25B=((struct Cyc_Absyn_Pat_b_struct*)_tmp25A)->f1;_LL12E: _tmp25D=_tmp25B;goto
_LL130;_LL12F: if(*((int*)_tmp247)!= 1)goto _LL131;_tmp25C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp247)->f2;if(_tmp25C <= (void*)1)goto _LL131;if(*((int*)_tmp25C)!= 3)goto _LL131;
_tmp25D=((struct Cyc_Absyn_Local_b_struct*)_tmp25C)->f1;_LL130: _tmp25F=_tmp25D;
goto _LL132;_LL131: if(*((int*)_tmp247)!= 1)goto _LL133;_tmp25E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp247)->f2;if(_tmp25E <= (void*)1)goto _LL133;if(*((int*)_tmp25E)!= 0)goto _LL133;
_tmp25F=((struct Cyc_Absyn_Global_b_struct*)_tmp25E)->f1;_LL132: _tmp261=_tmp25F;
goto _LL134;_LL133: if(*((int*)_tmp247)!= 1)goto _LL135;_tmp260=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp247)->f2;if(_tmp260 <= (void*)1)goto _LL135;if(*((int*)_tmp260)!= 2)goto _LL135;
_tmp261=((struct Cyc_Absyn_Param_b_struct*)_tmp260)->f1;_LL134: if(_tmp261->escapes)
return 0;{struct Cyc_List_List*_tmp262=relns;for(0;_tmp262 != 0;_tmp262=_tmp262->tl){
struct Cyc_CfFlowInfo_Reln*_tmp263=(struct Cyc_CfFlowInfo_Reln*)_tmp262->hd;if(
_tmp263->vd == _tmp261){union Cyc_CfFlowInfo_RelnOp_union _tmp264=_tmp263->rop;
struct Cyc_Absyn_Vardecl*_tmp265;struct Cyc_Absyn_Vardecl*_tmp266;unsigned int
_tmp267;_LL138: if((_tmp264.LessSize).tag != 2)goto _LL13A;_tmp265=(_tmp264.LessSize).f1;
_LL139: if(_tmp246 == _tmp265)return 1;else{goto _LL137;}_LL13A: if((_tmp264.LessVar).tag
!= 1)goto _LL13C;_tmp266=(_tmp264.LessVar).f1;_LL13B:{struct Cyc_List_List*_tmp268=
relns;for(0;_tmp268 != 0;_tmp268=_tmp268->tl){struct Cyc_CfFlowInfo_Reln*_tmp269=(
struct Cyc_CfFlowInfo_Reln*)_tmp268->hd;if(_tmp269->vd == _tmp266){union Cyc_CfFlowInfo_RelnOp_union
_tmp26A=_tmp269->rop;struct Cyc_Absyn_Vardecl*_tmp26B;struct Cyc_Absyn_Vardecl*
_tmp26C;unsigned int _tmp26D;struct Cyc_Absyn_Vardecl*_tmp26E;_LL141: if((_tmp26A.LessEqSize).tag
!= 4)goto _LL143;_tmp26B=(_tmp26A.LessEqSize).f1;_LL142: _tmp26C=_tmp26B;goto
_LL144;_LL143: if((_tmp26A.LessSize).tag != 2)goto _LL145;_tmp26C=(_tmp26A.LessSize).f1;
_LL144: if(_tmp246 == _tmp26C)return 1;goto _LL140;_LL145: if((_tmp26A.EqualConst).tag
!= 0)goto _LL147;_tmp26D=(_tmp26A.EqualConst).f1;_LL146: return Cyc_Toc_check_const_array(
_tmp26D,(void*)_tmp246->type);_LL147: if((_tmp26A.LessVar).tag != 1)goto _LL149;
_tmp26E=(_tmp26A.LessVar).f1;_LL148: if(Cyc_Toc_check_leq_size_var(relns,_tmp246,
_tmp26E))return 1;goto _LL140;_LL149:;_LL14A: goto _LL140;_LL140:;}}}goto _LL137;
_LL13C: if((_tmp264.LessConst).tag != 3)goto _LL13E;_tmp267=(_tmp264.LessConst).f1;
_LL13D: return Cyc_Toc_check_const_array(_tmp267,(void*)_tmp246->type);_LL13E:;
_LL13F: goto _LL137;_LL137:;}}}goto _LL122;_LL135:;_LL136: goto _LL122;_LL122:;}goto
_LL117;_LL120:;_LL121: goto _LL117;_LL117:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp26F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp270="Missing type in primop ";_tag_dynforward(_tmp270,sizeof(char),
_get_zero_arr_size(_tmp270,24));}),_tag_dynforward(_tmp26F,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp271[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp272="Missing type in primop ";_tag_dynforward(_tmp272,sizeof(char),
_get_zero_arr_size(_tmp272,24));}),_tag_dynforward(_tmp271,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp273=_cycalloc(
sizeof(*_tmp273));_tmp273->f1=Cyc_Toc_mt_tq;_tmp273->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp273;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->f1=0;
_tmp274->f2=e;_tmp274;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp275=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp275):
Cyc_Toc_malloc_ptr(_tmp275));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp275);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp276=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp276 != 0;_tmp276=_tmp276->tl){struct _tuple6 _tmp278;struct Cyc_List_List*
_tmp279;struct Cyc_Absyn_Exp*_tmp27A;struct _tuple6*_tmp277=(struct _tuple6*)
_tmp276->hd;_tmp278=*_tmp277;_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp279 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp279->tl != 0)({void*_tmp27B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp27C="multiple designators in array";
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,30));}),
_tag_dynforward(_tmp27B,sizeof(void*),0));});{void*_tmp27D=(void*)_tmp279->hd;
void*_tmp27E=_tmp27D;struct Cyc_Absyn_Exp*_tmp27F;_LL14C: if(*((int*)_tmp27E)!= 0)
goto _LL14E;_tmp27F=((struct Cyc_Absyn_ArrayElement_struct*)_tmp27E)->f1;_LL14D:
Cyc_Toc_exp_to_c(nv,_tmp27F);e_index=_tmp27F;goto _LL14B;_LL14E: if(*((int*)
_tmp27E)!= 1)goto _LL14B;_LL14F:({void*_tmp280[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp281="field name designators in array";
_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,32));}),
_tag_dynforward(_tmp280,sizeof(void*),0));});_LL14B:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp282=(void*)_tmp27A->r;struct Cyc_List_List*
_tmp283;struct Cyc_Absyn_Vardecl*_tmp284;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Exp*
_tmp286;int _tmp287;void*_tmp288;struct Cyc_List_List*_tmp289;_LL151: if(*((int*)
_tmp282)!= 28)goto _LL153;_tmp283=((struct Cyc_Absyn_Array_e_struct*)_tmp282)->f1;
_LL152: s=Cyc_Toc_init_array(nv,lval,_tmp283,s);goto _LL150;_LL153: if(*((int*)
_tmp282)!= 29)goto _LL155;_tmp284=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp282)->f1;_tmp285=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp282)->f2;
_tmp286=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp282)->f3;_tmp287=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp282)->f4;_LL154: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp284,_tmp285,_tmp286,_tmp287,s,0);goto _LL150;_LL155: if(*((int*)
_tmp282)!= 31)goto _LL157;_tmp288=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp282)->f1;_tmp289=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp282)->f2;_LL156:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp288,_tmp289,s);goto _LL150;_LL157:;_LL158:
Cyc_Toc_exp_to_c(nv,_tmp27A);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp27A,0),s,0);goto _LL150;_LL150:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp28A=vd->name;
void*_tmp28B=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp28C=
_new_region("r2");struct _RegionHandle*r2=& _tmp28C;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp28A,Cyc_Absyn_varb_exp(_tmp28A,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=({struct
Cyc_Absyn_Local_b_struct _tmp297;_tmp297.tag=3;_tmp297.f1=vd;_tmp297;});_tmp296;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp28A,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp28A,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp28A,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp28A,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp28D=(void*)e2->r;struct Cyc_List_List*
_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*
_tmp291;int _tmp292;void*_tmp293;struct Cyc_List_List*_tmp294;_LL15A: if(*((int*)
_tmp28D)!= 28)goto _LL15C;_tmp28E=((struct Cyc_Absyn_Array_e_struct*)_tmp28D)->f1;
_LL15B: body=Cyc_Toc_init_array(nv2,lval,_tmp28E,Cyc_Toc_skip_stmt_dl());goto
_LL159;_LL15C: if(*((int*)_tmp28D)!= 29)goto _LL15E;_tmp28F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp28D)->f1;_tmp290=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp28D)->f2;
_tmp291=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp28D)->f3;_tmp292=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp28D)->f4;_LL15D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp28F,_tmp290,_tmp291,_tmp292,Cyc_Toc_skip_stmt_dl(),0);goto _LL159;
_LL15E: if(*((int*)_tmp28D)!= 31)goto _LL160;_tmp293=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp28D)->f1;_tmp294=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp28D)->f2;_LL15F:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp293,_tmp294,Cyc_Toc_skip_stmt_dl());
goto _LL159;_LL160:;_LL161: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL159;_LL159:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp28B,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp295=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp28A,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp295;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp298=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp298 != 0;_tmp298=_tmp298->tl){struct _tuple6 _tmp29A;
struct Cyc_List_List*_tmp29B;struct Cyc_Absyn_Exp*_tmp29C;struct _tuple6*_tmp299=(
struct _tuple6*)_tmp298->hd;_tmp29A=*_tmp299;_tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;
if(_tmp29B == 0)({void*_tmp29D[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp29E="empty designator list";
_tag_dynforward(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,22));}),
_tag_dynforward(_tmp29D,sizeof(void*),0));});if(_tmp29B->tl != 0)({void*_tmp29F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2A0="too many designators in anonymous struct";_tag_dynforward(
_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,41));}),_tag_dynforward(_tmp29F,
sizeof(void*),0));});{void*_tmp2A1=(void*)_tmp29B->hd;struct _dynforward_ptr*
_tmp2A2;_LL163: if(*((int*)_tmp2A1)!= 1)goto _LL165;_tmp2A2=((struct Cyc_Absyn_FieldName_struct*)
_tmp2A1)->f1;_LL164: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp2A2,0);{void*_tmp2A3=(void*)_tmp29C->r;struct Cyc_List_List*_tmp2A4;struct Cyc_Absyn_Vardecl*
_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A7;int _tmp2A8;void*
_tmp2A9;struct Cyc_List_List*_tmp2AA;_LL168: if(*((int*)_tmp2A3)!= 28)goto _LL16A;
_tmp2A4=((struct Cyc_Absyn_Array_e_struct*)_tmp2A3)->f1;_LL169: s=Cyc_Toc_init_array(
nv,lval,_tmp2A4,s);goto _LL167;_LL16A: if(*((int*)_tmp2A3)!= 29)goto _LL16C;_tmp2A5=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A3)->f1;_tmp2A6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2A3)->f2;_tmp2A7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A3)->f3;
_tmp2A8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A3)->f4;_LL16B: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2A5,_tmp2A6,_tmp2A7,_tmp2A8,s,0);goto _LL167;_LL16C: if(*((int*)
_tmp2A3)!= 31)goto _LL16E;_tmp2A9=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2A3)->f1;_tmp2AA=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2A3)->f2;_LL16D:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2A9,_tmp2AA,s);goto _LL167;_LL16E:;_LL16F:
Cyc_Toc_exp_to_c(nv,_tmp29C);if(Cyc_Toc_is_poly_field(struct_type,_tmp2A2))
_tmp29C=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp29C);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp29C,0),0),s,0);goto _LL167;
_LL167:;}goto _LL162;}_LL165:;_LL166:({void*_tmp2AB[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2AC="array designator in struct";_tag_dynforward(_tmp2AC,sizeof(char),
_get_zero_arr_size(_tmp2AC,27));}),_tag_dynforward(_tmp2AB,sizeof(void*),0));});
_LL162:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp2AD=_new_region("r");struct _RegionHandle*r=& _tmp2AD;
_push_region(r);{struct Cyc_List_List*_tmp2AE=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _dynforward_ptr*_tmp2AF=Cyc_Toc_add_tuple_type(
_tmp2AE);struct _tuple1*_tmp2B0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2B1=
Cyc_Absyn_var_exp(_tmp2B0,0);struct Cyc_Absyn_Stmt*_tmp2B2=Cyc_Absyn_exp_stmt(
_tmp2B1,0);struct Cyc_Absyn_Exp*(*_tmp2B3)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp2B4=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2B4);for(0;_tmp2B4 != 0;(_tmp2B4=
_tmp2B4->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2B4->hd;struct
Cyc_Absyn_Exp*lval=_tmp2B3(_tmp2B1,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2B5=(void*)e->r;struct Cyc_List_List*_tmp2B6;struct Cyc_Absyn_Vardecl*
_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B9;int _tmp2BA;
_LL171: if(*((int*)_tmp2B5)!= 28)goto _LL173;_tmp2B6=((struct Cyc_Absyn_Array_e_struct*)
_tmp2B5)->f1;_LL172: _tmp2B2=Cyc_Toc_init_array(nv,lval,_tmp2B6,_tmp2B2);goto
_LL170;_LL173: if(*((int*)_tmp2B5)!= 29)goto _LL175;_tmp2B7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2B5)->f1;_tmp2B8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B5)->f2;
_tmp2B9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B5)->f3;_tmp2BA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2B5)->f4;_LL174: _tmp2B2=Cyc_Toc_init_comprehension(
nv,lval,_tmp2B7,_tmp2B8,_tmp2B9,_tmp2BA,_tmp2B2,0);goto _LL170;_LL175:;_LL176: Cyc_Toc_exp_to_c(
nv,e);_tmp2B2=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2B3(
_tmp2B1,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp2B2,0);goto _LL170;_LL170:;}}}{
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Toc_make_struct(nv,_tmp2B0,Cyc_Absyn_strct(
_tmp2AF),_tmp2B2,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp2BB;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp2BC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_var_exp(_tmp2BC,0);struct Cyc_Absyn_Stmt*
_tmp2BE=Cyc_Absyn_exp_stmt(_tmp2BD,0);struct Cyc_Absyn_Exp*(*_tmp2BF)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp2C0=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp2C1;union
Cyc_Absyn_AggrInfoU_union _tmp2C2;_LL178: if(_tmp2C0 <= (void*)4)goto _LL17A;if(*((
int*)_tmp2C0)!= 10)goto _LL17A;_tmp2C1=((struct Cyc_Absyn_AggrType_struct*)_tmp2C0)->f1;
_tmp2C2=_tmp2C1.aggr_info;_LL179: ad=Cyc_Absyn_get_known_aggrdecl(_tmp2C2);goto
_LL177;_LL17A:;_LL17B:({void*_tmp2C3[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2C4="init_struct: bad struct type";
_tag_dynforward(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,29));}),
_tag_dynforward(_tmp2C3,sizeof(void*),0));});_LL177:;}{struct _RegionHandle
_tmp2C5=_new_region("r");struct _RegionHandle*r=& _tmp2C5;_push_region(r);{struct
Cyc_List_List*_tmp2C6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp2C6 != 0;_tmp2C6=_tmp2C6->tl){struct _tuple6
_tmp2C8;struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;struct _tuple6*
_tmp2C7=(struct _tuple6*)_tmp2C6->hd;_tmp2C8=*_tmp2C7;_tmp2C9=_tmp2C8.f1;_tmp2CA=
_tmp2C8.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CA->topt))->v);if(_tmp2C9 == 0)({void*_tmp2CB[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2CC="empty designator list";_tag_dynforward(_tmp2CC,sizeof(char),
_get_zero_arr_size(_tmp2CC,22));}),_tag_dynforward(_tmp2CB,sizeof(void*),0));});
if(_tmp2C9->tl != 0){struct _tuple1*_tmp2CD=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp2CE=Cyc_Absyn_var_exp(_tmp2CD,0);for(0;_tmp2C9 != 0;_tmp2C9=_tmp2C9->tl){void*
_tmp2CF=(void*)_tmp2C9->hd;struct _dynforward_ptr*_tmp2D0;_LL17D: if(*((int*)
_tmp2CF)!= 1)goto _LL17F;_tmp2D0=((struct Cyc_Absyn_FieldName_struct*)_tmp2CF)->f1;
_LL17E: if(Cyc_Toc_is_poly_field(struct_type,_tmp2D0))_tmp2CE=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2CE);_tmp2BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2BF(_tmp2BD,_tmp2D0,0),_tmp2CE,0),0),_tmp2BE,0);goto _LL17C;_LL17F:;_LL180:({
void*_tmp2D1[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp2D2="array designator in struct";
_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,27));}),
_tag_dynforward(_tmp2D1,sizeof(void*),0));});_LL17C:;}Cyc_Toc_exp_to_c(nv,
_tmp2CA);_tmp2BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2CE,_tmp2CA,0),0),_tmp2BE,0);}else{void*_tmp2D3=(void*)_tmp2C9->hd;struct
_dynforward_ptr*_tmp2D4;_LL182: if(*((int*)_tmp2D3)!= 1)goto _LL184;_tmp2D4=((
struct Cyc_Absyn_FieldName_struct*)_tmp2D3)->f1;_LL183: {struct Cyc_Absyn_Exp*lval=
_tmp2BF(_tmp2BD,_tmp2D4,0);{void*_tmp2D5=(void*)_tmp2CA->r;struct Cyc_List_List*
_tmp2D6;struct Cyc_Absyn_Vardecl*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*
_tmp2D9;int _tmp2DA;void*_tmp2DB;struct Cyc_List_List*_tmp2DC;_LL187: if(*((int*)
_tmp2D5)!= 28)goto _LL189;_tmp2D6=((struct Cyc_Absyn_Array_e_struct*)_tmp2D5)->f1;
_LL188: _tmp2BE=Cyc_Toc_init_array(nv,lval,_tmp2D6,_tmp2BE);goto _LL186;_LL189: if(*((
int*)_tmp2D5)!= 29)goto _LL18B;_tmp2D7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2D5)->f1;_tmp2D8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D5)->f2;
_tmp2D9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D5)->f3;_tmp2DA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2D5)->f4;_LL18A: _tmp2BE=Cyc_Toc_init_comprehension(
nv,lval,_tmp2D7,_tmp2D8,_tmp2D9,_tmp2DA,_tmp2BE,0);goto _LL186;_LL18B: if(*((int*)
_tmp2D5)!= 31)goto _LL18D;_tmp2DB=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2D5)->f1;_tmp2DC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2D5)->f2;_LL18C:
_tmp2BE=Cyc_Toc_init_anon_struct(nv,lval,_tmp2DB,_tmp2DC,_tmp2BE);goto _LL186;
_LL18D:;_LL18E: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CA->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp2CA);{struct Cyc_Absyn_Aggrfield*
_tmp2DD=Cyc_Absyn_lookup_decl_field(ad,_tmp2D4);if(Cyc_Toc_is_poly_field(
struct_type,_tmp2D4) && !was_ptr_type)_tmp2CA=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2CA);if(has_exists)_tmp2CA=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp2DD))->type,_tmp2CA);_tmp2BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp2CA,0),0),_tmp2BE,0);goto _LL186;}}_LL186:;}goto
_LL181;}_LL184:;_LL185:({void*_tmp2DE[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2DF="array designator in struct";
_tag_dynforward(_tmp2DF,sizeof(char),_get_zero_arr_size(_tmp2DF,27));}),
_tag_dynforward(_tmp2DE,sizeof(void*),0));});_LL181:;}}}{struct Cyc_Absyn_Exp*
_tmp2E0=Cyc_Toc_make_struct(nv,_tmp2BC,Cyc_Absyn_strctq(tdn),_tmp2BE,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp2E0;};_pop_region(r);}}struct
_tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));
_tmp2E1[0]=({struct Cyc_Absyn_Increment_e_struct _tmp2E2;_tmp2E2.tag=5;_tmp2E2.f1=
e1;_tmp2E2.f2=(void*)incr;_tmp2E2;});_tmp2E1;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp2E3=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp2E4;void*_tmp2E5;struct Cyc_Absyn_Exp*
_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E7;struct _dynforward_ptr*_tmp2E8;_LL190: if(*((
int*)_tmp2E3)!= 38)goto _LL192;_tmp2E4=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp2E3)->f1;_LL191: Cyc_Toc_lvalue_assign_stmt(_tmp2E4,fs,f,f_env);goto _LL18F;
_LL192: if(*((int*)_tmp2E3)!= 15)goto _LL194;_tmp2E5=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2E3)->f1;_tmp2E6=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E3)->f2;_LL193: Cyc_Toc_lvalue_assign(
_tmp2E6,fs,f,f_env);goto _LL18F;_LL194: if(*((int*)_tmp2E3)!= 23)goto _LL196;
_tmp2E7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2E3)->f1;_tmp2E8=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2E3)->f2;_LL195:(void*)(e1->r=(void*)((void*)_tmp2E7->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));
_tmp2E9->hd=_tmp2E8;_tmp2E9->tl=fs;_tmp2E9;}),f,f_env);goto _LL18F;_LL196:;_LL197: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL18F;}_LL18F:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp2EA=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp2EB;struct Cyc_Absyn_Decl*_tmp2EC;struct Cyc_Absyn_Stmt*
_tmp2ED;struct Cyc_Absyn_Stmt*_tmp2EE;_LL199: if(_tmp2EA <= (void*)1)goto _LL19F;if(*((
int*)_tmp2EA)!= 0)goto _LL19B;_tmp2EB=((struct Cyc_Absyn_Exp_s_struct*)_tmp2EA)->f1;
_LL19A: Cyc_Toc_lvalue_assign(_tmp2EB,fs,f,f_env);goto _LL198;_LL19B: if(*((int*)
_tmp2EA)!= 11)goto _LL19D;_tmp2EC=((struct Cyc_Absyn_Decl_s_struct*)_tmp2EA)->f1;
_tmp2ED=((struct Cyc_Absyn_Decl_s_struct*)_tmp2EA)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(
_tmp2ED,fs,f,f_env);goto _LL198;_LL19D: if(*((int*)_tmp2EA)!= 1)goto _LL19F;_tmp2EE=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2EA)->f2;_LL19E: Cyc_Toc_lvalue_assign_stmt(
_tmp2EE,fs,f,f_env);goto _LL198;_LL19F:;_LL1A0:({struct Cyc_String_pa_struct
_tmp2F1;_tmp2F1.tag=0;_tmp2F1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_stmt2string(s));{void*_tmp2EF[1]={& _tmp2F1};Cyc_Toc_toc_impos(({
const char*_tmp2F0="lvalue_assign_stmt: %s";_tag_dynforward(_tmp2F0,sizeof(char),
_get_zero_arr_size(_tmp2F0,23));}),_tag_dynforward(_tmp2EF,sizeof(void*),1));}});
_LL198:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp2F2=
_region_malloc(r2,sizeof(*_tmp2F2));_tmp2F2->hd=(void*)f((void*)x->hd,env);
_tmp2F2->tl=0;_tmp2F2;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp2F3=_region_malloc(r2,sizeof(*
_tmp2F3));_tmp2F3->hd=(void*)f((void*)x->hd,env);_tmp2F3->tl=0;_tmp2F3;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple6*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=0;
_tmp2F4->f2=e;_tmp2F4;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp2F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2F6;_LL1A2: if(
_tmp2F5 <= (void*)4)goto _LL1A4;if(*((int*)_tmp2F5)!= 4)goto _LL1A4;_tmp2F6=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F5)->f1;_LL1A3: return _tmp2F6;_LL1A4:;
_LL1A5:({void*_tmp2F7[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F8="get_ptr_typ: not a pointer!";
_tag_dynforward(_tmp2F8,sizeof(char),_get_zero_arr_size(_tmp2F8,28));}),
_tag_dynforward(_tmp2F7,sizeof(void*),0));});_LL1A1:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp2F9=Cyc_Tcutil_compress(
t);void*_tmp2FA;void*_tmp2FB;void*_tmp2FC;void*_tmp2FD;void*_tmp2FE;void*_tmp2FF;
void*_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;_LL1A7: if(_tmp2F9 <= (void*)4)
goto _LL1B7;if(*((int*)_tmp2F9)!= 4)goto _LL1A9;_LL1A8: res=Cyc_Absyn_null_exp(0);
goto _LL1A6;_LL1A9: if(*((int*)_tmp2F9)!= 5)goto _LL1AB;_tmp2FA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2F9)->f1;_tmp2FB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f2;if((
int)_tmp2FB != 0)goto _LL1AB;_LL1AA: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp304;(_tmp304.Char_c).tag=0;(_tmp304.Char_c).f1=(
void*)_tmp2FA;(_tmp304.Char_c).f2='\000';_tmp304;}),0);goto _LL1A6;_LL1AB: if(*((
int*)_tmp2F9)!= 5)goto _LL1AD;_tmp2FC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2F9)->f1;_tmp2FD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f2;if((
int)_tmp2FD != 1)goto _LL1AD;_LL1AC: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp305;(_tmp305.Short_c).tag=1;(_tmp305.Short_c).f1=(
void*)_tmp2FC;(_tmp305.Short_c).f2=0;_tmp305;}),0);goto _LL1A6;_LL1AD: if(*((int*)
_tmp2F9)!= 12)goto _LL1AF;_LL1AE: goto _LL1B0;_LL1AF: if(*((int*)_tmp2F9)!= 13)goto
_LL1B1;_LL1B0: _tmp2FE=(void*)1;goto _LL1B2;_LL1B1: if(*((int*)_tmp2F9)!= 5)goto
_LL1B3;_tmp2FE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f1;_tmp2FF=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f2;if((int)_tmp2FF != 2)goto
_LL1B3;_LL1B2: _tmp300=_tmp2FE;goto _LL1B4;_LL1B3: if(*((int*)_tmp2F9)!= 5)goto
_LL1B5;_tmp300=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f1;_tmp301=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f2;if((int)_tmp301 != 3)goto
_LL1B5;_LL1B4: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp306;(_tmp306.Int_c).tag=2;(_tmp306.Int_c).f1=(void*)_tmp300;(_tmp306.Int_c).f2=
0;_tmp306;}),0);goto _LL1A6;_LL1B5: if(*((int*)_tmp2F9)!= 5)goto _LL1B7;_tmp302=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2F9)->f1;_tmp303=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2F9)->f2;if((int)_tmp303 != 4)goto _LL1B7;_LL1B6: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp307;(_tmp307.LongLong_c).tag=
3;(_tmp307.LongLong_c).f1=(void*)_tmp302;(_tmp307.LongLong_c).f2=(long long)0;
_tmp307;}),0);goto _LL1A6;_LL1B7: if((int)_tmp2F9 != 1)goto _LL1B9;_LL1B8: goto _LL1BA;
_LL1B9: if(_tmp2F9 <= (void*)4)goto _LL1BB;if(*((int*)_tmp2F9)!= 6)goto _LL1BB;
_LL1BA: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp308;(_tmp308.Float_c).tag=4;(_tmp308.Float_c).f1=({const char*_tmp309="0.0";
_tag_dynforward(_tmp309,sizeof(char),_get_zero_arr_size(_tmp309,4));});_tmp308;}),
0);goto _LL1A6;_LL1BB:;_LL1BC:({struct Cyc_String_pa_struct _tmp30C;_tmp30C.tag=0;
_tmp30C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp30A[1]={& _tmp30C};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30B="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp30B,sizeof(char),_get_zero_arr_size(_tmp30B,40));}),
_tag_dynforward(_tmp30A,sizeof(void*),1));}});_LL1A6:;}res->topt=({struct Cyc_Core_Opt*
_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->v=(void*)t;_tmp30D;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp30E=Cyc_Toc_typ_to_c(
elt_type);void*_tmp30F=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp310=Cyc_Absyn_cstar_typ(
_tmp30E,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp311=({struct Cyc_Core_Opt*_tmp34F=
_cycalloc(sizeof(*_tmp34F));_tmp34F->v=(void*)_tmp310;_tmp34F;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp312=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_Absyn_Exp*
_tmp314;struct Cyc_Absyn_Exp*_tmp315;_LL1BE: if(*((int*)_tmp312)!= 22)goto _LL1C0;
_tmp313=((struct Cyc_Absyn_Deref_e_struct*)_tmp312)->f1;_LL1BF: if(!is_fat){
_tmp313=Cyc_Toc_cast_it(fat_ptr_type,_tmp313);_tmp313->topt=({struct Cyc_Core_Opt*
_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316->v=(void*)fat_ptr_type;_tmp316;});}
Cyc_Toc_exp_to_c(nv,_tmp313);xinit=_tmp313;goto _LL1BD;_LL1C0: if(*((int*)_tmp312)
!= 25)goto _LL1C2;_tmp314=((struct Cyc_Absyn_Subscript_e_struct*)_tmp312)->f1;
_tmp315=((struct Cyc_Absyn_Subscript_e_struct*)_tmp312)->f2;_LL1C1: if(!is_fat){
_tmp314=Cyc_Toc_cast_it(fat_ptr_type,_tmp314);_tmp314->topt=({struct Cyc_Core_Opt*
_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->v=(void*)fat_ptr_type;_tmp317;});}
Cyc_Toc_exp_to_c(nv,_tmp314);Cyc_Toc_exp_to_c(nv,_tmp315);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp318[3];_tmp318[2]=_tmp315;_tmp318[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp318[0]=_tmp314;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp318,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp319[3];_tmp319[2]=_tmp315;_tmp319[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp319[0]=_tmp314;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp319,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1BD;_LL1C2:;_LL1C3:({void*_tmp31A[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp31B="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,53));}),
_tag_dynforward(_tmp31A,sizeof(void*),0));});_LL1BD:;}{struct _tuple1*_tmp31C=Cyc_Toc_temp_var();
struct _RegionHandle _tmp31D=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp31D;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp31E=Cyc_Toc_add_varmap(rgn2,nv,_tmp31C,
Cyc_Absyn_var_exp(_tmp31C,0));struct Cyc_Absyn_Vardecl*_tmp31F=({struct Cyc_Absyn_Vardecl*
_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->sc=(void*)((void*)2);_tmp34E->name=
_tmp31C;_tmp34E->tq=Cyc_Toc_mt_tq;_tmp34E->type=(void*)_tmp30F;_tmp34E->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp34E->rgn=0;_tmp34E->attributes=0;_tmp34E->escapes=
0;_tmp34E;});struct Cyc_Absyn_Local_b_struct*_tmp320=({struct Cyc_Absyn_Local_b_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Absyn_Local_b_struct
_tmp34D;_tmp34D.tag=3;_tmp34D.f1=_tmp31F;_tmp34D;});_tmp34C;});struct Cyc_Absyn_Exp*
_tmp321=Cyc_Absyn_varb_exp(_tmp31C,(void*)_tmp320,0);_tmp321->topt=({struct Cyc_Core_Opt*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->v=(void*)fat_ptr_type;_tmp322;});{
struct Cyc_Absyn_Exp*_tmp323=Cyc_Absyn_deref_exp(_tmp321,0);_tmp323->topt=({
struct Cyc_Core_Opt*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->v=(void*)
elt_type;_tmp324;});Cyc_Toc_exp_to_c(_tmp31E,_tmp323);{struct _tuple1*_tmp325=Cyc_Toc_temp_var();
_tmp31E=Cyc_Toc_add_varmap(rgn2,_tmp31E,_tmp325,Cyc_Absyn_var_exp(_tmp325,0));{
struct Cyc_Absyn_Vardecl*_tmp326=({struct Cyc_Absyn_Vardecl*_tmp34B=_cycalloc(
sizeof(*_tmp34B));_tmp34B->sc=(void*)((void*)2);_tmp34B->name=_tmp325;_tmp34B->tq=
Cyc_Toc_mt_tq;_tmp34B->type=(void*)_tmp30E;_tmp34B->initializer=(struct Cyc_Absyn_Exp*)
_tmp323;_tmp34B->rgn=0;_tmp34B->attributes=0;_tmp34B->escapes=0;_tmp34B;});
struct Cyc_Absyn_Local_b_struct*_tmp327=({struct Cyc_Absyn_Local_b_struct*_tmp349=
_cycalloc(sizeof(*_tmp349));_tmp349[0]=({struct Cyc_Absyn_Local_b_struct _tmp34A;
_tmp34A.tag=3;_tmp34A.f1=_tmp326;_tmp34A;});_tmp349;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp328=Cyc_Absyn_varb_exp(_tmp325,(
void*)_tmp327,0);_tmp328->topt=_tmp323->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp328,e2,0);z_init->topt=_tmp328->topt;}Cyc_Toc_exp_to_c(_tmp31E,
z_init);{struct _tuple1*_tmp329=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp32A=({struct Cyc_Absyn_Vardecl*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->sc=(
void*)((void*)2);_tmp348->name=_tmp329;_tmp348->tq=Cyc_Toc_mt_tq;_tmp348->type=(
void*)_tmp30E;_tmp348->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp348->rgn=0;
_tmp348->attributes=0;_tmp348->escapes=0;_tmp348;});struct Cyc_Absyn_Local_b_struct*
_tmp32B=({struct Cyc_Absyn_Local_b_struct*_tmp346=_cycalloc(sizeof(*_tmp346));
_tmp346[0]=({struct Cyc_Absyn_Local_b_struct _tmp347;_tmp347.tag=3;_tmp347.f1=
_tmp32A;_tmp347;});_tmp346;});_tmp31E=Cyc_Toc_add_varmap(rgn2,_tmp31E,_tmp329,
Cyc_Absyn_var_exp(_tmp329,0));{struct Cyc_Absyn_Exp*_tmp32C=Cyc_Absyn_varb_exp(
_tmp325,(void*)_tmp327,0);_tmp32C->topt=_tmp323->topt;{struct Cyc_Absyn_Exp*
_tmp32D=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp32E=Cyc_Absyn_prim2_exp((
void*)5,_tmp32C,_tmp32D,0);_tmp32E->topt=({struct Cyc_Core_Opt*_tmp32F=_cycalloc(
sizeof(*_tmp32F));_tmp32F->v=(void*)Cyc_Absyn_sint_typ;_tmp32F;});Cyc_Toc_exp_to_c(
_tmp31E,_tmp32E);{struct Cyc_Absyn_Exp*_tmp330=Cyc_Absyn_varb_exp(_tmp329,(void*)
_tmp32B,0);_tmp330->topt=_tmp323->topt;{struct Cyc_Absyn_Exp*_tmp331=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp332=Cyc_Absyn_prim2_exp((void*)6,_tmp330,
_tmp331,0);_tmp332->topt=({struct Cyc_Core_Opt*_tmp333=_cycalloc(sizeof(*_tmp333));
_tmp333->v=(void*)Cyc_Absyn_sint_typ;_tmp333;});Cyc_Toc_exp_to_c(_tmp31E,_tmp332);{
struct Cyc_List_List*_tmp334=({struct Cyc_Absyn_Exp*_tmp345[2];_tmp345[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp345[0]=Cyc_Absyn_varb_exp(_tmp31C,(void*)
_tmp320,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp345,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp335=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=
Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,
_tmp334,0),_tmp335,0);else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp334,0),_tmp335,0);}{struct Cyc_Absyn_Exp*
_tmp336=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp32E,_tmp332,0),0);struct
Cyc_Absyn_Stmt*_tmp337=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,
0,0),0);struct Cyc_Absyn_Exp*_tmp338=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp31C,(void*)_tmp320,0),Cyc_Toc_curr_sp,0);_tmp338=Cyc_Toc_cast_it(_tmp310,
_tmp338);{struct Cyc_Absyn_Exp*_tmp339=Cyc_Absyn_deref_exp(_tmp338,0);struct Cyc_Absyn_Exp*
_tmp33A=Cyc_Absyn_assign_exp(_tmp339,Cyc_Absyn_var_exp(_tmp329,0),0);struct Cyc_Absyn_Stmt*
_tmp33B=Cyc_Absyn_exp_stmt(_tmp33A,0);_tmp33B=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp336,_tmp337,Cyc_Absyn_skip_stmt(0),0),_tmp33B,0);_tmp33B=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp33E;_tmp33E.tag=0;_tmp33E.f1=_tmp32A;
_tmp33E;});_tmp33D;}));_tmp33C->loc=0;_tmp33C;}),_tmp33B,0);_tmp33B=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340[
0]=({struct Cyc_Absyn_Var_d_struct _tmp341;_tmp341.tag=0;_tmp341.f1=_tmp326;
_tmp341;});_tmp340;}));_tmp33F->loc=0;_tmp33F;}),_tmp33B,0);_tmp33B=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343[
0]=({struct Cyc_Absyn_Var_d_struct _tmp344;_tmp344.tag=0;_tmp344.f1=_tmp31F;
_tmp344;});_tmp343;}));_tmp342->loc=0;_tmp342;}),_tmp33B,0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(_tmp33B));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple9{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp350=(void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp353;_tmp353.tag=0;_tmp353.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp351[1]={& _tmp353};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp352="exp_to_c: no type for %s";_tag_dynforward(_tmp352,sizeof(char),
_get_zero_arr_size(_tmp352,25));}),_tag_dynforward(_tmp351,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp354=
_tmp350;union Cyc_Absyn_Cnst_union _tmp355;struct _tuple1*_tmp356;void*_tmp357;
struct _tuple1*_tmp358;void*_tmp359;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Exp*
_tmp35B;void*_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Core_Opt*_tmp35E;
struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Exp*
_tmp361;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*
_tmp364;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Exp*
_tmp367;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_List_List*
_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct Cyc_List_List*_tmp36C;struct Cyc_Absyn_VarargCallInfo*
_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_List_List*_tmp36F;struct Cyc_Absyn_VarargCallInfo*
_tmp370;struct Cyc_Absyn_VarargCallInfo _tmp371;int _tmp372;struct Cyc_List_List*
_tmp373;struct Cyc_Absyn_VarargInfo*_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*
_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_List_List*_tmp378;void*_tmp379;
void**_tmp37A;struct Cyc_Absyn_Exp*_tmp37B;int _tmp37C;void*_tmp37D;struct Cyc_Absyn_Exp*
_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_Absyn_Exp*
_tmp381;void*_tmp382;void*_tmp383;void*_tmp384;struct _dynforward_ptr*_tmp385;
void*_tmp386;void*_tmp387;unsigned int _tmp388;struct Cyc_Absyn_Exp*_tmp389;struct
Cyc_Absyn_Exp*_tmp38A;struct _dynforward_ptr*_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;
struct _dynforward_ptr*_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct Cyc_Absyn_Exp*
_tmp38F;struct Cyc_List_List*_tmp390;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Vardecl*
_tmp392;struct Cyc_Absyn_Exp*_tmp393;struct Cyc_Absyn_Exp*_tmp394;int _tmp395;
struct _tuple1*_tmp396;struct Cyc_List_List*_tmp397;struct Cyc_List_List*_tmp398;
struct Cyc_Absyn_Aggrdecl*_tmp399;void*_tmp39A;struct Cyc_List_List*_tmp39B;struct
Cyc_List_List*_tmp39C;struct Cyc_Absyn_Tuniondecl*_tmp39D;struct Cyc_Absyn_Tunionfield*
_tmp39E;struct Cyc_List_List*_tmp39F;struct Cyc_Absyn_Tuniondecl*_tmp3A0;struct Cyc_Absyn_Tunionfield*
_tmp3A1;struct Cyc_Absyn_MallocInfo _tmp3A2;int _tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;
void**_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;int _tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;
struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Stmt*_tmp3AA;_LL1C5: if(*((int*)
_tmp354)!= 0)goto _LL1C7;_tmp355=((struct Cyc_Absyn_Const_e_struct*)_tmp354)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp354)->f1).Null_c).tag != 6)goto _LL1C7;
_LL1C6: {struct Cyc_Absyn_Exp*_tmp3AB=Cyc_Absyn_uint_exp(0,0);int forward_only=0;
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp3AB,
_tmp3AB,forward_only))->r));else{if(forward_only)(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp3AC[3];_tmp3AC[2]=_tmp3AB;
_tmp3AC[1]=_tmp3AB;_tmp3AC[0]=_tmp3AB;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3AC,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3AD[3];_tmp3AD[2]=_tmp3AB;_tmp3AD[1]=_tmp3AB;_tmp3AD[0]=
_tmp3AB;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3AD,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1C4;}_LL1C7: if(*((int*)_tmp354)!= 0)
goto _LL1C9;_LL1C8: goto _LL1C4;_LL1C9: if(*((int*)_tmp354)!= 1)goto _LL1CB;_tmp356=((
struct Cyc_Absyn_Var_e_struct*)_tmp354)->f1;_tmp357=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp354)->f2;_LL1CA:{struct _handler_cons _tmp3AE;_push_handler(& _tmp3AE);{int
_tmp3B0=0;if(setjmp(_tmp3AE.handler))_tmp3B0=1;if(!_tmp3B0){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp356))->r));;_pop_handler();}else{void*
_tmp3AF=(void*)_exn_thrown;void*_tmp3B2=_tmp3AF;_LL21C: if(_tmp3B2 != Cyc_Dict_Absent)
goto _LL21E;_LL21D:({struct Cyc_String_pa_struct _tmp3B5;_tmp3B5.tag=0;_tmp3B5.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp356));{
void*_tmp3B3[1]={& _tmp3B5};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3B4="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp3B4,sizeof(char),_get_zero_arr_size(_tmp3B4,32));}),
_tag_dynforward(_tmp3B3,sizeof(void*),1));}});_LL21E:;_LL21F:(void)_throw(
_tmp3B2);_LL21B:;}}}goto _LL1C4;_LL1CB: if(*((int*)_tmp354)!= 2)goto _LL1CD;_tmp358=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp354)->f1;_LL1CC:({void*_tmp3B6[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3B7="unknownid";_tag_dynforward(_tmp3B7,sizeof(char),
_get_zero_arr_size(_tmp3B7,10));}),_tag_dynforward(_tmp3B6,sizeof(void*),0));});
_LL1CD: if(*((int*)_tmp354)!= 3)goto _LL1CF;_tmp359=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp354)->f1;_tmp35A=((struct Cyc_Absyn_Primop_e_struct*)_tmp354)->f2;_LL1CE: {
struct Cyc_List_List*_tmp3B8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp35A);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp35A);{void*_tmp3B9=_tmp359;_LL221:
if((int)_tmp3B9 != 19)goto _LL223;_LL222: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp35A))->hd;{void*_tmp3BA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_PtrInfo _tmp3BD;void*_tmp3BE;
struct Cyc_Absyn_PtrAtts _tmp3BF;struct Cyc_Absyn_Conref*_tmp3C0;struct Cyc_Absyn_Conref*
_tmp3C1;struct Cyc_Absyn_Conref*_tmp3C2;_LL236: if(_tmp3BA <= (void*)4)goto _LL23A;
if(*((int*)_tmp3BA)!= 7)goto _LL238;_tmp3BB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3BA)->f1;_tmp3BC=_tmp3BB.num_elts;_LL237:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp3BC))->r));goto _LL235;_LL238: if(*((int*)
_tmp3BA)!= 4)goto _LL23A;_tmp3BD=((struct Cyc_Absyn_PointerType_struct*)_tmp3BA)->f1;
_tmp3BE=(void*)_tmp3BD.elt_typ;_tmp3BF=_tmp3BD.ptr_atts;_tmp3C0=_tmp3BF.nullable;
_tmp3C1=_tmp3BF.bounds;_tmp3C2=_tmp3BF.zero_term;_LL239:{void*_tmp3C3=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3C1);struct Cyc_Absyn_Exp*_tmp3C4;_LL23D: if((int)_tmp3C3
!= 0)goto _LL23F;_LL23E:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp3C5[2];_tmp3C5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3BE),0);_tmp3C5[0]=(struct Cyc_Absyn_Exp*)_tmp35A->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3C5,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL23C;_LL23F: if((int)_tmp3C3 != 1)goto _LL241;_LL240:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp3C6[2];_tmp3C6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3BE),0);
_tmp3C6[0]=(struct Cyc_Absyn_Exp*)_tmp35A->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3C6,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL23C;_LL241: if(_tmp3C3 <= (void*)2)goto _LL243;if(*((int*)_tmp3C3)
!= 0)goto _LL243;_tmp3C4=((struct Cyc_Absyn_Upper_b_struct*)_tmp3C3)->f1;_LL242:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3C2))(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp3C7[2];_tmp3C7[1]=_tmp3C4;_tmp3C7[0]=(struct Cyc_Absyn_Exp*)_tmp35A->hd;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3C7,sizeof(struct Cyc_Absyn_Exp*),2));})));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp3C0))(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp3C4,Cyc_Absyn_uint_exp(0,0)));else{(void*)(e->r=(void*)((void*)_tmp3C4->r));
goto _LL23C;}}goto _LL23C;_LL243: if(_tmp3C3 <= (void*)2)goto _LL23C;if(*((int*)
_tmp3C3)!= 1)goto _LL23C;_LL244:({void*_tmp3C8[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3C9="toc: size of AbsUpper_b";
_tag_dynforward(_tmp3C9,sizeof(char),_get_zero_arr_size(_tmp3C9,24));}),
_tag_dynforward(_tmp3C8,sizeof(void*),0));});_LL23C:;}goto _LL235;_LL23A:;_LL23B:({
struct Cyc_String_pa_struct _tmp3CD;_tmp3CD.tag=0;_tmp3CD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp3CC;
_tmp3CC.tag=0;_tmp3CC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp3CA[2]={&
_tmp3CC,& _tmp3CD};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3CB="size primop applied to non-array %s (%s)";_tag_dynforward(
_tmp3CB,sizeof(char),_get_zero_arr_size(_tmp3CB,41));}),_tag_dynforward(_tmp3CA,
sizeof(void*),2));}}});_LL235:;}goto _LL220;}_LL223: if((int)_tmp3B9 != 0)goto
_LL225;_LL224:{void*_tmp3CE=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)
_check_null(_tmp3B8))->hd);struct Cyc_Absyn_PtrInfo _tmp3CF;void*_tmp3D0;struct Cyc_Absyn_PtrAtts
_tmp3D1;struct Cyc_Absyn_Conref*_tmp3D2;struct Cyc_Absyn_Conref*_tmp3D3;_LL246: if(
_tmp3CE <= (void*)4)goto _LL248;if(*((int*)_tmp3CE)!= 4)goto _LL248;_tmp3CF=((
struct Cyc_Absyn_PointerType_struct*)_tmp3CE)->f1;_tmp3D0=(void*)_tmp3CF.elt_typ;
_tmp3D1=_tmp3CF.ptr_atts;_tmp3D2=_tmp3D1.bounds;_tmp3D3=_tmp3D1.zero_term;_LL247:{
void*_tmp3D4=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3D2);struct Cyc_Absyn_Exp*
_tmp3D5;_LL24B: if((int)_tmp3D4 != 0)goto _LL24D;_LL24C: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35A))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35A->tl))->hd;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,({struct
Cyc_Absyn_Exp*_tmp3D6[3];_tmp3D6[2]=e2;_tmp3D6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3D0),0);_tmp3D6[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3D6,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL24A;}_LL24D:
if((int)_tmp3D4 != 1)goto _LL24F;_LL24E: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp35A))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp35A->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3D7[3];_tmp3D7[2]=e2;
_tmp3D7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3D0),0);_tmp3D7[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3D7,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL24A;}_LL24F: if(_tmp3D4 <= (
void*)2)goto _LL251;if(*((int*)_tmp3D4)!= 0)goto _LL251;_tmp3D5=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3D4)->f1;_LL250: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3D3)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp35A))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp35A->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp3D8[3];_tmp3D8[2]=e2;_tmp3D8[
1]=_tmp3D5;_tmp3D8[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),3));})));}goto _LL24A;_LL251:
if(_tmp3D4 <= (void*)2)goto _LL24A;if(*((int*)_tmp3D4)!= 1)goto _LL24A;_LL252:({
void*_tmp3D9[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3DA="toc: plus on AbsUpper_b";_tag_dynforward(
_tmp3DA,sizeof(char),_get_zero_arr_size(_tmp3DA,24));}),_tag_dynforward(_tmp3D9,
sizeof(void*),0));});_LL24A:;}goto _LL245;_LL248:;_LL249: goto _LL245;_LL245:;}goto
_LL220;_LL225: if((int)_tmp3B9 != 2)goto _LL227;_LL226: {void*elt_typ=(void*)0;int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp3B8))->hd,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35A))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35A->tl))->hd;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp3B8->tl))->hd,& forward_only)){(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(
Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp3DB[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3DC="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp3DC,sizeof(char),_get_zero_arr_size(_tmp3DC,50));}),
_tag_dynforward(_tmp3DB,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3DD[3];_tmp3DD[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp3DD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp3DD[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL220;}
_LL227: if((int)_tmp3B9 != 5)goto _LL229;_LL228: goto _LL22A;_LL229: if((int)_tmp3B9 != 
6)goto _LL22B;_LL22A: goto _LL22C;_LL22B: if((int)_tmp3B9 != 7)goto _LL22D;_LL22C: goto
_LL22E;_LL22D: if((int)_tmp3B9 != 9)goto _LL22F;_LL22E: goto _LL230;_LL22F: if((int)
_tmp3B9 != 8)goto _LL231;_LL230: goto _LL232;_LL231: if((int)_tmp3B9 != 10)goto _LL233;
_LL232: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp35A))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp35A->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3B8))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp3B8->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL220;}_LL233:;_LL234: goto _LL220;_LL220:;}
goto _LL1C4;}_LL1CF: if(*((int*)_tmp354)!= 5)goto _LL1D1;_tmp35B=((struct Cyc_Absyn_Increment_e_struct*)
_tmp354)->f1;_tmp35C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp354)->f2;
_LL1D0: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp35B->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp3E9="increment";_tag_dynforward(
_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,10));});if(_tmp35C == (void*)2
 || _tmp35C == (void*)3)incr_str=({const char*_tmp3DE="decrement";_tag_dynforward(
_tmp3DE,sizeof(char),_get_zero_arr_size(_tmp3DE,10));});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp35B,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp3E1;_tmp3E1.tag=0;_tmp3E1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp3DF[1]={& _tmp3E1};Cyc_Tcutil_terr(e->loc,({const char*_tmp3E0="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp3E0,sizeof(char),_get_zero_arr_size(_tmp3E0,74));}),
_tag_dynforward(_tmp3DF,sizeof(void*),1));}});({void*_tmp3E2[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3E3="in-place inc on zero-term";_tag_dynforward(_tmp3E3,sizeof(char),
_get_zero_arr_size(_tmp3E3,26));}),_tag_dynforward(_tmp3E2,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp35B);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp35C == (void*)3  || _tmp35C == (void*)2)({void*_tmp3E4[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp3E5="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,43));}),
_tag_dynforward(_tmp3E4,sizeof(void*),0));});fn_e=_tmp35C == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp35C == (void*)1  || _tmp35C
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp35C == (void*)2  || _tmp35C == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp3E6[3];_tmp3E6[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp3E6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp3E6[0]=
Cyc_Absyn_address_exp(_tmp35B,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp3E6,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp3E7=_tmp35C == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e: Cyc_Toc__zero_arr_inplace_plus_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp3E7,({struct Cyc_Absyn_Exp*_tmp3E8[2];
_tmp3E8[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp3E8[0]=_tmp35B;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3E8,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp35B)){Cyc_Toc_lvalue_assign(
_tmp35B,0,Cyc_Toc_incr_lvalue,_tmp35C);(void*)(e->r=(void*)((void*)_tmp35B->r));}}}
goto _LL1C4;}}_LL1D1: if(*((int*)_tmp354)!= 4)goto _LL1D3;_tmp35D=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp354)->f1;_tmp35E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp354)->f2;_tmp35F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp354)->f3;_LL1D2: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp35D,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp35D,_tmp35E,_tmp35F,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp35D);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp35D->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp35F->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp35D);
Cyc_Toc_exp_to_c(nv,_tmp35F);{int done=0;if(_tmp35E != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp3EA=(void*)_tmp35E->v;_LL254:
if((int)_tmp3EA != 0)goto _LL256;_LL255: change=_tmp35F;goto _LL253;_LL256: if((int)
_tmp3EA != 2)goto _LL258;_LL257: if(forward_only)({void*_tmp3EB[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3EC="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp3EC,sizeof(char),_get_zero_arr_size(_tmp3EC,46));}),
_tag_dynforward(_tmp3EB,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp35F,0);goto _LL253;_LL258:;_LL259:({void*_tmp3ED[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3EE="bad t ? pointer arithmetic";_tag_dynforward(_tmp3EE,sizeof(char),
_get_zero_arr_size(_tmp3EE,27));}),_tag_dynforward(_tmp3ED,sizeof(void*),0));});
_LL253:;}done=1;{struct Cyc_Absyn_Exp*_tmp3EF=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp3EF,({struct Cyc_Absyn_Exp*_tmp3F0[3];_tmp3F0[2]=change;_tmp3F0[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp3F0[0]=Cyc_Absyn_address_exp(_tmp35D,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3F0,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp3F1=(void*)_tmp35E->v;_LL25B: if((int)_tmp3F1 != 0)
goto _LL25D;_LL25C: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp3F2[2];_tmp3F2[1]=_tmp35F;_tmp3F2[0]=_tmp35D;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3F2,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL25A;_LL25D:;_LL25E:({void*_tmp3F3[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3F4="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp3F4,sizeof(
char),_get_zero_arr_size(_tmp3F4,39));}),_tag_dynforward(_tmp3F3,sizeof(void*),0));});
_LL25A:;}}}if(!done){if(e1_poly)(void*)(_tmp35F->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp35F->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp35D)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp35D,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp3F5=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp3F5[0]=({struct _tuple8 _tmp3F6;_tmp3F6.f1=_tmp35E;_tmp3F6.f2=
_tmp35F;_tmp3F6;});_tmp3F5;}));(void*)(e->r=(void*)((void*)_tmp35D->r));}}goto
_LL1C4;}}}_LL1D3: if(*((int*)_tmp354)!= 6)goto _LL1D5;_tmp360=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp354)->f1;_tmp361=((struct Cyc_Absyn_Conditional_e_struct*)_tmp354)->f2;
_tmp362=((struct Cyc_Absyn_Conditional_e_struct*)_tmp354)->f3;_LL1D4: Cyc_Toc_exp_to_c(
nv,_tmp360);Cyc_Toc_exp_to_c(nv,_tmp361);Cyc_Toc_exp_to_c(nv,_tmp362);goto _LL1C4;
_LL1D5: if(*((int*)_tmp354)!= 7)goto _LL1D7;_tmp363=((struct Cyc_Absyn_And_e_struct*)
_tmp354)->f1;_tmp364=((struct Cyc_Absyn_And_e_struct*)_tmp354)->f2;_LL1D6: Cyc_Toc_exp_to_c(
nv,_tmp363);Cyc_Toc_exp_to_c(nv,_tmp364);goto _LL1C4;_LL1D7: if(*((int*)_tmp354)!= 
8)goto _LL1D9;_tmp365=((struct Cyc_Absyn_Or_e_struct*)_tmp354)->f1;_tmp366=((
struct Cyc_Absyn_Or_e_struct*)_tmp354)->f2;_LL1D8: Cyc_Toc_exp_to_c(nv,_tmp365);
Cyc_Toc_exp_to_c(nv,_tmp366);goto _LL1C4;_LL1D9: if(*((int*)_tmp354)!= 9)goto
_LL1DB;_tmp367=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp354)->f1;_tmp368=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp354)->f2;_LL1DA: Cyc_Toc_exp_to_c(nv,_tmp367);Cyc_Toc_exp_to_c(
nv,_tmp368);goto _LL1C4;_LL1DB: if(*((int*)_tmp354)!= 10)goto _LL1DD;_tmp369=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp354)->f1;_tmp36A=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp354)->f2;_LL1DC: _tmp36B=_tmp369;_tmp36C=_tmp36A;goto _LL1DE;_LL1DD: if(*((int*)
_tmp354)!= 11)goto _LL1DF;_tmp36B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp354)->f1;
_tmp36C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp354)->f2;_tmp36D=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp354)->f3;if(_tmp36D != 0)goto _LL1DF;_LL1DE: Cyc_Toc_exp_to_c(nv,_tmp36B);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp36C);goto _LL1C4;
_LL1DF: if(*((int*)_tmp354)!= 11)goto _LL1E1;_tmp36E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp354)->f1;_tmp36F=((struct Cyc_Absyn_FnCall_e_struct*)_tmp354)->f2;_tmp370=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp354)->f3;if(_tmp370 == 0)goto _LL1E1;_tmp371=*
_tmp370;_tmp372=_tmp371.num_varargs;_tmp373=_tmp371.injectors;_tmp374=_tmp371.vai;
_LL1E0:{struct _RegionHandle _tmp3F7=_new_region("r");struct _RegionHandle*r=&
_tmp3F7;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp372,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp374->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp36F);int num_normargs=num_args - _tmp372;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp36F=_tmp36F->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp36F))->hd);
new_args=({struct Cyc_List_List*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->hd=(
struct Cyc_Absyn_Exp*)_tmp36F->hd;_tmp3F8->tl=new_args;_tmp3F8;});}}new_args=({
struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp3FA[3];_tmp3FA[2]=
num_varargs_exp;_tmp3FA[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp3FA[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3FA,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp3F9->tl=
new_args;_tmp3F9;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp36E);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp36E,new_args,0),0);if(_tmp374->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp3FB=Cyc_Tcutil_compress((void*)_tmp374->type);
struct Cyc_Absyn_TunionInfo _tmp3FC;union Cyc_Absyn_TunionInfoU_union _tmp3FD;struct
Cyc_Absyn_Tuniondecl**_tmp3FE;struct Cyc_Absyn_Tuniondecl*_tmp3FF;_LL260: if(
_tmp3FB <= (void*)4)goto _LL262;if(*((int*)_tmp3FB)!= 2)goto _LL262;_tmp3FC=((
struct Cyc_Absyn_TunionType_struct*)_tmp3FB)->f1;_tmp3FD=_tmp3FC.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3FB)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL262;_tmp3FE=(_tmp3FD.KnownTunion).f1;_tmp3FF=*_tmp3FE;_LL261: tud=
_tmp3FF;goto _LL25F;_LL262:;_LL263:({void*_tmp400[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp401="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp401,
sizeof(char),_get_zero_arr_size(_tmp401,42));}),_tag_dynforward(_tmp400,sizeof(
void*),0));});_LL25F:;}{struct _dynforward_ptr vs=({unsigned int _tmp409=(
unsigned int)_tmp372;struct _tuple1**_tmp40A=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp409));struct _dynforward_ptr _tmp40C=
_tag_dynforward(_tmp40A,sizeof(struct _tuple1*),_tmp409);{unsigned int _tmp40B=
_tmp409;unsigned int i;for(i=0;i < _tmp40B;i ++){_tmp40A[i]=Cyc_Toc_temp_var();}}
_tmp40C;});struct Cyc_List_List*_tmp402=0;{int i=_tmp372 - 1;for(0;i >= 0;-- i){
_tmp402=({struct Cyc_List_List*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp403->tl=
_tmp402;_tmp403;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp402,0),s,0);{int i=0;for(0;_tmp36F != 0;(((
_tmp36F=_tmp36F->tl,_tmp373=_tmp373->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp36F->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp405;struct _tuple1*
_tmp406;struct Cyc_List_List*_tmp407;struct Cyc_Absyn_Tunionfield*_tmp404=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp373))->hd;_tmp405=*
_tmp404;_tmp406=_tmp405.name;_tmp407=_tmp405.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp407))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp406,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp406,({const char*_tmp408="_struct";
_tag_dynforward(_tmp408,sizeof(char),_get_zero_arr_size(_tmp408,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp36F != 0;(_tmp36F=_tmp36F->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp36F->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp36F->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1C4;_LL1E1: if(*((int*)_tmp354)!= 12)goto _LL1E3;
_tmp375=((struct Cyc_Absyn_Throw_e_struct*)_tmp354)->f1;_LL1E2: Cyc_Toc_exp_to_c(
nv,_tmp375);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp375),0))->r));goto _LL1C4;_LL1E3: if(*((int*)
_tmp354)!= 13)goto _LL1E5;_tmp376=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp354)->f1;_LL1E4: Cyc_Toc_exp_to_c(nv,_tmp376);goto _LL1C4;_LL1E5: if(*((int*)
_tmp354)!= 14)goto _LL1E7;_tmp377=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp354)->f1;
_tmp378=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp354)->f2;_LL1E6: Cyc_Toc_exp_to_c(
nv,_tmp377);for(0;_tmp378 != 0;_tmp378=_tmp378->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp378->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp40D=Cyc_Tcutil_compress((void*)_tmp378->hd);_LL265: if(_tmp40D
<= (void*)4)goto _LL269;if(*((int*)_tmp40D)!= 1)goto _LL267;_LL266: goto _LL268;
_LL267: if(*((int*)_tmp40D)!= 2)goto _LL269;_LL268: continue;_LL269:;_LL26A:(void*)(
e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp377,0))->r));goto _LL264;_LL264:;}
break;}}goto _LL1C4;_LL1E7: if(*((int*)_tmp354)!= 15)goto _LL1E9;_tmp379=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp354)->f1;_tmp37A=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp354)->f1);_tmp37B=((struct Cyc_Absyn_Cast_e_struct*)_tmp354)->f2;_tmp37C=((
struct Cyc_Absyn_Cast_e_struct*)_tmp354)->f3;_tmp37D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp354)->f4;_LL1E8: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp37B->topt))->v;void*new_typ=*_tmp37A;*_tmp37A=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(
nv,_tmp37B);{struct _tuple0 _tmp40F=({struct _tuple0 _tmp40E;_tmp40E.f1=Cyc_Tcutil_compress(
old_t2);_tmp40E.f2=Cyc_Tcutil_compress(new_typ);_tmp40E;});void*_tmp410;struct
Cyc_Absyn_PtrInfo _tmp411;void*_tmp412;struct Cyc_Absyn_PtrInfo _tmp413;void*
_tmp414;struct Cyc_Absyn_PtrInfo _tmp415;void*_tmp416;_LL26C: _tmp410=_tmp40F.f1;
if(_tmp410 <= (void*)4)goto _LL26E;if(*((int*)_tmp410)!= 4)goto _LL26E;_tmp411=((
struct Cyc_Absyn_PointerType_struct*)_tmp410)->f1;_tmp412=_tmp40F.f2;if(_tmp412 <= (
void*)4)goto _LL26E;if(*((int*)_tmp412)!= 4)goto _LL26E;_tmp413=((struct Cyc_Absyn_PointerType_struct*)
_tmp412)->f1;_LL26D: {int _tmp417=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp411.ptr_atts).nullable);int _tmp418=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp413.ptr_atts).nullable);void*_tmp419=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp411.ptr_atts).bounds);void*_tmp41A=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp413.ptr_atts).bounds);int _tmp41B=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp411.ptr_atts).zero_term);int
_tmp41C=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp413.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp41E=({struct _tuple0
_tmp41D;_tmp41D.f1=_tmp419;_tmp41D.f2=_tmp41A;_tmp41D;});void*_tmp41F;struct Cyc_Absyn_Exp*
_tmp420;void*_tmp421;struct Cyc_Absyn_Exp*_tmp422;void*_tmp423;struct Cyc_Absyn_Exp*
_tmp424;void*_tmp425;void*_tmp426;struct Cyc_Absyn_Exp*_tmp427;void*_tmp428;void*
_tmp429;void*_tmp42A;struct Cyc_Absyn_Exp*_tmp42B;void*_tmp42C;void*_tmp42D;
struct Cyc_Absyn_Exp*_tmp42E;void*_tmp42F;void*_tmp430;void*_tmp431;void*_tmp432;
void*_tmp433;void*_tmp434;void*_tmp435;void*_tmp436;void*_tmp437;void*_tmp438;
void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;_LL273: _tmp41F=_tmp41E.f1;
if(_tmp41F <= (void*)2)goto _LL275;if(*((int*)_tmp41F)!= 0)goto _LL275;_tmp420=((
struct Cyc_Absyn_Upper_b_struct*)_tmp41F)->f1;_tmp421=_tmp41E.f2;if(_tmp421 <= (
void*)2)goto _LL275;if(*((int*)_tmp421)!= 0)goto _LL275;_tmp422=((struct Cyc_Absyn_Upper_b_struct*)
_tmp421)->f1;_LL274: if(_tmp417  && !_tmp418){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp43D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp43E="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp43E,sizeof(char),_get_zero_arr_size(_tmp43E,44));}),_tag_dynforward(_tmp43D,
sizeof(void*),0));});if(_tmp37D != (void*)2)({struct Cyc_String_pa_struct _tmp441;
_tmp441.tag=0;_tmp441.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp43F[1]={& _tmp441};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp440="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp440,sizeof(char),_get_zero_arr_size(_tmp440,41));}),
_tag_dynforward(_tmp43F,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp37B);if(do_null_check){if(!_tmp37C)({void*_tmp442[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp443="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp443,sizeof(char),_get_zero_arr_size(_tmp443,58));}),
_tag_dynforward(_tmp442,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp37A,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp444=
_cycalloc(sizeof(*_tmp444));_tmp444->hd=_tmp37B;_tmp444->tl=0;_tmp444;}),0)));}}}
goto _LL272;_LL275: _tmp423=_tmp41E.f1;if(_tmp423 <= (void*)2)goto _LL277;if(*((int*)
_tmp423)!= 0)goto _LL277;_tmp424=((struct Cyc_Absyn_Upper_b_struct*)_tmp423)->f1;
_tmp425=_tmp41E.f2;if((int)_tmp425 != 0)goto _LL277;_LL276: p2_forward_only=1;
_tmp427=_tmp424;goto _LL278;_LL277: _tmp426=_tmp41E.f1;if(_tmp426 <= (void*)2)goto
_LL279;if(*((int*)_tmp426)!= 0)goto _LL279;_tmp427=((struct Cyc_Absyn_Upper_b_struct*)
_tmp426)->f1;_tmp428=_tmp41E.f2;if((int)_tmp428 != 1)goto _LL279;_LL278: if(_tmp37D
== (void*)2)({struct Cyc_String_pa_struct _tmp447;_tmp447.tag=0;_tmp447.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp445[
1]={& _tmp447};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp446="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp446,sizeof(char),_get_zero_arr_size(_tmp446,44));}),_tag_dynforward(_tmp445,
sizeof(void*),1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp41B  && !(_tmp413.elt_tq).real_const)
 && !_tmp41C)_tmp427=Cyc_Absyn_prim2_exp((void*)2,_tmp427,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp427,
_tmp37B,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp448=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp41B){struct _tuple1*
_tmp449=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp44A=Cyc_Absyn_var_exp(_tmp449,
0);struct Cyc_Absyn_Exp*_tmp44B=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp450[2];_tmp450[1]=_tmp427;_tmp450[0]=_tmp44A;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp450,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp41C  && !(_tmp413.elt_tq).real_const)_tmp44B=Cyc_Absyn_prim2_exp((
void*)2,_tmp44B,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp413.elt_typ),0);struct Cyc_Absyn_Exp*_tmp44D=Cyc_Absyn_fncall_exp(
_tmp448,({struct Cyc_Absyn_Exp*_tmp44F[3];_tmp44F[2]=_tmp44B;_tmp44F[1]=_tmp44C;
_tmp44F[0]=_tmp44A;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp44F,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp44E=Cyc_Absyn_exp_stmt(_tmp44D,0);_tmp44E=Cyc_Absyn_declare_stmt(_tmp449,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp37B,_tmp44E,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp44E));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp448,({struct Cyc_Absyn_Exp*
_tmp451[3];_tmp451[2]=_tmp427;_tmp451[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp413.elt_typ),0);_tmp451[0]=_tmp37B;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp451,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL272;_LL279: _tmp429=_tmp41E.f1;if((int)_tmp429 != 0)goto _LL27B;
_tmp42A=_tmp41E.f2;if(_tmp42A <= (void*)2)goto _LL27B;if(*((int*)_tmp42A)!= 0)goto
_LL27B;_tmp42B=((struct Cyc_Absyn_Upper_b_struct*)_tmp42A)->f1;_LL27A:
p1_forward_only=1;_tmp42E=_tmp42B;goto _LL27C;_LL27B: _tmp42C=_tmp41E.f1;if((int)
_tmp42C != 1)goto _LL27D;_tmp42D=_tmp41E.f2;if(_tmp42D <= (void*)2)goto _LL27D;if(*((
int*)_tmp42D)!= 0)goto _LL27D;_tmp42E=((struct Cyc_Absyn_Upper_b_struct*)_tmp42D)->f1;
_LL27C: if(Cyc_Toc_is_toplevel(nv))({void*_tmp452[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp453="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp453,sizeof(char),_get_zero_arr_size(_tmp453,45));}),
_tag_dynforward(_tmp452,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp454=
_tmp42E;if(_tmp41B  && !_tmp41C)_tmp454=Cyc_Absyn_add_exp(_tmp42E,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp455=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp456=Cyc_Absyn_fncall_exp(
_tmp455,({struct Cyc_Absyn_Exp*_tmp458[3];_tmp458[2]=_tmp454;_tmp458[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp411.elt_typ),0);_tmp458[0]=_tmp37B;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp458,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp418)(void*)(_tmp456->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->hd=Cyc_Absyn_copy_exp(
_tmp456);_tmp457->tl=0;_tmp457;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp37A,_tmp456));goto _LL272;}}_LL27D: _tmp42F=_tmp41E.f1;if((int)_tmp42F != 0)
goto _LL27F;_tmp430=_tmp41E.f2;if((int)_tmp430 != 0)goto _LL27F;_LL27E:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL27F: _tmp431=_tmp41E.f1;if((
int)_tmp431 != 0)goto _LL281;_tmp432=_tmp41E.f2;if((int)_tmp432 != 1)goto _LL281;
_LL280: p1_forward_only=1;goto DynCast;_LL281: _tmp433=_tmp41E.f1;if((int)_tmp433 != 
1)goto _LL283;_tmp434=_tmp41E.f2;if((int)_tmp434 != 0)goto _LL283;_LL282:
p2_forward_only=1;goto DynCast;_LL283: _tmp435=_tmp41E.f1;if((int)_tmp435 != 1)goto
_LL285;_tmp436=_tmp41E.f2;if((int)_tmp436 != 1)goto _LL285;_LL284: DynCast: if((
_tmp41B  && !_tmp41C) && !(_tmp413.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp459[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp45A="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp45A,sizeof(char),_get_zero_arr_size(_tmp45A,70));}),
_tag_dynforward(_tmp459,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp45B=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp45B,({struct Cyc_Absyn_Exp*_tmp45C[3];
_tmp45C[2]=Cyc_Absyn_uint_exp(1,0);_tmp45C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp411.elt_typ),0);_tmp45C[0]=_tmp37B;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp45C,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp45D[1];_tmp45D[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp45D,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp45E[1];_tmp45E[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp45E,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(
nv))({void*_tmp45F[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp460="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp460,sizeof(char),_get_zero_arr_size(_tmp460,43));}),
_tag_dynforward(_tmp45F,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp461[1];_tmp461[0]=_tmp37B;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp461,sizeof(struct Cyc_Absyn_Exp*),1));})));
else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp462[1];_tmp462[0]=_tmp37B;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp462,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}goto _LL272;_LL285: _tmp437=_tmp41E.f1;if(_tmp437 <= (void*)2)goto
_LL287;if(*((int*)_tmp437)!= 1)goto _LL287;_tmp438=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp437)->f1;_tmp439=_tmp41E.f2;if(_tmp439 <= (void*)2)goto _LL287;if(*((int*)
_tmp439)!= 1)goto _LL287;_tmp43A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp439)->f1;_LL286: if(_tmp417  && !_tmp418){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp463[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp464="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp464,sizeof(char),_get_zero_arr_size(_tmp464,44));}),_tag_dynforward(_tmp463,
sizeof(void*),0));});if(_tmp37D != (void*)2)({struct Cyc_String_pa_struct _tmp467;
_tmp467.tag=0;_tmp467.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp465[1]={& _tmp467};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp466="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp466,sizeof(char),_get_zero_arr_size(_tmp466,41));}),
_tag_dynforward(_tmp465,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp37B);if(do_null_check){if(!_tmp37C)({void*_tmp468[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp469="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp469,sizeof(char),_get_zero_arr_size(_tmp469,58));}),
_tag_dynforward(_tmp468,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp37A,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp46A=
_cycalloc(sizeof(*_tmp46A));_tmp46A->hd=_tmp37B;_tmp46A->tl=0;_tmp46A;}),0)));}}}
goto _LL272;_LL287: _tmp43B=_tmp41E.f1;if(_tmp43B <= (void*)2)goto _LL289;if(*((int*)
_tmp43B)!= 1)goto _LL289;_LL288:({struct Cyc_String_pa_struct _tmp46D;_tmp46D.tag=0;
_tmp46D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp37B->loc));{void*_tmp46B[1]={& _tmp46D};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp46C="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp46C,sizeof(char),_get_zero_arr_size(_tmp46C,30));}),
_tag_dynforward(_tmp46B,sizeof(void*),1));}});_LL289: _tmp43C=_tmp41E.f2;if(
_tmp43C <= (void*)2)goto _LL272;if(*((int*)_tmp43C)!= 1)goto _LL272;_LL28A: goto
_LL272;_LL272:;}goto _LL26B;}_LL26E: _tmp414=_tmp40F.f1;if(_tmp414 <= (void*)4)goto
_LL270;if(*((int*)_tmp414)!= 4)goto _LL270;_tmp415=((struct Cyc_Absyn_PointerType_struct*)
_tmp414)->f1;_tmp416=_tmp40F.f2;if(_tmp416 <= (void*)4)goto _LL270;if(*((int*)
_tmp416)!= 5)goto _LL270;_LL26F:{void*_tmp46E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp415.ptr_atts).bounds);_LL28C: if((int)_tmp46E != 0)goto _LL28E;_LL28D: goto
_LL28F;_LL28E: if((int)_tmp46E != 1)goto _LL290;_LL28F:(void*)(_tmp37B->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp37B->r,_tmp37B->loc),Cyc_Toc_curr_sp));
goto _LL28B;_LL290:;_LL291: goto _LL28B;_LL28B:;}goto _LL26B;_LL270:;_LL271: goto
_LL26B;_LL26B:;}goto _LL1C4;}_LL1E9: if(*((int*)_tmp354)!= 16)goto _LL1EB;_tmp37E=((
struct Cyc_Absyn_Address_e_struct*)_tmp354)->f1;_LL1EA:{void*_tmp46F=(void*)
_tmp37E->r;struct _tuple1*_tmp470;struct Cyc_List_List*_tmp471;struct Cyc_List_List*
_tmp472;struct Cyc_List_List*_tmp473;_LL293: if(*((int*)_tmp46F)!= 30)goto _LL295;
_tmp470=((struct Cyc_Absyn_Struct_e_struct*)_tmp46F)->f1;_tmp471=((struct Cyc_Absyn_Struct_e_struct*)
_tmp46F)->f2;_tmp472=((struct Cyc_Absyn_Struct_e_struct*)_tmp46F)->f3;_LL294: if(
Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp476;_tmp476.tag=0;
_tmp476.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp37E->loc));{void*_tmp474[1]={& _tmp476};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp475="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp475,sizeof(char),_get_zero_arr_size(_tmp475,42));}),
_tag_dynforward(_tmp474,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp37E->topt))->v,_tmp471 != 0,1,0,
_tmp472,_tmp470))->r));goto _LL292;_LL295: if(*((int*)_tmp46F)!= 26)goto _LL297;
_tmp473=((struct Cyc_Absyn_Tuple_e_struct*)_tmp46F)->f1;_LL296: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp479;_tmp479.tag=0;_tmp479.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp37E->loc));{
void*_tmp477[1]={& _tmp479};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp478="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp478,sizeof(char),_get_zero_arr_size(_tmp478,42));}),
_tag_dynforward(_tmp477,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp473))->r));goto _LL292;_LL297:;_LL298: Cyc_Toc_exp_to_c(nv,_tmp37E);if(
!Cyc_Absyn_is_lvalue(_tmp37E)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp37E,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp37E));}goto _LL292;
_LL292:;}goto _LL1C4;_LL1EB: if(*((int*)_tmp354)!= 17)goto _LL1ED;_tmp37F=((struct
Cyc_Absyn_New_e_struct*)_tmp354)->f1;_tmp380=((struct Cyc_Absyn_New_e_struct*)
_tmp354)->f2;_LL1EC: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp47C;_tmp47C.tag=0;_tmp47C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Position_string_of_segment(_tmp380->loc));{void*_tmp47A[1]={& _tmp47C};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp47B="%s: new at top-level";_tag_dynforward(_tmp47B,sizeof(char),
_get_zero_arr_size(_tmp47B,21));}),_tag_dynforward(_tmp47A,sizeof(void*),1));}});{
void*_tmp47D=(void*)_tmp380->r;struct Cyc_List_List*_tmp47E;struct Cyc_Absyn_Vardecl*
_tmp47F;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp481;int _tmp482;
struct _tuple1*_tmp483;struct Cyc_List_List*_tmp484;struct Cyc_List_List*_tmp485;
struct Cyc_List_List*_tmp486;_LL29A: if(*((int*)_tmp47D)!= 28)goto _LL29C;_tmp47E=((
struct Cyc_Absyn_Array_e_struct*)_tmp47D)->f1;_LL29B: {struct _tuple1*_tmp487=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp488=Cyc_Absyn_var_exp(_tmp487,0);struct Cyc_Absyn_Stmt*
_tmp489=Cyc_Toc_init_array(nv,_tmp488,_tmp47E,Cyc_Absyn_exp_stmt(_tmp488,0));
void*old_elt_typ;{void*_tmp48A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp48B;void*_tmp48C;struct Cyc_Absyn_Tqual _tmp48D;struct Cyc_Absyn_PtrAtts _tmp48E;
struct Cyc_Absyn_Conref*_tmp48F;_LL2A5: if(_tmp48A <= (void*)4)goto _LL2A7;if(*((int*)
_tmp48A)!= 4)goto _LL2A7;_tmp48B=((struct Cyc_Absyn_PointerType_struct*)_tmp48A)->f1;
_tmp48C=(void*)_tmp48B.elt_typ;_tmp48D=_tmp48B.elt_tq;_tmp48E=_tmp48B.ptr_atts;
_tmp48F=_tmp48E.zero_term;_LL2A6: old_elt_typ=_tmp48C;goto _LL2A4;_LL2A7:;_LL2A8:
old_elt_typ=({void*_tmp490[0]={};Cyc_Toc_toc_impos(({const char*_tmp491="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp491,sizeof(char),_get_zero_arr_size(_tmp491,52));}),
_tag_dynforward(_tmp490,sizeof(void*),0));});_LL2A4:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp492=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp493=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47E),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp37F == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp493);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp37F;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp493);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp487,
_tmp492,(struct Cyc_Absyn_Exp*)e1,_tmp489,0)));goto _LL299;}}_LL29C: if(*((int*)
_tmp47D)!= 29)goto _LL29E;_tmp47F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp47D)->f1;_tmp480=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47D)->f2;
_tmp481=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47D)->f3;_tmp482=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp47D)->f4;_LL29D: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp494=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp495;void*_tmp496;struct Cyc_Absyn_Tqual _tmp497;struct Cyc_Absyn_PtrAtts _tmp498;
struct Cyc_Absyn_Conref*_tmp499;struct Cyc_Absyn_Conref*_tmp49A;_LL2AA: if(_tmp494
<= (void*)4)goto _LL2AC;if(*((int*)_tmp494)!= 4)goto _LL2AC;_tmp495=((struct Cyc_Absyn_PointerType_struct*)
_tmp494)->f1;_tmp496=(void*)_tmp495.elt_typ;_tmp497=_tmp495.elt_tq;_tmp498=
_tmp495.ptr_atts;_tmp499=_tmp498.bounds;_tmp49A=_tmp498.zero_term;_LL2AB:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp499)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp499)== (void*)1;
goto _LL2A9;_LL2AC:;_LL2AD:({void*_tmp49B[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp49C="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp49C,sizeof(char),_get_zero_arr_size(_tmp49C,42));}),
_tag_dynforward(_tmp49B,sizeof(void*),0));});_LL2A9:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp481->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp480);{
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_var_exp(max,0);if(_tmp482)_tmp49D=Cyc_Absyn_add_exp(
_tmp49D,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp47F,Cyc_Absyn_var_exp(max,0),_tmp481,_tmp482,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp49E=_new_region("r");struct _RegionHandle*r=& _tmp49E;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4AF=
_region_malloc(r,sizeof(*_tmp4AF));_tmp4AF->hd=({struct _tuple9*_tmp4B0=
_region_malloc(r,sizeof(*_tmp4B0));_tmp4B0->f1=max;_tmp4B0->f2=Cyc_Absyn_uint_typ;
_tmp4B0->f3=(struct Cyc_Absyn_Exp*)_tmp480;_tmp4B0;});_tmp4AF->tl=0;_tmp4AF;});
struct Cyc_Absyn_Exp*ai;if(_tmp37F == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp49F[2];_tmp49F[1]=_tmp49D;
_tmp49F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp49F,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp37F;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp4A0[2];_tmp4A0[1]=_tmp49D;_tmp4A0[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4A0,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp4A1=
_region_malloc(r,sizeof(*_tmp4A1));_tmp4A1->hd=({struct _tuple9*_tmp4A2=
_region_malloc(r,sizeof(*_tmp4A2));_tmp4A2->f1=a;_tmp4A2->f2=ptr_typ;_tmp4A2->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp4A2;});_tmp4A1->tl=decls;_tmp4A1;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp4A3=Cyc_Toc_temp_var();
void*_tmp4A4=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp4A5=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp4A6=Cyc_Absyn_fncall_exp(_tmp4A5,({struct Cyc_Absyn_Exp*_tmp4A9[3];_tmp4A9[2]=
_tmp49D;_tmp4A9[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp4A9[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4A9,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp4A7=_region_malloc(r,sizeof(*_tmp4A7));_tmp4A7->hd=({struct _tuple9*_tmp4A8=
_region_malloc(r,sizeof(*_tmp4A8));_tmp4A8->f1=_tmp4A3;_tmp4A8->f2=_tmp4A4;
_tmp4A8->f3=(struct Cyc_Absyn_Exp*)_tmp4A6;_tmp4A8;});_tmp4A7->tl=decls;_tmp4A7;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4A3,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp4AA=decls;for(0;_tmp4AA != 0;_tmp4AA=_tmp4AA->tl){struct
_tuple1*_tmp4AC;void*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct _tuple9 _tmp4AB=*((
struct _tuple9*)_tmp4AA->hd);_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;_tmp4AE=
_tmp4AB.f3;s=Cyc_Absyn_declare_stmt(_tmp4AC,_tmp4AD,_tmp4AE,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL299;}}}}_LL29E: if(*((int*)
_tmp47D)!= 30)goto _LL2A0;_tmp483=((struct Cyc_Absyn_Struct_e_struct*)_tmp47D)->f1;
_tmp484=((struct Cyc_Absyn_Struct_e_struct*)_tmp47D)->f2;_tmp485=((struct Cyc_Absyn_Struct_e_struct*)
_tmp47D)->f3;_LL29F:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp380->topt))->v,_tmp484 != 0,1,_tmp37F,_tmp485,
_tmp483))->r));goto _LL299;_LL2A0: if(*((int*)_tmp47D)!= 26)goto _LL2A2;_tmp486=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp47D)->f1;_LL2A1:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp37F,_tmp486))->r));goto _LL299;_LL2A2:;_LL2A3: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp380->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp4B1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_var_exp(_tmp4B1,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp4B2,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp37F == 0)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp37F;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*_tmp4B3=(void*)
_tmp380->r;void*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;_LL2AF: if(*((int*)_tmp4B3)!= 
15)goto _LL2B1;_tmp4B4=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4B3)->f1;
_tmp4B5=((struct Cyc_Absyn_Cast_e_struct*)_tmp4B3)->f2;_LL2B0:{struct _tuple0
_tmp4B7=({struct _tuple0 _tmp4B6;_tmp4B6.f1=Cyc_Tcutil_compress(_tmp4B4);_tmp4B6.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4B5->topt))->v);
_tmp4B6;});void*_tmp4B8;struct Cyc_Absyn_PtrInfo _tmp4B9;void*_tmp4BA;struct Cyc_Absyn_PtrAtts
_tmp4BB;struct Cyc_Absyn_Conref*_tmp4BC;void*_tmp4BD;struct Cyc_Absyn_PtrInfo
_tmp4BE;struct Cyc_Absyn_PtrAtts _tmp4BF;struct Cyc_Absyn_Conref*_tmp4C0;_LL2B4:
_tmp4B8=_tmp4B7.f1;if(_tmp4B8 <= (void*)4)goto _LL2B6;if(*((int*)_tmp4B8)!= 4)goto
_LL2B6;_tmp4B9=((struct Cyc_Absyn_PointerType_struct*)_tmp4B8)->f1;_tmp4BA=(void*)
_tmp4B9.elt_typ;_tmp4BB=_tmp4B9.ptr_atts;_tmp4BC=_tmp4BB.bounds;_tmp4BD=_tmp4B7.f2;
if(_tmp4BD <= (void*)4)goto _LL2B6;if(*((int*)_tmp4BD)!= 4)goto _LL2B6;_tmp4BE=((
struct Cyc_Absyn_PointerType_struct*)_tmp4BD)->f1;_tmp4BF=_tmp4BE.ptr_atts;
_tmp4C0=_tmp4BF.bounds;_LL2B5:{struct _tuple0 _tmp4C2=({struct _tuple0 _tmp4C1;
_tmp4C1.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4BC);_tmp4C1.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp4C0);_tmp4C1;});void*_tmp4C3;void*_tmp4C4;struct Cyc_Absyn_Exp*
_tmp4C5;void*_tmp4C6;void*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;_LL2B9: _tmp4C3=
_tmp4C2.f1;if((int)_tmp4C3 != 0)goto _LL2BB;_tmp4C4=_tmp4C2.f2;if(_tmp4C4 <= (void*)
2)goto _LL2BB;if(*((int*)_tmp4C4)!= 0)goto _LL2BB;_tmp4C5=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4C4)->f1;_LL2BA: forward_only=1;_tmp4C8=_tmp4C5;goto _LL2BC;_LL2BB: _tmp4C6=
_tmp4C2.f1;if((int)_tmp4C6 != 1)goto _LL2BD;_tmp4C7=_tmp4C2.f2;if(_tmp4C7 <= (void*)
2)goto _LL2BD;if(*((int*)_tmp4C7)!= 0)goto _LL2BD;_tmp4C8=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4C7)->f1;_LL2BC: Cyc_Toc_exp_to_c(nv,_tmp4B5);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp4C9=
forward_only?Cyc_Toc__init_dynforward_ptr_e: Cyc_Toc__init_dyneither_ptr_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4C9,({struct Cyc_Absyn_Exp*_tmp4CA[4];
_tmp4CA[3]=_tmp4C8;_tmp4CA[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4BA),
0);_tmp4CA[1]=_tmp4B5;_tmp4CA[0]=mexp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4CA,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2B8;}_LL2BD:;_LL2BE: goto _LL2B8;_LL2B8:;}goto _LL2B3;_LL2B6:;
_LL2B7: goto _LL2B3;_LL2B3:;}goto _LL2AE;_LL2B1:;_LL2B2: goto _LL2AE;_LL2AE:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp4CB=Cyc_Absyn_exp_stmt(_tmp4B2,0);struct Cyc_Absyn_Exp*
_tmp4CC=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp380);_tmp4CB=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp4B2,_tmp4CC,0),_tmp380,0),
_tmp4CB,0);{void*_tmp4CD=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4B1,_tmp4CD,(struct Cyc_Absyn_Exp*)
mexp,_tmp4CB,0)));}}goto _LL299;}}_LL299:;}goto _LL1C4;_LL1ED: if(*((int*)_tmp354)
!= 19)goto _LL1EF;_tmp381=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp354)->f1;
_LL1EE: Cyc_Toc_exp_to_c(nv,_tmp381);goto _LL1C4;_LL1EF: if(*((int*)_tmp354)!= 18)
goto _LL1F1;_tmp382=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp354)->f1;
_LL1F0:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4CE=
_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp4CF;_tmp4CF.tag=18;_tmp4CF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp382);_tmp4CF;});
_tmp4CE;})));goto _LL1C4;_LL1F1: if(*((int*)_tmp354)!= 21)goto _LL1F3;_LL1F2:({void*
_tmp4D0[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4D1="__gen() in code generator";_tag_dynforward(_tmp4D1,sizeof(
char),_get_zero_arr_size(_tmp4D1,26));}),_tag_dynforward(_tmp4D0,sizeof(void*),0));});
_LL1F3: if(*((int*)_tmp354)!= 20)goto _LL1F5;_tmp383=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp354)->f1;_tmp384=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp354)->f2;
if(*((int*)_tmp384)!= 0)goto _LL1F5;_tmp385=((struct Cyc_Absyn_StructField_struct*)
_tmp384)->f1;_LL1F4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp4D3;_tmp4D3.tag=20;_tmp4D3.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp383);_tmp4D3.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4D4=_cycalloc(sizeof(*
_tmp4D4));_tmp4D4[0]=({struct Cyc_Absyn_StructField_struct _tmp4D5;_tmp4D5.tag=0;
_tmp4D5.f1=_tmp385;_tmp4D5;});_tmp4D4;}));_tmp4D3;});_tmp4D2;})));goto _LL1C4;
_LL1F5: if(*((int*)_tmp354)!= 20)goto _LL1F7;_tmp386=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp354)->f1;_tmp387=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp354)->f2;
if(*((int*)_tmp387)!= 1)goto _LL1F7;_tmp388=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp387)->f1;_LL1F6:{void*_tmp4D6=Cyc_Tcutil_compress(_tmp386);struct Cyc_Absyn_AggrInfo
_tmp4D7;union Cyc_Absyn_AggrInfoU_union _tmp4D8;struct Cyc_List_List*_tmp4D9;_LL2C0:
if(_tmp4D6 <= (void*)4)goto _LL2C8;if(*((int*)_tmp4D6)!= 10)goto _LL2C2;_tmp4D7=((
struct Cyc_Absyn_AggrType_struct*)_tmp4D6)->f1;_tmp4D8=_tmp4D7.aggr_info;_LL2C1: {
struct Cyc_Absyn_Aggrdecl*_tmp4DA=Cyc_Absyn_get_known_aggrdecl(_tmp4D8);if(
_tmp4DA->impl == 0)({void*_tmp4DB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4DC="struct fields must be known";
_tag_dynforward(_tmp4DC,sizeof(char),_get_zero_arr_size(_tmp4DC,28));}),
_tag_dynforward(_tmp4DB,sizeof(void*),0));});_tmp4D9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4DA->impl))->fields;goto _LL2C3;}_LL2C2: if(*((int*)_tmp4D6)!= 11)
goto _LL2C4;_tmp4D9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4D6)->f2;_LL2C3: {
struct Cyc_Absyn_Aggrfield*_tmp4DD=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp4D9,(int)_tmp388);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4DF;_tmp4DF.tag=20;_tmp4DF.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp386);_tmp4DF.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4E0=
_cycalloc(sizeof(*_tmp4E0));_tmp4E0[0]=({struct Cyc_Absyn_StructField_struct
_tmp4E1;_tmp4E1.tag=0;_tmp4E1.f1=_tmp4DD->name;_tmp4E1;});_tmp4E0;}));_tmp4DF;});
_tmp4DE;})));goto _LL2BF;}_LL2C4: if(*((int*)_tmp4D6)!= 9)goto _LL2C6;_LL2C5:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4E2=_cycalloc(
sizeof(*_tmp4E2));_tmp4E2[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4E3;_tmp4E3.tag=
20;_tmp4E3.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp386);_tmp4E3.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({
struct Cyc_Absyn_StructField_struct _tmp4E5;_tmp4E5.tag=0;_tmp4E5.f1=Cyc_Absyn_fieldname((
int)(_tmp388 + 1));_tmp4E5;});_tmp4E4;}));_tmp4E3;});_tmp4E2;})));goto _LL2BF;
_LL2C6: if(*((int*)_tmp4D6)!= 3)goto _LL2C8;_LL2C7: if(_tmp388 == 0)(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4E6=_cycalloc(sizeof(*
_tmp4E6));_tmp4E6[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4E7;_tmp4E7.tag=20;
_tmp4E7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp386);_tmp4E7.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({
struct Cyc_Absyn_StructField_struct _tmp4E9;_tmp4E9.tag=0;_tmp4E9.f1=Cyc_Toc_tag_sp;
_tmp4E9;});_tmp4E8;}));_tmp4E7;});_tmp4E6;})));else{(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4EB;_tmp4EB.tag=20;_tmp4EB.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp386);_tmp4EB.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4EC=
_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_Absyn_StructField_struct
_tmp4ED;_tmp4ED.tag=0;_tmp4ED.f1=Cyc_Absyn_fieldname((int)_tmp388);_tmp4ED;});
_tmp4EC;}));_tmp4EB;});_tmp4EA;})));}goto _LL2BF;_LL2C8:;_LL2C9:({void*_tmp4EE[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4EF="impossible type for offsetof tuple index";_tag_dynforward(
_tmp4EF,sizeof(char),_get_zero_arr_size(_tmp4EF,41));}),_tag_dynforward(_tmp4EE,
sizeof(void*),0));});_LL2BF:;}goto _LL1C4;_LL1F7: if(*((int*)_tmp354)!= 22)goto
_LL1F9;_tmp389=((struct Cyc_Absyn_Deref_e_struct*)_tmp354)->f1;_LL1F8: {void*
_tmp4F0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp389->topt))->v);{
void*_tmp4F1=_tmp4F0;struct Cyc_Absyn_PtrInfo _tmp4F2;void*_tmp4F3;struct Cyc_Absyn_Tqual
_tmp4F4;struct Cyc_Absyn_PtrAtts _tmp4F5;void*_tmp4F6;struct Cyc_Absyn_Conref*
_tmp4F7;struct Cyc_Absyn_Conref*_tmp4F8;struct Cyc_Absyn_Conref*_tmp4F9;_LL2CB: if(
_tmp4F1 <= (void*)4)goto _LL2CD;if(*((int*)_tmp4F1)!= 4)goto _LL2CD;_tmp4F2=((
struct Cyc_Absyn_PointerType_struct*)_tmp4F1)->f1;_tmp4F3=(void*)_tmp4F2.elt_typ;
_tmp4F4=_tmp4F2.elt_tq;_tmp4F5=_tmp4F2.ptr_atts;_tmp4F6=(void*)_tmp4F5.rgn;
_tmp4F7=_tmp4F5.nullable;_tmp4F8=_tmp4F5.bounds;_tmp4F9=_tmp4F5.zero_term;_LL2CC:{
void*_tmp4FA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4F8);_LL2D0: if(
_tmp4FA <= (void*)2)goto _LL2D2;if(*((int*)_tmp4FA)!= 0)goto _LL2D2;_LL2D1: {int
do_null_check=Cyc_Toc_need_null_check(_tmp389);Cyc_Toc_exp_to_c(nv,_tmp389);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp4FB[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4FC="inserted null check due to dereference";
_tag_dynforward(_tmp4FC,sizeof(char),_get_zero_arr_size(_tmp4FC,39));}),
_tag_dynforward(_tmp4FB,sizeof(void*),0));});(void*)(_tmp389->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c(_tmp4F0),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->hd=Cyc_Absyn_copy_exp(_tmp389);
_tmp4FD->tl=0;_tmp4FD;}),0)));}goto _LL2CF;}_LL2D2: if((int)_tmp4FA != 0)goto _LL2D4;
_LL2D3: goto _LL2D5;_LL2D4: if((int)_tmp4FA != 1)goto _LL2D6;_LL2D5: {struct Cyc_Absyn_Exp*
_tmp4FE=Cyc_Absyn_uint_exp(0,0);_tmp4FE->topt=({struct Cyc_Core_Opt*_tmp4FF=
_cycalloc(sizeof(*_tmp4FF));_tmp4FF->v=(void*)Cyc_Absyn_uint_typ;_tmp4FF;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp389,_tmp4FE));Cyc_Toc_exp_to_c(nv,
e);goto _LL2CF;}_LL2D6: if(_tmp4FA <= (void*)2)goto _LL2CF;if(*((int*)_tmp4FA)!= 1)
goto _LL2CF;_LL2D7:({void*_tmp500[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp501="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp501,sizeof(char),_get_zero_arr_size(_tmp501,30));}),
_tag_dynforward(_tmp500,sizeof(void*),0));});_LL2CF:;}goto _LL2CA;_LL2CD:;_LL2CE:({
void*_tmp502[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp503="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp503,sizeof(char),_get_zero_arr_size(_tmp503,29));}),
_tag_dynforward(_tmp502,sizeof(void*),0));});_LL2CA:;}goto _LL1C4;}_LL1F9: if(*((
int*)_tmp354)!= 23)goto _LL1FB;_tmp38A=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp354)->f1;_tmp38B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp354)->f2;_LL1FA:
Cyc_Toc_exp_to_c(nv,_tmp38A);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1C4;
_LL1FB: if(*((int*)_tmp354)!= 24)goto _LL1FD;_tmp38C=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp354)->f1;_tmp38D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp354)->f2;_LL1FC: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp38C->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp38C);Cyc_Toc_exp_to_c(nv,_tmp38C);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp505;struct Cyc_Absyn_Tqual
_tmp506;struct Cyc_Absyn_PtrAtts _tmp507;void*_tmp508;struct Cyc_Absyn_Conref*
_tmp509;struct Cyc_Absyn_Conref*_tmp50A;struct Cyc_Absyn_Conref*_tmp50B;struct Cyc_Absyn_PtrInfo
_tmp504=Cyc_Toc_get_ptr_type(e1typ);_tmp505=(void*)_tmp504.elt_typ;_tmp506=
_tmp504.elt_tq;_tmp507=_tmp504.ptr_atts;_tmp508=(void*)_tmp507.rgn;_tmp509=
_tmp507.nullable;_tmp50A=_tmp507.bounds;_tmp50B=_tmp507.zero_term;{void*_tmp50C=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp50A);struct Cyc_Absyn_Exp*_tmp50D;
_LL2D9: if(_tmp50C <= (void*)2)goto _LL2DB;if(*((int*)_tmp50C)!= 0)goto _LL2DB;
_tmp50D=((struct Cyc_Absyn_Upper_b_struct*)_tmp50C)->f1;_LL2DA: {unsigned int
_tmp50F;int _tmp510;struct _tuple5 _tmp50E=Cyc_Evexp_eval_const_uint_exp(_tmp50D);
_tmp50F=_tmp50E.f1;_tmp510=_tmp50E.f2;if(_tmp510){if(_tmp50F < 1)({void*_tmp511[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp512="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp512,sizeof(char),_get_zero_arr_size(_tmp512,44));}),_tag_dynforward(_tmp511,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp513[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp514="inserted null check due to dereference";
_tag_dynforward(_tmp514,sizeof(char),_get_zero_arr_size(_tmp514,39));}),
_tag_dynforward(_tmp513,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp38C->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->hd=_tmp38C;_tmp515->tl=0;_tmp515;}),
0)),_tmp38D));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp38C->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp516[4];_tmp516[3]=Cyc_Absyn_uint_exp(0,0);_tmp516[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp505,0);_tmp516[1]=_tmp50D;_tmp516[0]=_tmp38C;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp516,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp38D));}goto _LL2D8;}_LL2DB: if((int)_tmp50C != 0)goto _LL2DD;_LL2DC:
forward_only=1;goto _LL2DE;_LL2DD: if((int)_tmp50C != 1)goto _LL2DF;_LL2DE: {struct
Cyc_Absyn_Exp*_tmp517=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp505);(void*)(_tmp38C->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp506),Cyc_Absyn_fncall_exp(_tmp517,({struct Cyc_Absyn_Exp*
_tmp518[3];_tmp518[2]=Cyc_Absyn_uint_exp(0,0);_tmp518[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp518[0]=Cyc_Absyn_copy_exp(_tmp38C);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp518,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL2D8;}_LL2DF: if(_tmp50C <= (void*)2)goto _LL2D8;if(*((int*)
_tmp50C)!= 1)goto _LL2D8;_LL2E0:({void*_tmp519[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp51A="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp51A,sizeof(char),_get_zero_arr_size(_tmp51A,34));}),
_tag_dynforward(_tmp519,sizeof(void*),0));});_LL2D8:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1C4;}}
_LL1FD: if(*((int*)_tmp354)!= 25)goto _LL1FF;_tmp38E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp354)->f1;_tmp38F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp354)->f2;_LL1FE: {
void*_tmp51B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp38E->topt))->v);{void*_tmp51C=_tmp51B;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_PtrInfo
_tmp51E;void*_tmp51F;struct Cyc_Absyn_Tqual _tmp520;struct Cyc_Absyn_PtrAtts _tmp521;
void*_tmp522;struct Cyc_Absyn_Conref*_tmp523;struct Cyc_Absyn_Conref*_tmp524;
struct Cyc_Absyn_Conref*_tmp525;_LL2E2: if(_tmp51C <= (void*)4)goto _LL2E6;if(*((int*)
_tmp51C)!= 9)goto _LL2E4;_tmp51D=((struct Cyc_Absyn_TupleType_struct*)_tmp51C)->f1;
_LL2E3: Cyc_Toc_exp_to_c(nv,_tmp38E);Cyc_Toc_exp_to_c(nv,_tmp38F);{unsigned int
_tmp527;int _tmp528;struct _tuple5 _tmp526=Cyc_Evexp_eval_const_uint_exp(_tmp38F);
_tmp527=_tmp526.f1;_tmp528=_tmp526.f2;if(!_tmp528)({void*_tmp529[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp52A="unknown tuple subscript in translation to C";_tag_dynforward(_tmp52A,
sizeof(char),_get_zero_arr_size(_tmp52A,44));}),_tag_dynforward(_tmp529,sizeof(
void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp38E,Cyc_Absyn_fieldname((
int)(_tmp527 + 1))));goto _LL2E1;}_LL2E4: if(*((int*)_tmp51C)!= 4)goto _LL2E6;
_tmp51E=((struct Cyc_Absyn_PointerType_struct*)_tmp51C)->f1;_tmp51F=(void*)
_tmp51E.elt_typ;_tmp520=_tmp51E.elt_tq;_tmp521=_tmp51E.ptr_atts;_tmp522=(void*)
_tmp521.rgn;_tmp523=_tmp521.nullable;_tmp524=_tmp521.bounds;_tmp525=_tmp521.zero_term;
_LL2E5: {struct Cyc_List_List*_tmp52B=Cyc_Toc_get_relns(_tmp38E);int in_bnds=0;{
void*_tmp52C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp524);_LL2E9: if(
_tmp52C <= (void*)2)goto _LL2EB;if(*((int*)_tmp52C)!= 1)goto _LL2EB;_LL2EA: goto
_LL2E8;_LL2EB:;_LL2EC: in_bnds=Cyc_Toc_check_bounds(_tmp52B,_tmp38E,_tmp38F);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp52F;
_tmp52F.tag=0;_tmp52F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp52D[1]={& _tmp52F};Cyc_Tcutil_warn(e->loc,({const char*_tmp52E="bounds check necessary for %s";
_tag_dynforward(_tmp52E,sizeof(char),_get_zero_arr_size(_tmp52E,30));}),
_tag_dynforward(_tmp52D,sizeof(void*),1));}});_LL2E8:;}Cyc_Toc_exp_to_c(nv,
_tmp38E);Cyc_Toc_exp_to_c(nv,_tmp38F);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp530=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp524);
struct Cyc_Absyn_Exp*_tmp531;void*_tmp532;_LL2EE: if(_tmp530 <= (void*)2)goto _LL2F0;
if(*((int*)_tmp530)!= 0)goto _LL2F0;_tmp531=((struct Cyc_Absyn_Upper_b_struct*)
_tmp530)->f1;_LL2EF: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp523);void*ta1=Cyc_Toc_typ_to_c(_tmp51F);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp520);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp525))(void*)(e->r=(void*)
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp533[3];_tmp533[2]=_tmp38F;_tmp533[1]=_tmp531;_tmp533[0]=
_tmp38E;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp533,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp534[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp535="inserted null check due to dereference";
_tag_dynforward(_tmp535,sizeof(char),_get_zero_arr_size(_tmp535,39));}),
_tag_dynforward(_tmp534,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp536[4];_tmp536[3]=_tmp38F;_tmp536[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp536[1]=_tmp531;_tmp536[0]=_tmp38E;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp536,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp38F->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp537[2];_tmp537[1]=Cyc_Absyn_copy_exp(_tmp38F);_tmp537[0]=
_tmp531;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp537,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL2ED;}
_LL2F0: if((int)_tmp530 != 0)goto _LL2F2;_LL2F1: forward_only=1;goto _LL2F3;_LL2F2:
if((int)_tmp530 != 1)goto _LL2F4;_LL2F3: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp51F);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp520),Cyc_Absyn_aggrmember_exp(
_tmp38E,Cyc_Toc_curr_sp,0)),_tmp38F));}else{struct Cyc_Absyn_Exp*_tmp538=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp520),Cyc_Absyn_fncall_exp(_tmp538,({struct Cyc_Absyn_Exp*_tmp539[3];_tmp539[2]=
_tmp38F;_tmp539[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp539[0]=_tmp38E;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp539,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL2ED;}_LL2F4: if(_tmp530 <= (void*)2)goto _LL2ED;if(*((int*)
_tmp530)!= 1)goto _LL2ED;_tmp532=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp530)->f1;_LL2F5:{void*_tmp53A=(void*)_tmp38F->annot;struct Cyc_List_List*
_tmp53B;_LL2F7: if(*((void**)_tmp53A)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL2F9;
_tmp53B=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp53A)->f1;_LL2F8: for(0;
_tmp53B != 0;_tmp53B=_tmp53B->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp53B->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp53B->hd)->bd,_tmp532)== 0){in_bnds=1;break;}}goto _LL2F6;_LL2F9:;_LL2FA: goto
_LL2F6;_LL2F6:;}if(!in_bnds)({void*_tmp53C[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp53D="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp53D,sizeof(char),_get_zero_arr_size(_tmp53D,45));}),_tag_dynforward(_tmp53C,
sizeof(void*),0));});goto _LL2ED;_LL2ED:;}goto _LL2E1;}}_LL2E6:;_LL2E7:({void*
_tmp53E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp53F="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp53F,sizeof(char),_get_zero_arr_size(_tmp53F,49));}),
_tag_dynforward(_tmp53E,sizeof(void*),0));});_LL2E1:;}goto _LL1C4;}_LL1FF: if(*((
int*)_tmp354)!= 26)goto _LL201;_tmp390=((struct Cyc_Absyn_Tuple_e_struct*)_tmp354)->f1;
_LL200: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp390))->r));else{struct Cyc_List_List*_tmp540=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp390);struct _dynforward_ptr*_tmp541=Cyc_Toc_add_tuple_type(
_tmp540);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp390 != 0;(_tmp390=_tmp390->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp390->hd);dles=({struct Cyc_List_List*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->hd=({struct _tuple6*_tmp543=
_cycalloc(sizeof(*_tmp543));_tmp543->f1=0;_tmp543->f2=(struct Cyc_Absyn_Exp*)
_tmp390->hd;_tmp543;});_tmp542->tl=dles;_tmp542;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}goto _LL1C4;_LL201: if(*((int*)_tmp354)!= 28)goto _LL203;_tmp391=((struct
Cyc_Absyn_Array_e_struct*)_tmp354)->f1;_LL202:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp391));{struct Cyc_List_List*_tmp544=_tmp391;for(0;_tmp544 != 0;_tmp544=
_tmp544->tl){struct _tuple6 _tmp546;struct Cyc_Absyn_Exp*_tmp547;struct _tuple6*
_tmp545=(struct _tuple6*)_tmp544->hd;_tmp546=*_tmp545;_tmp547=_tmp546.f2;Cyc_Toc_exp_to_c(
nv,_tmp547);}}goto _LL1C4;_LL203: if(*((int*)_tmp354)!= 29)goto _LL205;_tmp392=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp354)->f1;_tmp393=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp354)->f2;_tmp394=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp354)->f3;
_tmp395=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp354)->f4;_LL204: {
unsigned int _tmp549;int _tmp54A;struct _tuple5 _tmp548=Cyc_Evexp_eval_const_uint_exp(
_tmp393);_tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;{void*_tmp54B=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp394->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp394);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp394)){if(!_tmp54A)({
void*_tmp54C[0]={};Cyc_Tcutil_terr(_tmp393->loc,({const char*_tmp54D="cannot determine value of constant";
_tag_dynforward(_tmp54D,sizeof(char),_get_zero_arr_size(_tmp54D,35));}),
_tag_dynforward(_tmp54C,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp549;i
++){es=({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->hd=({
struct _tuple6*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->f1=0;_tmp54F->f2=
_tmp394;_tmp54F;});_tmp54E->tl=es;_tmp54E;});}}if(_tmp395){struct Cyc_Absyn_Exp*
_tmp550=Cyc_Toc_cast_it(_tmp54B,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->hd=({struct _tuple6*_tmp552=
_cycalloc(sizeof(*_tmp552));_tmp552->f1=0;_tmp552->f2=_tmp550;_tmp552;});_tmp551->tl=
0;_tmp551;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1C4;}}}_LL205: if(*((int*)_tmp354)!= 30)goto _LL207;_tmp396=((struct Cyc_Absyn_Struct_e_struct*)
_tmp354)->f1;_tmp397=((struct Cyc_Absyn_Struct_e_struct*)_tmp354)->f2;_tmp398=((
struct Cyc_Absyn_Struct_e_struct*)_tmp354)->f3;_tmp399=((struct Cyc_Absyn_Struct_e_struct*)
_tmp354)->f4;_LL206: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp397 != 0,0,0,_tmp398,_tmp396))->r));else{if(_tmp399 == 0)({void*
_tmp553[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp554="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp554,
sizeof(char),_get_zero_arr_size(_tmp554,35));}),_tag_dynforward(_tmp553,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp399;
struct _RegionHandle _tmp555=_new_region("rgn");struct _RegionHandle*rgn=& _tmp555;
_push_region(rgn);{struct Cyc_List_List*_tmp556=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp398,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp557=0;struct Cyc_List_List*_tmp558=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp558 != 0;_tmp558=_tmp558->tl){struct Cyc_List_List*
_tmp559=_tmp556;for(0;_tmp559 != 0;_tmp559=_tmp559->tl){if((*((struct _tuple10*)
_tmp559->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp558->hd){struct _tuple10
_tmp55B;struct Cyc_Absyn_Aggrfield*_tmp55C;struct Cyc_Absyn_Exp*_tmp55D;struct
_tuple10*_tmp55A=(struct _tuple10*)_tmp559->hd;_tmp55B=*_tmp55A;_tmp55C=_tmp55B.f1;
_tmp55D=_tmp55B.f2;{void*_tmp55E=(void*)_tmp55C->type;Cyc_Toc_exp_to_c(nv,
_tmp55D);if(Cyc_Toc_is_void_star(_tmp55E))(void*)(_tmp55D->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp55D->r,0)));_tmp557=({
struct Cyc_List_List*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->hd=({struct
_tuple6*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->f1=0;_tmp560->f2=_tmp55D;
_tmp560;});_tmp55F->tl=_tmp557;_tmp55F;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp557)));};
_pop_region(rgn);}}goto _LL1C4;_LL207: if(*((int*)_tmp354)!= 31)goto _LL209;_tmp39A=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp354)->f1;_tmp39B=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp354)->f2;_LL208: {struct Cyc_List_List*fs;{void*_tmp561=Cyc_Tcutil_compress(
_tmp39A);struct Cyc_List_List*_tmp562;_LL2FC: if(_tmp561 <= (void*)4)goto _LL2FE;if(*((
int*)_tmp561)!= 11)goto _LL2FE;_tmp562=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp561)->f2;_LL2FD: fs=_tmp562;goto _LL2FB;_LL2FE:;_LL2FF:({struct Cyc_String_pa_struct
_tmp565;_tmp565.tag=0;_tmp565.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp39A));{void*_tmp563[1]={& _tmp565};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp564="anon struct has type %s";_tag_dynforward(_tmp564,sizeof(char),
_get_zero_arr_size(_tmp564,24));}),_tag_dynforward(_tmp563,sizeof(void*),1));}});
_LL2FB:;}{struct _RegionHandle _tmp566=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp566;_push_region(rgn);{struct Cyc_List_List*_tmp567=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp39B,fs);for(0;_tmp567 != 0;_tmp567=_tmp567->tl){struct _tuple10 _tmp569;struct
Cyc_Absyn_Aggrfield*_tmp56A;struct Cyc_Absyn_Exp*_tmp56B;struct _tuple10*_tmp568=(
struct _tuple10*)_tmp567->hd;_tmp569=*_tmp568;_tmp56A=_tmp569.f1;_tmp56B=_tmp569.f2;{
void*_tmp56C=(void*)_tmp56A->type;Cyc_Toc_exp_to_c(nv,_tmp56B);if(Cyc_Toc_is_void_star(
_tmp56C))(void*)(_tmp56B->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp56B->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp39B));};_pop_region(rgn);}goto _LL1C4;}_LL209: if(*((int*)_tmp354)!= 32)goto
_LL20B;_tmp39C=((struct Cyc_Absyn_Tunion_e_struct*)_tmp354)->f1;if(_tmp39C != 0)
goto _LL20B;_tmp39D=((struct Cyc_Absyn_Tunion_e_struct*)_tmp354)->f2;_tmp39E=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp354)->f3;if(!(!_tmp39D->is_flat))goto _LL20B;
_LL20A: {struct _tuple1*qv=_tmp39E->name;struct Cyc_Absyn_Exp*tag_exp=_tmp39D->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp39D,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1C4;}_LL20B: if(*((int*)_tmp354)!= 32)goto _LL20D;
_tmp39F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp354)->f1;_tmp3A0=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp354)->f2;_tmp3A1=((struct Cyc_Absyn_Tunion_e_struct*)_tmp354)->f3;_LL20C: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp56D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp56E=Cyc_Absyn_var_exp(_tmp56D,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp3A0->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp3A0->name,({const char*_tmp56F="_union";_tag_dynforward(_tmp56F,sizeof(char),
_get_zero_arr_size(_tmp56F,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp3A0,_tmp3A1->name,
1);{struct _tuple1 _tmp571;struct _dynforward_ptr*_tmp572;struct _tuple1*_tmp570=
_tmp3A1->name;_tmp571=*_tmp570;_tmp572=_tmp571.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp56E,_tmp572,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3A1->name,({const char*_tmp573="_struct";_tag_dynforward(_tmp573,sizeof(char),
_get_zero_arr_size(_tmp573,8));})));tag_exp=_tmp3A0->is_xtunion?Cyc_Absyn_var_exp(
_tmp3A1->name,0): Cyc_Toc_tunion_tag(_tmp3A0,_tmp3A1->name,1);member_exp=_tmp56E;}{
struct Cyc_List_List*_tmp574=_tmp3A1->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp39F != 0;(_tmp39F=_tmp39F->tl,_tmp574=_tmp574->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp39F->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp574))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->hd=({
struct _tuple6*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->f1=0;_tmp576->f2=
cur_e;_tmp576;});_tmp575->tl=dles;_tmp575;});}dles=({struct Cyc_List_List*_tmp577=
_cycalloc(sizeof(*_tmp577));_tmp577->hd=({struct _tuple6*_tmp578=_cycalloc(
sizeof(*_tmp578));_tmp578->f1=0;_tmp578->f2=tag_exp;_tmp578;});_tmp577->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp577;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp3A0->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3A1->name,({const char*_tmp579="_struct";_tag_dynforward(_tmp579,sizeof(char),
_get_zero_arr_size(_tmp579,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp57A=({struct Cyc_List_List*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp580->tl=0;_tmp580;});{int i=1;for(0;_tmp39F != 0;(((_tmp39F=_tmp39F->tl,
i ++)),_tmp574=_tmp574->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp39F->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp574))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp57B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp57A=({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));
_tmp57C->hd=_tmp57B;_tmp57C->tl=_tmp57A;_tmp57C;});}}}{struct Cyc_Absyn_Stmt*
_tmp57D=Cyc_Absyn_exp_stmt(_tmp56E,0);struct Cyc_Absyn_Stmt*_tmp57E=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->hd=_tmp57D;_tmp57F->tl=_tmp57A;
_tmp57F;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp56D,tunion_ctype,0,_tmp57E,0)));}}goto _LL1C4;}}_LL20D: if(*((int*)_tmp354)!= 
33)goto _LL20F;_LL20E: goto _LL210;_LL20F: if(*((int*)_tmp354)!= 34)goto _LL211;
_LL210: goto _LL1C4;_LL211: if(*((int*)_tmp354)!= 35)goto _LL213;_tmp3A2=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp354)->f1;_tmp3A3=_tmp3A2.is_calloc;_tmp3A4=_tmp3A2.rgn;_tmp3A5=_tmp3A2.elt_type;
_tmp3A6=_tmp3A2.num_elts;_tmp3A7=_tmp3A2.fat_result;_LL212: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp3A5)));Cyc_Toc_exp_to_c(nv,_tmp3A6);if(_tmp3A7){struct
_tuple1*_tmp581=Cyc_Toc_temp_var();struct _tuple1*_tmp582=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp3A3){xexp=_tmp3A6;if(_tmp3A4 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3A4;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp581,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp3A5,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp581,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp583[3];_tmp583[2]=Cyc_Absyn_var_exp(_tmp581,0);_tmp583[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp583[0]=Cyc_Absyn_var_exp(_tmp582,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp583,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp3A6,0);if(_tmp3A4 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3A4;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp581,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp3A5,Cyc_Absyn_var_exp(_tmp581,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp584[3];_tmp584[2]=Cyc_Absyn_var_exp(_tmp581,0);_tmp584[1]=Cyc_Absyn_uint_exp(
1,0);_tmp584[0]=Cyc_Absyn_var_exp(_tmp582,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp584,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp585=Cyc_Absyn_declare_stmt(_tmp581,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp582,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp585));}}else{if(_tmp3A4 != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp3A4;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((
void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}else{(void*)(
e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp3A5,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}}
goto _LL1C4;}_LL213: if(*((int*)_tmp354)!= 36)goto _LL215;_tmp3A8=((struct Cyc_Absyn_Swap_e_struct*)
_tmp354)->f1;_tmp3A9=((struct Cyc_Absyn_Swap_e_struct*)_tmp354)->f2;_LL214: {int
is_dynforward_ptr=0;int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A8->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A9->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dynforward_ptr,& is_dyneither_ptr))({void*_tmp586[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp587="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dynforward(_tmp587,sizeof(char),_get_zero_arr_size(_tmp587,57));}),
_tag_dynforward(_tmp586,sizeof(void*),0));});{struct Cyc_Absyn_Exp*swap_fn;if(
is_dynforward_ptr)swap_fn=Cyc_Toc__swap_dynforward_e;else{if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{swap_fn=Cyc_Toc__swap_word_e;}}if(!Cyc_Absyn_is_lvalue(
_tmp3A8))({struct Cyc_String_pa_struct _tmp58A;_tmp58A.tag=0;_tmp58A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp3A8));{void*
_tmp588[1]={& _tmp58A};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp589="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp589,sizeof(char),_get_zero_arr_size(_tmp589,30));}),
_tag_dynforward(_tmp588,sizeof(void*),1));}});if(!Cyc_Absyn_is_lvalue(_tmp3A9))({
struct Cyc_String_pa_struct _tmp58D;_tmp58D.tag=0;_tmp58D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp3A9));{void*
_tmp58B[1]={& _tmp58D};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp58C="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp58C,sizeof(char),_get_zero_arr_size(_tmp58C,30));}),
_tag_dynforward(_tmp58B,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp3A8);Cyc_Toc_exp_to_c(
nv,_tmp3A9);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*
_tmp58E[2];_tmp58E[1]=Cyc_Absyn_address_exp(_tmp3A9,0);_tmp58E[0]=Cyc_Absyn_address_exp(
_tmp3A8,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp58E,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL1C4;}}
_LL215: if(*((int*)_tmp354)!= 38)goto _LL217;_tmp3AA=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp354)->f1;_LL216: Cyc_Toc_stmt_to_c(nv,_tmp3AA);goto _LL1C4;_LL217: if(*((int*)
_tmp354)!= 37)goto _LL219;_LL218:({void*_tmp58F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp590="UnresolvedMem";_tag_dynforward(_tmp590,sizeof(char),_get_zero_arr_size(
_tmp590,14));}),_tag_dynforward(_tmp58F,sizeof(void*),0));});_LL219: if(*((int*)
_tmp354)!= 27)goto _LL1C4;_LL21A:({void*_tmp591[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp592="compoundlit";
_tag_dynforward(_tmp592,sizeof(char),_get_zero_arr_size(_tmp592,12));}),
_tag_dynforward(_tmp591,sizeof(void*),0));});_LL1C4:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple12{struct _tuple1*f1;void*f2;};struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp593=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp594;struct Cyc_Absyn_Vardecl
_tmp595;struct _tuple1*_tmp596;struct Cyc_Absyn_Pat*_tmp597;struct Cyc_Absyn_Vardecl*
_tmp598;struct Cyc_Absyn_Vardecl _tmp599;struct _tuple1*_tmp59A;struct Cyc_Absyn_Vardecl*
_tmp59B;struct Cyc_Absyn_Pat*_tmp59C;void*_tmp59D;int _tmp59E;char _tmp59F;struct
_dynforward_ptr _tmp5A0;struct Cyc_Absyn_Enumdecl*_tmp5A1;struct Cyc_Absyn_Enumfield*
_tmp5A2;void*_tmp5A3;struct Cyc_Absyn_Enumfield*_tmp5A4;struct Cyc_Absyn_Tuniondecl*
_tmp5A5;struct Cyc_Absyn_Tunionfield*_tmp5A6;struct Cyc_List_List*_tmp5A7;struct
Cyc_Absyn_Pat*_tmp5A8;struct Cyc_Absyn_Pat _tmp5A9;void*_tmp5AA;struct Cyc_Absyn_Tuniondecl*
_tmp5AB;struct Cyc_Absyn_Tunionfield*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct
Cyc_Absyn_Tuniondecl*_tmp5AE;struct Cyc_Absyn_Tunionfield*_tmp5AF;struct Cyc_List_List*
_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_List_List*_tmp5B2;struct Cyc_Absyn_AggrInfo
_tmp5B3;union Cyc_Absyn_AggrInfoU_union _tmp5B4;struct Cyc_List_List*_tmp5B5;struct
Cyc_Absyn_Pat*_tmp5B6;_LL301: if(_tmp593 <= (void*)2)goto _LL305;if(*((int*)_tmp593)
!= 0)goto _LL303;_tmp594=((struct Cyc_Absyn_Var_p_struct*)_tmp593)->f1;_tmp595=*
_tmp594;_tmp596=_tmp595.name;_tmp597=((struct Cyc_Absyn_Var_p_struct*)_tmp593)->f2;
_LL302: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp596,r),rgn,t,r,path,
_tmp597,tag_fail_stmt,fail_stmt,decls);_LL303: if(*((int*)_tmp593)!= 2)goto _LL305;
_tmp598=((struct Cyc_Absyn_TagInt_p_struct*)_tmp593)->f2;_tmp599=*_tmp598;_tmp59A=
_tmp599.name;_LL304: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp59A,r);goto _LL306;_LL305:
if((int)_tmp593 != 0)goto _LL307;_LL306: s=Cyc_Toc_skip_stmt_dl();goto _LL300;_LL307:
if(_tmp593 <= (void*)2)goto _LL309;if(*((int*)_tmp593)!= 1)goto _LL309;_tmp59B=((
struct Cyc_Absyn_Reference_p_struct*)_tmp593)->f1;_tmp59C=((struct Cyc_Absyn_Reference_p_struct*)
_tmp593)->f2;_LL308: {struct _tuple1*_tmp5B7=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp5B8=_region_malloc(rgn,sizeof(*_tmp5B8));_tmp5B8->hd=({struct _tuple12*
_tmp5B9=_region_malloc(rgn,sizeof(*_tmp5B9));_tmp5B9->f1=_tmp5B7;_tmp5B9->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp5B9;});_tmp5B8->tl=decls;_tmp5B8;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp59B->name,Cyc_Absyn_var_exp(_tmp5B7,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp5B7,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple11 _tmp5BA=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp59C,tag_fail_stmt,fail_stmt,decls);_tmp5BA.f3=Cyc_Absyn_seq_stmt(
s,_tmp5BA.f3,0);return _tmp5BA;}}_LL309: if((int)_tmp593 != 1)goto _LL30B;_LL30A: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL300;_LL30B:
if(_tmp593 <= (void*)2)goto _LL30D;if(*((int*)_tmp593)!= 7)goto _LL30D;_tmp59D=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp593)->f1;_tmp59E=((struct Cyc_Absyn_Int_p_struct*)
_tmp593)->f2;_LL30C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp59D,_tmp59E,0),
fail_stmt);goto _LL300;_LL30D: if(_tmp593 <= (void*)2)goto _LL30F;if(*((int*)_tmp593)
!= 8)goto _LL30F;_tmp59F=((struct Cyc_Absyn_Char_p_struct*)_tmp593)->f1;_LL30E: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp59F,0),fail_stmt);goto _LL300;_LL30F:
if(_tmp593 <= (void*)2)goto _LL311;if(*((int*)_tmp593)!= 9)goto _LL311;_tmp5A0=((
struct Cyc_Absyn_Float_p_struct*)_tmp593)->f1;_LL310: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp5A0,0),fail_stmt);goto _LL300;_LL311: if(_tmp593 <= (void*)2)goto _LL313;if(*((
int*)_tmp593)!= 10)goto _LL313;_tmp5A1=((struct Cyc_Absyn_Enum_p_struct*)_tmp593)->f1;
_tmp5A2=((struct Cyc_Absyn_Enum_p_struct*)_tmp593)->f2;_LL312: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp5BB=_cycalloc(
sizeof(*_tmp5BB));_tmp5BB[0]=({struct Cyc_Absyn_Enum_e_struct _tmp5BC;_tmp5BC.tag=
33;_tmp5BC.f1=_tmp5A2->name;_tmp5BC.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5A1;
_tmp5BC.f3=(struct Cyc_Absyn_Enumfield*)_tmp5A2;_tmp5BC;});_tmp5BB;}),0),
fail_stmt);goto _LL300;_LL313: if(_tmp593 <= (void*)2)goto _LL315;if(*((int*)_tmp593)
!= 11)goto _LL315;_tmp5A3=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp593)->f1;
_tmp5A4=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp593)->f2;_LL314: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp5BD=_cycalloc(
sizeof(*_tmp5BD));_tmp5BD[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp5BE;_tmp5BE.tag=
34;_tmp5BE.f1=_tmp5A4->name;_tmp5BE.f2=(void*)_tmp5A3;_tmp5BE.f3=(struct Cyc_Absyn_Enumfield*)
_tmp5A4;_tmp5BE;});_tmp5BD;}),0),fail_stmt);goto _LL300;_LL315: if(_tmp593 <= (void*)
2)goto _LL317;if(*((int*)_tmp593)!= 6)goto _LL317;_tmp5A5=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp593)->f1;_tmp5A6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp593)->f2;_tmp5A7=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp593)->f3;if(_tmp5A7 != 0)goto _LL317;if(!(!
_tmp5A5->is_flat))goto _LL317;_LL316: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp5A5->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp5A6->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp5A5,_tmp5A6->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL300;}_LL317: if(_tmp593 <= (void*)2)goto _LL319;if(*((
int*)_tmp593)!= 4)goto _LL319;_tmp5A8=((struct Cyc_Absyn_Pointer_p_struct*)_tmp593)->f1;
_tmp5A9=*_tmp5A8;_tmp5AA=(void*)_tmp5A9.r;if(_tmp5AA <= (void*)2)goto _LL319;if(*((
int*)_tmp5AA)!= 6)goto _LL319;_tmp5AB=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5AA)->f1;
_tmp5AC=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5AA)->f2;_tmp5AD=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5AA)->f3;if(!(_tmp5AD != 0  && !_tmp5AB->is_flat))goto _LL319;_LL318: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp5AC->name,({const
char*_tmp5CB="_struct";_tag_dynforward(_tmp5CB,sizeof(char),_get_zero_arr_size(
_tmp5CB,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp5BF=_tmp5AC->typs;
for(0;_tmp5AD != 0;(((_tmp5AD=_tmp5AD->tl,_tmp5BF=((struct Cyc_List_List*)
_check_null(_tmp5BF))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp5C0=(struct Cyc_Absyn_Pat*)
_tmp5AD->hd;if((void*)_tmp5C0->r == (void*)0)continue;{void*_tmp5C1=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5BF))->hd)).f2;struct _tuple1*
_tmp5C2=Cyc_Toc_temp_var();void*_tmp5C3=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5C0->topt))->v;void*_tmp5C4=Cyc_Toc_typ_to_c(_tmp5C3);struct Cyc_Absyn_Exp*
_tmp5C5=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp5C1)))_tmp5C5=Cyc_Toc_cast_it(_tmp5C4,_tmp5C5);decls=({
struct Cyc_List_List*_tmp5C6=_region_malloc(rgn,sizeof(*_tmp5C6));_tmp5C6->hd=({
struct _tuple12*_tmp5C7=_region_malloc(rgn,sizeof(*_tmp5C7));_tmp5C7->f1=_tmp5C2;
_tmp5C7->f2=_tmp5C4;_tmp5C7;});_tmp5C6->tl=decls;_tmp5C6;});{struct _tuple11
_tmp5C8=Cyc_Toc_xlate_pat(nv,rgn,_tmp5C3,Cyc_Absyn_var_exp(_tmp5C2,0),_tmp5C5,
_tmp5C0,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5C8.f1;decls=
_tmp5C8.f2;{struct Cyc_Absyn_Stmt*_tmp5C9=_tmp5C8.f3;struct Cyc_Absyn_Stmt*_tmp5CA=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5C2,0),_tmp5C5,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5CA,_tmp5C9,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp5AB->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp5AC->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp5AB,_tmp5AC->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp5AB);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL300;}}_LL319: if(_tmp593 <= (void*)2)goto
_LL31B;if(*((int*)_tmp593)!= 6)goto _LL31B;_tmp5AE=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp593)->f1;_tmp5AF=((struct Cyc_Absyn_Tunion_p_struct*)_tmp593)->f2;_tmp5B0=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp593)->f3;if(!_tmp5AE->is_flat)goto _LL31B;
_LL31A: {struct _tuple1 _tmp5CD;struct _dynforward_ptr*_tmp5CE;struct _tuple1*
_tmp5CC=_tmp5AF->name;_tmp5CD=*_tmp5CC;_tmp5CE=_tmp5CD.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp5CE,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp5CE,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp5AE,_tmp5AF->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp5B0 != 0;(_tmp5B0=_tmp5B0->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5CF=(struct Cyc_Absyn_Pat*)
_tmp5B0->hd;if((void*)_tmp5CF->r == (void*)0)continue;{struct _tuple1*_tmp5D0=Cyc_Toc_temp_var();
void*_tmp5D1=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5CF->topt))->v;decls=({
struct Cyc_List_List*_tmp5D2=_region_malloc(rgn,sizeof(*_tmp5D2));_tmp5D2->hd=({
struct _tuple12*_tmp5D3=_region_malloc(rgn,sizeof(*_tmp5D3));_tmp5D3->f1=_tmp5D0;
_tmp5D3->f2=Cyc_Toc_typ_to_c(_tmp5D1);_tmp5D3;});_tmp5D2->tl=decls;_tmp5D2;});{
struct _tuple11 _tmp5D4=Cyc_Toc_xlate_pat(nv,rgn,_tmp5D1,Cyc_Absyn_var_exp(_tmp5D0,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5CF,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5D4.f1;decls=_tmp5D4.f2;{struct Cyc_Absyn_Stmt*
_tmp5D5=_tmp5D4.f3;struct Cyc_Absyn_Stmt*_tmp5D6=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5D0,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5D6,_tmp5D5,0),0);}}}}goto _LL300;}}_LL31B: if(_tmp593 <= (
void*)2)goto _LL31D;if(*((int*)_tmp593)!= 6)goto _LL31D;_tmp5B1=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp593)->f3;_LL31C: _tmp5B2=_tmp5B1;goto _LL31E;_LL31D: if(_tmp593 <= (void*)2)goto
_LL31F;if(*((int*)_tmp593)!= 3)goto _LL31F;_tmp5B2=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp593)->f1;_LL31E: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp5B2 != 0;(
_tmp5B2=_tmp5B2->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5D7=(struct Cyc_Absyn_Pat*)
_tmp5B2->hd;if((void*)_tmp5D7->r == (void*)0)continue;{struct _tuple1*_tmp5D8=Cyc_Toc_temp_var();
void*_tmp5D9=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5D7->topt))->v;decls=({
struct Cyc_List_List*_tmp5DA=_region_malloc(rgn,sizeof(*_tmp5DA));_tmp5DA->hd=({
struct _tuple12*_tmp5DB=_region_malloc(rgn,sizeof(*_tmp5DB));_tmp5DB->f1=_tmp5D8;
_tmp5DB->f2=Cyc_Toc_typ_to_c(_tmp5D9);_tmp5DB;});_tmp5DA->tl=decls;_tmp5DA;});{
struct _tuple11 _tmp5DC=Cyc_Toc_xlate_pat(nv,rgn,_tmp5D9,Cyc_Absyn_var_exp(_tmp5D8,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5D7,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5DC.f1;decls=_tmp5DC.f2;{struct Cyc_Absyn_Stmt*
_tmp5DD=_tmp5DC.f3;struct Cyc_Absyn_Stmt*_tmp5DE=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5D8,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5DE,_tmp5DD,0),0);}}}}goto _LL300;}_LL31F: if(_tmp593 <= (
void*)2)goto _LL321;if(*((int*)_tmp593)!= 5)goto _LL321;_tmp5B3=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp593)->f1;_tmp5B4=_tmp5B3.aggr_info;_tmp5B5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp593)->f3;_LL320: {struct Cyc_Absyn_Aggrdecl*_tmp5DF=Cyc_Absyn_get_known_aggrdecl(
_tmp5B4);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp5B5 != 0;_tmp5B5=_tmp5B5->tl){struct
_tuple13*_tmp5E0=(struct _tuple13*)_tmp5B5->hd;struct Cyc_Absyn_Pat*_tmp5E1=(*
_tmp5E0).f2;if((void*)_tmp5E1->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp5E2=(void*)((struct Cyc_List_List*)_check_null((*_tmp5E0).f1))->hd;
struct _dynforward_ptr*_tmp5E3;_LL32A: if(*((int*)_tmp5E2)!= 1)goto _LL32C;_tmp5E3=((
struct Cyc_Absyn_FieldName_struct*)_tmp5E2)->f1;_LL32B: f=_tmp5E3;goto _LL329;
_LL32C:;_LL32D:(int)_throw((void*)Cyc_Toc_Match_error);_LL329:;}{struct _tuple1*
_tmp5E4=Cyc_Toc_temp_var();void*_tmp5E5=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5E1->topt))->v;void*_tmp5E6=Cyc_Toc_typ_to_c(_tmp5E5);decls=({struct Cyc_List_List*
_tmp5E7=_region_malloc(rgn,sizeof(*_tmp5E7));_tmp5E7->hd=({struct _tuple12*
_tmp5E8=_region_malloc(rgn,sizeof(*_tmp5E8));_tmp5E8->f1=_tmp5E4;_tmp5E8->f2=
_tmp5E6;_tmp5E8;});_tmp5E7->tl=decls;_tmp5E7;});{struct _tuple11 _tmp5E9=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5E5,Cyc_Absyn_var_exp(_tmp5E4,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp5E1,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5E9.f1;decls=
_tmp5E9.f2;{struct Cyc_Absyn_Exp*_tmp5EA=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5DF->impl))->fields,f)))->type))_tmp5EA=
Cyc_Toc_cast_it(_tmp5E6,_tmp5EA);{struct Cyc_Absyn_Stmt*_tmp5EB=_tmp5E9.f3;struct
Cyc_Absyn_Stmt*_tmp5EC=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5E4,0),
_tmp5EA,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp5EC,_tmp5EB,0),0);}}}}}}
goto _LL300;}_LL321: if(_tmp593 <= (void*)2)goto _LL323;if(*((int*)_tmp593)!= 4)goto
_LL323;_tmp5B6=((struct Cyc_Absyn_Pointer_p_struct*)_tmp593)->f1;_LL322: {struct
_tuple1*_tmp5ED=Cyc_Toc_temp_var();void*_tmp5EE=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5B6->topt))->v;decls=({struct Cyc_List_List*_tmp5EF=
_region_malloc(rgn,sizeof(*_tmp5EF));_tmp5EF->hd=({struct _tuple12*_tmp5F0=
_region_malloc(rgn,sizeof(*_tmp5F0));_tmp5F0->f1=_tmp5ED;_tmp5F0->f2=Cyc_Toc_typ_to_c(
_tmp5EE);_tmp5F0;});_tmp5EF->tl=decls;_tmp5EF;});{struct _tuple11 _tmp5F1=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5EE,Cyc_Absyn_var_exp(_tmp5ED,0),Cyc_Absyn_deref_exp(path,0),_tmp5B6,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5F1.f1;decls=_tmp5F1.f2;{
struct Cyc_Absyn_Stmt*_tmp5F2=_tmp5F1.f3;struct Cyc_Absyn_Stmt*_tmp5F3=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5ED,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp5F2,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp5F3,0);else{s=_tmp5F3;}goto _LL300;}}}_LL323: if(_tmp593 <= (void*)2)goto
_LL325;if(*((int*)_tmp593)!= 12)goto _LL325;_LL324:({void*_tmp5F4[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5F5="unknownid pat";_tag_dynforward(_tmp5F5,sizeof(char),_get_zero_arr_size(
_tmp5F5,14));}),_tag_dynforward(_tmp5F4,sizeof(void*),0));});_LL325: if(_tmp593 <= (
void*)2)goto _LL327;if(*((int*)_tmp593)!= 13)goto _LL327;_LL326:({void*_tmp5F6[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5F7="unknowncall pat";_tag_dynforward(_tmp5F7,sizeof(char),
_get_zero_arr_size(_tmp5F7,16));}),_tag_dynforward(_tmp5F6,sizeof(void*),0));});
_LL327: if(_tmp593 <= (void*)2)goto _LL300;if(*((int*)_tmp593)!= 14)goto _LL300;
_LL328:({void*_tmp5F8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5F9="exp pat";
_tag_dynforward(_tmp5F9,sizeof(char),_get_zero_arr_size(_tmp5F9,8));}),
_tag_dynforward(_tmp5F8,sizeof(void*),0));});_LL300:;}return({struct _tuple11
_tmp5FA;_tmp5FA.f1=nv;_tmp5FA.f2=decls;_tmp5FA.f3=s;_tmp5FA;});}struct _tuple14{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp5FB=_region_malloc(r,sizeof(*_tmp5FB));_tmp5FB->f1=
Cyc_Toc_fresh_label();_tmp5FB->f2=Cyc_Toc_fresh_label();_tmp5FB->f3=sc;_tmp5FB;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp5FC=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp5FD;union Cyc_Absyn_TunionInfoU_union _tmp5FE;struct
Cyc_Absyn_Tuniondecl**_tmp5FF;struct Cyc_Absyn_Tuniondecl*_tmp600;_LL32F: if(
_tmp5FC <= (void*)4)goto _LL331;if(*((int*)_tmp5FC)!= 2)goto _LL331;_tmp5FD=((
struct Cyc_Absyn_TunionType_struct*)_tmp5FC)->f1;_tmp5FE=_tmp5FD.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp5FC)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL331;_tmp5FF=(_tmp5FE.KnownTunion).f1;_tmp600=*_tmp5FF;if(!(!_tmp600->is_flat))
goto _LL331;_LL330: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp601=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp600->fields))->v;
for(0;(unsigned int)_tmp601;_tmp601=_tmp601->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp601->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL32E;}_LL331:;_LL332: goto _LL32E;_LL32E:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp602=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp603;struct Cyc_Absyn_Pat*_tmp604;struct Cyc_Absyn_Pat
_tmp605;void*_tmp606;struct Cyc_Absyn_Tuniondecl*_tmp607;struct Cyc_Absyn_Tunionfield*
_tmp608;struct Cyc_List_List*_tmp609;_LL334: if(_tmp602 <= (void*)2)goto _LL338;if(*((
int*)_tmp602)!= 0)goto _LL336;_tmp603=((struct Cyc_Absyn_Var_p_struct*)_tmp602)->f2;
_LL335: return Cyc_Toc_no_tag_test(_tmp603);_LL336: if(*((int*)_tmp602)!= 4)goto
_LL338;_tmp604=((struct Cyc_Absyn_Pointer_p_struct*)_tmp602)->f1;_tmp605=*_tmp604;
_tmp606=(void*)_tmp605.r;if(_tmp606 <= (void*)2)goto _LL338;if(*((int*)_tmp606)!= 
6)goto _LL338;_tmp607=((struct Cyc_Absyn_Tunion_p_struct*)_tmp606)->f1;_tmp608=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp606)->f2;_tmp609=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp606)->f3;if(!(_tmp609 != 0  && !_tmp607->is_flat))goto _LL338;_LL337: return 0;
_LL338:;_LL339: return 1;_LL333:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp60A=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp60B=Cyc_Tcutil_compress(_tmp60A);_LL33B: if(_tmp60B
<= (void*)4)goto _LL33F;if(*((int*)_tmp60B)!= 5)goto _LL33D;_LL33C: goto _LL33E;
_LL33D: if(*((int*)_tmp60B)!= 12)goto _LL33F;_LL33E: leave_as_switch=1;goto _LL33A;
_LL33F:;_LL340: leave_as_switch=0;goto _LL33A;_LL33A:;}{struct Cyc_List_List*
_tmp60C=scs;for(0;_tmp60C != 0;_tmp60C=_tmp60C->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp60C->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp60C->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp60D=scs;for(0;_tmp60D != 0;_tmp60D=_tmp60D->tl){struct Cyc_Absyn_Stmt*
_tmp60E=((struct Cyc_Absyn_Switch_clause*)_tmp60D->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp60D->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp60E,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp60F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp60F;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp60E);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp610=_new_region("rgn");struct _RegionHandle*rgn=& _tmp610;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp611=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp60A);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp612=lscs;for(0;_tmp612 != 0;_tmp612=_tmp612->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp612->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp612->tl
== 0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp612->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp613=_tmp612->tl;for(0;(unsigned int)
_tmp613;_tmp613=_tmp613->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple14*)_tmp613->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp614=_region_malloc(rgn,sizeof(*_tmp614));
_tmp614[0]=Cyc_Absyn_goto_stmt((*((struct _tuple14*)_tmp613->hd)).f1,0);_tmp614;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp615=_region_malloc(rgn,sizeof(*_tmp615));_tmp615[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp615;});}{struct Cyc_Toc_Env*_tmp617;struct Cyc_List_List*_tmp618;
struct Cyc_Absyn_Stmt*_tmp619;struct _tuple11 _tmp616=Cyc_Toc_xlate_pat(_tmp611,rgn,
_tmp60A,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp617=_tmp616.f1;_tmp618=_tmp616.f2;_tmp619=_tmp616.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp61A=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp617,_tmp61A);_tmp619=Cyc_Absyn_seq_stmt(_tmp619,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp61A,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp618;nvs=({struct Cyc_List_List*_tmp61B=_region_malloc(rgn,
sizeof(*_tmp61B));_tmp61B->hd=_tmp617;_tmp61B->tl=nvs;_tmp61B;});test_stmts=({
struct Cyc_List_List*_tmp61C=_region_malloc(rgn,sizeof(*_tmp61C));_tmp61C->hd=
_tmp619;_tmp61C->tl=test_stmts;_tmp61C;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple14 _tmp61E;struct _dynforward_ptr*_tmp61F;struct _dynforward_ptr*
_tmp620;struct Cyc_Absyn_Switch_clause*_tmp621;struct _tuple14*_tmp61D=(struct
_tuple14*)lscs->hd;_tmp61E=*_tmp61D;_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;
_tmp621=_tmp61E.f3;{struct Cyc_Toc_Env*_tmp622=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp621->body;{struct _RegionHandle
_tmp623=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp623;_push_region(rgn2);
if(lscs->tl != 0){struct _tuple14 _tmp625;struct _dynforward_ptr*_tmp626;struct Cyc_Absyn_Switch_clause*
_tmp627;struct _tuple14*_tmp624=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp625=*_tmp624;_tmp626=_tmp625.f2;_tmp627=_tmp625.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp622,end_l,_tmp626,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp627->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp622,end_l),s);};
_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp61F,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp620,s,0),0);stmts=({struct Cyc_List_List*_tmp628=_region_malloc(rgn,sizeof(*
_tmp628));_tmp628->hd=s;_tmp628->tl=stmts;_tmp628;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple12 _tmp62A;struct _tuple1*
_tmp62B;void*_tmp62C;struct _tuple12*_tmp629=(struct _tuple12*)decls->hd;_tmp62A=*
_tmp629;_tmp62B=_tmp62A.f1;_tmp62C=_tmp62A.f2;res=Cyc_Absyn_declare_stmt(_tmp62B,
_tmp62C,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp62D=
_cycalloc(sizeof(*_tmp62D));_tmp62D->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp62D->tl=0;_tmp62D;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp62E=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp62F;struct Cyc_Absyn_Stmt*_tmp630;struct Cyc_Absyn_Stmt*_tmp631;struct Cyc_Absyn_Exp*
_tmp632;struct Cyc_Absyn_Exp*_tmp633;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Stmt*
_tmp635;struct _tuple3 _tmp636;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Stmt*
_tmp638;struct Cyc_Absyn_Stmt*_tmp639;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Stmt*
_tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct _tuple3 _tmp63D;struct Cyc_Absyn_Exp*
_tmp63E;struct _tuple3 _tmp63F;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Stmt*
_tmp641;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_List_List*_tmp643;struct Cyc_List_List*
_tmp644;struct Cyc_Absyn_Switch_clause**_tmp645;struct Cyc_Absyn_Decl*_tmp646;
struct Cyc_Absyn_Stmt*_tmp647;struct _dynforward_ptr*_tmp648;struct Cyc_Absyn_Stmt*
_tmp649;struct Cyc_Absyn_Stmt*_tmp64A;struct _tuple3 _tmp64B;struct Cyc_Absyn_Exp*
_tmp64C;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_List_List*_tmp64E;struct Cyc_Absyn_Tvar*
_tmp64F;struct Cyc_Absyn_Vardecl*_tmp650;int _tmp651;struct Cyc_Absyn_Exp*_tmp652;
struct Cyc_Absyn_Stmt*_tmp653;struct Cyc_Absyn_Exp*_tmp654;_LL342: if((int)_tmp62E
!= 0)goto _LL344;_LL343: return;_LL344: if(_tmp62E <= (void*)1)goto _LL346;if(*((int*)
_tmp62E)!= 0)goto _LL346;_tmp62F=((struct Cyc_Absyn_Exp_s_struct*)_tmp62E)->f1;
_LL345: Cyc_Toc_exp_to_c(nv,_tmp62F);return;_LL346: if(_tmp62E <= (void*)1)goto
_LL348;if(*((int*)_tmp62E)!= 1)goto _LL348;_tmp630=((struct Cyc_Absyn_Seq_s_struct*)
_tmp62E)->f1;_tmp631=((struct Cyc_Absyn_Seq_s_struct*)_tmp62E)->f2;_LL347: Cyc_Toc_stmt_to_c(
nv,_tmp630);s=_tmp631;continue;_LL348: if(_tmp62E <= (void*)1)goto _LL34A;if(*((int*)
_tmp62E)!= 2)goto _LL34A;_tmp632=((struct Cyc_Absyn_Return_s_struct*)_tmp62E)->f1;
_LL349: {struct Cyc_Core_Opt*topt=0;if(_tmp632 != 0){topt=({struct Cyc_Core_Opt*
_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp632->topt))->v);_tmp655;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp632);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp656=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp657=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp656,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp656,(void*)topt->v,_tmp632,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp657,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL34A: if(_tmp62E <= (void*)1)goto _LL34C;if(*((int*)_tmp62E)!= 3)goto
_LL34C;_tmp633=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp62E)->f1;_tmp634=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp62E)->f2;_tmp635=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp62E)->f3;_LL34B: Cyc_Toc_exp_to_c(nv,_tmp633);Cyc_Toc_stmt_to_c(nv,_tmp634);s=
_tmp635;continue;_LL34C: if(_tmp62E <= (void*)1)goto _LL34E;if(*((int*)_tmp62E)!= 4)
goto _LL34E;_tmp636=((struct Cyc_Absyn_While_s_struct*)_tmp62E)->f1;_tmp637=
_tmp636.f1;_tmp638=((struct Cyc_Absyn_While_s_struct*)_tmp62E)->f2;_LL34D: Cyc_Toc_exp_to_c(
nv,_tmp637);{struct _RegionHandle _tmp658=_new_region("temp");struct _RegionHandle*
temp=& _tmp658;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp638);;_pop_region(temp);}return;_LL34E: if(_tmp62E <= (void*)1)goto _LL350;if(*((
int*)_tmp62E)!= 5)goto _LL350;_tmp639=((struct Cyc_Absyn_Break_s_struct*)_tmp62E)->f1;
_LL34F: {struct Cyc_Toc_Env _tmp65A;struct _dynforward_ptr**_tmp65B;struct Cyc_Toc_Env*
_tmp659=nv;_tmp65A=*_tmp659;_tmp65B=_tmp65A.break_lab;if(_tmp65B != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp65B,0));{int dest_depth=_tmp639 == 0?0: _tmp639->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL350: if(_tmp62E <= (
void*)1)goto _LL352;if(*((int*)_tmp62E)!= 6)goto _LL352;_tmp63A=((struct Cyc_Absyn_Continue_s_struct*)
_tmp62E)->f1;_LL351: {struct Cyc_Toc_Env _tmp65D;struct _dynforward_ptr**_tmp65E;
struct Cyc_Toc_Env*_tmp65C=nv;_tmp65D=*_tmp65C;_tmp65E=_tmp65D.continue_lab;if(
_tmp65E != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp65E,0));_tmp63B=_tmp63A;
goto _LL353;}_LL352: if(_tmp62E <= (void*)1)goto _LL354;if(*((int*)_tmp62E)!= 7)goto
_LL354;_tmp63B=((struct Cyc_Absyn_Goto_s_struct*)_tmp62E)->f2;_LL353: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp63B))->try_depth,s);
return;_LL354: if(_tmp62E <= (void*)1)goto _LL356;if(*((int*)_tmp62E)!= 8)goto
_LL356;_tmp63C=((struct Cyc_Absyn_For_s_struct*)_tmp62E)->f1;_tmp63D=((struct Cyc_Absyn_For_s_struct*)
_tmp62E)->f2;_tmp63E=_tmp63D.f1;_tmp63F=((struct Cyc_Absyn_For_s_struct*)_tmp62E)->f3;
_tmp640=_tmp63F.f1;_tmp641=((struct Cyc_Absyn_For_s_struct*)_tmp62E)->f4;_LL355:
Cyc_Toc_exp_to_c(nv,_tmp63C);Cyc_Toc_exp_to_c(nv,_tmp63E);Cyc_Toc_exp_to_c(nv,
_tmp640);{struct _RegionHandle _tmp65F=_new_region("temp");struct _RegionHandle*
temp=& _tmp65F;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp641);;_pop_region(temp);}return;_LL356: if(_tmp62E <= (void*)1)goto _LL358;if(*((
int*)_tmp62E)!= 9)goto _LL358;_tmp642=((struct Cyc_Absyn_Switch_s_struct*)_tmp62E)->f1;
_tmp643=((struct Cyc_Absyn_Switch_s_struct*)_tmp62E)->f2;_LL357: Cyc_Toc_xlate_switch(
nv,s,_tmp642,_tmp643);return;_LL358: if(_tmp62E <= (void*)1)goto _LL35A;if(*((int*)
_tmp62E)!= 10)goto _LL35A;_tmp644=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp62E)->f1;
_tmp645=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp62E)->f2;_LL359: {struct Cyc_Toc_Env
_tmp661;struct Cyc_Toc_FallthruInfo*_tmp662;struct Cyc_Toc_Env*_tmp660=nv;_tmp661=*
_tmp660;_tmp662=_tmp661.fallthru_info;if(_tmp662 == 0)({void*_tmp663[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp664="fallthru in unexpected place";_tag_dynforward(_tmp664,sizeof(char),
_get_zero_arr_size(_tmp664,29));}),_tag_dynforward(_tmp663,sizeof(void*),0));});{
struct _dynforward_ptr*_tmp666;struct Cyc_List_List*_tmp667;struct Cyc_Dict_Dict
_tmp668;struct Cyc_Toc_FallthruInfo _tmp665=*_tmp662;_tmp666=_tmp665.label;_tmp667=
_tmp665.binders;_tmp668=_tmp665.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp666,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp645)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp669=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp667);struct Cyc_List_List*
_tmp66A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp644);
for(0;_tmp669 != 0;(_tmp669=_tmp669->tl,_tmp66A=_tmp66A->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66A))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp668,(struct _tuple1*)_tmp669->hd),(struct Cyc_Absyn_Exp*)
_tmp66A->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}}_LL35A: if(
_tmp62E <= (void*)1)goto _LL35C;if(*((int*)_tmp62E)!= 11)goto _LL35C;_tmp646=((
struct Cyc_Absyn_Decl_s_struct*)_tmp62E)->f1;_tmp647=((struct Cyc_Absyn_Decl_s_struct*)
_tmp62E)->f2;_LL35B:{void*_tmp66B=(void*)_tmp646->r;struct Cyc_Absyn_Vardecl*
_tmp66C;struct Cyc_Absyn_Pat*_tmp66D;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_List_List*
_tmp66F;struct Cyc_Absyn_Fndecl*_tmp670;_LL367: if(_tmp66B <= (void*)2)goto _LL36F;
if(*((int*)_tmp66B)!= 0)goto _LL369;_tmp66C=((struct Cyc_Absyn_Var_d_struct*)
_tmp66B)->f1;_LL368:{struct _RegionHandle _tmp671=_new_region("temp");struct
_RegionHandle*temp=& _tmp671;_push_region(temp);{struct Cyc_Toc_Env*_tmp672=Cyc_Toc_add_varmap(
temp,nv,_tmp66C->name,Cyc_Absyn_varb_exp(_tmp66C->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Absyn_Local_b_struct
_tmp674;_tmp674.tag=3;_tmp674.f1=_tmp66C;_tmp674;});_tmp673;}),0));Cyc_Toc_local_decl_to_c(
_tmp672,_tmp672,_tmp66C,_tmp647);};_pop_region(temp);}goto _LL366;_LL369: if(*((
int*)_tmp66B)!= 2)goto _LL36B;_tmp66D=((struct Cyc_Absyn_Let_d_struct*)_tmp66B)->f1;
_tmp66E=((struct Cyc_Absyn_Let_d_struct*)_tmp66B)->f3;_LL36A:{void*_tmp675=(void*)
_tmp66D->r;struct Cyc_Absyn_Vardecl*_tmp676;struct Cyc_Absyn_Pat*_tmp677;struct Cyc_Absyn_Pat
_tmp678;void*_tmp679;_LL372: if(_tmp675 <= (void*)2)goto _LL374;if(*((int*)_tmp675)
!= 0)goto _LL374;_tmp676=((struct Cyc_Absyn_Var_p_struct*)_tmp675)->f1;_tmp677=((
struct Cyc_Absyn_Var_p_struct*)_tmp675)->f2;_tmp678=*_tmp677;_tmp679=(void*)
_tmp678.r;if((int)_tmp679 != 0)goto _LL374;_LL373: {struct _tuple1*old_name=_tmp676->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp676->name=new_name;_tmp676->initializer=(
struct Cyc_Absyn_Exp*)_tmp66E;(void*)(_tmp646->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({struct Cyc_Absyn_Var_d_struct
_tmp67B;_tmp67B.tag=0;_tmp67B.f1=_tmp676;_tmp67B;});_tmp67A;})));{struct
_RegionHandle _tmp67C=_new_region("temp");struct _RegionHandle*temp=& _tmp67C;
_push_region(temp);{struct Cyc_Toc_Env*_tmp67D=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E[0]=({struct Cyc_Absyn_Local_b_struct
_tmp67F;_tmp67F.tag=3;_tmp67F.f1=_tmp676;_tmp67F;});_tmp67E;}),0));Cyc_Toc_local_decl_to_c(
_tmp67D,nv,_tmp676,_tmp647);};_pop_region(temp);}goto _LL371;}_LL374:;_LL375:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp66D,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp66E->topt))->v,_tmp66E,_tmp647))->r));goto _LL371;_LL371:;}goto
_LL366;_LL36B: if(*((int*)_tmp66B)!= 3)goto _LL36D;_tmp66F=((struct Cyc_Absyn_Letv_d_struct*)
_tmp66B)->f1;_LL36C: {struct Cyc_List_List*_tmp680=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp66F);if(_tmp680 == 0)({void*_tmp681[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp682="empty Letv_d";_tag_dynforward(_tmp682,sizeof(char),
_get_zero_arr_size(_tmp682,13));}),_tag_dynforward(_tmp681,sizeof(void*),0));});(
void*)(_tmp646->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp683=
_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_Var_d_struct _tmp684;
_tmp684.tag=0;_tmp684.f1=(struct Cyc_Absyn_Vardecl*)_tmp680->hd;_tmp684;});
_tmp683;})));_tmp680=_tmp680->tl;for(0;_tmp680 != 0;_tmp680=_tmp680->tl){struct
Cyc_Absyn_Decl*_tmp685=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686[0]=({struct Cyc_Absyn_Var_d_struct
_tmp687;_tmp687.tag=0;_tmp687.f1=(struct Cyc_Absyn_Vardecl*)_tmp680->hd;_tmp687;});
_tmp686;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp685,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL366;}_LL36D: if(*((int*)
_tmp66B)!= 1)goto _LL36F;_tmp670=((struct Cyc_Absyn_Fn_d_struct*)_tmp66B)->f1;
_LL36E: {struct _tuple1*_tmp688=_tmp670->name;{struct _RegionHandle _tmp689=
_new_region("temp");struct _RegionHandle*temp=& _tmp689;_push_region(temp);{struct
Cyc_Toc_Env*_tmp68A=Cyc_Toc_add_varmap(temp,nv,_tmp670->name,Cyc_Absyn_var_exp(
_tmp688,0));Cyc_Toc_fndecl_to_c(_tmp68A,_tmp670,0);Cyc_Toc_stmt_to_c(_tmp68A,
_tmp647);};_pop_region(temp);}goto _LL366;}_LL36F:;_LL370:({void*_tmp68B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp68C="bad nested declaration within function";_tag_dynforward(
_tmp68C,sizeof(char),_get_zero_arr_size(_tmp68C,39));}),_tag_dynforward(_tmp68B,
sizeof(void*),0));});_LL366:;}return;_LL35C: if(_tmp62E <= (void*)1)goto _LL35E;if(*((
int*)_tmp62E)!= 12)goto _LL35E;_tmp648=((struct Cyc_Absyn_Label_s_struct*)_tmp62E)->f1;
_tmp649=((struct Cyc_Absyn_Label_s_struct*)_tmp62E)->f2;_LL35D: s=_tmp649;
continue;_LL35E: if(_tmp62E <= (void*)1)goto _LL360;if(*((int*)_tmp62E)!= 13)goto
_LL360;_tmp64A=((struct Cyc_Absyn_Do_s_struct*)_tmp62E)->f1;_tmp64B=((struct Cyc_Absyn_Do_s_struct*)
_tmp62E)->f2;_tmp64C=_tmp64B.f1;_LL35F:{struct _RegionHandle _tmp68D=_new_region("temp");
struct _RegionHandle*temp=& _tmp68D;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp64A);Cyc_Toc_exp_to_c(nv,_tmp64C);;_pop_region(temp);}return;_LL360:
if(_tmp62E <= (void*)1)goto _LL362;if(*((int*)_tmp62E)!= 14)goto _LL362;_tmp64D=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp62E)->f1;_tmp64E=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp62E)->f2;_LL361: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->v=(
void*)e_typ;_tmp68E;});{struct _RegionHandle _tmp68F=_new_region("temp");struct
_RegionHandle*temp=& _tmp68F;_push_region(temp);{struct Cyc_Toc_Env*_tmp690=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp690,_tmp64D);{struct Cyc_Absyn_Stmt*
_tmp691=Cyc_Absyn_seq_stmt(_tmp64D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp692=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp693=
Cyc_Absyn_var_exp(_tmp692,0);struct Cyc_Absyn_Vardecl*_tmp694=Cyc_Absyn_new_vardecl(
_tmp692,Cyc_Absyn_exn_typ,0);_tmp693->topt=({struct Cyc_Core_Opt*_tmp695=
_cycalloc(sizeof(*_tmp695));_tmp695->v=(void*)Cyc_Absyn_exn_typ;_tmp695;});{
struct Cyc_Absyn_Pat*_tmp696=({struct Cyc_Absyn_Pat*_tmp6A6=_cycalloc(sizeof(*
_tmp6A6));_tmp6A6->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp6A8=
_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_Var_p_struct _tmp6A9;
_tmp6A9.tag=0;_tmp6A9.f1=_tmp694;_tmp6A9.f2=({struct Cyc_Absyn_Pat*_tmp6AA=
_cycalloc(sizeof(*_tmp6AA));_tmp6AA->r=(void*)((void*)0);_tmp6AA->topt=({struct
Cyc_Core_Opt*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->v=(void*)Cyc_Absyn_exn_typ;
_tmp6AB;});_tmp6AA->loc=0;_tmp6AA;});_tmp6A9;});_tmp6A8;}));_tmp6A6->topt=({
struct Cyc_Core_Opt*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->v=(void*)Cyc_Absyn_exn_typ;
_tmp6A7;});_tmp6A6->loc=0;_tmp6A6;});struct Cyc_Absyn_Exp*_tmp697=Cyc_Absyn_throw_exp(
_tmp693,0);_tmp697->topt=({struct Cyc_Core_Opt*_tmp698=_cycalloc(sizeof(*_tmp698));
_tmp698->v=(void*)((void*)0);_tmp698;});{struct Cyc_Absyn_Stmt*_tmp699=Cyc_Absyn_exp_stmt(
_tmp697,0);struct Cyc_Absyn_Switch_clause*_tmp69A=({struct Cyc_Absyn_Switch_clause*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->pattern=_tmp696;_tmp6A3->pat_vars=({
struct Cyc_Core_Opt*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->v=({struct Cyc_List_List*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=_tmp694;_tmp6A5->tl=0;_tmp6A5;});
_tmp6A4;});_tmp6A3->where_clause=0;_tmp6A3->body=_tmp699;_tmp6A3->loc=0;_tmp6A3;});
struct Cyc_Absyn_Stmt*_tmp69B=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp64E,({struct
Cyc_List_List*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->hd=_tmp69A;_tmp6A2->tl=
0;_tmp6A2;})),0);Cyc_Toc_stmt_to_c(_tmp690,_tmp69B);{struct Cyc_Absyn_Exp*_tmp69C=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp6A1=_cycalloc(
sizeof(*_tmp6A1));_tmp6A1->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp6A1->tl=0;_tmp6A1;}),0);struct Cyc_Absyn_Stmt*_tmp69D=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp6A0=
_cycalloc(sizeof(*_tmp6A0));_tmp6A0->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp6A0->tl=
0;_tmp6A0;}),0),0);struct Cyc_Absyn_Exp*_tmp69E=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp69F=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp69D,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp69E,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp69C,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp69F,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp691,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp69B,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL362: if(_tmp62E <= (void*)1)goto _LL364;if(*((int*)
_tmp62E)!= 15)goto _LL364;_tmp64F=((struct Cyc_Absyn_Region_s_struct*)_tmp62E)->f1;
_tmp650=((struct Cyc_Absyn_Region_s_struct*)_tmp62E)->f2;_tmp651=((struct Cyc_Absyn_Region_s_struct*)
_tmp62E)->f3;_tmp652=((struct Cyc_Absyn_Region_s_struct*)_tmp62E)->f4;_tmp653=((
struct Cyc_Absyn_Region_s_struct*)_tmp62E)->f5;_LL363: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp650->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);{struct _RegionHandle
_tmp6AC=_new_region("temp");struct _RegionHandle*temp=& _tmp6AC;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp653);;_pop_region(
temp);}if(_tmp652 == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,
rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp6AD->tl=0;_tmp6AD;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp6AE[1];_tmp6AE[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6AE,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp653,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp6AF[1];_tmp6AF[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6AF,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp652);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6B0[2];_tmp6B0[1]=(struct Cyc_Absyn_Exp*)_tmp652;_tmp6B0[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B0,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp653,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6B1[1];_tmp6B1[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B1,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}return;}_LL364: if(_tmp62E <= (void*)1)goto _LL341;if(*((
int*)_tmp62E)!= 16)goto _LL341;_tmp654=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp62E)->f1;_LL365: Cyc_Toc_exp_to_c(nv,_tmp654);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp6B2=
_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=_tmp654;_tmp6B2->tl=0;_tmp6B2;}),0)));
return;_LL341:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct
_tuple15{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void
Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=
0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct _RegionHandle _tmp6B3=_new_region("frgn");struct _RegionHandle*frgn=& _tmp6B3;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp6B4=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp6B5=f->args;for(0;_tmp6B5 != 0;_tmp6B5=_tmp6B5->tl){struct
_tuple1*_tmp6B6=({struct _tuple1*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->f1=(
union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp6B8;(_tmp6B8.Loc_n).tag=0;_tmp6B8;}));_tmp6B7->f2=(*((struct _tuple15*)
_tmp6B5->hd)).f1;_tmp6B7;});(*((struct _tuple15*)_tmp6B5->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple15*)_tmp6B5->hd)).f3);_tmp6B4=Cyc_Toc_add_varmap(frgn,_tmp6B4,
_tmp6B6,Cyc_Absyn_var_exp(_tmp6B6,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp6BA;struct Cyc_Absyn_Tqual
_tmp6BB;void*_tmp6BC;int _tmp6BD;struct Cyc_Absyn_VarargInfo _tmp6B9=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp6BA=_tmp6B9.name;_tmp6BB=_tmp6B9.tq;_tmp6BC=(
void*)_tmp6B9.type;_tmp6BD=_tmp6B9.inject;{void*_tmp6BE=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp6BC,(void*)2,_tmp6BB,Cyc_Absyn_false_conref));struct _tuple1*_tmp6BF=({struct
_tuple1*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp6C3;(_tmp6C3.Loc_n).tag=
0;_tmp6C3;}));_tmp6C2->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp6BA))->v;_tmp6C2;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp6C0=
_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=({struct _tuple15*_tmp6C1=_cycalloc(
sizeof(*_tmp6C1));_tmp6C1->f1=(struct _dynforward_ptr*)_tmp6BA->v;_tmp6C1->f2=
_tmp6BB;_tmp6C1->f3=_tmp6BE;_tmp6C1;});_tmp6C0->tl=0;_tmp6C0;}));_tmp6B4=Cyc_Toc_add_varmap(
frgn,_tmp6B4,_tmp6BF,Cyc_Absyn_var_exp(_tmp6BF,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp6C4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp6C4 != 0;_tmp6C4=_tmp6C4->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp6C4->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp6C4->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp6B4),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp6C5=s;_LL377:
if((int)_tmp6C5 != 1)goto _LL379;_LL378: return(void*)2;_LL379: if((int)_tmp6C5 != 4)
goto _LL37B;_LL37A: return(void*)3;_LL37B:;_LL37C: return s;_LL376:;}struct Cyc_Dict_Dict*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp6C6=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Dict_Dict*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp6C7;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp6C8=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far)),_tmp6C6);if(_tmp6C8 == 0){seen_defn_before=0;*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_aggrs_so_far))=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_aggrs_so_far)),_tmp6C6,ad);}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp6C8->v)->impl == 0){*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far)),_tmp6C6,ad);seen_defn_before=0;}else{
seen_defn_before=1;}}(void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp6C9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp6C9 != 0;_tmp6C9=_tmp6C9->tl){((struct Cyc_Absyn_Aggrfield*)_tmp6C9->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp6C9->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp6C9->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp6CA;});{
struct _tuple1*_tmp6CB=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp6CB))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp6CB));{struct Cyc_List_List*
flat_structs=0;{struct Cyc_List_List*_tmp6CC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp6CC != 0;_tmp6CC=_tmp6CC->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp6CC->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp6CD=0;int i=1;{struct Cyc_List_List*_tmp6CE=f->typs;for(0;
_tmp6CE != 0;(_tmp6CE=_tmp6CE->tl,i ++)){struct _dynforward_ptr*_tmp6CF=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp6D0=({struct Cyc_Absyn_Aggrfield*_tmp6D2=
_cycalloc(sizeof(*_tmp6D2));_tmp6D2->name=_tmp6CF;_tmp6D2->tq=(*((struct _tuple4*)
_tmp6CE->hd)).f1;_tmp6D2->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp6CE->hd)).f2);_tmp6D2->width=0;_tmp6D2->attributes=0;_tmp6D2;});_tmp6CD=({
struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=_tmp6D0;
_tmp6D1->tl=_tmp6CD;_tmp6D1;});}}_tmp6CD=({struct Cyc_List_List*_tmp6D3=_cycalloc(
sizeof(*_tmp6D3));_tmp6D3->hd=({struct Cyc_Absyn_Aggrfield*_tmp6D4=_cycalloc(
sizeof(*_tmp6D4));_tmp6D4->name=Cyc_Toc_tag_sp;_tmp6D4->tq=Cyc_Toc_mt_tq;_tmp6D4->type=(
void*)Cyc_Absyn_sint_typ;_tmp6D4->width=0;_tmp6D4->attributes=0;_tmp6D4;});
_tmp6D3->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6CD);_tmp6D3;});{struct Cyc_Absyn_Aggrdecl*_tmp6D5=({struct Cyc_Absyn_Aggrdecl*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1->kind=(void*)((void*)0);_tmp6E1->sc=(
void*)((void*)2);_tmp6E1->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp6E3="_struct";_tag_dynforward(_tmp6E3,sizeof(char),_get_zero_arr_size(
_tmp6E3,8));}));_tmp6E1->tvs=0;_tmp6E1->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->exist_vars=0;_tmp6E2->rgn_po=0;
_tmp6E2->fields=_tmp6CD;_tmp6E2;});_tmp6E1->attributes=0;_tmp6E1;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));
_tmp6D7[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6D8;_tmp6D8.tag=4;_tmp6D8.f1=
_tmp6D5;_tmp6D8;});_tmp6D7;}),0);_tmp6D6->tl=Cyc_Toc_result_decls;_tmp6D6;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp6D9=({struct Cyc_Absyn_Aggrfield*
_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB->name=(*f->name).f2;_tmp6DB->tq=Cyc_Toc_mt_tq;
_tmp6DB->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp6DC=_cycalloc(
sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_Absyn_AggrType_struct _tmp6DD;_tmp6DD.tag=
10;_tmp6DD.f1=({struct Cyc_Absyn_AggrInfo _tmp6DE;_tmp6DE.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp6DF;(_tmp6DF.KnownAggr).tag=1;(_tmp6DF.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=_tmp6D5;
_tmp6E0;});_tmp6DF;});_tmp6DE.targs=0;_tmp6DE;});_tmp6DD;});_tmp6DC;}));_tmp6DB->width=
0;_tmp6DB->attributes=0;_tmp6DB;});flat_structs=({struct Cyc_List_List*_tmp6DA=
_cycalloc(sizeof(*_tmp6DA));_tmp6DA->hd=_tmp6D9;_tmp6DA->tl=flat_structs;_tmp6DA;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp6E4=({struct Cyc_Absyn_Aggrdecl*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->kind=(void*)((void*)1);_tmp6E8->sc=(
void*)((void*)2);_tmp6E8->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp6EA="_union";_tag_dynforward(_tmp6EA,sizeof(char),_get_zero_arr_size(_tmp6EA,
7));}));_tmp6E8->tvs=0;_tmp6E8->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp6E9=
_cycalloc(sizeof(*_tmp6E9));_tmp6E9->exist_vars=0;_tmp6E9->rgn_po=0;_tmp6E9->fields=
flat_structs;_tmp6E9;});_tmp6E8->attributes=0;_tmp6E8;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));
_tmp6E6[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6E7;_tmp6E7.tag=4;_tmp6E7.f1=
_tmp6E4;_tmp6E7;});_tmp6E6;}),0);_tmp6E5->tl=Cyc_Toc_result_decls;_tmp6E5;});}}}}}
static struct Cyc_Dict_Dict*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Dict_Dict*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp6EB;});if(xd->fields == 0)return;{struct _tuple1*_tmp6EC=xd->name;struct Cyc_List_List*
_tmp6ED=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp6ED != 0;_tmp6ED=_tmp6ED->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp6ED->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp6EE=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp6EF=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp6EE,Cyc_Absyn_false_conref,
0);struct Cyc_Core_Opt*_tmp6F0=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict d,
struct _tuple1*k))Cyc_Dict_lookup_opt)(*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),
f->name);struct Cyc_Core_Opt _tmp6F1;int _tmp6F2;_LL37E: if(_tmp6F0 != 0)goto _LL380;
_LL37F: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp6F9;_tmp6F9.tag=0;_tmp6F9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp6F8;
_tmp6F8.tag=1;_tmp6F8.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp6F7;_tmp6F7.tag=1;_tmp6F7.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp6F6;_tmp6F6.tag=1;_tmp6F6.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp6F5;_tmp6F5.tag=1;_tmp6F5.f1=(unsigned long)((int)zero);{void*_tmp6F3[5]={&
_tmp6F5,& _tmp6F6,& _tmp6F7,& _tmp6F8,& _tmp6F9};Cyc_aprintf(({const char*_tmp6F4="%c%c%c%c%s";
_tag_dynforward(_tmp6F4,sizeof(char),_get_zero_arr_size(_tmp6F4,11));}),
_tag_dynforward(_tmp6F3,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*
_tmp6FA=Cyc_Absyn_new_vardecl(f->name,_tmp6EF,initopt);(void*)(_tmp6FA->sc=(void*)((
void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6FB=_cycalloc(
sizeof(*_tmp6FB));_tmp6FB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_Var_d_struct
_tmp6FD;_tmp6FD.tag=0;_tmp6FD.f1=_tmp6FA;_tmp6FD;});_tmp6FC;}),0);_tmp6FB->tl=
Cyc_Toc_result_decls;_tmp6FB;});*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far))=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),f->name,(void*)
f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp6FE=f->typs;for(0;_tmp6FE != 0;(_tmp6FE=_tmp6FE->tl,i ++)){struct
_dynforward_ptr*_tmp6FF=({struct _dynforward_ptr*_tmp703=_cycalloc(sizeof(*
_tmp703));_tmp703[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp706;
_tmp706.tag=1;_tmp706.f1=(unsigned long)i;{void*_tmp704[1]={& _tmp706};Cyc_aprintf(({
const char*_tmp705="f%d";_tag_dynforward(_tmp705,sizeof(char),_get_zero_arr_size(
_tmp705,4));}),_tag_dynforward(_tmp704,sizeof(void*),1));}});_tmp703;});struct
Cyc_Absyn_Aggrfield*_tmp700=({struct Cyc_Absyn_Aggrfield*_tmp702=_cycalloc(
sizeof(*_tmp702));_tmp702->name=_tmp6FF;_tmp702->tq=(*((struct _tuple4*)_tmp6FE->hd)).f1;
_tmp702->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp6FE->hd)).f2);
_tmp702->width=0;_tmp702->attributes=0;_tmp702;});fields=({struct Cyc_List_List*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701->hd=_tmp700;_tmp701->tl=fields;
_tmp701;});}}fields=({struct Cyc_List_List*_tmp707=_cycalloc(sizeof(*_tmp707));
_tmp707->hd=({struct Cyc_Absyn_Aggrfield*_tmp708=_cycalloc(sizeof(*_tmp708));
_tmp708->name=Cyc_Toc_tag_sp;_tmp708->tq=Cyc_Toc_mt_tq;_tmp708->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp708->width=0;_tmp708->attributes=0;_tmp708;});
_tmp707->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp707;});{struct Cyc_Absyn_Aggrdecl*_tmp709=({struct Cyc_Absyn_Aggrdecl*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D->kind=(void*)((void*)0);_tmp70D->sc=(
void*)((void*)2);_tmp70D->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp70F="_struct";_tag_dynforward(_tmp70F,sizeof(char),_get_zero_arr_size(
_tmp70F,8));}));_tmp70D->tvs=0;_tmp70D->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->exist_vars=0;_tmp70E->rgn_po=0;
_tmp70E->fields=fields;_tmp70E;});_tmp70D->attributes=0;_tmp70D;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp70B=_cycalloc(sizeof(*_tmp70B));
_tmp70B[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp70C;_tmp70C.tag=4;_tmp70C.f1=
_tmp709;_tmp70C;});_tmp70B;}),0);_tmp70A->tl=Cyc_Toc_result_decls;_tmp70A;});}}
goto _LL37D;}}_LL380: if(_tmp6F0 == 0)goto _LL382;_tmp6F1=*_tmp6F0;_tmp6F2=(int)
_tmp6F1.v;if(_tmp6F2 != 0)goto _LL382;_LL381: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp710=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp71B;_tmp71B.tag=0;_tmp71B.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp71A;_tmp71A.tag=1;_tmp71A.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp719;_tmp719.tag=1;_tmp719.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp718;_tmp718.tag=1;_tmp718.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp717;_tmp717.tag=1;_tmp717.f1=(
unsigned long)((int)zero);{void*_tmp715[5]={& _tmp717,& _tmp718,& _tmp719,& _tmp71A,&
_tmp71B};Cyc_aprintf(({const char*_tmp716="%c%c%c%c%s";_tag_dynforward(_tmp716,
sizeof(char),_get_zero_arr_size(_tmp716,11));}),_tag_dynforward(_tmp715,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp711=Cyc_Absyn_new_vardecl(f->name,
_tmp6EF,(struct Cyc_Absyn_Exp*)_tmp710);(void*)(_tmp711->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp712=_cycalloc(sizeof(*_tmp712));
_tmp712->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp713=
_cycalloc(sizeof(*_tmp713));_tmp713[0]=({struct Cyc_Absyn_Var_d_struct _tmp714;
_tmp714.tag=0;_tmp714.f1=_tmp711;_tmp714;});_tmp713;}),0);_tmp712->tl=Cyc_Toc_result_decls;
_tmp712;});*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far))=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),f->name,1);}
goto _LL37D;_LL382:;_LL383: goto _LL37D;_LL37D:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp71C=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp71D;struct Cyc_Absyn_Exp*_tmp71E;struct Cyc_Absyn_Exp*_tmp71F;int _tmp720;
_LL385: if(*((int*)_tmp71C)!= 29)goto _LL387;_tmp71D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp71C)->f1;_tmp71E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp71C)->f2;
_tmp71F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp71C)->f3;_tmp720=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp71C)->f4;_LL386: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp71D,_tmp71E,_tmp71F,_tmp720,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL384;_LL387:;_LL388: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp721=_new_region("temp");struct _RegionHandle*temp=& _tmp721;_push_region(temp);{
struct Cyc_Toc_Env*_tmp722=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp722,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL384;_LL384:;}else{void*_tmp723=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp724;void*_tmp725;struct Cyc_Absyn_Exp*_tmp726;struct Cyc_Absyn_Conref*_tmp727;
_LL38A: if(_tmp723 <= (void*)4)goto _LL38C;if(*((int*)_tmp723)!= 7)goto _LL38C;
_tmp724=((struct Cyc_Absyn_ArrayType_struct*)_tmp723)->f1;_tmp725=(void*)_tmp724.elt_type;
_tmp726=_tmp724.num_elts;_tmp727=_tmp724.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp727))goto _LL38C;_LL38B: if(_tmp726 == 0)({void*
_tmp728[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp729="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp729,sizeof(char),_get_zero_arr_size(_tmp729,55));}),
_tag_dynforward(_tmp728,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp726;struct Cyc_Absyn_Exp*_tmp72A=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp72B=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp72A,_tmp72B,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL389;}_LL38C:;_LL38D: goto _LL389;
_LL389:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp72C=_cycalloc(
sizeof(*_tmp72C));_tmp72C[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp72C;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp72D=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp72D;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp72E=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp72F=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp731;struct Cyc_List_List*_tmp732;struct Cyc_Absyn_Stmt*
_tmp733;struct _tuple11 _tmp730=Cyc_Toc_xlate_pat(_tmp72F,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp72E,Cyc_Toc_throw_match_stmt(),
0);_tmp731=_tmp730.f1;_tmp732=_tmp730.f2;_tmp733=_tmp730.f3;Cyc_Toc_stmt_to_c(
_tmp731,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp733,s,0),0);for(0;_tmp732 != 0;_tmp732=_tmp732->tl){
struct _tuple12 _tmp735;struct _tuple1*_tmp736;void*_tmp737;struct _tuple12*_tmp734=(
struct _tuple12*)_tmp732->hd;_tmp735=*_tmp734;_tmp736=_tmp735.f1;_tmp737=_tmp735.f2;
s=Cyc_Absyn_declare_stmt(_tmp736,_tmp737,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp738=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp739;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Exp*
_tmp73B;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Exp*_tmp73D;struct Cyc_Absyn_Exp*
_tmp73E;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_List_List*
_tmp741;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_Absyn_Exp*
_tmp744;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Exp*_tmp746;struct Cyc_Absyn_Exp*
_tmp747;struct Cyc_Absyn_Exp*_tmp748;struct Cyc_Absyn_Exp*_tmp749;struct Cyc_Absyn_Exp*
_tmp74A;struct Cyc_Absyn_Exp*_tmp74B;struct Cyc_Absyn_Exp*_tmp74C;struct Cyc_Absyn_Exp*
_tmp74D;struct Cyc_Absyn_Exp*_tmp74E;struct Cyc_Absyn_Exp*_tmp74F;struct Cyc_Absyn_Exp*
_tmp750;struct Cyc_Absyn_Exp*_tmp751;struct Cyc_List_List*_tmp752;struct Cyc_Absyn_Exp*
_tmp753;struct Cyc_List_List*_tmp754;void*_tmp755;void**_tmp756;struct Cyc_Absyn_Exp*
_tmp757;struct _tuple2*_tmp758;struct _tuple2 _tmp759;void*_tmp75A;void**_tmp75B;
struct Cyc_List_List*_tmp75C;struct Cyc_List_List*_tmp75D;struct Cyc_List_List*
_tmp75E;void*_tmp75F;void**_tmp760;void*_tmp761;void**_tmp762;struct Cyc_Absyn_Stmt*
_tmp763;struct Cyc_Absyn_MallocInfo _tmp764;struct Cyc_Absyn_MallocInfo*_tmp765;
_LL38F: if(*((int*)_tmp738)!= 22)goto _LL391;_tmp739=((struct Cyc_Absyn_Deref_e_struct*)
_tmp738)->f1;_LL390: _tmp73A=_tmp739;goto _LL392;_LL391: if(*((int*)_tmp738)!= 23)
goto _LL393;_tmp73A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp738)->f1;_LL392:
_tmp73B=_tmp73A;goto _LL394;_LL393: if(*((int*)_tmp738)!= 24)goto _LL395;_tmp73B=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp738)->f1;_LL394: _tmp73C=_tmp73B;goto
_LL396;_LL395: if(*((int*)_tmp738)!= 16)goto _LL397;_tmp73C=((struct Cyc_Absyn_Address_e_struct*)
_tmp738)->f1;_LL396: _tmp73D=_tmp73C;goto _LL398;_LL397: if(*((int*)_tmp738)!= 12)
goto _LL399;_tmp73D=((struct Cyc_Absyn_Throw_e_struct*)_tmp738)->f1;_LL398: _tmp73E=
_tmp73D;goto _LL39A;_LL399: if(*((int*)_tmp738)!= 13)goto _LL39B;_tmp73E=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp738)->f1;_LL39A: _tmp73F=_tmp73E;goto _LL39C;
_LL39B: if(*((int*)_tmp738)!= 19)goto _LL39D;_tmp73F=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp738)->f1;_LL39C: _tmp740=_tmp73F;goto _LL39E;_LL39D: if(*((int*)_tmp738)!= 5)
goto _LL39F;_tmp740=((struct Cyc_Absyn_Increment_e_struct*)_tmp738)->f1;_LL39E: Cyc_Toc_exptypes_to_c(
_tmp740);goto _LL38E;_LL39F: if(*((int*)_tmp738)!= 3)goto _LL3A1;_tmp741=((struct
Cyc_Absyn_Primop_e_struct*)_tmp738)->f2;_LL3A0:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp741);goto _LL38E;
_LL3A1: if(*((int*)_tmp738)!= 7)goto _LL3A3;_tmp742=((struct Cyc_Absyn_And_e_struct*)
_tmp738)->f1;_tmp743=((struct Cyc_Absyn_And_e_struct*)_tmp738)->f2;_LL3A2: _tmp744=
_tmp742;_tmp745=_tmp743;goto _LL3A4;_LL3A3: if(*((int*)_tmp738)!= 8)goto _LL3A5;
_tmp744=((struct Cyc_Absyn_Or_e_struct*)_tmp738)->f1;_tmp745=((struct Cyc_Absyn_Or_e_struct*)
_tmp738)->f2;_LL3A4: _tmp746=_tmp744;_tmp747=_tmp745;goto _LL3A6;_LL3A5: if(*((int*)
_tmp738)!= 9)goto _LL3A7;_tmp746=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp738)->f1;
_tmp747=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp738)->f2;_LL3A6: _tmp748=_tmp746;
_tmp749=_tmp747;goto _LL3A8;_LL3A7: if(*((int*)_tmp738)!= 25)goto _LL3A9;_tmp748=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp738)->f1;_tmp749=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp738)->f2;_LL3A8: _tmp74A=_tmp748;_tmp74B=_tmp749;goto _LL3AA;_LL3A9: if(*((int*)
_tmp738)!= 36)goto _LL3AB;_tmp74A=((struct Cyc_Absyn_Swap_e_struct*)_tmp738)->f1;
_tmp74B=((struct Cyc_Absyn_Swap_e_struct*)_tmp738)->f2;_LL3AA: _tmp74C=_tmp74A;
_tmp74D=_tmp74B;goto _LL3AC;_LL3AB: if(*((int*)_tmp738)!= 4)goto _LL3AD;_tmp74C=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp738)->f1;_tmp74D=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp738)->f3;_LL3AC: Cyc_Toc_exptypes_to_c(_tmp74C);Cyc_Toc_exptypes_to_c(_tmp74D);
goto _LL38E;_LL3AD: if(*((int*)_tmp738)!= 6)goto _LL3AF;_tmp74E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp738)->f1;_tmp74F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp738)->f2;
_tmp750=((struct Cyc_Absyn_Conditional_e_struct*)_tmp738)->f3;_LL3AE: Cyc_Toc_exptypes_to_c(
_tmp74E);Cyc_Toc_exptypes_to_c(_tmp74F);Cyc_Toc_exptypes_to_c(_tmp750);goto
_LL38E;_LL3AF: if(*((int*)_tmp738)!= 11)goto _LL3B1;_tmp751=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp738)->f1;_tmp752=((struct Cyc_Absyn_FnCall_e_struct*)_tmp738)->f2;_LL3B0:
_tmp753=_tmp751;_tmp754=_tmp752;goto _LL3B2;_LL3B1: if(*((int*)_tmp738)!= 10)goto
_LL3B3;_tmp753=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp738)->f1;_tmp754=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp738)->f2;_LL3B2: Cyc_Toc_exptypes_to_c(
_tmp753);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp754);goto _LL38E;_LL3B3: if(*((int*)_tmp738)!= 15)goto
_LL3B5;_tmp755=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp738)->f1;_tmp756=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp738)->f1);_tmp757=((struct
Cyc_Absyn_Cast_e_struct*)_tmp738)->f2;_LL3B4:*_tmp756=Cyc_Toc_typ_to_c(*_tmp756);
Cyc_Toc_exptypes_to_c(_tmp757);goto _LL38E;_LL3B5: if(*((int*)_tmp738)!= 27)goto
_LL3B7;_tmp758=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp738)->f1;_tmp759=*
_tmp758;_tmp75A=_tmp759.f3;_tmp75B=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp738)->f1).f3;_tmp75C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp738)->f2;
_LL3B6:*_tmp75B=Cyc_Toc_typ_to_c(*_tmp75B);_tmp75D=_tmp75C;goto _LL3B8;_LL3B7: if(*((
int*)_tmp738)!= 37)goto _LL3B9;_tmp75D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp738)->f2;_LL3B8: _tmp75E=_tmp75D;goto _LL3BA;_LL3B9: if(*((int*)_tmp738)!= 28)
goto _LL3BB;_tmp75E=((struct Cyc_Absyn_Array_e_struct*)_tmp738)->f1;_LL3BA: for(0;
_tmp75E != 0;_tmp75E=_tmp75E->tl){struct Cyc_Absyn_Exp*_tmp767;struct _tuple6
_tmp766=*((struct _tuple6*)_tmp75E->hd);_tmp767=_tmp766.f2;Cyc_Toc_exptypes_to_c(
_tmp767);}goto _LL38E;_LL3BB: if(*((int*)_tmp738)!= 20)goto _LL3BD;_tmp75F=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp738)->f1;_tmp760=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp738)->f1);_LL3BC: _tmp762=_tmp760;goto
_LL3BE;_LL3BD: if(*((int*)_tmp738)!= 18)goto _LL3BF;_tmp761=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp738)->f1;_tmp762=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp738)->f1);_LL3BE:*_tmp762=Cyc_Toc_typ_to_c(*_tmp762);goto _LL38E;_LL3BF: if(*((
int*)_tmp738)!= 38)goto _LL3C1;_tmp763=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp738)->f1;_LL3C0: Cyc_Toc_stmttypes_to_c(_tmp763);goto _LL38E;_LL3C1: if(*((int*)
_tmp738)!= 35)goto _LL3C3;_tmp764=((struct Cyc_Absyn_Malloc_e_struct*)_tmp738)->f1;
_tmp765=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp738)->f1;
_LL3C2: if(_tmp765->elt_type != 0)_tmp765->elt_type=({void**_tmp768=_cycalloc(
sizeof(*_tmp768));_tmp768[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp765->elt_type)));
_tmp768;});Cyc_Toc_exptypes_to_c(_tmp765->num_elts);goto _LL38E;_LL3C3: if(*((int*)
_tmp738)!= 0)goto _LL3C5;_LL3C4: goto _LL3C6;_LL3C5: if(*((int*)_tmp738)!= 1)goto
_LL3C7;_LL3C6: goto _LL3C8;_LL3C7: if(*((int*)_tmp738)!= 2)goto _LL3C9;_LL3C8: goto
_LL3CA;_LL3C9: if(*((int*)_tmp738)!= 33)goto _LL3CB;_LL3CA: goto _LL3CC;_LL3CB: if(*((
int*)_tmp738)!= 34)goto _LL3CD;_LL3CC: goto _LL38E;_LL3CD: if(*((int*)_tmp738)!= 31)
goto _LL3CF;_LL3CE: goto _LL3D0;_LL3CF: if(*((int*)_tmp738)!= 32)goto _LL3D1;_LL3D0:
goto _LL3D2;_LL3D1: if(*((int*)_tmp738)!= 30)goto _LL3D3;_LL3D2: goto _LL3D4;_LL3D3:
if(*((int*)_tmp738)!= 29)goto _LL3D5;_LL3D4: goto _LL3D6;_LL3D5: if(*((int*)_tmp738)
!= 26)goto _LL3D7;_LL3D6: goto _LL3D8;_LL3D7: if(*((int*)_tmp738)!= 14)goto _LL3D9;
_LL3D8: goto _LL3DA;_LL3D9: if(*((int*)_tmp738)!= 17)goto _LL3DB;_LL3DA: goto _LL3DC;
_LL3DB: if(*((int*)_tmp738)!= 21)goto _LL38E;_LL3DC:({void*_tmp769[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp76A="Cyclone expression within C code";_tag_dynforward(
_tmp76A,sizeof(char),_get_zero_arr_size(_tmp76A,33));}),_tag_dynforward(_tmp769,
sizeof(void*),0));});goto _LL38E;_LL38E:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp76B=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp76C;
struct Cyc_Absyn_Fndecl*_tmp76D;struct Cyc_Absyn_Aggrdecl*_tmp76E;struct Cyc_Absyn_Enumdecl*
_tmp76F;struct Cyc_Absyn_Typedefdecl*_tmp770;_LL3DE: if(_tmp76B <= (void*)2)goto
_LL3F6;if(*((int*)_tmp76B)!= 0)goto _LL3E0;_tmp76C=((struct Cyc_Absyn_Var_d_struct*)
_tmp76B)->f1;_LL3DF:(void*)(_tmp76C->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp76C->type));
if(_tmp76C->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp76C->initializer));goto _LL3DD;_LL3E0: if(*((int*)_tmp76B)!= 1)
goto _LL3E2;_tmp76D=((struct Cyc_Absyn_Fn_d_struct*)_tmp76B)->f1;_LL3E1:(void*)(
_tmp76D->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp76D->ret_type));{struct Cyc_List_List*
_tmp771=_tmp76D->args;for(0;_tmp771 != 0;_tmp771=_tmp771->tl){(*((struct _tuple15*)
_tmp771->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp771->hd)).f3);}}goto
_LL3DD;_LL3E2: if(*((int*)_tmp76B)!= 4)goto _LL3E4;_tmp76E=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp76B)->f1;_LL3E3: Cyc_Toc_aggrdecl_to_c(_tmp76E);goto _LL3DD;_LL3E4: if(*((int*)
_tmp76B)!= 6)goto _LL3E6;_tmp76F=((struct Cyc_Absyn_Enum_d_struct*)_tmp76B)->f1;
_LL3E5: if(_tmp76F->fields != 0){struct Cyc_List_List*_tmp772=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp76F->fields))->v;for(0;_tmp772 != 0;_tmp772=
_tmp772->tl){struct Cyc_Absyn_Enumfield*_tmp773=(struct Cyc_Absyn_Enumfield*)
_tmp772->hd;if(_tmp773->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp773->tag));}}goto _LL3DD;_LL3E6: if(*((int*)_tmp76B)!= 7)goto
_LL3E8;_tmp770=((struct Cyc_Absyn_Typedef_d_struct*)_tmp76B)->f1;_LL3E7: _tmp770->defn=({
struct Cyc_Core_Opt*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp770->defn))->v);_tmp774;});goto
_LL3DD;_LL3E8: if(*((int*)_tmp76B)!= 2)goto _LL3EA;_LL3E9: goto _LL3EB;_LL3EA: if(*((
int*)_tmp76B)!= 3)goto _LL3EC;_LL3EB: goto _LL3ED;_LL3EC: if(*((int*)_tmp76B)!= 5)
goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE: if(*((int*)_tmp76B)!= 8)goto _LL3F0;_LL3EF:
goto _LL3F1;_LL3F0: if(*((int*)_tmp76B)!= 9)goto _LL3F2;_LL3F1: goto _LL3F3;_LL3F2:
if(*((int*)_tmp76B)!= 10)goto _LL3F4;_LL3F3: goto _LL3F5;_LL3F4: if(*((int*)_tmp76B)
!= 11)goto _LL3F6;_LL3F5:({void*_tmp775[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp776="Cyclone declaration within C code";_tag_dynforward(_tmp776,sizeof(char),
_get_zero_arr_size(_tmp776,34));}),_tag_dynforward(_tmp775,sizeof(void*),0));});
goto _LL3DD;_LL3F6: if((int)_tmp76B != 0)goto _LL3F8;_LL3F7: goto _LL3F9;_LL3F8: if((
int)_tmp76B != 1)goto _LL3DD;_LL3F9: goto _LL3DD;_LL3DD:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp777=(void*)s->r;struct Cyc_Absyn_Exp*_tmp778;
struct Cyc_Absyn_Stmt*_tmp779;struct Cyc_Absyn_Stmt*_tmp77A;struct Cyc_Absyn_Exp*
_tmp77B;struct Cyc_Absyn_Exp*_tmp77C;struct Cyc_Absyn_Stmt*_tmp77D;struct Cyc_Absyn_Stmt*
_tmp77E;struct _tuple3 _tmp77F;struct Cyc_Absyn_Exp*_tmp780;struct Cyc_Absyn_Stmt*
_tmp781;struct Cyc_Absyn_Exp*_tmp782;struct _tuple3 _tmp783;struct Cyc_Absyn_Exp*
_tmp784;struct _tuple3 _tmp785;struct Cyc_Absyn_Exp*_tmp786;struct Cyc_Absyn_Stmt*
_tmp787;struct Cyc_Absyn_Exp*_tmp788;struct Cyc_List_List*_tmp789;struct Cyc_Absyn_Decl*
_tmp78A;struct Cyc_Absyn_Stmt*_tmp78B;struct Cyc_Absyn_Stmt*_tmp78C;struct _tuple3
_tmp78D;struct Cyc_Absyn_Exp*_tmp78E;_LL3FB: if(_tmp777 <= (void*)1)goto _LL40D;if(*((
int*)_tmp777)!= 0)goto _LL3FD;_tmp778=((struct Cyc_Absyn_Exp_s_struct*)_tmp777)->f1;
_LL3FC: Cyc_Toc_exptypes_to_c(_tmp778);goto _LL3FA;_LL3FD: if(*((int*)_tmp777)!= 1)
goto _LL3FF;_tmp779=((struct Cyc_Absyn_Seq_s_struct*)_tmp777)->f1;_tmp77A=((struct
Cyc_Absyn_Seq_s_struct*)_tmp777)->f2;_LL3FE: Cyc_Toc_stmttypes_to_c(_tmp779);Cyc_Toc_stmttypes_to_c(
_tmp77A);goto _LL3FA;_LL3FF: if(*((int*)_tmp777)!= 2)goto _LL401;_tmp77B=((struct
Cyc_Absyn_Return_s_struct*)_tmp777)->f1;_LL400: if(_tmp77B != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp77B);goto _LL3FA;_LL401: if(*((int*)_tmp777)!= 3)goto
_LL403;_tmp77C=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp777)->f1;_tmp77D=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp777)->f2;_tmp77E=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp777)->f3;_LL402: Cyc_Toc_exptypes_to_c(_tmp77C);Cyc_Toc_stmttypes_to_c(
_tmp77D);Cyc_Toc_stmttypes_to_c(_tmp77E);goto _LL3FA;_LL403: if(*((int*)_tmp777)!= 
4)goto _LL405;_tmp77F=((struct Cyc_Absyn_While_s_struct*)_tmp777)->f1;_tmp780=
_tmp77F.f1;_tmp781=((struct Cyc_Absyn_While_s_struct*)_tmp777)->f2;_LL404: Cyc_Toc_exptypes_to_c(
_tmp780);Cyc_Toc_stmttypes_to_c(_tmp781);goto _LL3FA;_LL405: if(*((int*)_tmp777)!= 
8)goto _LL407;_tmp782=((struct Cyc_Absyn_For_s_struct*)_tmp777)->f1;_tmp783=((
struct Cyc_Absyn_For_s_struct*)_tmp777)->f2;_tmp784=_tmp783.f1;_tmp785=((struct
Cyc_Absyn_For_s_struct*)_tmp777)->f3;_tmp786=_tmp785.f1;_tmp787=((struct Cyc_Absyn_For_s_struct*)
_tmp777)->f4;_LL406: Cyc_Toc_exptypes_to_c(_tmp782);Cyc_Toc_exptypes_to_c(_tmp784);
Cyc_Toc_exptypes_to_c(_tmp786);Cyc_Toc_stmttypes_to_c(_tmp787);goto _LL3FA;_LL407:
if(*((int*)_tmp777)!= 9)goto _LL409;_tmp788=((struct Cyc_Absyn_Switch_s_struct*)
_tmp777)->f1;_tmp789=((struct Cyc_Absyn_Switch_s_struct*)_tmp777)->f2;_LL408: Cyc_Toc_exptypes_to_c(
_tmp788);for(0;_tmp789 != 0;_tmp789=_tmp789->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp789->hd)->body);}goto _LL3FA;_LL409: if(*((int*)
_tmp777)!= 11)goto _LL40B;_tmp78A=((struct Cyc_Absyn_Decl_s_struct*)_tmp777)->f1;
_tmp78B=((struct Cyc_Absyn_Decl_s_struct*)_tmp777)->f2;_LL40A: Cyc_Toc_decltypes_to_c(
_tmp78A);Cyc_Toc_stmttypes_to_c(_tmp78B);goto _LL3FA;_LL40B: if(*((int*)_tmp777)!= 
13)goto _LL40D;_tmp78C=((struct Cyc_Absyn_Do_s_struct*)_tmp777)->f1;_tmp78D=((
struct Cyc_Absyn_Do_s_struct*)_tmp777)->f2;_tmp78E=_tmp78D.f1;_LL40C: Cyc_Toc_stmttypes_to_c(
_tmp78C);Cyc_Toc_exptypes_to_c(_tmp78E);goto _LL3FA;_LL40D: if((int)_tmp777 != 0)
goto _LL40F;_LL40E: goto _LL410;_LL40F: if(_tmp777 <= (void*)1)goto _LL411;if(*((int*)
_tmp777)!= 5)goto _LL411;_LL410: goto _LL412;_LL411: if(_tmp777 <= (void*)1)goto
_LL413;if(*((int*)_tmp777)!= 6)goto _LL413;_LL412: goto _LL414;_LL413: if(_tmp777 <= (
void*)1)goto _LL415;if(*((int*)_tmp777)!= 7)goto _LL415;_LL414: goto _LL3FA;_LL415:
if(_tmp777 <= (void*)1)goto _LL417;if(*((int*)_tmp777)!= 10)goto _LL417;_LL416: goto
_LL418;_LL417: if(_tmp777 <= (void*)1)goto _LL419;if(*((int*)_tmp777)!= 12)goto
_LL419;_LL418: goto _LL41A;_LL419: if(_tmp777 <= (void*)1)goto _LL41B;if(*((int*)
_tmp777)!= 14)goto _LL41B;_LL41A: goto _LL41C;_LL41B: if(_tmp777 <= (void*)1)goto
_LL41D;if(*((int*)_tmp777)!= 15)goto _LL41D;_LL41C: goto _LL41E;_LL41D: if(_tmp777 <= (
void*)1)goto _LL3FA;if(*((int*)_tmp777)!= 16)goto _LL3FA;_LL41E:({void*_tmp78F[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp790="Cyclone statement in C code";
_tag_dynforward(_tmp790,sizeof(char),_get_zero_arr_size(_tmp790,28));}),
_tag_dynforward(_tmp78F,sizeof(void*),0));});goto _LL3FA;_LL3FA:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({
void*_tmp791[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp792="decls_to_c: not at toplevel!";
_tag_dynforward(_tmp792,sizeof(char),_get_zero_arr_size(_tmp792,29));}),
_tag_dynforward(_tmp791,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp793=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp794;struct Cyc_Absyn_Fndecl*
_tmp795;struct Cyc_Absyn_Aggrdecl*_tmp796;struct Cyc_Absyn_Tuniondecl*_tmp797;
struct Cyc_Absyn_Enumdecl*_tmp798;struct Cyc_Absyn_Typedefdecl*_tmp799;struct Cyc_List_List*
_tmp79A;struct Cyc_List_List*_tmp79B;struct Cyc_List_List*_tmp79C;struct Cyc_List_List*
_tmp79D;_LL420: if(_tmp793 <= (void*)2)goto _LL430;if(*((int*)_tmp793)!= 0)goto
_LL422;_tmp794=((struct Cyc_Absyn_Var_d_struct*)_tmp793)->f1;_LL421: {struct
_tuple1*_tmp79E=_tmp794->name;if((void*)_tmp794->sc == (void*)4)_tmp79E=({struct
_tuple1*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7A0;(_tmp7A0.Rel_n).tag=1;(_tmp7A0.Rel_n).f1=0;
_tmp7A0;});_tmp79F->f2=(*_tmp79E).f2;_tmp79F;});if(_tmp794->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp794->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp794->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp794->name,Cyc_Absyn_varb_exp(_tmp79E,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=({
struct Cyc_Absyn_Global_b_struct _tmp7A2;_tmp7A2.tag=0;_tmp7A2.f1=_tmp794;_tmp7A2;});
_tmp7A1;}),0));_tmp794->name=_tmp79E;(void*)(_tmp794->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp794->sc));(void*)(_tmp794->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp794->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7A3=_cycalloc(
sizeof(*_tmp7A3));_tmp7A3->hd=d;_tmp7A3->tl=Cyc_Toc_result_decls;_tmp7A3;});goto
_LL41F;}_LL422: if(*((int*)_tmp793)!= 1)goto _LL424;_tmp795=((struct Cyc_Absyn_Fn_d_struct*)
_tmp793)->f1;_LL423: {struct _tuple1*_tmp7A4=_tmp795->name;if((void*)_tmp795->sc
== (void*)4){_tmp7A4=({struct _tuple1*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7A6;(_tmp7A6.Rel_n).tag=
1;(_tmp7A6.Rel_n).f1=0;_tmp7A6;});_tmp7A5->f2=(*_tmp7A4).f2;_tmp7A5;});(void*)(
_tmp795->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp795->name,Cyc_Absyn_var_exp(
_tmp7A4,0));_tmp795->name=_tmp7A4;Cyc_Toc_fndecl_to_c(nv,_tmp795,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7->hd=d;_tmp7A7->tl=
Cyc_Toc_result_decls;_tmp7A7;});goto _LL41F;}_LL424: if(*((int*)_tmp793)!= 2)goto
_LL426;_LL425: goto _LL427;_LL426: if(*((int*)_tmp793)!= 3)goto _LL428;_LL427:({void*
_tmp7A8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp7A9="letdecl at toplevel";_tag_dynforward(_tmp7A9,sizeof(char),
_get_zero_arr_size(_tmp7A9,20));}),_tag_dynforward(_tmp7A8,sizeof(void*),0));});
_LL428: if(*((int*)_tmp793)!= 4)goto _LL42A;_tmp796=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp793)->f1;_LL429: Cyc_Toc_aggrdecl_to_c(_tmp796);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA->hd=d;_tmp7AA->tl=Cyc_Toc_result_decls;
_tmp7AA;});goto _LL41F;_LL42A: if(*((int*)_tmp793)!= 5)goto _LL42C;_tmp797=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp793)->f1;_LL42B: if(_tmp797->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp797);else{Cyc_Toc_tuniondecl_to_c(_tmp797);}goto _LL41F;_LL42C: if(*((int*)
_tmp793)!= 6)goto _LL42E;_tmp798=((struct Cyc_Absyn_Enum_d_struct*)_tmp793)->f1;
_LL42D: Cyc_Toc_enumdecl_to_c(nv,_tmp798);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB->hd=d;_tmp7AB->tl=Cyc_Toc_result_decls;
_tmp7AB;});goto _LL41F;_LL42E: if(*((int*)_tmp793)!= 7)goto _LL430;_tmp799=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp793)->f1;_LL42F: _tmp799->name=_tmp799->name;
_tmp799->tvs=0;if(_tmp799->defn != 0)_tmp799->defn=({struct Cyc_Core_Opt*_tmp7AC=
_cycalloc(sizeof(*_tmp7AC));_tmp7AC->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp799->defn))->v);_tmp7AC;});else{void*_tmp7AD=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp799->kind))->v;_LL43D: if((int)
_tmp7AD != 2)goto _LL43F;_LL43E: _tmp799->defn=({struct Cyc_Core_Opt*_tmp7AE=
_cycalloc(sizeof(*_tmp7AE));_tmp7AE->v=(void*)Cyc_Absyn_void_star_typ();_tmp7AE;});
goto _LL43C;_LL43F:;_LL440: _tmp799->defn=({struct Cyc_Core_Opt*_tmp7AF=_cycalloc(
sizeof(*_tmp7AF));_tmp7AF->v=(void*)((void*)0);_tmp7AF;});goto _LL43C;_LL43C:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));
_tmp7B0->hd=d;_tmp7B0->tl=Cyc_Toc_result_decls;_tmp7B0;});goto _LL41F;_LL430: if((
int)_tmp793 != 0)goto _LL432;_LL431: goto _LL433;_LL432: if((int)_tmp793 != 1)goto
_LL434;_LL433: goto _LL41F;_LL434: if(_tmp793 <= (void*)2)goto _LL436;if(*((int*)
_tmp793)!= 8)goto _LL436;_tmp79A=((struct Cyc_Absyn_Namespace_d_struct*)_tmp793)->f2;
_LL435: _tmp79B=_tmp79A;goto _LL437;_LL436: if(_tmp793 <= (void*)2)goto _LL438;if(*((
int*)_tmp793)!= 9)goto _LL438;_tmp79B=((struct Cyc_Absyn_Using_d_struct*)_tmp793)->f2;
_LL437: _tmp79C=_tmp79B;goto _LL439;_LL438: if(_tmp793 <= (void*)2)goto _LL43A;if(*((
int*)_tmp793)!= 10)goto _LL43A;_tmp79C=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp793)->f1;_LL439: nv=Cyc_Toc_decls_to_c(r,nv,_tmp79C,top,cinclude);goto _LL41F;
_LL43A: if(_tmp793 <= (void*)2)goto _LL41F;if(*((int*)_tmp793)!= 11)goto _LL41F;
_tmp79D=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp793)->f1;_LL43B: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp79D,top,1);goto _LL41F;_LL41F:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;Cyc_Toc_globals=_tag_dynforward(({struct _dynforward_ptr**_tmp7B1=_cycalloc(
sizeof(struct _dynforward_ptr*)* 50);_tmp7B1[0]=& Cyc_Toc__throw_str;_tmp7B1[1]=&
Cyc_Toc_setjmp_str;_tmp7B1[2]=& Cyc_Toc__push_handler_str;_tmp7B1[3]=& Cyc_Toc__pop_handler_str;
_tmp7B1[4]=& Cyc_Toc__exn_thrown_str;_tmp7B1[5]=& Cyc_Toc__npop_handler_str;
_tmp7B1[6]=& Cyc_Toc__check_null_str;_tmp7B1[7]=& Cyc_Toc__check_known_subscript_null_str;
_tmp7B1[8]=& Cyc_Toc__check_known_subscript_notnull_str;_tmp7B1[9]=& Cyc_Toc__check_dynforward_subscript_str;
_tmp7B1[10]=& Cyc_Toc__check_dyneither_subscript_str;_tmp7B1[11]=& Cyc_Toc__dynforward_ptr_str;
_tmp7B1[12]=& Cyc_Toc__dyneither_ptr_str;_tmp7B1[13]=& Cyc_Toc__tag_dynforward_str;
_tmp7B1[14]=& Cyc_Toc__tag_dyneither_str;_tmp7B1[15]=& Cyc_Toc__init_dynforward_ptr_str;
_tmp7B1[16]=& Cyc_Toc__init_dyneither_ptr_str;_tmp7B1[17]=& Cyc_Toc__untag_dynforward_ptr_str;
_tmp7B1[18]=& Cyc_Toc__untag_dyneither_ptr_str;_tmp7B1[19]=& Cyc_Toc__get_dynforward_size_str;
_tmp7B1[20]=& Cyc_Toc__get_dyneither_size_str;_tmp7B1[21]=& Cyc_Toc__get_zero_arr_size_str;
_tmp7B1[22]=& Cyc_Toc__dynforward_ptr_plus_str;_tmp7B1[23]=& Cyc_Toc__dyneither_ptr_plus_str;
_tmp7B1[24]=& Cyc_Toc__zero_arr_plus_str;_tmp7B1[25]=& Cyc_Toc__dynforward_ptr_inplace_plus_str;
_tmp7B1[26]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp7B1[27]=& Cyc_Toc__zero_arr_inplace_plus_str;
_tmp7B1[28]=& Cyc_Toc__dynforward_ptr_inplace_plus_post_str;_tmp7B1[29]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;
_tmp7B1[30]=& Cyc_Toc__dynforward_to_dyneither_str;_tmp7B1[31]=& Cyc_Toc__dyneither_to_dynforward_str;
_tmp7B1[32]=& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp7B1[33]=& Cyc_Toc__cycalloc_str;
_tmp7B1[34]=& Cyc_Toc__cyccalloc_str;_tmp7B1[35]=& Cyc_Toc__cycalloc_atomic_str;
_tmp7B1[36]=& Cyc_Toc__cyccalloc_atomic_str;_tmp7B1[37]=& Cyc_Toc__region_malloc_str;
_tmp7B1[38]=& Cyc_Toc__region_calloc_str;_tmp7B1[39]=& Cyc_Toc__check_times_str;
_tmp7B1[40]=& Cyc_Toc__new_region_str;_tmp7B1[41]=& Cyc_Toc__push_region_str;
_tmp7B1[42]=& Cyc_Toc__pop_region_str;_tmp7B1[43]=& Cyc_Toc__open_dynregion_str;
_tmp7B1[44]=& Cyc_Toc__push_dynregion_str;_tmp7B1[45]=& Cyc_Toc__pop_dynregion_str;
_tmp7B1[46]=& Cyc_Toc__reset_region_str;_tmp7B1[47]=& Cyc_Toc__throw_arraybounds_str;
_tmp7B1[48]=& Cyc_Toc__dynforward_ptr_decrease_size_str;_tmp7B1[49]=& Cyc_Toc__dyneither_ptr_decrease_size_str;
_tmp7B1;}),sizeof(struct _dynforward_ptr*),50);}struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmp7B2=_new_region("start");
struct _RegionHandle*start=& _tmp7B2;_push_region(start);Cyc_Toc_decls_to_c(start,
Cyc_Toc_empty_env(start),ds,1,0);;_pop_region(start);}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
