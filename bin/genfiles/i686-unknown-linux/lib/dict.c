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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,
void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_cardinality(struct Cyc_Dict_Dict
d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);struct Cyc_Dict_Dict Cyc_Dict_insert_new(
struct Cyc_Dict_Dict d,void*k,void*v);struct Cyc_Dict_Dict Cyc_Dict_inserts(struct
Cyc_Dict_Dict d,struct Cyc_List_List*l);struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*
cmp)(void*,void*),void*k,void*v);struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct
_RegionHandle*,int(*cmp)(void*,void*),void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict
d,void*k);void**Cyc_Dict_rlookup_opt(struct _RegionHandle*,struct Cyc_Dict_Dict d,
void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);void*Cyc_Dict_fold(
void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);void*Cyc_Dict_fold_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum);
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_app_c(
void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);void Cyc_Dict_iter(
void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict d);void Cyc_Dict_iter2(void(*f)(void*,
void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);void Cyc_Dict_iter2_c(void(*f)(
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);void*
Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2,void*accum);struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct
_RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Dict_Dict d);struct
Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct
Cyc_Dict_Dict d);struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*,int(*f)(void*,void*),
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(
struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,
void*);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,
struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),
struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict);struct Cyc_Dict_Dict Cyc_Dict_unmarshal(
struct _RegionHandle*rgn,void**env,int(*cmp)(void*,void*),void*(*read_key)(void**,
struct Cyc___cycFILE*),void*(*read_val)(void**,struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp);char Cyc_Dict_Absent[11]="\000\000\000\000Absent\000";char Cyc_Dict_Present[12]="\000\000\000\000Present\000";
struct Cyc_Dict_T{void*color;struct Cyc_Dict_T*left;struct Cyc_Dict_T*right;struct
_tuple0 key_val;};struct Cyc_Dict_Dict;struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*r,int(*comp)(void*,void*)){return({struct Cyc_Dict_Dict _tmp0;_tmp0.rel=
comp;_tmp0.r=r;_tmp0.t=0;_tmp0;});}struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(
void*,void*)){return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}struct Cyc_Dict_Dict
Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*
data){return({struct Cyc_Dict_Dict _tmp1;_tmp1.rel=comp;_tmp1.r=r;_tmp1.t=({struct
Cyc_Dict_T*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->color=(void*)((void*)1);
_tmp2->left=0;_tmp2->right=0;_tmp2->key_val=({struct _tuple0 _tmp3;_tmp3.f1=key;
_tmp3.f2=data;_tmp3;});_tmp2;});_tmp1;});}struct Cyc_Dict_Dict Cyc_Dict_singleton(
int(*comp)(void*,void*),void*key,void*data){return Cyc_Dict_rsingleton(Cyc_Core_heap_region,
comp,key,data);}struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct
Cyc_Dict_Dict d){return({struct Cyc_Dict_Dict _tmp4;_tmp4.rel=d.rel;_tmp4.r=r;_tmp4.t=(
struct Cyc_Dict_T*)d.t;_tmp4;});}int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){
return d.t == 0;}int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){int(*_tmp5)(
void*,void*)=d.rel;struct Cyc_Dict_T*_tmp6=d.t;while(_tmp6 != 0){int _tmp7=_tmp5(
key,(_tmp6->key_val).f1);if(_tmp7 < 0)_tmp6=_tmp6->left;else{if(_tmp7 > 0)_tmp6=
_tmp6->right;else{return 1;}}}return 0;}void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*key){int(*_tmp8)(void*,void*)=d.rel;struct Cyc_Dict_T*_tmp9=d.t;while(_tmp9
!= 0){int _tmpA=_tmp8(key,(_tmp9->key_val).f1);if(_tmpA < 0)_tmp9=_tmp9->left;
else{if(_tmpA > 0)_tmp9=_tmp9->right;else{return(_tmp9->key_val).f2;}}}(int)
_throw((void*)Cyc_Dict_Absent);}struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict
d,void*key){int(*_tmpB)(void*,void*)=d.rel;struct Cyc_Dict_T*_tmpC=d.t;while(
_tmpC != 0){int _tmpD=_tmpB(key,(_tmpC->key_val).f1);if(_tmpD < 0)_tmpC=_tmpC->left;
else{if(_tmpD > 0)_tmpC=_tmpC->right;else{return({struct Cyc_Core_Opt*_tmpE=
_cycalloc(sizeof(*_tmpE));_tmpE->v=(void*)(_tmpC->key_val).f2;_tmpE;});}}}return
0;}void**Cyc_Dict_rlookup_opt(struct _RegionHandle*r,struct Cyc_Dict_Dict d,void*
key){int(*_tmpF)(void*,void*)=d.rel;struct Cyc_Dict_T*_tmp10=d.t;while(_tmp10 != 0){
int _tmp11=_tmpF(key,(_tmp10->key_val).f1);if(_tmp11 < 0)_tmp10=_tmp10->left;else{
if(_tmp11 > 0)_tmp10=_tmp10->right;else{return({void**_tmp12=_region_malloc(r,
sizeof(*_tmp12));_tmp12[0]=(_tmp10->key_val).f2;_tmp12;});}}}return 0;}int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict d,void*key,void**ans_place){int(*_tmp13)(void*,void*)=d.rel;
struct Cyc_Dict_T*_tmp14=d.t;while(_tmp14 != 0){int _tmp15=_tmp13(key,(_tmp14->key_val).f1);
if(_tmp15 < 0)_tmp14=_tmp14->left;else{if(_tmp15 > 0)_tmp14=_tmp14->right;else{*
ans_place=(_tmp14->key_val).f2;return 1;}}}return 0;}struct _tuple1{void*f1;struct
Cyc_Dict_T*f2;struct Cyc_Dict_T*f3;struct _tuple0 f4;};static struct Cyc_Dict_T*Cyc_Dict_balance(
struct _RegionHandle*r,struct _tuple1 quad){struct _tuple1 _tmp16=quad;void*_tmp17;
struct Cyc_Dict_T*_tmp18;struct Cyc_Dict_T _tmp19;void*_tmp1A;struct Cyc_Dict_T*
_tmp1B;struct Cyc_Dict_T _tmp1C;void*_tmp1D;struct Cyc_Dict_T*_tmp1E;struct Cyc_Dict_T*
_tmp1F;struct _tuple0 _tmp20;struct Cyc_Dict_T*_tmp21;struct _tuple0 _tmp22;struct Cyc_Dict_T*
_tmp23;struct _tuple0 _tmp24;void*_tmp25;struct Cyc_Dict_T*_tmp26;struct Cyc_Dict_T
_tmp27;void*_tmp28;struct Cyc_Dict_T*_tmp29;struct Cyc_Dict_T*_tmp2A;struct Cyc_Dict_T
_tmp2B;void*_tmp2C;struct Cyc_Dict_T*_tmp2D;struct Cyc_Dict_T*_tmp2E;struct _tuple0
_tmp2F;struct _tuple0 _tmp30;struct Cyc_Dict_T*_tmp31;struct _tuple0 _tmp32;void*
_tmp33;struct Cyc_Dict_T*_tmp34;struct Cyc_Dict_T*_tmp35;struct Cyc_Dict_T _tmp36;
void*_tmp37;struct Cyc_Dict_T*_tmp38;struct Cyc_Dict_T _tmp39;void*_tmp3A;struct Cyc_Dict_T*
_tmp3B;struct Cyc_Dict_T*_tmp3C;struct _tuple0 _tmp3D;struct Cyc_Dict_T*_tmp3E;
struct _tuple0 _tmp3F;struct _tuple0 _tmp40;void*_tmp41;struct Cyc_Dict_T*_tmp42;
struct Cyc_Dict_T*_tmp43;struct Cyc_Dict_T _tmp44;void*_tmp45;struct Cyc_Dict_T*
_tmp46;struct Cyc_Dict_T*_tmp47;struct Cyc_Dict_T _tmp48;void*_tmp49;struct Cyc_Dict_T*
_tmp4A;struct Cyc_Dict_T*_tmp4B;struct _tuple0 _tmp4C;struct _tuple0 _tmp4D;struct
_tuple0 _tmp4E;void*_tmp4F;struct Cyc_Dict_T*_tmp50;struct Cyc_Dict_T*_tmp51;struct
_tuple0 _tmp52;_LL1: _tmp17=_tmp16.f1;if((int)_tmp17 != 1)goto _LL3;_tmp18=_tmp16.f2;
if(_tmp18 == 0)goto _LL3;_tmp19=*_tmp18;_tmp1A=(void*)_tmp19.color;if((int)_tmp1A
!= 0)goto _LL3;_tmp1B=_tmp19.left;if(_tmp1B == 0)goto _LL3;_tmp1C=*_tmp1B;_tmp1D=(
void*)_tmp1C.color;if((int)_tmp1D != 0)goto _LL3;_tmp1E=_tmp1C.left;_tmp1F=_tmp1C.right;
_tmp20=_tmp1C.key_val;_tmp21=_tmp19.right;_tmp22=_tmp19.key_val;_tmp23=_tmp16.f3;
_tmp24=_tmp16.f4;_LL2: return({struct Cyc_Dict_T*_tmp53=_region_malloc(r,sizeof(*
_tmp53));_tmp53->color=(void*)((void*)0);_tmp53->left=({struct Cyc_Dict_T*_tmp55=
_region_malloc(r,sizeof(*_tmp55));_tmp55->color=(void*)((void*)1);_tmp55->left=
_tmp1E;_tmp55->right=_tmp1F;_tmp55->key_val=_tmp20;_tmp55;});_tmp53->right=({
struct Cyc_Dict_T*_tmp54=_region_malloc(r,sizeof(*_tmp54));_tmp54->color=(void*)((
void*)1);_tmp54->left=_tmp21;_tmp54->right=_tmp23;_tmp54->key_val=_tmp24;_tmp54;});
_tmp53->key_val=_tmp22;_tmp53;});_LL3: _tmp25=_tmp16.f1;if((int)_tmp25 != 1)goto
_LL5;_tmp26=_tmp16.f2;if(_tmp26 == 0)goto _LL5;_tmp27=*_tmp26;_tmp28=(void*)_tmp27.color;
if((int)_tmp28 != 0)goto _LL5;_tmp29=_tmp27.left;_tmp2A=_tmp27.right;if(_tmp2A == 0)
goto _LL5;_tmp2B=*_tmp2A;_tmp2C=(void*)_tmp2B.color;if((int)_tmp2C != 0)goto _LL5;
_tmp2D=_tmp2B.left;_tmp2E=_tmp2B.right;_tmp2F=_tmp2B.key_val;_tmp30=_tmp27.key_val;
_tmp31=_tmp16.f3;_tmp32=_tmp16.f4;_LL4: return({struct Cyc_Dict_T*_tmp56=
_region_malloc(r,sizeof(*_tmp56));_tmp56->color=(void*)((void*)0);_tmp56->left=({
struct Cyc_Dict_T*_tmp58=_region_malloc(r,sizeof(*_tmp58));_tmp58->color=(void*)((
void*)1);_tmp58->left=_tmp29;_tmp58->right=_tmp2D;_tmp58->key_val=_tmp30;_tmp58;});
_tmp56->right=({struct Cyc_Dict_T*_tmp57=_region_malloc(r,sizeof(*_tmp57));_tmp57->color=(
void*)((void*)1);_tmp57->left=_tmp2E;_tmp57->right=_tmp31;_tmp57->key_val=_tmp32;
_tmp57;});_tmp56->key_val=_tmp2F;_tmp56;});_LL5: _tmp33=_tmp16.f1;if((int)_tmp33
!= 1)goto _LL7;_tmp34=_tmp16.f2;_tmp35=_tmp16.f3;if(_tmp35 == 0)goto _LL7;_tmp36=*
_tmp35;_tmp37=(void*)_tmp36.color;if((int)_tmp37 != 0)goto _LL7;_tmp38=_tmp36.left;
if(_tmp38 == 0)goto _LL7;_tmp39=*_tmp38;_tmp3A=(void*)_tmp39.color;if((int)_tmp3A
!= 0)goto _LL7;_tmp3B=_tmp39.left;_tmp3C=_tmp39.right;_tmp3D=_tmp39.key_val;
_tmp3E=_tmp36.right;_tmp3F=_tmp36.key_val;_tmp40=_tmp16.f4;_LL6: return({struct
Cyc_Dict_T*_tmp59=_region_malloc(r,sizeof(*_tmp59));_tmp59->color=(void*)((void*)
0);_tmp59->left=({struct Cyc_Dict_T*_tmp5B=_region_malloc(r,sizeof(*_tmp5B));
_tmp5B->color=(void*)((void*)1);_tmp5B->left=_tmp34;_tmp5B->right=_tmp3B;_tmp5B->key_val=
_tmp40;_tmp5B;});_tmp59->right=({struct Cyc_Dict_T*_tmp5A=_region_malloc(r,
sizeof(*_tmp5A));_tmp5A->color=(void*)((void*)1);_tmp5A->left=_tmp3C;_tmp5A->right=
_tmp3E;_tmp5A->key_val=_tmp3F;_tmp5A;});_tmp59->key_val=_tmp3D;_tmp59;});_LL7:
_tmp41=_tmp16.f1;if((int)_tmp41 != 1)goto _LL9;_tmp42=_tmp16.f2;_tmp43=_tmp16.f3;
if(_tmp43 == 0)goto _LL9;_tmp44=*_tmp43;_tmp45=(void*)_tmp44.color;if((int)_tmp45
!= 0)goto _LL9;_tmp46=_tmp44.left;_tmp47=_tmp44.right;if(_tmp47 == 0)goto _LL9;
_tmp48=*_tmp47;_tmp49=(void*)_tmp48.color;if((int)_tmp49 != 0)goto _LL9;_tmp4A=
_tmp48.left;_tmp4B=_tmp48.right;_tmp4C=_tmp48.key_val;_tmp4D=_tmp44.key_val;
_tmp4E=_tmp16.f4;_LL8: return({struct Cyc_Dict_T*_tmp5C=_region_malloc(r,sizeof(*
_tmp5C));_tmp5C->color=(void*)((void*)0);_tmp5C->left=({struct Cyc_Dict_T*_tmp5E=
_region_malloc(r,sizeof(*_tmp5E));_tmp5E->color=(void*)((void*)1);_tmp5E->left=
_tmp42;_tmp5E->right=_tmp46;_tmp5E->key_val=_tmp4E;_tmp5E;});_tmp5C->right=({
struct Cyc_Dict_T*_tmp5D=_region_malloc(r,sizeof(*_tmp5D));_tmp5D->color=(void*)((
void*)1);_tmp5D->left=_tmp4A;_tmp5D->right=_tmp4B;_tmp5D->key_val=_tmp4C;_tmp5D;});
_tmp5C->key_val=_tmp4D;_tmp5C;});_LL9: _tmp4F=_tmp16.f1;_tmp50=_tmp16.f2;_tmp51=
_tmp16.f3;_tmp52=_tmp16.f4;_LLA: return({struct Cyc_Dict_T*_tmp5F=_region_malloc(r,
sizeof(*_tmp5F));_tmp5F->color=(void*)_tmp4F;_tmp5F->left=_tmp50;_tmp5F->right=
_tmp51;_tmp5F->key_val=_tmp52;_tmp5F;});_LL0:;}static struct Cyc_Dict_T*Cyc_Dict_ins(
struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,struct Cyc_Dict_T*
t){struct Cyc_Dict_T*_tmp60=t;struct Cyc_Dict_T _tmp61;void*_tmp62;struct Cyc_Dict_T*
_tmp63;struct Cyc_Dict_T*_tmp64;struct _tuple0 _tmp65;_LLC: if(_tmp60 != 0)goto _LLE;
_LLD: return({struct Cyc_Dict_T*_tmp66=_region_malloc(r,sizeof(*_tmp66));_tmp66->color=(
void*)((void*)0);_tmp66->left=0;_tmp66->right=0;_tmp66->key_val=key_val;_tmp66;});
_LLE: if(_tmp60 == 0)goto _LLB;_tmp61=*_tmp60;_tmp62=(void*)_tmp61.color;_tmp63=
_tmp61.left;_tmp64=_tmp61.right;_tmp65=_tmp61.key_val;_LLF: {int _tmp67=rel(
key_val.f1,_tmp65.f1);if(_tmp67 < 0)return Cyc_Dict_balance(r,({struct _tuple1
_tmp68;_tmp68.f1=_tmp62;_tmp68.f2=Cyc_Dict_ins(r,rel,key_val,_tmp63);_tmp68.f3=
_tmp64;_tmp68.f4=_tmp65;_tmp68;}));else{if(_tmp67 > 0)return Cyc_Dict_balance(r,({
struct _tuple1 _tmp69;_tmp69.f1=_tmp62;_tmp69.f2=_tmp63;_tmp69.f3=Cyc_Dict_ins(r,
rel,key_val,_tmp64);_tmp69.f4=_tmp65;_tmp69;}));else{return({struct Cyc_Dict_T*
_tmp6A=_region_malloc(r,sizeof(*_tmp6A));_tmp6A->color=(void*)_tmp62;_tmp6A->left=
_tmp63;_tmp6A->right=_tmp64;_tmp6A->key_val=key_val;_tmp6A;});}}}_LLB:;}struct
Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){struct Cyc_Dict_T*
_tmp6B=Cyc_Dict_ins(d.r,d.rel,({struct _tuple0 _tmp6E;_tmp6E.f1=key;_tmp6E.f2=data;
_tmp6E;}),d.t);(void*)(((struct Cyc_Dict_T*)_check_null(_tmp6B))->color=(void*)((
void*)1));{struct Cyc_Dict_Dict _tmp6C=({struct Cyc_Dict_Dict _tmp6D;_tmp6D.rel=d.rel;
_tmp6D.r=d.r;_tmp6D.t=_tmp6B;_tmp6D;});return _tmp6C;}}struct Cyc_Dict_Dict Cyc_Dict_insert_new(
struct Cyc_Dict_Dict d,void*key,void*data){if(Cyc_Dict_member(d,key))(int)_throw((
void*)Cyc_Dict_Absent);return Cyc_Dict_insert(d,key,data);}struct Cyc_Dict_Dict Cyc_Dict_inserts(
struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_Dict_insert(
d,(((struct _tuple0*)kds->hd)[_check_known_subscript_notnull(1,0)]).f1,(((struct
_tuple0*)kds->hd)[_check_known_subscript_notnull(1,0)]).f2);}return d;}static void*
Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),struct Cyc_Dict_T*t,void*accum){
struct Cyc_Dict_T _tmp70;struct Cyc_Dict_T*_tmp71;struct Cyc_Dict_T*_tmp72;struct
_tuple0 _tmp73;void*_tmp74;void*_tmp75;struct Cyc_Dict_T*_tmp6F=t;_tmp70=*_tmp6F;
_tmp71=_tmp70.left;_tmp72=_tmp70.right;_tmp73=_tmp70.key_val;_tmp74=_tmp73.f1;
_tmp75=_tmp73.f2;if(_tmp71 != 0)accum=Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)
_tmp71,accum);accum=f(_tmp74,_tmp75,accum);if(_tmp72 != 0)accum=Cyc_Dict_fold_tree(
f,(struct Cyc_Dict_T*)_tmp72,accum);return accum;}void*Cyc_Dict_fold(void*(*f)(
void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){if(d.t == 0)return accum;
return Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)d.t,accum);}static void*Cyc_Dict_fold_tree_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_T*t,void*accum){
struct Cyc_Dict_T _tmp77;struct Cyc_Dict_T*_tmp78;struct Cyc_Dict_T*_tmp79;struct
_tuple0 _tmp7A;void*_tmp7B;void*_tmp7C;struct Cyc_Dict_T*_tmp76=t;_tmp77=*_tmp76;
_tmp78=_tmp77.left;_tmp79=_tmp77.right;_tmp7A=_tmp77.key_val;_tmp7B=_tmp7A.f1;
_tmp7C=_tmp7A.f2;if(_tmp78 != 0)accum=Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)
_tmp78,accum);accum=f(env,_tmp7B,_tmp7C,accum);if(_tmp79 != 0)accum=Cyc_Dict_fold_tree_c(
f,env,(struct Cyc_Dict_T*)_tmp79,accum);return accum;}void*Cyc_Dict_fold_c(void*(*
f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){if(d.t == 0)
return accum;return Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)d.t,accum);}
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T
_tmp7E;struct Cyc_Dict_T*_tmp7F;struct Cyc_Dict_T*_tmp80;struct _tuple0 _tmp81;void*
_tmp82;void*_tmp83;struct Cyc_Dict_T*_tmp7D=t;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.left;
_tmp80=_tmp7E.right;_tmp81=_tmp7E.key_val;_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;if(
_tmp7F != 0)Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)_tmp7F);f(_tmp82,_tmp83);if(
_tmp80 != 0)Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)_tmp80);}void Cyc_Dict_app(void*(*
f)(void*,void*),struct Cyc_Dict_Dict d){if(d.t != 0)Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)
d.t);}static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,struct
Cyc_Dict_T*t){struct Cyc_Dict_T _tmp85;struct Cyc_Dict_T*_tmp86;struct Cyc_Dict_T*
_tmp87;struct _tuple0 _tmp88;void*_tmp89;void*_tmp8A;struct Cyc_Dict_T*_tmp84=t;
_tmp85=*_tmp84;_tmp86=_tmp85.left;_tmp87=_tmp85.right;_tmp88=_tmp85.key_val;
_tmp89=_tmp88.f1;_tmp8A=_tmp88.f2;if(_tmp86 != 0)Cyc_Dict_app_tree_c(f,env,(
struct Cyc_Dict_T*)_tmp86);f(env,_tmp89,_tmp8A);if(_tmp87 != 0)Cyc_Dict_app_tree_c(
f,env,(struct Cyc_Dict_T*)_tmp87);}void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d){if(d.t != 0)Cyc_Dict_app_tree_c(f,env,(struct Cyc_Dict_T*)
d.t);}static void Cyc_Dict_iter_tree(void(*f)(void*,void*),struct Cyc_Dict_T*t){
struct Cyc_Dict_T _tmp8C;struct Cyc_Dict_T*_tmp8D;struct Cyc_Dict_T*_tmp8E;struct
_tuple0 _tmp8F;void*_tmp90;void*_tmp91;struct Cyc_Dict_T*_tmp8B=t;_tmp8C=*_tmp8B;
_tmp8D=_tmp8C.left;_tmp8E=_tmp8C.right;_tmp8F=_tmp8C.key_val;_tmp90=_tmp8F.f1;
_tmp91=_tmp8F.f2;if(_tmp8D != 0)Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)_tmp8D);f(
_tmp90,_tmp91);if(_tmp8E != 0)Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)_tmp8E);}
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){if(d.t != 0)Cyc_Dict_iter_tree(
f,(struct Cyc_Dict_T*)d.t);}static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp93;struct Cyc_Dict_T*
_tmp94;struct Cyc_Dict_T*_tmp95;struct _tuple0 _tmp96;void*_tmp97;void*_tmp98;
struct Cyc_Dict_T*_tmp92=t;_tmp93=*_tmp92;_tmp94=_tmp93.left;_tmp95=_tmp93.right;
_tmp96=_tmp93.key_val;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;if(_tmp94 != 0)Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_tmp94);f(env,_tmp97,_tmp98);if(_tmp95 != 0)Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_tmp95);}void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d){if(d.t != 0)Cyc_Dict_iter_tree_c(f,env,(struct Cyc_Dict_T*)
d.t);}static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){*cnt=*cnt + 1;}int Cyc_Dict_cardinality(
struct Cyc_Dict_Dict d){int num=0;((void(*)(void(*f)(int*,void*,void*),int*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_count_elem,& num,d);return num;}
struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};static void Cyc_Dict_iter2_f(
struct _tuple2*env,void*a,void*b1){struct _tuple2 _tmp9A;void(*_tmp9B)(void*,void*);
struct Cyc_Dict_Dict _tmp9C;struct _tuple2*_tmp99=env;_tmp9A=*_tmp99;_tmp9B=_tmp9A.f1;
_tmp9C=_tmp9A.f2;_tmp9B(b1,Cyc_Dict_lookup(_tmp9C,a));}void Cyc_Dict_iter2(void(*
f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct _tuple2 _tmp9D=({
struct _tuple2 _tmp9E;_tmp9E.f1=f;_tmp9E.f2=d2;_tmp9E;});((void(*)(void(*f)(struct
_tuple2*,void*,void*),struct _tuple2*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Dict_iter2_f,& _tmp9D,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct
Cyc_Dict_Dict f2;void*f3;};static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,
void*b1){struct _tuple3 _tmpA0;void(*_tmpA1)(void*,void*,void*);struct Cyc_Dict_Dict
_tmpA2;void*_tmpA3;struct _tuple3*_tmp9F=env;_tmpA0=*_tmp9F;_tmpA1=_tmpA0.f1;
_tmpA2=_tmpA0.f2;_tmpA3=_tmpA0.f3;_tmpA1(_tmpA3,b1,Cyc_Dict_lookup(_tmpA2,a));}
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2){struct _tuple3 _tmpA4=({struct _tuple3 _tmpA5;_tmpA5.f1=f;
_tmpA5.f2=d2;_tmpA5.f3=inner_env;_tmpA5;});((void(*)(void(*f)(struct _tuple3*,
void*,void*),struct _tuple3*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,&
_tmpA4,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict
f2;void*f3;};static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*
accum){struct _tuple4 _tmpA7;void*(*_tmpA8)(void*,void*,void*,void*,void*);struct
Cyc_Dict_Dict _tmpA9;void*_tmpAA;struct _tuple4*_tmpA6=env;_tmpA7=*_tmpA6;_tmpA8=
_tmpA7.f1;_tmpA9=_tmpA7.f2;_tmpAA=_tmpA7.f3;return _tmpA8(_tmpAA,a,b1,Cyc_Dict_lookup(
_tmpA9,a),accum);}void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),
void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){struct
_tuple4 _tmpAB=({struct _tuple4 _tmpAC;_tmpAC.f1=f;_tmpAC.f2=d2;_tmpAC.f3=inner_env;
_tmpAC;});return((void*(*)(void*(*f)(struct _tuple4*,void*,void*,void*),struct
_tuple4*env,struct Cyc_Dict_Dict d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,&
_tmpAB,d1,accum);}static struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*
r2,struct Cyc_Dict_T*t){if(t == 0)return 0;else{void*_tmpAE;struct Cyc_Dict_T*_tmpAF;
struct Cyc_Dict_T*_tmpB0;struct _tuple0 _tmpB1;struct Cyc_Dict_T _tmpAD=*t;_tmpAE=(
void*)_tmpAD.color;_tmpAF=_tmpAD.left;_tmpB0=_tmpAD.right;_tmpB1=_tmpAD.key_val;{
struct Cyc_Dict_T*_tmpB2=Cyc_Dict_copy_tree(r2,_tmpAF);struct Cyc_Dict_T*_tmpB3=
Cyc_Dict_copy_tree(r2,_tmpB0);return({struct Cyc_Dict_T*_tmpB4=_region_malloc(r2,
sizeof(*_tmpB4));_tmpB4->color=(void*)_tmpAE;_tmpB4->left=_tmpB2;_tmpB4->right=
_tmpB3;_tmpB4->key_val=_tmpB1;_tmpB4;});}}}struct Cyc_Dict_Dict Cyc_Dict_rcopy(
struct _RegionHandle*r2,struct Cyc_Dict_Dict d){return({struct Cyc_Dict_Dict _tmpB5;
_tmpB5.rel=d.rel;_tmpB5.r=r2;_tmpB5.t=Cyc_Dict_copy_tree(r2,d.t);_tmpB5;});}
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){return Cyc_Dict_rcopy(Cyc_Core_heap_region,
d);}static struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(
void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpB7;void*_tmpB8;struct Cyc_Dict_T*
_tmpB9;struct Cyc_Dict_T*_tmpBA;struct _tuple0 _tmpBB;void*_tmpBC;void*_tmpBD;
struct Cyc_Dict_T*_tmpB6=t;_tmpB7=*_tmpB6;_tmpB8=(void*)_tmpB7.color;_tmpB9=
_tmpB7.left;_tmpBA=_tmpB7.right;_tmpBB=_tmpB7.key_val;_tmpBC=_tmpBB.f1;_tmpBD=
_tmpBB.f2;{struct Cyc_Dict_T*_tmpBE=_tmpB9 == 0?0: Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)
_tmpB9);void*_tmpBF=f(_tmpBD);struct Cyc_Dict_T*_tmpC0=_tmpBA == 0?0: Cyc_Dict_map_tree(
r,f,(struct Cyc_Dict_T*)_tmpBA);return({struct Cyc_Dict_T*_tmpC1=_region_malloc(r,
sizeof(*_tmpC1));_tmpC1->color=(void*)_tmpB8;_tmpC1->left=_tmpBE;_tmpC1->right=
_tmpC0;_tmpC1->key_val=({struct _tuple0 _tmpC2;_tmpC2.f1=_tmpBC;_tmpC2.f2=_tmpBF;
_tmpC2;});_tmpC1;});}}struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,
void*(*f)(void*),struct Cyc_Dict_Dict d){if(d.t == 0)return({struct Cyc_Dict_Dict
_tmpC3;_tmpC3.rel=d.rel;_tmpC3.r=r;_tmpC3.t=0;_tmpC3;});return({struct Cyc_Dict_Dict
_tmpC4;_tmpC4.rel=d.rel;_tmpC4.r=r;_tmpC4.t=Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)
d.t);_tmpC4;});}struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict
d){return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}static struct Cyc_Dict_T*Cyc_Dict_map_tree_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_T*t){struct
Cyc_Dict_T _tmpC6;void*_tmpC7;struct Cyc_Dict_T*_tmpC8;struct Cyc_Dict_T*_tmpC9;
struct _tuple0 _tmpCA;void*_tmpCB;void*_tmpCC;struct Cyc_Dict_T*_tmpC5=t;_tmpC6=*
_tmpC5;_tmpC7=(void*)_tmpC6.color;_tmpC8=_tmpC6.left;_tmpC9=_tmpC6.right;_tmpCA=
_tmpC6.key_val;_tmpCB=_tmpCA.f1;_tmpCC=_tmpCA.f2;{struct Cyc_Dict_T*_tmpCD=_tmpC8
== 0?0: Cyc_Dict_map_tree_c(r,f,env,(struct Cyc_Dict_T*)_tmpC8);void*_tmpCE=f(env,
_tmpCC);struct Cyc_Dict_T*_tmpCF=_tmpC9 == 0?0: Cyc_Dict_map_tree_c(r,f,env,(struct
Cyc_Dict_T*)_tmpC9);return({struct Cyc_Dict_T*_tmpD0=_region_malloc(r,sizeof(*
_tmpD0));_tmpD0->color=(void*)_tmpC7;_tmpD0->left=_tmpCD;_tmpD0->right=_tmpCF;
_tmpD0->key_val=({struct _tuple0 _tmpD1;_tmpD1.f1=_tmpCB;_tmpD1.f2=_tmpCE;_tmpD1;});
_tmpD0;});}}struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(
void*,void*),void*env,struct Cyc_Dict_Dict d){if(d.t == 0)return({struct Cyc_Dict_Dict
_tmpD2;_tmpD2.rel=d.rel;_tmpD2.r=r;_tmpD2.t=0;_tmpD2;});return({struct Cyc_Dict_Dict
_tmpD3;_tmpD3.rel=d.rel;_tmpD3.r=r;_tmpD3.t=Cyc_Dict_map_tree_c(r,f,env,(struct
Cyc_Dict_T*)d.t);_tmpD3;});}struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_Dict_Dict d){return Cyc_Dict_rmap_c(Cyc_Core_heap_region,
f,env,d);}struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d){if(d.t == 0)(int)_throw((void*)Cyc_Dict_Absent);return({struct _tuple0*_tmpD4=
_region_malloc(r,sizeof(*_tmpD4));_tmpD4->f1=((d.t)->key_val).f1;_tmpD4->f2=((d.t)->key_val).f2;
_tmpD4;});}static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpD6;struct Cyc_Dict_T*_tmpD7;struct Cyc_Dict_T*
_tmpD8;struct _tuple0 _tmpD9;void*_tmpDA;void*_tmpDB;struct Cyc_Dict_T*_tmpD5=t;
_tmpD6=*_tmpD5;_tmpD7=_tmpD6.left;_tmpD8=_tmpD6.right;_tmpD9=_tmpD6.key_val;
_tmpDA=_tmpD9.f1;_tmpDB=_tmpD9.f2;return((_tmpD7 == 0  || Cyc_Dict_forall_tree_c(f,
env,(struct Cyc_Dict_T*)_tmpD7)) && f(env,_tmpDA,_tmpDB)) && (_tmpD8 == 0  || Cyc_Dict_forall_tree_c(
f,env,(struct Cyc_Dict_T*)_tmpD8));}int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d){if(d.t == 0)return 1;return Cyc_Dict_forall_tree_c(f,
env,(struct Cyc_Dict_T*)d.t);}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict
f2;};static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
struct _tuple5 _tmpDD;int(*_tmpDE)(void*,void*,void*);struct Cyc_Dict_Dict _tmpDF;
struct _tuple5*_tmpDC=env;_tmpDD=*_tmpDC;_tmpDE=_tmpDD.f1;_tmpDF=_tmpDD.f2;if(Cyc_Dict_member(
_tmpDF,a))return _tmpDE(a,b,Cyc_Dict_lookup(_tmpDF,a));return 1;}int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct
_tuple5 _tmpE0=({struct _tuple5 _tmpE1;_tmpE1.f1=f;_tmpE1.f2=d2;_tmpE1;});return((
int(*)(int(*f)(struct _tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict
d))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& _tmpE0,d1);}struct _tuple6{
void*(*f1)(void*,void*,void*,void*);void*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_union_f(
struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){if(Cyc_Dict_member(*d1,a)){
void*_tmpE2=Cyc_Dict_lookup(*d1,a);void*_tmpE3=((*env).f1)((*env).f2,a,_tmpE2,b);
if(_tmpE3 != _tmpE2)*d1=Cyc_Dict_insert(*d1,a,_tmpE3);return d1;}*d1=Cyc_Dict_insert(*
d1,a,b);return d1;}struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){if((int)d1.t
== (int)d2.t)return d1;if(d1.t == 0)return d2;if(d2.t == 0)return d1;{struct _tuple6
_tmpE4=({struct _tuple6 _tmpE5;_tmpE5.f1=f;_tmpE5.f2=env;_tmpE5;});((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*(*f)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct
_tuple6*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,&
_tmpE4,d2,& d1);return d1;}}struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){if((int)
d1.t == (int)d2.t  || d2.t == 0)return d2;{struct Cyc_Dict_T*_tmpE6=0;{struct
_RegionHandle _tmpE7=_new_region("temp");struct _RegionHandle*temp=& _tmpE7;
_push_region(temp);{struct _dynforward_ptr queue=_tag_dynforward(({unsigned int
_tmpF4=(unsigned int)16;struct Cyc_Dict_T**_tmpF5=(struct Cyc_Dict_T**)
_region_malloc(temp,_check_times(sizeof(struct Cyc_Dict_T*),_tmpF4));{
unsigned int _tmpF6=_tmpF4;unsigned int i;for(i=0;i < _tmpF6;i ++){_tmpF5[i]=(struct
Cyc_Dict_T*)d2.t;}}_tmpF5;}),sizeof(struct Cyc_Dict_T*),(unsigned int)16);int ind=
0;while(ind != - 1){struct Cyc_Dict_T _tmpE9;struct Cyc_Dict_T*_tmpEA;struct Cyc_Dict_T*
_tmpEB;struct _tuple0 _tmpEC;void*_tmpED;void*_tmpEE;struct Cyc_Dict_T*_tmpE8=*((
struct Cyc_Dict_T**)_check_dynforward_subscript(queue,sizeof(struct Cyc_Dict_T*),
ind --));_tmpE9=*_tmpE8;_tmpEA=_tmpE9.left;_tmpEB=_tmpE9.right;_tmpEC=_tmpE9.key_val;
_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(ind + 2 >= _get_dynforward_size(queue,sizeof(
struct Cyc_Dict_T*)))queue=({unsigned int _tmpEF=_get_dynforward_size(queue,
sizeof(struct Cyc_Dict_T*))* 2;struct Cyc_Dict_T**_tmpF0=(struct Cyc_Dict_T**)
_region_malloc(temp,_check_times(sizeof(struct Cyc_Dict_T*),_tmpEF));struct
_dynforward_ptr _tmpF2=_tag_dynforward(_tmpF0,sizeof(struct Cyc_Dict_T*),_tmpEF);{
unsigned int _tmpF1=_tmpEF;unsigned int i;for(i=0;i < _tmpF1;i ++){_tmpF0[i]=i < 
_get_dynforward_size(queue,sizeof(struct Cyc_Dict_T*))?((struct Cyc_Dict_T**)queue.curr)[(
int)i]:(struct Cyc_Dict_T*)d2.t;}}_tmpF2;});if(_tmpEA != 0)*((struct Cyc_Dict_T**)
_check_dynforward_subscript(queue,sizeof(struct Cyc_Dict_T*),++ ind))=(struct Cyc_Dict_T*)
_tmpEA;if(_tmpEB != 0)*((struct Cyc_Dict_T**)_check_dynforward_subscript(queue,
sizeof(struct Cyc_Dict_T*),++ ind))=(struct Cyc_Dict_T*)_tmpEB;if(Cyc_Dict_member(
d1,_tmpED))_tmpE6=Cyc_Dict_ins(d2.r,d2.rel,({struct _tuple0 _tmpF3;_tmpF3.f1=
_tmpED;_tmpF3.f2=f(env,_tmpED,Cyc_Dict_lookup(d1,_tmpED),_tmpEE);_tmpF3;}),
_tmpE6);}};_pop_region(temp);}return({struct Cyc_Dict_Dict _tmpF7;_tmpF7.rel=d2.rel;
_tmpF7.r=d2.r;_tmpF7.t=_tmpE6;_tmpF7;});}}static void*Cyc_Dict_intersect_f(void*(*
f)(void*,void*,void*),void*a,void*b1,void*b2){return f(a,b1,b2);}struct Cyc_Dict_Dict
Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){return((struct Cyc_Dict_Dict(*)(void*(*f)(void*(*)(void*,void*,void*),void*,
void*,void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}static struct Cyc_List_List*
Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_tmpF8=_region_malloc(r,sizeof(*_tmpF8));_tmpF8->hd=({
struct _tuple0*_tmpF9=_region_malloc(r,sizeof(*_tmpF9));_tmpF9->f1=k;_tmpF9->f2=v;
_tmpF9;});_tmpF8->tl=accum;_tmpF8;});}struct Cyc_List_List*Cyc_Dict_rto_list(
struct _RegionHandle*r,struct Cyc_Dict_Dict d){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),
struct _RegionHandle*env,struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(
Cyc_Dict_to_list_f,r,d,0);}struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict
d){return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,
void*);struct _RegionHandle*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(
struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple7 _tmpFB;int(*
_tmpFC)(void*,void*);struct _RegionHandle*_tmpFD;struct _tuple7*_tmpFA=env;_tmpFB=*
_tmpFA;_tmpFC=_tmpFB.f1;_tmpFD=_tmpFB.f2;if(_tmpFC(x,y))*acc=Cyc_Dict_insert(*
acc,x,y);return acc;}struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,
int(*f)(void*,void*),struct Cyc_Dict_Dict d){struct _tuple7 _tmpFE=({struct _tuple7
_tmp100;_tmp100.f1=f;_tmp100.f2=r2;_tmp100;});struct Cyc_Dict_Dict _tmpFF=Cyc_Dict_rempty(
r2,d.rel);return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct
_tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& _tmpFE,d,&
_tmpFF);}struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict
d){return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,
void*,void*);void*f2;struct _RegionHandle*f3;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(
struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple8 _tmp102;
int(*_tmp103)(void*,void*,void*);void*_tmp104;struct _RegionHandle*_tmp105;struct
_tuple8*_tmp101=env;_tmp102=*_tmp101;_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;
_tmp105=_tmp102.f3;if(_tmp103(_tmp104,x,y))*acc=Cyc_Dict_insert(*acc,x,y);return
acc;}struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,
void*,void*),void*f_env,struct Cyc_Dict_Dict d){struct _tuple8 _tmp106=({struct
_tuple8 _tmp108;_tmp108.f1=f;_tmp108.f2=f_env;_tmp108.f3=r2;_tmp108;});struct Cyc_Dict_Dict
_tmp107=Cyc_Dict_rempty(r2,d.rel);return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*
f)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmp106,d,&
_tmp107);}struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*
f_env,struct Cyc_Dict_Dict d){return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,
f_env,d);}static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(*d,x);}struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct
_RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){return((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*
f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,& d2,d1);}
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*
f1)(void*,void*);void*f2;};static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,
void*y){struct _tuple9 _tmp10A;int(*_tmp10B)(void*,void*);void*_tmp10C;struct
_tuple9*_tmp109=env;_tmp10A=*_tmp109;_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;
return _tmp10B(_tmp10C,x)!= 0;}struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(
r2,d);{struct _tuple9 _tmp10D=({struct _tuple9 _tmp10E;_tmp10E.f1=d.rel;_tmp10E.f2=x;
_tmp10E;});return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct
_tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
r2,Cyc_Dict_delete_f,& _tmp10D,d);}}struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict d,void*x){if(!Cyc_Dict_member(d,x))return d;{struct _tuple9
_tmp10F=({struct _tuple9 _tmp110;_tmp110.f1=d.rel;_tmp110.f2=x;_tmp110;});return((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),
struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(d.r,Cyc_Dict_delete_f,&
_tmp10F,d);}}struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct
_dynforward_ptr f1;int f2;};int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*
dest){struct _tuple10 _tmp112;struct _dynforward_ptr _tmp113;int _tmp114;int*_tmp115;
struct _tuple10*_tmp111=stk;_tmp112=*_tmp111;_tmp113=_tmp112.f1;_tmp114=_tmp112.f2;
_tmp115=(int*)&(*_tmp111).f2;{int _tmp116=*_tmp115;if(_tmp116 == - 1)return 0;{
struct Cyc_Dict_T*_tmp117=*((struct Cyc_Dict_T**)_check_dynforward_subscript(
_tmp113,sizeof(struct Cyc_Dict_T*),_tmp116));*dest=((struct Cyc_Dict_T*)
_check_null(_tmp117))->key_val;-- _tmp116;if((unsigned int)_tmp117->left)*((
struct Cyc_Dict_T**)_check_dynforward_subscript(_tmp113,sizeof(struct Cyc_Dict_T*),
++ _tmp116))=_tmp117->left;if((unsigned int)_tmp117->right)*((struct Cyc_Dict_T**)
_check_dynforward_subscript(_tmp113,sizeof(struct Cyc_Dict_T*),++ _tmp116))=
_tmp117->right;*_tmp115=_tmp116;return 1;}}}struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){int half_max_size=1;struct Cyc_Dict_T*
_tmp118=d.t;while(_tmp118 != 0){_tmp118=_tmp118->left;++ half_max_size;}_tmp118=d.t;{
struct _dynforward_ptr _tmp119=({unsigned int _tmp11C=(unsigned int)(2 * 
half_max_size);struct Cyc_Dict_T**_tmp11D=(struct Cyc_Dict_T**)_region_malloc(rgn,
_check_times(sizeof(struct Cyc_Dict_T*),_tmp11C));struct _dynforward_ptr _tmp11F=
_tag_dynforward(_tmp11D,sizeof(struct Cyc_Dict_T*),_tmp11C);{unsigned int _tmp11E=
_tmp11C;unsigned int i;for(i=0;i < _tmp11E;i ++){_tmp11D[i]=_tmp118;}}_tmp11F;});
return({struct Cyc_Iter_Iter _tmp11A;_tmp11A.env=(void*)({struct _tuple10*_tmp11B=
_region_malloc(rgn,sizeof(*_tmp11B));_tmp11B->f1=_tmp119;_tmp11B->f2=(
unsigned int)_tmp118?0: - 1;_tmp11B;});_tmp11A.next=(int(*)(void*env,void*dest))
Cyc_Dict_iter_f;_tmp11A;});}}void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*
env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,
struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);int len=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dict_list);(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Core_Failure_struct
_tmp121;_tmp121.tag=Cyc_Core_Failure;_tmp121.f1=({const char*_tmp122="Dict::marshal: Write failure";
_tag_dynforward(_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,29));});_tmp121;});
_tmp120;}));while(dict_list != 0){env=((void*(*)(void*,struct Cyc___cycFILE*,
struct _tuple0*))write_key)(env,fp,(struct _tuple0*)dict_list->hd);env=((void*(*)(
void*,struct Cyc___cycFILE*,struct _tuple0*))write_val)(env,fp,(struct _tuple0*)
dict_list->hd);dict_list=dict_list->tl;}return env;}struct Cyc_Dict_Dict Cyc_Dict_unmarshal(
struct _RegionHandle*rgn,void**env,int(*cmp)(void*,void*),void*(*read_key)(void**,
struct Cyc___cycFILE*),void*(*read_val)(void**,struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp){struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);int len=Cyc_getw(fp);if(len == - 1)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp123=_cycalloc(sizeof(*
_tmp123));_tmp123[0]=({struct Cyc_Core_Failure_struct _tmp124;_tmp124.tag=Cyc_Core_Failure;
_tmp124.f1=({const char*_tmp125="Dict::unmarshal: list length is -1";
_tag_dynforward(_tmp125,sizeof(char),_get_zero_arr_size(_tmp125,35));});_tmp124;});
_tmp123;}));{int i=0;for(0;i < len;i ++){void*key=read_key(env,fp);void*val=
read_val(env,fp);dict=Cyc_Dict_insert(dict,key,val);}}return dict;}
