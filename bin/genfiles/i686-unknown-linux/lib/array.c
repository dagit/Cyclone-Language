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
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct _dynforward_ptr Cyc_List_to_array(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_from_array(struct _dynforward_ptr arr);void Cyc_Array_qsort(
int(*)(void**,void**),struct _dynforward_ptr x,int len);void Cyc_Array_msort(int(*)(
void**,void**),struct _dynforward_ptr x,int len);struct _dynforward_ptr Cyc_Array_from_list(
struct Cyc_List_List*l);struct Cyc_List_List*Cyc_Array_to_list(struct
_dynforward_ptr x);struct _dynforward_ptr Cyc_Array_copy(struct _dynforward_ptr x);
struct _dynforward_ptr Cyc_Array_map(void*(*f)(void*),struct _dynforward_ptr x);
struct _dynforward_ptr Cyc_Array_map_c(void*(*f)(void*,void*),void*env,struct
_dynforward_ptr x);void Cyc_Array_imp_map(void*(*f)(void*),struct _dynforward_ptr x);
void Cyc_Array_imp_map_c(void*(*f)(void*,void*),void*env,struct _dynforward_ptr x);
extern char Cyc_Array_Array_mismatch[19];struct _dynforward_ptr Cyc_Array_map2(void*(*
f)(void*,void*),struct _dynforward_ptr x,struct _dynforward_ptr y);void Cyc_Array_app(
void*(*f)(void*),struct _dynforward_ptr x);void Cyc_Array_app_c(void*(*f)(void*,
void*),void*env,struct _dynforward_ptr x);void Cyc_Array_iter(void(*f)(void*),
struct _dynforward_ptr x);void Cyc_Array_iter_c(void(*f)(void*,void*),void*env,
struct _dynforward_ptr x);void Cyc_Array_app2(void*(*f)(void*,void*),struct
_dynforward_ptr x,struct _dynforward_ptr y);void Cyc_Array_app2_c(void*(*f)(void*,
void*,void*),void*env,struct _dynforward_ptr x,struct _dynforward_ptr y);void Cyc_Array_iter2(
void(*f)(void*,void*),struct _dynforward_ptr x,struct _dynforward_ptr y);void Cyc_Array_iter2_c(
void(*f)(void*,void*,void*),void*env,struct _dynforward_ptr x,struct
_dynforward_ptr y);void*Cyc_Array_fold_left(void*(*f)(void*,void*),void*accum,
struct _dynforward_ptr x);void*Cyc_Array_fold_left_c(void*(*f)(void*,void*,void*),
void*env,void*accum,struct _dynforward_ptr x);void*Cyc_Array_fold_right(void*(*f)(
void*,void*),struct _dynforward_ptr x,void*accum);void*Cyc_Array_fold_right_c(void*(*
f)(void*,void*,void*),void*env,struct _dynforward_ptr x,void*accum);struct
_dynforward_ptr Cyc_Array_rev_copy(struct _dynforward_ptr x);void Cyc_Array_imp_rev(
struct _dynforward_ptr x);int Cyc_Array_forall(int(*pred)(void*),struct
_dynforward_ptr x);int Cyc_Array_forall_c(int(*pred)(void*,void*),void*env,struct
_dynforward_ptr x);int Cyc_Array_exists(int(*pred)(void*),struct _dynforward_ptr x);
int Cyc_Array_exists_c(int(*pred)(void*,void*),void*env,struct _dynforward_ptr x);
struct _tuple0{void*f1;void*f2;};struct _dynforward_ptr Cyc_Array_zip(struct
_dynforward_ptr x,struct _dynforward_ptr y);struct _tuple1{struct _dynforward_ptr f1;
struct _dynforward_ptr f2;};struct _tuple1 Cyc_Array_split(struct _dynforward_ptr x);
int Cyc_Array_memq(struct _dynforward_ptr l,void*x);int Cyc_Array_mem(int(*cmp)(void*,
void*),struct _dynforward_ptr l,void*x);struct _dynforward_ptr Cyc_Array_extract(
struct _dynforward_ptr x,int start,int*len_opt);void Cyc_Array_qsort(int(*less_eq)(
void**,void**),struct _dynforward_ptr arr,int len){int base_ofs=0;void*temp;int sp[40];
int sp_ofs;int i;int j;int limit_ofs;if((base_ofs < 0  || base_ofs + len > 
_get_dynforward_size(arr,sizeof(void*))) || len < 0)(int)_throw((void*)({struct
Cyc_Core_Invalid_argument_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp1;_tmp1.tag=Cyc_Core_Invalid_argument;
_tmp1.f1=({const char*_tmp2="Array::qsort";_tag_dynforward(_tmp2,sizeof(char),
_get_zero_arr_size(_tmp2,13));});_tmp1;});_tmp0;}));limit_ofs=base_ofs + len;
sp_ofs=0;for(0;1;0){if(limit_ofs - base_ofs > 3){temp=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),(limit_ofs - base_ofs)/ 2 + base_ofs));*((
void**)_check_dynforward_subscript(arr,sizeof(void*),(limit_ofs - base_ofs)/ 2 + 
base_ofs))=*((void**)_check_dynforward_subscript(arr,sizeof(void*),base_ofs));*((
void**)_check_dynforward_subscript(arr,sizeof(void*),base_ofs))=temp;i=base_ofs + 
1;j=limit_ofs - 1;if(less_eq((void**)_untag_dynforward_ptr(_dynforward_ptr_plus(
arr,sizeof(void*),i),sizeof(void*),1),(void**)_untag_dynforward_ptr(
_dynforward_ptr_plus(arr,sizeof(void*),j),sizeof(void*),1))> 0){temp=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),i));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),i))=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j))=temp;}if(less_eq((void**)
_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),base_ofs),sizeof(
void*),1),(void**)_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),j),
sizeof(void*),1))> 0){temp=*((void**)_check_dynforward_subscript(arr,sizeof(void*),
base_ofs));*((void**)_check_dynforward_subscript(arr,sizeof(void*),base_ofs))=*((
void**)_check_dynforward_subscript(arr,sizeof(void*),j));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j))=temp;}if(less_eq((void**)
_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),i),sizeof(void*),1),(
void**)_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),base_ofs),
sizeof(void*),1))> 0){temp=*((void**)_check_dynforward_subscript(arr,sizeof(void*),
i));*((void**)_check_dynforward_subscript(arr,sizeof(void*),i))=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),base_ofs));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),base_ofs))=temp;}for(0;1;0){do{i ++;}while(
less_eq((void**)_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),i),
sizeof(void*),1),(void**)_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(
void*),base_ofs),sizeof(void*),1))< 0);do{j --;}while(less_eq((void**)
_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),j),sizeof(void*),1),(
void**)_untag_dynforward_ptr(_dynforward_ptr_plus(arr,sizeof(void*),base_ofs),
sizeof(void*),1))> 0);if(i > j)break;temp=*((void**)_check_dynforward_subscript(
arr,sizeof(void*),i));*((void**)_check_dynforward_subscript(arr,sizeof(void*),i))=*((
void**)_check_dynforward_subscript(arr,sizeof(void*),j));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j))=temp;}temp=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),base_ofs));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),base_ofs))=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j))=temp;if(j - base_ofs > limit_ofs
- i){sp[_check_known_subscript_notnull(40,sp_ofs)]=base_ofs;sp[
_check_known_subscript_notnull(40,sp_ofs + 1)]=j;base_ofs=i;}else{sp[
_check_known_subscript_notnull(40,sp_ofs)]=i;sp[_check_known_subscript_notnull(
40,sp_ofs + 1)]=limit_ofs;limit_ofs=j;}sp_ofs +=2;}else{for((j=base_ofs,i=j + 1);i < 
limit_ofs;(j=i,i ++)){for(0;less_eq((void**)_untag_dynforward_ptr(
_dynforward_ptr_plus(arr,sizeof(void*),j),sizeof(void*),1),(void**)
_untag_dynforward_ptr(_dynforward_ptr_plus(_dynforward_ptr_plus(arr,sizeof(void*),
j),sizeof(void*),1),sizeof(void*),1))> 0;j --){temp=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j))=*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j + 1));*((void**)
_check_dynforward_subscript(arr,sizeof(void*),j + 1))=temp;if(j == base_ofs)break;}}
if(sp_ofs != 0){sp_ofs -=2;base_ofs=sp[_check_known_subscript_notnull(40,sp_ofs)];
limit_ofs=sp[_check_known_subscript_notnull(40,sp_ofs + 1)];}else{break;}}}}void
Cyc_Array_msort(int(*less_eq)(void**,void**),struct _dynforward_ptr arr,int len){
if(len > _get_dynforward_size(arr,sizeof(void*)) || len < 0)(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp4;_tmp4.tag=Cyc_Core_Invalid_argument;
_tmp4.f1=({const char*_tmp5="Array::msort";_tag_dynforward(_tmp5,sizeof(char),
_get_zero_arr_size(_tmp5,13));});_tmp4;});_tmp3;}));{struct _dynforward_ptr from=({
unsigned int _tmp6=(unsigned int)len;void**_tmp7=(void**)_cycalloc(_check_times(
sizeof(void*),_tmp6));struct _dynforward_ptr _tmp9=_tag_dynforward(_tmp7,sizeof(
void*),_tmp6);{unsigned int _tmp8=_tmp6;unsigned int i;for(i=0;i < _tmp8;i ++){_tmp7[
i]=*((void**)_check_dynforward_subscript(arr,sizeof(void*),0));}}_tmp9;});struct
_dynforward_ptr to=arr;struct _dynforward_ptr swap;int swapped=0;int stepsize;int
start;int lstart;int lend;int rstart;int rend;int dest;for(stepsize=1;stepsize < len;
stepsize=stepsize * 2){swap=from;from=to;to=swap;dest=0;for(start=0;start < len;
start=start + stepsize * 2){lstart=start;rstart=start + stepsize < len?start + 
stepsize: len;lend=rstart;rend=start + stepsize * 2 < len?start + stepsize * 2: len;
while(lstart < lend  && rstart < rend){if(less_eq((void**)_untag_dynforward_ptr(
_dynforward_ptr_plus(from,sizeof(void*),lstart),sizeof(void*),1),(void**)
_untag_dynforward_ptr(_dynforward_ptr_plus(from,sizeof(void*),rstart),sizeof(
void*),1))<= 0)*((void**)_check_dynforward_subscript(to,sizeof(void*),dest ++))=*((
void**)_check_dynforward_subscript(from,sizeof(void*),lstart ++));else{*((void**)
_check_dynforward_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_dynforward_subscript(from,sizeof(void*),rstart ++));}}while(lstart < lend){*((
void**)_check_dynforward_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_dynforward_subscript(from,sizeof(void*),lstart ++));}while(rstart < rend){*((
void**)_check_dynforward_subscript(to,sizeof(void*),dest ++))=*((void**)
_check_dynforward_subscript(from,sizeof(void*),rstart ++));}}}if(swapped){int i=0;
for(0;i < len;i ++){*((void**)_check_dynforward_subscript(from,sizeof(void*),i))=*((
void**)_check_dynforward_subscript(to,sizeof(void*),i));}}}}struct
_dynforward_ptr Cyc_Array_from_list(struct Cyc_List_List*x){return Cyc_List_to_array(
x);}struct Cyc_List_List*Cyc_Array_to_list(struct _dynforward_ptr x){return Cyc_List_from_array(
x);}struct _dynforward_ptr Cyc_Array_copy(struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));return({unsigned int _tmpA=(unsigned int)sx;
void**_tmpB=(void**)_cycalloc(_check_times(sizeof(void*),_tmpA));struct
_dynforward_ptr _tmpD=_tag_dynforward(_tmpB,sizeof(void*),_tmpA);{unsigned int
_tmpC=_tmpA;unsigned int i;for(i=0;i < _tmpC;i ++){_tmpB[i]=((void**)x.curr)[(int)i];}}
_tmpD;});}struct _dynforward_ptr Cyc_Array_map(void*(*f)(void*),struct
_dynforward_ptr x){int sx=(int)_get_dynforward_size(x,sizeof(void*));return({
unsigned int _tmpE=(unsigned int)sx;void**_tmpF=(void**)_cycalloc(_check_times(
sizeof(void*),_tmpE));struct _dynforward_ptr _tmp11=_tag_dynforward(_tmpF,sizeof(
void*),_tmpE);{unsigned int _tmp10=_tmpE;unsigned int i;for(i=0;i < _tmp10;i ++){
_tmpF[i]=f(((void**)x.curr)[(int)i]);}}_tmp11;});}struct _dynforward_ptr Cyc_Array_map_c(
void*(*f)(void*,void*),void*env,struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));return({unsigned int _tmp12=(unsigned int)
sx;void**_tmp13=(void**)_cycalloc(_check_times(sizeof(void*),_tmp12));struct
_dynforward_ptr _tmp15=_tag_dynforward(_tmp13,sizeof(void*),_tmp12);{unsigned int
_tmp14=_tmp12;unsigned int i;for(i=0;i < _tmp14;i ++){_tmp13[i]=f(env,((void**)x.curr)[(
int)i]);}}_tmp15;});}void Cyc_Array_imp_map(void*(*f)(void*),struct
_dynforward_ptr x){int sx=(int)_get_dynforward_size(x,sizeof(void*));int i=0;for(0;
i < sx;i ++){*((void**)_check_dynforward_subscript(x,sizeof(void*),i))=f(*((void**)
_check_dynforward_subscript(x,sizeof(void*),i)));}}void Cyc_Array_imp_map_c(void*(*
f)(void*,void*),void*env,struct _dynforward_ptr x){int sx=(int)_get_dynforward_size(
x,sizeof(void*));int i=0;for(0;i < sx;i ++){*((void**)_check_dynforward_subscript(x,
sizeof(void*),i))=f(env,*((void**)_check_dynforward_subscript(x,sizeof(void*),i)));}}
char Cyc_Array_Array_mismatch[19]="\000\000\000\000Array_mismatch\000";struct
_dynforward_ptr Cyc_Array_map2(void*(*f)(void*,void*),struct _dynforward_ptr x,
struct _dynforward_ptr y){int sx=(int)_get_dynforward_size(x,sizeof(void*));if(sx != 
_get_dynforward_size(y,sizeof(void*)))(int)_throw((void*)Cyc_Array_Array_mismatch);
return({unsigned int _tmp16=(unsigned int)sx;void**_tmp17=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp16));struct _dynforward_ptr _tmp19=_tag_dynforward(
_tmp17,sizeof(void*),_tmp16);{unsigned int _tmp18=_tmp16;unsigned int i;for(i=0;i < 
_tmp18;i ++){_tmp17[i]=f(((void**)x.curr)[(int)i],*((void**)
_check_dynforward_subscript(y,sizeof(void*),(int)i)));}}_tmp19;});}void Cyc_Array_app(
void*(*f)(void*),struct _dynforward_ptr x){int sx=(int)_get_dynforward_size(x,
sizeof(void*));int i=0;for(0;i < sx;i ++){f(*((void**)_check_dynforward_subscript(x,
sizeof(void*),i)));}}void Cyc_Array_app_c(void*(*f)(void*,void*),void*env,struct
_dynforward_ptr x){int sx=(int)_get_dynforward_size(x,sizeof(void*));int i=0;for(0;
i < sx;i ++){f(env,*((void**)_check_dynforward_subscript(x,sizeof(void*),i)));}}
void Cyc_Array_iter(void(*f)(void*),struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));int i=0;for(0;i < sx;i ++){f(*((void**)
_check_dynforward_subscript(x,sizeof(void*),i)));}}void Cyc_Array_iter_c(void(*f)(
void*,void*),void*env,struct _dynforward_ptr x){int sx=(int)_get_dynforward_size(x,
sizeof(void*));int i=0;for(0;i < sx;i ++){f(env,*((void**)
_check_dynforward_subscript(x,sizeof(void*),i)));}}void Cyc_Array_app2(void*(*f)(
void*,void*),struct _dynforward_ptr x,struct _dynforward_ptr y){int sx=(int)
_get_dynforward_size(x,sizeof(void*));if(sx != _get_dynforward_size(y,sizeof(void*)))(
int)_throw((void*)Cyc_Array_Array_mismatch);{int i=0;for(0;i < sx;i ++){f(*((void**)
_check_dynforward_subscript(x,sizeof(void*),i)),*((void**)
_check_dynforward_subscript(y,sizeof(void*),i)));}}}void Cyc_Array_app2_c(void*(*
f)(void*,void*,void*),void*env,struct _dynforward_ptr x,struct _dynforward_ptr y){
int sx=(int)_get_dynforward_size(x,sizeof(void*));if(sx != _get_dynforward_size(y,
sizeof(void*)))(int)_throw((void*)Cyc_Array_Array_mismatch);{int i=0;for(0;i < sx;
i ++){f(env,*((void**)_check_dynforward_subscript(x,sizeof(void*),i)),*((void**)
_check_dynforward_subscript(y,sizeof(void*),i)));}}}void Cyc_Array_iter2(void(*f)(
void*,void*),struct _dynforward_ptr x,struct _dynforward_ptr y){int sx=(int)
_get_dynforward_size(x,sizeof(void*));if(sx != _get_dynforward_size(y,sizeof(void*)))(
int)_throw((void*)Cyc_Array_Array_mismatch);{int i=0;for(0;i < sx;i ++){f(*((void**)
_check_dynforward_subscript(x,sizeof(void*),i)),*((void**)
_check_dynforward_subscript(y,sizeof(void*),i)));}}}void Cyc_Array_iter2_c(void(*
f)(void*,void*,void*),void*env,struct _dynforward_ptr x,struct _dynforward_ptr y){
int sx=(int)_get_dynforward_size(x,sizeof(void*));if(sx != _get_dynforward_size(y,
sizeof(void*)))(int)_throw((void*)Cyc_Array_Array_mismatch);{int i=0;for(0;i < sx;
i ++){f(env,*((void**)_check_dynforward_subscript(x,sizeof(void*),i)),*((void**)
_check_dynforward_subscript(y,sizeof(void*),i)));}}}void*Cyc_Array_fold_left(
void*(*f)(void*,void*),void*accum,struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){accum=f(accum,*((
void**)_check_dynforward_subscript(x,sizeof(void*),i)));}}return accum;}void*Cyc_Array_fold_left_c(
void*(*f)(void*,void*,void*),void*env,void*accum,struct _dynforward_ptr x){int sx=(
int)_get_dynforward_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){accum=f(env,
accum,*((void**)_check_dynforward_subscript(x,sizeof(void*),i)));}}return accum;}
void*Cyc_Array_fold_right(void*(*f)(void*,void*),struct _dynforward_ptr x,void*
accum){{int i=(int)(_get_dynforward_size(x,sizeof(void*))- 1);for(0;i >= 0;i --){
accum=f(*((void**)_check_dynforward_subscript(x,sizeof(void*),i)),accum);}}
return accum;}void*Cyc_Array_fold_right_c(void*(*f)(void*,void*,void*),void*env,
struct _dynforward_ptr x,void*accum){{int i=(int)(_get_dynforward_size(x,sizeof(
void*))- 1);for(0;i >= 0;i --){accum=f(env,*((void**)_check_dynforward_subscript(x,
sizeof(void*),i)),accum);}}return accum;}struct _dynforward_ptr Cyc_Array_rev_copy(
struct _dynforward_ptr x){int sx=(int)_get_dynforward_size(x,sizeof(void*));int n=sx
- 1;return({unsigned int _tmp1A=(unsigned int)sx;void**_tmp1B=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp1A));struct _dynforward_ptr _tmp1D=_tag_dynforward(
_tmp1B,sizeof(void*),_tmp1A);{unsigned int _tmp1C=_tmp1A;unsigned int i;for(i=0;i < 
_tmp1C;i ++){_tmp1B[i]=*((void**)_check_dynforward_subscript(x,sizeof(void*),(int)(
n - i)));}}_tmp1D;});}void Cyc_Array_imp_rev(struct _dynforward_ptr x){void*temp;int
i=0;int j=(int)(_get_dynforward_size(x,sizeof(void*))- 1);while(i < j){temp=*((
void**)_check_dynforward_subscript(x,sizeof(void*),i));*((void**)
_check_dynforward_subscript(x,sizeof(void*),i))=*((void**)
_check_dynforward_subscript(x,sizeof(void*),j));*((void**)
_check_dynforward_subscript(x,sizeof(void*),j))=temp;i ++;j --;}}int Cyc_Array_forall(
int(*pred)(void*),struct _dynforward_ptr x){int sx=(int)_get_dynforward_size(x,
sizeof(void*));{int i=0;for(0;i < sx;i ++){if(!pred(*((void**)
_check_dynforward_subscript(x,sizeof(void*),i))))return 0;}}return 1;}int Cyc_Array_forall_c(
int(*pred)(void*,void*),void*env,struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){if(!pred(env,*((
void**)_check_dynforward_subscript(x,sizeof(void*),i))))return 0;}}return 1;}int
Cyc_Array_exists(int(*pred)(void*),struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){if(pred(*((void**)
_check_dynforward_subscript(x,sizeof(void*),i))))return 1;}}return 0;}int Cyc_Array_exists_c(
int(*pred)(void*,void*),void*env,struct _dynforward_ptr x){int sx=(int)
_get_dynforward_size(x,sizeof(void*));{int i=0;for(0;i < sx;i ++){if(pred(env,*((
void**)_check_dynforward_subscript(x,sizeof(void*),i))))return 1;}}return 0;}
struct _dynforward_ptr Cyc_Array_zip(struct _dynforward_ptr x,struct _dynforward_ptr y){
int sx=(int)_get_dynforward_size(x,sizeof(void*));if(sx != _get_dynforward_size(y,
sizeof(void*)))(int)_throw((void*)Cyc_Array_Array_mismatch);return({unsigned int
_tmp1E=(unsigned int)sx;struct _tuple0*_tmp1F=(struct _tuple0*)_cycalloc(
_check_times(sizeof(struct _tuple0),_tmp1E));struct _dynforward_ptr _tmp22=
_tag_dynforward(_tmp1F,sizeof(struct _tuple0),_tmp1E);{unsigned int _tmp20=_tmp1E;
unsigned int i;for(i=0;i < _tmp20;i ++){_tmp1F[i]=({struct _tuple0 _tmp21;_tmp21.f1=((
void**)x.curr)[(int)i];_tmp21.f2=*((void**)_check_dynforward_subscript(y,sizeof(
void*),(int)i));_tmp21;});}}_tmp22;});}struct _tuple1 Cyc_Array_split(struct
_dynforward_ptr x){int sx=(int)_get_dynforward_size(x,sizeof(struct _tuple0));
return({struct _tuple1 _tmp23;_tmp23.f1=({unsigned int _tmp28=(unsigned int)sx;void**
_tmp29=(void**)_cycalloc(_check_times(sizeof(void*),_tmp28));struct
_dynforward_ptr _tmp2B=_tag_dynforward(_tmp29,sizeof(void*),_tmp28);{unsigned int
_tmp2A=_tmp28;unsigned int i;for(i=0;i < _tmp2A;i ++){_tmp29[i]=(((struct _tuple0*)x.curr)[(
int)i]).f1;}}_tmp2B;});_tmp23.f2=({unsigned int _tmp24=(unsigned int)sx;void**
_tmp25=(void**)_cycalloc(_check_times(sizeof(void*),_tmp24));struct
_dynforward_ptr _tmp27=_tag_dynforward(_tmp25,sizeof(void*),_tmp24);{unsigned int
_tmp26=_tmp24;unsigned int i;for(i=0;i < _tmp26;i ++){_tmp25[i]=(((struct _tuple0*)x.curr)[(
int)i]).f2;}}_tmp27;});_tmp23;});}int Cyc_Array_memq(struct _dynforward_ptr l,void*
x){int s=(int)_get_dynforward_size(l,sizeof(void*));{int i=0;for(0;i < s;i ++){if(*((
void**)_check_dynforward_subscript(l,sizeof(void*),i))== x)return 1;}}return 0;}
int Cyc_Array_mem(int(*compare)(void*,void*),struct _dynforward_ptr l,void*x){int s=(
int)_get_dynforward_size(l,sizeof(void*));{int i=0;for(0;i < s;i ++){if(0 == compare(*((
void**)_check_dynforward_subscript(l,sizeof(void*),i)),x))return 1;}}return 0;}
struct _dynforward_ptr Cyc_Array_extract(struct _dynforward_ptr x,int start,int*n_opt){
int sx=(int)_get_dynforward_size(x,sizeof(void*));int n=n_opt == 0?sx - start:*n_opt;
if((start < 0  || n <= 0) || start + (n_opt == 0?0: n)> sx)(int)_throw((void*)({struct
Cyc_Core_Invalid_argument_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp2D;_tmp2D.tag=Cyc_Core_Invalid_argument;
_tmp2D.f1=({const char*_tmp2E="Array::extract";_tag_dynforward(_tmp2E,sizeof(char),
_get_zero_arr_size(_tmp2E,15));});_tmp2D;});_tmp2C;}));return({unsigned int
_tmp2F=(unsigned int)n;void**_tmp30=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp2F));struct _dynforward_ptr _tmp32=_tag_dynforward(_tmp30,sizeof(void*),_tmp2F);{
unsigned int _tmp31=_tmp2F;unsigned int i;for(i=0;i < _tmp31;i ++){_tmp30[i]=*((void**)
_check_dynforward_subscript(x,sizeof(void*),(int)(start + i)));}}_tmp32;});}
